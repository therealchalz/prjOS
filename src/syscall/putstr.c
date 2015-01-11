/*
 * putstr.c
 *
 *  Created on: Dec 30, 2014
 *      Author: che
 */


#include <prjOS/include/base_tasks/serialDriver.h>
#include "prjOS/include/syscall.h"
#include "prjOS/include/task.h"
#include "string.h"

uint32_t prjPutStr(const char* str, uint32_t serialTid) {
	uint16_t len = strlen(str);

	if (len == 0)
		return 0;

	return prjPutBuf((const uint8_t*)str, len, serialTid);}
