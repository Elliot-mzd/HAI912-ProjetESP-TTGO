// ========================= CONFIGURATION =========================
// Déclaration des variables pour la mélodie à jouer
int* melodyToPlay = nullptr;  // Pointeur vers la mélodie à jouer
int* durationsToPlay = nullptr;  // Pointeur vers la durée des notes à jouer
int melodyLength = 0;  // Longueur de la mélodie (nombre de notes)
TaskHandle_t melodyTaskHandle = nullptr;  // Gestion de la tâche pour jouer la mélodie

// Notes (fréquences) et durées des mélodies
#define NOTE_C4  261
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659

// Durées (temps en ms)
#define WHOLE 800
#define HALF  400
#define QUARTER 200
#define EIGHTH 100

// Frère Jacques
int melody_jacques[] = {
    NOTE_C4, NOTE_D4, NOTE_E4, NOTE_C4, NOTE_C4, NOTE_D4, NOTE_E4, NOTE_C4,
    NOTE_E4, NOTE_F4, NOTE_G4, NOTE_E4, NOTE_F4, NOTE_G4,
    NOTE_G4, NOTE_A4, NOTE_G4, NOTE_F4, NOTE_E4, NOTE_C4,
    NOTE_G4, NOTE_A4, NOTE_G4, NOTE_F4, NOTE_E4, NOTE_C4
};
int durations_jacques[] = {
    QUARTER, QUARTER, QUARTER, QUARTER, QUARTER, QUARTER, QUARTER, QUARTER,
    QUARTER, QUARTER, HALF, QUARTER, QUARTER, HALF,
    QUARTER, QUARTER, QUARTER, QUARTER, QUARTER, HALF,
    QUARTER, QUARTER, QUARTER, QUARTER, QUARTER, HALF
};

// Au Clair de la Lune
int melody_au_clair_de_la_lune[] = {
    NOTE_C4, NOTE_C4, NOTE_C4, NOTE_D4, NOTE_E4, NOTE_D4, NOTE_C4, NOTE_E4, NOTE_D4, NOTE_D4, NOTE_C4,
    NOTE_C4, NOTE_C4, NOTE_C4, NOTE_D4, NOTE_E4, NOTE_D4, NOTE_C4, NOTE_E4, NOTE_D4, NOTE_D4, NOTE_C4,
};
int durations_au_clair_de_la_lune[] = {
    QUARTER, QUARTER, QUARTER, QUARTER, HALF, HALF, QUARTER, QUARTER, QUARTER, QUARTER, HALF,
    QUARTER, QUARTER, QUARTER, QUARTER, HALF, HALF, QUARTER, QUARTER, QUARTER, QUARTER, HALF,
};

// Für Elise (simplifiée)
int melody_fur_elise[] = {
    NOTE_E5, NOTE_DS5, NOTE_E5, NOTE_DS5, NOTE_E5, NOTE_B4, NOTE_D5, NOTE_C5, NOTE_A4,
    NOTE_C4, NOTE_E4, NOTE_A4, NOTE_B4, NOTE_E4, NOTE_GS4, NOTE_B4, NOTE_C5
};
int durations_fur_elise[] = {
    QUARTER, QUARTER, QUARTER, QUARTER, QUARTER, QUARTER, QUARTER, QUARTER, HALF,
    QUARTER, QUARTER, QUARTER, HALF, QUARTER, QUARTER, QUARTER, HALF
};

// Ode à la Joie (simplifiée)
int melody_ode_to_joy[] = {
    NOTE_E4, NOTE_E4, NOTE_F4, NOTE_G4, NOTE_G4, NOTE_F4, NOTE_E4, NOTE_D4,
    NOTE_C4, NOTE_C4, NOTE_D4, NOTE_E4, NOTE_E4, NOTE_D4, NOTE_D4,
    NOTE_E4, NOTE_E4, NOTE_F4, NOTE_G4, NOTE_G4, NOTE_F4, NOTE_E4, NOTE_D4,
    NOTE_C4, NOTE_C4, NOTE_D4, NOTE_E4, NOTE_D4, NOTE_C4, NOTE_C4
};
int durations_ode_to_joy[] = {
    HALF, HALF, HALF, HALF, HALF, HALF, HALF, HALF,
    HALF, HALF, HALF, HALF, HALF, HALF, WHOLE,
    HALF, HALF, HALF, HALF, HALF, HALF, HALF, HALF,
    HALF, HALF, HALF, HALF, HALF, HALF, WHOLE
};

// Sonnerie Nokia
int melody_nokia[] = {
    NOTE_E5, NOTE_D5, NOTE_E5, NOTE_D5, NOTE_E5, NOTE_D5, NOTE_E5, NOTE_D5, NOTE_E5, NOTE_CS5,
    NOTE_D5, NOTE_CS5, NOTE_B4, NOTE_E5, NOTE_D5, NOTE_E5, NOTE_CS5, NOTE_B4, NOTE_A4
};
int durations_nokia[] = {
    QUARTER, QUARTER, QUARTER, QUARTER, QUARTER, QUARTER, QUARTER, QUARTER, QUARTER, HALF,
    QUARTER, QUARTER, HALF, WHOLE, HALF, QUARTER, QUARTER, QUARTER, WHOLE
};

// ========================= MELODIES FUNCTIONS =========================
// Fonction pour jouer une mélodie spécifique
void playMelodyTask(void* parameter) {
    for (int i = 0; i < melodyLength; i++) {
        int note = melodyToPlay[i]; // Recupere la note
        int duration = durationsToPlay[i]; // Recupere la durée

        if (note > 0) {
            tone(BUZZER_PIN, note, duration); // Joue la note
        } else {
            noTone(BUZZER_PIN); // Silence
        }

        delay(duration);       // Durée de la note
        delay(50);             // Pause entre les notes
    }

    noTone(BUZZER_PIN);        // Arrête le buzzer à la fin de la mélodie
    melodyTaskHandle = nullptr; // Libère la tâche
    vTaskDelete(nullptr);      // Termine cette tâche
}

// Fonction pour sélectionner et jouer une mélodie spécifique
void selectAndPlayMelody(int melodyNumber) {
    // Empêcher de jouer une nouvelle mélodie si une autre est déjà en cours
    if (melodyTaskHandle != nullptr) {
        return; // Ne rien faire si une mélodie est déjà en cours
    }

    // Choisir la mélodie en fonction du numéro passé en paramètre
    switch (melodyNumber) {
        case 1: // Frère Jacques
            melodyToPlay = melody_jacques;
            durationsToPlay = durations_jacques;
            melodyLength = sizeof(melody_jacques) / sizeof(melody_jacques[0]);
            break;

        case 2: // Au clair de la lune
            melodyToPlay = melody_au_clair_de_la_lune;
            durationsToPlay = durations_au_clair_de_la_lune;
            melodyLength = sizeof(melody_au_clair_de_la_lune) / sizeof(melody_au_clair_de_la_lune[0]);
            break;

        case 3: // Für Elise
            melodyToPlay = melody_fur_elise;
            durationsToPlay = durations_fur_elise;
            melodyLength = sizeof(melody_fur_elise) / sizeof(melody_fur_elise[0]);
            break;

        case 4: // Ode à la joie
            melodyToPlay = melody_ode_to_joy;
            durationsToPlay = durations_ode_to_joy;
            melodyLength = sizeof(melody_ode_to_joy) / sizeof(melody_ode_to_joy[0]);
            break;

        case 5: // Sonnerie Nokia
            melodyToPlay = melody_nokia;
            durationsToPlay = durations_nokia;
            melodyLength = sizeof(melody_nokia) / sizeof(melody_nokia[0]);
            break;

        default: // Mélodie invalide
            melodyToPlay = nullptr;
            durationsToPlay = nullptr;
            melodyLength = 0;
            return; // Si la mélodie n'est pas valide, ne rien faire
    }

    // Créer une tâche pour jouer la mélodie
    xTaskCreate(
        playMelodyTask,    // Fonction qui joue la mélodie
        "MelodyTask",      // Nom de la tâche
        2048,              // Taille de la pile
        nullptr,           // Pas de paramètre
        1,                 // Priorité de la tâche
        &melodyTaskHandle  // Handler de la tâche
    );
    return;
}

// Fonction pour récupérer le nom de la mélodie en fonction du numéro
String getMelodyName(int melodyNumber) {
    // Tableau des noms de mélodies
    const char* melodies[] = {
        "Frère Jacques",
        "Au clair de la lune",
        "Für Elise",
        "Ode à la joie",
        "Sonnerie Nokia"
    };

    // Vérifier si le numéro de mélodie est valide
    if (melodyNumber >= 1 && melodyNumber <= 5) {
        return melodies[melodyNumber - 1]; // Décalage car les indices de tableau commencent à 0
    } else {
        return "Mélodie inconnue"; // Si le numéro de mélodie est invalide
    }
}