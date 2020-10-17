// Basic demo for accelerometer readings from Adafruit MPU6050

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;
int xSet =0;
int ySet =0;
int zSet = 6;

void setup(void) {
  Serial.begin(115200);
  while (!Serial) {
    delay(10); // will pause Zero, Leonardo, etc until serial console opens
  }

  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.println("");
  delay(100);
}

void loop() {

  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  /* Print out the values */
//  Serial.print(a.acceleration.x);
//  Serial.print(",");
//  Serial.print(a.acceleration.y);
//  Serial.print(",");
//  Serial.print(a.acceleration.z);

  if(xSet > 1 || xset < -1
  || ySet > 1 || ySet < -1 
  || zSet > 7 || zSet < 5 ){
      Serial.print("S3: 0 "+xSet);
  }else{
      Serial.print("S3: 1  0.0");
  }
  Serial.println("");

  delay(100);
}
