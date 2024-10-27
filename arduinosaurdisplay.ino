//libraries
#include <LiquidCrystal.h>

//pins
const int shiftDataPin = 11, shiftLatchPin = 12, shiftClockPin = 13;
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
const int transistorSwitchPin = 8;
const int bitePin = 9;

//LCD
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
//variables
int value = 0;
int values[] = {0,0,0,0};
long lastBite = 0;
bool biteCheck = false;

//lookups
const int segmentLookup[10] = {63,6,91,79,102,109,125,7,127,111};
const int digitLookup[4] = {224,208,176,112};
const int digitHigh = 240;

void setup() {
  //pinmodes
  pinMode(shiftDataPin,OUTPUT);
  pinMode(shiftLatchPin,OUTPUT);
  pinMode(shiftClockPin,OUTPUT);
  pinMode(transistorSwitchPin,OUTPUT);
  pinMode(bitePin,OUTPUT);

  //LCD setup
  digitalWrite(transistorSwitchPin, HIGH);
  lcd.begin(16, 2);
  lcd.print("Dinosaur Attacks");
  lcd.setCursor(0, 1);
  lcd.print("Today:"); 
  digitalWrite(transistorSwitchPin, LOW);
  lcd.setCursor(0, 0);
  lcd.print("Last attack was:");
  lcd.setCursor(0, 1);
  lcd.print("9999 Seconds Ago");

}

void loop() {
  //bite testing
  if(digitalRead(bitePin) && !biteCheck){
    value += 1;
    lastBite = millis();
    biteCheck = true;
  }
  else if(!digitalRead(bitePin) && biteCheck){
    biteCheck = false;
  }

  //seven segment update
  values[0] = (value % 10000)/1000;
  values[1] = (value % 1000)/100;
  values[2] = (value % 100)/10;
  values[3] = (value % 10)/1;
  for(int digit=0; digit<4; digit++){
    digitalWrite(shiftLatchPin, LOW);
    shiftOut(shiftDataPin, shiftClockPin, MSBFIRST, segmentLookup[values[digit]]);
    shiftOut(shiftDataPin, shiftClockPin, MSBFIRST, digitLookup[digit]);
    digitalWrite(shiftLatchPin, HIGH);
    delay(1);
  }
  digitalWrite(shiftLatchPin, LOW);
  shiftOut(shiftDataPin, shiftClockPin, MSBFIRST, 0);
  shiftOut(shiftDataPin, shiftClockPin, MSBFIRST, digitHigh);
  digitalWrite(shiftLatchPin, HIGH);

  //LCD updates
  lcd.print(((millis()-lastBite)%1000) + " Seconds Ago");

  //delay
  delay(5);
}