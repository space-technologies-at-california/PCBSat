#include "Energia.h"
#if defined(__MSP430_HAS_USCI__) || defined(__MSP430_HAS_USCI_B0__) || defined(__MSP430_HAS_EUSCI_A0__) || defined(__MSP430_HAS_USCI_A0__)
#include "usci_isr_handler.h"

/* This dummy function ensures that, when called from any module that 
 * is interested in having the USCIAB0TX_VECTOR and USCIAB0TX_VECTOR
 * installed, the linker won't strip the vectors.*/
void usci_isr_install(){}

#ifdef __MSP430_HAS_USCI_A0__
__attribute__((interrupt(USCI_A0_VECTOR)))
void USCIA0_ISR(void)
{
	if (UCA0IFG & UCTXIFG)
		uart_tx_isr();
	
	if (UCA0IFG & UCRXIFG)
		uart_rx_isr();
}
#endif

#ifdef __MSP430_HAS_EUSCI_A0__
__attribute__((interrupt(USCI_A0_VECTOR)))
void USCIA0_ISR(void)
{
  switch ( UCA0IV ) 
  { 
    case USCI_UART_UCRXIFG: uart_rx_isr(); break;
    case USCI_UART_UCTXIFG: uart_tx_isr(); break;
  }  
}
#endif

/*
#ifdef __MSP430_HAS_USCI_B0__
__attribute__((interrupt(USCI_B0_VECTOR))) 
void USCI_B0_VECTOR_ISR(void)
{
	//USCI_B0 I2C TX RX interrupt.
	if ((UCB0IFG & (UCTXIFG | UCRXIFG)) != 0)
		i2c_txrx_isr();
		
	//USCI_B0 I2C state change interrupt.
	if ((UCB0STAT & (UCALIFG | UCNACKIFG | UCSTTIFG | UCSTPIFG)) != 0)
		i2c_state_isr();

}
#endif
*/

#ifdef __MSP430_HAS_USCI__
/* USCI_Ax and USCI_Bx share the same TX interrupt vector.
 * UART: 
 *	USCIAB0TX_VECTOR services the UCA0TXIFG set in UC0IFG.
 *	USCIAB0RX_VECTOR services the UCA0RXIFG set in UC0IFG.
 * I2C: 
 *	USCIAB0TX_VECTOR services both UCB0TXIFG and UCB0RXIFG
 *	set in UC0IFG.
 *	USCIAB0RX_VECTOR services the state change interrupts
 *	UCSTTIFG, UCSTPIFG, UCIFG, UCALIFG set in UCB0STAT.*/

__attribute__((interrupt(USCIAB0TX_VECTOR))) 
void USCIAB0TX_ISR(void)
{
	/* USCI_A0 UART interrupt? */
	if (UC0IFG & UCA0TXIFG)
		uart_tx_isr();

	/* USCI_B0 I2C TX RX interrupt. */
	if ((UC0IFG & (UCB0TXIFG | UCB0RXIFG)) != 0)
		i2c_txrx_isr();

}

__attribute__((interrupt(USCIAB0RX_VECTOR)))
void USCIAB0RX_ISR(void)
{
	/* USCI_A0 UART interrupt? */
	if (UC0IFG & UCA0RXIFG)
		uart_rx_isr();

	/* USCI_B0 I2C state change interrupt. */
	if ((UCB0STAT & (UCALIFG | UCNACKIFG | UCSTTIFG | UCSTPIFG)) != 0)
		i2c_state_isr(); 
}
#endif

#endif // defined(__MSP430_HAS_USCI__) || defined(__MSP430_HAS_USCI_B0__) || defined(__MSP430_HAS_EUSCI_A0__) || defined(__MSP430_HAS_USCI_A0__)
