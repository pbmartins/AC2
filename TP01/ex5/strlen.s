	.text
	.globl strlen
strlen:
	or $v0, $0, $0

for:
	lb $t0, 0($a0)
	beq $t0, $0, endfor
	addi $v0, $v0, 1
	addiu $a0, $a0, 1

endfor:
	jr $ra