
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "sevenseg.h"
#include "pins.h"

volatile int secs;
volatile uint8_t is_running;
char time_str[4];

ISR(TIMER1_COMPA_vect) {
    if(is_running) {
        secs += 1;
    }
}

void init_timer() {

    TCCR1A |=
            (1 << WGM12);   // CTC Mode

    TCCR1B |=
            (1 << WGM13)    // CTC Mode
        |   (0 << CS12)
        |   (1 << CS11)
        |   (1 << CS10);    // 64x prescaler

    TIMSK |=
            (1  << OCIE1A);

}

int is_pressed(uint8_t pin_port, uint8_t pin, uint8_t *is_btn_changed) {
        if ((pin_port & (1 << pin)) && !(*is_btn_changed)) {
            *is_btn_changed = 1;

            // Cycle i times to debounce button
            for (uint8_t i = 0; i < 5; ++i) {
                secs_to_time_str(secs, time_str);
                disp_time(time_str);
            }

            if (pin_port & (1 << pin)) {
                return 1;
            }

            return 0;

        } else if (!(pin_port & (1 << pin))) {
            *is_btn_changed = 0;
            return 0;
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

    // One second is exactly 7811.5, rounding up for convenience's sake
    ICR1 = 7812;
    OCR1A = 0;
    init_timer();

    uint8_t is_reset_btn_changed = 0;
    memset(time_str, 0, strlen(time_str));

    while (1) {
        secs_to_time_str(secs, time_str);
        disp_time(time_str);

        if(is_pressed(BTN_PIN, BTN_RESET, &is_reset_btn_changed) && is_reset_btn_changed == 1) {
            unsigned char sreg;

            // Safely set clock counter to 0 again
            sreg = SREG;
            cli();
            TCNT1 = 0;
            SREG = sreg;

            secs = 0;
            is_running = !is_running;
        }

    }

    return 0;
}
