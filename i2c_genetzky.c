/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#include "i2c_genetzky.h"


//? Serial clock (SCLx) ? RC3/SCK1/SCL1 or RD6/SCK2/SCL2
//? Serial data (SDAx) ? RC4/SDI1/SDA1 or RD5/SDI2/SDA2

//? MSSP Control Register 1 (SSPxCON1)
//? MSSP Control Register 2 (SSPxCON2)
//? MSSP Status Register (SSPxSTAT)
//? Serial Receive/Transmit Buffer Register
//(SSPxBUF)
//? MSSP Shift Register (SSPxSR) ? Not directly
//accessible
//? MSSP Address Register (SSPxADD)
/******************************************************************************/
/* User Functions                                                             */
/******************************************************************************/

void I2C_setup_slave(uint8_t address){
    // The user must configure these pins as inputs by setting the associated TRIS bits.
    TRISCbits.RC3 = 1;
    TRISCbits.RC4 = 1;
    LATCbits.LC3 = 1;
    LATCbits.LC4 = 1;
    
    // Setup MSSP in 7 bit I2C Slave mode
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
void I2C_on_flag(){     // I2C slave interrupt handler
    //transmit data to master
    if (SSPSTATbits.R_W == 1){             // Read request from master
        I2C_on_request();
    } else if (SSPSTATbits.D_A == 1){  // D/A: Data/Address bit 
        I2C_on_recieve();
    } else if (SSPSTATbits.BF == 0){ 
        
        // BF: Buffer Full Status bit is empty // all done,
        // read buffer to clear flag. [address]
        uint8_t junk = SSPBUF;  // Nothing in buffer so exit
    } 
}
void I2C_on_recieve(){
    //receive data from master  
    // Data [not address]
    uint8_t data = ReadI2C1();
    LATD = data; 
}
void I2C_on_request(){
    WriteI2C1('S');
    SSPCON1bits.CKP = 1;                  // Release SCL line
}