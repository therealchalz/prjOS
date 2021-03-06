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

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <prjOS/include/base_tasks/serialDriver.h>
#include <prjOS/include/bwio.h>
#include <prjOS/include/hardware_dependent/cpu.h>
#include <prjOS/include/hardware_dependent/cpu_defs.h>
#include <prjOS/include/hardware_dependent/board.h>
#include <prjOS/include/hardware_dependent/interrupts.h>
#include <prjOS/include/debug.h>
#include <prjOS/include/scheduler.h>
#include <prjOS/include/syscall.h>
#include <prjOS/include/sys_syscall.h>
#include <prjOS/include/kernel_data.h>
#include <prjOS/include/base_tasks/nameserver.h>

extern uint32_t _stack_top;

extern void initTask(void* firstTaskfn);

static void createFirstTask(KernelData* kernelData, TaskDescriptor *tds, int count, void* firstTaskFn) {
	TaskCreateParameters params;
	setupDefaultCreateParameters(&params, initTask);
	params.parentId = 0;
	params.priority = 0;
	TaskDescriptor* td = createTask(tds, count, &params);
	td->systemCall.returnValue = (uint32_t)firstTaskFn;
	schedulerAdd(kernelData->schedulerStructure, td);
}

void handleSyscall(KernelData* kernelData, TaskDescriptor* t) {
	switch (t->state) {
	case TASKS_STATE_SYSCALL_BLK:
		break;
	default:
		return;
	}
	switch (t->systemCall.syscall) {
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
		t->systemCall.returnValue = sys_receive(t, kernelData, true);
		break;
	case SYSCALL_RECEIVE_NONBLOCK:
		t->systemCall.returnValue = sys_receive(t, kernelData, false);
		break;
	case SYSCALL_REPLY:
		t->systemCall.returnValue = sys_reply(t, kernelData);
		break;
	case SYSCALL_AWAIT_EVENT:
		sys_awaitEvent(t, kernelData);
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
			setTaskReady(t);
		}
		break;
	case SYSCALL_CREATE_MICROTASK:
		{
			TaskCreateParameters params;
			int r = sys_createMicroTask(t, &params);
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
			setTaskReady(t);
		}
		break;
	case SYSCALL_CHANGEPRIORITY:
		t->systemCall.returnValue = sys_changePriority(t);
		break;
	case SYSCALL_WHOISNS:
		t->systemCall.returnValue = sys_whoIsNs(t, kernelData);
		break;
	case SYSCALL_REGISTERNS:
		t->systemCall.returnValue = sys_registerNs(t, kernelData);
		break;
	case SYSCALL_META_INFO:
		t->systemCall.returnValue = sys_metaInfo(t, kernelData);
		break;
	case SYSCALL_MONOTONIC_MICROS:
		t->systemCall.returnValue = sys_getMonotonicMicros(t, kernelData);
		break;
	}
}

void kernelTasks(TaskDescriptor* activeTask, KernelData* kernelData, uint32_t preemptionReason) {
	if (preemptionReason == 0) {
		//Syscall
		handleSyscall(kernelData, activeTask);
	} else {
		//HW Interrupt, handled by kernel
		handleInterrupt(kernelData, preemptionReason);
	}
}

uint64_t getSystemTimerMicros(KernelData* kernelData) {
	uint64_t ret = 0;
	disableSystemTimer();
	ret = kernelData->systemMicroCount;
	enableSystemTimer();
	return ret;
}

static void checkKernelStack(KernelData* kernelData) {
	uint32_t* sp = getSP();
	uint32_t stackUsage = (uint32_t)kernelData->stackBase - (uint32_t)sp;

	if (stackUsage > KERNEL_STACK_SIZE) {
		bwprintf("KERNEL PANIC: Kernel stack size is 0x%x but it's 0x%x right now! (%d bytes)\n\r", KERNEL_STACK_SIZE, stackUsage, stackUsage);
		while(1) {
			//TODO: something
		}
	}

}


int rtos_main(void* firstTaskFn) {

	uint32_t* stack_top = &_stack_top;

	cpuInit();	/* Clock and other configuration */

	//Put the TDs on the kernel stack
	TaskDescriptor taskDescriptors[KERNEL_NUMBER_OF_TASK_DESCRIPTORS];
	initializeTds(taskDescriptors, KERNEL_NUMBER_OF_TASK_DESCRIPTORS, stack_top);

	//bwprintf("Tasks are taking %d bytes on the kernel stack\n\r", (KERNEL_NUMBER_OF_TASK_DESCRIPTORS)*sizeof(TaskDescriptor));
	//Scheduler stuff
	SchedulerStructure schedStruct;
	schedulerInit(&schedStruct, taskDescriptors);

	EventData eventData;
	memset(&eventData, 0, sizeof(EventData));

	KernelData kernelData;
	memset(&kernelData, 0, sizeof(KernelData));
	kernelData.taskDescriptorList = taskDescriptors;
	kernelData.tdCount = KERNEL_NUMBER_OF_TASK_DESCRIPTORS;
	kernelData.schedulerStructure = &schedStruct;
	kernelData.eventData = &eventData;
	kernelData.systemMicroCount = 0;
	kernelData.stackBase = stack_top;

	boardInit(&kernelData);	/* Initializes all the other board and
						application specific CPU and board peripherals */

	bwprintf("\n********Kernel Starting********\n\r\n");
	//bwprintf("Stack Base: %x",(stackBasePtr));
	cpuPrintInfo();
	printCEnvironmentSettings();
	bwprintf("Kernel structures are taking %d bytes.\n\r", ((KERNEL_NUMBER_OF_TASK_DESCRIPTORS)*sizeof(TaskDescriptor) + sizeof(SchedulerStructure) + sizeof(KernelData)));

	//Create first tasks
	createFirstTask(&kernelData, taskDescriptors, KERNEL_NUMBER_OF_TASK_DESCRIPTORS, firstTaskFn);

	TaskDescriptor* currentTask = 0;
	uint32_t preemptionReason = 0;
	uint64_t systemTimeMicros = 0;

	while(1)
	{
		//bwprintf("Getting task to schedule...\n\r");
		//schedulerPrintTasks(&schedStruct);
		if (currentTask != 0) {
			if (currentTask->systemTimeEntryMicros != 0) {
				systemTimeMicros = getSystemTimerMicros(&kernelData);
				currentTask->systemTimeMicros += (systemTimeMicros - currentTask->systemTimeEntryMicros);
			}
		}
		currentTask = schedule(&schedStruct);
		//bwprintf("Scheduler gave us task %d\n\r", currentTask->taskId);

		if (currentTask != 0) {
			//bwprintf("%d Kernel switching to task...(td @ %x)\r\n", loopCount, currentTask);
			systemTimeMicros = getSystemTimerMicros(&kernelData);
			currentTask->userTimeEntryMicros = systemTimeMicros;
			currentTask->contextSwitchCount = currentTask->contextSwitchCount + 1;

			checkKernelStack(&kernelData);

			cpuPreemptionTimerEnable();
			preemptionReason = prjTaskSwitch(currentTask);
			cpuPreemptionTimerDisable();

			systemTimeMicros = getSystemTimerMicros(&kernelData);
			currentTask->userTimeMicros += systemTimeMicros - currentTask->userTimeEntryMicros;

			if (preemptionReason == 0) {
				//Only count the kernel time here if we did a syscall; if it is a regular HW interrupt
				// then it doesn't count for the current task
				currentTask->systemTimeEntryMicros = systemTimeMicros;
			} else {
				currentTask->systemTimeEntryMicros = 0;
			}

			if (taskStackOverflow(currentTask)) {
				bwprintf("KERNEL: Killing task %d due to overflow\n\r", currentTask->taskId);
				taskKill(currentTask);
			}

			//bwprintf("Kernel running.  Prepemtion Reason: %d\r\n", preemptionReason);
			if (!hasExited(currentTask)) {
				kernelTasks(currentTask, &kernelData, preemptionReason);
				schedulerAdd(&schedStruct, currentTask);
			} else {
				//bwprintf("Not adding task to scheduler - it has quit\n\r");
			}
		}
	}
	bwprintf("System Halted\n\r");
	return 0;
}

