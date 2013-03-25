/*
 * uart.c
 *
 * For Stellaris Launchpad
 */

#include <hardware_dependent/uart.h>
#include <inc/hw_types.h>
#include <driverlib/sysctl.h>
#include <inc/hw_memmap.h>
#include <inc/hw_uart.h>
#include <driverlib/rom.h>
#include <driverlib/uart.h>
#include <driverlib/gpio.h>

void UARTPutC( char c ) {
	ROM_UARTCharPut( UART0_BASE, c );
}

int UARTGetC() {
	return (int)ROM_UARTCharGet( UART0_BASE );
}

void UARTInit( unsigned long baud ) {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
    ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	ROM_UARTConfigSetExpClk(UART0_BASE, ROM_SysCtlClockGet(), baud, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
	ROM_UARTEnable(UART0_BASE);
	ROM_UARTFIFODisable(UART0_BASE);
}


