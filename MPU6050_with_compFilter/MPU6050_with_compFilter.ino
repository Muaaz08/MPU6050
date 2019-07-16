#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

float pitchA, rollA, pitchC, rollC;
long accelX, accelY, accelZ;
float gForceX, gForceY, gForceZ;
float angle_pitch_output, angle_roll_output;
boolean set_gyro_angles;


// Timers
unsigned long timer = 0;
float timeStep = 0.01;

// Pitch, Roll and Yaw values
float pitch = 0;
float roll = 0;
float yaw = 0;

void setup()
{
  Serial.begin(115200);

  // Initialize MPU6050
  while (!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G))
  {
    Serial.println("Could not find a valid MPU6050 sensor, check wiring!");
    delay(500);
  }
  mpu.calibrateGyro();
  mpu.setThreshold(3);
}

void loop()
{
  timer = millis();

  Vector norm = mpu.readNormalizeGyro();

  //read yaw,pitch and roll from gyroscope
  pitch = pitch + norm.YAxis * timeStep;
  roll = roll + norm.XAxis * timeStep;
  yaw = yaw + norm.ZAxis * timeStep;

//  Serial.print(pitch);
//  Serial.print("/");
//  Serial.print(roll);
//  Serial.print("/");
  Serial.print(yaw);
  Serial.print("/");

  readAcc();
//
//  Serial.print(pitchA);
//  Serial.print("/");
//  Serial.print(rollA);
//  Serial.print("/");

  pitchA -= 0.83;
  rollA -= -1.58;

  if (set_gyro_angles) {                                               //If the IMU is already started
    pitch = pitch * 0.98 + pitchA * 0.02;     //Correct the drift of the gyro pitch angle with the accelerometer pitch angle
    roll = roll * 0.98 + rollA * 0.02;        //Correct the drift of the gyro roll angle with the accelerometer roll angle
  }
  else {                                                               //At first start
    pitch = pitchA;                                     //Set the gyro pitch angle equal to the accelerometer pitch angle
    roll = rollA;                                       //Set the gyro roll angle equal to the accelerometer roll angle
    set_gyro_angles = true;                                            //Set the IMU started flag
  }

  //To dampen the pitch and roll angles a complementary filter is used
  angle_pitch_output = angle_pitch_output * 0.9 + pitch * 0.1;   //Take 90% of the output pitch value and add 10% of the raw pitch value
  angle_roll_output = angle_roll_output * 0.9 + roll * 0.1;      //Take 90% of the output roll value and add 10% of the raw roll value

  Serial.print(angle_pitch_output);
  Serial.print("/");
  Serial.println(angle_roll_output);
  // Wait to full timeStep period
  delay((timeStep * 1000) - (millis() - timer));
}

void readAcc() {
  //  recordAccelRegisters
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
  pitchA = -(atan2(gForceX, sqrt(pow(gForceY, 2) + pow(gForceZ, 2))) * 180.0) / 3.14159;
  rollA = (atan2(gForceY, gForceZ) * 180.0) / 3.14159;

  // printData
  //  Serial.print("/");
  //  Serial.print(pitchA);
  //  Serial.print("/");
  //  Serial.print(rollA);
  //  Serial.println("/");
  //delay(1000);
}
