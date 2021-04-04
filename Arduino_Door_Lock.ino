// Uključite potrebne biblioteke
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Servo.h>
#include <SPI.h>
// Kreirajte instance
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
MFRC522 mfrc522(10, 9); // MFRC522 mfrc522(SS_PIN, RST_PIN)
Servo sg90;
// Inicijalizirajte iglice za led i servo 
// Plava LED je spojena na 5V
constexpr uint8_t servoPin = 8;
char initial_password[4] = {'1', '2', '3', '4'};  // Promjenjiva za pohranu početne lozinke
String tagUID = "D3 E3 53 21";  // Niz za pohranu UID-a oznake. Promijenite ga s UID-om svoje oznake
char password[4];   //Promjenjiva za pohranu lozinke korisnika
boolean RFIDMode = true; // boolean za promjenu načina rada
char key_pressed = 0; // Promjenjiva za pohranu dolaznih ključeva
uint8_t i = 0;  // Varijabla koja se koristi za brojač
// definirajući koliko redova i kolona ima naša tastatura
const byte rows = 4;
const byte columns = 4;
// pin mapa "keypada"
char hexaKeys[rows][columns] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
// inicijalizacijske igle za "keypad"
byte row_pins[rows] = {A0, A1, A2, A3};
byte column_pins[columns] = {2, 1, 0};
// Kreirajte instancu za "keypad"
Keypad keypad_key = Keypad( makeKeymap(hexaKeys), row_pins, column_pins, rows, columns);
void setup() {
  // Arduino Pin konfiguracija
  sg90.attach(servoPin);  //deklarisite iglu 8 za servo
  sg90.write(0); // Postavite početni položaj na 90 stepeni
  lcd.begin(16, 2);
  lcd.backlight();
  SPI.begin();  
  // Init SPI bus
  mfrc522.PCD_Init();   // Init MFRC522
  lcd.clear(); // ukloni sve sa LCD ekrana
}
void loop() {
  // Sistem će prvo potražiti način
  if (RFIDMode == true) {
    lcd.setCursor(0, 0);
    lcd.print("   pametna_brava");
    lcd.setCursor(0, 1);
    lcd.print(" Skenirajte svoju oznaku ");
    // Potražite nove kartice
    if ( ! mfrc522.PICC_IsNewCardPresent()) {
      return;
    }
    // Odaberite jednu karatu
    if ( ! mfrc522.PICC_ReadCardSerial()) {
      return;
    }
    //Čitanje s kartice
    String tag = "";
    for (byte j = 0; j < mfrc522.uid.size; j++)
    {
      tag.concat(String(mfrc522.uid.uidByte[j] < 0x10 ? " 0" : " "));
      tag.concat(String(mfrc522.uid.uidByte[j], HEX));
    }
    tag.toUpperCase();
    //Provjera kartice
    if (tag.substring(1) == tagUID)
    {
      // Ako se podudara UID oznake.
      lcd.clear();
      lcd.print("Oznaka se podudara");
      delay(3000);
      lcd.clear();
      lcd.print("Unesite lozinku:");
      lcd.setCursor(0, 1);
      RFIDMode = false; // Make RFID mode false
    }
    else
    {
      // Ako se ne podudara UID oznake.
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Prikazana pogresna oznaka");
      lcd.setCursor(0, 1);
      lcd.print("Pristup odbijen");

      delay(3000);

      lcd.clear();
    }
  }
  // Ako je RFID način netačan, tražit će tipke s tipkovnice
  if (RFIDMode == false) {
    key_pressed = keypad_key.getKey(); // Pohranjivanje ključeva
    if (key_pressed)
    {
      password[i++] = key_pressed; // Pohranjivanje u varijablu lozinke
      lcd.print("*");
    }
    if (i == 4) // Ako su popunjena 4 ključa
    {
      delay(200);
      if (!(strncmp(password, initial_password, 4))) // Ako se podudara lozinka
      {
        lcd.clear();
        lcd.print("Prihvaceno");
        sg90.write(90); // vrata su otvorena

        delay(3000);

        sg90.write(0); // Vrata su zatvorena
        lcd.clear();
        i = 0;
        RFIDMode = true; // učiniti RFID mod = 1
      }
      else    // Ako se lozinka ne podudara
      {
        lcd.clear();
        lcd.print("Pogresna lozinka");

        delay(3000);

        lcd.clear();
        i = 0;
        RFIDMode = true;  // učiniti RFID mod = 1
      }
    }
  }
}
