// Use Due timer interrupt to trigger ADC conversion.
// The freqency of the timer interrupt can be changed, 
// and thus the sample frequency can be changed.

#include <PEAKS.h>


Peaks peak_max_array[10];

Heart peak[400]; //Might want to make this a dynamic array

int curr_max = 0;
int i = 0;
int j = 0;
int z = 0;
int threshold = 50;
int peak_index = 0;
int zero_index = 0;
int max_index = 0;

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  for (i = 0; i < 402; i++) // populate the struct
  {
    if (i == 200 || i == 400)
    {
      peak.value[i] = 0;
      peak.tyme[i] = micros();
    }
    else if (i == 201 || i == 401) 
    {
      peak.value[i] = 1;
      peak.tyme[i] = micros();
    }  
    else 
    {
    peak.value[i] = i;
    peak.tyme[i] = micros();
    }
  }

  for (z = 0; z < 402; z++)
  {
    if (peak.value[z] > curr_max)
    {
      curr_max = peak.value[z];
      max_index = z;
    }
    else if (peak.value[z] == 0) // this value needs to be adjusted to what the adc reads at 0 V
    {
      zero_index = z;
    }
    else
    {
      if (zero_index > max_index && curr_max >= threshold) // some minimal adc value that peaks are never below
      {
        peak_max_array.Value[peak_index] = peak.value[max_index]; // need to be global variables. increment in main loop
        peak_max_array.Tyme[peak_index] = peak.tyme[max_index];
        peak_index++;
        curr_max = 0; // reset to 0 to detect new peaks
      }  
    }
  }
    
  if (peak_index == 10)
  {
    for (j = 0; j <=11; j++)
    {
      print(peak_max_array.Value[j];
      print(peak_max_array.Tyme[j];
      peak_index = 0;
    }
  }
}

//////////////////////////
/*  Inside ADC Handler  */
//////////////////////////
/*
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
*/
