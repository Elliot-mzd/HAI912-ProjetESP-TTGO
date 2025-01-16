// ========================= PROTOTYPES =========================
// Déclaration de la fonction pour récuperer le nom de la mélodie
String getMelodyName(int melodyNumber);

// ========================= SERVER ROUTE HANDLERS ===================
// Gérer la route pour obtenir les données du capteur de lumière ("/get/light")
void handleGetLight(AsyncWebServerRequest* request) {
    int analogValue = analogRead(PHOTORESISTOR_PIN); // Lecture de la valeur analogique du capteur de lumière
    float voltage = analogValue * (3.3 / 4095.0); // Conversion de la valeur en tension
    String lightValue = String(voltage, 2) + " V"; // Formatage de la tension avec 2 décimales

    // Création d'un objet JSON pour la réponse
    DynamicJsonDocument jsonDoc(1024);
    jsonDoc["light"] = voltage; // Ajouter la valeur de la lumière en volts

    String jsonResponse;
    serializeJson(jsonDoc, jsonResponse); // Sérialiser le JSON en chaîne de caractères

    request->send(200, "application/json", jsonResponse); // Envoyer la réponse JSON
    displayMessage("Route: /get/light\nLight: " + lightValue); // Affichage de la valeur sur l'écran OLED
}

// Gérer la route pour obtenir les données de température ("/get/temperature")
void handleGetTemperature(AsyncWebServerRequest* request) {
    float temperature = readTemperature(); // Lecture de la température depuis le capteur
    String temperatureStr = String(temperature, 2) + " °C"; // Formatage de la température avec 2 décimales

    // Création d'un objet JSON pour la réponse
    DynamicJsonDocument jsonDoc(1024);
    jsonDoc["temperature"] = temperature; // Ajouter la température en degrés Celsius

    String jsonResponse;
    serializeJson(jsonDoc, jsonResponse); // Sérialiser le JSON en chaîne de caractères

    request->send(200, "application/json", jsonResponse); // Envoyer la réponse JSON
    displayMessage("Route: /get/temperature\nTemp: " + temperatureStr); // Affichage de la température sur l'écran OLED
}

// Gérer la route pour contrôler la LED rouge ("/led/red")
void handleRedLED(AsyncWebServerRequest* request) {
    if (request->hasParam("state")) { // Vérifier si le paramètre 'state' est fourni
        String state = request->getParam("state")->value(); // Récupérer la valeur du paramètre 'state'

        // Création d'un objet JSON pour la réponse
        DynamicJsonDocument jsonDoc(1024);

        if (state == "true") {
            digitalWrite(REDLED_PIN, HIGH); // Allumer la LED rouge
            jsonDoc["status"] = "success";
            jsonDoc["message"] = "Red LED turned on"; // Ajouter un message dans la réponse JSON
            displayMessage("Red LED ON"); // Afficher l'état sur l'écran
        } else if (state == "false") {
            digitalWrite(REDLED_PIN, LOW); // Éteindre la LED rouge
            jsonDoc["status"] = "success";
            jsonDoc["message"] = "Red LED turned off"; // Ajouter un message dans la réponse JSON
            displayMessage("Red LED OFF"); // Afficher l'état sur l'écran
        } else {
            jsonDoc["status"] = "error";
            jsonDoc["message"] = "Invalid 'state' parameter"; // Ajouter un message d'erreur
        }

        String jsonResponse;
        serializeJson(jsonDoc, jsonResponse); // Sérialiser le JSON en chaîne de caractères

        request->send(200, "application/json", jsonResponse); // Envoyer la réponse JSON
    } else {
        // Réponse si le paramètre 'state' est manquant
        DynamicJsonDocument jsonDoc(1024);
        jsonDoc["status"] = "error";
        jsonDoc["message"] = "Missing 'state' parameter"; // Ajouter un message d'erreur

        String jsonResponse;
        serializeJson(jsonDoc, jsonResponse); // Sérialiser le JSON en chaîne de caractères

        request->send(400, "application/json", jsonResponse); // Envoyer une réponse avec le code 400
    }
}

// Gérer la route pour contrôler la LED verte ("/control/green")
void handleGreenLED(AsyncWebServerRequest* request) {
    if (request->hasParam("state")) { // Vérifier si le paramètre 'state' est fourni
        String state = request->getParam("state")->value(); // Récupérer la valeur du paramètre 'state'

        // Vérifier si la valeur de 'state' est valide
        if (state == "true") {
            digitalWrite(GREENLED_PIN, HIGH); // Allumer la LED verte
            request->send(200, "application/json", "{\"status\":\"success\", \"message\":\"Green LED turned on\"}"); // Envoyer une réponse JSON
            displayMessage("Green LED ON"); // Afficher un message sur l'écran OLED
        } else if (state == "false") {
            digitalWrite(GREENLED_PIN, LOW); // Éteindre la LED verte
            request->send(200, "application/json", "{\"status\":\"success\", \"message\":\"Green LED turned off\"}"); // Envoyer une réponse JSON
            displayMessage("Green LED OFF"); // Afficher un message sur l'écran OLED
        } else {
            // Si la valeur de 'state' est invalide
            request->send(400, "application/json", "{\"status\":\"error\", \"message\":\"Invalid 'state' parameter\"}"); // Envoyer une réponse d'erreur JSON
        }
    } else {
        // Si le paramètre 'state' est manquant
        request->send(400, "application/json", "{\"status\":\"error\", \"message\":\"Missing 'state' parameter\"}"); // Envoyer une réponse d'erreur JSON
    }
}

// Gérer la route pour activer/désactiver le mode nuit ("/nightMode")
void handleNightMode(AsyncWebServerRequest* request) {
    if (request->hasParam("state")) { // Vérifier si le paramètre 'state' est fourni
        String state = request->getParam("state")->value(); // Récupérer la valeur du paramètre 'state'

        // Valeur par défaut du seuil de luminosité
        float threshold = lightThreshold; // Initialiser avec la valeur actuelle du seuil de luminosité

        // Vérifier si le paramètre 'lightThreshold' est fourni et l'utiliser si nécessaire
        if (request->hasParam("seuil")) {
            String thresholdStr = request->getParam("seuil")->value(); // Récupérer la valeur du paramètre 'threshold'
            threshold = thresholdStr.toFloat(); // Convertir en float
            lightThreshold = threshold; // Définir le seuil de luminosité
        } else {
            lightThreshold = 1.0; // Valeur par défaut si 'seuil' n'est pas précisé
        }

        // Vérifier si le seuil est valide
        if (threshold <= 0) {
            request->send(400, "application/json", "{\"status\":\"error\", \"message\":\"Invalid 'threshold' value\"}"); // Envoyer une réponse d'erreur
            return;
        }

        // Vérifier si la valeur du paramètre 'state' est valide
        if (state == "true") {
            nightMode = true; // Activer le mode nuit
            request->send(200, "application/json", "{\"status\":\"success\", \"message\":\"Night mode enabled with threshold " + String(lightThreshold) + "\"}"); // Envoyer une réponse JSON
            displayMessage("Night mode ON\nThreshold: " + String(lightThreshold)); // Afficher un message sur l'écran OLED
        } else if (state == "false") {
            nightMode = false; // Désactiver le mode nuit
            digitalWrite(GREENLED_PIN, LOW); // S'assurer que la LED verte est éteinte
            request->send(200, "application/json", "{\"status\":\"success\", \"message\":\"Night mode disabled\"}"); // Envoyer une réponse JSON
            displayMessage("Night mode OFF"); // Afficher un message sur l'écran OLED
        } else {
            // Si la valeur du paramètre 'state' est invalide
            request->send(400, "application/json", "{\"status\":\"error\", \"message\":\"Invalid 'state' parameter\"}"); // Envoyer une réponse d'erreur JSON
        }
    } else {
        // Si l'un des paramètres 'state' ou 'seuil' est manquant
        request->send(400, "application/json", "{\"status\":\"error\", \"message\":\"Missing 'state' or 'seuil' parameter\"}"); // Envoyer une réponse d'erreur JSON
    }
}

// Gérer la route pour activer/désactiver le mode thermomètre ("/thermometerMode")
void handleThermometerMode(AsyncWebServerRequest* request) {
    if (request->hasParam("state")) { // Vérifier si les paramètres 'state' et 'threshold' sont fournis
        String state = request->getParam("state")->value(); // Récupérer la valeur du paramètre 'state'

        // Valeur par défaut du seuil de température
        float threshold = temperatureThreshold; // Initialiser avec la valeur actuelle du seuil de température
        
        // Vérifier si le paramètre 'temperatureThreshold' est fourni et l'utiliser si nécessaire
        if (request->hasParam("seuil")) {
            String thresholdStr = request->getParam("seuil")->value(); // Récupérer la valeur du paramètre 'threshold'
            threshold = thresholdStr.toFloat(); // Convertir en float
            temperatureThreshold = threshold; // Définir le seuil de température
        } else {
          temperatureThreshold = 20.0;
        }
        
        // Vérifier si le seuil est valide
        if (threshold <= (-15)) {
            request->send(400, "application/json", "{\"status\":\"error\", \"message\":\"Invalid 'threshold' value\"}"); // Envoyer une réponse d'erreur
            return;
        }

        // Vérifier si la valeur du paramètre 'state' est valide
        if (state == "true") {
            thermometerMode = true; // Activer le mode thermomètre
            request->send(200, "application/json", "{\"status\":\"success\", \"message\":\"Thermometer mode enabled with threshold " + String(temperatureThreshold) + "\"}"); // Envoyer une réponse JSON
            displayMessage("Thermometer mode ON\nThreshold: " + String(temperatureThreshold)); // Afficher un message sur l'écran OLED
        } else if (state == "false") {
            thermometerMode = false; // Désactiver le mode thermomètre
            digitalWrite(REDLED_PIN, LOW); // S'assurer que la LED rouge est éteinte
            request->send(200, "application/json", "{\"status\":\"success\", \"message\":\"Thermometer mode disabled\"}"); // Envoyer une réponse JSON
            displayMessage("Thermometer mode OFF"); // Afficher un message sur l'écran OLED
        } else {
            // Si la valeur du paramètre 'state' est invalide
            request->send(400, "application/json", "{\"status\":\"error\", \"message\":\"Invalid 'state' parameter\"}"); // Envoyer une réponse d'erreur JSON
        }
    } else {
        // Si l'un des paramètres 'state' ou 'threshold' est manquant
        request->send(400, "application/json", "{\"status\":\"error\", \"message\":\"Missing 'state' or 'seuil' parameter\"}"); // Envoyer une réponse d'erreur JSON
    }
}


// Gérer la route pour jouer des mélodies ("/play/melody")
void handlePlayMelody(AsyncWebServerRequest* request) {
    // Vérifier si le paramètre 'melody' est présent dans la requête
    if (request->hasParam("melody")) { 
        // Récupérer la valeur du paramètre 'melody' sous forme de chaîne
        String melodyParam = request->getParam("melody")->value(); 
        // Convertir la chaîne en entier pour obtenir le numéro de la mélodie
        int melodyNumber = melodyParam.toInt(); 

        // Appeler la fonction factorisée pour sélectionner et jouer la mélodie
        selectAndPlayMelody(melodyNumber);

        // Obtenir le nom de la mélodie correspondante pour la réponse
        String melodyName = getMelodyName(melodyNumber); 

        // Créer un document JSON pour la réponse
        DynamicJsonDocument jsonDoc(1024);
        jsonDoc["status"] = "success"; // Indiquer que l'opération a réussi
        jsonDoc["message"] = "Playing melody: " + String(melodyName); // Inclure le nom de la mélodie jouée
        
        // Sérialiser l'objet JSON en une chaîne pour l'envoyer en réponse
        String jsonResponse;
        serializeJson(jsonDoc, jsonResponse);

        // Envoyer une réponse HTTP 200 (succès) avec le JSON généré
        request->send(200, "application/json", jsonResponse);
    } else {
        // Si le paramètre 'melody' est manquant, préparer une réponse d'erreur
        DynamicJsonDocument jsonDoc(1024);
        jsonDoc["status"] = "error"; // Indiquer une erreur
        jsonDoc["message"] = "Missing 'melody' parameter"; // Message d'erreur pour le client
        
        // Sérialiser l'objet JSON en une chaîne pour l'envoyer en réponse
        String jsonResponse;
        serializeJson(jsonDoc, jsonResponse);

        // Envoyer une réponse HTTP 400 (mauvaise requête) avec le JSON généré
        request->send(400, "application/json", jsonResponse);
    }
}


// Gérer la route pour mettre en place une alarme ("/setAlarm")
void handleSetAlarm(AsyncWebServerRequest* request) {
    if (request->hasParam("hour") && request->hasParam("minute") && request->hasParam("melody")) {
        int alarmHour = request->getParam("hour")->value().toInt();
        int alarmMinute = request->getParam("minute")->value().toInt();
        int melodyNumber = request->getParam("melody")->value().toInt();

        // Vérifier que la mélodie est valide
        if (melodyNumber < 1 || melodyNumber > 5) {
            DynamicJsonDocument jsonDoc(1024);
            jsonDoc["status"] = "error";
            jsonDoc["message"] = "Invalid melody number. Use 1-5.";
            String jsonResponse;
            serializeJson(jsonDoc, jsonResponse);
            request->send(400, "application/json", jsonResponse);
            return;
        }

        // Enregistrer l'heure du réveil et la mélodie
        alarmTime.hour = alarmHour;
        alarmTime.minute = alarmMinute;
        alarmMelody = melodyNumber;

        String melodyName = getMelodyName(melodyNumber); // Appeler la fonction pour obtenir le nom de la mélodie

        // Répondre avec un message de succès
        DynamicJsonDocument jsonDoc(1024);
        jsonDoc["status"] = "success";
        jsonDoc["message"] = "Alarm set for " + String(alarmHour) + ":" + String(alarmMinute) + " with melody " + String(melodyName);
        String jsonResponse;
        serializeJson(jsonDoc, jsonResponse);
        request->send(200, "application/json", jsonResponse);
    } else {
        // Paramètres manquants
        DynamicJsonDocument jsonDoc(1024);
        jsonDoc["status"] = "error";
        jsonDoc["message"] = "Missing parameters";
        String jsonResponse;
        serializeJson(jsonDoc, jsonResponse);
        request->send(400, "application/json", jsonResponse);
    }
}

// Gérer la route pour récupérer l'état de la LED rouge ("/led/red/status")
void handleRedLEDStatus(AsyncWebServerRequest* request) {
    // Création d'un objet JSON pour la réponse
    DynamicJsonDocument jsonDoc(1024);

    // Lire l'état actuel de la LED rouge
    bool isOn = digitalRead(REDLED_PIN) == HIGH;

    // Remplir le JSON avec l'état de la LED
    jsonDoc["status"] = "success";
    jsonDoc["redLED"] = isOn ? "on" : "off"; // Indiquer si la LED est allumée ou éteinte

    // Sérialiser le JSON en chaîne de caractères
    String jsonResponse;
    serializeJson(jsonDoc, jsonResponse);

    // Envoyer la réponse JSON
    request->send(200, "application/json", jsonResponse);
}

// Gérer la route pour récupérer l'état de la LED verte ("/led/green/status")
void handleGreenLEDStatus(AsyncWebServerRequest* request) {
    // Création d'un objet JSON pour la réponse
    DynamicJsonDocument jsonDoc(1024);

    // Lire l'état actuel de la LED verte
    bool isOn = digitalRead(GREENLED_PIN) == HIGH;

    // Remplir le JSON avec l'état de la LED
    jsonDoc["status"] = "success";
    jsonDoc["greenLED"] = isOn ? "on" : "off"; // Indiquer si la LED est allumée ou éteinte

    // Sérialiser le JSON en chaîne de caractères
    String jsonResponse;
    serializeJson(jsonDoc, jsonResponse);

    // Envoyer la réponse JSON
    request->send(200, "application/json", jsonResponse);
}

// Fonction pour gérer les erreurs 404 (page non trouvée)
void handleNotFound(AsyncWebServerRequest* request) {
    request->send(404, "text/plain", "Non trouvé");  // Envoie un message d'erreur 404
    displayMessage("404 : Non trouvé");  // Affiche un message d'erreur sur l'écran TFT
}