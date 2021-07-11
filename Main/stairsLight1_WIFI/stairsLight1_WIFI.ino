/*
    'stairsLight1_WIFI' by Thurstan. LEDs controlled by motion sensors.
    Copyright (C) 2018 MTS Standish (mattThurstan)
    
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
#include <ESP8266WiFi.h>                          // https://github.com/esp8266/Arduino
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>                          // https://github.com/tzapu/WiFiManager
#include <ArduinoJson.h>                          // https://github.com/bblanchon/ArduinoJson
#include <PubSubClient.h>                         // https://github.com/knolleary/pubsubclient
#include <MD5Builder.h>
#include <FastLED.h>                              // WS2812B LED strip control and effects
#include <MT_LightControlDefines.h>


/*----------------------------system----------------------------*/
const String _progName = "stairsLight1_WIFI";
const String _progVers = "0.262";                 // 
#define UPDATES_PER_SECOND 0           //120      // main loop FastLED show delay - 1000/120
#define DEBUG 1

/*----------------------------pins----------------------------*/
//2=top, 3=bottom - due to the way the LED strip is wired (top to bot) so thats the way the array goes..
const byte _pirPin[2] = { 2, 3 };                 // 2 PIR sensors on interrupt pins (triggered on HIGH)
const byte _ledDOut0Pin = 14;                     // FastLED strip

/*----------------------------PIR----------------------------*/
const unsigned long _pirHoldInterval = 30000; //150000;  //15000=15 sec. 30000=30 sec. 150000=2.5 mins.
volatile byte _state = 0;                         // 0-Off, 1-Fade On, 2-On, 3-Fade Off
volatile byte _stateSave = 0;                     // temp save state for inside for-loops
//direction for fade on/off is determined by last pir triggered
volatile unsigned long _pirHoldPrevMillis = 0;
volatile byte _pirLastTriggered = 255;            // last PIR sensor triggered (0=top or 1=bottom)
volatile boolean _timerRunning = false;           // is the hold timer in use?
volatile byte _fadeOnDirection = 255;             // direction for fade on loop. 0=fade down the stairs (top to bot), 1=fade up the stairs (bot to top).

/*----------------------------LED----------------------------*/
#define MAX_POWER_DRAW 450  //900                 // limit the maximum power draw (in milliamps mA)
typedef struct {
  byte first;
  byte last;
  byte total;                                     // using a byte here is ok as we haven't got more than 256 LEDs in a segment
} LED_SEGMENT;
const byte _segmentTotal = 1;                     // runs down stair banister from top to bottom
const byte _ledNum = 20; //108;                   //
LED_SEGMENT ledSegment[_segmentTotal] = {
  //{ 0, 0, 1 },
  //{ 0, 107, 108 },
  { 0, 19, 20 },
};
CRGBArray<_ledNum> _leds;                         // CRGBArray means can do multiple '_leds(0, 2).fadeToBlackBy(40);' as well as single '_leds[0].fadeToBlackBy(40);'

byte _ledGlobalBrightnessCur = 255;               // current global brightness
unsigned long _ledRiseSpeed = 25; //35;           // speed at which the LEDs turn on (runs backwards)
#define GHUE_CYCLE_TIME 200                       // gHue loop update time (in milliseconds)
uint8_t _gHue = 0;                                // incremental "base color" used by loop
CHSV _topColorHSV( 50, 150, 255 );                // 0, 0, 200  -  50, 80, 159
CHSV _botColorHSV( 50, 150, 255 );                // 0, 0, 200  -  50, 80, 159


/*----------------------------MQTT----------------------------*/
unsigned long mqttConnectionPreviousMillis = millis();
const long mqttConnectionInterval = 60000;

char mqtt_server[] = MQTT_BROKER_IP;
char mqtt_port[] = "1883"; //MQTT_BROKER_PORT;
//uint16_t mqtt_broker_port = MQTT_BROKER_PORT;
char workgroup[] = WORKGROUP_NAME;
char username[] = MQTT_BROKER_USERNAME;
char password[] = MQTT_BROKER_PASSWORD;

char machineId[32] = "";                          // MD5 of chip ID
bool shouldSaveConfig = false;                    // flag for saving data

// MQTT
WiFiClient espClient;
PubSubClient mqttClient(espClient);
//PubSubClient mqttClient(MQTT_BROKER_IP, MQTT_BROKER_PORT, mqttCallback, espClient);

const uint8_t MSG_BUFFER_SIZE = 50;
char m_msg_buffer[MSG_BUFFER_SIZE];               // buffer used to send/receive data with MQTT

//broadcast states and subscribe to commands
const PROGMEM char* MQTT_LIGHTS_TOPIC_STATE = "house/stairs/lights/light/status";
const PROGMEM char* MQTT_LIGHTS_TOPIC_COMMAND = "house/stairs/lights/light/switch";

const PROGMEM char* MQTT_LIGHTS_BRIGHTNESS_TOPIC_STATE = "house/stairs/lights/brightness/status";
const PROGMEM char* MQTT_LIGHTS_BRIGHTNESS_TOPIC_COMMAND = "house/stairs/lights/brightness/set";

//const PROGMEM char* MQTT_LIGHTS_HUE_TOPIC_STATE = "house/stairs/lights/hue/status";
//const PROGMEM char* MQTT_LIGHTS_HUE_TOPIC_COMMAND = "house/stairs/lights/hue/set";

const PROGMEM char* MQTT_LIGHTS_TOP_RGB_TOPIC_STATE = "house/stairs/lights/top/rgb/status";
const PROGMEM char* MQTT_LIGHTS_TOP_RGB_TOPIC_COMMAND = "house/stairs/lights/top/rgb/set";

const PROGMEM char* MQTT_LIGHTS_BOT_RGB_TOPIC_STATE = "house/stairs/lights/bot/rgb/status";
const PROGMEM char* MQTT_LIGHTS_BOT_RGB_TOPIC_COMMAND = "house/stairs/lights/bot/rgb/set";

//const PROGMEM char* LIGHTS_ON = "ON";
//const PROGMEM char* LIGHTS_OFF = "OFF";

const PROGMEM char* MQTT_SENSORS_TOP_TOPIC_STATE = "house/stairs/sensors/top/status";
const PROGMEM char* MQTT_SENSORS_BOT_TOPIC_STATE = "house/stairs/sensors/bot/status";

const PROGMEM char* MQTT_LIGHTS_MODE = "house/stairs/lights/mode";
//char* _effect = "Normal";
String _modeString = "Fade";  //Normal


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
  
  loadConfigAndSettings();
    //flashLED(1);  // can't flash LED before LED's setup.......
  setupLED();
    flashLED(2);
  setupPIR();
    flashLED(3);
  calculateMachineId();
    flashLED(4);

  // Set MQTT topics
  //sprintf(cmnd_power_topic, "cmnd/%s/power", machineId);
  //sprintf(cmnd_color_topic, "cmnd/%s/color", machineId);
  //sprintf(stat_power_topic, "stat/%s/power", machineId);
  //sprintf(stat_color_topic, "stat/%s/color", machineId);

  setupWifi();
    flashLED(5);
  saveConfig();
    flashLED(6);
  setupMQTT();
    flashLED(7);
    
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
  factoryReset();              //TODO               // Press and hold the button to reset to factory defaults
}


/*----------------------------interrupt callbacks----------------------------*/
void pirInterrupt0() {
  if (DEBUG) { Serial.println("pirInterrupt0"); }
  _pirLastTriggered = 0;  //top
  //publishSensorTop();
  mqttClient.publish(MQTT_SENSORS_BOT_TOPIC_STATE, LIGHTS_ON, true);
  pirInterruptPart2();
}

void pirInterrupt1() {
  if (DEBUG) { Serial.println("pirInterrupt0"); }
  _pirLastTriggered = 1;  //bottom
  //publishSensorBot();
  mqttClient.publish(MQTT_SENSORS_TOP_TOPIC_STATE, LIGHTS_ON, true);
  pirInterruptPart2();
}

void pirInterruptPart2() {
  if (_state == 0 || _state == 3) {
    _state = 1;                                   //if off or fading down, then fade back up again
    _fadeOnDirection = _pirLastTriggered;
  }
  _pirHoldPrevMillis = millis();                  //store the current time (reset the timer)
  _timerRunning = true;                           //enable the timer loop in pir
}
