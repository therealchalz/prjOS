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
 * registerNameserver.c
 */

#include <syscall.h>
#include <task.h>
#include <kernel_data.h>
#include <string.h>
#include <bwio.h>
#include <base_tasks/nameserver.h>

int prjRegisterNameserver(int tid) {
	int ret;
	asm (svcArg(SYSCALL_REGISTERNS));
	asm (" MOV %[ret], R0\n": [ret] "=r" (ret): :);
	return ret;
}

int sys_registerNs(TaskDescriptor* active, KernelData* kData){
	setTaskReady(active);
	kData->nameserverTid = active->systemCall.param1;
	return 0;
}
