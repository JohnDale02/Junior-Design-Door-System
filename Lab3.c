/**************************************************************
 * Lab3.c : this program reads the temperature in fahrenheit 
 * from a sensor and displays that temp using a 7 segment Led.
 *  an active pushbutton switch will change the temp to 
 * show celcius 
 * Date: 3/26/23     Author: John Dale     Revision: initial
 ************************************************************/
 
 #include <avr/io.h>
 #include <string.h>
 #include <util/delay.h>
 #include <stdlib.h>

 void uart_init(void);
 void uart_send(unsigned char);
 void send_string(char *stringAddress);
 void adc_init(void);
 unsigned int get_adc(void);
 void VoutToC(float);
 void VoutToF(float);
 void display(int temp);

 char dRaw[6], vRaw[6], C_temp_str[6], F_temp_str[6], Voutstr[6],digitsAD[10], digitsEDP[10],digitDP[10];
 float digitalVal,voltInt;

 int main(void){
     uart_init();
     adc_init();
     DDRD = (1<<4) | (1<<5) | (1<<6) | (1<<7); // setting ports D4,5,6,7 as outputs, others as input
     DDRC = (1<<0) | (1<<1) | (1<<2) | (1<<3); // Setting ports C0,1,2,3 as outputs, others as input
     DDRB = (1<<1) | (1<<2) | (1<<3) | (1<<4); // Setting ports B1,2,3,4 as outputs, others as input
     PORTD = 1<<2;  // setting internal pullup resistor on D2

     while(1){
         if (!((PIND & (1<<2))==0)){    //while switch is not pressed 
            digitalVal = get_adc();     // get digital sample
            voltInt = digitalVal*5./1.024;    // converting to analog value 
            uart_send(' ');
            VoutToF(voltInt);     // send analog value to function to convert to C
            uart_send(13);  //  fromatting 
            uart_send(10);  
         }
         else{    // if the button is pressed  
            digitalVal = get_adc();   // get digital sample
            voltInt = digitalVal*5./1.024; // converting digital to analog
            uart_send(' ');
            VoutToC(voltInt);
            uart_send(13); // formatting
            uart_send(10);
         }

     }
     return 0; 
     }


void display(int temp){  // function to display Temperature vales on 7 segment LED
    unsigned char i=0;
    char digitsAD[10]= {0x0F,0x06,0x0B,0x0F,0x06,0x0D,0x0D,0x07,0x0F,0x0F};
    char digitsEDP[10]={0x30,0x00,0x50,0x40,0x60,0x60,0x70,0x00,0x70,0x60};
    char digitDP[10]={0xB0,0x80,0xD0,0xC0,0xE0,0XE0,0xF0,0x80,0xF0,0xE0};
    PORTB = 0xFD;
    PORTC = (digitsAD[temp/100 % 10]);
    PORTD = (digitsEDP[temp/100 %10]);
    _delay_ms(5);
    PORTB = 0xFB;
    PORTC = (digitsAD[temp/10 %10]);
    PORTD = (digitDP[temp/10 %10]);
     _delay_ms(5);
    PORTB = 0xF7;
    PORTC = (digitsAD[temp %10]);
    PORTD = (digitsEDP[temp %10]);
     _delay_ms(5);
    
    if (!((PIND & (1<<2))==0)){
    PORTB=0xEF;
    PORTC = (0x01);
    PORTD = (0x70);
    
    _delay_ms(5);
    }
    else{
    PORTB=0xEF;
    PORTC = (0x09);
    PORTD = (0x30);
    _delay_ms(5);
    }
    PORTC = 0x00;
    PORTD =0x00;

     for (i=0; i<5;i++){
         _delay_ms(5);
         display(temp);
         
     }

}

void VoutToC(float Vout){  // function to convert voltage to C and display it
    float C_temp = ((Vout-750)/10)+25;   // conversion from voltage to C
    itoa(C_temp,C_temp_str,10);  // creating C_temp to string
    send_string(C_temp_str);   // sending C temp to uart (2 digits)
    uart_send(13); // formatting
    uart_send(10);
    display(10*C_temp);   // display temp in C

}

void VoutToF(float Vout){  // function to convert voltage to F and display it
    float F_temp = 1.8*(((Vout-750)/10)+25)+32;  // conversion for voltage to C to F
    itoa(F_temp,F_temp_str,10);   // creating F_temp to string
    send_string(F_temp_str);   // sending F temp to uart (2 digits)
    uart_send(13); // formatting
    uart_send(10);
    display(10*F_temp);   // display temp in F

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
