
 #include <avr/io.h>
 #include <string.h>
 #include <util/delay.h>
 #include <stdlib.h>

 void uart_init(void);
 void uart_send(unsigned char);
 void send_string(char *stringAddress);
 void adc_init(void);
 unsigned int get_adc(void);
 void display(int temp); 
                                                                               
void main() {

    uart_init();  // initialize the uart for serial output 
    adc_init();  // initialize the ADC for tmp36

    int entry_distance = get_entry_distance();   // function for getting distance of person from entrance and exit
    int exit_distance = get_exit_distance();

    update_displays();   // display our capacity on OLED and 7 segment LED

    if (entry_distance < 70 || exit_distance < 70 ){  // if disance less than 70 (3 feet away)

        display_health_prompt();  // ask the user to grab the TMP36 sensor 
        
        if (exit_distance < 70){   // if someone is exiting, we give them preference 

            open_door();
        }

        else{  // if someone is trying to enter

            bool health = check_temp();

            if (health == true){  // if healthy individual

                increment_counter();
                update_displays();
            }
        }

    }

}



void send_string(char *stringAddress){  // function to send a string to UART
    unsigned char i = 0;
    for ( i=0; i < strlen(stringAddress); i++) {// for each char in temp, send to uart
        uart_send(stringAddress[i]);  // sending char using uart_send 
    }
}

void adc_init(void){ // function to initialize ADC
     ADMUX = 0x45;    // Select ADC5; Vref = AVCC = 5v
     ADCSRA = 0x87;   // Enables ADC; sets speed to 125 KHz
}

unsigned int get_adc(){  // function to get data from ADC
    ADCSRA |= (1<<ADSC);    // start ADC conversion 
    while ((ADCSRA & (1<<ADIF))==0);  // wait for ADC to finish
    return ADCL | (ADCH <<8);  // Read ADCL first

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
