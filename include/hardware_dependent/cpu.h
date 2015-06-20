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
 * cpu.h
 */

#ifndef CPU_H_
#define CPU_H_

#include <stdint.h>
#include "prjOS/include/task.h"
#include "prjOS/include/hardware_dependent/cpu_defs.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_nvic.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/fpu.h"
#include "driverlib/systick.h"
#include "prjOS/include/bwio.h"
#include "prjOS/include/hardware_dependent/cpu_defs.h"
#include "prjOS/include/task.h"

void cpuInit();
void cpuPrintInfo();
void cpuBarf();

uint32_t* cpuCreateTask(uint32_t* stackLocation, const TaskCreateParameters *parms);
void cpuSetupTaskDefaultParameters(TaskCpuCreateParameters *params, void* startFunction);
void cpuPreemptionTimerDisable();
void cpuPreemptionTimerEnable();

void contextSwitch(TaskDescriptor *t);
void hwContextSwitch();

uint32_t cpuHelperGetIsr();

#endif /* CPU_H_ */
