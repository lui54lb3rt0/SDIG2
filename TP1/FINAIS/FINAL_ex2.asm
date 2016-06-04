/*#######################################
# T1_F2.asm				#
#					#
#	SDIG2 - TURMA 2NA		#
#	1101420	LUIS SILVA		#
#					#
#######################################*/ 

.include<m128def.inc>

	.def	temp	=	r16

/* Definição dos pinos dos SW's */
	.equ   	sw1	=	0
	.equ   	sw6	=	5

/* Valores para DELAY 500ms */
	.equ   	a	=	150
	.equ   	b	=	150
	.equ   	c	=	118

	.cseg
	.org	0x00


/* Define STACK */

CONFIG:
	ldi		temp,	LOW (RAMEND)
	out		SPL,	temp
	ldi		temp,	HIGH (RAMEND)
	out		SPH,	temp

/* Configuração do Sistema */

INIT:
	ser		temp		/* set register 1 */
	out		portc,	temp	/* coloca val. de r16 em portc	tudo a 1 */
	out		ddrc,	temp	/* coloca val. de r16 em ddrc	OUTPUTS (LEDS) */
	clr		temp		/* coloca r16 a 0 */
	out		ddra,	temp	/* coloca val. de r16 em ddra	INPUTS (SW) */

/* Programa Principal */

START:
	sbis	pina,	sw1	/* sw1 pressionado */
	jmp	ASC		/* se for pressionado salta para o ciclo ASC */
	jmp	START		/* se não volta ao inicio do ciclo para testar novamente o botão */

DESC:
	sec
	ldi	r17,	7		/* Roda palavra de configuração até o registo 17 ficar com o valor 8 */
	ldi	temp,	0b11111110	/* Carrega a palavra de configuração na variavél temp */
	out	portc,	temp
	call	delay
	DESC_AUX:
		rol	temp
		out	portc,	temp
		call	delay
		dec	r17	
		brne	DESC_AUX
		sbis	pina,	sw1
		jmp	ASC
		jmp	G_O	


ASC:
	sec
	ldi	r17,	7
	ldi	temp,	0b01111111
	out	portc,	temp
	call	delay
	ASC_AUX:
		ror	temp
		out	portc,	temp
		call	delay
		dec	r17
		brne	ASC_AUX
		sbis	pina,	sw6
		jmp	DESC
		jmp	G_O

G_O:
	ldi	r17,	3			/*  */
	G_O_AUX:				/*  */
		ldi	temp,	0b11100111	
		out	portc,	temp
		call	delay
		ldi	temp,	0b11111111
		out	portc,	temp
		call	delay
		dec	r17
		brne	G_O_AUX
		jmp	fim	

fim:
	ldi	temp,	0b11111111		/* carrega a palavra de configuração com todos os leds apagados para a variavél temp (registo r16) */
	out	portc,	temp			/* coloca no PORTC a palavra de configuração carregada na variavél temp */
	jmp	START

/* ROTINA DE DELAY (VALORES DE a, b E c PARA 500ms) */

delay:
	push	r18
	push	r19
	push	r20

		ldi	r20,	c
	ciclo0:
		ldi	r19,	b
	ciclo1:
		ldi	r18,	a
	ciclo2:
		dec	r18
		brne	ciclo2	
	
		dec	r19
		brne	ciclo1

		dec	r20
		brne	ciclo0

	pop	r20
	pop	r19
	pop	r18

	ret

