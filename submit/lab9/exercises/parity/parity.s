	.text
	.globl get_parity
#edi contains n	
get_parity:

	#@TODO: add code here to set eax to 1 iff edi has even parity
	testl %edi, %edi
	jpe .parity_even
	movl $0, %eax
	jmp .end
.parity_even:
	movl $1, %eax
.end:
	ret
	
