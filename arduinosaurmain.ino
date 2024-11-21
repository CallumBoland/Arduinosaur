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
const float maxDistance = 110, minDistance = 10; //rear
const float maxForwardDistance = 110, minForwardDistance = 10; //front
  //behaviours and goals
const float lurchDistance = 60; //rear
const float retreatThreshold = 40; //rear
const float curiosityDistance = 50; //front
  //motors
const float positionTolerance = 2.5; //tolerance = +-positionTolerance

  //tail
const int tailMin = 0, tailMax = 90;
const int tailSteps = 20;
  //neck
const int neckMin = 0, neckMax = 90;
  //jaw
const int jawOpenAngle = 90;
const int jawClosedAngle = 0;

//globals
float frontDistance = 0;
float rearDistance = 0;
  //motors
float desiredPosition = 50;
float forwardSpeed = 1;
int goal = 0; /*goals:
                0: creep
                1: retreat
                2: lurch   
              */
  //tail
int tailAmplitude = 0;
int tailStep = 0;
  //neck
float neckAngle = 0;
  //eyes
float brightness = 0;
  //jaw
long lastBite = -6000;
bool jawOpen = false;
bool biteCheck = false;


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
  loopTail(); //done
  loopNeck(); //done
  loopJaw(); //?
  loopEyes(); //done
  loopMotors(); //done?

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
  //amplitude proportional to aggression
  //frequency constant
  tailAmplitude = int((tailMax-tailMin)*aggression);
  int angle = tailMin + tailAmplitude*(float(abs(tailStep-tailSteps*0.5))/tailSteps);
  tailServo.write(angle);
  //increment
  tailStep = (tailStep+1)%tailSteps;
}

void loopNeck(){
  if(curiosity >= 0.5) { // increase based on curiosity
    neckAngle = ((curiosity-0.5)*2) * (neckMax-neckMin) + neckMin;
    neckAngle = constrain(neckAngle, neckMin, neckMax); // Constraining just in case
  }
  else{
    neckAngle = neckMin;
  }
  //move
  neckServo.write(neckAngle);
}

void loopJaw(){
  if(!bitecheck && millis() >= lastBite+6000){//3 second cooldown
    return;
  }
  //open mouth at max curiosity
  if(curiosity == 1 && !jawOpen){
    openJaw();
  }
  //wait until tongue is pressed
  else if(digitalRead(biteInputPin)){
    //bite down for 3 seconds or until tongue released
    if(!biteCheck){
      lastBite = millis();
      biteCheck = true;
      closeJaw();
      digitalWrite(biteOutputPin,HIGH); //update bite counter
    }
    else if(millis() >= lastBite+3000 || !digitalRead(biteInputPin)){
      openJaw();
      digitalWrite(biteOutputPin,LOW);
      biteCheck = false;
    }
  }
  else{
    closeJaw();
  }
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
  //update distances
  updateUltrasound(10,0);
  //Behaviours:
  switch(goal){
    case 0: //creep forward
      desiredPosition = maxDistance;
      forwardSpeed = 0.5;
      fear = 0; //consult the graph
      aggression = constrain((frontDistance-curiosityDistance)/(maxForwardDistance-curiosityDistance),1,0)
      curiosity = constrain(1-aggression,1,0)


      //only update goals when creeping
      if(frontDistance < minForwardDistance){
        if(rearDistance => retreatThreshold){
          goal = 1; //retreat
          fear = 1;
        }
        else{
          goal = 2; //lurch
          aggression = 1;
        }
      }
      break;

    case 1: //retreat back
      forwardSpeed = 1;
      desiredPosition = minDistance;
      if(rearDistance <= minDistance + positionTolerance){
        goal = 0; //return to creeping when position achieved
      }
      break;

    case 2: //lurch forward
      desiredPosition = lurchDistance;
      forwardSpeed = 1;
      if(rearDistance >= minDistance - positionTolerance){
        goal = 0; //return to creeping when position achieved
      }
      break;

    default: //just incase
      goal = 0;
      break;
  }
  //update move
  move();



  /* legacy
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
  */
}

void updateUltrasound(int delay1, int delay2){
  frontDistance = 0.017 * ultrasound(trigPinF,echoPinF);
  delay(delay1);
  rearDistance = 0.017 * ultrasound(trigPinR,echoPinR);
  delay(delay2);
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

void move(){
  float localDesiredPosition = desiredPosition;
  //clamp input
  if(localDesiredPosition>maxDistance){
    localDesiredPosition = maxDistance;
  }
  else if(localDesiredPosition<minDistance){
    localDesiredPosition = minDistance;
  }

  //forwards
  if(localDesiredPosition < (rearDistance - positionTolerance)){
    digitalWrite(backwardPin, LOW);
    analogWrite(forwardPin, int(255*forwardSpeed));
  }
  //backwards
  else if(localDesiredPosition > (rearDistance + positionTolerance)){
    digitalWrite(forwardPin, LOW);
    digitalWrite(backwardPin, HIGH);
  }
  //no movement
  else{
    stop();
  }
}

void stop(){
  digitalWrite(forwardPin, LOW);
  digitalWrite(backwardPin, LOW);
}

void openJaw(){
  jawOpen = true;
  jawServo.write(jawOpenAngle);
}

void closeJaw(){
  jawOpen = false;
  jawServo.write(jawClosedAngle);
}

