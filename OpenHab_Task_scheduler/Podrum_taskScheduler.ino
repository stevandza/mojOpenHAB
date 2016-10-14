/*
 Reconnecting MQTT example - non-blocking
 This sketch demonstrates how to keep the client connected
 using a non-blocking reconnect function. If the client loses
 its connection, it attempts to reconnect every 5 seconds
 without blocking the main loop.
 03.06.2016. - SA E5CN-a
 15.07.2016.
 19.07.2016. - Probano radi sve ! Dodat je i izlaz 36 sa Arduino Mega na Rele4 ali se ne koristi
 15.08.2016 -  Dodato i dobro ocitavanje pritiska vode 0-25bar.
 10.10.2016 -  Izbrisano javljanje Open hab-u sa OPEN i CLOSED
Probano i radi bez diskonektovanja !!!!! :P
*/

#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SoftwareSerial.h>
#include <TaskScheduler.h>
#include <Wire.h>     
#include "DHT.h"


void naVreme1();  // na 5 sekundi
void naVreme2();  // na 1 minut

Task t1(5000,  TASK_FOREVER, &naVreme1);
Task t2(10000, TASK_FOREVER, &naVreme2);

Scheduler runner;

// IZLAZI
#define PODRUM_SVETLO           22
#define PODRUM_VENTILACIJA      24
#define PODRUM_GREJANJE         26 
#define PODRUM_JUNE_ZADAVANJE   2    

// ULAZI
#define PODRUM_PROZOR                    28
#define PODRUM_NIVO_VODE_GREJANJA        A0   
#define PODRUM_PRITISAK_VODOVOD          A1
#define PODRUM_DHTPIN                    31  // DHT11 - Senzor vlage i temperature
#define PODRUM_TEMP_GREJANJE             38  // Senzor DS18B20
#define PODRUM_KVALIET_VAZDUHA           A2  // Senzor za gas MQ-2


// SWF , 9600, 8 Bit, 1 Stop, NoNe Parity, 1 Address 
#define TX_ENABLE_PIN 35         // define the EIA-485 transmit driver pin ( RE + DE pin's modules)
SoftwareSerial rs485(30, 34);    // SoftwareSerial rs485(RO, DI); pins name on the module

#define DHTTYPE  DHT22
DHT dht(PODRUM_DHTPIN, DHTTYPE);


OneWire oneWire(PODRUM_TEMP_GREJANJE);
DallasTemperature DS18B20(&oneWire);

byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 30);
IPAddress server(192, 168, 1, 100);

char msg1[10], msg2[10], msg3[10]; // msg1- temperatura vode grejanja  , msg2- vlaznost podruma , msg3- temperatura podruma

float temp;
float tempOld = 0;

float vodaPice;

char vazduhCh[5], vodaGrejCh[5], vodaPiceCh[5];

int ko1=1;      // Kontakt prozora u podrumu
int old_ko1=0;

unsigned long lastTime;
char buffer[10];
int Tpomocni, TpomocniStaro;

//******* E5CN ****************************************************

   char msgE [] = {'@','0','1','W','S','0','1', '1','6','3','2','4','2','*','\r'}; // podatak za temperaturu 163.2 C za testiranje    
   char msgRbuf[50];
   byte crcR;
   int  dig1,dig2,dig3,dig4, u;
   int  aUlaz=0, aUlazStaro ; 

//************************************************************************************
EthernetClient ethClient;
PubSubClient client(ethClient);

void naVreme1() {                                 // PODACI KOJI SE SALJU NA 5 SEKUNDI

//****** Ocitavanje sa DHT11 senzora *************
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t) ) {
    delay(20);
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  dtostrf(h,4,2,msg2);
  Serial.print("Vlaznost podruma je (DHT): ");
  Serial.println(msg2);
  client.publish("kuca/vlaPod",msg2);

  dtostrf(t,4,2,msg3);
  Serial.print("Temperatura podruma je (DHT): ");
  Serial.println(msg3);
  client.publish("kuca/tempPod",msg3);


//****** Saljem Kvalitet vazduha u podrumu   ************* 
  int vazduh = analogRead(PODRUM_KVALIET_VAZDUHA) / 10.24;  // Skalirano od 0 - 100%
  dtostrf(vazduh,4,2,vazduhCh);
//  snprintf (vazduhCh, 75, "%ld", vazduh);
  Serial.print("Kvalitet vazduha u podrumu  je:  ");
  Serial.println(vazduhCh);
  client.publish("kuca/kvaPod", vazduhCh); 
 
  
//****** Saljem Nivo vode grejanja u podrumu ************* 
  float vodaGrej = analogRead(PODRUM_NIVO_VODE_GREJANJA) / 10.24;  // Skalirano od 0 - 100%
  dtostrf(vodaGrej,4,2,vodaGrejCh);
  Serial.print("Nivo vode zagrejanje u podrumu  je:  ");
  Serial.println(vodaGrejCh);
  client.publish("kuca/nivoVoGrej", vodaGrejCh); 

  
//****** Saljem Pritisak vode za pice u podrumu *************   
 if (analogRead(PODRUM_PRITISAK_VODOVOD)>204) {
  
  float vodaPice = (analogRead(PODRUM_PRITISAK_VODOVOD) - 204) / 32.76; // Skalirano od 0 - 25bar
   
  dtostrf(vodaPice,4,2,vodaPiceCh);
  Serial.print("Pritisak vode za pice u podrumu  je:  ");
  Serial.println(vodaPiceCh);
  client.publish("kuca/pritVoPice", vodaPiceCh);
      }

else  {

  vodaPice = 0;                                        // U koliko je struja manja od 4mA
  dtostrf(vodaPice,4,2,vodaPiceCh);
  Serial.print("Pritisak vode za pice u podrumu  je:  ");
  Serial.println(vodaPiceCh);
  client.publish("kuca/pritVoPice", vodaPiceCh);
  
}

//****** Saljem temperaturu grejanja peci JUNE ************* 
          do {
          DS18B20.requestTemperatures(); 
          temp = DS18B20.getTempCByIndex(0);
//        Serial.print("Temperatura grejanja je : ");
//        Serial.println(temp);
          delay(10);
        } while (temp == 85.0 || temp == (-127.0));

        
  if (tempOld != temp) {
         dtostrf(temp,4,2,msg1);
         Serial.print("");
         Serial.print("Temperatura vode grejanja je: ");
         Serial.println(msg1);
         client.publish("kuca/tempVoGrej",msg1);
         }
      tempOld = temp;
}


void naVreme2() {                          // PODACI KOJI SE APLODUJU SA HAB-A NA 1 MINUT
        client.subscribe("kuca/Podrum/#"); 
                }

void callback(char* topic, byte* payload, unsigned int length) {
    int i=0;
    char message_buff[100];

       for (int i = 0; i < length; i++) {
            message_buff[i] = (char)payload[i];
                 }

    payload[length] = '\0';
    String strPayload = String((char*)payload);
   
   Serial.println("");
   Serial.print(String((char*)topic));
   Serial.print("[");
   Serial.print(strPayload);
   Serial.println("]");


 if (String((char*)topic) == "kuca/Podrum/sv1") {         // Svetlo u podrumu
      if (strPayload == "on") { 
        digitalWrite(PODRUM_SVETLO,LOW);
        Serial.println("Ukljuceno je svetlo u podrumu");
                              }
       else       {         
        digitalWrite(PODRUM_SVETLO,HIGH);  
        Serial.println("Iskljuceno je svetlo u podrumu");
                  }
 }

if (String((char*)topic) == "kuca/Podrum/sv2") {         // Ventilacija u podrumu
      if (strPayload == "on") { 
        digitalWrite(PODRUM_VENTILACIJA,LOW);
        Serial.println("Ukljucena je ventilacija u podrumu");
                              }
       else       {         
        digitalWrite(PODRUM_VENTILACIJA,HIGH);  
        Serial.println("Iskljucena je ventilacija u podrumu");
                  }
 }


if (String((char*)topic) == "kuca/Podrum/sv3") {         // Grejanje / Pec JUNE
      if (strPayload == "on") { 
        digitalWrite(PODRUM_GREJANJE,LOW);
        Serial.println("Ukljuceno je grejanje peci JUNE");
                              }
       else       {         
        digitalWrite(PODRUM_GREJANJE,HIGH);  
        Serial.println("Iskljuceno je grejanje peci JUNE");
                  }
 }


 if (String((char*)topic) == "kuca/Podrum/VoGrejSet") {  // Zadavanje temperature - kotao JUNE

       Serial.println("");
       Tpomocni = strPayload.toInt();
       Serial.print("Zadata temperatura vode grejanja - kotao JUNE : ");
       Serial.print(strPayload.toInt());
       Serial.println("");             
           } 
}

boolean reconnect() {
  while (!client.connected()) {
          Serial.println("Pokusavam da se konektujem na MQTT...");
          client.connect("Klijent_Podrum");
          delay(10);
    if (client.connect("Klijent_Podrum")) {
        Serial.println("konektovan");
        client.publish("Klijent_Podrum", "Klijent PODRUM je konektovan");
        client.subscribe("kuca/Podrum/#");
    } else {
      Serial.print("greska, rc=");
      Serial.print(client.state());
      Serial.println(" pokusacu ponovo za 2 sekunde");
      // Wait 2 seconds before retrying
      delay(2000);
    }
  }
return client.connected();

}

void setup()
{
   pinMode(PODRUM_SVETLO,         OUTPUT);
   pinMode(PODRUM_VENTILACIJA,    OUTPUT);
   pinMode(PODRUM_GREJANJE,       OUTPUT); 
   pinMode(PODRUM_JUNE_ZADAVANJE, OUTPUT);     

   pinMode(PODRUM_PROZOR,               INPUT_PULLUP);      
   pinMode(PODRUM_NIVO_VODE_GREJANJA,   INPUT);      
   pinMode(PODRUM_PRITISAK_VODOVOD,     INPUT);      
   pinMode(PODRUM_DHTPIN,               INPUT_PULLUP);      
   pinMode(PODRUM_KVALIET_VAZDUHA,      INPUT); 
   pinMode(PODRUM_TEMP_GREJANJE,        INPUT); 

// inicijalizacija izlaza - Ugasi sve izlaze
  digitalWrite(PODRUM_SVETLO,         HIGH);
  digitalWrite(PODRUM_VENTILACIJA,    HIGH);
  digitalWrite(PODRUM_GREJANJE,       HIGH);
  digitalWrite(PODRUM_JUNE_ZADAVANJE, HIGH);
  
  pinMode(TX_ENABLE_PIN, OUTPUT);   // driver output enable
  rs485.begin (9600);               // vitesse com

  Serial.begin(115200);
  
  client.setServer(server, 1883);
  client.setCallback(callback);
  Ethernet.begin(mac, ip);
  delay(1500);

  runner.init();
  runner.addTask(t1);
  runner.addTask(t2);
  
 
// Posalji trenutno stanje digitalnih ulaza na openHAB

ko1 = digitalRead(PODRUM_PROZOR);
if ( ko1 == 1)  { client.publish("kuca/Podrum/ko1", "OPEN");
                  Serial.println("SETUP : Podrumski prozor je otvoren ");
                }    
     else        {   client.publish("kuca/Podrum/ko1", "CLOSED");
                     Serial.println("SETUP : Podrumski prozor je zatvoren ");
                 }
}

void loop() {

runner.execute();   // startuj petlje za slanje podataka na odredjeno vreme.

// ***** TEMP E5CN *********
      byte crcR =0;
      char charBuf[16];
    
      u=0; 
      
     dig1= 0;
     dig2= Tpomocni / 10;
     dig3= Tpomocni % 10;
     dig4= 0;
     
//      Serial.print(dig2);  
//      Serial.print(dig3);    

      msgE[7] =  '48';
      msgE[8] =  dig2+48;
      msgE[9] =  dig3+48;
      msgE[10]=  '48';

// ********************* CRC kalkulacija *************************   

  byte m = msgE[0] ^ msgE[1]^ msgE[2] ^ msgE[3] ^ msgE[4] ^ msgE[5] ^ msgE[6] ^ msgE[7] ^ msgE[8] ^ msgE[9] ^ msgE[10];  // Decimalno
  
  String m1=String(m,HEX);
  m1.toUpperCase();

  String msgT1= String("@01WS01");
  String msgT2= String(dig1);  // broj 0
  String msgT3= String(dig2);  // prva cifra temperature
  String msgT4= String(dig3);  // druga cifra temperature
  String msgT5= String(dig4);  // broj 0
  String msgT6= String("*\r\0");
 
  String msgT= msgT1 + msgT2 + msgT3 + msgT4 + msgT5 +m1 +msgT6 ;

  msgT.toCharArray(charBuf, 16) ;

//******** Zadavanje  TEMPERATURE ******************************
if (Tpomocni != TpomocniStaro)  {
  
  digitalWrite (TX_ENABLE_PIN, HIGH);   // Omoguci slanje ZADATE TEMPERATURE
  delay(30);
  Serial.println("");
  Serial.print("Salje na rs485: "); 
  
    for (int k = 0; k < 15; k++) {
       Serial.print(charBuf[k]);                
       rs485.write(charBuf[k]);
                                 }   
}

  TpomocniStaro = Tpomocni;

//******* Da li je prozor u podrumu  otvoren ili zatvorena ? *************
  ko1 = digitalRead(PODRUM_PROZOR);
  if ( old_ko1 != ko1) {
  
     if ( ko1 == 1)  { client.publish("kuca/Podrum/ko1", "OPEN");
                       Serial.println("Podrumski prozor je otvoren ");

                      }
     
     else        {     client.publish("kuca/Podrum/ko1", "CLOSED");
                       Serial.println("Podrumski prozor je zatvoren ");

     }
  }
     old_ko1 = ko1;
//**********************************************************************
 
  if (!client.connected()) {
    Serial.println("Izgubljena je konekcija - pozivam...reconnect() !");
    reconnect();
    delay(100);
        }

  client.loop(); 
  delay(100); 

}
