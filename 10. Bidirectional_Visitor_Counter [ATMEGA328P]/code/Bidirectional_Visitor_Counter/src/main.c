/*-----------------------------------------------------------------------
-----------------------BI DIRECTIONAL VISITOR COUNTER ----------------------------
OWNER: SUBHADEEP DHANG	                  CREATION DATE: 24.12.2021
----------------------------------------------------------------------------
# MAIN SOURCE FILE
# DEVELOPED FOR ATMEGA328P [CRYSTAL 8 MHZ] 
# BUILD USING VDCODE & PLATFORM.IO EXTENSION
# THIS IS OPEN SOURCE , ANYONE CAN DOWNLOAD & USE IT 
-------------------------------------------------------------------------*/



void init_port(void);
void outportb(char);
void outportd(char);
char inportc(void);
void display_count(void);
void delay_ms(long);
void check_sensor(void);
void check_switch(void);

char count = 0;

int main(void){

	init_port();
	display_count();
}

void init_port(void){

	volatile char *dird = (volatile char *)0x2a;
	volatile char *dirb = (volatile char *)0x24;
	volatile char *dirc = (volatile char *)0x27;

	*dird = 0x03;       // 7 segment digit control
	*dirb = 0xFF;       // 7 segment data
	*dirc = 0x00;       // input sensors , reset switch
}

void outportb(char data){

	volatile char *out = (volatile char *)0x25;
	*out = data;
}

void outportd(char data){

	volatile char *out = (volatile char *)0x2b;
	*out = data & 0x03;
}

char inportc(void){

	char data;
	volatile char *in = (volatile char *)0x26;
	data = *in;
	return data & 0x07;
}


void display_count(void){

	char num[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x67};
	int i;
	char dig1, dig2;
    
	while(1){

		dig2 = count % 10;
		dig1 = count / 10;
		check_sensor();
		check_switch();

		for (i = 0; i < 20; i ++){

			outportb(num[dig1]);
			outportd(~0x01);
			delay_ms(2);
			outportd(0x03);

			outportb(num[dig2]);
			outportd(~0x02);
			delay_ms(2);
			outportd(0x03);
		}
	}
}

void delay_ms(long val){

	volatile long i;
	for (i = 0; i < (val * 100); i++){}
}


void check_sensor(void){

	char sensor_data = inportc();

	if(sensor_data == 0x01){

		count++;
		delay_ms(10);
	}
	else if(sensor_data == 0x02){

		count--;
		delay_ms(10);
	}
}

void check_switch(void){

	char data = inportc();

	if(data == 0x04){

		count = 0;
		delay_ms(5);
	}
}