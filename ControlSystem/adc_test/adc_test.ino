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
  Serial.begin (115200) ; // was for debugging
  
  adc_setup () ;         // setup ADC
  
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

  //startTimer(TC1, 1, TC4_IRQn, 2);
  //startTimer(TC2, 1, TC7_IRQn, 20);
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
  //ADC->ADC_CGR = 0x15559556 ;  
  //ADC->ADC_COR = 0x00000081 ;   
  
  ADC->ADC_MR = (ADC->ADC_MR & 0xFFFFFFF0) | (1 << 1) | ADC_MR_TRGEN ;  // 1 = trig source TIO from TC0

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


void loop() 
{
}

// This function is called every 1/40 sec.
void TC4_Handler()
{
  // You must do TC_GetStatus to "accept" interrupt
  // As parameters use the first two parameters used in startTimer (TC1, 0 in this case)
  TC_GetStatus(TC1, 1);

  int time1 = millis();
  Serial.print("timer 1: ");
  Serial.println(time1);

}

void TC7_Handler()
{
  // You must do TC_GetStatus to "accept" interrupt
  // As parameters use the first two parameters used in startTimer (TC1, 0 in this case)
  TC_GetStatus(TC2, 1);

  int time2 = millis();
  Serial.print("timer 2: ");
  Serial.println(time2);

}
