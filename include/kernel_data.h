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

#include <prjOS/include/scheduler.h>
#include <prjOS/include/task.h>

typedef struct EventQuery {
	task_id_t waitingTid;
	struct EventQuery* nextQuery;
	uint32_t state;
	uint32_t eventId;
} EventQuery;

typedef struct EventData {
	EventQuery eventList[EVENTS_NUM_EVENTS];
	EventQuery* eventQueueHead[EVENTS_NUM_TYPES];
	EventQuery* eventQueueTail[EVENTS_NUM_TYPES];
} EventData;

typedef struct KernelData {
	TaskDescriptor* taskDescriptorList;		// Pointer to the list of all tasks
	uint32_t tdCount;						// Size of task list
	SchedulerStructure* schedulerStructure;	// Used to hold the state of the scheduler
	task_id_t nameserverTid;				// Task ID of the nameserver
	EventData* eventData;					// Pointer to event queues and other data
	volatile uint64_t systemMicroCount;				/*	Number of microseconds since system start
												subject to implementation's timer's accuracy
												and granularity, etc. */
	uint32_t* stackBase;					// Base SP (eg: |base - current| = total usage)
} KernelData;

#endif /* KERNEL_DATA_H_ */
