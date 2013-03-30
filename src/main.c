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


#include <hardware_dependent/uart.h>
#include <bwio.h>
#include <hardware_dependent/cpu.h>
#include <hardware_dependent/cpu_defs.h>
#include <hardware_dependent/board.h>
#include <debug.h>
#include <scheduler.h>
#include <syscall.h>
#include <sys_syscall.h>
#include <kernel_data.h>
#include <string.h>

#ifdef DEBUG
void __error__(char *filename, unsigned long line) {
	cpuBarf();
	bwprintf("Driver Crash: file: %s, line: %u", filename, line);
	//Keep this at the end because it may cause a hard fault
	printCurrentStackTop(16);
	while(1) {}
}
#endif

int testParameterFunction(int p1, int p2, int p3, int p4, int p5, int p6) {
	int ret;
	asm (svcArg(1));
	asm (" MOV %[ret], R0\n": [ret] "=r" (ret): :);
	return ret;
}

void testTask() {
	int tid = getTid();
	int pid = getParentTid();

	bwprintf("Task Running.  TaskID: %d.  Parent: %d\n\r", tid, pid);

	yield();

	bwprintf("Task Running.  TaskID: %d.  Parent: %d\n\r", tid, pid);

	threadExit();
}

void firstUserTask() {
	bwprintf("First User Task Created %d\n\r", create(2, &testTask));
	bwprintf("First User Task Created %d\n\r", create(2, &testTask));
	bwprintf("First User Task Created %d\n\r", create(0, &testTask));
	bwprintf("First User Task Created %d\n\r", create(0, &testTask));
	bwprintf("First Exiting\n\r");
	threadExit();
}

void firstTask() {
	bwprintf("Init Task Starting...\n\r");
	bwprintf("Init created %d\n\r", create(1, &firstUserTask));
	changePriority(TASKS_MAX_PRIORITY-1);
	while(1) {
		yield();
	}
}

TaskDescriptor* createFirstTask(TaskDescriptor *tds, int count) {
	TaskCreateParameters params;
	setupDefaultCreateParameters(&params, &firstTask);
	params.parentId = 0;
	params.taskId = 1;
	params.priority = 0;
	return createTask(tds, count, &params);
}

void TaskSwitch(TaskDescriptor* t) {
	asm (" MOV R0, %[td]": :[td] "r" (t));
	asm (svcArg(0));
}

void handleSyscall(TaskDescriptor* t, KernelData* kernelData) {
	//TODO: if this TD has nothing to to, look for one that has waiting work
	if (isTaskReady(t)) {
		return;
	}
	switch (t->systemCall.syscall) {
	case SYSCALL_HARDWARE_CALL:
		break;
	case SYSCALL_GET_TID:
		t->systemCall.returnValue = sys_getTid(t);
		setTaskReady(t);
		break;
	case SYSCALL_GET_PARENT_TID:
		t->systemCall.returnValue = sys_getParentTid(t);
		setTaskReady(t);
		break;
	case SYSCALL_YIELD:
		t->systemCall.returnValue = sys_yield(t);
		setTaskReady(t);
		break;
	case SYSCALL_THREADEXIT:
		t->systemCall.returnValue = sys_threadexit(t);
		//Don't set the task as ready - it has quit
		break;
	case SYSCALL_CREATE:
		//This one receives special attention
		{
			TaskCreateParameters params;
			sys_create(t, &params);
			params.taskId = kernelData->nextTaskId++;
			TaskDescriptor* theNewTask = createTask(kernelData->taskDescriptorList, kernelData->tdCount, &params);
			schedulerAdd(kernelData->schedulerStructure, theNewTask);
			t->systemCall.returnValue = theNewTask->taskId;
		}
		setTaskReady(t);
		break;
	case SYSCALL_CHANGEPRIORITY:
		t->systemCall.returnValue = sys_changePriority(t);
		setTaskReady(t);
		break;
	}
}

int main(void) {

	cpuInit();

	boardInit();

	UARTInit(115200);

	bwprintf("\n\n\n********Kernel Starting********\n\r\n");
	cpuPrintInfo();
	printCEnvironmentSettings();

	//Put the TDs on the kernel stack
	TaskDescriptor taskDescriptors[KERNEL_MAX_NUMBER_OF_TASKS];
	initializeTds(taskDescriptors, KERNEL_MAX_NUMBER_OF_TASKS);

	bwprintf("Tasks are taking %d bytes on the kernel stack\n\r", (KERNEL_MAX_NUMBER_OF_TASKS)*sizeof(TaskDescriptor));
	//Scheduler stuff
	SchedulerStructure schedStruct;
	schedulerInit(&schedStruct, taskDescriptors);

	KernelData kernelData;
	memset(&kernelData, 0, sizeof(KernelData));
	kernelData.taskDescriptorList = taskDescriptors;
	kernelData.tdCount = KERNEL_MAX_NUMBER_OF_TASKS;
	kernelData.nextTaskId = 2;
	kernelData.schedulerStructure = &schedStruct;

	bwprintf("Kernel structures are taking %d bytes.\n\r", ((KERNEL_MAX_NUMBER_OF_TASKS)*sizeof(TaskDescriptor) + sizeof(SchedulerStructure) + sizeof(KernelData)));

	//Create first task
	TaskDescriptor* currentTask = createFirstTask(taskDescriptors, KERNEL_MAX_NUMBER_OF_TASKS);

	schedulerAdd(&schedStruct, currentTask);

	int testLoop = 100;

	while(testLoop >= 0)
	{
		testLoop--;
		//bwprintf("Getting task to schedule...\n\r");
		//schedulerPrintTasks(&schedStruct);
		currentTask = schedule(&schedStruct);
		//bwprintf("Scheduler gave us task %d\n\r", currentTask->taskId);

		if (currentTask != 0) {
			//bwprintf("Kernel switching to task...\r\n");
			TaskSwitch(currentTask);
			//bwprintf("Kernel running...\r\n");
			handleSyscall(currentTask, &kernelData);
			if (!hasExited(currentTask)) {
				schedulerAdd(&schedStruct, currentTask);
			} else {
				//bwprintf("Not adding task to scheduler - it has quit\n\r");
			}
		}

		boardSetIndicatorLED(1);
		long l = 0x0000ffff;
		while (l > 0) {
			l--;
		}
		boardSetIndicatorLED(0);
		l = 0x0000ffff;
		while (l > 0) {
			l--;
		}
		boardSetIndicatorLED(0);
	}
	bwprintf("System Halted\n\r");
	while(1) {}

	return 0x420;
}

