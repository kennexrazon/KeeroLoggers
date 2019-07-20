#include <math.h>
#include <SPI.h>
#include <avr/dtostrf.h>
const int AccSelectPin = A5;

SPISettings settingSCA(4000000, MSBFIRST, SPI_MODE0);


void setup() {
  Serial.begin(115200);

  pinMode(AccSelectPin, OUTPUT);
  pinMode(A0,OUTPUT);
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
//delay(5000);
  enableANG(); // enable angle outputs
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

float readX(){
  uint32_t Read_ANG_X = (0x240000C7);
  char tmpString[5];
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
  float angle = ((result*1.0) / 16384.0) * 90.0;
  dtostrf(angle,5,4,tmpString);
//  Serial.print("X:"); 
  Serial.print(tmpString);
  return angle;
}
float readY(){
  uint32_t Read_ANG_Y = (0x280000CD);
  char tmpString[5];
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
  float angle = ((result*1.0) / 16384.0) * 90.0;
  dtostrf(angle,5,4,tmpString);
  Serial.print("\tY:"); 
  Serial.print(",");
  Serial.print(tmpString);
  return angle;
}

float readZ(){
  uint32_t Read_ANG_Z = (0x280000CB);
  char tmpString[5];
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
  float angle = ((result*1.0) / 16384.0) * 90.0;
  dtostrf(angle,5,4,tmpString);
//  Serial.print("\tZ:"); 
  Serial.print(",");
  Serial.println(tmpString);
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
//digitalWrite(A0,HIGH);
}
