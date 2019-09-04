/***********************************
	Trabalho		:	T3F1
	Objectivo		:	Introducao a programacao em C
						Programa simples de leitura de entradas e atualizacao de saidas
	Funcionamento	:	Le os switches e se algum premido troca o valor da saida correspondente
	Linguagem		:	C
	Ano				:	2015/2016
	Unidade Cur.	:	Sistemas Digitais 2
	Semestre		:	1
	Autor			:	Luís Silva
	E-mail			:	1101420@isep.ipp.pt
************************************/


/***********************************
	Constants & Macro Definition
************************************/
#include <avr/io.h>
#include <avr/interrupt.h>

#define	sw1	0b00011110
#define sw2	0b00011101
#define sw3	0b00011011
#define sw4	0b00010111
#define sw5	0b00001111
#define left 1 //	Sentido dos ponteiros do rel�gio
#define right 0 //	Sentido inversos dos ponteiros do rel�gio
#define PWM(val) ((255*val)/100); //	% Duty Cicle

/***********************************
	Global Variables & Structures
************************************/

//vetor com saidas para display 7 segmentos
const unsigned char segments[14]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0xFF,0xBF,0xC2,0xC7};
/* Segment [10] = blank
 Segment [11] = -
 Segment [12] = G
 Segment [13] = L*/
unsigned char	ndisplays	=	4; //Display
unsigned char	DispValue[4]; //Display
unsigned char	DispSelector=	3;
unsigned char s=left;
volatile unsigned char a=0;
volatile unsigned char c=100;
unsigned char sw, OCR_AUX;

void conta500ms(void){
	c=100;
	while (c!=0){ }
	return;
}
void conta250ms(void){
	c=50;
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

ISR(TIMER1_OVF_vect)
{
    //covf++;    
}

ISR(TIMER1_CAPT_vect)
{

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

void inic(void)
{
	DDRA	=	0b11000000;
	PORTA	=	0b11000000;//PortA    -switch

	DDRB	=	0b11100000;
	PORTB	=	0b00100000;//PortB    -motor
	
	DDRC	=	0xFF;//PortC    -display


	OCR0	=	77; //Timer 0 Configuration
	TCCR0	=	0b00001111; //Timer 0 Configuration
//	TIMSK	=	0b00000010; //Timer 0 Configuration


	TCCR1B = (1<< CS10);//Timer 1 Configuration

	TIMSK = (1<<TICIE1) | (1<<TOIE1) | (1<<OCIE0);


	OCR2	=	PWM(0);
	TCCR2	=	0b01101101;    //Timer 2 Configuration     TCCR2 = (1<<WGM20)|(1<<COM21)|(0<<CS22)|(1<<CS21)|(1<<CS20);

	sei();					//ativar interrupções
}
int main(void)
{
    int a=0;
	inic();
	Disp(a);
	
	while(1)
	{
		sw = PINA & 0b00011111;
		if ((s==right)&(a!=0)) DispValue[1] = 11;
		else DispValue[1] = 10; //não mostra nada no segundo display
		
		switch (sw)
		{
			case sw1:
			a=25;
            OCR2	=	PWM(a);		//PWM 25%
			Disp(a);
			break;
			case sw2:
			a=50;
            OCR2	=	PWM(a);		//PWM 50%
			Disp(a);
			break;
			case sw3:
			a=90;
            OCR2	=	PWM(a);		//PWM 90%
			Disp(a);
			break;
			case sw4:
			OCR2	=	PWM(0);			//PWM 0%
			
            conta250ms();
			Disp(0);
			conta250ms();
            Disp(a);
			OCR2	=	PWM(a);		//PWM à %% anterior
            //Inverter sentido do motor
			if (s==left){
				PORTB	=	0b01000000;
				s=right;
                
			}
			else{
				PORTB	=	0b00100000;
				s=left;
			}
			break;
		}
	}
}
