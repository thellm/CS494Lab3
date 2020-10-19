// Basic demo for accelerometer readings from Adafruit MPU6050

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <MPU6050_tockn.h>
#include <Wire.h>



Adafruit_MPU6050 mpu;
int xSet =0;
int ySet =0;
int zSet = 6;

//variables to store measurements and time

float stepL;
float strideL;
int totalSteps = 0;

unsigned long time1;
unsigned long time2;
unsigned long interval;

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

Wire.begin();
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);
}

void loop() {

mpu6050.update();

time1 = millis();
  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  

  if(millis() - timer > 1000){
    

    Serial.print("\taccY : ");Serial.print(mpu6050.getAccY());   
  }
if(mpu6050.getAccY()>1)
{

totalSteps += 1;
time2 = millis();
interval = time2 - time1;
stepL = (1/2)*mpu6050.getAccY()*sq(interval);
strideL = 2 * stepL;

delay(350);

}



  delay(100);
}
