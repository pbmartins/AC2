    .equ SFR_HI_BASE, 0xBF88
    .equ TRISB, 0x6040
    .equ PORTB, 0x6050
    .equ LATB, 0x6060
    .equ TRISE, 0x6100
    .equ PORTE, 0x6110
    .equ LATE, 0x6120
    .equ TIMER, 20000000
    .text
    .globl main
main:
    lui $s0, SFR_HI_BASE
    
    # Configure leds
    lw $t0, TRISE($s0)
    andi $t0, $t0, 0xF0
    sw $t0, TRISE($s0)

    # Configure switches
    lw $t0, TRISB($s0)
    ori $t0, $t0, 0x0F
    sw $t0, TRISB($s0)

    # counter
    or $s1, $0, $0

loop:
    # Rear inkey
    ori $v0, $0, 1
    syscall

    bne $v0, 105, else
    # Read switches
    lw $t0, PORTB($s0)
    andi $s1, $t0, 0x0F
    b display

else:
    # Read clock value
    ori $v0, $0, 11
    syscall
    ble $v0, TIMER, loop
    
    # Reset clock
    ori $v0, $0, 12
    syscall

display:
    # Show counter on leds
    lw $t0, LATE($s0)
    andi $t0, $t0, 0xF0
    or $t0, $t0, $s1
    sw $t0, LATE($s0)

    addi $s1, $s1, -1
    andi $s1, $s1, 0x0F
    
    b loop

endloop:
    jr $ra
