// ========================= CONFIGURATION =========================
// Constantes pour le calcul de la température
const float BETA = 3950;
const float R_REF = 10000;
const float TEMP_0 = 298.15;  // 25°C en Kelvin
const float R_INF = R_REF * exp(-BETA / TEMP_0);


// Instances de l'écran TFT 
TFT_eSPI tft = TFT_eSPI();

// ========================= HELPER FUNCTIONS =========================
// Fonction pour afficher un message sur l'écran TFT
void displayMessage(const String& message) {
    tft.fillScreen(TFT_BLACK);  // Remplie l'écran avec un fond noir
    tft.setTextColor(TFT_WHITE);  // Définit la couleur du texte en blanc
    tft.setTextSize(2);  // Définit la taille du texte
    tft.setCursor(10, 20);  // Définit la position du curseur pour le texte
    tft.println("IP : " + WiFi.localIP().toString());  // Affiche l'adresse IP locale de l'ESP32
    tft.println(message);  // Affiche le message passé en argument
}

// Fonction pour calculer la température à partir de la lecture du thermistor
float readTemperature() {
    int analogValue = analogRead(THERMISTOR_PIN);  // Lit la valeur analogique du thermistor
    float voltage = analogValue * (3.3 / 4095.0);  // Convertit la valeur analogique en tension
    float resistance = (3.3 / voltage - 1.0) * R_REF;  // Calcule la résistance du thermistor
    float temperatureKelvin = BETA / log(resistance / R_INF);  // Utilise l'équation de Steinhart-Hart pour obtenir la température en Kelvin
    return temperatureKelvin - 273.15;  // Convertit la température de Kelvin à Celsius
}

// Fonction pour gérer le mode nuit
void handleNight() {
    int analogValue = analogRead(PHOTORESISTOR_PIN);  // Lit la valeur du capteur de lumière
    float voltage = analogValue * (3.3 / 4095.0);  // Convertit la valeur en tension
    if (voltage < lightThreshold) {
        digitalWrite(GREENLED_PIN, HIGH); // Allume la LED verte si la lumière est inférieure au seuil
        displayMessage("Nuit : Sombre, LED verte ON"); 
    } else {
        digitalWrite(GREENLED_PIN, LOW); // Éteint la LED verte sinon
        displayMessage("Nuit : Lumineux, LED verte OFF"); 
    }
}

// Fonction pour gérer le mode thermomètre
void handleThermometer() {
    float temperature = readTemperature();  // Lit la température
    if (temperature > temperatureThreshold) {
        digitalWrite(REDLED_PIN, HIGH); // Allume la LED rouge si la température est supérieure à 20°C
        //displayMessage("Température : Haute, LED rouge ON"); 
    } else {
        digitalWrite(REDLED_PIN, LOW); // Éteint la LED rouge si la température est inférieure ou égale à 20°C
        //displayMessage("Température : Normale, LED rouge OFF"); 
    }
}

// Fonction pour vérifier l'heure et jouer la mélodie si nécessaire
void checkAlarmAndPlayMelody() {
    // Vérifier l'heure actuelle
    int currentHour = hour();   // Utilisation de la bibliothèque Time pour obtenir l'heure actuelle
    int currentMinute = minute(); // Utilisation de la bibliothèque Time pour obtenir les minutes actuelles

    // Si l'heure actuelle correspond à l'heure du réveil, jouer la mélodie
    if (currentHour == alarmTime.hour && currentMinute == alarmTime.minute && alarmMelody != -1) {
        Serial.println("Playng melody...");
        selectAndPlayMelody(alarmMelody); // Jouer la mélodie du réveil
        delay(60000); // Attendre une minute pour ne pas rejouer la mélodie
    }
}