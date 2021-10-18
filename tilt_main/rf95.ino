void buildID(char *line, char *sensor)
{
  assignNull(line);
  strcpy(line, keyValueOutput[0].AREA);

  strcat(line, "-");
  strcat(line, keyValueOutput[0].SITE);
  strcat(line, "-");
  strcat(line, sensor);
  strcat(line, keyValueOutput[0].SENSEID);
  strcat(line, terminator);
}

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
      Serial.println("Waiting for GATEWAY:AXL reply . . . ");
      start = millis();
      do {
          // Should be a reply message for us now
            // rf95.setModeRx();
            
            assignNull2(buf);
                rf95.waitAvailableTimeout(del);
                if (rf95.recv(buf, &len)){
                    Serial.println((char*)buf);
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
                  Serial.println((char*)buf);
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

bool ackNEW2(char *reply)
{
  char searchACK[100];
  assignNull(searchACK);
  String rcvRply = "ACKAXL";
  String newSenseId = String(keyValueOutput[0].SENSEID);
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
  String newSenseId = String(keyValueOutput[0].SENSEID);
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
