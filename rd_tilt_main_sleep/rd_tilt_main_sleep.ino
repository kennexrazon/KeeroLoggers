//#define sense_id "04"
//#define area "MGH"  
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
// CBD - 76,77,78 433MHZ
// PDH - 79,80 868MHZ
// PDE - 73,74,75 433MHZ
// PRS - 81,82,83 868MHZ
// PLI - 84,85 433MHZ
// APO
// TCT 86, 87 433MHZ
// PCO 88, 89 433MHZ
// LTC 90 433MHZ

#include <math.h>
#include <SPI.h>
#include <RH_RF95.h>
#include <Wire.h>
#include <Adafruit_SleepyDog.h>
#include <RHReliableDatagram.h>

#if defined (ARDUINO_SAMD_ZERO)
  #include "ArduinoLowPower.h"
  #include <avr/dtostrf.h>
  #include <FlashStorage.h>
  #define FLASH_DEBUG 0
  #define RFM95_CS 8
  #define RFM95_RST 4
  #define RFM95_INT 3
  #define VBATPIN A7 // https://learn.adafruit.com/assets/110921
  // FlashStorage(flashStorage,keyValuePair); 
  #define  MICROCON "M0"
  #define UC_TYPE 2
#elif defined (ARDUINO_AVR_FEATHER32U4)
  #include <Adafruit_SleepyDog.h>
  #include <EEPROMex.h> 
  #define RFM95_CS 8
  #define RFM95_RST 4
  #define RFM95_INT 7
//https://learn.adafruit.com/adafruit-feather-32u4-radio-with-lora-radio-module/pinouts
  #define VBATPIN A9 
  #define MICROCON "32U4"
  #define UC_TYPE 1
#endif

#define terminator "$"
#define somsPin A3
#define SEND_RETRY_LIMIT 3
#define RF_PAYLOAD_MAX_CHAR 120
#define READPROPSPIN 11
#define HIGHPROPSPIN 12
#define SLEEPTIMEMS 9000
#define WAKE_ON_N_SLEEP 50
#define ENA3V3 A0
#define SENSOR_ADDRESS 3
#define GATEWAY_ADDRESS 0

const int WRITTEN_SIGNATURE = 0xBEEFDEED;
const int AccSelectPin = A5;
SPISettings settingSCA(2000000, MSBFIRST, SPI_MODE0);


const int address = 0;
int SOMS_TYPE = 1;

struct SensorProps
{
    char sense_id[4];
    char area[4];
    char site[4];
    int freq;
    int soms_type;
    float rf95_freq; 
};

SensorProps inputProps;
SensorProps this_sensor;

#if defined (ARDUINO_SAMD_ZERO)
    FlashStorage(props_storage,SensorProps);
#endif

RH_RF95 rf95(RFM95_CS, RFM95_INT);
RHReliableDatagram manager(rf95, SENSOR_ADDRESS);

char line1[RH_RF95_MAX_MESSAGE_LEN];
char line3[RH_RF95_MAX_MESSAGE_LEN];
uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];

struct SclData
{
    double ang_x;
    double ang_y;
    double ang_z;
} struct_SclData;

struct LgrData
{
    float vol;
    float cur;
    float soms1;
    float axelTemp;
} struct_LgrData;

uint8_t lora_id = 0;

void setup()
{
    Serial.begin(9600);
    long now = millis();

    pinMode(RFM95_RST, OUTPUT);
    digitalWrite(RFM95_RST, HIGH);

    pinMode(HIGHPROPSPIN,OUTPUT);
    digitalWrite(HIGHPROPSPIN,HIGH);
    
    pinMode(READPROPSPIN,INPUT);

    pinMode(ENA3V3,OUTPUT); // comment muna for checking btv
    pinMode(LED_BUILTIN,OUTPUT);
    digitalWrite(ENA3V3,LOW);
    digitalWrite(LED_BUILTIN,LOW);
    
    if (digitalRead(READPROPSPIN)==HIGH){
        if (getSensorProps()){
            #if defined (ARDUINO_AVR_FEATHER32U4)
                EEPROM.writeBlock(address, &this_sensor, 1);
            #elif defined (ARDUINO_SAMD_ZERO)
                props_storage.write(this_sensor);
            #endif
        }
    }

    #if defined (ARDUINO_AVR_FEATHER32U4)
        EEPROM.readBlock(address, &this_sensor,1);
    #elif defined (ARDUINO_SAMD_ZERO)
        this_sensor = props_storage.read();
    #endif

    lora_id = (uint8_t)atoi(this_sensor.sense_id);
    Serial.print("sensor id: "); Serial.println(lora_id);
    

    displayConfig();
    
    strncpy(line1,this_sensor.area,4);
    strncpy(line3,this_sensor.area,4);

    pinMode(A0, OUTPUT);
    digitalWrite(A0,LOW);

    digitalWrite(RFM95_RST, LOW);
    delay(20);
    digitalWrite(RFM95_RST, HIGH);
    delay(500);

    digitalWrite(LED_BUILTIN, HIGH);

    // init_radio(lora_id);
    
    // rf95.setCADTimeout(10000);

    Serial.println("done initializing......\n");

    SPI.begin();
    pinMode(AccSelectPin, OUTPUT);
    digitalWrite(AccSelectPin, HIGH);
    blinkled();
    Serial.println("done setup");

}

uint16_t sleep_ctr=0;
void loop()
{
    if (sleep_ctr==0){
        digitalWrite(ENA3V3,HIGH);
        if (init_radio()){
            digitalWrite(ENA3V3,HIGH);
            // digitalWrite(LED_BUILTIN, HIGH);
            delay(50);
            
            SclData sc = scl_ave_axl();
            LgrData lgr = get_data_lgr();
            // delay(4000);

            // delay(50);
            // digitalWrite(ENA3V3,LOW);
            // digitalWrite(LED_BUILTIN, LOW);

            //build/parse the line packets

            buildLineAXL(line1, sc);
            buildLineSMS(line3, lgr);

            sendLine4(line1, 1);
            delay(100);
            sendLine4(line3, 3);
            delay(100);
            Serial.println("#################################");
        }
        digitalWrite(ENA3V3,LOW);
    }
    else{
        blinkled();
    }

    // while(1);

    // delay(5000);
    rf95.sleep();
    goIdle();
    sleep_ctr++;
    if (sleep_ctr>WAKE_ON_N_SLEEP){
        sleep_ctr=0;
    }
    

}

bool init_radio(){
    uint8_t rst=0;
    SPI.begin();
    while (rst<10){
        if (!manager.init())
        {
            Serial.print("LoRa radio init failed: ");
            Serial.println(rst);

            digitalWrite(RFM95_RST, LOW);
            delay(10);
            digitalWrite(RFM95_RST, HIGH);
            delay(10);

        }
        else{
            Serial.println("Init okay");
            digitalWrite(LED_BUILTIN, LOW);
            break;
        }
        rst=rst+1;     
        delay(1000);   
    }
    if (rst==10){
        Serial.print("Radio failed after ");
        Serial.print(rst);
        Serial.println(" tries.");
        delay(1000);
        return false;
    }
    manager.setThisAddress(lora_id);

    if (!rf95.setFrequency(this_sensor.rf95_freq))
    {
        Serial.println("setFrequency failed");
        return false;
    }
    Serial.print("Set Freq to: ");
    Serial.println(this_sensor.rf95_freq);

    rf95.setTxPower(23, false);
    // rf95.setSignalBandwidth(62500);
    // rf95.setCodingRate4(8);
    // rf95.setSpreadingFactor(9);

    manager.setTimeout(500);
    manager.setRetries(10);

    return true;

}

void goIdle(){
    #if defined (ARDUINO_SAMD_ZERO)
        LowPower.sleep(SLEEPTIMEMS);
        SPI.begin();
    #elif defined (ARDUINO_AVR_FEATHER32U4)
        Watchdog.sleep(SLEEPTIMEMS);        
    #endif
}

void displayConfig(){
    if (this_sensor.sense_id[3] != '\0'){
        strcpy(this_sensor.sense_id, "999");
    }

    if (this_sensor.area[3] != '\0'){
        strcpy(this_sensor.area, "GEN");
    }

    if ( this_sensor.site[3] != '\0'){
        strcpy(this_sensor.site, "DEF");
    }

    // if (this_sensor.freq == 1){
    //     rf95_freq = 433.0;
    // } else {
    //     rf95_freq = 868.0; 
    // }  

    // this_sensor.soms_type

    // if (this_sensor.soms_type == 1){
    //     SOMS_TYPE = 2; // HC-10
    // } else {
    //     SOMS_TYPE = 1; // ECHO-5
    // } 

    Serial.print(this_sensor.site);
    Serial.print("-");
    Serial.println(this_sensor.area);
    Serial.print("-");
    Serial.print(this_sensor.sense_id);
}


struct SclData scl_ave_axl()
{
    int samples = 1;
    double X = 0.0;
    double Y = 0.0;
    double Z = 0.0;

    struct SclData sc;
    sc.ang_x = get_accx();
    sc.ang_y = get_accy();
    sc.ang_z = get_accz();
    return sc;
}

struct LgrData get_data_lgr()
{
    struct LgrData dt;

    dt.vol = get_bat_vol();
    dt.cur = get_cur();
    dt.soms1 = get_soms_VWC(somsPin,this_sensor.soms_type);
    dt.axelTemp = getTemp();

    return dt;
}

int storeToEeprom(){
    return 0;
}

void getSerialInput(char *input){
    char inChar;
    int i=0;
    long start = millis();
    // Serial.println(start);

    // Serial.println("get serial");

    do{
        if(Serial.available()>0){
            inChar = Serial.read();

            if (inChar>'/' && inChar<'['){
                input[i] = inChar;
                i++;
            }
        }
    }while(inChar!='\r' && millis()-start<10000);

    // Serial.println(millis()-start);/**/

    // Serial.println("end get serial");

}

// test: MNG;TST;131;1;1
// test: MNG;TST;131;3;1
// test: MNG;TST;131;1;3
// test: MNG;TST;131;1;1
// test: HHH;EEE;222;1;1

bool getSensorProps(){
    char serialInput[50];
    char inputForProcessing[50];
    char *_props[5];
    char *ptr=NULL;
    byte index = 0;
    char isPropsValid = false;

    Serial.setTimeout(1000);

    assignNull(serialInput,50);

    while(!Serial);

    Serial.println(">> Enter sensor properties (10 sec timeout)");
    Serial.println(">> Send [x] to exit ");
    Serial.println("AREA[xxx];SITE[xxx];ID[xxx];FREQ[1 or 2];SOMS_TYPE[1 or 2]");

    for (int r=0; r<5; r++){
        index = 0;
        char *ptr=NULL;

        Serial.print("> ");
        getSerialInput(serialInput);

        Serial.println(serialInput);

        if (serialInput[0]=='\0') continue;
        else if (serialInput[0]=='x') break;

        ptr = strtok(serialInput,";");
        while(ptr != NULL){
            _props[index] = ptr;
            index++;
            ptr = strtok(NULL,";");
        };

        if (index<4){
            Serial.println("ERROR: Missing properties");

        }
        else if (
            strlen(_props[0])!=3 || 
            strlen(_props[1])!=3 || 
            strlen(_props[2])!=3)
        {
            Serial.println("ERROR: Invalid name properties");
            Serial.println(_props[0]);
            Serial.println(_props[1]);
            Serial.println(_props[2]);

        }
        else if (_props[3][0] < '1' || _props[3][0] > '4')
        {
            Serial.println("ERROR: unknown freq option");
            Serial.println(_props[3][0]);
        }
        else if (_props[4][0] < '0' ||  _props[4][0] > '3')
        {
            Serial.println("ERROR: unknown soms type option");
            Serial.println(_props[4][0]);
        } 
        else if(r == 4){
            Serial.println("ERROR: reached limit.");
            return isPropsValid;
        }
        else{
            Serial.println("Valid properties");
            for(int n = 0; n < index; n++)
            { 
                Serial.print(_props[n]);
                Serial.print(" - ");
            }
            Serial.println("");
            isPropsValid = true;
            break;
        }    

        assignNull(serialInput, 50);
    }

    if (isPropsValid){

        Serial.print("Assigning properties...");

        strncpy(this_sensor.area, _props[0],3);
        strncpy(this_sensor.site, _props[1],3);
        strncpy(this_sensor.sense_id, _props[2],3);

        if (_props[3][0] == '1'){
            this_sensor.rf95_freq = 433.0;
        }else if ( _props[3][0] == '3'){
            this_sensor.rf95_freq = 900.0;
        }else if ( _props[3][0] == '4'){
            this_sensor.rf95_freq = 915.0;
        }else {
            this_sensor.rf95_freq = 868.0; 
        }  

        this_sensor.soms_type = _props[4][0];

        // if (_props[4][0] == ''){
        //     this_sensor.soms_type = 1; // HC-10
        // else if (_props[4][0] == '3'){
        //     this_sensor.soms_type = 2; // HC-10
        // } else {
        //     this_sensor.soms_type = 1; // ECHO-5
        // } 

        Serial.println("done");
        return isPropsValid;
    }

}

void assignNull(char *txt, uint16_t len)
{
  for (int i = 0; i < len; i++) txt[i] = '\0';
}

