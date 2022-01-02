/*------------------ ELECTRONIC DIE -----------------------------

OWNER: Subhadeep Dhang                  CREATION DATE: 18.12.2021

------------------------------------------------------------------
# CREATED FOR ATMEGA328P MICROCONTROLLER
# THIS IS OPEN SOURCE 
# ANYONE CAN DOWNLOAD AND USE IT
--------------------------------------------------------- */

#include <avr/interrupt.h>

void init_port(void);
void run_counter(void);
void display_number(char);
void init_ext_interrupt(void);
void debug_led(void);
void show_animation(void);
void delay1(long);

int main(void){

	init_port();
	init_ext_interrupt();  // INT0 
	run_counter();
	while(1);
}

void init_port(void){

	volatile char *Reg_DIRD = (volatile char *)0x2A;
	volatile char *Reg_DIRB = (volatile char *)0x24;

	*Reg_DIRD = ~(0x04);  // pin 2 input (INT0)  connected to switch
	*Reg_DIRB = 0xFF;     // connected led die
}


void init_ext_interrupt(void){

	volatile char *Reg_EICRA = (volatile char *)0x69;
	volatile char *Reg_EIMSK = (volatile char *)0x3D;
	volatile char *Reg_SERG = (volatile char *)0x5F;

	*Reg_SERG = 0x80;		 // global interupt enable
	*Reg_EICRA = 0b00000010; // interupt on faling edge  , normally switch pin is pulled up
	*Reg_EIMSK = 0x01;       //enable INT0
}


void led_dice_data(char data){

	volatile char *Reg_OUTB = (volatile char *)0x25;
	*Reg_OUTB = data;
}



void run_counter(void){

	volatile char *Reg_TCC1RA = (volatile char *)0x80;
	volatile char *Reg_TCC1RB = (volatile char *)0x81;
	volatile short *Reg_OCR1A = (volatile short *)0x88;

	*Reg_TCC1RA = 0x00;
	*Reg_OCR1A = 5;            // count 0 to 5 then reset again count 0 to 5 then reset and so on . .
	*Reg_TCC1RB = 0b00001100;  // clk / 256 , ctc mode
}



ISR ( INT0_vect){

	volatile short *Reg_TCNT1 = (volatile short *)0x84;    // value will be 0 to 5
	char dice_values[6] = {0x08, 0x14, 0x1C, 0x63, 0x6B, (~0x08) };
	short index = *Reg_TCNT1;
	
	show_animation();
	led_dice_data(dice_values[index]);
}


void show_animation(void){

	char count = 3;
	
	while(count--){
		led_dice_data(0xFF);
		delay1(5);
		led_dice_data(0x00);
		delay1(5);
	}
}

void delay1(long val){

	volatile long i;
	for (i = 0; i < (val * 1000); i++){}
}