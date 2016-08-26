/*
  Simple example for receiving
  https://github.com/sui77/rc-switch/
*/

#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();

void blink() {
  
    digitalWrite(7, LOW);
    delay(50);
    digitalWrite(7, HIGH); 
}


void setup() {
  
  pinMode(7, OUTPUT);     // Impulsni izlaz
  pinMode(8, OUTPUT);     //A
  pinMode(9, OUTPUT);     //B
  pinMode(10, OUTPUT);    //C
  pinMode(11, OUTPUT);    //D
  pinMode(12, OUTPUT);    //E

  digitalWrite(7, HIGH);
  digitalWrite(8, HIGH);
  digitalWrite(9, HIGH);
  digitalWrite(10, HIGH);
  digitalWrite(11, HIGH);
  digitalWrite(12, HIGH);
 
  Serial.begin(9600);
  
  mySwitch.enableReceive(1);  // Receiver on interrupt 0 => that is pin #2
  
  pinMode(3, INPUT_PULLUP); // RF-ulaz
}

void loop() {
  if (mySwitch.available()) {
    
    int value = mySwitch.getReceivedValue();
    
    if (value == 0) {
      Serial.print("Unknown encoding");
    } else {
      Serial.print("Received ");
      Serial.println( mySwitch.getReceivedValue() );
//      Serial.print(" / ");
//      Serial.print( mySwitch.getReceivedBitlength() );
//      Serial.print("bit ");
//      Serial.print("Protocol: ");
//      Serial.println( mySwitch.getReceivedProtocol() );

      switch (value) {
    case 16762196:            //AAAAAAAAAAAAAAAAA
         digitalWrite(8,LOW);
         Serial.print("A-ON "); 
         blink(); 
      break;
    case 16762193:
         digitalWrite(8,HIGH);
         Serial.print("A-OFF ");
         blink();
      break;
    case 16765268:            //BBBBBBBBBBBBBBBBB
         digitalWrite(9,LOW);        
         Serial.print("B-ON ");
         blink();
      break;
    case 16765265:
         digitalWrite(9,HIGH);
         Serial.print("B-OFF ");
         blink();
    break;

  case 16766036:              //CCCCCCCCCCCCCCCCCC
         digitalWrite(10,LOW);
         Serial.print("C-ON");
         blink();
      break;
    case 16766033:
         digitalWrite(10,HIGH);
         Serial.print("C-OFF");
         blink();
    break;

  case 16766228:            //DDDDDDDDDDDDDDDDDDDDDD
         digitalWrite(11,LOW);
         Serial.print("D-ON ");
         blink();
      break;
    case 16766225:
         digitalWrite(11,HIGH);
         Serial.print("D-OFF ");
         blink();
    break;

  case 16766276:            //EEEEEEEEEEEEEEEEEEEEEEEE
         digitalWrite(12,LOW);
         Serial.print("E-ON ");
         blink();
      break;
    case 16766273:
         digitalWrite(12,HIGH);
         Serial.print("E-OFF ");
         blink();
    break;
   
  }
      }
       
    mySwitch.resetAvailable();

    delay(500);
  }
}
