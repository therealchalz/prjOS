#include <hardware_dependent/uart.h>
#include <bwio.h>
#include <hardware_dependent/cpu.h>
#include <hardware_dependent/cpu_defs.h>
#include <hardware_dependent/board.h>
#include <debug.h>


#ifdef DEBUG
void __error__(char *filename, unsigned long line) {
	cpuBarf();
	printCurrentStackTop(12);
	bwprintf("Driver Crash: file: %s, line: %u", filename, line);
}
#endif

void simpleTask() {
	bwprintf("Simple Task Running...");
}

TaskDescriptor* createFirstTask(TaskDescriptorList *tds) {
	TaskCreateParameters params;
	setupDefaultCreateParameters(&params, 1, 0, &simpleTask);
	return createTask(tds, &params);
}

TaskDescriptor* schedule(TaskDescriptor* tds, TaskDescriptor* current) {
	TaskDescriptor* next = current->nextTask;
	//bwprintf("Scheduler: Next task is:\n");
	//printTd(next, 17);
	return next;
}

int main(void) {

	cpuInit();

	boardInit();

	UARTInit(115200);

	bwprintf("\n\n\n********Kernel Starting********\n\r\n");
	cpuPrintInfo();

	//Put the TDs on the kernel stack
	TaskDescriptor taskDescriptors[KERNEL_MAX_NUMBER_OF_TASKS];
	TaskDescriptorList taskList;
	taskList.tds = taskDescriptors;
	taskList.count = KERNEL_MAX_NUMBER_OF_TASKS;

	initializeTds(taskList);


	TaskDescriptor* nextTask;
	//Create first task
	TaskDescriptor* currentTask = createFirstTask(&taskList);

	int testLoop = 4;

	while(testLoop > 0)
	{
		testLoop--;

		//TODO
		nextTask = schedule(taskDescriptors, currentTask);
		//TaskSwitch(nextTask);

		bwprintf("\r\nKernel Running\r\n");

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
	}
	bwprintf("System Halted\n\r");
	while(1) {}

	return 0x420;
}

