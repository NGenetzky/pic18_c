/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#include "i2c_genetzky.h"
/******************************************************************************/
/* User Functions                                                             */
/******************************************************************************/

void I2C_setup_slave(uint8_t address){
    // Setup MSSP in 7 bit I2C Slave mode
    // Not sure if these first set of statements are required
    // Set direction on SCL and SDA
    TRISCbits.RC3 = 1;
    TRISCbits.RC4 = 1;
    LATCbits.LC3 = 1;
    LATCbits.LC4 = 1;
    
    SSPADD         = address;       // Set I2C address
    SSPCON1        = 0b00110110;    // SSPEN: Synchronous Serial Port Enable bit - Enables the serial port and configures the SDA and SCL pins as the serial port pins
                                    // CKP: SCK Release Control bit              - Release clock
                                    // SSPM3:SSPM0: SSP Mode Select bits         - 0110 = I2C Slave mode, 7-bit address    
    SSPSTAT        = 0x00;          // Slew rate in this reg.
    SSPCON2        = 0x01;          // GCEN: General Call address (00h) (Slave mode only) 0 = General call address disabled
                                    // SEN: Start Condition Enable/Stretch Enable bit(1) ()Slave mode) 1 = Clock stretching is enabled 
    I2C1_Clear_Intr_Status_Bit;     // Clear MSSP interrupt request flag
        //PIR1bits.SSPIF = 0;
    EnableIntI2C1; // Enable the interrupt 
        // PIE1bits.SSPIE = 1;      // Enable MSSP interrupt enable bit
}
// Called from interrupt function
void I2C_on_recieve(){
    
}
void I2C_on_request(){
    
}