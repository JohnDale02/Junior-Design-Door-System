//This code uses the Infrasound sensor to detect objects between 5 and 200 cm 
//Outputs the distance of an object to an OLED screen. Uses battery power.  

#include <avr/io.h> 
#include "i2c.h"
#include "SSD1306.h"
#include <util/delay.h> 

void timer0_init(); 

#define TRIG PB1 //PB1 = pin15 //trigger = transmitter, sends out an echo
#define ECHO PB0    //PB0 = pin14  //echo = receiver, receives echo that bounces off an object, certain distance away
#define RANGE_PER_CLOCK 1.098

float measureDistance(void);

int main(void){
    OLED_Init();
    float distance = 0;
    while (1) {
       
        
        distance = measureDistance();
        if (distance>5 && distance<200){
            OLED_GoToLine(0);
            OLED_DisplayString("Distance (cm):");
            OLED_GoToLine(1);
            OLED_DisplayFloatNumber(distance);

            OLED_GoToLine(2);
            OLED_DisplayString("Distance (in):");
            OLED_GoToLine(3);
            OLED_DisplayFloatNumber(distance/2.54);
            _delay_ms(100); 
        }

    }
    return 0; //never reached
}

float measureDistance(void){
    unsigned char rising_edge_clocks, falling_edge_clocks, echo_width_clocks;
    float target_range = 0;
    DDRB = 1<<TRIG; //Trig is output pin    
    PORTB &= ~(1<<TRIG); //Set TRIG pin low. (Already sent out a wave. Dont want to send out a wave anymore)


    //A timer:
    TCNT0 = 0; //Load counter with 0 
    PORTB |= 1<<TRIG; // "Or a '1' to TRIG pin" (Sends out a wave) These three lines of code 
    _delay_us(10); //Put a 10 usec pulse on the 
    PORTB &= ~(1<<TRIG); //TRIG PIN low (stop sending out more waves)
    timer0_init(); //initialize timer 

    //Wait till the ECHO Pulse goes high (echo pulse sent out)
    while((PINB & (1<<ECHO)) ==0); //pin register detects it =0
    rising_edge_clocks = TCNT0; //Note the rising edge time 
    //Now wait till the ECHO pulse goes low (echo pulse received, falling edge)
    while (!(PINB & (1<<ECHO))==0);
    falling_edge_clocks = TCNT0; //Note the falling edge time 

    if (falling_edge_clocks > rising_edge_clocks){
        //Compute target range
        echo_width_clocks = falling_edge_clocks - rising_edge_clocks; 
        target_range = echo_width_clocks * RANGE_PER_CLOCK; 
            
    }
    _delay_ms(200); //Delay then go again
    return target_range;
}

void timer0_init(){
    TCCR0A = 0; //Timer 1 Normal mode (count up)
    TCCR0B = 5; //Divide the clock by 1024 
    TCNT0 = 0; //Start the timer at 0 
}







