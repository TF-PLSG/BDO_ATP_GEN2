/**************************************************************************************************
*  
* MODULE:      message_handler.c

  Copyright (c) 2008, Hypercom, Inc.
  All Rights Reserved.

* TITLE:       ATP Standard - Terminal Handler Main.
*  
* DESCRIPTION: Main startup and end process routines. Waits for incoming PTE message and 
*              passes control to message processing loop.  
*
* APPLICATION: dcpos
*
* AUTHOR:  Abhishek Verma
**************************************************************************************************/

#include <stdio.h>
#include "pteipc.h"
#include "dcpos_global_definitions.h"
#include "ntutils.h"
#include "ptesystm.h"
#include "dcpos_message_handler.h"
#include "genutil.h"
#include "txutils.h"
#include "txtrace.h"
#include "diskio.h"
#include "dcpos_service_functions.h"
#include "dcpos_process_incoming_msg.h"
#include "dcpos_process_outgoing_msg.h"
#include "dcpos_database_response.h"
#include "dcpos_general_utilities.h"

AUTH_TX auth_tx ;
TPOS01  Tpos_details;

CHAR    trands_que_name []     = "trandsA" ;
CHAR    posds_que_name []      = "posdsA" ;
CHAR    oprmon_que_name []     = "oprmonI" ;
CHAR    dcpiso_que_name[]         = "dcpisoA" ;
CHAR    Version[]              = "ATP_11.14.0";
											
/* Statistics for Monitor */
TXN_COUNTS     Txn_Counts;
MONITOR_STATS  Monitor_Stats;

/* Txn Tracing Info */
CHAR   DirPath[MAX_APP_NAME_SIZE];
CHAR   TraceFile[MAX_APP_NAME_SIZE];
INT    Tracing;
FILE   TraceFile_fp;
FILE   *pTraceFile_fp;
FILE   **ppTraceFile_fp;

/* RRN counter to maintain a unique value per second (0 - 99). */
INT  RRNctr;

/* External Function Prototypes */
extern CHAR     ServiceName[12] ;
extern INT      EndProcessSignalled ;
extern INT      MainProcessDone ;
INT void_reload_timeout;
INT mra_send_falg = 0;

CHAR response_code_for_DUAL_BRAND_DEBIT_TRANSACTION[3]={0};
CHAR response_code_for_DEBIT_MAGSTRIPE_TRANSACTION[3]={0};
CHAR response_code_for_DEBIT_EMV_TRANSACTION[3]={0};
CHAR response_code_for_DCC_TRANSACTION[3]={0};
CHAR response_code_for_DINERS_TRANSACTION[3]={0};
CHAR response_code_for_FLEET_CARD_TRANSACTION[3]={0};
CHAR response_code_for_CUP_UPI_MAGSTRIPE_TRANSACTION[3]={0};
CHAR response_code_for_AMEX_TRANSACTION_ON_BIN_NII[3]={0};
CHAR response_code_for_BIN_NOT_FOUND_FOR_BIN_ROUTING[3]={0};

CHAR  CUP_merchant_indicator[8]={0};
CHAR  dcpos_Erro_warning_Filename[256] = {0};
CHAR  dcpos_module_error_warning_file_name[256] = {0};
CHAR  dcpos_error_warning_file_path[256] = {0};

CHAR DCC_Termina_Starting_Prefix_1[3]={0};
CHAR DCC_Termina_Starting_Prefix_2[3]={0};
CHAR DCC_Termina_Starting_Prefix_3[3]={0};
CHAR DCC_Termina_Starting_Prefix_4[3]={0};
CHAR DCC_Termina_Starting_Prefix_5[3]={0};
CHAR DCC_Termina_Starting_Prefix_6[3]={0};
CHAR DCC_Termina_Starting_Prefix_7[3]={0};
CHAR DCC_Termina_Starting_Prefix_8[3]={0};
CHAR DCC_Termina_Starting_Prefix_9[3]={0};
CHAR DCC_Termina_Starting_Prefix_10[3]={0};

/*****************************************************************************
  Function:   end_process

  Description: This function will shutdown the terminal handler.
  Author:  Abhishek Verma
  
******************************************************************************/
void EndProcess(void)
{
   CHAR Buffer[100] = {0};

#ifdef _DEBUG
    pteipc_trace_off(); 
#endif

   if ( Tracing == TRACE_ON )
   {
      if ( false == close_trace_file( ppTraceFile_fp ))
      {
         sprintf( Buffer, "Unable to close trace file %s", TraceFile );
         dcpos_log_message( 1, 1, Buffer,"EndProcess",0 );
      }
   }

   sprintf( Buffer, "Shutting down the %s Service, version %s",
            ServiceName, Version );
   dcpos_log_message( 2, 1, Buffer, "EndProcess", 0 );
   strcat( Buffer, "\n" );
   PRINT( Buffer );

} /* EndProcess */


/******************************************************************************
 *
 *  NAME:         Read_DUAL_BRAND_DEBIT_TRANSACTION_Response_code
 *
 *  DESCRIPTION:  This function reads information from the tf ini file.
 *
 *  INPUTS:       None
 *  OUTPUTS:      err_buf - Text message explaining why this function failed
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT Read_DUAL_BRAND_DEBIT_TRANSACTION_Response_code( void )
{
   DWORD rc = 0;
   CHAR filename     [MAX_FILENAME_SIZE] = {0};
   CHAR tmpstr       [MAX_FILENAME_SIZE] = {0};
   CHAR tmpstring    [MAX_INI_INFO_SIZE] = {0};
   CHAR ini_info     [MAX_INI_INFO_SIZE] = {0};
   INT  ret_val = true;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
   rc = GetPrivateProfileString(
             "BIN_ROUTING_RESPONSE_CODE",    /* points to section name       */
             "DUAL_BRAND_DEBIT_TRANSACTION",  /* points to key name           */
             "0",                       /* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   strncpy( response_code_for_DUAL_BRAND_DEBIT_TRANSACTION, tmpstring, 2 );
   if(strlen(response_code_for_DUAL_BRAND_DEBIT_TRANSACTION)==0)
   {
	   strncpy( response_code_for_DUAL_BRAND_DEBIT_TRANSACTION,
	   		    DUAL_BRAND_DEBIT_TRANSACTION_RESPONSE_CODE,
	   		    RESPONSE_CODE_LEN);;// Default Value
   }
   return( ret_val );
}



/******************************************************************************
 *
 *  NAME:         Read_DEBIT_MAGSTRIPE_TRANSACTION_Response_code
 *
 *  DESCRIPTION:  This function reads information from the tf ini file.
 *
 *  INPUTS:       None
 *  OUTPUTS:      err_buf - Text message explaining why this function failed
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT Read_DEBIT_MAGSTRIPE_TRANSACTION_Response_code( void )
{
   DWORD rc = 0;
   CHAR filename     [MAX_FILENAME_SIZE] = {0};
   CHAR tmpstr       [MAX_FILENAME_SIZE] = {0};
   CHAR tmpstring    [MAX_INI_INFO_SIZE] = {0};
   CHAR ini_info     [MAX_INI_INFO_SIZE] = {0};
   INT  ret_val = true;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
   rc = GetPrivateProfileString(
             "BIN_ROUTING_RESPONSE_CODE",    /* points to section name       */
             "DEBIT_MAGSTRIPE_TRANSACTION",  /* points to key name           */
             "0",                       /* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   strncpy( response_code_for_DEBIT_MAGSTRIPE_TRANSACTION, tmpstring, 2 );
   if(strlen(response_code_for_DEBIT_MAGSTRIPE_TRANSACTION)==0)
   {
	   strncpy( response_code_for_DEBIT_MAGSTRIPE_TRANSACTION,
	   			DEBIT_MAGSTRIPE_TRANSACTION_RESPONSE_CODE,
	   			RESPONSE_CODE_LEN);;// Default Value
   }
   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         Read_DEBIT_EMV_TRANSACTION_Response_code
 *
 *  DESCRIPTION:  This function reads information from the tf ini file.
 *
 *  INPUTS:       None
 *  OUTPUTS:      err_buf - Text message explaining why this function failed
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT Read_DEBIT_EMV_TRANSACTION_Response_code( void )
{
   DWORD rc = 0;
   CHAR filename     [MAX_FILENAME_SIZE] = {0};
   CHAR tmpstr       [MAX_FILENAME_SIZE] = {0};
   CHAR tmpstring    [MAX_INI_INFO_SIZE] = {0};
   CHAR ini_info     [MAX_INI_INFO_SIZE] = {0};
   INT  ret_val = true;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
   rc = GetPrivateProfileString(
             "BIN_ROUTING_RESPONSE_CODE",     /* points to section name       */
             "DEBIT_EMV_TRANSACTION",  /* points to key name           */
             "0",                       /* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   strncpy( response_code_for_DEBIT_EMV_TRANSACTION, tmpstring, 2 );
   if(strlen(response_code_for_DEBIT_EMV_TRANSACTION)==0)
   {
	   strncpy( response_code_for_DEBIT_EMV_TRANSACTION,
	   			DEBIT_EMV_TRANSACTION_RESPONSE_CODE,
	   			RESPONSE_CODE_LEN);// Default Value
   }
   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         Read_DCC_TRANSACTION_Response_code
 *
 *  DESCRIPTION:  This function reads information from the tf ini file.
 *
 *  INPUTS:       None
 *  OUTPUTS:      err_buf - Text message explaining why this function failed
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT Read_DCC_TRANSACTION_Response_code( void )
{
   DWORD rc = 0;
   CHAR filename     [MAX_FILENAME_SIZE] = {0};
   CHAR tmpstr       [MAX_FILENAME_SIZE] = {0};
   CHAR tmpstring    [MAX_INI_INFO_SIZE] = {0};
   CHAR ini_info     [MAX_INI_INFO_SIZE] = {0};
   INT  ret_val = true;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
   rc = GetPrivateProfileString(
             "BIN_ROUTING_RESPONSE_CODE",     /* points to section name       */
             "DCC_TRANSACTION",  			/* points to key name           */
             "0",                       /* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   strncpy( response_code_for_DCC_TRANSACTION, tmpstring, 2 );
   if(strlen(response_code_for_DCC_TRANSACTION)==0)
   {
	   strncpy( response_code_for_DCC_TRANSACTION,
	   			DCC_TRANSACTION_RESPONSE_CODE,
	   			RESPONSE_CODE_LEN);// Default Value
   }
   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         Read_DINERS_TRANSACTION_Response_code
 *
 *  DESCRIPTION:  This function reads information from the tf ini file.
 *
 *  INPUTS:       None
 *  OUTPUTS:      err_buf - Text message explaining why this function failed
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT Read_DINERS_TRANSACTION_Response_code( void )
{
   DWORD rc = 0 ;
   CHAR filename     [MAX_FILENAME_SIZE] = {0};
   CHAR tmpstr       [MAX_FILENAME_SIZE] = {0};
   CHAR tmpstring    [MAX_INI_INFO_SIZE] = {0};
   CHAR ini_info     [MAX_INI_INFO_SIZE] = {0};
   INT  ret_val = true;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
   rc = GetPrivateProfileString(
             "BIN_ROUTING_RESPONSE_CODE",   /* points to section name       */
             "DINERS_TRANSACTION",  		/* points to key name           */
             "0",                       	/* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   strncpy( response_code_for_DINERS_TRANSACTION, tmpstring, 2 );
   if(strlen(response_code_for_DINERS_TRANSACTION)==0)
   {
	   strncpy( response_code_for_DINERS_TRANSACTION,
	   			DINERS_TRANSACTION_RESPONSE_CODE,
	   			RESPONSE_CODE_LEN);// Default Value
   }
   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         Read_FLEET_CARD_TRANSACTION_Response_code
 *
 *  DESCRIPTION:  This function reads information from the tf ini file.
 *
 *  INPUTS:       None
 *  OUTPUTS:      err_buf - Text message explaining why this function failed
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT Read_FLEET_CARD_TRANSACTION_Response_code( void )
{
   DWORD rc = 0;
   CHAR filename     [MAX_FILENAME_SIZE] = {0};
   CHAR tmpstr       [MAX_FILENAME_SIZE] = {0};
   CHAR tmpstring    [MAX_INI_INFO_SIZE] = {0};
   CHAR ini_info     [MAX_INI_INFO_SIZE] = {0};
   INT  ret_val = true;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
   rc = GetPrivateProfileString(
             "BIN_ROUTING_RESPONSE_CODE",   /* points to section name       */
             "FLEET_CARD_TRANSACTION",  	/* points to key name           */
             "0",                       	/* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   strncpy( response_code_for_FLEET_CARD_TRANSACTION, tmpstring, 2 );
   if(strlen(response_code_for_FLEET_CARD_TRANSACTION)==0)
   {
	   strncpy( response_code_for_FLEET_CARD_TRANSACTION,
	   			FLEET_CARD_TRANSACTION_RESPONSE_CODE,
	   			RESPONSE_CODE_LEN);// Default Value
   }
   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         Read_CUP_UPI_MAGSTRIPE_TRANSACTION_Response_code
 *
 *  DESCRIPTION:  This function reads information from the tf ini file.
 *
 *  INPUTS:       None
 *  OUTPUTS:      err_buf - Text message explaining why this function failed
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT Read_CUP_UPI_MAGSTRIPE_TRANSACTION_Response_code( void )
{
   DWORD rc = 0;
   CHAR filename     [MAX_FILENAME_SIZE] = {0};
   CHAR tmpstr       [MAX_FILENAME_SIZE] = {0};
   CHAR tmpstring    [MAX_INI_INFO_SIZE] = {0};
   CHAR ini_info     [MAX_INI_INFO_SIZE] = {0};
   INT  ret_val = true;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
   rc = GetPrivateProfileString(
             "BIN_ROUTING_RESPONSE_CODE",     /* points to section name       */
             "CUP_UPI_MAGSTRIPE_TRANSACTION",  /* points to key name           */
             "0",                       		/* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   strncpy( response_code_for_CUP_UPI_MAGSTRIPE_TRANSACTION, tmpstring, 2 );
   if(strlen(response_code_for_CUP_UPI_MAGSTRIPE_TRANSACTION)==0)
   {
	   strncpy( response_code_for_CUP_UPI_MAGSTRIPE_TRANSACTION,
	   			CUP_UPI_MAGSTRIPE_TRANSACTION_RESPONSE_CODE,
	   			RESPONSE_CODE_LEN);// Default Value
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         Read_AMEX_TRANSACTION_ON_BIN_NII_Response_code
 *
 *  DESCRIPTION:  This function reads information from the tf ini file.
 *
 *  INPUTS:       None
 *  OUTPUTS:      err_buf - Text message explaining why this function failed
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT Read_AMEX_TRANSACTION_ON_BIN_NII_Response_code( void )
{
   DWORD rc = 0;
   CHAR filename     [MAX_FILENAME_SIZE] = {0};
   CHAR tmpstr       [MAX_FILENAME_SIZE] = {0};
   CHAR tmpstring    [MAX_INI_INFO_SIZE] = {0};
   CHAR ini_info     [MAX_INI_INFO_SIZE] = {0};
   INT  ret_val = true;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
   rc = GetPrivateProfileString(
             "BIN_ROUTING_RESPONSE_CODE",     /* points to section name       */
             "AMEX_TRANSACTION",  			  /* points to key name           */
             "79",                       	  /* points to default string     */
             ini_info,                        /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,           /* size of destination buffer   */
             filename                         /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   strncpy( response_code_for_AMEX_TRANSACTION_ON_BIN_NII, tmpstring, 2 );
   if(strlen(response_code_for_AMEX_TRANSACTION_ON_BIN_NII)==0)
   {
	   strncpy( response_code_for_AMEX_TRANSACTION_ON_BIN_NII,
			    AMEX_TRANSACTION_ON_BIN_NII_RESPONSE_CODE,
	   			RESPONSE_CODE_LEN);// Default Value
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         Read_BIN_NOT_FOUND_FOR_BIN_ROUTING_Eror_Response_code
 *
 *  DESCRIPTION:  This function reads information from the tf ini file.
 *
 *  INPUTS:       None
 *  OUTPUTS:      err_buf - Text message explaining why this function failed
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT Read_BIN_NOT_FOUND_FOR_BIN_ROUTING_Eror_Response_code( void )
{
   DWORD rc = 0;
   CHAR filename     [MAX_FILENAME_SIZE] = {0};
   CHAR tmpstr       [MAX_FILENAME_SIZE] = {0};
   CHAR tmpstring    [MAX_INI_INFO_SIZE] = {0};
   CHAR ini_info     [MAX_INI_INFO_SIZE] = {0};
   INT  ret_val = true;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
   rc = GetPrivateProfileString(
             "ERROR_RESPONSE_CODE",     /* points to section name       */
             "BIN_NOT_FOUND_FOR_BIN_ROUTING",  /* points to key name           */
             "0",                       		/* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   strncpy( response_code_for_BIN_NOT_FOUND_FOR_BIN_ROUTING, tmpstring, 2 );
   if(strlen(response_code_for_BIN_NOT_FOUND_FOR_BIN_ROUTING)==0)
   {
	   strncpy( response_code_for_BIN_NOT_FOUND_FOR_BIN_ROUTING,
			    NO_BIN_FOUND_RESPONSE_ERROR_CODE,
	   			RESPONSE_CODE_LEN);// Default Value
   }
   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         dcpos_read_CUP_merchant_indicator
 *
 *  DESCRIPTION:  This function reads information from the tf ini file.
 *
 *  INPUTS:       None
 *  OUTPUTS:      err_buf - Text message explaining why this function failed
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT dcpos_read_CUP_merchant_indicator( void )
{
   DWORD rc = 0;
   CHAR filename     [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstr       [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstring    [MAX_INI_INFO_SIZE]= {0};
   CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
   INT  ret_val = true;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
   rc = GetPrivateProfileString(
             "CUPMIDSECTION",     /* points to section name       */
             "CUP_MID_INDICATOR",  /* points to key name           */
             "0",                       		/* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   strncpy( CUP_merchant_indicator, tmpstring, CUP_MID_INDICATOR_LEN );
   if(strlen(CUP_merchant_indicator)==0)
   {
	   strncpy( CUP_merchant_indicator,
			    CUP_MID_INDICATOR_DEFAULT,
			    CUP_MID_INDICATOR_LEN);// Default Value
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         Read_DCC_Terminal_Starting_Prefix
 *
 *  DESCRIPTION:  This function reads information from the tf ini file.
 *
 *  INPUTS:       None
 *  OUTPUTS:      err_buf - Text message explaining why this function failed
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT Read_DCC_Terminal_Starting_Prefix( void )
{
	DWORD rc = 0;
	CHAR filename     [MAX_FILENAME_SIZE] = {0};
	CHAR tmpstr       [MAX_FILENAME_SIZE] = {0};
	CHAR tmpstring    [MAX_INI_INFO_SIZE] = {0};
	CHAR ini_info     [MAX_INI_INFO_SIZE] = {0};
	INT  ret_val = true;

	/* Determine directory and filename of the .ini file. */
	memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
	GetPinnacleConfigDirectory(tmpstr);
	sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
	rc = GetPrivateProfileString(
				"DCC_TERMINAL_STARTING_PREFIX",    /* points to section name       */
				"DCC_TERMINAL_STARTING_PREFIX_1",  /* points to key name           */
				"0",                       /* points to default string     */
				ini_info,                      /* points to destination buffer */
				MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
				filename                       /* points to ini filename       */
				);

	/* Convert entry from a string into a number. */
	memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
	strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
	strncpy( DCC_Termina_Starting_Prefix_1, tmpstring, 2 );
	if(strlen(DCC_Termina_Starting_Prefix_1)==0)
	{
		strncpy( DCC_Termina_Starting_Prefix_1,
				DCC_TERMINAL_STARTING_PREFIX_1,
				RESPONSE_CODE_LEN);;// Default Value
	}

	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
	rc = GetPrivateProfileString(
				"DCC_TERMINAL_STARTING_PREFIX",    /* points to section name       */
				"DCC_TERMINAL_STARTING_PREFIX_2",  /* points to key name           */
				"0",                       /* points to default string     */
				ini_info,                      /* points to destination buffer */
				MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
				filename                       /* points to ini filename       */
				);

	/* Convert entry from a string into a number. */
	memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
	strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
	strncpy( DCC_Termina_Starting_Prefix_2, tmpstring, 2 );
	if(strlen(DCC_Termina_Starting_Prefix_2)==0)
	{
		strncpy( DCC_Termina_Starting_Prefix_2,
				 DCC_TERMINAL_STARTING_PREFIX_2,
				 RESPONSE_CODE_LEN);;// Default Value
	}

	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
	rc = GetPrivateProfileString(
				"DCC_TERMINAL_STARTING_PREFIX",    /* points to section name       */
				"DCC_TERMINAL_STARTING_PREFIX_3",  /* points to key name           */
				"0",                       /* points to default string     */
				ini_info,                      /* points to destination buffer */
				MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
				filename                       /* points to ini filename       */
				);

	/* Convert entry from a string into a number. */
	memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
	strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
	strncpy( DCC_Termina_Starting_Prefix_3, tmpstring, 2 );
	if(strlen(DCC_Termina_Starting_Prefix_3)==0)
	{
		strncpy( DCC_Termina_Starting_Prefix_3,
				 DCC_TERMINAL_STARTING_PREFIX_3,
				 RESPONSE_CODE_LEN);;// Default Value
	}

	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
	rc = GetPrivateProfileString(
				"DCC_TERMINAL_STARTING_PREFIX",    /* points to section name       */
				"DCC_TERMINAL_STARTING_PREFIX_4",  /* points to key name           */
				"0",                       /* points to default string     */
				ini_info,                      /* points to destination buffer */
				MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
				filename                       /* points to ini filename       */
				);

	/* Convert entry from a string into a number. */
	memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
	strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
	strncpy( DCC_Termina_Starting_Prefix_4, tmpstring, 2 );
	if(strlen(DCC_Termina_Starting_Prefix_4)==0)
	{
		strncpy( DCC_Termina_Starting_Prefix_4,
				 DCC_TERMINAL_STARTING_PREFIX_4,
				 RESPONSE_CODE_LEN);;// Default Value
	}

	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
	rc = GetPrivateProfileString(
				"DCC_TERMINAL_STARTING_PREFIX",    /* points to section name       */
				"DCC_TERMINAL_STARTING_PREFIX_5",  /* points to key name           */
				"0",                       /* points to default string     */
				ini_info,                      /* points to destination buffer */
				MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
				filename                       /* points to ini filename       */
				);

	/* Convert entry from a string into a number. */
	memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
	strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
	strncpy( DCC_Termina_Starting_Prefix_5, tmpstring, 2 );
	if(strlen(DCC_Termina_Starting_Prefix_5)==0)
	{
		strncpy( DCC_Termina_Starting_Prefix_5,
				DCC_TERMINAL_STARTING_PREFIX_5,
				RESPONSE_CODE_LEN);;// Default Value
	}

	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
	rc = GetPrivateProfileString(
				"DCC_TERMINAL_STARTING_PREFIX",    /* points to section name       */
				"DCC_TERMINAL_STARTING_PREFIX_6",  /* points to key name           */
				"0",                       /* points to default string     */
				ini_info,                      /* points to destination buffer */
				MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
				filename                       /* points to ini filename       */
				);

	/* Convert entry from a string into a number. */
	memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
	strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
	strncpy( DCC_Termina_Starting_Prefix_6, tmpstring, 2 );
	if(strlen(DCC_Termina_Starting_Prefix_6)==0)
	{
		strncpy( DCC_Termina_Starting_Prefix_6,
				 DCC_TERMINAL_STARTING_PREFIX_6,
				 RESPONSE_CODE_LEN);;// Default Value
	}

	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
	rc = GetPrivateProfileString(
				"DCC_TERMINAL_STARTING_PREFIX",    /* points to section name       */
				"DCC_TERMINAL_STARTING_PREFIX_7",  /* points to key name           */
				"0",                       /* points to default string     */
				ini_info,                      /* points to destination buffer */
				MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
				filename                       /* points to ini filename       */
				);

	/* Convert entry from a string into a number. */
	memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
	strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
	strncpy( DCC_Termina_Starting_Prefix_7, tmpstring, 2 );
	if(strlen(DCC_Termina_Starting_Prefix_7)==0)
	{
		strncpy( DCC_Termina_Starting_Prefix_7,
				DCC_TERMINAL_STARTING_PREFIX_7,
				RESPONSE_CODE_LEN);;// Default Value
	}

	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
	rc = GetPrivateProfileString(
				"DCC_TERMINAL_STARTING_PREFIX",    /* points to section name       */
				"DCC_TERMINAL_STARTING_PREFIX_8",  /* points to key name           */
				"0",                       /* points to default string     */
				ini_info,                      /* points to destination buffer */
				MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
				filename                       /* points to ini filename       */
				);

	/* Convert entry from a string into a number. */
	memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
	strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
	strncpy( DCC_Termina_Starting_Prefix_8, tmpstring, 2 );
	if(strlen(DCC_Termina_Starting_Prefix_8)==0)
	{
		strncpy( DCC_Termina_Starting_Prefix_8,
				 DCC_TERMINAL_STARTING_PREFIX_8,
				 RESPONSE_CODE_LEN);;// Default Value
	}

	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
	rc = GetPrivateProfileString(
				"DCC_TERMINAL_STARTING_PREFIX",    /* points to section name       */
				"DCC_TERMINAL_STARTING_PREFIX_9",  /* points to key name           */
				"0",                       /* points to default string     */
				ini_info,                      /* points to destination buffer */
				MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
				filename                       /* points to ini filename       */
				);

	/* Convert entry from a string into a number. */
	memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
	strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
	strncpy( DCC_Termina_Starting_Prefix_9, tmpstring, 2 );
	if(strlen(DCC_Termina_Starting_Prefix_9)==0)
	{
		strncpy( DCC_Termina_Starting_Prefix_9,
				 DCC_TERMINAL_STARTING_PREFIX_9,
				 RESPONSE_CODE_LEN);;// Default Value
	}

	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
	rc = GetPrivateProfileString(
				"DCC_TERMINAL_STARTING_PREFIX",    /* points to section name       */
				"DCC_TERMINAL_STARTING_PREFIX_10",  /* points to key name           */
				"0",                       /* points to default string     */
				ini_info,                      /* points to destination buffer */
				MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
				filename                       /* points to ini filename       */
				);

	/* Convert entry from a string into a number. */
	memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
	strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
	strncpy( DCC_Termina_Starting_Prefix_10, tmpstring, 2 );
	if(strlen(DCC_Termina_Starting_Prefix_10)==0)
	{
		strncpy( DCC_Termina_Starting_Prefix_10,
				 DCC_TERMINAL_STARTING_PREFIX_10,
				 RESPONSE_CODE_LEN);;// Default Value
	}
	return( ret_val );
}



/*****************************************************************************

  Function:   MainProcessor

  Description: Initial application setup.  This is the main entry point for 
               incoming PTE messages.
  Author:  Abhishek

******************************************************************************/
void MainProcessor (void)
{
   pPTE_MSG p_msg = NULL_PTR ; 
   LONG     retCode = 0L ;
   INT      rc;
   INT      err_ctr = 0;
   CHAR     strError[512] = {0} ;
   CHAR     strTemp[512] = {0} ;
   CHAR     strAppName[512] = {0} ;
   CHAR     strInstanceName[512] = {0} ;
   CHAR     time_date[25] ={0};
   CHAR     seconds   [3] = {0};

   GetAppName (strAppName) ;
   GetXipcInstanceName (strInstanceName) ;
   sprintf( strTemp,
           "Starting the Service: %s, version %s",
            ServiceName, Version );
   dcpos_log_message( 2, 1, strTemp, "MainProcessor", 0 );
   printf( "%s\n", strTemp );

   #ifdef _DEBUG
      pteipc_trace_on(); 
   #endif

   if(!pteipc_init_multiple_instance_app (strAppName, ServiceName, strInstanceName))
   {
      strcpy (strError, "Failed to create XIPC queues");
      dcpos_log_message( 1, 1, strError,"MainProcessor",0 );

      /* signal that we are ready to shut down and quit */
      pteipc_shutdown_multiple_instance_app();
      MainProcessDone = 1;
      return;
   } 

   /* Initialize transaction statistics */
   (void)reset_stats( &Txn_Counts );
   dcpos_get_error_warning_file_name_path();
   /* Determine if transaction tracing should be turned on. */
   rc = get_trace_ini_info( strAppName, DirPath, strError );
   if ( rc == 0 )
   {
      Tracing = TRACE_ON;

      /* Open the transaction trace file. */
      memset( TraceFile, 0x00, sizeof(TraceFile) );

      pTraceFile_fp  = &TraceFile_fp;
      ppTraceFile_fp = &pTraceFile_fp;

      if (false == open_trace_file(strAppName,DirPath,ppTraceFile_fp,TraceFile))
      {
         Tracing = TRACE_OFF;
         sprintf( strError,
                 "Unable to open trace file %s. Tracing is off",
                  TraceFile );
         dcpos_log_message( 1, 1, strError,"MainProcessor",0 );
      }
      else
    	  dcpos_log_message( 1, 1, strError,"MainProcessor",0 );
   }
   else
   {
      Tracing = TRACE_OFF;
      if ( rc == -1 )
      {
         dcpos_log_message( 1, 1, strError,"MainProcessor",0 );
      }
      dcpos_log_message( 1, 1, "Tracing is turned off","MainProcessor",0 );
   }

   rc = Read_DUAL_BRAND_DEBIT_TRANSACTION_Response_code();
   rc = Read_DEBIT_MAGSTRIPE_TRANSACTION_Response_code();
   rc = Read_DEBIT_EMV_TRANSACTION_Response_code();
   rc = Read_DCC_TRANSACTION_Response_code();
   rc = Read_DINERS_TRANSACTION_Response_code();
   rc = Read_FLEET_CARD_TRANSACTION_Response_code();
   rc = Read_CUP_UPI_MAGSTRIPE_TRANSACTION_Response_code();
   rc = Read_BIN_NOT_FOUND_FOR_BIN_ROUTING_Eror_Response_code();
   rc  = Read_AMEX_TRANSACTION_ON_BIN_NII_Response_code();
   rc = dcpos_read_CUP_merchant_indicator();
   rc = Read_DCC_Terminal_Starting_Prefix();

   /* Initialize the RRN counter that ensures unique RRNs per second. */
   ptetime_get_timestamp( time_date );
   memcpy( seconds, time_date+17, 2 );
   RRNctr = atoi( seconds );

   while (!EndProcessSignalled)
   {
      /* You are blocked here waiting for a message on either app queue or control que  */
      /* if there is no message on either que for 5 seconds, the blocking call returns   */
      p_msg = pteipc_receive (application_que_name, control_que_name, 5, &retCode);

      if (p_msg != NULL_PTR)
      {
         PinnacleMsgHandler (p_msg);
         free (p_msg);
      }
      else if (retCode != QUE_ER_TIMEOUT) 
      {
         pteipc_get_errormsg( retCode, strTemp );
         sprintf(strError, "Error on pteipc_receive %s", strTemp);
         dcpos_log_message( 1, 1,strError,"MainProcessor",0 );
         /* There is a problem with the queue.  Delay 1 second
          * so as to not flood Applnk's queue and syslog.
          */
         #ifdef WIN32
            Sleep( 1000 );
         #else
            usleep( 1000000 );
         #endif

         /* Inform Operators if service's queue is no longer readable. */
         if ( ++err_ctr > 10 )
         {
            err_ctr = 0;
            sprintf( strError,
                    "SHUT DOWN %s. UNABLE TO ACCESS QUEUE!!!!",
                     ServiceName );
            dcpos_log_message( 1, 1,strError,"MainProcessor",0 );
            #ifdef WIN32
               Sleep (5000);
            #else
               usleep (5000000);
            #endif
         }
      }
   }

   pteipc_shutdown_multiple_instance_app ();
   MainProcessDone = 1;
   free (p_msg);
   return;
} /* MainProcessor */

/*****************************************************************************

  Function:   PinnacleMsgHandler
  Description: This function receives a PTE message and passes it to the 
               appropriate processing function.
  Author: Abhishek Verma
   
  Inputs:  p_msg_in - incoming PTE message
  Outputs: None
  Return values: None
******************************************************************************/
void PinnacleMsgHandler (pPTE_MSG p_msg_in )
{
   pPTE_MSG p_cur_msg;
   pPTE_MSG p_msg_out;
   BYTE msgtyp;
   CHAR buffer[100]="";
   INT      rc;
   CHAR     strError[512] = {0} ;

   BYTE retCode = 0 ;
   memset(&auth_tx, 0, sizeof auth_tx) ;

   p_cur_msg = p_msg_in ;
   p_msg_out = NULL_PTR ;

   msgtyp = ptemsg_get_msg_type( p_cur_msg) ;
   switch (ptemsg_get_msg_type( p_cur_msg))
   {
      case MT_INCOMING:
         retCode = Process_Incoming_Message_From_Terminal (p_msg_in) ;

         if (retCode != DEVICE_OK)
         {
            /* Could be ISO_PARSE_ERROR or DEVICE_ERROR or SYSTEM_ERROR */
            if (retCode == ISO_PARSE_ERROR)
            {
               /* Drop the transaction as writing to TLF01 does not
                * make any sense as auth_tx might not be populated
                */
               break ;
            }
            Send_Transaction_To_Originator () ;
         }
      break;

      case MT_TIMER_TIMEOUT:
      break;	

      case MT_SYSTEM_REQUEST:
        ptesystm_system_msg_handler (p_msg_in);
		sprintf( buffer,"Pinged -> Device Controller Service: %s, version %s",
				 ServiceName, Version );
		dcpos_log_message( 1, 1,buffer,"PinnacleMsgHandler",0 );
		break;        

      case MT_DB_REPLY:
         retCode = ptemsg_get_result_code (p_msg_in) ;
         if (retCode == PTEMSG_OK)
         {
            retCode = Process_Dataserver_OK_Message (p_msg_in) ;
         }
         else if (retCode == PTEMSG_NOT_FOUND)
         {
            retCode = Process_Dataserver_NotFound_Message (p_msg_in) ;
         }
         else 
         {
            retCode = Process_Dataserver_ERROR_Message (p_msg_in) ;
         }

      break;

     case MT_GET_STATS:
         (void)send_transaction_statistics( p_msg_in );
      break;

      case MT_RESET_STATS:
         (void)reset_transaction_statistics( p_msg_in );
      break;

      case MT_SYSTEM_REPLY: 
      break;

      case MT_DB_REQUEST:
      break;

      case MT_TIMER_REPLY:
      break;

      default:
         dcpos_log_message( 2, 1, "unknown msg_type received", "PinnacleMsgHandler", 0 );
      break;
   }  /* switch msg_type */
} /* PinnacleMsgHandler */
/******************************  END OF FILE  ****************************************************/
