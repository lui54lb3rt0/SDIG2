/***********************************
	Trabalho		:	T3F1
	Objectivo		:	Introducao a programacao em C
						Programa simples de leitura de entradas e atualizacao de saidas
	Funcionamento	:	Le os switches e se algum premido troca o valor da saida correspondente
	Linguagem		:	C
	Ano				:	2015/2016
	Unidade Cur.	:	Sistemas Digitais 2
	Semestre		:	2
	Autor			:	Luís Silva
	E-mail			:	1101420@isep.ipp.pt
************************************/


#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

typedef struct USARTRX
{
	char receiver_buffer;
	unsigned char status;
	unsigned char receive 	: 1;
	unsigned char error	: 1;

}USARTRX_st;

volatile USARTRX_st rxUSART = {0,0,0,0};
char transmit_buffer[10];

#define	sw1	0b00011110
#define sw2	0b00011101
#define sw3	0b00011011
#define sw4	0b00010111
#define sw5	0b00001111
#define left 1
#define right 0

const unsigned char segments[14]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0xFF,0xBF,0xC2,0xC7};
/* 
	Segment [10] = blank
	Segment [11] = -
	Segment [12] = G
	Segment [13] = L
*/
 
unsigned char	ndisplays	=	4;
unsigned char	DispValue[4];
unsigned char	DispSelector=	3;

#define PWM(val) ((255*val)/100);
unsigned char s=left;
volatile unsigned char a=0;
volatile unsigned char c=100;
unsigned char sw, OCR_AUX;

void send_message(char * buffer)
{
	unsigned char i=0;
	while(buffer[i]!='\0')
	{
		while((UCSR1A & 1<<UDRE1)==0);
		UDR1=buffer[i];
		i++;
	}
}

void conta500ms(void){
	c=100;
	while (c!=0){ }
	return;
}

void MostradorUpdate()
{
	switch (a)
	{
		case 1 :
		DispSelector = 3;
		break;
		case 2 :
		DispSelector = 2;
		break;
		case 3 :
		DispSelector = 1;
		//a=0;
		break;
		case 4 :
		DispSelector = 0;
		a=0;
		break;
	
	}
	PORTA = DispSelector << 6;
	PORTC = segments[DispValue[DispSelector]];
}

ISR(TIMER0_COMP_vect)
{
	if (c>0) c--;
	a++;
	MostradorUpdate();
}

ISR (USART1_RX_vect)
{
	rxUSART.status = UCSR1A;
	if(rxUSART.status & ((1<<FE1) | (1<<DOR1) | (1<<UPE1)))
		rxUSART.error = 1;
	rxUSART.receiver_buffer = UDR1;
	rxUSART.receive = 1;
}

void Disp(unsigned char value)
{
	unsigned char M =(value/1000)%10;
	unsigned char C =(value/100)%10;
	unsigned char D =(value/10)%10;
	unsigned char U =(value/1)%10;
	if (M==0) DispValue[0] = 10;
	if (C==0) DispValue[1] = 10;
	if (D>0) DispValue[2] = D;
	else DispValue[2] = 10;
	if (U>=0) DispValue[3] = U;
}

void init(void)
{
	UBRR1H=0;
	UBRR1L=51;

	UCSR1A =0;
	UCSR1B = (1<<RXCIE1) | (1<<RXEN1) | (1<<TXEN1);
	UCSR1C = (1<<UCSZ11) | (1<<UCSZ10);

	DDRA	=	0b11000000;
	PORTA	=	0b11000000;

	DDRB	=	0b11100000;
	PORTB	=	0b00100000;
	
	DDRC	=	0xFF;

	OCR0	=	77;
	TCCR0	=	(1<<WGM01) | (1<<CS02) | (1<<CS01) | (1<<CS00); // 0b00001111; //
	TIMSK	=	(1<<OCIE0); //0b00000010; //



	OCR2	=	PWM(0);

	TCCR2	=	(1<<WGM20) | (1<<COM21) | (1<<WGM21) | (1<<CS22) | (1<<CS20); // 0b01101101; //	
	sei();					//ativar interrupções
}


int main(void)
{
	init();
	Disp(0);
	
	while(1)
	{
		sw = PINA & 0b00011111;
		if (s==right) DispValue[1] = 11;
		else DispValue[1] = 10;
		
		switch (sw)
		{
			case sw1:
				OCR2	=	PWM(25);		//PWM 25%
				Disp(25);
			break;
			case sw2:
				OCR2	=	PWM(50);		//PWM 50%
				Disp(50);
			break;
			case sw3:
				OCR2	=	PWM(90);		//PWM 90%
				Disp(90);
			break;
			case sw4:
				OCR_AUX	=	OCR2;
				OCR2	=	PWM(0);			//PWM 0%
				conta500ms();
				//Inverter sentido do motor
				if (s==left){
					PORTB	=	0b01000000;
					
					s=right;
				}
				else{
					PORTB	=	0b00100000;
					
					s=left;
				}
				OCR2	=	OCR_AUX;		//PWM 0%
			break;
			case sw5:
				OCR2	=	PWM(90);		//PWM 90%
				Disp(90);
			break;
		}

		if (rxUSART.receive == 1)
		{
			if (rxUSART.error == 1)
			{
				rxUSART.error = 0;
			}
			else
			{
				switch(rxUSART.receiver_buffer)
				{
					case 'P':
					case 'p':
						OCR2 = PWM(0);
						DispValue[1]=10;
						Disp(0);
						sprintf(transmit_buffer,"tecla: %c \r\n",rxUSART.receiver_buffer);
					break;

					case 'L':
					case 'l':
						sprintf(transmit_buffer,"tecla: %c \r\n",rxUSART.receiver_buffer);
					break;

					case '1':
						OCR2	=	PWM(25);		//PWM 25%
						Disp(25);
						sprintf(transmit_buffer,"tecla: %c \r\n",rxUSART.receiver_buffer);
					break;

					case '2':
						OCR2	=	PWM(50);		//PWM 50%
						Disp(50);
						sprintf(transmit_buffer,"tecla: %c \r\n",rxUSART.receiver_buffer);
					break;

					case '3':
						OCR2	=	PWM(90);		//PWM 90%
						Disp(90);
						sprintf(transmit_buffer,"tecla: %c \r\n",rxUSART.receiver_buffer);
					break;

					default:
						sprintf(transmit_buffer,"error");
					break;
				}
				
				send_message(transmit_buffer);
			}
			rxUSART.receive = 0;
		}
	}
}
