/*
 * task.h
 */

#ifndef TASK_H_
#define TASK_H_

#include <hardware_dependent/cpu_defs.h>

typedef struct TaskDescriptor {
	int taskId;
	int parentId;
	int* stackPointer;
	int returnValue;
	int state;
	int priority;
	struct TaskDescriptor* nextTask;
} TaskDescriptor;

typedef struct TaskDescriptorList {
	TaskDescriptor* tds;
	int count;
} TaskDescriptorList;

typedef struct TaskCreateParameters {
	unsigned int taskId;		//OS-assigned task id
	unsigned int parentId;		//OS-assigned task parent
	int* stackPointer;
	//Hardware-dependent parameters
	TaskCpuCreateParameters cpuSpecific;
} TaskCreateParameters;

void initializeTds(TaskDescriptorList tds);
void printTd(TaskDescriptor* td, int stackAmount);
TaskDescriptor* createTask(TaskDescriptorList *tds, const TaskCreateParameters *parms);
void setupDefaultCreateParameters(TaskCreateParameters *params, int taskId, int parentId, void* taskEntry);

#endif /* TASK_H_ */
