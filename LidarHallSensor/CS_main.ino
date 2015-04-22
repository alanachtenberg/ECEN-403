
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

int reading = 0;
volatile unsigned int quarter_revolutions;
float rps;
unsigned long timeold;
float time_buff[5];
unsigned int lidar_distance[5];
int i=0;
unsigned int relVel = 0;
 
void setup()
{
  Wire.begin(); // join i2c bus
  Serial.begin(9600); // start serial communication at 9600bps
  pinMode(12, INPUT);
  pinMode(13, INPUT);
  attachInterrupt(12, magnet_detect, RISING);//Initialize the intterrupt pin (Arduino digital pin 2)
  attachInterrupt(13, magnet_detect2, FALLING);
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
   if (quarter_revolutions >=4) { 
     rps = quarter_revolutions*250/(float)((millis() - timeold));
     timeold = millis();
     quarter_revolutions = 0;
     Serial.println(rps);
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
  time_buff[i] = 1000*millis();
  //delay(10); // Wait 20ms for transmit

  Wire.requestFrom((int)LIDARLite_ADDRESS, 2); // request 2 bytes from LIDAR-Lite

  if(2 <= Wire.available()) // if two bytes were received
  {
    reading = Wire.read(); // receive high byte (overwrites previous reading)
    reading = reading << 8; // shift high byte to be high 8 bits
    reading |= Wire.read(); // receive low byte as lower 8 bits
    Serial.println(reading); // print the reading
    lidar_distance[i] = reading;
  }
   
  i++;
  
  if (i = 5)
  {
   relVel = 1/5*((lidar_distance[0]/time_buff[0])+(lidar_distance[1]/time_buff[1])+(lidar_distance[2]/time_buff[2])+(lidar_distance[3]/time_buff[3])+(lidar_distance[4]/time_buff[4]));
   i=0;
   Serial.print('Relative Velocity: ');
   Serial.println(relVel);
  }
 

}

 void magnet_detect()//This function is called whenever a magnet/interrupt is detected by the arduino
 {
   quarter_revolutions++;
   //Serial.println("detect");
   //Serial.println(rps);
 }
 void magnet_detect2()
 {
   quarter_revolutions++;
   //Serial.println("detect2");
   //Serial.println(rps);
 }






 

 
 
 
 
 

