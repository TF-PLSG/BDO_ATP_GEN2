/*

  Krishna@TF, 20.5.11: 
  
	This Library file contains all the common functions of UI Modules like: 
	ValidateIdleLogin
	
*/

#include "ptepci.h"
#include "app_info.h"
extern "C"
{
	#include "string.h"
	#include "stdio.h"
	#include "common.h"
	#include "Txutils.h"
	#include "dc_dbstruct.h"
	#include "nc_dbstruct.h"
	#include "tx_dbstruct.h"
	#include "txdsapi.h"
	#include "ptedes.h"
}

/*
Krishna@TF, 18.5.11 : Function to authenticate user during Idle Login of ATP
*/


short CPtepci::isalphabet(char string[])
{
	int i;
	int length;
	
	length = (int)strlen(string);
	
	for (i = 0; i < length; i++)
	{
		if (isalpha(string[i]))
			return 1;
	}
	return 0;
}
short CPtepci::isnumeric(char string[])
{
	int i;
	int length;
	
	length = (int)strlen(string);
	
	for (i = 0; i < length; i++)
	{
		if (isdigit(string[i]))
			return 1;
	}
	return 0;
}


short CPtepci::ValidateIdleLogin(char m_userid[], char m_userpwd[], char *error)
{
	char tmpPassword[51] = {0};	
	int rcode;
	
	UCF01 ucf01;
	
	strncpy(tmpPassword, m_userpwd, 50);
	memset(&ucf01, 0x00, sizeof(ucf01));
	
	strcpy( (char *)ucf01.primary_key.name, m_userid );
	
	rcode = txdsapi_get_record( (pBYTE)&ucf01,sizeof (UCF01), UCF01_DATA, error );
	
	if( rcode == PTEMSG_NOT_FOUND )
	{			
		strcpy(error,"Invalid username/password.  Login denied.");
		return 0;
	}
	else if( rcode != PTEMSG_OK )
	{            
		return 0;
	}
	
	//Checking the User accountstatus
	if( strcmp( (char *)ucf01.accountstatus, USER_LOCKED ) == 0 )
	{
		strcpy(error,"Your User id is disabled/inactive to run ATP. Please contact your System Administrator.");
		return 0;			
	}	
	
	if(strlen(m_userid) < MIN_USERNAME_LEN)
	{		 	  
		sprintf(error, "User Name length is less than %d chars", MIN_USERNAME_LEN);
		
		return 0;
	}
	if(strlen(tmpPassword) < MIN_USERPWD_LEN)
	{
		sprintf(error, "User Password length is less than %d chars", MIN_USERPWD_LEN);
		
		return 0;
	}
	if(!isalphabet(tmpPassword))
	{
		strcpy(error,"User Password does not contain any Alphabets");
		return 0;
	}
	if(!isnumeric(tmpPassword))
	{
		strcpy(error,"User Password does not contain any Numbers");
		return 0;
	}
	/**** Encrypt the CHANGE OLD password  - Start ****/
	{
		int len = strlen(tmpPassword);
		DWORD hash = 0;
		if(len <= MAX_USERPWD_LEN)
			hash = gui_password_encryption((BYTE *)tmpPassword,len);
		//simple_encryption((BYTE *)tmpPassword,len);
	}
	/**** Encrypt the password  - End ****/
	// check password		
	if( strcmp( (char *)ucf01.password, tmpPassword ) != 0 )
	{		
		strcpy(error,"Invalid username/password.  Login denied.");
		return 2;
	}
	
	
		  return 1;
}

short CPtepci::BlockUser(char m_userid[], char *error)
{
	int rcode;
	
	UCF01 ucf01;
	
	memset(&ucf01, 0x00, sizeof(ucf01));
	
	strcpy( (char *)ucf01.primary_key.name, m_userid );	 
	
	rcode = txdsapi_get_record( (pBYTE)&ucf01,sizeof (UCF01), UCF01_DATA, error );
	
	if( rcode == PTEMSG_NOT_FOUND )
	{			
		strcpy(error,"Invalid username/password.  Login denied.");
		return 0;
	}
	else if( rcode != PTEMSG_OK )
	{            
		return 0;
	}
	
	strcpy( (char *)ucf01.accountstatus , USER_LOCKED);
	rcode = txdsapi_update_record( (pBYTE)&ucf01, sizeof (UCF01), UCF01_DATA, error);
	
	if( rcode == PTEMSG_OK )
	{
		strcpy(error,"Your User id is disabled/inactive to run ATP. Please contact your System Administrator.");		
	}
	return 1;
	
}