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
 * scheduler.h
 */

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <task.h>
#include <hardware_dependent/cpu_defs.h>

typedef struct SchedulerTask {
	TaskDescriptor* td;
	struct SchedulerTask* next;
} SchedulerTask;

typedef struct SchedulerStructure {
	SchedulerTask* queueHeads[TASKS_MAX_PRIORITY];
	SchedulerTask* queueTails[TASKS_MAX_PRIORITY];
	SchedulerTask allTasks[KERNEL_MAX_NUMBER_OF_TASKS];
} SchedulerStructure;

void schedulerInit(SchedulerStructure *schedStruct, TaskDescriptor *tds);
void schedulerAdd(SchedulerStructure* schedStruct, TaskDescriptor* td);
void schedulerPrintTasks(SchedulerStructure* schedStruct);
TaskDescriptor* schedule(SchedulerStructure* schedStruct);

#endif /* SCHEDULER_H_ */
