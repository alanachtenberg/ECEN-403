/*
 * main.c
 *
 *  Created on: Feb 2, 2015
 *      Author: Alan
 */


/*
 * Stellaris Tri-color LED, Software toggle
 *
 * This program will toggle inside of a loop to start and stop the led blinking
 *
 * MCU Model Number: LM4120H5QR, important for project target/compiler

 * Pin Numbers
 *
 * PF1 - RED LED
 * PF2 - BLUE LED
 * PF3 - GREEN LED
 */

//---------------------------------INCLUDES---------------------------------

#include <inc/hw_types.h>
//common types and macros such as tBoolean and HWREG(x)
#include <inc/hw_memmap.h>
//Macros defining memory mapping of the stellaris, including peripheral base address locations
#include <driverlib/sysctl.h>
//Defines and macros for System Control API of DriverLib, including SysCtlClockSet and SysCtlClockGet
#include <driverlib/gpio.h>
//Defines and macros for GPIO api. This includes functions GPIOPinTypePWM & GPIOPinWrite



//---------------------------------MAIN---------------------------------

int main(){
	//we start with 2 because PF0 is wired to a switch
	//pindata is set in a one hot fasion ie 1 for pin 1, 01 for pin 1, 001 for pin 2 etc
	int PinData = 2; /*Creates an integer variable called PinData and initializes it to 2.
	This will be used to cycle through the three LEDs, lighting them one at a time.*/

	/*Before calling any peripheral specific driverLib function, enable the clock
	for that peripheral. If not enabled, it will result in a Fault ISR (address fault).
	This is a common mistake for new Stellaris users.*/

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	/*Using the launchpad schematic, locate the GPIO layout. You will see grouping labeled PA#,
	PB#, PC#, PD#, PE#, and PF#. PF1, PF2, and PF3 connect the red, blue, and green LED lights on
	the tri-color LED.
	Now open the sysctl.h file and look for "#define SYSCTL_PERIPH_GPIO." There are several
	entries A to S.
	Since the tri-color LED is connected to GPIOF this is the peripheral to enable.*/

	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
	/*This statement configures the three GPIO pins connected to the LEDs as outputs. Open up the gpio.h
	header file and locate the hardware calls for the pins required for the tri-color LED. Next locate "extern
	void GPIOPinTypeGPIOOutput(unsigned long ulPort, unsigned char ucPins);" this function defines
	selects the direction of the corresponding I/O port and pins. To define the port open hw_memmap.h
	header file containing Stellaris memory map information. There are two port access modes of GPIO ports.
	GPIO ports accessed through the high speed port toggle every clock cycle vs. once every two cycles for
	peripheral ports. In power sensitive applications, the peripheral port works better than the high speed port.
	For this exercise use GPIO_PORTF_BASE. For more information, see the Stellaris datasheet section
	10.4. Using the bitwise OR (|) for each pin to be enabled turns off that pin. For more on the GPIO
	direction register see the Stellaris datasheet pg. 635.*/


	while(1)
	{
	//Turn on the LED
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, PinData);
	/*Now go to the gpio.h header file and locate "extern void GPIOPinWrite(unsigned long ulPort,
	unsigned char ucPins, unsigned char ucVal);" this function call will turn on the tri-color LED.
	Declare which port and pins will be written. Make sure to read and understand how the
	GPIOPinWrite function is used in the Datasheet. The third data argument is not simply a 1 or 0,
	but represents the entire 8-bit data port. From Stellaris datasheet pg. 634, "The GPIODATA
	register is the data register. In software control mode, values written in the GPIODATA register
	are transferred onto the GPIO port pins if the respective pins have been configured as outputs
	through the GPIO Direction (GPIODIR) register (see page 635)."*/


	SysCtlDelay(2000000);
	/*Loop timer provided in StellarisWare. The count parameter is the
	* loop count, not the actual delay in clock cycles.*/
	// Cycle through Red, Green and Blue LEDs

	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0);
	/*What value is used to "turn off" the pins? */
	SysCtlDelay(2000000);
	if (PinData == 8)
		PinData = 2;
	else
		PinData = PinData*2;
	/*This statement reads "if PinData equals eight then set PinData equal to two otherwise
	take the current PinData value, multiply it by two and redefine the value of PinData based
	on the product. */
	}

}
