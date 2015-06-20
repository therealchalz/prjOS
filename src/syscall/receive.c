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

#include <prjOS/include/sys_syscall.h>

uint32_t prjReceive(task_id_t *tid, uint8_t *msg, uint32_t msgLen) {
	asm (svcArg(SYSCALL_RECEIVE));
	uint32_t ret;
	asm (" MOV %[ret], R0\n": [ret] "=r" (ret): :);
	return ret;
}

uint32_t prjReceiveNonBlocking(task_id_t *tid, uint8_t *msg, uint32_t msgLen) {
	asm (svcArg(SYSCALL_RECEIVE_NONBLOCK));
	uint32_t ret;
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

uint32_t sys_receive(TaskDescriptor* active, KernelData * kData, bool blocking){
	//NOTE: The return value can also be set from Send();ERR_RECEIVE_BUFFER_TOO_SMALL

	uint32_t destLen = active->systemCall.param3;
	uint32_t* tid = (uint32_t*)active->systemCall.param1;
	uint8_t* dest = (uint8_t*) active->systemCall.param2;

	//bwprintf("RECEIVE: DEBUG: Receiving... Message length: %d\n\r", destLen);

	/* Check if buffer is valid */
	if (dest == 0)
		return ERR_RECEIVE_BAD_BUFFER;

	TaskDescriptor* senderTask = pop(active);
	uint32_t ret = 0;

	if (senderTask!=0) {

		//bwprintf("RECEIVE: DEBUG: Sender is already ready.\n\r");

		// Set the sender to state reply blocked
		senderTask->state = TASKS_STATE_RPLY_BLK;

		//Copy message to receiver's buffer
		uint32_t sourceSize = senderTask->systemCall.param3;
		uint8_t* source = (uint8_t*)senderTask->systemCall.param2;

		uint32_t size = sourceSize;
		if (size > destLen) {
			size = destLen;
		}

		memcpy(dest, source, size);

		//bwprintf("RECEIVE: DEBUG: Trying to receive\n\r");

		// Set this task's state to running
		active->state = TASKS_STATE_RUNNING;
		// Set tid
		*tid = senderTask->taskId;

		//bwprintf("RECEIVE: DEBUG: senderTaskId: %d\n\r", senderTask->taskId);

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

		if (blocking) {

			//bwprintf("RECEIVE: DEBUG: Sender is not ready, blocking.\n\r");

			// Set state to send blocked
			active->state = TASKS_STATE_SEND_BLK;
		} else {
			//bwprintf("RECEIVE: DEBUG: Sender is not ready, but not blocking\n\r");

			active->state = TASKS_STATE_RUNNING;

			ret = ERR_RECEIVE_NOSEND;
		}
	}

	//bwprintf("RECEIVE: DEBUG: Done Receiving...\n\r");

	return ret;
}

