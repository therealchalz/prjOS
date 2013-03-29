
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
 * cpu_barf.asm
 */

	.thumb
	.syntax unified
	.thumb_func
	.section .text.cpuBarf
	.global	cpuBarf


cpuBarf:
		/* Store all the registers before we trash them */
		PUSH		{r0-r12,lr}

		/* R0 */
		ADR       	R0, $C$SL1
		LDR			R1, [SP]
		BL        	bwprintf
		/* R1 */
		ADR       	R0, $C$SL2
		LDR			R1, [SP, #4]
		BL        	bwprintf
		/* R2 */
		ADR       	R0, $C$SL3
		LDR			R1, [SP, #8]
		BL        	bwprintf
		/* R3 */
		ADR       	R0, $C$SL4
		LDR			R1, [SP, #12]
		BL        	bwprintf
		/* R4 */
		ADR       	R0, $C$SL5
		LDR			R1, [SP, #16]
		BL        	bwprintf
		/* R5 */
		ADR       	R0, $C$SL6
		LDR			R1, [SP, #20]
		BL        	bwprintf
		/* R6 */
		ADR       	R0, $C$SL7
		LDR			R1, [SP, #24]
		BL        	bwprintf
		/* R7 */
		ADR       	R0, $C$SL8
		LDR			R1, [SP, #28]
		BL        	bwprintf
		/* R8 */
		ADR       	R0, $C$SL9
		LDR			R1, [SP, #32]
		BL        	bwprintf
		/* R9 */
		ADR       	R0, $C$SL10
		LDR			R1, [SP, #36]
		BL        	bwprintf
		/* R10 */
		ADR       	R0, $C$SL11
		LDR			R1, [SP, #40]
		BL        	bwprintf
		/* R11 */
		ADR       	R0, $C$SL12
		LDR			R1, [SP, #44]
		BL        	bwprintf
		/* R12 */
		ADR       	R0, $C$SL13
		LDR			R1, [SP, #48]
		BL        	bwprintf
		/* SP */
		ADR       	R0, $C$SL14
		MOV			R1, SP
		ADD			R1, R1, #54
		BL        	bwprintf
		/* LR */
		ADR       	R0, $C$SL15
		LDR			R1, [SP, #52]
		BL        	bwprintf

		/* PSR */
		ADR       	R0, $C$SL16
		MRS			R1, IAPSR
		MRS			R2, EPSR
		ORR			R1, R2, R1
		BL        	bwprintf
		/* CONTROL */
		ADR       	R0, $C$SL17
		MRS			R1, CONTROL
		BL        	bwprintf
		/* ISR Number */
		ADR       	R0, $C$SL18
		MRS			R1, IAPSR
		AND			R1, R1, #0xFF
		BL        	bwprintf
		/* Floating point context active */
		ADR       	R0, $C$SL19
		MRS			R1, CONTROL
		AND			R1, R1, #4
		LSR			R1, R1, #2
		BL        	bwprintf
		/* Using PSP */
		ADR       	R0, $C$SL20
		MRS			R1, CONTROL
		AND			R1, R1, #2
		LSR			R1, R1, #1
		BL        	bwprintf
		/* PSP */
		ADR       	R0, $C$SL21
		MRS			R1, PSP
		BL        	bwprintf
		/* MSP */
		ADR       	R0, $C$SL22
		MRS			R1, MSP
		BL        	bwprintf
		/* Unprivileged mode */
		ADR       	R0, $C$SL23
		MRS			R1, CONTROL
		AND			R1, R1, #1
		BL        	bwprintf

		LDMFD		SP!, {R0-R12,LR}
        BX		LR

	.align	4
C$SL1:	.string	"R0: 0x%08X\r\n"
	.align	4
C$SL2:	.string	"R1: 0x%08X\r\n"
	.align	4
C$SL3:	.string	"R2: 0x%08X\r\n"
	.align	4
C$SL4:	.string	"R3: 0x%08X\r\n"
	.align	4
C$SL5:	.string	"R4: 0x%08X\r\n"
	.align	4
C$SL6:	.string	"R5: 0x%08X\r\n"
	.align	4
C$SL7:	.string	"R6: 0x%08X\r\n"
	.align	4
C$SL8:	.string	"R7: 0x%08X\r\n"
	.align	4
C$SL9:	.string	"R8: 0x%08X\r\n"
	.align	4
C$SL10:	.string	"R9: 0x%08X\r\n"
	.align	4
C$SL11:	.string	"R10: 0x%08X\r\n"
	.align	4
C$SL12:	.string	"R11: 0x%08X\r\n"
	.align	4
C$SL13:	.string	"R12: 0x%08X\r\n"
	.align	4
C$SL14:	.string	"SP: 0x%08X\r\n"
	.align	4
C$SL15:	.string	"LR(PC): 0x%08X\r\n"
	.align	4
C$SL16:	.string	"PSR: 0x%08X\r\n"
	.align	4
C$SL17:	.string	"CONTROL: 0x%08X\r\n"
	.align	4
C$SL18:	.string	"ISR Number: 0x%08X\r\n"
	.align	4
C$SL19:	.string	"Floating point context active: %d\r\n"
	.align	4
C$SL20:	.string	"Using PSP: %d\r\n"
	.align	4
C$SL21:	.string	"PSP: %08X\r\n"
	.align	4
C$SL22:	.string	"MSP: %08X\r\n"
	.align	4
C$SL23:	.string	"Unprivileged mode: %d\r\n"


