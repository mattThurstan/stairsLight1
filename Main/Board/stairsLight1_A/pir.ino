
void loopPir() 
{

  if (_onOff) {
  
    for (byte i = 0; i < _pirTotal; i++) {
      if (_onState[i] == 0) {
        if (DEBUG) { Serial.println("LED state 0 - fading on"); }
        if (i == 0) {  
          ledsFadeOn_Top2Bot(); //pin 2 top
        } else if (i == 1) { 
          ledsFadeOn_Bot2Top(); //pin 3 bottom
        }
      } else {
        _leds( ledSegment[0].first, ledSegment[0].last).fill_gradient(_topColorHSV, _botColorHSV);
        if (DEBUG) { Serial.println("LED state 1 - on"); }
      }
    }
  
  } else {
    
    if (_pirLastTriggered == 0) {
      if (_offState[0] == 0) {
        ledsFadeOff_Bot2Top();
      } else {
        _leds.fadeToBlackBy(4); //should already be black, just in case
      }
    } else if (_pirLastTriggered == 1) {
      if (_offState[1] == 0) {
        ledsFadeOff_Top2Bot();
      } else {
        _leds.fadeToBlackBy(4); //should already be black, just in case
      }
    }
    //_leds.fadeToBlackBy(4);
  } //END if _onOff

  for (byte i = 0; i < _pirTotal; i++) {
    unsigned long pirHoldCurMillis = millis();    //get current time
    if( ( (unsigned long) (pirHoldCurMillis - _pirHoldPrevMillis[i]) >= _pirHoldInterval ) && _onOff) {
      //when the time has expired, do this..
      //note all led fade loops should complete within this time.
      _onState[i] = 0;
      _onOff = false;
      _pirHoldPrevMillis[i] = millis();              //store the current time
      if (DEBUG) { 
        Serial.print("PIR ");
        Serial.print(i);
        Serial.println(" hold interval expired!"); 
      }
    }
  }
    
} //END loopPir

void ledsFadeOn_All() { }

void ledsFadeOn_Top2Bot() 
{
  for (byte i = ledSegment[0].first; i <= ledSegment[0].last; i++) {
    _offState[0] = 0;
    _leds[i] = _topColorHSV;
    FastLED.show();
    FastLED.delay(_ledRiseSpeed);
    if (i == ledSegment[0].last) { _onState[0] = 1; }
  }
}

void ledsFadeOn_Bot2Top() 
{
  for (byte i = ledSegment[0].last; i >= ledSegment[0].first; i--) {
    _offState[1] = 0;
    _leds[i] = _botColorHSV;
    FastLED.show();
    FastLED.delay(_ledRiseSpeed);
    if (i == ledSegment[0].first) { _onState[1] = 1; }
  }
}

void ledsFadeOff_All() 
{
  //for(int i = 0; i < _ledNum; i++) { _leds[i].nscale8(250); }
  _leds.fadeToBlackBy(4);
}

void ledsFadeOff_Top2Bot() 
{
  for (byte i = ledSegment[0].first; i <= ledSegment[0].last; i++) {
    _leds[i] = CRGB::Black;
    FastLED.show();
    FastLED.delay(_ledRiseSpeed);
    if (i == ledSegment[0].last) { _offState[1] = 1; }
  }
}

void ledsFadeOff_Bot2Top() 
{
  for (byte i = ledSegment[0].last; i >= ledSegment[0].first; i--) {
    _leds[i] = CRGB::Black;
    FastLED.show();
    FastLED.delay(_ledRiseSpeed);
    if (i == ledSegment[0].first) { _offState[0] = 1; }
  }
}


//

//void loop() {
//  meteorRain(0xff,0xff,0xff,10, 64, true, 30);
//}
/*
void meteorRain(byte red, byte green, byte blue, byte meteorSize, byte meteorTrailDecay, boolean meteorRandomDecay, int SpeedDelay) {  
  setAll(0,0,0);
 
  for(int i = 0; i < NUM_LEDS+NUM_LEDS; i++) {
   
   
    // fade brightness all LEDs one step
    for(int j=0; j<NUM_LEDS; j++) {
      if( (!meteorRandomDecay) || (random(10)>5) ) {
        fadeToBlack(j, meteorTrailDecay );        
      }
    }
   
    // draw meteor
    for(int j = 0; j < meteorSize; j++) {
      if( ( i-j <NUM_LEDS) && (i-j>=0) ) {
        setPixel(i-j, red, green, blue);
      }
    }
   
    showStrip();
    delay(SpeedDelay);
  }
}
*/

