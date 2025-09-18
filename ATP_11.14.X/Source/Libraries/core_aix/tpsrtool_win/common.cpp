/******************************************************************************

 Module: common.c
 
  Title: ASCENDENT Dialog Manager
  
   Description: This is a modified file of original common.c.
   Error logging to DB is replaced by logging to NT's
   event log
   
					   
******************************************************************************/
#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>


#include "tpsrtool.h"
#include "common.h"
#include "serial.h"
#include "tcp.h"
#include "pteipc.h"
#include "ntutils.h"

extern HWND hWnd;
INT      volatile connection_up  = FALSE;

INT      volatile trace_on       = TRUE;
BYTE     volatile trace_number   = 0;
INT      volatile trace_lock_on  = FALSE;
BYTE     volatile send_keepalive = FALSE;

CHAR     trace_file  [64];
CHAR     trace_queue [MAX_QUE_NAME_SIZE];

CHAR     gui_orig_info[20];

void     (*pfNotify)   (pBYTE, WORD, pIPC_INFO);
BOOLEAN  (*port_write) (const WORD, const void *, pIPC_INFO);
void     (*port_close) (INT);

/* Convert a two byte BCD value to an integer value.
bp - pointer to first byte (MSB) of 2 byte area. */
WORD bcd2bin(const pBYTE lpBcd)
{      
	WORD wTot;
	
	wTot = (lpBcd[0] / 0x10) * 1000  +
		(lpBcd[0] % 0x10) *  100  +
		(lpBcd[1] / 0x10) *   10  +
		(lpBcd[1] % 0x10);
	
	return(wTot);
}


/* Convert an unsigned number to a two byte bcd value
bp  - pointer to first byte (MSB) of 2 byte area
INT - unsigned integer value to be translated */
void bin2bcd(pBYTE lpBcd, const WORD wInt)
{
	WORD wJ;
	
	wJ = wInt / 100;
	lpBcd[0] = (BYTE)((wJ/10)*0x10 + (wJ%10));
	wJ = wInt % 100;
	lpBcd[1] = (BYTE)((wJ/10)*0x10 + (wJ%10));
}

/****************************************************
****************************************************/
BOOLEAN tpsr_port_init(BYTE type, const void * userdata, pIPC_INFO ipc_info, int tlevel)
{
	switch(type)
	{
	case  DM_PORTTYPE_TCP_SERVER_WITH_TPDU:
	case  DM_PORTTYPE_TCP_SERVER_WITHOUT_TPDU:
	case  DM_PORTTYPE_TCP_SERVER_IGNORE_TPDU:
		return(server_tcp_init((const USER_TCP_STRUCT *)userdata, ipc_info));
		break;
		
	case  DM_PORTTYPE_TCP_CLIENT_WITH_TPDU:
	case  DM_PORTTYPE_TCP_CLIENT_WITHOUT_TPDU:
	case  DM_PORTTYPE_TCP_CLIENT_IGNORE_TPDU:
		return(tpsr_client_tcp_init((const USER_TCP_STRUCT *)userdata, ipc_info, tlevel));
		break;
		
	default:
		printf("ASCENDENT: port_init, unknown port type passed to port init function");
		return(FALSE);
		break;
	}
	
	return(TRUE);
}

/****************************************************
****************************************************/
INT thread_logon(pCHAR instancename, pCHAR loginname)
{
	//return pteipc_xipclogin(instancename, loginname);
	return 0;
}

/****************************************************
****************************************************/
void thread_logoff()
{
	//pteipc_xipclogout();
}

/***********************************************************
Save the binary TPDU as ASCII data
***********************************************************/
void bin_to_ascii (pCHAR src, pCHAR dst, INT len)
{
	INT  i, j;
	
	for (i = 0; i < (len*2); i+=2)
	{
		j = (src[i/2] < 0) ? (src[i/2] + 256) : src[i/2];
		
		if ((j / 16) <= 9) 
			dst[i] = (j / 16) + '0';
		else
			dst[i] = (j/16 - 0x0a)+'A';
		
		if ((j & 0x0f) <= 9)
			dst[i+1] = (j & 0x0F) + '0'; 
		else
			dst[i+1] = (j & 0x0f) - 0x0a + 'A';    
	}
} 

/************************************************************
Retrieve the ASCII hex TPDU and convert it to binary data
************************************************************/
void ascii_to_bin (pBYTE src, pBYTE dst, INT len)
{
	INT  i, j, k;
	
	for (i = 0; i < (len*2); i+=2)
	{
		/* if I hit a null terminator, quit.  */
		if (src[i] == '\0')
			break;
		
		/* if these are leading blanks, leave the bcd zero.  */
		if (src[i] != ' ')
		{
			if (src[i] <= '9')
				dst[i/2] = ((src[i]-'0')*16); 
			else
				dst[i/2] = ((src[i]-'A')+0x0A)*16;   /* handle 0a-0f */
		}
		
		if (((i+1) < (len*2)) && (src[i+1] != ' '))
		{
			if (src[i+1] <= '9')
				dst[i/2] += (src[i+1]-'0'); 
			else
			{
				j         = (toupper(src[i+1]) - 'A');
				k         = j + 0x0A;
				dst[i/2] += k;
				
				/* dst[i/2] += ((src[i+1]-'A')+0x0A);   */
			}
		}
	}
} /* ascii_to_bin */

