#pragma once

// Webervice : include
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include "RaceConfig.h"

class WebService
{
private:
  WebService();
  static WebService *_instance;

public:
  static WebService &Instance();
  void Init();
  void modifyGeneral( AsyncWebServerRequest *request);
  void modifyPlayer( AsyncWebServerRequest *request);
  void modifyObstacle( AsyncWebServerRequest *request);

private:
  AsyncWebServer _server;

  // Variable to store the HTTP request
  String _header;
  IPAddress _IP;

  // Webervice Wifi credentials
  static const char *_ssid;
  static const char *_password;

  static const String _pin_info_html;
  static String _index_html;
  static String _players_html;
  static String _obstacles_html;
    
  void notFound(AsyncWebServerRequest *request);

  void buildPlayersHTML();
  void buildObstaclesHTML();
  void buildIndexHTML();
  void buildBoardData();
};
