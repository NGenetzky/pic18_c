/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#elif defined(__18CXX)
    #include <p18cxxx.h>    /* C18 General Include File */
#endif

#if defined(__XC) || defined(HI_TECH_C)

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */

#endif

#include "system.h"
#include "user.h"

#include "lcd.h"
#include "i2c_genetzky.h"
#include "ringbuffer.h"
/******************************************************************************/
/* User Functions                                                             */
/******************************************************************************/

/* <Initialize variables in user.h and insert code for user algorithms.> */
extern int state;
extern int sys;

/******************************************************************************/
/* Setup Functions                                                            */
/******************************************************************************/

void setup_RB0_INT0(){
    TRISBbits.RB0 =1; // Configure button as input.
    
    // INT0
    INTCON2bits.INTEDG0 = 0;
    INTCONbits.INT0F = 0;
    INTCONbits.INT0IE = 1;
}

/******************************************************************************/
/* Interrupt Routines                                                         */
/******************************************************************************/
void button_RB0_on_click(){
    
    if(!PORTAbits.RA5){
        // Right Button pressed.
        LATD ^= 0xFF; // Invert the LEDs on PORTD
        if(sys==4){
            sys = 6;
        } else if(sys==6){
            sys = 4;
        }
        
    } else {
        if(!ringbuffer_is_empty()){
            LATD = ringbuffer_get();
        } else{
            LATD ^= 0x0F; // Invert the LEDs on PORTD
            delay_1MSx(500);
            LATD ^= 0x0F; // Invert the LEDs on PORTD
        }
        
    }  
    
}

void I2C_on_flag(){
    if(sys == 4){
        I2C_slave_event();
    } else if(sys == 5){
        I2C_master_event();
    } 
}

/******************************************************************************/
/* Loop Functions                                                             */
/******************************************************************************/
void poll_buttons(){
    if(!PORTBbits.RB0){
        // Left Button pressed.
        
    } else if(!PORTAbits.RA5){
        // Right Button pressed.
        
    } else {
        //No button pressed
    }  
}

void loop_system(){
    if(sys==4){
        system4_SLAVE_LED();
    } else if(sys==5){
        system5_MASTER_SEND();
    } else if (sys==6){
        system6_LCD_RINGBUFFER();
    }
}
/******************************************************************************/
/* Helper Functions                                                           */
/******************************************************************************/

void print_hello_world(){
    char hello_cstr[] = "Hello World";
    lcdWriteString(hello_cstr);
}

void print_i2c_inbuff(){
    lcdGoTo(0);
    lcd_init();
    while(!ringbuffer_is_empty()){
        lcdChar(ringbuffer_get());
    }

    print_hello_world();
}
/******************************************************************************/
/* Systems                                                                    */
/******************************************************************************/
void system4_SLAVE_LED(){
    if(state==1){
        I2C_setup_slave(0x07<<1);
        state=2;
    } else if(state==2){
        if(!ringbuffer_is_empty()){
            state=3;
        } else{
            state = 4;
        }
    } else if(state==3){ // Buffer has data
        if(!PORTAbits.RA5){// Right Button pressed.
            LATD = ringbuffer_get();
        } else {
            LATD ^= 0x40; // Invert 2nd from left LED
        }  
        state = 5;
    } else if (state==4){
        state = 5;
    } else if(state==5){
        delay_1MSx(500);
        state = 2;
    }
}

void system5_MASTER_SEND(){
    char data[] = {0xF0, 0xF1, 0xF2, 0xF3, 0xF4};
    if(state==1){
        I2C_setup_master();
        state=2;

    } else if(state==2){
        if(!PORTAbits.RA5){
            state = 3;
        } else {
            //No button pressed
            state =4;
        } 
    } else if(state==3){
        
    } else if (state==4){
        begin_tranmission(0x05);
        i2c_write(data, 5);
        end_transmission();
        
    } else if(state==5){
        delay_1MSx(500);
    }
}

void system6_LCD_RINGBUFFER(){
    if(state==1){
        SSP1CON1bits.SSPEN = 0; // 0 = Disables serial port and configures these pins as I/O port pins
        lcd_init();
        state = 2;
        
    } else if(state==2){
        lcdGoTo(0);
        if(!ringbuffer_is_empty()){
            lcdChar(ringbuffer_get()); 
        } else {
            lcdChar('N');
            lcdChar('o');
            lcdChar('n');
            lcdChar('e');
        }
        state = 3;
    } else if (state ==3){
        delay_1MSx(500);
        LATD ^= 0xFF;
        delay_1MSx(500);
        LATD ^= 0xFF;
    }
}