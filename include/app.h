#ifndef _APP_H_
#define _APP_H_


/*
*----------------------------------------------------------------------------------------------------------------
*	MACROS
*-----------------------------------------------------------------------------------------------------------------
*/

//#define __FACTORY_CONFIGURATION__
#define	COIL_STATUS_CHECK_TIME		10UL

/*
*----------------------------------------------------------------------------------------------------------------
*	Enumerations
*-----------------------------------------------------------------------------------------------------------------
*/
typedef enum 
{
	OFF,
	ON
}INDICATOR_STATE;

typedef enum _ISSUE_TYPE
{
	NO_ISSUE,
	RAISED,
	RESOLVED
}ISSUE_TYPE;



typedef enum _APP_PARAM
{
	MAX_KEYPAD_ENTRIES = 27,
	MAX_ISSUES = 24,
	MAX_DEPARTMENTS = 20,
	MAX_LOG_ENTRIES = 13,
	LOG_BUFF_SIZE = MAX_KEYPAD_ENTRIES+1

}APP_PARAM;

typedef enum _LOGDATA
{
	HW_TMEOUT = 10,
	APP_TIMEOUT = 1000,
	TIMESTAMP_UPDATE_VALUE = (APP_TIMEOUT/HW_TMEOUT)
}LOGDATA;

typedef enum
{
	ISSUE_RESOLVED,
	ISSUE_RAISED,
	ISSUE_ACKNOWLEDGED,
	ISSUE_CRITICAL
}APP_STATE;

enum
{
	CMD_RAISE_ISSUE = 0x80,
	CMD_ACKNOWLEDGE_ISSUE = 0x81,
	CMD_RESOLVE_ISSUE = 0x82,
	CMD_PRODUCTION_COUNT = 0x83,


	CMD_SET_ADMIN_PASSWORD = 0x91,
	CMD_SET_LOGON_PASSWORD = 0x92,
	CMD_SET_BUZZER_TIMEOUT = 0x93,


	CMD_PING = 0xA0,
	CMD_CLEAR_ISSUES = 0xA1

};

enum
{
	SEND_LOG,
	CHECK_COIL_STATUS
};

typedef struct _OpenIssue
{
	UINT8 tag[32];
	INT8 ID;
}OpenIssue;

extern void APP_init(void);
extern void APP_task(void);
extern BOOL  APP_updateIssueInfo( UINT8 depId , ISSUE_TYPE issueType);
void APP_raiseIssues(far UINT8* data);
void APP_acknowledgeIssues(UINT8 ID);
void APP_resolveIssues(UINT8 id);
void APP_clearIssues(void);
BOOL APP_checkPassword(UINT8 *password);
BOOL APP_login(UINT8 *password,UINT8 *data);
BOOL APP_logout(UINT8 *password,UINT8 *data);
void APP_getOpenIssue(OpenIssue *);
void APP_getAcknowledgedIssue(far OpenIssue *openIssue);
#endif