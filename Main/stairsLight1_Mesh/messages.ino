/*----------------------------internal mesh messages - get passed to/from MQTT broker by bridge----------------------------*/
void receiveMessage(uint32_t from, String msg)
{
  uint8_t firstMsgIndex = msg.indexOf(':');
  String targetSub = msg.substring(0, firstMsgIndex);
  String msgSub = msg.substring(firstMsgIndex+1);

  if (targetSub == "lights/light/switch")
  {
    if (msgSub == LIGHTS_ON)  //"ON"
    {
      if (_state == 0) {
        pirInterrupt0();  //trigger bot sensor
        publishState(true);
      }
    }
    else if (msgSub == LIGHTS_OFF)  //"OFF"
    {
      if (_state != 0) {
        _state = 3; //force a fade out
        //does a 'publish state' from 'loopPir' when finished fade out
      }
    }
    if (DEBUG_COMMS) { Serial.print(targetSub); Serial.print(" : "); Serial.println(msgSub); }
  } 
  else if (targetSub == "lights/brightness/set")
  {
    uint8_t brightness = msgSub.toInt();
    if (DEBUG_COMMS) { Serial.print(targetSub); Serial.println(brightness); }
    if (brightness < 0 || brightness > 255) {
      // do nothing...
      return;
    } else {
      setBrightnessCur(brightness);
      publishBrightness(true);
    }
  }
  else if (targetSub == "lights/top/rgb/set")
  {
    CRGB tempRGB;
    if (DEBUG_COMMS) { Serial.print(targetSub); Serial.println(msgSub); }
  
    // get the position of the first and second commas
    uint8_t firstIndex = msgSub.indexOf(',');
    uint8_t lastIndex = msgSub.lastIndexOf(',');
    
    uint8_t rgb_red = msgSub.substring(0, firstIndex).toInt();
    if (rgb_red < 0 || rgb_red > 255) {
      return;
    } else {
      tempRGB.r = rgb_red;
    }
    
    uint8_t rgb_green = msgSub.substring(firstIndex + 1, lastIndex).toInt();
    if (rgb_green < 0 || rgb_green > 255) {
      return;
    } else {
      tempRGB.g = rgb_green;
    }
    
    uint8_t rgb_blue = msgSub.substring(lastIndex + 1).toInt();
    if (rgb_blue < 0 || rgb_blue > 255) {
      return;
    } else {
      tempRGB.b = rgb_blue;
    }

    setColorTopRGB(tempRGB);
    publishTopRGB(true);
  }
  else if (targetSub == "lights/bot/rgb/set")
  {
    CRGB tempRGB;
    if (DEBUG_COMMS) { Serial.print(targetSub); Serial.println(msgSub); }
  
    // get the position of the first and second commas
    uint8_t firstIndex = msgSub.indexOf(',');
    uint8_t lastIndex = msgSub.lastIndexOf(',');
    
    uint8_t rgb_red = msgSub.substring(0, firstIndex).toInt();
    if (rgb_red < 0 || rgb_red > 255) {
      return;
    } else {
      tempRGB.r = rgb_red;
    }
    
    uint8_t rgb_green = msgSub.substring(firstIndex + 1, lastIndex).toInt();
    if (rgb_green < 0 || rgb_green > 255) {
      return;
    } else {
      tempRGB.g = rgb_green;
    }
    
    uint8_t rgb_blue = msgSub.substring(lastIndex + 1).toInt();
    if (rgb_blue < 0 || rgb_blue > 255) {
      return;
    } else {
      tempRGB.b = rgb_blue;
    }

    setColorBotRGB(tempRGB);
    publishBotRGB(true);
  }
  else if (targetSub == "lights/mode")
  {
    if (msgSub == "Normal" || msgSub == "Fade") {
      _modeString = msgSub;
      publishMode(true);
    }
    if (DEBUG_COMMS) { Serial.print(targetSub); Serial.println(msgSub); }
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
    if (DEBUG_COMMS) { Serial.print(targetSub); Serial.println(msgSub); }
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
    if (DEBUG_COMMS) { Serial.print(targetSub); Serial.println(msgSub); }
  }
  /*
   * Breath : (noun) Refers to a full cycle of breathing. It can also refer to the air that is inhaled or exhaled.
   */
  else if (targetSub == "lights/breath")
  {
    if (msgSub == LIGHTS_ON) {
      
      //publishMode(true);
    }
    else if (msgSub == LIGHTS_OFF) {
      
      //publishMode(true);
    }
    if (DEBUG_COMMS) { Serial.print(targetSub); Serial.println(msgSub); }
  }
  else if (targetSub == "lights/breath/xyz")
  {
    // msg will contain xyz coords for position within the house
    if (DEBUG_COMMS) { Serial.print(targetSub); Serial.println(msgSub); }
  }
  else if (targetSub == "lights/breath/xyz/mode")
  {
    // set positional mode
    // independent, global
    if (DEBUG_COMMS) { Serial.print(targetSub); Serial.println(msgSub); }
  }
  else if(targetSub == "debug/general") 
  {
    if(msg == "ON") { DEBUG_GEN = true; } 
    else if(msg == "OFF") { DEBUG_GEN = false; }
  }
  else if (targetSub == "debug/overlay")
  {
    if (msgSub == LIGHTS_ON) { DEBUG_OVERLAY = true; }
    else if (msgSub == LIGHTS_OFF) { DEBUG_OVERLAY = false; }
    if (DEBUG_COMMS) { Serial.print(targetSub); Serial.println(msgSub); }
  }
  else if (targetSub == "debug/meshsync")
  {
    if (msgSub == LIGHTS_ON) { DEBUG_MESHSYNC = true; }
    else if (msgSub == LIGHTS_OFF) { DEBUG_MESHSYNC = false; }
    if (DEBUG_COMMS) { Serial.print(targetSub); Serial.println(msgSub); }
  }
  else if(targetSub == "debug/comms") 
  {
    if(msg == "ON") { DEBUG_COMMS = true; } 
    else if(msg == "OFF") { DEBUG_COMMS = false; }
    if (DEBUG_COMMS) { Serial.print(targetSub); Serial.println(msgSub); }
  }
  
}

/*----------------------------send messages----------------------------*/
void publishState(bool save)
{
  if (DEBUG_COMMS) { Serial.print("publishState "); }
  String msg = "lights/light/status";
  msg += ":"; //..just so we are all sure what is going on here !?
  if(_state == 0) {
    msg += LIGHTS_OFF;
  } else {
    msg += LIGHTS_ON;
  }
  mesh.sendSingle(id, msg);
  //mesh.sendBroadcast(msg);
  if (DEBUG_COMMS) { Serial.println(msg); }
  if (save == true) { shouldSaveSettings = true; }
}

void publishBrightness(bool save)
{
  if (DEBUG_COMMS) { Serial.print("publishBrightness "); }
  String msg = "lights/brightness/status";
  msg += ":"; //..just so we are all sure what is going on here !?
  msg += String(_ledGlobalBrightnessCur);
  mesh.sendSingle(id, msg);
  if (DEBUG_COMMS) { Serial.println(msg); }
  if (save == true) { shouldSaveSettings = true; }
}

//void publishHue()
//{
//  snprintf(m_msg_buffer, MSG_BUFFER_SIZE, "%d", gHue);
//  mqttClient.publish(MQTT_LIGHTS_HUE_TOPIC_STATE, m_msg_buffer, true);
//}

//currently using hsv, need to convert
void publishTopRGB(bool save)
{
  CRGB tempRGB = _topColorHSV;
  if (DEBUG_COMMS) { Serial.print("publishTopRGB "); }
  String msg = "lights/top/rgb/status";
  msg += ":"; //..just so we are all sure what is going on here !?
  msg += String(tempRGB.r);
  msg += ",";
  msg += String(tempRGB.g);
  msg += ",";
  msg += String(tempRGB.b);
  mesh.sendSingle(id, msg);
  if (DEBUG_COMMS) { Serial.println(msg); } 
  if (save == true) { shouldSaveSettings = true; }
}

void publishBotRGB(bool save)
{
  CRGB tempRGB = _botColorHSV;
  if (DEBUG_COMMS) { Serial.print("publishBotRGB "); }
  String msg = "lights/bot/rgb/status";
  msg += ":"; //..just so we are all sure what is going on here !?
  msg += String(tempRGB.r);
  msg += ",";
  msg += String(tempRGB.g);
  msg += ",";
  msg += String(tempRGB.b);
  mesh.sendSingle(id, msg);
  if (DEBUG_COMMS) { Serial.println(msg); }
  if (save == true) { shouldSaveSettings = true; }
}

void publishSensorTop(bool save)
{
  if (DEBUG_COMMS) { Serial.print("publishSensorTop "); }
  String msg = "sensors/top/status";
  msg += ":"; //..just so we are all sure what is going on here !?
  if (_state == 0 || _state == 3) { // && _pirLastTriggered == 0) {
    msg += LIGHTS_OFF;
  } else {
    msg += LIGHTS_ON;
  }
  mesh.sendSingle(id, msg);
  if (DEBUG_COMMS) { Serial.println(msg); }
  if (save == true) { shouldSaveSettings = true; }
}

void publishSensorBot(bool save)
{
  if (DEBUG_COMMS) { Serial.print("publishSensorBot "); }
  String msg = "sensors/bot/status";
  msg += ":"; //..just so we are all sure what is going on here !?
  if (_state == 0 || _state == 3) { // && _pirLastTriggered == 1) {
    msg += LIGHTS_OFF;
  } else {
    msg += LIGHTS_ON;
  }
  mesh.sendSingle(id, msg);
  if (DEBUG_COMMS) { Serial.println(msg); }
  if (save == true) { shouldSaveSettings = true; }
}

void publishMode(bool save)
{
  if (DEBUG_COMMS) { Serial.print("publishMode "); }
  String msg = "lights/mode";
  msg += ":"; //..just so we are all sure what is going on here !?
  msg += _modeString;
  mesh.sendSingle(id, msg);
  if (DEBUG_COMMS) { Serial.println(msg); }
  if (save == true) { shouldSaveSettings = true; }
}

