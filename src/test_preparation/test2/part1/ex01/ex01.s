    .equ SFR_HI_BASE, 0xBF88
    .equ TRISE, 0x6100
    .equ PORTE, 0x6110
    .equ LATE, 0x6120
    .equ TIMER, 20000000
    .text
    .globl main
main:
    lui $s0, SFR_HI_BASE
    
    or $s1, $0, $0 # counter

    # Configure LEDs as outputs
    lw $t0, TRISE($s0)
    andi $t0, $t0, 0xF0
    sw $t0, TRISE($s0)

loop:
    # Read core timer
    ori $v0, $0, 11
    syscall

    ble $v0, TIMER, loop

    # Reset core timer
    ori $v0, $0, 12
    syscall

    # Copy value of counter to LEDs
    lw $t0, LATE($s0)
    andi $t0, $t0, 0xF0
    or $t0, $t0, $s1
    sw $t0, LATE($s0)
    
    addi $s1, $s1, -1
    andi $s1, $s1, 0x0F
    b loop

endloop:
    jr $ra
