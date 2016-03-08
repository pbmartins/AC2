    .equ SFR_BASE_HI, 0xBF88
    .equ TRISE, 0x6100
    .equ PORTE, 0x6110
    .equ LATE, 0x6120
    .text
    .globl main
main:
    lui $t0, SFR_BASE_HI
    lw $t1, TRISE($t0)
    andi $t1, $t1, 0xFFFE # R0 - output
    ori $t1, $t1, 0x0040 # R6 - input
    sw $t1, TRISE($t0)

loop:
    lw $t1, PORTE($t0)
    andi $t1, $t1, 0x0040
    srl $t1, $t1, 6
    lw $t2, LATE($t0)
    andi $t2, $t2, 0xFFFE
    or $t2, $t2, $t1
    sw $t2, LATE($t0)
    j loop

