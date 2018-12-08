
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "pins.h"

const uint8_t seg_nums[10] = {
    // 0
    0 << SEG_A | 0 << SEG_B | 0 << SEG_C | 0 << SEG_D | 0 << SEG_E | 0 << SEG_F | 1 << SEG_G,
    // 1
    1 << SEG_A | 0 << SEG_B | 0 << SEG_C | 1 << SEG_D | 1 << SEG_E | 1 << SEG_F | 1 << SEG_G,
    // 2
    0 << SEG_A | 0 << SEG_B | 1 << SEG_C | 0 << SEG_D | 0 << SEG_E | 1 << SEG_F | 0 << SEG_G,
    // 3
    0 << SEG_A | 0 << SEG_B | 0 << SEG_C | 0 << SEG_D | 1 << SEG_E | 1 << SEG_F | 0 << SEG_G,
    // 4
    1 << SEG_A | 0 << SEG_B | 0 << SEG_C | 1 << SEG_D | 1 << SEG_E | 0 << SEG_F | 0 << SEG_G,
    // 5
    0 << SEG_A | 1 << SEG_B | 0 << SEG_C | 0 << SEG_D | 1 << SEG_E | 0 << SEG_F | 0 << SEG_G,
    // 6
    0 << SEG_A | 1 << SEG_B | 0 << SEG_C | 0 << SEG_D | 0 << SEG_E | 0 << SEG_F | 0 << SEG_G,
    // 7
    0 << SEG_A | 0 << SEG_B | 0 << SEG_C | 1 << SEG_D | 1 << SEG_E | 1 << SEG_F | 1 << SEG_G,
    // 8
    0 << SEG_A | 0 << SEG_B | 0 << SEG_C | 0 << SEG_D | 0 << SEG_E | 0 << SEG_F | 0 << SEG_G,
    // 9
    0 << SEG_A | 0 << SEG_B | 0 << SEG_C | 0 << SEG_D | 1 << SEG_E | 0 << SEG_F | 0 << SEG_G,
};

const uint8_t dig_nums[4] = {
    1 << DIG_0 | 0 << DIG_1 | 0 << DIG_2 | 0 << DIG_3,
    0 << DIG_0 | 1 << DIG_1 | 0 << DIG_2 | 0 << DIG_3,
    0 << DIG_0 | 0 << DIG_1 | 1 << DIG_2 | 0 << DIG_3,
    0 << DIG_0 | 0 << DIG_1 | 0 << DIG_2 | 1 << DIG_3,
};

void disp_sing_num(uint8_t digit_pin, uint8_t num) {
    DIG_PORT = dig_nums[digit_pin];
    SEG_PORT = seg_nums[num];
}

void disp_time(char num[]) {
    int8_t len = strlen(num);

    for (int8_t i = len - 1; i >= 0; --i) {
        disp_sing_num(i+(4-len), num[i] - '0');
        _delay_ms(1);
    }

    for (int8_t i = 3-len; i >= 0; --i) {
        disp_sing_num(i, 0);
        _delay_ms(1);
    }
}

void light_top_dot() {
    DOT_PORT |= (1 << DOT_TOP);
}

void unlight_top_dot() {
    DOT_PORT &= ~(1 << DOT_TOP);
}

void light_colon() {
    DOT_PORT |= 1 << DOT_COLON;
}

void unlight_colon() {
    DOT_PORT &= ~(1 << DOT_COLON);
}

void secs_to_time_str(int secs, char *buf) {
    char str_mins[3];
    char str_secs[3];
    memset(buf,0,strlen(buf));
    snprintf(str_mins, 3, "%02d", secs / 60);
    snprintf(str_secs, 3, "%02d", secs % 60);
    strcat(buf, str_mins);
    strcat(buf, str_secs);
}

