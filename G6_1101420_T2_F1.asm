/*#####################################
# T2_F1.asm
#
#	SDIG2 - TURMA 2NA
#	1101420	LUIS SILVA
#   
#####################################*/ 

.include <m128def.inc>

.def	temp			=	r16			
.def	aux				=	r17
.def	flagdot			=	r18
.def 	conta500ms 		=	r20
.def	contaciclos		=	r21
.def	contasegundos	=	r22
.def	REGESTADO		=	r23
//	DEFINE INTERRUPTS
.equ 	timecgf		=	(1<<WGM01)|(1<<CS02)|(1<<CS00)
.equ	setTIMSK	=	(1<<OCIE0)
.equ	setEICRA	=	(1<<ISC11)|(1<<ISC01)	//	0b00001010	//
.equ	setEIMSK	=	(1<<INT1)|(1<<INT0)		//	0b00000011	//
.equ	OCR		=	249
// DEFINE STATUS CONDITIONS
.equ	READY	=	0
.equ	RUNNING	=	1
.equ	PAUSE	=	2
.equ	FINISH	=	3
.equ	ERROR	=	4

.cseg
.org	0x00					/* inicio do programa */
jmp		main

.cseg
.org	INT0addr				/* interrupção 0 endereço 0x0002 (start) */
jmp		start

.cseg
.org	INT1addr				/* interrupção 1 endereço 0x0004 (stop) */
jmp		stop

.cseg
.org	0x001E					/* interrupção de TIMER0 por comparação endereço 0x001E */
jmp		timer

.cseg
.org	0x46

init:
	ldi 	temp,	0b11000000		/* PD.6 e PD.7 saida de dados e com valor 1 para utilizar o DISPLAY da direita */
	out 	ddrd,	temp
	out 	portd, 	temp

	ldi 	temp,	0b11000000		/* PC.0 e PC.7 saida de dados para o DISPLAY */
	out 	ddrd,	temp
	out 	portd, 	temp

	ser		temp
	out 	ddrc, 	temp			/* Configura DDRC | todos os pinos como saidas | palavra de configuração 0b11111111 */
	ldi		temp,	0xc0
	out		portc,	temp

	ldi		temp,	OCR
	out		OCR0,	temp

	ldi		temp,	setEICRA
	sts		eicra,	temp

	ldi		temp,	setEIMSK
	out		eimsk,	temp

	ldi		temp,	setTIMSK
	out		timsk,	temp

	//Configura o timer (Modo CTC, OC0 off, Prescaler=128)
	ldi temp,	timecgf
	out tccr0,	temp
	
	//carrega as condições iniciais do programa
	ldi		conta500ms, 	250
	ldi		contaciclos,	2
	ldi 	contasegundos,	0
	ldi		REGESTADO,		READY
	
	sei								/* coloca o registo da flag I a 1 em SREG	*/
	ret

main:
	//Stack Init
	ldi		temp,	low(ramend)
	out 	spl,	temp
	ldi 	temp, 	high(ramend)
	out 	sph,	temp
	
	call 	init
	
fim:
	jmp		fim			//<=> while(1);

start:
	cpi		REGESTADO,		READY
	brne	fimint0
	ldi		REGESTADO,		RUNNING	
	ldi 	contasegundos,	9
fimint0:
	reti

stop:
	cpi		REGESTADO,		RUNNING
	brne	CONTINUA
	ldi		REGESTADO,		PAUSE
	ldi		conta500ms,		250
	ldi		contaciclos,	2
	ldi		aux,			10			/* tempo do stop em segundos 2x o ciclo = 2 */
	cpi		REGESTADO,		PAUSE
	breq	fimint1
	CONTINUA:
		cpi		REGESTADO,	PAUSE
		brne	fimint1
		ldi		REGESTADO,	RUNNING
		cpi		REGESTADO,	RUNNING
		breq	fimint1
fimint1:
	reti
	
timer:
		cpi		REGESTADO,		RUNNING
		brne	TIMER_PAUSE
		dec		conta500ms
		cpi 	conta500ms,		0
		brne	ENDTIMER
		ldi		conta500ms,		250
		dec		contaciclos
		cpi		contaciclos,	0
		brne	ENDTIMER
		ldi		contaciclos,	2
		dec		contasegundos	
		cpi		contasegundos,	0
		brne	ENDTIMER
		cpi		REGESTADO,		RUNNING
		brne	ENDTIMER
		ldi		REGESTADO, 		READY
		jmp		ENDTIMER
		
	TIMER_PAUSE:
			cpi		REGESTADO,		ERROR
			breq	decr
			cpi		REGESTADO,		PAUSE
			brne	ENDTIMER				/* se diferente de PAUSE vai para ENDTIMER */
	decr:
			dec		conta500ms
			cpi 	conta500ms,		0
			brne	ENDTIMER				/* se diferente de 0 vai para ENDTIMER */
			ldi		conta500ms,		250
			ldi		flagdot,		1
			dec		contaciclos
			cpi		contaciclos,	0
			brne	ENDTIMER				/* se diferente de 0 vai para ENDTIMER */
			ldi		contaciclos,	2
			ldi		flagdot,		0
			dec		aux	
			cpi		aux,			0
			brne	ENDTIMER				/* se diferente de 0 vai para ENDTIMER */
			cpi		REGESTADO,		PAUSE
			brne	FIMERRO					/* se diferente de PAUSE vai para FIMERRO */
			ldi		REGESTADO, 		ERROR
			ldi		conta500ms, 	250
			ldi		contaciclos,	2
			ldi 	aux,			3
			jmp		ENDTIMER
	FIMERRO:
			ldi		REGESTADO, 		READY
			ldi		contasegundos, 	0

ENDTIMER:
		call	DISPLAY
		RETI

		
DISPLAY:
	cpi		REGESTADO,	ERROR
	ldi		temp,		0x86
	breq	output
	ldi		zh, 		high(tabela<<1)
	ldi		zl,			low(tabela<<1)
	add		zl,			contasegundos		/* Valor a colocar no display posição da tabela com o valor registo contaseguntos */
	clr		temp
	adc		zh,			temp
	lpm		temp,		z
	
	output:
		out		portc,		temp
		cpi		REGESTADO,	PAUSE
		brne	fim_DISPLAY
		cpi		flagdot,	0
		breq 	on
		sbi		portc, 		7
		jmp		fim_DISPLAY
	on:
		cbi		portc, 		7
fim_DISPLAY:
	RET

tabela:
	.db 0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x90		/* 0 1 2 3 4 5 6 7 8 9 */