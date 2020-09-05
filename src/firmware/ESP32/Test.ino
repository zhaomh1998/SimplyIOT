// TODO: cfg add current time record

#include "TCPServer.h"
#include "JsonHandler.h"

/* Set these to your desired credentials. */
// const char *ssid = "ESPTest";
// TCPServer espServer(23);
JsonHandler jsHandler;
TCPServer* espServer;
Config cfg;
void setup()
{
  // Hardware Init
  Serial.begin(115200);
  while (!SPIFFS.begin()) {
    Serial.println(F("Failed to initialize SPIFFS library"));
    delay(1000);
  }

  SPIFFS.remove(CONF_FILE);
  jsHandler.createDefaultConf();
  // Software Init
  jsHandler.loadConfiguration(cfg);
  if(isConf(cfg.connMode, "AP")) {
    isConf(cfg.connPass, "0")? WiFi.softAP(cfg.connName) : WiFi.softAP(cfg.connName, cfg.connPass);
    espServer = new TCPServer(cfg.servPort);
    espServer -> start();
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);
  }
  else if(isConf(cfg.connMode, "WiFi")) {
    Serial.println("WiFi");
  }
  else if(isConf(cfg.connMode, "BLE")) {
    Serial.println("BLE");
  }
  else {
    Serial.println("Cfg Corrupted. Resetting Config.");
    SPIFFS.remove(CONF_FILE);
    jsHandler.createDefaultConf();
    Serial.println("Reset done, reboot in 2s");
    delay(2000);
    ESP.restart();
  }
  // WiFi.softAP(ssid);
  // //delay(10000);
  // IPAddress myIP = WiFi.softAPIP();
  // Serial.print("AP IP address: ");
  // Serial.println(myIP);

  // Serial.println("\nStarting server...");
  // // start the server:
  // espServer.start();
}




void loop()
{
  int clientID = espServer->connectClient();
  if (clientID >= 0) {
    Serial.println("Client Connected");
    espServer -> sendStr(clientID, "helloFromESPMainProgram");
    espServer -> sendCfg(clientID, cfg);
    while (1) {
      espServer->readCMD(clientID);
    }
  }
   espServer->echoTest();
}

bool isConf(char* confVal, char* compareWith) {
  if(strcmp(confVal, compareWith) == 0)
    return true;
  else
    return false;
}