#include <IRremote.h>
int RECV_PIN = 13;
IRrecv receiver(RECV_PIN);
decode_results results;

String codeLookup[] = {"PWR","VOL+","FUNC/STOP","|<<","PAUSE",">>|","DOWN","VOL-","UP","0","EQ","ST/REPT","1","2","3","4","5","6","7","8","9","repeat","error"};
int irLookup(long code){
  switch (code){
    case 4294967295:
      return(21);
    case 16753245:
      return(0);
    case 16736925:
      return(1);
    case 16769565:
      return(2);
    case 16720605:
      return(3);
    case 16712445:
      return(4);
    case 16761405:
      return(5);
    case 16769055:
      return(6);
    case 16754775:
      return(7);
    case 16748655:
      return(8);
    case 16738455:
      return(9);
    case 16750695:
      return(10);
    case 16756815:
      return(11);
    case 16724175:
      return(12);
    case 16718055:
      return(13);
    case 16743045:
      return(14);
    case 16716015:
      return(15);
    case 16726215:
      return(16);
    case 16734885:
      return(17);
    case 16728765:
      return(18);
    case 16730805:
      return(19);
    case 16732845:
      return(20);
    default:
      return(22);
  }
}


void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  receiver.enableIRIn();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (receiver.decode(&results)) {
    Serial.println(codeLookup[irLookup(results.value)]);//  Print the code as a hexadecimal value
    receiver.resume();
  }
  //Serial.println(digitalRead(dataPin));
}
