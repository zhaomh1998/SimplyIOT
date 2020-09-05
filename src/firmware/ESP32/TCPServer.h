/*
  TCPServer.h - Library for TCP socket server operations. Tested on ESP8266.
  Created by Scott Zhao, August 1, 2018.
  Released into the public domain.
*/
#ifndef TCPServer_h
#define TCPServer_h

#include "Arduino.h"
//#include "ESP8266WiFi.h"  // For ESP8266
#include "WiFi.h"           // For ESP32
#include "WiFiClient.h"
//#include "ESP8266WebServer.h" // For ESP8266
#include "WebServer.h"          // For ESP32
#include "JsonHandler.h"

class TCPServer
{
  public:
    TCPServer(int portNumber);
    void start();
    int connectClient(); //returns the client id if connected successfully, else return -1
    int clientAvaileble(uint8_t clientID);
    char readChar(uint8_t clientID);
    void sendStr(uint8_t clientID, char dataStream[]);
    void echoTest();
    void readCMD(uint8_t clientID);
    void sendCfg(uint8_t clientID, Config &config);
  private:
    WiFiClient _client[50];
    WiFiServer _server;//(int portNumber);
    int _clientCount = 0;
};

#endif
