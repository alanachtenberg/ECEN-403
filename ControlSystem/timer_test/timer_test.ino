// Online Example

// Black magic
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

void setup(){
  Serial.begin(115200);

  // Start timer. Parameters are:

  // TC1 : timer counter. Can be TC0, TC1 or TC2
  // 0   : channel. Can be 0, 1 or 2
  // TC3_IRQn: irq number. See table.
  // 40  : frequency (in Hz)
  // The interrupt service routine is TC3_Handler. See table.

  startTimer(TC1, 0, TC3_IRQn, 2);
  startTimer(TC2, 0, TC6_IRQn, 2);

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

void loop(){
}

volatile boolean l;

// This function is called every 1/40 sec.
void TC3_Handler()
{
  // You must do TC_GetStatus to "accept" interrupt
  // As parameters use the first two parameters used in startTimer (TC1, 0 in this case)
  TC_GetStatus(TC1, 0);

  int time1 = millis();
  Serial.print("timer 1: ");
  Serial.println(time1);

}

void TC6_Handler()
{
  // You must do TC_GetStatus to "accept" interrupt
  // As parameters use the first two parameters used in startTimer (TC1, 0 in this case)
  TC_GetStatus(TC2, 0);

  int time2 = millis();
  Serial.print("timer 2: ");
  Serial.println(time2);

}
