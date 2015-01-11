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

void serialUITask(void) {

	uint32_t tid;
	uint32_t ch = 0;
	prjReceive((uint32_t*)&tid, (uint8_t*)&ch, 1);
	prjReply(tid, (uint8_t*)&ch, 1);

	char run = 1;

	tid = prjWhoIs(NAMESERVER_NAME_SERIAL_DRIVER);

	char line[MAX_LINE_LENGTH];
	char command[MAX_LINE_LENGTH];



	uint16_t cursor = 0;
	clearLine(line, &cursor);
	prjPutStr(line, tid);
	while (run) {
		if(prjGetCh(&ch, tid) > 0) {
			if (ch >= 32 && ch <= 126) {
				addchar(line, ch, &cursor);
				prjPutStr("\r", tid);
				prjPutStr(line, tid);
			} else if (ch == 8) {
				backspace(line, &cursor);
				prjPutStr("\r", tid);
				prjPutStr(line, tid);
				shortenLine(line, &cursor);
				prjPutStr("\r", tid);
				prjPutStr(line, tid);
			} else if (ch == 13) {
				getCommand(line, command);
				prjPutStr("\n\rGot Command:\n\r", tid);
				prjPutStr(command, tid);
				prjPutStr("\n\r", tid);
				clearLine(line, &cursor);
				prjPutStr(line, tid);
			}
		}
	}
	//prjPutStr("")
}
