/*----------------------------memory----------------------------*/
void loadConfigAndSettings()
{
    //read configuration from FS json
    if (DEBUG) { Serial.println("mounting FS..."); }

    if (SPIFFS.begin())
    {
        if (DEBUG) { Serial.println("mounted file system"); }
        if (SPIFFS.exists("/config.json")) {
            //file exists, reading and loading
            if (DEBUG) { Serial.println("reading config file"); }
            File configFile = SPIFFS.open("/config.json", "r");
            if (configFile)
            {
                if (DEBUG) { Serial.println("opened config file"); }
                const size_t size = configFile.size();
                // Allocate a buffer to store contents of the file.
                std::unique_ptr<char[]> buf(new char[size]);

                configFile.readBytes(buf.get(), size);
                DynamicJsonBuffer jsonBuffer;
                JsonObject& json = jsonBuffer.parseObject(buf.get());
                json.printTo(Serial);
                if (json.success())
                {
                    if (DEBUG) { Serial.println("\nparsed json"); }

                    strcpy(mqtt_server, json["mqtt_server"]);
                    strcpy(mqtt_port, json["mqtt_port"]);
                    strcpy(workgroup, json["workgroup"]);
                    strcpy(username, json["username"]);
                    strcpy(password, json["password"]);

                }
                else
                {
                    if (DEBUG) { Serial.println("failed to load json config"); }
                }
            }
        }
        if (SPIFFS.exists("/settings.json")) {
            //file exists, reading and loading
            if (DEBUG) { Serial.println("reading user settings file"); }
            File configFile = SPIFFS.open("/settings.json", "r");
            if (configFile)
            {
                if (DEBUG) { Serial.println("opened user settings file"); }
                const size_t size = configFile.size();
                // Allocate a buffer to store contents of the file.
                std::unique_ptr<char[]> buf(new char[size]);

                configFile.readBytes(buf.get(), size);
                DynamicJsonBuffer jsonBuffer;
                JsonObject& json = jsonBuffer.parseObject(buf.get());
                json.printTo(Serial);
                if (json.success())
                {
                    if (DEBUG) { Serial.println("\nparsed json"); }

                    //strcpy(mqtt_server, json["mqtt_server"]);
                    _ledGlobalBrightnessCur = json["gBrightnessCur"];
                    _ledRiseSpeed = json["riseSpeed"];
                    //GHUE_CYCLE_TIME = json["gHueCycleTime"];
                    //gHue = json["gHue"];
                    _topColorHSV.h = json["topColorHSV_h"];
                    _topColorHSV.s = json["topColorHSV_s"];
                    _topColorHSV.v = json["topColorHSV_v"];
                    _botColorHSV.h = json["botColorHSV_h"];
                    _botColorHSV.s = json["botColorHSV_s"];
                    _botColorHSV.v = json["botColorHSV_v"];
                    //_pirHoldInterval
                    
                }
                else
                {
                    if (DEBUG) { Serial.println("failed to load json user settings"); }
                }
            }
        }
    }
    else
    {
        if (DEBUG) { Serial.println("failed to mount FS"); }
    }
   
}

void saveConfig()
{
  //save the custom parameters to FS
  if (shouldSaveConfig)
  {
      if (DEBUG) { Serial.println("saving config"); }
      DynamicJsonBuffer jsonBuffer;
      JsonObject& json = jsonBuffer.createObject();
      json["mqtt_server"] = mqtt_server;
      json["mqtt_port"] = mqtt_port;
      json["workgroup"] = workgroup;
      json["username"] = username;
      json["password"] = password;

      File configFile = SPIFFS.open("/config.json", "w");
      if (!configFile && DEBUG) { Serial.println("failed to open config file for writing"); }

      json.printTo(Serial);
      json.printTo(configFile);
      configFile.close();
  }
}

void saveSettings()
{
  if (DEBUG) { Serial.println("saving user settings"); }
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  //json["mqtt_server"] = mqtt_server;
  
  json["gBrightnessCur"] = _ledGlobalBrightnessCur;
  json["riseSpeed"] = _ledRiseSpeed;
  //json["gHueCycleTime"] = GHUE_CYCLE_TIME;
  //json["gHue"] = gHue;
  json["topColorHSV_h"] = _topColorHSV.h;
  json["topColorHSV_s"] = _topColorHSV.s;
  json["topColorHSV_v"] = _topColorHSV.v;
  json["botColorHSV_h"] = _botColorHSV.h;
  json["botColorHSV_s"] = _botColorHSV.s;
  json["botColorHSV_v"] = _botColorHSV.v;
  //_pirHoldInterval

  File settingsFile = SPIFFS.open("/settings.json", "w");
  if (!settingsFile && DEBUG) { Serial.println("failed to open user settings file for writing"); }

  json.printTo(Serial);
  json.printTo(settingsFile);
  settingsFile.close();
}

