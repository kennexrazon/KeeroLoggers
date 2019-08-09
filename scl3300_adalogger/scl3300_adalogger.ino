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

  if (!SD.begin(cardSelect)) {
    Serial.println("Card init. failed!");
  }

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
  double temp;
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

double getTemp(){
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
  double temp = -273.0 + ((result*1.0)/18.9);
  return temp;
}

double getAngX(){
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
  double angle = ((result*1.0) / 16384.0) * 90.0; 
  // Serial.println(angle); 
  return angle;
}

double getAngY(){
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
  double angle = ((result*1.0) / 16384.0) * 90.0;
  return angle;
}

double getAngZ(){
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
  double angle = ((result*1.0) / 16384.0) * 90.0;
  return angle;
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
  int samples = 10;
  double X = 0.0;
  double Y = 0.0;
  double Z = 0.0;
  // double temp = 0.0;
  
  struct scl_data sc;
  for(int i = samples; i>0; i-- ){
    X = X + getAngX();
    Y = Y + getAngY();
    Z = Z + getAngZ(); 
    delay(100); // scl3300 10hz at mode 4 inclination mode
  }
  sc.ang_x = X / (samples*1.0);
  sc.ang_y = Y / (samples*1.0);
  sc.ang_z = Z / (samples*1.0);
  sc.temp = getTemp();
  // Serial.println(sc.ang_x);
  // Serial.println(sc.ang_y);
  // Serial.println(sc.ang_z);
  return sc;
}

struct scl_data scl_temp_comp(struct scl_data sc){

  double x3 = 4.26808;
  double x2 = -0.39185;
  double x1 = 0.012025;
  double x0 = -0.00013;
 
  sc.ang_x =  sc.ang_x - (( x3 + (x2*sc.temp) + (x1*square(sc.temp)) 
    + x0*pow(sc.temp,3)));

  double y3 = 0.74327;
  double y2 = -0.07600;
  double y1 = 0.00236;
  double y0 = -0.00002; 

  sc.ang_y =  sc.ang_y - (( y3 + (y2*sc.temp) + (y1*square(sc.temp)) 
  + y0*pow(sc.temp,3)));

  double z3 = -4.79244;
  double z2 = 0.43502;
  double z1 = -0.01237;
  double z0 = 0.00014;

  sc.ang_z =  sc.ang_z - (( z3 + (z2*sc.temp) + (z1*square(sc.temp)) 
  + z0*pow(sc.temp,3)));

  // return sc;

}

void loop(){
  char XtmpString[6];
  char YtmpString[6];
  char ZtmpString[6];
  char TempString[6];

  // char temp[6];
  struct scl_data scc = scl_ave_axl();
  // struct scl_data scc;
  // scl_temp_comp(scc);
  File dataFile = SD.open("datalog.txt", FILE_WRITE);

  if (dataFile) {

    
    // dtostrf(sc.ang_x,6,5,XtmpString);
    // dataFile.print(XtmpString);
    // dataFile.print(",");
    // dtostrf(sc.ang_y,6,5,YtmpString);
    // dataFile.print(YtmpString);
    // dataFile.print(",");
    // dtostrf(sc.ang_z,6,5,ZtmpString);
    // dataFile.print(ZtmpString);
    // dataFile.print(",");    
    dtostrf(scc.ang_x,6,5,XtmpString);
    dataFile.print(XtmpString);
    dataFile.print(",");
    dtostrf(scc.ang_y,6,5,YtmpString);
    dataFile.print(YtmpString);
    dataFile.print(",");
    dtostrf(scc.ang_z,6,5,ZtmpString);
    dataFile.print(ZtmpString);
    dataFile.print(",");
    dtostrf(scc.temp,6,5,TempString);
    Serial.println(TempString);
    dataFile.println(TempString);
    dataFile.flush();    

    dataFile.close();
    // digitalWrite(13,LOW); 
    // print to the serial port too:
  } else {
    Serial.println(".");  
  }
 
}
