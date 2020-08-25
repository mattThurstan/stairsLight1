/*
    'stairsLight1_Mesh' by Thurstan. LEDs controlled by motion sensors.
    Copyright (C) 2020 MTS Standish (Thurstan|mattKsp)
    
    https://github.com/mattThurstan/
    
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
    
    WeMos D1 (R2 &) mini ESP8266, 80 MHz, 115200 baud, 4M, (1M SPIFFS)
*/


#include <FS.h>                                   // file system
#include <FastLED.h>                              // still using some bits
#include <NeoPixelBrightnessBus.h>                // NeoPixelBrightnessBus (just for ESP8266)- for brightness functions (instead of NeoPixelBus.h)
#include "painlessMesh.h"
#include <MT_LightControlDefines.h>


/*----------------------------system----------------------------*/
const String _progName = "stairsLight1_Mesh";
const String _progVers = "0.402";                 // working

boolean DEBUG_GEN = false;                        // realtime serial debugging output - general
boolean DEBUG_OVERLAY = false;                    // show debug overlay on leds (eg. show segment endpoints, center, etc.)
boolean DEBUG_MESHSYNC = false;                   // show painless mesh sync by flashing some leds (no = count of active mesh nodes) 
boolean DEBUG_COMMS = false;                      // realtime serial debugging output - comms
boolean DEBUG_INTERRUPT = false;                  // realtime serial debugging output - interrupt pins
boolean DEBUG_USERINPUT = false;                  // realtime serial debugging output - user input

boolean _firstTimeSetupDone = false;              // starts false //this is mainly to catch an interrupt trigger that happens during setup, but is usefull for other things
//volatile boolean _onOff = true; //flip _state // issues with mqtt and init false // this should init false, then get activated by input - on/off true/false
bool _shouldSaveSettings = false;                 // flag for saving data
bool _runonce = true;                             // flag for sending states when first mesh conection
//const int _mainLoopDelay = 0;                     // just in case  - using FastLED.delay instead..
bool _isBreathing = false;                        // toggle for breath
bool _isBreathOverlaid = false;                   // toggle for whether breath is overlaid on top of modes
bool _isBreathingSynced = false;                  // breath sync local or global

/*----------------------------pins----------------------------*/
// NeoPixelBus - For Esp8266, the Pin is omitted and it uses GPIO3 (RX) due to DMA hardware use. 
//2=top, 3=bottom - due to the way the LED strip is wired (top to bot) so thats the way the array goes..
const byte _pirPin[2] = { 5, 4 }; // D1, D2       // 2 PIR sensors on interrupt pins (triggered on HIGH)

/*----------------------------modes----------------------------*/
const int _modeNum = 2;                           // normal, cycle (gHue)
volatile int _modeCur = 1;                        // current mode in use
String _modeName[_modeNum] = { "Normal", "Cycle" };

/*----------------------------PIR----------------------------*/
const unsigned long _pirHoldInterval = 30000; //150000; // 15000=15 sec. 30000=30 sec. 150000=2.5 mins.
volatile byte _state = 0;                         // 0-Off, 1-Fade On, 2-On, 3-Fade Off
volatile byte _stateSave = 0;                     // temp save state for inside for-loops
//direction for fade on/off is determined by last pir triggered
volatile unsigned long _pirHoldPrevMillis = 0;
volatile byte _pirLastTriggered = 255;            // last PIR sensor triggered (0=top or 1=bottom)
volatile boolean _timerRunning = false;           // is the hold timer in use?
volatile byte _fadeOnDirection = 255;             // direction for fade on loop. 0=fade down the stairs (top to bot), 1=fade up the stairs (bot to top).
// crash at boot with ISR not in IRAM error
void ICACHE_RAM_ATTR pirInterrupt0();
void ICACHE_RAM_ATTR pirInterrupt1();
//void ICACHE_RAM_ATTR pirInterruptPart2();

/*----------------------------LED----------------------------*/
const uint16_t _ledNum = 99;                      // NeoPixelBus - 98 + 1 LEDs
NeoPixelBrightnessBus<NeoGrbFeature, Neo800KbpsMethod> strip(_ledNum);

typedef struct {
  byte first;
  byte last;
  byte total;                                     // using a byte here is ok as we haven't got more than 256 LEDs in a segment
} LED_SEGMENT;
const byte _segmentTotal = 2;                     // (1 + 1) runs down stair banister from top to bottom
LED_SEGMENT ledSegment[_segmentTotal] = {
  { 0, 0, 1 },  // sacrificial level changer
  { 1, 99, 98 }
};

uint8_t _ledGlobalBrightnessCur = 255;            // current global brightness - adjust this
uint8_t _ledBrightnessIncDecAmount = 10;          // the brightness amount to increase or decrease
unsigned long _ledRiseSpeed = 25; //35;           // speed at which the LEDs turn on (runs backwards)
uint8_t _ledRiseSpeedSaved = 25;                  // cos of saving / casting unsigned long issues - use 0-255 via mqtt
uint8_t _gHue2 = 0;                               // incremental cycling "base color", 0-100, converted to 0-1
uint8_t _gHue2saved = 0;                          // used to revert color when going back to 'Normal' mode
unsigned long _gHue2CycleMillis = 200UL;          // gHue loop update time (millis)
uint8_t _gHue2CycleSaved = 50;                    // 0-255 mapped to millis range
uint8_t _gHue2CycleMultiplier = 4;                // (__gHue2CycleSaved * _gHue2CycleMultiplier) = (unsigned long) _gHue2CycleMillis
unsigned long _gHue2PrevMillis;                   // gHue loop previous time (millis)

RgbColor _rgbRed(255, 0, 0);
RgbColor _rgbGreen(0, 255, 0);
RgbColor _rgbBlue(0, 0, 255);
RgbColor _rgbYellow(255, 255, 0);
RgbColor _rgbFuchsia(255, 0, 128);
RgbColor _rgbOrange(255, 165, 0);
RgbColor _rgbViolet(148, 0, 211);
RgbColor _rgbTeal(0, 128, 128);
RgbColor _rgbPink(255, 105, 180);
RgbColor _rgbWhite(255, 255, 255);
RgbColor _rgbBlack(0, 0, 0);

//HslColor color(_gHue / 255.0f, saturationValue, lightnessValue); // 0.0 to 1.0
HslColor _colorHSL(0.25f, 0.5f, 0.5f);

/*----------------------------Mesh----------------------------*/
painlessMesh  mesh;                               // initialise
uint32_t id = DEVICE_ID_BRIDGE1;

void receivedCallback(uint32_t from, String &msg ) {
  if (DEBUG_COMMS) { Serial.printf("stairsLight1_Mesh: Received from %u msg=%s\n", from, msg.c_str()); }
  receiveMessage(from, msg);
}

void newConnectionCallback(uint32_t nodeId) {
  if (_runonce == true) {
    publishStatusAll(false);
    _runonce = false;
  }
  if (DEBUG_COMMS) { Serial.printf("--> stairsLight1_Mesh: New Connection, nodeId = %u\n", nodeId); }
}

void changedConnectionCallback() {
  //publish..
  if (DEBUG_COMMS) { Serial.printf("Changed connections %s\n",mesh.subConnectionJson().c_str()); }
}

void nodeTimeAdjustedCallback(int32_t offset) {
  if (DEBUG_COMMS) { Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset); }
}

void delayReceivedCallback(uint32_t from, int32_t delay) {
  if (DEBUG_COMMS) { Serial.printf("Delay to node %u is %d us\n", from, delay); }
}


/*----------------------------MAIN----------------------------*/
void setup() {
  
  // LED strip - Wemos D1 - GPIO 3 (RX) - swap the pin from serial to a GPIO.
  pinMode(3, FUNCTION_3); // FUNCTION_0 = swap back

  // start serial regardless but control debug output from mqtt
  Serial.begin(115200);
  
  Serial.println();
  Serial.print(_progName);
  Serial.print(" v");
  Serial.print(_progVers);
  Serial.println();
  Serial.print("..");
  Serial.println();
  
  delay(3000);                                    // give the power, LED strip, etc. a couple of secs to stabilise
  
  loadSettings();
  
  setupLEDs();
  strip.ClearTo(_rgbBlack);
  strip.SetPixelColor(0, _rgbYellow);
  strip.Show();
  delay(400);
  
  setupPIR();
  strip.ClearTo(_rgbBlack);
  strip.SetPixelColor(0, _rgbBlue);
  strip.Show();
  delay(400);
  
  setupMesh();
  strip.ClearTo(_rgbBlack);
  strip.SetPixelColor(0, _rgbViolet);
  strip.Show();
  delay(400);

  //everything done? ok then..
  Serial.print(F("Setup done"));
  Serial.println("-----");
  Serial.print(F("Device Node ID is "));
  String s = String(mesh.getNodeId());
  Serial.println(s);
  Serial.println("-----");
  Serial.println("");
  
  strip.ClearTo(_rgbBlack);
  strip.SetPixelColor(0, _rgbGreen);
  strip.Show();
  delay(1500);
  strip.ClearTo(_rgbBlack);
}

void loop()  {
  
  if(_firstTimeSetupDone == false) {
    _firstTimeSetupDone = true;                   // need this for stuff like setting sunrise, cos it needs the time to have been set
    if (DEBUG_GEN) { Serial.print(F("firstTimeSetupDone  = true")); }
  }

  mesh.update();
  
  loopPir();
  gHueRotate();
  
  if (DEBUG_OVERLAY) {
    showSegmentEndpoints();
  } else {
    strip.SetPixelColor(0, _rgbBlack);            // modes are responsible for all other leds
  }
  
  if (DEBUG_MESHSYNC) { }
 
  EVERY_N_SECONDS(60) {                           // too much ???
    if (_shouldSaveSettings == true)
    { 
      saveSettings(); 
      _shouldSaveSettings = false; 
    }
  }
  //factoryReset();              //TODO           // Press and hold the button to reset to factory defaults

  strip.Show();
  //delay(_mainLoopDelay); 
}

/*----------------------------interrupt callbacks----------------------------*/
void pirInterrupt0() {
  if (DEBUG_INTERRUPT) { Serial.println(F("pirInterrupt0")); }
  _pirLastTriggered = 0;  //top
  pirInterruptPart2();
}

void pirInterrupt1() {
  if (DEBUG_INTERRUPT) { Serial.println(F("pirInterrupt1")); }
  _pirLastTriggered = 1;  //bottom
  pirInterruptPart2();
}

void pirInterruptPart2() {
  if (_state == 0 || _state == 3) {
    _state = 1;                                   // if off or fading down, then fade back up again
    _fadeOnDirection = _pirLastTriggered;
  }
  if (_pirLastTriggered == 0) {
    publishSensorTop(true);
  } else if (_pirLastTriggered == 1) {
    publishSensorBot(true);
  }
  _pirHoldPrevMillis = millis();                  // store the current time (reset the timer)
  _timerRunning = true;                           // enable the timer loop in pir
}
