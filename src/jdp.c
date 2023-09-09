#define __AVR_ATmega328P__
#include <avr/io.h> 
#include "SSD1306/i2c.h"   // HEADERFILE for the OLED display (i2c)
#include "SSD1306/SSD1306.h"  // headerfile for the OLED display
#include "SSD1306/i2c.c"  // src file for the OLED display (i2c)
#include "SSD1306/SSD1306.c"  // src file for the Oled display
#include <util/delay.h> 

#define MYDELAY 1000

#define TRIG1 PB0 // Trig 1
#define ECHO1 PB2    // Echo 1
#define TRIG2 PB1 // Trig 2 -- Sends out pulse
#define ECHO2 PB3    // Echo 2 -- Recieves pulse back
#define DIST_PER_CLOCK 1.098

int capacity = 5;

void uart_init(void);   // function to initilize UART agreements
void uart_send(unsigned char letter);   // function to send character to UART
void send_string(char *stringAddress);  // function to send a string to UART
void timer0_init();
void ultrasonic_init(); // setting PortB DDRB registers
void LED_7segment_init();
void display(int people); // function to display Temperature vales on 7 segment LED
int get_entry_distance();
int get_exit_distance();
void adc_init(void); // function to initialize ADC
unsigned int get_adc();  // function to get data from ADC
int check_temp();  // function to query adc and give healthy or unhealthy response
void update_displays(int people);  // function for displaying the capacity indicator on OLED display & 7 segment led
void display_health_prompt();
void display_unhealthy_prompt();
void display_healthy_prompt();
void open_door();
void motor_init();

int main() {
    int people = 0;
    ultrasonic_init();
    LED_7segment_init();
    OLED_Init();
    adc_init();  // initialize the ADC for tmp36
    motor_init();

    while(1){
        int entry_distance = get_entry_distance();   // function for getting distance of person from entrance and exit
        int exit_distance = get_exit_distance();
        update_displays(people);   // display our capacity on OLED and 7 segment LED

        if (entry_distance < 90 || exit_distance < 90 ){  // if disance less than 90 cm (3 feet away)
            if (exit_distance < 90){   // if someone is exiting, we give them preference 
                open_door();
                if (people > 0){
                    people--;
                }
            }
            else{  // if someone is trying to enter
                if (people < capacity){
                    display_health_prompt();  // ask the user to grab the TMP36 sensor 
                    int health = check_temp();
                    if (health == 1){  // if healthy individual
                        display_healthy_prompt();
                        open_door();
                        people++;
                        _delay_ms(4000);
                    }    
                    else{  // if unhealthy individual
                        display_unhealthy_prompt();
                        _delay_ms(4000);
                    }
                }
            }
        }
    }
    return 0;
}
void open_door(){
    PORTB |= (1<<PB5);
    _delay_ms(50);
    PORTB &= ~(1<<PB5);
}
void display_healthy_prompt(){
    OLED_Clear();
    OLED_GoToLine(1);
    OLED_DisplayString("Your may ENTER...");
    OLED_GoToLine(3);
    OLED_DisplayString("Enjoy!");
}
void display_unhealthy_prompt(){
    OLED_Clear();
    OLED_GoToLine(1);
    OLED_DisplayString("You CANNOT enter...");
    OLED_GoToLine(2);
    OLED_DisplayString("You're SICK..");
    OLED_GoToLine(3);
    OLED_DisplayString("Please come back...");
    OLED_GoToLine(4);
    OLED_DisplayString("once you're healthy...");
}
void display_health_prompt(){
    OLED_Clear();
    OLED_GoToLine(1);
    OLED_DisplayString("In order to ENTER...");
    OLED_GoToLine(2);
    OLED_DisplayString("Hold the Temeperature");
    OLED_GoToLine(3);
    OLED_DisplayString("    Sensor Below...");
    OLED_GoToLine(4);
    OLED_DisplayString("    For 3 Seconds");
}
void update_displays(int people){
    OLED_Clear();
    display(people);  // display number of people on 7 segment led
    if (people <= 2){
        OLED_GoToLine(3);
        OLED_DisplayString("          LOW");
    }
    else if (people > 2 && people < 4){
        OLED_GoToLine(3);
        OLED_DisplayString("          MEDIUM");
    }
    else if (people == 4){
        OLED_GoToLine(3);
        OLED_DisplayString("          HIGH");
    }
    else if (people == 5){
        OLED_GoToLine(2);
        OLED_DisplayString("    YOU CANNOT ENTER");
        OLED_GoToLine(4);
        OLED_DisplayString("  WE HAVE REACHED CAPACITY");
        OLED_GoToLine(5);
        OLED_DisplayString("     CAPACITY");
    }
}
int check_temp(){   // function to check the termpature of customer
    _delay_ms(1000);
    int digitalVal = get_adc();   // get digital sample
    float Vout = digitalVal*5./1.024; // converting digital to analog
    float F_temp = 1.8*(((Vout-750)/10)+25)+32;  
    OLED_Clear();
    OLED_DisplayFloatNumber(F_temp);
    _delay_ms(3000);
    if (F_temp > 100){  // unhealthy = 0
        return 0;
    }
    else{   // healthy = 1
        return 1;
    }
}
void uart_init(void){    // function to initilize UART agreements
    UCSR0B = (1<<TXEN0) ;  // Enable TX
    UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);   // 8 ASCII bits, 1 stop bit, no parity
    UBRR0L = 103;  // sets baud to 9600
}
void uart_send(unsigned char letter){   // function to send character to UART
    while(!(UCSR0A & (1<<UDRE0)));  // wait for empty Data buffer
    UDR0 = letter; // sends char to Data register
}
void send_string(char *stringAddress){  // function to send a string to UART
    unsigned char i = 0;
    for ( i=0; i < strlen(stringAddress); i++) {// for each char in temp, send to uart
        uart_send(stringAddress[i]);  // sending char using uart_send 
    }
}
void timer0_init(){
    TCCR0A = 0; //Timer 1 Normal mode 
    TCCR0B = 5; //Divide the clock by 1024 
    TCNT0 = 0; //Start the timer at 0 
}
void ultrasonic_init(){  // setting PortB DDRB registers
    DDRB = 0b00000000;
    DDRB |= 1<<TRIG1;  //Trig1 is output pin  
    DDRB |= 1<<TRIG2;  //Trig2 is output pin  
}
void motor_init(){
    DDRB |= (1<<PB5);  // make motor pin B5 an output
}
void adc_init(void){ // function to initialize ADC
     ADMUX = (1 << REFS0);    // Vref = AVCC = 5v
     ADMUX |= (1 << MUX0);  // set analog input pin to C0 (pin23) for tmp36
     ADCSRA = 0x87;   // Enables ADC; sets speed to 125 KHz
}
unsigned int get_adc(){  // function to get data from ADC
    ADCSRA |= (1<<ADSC);    // start ADC conversion 
    while ((ADCSRA & (1<<ADIF))==0);  // wait for ADC to finish
    return ADCL | (ADCH <<8);  // Read ADCL first

}
void LED_7segment_init(){
    DDRD = 0xFF;  // set all pins in PortD to output pins
}
void display(int people){  // function to display Temperature vales on 7 segment LED
    char digitsTens[10]= {0xBF,0x86,0xDB,0xCF,0xC6,0xED,0xFC,0x87,0xFF,0xE7};
    char digitsOnes[10]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7C,0x07,0x7F,0x67};

    PORTD = (digitsOnes[people]);
}
int get_entry_distance(){
    unsigned char rising_edge_clocks, falling_edge_clocks, echo_width_clocks;
    int target_range = 0;  
    PORTB &= ~(1<<TRIG1); //Set TRIG pin low. (Already sent out a wave. Dont want to send out a wave anymore)

    TCNT0 = 0; // count from 0
    PORTB |= 1<<TRIG1; // send out a pulse; set pin high
    _delay_us(10); // 10 microseconds
    PORTB &= ~(1<<TRIG1);
    timer0_init(); //initialize timer 

    //Wait till the ECHO Pulse goes high 
    while((PINB & (1<<ECHO1)) == 0); //pin register detects it =0
    rising_edge_clocks = TCNT0; //Note the rising edge time 

    while (!(PINB & (1<<ECHO1))==0);
    falling_edge_clocks = TCNT0; //Note the falling edge time 

    if (falling_edge_clocks > rising_edge_clocks){
        //Compute target range
        echo_width_clocks = falling_edge_clocks - rising_edge_clocks; 
        target_range = echo_width_clocks * DIST_PER_CLOCK;          
    }
    _delay_ms(200); 

    return target_range;
}
int get_exit_distance(){
    unsigned char rising_edge_clocks, falling_edge_clocks, echo_width_clocks;
    int target_range = 0;  
    PORTB &= ~(1<<TRIG2); //Set TRIG pin low. (Already sent out a wave. Dont want to send out a wave anymore)

    TCNT0 = 0; // count from 0
    PORTB |= 1<<TRIG2; // send out a pulse; set pin high
    _delay_us(10); // 10 microseconds
    PORTB &= ~(1<<TRIG2);
    timer0_init(); //initialize timer 

    //Wait till the ECHO Pulse goes high 
    while((PINB & (1<<ECHO2)) == 0); //pin register detects it =0
    rising_edge_clocks = TCNT0; //Note the rising edge time 

    while (!(PINB & (1<<ECHO2))==0);
    falling_edge_clocks = TCNT0; //Note the falling edge time 

    if (falling_edge_clocks > rising_edge_clocks){
        //Compute target range
        echo_width_clocks = falling_edge_clocks - rising_edge_clocks; 
        target_range = echo_width_clocks * DIST_PER_CLOCK;        
    }
    _delay_ms(200); 
    return target_range;
}
