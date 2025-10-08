# timetemplate.S
# Written 2015 by F Lundevall
# Ported 2024/06 by W Szczerek (from MIPS to RISC-V)
# Copyright abandonded - this file is in the public domain.

#############################################################
# Choose the macro syntax for RARS or DTEK-V board.         #
# The syntax for RARS is probably due to its MIPS heritage. #
#############################################################
###################
# Macros for RARS #
###################
#.macro	PUSH (%reg)
#	addi	sp,sp,-4
#	sw	%reg,0(sp) 
#.end_macro

#.macro	POP (%reg)
#	lw	%reg,0(sp)
#	addi	sp,sp,4
#.end_macro
###################
# Macros for DTEK #
###################
.macro	PUSH reg
	addi sp,sp,-4
	sw \reg,0(sp) 
.endm

.macro	POP reg
	lw	\reg,0(sp)
	addi	sp,sp,4
.endm
#############################################################

	.data
	.align 2
mytime:	.word 	0x5957
timstr:	.asciz 	"text more text lots of text\0"
	.text
	.globl timetemplate, tick, time2string, display_string, delay

# Function for displaying a string with a newline at the end	
display_string:	
	li a7,4
	ecall
	li a0, 10
	li a7,11
	ecall
	jr ra
	
timetemplate:
	la	a0, timstr
	jal     display_string
	
	# wait a little
	li	a0, 1000	# ms
	jal	delay
	
	# call tick
	la 	a0, mytime
	jal	tick
	
	# call your function time2string
	la	a0, timstr
	la	t0, mytime
	lw	a1, 0(t0)
	jal	time2string
	
	# go back and do it all again
	j	timetemplate

	
# tick: update time pointed to by $a0
tick:	lw	t0, 0(a0)	# get time
	addi	t0, t0, 1	# increase
	andi	t1, t0, 0xf	# check lowest digit
	sltiu	t2, t1, 0xa	# if digit < a, okay
	bnez	t2, tiend
	addi	t0, t0, 0x6	# adjust lowest digit
	
	andi	t1, t0, 0xf0	# check next digit
	sltiu	t2, t1, 0x60	# if digit < 6, okay
	bnez	t2, tiend
	addi	t0, t0, 0xa0	# adjust digit
	
	li	t3, 0xF
	slli	t3, t3, 0x8
	and	t1, t0, t3	# check minute digit
	addi	t3, x0, 0xA
	slli	t3, t3, 0x8
	slt	t2, t1, t3	# if digit < a, okay
	bnez	t2, tiend
	addi	t0, t0, 0x600	# adjust digit - this one's okay, it's lower than 0x7FF 
	
	li	t3, 0xF
	slli	t3, t3, 0xC
	and	t1, t0, t3	# check last digit
	addi	t3, x0, 0x6
	slli	t3, t3, 0xC
	slt	t2, t1, t3	# if digit < 6, okay
	bnez	t2, tiend
	
	li	t3, 0xA
	slli	t3, t3, 0xC
	add	t0, t0, t3	# adjust last digit
tiend:	sw	t0,0(a0)	# save updated result
	jr	ra		# return

#########################################################
# Place for your functions: time2string, hex2asc, delay.#
#########################################################

hex2asc:
	li	a0, 4		# test number (from 0 to 15)
	
	addi    sp,sp,-4
	sw      ra,0(sp)
	
	jal	hexasc		# call hexasc
	
	li	a7, 11	# write a0 to stdout
	ecall

	lw      ra,0(sp)
	addi    sp,sp,4
	jr      ra
	
# You can write your own code for hexasc here
  #
hexasc:
  	andi a0, a0, 0xf  # does and with all 0s except for the four last bits, retaining the four last bits of a0 and making all the others 0
  	li t0, 0xA  # loads the value 10 in a temp register
  	bge a0, t0, letter  # a0 is greater than 9, jump to the letter label
  	addi a0,a0, 0x30 # adds 48 to get the right ascii value for numbers
  	j continue
letter:
  	addi a0,a0, 0x37 # ads 55 to get the right ascii value for letters
continue:
  	jr ra
  		
delay:
	#a0 = ms, t0 = i, t1 = 4711
	li t1, 10000
while:
	ble a0, zero, done
	addi a0, a0, -1
	addi t0, zero, 0x0 
for:
	bge t0, t1, while
	addi t0, t0, 0x1
	j for
	
done:
	jr ra
	
time2string: 
	PUSH s0
	PUSH s1
	PUSH ra
	
	mv s0, a0
	mv s1, a1
	
	# shifts bit 16-12 to 4-0, calculates the ascii value, and saves it
	srli a0,s1, 12
	jal hexasc
	sb a0, 0(s0)
	
	# shifts bit 12-8  to 4-0, calculates the ascii value, and saves it
	srli a0, s1, 8
	jal hexasc
	sb a0, 1(s0)
	
	#saves the ascii character for ':'
	addi t0, zero, 0x3A
	sb t0, 2(s0)
	
	#shifts bit 8-4 to 4-0, calculates the ascii value, and saves it
	srli a0, s1, 4
	jal hexasc
	sb a0, 3(s0)
	
	#no shifting needed the bytes are aleady in the correct position
	mv a0, s1
	jal hexasc
	sb a0, 4(s0)
	
	
	andi a0, s1,0x1
	beqz a0, even
	addi a0, zero, 13
	jal hexasc
	sb a0, 5(s0)
	j cont
	
even:
	addi a0, zero, 14
	jal hexasc
	sb a0, 5(s0)

cont:
	
	#saves the ascii character for the null terminator
	addi a0, zero, 0x0
	sb a0, 6(s0)
	
	POP ra
	POP s1
	POP s0
	
	jr ra
	