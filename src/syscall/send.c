/*
 * send.c
 */

#include <syscall.h>
#include <task.h>
#include <kernel_data.h>

int prjSend(int tid, char *msg, int msgLen, char *reply, int replyLen) {
	int ret;
	asm (svcArg(SYSCALL_SEND));
	asm (" MOV %[ret], R0\n": [ret] "=r" (ret): :);
	return ret;
}

int sys_send(TaskDescriptor* active, KernelData * kData) {
	return active->taskId;
}

