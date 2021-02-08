#include <math.h>
#include <SPI.h>
//#include <avr/dtostrf.h>
#include <RH_RF95.h>
#include <Wire.h>
#include <EEPROM.h>
#include <Adafruit_SleepyDog.h>

#define SENSEID "99"
#define AREA "MGH"
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

#define SITE "EVM"
#define terminator "$"

#define VBATPIN A9
#define somsPin A3

char line1[60] = AREA; //axl gravity
char line3[80] = AREA; //ina power + soms
uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];

struct scl_data
{
  double ang_x;
  double ang_y;
  double ang_z;
  // double temp;
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
  
  // Print logger IDs
  Serial.print(AREA);
  Serial.print("-");
  Serial.print(SITE);
  Serial.print("-");
  Serial.println(SENSEID);

  // Timer control pin 
  pinMode(A0, OUTPUT);
  digitalWrite(A0,LOW);

  setupRadio();  

  Serial.println("done initializing......\n");

  SPI.begin();

  setupAccel();  
  
  blinkled(1);
  Serial.println("done setup");
  
}


void loop()
{
  // Watchdog.enable(25000);
  struct scl_data sc = scl_ave_axl();
  struct lgr_data lgr = get_data_lgr();
  // sc = scl_temp_comp(sc);

  // build/parse the line packets
  buildLineAXL(line1, sc);
  buildLineSMS(line3, lgr);

  //transmit data
  // Watchdog.enable(12000);
  sendLine(line1, "ACKAXL");
  // Watchdog.reset();
  // Watchdog.enable(13000);
  sendLine(line3, "ACKSMS");
  Serial.println("#################################");
  
  // delay(1000);
  digitalWrite(A0, HIGH);
  delay(60000);
  // Watchdog.reset();
    
}

struct scl_data scl_ave_axl()
{
  int samples = 1;
  double X = 0.0;
  double Y = 0.0;
  double Z = 0.0;
  // double temp = 0.0;

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
  dt.soms1 = get_soms_VWC(somsPin);
  dt.axelTemp = getTemp();

  return dt;
}

void buildLineAXL(char *line, struct scl_data dt)
{
  char sensorType[4] = "AXL";
  char tmpString[5];
  buildID(line, sensorType);

  assignNull(tmpString);

  dtostrf(dt.ang_x, 5, 4, tmpString);
  strcat(line, tmpString);
  strcat(line, ",");

  dtostrf(dt.ang_y, 5, 4, tmpString);
  strcat(line, tmpString);
  strcat(line, ",");

  dtostrf(dt.ang_z, 5, 4, tmpString);
  strcat(line, tmpString);

  Serial.println(line);
}

void buildLineSMS(char *line, struct lgr_data dt)
{
  char sensorType[4] = "SMS";
  char tmpString[5];
  buildID(line, "SMS");

  assignNull(tmpString);
  // insert "BTV:" - battery voltage here
  strcat(line, "BTV:");
  dtostrf(dt.vol, 5, 4, tmpString);
  strcat(line, tmpString);
  strcat(line, ",");
  // insert "BTA:" - Battery amperage ? Current
  strcat(line, "BTA:");
  dtostrf(dt.cur, 5, 4, tmpString);
  strcat(line, tmpString);
  strcat(line, ",");
  // insert "VWC:"  - volumetric water content
  strcat(line, "VWC:");
  dtostrf(dt.soms1, 5, 4, tmpString);
  strncat(line, tmpString, 5);
  strcat(line, ",");
  // insert "ACT:" - Accel Temperature
  strcat(line, "ACT:");
  dtostrf(dt.axelTemp, 5, 4, tmpString);
  strncat(line, tmpString, 5);
  // strcat(line,",");

  Serial.println(line);
}

