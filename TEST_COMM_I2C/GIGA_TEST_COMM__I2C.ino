#include <Wire.h>

const byte slaveAddress = 0x08;
int valueToSend = 0;

// SETUP - RUNS ONCE AT PROGRAM START --------------------------------------
void setup() {
  Wire1.begin(); // Initialise en maître
  Serial.begin(9600);
}

// LOOP - RUNS REPEATEDLY AFTER SETUP --------------------------------------
void loop() {
  valueToSend = (valueToSend + 1) % 10000; // Valeur de test entre 0 et 99

  Wire1.beginTransmission(slaveAddress);
  Wire1.write(valueToSend); // Envoie d'un octet
  Wire1.endTransmission();

  Serial.print("Envoyé au slave Metro : ");
  Serial.println(valueToSend);

  delay(500); 
}
