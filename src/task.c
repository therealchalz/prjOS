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

#include <prjOS/include/task.h>


void setupDefaultCreateParameters(TaskCreateParameters *params, void* taskEntry) {
	memset(params, 0, sizeof(TaskCreateParameters));

	params->stackPointer = 0;	//use default
	params->taskType = TASK_TYPE_REGULAR;	//Regular stack size

	cpuSetupTaskDefaultParameters(&(params->cpuSpecific), taskEntry);
}

void reinitializeTd(TaskDescriptor* td, uint32_t* stackBase) {

	task_id_t oldId = td->taskId;
	uint32_t idx = oldId & TASKS_ID_INDEX_MASK;
	uint32_t stackPointer = (uint32_t)td->stackBase;
	uint32_t taskType = td->taskType;

	memset(td, 0, sizeof(TaskDescriptor));

	if (stackBase != 0) {
		uint32_t stackOffset;
		if (idx >= 0 && idx < KERNEL_NUMBER_OF_TASKS) {
			taskType = TASK_TYPE_REGULAR;
			stackOffset = KERNEL_STACK_SIZE + (idx * KERNEL_TASK_DEFAULT_STACK_SIZE);
		} else if (idx >= KERNEL_NUMBER_OF_TASKS && idx < KERNEL_NUMBER_OF_MICROTASKS+KERNEL_NUMBER_OF_TASKS) {
			taskType = TASK_TYPE_MICRO;
			stackOffset = KERNEL_STACK_SIZE +
					(KERNEL_NUMBER_OF_TASKS * KERNEL_TASK_DEFAULT_STACK_SIZE) +
					((idx-KERNEL_NUMBER_OF_TASKS) * KERNEL_MICROTASK_DEFAULT_STACK_SIZE);
		}
		stackPointer = ((uint32_t)stackBase - stackOffset);

		//Align to 32-bit boundary if required
		if (stackPointer != (stackPointer & ~0x3)) {
			stackPointer = stackPointer & ~0x3;
		}
	}

	td->taskType = taskType;
	td->taskId = oldId;
	td->stackPointer = (uint32_t*)stackPointer;
	td->stackBase = (uint32_t*)stackPointer;
	td->state = TASKS_STATE_INVALID;
}

void initializeTds(TaskDescriptor* tds, uint32_t count, uint32_t* stackBase) {
	memset(tds, 0, count*sizeof(TaskDescriptor));
	uint16_t i;
	for (i=0; i<count; i++) {
		tds[i].taskId = i;
		reinitializeTd(&tds[i], stackBase);
	}
}
void printTd(TaskDescriptor* td, uint32_t stackAmount) {
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

uint32_t isTaskReady(TaskDescriptor* td) {
	uint32_t ret = 0;
	switch (td->state) {
	//add other states as required
	case TASKS_STATE_RUNNING:
	case TASKS_STATE_HWINT:
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

uint32_t isTdAvailable(TaskDescriptor* td) {
	switch (td->state) {
	case TASKS_STATE_INVALID:
	case TASKS_STATE_EXITED:
		return 1;
		break;
	}
	return 0;
}

uint32_t hasExited(TaskDescriptor* td) {
	if (td->state == TASKS_STATE_EXITED)
		return 1;
	return 0;
}

TaskDescriptor* findTd(task_id_t td, TaskDescriptor* tdList, uint32_t count) {
	TaskDescriptor* ret = 0;

	uint32_t tdIndex = td & TASKS_ID_INDEX_MASK;

	if (tdIndex >= count)
		return 0;

	ret = &tdList[tdIndex];

	if (ret->taskId == td)
		return ret;

	return 0;
}

TaskDescriptor* createTask(TaskDescriptor *tds, uint32_t count, const TaskCreateParameters *parms) {
	// Search for available task descriptor
	uint32_t i;
	TaskDescriptor* ret = 0;
	uint32_t minIdx, maxIdx;
	switch (parms->taskType) {
	case TASK_TYPE_REGULAR:
		minIdx = 0;
		maxIdx = KERNEL_NUMBER_OF_TASKS-1;
		break;
	case TASK_TYPE_MICRO:
		minIdx = KERNEL_NUMBER_OF_TASKS;
		maxIdx = KERNEL_NUMBER_OF_MICROTASKS+KERNEL_NUMBER_OF_TASKS-1;
		break;
	}
	for (i=minIdx; i<=maxIdx; i++) {
		if (isTdAvailable(&tds[i])) {
			ret = &tds[i];
			break;
		}
	}

	if (ret != 0) {
		reinitializeTd(ret, 0);

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

uint16_t getNumRegularTasks(TaskDescriptor* tdList, uint32_t count) {
	int i;
	uint16_t ret = 0;
	for (i = 0; i<KERNEL_NUMBER_OF_TASKS; i++) {
		if (!isTdAvailable(&tdList[i])) {
			ret++;
		}
	}
	return ret;
}
uint16_t getNumMicroTasks(TaskDescriptor* tdList, uint32_t count) {
	int i;
	uint16_t ret = 0;
	for (i = KERNEL_NUMBER_OF_TASKS; i<KERNEL_NUMBER_OF_MICROTASKS+KERNEL_NUMBER_OF_TASKS; i++) {
		if (!isTdAvailable(&tdList[i])) {
			ret++;
		}
	}
	return ret;
}
uint32_t getRegularTaskStackSize() {
	return KERNEL_TASK_DEFAULT_STACK_SIZE;
}
uint32_t getMicroTaskStackSize() {
	return KERNEL_MICROTASK_DEFAULT_STACK_SIZE;
}

uint32_t getParentTid(TaskDescriptor* tdList, uint32_t count, uint32_t tid) {
	TaskDescriptor* task = findTd(tid, tdList, count);
	if (task == 0) {
		return 0;
	}
	return task->parentId;
}

uint32_t* getStackBase(TaskDescriptor* tdList, uint32_t count, uint32_t tid) {
	TaskDescriptor* task = findTd(tid, tdList, count);
	if (task == 0) {
		return 0;
	}
	return task->stackBase;
}

uint32_t* getStackHead(TaskDescriptor* tdList, uint32_t count, uint32_t tid) {
	TaskDescriptor* task = findTd(tid, tdList, count);
	if (task == 0) {
		return 0;
	}
	return task->stackPointer;
}

uint32_t getTaskState(TaskDescriptor* tdList, uint32_t count, uint32_t tid) {
	TaskDescriptor* task = findTd(tid, tdList, count);
	if (task == 0) {
		return 0;
	}
	return task->state;
}

uint8_t getTaskType(TaskDescriptor* tdList, uint32_t count, uint32_t tid) {
	TaskDescriptor* task = findTd(tid, tdList, count);
	if (task == 0) {
		return 0;
	}
	return task->taskType;
}
