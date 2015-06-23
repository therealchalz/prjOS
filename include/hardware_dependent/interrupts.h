/*
 * interrupts.h
 *
 *  Created on: Jan 1, 2015
 *      Author: che
 */



#ifndef PRJOS_INCLUDE_HARDWARE_DEPENDENT_INTERRUPTS_H_
#define PRJOS_INCLUDE_HARDWARE_DEPENDENT_INTERRUPTS_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <prjOS/include/hardware_dependent/cpu.h>
#include <prjOS/include/syscall.h>
#include <prjOS/include/sys_syscall.h>
#include <prjOS/include/kernel_data.h>
#include <prjOS/include/kernel_data.h>
#include <inc/hw_ints.h>

void handleInterrupt(KernelData* kData, uint32_t isrNumber);
void initInterrupts(KernelData *kernelData);
void enableSystemTimer(void);
void disableSystemTimer(void);

#endif /* PRJOS_INCLUDE_HARDWARE_DEPENDENT_INTERRUPTS_H_ */
