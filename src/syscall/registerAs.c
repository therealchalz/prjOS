
/*
 * registerAs.c
 */

#include <syscall.h>
#include <task.h>
#include <kernel_data.h>
#include <string.h>
#include <bwio.h>
#include <base_tasks/nameserver.h>

int prjRegisterAs(char* name) {
	//Construct query
	NameserverQuery query;
	NameserverQuery reply;

	query.senderTid = prjGetTid();
	query.operation = NAMESERVER_OPERATION_REGISTER;
	memcpy(query.buffer, name, strlen(name)+1);
	query.bufferLen = strlen(query.buffer)+1;

	//Send query to nameserver
	int ret = prjSend(prjWhoIs(NAMESERVER_NAMESTR), (char*)&query, sizeof(query), (char*)&reply, sizeof(reply));
	switch (ret) {
	case ERR_SEND_TASKID_DNE:
		bwprintf("REGISTERAS: ERR: Name server tid invalid.\n\r");
		ret = ERR_REGISTERAS_INVALID_TID;
		break;
	case ERR_SEND_TASKID_NOTFOUND:
		bwprintf("REGISTERAS: ERR: Couldn't find name server.\n\r");
		ret = ERR_REGISTERAS_WRONG_TID;
		break;
	case ERR_SEND_BAD_BUFFER:
		bwprintf("REGISTERAS: ERR: One of the buffers was bad.\n\r");
		ret = ERR_REGISTERAS_ERROR;
		break;
	case ERR_SEND_INCOMPLETE:
		bwprintf("REGISTERAS: ERR: The send was incomplete.\n\r");
		ret = ERR_REGISTERAS_ERROR;
		break;
	default:
		bwprintf("REGISTERAS: DEBUG: The send returned: %d\n\r", ret);
		break;
	}
	if (ret > 0) {
		switch (reply.operation) {
		case NAMESERVER_OPERATION_TOO_LONG:
			bwprintf("REGISTERAS: ERR: The length of the name to register was too long.\n\r");
			ret = ERR_REGISTERAS_TOO_LONG;
			break;
		case NAMESERVER_OPERATION_ERROR:
			bwprintf("REGISTERAS: ERR: The source or destination buffer was invalid.\n\r");
			ret = ERR_REGISTERAS_ERROR;
			break;
		case NAMESERVER_OPERATION_NO_SPACE:
			bwprintf("REGISTERAS: ERR: The nameserver reported it was full.\n\r");
			ret = ERR_REGISTERAS_NO_SPACE;
			break;
		case NAMESERVER_OPERATION_REGISTER:
			bwprintf("REGISTERAS: DEBUG: The register completed.\n\r");
			ret = 0;
			break;
		default:
			bwprintf("REGISTERAS: ERR: The response from the nameserver was invalid.\n\r");
			ret = ERR_REGISTERAS_ERROR;
			break;
		}
	}

	return ret;
}

