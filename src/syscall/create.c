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
 * create.c
 */

#include <prjOS/include/sys_syscall.h>

task_id_t prjCreate(uint32_t priority, void* entryPoint) {
	asm (svcArg(SYSCALL_CREATE));
	task_id_t ret;
	asm (" MOV %[ret], R0\n": [ret] "=r" (ret): :);
	return ret;
}

task_id_t prjCreateMicroTask(void* entryPoint) {
	asm (svcArg(SYSCALL_CREATE_MICROTASK));
	task_id_t ret;
	asm (" MOV %[ret], R0\n": [ret] "=r" (ret): :);
	return ret;
}

uint32_t sys_create(TaskDescriptor* active, TaskCreateParameters* params) {
	setupDefaultCreateParameters(params, (void*)active->systemCall.param2);
	params->parentId = active->taskId;
	params->taskType = TASK_TYPE_REGULAR;
	//lets not let someone create a task with a higher priority then they are
	uint32_t priority = active->systemCall.param1;
	if (priority < 0 && priority >= TASKS_MAX_PRIORITY) {
		return ERR_CREATE_INVAL_PRIORITY;
	}
	/* Temporarily removed
	if (priority > active->priority)
		priority = active->priority;
	*/
	params->priority = priority;
	return 0;
}

uint32_t sys_createMicroTask(TaskDescriptor* active, TaskCreateParameters* params) {
	setupDefaultCreateParameters(params, (void*)active->systemCall.param1);
	params->parentId = active->taskId;
	params->taskType = TASK_TYPE_MICRO;
	//Microtasks inherit priority
	uint32_t priority = active->priority;
	if (priority < 0 && priority >= TASKS_MAX_PRIORITY) {
		return ERR_CREATE_INVAL_PRIORITY;
	}
	params->priority = priority;
	return 0;
}

