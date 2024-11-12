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
const int tailMin = 0, tailMax = 90;
  //neck
const int neckMin = 0, neckMax = 90;
const long baseTailPeriod = 1000;

//globals
float frontDistance = 0;
float rearDistance = 0;
  //tail
float tailFrequencyMult = 0;
float tailAmplitudeMult = 0;
long tailPeriod = baseTailPeriod;
int tailAmplitude = 0;
int tailSteps = tailPeriod/loopPeriod;
int tailStep = 0;
  //neck
float neckAngle = 0;
  //eyes
float brightness = 0;

//behaviours
float fear = 0;
float aggression = 0;
float curiosity = 0;

//setups
void setup() {
  setupTail();
  setupNeck();
  setupJaw();
  setupEyes();
  setupMotors();
}

//main loop
void loop() {
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
  //write angle
  int angle = tailMin + tailAmplitude*(float(abs(tailStep-tailSteps*0.5))/tailSteps);
  tailServo.write(angle);

  //recalculate frequency and amplitude each cycle
  if(tailStep == 0){
    setTailParameters();
    tailAmplitude = int((tailMax-tailMin)*tailAmplitudeMult);
    tailPeriod = baseTailPeriod/tailFrequencyMult;
    tailPeriod = tailPeriod-(tailPeriod%(2*loopPeriod)); //constrain to even multiples of loop period
    tailSteps = tailPeriod/loopPeriod;
  }
  //increment
  tailStep = (tailStep+1)%tailSteps;
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
  analogWrite(redPin,(255*brightness*(1-curiosity)));
  analogWrite(greenBluePin,(255*brightness*(1-aggression)));
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

void setTailParameters(){
  if(fear > curiosity + aggression){
    //frequency = proportional to 1-fear
    //amplitude = proportional to fear
    tailFrequencyMult = (1-fear);
    tailAmplitudeMult = fear;
  }
  else if(curiosity > aggression + fear){
    //frequency = proportional to curiosity
    //fixed amplitude
    tailFrequencyMult = curiosity;
    tailAmplitudeMult = 0.5;
  }
  else if(aggression > fear + curiosity){
    //amplitude = proportional to aggression
    //fixed frequency
    tailFrequencyMult = 0.5;
    tailAmplitudeMult = aggression;
  }
}
