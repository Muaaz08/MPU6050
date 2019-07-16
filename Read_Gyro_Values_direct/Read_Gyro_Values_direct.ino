#include <Wire.h>

int gyroX, gyroY, gyroZ;
float rotX, rotY, rotZ;
long gyro_x_cal;
long gyro_y_cal;
long gyro_z_cal;

float pitch = 0;
float roll = 0;
float yaw = 0;
float angle_pitch, angle_roll;
float timeStep = 0.01;
unsigned long timer = 0;
long loop_timer;

void setup() {
  Serial.begin(9600);
  Wire.begin();

  //setupMPU
  Wire.beginTransmission(0b1101000); //This is the I2C address of the MPU (b1101000/b1101001 for AC0 low/high datasheet sec. 9.2)
  Wire.write(0x6B); //Accessing the register 6B - Power Management (Sec. 4.28)
  Wire.write(0b00000000); //Setting SLEEP register to 0. (Required; see Note on p. 9)
  Wire.endTransmission();

  Wire.beginTransmission(0b1101000); //I2C address of the MPU
  Wire.write(0x1B); //Accessing the register 1B - Gyroscope Configuration (Sec. 4.4)
  Wire.write(0x08); //Setting the gyro to full scale +/- 250deg./s
  Wire.endTransmission();

  Serial.print("Calibrating gyro");
  for (int cal_int = 0; cal_int < 2000 ; cal_int ++) {                 //Run this code 2000 times
    if (cal_int % 125 == 0)Serial.print(".");                             //Print a dot on the LCD every 125 readings
    Wire.beginTransmission(0b1101000); //I2C address of the MPU
    Wire.write(0x43); //Starting register for Gyro Readings
    Wire.endTransmission();
    Wire.requestFrom(0b1101000, 6); //Request Gyro Registers (43 - 48)
    while (Wire.available() < 6);
    gyroX = Wire.read() << 8 | Wire.read(); //Store first two bytes into accelX
    gyroY = Wire.read() << 8 | Wire.read(); //Store middle two bytes into accelY
    gyroZ = Wire.read() << 8 | Wire.read(); //Store last two bytes into accelZ                                            //Read the raw acc and gyro data from the MPU-6050
    gyro_x_cal += gyroX;                                              //Add the gyro x-axis offset to the gyro_x_cal variable
    gyro_y_cal += gyroY;                                              //Add the gyro y-axis offset to the gyro_y_cal variable
    gyro_z_cal += gyroZ;                                              //Add the gyro z-axis offset to the gyro_z_cal variable
    delay(3);                                                          //Delay 3us to simulate the 250Hz program loop
  }
  gyro_x_cal /= 2000;                                                  //Divide the gyro_x_cal variable by 2000 to get the avarage offset
  gyro_y_cal /= 2000;                                                  //Divide the gyro_y_cal variable by 2000 to get the avarage offset
  gyro_z_cal /= 2000;

}


void loop() {
  timer = micros();
  
  //recordGyroRegisters
  Wire.beginTransmission(0b1101000); //I2C address of the MPU
  Wire.write(0x43); //Starting register for Gyro Readings
  Wire.endTransmission();
  Wire.requestFrom(0b1101000, 6); //Request Gyro Registers (43 - 48)
  while (Wire.available() < 6);
  gyroX = Wire.read() << 8 | Wire.read(); //Store first two bytes into accelX
  gyroY = Wire.read() << 8 | Wire.read(); //Store middle two bytes into accelY
  gyroZ = Wire.read() << 8 | Wire.read(); //Store last two bytes into accelZ

  gyroX -= gyro_x_cal;
  gyroY -= gyro_y_cal;
  gyroZ -= gyro_z_cal;

  rotX = gyroX / 131.0;
  rotY = gyroY / 131.0;
  rotZ = gyroZ / 131.0;

  pitch = pitch + rotX * timeStep;
  roll = roll + rotY * timeStep;
  yaw = yaw + rotZ * timeStep;

               
  Serial.print("Gyro (deg)");
  Serial.print(" X=");
  Serial.print(pitch);
  Serial.print(" Y=");
  Serial.print(roll);
  Serial.print(" Z=");
  Serial.println(yaw);

  // Wait to full timeStep period
  delay(1000);
}
