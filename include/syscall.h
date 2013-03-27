/*
 * syscall.h
 */

#ifndef SYSCALL_H_
#define SYSCALL_H_

#define SYSCALL_HARDWARE_CALL		0
#define SYSCALL_GET_TID				1
#define SYSCALL_GET_PARENT_TID		2
#define SYSCALL_YIELD				3

int getParentTid(void);
int getTid(void);
int yield(void);

#endif /* SYSCALL_H_ */
