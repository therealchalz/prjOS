/*
 * putstr.c
 *
 *  Created on: Dec 30, 2014
 *      Author: che
 */


#include <prjOS/include/base_tasks/serialDriver.h>
#include <prjOS/include/sys_syscall.h>

uint32_t prjPutStr(const char* str, task_id_t serialTid) {
	uint16_t len = strlen(str);

	if (len == 0)
		return 0;

	return prjPutBuf((const uint8_t*)str, len, serialTid);}
