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

#include <prjOS/include/syscall.h>
#include <prjOS/include/bwio.h>
#include <prjOS/include/kernel_data.h>

/* Event ID Codes */
#define EVENTS_STATE_INVALID		0	/* Keep as 0 so memset(0) will make everything invalid */
#define EVENTS_STATE_VALID			1

task_id_t sys_getParentTid(TaskDescriptor* active);
task_id_t sys_getTid(TaskDescriptor* active);
uint32_t sys_yield(TaskDescriptor* active);
uint32_t sys_threadexit(TaskDescriptor* active);
uint32_t sys_create(TaskDescriptor* active, TaskCreateParameters* params);
uint32_t sys_changePriority(TaskDescriptor* active);
uint32_t sys_exit(TaskDescriptor* active);
uint32_t sys_send(TaskDescriptor* active, KernelData* kData);
uint32_t sys_receive(TaskDescriptor* active, KernelData* kData, bool block);
uint32_t sys_reply(TaskDescriptor* active, KernelData* kData);
task_id_t sys_whoIsNs(TaskDescriptor* active, KernelData* kData);
uint32_t sys_registerNs(TaskDescriptor* active, KernelData* kData);
uint32_t sys_awaitEvent(TaskDescriptor* active, KernelData* kData);
uint32_t sys_eventHappened(KernelData* kData, uint32_t eventid, uint32_t retValue);
uint32_t sys_createMicroTask(TaskDescriptor* active, TaskCreateParameters* params);
uint32_t sys_metaInfo(TaskDescriptor* active, KernelData* kData);

uint32_t prjTaskSwitch(TaskDescriptor* td);	//to be called only by kernel threads

#endif /* SYS_SYSCALL_H_ */
