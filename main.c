/*
 * GccApplication3.c
 *
 * Created: 12/27/2018 10:04:54 AM
 * Author : User
 */ 

#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>

#define RS PB4
#define E PB5
#define servo PB1

unsigned char col , row ;
char key [4]="1234";

unsigned char keypad [4][4]= {{'7','8','9','/'},
{'4','5','6','*'},
{'1','2','3','-'},
{'?','0','=','+'}};
	
char find ()
{	
	while(1)
	{
		do
		{	
			col=(PIND & 0Xf0);
		}while(col !=0xf0);

		PORTD=0xfE;//ground row0

		col = (PIND & 0xf0);
		if(col != 0xf0)//column detected
		{	
			row = 0;
			break;
		}

		PORTD=0xfD;//ground row1

		col = (PIND & 0xf0);
		if(col != 0xf0)//column detected
		{
			row = 1;
			break;
		}

		PORTD=0xfb;//ground row2

		col = (PIND & 0xf0);
		if(col != 0xf0)//column detected
		{
			row = 2;
			break;
		}

		PORTD=0xf7;//ground row3
		
		col = (PIND & 0xf0);
		if(col != 0xf0)//column detected
		{
			row = 3;
			break;
		}
	}

	if(col == 0xe0) return(keypad[row][0]);
	else if(col == 0xd0) return(keypad[row][1]);
	else if(col == 0xb0) return(keypad[row][2]);
	else return(keypad[row][3]);

	_delay_ms(40);

}
	
int main(void)
{
	DDRC=0xff;
	DDRB|=1<<RS|1<<E;
	
	lcd_init();
	
	DDRD=0x0f; //columns rows
	PORTD=0xf0;
		 
	while(1)
	{
		char user [4];

		
		lcd_print("Enter pin");
		lcd_gotoxy(1,2);//move to second line

		for (int i=0;i<4;i++)
		{
			user[i]=find();
			send_data(user[i]);	
				 
		}
				
		send_command(0x01);
		
		if (user[0]=='1'&
			user[1]=='2'&
			user[2]=='3'&
			user[3]=='4')
		{
			lcd_print("Welcome");
			DDRB |=(1<<PORTB1);
			TCNT1 = 0; // reset
			TCCR1A |=(1<<WGM11) |(1<<COM1A1) ;
			TCCR1B |=(1<<WGM13) |(1<<WGM12) |(1<<CS10) | (1<<CS11);
			ICR1=2499;//top
			 
			OCR1A=600;
			_delay_ms(2000);
			send_command(0x01);
	
			OCR1A=374;
			_delay_ms(1000);
		} 
		else
		{
			lcd_print("False");
			send_command(0x01);
		}
	}

}//end main
void lcd_init(){
	send_command(0x33);
	send_command(0x32);
	send_command(0x28);
	send_command(0x0E);
	send_command(0x01);
	_delay_ms(1000);
}
void send_command(unsigned char cmd){
	
	//char i,j;
	
	/*i=cmd & 0xf0; //send higher nibble*/
	PORTC = cmd>>4;
	PORTB &= ~ (1<<RS); //RS = 0 ; commmand
	PORTB |=  (1<<E); //send pulse
	_delay_ms(100);
	PORTB &= ~(1<<E);
	
/*	j=cmd & 0x0f; //send lower nibble*/
	PORTC = cmd & 0x0f;
	PORTB &= ~ (1<<RS); //RS = 0 ; commmand
	PORTB |=  (1<<E); //send pulse
	_delay_ms(100);
	PORTB &= ~(1<<E);
}
void send_data(unsigned char data){
	
	unsigned char i,j;
	
	i=data & 0xf0;//send higher nibble
	PORTC =i>>4;
	PORTB |= (1<<RS); //RS = 1 ; Data
	PORTB |=  (1<<E); //send pulse
	_delay_ms(100);
	PORTB &= ~(1<<E);
	
	j=data & 0x0f; //send lower nibble
	PORTC =j;
	PORTB |= (1<<RS); //RS = 1 ; Data
	PORTB |=  (1<<E); //send pulse
	_delay_ms(100);
	PORTB &= ~(1<<E);
}
void lcd_print(char*str){
	unsigned char i=0;
	while (str[i]!=0)
	{
		send_data(str[i]);
		i++;
	}
}
void lcd_gotoxy(unsigned char x, unsigned char y){
	unsigned char firstCharAdr[]={0x80, 0xC0, 0x94, 0xD4};
	send_command(firstCharAdr[y-1]+x-1);
	_delay_us(100);
	
}
