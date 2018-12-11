
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "core.h"
#include "sevenseg.h"
#include "pins.h"
#include "consts.h"

volatile int secs;
volatile int target_secs;
volatile uint8_t is_running;
volatile uint8_t is_finished;
volatile uint8_t in_set_mode;

char time_str[TIME_STR_SIZE];
char target_time_str[TIME_STR_SIZE];

ISR(TIMER1_COMPA_vect) {
    if (is_running) {
        secs -= SECS_INCR_AMOUNT;
    }
    if (secs == 0) {
        is_finished = 1;
        is_running = 0;
    }
}

// Handle counterclockwise turns from rotary encoder
ISR(INT0_vect) {
    // Need value of other input B/INT1/PD3/clockwise immediately
    if(BTN_PIN & (1 << BTN_B_CW)) {
        if (in_set_mode) {
            target_secs -= SECS_JUMP_INTERVAL;
            if (target_secs < MIN_SECS) {
                target_secs = MIN_SECS;
            }
        }
    }
}

// Handle clockwise turns from rotary encoder
ISR(INT1_vect) {
    // Need value of other input A/INT0/PD2/counterclockwise immediately
    if(BTN_PIN & (1 << BTN_A_CCW)) {
        if (in_set_mode) {
            target_secs += SECS_JUMP_INTERVAL;
            if (target_secs > MAX_SECS) {
                target_secs = MAX_SECS;
            }
        }
    }
}

int is_pressed(uint8_t pin_port, uint8_t pin) {
        if (pin_port & (1 << pin)) {

            // Cycle i times to debounce button
            for (uint8_t i = 0; i < 2; ++i) {
                if (in_set_mode) {
                    secs_to_time_str(target_secs, target_time_str);
                    disp_time(target_time_str);
                } else {
                    secs_to_time_str(secs, time_str);
                    disp_time(time_str);
                }
            }

            if (pin_port & (1 << pin)) {
                return 1;
            }
        }

        return 0;
}

int main(void) {

    SEG_DDR = 0xFF;
    SEG_PORT = 0x00;

    DIG_DDR = 0xFF;
    DOT_DDR = 0xFF;
    light_colon();
    BTN_DDR = 0x00;

    secs = TARGET_START_SECS;
    target_secs = TARGET_START_SECS;
    is_running = 0;
    is_finished = 0;
    in_set_mode = 0;

    uint8_t is_reset_btn_changed = 0;
    uint8_t is_pause_btn_changed = 0;
    uint8_t is_set_mode_btn_changed = 0;
    memset(time_str, 0, strlen(time_str));
    memset(target_time_str, 0, strlen(target_time_str));

    ICR1 = TICKS_PER_SEC;
    init_timer();
    sei();

    while (1) {
        if (in_set_mode) {
            secs_to_time_str(target_secs, target_time_str);
            disp_time(target_time_str);
        } else {
            secs_to_time_str(secs, time_str);
            disp_time(time_str);
        }

        if (is_finished) {
            // TODO 
            ;
        }

        // Handle reset button presses
        if(is_pressed(BTN_PIN, BTN_RESET)) {
            if (is_reset_btn_changed == 0) {
                reset_timer();
                secs = target_secs;
                is_running = 0;
                is_finished = 0;
                is_reset_btn_changed = 1;
            }
        } else {
            is_reset_btn_changed = 0;
        }

        // Handle pause button presses
        if(is_pressed(BTN_PIN, BTN_PAUSE)) {
            if (is_pause_btn_changed == 0) {
                // Unpausing... 
                if (!is_running) {
                    // ... so set timer ticks back to 0 so we get a full second
                    set_ticks(0);
                }
                is_running = !is_running;
                is_pause_btn_changed = 1;
            }
        } else {
            is_pause_btn_changed = 0;
        }

        // Handle rotary encoder push presses
        // This brings timer in and out of set mode
        if(is_pressed(BTN_PIN, BTN_SET_MODE)) {
            if (is_set_mode_btn_changed == 0) {
                in_set_mode = !in_set_mode;
                if (in_set_mode) {
                    light_top_dot();
                } else {
                    unlight_top_dot();
                }
                is_set_mode_btn_changed = 1;
            }
        } else {
            is_set_mode_btn_changed = 0;
        }

    }

    return 0;
}
