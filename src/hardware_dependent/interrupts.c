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
#include "prjOS/include/hardware_dependent/interrupts.h"
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

void handleInterrupt(uint32_t isrNumber) {

	//isrNumber = cpuHelperGetIsr();

	switch (isrNumber) {
	case INTERRUPT_USB0:
		USB0DeviceIntHandler();
	}
}


void initInterrupts() {
	//Lower ALL used interrupt sources to 1 or lower, but leave
	//the SVC interrupt at 0.
	IntPrioritySet(INTERRUPT_USB0, 1 << (8-NUM_PRIORITY_BITS));
	IntPrioritySet(INTERRUPT_TIMER0A, 1 << (8-NUM_PRIORITY_BITS));
	IntPrioritySet(INTERRUPT_SYSTICK, 1 << (8-NUM_PRIORITY_BITS));

	IntPrioritySet(INTERRUPT_SYSCALL, 0 << (8-NUM_PRIORITY_BITS));
}
