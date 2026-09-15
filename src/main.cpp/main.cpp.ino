#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>

Preferences preferences;
WebServer server(80);

// Valeurs par défaut si aucune configuration n'est enregistrée
String ap_ssid     = "Smart_Gate_V2";
String ap_password = "gate12345678";

#define DATA_PIN 13            
#define RELAY_PIN 23           
#define MAX_EDGES 100
#define MIN_GAP_US 2500       
#define NOISE_FLOOR_US 300    
#define BIT_THRESHOLD_US 980  

const String TARGET_BINARY_CODE = "100110011001100110011010";

volatile unsigned long edgeTimes[MAX_EDGES];
volatile int edgeCount = 0;
volatile unsigned long lastEdgeMicros = 0;
volatile bool frameReady = false;

void IRAM_ATTR onDataChange() {
  unsigned long now = micros();
  unsigned long gap = now - lastEdgeMicros;
  lastEdgeMicros = now;

  if (gap > MIN_GAP_US) {
    if (edgeCount >= 24 && !frameReady) {
      frameReady = true;
    } else if (!frameReady) {
      edgeCount = 0;
    }
  }

  if (!frameReady && edgeCount < MAX_EDGES) {
    edgeTimes[edgeCount++] = now;
  }
}

void triggerGateRelay() {
  digitalWrite(RELAY_PIN, HIGH);
  delay(1000);
  digitalWrite(RELAY_PIN, LOW);
}

// Interface Web Modern UI avec icône Paramètres & Modal CSS/JS
void handleRoot() {
  String html = "<!DOCTYPE html><html lang='en'>";
  html += "<head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<title>House Garbaa Gate</title>";
  html += "<style>";
  html += "* { box-sizing: border-box; margin: 0; padding: 0; font-family: 'Segoe UI', Roboto, sans-serif; }";
  html += "body { background: #0d1117; color: #c9d1d9; min-height: 100vh; display: flex; flex-direction: column; justify-content: space-between; align-items: center; padding: 20px; text-align: center; position: relative; }";
  
  // Icone Paramètres
  html += ".settings-btn { position: absolute; top: 20px; right: 20px; background: none; border: none; color: #8b949e; cursor: pointer; transition: color 0.3s; }";
  html += ".settings-btn:hover { color: #58a6ff; }";
  
  html += "header { margin-top: 40px; }";
  html += "h1 { font-size: 20px; font-weight: 700; letter-spacing: 2px; color: #2ea44f; text-transform: uppercase; }";
  html += ".subtitle { font-size: 12px; color: #8b949e; margin-top: 6px; text-transform: uppercase; letter-spacing: 1px; }";
  
  html += ".main-container { display: flex; justify-content: center; align-items: center; width: 100%; margin: auto 0; }";
  html += ".btn-gate { width: 170px; height: 170px; border-radius: 50%; border: none; background: linear-gradient(145deg, #34d058, #28a745); color: #ffffff; font-size: 20px; font-weight: bold; cursor: pointer; box-shadow: 0 0 25px rgba(46,164,79,0.4); transition: transform 0.2s; outline: none; }";
  html += ".btn-gate:active { transform: scale(0.92); background: #22863a; }";
  
  html += "footer { margin-bottom: 10px; font-size: 12px; color: #8b949e; }";
  html += "footer span { color: #58a6ff; font-weight: 600; }";

  // Fenêtre Modal pour Paramètres Wi-Fi
  html += ".modal { display: none; position: fixed; top: 0; left: 0; width: 100%; height: 100%; background: rgba(0,0,0,0.8); justify-content: center; align-items: center; z-index: 100; }";
  html += ".modal-content { background: #161b22; border: 1px solid #30363d; padding: 25px; border-radius: 12px; width: 90%; max-width: 320px; text-align: left; }";
  html += ".modal-content h2 { font-size: 16px; color: #c9d1d9; margin-bottom: 15px; text-align: center; }";
  html += ".modal-content label { font-size: 12px; color: #8b949e; display: block; margin-top: 10px; }";
  html += ".modal-content input { width: 100%; padding: 10px; margin-top: 5px; background: #0d1117; border: 1px solid #30363d; border-radius: 6px; color: white; font-size: 14px; outline: none; }";
  html += ".modal-actions { display: flex; justify-content: space-between; margin-top: 20px; }";
  html += ".btn-save { background: #2ea44f; color: white; border: none; padding: 8px 16px; border-radius: 6px; cursor: pointer; font-weight: 600; }";
  html += ".btn-cancel { background: #30363d; color: #c9d1d9; border: none; padding: 8px 16px; border-radius: 6px; cursor: pointer; }";

  html += "</style></head><body>";
  
  // Bouton engrenage SVG
  html += "<button class='settings-btn' onclick='openModal()'>";
  html += "<svg width='28' height='28' viewBox='0 0 24 24' fill='none' stroke='currentColor' stroke-width='2' stroke-linecap='round' stroke-linejoin='round'><circle cx='12' cy='12' r='3'></circle><path d='M19.4 15a1.65 1.65 0 0 0 .33 1.82l.06.06a2 2 0 0 1 0 2.83 2 2 0 0 1-2.83 0l-.06-.06a1.65 1.65 0 0 0-1.82-.33 1.65 1.65 0 0 0-1 1.51V21a2 2 0 0 1-2 2 2 2 0 0 1-2-2v-.09A1.65 1.65 0 0 0 9 19.4a1.65 1.65 0 0 0-1.82.33l-.06.06a2 2 0 0 1-2.83 0 2 2 0 0 1 0-2.83l.06-.06a1.65 1.65 0 0 0 .33-1.82 1.65 1.65 0 0 0-1.51-1H3a2 2 0 0 1-2-2 2 2 0 0 1 2-2h.09A1.65 1.65 0 0 0 4.6 9a1.65 1.65 0 0 0-.33-1.82l-.06-.06a2 2 0 0 1 0-2.83 2 2 0 0 1 2.83 0l.06.06a1.65 1.65 0 0 0 1.82.33H9a1.65 1.65 0 0 0 1-1.51V3a2 2 0 0 1 2-2 2 2 0 0 1 2 2v.09a1.65 1.65 0 0 0 1 1.51 1.65 1.65 0 0 0 1.82-.33l.06-.06a2 2 0 0 1 2.83 0 2 2 0 0 1 0 2.83l-.06.06a1.65 1.65 0 0 0-.33 1.82V9a1.65 1.65 0 0 0 1.51 1H21a2 2 0 0 1 2 2 2 2 0 0 1-2 2h-.09a1.65 1.65 0 0 0-1.51 1z'></path></svg>";
  html += "</button>";
  
  html += "<header>";
  html += "<h1>Welcome to House Garbaa</h1>";
  html += "<div class='subtitle'>Automatic Gate Control</div>";
  html += "</header>";
  
  html += "<div class='main-container'>";
  html += "<button class='btn-gate' onclick=\"fetch('/open')\">OPEN</button>";
  html += "</div>";
  
  html += "<footer>";
  html += "Realized by <span>Bouzid Aluminium & Automatisation</span>";
  html += "</footer>";

  // HTML du Modal
  html += "<div id='settingsModal' class='modal'>";
  html += "<div class='modal-content'>";
  html += "<h2>Wi-Fi Settings</h2>";
  html += "<form action='/save_wifi' method='POST'>";
  html += "<label>SSID (Wi-Fi Name):</label>";
  html += "<input type='text' name='ssid' value='" + ap_ssid + "' required>";
  html += "<label>Password (Min 8 chars):</label>";
  html += "<input type='password' name='pass' value='" + ap_password + "' required minlength='8'>";
  html += "<div class='modal-actions'>";
  html += "<button type='button' class='btn-cancel' onclick='closeModal()'>Cancel</button>";
  html += "<button type='submit' class='btn-save'>Save & Reboot</button>";
  html += "</div></form></div></div>";

  // Javascript pour ouvrir/fermer la modale
  html += "<script>";
  html += "function openModal(){ document.getElementById('settingsModal').style.display = 'flex'; }";
  html += "function closeModal(){ document.getElementById('settingsModal').style.display = 'none'; }";
  html += "</script>";
  
  html += "</body></html>";
  
  server.send(200, "text/html", html);
}

void handleOpenGate() {
  Serial.println("\n>>> [MOBILE COMMAND] Gate Opened via Direct Wi-Fi! <<<");
  triggerGateRelay();
  server.send(200, "text/plain", "OK");
}

// Sauvegarde des nouveaux identifiants Wi-Fi
void handleSaveWifi() {
  if (server.hasArg("ssid") && server.hasArg("pass")) {
    String new_ssid = server.arg("ssid");
    String new_pass = server.arg("pass");

    if (new_pass.length() >= 8) {
      preferences.begin("wifi_config", false);
      preferences.putString("ssid", new_ssid);
      preferences.putString("pass", new_pass);
      preferences.end();

      String html = "<html><body style='background:#0d1117;color:white;text-align:center;padding-top:50px;font-family:sans-serif;'>";
      html += "<h2>Settings Saved!</h2><p>Rebooting ESP32... Connect to the new Wi-Fi in 10 seconds.</p></body></html>";
      server.send(200, "text/html", html);

      delay(2000);
      ESP.restart(); // Redémarrage de l'ESP32 pour appliquer le nouveau Wi-Fi
    } else {
      server.send(400, "text/plain", "Password must be at least 8 characters long.");
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(500);

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  pinMode(DATA_PIN, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(DATA_PIN), onDataChange, CHANGE);

  // Charger le SSID et le mot de passe depuis la mémoire NVS (Flash)
  preferences.begin("wifi_config", true);
  ap_ssid = preferences.getString("ssid", ap_ssid);
  ap_password = preferences.getString("pass", ap_password);
  preferences.end();

  // Démarrage du Point d'Accès Wi-Fi
  WiFi.softAP(ap_ssid.c_str(), ap_password.c_str());
  Serial.print("Access Point SSID: ");
  Serial.println(ap_ssid);
  Serial.print("AP IP Address: ");
  Serial.println(WiFi.softAPIP());

  // Configuration des routes Web
  server.on("/", handleRoot);
  server.on("/open", handleOpenGate);
  server.on("/save_wifi", HTTP_POST, handleSaveWifi);
  server.begin();
  Serial.println("Offline Web Server Started.");
}

void loop() {
  server.handleClient();

  if (frameReady) {
    decodeRFSignal();
    edgeCount = 0;
    frameReady = false;
  }
}

void decodeRFSignal() {
  int n = edgeCount;
  String binaryCode = "";

  for (int i = 1; i < n; i++) {
    unsigned long duration = edgeTimes[i] - edgeTimes[i - 1];
    if (duration < NOISE_FLOOR_US) continue;

    if (duration > BIT_THRESHOLD_US && duration < MIN_GAP_US) {
      binaryCode += "1";
    } else if (duration <= BIT_THRESHOLD_US) {
      binaryCode += "0";
    }
  }

  if (binaryCode.length() == 24 && binaryCode == TARGET_BINARY_CODE) {
    Serial.println("\n>>> [RF COMMAND] Keypad Signal Received! <<<");
    triggerGateRelay();
    delay(2000); 
  }
}