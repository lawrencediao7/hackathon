int leftPin = 10;
int rightPin = 11;
int clockSpeed = 500;
int clockHalf = 0;

  // See http://en.wikipedia.org/wiki/Magnetic_stripe_card for
  // explanation of what these bits mean.
  // In particular this is track 1, and it is encoded using DEC SIXBIT:
  // as explained here: http://en.wikipedia.org/wiki/Six-bit_character_code#Examples_of_sixbit_ASCII_variants
static byte myCC[] = {
     // Start sentinel (%)
    1, 0, 1, 0, 0, 0, 1,

    // Format code (B)
    0, 1, 0, 0, 0, 1, 1,

    // Account number-- this is what we play with. Indices 14:125 inclusive.
    // 4147-2021-2639-6546
    0, 0, 1, 0, 1, 0, 1,
    1, 0, 0, 0, 1, 0, 1,  
    0, 0, 1, 0, 1, 0, 1,
    1, 1, 1, 0, 1, 0, 1,
    0, 1, 0, 0, 1, 0, 1,
    0, 0, 0, 0, 1, 0, 0, 
    0, 1, 0, 0, 1, 0, 1, 
    1, 0, 0, 0, 1, 0, 1, 
    0, 1, 0, 0, 1, 0, 1, 
    0, 1, 1, 0, 1, 0, 0, 
    1, 1, 0, 0, 1, 0, 0, 
    1, 0, 0, 1, 1, 0, 0, 
    0, 1, 1, 0, 1, 0, 0, 
    1, 0, 1, 0, 1, 0, 0, 
    0, 0, 1, 0, 1, 0, 1, 
    0, 1, 1, 0, 1, 0, 0,

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
 
  static byte hybridCC[] = {
     // Start sentinel (%)
    1, 0, 1, 0, 0, 0, 1,

    // Format code (B)
    0, 1, 0, 0, 0, 1, 1,

    // Account number-- this is what we play with. Indices 14:125 inclusive.
    // 4266-8412-9952-6002
    0, 0, 1, 0, 1, 0, 1, 
    0, 1, 0, 0, 1, 0, 1, 
    0, 1, 1, 0, 1, 0, 0, 
    0, 1, 1, 0, 1, 0, 0, 
    0, 0, 0, 1, 1, 0, 1, 
    0, 0, 1, 0, 1, 0, 1, 
    1, 0, 0, 0, 1, 0, 1, 
    0, 1, 0, 0, 1, 0, 1, 
    1, 0, 0, 1, 1, 0, 0, 
    1, 0, 0, 1, 1, 0, 0, 
    1, 0, 1, 0, 1, 0, 0, 
    0, 1, 0, 0, 1, 0, 1, 
    0, 1, 1, 0, 1, 0, 0, 
    0, 0, 0, 0, 1, 0, 0, 
    0, 0, 0, 0, 1, 0, 0, 
    0, 1, 0, 0, 1, 0, 1,

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
    
 static byte freedomCC[] = {
    // Start sentinel (%)
    1, 0, 1, 0, 0, 0, 1, 
    
    // Format code (B)
    0, 1, 0, 0, 0, 1, 1, 
    
    // Account number-- this is what we play with. Indices 14:125 inclusive.
    // 4266-8412-9952-6002
    0, 0, 1, 0, 1, 0, 1, 
    0, 1, 0, 0, 1, 0, 1, 
    0, 1, 1, 0, 1, 0, 0, 
    0, 1, 1, 0, 1, 0, 0, 
    0, 0, 0, 1, 1, 0, 1, 
    0, 0, 1, 0, 1, 0, 1, 
    1, 0, 0, 0, 1, 0, 1, 
    0, 1, 0, 0, 1, 0, 1, 
    1, 0, 0, 1, 1, 0, 0, 
    1, 0, 0, 1, 1, 0, 0, 
    1, 0, 1, 0, 1, 0, 0, 
    0, 1, 0, 0, 1, 0, 1, 
    0, 1, 1, 0, 1, 0, 0, 
    0, 0, 0, 0, 1, 0, 0, 
    0, 0, 0, 0, 1, 0, 0, 
    0, 1, 0, 0, 1, 0, 1,
   
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
    
    // Expitarion as YYMM (1508)
    1, 0, 0, 0, 1, 0, 1, 
    1, 0, 1, 0, 1, 0, 0, 
    0, 0, 0, 0, 1, 0, 0, 
    0, 0, 0, 1, 1, 0, 1, 
    
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
    0, 0, 1, 0, 1, 0, 1, 
    0, 1, 0, 0, 1, 0, 1, 
    0, 0, 0, 1, 1, 0, 1, 
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
    0, 0, 0, 1, 0, 1, 1};
   
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

void writeNumber(int numToWrite)
{
  if(numToWrite == 0)
  {
     writeBit(0);
     writeBit(0);
     writeBit(0);
     writeBit(0);
     writeBit(1);
   }
  else if(numToWrite == 1)
  {
     writeBit(1);
     writeBit(0);
     writeBit(0);
     writeBit(0);
     writeBit(0);
  }
  else if(numToWrite == 2)
  {
     writeBit(0);
     writeBit(1);
     writeBit(0);
     writeBit(0);
     writeBit(0);
  }
  else if(numToWrite == 3)
  {
     writeBit(1);
     writeBit(1);
     writeBit(0);
     writeBit(0);
     writeBit(1);
  }
  else if(numToWrite == 4)
  {
     writeBit(0);
     writeBit(0);
     writeBit(1);
     writeBit(0);
     writeBit(0);
  }
  else if(numToWrite == 5)
  {
     writeBit(1);
     writeBit(0);
     writeBit(1);
     writeBit(0);
     writeBit(1);
  }
  else if(numToWrite == 6)
  {
     writeBit(0);
     writeBit(1);
     writeBit(1);
     writeBit(0);
     writeBit(1);
  }
  else if(numToWrite == 7)
  {
     writeBit(1);
     writeBit(1);
     writeBit(1);
     writeBit(0);
     writeBit(0);
  }
  else if(numToWrite == 8)
  {
     writeBit(0);
     writeBit(0);
     writeBit(0);
     writeBit(1);
     writeBit(0);
  }
  else if(numToWrite == 9)
  {
     writeBit(1);
     writeBit(0);
     writeBit(0);
     writeBit(1);
     writeBit(1);
  }
  else if(numToWrite == 10)
  {
     writeBit(0);
     writeBit(1);
     writeBit(0);
     writeBit(1);
     writeBit(1);
  }
  else if(numToWrite == 11)
  {
     writeBit(1);
     writeBit(1);
     writeBit(0);
     writeBit(1);
     writeBit(0);
  }
  else if(numToWrite == 12)
  {
     writeBit(0);
     writeBit(0);
     writeBit(1);
     writeBit(1);
     writeBit(1);
  }
  else if(numToWrite == 13)
  {
     writeBit(1);
     writeBit(0);
     writeBit(1);
     writeBit(1);
     writeBit(0);
  }
  else if(numToWrite == 14)
  {
     writeBit(0);
     writeBit(1);
     writeBit(1);
     writeBit(1);
     writeBit(0);
  }
  else if(numToWrite == 15)
  {
     writeBit(1);
     writeBit(1);
     writeBit(1);
     writeBit(1);
     writeBit(1); 
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
//
// XXX: Get it to compute the checksum properly.
void randomizeNumber(byte* cc)
{
  int first = 8;
  int last = 16; //Not inclusive
  

  for (int i = 7 * (first+2); i < 7 * (last+2); i += 7) {
    int rand = random(10);
    convertDecimal(rand, cc+i);
  }  
  
  /*
  for (int i = 7 * (first+2); i < 7 * (last+2); i += 7) {
    // First 4 bits have to be between 0-9. See: http://en.wikipedia.org/wiki/Six-bit_character_code#DEC_sixbit_code
    // Being safe and only setting the first three so we get 0:7 as opposed to 0:15, which would overflow.
    for (int j = 0; j < 3; j++) {
      int rand = random(0,2);
      cc[i+j] = rand;
      //Serial.print(rand);
    }

    // Set the fourth bit to zero.
    cc[i+3] = 0;
    
    // Next 2 bits need to be 1. See the above link.
    cc[i+4] = 1;
    cc[i+5] = 0;
 
    //Serial.println(parity);
    cc[i+6] = computeParity(cc+i, 6);
  }*/
}

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
    
    /*
    if (i%7 == 0) {
      Serial.println();
    }  
    Serial.print(cc[i]);*/
  }  
  //Serial.println(); 
}
  
static int switchCC = 1;
void loop(){
  // This gets output to the "Serial Monitor" at the top-right button on the screen.
  //Serial.println("hello");
  
  //Turn on LED
  analogWrite(5, 255);
  analogWrite(6, 0);
  
  for(int i = 0; i < 20; i++)
  {
    writeBit(0);
  }
  
  randomizeNumber(myCC);
  
  /*
  if (switchCC) {
     setLRC(myCC, 469);
     writeCC(myCC, 469);
  }
  else {
     setLRC(freedomCC, 469);
     writeCC(freedomCC, 469);
  }
  
  switchCC ^= 1;
  */
  
  byte *cc = switchCC ? myCC : hybridCC;
  //switchCC ^= 1;
  
  setLRC(cc, 469);
  writeCC(cc, 469);
  
  for(int i = 0; i < 20; i++)
  {
    writeBit(0);
  }
  
  // Turn off LED
  analogWrite(5, 0);
  
  writeLow();
  delay(1000);

}
