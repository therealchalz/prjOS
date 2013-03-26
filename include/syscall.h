/*
 * syscall.h
 */

#ifndef SYSCALL_H_
#define SYSCALL_H_

#define SYSCALL_HARDWARE_CALL		0
#define SYSCALL_GET_TID				1
#define SYSCALL_GET_PARENT_TID		2

int getParentTid(void);
int getTid(void);

#endif /* SYSCALL_H_ */
