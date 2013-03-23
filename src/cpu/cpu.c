/*
 * cpu.c
 * For Stellaris Launchpad
 */

#include <cpu.h>
#include <inc/hw_memmap.h>
#include <inc/hw_types.h>
#include <driverlib/sysctl.h>

void cpuInit() {
	FPULazyStackingEnable();
	SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
}
