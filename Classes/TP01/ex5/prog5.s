	.equ STR_MAX_SIZE, 20
	.equ STR_MAX_SIZE1, 21
	.equ STR_MAX_SIZE21, 41
	.data
str1: .space STR_MAX_SIZE1
str2: .space STR_MAX_SIZE1
str3: .space STR_MAX_SIZE21
pstr1: .asciiz "Introduza 2 strings: "
pstr2: .asciiz "\nResultado "
	.text
	.globl main
main:
	addiu $sp, $sp, -4
	sw $ra, 0($sp)
	# printStr("Introduza 2 strings: ");
	li $v0, 8
	la $a0, pstr1
	syscall
	# readStr( str1, STR_MAX_SIZE );
	li $v0, 9
	la $a0, str1
	ori $a1, $0, STR_MAX_SIZE # ?
	syscall
	# readStr( str2, STR_MAX_SIZE );
	li $v0, 9
	la $a0, str2
	ori $a1, $0, STR_MAX_SIZE # ?
	syscall
	# printStr("Resultados:\n");
	li $v0, 8
	la $a0, pstr2
	syscall
	# prinInt( strlen(str1), 10 );
	la $a0, str1
	jal strlen
	or $a0, $v0, $0
	li $a1, 10
	li $v0, 6
	syscall
	# prinInt( strlen(str2), 10 );
	la $a0, str2
	jal strlen
	or $a0, $v0, $0
	li $a1, 10
	li $v0, 6
	syscall
	# strcpy(str3, str1);
	la $a0, str3
	la $a1, str1
	jal strcpy
	# printStr( strcat(str3, str2) );
	la $a0, str3
	la $a1, str2
	jal strcat
	or $a0, $v0, $0
	li $v0, 8
	syscall
	# printInt10( strcmp(str1, str2) );
	la $a0, str1
	la $a1, str2
	jal strcmp
	or $a0, $v0, $0
	li $v0, 7
	syscall
	# return 0;
	li $v0, 0
	lw $ra, 0($sp)
	addiu $sp, $sp, 4
	jr $ra
