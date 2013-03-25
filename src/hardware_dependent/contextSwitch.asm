/*
 * contextSwitch.asm
 */


	.thumb
	.syntax unified
	.section .text.contextSwitch
	.thumb_func
	.global	contextSwitch


contextSwitch:
#* --------------------------------------------------------------------------*

		# The exception mechanism did this for us:
		# STMFD		SP!, {R0-R3,R12,LR,PC,xPSR}
		# and the exception mechanism will automatically revserse this for us
		# when we return.  NOTE it also saves the floating point context
		# if required (and restores it too).

		#Now we've got to save off all the CPU state.  The stack will look like:
		#R4,R5,R6,R7,R8,R9,R10,R11,LR(EXC_RETURN),R0,R1,R2,R3,R12,LR(Pre exception),PC(Pre exception),xPSR(Pre exception)

		# Check if we're swapping from the kernel to a task or vice versa.
		# If were going kernel to task, we don't need to save our SP (because we always use MSP)
		# If were going task to kernel, we'll update the TD with the new SP
		MOV			R1, LR
		AND			R1, R1, #0x0F
		MOV			R2, #0x09
		CMP			R1, R2	/* On equal, we're going from kernel to task */
		BNE			taskToKernel

# Kernel to Task:
		# Store kernel registers on MSP
		PUSH 		{R4-R11,LR}

		# Get SP of task
		LDR			R1, [R0, #8]	/* Task SP from TD */
		LDR			R0, [R0, #12]	/* Task syscall return value from TD */

		# Switch to the task's stack
        # In handler mode we can't switch to PSP, so switch MSP to PSP
        MOV			R2, SP
        MOV			SP, R1

		# Pop some of the task's registers
        POP			{R4-R11,LR}

        # Here we pop R0 temporarily, then push on the return value
        POP			{R1}
        PUSH		{R0}

		# Save PSP
		MOV			R1, SP
        MSR			PSP, R1			/* Set PSP to task SP */

        # Restore MSP
        MOV			SP, R2


        B			popTheRest

taskToKernel:
		# Get the task's SP
		MRS			R1, PSP

		# Save task registers on the task's stack
		MOV			R2, SP
		MOV			SP, R1
		PUSH 		{R4-R11,LR}
		MOV			R1, SP
		MOV			SP, R2

		# Get the TD location off the stack
		LDR			R0, [SP, #36]

        # Save the new SP to the TD
		STR			R1, [R0, #8]

		# The TaskSwitch syscall always returns 0
		MOV			R0, #0

		# Pop some of the kernel's registers
        POP			{R4-R11,LR}

popTheRest:

		#PUSH		{LR}
		#MOV			R0, LR
		#BL			cpuBarf
		#MRS			R0, PSP
		#MOV			R1, #8
		#BL 			prettyPrintMemoryWords
		#POP			{LR}

#loopMe:
		#B 			loopMe

		#MRS			R0, PSP
		#LDR			R1, [R0, #24]
		#BX			R1

        BX			LR	/* Exception return, pops the rest of the stack */

