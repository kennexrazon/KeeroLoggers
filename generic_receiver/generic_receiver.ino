#include <SPI.h>
#include <RH_RF95.h>

#if defined (ARDUINO_SAMD_ZERO)
  #include <avr/dtostrf.h>
  #include <FlashStorage.h>
  #define FLASH_DEBUG 0
  #define RFM95_CS 8
  #define RFM95_RST 4
  #define RFM95_INT 3
  #define VBATPIN A7
  // FlashStorage(flashStorage,keyValuePair); 
  #define  MICROCON "M0"
  #define UC_TYPE 2
#elif defined (ARDUINO_AVR_FEATHER32U4)
  #include <EEPROMex.h> 
  #define RFM95_CS 8
  #define RFM95_RST 4
  #define RFM95_INT 7
  #define VBATPIN A9
  #define MICROCON "32U4"
  #define UC_TYPE 1
#endif


 // for feather32u4 

// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 868.0
 
// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);
 
// Blinky on receipt
#define LED 13
 
void setup()
{
  pinMode(LED, OUTPUT);
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
 
  Serial.begin(115200);
//  while (!Serial) {
//    delay(1);
//  }
  delay(4000);
 
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
 
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
 
  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);
}
 
void loop()
{
    if (rf95.available()) {
        // Should be a message for us now
        uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
        uint8_t len = sizeof(buf);
     
        if (rf95.recv(buf, &len))        {
          digitalWrite(LED, HIGH);
      // RH_RF95::printBuffer("Received: ", buf, len);
          Serial.print("Got: ");
          Serial.print((char*)buf);
//          for (int i = 0; i<len; i++){
//              Serial.print((char)buf[i]);
//          }
          Serial.println();
          Serial.print("RSSI: ");
          Serial.println(rf95.lastRssi(), DEC);

        } else {
          Serial.println("Receive failed");
        }
    }
}
