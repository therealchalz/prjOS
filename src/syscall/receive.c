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
 * receive.c
 */

#include "prjOS/include/syscall.h"
#include "prjOS/include/task.h"
#include "prjOS/include/kernel_data.h"
#include "string.h"
#include "prjOS/include/bwio.h"

int prjReceive(int *tid, char *msg, int msgLen) {
	int ret;
	asm (svcArg(SYSCALL_RECEIVE));
	asm (" MOV %[ret], R0\n": [ret] "=r" (ret): :);
	return ret;
}

static TaskDescriptor* pop(TaskDescriptor* active){
	if(active->sendQueueHead!=0){
	  TaskDescriptor * task = (TaskDescriptor *)active->sendQueueHead;
	  active->sendQueueHead = task->sendQueueNext;
	  task->sendQueueNext = 0;
	  return task;
	}
	return 0;
}

int sys_receive(TaskDescriptor* active, KernelData * kData){
	//NOTE: The return value can also be set from Send();ERR_RECEIVE_BUFFER_TOO_SMALL

	int destLen = active->systemCall.param3;
	int* tid = (int*)active->systemCall.param1;
	char* dest = (char*) active->systemCall.param2;

	bwprintf("RECEIVE: DEBUG: Receiving... Message length: %d\n\r", destLen);

	/* Check if buffer is valid */
	if (dest == 0)
		return ERR_RECEIVE_BAD_BUFFER;

	TaskDescriptor* senderTask = pop(active);
	int ret = 0;

	if (senderTask!=0) {

		bwprintf("RECEIVE: DEBUG: Sender is already ready.\n\r");

		// Set the sender to state reply blocked
		senderTask->state = TASKS_STATE_RPLY_BLK;

		//Copy message to receiver's buffer
		int sourceSize = senderTask->systemCall.param3;
		char* source = (char*)senderTask->systemCall.param2;

		int size = sourceSize;
		if (size > destLen) {
			size = destLen;
		}

		memcpy(dest, source, size);

		bwprintf("RECEIVE: DEBUG: Trying to receive\n\r");

		// Set this task's state to running
		active->state = TASKS_STATE_RUNNING;
		// Set tid
		*tid = senderTask->taskId;

		bwprintf("RECEIVE: DEBUG: senderTaskId: %d\n\r", senderTask->taskId);

		if(size != sourceSize){
			ret = ERR_RECEIVE_BUFFER_TOO_SMALL;
			senderTask->systemCall.returnValue=ERR_SEND_INCOMPLETE;
		}
		else {
			ret = size;
			senderTask->systemCall.returnValue=size;
		}

		//Check if our priority should be reset
		//if (active->currentPriority != active->defaultPriority)
		//	active->currentPriority = active->defaultPriority;


	} else {

		bwprintf("RECEIVE: DEBUG: Sender is not ready, blocking.\n\r");

		// Set state to send blocked
		active->state = TASKS_STATE_SEND_BLK;
	}

	bwprintf("RECEIVE: DEBUG: Done Receiving...\n\r");

	return ret;
}

