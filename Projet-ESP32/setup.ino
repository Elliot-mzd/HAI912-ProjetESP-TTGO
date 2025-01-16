// ========================= CONFIGURATION =========================
// Identifiants Wi-Fi
const char* ssid = "Livebox-DFE0";
const char* password = "Cz5VJskzcSnqAmp2yQ";

// Initialisation de serveur pour l'horloge
WiFiUDP udp;
NTPClient timeClient(udp, "pool.ntp.org", 0, 3600);  // Serveur NTP

// ========================= SETUP FUNCTIONS =========================
// Initialiser la connexion Wi-Fi
void setupWiFi() {
    WiFi.mode(WIFI_STA);
    
    while (true) {  // Boucle continue jusqu'à connexion réussie
        Serial.println("Attempting WiFi connection...");
        displayMessage("Connecting...");
        
        WiFi.begin(ssid, password);
        
        if (WiFi.waitForConnectResult() == WL_CONNECTED) {
            // Connexion réussie
            Serial.println("WiFi connected!");
            Serial.println("IP Address: " + WiFi.localIP().toString());
            displayMessage("");
            break;  // Sort de la boucle si connecté
        } else {
            // Échec de connexion
            Serial.println("WiFi connection failed!");
            displayMessage("WiFi Failed! Retry in 1min");
            WiFi.disconnect();  // Déconnecte pour nettoyer l'état
            delay(60000);      // Attend 1 minute (60000 ms)
        }
    }
}

// Initialiser l'écran TFT
void setupScreen() {
    tft.init(); // Initialiser l'écran TFT
    tft.setRotation(1); // Définir l'orientation de l'écran
    tft.fillScreen(TFT_BLACK); // Effacer l'écran avec la couleur noire
}

// Fonction de configuration OTA
void setupOTA() {
  // Callback lors du démarrage de la mise à jour OTA
  ArduinoOTA.onStart([]() {
    ArduinoOTA.setPassword("1234");
    String type = ArduinoOTA.getCommand() == U_FLASH ? "flash" : "filesystem";
    Serial.println("Début de la mise à jour OTA: " + type);
  });

  // Callback lors de la fin de la mise à jour OTA
  ArduinoOTA.onEnd([]() {
    displayMessage("Mise à jour OTA terminée !");
    //ESP.restart();  // Redémarre l'ESP32 pour appliquer les changements

  });

  // Callback pour afficher la progression de la mise à jour
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progression: %u%%\r", (progress / (total / 100)));
  });

  // Callback pour les erreurs OTA
  ArduinoOTA.onError([](ota_error_t error) {
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Erreur d'authentification OTA");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Erreur de début OTA");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Erreur de connexion OTA");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Erreur de réception OTA");
    } else if (error == OTA_END_ERROR) {
      Serial.println("Erreur de fin OTA");
    }
  });

  // Démarrage de l'OTA
  ArduinoOTA.begin();
}

// Sert le fichier "index.html" sur la route par default "/"
void setupSwagger(){
   // Servir Swagger UI (HTML, CSS, JS)
  server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");
}

// Initialise le système de fichiers
void setupLittleFS(){
  // Initialisation de LittleFS
  if (!LittleFS.begin()) {
    Serial.println("Erreur LittleFS");
    return;
  }
}

// Fonction pour initialiser le NTP et mettre à jour l'heure
void setupNTP() {
  // Connexion au Wi-Fi
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }

  // Initialisation du client NTP
  timeClient.begin();

  // Attendre que l'heure soit mise à jour
  while (!timeClient.update()) {
    timeClient.forceUpdate(); // Force la mise à jour si elle échoue
    delay(1000);
  }

  // Ajuster le fuseau horaire (UTC + 1 pour Paris par exemple)
  timeClient.setTimeOffset(3600); // Décalage d'une heure en secondes (1 heure = 3600 secondes)

  // Récupérer et définir l'heure NTP
  setTime(timeClient.getEpochTime()); // Ajuste l'heure avec l'heure NTP
  Serial.println("NTP Time Updated");
}