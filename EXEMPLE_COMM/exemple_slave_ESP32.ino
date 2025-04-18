#define RXD2 22 // RX2 sur l'ESP32
#define TXD2 23  // TX2 sur l'ESP32

void setup() {
    Serial.begin(115200);  // Moniteur série ESP32
    Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
}

void loop() {
    if (Serial2.available()) {
        String receivedMessage = Serial2.readStringUntil('\n');
        Serial.print("Message reçu de l'Arduino : ");
        Serial.println(receivedMessage);

        // Réponse à l'Arduino
        Serial2.println("Hello Arduino !");
    }

    delay(500);
}
