/*
 * serialUI.c
 *
 *  Created on: Dec 30, 2014
 *      Author: che
 */

#include "prjOS/include/base_tasks/serialUI.h"

void serialUITask(void);

uint32_t initializeSerialUI(SerialUIInitData initData) {
	uint32_t uiTid;
	uint32_t dummy;

	SerialUIData data;

	data.serialDriverTid = initData.serialDriverTid;

	uiTid = prjCreate(0, &serialUITask);

	prjSend(uiTid, (uint8_t*)&data, sizeof(SerialUIData), (uint8_t*)&dummy, sizeof(uint32_t));

	return uiTid;
}

static void clearLine(uint8_t* line, uint16_t* cursor) {
	memset(line, 0, MAX_LINE_LENGTH);
	line[0] = '>';
	*cursor = 1;
}

static void shortenLine(uint8_t* line, uint16_t* cursor) {
	int i = *cursor;
	for (; i<MAX_LINE_LENGTH; i++) {
		line[i] = 0;
	}
}

static void addchar(uint8_t* line, uint8_t ch, uint16_t* cursor) {
	if (*cursor < MAX_LINE_LENGTH-1) {
		line[(*cursor)] = ch;
		line[(*cursor)+1] = 0;
		*cursor = *cursor + 1;
	}
}
static void backspace(uint8_t* line, uint16_t* cursor) {
	if (*cursor > 1) {
		line[*(cursor)-1] = ' ';
		line[*(cursor)] = 0;
		*cursor = *cursor - 1;
	}
}
static void getCommand(uint8_t* line, uint8_t* command) {
	uint8_t* src = line+1;
	uint8_t* dest = command;
	uint16_t soFar = 0;
	while (soFar++ < MAX_LINE_LENGTH && *src != 0) {
		*dest = *src;
		dest++;
		src++;
	}
	*dest = 0;
}

void serialUICharacterCourier(void) {
	uint8_t run = 1;
	uint32_t uiTaskTid;
	uint32_t serialDriverTid;
	serialUIMessage message;

	uint32_t ch = 0;
	prjReceive((uint32_t*)&uiTaskTid, (uint8_t*)&serialDriverTid, sizeof(uint32_t));
	prjReply(uiTaskTid, (uint8_t*)&ch, sizeof(uint32_t));

	message.messageType = SERIAL_UI_MESSAGE_TYPE_CHARACTER_RECEIVED;

	while (run) {
		if(prjGetCh(&ch, serialDriverTid) > 0) {
			message.data = ch;
			prjSend(uiTaskTid, (uint8_t*)&message, sizeof(serialUIMessage), (uint8_t*)&ch, sizeof(uint32_t));
		}
	}
	//TODO: how to get this task to exit?
}

void serialUITask(void) {
	uint32_t otherTask;
	uint32_t dummy;
	uint8_t ch;

	uint8_t run = 1;
	uint8_t line[MAX_LINE_LENGTH];
	uint8_t command[MAX_LINE_LENGTH];
	uint16_t cursor = 0;
	uint32_t commandExecutorTid = 0;

	SerialUIData uiData;
	serialUIMessage message;

	//Receive init data from initializer
	prjReceive(&otherTask, (uint8_t*)&uiData, sizeof(SerialUIData));

	//Create the character courier
	dummy = prjCreateMicroTask(serialUICharacterCourier);
	prjSend(dummy, &uiData.serialDriverTid, sizeof(uint32_t), &dummy, sizeof(uint32_t));

	//Resume the initializer
	prjReply(otherTask, (uint8_t*)&dummy, sizeof(uint32_t));

	clearLine(line, &cursor);
	prjPutStr(line, uiData.serialDriverTid);
	while (run) {
		if (prjReceive((uint32_t*)&otherTask, (uint8_t*)&message, sizeof(serialUIMessage)) > 0) {
			switch (message.messageType) {
			case SERIAL_UI_MESSAGE_TYPE_COMMAND_SUBSCRIBE:
				if (commandExecutorTid == 0) {
					commandExecutorTid = otherTask;
				} else {
					uint8_t dummy = 0;
					prjReply(otherTask, &dummy, 1);
				}
				break;
			case SERIAL_UI_MESSAGE_TYPE_REDRAW:
				prjPutStr("\r", uiData.serialDriverTid);
				prjPutStr(line, uiData.serialDriverTid);
				prjReply(otherTask, (uint8_t*)&dummy, 1);
				break;
			case SERIAL_UI_MESSAGE_TYPE_CHARACTER_RECEIVED:
				prjReply(otherTask, (uint8_t*)&dummy, 1);
				ch = message.data;
				if (ch >= 32 && ch <= 126) {
					addchar(line, ch, &cursor);
					prjPutStr("\r", uiData.serialDriverTid);
					prjPutStr(line, uiData.serialDriverTid);
				} else if (ch == 8) {
					backspace(line, &cursor);
					prjPutStr("\r", uiData.serialDriverTid);
					prjPutStr(line, uiData.serialDriverTid);
					shortenLine(line, &cursor);
					prjPutStr("\r", uiData.serialDriverTid);
					prjPutStr(line, uiData.serialDriverTid);
				} else if (ch == 13) {
					getCommand(line, command);
					prjPutStr("\n\r", uiData.serialDriverTid);
					clearLine(line, &cursor);
					if (commandExecutorTid != 0) {
						prjReply(commandExecutorTid, &command, strlen(command)+1);
						commandExecutorTid = 0;
					} else {
						prjPutStr(line, uiData.serialDriverTid);
					}
				}
				break;
			}
		}
	}
	//prjPutStr("")
	prjExit();
}
