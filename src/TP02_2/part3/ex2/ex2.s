    .equ SFR_BASE_HI, 0xBF88
    .equ TRISE, 0x6100
    .equ PORTE, 0x6110
    .equ LATE, 0x6120
    .text
    .globl main
main:
    addiu $sp, $sp, -12
    sw $ra, 0($sp)
    sw $s0, 4($sp)
    sw $s1, 8($sp)
    or $s1, $0, $0                      # int v = 0;

    lui $s0, SFR_BASE_HI                # TRISE4 = 0;
    lw $t0, TRISE($s0)
    andi $t0, $t0, 0xFFEF
    sw $t0, TRISE($s0)

while:                                  # while(1)
    lw $t0, LATE($s0)                   # LATE4 = v;
    andi $t0, $t0, 0xFFEF
    sll $t1, $s1, 4
    or $t0, $t0, $t1
    sw $t0, LATE($s0)
    li $a0, 1                           # delay(1);
    jal delay
    xori $s0, $s0, 1                    # v ^= 1;
    j while

    addiu $sp, $sp, 12
    lw $ra, 0($sp)
    lw $s0, 4($sp)
    lw $s1, 8($sp)
    jr $ra
