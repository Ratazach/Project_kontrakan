/*
esp-01 test for Adafruit_MQTT for IoT
*/
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define WLAN_SSID       "wi-fi"             
#define WLAN_PASS       "pass"            

#define AIO_SERVER      "io.adafruit.com" //Adafruit Server
#define AIO_SERVERPORT  1883                   
#define AIO_USERNAME    "username" 
#define AIO_KEY         "Auth"
#define Relay 0         //esp-01 relay module pin
//WIFI CLIENT
WiFiClient client;

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

Adafruit_MQTT_Subscribe Light1 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME"/feeds/Relay5"); // Feeds name should be same everywhere

void MQTT_connect();


void setup() {
  // put your setup code here, to run once:
    Serial.begin(115200);

pinMode(Relay, OUTPUT);
Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); 
  Serial.println(WiFi.localIP());
 
  mqtt.subscribe(&Light1);
//  digitalWrite(Relay,HIGH);
//  delay(200);
//  
//  digitalWrite(Relay,LOW);
//  delay(200);
}

void loop() {
  // put your main code here, to run repeatedly:
MQTT_connect();
//  Serial.println("p");
//delay(1000);}
//  digitalWrite(Relay,HIGH);
//  delay(200);
//  
//  digitalWrite(Relay,LOW);
//  delay(200);

  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(100))) {
    if (subscription == &Light1) {
      Serial.print(F("relay 1 Got: "));
      Serial.println((char *)Light1.lastread);
      int Light1_State = atoi((char *)Light1.lastread);
      digitalWrite(Relay, Light1_State);
  
    }
}}

void MQTT_connect() {
  int8_t ret;

  if (mqtt.connected()) {
//    digitalWrite(Relay,HIGH);
//  delay(200);
//  
//  digitalWrite(Relay,LOW);
//  delay(200);
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  
  while ((ret = mqtt.connect()) != 0) {
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 3 seconds...");
    mqtt.disconnect();
    delay(3000); 
    retries--;
    Serial.print ("retry atemps : ");Serial.print (retries);
    if (retries == 0) {
//      ESP.reset();
//      while (1);
    }
  }
  Serial.println("MQTT Connected!");
  
}
