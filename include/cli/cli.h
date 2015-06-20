/*
 * cli.h
 *
 *  Created on: Jun 18, 2015
 *      Author: che
 */

#ifndef PRJOS_INCLUDE_CLI_CLI_H_
#define PRJOS_INCLUDE_CLI_CLI_H_

typedef struct {
	uint32_t serialUITid;
	uint32_t serialDriverTid;
} CliInitData;

uint32_t cliInit(CliInitData *cliInitData);

#endif /* PRJOS_INCLUDE_CLI_CLI_H_ */
