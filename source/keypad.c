/*
*------------------------------------------------------------------------------
* keypad_driver.c
*
* keypad driver module.
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
* File				: keypad_driver.c
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

#include "keypad.h"

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

/*
*------------------------------------------------------------------------------
* Public Variables
*------------------------------------------------------------------------------
*/

/*
*------------------------------------------------------------------------------
* Private Variables (static)
*------------------------------------------------------------------------------
*/

typedef struct _KEYPAD
{
	UINT8 buffer[KEYPAD_BUFFER_LENGTH];
	UINT8 bufferIndex;
	UINT8 dataIndex;
	UINT8 debounceCount;	
	UINT8 currentKey;
	UINT8 keyCount;
}KEYPAD;

#pragma idata KEYPAD_DATA
KEYPAD keypad = {0};

static UINT8 keypadReceiveBuffer[KEYPAD_BUFFER_LENGTH + 1][2] = {0};
static UINT8 kepadInReadIndex = 0;     	// Data in buffer that has been read
static UINT8 keypadInWaitingIndex= 0;  // Data in buffer not yet read
static UINT8 lastValidKey = KEYPAD_NO_NEW_DATA;
volatile static UINT16 duration = 0; 
static UINT8 oldKey =KEYPAD_NO_NEW_DATA ;
#pragma idata 

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

/*
*------------------------------------------------------------------------------
* Private Function Prototypes (static)
*------------------------------------------------------------------------------
*/

static BOOL scanKeypad(UINT8* const pkeyNormal);

/*
*------------------------------------------------------------------------------
* Public Functions
*------------------------------------------------------------------------------
*/
/*
*------------------------------------------------------------------------------
* void InitializeKeypad(void)
*
* Summary	: Initialize keypad buffer indexes
*
* Input		: None
*
* Output	: None
*------------------------------------------------------------------------------
*/
void KEYPAD_init(void)
{
	kepadInReadIndex = 0;
   	keypadInWaitingIndex = 0;

	keypad.bufferIndex = 0;
	keypad.dataIndex = 0;
	keypad.debounceCount = 0;
	keypad.currentKey = 0xFF;
	keypad.keyCount = 0;

	
}

/*
*------------------------------------------------------------------------------
* rom void UpdateKeypadTask(void)
*
* Summary	: The function update the key pressed into keypad buffer
*			  This must be schedule at approx every 50 - 200 ms .
*
* Input		: None
*
* Output	: None
*------------------------------------------------------------------------------
*/
rom void KEYPAD_task(void)
{
	UINT8 currentKey, functionKey;

	duration++;
   	// Scan Keypad here...
   	if (scanKeypad(&currentKey) == 0)
    {
    	// No new Key data - just return
      	return;
    }

   	// Want to read into index 0, if old data has been read
   	// (simple ~circular buffer)
   	if (keypadInWaitingIndex == kepadInReadIndex)
    {
    	keypadInWaitingIndex = 0;
      	kepadInReadIndex = 0;
    }

   	// Load Keypad data into buffer
   	keypadReceiveBuffer[keypadInWaitingIndex][0] = currentKey;
	keypadReceiveBuffer[keypadInWaitingIndex][1] = duration;

   	if (keypadInWaitingIndex < KEYPAD_BUFFER_LENGTH)
    {
    	// Increment without overflowing buffer
      	keypadInWaitingIndex++;
    }
	duration = 0;
}

//#define INPUT_SIMULATION

#ifdef INPUT_SIMULATION

#define NO_INPUTS  36

static rom UINT16 Key[][2] = {{0x0,40},{0xf,50},
								{0x0,50},{0xf,50},
								{0x1,50},{0xf,50},
								{0x2,50},{0xf,50},
								{0x2,50},{0x5,50},{0x4,50},{0xf,50},
								{0x3,40},{0xf,30},
								{0x3,40},{0xf,30},
								{0x7,40},{0xf,30},{0xb,50},{0xc,30},
								
								{0xe,40},{0xe,30},{0xe,40},{0xc,30},

								{0x1,50},{0xc,50},{0x3,50},{0xc,50},
								{0x1,40},{0xc,30},{0x1,50},{0xc,50},{0x4,50},{0xc,50},
								{0x1,40},{0xc,30},{0x1,50},{0xc,50},{0x5,50},{0xc,50},
								{0x1,40},{0xc,30},{0x1,50},{0xc,50},{0x6,50},{0xc,50},
								{0x1,50},{0xc,50},{0x2,50},
								{0xc,40},{0x2,30},{0xb,50},{0x8,50},{0x1,50},{0xc,50},{0xc,50},
								{0xb,40},{0xc,30},{0x5,50},{0x6,50},{0x3,50},{0xc,50},
								
								{0x01,50},{0x0c,50},{0x09,50},{0x0c,50},
							
								{0x01,50},{0x02,50},{0x0c,50},{0x01,50},{0x0c,50},{0x03,50},{0x01,50},{0xc,50},
								{0xb,50},{0x02,50},{0xc,50},{0x02,50},{0xc,50},{0x01,50},{0xc,50},
								{0x03,50},{0x01,50},{0x02,50},{0x03,50},{0x01,50},{0x02,50},{0x03,50},
							  	{0x01,50},{0x02,50},{0x03,50},{0x01,50},{0x02,50},{0x03,50},{0x01,50},{0x02,50},{0x18,45},
								};

UINT8 keyIndex = 0;
UINT8 delayCount = 5;


/*
*------------------------------------------------------------------------------
* BOOL GetDataFromKeypadBuffer(UINT8* const pkeyNormal)
*
* Summary	: This extracts data from the keypad buffer.
*
* Input		: UINT8* const pkeyNormal  - pointer to get normal key entry
*
* Output	: BOOL - 1 - if data exisist in the buffer
*			         0 - if no data in the buffer
*------------------------------------------------------------------------------
*/
BOOL KEYPAD_read(UINT8* const pkey, UINT8* pduration)
{

	if( keyIndex >= NO_INPUTS)
	{
		return 0;
		keyIndex = 0;
	}

	if( Key[keyIndex][0] != 0xFF && Key[keyIndex][0] != 0x18)
	{
		*pkey = Key[keyIndex][0];
		*pduration = Key[keyIndex][1];
		keyIndex++;
		return 1;
	}
	else if( Key[keyIndex][0] == 0x18)
	{
		DelayMs(100);
		*pkey = Key[keyIndex][0];
		*pduration = Key[keyIndex][1];
		keyIndex++;
		return 1;
	}	
	//DelayMs(50);
	keyIndex++;
	return 0;
}
#else

/*
*------------------------------------------------------------------------------
* BOOL GetDataFromKeypadBuffer(UINT8* const pkeyNormal)
*
* Summary	: This extracts data from the keypad buffer.
*
* Input		: UINT8* const pkeyNormal  - pointer to get normal key entry
*
* Output	: BOOL - 1 - if data exisist in the buffer
*			         0 - if no data in the buffer
*------------------------------------------------------------------------------
*/
BOOL KEYPAD_read(UINT8* const pkeyNormal,UINT8* pduration)
{
	// If there is new data in the buffer
   	if (kepadInReadIndex < keypadInWaitingIndex)
    {
  		*pkeyNormal = keypadReceiveBuffer[kepadInReadIndex][0];
		*pduration = keypadReceiveBuffer[kepadInReadIndex][1];
      	kepadInReadIndex++;

    	return 1;
    }
   	return 0;
}
#endif
/*
*------------------------------------------------------------------------------
* void ClearKeytpadBuffer(void)
*
* Summary	: Clears the keypad buffer by resetting read and write index
*
* Input		: None
*
* Output	: None
*
*------------------------------------------------------------------------------
*/
void KEYPAD_reset(void)
{
	keypadInWaitingIndex = 0;
   	kepadInReadIndex = 0;

	keypad.bufferIndex = 0;
	keypad.dataIndex = 0;
	keypad.debounceCount = 0;
	keypad.currentKey = 0xFF;
}


/*
*------------------------------------------------------------------------------
* Private Functions
*------------------------------------------------------------------------------
*/
/*
*------------------------------------------------------------------------------
* BOOL scanKeypad(UINT8* const pkeyNormal)
*
* Summary	: This function is called from scheduled UpdateKeypadTask function.
*
* Input		: UINT8* const pkeyNormal -  pointer to get normal key data
*
* Output	: BOOL - 0 - if no new key data
*       	         1 - if there is new key data
*
* Note		: Must be edited as required to match Key labels.
*------------------------------------------------------------------------------
*/
BOOL scanKeypad(UINT8* const pkeyNormal)
{
	

   	UINT8 currentKey = KEYPAD_NO_NEW_DATA;

	if(1 == KEYPAD_DEC_INT)
	{
		currentKey = (((KEYPAD_BCD3 << 3) |(KEYPAD_BCD2 << 2) |(KEYPAD_BCD1 << 1) | KEYPAD_BCD0) & 0x0F);


#ifdef KEYPAD_TEST

		switch( currentKey )
		{
			case KEYPAD_KEY00:
				currentKey = '5';
			break;
			case KEYPAD_KEY01:
				currentKey = '6';
			break;
			case KEYPAD_KEY02:
				currentKey = '9';
			break;
			case KEYPAD_KEY03:
				currentKey = 'A';
			break;	

			case KEYPAD_KEY04:
				currentKey = '2';
			break;
			case KEYPAD_KEY05:
				currentKey = '1';
			break;
			case KEYPAD_KEY06:
				currentKey = '3';
			break;
			case KEYPAD_KEY07:
				currentKey = '7';
			break;



			case KEYPAD_KEY08:
				currentKey = '4';
			break;
			case KEYPAD_KEY09:
				currentKey = '0';
			break;
			case KEYPAD_KEY10:
				currentKey = 'B';
			break;
			case KEYPAD_KEY11:
				currentKey = 'F';
			break;


			case KEYPAD_KEY12:
				currentKey = 'C';
			break;
			case KEYPAD_KEY13:
				currentKey = '8';
			break;
			case KEYPAD_KEY14:
				currentKey = 'E';
			break;
			case KEYPAD_KEY15:
				currentKey = 'D';
			break;


			default: currentKey = KEYPAD_NO_NEW_DATA;
			break;
		}
#else

		switch( currentKey )
		{
			case KEYPAD_KEY00:
				currentKey = 5;
			break;
			case KEYPAD_KEY01:
				currentKey = 6;
			break;
			case KEYPAD_KEY02:
				currentKey = 9;
			break;
			case KEYPAD_KEY03:
				currentKey = 10;
			break;	

			case KEYPAD_KEY04:
				currentKey = 2;
			break;
			case KEYPAD_KEY05:
				currentKey = 1;
			break;
			case KEYPAD_KEY06:
				currentKey = 3;
			break;
			case KEYPAD_KEY07:
				currentKey = 7;
			break;



			case KEYPAD_KEY08:
				currentKey = 4;
			break;
			case KEYPAD_KEY09:
				currentKey = 0;
			break;
			case KEYPAD_KEY10:
				currentKey = 11;
			break;
			case KEYPAD_KEY11:
				currentKey = 15;
			break;


			case KEYPAD_KEY12:
				currentKey = 12;
			break;
			case KEYPAD_KEY13:
				currentKey = 8;
			break;
			case KEYPAD_KEY14:
				currentKey = 14;
			break;
			case KEYPAD_KEY15:
				currentKey = 13;
			break;


			default: currentKey = KEYPAD_NO_NEW_DATA;
			break;
		}

#endif
	}

   	if (currentKey == KEYPAD_NO_NEW_DATA)
    {
    	// No key pressed (or just a function key)
      	oldKey = KEYPAD_NO_NEW_DATA;
      	lastValidKey = KEYPAD_NO_NEW_DATA;
      	return 0;  // No new data
    }

	// A Key has been pressed: debounce by checking twice
   	if (currentKey == oldKey)
    {
    	// A valid (debounced) key press has been detected

      	// Must be a new key to be valid - no 'auto repeat'
      	if (currentKey != lastValidKey)
        {
        	// New key!
         	*pkeyNormal = currentKey;
         	lastValidKey = currentKey;
         	return 1;
     	}
   	}

   	// No new data
   	oldKey = currentKey;
   	return 0;
}
/*
*  End of keypad_driver.c
*/