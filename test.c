#include <avr/io.h>
#include <util/delay.h>
# define MYDELAY 1000


int main(void){

    while(1){    
    DDRB = 1<<PB5;
    _delay_ms(MYDELAY);
    PORTB = ~ (1<<PB5);
    _delay_ms(MYDELAY);
    }

    return 0;
}

