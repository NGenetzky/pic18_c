/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#include "i2c_genetzky.h"
#include "ringbuffer.h"

// Pins:
// Serial clock (SCLx) ? RC3/SCK1/SCL1 or RD6/SCK2/SCL2
// Serial data (SDAx) ? RC4/SDI1/SDA1 or RD5/SDI2/SDA2

// Registers
// MSSP Control Register 1 (SSPxCON1)
// MSSP Control Register 2 (SSPxCON2)
// MSSP Status Register (SSPxSTAT)
// Serial Receive/Transmit Buffer Register (SSPxBUF)
// MSSP Shift Register (SSPxSR) ? Not directly accessible
// MSSP Address Register (SSPxADD)

//Library Functions
//WriteI2C1(); // 2 means NACK, 1 means Write Collision. 0 is success.
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
                                    // b0: 0 = Slew rate control enabled for High-Speed mode (400 kHz)
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
    SSPCON2        = 0x00;          // GCEN: General Call address (00h) (Slave mode only) 0 = General call address disabled
                                    // b0_SEN: Stretch Enable bit(1) (Slave mode) 0 = Clock stretching is disabled 

    PIR1bits.SSPIF = 0;     // Clear MSSP interrupt request flag   
    PIE1bits.SSPIE = 1;         // Enable SSP interrupt
    IPR1bits.SSPIP = 1;         // Set SSP interrupt priority to high
}

void I2C_master_event(){
    //Start condition
    if(SSP1CON2bits.SEN){
        // 1. The user generated a Start condition by setting the Start Enable bit, SEN (SSPxCON2<0>).
        // The user loads the SSPxBUF with the slave address to transmit.
        LATD=0xD1;
    } 
    //Stop condition
    else if(SSP1CON2bits.PEN){
        // 11. The user generated a Stop condition by setting the Stop Enable bit, PEN (SSPxCON2<2>).
        LATD=0xD2;
    }
    //Data transfer byte transmitted/received
        //NOT CAUGHT
        
    //Acknowledge transmit
    else if(SSP1CON2bits.ACKSTAT){ //ACKSTAT: Acknowledge Status bit (Master Transmit mode only)
        if(SSPSTATbits.D_A){
            //Slave has ACK that data was received
            LATD=0xD3;
        } else {
            //OR Slave has ACK that address was received
            LATD=0xD4;
        }
    } else{
        LATD=0xD5;
    }
    //Repeated Start
        //NOT CAUGHT
}

//State | Operation | Last byte | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 0
//------+-----------+-----------+-------+-------+-------+-------+-------
//1     | M write   | address   |   0   |   0   |   1   |   0   |   1
//2     | M write   | data      |   1   |   0   |   1   |   0   |   1
//3     | M read    | address   |   0   |   0   |   1   |   1   |   0
//4     | M read    | data      |   1   |   0   |   1   |   1   |   0
//5     | none      | -         |   ?   |   ?   |   ?   |   ?   |   ?
void I2C_slave_event(){     // I2C slave interrupt handler (ISR)
    unsigned char dataFromMaster, dataToMaster;
    unsigned char temp = SSPSTAT & 0b00101101;
    if ((temp ^ 0x09) == 0x00) {
        // State 1: write operation, last byte was address
        dataFromMaster = ReadI2C(); // Do a dummy read of SSPBUF
        
    } else if ((temp ^ 0x29) == 0x00) { 
        // State 2: write operation, last byte was data
        dataFromMaster = ReadI2C();
        ringbuffer_put(dataFromMaster); // Add data to ring buffer
   
    } else if (((temp & 0x2c) ^ 0x0c) == 0x00) {
        // State 3: read operation, last byte was address
        dataToMaster = ringbuffer_get(); // Add data to ring buffer
        WriteI2C(dataToMaster);
        LATD = 3;

    } else if (!SSPCON1bits.CKP) {
        // State 4: read operation, last byte was data
        dataToMaster = ringbuffer_get(); // Add data to ring buffer
        WriteI2C(dataToMaster);
        LATD = 4;

    } else {                                        
        // State 5: slave logic reset by NACK from master
        LATD = 5;
    }
}
void I2C_on_recieve(){
    //receive data from master (not an address)
    uint8_t data = ReadI2C1();
    LATD = data;
    ringbuffer_put(data);
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
    WriteI2C( slave_address & 0xFE );  // Send address with R/W cleared for write
}
// Must call begin_tranmission before calling this function and then afterwards
// end_transmission must be called before data is sent.
void i2c_write(uint8_t* data, uint8_t len){
    for(int i=0; i<len; i++){
        WriteI2C( data[i] );                   // Write a byte of data at a time
    }
}
void end_transmission(){
    IdleI2C();                         // Wait for ACK
    StopI2C();                         // Hang up, send STOP condition
}