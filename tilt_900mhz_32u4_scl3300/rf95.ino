void buildID(char* line,char* sensor){
  strcat(line,"-");
  strcat(line,SITE);
  strcat(line,"-");
  strcat(line,sensor);
  strcat(line,SENSEID);
  strcat(line,terminator);
}

bool ack2(char* reply){
  if ((reply != NULL) && (reply[0] == '\0')) {
    Serial.println("Empty reply buffer.");
    return false;
  } else {
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



void sendLine(char* line,int inLen,int blinks){
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t  len = sizeof(buf);
  int retry = 0;
  int del = 1500;

  do{
//    assignNull(buf); // di daw kailangang i-clear ang integer array / buffer

    Serial.print("..........................................");
    Serial.println(del);
    for(int i=0;i<blinks;i++)blinkled();
    
    Serial.print("Sending "); Serial.println(line);
    
    Serial.println("Sending...");
    delay(10);

    append_retry(line,retry);
    if (!(rf95.send((uint8_t *)line, inLen))){
      Serial.println("CAD timeout before clear channel was detected!");
    }
  
    rf95.waitPacketSent();

  
    // Now wait for a reply
    Serial.println("Waiting for reply...");
    if (rf95.waitAvailableTimeout(del))   
    { 
      // Should be a reply message for us now   
      if (rf95.recv(buf, &len))
      {
        Serial.print(">>: ");
        Serial.println((char*)buf);
        Serial.print("RSSI: ");
        Serial.println(rf95.lastRssi(), DEC);    
      } else {
        Serial.println("Receive failed");
      }
    } 
    Serial.print("\t retry count: ");
    Serial.println(retry);

    retry++;

  // }while(strcmp((char*)buf,"ACK") && (retry < SEND_RETRY_LIMIT));
  }while(!ack3((char*)buf) && (retry < SEND_RETRY_LIMIT));
}




bool ack(char* reply){
  char* pch;
  pch = strstr(reply,"ACK1");
  if (pch != NULL){
    return true;
  } else {
    // Serial.println("No ACK found.");
    // return true;
     return false;
  }
}

bool ack3(char* reply){
  char* pch;
  char ack[6];
  assignNull(ack);
  char tmpString[4] = "ACK";
  strcat(ack,tmpString);
  strncat(ack,SENSEID,2);

  pch = strstr(reply,ack);
  if (pch != NULL){
    return true;
  } else {
    // Serial.println("No ACK found.");
    // return true;
     return false;
  }
}


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
void buildLineAXL(char* line, struct scl_data dt){
  char sensorType[4] = "AXL";
  char tmpString[5];
  buildID(line,sensorType);

  assignNull(tmpString);

  dtostrf(dt.ang_x,5,4,tmpString);
  strcat(line,tmpString);
  strcat(line,",");

  dtostrf(dt.ang_y,5,4,tmpString);
  strcat(line,tmpString);
  strcat(line,",");

  dtostrf(dt.ang_z,5,4,tmpString);
  strcat(line,tmpString);
  
  Serial.println(line);
}

void buildLineSMS(char* line, struct lgr_data dt){
  char sensorType[4] = "SMS";
  char tmpString[5];
  buildID(line,"SMS");

  assignNull(tmpString);
  // insert "BTV:" - battery voltage here
  strcat(line,"BTV:");
  dtostrf(dt.vol,5,4,tmpString);
  strcat(line,tmpString);
  strcat(line,",");
  // insert "BTA:" - Battery amperage ? Current
  strcat(line,"BTA:");
  dtostrf(dt.cur,5,4,tmpString);
  strcat(line,tmpString);
  strcat(line,",");
  // insert "VWC:"  - volumetric water content
  strcat(line,"VWC:");
  dtostrf(dt.soms1,5,4,tmpString);
  strncat(line,tmpString,5);
  strcat(line,",");
  // insert "ACT:" - Accel Temperature
  strcat(line,"ACT:");
  dtostrf(dt.axelTemp,5,4,tmpString);
  strncat(line,tmpString,5);
  // strcat(line,",");
 
  Serial.println(line);
}

void append_retry(char* line, int retry){
  char tmpString[3];
  assignNull(tmpString);

  strcat(line,",crc");
  dtostrf(retry,2,0,tmpString);
  strcat(line,tmpString);

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


