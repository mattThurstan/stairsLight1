/*----------------------------setup pir----------------------------*/
void setupPIR() {
  if (DEBUG_GEN) { Serial.println("Setup PIR"); }
  //setup PIR pins and attach interrupts
  pinMode(_pirPin[0], INPUT_PULLUP);
  pinMode(_pirPin[1], INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(_pirPin[0]), pirInterrupt0, RISING);
  attachInterrupt(digitalPinToInterrupt(_pirPin[1]), pirInterrupt1, RISING);
}

/*----------------------------loop pir----------------------------*/
// if _dayMode is set to TRUE (day) then the lights will not turn on (breathing is still seperate).

void loopPir()  {
  if (_PIRtriggeredTimerRunning) {
    //pir timer - flips lights state if not in day mode
    unsigned long pirHoldCurMillis = millis();    // get current time
    if( (unsigned long)(pirHoldCurMillis - _pirHoldPrevMillis) >= _pirHoldInterval ) {
      //when the time has expired, do this..
      if ((_state == 1 || _state == 2) && _dayMode == false) {
        _state = 3;
        if (DEBUG_INTERRUPT) { Serial.print("State = 3"); }
      }
      publishSensorTopOff(true);
      publishSensorBotOff(true);
      _PIRtriggeredTimerRunning = false;                      // disable itself
    }
  }
  
  if (_state == 0) {
    //off
    RgbColor color;
    for (uint16_t i = 1; i < strip.PixelCount(); i++)
    {
        color = strip.GetPixelColor(i);
        color.Darken(2); //uint8_t darkenBy
        strip.SetPixelColor(i, color);
    }
  } else if (_state == 1) {
    publishState(true);
    publishBrightness(true);
    publishRGB(true);
    fadeOn();
  } else if (_state == 2) {
    //on
    strip.ClearTo(_colorHSL, ledSegment[1].first, ledSegment[1].last); 
  } else if (_state == 3) {
    publishState(true);
    fadeOff();
  }
}

void fadeOn() {
  _stateSave = _state;                            // interrupt catch
  //_leds.fadeToBlackBy(4);                         // just in case
  if (_fadeOnDirection == 0) {
    //fade on top to bottom
    for (byte i = ledSegment[1].first; i <= ledSegment[1].last; i++) {
      if (_state != _stateSave) { return; }       // interrupt catch
      fadeShowLEDs(ledSegment[1].first, i);
      if (i == ledSegment[1].last) { 
        _state = 2;
        if (DEBUG_INTERRUPT) { Serial.println("State = 2"); }
        return;
      }
    }
  } else if (_fadeOnDirection == 1) {
    //fade on bottom to top
    for (byte i = ledSegment[1].last; i >= ledSegment[1].first; i--) {
      if (_state != _stateSave) { return; }       // interrupt catch
      fadeShowLEDs(i, ledSegment[1].last);
      if (i == ledSegment[1].first) { 
        _state = 2;
        if (DEBUG_INTERRUPT) { Serial.println("State = 2"); }
        return;
      }
    }
  }
}

void fadeOff() {
  _stateSave = _state;                            // interrupt catch
  if (_pirLastTriggered == 0) {
    //fade off bottom to top 
    for (byte i = ledSegment[1].last; i >= ledSegment[1].first; i--) {
      if (_state != _stateSave) { return; }       // interrupt catch
      strip.ClearTo(_rgbBlack);                   // fade em all
      fadeShowLEDs(ledSegment[1].first, i);       // then switch back on the ones we want
      if (i == ledSegment[1].first) { 
        strip.SetPixelColor(1, _rgbBlack);        // turn off the last pixel before changing state
        _state = 0;
        if (DEBUG_INTERRUPT) { Serial.print("State = 0"); }
        return;
      }
    }
  } else if (_pirLastTriggered == 1) {
    //fade off top to bottom
    for (byte i = ledSegment[1].first; i <= ledSegment[1].last; i++) {
      if (_state != _stateSave) { return; }       // interrupt catch
      strip.ClearTo(_rgbBlack);
      fadeShowLEDs(i, ledSegment[1].last);
      if (i == ledSegment[1].last) {    
        strip.SetPixelColor(11, _rgbBlack);       // turn off the last pixel before changing state
        _state = 0;
        if (DEBUG_INTERRUPT) { Serial.print("State = 0"); }
        return;
      }
    }
  }
}
