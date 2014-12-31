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

	while (data.keepRunning) {

		// If someone is blocked waiting for data, we constantly poll to see if
		// a character has arrived, otherwise we just block until someone has a
		// request.
		//TODO: improve fairness, or have some form of locking
		if (data.blockedCharTid) {
			msgLen = prjReceiveNonBlocking(&otherTask, message, MAX_MESSAGE_LEN);
		} else {
			msgLen = prjReceive(&otherTask, message, MAX_MESSAGE_LEN);
			//msgLen = prjReceiveNonBlocking(&otherTask, message, MAX_MESSAGE_LEN);
		}
		if (msgLen > 0) {
			processMessage(&data, otherTask, message, msgLen);
		}

		if (data.blockedCharTid) {
			uint32_t ch = readCharNonblocking();
			if (ch != -1) {
				uint32_t ret = prjReply(otherTask, (char*)&ch, 4);
				data.blockedCharTid = 0;
				if (ret < 0) {
					bwprintf("Serial Driver: Bad reply return value: %d\n\r", ret);
				}
			}
		}

		prjYield();
	}

	prjExit();
}
