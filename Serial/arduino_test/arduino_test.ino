void setup()
{
  Serial.begin(115200);
  
}

void loop()
{
  float hdr = 8;
  float msg = 14;
  float spd = 19;
  float ftr = 21;
    

  Serial.println(hdr);
  //delay(100);
  Serial.println(msg);
  //delay(100);
  Serial.println(spd);
  //delay(100);
  Serial.println(ftr);
  //delay(1000);

}

