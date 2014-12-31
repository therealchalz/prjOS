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
 * kernel_data.h
 */

#ifndef KERNEL_DATA_H_
#define KERNEL_DATA_H_

#include "prjOS/include/scheduler.h"

typedef struct KernelData {
	TaskDescriptor* taskDescriptorList;	// pointer to the list of all tasks
	uint32_t tdCount;				// size of task list
	SchedulerStructure* schedulerStructure;	// Used to hold the state of the scheduler
	uint32_t nameserverTid;
} KernelData;

#endif /* KERNEL_DATA_H_ */
