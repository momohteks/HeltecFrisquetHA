#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

// Structure pour stocker la config (WiFi, MQTT…)
struct Config {
  char wifiSsid[32];
  char wifiPass[64];
  char mqttHost[64];
  uint16_t mqttPort;
  char mqttUser[32];
  char mqttPass[64];
};

extern Config cfg;

void portalInit();
void portalLoop();
bool portalIsApRunning();
bool portalIsStaConnected();
