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

Heart peak[200]; //Might want to make this a dynamic array

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
  Serial.begin(115200);
}

void loop()
{
  for (i = 0; i < 202; i++) // populate the struct
  {
    if (i == 100 || i == 200)
    {
      peak[i].value = 0;
      peak[i].tyme = millis();
    }
    else if (i == 101 || i == 201) 
    {
      peak[i].value = 1;
      peak[i].tyme = millis();
    }  
    else 
    {
      peak[i].value = i;
      peak[i].tyme = millis();
    }
  }

  for (z = 0; z < 202; z++)
  {
    if (peak[z].value > curr_max)
    {
      curr_max = peak[z].value;
      max_index = z;
    }
    else if (peak[z].value == 0) // this value needs to be adjusted to what the adc reads at 0 V
    {
      zero_index = z;
    }
    else
    {
      if (zero_index > max_index && curr_max >= threshold) // some minimal adc value that peaks are never below
      {
        peak_max_array[peak_index].Value = peak[max_index].value; // need to be global variables. increment in main loop
        peak_max_array[peak_index].Tyme = peak[max_index].tyme;
        peak_index++;
        curr_max = 0; // reset to 0 to detect new peaks
      }  
    }
  }
    
  if (peak_index == 10)
  {
    for (j = 0; j <=11; j++)
    {
      Serial.print("peak value: ");
      Serial.println(peak_max_array[j].Value);
      Serial.print("peak time: ");
      Serial.println(peak_max_array[j].Tyme);
      peak_index = 0;
      
    }
  }
  
  delay(1000);
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
