		#dos starts all .com programs at this offset
		.org	0x100

	        .code16

#Define constants
DOS_INT 	=       0x21
COUNT  		=       25
QUIT   		= 	'q'
DELAY_LO 	=	0x0
DELAY_HI 	=	0x1

#Entered at program startup.
		jmp	main


#data area	
inChar:		.byte	0

main:
main1:	
		call    getchar         #read input char into inChar
		mov	inChar, %al	#al set to char read
		cmp	$QUIT, %al	#is it a QUIT character?
		jz	main3		#quit if got QUIT character
		mov     $COUNT, %cx     #set output counter
main2:  
	        call    putchar         #output inChar to console
		call    delay           #delay for time proportional to DELAY
                dec	%cx             #decrement loop counter
		jnz     main2           #loop if output counter nonzero
		jmp     main1           #back to read next char
main3:		call	exit

#Read next char from stdin into inChar.  Quit if ^C
getchar:
		push	%ax
		mov     $0x08,%ah       #specify fn: read without echo into al
		int     $DOS_INT        #read stdin into %al
		mov     %al,inChar      #mov char read to inChar
		pop	%ax
		ret

#Write inChar to console	
putchar:
		push	%ax
		movb    $02,%ah         #specify fn: output dl to stdout
		movb    inChar,%dl      #char to output
		int     $DOS_INT        #output inChar
		pop	%ax
		ret

exit:
		mov	$0x4c,%ah      	#specify fn: program exit
		mov	$0x0,%al        #setup exit status
		int	$DOS_INT        #exit program
	
	

#spin loop for delay time proportional to value in DELAY_HI_DELAY_LO
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

