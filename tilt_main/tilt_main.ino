#include <math.h>
#include <SPI.h>
#include <RH_RF95.h>
#include <Wire.h>
#include <Adafruit_SleepyDog.h>

const int AccSelectPin = A5;
SPISettings settingSCA(2000000, MSBFIRST, SPI_MODE0);

struct keyValuePair 
{
    char SENSEID[4];
    char AREA[4];
    char SITE[4];
    int FREQ;
    int  SOMS_SENSOR;
};

keyValuePair keyValueInput[1];
keyValuePair keyValueOutput[1];

#if defined (ARDUINO_SAMD_ZERO)
  #include <avr/dtostrf.h>
  #include <FlashStorage.h>
  #define FLASH_DEBUG       0
  #define RFM95_CS 8
  #define RFM95_RST 4
  #define RFM95_INT 3
  #define VBATPIN A7
  const int WRITTEN_SIGNATURE = 0xBEEFDEED;
  FlashStorage(flashStorage,keyValuePair);
  #define  MICROCON "M0"
#elif defined (ARDUINO_AVR_FEATHER32U4)
  #include <EEPROMex.h> 
  #define RFM95_CS 8
  #define RFM95_RST 4
  #define RFM95_INT 7
  #define VBATPIN A9
  #define MICROCON "32U4"
#endif

float RF95_FREQ; 
const int address = 0;
int SOMS_TYPE = 1;

// struct keyValuePair 
// {
//     char SENSEID[4];
//     char AREA[4];
//     char SITE[4];
//     int FREQ;
//     int  SOMS_SENSOR;
// };




//#define SENSEID "04"
//#define AREA "MGH"  
// #define SomsSensor "ECHO5"
// MSL - 19 - 21
// SMR - 22 - 24s
// MUR - 25 - 27
// MGA - 28 - 29
// RDO - 30 - 32
// NIGBU - NGR
// NJT - 33 - 35
// MLY - 36 - 38
// NTE - 39 - 41
// SGD - 42 - 44
// APO - APO
// PCO - 45 - 47
// LTC - 48 - 51
// TCT - 54 - 56 // phase 4
// MDR - 57 - 59
// GEN - 60 - 61  generic codes
// BACMAN - BCM
// LRD -- ulit loop road
// BACMAN - BCM // phase 5
// LRD - 62 - 67  ( 62-63:m0,HS-10 || 64 - 65 32u4,ECHO5 || 66 - 67 32u4, HS-10) 

//#define SITE "TST"

#define terminator "$"
#define somsPin A3
#define SEND_RETRY_LIMIT 3

RH_RF95 rf95(RFM95_CS, RFM95_INT);

char line1[60];
char line3[60];
uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];

struct scl_data
{
    double ang_x;
    double ang_y;
    double ang_z;
} struct_scl_data;

struct lgr_data
{
    float vol;
    float cur;
    float soms1;
    float axelTemp;
} struct_lgr_data;

void setup()
{
    Serial.begin(9600);
    long now = millis();
    pinMode(12,OUTPUT);
    digitalWrite(12,HIGH);
    pinMode(11,INPUT);

    if (digitalRead(11)){
        storeToEeprom();
    }

    #if defined (ARDUINO_AVR_FEATHER32U4)
        EEPROM.readBlock(address, keyValueOutput,1);
    #elif defined (ARDUINO_SAMD_ZERO)
        // EEPROM.read(address,keyValueOutput);
        keyValueOutput[0] = flashStorage.read();
    #endif

    displayConfig();
    
    strncpy(line1,keyValueOutput[0].AREA,4);
    strncpy(line3,keyValueOutput[0].AREA,4);

    pinMode(A0, OUTPUT);
    digitalWrite(A0,LOW);

    digitalWrite(RFM95_RST, LOW);
    delay(10);
    digitalWrite(RFM95_RST, HIGH);
    delay(10);

    while (!rf95.init())
    {
        Serial.println("LoRa radio init failed, shutting down");
        digitalWrite(A0, HIGH);
        delay(1000);
    }

    if (!rf95.setFrequency(RF95_FREQ))
    {
        Serial.println("setFrequency failed");
    }
    Serial.print("Set Freq to: ");
    Serial.println(RF95_FREQ);

    rf95.setTxPower(20, false);

    Serial.println("done initializing......\n");

    SPI.begin();
    pinMode(AccSelectPin, OUTPUT);
    blinkled();
    Serial.println("done setup");
  
}

void displayConfig(){
    if (keyValueOutput[0].SENSEID[3] != '\0'){
        strcpy(keyValueOutput[0].SENSEID, "999");
    }

    if (keyValueOutput[0].AREA[3] != '\0'){
        strcpy(keyValueOutput[0].AREA, "GEN");
    }

    if ( keyValueOutput[0].SITE[3] != '\0'){
        strcpy(keyValueOutput[0].SITE, "DEF");
    }

    if (keyValueOutput[0].FREQ == 1){
        RF95_FREQ = 433.0;
    } else {
        RF95_FREQ = 868.0; 
    }  

    if (keyValueOutput[0].SOMS_SENSOR == 1){
        SOMS_TYPE = 2; // HC-10
    } else {
        SOMS_TYPE = 1; // ECHO-5
    } 

    Serial.print(keyValueOutput[0].SENSEID);
    Serial.print("\t");
    Serial.print(keyValueOutput[0].SITE);
    Serial.print("\t");
    Serial.println(keyValueOutput[0].AREA);
}
void processData()
{
  struct scl_data sc = scl_ave_axl();
  struct lgr_data lgr = get_data_lgr();

  //build/parse the line packets

  buildLineAXL(line1, sc);
  buildLineSMS(line3, lgr);

  sendLine2(line1, 1);
  sendLine3(line3, 3);
  Serial.println("#################################");
  
  digitalWrite(A0, HIGH);
  delay(1000);
}

void loop()
{
  processData();
}

struct scl_data scl_ave_axl()
{
    int samples = 1;
    double X = 0.0;
    double Y = 0.0;
    double Z = 0.0;

    struct scl_data sc;
    sc.ang_x = get_accx();
    sc.ang_y = get_accy();
    sc.ang_z = get_accz();
    return sc;
}

struct lgr_data get_data_lgr()
{
    struct lgr_data dt;

    dt.vol = get_bat_vol();
    dt.cur = get_cur();
    dt.soms1 = get_soms_VWC(somsPin,SOMS_TYPE);
    dt.axelTemp = getTemp();

    return dt;
}

int storeToEeprom(){
    String sid,area,site,freq;
    char temp1[4]; 
    long start_time;

    Serial.println("Store to EEPROM");

    if (readFromSerial(sid,temp1,1) > 0){
        Serial.println(">>>");
    } else {
        Serial.println("Failed to save new SENSOR ID");
        return -1;
    }

    if (readFromSerial(site,temp1,2) > 0){
        Serial.println(">>>");
    } else {
        Serial.println("Failed to save new SITE ID");
        return -1;
    }

    if (readFromSerial(area,temp1,3) > 0){
        Serial.println(">>>");
    } else {
        Serial.println("Failed to save new AREA ID");
        return -1;
    }

    if (readFromSerial(freq,temp1,4) > 0){
        Serial.println(">>>");
    } else {
        Serial.println("Failed to save new FREQUENCY");
        return -1;
    }
    #if defined (ARDUINO_AVR_FEATHER32U4)
        EEPROM.writeBlock(address, keyValueInput,1);
    #elif defined (ARDUINO_SAMD_ZERO)
        flashStorage.write(keyValueInput[0]);
    #endif
    Serial.println("Storing done!");
    return 1;
}


int readFromSerial(String input,char *temp,int type){
  long start = millis();
  if (type == 1){
    Serial.println("New SENSOR ID (000 - 999): ");
  } else if (type == 2){
    Serial.println("New SITE ID (XXX): ");
  } else if (type == 3){
    Serial.println("New AREA ID (XXX): ");
  } else if (type == 4){
    Serial.println("Pick Frequency:");
    Serial.println("Type 1 for 433Mhz \t Type 2 for 868Mhz");
  }

  while (!Serial.available()){
      if (millis() - start < 10000){
        delay(10);
      } else {
        return -1; // timeout
      }
  }
  if ( Serial.available() > 0 ) {
      input = Serial.readString();
  }
  if (type == 1){
      if (!input.toInt() ){
          Serial.println("Sensor ID requires a number from 000 to 999");
          Serial.print("You entered: ");
          Serial.println(input);
          return -1;
      } else {
          Serial.print(">>");
          Serial.print(input);
          input.toCharArray(temp,4);
          strcpy(keyValueInput[0].SENSEID, temp);
          return 1;
      }
  } else if( type == 2 || type == 3) {
        input.toUpperCase();
        input.replace("\n","");
        input.replace("\r","");
        if (input.length() != 3){
            if (type == 2){
                Serial.println("SITE ID requires 3 characters.");
            } else if(type == 3){
                Serial.println("AREA ID requires 3 characters.");
            } 
            Serial.print("You entered: ");
            Serial.println(input);
        } else {
            if (type == 2){
                input.toCharArray(temp,4);
                strcpy(keyValueInput[0].SITE, temp);
            } else if(type == 3){
                input.toCharArray(temp,4);
                strcpy(keyValueInput[0].AREA, temp);
          } 
          Serial.print(">>");
          Serial.print(input);
          return 1;
      }
  } else if (type == 4){
        input.replace("\n","");
        input.replace("\r","");
        if (!input.toInt() ){
            Serial.println("Pick Frequency require 1 or 2.");
            Serial.print("You entered: ");
            Serial.println(input);
            return -1;
        } else {
            Serial.print(">>");
            if (input == "1"){
                Serial.print("433Mhz");
            } else if ( input == "2"){
                Serial.print("868Mhz");
            }
            keyValueInput[0].FREQ = input.toInt();
            return 1;
      }
  }
  return -1;
}
