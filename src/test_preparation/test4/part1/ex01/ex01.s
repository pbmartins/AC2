    .equ SFR_HI_BASE, 0xBF88
    .equ TRISE, 0x6100
    .equ PORTE, 0x6110
    .equ LATE, 0x6120
    .text
    .globl main
main:
    lui $s0, SFR_HI_BASE

    # Configure LEDs as outputs and switches as inputs
    lw $t0, TRISE($s0)
    andi $t0, $t0, 0xF0
    ori $t0, $t0, 0x0F0
    sw $t0, TRISE($s0)

loop:
    # Read switches
    lw $t0, PORTE($s0)
    srl $t0, $t0, 4
    andi $t0, $t0, 0x0F

    srl $t1, $t0, 3
    andi $s1, $t1, 0x01

    srl $t1, $t0, 1
    andi $t1, $t1, 0x02
    or $s1, $s1, $t1
    
    sll $t1, $t0, 1
    andi $t1, $t1, 0x04
    or $s1, $s1, $t1

    sll $t1, $t0, 3
    andi $t1, $t1, 0x08
    or $s1, $s1, $t1

    lw $t0, LATE($s0)
    andi $t0, $t0, 0xF0
    or $t0, $t0, $s1
    sw $t0, LATE($s0)

    b loop

endloop:
    jr $ra
