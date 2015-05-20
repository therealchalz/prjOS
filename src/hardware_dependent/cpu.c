/***************************************
 * Copyright (c) 2013 Charles Hache <chache@brood.ca>. All rights reserved. 
 * 
 * This file is part of the prjOS project.
 * prjOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * prjOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with prjOS.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * Contributors:
 *     Charles Hache <chache@brood.ca> - initial API and implementation
***************************************/

/*
 * cpu.c
 * For Stellaris Launchpad
 */

#include "prjOS/include/hardware_dependent/cpu.h"

void cpuPrintInfo() {
	bwprintf("Processor: LM4F120\r\n");
	bwprintf("Board: Stellaris Launchpad Rev A\r\n");
	bwprintf("Clock: %u\r\n", SysCtlClockGet());
}

void cpuInit() {
	FPULazyStackingEnable();
	FPUEnable();
	SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
}

void cpuSetupTaskDefaultParameters(TaskCpuCreateParameters *params, void* startFunction) {
	int r = 0;
	for (r=0; r<13; r++)
		params->regs[r] = r;

	params->IAPSR = 0;
	params->exceptionReturn = 0xFFFFFFFD;	//Return to Thread/PSP mode
	params->startFunction = startFunction;
}

void cpuPreemptionTimerDisable() {
	SysTickDisable();
}
void cpuPreemptionTimerEnable() {
	HWREG(NVIC_ST_CURRENT)=0;	//resets systick counter
	SysTickEnable();
}

uint32_t* cpuCreateTask(uint32_t* stackLocation, const TaskCreateParameters *parms) {
	//Need to set up stack like this:
	//R4,R5,R6,R7,R8,R9,R10,R11,LR(EXC_RETURN),R0,R1,R2,R3,R12,LR(Pre exception),PC(Pre exception),xPSR(Pre exception)
	uint32_t* newStack = stackLocation;

	//Align stack to 32-bit boundary, using full descending stack so if we need to move it, move it down
	if ((uint32_t)newStack != ((uint32_t)newStack &~0x3)) {
		newStack = (uint32_t*)((uint32_t)newStack & ~0x3);
	}

	//push on 6 dummy things - the context switch expects the compiler
	//to push the syscall parameters onto the stack.  If we don't make
	//dummy space here, if we were to call a syscall as soon as our task starts
	//and the syscall takes no arguments, then the context switch could go
	//past the end of our stack looking for the parameters
	int i=0;
	for (; i<6; i++) {
		*(--newStack) = 0x00000000;
	}
	//push 17 things on the stack
	*(--newStack) = 0x01000000;
	*(--newStack) = ((uint32_t)(parms->cpuSpecific.startFunction));
	*(--newStack) = 0xFFFFFFFF;
	*(--newStack) = parms->cpuSpecific.regs[12];
	*(--newStack) = parms->cpuSpecific.regs[3];
	*(--newStack) = parms->cpuSpecific.regs[2];
	*(--newStack) = parms->cpuSpecific.regs[1];
	*(--newStack) = parms->cpuSpecific.regs[0];	//This one is overwritten with the syscall return value in the context handler
	*(--newStack) = parms->cpuSpecific.exceptionReturn;
	*(--newStack) = parms->cpuSpecific.regs[11];
	*(--newStack) = parms->cpuSpecific.regs[10];
	*(--newStack) = parms->cpuSpecific.regs[9];
	*(--newStack) = parms->cpuSpecific.regs[8];
	*(--newStack) = parms->cpuSpecific.regs[7];
	*(--newStack) = parms->cpuSpecific.regs[6];
	*(--newStack) = parms->cpuSpecific.regs[5];
	*(--newStack) = (uint32_t)parms->cpuSpecific.regs[4];

	return newStack;
}
