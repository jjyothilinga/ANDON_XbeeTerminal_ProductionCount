#ifndef KEYPAD_DRIVER_H
#define KEYPAD_DRIVER_H


/*
*------------------------------------------------------------------------------
* keypad_driver.h
*
* Include file for lcd_driver module.
*
* (C)2012 samslogix.
*
* The copyright notice above does not evidence any
* actual or intended publication of such source code.
*
*------------------------------------------------------------------------------
*/

/*
*------------------------------------------------------------------------------
* File				: keypad_driver.h
* Created by		: Sam
* Last changed by	: Sam
* Last changed		: 10/03/2012
*------------------------------------------------------------------------------
*
* Revision 1.0 10/03/2012 Sam
* Initial revision
*
*------------------------------------------------------------------------------
*/


/*
*------------------------------------------------------------------------------
* Include Files
*------------------------------------------------------------------------------
*/

#include "board.h"

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

// Any valid UINT8 will do - must not match anything on currentKeypad

#define KEYPAD_NO_NEW_DATA 		(UINT8)0xFF//'-'


enum
{
	KEYPAD_KEY00 = 0		 	,
	KEYPAD_KEY01,
	KEYPAD_KEY02,
	KEYPAD_KEY03,	
	KEYPAD_KEY04,	
	KEYPAD_KEY05,	
	KEYPAD_KEY06,	
	KEYPAD_KEY07,	
	KEYPAD_KEY08,	
	KEYPAD_KEY09,	
	KEYPAD_KEY10,	
	KEYPAD_KEY11,	 
	KEYPAD_KEY12,	 
	KEYPAD_KEY13,	 
	KEYPAD_KEY14,	 
	KEYPAD_KEY15	
};







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
extern void KEYPAD_init(void);
extern rom void KEYPAD_task(void);
extern BOOL KEYPAD_read(UINT8* const pkeyNormal,UINT8* pduration);
extern void KEYPAD_reset(void);



#endif
/*
*  End of keypad_driver.h
*/



