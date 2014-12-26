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
 * scheduler.c
 */

#include "string.h"
#include "prjOS/include/scheduler.h"
#include "prjOS/include/bwio.h"

void schedulerInit(SchedulerStructure *schedStruct, TaskDescriptor *tds) {
	memset(schedStruct, 0, sizeof(SchedulerStructure));
	int i;
	for (i = 0; i < KERNEL_MAX_NUMBER_OF_TASKS; i++) {
		schedStruct->allTasks[i].td = &tds[i];
		schedStruct->allTasks[i].next = 0;
	}
}

SchedulerTask* getSchedulerTask(SchedulerStructure* schedStruct, TaskDescriptor* td) {
	//TODO: this is sloppy, speed it up
	SchedulerTask* ret = 0;
	int i;
	for (i = 0; i < KERNEL_MAX_NUMBER_OF_TASKS; i++) {
		if (schedStruct->allTasks[i].td == td) {
			ret = &(schedStruct->allTasks[i]);
			break;
		}
	}

	if (ret == 0) {
		bwprintf("ERROR: scheduler couldn't find the task!\r\n");
		while (1) {};
	}

	return ret;
}

void schedulerAdd(SchedulerStructure* schedStruct, TaskDescriptor* td) {
	if (td == 0) {
		bwprintf("Error: null task descriptor given to scheduler\n\r");
		return;
	}
	//Not checking priority - it should be enforced further up
	int pri = td->priority;
	SchedulerTask* schedTask = getSchedulerTask(schedStruct, td);
	schedTask->next = 0;
	if (schedStruct->queueTails[pri] == 0) {
		schedStruct->queueHeads[pri] = schedTask;
		schedStruct->queueTails[pri] = schedTask;
	} else {
		schedStruct->queueTails[pri]->next = schedTask;
		schedStruct->queueTails[pri] = schedTask;
	}
}
void schedulerPrintTasks(SchedulerStructure* schedStruct) {
	int i;
	for (i=0; i < TASKS_MAX_PRIORITY; i++) {
		int count =0;
		SchedulerTask* current;
		current = schedStruct->queueHeads[i];
		while (current) {
			current = current->next;
			count++;
		}
		bwprintf("There are %d priority %d tasks.\n\r", count, i+1);
	}
}
//Returns 0 if no tasks are ready.  best to leave an idle task going...
TaskDescriptor* schedule(SchedulerStructure* schedStruct) {
	TaskDescriptor* ret = 0;
	int i;
	for (i=0; i < TASKS_MAX_PRIORITY && ret == 0; i++) {
		SchedulerTask* current = schedStruct->queueHeads[i];
		SchedulerTask* last = 0;
		while (current != 0) {
			if (isTaskReady(current->td)) {
				ret = current->td;
				if (last == 0) { //if this is the current head
					schedStruct->queueHeads[i] = current->next;
					if (current->next == 0) { //this was the last one
						schedStruct->queueTails[i] = 0;
					}
				} else {
					last->next = current->next;
					if (last->next == 0) { //we're the last now
						schedStruct->queueTails[i] = last;
					}
				}
				goto doneLooking;
			}
			last = current;
			current = current->next;
		}
	} // looping over all priorities
	doneLooking:
	return ret;
}
