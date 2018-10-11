
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "core.h"
#include "sevenseg.h"
#include "pins.h"

volatile int secs;
volatile int ticks;
volatile uint8_t is_running;
char time_str[4];

ISR(TIMER1_COMPA_vect) {
    if(is_running) {
        secs += 1;
    }
}

int is_pressed(uint8_t pin_port, uint8_t pin) {
        if (pin_port & (1 << pin)) {

            // Cycle i times to debounce button
            for (uint8_t i = 0; i < 2; ++i) {
                secs_to_time_str(secs, time_str);
                disp_time(time_str);
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

    BTN_DDR = 0x00;

    sei();

    secs = 0;
    is_running = 1;

    // One second is exactly 15625
    ICR1 = 15625;
    OCR1A = 0;
    init_timer();

    uint8_t is_reset_btn_changed = 0;
    uint8_t is_pause_btn_changed = 0;
    memset(time_str, 0, strlen(time_str));

    while (1) {
        secs_to_time_str(secs, time_str);
        disp_time(time_str);

        // Handle reset button presses
        if(is_pressed(BTN_PIN, BTN_RESET)) {
            if (is_reset_btn_changed == 0) {
                reset_timer();
                secs = 0;
                is_running = 0;
                is_reset_btn_changed = 1;
            }
        } else {
            is_reset_btn_changed = 0;
        }

        // Handle pause button presses
        if(is_pressed(BTN_PIN, BTN_PAUSE)) {
            if (is_pause_btn_changed == 0) {
                if(is_running) {
                    ticks = get_ticks();
                } else {
                    set_ticks(ticks);
                }
                is_running = !is_running;
                is_pause_btn_changed = 1;
            }
        } else {
            is_pause_btn_changed = 0;
        }

    }

    return 0;
}
