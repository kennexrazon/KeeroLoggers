#include <math.h>
#include <SPI.h>
const int AccSelectPin = A5;

double FullScale = 2074.0;

const byte READCommand_REVID = 0x01;
const byte READCommand_ProductID = 0x9D;
const byte READCommand_CTRL = 0x04;
const byte WRITECommand_CTRL = 0x07;
const byte READCommand_STATUS = 0x08;

const byte READCommand_X_LSB = 0x10;
const byte READCommand_X_MSB = 0x15;

const byte READCommand_Y_LSB = 0x19;
const byte READCommand_Y_MSB = 0x1C;

const byte READCommand_Z_LSB = 0x20;
const byte READCommand_Z_MSB = 0x25;

SPISettings settingSCA(8000000, MSBFIRST, SPI_MODE0);


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(AccSelectPin, OUTPUT);
  // sabi sa website ng feather lora m0
  pinMode(8,OUTPUT);
  digitalWrite(8,HIGH);
  
  SPI.begin();
  delay(1000);
  char incomingByte = Serial.read();
  while (incomingByte != 'a') {
    incomingByte = Serial.read();
    delay(100);
  }
}

void setupACC()
{
  //Read DevID
  SPI.beginTransaction(settingSCA);
  digitalWrite(AccSelectPin, LOW);
  delay(10);
  byte head = SPI.transfer(READCommand_REVID);
  byte result = SPI.transfer(0x00);
  delay(10);
  digitalWrite(AccSelectPin, HIGH);
  SPI.endTransaction();


//  Serial.print("REVID:");
//  Serial.print(head,BIN);
//  Serial.print("\t");
//  Serial.println(result,HEX);

  
//  //read CTRL
//  SPI.beginTransaction(settingSCA);
//  digitalWrite(AccSelectPin, LOW);
//  head = SPI.transfer(WRITECommand_CTRL);
//  result = SPI.transfer(0x00);
//  digitalWrite(AccSelectPin, HIGH);
//  SPI.endTransaction();
}

void loop() {
  // put your main code here, to run repeatedly:
  SPI.beginTransaction(settingSCA);
  digitalWrite(AccSelectPin, LOW);
//  delay(10);
  byte head_M = SPI.transfer(READCommand_X_MSB);
  byte MSB = SPI.transfer(0x00);
//  delay(10);
  digitalWrite(AccSelectPin, HIGH);

  digitalWrite(AccSelectPin, LOW);
//  delay(10);
  byte head_L = SPI.transfer(READCommand_X_LSB);
  byte LSB = SPI.transfer(0x00);
//  delay(10);
  digitalWrite(AccSelectPin, HIGH);

  short int finalX = ((MSB << 8) | LSB);
  finalX = finalX >> 2;
  
  digitalWrite(AccSelectPin, LOW);
//  delay(10);
  head_M = SPI.transfer(READCommand_Y_MSB);
  MSB = SPI.transfer(0x00);
//  delay(10);
  digitalWrite(AccSelectPin, HIGH);

  digitalWrite(AccSelectPin, LOW);
//  delay(10);
  head_L = SPI.transfer(READCommand_Y_LSB);
  LSB = SPI.transfer(0x00);
//  delay(10);
  digitalWrite(AccSelectPin, HIGH);
  
  short int finalY= ((MSB << 8) | LSB);
  finalY = finalY >> 2;
  
  digitalWrite(AccSelectPin, LOW);
  head_M = SPI.transfer(READCommand_Z_MSB);
  MSB = SPI.transfer(0x00);
  digitalWrite(AccSelectPin, HIGH);

  digitalWrite(AccSelectPin, LOW);
  head_L = SPI.transfer(READCommand_Z_LSB);
  LSB = SPI.transfer(0x00);
  digitalWrite(AccSelectPin, HIGH);
  SPI.endTransaction();
  
  short int finalZ= ((MSB << 8) | LSB);
  finalZ = finalZ >> 2;

//  double angel = atan2(finalY, finalZ)*180/PI;

  /*
  double gRateForOtherSide = gRate * gRate;
  gRateForOtherSide = 1-gRateForOtherSide;
  gRateForOtherSide = sqrt(gRateForOtherSide);
  double tang = gRateForOtherSide/gRate;
  */
//  Serial.print("finalX:");
  //Serial.print(finalX,BIN);
  //Serial.print("\t");
  
  Serial.print(finalX);
  Serial.print(",");

//  Serial.print("finalY:");
  Serial.print(finalY);
  Serial.print(",");

//  Serial.print("finalZ:");
  Serial.println(finalZ);
//  Serial.print("\t");

//  Serial.print("Angel:");
//  Serial.println(angel);
  
  

  
  delay(10);
}
