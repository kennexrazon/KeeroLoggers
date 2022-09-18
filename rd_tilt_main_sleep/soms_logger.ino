
// hs-10
float get_soms_VWC(int pin, int soms_type)
{
      float somsADC = 0.0;
      float soms_avg_vwc = 0.0;
      float somsVWC = 0.0;
      int samples = 10;

      delay(250);

      Serial.print("soms: ");
      Serial.println(soms_type);

      for (int i = 0; i < 10; i++){
        delay(50);
        somsADC = (float)(analogRead(pin));
        // delay(100);
        soms_avg_vwc = soms_avg_vwc + somsADC;
      }
       soms_avg_vwc = (soms_avg_vwc/1023.0) * 3.30 / 10.0;

      Serial.print("soms avg adc voltage: ");
      Serial.println(soms_avg_vwc);

      if (soms_type == '2') {// HC-10      
        somsVWC = (7.39 * soms_avg_vwc * soms_avg_vwc * soms_avg_vwc) +
                    (-14.2 * soms_avg_vwc * soms_avg_vwc) + (9.42 * soms_avg_vwc) - 2.0;
      }
      else if (soms_type == '1') {// EC-5      
       // somsVWC = (0.14*soms_avg_vwc) - 0.4839;
        somsVWC = 0.0242 + (-0.465*soms_avg_vwc) + (1.31*soms_avg_vwc*soms_avg_vwc);
      }
      else if (soms_type == '3') {// China
        somsVWC = (-0.0187 + (0.51*soms_avg_vwc) - (0.37*soms_avg_vwc*soms_avg_vwc) + (0.088*soms_avg_vwc*soms_avg_vwc*soms_avg_vwc));
      }
      else{
        somsVWC = soms_avg_vwc;
      }

      Serial.print("soms vwc: ");
      Serial.println(somsVWC);

      // data bounds
      if (somsVWC<0) somsVWC=0.0;
      else if (somsVWC>1.10) somsVWC=1.10;
      
      return somsVWC;

      // if (soms_type == 2) {// HC-10
      //     for (int i = 0; i < 10; i++){
      //       somsADC = (analogRead(pin) / 1023.0) * 3.30;
      //       somsVWC = (2.97 * somsADC * somsADC * somsADC) -
      //               (7.37 * somsADC * somsADC) + (6.69 * somsADC) - 1.92;
      //       soms_avg_vwc = soms_avg_vwc + somsVWC;
      //     }
      //     return (soms_avg_vwc / 10.0);


      // } else if ( soms_type == 1 ){ // ECHO-5
          
      //     for (int i=0;i <= 9 ;i++) {
      //       somsADC = analogRead(pin);
      //       somsVWC = (0.0041*somsADC) - 0.4839;
      //       soms_avg_vwc = soms_avg_vwc + somsVWC; 
      //       delay(10);
      //     }
      //     soms_avg_vwc = (soms_avg_vwc / 10.0);
      //     return soms_avg_vwc;
      // }

      // else if (soms_type == 3) { // China SOMS
      //     for (int i = 0; i < 10; i++){
      //       somsADC = (analogRead(pin) / 1023.0) * 3.30;
      //       somsVWC = (-0.0187 + (0.51*somsADC) - (0.37*somsADC*somsADC) + (0.088*somsADC*somsADC*somsADC));
      //       soms_avg_vwc = soms_avg_vwc + somsVWC;
      //       delay(100);
      //     }
      //     return (soms_avg_vwc / 10.0);
      // }

}


float get_cur()
{
  return 0.0;
}

float get_bat_vol()
{

  float measuredvbat = analogRead(VBATPIN);
  measuredvbat *= 2;    // we divided by 2, so multiply back
  measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
  measuredvbat /= 1024; // convert to voltage
  Serial.print("VBat: ");
  Serial.println(measuredvbat);

  return measuredvbat;
}
