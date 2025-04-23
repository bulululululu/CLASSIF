#include <Adafruit_Protomatter.h>
#include <Adafruit_GFX.h>
//#include <Fonts/FreeMono9pt7b.h>

uint8_t rgbPins[]  = {2,3,4,5,6,7};
uint8_t addrPins[] = {A0,A1,A2,A3};
uint8_t clockPin   = 8;
uint8_t latchPin   = 10;
uint8_t oePin      = 9;

#define RXD2 11 // RX2 sur METRO
#define TXD2 12  // TX2 sur METRO

Adafruit_Protomatter matrix(
  192,          // Width of matrix (or matrices, if tiled horizontally)
  1,           // Bit depth, 1-6
  1, rgbPins,  // # of matrix chains, array of 6 RGB pins for each
  4, addrPins, // # of address pins (height is inferred), array of pins
  clockPin, latchPin, oePin, // Other matrix control pins
  false,       // No double-buffering here (see "doublebuffer" example)
  -1);         // Row tiling


// SETUP - RUNS ONCE AT PROGRAM START --------------------------------------

void setup(void) {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);

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

void loop(void) {
   if (Serial2.available()) {
        String receivedMessage = Serial2.readStringUntil('\n');
        //Serial.print("Message reçu de l'Arduino : ");
        //Serial.println(receivedMessage);
        matrix.fillScreen(0); // Clear the background
        matrix.setCursor(1, 0);
        matrix.write(0x10);
        matrix.print(" ");
        matrix.println(receivedMessage);
        
        // Réponse à l'Arduino
        //Serial2.println("Hello Arduino !");
        matrix.show(); // Copy data to matrix buffers
    }

    delay(50);
}
