/* -------------- Atmega328P I2c CINFIG.----------------------

>>OWNER: SUBHADEEP DHANG                    >>CREATION DATE: 22.10.2021


------------------- I2C REGISTERS------------------------*/


void init_i2c(void) {

  volatile char *Reg_TWBR = (volatile char *)0xB8;
  volatile char *Reg_TWCR = (volatile char *)0xBC;
  volatile char *Reg_TWSR = (volatile char *)0xB9;

  *Reg_TWBR = 0x48;            // communication frq 
  *Reg_TWSR = 0;               // prescaler 1
  *Reg_TWCR = 0b10000100;      // enable i2c
}

void start_i2c(void) {

  volatile char *Reg_TWCR = (volatile char *)0xBC;
  *Reg_TWCR = 0b10100100;         // enable i2c , clear twint flag ,start i2c
  while (!(*Reg_TWCR & 0x80));   // wait for start condition to transmit
}

void stop_i2c(void) {

  volatile char *Reg_TWCR = (volatile char *)0xBC;
  *Reg_TWCR = 0b10010100 ;        // enable i2c , clear twint flag , stop i2c
}                                 // twint dont set after stop


void write_i2c(char data) {

  volatile char *Reg_TWDR = (volatile char *)0xBB;
  volatile char *Reg_TWCR = (volatile char *)0xBC;
 
  *Reg_TWDR = data;
  *Reg_TWCR = 0b10000100;   // enable i2c , clear twint flag
  
  while (!(*Reg_TWCR & 0x80));
  
}

char read_i2c_ack(void){
  
  volatile char *Reg_TWDR = (volatile char *)0xBB;
  volatile char *Reg_TWCR = (volatile char *)0xBC;

  *Reg_TWCR = 0b11000100;  // enable i2c , clear twint , acknowledge bit
  while (!(*Reg_TWCR & 0x80));
  return *Reg_TWDR;
}

char read_i2c_nack(void){
  
  volatile char *Reg_TWDR = (volatile char *)0xBB;
  volatile char *Reg_TWCR = (volatile char *)0xBC;

  *Reg_TWCR = 0b10000100;  // enable i2c , clear twint , No acknowledge bit
  while (!(*Reg_TWCR & 0x80));
  return *Reg_TWDR;
}