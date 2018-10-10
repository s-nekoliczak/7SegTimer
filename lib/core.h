
void init_timer();

// Safely set clock counter to 0 again
void reset_timer();

/*
Get count of current value of TCNT1 register.
TCNT1 reg contains how many ticks of a second we are into the current second.
e.g. If a second is 15625 ticks and we're 7012 ticks into the current second,
then return 7012.
*/
int get_ticks();

/*
Like set ticks, except setting instead of getting.
*/
void set_ticks(int ticks);
