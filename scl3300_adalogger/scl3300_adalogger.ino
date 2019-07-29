#include <math.h>
#include <SPI.h>
//#include <avr/dtostrf.h>

#include <Wire.h>

// #include <SD.h>
#include "SdFat.h"
SdFat SD;
#define cardSelect 4

// File logfile;

const int AccSelectPin = A5;
SPISettings settingSCA(2000000, MSBFIRST, SPI_MODE0);


void setup() {
  Serial.begin(9600);
  delay(5000);

  // 
  // char incomingByte = Serial.read();
  // while (incomingByte != 'a') {
  //   incomingByte = Serial.read();
  //   delay(100);
  // }
  // // 
  // pinMode(A0,OUTPUT);
  // sabi sa website ng feather lora m0 at 3u4
  // disable spi for radio when not in use
  // pinMode(8,OUTPUT);
  // digitalWrite(8,HIGH);
  // cs for sd card

  // Serial.println("Starting");
  // pinMode(cardSelect,OUTPUT);
  if (!SD.begin(cardSelect)) {
    Serial.println("Card init. failed!");
  }
  // char filename[15];
  // strcpy(filename, "datalog00.txt");
  // for (uint8_t i = 0; i < 100; i++) {
  //   filename[7] = '0' + i/10;
  //   filename[8] = '0' + i%10;
  //   // create if does not exist, do not open existing, write, sync after write
  //   if (! SD.exists(filename)) {
  //     break;
  //   }
  // }

  delay(2000);


  // pinMode(13,OUTPUT);
  SPI.begin();
  pinMode(AccSelectPin, OUTPUT);
  set_mode4();
  enableANG(); // enable angle outputs
}

struct scl_data{
  double ang_x;
  double ang_y;
  double ang_z;
} struct_scl_data;

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

void scl_temp(char* tmpString){
  //  140000EF
  SPI.beginTransaction(settingSCA);
  digitalWrite(AccSelectPin, LOW);
  delay(1);
  SPI.transfer(0x14);
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  SPI.transfer(0xEF);
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
  int result = (( b << 8) | c );
  float temp = -273.0 + ((result*1.0)/18.9);
  dtostrf(temp,6,5,tmpString);
}

void set_mode3(){
  uint32_t mode3 = 0xB4000225;
  SPI.beginTransaction(settingSCA);
  digitalWrite(AccSelectPin, LOW);
  delay(10);
  SPI.transfer(0xB4);
  SPI.transfer(0x00);
  SPI.transfer(0x02);
  SPI.transfer(0x25);
  digitalWrite(AccSelectPin, HIGH);
  SPI.endTransaction();
  delay(1);
}

void set_mode4(){
  uint32_t mode4 = 0xB4000338;
  SPI.beginTransaction(settingSCA);
  digitalWrite(AccSelectPin, LOW);
  delay(10);
  SPI.transfer(0xB4);
  SPI.transfer(0x00);
  SPI.transfer(0x03);
  SPI.transfer(0x38);
  digitalWrite(AccSelectPin, HIGH);
  SPI.endTransaction();
  delay(1);
}

void readX(char* tmpString){
  //  uint32_t Read_ANG_X = (0x240000C7);
  //  char tmpString[7];
  SPI.beginTransaction(settingSCA);
  digitalWrite(AccSelectPin, LOW);
  delay(1);
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
  int result = (( b << 8) | c ); 
  float angle = ((result*1.0) / 16384.0) * 90.0;
  dtostrf(angle,6,5,tmpString);
  // Serial.print(tmpString);
}

void readY(char* tmpString){
  //  uint32_t Read_ANG_Y = (0x280000CD);
  //  char tmpString[7];
  SPI.beginTransaction(settingSCA);
  digitalWrite(AccSelectPin, LOW);
  delay(1);
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
  int result = (( b << 8) | c );
  float angle = ((result*1.0) / 16384.0) * 90.0;
  dtostrf(angle,6,5,tmpString);
  // Serial.print(",");
  // Serial.print(tmpString);
}

void readZ(char* tmpString){
  //  uint32_t Read_ANG_Z = (0x2C0000CB);
  //  char tmpString[7];
  SPI.beginTransaction(settingSCA);
  digitalWrite(AccSelectPin, LOW);
  delay(1);
  SPI.transfer(0x2C);
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
  int result = (( b << 8) | c );
  float angle = ((result*1.0) / 16384.0) * 90.0;
  dtostrf(angle,6,5,tmpString);
  // Serial.print(",");
  // Serial.print(tmpString);
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

void read_accx(char* tmpString){
  uint32_t Read_ACC_X = (0x040000F7);
  SPI.beginTransaction(settingSCA);
  digitalWrite(AccSelectPin, LOW);
  delay(10);
  SPI.transfer(0x04);
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  SPI.transfer(0xF7);
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
  int result = (( b << 8) | c );
  float acc = (result*1.0/6000.0);
  dtostrf(acc,6,5,tmpString);
  Serial.print(tmpString);
}

void read_accy(char* tmpString){
  uint32_t Read_ACC_Y = (0x080000FD);
  SPI.beginTransaction(settingSCA);
  digitalWrite(AccSelectPin, LOW);
  delay(10);
  SPI.transfer(0x08);
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  SPI.transfer(0xFD);
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
  int result = (( b << 8) | c );
  float acc = (result*1.0/6000.0);
  dtostrf(acc,6,5,tmpString);
  Serial.print(",");
  Serial.print(tmpString);
}

void read_accz(char* tmpString){
  uint32_t Read_ACC_Z = (0x0C0000FB);
  SPI.beginTransaction(settingSCA);
  digitalWrite(AccSelectPin, LOW);
  delay(10);
  SPI.transfer(0x0C);
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  SPI.transfer(0xFB);
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
  int result = (( b << 8) | c );
  float acc = (result*1.0/6000.0);
  dtostrf(acc,6,5,tmpString);
  Serial.print(",");
  Serial.print(tmpString);
}

struct scl_data scl_ave_axl(){
  int samples = 50;
  char XtmpString[6];
  char YtmpString[6];
  char ZtmpString[6];
  double X = 0.0;
  double Y = 0.0;
  double Z = 0.0;
  struct scl_data sc;
  // digitalWrite(13,HIGH);
  for(int i = samples; i>0; i-- ){
    readX(XtmpString);
    readY(YtmpString);
    readZ(ZtmpString);
    X = X + atof(XtmpString);
    Y = Y + atof(YtmpString);
    Z = Z + atof(ZtmpString); 
    delay(100); // scl3300 10hz at mode 4 inclination mode
  }
  // digitalWrite(13,LOW);
  sc.ang_x = X / (samples*1.0);
  sc.ang_y = Y / (samples*1.0);
  sc.ang_z = Z / (samples*1.0);
  return sc;
}

void loop(){
  char XtmpString[6];
  char YtmpString[6];
  char ZtmpString[6];

  char temp[6];
  struct scl_data sc = scl_ave_axl();
  File dataFile = SD.open("datalog.txt", FILE_WRITE);

  if (dataFile) {

    
    dtostrf(sc.ang_x,6,5,XtmpString);
    dataFile.print(XtmpString);
    dataFile.print(",");
    dtostrf(sc.ang_y,6,5,YtmpString);
    dataFile.print(YtmpString);
    dataFile.print(",");
    dtostrf(sc.ang_z,6,5,ZtmpString);
    dataFile.print(ZtmpString);
    dataFile.print(",");    
    scl_temp(temp);
    dataFile.println(temp);
    dataFile.flush();    

    dataFile.close();
    // digitalWrite(13,LOW); 
    // print to the serial port too:
  } else {
    Serial.println(".");  
  }
  
  // readX(XtmpString);
  // readY(YtmpString);
  // readZ(ZtmpString);
  // read_accx(XtmpString);
  // read_accy(YtmpString);
  // read_accz(ZtmpString);


}
