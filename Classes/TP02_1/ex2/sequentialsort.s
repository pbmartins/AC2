	.text
	.globl sequentialsort
sequentialsort:
	addiu $sp, $sp, -28
	sw $ra, 0($sp)
	sw $s0, 4($sp)
	sw $s1, 8($sp)
	sw $s2, 12($sp)
	sw $s3, 16($sp)
	sw $s4, 20($sp)
	sw $s5, 24($sp)

	or $s0, $a0, $0 # n_val
	or $s1, $a1, $0 # $array
	or $s2, $0, $0 # i
	or $s3, $0, $0 # j
	or $s4, $0, $0 # n_trocas
	addiu $s5, $s0, -1 # n_val - 1

externalfor:
	bge $s2, $s5, endexternalfor
	addiu $s3, $s2, 1

internalfor:
	bge $s3, $s0, endinternalfor
	sll $t0, $s2, 2
	addu $a0, $s1, $t0 # &array[i]
	lw $t0, 0($a0)

	sll $t1, $s3, 2
	addu $a1, $s1, $t1 # &array[j]
	lw $t1, 0($a1)

	bgeu $t1, $t0, endif
	jal troca
	addiu $s4, $s4, 1

endif:
	addiu $s3, $s3, 1
	j internalfor

endinternalfor:
	addiu $s2, $s2, 1
	j externalfor

endexternalfor:
	or $v0, $s4, $0
	lw $ra, 0($sp)
	lw $s0, 4($sp)
	lw $s1, 8($sp)
	lw $s2, 12($sp)
	lw $s3, 16($sp)
	lw $s4, 20($sp)
	lw $s5, 24($sp)
	addiu $sp, $sp, 28
	jr $ra
