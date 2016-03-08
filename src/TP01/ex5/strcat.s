	.text
	.globl strcat
strcat:
	addiu $sp, $sp, -8
	sw $ra, 0($sp)
	sw $s0, 4($sp)
	or $s0, $a0, $0

for:
	lb $t0, 0($a0)
	beq $t0, $0, endfor
	addiu $a0, $a0, 1
	j for

endfor:
	jal strcpy
	or $v0, $s0, $0
	lw $ra, 0($sp)
	lw $s0, 4($sp)
	jr $ra