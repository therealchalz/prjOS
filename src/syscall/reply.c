/*
 * reply.c
 */

#include <syscall.h>
#include <task.h>
#include <kernel_data.h>
#include <string.h>
#include <bwio.h>

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

	bwprintf("REPLY: DEBUG: Tid: %d, ReplyLen: %d\n\r", active->taskId,replyLen);

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
		int* loc = (int *)sendingTask->stackPointer;
		int destSize = *(loc);
		char* destination = (char*)*(++loc);
		sendingTask->stackPointer = ++loc;

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
	}
	else{
	  bwprintf("REPLY: WARN: Called reply, but no one is listening.\n\r");
	  ret = ERR_REPLY_NOT_REPLY_BLOCKED;
	}

	bwprintf("REPLY: DEBUG: Done Replying...\n\r");

	return ret;
}

