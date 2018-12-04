/*----------------------------internal mesh messages - get passed to/from MQTT broker by bridge----------------------------*/
void receiveMessage(uint32_t from, String msg)
{
  uint8_t firstMsgIndex = msg.indexOf(':');
  String targetSub = msg.substring(0, firstMsgIndex);
  String msgSub = msg.substring(firstMsgIndex);

  if (targetSub == "lights/light/switch")
  {
    if (msgSub == LIGHTS_ON)  //"ON"
    {
      if (DEBUG) { Serial.print(targetSub); Serial.println(msgSub); }
      if (_state == 0) {
        pirInterrupt0();  //trigger bot sensor
        publishState();
      }
    }
    else if (msgSub == LIGHTS_OFF)  //"OFF"
    {
      if (_state != 0) {
        _state = 3; //force a fade out
        //does a 'publish state' from 'loopPir' when finished fade out
      }
    }
  } 
  else if (targetSub == "lights/brightness/set")
  {
    uint8_t brightness = msgSub.toInt();
    if (DEBUG) { Serial.print(targetSub); Serial.println(brightness); }
    if (brightness < 0 || brightness > 255) {
      // do nothing...
      return;
    } else {
      setBrightnessCur(brightness);
      publishBrightness();
    }
  }
  else if (targetSub == "lights/top/rgb/set")
  {
    CRGB tempRGB;
    if (DEBUG) { Serial.print(targetSub); Serial.println(msgSub); }
  
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
    publishTopRGB();
  }
  else if (targetSub == "lights/bot/rgb/set")
  {
    CRGB tempRGB;
    if (DEBUG) { Serial.print(targetSub); Serial.println(msgSub); }
  
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
    publishBotRGB();
  }
  else if (targetSub == "lights/mode")
  {
    if (msgSub == "Normal" || msgSub == "Fade") {
      _modeString = msgSub;
      if (DEBUG) { Serial.print(targetSub); Serial.println(msgSub); }
      publishMode();
    }
  }
  else if (targetSub == "sunrise")
  {
    if (msgSub == LIGHTS_ON) {
      
      if (DEBUG) { Serial.print(targetSub); Serial.println(msgSub); }
      //publishMode();
    }
  }
  else if (targetSub == "sunset")
  {
    if (msgSub == LIGHTS_ON) {
      
      if (DEBUG) { Serial.print(targetSub); Serial.println(msgSub); }
      //publishMode();
    }
  }
  
}

void publishState()
{
  if (DEBUG) { Serial.print("publishState "); }
  String msg = "lights/light/status";
  msg += ":"; //..just so we are all sure what is going on here !?
  if(_state == 0) {
    msg += LIGHTS_OFF;
  } else {
    msg += LIGHTS_ON;
  }
  uint32_t id = DEVICE_ID_BRIDGE1;
  mesh.sendSingle(id, msg);
  //mesh.sendBroadcast(msg);
  if (DEBUG) { Serial.println(msg); }
  shouldSaveSettings = true;
}

void publishBrightness()
{
  if (DEBUG) { Serial.print("publishBrightness "); }
  String msg = "lights/brightness/status";
  msg += ":"; //..just so we are all sure what is going on here !?
  msg += String(_ledGlobalBrightnessCur);
  uint32_t id = DEVICE_ID_BRIDGE1;
  mesh.sendSingle(id, msg);
  if (DEBUG) { Serial.println(msg); }
  shouldSaveSettings = true;
}

//void publishHue()
//{
//  snprintf(m_msg_buffer, MSG_BUFFER_SIZE, "%d", gHue);
//  mqttClient.publish(MQTT_LIGHTS_HUE_TOPIC_STATE, m_msg_buffer, true);
//}

//currently using hsv, need to convert
void publishTopRGB()
{
  CRGB tempRGB = _topColorHSV;
  if (DEBUG) { Serial.print("publishTopRGB "); }
  String msg = "lights/top/rgb/status";
  msg += ":"; //..just so we are all sure what is going on here !?
  msg += String(tempRGB.r);
  msg += ",";
  msg += String(tempRGB.g);
  msg += ",";
  msg += String(tempRGB.b);
  uint32_t id = DEVICE_ID_BRIDGE1;
  mesh.sendSingle(id, msg);
  if (DEBUG) { Serial.println(msg); } 
  shouldSaveSettings = true;
}

void publishBotRGB()
{
  CRGB tempRGB = _botColorHSV;
  if (DEBUG) { Serial.print("publishBotRGB "); }
  String msg = "lights/bot/rgb/status";
  msg += ":"; //..just so we are all sure what is going on here !?
  msg += String(tempRGB.r);
  msg += ",";
  msg += String(tempRGB.g);
  msg += ",";
  msg += String(tempRGB.b);
  uint32_t id = DEVICE_ID_BRIDGE1;
  mesh.sendSingle(id, msg);
  if (DEBUG) { Serial.println(msg); }
  shouldSaveSettings = true;
}

void publishSensorTop()
{
  if (DEBUG) { Serial.print("publishSensorTop "); }
  String msg = "sensors/top/status";
  msg += ":"; //..just so we are all sure what is going on here !?
  if ( (_state == 0 || _state == 3) && _pirLastTriggered == 0) {
    msg += LIGHTS_OFF;
  } else {
    msg += LIGHTS_ON;
  }
  uint32_t id = DEVICE_ID_BRIDGE1;
  mesh.sendSingle(id, msg);
  if (DEBUG) { Serial.println(msg); }
  shouldSaveSettings = true;
}

void publishSensorBot()
{
  if (DEBUG) { Serial.print("publishSensorBot "); }
  String msg = "sensors/bot/status";
  msg += ":"; //..just so we are all sure what is going on here !?
  if ( (_state == 0 || _state == 3) && _pirLastTriggered == 1) {
    msg += LIGHTS_OFF;
  } else {
    msg += LIGHTS_ON;
  }
  uint32_t id = DEVICE_ID_BRIDGE1;
  mesh.sendSingle(id, msg);
  if (DEBUG) { Serial.println(msg); }
  shouldSaveSettings = true;
}

void publishMode()
{
  if (DEBUG) { Serial.print("publishMode "); }
  String msg = "lights/mode";
  msg += ":"; //..just so we are all sure what is going on here !?
  msg += _modeString;
  uint32_t id = DEVICE_ID_BRIDGE1;
  mesh.sendSingle(id, msg);
  if (DEBUG) { Serial.println(msg); }
  shouldSaveSettings = true;
}

