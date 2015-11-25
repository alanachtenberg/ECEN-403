char EcgHdr = 'e';
float BPM = 60;
int MbFlag = 1;
int LvpFlag = 1;
float LvpValue = 2;
char EcgFtr = 'f';
char KineHdr = 'k';
float k_val1 = 20;
char KineFtr = 'f';
int zero = 0;

void setup()
{
  Serial.begin(115200);
  
}

void loop()
{
  if (Serial.read = 'E')
  {
    Serial.println(EcgHdr);
    Serial.println(BPM);
    Serial.println(MbFlag);
    Serial.println(LvpFlag);
    Serial.println(LvpValue);
    Serial.println(EcgFtr);    
  }
  
  if (Serial.read = 'K')
  {
    Serial.println(KineHdr);
    Serial.println(k_val1);
    Serial.println(zero);
    Serial.println(zero);
    Serial.println(zero);
    Serial.println(KineFtr); 
  }
  
}

