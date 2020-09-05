/*
  JsonHandler.h - Library for handling configurations stored in file system using Arduino Json Library. Tested on ESP32.
  Created by Scott Zhao, August 17, 2018.
  Released into the public domain.
*/
#ifndef JsonHandler_h
#define JsonHandler_h

#define VERSION_INFO "Beta 1.0 Aug 17 2018 10:12pm"
#define CONF_FILE "/config.txt"
#define DEFAULT_CONN_MODE "AP"
#define DEFAULT_CONN_NAME "ESPTest"
#define DEFAULT_CONN_PASS "0"
#define DEFAULT_SERV_PORT 23

#include "Arduino.h"
#include "FS.h"
#include "SPIFFS.h"
#include "ArduinoJson.h"

struct Config{
  char connMode[64];
  char connName[64];
  char connPass[64];
  short servPort;
};

class JsonHandler
{
  public:
    JsonHandler();
    String get(String keyName);
    bool set(String keyName, String keyValue);
    void printFile();
    void loadConfiguration(Config &config);
    bool createDefaultConf();
//   private:
};

#endif