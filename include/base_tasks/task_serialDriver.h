/*
 * task_serialDriver.h
 *
 *  Created on: Dec 29, 2014
 *      Author: che
 */

#include <stdint.h>
#include <stdbool.h>
#include "prjOS/include/syscall.h"
#include "prjOS/include/task.h"
#include "prjOS/include/kernel_data.h"
#include "string.h"
#include "prjOS/include/bwio.h"
#include "prjOS/include/base_tasks/nameserver.h"

#define MAX_MESSAGE_LEN			512
#define MAX_PENDING_MESSAGES	6

#define MESSAGE_GET_CHAR				0x1
#define MESSAGE_GET_CHAR_NONBLOCKING	0x2
#define MESSAGE_SEND_MESSAGE			0x3
#define MESSAGE_QUIT					0x4

extern void task_serialDriver();

typedef struct {
	bool keepRunning;
	uint32_t blockedCharTid;
} SerialDriverData;
