//#include <avr/dtostrf.h>

//lib required #includes
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

#include <Wire.h>
//#include <Adafruit_INA219.h>

#include <SPI.h>
#include <RH_RF95.h>

// for feather32u4 
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 7
#define RF95_FREQ 915.0

#define sensorid "epalbrd"
//#define sensorid "TSTPOS1"

#define VBATPIN A9

//lib required #define and global variable declarations
#define BNO055_SAMPLERATE_DELAY_MS (100)

/*
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3
#define RF95_FREQ 433.0
*/

Adafruit_BNO055 bno = Adafruit_BNO055();

int AXLadr = 0x28;

//Adafruit_INA219 ina219;

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

//
#define SEND_RETRY_LIMIT 3
void setup(void)
{
  Serial.begin(9600);
  delay(2000);
  Serial.println(sensorid);Serial.println();
    
  Serial.println("Orientation Sensor Raw Data Test"); Serial.println("");
  
  if(!bno.begin())
  {
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
//    while(1);
// do something to inform the devs that bn0o5 is out.
  }
  
  delay(1000);
  
  int8_t temp = bno.getTemp();
  Serial.print("Current Temperature: ");
  Serial.print(temp);
  Serial.println(" C");
  Serial.println("");

  bno.setExtCrystalUse(true);

  Serial.println("Calibration status values: 0=uncalibrated, 3=fully calibrated");
  
  //Start rx
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  delay(100);

  Serial.println("Feather LoRa TX Test!");

  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }
  Serial.println("LoRa radio init OK!");

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
//    while (1);
// do something to inform devs
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
  
  rf95.setTxPower(23, false);

  Serial.println("done initializing......\n");
  randomSeed(analogRead(1));
  delay(random(3000,4000));
  blinkled();
  Serial.println("done setup");
}

int16_t packetnum = 0;  // packet counter, we increment per xmission

void loop(void)
{
  Serial.println("begin loop");
  
  imu::Vector<3> grvty = bno.getVector(Adafruit_BNO055::VECTOR_GRAVITY);
  imu::Vector<3> mgntr = bno.getVector(Adafruit_BNO055::VECTOR_MAGNETOMETER);
  Serial.println("done init axl variables");
  
  //Read data from sensors: [1] axl (g's), [2] axl (m's), [3] power, [4] soms
  char axlX[4];assignNull(axlX);
  float gx = grvty.x()/9.8;
  float gy = grvty.y()/9.8;
  float gz = grvty.z()/9.8;

  float mx = mgntr.x()/9.8;
  float my = mgntr.y()/9.8;
  float mz = mgntr.z()/9.8;
  
  delay(BNO055_SAMPLERATE_DELAY_MS);
  Serial.println("here");
  
  float measuredvbat = analogRead(VBATPIN);
  measuredvbat *= 2;    // we divided by 2, so multiply back
  measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
  measuredvbat /= 1024; // convert to voltage
  Serial.print("VBat: " ); Serial.println(measuredvbat);

  char line1[50] = "lgrid:";//axl gravity
  char line2[50] = "lgrid:";//axl mag
  char line3[44] = "lgrid:";//ina power + soms

  //build/parse the line packets
  buildLineAxl(line1,";axl:",gx,gy,gz);
  buildLineAxl(line2,";mgr:",mx,my,mz);
  buildLineOth(line3,measuredvbat,0,12345);

  //transmit data
  sendLine(line1,50,1);
  sendLine(line2,50,2);
  sendLine(line3,50,3);
  
  Serial.println("#################################3");
  pinMode(A0,OUTPUT);
  delay(1000);
  digitalWrite(A0,HIGH);
  delay(1000);
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

void buildLineAxl(char* line, char * type, float vx, float vy, float vz){
  char axlString[4];assignNull(axlString);
  
  //line parsing
  strcat(line,sensorid);
  strcat(line,type);
  
  dtostrf(vx,7,4,axlString);
  strcat(line,axlString);
  strcat(line,",");
  
  dtostrf(vy,7,4,axlString);
  strcat(line,axlString);
  strcat(line,",");
  
  dtostrf(vz,7,4,axlString);
  strcat(line,axlString);
  
  Serial.println(line);
}

void buildLineOth(char* line, float vx, float vy, float vz){
  char fltString[4];assignNull(fltString);
  
  //line parsing
  strcat(line,sensorid);
  strcat(line,";");
  
  strcat(line,"vol:");
  dtostrf(vx,6,3,fltString);
  strcat(line,fltString);
  strcat(line,";");

  strcat(line,"cur:");
  dtostrf(vy,6,3,fltString);
  strcat(line,fltString);
  strcat(line,",");
  
  strcat(line,"som:");
  dtostrf(vz,5,0,fltString);
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
