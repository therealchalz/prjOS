/*
 * task.c
 */

#include <task.h>
#include <hardware_dependent/cpu_defs.h>
#include <debug.h>
#include <bwio.h>
#include <hardware_dependent/cpu.h>

void setupDefaultCreateParameters(TaskCreateParameters *params, int taskId, int parentId, void* taskEntry) {
	memset(0, &params, sizeof(params));

	params->taskId = taskId;
	params->parentId = parentId;
	params->stackPointer = 0;	//use default

	cpuSetupTaskDefaultParameters(&(params->cpuSpecific), taskEntry);
}

void initializeTds(TaskDescriptorList tds) {
	memset(tds.tds, 0, tds.count*sizeof(TaskDescriptor));

	int stackOffset = 0;
	int i;
	for (i=0; i<tds.count; i++) {
		tds.tds[i].stackPointer = (int*)(STACK_BASE - stackOffset);
		stackOffset += KERNEL_TASK_DEFAULT_STACK_SIZE;
	}
}
void printTd(TaskDescriptor* td, int stackAmount) {
	bwprintf("Task %d\r\n", td->taskId);
	bwprintf("  Parent: %d\r\n", td->parentId);
	//bwprintf("  Program Counter: %x\r\n", td->programCounter);
	bwprintf("  Stack Location: %x\r\n", td->stackPointer);
	bwprintf("  TOS:\r\n");
	printStackTop((char*)td->stackPointer, stackAmount);
}

TaskDescriptor* createTask(TaskDescriptorList *tds, const TaskCreateParameters *parms) {
	// Search for available task descriptor
	int i;
	TaskDescriptor* ret = 0;
	for (i=0; i<tds->count; i++) {
		if (tds->tds[i].taskId == 0) {
			ret = &tds->tds[i];
			break;
		}
	}

	if (ret != 0) {
		ret->taskId = parms->taskId;
		ret->parentId = parms->parentId;
		if (parms->stackPointer != 0)
			ret->stackPointer = parms->stackPointer;

		ret->stackPointer = cpuCreateTask(ret->stackPointer, parms);

		printTd(ret,17);
	} else {
		bwprintf("PANIC: Ran out task descriptor space");
		while(1) {
		}
	}

	return ret;
}
