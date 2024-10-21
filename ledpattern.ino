const int ledPins[] = {5,6,9,10,11};
int ledBrightness[5];
const int brightnessDifference = 40;
const int brightnessStep = 16;

const int minBright = 16;
const int maxBright = 256;

void setup() {
  // put your setup code here, to run once:
  for(int i=0; i<5; i++){
    pinMode(ledPins[i],OUTPUT);
    ledBrightness[i] = (minBright+(i*brightnessDifference)%(maxBright-minBright));
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  for(int o=0; o<5; o++){ //brightness adjustment
    ledBrightness[o] = minBright+(ledBrightness[o]+brightnessStep)%(maxBright-minBright);
    analogWrite(ledPins[o],ledBrightness[o]);
    delay(2);
  }
  delay(40);
}
