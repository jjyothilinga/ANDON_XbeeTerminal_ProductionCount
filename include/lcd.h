
#ifndef LCD_DRIVER_H
#define LCD_DRIVER_H


/*
*------------------------------------------------------------------------------
* lcd_driver.h
*
* Include file for lcd_driver module.
*
* (C)2008 Sam's Logic.
*
* The copyright notice above does not evidence any
* actual or intended publication of such source code.
*
*------------------------------------------------------------------------------
*/

/*
*------------------------------------------------------------------------------
* File				: lcd_driver.h
* Created by		: Sam
* Last changed by	: Sam
* Last changed		: 10/0/2009
*------------------------------------------------------------------------------
*
* Revision 0.2 10/0/2009 Sam
* Added void ClearDisplayLine(UINT8 lineNumber) for clear the required line
* Revision 0.1 19/07/2009 Sam
* Added Macros for supporting both 2X16 and 4X20 char displays
* Revision 0.0 03/10/2008 Sam
* Initial revision
*
*------------------------------------------------------------------------------
*/


/*
*------------------------------------------------------------------------------
* Include Files
*------------------------------------------------------------------------------
*/
#include <delays.h>
#include "typedefs.h"
/*
*------------------------------------------------------------------------------
* Public Defines
*------------------------------------------------------------------------------
*/



/*
*------------------------------------------------------------------------------
* Public Macros
*------------------------------------------------------------------------------
*/
// Enable if the lcd used has 2 line , 16 char
#define LCD_TYPE_2X16

// Enable if the lcd used has 4 line , 20 char
//#define LCD_TYPE_4X20

// Wait Data setup time (min 40ns)
#define DELAY_40NS()		Nop();\
						//	Nop();

// Wait E Pulse width time (min 230ns)
#define DELAY_230NS()		Nop();\
							Nop();\
						//	Nop();\
						//	Nop();\
						//	Nop();\
						//	Nop();\
						//	Nop();\
						//	Nop();\
						//	Nop();


enum
{
 	DISPLAY_MAX_COLUMNS_PER_ROW = 16,
	DISPLAY_MAX_ROWS = 2,
	DISPLAY_MAX_CHARS = (DISPLAY_MAX_COLUMNS_PER_ROW * DISPLAY_MAX_ROWS),
 	DISPLAY_DDRAM_BASE = (0x80)
};

// LCD commands
#define 	DISPLAY_CLEAR				(0x01)		// Clear display screen
#define 	DISPLAY_RETURN_HOME			(0x02)		// Return Home
#define 	DISPLAY_DEC_CURSOR			(0x04)		// Decrement cursor (shift cursor to left)
#define 	DISPLAY_INC_CURSOR			(0x05)		// Increment cursor (shift cursor to ritht)
#define	 	DISPLAY_SHIFT_RIGHT_DPY		(0x06)		// Shift display right
#define 	DISPLAY_SHIFT_LEFT_DPY		(0x07)		// Shift display left
#define 	DISPLAY_OFF_CUR_OFF			(0x08)		// Display off, cursor off
#define 	DISPLAY_OFF_CUR_ON			(0x0A)		// Display off, cursor on
#define		DISPLAY_ON_CUR_OFF			(0x0C)		// Display on, cursor off
#define 	DISPLAY_ON_CUR_BLINK		(0x0E)		// Display on, cursor blinking
#define 	DISPLAY_ON_CUR_BLINK_BIG	(0x0F)		// Display on, cursor blinking
#define 	DISPLAY_SHIFT_LEFT_CUR		(0x10)		// Shift cursor position to left
#define	 	DISPLAY_SHIFT_RIGHT_CUR		(0x14)		// Shift cursor position to right
#define 	DISPLAY_SHIFT_LEFT_DPY_ENT	(0x18)		// Shift the entire display to the left
#define 	DISPLAY_SHIFT_RIGHT_DPY_ENT	(0x1C)		// Shift the entire display to the right
#define 	DISPLAY_DPY_1ST_LINE		(0x80)		// Force cursor to the beginning of 1st line
#define 	DISPLAY_DPY_2ND_LINE		(0xC0)		// Force cursor to the beginning of 2nd line
#define 	DISPLAY_DPY_3RD_LINE		(0x94)		// Force cursor to the beginning of 3rd line
#define 	DISPLAY_DPY_4TH_LINE		(0xD4)		// Force cursor to the beginning of 4th line
#define 	DISPLAY_2LIN_5X7			(0x38)		// 2 lines and 5 x 7 matrix

/*
*------------------------------------------------------------------------------
* Public Data Types
*------------------------------------------------------------------------------
*/

/*
*------------------------------------------------------------------------------
* Public Variables (extern)
*------------------------------------------------------------------------------
*/
//extern UINT8 LCDText;
/*
*------------------------------------------------------------------------------
* Public Constants (extern)
*------------------------------------------------------------------------------
*/

/*
*------------------------------------------------------------------------------
* Public Function Prototypes (extern)
*------------------------------------------------------------------------------
*/
extern void LCD_init(void);
extern void LCD_writeCommand(UINT8 commandByte);
extern void LCD_writeData(UINT8 );

extern void LCD_clear(void);
extern void LCD_putChar(UINT8 data);
extern void LCD_setBackSpace(UINT8 data);

#endif
/*
*  End of lcd_driver.h
*/



