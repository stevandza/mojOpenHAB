/*
Senzor svetla TSL2561 - proba sa Esp8266 E12
MQTT : "kuca/svn1"   ---->  
16.09.2016
Proba slanja podataka nivoa svetla na MQTT
 
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SparkFunTSL2561.h>
#include <Wire.h>


SFE_TSL2561 light;


#define DVORISNA_RASVETA       6     //  OUT -   D2  


boolean gain;     // Gain setting, 0 = X1, 1 = X16;
unsigned int ms;  // Integration ("shutter") time in milliseconds 
double lux;    // Resulting lux value
boolean good;  // True if neither sensor is saturated

char lux_Ch[6];
char msg[6];

// Update these with values suitable for your network.

const char* ssid = "Zozon";
const char* password = "zozon1978";
const char* mqtt_server = "192.168.1.100";



WiFiClient espClient;
PubSubClient client(espClient);


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
  
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266ClientTSL2561")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("TSL2561_ESP8266", "hello world");
      // ... and resubscribe
      client.subscribe("kuca/Dvo/sv1");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void printError(byte error)

{
  Serial.print("I2C error: ");
  Serial.print(error,DEC);
  Serial.print(", ");
  
  switch(error)
  {
    case 0:
      Serial.println("success");
      break;
    case 1:
      Serial.println("data too long for transmit buffer");
      break;
    case 2:
      Serial.println("received NACK on address (disconnected?)");
      break;
    case 3:
      Serial.println("received NACK on data");
      break;
    case 4:
      Serial.println("other error");
      break;
    default:
      Serial.println("unknown error");
  }
}

void setup() {

  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  
  
  Serial.println("TSL2561 example sketch");
  light.begin();
  unsigned char ID;
  
  if (light.getID(ID))
  {
    Serial.print("Got factory ID: 0X");
    Serial.print(ID,HEX);
    Serial.println(", should be 0X5X");
  }
  else
    {
      byte error = light.getError();
      printError(error);
   }

  gain = 0;
  unsigned char time = 2;
  Serial.println("Set timing...");
  light.setTiming(gain,time,ms);
  Serial.println("Powerup...");
  light.setPowerUp();
 
//  pinMode(DVORISNA_RASVETA,OUTPUT);              // Dvorisna rasveta

 // inicijalizacija izlaza
 
//  digitalWrite(DVORISNA_RASVETA,HIGH);

}

void loop() {
  
 unsigned int data0, data1;
 
  if (!client.connected()) {
    reconnect();
        }
    
  client.loop();
  delay(ms);
  
  
  if (light.getData(data0,data1))
  {

    Serial.print("data0: ");
    Serial.print(data0);
    Serial.print(" data1: ");
    Serial.print(data1);
    
    good = light.getLux(gain,ms,data0,data1,lux);
  
    Serial.print(" lux: ");
    Serial.print(lux);
    
    if (good) Serial.println(" (good)"); 
    else Serial.println(" (BAD)");
  }
else
  {
    byte error = light.getError();
    printError(error);
  }

  float luxF ;
  luxF = lux;
  dtostrf(luxF,6,1,lux_Ch);
  Serial.print("Nivo dnevnog svetla je:  ");
  Serial.println(lux_Ch);
  client.publish("kuca/svn1",lux_Ch); 

 
  
  delay(1000);
    
}
