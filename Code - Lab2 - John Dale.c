/***********************************************
*   Hw1.cpp File which does the tasks in Lab #2 
* - Blinks each Led when pressed
* - One button blinks both simultaneously
* - if two buttons are pressed together, no blinking
*
* Revision History
*Date       Author      Revision
*2/25/22    John Dale   initial code write
************************************************/

#include <avr/io.h>
#include <util/delay.h>
#define TIME 100   // 200 ms time delay (5 blinks per second, delay happens twice per blink)

int main(void){
    DDRD = 1<<7|1<<6;  // setting Data register 6 and 7 to outputs, 0-5 are inputs
    PORTD = 1<<0|1<<1|1<<2;  // setting internal pullup resistors on PIN1 and PIN0

    while(1){
        if (!((PIND & (1<<2))==0)&&((PIND & (1<<0))==0)&&(!(PIND & (1<<1))==0)){ // PIN1,PIN2 is Pulled-up (OFF), PIN0 is ground (ON)
            PORTD|=(1<<6);    // OR mask to set 6 HIGH
            _delay_ms(TIME);   // delay by 200 miliseconds
            PORTD&=~(1<<6);   // AND mask to set 6 LOW
            _delay_ms(TIME);   // delay by 200 miliseconds
            continue;        // needed to check button status!
        }
        if (!((PIND & (1<<2))==0)&&(!(PIND & (1<<0))==0)&&((PIND & (1<<1))==0)){  // PIN0,PIN2 is Pulled-up (OFF), PIN1 is ground (ON)
            PORTD|=(1<<7);  // OR mask to set 7 HIGH
            _delay_ms(TIME);    // delay by 200 miliseconds
            PORTD&=~(1<<7);     // AND mask to set 7 LOW
            _delay_ms(TIME);    // delay by 200 miliseconds
            continue;   // needed to check button status!
        }
        if (((PIND & (1<<2))==0)&&(!(PIND & (1<<0))==0)&&(!(PIND & (1<<1))==0)){    //PIN2 is ground (ON), PIN1,PIN0, pulled-up (OFF)
            PORTD|=((1<<7)|(1<<6));  // OR mask set 6,7 HIGH
            _delay_ms(TIME);    // delay by 200 miliseconds
            PORTD&=~((1<<7)|(1<<6));  // AND mask set 6,7 LOW
            _delay_ms(TIME);       // delay by 200 miliseconds
            continue;   // needed to check button status!
        }
        
        else {
            PORTD&= ~((1<<7)|(1<<6));  // if no button is pressed, set PIN6, PIN7 LOW (OFF)
            continue;  // needed to check button status!
        }
        return(0);
    }



}