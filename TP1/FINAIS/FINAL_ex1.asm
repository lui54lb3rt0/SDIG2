/*#######################################
# T1_F1.asm				#
#					#
#	SDIG2 - TURMA 2NA		#
#	1101420	LUIS SILVA		#
#					#
#######################################*/

.include<m128def.inc>
	.cseg
	.org	0x00

config:
	ser	r16		/* set register 1 */
	out	portc,	r16	/* coloca val. de r16 em portc	tudo a 1 */
	out	ddrc,	r16	/* coloca val. de r16 em ddrc	OUTPUTS (LEDS) */
	clr	r16		/* coloca r16 a 0 */
	out	ddra,	r16	/* coloca val. de r16 em ddra	INPUTS (SW) */

sw:
	sbis	pina,	0	/* se = 1 salta */
	jmp	led1
	sbis	pina,	1	/* se = 1 salta */
	jmp	led2
	sbis	pina,	2	/* se = 1 salta */
	jmp	led3
	sbis	pina,	3	/* se = 1 salta */
	jmp	led4
	sbis	pina,	5	/* se = 1 salta */
	jmp	led_off
	jmp	sw

led1:
	ldi	r16,	0b00000000	/* Carrega a palavra de configuração para o registo r16 */
	out	portc,	r16
	jmp	sw

led2:
	ldi	r16,	0b10000001	/* Carrega a palavra de configuração para o registo r16 */
	out	portc,	r16
	jmp	sw

led3:
	ldi	r16,	0b11000011	/* Carrega a palavra de configuração para o registo r16 */
	out	portc,	r16
	jmp	sw

led4:
	ldi	r16,	0b11100111	/* Carrega a palavra de configuração para o registo r16 */
	out	portc,	r16
	jmp	sw

led_off:
	ldi	r16,	0b11111111	/* Carrega a palavra de configuração para o registo r16 */
	out	portc,	r16
	jmp	sw

