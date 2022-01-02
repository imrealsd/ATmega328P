/*-----------------------------------------------------------------------
----------------------- RC RECEIVER CODE ---------------------------


OWNER: SUBHADEEP DHANG	                  CREATION DATE: 11.12.2021
----------------------------------------------------------------------------
# MAIN SOURCE FILE
# DEVELOPED FOR ATMEGA328P [CRYSTAL 16 MHZ] 
# BUILD USING VDCODE & PLATFORM.IO EXTENSION
# THIS IS OPEN SOURCE , ANYONE CAN DOWNLOAD & USE IT 
-------------------------------------------------------------------------*/


#include "atmega328p.h"

void move_forword(void);
void move_backword(void);
void move_left(void);
void move_right(void);
void blow_horn(void);
void init_port(void);
void actions(char);
void clear_ports(void);
void stop_moving(void);


int main(void){

    init_port();
    outport_c(0x00);
    outport_b(0x00);
    char cmd;

    while(1){

        cmd = inport_d();
        cmd = (cmd & 0x0F);
        actions(cmd);
        while( cmd == (inport_d() & 0x0F));    // wait until data is changed or new data arived 
    }
}

void init_port(void){

    dir_d(0xF0);  // lower 4 bit input from decoder
    dir_b(0x07);  // motor 1, 2 control
    dir_c(0x0F);  // motor 3, 4 and horn control
}

void move_forword(void){

    outport_b(0b00000101);    // motor 1, 2
    outport_c(0b00000101);    // motor 3, 4
}

void move_backword(void){

    outport_b(0b00000110);    // motor 1, 2
    outport_c(0b00000110);    // motor 3, 4
}

void move_left(void){

    outport_b(0b00000101);
    outport_c(0b00000110);
}

void move_right(void){

    outport_b(0b00000110);
    outport_c(0b00000101);
}

void blow_horn(void){         // 3rd bit is horn

    volatile char *out = (volatile char *)0x28;

    *out |= 0x08;    // bit set
    delay_ms(30);
    *out &= ~(0x08); // bit reset
}

void stop_moving(void){

    outport_b(0x00);
    outport_c(0x00);
}

void actions(char cmd){

    switch (cmd)
    {
        case 0x01:

            move_forword();
            break;

        case 0x02:

            move_backword();
            break;

        case 0x03:

            move_left();
            break;
        
        case 0x04:

            move_right();
            break;

        case 0x05:

            blow_horn();
            break;

        case 0x06:
            stop_moving();
            break;
        default:

            break;
    }
}

