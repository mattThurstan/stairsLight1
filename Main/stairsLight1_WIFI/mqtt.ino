/*----------------------------setup MQTT----------------------------*/
void setupMQTT()
{
  if (DEBUG) {
    Serial.print("MQTT Server: ");
    Serial.println(mqtt_server);
    Serial.print("MQTT Port: ");
    Serial.println(mqtt_port);
    // Print MQTT Username
    Serial.print("MQTT Username: ");
    Serial.println(username);
    // Hide password from the log and show * instead
    char hiddenpass[20] = "";
    for (size_t charP=0; charP < strlen(password); charP++)
    {
        hiddenpass[charP] = '*';
    }
    hiddenpass[strlen(password)] = '\0';
    Serial.print("MQTT Password: ");
    Serial.println(hiddenpass);
  }

  const int mqttPort = atoi(mqtt_port);
  mqttClient.setServer(mqtt_server, mqttPort);
  mqttClient.setCallback(mqttCallback);

  mqttReconnect();
}

/*----------------------------MQTT callback----------------------------*/
void loopMQTT()
{
  mqttClient.loop();
  
  // Reconnect if there is an issue with the MQTT connection
  const unsigned long mqttConnectionMillis = millis();
  if ( (false == mqttClient.connected()) && (mqttConnectionInterval <= (mqttConnectionMillis - mqttConnectionPreviousMillis)) )
  {
    mqttConnectionPreviousMillis = mqttConnectionMillis;
    mqttReconnect();
  }
}

void mqttReconnect()
{
    for (int attempt = 0; attempt < 3; ++attempt)
    {
        //Loop until we're reconnected
        if (DEBUG) { Serial.print("Attempting MQTT connection..."); }
        // Create a random client ID
        //String clientId = "ESP8266Client-";
        //clientId += String(random(0xffff), HEX);
        const String clientId = "Stairs_lights";
        // Attempt to connect
        if (true == mqttClient.connect(clientId.c_str(), username, password))
        {
            if (DEBUG) { Serial.println("connected"); }

            // Publish current stats
            publishState();
            publishBrightness();
            //publishHue();
            publishTopRGB();
            publishBotRGB();
            //publishSensorTop();
            //publishSensorBot();

            // Subscribe to MQTT topics
            //mqttClient.subscribe(cmnd_power_topic);
            //mqttClient.subscribe(cmnd_color_topic);
            mqttClient.subscribe(MQTT_LIGHTS_TOPIC_COMMAND);
            mqttClient.subscribe(MQTT_LIGHTS_BRIGHTNESS_TOPIC_COMMAND);
            //mqttClient.subscribe(MQTT_LIGHTS_HUE_TOPIC_COMMAND);
            mqttClient.subscribe(MQTT_LIGHTS_TOP_RGB_TOPIC_COMMAND);
            mqttClient.subscribe(MQTT_LIGHTS_BOT_RGB_TOPIC_COMMAND);
            break;

        }
        else
        {
          if (DEBUG) { 
            Serial.print("failed, rc=");
            Serial.print(mqttClient.state());
            Serial.println(" try again in 5 seconds");
          }
          // Wait 5 seconds before retrying
          delay(5000);
        }
    }
}

/*----------------------------MQTT ..the rest----------------------------*/
void mqttCallback(char* p_topic, byte* p_payload, unsigned int p_length)
{
  if (DEBUG) { Serial.println("mqttCallback"); }
  // 1st version - concat the payload into a string
  String payload;
  for (uint8_t i = 0; i < p_length; i++) {
    payload.concat((char)p_payload[i]);
  }
  // 2nd version - snprintf received bytes to a string
  //char text[length + 1];
  //snprintf(text, length + 1, "%s", p_payload);
  
  // handle message topic
  if (String(MQTT_LIGHTS_TOPIC_COMMAND).equals(p_topic)) 
  {
    // test if the payload is equal to "ON" or "OFF"
    if (payload.equals(String(LIGHTS_ON))) 
    {
      if (DEBUG) { Serial.print(MQTT_LIGHTS_TOPIC_COMMAND); Serial.println(payload); }
      if (_state == 0) {
        //m_rgb_state = true;
        //setColor(m_rgb_red, m_rgb_green, m_rgb_blue);
        //publishRGBState();
        pirInterrupt0();  //trigger bot sensor
        publishState();
      }
    } else if (payload.equals(String(LIGHTS_OFF))) {
      if (_state != 0) {
        //m_rgb_state = false;
        //setColor(0, 0, 0);
        //publishRGBState();
        _state = 3; //force a fade out
        //publishState();
      }
    }
  } 
  else if (String(MQTT_LIGHTS_BRIGHTNESS_TOPIC_COMMAND).equals(p_topic)) 
  {
    uint8_t brightness = payload.toInt();
    if (DEBUG) { Serial.print(MQTT_LIGHTS_BRIGHTNESS_TOPIC_COMMAND); Serial.println(brightness); }
    if (brightness < 0 || brightness > 255) {
      // do nothing...
      return;
    } else {
      //_ledGlobalBrightnessCur = brightness;
      //setColor(m_rgb_red, m_rgb_green, m_rgb_blue);
      setBrightnessCur(brightness);
      //publishRGBBrightness();
      publishBrightness();
    }
  } 
//  else if (String(MQTT_LIGHTS_HUE_TOPIC_COMMAND).equals(p_topic))
//  {
//    uint8_t hue = payload.toInt();
//    if (DEBUG) { Serial.print(MQTT_LIGHTS_HUE_TOPIC_COMMAND); Serial.println(brightness); }
//    if (hue < 0 || hue > 255) {
//      // do nothing...
//      return;
//    } else {
//      //_ledGlobalBrightnessCur = brightness;
//      //setColor(m_rgb_red, m_rgb_green, m_rgb_blue);
//      //setBrightnessCur();
//      //publishRGBBrightness();
//      gHue = hue; 
//      publishHue();
//    }
//  }
  else if (String(MQTT_LIGHTS_TOP_RGB_TOPIC_COMMAND).equals(p_topic)) 
  {
    CRGB tempRGB;
    if (DEBUG) { Serial.print(MQTT_LIGHTS_TOP_RGB_TOPIC_COMMAND); Serial.println(payload); }
  
    // get the position of the first and second commas
    uint8_t firstIndex = payload.indexOf(',');
    uint8_t lastIndex = payload.lastIndexOf(',');
    
    uint8_t rgb_red = payload.substring(0, firstIndex).toInt();
    if (rgb_red < 0 || rgb_red > 255) {
      return;
    } else {
      tempRGB.r = rgb_red;
    }
    
    uint8_t rgb_green = payload.substring(firstIndex + 1, lastIndex).toInt();
    if (rgb_green < 0 || rgb_green > 255) {
      return;
    } else {
      tempRGB.g = rgb_green;
    }
    
    uint8_t rgb_blue = payload.substring(lastIndex + 1).toInt();
    if (rgb_blue < 0 || rgb_blue > 255) {
      return;
    } else {
      tempRGB.b = rgb_blue;
    }

    setColorTopRGB(tempRGB);
    publishTopRGB();
  }
  else if (String(MQTT_LIGHTS_BOT_RGB_TOPIC_COMMAND).equals(p_topic)) 
  {
    CRGB tempRGB;
    if (DEBUG) { Serial.print(MQTT_LIGHTS_BOT_RGB_TOPIC_COMMAND); Serial.println(payload); }
  
    // get the position of the first and second commas
    uint8_t firstIndex = payload.indexOf(',');
    uint8_t lastIndex = payload.lastIndexOf(',');
    
    uint8_t rgb_red = payload.substring(0, firstIndex).toInt();
    if (rgb_red < 0 || rgb_red > 255) {
      return;
    } else {
      tempRGB.r = rgb_red;
    }
    
    uint8_t rgb_green = payload.substring(firstIndex + 1, lastIndex).toInt();
    if (rgb_green < 0 || rgb_green > 255) {
      return;
    } else {
      tempRGB.g = rgb_green;
    }
    
    uint8_t rgb_blue = payload.substring(lastIndex + 1).toInt();
    if (rgb_blue < 0 || rgb_blue > 255) {
      return;
    } else {
      tempRGB.b = rgb_blue;
    }

    setColorBotRGB(tempRGB);
    publishBotRGB();
  }
  else if (String(MQTT_LIGHTS_MODE).equals(p_topic)) 
  {
    if (payload == "Normal" || payload == "Fade") {
      _modeString = payload;
      if (DEBUG) { Serial.print(MQTT_LIGHTS_MODE); Serial.println(payload); }
      publishMode();
    }
  } 
  
}

/*
void mqttCallbackORIG(char* topic, byte* payload, unsigned int length)
{
  // Convert received bytes to a string
  char text[length + 1];
  snprintf(text, length + 1, "%s", payload);

  if (DEBUG) { 
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    Serial.println(text);
  }

    if (strcmp(topic, cmnd_power_topic) == 0)
    {
        power = strcmp(text, "ON") == 0;
    }
    else if (strcmp(topic, cmnd_color_topic) == 0)
    {
        StaticJsonBuffer<200> jsonBuffer;
        JsonObject& data = jsonBuffer.parseObject(text);

        if (data.containsKey("color"))
        {
            const int r = data["color"]["r"];
            const int g = data["color"]["g"];
            const int b = data["color"]["b"];
            currentRed = ((0 <= r) && (255 >= r)) ? r : 0;
            currentGreen = ((0 <= g) && (255 >= g)) ? g : 0;
            currentBlue = ((0 <= b) && (255 >= b)) ? b : 0;
        }
        if (data.containsKey("brightness"))
        {
            const int brightness = data["brightness"];
            if ( (0 <= brightness) && (255 >= brightness) )
            {
                brightnessLevel = brightness;
            }
        }
        calculateBrightness();
        if (data.containsKey("state"))
        {
            power = data["state"] == "ON";
        } else if (data.containsKey("brightness") || data.containsKey("color")) {
            // Turn on if any of the colors is greater than 0
            // Only if *either* color or brightness have been set.
            power = ( (0 < lightRed) || (0 < lightGreen) || (0 < lightBlue) );
        }
    }

    publishState();
  
  if (DEBUG) { 
    //Serial.print("Red: ");
    //Serial.println(lightRed);
    //Serial.print("Green: ");
    //Serial.println(lightGreen);
    //Serial.print("Blue: ");
    //Serial.println(lightBlue);
    Serial.print("Power: ");
    Serial.println(power);
  }

    // Set colors of RGB LED strip
    if (power)
    {
        //analogWrite(pinLedRed, lightRed);
        //analogWrite(pinLedGreen, lightGreen);
        //analogWrite(pinLedBlue, lightBlue);
    }
    else
    {
        //analogWrite(pinLedRed, 0);
        //analogWrite(pinLedGreen, 0);
        //analogWrite(pinLedBlue, 0);
    }
}
*/

/*----------------------------publish MQTT----------------------------*/
void publishState()
{
  if (DEBUG) { Serial.print("publishState "); }
  if(_state == 0) {
    mqttClient.publish(MQTT_LIGHTS_TOPIC_STATE, LIGHTS_OFF, true);
    if (DEBUG) { Serial.println(LIGHTS_OFF); }
  } else {
    mqttClient.publish(MQTT_LIGHTS_TOPIC_STATE, LIGHTS_ON, true);
    if (DEBUG) { Serial.println(LIGHTS_ON); }
  }
}

void publishBrightness()
{
  snprintf(m_msg_buffer, MSG_BUFFER_SIZE, "%d", _ledGlobalBrightnessCur);
  mqttClient.publish(MQTT_LIGHTS_BRIGHTNESS_TOPIC_STATE, m_msg_buffer, true);
  if (DEBUG) { Serial.print("publishBrightness "); }
  if (DEBUG) { Serial.println(_ledGlobalBrightnessCur); }
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
  snprintf(m_msg_buffer, MSG_BUFFER_SIZE, "%d,%d,%d", tempRGB.r, tempRGB.g, tempRGB.b);
  mqttClient.publish(MQTT_LIGHTS_TOP_RGB_TOPIC_STATE, m_msg_buffer, true);
  if (DEBUG) { Serial.print("publishTopRGB "); }
  if (DEBUG) { Serial.println(tempRGB); }
}

void publishBotRGB()
{
  CRGB tempRGB = _botColorHSV;
  snprintf(m_msg_buffer, MSG_BUFFER_SIZE, "%d,%d,%d", tempRGB.r, tempRGB.g, tempRGB.b);
  mqttClient.publish(MQTT_LIGHTS_BOT_RGB_TOPIC_STATE, m_msg_buffer, true);
  if (DEBUG) { Serial.print("publishBotRGB "); }
  if (DEBUG) { Serial.println(tempRGB); }
}

void publishSensorTop()
{
  if (DEBUG) { Serial.print("publishSensorTop "); }
  if ( (_state == 0 || _state == 3) && _pirLastTriggered == 0) {
    mqttClient.publish(MQTT_SENSORS_TOP_TOPIC_STATE, LIGHTS_OFF, true);
    if (DEBUG) { Serial.println(LIGHTS_OFF); }
  } else {
    mqttClient.publish(MQTT_SENSORS_TOP_TOPIC_STATE, LIGHTS_ON, true);
    if (DEBUG) { Serial.println(LIGHTS_ON); }
  }
}

void publishSensorBot()
{
  if (DEBUG) { Serial.print("publishSensorBot "); }
  if ( (_state == 0 || _state == 3) && _pirLastTriggered == 1) {
    mqttClient.publish(MQTT_SENSORS_BOT_TOPIC_STATE, LIGHTS_OFF, true);
    if (DEBUG) { Serial.println(LIGHTS_OFF); }
  } else {
    mqttClient.publish(MQTT_SENSORS_BOT_TOPIC_STATE, LIGHTS_ON, true);
    if (DEBUG) { Serial.println(LIGHTS_ON); }
  }
}

void publishMode()
{
  //effectString.c_str();
  //snprintf(m_msg_buffer, MSG_BUFFER_SIZE, "%d", _ledGlobalBrightnessCur);
  mqttClient.publish(MQTT_LIGHTS_MODE, _modeString.c_str(), true);
  if (DEBUG) { Serial.print("publishMode "); }
  if (DEBUG) { Serial.println(_modeString); }
}


/*
void publishState()
{
  StaticJsonBuffer<150> jsonBuffer;
  char payload[150] = {0};
  JsonObject& json = jsonBuffer.createObject();
  const char* state = power ? "ON" : "OFF";
  json["state"] = state;
  json["brightness"] = brightnessLevel;

  JsonObject& color = json.createNestedObject("color");
  //color["r"] = power ? currentRed : 0;
  //color["g"] = power ? currentGreen : 0;
  //color["b"] = power ? currentBlue : 0;

  json.printTo((char*)payload, json.measureLength() + 1);

  if (DEBUG) { 
    Serial.print("[");
    Serial.print(stat_color_topic);
    Serial.print("] ");
    Serial.println(payload);
  }
  mqttClient.publish(stat_color_topic, payload, true);

  if (DEBUG) { 
    Serial.print("[");
    Serial.print(stat_power_topic);
    Serial.print("] ");
    Serial.println(state);
  }
  mqttClient.publish(stat_power_topic, state, true);
}
*/

