    .equ SFR_HI_BASE, 0xBF88
    .equ TRISE, 0x6100
    .equ PORTE, 0x6110
    .equ LATE, 0x6120
    .text
    .globl main
main:
    lui $s0, SFR_HI_BASE

    # Configure switches as inputs and LEDs as outputs
    lw $t0, TRISE($s0)
    andi $t0, $t0, 0xF0
    ori $t0, $t0, 0xF0
    sw $t0, TRISE($s0)

    addi $s1, $0, -7

loop:
    # Read value from keyboard
    ori $v0, $0, 5
    syscall

    bgt $v0, 7, loop
    blt $v0, $s1, loop
    
    andi $t0, $v0, 0x0F

    srl $t1, $t0, 3

    # Convert to sign and module
    beq $t1, $0, else
    
    # negative values
    nor $t0, $t0, $0
    andi $t0, $t0, 0x0F
    addi $t0, $t0, 1    
    beq $t0, $0, else
    ori $t0, $t0, 0x08

else:
    # save values on LEDs
    lw $t1, LATE($s0)
    andi $t1, $t1, 0xF0
    or $t1, $t1, $t0
    sw $t1, LATE($s0)

    b loop
    
endloop:
    jr $ra
