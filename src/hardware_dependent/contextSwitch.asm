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
		# when we return.  NOTE it also saves the floating point context if
		# required (and restores it too).

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
		LDR			R0, [R0, #16]	/* Task syscall return value from TD */

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

		# Get the SVC call argument.
		LDR			R2, [R1, #60]
		LDR			R2, [R2, #-2]
		AND			R2, R2, #255

		# Save the syscall arg, clear the ret value and handled flag
		STR			R2,	[R0, #12]
		MOV			R3, #0
		STR			R3, [R0, #16]
		STR			R3, [R0, #20]

		# If the SVC call arg is 0 it means hardware interrupt,
		# otherwise it's a syscall and we should save the arguments.
		CMP			R2, R3
		BNE			syscall
# Hardware Interrupt
		# Do nothing...
syscall:
		# Find the syscall parameters on the stack (saved from the
		# registers by the exception)
		# R0
		LDR			R2, [R1, #36]
		STR			R2, [R0, #24]
		# R1, R2, R3
		LDR			R2, [R1, #40]
		STR			R2, [R0, #28]
		LDR			R2, [R1, #44]
		STR			R2, [R0, #32]
		LDR			R2, [R1, #48]
		STR			R2, [R0, #36]
		# Parameters 5 and 6, from the stack before the exception
		# main task SP = [R0, #68]
		# then compiler puts a 6 word frame on top,
		# and compilers pushes 2 words to save,
		# so p5 = [R0, #(68+8*4)]...
		LDR			R2, [R1, #100]
		STR			R2, [R0, #40]
		LDR			R2, [R1, #104]
		STR			R2, [R0, #44]

		# The TaskSwitch syscall always returns 0
		MOV			R0, #0

		# Pop some of the kernel's registers (rest handled by exception return)
        POP			{R4-R11,LR}

popTheRest:

        BX			LR	/* Exception return, pops the rest of the stack */

