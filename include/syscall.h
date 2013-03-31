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

int prjGetParentTid(void);
int prjGetTid(void);
int prjYield(void);
int prjExit(void);
int prjCreate(int priority, void* entryPoint);
int prjChangePriority(int newPriority);
int prjSend(int tid, char *msg, int msgLen, char *reply, int replyLen);
int prjReceive(int *tid, char *msg, int msgLen);
int prjReply(int tid, char *msg, int msgLen);

#endif /* SYSCALL_H_ */
