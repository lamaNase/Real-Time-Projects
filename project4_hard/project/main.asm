;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;	Project 4
;
;	Lama Naser 1200190
;	Maha Mali 1200746
;	Sewar Abueid 1200043
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	PROCESSOR 16F877
;	Clock = XT 4MHz, standard fuse settings
	__CONFIG 0x3731	; 00011011100110001

;	LABEL EQUATES	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	INCLUDE "P16F877A.INC"	


hund	EQU		30		; hold 100 for division
Num1	EQU		31		; First minimum reading
Num2	EQU		32		; Second minimum reading
Num3	EQU		33		; Third minimum reading
Num4	EQU		34		; Fourth minimum reading
Temp	EQU		35		; Temporary register
Blinks	EQU		36		; Count of nuber of blinks
TempH	EQU		37		; Timer 1 Hight bits
TempL	EQU		38		; Timer 1 low bits
ten		EQU		39		; hold 10 for division
counter	EQU		40		; counter register
resH		EQU		41		
resL		EQU		42	
sensor	EQU		43		; current sensor
sensor1	EQU		44		; first sensor number with higher reading
sensor2	EQU		45		; second sensor number with higher reading
sensor3	EQU		46		; third sensor number with higher reading
sensor4	EQU		47		; fourth sensor number with higher reading
index	EQU		48		; register to hold the ultrasonic number in a single decoder/mux

RS		EQU		1		; Register select output bit
E		EQU		2		; Display data strobe

; Program begins ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	ORG		0			; Default start address 
	NOP					; required for ICD mode

	BANKSEL	TRISD		; Select bank 1
	CLRF	TRISD		; Display port is output for LCD

	BANKSEL TRISC
	; Enables pins configured as outputs
	BCF TRISC, RC0		; enable mux 1 as output
	BCF TRISC, RC1		; enable mux 2 as output
	BCF TRISC, RC2		; enable mux 3 as output
	BCF TRISC, RC3		; enable decoder 1 as output
	BCF TRISC, RC4		; enable decoder 2 as output
	BCF TRISC, RC5		; enable decoder 3 as output
	BSF TRISC, RC6		; Echo pin as input
	
	BANKSEL TRISB		; Select bank 0
	BCF TRISB,RB0			; selection line A
	BCF TRISB, RB1		; selection line B
	BCF TRISB, RB2		; selection line C
	BCF TRISB, RB3
 
	BANKSEL PORTD		; Select bank 0
	CLRF	PORTD		; Clear display outputsC

	BANKSEL PORTC
	; initialize enables with disable mode
	BSF PORTC, RC0		; enable mux 1
	BSF PORTC, RC1		; enable mux 2
	BSF PORTC, RC2		; enable mux 3
	BCF PORTC, RC3		; enable decoder 1
	BCF PORTC, RC4		; enable decoder 2
	BCF PORTC, RC5		; enable decoder 3

	; initalize 10 & 100 for divisions
	MOVLW D'10'
	MOVWF ten
	MOVLW D'100'
	MOVWF hund

	GOTO	start			; Jump to main program

; MAIN LOOP ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

start
	BANKSEL PORTB
	MOVLW B'00000000'
	MOVWF index
	MOVWF PORTB		; set selections A, B and C to 000

	BANKSEL PORTD
	CALL	inid			; Initialise the display
	MOVLW	0x80		; position to home cursor
	BCF	Select,RS			; Select command mode
	CALL	send			; and send code
	CLRW	Char			; ASCII = 0

	; display a message that blinks three times
	CALL displayBlinks

loop:
	MOVLW D'0'
	MOVWF sensor		; current sensor number

	; initalize highest distances as zeros
	MOVLW D'0'
	MOVWF Num1
	MOVWF Num2
	MOVWF Num3
	MOVWF Num4

	; make some delay
	MOVLW 10000
	CALL xms

	BANKSEL PORTD
	CALL clear

	BANKSEL PORTC
	BCF PORTC, RC0		; enable mux 1
	BSF PORTC, RC1		; disable mux 2
	BSF PORTC, RC2		; disable mux 3
	; note that multiplexers' enables work active low

	BSF PORTC, RC3		; enable decoder 1
	BCF PORTC, RC4		; disable decoder 2
	BCF PORTC, RC5		; disable decoder 3
	; note that decoders' enables work active high

	CALL read_echo1		; read first 8 ultrasonic sensors

	BANKSEL PORTC
	BSF PORTC, RC0		; disable mux 1
	BCF PORTC, RC1		; enable mux 2
	BSF PORTC, RC2		; disable mux 3

	BCF PORTC, RC3		; disable decoder 1
	BSF PORTC, RC4		; enable decoder 2
	BCF PORTC, RC5		; disable decoder 3

	CALL read_echo2

	BANKSEL PORTC
	BSF PORTC, RC0		; disable mux 1
	BSF PORTC, RC1		; disable mux 2
	BCF PORTC, RC2		; enable mux 3

	BCF PORTC, RC3		; disable decoder 1
	BCF PORTC, RC4		; disable decoder 2
	BSF PORTC, RC5		; enable decoder 3

	CALL read_echo3

	; reseting for reading all ultrasonics again
	CALL print_reset

	; make some delay before going to initial state
	MOVLW 5000
	CALL xms

	GOTO loop

; SUBROUTINES ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

read_echo1:
	; This function sends pulse to trigger pin and read pulse from echo pin for the first 8 ultrasonics
	BANKSEL PORTB
	MOVLW B'00000000'
	MOVWF index
	MOVWF PORTB		; set selections A, B and C to 000

	BANKSEL PORTD
	CALL clear

read_loop1:

	; send pulse to trigger pin
	BANKSEL PORTC
	BCF PORTC, RC3		; disable decoder one
	CALL delay_2us		; wait 2 micro seconds
	BSF PORTC, RC3		; enable decoder one
	CALL delay_10us		;wait 10 micro seconds
	BCF PORTC, RC3		; disable decoder one

wait_echo_high1:
	BTFSS PORTC, RC6
	GOTO wait_echo_high1

	; Echo pulse is high, start timing
	BANKSEL T1CON
	CLRF TMR1H
	CLRF TMR1L
	BSF T1CON, TMR1ON

	BANKSEL PORTC
wait_echo_low1:
	BTFSC PORTC,RC6
	 GOTO wait_echo_low1
	
	; echo low, stop timer
	BANKSEL T1CON
	BCF T1CON, TMR1ON

	; convert time from micro second to second
	; Right shift TMR1 by 20 bits (divide by 1,048,576 ˜ 1,000,000)
	MOVLW D'20'         	; Load loop counter with 20
    	MOVWF Temp       		; Store loop counter in Temp or any working register
	CALL shift_right_nbits

	; convert to distance
	CALL mul_1114
	; Right shift TMR1 by 16 bits (divide by 65,536)
	MOVLW D'16'         	; Load loop counter with 16
    	MOVWF Temp       		; Store loop counter in Temp or any working register
	CALL shift_right_nbits

	CALL print_dist_highest
		
	INCF sensor			; go to the next sensor

	BANKSEL PORTB		; go to bank 0
	INCF index			; go to the next ultrasonic
	MOVF index,W
	MOVWF PORTB	

	; wait one second before going to the next sensor
	BANKSEL PORTD
	MOVLW 6000
	CALL xms			; add 1 second delay
	CALL clear			; clear the display for the next reading

	BANKSEL PORTB
	BTFSS PORTB, RB3		; test if we reached the 111
	GOTO read_loop1
	RETURN				; go to the next 8 ultrasonics

read_echo2:
	BANKSEL PORTB
	MOVLW B'00000000'
	MOVWF index
	BANKSEL PORTB
	MOVWF PORTB		; set selections A, B and C to 000

	BANKSEL PORTD
	CALL clear

read_loop2:

	BANKSEL PORTC
	BCF PORTC, RC4		; disable decoder two
	CALL delay_2us		; wait 2 micro seconds
	BSF PORTC, RC4		; enable decoder two
	CALL delay_10us		;wait 10 micro seconds
	BCF PORTC, RC4		; disable decoder two

wait_echo_high2:
	BTFSS PORTC, RC6
	GOTO wait_echo_high2

	; Echo pulse is high, start timing
	BANKSEL T1CON
	CLRF TMR1H
	CLRF TMR1L
	BSF T1CON, TMR1ON

	BANKSEL PORTC
wait_echo_low2:
	BTFSC PORTC,RC6
	 GOTO wait_echo_low2
	
	; echo low, stop timer
	BANKSEL T1CON
	BCF T1CON, TMR1ON

	; convert time from micro second to second
	; Right shift TMR1 by 20 bits (divide by 1,048,576 ˜ 1,000,000)
	MOVLW D'20'         	; Load loop counter with 20
    	MOVWF Temp       		; Store loop counter in Temp or any working register
	CALL shift_right_nbits

	; convert to distance
	CALL mul_1114
	; Right shift TMR1 by 16 bits (divide by 65,536)
	MOVLW D'16'         	; Load loop counter with 16
    	MOVWF Temp       		; Store loop counter in Temp or any working register
	CALL shift_right_nbits

	CALL print_dist_highest
		
	INCF sensor			; go to the next sensor

	BANKSEL PORTB		; go to bank 0
	INCF index			; go to the next ultrasonic
	MOVF index,W
	MOVWF PORTB	

	; wait one second before going to the next sensor
	BANKSEL PORTD
	MOVLW 6000
	CALL xms
	CALL clear			; clear the display for the next reading

	BTFSS PORTB, RB3		; test if we reached the 111
	GOTO read_loop2
	RETURN

read_echo3:
	BANKSEL PORTB
	MOVLW B'00000000'
	MOVWF index
	BANKSEL PORTB
	MOVWF PORTB		; set selections A, B and C to 000

	BANKSEL PORTD
	CALL clear

read_loop3:

	BANKSEL PORTC
	BCF PORTC, RC5		; disable decoder three
	CALL delay_2us		; wait 2 micro seconds
	BSF PORTC, RC5		; enable decoder three
	CALL delay_10us		;wait 10 micro seconds
	BCF PORTC, RC5		; disable decoder three

wait_echo_high3:
	BTFSS PORTC, RC6
	GOTO wait_echo_high3

	; Echo pulse is high, start timing
	BANKSEL T1CON
	CLRF TMR1H
	CLRF TMR1L
	BSF T1CON, TMR1ON

	BANKSEL PORTC
wait_echo_low3:
	BTFSC PORTC,RC6
	 GOTO wait_echo_low3
	
	; echo low, stop timer
	BANKSEL T1CON
	BCF T1CON, TMR1ON

	; convert time from micro second to second
	; Right shift TMR1 by 20 bits (divide by 1,048,576 ˜ 1,000,000)
	MOVLW D'20'         	; Load loop counter with 20
    	MOVWF Temp       		; Store loop counter in Temp or any working register
	CALL shift_right_nbits

	; convert to distance
	CALL mul_1114
	; Right shift TMR1 by 16 bits (divide by 65,536)
	MOVLW D'16'         	; Load loop counter with 16
    	MOVWF Temp       		; Store loop counter in Temp or any working register
	CALL shift_right_nbits
	CALL check_valid_range

	CALL print_dist_highest
		
	INCF sensor			; go to the next sensor

	BANKSEL PORTB		; go to bank 0
	INCF index			; go to the next ultrasonic
	MOVF index,W
	MOVWF PORTB	

	; wait one second before going to the next sensor
	BANKSEL PORTD
	MOVLW 6000
	CALL xms
	CALL clear			; clear the display for the next reading

	BTFSS PORTB, RB3		; test if we reached the 111
	GOTO read_loop3
	RETURN

; Include LCD driver routine	
	INCLUDE	"LCDIS.INC"
	
displayBlinks:
	BANKSEL PORTD
	MOVLW   D'3'		; Load the W register with the value 3
	MOVWF Blinks 		; Store the value in the Blinks register

loop_blink
	; Display 'Welcome to' on first row
	BSF Select, RS			; Select command mode
	movlw 'W'			; move letter 'W' to register w
	call send				; and send code
	
	BSF Select, RS			; Select command mode
	movlw 'e'				; move letter 'e' to register w
	call send				; and send code
	
	BSF Select, RS			; Select command mode
	movlw 'l'				; move letter 'l' to register w
	call send				; and send code
	
	BSF Select, RS			; Select command mode
	movlw 'c'				; move letter 'c' to register w
	call send				; and send code
	
	BSF Select, RS			; Select command mode
	movlw 'm'			; move letter 'm' to register w
	call send				; and send code
	
	BSF Select, RS			; Select command mode
	movlw 'e'				; move letter 'e' to register w
	call send				; and send code
	
	BSF Select, RS			; Select command mode
	movlw ' '				; move letter space to register w
	call send				; and send code

	BSF Select, RS			; Select command mode
	movlw 't'				; move letter 't' to register w
	call send				; and send code

	BSF Select, RS			; Select command mode
	movlw 'o'				; move letter 'o' to register w
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
	movlw 'M'			; move letter '0' to register w
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
print_reset:
	BANKSEL PORTD
	CALL clear

	; PRINT 'Restting...' on the first line
	BSF Select, RS			; Select command mode
	movlw 'R'				; move letter 'd' to register w
	call send				; and send code

	BSF Select, RS			; Select command mode
	movlw 'e'				; move letter 'd' to register w
	call send				; and send code

	BSF Select, RS			; Select command mode
	movlw 's'				; move letter 'd' to register w
	call send				; and send code

	BSF Select, RS			; Select command mode
	movlw 't'				; move letter 'd' to register w
	call send				; and send code

	BSF Select, RS			; Select command mode
	movlw 't'				; move letter 'd' to register w
	call send				; and send code

	BSF Select, RS			; Select command mode
	movlw 'i'				; move letter 'd' to register w
	call send				; and send code

	BSF Select, RS			; Select command mode
	movlw 'n'				; move letter 'd' to register w
	call send				; and send code

	BSF Select, RS			; Select command mode
	movlw 'g'				; move letter 'd' to register w
	call send				; and send code

	BSF Select, RS			; Select command mode
	movlw '.'				; move letter 'd' to register w
	call send				; and send code

	BSF Select, RS			; Select command mode
	movlw '.'				; move letter 'd' to register w
	call send				; and send code

	BSF Select, RS			; Select command mode
	movlw '.'				; move letter 'd' to register w
	call send				; and send code

	; Delay with 5 seconds = 5000ms
	MOVLW 5000		; move 500 to register w
	CALL xms			; call xms delay function from LCD includ
	
	NOP
	NOP
	; make some delay before going to initial state
	MOVLW 5000
	CALL xms

	NOP
	NOP

	; make some delay before going to initial state
	MOVLW 5000
	CALL xms

	RETURN
clear:
	BANKSEL PORTD
	MOVLW	01			; code to clear display
	BCF	Select,RS			; Select data mode
	CALL	send			; and send code	
	RETURN

delay_2us:
	NOP
	NOP
	RETURN

; Delay subroutine for 10 microseconds
delay_10us:
   	 ; With a 4MHz clock, each instruction cycle is 1 microsecond
   	 movlw 10 				; Load W with 10
Delay_10us_loop:
   	 NOP     					; 1 cycle
   	 DECFSZ W, F 				; Decrement WREG and skip if zero
   	 goto Delay_10us_loop 		; Repeat 10 times
   	 return

print_dist_highest:
	; check if current distance is higher than the highest one, then check the second, then third then fourth
	CALL check_highest
	CALL print_distances
	RETURN

print_ones:
; This function prints the ones digit of the number stored in Temp
DIV_LOOP_ONES:
	MOVF ten,W
	SUBWF Temp,W
	BTFSC STATUS, Z
	GOTO print_zero
	MOVF ten,W
	SUBWF Temp,W
	BTFSS   STATUS, C
	GOTO stop_ones
	MOVWF Temp
	SUBWF ten,W
	BTFSC STATUS, Z
	GOTO print_zero
	BTFSC  STATUS, C
	GOTO stop_ones
	GOTO DIV_LOOP_ONES

stop_ones:
	MOVF Temp,W
	ADDLW D'48'
	BSF Select, RS
	;MOVLW D'48'
	CALL send
	RETURN

print_tens:
; This function prints the tens digit from the number stored in Temp
	MOVLW D'0'
	MOVWF counter
DIV_LOOP_TENS:
	MOVF ten,W
	SUBWF Temp,W
	BTFSS STATUS, Z			; if the number is 10
	GOTO continue
	INCF counter
	GOTO stop_tens
continue:
	MOVF ten,W
	SUBWF Temp,W
	BTFSS   STATUS, C
	GOTO print_zero
	INCF counter
	MOVWF Temp
	SUBWF ten,W
	BTFSC   STATUS, C
	GOTO stop_tens
	GOTO DIV_LOOP_TENS

stop_tens:
	MOVF counter,W
	MOVWF Temp
	MOVLW D'10'
	MOVWF ten
	CALL print_ones
	RETURN

print_zero:
	BSF Select, RS
	MOVLW D'48'
	CALL send
	RETURN

print_hunds:
; This function prints the hundred's digit from the number stored in Temp
	MOVLW D'0'
	MOVWF counter
DIV_LOOP_HUNDS:
	MOVF hund,W
	SUBWF Temp,W
	MOVWF Temp
	INCF counter
	SUBWF hund,W
	BTFSC   STATUS, C
	GOTO stop_hunds
	GOTO DIV_LOOP_HUNDS
stop_hunds:
	MOVF counter,W
	ADDLW D'48'
	BSF Select, RS
	;MOVLW D'48'
	CALL send
	RETURN

shift_right_nbits:
	MOVLW 16
	SUBWF Temp, W
	BTFSC STATUS, C
    	GOTO more_than_16_shifts 	 		; If greater, handle it separately
shift_right_loop:
   	 RRF TMR1L, F             				 ; Rotate right through carry TMR1L
   	 RRF TMR1H, F             				 ; Rotate right through carry TMR1H
   	 DECFSZ Temp, F            				; Decrement loop counter and skip if zero
   	 GOTO shift_right_loop     			; Repeat loop if not zero
   	 RETURN

more_than_16_shifts:
    	CLRF TMR1L               				 ; Clear TMR1L since it will be completely shifted out
    	MOVLW 4                  				 ; Load W register with 20 - 16 = 4
    	MOVWF Temp                				; Move it to Temp
shift_right_loop_after_16:
    	RRF TMR1H, F              				; Rotate right through carry TMR1H
    	DECFSZ Temp, F           	 			; Decrement loop counter and skip if zero
    	GOTO shift_right_loop_after_16  		; Repeat loop if not zero
    	RETURN

shift_left_nbits:
	MOVF TMR1H,W
	MOVWF TempH
	MOVF TMR1L,W
	MOVWF TempL
shift_left_loop:
	RLF TempL, F     			; Rotate left through carry TMR1L
    	RLF TempH, F    			; Rotate left through carry TMR1H
    	DECFSZ Temp, F   			; Decrement loop counter and skip if zero
    	GOTO shift_left_loop  		; Repeat loop if not zero
	RETURN

mul_1114:
; 1114 = (1024 + 64 + 16 + 8 + 2) so do left shift for (10b, 6b, 4b, 3b and 1b) and accomulate them
	CLRF resH
	CLRF resL

	MOVLW D'10'
	MOVWF Temp
	CALL shift_left_nbits
	MOVF TempH,W
	ADDWF resH,F
	MOVF TempL,W
	ADDWF resL

	MOVLW D'6'
	MOVWF Temp
	CALL shift_left_nbits
	MOVF resH,W
	ADDWF TempH,W
	MOVWF resH
	MOVF resL,W
	ADDWF TempL,W
	MOVWF resL

	MOVLW D'4'
	MOVWF Temp
	CALL shift_left_nbits
	MOVF resH,W
	ADDWF TempH
	MOVWF resH
	MOVF resL,W
	ADDWF TempL,W
	MOVWF resL

	MOVLW D'3'
	MOVWF Temp
	CALL shift_left_nbits
	MOVF resH,W
	ADDWF TempH
	MOVWF resH
	MOVF resL,W
	ADDWF TempL,W
	MOVWF resL

	MOVLW D'1'
	MOVWF Temp
	CALL shift_left_nbits
	MOVF resH,W
	ADDWF TempH
	MOVWF resH
	MOVF resL,W
	ADDWF TempL,W
	MOVWF resL

	; move resH to TMR1H, resL to TMR1L
	MOVF resH,W
	MOVWF TMR1L
	MOVF resH,W
	MOVWF TMR1H
	RETURN

check_valid_range:
	MOVLW 2
	SUBWF TMR1L, W
	BTFSC STATUS, C
	GOTO check_400
	CLRF TMR1L
check_400:
	MOVLW 400
	SUBWF TMR1L, W
	BTFSS STATUS, C
	RETURN
	MOVLW D'0'
	CLRF TMR1L

check_highest:
	MOVF Num1, W
	SUBWF TMR1L,W
	BTFSS   STATUS, C			; skip if current distance is larger
	GOTO check_second		; if it is less than the highest, check with the second
	
	; TMR1 is greater then, do shift
	; first, shift third number to fourth number
	MOVF Num3, W			; move third number to register W
	MOVWF Num4			; move third number to fourth number

	MOVF sensor3, W
	MOVWF sensor4			; move senosr number with third highest to fourth highest

	; second, shift second number to third number
	MOVF Num2, W			; move second number to register W
	MOVWF Num3			; move decond number to third number

	MOVF sensor2, W
	MOVWF sensor3			; move senosr number with decond highest to third highest

	; third, shift first value to second
	MOVF Num1, W			; move first highest distance value to register W
	MOVWF Num2			; move the new value to Num2

	MOVF sensor1, W			; move highest sensor number to register W
	MOVWF sensor2			; move W to second highest distance sensor number

	; finally, shift the new value to the first value
	MOVF TMR1L, W			; move time1 value to register W
	MOVWF Num1			; move the new value to Num1 as the highest distance

	MOVF sensor, W			; move current sensor number to register W
	MOVWF sensor1			; move current sensor number as the highest one

	RETURN

check_second:
	MOVF Num2, W
	SUBWF TMR1L,W
	BTFSS   STATUS, C
	GOTO check_third			; if it is less than the second, check with the third
	
	; TMR1 is greater then, do shift
	; first, shift third number to fourth number
	MOVF Num3, W			; move third number to register W
	MOVWF Num4			; move third number to fourth number

	MOVF sensor3, W
	MOVWF sensor4			; move senosr number with third highest to fourth highest

	; second, shift second number to third number
	MOVF Num2, W			; move second number to register W
	MOVWF Num3			; move decond number to third number

	MOVF sensor2, W
	MOVWF sensor3			; move senosr number with decond highest to third highest

	; finally, shift the new value to the second value
	MOVF TMR1L, W			; move time1 value to register W
	MOVWF Num2			; move the new value to Num2 as the second highest distance

	MOVF sensor, W			; move current sensor number to register W
	MOVWF sensor2			; move current sensor number as the srcond highest one

	RETURN

check_third:
	MOVF Num3, W
	SUBWF TMR1L,W
	BTFSS   STATUS, C
	GOTO check_fourth		; if it is less than the third, check with the fourth

	; TMR1 is greater then, do shift
	; first, shift third number to fourth number
	MOVF Num3, W			; move third number to register W
	MOVWF Num4			; move third number to fourth number

	MOVF sensor3, W
	MOVWF sensor4			; move senosr number with third highest to fourth highest

	; finally, shift the new value to the third value
	MOVF TMR1L, W			; move time1 value to register W
	MOVWF Num3			; move the new value to Num3 as the third highest distance

	MOVF sensor, W			; move current sensor number to register W
	MOVWF sensor3			; move current sensor number as the third highest one

	RETURN

check_fourth:
	MOVF Num4, W
	SUBWF TMR1L,W
	BTFSS   STATUS, C
	RETURN					; if it is less than the fourth, do nothing
	
	; TMR1 is greater then, do shift
	MOVF TMR1L, W			; move time1 value to register W
	MOVWF Num4			; move the new value to Num4 as the fourth highest distance

	MOVF sensor, W			; move current sensor number to register W
	MOVWF sensor4			; move current sensor number as the fourth highest one
	RETURN

print_distances:
	; check if the highest value is zero or not
	MOVF Num1, W
	BTFSC STATUS, Z		; test if zero flag is set and skip if not zero
	RETURN				; if zero, do not print anything

	; if it is not zero print Num1, sensor 1
	GOTO print_num1_sensor1
return1:	
	; check if Num2 equals zero or not
	MOVF Num2, W
	BTFSC STATUS, Z		; test if zero flag is set and skip if not zero
	RETURN				; if zero, do not print anything

	; if it is not zero print Num2, sensor 2
	GOTO print_num2_sensor2
return2:
	; check if Num3 equals zero or not
	MOVF Num3, W
	BTFSC STATUS, Z		; test if zero flag is set and skip if not zero
	RETURN				; if zero, do not print anything

	; if it is not zero print Num3, sensor 3
	GOTO print_num3_sensor3

return3:
	; check if Num4 equals zero or not
	MOVF Num4, W
	BTFSC STATUS, Z		; test if zero flag is set and skip if not zero
	RETURN				; if zero, do not print anything

	; if it is not zero print Num4, sensor 4
	GOTO print_num4_sensor4
return4:
	RETURN

print_num1_sensor1:
	BANKSEL PORTD

	BSF Select, RS			; Select command mode
	movlw 'U'			; move letter 'U' to register w
	call send				; and send code

	MOVF sensor1, W		; print sensor number
	MOVWF Temp
	CALL print_tens		; print the tens digit

	MOVF sensor1, W
	MOVWF Temp
	CALL print_ones		; prints the ones digit

	BSF Select, RS			; Select command mode
	movlw ':'				; move letter ':' to register w
	call send				; and send code

	; now print the distance value in Num1
	;MOVLW D'227'
	MOVF Num1,W
	MOVWF Temp
	CALL print_hunds

	;MOVLW D'227'
	MOVF Num1,W
	MOVWF Temp
	CALL print_tens

	;MOVLW D'227'
	MOVF Num1,W
	MOVWF Temp
	CALL print_ones
	GOTO return1

print_num2_sensor2:
	BANKSEL PORTD

	BSF Select, RS			; Select command mode
	movlw ' '				; move letter space to register w
	call send				; and send code

	BSF Select, RS			; Select command mode
	movlw ' '				; move letter space to register w
	call send				; and send code

	BSF Select, RS			; Select command mode
	movlw 'U'			; move letter 'U' to register w
	call send				; and send code

	MOVF sensor2, W		; print sensor number
	MOVWF Temp
	CALL print_tens		; print the tens digit

	MOVF sensor2, W
	MOVWF Temp
	CALL print_ones		; prints the ones digit

	BSF Select, RS			; Select command mode
	movlw ':'				; move letter ':' to register w
	call send				; and send code

	; now print the distance value in Num1
	;MOVLW D'227'
	MOVF Num2,W
	MOVWF Temp
	CALL print_hunds

	;MOVLW D'227'
	MOVF Num2,W
	MOVWF Temp
	CALL print_tens

	;MOVLW D'227'
	MOVF Num2,W
	MOVWF Temp
	CALL print_ones
	GOTO return2

print_num3_sensor3:
	BANKSEL PORTD

	MOVLW	0xC0		; position to second row
	BCF	Select,RS			; Select command mode
	CALL	send			; and send code

	BSF Select, RS			; Select command mode
	movlw 'U'			; move letter 'U' to register w
	call send				; and send code

	MOVF sensor3, W		; print sensor number
	MOVWF Temp
	CALL print_tens		; print the tens digit

	MOVF sensor3, W
	MOVWF Temp
	CALL print_ones		; prints the ones digit

	BSF Select, RS			; Select command mode
	movlw ':'				; move letter ':' to register w
	call send				; and send code

	; now print the distance value in Num1
	;MOVLW D'227'
	MOVF Num3,W
	MOVWF Temp
	CALL print_hunds

	;MOVLW D'227'
	MOVF Num3,W
	MOVWF Temp
	CALL print_tens

	;MOVLW D'227'
	MOVF Num3,W
	MOVWF Temp
	CALL print_ones
	GOTO return3

print_num4_sensor4:
	BANKSEL PORTD

	BSF Select, RS			; Select command mode
	movlw ' '				; move letter space to register w
	call send				; and send code

	BSF Select, RS			; Select command mode
	movlw ' '				; move letter space to register w
	call send				; and send code

	BSF Select, RS			; Select command mode
	movlw 'U'			; move letter 'U' to register w
	call send				; and send code

	MOVF sensor4, W		; print sensor number
	MOVWF Temp
	CALL print_tens		; print the tens digit

	MOVF sensor4, W
	MOVWF Temp
	CALL print_ones		; prints the ones digit

	BSF Select, RS			; Select command mode
	movlw ':'				; move letter ':' to register w
	call send				; and send code

	; now print the distance value in Num1
	;MOVLW D'227'
	MOVF Num4,W
	MOVWF Temp
	CALL print_hunds

	;MOVLW D'227'
	MOVF Num4,W
	MOVWF Temp
	CALL print_tens

	;MOVLW D'227'
	MOVF Num4,W
	MOVWF Temp
	CALL print_ones

	; make some delay for better vision on the LCD
	MOVLW 100			; move 100 to register w
	CALL xms
	GOTO return4

	END	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;