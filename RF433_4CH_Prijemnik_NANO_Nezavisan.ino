/*
    Prijem signala sa daljinskih upravljaca 
    -Prizemlje
    -Prvi sprat

  11.08.2017
  
  Simple example for receiving
  https://github.com/sui77/rc-switch/
  
  
*/

#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();

unsigned long int value;

void blink() {                  //  Blink diode pri svakom regularno primljenom Kodu
    digitalWrite(13, HIGH);
    delay(50);
    digitalWrite(13, LOW); 
      }


void setup() {
  
  pinMode(2, INPUT_PULLUP); // RF-ulaz
  
  pinMode(13, OUTPUT);    // Impulsni izlaz - Diodica na ploci NANO
  digitalWrite(13, LOW);

  
  pinMode(7, OUTPUT);     //  Rele 1 - Dvorisna rasveta
  pinMode(8, OUTPUT);     //  Rele 2 - Svetlo ispred garaze
  pinMode(9, OUTPUT);     //  Relw 3
  pinMode(10, OUTPUT);    //  Rele 4
  
  
  
  digitalWrite(7, HIGH);
  digitalWrite(8, HIGH);
  digitalWrite(9, HIGH);
  digitalWrite(10, HIGH);

 
  Serial.begin(9600);
 
  mySwitch.enableReceive(0);  // Receiver on interrupt 0 => that is pin D2
  
  
}

void loop() {


  if (mySwitch.available()) {
    
  value = mySwitch.getReceivedValue();
    
    if (value == 0) {
      Serial.print("Unknown encoding");
    } 
    
    else {
      Serial.println("");
      Serial.print("Received ");
      Serial.println(value);
      
//    Serial.println( mySwitch.getReceivedValue() );
    
//    Serial.print(" / ");
//    Serial.print( mySwitch.getReceivedBitlength() );
//    Serial.print("bit ");
//    Serial.print("Protocol: ");
//    Serial.println( mySwitch.getReceivedProtocol() );




      switch (value) {
        
    case 11143156:              //Rele 1
         digitalWrite(7,LOW);
         Serial.print("Rele 1-ON ");
         blink(); 
      break;
      
    case 11143160:               //Rele 1
         digitalWrite(7,HIGH);
         Serial.print("Rele 1-OFF ");
         blink();
      break;
  
    case 11143158:               //Rele 2
         digitalWrite(8,LOW);        
         Serial.print("Rele 2-ON ");
         blink();
    break;

    
    case 11143161:              // Rele 2
         digitalWrite(8,HIGH);
         Serial.print("Rele 2-OFF ");
         blink();
    break;

    case 11143154:              //Rele 3
         digitalWrite(9,LOW);
         Serial.print("Rele 3-ON");
         blink();
    break;

    case 11143153:               //Rele 3
         digitalWrite(9,HIGH);
         Serial.print("Rele 3-OFF");
         blink();
    break;

    case 14004856:              //Rele 1
         digitalWrite(7,LOW);
         Serial.print("Rele 1-ON ");
         blink(); 
      break;
      
    case 14004852:               //Rele 1
         digitalWrite(7,HIGH);
         Serial.print("Rele 1-OFF ");
         blink();
      break;
  
    case 14004850:               //Rele 2
         digitalWrite(8,LOW);        
         Serial.print("Rele 2-ON ");
         blink();
    break;

    
    case 14004849:              // Rele 2
         digitalWrite(8,HIGH);
         Serial.print("Rele 2-OFF ");
         blink();
    break;

    case 14004858:              //Rele 3
         digitalWrite(9,LOW);
         Serial.print("Rele 3-ON");
         blink();
    break;

    case 14004853:               //Rele 3
         digitalWrite(9,HIGH);
         Serial.print("Rele 3-OFF");         
         blink();
    break;

    
    
    case 12975090:                          //Rele 1  - togle
       
       if ( digitalRead(7) == LOW  ) 
                  { digitalWrite(7,HIGH); }
       else 
                  { digitalWrite(7,LOW); }
       blink(); 
       delay (1000);
      break;
      
    case 12975089:                           //Rele 2 - togle
        if ( digitalRead(8) == LOW  ) 
                  { digitalWrite(8,HIGH); }
       else 
                  { digitalWrite(8,LOW); }
       blink(); 
       delay (1000);
      break;
      break;
  
    case 12975096:                          //Rele 3 - togle
         if ( digitalRead(9) == LOW  ) 
                  { digitalWrite(9,HIGH); }
       else 
                  { digitalWrite(9,LOW); }
       blink(); 
       delay (1000);
      break;
    break;

      
  }
 
      }
       
    mySwitch.resetAvailable();
    delay(100);
  }
}
