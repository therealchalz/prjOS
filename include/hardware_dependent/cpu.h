/*
 * cpu.h
 */

#ifndef CPU_H_
#define CPU_H_

#include <task.h>
#include <hardware_dependent/cpu_defs.h>

void cpuInit();
void cpuPrintInfo();
void cpuBarf();

int* cpuCreateTask(int* stackLocation, const TaskCreateParameters *parms);
void cpuSetupTaskDefaultParameters(TaskCpuCreateParameters *params, void* startFunction);

void contextSwitch(TaskDescriptor *t);

#endif /* CPU_H_ */
