/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#if defined(__XC)
    #include <xc.h>        /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>       /* HiTech General Include File */
#elif defined(__18CXX)
    #include <p18cxxx.h>   /* C18 General Include File */
#endif

#if defined(__XC) || defined(HI_TECH_C)

#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */

#endif

#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "user.h"          /* User funct/params, such as InitApp */

#include "i2c_genetzky.h"
#include "lcd.h"
/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/

/* i.e. uint8_t <variable_name>; */

/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/

void main(void)
{
    /* Configure the oscillator for the device */
    ConfigureOscillator();

    /* Initialize I/O and Peripherals for application */
    setup();

    /* TODO <INSERT USER APPLICATION CODE HERE> */

    while(1) {
        loop();
    }
}


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
     I2C_setup_master();
     
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
    char data[] = {'A'};
    uint8_t slave = 0x2<<1;
    
    begin_tranmission(slave);
    i2c_write(data, 1);
    end_transmission();
    LATD ^= 0xFF; // Invert the LEDs on PORTD
    
    delay_1Sx(1);
}
