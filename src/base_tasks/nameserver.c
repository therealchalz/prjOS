/*
 * nameserver.c
 */

#include <base_tasks/nameserver.h>
#include <syscall.h>
#include <string.h>
#include <bwio.h>

void nameserverEntry() {
	int nameserverTid = prjGetTid();
	bwprintf("NAMESERVER: INFO: Nameserver started: %d.\n\r", nameserverTid);
	char run = 1;

	//Set up storage
	NameserverEntry nameEntries[NAMESERVER_MAX_NAMES];

	memset(nameEntries, 0, NAMESERVER_MAX_NAMES * sizeof(NameserverEntry));

	//Read in query
	int sender;
	NameserverQuery query;
	NameserverQuery reply;
	int receiveRet;	//return value for receive;
	int replyRet;	//return value for reply;
	int replyOperation;
	int returnValue;
	while(run) {

		bwprintf("NAMESERVER: DEBUG: Waiting for message.\n\r");

		receiveRet = prjReceive(&sender, (char*)&query, sizeof(query));
		if (receiveRet < 0) {
			bwprintf("NAMESERVER: ERR: Receive returned %d.\n\r", receiveRet);
		}

		if(sender==0) {
			bwprintf("NAMESERVER: ERR: Sender is 0.\n\r");
			continue;
		}
		bwprintf("NAMESERVER: DEBUG: Received request from: %d.  Opcode: %d.\n\r", sender, query.operation);
		bwprintf("NAMESERVER: DEBUG: Got string: %s\n\r", query.buffer);
		int i;
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
						bwprintf("NAMESERVER: DEBUG: Successfully overwrote registration.\n\r");
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
				bwprintf("NAMESERVER: DEBUG: Checking %s and %s.\n\r", query.buffer, nameEntries[i].name);
				if (strcmp(query.buffer, nameEntries[i].name) == 0) {
					bwprintf("NAMESERVER: INFO: Found tid of target.\n\r");
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
			bwprintf("NAMESERVER: INFO: Received exit request.\n\r");
			replyOperation = NAMESERVER_OPERATION_WHOIS;
			run = 0;
			break;
		default:

			bwprintf("NAMESERVER: WARN: Invalid operation query sent to nameserver: %d.\n\r", query.operation);
			replyOperation = NAMESERVER_OPERATION_INVALID;
			break;
		}

		//Build reply
		*((int*)reply.buffer) = returnValue;
		reply.bufferLen = sizeof(returnValue);
		reply.operation = replyOperation;
		bwprintf("NAMESERVER: DEBUG: Replying to sender: %d.\n\r", sender);
		replyRet = prjReply(sender, (char*)&reply, sizeof(reply));
		switch (replyRet) {
		default:
			if (replyRet != 0) {
				bwprintf("NAMESERVER: ERR: Reply returned %d.\n\r", replyRet);
			}
			break;
		}
	}
	prjExit();
}
