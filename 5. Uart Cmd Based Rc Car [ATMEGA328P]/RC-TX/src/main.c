/*-----------------------------------------------------------------------
----------------------- RC TRANSMITTER CODE ---------------------------


OWNER: SUBHADEEP DHANG	                  CREATION DATE: 11.12.2021
----------------------------------------------------------------------------
# MAIN SOURCE FILE
# DEVELOPED FOR ATMEGA328P [CRYSTAL 16 MHZ] 
# BUILD USING VDCODE & PLATFORM.IO EXTENSION
# THIS IS OPEN SOURCE , ANYONE CAN DOWNLOAD & USE IT 
-------------------------------------------------------------------------*/

#include "atmega328p.h"
#include "uart.h"
#include <avr/interrupt.h>

void init_port(void);
void send_to_encoder(char);
void send_commands(char);
void transmit_enable_pulse(void);

int main(void){

    init_port();
    init_uart_rx();
    outport_c(0x01);   // initially transmit disble
    while(1);
}


void init_port(void){

    dir_c(0x01);   // Transmit enable [ active low] [ initially pulled up]
    dir_b(0x0F);   // output 4 bit data to encoder
}


ISR ( USART_RX_vect){            // read the data received by uart [ send from virtual terminal]

    volatile char received_data;
    received_data = recieve_uart();
    send_commands(received_data); // generate particular commands for particular  data
}

void send_commands(char received_data){  // 4 bit command

    switch(received_data)
    {
    case 'F' : // move forword

        outport_b(0x01);           
        transmit_enable_pulse();
        break;

    case 'B' : // move backword

        outport_b(0x02);           
        transmit_enable_pulse();
        break;

    case 'L' : // move left

        outport_b(0x03);
        transmit_enable_pulse();
        break;

    case 'R' : // move right

        outport_b(0x04);
        transmit_enable_pulse();
        break;

    case 'H': // blow horn

        outport_b(0x05);
        transmit_enable_pulse();
        break;
        
    case 'S':  // stop moving

        outport_b(0x06);
        transmit_enable_pulse();

    default:
        break;
    }
}

void transmit_enable_pulse(void){

    outport_c(0x00);  // transmit enable
    delay_ms(2);
    outport_c(0x01);  // transmit disable
}