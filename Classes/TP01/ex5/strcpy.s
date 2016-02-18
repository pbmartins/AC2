	.text
	.globl strcpy
strcpy:
	or $v0, $a0, $0

for:
	lb $t0, 0($a1)
	sb $t0, 0($a0)
	beq $t0, $0, endfor
	addiu $a0, $a0, 1
	addiu $a1, $a1, 1
	j for

endfor:
	jr $ra