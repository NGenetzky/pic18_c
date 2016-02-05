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

#include "user.h"
#include "lcd.h"
#include "system.h"

/******************************************************************************/
/* User Functions                                                             */
/******************************************************************************/

/* <Initialize variables in user.h and insert code for user algorithms.> */

void setup(void)
{
    /* TODO Initialize User Ports/Peripherals/Project here */
    TRISAbits.RA5 = 1;  // Configure button as input.
    TRISBbits.RB0 = 1;  // Configure button as input.
    //Only Affect PCFG3:PCFG0: A/D Port Configuration Control bits
    //Change all ANx ports to digital I/O.
    ADCON1 |= 0x0F;  
            
    lcd_init();
    /* Setup analog functionality and port direction */

    /* Initialize peripherals */

    /* Configure the IPEN bit (1=on) in RCON to turn on/off int priorities */

    /* Enable interrupts */
    RCONbits.IPEN = 1; //Enable High/Low Priority distinction
    
    setup_RB0_INT0();
    
    INTCONbits.GIE = 1; //Enable HighPriority Global Interrupts
    INTCONbits.GIEL = 1; //Enable LowPriority Global Interrupts
    
    // Other setup tasks
    TRISD = 0x00;
    LATD = 0xAA;
}

void loop(){
    LATD ^= 0xFF;
    poll_buttons();
    delay_1Sx(1);
}

void print_hello_world(){
    char hello_cstr[] = "Hello World";
    lcdWriteString(hello_cstr);
}

void setup_RB0_INT0(){
    TRISBbits.RB0 =1; // Configure button as input.
    
    // INT0
    INTCON2bits.INTEDG0 = 0;
    INTCONbits.INT0F = 0;
    INTCONbits.INT0IE = 1;
}

void button_RB0_on_click(){
    LATD = 0xF0;
}

void poll_buttons(){
    if(!PORTBbits.RB0){
        // Left Button pressed.
        
    } else if(!PORTAbits.RA5){
        // Right Button pressed.
        
    } else {
        //No button pressed
    }  
}