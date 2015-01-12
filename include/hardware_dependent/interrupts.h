/*
 * interrupts.h
 *
 *  Created on: Jan 1, 2015
 *      Author: che
 */



#ifndef PRJOS_INCLUDE_HARDWARE_DEPENDENT_INTERRUPTS_H_
#define PRJOS_INCLUDE_HARDWARE_DEPENDENT_INTERRUPTS_H_

#include "prjOS/include/kernel_data.h"
#include "inc/hw_ints.h"

void handleInterrupt(KernelData* kData, uint32_t isrNumber);
void initInterrupts();

#endif /* PRJOS_INCLUDE_HARDWARE_DEPENDENT_INTERRUPTS_H_ */
