// Use Due timer interrupt to trigger ADC conversion.
// The freqency of the timer interrupt can be changed, 
// and thus the sample frequency can be changed.


//Structure to hold ADC values and their times
struct Heart
{
  int value; // voltage value from ADC
  int tyme; // time of measurement
};

Heart peak[200]; //Might want to make this a dynamic array

int curr_max = 0;
int i = 0;
int thresh;

//////////////////////////
/*  Inside ADC Handler  */
//////////////////////////
void adc_handler()
{
  peak.value[i] = adc measurement
  peak.tyme[i] = micros()

  if (peak.value[i] > curr_max)
  {
    curr_max = peak.value[i];
    max_index = i;
  }
  else if (peak.value[i] == 0) // this value needs to be adjusted to what the adc reads at 0 V
  {
    zero_index = i;
  }
  else
  {
    if (zero_index > max_index && curr_max >= threshold) // some minimal adc value that peaks are never below
    {
      peak_max_array[peak_index] = curr_max; // need to be global variables. increment in main loop
      peak_index++;
      curr_max = 0; // reset to 0 to detect new peaks
    }  
  }
  
  i++;
  if (i == 676) //675 + 1 samples
  {
    i = 0;
    // set flag and process data
  }
  
}
