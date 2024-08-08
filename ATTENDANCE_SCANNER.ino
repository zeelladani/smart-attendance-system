#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h> // Include Wire library for I2C communication
#include <U8g2lib.h> // Include U8g2 library

#define SS_PIN 10 // RX slave select
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance.

byte card_ID[4]; // Card UID size 4 bytes
byte Name1[4] = {0x56, 0x22, 0xB8, 0x89}; // 56 22 B8 89
byte Name2[4] = {0xFA, 0xE6, 0xC4, 0x80}; // FA E6 C4 80

// If you want the Arduino to detect the cards only once
int NumbCard[2]; // This array contains the number of cards. In my case, I have just two cards.
int j = 0;

String Name;      // User name
long Roll_No;      // User number
String Enrollment;  // Student's enrollment
int n = 0;        // The number of card you want to detect (optional)

int const RedLed = 5;
int const Buzzer = 8;

// Initialize the U8g2 library for I2C communication with the SSD1306 OLED display
U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);

void setup() {
  Serial.begin(9600); // Initialize serial communications with the PC
  SPI.begin();         // Init SPI bus
  mfrc522.PCD_Init();  // Init MFRC522 card

  pinMode(RedLed, OUTPUT);
  pinMode(Buzzer, OUTPUT);

  // Initialize the OLED display
  u8g2.begin();
  u8g2.setFont(u8g2_font_unifont_t_symbols); // Choose a suitable font
  u8g2.clearBuffer(); // Clear the internal memory
  u8g2.drawStr(15, 20, "ZEEL LADANI"); // Display initial text
  u8g2.sendBuffer(); // Transfer internal memory to the display
  delay(1000); // Pause for 2 seconds
  u8g2.clearBuffer(); // Clear the display buffer
}

void loop() {
  // Look for new card
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return; // Go to start of loop if there is no card present
  }
  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial()) {
    return; // If read card serial(0) returns 1, the uid struct contains the ID of the read card.
  }

  bool isKnownCard = false;
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    card_ID[i] = mfrc522.uid.uidByte[i];
    if (card_ID[i] == Name1[i]) {
      Name = "ZEEL LADANI"; // User name
      Roll_No = 11;          // User number
      Enrollment = "22SDSEE01014"; // Student's enrollment
      j = 0;                // First number in the NumbCard array : NumbCard[j]
      isKnownCard = true;
    } else if (card_ID[i] == Name2[i]) {
      Name = "AMIT GOHEL"; // User name
      Roll_No = 05;          // User number
      Enrollment = "22SDSEE01014"; // Student's enrollment
      j = 1;                // Second number in the NumbCard array : NumbCard[j]
      isKnownCard = true;
    } else {
      isKnownCard = false;
      break; // Exit the loop if card ID does not match
    }
  }

  if (isKnownCard) {
    if (NumbCard[j] == 1) { // To check if the card already detected
      u8g2.clearBuffer(); // Clear the display buffer
      u8g2.setFont(u8g2_font_unifont_t_symbols); // Choose a suitable font
      u8g2.drawStr(2, 20, "ALREADY SCANNED"); // Display "DONE"
      u8g2.sendBuffer(); // Transfer internal memory to the display
      delay(1000); // Pause for 1 second
      u8g2.clearBuffer(); // Clear the display buffer
      return; // Exit the loop function to stop further processing
    } else {
      NumbCard[j] = 1; // Put 1 in the NumbCard array : NumbCard[j]={1,1} to let the Arduino know if the card was detected
      n++;             // Increment the card count
      digitalWrite(RedLed, HIGH); // Turn on Red LED
      digitalWrite(Buzzer, HIGH); // Turn on Buzzer
      delay(300); // Wait for 0.3 seconds
      digitalWrite(RedLed, LOW); // Turn off Red LED
      digitalWrite(Buzzer, LOW); // Turn off Buzzer
      Serial.print("DATA,DATE,TIME," + Name); // Send the Name to Excel
      Serial.print(",");
      Serial.print(Roll_No); // Send the Number to Excel
      Serial.print(",");
      Serial.println(Enrollment); // Send the Enrollment to Excel

      // Display count on OLED
      u8g2.clearBuffer(); // Clear the display buffer
      u8g2.setFont(u8g2_font_unifont_t_symbols); // Choose a suitable font
      u8g2.setCursor(0, 10); // Set cursor to top-left
      u8g2.print("ROLL NO: "); // Display text
      u8g2.print(Roll_No); // Display the student count
      u8g2.setCursor(0, 30); // Set cursor to top-left
      u8g2.print("TOTAL: "); // Display text    
      u8g2.print(n); // Display the student count
      u8g2.sendBuffer(); // Transfer internal memory to the display
    }
  }

  // Continue the loop to keep checking for more cards
  delay(100);
}
