/*----------------------------utils----------------------------*/
void factoryReset()
{ 
}

/*    
  if (false == digitalRead(pinButton))
    {
        if (DEBUG_GEN) { Serial.println("Hold the button to reset to factory defaults..."); }
        for (int iter=0; iter<30; iter++)
        {
            //digitalWrite(pinAlarm, HIGH);
            //delay(100);
            //digitalWrite(pinAlarm, LOW);
            delay(100);
        }
        if (false == digitalRead(pinButton))
        {
            if (DEBUG_GEN) { Serial.println("Disconnecting..."); }
            WiFi.disconnect();

            // NOTE: the boot mode:(1,7) problem is known and only happens at the first restart after serial flashing.

            if (DEBUG_GEN) { Serial.println("Restarting..."); }
            // Clean the file system with configurations
            SPIFFS.format();
            // Restart the board
            ESP.restart();
        }
        else
        {
            // Cancel reset to factory defaults
            if (DEBUG_GEN) { Serial.println("Reset to factory defaults cancelled."); }
        }
    } 
}
*/

/*
 * Alternative map function that uses float (instead of int)
 */
float mapf(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
