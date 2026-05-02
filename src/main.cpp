#include "support/analog_read.h"
#include "support/timer_helper.h"
#include "setup/pwm.h"
#include <Arduino.h>
#include <util/delay.h>
#include <math.h>

#define MAX_DELAY 200
#define MIN_DELAY 20

int16_t y_c = 512; 
int16_t x_c = 512;
int16_t deg = 0;
int16_t prev_val = 0;

volatile int16_t x_speed = 0;
volatile uint16_t tick_count = 0;
volatile bool timer_flag;

int main() {
    adc_init();
    sei();
    Serial.begin(9600);

      int16_t x;
      int16_t y;

      servo_init(PWM_TIMER1, PWM_CH_A, SERVO_GENERIC);
      initialize_timer(Timer3, 10000);
      
       while(1) {

        //Serial.println(deg);
        
        int16_t a_x = analog_read(0); 
        int16_t a_y = analog_read(1);  

        if(a_x < 512+25 && a_x > 512-25) a_x = 512;
        if(a_y < 512+25 && a_y > 512-25) a_y = 512;
        
        x = a_x - x_c;
        y = a_y - y_c;

        int16_t local_speed = (MAX_DELAY - ((float)abs(x)/512)*(MAX_DELAY - MIN_DELAY));
        cli();
        x_speed = local_speed;
        sei();

        if(x != 0 && timer_flag) {
          
          if(x > 0) deg += 1;
          if (x < 0) deg -= 1;
          if (deg > 90) deg = 90; 
          if (deg < -90) deg = -90;
          servo_set_degrees(PWM_TIMER1, PWM_CH_A, deg);
          timer_flag = false;
        }

    }
}


ISR(TIMER3_COMPA_vect) {
  tick_count++;
  
  if(tick_count >= x_speed) {
    reset_timer(Timer3);
    timer_flag = true;
    tick_count = 0;
  }
}