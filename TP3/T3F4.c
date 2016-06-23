/***********************************
	Trabalho		:	T3F4
	Objectivo		:	Introducao a programacao em C
						
	Funcionamento	:	Programa para leitura de ADC e transmissão de dados via USART0
	Linguagem		:	C
	Ficheiro ASM	:	getadc.asm
	Ano				:	2015/2016
	Unidade Cur.	:	Sistemas Digitais 2
	Semestre		:	3
	Autor			:	Luís Silva
	E-mail			:	1101420@isep.ipp.pt
************************************/


#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

char transmit_buffer[10];

uint16_t a;
volatile uint8_t t;
uint8_t LeituraH, LeituraL;

void init(void);
int getadc(void);
void send_message(char *buffer);

int main()
{
	init();
	while(1)
	{
		if (t==1)
		{
			sprintf(transmit_buffer,"%d\r\n",getadc());
			send_message(transmit_buffer);
			t=0;
		}
	}
}

void send_message(char * buffer)
{
	uint8_t i=0;
	while(buffer[i]!='\0')
	{
		while((UCSR0A & 1<<UDRE0)==0);
		UDR0=buffer[i];
		i++;
	}
}

void init(void)
{
	OCR0	=	156;
	TCCR0	=	(1<<WGM01) | (1<<CS02) | (1<<CS01) | (1<<CS00); // 0b00001111; //
	TIMSK	=	(1<<OCIE0); //0b00000010; //
	
	UBRR0H=0;
	UBRR0L=51;

	UCSR0A =0;
	UCSR0B = (1<<TXEN0);
	UCSR0C = (1<<UCSZ01) | (1<<UCSZ00);

	ADMUX = (1<<REFS0); // 0b00000010; //
	ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0); // 0b10000111; //
	
	sei();
}

ISR(TIMER0_COMP_vect)
{
	t=1;
}
