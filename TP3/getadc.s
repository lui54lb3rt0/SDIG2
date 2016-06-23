#include	<avr/io.h>
#define __SFR_OFFSET 0


.extern LeituraL, LeituraH
.global getadc

getadc:
	push	r17
	clr	r19
	clr	r18
	sbi	ADCSRA,6

a:	sbic	ADCSRA,6
	brne	a

	ldi 	r17, 4
c1:
	in		r25, ADCL
	in		r24, ADCH

	add   	r19, r25			//LeituraL
	adc   	r18, r24			//LeituraH

	dec		r17
	cpi		r17,0
	brne 	c1

ldi 	r17, 2
c2:
	rol		r19
	rol		r18
	dec		r17
	cpi		r17,0
	brne 	c2
	

	mov		r25, r18
	mov		r24, r19

	pop	r17

ret
