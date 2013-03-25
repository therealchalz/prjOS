/*
 * task.c
 */

#include <task.h>
#include <hardware_dependent/cpu_defs.h>
#include <debug.h>
#include <bwio.h>
#include <hardware_dependent/cpu.h>
#include <string.h>
#include <syscall.h>

void setupDefaultCreateParameters(TaskCreateParameters *params, int taskId, int parentId, void* taskEntry) {
	memset(params, 0, sizeof(TaskCreateParameters));

	params->taskId = taskId;
	params->parentId = parentId;
	params->stackPointer = 0;	//use default

	cpuSetupTaskDefaultParameters(&(params->cpuSpecific), taskEntry);
}

void initializeTds(TaskDescriptorList tds) {
	memset(tds.tds, 0, tds.count*sizeof(TaskDescriptor));

	int stackOffset = KERNEL_TASK_DEFAULT_STACK_SIZE;
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

void printSystemCall(SystemCall* sc) {
	bwprintf("SystemCall: %d\n\r", sc->syscall);
	if (sc->syscall == SYSCALL_HARDWARE_CALL) {
		bwprintf("  Hardware interrupt; no call data\n\r");
	} else {
		bwprintf("  Parameter 1: %d\n\r", sc->param1);
		bwprintf("  Parameter 2: %d\n\r", sc->param2);
		bwprintf("  Parameter 3: %d\n\r", sc->param3);
		bwprintf("  Parameter 4: %d\n\r", sc->param4);
		bwprintf("  Parameter 5: %d\n\r", sc->param5);
		bwprintf("  Parameter 6: %d\n\r", sc->param6);
	}
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