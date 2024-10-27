//pins
const int shiftDataPin = 11, shiftLatchPin = 12, shiftClockPin = 13;
//variables
int value = 0000;
int values[] ={0,0,0,0};
int decimalPoint=-1;

//lookups (fuck me)
const int segmentLookup[10] = {63,6,91,79,102,109,125,7,127,111};
const int digitLookup[4] = {224,208,176,112};
const int digitHigh = 240;
//segmentLookup[digit][pin]

void setup() {
  // put your setup code here, to run once:
  pinMode(shiftDataPin,OUTPUT);
  pinMode(shiftLatchPin,OUTPUT);
  pinMode(shiftClockPin,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  value = (value + 1) % 10000;
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
  delay(10);
}