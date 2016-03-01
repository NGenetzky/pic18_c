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

/******************************************************************************/
/* User Functions                                                             */
/******************************************************************************/

/* <Initialize variables in user.h and insert code for user algorithms.> */


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
        
    } else {
        print_i2c_inbuff();
    }  
    
}

void I2C_on_flag(){
    I2C_master_event();
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
/******************************************************************************/
/* Helper Functions                                                           */
/******************************************************************************/

void print_hello_world(){
    char hello_cstr[] = "Hello World";
    lcdWriteString(hello_cstr);
}

void print_i2c_inbuff(){
    lcdGoTo(0);
    I2C_in_index=5;
    lcd_init();
    for (int i = 0; i< I2C_in_index; i++){
        lcdChar(I2C_in_buffer[i]);
    }
    I2C_in_index = 0;
    print_hello_world();
}

