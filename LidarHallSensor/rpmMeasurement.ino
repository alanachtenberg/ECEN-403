/*
Arduino Hall Effect Sensor Project
by Arvind Sanjeev
Please check out  http://diyhacking.com for the tutorial of this project.
DIY Hacking
*/


 volatile unsigned int quarter_revolutions;
 float rps;
 unsigned long timeold;
 void setup()
 {
   Serial.begin(9600);
   attachInterrupt(0, magnet_detect, RISING);//Initialize the intterrupt pin (Arduino digital pin 2)
   attachInterrupt(1, magnet_detect2, FALLING);
   quarter_revolutions = 0;
   rps = 0;
   timeold = 0;
 }
 void loop()//Measure RPM
 {
   if (quarter_revolutions >=4) { 
     rps = quarter_revolutions*250/(float)((millis() - timeold));
     timeold = millis();
     quarter_revolutions = 0;
     //Serial.println(rpm,DEC);
   }
 }
 void magnet_detect()//This function is called whenever a magnet/interrupt is detected by the arduino
 {
   quarter_revolutions++;
   Serial.println("detect");
   Serial.println(rps);
 }
 void magnet_detect2()
 {
   quarter_revolutions++;
   Serial.println("detect2");
   Serial.println(rps);
 }
