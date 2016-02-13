	.data
str1: .asciiz "\nIntroduza um numero (sinal e módulo): "
str2: .asciiz "\nValor lido em base 2: "
str3: .asciiz "\nValor lido em base 16: "
str4: .asciiz "\nValor lido em base 10 (unsigned): "
str5: .asciiz "\nValor lido em base 10 (signed): "
	.text
	.globl main
main:
	li $v0, 8
	la $a0, str1
	syscall

	li $v0, 5
	syscall
	or $t0, $v0, $0

	li $v0, 8
	la $a0, str2
	syscall
	li $v0, 6 # printInt(value, 2);
	li $a1, 2
	or $a0, $t0, $0
	syscall

	li $v0, 8
	la $a0, str3
	syscall
	li $v0, 6 # printInt(value, 16);
	li $a1, 16
	or $a0, $t0, $0
	syscall

	li $v0, 8
	la $a0, str4
	syscall
	li $v0, 6 # printInt(value, 10);
	li $a1, 10
	or $a0, $t0, $0
	syscall

	li $v0, 8
	la $a0, str5
	syscall
	li $v0, 7 # printInt10(value);
	or $a0, $t0, $0
	syscall

	j main
	jr $ra