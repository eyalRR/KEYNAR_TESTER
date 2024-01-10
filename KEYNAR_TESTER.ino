/*
KINAR ZABAD - Laser Range Finder
File Name: KinarZabad
Written by: Eyal Rabinovich
Date: 1/1/24
*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define DELAY_AFTER_TRIGGER     850     // micro seconds
#define BIT_DURATION            100     // micro seconds
#define DIFFERENCE_THRESHHOLD   100     // meters
#define SAMPLE_SIZE             15      
#define SAMPLE_RATE             100     // milli seconds
#define WORD_SIZE               16      // number of bits
#define UPDATE_RATE             1000    // milli seconds

/* Function Prototypes */

void resetArray(uint16_t *arr);
int findMaxIndex(int *array, int size);
uint16_t mostCommon(uint16_t *arr);
bool isEqual(int num1, int num2);
void startUpLCD();
void printDistance(uint16_t measuredDistance);

// end of Function Prototypes

LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display


// Pin defenitions
const int triggerPin = 3;   // The pin for the rising edge trigger
const int syncPin = 2;      // The pin for the rising edge trigger
const int dataPin = 8;      // The pin to read the 16-bit word
const int checkPin = 9;     // The pin to check if the trigger has occurred


// Variables
volatile uint16_t receivedWord = 0;     // 16-bit word received
uint16_t measuredDistance = 0;
uint16_t samples[SAMPLE_SIZE] = {0};

void setup() {
  pinMode(triggerPin, OUTPUT);
  pinMode(dataPin, INPUT);
  pinMode(syncPin, INPUT);
  pinMode(checkPin, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(syncPin), triggerInterrupt, RISING);     // Attach interrupt to the rising edge of the trigger pin
  startUpLCD();
  delay(5000);
}

void loop() {
  resetArray(samples);
  digitalWrite(triggerPin, LOW);
  delay(UPDATE_RATE);

  for (int i = 0; i < SAMPLE_SIZE; i++) 
  {
    digitalWrite(triggerPin, LOW);
    receivedWord = 0;
    delay(SAMPLE_RATE);
    digitalWrite(triggerPin, HIGH);
    // Wait after the trigger
    delayMicroseconds( DELAY_AFTER_TRIGGER);
    // Read the 16-bit word
    for (int i = WORD_SIZE; i >= 0; --i) 
    {
      // Set the corresponding bit in the word
      receivedWord |= (digitalRead(dataPin) << i);
      // Wait for the next interval
      delayMicroseconds(0.91 * BIT_DURATION);
    }
    samples[i] = receivedWord;
  }
  measuredDistance = mostCommon(samples);

  printDistance(measuredDistance);

}

  /* Function Implementations */
  
  void resetArray(uint16_t *arr){
    for(int i = 0; i < SAMPLE_SIZE; i++){
      arr[i] = 0;
    }
  }

  uint16_t mostCommon(uint16_t *arr)
  {
    uint16_t counterArr[SAMPLE_SIZE] = {0};
    for(int i = 0; i < SAMPLE_SIZE; i++)
    {
      for(int j = 0; j < SAMPLE_SIZE; j++)
      {

        if(i != j)
        {
          if(isEqual(arr[i], arr[j]))
          {
            counterArr[i]++;
          }
        }
      }
    }

    return ( arr[ findMaxIndex( counterArr, SAMPLE_SIZE ) ] );
  }

  int findMaxIndex(uint16_t *array, int size) { 
    uint8_t maxIndex = 0;
    uint16_t maxValue = array[0];
    for (int i = 1; i < size; i++) {
        if (array[i] > maxValue) {
            maxValue = array[i];
            maxIndex = i;
        }
    }
    return maxIndex;
  }

/**
 * @brief Checks if the absolute difference between two integers is less than a predefined threshold.
 *
 * This function compares two integer values, `num1` and `num2`, and returns true if the absolute
 * difference between them is less than a predefined threshold (`DIFFERENCE_THRESHOLD`), indicating
 * that the two numbers are considered equal. Otherwise, it returns false.
 *
 * @param num1 The first integer to compare.
 * @param num2 The second integer to compare.
 * @return true if the absolute difference is less than the threshold, false otherwise.
 */
  bool isEqual(int num1, int num2)
  {
    if(abs(num1 - num2) < DIFFERENCE_THRESHHOLD)
      return true;
    return false;
  }


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
  lcd.setCursor(2, 0);
  lcd.print("Distance is: ");
  lcd.setCursor(2, 1);
  lcd.print(String(measuredDistance, HEX));
}

// Interrupt service routine for the rising edge trigger
void triggerInterrupt() 
{
  // Record the time of the trigger
  digitalWrite(checkPin, HIGH); 
  digitalWrite(checkPin, LOW); 

  // Set the corresponding bit in the word
  //receivedWord |= (digitalRead(dataPin) << i);
  //Serial.println("triggerOccurred");
}

// End of Function Implementation






