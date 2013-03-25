#include <hardware_dependent/uart.h>
#include <bwio.h>
#include <hardware_dependent/cpu.h>
#include <hardware_dependent/cpu_defs.h>
#include <hardware_dependent/board.h>
#include <debug.h>


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
	while (1) {
		bwprintf("Simple Task Running...\r\n");
		int ret = testParameterFunction(10,20,30,40,50,60);
		bwprintf("Got return value: %d\r\n", ret);
	}
}

TaskDescriptor* createFirstTask(TaskDescriptorList *tds) {
	TaskCreateParameters params;
	setupDefaultCreateParameters(&params, 1, 0, &simpleTask);
	return createTask(tds, &params);
}

TaskDescriptor* schedule(TaskDescriptor* tds, TaskDescriptor* current) {
	TaskDescriptor* next = current->nextTask;
	//bwprintf("Scheduler: Next task is:\r\n");
	//printTd(next, 17);
	return next;
}
void TaskSwitch(TaskDescriptor* t) {
	asm (" MOV R0, %[td]": :[td] "r" (t));
	asm (svcArg(0));
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
	TaskDescriptorList taskList;
	taskList.tds = taskDescriptors;
	taskList.count = KERNEL_MAX_NUMBER_OF_TASKS;

	initializeTds(taskList);


	//Create first task
	TaskDescriptor* currentTask = createFirstTask(&taskList);
	currentTask->nextTask = currentTask;

	int testLoop = 4;

	while(testLoop >= 0)
	{
		testLoop--;

		//contextSwitch(currentTask);
		//cpuBarf();
		//asm (svcArg(0));
		bwprintf("Kernel switching to task...\r\n");
		TaskSwitch(currentTask);
		bwprintf("Kernel running...\r\n");

		printSystemCall(&currentTask->systemCall);

		currentTask->systemCall.returnValue = testLoop+1;

		currentTask = schedule(taskDescriptors, currentTask);

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

