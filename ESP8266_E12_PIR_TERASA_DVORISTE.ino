/*
Pir senzor svetla ispod terase na dvoristu.

MQTT : "kuca/Dvo/PIR"   ---->  OPEN / CLOSE

MAP  :  pir.map

01.08.2016

 Na samom senzoru je i povratna informacija o stanju dvorisne rasvete navezano za PIR senzor !

 Iz nekog razloga ova verzija ne gubi konekciju pretpostavljam da svaki klijent mora imati svoje ime    client.connect("ESP8266ClientPir") !
 
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>


#define DVORISNA_RASVETA       4     //  OUT -   D2   
#define PIR_TERASA_DVORISTE    2     //  IN  -   D4

// Update these with values suitable for your network.

const char* ssid = "Zozon";
const char* password = "zozon1978";
const char* mqtt_server = "192.168.1.100";

int channel = 11;


WiFiClient espClient;
PubSubClient client(espClient);


int ko1=1; 
int old_ko1=0; 



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
    if (client.connect("ESP8266ClientPir")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("PIR_ESP8266", "hello world");
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

void setup() {
  
  
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
 
  pinMode(DVORISNA_RASVETA,OUTPUT);              // Dvorisna rasveta
  pinMode(PIR_TERASA_DVORISTE,INPUT_PULLUP);     // Senzor za svetlo
  
// inicijalizacija izlaza
 
  digitalWrite(DVORISNA_RASVETA,HIGH);

}

void loop() {
 
  if (!client.connected()) {
    reconnect();
        }
        
  client.loop();

  int ko1 = digitalRead(PIR_TERASA_DVORISTE);
  if (old_ko1 != ko1 ) {
     if ( ko1 == 1)   { client.publish("kuca/Dvo/PIR", "OPEN");
     Serial.println("PIR je OPEN");
     }
     
     else           {  client.publish("kuca/Dvo/PIR", "CLOSED");  
     
                        Serial.println("PIR je CLOSED");
     }
  }
     old_ko1 = ko1; 

  delay(200);
    
}




