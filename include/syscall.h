/*
 * syscall.h
 */

#ifndef SYSCALL_H_
#define SYSCALL_H_

#define SYSCALL_HARDWARE_CALL		0
#define SYSCALL_GET_TID				1
#define SYSCALL_GET_PARENT_TID		2
#define SYSCALL_YIELD				3
#define SYSCALL_THREADEXIT			4
#define SYSCALL_CREATE				5
#define SYSCALL_CHANGEPRIORITY		6

int getParentTid(void);
int getTid(void);
int yield(void);
int threadExit(void);
int create(int priority, void* entryPoint);
int changePriority(int newPriority);

#endif /* SYSCALL_H_ */
