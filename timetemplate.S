
	.text
	.globl delay
	
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
	
