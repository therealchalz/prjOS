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

typedef struct NameserverEntry {
	unsigned int tid;
	char name[NAMESERVER_MAX_NAME_LENGTH];
} NameserverEntry;

typedef struct NameserverQuery {
	char buffer[NAMESERVER_MAX_NAME_LENGTH];
	unsigned int bufferLen;
	unsigned int operation; //Possible values defined above
} NameserverQuery;

void nameserverEntry();

#endif /* NAMESERVER_H_ */
