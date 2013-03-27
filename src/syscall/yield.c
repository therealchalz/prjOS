/*
 * yield.c
 */

#include <syscall.h>
#include <task.h>

int yield() {
	int ret;
	asm (svcArg(SYSCALL_YIELD));
	asm (" MOV %[ret], R0\n": [ret] "=r" (ret): :);
	return ret;
}

int sys_yield(TaskDescriptor* active){
	return 0;
}
