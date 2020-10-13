#define fsr1 A1
#define fsr2 A2
#define fsr3 A3
#define fsr4 A4


//Define variable to store sensor readings:
int fsr1reading; 
int fsr2reading; 
int fsr3reading; 
int fsr4reading; 

int ledR = 8;
int ledY = 7;
int ledG = 6;
int ledB = 5;

void setup() {
  // Begin serial communication at a baud rate of 9600:
  Serial.begin(9600);
  pinMode(ledR, OUTPUT);
  pinMode(ledY, OUTPUT);
  pinMode(ledG, OUTPUT);
  pinMode(ledB, OUTPUT);
}
  void loop() {

//reads FSR pin and stores output as fsr_reading
    fsr1reading = analogRead(fsr1);
    fsr2reading = analogRead(fsr2);
    fsr3reading = analogRead(fsr3);
    fsr4reading = analogRead(fsr4);

digitalWrite(ledR, HIGH); 
digitalWrite(ledY, HIGH); 
digitalWrite(ledG, HIGH); 
digitalWrite(ledB, HIGH); 

Serial.print("FSR 1: ");

Serial.println(fsr1reading);


Serial.print("FSR 2: ");

Serial.println(fsr2reading);

Serial.print("FSR 3: ");

Serial.println(fsr3reading);

Serial.print("FSR 4: ");

Serial.println(fsr4reading);

delay(500);
    
  }
