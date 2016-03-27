/******************************************************************************/
/* User Level #define Macros                                                  */
/******************************************************************************/

/* TODO Application specific user parameters used in user.c may go here */

/******************************************************************************/
/* User Function Prototypes                                                   */
/******************************************************************************/

/* TODO User level functions prototypes (i.e. InitApp) go here */

void setup(void);       /* I/O and Peripheral Initialization */
void loop();
void poll_buttons();
void print_hello_world();
void setup_RB0_INT0();
void button_RB0_on_click();
void I2C_on_flag();
void print_i2c_inbuff();

void loop_system();
void system4_SLAVE_LED();
void system5_MASTER_SEND();
void system6_LCD_RINGBUFFER();