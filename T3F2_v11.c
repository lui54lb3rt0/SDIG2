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


/***********************************
	Constants & Macro Definition
************************************/
#include <avr/io.h>
#include <avr/interrupt.h>

#define sw_mask 0b00011111
#define	sw1	0b00011110
#define sw2	0b00011101
#define sw3	0b00011011
#define sw4	0b00010111
#define sw5	0b00001111

#define free	0b00011111

#define left 1 //	Sentido dos ponteiros do rel�gio
#define right 0 //	Sentido inversos dos ponteiros do rel�gio
#define G 0    
#define L 1  

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

#define PWM(val) ((255*val)/100); //	% Duty Cicle
volatile unsigned char a = 0, flag5ms = 0;
volatile unsigned char c = 100;
signed char dutty = 0; //variavel armazenamento do valor calculado dutty cicle
unsigned char sw, s=left, OCR_AUX, mode = G;
volatile uint8_t flagcapt = 0;
volatile int c_ovf;
volatile unsigned int valor1, valor2, valor3, period, positive_pulse;

void conta500ms(void){
	c=100;
	while (c!=0){ }
	return;
}

void conta5ms(void){
	c=1;
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
    c_ovf++;    
}

/***********************************
    Timer 1 Interrupt - Capture
************************************/
ISR(TIMER1_CAPT_vect)
{

	if (flagcapt == 0)
    {
	    valor1 = ICR1;
	    c_ovf = 0;
	    TCCR1B ^= 0b01000000;        //troca flanco ascendente -> descendente
	    TIFR |=(1<<ICF1);            //limpa flag de interrupcao
    }
    
    if (flagcapt == 1)
    {
	    valor2 = ICR1;
	    TCCR1B ^= 0b01000000;        //troca flanco descendente -> ascendente
	    TIFR |=(1<<ICF1);            //limpa flag de interrupcao
    }
    
    if (flagcapt == 2)
    {
    	valor3 = ICR1;
    }
    
    flagcapt++;
}


void Disp(unsigned char value)
{
	unsigned char M =(value/1000)%10;
	unsigned char C =(value/100)%10;
	unsigned char D =(value/10)%10;
	unsigned char U =(value/1)%10;
	if (M == 0) DispValue[0] = 10;
	if (C == 0) DispValue[1] = 10;
	if (D > 0) DispValue[2] = D;
	else DispValue[2] = 10;
	if (U >= 0) DispValue[3] = U;
    if (mode == G) DispValue[0] = 12;
    else if (mode == L ) 
    {
		DispValue[0] = 13;	  
	}
}



void inic(void)
{
	DDRA	=	0b11000000;//configura PORTA - SWITCHES
	PORTA	=	0b1100000;//pinos 6,7 saídas e restantes entradas

	DDRB	=	0b11100000;//pinos 5,6,7 saídas e restantes entradas - CONTROLO MOTOR
	PORTB	=	0b00100000;//configura PORTB - sentido rotação horário
	
	DDRC	=	0xFF;//configura PORTC - DISPLAYS


	OCR0	=	77; //Timer 0 Configuration
	TCCR0	=	(1<<WGM01)|(1<<CS02)|(1<<CS01)|(1<<CS00) ; //0b00001111; //Timer 0 Configuration
//	TIMSK	=	0b00000010; //TIMSK = (1<<OCIE0)|(1<<TICIE1)|(1<<TOIE1); //Timer 0 Configuration, interrupçao TCO por compare match

	//Timer 1 Configuration
	TCCR1A = 0x0;
	TCCR1B = (1<<ICNC1)|(1<<ICES1)|(0<<WGM13)|(0<<WGM12)|(0<<CS12)|(0<<CS11)|(1<<CS10);
	//filtro ativo, flanco ascendente
	//Waveform generation normal
	//Prescaler = 1 (no prescaling)
	TCCR1C = 0x0;

	TIMSK = (1<<TICIE1) | (1<<TOIE1) | (1<<OCIE0);

	OCR2	=	PWM(0);
	TCCR2 = (1<<WGM20)|(1<<COM21)|(1<<WGM21)|(1<<CS22)|(1<<CS20); //	0b01101101   Timer 2 Configuration     

	sei();					//ativar interrupções
}


int main(void)
{
    unsigned char b=0;
	inic();
	Disp(b);
	uint8_t f1=0;
	
	while(1)
	{
		//if ((s==right) && (b!=0)) DispValue[1] = 11; //mostra o sinal menos no segundo display
		if (s==right) DispValue[1] = 11; //mostra o sinal menos no segundo display
		else DispValue[1] = 10; //não mostra nada no segundo display
        
        if (flagcapt == 3)
        {
	        if (c_ovf == 1)
	        period = (65535 - valor1) + valor3;
	        
	        if (c_ovf == 0)
	        period = valor3 - valor1;
	        
	        if (valor2 > valor1)
	        { positive_pulse = valor2 - valor1;}
	        
	        else
	        { positive_pulse = (65535 - valor1) + valor2; }
	        
	        dutty = (signed char)((((float)((float)positive_pulse/(float)period))*100)+0.5);
	        //dutty=77;
	        //Warning: integer -> signed char
	        //add 0.5 to prevent flickring of least significative digit
	        flagcapt = 0;
	        c_ovf = 0;
        }

		if (flagcapt > 3) flagcapt = 0;

        sw = PINA & sw_mask;
		switch (sw)
		{
			case sw1:
				conta5ms();
				if ((PINA & sw_mask)==sw1)
				{
				    b=25;
	                OCR2	=	PWM(b);		//PWM 25%
					if (mode == G) Disp(b);
					else if (mode == L) Disp(dutty);
				}
		    	break;
			case sw2:
				conta5ms();
				if ((PINA & sw_mask)==sw2)
				{
			    	b=50;
	                OCR2	=	PWM(b);		//PWM 50%
				    if (mode == G) Disp(b);
				    else if (mode == L) Disp(dutty);
				}
			    break;
			case sw3:
				conta5ms();
				if ((PINA & sw_mask)==sw3)
				{			
				    b=90;
	                OCR2	=	PWM(b);		//PWM 90%
				    if (mode == G) Disp(b);
				    else if (mode == L) Disp(dutty);
				}
			    break;
			case sw4:
				conta5ms();
				if ( ((PINA & sw_mask)==sw4) && (f1==0) )
				{
					f1=1;
				    OCR2 =	PWM(0);			//PWM 0%
				    Disp(0);//DispValue[4] = 0;////altera
				    conta500ms();
	                Disp(b);
	                //Inverter sentido do motor
				    if (s==left){
					    PORTB	=	0b01000000;
					    s=right;
	                
				    }
				    else if(s==right){
					    PORTB	=	0b00100000;
					    s=left;
				    }
				    OCR2 =	PWM(b);		//PWM à %% anterior
				}
			    break;
            case sw5:
            	conta5ms();
				if ( ((PINA & sw_mask)==sw5) && (f1==0) )
				//if ((PINA & sw_mask)==sw5)
				{
					f1=1;
	                if (mode == G) 
	            	{
						Disp(dutty);
	            		mode = L;
	            	}
	                else if (mode == L) 
	                {
	                	Disp(b);//Disp(a); //0
	                	mode = G;
					}
				//f1=1;
				}
                break; 
                
			case free:	f1=0;
						break;
             
		} 
		if (mode == L) Disp(dutty);
	}
}