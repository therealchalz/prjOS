

#include <inc/hw_memmap.h>
#include <inc/hw_types.h>
#include <driverlib/debug.h>
#include <driverlib/fpu.h>
#include <driverlib/gpio.h>
#include <driverlib/pin_map.h>
#include <driverlib/rom.h>
#include <driverlib/sysctl.h>


#ifdef DEBUG
void __error__(char *filename, unsigned long line) {
	//TODO: something useful
}
#endif


int main(void) {

	FPULazyStackingEnable();
	SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

	int testLoop = 10;

	while(testLoop > 0)
	{
		testLoop--;

		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);
		SysCtlDelay(SysCtlClockGet() / 10 / 3);
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0);
		SysCtlDelay(SysCtlClockGet() / 10 / 3);
	}

	return 420;
}

