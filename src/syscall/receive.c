/*
 * receive.c
 */

#include <syscall.h>
#include <task.h>
#include <kernel_data.h>

int prjReceive(int *tid, char *msg, int msgLen) {
	int ret;
	asm (svcArg(SYSCALL_RECEIVE));
	asm (" MOV %[ret], R0\n": [ret] "=r" (ret): :);
	return ret;
}

int sys_receive(TaskDescriptor* active, KernelData * kData){
	return active->taskId;
}

