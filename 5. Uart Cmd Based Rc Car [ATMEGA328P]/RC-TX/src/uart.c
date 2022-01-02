/* -------------- Atmega328P UART CINFIG.----------------------

>>OWNER: SUBHADEEP DHANG                    >>CREATION DATE: 22.10.2021


------------------- UART REGISTERS------------------------*/


void init_uart_tx(void) {

  volatile short *Reg_UBRR = (volatile short *)0xC4;    // baud rate set reg
  volatile char *Reg_USCRB = (volatile char *)0xC1;     //  ststus & control B reg
  volatile char *Reg_USCRC = (volatile char *)0xC2;     //  ststus & control C reg

  *Reg_UBRR = 103;    // f = 16 mhz , baud-rate = 9600
  *Reg_USCRB = 0x18;  // enable tx , 8 bit data mode
  *Reg_USCRC = 0x06;  // 8 bit data , 1 stop bit , no parity , asynchronous uart
}

void send_uart(char data) {

  volatile char *Reg_UDR   = (volatile char *)0xC6;  // uart i/o reg
  volatile char *Reg_USCRA = (volatile char *)0xC0;  //  ststus & control A reg

  while ((*Reg_USCRA & 0x20) == 0); // wait for the buffer empty flag
  *Reg_UDR = data;
}

void init_uart_rx(void){

  volatile char  *Reg_UCSR0B = (volatile char *)0xC1;
  volatile char  *Reg_UCSR0C = (volatile char *)0xC2;
  volatile short *Reg_UBRR0  = (volatile short *)0xC4;
  volatile char *Reg_SERG = (volatile char *)0x5F;

  *Reg_SERG = 0x80;          // global interupt enable
  *Reg_UCSR0B = 0b10010000;  // recieve complete interupt enable,  rx enable, 8 bit data mode
  *Reg_UCSR0C = 0b00000110;  // no parity , 8 bit data , aynchronus mode , 1 stop bit
  *Reg_UBRR0  = 103;         // 16 mhz clk , baud rate 9600
}



char recieve_uart(void){

  char data;
  volatile short *Reg_UDR0   = (volatile short *)0xC6;
  volatile char *Reg_UCSR0A  = (volatile char *)0xC0;

  while((*Reg_UCSR0A & 0x80) == 1);  // wait until RXC0 is 0 [receiving data is completed]
  data = *Reg_UDR0;
  return data;
}


