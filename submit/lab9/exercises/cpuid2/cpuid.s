#Currently get_cpuid(int *eax, int *ebx, int *ecx, int *edx).
#Modify to get_cpuid(int cpuid_op, int *eax, int *ebx, int *ecx, int *edx).	
	.text
	.globl get_cpuid
get_cpuid:
	pushq	%r8		#push address of edx return
	pushq   %rcx		#push address of ecx return
	pushq   %rdx		#push address of ebx return
	xorl 	%eax, %eax	#setup cpuid opcode to 0
	movq 	%rdi, %rax
	cpuid
	#largest param in %eax
	#12-char manufacturer string in ebx, edx, ecx.
	movl	%eax, (%rsi)	#store eax cpuid result
	popq	%rax
	movl	%ebx, (%rax)	#store ebx cpuid result
	popq	%rax		#pop address for edxP
	movl    %edx, (%rax)    #store ecx cpuid result
	popq    %rax		#pop address for ecxP
	movl	%ecx, (%rax)	#store edx cpuid result
	
	ret
	
