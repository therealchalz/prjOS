/*
 * sys_create.c
 */

#include <task.h>
#include <bwio.h>

int sys_create(unsigned int priority, void* entryPoint, int parentTid, TaskDescriptor* tasks, int count) {
	if(priority <= 1 || priority > TASKS_MAX_PRIORITY){
	 	bwprintf("ERROR: Invalid priority\n\r");
		return -1;
	}

	TaskCreateParameters params;
	setupDefaultCreateParameters(&params, 1, 0, entryPoint);
	return createTask(tasks, count, &params);

}
