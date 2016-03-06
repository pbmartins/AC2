	.equ N_INT, 5
	.equ TRUE, 1
	.equ FALSE, 0
	.data
lista: .word N_INT
str1: .asciiz "\nLeitura e ordenacao de inteiros em base 10\n"
str2: .asciiz "Introduza 5 Inteiros: "
str3: .asciiz "\nNumero de trocas realizado: "
str4: .asciiz "\nResultado da ordenacao: "
	.text
	.globl main
main:
	addiu $sp, $sp, -12
	sw $ra, 0($sp)
	sw $s0, 4($sp)
	sw $s1, 8($sp)

	la $s0, lista
	or $s1, $0, $0 # n_trocas

	ori $v0, $0, 8
	la $a0, str1
	syscall
	ori $v0, $0, 8
	la $a0, str2
	syscall

	or $t0, $0, $0

firstfor:
	bge $t0, N_INT, endfirstfor
	li $v0, 5
	syscall
	sll $t1, $t0, 2
	addu $t1, $s0, $t1
	sw $v0, 0($t1)

	li $v0, 3
	ori $a0, $0, ' '
	syscall

	addiu $t0, $t0, 1
	j firstfor

endfirstfor:
	ori $a0, $0, N_INT
	or $a1, $s0, $0
	jal sequentialsort
	or $s1, $v0, $0

	ori $v0, $0, 8
	la $a0, str3
	syscall
	ori $v0, $0, 7
	or $a0, $s1, $0
	syscall

	ori $v0, $0, 8
	la $a0, str4
	syscall

	or $t0, $s0, $0
    ori $t1, $0, N_INT
    sll $t1, $t1, 2
	addu $t1, $s0, $t1

secondfor:
	bge $t0, $t1, endsecondfor
	lw $a0, 0($t0)
	li $v0, 7
	syscall

	li $v0, 3
	ori $a0, $0, ' '
	syscall

	addiu $t0, $t0, 4
	j secondfor

endsecondfor:
	lw $ra, 0($sp)
	lw $s0, 4($sp)
	lw $s1, 8($sp)
	addiu $sp, $sp, 12
	jr $ra
