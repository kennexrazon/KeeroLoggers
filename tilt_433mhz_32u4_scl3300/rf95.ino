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
/*
bool ack2(char *reply)
{
  if ((reply != NULL) && (reply[0] == '\0'))
  {
    Serial.println("Empty reply buffer.");
    return false;
  }
  else
  {
    return true;
  }

  // char* pch;
  // pch = strstr(reply,"ACK");
  // if (pch != NULL){
  //   return true;
  // } else {
  //   Serial.println("No ACK found.");
  //   return true;
  //   // return false;
  // }
}
*/
/*  //original code
void sendLine(char *line, int blinks)
{
  // uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);
  char received[100];

  int retry = 0;
  int del = 1500;
  int trueLen = 0;

  do
  {
    //    assignNull(buf); // di daw kailangang i-clear ang integer array / buffer
    Serial.print("..........................................");
    Serial.println(del);
    for (int i = 0; i < blinks; i++)
      blinkled();

    Serial.println("Sending...");
    delay(10);

    append_retry(line, retry);
    trueLen = strlen(line) + 1;

    Serial.print("Sending: ");
    Serial.println(line);

    // rf95.send((uint8_t *)line, trueLen);
    if (!(rf95.send((uint8_t *)line, trueLen)))
    {
      Serial.println("CAD timeout before clear channel was detected!");
    }
    rf95.waitPacketSent();

    // Now wait for a reply
    Serial.println("Waiting for reply...");
    delay(10);
    if (rf95.waitAvailableTimeout(del))
    {
      // Should be a reply message for us now
      if (rf95.recv(buf, &len))
      {
        // Serial.print(">>: ");
        // Serial.println((char *)buf);
        // Serial.print("RSSI: ");
        // Serial.println(rf95.lastRssi(), DEC);
        int i = 0;
        for (i = 0; i < len; i++)
        {
          received[i] = (uint8_t)buf[i];
        }
        received[i] = (uint8_t)'\0'; //NULL terminate
      }
      else
      {
        Serial.println("Receive failed");
      }
      // Serial.print("Received: ");
      // Serial.println(received);
      Serial.print("RSSI: ");
      Serial.println(rf95.lastRssi(), DEC);
    }

    Serial.print("retry count: ");
    Serial.println(retry);
    retry++;
    delay(300); //kelangan may delay

    // }while(strcmp((char*)buf,"ACK") && (retry < SEND_RETRY_LIMIT));
    } while (!ack3((char *)buf) && (retry < SEND_RETRY_LIMIT)); //orig
    // } while (ackNEW(received) && (retry < SEND_RETRY_LIMIT));
}
*/

/*
void sendLine(char *line, int blinks)
{
  // uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);
  char received[100];

  int retry = 0;
  int del = 1000;
  int trueLen = 0;

  for (int ii = 0; ii < SEND_RETRY_LIMIT; ii++)
  {
    Serial.println("Starting . . .");

    for (int iii = 0; iii < blinks; iii++)
      blinkled();
    assignNull(received); //clear buffer received

    // append_retry(line, retry);
    trueLen = strlen(line) + 1;

    Serial.print("Sending: ");
    Serial.println(line);

    rf95.send((uint8_t *)line, trueLen); //send data
    // Serial.println("Waiting for packet to complete...");
    delay(10);
    rf95.waitPacketSent();

    Serial.print("Waiting for GATEWAY reply . . . ");
    Serial.print("Receive timeout: ");
    Serial.println(del);
    if (rf95.waitAvailableTimeout(del))
    {
      // Should be a reply message for us now
      if (rf95.recv(buf, &len))
      {
        int i = 0;
        for (i = 0; i < len; i++)
        {
          received[i] = (uint8_t)buf[i];
        }
        received[i] = (uint8_t)'\0'; //NULL terminate

        if (ackNEW(received))
        {
          break;
        }
        // Serial.print("Got reply: ");
        // Serial.println((char *)buf);
        Serial.print("Gateway RSSI: ");
        Serial.println(rf95.lastRssi(), DEC);
      }
      else
      {
        Serial.println("Receive failed");
      }
    }
    else
    {
      Serial.println("No reply from GATEWAY, is there a listener around?");
    }
    retry++;
    delay(100);
  }
  Serial.println("..........................................");
}
*/
/*
void sendLine2(char *line, int blinks){
  uint8_t len = sizeof(buf);
  char received[100];
  int retry = 0;
  int del = 1000;
  int trueLen = 0;
  bool correct_ack = false;
  int start = millis();

  for (int ii = 0; ii < SEND_RETRY_LIMIT; ii++)
  {
      for (int iii = 0; iii < blinks; iii++)
          blinkled();
      assignNull(received); //clear buffer received
      trueLen = strlen(line) + 1;
      Serial.print("Sending: ");
      Serial.println(line);

      rf95.send((uint8_t *)line, trueLen); //send data
      delay(10);
      rf95.waitPacketSent();
      // Serial.println(retry);
      Serial.print("Waiting for GATEWAY reply . . . ");
      // Serial.println(del);
      
      do {
          // Should be a reply message for us now
            start = millis();
            assignNull(buf);
                rf95.waitAvailableTimeout(del);
                if (rf95.recv(buf, &len)){
                    int i = 0;
                    for (i = 0; i < len; i++) {
                        received[i] = (uint8_t)buf[i];
                    }
                    received[i] = (uint8_t)'\0'; //NULL terminate

                    if (ackNEW(received)) {
                        correct_ack = true;
                        ii = SEND_RETRY_LIMIT;
                        break;
                    } 
                    Serial.print("\t\t\t\t::");
                    Serial.println((char *)buf);
                    // Serial.print("Gateway RSSI: ");
                    // Serial.println(rf95.lastRssi(), DEC);
                } 
        } while ( correct_ack || ( ( (millis() - start) < 5000 ) ) );
      retry++;
      // Serial.println("++++");
  }
  Serial.println("..........................................");
}
*/
void sendLine2(char *line, int blinks){
  uint8_t len = sizeof(buf);
  char received[100];
  int retry = 0;
  int del = 1000;
  int trueLen = 0;
  bool correct_ack = false;
  unsigned long start = millis();

  for (int ii = 0; ii < SEND_RETRY_LIMIT; ii++)
  {
      for (int iii = 0; iii < blinks; iii++)
          blinkled();
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
          // Should be a reply message for us now
            // rf95.setModeRx();
            
            assignNull2(buf);
                rf95.waitAvailableTimeout(del);
                if (rf95.recv(buf, &len)){
                    int i = 0;
                    for (i = 0; i < len; i++) {
                        received[i] = (uint8_t)buf[i];
                    }
                    received[i] = (uint8_t)'\0'; //NULL terminate

                    if (ackNEW2(received)) {
                        correct_ack = true;
                        ii = SEND_RETRY_LIMIT;
                        break;
                    } 
                    Serial.print("\t\t\t\t\t\t::");
                    Serial.println((char *)buf);
                    // Serial.print("Gateway RSSI: ");
                    // Serial.println(rf95.lastRssi(), DEC);
                } else {
                  Serial.println(".");
                }
        } while ( correct_ack  || ( ( (millis() - start) < 3000 ) ) );
      retry++;
      // Serial.println("++++");
  }
  Serial.println("..........................................");
}


void sendLine3(char *line, int blinks){
  uint8_t len = sizeof(buf);
  char received[100];
  int retry = 0;
  int del = 1000;
  int trueLen = 0;
  bool correct_ack = false;
  unsigned long start = millis();

  for (int ii = 0; ii < SEND_RETRY_LIMIT; ii++)
  {
      for (int iii = 0; iii < blinks; iii++)
          blinkled();
      assignNull(received); //clear buffer received
      trueLen = strlen(line) + 1;
      Serial.print("Sending: ");
      Serial.println(line);

      rf95.send((uint8_t *)line, trueLen); //send data
      delay(10);
      rf95.waitPacketSent();
      // Serial.println(retry);
      Serial.println("Waiting for GATEWAY:SMS reply . . . ");
      // Serial.println(del);
      start = millis();
      do {
          // Should be a reply message for us now
            // rf95.setModeRx();
            
            assignNull2(buf);
                rf95.waitAvailableTimeout(del);
                if (rf95.recv(buf, &len)){
                    int i = 0;
                    for (i = 0; i < len; i++) {
                        received[i] = (uint8_t)buf[i];
                    }
                    received[i] = (uint8_t)'\0'; //NULL terminate

                    if (ackNEW3(received)) {
                        correct_ack = true;
                        ii = SEND_RETRY_LIMIT;
                        break;
                    } 
                    Serial.print("\t\t\t\t\t\t::");
                    Serial.println((char *)buf);
                    // Serial.print("Gateway RSSI: ");
                    // Serial.println(rf95.lastRssi(), DEC);
                } else {
                  Serial.println(".");
                }
        } while ( correct_ack  || ( ( (millis() - start) < 3000 ) ) );
      retry++;
      // Serial.println("++++");
  }
  Serial.println("..........................................");
}

/*
bool ackNEW(char *reply)
{
  char searchACK[100];
  assignNull(searchACK);

  String rcvRply = "ACK";
  String newSenseId = String(SENSEID);
  String finalSenseID = rcvRply + newSenseId;
  finalSenseID.toCharArray(searchACK, 100);

  if (strstr(reply, searchACK))
  {
    Serial.print("Received correct ack: ");
    Serial.println(searchACK);
    return true;
  }
  else
  {
    Serial.println("NO ACK!");
    return false;
  }
}
*/
bool ackNEW2(char *reply)
{
  char searchACK[100];
  assignNull(searchACK);
  String rcvRply = "ACKAXL";
  String newSenseId = String(SENSEID);
  String finalSenseID = rcvRply + newSenseId;
  finalSenseID.toCharArray(searchACK, 100);

  if (strstr(reply, searchACK))
  {
    Serial.print("Received correct ack: ");
    Serial.println(searchACK);
    return true;
  }
  else
  {
    Serial.println("NO ACKAXL!");
    return false;
  }
}

bool ackNEW3(char *reply)
{
  char searchACK[100];
  assignNull(searchACK);
  String rcvRply = "ACKSMS";
  String newSenseId = String(SENSEID);
  String finalSenseID = rcvRply + newSenseId;
  finalSenseID.toCharArray(searchACK, 100);

  if (strstr(reply, searchACK))
  {
    Serial.print("Received correct ack: ");
    Serial.println(searchACK);
    return true;
  }
  else
  {
    Serial.println("NO ACKSMS!");
    return false;
  }
}

/*
bool ack(char *reply)
{
  char *pch;
  pch = strstr(reply, "ACK1");
  if (pch != NULL)
  {
    return true;
  }
  else
  {
    // Serial.println("No ACK found.");
    // return true;
    return false;
  }
}

bool ack3(char *reply)
{
  char *pch;
  char ack[6];
  assignNull(ack);
  char tmpString[4] = "ACK";
  strcat(ack, tmpString);
  strncat(ack, SENSEID, 2);

  pch = strstr(reply, ack);
  if (pch != NULL)
  {
    return true;
  }
  else
  {
    // Serial.println("No ACK found.");
    // return true;
    return false;
  }
}
*/
/*
void sendLine(char* line,int inLen,int blinks){
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t  len = sizeof(buf);
  int retry = 0;
  do{
    // randomSeed(analogRead(1));
    // int del = random(1500,2000);
    int del = 1500;

    Serial.print("..........................................");
    Serial.println(del);
    for(int i=0;i<blinks;i++)blinkled();
    
    Serial.print("Sending "); Serial.println(line);
    Serial.println("Sending...");
    delay(10);
    if (!(rf95.send((uint8_t *)line, inLen))){
      Serial.println("CAD timeout before clear channel was detected!");
    }
  
    // Serial.println("Waiting for packet to complete..."); 
    // delay(10);
    rf95.waitPacketSent();
  
    // Now wait for a reply
    Serial.println("Waiting for reply...");
    if (rf95.waitAvailableTimeout(del)) // bakit random ito? di kailangan na random ito      
    { 
      // Should be a reply message for us now   
      if (rf95.recv(buf, &len))
     {
        Serial.print(">>: ");
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
      retry++;
      Serial.print("No reply, is there a listener around?");
      Serial.print("\t retry: ");
      Serial.println(retry);
      
    }
  // }while(strcmp((char*)buf,"ACK") && (retry < SEND_RETRY_LIMIT));
  }while(!ack((char*)buf) && (retry < SEND_RETRY_LIMIT));
}

*/
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

void append_retry(char *line, int retry)
{
  char tmpString[3];
  assignNull(tmpString);
  strcat(line, ",crc");
  dtostrf(retry, 1, 0, tmpString); // removed extra white space
  strcat(line, tmpString);
}

void blinkled()
{
  digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
  delay(100);                      // wait for a second
  digitalWrite(LED_BUILTIN, LOW);  // turn the LED off by making the voltage LOW
  delay(100);                      // wait for a second
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
