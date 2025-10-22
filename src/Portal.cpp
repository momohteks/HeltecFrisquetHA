#include "Portal.h"
#include <Preferences.h>
#include "Logs.h"

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
  html += "<a class='btn' href='/logs'>Voir les logs</a>";
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

// Page HTML (auto-refresh, filtrage léger, téléchargement)
static void handleLogsPage() {
  String html = F(
    "<!DOCTYPE html><html><head><meta charset='utf-8'>"
    "<meta name='viewport' content='width=device-width,initial-scale=1'>"
    "<title>Frisquet - Logs</title>"
    "<style>"
      "body{font-family:ui-monospace,Menlo,Consolas,monospace;margin:12px;}"
      "header{display:flex;gap:8px;align-items:center;flex-wrap:wrap;margin-bottom:8px}"
      "input,select,button{padding:6px 10px}"
      "pre{white-space:pre-wrap;background:#0b0b0b;color:#e6e6e6;padding:12px;border-radius:8px;max-height:70vh;overflow:auto}"
      ".muted{opacity:.7}"
    "</style></head><body>"
    "<header>"
      "<a href='/'>&larr; Retour</a>"
      "<strong>Logs</strong>"
      "<label class='muted'>Refresh "
        "<select id='refresh'>"
          "<option value='0'>off</option>"
          "<option value='1000'>1s</option>"
          "<option value='2000' selected>2s</option>"
          "<option value='5000'>5s</option>"
          "<option value='10000'>10s</option>"
        "</select>"
      "</label>"
      "<label class='muted'>Filtre <input id='filter' placeholder='texte...' /></label>"
      "<button id='btnReload'>Recharger</button>"
      "<button id='btnClear'>Effacer</button>"
      "<a id='btnDownload' href='/logs.txt' download='logs.txt'>Télécharger</a>"
    "</header>"
    "<pre id='log'>(chargement...)</pre>"
    "<script>"
    "let t=null;"
    "const logEl=document.getElementById('log');"
    "const sel=document.getElementById('refresh');"
    "const filter=document.getElementById('filter');"
    "const reload=()=>{fetch('/logs.txt?ts='+Date.now(),{cache:'no-store'})"
      ".then(r=>r.ok?r.text():Promise.reject(r.status))"
      ".then(txt=>{"
        "const f=filter.value.trim().toLowerCase();"
        "if(f){"
          "txt=txt.split('\\n').filter(l=>l.toLowerCase().includes(f)).join('\\n');"
        "}"
        "logEl.textContent=txt||'(vide)';"
        "logEl.scrollTop=logEl.scrollHeight;"
      "}).catch(e=>{logEl.textContent='Erreur chargement logs: '+e;});};"
    "document.getElementById('btnReload').onclick=reload;"
    "document.getElementById('btnClear').onclick=()=>{"
      "fetch('/logs/clear',{method:'POST'}).then(()=>reload());"
    "};"
    "sel.onchange=()=>{ if(t){clearInterval(t);t=null;} const v=+sel.value; if(v>0){t=setInterval(reload,v);} };"
    "reload(); t=setInterval(reload,2000);"
    "</script>"
    "</body></html>"
  );
  server.send(200, "text/html", html);
}

// Texte brut (pour téléchargement / intégrations)
static void handleLogsRaw() {
  String all = logs.getAllLogs();
  // Empêche la mise en cache côté navigateur
  server.sendHeader("Cache-Control", "no-store, max-age=0");
  server.send(200, "text/plain", all);
}

// Effacer les logs
static void handleLogsClear() {
  if (server.method() != HTTP_POST) {
    server.send(405, "text/plain", "Method Not Allowed");
    return;
  }
  logs.clear();
  server.send(200, "text/plain", "OK");
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
  WiFi.setHostname("ESPFrisquet Connect");
  WiFi.setAutoReconnect(true);
  if (strlen(cfg.wifiSsid) > 0) {
    WiFi.begin(cfg.wifiSsid, cfg.wifiPass);
  }
  startAP();

  server.on("/", handleRoot);
  server.on("/save", handleSave);
  
  // --- routes logs ---
  server.on("/logs",     HTTP_GET,  handleLogsPage);
  server.on("/logs.txt", HTTP_GET,  handleLogsRaw);
  server.on("/logs/clear", HTTP_POST, handleLogsClear);

  server.begin();
}

void portalLoop() {
  server.handleClient();
}

bool portalIsApRunning()   { return apRunning; }
bool portalIsStaConnected(){ return staGotIP; }
