	.data
str: .asciiz "\r\n"
	.text
	.globl main
main:
	addiu $sp, $sp, -8
	sw $ra, 0($sp)
	sw $s0, 4($sp)

	ori $t0, $0, 10						# $s0 = 10 + (10 << 16)
	sll $t0, $t0, 16
	addiu $s0, $t0, 10

while:									# while(1)
	li $v0, 12							# resetCoreTimer();
	syscall
	li $a0, 1							# delay(1);
	jal delay

	li $v0, 11							# readCoreTimer();
	syscall
	or $a0, $v0, $0						# printInt(readCoreTimer(), 10 + (10 << 16));
	or $a1, $s0, $0
	li $v0, 6
	syscall

	li $v0, 8							# printStr("\r\n");
	la $a0, str
	syscall
	j while

	addiu $sp, $sp, 8
	lw $ra, 0($sp)
	lw $s0, 4($sp)
	jr $ra