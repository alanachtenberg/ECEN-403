
#include <Wire.h>
#define    LIDARLite_ADDRESS   0x62          // Default I2C Address of LIDAR-Lite.
#define    RegisterMeasure     0x00          // Register to write to initiate ranging.
#define    MeasureValue        0x04          // Value to initiate ranging.
#define    RegisterHighLowB    0x8f          // Register to get both High and Low bytes in 1 call.
#define    BUFF_SIZE 20 //size of buffer for time and distance measurements
#define    TTC_BUFF_SIZE 10
#define    circum .0007933//1.2767// meters
#define    rxn_time 2//seconds
#define    decel_max 3.4 // m/s^2     //2.235//cm/s^2 (assumption)

//VARS IN INTERRUPT//
long reading;
double distance;
unsigned long current_time;
double curr_time;

int wait = 1;
unsigned int values_read=0;
double time_buff[BUFF_SIZE];//in seconds
double distance_buff[BUFF_SIZE];//meters
double smooth_dist[BUFF_SIZE];

double velocity_buff[BUFF_SIZE];//meters per second
double acceleration_buff[BUFF_SIZE];//meters per second^2
double collision_buff[BUFF_SIZE];

// Hall Variables 
float t_stop = 1;//s
float v1 = 0;
float rpm;
volatile unsigned int quarter_revolutions;

unsigned long timeold;//in milliseconds

void startTimer(Tc *tc, uint32_t channel, IRQn_Type irq, uint32_t frequency) {
  pmc_set_writeprotect(false);
  pmc_enable_periph_clk((uint32_t)irq);
  TC_Configure(tc, channel, TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC | TC_CMR_TCCLKS_TIMER_CLOCK4);
  uint32_t rc = VARIANT_MCK/128/frequency; //128 because we selected TIMER_CLOCK4 = MCK/128 above
  TC_SetRA(tc, channel, rc/2); //50% high, 50% low (duty cycle)
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
  pinMode(13, INPUT);
  pinMode(12, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(10, OUTPUT);
    
  digitalWrite(10, LOW);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(11, LOW);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(12, LOW);
  attachInterrupt(13, magnet_detect, RISING);//Initialize the intterrupt pin (Arduino digital pin 2)

//  adc_setup (); // setup ADC
//  adc_timer(); // setup timer that triggers ADC conversion
  startTimer(TC2, 1, TC7_IRQn, 20); // start Lidar timer interrupt .... delta_t = .18s
  // TimerClock2, channel 1, IRQ 7 - TC7_Handler, 5Hz sampling rate 
}

double past_vel;
  void loop()
  {

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
        //Serial.print("Velocity(mph): ");
        //Serial.println(time1);
        //Serial.print("\n");
        
//        Serial.print("stopping distance: ");
//        Serial.println(d_stop);
//        Serial.print("\n");
      }
    
    
    while (wait != 1)
    {
      if (values_read > BUFF_SIZE)
      {
        double curr_dist = get_avg_dist();
         push_smooth_distance(curr_dist);
         double smooth_avg = get_smooth_dist();
        //Serial.print("Avg Dist : ");
        //Serial.print(smooth_avg, 4);
        //Serial.print("\n");
        //Serial.print(", ");
        
          
          calc_push_velocity();
          double curr_vel = get_velocity_avg();
          //Serial.print("Avg Velocity : ");
          //Serial.print(curr_vel, 4);
          //Serial.print("\n");
          //Serial.print(", ");

          
          calc_push_acceleration(curr_vel, past_vel);
          double curr_accel = get_acceleration_avg();
          //Serial.print("Avg Acceleration : ");
          //Serial.print(curr_accel, 4);
          //Serial.print("\n");
          //Serial.print(", ");
          past_vel = curr_vel;


         
          if(curr_vel < 0)// && rel_accel <0)
          {
            calc_push_collision(smooth_avg, curr_vel, curr_accel);
           
            double ttc= get_collision_avg();
            //Serial.print("ttc: ");
            //Serial.print(ttc, 4); 
            //Serial.print("\n");
            
            
            if (ttc <= 15 && ttc > 3)
              light_yellow();
            else if (ttc <=3 )
              light_red();
            else
              light_green();
          }
          else
            light_green();
            
      }
      wait = 1;
    }
    
  }
  
  void TC7_Handler()
  {
    //interrupts();
    double times1 = (double)micros();
    Serial.print("t1: ");
    Serial.println(times1);
    wait = 0;
    // We need to get the status to clear it and allow the interrupt to fire again
 
    Wire.beginTransmission((int)LIDARLite_ADDRESS); // transmit to LIDAR-Lite
    Wire.write((int)RegisterMeasure); // sets register pointer to  (0x00)  
    Wire.write((int)MeasureValue); // sets register pointer to  (0x00)  
    Wire.endTransmission(); // stop transmitting\
    delayMicroseconds(20000); // Wait 20ms for transmit
    Wire.beginTransmission((int)LIDARLite_ADDRESS); // transmit to LIDAR-Lite
    Wire.write((int)RegisterHighLowB); // sets register pointer to (0x8f)
    Wire.endTransmission(); // stop transmitting
  
    Wire.requestFrom((int)LIDARLite_ADDRESS, 2); // request 2 bytes from LIDAR-Lite
    while (Wire.available()<2){//waits for two bytes to be available
      //delay(2);//wait two milliseconds
    }   
    reading = Wire.read(); // receive high byte (overwrites previous reading)
    reading = reading << 8; // shift high byte to be high 8 bits
    reading |= Wire.read(); // receive low byte as lower 8 bits
    distance = (double)reading/100 - .5; //convert to meters /100
    current_time = millis(); //convert to seconds /1000
    curr_time = (double)current_time/1000;
    
    
    if(distance < 35)
    {//set max (35 meters) and min (.07 meters) distance values to disregard distance errors 
    //reset rel_vel calculation if unrealistic change in distance occure

    if(((distance_buff[BUFF_SIZE-1]-distance) > 2 || (distance_buff[BUFF_SIZE-1]-distance) < -2)&&values_read!=0)
    {
      values_read = 0;
      for(int i=0;i<BUFF_SIZE;i++){
        distance_buff[i] = 0;
        time_buff[i] = 0;
       
      }
      Serial.println("RESET");
      return;
    }
    push_distance_time(distance, curr_time);
    //Serial.print("Distance(m): ");
    //Serial.println(distance); // print distance
    ++values_read;//increment values read
    }   
    
    //double times2 = (double)micros();
    //Serial.print("t2: ");
    //Serial.println(timmes2);
    //Serial.print("t_diff: ");
   //Serial.println(times2-times1);
    

    TC_GetStatus(TC2, 1); // TC2 = Master Clock (84 MHz)/8
 }

void push_distance_time(double distance, double time){
  for (int i=0;i<BUFF_SIZE-1;++i){//shift all values in queues left
    distance_buff[i]=distance_buff[i+1];
    time_buff[i]=time_buff[i+1];
  }  
  //assign new values to end of queue
  distance_buff[BUFF_SIZE-1] = distance; //meters
  time_buff[BUFF_SIZE-1] = time; //seconds
  
  //Serial.print("time: ");
  //Serial.println(time_buff[BUFF_SIZE-1]-time_buff[BUFF_SIZE-2],4);
}
 
double get_avg_dist(){
  double avg = 0;
  for (int i=0;i<BUFF_SIZE;++i){
    avg += distance_buff[i];
  }  
  return avg/BUFF_SIZE;
}

 void push_smooth_distance(double distance)
 {
     for (int i=0;i<BUFF_SIZE-1;++i){//shift all values in queues left
        smooth_dist[i]=smooth_dist[i+1];
     }  
     if (smooth_dist[BUFF_SIZE-1]-distance >= .04 || smooth_dist[BUFF_SIZE-1]-distance <= -.04) //filter inherent device distance read errors of 4%
        smooth_dist[BUFF_SIZE-1] = distance;     
     else   
        smooth_dist[BUFF_SIZE-1] = smooth_dist[BUFF_SIZE-2];      
 }

double get_smooth_dist(){
  double avg = 0;
  for (int i=0;i<BUFF_SIZE;++i){
    avg += smooth_dist[i];
  }  
  return avg/BUFF_SIZE;
}

void calc_push_velocity(){
     for (int i=0;i<BUFF_SIZE-1;++i){//shift all values in queues left
       velocity_buff[i]=velocity_buff[i+1];
     }  
  velocity_buff[BUFF_SIZE-1] = (smooth_dist[BUFF_SIZE-1]-smooth_dist[BUFF_SIZE-2])/(time_buff[BUFF_SIZE-1]-time_buff[BUFF_SIZE-2]);  
}

double get_velocity_avg(){
  double avg = 0;
  for (int i=0;i<BUFF_SIZE;++i){
    avg += velocity_buff[i];
  }  
  return avg/BUFF_SIZE;
}

void calc_push_acceleration(double curr_vel, double past_vel){
     for (int i=0;i<BUFF_SIZE-1;++i){//shift all values in queues left
       acceleration_buff[i]=acceleration_buff[i+1];
     }  
  acceleration_buff[BUFF_SIZE-1] = (curr_vel-past_vel)/(time_buff[BUFF_SIZE-1]-time_buff[BUFF_SIZE-2]);
  //((smooth_dist[BUFF_SIZE-3]-smooth_dist[BUFF_SIZE-2])-(smooth_dist[BUFF_SIZE-2]-smooth_dist[BUFF_SIZE-1]))/(sq(time_buff[BUFF_SIZE-1]-time_buff[BUFF_SIZE-2]));
  //Serial.print("time: ");
  //Serial.println(time_buff[BUFF_SIZE-1]-time_buff[BUFF_SIZE-2],4);
  //(velocity_buff[BUFF_SIZE-1]-velocity_buff[BUFF_SIZE-2])/(time_buff[BUFF_SIZE-1]-time_buff[BUFF_SIZE-2]);  
}

double get_acceleration_avg(){
  double avg = 0;
  for (int i=0;i<BUFF_SIZE;++i){
    avg += acceleration_buff[i];
  }  
  return avg/BUFF_SIZE;
}

 void calc_push_collision(double d, double v, double a){
   for (int i=0;i<TTC_BUFF_SIZE-1;++i){//shift all values in queues left
       collision_buff[i]=collision_buff[i+1];
     }  
   if (a < -0.5 || a > 0.5) //filter out negligable accelerations
     collision_buff[TTC_BUFF_SIZE-1]= (-v-sqrt(sq(v)-2*a*d))/a;
   else
     collision_buff[TTC_BUFF_SIZE-1] = -(d/v);
 }
double get_collision_avg(){
  double avg = 0;
  for (int i=0;i<TTC_BUFF_SIZE;++i){
    avg += collision_buff[i];
  }  
  return avg/TTC_BUFF_SIZE;
}



  void light_red(){
  digitalWrite(11, LOW);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(12, LOW);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(10, HIGH);   // turn the LED on (HIGH is the voltage level)

 }
void light_yellow(){
  digitalWrite(12, LOW);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(10, LOW);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(11, HIGH);   // turn the LED on (HIGH is the voltage level)
}
void light_green(){
  digitalWrite(10, LOW);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(11, LOW);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(12, HIGH);   // turn the LED on (HIGH is the voltage level)
}


 void magnet_detect()//This function is called whenever a magnet/interrupt is detected by the arduino
 {
   quarter_revolutions++;
   //Serial.println(quarter_revolutions);
   //Serial.println(rpm);
 }

