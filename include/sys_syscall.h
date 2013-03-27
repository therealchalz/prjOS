/*
 * sys_syscall.h
 */

#ifndef SYS_SYSCALL_H_
#define SYS_SYSCALL_H_

int sys_getParentTid(TaskDescriptor* active);
int sys_getTid(TaskDescriptor* active);
int sys_yield(TaskDescriptor* active);

#endif /* SYS_SYSCALL_H_ */
