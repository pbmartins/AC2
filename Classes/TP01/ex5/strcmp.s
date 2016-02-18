	.text
	.globl strcmp
strcmp:
	lb $t0, 0($a0)
	lb $t1, 0($a1)
	bne $t0, $t1, endfor
	beq $t0, $0, endfor
	addiu $a0, $a0, 1
	addiu $a1, $a1, 1
	j strcmp

endfor:
	sub $v0, $t0, $t1
	jr $ra
