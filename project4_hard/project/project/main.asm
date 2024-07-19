;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;	Project 4
;
;	Lama Naser 1200190
;	Maha Mali 1200746
;	Sewar Abueid 
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	PROCESSOR 16F877
;	Clock = XT 4MHz, standard fuse settings
	__CONFIG 0x3731	; 00011011100110001

;	LABEL EQUATES	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	INCLUDE "P16F877A.INC"	


Char		EQU		30	; Display character code
Num1	EQU		31	; First minimum reading
Num2	EQU		32	; Second minimum reading
Num3	EQU		33	; Third minimum reading
Num4	EQU		34	; Fourth minimum reading
Temp	EQU		35	; Temporary register
Blinks	EQU		36	; Count of nuber of blinks

;Kcode	EQU		37	; ASCII code for key
;Msd		EQU		38	; Most significant digit of result
;Lsd		EQU		39	; Least significant digit of result
;Kval	EQU		40	; Key numerical value

RS		EQU		1	; Register select output bit
E		EQU		2	; Display data strobe

; Program begins ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	ORG		0		; Default start address 
	NOP				; required for ICD mode

	BANKSEL	TRISC		; Select bank 1
	CLRF	TRISD		; Display port is output for LCD

	BANKSEL PORTC		; Select bank 0
	CLRF	PORTD		; Clear display outputs

	GOTO	start			; Jump to main program

; MAIN LOOP ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

start
	CALL	inid			; Initialise the display
	MOVLW	0x80		; position to home cursor
	BCF	Select,RS			; Select command mode
	CALL	send			; and send code
	CLRW	Char			; ASCII = 0
	
	CALL displayBlinks

loop:
	GOTO loop

; SUBROUTINES ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; Include LCD driver routine	
	INCLUDE	"LCDIS.INC"
	
displayBlinks:
	MOVLW   D'3'		; Load the W register with the value 3
	MOVWF Blinks 	; Store the value in the Blinks register

	loop_blink
	; Display 'Welcome to' on first row
	BSF Select, RS			; Select command mode
	movlw 'W'			; move letter 'W' to register w
	call send				; and send code
	
	BSF Select, RS			; Select command mode
	movlw 'e'			; move letter 'e' to register w
	call send				; and send code
	
	BSF Select, RS			; Select command mode
	movlw 'l'			; move letter 'l' to register w
	call send				; and send code
	
	BSF Select, RS			; Select command mode
	movlw 'c'			; move letter 'c' to register w
	call send				; and send code
	
	BSF Select, RS			; Select command mode
	movlw 'm'			; move letter 'm' to register w
	call send				; and send code
	
	BSF Select, RS			; Select command mode
	movlw 'e'			; move letter 'e' to register w
	call send				; and send code
	
	BSF Select, RS			; Select command mode
	movlw ' '			; move letter space to register w
	call send				; and send code

	BSF Select, RS			; Select command mode
	movlw 't'			; move letter 't' to register w
	call send				; and send code

	BSF Select, RS			; Select command mode
	movlw 'o'			; move letter 'o' to register w
	call send				; and send code

	; Display 'SFR04 Modules' on second row
	MOVLW	0xC0		; position to second row
	BCF	Select,RS			; Select command mode
	CALL	send			; and send code
	
	BSF Select, RS			; Select command mode
	movlw 'S'				; move letter 'S' to register w
	call send				; and send code

	BSF Select, RS			; Select command mode
	movlw 'F'				; move letter 'F' to register w
	call send				; and send code

	BSF Select, RS			; Select command mode
	movlw 'R'				; move letter 'R' to register w
	call send				; and send code	

	BSF Select, RS			; Select command mode
	movlw '0'				; move letter '0' to register w
	call send				; and send code

	BSF Select, RS			; Select command mode
	movlw '4'				; move letter '4' to register w
	call send				; and send code

	BSF Select, RS			; Select command mode
	movlw ' '				; move letter space to register w
	call send				; and send code

	BSF Select, RS			; Select command mode
	movlw 'M'				; move letter '0' to register w
	call send				; and send code

	BSF Select, RS			; Select command mode
	movlw 'o'				; move letter 'o' to register w
	call send				; and send code

	BSF Select, RS			; Select command mode
	movlw 'd'				; move letter 'd' to register w
	call send				; and send code

	BSF Select, RS			; Select command mode
	movlw 'u'				; move letter 'u' to register w
	call send				; and send code

	BSF Select, RS			; Select command mode
	movlw 'l'				; move letter 'l' to register w
	call send				; and send code

	BSF Select, RS			; Select command mode
	movlw 's'				; move letter 's' to register w
	call send				; and send code
	
	; Delay with 0.5 second = 500ms
	MOVLW 500			; move 500 to register w
	CALL xms			; call xms delay function from LCD include file
	CALL clear			; clear the display
	CALL xms			; delay for 0.5 second
	DECFSZ Blinks,F  		;Decrement W and skip the next instruction if zero		
	GOTO loop_blink
	RETURN
	
; Restart ................................................
clear:
	MOVLW	01			; code to clear display
	BCF	Select,RS			; Select data mode
	CALL	send			; and send code	
	RETURN

	END	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;