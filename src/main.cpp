//
// C++ code
//
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>

// Display
LiquidCrystal_I2C lcd(0x3f, 16, 2);

// Data
int giorno = 0;
int mese = 0;
int anno = 0;

// Keypad
const byte ROWS = 4;
const byte COLS = 4; 

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte pinROWS[ROWS] = {2, 3, 4, 5}; 
byte pinCOLS[COLS] = {6, 7, 8, 9}; 

Keypad keypad4x4 = Keypad(makeKeymap(hexaKeys), pinROWS, pinCOLS, ROWS, COLS); 

/**
 * Setup dello schermo per inserimento data di nascita
 */
void impostaSchermoDataDiNascita() {
	lcd.backlight();
	lcd.print("Data di Nascita:");

	lcd.setCursor(0, 1);
  lcd.print("__/__/____");
}

void setup() {
  Serial.begin(9600);
  lcd.init();
  
  for (int i = 2; i <= 9; i++) {
  	pinMode(i, INPUT);
  }

  impostaSchermoDataDiNascita();
}

char getKey() {
  while (1) {
    char key = keypad4x4.getKey();
    if (key) {
      return key;
    }
  }
}

void loop() {
  char key = keypad4x4.getKey();
  Serial.println(key);
}
