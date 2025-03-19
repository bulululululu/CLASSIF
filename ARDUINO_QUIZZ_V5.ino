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
int LANGUE = 0;
bool gameStarted = false;


// Définition des boutons corrects pour chaque étape
const byte stages[4][NUM_BUTTONS] = {
    {1, 1, 0, 0, 0, 0, 0, 0},  // Étape 1 : boutons à éteindre
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
    delay (1000); // delas pour donner de l'avance au démarrage à l'ESP32
    randomSeed(analogRead(0));
    int LANGUE = random(0,3); // Random Value from 0 to 3
    
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

    switch (LANGUE) {
    case 0:
      mySerial.println("Elimine les carres verts en appuyant dessus"); //FR
      break;
    case 1:
      mySerial.println("Verwijder de groene vierkanten door erop te drukken."); //NL
      break;
    case 2:
      mySerial.println("Eliminate the green squares by pressing on them.");//EN
      break;
    case 3:
      mySerial.println("Hallo! Dies ist eine Nachricht auf Deutsch."); //DE
      break;
    default:
      Serial.println("Erreur de la variable LANGUE !");
      break;
  }
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
              Serial.println("Français"); //FR
              break;
            case 1:
              Serial.println("Nederlands"); //NL
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
              mySerial.println("Il reste uniquement les triangles jaunes !"); //FR
              break;
            case 1:
              mySerial.println("Alleen de gele driehoeken blijven over!"); //NL
              break;
            case 2:
              mySerial.println("Only the yellow triangles remain!");//EN
              break;
            case 3:
              mySerial.println("Nur die gelben Dreiecke sind übrig!"); //DE
              break;
            default:
              Serial.println("Erreur étape de fin");
              break;
  }
        //resetGame(); // Pas utile le jeu reviens à zéro avec le timeout !
    } else {
        Serial.print("Étape "); Serial.println(currentStage + 1);
        
        if (currentStage == 0) {
              switch (LANGUE) {
              case 0:
                mySerial.println("Elimine les carres verts en appuyant dessus"); //FR
                break;
              case 1:
                mySerial.println("Verwijder de groene vierkanten door erop te drukken."); //NL
                break;
              case 2:
                mySerial.println("Eliminate the green squares by pressing on them.");//EN
                break;
              case 3:
                mySerial.println("Hallo! Dies ist eine Nachricht auf Deutsch."); //DE
                break;
              default:
                Serial.println("Erreur étape 0");
                break;
            }
          }

           if (currentStage == 1) {
              switch (LANGUE) {
              case 0:
                mySerial.println("Elimine les rectangles jaunes en appuyant dessus"); //FR
                break;
              case 1:
                mySerial.println("Verwijder de gele rechthoeken door erop te drukken."); //NL
                break;
              case 2:
                mySerial.println("Eliminate the yellow rectangles by pressing on them.");//EN
                break;
              case 3:
                mySerial.println("Entferne die gelben Rechtecke, indem du darauf drückst."); //DE
                break;
              default:
                Serial.println("Erreur étape 1");
                break;
            }
          }

           if (currentStage == 2) {
              switch (LANGUE) {
              case 0:
                mySerial.println("Elimine les carres rouges en appuyant dessus"); //FR
                break;
              case 1:
                mySerial.println("Verwijder de rode vierkanten door erop te drukken."); //NL
                break;
              case 2:
                mySerial.println("Eliminate the red squares by pressing on them.");//EN
                break;
              case 3:
                mySerial.println("Entferne die roten Quadrate, indem du darauf drückst."); //DE
                break;
              default:
                Serial.println("Erreur étape 2");
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
        mySerial.println("Elimine les carres verts en appuyant dessus"); //FR
        break;
      case 1:
        mySerial.println("Verwijder de groene vierkanten door erop te drukken."); //NL
        break;
      case 2:
        mySerial.println("Eliminate the green squares by pressing on them.");//EN
        break;
      case 3:
        mySerial.println("Entferne die grünen Quadrate, indem du darauf drückst."); //DE
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
