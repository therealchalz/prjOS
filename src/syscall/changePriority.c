/*
 * changePriority.c
 */

#include <syscall.h>
#include <task.h>

int changePriority(int priority) {
	int ret;
	asm (svcArg(SYSCALL_CHANGEPRIORITY));
	asm (" MOV %[ret], R0\n": [ret] "=r" (ret): :);
	return ret;
}

int sys_changePriority(TaskDescriptor* active){
	if (active->systemCall.param1>=0 && active->systemCall.param1<TASKS_MAX_PRIORITY)
		active->priority = active->systemCall.param1;
	return active->priority;
}

