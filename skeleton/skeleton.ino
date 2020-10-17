#define mf A0
#define lf A1
#define mm A2
#define heel A3


int counter = 0; // counts iterations of loop
float sumMFP = 0; // calculation of MFP for current loop iteration
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

//LED pin numbers
int ledR = 8;
int ledY = 7;
int ledG = 6;
int ledB = 5;

//timing variables
unsigned long int starttime = 0;
unsigned long int endtime = 0;

void setup() {
  // Begin serial communication at a baud rate of 9600:
  Serial.begin(9600);
  pinMode(ledR, OUTPUT);
  pinMode(ledY, OUTPUT);
  pinMode(ledG, OUTPUT);
  pinMode(ledB, OUTPUT);
}
void loop() {

  //TODO 
  //add serial communication part
  //some things changed from serial communication document, in particular having to do with section 2
  
}

void sendFSRData(){
  Serial.print("P: ");
  Serial.print(MF); Serial.print(" ");
  Serial.print(LF); Serial.print(" ");
  Serial.print(MM); Serial.print(" ");
  Serial.println(HEEL); 
}

//Input is a char of the MFP walk/gait type we are measuring
void configureMSF(char gait_type){
  starttime = millis();
  endtime = starttime;
  
  //loop for 30 seconds
  while((endtime-starttime) < 30000){
    
    readSensors(); //get sensor data  
    sendFSRData(); //send sensor data

    MFP = ((MM + MF) * 100) / (MM + MF + LF + HEEL + 0.001); //calculate MFP
    
    sumMFP = sumMFP + MFP;  //sum all MFP
    counter++;              //increment counter which is amount of value readings

    //small delay between reading to not cluster serial
    delay(10);
    
    //update end time
    endtime = millis();
  }
  //get average MSF by dividing sum by amount of values
  MFP = MFP/counter;

  Serial.print(gait_type);
  Serial.print(": ");
  Serial.println(MFP);

  //TODO put in the char that represent this gait
  if(gait_type == 'N'){ //normal
    mfp_normal = MFP;
  }
  else if(gait_type == 'I'){ //intoe
    mfp_intoe = MFP;
  }
  else if(gait_type == 'O'){ //outtoe
    mfp_outtoe = MFP;
  }
  else if(gait_type == 'T'){ //tiptoe
    mfp_tiptoe = MFP;
  }
  else if(gait_type == 'H'){ //heel
    mfp_heel = MFP;
  }
}
void readSensors() {
  MF = analogRead(mf);
  LF = analogRead(lf);
  MM = analogRead(mm);
  HEEL = analogRead(heel);
}
void diagnose(){
  starttime = millis();
  endtime = starttime;

  float sum = 0;
  float count = 0;
  float avgMFP = 0;
  
  //loop for a minute
  while((endtime - starttime) < 60000){
    
    float percent_diff_normal = 0;
    float percent_diff_intoe = 0;
    float percent_diff_outtoe = 0;
    float percent_diff_tiptoe = 0;
    float percent_diff_heel = 0;
    
    float mini = 1;
    String walk_type = "";
    
    readSensors(); //get sensor data  
    sendFSRData(); //send sensor data

    MFP = ((MM + MF) * 100) / (MM + MF + LF + HEEL + 0.001); //calculate MFP

    sum = sum + MFP;
    count++;
    
    //calculate percent difference for all values of preset mfp and calculated mfp for gaits
    percent_diff_normal = abs((mfp_normal - MFP )/((mfp_normal + MFP)/2));
    percent_diff_intoe = abs((mfp_intoe - MFP )/((mfp_intoe + MFP)/2));
    percent_diff_outtoe = abs((mfp_outtoe - MFP )/((mfp_outtoe + MFP)/2));
    percent_diff_tiptoe = abs((mfp_tiptoe - MFP )/((mfp_tiptoe + MFP)/2));
    percent_diff_heel = abs((mfp_heel - MFP )/((mfp_heel + MFP)/2));

    //compare gaits to see which has smallest percent difference and that is the walk
    if( percent_diff_normal < mini) {
      mini =  percent_diff_normal;
      walk_type = "Normal";
    }
    if( percent_diff_intoe < mini){
      mini =  percent_diff_intoe;
      walk_type = "In-Toe";
    }
    if( percent_diff_outtoe < mini){
      mini =  percent_diff_outtoe;
      walk_type = "Out-Toe";
    }
    if( percent_diff_tiptoe < mini){
      mini =  percent_diff_tiptoe;
      walk_type = "Tip-Toe";
    }
    if( percent_diff_heel < mini){
      mini =  percent_diff_heel;
      walk_type = "Heel";
    }

    Serial.print("D: ");
    Serial.println(walk_type);
    
    delay(10);
    
    endtime = millis();    
  }

  avgMFP = sum / count; //average MFP
  
  //do comparisons again
  float percent_diff_normal = 0;
  float percent_diff_intoe = 0;
  float percent_diff_outtoe = 0;
  float percent_diff_tiptoe = 0;
  float percent_diff_heel = 0;
  
  float mini = 1;
  String walk_type = "";
  
  //calculate percent difference for all values of preset mfp and calculated mfp for gaits
  percent_diff_normal = abs((mfp_normal - avgMFP )/((mfp_normal + avgMFP)/2));
  percent_diff_intoe = abs((mfp_intoe - avgMFP )/((mfp_intoe + avgMFP)/2));
  percent_diff_outtoe = abs((mfp_outtoe - avgMFP )/((mfp_outtoe + avgMFP)/2));
  percent_diff_tiptoe = abs((mfp_tiptoe - avgMFP )/((mfp_tiptoe + avgMFP)/2));
  percent_diff_heel = abs((mfp_heel - avgMFP )/((mfp_heel + avgMFP)/2));

  //compare gaits to see which has smallest percent difference and that is the walk
  if( percent_diff_normal < mini) {
    mini =  percent_diff_normal;
    walk_type = "Normal";
  }
  if( percent_diff_intoe < mini){
    mini =  percent_diff_intoe;
    walk_type = "In-Toe";
  }
  if( percent_diff_outtoe < mini){
    mini =  percent_diff_outtoe;
    walk_type = "Out-Toe";
  }
  if( percent_diff_tiptoe < mini){
    mini =  percent_diff_tiptoe;
    walk_type = "Tip-Toe";
  }
  if( percent_diff_heel < mini){
    mini =  percent_diff_heel;
    walk_type = "Heel";
  }

  Serial.print("D: ");
  Serial.print("Final Diagnoses of walk is...");
  Serial.println(walk_type);
}
