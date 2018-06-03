/*
  SpriteGyro.cpp - An Energia library for the ITG3200

*/

#include "SpriteGyro.h"
#include "utility/ITG3200_Config.h"
#include "TI_USCI_I2C_master.h"

SpriteGyro::SpriteGyro() {
	m_biasx = 0;
	m_biasy = 0;
	m_biasz = 0;
}

SpriteGyro::SpriteGyro(AngularVelocity bias) {
	m_biasx = bias.x;
	m_biasy = bias.y;
	m_biasz = bias.z;
}

void SpriteGyro::init() {
	//Sample rate divider registers
	m_sendBufer[0] = SMPL_RATE_REG_ADDR;
	m_sendBufer[1] = GYRO_SAMPLE_RATE;				
	
	TI_USCI_I2C_transmitinit(GYRO_ADDRESS, I2C_PRESCALE);
	TI_USCI_I2C_transmit(m_sendBufer, 2);						//write to register

	m_sendBufer[0] = DLPF_RANGE_REG_ADDR;					//Sample rate divider registers
	m_sendBufer[1] = GYRO_RANGE|GYRO_FILTER_SMPL_RATE ;		//sample rate divider relative to internal sampling

	TI_USCI_I2C_transmit(m_sendBufer, 2);						//write to DLPF and range register
}

AngularVelocity SpriteGyro::read() {
	m_sendBufer[0] = 0x1D;							//point to first gyro data register
	TI_USCI_I2C_transmitinit(GYRO_ADDRESS, I2C_PRESCALE);
	TI_USCI_I2C_transmit(m_sendBufer, 1);

	TI_USCI_I2C_receiveinit(GYRO_ADDRESS, I2C_PRESCALE);   	//receiving 6 bytes
	TI_USCI_I2C_receive(m_receiveBuffer, 6);

	AngularVelocity output;
	output.x = ((int)m_receiveBuffer[0]<<8) + (int)m_receiveBuffer[1] + m_biasx;
	output.y = ((int)m_receiveBuffer[2]<<8) + (int)m_receiveBuffer[3] + m_biasy;
	output.z = ((int)m_receiveBuffer[4]<<8) + (int)m_receiveBuffer[5] + m_biasz;

	return output;
}
