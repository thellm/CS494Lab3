import processing.serial.*;
import controlP5.*;

//Globals
ControlP5 cp5;
ControlTimer timer;
Serial port;
PFont font;
PFont button_font;
PFont button_font2;

//user stats
String step_count = "0";
String step_length = "0";
String stride_length = "0";
String time = "0";
String speed = "0";
String walking = "";
String diagnoses = "Unknown";
String message = "";

//input line from Serial
String[] line;
String mode;
PImage foot_img;

//Graph variables
int xPos = 900;
int yPos = 450;
int xEdge = 1600;
int Index;
boolean newData = false;

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

//=============================================================================================

void setup() {
  
  printArray(Serial.list());
  port = new Serial(this, "COM5", 115200); //make sure to change to your port connected to device
  //port = new Serial(this, "/dev/cu.usbmodem123456781", 115200); //make sure to change to your port connected to device
  port.bufferUntil('\n');
  
   foot_img = loadImage("foot.png");
  
  // Create the font
  font = createFont("SourceCodePro-Regular.ttf", 36);
  textFont(font);
  frameRate(25);
  smooth();
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
  
  fill(200,0,0);                //covers set gait button area
  rect(900, 470, 280, 210, 7); 
      
  fill(100,220,100);            //covers diagnose button area
  rect(1235, 600, 370, 60, 7);
  
  fill(0,100,200);              //covers reset/configure button area
  rect(1235, 470, 370, 110, 7);
  
  fill(240,240,100);            //cover instructions
  rect(900, 695, 705, 120,7);
  
  fill(250);                    //updates area
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
  
  //draw circle for heel, RED color
  fill(255,0,0);
  circle(200, 700, heel);
  
  //draw circle for mm, BLUE color
  fill(0, 0, 255);
  circle(280, 400, mm);
  
  //draw circle for mf, GREEN color
  fill(80, 220, 100);
  circle(200, 280, mf);
  
  //draw circle for lf, ORANGE color
  fill(255, 165, 0);
  circle(140, 480, lf);
  
//-----------------------Area for graph----------------------------------------
  fill(235);
  rect(900, 150, 705, 300, 7);
  
  fill(255,0,0);
  
  if(xPos + Index > xEdge) {
    Index = 0;
  }
  circle(xPos + Index, yPos - heel, 10); //heel


  fill(0, 0, 255);
  circle(xPos + Index, yPos - mm, 10); //mm
  

  fill(0, 255, 0);
  circle(xPos + Index, yPos - mf, 10); //mf

  fill(255, 165, 0);
  circle(xPos + Index, yPos - lf, 10); //lf
  Index+=1;
//--------------------User info display area-----------------------------------
  fill(0);
 
  textSize(32);
  text("Step Count: ", 400, 200); text(step_count, 700, 200);
  text("Step Length: ", 400, 250); text(step_length, 700, 250);
  text("Stride Length: ", 400, 300); text(stride_length, 700, 300);
  text("Time: ", 400, 350); text(time, 700, 350);   
  text("Cadence: ", 400, 400); text(speed, 700, 400);
  text("Gait: ", 400, 450); text(diagnoses, 700, 450);
  text(walking, 400, 600);
  text("Updates:", 400, 730);
  textSize(20);
  text(message,400,770); 
  text("Updates will diplasy here....",400,770);
}

//=============================================================================================

//Capture serial information
void serialEvent (Serial port) { 
  //read in string until newline
  String inString = port.readStringUntil('\n');
   
  if (inString != null) {
    //remove whitespace
    inString = trim(inString); 
    line =  splitTokens(inString);
  
  
    mode = line[0];
  
    //pressure mode
    if(mode.indexOf("P:") != -1){
      //pressure values in order of MF LF MM Heel 
      mf = Integer.valueOf(line[1]);
      mf = int(ceil(map(mf, 0, 1023, 10, 130)));
      
      lf = Integer.valueOf(line[2]);
      lf = int(ceil(map(lf, 0, 1023, 10, 130)));
      
      mm = Integer.valueOf(line[3]);
      mm = int(ceil(map(mm, 0, 1023, 10, 130)));
      
      heel = Integer.valueOf(line[4]);
      heel = int(ceil(map(heel, 0, 1023, 10, 130)));
    }   
    if(mode.indexOf("S1:") != -1){
      step_length = line[1];
      stride_length = line[2];
      speed = line[3];
      step_count = line[4];
    }
    if(mode.indexOf("S2:") != -1){
      message = "";
      //if next element if F then final diagnoses follows
      if(line[1].indexOf("F") != -1) { 
        diagnoses = line[2];
        message = "The final diagnoses is " + line[2];
      }
      else {
        //get the message
        for(int i = 1; i < line.length; i++ ){
          message = message + " " + line [i]; 
        }
      }
    }
    if(mode.indexOf("S3:") != -1){
      if(line[1].indexOf("0")!=-1){ //not walking
        message = "You are resting";
      }
      else if(line[1].indexOf("1")!=-1) { //walking
        walking = "You are walking";
        time = line[2];
      }
    }
    
  }//END OF if(inString != NULL)
  else{
      println("Serial is null");

  }
}
//=============================================================================================
//Button functions
void normalgait(){
  println("Normal gait button press");
  port.write("N");
}
void intoe(){
  println("In-Toe gait button press");
  port.write("I");
}
void outtoe(){
  println("Out-Toe gait button press");
  port.write("O");
}
void heel(){
  println("Heel gait button press");
  port.write("H");
}
void tiptoe(){
  println("Tiptoe gait button press");
  port.write("T");
}
void configure(){
  println("Configure motion button press");
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
  message = "";
  
}
void diagnose(){
  println("Diagnose button press");
  port.write("D");
}

//=============================================================================================
