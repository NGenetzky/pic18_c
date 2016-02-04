#ifndef LCD_H
#define	LCD_H

/******************************************************************************/
/* User Level #define Macros                                                  */
/******************************************************************************/


/******************************************************************************/
/* User Function Prototypes                                                   */
/******************************************************************************/
void lcd_init();
void setIODIR(char, char);
void setGPIO(char, char);
void lcdCommand(char);
void lcdChar(unsigned char);
void lcdGoTo(char);
void lcdWriteString(unsigned char*);

#endif	/* LCD_H */

