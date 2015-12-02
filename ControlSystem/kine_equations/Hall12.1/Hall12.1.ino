#define    circum .0007933//1.2767// meters
// Hall Variables 
float t_stop = 1;//s
float v1 = 0;
float rpm;
volatile unsigned int quarter_revolutions;

unsigned long timeold;//in milliseconds


void setup() {
  Serial.begin(115200); // start serial communication at 9600bps
  pinMode(13, INPUT);
  attachInterrupt(13, magnet_detect, RISING);
}

void loop() {
                        if (quarter_revolutions == 1) 
      { //calculate revolution per second using 4 magnets
        float time1 = float(millis()-timeold);
        rpm = 60*quarter_revolutions*1000/(time1);
        //rpm = 60*quarter_revolutions*250/(float((millis()-timeold));
        //v1 = rpm*circum*60;
        v1 = rpm*circum*60/4;//vehicle speed
        //float v2 = v1/2.23694;
        timeold = millis();
        quarter_revolutions = 0;
//        v_stop_avg = v1/2;
//        t_stop = (-v1)/(-decel_max);//time to stop based on estimated max decel
//        d_stop = v_stop_avg*t_stop;//distance to stop
        //collision ();//Call function to detect if collision may occur
        Serial.print("Velocity(mph): ");
        Serial.println(time1);
        Serial.print("\n");
        
//        Serial.print("stopping distance: ");
//        Serial.println(d_stop);
//        Serial.print("\n");
      }
}


 void magnet_detect()//This function is called whenever a magnet/interrupt is detected by the arduino
 {
   quarter_revolutions++;
   //Serial.println(quarter_revolutions);
   //Serial.println(rpm);
 }
