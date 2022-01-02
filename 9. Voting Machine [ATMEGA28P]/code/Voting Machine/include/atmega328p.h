/* -------------- Atmega328P REGISTER CINFIG.----------------------

>>OWNER: SUBHADEEP DHANG                    >>CREATION DATE: 22.10.2021

---------------------------------------------------------------------*/



#ifndef ATMEGA328P_H
#define ATMEGA328P_H

/* ------------------- I/O DIRECTION REGISTER------------------------*/

void dir_b(char data);
void dir_d(char data);
void dir_c(char data);

/*-------------------I/O OUTPUT REGISTER----------------------*/

void outport_b(char data);
void outport_d(char data);
void outport_c(char data);

/*-------------------I/O INPUT REGISTER----------------------*/

char inport_b();
char inport_d();
char inport_c();

/*-------------------------- Delay-----------------------------*/

void delay_ms(long val);


/*---------------------------------------*/

#endif