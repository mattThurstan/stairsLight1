/*----------------------------internal mesh messages - get passed to/from MQTT broker by bridge----------------------------*/
void receiveMessage(uint32_t from, String msg)
{
  uint8_t firstMsgIndex = msg.indexOf(':');
  String targetSub = msg.substring(0, firstMsgIndex);
  String msgSub = msg.substring(firstMsgIndex+1);

  if (targetSub == "lights/light/switch")
  {
    if (msgSub == LIGHTS_ON) {
      if (_state == 0 || _state == 3) {
        _state = 1; // fade on
      }
    }
    else if (msgSub == LIGHTS_OFF) {
      if (_state == 1 || _state == 2) {
        _state = 3; // fade out
      }
    }
    publishState(true);
  }
  else if (targetSub == "lights/day/set") {
    if (msgSub == LIGHTS_ON) { 
      _dayMode = true;
      if (_state == 1 || _state == 2) {
        _state = 3; // fade out - does a 'publish state' from 'loopPir' when finished fade out
      }
    }
    else if (msgSub == LIGHTS_OFF) { 
      // normal service is resumed
      _dayMode = false;
    }
    publishDayMode(true);
  }
  else if (targetSub == "lights/brightness/set")
  {
    uint8_t brightness = msgSub.toInt();
    if (brightness < 0 || brightness > 255) {
      // do nothing...
      return;
    } else {
      setGlobalBrightness(brightness);
      publishBrightness(true);
    }
  }
  else if (targetSub == "lights/rgb/set")
  {
    RgbColor tempRGB;
  
    // get the position of the first and second commas
    uint8_t firstIndex = msgSub.indexOf(',');
    uint8_t lastIndex = msgSub.lastIndexOf(',');
    
    uint8_t rgb_red = msgSub.substring(0, firstIndex).toInt();
    if (rgb_red < 0 || rgb_red > 255) {
      return;
    } else {
      tempRGB.R = rgb_red;
    }
    
    uint8_t rgb_green = msgSub.substring(firstIndex + 1, lastIndex).toInt();
    if (rgb_green < 0 || rgb_green > 255) {
      return;
    } else {
      tempRGB.G = rgb_green;
    }
    
    uint8_t rgb_blue = msgSub.substring(lastIndex + 1).toInt();
    if (rgb_blue < 0 || rgb_blue > 255) {
      return;
    } else {
      tempRGB.B = rgb_blue;
    }

    setColorHSL(tempRGB);
    publishRGB(true);
  }
  else if (targetSub == "lights/mode")
  {
    if (msgSub == "Normal") {
      _modeCur = 0;
      _gHue2 = _gHue2saved;
      checkAndSetColorHSL_H();
    } else if (msgSub == "Cycle") {
      _modeCur = 1;
    }
    //publishMode(true);  // too much bounce back on network
  }
  else if (targetSub == "lights/risespeed/set")
  {
    uint8_t risespeed = msgSub.toInt();
    if (risespeed < 0 || risespeed > 255) {
      // do nothing...
      return;
    } else {
      _ledRiseSpeedSaved = risespeed;
      checkAndSetLedRiseSpeed();
      publishRiseSpeed(true);
    }
  }
  else if (targetSub == "lights/hue/cycle/set")
  {
    uint8_t hueCycleLoopTime = msgSub.toInt();
    if (hueCycleLoopTime < 0 || hueCycleLoopTime > 255) {
      // do nothing...
      return;
    } else {
      _gHue2CycleSaved = hueCycleLoopTime;
      checkAndSetGHue2CycleMillis();
      publishGHue2Cycle(true);
    }
  }
  else if (targetSub == "sunrise") {
    // trigger only (global synced)
    if (msgSub == LIGHTS_ON) {
      //start sunrise
    }
    else if (msgSub == LIGHTS_OFF) {
      //stop sunrise and revert to previous setting
    }
  }
  else if (targetSub == "lights/sunrise") {
    // trigger only
    // note: the single mesh msg of 'sunrise' is used for synced global sunrise
    if (msgSub == LIGHTS_ON) {
      //start sunrise
      //publishMode(true);
    }
    else if (msgSub == LIGHTS_OFF) {
      //stop sunrise and revert to previous setting
      //publishMode(true);
    }
    //else if (msgSub == "receive a time for sunrise to happen at") {
    //set sunrise time
    //}
  }
  else if (targetSub == "sunset") {
    // trigger only (global synced)
    if (msgSub == LIGHTS_ON) {
      //start sunset
    }
    else if (msgSub == LIGHTS_OFF) {
      //stop sunset and revert to previous setting
    }
  }
  else if (targetSub == "lights/sunset") {
    // trigger only
    // note: the single mesh msg of 'sunset' is used for synced global sunset
    if (msgSub == LIGHTS_ON) {
      //start sunset
      //publishMode(true);
    }
    else if (msgSub == LIGHTS_OFF) {
      //stop sunset and revert to previous setting
      //publishMode(true);
    }
    //else if (msgSub == "receive a time for sunset to happen at") {
    //set sunset time
    //}
  }
  /*
   * Breath : (noun) Refers to a full cycle of breathing. It can also refer to the air that is inhaled or exhaled.
   */
  else if (targetSub == "breath")
  {
    // trigger only (global synced)
    // note: the single mesh msg of 'breath' is used for synced global breathing
    if (msgSub == ON) {
      _isBreathingSynced = true;                    // set sync to global
      _isBreathing = true;                          // start synced breathing
    }
    else if (msgSub == OFF) {
      _isBreathing = false;                         // stop breathing
      _isBreathingSynced = false;                   // set sync to local
    }
    //publishBreath(false);
  }
  else if (targetSub == "lights/breath")
  {
    // trigger only (local)
    // note: the single mesh msg of 'breath' is used for synced global breathing
    if (msgSub == ON) {
      _isBreathingSynced = false;                   // set sync to local
      _isBreathing = true;                          // start local breathing
    }
    else if (msgSub == OFF) {
      _isBreathing = false;                         // stop breathing
      _isBreathingSynced = false;                   // set sync to local
    }
    //publishLightsBreath(false);
  }
  else if (targetSub == "lights/breath/xyz")
  {
    // msg will contain xyz coords for position within the house
    //
    //publishLightsBreathXyzSet(false);
  }
  else if (targetSub == "lights/breath/xyz/mode")
  {
    // set positional mode
    if (msgSub == "Independent") { }
    else if (msgSub == "Global") { }
    //publishLightsBreathXyzMode(false);
  }
  else if(targetSub == "debug/general/set") 
  {
    if (msgSub == ON) { DEBUG_GEN = true; } 
    else if (msgSub == OFF) { DEBUG_GEN = false; }
    publishDebugGeneralState(false);
  }
  else if (targetSub == "debug/overlay/set")
  {
    if (msgSub == ON) { DEBUG_OVERLAY = true; }
    else if (msgSub == OFF) { DEBUG_OVERLAY = false; }
    publishDebugOverlayState(false);
  }
  else if (targetSub == "debug/meshsync/set")
  {
    if (msgSub == ON) { DEBUG_MESHSYNC = true; }
    else if (msgSub == OFF) { DEBUG_MESHSYNC = false; }
    publishDebugMeshsyncState(false);
  }
  else if(targetSub == "debug/comms/set") 
  {
    if (msgSub == ON) { DEBUG_COMMS = true; } 
    else if (msgSub == OFF) { DEBUG_COMMS = false; }
    publishDebugCommsState(false);
  }
  // don't really need an ON msg but using just to sure it wasn't sent in error
  else if(targetSub == "debug/reset") { if (msgSub == ON) { doReset(); } }
  else if(targetSub == "debug/restart") 
  {
    uint8_t restartTime = msg.toInt();
    if (restartTime < 0 || restartTime > 255) { return; /* do nothing... */ } 
    else { doRestart(restartTime); }
  }
  else if(targetSub == "reset") { if (msgSub == ON) { doReset(); } }
  else if(targetSub == "restart") 
  {
    uint8_t restartTime = msg.toInt();
    if (restartTime < 0 || restartTime > 255) { return; /* do nothing... */ } 
    else { doRestart(restartTime); }
  }
  else if(targetSub == "lockdown") 
  {
    uint8_t severity = msg.toInt();
    if (severity < 0 || severity > 255) { return; /* do nothing... */ } 
    else { doLockdown(severity); }
  }
  else if(targetSub == "status/request") { if (msgSub == ON) { publishStatusAll(false); }  }
  
  if (DEBUG_COMMS) { Serial.print(targetSub); Serial.print(" : "); Serial.println(msgSub); }
}
