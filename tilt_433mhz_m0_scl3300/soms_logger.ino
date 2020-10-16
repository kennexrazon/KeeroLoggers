float get_soms_VWC(int pin)
{
  float somsADC = 0.0;
  float soms_avg_vwc = 0.0;
  float somsVWC = 0.0;
  int samples = 10;

  for (int i = 0; i < 10; i++)
  {
  
    somsADC = (analogRead(pin) / 1023.0) * 3.30;

    somsVWC = (2.97 * somsADC * somsADC * somsADC) -
            (7.37 * somsADC * somsADC) + (6.69 * somsADC) - 1.92;


    soms_avg_vwc = soms_avg_vwc + somsVWC;
  }
  return (soms_avg_vwc / 10.0);
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

