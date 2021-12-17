void buildID(char *line, char *sensor)
{
  assignNull(line,RH_RF95_MAX_MESSAGE_LEN);
  strcpy(line, this_sensor.area);

  strcat(line, "-");
  strcat(line, this_sensor.site);
  strcat(line, "-");
  strcat(line, sensor);
  strcat(line, this_sensor.sense_id);
  strcat(line, terminator);
}

void sendLine4(char *line, int blinks, char *ackPrefix){
  // uint8_t len = sizeof(buf);
  int retry = 0;
  int del = 1000;
  int trueLen = 0;
  bool correct_ack = false;
  unsigned long start = millis();

  // do{
  //   for (int iii = 0; iii < blinks; iii++) blinkled();

  if (manager.sendtoWait((uint8_t *)line, sizeof(line), SERVER_ADDRESS))
  {
      // // Now wait for a reply from the server
      // uint8_t len = sizeof(buf);
      // uint8_t from;   
      // if (manager.recvfromAckTimeout(buf, &len, 2000, &from))
      // {
      //   // Serial.print("got reply from : 0x");
      //   // Serial.print(from, HEX);
      //   // Serial.print(": ");
      //   Serial.println((char*)buf);

      //   if (ackEval((char *)buf, ackPrefix)) {
      //     correct_ack = true;
      //     ii = SEND_RETRY_LIMIT;
      //     Serial.println(" -> Correct");
      //     break;
      //   }

      //   else{
      //     Serial.println("Not a valid ACK");  
      //   }

      // }
      // else
      // {
      //   Serial.println("No reply, is rf95_reliable_datagram_server running?");
      // }
    }
    else
      Serial.println("sendtoWait failed");

  //   for (int l=0; l<len; l++) buf[l] = '\0';
  //   delay(500);
  // }while (correct_ack);

}


void sendLine3(char *line, int blinks, char *ackPrefix){
  uint8_t len = sizeof(buf);
  int retry = 0;
  int del = 1000;
  int trueLen = 0;
  bool correct_ack = false;
  unsigned long start = millis();

  // uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  // len = sizeof(buf);

  for (int ii = 0; ii < SEND_RETRY_LIMIT; ii++)
  {
    for (int iii = 0; iii < blinks; iii++) blinkled();

    trueLen = strlen(line) + 1;
    Serial.print("Sending: ");
    Serial.println(line);

    rf95.send((uint8_t *)line, trueLen); //send data
    delay(10);
    rf95.waitPacketSent();
    // Serial.println(retry);
    Serial.println("Waiting for GATEWAY reply ... ");
    // Serial.println(del);
    start = millis();
    
    Serial.print("Retry (");
    Serial.print(ii);
    Serial.println("): ");

    do {
        
      rf95.waitAvailableTimeout(del);
      if (rf95.recv(buf, &len)){
        
        Serial.print("Received::");
        Serial.println((char *)buf);

        // if (ackEval(buf, ackPrefix)) {
        if (ackEval((char *)buf, ackPrefix)) {
            correct_ack = true;
            ii = SEND_RETRY_LIMIT;
            Serial.println(" -> Correct");
            break;
        }

        else{
          Serial.println("Not a valid ACK");  
        }

        // Serial.print("Gateway RSSI: ");
        // Serial.println(rf95.lastRssi(), DEC);
        // assignNull(received);
        // assignNull2(buf);
        // memset(0, buf, sizeof(buf));
        for (int l=0; l<len; l++) buf[l] = '\0';
        // buf[0] = char(0);
      } 
      else Serial.print(".");
     
    } while ( correct_ack  || ( ( (millis() - start) < 3000 ) ) );
    retry++;
    if (retry>SEND_RETRY_LIMIT) Serial.println("ERROR: No Correct reply received");
  }
  Serial.println("..........................................");
}

bool ackEval(char *reply, char *rcvRply)
{
  char searchACK[100];
  searchACK[0]=char(0);

  strncat(searchACK, rcvRply, 6);
  strncat(searchACK, this_sensor.sense_id, 3);


  if (strstr(reply, searchACK))
    return true;
  else
    return false;
}

void buildLineAXL(char *line, struct SclData dt)
{
  char sensorType[4] = "AXL";
  char tmpString[5];
  buildID(line, sensorType);

  assignNull(tmpString,5);

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

void buildLineSMS(char *line, struct LgrData dt)
{
  char sensorType[4] = "SMS";
  char tmpString[5];
  buildID(line, "SMS");

  assignNull(tmpString,5);
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
  assignNull(tmpString, 3);
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

void assignNull2(uint8_t *txt)
{
  for (int i = 0; i < sizeof(txt); i++)
  {
    txt[i] = '\0';
  }
}
