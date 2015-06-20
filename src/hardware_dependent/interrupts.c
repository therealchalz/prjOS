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
#include "prjOS/include/syscall.h"
#include "prjOS/include/sys_syscall.h"
#include "prjOS/include/kernel_data.h"
#include "string.h"
//#include "prjOS/include/base_tasks/nameserver.h"
//#include "inc/hw_nvic.h"
//extern void USB0DeviceIntHandler(void);

void handleInterrupt(KernelData* kData, uint32_t isrNumber) {
	switch (isrNumber) {
	case INT_USB0:
		//USB0DeviceIntHandler();
		sys_eventHappened(kData, EVENTID_USB0, 0);
		break;
	case INT_UART0:
		{
			uint32_t interrupts= MAP_UARTIntStatus(UART0_BASE, 1);
			MAP_UARTIntClear(UART0_BASE, interrupts);
			sys_eventHappened(kData, EVENTID_UART0, 0);
			break;
		}
	}

}

void initInterrupts() {
	MAP_IntMasterDisable();

	MAP_IntPriorityMaskSet( 1 << (8-NUM_PRIORITY_BITS));

	//Lower ALL used interrupt sources to 1 or lower, but leave
	//the SVC interrupt at 0.
	MAP_IntPrioritySet(INT_USB0, 1 << (8-NUM_PRIORITY_BITS));
	MAP_IntPrioritySet(INT_UART0, 1 << (8-NUM_PRIORITY_BITS));
	MAP_IntPrioritySet(INT_TIMER0A, 1 << (8-NUM_PRIORITY_BITS));
	MAP_IntPrioritySet(FAULT_SYSTICK, 1 << (8-NUM_PRIORITY_BITS));
	MAP_IntPrioritySet(FAULT_SVCALL, 0 << (8-NUM_PRIORITY_BITS));

	MAP_IntMasterEnable();
}
