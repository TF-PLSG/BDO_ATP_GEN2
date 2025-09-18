/**************************************************************************************************
*  
* MODULE:      Database_Response.c

  Copyright (c) 2005, Hypercom, Inc.
  All Rights Reserved.

* TITLE:       ATP Standard
*  
* DESCRIPTION: Contains database response routines
*
* APPLICATION: dcpos
*
* AUTHOR:  Abhishek Verma

      
**************************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "basictyp.h"
#include "ptemsg.h"
#include "dcpos_global_definitions.h"
#include "dcpos_database_request.h"
#include "dcpos_database_response.h"
#include "dcpos_general_utilities.h"
#include "dcpos_service_functions.h"
#include "dcpos_process_incoming_msg.h"
#include "dcpos_process_outgoing_msg.h"
#include "genutil.h"
#include "txutils.h"
#include "ntutils.h"

extern AUTH_TX auth_tx ;
extern CHAR dcpiso_que_name[];

BYTE Process_Dataserver_OK_Message (pPTE_MSG p_msg_in)
{
   CHAR           strError[256] = {0} ;
   pBYTE          pCurrent = NULL_PTR ;
   pPTE_MSG_DATA  p_msg_data = NULL_PTR ;
   BYTE           subType1 = 0, subType2 = 0 ;
   BYTE           app_data_type = 0 ;
   BYTE           retCode = 0 ;

   p_msg_data = ptemsg_get_pte_msg_data (p_msg_in) ;
   pCurrent = ptemsg_get_pte_msg_data_data (p_msg_data) ;

   memset (&auth_tx, 0, sizeof auth_tx) ;
   memcpy (&auth_tx, pCurrent, sizeof auth_tx) ;

   app_data_type = ptemsg_get_pte_msg_data_app_data_type (p_msg_data) ;
   subType1 = ptemsg_get_msg_subtype1 (p_msg_in) ;
   subType2 = ptemsg_get_msg_subtype2 (p_msg_in) ;

   retCode = Perform_Auth_Tx_Checks () ;
   if (retCode != DEVICE_OK)
         return retCode ;

   if (auth_tx.TLF01_details.tx_key < 100)
   {   
	   if(auth_tx.bin_type=='D')
	   {
		   if (subType1 == ST1_DB_CHECK_BIN_RANGE)
		   {
			   if (app_data_type == BIN01_DATA)
			   {
				   retCode = Process_select_bin01_db_OK (pCurrent) ;
			   }
			   else
			   {
				   sprintf (strError, "Invalid App data type %d for Debit Bin,Look for Credit Bin",app_data_type) ;
				   dcpos_log_message( 2, 1,strError,"Process_Dataserver_OK_Message",1 );
				   retCode = Process_bin01_db_not_found (pCurrent) ;
			   }
		   }
		   else
		   {
			   sprintf (strError, "Invalid subtype1 %d for Debit Bin, Look for Credit Bin",subType1) ;
			   dcpos_log_message( 2, 1,strError,"Process_Dataserver_OK_Message",1 );
			   retCode = Process_bin01_db_not_found (pCurrent) ;
		   }
	   }
	   else  if(auth_tx.bin_type=='C')
	   {
		   if (subType1 == ST1_DB_CHECK_BIN_RANGE)
		   {
			   if (app_data_type == BIN01_DATA)
			   {
				   retCode = Process_select_bin01_db_OK (pCurrent) ;
			   }
			   else
			   {
				   sprintf (strError, "Invalid App data type %d for Credit Bin,Forward to dcpiso",app_data_type) ;
				   dcpos_log_message( 2, 1,strError,"Process_Dataserver_OK_Message",1 );
				   retCode = Send_Request_To_Service ( dcpiso_que_name ,
						   	   	   	   	   	   	   	   MT_INCOMING_DCPOS,
													   0,
													   0,
													   0,
													   0,
													   0) ;
			   }
		   }
		   else
		   {
			   sprintf (strError, "Invalid subtype1 %d for Credit Bin, Look for Credit Bin",subType1) ;
			   dcpos_log_message( 2, 1,strError,"Process_Dataserver_OK_Message",1 );
			   retCode = Send_Request_To_Service ( dcpiso_que_name ,
												   MT_INCOMING_DCPOS,
												   0,
												   0,
												   0,
												   0,
												   0) ;
		   }
	   }
	   else
	   {
			sprintf (strError, "Invalid Bin type %c, forward to dcpiso",auth_tx.bin_type) ;
			dcpos_log_message( 2, 1,strError,"Process_Dataserver_OK_Message",1 );
			retCode = Send_Request_To_Service ( dcpiso_que_name ,
												MT_INCOMING_DCPOS,
												0,
												0,
												0,
												0,
												0) ;
	   }
   }
   return DEVICE_OK ;
} /* Process_Dataserver_OK_Message */

BYTE Process_Dataserver_NotFound_Message (pPTE_MSG p_msg_in)
{
	pBYTE			pCurrent = 0 ;
	pPTE_MSG_DATA	p_msg_data= 0 ;
	BYTE			app_data_type = 0 ;
	pCHAR			temp_dest_queue = 0 ;
	pCHAR			temp_orig_queue = 0 ;
	CHAR			error_msg[200] = {0} ;
	CHAR			Buffer[240] = {0} ;
	BYTE			subtype1 = 0, subtype2 = 0 ;
	BYTE			retCode = 0 ;
	CHAR           strError[256] = {0} ;

	p_msg_data = ptemsg_get_pte_msg_data(p_msg_in) ;
	pCurrent = ptemsg_get_pte_msg_data_data(p_msg_data) ;

	memset (&auth_tx, 0, sizeof auth_tx) ;
	memcpy (&auth_tx, pCurrent, sizeof auth_tx) ;

	app_data_type = ptemsg_get_pte_msg_data_app_data_type (p_msg_data) ;
	temp_dest_queue = ptemsg_get_msg_dest_queue (p_msg_in) ;
	temp_orig_queue = ptemsg_get_msg_orig_queue (p_msg_in) ;

	subtype1 = ptemsg_get_msg_subtype1 (p_msg_in) ;
	subtype2 = ptemsg_get_msg_subtype2 (p_msg_in) ;
	
	retCode = Perform_Auth_Tx_Checks () ;
	if (retCode != DEVICE_OK)
	{
		dcpos_log_message( 2, 1,"Perform_Auth_Tx_Checks Failed","Process_Dataserver_NotFound_Message",1 );
		return retCode ;
	}

	retCode = DEVICE_OK;
	if (auth_tx.TLF01_details.tx_key < 100)
	{   
		if(auth_tx.bin_type=='D')
		{
			if (subtype1 == ST1_DB_CHECK_BIN_RANGE )
			{
				if (app_data_type == BIN01_DATA)
				{
					retCode = Process_bin01_db_not_found (pCurrent) ;
				}
				else
				{
					sprintf (strError, "Invalid App data type %d for Debit Bin, Look for Credit Bin",app_data_type) ;
					dcpos_log_message( 2, 1,strError,"Process_Dataserver_NotFound_Message",1 );
					retCode = Process_bin01_db_not_found (pCurrent) ;
				}
			}
			else
			{
				sprintf (strError, "Invalid subtype1 %d for Debit Bin, Look for Credit Bin",subtype1) ;
				dcpos_log_message( 2, 1,strError,"Process_Dataserver_NotFound_Message",1 );
				retCode = Process_bin01_db_not_found (pCurrent) ;
			}
		}
		else if (auth_tx.bin_type=='C')
		{
			if (subtype1 == ST1_DB_CHECK_BIN_RANGE )
			{
				if (app_data_type == BIN01_DATA)
				{
					dcpos_log_message( 2, 1,"No Credit Bin found in Database, Forward to dcpsio","Process_Dataserver_NotFound_Message",1 );
					retCode = Send_Request_To_Service ( dcpiso_que_name ,
														MT_INCOMING_DCPOS,
														0,
														0,
														0,
														0,
														0) ;
				}
				else
				{
					sprintf (strError, "Invalid App data type %d for Credit Bin, Forward to dcpsio",app_data_type) ;
					dcpos_log_message( 2, 1,strError,"Process_Dataserver_NotFound_Message",1 );
					retCode = Send_Request_To_Service ( dcpiso_que_name ,
																MT_INCOMING_DCPOS,
																0,
																0,
																0,
																0,
																0) ;
				}
			}
			else
			{
				sprintf (strError, "Invalid subtype1 %d for Credit Bin, Forward to dcpsio",subtype1) ;
				dcpos_log_message( 2, 1,strError,"Process_Dataserver_NotFound_Message",1 );
				retCode = Send_Request_To_Service ( dcpiso_que_name ,
													MT_INCOMING_DCPOS,
													0,
													0,
													0,
													0,
													0) ;
			}
		}
		else
		{
			sprintf (strError, "Invalid Bin type %c, forward to dcpiso",auth_tx.bin_type);
			dcpos_log_message( 2, 1,strError,"Process_Dataserver_NotFound_Message",1 );
			retCode = Send_Request_To_Service ( dcpiso_que_name ,
												MT_INCOMING_DCPOS,
												0,
												0,
												0,
												0,
												0) ;
		}
	}

	return( retCode );
} /* Process_Dataserver_NotFound_Message */

BYTE Process_Dataserver_ERROR_Message (pPTE_MSG p_msg_in)
{
   pBYTE          pCurrent = NULL_PTR ;
   pPTE_MSG_DATA  p_msg_data = NULL_PTR ;
   BYTE           retCode;
   BYTE           app_data_type = 0;
   BYTE           subType2;
   CHAR           strError[512] = {0} ;

   p_msg_data    = ptemsg_get_pte_msg_data (p_msg_in) ;
   pCurrent      = ptemsg_get_pte_msg_data_data (p_msg_data) ;
   app_data_type = ptemsg_get_pte_msg_data_app_data_type (p_msg_data) ;

   memset (&auth_tx, 0, sizeof auth_tx) ;
   memcpy (&auth_tx, pCurrent, sizeof auth_tx) ;

   strcpy (strError, pCurrent + sizeof auth_tx) ;
   retCode = Generate_Usage_Error_Message (SET_SYSTEM_MONITOR, strError,
                                          "Process_Dataserver_ERROR_Message",
                                           GS_TXN_SYSTEM_ERROR,
                                           SYSTEM_MALFUNCTION) ;
   if (app_data_type == TPOS01_DATA)
   {

         sprintf( strError, "Unable to Insert in TPOS01" );
         retCode = Generate_Usage_Error_Message (SET_SYSTEM_MONITOR, strError,
                                                 "Process_Dataserver_ERROR_Message",
                                                 GS_TXN_SYSTEM_ERROR,
                                                 SYSTEM_MALFUNCTION );

   }
   else
   {
	   sprintf (strError, "Databases Error while processing transaction with App data type %s",app_data_type) ;
			    dcpos_log_message( 2, 1,strError,"Process_Dataserver_OK_Message",1 );
			    retCode = Send_Request_To_Service ( dcpiso_que_name ,
												   MT_INCOMING_DCPOS,
												   0,
												   0,
												   0,
												   0,
												   0) ;
   }
    return retCode ;
} /* Process_Dataserver_ERROR_Message */

BYTE Process_select_bin01_db_OK (pBYTE pCurrent)
{
	BYTE retCode = 0 ;

	BIN01 sBIN01 ;

	memset (&sBIN01, 0, sizeof sBIN01) ;
	memcpy (&sBIN01, pCurrent+ sizeof auth_tx, sizeof sBIN01) ;
	memcpy (&auth_tx.BIN01_details, &sBIN01, sizeof sBIN01) ;

    if(auth_tx.bin_type=='D')
    {
       	retCode = Process_When_Debit_Bin_Range_Found_Dcpos () ;
    }
    else
    {
    	retCode = Process_When_Credit_Bin_Range_Found_Dcpos () ;
    }
	return retCode ;

} /* Process_select_bin01_db_OK */

BYTE Process_bin01_db_not_found (pBYTE pCurrent)
{
	BYTE retCode = 0 ;
	BYTE AccountType_credit = 'C' ;
	BIN01 sBIN01 ;

	memset (&sBIN01, 0, sizeof sBIN01) ;
	memcpy (&sBIN01, pCurrent+ sizeof auth_tx, sizeof sBIN01) ;
	memcpy (&auth_tx.BIN01_details, &sBIN01, sizeof sBIN01) ;

	if(auth_tx.bin_type=='D')
	{
		// No debit Bin is present , Let know check for credit bin
		retCode= Retrieve_BIN01_Record_Dcpos_Credit(AccountType_credit);
		if (retCode == RECORD_FOUND_IN_SHARED_MEMORY)
		{
			retCode = Process_When_Credit_Bin_Range_Found_Dcpos () ;
		}
		else
		{
			// Record is not found in SHM MEM , It will go do Data base read flow.
		}
	}
	else
	{
		dcpos_log_message( 2, 1,"No Credit Bin found in Database, Forward to dcpsio","Process_bin01_db_not_found",1 );
		retCode = Send_Request_To_Service ( dcpiso_que_name ,
											MT_INCOMING_DCPOS,
											0,
											0,
											0,
											0,
											0) ;
	}
	return retCode ; /* returns SYSTEM_ERROR for any system related error */
} /* Process_bin01_db_not_found */

