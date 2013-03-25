/*
 * cpu.c
 * For Stellaris Launchpad
 */

#include <hardware_dependent/cpu.h>
#include <inc/hw_memmap.h>
#include <inc/hw_types.h>
#include <driverlib/sysctl.h>
#include <driverlib/fpu.h>
#include <bwio.h>
#include <hardware_dependent/cpu_defs.h>
#include <task.h>

void cpuInit() {
	FPULazyStackingEnable();
	SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
}

void cpuPrintInfo() {
	bwprintf("Processor: LM4F120\r\n");
	bwprintf("Board: Stellaris Launchpad\r\n");
	bwprintf("Clock: %u\r\n", SysCtlClockGet());
}

void cpuSetupTaskDefaultParameters(TaskCpuCreateParameters *params, void* startFunction) {
	int r = 0;
	for (r=0; r<13; r++)
		params->regs[r] = r;

	params->IAPSR = 0;
	params->exceptionReturn = 0xFFFFFFFD;	//Return to Thread/PSP mode
	params->startFunction = startFunction;
}

int* cpuCreateTask(int* stackLocation, const TaskCreateParameters *parms) {
	//Need to set up stack like this:
	//R4,R5,R6,R7,R8,R9,R10,R11,LR(EXC_RETURN),R0,R1,R2,R3,R12,LR(Pre exception),PC(Pre exception),xPSR(Pre exception)
	int* newStack = stackLocation;
	//push 17 things on the stack
	*(--newStack) = 0x01000000;
	*(--newStack) = ((int)(parms->cpuSpecific.startFunction));
	*(--newStack) = 0xFFFFFFFF;
	*(--newStack) = parms->cpuSpecific.regs[12];
	*(--newStack) = parms->cpuSpecific.regs[3];
	*(--newStack) = parms->cpuSpecific.regs[2];
	*(--newStack) = parms->cpuSpecific.regs[1];
	*(--newStack) = parms->cpuSpecific.regs[0];
	*(--newStack) = parms->cpuSpecific.exceptionReturn;
	*(--newStack) = parms->cpuSpecific.regs[11];
	*(--newStack) = parms->cpuSpecific.regs[10];
	*(--newStack) = parms->cpuSpecific.regs[9];
	*(--newStack) = parms->cpuSpecific.regs[8];
	*(--newStack) = parms->cpuSpecific.regs[7];
	*(--newStack) = parms->cpuSpecific.regs[6];
	*(--newStack) = parms->cpuSpecific.regs[5];
	*(--newStack) = (int)parms->cpuSpecific.regs[4];

	return newStack;
}
