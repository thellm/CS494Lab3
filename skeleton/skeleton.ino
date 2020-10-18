#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

//FSR sensor pins
#define mf A0
#define lf A1
#define mm A2
#define heel A3

//accelorometer information 
Adafruit_MPU6050 mpu;

//the set data from configuration
int xSet = 0;
int ySet = 0;
int zSet = 6;

//values being read in live
int x = 0;
int y = 0;
int z = 0;

//checking if in motion
bool is_motion = false;

//booleans for set configurations
bool is_normal = false;
bool is_intoe = false;
bool is_outtoe = false;
bool is_tiptoe = false;
bool is_heel = false;
bool is_config = false;

int counter = 0; // counts iterations of loop
float sumMFP = 0; // calculation of MFP for current loop iteration
float avgMFP = 0; //calculation for average MFP
float MFP = 0; // final value for MFP
float MF = 0; // Medial Forefoot FSR value
float LF = 0; // Lateral Midfoot FSR value
float MM = 0; // Medial Midfoot FSR value
float HEEL = 0; // Heel FSR value

//set mfp values for different area of foot
float mfp_normal = 0;
float mfp_intoe = 0;
float mfp_outtoe = 0;
float mfp_tiptoe = 0;
float mfp_heel = 0;

//percent difference between set mfp gait values and calculation
float percent_diff_normal = 0;
float percent_diff_intoe = 0;
float percent_diff_outtoe = 0;
float percent_diff_tiptoe = 0;
float percent_diff_heel = 0;

//LED pin numbers
int led_mm = 8;
int led_mf = 7;
int led_lf = 6;
int led_heel = 5;

//Serial communication value
String val = "";

//timing variables
unsigned long int starttime = 0;
unsigned long int endtime = 0;

void setup() {

  Serial.begin(115200);
  while(!Serial){delay(10);}

  if(!mpu.begin()){
    Serial.println("Failed to find MPU6050 chip!");
    while(1){
      delay(10);
    }
  }

  //setting up the accelorometer
  mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.println("");
  delay(100);
  
  pinMode(led_mm, OUTPUT);
  pinMode(led_mf, OUTPUT);
  pinMode(led_lf, OUTPUT);
  pinMode(led_heel, OUTPUT);
  
}
void loop() {
  
  while (Serial.available()) {
    val = Serial.read();
  }
  if(val == 'C') {  //configure motion sensor
    configureMotion();
  }
  else if(val == 'N'||val == 'I'||val == 'O'||val == 'T'||val == 'H'){ //configure gait
    configureMFP(val);
  }
  else if(val == 'R'){
    reset();
  }
  else if(val == 'D' && is_ready()){
    diagnose();
  }
  else if(is_ready()){ 
    readFSR();
    readMotion();
    lightLED();
    
    sendMotion();
    sendFSR();
  }
}

void calcMFP(){
  MFP = ((MM + MF) * 100) / (MM + MF + LF + HEEL + 0.001);
}

void readFSR() {
  MF = analogRead(mf);
  LF = analogRead(lf);
  MM = analogRead(mm);
  HEEL = analogRead(heel);
  
}
void sendFSR(){
  Serial.print("P: ");
  Serial.print(MF); Serial.print(" ");
  Serial.print(LF); Serial.print(" ");
  Serial.print(MM); Serial.print(" ");
  Serial.println(HEEL); 
}
void lightLED(){
  // analogRead values go from 0 to 1023, analogWrite values from 0 to 255
  analogWrite(led_mm, MM / 4); 
  analogWrite(led_mf, MF / 4); 
  analogWrite(led_lf, LF / 4); 
  analogWrite(led_heel, HEEL / 4);   
}
void readMotion(){
  //get new sensor event
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  //get values
  x = a.acceleration.x;
  y = a.acceleration.y;
  z = a.acceleration.z;
}

void sendMotion(){
  //for difference calculations
  float diff_x = 0;
  float diff_y = 0;
  float diff_z = 0;
  
  Serial.print("S3: ");

  readMotion(); //read motion data

  //if the difference between configured set data and data read has a difference then in motion
  diff_x = abs(xSet - x); 
  diff_y = abs(ySet - y);
  diff_z = abs(zSet - z);

  if(diff_z&&diff_y&&diff_x){
    is_motion = true;
  }
  else{
    is_motion = false;
  }
  
  if(is_motion){
    Serial.println("1");
  }
  else {
    Serial.println("0");
  }
}
void configureMotion(){
  starttime = millis();
  endtime = starttime;

  float count = 0;
  float sum_x = 0;
  float sum_y = 0;
  float sum_z = 0;

  while((endtime - starttime) < 30000){
    readMotion();
    delay(10);
    sum_x = sum_x + x;
    sum_y = sum_y + y;
    sum_z = sum_z + z;
    count++;

    endtime = millis();
  }
  xSet = sum_x/count;
  ySet = sum_y/count;
  zSet = sum_z/count;
  is_motion = true;
}

//Input is a char of the MFP walk/gait type we are measuring
void configureMFP(String gait_type){
  starttime = millis();
  endtime = starttime;

  //reset MFP variables
  sumMFP = 0;
  avgMFP = 0;
  counter = 0;
  
  //loop for 30 seconds
  while((endtime-starttime) < 30000){
    
    readFSR(); //get sensor data  
    sendFSR(); //send sensor data
    calcMFP(); //calculate MFP
    
    sumMFP = sumMFP + MFP;  //sum all MFP
    counter++;              //increment counter which is amount of value readings

    //small delay between reading to not cluster serial
    delay(10);
    
    //update end time
    endtime = millis();
  }
  //get average MFP by dividing sum by amount of values
  avgMFP = sumMFP/counter;

  Serial.print(gait_type);
  Serial.print(" ");
  Serial.println(MFP);

  //TODO put in the char that represent this gait
  if(gait_type == 'N'){ //normal
    mfp_normal = MFP;
    is_normal = true;
  }
  else if(gait_type == 'I'){ //intoe
    mfp_intoe = MFP;
    is_intoe = true;
  }
  else if(gait_type == 'O'){ //outtoe
    mfp_outtoe = MFP;
    is_outtoe = true;
  }
  else if(gait_type == 'T'){ //tiptoe
    mfp_tiptoe = MFP;
    is_tiptoe = true;
  }
  else if(gait_type == 'H'){ //heel
    mfp_heel = MFP;
    is_heel = true;
  }
}

String get_walk_type(float mfp){
  
  int mini = 1;
  
  //calculate percent difference for all values of preset mfp and calculated mfp for gaits
  percent_diff_normal = abs((mfp_normal - mfp )/((mfp_normal + mfp)/2));
  percent_diff_intoe = abs((mfp_intoe - mfp )/((mfp_intoe + mfp)/2));
  percent_diff_outtoe = abs((mfp_outtoe - mfp )/((mfp_outtoe + mfp)/2));
  percent_diff_tiptoe = abs((mfp_tiptoe - mfp )/((mfp_tiptoe + mfp)/2));
  percent_diff_heel = abs((mfp_heel - mfp )/((mfp_heel + mfp)/2));
  
  //compare gaits to see which has smallest percent difference and that is the walk
  if( percent_diff_normal < mini) {
    mini =  percent_diff_normal;
    return("Normal");
  }
  if( percent_diff_intoe < mini){
    mini =  percent_diff_intoe;
    return("In-Toe");
  }
  if( percent_diff_outtoe < mini){
    mini =  percent_diff_outtoe;
    return("Out-Toe");
  }
  if( percent_diff_tiptoe < mini){
    mini =  percent_diff_tiptoe;
    return("Tip-Toe");
  }
  if( percent_diff_heel < mini){
    mini =  percent_diff_heel;
    return("Heel");
  }
}

void diagnose(){
  starttime = millis();
  endtime = starttime;
  
  String walk_type = "";
  
  //reset avg MFP variables
  sumMFP = 0;
  counter = 0;
  avgMFP = 0;
  
  //loop for a minute
  while((endtime - starttime) < 60000){
    
    readFSR(); //get sensor data  
    sendFSR(); //send sensor data

    calcMFP(); //calculate MFP

    sumMFP = sumMFP + MFP;
    counter++;

    //get walk type after finding percent difference
    walk_type = get_walk_type(MFP);
    
    Serial.print("S2: "); //section 2
    Serial.println(walk_type); //walk type
    
    delay(10);
    
    endtime = millis();    
  } //end of while loop

  avgMFP = sumMFP / counter; //average MFP is final MFP
  
  //get the final walk type
  walk_type = get_walk_type(avgMFP);

  Serial.print("S2: "); //section 2
  Serial.print("F "); //final
  Serial.println(walk_type); //walk type
}
void reset(){ //resetting stats
  xSet = 0;
  ySet = 0;
  zSet = 6;
  is_motion = false;

  mfp_normal = 0;
  mfp_intoe = 0;
  mfp_outtoe = 0;
  mfp_tiptoe = 0;
  mfp_heel = 0;
  
  is_normal = false;
  is_intoe = false;
  is_outtoe = false;
  is_tiptoe = false;
  is_heel = false;
  is_config = false;
  
}
bool is_ready(){
  if(is_normal && is_intoe && is_outtoe && is_tiptoe && is_heel && is_config){
    return true;
  }
  else{
    return false;
  }
}
