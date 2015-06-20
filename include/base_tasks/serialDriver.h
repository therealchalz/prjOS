/*
 * task_serialDriver.h
 *
 *  Created on: Dec 29, 2014
 *      Author: che
 */

#ifndef _SERIAL_DRIVER_H_
#define _SERIAL_DRIVER_H_

#include <stdint.h>
#include <stdbool.h>
#include <prjOS/include/syscall.h>
#include <prjOS/include/task.h>
#include <prjOS/include/kernel_data.h>
#include <string.h>
#include <prjOS/include/bwio.h>
#include <prjOS/include/base_tasks/nameserver.h>

#define MAX_MESSAGE_LEN			512

#define MESSAGE_GET_CHAR				0x1
#define MESSAGE_GET_CHAR_NONBLOCKING	0x2
#define MESSAGE_SEND_MESSAGE			0x3
#define MESSAGE_QUIT					0x4
#define MESSAGE_CHARACTER_RECEIVED		0x5


extern void serialDriverTask();

typedef struct {
	uint32_t receiveAwaitEventId;		//Await Event parameter for blocking to receive a character
	uint32_t (*getCharNonBlocking)(void);	//pointer to a non-blocking read call
	uint32_t (*putCharBlocking)(uint32_t);	//pointer to a blocking write call
} SerialDriverInitData;

typedef struct {
	bool keepRunning;
	uint32_t blockedCharTid;
	uint32_t receiveAwaitEventId;			//Await Event parameter for blocking to receive a character
	uint32_t (*getCharNonBlocking)(void);	//pointer to a non-blocking read call
	uint32_t (*putCharBlocking)(uint32_t);	//pointer to a blocking write call
} SerialDriverData;

uint32_t initializeSerialDriver(SerialDriverInitData initData);

#endif //_SERIAL_DRIVER_H_
