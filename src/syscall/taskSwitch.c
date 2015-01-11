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
 * taskSwitch.c
 */

#include "prjOS/include/syscall.h"
#include "prjOS/include/task.h"
//TODO: make this file hardware-independent
#include "inc/hw_ints.h"

//extern kernelToTask(TaskDescriptor* t);

__attribute__ ((naked ))
uint32_t sys_taskswitch(TaskDescriptor* t) {
	asm (svcArg(SYSCALL_TASKSWITCH));
	asm ("		BX	lr\n");
}

uint32_t prjTaskSwitch(TaskDescriptor* t) {
	//Re-enable SVC call exception, but block all others
	IntPriorityMaskSet(1 << (8-NUM_PRIORITY_BITS));
	IntMasterEnable();

	//Switch
	return sys_taskswitch(t);
}

/*
int prjTaskSwitch(TaskDescriptor* t) {
	kernelToTask(t);
}
*/
