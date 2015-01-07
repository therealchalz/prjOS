#-------------------------------------------------------------------------------
# Copyright (c) 2013 Charles Hache <chache@brood.ca>. All rights reserved. 
# 
# This file is part of the prjOS project.
# prjOS is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# prjOS is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with prjOS.  If not, see <http://www.gnu.org/licenses/>.
# 
# Contributors:
#     Charles Hache <chache@brood.ca> - initial API and implementation
#-------------------------------------------------------------------------------
/*
 * contextSwitch.asm
 *
 * For simplicity at this point, there are 3 functions defined in this file:
 * hwContextSwitch - Called by ISRs to be handled by the kernel
 * contextSwitch - Called by the SVC exception so tasks can switch to the kernel
 * kernelToTask - Called by the kernel to switch to the given task, regardless if the
 *  task was switched out by HW interrupt or by syscall.  In other words, kernelToTask
 *  is the complementary function to both contextSwitch and hwContextSwitch.
 */


	.thumb
	.syntax unified
	.section .text.hwContextSwitch
	.thumb_func
	.global hwContextSwitch

hwContextSwitch:
# Very similar to the contextSwitch below, but hacked a bit to allow
# for hardware preemption.  Ideally in the future the different functions
# would be merged but for now this is a quick solution.

# This hwContextSwitch function should be called by a hardware preemption timer as an ISR.
# The contextSwitch function further below should be called as an ISR to SVC calls or from
# going from kernel to thread mode (it will handle both HW and SVC thread context restoration 
# internally).
# The functions differ in that this one knows that we're premption a thread
# mid-processing, so doesn't do any system call parameter/return value handling.

# Note that this function is specifically only crafted for switching from thread
# mode to kernel mode for the task of task-switching preemption - it is not
# intended for general exception/interrupt handling.

		# Disable global interrupts
		CPSID		i

		LDR         R3, =0x1234abcd

		# Disable all 1 and lower interrupts
		MOV			R1, #0x20		/*binary 00100000, for NVIC with 3 bit priority*/
		MSR			BASEPRI, R1
		# Re-enable global interrupts
		CPSIE		i

        LDR         R3, =0x43210001

		# Get the task's SP
		MRS			R1, PSP

        LDR         R3, =0x43210002

		# Save task registers on the task's stack
		MOV			R2, SP
		MOV			SP, R1
		PUSH 		{R4-R11,LR}
		MOV			R1, SP
		MOV			SP, R2

        LDR         R3, =0x43210003

		# Get the TD location off the stack
		LDR			R0, [SP, #36]

        LDR         R3, =0x43210004

		# Save the new SP to the TD
		STR			R1, [R0, #8]

        LDR         R3, =0x43210005

		# Save the syscall arg (set it to 0 to indicate a HW interrupt)
		MOV			R2, #0
		STR			R2,	[R0, #12]

		# Set status to 8-HW Interrupted
		MOV                     R4, #8
        STR                     R4, [R0, #44]

       	LDR         R3, =0x43210006

		# Pop some of the kernel's registers (rest handled by exception return)
		POP			{R4-R11,LR}

		LDR         R3, =0x43210007

		# The TaskSwitch call returns the ISR number that triggered the task switch
		# NB: This mustn't return 0, otherwise the kernel will think that the task
		# made a syscall.
		POP			{R0}
		MRS			R0, IPSR

		LDR         R3, =0x43210008

		# If IPSR is 0, return -1 instead of IPSR
		MOV			R2, #0
		CMP			R0, R2
		IT			EQ
		MOVEQ		R0, #-1

		PUSH {R0}

		LDR         R3, =0x43210009

		# Done restoring kernel context - pops the remainder
		BX			LR


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

		# Disable global interrupts
		CPSID		i

		# Check if we're swapping from the kernel to a task or vice versa.
		# If were going kernel to task, we don't need to save our SP (because we always use MSP)
		# If were going task to kernel, we'll update the TD with the new SP

		MOV			R1, LR
		AND			R1, R1, #0x0F
		MOV			R2, #0x09 /* Check if the exception LR indicates we came from the kernel */
		CMP			R1, R2  /* On equal, we're going from kernel to task */
		BEQ			kernelToTask

taskToKernel:

		LDR         R3, =0x1234ab00

		# Disable all 1 and lower interrupts
		MOV			R1, #0x20		/*binary 00100000, for NVIC with 3 bit priority*/
		MSR			BASEPRI, R1
		# Re-enable global interrupts
		CPSIE		i

        LDR         R3, =0x12340060

		# Get the task's SP and keep in R1
		MRS			R1, PSP

        LDR         R3, =0x12340061
        # Get the kernel stack's SP and keep in R2
		MOV			R2, SP

		# Install the task's stack and save task registers on to it
        # Also get updated task SP and keep in R1
		MOV			SP, R1
		PUSH 		{R4-R11,LR}
		MOV			R1, SP

        # Return to kernel stack
		MOV			SP, R2

        LDR         R3, =0x12340062

		# Get the TD location off the stack and keep in R0
		LDR			R0, [SP, #36]

        LDR         R3, =0x12340063

		# Save the new task SP to the TD
		STR			R1, [R0, #8]

        LDR         R3, =0x12340064

		# Get the SVC call argument using the pre-exception PC of the task
		LDR			R2, [R1, #60]
		LDR			R2, [R2, #-2]
		AND			R2, R2, #255

        LDR         R3, =0x12340065

		# Save the syscall arg to the TD, clear the ret value
		STR			R2,	[R0, #12]
		MOV			R2, #0
		STR			R2, [R0, #16]

        LDR         R3, =0x12340066

		# Set the task state to be blocking on syscall
		MOV			R2, #7
		STR			R2, [R0, #44]

        LDR         R3, =0x12340067

		# Find the syscall parameters on the stack (saved from the
		# registers by the compiler/gcc)
		# R0
		LDR			R2, [R1, #36]
		STR			R2, [R0, #20]
		# R1, R2, R3
		LDR			R2, [R1, #40]
		STR			R2, [R0, #24]
		LDR			R2, [R1, #44]
		STR			R2, [R0, #28]
		LDR			R2, [R1, #48]
		STR			R2, [R0, #32]
		# Parameters 5 and 6, from the stack before the exception
		# main task SP = [R1, #68]
		# then compiler puts a 6 word frame on top,
		# and compilers pushes 2 words to save,
		# so p5 = [R0, #(68+8*4)]...
		LDR			R2, [R1, #100]
		STR			R2, [R0, #36]
		LDR			R2, [R1, #104]
		STR			R2, [R0, #40]

        LDR         R3, =0x12340068

		# Start popping some of the kernel's registers (rest handled by exception return)
        POP			{R4-R11,LR}

        # The TaskSwitch syscall always returns 0 when the task was interrupted
        # due to a syscall
        POP			{R0}
		MOV			R0, #0
		PUSH		{R0}

		LDR         R3, =0x12340069

		#CPSIE		i
		# Done restoring kernel context - pops the remainder
		BX			LR


	.thumb
	.syntax unified
	.section .text.kernelToTask
	.thumb_func
	.global	kernelToTask

# R0 = TD location (using R0 pushed on stack by exception mechanism)
kernelToTask:

		LDR         R3, =0x12340020

		# Recall, the stack will look like:
		# R4,R5,R6,R7,R8,R9,R10,R11,LR(EXC_RETURN),R0,R1,R2,R3,R12,LR(Pre exception),PC(Pre exception),xPSR(Pre exception)

		# Store the remaining kernel registers on MSP
		PUSH 		{R4-R11,LR}

        LDR         R3, =0x12340021

        # Keep Task Descriptor pointer in R0
        LDR			R0, [SP, #36]

		# Get SP of task and keep in R1
		LDR			R1, [R0, #8]	/* Task SP from TD */

        # Save the kernel SP in R2
        MOV			R2, SP

        LDR         R3, =0x12340022

		# Switch to the task's stack
        # In handler mode we can't switch to PSP, so switch MSP to task's PSP
        MOV			SP, R1

        # Check if task was preempted or did a syscall
		LDR			R4, [R0, #44]	/* Task state: 8 = hwinterrupted, anything else is irrelevant for us here */
		MOV			R5, #8
		CMP			R4, R5

		LDR         R3, =0x12340023

        # Pop some of the task's registers
		POP			{R4-R11,LR}

		LDR         R3, =0x12340024

        #At this point, R0=Task descriptor pointer, R1=Task SP, R2 = Kernel SP
		BNE			syscall_kernel_to_task

hw_kernel_to_task:

        # Nothing to do

		B finish_kernel_to_task;

syscall_kernel_to_task:

        #At this point, R0=Task descriptor pointer, R1=Task SP, R2 = Kernel SP, R4-R11&LR are restored from process

        #Replace the task's stack-stored R0 with the proper syscall return value
		# Here we pop 'R0' from the stack temporarily, then push on the return value
		POP			{R1}
		LDR			R1, [R0, #16]	/* Task syscall return value from TD */
		PUSH		{R1}

        #LDR         R3, =0x12340044

		B finish_kernel_to_task;

finish_kernel_to_task:

        LDR         R3, =0x12340025

		# Save PSP
		MOV			R1, SP
        MSR			PSP, R1			/* Set PSP to task SP */

        LDR         R3, =0x12340026

        # Restore MSP
        MOV			SP, R2

        LDR         R3, =0x12340027

        # Re-enable priority 1 and lower interrupts
        MOV			R2, #0
        MSR			BASEPRI, R2

		# Re-enable global interrupts
		CPSIE		i

		BX			LR	/* Exception return, pops the rest of the stack */

