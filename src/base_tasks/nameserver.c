/***************************************
 * Copyright (c) 2013 Charles Hache <chache@brood.ca>. All rights reserved. 
 * 
 * This file is part of the prjOS project.
 * prjOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * prjOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with prjOS.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * Contributors:
 *     Charles Hache <chache@brood.ca> - initial API and implementation
***************************************/

/*
 * nameserver.c
 */

#include "prjOS/include/base_tasks/nameserver.h"
#include "prjOS/include/syscall.h"
#include "string.h"
#include "prjOS/include/bwio.h"

void nameserverEntry() {
	uint32_t nameserverTid = prjGetTid();
	bwprintf("NAMESERVER: INFO: Nameserver started: %d.\n\r", nameserverTid);
	char run = 1;

	//Set up storage
	NameserverEntry nameEntries[NAMESERVER_MAX_NAMES];

	memset(nameEntries, 0, NAMESERVER_MAX_NAMES * sizeof(NameserverEntry));

	//Read in query
	uint32_t sender;
	NameserverQuery query;

	prjRegisterNameserver(nameserverTid);

	while(run) {

		//bwprintf("NAMESERVER: DEBUG: Waiting for message.\n\r");

		uint32_t receiveRet = prjReceive(&sender, (uint8_t*)&query, sizeof(query));
		if (receiveRet < 0) {
			bwprintf("NAMESERVER: ERR: Receive returned %d.\n\r", receiveRet);
		}

		if (receiveRet != sizeof(query)) {
			//echo invalid messages - useful for debugging and seeing if the server is booted yet.
			prjReply(sender, (uint8_t*)&query, receiveRet);
			continue;
		}

		if(sender==0) {
			bwprintf("NAMESERVER: ERR: Sender is 0.\n\r");
			continue;
		}
		//bwprintf("NAMESERVER: DEBUG: Received request from: %d.  Opcode: %d.\n\r", sender, query.operation);
		//bwprintf("NAMESERVER: DEBUG: Got string: %s\n\r", query.buffer);
		uint32_t i;

		uint32_t replyOperation;
		uint32_t returnValue;

		switch (query.operation) {
		case NAMESERVER_OPERATION_REGISTER:

			//Find entry to save at
			for (i=0; i<NAMESERVER_MAX_NAMES; i++) {

				//find empty entry
				if (nameEntries[i].tid == 0) {
					nameEntries[i].tid = sender;

					if (query.bufferLen <= NAMESERVER_MAX_NAME_LENGTH){
						strcpy(nameEntries[i].name, query.buffer);
						replyOperation = NAMESERVER_OPERATION_REGISTER;
					} else {
						bwprintf("NAMESERVER: ERR: Name too long error in register request. Name length: %d.\n\r", query.bufferLen);
						replyOperation = NAMESERVER_OPERATION_TOO_LONG;
					}
					break;
				} else { //check for duplicate entry
					if (strcmp(query.buffer, nameEntries[i].name) == 0) {
						//bwprintf("NAMESERVER: DEBUG: Successfully overwrote registration.\n\r");
						nameEntries[i].tid = sender;
						replyOperation = NAMESERVER_OPERATION_REGISTER;
						break;
					}
				}
			}
			if (i==NAMESERVER_MAX_NAMES) { //no space
				bwprintf("NAMESERVER: ERR: No space left to store mappings.\n\r");
				replyOperation = NAMESERVER_OPERATION_NO_SPACE;
			}

			break;
		case NAMESERVER_OPERATION_WHOIS:

			//Retreive the info
			for (i=0; i<NAMESERVER_MAX_NAMES; i++) {
				//bwprintf("NAMESERVER: DEBUG: Checking %s and %s.\n\r", query.buffer, nameEntries[i].name);
				if (strcmp(query.buffer, nameEntries[i].name) == 0) {
					//bwprintf("NAMESERVER: INFO: Found tid of target.\n\r");
					replyOperation = NAMESERVER_OPERATION_WHOIS;
					returnValue = nameEntries[i].tid;
					break;
				}
			}
			if (i == NAMESERVER_MAX_NAMES) { //Couldn't find
				replyOperation = NAMESERVER_OPERATION_WHOIS;
				bwprintf("NAMESERVER: WARN: Couldn't find tid: %s.\n\r", query.buffer);
				returnValue = -1;
			}
			break;
		case NAMESERVER_OPERATION_EXIT:
			//bwprintf("NAMESERVER: INFO: Received exit request.\n\r");
			replyOperation = NAMESERVER_OPERATION_WHOIS;
			run = 0;
			break;
		default:
			//bwprintf("NAMESERVER: WARN: Invalid operation query sent to nameserver: %d.\n\r", query.operation);
			replyOperation = NAMESERVER_OPERATION_INVALID;
			break;
		}

		//Build reply
		NameserverQuery reply;
		reply.senderTid = nameserverTid;
		*((uint32_t*)reply.buffer) = returnValue;
		reply.bufferLen = sizeof(returnValue);
		reply.operation = replyOperation;
		//bwprintf("NAMESERVER: DEBUG: Replying to sender: %d.\n\r", sender);
		int replyRet = prjReply(sender, (uint8_t*)&reply, sizeof(reply));
		if (replyRet != 0) {
			bwprintf("NAMESERVER: ERR: Reply returned %d.\n\r", replyRet);
		}
	}
	prjExit();
}
