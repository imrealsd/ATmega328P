/* -------------- Atmega328P REGISTER CINFIG.----------------------

>>OWNER: SUBHADEEP DHANG                    >>CREATION DATE: 22.10.2021


------------------- I/O DIRECTION REGISTER------------------------*/

void dir_b(char data){

	volatile char *dirb = (volatile char *)0x24;
	*dirb = data;
}

void dir_c(char data){

	volatile char *dirc = (volatile char *)0x27;
	*dirc = data;
}

void dir_d(char data){

	volatile char *dird = (volatile char *)0x2A;
	*dird = data;
}

/*-------------------------------------------------------*/



/*-------------------I/O OUTPUT REGISTER----------------------*/

void outport_b(char data){

	volatile char * outb = (volatile char *)0x25;
	*outb = data;
}

void outport_c(char data){

	volatile char * outc = (volatile char *)0x28;
	*outc = data;
}

void outport_d(char data){

	volatile char * outd = (volatile char *)0x2B;
	*outd = data;
}

/*----------------------------------------------------------*/



/*--------------------I/O INPUT REGISTER ----------------------*/

char inport_b(){

	char data;
	volatile char *inportb = (volatile char*)0x23;
	data = *inportb;
	return data;
}

char inport_c(){

	char data;
	volatile char *inportc = (volatile char*)0x26;
	data = *inportc;
	return data;
}

char inport_d(){

	char data;
	volatile char *inportd = (volatile char*)0x29;
	data = *inportd;
	return data;

}

/*------------------------------------------------------------------*/



/*-------------------TIMER 1 DELAY-----------------------------------*/

void delay_ms(long val){

    
    volatile char *Timer1_TCCR1A = (volatile char *)0x80;
    volatile char *Timer1_TCCR1B = (volatile char *)0x81;
    volatile short *Timer1_TCNT1 = (volatile short *)0x84;
    volatile char *Timer1_TIFR1  = (volatile char *)0x36;

    while(val){
        
        *Timer1_TCCR1A = 0;           // initializing to zero is important
        *Timer1_TCCR1B = 0; 
        *Timer1_TCNT1  = 0;
    
        *Timer1_TCNT1 = 65519;          // loaded claculated value 65519 for 1s delay
        *Timer1_TCCR1B = 0x05;         // normal mode , clk/1024 presclaer
    
        while((*Timer1_TIFR1 & 0x01) == 0x00);         // wait here 
        
        *Timer1_TCCR1B = 0x00;
        *Timer1_TIFR1  = 0x01;        // reset flag

        val--;
    }
}

/*----------------------------------------------------------------*/
