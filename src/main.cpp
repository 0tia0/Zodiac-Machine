//
// ZodiacMachine v1.0.0/DEV
// By Lorenzo Rocca, Mattia Marelli, Alessio Randò, Elia Simonetto
//

#pragma region Inclusioni e Macro

// Inclusione librerie
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>

// Macro x il tastierino
#define KEYPAD_ROWS                 4
#define KEYPAD_COLUMNS              4

#define KEYPAD_ROW_1_PIN            2
#define KEYPAD_ROW_2_PIN            3
#define KEYPAD_ROW_3_PIN            4
#define KEYPAD_ROW_4_PIN            5

#define KEYPAD_COLUMN_1_PIN         6
#define KEYPAD_COLUMN_2_PIN         7
#define KEYPAD_COLUMN_3_PIN         8
#define KEYPAD_COLUMN_4_PIN         9

// Macro x segno zodiacale
#define NORMAL_RANGE_START          1, 20, 50, 80, 110, 140, 171, 202, 233, 263, 293, 322, 356
#define NORMAL_RANGE_END            19, 49, 79, 109, 139, 170, 201, 232, 262, 292, 321, 355, 365
#define BISESTILE_RANGE_START       1, 20, 51, 81, 111, 141, 172, 203, 234, 264, 294, 323, 357
#define BISESTILE_RANGE_END         19, 50, 80, 110, 140, 171, 202, 233, 263, 293, 322, 356, 366

// Macro LCD
#define AGE_OF_BIRTH_TITLE          "Data di nascita:"

#pragma endregion

#pragma region Strutture dati

// Struttura per contenere la data
struct Date {
    // Parametri della data
    int day;
    int month;
    int year;

    // Se l'anno è bisestile
    bool bisestile = false;
};

// Struttura per contenere le informazioni del segno zodiacale
struct ZodiacSign {
    // La data
    Date date;

    // Dettagli del segno
    int sign;
    int matrice;
    char* name;
    bool cusp;
};

// I nomi dei segni zodiacali
char* zodiacSignName[] {
    "CAPRICORNO",
    "ACQUARIO",
    "PESCI",
    "ARIETE",
    "TORO",
    "GEMELLI",
    "CANCRO",
    "LEONE",
    "VERGINE",
    "BILANCIA",
    "SCORPIONE",
    "SAGGITTARIO"
};

#pragma endregion

#pragma region Display LCD

// Istanza del display LCD
LiquidCrystal_I2C lcd(0x3f, 16, 2);

/**
 * @brief Questa funzione scrive la data a schermo
 *
 * @param title Il titolo da stampare
 * @param subTitle Il sottotitolo
 */
void writeToLCD(char* title, char* subTitle) {
    // Azzera il display LCD
    lcd.backlight();
    lcd.clear();

    // Stampa il titolo
    lcd.setCursor(0, 0);
    lcd.print(title);

    // Stampa il sottotitolo
    lcd.setCursor(0, 1);
    lcd.print(subTitle);
}

#pragma endregion

#pragma region Keypad

// Caratteri del Keypad
char hexaKeys[KEYPAD_ROWS][KEYPAD_COLUMNS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

// Pin a cui sono state collegate le linee
byte pinRows[KEYPAD_ROWS] = {
    KEYPAD_ROW_1_PIN, 
    KEYPAD_ROW_2_PIN, 
    KEYPAD_ROW_3_PIN, 
    KEYPAD_ROW_4_PIN
};

// Pin a cui sono state collegate le colonne
byte pinCols[KEYPAD_COLUMNS] = {
    KEYPAD_COLUMN_1_PIN, 
    KEYPAD_COLUMN_2_PIN, 
    KEYPAD_COLUMN_3_PIN, 
    KEYPAD_COLUMN_4_PIN
};

// Inizializza il tastierino
Keypad keypad = Keypad(makeKeymap(hexaKeys), pinRows, pinCols, KEYPAD_ROWS, KEYPAD_COLUMNS);

/**
 * @brief Leggi, rimanendo in attesa, un carattere dal tastierino
 *
 * @return Il carattere del tasto premuto
 */
char getKey() {
    // Resta in loop finché non viene letto un carattere valido
    while (1) {
        // Leggi il carattere attualmente selezionato.
        char key = keypad.getKey();

        // Se il carattere è diverso da 0, ritornalo
        if (key) {
            return key;
        }
    }
}

#pragma endregion

#pragma region Elaborazione Data

/**
 * @brief Ritorna se una specifica data passata è bisestile
 *
 * @param date La data da controllare
 * @return Se è bisestile o meno
 */
bool checkBisestile(Date* date) {
    // Controllo per vedere se la data è bisestile
    if ((date->year % 4 == 0 && date->year % 100 != 0) || (date->year % 400 == 0)) {
        // Imposta la data come bisestile e ritorna true
        return date->bisestile = true;
    }

    // Ritorna false
    return date->bisestile = false;
}

/**
 * @brief Converti una data formattata nel rispettivo numero di giorno dall'inizio dell'anno.
 *
 * @param date La data da convertire
 * @return int Il numero di giorni dall'inizio dell'anno
 */
int fromDateToDays(Date date) {
    // Definizione dei numeri totali
    int totalDays = 0;
    // Per ciascun mese, il numero di giorni già passati
    int daysGone[12] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};

    // Imposta il numero di giorni passati
    totalDays = daysGone[date.month - 1] + date.day;

    // Incrementa di uno se si è superato febbraio e ci si trova in un anno bisestile
    if (date.month > 2 && date.bisestile) {
        totalDays++;
    }

    // Ritorna il numero totale di giorni passati
    return totalDays;
}

#pragma endregion

#pragma region Elaborazione Segno Zodiacale

/**
 * @brief Calcola il segno Zodiacale corrispondente alla data passata
 *
 * @param date La data da utilizzare
 * @return La struttura contenente tutte le informazioni necessarie
 */
ZodiacSign calculateSign(Date date) {
    // Definisci la struttura da ritornare in seguito
    ZodiacSign zodiacSign;
    zodiacSign.date = date;

    // Esegui la convalida dell'anno bisestile
    checkBisestile(&zodiacSign.date);

    // Ottieni il numero di giorni totali passati dall'inizio dell'anno
    int totalDays = fromDateToDays(zodiacSign.date);

    // Definisco i range per i giorni
    int signRange[2][13] = {
        {zodiacSign.date.bisestile ? BISESTILE_RANGE_START : NORMAL_RANGE_START},
        {zodiacSign.date.bisestile ? BISESTILE_RANGE_END : NORMAL_RANGE_END}
    };

    // Itera finché non si trova il range corretto
    for (int i = 0; i < 13; i++) {
        // Se ci si trova nel range, completa il calcolo
        if (totalDays >= signRange[0][i] && totalDays <= signRange[1][i]) {
            // Applica il segno
            zodiacSign.sign = i == 12 ? 1 : i + 1;
            break;
        }
    }

    // Ritorna il segno zodiacale
    return zodiacSign;
}

#pragma endregion

#pragma region Debug

/**
 * @brief La data da scrivere in seriale
 * 
 * @param date La data da scrivere
 */
void printDate(Date date) {
    Serial.print(date.day);
    Serial.print("/");
    Serial.print(date.month);
    Serial.print("/");
    Serial.print(date.year);
}

#pragma endregion

#pragma region Algoritmo Principale

/**
 * @brief Funzione di setup dell'arduino
 */
void setup() {
    // Inizializza la comunicazione seriale
    Serial.begin(9600);

    // Inizializza il display LCD
    lcd.init();

    // Imposta i pin del tastierino come pin di INPUT
    for (int i = 2; i <= 9; i++) {
        pinMode(i, INPUT);
    }

    // Array per la data, si modifica durante l'input
    char inputDate[11] = "__/__/____";

    // Scrivi il prompt sul display LCD
    writeToLCD(AGE_OF_BIRTH_TITLE, inputDate);

    // Definisco il cursore
    int cursor = 0;

    // Itero fino alla conferma
    while (1) {
        // Attendo l'input dell'utente
        char key = getKey();

        // Se si sta inserendo un numero
        if (key >= '0' && key <= '9' && cursor < 10) {
            // Scrivi il valore nel buffer
            inputDate[cursor] = key;
            cursor++;

            // Aggiorna il prompt
            writeToLCD(AGE_OF_BIRTH_TITLE, inputDate);
        }
        else if (key == '#' && cursor > 9) {
            // Completa l'input della data di nascita
            break;
        }
        else if (key == '*' && cursor > 0) {
            // Decrementa il cursore (di due se ci si trova prima dello slash)
            cursor -= cursor == 3 || cursor == 6 ? 2 : 1;
            inputDate[cursor] = '_';

            // Aggiorna il prompt
            writeToLCD(AGE_OF_BIRTH_TITLE, inputDate);
        }

        // Scrittura dello slash nei casi specifici
        if (cursor == 2 || cursor == 5) {
            // Scrivi lo slash
            inputDate[cursor] = '/';

            // Incrementa il cursore
            cursor++;
        }
    }

    // Costruisci la data sulla base dei dati letti
    Date date;
    date.day = (inputDate[0] - '0') * 10 + (inputDate[1] - '0');
    date.month = (inputDate[3] - '0') * 10 + (inputDate[4] - '0');
    date.year = (inputDate[6] - '0') * 1000 + (inputDate[7] - '0') * 100 + (inputDate[8] - '0') * 10 + (inputDate[9] - '0');

    // Scrivi la data x debug
    printDate(date);

    // Calcola il segno basandosi sulla data
    ZodiacSign sign = calculateSign(date);

    // Scrivi a schermo il segno zodiacale
    writeToLCD("Segno Zodiacale:", zodiacSignName[sign.sign - 1]);
}

/**
 * @brief Funzione loop. Attualmente, non esegue nulla
 */
void loop() {
    return;
}

#pragma endregion