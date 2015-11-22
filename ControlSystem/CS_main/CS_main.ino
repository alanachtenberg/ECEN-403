#include <Wire.h>
#include "Collision.h"
#include "ECG.h"

void setup()
{
  Wire.begin(); // join i2c bus
  Serial.begin(115200); // start serial communication at 9600bps
  pinMode(13, INPUT);
  pinMode(9, OUTPUT);
  //attachInterrupt(13, magnet_detect, RISING);//Initialize the intterrupt pin (Arduino digital pin 2)

  adc_setup();// setup ADC
  startADC();
  startTimer(TC2, 1, TC7_IRQn,20); // start Lidar timer interrupt .... delta_t = .18s 
}

double past_vel;
void loop()
{
  while (wait != 1)
  {
    if (values_read > BUFF_SIZE)
    {
      double curr_dist = get_avg_dist();
      push_smooth_distance(curr_dist);
      double smooth_avg = get_smooth_dist();
      //Serial.print("Avg Dist : ");
      //Serial.println(smooth_avg, 4);
      //Serial.print("\n");
      
      calc_push_velocity();
      double curr_vel = get_velocity_avg();
      //Serial.print("Avg Velocity : ");
      //Serial.println(curr_vel, 4);
      //Serial.print("\n");
      
      calc_push_acceleration(curr_vel, past_vel);
      double curr_accel = get_acceleration_avg();
      //Serial.print("Avg Acceleration : ");
      //Serial.println(curr_accel, 4);
      //Serial.print("\n");
      past_vel = curr_vel;

       
      if(curr_vel < 0)// && rel_accel <0)
      {
        calc_push_collision(smooth_avg, curr_vel, curr_accel);
       
        double ttc= get_collision_avg();
        //Serial.print("ttc: ");
        //Serial.println(ttc, 4);
        
        
        /*if (ttc <= 15 && ttc > 3)
          /light_yellow();
        else if (ttc <=3 )
          light_red();
        else
          light_green();*/
      }
      //else
        //light_green();
          
          
      if (quarter_revolutions >=4) 
      { //calculate revolution per second using 4 magnets
        rpm = 60*quarter_revolutions*250/(float)((millis() - timeold));
        v1 = rpm*circum;//vehicle speed
        timeold = millis();
        quarter_revolutions = 0;
        t_stop = (-v1)/(-decel_max);//time to stop based on estimated max decel
        //collision ();//Call function to detect if collision may occur
        //Serial.print("Velocity(cm/s): ");
        //Serial.println(v1);
        //Serial.print("\n");
      }  
            
    }
    
    wait = 1;
    
  }
  
    if (adc_flag == 1)
  {
    if (peak.value > curr_max)
    {
      curr_max = peak.value;
      max_time = peak.tyme;
      
    }
    
    else if (peak.value <= 1250 && peak.value > 1000) // this value needs to be adjusted to what the adc reads at 0 V
    {
      zero_time = peak.tyme;
    }
    
    else
    {
      if (zero_time > max_time && curr_max >= threshold) // some minimal adc value that peaks are never below
      {
        peak_max_array[peak_index].Value = (peak.value/4096)*3.3; // convert to a voltage 
        peak_max_array[peak_index].Tyme = peak.tyme;
        //Serial.print("PV: ");
        //Serial.println(curr_max);
        //Serial.println(peak.tyme);
        //Serial.println(peak_max_array[peak_index].Value);
        VoltageSum = VoltageSum + peak_max_array[peak_index].Value;
        peak_index++;
        curr_max = 1500; // reset to 0 to detect new peaks
        
      }  
    }
    
  adc_flag = 0;
  
  }
  
  if (peak_index == 40)
  {
    // get voltage average to compute low voltage peaks
    VoltageAvg = VoltageSum/40;
    
    for(int i = 0; i < 38; i++) // 38 because we're looking at i + 2
    {
      if(peak_max_array[i+2].Tyme - peak_max_array[i+1].Tyme > (peak_max_array[i+1].Tyme - peak_max_array[i].Tyme) + max_rate_change ||
         peak_max_array[i+2].Tyme - peak_max_array[i+1].Tyme > (peak_max_array[i+1].Tyme - peak_max_array[i].Tyme) - max_rate_change) 
      {
        // missed beat
      }
      
      if(peak_max_array[i].Value <= (VoltageAvg/2)) // low voltage peak if it's less than half
      {
        // low voltage peak
      }
      
    }
    
    // access 39th and 40th indices directly since for loop only goes to 38
    if(peak_max_array[39].Value <= (VoltageAvg/2)) // low voltage peak if it's less than half
    {
      // low voltage peak
    }
    if(peak_max_array[40].Value <= (VoltageAvg/2)) // low voltage peak if it's less than half
    {
      // low voltage peak
    }
    
    // get BPM
    BPM = (40/(peak_max_array[40].Tyme - peak_max_array[0].Tyme))*60000; // Beats/ms * 1000 * 60 = Beats/min
    VoltageSum = 0;
    peak_index = 0;
  }
}
  
void TC7_Handler()
{
  wait = 0;
  // We need to get the status to clear it and allow the interrupt to fire again
 
  Wire.beginTransmission((int)LIDARLite_ADDRESS); // transmit to LIDAR-Lite
  Wire.write((int)RegisterMeasure); // sets register pointer to  (0x00)  
  Wire.write((int)MeasureValue); // sets register pointer to  (0x00)  
  Wire.endTransmission(); // stop transmitting

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
    //Serial.println("RESET");
    //return; //don't think you need/want this
  }
  push_distance_time(distance, curr_time);
  //Serial.print("Distance(m): ");
  //Serial.println(distance); // print distance
  ++values_read;//increment values read
  }   
  TC_GetStatus(TC2, 1); // TC2 = Master Clock (84 MHz)/8
 }

#ifdef __cplusplus
extern "C" 
{
#endif

void ADC_Handler (void)
{
  if (ADC->ADC_ISR & ADC_ISR_EOC7)   // ensure there was an End-of-Conversion and we read the ISR reg
  {
    //int val = *(ADC->ADC_CDR+7) ;    // get conversion result
    //Serial.print("ADC value: ");
    //Serial.println(val);
    //Serial.print('time: ');
    //Serial.println(micros());
    peak.value = *(ADC->ADC_CDR+7) ;    // get conversion result
    peak.tyme = millis();
    
    adc_flag = 1;
  
  }

}

#ifdef __cplusplus
}
#endif

void startADC()
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
  
  t->TC_RC =  42000 ;     // 1000 Hz; counter resets on RC, so sets period in terms of 42MHz clock
  t->TC_RA =  21000 ;     // roughly square wave
  t->TC_CMR = (t->TC_CMR & 0xFFF0FFFF) | TC_CMR_ACPA_CLEAR | TC_CMR_ACPC_SET ;  // set clear and set from RA and RC compares
  
  t->TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG ;  // re-enable local clocking and switch to hardware trigger source.
}

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
     
     
   




 

 
 
 
 
 

