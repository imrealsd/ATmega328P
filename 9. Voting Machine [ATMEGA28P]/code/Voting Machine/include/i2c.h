/* -------------- Atmega328P I2c CINFIG.----------------------

>>OWNER: SUBHADEEP DHANG                    >>CREATION DATE: 22.10.2021


------------------- I2C REGISTERS------------------------*/

#ifndef I2C_H
#define  I2C_H

void init_i2c(void);
void start_i2c(void);
void stop_i2c(void);
void write_i2c(char);
char read_i2c_ack(void);
char read_i2c_nack(void);
#endif 