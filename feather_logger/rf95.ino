#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 7
#define RF95_FREQ 433.0
#define SEND_TIMEOUT_MS 3000
#define SEND_RETRY_LIMIT 3

RH_RF95 rf95(RFM95_CS, RFM95_INT);

//rf95.node = 1;
//rf95.destination = 2;

void setupRadio(){
  resetRadio();

  while (!rf95.init())
  {
    Serial.println("LoRa radio init failed");
    while (1)
      ;
  }

  if (!rf95.setFrequency(RF95_FREQ))
  {
    Serial.println("setFrequency failed");
  }
  Serial.print("Set Freq to: ");
  Serial.println(RF95_FREQ);

  rf95.setTxPower(20, false);

}

void resetRadio()
{
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
}

void buildID(char *line, char *sensor)
{
  assignNull(line);
  strcpy(line, AREA);

  strcat(line, "-");
  strcat(line, SITE);
  strcat(line, "-");
  strcat(line, sensor);
  strcat(line, SENSEID);
  strcat(line, terminator);
}

void sendLine(char *line, String ackstr){
  uint8_t len = sizeof(buf);
  char received[100];
  int retry = 0;
  int del = 1000;
  int trueLen = 0;
  bool correct_ack = false;
  unsigned long start = millis();

  Serial.println("**************************");
  retry = 0;
  do{
    blinkled(3);
    assignNull(received); //clear buffer received
    trueLen = strlen(line) + 1;
    
    Serial.print("Sending: ");
    Serial.println(line);
    
    rf95.send((uint8_t *)line, trueLen); //send data
    delay(10);
    rf95.waitPacketSent();
    // Serial.println(retry);
    Serial.println("Waiting for GATEWAY:AXL reply . . . ");
    // Serial.println(del);
    start = millis();
    do {
      
      assignNull(buf);
      rf95.waitAvailableTimeout(del);

      if (rf95.recv(buf, &len)){
        int rec_ptr = 0;
        for (rec_ptr = 0; rec_ptr < len; rec_ptr++) {
            received[rec_ptr] = (uint8_t)buf[rec_ptr];
        }
        received[rec_ptr] = (uint8_t)'\0'; //NULL terminate

        if (ackNEW(received, ackstr)) {
            correct_ack = true;
            break;
        } 
        Serial.print("\t-->");
        Serial.println((char *)buf);
        // Serial.print("Gateway RSSI: ");
        // Serial.println(rf95.lastRssi(), DEC);
      } else {
        Serial.print(".");
        delay(10);
      }
    } while((millis()-start)<SEND_TIMEOUT_MS);

    if (correct_ack){
      Serial.println("Message sent!");
      break;
    }
    else{

      retry++;
      Serial.print("No reply received.. Retrying(");
      Serial.print(retry);
      Serial.println(")");
      append_retry(line, retry);
      delay(500);
    }
    // Serial.println("++++");
  }while (retry<SEND_RETRY_LIMIT);

  if (!correct_ack)
    Serial.println("Message sending failed.");
  
  Serial.println("..........................................");
}

bool ackNEW(char *reply, String ackstr)
{
  char searchACK[100];
  assignNull(searchACK);
  String rcvRply = ackstr;
  String newSenseId = String(SENSEID);
  String finalSenseID = rcvRply + newSenseId;
  finalSenseID.toCharArray(searchACK, 100);

  if (strstr(reply, searchACK))
  {
    Serial.print("Received correct ack!");
    return true;
  }
  else
  {
    Serial.println("Incorrent ACK");
    return false;
  }
}


void append_retry(char *line, int retry)
{
  char tmpString[3];
  assignNull(tmpString);
  strcat(line, "+");
}

void assignNull(char *txt)
{
  for (int i = 0; i < strlen(txt); i++)
  {
    txt[i] = '\0';
  }
}

void assignNull2(uint8_t *txt)
{
  for (int i = 0; i < sizeof(txt); i++)
  {
    txt[i] = '\0';
  }
}
