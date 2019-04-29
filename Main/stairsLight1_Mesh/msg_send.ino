/*----------------------------send messages----------------------------*/
void publishState(bool save) {
  
  if (DEBUG_COMMS) { Serial.print("publishState "); }
  String msg = "lights/light/status";
  msg += ":"; //..just so we are all sure what is going on here !?
  if(_state == 0) {
    msg += LIGHTS_OFF;
  } else {
    msg += LIGHTS_ON;
  }
  mesh.sendSingle(id, msg);
  if (DEBUG_COMMS) { Serial.println(msg); }
  if (save == true) { _shouldSaveSettings = true; }
}

void publishBrightness(bool save) {
  
  if (DEBUG_COMMS) { Serial.print("publishBrightness "); }
  String msg = "lights/brightness/status";
  msg += ":"; //..just so we are all sure what is going on here !?
  msg += String(_ledGlobalBrightnessCur);
  mesh.sendSingle(id, msg);
  if (DEBUG_COMMS) { Serial.println(msg); }
  if (save == true) { _shouldSaveSettings = true; }
}

void publishRGB(bool save) {
  
  RgbColor tempRGB = _colorHSL;
  if (DEBUG_COMMS) { Serial.print("publishRGB "); }
  String msg = "lights/rgb/status";
  msg += ":"; //..just so we are all sure what is going on here !?
  msg += String(tempRGB.R);
  msg += ",";
  msg += String(tempRGB.G);
  msg += ",";
  msg += String(tempRGB.B);
  mesh.sendSingle(id, msg);
  if (DEBUG_COMMS) { Serial.println(msg); } 
  if (save == true) { _shouldSaveSettings = true; }
}

void publishRiseSpeed(bool save) {
  
  if (DEBUG_COMMS) { Serial.print("publishRiseSpeed "); }
  String msg = "lights/risespeed/status";
  msg += ":"; //..just so we are all sure what is going on here !?
  msg += String(_ledRiseSpeedSaved);
  mesh.sendSingle(id, msg);
  if (DEBUG_COMMS) { Serial.println(msg); }
  if (save == true) { _shouldSaveSettings = true; }
}

void publishGHue2Cycle(bool save) {
  
  if (DEBUG_COMMS) { Serial.print("publishGHue2Cycle "); }
  String msg = "lights/hue/cycle/status";
  msg += ":"; //..just so we are all sure what is going on here !?
  msg += String(_gHue2CycleSaved);
  mesh.sendSingle(id, msg);
  if (DEBUG_COMMS) { Serial.println(msg); }
  if (save == true) { _shouldSaveSettings = true; }
}

void publishSensorTop(bool save) {
  
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
  if (save == true) { _shouldSaveSettings = true; }
}

void publishSensorBot(bool save) {
  
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
  if (save == true) { _shouldSaveSettings = true; }
}

void publishMode(bool save) {
  
  if (DEBUG_COMMS) { Serial.print("publishMode "); }
  String msg = "lights/mode";
  msg += ":"; //..just so we are all sure what is going on here !?
  msg += _modeName[_modeCur];
  mesh.sendSingle(id, msg);
  if (DEBUG_COMMS) { Serial.println(msg); }
  if (save == true) { _shouldSaveSettings = true; }
}

void publishDebugGeneralState(bool save)  {
  
  if (DEBUG_COMMS) { Serial.print("publishDebugGeneralState "); }
  String msg = "debug/general/status";
  msg += ":"; //..just so we are all sure what is going on here !?
  if (DEBUG_GEN == false) {
    msg += "OFF";
  } else if (DEBUG_GEN == true) {
    msg += "ON";
  }
  mesh.sendSingle(id, msg);
  if (DEBUG_COMMS) { Serial.println(msg); }
  if (save == true) { _shouldSaveSettings = true; }
}

void publishDebugOverlayState(bool save) {
  
  if (DEBUG_COMMS) { Serial.print("publishDebugOverlayState "); }
  String msg = "debug/overlay/status";
  msg += ":"; //..just so we are all sure what is going on here !?
  if (DEBUG_OVERLAY == false) {
    msg += "OFF";
  } else if (DEBUG_OVERLAY == true) {
    msg += "ON";
  }
  mesh.sendSingle(id, msg);
  if (DEBUG_COMMS) { Serial.println(msg); }
  if (save == true) { _shouldSaveSettings = true; }
}

void publishDebugMeshsyncState(bool save) {
  
  if (DEBUG_COMMS) { Serial.print("publishDebugMeshsyncState "); }
  String msg = "debug/meshsync/status";
  msg += ":"; //..just so we are all sure what is going on here !?
  if (DEBUG_MESHSYNC == false) {
    msg += "OFF";
  } else if (DEBUG_MESHSYNC == true) {
    msg += "ON";
  }
  mesh.sendSingle(id, msg);
  if (DEBUG_COMMS) { Serial.println(msg); }
  if (save == true) { _shouldSaveSettings = true; }
}

void publishDebugCommsState(bool save) {
  
  if (DEBUG_COMMS) { Serial.print("publishDebugCommsState "); }
  String msg = "debug/comms/status";
  msg += ":"; //..just so we are all sure what is going on here !?
  if (DEBUG_COMMS == false) {
    msg += "OFF";
  } else if (DEBUG_COMMS == true) {
    msg += "ON";
  }
  mesh.sendSingle(id, msg);
  if (DEBUG_COMMS) { Serial.println(msg); }
  if (save == true) { _shouldSaveSettings = true; }
}
