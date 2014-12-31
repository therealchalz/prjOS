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

int prjGetParentTid(void);
int prjGetTid(void);
int prjYield(void);
int prjExit(void);
int prjCreate(int priority, void* entryPoint);
int prjChangePriority(int newPriority);
int prjSend(int tid, char *msg, int msgLen, char *reply, int replyLen);
int prjReceive(int *tid, char *msg, int msgLen);
int prjReceiveNonBlocking(int *tid, char *msg, int msgLen);
int prjReply(int tid, char *msg, int msgLen);
int prjWhoIs(char *name);
int prjRegisterNameserver(int tid);
int prjRegisterAs(char* name);
uint32_t prjGetCh(uint32_t* charOut, uint32_t serialTid);
uint32_t prjGetChNonBlocking(uint32_t* charOut, uint32_t serialTid);
uint32_t prjPutStr(const char* str, uint16_t len, uint32_t serialTid);

#endif /* SYSCALL_H_ */
