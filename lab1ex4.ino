
int ledPin = 11;
int buttonPin = 10;
bool buttonPress = false;

void setup() {
  // put your setup code here, to run once:
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  buttonPress = digitalRead(buttonPin) == HIGH;

  digitalWrite(ledPin, digitalRead(buttonPin));
  
}
