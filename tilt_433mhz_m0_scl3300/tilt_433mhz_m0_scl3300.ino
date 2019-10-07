#include <math.h>
#include <SPI.h>
#include <RH_RF95.h>
#include <Wire.h>
// for m0
#include <avr/dtostrf.h>


/* for 32u4
#define VBATPIN A9
#define RFM95_INT 7
*/


const int AccSelectPin = A5;
SPISettings settingSCA(2000000, MSBFIRST, SPI_MODE0);

// for m0
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3


#define RF95_FREQ 433.0


#define SENSEID "45"
#define AREA "APO"
// MSL - 19 - 21 
// SMR - 22 - 24
// MUR - 25 - 27
// MGA - 28 - 29
// RDO - 30 - 32
// NIGBU - NGR 868mhz
// NJT - 33 - 35 
// MLY - 36 - 38
// NTE - 39 - 41
// SGD - 42 - 44
// APO - APO 433mhz
// PCO - 45 - 47
// LTC - 48 - 51
#define SITE "PCO"
#define terminator "$"

#define VBATPIN A7
#define somsPin A3
#define SEND_RETRY_LIMIT 3



RH_RF95 rf95(RFM95_CS, RFM95_INT);

struct scl_data{
  double ang_x;
  double ang_y;
  double ang_z;
  // double temp;
} struct_scl_data;

struct lgr_data{
  float vol;
  float cur;
  float soms1;
  float axelTemp;
} struct_lgr_data;

void setup() {
  Serial.begin(9600);
  delay(2000);
  Serial.print(AREA);Serial.print("-");
  Serial.print(SITE); Serial.print("-");
  Serial.println(SENSEID);

  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }

  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
  
  rf95.setTxPower(23, false);

  Serial.println("done initializing......\n");

  SPI.begin();
  pinMode(AccSelectPin, OUTPUT);
  // set_mode4();
  // enableANG(); // enable angle outputs
 
  randomSeed(analogRead(1));
  delay(random(3000,4000));
  blinkled();
  Serial.println("done setup");
}

void loop(){

  struct scl_data sc = scl_ave_axl();
  struct lgr_data lgr = get_data_lgr();
  // sc = scl_temp_comp(sc);

  char line1[50] = AREA;//axl gravity
  char line3[57] = AREA;//ina power + soms

  //build/parse the line packets

  buildLineAXL(line1,sc);
  buildLineSMS(line3,lgr);

  //transmit data
  sendLine(line1,50,1);
  // sendLine(line2,50,2);
  sendLine(line3,57,3);
  
  Serial.println("#################################");
  pinMode(A0,OUTPUT);
  // digitalWrite(A0,LOW);
  // delay(1000);
  digitalWrite(A0,HIGH);
  delay(1000);
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

double get_accx(){
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
  int16_t result = (( b << 8) | c );
  double acc = ((result*1.0)/6000.0);
  return acc;
}

double get_accy(){
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
  int16_t result = (( b << 8) | c );
  double acc = (result*1.0)/6000.0;
  return acc;
}

double get_accz(){
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
  int16_t result = (( b << 8) | c );
  double acc = (result*1.0)/6000.0;
  return acc;
}

struct scl_data scl_ave_axl(){
  int samples = 1;
  double X = 0.0;
  double Y = 0.0;
  double Z = 0.0;
  // double temp = 0.0;
  
  struct scl_data sc;
  // for(int i = samples; i>0; i-- ){
  //   X = X + get_accx();
  //   Y = Y + get_accy();
  //   Z = Z + get_accz(); 
  //   delay(100); // scl3300 10hz at mode 4 inclination mode
  // }
  // sc.ang_x = X / (samples*1.0);
  // sc.ang_y = Y / (samples*1.0);
  // sc.ang_z = Z / (samples*1.0);
  // // sc.temp = getTemp();

  sc.ang_x = get_accx();
  sc.ang_y = get_accy();
  sc.ang_z = get_accz();
  // Serial.println(sc.ang_x);
  // Serial.println(sc.ang_y);
  // Serial.println(sc.ang_z);
  return sc;
}

float get_soms_VWC(int pin) {
 float somsADC = 0.0;
 float soms_avg_vwc = 0.0;
 float somsVWC = 0.0;
 int samples = 10;
 
 somsADC = (analogRead(pin) / 1023.0)*3.30;

 somsVWC = (2.97*somsADC*somsADC*somsADC) -
 (7.37*somsADC*somsADC) + (6.69*somsADC) - 1.92;  

 for(int i = 0; i<10; i++){
   soms_avg_vwc = soms_avg_vwc + somsVWC;
 }

 return (soms_avg_vwc/10.0);

}

struct lgr_data get_data_lgr(){
  struct lgr_data dt;

  dt.vol = get_bat_vol(); 
  dt.cur = get_cur();
  dt.soms1 = get_soms_VWC(somsPin);
  dt.axelTemp = getTemp();

  return dt;
}

float get_cur(){
  return 0.0;
}

float get_bat_vol(){

  float measuredvbat = analogRead(VBATPIN);
  measuredvbat *= 2;    // we divided by 2, so multiply back
  measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
  measuredvbat /= 1024; // convert to voltage
  Serial.print("VBat: " ); Serial.println(measuredvbat);

  return measuredvbat;
}
/*
void scl_xyz(){
  char X[6];
  char Y[6];
  char Z[6];

  readX(X);
  readY(Y);
  readZ(Z);

  delay(100);
}
*/
void buildID(char* line,char* sensor){
  strcat(line,"-");
  strcat(line,SITE);
  strcat(line,"-");
  strcat(line,sensor);
  strcat(line,SENSEID);
  strcat(line,terminator);
}

void sendLine(char* line,int inLen,int blinks){
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t  len = sizeof(buf);
  int retry = 0;
  do{
    randomSeed(analogRead(1));
    int del = random(1000,2000);
    Serial.print("..........................................");
    Serial.println(del);
    for(int i=0;i<blinks;i++)blinkled();
    
    Serial.print("Sending "); Serial.println(line);
    Serial.println("Sending...");
    delay(10);
    rf95.send((uint8_t *)line, inLen);
  
    Serial.println("Waiting for packet to complete..."); 
    delay(10);
    rf95.waitPacketSent();
  
    // Now wait for a reply
    Serial.println("Waiting for reply...");
    if (rf95.waitAvailableTimeout(del))
    { 
      // Should be a reply message for us now   
      if (rf95.recv(buf, &len))
     {
        Serial.print("Got reply: ");
        Serial.println((char*)buf);
        Serial.print("RSSI: ");
        Serial.println(rf95.lastRssi(), DEC);    
      }
      else
      {
        Serial.println("Receive failed");
      }
    }
    else
    {
      retry++;
      Serial.print("No reply, is there a listener around?");
      Serial.print("\t retry: ");
      Serial.println(retry);
      
    }
  }while(strcmp((char*)buf,"ACK") && (retry < SEND_RETRY_LIMIT));
}

void buildLineAXL(char* line, struct scl_data dt){
  char sensorType[4] = "AXL";
  char tmpString[5];
  buildID(line,sensorType);

  assignNull(tmpString);

  dtostrf(dt.ang_x,5,4,tmpString);
  strcat(line,tmpString);
  strcat(line,",");

  dtostrf(dt.ang_y,5,4,tmpString);
  strcat(line,tmpString);
  strcat(line,",");

  dtostrf(dt.ang_z,5,4,tmpString);
  strcat(line,tmpString);
  
  Serial.println(line);
}

void buildLineSMS(char* line, struct lgr_data dt){
  char sensorType[4] = "SMS";
  char tmpString[5];
  buildID(line,"SMS");

  assignNull(tmpString);
  // insert "BTV:" - battery voltage here
  strcat(line,"BTV:");
  dtostrf(dt.vol,5,4,tmpString);
  strcat(line,tmpString);
  strcat(line,",");
  // insert "BTA:" - Battery amperage ? Current
  strcat(line,"BTA:");
  dtostrf(dt.cur,5,4,tmpString);
  strcat(line,tmpString);
  strcat(line,",");
  // insert "VWC:"  - volumetric water content
  strcat(line,"VWC:");
  dtostrf(dt.soms1,5,4,tmpString);
  strncat(line,tmpString,5);
  strcat(line,",");
  // insert "ACT:" - Accel Temperature
  strcat(line,"ACT:");
  dtostrf(dt.axelTemp,5,4,tmpString);
  strncat(line,tmpString,5);
  // strcat(line,",");


 
  Serial.println(line);
}

void blinkled(){
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(100);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(100);                       // wait for a second
}

void assignNull(char* txt){
  for(int i=0;i<strlen(txt);i++){
    txt[i] = '\0'; 
  }
}
