/*
 * task_init.c
 *
 *  Created on: Dec 30, 2014
 *      Author: che
 */


#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <prjOS/include/syscall.h>
#include <prjOS/include/task.h>
#include <prjOS/include/kernel_data.h>
#include <prjOS/include/bwio.h>
#include <prjOS/include/cli/serialUI.h>
#include <prjOS/include/base_tasks/serialDriver.h>
#include <prjOS/include/base_tasks/nameserver.h>

void initTask(void* firstTaskfn) {

	task_id_t tid;
	uint8_t dummy = 0;

	bwprintf("Init starting\n\r");
	tid = prjCreate(0, nameserverEntry);
	prjSend(tid, &dummy, 1, &dummy, 1);
	bwprintf("Init: Nameserver running (%d)\n\r", tid);

	bwprintf("Init: Creating first task (0x%x)\n\r", firstTaskfn);
	prjCreate(0, firstTaskfn);

	prjChangePriority(6);

	while (1) {
		prjYield();
	}
	prjExit();
}
