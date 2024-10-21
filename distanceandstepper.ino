#include "Stepper.h"
#define echoPin 2 // attach pin D2 Arduino to pin Echo of HC-SR04
#define trigPin 3 //attach pin D3 Arduino to pin Trig of HC-SR04
// defines variables
long duration; // variable for the duration of sound wave travel
int distance; // variable for the distance measurement
int desiredAngle;
float currentAngle = 0;
const int stepsPerRevolution = 2048;
Stepper myStepper = Stepper(stepsPerRevolution, 8, 10, 9, 11);



void setup() {
 pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
 pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT
 Serial.begin(9600); // // Serial Communication is starting with 9600 of baudrate speed
 Serial.println("Jhole"); 
 myStepper.setSpeed(5);
}
void loop() {
  // Clears the trigPin condition
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.017; // Speed of sound wave divided by 2 (go and back)
  // Displays the distance on the Serial Monitor // Displays the distance on the Serial Monitor
  desiredAngle = (float(distance-30)/20)*90;
  if(desiredAngle>90){
    desiredAngle=90;
  }
  else if(desiredAngle<-90){
    desiredAngle=-90;
  }
  int steps = int((desiredAngle-currentAngle)*(stepsPerRevolution/360));
  myStepper.step(steps);
  currentAngle = desiredAngle;
  Serial.print(distance);
  Serial.print(" ");
  Serial.print(currentAngle);
  Serial.print(" ");
  Serial.println(desiredAngle);
  delay(500);
}
