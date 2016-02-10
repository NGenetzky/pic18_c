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

// TODO Insert C++ class definitions if appropriate
#define I2C_BUFFER_SIZE 8
uint8_t i2c_buffer[I2C_BUFFER_SIZE];

// TODO Insert declarations
void I2C_setup_slave(uint8_t address);
void I2C_on_event();
void I2C_on_recieve();
void I2C_on_request();
int I2C_master_request_from(uint8_t slave_address);
void I2C_master_write(uint8_t slave_address);

#endif	/* I2C_GENETZKY_H */

