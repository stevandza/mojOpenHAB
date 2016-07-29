/*
Ovo je radilo !
13.05.2016.
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include <Wire.h>  // Comes with Arduino IDE
#include "DHT.h"


#define DIMER_DVORISTE       2    //  OUT -   D9  // BUILTIN LED  WeMos
#define DVORISNA_RASVETA     12   //  OUT -   D6
#define SVETLO_IS_GARAZA     14   //  OUT -   D5
#define SVETLO_IS_KUCE       13   //  OUT -   D7
#define GARAZA_VRATA_RELE    15   //  OUT -   SS


#define PRISUSTVO_OSOBE       0     //  IN -   D8
#define SENZOR_SVETLA        16     //  IN -   D2
#define ANALOGNI_SENZOR      A0     //  IN -   A0
#define GARAZA_VRATA_STATE   4      //  IN -   D3



#define DHTPIN                 5       //  IN  -   D8 .....DHT11
#define DHTTYPE DHT11

// #define ONE_WIRE_BUS            0       // DS18B20  pin ---- D8 ULAZ
// OneWire oneWire(ONE_WIRE_BUS);
// DallasTemperature DS18B20(&oneWire);

DHT dht(DHTPIN, DHTTYPE);

// Update these with values suitable for your network.

const char* ssid = "Zozon";
const char* password = "zozon1978";
const char* mqtt_server = "192.168.1.8";


WiFiClient espClient;
PubSubClient client(espClient);



long lastMsg = 0;
char msg[50], msg1[10], msg2[10], msg3[10];
int  value = 0;

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


void setup_wifi() {
  
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}



void callback(char* topic, byte* payload, unsigned int length) {
  int i=0;
  char message_buff[100];
  
//  Serial.print("Message arrived [");
//  Serial.print(topic);
//  Serial.print("] ");

  for (int i = 0; i < length; i++) {
       message_buff[i] = (char)payload[i];
//     Serial.print((char)payload[i]);
    }
   payload[length] = '\0';
   String strPayload = String((char*)payload);
   
//   Serial.println("");
//   Serial.print(String((char*)topic));
//   Serial.print("[");
//   Serial.print(strPayload);
//   Serial.println("]");



     
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

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("Dvo_ESP8266", "hello world");
      // ... and resubscribe
      client.subscribe("kuca/Dvo/#");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {

  
 
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  
  analogWriteRange(100); 
   
  dht.begin();
 
    pinMode(DVORISNA_RASVETA, OUTPUT);       // Dvorisna rasveta
    pinMode(DIMER_DVORISTE,   OUTPUT);       // Dvorisna rasveta - DIMMER
    pinMode(SVETLO_IS_GARAZA, OUTPUT);       // Svetlo ispred garaze
    pinMode(SVETLO_IS_KUCE,   OUTPUT);       // Svetlo ispred kuce
    pinMode(GARAZA_VRATA_RELE,OUTPUT);       // Zakljucavanje / otkljucavanje vrata od garaze
     

    pinMode(ANALOGNI_SENZOR,INPUT);           // Ulaz analognog senzora
    pinMode(GARAZA_VRATA_STATE,INPUT_PULLUP); // Vrata garaze
    pinMode(PRISUSTVO_OSOBE,INPUT_PULLUP);    // Prisustvo osobe u garazi
    pinMode(SENZOR_SVETLA,INPUT);             // Senzor za svetlo
    
//  pinMode(DHTPIN, INPUT_PULLUP);            // Senzor vlaznosti i temperature

  

//  Inicijalizacija izlaza
 
    digitalWrite(DVORISNA_RASVETA, HIGH);
    digitalWrite(DIMER_DVORISTE,   HIGH);  
    digitalWrite(SVETLO_IS_GARAZA, HIGH); 
    digitalWrite(SVETLO_IS_KUCE,   HIGH); 
    digitalWrite(GARAZA_VRATA_RELE,HIGH);  


}

void loop() {
 
  if (!client.connected()) {
    reconnect();
        }
  client.loop();
   
//******* Da li su vrata GARAZE  otvorena ili zatvorena ? *************
  int ko1 = digitalRead(GARAZA_VRATA_STATE);
  if ( old_ko1 != ko1) {
  
     if ( ko1 == 1)   client.publish("kuca/Dvo/ko1", "OPEN");
     else             client.publish("kuca/Dvo/ko1", "CLOSED");
  }
     old_ko1 = ko1;
//**********************************************************************
//******* Da li su vrata garaze  OTKLJUCANA ili ZAKLJUCANA ? ***********
// Provera stanja izlaza ESP8266 modula i slanje stanja na OpenHAB

  int ko4 = digitalRead(GARAZA_VRATA_RELE);
  if ( old_ko4 != ko4) {
  
     if ( ko4 == 1)   client.publish("kuca/Dvo/sv4/state", "OPEN");
     else             client.publish("kuca/Dvo/sv4/state", "CLOSED");
  }
     old_ko4 = ko4;
     
//**********************************************************************

  int ko2 = digitalRead(PRISUSTVO_OSOBE);
  if (old_ko2 != ko2 ) {
     if ( ko2 == 1)   client.publish("kuca/Dvo/ko2", "CLOSED");
     else             client.publish("kuca/Dvo/ko2", "OPEN");  
  }
     old_ko2 = ko2;

     
/*  int ko3 = digitalRead(SENZOR_SVETLA);
  if (old_ko3 != ko3 ) {
     if ( ko3 == 1)   client.publish("kuca/Dvo/ko3", "OPEN");
     else             client.publish("kuca/Dvo/ko3", "CLOSED");  
  }
     old_ko3 = ko3;   
*/

  
  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;

//******* Privremeni Brojac ****************************** 

    snprintf (msg, 75, "%ld", value);
//  Serial.print("Publish message : ");
//  Serial.println(msg);
    client.publish("kuca/tempPriz", msg);
//******************************************************** 



/*
//**** Ocitavanje temperature sa DS18B20 *****************
     do {
      DS18B20.requestTemperatures(); 
      temp = DS18B20.getTempCByIndex(0);
      Serial.print("Temperatura napolju je : ");
      Serial.println(temp);
      delay(10);
    } while (temp == 85.0 || temp == (-127.0));


      dtostrf(temp,4,2,msg1);
      Serial.print("Temperatura napolju je: ");
      Serial.println(msg1);
      client.publish("kuca/tempNap",msg1);
*/      

// ***** Ocitavanje sa DHT22 senzora *****

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

  dtostrf(t,4,2,msg1);
  Serial.print("Temperatura napolju je (DHT): ");
  Serial.println(msg1);
  client.publish("kuca/tempNap",msg1);
  

//**** Ocitavanje sa senzora svetla ********
  int ko3 = analogRead(ANALOGNI_SENZOR) / 10.24;  // Skalirano od 0 - 100%
  snprintf (msg3, 75, "%ld", ko3);
  Serial.print("Nivo dnevnog svetla je:  ");
  Serial.println(msg3);
  if (old_ko3 != ko3 ) {
        client.publish("kuca/Dvo/svn1", msg3); 
          }
     old_ko3 = ko3;  
    
      
    }
    
}





