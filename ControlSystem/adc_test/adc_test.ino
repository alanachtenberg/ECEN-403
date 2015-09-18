// Circular buffer, power of two.
#define BUFSIZE 0x400
#define BUFMASK 0x3FF
volatile int samples [BUFSIZE] ;
volatile int sptr = 0 ;
volatile int isr_count = 0 ;   // this was for debugging


void setup()
{
  Serial.begin (115200) ; // for debugging
  adc_setup () ;         // setup ADC
  setup_pio_TIOA0 () ;  // drive Arduino pin 2 at 48kHz to bring clock out
  // Start timer. Parameters are:

  // TC0 : timer counter. Can be TC0, TC1 or TC2
  // 0   : channel. Can be 0, 1 or 2
  // TC0_IRQn: irq number. See table.
  // 1  : frequency (in Hz)
  // The interrupt service routine is TC0_Handler. See table.

  startTimer(TC0, 0, TC0_IRQn, 1);

  // Paramters table:
  // TC0, 0, TC0_IRQn  =>  TC0_Handler()
  // TC0, 1, TC1_IRQn  =>  TC1_Handler()
  // TC0, 2, TC2_IRQn  =>  TC2_Handler()
  // TC1, 0, TC3_IRQn  =>  TC3_Handler()
  // TC1, 1, TC4_IRQn  =>  TC4_Handler()
  // TC1, 2, TC5_IRQn  =>  TC5_Handler()
  // TC2, 0, TC6_IRQn  =>  TC6_Handler()
  // TC2, 1, TC7_IRQn  =>  TC7_Handler()
  // TC2, 2, TC8_IRQn  =>  TC8_Handler()
}

void loop() 
{
}


/* Function Defines */

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

void setup_pio_TIOA0 ()  // Configure Ard pin 2 as output from TC0 channel A (copy of trigger event)
{
  PIOB->PIO_PDR = PIO_PB25B_TIOA0 ;  // disable PIO control
  PIOB->PIO_IDR = PIO_PB25B_TIOA0 ;   // disable PIO interrupts
  PIOB->PIO_ABSR |= PIO_PB25B_TIOA0 ;  // switch to B peripheral
}


/* Interrupt Handlers */

// This function is called every 1 sec.
void TC0_Handler()
{
  // You must do TC_GetStatus to "accept" interrupt
  // As parameters use the first two parameters used in startTimer (TC0, 0 in this case)
  TC_GetStatus(TC0, 0);
  Serial.print("time(ms): "):
  Serial.print(millis());

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
    Serial.print("ADC Value: ");
    Serial.println(val);
    samples [sptr] = val ;           // stick in circular buffer
    sptr = (sptr+1) & BUFMASK ;      // move pointer
  }
  isr_count ++ ;
}

#ifdef __cplusplus
}
#endif
