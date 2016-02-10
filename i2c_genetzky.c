/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#include "i2c_genetzky.h"


// Serial clock (SCLx) ? RC3/SCK1/SCL1 or RD6/SCK2/SCL2
// Serial data (SDAx) ? RC4/SDI1/SDA1 or RD5/SDI2/SDA2

// MSSP Control Register 1 (SSPxCON1)
// MSSP Control Register 2 (SSPxCON2)
// MSSP Status Register (SSPxSTAT)
// Serial Receive/Transmit Buffer Register (SSPxBUF)
// MSSP Shift Register (SSPxSR) ? Not directly accessible
// MSSP Address Register (SSPxADD)

/******************************************************************************/
/* I2C library patch                                                          */
/******************************************************************************/
#define IdleI2C()    while ((SSPCON2 & 0x1F) | (SSPSTATbits.R_W))

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
    SSPCON1        = 0b00100110;    // SSPEN: Synchronous Serial Port Enable bit - Enables the serial port and configures the SDA and SCL pins as the serial port pins
                                    // b4_CKP: SCK Release Control bit {1:Release clock , 0:Holds clock low (clock stretch), used to ensure data setup time}
                                    // SSPM3:SSPM0: SSP Mode Select bits         - 0110 = I2C Slave mode, 7-bit address    
                                        
    SSPSTAT        = 0x00;          // Slew rate in this reg. //Disable SMBus specific inputs
    SSPCON2        = 0x01;          // GCEN: General Call address (00h) (Slave mode only) 0 = General call address disabled
                                    // SEN: Start Condition Enable/Stretch Enable bit(1) ()Slave mode) 1 = Clock stretching is enabled 
    I2C1_Clear_Intr_Status_Bit;     // Clear MSSP interrupt request flag
        //PIR1bits.SSPIF = 0;
    EnableIntI2C1; // Enable the interrupt 
        // PIE1bits.SSPIE = 1;      // Enable MSSP interrupt enable bit
}

// Called from interrupt function
void I2C_on_event(){     // I2C slave interrupt handler
    if (SSPSTATbits.R_NOT_W == 1){  // Read request from master
        I2C_on_request(); //    transmit data to master
    } else if (SSPSTATbits.D_A == 1){  // D/A: Data/Address bit 
        // Received data from address
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
    WriteI2C1('C');
    // If clock stretching is enabled then the clock must be released:
    SSPCON1bits.CKP = 1;                  // Release SCL line
}
int I2C_master_request_from(uint8_t slave_address){
    uint8_t received_byte;

    IdleI2C();                         // Wait until the bus is idle
    StartI2C();                        // Send START condition
    IdleI2C();                         // Wait for the end of the START condition
    WriteI2C( slave_address | 0x01 );  // Send address with R/W set for read
    IdleI2C();                         // Wait for ACK
    received_byte = ReadI2C();               // Read first byte of data
    
    //AckI2C();                          // Send ACK
    //data[n] = ReadI2C();               // Read nth byte of data
    
    NotAckI2C();                       // Send NACK
    StopI2C();                         // Hang up, send STOP condition
    return received_byte;
}
void I2C_master_write(uint8_t slave_address){
    IdleI2C();                         // Wait until the bus is idle
    StartI2C();                        // Send START condition
    IdleI2C();                         // Wait for the end of the START condition
    WriteI2C( slave_address & 0xfe );  // Send address with R/W cleared for write
    IdleI2C();                         // Wait for ACK
    WriteI2C( '8' );               // Write first byte of data
    
    //IdleI2C();                         // Wait for ACK
    //WriteI2C( data[n] );               // Write nth byte of data
    
    IdleI2C();                         // Wait for ACK
    StopI2C();                         // Hang up, send STOP condition
}