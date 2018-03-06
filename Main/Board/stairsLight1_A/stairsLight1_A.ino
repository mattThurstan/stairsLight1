/*
    'stairsLight1_A' by Thurstan. Standalone PIR sensor(s) which switches on LED strip.
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

    https://github.com/mattKsp/
*/

/*
 * Arduino Pro Mini (5v, ATmega328, 16mhz clock speed, 32KB Flash, 2KB SRAM, 1KB EEPROM)
 * http://wiki.keyestudio.com/index.php/Ks0052_keyestudio_PIR_Motion_Sensor 
*/

boolean DEBUG = true;

/*----------------------------arduino pins----------------------------*/

const byte _pirTotal = 2;
const byte _pirPin[_pirTotal] = { 2, 3 };   //2 top, 3 bottom
const byte _ledDOut0Pin = 4;

/*----------------------------libraries----------------------------*/
#include <EEPROM.h>                         //a few saved settings
#include <FastLED.h>                        //WS2812B LED strip control and effects

/*----------------------------system----------------------------*/
const String _progName = "stairsLight1_A";
const String _progVers = "0.124";             //
//const int _mainLoopDelay = 0;               //just in case  - using FastLED.delay instead..
#define UPDATES_PER_SECOND 0           //120      //main loop FastLED show delay - 1000/120

/*----------------------------PIR----------------------------*/
boolean _onOff = false;            //global. this should init false, then get activated by input - on/off true/false
byte _onState[_pirTotal] = { 0, 0 };
byte _offState[_pirTotal] = { 0, 0 };
const unsigned long _pirReadInterval = 10;
byte _pirPrevState[_pirTotal] = { 0, 0 };
const unsigned long _pirDebounceInterval = 310; //debounce for the PIR as it stays on 1 for a while
unsigned long _pirDebouncePrevMillis[_pirTotal] = { 0, 0 };
boolean _pirDebounceTriggered[_pirTotal] = { false, false };                    //latch for PIR debounce
const unsigned long _pirHoldInterval = 150000;            //30000=30 sec. 150000=2.5 mins.
unsigned long _pirHoldPrevMillis[_pirTotal] = { 0, 0 };
byte _pirLastTriggered = 255;

/*----------------------------LED----------------------------*/
typedef struct {
  byte first;
  byte last;
  byte total;                                     //byte ok as haven't got more than 256 LEDs in a segment
} LED_SEGMENT;
const byte _segmentTotal = 1;                     //
const byte _ledNum = 108;                         //
LED_SEGMENT ledSegment[_segmentTotal] = {
  /*{ 0, 16, 17 },*/ 
  { 0, 107, 108 },
};
CRGBArray<_ledNum> _leds;                         //CRGBArray means can do multiple '_leds(0, 2).fadeToBlackBy(40);' as well as single '_leds[0].fadeToBlackBy(40);'

byte _ledGlobalBrightnessCur = 255;               //current global brightness
unsigned long _ledRiseSpeed = 35;                 //speed at which the LEDs turn on (runs backwards)

CHSV _topColorHSV( 50, 80, 159 );                 //0, 0, 200
CHSV _botColorHSV( 50, 80, 159 );                   //0, 0, 200
//CRGB _startColorRGB( 3, 144, 232 );
//CRGB _endColorRGB( 249, 180, 1 );


/*----------------------------MAIN----------------------------*/
void setup()
{
  if (DEBUG) {
    Serial.begin(9600);
    Serial.println();
    Serial.print(_progName);
    Serial.print(" ");
    Serial.print(_progVers);
    Serial.println();
    Serial.print("..");
    Serial.println();
  }
  
  for (byte i = 0; i < _pirTotal; i++) {
    pinMode(_pirPin[i],INPUT);
  }
  
  delay(3000);                              //give the power, LED strip, etc. a couple of secs to stabilise

  FastLED.setMaxPowerInVoltsAndMilliamps(5, 900);  //limit power draw to 0.9A at 5v
  FastLED.addLeds<WS2812B, _ledDOut0Pin, GRB>(_leds, ledSegment[0].first, _ledNum).setCorrection( TypicalSMD5050 );
  FastLED.setBrightness(_ledGlobalBrightnessCur);      //set global brightness
  FastLED.setTemperature(UncorrectedTemperature);   //set first temperature

  FastLED.clear();
  FastLED.show();
}


void loop()
{
  
  EVERY_N_MILLISECONDS(_pirReadInterval) 
  {
    for (byte i = 0; i < _pirTotal; i++) {
      byte state = digitalRead(_pirPin[i]);
      if (state == 1 && _pirDebounceTriggered[i] == false) {
        _pirDebounceTriggered[i] = true;
        _pirDebouncePrevMillis[i] = millis();
        _onOff = true;
        _pirHoldPrevMillis[i] = millis();
        _pirLastTriggered = i;
        if (DEBUG) { Serial.println("PIR triggered!"); }
      } else if (state == 0) { 
        //if (DEBUG) {  Serial.println("No one!"); } 
      }
    }
  }

  for (byte i = 0; i < _pirTotal; i++) {
    if (_pirDebounceTriggered[i] == true) {
      unsigned long pirDebounceCurMillis = millis();    //get current time
      if((unsigned long) (pirDebounceCurMillis - _pirDebouncePrevMillis[i]) >= _pirDebounceInterval) {
        //
        _pirDebounceTriggered[i] = false;
        if (DEBUG) { Serial.println("PIR debounce interval expired!"); }
      }
    }
  }

  loopPir();
  
  FastLED.show();                           //send all the data to the strips
  FastLED.delay(UPDATES_PER_SECOND);        // (1000/UPDATES_PER_SECOND)
  //
  //delay(_mainLoopDelay);  //using FastLED.delay instead..
}
