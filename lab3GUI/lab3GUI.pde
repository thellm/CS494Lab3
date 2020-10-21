import apsync.*;
import processing.serial.*;
import controlP5.*;

//Globals for GUI
ControlP5 cp5;
PImage foot_img;
PFont font;
PFont button_font;
PFont button_font2;
String message = "Updates will diplay here...";

//Serial communication variables
AP_Sync streamer;
String[] line;
Serial port;
String mode;

//user stats
String step_count = "0";
String step_length = "0";
String stride_length = "0";
String time = "";
String speed = "0";
String walking = "";
String diagnoses = "Unknown";
String hour = "0";
String minute = "0";
String second = "0";

//Pressure readings for different foot area
//the value will manipulate circle size
//and the point where graph is
int mm = 10;
int point_mm;

int mf = 10;
int point_mf;

int lf = 10;
int point_lf;

int heel = 10;
int point_heel;

//Graph variables
int xPos = 900;
int yPos = 450;
int xEdge = 1600;

ArrayList<Integer> currentLF = new ArrayList<Integer>();
ArrayList<Integer> current_LFAxis = new ArrayList<Integer>();
int LFIndex;
boolean newDataLF = false;

ArrayList<Integer> currentMF = new ArrayList<Integer>();
ArrayList<Integer> current_MFAxis = new ArrayList<Integer>();
int MFIndex;
boolean newDataMF = false;

ArrayList<Integer> currentMM = new ArrayList<Integer>();
ArrayList<Integer> current_MMAxis = new ArrayList<Integer>();
int MMIndex;
boolean newDataMM = false;

ArrayList<Integer> currentHEEL = new ArrayList<Integer>();
ArrayList<Integer> current_HEELAxis = new ArrayList<Integer>();
int HEELIndex;
boolean newDataHEEL = false;

//=============================================================================================

void setup() {
  
  printArray(Serial.list());
  port = new Serial(this, "COM3", 115200); //make sure to change to your port connected to device
  //streamer = new AP_Sync(this, "COM3", 115200);
  // port = new Serial(this, "/dev/cu.usbmodem123456781", 115200); //make sure to change to your port connected to device
  port.bufferUntil('\n');
  
  foot_img = loadImage("foot.png");
  frameRate(10);
  // Create the font
  font = createFont("SourceCodePro-Regular.ttf", 36);
  textFont(font);
  cp5 = new ControlP5(this);
  
  button_font = createFont("SourceCodePro-Regular.ttf", 20);
  button_font2 = createFont("SourceCodePro-Regular.ttf", 30);
  // create a new button with name 'buttonA'
  cp5.addButton("normalgait")
   .setLabel("Set Normal Gait")
   .setPosition(910,480)
   .setSize(250,30)
   .setFont(button_font)
   ;
  cp5.addButton("intoe")
   .setLabel("Set In-Toe Gait")
   .setPosition(910,520)
   .setSize(250,30)
   .setFont(button_font)
   ;
  cp5.addButton("outtoe")
   .setLabel("Set Out-Toe Gait")
   .setPosition(910,560)
   .setSize(250,30)
   .setFont(button_font)
   ;
  cp5.addButton("tiptoe")
   .setLabel("Set Tip-Toe Gait")
   .setPosition(910,600)
   .setSize(250,30)
   .setFont(button_font)
   ;
  cp5.addButton("heel")
   .setLabel("Set Heel Gait")
   .setPosition(910,640)
   .setSize(250,30)
   .setFont(button_font)
   ;
  cp5.addButton("diagnose")
   .setLabel("Diagnose Me")
   .setPosition(1245, 610)
   .setSize(350,40)
   .setFont(button_font2)
   ;
  cp5.addButton("reset")
   .setLabel("Reset All")
   .setPosition(1245,480)
   .setSize(350,40)
   .setFont(button_font2)
   ;
  cp5.addButton("configure")
   .setLabel("Configure Motion")
   .setPosition(1245,530)
   .setSize(350,40)
   .setFont(button_font2)
   ; 

  delay(1000);
}

//=============================================================================================
public void settings() {
  size(1650, 830);
}

void draw() {
  
  background(181,211,231);
  
  fill(200,0,0);                //covers set gait button area, RED box
  rect(900, 470, 270, 210, 7); 
      
  fill(100,220,100);            //covers diagnose button area, GREEN box
  rect(1235, 600, 370, 60, 7);
  
  fill(0,100,200);              //covers reset/configure button area, BLUE box
  rect(1235, 470, 370, 110, 7);
  
  fill(240,240,100);            //cover instructions, YELLOW box
  rect(900, 695, 705, 120,7);
  
  fill(250);                    //updates area, WHITE box
  rect(380, 695, 480, 120, 7);
  
  fill(0);                      //Instructions for GUI
  textSize(18);
  text("STEP 1: Configure motion sensor, stand still for 30 seconds", 910, 720);
  text("STEP 2: Set each gait type with walking style for 30 seconds", 910, 740);
  text("STEP 3: Press reset if need to reconfigure and repeat steps 1,2", 910, 760);
  text("STEP 4: Check your stats while in motion or press 'Diagnose Me'", 910, 780);
  text("        to find out your walking pattern", 910, 800);
      
             
  fill(255);
  ellipse(530,60,240,105);
  fill(0);
  textSize(60);                //welcome message at top of gui
  text("Welcome to ", 25, 80);
  textSize(80);
  fill(255, 165, 0);
  text("W", 445, 90);
  fill(80, 220, 100);
  text("Y", 502, 90);
  fill(0,0,255);
  text("S", 545, 90);
  fill(255,0,0);
  text("!", 585, 90);
  textSize(60); 
  fill(0);
  text(" 'Watch Your Step' Module", 630, 80);
  
  
  //fill(0);
  //textSize(65);
  //text("Welcome to WYS! 'Watch Your Step' Module", 40, 70);
  
//------------foot imaging area------------------------------------------------
  image(foot_img, 10, 100, width/4.5, 700);
  
//-----------------------Area for graph----------------------------------------
 
  fill(235);
  rect(890, 135, 340, 150, 7);  //LF
  rect(1240, 135, 340, 150, 7); //MF
  rect(890, 305, 340, 150, 7); // MM
  rect(1240, 305, 340, 150, 7); //HEEL
  textSize(30); 
  fill(0);
  text("HEEL", 1250, 340);
  text("MF", 1250, 170);
  text("LF", 900, 170);
  text("MM", 900, 340);
  
  fill(255,0,0);

  textSize(35); 
  //draw circle for heel, RED color
  fill(255,0,0);
  circle(210, 690, heel);
  fill(250);
  text("HEEL", 170, 740);
  
  //draw circle for mm, BLUE color
  fill(0, 0, 255);
  circle(260, 490, mm);
  fill(250);
  text("MM", 240, 530);
    
  //draw circle for mf, GREEN color
  fill(80, 220, 100);
  circle(200, 280, mf);
  fill(250);
  text("MF", 180, 320);
  
  //draw circle for lf, ORANGE color
  fill(255, 175, 0);
  circle(110, 420, lf);
  fill(250);
  text("LF", 90, 460);
  
  if(newDataHEEL){
    for(int i = 0; i < currentHEEL.size(); i++){
      fill(255,0,0);
      circle(current_HEELAxis.get(i), 450 - currentHEEL.get(i), 5);
    }
    if(HEELIndex + xPos > 1210){
      HEELIndex = 0;
      currentHEEL = new ArrayList<Integer>();
      current_HEELAxis = new ArrayList<Integer>();
    }
    else{ HEELIndex+=10; }
    newDataHEEL = false;
  }
  
  if(newDataMM){
    for(int i = 0; i < currentMM.size(); i++){
      fill(0, 0, 255);
      circle(current_MMAxis.get(i), 450 - currentMM.get(i), 5);
    }
    if(MMIndex  + xPos > 1210){
      MMIndex = 0;
      currentMM = new ArrayList<Integer>();
      current_MMAxis = new ArrayList<Integer>();
    }
    else{ MMIndex+=10; }
    newDataMM = false;
  }
  
  if(newDataMF){
    for(int i = 0; i < currentMF.size(); i++){
      fill(80, 220, 100);
      circle(current_MFAxis.get(i), 280 - currentMF.get(i), 5);
    }
    if(MFIndex + xPos > 1210){
      MFIndex = 0;
      currentMF = new ArrayList<Integer>();
      current_MFAxis = new ArrayList<Integer>();
    }
    else{ MFIndex+=10; }
    newDataMF = false;    
  }
  
  if(newDataLF){
    for(int i = 0; i < currentLF.size(); i++){
      fill(255, 175, 0);
      circle(current_LFAxis.get(i), 280 - currentLF.get(i), 5);
    }
    if(LFIndex + xPos > 1210){
      LFIndex = 0;
      currentLF = new ArrayList<Integer>();
      current_LFAxis = new ArrayList<Integer>();
    }
    else{ LFIndex+=10; }
    newDataLF = false;
  }

//--------------------User info display area-----------------------------------
  fill(0);
 
  textSize(32);
  text("Step Count: ", 400, 200); text(step_count, 700, 200);
  text("Step Length: ", 400, 250); text(step_length, 700, 250);
  text("Stride Length: ", 400, 300); text(stride_length, 700, 300);
  time = hour + ":" + minute + ":" + second;
  text("Time: ", 400, 350); text(time, 700, 350);   
  text("Cadence: ", 400, 400); text(speed, 700, 400);
  text("Gait: ", 400, 450); text(diagnoses, 700, 450);
  text(walking, 400, 600);
  text("Updates:", 400, 730);
  textSize(20);
  text(message,400,770); 
}

//=============================================================================================

//Capture serial information
void serialEvent (Serial port) { 
   try{ 
    //read in string until newline
    String inString = port.readStringUntil('\n');
    
    if (inString != null) {
      //remove whitespace
      inString = trim(inString); 
      line =  splitTokens(inString);
  
  
      if(line[0].indexOf("MM:") != -1){
        mm = Integer.valueOf(line[1]);
        mm = int(map(mm, 0, 1023, 10, 150));
        currentMM.add(int(map(mm, 10, 150, 0, 50)));
        current_MMAxis.add(900 + MMIndex);
        newDataMM = true;
      }
      if(line[0].indexOf("MF:") != -1){
        mf = Integer.valueOf(line[1]);
        mf = int(ceil(map(mf, 0, 1023, 10, 150)));
        currentMF.add(int(map(mf,10,150,0, 50)));
        current_MFAxis.add(1250 + MFIndex);
        newDataMF = true;
      }
      if(line[0].indexOf("LF:") != -1){
        lf = Integer.valueOf(line[1]);
        lf = int(ceil(map(lf, 0, 1023, 10, 150)));
        currentLF.add(int(map(lf,10,150,0, 50)));
        current_LFAxis.add(900 + LFIndex);
        newDataLF = true;
      }
      if(line[0].indexOf("HEEL:") != -1){
        heel = Integer.valueOf(line[1]);
        heel = int(ceil(map(heel, 0, 1023, 10, 150)));
        currentHEEL.add(int(map(heel,10,150,0, 50)));
        current_HEELAxis.add(1250 + HEELIndex);
        newDataHEEL = true;
      }   
      if(line[0].indexOf("STEPCOUNT:") != -1){
        step_count = line[1];
      }  
      if(line[0].indexOf("HOUR:") != -1){
        hour = line[1];
      }
      if(line[0].indexOf("SECOND:") != -1){
        second = line[1];
      }
      if(line[0].indexOf("MINUTE:") != -1){
        minute = line[1];
      }
      if(line[0].indexOf("S3:") != -1){
        if(line[1].equals("0")){walking = "Standing Still";}
        else{walking = "In Motion!";}
      }
    }//END OF if(inString != NULL)
    else{
        println("Serial is null");
    }
   }
   catch(RuntimeException e){
     e.printStackTrace();
   }
}
//=============================================================================================
//Button functions
void normalgait(){
  println("Normal gait button press");
  message = "Calculating normal gait MFP";
  port.write("N");
}
void intoe(){
  println("In-Toe gait button press");
  message = "Calculating in-toe gait MFP";
  port.write("I");
}
void outtoe(){
  println("Out-Toe gait button press");
  message = "Calculating out-toe gait MFP";
  port.write("O");
}
void heel(){
  println("Heel gait button press");
  message = "Calculating heel gait MFP";
  port.write("H");
}
void tiptoe(){
  println("Tiptoe gait button press");
  message = "Calculating tiptoe gait MFP";
  port.write("T");
}
void configure(){
  println("Configure motion button press");
  message = "Updates will display here...";
  port.write("C");
}
void reset(){
  println("Reset button press");
  port.write("R");
  
  step_count = "0";
  step_length = "0";
  stride_length = "0";
  time = "0";
  speed = "0";
  diagnoses = "Unknown";
  message = "Updates will display here";
  
}
void diagnose(){
  println("Diagnose button press");
  message = "Diagnosing...";
  port.write("D");
}
