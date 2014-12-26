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
 * task.c
 */

#include "prjOS/include/task.h"
#include "prjOS/include/hardware_dependent/cpu_defs.h"
#include "prjOS/include/debug.h"
#include "prjOS/include/bwio.h"
#include "prjOS/include/hardware_dependent/cpu.h"
#include "string.h"
#include "prjOS/include/syscall.h"

void setupDefaultCreateParameters(TaskCreateParameters *params, void* taskEntry) {
	memset(params, 0, sizeof(TaskCreateParameters));

	params->stackPointer = 0;	//use default

	cpuSetupTaskDefaultParameters(&(params->cpuSpecific), taskEntry);
}

void reinitializeTd(TaskDescriptor* td, int* stackBase) {
	int oldId = td->taskId;
	int idx = oldId & TASKS_ID_INDEX_MASK;
	int stackOffset = KERNEL_STACK_SIZE + (idx * KERNEL_TASK_DEFAULT_STACK_SIZE);
	int* stackPointer = (int*)(stackBase - stackOffset);
	memset(td, 0, sizeof(TaskDescriptor));
	td->taskId = oldId;
	td->stackPointer = stackPointer;
}

void initializeTds(TaskDescriptor* tds, int count, int* stackBase) {
	memset(tds, 0, count*sizeof(TaskDescriptor));

	int stackOffset = KERNEL_STACK_SIZE;
	int i;
	for (i=0; i<count; i++) {
		tds[i].stackPointer = (int*)(stackBase - stackOffset);
		tds[i].state = TASKS_STATE_INVALID;
		tds[i].taskId = i;
		stackOffset += KERNEL_TASK_DEFAULT_STACK_SIZE;
	}
}
void printTd(TaskDescriptor* td, int stackAmount) {
	bwprintf("Task %d\r\n", td->taskId);
	bwprintf("  Parent: %d\r\n", td->parentId);
	//bwprintf("  Program Counter: %x\r\n", td->programCounter);
	bwprintf("  Stack Location: %x\r\n", td->stackPointer);
	bwprintf("  Priority: %d\r\n", td->priority);
	bwprintf("  State: %d\r\n", td->state);
	bwprintf("  TOS:\r\n");
	printStackTop((char*)td->stackPointer, stackAmount);
}

void printSystemCall(SystemCall* sc) {
	bwprintf("SystemCall: %d\n\r", sc->syscall);
	if (sc->syscall == SYSCALL_HARDWARE_CALL) {
		bwprintf("  Hardware interrupt; no call data\n\r");
	} else {
		bwprintf("  Parameter 1: %d\n\r", sc->param1);
		bwprintf("  Parameter 2: %d\n\r", sc->param2);
		bwprintf("  Parameter 3: %d\n\r", sc->param3);
		bwprintf("  Parameter 4: %d\n\r", sc->param4);
		bwprintf("  Parameter 5: %d\n\r", sc->param5);
		bwprintf("  Parameter 6: %d\n\r", sc->param6);
	}
}

int isTaskReady(TaskDescriptor* td) {
	int ret = 0;
	switch (td->state) {
	//add other states as required
	case TASKS_STATE_RUNNING:
		ret = 1;
		break;
	}
	return ret;
}

void setTaskReady(TaskDescriptor* td) {
	td->state = TASKS_STATE_RUNNING;
}

void taskExit(TaskDescriptor* td) {
	td->state = TASKS_STATE_EXITED;
}

int isTdAvailable(TaskDescriptor* td) {
	switch (td->state) {
	case TASKS_STATE_INVALID:
	case TASKS_STATE_EXITED:
		return 1;
		break;
	}
	return 0;
}

int hasExited(TaskDescriptor* td) {
	if (td->state == TASKS_STATE_EXITED)
		return 1;
	return 0;
}

TaskDescriptor* findTd(int td, TaskDescriptor* tdList, int count) {
	TaskDescriptor* ret = 0;

	int tdIndex = td & TASKS_ID_INDEX_MASK;

	if (tdIndex >= count)
		return 0;

	ret = &tdList[tdIndex];

	if (ret->taskId == td)
		return ret;

	return 0;
}

TaskDescriptor* createTask(TaskDescriptor *tds, int count, const TaskCreateParameters *parms) {
	// Search for available task descriptor
	int i;
	TaskDescriptor* ret = 0;
	for (i=0; i<count; i++) {
		if (isTdAvailable(&tds[i])) {
			ret = &tds[i];
			break;
		}
	}

	if (ret != 0) {
		//TODO:
		//reinitializeTd(ret);

		ret->taskId += TASKS_ID_GENERATION_INCREMENT;
		ret->parentId = parms->parentId;
		if (parms->stackPointer != 0)
			ret->stackPointer = parms->stackPointer;

		ret->stackPointer = cpuCreateTask(ret->stackPointer, parms);

		ret->state = TASKS_STATE_RUNNING;
		ret->priority = parms->priority;

	} else {
		bwprintf("PANIC: Ran out task descriptor space");
	}

	return ret;
}
