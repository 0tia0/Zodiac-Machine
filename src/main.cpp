//
// ZodiacMachine v1.0.0/DEV
// By Lorenzo Rocca, Mattia Marelli, Alessio Randò, Elia Simonetto
//

#pragma region Inclusioni

// Inclusione librerie
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>

#pragma endregion

#pragma region data structure and arrays

// Struct to contains the date
struct Date
{
    int day;
    int month;
    int year;

    bool bisestile = false;
};

// Struct to contains the zodiac sign information
struct zodiacSign
{
    Date date;
    int sign;
    int matrice;
    char *name;
    bool cusp;
};

// Array names of zodiac sign
char *zodiacSignName[] {
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

#pragma region config Tastierino e Display

// Display inizialization
LiquidCrystal_I2C lcd(0x3f, 16, 2);

// Keypad numbers of cols and rows
const byte ROWS = 4;
const byte COLS = 4;

// Keypad character
char hexaKeys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};

// Keypad pin definition
byte pinROWS[ROWS] = {2, 3, 4, 5};
byte pinCOLS[COLS] = {6, 7, 8, 9};

// Keypad inizialization
Keypad keypad4x4 = Keypad(makeKeymap(hexaKeys), pinROWS, pinCOLS, ROWS, COLS);

#pragma endregion 

#pragma region funzioni hardware

/**
 * @brief function to print date on the screen
 * 
 * @param title first line of text
 * @param subTitle second line of text
 */
void writeToLCD(char *title, char *subTitle)
{
    lcd.backlight();
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print(title);

    lcd.setCursor(0, 1);
    lcd.print(subTitle);
}

/**
 * @brief Get the first key pressed after function call
 * 
 * @return pressed char 
 */
char getKey()
{
    while (1)
    {
        char key = keypad4x4.getKey();
        if (key)
        {
            return key;
        }
    }
}

#pragma endregion

#pragma region calculation function

/**
 * @brief check if the year in the struct d is bisestile
 * 
 * @param d the date to check
 * @return true if bisestile
 * @return false is !bisestile
 */
bool checkBisestile(Date *d)
{
    if ((d->year % 4 == 0 && d->year % 100 != 0) || (d->year % 400 == 0))
    {
        return d->bisestile = true;
    }
}

/**
 * @brief convert the date to the days passed since the start of the year, for example if you born on 10/02 we can say that you're born on the 41fs day of the year
 * 
 * @param d the date to convert
 * @return int the total of the days 
 */
int fromDateToDays(Date d)
{

    int totalDays = 0;
    int daysGone[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};

    if (d.month == 1)
    {
        totalDays = d.day;
    }
    else
    {
        totalDays = daysGone[d.month - 1] + d.day;
    }

    if (d.month > 2 && d.bisestile)
    {
        totalDays++;
    }

    return totalDays;
}

/**
 * @brief calculate the zodiac sign from the date
 * 
 * @param d the date used for the calculation 
 * @return zodiacSign struct containg all the useful data
 */
zodiacSign calculateSign(Date d)
{
    zodiacSign zS;
    zS.date = d;

    // check bisestile
    checkBisestile(&zS.date);
    int totalDays = fromDateToDays(zS.date);

    int signRange[2][2][13] = {
        {
            // NORMALE
            {1, 20, 50, 110, 140, 141, 172, 204, 236, 266, 297, 326, 356}, // start
            {19, 49, 79, 139, 140, 171, 203, 235, 265, 296, 325, 356, 365} // end
        },
        {
            // BISESTILE
            {1, 20, 50, 110, 140, 141, 172, 204, 236, 266, 297, 326, 356}, // start
            {19, 49, 80, 139, 140, 171, 203, 235, 265, 296, 325, 356, 366} // end
        },
    };

    for (int i = 0; i < 13; i++)
    {
        if (totalDays >= signRange[(int)zS.date.bisestile][0][i] && totalDays <= signRange[(int)zS.date.bisestile][1][i])
        {
            if (i == 12)
            {
                zS.sign = 1;
            }
            else
            {
                zS.sign = i + 1;
            }
            break;
        }
    }

    return zS;
}

#pragma endregion

#pragma region setup and loop

void setup()
{
    Serial.begin(9600);
    lcd.init();

    for (int i = 2; i <= 9; i++)
    {
        pinMode(i, INPUT);
    }

    char *dateTitle = "Data di nascita:";
    char inputDate[11] = "__/__/____";

    writeToLCD(dateTitle, inputDate);

    int index = 0;
    bool writing = true;

    while (writing)
    {
        char key = getKey();
        if (key >= '0' && key <= '9' && index < 10)
        {
            inputDate[index] = key;
            index++;

            writeToLCD(dateTitle, inputDate);
        }
        else if (key == '#' && index > 9)
        {
            writing = false;
            break;
        }
        else if (key == '*' && index > 0)
        {
            if (index == 3 || index == 6)
            {
                index--;
            }
            index--;
            inputDate[index] = '_';
            writeToLCD(dateTitle, inputDate);
        }
        if (index == 2 || index == 5)
        {
            index++;
            inputDate[index - 1] = '/';
            continue;
        }
    }

    Date date;
    date.day = (inputDate[0] - '0') * 10 + (inputDate[1] - '0');
    date.month = (inputDate[3] - '0') * 10 + (inputDate[4] - '0');
    date.year = (inputDate[6] - '0') * 1000 + (inputDate[7] - '0') * 100 + (inputDate[8] - '0') * 10 + (inputDate[9] - '0');

    Serial.print(date.day);
    Serial.print("/");
    Serial.print(date.month);
    Serial.print("/");
    Serial.print(date.year);

    zodiacSign sign = calculateSign(date);

    writeToLCD("Segno Zodiacale:", zodiacSignName[sign.sign-1]);
}

void loop()
{
}

#pragma endregion