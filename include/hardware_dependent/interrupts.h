/*
 * interrupts.h
 *
 *  Created on: Jan 1, 2015
 *      Author: che
 */



#ifndef PRJOS_INCLUDE_HARDWARE_DEPENDENT_INTERRUPTS_H_
#define PRJOS_INCLUDE_HARDWARE_DEPENDENT_INTERRUPTS_H_

#include "inc/hw_ints.h"

#define INTERRUPT_USB0		INT_USB0
#define INTERRUPT_TIMER0A	INT_TIMER0A
#define INTERRUPT_SYSTICK	FAULT_SYSTICK
#define INTERRUPT_SYSCALL	FAULT_SVCALL


void handleInterrupt(uint32_t isrNumber);
void initInterrupts();

#endif /* PRJOS_INCLUDE_HARDWARE_DEPENDENT_INTERRUPTS_H_ */
