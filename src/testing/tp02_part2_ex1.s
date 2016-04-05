    .equ SFR_HI_BASE, 0xBF88
    .equ TRISE, 0x6100
    .equ PORTE, 0x6110
    .equ LATE,  0x6120
    .equ TIMER, 5000000
    .equ JOHNSON_SIZE, 7                # length(johnson) - 1
    .data
johnson:    .word 0x00, 0x01, 0x03, 0x06, 0x0F, 0x0E, 0x0C, 0x0A
counter:    .word 0
    .text
    .globl main
main:
    addiu $sp, $sp, -12
    sw $ra, 0($sp)
    sw $s0, 4($sp)                      # SFR base register
    sw $s1, 8($sp)                      # RE6 previous value

    # $t1 = RE6 current value
    # $t2 = RE7 current value

    la $s0, SFR_HI_BASE
    
    # Configure R0-3 as outputs and R6-7 as inputs
    lw $t0, TRISE($s0)
    andi $t0, $t0, 0xF0
    ori $t0, $t0, 0x0C0
    sw $t0, 0($s0)

    # Start infinite loop
loop:
    # Read coreTimer
    ori $v0, $0, 11
    syscall

    blt $v0, TIMER, loop                # if (readCoreTimer() <= TIMER) continue;

    # Reset coreTimer
    ori $v0, $0, 12
    syscall

    # Read RE6-7 bits
    lw $t0, PORTE($s0)
    andi $t0, $t0, 0x0C0
    srl $t0, $t0, 6
    andi $t1, $t0, 0x01                 # RE6 = ((PORTE & 0x0C0) >> 6) && 0x01;
    srl $t2, $t0, 1                     # RE7 = ((PORTE & 0x0C0) >> 6) >> 1;

    or $a0, $0, $0                      # reset = 0;
    
    beq $s1, $t1, endifreset            # if (previousRE6 != RE6)
    ori $a0, $0, 1                      # reset = 1;

endifreset:
    addi $a1, $0, -1                    # increment = -1;

    bne $t2, $0, endifincrement         # if (RE7 == 0)
    ori $a1, $0, 1                      # increment = 1;

endifincrement:
    bne $t1, $0, elsecounter            # if (RE6 == 0)
    jal binarycounter                   # binaryCounter(reset, increment);
    b endifcounter

elsecounter:
    jal johnsoncounter                  # johnsonCounter(reset, increment);

endifcounter:
    # Save current RE6 value in previous RE6 variable
    or $s1, $t1, $0                     # previousR6 = RE6;

    # Send $v0 to LEDs (RE0-3) 
    lw $t0, LATE($s0)
    andi $t0, $t0, 0xF0
    or $t0, $t0, $v0
    sw $t0, LATE($s0)    

    b loop

endloop: 
    lw $ra, 0($sp)
    lw $s0, 4($sp)
    lw $s1, 8($sp)
    addiu $sp, $sp, 12
    jr $ra


#####################################################
### int binarycounter (int reset, int increment) ####
#####################################################
binarycounter:
    la $t0, counter
    bne $a0, $0, endifresetbin          # if (reset == 0)
    or $v0, $0, $0                      # $v0 = 0;
    b returnbin
    
endifresetbin:
    lw $v0, 0($t0)                      # $v0 = *counter;
    addu $v0, $v0, $a1                  # $v0 += increment;
    andi $v0, $v0, 0x0F                 # $v0 &= 0x0F;    

returnbin:
    sw $v0, 0($t0)                      # *counter = $v0;
    jr $ra                              # return *counter;


#####################################################
### int johnsoncounter (int reset, int increment) ###
#####################################################

johnsoncounter:
    la $t0, counter
    bne $a0, $0, endifresetjohnson      # if (reset == 0)

resetjohnson:
    or $v0, $0, $0                      # $v0 = 0;
    b returnjohnson

endifresetjohnson:
    lw $v0, 0($t0)                      # $v0 = *counter;
    addu $v0, $v0, $a1                  # $v0 += increment;
    bgt $v0, JOHNSON_SIZE, resetjohnson # if ($v0 > JOHNSON_SIZE - 1)
    bge $v0, $0, returnjohnson          # if ($v0 < 0)
    ori $v0, $0, JOHNSON_SIZE           # $v0 = JOHNSON_SIZE - 1;

returnjohnson:
    sw $v0, 0($t0)                      # *counter = $v0;

    sll $v0, $v0, 2                     # $v0 *= 4;
    la $t0, johnson
    addu $t0, $t0, $v0
    lw $v0, 0($t0)                      # $v0 = johnson[*counter];
    jr $ra

