/*
 * exit.c
 */


#include <syscall.h>
#include <task.h>

int exit() {
	int ret;
	asm (svcArg(SYSCALL_EXIT));
	asm (" MOV %[ret], R0\n": [ret] "=r" (ret): :);
	return ret;
}

int sys_exit(TaskDescriptor* active){
	threadExit(active);
	return 0;
}

