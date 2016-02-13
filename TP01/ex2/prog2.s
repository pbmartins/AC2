	.data
str: .asciiz "Key pressed\n"
	.text
	.globl main
main:
	li $v0, 1 # (c = inkey())
	syscall
	or $t0, $v0, $0
	beq $t0, $0, main # while ((c = inkey()) == 0);
	bne $t0, '\n', printstr # if (c == '\n')
	j end # break

printstr:
	li $v0, 8
	la $a0, str
	syscall
	j main

end:
	li $v0, 0
	jr $ra