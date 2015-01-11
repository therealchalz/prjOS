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
 * send.c
 */

#include "prjOS/include/syscall.h"
#include "prjOS/include/task.h"
#include "prjOS/include/kernel_data.h"
#include "string.h"
#include "prjOS/include/bwio.h"

uint32_t prjSend(uint32_t tid, uint8_t *msg, uint32_t msgLen, uint8_t *reply, uint32_t replyLen) {
	asm (svcArg(SYSCALL_SEND));
	uint32_t ret;
	asm (" MOV %[ret], R0\n": [ret] "=r" (ret): :);
	return ret;
}

static void push(TaskDescriptor* active, TaskDescriptor* receiving){
	if(receiving->sendQueueHead==0){
	  receiving->sendQueueHead=active;
	  receiving->sendQueueTail=active;
	  active->sendQueueNext=0;
	}
	else{
	  TaskDescriptor* lastTask = (TaskDescriptor*)receiving->sendQueueTail;
	  lastTask->sendQueueNext=active;
	  receiving->sendQueueTail=active;
	  active->sendQueueNext=0;
	}
}


uint32_t sys_send(TaskDescriptor* active, KernelData * kData) {
	//NOTE: The return value can also be set from Reply();ERR_SEND_INCOMPLETE

	//bwprintf("SEND: DEBUG: Sending...\n\r");

	uint32_t ret = 0;

	/* Do error checking on arguments */
	/* Is the tid possible? */
	uint32_t tid = active->systemCall.param1;
	uint32_t generationId = (tid & TASKS_ID_GENERATION_MASK) >> TASKS_ID_GENERATION_SHIFTBITS;
	uint32_t taskIndex = tid & TASKS_ID_INDEX_MASK;

	if (generationId == 0 || taskIndex < 0 || taskIndex >= kData->tdCount) {
		active->state = TASKS_STATE_RUNNING;
		return ERR_SEND_TASKID_DNE;
	}

	/* Does the task exist? */
	TaskDescriptor* receiveTask = findTd(tid, kData->taskDescriptorList, kData->tdCount);
	if (receiveTask == 0) {
		active->state = TASKS_STATE_RUNNING;
		return ERR_SEND_TASKID_NOTFOUND;
	}

	/* Are the buffers any good? */
	uint8_t* message = (uint8_t*)active->systemCall.param2;
	uint32_t messageLen = active->systemCall.param3;
	uint8_t* reply = (uint8_t*)active->systemCall.param4;
	uint32_t replyLen = active->systemCall.param5;
	if (message == 0 || reply == 0 || replyLen == 0) {
		active->state = TASKS_STATE_RUNNING;
		return ERR_SEND_BAD_BUFFER;
	}

	// Is the receiver ready?
	//bwprintf("SEND: DEBUG: Checking Receiver\n\r");
	if (receiveTask->state == TASKS_STATE_SEND_BLK) {

		//bwprintf("SEND: DEBUG: Receiver is already ready.\n\r");

		// Update the receiver's state
		receiveTask->state = TASKS_STATE_RUNNING;
		// Update the sender's state
		active->state = TASKS_STATE_RPLY_BLK;

		receiveTask->sendQueueHead = 0; //reset pointer
		//Copy message to receiver's buffer
		uint32_t destSize = receiveTask->systemCall.param3;
		uint8_t* destination = (uint8_t*)receiveTask->systemCall.param2;
		uint32_t* receiverTidParam = (uint32_t*)receiveTask->systemCall.param1;

		// Set the tid of the receive
		*(receiverTidParam) = active->taskId;

		uint32_t size = messageLen;
		if (size > destSize) {
			size = destSize;
		}
		memcpy(destination, message, size);
		if (size < messageLen) {
			receiveTask->systemCall.returnValue = ERR_RECEIVE_BUFFER_TOO_SMALL;
		} else {
			receiveTask->systemCall.returnValue = size;
		}
		ret = messageLen;

	} else {

		//bwprintf("SEND: DEBUG: Receiver not ready; blocking.\n\r");
		//Receiver is not ready, increase its priority if required
		//Note that this won't have an effect until the next time the task
		// gets put in the ready queue.
		//TODO: implement this part
		//if (receiveTask->currentPriority < activeTask->currentPriority)
		//	receiveTask->currentPriority = activeTask->currentPriority;

		// Set state to receive blocked
		active->state = TASKS_STATE_RCVD_BLK;

		// add active task to send queue
		push(active,receiveTask);
	}

	//bwprintf("SEND: DEBUG: Done Sending...\n\r");

	return ret;
}

