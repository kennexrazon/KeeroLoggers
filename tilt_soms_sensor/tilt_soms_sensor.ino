#include <SD.h>

//lib required #includes
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

#include <Wire.h>

#include <SPI.h>

#define sensorid "epalbrd"
//#define sensorid "TSTPOS1"

//lib required #define and global variable declarations
#define BNO055_SAMPLERATE_DELAY_MS (100)
Adafruit_BNO055 bno = Adafruit_BNO055();
int AXLadr = 0x28;
const int chipSelect = 4;
void setup(void)
{
  Serial.begin(9600);
  delay(2000);
  Serial.println(sensorid);Serial.println();
    
  Serial.println("Orientation Sensor Raw Data Test"); Serial.println("");
  
  /* Initialise the sensor */
  if(!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
  
  delay(1000);

  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");
  
  /* Display the current temperature */
//  int8_t temp = bno.getTemp();
//  Serial.print("Current Temperature: ");
//  Serial.print(temp);
//  Serial.println(" C");
//  Serial.println("");

  bno.setExtCrystalUse(true);

//  Serial.println("Calibration status values: 0=uncalibrated, 3=fully calibrated");
  
 //start current sense
//  uint32_t currentFrequency;
//  ina219.begin();
//  Serial.println("initializing ina219 done");
  
  //Start rx
//  pinMode(RFM95_RST, OUTPUT);
//  digitalWrite(RFM95_RST, HIGH);

//  delay(100);

//  Serial.println("Feather LoRa TX Test!");

  // manual reset
//  digitalWrite(RFM95_RST, LOW);
//  delay(10);
//  digitalWrite(RFM95_RST, HIGH);
//  delay(10);

//  while (!rf95.init()) {
//    Serial.println("LoRa radio init failed");
//    while (1);
//  }
//  Serial.println("LoRa radio init OK!");
//
//  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
//  if (!rf95.setFrequency(RF95_FREQ)) {
//    Serial.println("setFrequency failed");
//    while (1);
//  }
//  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
//  
//  rf95.setTxPower(23, false);
//
//  Serial.println("done initializing......\n");
//  randomSeed(analogRead(1));
//  delay(random(3000,4000));
//  blinkled();
  Serial.println("done setup");
}

int16_t packetnum = 0;  // packet counter, we increment per xmission

void loop(void)
{
  String dataString = "";
  imu::Vector<3> grvty = bno.getVector(Adafruit_BNO055::VECTOR_GRAVITY);
//  imu::Vector<3> mgntr = bno.getVector(Adafruit_BNO055::VECTOR_MAGNETOMETER);
  imu::Quaternion quat = bno.getQuat();
//  Serial.println("done init axl variables");
  
  //Read data from sensors: [1] axl (g's), [2] axl (m's), [3] power, [4] soms
//  char axlX[4];assignNull(axlX);
  int8_t temp = bno.getTemp();
  float gx = grvty.x()/9.8;
  float gy = grvty.y()/9.8;
  float gz = grvty.z()/9.8;

//  float mx = mgntr.x()/9.8;
//  float my = mgntr.y()/9.8;
//  float mz = mgntr.z()/9.8;

  float qw = quat.w();
  float qx = quat.x();
  float qy = quat.y();
  float qz = quat.z();
//  Serial.print("qw:"); Serial.print(qw,4);
//  Serial.print("  qx:"); Serial.print(qx,4);
//  Serial.print("  qy:"); Serial.print(qy,4);
//  Serial.print("  qz:"); Serial.println(qz,4);

//  delay(BNO055_SAMPLERATE_DELAY_MS);

  dataString = String(temp) + String(",");
  dataString += String(gx,4) + String(",");
  dataString += String(gy,4) + String(",");
  dataString += String(gz,4) + String(",");

  dataString += String(qw,4) + String(",");
  dataString += String(qx,4) + String(",");
  dataString += String(qy,4) + String(",");
  dataString += String(qz,4);

//  Serial.println(dataString);
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  
  File dataFile = SD.open("datalog.txt", FILE_WRITE);
  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    Serial.println(dataString);
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }

  delay(5000);  
//  Serial.println("here");

  //delete me
  //digitalWrite(A0,HIGH);
  //Serial.println("TPL 5110 failed");

  //transmit data
//  sendLine(line1,50,1);
//  sendLine(line2,50,2);
//  sendLine(line3,49,3);
  
//  Serial.println("#################################3");
//  pinMode(A0,OUTPUT);
//  delay(1000);
//  digitalWrite(A0,HIGH);
//  delay(1000);
}

//void sendLine(char* line,int inLen,int blinks){
//  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
//  uint8_t  len = sizeof(buf);
//  int retry = 0;
//  do{
//    randomSeed(analogRead(1));
//    int del = random(1000,2000);
//    Serial.print("..........................................");
//    Serial.println(del);
//    for(int i=0;i<blinks;i++)blinkled();
//    
//    Serial.print("Sending "); Serial.println(line);
//    Serial.println("Sending...");
//    delay(10);
//    rf95.send((uint8_t *)line, inLen);
//  
//    Serial.println("Waiting for packet to complete..."); 
//    delay(10);
//    rf95.waitPacketSent();
//  
//    // Now wait for a reply
//    Serial.println("Waiting for reply...");
//    if (rf95.waitAvailableTimeout(del))
//    { 
//      // Should be a reply message for us now   
//      if (rf95.recv(buf, &len))
//     {
//        Serial.print("Got reply: ");
//        Serial.println((char*)buf);
//        Serial.print("RSSI: ");
//        Serial.println(rf95.lastRssi(), DEC);    
//      }
//      else
//      {
//        Serial.println("Receive failed");
//      }
//    }
//    else
//    {
//      retry++;
//      Serial.print("No reply, is there a listener around?");
//      Serial.print("\t retry: ");
//      Serial.println(retry);
//      
//    }
//  }while(strcmp((char*)buf,"ACK") && (retry < SEND_RETRY_LIMIT));
//}

//void buildLineAxl(char* line, char * type, float vx, float vy, float vz){
//  char axlString[4];assignNull(axlString);
//  
//  //line parsing
//  strcat(line,sensorid);
//  strcat(line,type);
//  
//  dtostrf(vx,7,4,axlString);
//  strcat(line,axlString);
//  strcat(line,",");
//  
//  dtostrf(vy,7,4,axlString);
//  strcat(line,axlString);
//  strcat(line,",");
//  
//  dtostrf(vz,7,4,axlString);
//  strcat(line,axlString);
//  
//  Serial.println(line);
//}
//
//void buildLineOth(char* line, float vx, float vy, float vz){
//  char fltString[4];assignNull(fltString);
//  
//  //line parsing
//  strcat(line,sensorid);
//  strcat(line,";");
//  
//  strcat(line,"vol:");
//  dtostrf(vx,6,3,fltString);
//  strcat(line,fltString);
//  strcat(line,";");
//
//  strcat(line,"cur:");
//  dtostrf(vy,6,3,fltString);
//  strcat(line,fltString);
//  strcat(line,",");
//  
//  strcat(line,"som:");
//  dtostrf(vz,5,0,fltString);
//  strcat(line,fltString);
//  
//  Serial.println(line);
//}

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
