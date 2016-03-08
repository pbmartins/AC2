# int main(void)
# {
# 	printStr("AC2 – DETPIC32 primer\n");
# 	return 0; 
# }

	.equ PRINT_STR, 8
	.data
msg: .asciiz "AC2 - DETPIC32 primer\n"
	.text
	.globl msg
	.globl main
main:
	la $a0, msg
	ori $v0, $0, PRINT_STR
	syscall						# printStr("AC2 – DETPIC32 primer\n");
	ori $v0, $0, 0				# return 0;
	jr $ra