#include <FastLED.h>
#include <Wire.h>

const byte slaveAddress = 0x08;
int valueToSend = 1;

#define NUM_BUTTONS 8  // Nombre de boutons et LEDs
#define LED_PIN 10 // Pin data pour les neopixels
#define NUM_LANGUE_BUTTONS 4  // Nombre de boutons de langues

CRGB leds[NUM_BUTTONS];
const int buttonPins[NUM_BUTTONS] = {2, 3, 4, 5, 6, 7, 8, 9};  // Broches des boutons
const int langueButtons[NUM_LANGUE_BUTTONS] = {A0, A1, A2, A3};  // Boutons pour changer la langue NL-FR-DE-EN

bool ledState[NUM_BUTTONS];      // true = allumée, false = éteinte
bool disabledButtons[NUM_BUTTONS] = {false};  // Empêcher les boutons désactivés d'agir
int currentStage = 0;            // Étape actuelle
int LANGUE;
bool gameStarted = false;

// Définition des boutons corrects pour chaque étape
const byte stages[4][NUM_BUTTONS] = {
    {1, 1, 0, 0, 0, 0, 0, 0},  // Étape 1 :  Les boutons à éteindre à la première étape
    {0, 0, 1, 1, 0, 0, 0, 0},  // Étape 2 :  Les boutons à éteindre à la seconde étape
    {0, 0, 0, 0, 1, 1, 0, 0},  // Étape 3 :  Les boutons à éteindre à la troisième étape
    {0, 0, 0, 0, 0, 0, 1, 1}   // Étape 4 :  Les boutons qui resteront allumés à la fin
};

// Timeout
unsigned long lastActionTime = 0;
const unsigned long TIMEOUT = 30000;  // 30 000 ms = 30 secondes

void setup() {
    Serial.begin(9600);
    Wire1.begin(); // Initialise en maître
    FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_BUTTONS);
    
    randomSeed(analogRead(A0)); // Initialisation de l'aléatoire avec une lecture analogique flottante (bruit)
    LANGUE = random(0, 4); // Génère un nombre entre 0 et 3 inclus
     
    delay (5000); // delais pour donner de l'avance au démarrage à l'ESP32
   
  
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
    Serial.println("démarrage !"); 

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

                if (stages[currentStage][i] == 1 && ledState[i]) {  
                    leds[i] = CRGB::Black;  // Éteindre la LED
                    ledState[i] = false;
                    disabledButtons[i] = true;  // Désactiver le bouton
                    Serial.println("LED éteinte définitivement !");
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
            switch (LANGUE) {
            case 0:
              Serial.println("Nederlands"); //NL
              break;
            case 1:
              Serial.println("Français"); //NL
              break;
            case 2:
              Serial.println("English");//EN
              break;
            case 3:
              Serial.println("Deutsh"); //DE
              break;
            default:
              Serial.println("Erreur de la variable LANGUE !");
              break;
  }
            lastActionTime = millis();  // Mettre à jour le timer
            delay(200);
            resetGame(); //relance le jeu lors du changement de langue
        }
    }
}

// Vérifie si un bouton est pressé pour démarrer le jeu
void checkFirstPress() {
    for (int i = 0; i < NUM_BUTTONS; i++) {
        if (digitalRead(buttonPins[i]) == LOW) {
            Serial.println("Jeu commencé !");
            gameStarted = true;
            Serial.println("Étape 1");
            lastActionTime = millis();  // Mettre à jour le timer
            return;
        }
    }
}

// Vérifie si on dépasse le timeout et réinitialise le jeu
void checkTimeout() {
    if (millis() - lastActionTime > TIMEOUT) {
        Serial.println("TIMEOUT...");
        resetGame();
    }
}

// Fait clignoter une LED rapidement (uniquement si elle est encore active)
void flashLed(int index) {
    Serial.print("Mauvais bouton : "); Serial.println(index);
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
        Serial.println("Jeu terminé !");
        switch (LANGUE) {
            case 0:
              valueToSend = 13; //NL STAGE 3
              Wire1.beginTransmission(slaveAddress);
              Wire1.write(valueToSend);
              Wire1.endTransmission();
              Serial.print("Envoyé au slave Metro : ");
              Serial.println(valueToSend);
              break;
            case 1:
              valueToSend = 14; //FR STAGE 3
              Wire1.beginTransmission(slaveAddress);
              Wire1.write(valueToSend);
              Wire1.endTransmission();
              Serial.print("Envoyé au slave Metro : ");
              Serial.println(valueToSend);
              break;
            case 2:
              valueToSend = 15; //EN STAGE 3
              Wire1.beginTransmission(slaveAddress);
              Wire1.write(valueToSend);
              Wire1.endTransmission();
              Serial.print("Envoyé au slave Metro : ");
              Serial.println(valueToSend);
              break;
            case 3:
              valueToSend = 16; //DE STAGE 3
              Wire1.beginTransmission(slaveAddress);
              Wire1.write(valueToSend);
              Wire1.endTransmission();
              Serial.print("Envoyé au slave Metro : ");
              Serial.println(valueToSend);
            default:
              Serial.println("Erreur de la variable LANGUE- STAGE 3 !");
              break;
  }
      for (int i = 0; i < NUM_BUTTONS; i++) {
        disabledButtons[i] = true; //désactive les boutons pour ne pas éteindre les leds de la dernière étape
    }

    } else {
        Serial.print("Étape "); Serial.println(currentStage+1);
        
        if (currentStage == 0) {
              switch (LANGUE) {
                case 0:
                  valueToSend = 1; //NL STAGE 0
                  Wire1.beginTransmission(slaveAddress);
                  Wire1.write(valueToSend);
                  Wire1.endTransmission();
                  Serial.print("Envoyé au slave Metro : ");
                  Serial.println(valueToSend);
                  break;
                case 1:
                  valueToSend = 2; //FR STAGE 0
                  Wire1.beginTransmission(slaveAddress);
                  Wire1.write(valueToSend);
                  Wire1.endTransmission();
                  Serial.print("Envoyé au slave Metro : ");
                  Serial.println(valueToSend);
                  break;
                case 2:
                  valueToSend = 3; //EN STAGE 0
                  Wire1.beginTransmission(slaveAddress);
                  Wire1.write(valueToSend);
                  Wire1.endTransmission();
                  Serial.print("Envoyé au slave Metro : ");
                  Serial.println(valueToSend);
                  break;
                case 3:
                  valueToSend = 4; //DE STAGE 0
                  Wire1.beginTransmission(slaveAddress);
                  Wire1.write(valueToSend);
                  Wire1.endTransmission();
                  Serial.print("Envoyé au slave Metro : ");
                  Serial.println(valueToSend);
                  break;
                default:
                  Serial.println("Erreur de la variable LANGUE- STAGE 0 !");
                  break;
            }
          }

           if (currentStage == 1) {
              switch (LANGUE) {
                  case 0:
                    valueToSend = 5; //NL STAGE 2
                    Wire1.beginTransmission(slaveAddress);
                    Wire1.write(valueToSend);
                    Wire1.endTransmission();
                    Serial.print("Envoyé au slave Metro : ");
                    Serial.println(valueToSend);
                    break;
                  case 1:
                    valueToSend = 6; //FR STAGE 2
                    Wire1.beginTransmission(slaveAddress);
                    Wire1.write(valueToSend);
                    Wire1.endTransmission();
                    Serial.print("Envoyé au slave Metro : ");
                    Serial.println(valueToSend);
                    break;
                  case 2:
                    valueToSend = 7; //EN STAGE 2
                    Wire1.beginTransmission(slaveAddress);
                    Wire1.write(valueToSend);
                    Wire1.endTransmission();
                    Serial.print("Envoyé au slave Metro : ");
                    Serial.println(valueToSend);
                    break;
                  case 3:
                    valueToSend = 8; //DE STAGE 2
                    Wire1.beginTransmission(slaveAddress);
                    Wire1.write(valueToSend);
                    Wire1.endTransmission();
                    Serial.print("Envoyé au slave Metro : ");
                    Serial.println(valueToSend);
                    break;
                  default:
                    Serial.println("Erreur de la variable LANGUE - STAGE 2 !");
                    break;
            }
                      }

           if (currentStage == 2) {
              switch (LANGUE) {
                  case 0:
                    valueToSend = 9; //NL STAGE 2
                    Wire1.beginTransmission(slaveAddress);
                    Wire1.write(valueToSend);
                    Wire1.endTransmission();
                    Serial.print("Envoyé au slave Metro : ");
                    Serial.println(valueToSend);
                    break;
                  case 1:
                    valueToSend = 10; //FR STAGE 2
                    Wire1.beginTransmission(slaveAddress);
                    Wire1.write(valueToSend);
                    Wire1.endTransmission();
                    Serial.print("Envoyé au slave Metro : ");
                    Serial.println(valueToSend);
                    break;
                  case 2:
                    valueToSend = 11; //EN STAGE 2
                    Wire1.beginTransmission(slaveAddress);
                    Wire1.write(valueToSend);
                    Wire1.endTransmission();
                    Serial.print("Envoyé au slave Metro : ");
                    Serial.println(valueToSend);
                    break;
                  case 3:
                    valueToSend = 12; //DE STAGE 2
                    Wire1.beginTransmission(slaveAddress);
                    Wire1.write(valueToSend);
                    Wire1.endTransmission();
                    Serial.print("Envoyé au slave Metro : ");
                    Serial.println(valueToSend);
                    break;
                  default:
                    Serial.println("Erreur de la variable LANGUE - STAGE 2 !");
                    break;
            }
          }
    }
    FastLED.show();
}


// Réinitialise le jeu après un timeout ou la fin du cycle
void resetGame() {
    currentStage = 0;
    gameStarted = false;
    Serial.println("Retour au début. Appuie sur un bouton pour commencer.");

      switch (LANGUE) {
        case 0:
          valueToSend = 1; //NL STAGE 0
          Wire1.beginTransmission(slaveAddress);
          Wire1.write(valueToSend);
          Wire1.endTransmission();
          Serial.print("Envoyé au slave Metro : ");
          Serial.println(valueToSend);
          break;
        case 1:
          valueToSend = 2; //FR STAGE 0
          Wire1.beginTransmission(slaveAddress);
          Wire1.write(valueToSend);
          Wire1.endTransmission();
          Serial.print("Envoyé au slave Metro : ");
          Serial.println(valueToSend);
          break;
        case 2:
          valueToSend = 3; //EN STAGE 0
          Wire1.beginTransmission(slaveAddress);
          Wire1.write(valueToSend);
          Wire1.endTransmission();
          Serial.print("Envoyé au slave Metro : ");
          Serial.println(valueToSend);
          break;
        case 3:
          valueToSend = 4; //DE STAGE 0
          Wire1.beginTransmission(slaveAddress);
          Wire1.write(valueToSend);
          Wire1.endTransmission();
          Serial.print("Envoyé au slave Metro : ");
          Serial.println(valueToSend);
          break;
        default:
          Serial.println("Erreur de la variable LANGUE !");
          break;
    }

    for (int i = 0; i < NUM_BUTTONS; i++) {
        leds[i] = CRGB::White;
        ledState[i] = true;
        disabledButtons[i] = false;
    }

    FastLED.show();
    lastActionTime = millis();  // Remettre à zéro le timer
}
