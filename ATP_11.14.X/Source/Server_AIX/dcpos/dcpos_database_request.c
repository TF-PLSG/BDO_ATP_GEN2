/**************************************************************************************************
*  
* MODULE:      Database_Request.c

  Copyright (c) 2007, Hypercom, Inc.
  All Rights Reserved.

* TITLE:       ATP Standard
*  
* DESCRIPTION: Contains database request routines
*
* APPLICATION: dcpos
*
* AUTHOR:  Abhishek Verma
*    
**************************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "basictyp.h"
#include "pteipc.h"
#include "dcpos_global_definitions.h"
#include "memmnger.h"
#include "dcpos_database_request.h"
#include "dcpos_database_response.h"
#include "dcpos_process_incoming_msg.h"
#include "dcpos_process_outgoing_msg.h"
#include "dcpos_service_functions.h"
#include "txutils.h"
#include "ntutils.h"
#include "dcpos_general_utilities.h"

extern AUTH_TX auth_tx ;
extern TPOS01   Tpos_details;

extern CHAR		posds_que_name[];
extern CHAR		trands_que_name [] ;
extern CHAR     dcpiso_que_name[];

BYTE p_buffer [MAX_DATASERVER_BUFFER_LENGTH] ; /* defined to be 25000 */

BYTE Get_BIN01_Details (BYTE read_mode)
{
	LONG	nReturnCode = 0L ;
	BOOLEAN bReturnCode = DEVICE_OK ;
	BIN01	sBinDetails ;
	char	strBuffer[30] = {0} ;
	char    strError [200] = {0};
	
	BYTE	strCardNumber[21] = {0} ;
	BYTE	strBinType[2] = {0} ;
	BYTE	nAccountType = 0 ; /* determines if the card is a CREDIT, DEBIT, FLEET, 
	                              or LOYALTY card */
	
	memset (&sBinDetails, 0, sizeof sBinDetails) ;
	strcpy (strCardNumber, auth_tx.TLF01_details.card_num) ;

	strBinType[0] = auth_tx.bin_type ;

	if (read_mode == SHARED_MEMORY_READ)
	{
		nReturnCode = FindMemBinnRange (strCardNumber, strBinType,(BYTE*) &sBinDetails) ;
		if (nReturnCode >= 0L)
		{
			/* have found the bin range in shared memory */
			memcpy (&auth_tx.BIN01_details, &sBinDetails, sizeof sBinDetails) ;
			return RECORD_FOUND_IN_SHARED_MEMORY ; 
		}
		else if (-1L == nReturnCode)
		{
			/* bin range not found in shared memory */
			sprintf (strError, "Bin Details not present in SHM MEM for Bintype %c ", auth_tx.bin_type) ;
			dcpos_log_message( 2, 1,strError,"Get_BIN01_Details",1 );
			return RECORD_NOT_FOUND_IN_SHARED_MEMORY ;
		}
		else
		{
			/* problems with the shared memory, so get from database */
			sprintf (strError, "Bin Details fetch failed from SHM MEM for Bintype %c ", auth_tx.bin_type) ;
			dcpos_log_message( 2, 1,strError,"Get_BIN01_Details",1 );
			return SYSTEM_PROBLEM_READING_SHARED_MEMORY ;
			
		}
	}
	else if (read_mode == DATABASE_READ)
	{
		memcpy (strBuffer, strCardNumber, strlen (strCardNumber)) ;
		memcpy (strBuffer+20, strBinType, strlen (strBinType)) ;
		sprintf (strError, "Send DB request to fetch Bin details for Bintype %c ", auth_tx.bin_type) ;
		dcpos_log_message( 2, 1,strError,"Get_BIN01_Details",1 );
		bReturnCode = Send_Request_To_Service (	trands_que_name,
												MT_DB_REQUEST, 
			                                    ST1_DB_CHECK_BIN_RANGE, 
												0, BIN01_DATA, 
												(pBYTE) strBuffer, 
												sizeof strBuffer) ;
		if (false == bReturnCode) 
		{
			sprintf (strError, "Not able to send request to trands for Bintype %c, forwad to dcpiso",auth_tx.bin_type) ;
			dcpos_log_message( 2, 1,strError,"Get_BIN01_Details",1 );
			bReturnCode = Send_Request_To_Service ( dcpiso_que_name ,
											   MT_INCOMING_DCPOS,
											   0,
											   0,
											   0,
											   0,
											   0) ;
			return SYSTEM_ERROR ;
		}
		return DEVICE_OK ;
	}
	return DEVICE_OK ;
} /* TxDataServer_Get_BIN01_Details */


BYTE Insert_TPOS01_Details( BYTE subtype2 )
{
	BYTE retCode = 0 ;
	BYTE err_buf[100]={0};
    CHAR buffer1[AUTH_TX_SIZE + TPOS01_SIZE];

    memset( buffer1, 0x00, sizeof( buffer1 ) );
    memcpy( buffer1, &auth_tx, sizeof(auth_tx) );

    Prepare_TPOS01_details();
	memcpy( buffer1+AUTH_TX_SIZE, &Tpos_details, sizeof(Tpos_details) );
    /*retCode = Send_Request_To_Service ( posds_que_name,
                                        MT_DB_REQUEST,
                                        ST1_DB_INSERT,
                                        subtype2,
										TPOS01_DATA,
                                        (pBYTE) &buffer1,
                                        sizeof(buffer1)) ;*/

    retCode = build_and_send_request_message( MT_DB_REQUEST,
    										  ST1_DB_INSERT,
                                              ST2_NONE,
											  posds_que_name,
                                              (pBYTE)buffer1,
                                              sizeof(buffer1),
											  TPOS01_DATA,
                                              NULL_STR,
                                              err_buf );
	return retCode ;
} /* Insert_TPOS01_Details */



BYTE Send_Request_To_Service (BYTE* que_name, BYTE nType, BYTE nSubType1, 
							  BYTE nSubType2, BYTE nAppDataType, 
							  pBYTE p_data,   INT nDataSize)
{
	LONG retCode = 0L ;
	pPTE_MSG p_msg_out = NULL_PTR ;
	INT nBufferSize = 0 ;
	char strError[512] = {0} ;
    CHAR logger_que_name[256] = {0} ;
	CHAR orig_comms_info [MAX_ORIG_INFO_SIZE] = {0} ; 

	memset (p_buffer, 0, sizeof p_buffer) ;
	memcpy (p_buffer, &auth_tx, sizeof auth_tx) ;
	
	if (p_data != 0)
		memcpy (p_buffer + sizeof auth_tx, p_data, nDataSize) ;
		
	nBufferSize = sizeof auth_tx + nDataSize ;

	p_msg_out = ptemsg_build_msg (  nType, 
								    nSubType1, 
								    nSubType2, 
								    que_name, 
								    application_que_name, 
								    (pBYTE) p_buffer, 
								    nBufferSize, 
								    nAppDataType) ;

	if (p_msg_out == NULL_PTR)
	{	
		strcpy (strError,"Insufficient Memory for building terminal message") ;
		
		Generate_Usage_Error_Message (SET_SYSTEM_MONITOR, strError,
									"Send_Request_To_Service",
									GS_TXN_SYSTEM_ERROR,
									SYSTEM_MALFUNCTION) ;
		return SYSTEM_ERROR ;
 	} 

	retCode = pteipc_send (p_msg_out, que_name) ;
	free (p_msg_out) ;

	if (retCode != 0)
	{	
		sprintf (strError,"Error %ld has occurred sending message to %s\n", retCode, que_name) ;
        
		Generate_Usage_Error_Message (	SET_SYSTEM_MONITOR, strError,
										"Send_Request_To_Service",
										GS_TXN_SYSTEM_ERROR,
										SYSTEM_MALFUNCTION) ;
		return SYSTEM_ERROR ;
	}

	return DEVICE_OK ;
} /* Send_Request_To_Service */

/******************************************************************************
 *
 *  NAME:         Retrieve_BIN01_Record_Dcpos
 *
 *  DESCRIPTION:  This function get the Bin details
 *
 *  INPUTS:       NULL
 *
 *  OUTPUTS:     Get the bin details if it is present
 *
 *  RTRN VALUE:   DEVICE_OK on success else error
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/

BYTE Retrieve_BIN01_Record_Dcpos (BYTE bin_type)
{

	BYTE ReturnCode = 0 ;
	char strError[200] = {0};

	auth_tx.bin_type = bin_type ;

	/* Get the BIN record for the card */
	/* Set the first parameter to SHARED_MEMORY_READ or DATABASE_READ */
	ReturnCode = Get_BIN01_Details (SHARED_MEMORY_READ) ;
	if (ReturnCode == RECORD_FOUND_IN_SHARED_MEMORY) // found the bin range in memory
	{
		return RECORD_FOUND_IN_SHARED_MEMORY ;
	}
	else if ( ReturnCode == RECORD_NOT_FOUND_IN_SHARED_MEMORY)
	{
		sprintf (strError, "Bin Details not present in SHM MEM for Debit, Lets check Credit in SHM MEM ") ;
		dcpos_log_message( 2, 1,strError,"Retrieve_BIN01_Record_Dcpos",1 );
		auth_tx.bin_type = 'C' ;
		ReturnCode = Get_BIN01_Details (SHARED_MEMORY_READ) ;
		if(ReturnCode == RECORD_FOUND_IN_SHARED_MEMORY )
		{
			return RECORD_FOUND_IN_SHARED_MEMORY_CREDIT ;
		}
		else if (ReturnCode == RECORD_NOT_FOUND_IN_SHARED_MEMORY )
		{
			/* bin range not found in shared memory
			sprintf (strError, "Bin Details not present in SHM MEM for Credit, forward to dcpiso ") ;
			dcpos_log_message( 2, 1,strError,"Retrieve_BIN01_Record_Dcpos",1 );
			ReturnCode = Send_Request_To_Service ( dcpiso_que_name ,
													MT_INCOMING_DCPOS,
													0,
													0,
													0,
													0,
													0) ;
			return ReturnCode ;*/

			sprintf (strError, "Bin Details not present in SHM MEM for Credit, Lets check in DB ") ;
			dcpos_log_message( 2, 1,strError,"Retrieve_BIN01_Record_Dcpos",1 );
			auth_tx.bin_type = 'C' ;
			ReturnCode = Get_BIN01_Details (DATABASE_READ);
			return ReturnCode ;
		}
		else if (ReturnCode == SYSTEM_PROBLEM_READING_SHARED_MEMORY)
		{
			/* bin range not found in shared memory */
			sprintf (strError, "SHM MEM read failed for Credit , Lets check in DB ") ;
			dcpos_log_message( 2, 1,strError,"Retrieve_BIN01_Record_Dcpos",1 );
			auth_tx.bin_type = 'C' ;
			ReturnCode = Get_BIN01_Details (DATABASE_READ);
			return ReturnCode ;
		}
	}
	else if (ReturnCode == SYSTEM_PROBLEM_READING_SHARED_MEMORY)
	{
		sprintf (strError, "Bin Details fetch failed from SHM MEM for Bintype %c, Lets check in DB ",bin_type) ;
		dcpos_log_message( 2, 1,strError,"Retrieve_BIN01_Record_Dcpos",1 );
		ReturnCode = Get_BIN01_Details (DATABASE_READ) ;
		return ReturnCode ;
	}
	return DEVICE_OK;
} /* Retrieve_BIN01_Record */


/******************************************************************************
 *
 *  NAME:         Retrieve_BIN01_Record_Dcpos_Credit
 *
 *  DESCRIPTION:  This function get the Bin details
 *
 *  INPUTS:       NULL
 *
 *  OUTPUTS:     Get the bin details if it is present
 *
 *  RTRN VALUE:   DEVICE_OK on success else error
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/

BYTE Retrieve_BIN01_Record_Dcpos_Credit (BYTE bin_type)
{

	BYTE ReturnCode = 0 ;
	char strError[200] = {0};

	auth_tx.bin_type = bin_type ;

	/* Get the BIN record for the card */
	/* Set the first parameter to SHARED_MEMORY_READ or DATABASE_READ */
	ReturnCode = Get_BIN01_Details (SHARED_MEMORY_READ) ;
	if (ReturnCode == RECORD_FOUND_IN_SHARED_MEMORY) // found the bin range in memory
	{
		return RECORD_FOUND_IN_SHARED_MEMORY ;
	}
	else if ( ReturnCode == RECORD_NOT_FOUND_IN_SHARED_MEMORY)
	{
		/* bin range not found in shared memory */
		sprintf (strError, "Bin Details not present in SHM MEM for Credit, forward to dcpiso ") ;
		dcpos_log_message( 2, 1,strError,"Retrieve_BIN01_Record_Dcpos_Credit",1 );
		ReturnCode = Send_Request_To_Service ( dcpiso_que_name ,
												MT_INCOMING_DCPOS,
												0,
												0,
												0,
												0,
												0) ;
		return ReturnCode ;;
	}
	else if (ReturnCode == SYSTEM_PROBLEM_READING_SHARED_MEMORY)
	{
		sprintf (strError, "Bin Details fetch failed from SHM MEM for Bintype %c, Lets check in DB ",bin_type) ;
		dcpos_log_message( 2, 1,strError,"Retrieve_BIN01_Record_Dcpos_Credit",1 );
		ReturnCode = Get_BIN01_Details (DATABASE_READ) ;
		return ReturnCode ;
	}
	return DEVICE_OK;
} /* Retrieve_BIN01_Record */



