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
 * nameserver.h
 */

#ifndef NAMESERVER_H_
#define NAMESERVER_H_

#define NAMESERVER_NAMESTR			"NameServer"
#define NAMESERVER_MAX_NAMES		16
#define NAMESERVER_MAX_NAME_LENGTH	32

#define NAMESERVER_OPERATION_INVALID	0
#define NAMESERVER_OPERATION_REGISTER	1
#define NAMESERVER_OPERATION_WHOIS		2
#define NAMESERVER_OPERATION_EXIT		3
#define NAMESERVER_OPERATION_NO_SPACE	4
#define NAMESERVER_OPERATION_TOO_LONG	5
#define NAMESERVER_OPERATION_ERROR		6

//Names consolidated here for system tasks
#define NAMESERVER_NAME_SERIAL_DRIVER	"serialdriver"

typedef struct NameserverEntry {
	unsigned int tid;
	char name[NAMESERVER_MAX_NAME_LENGTH];
} NameserverEntry;

typedef struct NameserverQuery {
	unsigned int senderTid;
	char buffer[NAMESERVER_MAX_NAME_LENGTH];
	unsigned int bufferLen;
	unsigned int operation; //Possible values defined above
} NameserverQuery;

void nameserverEntry();

#endif /* NAMESERVER_H_ */
