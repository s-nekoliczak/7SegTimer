
#include <avr/io.h>
#include <avr/interrupt.h>

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

    GICR |=
            (1 << INT1)     // Enable INT1 interrupt
        |   (1 << INT0);    // Enable INT0 interrupt

    MCUCR |=
            (1 << ISC11)    // INT1 triggers on falling edge
        |   (1 << ISC10)
        |   (1 << ISC01)    // INT0 triggers on falling edge
        |   (1 << ISC00);

}

void reset_timer() {
    unsigned char sreg;
    sreg = SREG;
    cli();
    TCNT1 = 0;
    SREG = sreg;
}

int get_ticks() {
    unsigned char sreg;
    sreg = SREG;
    cli();
    int ticks = TCNT1;
    SREG = sreg;

    return ticks;
}

void set_ticks(int ticks) {
    unsigned char sreg;
    sreg = SREG;
    cli();
    TCNT1 = ticks;
    SREG = sreg;
}
