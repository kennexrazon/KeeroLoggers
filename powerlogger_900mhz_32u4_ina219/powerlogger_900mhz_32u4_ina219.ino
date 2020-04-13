#include <math.h>
#include <SPI.h>
//#include <avr/dtostrf.h>
#include <RH_RF95.h>
#include <Wire.h>

#include <Adafruit_INA219.h>
Adafruit_INA219 ina219;

const int AccSelectPin = A5;
SPISettings settingSCA(2000000, MSBFIRST, SPI_MODE0);

#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 7
#define RF95_FREQ 868.0

#define SENSEID "39"
#define AREA "NGR"
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
#define SITE "NTE"
#define terminator "$"

#define VBATPIN A9
#define somsPin A3
#define SEND_RETRY_LIMIT 1

RH_RF95 rf95(RFM95_CS, RFM95_INT);
int counter = 0;
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

struct ina_data{
  float shuntvoltage;
  float busvoltage;
  float current_mA;
  float loadvoltage;
  float power_mW;
} struct_ina_data;

void setup() {
  Serial.begin(9600);
  ina219.begin();
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
  unsigned long start = millis();
  float elapsed_secs;
  delay(1000);
//  struct scl_data sc = scl_ave_axl();
//  struct lgr_data lgr = get_data_lgr();

  struct ina_data ina = get_ina_data();

  // sc = scl_temp_comp(sc);

  char line0[50] = "";
//  char line1[50] = AREA;//axl gravity
//  char line3[57] = AREA;//ina power + soms

  //build/parse the line packets
  counter++;
  elapsed_secs = ((millis() - start)*(1.0))/(1000*1.0);
  buildLineINA(line0,ina,elapsed_secs, counter);
//  buildLineAXL(line1,sc);
//  buildLineSMS(line3,lgr);

  //transmit data
  sendLine(line0,50,1);
//  sendLine(line1,50,1);
  // sendLine(line2,50,2);
//  sendLine(line3,57,3);
  
  Serial.println("#################################");
  pinMode(A0,OUTPUT);
  // digitalWrite(A0,LOW);
  // delay(1000);
  digitalWrite(A0,HIGH);
//  delay(1000);
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

struct ina_data get_ina_data(){
  struct ina_data dt;
  dt.shuntvoltage = ina219.getShuntVoltage_mV();
  dt.busvoltage = ina219.getBusVoltage_V();
  dt.current_mA = ina219.getCurrent_mA();
  dt.power_mW = ina219.getPower_mW();
  dt.loadvoltage = dt.busvoltage + (dt.shuntvoltage / 1000);
  return dt;
}
