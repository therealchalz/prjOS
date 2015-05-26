/*
 * serialUI.h
 *
 *  Created on: Dec 30, 2014
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

#ifndef PRJOS_INCLUDE_BASE_TASKS_SERIALUI_H_
#define PRJOS_INCLUDE_BASE_TASKS_SERIALUI_H_

#define MAX_LINE_LENGTH		80

#define SERIAL_UI_MESSAGE_TYPE_COMMAND_SUBSCRIBE		1
#define SERIAL_UI_MESSAGE_TYPE_CHARACTER_RECEIVED		2
#define SERIAL_UI_MESSAGE_TYPE_REDRAW					3

typedef struct {
	uint8_t messageType;
	uint32_t data;
} serialUIMessage;

extern void serialUITask(void);

#endif /* PRJOS_INCLUDE_BASE_TASKS_SERIALUI_H_ */
