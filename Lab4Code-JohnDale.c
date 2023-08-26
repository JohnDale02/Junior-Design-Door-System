/*******************************************************************
 * Code for Lab4, LED brightness control using PWM
 * Date: 4/13/22
 * John Dale: Embedded Systems ECE231
 * ****************************************************************/
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

void timer1_init(void);

int main(void){
   timer1_init();   // initializing timer
   PORTC = 1<<PC4|1<<PC5; // Setting internal pullup on PC4,PC5
   while(1){
      
       
       if(((PINC & (1<<PC5))==0)&&(!(PINC & (1<<PC4)))==0){   // PINC4 Pulled up (OFF), PINC5 Grounded (ON)
           OCR0A = 40;    // duty cycle of 40/256
       }
       if(((PINC & (1<<PC4))==0)&&(!(PINC & (1<<PC5)))==0){   // PINC4 grounded (ON), PINC5 pulled up (OFF)
            OCR0A = 110;   // duty cycle of 110/256
       }
        if(((PINC & (1<<PC4))==0)&&((PINC & (1<<PC5)))==0){    // PINC5 and PINC4 ground (ON)
            OCR0A = 255; 
        }
        else{         // PINC4,PINC5 is Pulled-up (OFF)
            OCR0A = 0;
            continue; 
        } 
        
   }

   
return 0;

}


void timer1_init(){
    DDRD = 1<<DDD6;  // set PD6 to output pin
    TCCR0A = 0x81;   // Set timer0 to FASTPWM
    TCCR0B = 0x03;   // Divide clock by 64
}