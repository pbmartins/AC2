	.text
	.globl main
main:
	li $v0, 2
	syscall
	or $t0, $v0, $0
	bne $t0, '\n', putchar
	j end

putchar:
	li $v0, 3
	or $a0, $t0, $0
	syscall
	j main

end:
	li $v0, 1
	jr $ra