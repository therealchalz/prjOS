/*
 * cpu.c
 * For Stellaris Launchpad
 */

#include <cpu.h>
#include <inc/hw_memmap.h>
#include <inc/hw_types.h>
#include <driverlib/sysctl.h>
#include <driverlib/fpu.h>
#include <bwio.h>

void cpuInit() {
	FPULazyStackingEnable();
	SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
}

void cpuPrintInfo() {
	bwprintf("Processor: LM4F120\r\n");
	bwprintf("Board: Stellaris Launchpad\r\n");
	bwprintf("Clock: %u\r\n", SysCtlClockGet());
}
