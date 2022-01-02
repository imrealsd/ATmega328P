/* ------------------------ DIGITAL VOLTMETER-----------------------------------------
OWNER: SUBHADEEP DHANG                      CREATION DATE: 22/12/21

# MADE FOR ATMEGA328P
# THIS IS OPEN SOURCE
# ANY ONE CAN DOWNLOAD AND USE IT
-------------------------------------------------------------------------------------*/

void init_port(void);
void outport_d(char);
void outport_b(char);
char inport_p2(void);
void lcd_cmd(char);
void lcd_data(char);
void lcd_str(char *);
void init_lcd(void);
void delay_ms(short);
short read_adc(void);
void display_voltage(short);

int main(void){

  short raw_val;
  init_port();
  init_lcd();
  lcd_str("DIGITAL VOLT ME.");
  lcd_cmd(0xc5);
  lcd_str(" Volt");

  while(1){

    raw_val = read_adc();
    display_voltage(raw_val);
    delay_ms(200);
  }
}

void init_port(void){

  volatile char *dir_c = (volatile char *)0x27;
  volatile char *dir_b = (volatile char *)0x24;
  volatile char *dir_d = (volatile char *)0x2A;

  *dir_d = 0xFF;        // lcd data
  *dir_c = 0x00;        // push button
  *dir_b = 0xFF;        // lcd control
}

void outport_d(char data){

  volatile char *out = (volatile char *)0x2B;
  *out = data;
}


void outport_b(char data){

  volatile char *out = (volatile char *)0x25;
  *out = data;
}


void lcd_cmd(char cmd){

  outport_d(cmd);
  outport_b(0x00);  // E = 0, RS = 0
  delay_ms(1);
  outport_b(0x01);  // E = 1, RS = 0
  delay_ms(1);
  outport_b(0x00);  // E = 0, RS = 0
}

void lcd_data(char data){

  outport_d(data);
  outport_b(0x02);  // E = 0, RS = 1
  delay_ms(1);
  outport_b(0x03);  // E = 1, RS = 1
  delay_ms(1);
  outport_b(0x02);  // E = 0, RS = 1
}

void init_lcd(void){

  delay_ms(10);
  lcd_cmd(0x01); // clear display
  lcd_cmd(0x38); // 8 bit , 2 line
  lcd_cmd(0x0C); // display on , cursor off
  lcd_cmd(0x06); // auto increment
}

void lcd_str(char *ptr){

  while(*ptr != '\0'){
    lcd_data(*ptr);
    ptr++;
  }
}

void delay_ms(short val){

  volatile short i;
  for (i = 0; i < (val * 100); i++){}
}

short read_adc(void){

  short data;
  volatile char *Reg_ADMUX = (volatile char *)0x7C;
  volatile char *Reg_ADCSRA = (volatile char *)0x7A;
  volatile short *Reg_ADC = (volatile short *)0x78;

  *Reg_ADMUX  = 0b01000000;              // right adjust, adc0, avcc
  *Reg_ADCSRA = 0b11000111;              // clk/128, start conversion , enable adc
  while((*Reg_ADCSRA & 0x10) == 0x00);   // wait to complete conversion
  *Reg_ADCSRA = 0x90;                    // reset adif flag , stop conversion
  data = *Reg_ADC;
  return data;
}

void display_voltage(short val){

  float float_volt;
  short int_volt;
  char d1, d2, d3, d4;

  float_volt = (float)(val * 5) / 1024;  // ex. 3.98
  float_volt = float_volt * 100 * 4;     // 0398 x 4    , voltage attenuated by 4 by op-amp
  int_volt = (short) float_volt;

  d1 = (char) ((int_volt /1) % 10);
  d2 = (char) ((int_volt / 10) % 10);
  d3 = (char) ((int_volt / 100) % 10);
  d4 = (char)((int_volt / 1000) % 10);

  lcd_cmd(0xc0);
  lcd_data(d4 + 48);
  lcd_data(d3 + 48);
  lcd_data('.');
  lcd_data(d2 + 48);
  lcd_data(d1 + 48);
}