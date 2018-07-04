/***************************************************************************
  This is a library for the LSM9DS1 Accelerometer and magnentometer/compass

  Designed specifically to work with the Adafruit LSM9DS1 Breakouts

  These sensors use I2C to communicate, 2 pins are required to interface.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit andopen-source hardware by purchasing products
  from Adafruit!

  Written by Kevin Townsend for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ***************************************************************************/
#include <lsm9ds1.h>

/***************************************************************************
 CONSTRUCTOR
 ***************************************************************************/

void lsm9ds1::initI2C( TwoWire* wireBus, int32_t sensorID ) {
    _i2c = true;
    _wire = wireBus;
    _lsm9dso_sensorid_mag = sensorID + 2;
    _lsm9dso_sensorid_gyro = sensorID + 3;
    _magSensor   = Sensor(this, &lsm9ds1::readMag,   &lsm9ds1::getMagEvent,   &lsm9ds1::getMagSensor);
    _gyroSensor  = Sensor(this, &lsm9ds1::readGyro,  &lsm9ds1::getGyroEvent,  &lsm9ds1::getGyroSensor);
}


// default
lsm9ds1::lsm9ds1( int32_t sensorID ) {
    initI2C(&Wire, sensorID);
}

lsm9ds1::lsm9ds1( TwoWire* wireBus, int32_t sensorID ) {
    initI2C(wireBus, sensorID);
}



bool lsm9ds1::begin()
{
  if (_i2c) {
    _wire->begin();
  } 

  // soft reset & reboot accel/gyro
  write8(XGTYPE, LSM9DS1_REGISTER_CTRL_REG8, 0x05);
  // soft reset & reboot magnetometer
  write8(MAGTYPE, LSM9DS1_REGISTER_CTRL_REG2_M, 0x0C);

  delay(10);


  /*
  for (uint8_t i=0; i<0x30; i++) {
    Serial.print("XG $"); Serial.print(i, HEX); Serial.print(" = 0x"); 
    Serial.println(read8(XGTYPE, i), HEX);
  }
  for (uint8_t i=0; i<0x30; i++) {
    Serial.print("M $"); Serial.print(i, HEX); Serial.print(" = 0x"); 
    Serial.println(read8(MAGTYPE, i), HEX);
  }
  */

  uint8_t id = read8(XGTYPE, LSM9DS1_REGISTER_WHO_AM_I_XG);
  //Serial.print ("XG whoami: 0x"); Serial.println(id, HEX);
  if (id != LSM9DS1_XG_ID)
    return false;

  id = read8(MAGTYPE, LSM9DS1_REGISTER_WHO_AM_I_M);
  //Serial.print ("MAG whoami: 0x"); Serial.println(id, HEX);
  if (id != LSM9DS1_MAG_ID)
    return false;

  // enable gyro continuous
  write8(XGTYPE, LSM9DS1_REGISTER_CTRL_REG1_G, 0xC0); // on XYZ


  // enable mag continuous
  //write8(MAGTYPE, LSM9DS1_REGISTER_CTRL_REG1_M, 0xFC); // high perf XY, 80 Hz ODR
  write8(MAGTYPE, LSM9DS1_REGISTER_CTRL_REG3_M, 0x00); // continuous mode
  //write8(MAGTYPE, LSM9DS1_REGISTER_CTRL_REG4_M, 0x0C); // high perf Z mode



  // Set default ranges for the various sensors  
  setupMag(LSM9DS1_MAGGAIN_4GAUSS);
  setupGyro(LSM9DS1_GYROSCALE_245DPS);

  return true;
}

/***************************************************************************
 PUBLIC FUNCTIONS
 ***************************************************************************/
void lsm9ds1::read()
{
  /* Read all the sensors. */
  readMag();
  readGyro();
}

void lsm9ds1::readMag() {
  // Read the magnetometer
  byte buffer[6];
  readBuffer(MAGTYPE, 
       0x80 | LSM9DS1_REGISTER_OUT_X_L_M, 
       6, buffer);
  
  uint8_t xlo = buffer[0];
  int16_t xhi = buffer[1];
  uint8_t ylo = buffer[2];
  int16_t yhi = buffer[3];
  uint8_t zlo = buffer[4];
  int16_t zhi = buffer[5];
  
  // Shift values to create properly formed integer (low byte first)
  xhi <<= 8; xhi |= xlo;
  yhi <<= 8; yhi |= ylo;
  zhi <<= 8; zhi |= zlo;
  magData.x = xhi;
  magData.y = yhi;
  magData.z = zhi;
}

void lsm9ds1::readGyro() {
  // Read gyro
  byte buffer[6];
  readBuffer(XGTYPE, 
       0x80 | LSM9DS1_REGISTER_OUT_X_L_G, 
       6, buffer);
  
  uint8_t xlo = buffer[0];
  int16_t xhi = buffer[1];
  uint8_t ylo = buffer[2];
  int16_t yhi = buffer[3];
  uint8_t zlo = buffer[4];
  int16_t zhi = buffer[5];
  
  // Shift values to create properly formed integer (low byte first)
  xhi <<= 8; xhi |= xlo;
  yhi <<= 8; yhi |= ylo;
  zhi <<= 8; zhi |= zlo;
  
  gyroData.x = xhi;
  gyroData.y = yhi;
  gyroData.z = zhi;
}

void lsm9ds1::setupMag ( lsm9ds1MagGain_t gain )
{
  uint8_t reg = read8(MAGTYPE, LSM9DS1_REGISTER_CTRL_REG2_M);
  reg &= ~(0b01100000);
  reg |= gain;
  write8(MAGTYPE, LSM9DS1_REGISTER_CTRL_REG2_M, reg );

  switch(gain)
  {
    case LSM9DS1_MAGGAIN_4GAUSS:
      _mag_mgauss_lsb = LSM9DS1_MAG_MGAUSS_4GAUSS;
      break;
    case LSM9DS1_MAGGAIN_8GAUSS:
      _mag_mgauss_lsb = LSM9DS1_MAG_MGAUSS_8GAUSS;
      break;
    case LSM9DS1_MAGGAIN_12GAUSS:
      _mag_mgauss_lsb = LSM9DS1_MAG_MGAUSS_12GAUSS;
      break;
    case LSM9DS1_MAGGAIN_16GAUSS:
      _mag_mgauss_lsb = LSM9DS1_MAG_MGAUSS_16GAUSS;
      break;
  }
}

void lsm9ds1::setupGyro ( lsm9ds1GyroScale_t scale )
{
  uint8_t reg = read8(XGTYPE, LSM9DS1_REGISTER_CTRL_REG1_G);
  reg &= ~(0b00011000);
  reg |= scale;
  write8(XGTYPE, LSM9DS1_REGISTER_CTRL_REG1_G, reg );

  switch(scale)
  {
    case LSM9DS1_GYROSCALE_245DPS:
      _gyro_dps_digit = LSM9DS1_GYRO_DPS_DIGIT_245DPS;
      break;
    case LSM9DS1_GYROSCALE_500DPS:
      _gyro_dps_digit = LSM9DS1_GYRO_DPS_DIGIT_500DPS;
      break;
    case LSM9DS1_GYROSCALE_2000DPS:
      _gyro_dps_digit = LSM9DS1_GYRO_DPS_DIGIT_2000DPS;
      break;
  }
}


/***************************************************************************
 UNIFIED SENSOR FUNCTIONS
 ***************************************************************************/

/**************************************************************************/
/*!
    @brief  Gets the most recent accel sensor event
*/
/**************************************************************************/
bool lsm9ds1::getEvent(//sensors_event_t *accelEvent,
                                sensors_event_t *magEvent,
                                sensors_event_t *gyroEvent)
                         //       sensors_event_t *tempEvent )
{
  /* Grab new sensor reading and timestamp. */
  read();
  uint32_t timestamp = millis();

  /* Update appropriate sensor events. */
  // if (accelEvent) getAccelEvent(accelEvent, timestamp);
  if (magEvent)   getMagEvent(magEvent, timestamp);
  if (gyroEvent)  getGyroEvent(gyroEvent, timestamp);
  // if (tempEvent)  getTempEvent(tempEvent, timestamp);
  
  return true;
}

/**************************************************************************/
/*!
    @brief  Gets the sensor_t data
*/
/**************************************************************************/
void lsm9ds1::getSensor( sensor_t *mag,
                                 sensor_t *gyro)
{
  /* Update appropriate sensor metadata. */
  if (mag)   getMagSensor(mag);
  if (gyro)  getGyroSensor(gyro);
}

/***************************************************************************
 PRIVATE FUNCTIONS
 ***************************************************************************/
void lsm9ds1::write8(boolean type, byte reg, byte value)
{
  byte address, _cs;

  if (type == MAGTYPE) {
    address = LSM9DS1_ADDRESS_MAG;
    _cs = _csm;
  } else {
    address = LSM9DS1_ADDRESS_ACCELGYRO;
    _cs = _csxg;
  }
  if (_i2c) {
    _wire->beginTransmission(address);
    _wire->write(reg);
    _wire->write(value);
    _wire->endTransmission();
    /*
    Serial.print("0x"); Serial.print(address, HEX);
    Serial.print(" $"); Serial.print(reg, HEX); Serial.print(" = ");
    Serial.println(value, HEX);
    */
  } 
}

byte lsm9ds1::read8(boolean type, byte reg)
{
  uint8_t value;

  readBuffer(type, reg, 1, &value);

  return value;
}

byte lsm9ds1::readBuffer(boolean type, byte reg, byte len, uint8_t *buffer)
{
  byte address, _cs;

  if (type == MAGTYPE) {
    address = LSM9DS1_ADDRESS_MAG;
    _cs = _csm;
  } else {
    address = LSM9DS1_ADDRESS_ACCELGYRO;
    _cs = _csxg;
  }

  if (_i2c) {
    _wire->beginTransmission(address);
    _wire->write(reg);
    _wire->endTransmission();
    if (_wire->requestFrom(address, (byte)len) != len) {
      return 0;
    }

    /*
      Serial.print("0x"); Serial.print(address, HEX);
      Serial.print(" $"); Serial.print(reg, HEX); Serial.print(": ");
    */

    for (uint8_t i=0; i<len; i++) {
      buffer[i] = _wire->read();
      //Serial.print(buffer[i], HEX); Serial.print(", ");
    }
    //Serial.println();

  } 
  return len;
}

void lsm9ds1::getMagEvent(sensors_event_t* event, uint32_t timestamp) {
  memset(event, 0, sizeof(sensors_event_t));
  event->version   = sizeof(sensors_event_t);
  event->sensor_id = _lsm9dso_sensorid_mag;
  event->type      = SENSOR_TYPE_MAGNETIC_FIELD;
  event->timestamp = timestamp;
  event->magnetic.x = magData.x * _mag_mgauss_lsb;
  event->magnetic.x /= 1000;
  event->magnetic.y = magData.y * _mag_mgauss_lsb;
  event->magnetic.y /= 1000;
  event->magnetic.z = magData.z * _mag_mgauss_lsb;
  event->magnetic.z /= 1000;
}

void lsm9ds1::getGyroEvent(sensors_event_t* event, uint32_t timestamp) {
  memset(event, 0, sizeof(sensors_event_t));
  event->version   = sizeof(sensors_event_t);
  event->sensor_id = _lsm9dso_sensorid_gyro;
  event->type      = SENSOR_TYPE_GYROSCOPE;
  event->timestamp = timestamp;
  event->gyro.x = gyroData.x * _gyro_dps_digit;
  event->gyro.y = gyroData.y * _gyro_dps_digit;
  event->gyro.z = gyroData.z * _gyro_dps_digit;
}

void lsm9ds1::getMagSensor(sensor_t* sensor) {
  memset(sensor, 0, sizeof(sensor_t));
  strncpy (sensor->name, "LSM9DS1_M", sizeof(sensor->name) - 1);
  sensor->name[sizeof(sensor->name)- 1] = 0;
  sensor->version     = 1;
  sensor->sensor_id   = _lsm9dso_sensorid_mag;
  sensor->type        = SENSOR_TYPE_MAGNETIC_FIELD;
  sensor->min_delay   = 0;
  sensor->max_value   = 0.0;  // ToDo
  sensor->min_value   = 0.0;  // ToDo
  sensor->resolution  = 0.0;  // ToDo
}

void lsm9ds1::getGyroSensor(sensor_t* sensor) {
  memset(sensor, 0, sizeof(sensor_t));
  strncpy (sensor->name, "LSM9DS1_G", sizeof(sensor->name) - 1);
  sensor->name[sizeof(sensor->name)- 1] = 0;
  sensor->version     = 1;
  sensor->sensor_id   = _lsm9dso_sensorid_gyro;
  sensor->type        = SENSOR_TYPE_GYROSCOPE;
  sensor->min_delay   = 0;
  sensor->max_value   = 0.0;  // ToDo
  sensor->min_value   = 0.0;  // ToDo
  sensor->resolution  = 0.0;  // ToDo
}


