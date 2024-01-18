/*
KINAR ZABAD - Laser Range Finder
File Name: KinarZabad
Written by: Eyal Rabinovich
Date: 1/1/24
*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>

/* Constants */

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

const int SS = 10;      // Chip Select Pin, must connect to GND
const int MOSI = 11;
const int MISO = 12;
const int SCK = 13;     // The clock pin



char buf [2];
volatile byte pos;
volatile boolean process_it;


// Variables
volatile uint16_t receivedWord = 0;     // 16-bit word received
//volatile uint8_t bitCounter = 0;
volatile uint8_t byteCounter = 0;
volatile uint8_t receivedByte = 0;


void setup() {
  pinMode(ackPin, OUTPUT);
  pinMode(dataPin, INPUT);
  pinMode(syncPin, INPUT);

  Serial.begin (9600);   // debugging

  // have to send on master in, *slave out*
  pinMode(MISO, OUTPUT);

  // turn on SPI in slave mode
  SPCR |= _BV(SPE);
  
  // turn on interrupts
  SPCR |= _BV(SPIE);
  
  pos = 0;
  process_it = false;

  //attachInterrupt(digitalPinToInterrupt(syncPin), syncInterrupt, RISING);     // Attach interrupt to the rising edge of the trigger pin
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

  if (process_it)
  {
    buf [pos] = 0;  
    Serial.println (buf);
    pos = 0;
    process_it = false;
  }  // end of flag set

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

// SPI interrupt routine
ISR(SPI_STC_vect)
{
  // Read the byte from SPDR
  receivedByte = SPDR;

  // Combine the received byte based on the counter
  receivedWord |= (receivedByte << (8 * (1 - byteCounter)));

  // Increment the counter, and reset if it reaches 2
  byteCounter = (byteCounter + 1) % 2;

  // Debugging: Print the received byte
  Serial.print("Received Byte: ");
  Serial.println(receivedByte, HEX);

  // Now, receivedWord contains the 16-bit value when byteCounter is 0
  if (byteCounter == 0)
  {
    // Debugging: Print the received word
    Serial.print("Received Word: ");
    Serial.println(receivedWord, HEX);

    // Optionally, reset receivedWord to zero if you don't need to accumulate across multiple words
    receivedWord = 0;
  }
}


// End of Function Implementation






