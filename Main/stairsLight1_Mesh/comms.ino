/*----------------------------comms----------------------------*/
void setupMesh() 
{
  mesh.setDebugMsgTypes( ERROR | STARTUP );       // set before init() so that you can see startup messages

  mesh.init(MESH_NAME, MESH_PASSWORD, MESH_PORT, WIFI_AP_STA, STATION_CHANNEL, MESH_NODE_HIDDEN, MESH_NODE_MAX_CONN); // hidden, 4 max connections
  
  mesh.setContainsRoot(true);
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
}

void changeChannel(uint8_t channel) {
  _stationChannel = channel;
  saveSettings();
  deviceRestart();
}

void publishDeviceOffline() {
  publishMeshMsgSingleState("publishState", "status", OFF, false);
  publishMeshMsgSingleState("publishState", "available", "offline", false);
}

void turnOffComms() 
{
    publishDeviceOffline();
    turnOffMesh();
    turnOffWifi();
    turnOffSerial();
}
