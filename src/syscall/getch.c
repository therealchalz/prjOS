/*
 * getch.c
 *
 *  Created on: Dec 29, 2014
 *      Author: che
 */
#include <prjOS/include/base_tasks/serialDriver.h>
#include <prjOS/include/sys_syscall.h>

uint32_t prjGetCh(uint32_t* charOut, task_id_t serialTid) {
	if (serialTid == 0) {
		return -1;
	}

	uint32_t ret;
	uint32_t message = MESSAGE_GET_CHAR;
	uint32_t ch;
	ret = prjSend(serialTid,
				(uint8_t*)(&message), sizeof(uint32_t),
				(uint8_t*)(&ch), sizeof(uint32_t));
	if (ret < 1)
		return -1;
	*charOut = ch;
	return 1;
}

uint32_t prjGetChNonBlocking(uint32_t* charOut, task_id_t serialTid) {

	if (serialTid == 0) {
		return -1;
	}

	uint32_t ret;
	uint32_t message = MESSAGE_GET_CHAR_NONBLOCKING;
	uint32_t ch;
	prjSend(serialTid,
				(uint8_t*)(&message), sizeof(uint32_t),
				(uint8_t*)(&ch), sizeof(uint32_t));
	if (ch == 0xfffffff) {
		ret = -1;
	} else {
		*charOut = ch;
		ret = 1;
	}

	return ret;
}

