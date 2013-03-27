/*
 * exit.c
 */


#include <syscall.h>
#include <task.h>

int threadExit() {
	int ret;
	asm (svcArg(SYSCALL_THREADEXIT));
	asm (" MOV %[ret], R0\n": [ret] "=r" (ret): :);
	return ret;
}

int sys_threadexit(TaskDescriptor* active){
	taskExit(active);
	return 0;
}

