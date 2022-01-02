/*-----------------------------------------------------------------------
-----------------------AUTO COOLING SYSTEM ----------------------------


OWNER: SUBHADEEP DHANG	                  CREATION DATE: 23.12.2021
--------------------------------------------------------------------------
TEMPARATURE < 25         [ FAN OFF]
TEMPARATURE > 25 & < 50  [ FAN ON 50 % DUTY CYCLE]
TEMPARATURE > 50 & < 60  [ FAN ON 75 % DUTY CYCLE]
TEMPARATURE > 60         [ FAN ON 100 % DUTY CYCLE]

----------------------------------------------------------------------------
# MAIN SOURCE FILE
# DEVELOPED FOR ATMEGA328P [CRYSTAL 8 MHZ] 
# BUILD USING VDCODE & PLATFORM.IO EXTENSION
# THIS IS OPEN SOURCE , ANYONE CAN DOWNLOAD & USE IT 
-------------------------------------------------------------------------*/ 

void init_port(void);
void led_port(char);
short read_adc(void);
short get_temp(void);
void pwm(short);
void delay_ms(long);
void fan_speed(short);

int main(void){

	short val;
	init_port();

	while(1){

		val = get_temp();
		fan_speed(val);
		delay_ms(2);
	}

}

void init_port(void){

	volatile char *Reg_DIRB = (volatile char *)0x24;
	volatile char *Reg_DIRC = (volatile char *)0x27;

	*Reg_DIRB = 0x1E;
	*Reg_DIRC = 0x00;
}

short read_adc(void){

	short adc_data;
	volatile char *Reg_ADMUX = (volatile char *)0x7C;
	volatile char *Reg_ADCSRA = (volatile char *)0x7A;
	volatile short *Reg_ADC = (volatile short *)0x78;

	*Reg_ADMUX  = 0b01000000;              // right adjust, adc0, avcc
	*Reg_ADCSRA = 0b11000111;              // clk/128, start conversion , enable adc
	while((*Reg_ADCSRA & 0x10) == 0x00);   // wait to complete conversion

	*Reg_ADCSRA = 0x90;                    // reset adif flag , stop conversion
	adc_data = *Reg_ADC;
	return adc_data;
}

short get_temp(void){                      // LM35 sensor with 10k resistance

    short temp = read_adc();
    temp = (temp * 4.88) / 10;
    return temp;
}

void led_port(char data){

	volatile char *Reg_OUTB = (volatile char *)0x25;
	*Reg_OUTB = data;
}

void pwm(short count_val){

	volatile char *Reg_TCCR1B = (volatile char *)0x81;
	volatile char *Reg_TCCR1A = (volatile char *)0x80;
	volatile short *Reg_ICR1  = (volatile short *)0x86;
	volatile short *Reg_OCR1A = (volatile short *)0x88;

	*Reg_ICR1  = 500;               // top
	*Reg_OCR1A = count_val;        
	*Reg_TCCR1A = 0b10000010;       // mode 14 fast pwm , clear OC1A at compare match , set at bottom
	*Reg_TCCR1B = 0b00011100;       // clk/128 = 8 Mhz / 128 = 62.5 khz
}

void delay_ms(long val){

	volatile long i;
	for (i = 0; i < (val * 1000); i++){}
}


void fan_speed(short temp){

	if(temp < 25){

		pwm(0);          // 0% duty cycle
		led_port(0x00);  // led off
	}
	else if(temp > 25 && temp < 50){

		pwm(250);        // 50% duty cycle
		led_port(0x04);  // green led
	}
	else if(temp > 50 && temp < 60){

		pwm(350);        // 75% duty cycle
		led_port(0x08);  // yellow led
	}
	else if(temp > 60){

		pwm(500);          // 100 %
		led_port(0x10);    // red led
	}
}