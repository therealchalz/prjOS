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
 * task.h
 */

#ifndef TASK_H_
#define TASK_H_

#include <stdbool.h>
#include <stdint.h>
#include "prjOS/include/hardware_dependent/cpu_defs.h"

//These may be hardcoded in some hardware dependent
//places so they shouldn't be changed.  Adding shouldn't
//be a problem though
#define TASKS_STATE_INVALID		0
#define TASKS_STATE_RUNNING		1
#define TASKS_STATE_EXITED		2
#define TASKS_STATE_RCVD_BLK	3
#define TASKS_STATE_SEND_BLK	4
#define TASKS_STATE_RPLY_BLK	5
#define TASKS_STATE_EVT_BLK		6
#define TASKS_STATE_SYSCALL_BLK	7
#define TASKS_STATE_HWINT		8

#define TASKS_MAX_PRIORITY	    7

#define TASKS_ID_GENERATION_MASK		0xFFFFF000
#define TASKS_ID_GENERATION_INCREMENT	0x00001000
#define TASKS_ID_GENERATION_SHIFTBITS	12
#define TASKS_ID_INDEX_MASK				0x00000FFF

#define TASKS_ID_INVALID		0

#define TASK_TYPE_REGULAR		1
#define TASK_TYPE_MICRO			2

typedef struct SystemCall {
	uint32_t syscall;	//0 means hardware interrupted, anything else = syscall
	uint32_t returnValue;
	uint32_t param1;
	uint32_t param2;
	uint32_t param3;
	uint32_t param4;
	uint32_t param5;
	uint32_t param6;
	//I'm drawing the line at 6...
} SystemCall;

typedef struct TaskDescriptor {
	uint32_t taskId;
	uint32_t parentId;
	uint32_t* stackPointer;
	SystemCall systemCall;
	uint32_t state;
	uint32_t priority;
	uint32_t taskType;
	struct TaskDescriptor* sendQueueNext;
	struct TaskDescriptor* sendQueueHead;
	struct TaskDescriptor* sendQueueTail;
} TaskDescriptor;

typedef struct TaskCreateParameters {
	uint32_t parentId;		//OS-assigned task parent task Id
	uint32_t* stackPointer;
	uint32_t priority;
	uint32_t taskType;
	//Hardware-dependent parameters
	TaskCpuCreateParameters cpuSpecific;
} TaskCreateParameters;

void initializeTds(TaskDescriptor* tds, uint32_t count, uint32_t* stackBase);
void printTd(TaskDescriptor* td, uint32_t stackAmount);
void printSystemCall(SystemCall* sc);
TaskDescriptor* createTask(TaskDescriptor *tds, uint32_t count, const TaskCreateParameters *parms);
void setupDefaultCreateParameters(TaskCreateParameters *params, void* taskEntry);
uint32_t isTaskReady(TaskDescriptor* td);
void setTaskReady(TaskDescriptor* td);
void taskExit(TaskDescriptor* td);
uint32_t hasExited(TaskDescriptor* td);
TaskDescriptor* findTd(int td, TaskDescriptor* tdList, uint32_t count);

#endif /* TASK_H_ */
