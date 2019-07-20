#include <math.h>
#include <SPI.h>
const int AccSelectPin = A5;

const byte Read_ANG_X = 0x240000C7;
const byte Read_ANG_Y = 0x280000CD;
const byte Read_ANG_Z = 0x280000CB;
const byte Read_TEMP = 0x140000EF;
const byte Enable_ANG = 0xB0001F6F;

const byte Read_ACC_X = 0x040000F7;

SPISettings settingSCA(4000000, MSBFIRST, SPI_MODE0);


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(AccSelectPin, OUTPUT);
  // sabi sa website ng feather lora m0 at 3u4
  pinMode(8,OUTPUT);
  digitalWrite(8,HIGH);
  enableANG();
  SPI.begin();
  delay(1000);
  char incomingByte = Serial.read();
  while (incomingByte != 'a') {
    incomingByte = Serial.read();
    delay(100);
  }
  byte ax = Read_ANG_X << 4;
//  byte ax = Read_ANG_X;
  Serial.println(Read_ANG_X,HEX);
  Serial.println(ax,HEX);
}

union FourByte{
   struct {
     unsigned long value:32; //24bit register values go in here
     byte command:8; //8bit command goes in here.
   };
   byte bit8[4]; //this is just used for efficient conversion of the above into 4 bytes.
};

unsigned long sendCMD(byte CMD){
  union FourByte data = {CMD};
  for(int i = 0; i<4; i++){
    int shift = i*8;
    byte a = CMD << shift;
    Serial.print(a,HEX);
    data.bit8[i] = SPI.transfer(a);
  }
  return data.value;
}

//union FourByte data

void enableANG(){
  union FourByte data;
  SPI.beginTransaction(settingSCA);
  digitalWrite(AccSelectPin, LOW);
  delay(5);
  data.bit8[0] = SPI.transfer(0xB0);
  data.bit8[1] = SPI.transfer(0x00);
  data.bit8[2] = SPI.transfer(0x1F);
  data.bit8[3] = SPI.transfer(0x6F);
//  sendCMD(Enable_ANG);
//  byte a = SPI.transfer(Enable_ANG);
  digitalWrite(AccSelectPin, HIGH);
  SPI.endTransaction();
}
/*
void readACC(){
  SPI.beginTransaction(settingSCA);
  digitalWrite(AccSelectPin, LOW);
  delay(5);
  byte a = SPI.transfer(Read_ACC_X);
  byte result = SPI.transfer(0x00);
  digitalWrite(AccSelectPin, HIGH);
  SPI.endTransaction();
  Serial.print("result=");
  Serial.println(result,HEX);
}
*/
unsigned long readX(){
  union FourByte data;
  SPI.beginTransaction(settingSCA);
  digitalWrite(AccSelectPin, LOW);
  delay(5);
//  unsigned long b = sendCMD(Read_ANG_X);  
//240000C7
  data.bit8[0] = SPI.transfer(0x24);
  data.bit8[1] = SPI.transfer(0x00);
  data.bit8[2] = SPI.transfer(0x00);
  data.bit8[3] = SPI.transfer(0xC7);
  digitalWrite(AccSelectPin, HIGH);
  SPI.endTransaction();
  return data.value;
//  Serial.print("result=");
//  Serial.println(b,HEX);
}
void loop() {
  unsigned long a = readX();
  Serial.println(a,HEX);
  delay(100);
//  // put your main code here, to run repeatedly:
//  SPI.beginTransaction(settingSCA);
//  digitalWrite(AccSelectPin, LOW);
////  delay(10);
//  byte head_M = SPI.transfer(READCommand_X_MSB);
//  byte MSB = SPI.transfer(0x00);
////  delay(10);
//  digitalWrite(AccSelectPin, HIGH);
//
//  digitalWrite(AccSelectPin, LOW);
////  delay(10);
//  byte head_L = SPI.transfer(READCommand_X_LSB);
//  byte LSB = SPI.transfer(0x00);
////  delay(10);
//  digitalWrite(AccSelectPin, HIGH);
//
//  short int finalX = ((MSB << 8) | LSB);
//  finalX = finalX >> 2;
//  
//  digitalWrite(AccSelectPin, LOW);
////  delay(10);
//  head_M = SPI.transfer(READCommand_Y_MSB);
//  MSB = SPI.transfer(0x00);
////  delay(10);
//  digitalWrite(AccSelectPin, HIGH);
//
//  digitalWrite(AccSelectPin, LOW);
////  delay(10);
//  head_L = SPI.transfer(READCommand_Y_LSB);
//  LSB = SPI.transfer(0x00);
////  delay(10);
//  digitalWrite(AccSelectPin, HIGH);
//  
//  short int finalY= ((MSB << 8) | LSB);
//  finalY = finalY >> 2;
//  
//  digitalWrite(AccSelectPin, LOW);
//  head_M = SPI.transfer(READCommand_Z_MSB);
//  MSB = SPI.transfer(0x00);
//  digitalWrite(AccSelectPin, HIGH);
//
//  digitalWrite(AccSelectPin, LOW);
//  head_L = SPI.transfer(READCommand_Z_LSB);
//  LSB = SPI.transfer(0x00);
//  digitalWrite(AccSelectPin, HIGH);
//  SPI.endTransaction();
//  
//  short int finalZ= ((MSB << 8) | LSB);
//  finalZ = finalZ >> 2;
//
////  double angel = atan2(finalY, finalZ)*180/PI;
//
//  /*
//  double gRateForOtherSide = gRate * gRate;
//  gRateForOtherSide = 1-gRateForOtherSide;
//  gRateForOtherSide = sqrt(gRateForOtherSide);
//  double tang = gRateForOtherSide/gRate;
//  */
////  Serial.print("finalX:");
//  //Serial.print(finalX,BIN);
//  //Serial.print("\t");
//  
//  Serial.print(finalX);
//  Serial.print(",");
//
////  Serial.print("finalY:");
//  Serial.print(finalY);
//  Serial.print(",");
//
////  Serial.print("finalZ:");
//  Serial.println(finalZ);
////  Serial.print("\t");
//
////  Serial.print("Angel:");
////  Serial.println(angel);
//  
//  
//
//  
  delay(10);
}
