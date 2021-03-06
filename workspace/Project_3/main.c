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
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "inc/hw_gpio.h"
#include "utils/ustdlib.h"
#include "utils/uartstdio.h"
#include "utils/cmdline.h"
/*
 * if a driver library API is called with incorrect parameters or a library function generates an
 *  error for some other reason. The following code will be executed
 */

#define ADC_PORT GPIO_PORTB_BASE
#define ADC_PIN GPIO_PIN_5
#define SYSTEM_VOLTAGE_MV 3300//millivolts
#define ADC_RESOLUTION 4096
#ifdef DEBUG
void__error__(char *pcFilename, unsigned long ulLine)
{
}
#endif

unsigned long ulADC0Value[4]; //array size matches fifo depth of ADC sequencer 1


volatile unsigned long voltage;

int main(void){
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	UARTStdioInit(0);


	//sets clock to 40 Mhz
	SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0); //enable the adc
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);//enable GPIO port B
	GPIOPinTypeGPIOInput(ADC_PORT,ADC_PIN);
	GPIOPinTypeADC(ADC_PORT,ADC_PIN);//sets
	SysCtlADCSpeedSet(SYSCTL_ADCSPEED_250KSPS); //set the sample freq to 250 khz
	//sequence sample period is 1/250Khz, total sampling period is 4/250khz
	ADCSequenceDisable(ADC0_BASE, 1);//disables adc sequencer 1, we will configure it and re-enable it later

	ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0); //configures the adc to use sequencer 1, be triggered by the processor, and have the highest priority 0
	//ch 11 is pb5
	ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_CH11);//configures the sequence 0 to read the temperature
	ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_CH11);//repeat for sequence 1
	ADCSequenceStepConfigure(ADC0_BASE, 1, 2, ADC_CTL_CH11);//repeat for sequence 2
	ADCSequenceStepConfigure(ADC0_BASE, 1, 3, ADC_CTL_CH11 | ADC_CTL_IE | ADC_CTL_END);//configures the final step to notify the processor it has completed its sequence vie interrupt
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
			unsigned long ac_voltage[4];
			int i=0;
			for (i=0;i<4;++i){
			ac_voltage[i]=(ulADC0Value[i]*SYSTEM_VOLTAGE_MV)/ADC_RESOLUTION;//voltage in millivolts
			}
			UARTprintf("alan");
			  /*  UARTprintf("Welcome to the Stellaris LM4F120 LaunchPad!\n");
			    UARTprintf("Type 'help' for a list of commands\n");
			    UARTprintf("> ");*/

}

