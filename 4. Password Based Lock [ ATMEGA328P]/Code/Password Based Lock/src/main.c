/*-----------------------------------------------------------------------
-----------------------PASSWORD BASED CIRCUIT----------------------------


OWNER: SUBHADEEP DHANG	                  CREATION DATE: 11.12.2021
----------------------------------------------------------------------------
# MAIN SOURCE FILE
# DEVELOPED FOR ATMEGA328P [CRYSTAL 16 MHZ] 
# BUILD USING VDCODE & PLATFORM.IO EXTENSION
# THIS IS OPEN SOURCE , ANYONE CAN DOWNLOAD & USE IT 
-------------------------------------------------------------------------*/ 


#include "atmega328p.h"
#include "i2c.h"

void init_port(void);
char scan_keypad(void);
void lcd_data(char);
void lcd_cmd(char);
void lcd_init(void);
void lcd_str(char *);
void indicator_led(char);
void check_password(char);
void check_lock_button(void);

int main(void){

	char value;
	init_i2c();
	init_port();
	lcd_init();
	indicator_led(0x01);  // red

	value = scan_keypad();

	while(1){

		while(value == 0){

			value = scan_keypad();
			delay_ms(170);
		}

		if (value < 10){         // ignore bottom row buttons (val < 10)

			lcd_data(value + 48);   // number to ascii
			check_password(value);
		}
		value = 0;
	}
}


void init_port(void){

	dir_c(0x0F);       // output rows
	dir_d(~(0x07));    // input cols
	dir_b(0x03);       // output led , input lock button
}


char scan_keypad(void){

	char row, col, col_data;

	for (row = 0; row < 4; row++){

		outport_c(~(1 << row));

		col_data = ~(0x07 & inport_d());
		col_data = col_data & 0x07;

		for (col = 0; col < 3; col++){

			if(col_data ==  (1 << col)){
				return ((row * 3) + col + 1);
			}
		}
	}
	return 0;
}

void lcd_cmd(char cmd){

	char upper_nibble, lower_nibble;
	upper_nibble = cmd & 0xF0;
	lower_nibble = cmd << 4;

	start_i2c();
	write_i2c(0x40);                 // slave sdress + write bit
	
	write_i2c(upper_nibble);         // upper 4 bit
	write_i2c(upper_nibble | 0x00);  // EN = 0 , RS = 0
	delay_ms(5);
	write_i2c(upper_nibble | 0x04);  // EN = 1 ,  RS = 0
	delay_ms(5);
	write_i2c(upper_nibble | 0x00);  // EN = 0 , RS = 0


	write_i2c(lower_nibble);         // lower 4 bit
	write_i2c(lower_nibble | 0x00);  // EN = 0,  RS = 0
	delay_ms(5);
	write_i2c(lower_nibble | 0x04);  // EN = 1,  RS = 0
	delay_ms(5);
	write_i2c(lower_nibble | 0x00);  // EN + 0 , RS = 0

	stop_i2c();
}

void lcd_data(char data){

	char upper_nibble, lower_nibble;
	upper_nibble = data & 0xF0;
	lower_nibble = data << 4;

	start_i2c();
	write_i2c(0x40);                 // slave sdress + write bit
	
	write_i2c(upper_nibble);         // upper 4 bit
	write_i2c(upper_nibble | 0x01);  // EN = 0,  RS = 1
	delay_ms(5);
	write_i2c(upper_nibble | 0x05);  // EN = 1,  RS = 1
	delay_ms(5);
	write_i2c(upper_nibble | 0x01);  // EN + 0 , RS = 1


	write_i2c(lower_nibble);         // lower 4 bit
	write_i2c(lower_nibble | 0x01);  // EN = 0, RS = 1
	delay_ms(5);
	write_i2c(lower_nibble | 0x05);  // EN = 1, RS = 1
	delay_ms(5);
	write_i2c(lower_nibble | 0x01);  // EN = 0 , RS = 1

	stop_i2c();
}


void lcd_init(void){

	delay_ms(50);

	lcd_cmd(0x28);        // 4 bit , 2 line
	lcd_cmd(0x0C);        // display on cursor off
	lcd_cmd(0x06);        // auto increment
	lcd_cmd(0x01);        // clear display
	lcd_cmd(0x80);        // cursor at home

	lcd_str("-ENTER PASSWORD-");
	//delay_ms(2000);
	lcd_cmd(0xC0);
}

void lcd_str(char *ptr){

	while(*ptr != '\0'){
		lcd_data(*ptr);
		ptr++;
	}
}

void indicator_led(char data){

	outport_b(data);     // 0x01 for red , 0x02 for green
}

void check_password(char num){

	static const char password[4] = {5, 1, 3, 4};
	static char entered[4];
	static char count = 0;

	if(count < 4){               // entry pressed number

		entered[count] = num;
		count++;
	
		if(count == 4){         // match password after entering 4 digit password

			delay_ms(500);      // for dosplaying last entered digit

			if((password[0] == entered[0])  && (password[1] == entered[1]) && (password [2] == entered [2]) && (password [3] == entered [3])){
				
				indicator_led(0x02); // green led

				lcd_cmd(0x01);
				lcd_str("CORRECT PASSWORD !");
				delay_ms(1000);
				lcd_cmd(0x01);
				lcd_str("UNLOCKED");
				delay_ms(1000);
				
				check_lock_button();	
			}
			else{

				lcd_cmd(0x01);
				lcd_str("WRONG PASSWORD !!");
				delay_ms(1000);
				lcd_cmd(0x01);
				lcd_str("-ENTER PASSWORD-");
				lcd_cmd(0xC0);

				entered[0] = 0;
				entered[1] = 0;
				entered[2] = 0;
				entered[3] = 0;

			}
			count = 0;
		}	
	}
	
}

void check_lock_button(void){

	lcd_cmd(0x01);
	lcd_str("PRESS BUTTON");
	lcd_cmd(0xC0);
	lcd_str("TO LOCK");

	while((inport_b() & 0x04) != 0x00);  // wait until button is pressed
	
	lcd_cmd(0x01);
	lcd_str("LOCKED");   
	indicator_led(0x01);
	delay_ms(1000);

	lcd_cmd(0x01);
	lcd_str("-ENTER PASSWORD-");
	lcd_cmd(0xC0);
}