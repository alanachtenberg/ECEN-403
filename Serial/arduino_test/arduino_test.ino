void setup()
{
  Serial.begin(115200);
  
}

void loop()
{
  float hdr = 10;
  float msg = 13;
  float spd = 17;
  float ftr = 20;
    

  Serial.println(hdr);
  Serial.println(msg);
  Serial.println(spd);
  Serial.println(ftr);

}

