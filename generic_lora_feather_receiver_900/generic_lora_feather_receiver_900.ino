#include <SPI.h>
#include <RH_RF95.h>

 /*
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3
#define RF95_FREQ 433.0
 */

 // for feather32u4 
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 7
#define RF95_FREQ 915.0

// Change to 434.0 or other frequency, must match RX's freq!
//#define RF95_FREQ 433.0
 
// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);
 
// Blinky on receipt
#define LED 13
#define RED A0
#define GREEN A1
#define ORANGE A2
#define YELLOW A3
#define WHITE A4

int rssi = 0;
void setup()
{
  pinMode(LED, OUTPUT);
  pinMode(RFM95_RST, OUTPUT);
  pinMode(RED,OUTPUT);
  pinMode(GREEN,OUTPUT);
  pinMode(ORANGE,OUTPUT);
  pinMode(YELLOW,OUTPUT);
  pinMode(WHITE,OUTPUT);  
  digitalWrite(RFM95_RST, HIGH);
 pinMode(10,OUTPUT);
  Serial.begin(115200);
//  while (!Serial) {
//    delay(1);
//  }
  delay(100);
 
  Serial.println("Feather LoRa RX Test!");
 
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
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
  rf95.setTxPower(23, false);
}
 
void loop()
{
    if (rf95.available()) {
        // Should be a message for us now
        uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
        uint8_t len = sizeof(buf);
     
        if (rf95.recv(buf, &len))        {
//          digitalWrite(LED, HIGH);
      // RH_RF95::printBuffer("Received: ", buf, len);
//          Serial.print("Got: ");
//          blinkRed();
          for (int i = 0; i<len; i++){
              Serial.print((char)buf[i]);
          } 
          Serial.println();
          Serial.print("RSSI: ");
          Serial.println(rf95.lastRssi(), DEC);
          rssi = rf95.lastRssi();
          RSSI_BLINK(rssi);        
          if ( strcmp((char*)buf,"ACK") ){      
              uint8_t data[] = "ACK";
              rf95.send(data, sizeof(data));
              rf95.waitPacketSent();
              Serial.println("Sent a reply");
          }
            digitalWrite(LED, LOW);
        } else {
          Serial.println("Receive failed");
        }
    }
}

void RSSI_BLINK(int rssi){

  if ( rssi < -97  ){
    digitalWrite(ORANGE,LOW);
    digitalWrite(GREEN,LOW);
    digitalWrite(YELLOW,LOW);
    digitalWrite(WHITE,LOW);    
    digitalWrite(RED,HIGH);
    delay(250);
    digitalWrite(RED,LOW);  
  } else if ( rssi <= -95 && rssi < -93 ){
    digitalWrite(ORANGE,LOW);
    digitalWrite(RED,LOW);
    digitalWrite(YELLOW,LOW);
    digitalWrite(WHITE,LOW);
    digitalWrite(GREEN,HIGH);
    delay(250);
    digitalWrite(GREEN,LOW);
  } else if ( rssi <= -93 && rssi < -90 ){
    digitalWrite(GREEN,LOW);
    digitalWrite(RED,LOW);
    digitalWrite(YELLOW,LOW);
    digitalWrite(WHITE,LOW);
    digitalWrite(ORANGE,HIGH);
    delay(250);
    digitalWrite(ORANGE,LOW);
  } else if ( rssi <= -90 && rssi < - 86 ){
    digitalWrite(GREEN,LOW);
    digitalWrite(RED,LOW);
    digitalWrite(ORANGE,LOW);
    digitalWrite(WHITE,LOW);
    digitalWrite(YELLOW,HIGH);
    delay(250);
    digitalWrite(YELLOW,LOW);
  } else if ( rssi >= -86 ){
    digitalWrite(GREEN,LOW);
    digitalWrite(RED,LOW);
    digitalWrite(ORANGE,LOW);
    digitalWrite(YELLOW,LOW);
    digitalWrite(WHITE,HIGH);
    delay(250);
    digitalWrite(WHITE,LOW);         
  }
}


