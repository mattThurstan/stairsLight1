/*----------------------------pir----------------------------*/
void loopPir() 
{
  if (_timerRunning) {
    //lights on hold timer
    unsigned long pirHoldCurMillis = millis();    //get current time
    if( (unsigned long)(pirHoldCurMillis - _pirHoldPrevMillis) >= _pirHoldInterval ) {
      //when the time has expired, do this..
      if (_state == 1 || _state == 2) {
        _state = 3;
        if (DEBUG) { Serial.println("State set to 3"); }
      }
      _timerRunning = false;                      //disable itself
      if (DEBUG) { 
        Serial.print(".");
        Serial.println("PIR hold interval expired. State set to 3 (fade off), Timer disabled."); 
      }
    }
  }
  
  if (_state == 0) {
    if (DEBUG) { Serial.println("State = 0 (off)"); }
    //off
  } else if (_state == 1) {
    if (DEBUG) { Serial.println("State = 1 (fade on)"); }
    //fade on
    fadeOn();
  } else if (_state == 2) {
    if (DEBUG) { Serial.println("State = 2 (on)"); }
    //on
  } else if (_state == 3) {
    if (DEBUG) { Serial.println("State = 3 (fade off)"); }
    //fade off
    fadeOff();
  }
}

void fadeOn() {
  if (_fadeOnDirection == 0) {
    //fade on top to bottom
    for (byte i = ledSegment[0].first; i <= ledSegment[0].last; i++) {
      fadeShowLEDs(ledSegment[0].first, i);
      if (i == ledSegment[0].last) { 
        _state = 2; 
        if (DEBUG) { Serial.println("State set to 2"); }
        return;
      }
    }
  } else if (_fadeOnDirection == 1) {
    //fade on bottom to top
    for (byte i = ledSegment[0].last; i >= ledSegment[0].first; i--) {
      fadeShowLEDs(i, ledSegment[0].last);
      if (i == ledSegment[0].first) { 
        _state = 2; 
        if (DEBUG) { Serial.println("State set to 2"); }
        return;
      }
    }
  }
  //see if this works here.. see below
  //_state = 2;
  //if (DEBUG) { Serial.println("State set to 2"); }
}

void fadeOff() {
  if (_pirLastTriggered == 0) {
    //fade off bottom to top 
    for (byte i = ledSegment[0].last; i >= ledSegment[0].first; i--) {
      //fade em all
      fadeShowLEDs(ledSegment[0].first, i); //then switch back on the ones we want
      if (i == ledSegment[0].first) {
        //turn off the last pixel before changing state
        _state = 0; 
        if (DEBUG) { Serial.println("State set to 0"); }
        return;
      }
    }
  } else if (_pirLastTriggered == 1) {
    //fade off top to bottom
    for (byte i = ledSegment[0].first; i <= ledSegment[0].last; i++) {
      //fade em all
      fadeShowLEDs(i, ledSegment[0].last);
      if (i == ledSegment[0].last) { 
        //turn off the last pixel before changing state
        _state = 0; 
        if (DEBUG) { Serial.println("State set to 0"); }
        return;
      }
    }
  }
  //see if this works.. 
  //due to FastLED non-blocking delay, this might, or not, only reach this after the increment loop has ended.
  //whichever, amend the other one above or this one to match
//  _state = 0;
//  if (DEBUG) { Serial.println("State set to 0"); }
}

void fadeShowLEDs(byte low, byte high) {
  //_leds(low, high).fill_gradient(_topColorHSV, _botColorHSV);
  //FastLED.show();
  //FastLED.delay(_ledRiseSpeed);
}

