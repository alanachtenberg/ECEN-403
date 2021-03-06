/*
 * main.c
 *
 *  Created on: Feb 9, 2015
 *      Author: Alan
 */

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h" //sysctl calls for enabling peripherals
#include "driverlib/adc.h"	  //definitions for the adc driver
#include "driverlib/interrupt.h"

/*
 * if a driver library API is called with incorrect parameters or a library function generates an
 *  error for some other reason. The following code will be executed
 */


#ifdef DEBUG
void__error__(char *pcFilename, unsigned long ulLine)
{
}
#endif

unsigned long ulADC0Value[4]; //array size matches fifo depth of ADC sequencer 1

volatile unsigned long ulTempAvg; //volatile keyword prevents compiler optimizations
volatile unsigned long ulTempValueC;// will allow us to see these variables during debug
volatile unsigned long ulTempValueF;

int main(void){

	//sets clock to 40 Mhz
	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0); //enable the adc

	SysCtlADCSpeedSet(SYSCTL_ADCSPEED_250KSPS); //set the sample freq to 250 khz

	ADCSequenceDisable(ADC0_BASE, 1);//disables adc sequencer 1, we will configure it and renable it later

	ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0); //configures the adc to use sequencer 1, be triggered by the processor, and have the highest priority 0

	ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_TS);//configures the sequence 0 to read the tempature
	ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_TS);//repeat for sequence 1
	ADCSequenceStepConfigure(ADC0_BASE, 1, 2, ADC_CTL_TS);//repeat for sequence 2
	ADCSequenceStepConfigure(ADC0_BASE, 1, 3, ADC_CTL_TS | ADC_CTL_IE | ADC_CTL_END);//configures the final step to notify the processor it has completed its sequence vie interrupt
	//adc_ctl_end notifies the adc that the sequence has been completed

	ADCSequenceEnable(ADC0_BASE, 1);//configuration done, time to reenable the sequencer
	ADCIntEnable(ADC0_BASE,1);//enables interrupt for the adc0 on sequencer 1
	IntEnable(INT_ADC0SS1);
	IntMasterEnable();
	while(1){
		ADCProcessorTrigger(ADC0_BASE, 1);//triggers the adc conversion with software
	}
}

//handler is declared in nvic, n startup_css.c
void SequenceIntHandler(void){
			ADCIntClear(ADC0_BASE,1);//clear the interrupt

			ADCSequenceDataGet(ADC0_BASE, 1, ulADC0Value);//copies sequence data into our buffer

			ulTempAvg = (ulADC0Value[0] + ulADC0Value[1] + ulADC0Value[2] + ulADC0Value[3]+2)/4;//get the average temp, 2 is for rounding
			//integers always round down, simply adding .5 will make it round correctly

			ulTempValueC = (1475 - ((2475 * ulTempAvg)) / 4096)/10;//value of temp in celsius
			//conversion in datasheet from section 13.3.6

			ulTempValueF = ((ulTempValueC * 9) + 160) / 5;//simple conversion from C to F
}

