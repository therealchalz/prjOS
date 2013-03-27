/*
 * sys_syscall.h
 */

#ifndef SYS_SYSCALL_H_
#define SYS_SYSCALL_H_

#include <task.h>

int sys_getParentTid(TaskDescriptor* active);
int sys_getTid(TaskDescriptor* active);
int sys_yield(TaskDescriptor* active);
int sys_threadexit(TaskDescriptor* active);
int sys_create(TaskDescriptor* active, TaskCreateParameters* params);
int sys_changePriority(TaskDescriptor* active);

#endif /* SYS_SYSCALL_H_ */
