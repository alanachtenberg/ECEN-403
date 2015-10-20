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
  Serial.println(msg);
  Serial.println(spd);
  Serial.println(ftr);
}

