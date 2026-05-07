#include "support/analog_read.h"
#include "support/timer_helper.h"
#include "setup/pwm.h"
#include <Arduino.h>
#include "support/joystick.h"
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include "setup/helpers.h"
#include "setup/I2C.h"
#include "setup/ssd1306.h"
#include "setup/spi.h"
#include "setup/uart.h"
#include <string.h>

#define MAX_DELAY 800
#define MIN_DELAY 250

#define ARM_ROTATION 11
#define ARM_STRETCH 12
#define ARM_HEIGHT 13
#define HAND_ROTATION 5
#define HAND_CLAW 2 
#define OPTIONAL_SERVO 3


Joystick j1, j2;
volatile uint32_t ms_ticks = 0;  
volatile uint32_t last_step[5] = {0,0,0,0,0};

int16_t deg[5] = {0,0,0,0,0};


int main() {
    // initiliasere alle dele 
    uart_init();
    _delay_ms(2000);
    I2C_Init();
    InitializeDisplay();
    clear_display();
    _delay_ms(2000);
    adc_init();
    joystick_init(&j1, 0, 1, 2, 50);
    joystick_init(&j2, 2, 3, 3, 50);
    servo_init(PWM_TIMER1, PWM_CH_A, SERVO_GENERIC);
    servo_init(PWM_TIMER1, PWM_CH_B, SERVO_GENERIC);
    servo_init(PWM_TIMER3, PWM_CH_A, SERVO_GENERIC);
    servo_init(PWM_TIMER4, PWM_CH_B, SERVO_GENERIC);
    servo_init(PWM_TIMER4, PWM_CH_A, SERVO_GENERIC);
    initialize_timer(Timer5, 10000);
    sei();
    uart_print("Robot Arm ready\r\nCmds: min X  max X  (range -60 to 60)\r\n");

    // globale 

    bool btn_prev = false;
    bool claw_opening = false;
    uint32_t last_btn_press = 0;
    uint32_t last_display = 0;
    int16_t deg_min = -60, deg_max = 60;

       while(1) {
        cli();
        uint32_t now = ms_ticks;
        sei();

        joystick_update(&j1);
        joystick_update(&j2);

        // knap tilstand styring 

        if(j1.btn && !btn_prev && (now - last_btn_press) >= 500) {
            claw_opening = !claw_opening;
            last_btn_press = now;
        }

        btn_prev = j1.btn;

        // UART styring af max "Degrees", tager imod kommandoerne "max x" og "min x" hvor x går fra -60 til 60. 

        if(receiveFlag) {
            char cmd[16];
            cli();
            memcpy(cmd, (char*)rxBuf, 16);
            receiveFlag = 0;
            sei();
            if(strncmp(cmd, "min ", 4) == 0) {
                int16_t v = (int16_t)atoi(cmd + 4);
                if(v >= -60 && v < deg_max) { deg_min = v; uart_print("min set\r\n"); }
                else uart_print("err: out of range\r\n");
            } else if(strncmp(cmd, "max ", 4) == 0) {
                int16_t v = (int16_t)atoi(cmd + 4);
                if(v <= 60 && v > deg_min) { deg_max = v; uart_print("max set\r\n"); }
                else uart_print("err: out of range\r\n");
            }
        }

        // Generel servo styring 


    // speed_xs afgør hastigheden af servoen, hvor større analog værdi medfører til mindre delay mellem inkrementeringerne af 'deg' 

        int16_t speed_x1 = MAX_DELAY - ((float)abs(j1.x) / 512) * (MAX_DELAY - MIN_DELAY);
        int16_t speed_y1 = MAX_DELAY - ((float)abs(j1.y) / 512) * (MAX_DELAY - MIN_DELAY);
        int16_t speed_x2 = MAX_DELAY - ((float)abs(j2.x) / 512) * (MAX_DELAY - MIN_DELAY);
        int16_t speed_y2 = MAX_DELAY - ((float)abs(j2.y) / 512) * (MAX_DELAY - MIN_DELAY);


    /* opdaterer indiduel servo på baggrund af speed_xs, som er beregnet ovenover.
       Dette sker hvert "now - last_step[0] == speed_x1" under ideele sammenhænge". 
       now er den tid er der gået siden programmet startede. Last_step er sidste "markering" i tidslinjen
       hvornår den pågældende servo opdaterede.
    */

        if(j1.x != 0 && (now - last_step[0]) >= (uint32_t)speed_x1) {
          deg[0] += (j1.x > 0) ? 1 : -1; // Hvis jx.x er større end 0, så: deg[x] = deg[x] + 1, ellers - 1. 
          if(deg[0] > deg_max) deg[0] = deg_max; // clamp max
          if(deg[0] < deg_min) deg[0] = deg_min; // clamp min 
          servo_set_degrees(PWM_TIMER1, PWM_CH_A, deg[0]); // api kald for opdateringen af den pågældende servo
          last_step[0] = now; // <-- Last step bliver opdateret her. 
}

      if(j1.y != 0 && (now - last_step[1]) >= (uint32_t)speed_y1) {
          deg[1] += (j1.y > 0) ? 1 : -1;
          if(deg[1] > deg_max) deg[1] = deg_max;
          if(deg[1] < deg_min) deg[1] = deg_min;
          servo_set_degrees(PWM_TIMER1, PWM_CH_B, deg[1]);
          last_step[1] = now;
      }

      if(j2.x != 0 && (now - last_step[2]) >= (uint32_t)speed_x2) {
          deg[2] += (j2.x > 0) ? 1 : -1;
          if(deg[2] > deg_max) deg[2] = deg_max;
          if(deg[2] < deg_min) deg[2] = deg_min;
          servo_set_degrees(PWM_TIMER3, PWM_CH_A, deg[2]);
          last_step[2] = now;
      }

      if(j2.y != 0 && (now - last_step[3]) >= (uint32_t)speed_y2) {
          deg[3] += (j2.y > 0) ? 1 : -1;
          if(deg[3] > deg_max) deg[3] = deg_max;
          if(deg[3] < deg_min) deg[3] = deg_min;
          servo_set_degrees(PWM_TIMER4, PWM_CH_A, deg[3]);
          last_step[3] = now;
      }

      // opdaterer claw. 
      if((now - last_step[4]) >= 250) { // hvis der er gået 25 ms: 
        if(claw_opening) { // hvis hånden er igang med at åbne:
            if(deg[4] < 60) deg[4]++; 
        } else {
            if(deg[4] > -30) deg[4]--; 
        }
        servo_set_degrees(PWM_TIMER4, PWM_CH_B, deg[4]);
        last_step[4] = now;
    }

    // print joystick værdierne + duty cycle

    if((now - last_display) >= 5000) {
        char buf[18];
        int32_t ocr;
        uint8_t di, dd;

        ocr = 1500 + (int32_t)deg[0] * 1000 / 90;
        di = ocr / 200; dd = (ocr % 200) * 10 / 200;
        sprintf(buf, "J1x:%4d %2d.%1d%%  ", j1.x, di, dd);
        sendStrXY(buf, 0, 0);

        ocr = 1500 + (int32_t)deg[1] * 1000 / 90;
        di = ocr / 200; dd = (ocr % 200) * 10 / 200;
        sprintf(buf, "J1y:%4d %2d.%1d%%  ", j1.y, di, dd);
        sendStrXY(buf, 1, 0);

        ocr = 1500 + (int32_t)deg[2] * 1000 / 90;
        di = ocr / 200; dd = (ocr % 200) * 10 / 200;
        sprintf(buf, "J2x:%4d %2d.%1d%%  ", j2.x, di, dd);
        sendStrXY(buf, 2, 0);

        ocr = 1500 + (int32_t)deg[3] * 1000 / 90;
        di = ocr / 200; dd = (ocr % 200) * 10 / 200;
        sprintf(buf, "J2y:%4d %2d.%1d%%  ", j2.y, di, dd);
        sendStrXY(buf, 3, 0);

        last_display = now;
    }
  }
}

// vores "ur", tæller i 10kHz, så når ms_ticks stiger med 10K, er der gået et sekund. 

ISR(TIMER5_COMPA_vect) {
  ms_ticks++;
}

