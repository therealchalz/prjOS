/*
 * getParentTid.c
 */


#include <syscall.h>
#include <task.h>

int getParentTid() {
	int ret;
	asm (svcArg(SYSCALL_GET_PARENT_TID));
	asm (" MOV %[ret], R0\n": [ret] "=r" (ret): :);
	return ret;
}

int sys_getParentTid(TaskDescriptor* active){
	return active->parentId;
}
