/*********************************************************************************
* Module:
*
* Title:
*
* Description:
*
* Application:
*
* Author:
*
* Revision History:
*  
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\SRVCOM_T.C  $
   
      Rev 1.5   Dec 01 2003 11:30:16   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.4   Jan 05 2000 14:54:20   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:59:40   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:16   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:24   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:57:02   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:41:16   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:10:04   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 11:00:06   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:09:44   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:48:22   iarustam
    
*
************************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "srvccomm.h"
#include "ptetcp.h"

CHAR srvcmgr_que_name[] = "srvcmgrA";
LONG time_out = 5;


//**********************************************************************************
//**********************************************************************************
BYTE srvccomm_get_service_table( SERVICE_INFO services_table[], pINT num_services,
                                   pCHAR error_msg )
{
    pPTE_MSG p_msg_out, p_msg_in;
	LONG			data_len;
	pBYTE			p_data;
	pPTE_MSG_DATA	p_msg_data;
	CHAR			temp_str[100];
	BYTE			dberror;

    p_msg_out = NULL_PTR;
    p_msg_in = NULL_PTR;

    // build the outgoing message
	p_msg_out = ptemsg_build_msg( MT_SRVC_CONTROL,
								  ST1_SRVC_GETALL,
								  0,
								  srvcmgr_que_name,
								  interactive_que_name,
								  NULL_PTR,
								  0,
								  0 );

	if( p_msg_out == NULL_PTR )
	{
		strcpy( error_msg, "Insufficient memory to build request message" );
		return PTEMSG_INSUFFICIENT_MEMORY;
	}

    // try to send the request and receive a response
	p_msg_in = ptetcp_send_receive( p_msg_out, time_out, temp_str );
	if( p_msg_in == NULL_PTR )
	{
		sprintf( error_msg, "Communication Error during request to ServiceManager: %s", temp_str );
		return PTEMSG_IPC_ERROR;
	}


	if( ( dberror = ptemsg_get_result_code( p_msg_in ) ) == PTEMSG_OK )
	{
         /* first retrieve the service table from the message */
		p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
		p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
		data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );

        memset( services_table, 0, sizeof( services_table ) );
        memcpy( services_table, p_data, data_len );

        /* next retrieve the value for num services */
        p_msg_data    = ptemsg_get_chained_pte_msg_data( p_msg_in, 2 );
        p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
		data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );
         
        memset( temp_str, 0, sizeof( temp_str ) );
        strncpy( temp_str, (pCHAR)p_data, data_len );
        *num_services = atoi( temp_str );
	}

	return dberror;
    
}


//**********************************************************************************
//**********************************************************************************
BYTE srvccomm_control_service( pCHAR service_name, BYTE action, pCHAR error_msg )
{
   pPTE_MSG p_msg_out, p_msg_in;
	BYTE			dberror;
	CHAR			temp_str[100];

   p_msg_out = NULL_PTR;
   p_msg_in = NULL_PTR;

    // build the outgoing message
	p_msg_out = ptemsg_build_msg( MT_SRVC_CONTROL,
								  action,
								  0,
								  srvcmgr_que_name,
								  interactive_que_name,
								  service_name,
								  strlen( service_name ) + 1,
								  0 );

	if( p_msg_out == NULL_PTR )
	{
		strcpy( error_msg, "Insufficient memory to build request message" );
		return PTEMSG_INSUFFICIENT_MEMORY;
	}

    // try to send the request and receive a response
	p_msg_in = ptetcp_send_receive( p_msg_out, time_out, temp_str );

	if( p_msg_in == NULL_PTR )
        // timed out. give it some more time.
    	p_msg_in = ptetcp_receive( time_out, temp_str );

    if( p_msg_in == NULL_PTR )
    {
		sprintf( error_msg, "Communication Error during request to ServiceManager: %s", temp_str );
		return PTEMSG_IPC_ERROR;
	}


   dberror = ptemsg_get_result_code( p_msg_in );
	return dberror;
    
}



//**********************************************************************************
//**********************************************************************************
BYTE srvccomm_process_all( BYTE action, pCHAR error_msg )
{
   pPTE_MSG    p_msg_out;
	CHAR			temp_str[100];
   INT         rcode;

   p_msg_out = NULL_PTR;

   // build the outgoing message
	p_msg_out = ptemsg_build_msg( MT_SRVC_CONTROL,
								  action,
								  0,
								  srvcmgr_que_name,
								  interactive_que_name,
								  NULL_PTR,
								  0,
								  0 );

	if( p_msg_out == NULL_PTR )
	{
		strcpy( error_msg, "Insufficient memory to build request message" );
		return PTEMSG_INSUFFICIENT_MEMORY;
	}

    // try to send the request
	rcode = ptetcp_send( p_msg_out, temp_str );
	if( !rcode )
	{
		sprintf( error_msg, "Communication Error during request to ServiceManager: %s", temp_str );
		return PTEMSG_IPC_ERROR;
	}

	free( p_msg_out );
	return PTEMSG_OK;
    
}



//**********************************************************************************
//**********************************************************************************
BYTE srvccomm_confirm_status( pCHAR error_msg )
{
    pPTE_MSG p_msg_in;
	CHAR	  temp_str[100];
    BYTE      dberror;

    p_msg_in = NULL_PTR;
    
    p_msg_in = ptetcp_receive( 10, temp_str ); 
	if( p_msg_in != NULL_PTR )
	{
        dberror = ptemsg_get_result_code( p_msg_in );
	    return dberror;
	}
    else
	{
		sprintf( error_msg, "Communication Error during request to ServiceManager: %s", temp_str );
		return PTEMSG_IPC_ERROR;
	}

}




//**********************************************************************************
//**********************************************************************************
BYTE srvccomm_ping_service( pCHAR que_name, pCHAR error_msg )
{
    pPTE_MSG p_msg_out, p_msg_in;
	BYTE			dberror;
	CHAR			temp_str[100];

    p_msg_out = NULL_PTR;
    p_msg_in = NULL_PTR;

    // build the outgoing message
	p_msg_out = ptemsg_build_msg( MT_SYSTEM_REQUEST,
								  ST1_SYS_PING,
								  0,
								  que_name,
								  interactive_que_name,
								  NULL_PTR,
								  0,
								  0 );

	if( p_msg_out == NULL_PTR )
	{
		strcpy( error_msg, "Insufficient memory to build request message" );
		return PTEMSG_INSUFFICIENT_MEMORY;
	}

    // try to send the request and receive a response
	p_msg_in = ptetcp_send_receive( p_msg_out, time_out, temp_str );

	if( p_msg_in == NULL_PTR )
	{
		sprintf( error_msg, "Communication Error during request to ServiceManager: %s", temp_str );
		return PTEMSG_IPC_ERROR;
	}


    dberror = ptemsg_get_result_code( p_msg_in );
	return dberror;    
}




//**********************************************************************************
//**********************************************************************************
INT srvccomm_change_connection( pCHAR machine_id, pCHAR instance )
{
    CHAR error_msg[100];

    // disconnect from the current login
    ptetcp_disconnect();

    if( ptetcp_connect( machine_id, error_msg ) )
        return 1;
    else
      return 1;
}



//**********************************************************************************
//**********************************************************************************
INT srvccomm_disconnect()
{

    // disconnect from the current login
    ptetcp_disconnect();
    return 1;
}