# analyze.S
# Written 2015 by F Lundevall
# Ported 2024/07 by W Szczerek (from MIPS to RISC-V)
# Copyright abandonded - this file is in the public domain.

	.text
	.globl analyze
analyze:
	li	s0, 0x30
loop:
	mv	a0, s0				# copy from s0 to a0
	
	li	a7, 11				# environment call with a7 = 11 will print out
	ecall					# one byte from a0 to the Run I/O window

	addi	s0, s0, 0x03	                #this line had to be changed , the value added with each iteration of the loop
	
	li	t0, 0x5A #Z	
	ble	s0, t0, loop
    	jr 	ra					
