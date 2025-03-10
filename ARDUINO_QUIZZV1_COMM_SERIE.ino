#include <FastLED.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(11, 12); // RX, TX

#define NUM_BUTTONS 8  // Nombre de boutons et LEDs
#define LED_PIN 10 // Pin DATA des LEDs

CRGB leds[NUM_BUTTONS];
const int buttonPins[NUM_BUTTONS] = {2, 3, 4, 5, 6, 7, 8, 9};  // Pins des boutons

bool ledState[NUM_BUTTONS];  // true = allumée, false = éteinte
int currentStage = 0;         // Étape actuelle
bool gameStarted = false;

// Définition des boutons corrects pour chaque étape
const byte stages[4][NUM_BUTTONS] = {
    {1, 1, 0, 0, 0, 0, 0, 0},  // Étape 1 : boutons corrects à éteindre
    {0, 0, 1, 1, 0, 0, 0, 0},  // Étape 2
    {0, 0, 0, 0, 1, 1, 0, 0},  // Étape 3
    {0, 0, 0, 0, 0, 0, 1, 1}   // Étape 4 // PAS UTILISEE juste pour infos
};

void setup() {
    Serial.begin(115200);
    mySerial.begin(9600);   // Port série vers ESP32
    FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_BUTTONS);
    
    for (int i = 0; i < NUM_BUTTONS; i++) {
        pinMode(buttonPins[i], INPUT_PULLUP);  // Activer résistance interne pull-up
        leds[i] = CRGB::White;  // Allume toutes les LEDs au départ
        ledState[i] = true;
    }
    FastLED.show();
    Serial.println("Appuie sur un bouton pour commencer !");
    mySerial.println("Appuie sur un bouton pour commencer !");
}

void loop() {
    if (!gameStarted) {
        checkFirstPress();  // Attend un premier appui pour démarrer
        return;
    }

    bool stageCompleted = true;

    for (int i = 0; i < NUM_BUTTONS; i++) {
        if (digitalRead(buttonPins[i]) == LOW) {  // Bouton pressé
            delay(50);  // Anti-rebond
            Serial.print("Bouton presse: "); Serial.println(i);
            mySerial.print("Bouton presse: "); mySerial.println(i);

            if (stages[currentStage][i] == 1 && ledState[i]) {  
                leds[i] = CRGB::Black;  // Éteindre la LED
                ledState[i] = false;
                Serial.println("LED eteinte !");
                mySerial.println("LED eteinte !");
            } else if (stages[currentStage][i] == 0) {  
                flashLed(i);
            }
            FastLED.show();
            delay(200);
        }

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
            Serial.println("Jeu commence !");
            mySerial.println("Jeu commence !");
            gameStarted = true;
            Serial.println("*Étape 1");
            mySerial.println("*Étape 1");
            return;
        }
    }
}

// Fait clignoter une LED rapidement
void flashLed(int index) {
    Serial.print("Mauvais bouton : "); Serial.println(index);
    mySerial.print("Mauvais bouton : "); mySerial.println(index);
    for (int j = 0; j < 5; j++) {
        leds[index] = CRGB::Black;
        FastLED.show();
        delay(50);
        leds[index] = CRGB::White;
        FastLED.show();
        delay(50);
    }
}

// Passe à l'étape suivante
void nextStage() {
    currentStage++;
    if (currentStage >= 3) {
        Serial.println("Jeu termine !");
        mySerial.println("Jeu termine !");
        delay(500);
        mySerial.println("5 !");
        delay(500);
        mySerial.println("4 !");
        delay(500);
        mySerial.println("3 !");
        delay(500);
        mySerial.println("2 !");
        delay(500);
        mySerial.println("1 !");
        delay(500);
        currentStage = 0;

        for (int i = 0; i < NUM_BUTTONS; i++) {
            leds[i] = CRGB::White;
            ledState[i] = true;
        }
    }
    Serial.print("*Étape "); Serial.println(currentStage + 1);
    mySerial.print("*Étape "); mySerial.println(currentStage + 1);
    FastLED.show();
}
