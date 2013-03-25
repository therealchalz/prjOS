/*
 * task.h
 */

#ifndef TASK_H_
#define TASK_H_

#include <hardware_dependent/cpu_defs.h>

typedef struct SystemCall {
	int syscall;	//0 means hardware interrupted, anything else = syscall
	int returnValue;
	int handled;	//has this syscall been handled yet?
	int param1;
	int param2;
	int param3;
	int param4;
	int param5;
	int param6;
	//I'm drawing the line at 6...
} SystemCall;

typedef struct TaskDescriptor {
	int taskId;
	int parentId;
	int* stackPointer;
	SystemCall systemCall;
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
void printSystemCall(SystemCall* sc);
TaskDescriptor* createTask(TaskDescriptorList *tds, const TaskCreateParameters *parms);
void setupDefaultCreateParameters(TaskCreateParameters *params, int taskId, int parentId, void* taskEntry);

#endif /* TASK_H_ */
