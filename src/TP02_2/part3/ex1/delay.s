	.equ CALIBRATION_VALUE, 9966
	.text
	.globl delay
delay:
	or $t0, $a0, $0 					# $t0 = $a0

externalfor:
	beq $t0, $0, endexternalfor			# for(; n_intervals != 0; n_intervals--)
	ori $t1, $0, CALIBRATION_VALUE		# i = CALIBRATION_VALUE;

internalfor:
	beq $t1, $0, endinternalfor			# for(i = CALIBRATION_VALUE; i != 0; i--)
	addiu $t1, $t1, -1					# i--;
	j internalfor

endinternalfor:
	addiu $t0, $t0, -1					# n_intervals--;
	j externalfor

endexternalfor:
	jr $ra
