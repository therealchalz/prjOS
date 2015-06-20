/***************************************
 * Copyright (c) 2013 Charles Hache <chache@brood.ca>. All rights reserved. 
 * 
 * This file is part of the prjOS project.
 * prjOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * prjOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with prjOS.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * Contributors:
 *     Charles Hache <chache@brood.ca> - initial API and implementation
***************************************/
/*
 * syscall.h
 */

#ifndef SYSCALL_H_
#define SYSCALL_H_

#include <stdint.h>
#include <stdbool.h>
#include <prjOS/include/types.h>


//SVC call parameters (immediate values)
#define SYSCALL_HARDWARE_CALL		0	//Do not change this one
#define SYSCALL_GET_TID				1
#define SYSCALL_GET_PARENT_TID		2
#define SYSCALL_YIELD				3
#define SYSCALL_THREADEXIT			4
#define SYSCALL_CREATE				5
#define SYSCALL_CHANGEPRIORITY		6
#define SYSCALL_TASKSWITCH			7
#define SYSCALL_SEND				8
#define SYSCALL_RECEIVE				9
#define SYSCALL_REPLY				10
#define SYSCALL_WHOISNS				11
#define SYSCALL_REGISTERNS			12
#define SYSCALL_RECEIVE_NONBLOCK	13
#define SYSCALL_AWAIT_EVENT			14
#define SYSCALL_CREATE_MICROTASK	15
#define SYSCALL_META_INFO			16

//TODO: Consolidate error codes.  Try to match POSIX ones?

/* CREATE error codes */
#define ERR_CREATE_INVAL_PRIORITY	(-1)	/* Priority was invalid */
#define ERR_CREATE_NO_SPACE			(-2)	/* No empty task descriptors */
#define ERR_CREATE_INVAL_NAME		(-3)	/* Invalid task entrypoint */

/* SEND error codes */
#define ERR_SEND_TASKID_DNE			(-1)	/* The task ID doesn't represent a possible task */
#define ERR_SEND_TASKID_NOTFOUND	(-2)	/* The task could not be found */
#define ERR_SEND_INCOMPLETE			(-3)	/* The transaction couldn't be completed */
#define ERR_SEND_BAD_BUFFER			(-4)	/* Either the send or the reply buffer is invalid (or the reply buffer has 0 size) */
#define ERR_SEND_EXITED				(-5)	/* The other task has exited before this request could be completed */

/* RECEIVE error codes */
#define ERR_RECEIVE_BUFFER_TOO_SMALL	(-1)	/* The message buffer wasn't large enough to hold the whole message */
#define ERR_RECEIVE_BAD_BUFFER		(-2)	/* The message buffer is invalid */
#define ERR_RECEIVE_NOSEND			(-3)	/* There was no sender on a non-blocking receive call */

/* REPLY error codes */
#define ERR_REPLY_TASKID_DNE		(-1)	/* The task ID doesn't represent a possible task */
#define ERR_REPLY_TASKID_NOTFOUND	(-2)	/* The task could not be found */
#define ERR_REPLY_NOT_REPLY_BLOCKED	(-3)	/* The task is not reply-blocked */
#define ERR_REPLY_BUFFER_TOO_SMALL	(-4)	/* The reply buffer wasn't large enough to hold the whole reply */
#define ERR_REPLY_ERROR				(-5)	/* There was another error */

/* REGISTERAS error codes */
#define ERR_REGISTERAS_INVALID_TID	(-1)	/* The TID used to connect to the nameserver doesn't represent a possible task */
#define ERR_REGISTERAS_WRONG_TID	(-2)	/* The TID is pointing to a task that is not the nameserver */
#define ERR_REGISTERAS_NO_SPACE		(-3)	/* The nameserver reported it is out of space for new registrations */
#define ERR_REGISTERAS_TOO_LONG		(-4)	/* The nameserver reported that the name registration requested has been truncated */
#define ERR_REGISTERAS_ERROR		(-5)	/* Other errors */

/* WHOIS error codes */
#define ERR_WHOIS_INVALID_TID		(-1)	/* The TID used to connect to the nameserver doesn't represent a possible task */
#define ERR_WHOIS_WRONG_TID			(-2)	/* The TID is pointing to a task that is not the nameserver */
#define ERR_WHOIS_NOT_FOUND			(-3)	/* Task not found */
#define ERR_WHOIS_ERROR				(-4)	/* Other errors */

/* AWAITEVENT error codes */
#define ERR_AWAITEVENT_VOLATILE_IN_BUFFER	0	/* Volatile data is in the buffer */
#define ERR_AWAITEVENT_INVALID_EVENT		(-1)	/* Invalid event */
#define ERR_AWAITEVENT_CORRUPTED			(-2)	/* Corrupted volatile data.  Error indication in the event buffer. */
#define ERR_AWAITEVENT_GET_VOLATILE			(-3)	/* Volatile data must be collected and interrupts re-enabled in the caller */
#define ERR_AWAITEVENT_BUFFER_TOO_SMALL		(-4)	/* The event buffer was full */
#define ERR_AWAITEVENT_BAD_EVENT_ID			(-5)	/* The event id given was not valid */

/* DELAY error codes */
#define ERR_DELAY_SUCCESS			0		/* The delay was successful */
#define ERR_DELAY_INVALID_TID		(-1)	/* The TID used to contact the clock server was invalid */
#define ERR_DELAY_WRONG_TID			(-2)	/* The TID used to contact the clock server is not the tid of the clock server */

/* TIME error codes */
#define ERR_TIME_INVALID_TID		(-1)	/* The TID used to contact the clock server was invalid */
#define ERR_TIME_WRONG_TID			(-2)	/* The TID used to contact the clock server is not the tid of the clock server */

/* DELAYUNTIL error codes */
#define ERR_DELAYUNTIL_INVALID_TID	(-1)	/* The TID used to contact the clock server was invalid */
#define ERR_DELAYUNTIL_WRONG_TID	(-2)	/* The TID used to contact the clock server is not the tid of the clock server */

/* Event ID Codes */
//TODO: these are hardware dependent to some extent - figure out what to do
#define EVENTID_TC1					0		/* Timer 1 interrupt */
#define EVENTID_TC2					1		/* Timer 2 interrupt */
#define EVENTID_TC3					2		/* Timer 3 interrupt */
#define EVENTID_UART0				3		/* UART0 interrupt */
#define EVENTID_UART1				4		/* UART1 interrupt */
#define EVENTID_UART2				5		/* UART2 interrupt */
#define EVENTID_USB0				6		/* USB 0 interrupt */
#define EVENTID_INVALID				-1

/* META_INFO request structure */
typedef struct MetaInfoRequest {
	uint32_t requestType;		//Required
	task_id_t taskId;			//Optional parameter for some calls
	void* extendedResult;		//Pointer to an extended result structure if required
} MetaInfoRequest;

#define META_REQUEST_TASKS_INFO				1
typedef struct MetaTasksInfoResult {
	uint16_t numTasks;
	uint16_t numMicroTasks;
	uint32_t taskDefaultStackSize;
	uint32_t microTaskDefaultStackSize;
	uint16_t maxNumTasks;
	uint16_t maxMicroNumTasks;
} MetaTasksInfoResult;

#define META_REQUEST_TOTAL_RAM				2
#define META_REQUEST_USED_RAM				3	//Stacks of all tasks + static & global variables + kernel stack

#define META_REQUEST_TASK_INFO				4
typedef struct MetaTaskInfoResult {
	task_id_t taskID;
	uint32_t* stackBase;
	uint32_t* stackHead;
	uint8_t taskState;
	task_id_t parentTid;
	uint8_t taskType;
} MetaTaskInfoResult;

#define META_REQUEST_STATIC_USED_RAM		5	//Static & global variables
#define META_REQUEST_NUMBER_OF_TASKS		6
#define META_REQUEST_NUMBER_OF_MICROTASKS	7
#define META_REQUEST_STACK_SIZE				8	//Requires taskID parameter
#define META_REQUEST_USED_STACK				9	//Requires taskID parameter
#define META_REQUEST_SYSTEM_LOAD			10
#define META_REQUEST_TOTAL_FLASH			11
#define META_REQUEST_USED_FLASH				12

task_id_t prjGetParentTid(void);
task_id_t prjGetTid(void);
uint32_t prjYield(void);
uint32_t prjExit(void);
task_id_t prjCreate(uint32_t priority, void* entryPoint);
uint32_t prjChangePriority(uint32_t newPriority);
uint32_t prjSend(task_id_t tid, uint8_t *msg, uint32_t msgLen, uint8_t *reply, uint32_t replyLen);
uint32_t prjReceive(task_id_t *tid, uint8_t *msg, uint32_t msgLen);
uint32_t prjReceiveNonBlocking(task_id_t *tid, uint8_t *msg, uint32_t msgLen);
uint32_t prjReply(task_id_t tid, uint8_t *msg, uint32_t msgLen);
task_id_t prjWhoIs(char *name);
uint32_t prjRegisterNameserver(task_id_t tid);
uint32_t prjRegisterAs(char* name);
uint32_t prjGetCh(uint32_t* charOut, task_id_t serialTid);
uint32_t prjGetChNonBlocking(uint32_t* charOut, task_id_t serialTid);
uint32_t prjPutBuf(const uint8_t* str, uint16_t len, task_id_t serialTid);
uint32_t prjPutStr(const char* str, task_id_t serialTid);
uint32_t prjAwaitEvent( uint32_t eventid);
task_id_t prjCreateMicroTask(void* entryPoint);
uint32_t prjMetaInfo(MetaInfoRequest* request);

#endif /* SYSCALL_H_ */
