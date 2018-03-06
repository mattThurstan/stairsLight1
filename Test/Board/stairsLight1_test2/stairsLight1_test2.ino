/*
    'stairsLight1_test2' by Thurstan. Test for LEDs controlled by motion sensors.
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

/* 
 * Arduino Pro Mini (5v, ATmega328, 16mhz clock speed, 32KB Flash, 2KB SRAM, 1KB EEPROM)
 * 
 * serial 0,1
 * interrupts 2,3
 * PWM 3,5,6,9,10,11
 * SPI 10(SS),11(MOSI),13(SCK)
 * I2C A4(SDA),A5(SCL)
 * 
 * http://wiki.keyestudio.com/index.php/Ks0052_keyestudio_PIR_Motion_Sensor
 */
  
  
boolean DEBUG = true;
const unsigned long _pirHoldInterval = 30000; //150000;  //15000=15 sec. 30000=30 sec. 150000=2.5 mins.
unsigned long _ledRiseSpeed = 35;                 //speed at which the LEDs turn on (runs backwards)

/*----------------------------arduino pins----------------------------*/
//2=top, 3=bottom - due to the way the LED strip is wired (top to bot) so thats the way the array goes..
const byte _pirPin[2] = { 2, 3 };             //2 PIR sensors on interrupt pins (triggered on HIGH)
const byte _ledDOut0Pin = 4;                  //FastLED strip

/*----------------------------libraries----------------------------*/
#include <FastLED.h>                        //WS2812B LED strip control and effects

/*----------------------------system----------------------------*/
const String _progName = "stairsLight1_test2";
const String _progVers = "0.100";               //
//const int _mainLoopDelay = 0;                   //just in case  - using FastLED.delay instead..
#define UPDATES_PER_SECOND 0           //120    //main loop FastLED show delay - 1000/120

/*----------------------------PIR----------------------------*/
volatile boolean _onOff = false;              //global. this should init false, then get activated by input - on/off true/false
byte _state = 0;                              //0-Off, 1-Fade On, 2-On, 3-Fade Off
//direction for fade on/off determined by last pir triggered
//might not need to debounce PIR input if using RISING on interrupt
//CHECK - think PIR stays HIGH for a duration ( ??? is default, 3m dist and 25ms hold ??? )
volatile unsigned long _pirHoldPrevMillis = 0;
volatile byte _pirLastTriggered = 255;        //last PIR sensor triggered (0=top or 1=bottom)
volatile boolean _timerRunning = false;       //is the hold timer in use?
volatile byte _fadeOnDirection = 255;         //direction for fade on loop. 0=fade down the stairs (top to bot), 1=fade up the stairs (bot to top).

/*----------------------------LED----------------------------*/


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
   
  //setup PIR pins and attach interrupts
  pinMode(_pirPin[0], INPUT_PULLUP);
  pinMode(_pirPin[1], INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(_pirPin[0]), pirInterrupt0, RISING);
  attachInterrupt(digitalPinToInterrupt(_pirPin[1]), pirInterrupt1, RISING);

  //setup LEDs
  
  if (DEBUG) { Serial.println("Setup done."); }
}

void loop() {

  loopPir();
  
  //
  //delay(_mainLoopDelay);  //using FastLED.delay instead..
}

void pirInterrupt0() {
  _pirLastTriggered = 0;  //top
  pirInterruptPart2();
  if (DEBUG) { Serial.println("PIR 0 interrupt triggered!"); }
}

void pirInterrupt1() {
  _pirLastTriggered = 1;  //bottom
  pirInterruptPart2();
  if (DEBUG) { Serial.println("PIR 1 interrupt triggered!"); }
}

void pirInterruptPart2() {
  if (_state == 0 || _state == 3) {
    _state = 1;                               //if off or fading down, fade back up again
    if (DEBUG) { Serial.println("State set to 1"); }
    _fadeOnDirection = _pirLastTriggered;
  }
  _pirHoldPrevMillis = millis();              //store the current time (reset the timer)
  _timerRunning = true;                       //enable the timer loop in pir
}

