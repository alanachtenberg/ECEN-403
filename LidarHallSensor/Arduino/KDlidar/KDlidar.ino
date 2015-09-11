
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
#define    BUFF_SIZE 5 //size of buffer for time and distance measurements
#define    circum 131.94
long reading = 0;
volatile unsigned int quarter_revolutions;
float rps;
float v1 =0;
unsigned long timeold;//in milliseconds
float current_time=0; //in milliseconds
float time_buff[5];//in sedconds
float lidar_distance[5];//cmeters
unsigned int   values_read=0;//current amount of values read from lidar
unsigned int   old_values_read=0;//amount of values read since main loop last executed
float rel_vel = 0;// cm/s

float decel_max = -223.5; // cm/s^2 (assumption)



void setup()
{
  Wire.begin(); // join i2c bus
  Serial.begin(115200); // start serial communication at 9600bps
  pinMode(12, INPUT);
  pinMode(13, INPUT);
  attachInterrupt(12, magnet_detect, RISING);//Initialize the north pole detection intterrupt(o/p high) pin (Arduino digital pin 12)
  attachInterrupt(13, magnet_detect2, FALLING);//south pole detection interrupt(o/p low)
  quarter_revolutions = 0;
  rps = 0;
  timeold = 0;
  
  /* turn on the timer clock in the power management controller */
  
  pmc_set_writeprotect(false);		 // disable write protection for pmc registers
  pmc_enable_periph_clk(ID_TC7);	 // enable peripheral clock TC7

  /* we want wavesel 01 with RC */
  TC_Configure(/* clock */TC2,/* channel */1, TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC | TC_CMR_TCCLKS_TIMER_CLOCK4); 
  TC_SetRC(TC2, 1, 131200);
  TC_Start(TC2, 1);

  // enable timer interrupts on the timer
  TC2->TC_CHANNEL[1].TC_IER=TC_IER_CPCS;   // IER = interrupt enable register
  TC2->TC_CHANNEL[1].TC_IDR=~TC_IER_CPCS;  // IDR = interrupt disable register

  /* Enable the interrupt in the nested vector interrupt controller */
  /* TC4_IRQn where 4 is the timer number * timer channels (3) + the channel number (=(1*3)+1) for timer1 channel1 */
  NVIC_EnableIRQ(TC7_IRQn);
  
}

  void loop()
  {
    if (quarter_revolutions >=4) { //calculate revolution per second using 4 magnets
       rps = quarter_revolutions*250/(float)((millis() - timeold));
       v1 = rps*circum;//vehicle speed
       timeold = millis();
       quarter_revolutions = 0;
       Serial.print("Velocity(cm/s): ");
       Serial.println(v1);
       Serial.print("\n");
    }
    //queue initially filled with zeroes; wait on queue to be filled before calculation
    if (values_read > 5 && values_read!=old_values_read)
    {
      rel_vel=0;//init to 0 every time so that it does not continually increase
      for (int i=0;i<BUFF_SIZE;++i){
        float delta_d=(lidar_distance[i+1]-lidar_distance[i]);
        float delta_t=(time_buff[i+1]-time_buff[i]);
        float delta_v=(delta_d/delta_t);
        rel_vel+=delta_v;
      }
    
    rel_vel=rel_vel/(BUFF_SIZE-1);//minus 1, we lose a value from the difference of distance and time
    Serial.print("Relative Velocity(cm/s) : ");
    Serial.println(rel_vel);
    Serial.print("\n");
    old_values_read=values_read;
  
    }
  }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// INTERRUPT HANDLERS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  void TC7_Handler()
  {
    // We need to get the status to clear it and allow the interrupt to fire again
    TC_GetStatus(TC2, 1);
    //Serial.println("Interrupt Ack");
      // do nothing timer interrupts will handle the blinking;
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
    current_time = millis();
    if(reading < 3500 && reading > 5){//set max (35 meters) and min (.05 meters) distance values to disregard distance errors
      //reset rel_vel calculation if unrealistic change in distance occurs
      if((lidar_distance[BUFF_SIZE-1]-reading) > 100 || (lidar_distance[BUFF_SIZE-1]-reading) < -100){
        values_read = 0;
      }
      for (int i=0;i<BUFF_SIZE-1;++i){//shift all values in queue left
        time_buff[i]=time_buff[i+1];
        lidar_distance[i]=lidar_distance[i+1];
      }  
      //assign new values to end of queue
      time_buff[BUFF_SIZE-1] = (float)current_time/1000;
      lidar_distance[BUFF_SIZE-1] = (float)reading;
    
      Serial.print("Distance(cm): ");
      Serial.println(reading); // print distance
      
      ++values_read;//increment values read
    }    // We need to get the status to clear it and allow the interrupt to fire again
 }

 void magnet_detect()//This function is called whenever a north pole magnet/interrupt is detected by the hall sensor
 {
   quarter_revolutions++;
   //Serial.println("detect");
   //Serial.println(rps);
 }
 void magnet_detect2()//called when south pole detected by hall sensor. hall o/p set to low
 {
   quarter_revolutions++;
   //Serial.println("detect2");
   //Serial.println(rps);
 }






 

 
 
 
 
 

