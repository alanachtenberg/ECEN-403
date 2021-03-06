// Use Due timer interrupt to trigger ADC conversion.
// The freqency of the timer interrupt can be changed, 
// and thus the sample frequency can be changed.

//#include "ECG.h"

float curr_max = 1500;
int threshold = 1800;
int peak_index = 0;
int zero_time = 0;
int max_time = 0;
int adc_flag = 0;
int max_rate_change = 0; // 1/6 seconds --> 167 milliseconds
float VoltageAvg = 0;
float VoltageSum = 0;
float BPM = 0;
float volt_value = 0;
unsigned long time_diff = 0;

typedef struct Heart
{
  float value;
  unsigned long tyme;
};

typedef struct Peaks
{
  float Value;
  unsigned long Tyme;
};

Peaks peak_max_array[40];
Heart peak; //Might want to make this a dynamic array



void setup()
{
  Serial.begin(115200);
  pinMode(13, OUTPUT);
  //pinMode(57, OUTPUT);
  
  adc_setup(); // setup ADC
  
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
  
  
  //startADC();

}

void adc_setup ()
{
  NVIC_EnableIRQ (ADC_IRQn) ;   // enable ADC interrupt vector
  ADC->ADC_IDR = 0xFFFFFFFF ;   // disable interrupts
  ADC->ADC_IER = 0x40 ;         // enable AD7 End-Of-Conv interrupt (Arduino pin A0)
  ADC->ADC_CHDR = 0xFFFF ;      // disable all channels
  ADC->ADC_CHER = 0x40 ;        // enable just A0 --> 0x80
  ADC->ADC_CGR = 0x15555555 ;   // All gains set to x1
  ADC->ADC_COR = 0x00000000 ;   // All offsets off
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
  if (ADC->ADC_ISR & ADC_ISR_EOC6)   // ensure there was an End-of-Conversion and we read the ISR reg
  {
    int val = *(ADC->ADC_CDR+6) ;    // get conversion result
    //Serial.print("ADC value: ");
    //Serial.println(val);
    //Serial.println('sup');
    //Serial.println(micros());
    peak.value = val;    // get conversion result
    peak.tyme = millis();
    //Serial.println(peak.value);
    adc_flag = 1;
  
  }

}

#ifdef __cplusplus
}
#endif

void loop()
{
 
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
        
        volt_value = (curr_max/4096)*3.3; // convert to a voltage
        peak_max_array[peak_index].Value = volt_value; // convert to a voltage
        peak_max_array[peak_index].Tyme = peak.tyme;
        //Serial.println(peak_max_array[peak_index].Value);
        Serial.println(peak_max_array[peak_index].Tyme);
        digitalWrite(13, HIGH);
        delay(10);
        digitalWrite(13, LOW);

        VoltageSum = VoltageSum + peak_max_array[peak_index].Value;
        peak_index++;
        Serial.println(peak_index);
        curr_max = 1500; // reset to 0 to detect new peaks
        
      }  
    }
    
  adc_flag = 0;
  
  }
  
  if (peak_index == 10)
  {
    Serial.println("r");
    // get voltage average to compute low voltage peaks
    VoltageAvg = VoltageSum/10;
    
    for(int i = 0; i < 7; i++) // 8 because we're looking at i + 2
    {
      if(peak_max_array[i+2].Tyme - peak_max_array[i+1].Tyme > (peak_max_array[i+1].Tyme - peak_max_array[i].Tyme) + max_rate_change ||
         peak_max_array[i+2].Tyme - peak_max_array[i+1].Tyme > (peak_max_array[i+1].Tyme - peak_max_array[i].Tyme) - max_rate_change) 
      {
        // missed beat
        //Serial.println("Missed Beat");
      }
      
      if(peak_max_array[i].Value <= (VoltageAvg/2)) // low voltage peak if it's less than half
      {
        // low voltage peak
      }
      
    }
    
    // access 39th and 40th indices directly since for loop only goes to 38
    if(peak_max_array[9].Value <= (VoltageAvg/2)) // low voltage peak if it's less than half
    {
      // low voltage peak
    }
    if(peak_max_array[10].Value <= (VoltageAvg/2)) // low voltage peak if it's less than half
    {
      // low voltage peak
    }
    
    // get BPM
    BPM = (float(9)/float((peak_max_array[9].Tyme - peak_max_array[0].Tyme)))*60000; // Beats/ms * 1000 * 60 = Beats/min
    //time_diff = peak_max_array[9].Tyme - peak_max_array[0].Tyme;
    //Serial.print("t1: ");
    //Serial.println(peak_max_array[9].Tyme);
    //Serial.print("t2: ");
    //Serial.println(peak_max_array[0].Tyme);
    //Serial.print("td: ");
    //Serial.println(time_diff);
    //Serial.print("va: ");
    //Serial.println(VoltageAvg);
    Serial.print("BPM: ");
    Serial.println(BPM);
    VoltageSum = 0;
    peak_index = 0;
  }
  
}
