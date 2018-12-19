/*
    'stairsLight1_B_test3' by Thurstan. LEDs controlled by motion sensors.
    Copyright (C) 2018  MTS Standish (Thurstan|mattKsp)
    
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    
    This program is distributed in the hope that it will be usefull,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
    
    https://github.com/mattThurstan/
*/


#include <FS.h>
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
#include <ArduinoJson.h>          //https://github.com/bblanchon/ArduinoJson
#include <PubSubClient.h>        //https://github.com/knolleary/pubsubclient
#include <MD5Builder.h>
#include <FastLED.h>                              //WS2812B LED strip control and effects


const unsigned long _pirHoldInterval = 30000; //150000;  //15000=15 sec. 30000=30 sec. 150000=2.5 mins.
unsigned long _ledRiseSpeed = 25; //35;                 //speed at which the LEDs turn on (runs backwards)

/*----------------------------arduino pins----------------------------*/
//2=top, 3=bottom - due to the way the LED strip is wired (top to bot) so thats the way the array goes..
const byte _pirPin[2] = { 2, 3 };             //2 PIR sensors on interrupt pins (triggered on HIGH)
const byte _ledDOut0Pin = 4;                  //FastLED strip

/*----------------------------system----------------------------*/
const String _progName = "stairsLight1_B_test3";
const String _progVers = "0.240";               //
#define UPDATES_PER_SECOND 0           //120    //main loop FastLED show delay - 1000/120
#define DEBUG 1

/*----------------------------PIR----------------------------*/
//volatile boolean _onOff = false;    //use state 0          //global. this should init false, then get activated by input - on/off true/false
volatile byte _state = 0;                     //0-Off, 1-Fade On, 2-On, 3-Fade Off
volatile byte _stateSave = 0;                 //temp save state for inside for-loops
//direction for fade on/off is determined by last pir triggered
volatile unsigned long _pirHoldPrevMillis = 0;
volatile byte _pirLastTriggered = 255;        //last PIR sensor triggered (0=top or 1=bottom)
volatile boolean _timerRunning = false;       //is the hold timer in use?
volatile byte _fadeOnDirection = 255;         //direction for fade on loop. 0=fade down the stairs (top to bot), 1=fade up the stairs (bot to top).

/*----------------------------LED----------------------------*/
#define MAX_POWER_DRAW 900                  //limit the maximum power draw (in milliamps mA)
typedef struct {
  byte first;
  byte last;
  byte total;                                     //using a byte here is ok as we haven't got more than 256 LEDs in a segment
} LED_SEGMENT;
const byte _segmentTotal = 1;                     //runs down stair banister from top to bottom
const byte _ledNum = 20; //108;                         //
LED_SEGMENT ledSegment[_segmentTotal] = {
  //{ 0, 0, 1 },
  //{ 0, 107, 108 },
  { 0, 19, 20 },
};
CRGBArray<_ledNum> _leds;                         //CRGBArray means can do multiple '_leds(0, 2).fadeToBlackBy(40);' as well as single '_leds[0].fadeToBlackBy(40);'

byte _ledGlobalBrightnessCur = 255;               //current global brightness
#define GHUE_CYCLE_TIME 200                       //gHue loop update time (in milliseconds)
uint8_t gHue = 0;                                 //incremental "base color" used by loop
CHSV _topColorHSV( 50, 150, 255 );                //0, 0, 200  -  50, 80, 159
CHSV _botColorHSV( 50, 150, 255 );                //0, 0, 200  -  50, 80, 159


/*----------------------------MQTT----------------------------*/
unsigned long mqttConnectionPreviousMillis = millis();
const long mqttConnectionInterval = 60000;

//define your default values here, if there are different values in config.json, they are overwritten.
char mqtt_server[40] = "";
char mqtt_port[6] = "";       //1884
char workgroup[32] = "workgroup";
// MQTT username and password
char username[20] = "";
char password[20] = "";

//MD5 of chip ID
char machineId[32] = "";

//flag for saving data
bool shouldSaveConfig = false;

// MQTT
WiFiClient espClient;
PubSubClient mqttClient(espClient);

const uint8_t MSG_BUFFER_SIZE = 50;
//long lastMsg = 0;
//char msg[MSG_BUFFER_SIZE];
//int value = 0;

// buffer used to send/receive data with MQTT
//const uint8_t MSG_BUFFER_SIZE = 20;
char m_msg_buffer[MSG_BUFFER_SIZE]; 

//char cmnd_power_topic[44];
//char cmnd_color_topic[44];

//char stat_power_topic[44];
//char stat_color_topic[44];

//broadcast states and subscribe to commands
const PROGMEM char* MQTT_LIGHTS_TOPIC_STATE = "house/stairs/lights/light/status";
const PROGMEM char* MQTT_LIGHTS_TOPIC_COMMAND = "house/stairs/lights/light/switch";

const PROGMEM char* MQTT_LIGHTS_BRIGHTNESS_TOPIC_STATE = "house/stairs/lights/brightness/status";
const PROGMEM char* MQTT_LIGHTS_BRIGHTNESS_TOPIC_COMMAND = "house/stairs/lights/brightness/set";

//const PROGMEM char* MQTT_LIGHTS_HUE_TOPIC_STATE = "house/stairs/lights/hue/status";
//const PROGMEM char* MQTT_LIGHTS_HUE_TOPIC_COMMAND = "house/stairs/lights/hue/set";

const PROGMEM char* MQTT_LIGHTS_RGB_TOPIC_STATE = "house/stairs/lights/rgb/status";
const PROGMEM char* MQTT_LIGHTS_RGB_TOPIC_COMMAND = "house/stairs/lights/rgb/set";

const PROGMEM char* LIGHTS_ON = "ON";
const PROGMEM char* LIGHTS_OFF = "OFF";


/*----------------------------MAIN----------------------------*/
void setup()
{
  Serial.begin(115200);
  Serial.println();
  
  if (DEBUG) {
    Serial.println();
    Serial.print(_progName);
    Serial.print(" ");
    Serial.print(_progVers);
    Serial.println();
    Serial.print("..");
    Serial.println();
  }
  
  setupPIR();
  setupLED();
  
  loadConfig();
  calculateMachineId();

  // Set MQTT topics
  //sprintf(cmnd_power_topic, "cmnd/%s/power", machineId);
  //sprintf(cmnd_color_topic, "cmnd/%s/color", machineId);
  //sprintf(stat_power_topic, "stat/%s/power", machineId);
  //sprintf(stat_color_topic, "stat/%s/color", machineId);

  setupWifi();
  
  //save the custom parameters to FS
  if (shouldSaveConfig)
  {
      if (DEBUG) { Serial.println("saving config"); }
      DynamicJsonBuffer jsonBuffer;
      JsonObject& json = jsonBuffer.createObject();
      json["mqtt_server"] = mqtt_server;
      json["mqtt_port"] = mqtt_port;
      json["workgroup"] = workgroup;
      json["username"] = username;
      json["password"] = password;

      File configFile = SPIFFS.open("/config.json", "w");
      if (!configFile)
      {
          if (DEBUG) { Serial.println("failed to open config file for writing"); }
      }

      json.printTo(Serial);
      json.printTo(configFile);
      configFile.close();
  }

  if (DEBUG) {
    Serial.println("local ip");
    Serial.println(WiFi.localIP());
  }

  setupMQTT();
    
  if (DEBUG) {
    Serial.println("");
    Serial.println("-----");
    Serial.print("Machine ID: ");
    Serial.println(machineId);
    Serial.println("-----");
    Serial.println("");
  }
}

void loop() 
{
  loopMQTT();
  loopPir();
  loopLED();   
  factoryReset();                             // Press and hold the button to reset to factory defaults
}


/*----------------------------interrupt callbacks----------------------------*/
void pirInterrupt0() {
  if (DEBUG) { Serial.print("pirInterrupt0"); }
  _pirLastTriggered = 0;  //top
  pirInterruptPart2();
}

void pirInterrupt1() {
  if (DEBUG) { Serial.print("pirInterrupt0"); }
  _pirLastTriggered = 1;  //bottom
  pirInterruptPart2();
}

void pirInterruptPart2() {
  if (_state == 0 || _state == 3) {
    _state = 1;                               //if off or fading down, then fade back up again
    _fadeOnDirection = _pirLastTriggered;
  }
  _pirHoldPrevMillis = millis();              //store the current time (reset the timer)
  _timerRunning = true;                       //enable the timer loop in pir
}

