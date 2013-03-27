/*
 * create.c
 */

#include <syscall.h>
#include <task.h>

int create(int priority, void* entryPoint) {
	int ret;
	asm (svcArg(SYSCALL_CREATE));
	asm (" MOV %[ret], R0\n": [ret] "=r" (ret): :);
	return ret;
}

int sys_create(TaskDescriptor* active, TaskCreateParameters* params) {
	setupDefaultCreateParameters(params, (void*)active->systemCall.param2);
	params->parentId = active->taskId;
	//lets not let someone create a task with a higher priority then they are
	int pri = active->systemCall.param1;
	/* Temporarily removed
	if (pri > active->priority)
		pri = active->priority;
	*/
	params->priority = pri;
	return 0;
}


