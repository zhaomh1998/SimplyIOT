/*
  TCPServer.cpp - Library for TCP server operations. Tested on ESP8266.
  Created by Scott Zhao, August 1, 2018.
  Released into the public domain.
*/

#include "Arduino.h"
//#include "ESP8266WiFi.h"  // For ESP8266
#include "WiFi.h"           // For ESP32
#include "WiFiClient.h"
//#include "ESP8266WebServer.h" // For ESP8266
#include "WebServer.h"          // For ESP32
#include "TCPServer.h"
#include "JsonHandler.h"

TCPServer::TCPServer(int portNumber)
 :_server(portNumber)
{
//   WiFiServer _server(portNumber);
}
void TCPServer::start() {
  _server.begin();
  Serial.println("Server Initialized");
}
int TCPServer::connectClient() {
  _client[_clientCount] = _server.available();
  if(_client[_clientCount]) { // A client is available
    // clead out the input buffer:
    _client[_clientCount].flush();
    _clientCount++;
    return _clientCount - 1;
  }
  return -1;
}
int TCPServer::clientAvaileble(uint8_t clientID) {
  return _client[clientID].available();
}
char TCPServer::readChar(uint8_t clientID) {
  return _client[clientID].read();
}
void TCPServer::sendStr(uint8_t clientID, char charArray[]) {
  _client[clientID].println(charArray);
}

void TCPServer::echoTest() {
    for (int i = 0; i < _clientCount; i++) {
    if (_client[i].available() > 0) {
      // read the bytes incoming from the client:
      char thisChar = _client[i].read();
      Serial.print("#"); Serial.print(thisChar); Serial.println("#");
      // echo the bytes back to the client:
      _client[i].println(thisChar);
      // echo the bytes to the server as well:
      // Serial.write(thisChar);
    }
  }
}

void TCPServer::readCMD(uint8_t clientID) {
  if(_client[clientID].available() > 0) {
    char thisChar = _client[clientID].read();
    switch(thisChar) {
      case 'a':
        Serial.println("Command a received!");
        break;
      case 'b':
        Serial.println("Command b received!");
        break;
      default:
        Serial.println(thisChar);
      }
    }  
  }

void TCPServer::sendCfg(uint8_t clientID, Config &config) {
  _client[clientID].print("cM");
  _client[clientID].println(config.connMode);
  _client[clientID].print("cN");
  _client[clientID].println(config.connName);
  _client[clientID].print("cS");
  _client[clientID].println(config.connPass);
  _client[clientID].print("cP");
  _client[clientID].println(config.servPort);
}