/*
 * reply.c
 */

#include <syscall.h>
#include <task.h>
#include <kernel_data.h>

int prjReply(int tid, char *msg, int msgLen) {
	int ret;
	asm (svcArg(SYSCALL_REPLY));
	asm (" MOV %[ret], R0\n": [ret] "=r" (ret): :);
	return ret;
}

int sys_reply(TaskDescriptor* active, KernelData * kData){
	return active->taskId;
}

