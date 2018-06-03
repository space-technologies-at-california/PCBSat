/*
  SpriteRadio.h - An Energia library for ITG3200

*/

#ifndef SpriteGyro_h
#define SpriteGyro_h

typedef struct AngularVelocity {
	int x;
	int y;
	int z;
} AngularVelocity;

class SpriteGyro {
  public:
	// Constructor
	SpriteGyro();
	SpriteGyro(AngularVelocity bias);
	
	void init();

	// Read angular rate from gyro
	AngularVelocity read();
	
  private:
  	unsigned char m_sendBufer[2];
  	unsigned char m_receiveBuffer[6];
	
	int m_biasx;
	int m_biasy;
	int m_biasz;

};

#endif //SpriteGyro_h