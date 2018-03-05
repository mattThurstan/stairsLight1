/*
    'stairsLight1_test2' by Thurstan. Test for LEDs controlled by motion sensors.
    Copyright (C) 2018  MTS Standish (mattThurstan)
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
 */
  
  
  const byte _pirPin[2] = { 2, 3 };             //2 PIR sensors on interrupt pins (triggered on HIGH)
  const byte _ledDOut0Pin = 4;                  //FastLED strip
  
  volatile byte _lastPirTriggered = -1;         //last PIR sensor triggered (0 or 1)
  
  
  void setup() {
    
    //setup PIR pins and attach interrupts
    pinMode(_pirPin[0], INPUT_PULLUP);
    pinMode(_pirPin[1], INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(_pirPin[0]), pirInterrupt0, RISING);
    attachInterrupt(digitalPinToInterrupt(_pirPin[1]), pirInterrupt1, RISING);

    //setup LEDs
    
  }
  
  void loop() {
    //
  }
  
  void pirInterrupt0() {
    _lastPirTriggered = 0;
    //set On State flag, which then starts/restarts hold timer etc.
  }
  

  void pirInterrupt1() {
    _lastPirTriggered = 1;
    //set On State flag, which then starts/restarts hold timer etc.
  }
  
