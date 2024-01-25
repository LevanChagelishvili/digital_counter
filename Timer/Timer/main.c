//#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <util/atomic.h>

#define CTC_OVERFLOW ((F_CPU / 1000) / 80)

#define control_bus PORTB
#define controlbus_direction DDRB
#define data_bus PORTB
#define databus_direction DDRB

#define RS PB4
#define EN PB5
#define D4 PB0
#define D5 PB1
#define D6 PB2
#define D7 PB3

volatile uint32_t timer1_millsec;  

void LCD_Write_Comand(unsigned char);
void LCD_DataWrite(unsigned char);
void LCD_Init();
void LCD_Integer(uint32_t);
void LCD_Disp(const char *);
void LCD_setCursor(unsigned char, unsigned char);
void Timer_Init();
ISR (TIMER1_COMPA_vect);
uint32_t millsec();
void LCD_Clear();
void LCD_default();

int main(void)
{

	LCD_Init();
	Timer_Init();
	LCD_default();
	
	DDRD &= ~(1 << PIND0);
	
	sei();
	
	
	while(1)
	{
		
		if (!(PIND & (1 << PIND0)))
		{
			uint32_t time = millsec();
			uint32_t wutebi = time / 60000;
			 time = time - (wutebi * 60000);
			
			for (uint8_t counter = 0 ; counter < 6; counter++)
			{
				LCD_setCursor(7-counter,1);
				
				if(7-counter == 4)
				{
					LCD_Disp(".");
				}
				else
				{
					uint32_t nashti = time % 10 ;
					time = time / 10;
					LCD_Integer(nashti);
				}
			}
				LCD_setCursor(1,1);
				LCD_Integer(wutebi%10);
				LCD_setCursor(0,1);
				LCD_Integer((wutebi/10));	
			
			if(millsec() > 180000){
				LCD_Clear();
				_delay_us(2);
				LCD_default();
				cli();
			}
		}
		if (!(PIND & (1 << PIND1)))
		{
			LCD_Clear();
			_delay_us(2);
			LCD_default();
		}
		sei();
	}
	
}

void LCD_Write_Comand(unsigned char s)
{
			
	if(s & 0x80) data_bus |= (1 << D7); 
	else data_bus &= ~(1 << D7);
	if(s & 0x40) data_bus |= (1 << D6); 
	else data_bus &= ~(1 << D6);
	if(s & 0x20) data_bus|=(1 << D5); 
	else data_bus &= ~(1 << D5);
	if(s & 0x10) data_bus|=(1 << D4); 
	else data_bus &= ~(1 << D4);
	
	control_bus &= ~(1 << RS);
	control_bus |= (1 << EN);
	_delay_us(2);
	control_bus &= ~(1 << EN);
	_delay_us(2);
	
	if(s & 0x08) data_bus |= (1 << D7); 
	else data_bus &= ~(1 << D7);
	if(s & 0x04) data_bus |=(1<<D6); 
	else data_bus &= ~(1<<D6);
	if(s & 0x02) data_bus |=(1 << D5); 
	else data_bus &= ~(1 << D5);
	if(s & 0x01) data_bus |= (1 << D4); 
	else data_bus &= ~(1 << D4);
	
	control_bus &= ~(1 << RS);
	control_bus |= (1 << EN);
	_delay_us(2);
	control_bus &= ~(1 << EN);
	_delay_us(2);
}




void LCD_DataWrite(unsigned char data)
{
	
	if(data & 0x80) data_bus |= (1 << D7);
	else data_bus &= ~(1 << D7);
	if(data & 0x40) data_bus |= (1 << D6);
	else data_bus &= ~(1 << D6);
	if(data & 0x20) data_bus|=(1 << D5);
	else data_bus &= ~(1 << D5);
	if(data & 0x10) data_bus|=(1 << D4);
	else data_bus &= ~(1 << D4);
	
	control_bus |= (1 << RS) | (1 << EN);
	_delay_us(2);
	control_bus &= ~(1 << EN);
	_delay_us(2);
	
	if(data & 0x08) data_bus |= (1 << D7);
	else data_bus &= ~(1 << D7);
	if(data & 0x04) data_bus |=(1<<D6);
	else data_bus &= ~(1<<D6);
	if(data & 0x02) data_bus |=(1 << D5);
	else data_bus &= ~(1 << D5);
	if(data & 0x01) data_bus |= (1 << D4);
	else data_bus &= ~(1 << D4);
	
	control_bus |= (1 << RS) | (1 << EN);
	_delay_us(2);
	control_bus &= ~(1 << EN);
	_delay_us(2);
	
}


void LCD_Init()
{
	controlbus_direction |= ((1 << RS) | (1 << EN));
	databus_direction |= ((1 << D7) | (1 << D6) | (1 << D5) | (1 << D4));
	
	_delay_us(2);
	LCD_Write_Comand(0x01); // clear display
	LCD_Write_Comand(0x02); // back to home
	LCD_Write_Comand(0x28); // 4bit,2line,5x7 pixel
	LCD_Write_Comand(0x06); // entry mode,cursor increments by cursor shift
	LCD_Write_Comand(0x0c); // display ON,cursor OFF
	LCD_Write_Comand(0x80); // force cursor to begin at line1

}



void LCD_Disp(const char *p)
{
	while(*p!='\0')
	{
		LCD_DataWrite(*p);
		p++; _delay_ms(2);
	}
}

void LCD_Integer(uint32_t number)
{
	char buffer_LCD[100];
	itoa(number, buffer_LCD, 10);
	LCD_Disp(buffer_LCD);
}


void LCD_setCursor(unsigned char a, unsigned char b)
{
	unsigned char i=0;
	switch(b)
	{
		case 0: LCD_Write_Comand(0x80);
		break;
		case 1: LCD_Write_Comand(0xC0);
		break;
		case 2: LCD_Write_Comand(0x94);
		break;
		case 3: LCD_Write_Comand(0xd4);
		break;
		}
	for(i = 0; i < a; i++)
	LCD_Write_Comand(0x14);
}


void Timer_Init()
{
	// Load the high byte, then the low byte
	// into Output Compare A on Timer 1
	OCR1AH = (CTC_OVERFLOW >> 8);
	OCR1AL = CTC_OVERFLOW;
	// CTC mode, Clock/8
	TCCR1B |= (1 << WGM12) | (1 << CS11);
	// Enable the compare match interrupt
	TIMSK1 |= (1 << OCIE1A);
	// enable global interrupts
	sei();
}

ISR (TIMER1_COMPA_vect)
{
	timer1_millsec++;
	if (!(PIND & (1 << PIND1))){
		timer1_millsec = 0;
	}
}

uint32_t millsec()
{
	uint32_t realtime;
	
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		
		realtime = timer1_millsec;
	}
	
	return realtime;
}

void LCD_Clear()
{
	LCD_Write_Comand(0x01);		/* Clear display */
	_delay_ms(2);
	LCD_Write_Comand(0x80);		/* Cursor at home position */
}

void LCD_default(){
	
	LCD_Clear();
	_delay_us(2);
	LCD_setCursor(0,0);
	LCD_Disp("StopWatch");
	
	LCD_setCursor(0,1);
	LCD_Integer(0);
	LCD_setCursor(1,1);
	LCD_Integer(0);
	LCD_setCursor(2,1);
	LCD_Integer(0);
	LCD_setCursor(3,1);
	LCD_Integer(0);;
	LCD_setCursor(4,1);
	LCD_Disp(".");
	LCD_setCursor(5,1);
	LCD_Integer(0);
	LCD_setCursor(6,1);
	LCD_Integer(0);
	LCD_setCursor(7,1);
	LCD_Integer(0);
	
}



