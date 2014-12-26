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

#include "prjOS/include/syscall.h"
#include "prjOS/include/task.h"

int prjCreate(int priority, void* entryPoint) {
	int ret;
	asm (svcArg(SYSCALL_CREATE));
	asm (" MOV %[ret], R0\n": [ret] "=r" (ret): :);
	return ret;
}

int sys_create(TaskDescriptor* active, TaskCreateParameters* params) {
	setupDefaultCreateParameters(params, (void*)active->systemCall.param2);
	params->parentId = active->taskId;
	//lets not let someone create a task with a higher priority then they are
	int priority = active->systemCall.param1;
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


