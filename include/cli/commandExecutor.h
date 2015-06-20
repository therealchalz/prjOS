/*
 * commandExecutor.h
 *
 *  Created on: Jun 18, 2015
 *      Author: che
 */

#ifndef PRJOS_INCLUDE_CLI_COMMANDEXECUTOR_H_
#define PRJOS_INCLUDE_CLI_COMMANDEXECUTOR_H_

#include "prjOS/include/cli/baseCli.h"
#include "prjOS/include/cli/cli.h"
#include "prjOS/include/prjOS.h"
#include "prjOS/include/cli/serialUI.h"
#include "prjOS/include/base_tasks/nameserver.h"
#include "prjOS/include/hardware_dependent/board.h"

typedef struct {
	uint32_t serialDriverTid;
	uint32_t serialUITid;
} CommandExecutorData;

#endif /* PRJOS_INCLUDE_CLI_COMMANDEXECUTOR_H_ */
