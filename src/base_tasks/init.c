/*
 * task_init.c
 *
 *  Created on: Dec 30, 2014
 *      Author: che
 */


#include <prjOS/include/base_tasks/serialDriver.h>
#include <stdint.h>
#include <stdbool.h>
#include "prjOS/include/syscall.h"
#include "prjOS/include/task.h"
#include "prjOS/include/kernel_data.h"
#include "string.h"
#include "prjOS/include/bwio.h"
#include "prjOS/include/base_tasks/nameserver.h"
#include "prjOS/include/base_tasks/serialUI.h"

void initTask(void* firstTaskfn) {

	uint32_t tid;
	uint8_t dummy = 0;

	bwprintf("Init starting\n\r");
	tid = prjCreate(0, nameserverEntry);
	prjSend(tid, &dummy, 1, &dummy, 1);
	bwprintf("Init: Nameserver running\n\r");

	tid = prjCreate(0, serialDriverTask);
	prjSend(tid, &dummy, 1, &dummy, 1);
	bwprintf("Init: Serial driver running\n\r");

	tid = prjCreate(0, serialUITask);
	prjSend(tid, &dummy, 1, &dummy, 1);
	bwprintf("Init: Serial UI running\n\r");

	bwprintf("Init: Creating first task (0x%x)\n\r", firstTaskfn);
	prjCreate(0, firstTaskfn);

	prjChangePriority(6);

	while (1) {
		prjYield();
	}
	prjExit();
}
