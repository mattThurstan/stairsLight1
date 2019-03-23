/*
    'stairsLight1_Standalone' by Thurstan. LEDs controlled by motion sensors.
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
#include <FastLED.h>                              // WS2812B LED strip control and effects


/*----------------------------system----------------------------*/
const String _progName = "stairsLight1_Standalone";
const String _progVers = "0.260";                 // moved to standalone
#define UPDATES_PER_SECOND 0           //120      // main loop FastLED show delay - 1000/120
#define DEBUG 0

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

String _modeString = "Fade";  //Normal


/*----------------------------MAIN----------------------------*/
void setup()
{
  if (DEBUG) {
    Serial.begin(115200);
    Serial.println();
  
    Serial.println();
    Serial.print(_progName);
    Serial.print(" ");
    Serial.print(_progVers);
    Serial.println();
    Serial.print("..");
    Serial.println();
  }
  
  setupLED();
    flashLED(1);
  setupPIR();
    flashLED(2);
    
}

void loop() 
{
  loopPir();
  loopLED();   
}


/*----------------------------interrupt callbacks----------------------------*/
void pirInterrupt0() {
  if (DEBUG) { Serial.println("pirInterrupt0"); }
  _pirLastTriggered = 0;  //top
  pirInterruptPart2();
}

void pirInterrupt1() {
  if (DEBUG) { Serial.println("pirInterrupt0"); }
  _pirLastTriggered = 1;  //bottom
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

