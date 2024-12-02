//libraries
#include <Servo.h>

//pins
const int trigPin = 12, jawPin = 11, neckPin = 10, tailPin = 9, echoPin = 7, redPin = 6, greenPin =5, bluePin = 3;
const int biteInputPin = 4, biteOutputPin = 2;

//servos
Servo tailServo;
Servo neckServo;
Servo jawServo;

//constants
const long loopPeriod = 200;
const float maxForwardDistance = 112, minForwardDistance = 12; //front
  //behaviours and goals
const float curiosityDistance = 42; //front
  //motors
//gone
  //tail
const int tailMin = 60, tailMax = 15;
const int tailFearAngle = 0;
const long tailPeriod = 2000;
const int tailSteps = tailPeriod/loopPeriod;
  //neck
const int neckLow = 20, neckHigh = 50;
const int neckFearAngle = 55;
const int neckSteps = 6;
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
int neckStep = 0;
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
  Serial.begin(9600); //for testing
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

  Serial.println(frontDistance);
  Serial.println(digitalRead(biteInputPin));


  //constant delay
  delay(loopPeriod-(millis()%loopPeriod));
}

//setups
void setupTail(){
  tailServo.attach(tailPin);
  tailServo.write(tailMin);
}

void setupNeck(){
  neckServo.attach(neckPin);
  neckServo.write(neckHigh);
}

void setupJaw(){
  jawServo.attach(jawPin);
  closeJaw();
  pinMode(biteOutputPin,OUTPUT);
  pinMode(biteInputPin,INPUT);
  digitalWrite(biteInputPin,HIGH);
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
  if(fear){
    tailServo.write(tailFearAngle);
    return;
  }
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
    neckStep = -5;
    neckAngle = neckFearAngle;
  }
  else{ //discrete steps in angle
    neckAngle = neckStepping(curiosity);
  }
  //move
  neckServo.write(neckAngle);
}

void loopJaw(){
  if(!biteCheck && millis() <= lastBite+6000){//3 second cooldown
    return;
  }
  //open mouth at max curiosity
  if(neckStep == neckSteps-1 || biteCheck){
    if(!biteCheck){
      openJaw();
    //wait until tongue is pressed
      if(digitalRead(biteInputPin)){
      //bite down for 3 seconds if not already biting
        lastBite = millis();
        biteCheck = true;
        closeJaw();
        digitalWrite(biteOutputPin,HIGH); //update bite counter
      }
    }
    else{
      //release bite if biting for more than 3 seconds
      if(millis() >= lastBite+3000){
        openJaw();
        digitalWrite(biteOutputPin,LOW);
        biteCheck = false;
      }
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
    analogWrite(redPin,(255*(aggression)));
    //cyanness
    analogWrite(greenPin,(255*(curiosity)));
    analogWrite(bluePin,(255*(curiosity)));
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
        float range = maxForwardDistance-curiosityDistance;
        if(frontDistance <= (curiosityDistance+range/2)){
          aggression = (frontDistance-curiosityDistance)/(range/2);
          aggression = constrain(aggression,0,1);
          curiosity = 1-aggression;
          curiosity = constrain(curiosity,0,1);
        }
        else{
          aggression = 1-((frontDistance-(curiosityDistance + range/2))/(range/2));
          aggression = constrain(aggression,0,1);
          curiosity = 0;
        }
      }
      else if(frontDistance < curiosityDistance){
        aggression = 0;
        curiosity = 1;
      }
      else{
        fear = false;
        aggression = 0;
        curiosity = 0;
      }
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

int neckStepping(float c){
  if(c>=0.5){
    if(c>=0.5+(0.5/(neckSteps-1))*(neckStep+1)){
      neckStep+=1;
    }
    else if(c<=0.5+(0.5/(neckSteps-1))*(neckStep-1)){
      neckStep+=-1;
    }
    neckStep = constrain(neckStep,-5,neckSteps-1);
    return(neckHigh-(neckHigh-neckLow)*(neckStep/(neckSteps-1.0)));
  }
  else{
    return(neckHigh);
  }



}
