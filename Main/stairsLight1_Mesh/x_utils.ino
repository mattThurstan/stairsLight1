/*----------------------------utils----------------------------*/

void factoryReset() { /* TODO */ }

void publishDeviceOffline() {
  publishMeshMsgSingleState("publishState", "status", OFF, false);
  publishMeshMsgSingleState("publishState", "available", "offline", false);
}

void deviceRestart() {
  ESP.restart();
}

void deviceShutdown() { /* ??? */ }

void turnOffWifi() {
  if (DEBUG_GEN) { Serial.println("Disconnecting..."); }
  WiFi.disconnect();
}

void turnOffMesh() {
  if (DEBUG_GEN) { Serial.println("Disconnecting mesh..."); }
  mesh.stop();
}

void turnOffSerial() {
  if (DEBUG_GEN) { Serial.println("Disconnecting serial..."); }
  //
}


/*----------------------------main calls-----------------------*/
/* Reset everything to default. */
void doReset() {
  //resetDefaults();
  //deviceRestart(); // ..and restart
}

/* Restart the device (with a delay) */
void doRestart(uint8_t restartTime) {
  // delay for restartTime
  deviceRestart(); // ..and restart
}

/*
 * Lockdown.
 * 
 * Emergency global disconnect (requires hard reset). 
 *  0 = do nothing (no severity)
 *  1 = disconnect from LAN (bridges shutdown and mesh reboots) 
 *  2 = shutdown mesh (everything reboots in standalone mode) 
 *  3 = shutdown everything. 
 *      - Devices to power off if possible, if not then reboot in standalone emergency mode. 
 *      - These devices will require a hardware reset button implemented to clear the emergency mode.)
 */
void doLockdown(uint8_t severity) {
  if (severity < 4) { 
    LOCKDOWN_SEVERITY = severity;
    if (severity > 0) { LOCKDOWN = true; }
  }
  
  if (severity == 0) { /* do nothing */ } 
  else if (severity == 1) {
    turnOffComms();
    // set to restart with no comms
    doRestart(0);
  } else if (severity == 2) {
    turnOffComms();
    // set to restart in standalone mode
    doRestart(0);
  } else if (severity == 3) {
    turnOffComms();
    //set emergency mode
    //set lockdown severity (includes restart)
    deviceShutdown(); // if possible
    // if not then set to restart in standalone emergency mode
    //deviceRestart(); // ..and restart 
  } 
  else { }
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
