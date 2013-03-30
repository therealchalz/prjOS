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

#include <hardware_dependent/cpu_defs.h>

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

#define TASKS_MAX_PRIORITY	    7

#define TASKS_ID_GENERATION_MASK	0xFFFF0000
#define TASKS_ID_INDEX_MASK			0x0000FFFF
#define TASKS_ID_INVALID			0x0000FFFF

typedef struct SystemCall {
	int syscall;	//0 means hardware interrupted, anything else = syscall
	int returnValue;
	int param1;
	int param2;
	int param3;
	int param4;
	int param5;
	int param6;
	//I'm drawing the line at 6...
} SystemCall;

typedef struct TaskDescriptor {
	unsigned int taskId;
	unsigned int parentId;
	int* stackPointer;
	SystemCall systemCall;
	unsigned int state;
	unsigned int priority;
} TaskDescriptor;

typedef struct TaskCreateParameters {
	unsigned int taskId;		//OS-assigned task id
	unsigned int parentId;		//OS-assigned task parent
	int* stackPointer;
	int priority;
	//Hardware-dependent parameters
	TaskCpuCreateParameters cpuSpecific;
} TaskCreateParameters;

void initializeTds(TaskDescriptor* tds, int count);
void printTd(TaskDescriptor* td, int stackAmount);
void printSystemCall(SystemCall* sc);
TaskDescriptor* createTask(TaskDescriptor *tds, int count, const TaskCreateParameters *parms);
void setupDefaultCreateParameters(TaskCreateParameters *params, void* taskEntry);
int isTaskReady(TaskDescriptor* td);
void setTaskReady(TaskDescriptor* td);
void taskExit(TaskDescriptor* td);
int hasExited(TaskDescriptor* td);

#endif /* TASK_H_ */
