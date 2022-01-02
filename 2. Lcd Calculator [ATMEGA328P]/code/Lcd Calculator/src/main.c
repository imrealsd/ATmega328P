/*--------------CONFIGURATION----------------------------- 

 port_d --> lcd 8 bit data bus
 port_c -->   c5   c4    c3     c2      c1       c0
              rs    e   col3   col2     col1     col0
 port_b -->   b3  b2  b1  b0    
              r3  r2  r1  r0         

---------------------------------------------------------*/

void init_port(void);
void init_lcd(void);
void lcd_data(char);
void lcd_cmd(char);
void lcd_str(char*);
void delay_ms(long);
void outport_c(char);   // for lcd control
void outport_d(char);   // for lcd data
void outport_b(char);   // for keypad output row
char inport_c(void);    // for keypad input  col
char scan_keypad(void);
char key_map(char, char);
void check_input(char);
void display(long long);
void int_to_str(long long, char*);
void reverse_str(char*);
void initialize(char*);

/* -----------------------------MAIN CODE ------------------------------*/
int main(void){

  char x;
  init_port();
  init_lcd();
  x = scan_keypad();

  while(1){

    while(x == 0){

      x = scan_keypad();
      delay_ms(170);
    }

    check_input(x);
    lcd_data(x);
    x = 0;
  }
}
/*------------------------------------------------------------------*/

/* ------------------PORTS AND HARDWARE UNIT----------------------- */

void init_port(void){

  volatile char *dir_b = (volatile char*)0x24;
  volatile char *dir_d = (volatile char*)0x2A;
  volatile char *dir_c = (volatile char*)0x27;
  *dir_c = 0xF0; // lower nibble input , upper nibble output
  *dir_d = 0xFF;
  *dir_b = 0x0F; // lower 4 bit output rows
}


void outport_b(char data){

  volatile char *out_b = (volatile char*)0x25;
  *out_b = data;
}


void outport_c(char data){  // c5 c4

  volatile char *out_c = (volatile char*)0x28;
  *out_c = data;
}

void outport_d(char data){

  volatile char *out_d = (volatile char*)0x2B;
  *out_d = data;
}

char inport_c(void){

  char data;
  volatile char *in_c = (volatile char*)0x26;
  data = *in_c & 0x0F;
  data = (~ data) & 0x0F;
  return data;
  
}

void lcd_data(char data){

  if((data != 'c') && (data != '=')){    // need not display equal and clear symbol
    outport_d(data);
  
    outport_c(0x20);          // e = 0, rs = 1,  0010 0000
    delay_ms(1);
    outport_c(0x30);          // e = 1, rs = 1 , 0011 0000
    delay_ms(1);
    outport_c(0x20);          // e = 0, rs = 1,  0010 0000
  }
}

void lcd_cmd(char data){

  outport_d(data);
  outport_c(0x00);          // e = 0, rs = 0,  0000 0000
  delay_ms(1);
  outport_c(0x10);          // e = 1, rs = 0 , 0001 0000
  delay_ms(1);
  outport_c(0x00);          // e = 0, rs = 0,  0000 0000
}


void delay_ms(long val){

    
    volatile char *Timer1_TCCR1A = (volatile char *)0x80;
    volatile char *Timer1_TCCR1B = (volatile char *)0x81;
    volatile short *Timer1_TCNT1 = (volatile short *)0x84;
    volatile char *Timer1_TIFR1  = (volatile char *)0x36;

    while(val){
        
        *Timer1_TCCR1A = 0;                        // initializing to zero is important
        *Timer1_TCCR1B = 0; 
        *Timer1_TCNT1  = 0;
    
        *Timer1_TCNT1 = 65519;                     // loaded claculated value 65519 for 1ms delay
        *Timer1_TCCR1B = 0x05;                     // normal mode , clk/1024 presclaer
    
        while((*Timer1_TIFR1 & 0x01) == 0x00);      // wait here 
        
        *Timer1_TCCR1B = 0x00;
        *Timer1_TIFR1  = 0x01;                      // reset flag

        val--;
    }
}



void init_lcd(void){

  lcd_cmd(0x38);   // 8bit 2 line
  lcd_cmd(0x0F);   // Display on Cursor Blinking
  lcd_cmd(0x01);   // Clear Display
  lcd_cmd(0x06);   // Auto Cursor Increment

 /* start up screen */
  lcd_cmd(0x83);  
  lcd_str("WELCOME");
  delay_ms(1500);
  lcd_cmd(0x01);
}

void lcd_str(char *ptr){

  short i;
  for(i = 0; ptr[i] != '\0'; i++){

    lcd_data(ptr[i]);
  }
}


char scan_keypad(void){
  char row, col, input_data;

  for(row = 0; row < 4; row++){

    outport_b(~(1 << row));
    input_data = inport_c();

    for(col = 0; col < 4; col++){

      if(input_data == (1 << col)){
        return (key_map(row,col));
      }
    }
  }
  return(0);
}

char key_map(char row , char col){

  char keys[4][4] = {  {'7', '8','9','/'},
                       {'4', '5','6','x'},
                       {'1', '2','3','-'},
                       {'c', '0','=','+'}};

  return(keys[row][col]);
}

/*------------------------------------------------------------------*/


/*-------------------CALCULATION PART -------------------------------*/

void check_input(char data){

  static short num_flag = 1, operation = 0; 
  static long long num1 = 0, num2 = 0;     // num_flag = 1 (store input at num1) , num_flag = 2 (store input at num2)
  long long result, remainder;

  if(data == 'c'){

    lcd_cmd(0x01);      // clear screen

    num1      = 0;      // reset variables
    num2      = 0;
    result    = 0;
    remainder = 0;
    operation = 0;
    num_flag  = 1; 
  }

  else if(data == '='){

    lcd_cmd(0xC0);           // go to 2nd row for print result

    switch (operation){
    
    case 1:

      result = num1 + num2;
      display(result);
      break;

    case 2:
      
      if(num1 > num2 || num1 == num2){
        result = num1 -  num2;
      }
      else{
        result = num2 - num1;
        lcd_data('-');          // put negative sign
      }
      display(result);
      break;

    case 3:

      result = num1 * num2;
      display(result);
      break;

    case 4:

      result = num1 / num2;     
      remainder = num1 % num2;
      display(result);
      lcd_cmd(0x14);  // space
      lcd_cmd(0x14);
      display(remainder);
      break;
    }         
  }

  else if ( data == '+' || data == '-' || data == 'x' || data == '/' ){  // for operator input

    switch(data){

    case('+'): 
      operation = 1;       // add
      break;
    case('-'):
      operation = 2;       // sub
      break;
    case('x'):
      operation = 3;       // mul
      break;
    case('/'): 
      operation = 4;       // div
      break;
    }

    num_flag = 2; // now store 2nd data
  }

  else {  // for number input

    if(num_flag == 1){ // store at num1

      num1 = (num1 * 10) + (data - 48);
    }
    else if(num_flag == 2){

      num2 = (num2 * 10) + (data - 48);
    }

  }
}


void display(long long result){

  if(result == 0){

    lcd_data('0');
  }
  else{

    char *str = (char *)malloc(16 * sizeof(char));
    int_to_str(result, str);
    reverse_str(str);
    lcd_str(str);
    initialize(str); 
  }
}


void int_to_str(long long result, char* str){ 

  int digit, i = 0;
  
  while(result != 0){             // store last digit first,  

    digit = result % 10;
    str[i] = digit + 48;          // int to ascii
    i++;
    result = result / 10;
  }
  
}

void reverse_str(char* str){

  int i, temp = 0;
  int size = strlen(str);

  for (i = 0; i < size / 2; i++){

    temp = str[i];
    str[i] = str[size - 1 - i];
    str[size - 1 - i] = temp;
  }
}

void initialize(char *str){

  short i = 0;
  for(i = 0; i < 16; i++){
    str[i] = '\0';
  }
}