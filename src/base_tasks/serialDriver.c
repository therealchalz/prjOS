/*
 * task_serialDriver.c
 *
 *  Created on: Dec 29, 2014
 *      Author: che
 */
#include <prjOS/include/base_tasks/serialDriver.h>

static void processMessage(SerialDriverData* data, uint32_t otherTask, char* message, uint32_t size) {
	if (size < 4) {
		prjReply(otherTask, message, size);
		return;
	}

	//read the message type out of the message and move the data pointer forward
	uint32_t messageType = *((uint32_t*)message);
	message+=sizeof(uint32_t);

	uint32_t ch = 0;

	switch (messageType) {
	case MESSAGE_CHARACTER_RECEIVED:
		prjReply(otherTask, (char*)&ch, 4);
		break;
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
		//[type(4)][len(2)][msg(len bytes)]
		if (size < 7)
			return;
		uint16_t msgLen = *((uint16_t*) message);
		message += sizeof(uint16_t);
		if (msgLen > MAX_MESSAGE_LEN)
			msgLen = MAX_MESSAGE_LEN;

		char* msg = message;
		uint16_t bytesSent = msgLen;
		while (msgLen--) {
			bwputc(*msg);
			msg++;
		}

		prjReply(otherTask, (char*)&bytesSent, 2);

		break;
	case MESSAGE_QUIT:
		data->keepRunning = false;
		break;
	default:
		prjReply(otherTask, message, size);
		break;
	}
}


void incomingCharacterPollingTask(void) {
	uint32_t parentTid = prjGetParentTid();
	uint32_t eventId = EVENTID_USB0;
	char run = 1;

	uint32_t message = MESSAGE_CHARACTER_RECEIVED;
	uint32_t reply = 0;

	while (run) {
		prjAwaitEvent(eventId);

		prjSend(parentTid, (char*)&message, 4, (char*)&reply, 4);
	}
}

//Messages:
// [4 bytes - message type][optional 2 byte message length incl null termination][optional n byte string, null terminated]

void serialDriverTask() {
	SerialDriverData data;

	int otherTask;
	char message[MAX_MESSAGE_LEN];
	int msgLen;

	data.keepRunning = true;
	data.blockedCharTid = 0;

	prjRegisterAs(NAMESERVER_NAME_SERIAL_DRIVER);

	prjCreateMicroTask(incomingCharacterPollingTask);

	while (data.keepRunning) {

		//TODO: improve fairness, or have some form of locking, for reads
		msgLen = prjReceive(&otherTask, message, MAX_MESSAGE_LEN);
		if (msgLen > 0) {
			processMessage(&data, otherTask, message, msgLen);
		}

		if (data.blockedCharTid) {
			uint32_t ch = readCharNonblocking();
			if (ch != -1) {
				uint32_t ret = prjReply(data.blockedCharTid, (char*)&ch, 4);
				data.blockedCharTid = 0;
				if (ret < 0) {
					bwprintf("Serial Driver: Bad reply return value: %d\n\r", ret);
				}
			}
		}
	}

	prjExit();
}
