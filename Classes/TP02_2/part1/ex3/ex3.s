    .equ SFR_BASE_HI, 0xBF88
    .equ TRISE, 0x6100
    .equ PORTE, 0x6110
    .equ LATE, 0x6120
    .text
    .globl main
main:
    lui $t0, SFR_BASE_HI
    lw $t1, TRISE($t0)
    andi $t1, $t1, 0xFFF0 # R0, R1, R2, R3 - output
    ori $t1, $t1, 0x00C0 # R6, R7 - input
    sw $t1, TRISE($t0)

loop:
    lw $t1, PORTE($t0)
    andi $t1, $t1, 0x00C0
    srl $t1, $t1, 6
    andi $t2, $t1, 0x0002
    srl $t2, $t2, 1 # $t2 = R7
    andi $t1, $t1, 0x0001 # $t1 = R6

    or $t3, $0, $0

    nor $t4, $t1, $t2
    andi $t4, $t4, 0x0001
    or $t3, $t3, $t4
    sll $t3, $t3, 1

    xor $t4, $t1, $t2
    or $t3, $t3, $t4
    sll $t3, $t3, 1

    or $t4, $t1, $t2
    or $t3, $t3, $t4
    sll $t3, $t3, 1

    and $t4, $t1, $t2
    or $t3, $t3, $t4

    lw $t2, LATE($t0)
    andi $t2, $t2, 0xFFF0
    or $t2, $t2, $t3
    sw $t2, LATE($t0)
    j loop

