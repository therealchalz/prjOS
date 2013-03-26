/*
 * scheduler.c
 */

#include <scheduler.h>

TaskDescriptor* schedule(SchedulerStructure* schedStruct) {
	TaskDescriptor* newNext = schedStruct->lastRun->nextTask;
	schedStruct->lastRun = newNext;
	return newNext;
}
