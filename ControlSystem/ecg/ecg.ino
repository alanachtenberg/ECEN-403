// Use Due timer interrupt to trigger ADC conversion.
// The freqency of the timer interrupt can be changed, 
// and thus the sample frequency can be changed.


int curr_max = 0;

//Structure to hold ADC values and their times
struct Heart
{
  int value; // voltage value from ADC
  int tyme; // time of measurement
};

Heart peak[200]; //Might want to make this a dynamic array


int i = 0;

//////////////////////////
/*  Inside ADC Handler  */
//////////////////////////
void adc_handler()
{
  peak.value[i] = adc measurement
  peak.tyme[i] = micros()
   
  if (i == 675) // wait until you get 675 samples
  {
    i = 0;
    // set flag maybe
    // search through array for peaks
    for (int j = 0; j < 675; j++)
    {
      if (peak.value[j] >= curr_max)
      {
        curr_max = peak.value[i];
        max_index = i;
      }
      else if (peak.value[j] == 0)
      {
        zero_index = j;
      }
      else
      {
        if (zero_index > max_index && curr_max >= threshold)
        {
          peak_max_array[peak_index] = curr_max;
          peak_index++;
        }  
      }
    }
  }
  i++;
}
