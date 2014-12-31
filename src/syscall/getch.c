/*
 * getch.c
 *
 *  Created on: Dec 29, 2014
 *      Author: che
 */
#include <prjOS/include/base_tasks/serialDriver.h>
#include "prjOS/include/syscall.h"
#include "prjOS/include/task.h"
#include "prjOS/include/base_tasks/nameserver.h"

uint32_t prjGetCh(uint32_t* charOut, uint32_t serialTid) {
	if (serialTid == 0) {
		return -1;
	}

	uint32_t ret;
	uint32_t message = MESSAGE_GET_CHAR;
	uint32_t ch;
	prjSend(serialTid,
				(char*)(&message), sizeof(uint32_t),
				(char*)(&charOut), sizeof(uint32_t));
	return 1;
}

uint32_t prjGetChNonBlocking(uint32_t* charOut, uint32_t serialTid) {

	if (serialTid == 0) {
		return -1;
	}

	uint32_t ret;
	uint32_t message = MESSAGE_GET_CHAR_NONBLOCKING;
	uint32_t ch;
	prjSend(serialTid,
				(char*)(&message), sizeof(uint32_t),
				(char*)(&ch), sizeof(uint32_t));
	if (ch == 0xfffffff) {
		ret = -1;
	} else {
		*charOut = ch;
		ret = 1;
	}

	return ret;
}

