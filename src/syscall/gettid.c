/*
 * gettid.c
 */

#include <syscall.h>
#include <task.h>

int getTid() {
	int ret;
	asm (svcArg(SYSCALL_GET_TID));
	asm (" MOV %[ret], R0\n": [ret] "=r" (ret): :);
	return ret;
}

int sys_getTid(TaskDescriptor* active){
	return active->taskId;
}
