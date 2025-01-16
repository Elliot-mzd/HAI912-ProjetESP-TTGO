// ========================= IMPORTS =========================
#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebSrv.h>
#include <TFT_eSPI.h>
#include <ArduinoUnit.h>
#include <ArduinoUnitMock.h>
#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <LittleFS.h>
#include <TimeLib.h>
#include <NTPClient.h>

// ========================= CONFIGURATION =========================
// Définition des pins
const int THERMISTOR_PIN = 36; // GPIO36, ADC1
const int PHOTORESISTOR_PIN = 39; // GPIO39, ADC1
const int REDLED_PIN = 13; // GPIO13, sortie numérique
const int GREENLED_PIN = 17; // GPIGPIO27, sortie numérique
const int BUZZER_PIN = 25; // GPIO25, sortie numérique

// Variables pour suivre l'état des modes
bool nightMode = false; 
bool thermometerMode = false; 

// Threshold de gestion de la température et de la lumière
float temperatureThreshold = 20.0;
float lightThreshold = 1.0;

// Structure pour stocker l'heure du réveil et la mélodie
struct AlarmTime {
    int hour = -1;  // Heure du réveil
    int minute = -1; // Minute du réveil
};

AlarmTime alarmTime;  // Variable pour stocker l'heure du réveil

int alarmMelody = -1; // Mélodie à jouer pour le réveil

// Instances du serveur
AsyncWebServer server(80);

// ========================= PROTOTYPES DE SETUP =========================
// Déclaration de la fonction pour initialiser la connexion Wi-Fi
void setupWiFi();

// Déclaration de la fonction pour initialiser l'écran TFT
void setupScreen();

// Déclaration de la fonction pour configurer les routes du serveur (API)
void setupServerRoutes();

// Déclaration de la fonction pour configurer le swagger (openAPI)
void setupSwagger();

// Déclaration de la fonction pour configurer le gestionnaire de fichier (LittleFS)
void setupLittleFS();

// Déclaration de la fonction pour configurer l'horloge de l'alarme
void setupNTP();


// ========================= PROTOTYPES DE HELPERS =========================
// Déclaration de la fonction pour afficher un message sur l'écran TFT
void displayMessage(const String& message);

// Déclaration de la fonction pour lire la température depuis un thermistor
float readTemperature();

// Déclaration de la fonction pour gérer le mode thermomètre
void handleThermometer();

// Déclaration de la fonction pour gérer le mode nuit
void handleNight();

// Déclaration de la fonction de vérification de l'alarme
void checkAlarmAndPlayMelody();


// ========================= PROTOTYPES DE TEST =========================
// Test de la LED rouge
void redLEDTest();

// Test de la LED vert
void greenLEDTest();

// Test de la température
void readTemperatureTest();

// Test de la luminosité
void lightSensorTest();

// Test du mode nuit
void nightModeTest();

// Test du mode thermomètre
void thermometerModeTest();


// ========================= PROTOTYPES DE ENDPOINTS =========================
// Déclaration de la fonction pour récupérer la valeur de la lumière ambiante
void handleGetLight(AsyncWebServerRequest* request);

// Déclaration de la fonction pour récupérer la température mesurée par le capteur
void handleGetTemperature(AsyncWebServerRequest* request);

// Déclaration de la fonction pour contrôler l'état de la LED rouge
void handleRedLED(AsyncWebServerRequest* request);

// Déclaration de la fonction pour contrôler l'état de la LED verte
void handleGreenLED(AsyncWebServerRequest* request);

// Déclaration de la fonction pour activer ou désactiver le mode nuit
void handleNightMode(AsyncWebServerRequest* request);

// Déclaration de la fonction pour activer ou désactiver le mode thermomètre
void handleThermometerMode(AsyncWebServerRequest* request);

// Déclaration de la fonction pour gérer la lecture d'une mélodie
void handlePlayMelody(AsyncWebServerRequest* request);

// Déclaration de la fonction pour configurer une alarme
void handleSetAlarm(AsyncWebServerRequest* request);

// Déclaration de la fonction pour connaitre l'état de la LED rouge
void handleRedLEDStatus(AsyncWebServerRequest* request);

// Déclaration de la fonction pour connaitre l'état de la LED verte
void handleGreenLEDStatus(AsyncWebServerRequest* request);

// Déclaration de la fonction pour gérer les erreurs 404 (page non trouvée)
void handleNotFound(AsyncWebServerRequest* request);


// ========================= ROUTE SETUP =============================
// Configuration des routes pour le serveur
void setupServerRoutes() {
    // Route pour récupérer la valeur de la lumière
    server.on("/get/light", HTTP_GET, handleGetLight);
    
    // Route pour récupérer la température
    server.on("/get/temperature", HTTP_GET, handleGetTemperature);

    // Route pour connaitre l'état de la LED rouge
    server.on("/led/red/status", HTTP_GET, handleRedLEDStatus);

    // Route pour connaitre l'état de la LED vert
    server.on("/led/green/status", HTTP_GET, handleGreenLEDStatus);
    
    // Route pour contrôler la LED rouge
    server.on("/led/red", HTTP_GET, handleRedLED);
    
    // Route pour contrôler la LED verte
    server.on("/led/green", HTTP_GET, handleGreenLED);
    
    // Route pour activer le mode nuit
    server.on("/control/nightMode", HTTP_POST, handleNightMode);
    
    // Route pour activer le mode thermomètre
    server.on("/control/thermometreMode", HTTP_POST, handleThermometerMode);
    
    // Route pour jouer une mélodie
    server.on("/play", HTTP_GET, handlePlayMelody);
    
    // Route pour mettre un réveil
    server.on("/setAlarm", HTTP_POST, handleSetAlarm);

    // Gestion des routes non trouvées (404)
    server.onNotFound(handleNotFound);
    
    // Démarre le serveur
    server.begin();
}


// ========================= ARDUINO MAIN FUNCTIONS =========================
// Fonction d'initialisation
void setup() {
    Serial.begin(115200);  // Initialise la communication série avec un débit de 115200 bauds
    while(!Serial){}
    delay(10000); // Attente pour assurer le fonctionnement correct du Serial

    pinMode(REDLED_PIN, OUTPUT);  // Définit la broche de la LED rouge comme sortie
    pinMode(GREENLED_PIN, OUTPUT);  // Définit la broche de la LED verte comme sortie
    digitalWrite(REDLED_PIN, LOW);  // Éteint la LED rouge
    digitalWrite(GREENLED_PIN, LOW);  // Éteint la LED verte
    pinMode(THERMISTOR_PIN, INPUT);  // Définit la broche du thermistor comme entrée
    pinMode(PHOTORESISTOR_PIN, INPUT);  // Définit la broche du photo-résistor comme entrée
    pinMode(BUZZER_PIN, OUTPUT);  // Définit la broche du buzzer comme sortie

    setupScreen();  // Initialise l'écran TFT
    setupWiFi();  // Initialise la connexion Wi-Fi
    setupServerRoutes();  // Configure les routes du serveur pour l'API
    setupOTA();// Initialisation d'OTA
    setupSwagger(); // Initialisation du swagger 
    setupLittleFS(); // Initialisation de système de fichier
    setupNTP(); // Initialisation de l'horloge
}

// Fonction de boucle principale
void loop() {
  Test::run(); // Exécute les tests

  if (nightMode) {
      handleNight();  // Si le mode nuit est activé, gère le mode nuit
  }
  if (thermometerMode) {
      handleThermometer();  // Si le mode thermomètre est activé, gère le mode thermomètre
  }

  checkAlarmAndPlayMelody();
  ArduinoOTA.handle();
  delay(1000);
}