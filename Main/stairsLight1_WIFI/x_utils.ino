/*----------------------------utils----------------------------*/
void saveConfigCallback ()
{
  //callback notifying us of the need to save config
  shouldSaveConfig = true;
  if (DEBUG) { Serial.println("Should save config"); }
}

void calculateMachineId()
{
    MD5Builder md5;
    md5.begin();
    char chipId[25];
    sprintf(chipId,"%d",ESP.getChipId());
    md5.add(chipId);
    md5.calculate();
    md5.toString().toCharArray(machineId, 32);
}

void factoryReset()
{
/*    
  if (false == digitalRead(pinButton))
    {
        if (DEBUG) { Serial.println("Hold the button to reset to factory defaults..."); }
        for (int iter=0; iter<30; iter++)
        {
            //digitalWrite(pinAlarm, HIGH);
            //delay(100);
            //digitalWrite(pinAlarm, LOW);
            delay(100);
        }
        if (false == digitalRead(pinButton))
        {
            if (DEBUG) { Serial.println("Disconnecting..."); }
            WiFi.disconnect();

            // NOTE: the boot mode:(1,7) problem is known and only happens at the first restart after serial flashing.

            if (DEBUG) { Serial.println("Restarting..."); }
            // Clean the file system with configurations
            SPIFFS.format();
            // Restart the board
            ESP.restart();
        }
        else
        {
            // Cancel reset to factory defaults
            if (DEBUG) { Serial.println("Reset to factory defaults cancelled."); }
        }
    } 
*/
}

