/*----------------------------setup led----------------------------*/
void setupLED()
{
  delay(3000);                                //give the power, LED strip, etc. a couple of secs to stabilise

  FastLED.setMaxPowerInVoltsAndMilliamps(5, MAX_POWER_DRAW);   //limit power draw to 0.9A at 5v for wall power supply
  FastLED.addLeds<WS2812B, _ledDOut0Pin, GRB>(_leds, ledSegment[0].first, _ledNum).setCorrection( TypicalSMD5050 );
  FastLED.setBrightness(_ledGlobalBrightnessCur);   //set global brightness
  FastLED.setTemperature(UncorrectedTemperature);   //set first temperature

  FastLED.clear();
  FastLED.show();
}

void flashLED(byte led)
{
  if (led == 0) {
    //
  } else if (led == 1) {
    _leds[1] = CRGB::Yellow;
  } else if (led == 2) {
    _leds[2] = CRGB::Fuchsia;
  } else if (led == 3) {
    _leds[3] = CRGB::Orange;
  } else if (led == 4) {
    _leds[4] = CRGB::Violet;
  } else if (led == 5) {
    _leds[5] = CRGB::Teal;
  } else if (led == 6) {
    _leds[6] = CRGB::Pink;
  } else if (led == 7) {
    _leds[7] = CRGB::Green;
  }
  FastLED.show();
  delay(400);
}

/*----------------------------loop led----------------------------*/
void loopLED()
{
  FastLED.show();                             // send all the data to the strips
  FastLED.delay(UPDATES_PER_SECOND);          // (1000/UPDATES_PER_SECOND)
   
  //EVERY_N_SECONDS( 1 ) { 
  if (_modeString == "Fade") {
    EVERY_N_MILLISECONDS( GHUE_CYCLE_TIME ) 
    {
      _gHue++;                                 //slowly cycle the "base color" through the rainbow
      //need to do a rollover catch here for _gHue
      _topColorHSV.h = _gHue;
      _botColorHSV.h = _gHue;
    } 
  }
}

//called from 'pir'
void fadeShowLEDs(byte low, byte high) {
  _leds(low, high).fill_gradient(_topColorHSV, _botColorHSV);
  FastLED.show();
  FastLED.delay(_ledRiseSpeed);
}

void setBrightnessCur(byte brightness) 
{
  FastLED.setBrightness(brightness);
  _ledGlobalBrightnessCur = brightness;
  if (DEBUG) { Serial.print("setBrightnessCur "); }
  if (DEBUG) { Serial.println(brightness); }
}

void setColorTopRGB(CRGB rgb)
{
  CHSV tempHSV = rgb2hsv_approximate(rgb);
  _topColorHSV = tempHSV;
  _gHue = tempHSV.h;
  if (DEBUG) { Serial.print("setColorTopRGB "); }
  if (DEBUG) { Serial.println(rgb); }
}

void setColorBotRGB(CRGB rgb)
{
  CHSV tempHSV = rgb2hsv_approximate(rgb);
  _botColorHSV = tempHSV;
  _gHue = tempHSV.h;
  if (DEBUG) { Serial.print("setColorBotRGB "); }
  if (DEBUG) { Serial.println(rgb); }
}

