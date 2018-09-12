#include "cc430uart.h"
#include "pins.h"

uint8_t uart_rxBuffer[SERIAL_BUFFER_SIZE];
uint8_t uart_rxLength = 0;
uint8_t uart_rxIndex = 0;

uint8_t uart_txBuffer[SERIAL_BUFFER_SIZE];
volatile uint8_t uart_tx_buffer_tail = 0;
uint8_t uart_tx_buffer_head = 0;

/**
 * begin
 * 
 * Initialize UART port
 *
 * @param baud Serial baudrate
 * @param config additional UART configuration
 */
void uart_begin(uint32_t baud, uint8_t config) {
	uint16_t ucbrf, divider;
    uint32_t clkfreq = __F_CPU;

	UCA0CTL1 = UCSWRST;
	UCA0CTL1 |= UCSSEL_2;  // SMCLK
	UCA0CTL0 = config;
	UCA0ABCTL = 0;

    divider = (clkfreq >> 4)/baud;
    ucbrf = (clkfreq % (baud << 4)) / baud;

    UCA0BR0 = divider;                   // Integer part of UART frequency scaler (low byte)
    UCA0BR1 = divider>>8;                // Integer part of UART frequency scaler (high byte)
    UCA0MCTL = ucbrf + UCBRS_0 + UCOS16; // This turns on oversampling and sets the decimal part of the scaler

	UCA0CTL1 &= ~UCSWRST;
	UCA0IE |= UCRXIE;                    // Enable Rx interrupt
}


/**
 * write
 * 
 * Send single byte
 *
 * @param c Character to be transmitted
 *
 * @return Amount of bytes transmitted
 */
uint8_t uart_write_byte(const uint8_t c) {

    // If the buffer and the data register are empty, just write the byte
    // to the data register and be done.
    if ((uart_tx_buffer_head == uart_tx_buffer_tail) && (UCA0IFG & UCTXIFG)) {
        UCA0TXBUF = c;
        return 1;
    }

    uint8_t i = (uart_tx_buffer_head + 1) % SERIAL_BUFFER_SIZE;

    // If the output buffer is full, then wait for the interrupt
    // handler to drain it of a character.
    while (i == uart_tx_buffer_tail);

    uart_txBuffer[uart_tx_buffer_head] = c;
    uart_tx_buffer_head = i;

    // Turn on interrupts, since there's a character in the buffer
    UCA0IE |= UCTXIE;

//    while(uart_tx_buffer_head != uart_tx_buffer_tail) {}
    return 1;
}

/**
 * write
 * 
 * Send data buffer
 *
 * @param buf Buffer to be sent
 * @param len Amount of bytes to be transferred
 *
 * @return Amount of bytes transmitted
 */
uint16_t uart_write(const uint8_t *buf, uint16_t len) {
  uint16_t i;

  // Send bytes
  for(i=0 ; i<len ; i++)
      uart_write_byte(buf[i]);

  return len;
}

void uart_end(void) {
  // Wait for transmission of outgoing data
  while (uart_tx_buffer_head != uart_tx_buffer_tail);

  while (UCA0STAT & UCBUSY);  // Wait whilst the UART is busy
  UCA0IE &= ~UCRXIE;          // Disable Rx interrupt
  UCA0IE &= ~UCTXIE;          // Disable Tx interrupt
  UCA0CTL1 |= UCSWRST;        // Reset UART
}
/**
 * read
 * 
 * Read single byte from Rx FIFO
 *
 * @return byte read
 */
int uart_read_byte(void) {

  if (uart_rxLength == 0) return -1;
  
  if (uart_rxIndex >= uart_rxLength) return -1;
  
  uint8_t b = uart_rxBuffer[uart_rxIndex++];

  if (uart_rxIndex == uart_rxLength) {
    uart_rxLength = 0;
    uart_rxIndex = 0;
  }
  
  return (int)b;
}

/**
 * read
 * 
 * Read string from the Rx FIFO
 *
 * @param buf Buffer to be sent
 *
 * @return amount of bytes read or 0 if the Rx FIFO is empty
 */
int uart_read(uint8_t *buf) {

  uint8_t i;
  
  for(i = 0; i < uart_rxLength ; i++) buf[i] = uart_rxBuffer[i];
  
  uart_rxLength = 0;
  uart_rxIndex = 0;
  
  return i;
}

/**
 * available
 * 
 * Get he amount of bytes waiting to be read from the
 * UART Rx FIFO
 *
 * @return true if there is at least one byte in the Rx FIFO
 */
int uart_available(void)
{
  if (uart_rxLength > 0) return 1;
  else return 0;
}

/**
 * uartISR
 *
 * UART interrupt routine
 */
__attribute__((interrupt(USCI_A0_VECTOR)))
void uartISR(void)
{
#ifdef DEBUG_UART
   P3OUT ^= BIT7;
#endif
   switch (UCA0IV) 
  { 
   case USCI_UCRXIFG:
      while (!(UCA0IFG&UCTXIFG));           // USCI_A0 TX buffer ready?
      uart_rxBuffer[uart_rxLength++] = UCA0RXBUF;
    P3OUT ^= BIT7;
       break;
    case USCI_UCTXIFG:
      UCA0TXBUF = uart_txBuffer[uart_tx_buffer_tail];

      uart_tx_buffer_tail = (uart_tx_buffer_tail + 1) % SERIAL_BUFFER_SIZE;
      if (uart_tx_buffer_head == uart_tx_buffer_tail)
      {
        // Buffer empty, so disable interrupts
        UCA0IE &= ~UCTXIE;
      }
#ifdef DEBUG_UART
    P3OUT ^= BIT7;
#endif
       break;
    default:
      break;
 }
}

