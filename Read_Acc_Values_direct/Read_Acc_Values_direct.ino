//ref: EEEnthusiast (https://www.youtube.com/watch?v=M9lZ5Qy5S2s)

#include <Wire.h>

int pitch, roll;
long accelX, accelY, accelZ;
float gForceX, gForceY, gForceZ;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  
  //setupMPU
  Wire.beginTransmission(0b1101000); //This is the I2C address of the MPU (b1101000/b1101001 for AC0 low/high datasheet sec. 9.2)
  Wire.write(0x6B); //Accessing the register 6B - Power Management (Sec. 4.28)
  Wire.write(0b00000000); //Setting SLEEP register to 0. (Required; see Note on p. 9)
  Wire.endTransmission();

  Wire.beginTransmission(0b1101000); //I2C address of the MPU
  Wire.write(0x1C); //Accessing the register 1C - Acccelerometer Configuration (Sec. 4.5)
  Wire.write(0b00000000); //Setting the accel to +/- 2g
  Wire.endTransmission();

  
}

void loop() {
  //  recordAccelRegisters();
  Wire.beginTransmission(0b1101000); //I2C address of the MPU
  Wire.write(0x3B); //Starting register for Accel Readings
  Wire.endTransmission();
  Wire.requestFrom(0b1101000, 6); //Request Accel Registers (3B - 40)
  while (Wire.available() < 6);
  accelX = Wire.read() << 8 | Wire.read(); //Store first two bytes into accelX
  accelY = Wire.read() << 8 | Wire.read(); //Store middle two bytes into accelY
  accelZ = Wire.read() << 8 | Wire.read(); //Store last two bytes into accelZ

  // process the data
  gForceX = accelX / 16384.0;
  gForceY = accelY / 16384.0;
  gForceZ = accelZ / 16384.0;

  // Calculate Pitch & Roll
  pitch = -(atan2(gForceX, sqrt(gForceY * gForceY + gForceZ * gForceZ)) * 180.0) / 3.14159;
  roll = (atan2(gForceY, gForceZ) * 180.0) / 3.14159;
  
  //  printData();
  //Serial.print(" Pitch:");
  Serial.print(pitch);
  Serial.print(" ");
  //Serial.print(" Roll:");
  Serial.println(roll);
  delay(1000);
}
