/******************************************************************************/
/* System Level #define Macros                                                */
/******************************************************************************/

/* TODO Define system operating frequency */
// OSC = LP // LP oscillator 
// OSC = XT // XT oscillator 
// OSC = HS // HS oscillator //10000000L
// OSC = RC // External RC oscillator, CLKO function on RA6 
// OSC = EC // EC oscillator, CLKO function on RA6 
// OSC = ECIO6 // EC oscillator, port function on RA6 
// OSC = HSPLL // HS oscillator, PLL enabled (Clock Frequency = 4 x FOSC1)  //40000000L
// OSC = RCIO6 // External RC oscillator, port function on RA6 
// OSC = INTIO67 // Intern// al oscillator block, port function on RA6 and RA7 
// OSC = INTIO7 // Internal oscillator block, CLKO function on RA6, port function on RA7 

#define CLOCK_10MHZ
#if defined(CLOCK_10MHZ)
    #pragma config OSC =HS
    #define SYS_FREQ        10000000L
#elif defined(CLOCK_40MHZ)
    #pragma config OSC = HSPLL
    #define SYS_FREQ        40000000L
#elif defined(CLOCK_4MHZ)
    #pragma config OSC = LP
    #define SYS_FREQ        4000000L
#endif
/* Microcontroller MIPs (FCY) */
#define FCY             SYS_FREQ/4
#define _XTAL_FREQ      SYS_FREQ


//Unions and Structures
typedef union {//Define a Union that can hold int or 4 unsigned chars.
        unsigned char c[4];
        unsigned int i;
} ByteX4;

typedef union {
  struct {
    unsigned char b0 : 1;
    unsigned char b1 : 1;
    unsigned char b2 : 1;
    unsigned char b3 : 1;
    unsigned char b4 : 1;
    unsigned char b5 : 1;
    unsigned char b6 : 1;
    unsigned char b7 : 1;
  };
  unsigned char c;
} ByteX1;

/******************************************************************************/
/* System Function Prototypes                                                 */
/******************************************************************************/

/* Custom oscillator configuration funtions, reset source evaluation
functions, and other non-peripheral microcontroller initialization functions
go here. */

void ConfigureOscillator(void); /* Handles clock switching/osc initialization */
void delay_1MSx(int n);
void delay_1Sx(int n);