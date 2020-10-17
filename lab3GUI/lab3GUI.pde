import processing.serial.*;
import controlP5.*;

//Globals
ControlP5 cp5;
ControlTimer timer;
Serial port;
PFont font;
PFont button_font;

//user stats
String step_count = "0";
String step_length = "0";
String time = "0";
String speed = "0";
String walking = "Not Walking";
String diagnoses = "Unknown";

//input line from Serial
String[] line;
String mode;
PImage foot_img;

//Graph variables
int xPos = 900;
int yPos = 450;
int xEdge = 1500;
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
  size(1600,800);
  foot_img = loadImage("foot.png");
  
  // Create the font
  font = createFont("SourceCodePro-Regular.ttf", 36);
  textFont(font);
  frameRate(25);
  smooth();
  cp5 = new ControlP5(this);
  

  button_font = createFont("SourceCodePro-Regular.ttf", 20);
  // create a new button with name 'buttonA'
  cp5.addButton("normalgait")
   .setLabel("Set Normal Gait")
   .setPosition(900,500)
   .setSize(250,30)
   .setFont(button_font)
   ;
  cp5.addButton("intoe")
   .setLabel("Set In-Toe Gait")
   .setPosition(900,540)
   .setSize(250,30)
   .setFont(button_font)
   ;
  cp5.addButton("outtoe")
   .setLabel("Set Out-Toe Gait")
   .setPosition(900,580)
   .setSize(250,30)
   .setFont(button_font)
   ;
  cp5.addButton("tiptoe")
   .setLabel("Set Tip-Toe Gait")
   .setPosition(900,620)
   .setSize(250,30)
   .setFont(button_font)
   ;
  cp5.addButton("heel")
   .setLabel("Set Heel Gait")
   .setPosition(900,660)
   .setSize(250,30)
   .setFont(button_font)
   ;
  cp5.addButton("diagnose")
   .setLabel("Diagnose")
   .setPosition(1200,500)
   .setSize(250,70)
   .setFont(font)
   ;
  cp5.addButton("reset")
   .setLabel("Reset")
   .setPosition(1200,620)
   .setSize(250,70)
   .setFont(font)
   ;
}

//=============================================================================================

void draw() {
  
  background(181,211,231);
  
  fill(200);
  rect(890, 490, 280, 210, 7);
  
  fill(0);
  textSize(50);
  text("Welcome to 'Watch Your Step' aka WYS", 400, 80);
  
//------------foot imaging area------------------------------------------------
  image(foot_img, 10, 50, width/4.5, 700);
  
  //draw circle for heel, RED color
  fill(255,0,0);
  circle(200, 650, heel);
  
  //draw circle for mm, BLUE color
  fill(0, 0, 255);
  circle(280, 350, mm);
  
  //draw circle for mf, GREEN color
  fill(0, 255, 0);
  circle(200, 230, mf);
  
  //draw circle for lf, ORANGE color
  fill(255, 165, 0);
  circle(140, 430, lf);
  
//-----------------------Area for graph----------------------------------------
  fill(0);
  rect(900, 150, 610, 300, 7);
  
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
  text("Time: ", 400, 300); text(time, 700, 300);   
  text("Speed: ", 400, 350); text(speed, 700, 350);
  text("Gait: ", 400, 400); text(diagnoses, 700, 400);
  
  textSize(40);
  text(walking, 400, 600);

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
    if(mode.equals("P:")){
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
    
    if(mode.equals("S1:")){
    }
    if(mode.equals("S2:")){
    }
    if(mode.equals("S3:")){
    }
    
  }//END OF if(inString != NULL)
}
//=============================================================================================
//Button functions
void normalgait(){
  println("Normal gait button press");
}
void intoe(){
  println("In-Toe gait button press");
}
void outtoe(){
  println("Out-Toe gait button press");
}
void heel(){
  println("Heel gait button press");
}
void tiptoe(){
  println("Tiptoe gait button press");
}
//=============================================================================================
