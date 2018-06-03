/*
  SpriteRadio.h - An Energia library for the HMC5993L

*/

#ifndef SpriteMag_h
#define SpriteMag_h

typedef struct MagneticField {
	float x;
	float y;
	float z;
} MagneticField;

class SpriteMag {
  public:
	// Constructor
	SpriteMag();

	void init();

	MagneticField read();
	
  private:
  	unsigned char m_sendBufer[2];
  	unsigned char m_receiveBuffer[6];

};

#endif //SpriteMag_h