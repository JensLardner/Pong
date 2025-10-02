  # hexmain.S
  # Written 2015-09-04 by F Lundevall
  # Copyright abandonded - this file is in the public domain.

	.text
	.globl hex2asc

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
  	addi a0, a0, 0x30 # adds 48 to get the right ascii value for numbers
  	j continue
letter:
  	addi a0,a0, 0x37 # ads 55 to get the right ascii value for letters
continue:
  	jr ra
  
