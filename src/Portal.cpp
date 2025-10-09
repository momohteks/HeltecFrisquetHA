#include "Portal.h"
#include <Preferences.h>

static WebServer server(80);
static Preferences prefs;

static bool apRunning = false;
static bool staGotIP  = false;

static void startAP(const char* ssid="Frisquet-Setup", const char* pass="frisquetconfig") {
  if (apRunning) return;
  WiFi.softAP(ssid, pass);
  Serial.printf("[AP] ON SSID=%s, IP=%s\n", ssid, WiFi.softAPIP().toString().c_str());
  apRunning = true;
}
static void stopAP() {
  if (!apRunning) return;
  WiFi.softAPdisconnect(true);
  Serial.println("[AP] OFF");
  apRunning = false;
}

static void onWiFiEvent(WiFiEvent_t event) {
  switch(event) {
    case SYSTEM_EVENT_STA_GOT_IP:
      Serial.printf("[WiFi] GOT_IP: %s\n", WiFi.localIP().toString().c_str());
      staGotIP = true;
      stopAP();
      break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
      Serial.println("[WiFi] STA_DISCONNECTED");
      staGotIP = false;
      startAP();
      break;
    default: break;
  }
}

static void handleRoot() {
  String html = "<h2>Frisquet - Config</h2>";
  html += "<form method='POST' action='/save'>";
  html += "SSID:<input name='wifiSsid' value='"+String(cfg.wifiSsid)+"'><br>";
  html += "Pass:<input name='wifiPass' type='password' value='"+String(cfg.wifiPass)+"'><br>";
  html += "MQTT host:<input name='mqttHost' value='"+String(cfg.mqttHost)+"'><br>";
  html += "MQTT port:<input name='mqttPort' value='"+String(cfg.mqttPort)+"'><br>";
  html += "MQTT user:<input name='mqttUser' value='"+String(cfg.mqttUser)+"'><br>";
  html += "MQTT pass:<input name='mqttPass' type='password' value='"+String(cfg.mqttPass)+"'><br>";
  html += "<button type='submit'>Save</button></form>";
  server.send(200, "text/html", html);
}

static void handleSave() {
  if (server.method() == HTTP_POST) {
    strncpy(cfg.wifiSsid, server.arg("wifiSsid").c_str(), sizeof(cfg.wifiSsid));
    strncpy(cfg.wifiPass, server.arg("wifiPass").c_str(), sizeof(cfg.wifiPass));
    strncpy(cfg.mqttHost, server.arg("mqttHost").c_str(), sizeof(cfg.mqttHost));
    cfg.mqttPort = server.arg("mqttPort").toInt();
    strncpy(cfg.mqttUser, server.arg("mqttUser").c_str(), sizeof(cfg.mqttUser));
    strncpy(cfg.mqttPass, server.arg("mqttPass").c_str(), sizeof(cfg.mqttPass));

    prefs.begin("config", false);
    prefs.putBytes("cfg", &cfg, sizeof(cfg));
    prefs.end();

    server.send(200, "text/plain", "Config saved. Reboot required.");
    delay(1000);
    ESP.restart();
  }
}

void portalInit() {
  prefs.begin("config", true);
  if (prefs.isKey("cfg")) {
    prefs.getBytes("cfg", &cfg, sizeof(cfg));
  } else {
    strcpy(cfg.wifiSsid, "");
    strcpy(cfg.wifiPass, "");
    strcpy(cfg.mqttHost, "192.168.1.10");
    cfg.mqttPort = 1883;
    strcpy(cfg.mqttUser, "");
    strcpy(cfg.mqttPass, "");
  }
  prefs.end();

  WiFi.mode(WIFI_AP_STA);
  WiFi.onEvent(onWiFiEvent);
  WiFi.setAutoReconnect(true);
  if (strlen(cfg.wifiSsid) > 0) {
    WiFi.begin(cfg.wifiSsid, cfg.wifiPass);
  }
  startAP();

  server.on("/", handleRoot);
  server.on("/save", handleSave);
  server.begin();
}

void portalLoop() {
  server.handleClient();
}

bool portalIsApRunning()   { return apRunning; }
bool portalIsStaConnected(){ return staGotIP; }
