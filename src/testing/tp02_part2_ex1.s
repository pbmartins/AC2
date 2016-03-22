    .equ SFR_BASE, 0xBF88
    .equ TRISE, 0x6100
    .equ PORTE, 0x6110
    .equ LATE, 0x6120
    .equ TIME, 5000000
    .equ JOHNSON_SIZE, 8
    .data
johnson: .word 0x00, 0x01, 0x03, 0x07, 0x0F, 0x0E, 0x0C, 0x08
counter: .word 0
    .text
    .globl main
main:
    # Save variables on stack
    addiu $sp, $sp, -12
    sw $ra, 0($sp)
    sw $s0, 4($sp)
    sw $s1, 8($sp)                              # previous value of RE4

    # Configure R0-3 as outputs and R4-5 as inputs
    lui $s0, SFR_BASE
    lw $t0, TRISE($s0)
    andi $t0, $t0, 0xF0
    ori $t0, $t0, 0x030
    sw $t0, TRISE($s0)

# Start infinite loop while it's reading values from R4-5
loop:
    # Get values of R4 and R5
    lw $t0, PORTE($s0)
    srl $t0, $t0, 4
    andi $t0, $t0, 0x03
    srl $t1, $t0, 1                             # Current value of RE5
    andi $t0, $t0, 0x01                         # Current value of RE4

    # There's only need to update values when the counter is above 5M of value
    li $v0, 11
    syscall
    ble $v0, TIME, loop                         # if (readCoreTimer() > 5000000) -> update timer

    or $a0, $0, $0                              # reset = 0
    beq $t0, $s1, endif1                        # if (previousRE4 != currentRE4)
    ori $a0, $0, 1                              # reset == 1

endif1:
    or $s1, $t0, $0                             # Save currentRE4 value on previousRE4 variable
    ori $a1, $0, 1                              # increment = 1
    beq $t1, $0, endif2                         # if (RE5 == 1)
    addi $a1, $0, -1                            # increment = -1

endif2:
    beq $t0, 1, else                            # if (currentRE4 == 0)
    jal binarycounter                           # binaryCounter(reset, increment);
    j endif3

else:
    jal johnsoncounter                          # johnsonCounter(reset, increment);

endif3:
    # Update RE0-RE4 values
    lw $t0, LATE($s0)
    andi $t0, $t0, 0xF0
    or $t0, $t0, $v0
    sw $t0, LATE($s0)

    # Reset core timer
    li $v0, 12
    syscall
    j loop

endloop:
    lw $ra, 0($sp)
    lw $s0, 4($sp)
    lw $s1, 8($sp)
    addiu $sp, $sp, 12
    jr $ra

######################################################
#### int binarycounter (int reset, int increment) ####
######################################################
binarycounter:
    la $t0, counter                             # $t0 = &counter
    beq $a0, 0, endifbinary                     # if (reset == 1)
    ori $v0, $0, 0                              # return counter = 0;
    sw $v0, 0($t0)
    jr $ra

endifbinary:
    lw $v0, 0($t0)
    add $v0, $v0, $a1                           # $v0 = *counter + increment;
    andi $v0, $v0, 0x0F
    sw $v0, 0($t0)                              # *counter = $t1;
    jr $ra                                      # return *counter;


######################################################
#### int johnsoncounter (int reset, int increment) ###
######################################################
johnsoncounter:
    la $t0, counter                             # $t0 = &counter
    beq $a0, 0, endifjohnson                    # if (reset == 1)

resetjohnson:
    or $t2, $0, $0                              # *counter = 0;
    j endjohnson

endifjohnson:
    lw $t2, 0($t0)
    add $t2, $t2, $a1                           # $t2 = *counter + increment
    bge $t2, JOHNSON_SIZE, resetjohnson         # if (i > JOHNSON_SIZE) return johnson[0];
    bge $t2, $0, endjohnson                     # if (i < 0) return johnson[JOHNSON_SIZE - 1];
    li $t2, 7

endjohnson:
    sw $t2, 0($t0)                              # Save new counter value
    la $t0, johnson                             # $t0 = &johnson
    sll $t2, $t2, 2                             # *counter *= 4;
    add $t0, $t0, $t2                           # $t0 = &johnson[0] + *counter;
    lw $v0, 0($t0)                              # return *(&johnson[0] + *counter);
    jr $ra

