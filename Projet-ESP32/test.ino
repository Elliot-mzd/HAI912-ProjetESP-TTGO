// ========================= TEST FONCTIONS =========================
// Test de la LED rouge
test(redLEDTest) {
    digitalWrite(REDLED_PIN, HIGH);  // Allume la LED rouge
    assertEqual(digitalRead(REDLED_PIN), HIGH); // Vérifie que la LED rouge est allumée

    digitalWrite(REDLED_PIN, LOW);   // Éteint la LED rouge
    assertEqual(digitalRead(REDLED_PIN), LOW);  // Vérifie qu'elle est éteinte
}

// Test de la LED verte
test(greenLEDTest) {
    digitalWrite(GREENLED_PIN, HIGH);  // Allume la LED verte
    assertEqual(digitalRead(GREENLED_PIN), HIGH); // Vérifie que la LED verte est allumée

    digitalWrite(GREENLED_PIN, LOW);   // Éteint la LED verte
    assertEqual(digitalRead(GREENLED_PIN), LOW);  // Vérifie qu'elle est éteinte
}

// Test de la lecture de la température
test(readTemperatureTest) {
    float temperature = readTemperature();  // Lit la température
    assertMore(temperature, -40);  // Vérifie que la température est supérieure à -40
    assertLess(temperature, 125); // Vérifie qu'elle est inférieure à 125
}

// Test du capteur de lumière
test(lightSensorTest) {
    int lightValue = analogRead(PHOTORESISTOR_PIN);  // Lit la valeur du capteur de lumière
    assertMore(lightValue, 0);   // Vérifie que la valeur est positive
    assertLess(lightValue, 4096); // Vérifie qu'elle est dans la plage ADC
}

// Test du mode nuit
test(nightModeTest) {
    nightMode = true;  // Active le mode nuit
    assertEqual(nightMode, true);  // Vérifie que le mode nuit est activé
    nightMode = false;  // Désactive le mode nuit
    assertEqual(nightMode, false);  // Vérifie que le mode nuit est désactivé
}

// Test du mode thermomètre
test(thermometerModeTest) {
    thermometerMode = true;  // Active le mode thermomètre
    assertEqual(thermometerMode, true);  // Vérifie que le mode thermomètre est activé
    thermometerMode = false;  // Désactive le mode thermomètre
    assertEqual(thermometerMode, false);  // Vérifie que le mode thermomètre est désactivé
}

test(thresholdValuesTest) {
    // Test temperature threshold
    temperatureThreshold = 20.0;
    assertMore(temperatureThreshold, -15.0);
    assertLess(temperatureThreshold, 125.0);

    // Test light threshold
    lightThreshold = 1.0;
    assertMore(lightThreshold, 0.0);
    assertLess(lightThreshold, 3.3);
}

// ========================= MELODY TESTS =========================
test(melodySelectionTest) {
    // Test valid melody selection
    String melodyName = getMelodyName(1);
    assertEqual(melodyName, "Frère Jacques");

    melodyName = getMelodyName(2);
    assertEqual(melodyName, "Au clair de la lune");

    // Test invalid melody number
    melodyName = getMelodyName(0);
    assertEqual(melodyName, "Mélodie inconnue");

    melodyName = getMelodyName(6);
    assertEqual(melodyName, "Mélodie inconnue");
}

