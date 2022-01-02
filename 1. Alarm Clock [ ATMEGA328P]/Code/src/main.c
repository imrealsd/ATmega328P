#include "atmega328p.h"
#include "i2c.h"

void init_port(void);
void get_time(void);
void display_time(void);
void delay1(long);
void dot_blink(void);
void play_tune(void);
void check_alarm_set_stop_button();
void check_min_hour_button(void);
void init_alarm(void);
void set_alarm(void);
void show_alarm_state(void);
void show_on(void);
void show_off(void);

char alarm_flag = 0;
struct TIME {

  char min;
  char hour;

} now, alarm;

int main(void){
  
  init_port();
  init_i2c();
  init_alarm();
  display_time();
}

void init_port(void){

  dird_(0xFF);      /* connected to 7 segment data lines */
  dirb_(0x1F);      /* 7 segment control lines (b0 to b3) & b4 connected to buzzer */
  dirc_(~(0x0F));   /* c0 , c1 , c2 c3 connected to 4 switches [input] */
}

void get_time(void){

  start_i2c();
  write_i2c(0xD0);    /* write mode */
  write_i2c(1);       /* set pointer to min position */

  start_i2c();        /* repeated start */
  write_i2c(0xD1);    /* read mode */
  now.min  = read_i2c_ack();
  now.hour = read_i2c_nack() & 0x3F;  /* not using 2 msb bits */
  stop_i2c();
}

void display_time(void){ 

  short i;
  char min_1, min_10, hour_1, hour_10;
  char num[10] = {0x3f, 0x06, 0x5B, 0x4F, 0X66, 0x6D, 0x7D, 0x07, 0x7F, 0x67};

  while(1){

    get_time();
    play_tune();
  
    min_1   = now.min & 0x0F;
    min_10  = (now.min & 0xF0) >> 4;
    hour_1  = now.hour & 0x0F;
    hour_10 = (now.hour & 0xF0) >> 4;

    for (i = 0; i < 20; i++){     // 200
      
      dot_blink();
      check_alarm_set_stop_button();
      

      outportd_(num[hour_10]);
      outportb_(~(0x01) & 0x0F);         /* enable digit 1 */
      delay1(1);
      outportb_(0x0F);                   /* disable all digits */

      outportd_(num[hour_1]);
      outportb_(~(0x02) & 0x0F);         /* enable digit 2 */  
      delay1(1);
      outportb_(0x0F);                   /* disable all digits */

      outportd_(num[min_10]);
      outportb_(~(0x04) & 0x0F);         /* enable digit 3 */
      delay1(1);
      outportb_(0x0F);                   /* disable all digits */

      outportd_(num[min_1]);
      outportb_(~(0x08) & 0x0F);         /* enable digit 4 */
      delay1(1);
      outportb_(0x0F);                   /* disable all digits */ 
      
    }
  }
}

void delay1(long val){

  volatile long i;
  for (i = 0; i < (val * 50); i++);
}


void dot_blink(void){

  static short count = 0;
  count++;

  if(count < 40){

    outportd_(0x80);
    outportb_(~(0x02) & 0x0F);
    delay1(2);
  }
  else if (count > 40){
    outportd_(0x00);
    outportb_(~(0x02) & 0x0F);
    delay1(2);
    
    if(count == 80){
      count = 0;
    }
  }
}

void check_alarm_set_stop_button(void){

  long i = 250;

  if((inportc_() & 0x04) == 0x00){    /* alarm buttton pressed */

    show_alarm_state();               /* show on / off */

    while(i--){

      set_alarm();
      check_min_hour_button();

      if((inportc_() & 0x08) == 0x00){      /* exit/ stop alarm button is pressed */
        alarm_flag = 1;                     /* enable alarm */
        break;                              /* back to display time mode */
      }
    }
  }
}



void set_alarm(void){

  char num[10] = {0x3f, 0x06, 0x5B, 0x4F, 0X66, 0x6D, 0x7D, 0x07, 0x7F, 0x67};
  char min_1, min_10, hour_1, hour_10;
  short i;

  min_1   = alarm.min % 10;
  min_10  = alarm.min / 10;
  hour_1  = alarm.hour % 10;
  hour_10 = alarm.hour / 10;

  for (i  = 0; i < 20; i++){
   
    outportd_(num[hour_10]);
    outportb_(~(0x01) & 0x0F);         /* enable digit 1 */
    delay1(1);
    outportb_(0x0F);                   /* disable all digits */

    outportd_(num[hour_1]);
    outportb_(~(0x02) & 0x0F);         /* enable digit 2 */
    delay1(1);
    outportb_(0x0F);                   /* disable all digits */

    outportd_(num[min_10]);
    outportb_(~(0x04) & 0x0F);         /* enable digit 3 */
    delay1(1);
    outportb_(0x0F);                   /* disable all digits */

    outportd_(num[min_1]);
    outportb_(~(0x08) & 0x0F);         /* enable digit 4 */
    delay1(1);
    outportb_(0x0F);                   /* disable all digits */ 
  }
}

void init_alarm(void){

  alarm.min = 0;
  alarm.hour = 0;
}


void check_min_hour_button(void){

  if((inportc_() & 0x01) == 0x00){             /*hour button pressed */
    
    if(alarm.hour == 23){
      alarm.hour = 0;
    }
    else{
      alarm.hour++;
    }
   
    delay1(100);
  }
  else if((inportc_() & 0x02) == 0x00){    /* minute button is pressed */
    
    if(alarm.min == 59){
      alarm.min = 0;
    }
    else{
      alarm.min++;
    }
    delay1(100);
  }
}

void play_tune(void){

  char temp1, temp2;
  temp1 = ((now.hour) / 16 * 10) + (now.hour % 16);
  temp2 = ((now.min) / 16 * 10) + (now.min % 16);

  if((temp1 == alarm.hour) && (temp2 == alarm.min) && alarm_flag == 1){


    outportb_(0x1F);                     /* turn on buzzer, turn off digit control bits */
    delay1(100);
    outportb_(0x0F); 
    delay1(80);

    if((inportc_() & 0x08) == 0x00){     /* alarm stop button is pressed */
      alarm_flag = 0;                    /* stop alarm */
      show_off();
    }                                  
  }
}

void show_alarm_state(void){

  if(alarm_flag == 1){
    show_on();
  }
  else{
    show_off();
  }
}

void show_on(void){

  short i = 250;
  while(i--){
    outportd_(0x3f);             /* display O */
    outportb_(~(0x02) & 0x0F);
    delay1(1);
    outportb_(0x0F);

    outportd_(0x54);           /* display n */
    outportb_(~(0x04) & 0x0F);
    delay1(1);
    outportb_(0x0F);
  }
}

void show_off(void){

  short i = 200;
  while(i--){

    outportd_(0x3f);
    outportb_(~(0x02) & 0x0F);
    delay1(1);
    outportb_(0x0F);

    outportd_(0x71);
    outportb_(~(0x04) & 0x0F);
    delay1(1);
    outportb_(0x0F);

    outportd_(0x71);
    outportb_(~(0x08) & 0x0F);
    delay1(1);
    outportb_(0x0F);
  }
}