// Uncomment the following line to enable serial debug output
//#define ENABLE_DEBUG

#ifdef ENABLE_DEBUG
       #define DEBUG_ESP_PORT Serial
       #define NODEBUG_WEBSOCKETS
       #define NDEBUG
#endif 

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "SinricPro.h"
#include "SinricProSwitch.h"

#include <map>

#define WIFI_SSID         "moshiw"    
#define WIFI_PASS         "konsbudjaya"
#define APP_KEY           "95fb9687-25b7-48ad-ae97-0277bb6025c9"      // Should look like "de0bxxxx-1x3x-4x3x-ax2x-5dabxxxxxxxx"
#define APP_SECRET        "48574c6f-057a-48cf-b106-8a34661f7ca2-6e653e9f-0b64-4d47-864b-29d7387c6905"   // Should look like "5f36xxxx-x3x7-4x3x-xexe-e86724a9xxxx-4c4axxxx-3x3x-x5xe-x9x3-333d65xxxxxx"

//Enter the device IDs here
#define device_ID_1   "613351629ff2c3320056421c" //humidifier
//#define device_ID_1   "62a05ce1ff368211543a54ef" //TEST

// define the GPIO connected with Relays and switches
#define RelayPin1 D3  //D1

#define SwitchPin1 2//SD3

#define wifiLed   15   //D0

#define nyala D1

// comment the following line if you use a toggle switches instead of tactile buttons
#define TACTILE_BUTTON 1

#define BAUD_RATE   9600

#define DEBOUNCE_TIME 250

typedef struct {      // struct for the std::map below
  int relayPIN;
  int flipSwitchPIN;
} deviceConfig_t;


// this is the main configuration
// please put in your deviceId, the PIN for Relay and PIN for flipSwitch
// this can be up to N devices...depending on how much pin's available on your device ;)
// right now we have 4 devicesIds going to 4 relays and 4 flip switches to switch the relay manually
std::map<String, deviceConfig_t> devices = {
    //{deviceId, {relayPIN,  flipSwitchPIN}}
    {device_ID_1, {  RelayPin1, SwitchPin1 }},  
};

typedef struct {      // struct for the std::map below
  String deviceId;
  bool lastFlipSwitchState;
  unsigned long lastFlipSwitchChange;
} flipSwitchConfig_t;

std::map<int, flipSwitchConfig_t> flipSwitches;    // this map is used to map flipSwitch PINs to deviceId and handling debounce and last flipSwitch state checks
                                                  // it will be setup in "setupFlipSwitches" function, using informations from devices map

void setupRelays() { 
  for (auto &device : devices) {           // for each device (relay, flipSwitch combination)
    int relayPIN = device.second.relayPIN; // get the relay pin
    pinMode(relayPIN, OUTPUT);             // set relay pin to OUTPUT
    digitalWrite(relayPIN, HIGH);
    pinMode(nyala,OUTPUT);
    digitalWrite(nyala, HIGH);
  }
}

void setupFlipSwitches() {
  for (auto &device : devices)  {                     // for each device (relay / flipSwitch combination)
    flipSwitchConfig_t flipSwitchConfig;              // create a new flipSwitch configuration

    flipSwitchConfig.deviceId = device.first;         // set the deviceId
    flipSwitchConfig.lastFlipSwitchChange = 0;        // set debounce time
    flipSwitchConfig.lastFlipSwitchState = true;     // set lastFlipSwitchState to false (LOW)--

    int flipSwitchPIN = device.second.flipSwitchPIN;  // get the flipSwitchPIN

    flipSwitches[flipSwitchPIN] = flipSwitchConfig;   // save the flipSwitch config to flipSwitches map
    pinMode(flipSwitchPIN, INPUT_PULLUP);                   // set the flipSwitch pin to INPUT
  }
}

bool onPowerState(String deviceId, bool &state)
{
  Serial.printf("%s: %s\r\n", deviceId.c_str(), state ? "on" : "off");
  int relayPIN = devices[deviceId].relayPIN; // get the relay pin for corresponding device
  digitalWrite(relayPIN, state);             // set the new relay state
  delay(1000);
  digitalWrite(nyala,state);
  Serial.println("nyala");
  delay(150);
  digitalWrite(nyala,LOW);
  return true;
}

void handleFlipSwitches() {
  unsigned long actualMillis = millis();                                          // get actual millis
  for (auto &flipSwitch : flipSwitches) {                                         // for each flipSwitch in flipSwitches map
    unsigned long lastFlipSwitchChange = flipSwitch.second.lastFlipSwitchChange;  // get the timestamp when flipSwitch was pressed last time (used to debounce / limit events)

    if (actualMillis - lastFlipSwitchChange > DEBOUNCE_TIME) {                    // if time is > debounce time...

      int flipSwitchPIN = flipSwitch.first;                                       // get the flipSwitch pin from configuration
      bool lastFlipSwitchState = flipSwitch.second.lastFlipSwitchState;           // get the lastFlipSwitchState
      bool flipSwitchState = digitalRead(flipSwitchPIN);                          // read the current flipSwitch state
      if (flipSwitchState != lastFlipSwitchState) {                               // if the flipSwitchState has changed...
#ifdef TACTILE_BUTTON
        if (flipSwitchState) {                                                    // if the tactile button is pressed 
#endif      
          flipSwitch.second.lastFlipSwitchChange = actualMillis;                  // update lastFlipSwitchChange time
          String deviceId = flipSwitch.second.deviceId;                           // get the deviceId from config
          int relayPIN = devices[deviceId].relayPIN;                              // get the relayPIN from config
          bool newRelayState = !digitalRead(relayPIN);                            // set the new relay State
          digitalWrite(relayPIN, newRelayState);                                  // set the trelay to the new state

          SinricProSwitch &mySwitch = SinricPro[deviceId];                        // get Switch device from SinricPro
          mySwitch.sendPowerStateEvent(!newRelayState);                            // send the event
#ifdef TACTILE_BUTTON
        }
#endif      
        flipSwitch.second.lastFlipSwitchState = flipSwitchState;                  // update lastFlipSwitchState
      }
    }
  }
}

void setupWiFi()
{
  Serial.printf("\r\n[Wifi]: Connecting");
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED)
  {
    digitalWrite(wifiLed, LOW);
    Serial.printf(".");
    delay(150);
    digitalWrite(wifiLed, HIGH);
    delay(150);
  }
  digitalWrite(wifiLed, HIGH);
  Serial.printf("connected!\r\n[WiFi]: IP-Address is %s\r\n", WiFi.localIP().toString().c_str());
}

void setupSinricPro()
{
  for (auto &device : devices)
  {
    const char *deviceId = device.first.c_str();
    SinricProSwitch &mySwitch = SinricPro[deviceId];
    mySwitch.onPowerState(onPowerState);
  }

  SinricPro.begin(APP_KEY, APP_SECRET);
  SinricPro.restoreDeviceStates(true);
}

void setup()
{
  Serial.begin(BAUD_RATE);

  pinMode(wifiLed, OUTPUT);
  digitalWrite(wifiLed, HIGH);
  setupRelays();
  setupFlipSwitches();
  setupWiFi();
  setupSinricPro();
}

void loop()
{
  SinricPro.handle();
  handleFlipSwitches();
}
