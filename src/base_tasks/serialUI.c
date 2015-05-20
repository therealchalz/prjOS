/*
 * serialUI.c
 *
 *  Created on: Dec 30, 2014
 *      Author: che
 */

#include "prjOS/include/base_tasks/serialUI.h"

static void clearLine(char* line, uint16_t* cursor) {
	memset(line, 0, MAX_LINE_LENGTH);
	line[0] = '>';
	*cursor = 1;
}

static void shortenLine(char* line, uint16_t* cursor) {
	int i = *cursor;
	for (; i<MAX_LINE_LENGTH; i++) {
		line[i] = 0;
	}
}

static void addchar(char* line, char ch, uint16_t* cursor) {
	if (*cursor < MAX_LINE_LENGTH-1) {
		line[(*cursor)] = ch;
		line[(*cursor)+1] = 0;
		*cursor = *cursor + 1;
	}
}
static void backspace(char* line, uint16_t* cursor) {
	if (*cursor > 1) {
		line[*(cursor)-1] = ' ';
		line[*(cursor)] = 0;
		*cursor = *cursor - 1;
	}
}
static void getCommand(char* line, char* command) {
	char* src = line+1;
	char* dest = command;
	uint16_t soFar = 0;
	while (soFar++ < MAX_LINE_LENGTH && *src != '0') {
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
	prjReceive((uint32_t*)&uiTaskTid, (uint8_t*)&ch, sizeof(uint32_t));
	prjReply(uiTaskTid, (uint8_t*)&ch, sizeof(uint32_t));

	serialDriverTid = prjWhoIs(NAMESERVER_NAME_SERIAL_DRIVER);

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
	uint32_t tid;
	uint32_t ch = 0;
	serialUIMessage message;

	prjReceive((uint32_t*)&tid, (uint8_t*)&ch, 1);
	prjReply(tid, (uint8_t*)&ch, 1);

	tid = prjCreateMicroTask(serialUICharacterCourier);
	prjSend(tid, &ch, sizeof(ch), &ch, sizeof(ch));
	bwprintf("Init: Serial UI running\n\r");

	uint8_t run = 1;

	uint32_t serialDriverTid = prjWhoIs(NAMESERVER_NAME_SERIAL_DRIVER);

	char line[MAX_LINE_LENGTH];
	char command[MAX_LINE_LENGTH];

	uint16_t cursor = 0;
	clearLine(line, &cursor);
	prjPutStr(line, serialDriverTid);
	while (run) {
		if (prjReceive((uint32_t*)&tid, (uint8_t*)&message, sizeof(serialUIMessage)) > 0) {
			switch (message.messageType) {
			case SERIAL_UI_MESSAGE_TYPE_COMMAND_SUBSCRIBE:
				prjReply(tid, (uint8_t*)&ch, 1);
				break;
			case SERIAL_UI_MESSAGE_TYPE_CHARACTER_RECEIVED:
				prjReply(tid, (uint8_t*)&ch, 1);
				ch = message.data;
				if (ch >= 32 && ch <= 126) {
					addchar(line, ch, &cursor);
					prjPutStr("\r", serialDriverTid);
					prjPutStr(line, serialDriverTid);
				} else if (ch == 8) {
					backspace(line, &cursor);
					prjPutStr("\r", serialDriverTid);
					prjPutStr(line, serialDriverTid);
					shortenLine(line, &cursor);
					prjPutStr("\r", serialDriverTid);
					prjPutStr(line, serialDriverTid);
				} else if (ch == 13) {
					getCommand(line, command);
					prjPutStr("\n\rGot Command:\n\r", serialDriverTid);
					prjPutStr(command, serialDriverTid);
					prjPutStr("\n\r", serialDriverTid);
					clearLine(line, &cursor);
					prjPutStr(line, serialDriverTid);
				}
				break;
			}
		}
	}
	//prjPutStr("")
}
