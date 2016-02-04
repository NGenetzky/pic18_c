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
    
    // One time execution statements
    print_hello_world();
    
    TRISD = 0x00;
    LATD = 0xAA;
}

void loop(){
    LATD ^= 0xFF;
    lcdChar('.');
    delay_1Sx(1);
}

void print_hello_world(){
    char hello_cstr[] = "Hello World";
    lcdWriteString(hello_cstr);
}