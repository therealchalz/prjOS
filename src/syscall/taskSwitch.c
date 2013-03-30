/*
 * taskSwitch.c
 */

#include <syscall.h>
#include <task.h>

int prjTaskSwitch(TaskDescriptor* t) {
	int ret;
	asm (svcArg(SYSCALL_TASKSWITCH));
	asm (" MOV %[ret], R0\n": [ret] "=r" (ret): :);
	return ret;
}

