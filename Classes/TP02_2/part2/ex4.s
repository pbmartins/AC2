    .equ SFR_BASE_HI, 0xBF88
    .equ TRISE, 0x6100
    .equ PORTE, 0x6110
    .equ LATE, 0x6120
    .equ TIME, 5000000
    .equ JOHNSON_SIZE, 8
    .equ JOHNSON_SIZE_1, 7
    .data
johnson:  .word 0x00, 0x01, 0x03, 0x07, 0x0F, 0x0E, 0x0C, 0x08
i:  .word 0
    .text
    .globl main
main:
	addiu $sp, $sp, -16
	sw $ra, 0($sp)
	sw $s0, 4($sp)
	sw $s1, 8($sp)
	sw $s2, 12($sp) # previous value

    lui $s0, SFR_BASE_HI
    lw $t0, TRISE($s0)
    andi $t0, $t0, 0xFFF0
    ori $t0, $t0, 0x0060
    sw $t0, TRISE($s0)

loop:
	# Get the current values of R6 and R7
    lw $s1, PORTE($s0)
    andi $s1, $s1, 0x00C0
    srl $t0, $s1, 6
    andi $t1, $t0, 0x0003
    andi $t0, $t0, 0x0001 			# $t0 = R6
    srl $t1, $t1, 1 				# $t1 = R7

    li $v0, 11						# Get current timer
    syscall
    ble $v0, TIME, loop				# if (readCoreTimer() > 5000000) -> update timer

    or $a0, $0, $0					# reset = 0
    beq $t0, $s2, endmainresetif	# if (R6_previousValue != R6)
    ori $a0, $0, 1					# reset = 1

endmainresetif:
	or $s2, $t0, $0 				# save previous R6 value on #s4
    ori $a1, $0, 1					# increment = 1
    beq $t1, $0, endincrementif		# if (R7bit == 1)
    addi $a1, $0, -1				# increment = -1

endincrementif:
    beq $t0, 1, elsecounter			# if (R6bit == 0)
    jal binarycounter				# binarycounter(reset, increment)
    j endcounterif

elsecounter:
	jal johnsoncounter				# johnsoncounter(reset, increment)

endcounterif:
	# Update value of R0, R1, R2, R3
	lw $t0, LATE($s0)
	andi $t0, $t0, 0xFFF0
	or $t0, $v0, $t0
	sw $t0, LATE($s0)

	li $v0, 12						# resetCoreTimer();
	syscall
	j loop

#########################
#### int binarycounter (int reset, int increment)
#########################
binarycounter:
	la $t0, i						# $t0 = &i
	bne $a0, 1, endbinaryresetif 	# if (reset = 1)
	sw $0, 0($t0)					# i = 0;
	or $v0, $0, $0					# return 0;
	jr $ra

endbinaryresetif:
    lw $t1, 0($t0)
	add $v0, $t1, $a1				# int to_return = i + increment;
	andi $v0, $v0, 0xFFFF			# to_return = to_return & 0xFFFF;
	sw $v0, 0($t0)					# i = to_return;
	jr $ra							# return i;


#########################
#### int johnsoncounter (int reset, int increment)
#########################
johnsoncounter:
	la $t0, i						# $t0 = &i
	la $t1, johnson 				# $t1 = &johnson
	bne $a0, 1, endjohnsonresetif 			# if (reset == 1)

resetif:
	or $t2, $0, $0					# i = 0;
	j endzeroif

endjohnsonresetif:
	lw $t2, 0($t0)					# $t2 = i;
	add $t2, $t2, $a1				# i += increment;
	bge $t2, JOHNSON_SIZE, resetif	# if (i > JOHNSON_SIZE) return johnson[0];
	bge $t2, $0, endzeroif			# if (i < 0) return johnson[JOHNSON_SIZE - 1];
	ori $t2, $0, JOHNSON_SIZE_1		# i = JOHNSON_SIZE - 1

endzeroif:
	sw $t2, 0($t0)
	sll $t2, $t2, 2
	add $t2, $t1, $t2
	lw $v0, 0($t2)					# return johnson[i];
	jr $ra
