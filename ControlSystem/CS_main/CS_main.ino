#include <Wire.h>


// These are the clock frequencies available to the timers /2,/8,/32,/128
// 84Mhz/2 = 42.000 MHz
// 84Mhz/8 = 10.500 MHz
// 84Mhz/32 = 2.625 MHz
// 84Mhz/128 = 656.250 KHz
//
// 42Mhz/44.1Khz = 952.38
// 10.5Mhz/44.1Khz = 238.09 
// 2.625Hmz/44.1Khz = 59.5
// 656Khz/44.1Khz = 14.88 // 131200 / 656000 = .2 (.2 seconds)

#include <Wire.h>
#define    LIDARLite_ADDRESS   0x62          // Default I2C Address of LIDAR-Lite.
#define    RegisterMeasure     0x00          // Register to write to initiate ranging.
#define    MeasureValue        0x04          // Value to initiate ranging.
#define    RegisterHighLowB    0x8f          // Register to get both High and Low bytes in 1 call.
#define    BUFF_SIZE 7 //size of buffer for time and distance measurements
#define circum 1.2767// meters
#define    rxn_time 4.8//seconds
#define    decel_max 2.235//cm/s^2 (assumption)
long reading = 0;
float distance = 0;
volatile unsigned int quarter_revolutions;
float rpm;
float v1 = 0;
float v2 = 0;
float v3 = 0;
float timeold;//in milliseconds
float current_time=0; //in milliseconds
float time_btwn = 0;
float revolutions = 0;
float time_buff[BUFF_SIZE];//in sedconds
float distance_buff[BUFF_SIZE];//cmeters
unsigned int   values_read=0;//current amount of values read from lidar
unsigned int   old_values_read=0;//amount of values read since main loop last executed
float rel_vel = 0;// cm/s
float TTC = 0;

float dummy_vel = 0;
float t_stop = 1;//s
float v_stop_avg = 0;
float d_stop = 0;
float t_collision = 0;
float rel_accel = 0;
float rel_vel_buff[BUFF_SIZE-2];

void startTimer(Tc *tc, uint32_t channel, IRQn_Type irq, uint32_t frequency) {
  pmc_set_writeprotect(false);
  pmc_enable_periph_clk((uint32_t)irq);
  TC_Configure(tc, channel, TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC | TC_CMR_TCCLKS_TIMER_CLOCK4);
  uint32_t rc = VARIANT_MCK/128/frequency; //128 because we selected TIMER_CLOCK4 above
  TC_SetRA(tc, channel, rc/2); //50% high, 50% low
  TC_SetRC(tc, channel, rc);
  TC_Start(tc, channel);
  tc->TC_CHANNEL[channel].TC_IER=TC_IER_CPCS;
  tc->TC_CHANNEL[channel].TC_IDR=~TC_IER_CPCS;
  NVIC_EnableIRQ(irq);
}


void setup()
{
  Wire.begin(); // join i2c bus
  Serial.begin(115200); // start serial communication at 9600bps
  pinMode(12, INPUT);
  pinMode(13, INPUT);
  attachInterrupt(13, magnet_detect, RISING);//Initialize the intterrupt pin (Arduino digital pin 2)

  quarter_revolutions = 0;
  rpm = 0;
  timeold = 0;
  
  startTimer(TC2, 1, TC7_IRQn, 5);
}

  void loop()
  {
    //collision(distance, rel_vel, rel_accel);
         if (values_read > BUFF_SIZE && values_read!=old_values_read)
    {
     
      rel_vel=0;//init to 0 every time so that it does not continually increase
      rel_vel = relative_velocity();
  
      Serial.print("Relative Velocity(m/s) : ");
      Serial.println(rel_vel);
      Serial.print("\n");
      old_values_read=values_read;
    
      push_velocity(rel_vel);

      rel_accel=0;
      rel_accel = relative_acceleration();
      Serial.print("Relative Acceleration(m/s^2) : ");
      Serial.println(rel_accel);
      Serial.print("\n");
    user_speed();

       TTC = -rel_vel-(sqrt(sq(rel_vel)-2*rel_accel*(distance/100))/rel_accel);
       Serial.println(TTC);
    
    }
//      if (quarter_revolutions >=4) 
//      { //calculate revolution per second using 4 magnets
//        rps = quarter_revolutions*250/(float)((millis() - timeold));
//        v1 = rps*circum;//vehicle speed
//        timeold = millis();
//        quarter_revolutions = 0;
//        v_stop_avg = v1/2;
//        t_stop = (-v1)/(-decel_max);//time to stop based on estimated max decel
//        d_stop = v_stop_avg*t_stop;//distance to stop
//        //collision ();//Call function to detect if collision may occur
//        Serial.print("Velocity(cm/s): ");
//        Serial.println(v1);
//        Serial.print("\n");
//        Serial.print("stopping distance: ");
//        Serial.println(d_stop);
//        Serial.print("\n");
//      }
      
  
    
    
  }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// INTERRUPT HANDLERS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  void TC7_Handler()
  {
    // We need to get the status to clear it and allow the interrupt to fire again
    TC_GetStatus(TC2, 1);
    Wire.beginTransmission((int)LIDARLite_ADDRESS); // transmit to LIDAR-Lite
    Wire.write((int)RegisterMeasure); // sets register pointer to  (0x00)  
    Wire.write((int)MeasureValue); // sets register pointer to  (0x00)  
    Wire.endTransmission(); // stop transmitting
  
    delayMicroseconds(20000); // Wait 20ms for transmit
  
    Wire.beginTransmission((int)LIDARLite_ADDRESS); // transmit to LIDAR-Lite
    Wire.write((int)RegisterHighLowB); // sets register pointer to (0x8f)
    Wire.endTransmission(); // stop transmitting
  
    //delay(10); // Wait 10ms for transmit
  
    Wire.requestFrom((int)LIDARLite_ADDRESS, 2); // request 2 bytes from LIDAR-Lite
    while (Wire.available()<2){//waits for two bytes to be available
      //delay(2);//wait two milliseconds
    }   
    reading = Wire.read(); // receive high byte (overwrites previous reading)
    reading = reading << 8; // shift high byte to be high 8 bits
    reading |= Wire.read(); // receive low byte as lower 8 bits
    distance = (float)reading; //convert to meters /100

    current_time = millis(); //convert to seconds /1000
   
   
    
    
    if(distance < 3500 && distance > 7){//set max (35 meters) and min (.05 meters) distance values to disregard distance errors
      //reset rel_vel calculation if unrealistic change in distance occurs
//      if((distance_buff[BUFF_SIZE-1]-distance) > 50 || (distance_buff[BUFF_SIZE-1]-distance) < -50){
//        values_read = 0;
//      }
      
      push_distance_time(distance, current_time);
    
      Serial.print("Distance(m): ");
      Serial.println(distance/100); // print distance
      ++values_read;//increment values read
    }    // We need to get the status to clear it and allow the interrupt to fire again
    

 }

 void magnet_detect()//This function is called whenever a magnet/interrupt is detected by the arduino
 {
   quarter_revolutions++;
   //Serial.println(quarter_revolutions);
   //Serial.println(rpm);
 }
 
 void user_speed(){
    if (quarter_revolutions >=4) { 
     rpm = 60*quarter_revolutions*250/(float)((millis() - timeold));
     v1 = rpm*circum; //cm/s
     v2 = v1*.0223694*2; //mph
     v3 = v1*2/100; // m/s
     
     timeold = millis();
     quarter_revolutions = 0;
     revolutions += 1; 
     Serial.println("User Speed (mph): ");
     Serial.println(v2);
     
     
 }
 }
 
 float relative_velocity()
 {
   float delta_d=0;
   float delta_t=0;
   float rel_velo=0;
   float rel_vel_sum = 0;


   for (int i=0;i<BUFF_SIZE-1;++i)
      {
        delta_d=(distance_buff[i+1]-distance_buff[i]);
        delta_t=(time_buff[i+1]-time_buff[i]);
        Serial.print("Delta T: ");
        Serial.println(delta_t);
      
        rel_velo=(delta_d/delta_t);
        rel_vel_sum+=rel_velo;
        
      }
    
    return rel_vel_sum/(BUFF_SIZE-1);

 }
 
  float relative_acceleration()
 {
   float rel_a=0;
   float rel_a_sum = 0;
   float delta_rel_vel=0;
   float delta_t=0;
   for (int i=0;i<BUFF_SIZE-2;++i)
      {
        delta_rel_vel=(rel_vel_buff[i+1]-rel_vel_buff[i]);
        delta_t=(time_buff[i+2]-time_buff[i+1]);
        rel_a=(delta_rel_vel/delta_t);
        rel_a_sum+=rel_a;
        
      }
    
    return rel_a_sum/(BUFF_SIZE-2);

 }
 
 void push_distance_time(float distance, float time )
 {
   for (int i=0;i<BUFF_SIZE-1;++i){//shift all values in queues left
        distance_buff[i]=distance_buff[i+1];
        time_buff[i]=time_buff[i+1];
      }  
      //assign new values to end of queue
      distance_buff[BUFF_SIZE-1] = distance/100;
      time_buff[BUFF_SIZE-1] = time/1000;
  
 }
 
  void push_velocity(float velocity)
 {
   for (int i=0;i<BUFF_SIZE-2;++i){//shift all values in queues left
        rel_vel_buff[i]=rel_vel_buff[i+1];
      }  
      //assign new values to end of que
      rel_vel_buff[BUFF_SIZE-2] = velocity;// convert to seconds
  
 }
 
 
 

 //void collision(float dist, float relVel, float relAccel){
   //TTC = -relVel-sqrt(sq(relVel)-2*relAccel*dist)/relAccel;
   //Serial.println(TTC);

//   if(t_stop < (t_collision+10)){
//     int j = 0;//do nothing
//   }
//   else if(t_stop < (t_collision+5)) {
//     Serial.print("t_stop: ");
//     Serial.println(t_stop);
//     Serial.print("t_collide: ");
//     Serial.println(t_collision);
//     Serial.println("BREAK!!! ");
//     Serial.print(" ");
//   }
//}
     
     
   




 

 
 
 
 
 

