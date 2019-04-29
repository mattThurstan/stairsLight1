/*----------------------------memory----------------------------*/
void loadSettings()
{
    //read configuration from FS json
    if (DEBUG_GEN) { Serial.println("mounting FS..."); }

    if (SPIFFS.begin())
    {
        if (DEBUG_GEN) { Serial.println("mounted file system"); }

        if (SPIFFS.exists("/settings.json")) {
            //file exists, reading and loading
            if (DEBUG_GEN) { Serial.println("reading user settings file"); }
            File configFile = SPIFFS.open("/settings.json", "r");
            if (configFile)
            {
                if (DEBUG_GEN) { Serial.println("opened user settings file"); }
                const size_t size = configFile.size();
                // Allocate a buffer to store contents of the file.
                std::unique_ptr<char[]> buf(new char[size]);

                configFile.readBytes(buf.get(), size);
                DynamicJsonBuffer jsonBuffer;
                JsonObject& json = jsonBuffer.parseObject(buf.get());
                if (DEBUG_GEN) { json.printTo(Serial); }
                if (json.success())
                {
                    if (DEBUG_GEN) { Serial.println("\nparsed json"); }

                    //_pirHoldInterval
                    _ledGlobalBrightnessCur = json["gBrightnessCur"];
                    _ledRiseSpeedSaved = json["ledRiseSpeedSaved"];
                    checkAndSetLedRiseSpeed();
                    _gHue2CycleSaved = json["gHue2CycleSaved"];
                    checkAndSetGHue2CycleMillis();
                    _colorHSL.H = json["colorHSL_H"];
                    _colorHSL.S = json["colorHSL_S"];
                    _colorHSL.L = json["colorHSL_L"];
                    
                }
                else
                {
                    if (DEBUG_GEN) { Serial.println("failed to load json user settings"); }
                }
            }
        }
    }
    else
    {
        if (DEBUG_GEN) { Serial.println("failed to mount FS"); }
    }
   
}

void saveSettings()
{
  if (DEBUG_GEN) { Serial.println("saving user settings"); }
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  
  //_pirHoldInterval
  json["gBrightnessCur"] = _ledGlobalBrightnessCur;
  json["ledRiseSpeedSaved"] = _ledRiseSpeedSaved;
  json["gHue2CycleSaved"] = _gHue2CycleSaved;
  json["colorHSL_H"] = _colorHSL.H;
  json["colorHSL_S"] = _colorHSL.S;
  json["colorHSL_L"] = _colorHSL.L;

  File settingsFile = SPIFFS.open("/settings.json", "w");
  if (!settingsFile && DEBUG_GEN) { Serial.println("failed to open user settings file for writing"); }
  if (DEBUG_GEN) { 
    json.printTo(Serial);
    json.printTo(settingsFile);
  }
  settingsFile.close();
}
