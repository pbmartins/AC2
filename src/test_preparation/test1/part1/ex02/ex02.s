    .equ SFR_HI_BASE, 0xBF88
    .equ TRISB, 0x6040
    .equ PORTB, 0x6050
    .equ LATB, 0x6060
    .equ TRISE, 0x6100
    .equ PORTE, 0x6110
    .equ LATE, 0x6120
    .equ TIMER, 20000000
    .data
gray: .word 0x00, 0x01, 0x03, 0x02, 0x06, 0x07, 0x05, 0x04, 0x0C, 0x0D, 0x0F, 0x0E, 0x0A, 0xB, 0x09, 0x08
    .text
    .globl main
main:
    lui $s0, SFR_HI_BASE
    la $s2, gray

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
    or $t0, $0, $0

searchloop:
    sll $t0, $t0, 2
    addu $t1, $t0, $s2
    lw $t1, 0($t1)
    beq $s1, $t1, endsearchloop
    addi $t0, $t0, 1
    b searchloop

endsearchloop:
    or $s1, $t0, $0
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
    sll $t1, $s1, 2
    addu $t1, $t1, $s2
    lw $t1, 0($t1)
    or $t0, $t0, $t1
    sw $t0, LATE($s0)

    addi $s1, $s1, -1
    andi $s1, $s1, 0x0F
    
    b loop

endloop:
    jr $ra
