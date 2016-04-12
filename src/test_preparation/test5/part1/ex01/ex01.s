    .equ SFR_HI_BASE, 0xBF88
    .equ TRISE, 0x6100
    .equ PORTE, 0x6110
    .equ LATE, 0x6120
    .equ TIMER, 20000000
    .text
    .globl main
main:
    lui $s0, SFR_HI_BASE

    # Configure LEDs as outputs and dip switches as inputs
    lw $t0, TRISE($s0)
    andi $t0, $t0, 0xF0
    ori $t0, $t0, 0x0F0
    sw $t0, TRISE($s0)

    or $s1, $0, $0 # counter

loop:
    # Read timer
    ori $v0, $0, 11
    syscall
    
    ble $v0, TIMER, loop

    # Reset timer
    ori $v0, $0, 12
    syscall

    # Read 'i' char
    ori $v0, $0, 1
    syscall

    bne $v0, 105, changevalue
    
    lw $t0, PORTE($s0)
    srl $t0, $t0, 4
    andi $s1, $t0, 0x0F
    
changevalue:
    lw $t0, LATE($s0)
    andi $t0, $t0, 0xF0
    or $t0, $t0, $s1
    sw $t0, LATE($s0)
    
    addi $s1, $s1, -1
    andi $s1, $s1, 0x0F

    b loop
