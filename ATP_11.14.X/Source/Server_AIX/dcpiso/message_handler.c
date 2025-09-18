/**************************************************************************************************
*  
* MODULE:      message_handler.c


* TITLE:       ATP Standard - Terminal Handler Main.
*  
* DESCRIPTION: Main startup and end process routines. Waits for incoming PTE message and 
*              passes control to message processing loop.  
*
* APPLICATION: DCPISO
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

AUTH_TX auth_tx;
NCF01  ncf01_recs[MAX_NCF01_RECS];

CHAR    devds_que_name[]       = "devdsA" ;
CHAR    trands_que_name []     = "trandsA" ;
CHAR    updateds_que_name []   = "updatdsA" ;
CHAR    mrads_que_name[]       = "mradsA" ;
CHAR    authorizer_que_name [] = "txcntlA" ;
CHAR    ncvisa_que_name []     = "ncvisaA" ;
CHAR    visa1_que_name []      = "visa1A" ;
CHAR    oprmon_que_name []     = "oprmonI" ;
CHAR    Version[]              = "ATP_11.14.5";
CHAR mracon_que_name  [MAX_QUE_NAME_SIZE+1] = "mraconA" ;
CHAR dcpiso_que_name[]       = "dcpisoA";
											
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

INT Nmra_send_flag = 0;
INT Mracon_Log_Enable_Flag = 0;
CHAR  pl_nii_value[4] = {0};

char mc_timeout_rc[3]={0};
char visa_timeout_rc[3]={0};
char amex_timeout_rc[3]={0};
char jcb_timeout_rc [3]={0};
char cup_timeout_rc [3]={0};
char dci_timeout_rc [3]={0};

char mc_fai_rc[3]={0};
char visa_fai_rc[3]={0};
char amex_fai_rc[3]={0};
char jcb_fai_rc [3]={0};
char cup_fai_rc [3]={0};
char dci_fai_rc [3]={0};
/*OFFLINE Preauth Validation specific Tf.ini parameters*/
CHAR  percent_overlimit_value_for_VISA[4] = {0};
CHAR  percent_overlimit_value_for_MC[4] = {0};
CHAR  percent_overlimit_value_for_JCB[4] = {0};
CHAR  percent_overlimit_value_for_AMEX[4] = {0};
CHAR  percent_overlimit_value_for_CUP[4] = {0};
CHAR  percent_overlimit_value_for_DINERS[4] = {0};
CHAR  error_response_code_for_PREAUTH_COMPLETION_VALIDATION[3]={0};

CHAR Incremental_Auth_flag_CUP[1]={0};
CHAR  emn_contactless_offline_sale_authID[3]={0};
CHAR  validate_preauth_validation_processing_code[7]={0};
CHAR  CUP_merchant_indicator[8]={0};
CHAR  dcpiso_Erro_warning_Filename[256] = {0};
CHAR  dcpiso_module_error_warning_file_name[256] = {0};
CHAR  dcpiso_error_warning_file_path[256] = {0};
CHAR  ATP_pos_default_response_code[3]= {0};


#if BDOR_62_PREAUTH_VOID
CHAR insufficient_preauth_declined_rc_for_MC[3]={0};
CHAR insufficient_preauth_declined_rc_for_VISA[3]={0};
CHAR insufficient_preauth_declined_rc_for_AMEX[3]={0};
CHAR insufficient_preauth_declined_rc_for_DCI[3]={0};
CHAR insufficient_preauth_declined_rc_for_JCB[3]={0};
CHAR insufficient_preauth_declined_rc_for_UPI[3]={0};

CHAR declined_rc_for_expired_used_preauths_for_MC[3]={0};
CHAR declined_rc_for_expired_used_preauths_for_VISA[3]={0};
CHAR declined_rc_for_expired_used_preauths_for_AMEX[3]={0};
CHAR declined_rc_for_expired_used_preauths_for_DCI[3]={0};
CHAR declined_rc_for_expired_used_preauths_for_JCB[3]={0};
CHAR declined_rc_for_expired_used_preauths_for_UPI[3]={0};

CHAR declined_rc_for_original_txn_not_present_for_MC[3]={0};
CHAR declined_rc_for_original_txn_not_present_for_VISA[3]={0};
CHAR declined_rc_for_original_txn_not_present_for_AMEX[3]={0};
CHAR declined_rc_for_original_txn_not_present_for_DCI[3]={0};
CHAR declined_rc_for_original_txn_not_present_for_JCB[3]={0};
CHAR declined_rc_for_original_txn_not_present_for_UPI[3]={0};

CHAR maximum_validity_of_preauth_in_days_for_MC[3]={0};
CHAR maximum_validity_of_preauth_in_days_for_VISA[3]={0};
CHAR maximum_validity_of_preauth_in_days_for_AMEX[3]={0};
CHAR maximum_validity_of_preauth_in_days_for_DCI[3]={0};
CHAR maximum_validity_of_preauth_in_days_for_JCB[3]={0};
CHAR maximum_validity_of_preauth_in_days_for_UPI[3]={0};

CHAR smcc_maximum_validity_of_preauth_in_days_for_MC[3]={0};
CHAR smcc_maximum_validity_of_preauth_in_days_for_VISA[3]={0};
CHAR smcc_maximum_validity_of_preauth_in_days_for_AMEX[3]={0};
CHAR smcc_maximum_validity_of_preauth_in_days_for_DCI[3]={0};
CHAR smcc_maximum_validity_of_preauth_in_days_for_JCB[3]={0};
CHAR smcc_maximum_validity_of_preauth_in_days_for_UPI[3]={0};


CHAR smcc_id1[5]={0};
CHAR smcc_id2[5]={0};
CHAR smcc_id3[5]={0};
CHAR smcc_id4[5]={0};
CHAR smcc_id5[5]={0};
CHAR smcc_id6[5]={0};
CHAR smcc_id7[5]={0};
CHAR smcc_id8[5]={0};
CHAR smcc_id9[5]={0};
CHAR smcc_id10[5]={0};


CHAR  flag_for_sending_partial_reversals_for_MC[2]={0};
CHAR  flag_for_sending_partial_reversals_for_VISA[2]={0};
CHAR  flag_for_sending_partial_reversals_for_AMEX[2]={0};
CHAR  flag_for_sending_partial_reversals_for_DCI[2]={0};
CHAR  flag_for_sending_partial_reversals_for_JCB[2]={0};

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
   CHAR Buffer[100] = {0};

#ifdef _DEBUG
    pteipc_trace_off(); 
#endif

   if ( Tracing == TRACE_ON )
   {
      if ( false == close_trace_file( ppTraceFile_fp ))
      {
         sprintf( Buffer, "Unable to close trace file %s", TraceFile );
         dcpiso_log_message( 1, 2, Buffer,"EndProcess",0 );
      }
   }

   sprintf( Buffer, "Shutting down the %s Service, version %s",
            ServiceName, Version );
   dcpiso_log_message( 2, 1, Buffer,"EndProcess", 0 );
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

INT dcpiso_read_TO_RC_for_card_brands()
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
	   			 "DCPISO_TIMEOUT_RC",                   /* points to section name       */
	   			"DCPISO_TIMEOUT_RC_FOR_MC",  /* points to key name           */
	   			"19",                       /* points to default string     */
	   			 ini_info,                      /* points to destination buffer */
	   			 MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
	   			 filename                       /* points to ini filename       */
	   	);

	   	strncpy(mc_timeout_rc,ini_info,2);

	   	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
	   	rc = GetPrivateProfileString(
	   			"DCPISO_TIMEOUT_RC",                   /* points to section name       */
	   		   "DCPISO_TIMEOUT_RC_FOR_VISA",  /* points to key name           */
	   		   "19",                       /* points to default string     */
	   			ini_info,                      /* points to destination buffer */
	   			MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
	   			filename                       /* points to ini filename       */
	   	);

	   	strncpy(visa_timeout_rc,ini_info,2);

	   	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
	   	rc = GetPrivateProfileString(
	   		   "DCPISO_TIMEOUT_RC",                   /* points to section name       */
	   		  "DCPISO_TIMEOUT_RC_FOR_JCB",  /* points to key name           */
	   		  "19",                       /* points to default string     */
	   		   ini_info,                      /* points to destination buffer */
	   		   MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
	   		   filename                       /* points to ini filename       */
	   	);

	   	strncpy(jcb_timeout_rc,ini_info,2);

	   	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
	   	rc = GetPrivateProfileString(
	   		  "DCPISO_TIMEOUT_RC",                   /* points to section name       */
	   		 "DCPISO_TIMEOUT_RC_FOR_AMEX",  /* points to key name           */
	   		 "19",                       /* points to default string     */
	   		  ini_info,                      /* points to destination buffer */
	   		  MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
	   		  filename                       /* points to ini filename       */
	   	);

	   	strncpy(amex_timeout_rc,ini_info,2);

	   	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
	   	rc = GetPrivateProfileString(
	   		  "DCPISO_TIMEOUT_RC",                   /* points to section name       */
	   		 "DCPISO_TIMEOUT_RC_FOR_CUP",  /* points to key name           */
	   		 "19",                       /* points to default string     */
	   		  ini_info,                      /* points to destination buffer */
	   		  MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
	   		  filename                       /* points to ini filename       */
	   	);

	   	strncpy(cup_timeout_rc,ini_info,2);

	   	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
	   	rc = GetPrivateProfileString(
	   		  "DCPISO_TIMEOUT_RC",                   /* points to section name       */
	   		 "DCPISO_TIMEOUT_RC_FOR_DCI",  /* points to key name           */
	   		 "19",                       /* points to default string     */
	   		  ini_info,                      /* points to destination buffer */
	   		  MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
	   		  filename                       /* points to ini filename       */
	   	);

	   	strncpy(dci_timeout_rc,ini_info,2);
   return( ret_val );
}

INT dcpiso_read_FAI_RC_for_card_brands()
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
	   			 "DCPISO_DECLINE_RC_FOR_FAI_DECISION_CODE",                   /* points to section name       */
	   			"DCPISO_DECLINE_RC_FOR_FAI_DECISION_CODE_MC",  /* points to key name           */
	   			"19",                       /* points to default string     */
	   			 ini_info,                      /* points to destination buffer */
	   			 MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
	   			 filename                       /* points to ini filename       */
	   	);

	   	strncpy(mc_fai_rc,ini_info,2);

	   	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
	   	rc = GetPrivateProfileString(
	   			"DCPISO_DECLINE_RC_FOR_FAI_DECISION_CODE",                   /* points to section name       */
	   		   "DCPISO_DECLINE_RC_FOR_FAI_DECISION_CODE_VISA",  /* points to key name           */
	   		   "19",                       /* points to default string     */
	   			ini_info,                      /* points to destination buffer */
	   			MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
	   			filename                       /* points to ini filename       */
	   	);

	   	strncpy(visa_fai_rc,ini_info,2);

	   	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
	   	rc = GetPrivateProfileString(
	   		   "DCPISO_DECLINE_RC_FOR_FAI_DECISION_CODE",                   /* points to section name       */
	   		  "DCPISO_DECLINE_RC_FOR_FAI_DECISION_CODE_JCB",  /* points to key name           */
	   		  "19",                       /* points to default string     */
	   		   ini_info,                      /* points to destination buffer */
	   		   MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
	   		   filename                       /* points to ini filename       */
	   	);

	   	strncpy(jcb_fai_rc,ini_info,2);

	   	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
	   	rc = GetPrivateProfileString(
	   		  "DCPISO_DECLINE_RC_FOR_FAI_DECISION_CODE",                   /* points to section name       */
	   		 "DCPISO_DECLINE_RC_FOR_FAI_DECISION_CODE_AMEX",  /* points to key name           */
	   		 "19",                       /* points to default string     */
	   		  ini_info,                      /* points to destination buffer */
	   		  MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
	   		  filename                       /* points to ini filename       */
	   	);

	   	strncpy(amex_fai_rc,ini_info,2);

	   	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
	   	rc = GetPrivateProfileString(
	   		  "DCPISO_DECLINE_RC_FOR_FAI_DECISION_CODE",                   /* points to section name       */
	   		 "DCPISO_DECLINE_RC_FOR_FAI_DECISION_CODE_CUP",  /* points to key name           */
	   		 "19",                       /* points to default string     */
	   		  ini_info,                      /* points to destination buffer */
	   		  MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
	   		  filename                       /* points to ini filename       */
	   	);

	   	strncpy(cup_fai_rc,ini_info,2);

	   	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
	   	rc = GetPrivateProfileString(
	   		  "DCPISO_DECLINE_RC_FOR_FAI_DECISION_CODE",                   /* points to section name       */
	   		 "DCPISO_DECLINE_RC_FOR_FAI_DECISION_CODE_DCI",  /* points to key name           */
	   		 "19",                       /* points to default string     */
	   		  ini_info,                      /* points to destination buffer */
	   		  MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
	   		  filename                       /* points to ini filename       */
	   	);

	   	strncpy(dci_fai_rc,ini_info,2);
	      return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         read_private_lable_flag
 *
 *  DESCRIPTION:  This function reads information from the tf ini file.
 *                This flag information is stored in global variables for use of 
 *					Private lable settlement transaction Send
 *                .
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      err_buf - Text message explaining why this function failed
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT read_private_lable_flag( )
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
             "PLSECTION",                   /* points to section name       */
             "NII",  /* points to key name           */
             "0",                       /* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   strncpy( pl_nii_value, tmpstring, 3 );
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         read_percent_overlimit_value_for_VISA
 *
 *  DESCRIPTION:  This function reads information from the tf ini file.
 *                This value is used to do validation for pre auth sale completion for
 *                visa tranasction.
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
INT read_percent_overlimit_value_for_VISA( )
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
             "PREAUTHOVERLIMITPERCENT",                   /* points to section name       */
             "VISA_PREAUTHOVERLIMITPERCENT",  /* points to key name           */
             "0",                       /* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   strncpy( percent_overlimit_value_for_VISA, tmpstring, 3 );
   if(strlen(percent_overlimit_value_for_VISA)==0)
   {
	   strncpy( percent_overlimit_value_for_VISA, PERCENT_OVERLIMIT_VALUE_DEFAULT, 3 );// Default Value
   }
   return( ret_val );
}

INT read_Incremental_Auth_flag_CUP( )
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
             "VALIDATION_INCREMENTAL_PREAUTH_FLAG_CUP",                   /* points to section name       */
             "INCREMENTAL_PREAUTH_FLAG_CUP",  /* points to key name           */
             "0",                       /* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   strncpy( Incremental_Auth_flag_CUP, tmpstring, 1 );

   return( ret_val );
}
/******************************************************************************
 *
 *  NAME:         read_percent_overlimit_value_for_MC
 *
 *  DESCRIPTION:  This function reads information from the tf ini file.
 *                This value is used to do validation for pre auth sale completion for
 *                MasterCard tranasction.
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
INT read_percent_overlimit_value_for_MC(  )
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
             "PREAUTHOVERLIMITPERCENT",                   /* points to section name       */
             "MC_PREAUTHOVERLIMITPERCENT",  /* points to key name           */
             "0",                       /* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   strncpy( percent_overlimit_value_for_MC, tmpstring, 3 );
   if(strlen(percent_overlimit_value_for_MC)==0)
   {
  	   strncpy( percent_overlimit_value_for_MC, PERCENT_OVERLIMIT_VALUE_DEFAULT, 3 );// Default Value
   }
   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         read_percent_overlimit_value_for_JCB
 *
 *  DESCRIPTION:  This function reads information from the tf ini file.
 *                This value is used to do validation for pre auth sale completion for
 *                JCB tranasction.
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
INT read_percent_overlimit_value_for_JCB(  )
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
             "PREAUTHOVERLIMITPERCENT",                   /* points to section name       */
             "JCB_PREAUTHOVERLIMITPERCENT",  /* points to key name           */
             "0",                       /* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   strncpy( percent_overlimit_value_for_JCB, tmpstring, 3 );
   if(strlen(percent_overlimit_value_for_JCB)==0)
   {
	   strncpy( percent_overlimit_value_for_JCB, PERCENT_OVERLIMIT_VALUE_DEFAULT, 3 );// Default Value
   }
   return( ret_val );
}




/******************************************************************************
 *
 *  NAME:         read_percent_overlimit_value_for_AMEX
 *
 *  DESCRIPTION:  This function reads information from the tf ini file.
 *                This value is used to do validation for pre auth sale completion for
 *                AMEX tranasction.
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
INT read_percent_overlimit_value_for_AMEX( )
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
             "PREAUTHOVERLIMITPERCENT",                   /* points to section name       */
             "AMEX_PREAUTHOVERLIMITPERCENT",  /* points to key name           */
             "0",                       /* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   strncpy( percent_overlimit_value_for_AMEX, tmpstring, 3 );
   if(strlen(percent_overlimit_value_for_AMEX)==0)
	{
	   strncpy( percent_overlimit_value_for_AMEX, PERCENT_OVERLIMIT_VALUE_DEFAULT, 3 );// Default Value
	}
   return( ret_val );
}



/******************************************************************************
 *
 *  NAME:         read_percent_overlimit_value_for_CUP
 *
 *  DESCRIPTION:  This function reads information from the tf ini file.
 *                This value is used to do validation for pre auth sale completion for
 *                CUP tranasction.
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
INT read_percent_overlimit_value_for_CUP(  )
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
             "PREAUTHOVERLIMITPERCENT",                   /* points to section name       */
             "CUP_PREAUTHOVERLIMITPERCENT",  /* points to key name           */
             "0",                       /* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   strncpy( percent_overlimit_value_for_CUP, tmpstring, 3 );
   if(strlen(percent_overlimit_value_for_CUP)==0)
   {
	   strncpy( percent_overlimit_value_for_CUP, PERCENT_OVERLIMIT_VALUE_DEFAULT, 3 );// Default Value
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         read_percent_overlimit_value_for_DINERS
 *
 *  DESCRIPTION:  This function reads information from the tf ini file.
 *                This value is used to do validation for pre auth sale completion for
 *                AMEX tranasction.
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
INT read_percent_overlimit_value_for_DINERS( )
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
             "PREAUTHOVERLIMITPERCENT",                   /* points to section name       */
             "DINERS_PREAUTHOVERLIMITPERCENT",  /* points to key name           */
             "0",                       /* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   strncpy( percent_overlimit_value_for_DINERS, tmpstring, 3 );
   if(strlen(percent_overlimit_value_for_DINERS)==0)
	{
	   strncpy( percent_overlimit_value_for_DINERS, PERCENT_OVERLIMIT_VALUE_DEFAULT, 3 );// Default Value
	}
   return( ret_val );
}



/******************************************************************************
 *
 *  NAME:         read_PREAUTH_COMPLETION_VALIDATION_Eror_Response_code
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
INT read_PREAUTH_COMPLETION_VALIDATION_Eror_Response_code( void )
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
             "ERROR_RESPONSE_CODE",     /* points to section name       */
             "PREAUTH_COMPLETION_VALIDATION",  /* points to key name           */
             "0",                       		/* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   strncpy( error_response_code_for_PREAUTH_COMPLETION_VALIDATION, tmpstring, 2 );
   if(strlen(error_response_code_for_PREAUTH_COMPLETION_VALIDATION)==0)
   {
	   strncpy( error_response_code_for_PREAUTH_COMPLETION_VALIDATION,
			    PREAUTH_COMPLETION_VALIDATION_RESPONSE_ERROR_CODE,
	   			RESPONSE_CODE_LEN);// Default Value
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         read_emn_contactless_offline_sale_authID
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
INT read_emn_contactless_offline_sale_authID( void )
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
             "PREAUTH_COMPLETION_VALIDATION",     /* points to section name       */
             "EMV_CONTLESS_OFFLINE_SALE_AUTHID",  /* points to key name           */
             "0",                       		/* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   strncpy( emn_contactless_offline_sale_authID, tmpstring, 2 );
   if(strlen(emn_contactless_offline_sale_authID)==0)
   {
	   strncpy( emn_contactless_offline_sale_authID,
			   EMV_CONTLESS_OFFLINE_SALE_AUTHID_VALUE,
	   			RESPONSE_CODE_LEN);// Default Value
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         read_validate_preauth_validation_processing_code
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
INT read_validate_preauth_validation_processing_code( void )
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
             "PREAUTH_COMPLETION_VALIDATION",     /* points to section name       */
             "PREAUTH_VALIDATION_PROC_CODE",  /* points to key name           */
             "0",                       		/* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   strncpy( validate_preauth_validation_processing_code, tmpstring, PROCESSING_CODE_LEN );
   if(strlen(validate_preauth_validation_processing_code)==0)
   {
	   strncpy( validate_preauth_validation_processing_code,
			    PREAUTH_VALIDATION_PROCESSING_CODE,
				PROCESSING_CODE_LEN);// Default Value
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         dcpiso_read_CUP_merchant_indicator
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
INT dcpiso_read_CUP_merchant_indicator_dcpiso( void )
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
 *  NAME:         dcpiso_read_POS_default_system_response_code
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
INT dcpiso_read_POS_default_system_response_code( void )
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

#if AMEX03B_APRIL2024
INT get_variable_auth_config_details()
{
	   CHAR  flag_value[2]="";
	   CHAR  date[9]={0};
	   INT   ret_val = true;
	   CHAR  filename   [80];
	   CHAR  tmpstr     [80];

	   /* Get path to the tf.ini file.*/
	   memset( tmpstr, 0x00, sizeof(tmpstr) );
	   GetPinnacleConfigDirectory(tmpstr);
	   sprintf(filename, "%s%s", tmpstr, "tf.ini" );

	   ret_val = GetPrivateProfileString(
	  								  "VARIABLE_AUTHORIZATION",             /* points to section name         */
	  								  "ACQ_AMEX_VARIABLE_AUTH_FLAG",       	/* points to key name             */
	  								  "N",  					/* Default string                 */
									  flag_value,              	/* points to destination buffer   */
	  								  sizeof(flag_value)-1,   	 /* size of destination buffer     */
	  								  filename                  /* points to ini filename         */
	  								);

	   acq_amex_variable_auth_flag=flag_value[0];

	   ret_val = GetPrivateProfileString(
	 								  "VARIABLE_AUTHORIZATION",             /* points to section name         */
	 								  "LAST_DATE",       	/* points to key name             */
	 								  "",  					/* Default string                 */
	 								  date,              	/* points to destination buffer   */
	 								  sizeof(date)-1,   	 /* size of destination buffer     */
	 								  filename                  /* points to ini filename         */
	 								);

	 	   strncpy(last_date,date,8);

	   return ret_val;
}
INT get_last_dated_date_only()
{
	   CHAR  date[9]={0};
	   INT   ret_val = true;
	   CHAR  filename   [80];
	   CHAR  tmpstr     [80];

	   memset( tmpstr, 0x00, sizeof(tmpstr) );
	   GetPinnacleConfigDirectory(tmpstr);
	   sprintf(filename, "%s%s", tmpstr, "tf.ini" );

	   memset(last_date,0x00,sizeof(last_date));

	   ret_val = GetPrivateProfileString(
								  "VARIABLE_AUTHORIZATION",             /* points to section name         */
								  "LAST_DATE",       	/* points to key name             */
								  "",  					/* Default string                 */
								  date,              	/* points to destination buffer   */
								  sizeof(date)-1,   	 /* size of destination buffer     */
								  filename                  /* points to ini filename         */
								);

	   strncpy(last_date,date,8);
	   return ret_val;
}
void create_system_date( pBYTE system_date )
{
   CHAR  time_date[25] = "";

   memset( system_date,  0x00, 9 );

   /* Get local time - Format = "YYYY-0M-0D-0H.0I.0S.JJJ" */
   ptetime_get_timestamp( time_date );

   /* Format it. */
   strncpy( system_date,  time_date,    4 );
   strncat( system_date, &time_date[5], 2 );
   strncat( system_date, &time_date[8], 2 );
   return;
}
#endif


#if BDOR_62_PREAUTH_VOID

INT get_declined_RC_for_insufficient_Preauth(void)
{
   CHAR  rc_value[3]={0};
   CHAR  date[9]={0};
   INT   ret_val = true;
   CHAR  filename   [80] = {0};
   CHAR  tmpstr     [80] = {0};

   /* Get path to the tf.ini file.*/
   memset( tmpstr, 0x00, sizeof(tmpstr) );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, "tf.ini" );

   ret_val = GetPrivateProfileString(
									  "INSUFFICIENT_PREAUTH_DECLINED_RC",             /* points to section name         */
									  "INSUFFICIENT_PREAUTH_DECLINED_RC_FOR_MC",       	/* points to key name             */
									  "12",  					/* Default string                 */
									  rc_value,              	/* points to destination buffer   */
									  sizeof(rc_value)-1,   	 /* size of destination buffer     */
									  filename                  /* points to ini filename         */
									);

	if(strlen(rc_value)==0)
	{
	   strncpy( insufficient_preauth_declined_rc_for_MC,
			    INSUFFICIENT_PREAUTH_DECLINED_RC,
				ISO_RESPONSE_CODE_LEN );
	}
	else
	{
		strncpy(insufficient_preauth_declined_rc_for_MC,rc_value,ISO_RESPONSE_CODE_LEN);
	}

	ret_val = GetPrivateProfileString(
										"INSUFFICIENT_PREAUTH_DECLINED_RC",             /* points to section name         */
										"INSUFFICIENT_PREAUTH_DECLINED_RC_FOR_VISA",       	/* points to key name             */
										"12",  					/* Default string                 */
										rc_value,              	/* points to destination buffer   */
										sizeof(rc_value)-1,   	 /* size of destination buffer     */
										filename                  /* points to ini filename         */
										);

	if(strlen(rc_value)==0)
	{
	   strncpy( insufficient_preauth_declined_rc_for_VISA,
			    INSUFFICIENT_PREAUTH_DECLINED_RC,
				ISO_RESPONSE_CODE_LEN );
	}
	else
	{
		strncpy(insufficient_preauth_declined_rc_for_VISA,rc_value,ISO_RESPONSE_CODE_LEN);
	}


	ret_val = GetPrivateProfileString(
										"INSUFFICIENT_PREAUTH_DECLINED_RC",             /* points to section name         */
										"INSUFFICIENT_PREAUTH_DECLINED_RC_FOR_AMEX",       	/* points to key name             */
										"12",  					/* Default string                 */
										rc_value,              	/* points to destination buffer   */
										sizeof(rc_value)-1,   	 /* size of destination buffer     */
										filename                  /* points to ini filename         */
										);
	if(strlen(rc_value)==0)
	{
		strncpy(insufficient_preauth_declined_rc_for_AMEX,
				INSUFFICIENT_PREAUTH_DECLINED_RC,
				ISO_RESPONSE_CODE_LEN );
	}
	else
	{
		strncpy(insufficient_preauth_declined_rc_for_AMEX,rc_value,ISO_RESPONSE_CODE_LEN);
	}

	ret_val = GetPrivateProfileString(
										"INSUFFICIENT_PREAUTH_DECLINED_RC",             /* points to section name         */
										"INSUFFICIENT_PREAUTH_DECLINED_RC_FOR_DCI",       	/* points to key name             */
										"12",  					/* Default string                 */
										rc_value,              	/* points to destination buffer   */
										sizeof(rc_value)-1,   	 /* size of destination buffer     */
										filename                  /* points to ini filename         */
										);

	if(strlen(rc_value)==0)
	{
		strncpy(insufficient_preauth_declined_rc_for_DCI,
				INSUFFICIENT_PREAUTH_DECLINED_RC,
				ISO_RESPONSE_CODE_LEN );
	}
	else
	{
		strncpy(insufficient_preauth_declined_rc_for_DCI,rc_value,ISO_RESPONSE_CODE_LEN);
	}

	ret_val = GetPrivateProfileString(
									  "INSUFFICIENT_PREAUTH_DECLINED_RC",             /* points to section name         */
									  "INSUFFICIENT_PREAUTH_DECLINED_RC_FOR_JCB",       	/* points to key name             */
									  "12",  					/* Default string                 */
									  rc_value,              	/* points to destination buffer   */
									  sizeof(rc_value)-1,   	 /* size of destination buffer     */
									  filename                  /* points to ini filename         */
									);

	if(strlen(rc_value)==0)
	{
		strncpy(insufficient_preauth_declined_rc_for_JCB,
				INSUFFICIENT_PREAUTH_DECLINED_RC,
				ISO_RESPONSE_CODE_LEN );
	}
	else
	{
		strncpy(insufficient_preauth_declined_rc_for_JCB,rc_value,ISO_RESPONSE_CODE_LEN);
	}

	ret_val = GetPrivateProfileString(
										"INSUFFICIENT_PREAUTH_DECLINED_RC",             /* points to section name         */
										"INSUFFICIENT_PREAUTH_DECLINED_RC_FOR_UPI",       	/* points to key name             */
										"12",  					/* Default string                 */
										rc_value,              	/* points to destination buffer   */
										sizeof(rc_value)-1,   	 /* size of destination buffer     */
										filename                  /* points to ini filename         */
										);

	if(strlen(rc_value)==0)
	{
		strncpy(insufficient_preauth_declined_rc_for_UPI,
				INSUFFICIENT_PREAUTH_DECLINED_RC,
				ISO_RESPONSE_CODE_LEN );
	}
	else
	{
		strncpy(insufficient_preauth_declined_rc_for_UPI,rc_value,ISO_RESPONSE_CODE_LEN);
	}

	return (ret_val);
}


INT get_declined_rc_for_expired_Used_preauths(void)
{
   CHAR  rc_value[3]={0};
   CHAR  date[9]={0};
   INT   ret_val = true;
   CHAR  filename   [80] = {0};
   CHAR  tmpstr     [80] = {0};

   /* Get path to the tf.ini file.*/
   memset( tmpstr, 0x00, sizeof(tmpstr) );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, "tf.ini" );

   ret_val = GetPrivateProfileString(
									  "EXPIRED_PREAUTH_DECLINED_RC",             /* points to section name         */
									  "EXPIRED_PREAUTH_DECLINED_RC_FOR_MC",       	/* points to key name             */
									  "12",  					/* Default string                 */
									  rc_value,              	/* points to destination buffer   */
									  sizeof(rc_value)-1,   	 /* size of destination buffer     */
									  filename                  /* points to ini filename         */
									);

	if(strlen(rc_value)==0)
	{
	   strncpy( declined_rc_for_expired_used_preauths_for_MC,
			    EXPIRED_PREAUTH_DECLINED_RC,
				ISO_RESPONSE_CODE_LEN );
	}
	else
	{
		strncpy(declined_rc_for_expired_used_preauths_for_MC,rc_value,ISO_RESPONSE_CODE_LEN);
	}

	ret_val = GetPrivateProfileString(
										"EXPIRED_PREAUTH_DECLINED_RC",             /* points to section name         */
										"EXPIRED_PREAUTH_DECLINED_RC_FOR_VISA",       	/* points to key name             */
										"12",  					/* Default string                 */
										rc_value,              	/* points to destination buffer   */
										sizeof(rc_value)-1,   	 /* size of destination buffer     */
										filename                  /* points to ini filename         */
										);

	if(strlen(rc_value)==0)
	{
	   strncpy( declined_rc_for_expired_used_preauths_for_VISA,
			   	EXPIRED_PREAUTH_DECLINED_RC,
				ISO_RESPONSE_CODE_LEN );
	}
	else
	{
		strncpy(declined_rc_for_expired_used_preauths_for_VISA,rc_value,ISO_RESPONSE_CODE_LEN);
	}


	ret_val = GetPrivateProfileString(
										"EXPIRED_PREAUTH_DECLINED_RC",             /* points to section name         */
										"EXPIRED_PREAUTH_DECLINED_RC_FOR_AMEX",       	/* points to key name             */
										"12",  					/* Default string                 */
										rc_value,              	/* points to destination buffer   */
										sizeof(rc_value)-1,   	 /* size of destination buffer     */
										filename                  /* points to ini filename         */
										);
	if(strlen(rc_value)==0)
	{
		strncpy(declined_rc_for_expired_used_preauths_for_AMEX,
				EXPIRED_PREAUTH_DECLINED_RC,
				ISO_RESPONSE_CODE_LEN );
	}
	else
	{
		strncpy(declined_rc_for_expired_used_preauths_for_AMEX,rc_value,ISO_RESPONSE_CODE_LEN);
	}

	ret_val = GetPrivateProfileString(
										"EXPIRED_PREAUTH_DECLINED_RC",             /* points to section name         */
										"EXPIRED_PREAUTH_DECLINED_RC_FOR_DCI",       	/* points to key name             */
										"12",  					/* Default string                 */
										rc_value,              	/* points to destination buffer   */
										sizeof(rc_value)-1,   	 /* size of destination buffer     */
										filename                  /* points to ini filename         */
										);

	if(strlen(rc_value)==0)
	{
		strncpy(declined_rc_for_expired_used_preauths_for_DCI,
				EXPIRED_PREAUTH_DECLINED_RC,
				ISO_RESPONSE_CODE_LEN );
	}
	else
	{
		strncpy(declined_rc_for_expired_used_preauths_for_DCI,rc_value,ISO_RESPONSE_CODE_LEN);
	}

	ret_val = GetPrivateProfileString(
									  "EXPIRED_PREAUTH_DECLINED_RC",             /* points to section name         */
									  "EXPIRED_PREAUTH_DECLINED_RC_FOR_JCB",       	/* points to key name             */
									  "12",  					/* Default string                 */
									  rc_value,              	/* points to destination buffer   */
									  sizeof(rc_value)-1,   	 /* size of destination buffer     */
									  filename                  /* points to ini filename         */
									);

	if(strlen(rc_value)==0)
	{
		strncpy(declined_rc_for_expired_used_preauths_for_JCB,
				EXPIRED_PREAUTH_DECLINED_RC,
				ISO_RESPONSE_CODE_LEN );
	}
	else
	{
		strncpy(declined_rc_for_expired_used_preauths_for_JCB,rc_value,ISO_RESPONSE_CODE_LEN);
	}

	ret_val = GetPrivateProfileString(
										"EXPIRED_PREAUTH_DECLINED_RC",             /* points to section name         */
										"EXPIRED_PREAUTH_DECLINED_RC_FOR_UPI",       	/* points to key name             */
										"12",  					/* Default string                 */
										rc_value,              	/* points to destination buffer   */
										sizeof(rc_value)-1,   	 /* size of destination buffer     */
										filename                  /* points to ini filename         */
										);

	if(strlen(rc_value)==0)
	{
		strncpy(declined_rc_for_expired_used_preauths_for_UPI,
				EXPIRED_PREAUTH_DECLINED_RC,
				ISO_RESPONSE_CODE_LEN );
	}
	else
	{
		strncpy(declined_rc_for_expired_used_preauths_for_UPI,rc_value,ISO_RESPONSE_CODE_LEN);
	}

	return (ret_val);
}

INT get_declined_rc_for_original_txn_not_present(void)
{
   CHAR  rc_value[3]={0};
   CHAR  date[9]={0};
   INT   ret_val = true;
   CHAR  filename   [80] = {0};
   CHAR  tmpstr     [80] = {0};

   /* Get path to the tf.ini file.*/
   memset( tmpstr, 0x00, sizeof(tmpstr) );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, "tf.ini" );

   ret_val = GetPrivateProfileString(
									  "ORIGINALl_PREAUTH_TXN_NOT_PRESENT_DECLINED_RC",             /* points to section name         */
									  "ORIGINALl_PREAUTH_TXN_NOT_PRESENT_DECLINED_RC_FOR_MC",       	/* points to key name             */
									  "12",  					/* Default string                 */
									  rc_value,              	/* points to destination buffer   */
									  sizeof(rc_value)-1,   	 /* size of destination buffer     */
									  filename                  /* points to ini filename         */
									);

	if(strlen(rc_value)==0)
	{
	   strncpy( declined_rc_for_original_txn_not_present_for_MC,
			    ORIGINAL_TXN_NOT_FOUND_FOR_VOID_PREAUTH_RC,
				ISO_RESPONSE_CODE_LEN );
	}
	else
	{
		strncpy(declined_rc_for_original_txn_not_present_for_MC,rc_value,ISO_RESPONSE_CODE_LEN);
	}

	ret_val = GetPrivateProfileString(
										"ORIGINALl_PREAUTH_TXN_NOT_PRESENT_DECLINED_RC",             /* points to section name         */
										"ORIGINALl_PREAUTH_TXN_NOT_PRESENT_DECLINED_RC_FOR_VISA",       	/* points to key name             */
										"12",  					/* Default string                 */
										rc_value,              	/* points to destination buffer   */
										sizeof(rc_value)-1,   	 /* size of destination buffer     */
										filename                  /* points to ini filename         */
										);

	if(strlen(rc_value)==0)
	{
	   strncpy( declined_rc_for_original_txn_not_present_for_VISA,
			    ORIGINAL_TXN_NOT_FOUND_FOR_VOID_PREAUTH_RC,
				ISO_RESPONSE_CODE_LEN );
	}
	else
	{
		strncpy(declined_rc_for_original_txn_not_present_for_VISA,rc_value,ISO_RESPONSE_CODE_LEN);
	}


	ret_val = GetPrivateProfileString(
										"ORIGINALl_PREAUTH_TXN_NOT_PRESENT_DECLINED_RC",             /* points to section name         */
										"ORIGINALl_PREAUTH_TXN_NOT_PRESENT_DECLINED_RC_FOR_AMEX",       	/* points to key name             */
										"12",  					/* Default string                 */
										rc_value,              	/* points to destination buffer   */
										sizeof(rc_value)-1,   	 /* size of destination buffer     */
										filename                  /* points to ini filename         */
										);
	if(strlen(rc_value)==0)
	{
		strncpy(declined_rc_for_original_txn_not_present_for_AMEX,
				ORIGINAL_TXN_NOT_FOUND_FOR_VOID_PREAUTH_RC,
				ISO_RESPONSE_CODE_LEN );
	}
	else
	{
		strncpy(declined_rc_for_original_txn_not_present_for_AMEX,rc_value,ISO_RESPONSE_CODE_LEN);
	}

	ret_val = GetPrivateProfileString(
										"ORIGINALl_PREAUTH_TXN_NOT_PRESENT_DECLINED_RC",             /* points to section name         */
										"ORIGINALl_PREAUTH_TXN_NOT_PRESENT_DECLINED_RC_FOR_DCI",       	/* points to key name             */
										"12",  					/* Default string                 */
										rc_value,              	/* points to destination buffer   */
										sizeof(rc_value)-1,   	 /* size of destination buffer     */
										filename                  /* points to ini filename         */
										);

	if(strlen(rc_value)==0)
	{
		strncpy(declined_rc_for_original_txn_not_present_for_DCI,
				ORIGINAL_TXN_NOT_FOUND_FOR_VOID_PREAUTH_RC,
				ISO_RESPONSE_CODE_LEN );
	}
	else
	{
		strncpy(declined_rc_for_original_txn_not_present_for_DCI,rc_value,ISO_RESPONSE_CODE_LEN);
	}

	ret_val = GetPrivateProfileString(
									  "ORIGINALl_PREAUTH_TXN_NOT_PRESENT_DECLINED_RC",             /* points to section name         */
									  "ORIGINALl_PREAUTH_TXN_NOT_PRESENT_DECLINED_RC_FOR_JCB",       	/* points to key name             */
									  "12",  					/* Default string                 */
									  rc_value,              	/* points to destination buffer   */
									  sizeof(rc_value)-1,   	 /* size of destination buffer     */
									  filename                  /* points to ini filename         */
									);

	if(strlen(rc_value)==0)
	{
		strncpy(declined_rc_for_original_txn_not_present_for_JCB,
				ORIGINAL_TXN_NOT_FOUND_FOR_VOID_PREAUTH_RC,
				ISO_RESPONSE_CODE_LEN );
	}
	else
	{
		strncpy(declined_rc_for_original_txn_not_present_for_JCB,rc_value,ISO_RESPONSE_CODE_LEN);
	}

	ret_val = GetPrivateProfileString(
										"ORIGINALl_PREAUTH_TXN_NOT_PRESENT_DECLINED_RC",             /* points to section name         */
										"ORIGINALl_PREAUTH_TXN_NOT_PRESENT_DECLINED_RC_FOR_UPI",       	/* points to key name             */
										"12",  					/* Default string                 */
										rc_value,              	/* points to destination buffer   */
										sizeof(rc_value)-1,   	 /* size of destination buffer     */
										filename                  /* points to ini filename         */
										);

	if(strlen(rc_value)==0)
	{
		strncpy(declined_rc_for_original_txn_not_present_for_UPI,
				ORIGINAL_TXN_NOT_FOUND_FOR_VOID_PREAUTH_RC,
				ISO_RESPONSE_CODE_LEN );
	}
	else
	{
		strncpy(declined_rc_for_original_txn_not_present_for_UPI,rc_value,ISO_RESPONSE_CODE_LEN);
	}

	return (ret_val);
}

INT get_maximum_validity_of_preauth_in_days(void)
{
   CHAR  rc_value[3]={0};
   CHAR  date[9]={0};
   INT   ret_val = true;
   CHAR  filename   [80] = {0};
   CHAR  tmpstr     [80] = {0};

   /* Get path to the tf.ini file.*/
   memset( tmpstr, 0x00, sizeof(tmpstr) );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, "tf.ini" );

   ret_val = GetPrivateProfileString(
									  "MAX_PREAUTH_VALIDITY_DAYS",             /* points to section name         */
									  "MAX_PREAUTH_VALIDITY_DAYS_FOR_MC",       	/* points to key name             */
									  "30",  					/* Default string                 */
									  rc_value,              	/* points to destination buffer   */
									  sizeof(rc_value)-1,   	 /* size of destination buffer     */
									  filename                  /* points to ini filename         */
									);

	if(strlen(rc_value)==0)
	{
	   strncpy( maximum_validity_of_preauth_in_days_for_MC,
			   	MAX_PREAUTH_VALIDITY_DAYS,
				MAX_PREAUTH_VALIDITY_DAYS_LEN );
	}
	else
	{
		strncpy(maximum_validity_of_preauth_in_days_for_MC,rc_value,MAX_PREAUTH_VALIDITY_DAYS_LEN);
	}

	ret_val = GetPrivateProfileString(
										"MAX_PREAUTH_VALIDITY_DAYS",             /* points to section name         */
										"MAX_PREAUTH_VALIDITY_DAYS_FOR_VISA",       	/* points to key name             */
										"30",  					/* Default string                 */
										rc_value,              	/* points to destination buffer   */
										sizeof(rc_value)-1,   	 /* size of destination buffer     */
										filename                  /* points to ini filename         */
										);

	if(strlen(rc_value)==0)
	{
	   strncpy( maximum_validity_of_preauth_in_days_for_VISA,
			    MAX_PREAUTH_VALIDITY_DAYS,
				MAX_PREAUTH_VALIDITY_DAYS_LEN );
	}
	else
	{
		strncpy(maximum_validity_of_preauth_in_days_for_VISA,rc_value,MAX_PREAUTH_VALIDITY_DAYS_LEN);
	}


	ret_val = GetPrivateProfileString(
										"MAX_PREAUTH_VALIDITY_DAYS",             /* points to section name         */
										"MAX_PREAUTH_VALIDITY_DAYS_FOR_AMEX",       	/* points to key name             */
										"30",  					/* Default string                 */
										rc_value,              	/* points to destination buffer   */
										sizeof(rc_value)-1,   	 /* size of destination buffer     */
										filename                  /* points to ini filename         */
										);
	if(strlen(rc_value)==0)
	{
		strncpy(maximum_validity_of_preauth_in_days_for_AMEX,
				MAX_PREAUTH_VALIDITY_DAYS,
				MAX_PREAUTH_VALIDITY_DAYS_LEN );
	}
	else
	{
		strncpy(maximum_validity_of_preauth_in_days_for_AMEX,rc_value,MAX_PREAUTH_VALIDITY_DAYS_LEN);
	}

	ret_val = GetPrivateProfileString(
										"MAX_PREAUTH_VALIDITY_DAYS",             /* points to section name         */
										"MAX_PREAUTH_VALIDITY_DAYS_FOR_DCI",       	/* points to key name             */
										"30",  					/* Default string                 */
										rc_value,              	/* points to destination buffer   */
										sizeof(rc_value)-1,   	 /* size of destination buffer     */
										filename                  /* points to ini filename         */
										);

	if(strlen(rc_value)==0)
	{
		strncpy(maximum_validity_of_preauth_in_days_for_DCI,
				MAX_PREAUTH_VALIDITY_DAYS,
				MAX_PREAUTH_VALIDITY_DAYS_LEN );
	}
	else
	{
		strncpy(maximum_validity_of_preauth_in_days_for_DCI,rc_value,MAX_PREAUTH_VALIDITY_DAYS_LEN);
	}

	ret_val = GetPrivateProfileString(
									  "MAX_PREAUTH_VALIDITY_DAYS",             /* points to section name         */
									  "MAX_PREAUTH_VALIDITY_DAYS_FOR_JCB",       	/* points to key name             */
									  "30",  					/* Default string                 */
									  rc_value,              	/* points to destination buffer   */
									  sizeof(rc_value)-1,   	 /* size of destination buffer     */
									  filename                  /* points to ini filename         */
									);

	if(strlen(rc_value)==0)
	{
		strncpy(maximum_validity_of_preauth_in_days_for_JCB,
				MAX_PREAUTH_VALIDITY_DAYS,
				MAX_PREAUTH_VALIDITY_DAYS_LEN );
	}
	else
	{
		strncpy(maximum_validity_of_preauth_in_days_for_JCB,rc_value,MAX_PREAUTH_VALIDITY_DAYS_LEN);
	}

	ret_val = GetPrivateProfileString(
										"MAX_PREAUTH_VALIDITY_DAYS",             /* points to section name         */
										"MAX_PREAUTH_VALIDITY_DAYS_FOR_UPI",       	/* points to key name             */
										"30",  					/* Default string                 */
										rc_value,              	/* points to destination buffer   */
										sizeof(rc_value)-1,   	 /* size of destination buffer     */
										filename                  /* points to ini filename         */
										);

	if(strlen(rc_value)==0)
	{
		strncpy(maximum_validity_of_preauth_in_days_for_UPI,
				MAX_PREAUTH_VALIDITY_DAYS,
				MAX_PREAUTH_VALIDITY_DAYS_LEN );
	}
	else
	{
		strncpy(maximum_validity_of_preauth_in_days_for_UPI,rc_value,MAX_PREAUTH_VALIDITY_DAYS_LEN);
	}

	return (ret_val);
}

#if OCT_MAN_2024_BLD_2
INT get_maximum_validity_of_preauth_in_days_for_smcc(void)
{
   CHAR  rc_value[3]={0};
   CHAR  date[9]={0};
   INT   ret_val = true;
   CHAR  filename   [80] = {0};
   CHAR  tmpstr     [80] = {0};

   /* Get path to the tf.ini file.*/
   memset( tmpstr, 0x00, sizeof(tmpstr) );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, "tf.ini" );

   ret_val = GetPrivateProfileString(
									  "MAX_PREAUTH_VALIDITY_DAYS",             /* points to section name         */
									  "SMCC_MAX_PREAUTH_VALIDITY_DAYS_FOR_MC",       	/* points to key name             */
									  "05",  					/* Default string                 */
									  rc_value,              	/* points to destination buffer   */
									  sizeof(rc_value)-1,   	 /* size of destination buffer     */
									  filename                  /* points to ini filename         */
									);

	if(strlen(rc_value)==0)
	{
	   strncpy( smcc_maximum_validity_of_preauth_in_days_for_MC,
			    SMCC_MAX_PREAUTH_VALIDITY_DAYS,
				SMCC_MAX_PREAUTH_VALIDITY_DAYS_LEN );
	}
	else
	{
		strncpy(smcc_maximum_validity_of_preauth_in_days_for_MC,rc_value,SMCC_MAX_PREAUTH_VALIDITY_DAYS_LEN);
	}

	ret_val = GetPrivateProfileString(
										"MAX_PREAUTH_VALIDITY_DAYS",             /* points to section name         */
										"SMCC_MAX_PREAUTH_VALIDITY_DAYS_FOR_VISA",       	/* points to key name             */
										"05",  					/* Default string                 */
										rc_value,              	/* points to destination buffer   */
										sizeof(rc_value)-1,   	 /* size of destination buffer     */
										filename                  /* points to ini filename         */
										);

	if(strlen(rc_value)==0)
	{
	   strncpy( smcc_maximum_validity_of_preauth_in_days_for_VISA,
			    SMCC_MAX_PREAUTH_VALIDITY_DAYS,
				SMCC_MAX_PREAUTH_VALIDITY_DAYS_LEN );
	}
	else
	{
		strncpy(smcc_maximum_validity_of_preauth_in_days_for_VISA,rc_value,SMCC_MAX_PREAUTH_VALIDITY_DAYS_LEN);
	}


	ret_val = GetPrivateProfileString(
										"MAX_PREAUTH_VALIDITY_DAYS",             /* points to section name         */
										"SMCC_MAX_PREAUTH_VALIDITY_DAYS_FOR_AMEX",       	/* points to key name             */
										"05",  					/* Default string                 */
										rc_value,              	/* points to destination buffer   */
										sizeof(rc_value)-1,   	 /* size of destination buffer     */
										filename                  /* points to ini filename         */
										);
	if(strlen(rc_value)==0)
	{
		strncpy(smcc_maximum_validity_of_preauth_in_days_for_AMEX,
				SMCC_MAX_PREAUTH_VALIDITY_DAYS,
				SMCC_MAX_PREAUTH_VALIDITY_DAYS_LEN );
	}
	else
	{
		strncpy(smcc_maximum_validity_of_preauth_in_days_for_AMEX,rc_value,SMCC_MAX_PREAUTH_VALIDITY_DAYS_LEN);
	}

	ret_val = GetPrivateProfileString(
										"MAX_PREAUTH_VALIDITY_DAYS",             /* points to section name         */
										"SMCC_MAX_PREAUTH_VALIDITY_DAYS_FOR_DCI",       	/* points to key name             */
										"05",  					/* Default string                 */
										rc_value,              	/* points to destination buffer   */
										sizeof(rc_value)-1,   	 /* size of destination buffer     */
										filename                  /* points to ini filename         */
										);

	if(strlen(rc_value)==0)
	{
		strncpy(smcc_maximum_validity_of_preauth_in_days_for_DCI,
				SMCC_MAX_PREAUTH_VALIDITY_DAYS,
				SMCC_MAX_PREAUTH_VALIDITY_DAYS_LEN );
	}
	else
	{
		strncpy(smcc_maximum_validity_of_preauth_in_days_for_DCI,rc_value,SMCC_MAX_PREAUTH_VALIDITY_DAYS_LEN);
	}

	ret_val = GetPrivateProfileString(
									  "MAX_PREAUTH_VALIDITY_DAYS",             /* points to section name         */
									  "SMCC_MAX_PREAUTH_VALIDITY_DAYS_FOR_JCB",       	/* points to key name             */
									  "05",  					/* Default string                 */
									  rc_value,              	/* points to destination buffer   */
									  sizeof(rc_value)-1,   	 /* size of destination buffer     */
									  filename                  /* points to ini filename         */
									);

	if(strlen(rc_value)==0)
	{
		strncpy(smcc_maximum_validity_of_preauth_in_days_for_JCB,
				SMCC_MAX_PREAUTH_VALIDITY_DAYS,
				SMCC_MAX_PREAUTH_VALIDITY_DAYS_LEN );
	}
	else
	{
		strncpy(smcc_maximum_validity_of_preauth_in_days_for_JCB,rc_value,SMCC_MAX_PREAUTH_VALIDITY_DAYS_LEN);
	}

	ret_val = GetPrivateProfileString(
										"MAX_PREAUTH_VALIDITY_DAYS",             /* points to section name         */
										"SMCC_MAX_PREAUTH_VALIDITY_DAYS_FOR_UPI",       	/* points to key name             */
										"05",  					/* Default string                 */
										rc_value,              	/* points to destination buffer   */
										sizeof(rc_value)-1,   	 /* size of destination buffer     */
										filename                  /* points to ini filename         */
										);

	if(strlen(rc_value)==0)
	{
		strncpy(smcc_maximum_validity_of_preauth_in_days_for_UPI,
				SMCC_MAX_PREAUTH_VALIDITY_DAYS,
				SMCC_MAX_PREAUTH_VALIDITY_DAYS_LEN );
	}
	else
	{
		strncpy(smcc_maximum_validity_of_preauth_in_days_for_UPI,rc_value,SMCC_MAX_PREAUTH_VALIDITY_DAYS_LEN);
	}

	return (ret_val);
}

INT get_mcc_of_preauth_in_days_for_smcc(void)
{
   CHAR  rc_value[5]={0};
   INT   ret_val = true;
   CHAR  filename   [80] = {0};
   CHAR  tmpstr     [80] = {0};

   /* Get path to the tf.ini file.*/
   memset( tmpstr, 0x00, sizeof(tmpstr) );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, "tf.ini" );

   ret_val = GetPrivateProfileString(
									  "MID_MCC",             /* points to section name         */
									  "MID_MCC_01",       	/* points to key name             */
									  "",  					/* Default string                 */
									  rc_value,              	/* points to destination buffer   */
									  sizeof(rc_value)-1,   	 /* size of destination buffer     */
									  filename                  /* points to ini filename         */
									);
   strncpy(smcc_id1,rc_value,MID_MCC_LEN);

   ret_val = GetPrivateProfileString(
   									  "MID_MCC",             /* points to section name         */
   									  "MID_MCC_02",       	/* points to key name             */
   									  "",  					/* Default string                 */
   									  rc_value,              	/* points to destination buffer   */
   									  sizeof(rc_value)-1,   	 /* size of destination buffer     */
   									  filename                  /* points to ini filename         */
   									);
   strncpy(smcc_id2,rc_value,MID_MCC_LEN);

   ret_val = GetPrivateProfileString(
   									  "MID_MCC",             /* points to section name         */
   									  "MID_MCC_03",       	/* points to key name             */
   									  "",  					/* Default string                 */
   									  rc_value,              	/* points to destination buffer   */
   									  sizeof(rc_value)-1,   	 /* size of destination buffer     */
   									  filename                  /* points to ini filename         */
   									);
   strncpy(smcc_id3,rc_value,MID_MCC_LEN);

   ret_val = GetPrivateProfileString(
   									  "MID_MCC",             /* points to section name         */
   									  "MID_MCC_04",       	/* points to key name             */
   									  "",  					/* Default string                 */
   									  rc_value,              	/* points to destination buffer   */
   									  sizeof(rc_value)-1,   	 /* size of destination buffer     */
   									  filename                  /* points to ini filename         */
   									);
   strncpy(smcc_id4,rc_value,MID_MCC_LEN);

   ret_val = GetPrivateProfileString(
   									  "MID_MCC",             /* points to section name         */
   									  "MID_MCC_05",       	/* points to key name             */
   									  "",  					/* Default string                 */
   									  rc_value,              	/* points to destination buffer   */
   									  sizeof(rc_value)-1,   	 /* size of destination buffer     */
   									  filename                  /* points to ini filename         */
   									);
   strncpy(smcc_id5,rc_value,MID_MCC_LEN);

   ret_val = GetPrivateProfileString(
   									  "MID_MCC",             /* points to section name         */
   									  "MID_MCC_06",       	/* points to key name             */
   									  "",  					/* Default string                 */
   									  rc_value,              	/* points to destination buffer   */
   									  sizeof(rc_value)-1,   	 /* size of destination buffer     */
   									  filename                  /* points to ini filename         */
   									);
   strncpy(smcc_id6,rc_value,MID_MCC_LEN);

   ret_val = GetPrivateProfileString(
   									  "MID_MCC",             /* points to section name         */
   									  "MID_MCC_07",       	/* points to key name             */
   									  "",  					/* Default string                 */
   									  rc_value,              	/* points to destination buffer   */
   									  sizeof(rc_value)-1,   	 /* size of destination buffer     */
   									  filename                  /* points to ini filename         */
   									);
   strncpy(smcc_id7,rc_value,MID_MCC_LEN);

   ret_val = GetPrivateProfileString(
   									  "MID_MCC",             /* points to section name         */
   									  "MID_MCC_08",       	/* points to key name             */
   									  "",  					/* Default string                 */
   									  rc_value,              	/* points to destination buffer   */
   									  sizeof(rc_value)-1,   	 /* size of destination buffer     */
   									  filename                  /* points to ini filename         */
   									);
   strncpy(smcc_id8,rc_value,MID_MCC_LEN);

   ret_val = GetPrivateProfileString(
   									  "MID_MCC",             /* points to section name         */
   									  "MID_MCC_09",       	/* points to key name             */
   									  "",  					/* Default string                 */
   									  rc_value,              	/* points to destination buffer   */
   									  sizeof(rc_value)-1,   	 /* size of destination buffer     */
   									  filename                  /* points to ini filename         */
   									);
   strncpy(smcc_id9,rc_value,MID_MCC_LEN);

   ret_val = GetPrivateProfileString(
   									  "MID_MCC",             /* points to section name         */
   									  "MID_MCC_10",       	/* points to key name             */
   									  "",  					/* Default string                 */
   									  rc_value,              	/* points to destination buffer   */
   									  sizeof(rc_value)-1,   	 /* size of destination buffer     */
   									  filename                  /* points to ini filename         */
   									);
   strncpy(smcc_id10,rc_value,MID_MCC_LEN);

}
#endif

INT get_flag_value_for_sending_partial_reversals(void)
{
   CHAR  pr_flag[2]={0};
   CHAR  date[9]={0};
   INT   ret_val = true;
   CHAR  filename   [80] = {0};
   CHAR  tmpstr     [80] = {0};

   /* Get path to the tf.ini file.*/
   memset( tmpstr, 0x00, sizeof(tmpstr) );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, "tf.ini" );

   ret_val = GetPrivateProfileString(
									  "PARTIAL_REVERSAL_FLAG",             /* points to section name         */
									  "PARTIAL_REVERSAL_FLAG_FOR_MC",       	/* points to key name             */
									  "0",  					/* Default string                 */
									  pr_flag,              	/* points to destination buffer   */
									  sizeof(pr_flag)-1,   	 /* size of destination buffer     */
									  filename                  /* points to ini filename         */
									);

	if(strlen(pr_flag)==0)
	{
	   strncpy( flag_for_sending_partial_reversals_for_MC,
			    PARTIAL_REVERSAL_FLAG_OFF,
				PARTIAL_REVERSAL_FLAG_LEN );
	}
	else
	{
		strncpy(flag_for_sending_partial_reversals_for_MC,pr_flag,PARTIAL_REVERSAL_FLAG_LEN);
	}

	ret_val = GetPrivateProfileString(
										"PARTIAL_REVERSAL_FLAG",             /* points to section name         */
										"PARTIAL_REVERSAL_FLAG_FOR_VISA",       	/* points to key name             */
										"0",  					/* Default string                 */
										pr_flag,              	/* points to destination buffer   */
										sizeof(pr_flag)-1,   	 /* size of destination buffer     */
										filename                  /* points to ini filename         */
										);

	if(strlen(pr_flag)==0)
	{
	   strncpy( flag_for_sending_partial_reversals_for_VISA,
			    PARTIAL_REVERSAL_FLAG_OFF,
				PARTIAL_REVERSAL_FLAG_LEN );
	}
	else
	{
		strncpy(flag_for_sending_partial_reversals_for_VISA,pr_flag,PARTIAL_REVERSAL_FLAG_LEN);
	}


	ret_val = GetPrivateProfileString(
										"PARTIAL_REVERSAL_FLAG",             /* points to section name         */
										"PARTIAL_REVERSAL_FLAG_FOR_AMEX",       	/* points to key name             */
										"0",  					/* Default string                 */
										pr_flag,              	/* points to destination buffer   */
										sizeof(pr_flag)-1,   	 /* size of destination buffer     */
										filename                  /* points to ini filename         */
										);
	if(strlen(pr_flag)==0)
	{
		strncpy(flag_for_sending_partial_reversals_for_AMEX,
				PARTIAL_REVERSAL_FLAG_OFF,
				PARTIAL_REVERSAL_FLAG_LEN );
	}
	else
	{
		strncpy(flag_for_sending_partial_reversals_for_AMEX,pr_flag,PARTIAL_REVERSAL_FLAG_LEN);
	}

	ret_val = GetPrivateProfileString(
										"PARTIAL_REVERSAL_FLAG",             /* points to section name         */
										"PARTIAL_REVERSAL_FLAG_FOR_DCI",       	/* points to key name             */
										"0",  					/* Default string                 */
										pr_flag,              	/* points to destination buffer   */
										sizeof(pr_flag)-1,   	 /* size of destination buffer     */
										filename                  /* points to ini filename         */
										);

	if(strlen(pr_flag)==0)
	{
		strncpy(flag_for_sending_partial_reversals_for_DCI,
				PARTIAL_REVERSAL_FLAG_OFF,
				PARTIAL_REVERSAL_FLAG_LEN );
	}
	else
	{
		strncpy(flag_for_sending_partial_reversals_for_DCI,pr_flag,PARTIAL_REVERSAL_FLAG_LEN);
	}

	ret_val = GetPrivateProfileString(
									  "PARTIAL_REVERSAL_FLAG",             /* points to section name         */
									  "PARTIAL_REVERSAL_FLAG_FOR_JCB",       	/* points to key name             */
									  "0",  					/* Default string                 */
									  pr_flag,              	/* points to destination buffer   */
									  sizeof(pr_flag)-1,   	 /* size of destination buffer     */
									  filename                  /* points to ini filename         */
									);

	if(strlen(pr_flag)==0)
	{
		strncpy(flag_for_sending_partial_reversals_for_JCB,
				PARTIAL_REVERSAL_FLAG_OFF,
				PARTIAL_REVERSAL_FLAG_LEN );
	}
	else
	{
		strncpy(flag_for_sending_partial_reversals_for_JCB,pr_flag,PARTIAL_REVERSAL_FLAG_LEN);
	}

	return (ret_val);
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
   sprintf( strTemp,
           "Starting the Service: %s, version %s",
            ServiceName, Version );
   dcpiso_log_message( 2, 1, strTemp,"MainProcessor", 0 );
   printf( "%s\n", strTemp );


   rc = read_tf_ini( strError );
   rc=  read_mra_send_flag(strError);
   rc=  read_private_lable_flag () ;
   rc = read_percent_overlimit_value_for_VISA();
   rc = read_percent_overlimit_value_for_MC();
   rc = read_percent_overlimit_value_for_JCB();
   rc = read_percent_overlimit_value_for_AMEX();
   rc = read_percent_overlimit_value_for_CUP();
   rc = read_percent_overlimit_value_for_DINERS();
   rc = read_PREAUTH_COMPLETION_VALIDATION_Eror_Response_code();
   rc = read_emn_contactless_offline_sale_authID();
   rc=	read_Incremental_Auth_flag_CUP();
   rc = read_validate_preauth_validation_processing_code();
   rc = dcpiso_read_CUP_merchant_indicator_dcpiso();
   rc = dcpiso_read_TO_RC_for_card_brands();
   rc =  dcpiso_read_FAI_RC_for_card_brands();
   rc = get_MRACON_state_from_ini();
   rc = dcpiso_read_POS_default_system_response_code();
   dcpiso_get_error_warning_file_name_path();
   get_variable_auth_config_details();

#if BDOR_62_PREAUTH_VOID
	rc=get_declined_RC_for_insufficient_Preauth();
	rc=get_declined_rc_for_expired_Used_preauths();
	rc=get_declined_rc_for_original_txn_not_present();
	rc=get_maximum_validity_of_preauth_in_days();
	rc=get_maximum_validity_of_preauth_in_days_for_smcc();
	rc=get_mcc_of_preauth_in_days_for_smcc();
	rc=get_flag_value_for_sending_partial_reversals();
#endif

   #ifdef _DEBUG
      pteipc_trace_on(); 
   #endif

   if(!pteipc_init_multiple_instance_app (strAppName, ServiceName, strInstanceName))
   {
      strcpy (strError, "Failed to create XIPC queues");
      dcpiso_log_message( 1, 3, strError,"MainProcessor",0 );

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
         dcpiso_log_message( 1, 2, strError,"MainProcessor",0 );
      }
      else
      {
      	  dcpiso_log_message( 2, 1, strError,"MainProcessor",0 );
      }
   }
   else
   {
      Tracing = TRACE_OFF;
      if ( rc == -1 )
      {
         dcpiso_log_message( 1, 1, strError,"MainProcessor",0 );
      }
      dcpiso_log_message( 1, 1, "Tracing is turned off","MainProcessor",0 );
   }

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
         dcpiso_log_message( 1, 2, strError,"MainProcessor",0 );
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
            dcpiso_log_message( 1, 2, strError,"MainProcessor",0 );
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
                  dcpiso_log_message( 1, 1, buffer,"PinnacleMsgHandler",0 );
                  (void)close_trace_file( ppTraceFile_fp );
               }
            }

            /* When pinged, display application version on Monitor. */
            sprintf( buffer,
                    "Pinged -> Device Controller Service: %s, version %s",
                     ServiceName, Version );
            dcpiso_log_message( 1, 1, buffer,"PinnacleMsgHandler",0 );
         }
		 if( ST1_SRVC_MRAON == ptemsg_get_msg_subtype1(p_msg_in) ) 
	     {
			 nMra_enable_flag = 1;
			send_trace_response(p_msg_in);
			sprintf( buffer,
                    "MRA enabled for %s, version %s",
                     ServiceName, Version );
            dcpiso_log_message( 1, 1, buffer,"PinnacleMsgHandler",0 );
	     }
		 if( ST1_SRVC_MRAOFF == ptemsg_get_msg_subtype1(p_msg_in) ) 
	     {
			 nMra_enable_flag = 0;
			send_trace_response(p_msg_in);
			sprintf( buffer,
                    "MRA disabled for %s, version %s",
                     ServiceName, Version );
            dcpiso_log_message( 1, 1, buffer,"PinnacleMsgHandler",0 );

	     }
		 if(ST1_LOAD_TO_RC_VALUES == ptemsg_get_msg_subtype1(p_msg_in))
		 {
			 dcpiso_read_TO_RC_for_card_brands();
			 send_trace_response(p_msg_in);
			 sprintf( buffer,
			                     "TO rc for card brands, AMEX: %s,DCI:%s,CUP:%s,JCB:%s,MC:%s,VISA:%s",
								 amex_timeout_rc, dci_timeout_rc,cup_timeout_rc,jcb_timeout_rc,mc_timeout_rc,visa_timeout_rc);
			 dcpiso_log_message( 1, 1, buffer,"PinnacleMsgHandler",0 );
		 }
		 if(ST1_LOAD_RC_FOR_FAI == ptemsg_get_msg_subtype1(p_msg_in))
		 {
			 dcpiso_read_FAI_RC_for_card_brands();
			 send_trace_response(p_msg_in);
			 sprintf( buffer,
								 "FAI decline rc for card brands, AMEX: %s,DCI:%s,CUP:%s,JCB:%s,MC:%s,VISA:%s",
								 amex_fai_rc, dci_fai_rc,cup_fai_rc,jcb_fai_rc,mc_fai_rc,visa_fai_rc);
			 dcpiso_log_message( 1, 1, buffer,"PinnacleMsgHandler",0 );
		 }
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
            	 if((0 == strncmp(auth_tx.TLF01_details.message_type,"0220",4)) &&
            	    ((true == Check_If_Transaction_qualifies_for_Preauth_validation())||
					 (true == dcpiso_check_If_Tra_qualify_MC_512_incremental_Preauth())||
					 (true == dcpiso_Check_If_CUP_Transaction())))
                {
            		 // Dont change the Response code . Let it be as is it. It might be have failed.
                }
            	else
            	{
            		strcpy( auth_tx.TLF01_details.response_code,
            				TRANSACTION_APPROVED) ;
            	}
            }
            Send_Transaction_To_Originator () ;
         }
      break;

	  case MT_INCOMING_DCPIMP:
	  	retCode = Process_incoming_dcpimp_message(p_msg_in);
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

	  case MT_INCOMING_DCPOS:
	  	  	retCode = Process_incoming_dcpos_message(p_msg_in);
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

      default:
         dcpiso_log_message( 1, 2, "unknown msg_type received","PinnacleMsgHandler", 0 );
      break;
   }  /* switch msg_type */
} /* PinnacleMsgHandler */
/******************************  END OF FILE  ****************************************************/
