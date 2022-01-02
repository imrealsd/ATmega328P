/* THIS CODE IS SAME FOR 2 MCU'S,
1ST SERIAL MONITOR -> 1ST MCU -> 2ND MCU -> 2ND SERIAL MONITOR */

#include "uart.h"
#include <avr/interrupt.h>

int main(void)
{
	init_uart_rx();
	while(1);
}


ISR (USART_RX_vect){       // interupt occurs if recived a new data

	char data = recieve_uart();

	init_uart_tx();
	send_uart(data);      // transmit the recieved data
	init_uart_rx();       // go back to reciever mode 
} 
