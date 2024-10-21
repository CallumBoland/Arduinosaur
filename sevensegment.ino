//pins
int pinA =12;//11
int pinB =11;//7
int pinC =10;//4
int pinD =9;//2
int pinE =8;//1
int pinF =7;//10
int pinG =6;//5
int pinDp =5;//3
int pinDig1 =4;//12
int pinDig2 =3;//9
int pinDig3 =2;//8
int pinDig4 =1;//6
int pins[] = {pinA,pinB,pinC,pinD,pinE,pinF,pinG,pinDp,pinDig1,pinDig2,pinDig3,pinDig4};
int digits[] = {pinDig1,pinDig2,pinDig3,pinDig4};

//variables
int value = 0;
int values[] ={0,0,0,0};
int decimalPoint=-1;

//lookups (fuck me)
const bool segmentLookup[10][7] = {
  {1,1,1,1,1,1,0},
  {0,1,1,0,0,0,0},
  {1,1,0,1,1,0,1},
  {1,1,1,1,0,0,1},
  {0,1,1,0,0,1,1},
  {1,0,1,1,0,1,1},
  {1,0,1,1,1,1,1},
  {1,1,1,0,0,0,0},
  {1,1,1,1,1,1,1},
  {1,1,1,1,0,1,1}};
//segmentLookup[digit][pin]

void setup() {
  // put your setup code here, to run once:
  pinMode(13,INPUT);
  for(int i = 0;i<12;i++){
    pinMode(pins[i],OUTPUT);
  }
    for(int i=0; i<4;i++){
      digitalWrite(digits[i],HIGH);
    }
}

void loop() {
  // put your main code here, to run repeatedly:
  values[0] = (value % 10000)/1000;
  values[1] = (value % 1000)/100;
  values[2] = (value % 100)/10;
  values[3] = (value % 10)/1;
  for(int digit=0; digit<4;digit++){
    
    digitalWrite(digits[digit],LOW);

    for(int pin=0; pin<7; pin++){
      digitalWrite(pins[pin],segmentLookup[values[digit]][pin]);
    }

    if(digit==decimalPoint){
      digitalWrite(pinDp,HIGH);
    }
    else{
      digitalWrite(pinDp,LOW);
    }
    delay(1);
    digitalWrite(digits[digit],HIGH);
    for(int i=0; i<7;i++){
      digitalWrite(pins[i],LOW);
    }
    delay(1);
  }
  delay(1);
  if (digitalRead(13)==LOW){
    value+=1;
  }


}
