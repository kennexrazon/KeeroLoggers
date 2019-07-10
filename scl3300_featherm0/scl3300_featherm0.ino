#include <math.h>
#include <SPI.h>
const int AccSelectPin = A5;

uint32_t Read_ACC_X = 0x040000F7;
uint32_t Read_ACC_Y = 0x080000FD;
uint32_t Read_ACC_Z = 0x0C0000FB;


//const unsigned long Read_ANG_X = 0x240000C7;
//const unsigned long Read_ANG_Y = 0x280000CD;
//const unsigned long Read_ANG_Z = 0x280000CB;
//const unsigned long Read_TEMP = 0x140000EF;
//const unsigned long Enable_ANG = 0xB0001F6F;


SPISettings settingSCA(4000000, MSBFIRST, SPI_MODE0);


void setup() {
  Serial.begin(115200);

  pinMode(AccSelectPin, OUTPUT);
  // sabi sa website ng feather lora m0 at 3u4
  // disable spi for radio when not in use
  pinMode(8,OUTPUT);
  digitalWrite(8,HIGH);

  
  SPI.begin();
  delay(1000);
  
  char incomingByte = Serial.read();
  while (incomingByte != 'a') {
    incomingByte = Serial.read();
    delay(100);
  }
//  read_status();
//  reset();
//  read_status();
  enableANG();
}

void read_status(){
  uint32_t READ_STATUS = 0x180000E5;
  uint32_t dummy = 0x00000000;
  
  SPI.beginTransaction(settingSCA);
  digitalWrite(AccSelectPin, LOW);
  delay(10);
  SPI.transfer(&READ_STATUS,4);
  digitalWrite(AccSelectPin, HIGH);
  Serial.print("1st Status Read ::");
  Serial.println(READ_STATUS,HEX);
  SPI.endTransaction();
  
  delay(10);
   
  SPI.beginTransaction(settingSCA);
  digitalWrite(AccSelectPin, LOW);
  READ_STATUS = 0x180000E5;
  SPI.transfer(&READ_STATUS,4);
  
  Serial.print("2nd Status Read ::"); 
  Serial.println(READ_STATUS,HEX); 
  digitalWrite(AccSelectPin, HIGH);
  SPI.endTransaction();
}

void reset(){
  
  uint32_t RESET = 0xB4002098;
  SPI.beginTransaction(settingSCA);
  digitalWrite(AccSelectPin, LOW);
  delay(10);
  SPI.transfer(&RESET,4);
  
  digitalWrite(AccSelectPin, HIGH);
  SPI.endTransaction();
  
}

void who(){
  uint32_t WHOAMI = 0x40000091;
  SPI.beginTransaction(settingSCA);
  digitalWrite(AccSelectPin, LOW);
  delay(10);
  SPI.transfer(0x40);
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  SPI.transfer(0x91);
  digitalWrite(AccSelectPin, HIGH);
  SPI.endTransaction();
  delay(1);
  SPI.beginTransaction(settingSCA);
  digitalWrite(AccSelectPin, LOW);
  byte a = SPI.transfer(0x00);
  byte b = SPI.transfer(0x00);
  byte c = SPI.transfer(0x00);
  byte d = SPI.transfer(0x00);
  digitalWrite(AccSelectPin, HIGH);
  SPI.endTransaction();
  Serial.print(a,HEX);
  Serial.print(b,HEX);
  Serial.print(c,HEX);
  Serial.println(d,HEX);
}

double readX(){
  uint32_t Read_ANG_X = (0x240000C7);
  SPI.beginTransaction(settingSCA);
  digitalWrite(AccSelectPin, LOW);
  delay(10);
  SPI.transfer(0x24);
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  SPI.transfer(0xC7);
  digitalWrite(AccSelectPin, HIGH);
  SPI.endTransaction();
  delay(1);
  SPI.beginTransaction(settingSCA);
  digitalWrite(AccSelectPin, LOW);
  byte a = SPI.transfer(0x00);
  byte b = SPI.transfer(0x00);
  byte c = SPI.transfer(0x00);
  byte d = SPI.transfer(0x00);
  digitalWrite(AccSelectPin, HIGH);
  SPI.endTransaction();
  int16_t result = (( b << 8) | c );
  double angle = ((result*1.0) / 16384.0) * 90.0;
  Serial.print("X:"); Serial.print(angle);
  return angle;
}
double readY(){
  uint32_t Read_ANG_Y = (0x280000CD);
  SPI.beginTransaction(settingSCA);
  digitalWrite(AccSelectPin, LOW);
  delay(10);
  SPI.transfer(0x28);
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  SPI.transfer(0xCD);
  digitalWrite(AccSelectPin, HIGH);
  SPI.endTransaction();
  delay(1);
  SPI.beginTransaction(settingSCA);
  digitalWrite(AccSelectPin, LOW);
  byte a = SPI.transfer(0x00);
  byte b = SPI.transfer(0x00);
  byte c = SPI.transfer(0x00);
  byte d = SPI.transfer(0x00);
  digitalWrite(AccSelectPin, HIGH);
  SPI.endTransaction();
  int16_t result = (( b << 8) | c );
  double angle = ((result*1.0) / 16384.0) * 90.0;
  Serial.print("\tY:"); Serial.print(angle);
  return angle;
}

double readZ(){
  uint32_t Read_ANG_Z = (0x280000CB);
  SPI.beginTransaction(settingSCA);
  digitalWrite(AccSelectPin, LOW);
  delay(10);
  SPI.transfer(0x28);
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  SPI.transfer(0xCB);
  digitalWrite(AccSelectPin, HIGH);
  SPI.endTransaction();
  delay(1);
  SPI.beginTransaction(settingSCA);
  digitalWrite(AccSelectPin, LOW);
  byte a = SPI.transfer(0x00);
  byte b = SPI.transfer(0x00);
  byte c = SPI.transfer(0x00);
  byte d = SPI.transfer(0x00);
  digitalWrite(AccSelectPin, HIGH);
  SPI.endTransaction();
  int16_t result = (( b << 8) | c );
  double angle = ((result*1.0) / 16384.0) * 90.0;
  Serial.print("\tZ:"); Serial.println(angle);
  return angle;
}
  
void enableANG(){
  uint32_t Enable_ANG = 0xB0001F6F;
  SPI.beginTransaction(settingSCA);
  digitalWrite(AccSelectPin, LOW);
  delay(10);
  SPI.transfer(0xB0);
  SPI.transfer(0x00);
  SPI.transfer(0x1F);
  SPI.transfer(0x6F);
  digitalWrite(AccSelectPin, HIGH);
  SPI.endTransaction();
  delay(1);
}


void loop(){
readX();
readY();
readZ();
delay(1000);
}
