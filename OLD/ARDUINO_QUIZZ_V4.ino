#include <FastLED.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(11, 12); // RX, TX

#define NUM_BUTTONS 8  // Nombre de boutons et LEDs
#define LED_PIN 10
#define NUM_LANGUE_BUTTONS 4  // Nombre de boutons de langues

CRGB leds[NUM_BUTTONS];
const int buttonPins[NUM_BUTTONS] = {2, 3, 4, 5, 6, 7, 8, 9};  // Broches des boutons
const int langueButtons[NUM_LANGUE_BUTTONS] = {A0, A1, A2, A3};  // Boutons pour changer la langue NL-FR-DE-EN

bool ledState[NUM_BUTTONS];      // true = allumée, false = éteinte
bool disabledButtons[NUM_BUTTONS] = {false};  // Empêcher les boutons désactivés d'agir
int currentStage = 0;            // Étape actuelle
int LANGUE = 0;  // Variable de langue (0 à 3)
bool gameStarted = false;


// Définition des boutons corrects pour chaque étape
const byte stages[4][NUM_BUTTONS] = {
    {1, 1, 0, 0, 0, 0, 0, 0},  // Étape 1 : boutons corrects à éteindre
    {0, 0, 1, 1, 0, 0, 0, 0},  // Étape 2
    {0, 0, 0, 0, 1, 1, 0, 0},  // Étape 3
    {0, 0, 0, 0, 0, 0, 1, 1}   // Étape 4
};

// Timeout
unsigned long lastActionTime = 0;
const unsigned long TIMEOUT = 30000;  // 30 000 ms = 30 secondes

void setup() {
    Serial.begin(115200);
    mySerial.begin(9600);   // Port série vers ESP32
    FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_BUTTONS);
    
    for (int i = 0; i < NUM_BUTTONS; i++) {
        pinMode(buttonPins[i], INPUT_PULLUP);  // Activer résistance interne pull-up
        leds[i] = CRGB::White;  // Allume toutes les LEDs au départ
        ledState[i] = true;
        disabledButtons[i] = false;  // Aucun bouton désactivé au début
    }


    for (int i = 0; i < NUM_LANGUE_BUTTONS; i++) {
        pinMode(langueButtons[i], INPUT_PULLUP);
    }

    FastLED.show();
    Serial.println("Appuie sur un bouton pour commencer !");
    mySerial.println("Appuie               sur un bouton pour   commencer !");
    lastActionTime = millis();  // Initialiser le timer
}

void loop() {
  
    checkLangueButtons();  // Vérifier les boutons de langue
    checkTimeout();  // Vérifier si on doit reset après inactivité
  
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
                mySerial.print("Bouton press"); mySerial.write(0x82); mySerial.print(" : ");;mySerial.println(i);

                if (stages[currentStage][i] == 1 && ledState[i]) {  
                    leds[i] = CRGB::Black;  // Éteindre la LED
                    ledState[i] = false;
                    disabledButtons[i] = true;  // Désactiver le bouton
                    Serial.println("LED éteinte définitivement !");
                    mySerial.print("LED "); mySerial.write(0x82); mySerial.println("teinte !");
                } else if (stages[currentStage][i] == 0) {  
                    flashLed(i);
                }
                FastLED.show();
                lastActionTime = millis();  // Mettre à jour le timer
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

// Vérifie si un bouton de langue est pressé et met à jour LANGUE
void checkLangueButtons() {
    for (int i = 0; i < NUM_LANGUE_BUTTONS; i++) {
        if (digitalRead(langueButtons[i]) == LOW) {
            delay(50);
            LANGUE = i;
            Serial.print("Langue sélectionnée : ");
            Serial.println(LANGUE);
            mySerial.print("Langue               "); mySerial.print("s"); mySerial.write(0x82); mySerial.print("lectionn"); mySerial.write(0x82); mySerial.print("e :");
            mySerial.println(LANGUE);
            lastActionTime = millis();  // Mettre à jour le timer
            delay(500);
            resetGame(); //relance le jeu lors du changement de langue
        }
    }
}

// Vérifie si un bouton est pressé pour démarrer le jeu
void checkFirstPress() {
    for (int i = 0; i < NUM_BUTTONS; i++) {
        if (digitalRead(buttonPins[i]) == LOW) {
            Serial.println("Jeu commencé !");
            mySerial.print("Jeu commenc"); mySerial.write(0x82); mySerial.println(" !");
            gameStarted = true;
            Serial.println("Étape 1");
            mySerial.write(0x90); mySerial.println("tape 1");
            lastActionTime = millis();  // Mettre à jour le timer
            return;
        }
    }
}

// Vérifie si on dépasse le timeout et réinitialise le jeu
void checkTimeout() {
    if (millis() - lastActionTime > TIMEOUT) {
        Serial.println("Temps écoulé ! Réinitialisation du jeu...");
        resetGame();
    }
}

// Fait clignoter une LED rapidement (uniquement si elle est encore active)
void flashLed(int index) {
    Serial.print("Mauvais bouton : "); Serial.println(index);
    mySerial.print("Mauvais bouton : "); mySerial.println(index);
    if (!disabledButtons[index]) {  // Ne pas clignoter si la Led est déjà éteinte
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
        Serial.println("Jeu terminé ! Recommence !");
        mySerial.print("Jeu termin"); mySerial.write(0x82); mySerial.print(" !        "); mySerial.print("Red"); mySerial.write(0x82); mySerial.println("mmarage...");
        delay(500);
        resetGame();
    } else {
        Serial.print("Étape "); Serial.println(currentStage + 1);
        mySerial.write(0x90); mySerial.print("tape "); mySerial.println(currentStage + 1);
    }
    FastLED.show();
}


// Réinitialise le jeu après un timeout ou la fin du cycle
void resetGame() {
    currentStage = 0;
    gameStarted = false;
    Serial.println("Retour au début. Appuie sur un bouton pour commencer.");
    mySerial.println("Appuie               sur un bouton pour   commencer !");

    for (int i = 0; i < NUM_BUTTONS; i++) {
        leds[i] = CRGB::White;
        ledState[i] = true;
        disabledButtons[i] = false;
    }

    FastLED.show();
    lastActionTime = millis();  // Remettre à zéro le timer
}
