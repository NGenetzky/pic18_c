/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#include "lcd.h" // Functions to write to onboard LCD.
#include <spi.h>
#include <delays.h>

////LCD0325
//An LCD display with two lines, 16 characters each, is connected to the SPI I/O
//expander, MCP23S17. The two control lines and eight data lines are connected to the
//I/O expander.
//The I/O expander has an SPI interface that connects it to the microcontroller.The I/O
//expander is disabled or enabled from the SPI by jumper JP3.

// this is our chip select (CS) pin according to our pic18 explorer board's connections
#define CS_IOEXPANDER PORTAbits.RA2
// addresses from MCP23S17's datasheet, think of the IODIR as TRIS and GPIO as PORT for the MCP23S17 (no the PIC micro)
#define IODIRA_ADDRESS 0x00
#define IODIRB_ADDRESS 0x01
#define GPIOA_ADDRESS 0x12
#define GPIOB_ADDRESS 0x13
/******************************************************************************/
/* User Functions                                                             */
/******************************************************************************/

void lcd_init()
{
    TRISAbits.RA2=0; // our chip select pin needs to be an output so that we can toggle it
    CS_IOEXPANDER=1; // set CS pin to high, meaning we are sending any information to the MCP23S17 chip
 
    // configure SPI: the MCP23S17 chip's max frequency is 10MHz, let's use 10MHz/64 (Note FOSC=10Mhz, our external oscillator)
    OpenSPI1(SPI_FOSC_64, MODE_10, SMPEND); // frequency, master-slave mode, sampling type
    // set LCD pins DB0-DB7 as outputs
    setIODIR(IODIRB_ADDRESS,0x00);
    // set RS and E LCD pins as outputs
    setIODIR(IODIRA_ADDRESS,0x00);
    // RS=0, E=0
    setGPIO(IODIRA_ADDRESS,0x00);
    // Function set: 8 bit, 2 lines, 5x8
    lcdCommand(0b00111111);
    // Cursor or Display Shift
    lcdCommand(0b00001111);
    // clear display
    lcdCommand(0b00000001);
    // entry mode
    lcdCommand(0b00000110);
}

void setGPIO(char address, char value){
    CS_IOEXPANDER=0;
    WriteSPI1(0x40);    // write command 0b0100[A2][A1][A0][R/W] = 0b01000000 = 0x40
    WriteSPI1(address); // select register by providing address
    WriteSPI1(value);    // set value
    CS_IOEXPANDER=1; // we are ending the transmission
}


void setIODIR(char address, char dir){
    CS_IOEXPANDER=0;
    WriteSPI1(0x40);    // write command (0b0100[A2][A1][A0][R/W]) also equal to 0x40
    WriteSPI1(address); // select IODIRB
    WriteSPI1(dir);    // set direction
    CS_IOEXPANDER=1;
}

void lcdCommand(char command){
    setGPIO(GPIOA_ADDRESS,0x00); // E=0
    Delay10TCYx(0);
    setGPIO(GPIOB_ADDRESS, command); // send data
    Delay10TCYx(0);
    setGPIO(GPIOA_ADDRESS,0x40); // E=1
    Delay10TCYx(0);
    setGPIO(GPIOA_ADDRESS,0x00); // E=0
    Delay10TCYx(0);
}

// prints out a character to the lcd display
void lcdChar(unsigned char letter){
    setGPIO(GPIOA_ADDRESS,0x80); // RS=1, we going to send data to be displayed
    Delay10TCYx(0); // let things settle down
    setGPIO(GPIOB_ADDRESS,letter); // send display character
    setGPIO(GPIOA_ADDRESS, 0xc0); // RS=1, EN=1
    Delay10TCYx(0);
    setGPIO(GPIOA_ADDRESS,0x00); // RS=0, EN=0 // this completes the enable pin toggle
    Delay10TCYx(0);
}

void lcdGoTo(char pos){
    // add 0x80 to be able to use HD44780 position convention
    lcdCommand(0x80+pos);
}

void lcdWriteString(unsigned char *s){
    while(*s)
    lcdChar(*s++);
}
