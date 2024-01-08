#define DELAY_AFTER_TRIGGER     850     // micro seconds
#define BIT_DURATION            100     // micro seconds
#define THRESHHOLD              100

// function defenition

void resetArray(uint16_t *arr);
int findMaxIndex(int array[], int size);
void mostCommon(uint16_t *arr);
bool isEqual(int num1, int num2);


// Pin definitions
const int triggerPin = 3;  // The pin for the rising edge trigger
const int dataPin = 8;     // The pin to read the 16-bit word

const int checkPin = 4;     // The pin to read the 16-bit word

// Variables
volatile unsigned long startTime;       // Time of trigger
volatile uint16_t receivedWord = 0;     // 16-bit word received

uint16_t samples[10] = {0};

void setup() {
  Serial.begin(9600);

  pinMode(triggerPin, OUTPUT);
  pinMode(dataPin, INPUT);
  pinMode(checkPin, OUTPUT);
}



void loop() {
    resetArray(samples);
    digitalWrite(triggerPin, LOW);
    digitalWrite(checkPin, LOW);
    
  for (int i = 0; i < 10; i++) {
    digitalWrite(triggerPin, LOW);
    delay(1000);
    digitalWrite(triggerPin, HIGH);
    // Wait for 10 microseconds after the trigger
    delayMicroseconds( DELAY_AFTER_TRIGGER);


    // Read the 16-bit word
    receivedWord = 0;
    for (int i = 15; i >= 0; --i) {
      // Set the corresponding bit in the word
      receivedWord |= (digitalRead(dataPin) << i);
      digitalWrite(checkPin, HIGH);

      // Wait for the next interval
      delayMicroseconds(0.9 * BIT_DURATION);
      digitalWrite(checkPin, LOW);
    }

  samples[i] = receivedWord;

    // Print the result as binary
    //Serial.println("Received word (Binary): " + String(receivedWord, BIN));

    // Print the result as decimal (BCD)
    //Serial.println("Received word (Decimal): " + String(receivedWord, HEX));

  }


    mostCommon(samples);


  
    // Print the result as binary
    //Serial.println("Received word (Binary): " + String(receivedWord, BIN));

    // Print the result as decimal (BCD)
    //Serial.println("Received word (Decimal): " + String(receivedWord, HEX));
    
  }
  
  void resetArray(uint16_t *arr){
    for(int i = 0; i < 10; i++){
      arr[i] = 0;
    }
  }

  void mostCommon(uint16_t *arr)
  {
    uint16_t incs[10] = {0};
    for(int i = 0; i<10; i++)
    {
      for(int j = 0; j < 10; j++)
      {

        if(i != j)
        {

          if(isEqual(arr[i], arr[j]))
          {

            incs[i]++;
          }
        }
      }
    }

    Serial.print("most common: ");
    Serial.println(String(arr[findMaxIndex(incs, 10)], HEX));
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

  bool isEqual(int num1, int num2)
  {
    if(abs(num1 - num2) < THRESHHOLD)
      return true;
    return false;
  }

