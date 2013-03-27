/*
 * kernel_data.h
 */

#ifndef KERNEL_DATA_H_
#define KERNEL_DATA_H_

typedef struct KernelData {
	TaskDescriptor* taskDescriptorList;	// pointer to the list of all tasks
	unsigned int tdCount;				// size of task list
	unsigned int nextTaskId;			// the taskId of the next task
	SchedulerStructure* schedulerStructure;	// Used to hold the state of the scheduler
} KernelData;

#endif /* KERNEL_DATA_H_ */
