//libraries
#include <Servo.h>

//pins
const int trigPin = 12, jawPin = 11, neckPin = 10, tailPin = 9, echoPin = 7, redPin = 6, greenPin =5, bluePin = 3, biteOutputPin = 2;
const int biteInputPin = A0;

//servos
Servo tailServo;
Servo neckServo;
Servo jawServo;

//constants
const long loopPeriod = 50;
const float maxForwardDistance = 70, minForwardDistance = 10; //front
  //behaviours and goals
const float curiosityDistance = 50; //front
  //motors
//gone
  //tail
const int tailMin = 0, tailMax = 70;
const long tailPeriod = 1000;
const int tailSteps = tailPeriod/loopPeriod;
  //neck
const int neckMin = 120, neckMax = 90;
const int neckFearAngle = 135;
  //jaw
const int jawOpenAngle = 0;
const int jawClosedAngle = 180;

//globals
float frontDistance = 0;
  //motors
//gone
  //tail
int tailAmplitude = 0;
int tailStep = 0;
  //neck
float neckAngle = 0;
  //eyes
//none
  //jaw
long lastBite = -6000;
bool jawOpen = false;
bool biteCheck = false;


//behaviours
bool fear = false;
float aggression = 0;
float curiosity = 0;

//setup
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
  loopJaw(); //done
  loopEyes(); //done
  loopMotors(); //done

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
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
}

void setupMotors(){
  pinMode(trigPin, OUTPUT); // Sets the trigPin as OUTPUT 
  pinMode(echoPin, INPUT); // Sets the echoPin as INPUT
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
  if(biteCheck){
    return; //change nothing if biting
  }

  if(fear){ //fear response in neck
    neckAngle = neckFearAngle;
  }
  else if(curiosity >= 0.5) { // lean down based on curiosity
    neckAngle = ((curiosity-0.5)*2) * (neckMax-neckMin) + neckMin;
    neckAngle = constrain(neckAngle, neckMin, neckMax); // Constraining just in case
  }
  else{ //otherwise
    neckAngle = neckMin;
  }
  //move
  neckServo.write(neckAngle);
}

void loopJaw(){
  if(!biteCheck && millis() >= lastBite+6000){//3 second cooldown
    return;
  }
  //open mouth at max curiosity
  if(curiosity >= 1 && !biteCheck){
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
    if(millis() >= lastBite+3000 || !digitalRead(biteInputPin)){
      openJaw();
      digitalWrite(biteOutputPin,LOW);
      biteCheck = false;
    }
  }
  else{ //close otherwise
    closeJaw();
  }
}

void loopEyes(){
  //eyes turn more cyan proportional to curiosity
  //eyes turn more red proportional to aggression
  //eyes turn purple for fear
  if(fear){
    //purple 
    analogWrite(redPin,76);
    analogWrite(greenPin,0);
    analogWrite(bluePin,196);
  }
  else{
    //redness
    analogWrite(redPin,(255*(1-curiosity)));
    //cyanness
    analogWrite(greenPin,(255*(1-aggression)));
    analogWrite(bluePin,(255*(1-aggression)));
  }
}

void loopMotors(){//no more motors lol
  //update distances
  updateUltrasound();
  //Behaviours:
  if(frontDistance>=minForwardDistance){
      //aggression and curiosity
      /*consult the graph:
      aggression:____/\_
       curiosity:_|⎺⎺\__
            fear:⎺|_____
      */
      if(frontDistance >= curiosityDistance && frontDistance <= maxForwardDistance){
        if(frontDistance > (curiosityDistance + maxForwardDistance)/2){
          aggression = constrain((frontDistance-curiosityDistance)/((curiosityDistance + maxForwardDistance)/2),1,0);
          curiosity = constrain(1-aggression,1,0);
        }
        else{
          aggression = 1-constrain((frontDistance-(curiosityDistance + maxForwardDistance)/2)/((curiosityDistance + maxForwardDistance)/2),1,0);
          curiosity = 0;
        }
      }
      else if(frontDistance < curiosityDistance){
        aggression = 0;
        curiosity = 1;
      }
      else{
        aggression = 0;
        curiosity = 0;
      }
      fear = false;
  }//fear point
  else{
    aggression = 0;
    curiosity = 0;
    fear = true;
  }

}

void updateUltrasound(){
  //updates proximity
  frontDistance = 0.017 * ultrasound(trigPin,echoPin);
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

void openJaw(){
  //opens jaw
  jawOpen = true;
  jawServo.write(jawOpenAngle);
}

void closeJaw(){
  //closes jaw
  jawOpen = false;
  jawServo.write(jawClosedAngle);
}

