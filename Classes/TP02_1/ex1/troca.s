	.text
	.globl troca
troca:
	lw $t0, 0($a0)
	lw $t1, 0($a1)
	sw $t1, 0($a0)
	sw $t0, 0($a1)
	jr $ra
