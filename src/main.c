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
#include <base_tasks/nameserver.h>

#ifdef DEBUG
void __error__(char *filename, unsigned long line) {
	cpuBarf();
	bwprintf("Driver Crash: file: %s, line: %u", filename, line);
	//Keep this at the end because it may cause a hard fault
	printCurrentStackTop(16);
	while(1) {}
}
#endif

void testTask() {
	int tid = prjGetTid();
	int pid = prjGetParentTid();

	bwprintf("Task Running.  TaskID: %d.  Parent: %d\n\r", tid, pid);

	prjYield();

	bwprintf("Task Running.  TaskID: %d.  Parent: %d\n\r", tid, pid);

	prjExit();
}

void firstUserTask() {
	bwprintf("First User Task Created %d\n\r", prjCreate(2, &testTask));
	bwprintf("First User Task Created %d\n\r", prjCreate(2, &testTask));
	bwprintf("First User Task Created %d\n\r", prjCreate(0, &testTask));
	bwprintf("First User Task Created %d\n\r", prjCreate(0, &testTask));
	bwprintf("First Exiting\n\r");
	prjExit();
}

void firstTask() {
	bwprintf("Init Task Starting...\n\r");
	int nameserver = prjCreate(3, &nameserverEntry);
	bwprintf("Init created nameserver: %d\n\r", nameserver);
	bwprintf("Init created %d\n\r", prjCreate(1, &firstUserTask));
	prjChangePriority(TASKS_MAX_PRIORITY-1);
	int x = 5;
	while(x > 0) {
		prjYield();
		x--;
	}
	NameserverQuery send;
	NameserverQuery receive;
	strcpy(send.buffer, "TESTING! 420?");
	send.bufferLen = strlen("TESTING! 420?");
	send.operation = NAMESERVER_OPERATION_EXIT;
	bwprintf("Quit command to nameserver returned: %d\n\r", prjSend(nameserver, (char*)&send, sizeof(NameserverQuery), (char*)&receive, sizeof(NameserverQuery)));
	bwprintf("Idle Task quitting\r\n");
	prjExit();
}

TaskDescriptor* createFirstTask(TaskDescriptor *tds, int count) {
	TaskCreateParameters params;
	setupDefaultCreateParameters(&params, &firstTask);
	params.parentId = 0;
	params.priority = 0;
	return createTask(tds, count, &params);
}

void TaskSwitch(TaskDescriptor* t) {
	asm (" MOV R0, %[td]": :[td] "r" (t));
	asm (svcArg(0));
}

void handleSyscall(TaskDescriptor* t, KernelData* kernelData) {
	//TODO: if this TD has nothing to to, look for one that has waiting work
	if (t->state != TASKS_STATE_SYSCALL_BLK) {
		return;
	}
	switch (t->systemCall.syscall) {
	case SYSCALL_HARDWARE_CALL:
		break;
	case SYSCALL_GET_TID:
		t->systemCall.returnValue = sys_getTid(t);
		break;
	case SYSCALL_GET_PARENT_TID:
		t->systemCall.returnValue = sys_getParentTid(t);
		break;
	case SYSCALL_YIELD:
		t->systemCall.returnValue = sys_yield(t);
		break;
	case SYSCALL_THREADEXIT:
		t->systemCall.returnValue = sys_exit(t);
		break;
	case SYSCALL_SEND:
		t->systemCall.returnValue = sys_send(t, kernelData);
		break;
	case SYSCALL_RECEIVE:
		t->systemCall.returnValue = sys_receive(t, kernelData);
		break;
	case SYSCALL_REPLY:
		t->systemCall.returnValue = sys_reply(t, kernelData);
		break;
	case SYSCALL_CREATE:
		//This one receives special attention
		{
			TaskCreateParameters params;
			int r = sys_create(t, &params);
			if (r) { //error
				t->systemCall.returnValue = r;
				break;
			}
			TaskDescriptor* theNewTask = createTask(kernelData->taskDescriptorList, kernelData->tdCount, &params);
			if (theNewTask == 0) {
				t->systemCall.returnValue = ERR_CREATE_NO_SPACE;
				break;
			}
			schedulerAdd(kernelData->schedulerStructure, theNewTask);
			t->systemCall.returnValue = theNewTask->taskId;
		}
		setTaskReady(t);
		break;
	case SYSCALL_CHANGEPRIORITY:
		t->systemCall.returnValue = sys_changePriority(t);
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
			prjTaskSwitch(currentTask);
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

