//#include <avr/dtostrf.h>

//lib required #includes
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <Wire.h>
#include <SPI.h>
//#include <RH_RF95.h>

// for feather32u4 
//#define RFM95_CS 8
//#define RFM95_RST 4
//#define RFM95_INT 7
//#define RF95_FREQ 915.0

#define SENSEID "18"
#define AREA "BCM"
// PLI - 10 - 12 Palayan Interface
// PRS - 13 - 15
// LRD - 16 - 18 Loop Road
#define SITE "LRD"
#define terminator "$"

#define VBATPIN A9
#define somsPin A3

#define SEND_RETRY_LIMIT 3

//lib required #define and global variable declarations
#define BNO055_SAMPLERATE_DELAY_MS (100)

Adafruit_BNO055 bno = Adafruit_BNO055();

int AXLadr = 0x28;

//RH_RF95 rf95(RFM95_CS, RFM95_INT); // Singleton instance of the radio driver

int16_t packetnum = 0;  // packet counter, we increment per xmission

void setup(void){
  Serial.begin(115200);
  delay(2000);

      
  if(!bno.begin())
  {
    Serial.print("No BNO055 detected ... Check your wiring or I2C ADDR!");
  }
  
  delay(1000);

  pinMode(somsPin,INPUT);
  
  bno.setExtCrystalUse(true); 
  delay(100);

  Serial.println("Feather LoRa TX Test!");

  blinkled();
  Serial.println("done setup");
}

struct imu_data{
  float axl_x;
  float axl_y;
  float axl_z;
  float mgr_x;
  float mgr_y;
  float mgr_z;
  float temp;
} struct_imu_data;

struct lgr_data{
  float vol;
  float cur;
  float soms1;
  float soms2;
  float soms3;
} struct_lgr_data;

void loop(void)
{
  struct imu_data imu = get_data_imu();
  char tmpStringX[5];
  char tmpStringY[5];
  char tmpStringZ[5];
  dtostrf(imu.axl_x,5,4,tmpStringX);
  dtostrf(imu.axl_y,5,4,tmpStringY);
  dtostrf(imu.axl_z,5,4,tmpStringZ);
  Serial.print(tmpStringX);Serial.print(",");
  Serial.print(tmpStringY);Serial.print(",");
  Serial.println(tmpStringZ);
//  delay(1000);
}

struct imu_data get_data_imu(){

  struct imu_data dt;

  imu::Vector<3> grvty = bno.getVector(Adafruit_BNO055::VECTOR_GRAVITY);
  imu::Vector<3> mgntr = bno.getVector(Adafruit_BNO055::VECTOR_MAGNETOMETER);

  dt.axl_x = grvty.x()/9.8;
  dt.axl_y = grvty.y()/9.8;
  dt.axl_z = grvty.z()/9.8;

  dt.mgr_x = mgntr.x()/9.8;
  dt.mgr_y = mgntr.y()/9.8;
  dt.mgr_z = mgntr.z()/9.8;

  return dt;
}

struct lgr_data get_data_lgr(){
  struct lgr_data dt;

  dt.vol = get_bat_vol(); 
  dt.cur = get_cur();
  dt.soms1 = get_soms_VWC(somsPin);

  return dt;
}

float get_bat_vol(){

  float measuredvbat = analogRead(VBATPIN);
  measuredvbat *= 2;    // we divided by 2, so multiply back
  measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
  measuredvbat /= 1024; // convert to voltage
  Serial.print("VBat: " ); Serial.println(measuredvbat);

  return measuredvbat;
}

//calibration equation from reference paper
// paper also averaged soms readings ( 10 samples )
float get_soms_VWC(int pin) {
  float somsADC = 0.0;
  float soms_avg_vwc = 0.0;
  float somsVWC = 0.0;
  
  for (int i=0;i <= 9 ;i++) {
    somsADC = analogRead(pin);
    somsVWC = (0.004*somsADC) - 0.4839;
    soms_avg_vwc = soms_avg_vwc + somsVWC; 
    delay(10);
  }

  soms_avg_vwc = (soms_avg_vwc / 10.0);

  return soms_avg_vwc;
}

float get_cur(){
  return 0.0;
}

void buildID(char* line,char* sensor){
  strcat(line,"-");
  strcat(line,SITE);
  strcat(line,"-");
  strcat(line,sensor);
  strcat(line,SENSEID);
  strcat(line,terminator);
}
/*
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
*/

void buildLineAXL(char* line, struct imu_data dt){
  char sensorType[4] = "AXL";
  char tmpString[5];
  buildID(line,sensorType);

  assignNull(tmpString);

  dtostrf(dt.axl_x,5,4,tmpString);
  strcat(line,tmpString);
  strcat(line,",");

  dtostrf(dt.axl_y,5,4,tmpString);
  strcat(line,tmpString);
  strcat(line,",");

  dtostrf(dt.axl_z,5,4,tmpString);
  strcat(line,tmpString);
  
  Serial.println(line);
}

void buildLineMGR(char* line, struct imu_data dt){
  char sensorType[4] = "MGR";
  char tmpString[5];
  buildID(line,sensorType);

  assignNull(tmpString);

  dtostrf(dt.mgr_x,5,4,tmpString);
  strcat(line,tmpString);
  strcat(line,",");

  dtostrf(dt.mgr_y,5,4,tmpString);
  strcat(line,tmpString);
  strcat(line,",");

  dtostrf(dt.mgr_z,5,4,tmpString);
  strcat(line,tmpString);
  
  Serial.println(line); 
}

void buildLineSMS(char* line, struct lgr_data dt){
  char sensorType[4] = "SMS";
  char tmpString[5];
  buildID(line,"SMS");

  assignNull(tmpString);
  
  dtostrf(dt.vol,5,4,tmpString);
  strcat(line,tmpString);
  strcat(line,",");
  
  dtostrf(dt.cur,5,4,tmpString);
  strcat(line,tmpString);
  strcat(line,",");

  dtostrf(dt.soms1,5,4,tmpString);
  strncat(line,tmpString,5);

  Serial.println(line);
}

void buildLineOth(char* line, float vx, float vy, float vz){
  char fltString[4];assignNull(fltString);
  
  //line parsing
  // strcat(line,sensorid);
  // strcat(line,";");
  buildID(line,"SMS");
  
  strcat(line,"vol:");
  dtostrf(vx,6,3,fltString);
  strcat(line,fltString);
  strcat(line,";");

  strcat(line,"cur:");
  dtostrf(vy,6,3,fltString);
  strcat(line,fltString);
  strcat(line,",");
  
  strcat(line,"som:");
  dtostrf(vz,5,4,fltString);
  strcat(line,fltString);
  
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
