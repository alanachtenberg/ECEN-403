void setup()
{
  Serial.begin(115200);
  
}

void loop()
{
  float hdr = 5;
  float msg = 7;
  float spd = 8.5;
  float ftr = 19.3;
    

  Serial.println(hdr);
  Serial.println(msg);
  Serial.println(spd);
  Serial.println(ftr);

}

