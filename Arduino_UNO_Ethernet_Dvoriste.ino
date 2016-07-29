/*
  
 DVORISTE

 Kombinacija sa UNO R2 i Ethernrt Shield plocom W5100
 
 Reconnecting MQTT example - non-blocking

 This sketch demonstrates how to keep the client connected
 using a non-blocking reconnect function. If the client loses
 its connection, it attempts to reconnect every 5 seconds
 without blocking the main loop.

 
*/

#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SoftwareSerial.h>

#include <RCSwitch.h>

#include <Wire.h>  // Comes with Arduino IDE
#include "DHT.h"

// IZLAZI

#define DIMER_DVORISTE       9    //  OUT -   
#define DVORISNA_RASVETA     8    //  OUT -   
#define SVETLO_IS_GARAZA     7    //  OUT -   
#define SVETLO_IS_KUCE       6    //  OUT -   
#define GARAZA_VRATA_RELE    3    //  OUT -     

// ULAZI

#define PRISUSTVO_OSOBE      19     //  IN - 
#define GARAZA_VRATA_STATE   14     //  IN -
  
#define SENZOR_SVETLA        A4     //  IN -     
   
#define GARAZA_DHTPIN        15     //  IN  - DHT11  Vlaznost i temperatura napolju
#define DHTTYPE DHT11
DHT dht(GARAZA_DHTPIN, DHTTYPE);


byte mac[]    = {  0xDA, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 31);
IPAddress server(192, 168, 1, 100);

EthernetClient ethClient;
PubSubClient client(ethClient);

RCSwitch mySwitch = RCSwitch();

long lastMsg  = 0;
long lastMsg1 = 0;
char msg1[10], msg2[10], msg3[10];
int  value  = 0;
int  value1 = 0;


int svd1=0;
int svn1=0;

float temp;
float tempOld = 0;

int ko1=1; 
int ko2=1;
int ko3=1;
int ko4=1;

int old_ko1=0; 
int old_ko2=0;
int old_ko3=0;
int old_ko4=0;


void callback(char* topic, byte* payload, unsigned int length) {
    int i=0;
    char message_buff[100];

    for (int i = 0; i < length; i++) {
       message_buff[i] = (char)payload[i];
//     Serial.print((char)payload[i]);
    }

    payload[length] = '\0';
    String strPayload = String((char*)payload);
   
   Serial.println("");
   Serial.print(String((char*)topic));
   Serial.print("[");
   Serial.print(strPayload);
   Serial.println("]");



  if (String((char*)topic) == "kuca/Dvo/sv1") {         // Dvorisna rasveta
      if (strPayload == "on") { 
        digitalWrite(DVORISNA_RASVETA,LOW);
        Serial.println("Ukljuceno je svetlo na dvoristu");
                              }
       else       {         
        digitalWrite(DVORISNA_RASVETA,HIGH);  
        Serial.println("Iskljuceno je svetlo na dvoristu");
                  }
 }

  if (String((char*)topic) == "kuca/Dvo/svd1") {       //  Dimer - Dvorisna rasveta

       analogWrite(DIMER_DVORISTE,100-strPayload.toInt());   
       Serial.println("");
       Serial.print("Nivo svetla na dvoristu je: ");
       Serial.print(strPayload.toInt());
       Serial.println("");
                     
           }
 
 
 if (String((char*)topic) == "kuca/Dvo/sv2") {          // Svetlo ispred garaze
     if (strPayload == "on") { 
        digitalWrite(SVETLO_IS_GARAZA,LOW);
        Serial.println("Ukljuceno je svetlo ispred garaze");
                              }
       else       {         
        digitalWrite(SVETLO_IS_GARAZA,HIGH);  
        Serial.println("Iskljuceno je svetlo ispred garaze");
                  }                          
 }
 
 if (String((char*)topic) == "kuca/Dvo/sv3") {          // Svetlo  ispred kuce
     if (strPayload == "on") { 
        digitalWrite(SVETLO_IS_KUCE,LOW);
        Serial.println("Ukljuceno je svetlo ispred kuce");
                              }       
       else       {         
        digitalWrite(SVETLO_IS_KUCE,HIGH);  
        Serial.println("Iskljuceno je svetlo ispred kuce");
                  }  
 }
 
 if (String((char*)topic) == "kuca/Dvo/sv4") {         // Zakljucavanje / otkljucavanje vrata garaze
     if (strPayload == "on") { 
        digitalWrite(GARAZA_VRATA_RELE,LOW);
        Serial.println("Vrata su zakljucana");
                              }      
       else       {         
        digitalWrite(GARAZA_VRATA_RELE,HIGH);  
        Serial.println("Vrata su otkljucana");
                  }  
 }

  
}



boolean reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
          Serial.println("Pokusavam da se konektujem na MQTT...");
          client.connect("Klijent_Podrum");
          delay(10);
          // Attempt to connect
    if (client.connect("Klijent_Podrum")) {
        Serial.println("konektovan");
         // Once connected, publish an announcement...
        client.publish("Klijent_Podrum", "Klijent PODRUM je konektovan");
         // ... and resubscribe
        client.subscribe("kuca/Podrum/#");
    } else {
      Serial.print("greska, rc=");
      Serial.print(client.state());
      Serial.println(" pokusacu ponovo za 2 sekunde");
      // Wait 3 seconds before retrying
      delay(2000);
    }
  }
return client.connected();


/*  
  if (client.connect("Klijent_Podrum")) {
    
    // Once connected, publish an announcement...
    
    client.publish("Klijent_Podrum","Klijent PODRUM je konektovan");
    Serial.println("Klijent PODRUM je konektovan");

    // ... and resubscribe
    client.subscribe("kuca/Podrum/#");
  }
  return client.connected();
 */
}

void setup()
{

    pinMode(DVORISNA_RASVETA, OUTPUT);       // Dvorisna rasveta
    pinMode(DIMER_DVORISTE,   OUTPUT);       // Dvorisna rasveta - DIMMER
    pinMode(SVETLO_IS_GARAZA, OUTPUT);       // Svetlo ispred garaze
    pinMode(SVETLO_IS_KUCE,   OUTPUT);       // Svetlo ispred kuce
    pinMode(GARAZA_VRATA_RELE,OUTPUT);       // Zakljucavanje / otkljucavanje vrata od garaze
     

    pinMode(SENZOR_SVETLA,INPUT);             // Ulaz analognog senzora
    pinMode(GARAZA_VRATA_STATE,INPUT_PULLUP); // Vrata garaze
    pinMode(PRISUSTVO_OSOBE,INPUT_PULLUP);    // Prisustvo osobe u garazi
    
  
  

// inicijalizacija izlaza - Ugasi sve izlaze

    digitalWrite(DVORISNA_RASVETA, HIGH);
    digitalWrite(DIMER_DVORISTE,   HIGH);  
    digitalWrite(SVETLO_IS_GARAZA, HIGH); 
    digitalWrite(SVETLO_IS_KUCE,   HIGH); 
    digitalWrite(GARAZA_VRATA_RELE,HIGH);

  
   Serial.begin(115200);
   client.setServer(server, 1883);
   client.setCallback(callback);

   Ethernet.begin(mac, ip);
   delay(1500);

   mySwitch.enableReceive(0);  // Receiver on interrupt 0 => that is pin #2
   
//******************************************************* 
// Posalji trenutno stanje digitalnih ulaza na openHAB

ko4 = digitalRead(GARAZA_VRATA_RELE);
if ( ko4 == 1)  { client.publish("kuca/Dvo/sv4/state", "OPEN");
                  Serial.println("SETUP : Podrumski prozor je otvoren ");
                }    
     else        {   client.publish("kuca/Dvo/sv4/state", "CLOSED");
                     Serial.println("SETUP : Podrumski prozor je zatvoren ");
                 }
old_ko4 = ko4;
                 
}

void loop() {

  if (!client.connected()) {
    Serial.println("Izgubljena je konekcija - pozivam...reconnect() !");
    reconnect();
    delay(100);
        }

// ******* Prijem RF signala **************************** 
if (mySwitch.available()) {
    
    int valueRF = mySwitch.getReceivedValue();
    
    if (valueRF == 0) {
      Serial.print("Unknown encoding");
    } else {
      Serial.print("Received ");
      Serial.print( mySwitch.getReceivedValue() );
      Serial.println("");   
    }

    mySwitch.resetAvailable();
  }
 
//******* Da li su vrata GARAZE  otvorena ili zatvorena ? *************
  int ko1 = digitalRead(GARAZA_VRATA_STATE);
  if ( old_ko1 != ko1) {
  
     if ( ko1 == 1)   client.publish("kuca/Dvo/ko1", "OPEN");
     else             client.publish("kuca/Dvo/ko1", "CLOSED");
  }
     old_ko1 = ko1;
//**********************************************************************
//******* Da li su vrata garaze  OTKLJUCANA ili ZAKLJUCANA ? ***********
// Provera stanja izlaza Arduino UNO R3 modula i slanje stanja na OpenHAB

  int ko4 = digitalRead(GARAZA_VRATA_RELE);
  if ( old_ko4 != ko4) {
  
     if ( ko4 == 1)   client.publish("kuca/Dvo/sv4/state", "OPEN");
     else             client.publish("kuca/Dvo/sv4/state", "CLOSED");
  }
     old_ko4 = ko4;
     
//************** Provera prisustva osobe ***************************
int ko2 = digitalRead(PRISUSTVO_OSOBE);
  if (old_ko2 != ko2 ) {
     if ( ko2 == 1)   client.publish("kuca/Dvo/ko2", "CLOSED");
     else             client.publish("kuca/Dvo/ko2", "OPEN");  
  }
     old_ko2 = ko2;



//****** Petlja za slanje podataka na 3 sekunde *************

 long now = millis();
  if (now - lastMsg > 3000) {
    lastMsg = now;
    ++value;

//****** Ocitavanje sa DHT11 senzora *************

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t) ) {
    delay(20);
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

//  Serial.print("Humidity DHT: ");
//  Serial.print(h);
//  Serial.println(" %\t");
//  Serial.print("Temperature DHT: ");
//  Serial.print(t);
//  Serial.println(" C ");

  dtostrf(h,4,2,msg2);
  Serial.print("Vlaznost napolju je (DHT): ");
  Serial.println(msg2);
  client.publish("kuca/vlaDvo",msg2);

  dtostrf(t,4,2,msg3);
  Serial.print("Temperatura napolju je (DHT): ");
  Serial.println(msg3);
  client.publish("kuca/tempNap",msg3);

//**** Ocitavanje sa senzora svetla ********

  int ko3 = analogRead(SENZOR_SVETLA) / 10.24;  // Skalirano od 0 - 100%
  snprintf (msg3, 75, "%ld", ko3);
  Serial.print("Nivo dnevnog svetla je:  ");
  Serial.println(msg3);
  if (old_ko3 != ko3 ) {
        client.publish("kuca/Dvo/svn1", msg3); 
          }
     old_ko3 = ko3;  

  }

  
//***** Na svakih 1 minut uploduj sa openHAB-a trenutna stanja 10000 = 1.40 minuta ********

long now1 = millis();
  if (now1 - lastMsg1 > 100000) {
    lastMsg1 = now1;
    ++value1;

client.subscribe("kuca/Dvo/#");

  }
  
//*******************************************************************************

  client.loop(); 
  delay(500); 

}
