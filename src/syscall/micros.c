#include <prjOS/include/sys_syscall.h>

uint32_t prjGetMonotonicMicros(uint64_t* micros) {
	asm (svcArg(SYSCALL_MONOTONIC_MICROS));
	uint32_t ret;
	asm (" MOV %[ret], R0\n": [ret] "=r" (ret): :);
	return ret;
}

uint32_t sys_getMonotonicMicros(TaskDescriptor* active, KernelData* kData) {
	setTaskReady(active);

	uint64_t time = getSystemTimerMicros(kData);

	uint64_t* timeDest = (uint64_t*)active->systemCall.param1;

	*timeDest = time;
	//*timeDest = 123456789101112;

	return 0;
}
