/*
 * scheduler.h
 */

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <task.h>
#include <hardware_dependent/cpu_defs.h>

typedef struct SchedulerTask {
	TaskDescriptor* td;
	struct SchedulerTask* next;
} SchedulerTask;

typedef struct SchedulerStructure {
	SchedulerTask* queueHeads[TASKS_MAX_PRIORITY];
	SchedulerTask* queueTails[TASKS_MAX_PRIORITY];
	SchedulerTask allTasks[KERNEL_MAX_NUMBER_OF_TASKS];
} SchedulerStructure;

void schedulerInit(SchedulerStructure *schedStruct, TaskDescriptor *tds);
void schedulerAdd(SchedulerStructure* schedStruct, TaskDescriptor* td);
void schedulerPrintTasks(SchedulerStructure* schedStruct);
TaskDescriptor* schedule(SchedulerStructure* schedStruct);

#endif /* SCHEDULER_H_ */
