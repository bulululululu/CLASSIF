#include <Adafruit_Protomatter.h>
#include <Adafruit_GFX.h>

#include <Wire.h> //I2C Communication

uint8_t rgbPins[]  = {2,3,4,5,6,7};
uint8_t addrPins[] = {A0,A1,A2,A3};
uint8_t clockPin   = 8;
uint8_t latchPin   = 10;
uint8_t oePin      = 9;

volatile int affichage = -1;
int previous_affichage = -1;

#define BLACK    0x0000
#define BLUE     0x001F
#define RED      0xF800
#define GREEN    0x07E0
#define CYAN     0x07FF
#define MAGENTA  0xF81F
#define YELLOW   0xFFE0 
#define WHITE    0xC638

#define FLECHE     0x10 //>
#define EACCENT    0x82 //é
#define EMAJACCENT 0x90 //É


Adafruit_Protomatter matrix(
  192,          // Width of matrix (or matrices, if tiled horizontally) 192,128,64
  1,           // Bit depth, 1-6
  1, rgbPins,  // # of matrix chains, array of 6 RGB pins for each
  4, addrPins, // # of address pins (height is inferred), array of pins
  clockPin, latchPin, oePin, // Other matrix control pins
  false,       // No double-buffering here (see "doublebuffer" example)
  -1);         // Row tiling


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
  matrix.setTextColor(WHITE);
}

// LOOP - RUNS REPEATEDLY AFTER SETUP --------------------------------------
void loop() {
  if (affichage != previous_affichage) {
    previous_affichage = affichage;
        switch (affichage) {
      case 1: //TXT NL ETAPE 0
        matrix.fillScreen(0);
        matrix.setCursor(1, 0);
        matrix.write(0x10);
        matrix.println(" Verwijder de groene vakjes door erop te drukken");
        matrix.println(affichage);
        matrix.show();
        break;
      case 2: //TXT FR ETAPE 0
        matrix.fillScreen(0);
        matrix.setCursor(1, 0);
        matrix.write(FLECHE);//fleche
        matrix.print(" ");
        matrix.write(EMAJACCENT);//É
        matrix.print("limine les carr");
        matrix.write(EACCENT); //é
        matrix.println("s verts");
        matrix.println("  en appuyant dessus");
        matrix.println(" ");
        matrix.println(affichage);
        matrix.show();
        break;
      case 3: //TXT EN ETAPE 0
        matrix.fillScreen(0);
        matrix.setCursor(1, 0);
        matrix.write(FLECHE);
        matrix.println(" Remove the green squares by tapping on them");
        matrix.println(affichage);
        matrix.show();
        break;
      case 4: //TXT DE ETAPE 0
        matrix.fillScreen(0);
        matrix.setCursor(1, 0);
        matrix.write(FLECHE);
        matrix.println(" Entferne die grünen Quadrate, indem du darauf tippst");
        matrix.println(affichage);
        matrix.show();
        break;
      case 5: //TXT NL ETAPE 1
        matrix.fillScreen(0);
        matrix.setCursor(1, 0);
        matrix.write(FLECHE);
        matrix.println(" Verwijder de gele rechthoeken door erop te drukken");
        matrix.println(affichage);
        matrix.show();
        break;
      case 6: //TXT FR ETAPE 1
        matrix.fillScreen(0);
        matrix.setCursor(1, 0);
        matrix.write(FLECHE);//fleche
        matrix.print(" ");
        matrix.write(EMAJACCENT);//É
        matrix.println("limine les rectangles jaunes");
        matrix.println("  en appuyant dessus");
        matrix.println(" ");
        matrix.println(affichage);
        matrix.show();
        break;
      case 7: //TXT EN ETAPE 1
        matrix.fillScreen(0);
        matrix.setCursor(1, 0);
        matrix.write(FLECHE);
        matrix.println(" Remove the yellow rectangles by tapping on them");
        matrix.println(affichage);
        matrix.show();
        break;
      case 8: //TXT DE ETAPE 1
        matrix.fillScreen(0);
        matrix.setCursor(1, 0);
        matrix.write(FLECHE);
        matrix.println(" Entferne die gelben Rechtecke, indem du darauf tippst");
        matrix.println(affichage);
        matrix.show();
        break;
      case 9: //TXT NL ETAPE 2
        matrix.fillScreen(0);
        matrix.setCursor(1, 0);
        matrix.write(FLECHE);
        matrix.println(" Verwijder de rode vakjes door erop te drukken");
        matrix.println(affichage);
        matrix.show();
        break;
      case 10: //TXT FR ETAPE 2
        matrix.fillScreen(0);
        matrix.setCursor(1, 0);
        matrix.write(FLECHE);//fleche
        matrix.print(" ");
        matrix.write(EMAJACCENT);//É
        matrix.print("limine les carr");
        matrix.write(EACCENT); //é
        matrix.println("s rouges");
        matrix.println("  en appuyant dessus");
        matrix.println(" ");
        matrix.println(affichage);
        matrix.show();
        break;
      case 11: //TXT EN ETAPE 2
        matrix.fillScreen(0);
        matrix.setCursor(1, 0);
        matrix.write(FLECHE);
        matrix.println(" Remove the red squares by tapping on them");
        matrix.println(affichage);
        matrix.show();
        break;
      case 12: //TXT DE ETAPE 2
        matrix.fillScreen(0);
        matrix.setCursor(1, 0);
        matrix.write(FLECHE);
        matrix.println(" Entferne die roten Quadrate, indem du darauf tippst");
        matrix.println(affichage);
        matrix.show();
        break;
      case 13: //TXT NL ETAPE 3
        clignote(100, GREEN);
        matrix.fillScreen(0);
        matrix.setCursor(1, 0);
        matrix.write(FLECHE);
        matrix.println(" Alleen de gele driehoeken blijven over!");
        matrix.println(affichage);
        matrix.show();
        break;
      case 14: //TXT FR ETAPE 3
        clignote(100, GREEN);
        matrix.fillScreen(0);
        matrix.setCursor(1, 0);
        matrix.write(FLECHE);
        matrix.println(" Il reste uniquement");
        matrix.println("  les triangles jaunes !");
        matrix.println(" ");
        matrix.println(affichage);
        matrix.show();
        break;
      case 15: //TXT EN ETAPE 3
        clignote(100, GREEN);
        matrix.fillScreen(0);
        matrix.setCursor(1, 0);
        matrix.write(FLECHE);
        matrix.println(" Only the yellow triangles are left!");
        matrix.println(affichage);
        matrix.show();
        break;
      case 16: //TXT DE ETAPE 3
        clignote(100, GREEN);
        matrix.fillScreen(0);
        matrix.setCursor(1, 0);
        matrix.write(FLECHE);
        matrix.println(" Es sind nur noch die gelben Dreiecke übrig!");
        matrix.println(affichage);
        matrix.show();
        break;
      default: //ERREUR !
        matrix.fillScreen(0);
        matrix.setCursor(1, 0);
        matrix.write(FLECHE);
        matrix.println(" ERREUR !");
        matrix.println(affichage);
        matrix.show();
        break;
      }

    Serial.print("Valeur reçue : ");
    Serial.println(affichage);
    }
 delay(20);
}
//I2C communication
void receiveEvent(int howMany) {
  while (Wire.available()) {
    affichage = Wire.read(); // Lecture du byte reçu
  }
}

//duree ECRAN
void clignote(int duree, int couleur) {
    matrix.fillScreen(couleur);
    matrix.show();
    delay(duree);
    matrix.fillScreen(0);
    matrix.show();
    delay(duree);
    matrix.fillScreen(couleur);
    matrix.show();
    delay(duree);       
    matrix.fillScreen(0);
    matrix.show();
    delay(duree);
    matrix.fillScreen(couleur);
    matrix.show();
    delay(duree);
}
