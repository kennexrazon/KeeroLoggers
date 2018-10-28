#define BAUD 115200
#define CR1000 Serial1

void setup() {
  Serial.begin(BAUD);
  CR1000.begin(BAUD);
  
  while(!Serial.available());
  send_cr();
}

void loop() {
  Serial.println("."); 
  delay(1000);   
}

void send_cr(){
  String inLine;
  int flag1 = 0;
  String prompt = "CR1000>";
  String ellipsis = " ...";
  int limit = 7;  
  int len = 0;
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
    Serial.println(inLine);
    if (inLine.startsWith(prompt) && flag1 != 2) {
      CR1000.write('7');
      CR1000.write('\r');
      flag1 = 2;
    } else if (inLine.startsWith(ellipsis) && flag1 != 3){
      CR1000.write('\r');
      flag1 = 3;    
    }
     
  }
}

