/*
 * Project_1.c
 *
 *  Created on: Sep 16, 2023
 *      Author: Mostafa Ahmed Abd elslam
 *      Diploma 81
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

unsigned char sec1,sec2,min1,min2,hours1,hours2;

void DISPLAY_SIX_7_SEGMENT() {
	PORTA = (1<<PA5);//SEC1
	PORTC = (sec1 & 0x0F) | (PORTC & 0xF0);
	_delay_us(2);

	PORTA = (1<<PA4);//SEC2
	PORTC = (sec2 & 0x0F) | (PORTC & 0xF0);
	_delay_us(2);

	PORTA = (1<<PA3);//MIN1
	PORTC = (min1 & 0x0F) | (PORTC & 0xF0);
	_delay_us(2);

	PORTA = (1<<PA2);//MIN2
	PORTC = (min2 & 0x0F) | (PORTC & 0xF0);
	_delay_us(2);

	PORTA = (1<<PA1);//HOURS1
	PORTC = (hours1 & 0x0F) | (PORTC & 0xF0);
	_delay_us(2);

	PORTA = (1<<PA0);//HOURS2
	PORTC = (hours2 & 0x0F) | (PORTC & 0xF0);
	_delay_us(2);
}

ISR(TIMER1_COMPA_vect){
sec1++;
 if(sec1==10){
sec1=0;
sec2++;
 if(sec2==6){
sec2=0;
min1++;
 if(min1==10)
{
min1=0;
min2++;
 if(min2==6)
{
min2=0;
hours1++;
 if(hours1==10)
{
hours1=0;
hours2++;
 if(hours2==6)
{
TCCR1A=0;
TCCR1B=0;
return;
	        }
	     }
	  }
    }
  }
 }
}
ISR(INT0_vect){
	//reset the stop watch...
	sec1 = 0, sec2 = 0, min1 = 0, min2 = 0, hours1 = 0, hours2 = 0;
	TCNT1 = 0;
	if(!(TCCR1A)){
		TCCR1A = (1<<FOC1A);
		TCCR1B = (1<<WGM12) | (1<<CS11) | (1<<CS10);
	}
}

ISR(INT1_vect){
	// pause the timer
	TCCR1A = 0;
	TCCR1B = 0;
}

ISR(INT2_vect){
	// resume the timer
	TCCR1A = (1<<FOC1A);
	TCCR1B = (1<<WGM12) | (1<<CS11) | (1<<CS10);
}

void timer1(){
	/*  Description :
		 * 	TCCR1A -  :
		 * 		Using Channel A in TIM1 ,  Bit 3 – FOC1A: =1
		 * 	TCCR1B :
		 * 		WGM12 =1 -  Mode 4 CTC OCR1A
		 * 		CS10,CS12 =1  - clkI/O/1024 (From prescaler)
		 * 	OCR1A -Output Compare Register 1 A : =977
		 * 	TIMSK  Timer/Counter Interrupt Mask Register:
		 * 		OCIE1A =977
		 */
	TCCR1A = (1<<FOC1A);							// NO PWM
	TCCR1B = (1 << WGM12)|(1 << CS10) | (1 << CS12); 	//CTC MODE, PRESCALE = 1024
	TCNT1 = 0;
	OCR1A = 977;
	TIMSK |= (1<<OCIE1A);	// Output Compare A Match Interrupt Enable

}

void interrupt_int0(){
	MCUCR |= (1<<ISC01);				// FALLING EDGE
	GICR |= 1<<INT0;
}

void interrupt_int1(){
	MCUCR |= (1<<ISC11)|(1<<ISC10);		// RISING EDGE
	GICR |= 1<<INT1;
}

void interrupt_int2(){
	MCUCSR &= ~(1<<ISC2);				// FALLING EDGE
	GICR |= 1<<INT2;
}

int main(){
	DDRC |= 0x0F;						//PC0 - PC3 ARE OUTPUT
	DDRA |= 0x3F;						//PA0 - PA5 ARE OUTPUT
	DDRD &= ~((1<<PD2) | (1<<PD3));		// PD2 , PD3 ARE INPUT
	DDRB &= ~(1<<PB2);					// PB2 IS INPUT

	/* ACTIVATING THE INTERNAL PULL-UP FOR PD2 AND PB2 */
	PORTD |= 1<<PD2;
	PORTB |= 1<<PB2;
	PORTC &= (0xF0);  /* ZERO IS THE INITIAL VALUE FOR THE SIX 7-SEGMENTS. */

	SREG |= 1<<7;
	timer1();
	interrupt_int0();
	interrupt_int1();
	interrupt_int2();

	while(1){
		DISPLAY_SIX_7_SEGMENT();
	}
}


