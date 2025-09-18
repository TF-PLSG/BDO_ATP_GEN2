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
* APPLICATION: DCPIST
*
* AUTHOR:  Unknown
*      
**************************************************************************************************/

#include <stdio.h>
#include "pteipc.h"
#include "global_definitions.h"
#include "ntutils.h"
#include "ptesystm.h"
#include "message_handler.h"
#include "genutil.h"
#include "txutils.h"
#include "txtrace.h"
#include "diskio.h"
#include "service_functions.h"
#include "process_incoming_msg.h"
#include "process_outgoing_msg.h"
#include "database_response.h"
#include "general_utilities.h"

AUTH_TX auth_tx ;


CHAR    devds_que_name[]       	= "dvmpdsA" ;
CHAR    trands_que_name []    	= "trandsA" ;
CHAR    updateds_que_name []   	= "updatdsA" ;
CHAR    mrads_que_name[]       	= "mradsA" ;
CHAR    authorizer_que_name [] 	= "txcntlA" ;
CHAR    ncvisa_que_name []     	= "ncvisaA" ;
CHAR    dcpimp_que_name [] 		= "dcpimpA" ;
CHAR    nciftr_que_name [] 		= "nciftrA" ;
CHAR    visa1_que_name []      	= "visa1A" ;
CHAR    oprmon_que_name []     	= "oprmonI" ;
CHAR    mracon_que_name [MAX_QUE_NAME_SIZE+1] = "mraconA" ;
CHAR    Version[]               = "ATP_11.12.0";

CHAR    nciftr_module_name [] 	= "nciftr";

											
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
INT nMra_enable_flag = 0;
PRIVATE_CCF03P_STRUCT private_ccf03p_struct ;
CHAR faisnd_que_name            [MAX_QUE_NAME_SIZE+1] = "faisndA" ;

#ifdef FALCON_CONTROLLER
CHAR falcntl_que_name           [MAX_QUE_NAME_SIZE+1] = "falcntlA" ;
#endif
INT nFalconSendFlag  = 0; 

INT Nmra_send_flag = 0;
INT Mracon_Log_Enable_Flag = 0;
INT Falcon_RT_Flag = 0;
INT Falcon_RT_EXT10_Flag = 0;

CHAR  CUP_merchant_indicator[8]={0};
CHAR  CUP_MP_Bin_range_1[11]={0};
CHAR  CUP_MP_Bin_range_2[11]={0};
CHAR  CUP_MP_Bin_range_3[11]={0};
CHAR  CUP_MP_Bin_range_4[11]={0};
CHAR  CUP_MP_Bin_range_5[11]={0};

CHAR  ATP_pos_default_response_code[3]= {0};
CHAR  pos_response_code[3]= {0};
CHAR  pos_res_txt[40]	  = {0};

INT   CUP_MP_Bin_range_len_1 = 0;
INT   CUP_MP_Bin_range_len_2 = 0;
INT   CUP_MP_Bin_range_len_3 = 0;
INT   CUP_MP_Bin_range_len_4 = 0;
INT   CUP_MP_Bin_range_len_5 = 0;

CHAR  dcpimp_Erro_warning_Filename[256] = {0};
CHAR  dcpimp_module_error_warning_file_name[256] = {0};
CHAR  dcpimp_error_warning_file_path[256] = {0};


char mp_rc_for_fai[3]={0};
char plfc_rc_for_fai[3]={0};
char mp_rc_for_TO[3]={0};
char plfc_rc_for_TO[3]={0};

CHAR  FRT_Decline_RC_MP[3]={0};

#ifdef HCTS2
BYTE card_activation_flag;
BYTE card_updating_status [5]      =  {0};
BYTE card_current_status [51][5]   =  {0};
INT  total_card_current_status     =  0;
BYTE mp_in_vertexon_codes[101][4]  =  {0};
BYTE pl_ft_vertexon_codes[101][4]  =  {0};
void dcpimp_read_atp_card_activation_parameters();
void dcpimp_read_vertexon_codes();
#endif

/*****************************************************************************

  Function:   end_process

  Description: This function will shutdown the terminal handler.
  Author: 
      unknown
  Inputs:
      None
  Outputs:
      None
  Return values:
      None
  Modified by:
      BGable      6/30/98
******************************************************************************/
void EndProcess(void)
{
   CHAR Buffer[100] = "";

#ifdef _DEBUG
    pteipc_trace_off(); 
#endif

   if ( Tracing == TRACE_ON )
   {
      if ( false == close_trace_file( ppTraceFile_fp ))
      {
         sprintf( Buffer, "Unable to close trace file %s", TraceFile );
         dcpimp_log_message( 1, 2, Buffer," EndProcess ", 0 );
      }
   }

   sprintf( Buffer, "Shutting down the %s Service, version %s",
            ServiceName, Version );
   dcpimp_log_message( 2, 1, Buffer, " EndProcess ",0 );
   strcat( Buffer, "\n" );
   PRINT( Buffer );

} /* EndProcess */

/******************************************************************************
 *
 *  NAME:         read_mra_send_flag
 *
 *  DESCRIPTION:  This function reads information from the tf ini file.
 *                This flag information is stored in global variables for use of 
 *					MRA DB Send
 *                .
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      err_buf - Text message explaining why this function failed
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT read_mra_send_flag( pCHAR err_buf )
{
   DWORD rc = 0;
   CHAR sectionname  [MAX_APPNAME_SIZE] = {0};
   CHAR filename     [MAX_FILENAME_SIZE] = {0};
   CHAR tmpstr       [MAX_FILENAME_SIZE] = {0};
   CHAR tmpstring    [MAX_INI_INFO_SIZE] = {0};
   CHAR ini_info     [MAX_INI_INFO_SIZE] = {0};
   INT  ret_val = true;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   strcpy( sectionname, "PREPAID" );
   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
   rc = GetPrivateProfileString(
             "MRA_SECTION",                   /* points to section name       */
            "SEND_TO_MRADB",  /* points to key name           */
            "0",                       /* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   nMra_enable_flag = atoi(tmpstring);
	

   return( ret_val );
}
/*****************************************************************************************************/
/*	This function helps to read latest falcon configuration from Config File and update FAI
	parameters to process transaction the parameters. This function helps to enable or disable
	the Falcon RealTime Feature */
/******************************************************************************************************/

void dcpimp_read_falconrt_flag_for_transactions()
{
	   DWORD rc;
	   CHAR  filename   [80] = {0};
	   CHAR  tmpstr     [80] = {0};
	   CHAR  reload_falcon_flag   [2]  = {0};

	   /* Get path to the tf.ini file. */
	   memset( tmpstr, 0x00, sizeof(tmpstr) );
	   GetPinnacleConfigDirectory(tmpstr);
	   sprintf(filename, "%s%s", tmpstr, "tf.ini" );
	   /*Read path for creating file in order to log db statics and db oracle error messages */

	   rc = GetPrivateProfileString(  "FALCON_RT_FLAG",   /* points to section name         */
									  "DCPIMP_FALCONRT_FLAG",       				/* points to key name             */
									   "0",  						/* Default string                 */
									   reload_falcon_flag,              		/* points to destination buffer   */
									   sizeof(reload_falcon_flag)-1,   	 	/* size of destination buffer     */
									   filename                  	/* points to ini filename         */
							 	   );
	   Falcon_RT_Flag = atoi(reload_falcon_flag);

	   memset( reload_falcon_flag, 0x00, sizeof(reload_falcon_flag) );
}

/******************************************************************************************************/
/*	This function helps to read latest falcon configuration from Config File and update FAI
	parameters to process transaction the parameters. This function helps to enable or disable
	the EXT10 flag for Falcon RealTime Feature
*/

void dcpimp_read_falconrt_ext10_flag_for_transactions()
{
	   DWORD rc;
	   CHAR  filename   [80] = {0};
	   CHAR  tmpstr     [80] = {0};
	   CHAR  reload_falcon_flag   [2]  = {0};

	   /* Get path to the tf.ini file. */
	   memset( tmpstr, 0x00, sizeof(tmpstr) );
	   GetPinnacleConfigDirectory(tmpstr);
	   sprintf(filename, "%s%s", tmpstr, "tf.ini" );
	   /*Read path for creating file in order to log db statics and db oracle error messages */

	   rc = GetPrivateProfileString(  "FALCON_RT_FLAG",   /* points to section name         */
									  "DCPIMP_FALCONRT_EXT10_FLAG",       				/* points to key name             */
									   "0",  						/* Default string                 */
									   reload_falcon_flag,              		/* points to destination buffer   */
									   sizeof(reload_falcon_flag)-1,   	 	/* size of destination buffer     */
									   filename                  	/* points to ini filename         */
							 	   );
	   Falcon_RT_EXT10_Flag = atoi(reload_falcon_flag);

	   memset( reload_falcon_flag, 0x00, sizeof(reload_falcon_flag) );

	   if (Falcon_RT_EXT10_Flag == 1)
		   dcpimp_read_vertexon_codes_for_ext10_falconrt();
}

/******************************************************************************************************/

/******************************************************************************
 *
 *  NAME:         get_MRACON_state_from_ini
 *
 *  DESCRIPTION:  This function reads information whether falcon is enabled.
 *
 *  INPUTS:       None
 *
 *  RTRN VALUE:   1 for enabled; 0 for disabled
 *
 *  AUTHOR:       Phani  - ThoughtFocus
 *
 ******************************************************************************/
INT get_MRACON_state_from_ini( void )
{
	#define MAX_INI_INFO_SIZE   20
   #define MAX_FILE_NAME_SZIE 128

   CHAR filename     [MAX_FILE_NAME_SZIE];
   CHAR tmpstr       [MAX_FILE_NAME_SZIE];

   CHAR tmpstring    [MAX_INI_INFO_SIZE];
   CHAR ini_info     [MAX_INI_INFO_SIZE];

   INT nARState_temp   = 0;
   INT nARState_retval = 1;
   INT Nmra_falg = 0;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILE_NAME_SZIE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   /* See if send is turned on */
   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );

   GetPrivateProfileString(
             "MRA_SECTION",                   /* points to section name       */
             "MRA_FAI_SEND_FLAG",                         /* points to key name           */
             "0",                            /* points to default string     */
             ini_info,                       /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,          /* size of destination buffer   */
             filename                        /* points to ini filename       */
             );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );

   Nmra_send_flag = atoi(tmpstring);
    /* See if send is turned on */
   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );

   GetPrivateProfileString(
             "MRA_SECTION",                   /* points to section name       */
             "SEND_TO_MRADB",                         /* points to key name           */
             "0",                            /* points to default string     */
             ini_info,                       /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,          /* size of destination buffer   */
             filename                        /* points to ini filename       */
             );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );

   Nmra_falg = atoi(tmpstring);


   /* See if send is turned on */
   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );

   GetPrivateProfileString(
             "MRA_SECTION",                   /* points to section name       */
             "MRACON_LOG_ENABLE_FLAG",       /* points to key name           */
             "0",                            /* points to default string     */
             ini_info,                       /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,          /* size of destination buffer   */
             filename                        /* points to ini filename       */
             );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );

   Mracon_Log_Enable_Flag = atoi(tmpstring);




   return(Nmra_send_flag);
}
/******************************************************************************



/******************************************************************************
 *
 *  NAME:         read_tf_ini
 *
 *  DESCRIPTION:  This function reads information from the tf ini file.
 *                This information is stored in global variables for use
 *                by any network controller function.  This information
 *                allows customers to have flexibility with some of the
 *                program logic.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      err_buf - Text message explaining why this function failed
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT read_tf_ini( pCHAR err_buf )
{
   DWORD rc = 0;
   CHAR sectionname  [MAX_APPNAME_SIZE] = {0};
   CHAR filename     [MAX_FILENAME_SIZE] = {0};
   CHAR tmpstr       [MAX_FILENAME_SIZE] = {0};
   CHAR tmpstring    [MAX_INI_INFO_SIZE] = {0};
   CHAR ini_info     [MAX_INI_INFO_SIZE] = {0};
   INT  ret_val = true;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   strcpy( sectionname, "PREPAID" );
   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
   rc = GetPrivateProfileString(
             sectionname,                   /* points to section name       */
            "Void_Reload_Timeout",  /* points to key name           */
            "600",                       /* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   void_reload_timeout = atoi(tmpstring);
	
   if (void_reload_timeout <= 0)
	   void_reload_timeout = 600;

   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         dcpimp_read_CUP_merchant_indicator
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
INT dcpimp_read_CUP_merchant_indicator( void )
{
   DWORD rc;
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
 *  NAME:         dcpimp_read_CUP_MP_Bin_range_len_1
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
INT dcpimp_read_CUP_MP_Bin_range_len_1( void )
{
   DWORD rc;
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
             "CUPMPBINDEFINATION",     /* points to section name       */
             "CUP_MP_BIN_RANGE_LEN_1",  /* points to key name           */
             "0",                       		/* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   CUP_MP_Bin_range_len_1= atoi(tmpstring);
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         dcpimp_read_CUP_MP_Bin_range_len_2
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
INT dcpimp_read_CUP_MP_Bin_range_len_2( void )
{
   DWORD rc;
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
             "CUPMPBINDEFINATION",     /* points to section name       */
             "CUP_MP_BIN_RANGE_LEN_2",  /* points to key name           */
             "0",                       		/* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   CUP_MP_Bin_range_len_2= atoi(tmpstring);
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         dcpimp_read_CUP_MP_Bin_range_len_3
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
INT dcpimp_read_CUP_MP_Bin_range_len_3( void )
{
   DWORD rc;
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
             "CUPMPBINDEFINATION",     /* points to section name       */
             "CUP_MP_BIN_RANGE_LEN_3",  /* points to key name           */
             "0",                       		/* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   CUP_MP_Bin_range_len_3= atoi(tmpstring);
   return( ret_val );
}

DWORD dcpimp_read_RC_for_fai()
{
	   DWORD rc;
	   CHAR sectionname  [MAX_APPNAME_SIZE] = {0};
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
	   			 "DCPIMP_DECLINE_RC_FOR_FAI_DECISION_CODE",                   /* points to section name       */
	   			"MP_RC_FOR_FAI",  			/* points to key name           */
	   			"19",                       /* points to default string     */
	   			 ini_info,                      /* points to destination buffer */
	   			 MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
	   			 filename                       /* points to ini filename       */
	   	);

	   	strncpy(mp_rc_for_fai,ini_info,2);

	   	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
	   	rc = GetPrivateProfileString(
	   			"DCPIMP_DECLINE_RC_FOR_FAI_DECISION_CODE",                   /* points to section name       */
	   		   "PLFC_RC_FOR_FAI",  /* points to key name           */
	   		   "19",                       /* points to default string     */
	   			ini_info,                      /* points to destination buffer */
	   			MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
	   			filename                       /* points to ini filename       */
	   	);

	   	strncpy(plfc_rc_for_fai,ini_info,2);

	   	return rc;
}

DWORD dcpimp_read_TO_RC_for_mp_plfc()
{
	   DWORD rc;
	   CHAR sectionname  [MAX_APPNAME_SIZE] = {0};
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
	   			 "DCPIMP_TIMEOUT_RC",                   /* points to section name       */
	   			"MP_RC_FOR_FAI",  			/* points to key name           */
	   			"19",                       /* points to default string     */
	   			 ini_info,                      /* points to destination buffer */
	   			 MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
	   			 filename                       /* points to ini filename       */
	   	);

	   	strncpy(mp_rc_for_TO,ini_info,2);

	   	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
	   	rc = GetPrivateProfileString(
	   			"DCPIMP_TIMEOUT_RC",                   /* points to section name       */
	   		   "PLFC_RC_FOR_FAI",  /* points to key name           */
	   		   "19",                       /* points to default string     */
	   			ini_info,                      /* points to destination buffer */
	   			MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
	   			filename                       /* points to ini filename       */
	   	);

	   	strncpy(plfc_rc_for_TO,ini_info,2);

	   	return rc;
}

DWORD dcpimp_write_falcon_RT_falg_into_inifile(pCHAR fai_rt_value)
{
	   DWORD rc;
	   CHAR sectionname  [MAX_APPNAME_SIZE] = {0};
	   CHAR filename     [MAX_FILENAME_SIZE]= {0};
	   CHAR tmpstr       [MAX_FILENAME_SIZE]= {0};
	   CHAR tmpstring    [MAX_INI_INFO_SIZE]= {0};
	   CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
	   INT  ret_val = true;
	 GetAscendentConfigDirectory(tmpstr);
	 sprintf(filename, "%stf.ini", tmpstr);

	 rc= WritePrivateProfileString(
		  "FALCON_RT_FLAG",	/* pointer to section name*/
		  "DCPIMP_FALCONRT_FLAG",	/* pointer to key name*/
		  fai_rt_value ,	/* pointer to string to add*/
		  filename			/* pointer to initialization filename*/
	 );

	 sprintf( tmpstr, "Falcon RT Flag value changed for processing Transactions:%s",fai_rt_value );
	 dcpimp_log_message( 3, 1,tmpstr, "PinnacleMsgHandler", 0 );
	 return rc;
}

DWORD dcpimp_write_falconRT_EXT10_flag_into_inifile(pCHAR fai_ext_value)
{
	   DWORD rc;
	   CHAR sectionname  [MAX_APPNAME_SIZE] = {0};
	   CHAR filename     [MAX_FILENAME_SIZE]= {0};
	   CHAR tmpstr       [MAX_FILENAME_SIZE]= {0};
	   CHAR tmpstring    [MAX_INI_INFO_SIZE]= {0};
	   CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
	   INT  ret_val = true;
	 GetAscendentConfigDirectory(tmpstr);
	 sprintf(filename, "%stf.ini", tmpstr);

	 rc= WritePrivateProfileString(
		  "FALCON_RT_FLAG",	/* pointer to section name*/
		  "DCPIMP_FALCONRT_EXT10_FLAG",	/* pointer to key name*/
		  fai_ext_value ,	/* pointer to string to add*/
		  filename			/* pointer to initialization filename*/
	 );

	 sprintf( tmpstr, "FalconRT EXT10 Flag value changed for processing Transactions:%s",fai_ext_value );
	 dcpimp_log_message( 3, 1,tmpstr, "PinnacleMsgHandler", 0 );
	 return rc;
}

/******************************************************************************
 *
 *  NAME:         dcpimp_read_CUP_MP_Bin_range_len_4
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
INT dcpimp_read_CUP_MP_Bin_range_len_4( void )
{
   DWORD rc;
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
             "CUPMPBINDEFINATION",     /* points to section name       */
             "CUP_MP_BIN_RANGE_LEN_4",  /* points to key name           */
             "0",                       		/* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   CUP_MP_Bin_range_len_4= atoi(tmpstring);
   return( ret_val );
}
/******************************************************************************
 *
 *  NAME:         dcpimp_read_CUP_MP_Bin_range_len_5
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
INT dcpimp_read_CUP_MP_Bin_range_len_5( void )
{
   DWORD rc;
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
             "CUPMPBINDEFINATION",     /* points to section name       */
             "CUP_MP_BIN_RANGE_LEN_5",  /* points to key name           */
             "0",                       		/* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   CUP_MP_Bin_range_len_5= atoi(tmpstring);
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         dcpimp_read_CUP_MP_Bin_range_1_indicator
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
INT dcpimp_read_CUP_MP_Bin_range_1_indicator( void )
{
   DWORD rc;
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
             "CUPMPBINDEFINATION",     /* points to section name       */
             "CUP_MP_BIN_RANGE_1",  /* points to key name           */
             "0",                       		/* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   strncpy( CUP_MP_Bin_range_1, tmpstring, CUP_MP_Bin_range_len_1 );
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         dcpimp_read_CUP_MP_Bin_range_2_indicator
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
INT dcpimp_read_CUP_MP_Bin_range_2_indicator( void )
{
   DWORD rc;
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
             "CUPMPBINDEFINATION",     /* points to section name       */
             "CUP_MP_BIN_RANGE_3",  /* points to key name           */
             "0",                       		/* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   strncpy( CUP_MP_Bin_range_2, tmpstring, CUP_MP_Bin_range_len_2 );
   return( ret_val );
}
/******************************************************************************
 *
 *  NAME:         dcpimp_read_CUP_MP_Bin_range_3_indicator
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
INT dcpimp_read_CUP_MP_Bin_range_3_indicator( void )
{
   DWORD rc;
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
             "CUPMPBINDEFINATION",     /* points to section name       */
             "CUP_MP_BIN_RANGE_3",  /* points to key name           */
             "0",                       		/* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   strncpy( CUP_MP_Bin_range_3, tmpstring, CUP_MP_Bin_range_len_3 );
   return( ret_val );
}
/******************************************************************************
 *
 *  NAME:         dcpimp_read_CUP_MP_Bin_range_4_indicator
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
INT dcpimp_read_CUP_MP_Bin_range_4_indicator( void )
{
   DWORD rc;
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
             "CUPMPBINDEFINATION",     /* points to section name       */
             "CUP_MP_BIN_RANGE_4",  /* points to key name           */
             "0",                       		/* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   strncpy( CUP_MP_Bin_range_4, tmpstring, CUP_MP_Bin_range_len_4 );
   return( ret_val );
}
/******************************************************************************
 *
 *  NAME:         dcpimp_read_CUP_MP_Bin_range_5_indicator
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
INT dcpimp_read_CUP_MP_Bin_range_5_indicator( void )
{
   DWORD rc;
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
             "CUPMPBINDEFINATION",     /* points to section name       */
             "CUP_MP_BIN_RANGE_5",  /* points to key name           */
             "0",                       		/* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   strncpy( CUP_MP_Bin_range_5, tmpstring, CUP_MP_Bin_range_len_5 );
   return( ret_val );
}

INT dcpimp_read_FRT_Decline_RC_MP( void )
{
   DWORD rc;
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
             "DCPIMP_DECLINE_RC_FOR_FAI_DECISION_CODE",     /* points to section name       */
             "DCPIMP_DECLINE_RC_FOR_FAI_DECISION_CODE_MP",  /* points to key name           */
             "01",                       		/* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   strncpy( FRT_Decline_RC_MP, tmpstring, FRT_DECLINE_RC_LEN );
   if(strlen(FRT_Decline_RC_MP)==0)
   {
	   strncpy( FRT_Decline_RC_MP,
			    FRT_DECLINE_RC_MP_DEFAULT,
				FRT_DECLINE_RC_LEN);// Default Value
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         get_falcon_state_from_ini
 *
 *  DESCRIPTION:  This function reads information whether falcon is enabled.
 *
 *  INPUTS:       None
 *
 *  RTRN VALUE:   1 for enabled; 0 for disabled
 *
 *  AUTHOR:       Abhishek
 *
 ******************************************************************************/
INT get_falcon_state_from_ini( void )
{
   CHAR filename     [MAX_FILE_NAME_SZIE];
   CHAR tmpstr       [MAX_FILE_NAME_SZIE];

   CHAR tmpstring    [MAX_INI_INFO_SIZE];
   CHAR ini_info     [MAX_INI_INFO_SIZE];

   INT nARState_temp   = 0;
   INT nARState_retval = 1;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILE_NAME_SZIE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   /* See if send is turned on */
   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );

   GetPrivateProfileString(
             "FALCON_SECTION",                   /* points to section name       */
             "SEND_TO_FALCON",                         /* points to key name           */
             "0",                            /* points to default string     */
             ini_info,                       /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,          /* size of destination buffer   */
             filename                        /* points to ini filename       */
             );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );

   nFalconSendFlag = atoi(tmpstring);
   return(nFalconSendFlag);
}

/******************************************************************************
 *
 *  NAME:         dcpimp_read_POS_default_system_response_code
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
INT dcpimp_read_POS_default_system_response_code( void )
{
   DWORD rc;
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
			 "ERROR_RESPONSE_CODE",     		/* points to section name       */
			 "ATP_POS_DEFAULT_RESPONSE_CODE",  	/* points to key name           */
			 "0",                       		/* points to default string     */
			 ini_info,                      	/* points to destination buffer */
			 MAX_INI_INFO_SIZE - 1,         	/* size of destination buffer   */
			 filename                       	/* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset ( tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   strncpy( ATP_pos_default_response_code, tmpstring, ISO_RESPONSE_CODE_LEN );
   if(strlen(ATP_pos_default_response_code)==0)
   {
	   strncpy( ATP_pos_default_response_code,
			   	SYSTEM_MALFUNCTION,
				ISO_RESPONSE_CODE_LEN);// Default Value
   }
   return( ret_val );
}

INT dcpimp_read_MID_capping_response_code( void )
{
   DWORD rc;
   CHAR filename     [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstr       [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstring    [MAX_INI_INFO_SIZE]= {0};
   CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
   CHAR tmp_res_txt  [MAX_RES_TXT_SIZE] = {0};
   INT  ret_val = true;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
   rc = GetPrivateProfileString(
			 "MID_RESPONSE_CODE",     		/* points to section name       */
			 "ATP_POS_RESPONSE_CODE",  	/* points to key name           */
			 "0",                       		/* points to default string     */
			 ini_info,                      	/* points to destination buffer */
			 MAX_INI_INFO_SIZE - 1,         	/* size of destination buffer   */
			 filename                       	/* points to ini filename       */
   );

   memset ( tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   strncpy( pos_response_code, tmpstring, ISO_RESPONSE_CODE_LEN );
   if(strlen(pos_response_code)==0)
   {
	   strncpy( pos_response_code,
			   	SYSTEM_MALFUNCTION,
				ISO_RESPONSE_CODE_LEN);// Default Value
   }

   memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   memset( tmp_res_txt, 0x00, MAX_RES_TXT_SIZE );
   rc = GetPrivateProfileString(
			 "MID_RESPONSE_CODE",     		/* points to section name       */
			 "RESPONSE_TEXT",  	/* points to key name           */
			 "0",                       		/* points to default string     */
			 tmp_res_txt,                      	/* points to destination buffer */
			 MAX_RES_TXT_SIZE - 1,         	/* size of destination buffer   */
			 filename                       	/* points to ini filename       */
   );

   strncpy( pos_res_txt, tmp_res_txt, MAX_RES_TXT_SIZE-1);


   return( ret_val );
}

#ifdef HCTS2
void dcpimp_read_atp_card_activation_parameters()
{
    int ind, index, statusIndex;
	DWORD rc;
    CHAR statusBuffer[256] = {0};
    CHAR dataBuffer[51] = {0};
    CHAR filename  [MAX_FILENAME_SIZE] = {0};
    CHAR tmpstr    [MAX_FILENAME_SIZE] = {0};
    CHAR tmpstring [MAX_INI_INFO_SIZE] = {0};
    CHAR ini_info  [MAX_INI_INFO_SIZE] = {0};

    card_activation_flag='\0';
    memset(card_updating_status, 0x00, sizeof(card_updating_status));
    memset(card_current_status,  0x00, sizeof(card_current_status));

    GetPinnacleConfigDirectory(tmpstr);
    sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

    rc = GetPrivateProfileString(
                "ATP_CARD_ACTIVATION",
                "CARD_ACTIVATION",
                "Y",
                ini_info,
                MAX_INI_INFO_SIZE - 1,
                filename);

	if(rc != 1)
	{
		dcpimp_log_message(2, 2, "CARD_ACTIVATION not read", "dcpimp_read_atp_card_activation_parameters", 0);
	}

	card_activation_flag = ini_info[0];
	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );


	rc = GetPrivateProfileStringSpaces(
	                "ATP_CARD_ACTIVATION",
	                "UPDATING_STATUS",
	                "  ",
	                ini_info,
	                MAX_INI_INFO_SIZE - 1,
	                filename);

	if(rc != 1)
	{
		dcpimp_log_message(2, 2, "UPDATING_STATUS not read", "dcpimp_read_atp_card_activation_parameters", 0);
	}

	strncpy(card_updating_status, ini_info, 2);
	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );


	rc = GetPrivateProfileString(
	                "ATP_CARD_ACTIVATION",
	                "CURRENT_STATUS",
	                "",
	                ini_info,
	                MAX_INI_INFO_SIZE - 1,
	                filename);

	if(rc != 1)
	{
		dcpimp_log_message(2, 2, "CURRENT_STATUS not read", "dcpimp_read_atp_card_activation_parameters", 0);
	}

	strncpy(statusBuffer, ini_info, strlen(ini_info));
	memset(dataBuffer, 0x00, sizeof(dataBuffer));
    ind = 0, index = 0; statusIndex = 0;
    if( card_activation_flag == 'Y' &&
        strlen(statusBuffer) > 0    &&
		statusBuffer[(strlen(statusBuffer)-1)] == ';'
	  )
    {
		while(statusBuffer[index] != ';')
		{
			if(statusBuffer[index] != ',')
			{
				card_current_status[statusIndex][ind] = statusBuffer[index];
				ind++;
			}
			else
			{
				statusIndex++;
				ind = 0;
			}
			index++;
		}
		total_card_current_status = statusIndex+1;
    }
    else
    {
    	dcpimp_log_message(2, 2, "CURRENT_STATUS contains invalid data", "dcpimp_read_atp_card_activation_parameters", 0);
    }

    if( card_activation_flag == 'Y')
    	dcpimp_read_vertexon_codes();
}

void dcpimp_read_vertexon_codes()
{
	#define MAX_INI_INFO_SIZE 256

    int ind, index, codeIndex;
	DWORD rc;
	CHAR codesCount[3] = {0};
    CHAR codesBuffer[256] = {0};
    CHAR filename  [MAX_FILENAME_SIZE] = {0};
    CHAR tmpstr    [MAX_FILENAME_SIZE] = {0};
    CHAR tmpstring [MAX_INI_INFO_SIZE] = {0};
    CHAR ini_info  [MAX_INI_INFO_SIZE] = {0};

    memset(mp_in_vertexon_codes, 0x00, sizeof(mp_in_vertexon_codes));
    memset(pl_ft_vertexon_codes, 0x00, sizeof(pl_ft_vertexon_codes));

    GetPinnacleConfigDirectory(tmpstr);
    sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

    //MP/Installment codes
    rc = GetPrivateProfileString(
                "ATP_CARD_ACTIVATION",
                "MP_IN_CODES",
                "",
                ini_info,
                MAX_INI_INFO_SIZE - 1,
                filename);

	if(rc != 1)
	{
		dcpimp_log_message(2, 2, "MP_IN_CODES not read", "dcpimp_read_vertexon_codes", 0);
	}
	memset(codesBuffer, 0x00, sizeof(codesBuffer));
	strncpy(codesBuffer, ini_info, strlen(ini_info));

	ind = 0; index = 0; codeIndex = 0;
    if( strlen(codesBuffer) > 0 && codesBuffer[(strlen(codesBuffer)-1)] == ';')
    {
		while(codesBuffer[index] != ';')
		{
			if(codesBuffer[index] != ',')
			{
				mp_in_vertexon_codes[codeIndex+1][ind] = codesBuffer[index];
				ind++;
			}
			else
			{
				codeIndex++;
				ind = 0;
			}
			index++;
		}
		sprintf(mp_in_vertexon_codes[0], "%d", codeIndex+1);
    }
    else
    {
    	dcpimp_log_message(2, 2, "MP_IN_CODES contains invalid data", "dcpimp_read_vertexon_codes", 0);
    }

    //Pvt. Label/Fleet codes
    rc = GetPrivateProfileString(
                "ATP_CARD_ACTIVATION",
                "PL_FT_CODES",
                "",
                ini_info,
                MAX_INI_INFO_SIZE - 1,
                filename);

	if(rc != 1)
	{
		dcpimp_log_message(2, 2, "PL_FT_CODES not read", "dcpimp_read_vertexon_codes", 0);
	}
	memset(codesBuffer, 0x00, sizeof(codesBuffer));
	strncpy(codesBuffer, ini_info, strlen(ini_info));

	ind = 0; index = 0; codeIndex = 0;
    if( strlen(codesBuffer) > 0 && codesBuffer[(strlen(codesBuffer)-1)] == ';')
    {
		while(codesBuffer[index] != ';')
		{
			if(codesBuffer[index] != ',')
			{
				pl_ft_vertexon_codes[codeIndex+1][ind] = codesBuffer[index];
				ind++;
			}
			else
			{
				codeIndex++;
				ind = 0;
			}
			index++;
		}
		sprintf(pl_ft_vertexon_codes[0], "%d", codeIndex+1);
    }
    else
    {
    	dcpimp_log_message(2, 2, "PL_FT_CODES contains invalid data", "dcpimp_read_vertexon_codes", 0);
    }
}
#endif
/*****************************************************************************

  Function:   MainProcessor

  Description: Initial application setup.  This is the main entry point for 
               incoming PTE messages.
  Author: 
      unknown
  Inputs:
      None
  Outputs:
      None
  Return values:
      None
  Modified by:
      BGable      6/30/98
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
   CHAR     time_date[25] = {0};
   CHAR     seconds   [3] = {0};


   GetAppName (strAppName) ;
   GetXipcInstanceName (strInstanceName) ;
   dcpimp_get_error_warning_file_name_path();
   sprintf( strTemp,
           "Starting the Service: %s, version %s",
            ServiceName, Version );
   dcpimp_log_message( 2, 1, strTemp, " MainProcessor ",0 );
   printf( "%s\n", strTemp );


   rc = read_tf_ini( strError );
   rc= read_mra_send_flag(strError);
   /* Read CUP Bin for MP details from tf.ini*/
   rc=dcpimp_read_CUP_MP_Bin_range_len_1();
   rc=dcpimp_read_CUP_MP_Bin_range_len_2();
   rc=dcpimp_read_CUP_MP_Bin_range_len_3();
   rc=dcpimp_read_CUP_MP_Bin_range_len_4();
   rc=dcpimp_read_CUP_MP_Bin_range_len_5();
   rc=dcpimp_read_CUP_MP_Bin_range_1_indicator();
   rc=dcpimp_read_CUP_MP_Bin_range_2_indicator();
   rc=dcpimp_read_CUP_MP_Bin_range_3_indicator();
   rc=dcpimp_read_CUP_MP_Bin_range_4_indicator();
   rc=dcpimp_read_CUP_MP_Bin_range_5_indicator();
  /* Read CUP MID indicator value from tf.ini*/
   rc = dcpimp_read_CUP_merchant_indicator();
   rc = get_MRACON_state_from_ini();
   rc = dcpimp_read_POS_default_system_response_code();
   rc = dcpimp_read_MID_capping_response_code();
   rc = dcpimp_read_RC_for_fai();
   rc = dcpimp_read_TO_RC_for_mp_plfc();
   dcpimp_read_falconrt_flag_for_transactions();
   dcpimp_read_falconrt_ext10_flag_for_transactions();

#ifdef HCTS2
   dcpimp_read_atp_card_activation_parameters();
#endif

   #ifdef _DEBUG
      pteipc_trace_on(); 
   #endif

   if(!pteipc_init_multiple_instance_app (strAppName, ServiceName, strInstanceName))
   {
      strcpy (strError, "Failed to create XIPC queues");
      dcpimp_log_message( 1, 3, strError," MainProcessor ", 0 );

      /* signal that we are ready to shut down and quit */
      pteipc_shutdown_multiple_instance_app();
      MainProcessDone = 1;
      return;
   } 

   /* Initialize transaction statistics */
   (void)reset_stats( &Txn_Counts );

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
         dcpimp_log_message( 1, 2, strError," MainProcessor ", 0 );
      }
      else
    	  dcpimp_log_message( 2, 1, strError," MainProcessor ", 0 );
   }
   else
   {
      Tracing = TRACE_OFF;
      if ( rc == -1 )
      {
         dcpimp_log_message( 1, 1, strError," MainProcessor ", 0 );
      }
      dcpimp_log_message( 1, 1, "Tracing is turned off"," MainProcessor ", 0 );
   }

   /* Initialize the RRN counter that ensures unique RRNs per second. */
   ptetime_get_timestamp( time_date );
   memcpy( seconds, time_date+17, 2 );
   RRNctr = atoi( seconds );

   /* Read the TF.INI file looking for section [AUTOMATIC_RESEND].
	  In that section will be an indicator to either have AUTOMATIC_RESEND   enabled or disabled. */
	  
   nFalconSendFlag	= get_falcon_state_from_ini();
   dcpimp_Get_Falcon_RT_ConfigureLink_From_DB_For_All_Brands();
   dcpimp_Get_Falcon_RT_ConfigureRule_From_DB_For_All_Brands();
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
         dcpimp_log_message( 1, 1, strError," MainProcessor ", 0 );
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
            dcpimp_log_message( 1, 1, strError," MainProcessor ", 0 );
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
  Author: 
      unknown
  Inputs:
      p_msg_in - incoming PTE message
  Outputs:
      None
  Return values:
      None
  Modified by:
      BGable      6/30/98
******************************************************************************/
void PinnacleMsgHandler (pPTE_MSG p_msg_in )
{
   pPTE_MSG p_cur_msg;
   pPTE_MSG p_msg_out;
   pBYTE          pCurrent   = NULL_PTR ;
   pPTE_MSG_DATA  p_msg_data = NULL_PTR ;
   BYTE msgtyp;
   CHAR buffer[100]="";
   INT      rc;
   CHAR     strError[512] = {0} ;

   BYTE retCode = 0 ;

   INT  dest_len = 0;

   memset(&auth_tx, 0, sizeof auth_tx) ;

   p_cur_msg = p_msg_in ;
   p_msg_out = NULL_PTR ;

   msgtyp = ptemsg_get_msg_type( p_cur_msg) ;
   switch (ptemsg_get_msg_type( p_cur_msg))
   {
      case MT_AUTH_RESPONSE:
            retCode = Process_Response_From_Authorizer (p_msg_in) ;	

            /* At this point, either the ALL_DATA update has been
             * called and DEVICE_OK has been returned OR SYSTEM_ERROR or
             * DEVICE_ERROR has been returned
             */
           if (retCode != DEVICE_OK)
              Send_Transaction_To_Originator () ;
      break;

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
		 process_encryption_flag( p_msg_in );
         if ( ST1_SYS_PING == ptemsg_get_msg_subtype1(p_msg_in) )
         {
            rc = read_tf_ini( strError );
			/* Get config parameter for mra send flag while ping request Added by praneeth*/
			rc = read_mra_send_flag( strError );//praneeth
            if ( Tracing == TRACE_ON )
            {
               /* Flush the trace file buffer when service is pinged. */
               if ( false == flush_file(ppTraceFile_fp) )
               {
                  Tracing = TRACE_OFF;
                  sprintf( buffer,
                       "%s: Unable to flush trace file buffer. Turning Trace off.",
                        ServiceName );
                  dcpimp_log_message( 1, 1, buffer," PinnacleMsgHandler ", 0 );
                  (void)close_trace_file( ppTraceFile_fp );
               }
            }

            /* When pinged, display application version on Monitor. */
            sprintf( buffer,
                    "Pinged -> Device Controller Service: %s, version %s",
                     ServiceName, Version );
            dcpimp_log_message( 1, 1, buffer," PinnacleMsgHandler ", 0 );
         }
		 if( ST1_SRVC_MRAON == ptemsg_get_msg_subtype1(p_msg_in) ) 
	     {
			 nMra_enable_flag = 1;
			send_trace_response(p_msg_in);
			sprintf( buffer,
                    "MRA enabled for %s, version %s",
                     ServiceName, Version );
            dcpimp_log_message( 1, 1, buffer," PinnacleMsgHandler ", 0 );
	     }
		 if( ST1_SRVC_MRAOFF == ptemsg_get_msg_subtype1(p_msg_in) ) 
	     {
			 nMra_enable_flag = 0;
			send_trace_response(p_msg_in);
			sprintf( buffer,
                    "MRA disabled for %s, version %s",
                     ServiceName, Version );
           dcpimp_log_message( 1, 1, buffer," PinnacleMsgHandler ", 0 );
	     }
		 if( ST1_SRVC_FALCONON == ptemsg_get_msg_subtype1(p_msg_in) ) 
	     {
			nFalconSendFlag = 1;
			send_trace_response(p_msg_in);
			sprintf( buffer, "FALCON enabled for %s, version %s", ServiceName, Version );
            dcpimp_log_message( 1, 1, buffer," PinnacleMsgHandler ", 0 );
	     }
		 if( ST1_SRVC_FALCONOFF == ptemsg_get_msg_subtype1(p_msg_in) ) 
	     {
			nFalconSendFlag = 0;
			send_trace_response(p_msg_in);
			sprintf( buffer, "FALCON disabled for %s, version %s", ServiceName, Version );
           dcpimp_log_message( 1, 1, buffer," PinnacleMsgHandler ", 0 );
	     }  
		 if( ST1_LOAD_MID_CPNG_TFINIRECORDS == ptemsg_get_msg_subtype1(p_msg_in) )
	     {
			dcpimp_read_MID_capping_response_code();
			send_trace_response(p_msg_in);
			sprintf( buffer, "MIDCpng records loaded for %s, version %s", ServiceName, Version );
            dcpimp_log_message( 1, 1, buffer," PinnacleMsgHandler ", 0 );
	     }
		 else if(ST1_LOAD_TO_RC_VALUES == ptemsg_get_msg_subtype1(p_msg_in))
		 {
			 dcpimp_read_TO_RC_for_mp_plfc();
			 send_trace_response(p_msg_in);
			 sprintf( buffer,"TO RC  MP: %s,PLFC:%s", mp_rc_for_TO, plfc_rc_for_TO);
			 dcpimp_log_message( 1, 1, buffer,"PinnacleMsgHandler",0 );
		 }
		 else if(ST1_LOAD_RC_FOR_FAI == ptemsg_get_msg_subtype1(p_msg_in))
		 {
			 dcpimp_read_FRT_Decline_RC_MP();
			 send_trace_response(p_msg_in);
			 sprintf( buffer,"FAI RC MP: %s,PLFC:%s", mp_rc_for_fai, plfc_rc_for_fai);
			 dcpimp_log_message( 1, 1, buffer,"PinnacleMsgHandler",0 );
		 }
		 else if(ST1_SYS_FALCONRT_ON == ptemsg_get_msg_subtype1(p_msg_in))
		 {
			 /* Read the FALCON RT processing flag from tf.ini*/
			 	Falcon_RT_Flag = 1;
			 	dcpimp_write_falcon_RT_falg_into_inifile("1");
			 	send_trace_response(p_msg_in);
		 }
		 else  if(ST1_SYS_FALCONRT_OFF == ptemsg_get_msg_subtype1(p_msg_in))
		 {
			 /* Read the FALCON RT processing flag from tf.ini*/
			 	Falcon_RT_Flag = 0;
			 	send_trace_response(p_msg_in);
			 	dcpimp_write_falcon_RT_falg_into_inifile("0");
		 }
		 else if(ST1_RELOAD_FALCON_FLAG == ptemsg_get_msg_subtype1(p_msg_in))
		 {
			 /* Read the FALCON RT processing flag from DB*/
			 dcpimp_Get_Falcon_RT_ConfigureLink_From_DB_For_All_Brands();
			 dcpimp_Get_Falcon_RT_ConfigureRule_From_DB_For_All_Brands();
			 dcpimp_read_FRT_Decline_RC_MP();
			 dcpimp_read_falconrt_ext10_flag_for_transactions();
			 send_trace_response(p_msg_in);
		 }
	#ifdef HCTS2
		 else if(ptemsg_get_msg_subtype1(p_msg_in) == ST1_VERTEXON_CARD_STATUS_FLAG)
		 {
			 dcpimp_read_atp_card_activation_parameters();
			 send_trace_response(p_msg_in);
			 dcpimp_log_message( 3, 1, "Vertexon card status parameters reloaded", "PinnacleMsgHandler", 0 );
		 }
	#endif
		 else if(ST1_SYS_FALCONRT_EXT10_ON == ptemsg_get_msg_subtype1(p_msg_in))
		 {
			 	Falcon_RT_EXT10_Flag = 1;
			 	dcpimp_write_falconRT_EXT10_flag_into_inifile("1");
			 	dcpimp_read_falconrt_ext10_flag_for_transactions();
			 	send_trace_response(p_msg_in);
		 }
		 else if(ST1_SYS_FALCONRT_EXT10_OFF == ptemsg_get_msg_subtype1(p_msg_in))
		 {
			 	Falcon_RT_EXT10_Flag = 0;
			 	dcpimp_write_falconRT_EXT10_flag_into_inifile("0");
			 	dcpimp_read_falconrt_ext10_flag_for_transactions();
			 	send_trace_response(p_msg_in);
		 }

      break;        

      case MT_DB_REPLY:
	#ifdef HCTS2
    	 if(ATP_CARD_STATUS_UPDATE_BY_BDO_HOST == ptemsg_get_pte_msg_data_app_data_type(ptemsg_get_pte_msg_data (p_msg_in)))
    		break;
	#endif

         retCode = ptemsg_get_result_code (p_msg_in) ;
         if (retCode == PTEMSG_OK)
         {
        	 printf("Reterive result code is OK %d\n",retCode);
            retCode = Process_Dataserver_OK_Message (p_msg_in) ;
         }
         else if (retCode == PTEMSG_NOT_FOUND)
         {
        	 printf("Reterive result code is Not OK %d\n",retCode);
            retCode = Process_Dataserver_NotFound_Message (p_msg_in) ;
         }
         else 
         {
        	 printf("Reterive result code is Error %d\n",retCode);
            retCode = Process_Dataserver_ERROR_Message (p_msg_in) ;
         }

         if (retCode != DEVICE_OK)
         {
            /* Control comes here on SYSTEM_ERROR, BCH01_TOTALS_MATCHED,
             * BCH01_TOTALS_NOT_MATCHED, BCH10_INSERT_SUCCESSFUL, 
             * SEND_RESPONSE_TO_ORIGINATOR 
             */
            if (If_Transaction_Cannot_Be_Declined ())
            {
               /* If transactions cannot be declined, set the response code
                * to 00 (even for SYSTEM_ERROR)
                */
               strcpy( auth_tx.TLF01_details.response_code,
                       TRANSACTION_APPROVED) ;
            }
            Send_Transaction_To_Originator () ;
         }
      break;

	  case MT_INCOMING_DCPISO:
	  	retCode = Process_incoming_dcpiso_message(p_msg_in);
	  
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

      case MT_INCOMING_GUI:
         (void)process_0312( p_msg_in );
      break;

      case MT_RT_RESPONSE_APPROVED:
    	  Send_Transaction_To_Authorizer();
		  break;

	  case MT_RT_RESPONSE_DECLINE:
		  /*This transaction is decline*/
		  p_msg_data = ptemsg_get_pte_msg_data (p_msg_in);
		  pCurrent   = ptemsg_get_pte_msg_data_data (p_msg_data);

		  memcpy (&auth_tx, pCurrent, sizeof auth_tx) ;
		  strncpy(auth_tx.TLF01_details.response_code,FRT_Decline_RC_MP,
				  FRT_DECLINE_RC_LEN);
		  sprintf(buffer,"Real Time response is Decline");
		  dcpimp_log_message( 3, 1,buffer, "PinnacleMsgHandler", 1 );

		  /* the transactions declined by FRT will be ATP-STIP procsd */
		  strcpy(auth_tx.TLF01_details.product_codes[1].code,STANDIN);
		  strcpy(auth_tx.TLF01_details.response_text,"ATP standin, FAI Decline");

		  Send_Transaction_To_Originator();
		  /* new fn with RT_decline and response code for each card brand */
		  break;

	  case MT_RT_RESPONSE_TIMEOUT:
		   /*print timeout msg in the log and do as approved */
		   sprintf(buffer,"Real Time response is timedout");
		   dcpimp_log_message( 3, 1,buffer, "PinnacleMsgHandler", 1 );
		   Send_Transaction_To_Authorizer();
		  break;


      default:
         dcpimp_log_message( 1, 2, "unknown msg_type received", " PinnacleMsgHandler ",0 );
      break;
   }  /* switch msg_type */
} /* PinnacleMsgHandler */


void dcpimp_read_vertexon_codes_for_ext10_falconrt()
{
	#define MAX_INI_INFO_SIZE 256

    int ind, index, codeIndex;
	DWORD rc;
	CHAR codesCount[3] = {0};
    CHAR codesBuffer[256] = {0};
    CHAR filename  [MAX_FILENAME_SIZE] = {0};
    CHAR tmpstr    [MAX_FILENAME_SIZE] = {0};
    CHAR tmpstring [MAX_INI_INFO_SIZE] = {0};
    CHAR ini_info  [MAX_INI_INFO_SIZE] = {0};

    memset(mp_in_vertexon_codes, 0x00, sizeof(mp_in_vertexon_codes));
    memset(pl_ft_vertexon_codes, 0x00, sizeof(pl_ft_vertexon_codes));

    GetPinnacleConfigDirectory(tmpstr);
    sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

    //MP/Installment codes
    rc = GetPrivateProfileString(
                "FALCON_EXT10_DATA",
                "MP_CODES",
                "",
                ini_info,
                MAX_INI_INFO_SIZE - 1,
                filename);

	if(rc != 1)
	{
		dcpimp_log_message(2, 2, "MP/IN CODES not read", "dcpimp_read_vertexon_codes_for_ext10_falconrt", 0);
	}
	memset(codesBuffer, 0x00, sizeof(codesBuffer));
	strncpy(codesBuffer, ini_info, strlen(ini_info));

	ind = 0; index = 0; codeIndex = 0;
    if( strlen(codesBuffer) > 0 && codesBuffer[(strlen(codesBuffer)-1)] == ';')
    {
		while(codesBuffer[index] != ';')
		{
			if(codesBuffer[index] != ',')
			{
				mp_in_vertexon_codes[codeIndex+1][ind] = codesBuffer[index];
				ind++;
			}
			else
			{
				codeIndex++;
				ind = 0;
			}
			index++;
		}
		sprintf(mp_in_vertexon_codes[0], "%d", codeIndex+1);
    }
    else
    {
    	dcpimp_log_message(2, 2, "MP/IN CODES contains invalid data", "dcpimp_read_vertexon_codes_for_ext10_falconrt", 0);
    }

    //Pvt. Label/Fleet codes
    rc = GetPrivateProfileString(
                "FALCON_EXT10_DATA",
                "PL_CODES",
                "",
                ini_info,
                MAX_INI_INFO_SIZE - 1,
                filename);

	if(rc != 1)
	{
		dcpimp_log_message(2, 2, "PL/FT CODES not read", "dcpimp_read_vertexon_codes_for_ext10_falconrt", 0);
	}
	memset(codesBuffer, 0x00, sizeof(codesBuffer));
	strncpy(codesBuffer, ini_info, strlen(ini_info));

	ind = 0; index = 0; codeIndex = 0;
    if( strlen(codesBuffer) > 0 && codesBuffer[(strlen(codesBuffer)-1)] == ';')
    {
		while(codesBuffer[index] != ';')
		{
			if(codesBuffer[index] != ',')
			{
				pl_ft_vertexon_codes[codeIndex+1][ind] = codesBuffer[index];
				ind++;
			}
			else
			{
				codeIndex++;
				ind = 0;
			}
			index++;
		}
		sprintf(pl_ft_vertexon_codes[0], "%d", codeIndex+1);
    }
    else
    {
    	dcpimp_log_message(2, 2, "PL/FT CODES contains invalid data", "dcpimp_read_vertexon_codes", 0);
    }
}

/******************************  END OF FILE  ****************************************************/
