#include <Adafruit_Protomatter.h>
#include <Adafruit_GFX.h>

#include <Wire.h> //I2C Communication

uint8_t rgbPins[]  = {2,3,4,5,6,7};
uint8_t addrPins[] = {A0,A1,A2,A3};
uint8_t clockPin   = 8;
uint8_t latchPin   = 10;
uint8_t oePin      = 9;

volatile int affiche = -1;
int previous_affiche = -1;

Adafruit_Protomatter matrix(
  192,          // Width of matrix (or matrices, if tiled horizontally)
  1,           // Bit depth, 1-6
  1, rgbPins,  // # of matrix chains, array of 6 RGB pins for each
  4, addrPins, // # of address pins (height is inferred), array of pins
  clockPin, latchPin, oePin, // Other matrix control pins
  false,       // No double-buffering here (see "doublebuffer" example)
  0);         // Row tiling


// SETUP - RUNS ONCE AT PROGRAM START --------------------------------------

void setup(void) {
  Wire.begin(0x08); // Initialise en esclave à l'adresse 0x08
  Wire.onReceive(receiveEvent); // Déclare la fonction de réception
  Serial.begin(9600);

  // Initialize matrix...
  ProtomatterStatus status = matrix.begin();
  Serial.print("Protomatter begin() status: ");
  Serial.println((int)status);
  if(status != PROTOMATTER_OK) {
    // DO NOT CONTINUE if matrix setup encountered an error.
    for(;;);
  }
}

// LOOP - RUNS REPEATEDLY AFTER SETUP --------------------------------------
void loop() {
  if (affiche != previous_affiche) {
    previous_affiche = affiche;
    Serial.print("Valeur reçue : ");
    Serial.println(affiche);
    matrix.fillScreen(0); // Clear the background
    matrix.setCursor(1, 0);
    matrix.write(0x10);
    matrix.print("Valeur : ");
    matrix.println(affiche);
    matrix.show();
    }
 delay(10);
}
//I2C communication
void receiveEvent(int howMany) {
  while (Wire.available()) {
    affiche = Wire.read(); // Lecture du byte reçu
  }
}
