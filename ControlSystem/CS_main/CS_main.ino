/* Include Libraries */
#include <Wire.h>

/* Define constants */
#define    LIDARLite_ADDRESS   0x62          // Default I2C Address of LIDAR-Lite.
#define    RegisterMeasure     0x00          // Register to write to initiate ranging.
#define    MeasureValue        0x04          // Value to initiate ranging.
#define    RegisterHighLowB    0x8f          // Register to get both High and Low bytes in 1 call.
#define    BUFF_SIZE 7 //size of buffer for time and distance measurements
#define    circum 131.94//cm
#define    rxn_time 4.8//seconds
#define    decel_max 223.5//cm/s^2 (assumption)

/* Global Variables */
long reading = 0;
volatile unsigned int quarter_revolutions;
float rps;
float v1 =0;
unsigned long timeold;//in milliseconds
float current_time=0; //in milliseconds
float time_buff[7];//in sedconds
volatile float lidar_distance[7];//cmeters
unsigned int   values_read=0;//current amount of values read from lidar
unsigned int   old_values_read=0;//amount of values read since main loop last executed
float rel_vel = 0;// cm/s
float dummy_vel = 0;
float t_stop = 0;//s
float v_stop_avg = 0;
float d_stop = 0;
float t_collision = 0;

/* Function definitions */
void adc_setup ()
{
  NVIC_EnableIRQ (ADC_IRQn) ;   // enable ADC interrupt vector
  ADC->ADC_IDR = 0xFFFFFFFF ;   // disable interrupts
  ADC->ADC_IER = 0x80 ;         // enable AD7 End-Of-Conv interrupt (Arduino pin A0)
  ADC->ADC_CHDR = 0xFFFF ;      // disable all channels
  ADC->ADC_CHER = 0x80 ;        // enable just A0
  ADC->ADC_CGR = 0x15555555 ;   // All gains set to x1
  ADC->ADC_COR = 0x00000000 ;   // All offsets off
  
  ADC->ADC_MR = (ADC->ADC_MR & 0xFFFFFFF0) | (1 << 1) | ADC_MR_TRGEN ;  // 1 = trig source TIO from TC0
}

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

void adc_timer()
{
  pmc_enable_periph_clk (TC_INTERFACE_ID + 0*3+0) ;  // clock the TC0 channel 0

  TcChannel * t = &(TC0->TC_CHANNEL)[0] ;    // pointer to TC0 registers for its channel 0
  t->TC_CCR = TC_CCR_CLKDIS ;  // disable internal clocking while setup regs
  t->TC_IDR = 0xFFFFFFFF ;     // disable interrupts
  t->TC_SR ;                   // read int status reg to clear pending
  t->TC_CMR = TC_CMR_TCCLKS_TIMER_CLOCK1 |   // use TCLK1 (prescale by 2, = 42 MHz)
              TC_CMR_WAVE |                  // waveform mode
              TC_CMR_WAVSEL_UP_RC |          // count-up PWM using RC as threshold
              TC_CMR_EEVT_XC0 |     // Set external events from XC0 (this setup TIOB as output)
              TC_CMR_ACPA_CLEAR | TC_CMR_ACPC_CLEAR |
              TC_CMR_BCPB_CLEAR | TC_CMR_BCPC_CLEAR ;
  
  t->TC_RC =  420000 ;     // counter resets on RC, so sets period in terms of 42MHz clock
  t->TC_RA =  210000 ;     // roughly square wave
  t->TC_CMR = (t->TC_CMR & 0xFFF0FFFF) | TC_CMR_ACPA_CLEAR | TC_CMR_ACPC_SET ;  // set clear and set from RA and RC compares
  
  t->TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG ;  // re-enable local clocking and switch to hardware trigger source.
}

/* Set up timers, hall sensors, Lidar and ADC */
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
  
  adc_setup (); // setup ADC
  adc_timer(); // setup timer that triggers ADC conversion
  startTimer(TC2, 1, TC7_IRQn, 20); // start Lidar timer interrupt
}

/* Main loop */
void loop()
{
  //queue initially filled with zeroes; wait on queue to be filled before calculation
  if (values_read > BUFF_SIZE && values_read!=old_values_read)
  {
    dummy_vel=0;
    rel_vel=0;//init to 0 every time so that it does not continually increase
    for (int i=0;i<BUFF_SIZE;++i)
    {
      float delta_d=(lidar_distance[i+1]-lidar_distance[i]);
      float delta_t=(time_buff[i+1]-time_buff[i]);
      float delta_v=(delta_d/delta_t);
      rel_vel+=delta_v;
      //dummy_vel+=delta_v;//for collision function
    }
    
    rel_vel=rel_vel/(BUFF_SIZE-1);//minus 1, we lose a value from the difference of distance and time
    //dummy_vel = dummy_vel/(BUFF_SIZE-1);
    //t_collision = lidar_distance[BUFF_SIZE-1]/dummy_vel;
      
      
    Serial.print("Relative Velocity(cm/s) : ");
    Serial.println(rel_vel);
    Serial.print("\n");
    //Serial.print("dummyV(cm/s) : ");
    //Serial.println(dummy_vel);
    //Serial.print("\n");
    
    //Serial.print("colTime: ");
    //Serial.println(t_collision);
    old_values_read=values_read;
      
    //Serial.print("dist: ");
    //Serial.println(reading);
     
    if (quarter_revolutions >=4) 
    { //calculate revolution per second using 4 magnets
      rps = quarter_revolutions*250/(float)((millis() - timeold));
      v1 = rps*circum;//vehicle speed
      timeold = millis();
      quarter_revolutions = 0;
      v_stop_avg = v1/2;
      t_stop = (-v1)/(-decel_max);//time to stop based on estimated max decel
      d_stop = v_stop_avg*t_stop;//distance to stop
      //collision ();//Call function to detect if collision may occur
      Serial.print("Velocity(cm/s): ");
      Serial.println(v1);
      Serial.print("\n");
      Serial.print("stopping distance: ");
      Serial.println(d_stop);
      Serial.print("\n");
    }
      
  
  }
    
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
    current_time = millis();
    
    if(reading < 3500 && reading > 7){//set max (35 meters) and min (.05 meters) distance values to disregard distance errors
      //reset rel_vel calculation if unrealistic change in distance occurs
      if((lidar_distance[BUFF_SIZE-1]-reading) > 100 || (lidar_distance[BUFF_SIZE-1]-reading) < -100){
        values_read = 0;
      }
      for (int i=0;i<BUFF_SIZE-1;++i){//shift all values in queues left
        time_buff[i]=time_buff[i+1];
        lidar_distance[i]=lidar_distance[i+1];
      }  
      //assign new values to end of queue
      time_buff[BUFF_SIZE-1] = (float)current_time/1000;
      lidar_distance[BUFF_SIZE-1] = (float)reading;
    
      Serial.print("Distance(cm): ");
      Serial.println(reading); // print distance
      
      ++values_read;//increment values read
    }
 }

 void magnet_detect()//This function is called whenever a north pole magnet/interrupt is detected by the hall sensor
 {
   quarter_revolutions++;
 }

 void magnet_detect2()//called when south pole detected by hall sensor. hall o/p set to low
 {
   quarter_revolutions++;
 }
 
 #ifdef __cplusplus
extern "C" 
{
#endif

void ADC_Handler (void)
{
  if (ADC->ADC_ISR & ADC_ISR_EOC7)   // ensure there was an End-of-Conversion and we read the ISR reg
  {
    int val = *(ADC->ADC_CDR+7) ;    // get conversion result
    Serial.print("ADC value: ");
    Serial.println(val);
    Serial.print('time: ');
    Serial.println(micros());
  }
}

#ifdef __cplusplus
}
#endif

     
     
   




 

 
 
 
 
 

