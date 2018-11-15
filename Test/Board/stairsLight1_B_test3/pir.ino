/*----------------------------setup pir----------------------------*/
void setupPIR()
{
  //setup PIR pins and attach interrupts
  pinMode(_pirPin[0], INPUT_PULLUP);
  pinMode(_pirPin[1], INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(_pirPin[0]), pirInterrupt0, RISING);
  attachInterrupt(digitalPinToInterrupt(_pirPin[1]), pirInterrupt1, RISING);
}

/*----------------------------loop pir----------------------------*/
void loopPir() 
{
  if (_timerRunning) {
    //lights on hold timer
    unsigned long pirHoldCurMillis = millis();    //get current time
    if( (unsigned long)(pirHoldCurMillis - _pirHoldPrevMillis) >= _pirHoldInterval ) {
      //when the time has expired, do this..
      if (_state == 1 || _state == 2) {
        _state = 3;
      }
      _timerRunning = false;                      //disable itself
    }
  }
  
  if (_state == 0) {
    //off
    _leds.fadeToBlackBy(4);
  } else if (_state == 1) {
    //fade on
    fadeOn();
  } else if (_state == 2) {
    //on
  _leds(ledSegment[0].first, ledSegment[0].last).fill_gradient(_topColorHSV, _botColorHSV);
  } else if (_state == 3) {
    //fade off
    fadeOff();
    publishState();
  }
}

void fadeOn() {
  _stateSave = _state;                        //interrupt catch
  _leds.fadeToBlackBy(4);       //just in case
  if (_fadeOnDirection == 0) {
    //fade on top to bottom
    for (byte i = ledSegment[0].first; i <= ledSegment[0].last; i++) {
      if (_state != _stateSave) { return; }   //interrupt catch
      
      fadeShowLEDs(ledSegment[0].first, i);
      if (i == ledSegment[0].last) { 
        _state = 2;
        return;
      }
    }
  } else if (_fadeOnDirection == 1) {
    //fade on bottom to top
    for (byte i = ledSegment[0].last; i >= ledSegment[0].first; i--) {
      if (_state != _stateSave) { return; }   //interrupt catch
      fadeShowLEDs(i, ledSegment[0].last);
      if (i == ledSegment[0].first) { 
        _state = 2;
        return;
      }
    }
  }
}

void fadeOff() {
  _stateSave = _state;                        //interrupt catch
  if (_pirLastTriggered == 0) {
    //fade off bottom to top 
    for (byte i = ledSegment[0].last; i >= ledSegment[0].first; i--) {
      if (_state != _stateSave) { return; }   //interrupt catch
      _leds = CRGB::Black;                    //fade em all
      fadeShowLEDs(ledSegment[0].first, i);   //then switch back on the ones we want
      if (i == ledSegment[0].first) {
        _leds = CRGB::Black;                  //turn off the last pixel before changing state
        _state = 0;
        return;
      }
    }
  } else if (_pirLastTriggered == 1) {
    //fade off top to bottom
    for (byte i = ledSegment[0].first; i <= ledSegment[0].last; i++) {
      if (_state != _stateSave) { return; }   //interrupt catch
      _leds = CRGB::Black;
      fadeShowLEDs(i, ledSegment[0].last);
      if (i == ledSegment[0].last) { 
        _leds = CRGB::Black;                  //turn off the last pixel before changing state
        _state = 0;
        return;
      }
    }
  }
}

void fadeShowLEDs(byte low, byte high) {
  _leds(low, high).fill_gradient(_topColorHSV, _botColorHSV);
  FastLED.show();
  FastLED.delay(_ledRiseSpeed);
}

