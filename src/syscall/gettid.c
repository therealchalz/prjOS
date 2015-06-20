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
 * gettid.c
 */

#include <prjOS/include/sys_syscall.h>

task_id_t prjGetTid() {
	asm (svcArg(SYSCALL_GET_TID));
	task_id_t ret;
	asm (" MOV %[ret], R0\n": [ret] "=r" (ret): :);
	return ret;
}

task_id_t sys_getTid(TaskDescriptor* active){
	setTaskReady(active);
	return active->taskId;
}
