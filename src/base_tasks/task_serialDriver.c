/*
 * task_serialDriver.c
 *
 *  Created on: Dec 29, 2014
 *      Author: che
 */
#include "prjOS/include/base_tasks/task_serialDriver.h"

static void processMessage(SerialDriverData* data, uint32_t otherTask, char* message, uint32_t size) {
	if (size < 4) {
		prjReply(otherTask, message, size);
		return;
	}

	uint32_t messageType = *((uint32_t*)message);
	message+=sizeof(uint32_t);
	uint32_t ch;

	switch (messageType) {
	case MESSAGE_GET_CHAR:
		ch = readCharNonblocking();
		if (ch == -1) {
			data->blockedCharTid = otherTask;
		} else {
			prjReply(otherTask, (char*)&ch, 4);
		}
		break;
	case MESSAGE_GET_CHAR_NONBLOCKING:
		ch = readCharNonblocking();
		prjReply(otherTask, (char*)&ch, 4);
		break;
	case MESSAGE_SEND_MESSAGE:
		if (size < 7)
			return;
		uint16_t msgLen = *((uint16_t*) message);
		message += sizeof(uint16_t);
		if (msgLen > MAX_MESSAGE_LEN)
			msgLen = MAX_MESSAGE_LEN;

		char* msg = message;

		if (msg[msgLen-1] != 0) {
			bwprintf("ERROR: SerialDriver received non-null terminal string (length %d)", msgLen);
		} else {
			bwprintf("%s", msg);
		}

		break;
	case MESSAGE_QUIT:
		data->keepRunning = false;
		break;
	default:
		prjReply(otherTask, message, size);
		return;
		break;
	}
}

//Messages:
// [4 bytes - message type][optional 2 byte message length incl null termination][optional n byte string, null terminated]

void task_serialDriver() {
	SerialDriverData data;

	int otherTask;
	char message[MAX_MESSAGE_LEN];
	int msgLen;

	data.keepRunning = true;
	data.blockedCharTid = 0;

	prjRegisterAs(NAMESERVER_NAME_SERIAL_DRIVER);

	while (data.keepRunning) {

		msgLen = prjReceive(&otherTask, message, MAX_MESSAGE_LEN);
		if (msgLen > 0) {
			processMessage(&data, otherTask, message, msgLen);
		}

		if (data.blockedCharTid) {
			uint32_t ch = readCharNonblocking();
			if (ch != -1) {
				prjReply(otherTask, (char*)&ch, 4);
				data.blockedCharTid = 0;
			}
		}

		prjYield();
	}

	prjExit();
}
