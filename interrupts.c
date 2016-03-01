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

#include "user.h"

/******************************************************************************/
/* Interrupt Routines                                                         */
/******************************************************************************/

// High-priority service
void interrupt high_isr(void)
{
      /* TODO Add High Priority interrupt routine code here. */

    if(INTCONbits.INT0IF){ //Button Press on RB0
      button_RB0_on_click();
      INTCONbits.INT0IF =0;  //Clear flag
    } else if(PIR1bits.SSP1IF){ // I2C1_Intr_Status
        I2C_on_flag();
        PIR1bits.SSP1IF=0; //I2C1_Clear_Intr_Status_Bit;
    } else {
        // Unhandled interrupts
    }


}

/* Low-priority interrupt routine */
void low_priority interrupt low_isr(void)
{
      /* This code stub shows general interrupt handling.  Note that these
      conditional statements are not handled within 3 seperate if blocks.
      Do not use a seperate if block for each interrupt flag to avoid run
      time errors. */

}
