/*----------------------------setup led----------------------------*/
void setupLED()
{
  delay(3000);                                //give the power, LED strip, etc. a couple of secs to stabilise

  FastLED.setMaxPowerInVoltsAndMilliamps(5, MAX_POWER_DRAW);   //limit power draw to 0.9A at 5v for wall power supply
  FastLED.addLeds<WS2812B, _ledDOut0Pin, GRB>(_leds, ledSegment[0].first, _ledNum).setCorrection( TypicalSMD5050 );
  FastLED.setBrightness(_ledGlobalBrightnessCur);   //set global brightness
  FastLED.setTemperature(UncorrectedTemperature);   //set first temperature

  _leds[0] = CRGB::White;
  FastLED.show();                             //send all the data to the strips
  FastLED.delay(100);          // (1000/UPDATES_PER_SECOND)
  
  FastLED.clear();
  FastLED.show();
}

/*----------------------------loop led----------------------------*/
void loopLED()
{
  FastLED.show();                             //send all the data to the strips
  FastLED.delay(UPDATES_PER_SECOND);          // (1000/UPDATES_PER_SECOND)
   
  //EVERY_N_SECONDS( 1 ) { 
  EVERY_N_MILLISECONDS( GHUE_CYCLE_TIME ) 
  {
    gHue++;                                   //slowly cycle the "base color" through the rainbow
    _topColorHSV.hue = gHue;
    _botColorHSV.hue = gHue;
  } 
}

void setBrightnessCur(byte brightness) 
{
  FastLED.setBrightness(brightness);
  _ledGlobalBrightnessCur = brightness;
}

void setColorRGB(CRGB rgb)
{
  CHSV tempHSV = rgb2hsv_approximate(rgb);
  _botColorHSV = tempHSV;
  _topColorHSV = tempHSV;
}

