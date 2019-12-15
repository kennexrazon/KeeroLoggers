#include <math.h>
#include <SPI.h>
#include <RH_RF95.h>
#include <Wire.h>

#if defined (ARDUINO_SAMD_ZERO)
  #include <avr/dtostrf.h>
  #define RFM95_CS 8
  #define RFM95_RST 4
  #define RFM95_INT 3
  #define VBATPIN A7

#elif defined (ARDUINO_AVR_FEATHER32U4)
  #define RFM95_CS 8
  #define RFM95_RST 4
  #define RFM95_INT 7
  #define VBATPIN A9

#endif

// LGBU - LEY
// MSL - 19 - 21 
// SMR - 22 - 24
// MUR - 25 - 27
// MGA - 28 - 29
// RDO - 30 - 32
// NIGBU - NGR
// NJT - 33 - 35
// MLY - 36 - 38
// NTE - 39 - 41
// SGD - 42 - 44


#define RF95_FREQ 868.0
#define SENSEID "52"
#define AREA "NGR"
#define SITE "MLY"
#define terminator "$"

#define somsPin A3
#define SEND_RETRY_LIMIT 3

const int AccSelectPin = A5;
SPISettings settingSCA(2000000, MSBFIRST, SPI_MODE0);

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

  Serial.println("Tilt sensor");

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


  char axl_msg[50];//axl gravity
  char sms_msg[57];//ina power + soms

  assignNull(axl_msg);
  assignNull(sms_msg);

  strncat(axl_msg,AREA,3);
  strncat(sms_msg,AREA,3);

  //build/parse the line packets

  buildLineAXL(axl_msg,sc);
  buildLineSMS(sms_msg,lgr);

  //transmit data
  sendLine(axl_msg,strlen(axl_msg),1);
  // sendLine(line2,50,2);
  sendLine(sms_msg,strlen(sms_msg),3);
  
  Serial.println("#################################");
  pinMode(A0,OUTPUT);
  // digitalWrite(A0,LOW);
  // delay(1000);
  digitalWrite(A0,HIGH);
  delay(300000);
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

struct lgr_data get_data_lgr(){
  struct lgr_data dt;

  dt.vol = get_bat_vol(); 
  dt.cur = get_cur();
  dt.soms1 = get_soms_VWC(somsPin);
  dt.axelTemp = getTemp();

  return dt;
}

