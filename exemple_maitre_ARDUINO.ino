#include <SoftwareSerial.h>

SoftwareSerial mySerial(11, 12); // RX, TX (Connecter TX de l'Arduino à RX de l'ESP32 et vice-versa)

void setup() {
    Serial.begin(115200);   // Moniteur série Arduino
    mySerial.begin(9600);   // Port série vers ESP32
}

void loop() {
    // Envoi d'un message à l'ESP32
    mySerial.println("Hello ESP32 !");
    Serial.println("Message envoyé à l'ESP32.");

    // Attente de la réponse
    if (mySerial.available()) {
        String receivedMessage = mySerial.readStringUntil('\n');
        Serial.print("Réponse de l'ESP32 : ");
        Serial.println(receivedMessage);
    }

    delay(2000); // Pause de 2 secondes avant le prochain envoi
}
