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
 * whois.c
 */

#include <syscall.h>
#include <task.h>
#include <kernel_data.h>
#include <string.h>
#include <bwio.h>
#include <base_tasks/nameserver.h>

int prjWhoIs (char *name) {
	int ret;
	if (strcmp(name,NAMESERVER_NAMESTR) == 0) {
		//Looking for NameServer - NOTE: Don't need arguments, so no big deal
		asm (svcArg(SYSCALL_WHOISNS));
		asm (" MOV %[ret], R0\n": [ret] "=r" (ret): :);
	} else {
		//Construct query
		struct NameserverQuery query;
		struct NameserverQuery reply;

		query.senderTid = prjGetTid();
		query.operation = NAMESERVER_OPERATION_WHOIS;
		memcpy(query.buffer, name, strlen(name)+1);
		query.bufferLen = strlen(name)+1;

		ret = prjSend(prjWhoIs(NAMESERVER_NAMESTR), (char*)&query, sizeof(query), (char*)&reply, sizeof(reply));
		switch (ret) {
		case ERR_SEND_TASKID_DNE:
			bwprintf("WHOIS: ERR: Name server tid invalid.\n\r");
			ret = ERR_WHOIS_INVALID_TID;
			break;
		case ERR_SEND_TASKID_NOTFOUND:
			bwprintf("WHOIS: ERR: Couldn't find name server.\n\r");
			ret = ERR_WHOIS_WRONG_TID;
			break;
		case ERR_SEND_BAD_BUFFER:
			bwprintf("WHOIS: ERR: One of the buffers was bad.\n\r");
			ret = ERR_WHOIS_ERROR;
			break;
		case ERR_SEND_INCOMPLETE:
			bwprintf("WHOIS: ERR: The send was incomplete.\n\r");
			ret = ERR_WHOIS_ERROR;
			break;
		default:
			//bwprintf("WHOIS: DEBUG: The send returned: %d\n\r", ret);
			break;
		}
		if (ret > 0) {
			if (reply.operation != NAMESERVER_OPERATION_WHOIS) {
				bwprintf("WHOIS: ERR: The response from the nameserver was invalid.\n\r");
				ret = ERR_WHOIS_ERROR;
			} else {
				if (reply.bufferLen != sizeof(int)) {
					bwprintf("WHOIS: ERR: The response from the nameserver contained invalid data.\n\r");
					ret = ERR_WHOIS_ERROR;
				} else {
					int value = (int)(*(int*)reply.buffer);
					if (value  > 0) { //valid tid
						//bwprintf("WHOIS: DEBUG: Tid is: %d.\n\r", value);
						ret = value;
					} else {
						bwprintf("WHOIS: WARN: Name not found.\n\r");
						ret = ERR_WHOIS_NOT_FOUND;
					}
				}
			}
		}
	}
	return ret;
}

int sys_whoIsNs(TaskDescriptor* active, KernelData* kData) {
	setTaskReady(active);
	return kData->nameserverTid;
}
