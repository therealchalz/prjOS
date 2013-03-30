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
//TODO:
#define SYSCALL_SEND				8
#define SYSCALL_RECEIVE				9
#define SYSCALL_REPLY				10

int prjGetParentTid(void);
int prjGetTid(void);
int prjYield(void);
int prjExit(void);
int prjCreate(int priority, void* entryPoint);
int prjChangePriority(int newPriority);

#endif /* SYSCALL_H_ */
