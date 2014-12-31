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
 * reply.c
 */

#include "prjOS/include/syscall.h"
#include "prjOS/include/task.h"
#include "prjOS/include/kernel_data.h"
#include "string.h"
#include "prjOS/include/bwio.h"

int prjReply(int tid, char *msg, int msgLen) {
	int ret;
	asm (svcArg(SYSCALL_REPLY));
	asm (" MOV %[ret], R0\n": [ret] "=r" (ret): :);
	return ret;
}

int sys_reply(TaskDescriptor* active, KernelData *kData){
	//bwprintf(COM2, "REPLY: DEBUG: Replying...\n\r");

	int ret = 0;
	/* Do error checking on arguments */
	int tid = active->systemCall.param1;
	char* reply = (char*)active->systemCall.param2;
	int replyLen = active->systemCall.param3;

	//bwprintf("REPLY: DEBUG: Tid: %d, ReplyLen: %d\n\r", active->taskId,replyLen);

	/* Is the tid possible? */
	int generationId = (tid & TASKS_ID_GENERATION_MASK) >> TASKS_ID_GENERATION_SHIFTBITS;
	int taskIndex = tid & TASKS_ID_INDEX_MASK;
	if (generationId == 0 || taskIndex < 0 || taskIndex >= kData->tdCount) {
		bwprintf("REPLY: ERR: Task ID does not exist: %d from :%d\n\r",tid, active->taskId);
		active->state = TASKS_STATE_RUNNING;
		return ERR_REPLY_TASKID_DNE;
	}

	/* Does the task exist? */
	TaskDescriptor* sendingTask = findTd(tid, kData->taskDescriptorList, kData->tdCount);
	if (sendingTask == 0) {
		active->state = TASKS_STATE_RUNNING;
		return ERR_REPLY_TASKID_NOTFOUND;
	}
	/* Are the buffers any good? */
	if (reply == 0 && replyLen != 0){
		bwprintf("REPLY: ERR: Reply message invalid.\n\r");
		active->state = TASKS_STATE_RUNNING;
		return ERR_REPLY_ERROR;
	}

	// Is the sending task ready?
	if (sendingTask->state == TASKS_STATE_RPLY_BLK) {
		// Update the sender's state
		sendingTask->state = TASKS_STATE_RUNNING;
		// Copy message to sender's buffer
		int destSize = sendingTask->systemCall.param5;
		char* destination = (char*)sendingTask->systemCall.param4;

		int size = replyLen;
		if (size > destSize) {
			size = destSize;
		}

		memcpy(destination, reply, size);

		// Return code handling
		if(size == replyLen) {// Everything worked
			sendingTask->systemCall.returnValue = size;
			ret = 0;
		} else {
			if(ret == -1) { // Buffer Too Small
				sendingTask->systemCall.returnValue = size;
				ret = ERR_REPLY_BUFFER_TOO_SMALL;
			} else { // Other Error
				sendingTask->systemCall.returnValue = ERR_SEND_INCOMPLETE;
				ret = ERR_REPLY_ERROR;
			}
		}
	} else {
	  bwprintf("REPLY: WARN: Called reply, but no one is listening.\n\r");
	  ret = ERR_REPLY_NOT_REPLY_BLOCKED;
	}

	//bwprintf("REPLY: DEBUG: Done Replying...\n\r");
	active->state = TASKS_STATE_RUNNING;
	return ret;
}

