#include <prjOS/include/sys_syscall.h>

extern uint32_t _meta_ram_start;
extern uint32_t _meta_ram_end;

uint32_t prjMetaInfo(MetaInfoRequest* request) {
	asm (svcArg(SYSCALL_META_INFO));
	uint32_t ret;
	asm (" MOV %[ret], R0\n": [ret] "=r" (ret): :);
	return ret;
}

uint32_t sys_metaInfo(TaskDescriptor* active, KernelData* kData) {
	setTaskReady(active);

	uint32_t ret = -1;

	MetaInfoRequest* request = (MetaInfoRequest*)active->systemCall.param1;

	switch (request->requestType) {
	case META_REQUEST_TASKS_INFO:
		{
			MetaTasksInfoResult* result = (MetaTasksInfoResult*)request->extendedResult;
			result->maxMicroNumTasks = KERNEL_NUMBER_OF_MICROTASKS;
			result->maxNumTasks = KERNEL_NUMBER_OF_TASKS;
			result->microTaskDefaultStackSize = getMicroTaskStackSize();
			result->taskDefaultStackSize = getRegularTaskStackSize();
			result->numMicroTasks = getNumMicroTasks(kData->taskDescriptorList, kData->tdCount);
			result->numTasks = getNumRegularTasks(kData->taskDescriptorList, kData->tdCount);
			ret = 0;
		}
		break;
	case META_REQUEST_TASK_INFO:
		{
			MetaTaskInfoResult* result = (MetaTaskInfoResult*)request->extendedResult;
			uint32_t taskId = request->taskId;
			result->taskID = taskId;
			result->parentTid = getParentTid(kData->taskDescriptorList, kData->tdCount, taskId);
			result->stackBase = getStackBase(kData->taskDescriptorList, kData->tdCount, taskId);
			result->stackHead = getStackHead(kData->taskDescriptorList, kData->tdCount, taskId);
			result->taskState = (uint8_t)getTaskState(kData->taskDescriptorList, kData->tdCount, taskId);
			result->taskType = getTaskType(kData->taskDescriptorList, kData->tdCount, taskId);
		}
		break;
	case META_REQUEST_TOTAL_RAM:
		{
			uint32_t startLoc = (uint32_t)&_meta_ram_start;
			uint32_t endLoc = (uint32_t)&_meta_ram_end;
			ret = endLoc - startLoc;
		}
		break;
	case META_REQUEST_USED_RAM:
		{
			uint16_t numRegTasks = getNumRegularTasks(kData->taskDescriptorList, kData->tdCount);
			uint16_t numMicroTasks = getNumMicroTasks(kData->taskDescriptorList, kData->tdCount);
			ret = numRegTasks*getRegularTaskStackSize() + numMicroTasks*getMicroTaskStackSize();
			//TODO: add static & global vars, kernel stack & data
		}
		break;
	}

	return ret;
}
