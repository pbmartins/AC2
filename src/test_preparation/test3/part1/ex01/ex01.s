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

loop:
    # Read dip switches
    lw $t0, PORTE($s0)
    andi $t0, $t0, 0x0F0
    srl $t0, $t0, 4

    srl $t1, $t0, 3

    # Convert to sign and module
    beq $t1, $0, else
    
    # negative values
    nor $t0, $t0, $0
    andi $t0, $t0, 0x0F    
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
