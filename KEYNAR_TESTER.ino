/*
KINAR ZABAD - Laser Range Finder
File Name: KinarZabad
Written by: Eyal Rabinovich
Date: 1/1/24
*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define WORD_SIZE               16      // number of bits
#define UPDATE_RATE             1000    // milli seconds
#define STARTUP_DELAY           5000    // milli seconds
#define LASER_RETURN_DELAY      1000    // milli seconds

/* Function Prototypes */

void startUpLCD();
void printDistance(uint16_t measuredDistance);

// end of Function Prototypes

LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display


// Pin defenitions
const int ackPin = 3;       // The trigger pin to send the data out to dataPin
const int syncPin = 2;      // The pin for the rising edge trigger ,works as clock for the data
const int dataPin = 8;      // The pin to read the 16-bit word

// Variables
volatile uint16_t receivedWord = 0;     // 16-bit word received
volatile uint8_t bitCounter = 0;

void setup() {
  pinMode(ackPin, OUTPUT);
  pinMode(dataPin, INPUT);
  pinMode(syncPin, INPUT);

  attachInterrupt(digitalPinToInterrupt(syncPin), syncInterrupt, RISING);     // Attach interrupt to the rising edge of the trigger pin
  startUpLCD();
  delay(STARTUP_DELAY);
}

void loop() {
  /* Initiate pins */
  digitalWrite(ackPin, LOW);
  delay(UPDATE_RATE);

  /* Initiate variables */
  receivedWord = 0;
  bitCounter = 0;

  /* Trigger the laser */
  digitalWrite(ackPin, HIGH);

  /* Wait for the laser to return the data */
  delay(LASER_RETURN_DELAY);

  printDistance(receivedWord);

}

  /* Function Implementations */

void startUpLCD()
{
  lcd.init();         // initialize the lcd
  lcd.backlight();    // Turn on the LCD screen backlight
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("KEYNAR TESTER");
  lcd.setCursor(0, 1);
  lcd.print("Eyal Rabinovich");
}

void printDistance(uint16_t measuredDistance)
{
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("KEYNAR TESTER");
  lcd.setCursor(0, 1);
  lcd.print("Distance: " + String(measuredDistance, HEX));
}

// Interrupt service routine for the rising edge trigger
void syncInterrupt() 
{
  if (bitCounter < WORD_SIZE)
  {
    // Set the corresponding bit in the word
    receivedWord |= (digitalRead(dataPin) << (WORD_SIZE - bitCounter -1) );
    bitCounter++;
  }
}

// End of Function Implementation






