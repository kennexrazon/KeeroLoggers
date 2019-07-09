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


SPISettings settingSCA(8000000, MSBFIRST, SPI_MODE0);


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
  read_status();
//  enableANG();
}

void read_status(){
  uint32_t READ_STATUS = 0x180000E5;
  uint32_t dummy = 0x00000000;

  SPI.beginTransaction(settingSCA);
  digitalWrite(AccSelectPin, LOW);
  SPI.transfer(&READ_STATUS,4); // first response shall be discarded
  digitalWrite(AccSelectPin, HIGH);
  SPI.endTransaction();
  delay(10);
  
  SPI.beginTransaction(settingSCA);
  digitalWrite(AccSelectPin, LOW);
  SPI.transfer(&dummy,4); // first response shall be discarded
  digitalWrite(AccSelectPin, HIGH);
  SPI.endTransaction();
  delay(10);

  Serial.println(dummy,HEX);
}

void who(){
  uint32_t WHOAMI = 0x40000091;
  uint32_t dummy = 0x00000000;

  SPI.beginTransaction(settingSCA);
  digitalWrite(AccSelectPin, LOW);
  SPI.transfer(&WHOAMI,4); // first response shall be discarded
  digitalWrite(AccSelectPin, HIGH);
  SPI.endTransaction();
  delay(10);

  SPI.beginTransaction(settingSCA);
  digitalWrite(AccSelectPin, LOW);
  SPI.transfer(&dummy,4); // first response shall be discarded
  digitalWrite(AccSelectPin, HIGH);
  SPI.endTransaction();
  delay(1);
  Serial.println(dummy,HEX);
}

void enableANG(){
  uint32_t Enable_ANG = 0xB0001F6F;
  SPI.beginTransaction(settingSCA);
  digitalWrite(AccSelectPin, LOW);
  delay(5);
  SPI.transfer(&Enable_ANG,4);
  digitalWrite(AccSelectPin, HIGH);
  SPI.endTransaction();
}

void read_angX() {
  uint32_t Read_ANG_X = (0x240000C7);
  uint32_t Read_ANG_Y = (0x280000CD);
  uint32_t Read_ANG_Z = (0x280000CB);
  uint32_t dummy = (0x00000000);
  
  
  SPI.beginTransaction(settingSCA);
  digitalWrite(AccSelectPin, LOW);
  SPI.transfer(&Read_ANG_X,sizeof(Read_ANG_X)); // first response shall be discarded
  digitalWrite(AccSelectPin, HIGH);
//  SPI.endTransaction();
  
  delay(10);
  
//  SPI.beginTransaction(settingSCA);
  digitalWrite(AccSelectPin, LOW);
  SPI.transfer(&Read_ANG_Y,sizeof(Read_ANG_Y)); // first response shall be discarded
  digitalWrite(AccSelectPin, HIGH);
//  SPI.endTransaction();
  
  delay(10);
  
//  SPI.beginTransaction(settingSCA);
  digitalWrite(AccSelectPin, LOW);
  SPI.transfer(&Read_ANG_Z,sizeof(Read_ANG_Z)); // first response shall be discarded
  digitalWrite(AccSelectPin, HIGH);
  SPI.endTransaction();

//  ; // first response shall be discarded
//  long X = ((Read_ACC_Y & 0x00111100) >> 8 );
//  long Y = ((Read_ACC_Z & 0x00111100) >> 8 );
  
  Serial.print("X::");
  Serial.print(Read_ANG_Y,HEX);
  Serial.print("\tY::");
  Serial.println(Read_ANG_Z,HEX);
  
  delay(10);
}

void loop(){
//  read_angX();
//who();
delay(1000);
}

