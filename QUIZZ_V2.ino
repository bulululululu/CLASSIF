#include <FastLED.h>

#define NUM_BUTTONS 8  // Nombre de boutons et LEDs
#define LED_PIN 10

CRGB leds[NUM_BUTTONS];
const int buttonPins[NUM_BUTTONS] = {2, 3, 4, 5, 6, 7, 8, 9};  // Broches des boutons

bool ledState[NUM_BUTTONS];      // true = allumée, false = éteinte
bool disabledButtons[NUM_BUTTONS] = {false};  // Empêcher les boutons désactivés d'agir
int currentStage = 0;            // Étape actuelle
bool gameStarted = false;

// Définition des boutons corrects pour chaque étape
const byte stages[4][NUM_BUTTONS] = {
    {1, 1, 0, 0, 0, 0, 0, 0},  // Étape 1 : boutons corrects à éteindre
    {0, 0, 1, 1, 0, 0, 0, 0},  // Étape 2
    {0, 0, 0, 0, 1, 1, 0, 0},  // Étape 3
    {0, 0, 0, 0, 0, 0, 1, 1}   // Étape 4
};

void setup() {
    Serial.begin(115200);
    FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_BUTTONS);
    
    for (int i = 0; i < NUM_BUTTONS; i++) {
        pinMode(buttonPins[i], INPUT_PULLUP);  // Activer résistance interne pull-up
        leds[i] = CRGB::White;  // Allume toutes les LEDs au départ
        ledState[i] = true;
        disabledButtons[i] = false;  // Aucun bouton désactivé au début
    }
    FastLED.show();
    Serial.println("Appuie sur un bouton pour commencer !");
}

void loop() {
    if (!gameStarted) {
        checkFirstPress();
        return;
    }

    bool stageCompleted = true;

    for (int i = 0; i < NUM_BUTTONS; i++) {
        if (digitalRead(buttonPins[i]) == LOW) {  // Bouton pressé
            delay(50);  // Anti-rebond

            if (!disabledButtons[i]) {  // Vérifie si le bouton est encore actif
                Serial.print("Bouton pressé: "); Serial.println(i);

                if (stages[currentStage][i] == 1 && ledState[i]) {  
                    leds[i] = CRGB::Black;  // Éteindre la LED
                    ledState[i] = false;
                    disabledButtons[i] = true;  // Désactiver le bouton
                    Serial.println("LED éteinte définitivement !");
                } else if (stages[currentStage][i] == 0) {  
                    flashLed(i);
                }
                FastLED.show();
                delay(200);
            }
        }

        // Vérifier si tous les bons boutons sont éteints
        if (stages[currentStage][i] == 1 && ledState[i]) {
            stageCompleted = false;
        }
    }

    if (stageCompleted) {
        nextStage();
    }
}

// Vérifie si un bouton est pressé pour démarrer le jeu
void checkFirstPress() {
    for (int i = 0; i < NUM_BUTTONS; i++) {
        if (digitalRead(buttonPins[i]) == LOW) {
            Serial.println("Jeu commencé !");
            gameStarted = true;
            Serial.println("Étape 1");
            return;
        }
    }
}

// Fait clignoter une LED rapidement (uniquement si encore active)
void flashLed(int index) {
    Serial.print("Mauvais bouton : "); Serial.println(index);
    if (!disabledButtons[index]) {  // Ne pas clignoter si déjà éteinte
        for (int j = 0; j < 5; j++) {
            leds[index] = CRGB::Black;
            FastLED.show();
            delay(50);
            leds[index] = CRGB::White;
            FastLED.show();
            delay(50);
        }
    }
}

// Passe à l'étape suivante
void nextStage() {
    currentStage++;
    if (currentStage >= 3) {
        Serial.println("Jeu terminé ! Redémmarage du jeu... !");
        delay(2000);
        currentStage = 0;

        for (int i = 0; i < NUM_BUTTONS; i++) {
            leds[i] = CRGB::White;
            ledState[i] = true;
            disabledButtons[i] = false;  // Réactivation pour le nouveau cycle
        }
    }
    Serial.print("Étape "); Serial.println(currentStage + 1);
    FastLED.show();
}
