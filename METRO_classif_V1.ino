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
#define WHITE    0xFFFF

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
  if (affichage != previous_affichage) {
    previous_affichage = affichage;
        switch (affichage) {
      case 1:
        matrix.fillScreen(0);
        matrix.setCursor(1, 0);
        matrix.write(0x10);
        matrix.println(" Verwijder de groene vakjes door erop te drukken");//TXT NL ETAPE 0
        matrix.println(affichage);
        matrix.show();
        break;
      case 2:
        matrix.fillScreen(0);
        matrix.setCursor(1, 0);
        matrix.write(0x10);
        matrix.println(" Élimine les carrés verts en appuyant dessus");//TXT FR ETAPE 0
        matrix.println(affichage);
        matrix.show();
        break;
      case 3:
        matrix.fillScreen(0);
        matrix.setCursor(1, 0);
        matrix.write(0x10);
        matrix.println(" Remove the green squares by tapping on them");//TXT EN ETAPE 0
        matrix.println(affichage);
        matrix.show();
        break;
      case 4:
        matrix.fillScreen(0);
        matrix.setCursor(1, 0);
        matrix.write(0x10);
        matrix.println(" Entferne die grünen Quadrate, indem du darauf tippst");//TXT DE ETAPE 0
        matrix.println(affichage);
        matrix.show();
        break;
      case 5:
        matrix.fillScreen(0);
        matrix.setCursor(1, 0);
        matrix.write(0x10);
        matrix.println(" Verwijder de gele rechthoeken door erop te drukken");//TXT NL ETAPE 1
        matrix.println(affichage);
        matrix.show();
        break;
      case 6:
        matrix.fillScreen(0);
        matrix.setCursor(1, 0);
        matrix.write(0x10);
        matrix.println(" Élimine les rectangles jaunes en appuyant dessus");//TXT FR ETAPE 1
        matrix.println(affichage);
        matrix.show();
        break;
      case 7:
        matrix.fillScreen(0);
        matrix.setCursor(1, 0);
        matrix.write(0x10);
        matrix.println(" Remove the yellow rectangles by tapping on them");//TXT EN ETAPE 1
        matrix.println(affichage);
        matrix.show();
        break;
      case 8:
        matrix.fillScreen(0);
        matrix.setCursor(1, 0);
        matrix.write(0x10);
        matrix.println(" Entferne die gelben Rechtecke, indem du darauf tippst");//TXT DE ETAPE 1
        matrix.println(affichage);
        matrix.show();
        break;
      case 9:
        matrix.fillScreen(0);
        matrix.setCursor(1, 0);
        matrix.write(0x10);
        matrix.println(" Verwijder de rode vakjes door erop te drukken");//TXT NL ETAPE 2
        matrix.println(affichage);
        matrix.show();
        break;
      case 10:
        matrix.fillScreen(0);
        matrix.setCursor(1, 0);
        matrix.write(0x10);
        matrix.println(" Élimine les carrés rouges en appuyant dessus");//TXT FR ETAPE 2
        matrix.println(affichage);
        matrix.show();
        break;
      case 11:
        matrix.fillScreen(0);
        matrix.setCursor(1, 0);
        matrix.write(0x10);
        matrix.println(" Remove the red squares by tapping on them");//TXT EN ETAPE 2
        matrix.println(affichage);
        matrix.show();
        break;
      case 12:
        matrix.fillScreen(0);
        matrix.setCursor(1, 0);
        matrix.write(0x10);
        matrix.println(" Entferne die roten Quadrate, indem du darauf tippst");//TXT DE ETAPE 2
        matrix.println(affichage);
        matrix.show();
        break;
      case 13:
        clignote(100, GREEN);
        matrix.fillScreen(0);
        matrix.setCursor(1, 0);
        matrix.write(0x10);
        matrix.println(" Alleen de gele driehoeken blijven over!");//TXT NL ETAPE 3
        matrix.println(affichage);
        matrix.show();
        break;
      case 14:
        clignote(100, GREEN);
        matrix.fillScreen(0);
        matrix.setCursor(1, 0);
        matrix.write(0x10);
        matrix.println(" Il reste uniquement les triangles jaunes !");//TXT FR ETAPE 3
        matrix.println(affichage);
        matrix.show();
        break;
      case 15:
        clignote(100, GREEN);
        matrix.fillScreen(0);
        matrix.setCursor(1, 0);
        matrix.write(0x10);
        matrix.println(" Only the yellow triangles are left!");//TXT EN ETAPE 3
        matrix.println(affichage);
        matrix.show();
        break;
      case 16:
        clignote(100, GREEN);
        matrix.fillScreen(0);
        matrix.setCursor(1, 0);
        matrix.write(0x10);
        matrix.println(" Es sind nur noch die gelben Dreiecke übrig!");//TXT DE ETAPE 3
        matrix.println(affichage);
        matrix.show();
        break;
      default:
        matrix.fillScreen(0);
        matrix.setCursor(1, 0);
        matrix.write(0x10);
        matrix.println(" ERREUR !");//ERREUR !
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
