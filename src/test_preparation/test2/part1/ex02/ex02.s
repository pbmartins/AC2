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
    
    or $s1, $0, $0 # counter

    # Configure LEDs as outputs
    lw $t0, TRISE($s0)
    andi $t0, $t0, 0xF0
    sw $t0, TRISE($s0)

    # Configure Switches as inputs
    lw $t0, TRISB($s0)
    ori $t0, $t0, 0x0F
    sw $t0, TRISB($s0)

loop:
    # Check if char 'i' is read
    ori $v0, $0, 1
    syscall

    beq $v0, 105, readswitches 

    # Read core timer
    ori $v0, $0, 11
    syscall

    ble $v0, TIMER, loop

    # Reset core timer
    ori $v0, $0, 12
    syscall

showleds:
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

readswitches:
    lw $t0, PORTB($s0)
    andi $s1, $t0, 0x0F
    b readswitches
