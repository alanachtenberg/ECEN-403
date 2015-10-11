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

// peak.value[i] = adc measurement
// peak.tyme[i] = micros()


// search for the max value
for (int i = 0; i < 200; i++)
{
  if (peak.value[i] > curr_max)
  {
    curr_max = peak.value[i];
  }
}

