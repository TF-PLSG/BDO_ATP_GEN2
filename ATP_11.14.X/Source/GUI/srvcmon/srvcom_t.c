/****************************************************************************************

   Module: srvcom_t.c

   Title: Pinnacle Services Monitor

   Description: This module provides an interface for the GUI to communicate with
                the Service Manager via applink.

   $Log:   N:\pvcs\PTE\CORE\Srvcmgr2\Gui\SRVCOM_T.C  $
   
      Rev 1.3   Mar 02 1999 16:37:42   MSALEH
   increase the time to wait for a response from 
   service manager
   
      Rev 1.2   Nov 18 1998 10:05:34   MSALEH
   Ran through Purify and corrected some
   calls to free()
   
      Rev 1.1   Nov 17 1998 12:12:36   MSALEH
   Added new functionality to manager
   logger/reply Primary/secondary designation,
   it works with logger usiong special message
   subtypes under MT_SYSTEM_REQUEST
   
      Rev 1.0   20 Aug 1998 09:18:34   rmalathk
   Initial revision of interface module for GUI to communicate
   with service manager using applink. This module
   replaces srvccomm.c which uses a XIPC interface.
   
*****************************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "srvccomm.h"
#include "ptetcp.h"
#include "srvcinfo.h"

CHAR srvcmgr_que_name[] = "srvcmgrA";
LONG time_out = 15;


//**********************************************************************************
//**********************************************************************************
BYTE srvccomm_get_service_table( SERVICE_INFO services_table[], pINT num_services,
                                   pCHAR error_msg )
{
	pPTE_MSG    p_msg_out1, p_msg_in1 ,p_msg_out2, p_msg_in2;
	LONG			data_len1,data_len2;
	pBYTE			p_data1, p_data2,p_data11,p_data21;
	pPTE_MSG_DATA	p_msg_data1, p_msg_data2;
	CHAR			temp_str1[100],temp_str2[100];
	BYTE			dberror1,dberror2;
	char * TemInfoData; // TO hold temp Server info data

	unsigned long int ncdatalen = 0;
	unsigned long int Nonncdatalen = 0;

	p_msg_out1   = NULL_PTR;
	p_msg_in1    = NULL_PTR;
	p_msg_out2   = NULL_PTR;
	p_msg_in2    = NULL_PTR;

    TemInfoData = (char *)malloc(sizeof(SERVICE_INFO) * MAX_SERVICES);
	error_msg[0]= NULL_PTR;

	#define    ST1_SRVC_GETNONNC       17
	#define    ST1_SRVC_GETNC          18

	// build the outgoing message
	// 1st get Non network data from Server

	p_msg_out1 = ptemsg_build_msg(  MT_SRVC_CONTROL,
								    ST1_SRVC_GETNONNC,
									0,
									srvcmgr_que_name,
									interactive_que_name,
									NULL_PTR,
									0,
									0 );

	if( p_msg_out1 == NULL_PTR )
	{
		strcpy( error_msg, "Insufficient memory to build request message" );
		return PTEMSG_INSUFFICIENT_MEMORY;
	}

	// try to send the request and receive a response
	p_msg_in1 = ptetcp_send_receive( p_msg_out1, time_out, temp_str1 );

	if( p_msg_in1 == NULL_PTR )
	{
		sprintf( error_msg, "Communication Error during request to ServiceManager: %s", temp_str1 );
		return PTEMSG_IPC_ERROR;
	}

	if( ( dberror1 = ptemsg_get_result_code( p_msg_in1 ) ) == PTEMSG_OK )
	{
		/* first retrieve the service table from the message */
		p_msg_data1    = ptemsg_get_pte_msg_data( p_msg_in1 );
		p_data1        = ptemsg_get_pte_msg_data_data( p_msg_data1 );
		data_len1      = ptemsg_get_pte_msg_data_data_length( p_msg_data1 );

		/* next retrieve the value for num services */
		p_msg_data1    = ptemsg_get_chained_pte_msg_data( p_msg_in1, 2 );
		p_data11        = ptemsg_get_pte_msg_data_data( p_msg_data1 );
		data_len1      = ptemsg_get_pte_msg_data_data_length( p_msg_data1 );

		memset( temp_str1, 0, sizeof( temp_str1 ) );
		strncpy( temp_str1, (pCHAR)p_data11, data_len1 );
		
		Nonncdatalen   = atoi(temp_str1);
		memset( services_table, 0, sizeof( services_table ) );
		memcpy( TemInfoData, p_data1, (Nonncdatalen * sizeof(SERVICE_INFO)) );


		// 1st get network data from Server_info
		p_msg_out2 = ptemsg_build_msg(  MT_SRVC_CONTROL,
										ST1_SRVC_GETNC,
										0,
										srvcmgr_que_name,
										interactive_que_name,
										NULL_PTR,
										0,
										0 );

		if( p_msg_out2 == NULL_PTR )
		{
			strcpy( error_msg, "Insufficient memory to build request message" );
			return PTEMSG_INSUFFICIENT_MEMORY;
		}

		// try to send the request and receive a response
		p_msg_in2 = ptetcp_send_receive( p_msg_out2, time_out, temp_str2 );

		if( p_msg_in2 == NULL_PTR )
		{
			sprintf( error_msg, "Communication Error during request to ServiceManager: %s", temp_str2 );
			return PTEMSG_IPC_ERROR;
		}

		if( ( dberror2 = ptemsg_get_result_code( p_msg_in2 ) ) == PTEMSG_OK )
		{
			/* first retrieve the service table from the message */
			p_msg_data2    = ptemsg_get_pte_msg_data( p_msg_in2 );
			p_data2        = ptemsg_get_pte_msg_data_data( p_msg_data2 );
			data_len2      = ptemsg_get_pte_msg_data_data_length( p_msg_data2 );
	
			
			/* next retrieve the value for num services */
			p_msg_data2    = ptemsg_get_chained_pte_msg_data( p_msg_in2, 2 );
			p_data21       = ptemsg_get_pte_msg_data_data( p_msg_data2 );
			data_len2      = ptemsg_get_pte_msg_data_data_length( p_msg_data2 );

			memset( temp_str2, 0, sizeof( temp_str2 ) );
			strncpy( temp_str2, (pCHAR)p_data21, data_len2 );
			ncdatalen = atoi(temp_str2);
			memcpy( TemInfoData + (Nonncdatalen* sizeof(SERVICE_INFO)), p_data2, (ncdatalen * sizeof(SERVICE_INFO)) );

			memcpy( services_table, TemInfoData, ((Nonncdatalen + ncdatalen) * sizeof(SERVICE_INFO) ));

			*num_services = atoi( temp_str1 ) + atoi( temp_str2 );
			//free(TemInfoData);
		}	
		return dberror2;
	}

	return dberror1;

}


//**********************************************************************************
//**********************************************************************************
BYTE srvccomm_control_service( pCHAR service_name, BYTE action, pCHAR error_msg )
{
   pPTE_MSG    p_msg_out, p_msg_in;
	BYTE			dberror;
	CHAR			temp_str[100];

   p_msg_out   = NULL_PTR;
   p_msg_in    = NULL_PTR;
   error_msg[0]= NULL_PTR;

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
BYTE srvccomm_load_rps_flag_service( pCHAR que_name,  pCHAR error_msg )
{
   pPTE_MSG    p_msg_out, p_msg_in;
	BYTE			dberror;
	CHAR			temp_str[100];

   p_msg_out   = NULL_PTR;
   p_msg_in    = NULL_PTR;
//   error_msg[0]= NULL_PTR;

    // build the outgoing message
	p_msg_out = ptemsg_build_msg( MT_SYSTEM_REQUEST,
								  ST1_LOAD_RPS_FLAG,
								  0,
								  que_name,
								  interactive_que_name,
								 (BYTE *) "",
								  strlen( que_name ) + 1,
								  0 );

	if( p_msg_out == NULL_PTR )
	{
		strcpy( error_msg, "Insufficient memory to build request message" );
		return PTEMSG_INSUFFICIENT_MEMORY;
	}

    // try to send the request and receive a response
	p_msg_in = ptetcp_send_receive( p_msg_out, time_out, temp_str );

	if( p_msg_in == NULL_PTR )
    	p_msg_in = ptetcp_receive( time_out, temp_str );

    if( p_msg_in == NULL_PTR )
    {
		sprintf( error_msg, "Communication Error during request to ServiceManager: %s", temp_str );
		return PTEMSG_IPC_ERROR;
	}

   dberror = ptemsg_get_result_code( p_msg_in );

	return dberror;
    
}

BYTE srvccomm_load_time_out_RC_and_reversal_attempts_values_service(pCHAR que_name,  pCHAR error_msg, BYTE Subtype )
{
	pPTE_MSG    p_msg_out, p_msg_in;
	BYTE			dberror;
	CHAR			temp_str[100];

   p_msg_out   = NULL_PTR;
   p_msg_in    = NULL_PTR;

	p_msg_out = ptemsg_build_msg( MT_SYSTEM_REQUEST,
								  Subtype,
								  0,
								  que_name,
								  interactive_que_name,
								 (BYTE *) "",
								  strlen( que_name ) + 1,
								  0 );

	if( p_msg_out == NULL_PTR )
	{
		strcpy( error_msg, "Insufficient memory to build request message" );
		return PTEMSG_INSUFFICIENT_MEMORY;
	}

    // try to send the request and receive a response
	p_msg_in = ptetcp_send_receive( p_msg_out, time_out, temp_str );

	if( p_msg_in == NULL_PTR )
    	p_msg_in = ptetcp_receive( time_out, temp_str );

    if( p_msg_in == NULL_PTR )
    {
		sprintf( error_msg, "Communication Error during request to ServiceManager: %s", temp_str );
		return PTEMSG_IPC_ERROR;
	}

   dberror = ptemsg_get_result_code( p_msg_in );

	return dberror;
}

BYTE srvccomm_load_OBS_records_service( pCHAR que_name,  pCHAR error_msg )
{
   pPTE_MSG    p_msg_out, p_msg_in;
	BYTE			dberror;
	CHAR			temp_str[100];

   p_msg_out   = NULL_PTR;
   p_msg_in    = NULL_PTR;
//   error_msg[0]= NULL_PTR;

    // build the outgoing message
	p_msg_out = ptemsg_build_msg( MT_SYSTEM_REQUEST,
								  ST1_LOAD_OBS_RECORDS,
								  0,
								  que_name,
								  interactive_que_name,
								 (BYTE *) "",
								  strlen( que_name ) + 1,
								  0 );

	if( p_msg_out == NULL_PTR )
	{
		strcpy( error_msg, "Insufficient memory to build request message" );
		return PTEMSG_INSUFFICIENT_MEMORY;
	}

    // try to send the request and receive a response
	p_msg_in = ptetcp_send_receive( p_msg_out, time_out, temp_str );

	if( p_msg_in == NULL_PTR )
    	p_msg_in = ptetcp_receive( time_out, temp_str );

    if( p_msg_in == NULL_PTR )
    {
		sprintf( error_msg, "Communication Error during request to ServiceManager: %s", temp_str );
		return PTEMSG_IPC_ERROR;
	}

   dberror = ptemsg_get_result_code( p_msg_in );

	return dberror;
    
}


BYTE srvccomm_load_VISA3DS2_records_service( pCHAR que_name,  pCHAR error_msg )
{
   pPTE_MSG    p_msg_out, p_msg_in;
	BYTE			dberror;
	CHAR			temp_str[100];

   p_msg_out   = NULL_PTR;
   p_msg_in    = NULL_PTR;
//   error_msg[0]= NULL_PTR;

    // build the outgoing message
	p_msg_out = ptemsg_build_msg( MT_SYSTEM_REQUEST,
								  ST1_LOAD_VISA3DS2_RECORDS,
								  0,
								  que_name,
								  interactive_que_name,
								 (BYTE *) "",
								  strlen( que_name ) + 1,
								  0 );

	if( p_msg_out == NULL_PTR )
	{
		strcpy( error_msg, "Insufficient memory to build request message" );
		return PTEMSG_INSUFFICIENT_MEMORY;
	}

    // try to send the request and receive a response
	p_msg_in = ptetcp_send_receive( p_msg_out, time_out, temp_str );

	if( p_msg_in == NULL_PTR )
    	p_msg_in = ptetcp_receive( time_out, temp_str );

    if( p_msg_in == NULL_PTR )
    {
		sprintf( error_msg, "Communication Error during request to ServiceManager: %s", temp_str );
		return PTEMSG_IPC_ERROR;
	}

   dberror = ptemsg_get_result_code( p_msg_in );

	return dberror;
    
}

BYTE srvccomm_load_AMEX3DS2_records_service( pCHAR que_name,  pCHAR error_msg )
{
   pPTE_MSG    p_msg_out, p_msg_in;
	BYTE			dberror;
	CHAR			temp_str[100];

   p_msg_out   = NULL_PTR;
   p_msg_in    = NULL_PTR;
//   error_msg[0]= NULL_PTR;

    // build the outgoing message
	p_msg_out = ptemsg_build_msg( MT_SYSTEM_REQUEST,
								  ST1_LOAD_AMEX3DS2_RECORDS,
								  0,
								  que_name,
								  interactive_que_name,
								 (BYTE *) "",
								  strlen( que_name ) + 1,
								  0 );

	if( p_msg_out == NULL_PTR )
	{
		strcpy( error_msg, "Insufficient memory to build request message" );
		return PTEMSG_INSUFFICIENT_MEMORY;
	}

    // try to send the request and receive a response
	p_msg_in = ptetcp_send_receive( p_msg_out, time_out, temp_str );

	if( p_msg_in == NULL_PTR )
    	p_msg_in = ptetcp_receive( time_out, temp_str );

    if( p_msg_in == NULL_PTR )
    {
		sprintf( error_msg, "Communication Error during request to ServiceManager: %s", temp_str );
		return PTEMSG_IPC_ERROR;
	}

   dberror = ptemsg_get_result_code( p_msg_in );

	return dberror;
    
}


BYTE srvccomm_load_DINERS3DS2_records_service( pCHAR que_name,  pCHAR error_msg )
{
   pPTE_MSG    p_msg_out, p_msg_in;
	BYTE			dberror;
	CHAR			temp_str[100];

   p_msg_out   = NULL_PTR;
   p_msg_in    = NULL_PTR;
//   error_msg[0]= NULL_PTR;

    // build the outgoing message
	p_msg_out = ptemsg_build_msg( MT_SYSTEM_REQUEST,
								  ST1_LOAD_DINERS3DS2_RECORDS,
								  0,
								  que_name,
								  interactive_que_name,
								 (BYTE *) "",
								  strlen( que_name ) + 1,
								  0 );

	if( p_msg_out == NULL_PTR )
	{
		strcpy( error_msg, "Insufficient memory to build request message" );
		return PTEMSG_INSUFFICIENT_MEMORY;
	}

    // try to send the request and receive a response
	p_msg_in = ptetcp_send_receive( p_msg_out, time_out, temp_str );

	if( p_msg_in == NULL_PTR )
    	p_msg_in = ptetcp_receive( time_out, temp_str );

    if( p_msg_in == NULL_PTR )
    {
		sprintf( error_msg, "Communication Error during request to ServiceManager: %s", temp_str );
		return PTEMSG_IPC_ERROR;
	}

   dberror = ptemsg_get_result_code( p_msg_in );

	return dberror;
    
}
BYTE srvccomm_load_JCB3DS2_records_service( pCHAR que_name,  pCHAR error_msg )
{
   pPTE_MSG    p_msg_out, p_msg_in;
	BYTE			dberror;
	CHAR			temp_str[100];

   p_msg_out   = NULL_PTR;
   p_msg_in    = NULL_PTR;
//   error_msg[0]= NULL_PTR;

    // build the outgoing message
	p_msg_out = ptemsg_build_msg( MT_SYSTEM_REQUEST,
								  ST1_LOAD_JCB3DS2_RECORDS,
								  0,
								  que_name,
								  interactive_que_name,
								 (BYTE *) "",
								  strlen( que_name ) + 1,
								  0 );

	if( p_msg_out == NULL_PTR )
	{
		strcpy( error_msg, "Insufficient memory to build request message" );
		return PTEMSG_INSUFFICIENT_MEMORY;
	}

    // try to send the request and receive a response
	p_msg_in = ptetcp_send_receive( p_msg_out, time_out, temp_str );

	if( p_msg_in == NULL_PTR )
    	p_msg_in = ptetcp_receive( time_out, temp_str );

    if( p_msg_in == NULL_PTR )
    {
		sprintf( error_msg, "Communication Error during request to ServiceManager: %s", temp_str );
		return PTEMSG_IPC_ERROR;
	}

   dberror = ptemsg_get_result_code( p_msg_in );

	return dberror;
    
}
BYTE srvccomm_load_JCB3DS2_tfinirecords_service( pCHAR que_name,  pCHAR error_msg )
{
   pPTE_MSG    p_msg_out, p_msg_in;
	BYTE			dberror;
	CHAR			temp_str[100];

   p_msg_out   = NULL_PTR;
   p_msg_in    = NULL_PTR;
//   error_msg[0]= NULL_PTR;

    // build the outgoing message
	p_msg_out = ptemsg_build_msg( MT_SYSTEM_REQUEST,
								  ST1_LOAD_JCB3DS2_TFINIRECORDS,
								  0,
								  que_name,
								  interactive_que_name,
								 (BYTE *) "",
								  strlen( que_name ) + 1,
								  0 );

	if( p_msg_out == NULL_PTR )
	{
		strcpy( error_msg, "Insufficient memory to build request message" );
		return PTEMSG_INSUFFICIENT_MEMORY;
	}

    // try to send the request and receive a response
	p_msg_in = ptetcp_send_receive( p_msg_out, time_out, temp_str );

	if( p_msg_in == NULL_PTR )
    	p_msg_in = ptetcp_receive( time_out, temp_str );

    if( p_msg_in == NULL_PTR )
    {
		sprintf( error_msg, "Communication Error during request to ServiceManager: %s", temp_str );
		return PTEMSG_IPC_ERROR;
	}

   dberror = ptemsg_get_result_code( p_msg_in );

	return dberror;
    
}
BYTE srvccomm_load_MidCpng_tfinirecords_service( pCHAR que_name,  pCHAR error_msg )
{
   pPTE_MSG    p_msg_out, p_msg_in;
	BYTE			dberror;
	CHAR			temp_str[100];

   p_msg_out   = NULL_PTR;
   p_msg_in    = NULL_PTR;
//   error_msg[0]= NULL_PTR;

    // build the outgoing message
	p_msg_out = ptemsg_build_msg( MT_SYSTEM_REQUEST,
								  ST1_LOAD_MID_CPNG_TFINIRECORDS,
								  0,
								  que_name,
								  interactive_que_name,
								 (BYTE *) "",
								  strlen( que_name ) + 1,
								  0 );

	if( p_msg_out == NULL_PTR )
	{
		strcpy( error_msg, "Insufficient memory to build request message" );
		return PTEMSG_INSUFFICIENT_MEMORY;
	}

    // try to send the request and receive a response
	p_msg_in = ptetcp_send_receive( p_msg_out, time_out, temp_str );

	if( p_msg_in == NULL_PTR )
    	p_msg_in = ptetcp_receive( time_out, temp_str );

    if( p_msg_in == NULL_PTR )
    {
		sprintf( error_msg, "Communication Error during request to ServiceManager: %s", temp_str );
		return PTEMSG_IPC_ERROR;
	}

   dberror = ptemsg_get_result_code( p_msg_in );

	return dberror;

}
BYTE srvccomm_load_CUP3DS2_records_service( pCHAR que_name,  pCHAR error_msg )
{
   pPTE_MSG    p_msg_out, p_msg_in;
	BYTE			dberror;
	CHAR			temp_str[100];

   p_msg_out   = NULL_PTR;
   p_msg_in    = NULL_PTR;
//   error_msg[0]= NULL_PTR;

    // build the outgoing message
	p_msg_out = ptemsg_build_msg( MT_SYSTEM_REQUEST,
								  ST1_LOAD_CUP3DS2_RECORDS,
								  0,
								  que_name,
								  interactive_que_name,
								 (BYTE *) "",
								  strlen( que_name ) + 1,
								  0 );

	if( p_msg_out == NULL_PTR )
	{
		strcpy( error_msg, "Insufficient memory to build request message" );
		return PTEMSG_INSUFFICIENT_MEMORY;
	}

    // try to send the request and receive a response
	p_msg_in = ptetcp_send_receive( p_msg_out, time_out, temp_str );

	if( p_msg_in == NULL_PTR )
    	p_msg_in = ptetcp_receive( time_out, temp_str );

    if( p_msg_in == NULL_PTR )
    {
		sprintf( error_msg, "Communication Error during request to ServiceManager: %s", temp_str );
		return PTEMSG_IPC_ERROR;
	}

   dberror = ptemsg_get_result_code( p_msg_in );

	return dberror;
    
}
BYTE srvccomm_reload_falcon_flag_service( pCHAR que_name,  pCHAR error_msg )
{
   pPTE_MSG    p_msg_out, p_msg_in;
	BYTE			dberror;
	CHAR			temp_str[100];

   p_msg_out   = NULL_PTR;
   p_msg_in    = NULL_PTR;
//   error_msg[0]= NULL_PTR;

    // build the outgoing message
	p_msg_out = ptemsg_build_msg( MT_SYSTEM_REQUEST,
								  ST1_RELOAD_FALCON_FLAG,
								  0,
								  que_name,
								  interactive_que_name,
								 (BYTE *) "",
								  strlen( que_name ) + 1,
								  0 );

	if( p_msg_out == NULL_PTR )
	{
		strcpy( error_msg, "Insufficient memory to build request message" );
		return PTEMSG_INSUFFICIENT_MEMORY;
	}

    // try to send the request and receive a response
	p_msg_in = ptetcp_send_receive( p_msg_out, time_out, temp_str );

	if( p_msg_in == NULL_PTR )
    	p_msg_in = ptetcp_receive( time_out, temp_str );

    if( p_msg_in == NULL_PTR )
    {
		sprintf( error_msg, "Communication Error during request to ServiceManager: %s", temp_str );
		return PTEMSG_IPC_ERROR;
	}

   dberror = ptemsg_get_result_code( p_msg_in );

	return dberror;
    
}

BYTE srvccomm_reload_SecExpiryDateValidation_flag_service( pCHAR que_name,  pCHAR error_msg )
{
   pPTE_MSG    p_msg_out, p_msg_in;
	BYTE			dberror;
	CHAR			temp_str[100];

   p_msg_out   = NULL_PTR;
   p_msg_in    = NULL_PTR;
//   error_msg[0]= NULL_PTR;

    // build the outgoing message
	p_msg_out = ptemsg_build_msg( MT_SYSTEM_REQUEST,
								  ST1_LOAD_SECONDARYEXPIRYDATE_FLAG,
								  0,
								  que_name,
								  interactive_que_name,
								 (BYTE *) "",
								  strlen( que_name ) + 1,
								  0 );

	if( p_msg_out == NULL_PTR )
	{
		strcpy( error_msg, "Insufficient memory to build request message" );
		return PTEMSG_INSUFFICIENT_MEMORY;
	}

    // try to send the request and receive a response
	p_msg_in = ptetcp_send_receive( p_msg_out, time_out, temp_str );

	if( p_msg_in == NULL_PTR )
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

   p_msg_out   = NULL_PTR;
   error_msg[0]= NULL_PTR;

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

	free( p_msg_out );

	if( !rcode )
	{
		sprintf( error_msg, "Communication Error during request to ServiceManager: %s", temp_str );
		return PTEMSG_IPC_ERROR;
	}

	return PTEMSG_OK;
    
}

//**********************************************************************************
//**********************************************************************************
BYTE srvccomm_confirm_status( pCHAR error_msg )
{
   pPTE_MSG p_msg_in;
	CHAR	   temp_str[100];
   BYTE     dberror;

   p_msg_in    = NULL_PTR;
   error_msg[0]= NULL_PTR;
    
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

   p_msg_out   = NULL_PTR;
   p_msg_in    = NULL_PTR;
   error_msg[0]= NULL_PTR;

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
BYTE srvccomm_vertexon_card_status_reload_service( pCHAR que_name, pCHAR error_msg )
{
   pPTE_MSG p_msg_out, p_msg_in;
	BYTE			dberror;
	CHAR			temp_str[100];

   p_msg_out   = NULL_PTR;
   p_msg_in    = NULL_PTR;
   error_msg[0]= NULL_PTR;

   // build the outgoing message
	p_msg_out = ptemsg_build_msg( MT_SYSTEM_REQUEST,
								  ST1_VERTEXON_CARD_STATUS_FLAG,
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
BYTE srvccomm_DB_control_service( pCHAR que_name, BYTE action ,pCHAR error_msg   )
{
	pPTE_MSG p_msg_out, p_msg_in;
	BYTE			dberror=0;
	CHAR			temp_str[100] = { 0 };

   p_msg_out   = NULL_PTR;
   p_msg_in    = NULL_PTR;
   error_msg[0]= NULL_PTR;

   // build the outgoing message
	p_msg_out = ptemsg_build_msg( MT_SYSTEM_REQUEST,
								  action,
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
INT srvccomm_change_connection( pCHAR machine_id )
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

//**********************************************************************************
//**********************************************************************************
BYTE srvccomm_get_primary_info( pCHAR que_name, pCHAR error_msg, pINT status )
{
   pPTE_MSG p_msg_out, p_msg_in;
	BYTE		msg_type, msg_subtype1;
	CHAR		temp_str[100];

   p_msg_out   = NULL_PTR;
   p_msg_in    = NULL_PTR;
   error_msg[0]= NULL_PTR;

   // build the outgoing message
	p_msg_out = ptemsg_build_msg( MT_SYSTEM_REQUEST,
								         ST1_SYS_ARE_YOU_PRIMARY,
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
		sprintf( error_msg, "Communication Error during request to Logger: %s", temp_str );
		return PTEMSG_IPC_ERROR;
	}

   msg_type = ptemsg_get_msg_type(p_msg_in);
   msg_subtype1 = ptemsg_get_msg_subtype1(p_msg_in);

   if (msg_type == MT_SYSTEM_REPLY)
   {
      *status = msg_subtype1;
   }
   else
   {
      sprintf( error_msg, "Srvccomm_get_primary_info: Unknown Msg Type: %d", msg_type );
      *status = msg_type;
   }

   return PTEMSG_OK;
}

//**********************************************************************************
//**********************************************************************************
BYTE srvccomm_send_be_primary_command(pCHAR que_name, pCHAR error_msg)
{
   INT      rcode;
   pPTE_MSG p_msg_out;
	CHAR		temp_str[100];

   p_msg_out   = NULL_PTR;
   error_msg[0]= NULL_PTR;

   // build the outgoing message
	p_msg_out = ptemsg_build_msg( MT_PRIVATE_LOGGER_REPLY,
								         ST1_PRV_BE_PRIMARY,
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
	rcode = ptetcp_send( p_msg_out, temp_str );

   free(p_msg_out);

	if( !rcode )
	{
		sprintf( error_msg, "Communication Error during request to Logger: %s", temp_str );
		return PTEMSG_IPC_ERROR;
	}


   return PTEMSG_OK;
}

//**********************************************************************************
//**********************************************************************************
BYTE srvccomm_send_switch_primary_command(pCHAR que_name, pCHAR error_msg)
{
   INT      rcode;
   pPTE_MSG p_msg_out;
	CHAR		temp_str[100];

   p_msg_out   = NULL_PTR;
   error_msg[0]= NULL_PTR;

   // build the outgoing message
	p_msg_out = ptemsg_build_msg( MT_PRIVATE_LOGGER_REPLY,
								         ST1_PRV_SWITCH_PRIMARY,
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
	rcode = ptetcp_send( p_msg_out, temp_str );

   free(p_msg_out);

	if( !rcode )
	{
		sprintf( error_msg, "Communication Error during request to Logger: %s", temp_str );
		return PTEMSG_IPC_ERROR;
	}


   return PTEMSG_OK;
}
