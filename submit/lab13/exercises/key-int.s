		#dos starts all .com programs at this offset
		.org	0x100

	        .code16

#Define constants
DOS_INT		=       0x21  #use INT 0x21 to generate s/w interrupt to DOS
KEY_INT 	= 	0x9   #interrupt vector # for keyboard interrupt
CHK_INT 	= 	0x16  #
COUNT  		=         50
QUIT   		= 	'q'
DELAY_LO 	=	0x0
DELAY_HI 	=	0x2

#Entered at program startup.
		jmp	main


#data area	
inChar:		.byte	0
hasKey:	        .byte   0
intAddr:	.word   0, 0
chkAddr:	.word   0, 0

main:
	        call    setupHandler
main1:	
		call    getchar         #read input char into inChar
		mov	inChar, %al	#al set to char read
		cmp	$QUIT, %al	#is it a QUIT character?
		jz	main3		#quit if got QUIT character
		mov     $COUNT, %cx     #set output counter
main2:
		mov	hasKey, %al	#put checkKey result into al
		test	%al, %al	#non-zero if keyboard has char
	        jz	main4           #no key press
		call	getchar
main4:	
	        call    putchar         #output inChar to console
		call    delay           #delay for time proportional to DELAY
                dec	%cx             #decrement loop counter
		jnz     main2           #loop if output counter nonzero
		jmp     main1           #back to read next char
main3:
		call    resetHandler
		call	exit

#Save address of current interrupt handler for keyboard interrupt in intAddr
#and address of bios interrupt handler in chkAddr.
#Point keyboard interrupt to intHandler	
setupHandler:
		push	%ax
		push	%bx
		push	%dx
		push	%es

	        #save address of handler for CHK_INT in chkAddr
		mov	$CHK_INT, %al   #int vec for checking keyboard buf
		mov	$0x35, %ah      #specify fn to read current int vector
		int	$DOS_INT	#interrupt vector addr in es:bx
		mov     %bx, chkAddr    #save old int vector 
		mov     %es, chkAddr+2  #in mem

	        #save address of handler for KEY_INT in intAddr
		mov	$KEY_INT, %al   #int vec for reading keyboard
		mov	$0x35, %ah      #specify fn to read current int vector
		int	$DOS_INT	#interrupt vector addr in es:bx
		mov     %bx, intAddr    #save old int vector
		mov     %es, intAddr+2  #in mem

	        #set address of keyboard interrupt handler to intHandler
		mov	$KEY_INT, %al   #replace int vec for reading keyboard
		mov	$0x25, %ah      #specify fn to write int vec from dx
		mov     $intHandler, %dx#point dx to new int handler
		int     $DOS_INT        #set int address to ds:dx
		pop	%es
		pop	%dx
		pop	%bx
		pop	%ax
		ret

#Cleanup keyboard interrupt handler: restore to original value saved
#in intAddr.
resetHandler:
		push	%ax
		push	%dx
		push	%ds
		mov	$KEY_INT, %al
		mov	$0x25, %ah
		mov	intAddr, %dx
		mov	intAddr + 2, %ax
		mov	%ax, %ds
		int     $DOS_INT        #set int address to ds:dx
		pop	%ds
		pop	%dx
		pop	%ax
		ret
	
#Interrupt handler for keyboard
#stack contains ret addr + flags
intHandler:
		push    %ds
		push    %ax
		mov     %cs, %ax	#ensure ds register same as
		mov	%ax, %ds	#cs register
		pushf                   #save flags
		lcall   *intAddr	#call original keyboard handler
		mov 	$0x01, %ah	#setup for keyboard status bios call
		pushf
		lcall	*chkAddr	#call bios interrupt handler
		cli			#disable interrupts
	        jz      intHandler1     #no key available
		mov	$0x1, %al
	        mov     %al, hasKey     #signal we have a key
intHandler1:	sti			#renable interrupts
		#stack contains flags
		pop 	%ax
		pop	%ds
		iret                    #special return from interrupt handler

#Read next char from stdin into inChar.  Quit if ^C
getchar:
		push	%ax
		mov     $0x08, %ah      #setup input function
		int     $DOS_INT        #read stdin into %al
		mov     %al, inChar     #mov char read to inChar
	        xor	%al, %al
		mov	%al, hasKey     #reset hasKey flag
		pop	%ax
		ret

#Write inChar to console	
putchar:
		push	%ax
		movb    $02,%ah         #setup output call
		movb    inChar,%dl      #char to output
		int     $DOS_INT        #output inChar
		pop	%ax
		ret

exit:
		mov	$0x4c,%ah      	#setup program exit
		mov	$0x0,%al        #setup exit status
		int	$DOS_INT        #exit program
	
	

#delay loop for time proportional to value in DELAY_HI_DELAY_LO
delay:		push	%ax
		push	%bx
		push	%cx
		mov	$DELAY_HI, %bx
delay1:		mov     $DELAY_LO, %cx
delay2:		mov     $0x0f0a0, %ax
	        imul    %cl
	        dec     %cx
	        jnz     delay2
	        dec     %bx
	        jnz	delay1
	        pop	%cx
		pop	%bx
		pop	%ax
		ret

