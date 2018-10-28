#include <avr/dtostrf.h>

//define sensor name/s
#define SENSEID "00"
#define AREA "BCM"
#define SITE "PDE"
#define terminator "$"

//define pin assignments
#define donePin A0  //done pin for tpl
#define randomPin A1  //pin for generating random seed for random delay when sending
#define somsPin A3  //SOMS input
#define sendTrials 5  //number of send retries before exit

#define TILT1 1
#define TILT2 1
#define TILT3 1
#define TILT4 1
#define TILT5 1

#define SOMS1 1
#define SOMS2 1
#define SOMS3 1
#define SOMS4 1
#define SOMS5 1
#define SOMS6 1


//#include <utility/imumaths.h>
#include <Wire.h>

//#include <Adafruit_INA219.h>

#include <SPI.h>
#include <RH_RF95.h>

//lib required #define and global variable declarations
//#define BNO055_SAMPLERATE_DELAY_MS (100)
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3
#define RF95_FREQ 433.0

// Serial related defines
#define BAUD 115200
#define CR1000 Serial1

RH_RF95 rf95(RFM95_CS, RFM95_INT);


struct gkn_data{
  String t1a; String t1b;
  String t2a; String t2b;
  String t3a; String t3b;
  String t4a; String t4b;
  String t1t;
  String t2t;
  String t3t;
  String t4t;
  
  String dl_batt;
  String dl_temp;
  String ec1;
  String ec2;
  String ec3;
  String ec4;
  String ec5;
  String ec6;
  String te525;
} struct_gkn_dta;

void setup() {
  Serial.begin(BAUD);
  CR1000.begin(BAUD);

  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  delay(100);

  Serial.println("Feather LoRa TX Test!");

  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  //do not continue if rf is not okay
  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }
  Serial.println("LoRa radio init OK!");

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
  
  rf95.setTxPower(23, false);

  //randomSeed(analogRead(randomPin));
  blinkled();
  Serial.println("done setup");
  
//  while(!Serial.available());
  struct gkn_data data = send_cr();
  char line1[200] =  AREA;
  char line2[200] =  AREA;
  char line3[200] =  AREA;
  
  buildLine_04(line1,data);
  buildLine_05(line2,data);
  buildLine_06(line3,data);


  sendLine(line1,125,1);
  sendLine(line2,93,2);
  sendLine(line3,62,3);
  
  Serial.println("#################################3");
  digitalWrite(donePin,HIGH);

}

void loop() {

Serial.println("."); 
delay(1000);   
}


struct gkn_data send_cr(){
  String inLine;
  int flag1 = 0;
  String prompt = "CR1000>";
  String ellipsis = " ...";
  int limit = 8;  
  int len = 0;
  struct gkn_data dt;
  if ( !CR1000.available()){
      CR1000.write('\r');
      delay(10);    
  }
  while ( (limit > 0)){
    if ( flag1 == 0){
      for (int i = 0; i < 1; i++){
        CR1000.write('\r');
        delay(10);
      }
    }
    inLine  = CR1000.readStringUntil('\r\n');
    len = inLine.length();
    if (len == 0){
      limit = limit - 1 ;
      Serial.println(limit);
    }
     dt = parseLine(inLine);
    if (inLine.startsWith(prompt) && flag1 != 2) {
      CR1000.write('7');
      CR1000.write('\r');
      flag1 = 2;
    } else if (inLine.startsWith(ellipsis) && flag1 != 3){
      CR1000.write('\r');
      flag1 = 3;    
    }
  }
  return dt;
}

void buildID(char* line,char* sensor){
  strcat(line,"-");
  strcat(line,SITE);
  strcat(line,"-");
  strcat(line,sensor);
  strcat(line,SENSEID);
  strcat(line,terminator);
}

void buildLine_04(char* line, struct gkn_data dt){
  // struct gkn_data dt;  
  char sensorType[8] =  "GKN-TLT";
  char tmpString[10];
  assignNull(tmpString);
  
  buildID(line,sensorType);
  if (TILT1){
    strcat(line,"T1:");
    dt.t1a.toCharArray(tmpString,10);
    strcat(line, tmpString);
    strcat(line,",");
    dt.t1b.toCharArray(tmpString,10);
    strcat(line, tmpString);
    strcat(line,",");
    strcat(line, "0.0");
    strcat(line,";");
  }  
  if (TILT2){
    strcat(line,"T2:");
    dt.t2a.toCharArray(tmpString,10);
    strcat(line, tmpString);
    strcat(line,",");
    dt.t2b.toCharArray(tmpString,10);
    strcat(line, tmpString);
    strcat(line,",");
    strcat(line, "0.0");
    strcat(line,";");
  }
  if (TILT3){
    strcat(line,"T3:");
    dt.t3a.toCharArray(tmpString,10);
    strcat(line, tmpString);
    strcat(line,",");
    dt.t3b.toCharArray(tmpString,10);
    strcat(line, tmpString);
    strcat(line,",");
    strcat(line, "0.0");
    strcat(line,";");
  }

  if (TILT4){
    strcat(line,"T4:");
    dt.t4a.toCharArray(tmpString,10);
    strcat(line, tmpString);
    strcat(line,",");
    dt.t4b.toCharArray(tmpString,10);
    strcat(line, tmpString);
    strcat(line,",");
    strcat(line, "0.0");
    strcat(line,";");
  }

  strcat(line,"$");
  Serial.println(line);
}

void buildLine_05(char* line, struct gkn_data dt){
  // struct gkn_data dt;  
  char sensorType[8] =  "GKN-EC5";
  char tmpString[10];
  assignNull(tmpString);
  
  buildID(line,sensorType);
  if (SOMS1){
    strcat(line,"EC51:");
    dt.ec1.toCharArray(tmpString,10);
    strcat(line, tmpString);
    strcat(line,";");
  }
  if (SOMS2){
    strcat(line,"EC52:");
    dt.ec2.toCharArray(tmpString,10);
    strcat(line, tmpString);
    strcat(line,";");
  }
  if (SOMS3){
    strcat(line,"EC53:");
    dt.ec3.toCharArray(tmpString,10);
    strcat(line, tmpString);
    strcat(line,";");
  }
  if (SOMS4){
    strcat(line,"EC54:");
    dt.ec4.toCharArray(tmpString,10);
    strcat(line, tmpString);
    strcat(line,";");
  }
  if (SOMS5){
    strcat(line,"EC55:");
    dt.ec5.toCharArray(tmpString,10);
    strcat(line, tmpString);
    strcat(line,";");
  }

  strcat(line,"$");
  Serial.println(line);
}

void buildLine_06(char* line, struct gkn_data dt){
  // struct gkn_data dt;  
  char sensorType[8] =  "GKN-DLS";
  char tmpString[10];
  assignNull(tmpString);
  
  buildID(line,sensorType);

  strcat(line,"dl_batt:");
  dt.dl_batt.toCharArray(tmpString,10);
  strcat(line, tmpString);
  strcat(line,";");

  strcat(line,"dl_temp:");
  dt.dl_temp.toCharArray(tmpString,10);
  strcat(line, tmpString);
  strcat(line,";");

  strcat(line,"te525:");
  dt.te525.toCharArray(tmpString,10);
  strcat(line, tmpString);
  strcat(line,";");

  strcat(line,"$");
  Serial.println(line);
}

struct gkn_data parseLine(String line){
//  removes spaces
  line.replace(" ", "");
// define all expected variable names
  String dl_batt = "dl_batt"; String dl_temp = "dl_temp";
  String t1a = "tilt1_angle_a"; String t1b = "tilt1_angle_b";
  String t2a = "tilt2_angle_a"; String t2b = "tilt2_angle_b";
  String t3a = "tilt3_angle_a"; String t3b = "tilt3_angle_b";
  String t4a = "tilt4_angle_a"; String t4b = "tilt4_angle_b";
  String t1t = "tilt1_temp"; String t2t = "tilt2_temp";
  String t3t = "tilt3_temp"; String t4t = "tilt4_temp";
  String ec1 = "ec_5_1"; 
  String ec2 = "ec_5_2";
  String ec3 = "ec_5_3";
  String ec4 = "ec_5_4";
  String ec5 = "ec_5_5";
  String ec6 = "ec_5_6";
  String te525 = "te525";
// find index of ":"
  int index1 = 0;
  
  line.toLowerCase();
  index1 = line.indexOf(":");
  if (line.startsWith(dl_batt)){
    struct_gkn_dta.dl_batt = line.substring(index1 + 1);
    Serial.println(struct_gkn_dta.dl_batt);
  } else if (line.startsWith(dl_temp)) {
    struct_gkn_dta.dl_temp = line.substring(index1 + 1);
    Serial.println(struct_gkn_dta.dl_temp);
  } else if (line.startsWith(t1a)){
    struct_gkn_dta.t1a = line.substring(index1 + 1);
    Serial.println(struct_gkn_dta.t1a);
  } else if (line.startsWith(t1b)){
    struct_gkn_dta.t1b = line.substring(index1 + 1);
    Serial.println(struct_gkn_dta.t1b);     
  } else if (line.startsWith(t2a)){
    struct_gkn_dta.t2a = line.substring(index1 + 1);
    Serial.println(struct_gkn_dta.t2a);     
  } else if (line.startsWith(t2b)){
    struct_gkn_dta.t2b = line.substring(index1 + 1);
    Serial.println(struct_gkn_dta.t2b);     
  } else if (line.startsWith(t3a)){
    struct_gkn_dta.t3a = line.substring(index1 + 1);
    Serial.println(struct_gkn_dta.t3a);     
  } else if (line.startsWith(t3b)){
    struct_gkn_dta.t3b = line.substring(index1 + 1);
    Serial.println(struct_gkn_dta.t3b);     
  } else if (line.startsWith(t4a)){
    struct_gkn_dta.t4a = line.substring(index1 + 1);
    Serial.println(struct_gkn_dta.t4a);     
  } else if (line.startsWith(t4b)){
    struct_gkn_dta.t4b = line.substring(index1 + 1);
    Serial.println(struct_gkn_dta.t4b);     
  } else if (line.startsWith(t1t)){
    struct_gkn_dta.t1t = line.substring(index1 + 1);
    Serial.println(struct_gkn_dta.t1t);     
  } else if (line.startsWith(t2t)){
    struct_gkn_dta.t2t = line.substring(index1 + 1);
    Serial.println(struct_gkn_dta.t2t);     
  } else if (line.startsWith(t3t)){
    struct_gkn_dta.t3t = line.substring(index1 + 1);
    Serial.println(struct_gkn_dta.t3t);     
  } else if (line.startsWith(t4t)){
    struct_gkn_dta.t4t = line.substring(index1 + 1);
    Serial.println(struct_gkn_dta.t4t);     
  }else if (line.startsWith(ec1)){
    struct_gkn_dta.ec1 = line.substring(index1 + 1);
    Serial.println(struct_gkn_dta.ec1);     
  } else if (line.startsWith(ec2)){
    struct_gkn_dta.ec2 = line.substring(index1 + 1);
    Serial.println(struct_gkn_dta.ec2);     
  } else if (line.startsWith(ec3)){
    struct_gkn_dta.ec3 = line.substring(index1 + 1);
    Serial.println(struct_gkn_dta.ec3);     
  } else if (line.startsWith(ec4)){
    struct_gkn_dta.ec4 = line.substring(index1 + 1);
    Serial.println(struct_gkn_dta.ec4);     
  } else if (line.startsWith(ec5)){
    struct_gkn_dta.ec5 = line.substring(index1 + 1);
    Serial.println(struct_gkn_dta.ec5);     
  } else if (line.startsWith(ec6)){
    struct_gkn_dta.ec6 = line.substring(index1 + 1);
    Serial.println(struct_gkn_dta.ec6);     
  } else if (line.startsWith(te525)){
    struct_gkn_dta.te525 = line.substring(index1 + 1);
    Serial.println(struct_gkn_dta.te525);
  } else {
    Serial.println(line);    
  }
  
  return struct_gkn_dta;
}

void sendLine(char* line,int inLen,int blinks){
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t  len = sizeof(buf);
  int sendCounter = 0;
  do{
    randomSeed(analogRead(randomPin));
    int del = random(1,30) * 100;//random delay between 100 - 3000 ms
    Serial.print("..........................................");
    Serial.println(del);
    for(int i=0;i<blinks;i++)blinkled();//blink 
    
  //send data
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
      Serial.println("No reply, is there a listener around?");
    }

  //check if # of retries is still valid or until an ACK is rcvd
    sendCounter++;
    if(sendCounter > sendTrials-1) break;
  }while(strcmp((char*)buf,"ACK"));
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

