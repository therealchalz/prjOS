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
 * cpu_dep.h
 * For Stellaris Launchpad
 */

#ifndef CPU_DEP_H_
#define CPU_DEP_H_

/* Macros for putting out the SVC */
#define changeArgToString(x) #x
#define macroToString(x) changeArgToString(x)
#define svcArg(a) "		SVC #"macroToString(a)"\n"

#define STACK_BASE						(0x20000000+0x8000)
#define KERNEL_STACK_SIZE				(0x1000)
#define KERNEL_MAX_NUMBER_OF_TASKS		(10)
#define KERNEL_TASK_DEFAULT_STACK_SIZE	(0x800)

typedef struct TaskCpuCreateParameters {
	int regs[13];			//R0-R12
	void* startFunction;	//aka LR, R14
	int exceptionReturn;	//aka EXC_RETURN, pg 106.  0xFFFF FFF9 - use MSP (kernel) , 0xFFFF FFFD (task)
	int IAPSR;				//typically always 0
} TaskCpuCreateParameters;

#endif /* CPU_DEP_H_ */
