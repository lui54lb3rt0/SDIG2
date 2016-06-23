#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

#define A 1
#define B 2
#define C 3

//volatile fir_yn[6] = {0,0,0,0,0,0};
volatile int hi[6]= {0.010268, 0.117885, 0.371847, 0.371847, 0.117885, 0.010268};
unsigned char yn[6];

char transmit_buffer[30];

int x,i=0,x_act;
uint8_t w_cks, mode=0;
volatile uint8_t t;
uint8_t LeituraH, LeituraL;

typedef struct USARTRX
{
	char receiver_buffer;
	unsigned char status;
	unsigned char receive 	: 1;
	unsigned char error	: 1;

}USARTRX_st;

volatile USARTRX_st rxUSART = {0,0,0,0};

void init(void);
int getadc(void);
int ler_ad(void);
void send_message(char *buffer);

int main()
{
	init();
	while(1)
	{
		if (t==1)
		{
			x_act=getadc();
			switch (mode)
			{
				case A:
				sprintf(transmit_buffer,"A\t%d\t",x);
				cks(transmit_buffer);
				sprintf(transmit_buffer,"A\t%d\t%c\r\n",x,w_cks);
				break;
				case B:
				sprintf(transmit_buffer,"B\t%d\t",yn);		//valor filtrado mudar variavel
				cks(transmit_buffer);
				sprintf(transmit_buffer,"B\t%d\t%c\r\n",x,w_cks);
				break;
				case C:
				sprintf(transmit_buffer,"C\t%d\t%d\t",x,yn);	//valor filtrado
				cks(transmit_buffer);
				sprintf(transmit_buffer,"C\t%d\t%d\t%c\r\n",x,x,cks());
				break;
				default:
				sprintf(transmit_buffer,"%d\r\n",x);	//valor filtrado
				break;
			}
			//			sprintf(transmit_buffer,"%d\r\n",x);
			send_message(transmit_buffer);
			t=0;
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
					case 'A':
					case 'a':
					mode = A;
					break;

					case 'B':
					case 'b':
					mode = B;
					break;

					case 'C':
					case 'c':
					mode = C;
					break;

				}
				//	sprintf(transmit_buffer,"C\t%d\t%d\t%c\r\n",x,x,w_cks);
				//	send_message(transmit_buffer);
			}
			rxUSART.receive = 0;
		}
		
	}
}


void cks(char * buffer)
{
	uint8_t i=0;
	while(buffer[i]!='\0')
	{
		w_cks += buffer[i];
		i++;
	}
	return w_cks;
}

void calc_filtro(void)
{
	// Codigo que calcula o valor filtrado
	
	unsigned char i=0, n=5;

	for(i=0; i = n; i++)			//	Mover para a esquerda os valores no vetor
	{
		x[i]=x[n+i];
	}
	
	x[5]=x_act;
	yn=0;
	
	for(i=0; i = n; i++)			//	Mover para a esquerda os valores no vetor
	{
		yn=yn+hi[i]*x[5-i];
		
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
	UCSR0B = (1<<RXCIE0) | (1<<RXEN0) | (1<<TXEN0);
	UCSR0C = (1<<UCSZ01) | (1<<UCSZ00);

	ADMUX = (1<<REFS0); // 0b00000010; //
	ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0); // 0b10000111; //

	sei();
}


ISR(TIMER0_COMP_vect)
{
	t=1;
}

ISR (USART0_RX_vect)
{
	rxUSART.status = UCSR0A;
	if(rxUSART.status & ((1<<FE0) | (1<<DOR0) | (1<<UPE0)))
	rxUSART.error = 1;
	rxUSART.receiver_buffer = UDR0;
	rxUSART.receive = 1;
}


