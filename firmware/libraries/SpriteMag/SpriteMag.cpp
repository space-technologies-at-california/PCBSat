/*
  SpriteRadio.cpp - An Energia library for the HMC5883L

*/

#include "SpriteMag.h"
#include "utility/HMC5883L_Config.h"
#include "TI_USCI_I2C_master.h"

SpriteMag::SpriteMag() {

}

void SpriteMag::init() {

	m_sendBufer[0] = 0x00;
	m_sendBufer[1] = (MAG_SAMPLES_AVE|MAG_DATA_RATE|MAG_MEAS_MODE);

	TI_USCI_I2C_transmitinit(MAG_ADDRESS, I2C_PRESCALE);
	TI_USCI_I2C_transmit(m_sendBufer, 2);

	m_sendBufer[0] = 0x01; m_sendBufer[1] = MAG_GAIN;
	TI_USCI_I2C_transmit(m_sendBufer, 2);

	m_sendBufer[0] = 0x02; m_sendBufer[1] = MAG_OPER_MODE;
	TI_USCI_I2C_transmit(m_sendBufer, 2);
}

MagneticField SpriteMag::read() {

	m_sendBufer[0] = 0x03;

	TI_USCI_I2C_transmitinit(MAG_ADDRESS, I2C_PRESCALE);
	TI_USCI_I2C_transmit(m_sendBufer, 1);

	TI_USCI_I2C_receiveinit(MAG_ADDRESS, I2C_PRESCALE);
	TI_USCI_I2C_receive(m_receiveBuffer, 6);

	MagneticField b;
	//Units are microtesla
	b.x = -.073*((m_receiveBuffer[0] << 8) | m_receiveBuffer[1]);
	b.z = .073*((m_receiveBuffer[2] << 8) | m_receiveBuffer[3]);
	b.y = -.073*((m_receiveBuffer[4] << 8) | m_receiveBuffer[5]);

	return b;
}
