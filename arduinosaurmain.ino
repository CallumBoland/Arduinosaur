//libraries
#include <Servo.h>

//pins
const int trigPinF = 13, trigPinR = 12, jawPin = 11, neckPin = 10, tailPin = 9, echoPinF = 8, 
echoPinR = 7, redPin = 6, greenBluePin = 5, backwardPin = 4, forwardPin = 3, biteOutputPin = 2;
const int ldrPin = A0, biteInputPin = A1;

//servos
Servo tailServo;
Servo neckServo;
Servo jawServo;

//constants
const long loopPeriod = 50;
const float maxDistance = 110;
const float minDistance = 10;
  //tail
const int tailmin = 0, tailmax = 90;
  //neck
const int neckMin = 0, neckMax = 90;
const long baseNeckPeriod = 1000;

//variables
float frontDistance = 0;
float rearDistance = 0;
  //tail
float tailFrequency = 0;
float tailAmplitude = 0;
long tailHighMillis = 0;
long tailLowMillis = tailHighMillis + baseNeckPeriod/2;
long tailCurrentPeriod = baseNeckPeriod;
  //neck
float neckAngle = 0;
  //eyes
float brightness = 0;

//behaviours
float fear = 0
float aggression = 0
float curiosity = 0


void setup() {
  //setups
  setupTail();
  setupNeck();
  setupJaw();
  setupEyes();
  setupMotors();
}

void loop() {
  //loops
  loopTail(); //in progress
  loopNeck(); //complete
  loopJaw(); //?
  loopEyes(); //done
  loopMotors(); //needs rework

  //constant delay
  delay(loopPeriod-(millis()%loopPeriod));
}

//setups
void setupTail(){
  tailServo.attach(tailPin);
}

void setupNeck(){
  neckServo.attach(neckPin);
}

void setupJaw(){
  jawServo.attach(jawPin);
}

void setupEyes(){
  pinMode(redPin, OUTPUT);
  pinMode(greenBluePin, OUTPUT);
  pinMode(ldrPin, INPUT);
}

void setupMotors(){
  pinMode(forwardPin, OUTPUT); 
  pinMode(backwardPin, OUTPUT);
  pinMode(trigPinF, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(trigPinR, OUTPUT); 
  pinMode(echoPinF, INPUT); // Sets the echoPin as an INPUT
  pinMode(echoPinR, INPUT); 
}
//loops
void loopTail(){
  setTailParameters();
  if()



}
void loopNeck(){
 if (minDistance < rearDistance && rearDistance < maxDistance / 2) { // Do jack until over half distance
    neckAngle = neckMin;
  } else if (rearDistance >= maxDistance / 2) { // Incriment Angle until at max dist
    neckAngle = map(rearDistance, maxDistance / 2, maxDistance, neckMin, neckMax);
    neckAngle = constrain(neckAngle, neckMin, neckMax); // Constraining just in case
  } else if (rearDistance > maxDistance) { // More constraining just incase
    neckAngle = neckMax;
  } else {
    neckAngle = neckMin;
  }
  //move
  neckServo.write(neckAngle);
}

void loopJaw(){
  
}

void loopEyes(){
  //led brightness proportional to environment brightness
  //eyes turn more cyan proportional to curiosity
  //eyes turn more red proportional to aggression
  brightness = (analogRead(ldrPin)/1023.0);
  analogWrite(redPin,(255*brightness*(1-curiosity)))
  analogWrite(greenBluePin,(255*brightness*(1-aggression)))
}

void loopMotors(){
  frontDistance = 0.017 * ultrasound(trigPinF,echoPinF);
  delay(20);
  rearDistance = 0.017 * ultrasound(trigPinR,echoPinR);
  delay(20);

  //Behaviours:
  //In the case the dinosaur is 'cornered', it'll lurch forward as if threatened
  if(frontDistance < 20 && rearDistance < 20){
    digitalWrite(forwardPin, HIGH);
    digitalWrite(backwardPin, LOW);
  }
  //Or gate to move forward if curious, or if being approached from behind
  else if(frontDistance > 20 || rearDistance < 20){
    digitalWrite(forwardPin, HIGH);
    digitalWrite(backwardPin, LOW);
  }
 //Backs off if too close but space to flee (and gate not needed as firs if statement cover this)
  else if(frontDistance < 20){ 
    digitalWrite(forwardPin, LOW);
    digitalWrite(backwardPin, HIGH);
  }

}

long ultrasound(int trigger, int echo){
  digitalWrite(trigger, LOW);
  delayMicroseconds(2);
  // Sets the trigger HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  return(pulseIn(echo, HIGH));
}

void setTailParameters{
  if(fear > curiosity + aggression){
    //frequency = proportional to 1-fear
    //amplitude = proportional to fear
    tailFrequency = (1-fear)
    tailAmplitude = fear
  }
  else if(curiosity > aggression + fear){
    //frequency = proportional to curiosity
    //fixed amplitude
    tailFrequency = curiosity
    tailAmplitude = 0.5
  }
  else if(aggression > fear + curiosity){
    //amplitude = proportional to aggression
    //fixed frequency
    tailFrequency = 0.5
    tailAmplitude = aggression
  }
}