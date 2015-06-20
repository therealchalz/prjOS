/*
 * cli.h
 *
 *  Created on: Jun 18, 2015
 *      Author: che
 */

#ifndef PRJOS_INCLUDE_CLI_CLI_H_
#define PRJOS_INCLUDE_CLI_CLI_H_

typedef struct {
	task_id_t serialUITid;
	task_id_t serialDriverTid;
} CliInitData;

task_id_t cliInit(CliInitData *cliInitData);

#endif /* PRJOS_INCLUDE_CLI_CLI_H_ */
