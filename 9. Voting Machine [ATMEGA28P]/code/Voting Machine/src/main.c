/*-----------------------------------------------------------------------
-----------------------ELECTRONIC VOTING MACHINE----------------------------


OWNER: SUBHADEEP DHANG	                  CREATION DATE: 25.12.2021
----------------------------------------------------------------------------
# MAIN SOURCE FILE
# DEVELOPED FOR ATMEGA328P [CRYSTAL 16 MHZ] 
# BUILD USING VDCODE & PLATFORM.IO EXTENSION
# THIS IS OPEN SOURCE , ANYONE CAN DOWNLOAD & USE IT 
-------------------------------------------------------------------------*/ 


#include "atmega328p.h"
#include "i2c.h"

void init_port(void);
void lcd_data(char);
void lcd_cmd(char);
void lcd_init(void);
void lcd_str(char *);
void welcome_screen(void);
void show_options(void);
void button_functions(char);
char switch_val(void);
void show_result(unsigned short, unsigned short, unsigned short);
void vote_done_msg(char *);
void buzzer(void);


int main(void){

	char value;
	init_i2c();
	init_port();
	lcd_init();

	welcome_screen();
	show_options();

	value = switch_val();

	while(1){
		
		while(value == 0){             //   until switch is not pressed stay here

			value = switch_val();
			delay_ms(1);
		}
		button_functions(value);
		value = 0;                       // reset 
	}
}


void init_port(void){

	dir_d(~(0x1F));    // input switch
	dir_b(0x01);       // output buzzer
}


void lcd_cmd(char cmd){

	char upper_nibble, lower_nibble;
	upper_nibble = cmd & 0xF0;
	lower_nibble = cmd << 4;

	start_i2c();
	write_i2c(0x40);                 // slave sdress + write bit
	
	write_i2c(upper_nibble);         // upper 4 bit
	write_i2c(upper_nibble | 0x00);  // EN = 0 , RS = 0
	delay_ms(1);
	write_i2c(upper_nibble | 0x04);  // EN = 1 ,  RS = 0
	delay_ms(1);
	write_i2c(upper_nibble | 0x00);  // EN = 0 , RS = 0


	write_i2c(lower_nibble);         // lower 4 bit
	write_i2c(lower_nibble | 0x00);  // EN = 0,  RS = 0
	delay_ms(1);
	write_i2c(lower_nibble | 0x04);  // EN = 1,  RS = 0
	delay_ms(1);
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
	delay_ms(1);
	write_i2c(upper_nibble | 0x05);  // EN = 1,  RS = 1
	delay_ms(1);
	write_i2c(upper_nibble | 0x01);  // EN + 0 , RS = 1


	write_i2c(lower_nibble);         // lower 4 bit
	write_i2c(lower_nibble | 0x01);  // EN = 0, RS = 1
	delay_ms(1);
	write_i2c(lower_nibble | 0x05);  // EN = 1, RS = 1
	delay_ms(1);
	write_i2c(lower_nibble | 0x01);  // EN = 0 , RS = 1

	stop_i2c();
}


void lcd_init(void){

	delay_ms(5);

	lcd_cmd(0x28);        // 4 bit , 2 line
	lcd_cmd(0x0C);        // display on cursor off
	lcd_cmd(0x06);        // auto increment
	lcd_cmd(0x01);        // clear display
	lcd_cmd(0x80);        // cursor at home
}


void lcd_str(char *ptr){

	while(*ptr != '\0'){
		lcd_data(*ptr);
		ptr++;
	}
}

void welcome_screen(void){

	lcd_str("   WELCOME TO");
	lcd_cmd(0xc0);
	lcd_str("   ELECTRONIC");
	lcd_cmd(0x90);
	lcd_str(" VOTING MACHINE");

	delay_ms(60);
	lcd_cmd(0x01);
}

void show_options(void){

	lcd_cmd(0x01);

	lcd_cmd(0x80);               // row 0
	lcd_str("ENTER YOUR VOTE");

	lcd_cmd(0xc0); // row 1
	lcd_str("----OPTIONS----");

	lcd_cmd(0x90);
	lcd_str("Team A, Team B");

	lcd_cmd(0xd0);
	lcd_str("    Team C");
}

void button_functions(char val){

	static unsigned short team_a = 0, team_b = 0, team_c = 0;

	switch (val)
	{
	case (0x01):      // RESET BUTTON

		team_a = 0;
		team_b = 0;
		team_c = 0;
		show_result(team_a, team_b, team_c);
		delay_ms(50);
		show_options();
		break;
		
	case (0x02):      // TEAM A

		team_a++;
		buzzer();
		vote_done_msg("  [ Team A ]");
		show_options();
		break;

	case (0x04):      // TEAM B
		
		team_b++;
		buzzer();
		vote_done_msg("  [ Team B ]");
		show_options();
		break;

	case (0x08):      // TEAM C
		
		team_c++;
		buzzer();
		vote_done_msg("  [ Team C ]");
		show_options();
		break;

	case (0x10):      // RESULT

		show_result(team_a, team_b, team_c);
		break;

	default:
		break;
	}
}

char switch_val(void){

	char data = inport_d() & 0x1F;
	data = (~data) & 0x1F;
	return data;
}

void show_result(unsigned short team_a, unsigned short team_b, unsigned short team_c ){

	char dig1, dig2, dig3, dig4;
	lcd_cmd(0x01);         // clear
	lcd_str("  --RESULT--");

    /*----------------------------------------------*/       // team a
	lcd_cmd(0xc0);
	lcd_str("Team A: ");

	dig1 = (char)((team_a / 1000) % 10);
	dig2 = (char)((team_a / 100) % 10);
	dig3 = (char)((team_a /10) % 10);
	dig4 = (char)(team_a % 10);

	lcd_data(dig1 + 48);
	lcd_data(dig2 + 48);
	lcd_data(dig3 + 48);
	lcd_data(dig4 + 48);


	/*----------------------------------------------*/   // team b

	lcd_cmd(0x90);
	lcd_str("Team B: ");

	dig1 = (char)((team_b / 1000) % 10);
	dig2 = (char)((team_b / 100) % 10);
	dig3 = (char)((team_b /10) % 10);
	dig4 = (char)(team_b % 10);

	lcd_data(dig1 + 48);
	lcd_data(dig2 + 48);
	lcd_data(dig3 + 48);
	lcd_data(dig4 + 48);


    /*----------------------------------------------*/  // team c
	lcd_cmd(0xd0); 
	lcd_str("Team C: ");

	dig1 = (char)((team_c / 1000) % 10);
	dig2 = (char)((team_c / 100) % 10);
	dig3 = (char)((team_c /10) % 10);
	dig4 = (char)(team_c % 10);

	lcd_data(dig1 + 48);
	lcd_data(dig2 + 48);
	lcd_data(dig3 + 48);
	lcd_data(dig4 + 48);
	
}

void vote_done_msg(char *team_name){

	lcd_cmd(0x01);
	lcd_str("   < DONE >");

	lcd_cmd(0xc0);
	lcd_str("VOTE REGISTERED");

	lcd_cmd(0x90);
	lcd_str(team_name);
	delay_ms(30);
}

void buzzer(void){

	outport_b(0x01);
	delay_ms(50);
	outport_b(0x00);
}