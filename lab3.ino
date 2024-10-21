

int sensorPin = A0;   // select the input pin for the potentiometer
int ledPin = 11;      // select the pin for the LED
int sensorValue = 0;  // variable to store the value coming from the sensor
float sensorMax = 500;
int ledMax = 255;
int servoMax = 180;

#include <Servo.h>
Servo myServo;
int pos = 0;

void setup() {
  // declare the ledPin as an OUTPUT:
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
  myServo.attach(9);
}

void loop() {
  // read the value from the sensor:
  sensorValue = analogRead(sensorPin);
  float output = sensorValue/sensorMax;
  if(output>1){
    output=1;
  }
  Serial.println(output);
  // turn the ledPin on
  analogWrite(ledPin, int(output*sensorMax) );

  myServo.write(int(output*servoMax)); // tell servo to go to position in variable 'pos'
  delay(15); // waits 15ms for the servo to reach the position
}
