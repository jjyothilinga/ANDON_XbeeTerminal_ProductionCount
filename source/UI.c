#include "board.h"
#include "config.h"
#include "Keypad.h"
#include "lcd.h"
#include "string.h"
#include "ui.h"
#include "app.h"



typedef struct _UI
{
	UI_STATE state;
	UI_STATE prevState;
	UINT8 buffer[MAX_INPUT_CHARS+1];
	UINT8 bufferIndex;
	UINT8 prevcode;
	UINT8 keyIndex;
	UINT8 input[MAX_INPUT_CHARS+1];
	UINT8 inputIndex;
}UI;





const rom UINT8 *UI_MSG[]=
		{"STATION:",
		"ISSUE:",
		"SAFETY",
		"BREAKDOWN",
		"QUALITY",
		"METHODS",
		"PRODUCTION",
		"MATERIAL",
		"PART NO:",
		"PASSWORD:",
		"ADMIN ACTIVITY:",
		"CLEAR ISSUES",

		};




const rom UINT8 keyMap[MAX_KEYS ][MAX_CHAR_PER_KEY] = {
													{'1','-','1','-','1'},{'2','A','B','C','2'},{'3','D','E','F','3'},{'\x0B','\x0B','\x0B','\x0B','\x0B'},
													{'4','G','H','I','4'},{'5','J','K','L','5'},{'6','M','N','O','6'},{'\x0C','\x0C','\x0C','\x0C','\x0C'},
													{'7','P','Q','R','S'},{'8','T','U','V','8'},{'9','W','X','Y','Z'},{'\x08','\x08','\x08','\x08','\x08'},
													{'*','*','*','*','*'},{'0',' ','0',' ','0'},{'\x13','\x13','\x13','\x13','\x13'},{'\x0A','\x0A','\x0A','\x0A','\x0A'}
													
													};




#pragma idata UI_DATA
UI ui = {0,0,{0},0,0xFF,0,0};
OpenIssue openIssue={{0},-1};
OpenIssue ackIssue={{0},-1};
//#pragma idata



UINT8 mapKey(UINT8 scancode, UINT8 duration);
UINT8 getStation(void);
void getData(void);
void clearUIBuffer(void);
void putUImsg(UINT8 msgIndex);
void setUImsg( UINT8 msgIndex );
void clearUIInput(void);
void showUImsg( UINT8* msg );



void UI_init(void)
{

	LCD_setBackSpace('\x08');	//Indicates LCD driver "\x08" is the symbol for backspace

	setUImsg(UI_MSG_STATION);

	clearUIBuffer();
	clearUIInput();
}



void UI_task(void)
{

	UINT8 keypressed = 0xFF;
	UINT8 i;
	UINT8 duration, scancode;
	UINT8 uimsg;

	if(KEYPAD_read(&scancode, &duration) == FALSE)			//Check whether key has been pressed
	{
		return;
	}

	
	keypressed = mapKey(scancode,duration);				//Map the key

	if( keypressed == 0xFF)
	{
		return;
	}


	switch(ui.state)
	{
		case UI_STATION:

		if( keypressed == '\x08')
		{
			if(ui.bufferIndex > 0 )
			{
				LCD_putChar(keypressed);
				ui.bufferIndex--;
				if( ui.inputIndex > 0 )
					ui.inputIndex--;
			}

		}

		else if( keypressed == '\x0A')
		{
			if(ui.bufferIndex > 0)
			{
				getStation() ;
				setUImsg(UI_MSG_ISSUE);
				clearUIBuffer();
				ui.state = UI_ISSUE;
				

			}
		}

		else if( keypressed == '\x13')
		{


			setUImsg(UI_MSG_PASSWORD);

			clearUIBuffer();

			ui.state = UI_PASSWORD;
				
			
		}

		else if( keypressed == '\x0C')
		{
			APP_getAcknowledgedIssue(&ackIssue);
			if(ackIssue.ID != - 1)
			{
				showUImsg(ackIssue.tag);
				clearUIBuffer();
				clearUIInput();
				ui.state= UI_ISSUE_RESOLVE;
			}
		}	

		else if( keypressed == '\x0B')
		{
			APP_getOpenIssue(&openIssue);
			if(openIssue.ID != - 1)
			{
				showUImsg(openIssue.tag);
				clearUIBuffer();
				clearUIInput();
				ui.state= UI_ISSUE_ACK;
			}
		}		
		else
		{
			ui.buffer[ui.bufferIndex] = keypressed;
			LCD_putChar(ui.buffer[ui.bufferIndex]);
			ui.bufferIndex++;
		}

		break;


	


		case UI_ISSUE:

		if( keypressed == '\x08')
		{
			if(ui.bufferIndex > 0 )
			{
				LCD_putChar(keypressed);
				ui.bufferIndex--;

			}
		
			else
			{
				
				setUImsg(UI_MSG_STATION);
				clearUIBuffer();
				clearUIInput();
				ui.state = UI_STATION;
			}

		}

		else
		{
			switch( keypressed )
			{
	
				case '1':
				putUImsg(UI_MSG_SAFETY);
	
				ui.input[ui.inputIndex]  = '1';
				ui.inputIndex++;
	
				ui.state = UI_BRK_QUA_MS;
				break;


				case '2':
				putUImsg(UI_MSG_BREAKDOWN);
	
				ui.input[ui.inputIndex]  = '2';
				ui.inputIndex++;
	
				ui.state = UI_BRK_QUA_MS;
				break;


				case '3':
				putUImsg(UI_MSG_QUALITY);
	
				ui.input[ui.inputIndex]  = '3';
				ui.inputIndex++;
	
				ui.state = UI_BRK_QUA_MS;
				break;

				case '4':
				putUImsg(UI_MSG_METHODS);
	
				ui.input[ui.inputIndex]  = '4';
				ui.inputIndex++;
	
				ui.state = UI_BRK_QUA_MS;
				break;


				case '5':
				putUImsg(UI_MSG_PRODUCTION);
	
				ui.input[ui.inputIndex]  = '5';
				ui.inputIndex++;
	
				ui.state = UI_BRK_QUA_MS;
				break;


				case '6':
				putUImsg(UI_MSG_MATERIAL);
	
				ui.input[ui.inputIndex]  = '6';
				ui.inputIndex++;
	
				ui.state = UI_BRK_QUA_MS;
				break;
	
				default:
				break;
	
			}
		}
		
		break;

		case UI_PART_NO:
		if( keypressed == '\x08')
		{
			if(ui.bufferIndex > 0 )
			{
				LCD_putChar(keypressed);
				ui.bufferIndex--;

			}

			else
			{

				setUImsg(UI_MSG_ISSUE);
				clearUIBuffer();
				ui.inputIndex = 2;
				ui.state = UI_ISSUE;
			}

			ui.prevcode = 0xFF;
			ui.keyIndex = 0;
		}
		else if( keypressed == '\x0A')
		{
			if( ui.bufferIndex > 0 )
			{
	
				getData();
				APP_raiseIssues( ui.input);
				setUImsg(UI_MSG_STATION);
				clearUIBuffer();
				clearUIInput();
				ui.state = UI_STATION;
			}
			ui.prevcode = 0xFF;
			ui.keyIndex = 0;
		}

		else
		{
			if(scancode == ui.prevcode )
			{
				if(duration < MIN_KEYPRESS_DURATION)
				{
					ui.bufferIndex--;
					ui.buffer[ui.bufferIndex] = keypressed;
					LCD_putChar('\x08');
					LCD_putChar(ui.buffer[ui.bufferIndex]);
					//DelayMs(20);
					ui.bufferIndex++;
				}
				else 
				{
					ui.buffer[ui.bufferIndex] = keypressed;
					LCD_putChar(ui.buffer[ui.bufferIndex]);
					//DelayMs(20);
					ui.bufferIndex++;
					ui.prevcode = 0xFF;
					ui.keyIndex = 0;
				}
			}
			else
			{
				ui.buffer[ui.bufferIndex] = keypressed;
				LCD_putChar(ui.buffer[ui.bufferIndex]);
				//DelayMs(20);
				ui.bufferIndex++;
				ui.prevcode = scancode;
			}
		ui.prevcode = scancode;	
		}
		break;



		case UI_BRK_QUA_MS:

		if( keypressed == '\x08')
		{
			setUImsg(UI_MSG_ISSUE);
			clearUIBuffer();
			ui.inputIndex = 2;
			ui.state = UI_ISSUE;

		}

		else if( keypressed == '\x0A')
		{
			if( ui.input[ui.inputIndex-1] == '6' )
			{
			
				setUImsg(UI_MSG_PART_NO);
				clearUIBuffer();
				//clearUIInput();
				ui.state = UI_PART_NO;
			}
			else
			{
				getData();
				APP_raiseIssues( ui.input);
				setUImsg(UI_MSG_STATION);
				clearUIBuffer();
				clearUIInput();
				ui.state = UI_STATION;
			}
		}

		break;



		case UI_ISSUE_ACK:

		if( keypressed == '\x08')
		{
			setUImsg(UI_MSG_STATION);
			clearUIBuffer();
			clearUIInput();
			ui.state = UI_STATION;
		}

		else if( keypressed == '\x0B')
		{
			APP_getOpenIssue(&openIssue);
			if( openIssue.ID != -1 )
			{
				showUImsg(openIssue.tag);
			}

		}
		else if( keypressed == '\x0A')
		{
			
			APP_acknowledgeIssues(openIssue.ID);
			openIssue.ID = -1;
			setUImsg(UI_MSG_STATION);
			clearUIBuffer();
			clearUIInput();
			ui.state = UI_STATION;

		}

		break;



	case UI_ISSUE_RESOLVE:

		if( keypressed == '\x08')
		{
			setUImsg(UI_MSG_STATION);
			clearUIBuffer();
			clearUIInput();
			ui.state = UI_STATION;
		}

		else if( keypressed == '\x0C')
		{
			APP_getAcknowledgedIssue(&ackIssue);
			if( ackIssue.ID != -1 )
			{
				showUImsg(ackIssue.tag);
			}

		}
		else if( keypressed == '\x0A')
		{
			APP_resolveIssues(ackIssue.ID);
			ackIssue.ID = -1;
			setUImsg(UI_MSG_STATION);
			clearUIBuffer();
			clearUIInput();
			ui.state = UI_STATION;

		}

		break;

	

		case UI_CLEAR_ISSUE:

		if( keypressed == '\x08')
		{
			setUImsg(UI_MSG_STATION);
			clearUIBuffer();
			ui.state = UI_STATION;

		}

		else if( keypressed == '\x0A')
		{
			APP_clearIssues();
			clearUIBuffer();
			clearUIInput();
			setUImsg(UI_MSG_STATION);
			ui.state = UI_STATION;
		}

		break;

		


		case UI_PASSWORD:
		if( keypressed == '\x08')
		{
		
				setUImsg(UI_MSG_STATION);
				clearUIBuffer();
				clearUIInput();
				ui.state = UI_STATION;
		
		

		}

		else if( keypressed == '\x0A')
		{
			BOOL result = FALSE;
			ui.buffer[ui.bufferIndex] = '\0';
	
		
			 
			result = APP_checkPassword(ui.buffer);	
			if( result == TRUE )
			{
				setUImsg(UI_MSG_ADMIN_ACTIVITY);
				clearUIBuffer();
				clearUIInput();
				ui.state = UI_ADMIN_ACTIVITY;
			}
			else
			{
				setUImsg(UI_MSG_STATION);
				clearUIBuffer();
				clearUIInput();
				ui.state = UI_STATION;
			}
		
		
		}

		else 
		{
			ui.buffer[ui.bufferIndex] = keypressed;
			LCD_putChar('*');
			ui.bufferIndex++;
		}
			
		break;		

		case UI_ADMIN_ACTIVITY:

		if( keypressed == '\x08')
		{
			setUImsg(UI_MSG_STATION);
			clearUIBuffer();
			clearUIInput();
			ui.state = UI_STATION;
			
		}

		else if( keypressed == '0')
		{
			setUImsg(UI_MSG_CLEAR_ISSUES);
			clearUIBuffer();
			clearUIInput();
			ui.state = UI_CLEAR_ISSUE;
		}

		break;
	
		default:
		break;


	}



}


UINT8 mapKey(UINT8 scancode, UINT8 duration)
{
	UINT8 keypressed = 0xFF;
	switch(ui.state)
	{

		case UI_STATION:
		keypressed = keyMap[scancode][0];
		
		if( (ui.bufferIndex >=2 ))
		{
		
			if( (keypressed != '\x08') && (keypressed !='\x0A') )
				keypressed = 0xFF;
		}
		else if( ui.bufferIndex > 0 && ui.bufferIndex )
		{
			if( ( keypressed == '*') ||(keypressed =='\x0B')|| (keypressed =='\x0C')     
					|| (keypressed =='\x13'))
				keypressed = 0xFF;
		}
		else 
		{
			if( keypressed == '*') 
				keypressed = 0xFF;
		}

		break;




	

		case UI_ISSUE:

		keypressed = keyMap[scancode][0];

		if((keypressed != '1')  && 
			(keypressed != '2') && 
			(keypressed != '3') && 
			(keypressed != '4') && 
			(keypressed != '5') &&
			(keypressed != '6') &&
			(keypressed != '\x08') && 
			(keypressed !='\x0A')    ) 
			keypressed = 0xFF;



		break;

		case UI_BRK_QUA_MS:

		keypressed = keyMap[scancode][0];

		if( (keypressed != '\x0A') && (keypressed != '\x08') )
		{
			keypressed = 0xFF;
		}

		break;



		case UI_PART_NO:

		if(scancode == ui.prevcode)
		{
			if(duration < MIN_KEYPRESS_DURATION )
			{
				ui.keyIndex++;
				if(ui.keyIndex >= MAX_CHAR_PER_KEY)
					ui.keyIndex = 0;
			}
			else
			{
				ui.keyIndex = 0;
			}

			
		}
		else
		{
			ui.keyIndex = 0;
		}

		keypressed = keyMap[scancode][ui.keyIndex];

		if( keypressed == '\x0C' || keypressed == '\x0B' )
			keypressed = 0xFF;

		else if( keypressed == '\x0A' || keypressed == '\x08' )
			ui.keyIndex = 0;
	
		break;

		case UI_ISSUE_ACK:

		keypressed = keyMap[scancode][0];

		if( (keypressed != '\x0A') && (keypressed != '\x08') && (keypressed != '\x0B') )
		{
			keypressed = 0xFF;
		}

		break;



		case UI_ISSUE_RESOLVE:

		keypressed = keyMap[scancode][0];

		if( (keypressed != '\x0A') && (keypressed != '\x08') && (keypressed != '\x0C') )
		{
			keypressed = 0xFF;
		}

		break;





		case UI_PASSWORD:
			keypressed = keyMap[scancode][0];
		break;



		case UI_CLEAR_ISSUE:
		keypressed = keyMap[scancode][0];

		if( (keypressed != '\x0A') && (keypressed != '\x08') )
		{
			keypressed = 0xFF;
		}

		break;

		case UI_ADMIN_ACTIVITY:
		keypressed = keypressed = keyMap[scancode][0];
		if( (keypressed != '0') && (keypressed !='\x08') && (keypressed != '\x0A') )
			keypressed = 0xFF;
		break;



		default:
		break;

	}

	return keypressed;
}

void UI_setState( UI_STATE state)
{
	switch( state)
	{
		case UI_STATION:
			
			setUImsg(UI_MSG_STATION);
			clearUIBuffer();
			clearUIInput();
			ui.state = UI_STATION;

		break;




		case UI_PART_NO:
			
			setUImsg(UI_MSG_PART_NO);
			clearUIBuffer();
			clearUIInput();
			ui.state = UI_PART_NO;

		break;
		case UI_ISSUE:
			
			setUImsg(UI_MSG_ISSUE);
			clearUIBuffer();
			clearUIInput();
			ui.state = UI_ISSUE;

		break;




		default:
		break;
	}
}



UINT8 getStation(void)
{
	UINT8 i,station = 0;

	if( ui.bufferIndex == 1 )
	{
		ui.input[ui.inputIndex] = '0';
		ui.inputIndex++;
		ui.input[ui.inputIndex] = ui.buffer[0];
		ui.inputIndex++;
	}

	else
	{
		ui.input[ui.inputIndex] = ui.buffer[0];
		ui.inputIndex++;
		ui.input[ui.inputIndex] = ui.buffer[1];
		ui.inputIndex++;
	}

	station = (ui.input[0]-'0')*10 + (ui.input[1]-'0');

	return station;
}


void getData(void)
{
	UINT8 i;

	for( i = 0; i< ui.bufferIndex; i++)
	{
		ui.input[ui.inputIndex] = ui.buffer[i];
		ui.inputIndex++;
		
	}
	ui.input[ui.inputIndex] = '\0';
	ui.inputIndex++;

	if( ui.inputIndex >= MAX_INPUT_CHARS )
		ui.inputIndex = 0;
}


void clearUIBuffer(void)
{
	memset(ui.buffer,0, MAX_INPUT_CHARS);
	ui.bufferIndex = 0;
	ui.keyIndex = 0;
	ui.prevcode = 0xFF;

}


void clearUIInput(void)
{
	memset((UINT8*)ui.input,0, MAX_INPUT_CHARS);
	ui.inputIndex = 0;
}




void showUImsg( UINT8* msg )
{
	UINT8 i;

	
	LCD_clear();

	i = 0;
	while( msg[i] != '\0')
	{
		LCD_putChar(msg[i]);
		i++;
	}
}


void setUImsg( UINT8 msgIndex )
{
	UINT8 i;

	const rom UINT8 *msg;

	msg = UI_MSG[msgIndex] ;

	LCD_clear();

	i = 0;
	while( msg[i] != '\0')
	{
		LCD_putChar(msg[i]);
		i++;
	}
}


void putUImsg(UINT8 msgIndex)
{
	UINT8 i;

	const rom UINT8 *msg;

	msg = UI_MSG[msgIndex] ;

	i = 0;
	while( msg[i] != '\0')
	{
		LCD_putChar(msg[i]);
		i++;
	}
}


