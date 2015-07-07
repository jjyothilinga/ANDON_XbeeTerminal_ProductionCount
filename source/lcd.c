/*
*------------------------------------------------------------------------------
* lcd_driver.c
*
* lcd driver module.
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
* File				: lcd_driver.c
* Created by		: Sam
* Last changed by	: Sam
* Last changed		: 10/0/2009
*------------------------------------------------------------------------------
*
* Revision 0.2 10/0/2009 Sam
* Added void ClearDisplayLine(UINT8 lineNumber) for clear the required line
* Revision 0.1 19/07/2009 Sam
* Added local static rom const UINT8 clearScreen[] array.
* Udated driver to support both 2X16 and 4X20 based displays
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
#include <string.h>
#include "board.h"
#include "lcd.h"

/*
*------------------------------------------------------------------------------
* Private Defines
*------------------------------------------------------------------------------
*/

/*
*------------------------------------------------------------------------------
* Private Macros
*------------------------------------------------------------------------------
*/


/*
*------------------------------------------------------------------------------
* Private Data Types
*------------------------------------------------------------------------------
*/
typedef struct _LCD
{
	UINT8 NOofChar, bckspc;
	UINT8 buff[DISPLAY_MAX_ROWS][DISPLAY_MAX_COLUMNS_PER_ROW];
}LCD;
/*
*------------------------------------------------------------------------------
* Public Variables
*------------------------------------------------------------------------------
*/
// LCDText is a 32 byte shadow of the LCD text.  Write to it and
// then LCDUpdateTask() copies the string into the LCD module.

#pragma idata LCD_DATA
LCD lcd = {0};	
#pragma idata

/*
*------------------------------------------------------------------------------
* Private Variables (static)
*------------------------------------------------------------------------------
*/




/*
*------------------------------------------------------------------------------
* Public Constants
*------------------------------------------------------------------------------
*/

/*
*------------------------------------------------------------------------------
* Private Constants (static)
*------------------------------------------------------------------------------
*/

#ifdef LCD_TYPE_2X16    					 //0123456789012345
static rom const UINT8 clearScreen[] 		= "                ";
#endif
#ifdef LCD_TYPE_4X20    					 //01234567890123456789
static rom const UINT8 clearScreen[] 		= "                    ";
#endif


/*
*------------------------------------------------------------------------------
* Private Function Prototypes (static)
*------------------------------------------------------------------------------
*/
static INT8 busyLcd(void);
static void rawWriteCommand8ToLcd(UINT8 commandByte);

void dataPortIn(void);
void dataPortOut(void);
/*
*------------------------------------------------------------------------------
* Private Functions
*------------------------------------------------------------------------------
*/

/*
*------------------------------------------------------------------------------
* INT8 busyLcd(void)
*
* Summary	: Reads the Busy Flag (BF) of the HD44780 LCD Display Controller and
*			  returns either the BF Flag is cleared or an timeout occurred.
*			  The timeout typically occurs when there is no display is connected.
*
* Input		: None
*
* Output	: INT8 - Busy satus.0 - Pass,  -1 - Fail
*------------------------------------------------------------------------------
*/
INT8 busyLcd(void)
{
    BOOL busy;
	UINT16 timeout = 30000;

    busy =1;
    while ((busy == 1))
    {
       	// Configure data Port as input (LCD_D7..busy flag)
        dataPortIn();
		DELAY_230NS();
        LCD_RS = 0; LCD_RW = 1;
		DELAY_230NS();
        LCD_E = 1;
		DELAY_230NS();
		busy = LCD_D7;
		LCD_E = 0;		// read 2. half of the data byte
		LCD_E = 1;
		DELAY_230NS();
		LCD_E = 0;              		// read 2. half of the data byte
        timeout--;
        if(timeout == 0)
		{
			dataPortOut();
           return -1;
		}
    }
	dataPortOut();
    return 0;
}

/*
*------------------------------------------------------------------------------
* void rawWriteCommand8ToLcd(UINT8 commandByte)
*
* Summary	: Used in display init function to send the function set command
*			  with out BF check.
*
* Input		: UINT8 commandByte - Basic command for LCD
*
* Output	: None
*------------------------------------------------------------------------------
*/
void rawWriteCommand8ToLcd(UINT8 commandByte)
{
	LCD_RS=0; LCD_RW=0;
	DELAY_40NS();
	LCD_DAT4_PORT = ((LCD_DAT4_PORT & 0x0F) | (commandByte & 0xF0));
	DELAY_40NS();
	LCD_E=1;
	DELAY_230NS();
	LCD_E=0;
}
/*
*------------------------------------------------------------------------------
* void LCD_writeCommand(UINT8 commandByte)
*
* Summary	: Used to send the commands with BF check.
*
* Input		: UINT8 commandByte - Basic command for LCD
*
* Output	: None
*------------------------------------------------------------------------------
*/
void LCD_writeCommand(UINT8 commandByte)
{

	busyLcd ();
	LCD_RS=0; LCD_RW=0;
	DELAY_40NS();
	LCD_DAT4_PORT = ((LCD_DAT4_PORT & 0x0F) | (commandByte & 0xF0));
	DELAY_40NS();
	LCD_E=1;
	DELAY_230NS();
	LCD_E=0;   // High-Byte
	LCD_DAT4_PORT = ((LCD_DAT4_PORT & 0x0F) | (commandByte<<4 & 0xF0));
	DELAY_40NS();
	LCD_E=1;
	DELAY_230NS()
	LCD_E=0;   // Low-Byte
}

/*
*------------------------------------------------------------------------------
* void rawWriteDataToLcd(UINT8 dataByte)
*
* Summary	: Used to send data to the LCD with BF check
*
* Input		: UINT8 commandByte - Basic command for LCD
*
* Output	: None
*------------------------------------------------------------------------------
*/
void LCD_writeData(UINT8 dataByte)
{

	busyLcd ();
	LCD_RS=1; LCD_RW=0;
	DELAY_40NS();
	LCD_DAT4_PORT = ((LCD_DAT4_PORT & 0x0F) | (dataByte & 0xF0));
	DELAY_40NS();
	LCD_E=1;
	DELAY_230NS();
	LCD_E=0;   // High-Byte
	LCD_DAT4_PORT = ((LCD_DAT4_PORT & 0x0F) | (dataByte<<4 & 0xF0));
	DELAY_40NS();
	LCD_E=1;
	DELAY_230NS();
	LCD_E=0;   // Low-Byte
}

/*
*------------------------------------------------------------------------------
* Public Functions
*------------------------------------------------------------------------------
*/


/*
*------------------------------------------------------------------------------
* void InitLcd(void)
*
* Summary	: Initialize the LCD-Controller HD44780,
*			  4-bit data bus; 2 Rows; 5x7 Dot.
*
* Input		: None
*
* Output	: None
* Note		: 1/20 MHz  = 0.00000005 sec = 0.05us x = 50uc
*------------------------------------------------------------------------------
*/
void InitLcd(void)
{
	INT8 retValue;

	dataPortOut();
	// Wait the required time for the LCD to reset
	DelayMs(40);
	// FUNC. SET: interface is 8 bit long
	rawWriteCommand8ToLcd(0x30);
	DelayMs(10);      // Wait 10ms
	// FUNC. SET: interface is 8 bit long
	rawWriteCommand8ToLcd(0x30);
	DelayMs(10);      // Wait 10ms
	// FUNC. SET: interface is 8 bit long
	rawWriteCommand8ToLcd(0x30);
	DelayMs(10);      // Wait 10ms
	// FUNC.SET:interface is 4 bit long
	rawWriteCommand8ToLcd(0x20);
	DelayMs(10);      // Wait 10ms
	// FUNC.SET:interface is 4 bit long  0x 28
	LCD_writeCommand(0x28);
	retValue = busyLcd ();
	// Check if Display is present
	if(retValue == 0)
	{
		//Display ON, Cursor on 0x0C
	 	LCD_writeCommand(0x0F);
		// Display Clear 0x01
		LCD_writeCommand(0x01);
		// Entry Mode Set 0x04
		LCD_writeCommand(0x04);
		// Entry Mode Set 0x06
		LCD_writeCommand(0x06);
	}

		return;

}

void LCD_setBackSpace(UINT8 data)
{
	lcd.bckspc = data;
}


void LCD_clear(void)
{
	LCD_writeCommand(DISPLAY_CLEAR);
	lcd.NOofChar = 0;
}


void LCD_putChar(UINT8 data)
{
	UINT8 i;
	if(data == lcd.bckspc)
	{
		if(lcd.NOofChar == 0)
		{
			return;
		}
		if(lcd.NOofChar == 16)
		{
			LCD_writeCommand(DISPLAY_DPY_1ST_LINE);
			for(i=0;i<16;i++)
				LCD_writeCommand(DISPLAY_SHIFT_RIGHT_CUR);
		}
		LCD_writeCommand(DISPLAY_SHIFT_LEFT_CUR);
		LCD_writeData(' ');
		LCD_writeCommand(DISPLAY_SHIFT_LEFT_CUR);
		lcd.NOofChar--;
		return;
	}
	if(lcd.NOofChar == DISPLAY_MAX_COLUMNS_PER_ROW)
		LCD_writeCommand(DISPLAY_DPY_2ND_LINE);
	else if(lcd.NOofChar >= DISPLAY_MAX_CHARS)
	{
		lcd.NOofChar = 0;
		LCD_writeCommand(DISPLAY_RETURN_HOME);
	}

	LCD_writeData(data);
	lcd.NOofChar++;
}

/*
*------------------------------------------------------------------------------
* void InitializeLcd(void)
*
* Summary	: Initialize LCD and setup LCD task to run.
*
* Input		: None
*
* Output	: None
*
*------------------------------------------------------------------------------
*/
void LCD_init(void)
{
	// Initialize LCD hardware
	InitLcd();
	// Clear the display at startup
	LCD_writeCommand(DISPLAY_CLEAR);
	lcd.NOofChar = 0;
}

/*
*------------------------------------------------------------------------------
* void UpdateLcdTask(void)
*
* Summary	: Copies the contents of the local LCDText[] array into the
*			  LCD's internal display buffer.  Null terminators in
*			  LCDText[] terminate the current line, so strings may be
*			  printed directly to LCDText[].
*
* Input		: None
*
* Output	: None
*
*------------------------------------------------------------------------------
*/
void writetoLCD(UINT8 *buff)
{
	UINT8 i;

	LCD_writeCommand(DISPLAY_CLEAR);

	// Output first line
	for(i = 0; i < DISPLAY_MAX_COLUMNS_PER_ROW; i++)
	{
		lcd.buff[0][i]= *buff;
		*buff++;
	}
	LCD_writeCommand(DISPLAY_DPY_2ND_LINE);
	for(i = 0; i < DISPLAY_MAX_COLUMNS_PER_ROW; i++)
	{
		lcd.buff[0][i]= *buff;
		*buff++;
	}
}


void dataPortIn(void)
{
	LCD_D7_DIR = 1;
	LCD_D6_DIR = 1;
	LCD_D5_DIR = 1;
	LCD_D4_DIR = 1;
	LCD_E_DIR = 0;
	LCD_RW_DIR = 0;
	LCD_RS_DIR = 0;
}

void dataPortOut(void)	
{
	LCD_D7_DIR = 0;
	LCD_D6_DIR = 0;
	LCD_D5_DIR = 0;
	LCD_D4_DIR = 0;
	LCD_E_DIR = 0;
	LCD_RW_DIR = 0;
	LCD_RS_DIR = 0;
}



/*
*  End of lcd_driver.c
*/