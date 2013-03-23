/*
 * board.c
 *
 * For Stellaris Launchpad
 */

#include <board.h>
#include <inc/hw_memmap.h>
#include <inc/hw_types.h>
#include <driverlib/sysctl.h>
#include <driverlib/gpio.h>

void boardInit() {
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);
}

void boardSetIndicatorLED(char status) {
	if (status) {
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);
	} else {
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0);
	}
}
