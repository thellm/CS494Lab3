#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>



//FSR sensor pins
int mf_pin = 0;
int lf_pin = 1;
int mm_pin = 2;
int heel_pin = 3;


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
int sumMFP = 0; // calculation of MFP for current loop iteration
int avgMFP = 0; //calculation for average MFP
int MFP = 0; // final value for MFP
int MF = 0; // Medial Forefoot FSR value
int LF = 0; // Lateral Midfoot FSR value
int MM = 0; // Medial Midfoot FSR value
int HEEL = 0; // Heel FSR value

int stepCount = 0;
boolean heel_down = false;
boolean heel_up = true;

//set mfp values for different area of foot
float mfp_normal = 0;
float mfp_intoe = 0;
float mfp_outtoe = 0;
float mfp_tiptoe = 0;
float mfp_heel = 0;

//LED pin numbers
int led_mm = 5;
//int led_mf = 9; // juan
//int led_lf = 7; // juan
int led_mf = 11; //angy, david
int led_lf = 9;  //angy, david
int led_heel = 3;

//Serial communication value
int val;

//timing variables
unsigned long int starttime = 0;
unsigned long int endtime = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("");
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
  
   //get new sensor event
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  xSet = a.acceleration.x;
  ySet = a.acceleration.y;
  zSet = a.acceleration.z;
}
void loop() {
      
  while (Serial.available()) {
    val = Serial.read();
  }

  if(val == 'N'||val == 'I'||val == 'O'||val == 'T'||val == 'H'){ //configure gait
    configureMFP(val);
  }
  if(val == 'R'){
    reset();
  }
  if(val == 'D'){
    diagnose();
  }
  
  readFSR();
  delay(10);
  sendFSR();
  delay(10);
  sendMotion();

  unsigned long runMillis = millis();
  unsigned long allSeconds = millis()/1000;
  int runHours = allSeconds/3600;
  int secsRemaining=allSeconds%3600;
  int runMinutes=secsRemaining/60;
  int runSeconds=secsRemaining%60;

  Serial.print("HOUR: ");
  Serial.println(runHours);
  Serial.print("MINUTE: ");
  Serial.println(runMinutes);
  Serial.print("SECOND: ");
  Serial.println(runSeconds);
  delay(10);
  /*
  char buf[21];
  sprintf(buf,"TIME: %02d:%02d:%02d",runHours,runMinutes,runSeconds);
  Serial.println(buf);*/
}

void calcMFP(){
  MFP = ((MM + MF) * 100) / (MM + MF + LF + HEEL + 0.001);
}

void readFSR() {
  MF = analogRead(mf_pin);
  LF = analogRead(lf_pin);
  MM = analogRead(mm_pin);
  HEEL = analogRead(heel_pin);

  if(HEEL > 0){
    if(heel_down == false){
      heel_down = true;
    }
  }
  if(HEEL == 0){
    if(heel_up == false){
      heel_up = true;

    }
  }
  delay(10);
  if(heel_up  == true && heel_down == true) {
    stepCount = stepCount + 1;
    heel_up = false;
    heel_down = false;
    delay(100);
  }
  analogWrite(led_mm, MM / 4); 
  analogWrite(led_mf, MF / 4); 
  analogWrite(led_lf, LF / 4); 
  analogWrite(led_heel, HEEL / 4); 

  
}
void sendFSR(){
  Serial.print("MF: ");
  Serial.println(MF); 
  delay(10);
  Serial.print("LF: ");
  Serial.println(LF);
  delay(10);
  Serial.print("MM: ");
  Serial.println(MM);
  delay(10);
  Serial.print("HEEL: ");
  Serial.println(HEEL); 
  delay(10);
  Serial.print("STEPCOUNT: ");
  Serial.println(stepCount);
  delay(10);
 
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

  readMotion();
  
  //if the difference between configured set data and data read has a difference then in motion
  diff_z = abs((float)(zSet - z )/(float)((zSet + z)/2));
  diff_y = abs((float)(ySet - y )/(float)((ySet + y)/2));
  diff_z = abs((float)(xSet - x )/(float)((xSet + x)/2));
  Serial.println(diff_z);
  Serial.println(diff_y);
  Serial.println(diff_x);
  

  Serial.print("S3: ");
  if(diff_z == 0.00 && diff_x == 0.00){
    Serial.println("0");
  }
  else{
    Serial.println("1");
  }
  xSet = x;
  ySet = y;
  zSet = z;
}
//Input is a char of the MFP walk/gait type we are measuring
void configureMFP(int gait_type){
  sendMessage("Starting to configure gait...");//sending starting configuration
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

  String gait = "";
  //TODO put in the char that represent this gait
  if(gait_type == 'N'){ //normal
    gait = "normal";
    mfp_normal = MFP;
    is_normal = true;
  }
  else if(gait_type == 'I'){ //intoe
    gait = "in-toe";
    mfp_intoe = MFP;
    is_intoe = true;
  }
  else if(gait_type == 'O'){ //outtoe
    gait = "out-toe";
    mfp_outtoe = MFP;
    is_outtoe = true;
  }
  else if(gait_type == 'T'){ //tiptoe
    gait = "tip-toe";
    mfp_tiptoe = MFP;
    is_tiptoe = true;
  }
  else if(gait_type == 'H'){ //heel
    gait = "heel";
    mfp_heel = MFP;
  }
  String m = "Finished configuring " + gait + " gait";
  sendMessage(m);  
}

String get_walk_type(float mfp){
  //percent difference between set mfp gait values and calculation
  float percent_diff_normal = 0;
  float percent_diff_intoe = 0;
  float percent_diff_outtoe = 0;
  float percent_diff_tiptoe = 0;
  float percent_diff_heel = 0;
  
  int mini = 1;
  
  //calculate percent difference for all values of preset mfp and calculated mfp for gaits
  percent_diff_normal = abs((float)(mfp_normal - mfp )/((float)(mfp_normal + mfp)/2));
  percent_diff_intoe = abs((float)(mfp_intoe - mfp )/((float)(mfp_intoe + mfp)/2));
  percent_diff_outtoe = abs((float)(mfp_outtoe - mfp )/((float)(mfp_outtoe + mfp)/2));
  percent_diff_tiptoe = abs((float)(mfp_tiptoe - mfp )/((float)(mfp_tiptoe + mfp)/2));
  percent_diff_heel = abs((float)(mfp_heel - mfp )/((float)(mfp_heel + mfp)/2));
  
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
void sendMessage(String m){
  Serial.print("M: ");
  Serial.println(m);
}
