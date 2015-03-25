/*
 * main.c
 */

/* Note from the datasheet: In PWM mode, timers are configured as 24-bit or 48-bit down-counter with assigned
start value (corresponding to period) defined by GPTMTnILR and GPTMTnPR registers.
*/

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h" //sysctl calls for enabling peripherals
#include "driverlib/adc.h"	  //definitions for the adc driver
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
	SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN); //sets clock to 40 Mhz
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0); //enable the adc
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB); //enable GPIO port B
	GPIOPinTypeGPIOInput(ADC_PORT,ADC_PIN);
	GPIOPinTypeADC(ADC_PORT,ADC_PIN); //sets
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
	// Configure pin PB6 as Timer_0 CCP0
	GPIOPinConfigure(GPIO_PB6_T0CCP0); // Configure pin PB6 as Timer 0_A output
	GPIOPinTypeTimer(GPIO_PORTB_BASE, GPIO_PIN_6 ); // Enable pin PB6 as output of timer addressed to it
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0); // Enable Timer 0
	// Configure the timers as split 16-bit pairs, and set each timer as PWM mode.
	TimerConfigure(TIMER0_BASE, (TIMER_CFG_SPLIT_PAIR|TIMER_CFG_A_PWM|TIMER_CFG_B_PWM));
	TimerControlLevel(TIMER0_BASE, TIMER_BOTH, 0); // Timer 0 is trigger low

	while(1){
		ADCProcessorTrigger(ADC0_BASE, 1);//triggers the adc conversion with software
	}

}

//handler is declared in nvic, n startup_css.c
void SequenceIntHandler(void){
			ADCIntClear(ADC0_BASE,1);//clear the interrupt
			ADCSequenceDataGet(ADC0_BASE, 1, ulADC0Value);//copies sequence data into our buffer
			unsigned long ac_voltage[4];
			unsigned long ulPeriod; // sets the period, and thus frequency, of our PWM
			unsigned long dutyCycle;
			unsigned long ac_voltage_avg;
			int i=0;
			for (i=0;i<4;++i){
				ac_voltage[i]=(ulADC0Value[i]*SYSTEM_VOLTAGE_MV)/ADC_RESOLUTION;//voltage in millivolts
			}
			ac_voltage_avg = (ac_voltage[0]+ac_voltage[1]+ac_voltage[2]+ac_voltage[3])/4;
			ulPeriod = (SysCtlClockGet() / 500)/2; // 40 MHz ---> 40 kHz
			if (ac_voltage_avg >= (0.75*(ulPeriod-1))) {
				dutyCycle = (0.75*(ulPeriod-1));
			}
			else if (ac_voltage_avg <= 0.25*(ulPeriod-1)) {
				dutyCycle = (0.25*(ulPeriod-1));
			}
			else {
				dutyCycle = ac_voltage_avg;
			}
			// These API functions load the match value into Timer n Match register(GPTMTnMATCHR)
			// Timer 0 Load set
			TimerLoadSet(TIMER0_BASE, TIMER_B, ulPeriod -1);
			// Timer 0 Match set
			TimerMatchSet(TIMER0_BASE, TIMER_A, dutyCycle);
			// Timers are now configured.
			// Finally, enable the timers, which will now run (API functions will set TnENbit in Reg 4 (GPTMCTL)
			TimerEnable(TIMER0_BASE, TIMER_BOTH);
}