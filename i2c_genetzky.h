/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef I2C_GENETZKY_H
#define	I2C_GENETZKY_H

// TODO Insert appropriate #include <>
#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdint.h>

#include <i2c.h>
#include "system.h"

// TODO Insert C++ class definitions if appropriate

//Define the speed mode used for I2C. See page 234 of PIC18F8722 Datasheet.
// I2C_SPEED and I2C_DIVIDER is only used in master mode.
#define I2C_SPEED_100KHZ
#if defined(CLOCK_10MHZ) // Frequency is set in system.h
    #if defined(I2C_SPEED_100KHZ)
        #define I2C_DIVIDER 0x63;
    #endif
#elif defined(CLOCK_40MHZ)
    #if defined(I2C_SPEED_100KHZ)
        #define I2C_DIVIDER 0x27;
    #endif
#elif defined(CLOCK_4MHZ)
    #if defined(I2C_SPEED_100KHZ)
        #define I2C_DIVIDER 0x09;
    #endif
#endif

#define I2C_BUFFER_SIZE 32
volatile int I2C_in_index=0, I2C_out_index=0;
volatile uint8_t I2C_in_buffer[I2C_BUFFER_SIZE+1];
uint8_t I2C_out_buffer[I2C_BUFFER_SIZE];


// TODO Insert declarations
uint8_t I2C_create_address(uint8_t address, uint8_t read_from);
void I2C_setup_master();
void I2C_setup_slave(uint8_t address);
void I2C_master_event();
void I2C_slave_event();
void I2C_on_recieve();
void I2C_on_request();
int I2C_master_request_from(uint8_t slave_address);
void begin_tranmission(uint8_t slave_address);
void i2c_write(uint8_t* data, uint8_t len);
void end_transmission();

#endif	/* I2C_GENETZKY_H */

