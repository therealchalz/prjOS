#include "prjOS/include/syscall.h"
#include "prjOS/include/sys_syscall.h"
#include "prjOS/include/task.h"

uint32_t prjAwaitEvent( uint32_t eventid) {
	asm volatile(svcArg(SYSCALL_AWAIT_EVENT));
	uint32_t ret;
	asm volatile ("mov %[ret], r0": [ret] "+r" (ret));
	return ret;
}

EventQuery* popEvent(EventData* eventData, uint32_t eventId) {

	if(eventData->eventQueueHead[eventId]!=0){
		EventQuery* query = eventData->eventQueueHead[eventId];
		eventData->eventQueueHead[eventId] = query->nextQuery;
		query->nextQuery = 0;
		return query;
	}

	return 0;
}

void pushEvent(EventData* eventData, EventQuery* query) {
	uint32_t eventId = query->eventId;
	if(eventData->eventQueueHead[eventId]==0){
		eventData->eventQueueHead[eventId]=query;
		eventData->eventQueueTail[eventId]=query;
		query->nextQuery=0;
	}
	else{
		EventQuery* lastQuery = eventData->eventQueueTail[eventId];
		lastQuery->nextQuery=query;
		eventData->eventQueueTail[eventId]=query;
		query->nextQuery=0;
	}
}

uint32_t sys_awaitEvent(TaskDescriptor* active, KernelData* kData) {
	//bwprintf("AWAITEVENT: DEBUG: AwaitEvent: %u\n\r", eventid);
	//Check if eventid is valid
	uint32_t eventid = active->systemCall.param1;
	if (eventid >= EVENTS_NUM_TYPES) {
		bwprintf("AWAITEVENT: ERR: Bad event code used: %d\n\r", eventid);
		active->systemCall.returnValue = ERR_AWAITEVENT_BAD_EVENT_ID;
		setTaskReady(active);
		return 0;
	}
	//Find vacant event
	uint16_t i=0;
	EventQuery* query = 0;
	for (; i<EVENTS_NUM_EVENTS; i++) {
		if (kData->eventData->eventList[i].waitingTid == TASKS_ID_INVALID) {
			query = &kData->eventData->eventList[i];
			break;
		}
	}

	if (query == 0) { //No vacant event was found
		bwprintf("AWAITEVENT: ERR: Event buffer is full\n\r");
		active->systemCall.returnValue = ERR_AWAITEVENT_BUFFER_TOO_SMALL;setTaskReady(active);
		return 0;
	}
		
	//Setup the event, push it, and block the task
	query->waitingTid = active->taskId;
	query->nextQuery = 0;
	query->state = EVENTS_STATE_VALID;
	query->eventId = eventid;
	pushEvent(kData->eventData, query);
	active->state = TASKS_STATE_EVT_BLK;
	
	return 0;
}

/* Called by the kernel isr in interrupt.c.  Not your average syscall (not to be called by client/user tasks) */
uint32_t sys_eventHappened(KernelData* kData, uint32_t eventid, uint32_t retValue) {
	EventQuery* query = popEvent(kData->eventData, eventid);
	if (query == 0) { //Nothing in event queue
		return 0;
	}

	TaskDescriptor* theTask = findTd(query->waitingTid, kData->taskDescriptorList, kData->tdCount);
	query->waitingTid = TASKS_ID_INVALID;
	query->nextQuery = 0;
	query->state = EVENTS_STATE_INVALID;
	if (retValue == 0)
		theTask->systemCall.returnValue = ERR_AWAITEVENT_VOLATILE_IN_BUFFER;
	else
		theTask->systemCall.returnValue = retValue;
	
	setTaskReady(theTask);
	return theTask->taskId;
}
