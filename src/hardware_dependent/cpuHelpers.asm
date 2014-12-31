
.thumb
	.syntax unified
	.section .text.cpuHelperGetIsr
	.thumb_func
	.global cpuHelperGetIsr

cpuHelperGetIsr:
	MRS		R0, IPSR
	BX		lr
