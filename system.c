/******************************************************************************/
/*Files to Include                                                            */
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

/* Refer to the device datasheet for information about available
oscillator configurations. */
void ConfigureOscillator(void)
{
    /* TODO Add clock switching code if appropriate.  */

    /* Typical actions in this function are to tweak the oscillator tuning
    register, select new clock sources, and to wait until new clock sources
    are stable before resuming execution of the main project. */
}


#define CLOCK_10MHZ
#if defined(CLOCK_10MHZ)
void delay_1MSx(int n){
    for(int i =0; i<n; i++){
        Delay10KTCYx(1); // Delay 10,000 Cycles = 0.001 seconds
    }
}
void delay_1Sx(int n){
    for(int i =0; i<n; i++){
        Delay10KTCYx(250); // Delay 2,500,000 Cycles = 0.25 seconds
        Delay10KTCYx(250); // Delay 2,500,000 Cycles = 0.25 seconds
        Delay10KTCYx(250); // Delay 2,500,000 Cycles = 0.25 seconds
        Delay10KTCYx(250); // Delay 2,500,000 Cycles = 0.25 seconds
    }
}
#elif defined(CLOCK_40MHZ)
void delay_1MSx(int n){
    for(int i =0; i<n; i++){
        Delay10KTCYx(4); // Delay 40,000 Cycles = 0.001 seconds
    }
}
void delay_1Sx(int n){
    for(int i =0; i<n; i++){
        for(counter = 0; counter<16; counter++){
            Delay10KTCYx(250);  // Delay 2,500,000 Cycles = 0.0625 seconds
        }
    }
}
#elif defined(CLOCK_4MHZ)
    

#endif
