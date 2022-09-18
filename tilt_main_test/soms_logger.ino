
// hs-10
float get_soms_VWC(int pin, int soms_type)
{
      float somsADC = 0.0;
      float soms_avg_vwc = 0.0;
      float somsVWC = 0.0;
      int samples = 10;

      
      delay(250);

      for (int i = 0; i < 10; i++){
        delay(50);
        somsADC = (float)(analogRead(pin));
        // delay(100);
        soms_avg_vwc = soms_avg_vwc + somsADC;
      }
       soms_avg_vwc = (soms_avg_vwc/1023.0) * 3.30 / 10.0;

      if (soms_type == 2) {// HC-10
          somsVWC = (7.39 * soms_avg_vwc * soms_avg_vwc * soms_avg_vwc) +
                    (-14.2 * soms_avg_vwc * soms_avg_vwc) + (9.42 * soms_avg_vwc) - 2.0;

      } else if ( soms_type == 1 ){ // ECHO-5
          
          somsVWC = 0.0242 + (-0.465*soms_avg_vwc) + (1.31*soms_avg_vwc*soms_avg_vwc);
      }

      return somsVWC;
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
