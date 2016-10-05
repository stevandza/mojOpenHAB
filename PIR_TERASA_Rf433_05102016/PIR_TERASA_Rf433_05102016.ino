/*
    Program za slanje signala prisustva osobe na dvoristu 
    sa PIR senzora putem veze 433Mhz
    
    Senzor salje kodove ON: 123451   ;  OFF: 123450
    Senzor takodje salje stanje senzora na svakih 15 sekundi !
    
    Kontakt relea sa PIR senzora vezan je izmedju ulaza D2 i GND 
    TX izlaz je pin A0 
    Led dioda (pin 13) trepne svaki put pti prenosu signala
 
  Example for different sending methods
  https://github.com/sui77/rc-switch/
  
*/

#include <RCSwitch.h>

#define PIR_ULAZ   2   // D2 je ulaz za pir senzor (prema GND)
#define TX_IZLAZ   A0
#define PCB_LED    13

int pirNovo;
int pirStaro;

long lastMsg  = 0;
int  value    = 0;

RCSwitch mySwitch = RCSwitch();

void setup() {

  pinMode(PIR_ULAZ, INPUT_PULLUP); 
  pinMode(PCB_LED, OUTPUT); 
  
  Serial.begin(9600);
  
  mySwitch.enableTransmit(TX_IZLAZ);

  // Optional set pulse length.
  // mySwitch.setPulseLength(320);
  
  // Optional set protocol (default is 1, will work for most outlets)
     mySwitch.setProtocol(1);
  
  // Optional set number of transmission repetitions.
  // mySwitch.setRepeatTransmit(15);
  
}

void loop() {

  pirNovo = digitalRead(PIR_ULAZ);
  
if ( pirStaro != pirNovo ) {    //  Slanje ukoliko se neko iznenada pojavi

    if ( pirNovo == 0)  {       //Kada je svetlo ukljuceno posalji 2 uzastopna koda

    digitalWrite(PCB_LED, HIGH); 
    mySwitch.send(123451, 24);   // IMA NEKOG
    delay(2000);
    mySwitch.send(123451, 24);
    delay(1000);
    digitalWrite(PCB_LED, LOW);
          }
          else {  
    digitalWrite(PCB_LED, HIGH); 
    mySwitch.send(123450, 24);    // NEMA NIKOG
    delay(2000);
    mySwitch.send(123450, 24);
    delay(1000);
    digitalWrite(PCB_LED, LOW);
               }
}
  pirStaro = pirNovo;

         // Redovno slanje stanja senzora na nekoliko minuta

 long now = millis();
     if (now - lastMsg > 10000) {
      lastMsg = now;
       ++value;
       
  if ( pirNovo == 0)  {       //Kada je svetlo ukljuceno posalji 2 uzastopna koda

    digitalWrite(PCB_LED, HIGH); 
    mySwitch.send(123451, 24);   // IMA NEKOG
    delay(500);
    digitalWrite(PCB_LED, LOW);
          }
          else {  
    digitalWrite(PCB_LED, HIGH); 
    mySwitch.send(123450, 24);    // NEMA NIKOG
    delay(500);
    digitalWrite(PCB_LED, LOW);
               }

  }

    
  /* See Example: TypeA_WithDIPSwitches */
 // mywitch.switchOn("11111", "00010");
 // delay(1000);
 // mySwitch.switchOn("11111", "00010");
 // delay(1000);



  /* Same switch as above, but using binary code */
  // mySwitch.send("000000000001010100010001");
  //  delay(1000);  
  //  mySwitch.send("000000000001010100010100");
  //  delay(1000);

  /* Same switch as above, but tri-state code */ 
  // mySwitch.sendTriState("00000FFF0F0F");
  // delay(1000);  
  // mySwitch.sendTriState("00000FFF0FF0");
  // delay(1000);

  // delay(500);
}
