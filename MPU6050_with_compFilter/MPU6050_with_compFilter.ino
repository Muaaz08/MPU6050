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

  Serial.print(yaw);
  Serial.print("/");

  // Read normalized values
  Vector normAccel = mpu.readNormalizeAccel();

  // Calculate Pitch & Roll
  int pitchA = -(atan2(normAccel.XAxis, sqrt(normAccel.YAxis * normAccel.YAxis + normAccel.ZAxis * normAccel.ZAxis)) * 180.0) / M_PI;
  int rollA = (atan2(normAccel.YAxis, normAccel.ZAxis) * 180.0) / M_PI;

  //=====================JOOP BROOKING'S CODE=================================//
  pitchA -= 0.83;
  rollA -= -1.58;

  if (set_gyro_angles) {                         //If the IMU is already started
    pitch = pitch * 0.98 + pitchA * 0.02;        //Correct the drift of the gyro pitch angle with the accelerometer pitch angle
    roll = roll * 0.98 + rollA * 0.02;           //Correct the drift of the gyro roll angle with the accelerometer roll angle
  }
  else {                                         //At first start
    pitch = pitchA;                              //Set the gyro pitch angle equal to the accelerometer pitch angle
    roll = rollA;                                //Set the gyro roll angle equal to the accelerometer roll angle
    set_gyro_angles = true;                      //Set the IMU started flag
  }
  //To dampen the pitch and roll angles a complementary filter is used
  angle_pitch_output = angle_pitch_output * 0.9 + pitch * 0.1;   //Take 90% of the output pitch value and add 10% of the raw pitch value
  angle_roll_output = angle_roll_output * 0.9 + roll * 0.1;      //Take 90% of the output roll value and add 10% of the raw roll value
  //==========================================================================//
 
  Serial.print(angle_pitch_output);
  Serial.print("/");
  Serial.println(angle_roll_output);
  
  // Wait to full timeStep period
  delay((timeStep * 1000) - (millis() - timer));
}
