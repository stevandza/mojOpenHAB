/*
    Prijem signala sa daljinskih upravljaca 
    - konvertovanje u dvocifren broj i slanje na softverski serijski port  A1-RX,  A0-TX
    - Posebni kodovi za posebne daljince
    
  29.09.2016.
  28.10.2016. - Dodati i kodovi za male transmittere
  
  Simple example for receiving
  https://github.com/sui77/rc-switch/
  
  
*/

#include <RCSwitch.h>
#include <SoftwareSerial.h>
SoftwareSerial mySerial(A1, A0); // A1- RX,  A0- TX

RCSwitch mySwitch = RCSwitch();

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

  pinMode(A2, OUTPUT);    //  Izlaz za Cesmu na dvoristu

  digitalWrite(13, LOW);
  
  digitalWrite(7, HIGH);
  digitalWrite(8, HIGH);
  digitalWrite(9, HIGH);
  digitalWrite(10, HIGH);
  digitalWrite(11, HIGH);
  digitalWrite(12, HIGH);

  digitalWrite(A2, HIGH);
  
 
  Serial.begin(9600);
  mySerial.begin(9600);
  
  mySwitch.enableReceive(0);  // Receiver on interrupt 0 => that is pin D2
  
  pinMode(2, INPUT_PULLUP); // RF-ulaz
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

  case  8548993:              //  Ukljucenje cesme na dvoristu
         Serial.print("Cesma je ukljucena");
         digitalWrite(A2,LOW);
         mySerial.write(82);
         blink();
   break;
   
    case  8548994:              //  Iskljucena je cesma na dvoristu
         Serial.print("Cesma je iskljucena");
         digitalWrite(A2,HIGH);
         mySerial.write(83);
         blink();
   break;
   

   case  8549457:              
         Serial.print("Dvorisna rasveta ON");
         mySerial.write(12);
         blink();
   break;

   case  8549458:              
         Serial.print("Dvorisna rasveta OFF");
         mySerial.write(13);
         blink();
   break;

   case  8549464:              
         Serial.print("Svetlo ispred garaze ON");
         mySerial.write(14);
         blink();
   break;
   case  8549460:              
         Serial.print("Svetlo ispred garaze OFF");
         mySerial.write(15);
         blink();
   break;
    case  8549459:              
         Serial.print("Svetlo ispred kuce ON");
         mySerial.write(16);
         blink();
     break;
    case  8549465:              
         Serial.print("Svetlo ispred kuce OFF");
         mySerial.write(17);
         blink();
     break;



     case  8549461:              
         Serial.print("Rez 1");
         mySerial.write(18);
         blink();
     break;

     case  8549466:              
         Serial.print("Rez 2");
         mySerial.write(19);
         blink();
     break;

     case  8549462:              
         Serial.print("Rez 3");
         mySerial.write(22);
         blink();
     break;

     case  8549468:              
         Serial.print("Rez 4");
         mySerial.write(23);
         blink();
     break;

     case  15565793:              
         Serial.print("Rez 5");
         mySerial.write(32);
         blink();
     break;

     case  15565794:              
         Serial.print("Rez 6");
         mySerial.write(33);
         blink();
     break;

     
      
  }
      }
       
    mySwitch.resetAvailable();
    delay(100);
  }
}
