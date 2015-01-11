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
 * sys_syscall.h
 */

#ifndef SYS_SYSCALL_H_
#define SYS_SYSCALL_H_

#include "prjOS/include/task.h"
#include "prjOS/include/kernel_data.h"

/* Event ID Codes */
#define EVENTS_STATE_INVALID		0	/* Keep as 0 so memset(0) will make everything invalid */
#define EVENTS_STATE_VALID			1

int sys_getParentTid(TaskDescriptor* active);
int sys_getTid(TaskDescriptor* active);
int sys_yield(TaskDescriptor* active);
int sys_threadexit(TaskDescriptor* active);
int sys_create(TaskDescriptor* active, TaskCreateParameters* params);
int sys_changePriority(TaskDescriptor* active);
int sys_exit(TaskDescriptor* active);
int sys_send(TaskDescriptor* active, KernelData* kData);
int sys_receive(TaskDescriptor* active, KernelData* kData, bool block);
int sys_reply(TaskDescriptor* active, KernelData* kData);
int sys_whoIsNs(TaskDescriptor* active, KernelData* kData);
int sys_registerNs(TaskDescriptor* active, KernelData* kData);
uint32_t sys_awaitEvent(TaskDescriptor* active, KernelData* kData);
uint32_t sys_eventHappened(KernelData* kData, uint32_t eventid, uint32_t retValue);

int prjTaskSwitch(TaskDescriptor* td);	//to be called only by kernel threads

#endif /* SYS_SYSCALL_H_ */
