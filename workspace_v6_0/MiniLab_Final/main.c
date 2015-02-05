/*
 * main.c
 *
 *  Created on: Feb 3, 2015
 *      Author: Alan
 */

//------------------------INCLUDES-------------------------

#include "inc/hw_ints.h" // Macros that define the interrupt assignment on Stellaris devices

#include "inc/hw_gpio.h"//include for general access of gpio registers

#include "inc/hw_memmap.h" /*: Macros defining the memory map of the Stellaris device. This includes
						  defines such as peripheral base address locations, e.g., GPIO_PORTF_BASE*/

#include "inc/hw_types.h" //Defines common types and macros such as tBoolean and HWREG(x)

#include "driverlib/sysctl.h" /*Defines and macros for System Control API of driverLib. This includes
								API functions such as SysCtlClockSet and SysCtlClockGet.*/

#include "driverlib/interrupt.h" /* Defines and macros for NVIC Controller (Interrupt) API of DriverLib.
								This includes API functions such as IntEnable and IntPrioritySet*/

#include "driverlib/gpio.h" /*Defines and macros for GPIO API of driverLib. This includes API functions
							such as GPIOPinTypePWM and GPIOPinWrite.*/

#include "driverlib/timer.h" /*Defines and macros for Timer API of driverLib. This includes API functions
							 such as TimerConfigure and TimerLoadSet.*/

//------------------------DEFINES--------------------------


#define FALSE 0//for use with simple flags
#define TRUE 1
#define CLOCK_SETTINGS SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN
#define LED_PINS GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
#define SWITCH1 GPIO_PIN_4
#define SWITCH2 GPIO_PIN_0
#define SWITCHES SWITCH1|SWITCH2
//------------------------DATA STRUCTS---------------------
	unsigned long ulPeriod; //variable for measuring timer period
	unsigned long MaxPeriod;//variable for storing a max value
	tBoolean EnableLED=true;//are LED's on or off
	unsigned int Led=2;//which LED is on

//------------------------INTERRUPT HANDLERS---------------


//------------------------MAIN-----------------------------
int main(void){

	//configures the system clock to 40 MHz
	//were using the 400MHz pll, that is auto divided by 2 and we are further dividing that by 5
	//the pll is driven by the main oscillator
	SysCtlClockSet(CLOCK_SETTINGS);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);// enables the GPIO F periph
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, LED_PINS); //sets pin 1 2 and 3 to output
	GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, SWITCHES);//sets pin 0 and 4 to input

	// Unlock PF0 so we can change it to a GPIO input
	// Once we have enabled (unlocked) the commit register then re-lock it
	// to prevent further changes.  PF0 is muxed with NMI
	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY_DD;
	HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;

	//sets the user switches to use a pull up resistor
	GPIOPadConfigSet(GPIO_PORTF_BASE, SWITCHES, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD_WPU);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);//enables timer periph to recieve the sys clock
	TimerConfigure(TIMER0_BASE, TIMER_CFG_32_BIT_PER);//configs timer, arg 1 is base address for timer 0
													  //arg 2 configs the timer 0 in 32 bit periodic mode
	  	  	  	  	  	  	  	  	  	  	  	  	  //16 bit timer0a and timer0b are combined to create the 32 bit timer

	ulPeriod = (SysCtlClockGet() / 10) / 2; //gets the clock freq, divides by our desired freq (10hz) to get number of cycles
											//further divide the number of cycles by 2 to get cycles required for 50 percent duty cycle
	MaxPeriod=ulPeriod*6;//sets the max period

	TimerLoadSet(TIMER0_BASE, TIMER_A, ulPeriod -1);//loads the period into the timer to fire interrupts every ulPeriod cycles
													//minus one because timer fires on 0 count
	GPIOPinIntDisable(GPIO_PORTF_BASE,LED_PINS); //disables interrupts on LED pins 1,2 and 3
	GPIOPinIntEnable(GPIO_PORTF_BASE,SWITCHES);// enables interrupts on switch pins 0 and 4
	GPIOIntTypeSet(GPIO_PORTF_BASE, SWITCHES ,GPIO_FALLING_EDGE);//sets interrupt to occur on falling edge, aka release of the button

	IntEnable(INT_GPIOF);//enables the gpiof interrupt for the nvic, this is what actually calls our handler
	IntEnable(INT_TIMER0A);//enables the specific vector in nested vector interrupt controller(NVIC)
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);//enables specific event to fire interrupt ie the "timeout"
	IntMasterEnable();//master enable of interrupts

	TimerEnable(TIMER0_BASE, TIMER_A);//actually enables/starts the timer

	while(1)// infinite loop to keep program running, all logic will be handled with interrupts
	{
	}
}


//declaration of handler will be added in startup_ccs.c
//we will replace the default handler in the interrupt vector with our handler below
void Timer0IntHandler(void)
{
	// Clear the timer interrupt
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	Led=Led*2;
	if (Led>8)//alternate which led's are on
		Led=2;
	if(EnableLED==false)//if pins are not enabled
	{
		GPIOPinWrite(GPIO_PORTF_BASE, LED_PINS, 0);//shut off leds
	}
	else
	{
		GPIOPinWrite(GPIO_PORTF_BASE, LED_PINS, Led);//turn on correct led
	}
}

void GPIOFIntHandler(void){

	long status=GPIOPinIntStatus(GPIO_PORTF_BASE,true);//get status of interrupts
	if (status==SWITCH1){//switch one was pressed
		GPIOPinIntClear(GPIO_PORTF_BASE,SWITCH1);//clear interrupt
		if (EnableLED==true)//toggle LED's
			EnableLED=false;
		else
			EnableLED=true;
	}
	else if(status==SWITCH2){//switch two was pressed
		GPIOPinIntClear(GPIO_PORTF_BASE,SWITCH2);
		ulPeriod=(ulPeriod+ulPeriod/2)%MaxPeriod;//legthen the period by one half its value, Rolls over after
		TimerLoadSet(TIMER0_BASE, TIMER_A, ulPeriod -1);//loads new period into the timer
	}
}
