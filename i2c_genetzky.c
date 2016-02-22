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
/* I2C Functions                                                              */
/******************************************************************************/
uint8_t I2C_create_address(uint8_t address, uint8_t read_from){
    return (address<<1) + read_from;
}

void I2C_setup_master(){
    // The user must configure these pins as inputs by setting the associated TRIS bits.
    TRISCbits.RC3 = 1;
    TRISCbits.RC4 = 1;
    LATCbits.LC3 = 1;
    LATCbits.LC4 = 1;
    
    // Setup MSSP in Master i2C
    SSP1ADD         = I2C_DIVIDER;   // Set the speed. clock = FOSC/(4 * (SSPxADD + 1))
    SSPCON1        = 0b00101000;    // SSPEN: Synchronous Serial Port Enable bit - Enables the serial port and configures the SDA and SCL pins as the serial port pins
                                    // b4_CKP: SCK Release Control bit {1:Release clock , 0:Holds clock low (clock stretch), used to ensure data setup time}
                                    // SSPM3:SSPM0: SSP Mode Select bits         - 1000 = I2C Master mode, clock = FOSC/(4 * (SSPxADD + 1))
                                        
    SSPSTAT        = 0x00;          // Slew rate in this reg. //Disable SMBus specific inputs
    SSPCON2        = 0b00000000;    // GCEN: General Call address (00h) (Slave mode only) 0 = General call address disabled
                                    // b0_SEN: Start Condition Enable/Stretch Enable bit 
    I2C1_Clear_Intr_Status_Bit;     // Clear MSSP interrupt request flag
        //PIR1bits.SSPIF = 0;
    EnableIntI2C1; // Enable the interrupt 
        // PIE1bits.SSPIE = 1;      // Enable MSSP interrupt enable bit
}

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
// The following events will cause the SSP Interrupt Flag bit, SSPxIF, to be set 
// (and SSP interrupt, if enabled):
    // Start condition; Stop condition; Data transfer byte transmitted/received;
    // Acknowledge transmit; Repeated Start

    if(SSP1CON1bits.SSPOV){// Input Overflow
        //In Receive mode:
        // 1 = A byte is received while the SSPxBUF register is still holding 
        // the previous byte (must be cleared in software)
        SSP1CON1bits.SSPOV = 0; // I think it the i2c module needs to be reset.
    }
    if(SSP1CON1bits.WCOL){// Write Collision
        //In Master Transmit mode:
        // A write to the SSPxBUF register was attempted while the I2C 
        // conditions were not valid for a transmission to be started 
        // (must be cleared in software)
        //In Slave Transmit mode:
        // The SSPxBUF register is written while it is still transmitting the 
        // previous word (must be cleared in software)
        SSP1CON1bits.WCOL = 0;
    }
    
    if(SSPSTATbits.S == 0){
        // Something bad. Start bit was not detected last.
    }
    
    if(SSPSTATbits.R_NOT_W == 1){
        // Read request from master
        if(SSPSTATbits.D_A == 1){
            if(SSPSTATbits.BF == 0){ //S = 1, D_A = 1, R_W = 1, BF = 0
                //State 4:      I2C read operation, last byte was a data byte.
                I2C_on_request(); //    transmit data to master
                LATD = 4;
                
            }else{
                // Currently requests go here.
                I2C_on_request(); //    transmit data to master
                LATD = 0xF3;
            }
        } else { 
            if(SSPSTATbits.BF == 0){ //S = 1, D_A = 0, R_W = 1, BF = 0
                //State 3:      I2C read operation, last byte was an address byte.
                I2C_on_request(); //    transmit data to master
                LATD = 3;
            } else{ //S = 1, D_A = 0, R_W = 1, BF = 1
                //State 3b:  I2C read operation, last byte was an address byte. MMSP w/ Global call
                I2C_on_request(); //    transmit data to master
                LATD = 7;
            }
        }
    } else{
        // Master writing data to slave
        if(SSPSTATbits.D_A == 1){ 
            
            if(SSPSTATbits.BF == 1){ //S = 1, D_A = 1, R_W = 0, BF = 1
                //State 2:      I2C write operation, last byte was a data byte.
                I2C_on_recieve();
                LATD = 2;
            }else{
                //State 5:  Slave I2C logic reset by NACK from master.
                LATD = 5;
            }
        } else {
            if(SSPSTATbits.BF == 1){//S = 1, D_A = 0, R_W = 0, BF = 1
                //State 1:      I2C write operation, last byte was an address byte.
                I2C_on_recieve();
                LATD = 1;
            }else{
                LATD = 0xF1;
            }
        }
    }
    uint8_t junk = SSPBUF;  // Nothing in buffer so exit
}
void I2C_on_recieve(){
    //receive data from master (not an address)
    uint8_t data = ReadI2C1();
    LATD = data;
    I2C_in_buffer[I2C_in_index]= data;
    
    I2C_in_index++;
    SSPCON1bits.CKP = 1;                  // Release SCL line
}
void I2C_on_request(){
    WriteI2C1('Z');

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
    received_byte = ReadI2C();         // Read first byte of data
    
    //AckI2C();                          // Send ACK
    //data[n] = ReadI2C();               // Read nth byte of data
    
    NotAckI2C();                       // Send NACK
    StopI2C();                         // Hang up, send STOP condition
    return received_byte;
}

//Wire.beginTransmission(I2C_target); // transmit to slave device
//Wire.write(I2C_out_buffer, len);
//error_code = Wire.endTransmission(); // stop transmitting
void begin_tranmission(uint8_t slave_address){
    IdleI2C();                         // Wait until the bus is idle
    StartI2C();                        // Send START condition
    IdleI2C();                         // Wait for the end of the START condition
    WriteI2C( slave_address & 0xfe );  // Send address with R/W cleared for write
}
// Must call begin_tranmission before calling this function and then afterwards
// end_transmission must be called before data is sent.
void i2c_write(uint8_t* data, uint8_t len){
    for(int i=0; i<len; i++){
        WriteI2C( data[i] );                   // Write first byte of data
    }
}
void end_transmission(){
    IdleI2C();                         // Wait for ACK
    StopI2C();                         // Hang up, send STOP condition
}
void I2C_master_write(uint8_t slave_address){
    IdleI2C();                         // Wait until the bus is idle
    StartI2C();                        // Send START condition
    IdleI2C();                         // Wait for the end of the START condition
    WriteI2C( slave_address & 0xfe );  // Send address with R/W cleared for write
    IdleI2C();                         // Wait for ACK
    WriteI2C( '8' );                   // Write first byte of data
    
    //IdleI2C();                         // Wait for ACK
    //WriteI2C( data[n] );               // Write nth byte of data
    
    IdleI2C();                         // Wait for ACK
    StopI2C();                         // Hang up, send STOP condition
}