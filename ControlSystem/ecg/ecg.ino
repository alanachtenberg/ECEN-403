// Use Due timer interrupt to trigger ADC conversion.
// The freqency of the timer interrupt can be changed, 
// and thus the sample frequency can be changed.

//#include <PEAKS.h>
typedef struct Heart
{
  int value;
  unsigned long tyme;
};

typedef struct Peaks
{
  int Value;
  unsigned long Tyme;
};

Peaks peak_max_array[10];

Heart peak[100]; //Might want to make this a dynamic array

int curr_max = 0;
int i = 0;
int threshold = 1000;
int peak_index = 0;
int zero_index = 0;
int max_index = 0;

void setup()
{
  Serial.begin(115200);
  
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

//////////////////////////
/*  Inside ADC Handler  */
//////////////////////////
void ADC_Handler (void)
{
  if (ADC->ADC_ISR & ADC_ISR_EOC7)   // ensure there was an End-of-Conversion and we read the ISR reg
  {
    int val = *(ADC->ADC_CDR+7) ;    // get conversion result
    //Serial.print("ADC value: ");
    //Serial.println(val);
    //Serial.print('time: ');
    //Serial.println(micros());
    peak[i].value = val;
    peak[i].tyme = micros();

    if (peak[i].value > curr_max)
    {
      curr_max = peak[i].value;
      max_index = i;
    }
    else if (peak[i].value == 0) // this value needs to be adjusted to what the adc reads at 0 V
    {
      zero_index = i;
    }
    else
    {
      if (zero_index > max_index && curr_max >= threshold) // some minimal adc value that peaks are never below
      {
        peak_max_array[peak_index].Value = curr_max; // need to be global variables. increment in main loop
        //peak_max_array[peak_index].Tyme = millis();
        //Serial.print("PV: ");
        Serial.println(curr_max);
        peak_index++;
        curr_max = 0; // reset to 0 to detect new peaks
      }  
    }
    
    i++;
    if (i == 101) //675 + 1 samples
    {
      i = 0;
      // set flag and process data
    }
    
    if (peak_index == 11)
    {
      peak_index = 0;
    }
  
  }

}

#ifdef __cplusplus
}
#endif


void loop()
{

}


