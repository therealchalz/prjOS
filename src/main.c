#include <hardware_dependent/uart.h>
#include <bwio.h>
#include <hardware_dependent/cpu.h>
#include <hardware_dependent/cpu_defs.h>
#include <hardware_dependent/board.h>
#include <debug.h>
#include <scheduler.h>
#include <syscall.h>
#include <sys_syscall.h>

#ifdef DEBUG
void __error__(char *filename, unsigned long line) {
	cpuBarf();
	bwprintf("Driver Crash: file: %s, line: %u", filename, line);
	//Keep this at the end because it may cause a hard fault
	printCurrentStackTop(16);
	while(1) {}
}
#endif

int testParameterFunction(int p1, int p2, int p3, int p4, int p5, int p6) {
	int ret;
	asm (svcArg(1));
	asm (" MOV %[ret], R0\n": [ret] "=r" (ret): :);
	return ret;
}

void simpleTask() {
	bwprintf("Simple Task Starting...\n\r");
	bwprintf("My Task ID is: %d and my parent is %d\r\n", getTid(), getParentTid());
	while (1) {
		bwprintf("Simple Task Running...\r\n");
		yield();
	}
}

TaskDescriptor* createFirstTask(TaskDescriptor *tds, int count) {
	TaskCreateParameters params;
	setupDefaultCreateParameters(&params, 1, 0, &simpleTask);
	return createTask(tds, count, &params);
}

void TaskSwitch(TaskDescriptor* t) {
	asm (" MOV R0, %[td]": :[td] "r" (t));
	asm (svcArg(0));
}

void handleSyscall(TaskDescriptor* t) {
	//TODO: if this TD has nothing to to, look for one that has waiting work
	if (t->systemCall.handled) {
		return;
	}
	switch (t->systemCall.syscall) {
	case SYSCALL_HARDWARE_CALL:
		break;
	case SYSCALL_GET_TID:
		t->systemCall.returnValue = sys_getTid(t);
		break;
	case SYSCALL_GET_PARENT_TID:
		t->systemCall.returnValue = sys_getParentTid(t);
		break;
	case SYSCALL_YIELD:
		t->systemCall.returnValue = sys_yield(t);
		break;
	}
	t->systemCall.handled = 1;
}

int main(void) {

	cpuInit();

	boardInit();

	UARTInit(115200);

	bwprintf("\n\n\n********Kernel Starting********\n\r\n");
	cpuPrintInfo();
	printCEnvironmentSettings();

	//Put the TDs on the kernel stack
	TaskDescriptor taskDescriptors[KERNEL_MAX_NUMBER_OF_TASKS];
	initializeTds(taskDescriptors, KERNEL_MAX_NUMBER_OF_TASKS);

	bwprintf("Tasks are taking %d bytes on the kernel stack\n\r", (KERNEL_MAX_NUMBER_OF_TASKS)*sizeof(TaskDescriptor));
	//Scheduler stuff
	SchedulerStructure schedStruct;
	schedulerInit(&schedStruct, taskDescriptors);

	//Create first task
	TaskDescriptor* currentTask = createFirstTask(taskDescriptors, KERNEL_MAX_NUMBER_OF_TASKS);
	currentTask->nextTask = currentTask;

	schedulerAdd(&schedStruct, currentTask);

	int testLoop = 6;

	while(testLoop >= 0)
	{
		testLoop--;

		currentTask = schedule(&schedStruct);

		if (currentTask != 0) {
			schedulerAdd(&schedStruct, currentTask);
			bwprintf("Kernel switching to task...\r\n");
			TaskSwitch(currentTask);
			bwprintf("Kernel running...\r\n");
			handleSyscall(currentTask);
		}

		boardSetIndicatorLED(1);
		long l = 0x000fffff;
		while (l > 0) {
			l--;
		}
		boardSetIndicatorLED(0);
		l = 0x000fffff;
		while (l > 0) {
			l--;
		}
		boardSetIndicatorLED(0);
	}
	bwprintf("System Halted\n\r");
	while(1) {}

	return 0x420;
}

