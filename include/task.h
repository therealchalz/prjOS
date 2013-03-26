/*
 * task.h
 */

#ifndef TASK_H_
#define TASK_H_

#include <hardware_dependent/cpu_defs.h>

#define TASKS_STATE_INVALID		0
#define TASKS_STATE_RUNNING		1
#define TASKS_STATE_EXITED		2
#define TASKS_STATE_RCVD_BLK	3
#define TASKS_STATE_SEND_BLK	4
#define TASKS_STATE_RPLY_BLK	5
#define TASKS_STATE_EVT_BLK		6

#define TASKS_MAX_PRIORITY	    7

#define TASKS_ID_GENERATION_MASK	0xFFFF0000
#define TASKS_ID_INDEX_MASK			0x0000FFFF
#define TASKS_ID_INVALID			0x0000FFFF

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

typedef struct TaskCreateParameters {
	unsigned int taskId;		//OS-assigned task id
	unsigned int parentId;		//OS-assigned task parent
	int* stackPointer;
	int priority;
	//Hardware-dependent parameters
	TaskCpuCreateParameters cpuSpecific;
} TaskCreateParameters;

void initializeTds(TaskDescriptor* tds, int count);
void printTd(TaskDescriptor* td, int stackAmount);
void printSystemCall(SystemCall* sc);
TaskDescriptor* createTask(TaskDescriptor *tds, int count, const TaskCreateParameters *parms);
void setupDefaultCreateParameters(TaskCreateParameters *params, int taskId, int parentId, void* taskEntry);

#endif /* TASK_H_ */
