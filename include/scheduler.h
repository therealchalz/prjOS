/*
 * scheduler.h
 */

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <task.h>

typedef struct SchedulerStructure {
	TaskDescriptor* tds;
	int count;

	struct TaskDescriptor* lastRun;
} SchedulerStructure;

TaskDescriptor* schedule(SchedulerStructure* schedStruct);

#endif /* SCHEDULER_H_ */
