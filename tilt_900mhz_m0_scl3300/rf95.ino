void buildID(char* line,char* sensor){
  strncat(line,"-",1);
  strncat(line,SITE,3);
  strncat(line,"-",1);
  strncat(line,sensor,strlen(sensor));
  strncat(line,SENSEID,strlen(SENSEID));
  strncat(line,terminator,1);
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
   assignNull((char *)buf);

    Serial.print("..........................................");
    Serial.println(del);
    for(int i=0;i<blinks;i++)blinkled();
    
    Serial.print("Sending "); Serial.println(line);
    
    Serial.println("Sending...");
    delay(10);

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
  }while(!ack2((char*)buf) && (retry < SEND_RETRY_LIMIT));
}



/*
bool ack(char* reply){
  char* pch;
  pch = strstr(reply,"ACK");
  if (pch != NULL){
    return true;
  } else {
    // Serial.println("No ACK found.");
    // return true;
     return false;
  }
}


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
  strncat(line,tmpString,strlen(tmpString));
  strncat(line,",",1);

  dtostrf(dt.ang_y,5,4,tmpString);
  strncat(line,tmpString,strlen(tmpString));
  strncat(line,",",1);

  dtostrf(dt.ang_z,5,4,tmpString);
  strncat(line,tmpString,strlen(tmpString));
  
  Serial.println(line);
}

void buildLineSMS(char* line, struct lgr_data dt){
  char sensorType[4] = "SMS";
  char tmpString[5];
  buildID(line,"SMS");

  assignNull(tmpString);
  // insert "BTV:" - battery voltage here
  strncat(line,"BTV:",4);
  dtostrf(dt.vol,5,4,tmpString);
  strncat(line,tmpString,strlen(tmpString));
  strncat(line,",",1);
  // insert "BTA:" - Battery amperage ? Current
  strncat(line,"BTA:",4);
  dtostrf(dt.cur,5,4,tmpString);
  strncat(line,tmpString,strlen(tmpString));
  strncat(line,",",1);
  // insert "VWC:"  - volumetric water content
  strncat(line,"VWC:",4);
  dtostrf(dt.soms1,5,4,tmpString);
  strncat(line,tmpString,5);
  strncat(line,",",1);
  // insert "ACT:" - Accel Temperature
  strncat(line,"ACT:",4);
  dtostrf(dt.axelTemp,5,4,tmpString);
  strncat(line,tmpString,5);
  // strcat(line,",");


 
  Serial.println(line);
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
