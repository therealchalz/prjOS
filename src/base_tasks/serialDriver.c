/*
 * task_serialDriver.c
 *
 *  Created on: Dec 29, 2014
 *      Author: che
 */
#include <prjOS/include/base_tasks/serialDriver.h>

task_id_t initializeSerialDriver(SerialDriverInitData initData) {
	task_id_t driverTid;
	uint32_t dummy;
	SerialDriverData driverData;

	driverData.getCharNonBlocking = initData.getCharNonBlocking;
	driverData.putCharBlocking = initData.putCharBlocking;
	driverData.receiveAwaitEventId = initData.receiveAwaitEventId;

	driverTid = prjCreate(0, serialDriverTask);

	prjSend(driverTid, (uint8_t*)&driverData, sizeof(SerialDriverData), (uint8_t*)&dummy, sizeof(dummy));

	return driverTid;
}

static void processMessage(SerialDriverData* data, task_id_t otherTask, uint8_t* message, uint32_t size) {
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
		prjReply(otherTask, (uint8_t*)&ch, 4);
		if (data->blockedCharTid) {
			uint32_t ch = data->getCharNonBlocking();
			if (ch != -1) {
				uint32_t ret = prjReply(data->blockedCharTid, (uint8_t*)&ch, 4);
				data->blockedCharTid = 0;
			}
		}
		break;
	case MESSAGE_GET_CHAR:
		ch = data->getCharNonBlocking();
		if (ch == -1) {
			if (data->blockedCharTid == 0) {
				//bwprintf("Serial Driver Block read from task%d\n\r", otherTask);
				data->blockedCharTid = otherTask;
			} else {
				//More than one task trying to read data, return -1 to second task
				ch = -1;
				prjReply(otherTask, (uint8_t*)&ch, 4);
			}
		} else {
			prjReply(otherTask, (uint8_t*)&ch, 4);
		}
		break;
	case MESSAGE_GET_CHAR_NONBLOCKING:
		ch = data->getCharNonBlocking();
		prjReply(otherTask, (uint8_t*)&ch, 4);
		break;
	case MESSAGE_SEND_MESSAGE:
		//[type(4)][len(2)][msg(len bytes)]
		if (size < 7)
			return;
		uint16_t msgLen = *((uint16_t*) message);
		message += sizeof(uint16_t);
		if (msgLen > MAX_MESSAGE_LEN)
			msgLen = MAX_MESSAGE_LEN;

		uint8_t* msg = message;
		uint16_t bytesSent = msgLen;
		while (msgLen--) {
			data->putCharBlocking(*msg);
			msg++;
		}

		prjReply(otherTask, (uint8_t*)&bytesSent, 2);

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
	task_id_t parentTid = prjGetParentTid();
	uint32_t eventId;
	char run = 1;

	uint32_t message = MESSAGE_CHARACTER_RECEIVED;
	uint32_t reply = 0;

	prjReceive(&reply, (uint8_t*)&eventId, sizeof(uint32_t));
	prjReply(reply, (uint8_t*)&eventId, sizeof(uint32_t));

	//TODO: find a way to make this task exit

	while (run) {
		prjAwaitEvent(eventId);

		prjSend(parentTid, (uint8_t*)&message, 4, (uint8_t*)&reply, 4);
	}
}

//Messages:
// [4 bytes - message type][optional 2 byte message length incl null termination][optional n byte string, null terminated]

void serialDriverTask() {
	SerialDriverData data;

	task_id_t otherTask;
	task_id_t pollingTask;
	uint8_t message[MAX_MESSAGE_LEN];
	uint32_t msgLen;
	uint32_t dummy;

	data.keepRunning = true;
	data.blockedCharTid = 0;

	//Receive data from initializer
	prjReceive(&otherTask, (uint8_t*)&data, sizeof(SerialDriverData));

	//Create and configure the character poller
	pollingTask = prjCreateMicroTask(incomingCharacterPollingTask);
	prjSend(pollingTask, (uint8_t*)&data.receiveAwaitEventId, sizeof(uint32_t), (uint8_t*)&dummy, sizeof(uint32_t));

	//Resume the initializer
	prjReply(otherTask, (uint8_t*)&otherTask, sizeof(uint32_t));

	while (data.keepRunning) {

		//TODO: improve fairness, or have some form of locking, for reads
		msgLen = prjReceive(&otherTask, message, MAX_MESSAGE_LEN);
		if (msgLen > 0) {
			processMessage(&data, otherTask, message, msgLen);
		}


	}

	prjExit();
}
