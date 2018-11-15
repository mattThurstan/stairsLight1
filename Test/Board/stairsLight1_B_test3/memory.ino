/*----------------------------memory----------------------------*/
void loadConfig()
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
    }
    else
    {
        if (DEBUG) { Serial.println("failed to mount FS"); }
    }
   
}

