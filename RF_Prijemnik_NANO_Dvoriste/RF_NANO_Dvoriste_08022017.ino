/*
    Prijem signala sa daljinskih upravljaca 
    - konvertovanje u dvocifren broj i slanje na softverski serijski port  A1-RX,  A0-TX
    - Posebni kodovi za posebne daljince

  29.09.2016.
  08.02.2017. - salje na serijski port kod i na kraju slovo E
  
  Simple example for receiving
  https://github.com/sui77/rc-switch/
  
  
*/

#include <RCSwitch.h>
#include <SoftwareSerial.h>
SoftwareSerial mySerial(A1, A0); // A1- RX,  A0- TX

RCSwitch mySwitch = RCSwitch();
char buf[15];

byte countDigits(long int number){
  byte count=0;
  while(number){
    number=number/10;
    count++;
  }
  return count;
}

void blink() {                  //  Blink diode pri svakom regularno primljenom Kodu
    digitalWrite(13, HIGH);
    delay(50);
    digitalWrite(13, LOW); 
      }


void setup() {
  
  pinMode(13, OUTPUT);    // Impulsni izlaz - Diodica na ploci NANO
  
  pinMode(7, OUTPUT);     // Izlaz za svetlo kada stigne signal sa PIR senzora na dvoristu
  
  pinMode(8, OUTPUT);     //A
  pinMode(9, OUTPUT);     //B
  pinMode(10, OUTPUT);    //C
  pinMode(11, OUTPUT);    //D
  pinMode(12, OUTPUT);    //E

  digitalWrite(13, LOW);
  
  digitalWrite(7, HIGH);
  digitalWrite(8, HIGH);
  digitalWrite(9, HIGH);
  digitalWrite(10, HIGH);
  digitalWrite(11, HIGH);
  digitalWrite(12, HIGH);
 
  Serial.begin(9600);
  mySerial.begin(9600);
  
  mySwitch.enableReceive(0);  // Receiver on interrupt 1 => that is pin D3
  
  pinMode(3, INPUT_PULLUP); // RF-ulaz
}

void loop() {
  if (mySwitch.available()) {
    
  unsigned long value = mySwitch.getReceivedValue();
    
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

ltoa(value, buf, 10);
byte k = countDigits(value);
buf[k]='E';

mySerial.write(buf);

Serial.println(k);
Serial.println(buf);

blink();
/*
      switch (value) {
    case 16762196:              //AAAAAAAA -ON
         digitalWrite(8,LOW);
         Serial.print("A-ON ");
         mySerial.write(11); 
         blink(); 
      break;
      
    case 16762193:               //AAAAAAAA -OFF
         digitalWrite(8,HIGH);
         Serial.print("A-OFF ");
         mySerial.write(10);
         blink();
      break;
  
    case 16765268:            //BBBBBBB - ON
         digitalWrite(9,LOW);        
         Serial.print("B-ON ");
         mySerial.write(21);
         blink();
    break;

    
    case 16765265:              //BBBBBBB - OFF
         digitalWrite(9,HIGH);
         Serial.print("B-OFF ");
         mySerial.write(20);
         blink();
    break;

    case 16766036:              //CCCCCCCC - ON
         digitalWrite(10,LOW);
         Serial.print("C-ON");
         mySerial.write(31);
         blink();
    break;

    case 16766033:            //CCCCCCCC - OFF
         digitalWrite(10,HIGH);
         Serial.print("C-OFF");
         mySerial.write(30);
         blink();
    break;

    case 16766228:              //DDDDDDDD - ON
         digitalWrite(11,LOW);
         Serial.print("D-ON ");
         mySerial.write(41);
         blink();
    break;
    
    case 16766225:            //DDDDDDDDD - OFF
         digitalWrite(11,HIGH);
         Serial.print("D-OFF ");
         mySerial.write(40);
         blink();
    break;

    case 16766276:              // EEEEEEEEEE - ON
         digitalWrite(12,LOW);
         Serial.print("E-ON ");
         mySerial.write(51);
         blink();
    break;
    
    case 16766273:            // EEEEEEEE - OFF
         digitalWrite(12,HIGH);
         Serial.print("E-OFF ");
         mySerial.write(50);
         blink();
    break;
    
    case 4822273:             //  Mali daljinski (taster A)
         digitalWrite(8,LOW);
         Serial.print("A - Mali daljinski ");
         mySerial.write(61); 
         blink(); 
    break;
      
   case 4822274:             //  Mali daljinski (taster B)
         digitalWrite(8,HIGH);
         Serial.print("B - Mali daljinski ");
         mySerial.write(71);
         blink();
   break; 
      
   case 4822276:             //  Mali daljinski (taster C)
         digitalWrite(9,LOW);        
         Serial.print("C - Mali daljinski ");
         mySerial.write(60);
         blink();
   break;
   
   case 4822280:              //  Mali daljinski (taster D)
         digitalWrite(9,HIGH);
         Serial.print("D - Mali daljinski ");
         mySerial.write(70);
         blink();
   break;
   
   case  123451:              //  Prijem koda sa PIR senzora ispod TERASE ( kod za ON  )
         Serial.print("PIR senzor - neko se seta na dvoristu");
         digitalWrite(7,LOW);
         mySerial.write(81);
         blink();
   break;

   case  123450:              //  Prijem koda sa PIR senzora ispod TERASE ( kod za OFF )
         Serial.print("PIR senzor - ugasen");
         digitalWrite(7,HIGH);
         mySerial.write(80);
         blink();
   break;

      
  }
  */
      }
       
    mySwitch.resetAvailable();
    delay(100);
  }
}
