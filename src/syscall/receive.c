/*
 * receive.c
 */

#include <syscall.h>
#include <task.h>
#include <kernel_data.h>
#include <string.h>
#include <bwio.h>

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
		int* loc = senderTask->stackPointer;
		int sourceSize = *(loc);
		char* source = (char*)*(++loc);
		senderTask->stackPointer = (++loc);

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

		// Push buffer location and size on to the active task's stack
		int* loc = (int *)active->stackPointer;
		*(--loc) = (int)tid; // Put TID location onto the stack
		*(--loc) = (int)dest;
		*(--loc) = destLen;
		active->stackPointer = loc;

		// Set state to send blocked
		active->state = TASKS_STATE_SEND_BLK;
	}

	bwprintf("RECEIVE: DEBUG: Done Receiving...\n\r");

	return ret;
}

