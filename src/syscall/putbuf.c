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
#include "prjOS/include/base_tasks/nameserver.h"

//len is number of bytes in str
uint32_t prjPutBuf(const uint8_t* str, uint16_t len, uint32_t serialTid) {
	if (serialTid == 0) {
		return -1;
	}

	if (len > MAX_MESSAGE_LEN) {
		len = MAX_MESSAGE_LEN;
	}

	uint8_t buffer[MAX_MESSAGE_LEN+6];

	*((uint32_t*)buffer) = MESSAGE_SEND_MESSAGE;
	*((uint16_t*)(buffer+4)) = len;
	memcpy(buffer+6, str, len);
	uint16_t ret = len;

	prjSend(serialTid,
				buffer, len+6,
				(uint8_t*)(&ret), sizeof(uint16_t));
	return len;
}
