/* -------------- Atmega328P UART CINFIG.----------------------

>>OWNER: SUBHADEEP DHANG                    >>CREATION DATE: 22.10.2021


------------------- UART SUNCTIONS ------------------------*/

#ifndef UART_H
#define UART_H

void init_uart_tx(void);
void init_uart_rx(void);
void send_uart(char);
char recieve_uart(void);

#endif /* UART_H */
