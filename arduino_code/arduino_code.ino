int leftPin = 10;
int rightPin = 11;
int clockSpeed = 500;
int clockHalf = 0;

  // See http://en.wikipedia.org/wiki/Magnetic_stripe_card for
  // explanation of what these bits mean.
  // In particular this is track 1, and it is encoded using DEC SIXBIT:
  // as explained here: http://en.wikipedia.org/wiki/Six-bit_character_code#Examples_of_sixbit_ASCII_variants
static byte cc[] = {
     // Start sentinel (%)
    1, 0, 1, 0, 0, 0, 1,

    // Format code (B)
    0, 1, 0, 0, 0, 1, 1,

    // Account number-- this is what we play with. Indices 14:125 inclusive.
    // Right now it's just all 0's.
    0, 0, 0, 0, 1, 0, 0,
    0, 0, 0, 0, 1, 0, 0,
    0, 0, 0, 0, 1, 0, 0,
    0, 0, 0, 0, 1, 0, 0,
    0, 0, 0, 0, 1, 0, 0,
    0, 0, 0, 0, 1, 0, 0,
    0, 0, 0, 0, 1, 0, 0,
    0, 0, 0, 0, 1, 0, 0,
    0, 0, 0, 0, 1, 0, 0,
    0, 0, 0, 0, 1, 0, 0,
    0, 0, 0, 0, 1, 0, 0,
    0, 0, 0, 0, 1, 0, 0,
    0, 0, 0, 0, 1, 0, 0,
    0, 0, 0, 0, 1, 0, 0,
    0, 0, 0, 0, 1, 0, 0,
    0, 0, 0, 0, 1, 0, 0,

    // Field separator (^)
    0, 1, 1, 1, 1, 1, 0,
 
    // Name (DIAO/LAWRENCE )
    0, 0, 1, 0, 0, 1, 1,
    1, 0, 0, 1, 0, 1, 0, 
    1, 0, 0, 0, 0, 1, 1, 
    1, 1, 1, 1, 0, 1, 0, 
    1, 1, 1, 1, 0, 0, 1, 
    0, 0, 1, 1, 0, 1, 0, 
    1, 0, 0, 0, 0, 1, 1, 
    1, 1, 1, 0, 1, 1, 0, 
    0, 1, 0, 0, 1, 1, 0, 
    1, 0, 1, 0, 0, 1, 0, 
    0, 1, 1, 1, 0, 1, 1, 
    1, 1, 0, 0, 0, 1, 0, 
    1, 0, 1, 0, 0, 1, 0, 
    0, 0, 0, 0, 0, 0, 1,
    
    // Field separator (^)
    0, 1, 1, 1, 1, 1, 0, 
 
    // Expitarion as YYMM (1605)
    1, 0, 0, 0, 1, 0, 1,
    0, 1, 1, 0, 1, 0, 0,
    0, 0, 0, 0, 1, 0, 0,
    1, 0, 1, 0, 1, 0, 0,

    // Service code (see wiki page linked above) (101)
    1, 0, 0, 0, 1, 0, 1,
    0, 0, 0, 0, 1, 0, 0, 
    1, 0, 0, 0, 1, 0, 1, 
    
    // Discretionary data.
    1, 0, 0, 0, 1, 0, 1, 
    0, 0, 0, 0, 1, 0, 0, 
    0, 0, 0, 0, 1, 0, 0, 
    0, 0, 0, 0, 1, 0, 0, 
    0, 0, 0, 0, 1, 0, 0, 
    1, 0, 0, 0, 1, 0, 1, 
    1, 0, 0, 0, 1, 0, 1, 
    0, 0, 0, 0, 1, 0, 0, 
    0, 0, 0, 0, 1, 0, 0, 
    0, 0, 0, 0, 1, 0, 0, 
    0, 0, 0, 0, 1, 0, 0, 
    0, 0, 0, 0, 1, 0, 0, 
    0, 0, 0, 0, 1, 0, 0, 
    0, 0, 0, 0, 1, 0, 0, 
    0, 0, 0, 0, 1, 0, 0, 
    0, 0, 0, 1, 1, 0, 1, 
    0, 0, 1, 0, 1, 0, 1, 
    1, 0, 0, 1, 1, 0, 0, 
    0, 0, 0, 0, 1, 0, 0,
    0, 0, 0, 0, 1, 0, 0, 
    0, 0, 0, 0, 1, 0, 0, 
    0, 0, 0, 0, 1, 0, 0, 
    0, 0, 0, 0, 1, 0, 0, 
    0, 0, 0, 0, 1, 0, 0,
    
    // End sentinel (?)
    1, 1, 1, 1, 1, 0, 0,
   
    // Longitudinal Redundancy Check (LRC)
    // See http://en.wikipedia.org/wiki/Longitudinal_redundancy_check
    1, 1, 1, 1, 0, 1, 0};
   
void setup() {
  // The interrupts are key to reliable
  // reading of the clock and data feed
  pinMode(leftPin, OUTPUT);
  pinMode(rightPin, OUTPUT);
  
  Serial.begin(9600);
}

void writeLow()
{
  digitalWrite(leftPin, LOW);
  digitalWrite(rightPin, LOW);
}

void writeBit(int lowOrHigh)
{
  if(lowOrHigh == 1)
  {
    if(clockHalf == 0)
    {
      digitalWrite(rightPin, LOW);
      digitalWrite(leftPin, HIGH);
      delayMicroseconds(clockSpeed);
      digitalWrite(leftPin, LOW);
      digitalWrite(rightPin, HIGH);
      delayMicroseconds(clockSpeed);
      //clockHalf = 1;
    }
    else
    {
      digitalWrite(leftPin, LOW);
      digitalWrite(rightPin, HIGH);
      delayMicroseconds(clockSpeed);
      digitalWrite(rightPin, LOW);
      digitalWrite(leftPin, HIGH);
      delayMicroseconds(clockSpeed);
      //clockHalf = 0;
    }
  }
  else
  {
    if(clockHalf == 0)
    {
      digitalWrite(rightPin, LOW);
      digitalWrite(leftPin, HIGH);
      delayMicroseconds(clockSpeed * 2);
      clockHalf = 1;
    }
    else
    {
      digitalWrite(leftPin, LOW);
      digitalWrite(rightPin, HIGH);
      delayMicroseconds(clockSpeed * 2);
      clockHalf = 0;
    }
  }
}

void convertDecimal(int digit, byte *sixbit) {
  // Digit must be between 0 and 9.
  // First 4 bits have to be between 0-9. See: http://en.wikipedia.org/wiki/Six-bit_character_code#DEC_sixbit_code
  //Serial.println("hello");
  for (int i = 0; i < 4; ++i) {
    sixbit[i] = (digit >> i) & 1;
  }
  
  // Next 2 bits need to be 1. See the above link.
  sixbit[4]= 1;
  sixbit[5]= 0;
  sixbit[6] = computeParity(sixbit, 6);
}

// Randomize the account number.
void randomizeNumber(byte* cc)
{
  int first = 0;
  int last = 16; //Not inclusive

  for (int i = 7 * (first+2); i < 7 * (last+2); i += 7) {
    int rand = random(10);
    convertDecimal(rand, cc+i);
  }  
}

// Longitudinal Redundancy Check (LRC)
// See http://en.wikipedia.org/wiki/Longitudinal_redundancy_check
void setLRC(byte* cc, int len)
{
  byte lrc[] = {0, 0, 0, 0, 0, 0};
  for (int i = 0; i < len-7; i+=7) {
    for (int j = 0; j < 6; j++) {
      lrc[j] ^= cc[i+j];
    }
  }
  
  for (int j = 0; j < 6; j++) {
    cc[len-7+j] = lrc[j];
  }
  cc[len-1] = computeParity(lrc, 6);
}

byte computeParity(byte* arr, int len) {
  byte parity = 1;
  for (int i = 0; i < len; i++) {
    parity ^= arr[i];
  }
  return parity;
}

void writeCC(byte* cc, int len) {
  for (int i = 0; i < len; i++) {
    writeBit(cc[i]);
  }  
}
  
static int switchCC = 1;
void loop(){
  // This gets output to the "Serial Monitor" at the top-right button on the screen.
  //Serial.println("hello");
  
  // Calculate random number
  randomizeNumber(myCC);
  setLRC(cc, 469);
  
  //Turn on LED
  analogWrite(5, 255);
  analogWrite(6, 0);

` // Write the credit card, padded with 0's.
  for(int i = 0; i < 20; i++) {
    writeBit(0);
  }
  writeCC(cc, 469);
  for(int i = 0; i < 20; i++) {
    writeBit(0);
  }
  
  // Turn off LED
  analogWrite(5, 0);
  
  writeLow();
  delay(2000);  // Wait 1 second between writes.

}
