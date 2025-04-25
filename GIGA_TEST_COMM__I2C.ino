//SDA Giga (D20) -> SDA Metro M0
//SCL Giga (D21) -> SCL Metro M0
//GND -> GND

#include <Wire.h>

const byte slaveAddress = 0x08;
int valueToSend = 0;

// SETUP - RUNS ONCE AT PROGRAM START --------------------------------------
void setup() {
  Wire.begin(); // Initialise en maître
  Serial.begin(9600);
}

// LOOP - RUNS REPEATEDLY AFTER SETUP --------------------------------------
void loop() {
  valueToSend = (valueToSend + 1) % 100; // Valeur de test entre 0 et 99

  Wire.beginTransmission(slaveAddress);
  Wire.write(valueToSend); // Envoie d'un octet
  Wire.endTransmission();

  Serial.print("Envoyé au slave Metro : ");
  Serial.println(valueToSend);

  delay(1000); 
}
