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
 * changePriority.c
 */

#include "prjOS/include/syscall.h"
#include "prjOS/include/task.h"

uint32_t prjChangePriority(uint32_t priority) {
	asm (svcArg(SYSCALL_CHANGEPRIORITY));
	uint32_t ret;
	asm (" MOV %[ret], R0\n": [ret] "=r" (ret): :);
	return ret;
}

uint32_t sys_changePriority(TaskDescriptor* active){
	setTaskReady(active);
	if (active->systemCall.param1>=0 && active->systemCall.param1<TASKS_MAX_PRIORITY)
		active->priority = active->systemCall.param1;
	return active->priority;
}

