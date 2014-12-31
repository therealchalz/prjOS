/*
 * interrupts.c
 *
 *  Created on: Dec 31, 2014
 *      Author: che
 */

//#include <prjOS/include/base_tasks/serialDriver.h>
#include <stdint.h>
#include <stdbool.h>
//#include "prjOS/include/bwio.h"
#include "prjOS/include/hardware_dependent/cpu.h"
//#include "prjOS/include/hardware_dependent/cpu_defs.h"
//#include "prjOS/include/hardware_dependent/board.h"
//#include "prjOS/include/debug.h"
//#include "prjOS/include/scheduler.h"
//#include "prjOS/include/syscall.h"
//#include "prjOS/include/sys_syscall.h"
//#include "prjOS/include/kernel_data.h"
#include "string.h"
//#include "prjOS/include/base_tasks/nameserver.h"
//#include "inc/hw_nvic.h"
extern void USB0DeviceIntHandler(void);

void handleInterrupt(void) {
	//pull the current ISR number from IPSR
	uint32_t isr = cpuHelperGetIsr();
	switch (isr) {
	case 60:
		USB0DeviceIntHandler();
	}
}
