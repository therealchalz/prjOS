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

#include <prjOS/include/sys_syscall.h>
#include <prjOS/include/base_tasks/nameserver.h>

uint32_t prjWhoIs (char *name) {
	task_id_t ret;
	if (strcmp(name,NAMESERVER_NAMESTR) == 0) {
		//Looking for NameServer - NOTE: Don't need arguments, so no big deal
		asm (svcArg(SYSCALL_WHOISNS));
		asm (" MOV %[ret], R0\n": [ret] "=r" (ret): :);
	} else {
		task_id_t nsTid = prjWhoIs(NAMESERVER_NAMESTR);
		if (nsTid <= 0)
			return ERR_WHOIS_WRONG_TID;
		//Construct query
		NameserverQuery query;
		NameserverQuery reply;

		query.senderTid = prjGetTid();
		query.operation = NAMESERVER_OPERATION_WHOIS;
		memcpy(query.buffer, name, strlen(name)+1);
		query.bufferLen = strlen(name)+1;

		ret = prjSend(nsTid, (uint8_t*)&query, sizeof(query), (uint8_t*)&reply, sizeof(reply));
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
				if (reply.bufferLen != sizeof(uint32_t)) {
					bwprintf("WHOIS: ERR: The response from the nameserver contained invalid data.\n\r");
					ret = ERR_WHOIS_ERROR;
				} else {
					uint32_t value = (uint32_t)(*(uint32_t*)reply.buffer);
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

task_id_t sys_whoIsNs(TaskDescriptor* active, KernelData* kData) {
	setTaskReady(active);
	return kData->nameserverTid;
}
