/*
 * interrupts.c
 *
 *  Created on: Dec 31, 2014
 *      Author: che
 */

#include <prjOS/include/hardware_dependent/interrupts.h>

static volatile uint64_t* microTickCounterPtr;

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

void systemTimerISR(void) {
	MAP_TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	(*microTickCounterPtr)+= (1000ll*1000ll) / KERNEL_SYSTEM_TICKS_PER_SECOND;
}

void enableSystemTimer(void) {
	MAP_TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
}

void disableSystemTimer(void) {
	MAP_TimerIntDisable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
}

void initInterrupts(KernelData *kernelData) {
	MAP_IntMasterDisable();

	microTickCounterPtr = &(kernelData->systemMicroCount);
	*microTickCounterPtr = 0;

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
