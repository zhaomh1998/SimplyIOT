/*
  JsonHandler.h - Library for handling configurations stored in file system using Arduino Json Library. Tested on ESP32.
  Created by Scott Zhao, August 17, 2018.
  Released into the public domain.
*/
#include "Arduino.h"
#include "FS.h"
#include "SPIFFS.h"
#include "ArduinoJson.h"
#include "JsonHandler.h"

JsonHandler::JsonHandler() {
  // TODO: Check file exist, if not create default
  if(!SPIFFS.exists(CONF_FILE)) {
    createDefaultConf();
  }
}

String JsonHandler::get(String keyName) {
  File file = SPIFFS.open(CONF_FILE);
  StaticJsonBuffer<512> jsonBuffer;
  JsonObject &confList = jsonBuffer.parseObject(file);
  file.close();
  if (!confList.success()) {
    Serial.println(F("Failed to read file"));
    return "-1";
  }
  return confList[keyName];
}

bool JsonHandler::set(String keyName, String keyValue) {
  // Get current configurations into buffer
  File file = SPIFFS.open(CONF_FILE);
  StaticJsonBuffer<512> jsonBuffer;
  JsonObject &tempRead = jsonBuffer.parseObject(file);
  file.close();

  if (!tempRead.success()) {
    Serial.println("File reading failed");
    Serial.println("Trying to create default configuration");
    if(createDefaultConf())
      return set(keyName, keyValue);
    else {
      Serial.println("Default Configuration Creation Failed");
      return "-1";
    }
  }
  // Update local configuration
  tempRead[keyName] = keyValue;

  // Write local conf to file
  SPIFFS.remove(CONF_FILE);
  file = SPIFFS.open(CONF_FILE, FILE_WRITE);
  if (!file) {
    Serial.println(F("Failed to create file"));
    return false;
  }
  // StaticJsonBuffer<512> jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();
  root["connMode"] = tempRead["connMode"];
  root["connName"] = tempRead["connName"];
  root["connPass"] = tempRead["connPass"];
  root["verInfo"] = tempRead["verInfo"];
  root["servPort"] = tempRead["servPort"];
  if (root.printTo(file) == 0) {
    Serial.println(F("Failed to write to file"));
  }
  file.close();
  return true;
}

bool JsonHandler::createDefaultConf() {
  File file = SPIFFS.open(CONF_FILE, FILE_WRITE);
  if (!file) {
    Serial.println(F("Failed to create file"));
    return false;
  }
  StaticJsonBuffer<512> jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();
  root["connMode"] = DEFAULT_CONN_MODE;
  root["connName"] = DEFAULT_CONN_NAME;
  root["connPass"] = DEFAULT_CONN_PASS;
  root["verInfo"] = VERSION_INFO;
  root["servPort"] = DEFAULT_SERV_PORT;
  if (root.printTo(file) == 0) {
    Serial.println(F("Failed to write to file"));
  }
  file.close();
  return true;
}

void JsonHandler::printFile() {
  // Open file for reading
  File file = SPIFFS.open(CONF_FILE);
  if (!file) {
    Serial.println(F("Failed to read file"));
    return;
  }

  // Extract each characters by one by one
  while (file.available()) {
    Serial.print((char)file.read());
  }
  Serial.println();

  // Close the file (File's destructor doesn't close the file)
  file.close();
}

void JsonHandler::loadConfiguration(Config &config) {
  File file = SPIFFS.open(CONF_FILE);
  StaticJsonBuffer<512> jsonBuffer;

  // Parse the root object
  JsonObject &root = jsonBuffer.parseObject(file);
  file.close();
  if (!root.success()) {
    Serial.println("File reading failed");
    Serial.println("Trying to create default configuration");
    if(createDefaultConf()) {
      Serial.println("Default configuration file created");
    }
    else {
      Serial.println("Default Configuration File Creation Failed. Will return default configuration");
    }
  }
  // Copy values from the JsonObject to the Config
  strlcpy(config.connMode, root["connMode"] | DEFAULT_CONN_MODE, sizeof(config.connMode));
  strlcpy(config.connName, root["connName"] | DEFAULT_CONN_NAME, sizeof(config.connName));
  strlcpy(config.connPass, root["connPass"] | DEFAULT_CONN_PASS, sizeof(config.connPass));
  config.servPort = root["servPort"] | DEFAULT_SERV_PORT;
}