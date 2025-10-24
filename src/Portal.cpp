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
  String html = F(
    "<!DOCTYPE html><html lang='fr'><head>"
    "<meta charset='utf-8'>"
    "<meta name='viewport' content='width=device-width,initial-scale=1'>"
    "<title>Frisquet – Configuration</title>"
    "<style>"
      ":root{--bg:#0f1115;--card:#171a21;--muted:#8a8f98;--txt:#e7e9ee;--acc:#3aa3ff;--bd:#2a2f39;--ok:#1fb86a;--warn:#ffb020}"
      "*,*:before,*:after{box-sizing:border-box}"
      "body{margin:0;padding:24px;background:var(--bg);color:var(--txt);font:15px/1.45 system-ui,Segoe UI,Roboto,Arial}"
      "h1,h2{margin:0 0 12px}"
      "a{color:var(--acc);text-decoration:none}"
      ".wrap{max-width:980px;margin:0 auto;display:grid;gap:16px}"
      ".grid{display:grid;grid-template-columns:1fr 1fr;gap:16px}"
      ".card{background:var(--card);border:1px solid var(--bd);border-radius:12px;padding:18px;box-shadow:0 4px 16px rgba(0,0,0,.2)}"
      ".row{display:flex;flex-direction:column;gap:6px}"
      "label{font-weight:600}"
      ".hint{color:var(--muted);font-size:12px}"
      "input[type=text],input[type=password],input[type=number]{width:100%;padding:10px 12px;border:1px solid var(--bd);border-radius:10px;background:#0d1016;color:var(--txt)}"
      ".pw{position:relative}"
      ".pw button{position:absolute;right:8px;top:50%;transform:translateY(-50%);border:1px solid var(--bd);background:#0d1016;color:var(--muted);padding:4px 8px;border-radius:8px;cursor:pointer}"
      ".actions{display:flex;gap:10px;flex-wrap:wrap;margin-top:8px}"
      ".btn{display:inline-flex;align-items:center;gap:8px;border:1px solid var(--bd);background:#0d1016;color:var(--txt);padding:10px 14px;border-radius:10px;cursor:pointer}"
      ".btn.primary{background:var(--acc);color:#061019;border-color:transparent;font-weight:700}"
      ".badge{display:inline-flex;align-items:center;gap:6px;padding:6px 10px;border-radius:999px;border:1px solid var(--bd);background:#10131a;font-size:13px}"
      ".ok{color:var(--ok)} .warn{color:var(--warn)}"
      ".split{display:grid;grid-template-columns:1.1fr .9fr;gap:16px}"
      ".footer{color:var(--muted);font-size:12px;text-align:center;margin-top:8px}"
      "@media (max-width:820px){.grid,.split{grid-template-columns:1fr}}"
    "</style>"
    "</head><body>"
    "<div class='wrap'>"

      "<div class='split'>"
        "<div class='card'>"
          "<h2>Frisquet – Configuration</h2>"
          "<p class='hint'>Renseignez le Wi-Fi et le broker MQTT puis cliquez sur <strong>Enregistrer</strong>.</p>"
          "<form method='POST' action='/save' autocomplete='off'>"

            "<div class='grid'>"
              "<div class='row'>"
                "<label>SSID Wi-Fi</label>"
                "<input name='wifiSsid' type='text' placeholder='MaBox' value='"
  );
  html += String(cfg.wifiSsid);
  html += F(
    "'>"
                "<div class='hint'>Nom du réseau (2.4 GHz recommandé).</div>"
              "</div>"

              "<div class='row pw'>"
                "<label>Mot de passe Wi-Fi</label>"
                "<input id='wifipass' name='wifiPass' type='password' placeholder='••••••••' value='"
  );
  html += String(cfg.wifiPass);
  html += F(
    "'>"
                "<button type='button' data-toggle='#wifipass'>Afficher</button>"
              "</div>"
            "</div>"

            "<div class='grid' style='margin-top:8px'>"
              "<div class='row'>"
                "<label>MQTT – Hôte</label>"
                "<input name='mqttHost' type='text' placeholder='192.168.1.10' value='"
  );
  html += String(cfg.mqttHost);
  html += F(
    "'>"
              "</div>"

              "<div class='row'>"
                "<label>MQTT – Port</label>"
                "<input name='mqttPort' type='number' min='1' max='65535' placeholder='1883' value='"
  );
  html += String(cfg.mqttPort);
  html += F(
    "'>"
              "</div>"

              "<div class='row'>"
                "<label>MQTT – Utilisateur</label>"
                "<input name='mqttUser' type='text' placeholder='mqtt-user' value='"
  );
  html += String(cfg.mqttUser);
  html += F(
    "'>"
              "</div>"

              "<div class='row pw'>"
                "<label>MQTT – Mot de passe</label>"
                "<input id='mqttpass' name='mqttPass' type='password' placeholder='••••••••' value='"
  );
  html += String(cfg.mqttPass);
  html += F(
    "'>"
                "<button type='button' data-toggle='#mqttpass'>Afficher</button>"
              "</div>"
            "</div>"

            "<div class='actions'>"
              "<button class='btn primary' type='submit'>Enregistrer</button>"
              "<a class='btn' href='/logs'>Voir les logs</a>"
            "</div>"
          "</form>"
        "</div>"

        "<div class='card'>"
          "<h2>Statut</h2>"
          "<div class='row'>"
            "<span class='badge'>Mode AP&nbsp;: "
  );

  // AP status badge
  html += apRunning ? "<span class='ok'>actif</span>" : "<span class='warn'>inactif</span>";
  html += F("</span>"
            "<span class='badge'>Station Wi-Fi&nbsp;: ");
  html += staGotIP ? "<span class='ok'>connectée</span>" : "<span class='warn'>déconnectée</span>";
  html += F("</span></div>"
            "<div class='row' style='margin-top:8px'>"
              "<div class='hint'>"
              "Si la station se déconnecte, un point d’accès <em>Frisquet-Setup</em> est lancé automatiquement."
              "</div>"
            "</div>"
        "</div>"
      "</div>"

      "<div class='footer'>© ");
  html += WiFi.getHostname() ? WiFi.getHostname() : "ESP Device";
  html += F(" · Portail de configuration</div>"
    "</div>"

    "<script>"
      "document.querySelectorAll('[data-toggle]').forEach(btn=>{"
        "btn.addEventListener('click',()=>{"
          "const sel=btn.getAttribute('data-toggle');"
          "const inp=document.querySelector(sel);"
          "if(!inp) return;"
          "inp.type = (inp.type==='password')?'text':'password';"
          "btn.textContent = (inp.type==='password')?'Afficher':'Masquer';"
        "});"
      "});"
    "</script>"

    "</body></html>"
  );

  server.sendHeader("Cache-Control", "no-store, max-age=0");
  server.send(200, "text/html", html);
}


static void handleSave() {
  if (server.method() != HTTP_POST) {
    server.send(405, "text/plain", "Method Not Allowed");
    return;
  }

  // 1) Récupération + persistance
  strncpy(cfg.wifiSsid, server.arg("wifiSsid").c_str(), sizeof(cfg.wifiSsid));
  strncpy(cfg.wifiPass, server.arg("wifiPass").c_str(), sizeof(cfg.wifiPass));
  strncpy(cfg.mqttHost, server.arg("mqttHost").c_str(), sizeof(cfg.mqttHost));
  cfg.mqttPort = server.arg("mqttPort").toInt();
  strncpy(cfg.mqttUser, server.arg("mqttUser").c_str(), sizeof(cfg.mqttUser));
  strncpy(cfg.mqttPass, server.arg("mqttPass").c_str(), sizeof(cfg.mqttPass));

  prefs.begin("config", false);
  prefs.putBytes("cfg", &cfg, sizeof(cfg));
  prefs.end();

  // 2) Page de feedback + auto-redirect quand l’ESP revient
  String html = F(
    "<!DOCTYPE html><html lang='fr'><head>"
    "<meta charset='utf-8'>"
    "<meta name='viewport' content='width=device-width,initial-scale=1'>"
    "<title>Frisquet – Redémarrage</title>"
    "<style>"
      ":root{--bg:#0f1115;--card:#171a21;--muted:#8a8f98;--txt:#e7e9ee;--acc:#3aa3ff;--bd:#2a2f39}"
      "*,*:before,*:after{box-sizing:border-box}"
      "body{margin:0;padding:24px;background:var(--bg);color:var(--txt);font:15px/1.45 system-ui,Segoe UI,Roboto,Arial}"
      ".wrap{max-width:720px;margin:0 auto}"
      ".card{background:var(--card);border:1px solid var(--bd);border-radius:12px;padding:22px;box-shadow:0 4px 16px rgba(0,0,0,.2)}"
      "h1{margin:0 0 6px}"
      ".muted{color:var(--muted)}"
      ".row{display:flex;gap:10px;align-items:center;flex-wrap:wrap}"
      ".spinner{width:22px;height:22px;border-radius:50%;border:3px solid #2a2f39;border-top-color:var(--acc);animation:sp 1s linear infinite}"
      "@keyframes sp{to{transform:rotate(360deg)}}"
      ".hint{margin-top:10px;color:var(--muted)}"
      ".small{font-size:12px}"
      "code{background:#0b0e13;border:1px solid var(--bd);padding:2px 6px;border-radius:6px}"
      ".btn{display:inline-flex;align-items:center;gap:8px;border:1px solid var(--bd);background:#0d1016;color:var(--txt);padding:8px 12px;border-radius:10px;text-decoration:none;margin-top:8px}"
    "</style>"
    "</head><body>"
    "<div class='wrap'>"
      "<div class='card'>"
        "<div class='row'><div class='spinner'></div><h1>Redémarrage en cours…</h1></div>"
        "<p class='muted'>La configuration a été enregistrée. L’appareil redémarre pour appliquer les changements.</p>"
        "<p class='muted'>Tentative de reconnexion automatique dans <strong id='count'>8</strong> s…</p>"
        "<div class='hint small'>"
          "Si la page ne revient pas, actualisez manuellement ou revenez à <a href='/' class='btn'>l'accueil</a>."
        "</div>"
      "</div>"
    "</div>"
    "<script>"
      "let secs=8, t=null;"
      "const count=document.getElementById('count');"
      "function tick(){ secs=Math.max(0,secs-1); count.textContent=secs; if(secs===0){ tryReconnect(); } }"
      "function tryReconnect(){"
        "fetch('/',{cache:'no-store'})"
          ".then(r=>{ if(r.ok){ location.replace('/'); } else { retry(); } })"
          ".catch(()=>retry());"
      "}"
      "function retry(){"
        "setTimeout(tryReconnect, 1500);"
      "}"
      "t=setInterval(tick,1000);"
      "setTimeout(tryReconnect, secs*1000);"
    "</script>"
    "</body></html>"
  );

  server.sendHeader("Cache-Control", "no-store, max-age=0");
  server.sendHeader("Connection", "close");
  server.send(200, "text/html", html);

  // 3) Laisse le temps au client de recevoir la page, puis reboot
  delay(400);      // flush TCP
  delay(600);      // petite marge visuelle
  ESP.restart();
}

static void handleLogsPage() {
  String html = F(
    "<!DOCTYPE html><html lang='fr'><head>"
    "<meta charset='utf-8'>"
    "<meta name='viewport' content='width=device-width,initial-scale=1'>"
    "<title>Frisquet – Logs</title>"
    "<style>"
      ":root{--bg:#0f1115;--card:#171a21;--muted:#8a8f98;--txt:#e7e9ee;--acc:#3aa3ff;--bd:#2a2f39;--ok:#1fb86a;--warn:#ffb020}"
      "*,*:before,*:after{box-sizing:border-box}"
      "body{margin:0;padding:24px;background:var(--bg);color:var(--txt);font:15px/1.45 system-ui,Segoe UI,Roboto,Arial}"
      "a{color:var(--acc);text-decoration:none}"
      "h1,h2{margin:0 0 12px}"
      ".wrap{max-width:980px;margin:0 auto;display:grid;gap:16px}"
      ".card{background:var(--card);border:1px solid var(--bd);border-radius:12px;padding:18px;box-shadow:0 4px 16px rgba(0,0,0,.2)}"
      ".toolbar{display:flex;flex-wrap:wrap;gap:10px;align-items:center;margin:8px 0 12px}"
      ".badge{display:inline-flex;align-items:center;gap:6px;padding:6px 10px;border-radius:999px;border:1px solid var(--bd);background:#10131a;font-size:13px}"
      ".btn{display:inline-flex;align-items:center;gap:8px;border:1px solid var(--bd);background:#0d1016;color:var(--txt);padding:10px 14px;border-radius:10px;cursor:pointer}"
      ".btn.primary{background:var(--acc);color:#061019;border-color:transparent;font-weight:700}"
      "label{color:var(--muted);font-weight:600}"
      "select,input[type=text],input[type=checkbox]{padding:8px 10px;border:1px solid var(--bd);border-radius:10px;background:#0d1016;color:var(--txt)}"
      "pre{white-space:pre-wrap;background:#0b0e13;color:#e6e6e6;padding:12px;border-radius:10px;max-height:70vh;overflow:auto;margin:0;font-family:ui-monospace,Menlo,Consolas,monospace}"
      ".muted{color:var(--muted)}"
      ".row{display:flex;gap:10px;flex-wrap:wrap;align-items:center}"
      ".topnav{display:flex;align-items:center;gap:10px;margin-bottom:4px}"
      "@media (max-width:820px){.toolbar{gap:8px}}"
    "</style>"
    "</head><body>"
    "<div class='wrap'>"

      "<div class='topnav'>"
        "<a href='/' class='btn'>&larr;&nbsp;Retour</a>"
        "<span class='badge'>Frisquet – Logs</span>"
        "<a class='btn' href='/logs.txt' download='logs.txt'>Télécharger</a>"
      "</div>"

      "<div class='card'>"
        "<div class='toolbar'>"
          "<label>Rafraîchissement&nbsp;"
            "<select id='refresh'>"
              "<option value='0'>Off</option>"
              "<option value='1000'>1s</option>"
              "<option value='2000' selected>2s</option>"
              "<option value='5000'>5s</option>"
              "<option value='10000'>10s</option>"
            "</select>"
          "</label>"

          "<label>Niveau&nbsp;"
            "<select id='level'>"
              "<option value=''>Tous</option>"
              "<option value='INFO'>INFO</option>"
              "<option value='DEBUG'>DEBUG</option>"
              "<option value='WARN'>WARN</option>"
              "<option value='ERROR'>ERROR</option>"
            "</select>"
          "</label>"

          "<label>Filtre texte&nbsp;<input id='filter' type='text' placeholder='rechercher...'></label>"

          "<label>Lignes&nbsp;"
            "<select id='limit'>"
              "<option value='0' selected>Toutes</option>"
              "<option value='200'>200</option>"
              "<option value='500'>500</option>"
              "<option value='1000'>1000</option>"
            "</select>"
          "</label>"

          "<label class='row'><input id='autoscroll' type='checkbox' checked>&nbsp;Auto-scroll</label>"

          "<button id='btnReload' class='btn'>Recharger</button>"
          "<button id='btnClear' class='btn'>Effacer</button>"
          "<a id='btnDownload' class='btn' href='/logs.txt' download='logs.txt'>Télécharger</a>"
        "</div>"

        "<pre id='log'>(chargement...)</pre>"
      "</div>"

      "<div class='muted' style='text-align:center'>"
        "Astuce&nbsp;: utilisez le filtre de niveau (p. ex. ERROR) et la limite pour ne garder que la fin du journal."
      "</div>"

    "</div>"

    "<script>"
      "let t=null, raw='';"
      "const elLog=document.getElementById('log');"
      "const selRef=document.getElementById('refresh');"
      "const selLvl=document.getElementById('level');"
      "const selLimit=document.getElementById('limit');"
      "const inpFilter=document.getElementById('filter');"
      "const cbAuto=document.getElementById('autoscroll');"

      "function applyFilters(txt){"
        "if(!txt) return '';"
        "let lines = txt.split('\\n');"
        "const lvl = selLvl.value.trim();"
        "const f = inpFilter.value.trim().toLowerCase();"

        "if(lvl){"
          "lines = lines.filter(l => l.includes(lvl));"
        "}"
        "if(f){"
          "lines = lines.filter(l => l.toLowerCase().includes(f));"
        "}"

        "const lim = parseInt(selLimit.value||'0',10);"
        "if(lim>0 && lines.length>lim){"
          "lines = lines.slice(-lim);"
        "}"
        "return lines.join('\\n');"
      "}"

      "function render(){"
        "const out = applyFilters(raw);"
        "elLog.textContent = out || '(vide)';"
        "if(cbAuto.checked){"
          "elLog.scrollTop = elLog.scrollHeight;"
        "}"
      "}"

      "function reload(){"
        "fetch('/logs.txt?ts='+Date.now(),{cache:'no-store'})"
          ".then(r=>r.ok?r.text():Promise.reject(r.status))"
          ".then(txt=>{ raw = txt; render(); })"
          ".catch(e=>{ elLog.textContent = 'Erreur chargement logs: '+e; });"
      "}"

      "document.getElementById('btnReload').onclick = reload;"
      "document.getElementById('btnClear').onclick = ()=>{"
        "fetch('/logs/clear',{method:'POST'}).then(()=>reload());"
      "};"

      "[selLvl, inpFilter, selLimit].forEach(el=>{ el.addEventListener('input', render); });"

      "selRef.onchange = ()=>{ if(t){clearInterval(t); t=null;} const v=+selRef.value; if(v>0){ t=setInterval(reload,v); } };"

      "reload(); t=setInterval(reload,2000);"
    "</script>"

    "</body></html>"
  );

  server.sendHeader("Cache-Control", "no-store, max-age=0");
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
