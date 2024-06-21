#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h> // Include Wire library for I2C communication
#include <Adafruit_GFX.h> // Include Adafruit GFX library
#include <Adafruit_SSD1306.h> // Include Adafruit SSD1306 OLED display library

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET    -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); // Initialize with I2C address 0x3C (for the 128x32)

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

void setup() {
  Serial.begin(9600); // Initialize serial communications with the PC
  SPI.begin();         // Init SPI bus
  mfrc522.PCD_Init();  // Init MFRC522 card

  pinMode(RedLed, OUTPUT);
  pinMode(Buzzer, OUTPUT);

  // Initialize the OLED display
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.setTextSize(2); // Set text size to 1
  display.setTextColor(SSD1306_WHITE); // Set text color to white
  display.clearDisplay(); // Clear the display buffer
  display.setCursor(40, 10); // Set cursor to top-left
  display.print("ZEEL"); // Display initial text
  display.display(); // Show the display
  delay(1000); // Pause for 2 seconds
  display.clearDisplay(); // Clear the display buffer
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

  for (byte i = 0; i < mfrc522.uid.size; i++) {
    card_ID[i] = mfrc522.uid.uidByte[i];

    if (card_ID[i] == Name1[i]) {
      Name = "ZEEL LADANI"; // User name
      Roll_No = 11;          // User number
      Enrollment = "22SDSEE01014"; // Student's enrollment
      j = 0;                // First number in the NumbCard array : NumbCard[j]
    } else if (card_ID[i] == Name2[i]) {
      Name = "AMIT GOHEL"; // User name
      Roll_No = 05;          // User number
      Enrollment = "22SDSEE01014"; // Student's enrollment
      j = 1;                // Second number in the NumbCard array : NumbCard[j]
    } else {
      goto cont; // Go directly to line 85
    }
  }
  if (NumbCard[j] == 1) { // To check if the card already detected
    // If you want to use LCD
    Serial.println("Already done");
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
    display.setTextSize(2); // Set text size to 1
    display.clearDisplay(); // Clear the display buffer
    display.setCursor(0, 0); // Set cursor to top-left
    display.print("ROLL NO:"); // Display text
    display.println(Roll_No); // Display the student count
    delay(50);
    display.setCursor(0, 17); // Set cursor to top-left
    display.print("PRESENT:"); // Display text    
    display.println(n); // Display the student count
    display.display(); // Show the display

    // display.setTextSize(1); // Set text size to 1
    // display.clearDisplay(); // Clear the display buffer
    // display.setCursor(5, 15); // Set cursor to top-left
    // display.print("students : "); // Display text
    // display.println(Roll_No); // Display the student count
    // display.display(); // Show the display
  }

  // Continue the loop to keep checking for more cards
  delay(100);
  return; // Added return to continue the loop
cont:
  delay(200);
}
