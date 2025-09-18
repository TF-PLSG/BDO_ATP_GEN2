/******************************************************************************

   Module: trands.c

   Title: Transaction Server Dataserver Routines

   Description: 

   $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\trands\tx_ds.c  $1025

     Rev 1.67   Jan 05 2011 TF - Srikanth
   Added code for UCF03 table contains additional fields to UCF01 table
   for PCI Changes.

      Rev 1.66   Jun 05 2007  DIrby
   Added code to create the AuthNumber table in Xipc shared memory
   at startup, and added an SQL to update the UAG01 table.
   SCR 12299

      Rev 1.65   Mar 12 2007 16:17:28   DIrby
   Modified the ping logic to just respond to the ping when subtype2
   is ST2_NO_VERSION.
   SCR 24081

      Rev 1.64   Mar 08 2007 10:28:44   DIrby
   Fixed bug when checking subtype 2 on a ping; changed '==' to '!='.
   SCR 24081
   
      Rev 1.63   Mar 02 2007 11:31:18   DIrby
   Added support for every SQL to be stored into shared memory
   in table SQL01 prior to being executed.
   SCR 24081
   
      Rev 1.62   Feb 09 2007 13:48:08   DIrby
   Modified the Auth ID Generation function to support numeric-only
   auth numbers.  Subtype2 is used to determine if auth number
   should be only numeric or the normal Alphanumeric.
   SCR 22836
   
      Rev 1.61   Jan 16 2007 10:54:52   DIrby
   Updated version to 4.4.1.6
   SCR 24046
   
      Rev 1.60   Sep 13 2006 16:06:46   dirby
   Modified BON02 to make multi-winner response common to all
   BON02 records.  
   SCR 22835
   
      Rev 1.59   Sep 05 2006 16:59:24   dirby
   Added startup code to cache table BON02 into Oracle memory.
   SCR 22835
   
      Rev 1.58   Sep 05 2006 10:26:42   dirby
   Added functionality for the GUI to interface with BON02.
   SCR 22385
   
      Rev 1.57   Aug 30 2006 14:45:14   dirby
   Added function to get a GUI list for BON02.
   SCR 22835
   
      Rev 1.56   Aug 24 2006 17:17:38   dirby
   Added support for BON02 in shared memory and for GUI access.
   SCR 22835
   
      Rev 1.55   Jul 07 2006 13:13:58   dirby
   Implemented functionality to gather statistics on database SQL calls.
   SCR 12299
   
      Rev 1.54   Jun 25 2005 16:39:24   dirby
   Added support for Fraudguard.
   SCR 14421
   
      Rev 1.53   Apr 06 2005 14:25:18   lmitchel
   Add request to support deletion of all MCC ranges for a card
   
      Rev 1.52   Apr 05 2005 16:00:22   dirby
   Updated version to 4.4.1.1
   SCR 12785
   
      Rev 1.51   Apr 01 2005 10:30:34   lmitchel
   Initial Checking prior to integration testing for updates made to support country/curreny code limits (scr13929), unlimited purchase card MCC ranges, and purchase card transaction limits(scr12785).
   
      Rev 1.50   Jul 08 2004 17:20:56   dirby
   Updated to 4.4.0.1
   SCRs 1287 & 1388
   
      Rev 1.49   Jun 17 2004 11:05:02   dirby
   Updated version to 4.3.0.2
   SCR 1397
   
      Rev 1.48   May 28 2004 13:47:00   dirby
   Added Negative Fraud and MCC Limits queries.
   SCR 1373 and 1380
   
      Rev 1.47   May 23 2003 12:06:08   dirby
   Updated version to 4.2.0.2
   SCR 1111
   
      Rev 1.46   Feb 17 2003 10:12:08   dirby
   Updated version to 4.2.0.1
   SCR 955
   
      Rev 1.45   Sep 13 2002 17:07:00   dirby
   Updated version to 4.1.1.2
   Put table BON01 into shared memory.
   SCR 804
   
      Rev 1.44   Aug 16 2002 17:48:54   dirby
   Updated version to 4.1.1.1
   SCR 255
   
      Rev 1.43   Jul 18 2002 15:21:54   dirby
   Added functionality to support timing of queries and report on
   any that take too long.  This is configurable in TF.INI.
   SCR 818
   
      Rev 1.42   Jul 03 2002 14:10:20   dirby
   1.  Updated version to 4.0.3.1
   2.  Added code to display version when pinged.
   SCR 803
   
      Rev 1.41   May 01 2002 11:18:16   dirby
   Updated version to 4.0.2.1
   SCR 680
   
      Rev 1.40   Oct 16 2001 14:01:08   dirby
   Updated version to 4.0.0.1     SCRs 403 and 361
   
      Rev 1.39   Jul 11 2001 11:59:04   dirby
   Updated version to 3.2.3.1
   
      Rev 1.38   Feb 14 2001 16:41:34   dirby
   Updated version to 3.2.2.8
   
      Rev 1.37   Jan 09 2001 14:16:50   dirby
   Updated version number.  This marks the release where the production
   stability issue has been resolved.  Some debug code has been removed and
   added #ifndef around some Unix signal code so the data server will compile
   on NT.
   
      Rev 1.36   Oct 24 2000 14:17:14   dirby
   Updated version to 3.2.2.6 - TEST.
   
      Rev 1.35   Oct 24 2000 14:01:24   lmitchel
   Restore correct version 1.33;  disregard revision 1.34 checked-in in error
   
      Rev 1.33   Oct 16 2000 12:28:16   dirby
   1.  Updated version number to 3.2.2.5 - TEST.
   2.  Added code to support XipcSystemErrReport in the event of XIPC problems.
   3.  Removed catching of OS signals CHILD and HangUP so the daemon process can catch them.
   
      Rev 1.32   Oct 03 2000 15:22:40   dirby
   Updated version number from 3.2.2.3 - test to 3.2.2.4 - test.
   
      Rev 1.31   Sep 01 2000 16:19:42   dirby
   Added TrapFunction to catch Unix signals.  This will not compile under NT.
   
      Rev 1.30   Aug 31 2000 17:30:22   dirby
   Appended "TEST" to the version number to account for some
   daemon testing getting linked in.
   
      Rev 1.29   Aug 25 2000 14:51:50   dirby
   Updated the version number to 3.2.2.3
   
      Rev 1.28   Aug 18 2000 10:05:32   dirby
   Updated version number to 3.2.2.2
   
      Rev 1.27   Aug 10 2000 15:58:00   dirby
   Updated version number to 3.2.2.1
   
      Rev 1.26   Jul 30 2000 15:37:10   dirby
   Modified to display version number at startup and shutdown.
   
      Rev 1.25   Jun 27 2000 12:56:38   dirby
   Removed some debug print statements in order to
   make it more efficient.
   
      Rev 1.24   Apr 13 2000 14:28:24   iarustam
   changed bin01 table, added offus_velocity table
   
      Rev 1.23   Nov 30 1999 17:21:16   iarustam
   added function for gui to retrieve crf01 list
   
      Rev 1.22   Nov 24 1999 15:36:46   iarustam
   added floot limit and added function 
   to delete bpf01 by profile id
   
      Rev 1.21   Sep 01 1999 14:14:44   egable
   Modified code such that on startup,  the dataserver writes the OPMON table to shared memory using WriteMemMontorTable () rather than WriteTable()
   This was originally fixed by S. Dasgupta on BP.
   
      Rev 1.20   Aug 17 1999 17:25:58   iarustam
   called init_multi_instance() instead of init_single_instance
   
      Rev 1.19   Jul 28 1999 17:09:30   sdasgupt
   Modified the function that checks the BIN02 table
   
      Rev 1.18   Jun 04 1999 17:11:44   iarustam
   Added new function for def01_gui_list_by_merchant_and_org_id
   
      Rev 1.17   May 26 1999 15:46:20   sdasgupt
   Modified code so that the all_data function will be also called for PIN_TRIES_EXCEEDED (75)
   
      Rev 1.16   May 18 1999 16:21:08   sdasgupt
    
   
      Rev 1.15   May 10 1999 15:29:18   sdasgupt
   Fixed bug where pvki was not being inserted into ccf02p
   
      Rev 1.14   Apr 15 1999 12:39:38   sdasgupt
   Added logic to update deferred account
   
      Rev 1.13   Mar 20 1999 08:58:32   iarustam
   Changes for GUI lists
   
      Rev 1.12   Mar 08 1999 17:14:38   iarustam
   Added def01_gui_list function
   
      Rev 1.11   Mar 02 1999 15:46:36   iarustam
   Added case for delete_ucf02 function
   
      Rev 1.10   Mar 02 1999 15:44:46   iarustam
   Added case for delete_ucf01 function
   
      Rev 1.9   Feb 25 1999 18:43:46   sdasgupt
    
   
      Rev 1.8   Feb 21 1999 18:04:36   sdasgupt
   Bug Fixes
   
      Rev 1.7   Feb 19 1999 17:12:30   sdasgupt
    
   
      Rev 1.6   Feb 17 1999 17:07:06   sdasgupt
   Modified all data functionality to incorporate BON01 table
   
      Rev 1.5   Feb 17 1999 09:51:48   iarustam
   Added acf01_gui_list and ncf04_gui_list
   
      Rev 1.4   Feb 15 1999 15:20:12   sdasgupt
   Added functions for new tables
   Removed ISO_MSG_LENGTH
   
      Rev 1.3   Feb 05 1999 18:06:52   sdasgupt
    
   
      Rev 1.2   01 Feb 1999 14:11:06   jhapitas
   Removed reference to NMF structure
   from the ALL_DATA case.
   
      Rev 1.1   01 Feb 1999 11:01:46   jhapitas
   Changes to include transaction control
   only code.
   
      Rev 1.0   25 Jan 1999 15:21:00   jhapitas
   Initial Revision
   
      Rev 1.0   17 Dec 1998 17:18:38   jhapitas
   Initial Equitable Revision

******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/timeb.h>
#include <signal.h>

#include "basictyp.h"
#include "pte.h"
#include "pteipc.h"
#include "ptemsg.h"
#include "ptestats.h"
#include "ptesystm.h"
#include "ptetimer.h"
#include "ptetime.h"
#include "ntutils.h"
#include "logger.h"

#include "dbcommon.h"
#include "tx_database.h"
#include "memmnger.h"
#include "dstimer.h"
#include "txutils.h"
#include "timings.h"

#include "trands.h"

#define DB_ARCH_PASSWORD_DATA 184
#define DB_MRA_PASSWORD_DATA 185

/* Reading data from tf.ini*/
#define   MAX_APPNAME_SIZE   8
#define   MAX_FILENAME_SIZE  80
#define   MAX_INI_INFO_SIZE  20
/* TF INI File Name */
#define  TF_INI_FILE   "tf.ini"

#define VISA7 1
#define MC12_OCT2023 1
#define MC13_OCT2023 1

#ifdef HCTS2
BYTE card_activation_flag;
BYTE card_updating_status [5]      =  {0};
BYTE card_current_status [51][5]   =  {0};
INT  total_card_current_status     =  0;
void trands_read_atp_card_activation_parameters();
#endif

/*Check null*/
#define MODE_INSERT "INSERT"
#define MODE_UPDATE "UPDATE"

#define NOT_CONNECTED_TO_DB             -3114
#define NOT_LOGGED_INTO_DB              -1012
#define TNS_PACKET_WRITER_FAILED        -12571
#define SHUTDOWN_OR_STARTUP_IN_PROGRESS -1033
/*   GLOBAL VARIABLES    */
extern CHAR ServiceName[];
extern int volatile EndProcessSignalled;
extern int volatile MainProcessDone;

/* For DB statics added by raghu */
BYTE DB_MsgSubtype2 ;
BYTE DB_MsgSubtype1 ;
CHAR DB_Logging_Filename[256];
CHAR  DB_module_file_name[256];
CHAR  DB_file_path[256];

FILE  *HstFilePtr;
INT fprint_retval = 0;
CHAR  HST01_Filename[256] ={0};
CHAR  trands_hst01_file_path[256] = {0};
CHAR masked_fields[151] = {0};
/* End   */
/*This variable will get the value from the tf.ini
 * from the section DATASERVER with key name DB_ERROR_STATICS_FLAG
 * */
INT db_error_statics_flag;
double db_error_statics_value;
INT DB_NullCheck_Flag = 0;

UINT Max_File_Size_Defined = 0 ;

CHAR User_status_Control_Report_file_name[256]	= {0};
//CHAR response_code_for_DUAL_BRAND_DEBIT_TRANSACTION[3]={0};
CHAR Bin01_Tablle_Size[5]={0};

AUTH_TX auth_tx ;




/* Function protype */
BYTE process_alldata_msg( pPTE_MSG  );
pPTE_MSG process_check_data_in_tables( pPTE_MSG );
BYTE process_db_password_change_msg( pDbPasswords,pCHAR);
BYTE process_db_select_dbpasswords( pDbPasswords dbpass_ptr, pCHAR ErrorMsg );
BYTE process_arch_db_password_change_msg( pArchDbPasswords,pCHAR);
BYTE process_arch_db_select_dbpasswords( pArchDbPasswords dbpass_ptr, pCHAR ErrorMsg );
BYTE process_mra_db_password_change_msg( pMraDbPasswords,pCHAR);
BYTE process_mra_db_select_dbpasswords( pMraDbPasswords dbpass_ptr, pCHAR ErrorMsg );
void process_db_cleanup( pPTE_MSG ); 
void TrapFunction( INT signum );
BYTE db_cache_bon02( pCHAR  ErrorMsg );
BOOLEAN SetArchPassword(pCHAR arch_password);
BOOLEAN GetArchPassword(pCHAR arch_password);
BOOLEAN GetArchUserId(pCHAR arch_user_id);
BOOLEAN SetMraPassword(pCHAR mra_password);
BOOLEAN GetMraPassword(pCHAR mra_password);
BOOLEAN GetMraUserId(pCHAR mra_user_id);
void strTrim_For_Mid_blocking (char* strSource, char* strDestination);

//void CheckNULLTerminatedinUCF01(pUCF01 ucf01);
void CheckNULLTerminatedinGPCF01 (pGPCF01 gpcf01,pCHAR mode);
void CheckNULLTerminatedinAMEX3DS2 (pAMEX3DS2 amex3ds2,pCHAR mode);
void CheckNULLTerminatedinCUP3DS2 (pCUP3DS2 cup3ds2,pCHAR mode);
void CheckNULLTerminatedinDCI3DS2 (pDCI3DS2 dci3ds2,pCHAR mode);
//void checknull(pCHAR data , INT Len);


INT get_dataserver_ini_db_error_statics_value(pINT  ds_timer_flag,
                                   double *  ds_timer_value,
                                   pCHAR appname, pCHAR errbuf );
void Create_Db_Statics_And_Db_Oracle_Error_Filename();

void Log_Db_Statics_And_Oracle_Db_Error_To_File(pCHAR Error_Buf);
void trands_log_message( INT dest, INT sev, pCHAR msg, pCHAR func);
INT trands_Log_error_warning_to_File(pCHAR Error_Buf,int sev,pCHAR func);
void Create_userstatuscontrol_tool_report();

PRIVATE union
{
   UCF01   ucf01;
   UCF03   ucf03;/* Added ucf03 table for PCI changes-TF Srikanth  */
   DbPasswords dbpass;/*Added Db password structure for DB password changes in tf.ini file -TF Srikanth*/
   UCF02   ucf02;
   BIN01   bin01;
   HST01   hst01;
   ATP01   atp01;
   ATP02   atp02;
   CCF02P  ccf02p;
   CCF04   ccf04;
   CCF10   ccf10;
   BON01   bon01;
   BON02   bon02;
   CCF01   ccf01;
   CAF01   caf01;
   CTF01   ctf01;
   MCT01   mct01;
   CCF03   ccf03p;
   PAF01   paf01;
   DEF01   def01;
   CSF01   csf01;
   CRF01   crf01;
   ACF01   acf01;
   FAF01   faf01;
   CLF01   clf01;
   BPF01   bpf01;
   JBF01   jbf01;
   APF01   apf01;
   FCF02   fcf02;
   MCF01   mcf01;
   TLF01   tlf01;
   UAG01   uag01;  /* Authorization Id table       */
   BIN02   bin02;  /* Table that has the PLUS data */
   OMC01   omc01;  /* Override Merchant Control 	*/
   MCC01   mcc01;	/* MCC codes table */
   CAF01_EMV caf01_emv;
   Offus_velocity      offus_velocity;
   NEGATIVE_FRAUD      negative_fraud;
   MCC_LIMITS          mcc_limits;
   COUNTRY_AMT_LIMITS  country_amt_limits;
   FG_RAW              fgraw;
   ArchDbPasswords dbpass1;
   MraDbPasswords mra_dbpass;
   MPPARA01 mppara01;
   MIDBINBLK01 midbinblk01;
   GPCF01   gpcf01;
   CUP3DS2  cup3ds2;
   KEY3DS01 key3ds01;
   AMEX3DS2  amex3ds2;
   DCI3DS2 dci3ds2;
   JCB3DS2 jcb3ds2;
   CR01    cr01;
   MCF03P  mcf03p;
   CCL01  ccl01;
   BDKKEY01 bdkkey01;
   CAF01_KEYBLOCK caf01_keyblock01;
   #ifdef VISA7
   MERCH_INT_PARAMS merch_int_param;
   #endif
} db_recs;

PRIVATE union
{
   UCF01_LIST          Ucf01_List;
   UCF03_LIST          Ucf03_List;/*Added UCF03 Table for PCI Changes by TF-Srikanth */
   UCF02_LIST          Ucf02_List;
   HST01_LIST          Hst01_List;
   BIN01_LIST          Bin01_List;
   BON01_LIST          Bon01_List;
   BON02_LIST          Bon02_List;
   CAF01_LIST          Caf01_List;
   CRF01_LIST          Crf01_List;
   FAF01_LIST          Faf01_List;
   CSF01_LIST          Csf01_List;
   ATP02_LIST          Atp02_List;
   BIN01_GUI_LIST      Bin01_Gui_List;
   CCF02P_GUI_LIST     Ccf02p_Gui_List;
   OMC01_GUI_LIST      Omc01_Gui_List;
   CCF10_GUI_LIST      Ccf10_Gui_List;
   ATP02_GUI_LIST      Atp02_Gui_List;
   CAF01_GUI_LIST      Caf01_Gui_List;
   CTF01_GUI_LIST      Ctf01_Gui_List;
   CTF01_LIST          Ctf01_List;
   APF01_GUI_LIST      Apf01_Gui_List;
   JBF01_GUI_LIST      Jbf01_Gui_List;
   CRF01_GUI_LIST      Crf01_Gui_List;
   BPF01_SCH_LIST      Bpf01_Sch_List;
   BPF01_GUI_LIST      Bpf01_Gui_List;
   PAF01_GUI_LIST      Paf01_Gui_List;
   CSF01_GUI_LIST      Csf01_Gui_List;
   MCT01_GUI_LIST      Mct01_Gui_List;
   ACF01_GUI_LIST      Acf01_Gui_List;
   CCF04_GUI_LIST      Ccf04_Gui_List;
   DEF01_GUI_LIST      Def01_Gui_List;
   MCC_LIMITS_GUI      MCC_Limits_Gui_List;
   COUNTRY_AMT_LIMITS_GUI		Country_Amt_Gui_List;
   FGRAW_GUI_LIST      Fgraw_Gui_List;
   BON02_GUI_LIST      Bon02_Gui_List;
   MCC01_GUI_LIST	   Mcc01_Gui_List;
   MCC01_LIST		   Mcc01_List;	
   MPPARA01_GUI_LIST 	Mppara01_Gui_List;
   JCB3DS2_GUI_LIST  	JCB3DS2_Gui_List;
   CR01_GUI_LIST     	CR01_Gui_List;
   CCL01_GUI_LIST		CCL01_Gui_List;
   BDKKEY01_GUI_LIST	BDKKEY01_Gui_List;
   CAF01_KEYBLOCK_GUI_LIST CAF01_KEYBLOCK01_Gui_List;

}db_lists;

INT DB_bin01_count;

PRIVATE LONG         Bin_Count;
PRIVATE LONG         Bon_Count;
PRIVATE LONG         Caf_Count;
PRIVATE LONG         Faf_Count;
PRIVATE LONG         Crf_Count;
PRIVATE LONG         Csf_Count;
PRIVATE BOOLEAN      Bin01InMem = false;
PRIVATE BOOLEAN      Bon01InMem = false;
PRIVATE BOOLEAN      Bon02InMem = false;
PRIVATE BOOLEAN      Caf01InMem = false;
PRIVATE BOOLEAN      Faf01InMem = false;
PRIVATE BOOLEAN      Crf01InMem = false;
PRIVATE BOOLEAN      Csf01InMem = false;
PRIVATE BOOLEAN      OprMonInMem= false;  
PRIVATE BOOLEAN      TranDetailInMem = false;
PRIVATE LONG         MemSize;
PRIVATE LONG         MemAvailable;
PRIVATE LONG  		 BIN01_SHA_MEM_Table_Size;

PRIVATE BYTE         MsgSubtype2;
PRIVATE BYTE         MsgSubtype1;
PRIVATE BYTE         result_code;
PRIVATE BOOLEAN      updates_to_db_made = false;
PRIVATE CHAR         ErrorMsg  [100];
        CHAR         AppName   [MAX_APP_NAME_SIZE];
PRIVATE CHAR         last_trid [MAX_TRAN_ID_SIZE];
PRIVATE pCHAR        reply_que;
PRIVATE CHAR         orig_comm_inf[MAX_ORIG_INFO_SIZE];
PRIVATE CHECKPOINT   checkpoint_rec;
PRIVATE CHAR         AtpBuffer[30000];
PRIVATE BYTE         app_data_type;
PRIVATE CHAR         applnk_que_name[8]="applnkA";

BOOLEAN   in_refeed_mode    = false;
CHAR      Version[] = "ATP_11.12.4";


INT ds_timer_flag;
INT ds_timer_value;

/* Database Timing Measurements */
TIMINGS_LIST  TimingStats;
INT           DB_Timing_Flag; /* True = Do DB Timing Stats, False = don't do */
INT           ForceReport;
double        NextReportTime;
CHAR          ReportTime[5];
CHAR  CUP_merchant_indicator[8]={0};

#ifdef _DEBUG
   #define PRINT( X ) printf( X )
#else
   #define PRINT( X )
#endif


/*
 * These defines control whether the dataserver is using logger-reply
 * and whether it should be doing checkpointing.
 *
 * Possible combinations:
 *    a) using logger-reply with checkpoints
 *    b) using logger-reply without checkpoints
 *    c) using neither.
 *
 * note: cannot use checkpoints without logger-reply!!!!!
 *
 */ 
// 12-07-98 gabriel
//#define USING_LOGGER_REPLY
//#define USING_CHECKPOINTS

#ifndef USING_LOGGER_REPLY
   #undef USING_CHECKPOINTS
#endif


/*****************************************************************************

  Function:   genutil_format_date

  Description:  ?????????????????
  Author: 
      unknown
  Inputs:
      temp 
  Outputs:
  Return values:
      None
  Modified by:
      BGable      6/26/98
******************************************************************************/
void genutil_format_date(pCHAR temp)
{
   struct timeb   time_structure;   // Time for ftime
   time_t long_time_since_1970;        // Number of seconds since 01/01/1970 A.D. 12:00 A.M.
   struct tm *current_time;            // Current time structure
   CHAR   temp1[4] = "0";

   ftime(&time_structure);
   long_time_since_1970 = time_structure.time;
   current_time = localtime(&long_time_since_1970);
   current_time->tm_mon++;
   if (current_time->tm_year >= 70)
      current_time->tm_year += 1900;
   else
      current_time->tm_year += 2000;
   itoa(current_time->tm_year, temp, 10);
   itoa(current_time->tm_mon, temp1 + 1, 10);
   if(strlen(temp1) == 2)
      memcpy(temp + 4, temp1, 2);
   else
      memcpy(temp + 4, temp1 + 1, 2);
   memset(temp1, 0, sizeof(temp1));
   temp1[0] = '0';
   itoa(current_time->tm_mday, temp1 + 1, 10);
   if(strlen(temp1) == 2)
      memcpy(temp + 6, temp1, 2);
   else
      memcpy(temp + 6, temp1 + 1, 2);
   memset(temp1, 0, sizeof(temp1));
   temp1[0] = '0';
   itoa(current_time->tm_hour, temp1 + 1, 10);
   if(strlen(temp1) == 2)
      memcpy(temp + 8, temp1, 2);
   else
      memcpy(temp + 8, temp1 + 1, 2);
   memset(temp1, 0, sizeof(temp1));
   temp1[0] = '0';
   itoa(current_time->tm_min, temp1 + 1, 10);
   if(strlen(temp1) == 2)
      memcpy(temp + 10, temp1, 2);
   else
      memcpy(temp + 10, temp1 + 1, 2);
   memset(temp1, 0, sizeof(temp1));
   temp1[0] = '0';
   itoa(current_time->tm_sec, temp1 + 1, 10);
   if(strlen(temp1) == 2)
      memcpy(temp + 12, temp1, 2);
   else
      memcpy(temp + 12, temp1 + 1, 2);

} /* genutil_format_date */

/******************************************************************************
 *
 *  NAME:         trands_read_CUP_merchant_indicator
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
INT trands_read_CUP_merchant_indicator( void )
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

#ifdef HCTS2
void trands_read_atp_card_activation_parameters()
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
		trands_log_message(2, 2, "CARD_ACTIVATION not read", "trands_read_atp_card_activation_parameters");
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
		trands_log_message(2, 2, "UPDATING_STATUS not read", "trands_read_atp_card_activation_parameters");
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
		trands_log_message(2, 2, "CURRENT_STATUS not read", "trands_read_atp_card_activation_parameters");
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
    	trands_log_message(2, 2, "CURRENT_STATUS contains invalid data", "trands_read_atp_card_activation_parameters");
    }
}
#endif

/*************************************************************************************/
/*************************************************************************************/
LONG CreateTable(BYTE AppData)
{
   BYTE return_code;
   char table_count[5];
   char SectionName[13] = "";
   char buffer [200] = {0};
   INT  num_rec, rec_size;
   
   //INT BIN01_SHA_MEM_Table_Size = 5000;
   
   memset(ErrorMsg, 0, sizeof(ErrorMsg));
   memset(table_count, 0, sizeof(table_count));
   if(AppData == BIN01_DATA)
   {
      return_code = db_get_bin01_count( table_count, ErrorMsg );
      if( return_code != PTEMSG_OK)
         return(-1L);
      Bin_Count = atol(table_count);
      DB_bin01_count=atoi(table_count);
      BIN01_SHA_MEM_Table_Size = atol (Bin01_Tablle_Size);
      if(Bin_Count > BIN01_SHA_MEM_Table_Size)
      {

         sprintf(buffer,"Total Bin Record in DB is:%ld BIN01 Table Creation Failed in SHM MEM",Bin_Count);
         trands_log_message(3,3,buffer,"CreateTable ");
         Bin_Count = 0;
         return(-1L);
      }
      //num_rec = Bin_Count;// Make no records records - 1500
	  num_rec =   BIN01_SHA_MEM_Table_Size;
      rec_size = sizeof(BIN01);
      strcpy(SectionName, "Bin01Table");
   }
   else if(AppData == CAF01_DATA)
   {
      return_code = db_get_caf01_count( table_count, ErrorMsg );
      if( return_code != PTEMSG_OK)
         return(-1L);
      Caf_Count = atol(table_count);
      if(Caf_Count > SERVICE_LIST_SIZE)
      {
         Caf_Count = 0;
         return(-1L);
      }
      //num_rec = Caf_Count;// Make no records records - 1000
		num_rec =   MAX_NO_OF_RECORDS_FOR_BIN_CAF;
      rec_size = sizeof(CAF01);
      strcpy(SectionName, "Caf01Table");
   }
   else if(AppData == FAF01_DATA)
   {
      return_code = db_get_faf01_count( table_count, ErrorMsg );
      if( return_code != PTEMSG_OK)
         return(-1L);
      Faf_Count = atol(table_count);
      if(Faf_Count > SERVICE_LIST_SIZE)
      {
         Faf_Count = 0;
         return(-1L);
      }
      num_rec = Faf_Count;
      rec_size = sizeof(FAF01);
      strcpy(SectionName, "Faf01Table");
   }
   else if(AppData == CRF01_DATA)
   {
      return_code = db_get_crf01_count( table_count, ErrorMsg );
      if( return_code != PTEMSG_OK)
         return(-1L);
      Crf_Count = atol(table_count);
      if(Crf_Count > SERVICE_LIST_SIZE)
      {
         Crf_Count = 0;
         return(-1L);
      }
      num_rec = Crf_Count;
      rec_size = sizeof(CRF01);
      strcpy(SectionName, "Crf01Table");
   }
   else if(AppData == CSF01_DATA)
   {
      return_code = db_get_csf01_count( table_count, ErrorMsg );
      if( return_code != PTEMSG_OK)
         return(-1L);
      Csf_Count = atol(table_count);
      if(Csf_Count > SERVICE_LIST_SIZE)
      {
         Csf_Count = 0;
         return(-1L);
      }
      num_rec = Csf_Count;
      rec_size = sizeof(CSF01);
      strcpy(SectionName, "Csf01Table");
   }
   else if(AppData == OPRMON_DATA)
   {
      return(CreateXipcTable(1, MAX_ORIG_INFO_SIZE, "MonitorTable"));
   }
   else if(AppData == BON02_DATA)
   {
      return_code = db_get_bon02_count( table_count, ErrorMsg );
      if( return_code != PTEMSG_OK)
         return(-1L);
      Bon_Count = atol(table_count);
      if(Bon_Count > SERVICE_LIST_SIZE)
      {
         Bon_Count = 0;
         return(-1L);
      }
      num_rec = Bon_Count;
      rec_size = sizeof(BON02);
      strcpy(SectionName, "Bon02Table");
   }
   else if(AppData == UAG01_DATA )
   {
      /* Consists of 6 byte auth number plus 1 byte lock flag plus nulls. */
      num_rec = 1;
      rec_size = 9;
      strcpy( SectionName, "AuthTable" );
   }
   else if(AppData == BON01_DATA)
   {
      return_code = db_get_bon01_count( table_count, ErrorMsg );
      if( return_code != PTEMSG_OK)
         return(-1L);
      Bon_Count = atol(table_count);
      if(Bon_Count > SERVICE_LIST_SIZE)
      {
         Bon_Count = 0;
         return(-1L);
      }
      num_rec = Bon_Count;
      rec_size = sizeof(BON01);
      strcpy(SectionName, "Bon01Table");
   }
   else
      return(-1L);
   return(CreateXipcTable(num_rec, rec_size, SectionName));
}
/*************************************************************************************/
/*Added DB Passwords Structure for Getting User ids , Old Passwords from tf.ini  file by TF-Srikanth */
/*************************************************************************************/

BYTE process_db_select_dbpasswords( pDbPasswords dbpass_ptr, pCHAR ErrorMsg )
{
	WORD key[] = {0x13A7, 0x6135, 0x9CDF, 0xA852};
	//Get the APP,Core password,id.
	GetCorePassword    ( dbpass_ptr->core_old_password );
	GetAppPassword     ( dbpass_ptr->app_old_password );	
	GetCoreUserId      ( dbpass_ptr->core_user_id );
	GetAppUserId       ( dbpass_ptr->app_user_id );

   return PTEMSG_OK;

}
BYTE process_arch_db_select_dbpasswords( pArchDbPasswords dbpass_ptr, pCHAR ErrorMsg )
{
	WORD key[] = {0x13A7, 0x6135, 0x9CDF, 0xA852};
	//Get the APP,Core password,id.
	GetArchPassword    ( dbpass_ptr->arch_old_password);
	GetArchUserId       ( dbpass_ptr->arch_user_id);

   return PTEMSG_OK;

}
BYTE process_mra_db_select_dbpasswords( pMraDbPasswords dbpass_ptr, pCHAR ErrorMsg )
{
	WORD key[] = {0x13A7, 0x6135, 0x9CDF, 0xA852};
	//Get the APP,Core password,id.
	GetMraPassword    ( dbpass_ptr->mra_old_password);
	GetMraUserId       ( dbpass_ptr->mra_user_id);

   return PTEMSG_OK;

}

BOOLEAN SetArchPassword(pCHAR arch_password)
{
	CHAR  filename	   [256];
	 CHAR  tmpstr		[256];
	
	 GetAscendentConfigDirectory(tmpstr);
	 sprintf(filename, "%stf.ini", tmpstr);
	
	 
	 WritePrivateProfileString(
		  "DATA_SOURCE",	/* pointer to section name*/
		  "ARCHIVE_PASSWORD",	/* pointer to key name*/
		  arch_password ,	/* pointer to string to add*/
		  filename			/* pointer to initialization filename*/
	 );
	return 1;
}

BOOLEAN GetArchUserId ( pCHAR arch_user_id )
{
   DWORD rc;
   CHAR  sectionname  [] = "DATA_SOURCE";
   CHAR  filename     [256];
   CHAR  tmpstr       [256];

   GetAscendentConfigDirectory(tmpstr);
   sprintf(filename, "%stf.ini", tmpstr);
  
   rc = GetPrivateProfileString(
    sectionname,	            /*  points to section name  */
    "ARCHIVE_USER_ID",             /*  points to key name  */
    "ARCH",               /*  points to default string  */
    tmpstr,                         /*  points to destination buffer  */
    sizeof(tmpstr) - 1,             /*  size of destination buffer  */
    filename                        /*  points to initialization filename  */
   );

   strcpy(arch_user_id, tmpstr);

   return 1;
} /* end of GetArchUserId */

/* ****************************************************************************** */
BOOLEAN GetArchPassword ( pCHAR arch_password )
{
   DWORD rc;
   CHAR  sectionname  [] = "DATA_SOURCE";
   CHAR  filename     [256];
   CHAR  tmpstr       [256];

   GetAscendentConfigDirectory(tmpstr);
   sprintf(filename, "%stf.ini", tmpstr);
  
   rc = GetPrivateProfileString(
    sectionname,	            /*  points to section name  */
    "ARCHIVE_PASSWORD",             /*  points to key name  */
    "ARCH",               /*  points to default string  */
    tmpstr,                         /*  points to destination buffer  */
    sizeof(tmpstr) - 1,             /*  size of destination buffer  */
    filename                        /*  points to initialization filename  */
   );

   DecryptPassword(tmpstr, arch_password);

   return 1;
} /* end of GetArchPassword */

BOOLEAN SetMraPassword(pCHAR mra_password)
{
	CHAR  filename	   [256];
	 CHAR  tmpstr		[256];
	
	 GetAscendentConfigDirectory(tmpstr);
	 sprintf(filename, "%stf.ini", tmpstr);
	 
	 WritePrivateProfileString(
		  "MRA_DATA_SOURCE",	/* pointer to section name*/
		  "MRA_PASSWORD",	/* pointer to key name*/
		  mra_password ,	/* pointer to string to add*/
		  filename			/* pointer to initialization filename*/
	 );
	return 1;
}

BOOLEAN GetMraUserId ( pCHAR mra_user_id )
{
   DWORD rc;
   CHAR  sectionname  [] = "MRA_DATA_SOURCE";
   CHAR  filename     [256];
   CHAR  tmpstr       [256];

   GetAscendentConfigDirectory(tmpstr);
   sprintf(filename, "%stf.ini", tmpstr);
  
   rc = GetPrivateProfileString(
    sectionname,	            /*  points to section name  */
    "MRA_USER_ID",             /*  points to key name  */
    "MRA",               /*  points to default string  */
    tmpstr,                         /*  points to destination buffer  */
    sizeof(tmpstr) - 1,             /*  size of destination buffer  */
    filename                        /*  points to initialization filename  */
   );

   strcpy(mra_user_id, tmpstr);

   return 1;
} /* end of GetMraUserId */

/* ****************************************************************************** */
BOOLEAN GetMraPassword ( pCHAR mra_password )
{
   DWORD rc;
   CHAR  sectionname  [] = "MRA_DATA_SOURCE";
   CHAR  filename     [256];
   CHAR  tmpstr       [256];

   GetAscendentConfigDirectory(tmpstr);
   sprintf(filename, "%stf.ini", tmpstr);
  
   rc = GetPrivateProfileString(
    sectionname,	            /*  points to section name  */
    "MRA_PASSWORD",             /*  points to key name  */
    "MRA",               /*  points to default string  */
    tmpstr,                         /*  points to destination buffer  */
    sizeof(tmpstr) - 1,             /*  size of destination buffer  */
    filename                        /*  points to initialization filename  */
   );

   DecryptPassword(tmpstr, mra_password);

   return 1;
} /* end of GetMraPassword */

/*************************************************************************************/
/*************************************************************************************/
LONG  PopulateTable(BYTE  AppData)
{
   char  SectionName[13] = "";
   LONG  TableSize;
   BYTE  ret_code;
   BYTE  OpMonFlag = 0;
   pBYTE TableList;
   CHAR  monitor_flag[100]={0} ;

   if(AppData == BIN01_DATA)
   {
      strcpy(SectionName, "Bin01Table");
      TableSize = sizeof(BIN01);
      ret_code = db_get_bin01_list(&db_lists.Bin01_List, ErrorMsg);
      if(ret_code != PTEMSG_OK)
         return(-1L);
      else
         TableList = (pBYTE)(&db_lists.Bin01_List);
   }
   else if(AppData == CAF01_DATA)
   {
      strcpy(SectionName, "Caf01Table");
      TableSize = sizeof(CAF01);
      ret_code = db_get_caf01_list(&db_lists.Caf01_List, ErrorMsg);
      if(ret_code != PTEMSG_OK)
         return(-1L);
      else
         TableList = (pBYTE)(&db_lists.Caf01_List);
   }
   else if(AppData == CRF01_DATA)
   {
      strcpy(SectionName, "Crf01Table");
      TableSize = sizeof(CRF01);
      ret_code = db_get_crf01_list(&db_lists.Crf01_List, ErrorMsg);
      if(ret_code != PTEMSG_OK)
         return(-1L);
      else
         TableList = (pBYTE)(&db_lists.Crf01_List);
   }
   else if(AppData == FAF01_DATA)
   {
      strcpy(SectionName, "Faf01Table");
      TableSize = sizeof(FAF01);
      ret_code = db_get_faf01_list(&db_lists.Faf01_List, ErrorMsg);
      if(ret_code != PTEMSG_OK)
         return(-1L);
      else
         TableList = (pBYTE)(&db_lists.Faf01_List);
   }
   else if(AppData == CSF01_DATA)
   {
      strcpy(SectionName, "Csf01Table");
      TableSize = sizeof(CSF01);
      ret_code = db_get_csf01_list(&db_lists.Csf01_List, ErrorMsg);
      if(ret_code != PTEMSG_OK)
         return(-1L);
      else
         TableList = (pBYTE)(&db_lists.Csf01_List);
   }
   else if(AppData == BON02_DATA)
   {
      strcpy(SectionName, "Bon02Table");
      TableSize = sizeof(BON02);
      ret_code = db_get_bon02_list(&db_lists.Bon02_List, ErrorMsg);
      if(ret_code != PTEMSG_OK)
         return(-1L);
      else
         TableList = (pBYTE)(&db_lists.Bon02_List);
   }
   else if(AppData == BON01_DATA)
   {
      strcpy(SectionName, "Bon01Table");
      TableSize = sizeof(BON01);
      ret_code = db_get_bon01_list(&db_lists.Bon01_List, ErrorMsg);
      if(ret_code != PTEMSG_OK)
         return(-1L);
      else
         TableList = (pBYTE)(&db_lists.Bon01_List);
   }
   else if(AppData == OPRMON_DATA)
   {
 	    return WriteMemMonitorTable ((pBYTE)monitor_flag) ;
   }
   else if(AppData == MPPARA01_DATA)
   {
      strcpy(SectionName, "Mppara01Table");
      TableSize = sizeof(MPPARA01);
      ret_code = db_get_mppara01_list(&db_lists.Mppara01_Gui_List, ErrorMsg);
      if(ret_code != PTEMSG_OK)
         return(-1L);
      else
         TableList = (pBYTE)(&db_lists.Mppara01_Gui_List);
   }
   else
      return(-1L);
   return(WriteTable(SectionName, TableSize, TableList));
}

/*************************************************************************************/
/*************************************************************************************/
BYTE  VerifyMem(LONG ReservedMem, LONG NeededMem)
{
   LONG Result;

   Result = ReservedMem - NeededMem;
   if (Result < 0)
      return(MEM_ERR);
   if( (Result * 100) < (ReservedMem * MEM_PERCENT))
      return(MEM_ENOUGH);
   return(MEM_OK);
}

/*************************************************************************************/
/*************************************************************************************/
pPTE_MSG process_check_binn_range(pPTE_MSG p_msg_in)
{
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   pPTE_MSG       p_msg_out;
   pBYTE          p_current;

   p_msg_out     = NULL_PTR;
   p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
   app_data_type = ptemsg_get_pte_msg_data_app_data_type( p_msg_data );
   p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
   memcpy(AtpBuffer, p_data, sizeof(AUTH_TX) + ISO_MSG_LENGTH1);
   p_current     = AtpBuffer + ISO_MSG_LENGTH1 + sizeof(AUTH_TX);
   p_data = p_data + ISO_MSG_LENGTH1 + sizeof(AUTH_TX);  
 
   if( ( result_code = db_check_binn_range(( pCHAR )p_data, &db_recs.bin01, ErrorMsg ) ) == PTEMSG_OK )
   {
      memcpy(p_current, (BYTE *)&db_recs.bin01, sizeof( db_recs.bin01 ));
      p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                    ptemsg_get_msg_subtype1 (p_msg_in),
                                    ptemsg_get_msg_subtype2 (p_msg_in),
                                    reply_que,
                                    application_que_name,
                                    (pBYTE)AtpBuffer,
                                    ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_recs.bin01),
                                    BIN01_DATA);


      if (p_msg_out == NULL_PTR)
      {
         result_code = PTEMSG_INSUFFICIENT_MEMORY;
         sprintf (ErrorMsg, "Insufficient Memory to build Select response to %s", reply_que);
      }
   }

  
   return (p_msg_out);

}

/*************************************************************************************/
/*************************************************************************************/
pPTE_MSG process_check_binn02_range(pPTE_MSG p_msg_in)
{
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   pPTE_MSG       p_msg_out;
   pBYTE          p_current;

   p_msg_out     = NULL_PTR;
   p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
   app_data_type = ptemsg_get_pte_msg_data_app_data_type( p_msg_data );
   p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
   memcpy(AtpBuffer, p_data, sizeof(AUTH_TX) + ISO_MSG_LENGTH1);
   p_current     = AtpBuffer + ISO_MSG_LENGTH1 + sizeof(AUTH_TX);
   p_data = p_data + ISO_MSG_LENGTH1 + sizeof(AUTH_TX);
 
   
   if( ( result_code = db_check_binn02_range(( pCHAR )p_data, &db_recs.bin02, ErrorMsg ) ) == PTEMSG_OK )
   {
      memcpy(p_current, (BYTE *)&db_recs.bin02, sizeof( db_recs.bin02 ));
      p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                    ptemsg_get_msg_subtype1 (p_msg_in),
                                    ptemsg_get_msg_subtype2 (p_msg_in),
                                    reply_que,
                                    application_que_name,
                                    (pBYTE)AtpBuffer,
                                    ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_recs.bin02),
                                    BIN02_DATA);


      if (p_msg_out == NULL_PTR)
      {
         result_code = PTEMSG_INSUFFICIENT_MEMORY;
         sprintf (ErrorMsg, "Insufficient Memory to build Select response to %s", reply_que);
      }
   }

  
   return (p_msg_out);

}

pPTE_MSG process_select_auth_id_msg (pPTE_MSG p_msg_in)
{
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   pPTE_MSG       p_msg_out;
   pBYTE          p_current;
   BYTE           subtype2;

   p_msg_out     = NULL_PTR;
   p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
   app_data_type = ptemsg_get_pte_msg_data_app_data_type( p_msg_data );
   p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );

   memcpy( AtpBuffer, p_data, sizeof(AUTH_TX) + ISO_MSG_LENGTH1 );

   p_current     = AtpBuffer + ISO_MSG_LENGTH1 + sizeof(AUTH_TX);
   p_data        = p_data    + ISO_MSG_LENGTH1 + sizeof(AUTH_TX);

   subtype2 = ptemsg_get_msg_subtype2( p_msg_in );

   /* Get Authorization ID - Annnnn, where A is alphanumeric. */
   result_code = db_select_auth_id( &db_recs.uag01, subtype2, ErrorMsg );
   if ( result_code == PTEMSG_OK )
   {
      memcpy(p_current, (BYTE *)&db_recs.uag01, sizeof( db_recs.uag01 ));
      p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                    ptemsg_get_msg_subtype1 (p_msg_in),
                                    ptemsg_get_msg_subtype2 (p_msg_in),
                                    reply_que,
                                    application_que_name,
                                    (pBYTE)AtpBuffer,
                                    ISO_MSG_LENGTH1
                                      + sizeof(AUTH_TX)
                                      + sizeof(db_recs.uag01),
                                    UAG01_DATA);
      if (p_msg_out == NULL_PTR)
      {
         result_code = PTEMSG_INSUFFICIENT_MEMORY;
         sprintf( ErrorMsg,
                 "Insufficient Memory to build Select response to %s",
                  reply_que );
      }
   }

   return( p_msg_out );
}




/*************************************************************************************/
/*************************************************************************************/
pPTE_MSG process_insert_msg( pPTE_MSG p_msg_in )
{
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   pPTE_MSG       p_msg_out;
   pBYTE          p_current;
   CHAR           buffer[100];
   pAUTH_TX       auth_tx;
   p_msg_out     = NULL_PTR;
   p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
   p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
   memcpy(AtpBuffer, p_data, sizeof(AUTH_TX) + ISO_MSG_LENGTH1);
   p_current     = AtpBuffer + ISO_MSG_LENGTH1 + sizeof(AUTH_TX);
   app_data_type = ptemsg_get_pte_msg_data_app_data_type( p_msg_data );
   p_data = p_data + ISO_MSG_LENGTH1 + sizeof(AUTH_TX);
   auth_tx = (pAUTH_TX)p_data;

   switch( app_data_type )
   {

   case JCB3DS2_DATA:

      PRINT( "request to insert JCB3DS2 record\n" );

       memset( &db_recs.jcb3ds2, 0, sizeof( db_recs.jcb3ds2 ) );
       memcpy( &db_recs.jcb3ds2, ( pJCB3DS2 )p_data, sizeof( JCB3DS2 ) );
       if (DB_NullCheck_Flag==1)
       {
           CheckNULLTerminatedinJCB3DS2 (&db_recs.jcb3ds2,MODE_INSERT);
        }
        result_code = db_insert_jcb3ds2( &db_recs.jcb3ds2, ErrorMsg );
        break;
	   case MCC01_DATA:
	   
		  PRINT( "request to insert mcc01 record\n" );
	   
		  /* retrieve the data into a ocf20 structure and insert into database */
		  memset( &db_recs.mcc01, 0, sizeof( db_recs.mcc01 ) );
		  memcpy( &db_recs.mcc01, ( pMCC01)p_data, sizeof( MCC01 ) );
		  if (DB_NullCheck_Flag==1)
		  {
			  CheckNULLTerminatedinMCC01 (&db_recs.mcc01,MODE_INSERT);
		  }
		  result_code = db_insert_mcc01 ( &db_recs.mcc01 , ErrorMsg );
		  break;
		case OMC01_DATA:
		  
			 PRINT( "request to insert omc01 record\n" );
		  
			 /* retrieve the data into a ocf20 structure and insert into database */
			 memset( &db_recs.omc01, 0, sizeof( db_recs.omc01 ) );
			 memcpy( &db_recs.omc01 , ( pOMC01)p_data, sizeof( OMC01 ) );
			 if (DB_NullCheck_Flag==1)
			 {
				  CheckNULLTerminatedinOMC01 (&db_recs.omc01,MODE_INSERT);
			 }
			 result_code = db_insert_omc01 ( &db_recs.omc01 , ErrorMsg );
			 break;

	  
	  case CLF01_DATA:

         PRINT( "request to insert clf01 record\n" );

         /* retrieve the data into a ocf20 structure and insert into database */
         memset( &db_recs.clf01, 0, sizeof( db_recs.clf01 ) );
         memcpy( &db_recs.clf01, ( pCLF01)p_data, sizeof( CLF01 ) );
		 if (DB_NullCheck_Flag==1)
		 {
			  CheckNULLTerminatedinCLF01 (&db_recs.clf01,MODE_INSERT);
		 }
         result_code = db_insert_clf01( &db_recs.clf01, ErrorMsg );
         break;

      
      case BPF01_DATA:

         PRINT( "request to insert bpf01 record\n" );

         /* retrieve the data into a ocf20 structure and insert into database */
         memset( &db_recs.bpf01, 0, sizeof( db_recs.bpf01 ) );
         memcpy( &db_recs.bpf01, ( pBPF01)p_data, sizeof( BPF01 ) );
		 if (DB_NullCheck_Flag==1)
		 {
			  CheckNULLTerminatedinBPF01 (&db_recs.bpf01,MODE_INSERT);
		 }
         result_code = db_insert_bpf01( &db_recs.bpf01, ErrorMsg );
         break;

      case JBF01_DATA:

         PRINT( "request to insert jbf01 record\n" );

         /* retrieve the data into a ocf20 structure and insert into database */
         memset( &db_recs.jbf01, 0, sizeof( db_recs.jbf01 ) );
         memcpy( &db_recs.jbf01, ( pJBF01)p_data, sizeof( JBF01 ) );
		 if (DB_NullCheck_Flag==1)
		 {
			  CheckNULLTerminatedinJBF01 (&db_recs.jbf01,MODE_INSERT);
		 }
         result_code = db_insert_jbf01( &db_recs.jbf01, ErrorMsg );
         break;

      case APF01_DATA:

         PRINT( "request to insert apf01 record\n" );

         /* retrieve the data into a ocf20 structure and insert into database */
         memset( &db_recs.apf01, 0, sizeof( db_recs.apf01 ) );
         memcpy( &db_recs.apf01, ( pAPF01)p_data, sizeof( APF01 ) );
		 if (DB_NullCheck_Flag==1)
		 {
			  CheckNULLTerminatedinAPF01 (&db_recs.apf01,MODE_INSERT);
		 }
         result_code = db_insert_apf01( &db_recs.apf01, ErrorMsg );
         break;

      case FAF01_DATA:

         PRINT( "request to insert faf01 record\n" );

         /* retrieve the data into a ocf20 structure and insert into database */
         memset( &db_recs.faf01, 0, sizeof( db_recs.faf01 ) );
         memcpy( &db_recs.faf01, ( pFAF01)p_data, sizeof( FAF01 ) );
		 if (DB_NullCheck_Flag==1)
		 {
			  CheckNULLTerminatedinFAF01 (&db_recs.faf01,MODE_INSERT);
		 }
         result_code = db_insert_faf01( &db_recs.faf01, ErrorMsg );
         break;

      case CCF10_DATA:

         PRINT( "request to insert ccf10 record\n" );

         /* retrieve the data into a mcf01 structure and insert into database */
         memset( &db_recs.ccf10, 0, sizeof( db_recs.ccf10 ) );
         memcpy( &db_recs.ccf10, ( pCCF10 )p_data, sizeof( CCF10 ) );
		 if (DB_NullCheck_Flag==1)
		 {
			  CheckNULLTerminatedinCCF10 (&db_recs.ccf10,MODE_INSERT);
		 }
         result_code = db_insert_ccf10( &db_recs.ccf10, ErrorMsg );
         break;

		case BIN02_DATA:
			PRINT( "request to insert bin02 record\n" );

         /* retrieve the data into a bin02 structure and insert into database */
         memset( &db_recs.bin02, 0, sizeof( db_recs.bin02 ) );
         memcpy( &db_recs.bin02, ( pBIN02 )p_data, sizeof( BIN02 ) );
		 if (DB_NullCheck_Flag==1)
		 {
			  CheckNULLTerminatedinBIN02 (&db_recs.bin02,MODE_INSERT);
		 }
         result_code = db_insert_bin02( &db_recs.bin02, ErrorMsg );
     break;


      case BON01_DATA:

         PRINT( "request to insert bon01 record\n" );

         /* retrieve the data into a bon01 structure and insert into database */
         memset( &db_recs.bon01, 0, sizeof( db_recs.bon01 ) );
         memcpy( &db_recs.bon01, ( pBON01 )p_data, sizeof( BON01 ) );
		 if (DB_NullCheck_Flag==1)
		 {
			  CheckNULLTerminatedinBON01 (&db_recs.bon01,MODE_INSERT);
		 }
         result_code = db_insert_bon01( &db_recs.bon01, ErrorMsg );
         break;

      case BON02_DATA:

         memset( &db_recs.bon02, 0, sizeof( db_recs.bon02 ) );
         memcpy( &db_recs.bon02, (pBON02)p_data, sizeof(BON02) );
		 if (DB_NullCheck_Flag==1)
		 {
			  CheckNULLTerminatedinBON02 (&db_recs.bon02,MODE_INSERT);
		 }
         result_code = db_insert_bon02( &db_recs.bon02, ErrorMsg );

         /* Update Multi-Winner Response to be common for all records. */
         if ( result_code == PTEMSG_OK )
         {
            result_code = db_update_bon02_multi_winner_resp(&db_recs.bon02,ErrorMsg);
            if ( result_code != PTEMSG_OK )
            {
               memset( buffer, 0x00, sizeof(buffer) );
               sprintf( buffer,
                       "Unable to update Multi-Winner Resp for Cash Bonus: %s",
                        db_recs.bon02.cash_id );

               trands_log_message(1,3,ErrorMsg," process_insert_msg ");
               trands_log_message(1,3,buffer," process_insert_msg ");
               result_code = PTEMSG_OK;
            }
         }
         break;

      case CCF01_DATA:

         PRINT( "request to insert ccf01 record\n" );

         /* retrieve the data into a mcf01 structure and insert into database */
         memset( &db_recs.ccf01, 0, sizeof( db_recs.ccf01 ) );
         memcpy( &db_recs.ccf01, ( pCCF01 )p_data, sizeof( CCF01 ) );
		 if (DB_NullCheck_Flag==1)
		 {
			  CheckNULLTerminatedinCCF01 (&db_recs.ccf01,MODE_INSERT);
		 }
         result_code = db_insert_ccf01( &db_recs.ccf01, ErrorMsg );
         break;

	  case MPPARA01_DATA:
	  
	   PRINT( "request to insert MPPARA01 record\n" );

	   /* retrieve the data into a mcf01 structure and insert into database */
	   memset( &db_recs.mppara01, 0, sizeof( db_recs.mppara01 ) );
	   memcpy( &db_recs.mppara01, ( pMPPARA01)p_data, sizeof( MPPARA01 ) );
	   if (DB_NullCheck_Flag==1)
	   {
			  CheckNULLTerminatedinMPPARA01 (&db_recs.mppara01,MODE_INSERT);
	   }
	   result_code = db_insert_mppara01( &db_recs.mppara01, ErrorMsg );
	   break;
 

     
      case ACF01_DATA:

         PRINT( "request to insert acf01 record\n" );

         /* retrieve the data into a mcf01 structure and insert into database */
         memset( &db_recs.acf01, 0, sizeof( db_recs.acf01 ) );
         memcpy( &db_recs.acf01, ( pACF01 )p_data, sizeof( ACF01 ) );
  	     if (DB_NullCheck_Flag==1)
  	     {
  			  CheckNULLTerminatedinACF01 (&db_recs.acf01,MODE_INSERT);
  	     }
         result_code = db_insert_acf01( &db_recs.acf01, ErrorMsg );
         break;

      case CCL01_DATA:

         PRINT( "request to insert ccl01 record\n" );

         /* retrieve the data into a ccl01 structure and insert into database */
         memset( &db_recs.ccl01, 0, sizeof( db_recs.ccl01 ) );
         memcpy( &db_recs.ccl01, ( pCCL01 )p_data, sizeof( CCL01 ) );
  	     if (DB_NullCheck_Flag==1)
  	     {
  			  CheckNULLTerminatedinCCL01 (&db_recs.ccl01,MODE_INSERT);
  	     }
         result_code = db_insert_ccl01( &db_recs.ccl01, ErrorMsg );
         break;

      case BDKKEY01_DATA:

         PRINT( "request to insert bdkkey01 record\n" );

         /* retrieve the data into a bdkkey01 structure and insert into database */
         memset( &db_recs.bdkkey01, 0, sizeof( db_recs.bdkkey01 ) );
         memcpy( &db_recs.bdkkey01, ( pBDKKEY01 )p_data, sizeof( BDKKEY01 ) );
  	     if (DB_NullCheck_Flag==1)
  	     {
  			  CheckNULLTerminatedinBDKKEY01 (&db_recs.bdkkey01,MODE_INSERT);
  	     }
         result_code = db_insert_bdkkey01( &db_recs.bdkkey01, ErrorMsg );
         break;

      case CAF01_KEYBLOCK_DATA:

         PRINT( "request to insert caf01_keyblock01 record\n" );

         /* retrieve the data into a caf01_keyblock01 structure and insert into database */
         memset( &db_recs.caf01_keyblock01, 0, sizeof( db_recs.caf01_keyblock01 ) );
         memcpy( &db_recs.caf01_keyblock01, ( pCAF01_KEYBLOCK )p_data, sizeof( CAF01_KEYBLOCK ) );
  	     if (DB_NullCheck_Flag==1)
  	     {
  	    	CheckNULLTerminatedinCAF01_KEYBLOCK01 (&db_recs.caf01_keyblock01,MODE_INSERT);
  	     }
         result_code = db_insert_caf01_keyblock01( &db_recs.caf01_keyblock01, ErrorMsg );
         break;

      case CRF01_DATA:

         PRINT( "request to insert crf01 record\n" );

         /* retrieve the data into a mcf01 structure and insert into database */
         memset( &db_recs.crf01, 0, sizeof( db_recs.crf01 ) );
         memcpy( &db_recs.crf01, ( pCRF01 )p_data, sizeof( CRF01 ) );
  	     if (DB_NullCheck_Flag==1)
  	     {
  			  CheckNULLTerminatedinCRF01 (&db_recs.crf01,MODE_INSERT);
  	     }
         result_code = db_insert_crf01( &db_recs.crf01, ErrorMsg );
         break;

      case CSF01_DATA:

         PRINT( "request to insert csf01 record\n" );

         /* retrieve the data into a mcf01 structure and insert into database */
         memset( &db_recs.csf01, 0, sizeof( db_recs.csf01 ) );
         memcpy( &db_recs.csf01, ( pCSF01 )p_data, sizeof( CSF01 ) );
  	     if (DB_NullCheck_Flag==1)
  	     {
  			  CheckNULLTerminatedinCSF01 (&db_recs.csf01,MODE_INSERT);
  	     }
         result_code = db_insert_csf01( &db_recs.csf01, ErrorMsg );
         break;

      case PAF01_DATA:

         PRINT( "request to insert paf01 record\n" );

         /* retrieve the data into a mcf01 structure and insert into database */
         memset( &db_recs.paf01, 0, sizeof( db_recs.paf01 ) );
         memcpy( &db_recs.paf01, ( pPAF01 )p_data, sizeof( PAF01 ) );
  	     if (DB_NullCheck_Flag==1)
  	     {
  			  CheckNULLTerminatedinPAF01 (&db_recs.paf01,MODE_INSERT);
  	     }
         result_code = db_insert_paf01( &db_recs.paf01, ErrorMsg );
         break;

      case DEF01_DATA:

         PRINT( "request to insert def01 record\n" );

         /* retrieve the data into a def01 structure and insert into database */
         memset( &db_recs.def01, 0, sizeof( db_recs.def01 ) );
         memcpy( &db_recs.def01, ( pDEF01 )p_data, sizeof( DEF01 ) );
  	     if (DB_NullCheck_Flag==1)
  	     {
  			  CheckNULLTerminatedinDEF01 (&db_recs.def01,MODE_INSERT);
  	     }
         result_code = db_insert_def01( &db_recs.def01, ErrorMsg );
         break;

      case FCF02_DATA:

         PRINT( "request to insert fcf02 record\n" );

         /* retrieve the data into a fcf02 structure and insert into database */
         memset( &db_recs.fcf02, 0, sizeof( db_recs.fcf02 ) );
         memcpy( &db_recs.fcf02, ( pFCF02 )p_data, sizeof( FCF02 ) );
  	     if (DB_NullCheck_Flag==1)
  	     {
  			  CheckNULLTerminatedinFCF02 (&db_recs.fcf02,MODE_INSERT);
  	     }
         result_code = db_insert_fcf02( &db_recs.fcf02, ErrorMsg );
         break;

      case CCF02P_DATA:

         PRINT( "request to insert ccf02p record\n" );

         /* retrieve the data into a ccf02p structure and insert into database */
         memset( &db_recs.ccf02p, 0, sizeof( db_recs.ccf02p ) );
         memcpy( &db_recs.ccf02p, ( pCCF02P )p_data, sizeof( CCF02P ) );
  	     if (DB_NullCheck_Flag==1)
  	     {
  			  CheckNULLTerminatedinCCF02P (&db_recs.ccf02p,MODE_INSERT);
  	     }
         result_code = db_insert_ccf02p( &db_recs.ccf02p, ErrorMsg );
         break;

 
      case CCF04_DATA:

         PRINT( "request to insert ccf04 record\n" );

         /* retrieve the data into a ccf04 structure and insert into database */
         memset( &db_recs.ccf04, 0, sizeof( db_recs.ccf04 ) );
         memcpy( &db_recs.ccf04, ( pCCF04 )p_data, sizeof( CCF04 ) );
  	     if (DB_NullCheck_Flag==1)
  	     {
  			  CheckNULLTerminatedinCCF04 (&db_recs.ccf04,MODE_INSERT);
  	     }
         result_code = db_insert_ccf04( &db_recs.ccf04, ErrorMsg );
         break;

      case CCF03P_DATA:

         PRINT( "request to insert ccf03p record\n" );

         /* retrieve the data into a ccf03p structure and insert into database */
         memset( &db_recs.ccf03p, 0, sizeof( db_recs.ccf03p ) );
         memcpy( &db_recs.ccf03p, ( pCCF03 )p_data, sizeof( CCF03 ) );
  	     if (DB_NullCheck_Flag==1)
  	     {
  			  CheckNULLTerminatedinCCF03 (&db_recs.ccf03p,MODE_INSERT);
  	     }
         result_code = db_insert_ccf03p( &db_recs.ccf03p, ErrorMsg );
         break;

      case OFFUS_VELOCITY_DATA:

         PRINT( "request to insert offline_velocity record\n" );

         /* retrieve the data into a ccf03p structure and insert into database */
         memset( &db_recs.offus_velocity, 0, sizeof( db_recs.offus_velocity ) );
         memcpy( &db_recs.offus_velocity, ( pOffus_velocity )p_data, sizeof( Offus_velocity ) );
  	     if (DB_NullCheck_Flag==1)
  	     {
  			  CheckNULLTerminatedinOffus_velocity (&db_recs.offus_velocity,MODE_INSERT);
  	     }
         result_code = db_insert_offus_velocity( &db_recs.offus_velocity, ErrorMsg );
         break;

      
      case MCT01_DATA:

         PRINT( "request to insert mct01 record\n" );

         /* retrieve the data into a mct01 structure and insert into database */
         memset( &db_recs.mct01, 0, sizeof( db_recs.mct01 ) );
         memcpy( &db_recs.mct01, ( pMCT01 )p_data, sizeof( MCT01 ) );
  	     if (DB_NullCheck_Flag==1)
  	     {
  			  CheckNULLTerminatedinMCT01 (&db_recs.mct01,MODE_INSERT);
  	     }
         result_code = db_insert_mct01( &db_recs.mct01, ErrorMsg );
         break;

      case CTF01_DATA:

         PRINT( "request to insert ctf01 record\n" );

         /* retrieve the data into a ctf01 structure and insert into database */
         memset( &db_recs.ctf01, 0, sizeof( db_recs.ctf01 ) );
         memcpy( &db_recs.ctf01, ( pCTF01 )p_data, sizeof( CTF01 ) );
  	     if (DB_NullCheck_Flag==1)
  	     {
  			  CheckNULLTerminatedinCTF01 (&db_recs.ctf01,MODE_INSERT);
  	     }
         result_code = db_insert_ctf01( &db_recs.ctf01, ErrorMsg );
         break;

      case CAF01_DATA:

         PRINT( "request to insert caf01 record\n" );

         /* retrieve the data into a caf01 structure and insert into database */
         memset( &db_recs.caf01, 0, sizeof( db_recs.caf01 ) );
         memcpy( &db_recs.caf01, ( pCAF01 )p_data, sizeof( CAF01 ) );
  	     if (DB_NullCheck_Flag==1)
  	     {
  			  CheckNULLTerminatedinCAF01 (&db_recs.caf01,MODE_INSERT);
  	     }
         result_code = db_insert_caf01( &db_recs.caf01, ErrorMsg );
         break;

      case KEY3DS01_DATA:

                           PRINT( "request to insert key3ds01 record\n" );

                           /* retrieve the data into a caf01 structure and insert into database */
                           memset( &db_recs.key3ds01, 0, sizeof( db_recs.key3ds01 ) );
                           memcpy( &db_recs.key3ds01, ( pKEY3DS01 )p_data, sizeof( KEY3DS01 ) );
                    	     if (DB_NullCheck_Flag==1)
                    	     {
                    			  CheckNULLTerminatedinKEY3DS01 (&db_recs.key3ds01,MODE_INSERT);
                    	     }
                           result_code = db_insert_key3ds01( &db_recs.key3ds01, ErrorMsg );
                           break;

	  case CAF01_EMV_DATA:

         PRINT( "request to insert caf01 record\n" );

         /* retrieve the data into a caf01 structure and insert into database */
         memset( &db_recs.caf01_emv, 0, sizeof( db_recs.caf01_emv ) );
         memcpy( &db_recs.caf01_emv, ( pCAF01_EMV )p_data, sizeof( CAF01_EMV ) );
  	     if (DB_NullCheck_Flag==1)
  	     {
  			  CheckNULLTerminatedinCAF01_EMV (&db_recs.caf01_emv,MODE_INSERT);
  	     }
         result_code = db_insert_caf01_emv( &db_recs.caf01_emv, ErrorMsg );
         break;

      case MCC_LIMITS_DATA:

         memset( &db_recs.mcc_limits, 0, sizeof( db_recs.mcc_limits ) );
         memcpy( &db_recs.mcc_limits, (pMCC_LIMITS)p_data, sizeof(MCC_LIMITS) );
  	     if (DB_NullCheck_Flag==1)
  	     {
  			  CheckNULLTerminatedinMCC_LIMITS (&db_recs.mcc_limits,MODE_INSERT);
  	     }
         result_code = db_insert_mcc_limits( &db_recs.mcc_limits, ErrorMsg );
         break;


	  case COUNTRY_AMT_LIMITS_DATA:
         memset( &db_recs.country_amt_limits, 0x00, sizeof( db_recs.country_amt_limits ) );
         memcpy( &db_recs.country_amt_limits, (pCOUNTRY_AMT_LIMITS)p_data, sizeof(COUNTRY_AMT_LIMITS) );

         result_code = db_insert_cntry_amt_limits( &db_recs.country_amt_limits, ErrorMsg );
         break;


      case ATP02_DATA:

         PRINT( "request to insert atp02 record\n" );

         /* retrieve the data into a atp02 structure and insert into database */
         memset( &db_recs.atp02, 0, sizeof( db_recs.atp02 ) );
         memcpy( &db_recs.atp02, ( pATP02 )p_data, sizeof( ATP02 ) );
  	     if (DB_NullCheck_Flag==1)
  	     {
  			  CheckNULLTerminatedinATP02 (&db_recs.atp02,MODE_INSERT);
  	     }
         result_code = db_insert_atp02( &db_recs.atp02, ErrorMsg );
         break;

      case ATP01_DATA:

         PRINT( "request to insert atp01 record\n" );

         /* retrieve the data into a ucf01 structure and insert into database */
         memset( &db_recs.atp01, 0, sizeof( db_recs.atp01 ) );
         memcpy( &db_recs.atp01, ( pATP01 )p_data, sizeof( ATP01 ) );
  	     if (DB_NullCheck_Flag==1)
  	     {
  			  CheckNULLTerminatedinATP01 (&db_recs.atp01,MODE_INSERT);
  	     }
         result_code = db_insert_atp01( &db_recs.atp01, ErrorMsg );
         break;

      case UCF01_DATA:

         PRINT( "request to insert ucf01 record\n" );

         /* retrieve the data into a ucf01 structure and insert into database */
         memset( &db_recs.ucf01, 0, sizeof( db_recs.ucf01 ) );
         memcpy( &db_recs.ucf01, ( pUCF01 )p_data, sizeof( UCF01 ) );
  	     if (DB_NullCheck_Flag==1)
  	     {
  			  CheckNULLTerminatedinUCF01 (&db_recs.ucf01,MODE_INSERT);
  	     }
         result_code = db_insert_ucf01( &db_recs.ucf01, ErrorMsg );
         break;
		  /* Added UCF03 Table code For PCI Changes - TF -Srikanth*/
	  case UCF03_DATA:

         PRINT( "request to insert ucf03 record\n" );

         /* retrieve the data into a ucf01 structure and insert into database */
         memset( &db_recs.ucf03, 0, sizeof( db_recs.ucf03 ) );
         memcpy( &db_recs.ucf03, ( pUCF03 )p_data, sizeof( UCF03 ) );
  	     if (DB_NullCheck_Flag==1)
  	     {
  			  CheckNULLTerminatedinUCF03 (&db_recs.ucf03,MODE_INSERT);
  	     }
         result_code = db_insert_ucf03( &db_recs.ucf03, ErrorMsg );
         break;
      case UCF02_DATA:

         PRINT( "request to insert ucf02 record\n" );

         /* retrieve the data into a ucf02 structure and insert into database */
         memset( &db_recs.ucf02, 0, sizeof( db_recs.ucf02 ) );
         memcpy( &db_recs.ucf02, ( pUCF02 )p_data, sizeof( UCF02 ) );
  	     if (DB_NullCheck_Flag==1)
  	     {
  			  CheckNULLTerminatedinUCF02 (&db_recs.ucf02,MODE_INSERT);
  	     }
         result_code = db_insert_ucf02( &db_recs.ucf02, ErrorMsg );
         break;

     
      case BIN01_DATA:

         PRINT( "request to insert bin01 record\n" );

         /* retrieve the data into a bin01 structure and insert into database */
         memset( &db_recs.bin01, 0, sizeof( db_recs.bin01 ) );
         memcpy( &db_recs.bin01, ( pBIN01 )p_data, sizeof( BIN01 ) );
  	     if (DB_NullCheck_Flag==1)
  	     {
  			  CheckNULLTerminatedinBIN01 (&db_recs.bin01,MODE_INSERT);
  	     }
         result_code = db_insert_bin01( &db_recs.bin01, ErrorMsg );
         break;

      case HST01_DATA:

         PRINT( "request to insert hst01 record\n" );

         /* retrieve the data into a hst01 structure and insert into database */
         memset( &db_recs.hst01, 0, sizeof( db_recs.hst01 ) );
         memcpy( &db_recs.hst01, ( pHST01 )p_data, sizeof( HST01 ) );
  	     if (DB_NullCheck_Flag==1)
  	     {
  			  CheckNULLTerminatedinHST01 (&db_recs.hst01,MODE_INSERT);
  	     }
         result_code = db_insert_hst01( &db_recs.hst01, ErrorMsg );
         OpenDSV_WriteHST01(&db_recs.hst01);

         break;
      case MIDBINBLK01_DATA:
      		PRINT( "request to insert midbinblk01 record\n" );

      		/* retrieve the data into a hst01 structure and insert into database */

      			memset( &db_recs.midbinblk01, 0, sizeof( db_recs.midbinblk01 ) );
      		    memcpy( &db_recs.midbinblk01, ( pMIDBINBLK01 )p_data, sizeof( MIDBINBLK01 ) );
      		  result_code = db_insert_binblkmid01( &db_recs.midbinblk01, ErrorMsg );

      		 break;

      case GPCF01_DATA:

               PRINT( "request to insert hst01 record\n" );

               /* retrieve the data into a hst01 structure and insert into database */
               memset( &db_recs.gpcf01, 0, sizeof( db_recs.gpcf01 ) );
               memcpy( &db_recs.gpcf01, ( pGPCF01 )p_data, sizeof( GPCF01 ) );
        	   if (DB_NullCheck_Flag==1)
        	   {
        		   CheckNULLTerminatedinGPCF01 (&db_recs.gpcf01,MODE_INSERT);
        	   }
               result_code = db_insert_gpcf01( &db_recs.gpcf01, ErrorMsg );
               break;

      case CR01_DATA:
    	  PRINT( "request to insert cr01 record\n" );
                     /* Retrieve the data into an cr01 structure and insert into database */
                      memset( &db_recs.cr01, 0, sizeof( db_recs.cr01 ) );
                      memcpy( &db_recs.cr01, ( pCR01)p_data, sizeof( CR01 ) );
            		  if (DB_NullCheck_Flag==1)
            		  {
            			  CheckNULLTerminatedinCR01 (&db_recs.cr01,MODE_INSERT);
            		  }
            		  result_code = db_insert_cr01( &db_recs.cr01, ErrorMsg);
            		  if(result_code != PTEMSG_OK)
            		  {
                          /* NewLog, Insert failed into cr01 */
                          memset(buffer,0x00,sizeof(buffer));
                          sprintf(buffer,"Insertion of cr01 failed, RuleID:%s",
                                          		 db_recs.cr01.rule_id);
                           trands_log_message(2,2,buffer,"process_insert_msg");

                  		  }

                           break;

      case AMEX3DS2_DATA:

               PRINT( "request to insert AMEX3DS2 record\n" );

               memset( &db_recs.amex3ds2, 0, sizeof( db_recs.amex3ds2 ) );
               memcpy( &db_recs.amex3ds2, ( pAMEX3DS2 )p_data, sizeof( AMEX3DS2 ) );
        	   if (DB_NullCheck_Flag==1)
        	   {
        		   CheckNULLTerminatedinAMEX3DS2 (&db_recs.amex3ds2,MODE_INSERT);
        	   }
               result_code = db_insert_amex3ds2( &db_recs.amex3ds2, ErrorMsg );
               break;



      case DINERS3DS2_DATA:

                     PRINT( "request to insert DINERS3DS2 record\n" );

                     memset( &db_recs.dci3ds2, 0, sizeof( db_recs.dci3ds2 ) );
                     memcpy( &db_recs.dci3ds2, ( pDCI3DS2 )p_data, sizeof( DCI3DS2 ) );
              	   if (DB_NullCheck_Flag==1)
              	   {
              		   CheckNULLTerminatedinDCI3DS2 (&db_recs.dci3ds2,MODE_INSERT);
              	   }
                     result_code = db_insert_dci3ds2( &db_recs.dci3ds2, ErrorMsg );
                     break;


      case CUP3DS2_DATA:

               PRINT( "request to insert CUP3DS2 record\n" );

               memset( &db_recs.cup3ds2, 0, sizeof( db_recs.cup3ds2 ) );
               memcpy( &db_recs.cup3ds2, ( pCUP3DS2 )p_data, sizeof( CUP3DS2 ) );
        	   if (DB_NullCheck_Flag==1)
        	   {
        		   CheckNULLTerminatedinCUP3DS2 (&db_recs.cup3ds2,MODE_INSERT);
        	   }
               result_code = db_insert_cup3ds2( &db_recs.cup3ds2, ErrorMsg );
               break;

     default:

         /* unknown app_data_type received. Log Event and return error message to client */
         result_code = PTEMSG_INVALID_DATATYPE;
         sprintf (ErrorMsg, "Unknown app_data_type for Insert request from %s", reply_que);
         break;
   }


   if (result_code == PTEMSG_OK)
   {
      
      updates_to_db_made = true;

      p_msg_out = ptemsg_build_msg (MT_DB_REPLY,
                                    ptemsg_get_msg_subtype1 (p_msg_in),
                                    ptemsg_get_msg_subtype2 (p_msg_in),
                                    reply_que,
                                    application_que_name,
                                    (pBYTE)AtpBuffer,
                                    ISO_MSG_LENGTH1 + sizeof(AUTH_TX),
                                    app_data_type);
   
      if (p_msg_out == NULL_PTR)
      {
         result_code = PTEMSG_INSUFFICIENT_MEMORY;
         sprintf (ErrorMsg, "Insufficient Memory to build Insert response to %s", reply_que);
      }
   }

   return (p_msg_out);
}




/*************************************************************************************/
/*************************************************************************************/
pPTE_MSG process_update_msg( pPTE_MSG p_msg_in )
{
   pBYTE          p_data =NULL_PTR;
   pPTE_MSG_DATA  p_msg_data=NULL_PTR;
   pPTE_MSG       p_msg_out=NULL_PTR;
   pBYTE          p_current=NULL_PTR;
   pAUTH_TX       auth_tx;
   LONG           data_length=0;
   CHAR           buffer[200]={0};
   CHAR			  enc_dec_key[ENC_DEC_KEY_SIZE]="";	
   p_msg_out     = NULL_PTR;
   p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
   p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
   memcpy(AtpBuffer, p_data, sizeof(AUTH_TX) + ISO_MSG_LENGTH1);
   p_current     = AtpBuffer + ISO_MSG_LENGTH1 + sizeof(AUTH_TX);
   app_data_type = ptemsg_get_pte_msg_data_app_data_type( p_msg_data );
   p_data = p_data + ISO_MSG_LENGTH1 + sizeof(AUTH_TX);
   data_length = ISO_MSG_LENGTH1 + sizeof(AUTH_TX);
   auth_tx = (pAUTH_TX)p_data;
   

   switch( app_data_type )
   {
#ifdef HCTS2
	case ATP_CARD_STATUS_UPDATE_BY_BDO_HOST:

        PRINT( "request to update CCF02P & PAF01 status\n" );

        /* retrieve the data into a ocf20 structure and update the database */
        memset( &db_recs.ccf02p, 0, sizeof( db_recs.ccf02p ) );
        memcpy( &db_recs.ccf02p, ( pCCF02P )p_data, sizeof( CCF02P ) );
        if (DB_NullCheck_Flag==1)
		 {
			CheckNULLTerminatedinCCF02P (&db_recs.ccf02p,MODE_UPDATE);
		 }
        result_code = db_update_card_account_status( &db_recs.ccf02p, ErrorMsg );
        break;
#endif

	case OMC01_DATA:

         PRINT( "request to update clf01 record\n" );

         /* retrieve the data into a ocf20 structure and update the database */
         memset( &db_recs.omc01, 0, sizeof( db_recs.omc01 ) );
         memcpy( &db_recs.omc01, ( pOMC01 )p_data, sizeof( OMC01 ) );
         if (DB_NullCheck_Flag==1)
		 {
			CheckNULLTerminatedinOMC01 (&db_recs.omc01,MODE_UPDATE);
		 }
         result_code = db_update_omc01( &db_recs.omc01, ErrorMsg );
         break;
   	case ENC_DEC_DATA:
			
			PRINT( "request to update Enc/DEc Key record\n" );
			
			memset( &enc_dec_key, 0, sizeof( enc_dec_key ) );
			
			memcpy( &enc_dec_key, p_data, sizeof(enc_dec_key ) );
			result_code = update_dec_key( &enc_dec_key, ErrorMsg );
			if(result_code ==  PTEMSG_OK)
			result_code = update_enc_key( &enc_dec_key, ErrorMsg );
				
		  break;				
	  case CLF01_DATA:

         PRINT( "request to update clf01 record\n" );

         /* retrieve the data into a ocf20 structure and update the database */
         memset( &db_recs.clf01, 0, sizeof( db_recs.clf01 ) );
         memcpy( &db_recs.clf01, ( pCLF01 )p_data, sizeof( CLF01 ) );
         if (DB_NullCheck_Flag==1)
		 {
			CheckNULLTerminatedinCLF01 (&db_recs.clf01,MODE_UPDATE);
		 }
         result_code = db_update_clf01( &db_recs.clf01, ErrorMsg );
         break;
      
      case BPF01_DATA:

         PRINT( "request to update bpf01 record\n" );

         /* retrieve the data into a ocf20 structure and update the database */
         memset( &db_lists.Bpf01_Sch_List, 0, sizeof( db_lists.Bpf01_Sch_List ) );
         memcpy( &db_lists.Bpf01_Sch_List, ( pBPF01_SCH_LIST )p_data, sizeof( BPF01_SCH_LIST ) );
         if (DB_NullCheck_Flag==1)
		 {
			CheckNULLTerminatedinBPF01 (&db_recs.bpf01,MODE_UPDATE);
		 }
         result_code = db_update_bpf01( &db_lists.Bpf01_Sch_List, ErrorMsg );
         break;

      case JBF01_DATA:

         PRINT( "request to update jbf01 record\n" );

         /* retrieve the data into a ocf20 structure and update the database */
         memset( &db_recs.jbf01, 0, sizeof( db_recs.jbf01 ) );
         memcpy( &db_recs.jbf01, ( pJBF01 )p_data, sizeof( JBF01 ) );
         if (DB_NullCheck_Flag==1)
		 {
			CheckNULLTerminatedinJBF01 (&db_recs.jbf01,MODE_UPDATE);
		 }
         result_code = db_update_jbf01( &db_recs.jbf01, ErrorMsg );
         break;

      case APF01_DATA:

         PRINT( "request to update apf01 record\n" );

         /* retrieve the data into a ocf20 structure and update the database */
         memset( &db_recs.apf01, 0, sizeof( db_recs.apf01 ) );
         memcpy( &db_recs.apf01, ( pAPF01 )p_data, sizeof( APF01 ) );
         if (DB_NullCheck_Flag==1)
		 {
			CheckNULLTerminatedinAPF01 (&db_recs.apf01,MODE_UPDATE);
		 }
         result_code = db_update_apf01( &db_recs.apf01, ErrorMsg );
         break;

      case FAF01_DATA:

         PRINT( "request to update faf01 record\n" );

         /* retrieve the data into a ocf20 structure and update the database */
         memset( &db_recs.faf01, 0, sizeof( db_recs.faf01 ) );
         memcpy( &db_recs.faf01, ( pFAF01 )p_data, sizeof( FAF01 ) );
         if (DB_NullCheck_Flag==1)
		 {
			CheckNULLTerminatedinFAF01 (&db_recs.faf01,MODE_UPDATE);
		 }
         result_code = db_update_faf01( &db_recs.faf01, ErrorMsg );
         break;

      case CCF10_DATA:

         PRINT( "request to update ccf10 record\n" );

         /* retrieve the data into a ccf01 structure and update the database */
         memset( &db_recs.ccf10, 0, sizeof( db_recs.ccf10 ) );
         memcpy( &db_recs.ccf10, ( pCCF10 )p_data, sizeof( CCF10 ) );
         if (DB_NullCheck_Flag==1)
		 {
			CheckNULLTerminatedinCCF10 (&db_recs.ccf10,MODE_UPDATE);
		 }
         result_code = db_update_ccf10( &db_recs.ccf10, ErrorMsg );
         break;

		case BIN02_DATA:
			PRINT( "request to update bin02 record\n" );

         /* retrieve the data into a bin02 structure and update the database */
         memset( &db_recs.bin02, 0, sizeof( db_recs.bin02 ) );
         memcpy( &db_recs.bin02, ( pBIN02 )p_data, sizeof( BIN02 ) );
         if (DB_NullCheck_Flag==1)
		 {
			CheckNULLTerminatedinBIN02 (&db_recs.bin02,MODE_UPDATE);
		 }
         result_code = db_update_bin02( &db_recs.bin02, ErrorMsg );
		break ;


      case BON01_DATA:

         PRINT( "request to update bon01 record\n" );

         /* retrieve the data into a bon01 structure and update the database */
         memset( &db_recs.bon01, 0, sizeof( db_recs.bon01 ) );
         memcpy( &db_recs.bon01, ( pBON01 )p_data, sizeof( BON01 ) );
         if (DB_NullCheck_Flag==1)
		 {
			CheckNULLTerminatedinBON01 (&db_recs.bon01,MODE_UPDATE);
		 }
         result_code = db_update_bon01( &db_recs.bon01, ErrorMsg );
         break;  

      case BON02_DATA:

         memset( &db_recs.bon02, 0, sizeof( db_recs.bon02 ) );
         memcpy( &db_recs.bon02, (pBON02)p_data, sizeof(BON02) );
         if (DB_NullCheck_Flag==1)
		 {
			CheckNULLTerminatedinBON02 (&db_recs.bon02,MODE_UPDATE);
		 }
         result_code = db_update_bon02( &db_recs.bon02, ErrorMsg );

         /* Update Multi-Winner Response to be common for all records. */
         if ( result_code == PTEMSG_OK )
         {
            result_code = db_update_bon02_multi_winner_resp(&db_recs.bon02,ErrorMsg);
            if ( result_code != PTEMSG_OK )
            {
               memset( buffer, 0x00, sizeof(buffer) );
               sprintf( buffer,
                       " Unable to update Multi-Winner Resp for Cash Bonus: %s ",
                        db_recs.bon02.cash_id );
               trands_log_message(1,3,ErrorMsg," process_update_msg ");  // one for printing errormsg and other for buffer
               trands_log_message(1,3,buffer," process_update_msg ");
               result_code = PTEMSG_OK;
            }
         }
         break;  

      case CCF01_DATA:

         PRINT( "request to update ccf01 record\n" );

         /* retrieve the data into a ccf01 structure and update the database */
         memset( &db_recs.ccf01, 0, sizeof( db_recs.ccf01 ) );
         memcpy( &db_recs.ccf01, ( pCCF01 )p_data, sizeof( CCF01 ) );
         if (DB_NullCheck_Flag==1)
		 {
			CheckNULLTerminatedinCCF01 (&db_recs.ccf01,MODE_UPDATE);
		 }
         result_code = db_update_ccf01( &db_recs.ccf01, ErrorMsg );
         break;
		 
	  case MPPARA01_DATA:

         PRINT( "request to update MPPARA01 record\n" );

         /* retrieve the data into a ccf01 structure and update the database */
         memset( &db_recs.mppara01, 0, sizeof( db_recs.mppara01 ) );
         memcpy( &db_recs.mppara01, ( pMPPARA01)p_data, sizeof( MPPARA01) );
         if (DB_NullCheck_Flag==1)
		 {
			CheckNULLTerminatedinMPPARA01 (&db_recs.mppara01,MODE_UPDATE);
		 }
         result_code = db_update_mppara01( &db_recs.mppara01, ErrorMsg );
         break;

		 

      case ACF01_DATA:

         PRINT( "request to update acf01 record\n" );

         /* retrieve the data into a acf01 structure and update the database */
         memset( &db_recs.acf01, 0, sizeof( db_recs.acf01 ) );
         memcpy( &db_recs.acf01, ( pACF01 )p_data, sizeof( ACF01 ) );
         if (DB_NullCheck_Flag==1)
		 {
			CheckNULLTerminatedinACF01 (&db_recs.acf01,MODE_UPDATE);
		 }
         result_code = db_update_acf01( &db_recs.acf01, ErrorMsg );
         break;

      case CCL01_DATA:

         PRINT( "request to update ccl01 record\n" );

         /* retrieve the data into a ccl01 structure and update the database */
         memset( &db_recs.ccl01, 0, sizeof( db_recs.ccl01 ) );
         memcpy( &db_recs.ccl01, ( pCCL01 )p_data, sizeof( CCL01 ) );
         if (DB_NullCheck_Flag==1)
		 {
			CheckNULLTerminatedinCCL01 (&db_recs.ccl01,MODE_UPDATE);
		 }
         result_code = db_update_ccl01( &db_recs.ccl01, ErrorMsg );
         break;

      case BDKKEY01_DATA:

         PRINT( "request to update bdkkey01 record\n" );

         /* retrieve the data into a bdkkey01 structure and update the database */
         memset( &db_recs.bdkkey01, 0, sizeof( db_recs.bdkkey01 ) );
         memcpy( &db_recs.bdkkey01, ( pBDKKEY01 )p_data, sizeof( BDKKEY01 ) );
         if (DB_NullCheck_Flag==1)
		 {
        	 CheckNULLTerminatedinBDKKEY01 (&db_recs.bdkkey01,MODE_UPDATE);
		 }
         result_code = db_update_bdkkey01( &db_recs.bdkkey01, ErrorMsg );
         break;

      case CAF01_KEYBLOCK_DATA:

         PRINT( "request to update caf01_keyblock01 record\n" );

         /* retrieve the data into a caf01_keyblock01 structure and update the database */
         memset( &db_recs.caf01_keyblock01, 0, sizeof( db_recs.caf01_keyblock01 ) );
         memcpy( &db_recs.caf01_keyblock01, ( pCAF01_KEYBLOCK )p_data, sizeof( CAF01_KEYBLOCK ) );
  	     if (DB_NullCheck_Flag==1)
  	     {
  	    	CheckNULLTerminatedinCAF01_KEYBLOCK01 (&db_recs.caf01_keyblock01,MODE_INSERT);
  	     }
         result_code = db_update_caf01_keyblock01( &db_recs.caf01_keyblock01, ErrorMsg );
         break;

      case CRF01_DATA:

         PRINT( "request to update crf01 record\n" );

         /* retrieve the data into a bin01 structure and update the database */
         memset( &db_recs.crf01, 0, sizeof( db_recs.crf01 ) );
         memcpy( &db_recs.crf01, ( pCRF01 )p_data, sizeof( CRF01 ) );
         if (DB_NullCheck_Flag==1)
		 {
			CheckNULLTerminatedinCRF01 (&db_recs.crf01,MODE_UPDATE);
		 }
         result_code = db_update_crf01( &db_recs.crf01, ErrorMsg );
         break;

      case PAF01_DATA:

         PRINT( "request to update paf01 record\n" );

         /* retrieve the data into a bin01 structure and update the database */
         memset( &db_recs.paf01, 0, sizeof( db_recs.paf01 ) );
         memcpy( &db_recs.paf01, ( pPAF01 )p_data, sizeof( PAF01 ) );
         if (DB_NullCheck_Flag==1)
		 {
			CheckNULLTerminatedinPAF01 (&db_recs.paf01,MODE_UPDATE);
		 }
         result_code = db_update_paf01( &db_recs.paf01, ErrorMsg );
         break;

      case DEF01_DATA:

         PRINT( "request to update def01 record\n" );

         /* retrieve the data into a def01 structure and update the database */
         memset( &db_recs.def01, 0, sizeof( db_recs.def01 ) );
         memcpy( &db_recs.def01, ( pDEF01 )p_data, sizeof( DEF01 ) );
         if (DB_NullCheck_Flag==1)
		 {
			CheckNULLTerminatedinDEF01 (&db_recs.def01,MODE_UPDATE);
		 }
         result_code = db_update_def01( &db_recs.def01, ErrorMsg );
         break;

      case CSF01_DATA:

         PRINT( "request to update csf01 record\n" );

         /* retrieve the data into a bin01 structure and update the database */
         memset( &db_recs.csf01, 0, sizeof( db_recs.csf01 ) );
         memcpy( &db_recs.csf01, ( pCSF01 )p_data, sizeof( CSF01 ) );
         if (DB_NullCheck_Flag==1)
		 {
			CheckNULLTerminatedinCSF01 (&db_recs.csf01,MODE_UPDATE);
		 }
         result_code = db_update_csf01( &db_recs.csf01, ErrorMsg );
         break;

      case FCF02_DATA:

         PRINT( "request to update fcf02 record\n" );

         /* retrieve the data into a fcf02 structure and update the database */
         memset( &db_recs.fcf02, 0, sizeof( db_recs.fcf02 ) );
         memcpy( &db_recs.fcf02, ( pFCF02 )p_data, sizeof( FCF02 ) );
         if (DB_NullCheck_Flag==1)
		 {
			CheckNULLTerminatedinFCF02 (&db_recs.fcf02,MODE_UPDATE);
		 }
         result_code = db_update_fcf02( &db_recs.fcf02, ErrorMsg );
         break;

      case CCF02P_DATA:

         PRINT( "request to update ccf02p record\n" );

         /* retrieve the data into a ccf02p structure and update the database */
         memset( &db_recs.ccf02p, 0, sizeof( db_recs.ccf02p ) );
         memcpy( &db_recs.ccf02p, ( pCCF02P )p_data, sizeof( CCF02P ) );
         if (DB_NullCheck_Flag==1)
		 {
			CheckNULLTerminatedinCCF02P (&db_recs.ccf02p,MODE_UPDATE);
		 }
         result_code = db_update_ccf02p( &db_recs.ccf02p, ErrorMsg );
         break;

      case CCF04_DATA:

         PRINT( "request to update ccf04 record\n" );

         /* retrieve the data into a ccf04 structure and update the database */
         memset( &db_recs.ccf04, 0, sizeof( db_recs.ccf04 ) );
         memcpy( &db_recs.ccf04, ( pCCF04 )p_data, sizeof( CCF04 ) );
         if (DB_NullCheck_Flag==1)
		 {
			CheckNULLTerminatedinCCF04 (&db_recs.ccf04,MODE_UPDATE);
		 }
         result_code = db_update_ccf04( &db_recs.ccf04, ErrorMsg );
         break;

      case CCF03P_DATA:

         PRINT( "request to update ccf03p record\n" );

         /* retrieve the data into a bin01 structure and update the database */
         memset( &db_recs.ccf03p, 0, sizeof( db_recs.ccf03p ) );
         memcpy( &db_recs.ccf03p, ( pCCF03 )p_data, sizeof( CCF03 ) );
         if (DB_NullCheck_Flag==1)
		 {
			CheckNULLTerminatedinCCF03 (&db_recs.ccf03p,MODE_UPDATE);
		 }
         result_code = db_update_ccf03p( &db_recs.ccf03p, ErrorMsg );
         break;

      case OFFUS_VELOCITY_DATA:

         PRINT( "request to update offline_velocity record\n" );

         /* retrieve the data into a bin01 structure and update the database */
         memset( &db_recs.offus_velocity, 0, sizeof( db_recs.offus_velocity ) );
         memcpy( &db_recs.offus_velocity, ( pOffus_velocity)p_data, sizeof( Offus_velocity ) );
         if (DB_NullCheck_Flag==1)
		 {
			CheckNULLTerminatedinOffus_velocity(&db_recs.offus_velocity,MODE_UPDATE);
		 }
         result_code = db_update_offus_velocity( &db_recs.offus_velocity, ErrorMsg );
         break;

      case MCT01_DATA:

         PRINT( "request to update mct01 record\n" );

         /* retrieve the data into a bin01 structure and update the database */
         memset( &db_recs.mct01, 0, sizeof( db_recs.mct01 ) );
         memcpy( &db_recs.mct01, ( pMCT01 )p_data, sizeof( MCT01 ) );
         if (DB_NullCheck_Flag==1)
		 {
			CheckNULLTerminatedinMCT01 (&db_recs.mct01,MODE_UPDATE);
		 }
         result_code = db_update_mct01( &db_recs.mct01, ErrorMsg );
         break;

      case BIN01_DATA:

         PRINT( "request to update bin01 record\n" );

         /* retrieve the data into a bin01 structure and update the database */
         memset( &db_recs.bin01, 0, sizeof( db_recs.bin01 ) );
         memcpy( &db_recs.bin01, ( pBIN01 )p_data, sizeof( BIN01 ) );
         if (DB_NullCheck_Flag==1)
		 {
			CheckNULLTerminatedinBIN01 (&db_recs.bin01,MODE_UPDATE);
		 }
         result_code = db_update_bin01( &db_recs.bin01, ErrorMsg );
         break;

      case CTF01_DATA:

         PRINT( "request to update ctf01 record\n" );

         /* retrieve the data into a ctf01 structure and update the database */
         memset( &db_recs.ctf01, 0, sizeof( db_recs.ctf01 ) );
         memcpy( &db_recs.ctf01, ( pCTF01 )p_data, sizeof( CTF01 ) );
         if (DB_NullCheck_Flag==1)
		 {
			CheckNULLTerminatedinCTF01 (&db_recs.bin01,MODE_UPDATE);
		 }
         result_code = db_update_ctf01( &db_recs.ctf01, ErrorMsg );
         break;

      case CAF01_DATA:

         PRINT( "request to update caf01 record\n" );

         /* retrieve the data into a caf01 structure and update the database */
         memset( &db_recs.caf01, 0, sizeof( db_recs.caf01 ) );
         memcpy( &db_recs.caf01, ( pCAF01 )p_data, sizeof( CAF01 ) );
         if (DB_NullCheck_Flag==1)
		 {
			CheckNULLTerminatedinCAF01 (&db_recs.caf01,MODE_UPDATE);
		 }
         result_code = db_update_caf01( &db_recs.caf01, ErrorMsg );
         break;

      case KEY3DS01_DATA:

            PRINT( "request to update key3ds01 record\n" );

            /* retrieve the data into a key3ds01 structure and update the database */
            memset( &db_recs.key3ds01, 0, sizeof( db_recs.key3ds01 ) );
            memcpy( &db_recs.key3ds01, ( pCAF01 )p_data, sizeof( CAF01 ) );
            if (DB_NullCheck_Flag==1)
      		 {
      			CheckNULLTerminatedinKEY3DS01 (&db_recs.key3ds01,MODE_UPDATE);
      		 }
            result_code = db_update_key3ds01( &db_recs.key3ds01, ErrorMsg );
            break;

	  case CAF01_EMV_DATA:

         PRINT( "request to update caf01_EMV record\n" );

         /* retrieve the data into a caf01 EMV structure and update the database */
         memset( &db_recs.caf01_emv, 0, sizeof( db_recs.caf01_emv ) );
         memcpy( &db_recs.caf01_emv, ( pCAF01_EMV )p_data, sizeof( CAF01_EMV) );
         if (DB_NullCheck_Flag==1)
		 {
			CheckNULLTerminatedinCAF01_EMV (&db_recs.caf01_emv,MODE_UPDATE);
		 }
         result_code = db_update_caf01_emv( &db_recs.caf01_emv, ErrorMsg );
         break;	
      case MCC_LIMITS_DATA:

         memset( &db_recs.mcc_limits, 0x00, sizeof( db_recs.mcc_limits ) );
         memcpy( &db_recs.mcc_limits, (pMCC_LIMITS)p_data, sizeof(MCC_LIMITS) );
         if (DB_NullCheck_Flag==1)
		 {
			CheckNULLTerminatedinMCC_LIMITS (&db_recs.mcc_limits,MODE_UPDATE);
		 }
         result_code = db_update_mcc_limits( &db_recs.mcc_limits, ErrorMsg );
         break;

	  case COUNTRY_AMT_LIMITS_DATA:
         memset( &db_recs.country_amt_limits, 0x00, sizeof( db_recs.country_amt_limits ) );
         memcpy( &db_recs.country_amt_limits, (pCOUNTRY_AMT_LIMITS)p_data, sizeof(COUNTRY_AMT_LIMITS) );
         if (DB_NullCheck_Flag==1)
		 {
			CheckNULLTerminatedinCOUNTRY_AMT_LIMITS (&db_recs.country_amt_limits,MODE_UPDATE);
		 }
         result_code = db_update_cntry_amt_limits( &db_recs.country_amt_limits, ErrorMsg );
         break;

     case ATP02_DATA:

         PRINT( "request to update atp02 record\n" );

         /* retrieve the data into a atp02 structure and update the database */
         memset( &db_recs.atp02, 0, sizeof( db_recs.atp02 ) );
         memcpy( &db_recs.atp02, ( pATP02 )p_data, sizeof( ATP02 ) );
         if (DB_NullCheck_Flag==1)
		 {
			CheckNULLTerminatedinATP02 (&db_recs.atp02,MODE_UPDATE);
		 }
         result_code = db_update_atp02( &db_recs.atp02, ErrorMsg );
         break;

     case ATP01_DATA:

         PRINT( "request to update atp01 record\n" );

         /* retrieve the data into a ATP01 structure and update the database */
         memset( &db_recs.atp01, 0, sizeof( db_recs.atp01 ) );
         memcpy( &db_recs.atp01, ( pATP01 )p_data, sizeof( ATP01 ) );
         if (DB_NullCheck_Flag==1)
		 {
			CheckNULLTerminatedinATP01 (&db_recs.atp01,MODE_UPDATE);
		 }
         result_code = db_update_atp01( &db_recs.atp01, ErrorMsg );
         break;

      case UCF01_DATA:

         PRINT( "request to update ucf01 record\n" );
          if(ptemsg_get_msg_subtype2 (p_msg_in) == ST2_DB_SELECT_UCF01_FOR_USR_STS_CNRL)
		  {
			 PRINT( "request to retrieve ucf01 list by User status\n" );
	         memset( &db_recs.ucf01, 0, sizeof( db_recs.ucf01 ) );
	         memcpy( &db_recs.ucf01, ( pATP01 )p_data, sizeof( UCF01 ) );

			 if( ( result_code = db_get_ucf01_list_by_User_Status(&db_recs.ucf01 , ErrorMsg ) )== PTEMSG_OK )
			 {
				memcpy(p_current, (BYTE *)&db_recs.ucf01, sizeof( db_recs.ucf01 ));
				p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
							ptemsg_get_msg_subtype1 (p_msg_in),
							ptemsg_get_msg_subtype2 (p_msg_in),
							reply_que,
							application_que_name,
							(pBYTE)AtpBuffer,
							ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_recs.ucf01),
							UCF01_DATA);

			 }

		  }
          else
          {
        	  /* retrieve the data into a ucf01 structure and update the database */
        	  memset( &db_recs.ucf01, 0, sizeof( db_recs.ucf01 ) );
        	  memcpy( &db_recs.ucf01, ( pUCF01 )p_data, sizeof( UCF01 ) );

/*			#ifdef _DEBUG
        	  	 CheckNULLTerminatedinUCF01 (&db_recs.ucf01,MODE_UPDATE);
			#endif */
                 if (DB_NullCheck_Flag==1)
        		 {
        			CheckNULLTerminatedinUCF01 (&db_recs.ucf01,MODE_UPDATE);
        		 }

        	  result_code = db_update_ucf01( &db_recs.ucf01, ErrorMsg );
          }
        	  break;
	  
	  
/*	  //Comented Ucf03 as it is not required in ATPGen2

		//Added UCF03 Table Code for PCI Changes by TF-Srikanth 
		case UCF03_DATA:

         PRINT( "request to update ucf03 record\n" );

         // retrieve the data into a ucf03 structure and update the database 
         memset( &db_recs.ucf03, 0, sizeof( db_recs.ucf03 ) );
         memcpy( &db_recs.ucf03, ( pUCF03 )p_data, sizeof( UCF03 ) );
         result_code = db_update_ucf03( &db_recs.ucf03, ErrorMsg );
         break;
*/
      case UCF02_DATA:

         PRINT( "request to update ucf02 record\n" );

         /* retrieve the data into a ucf01 structure and update the database */
         memset( &db_recs.ucf02, 0, sizeof( db_recs.ucf02 ) );
         memcpy( &db_recs.ucf02, ( pUCF02 )p_data, sizeof( UCF02 ) );
         if (DB_NullCheck_Flag==1)
		 {
			CheckNULLTerminatedinUCF02 (&db_recs.ucf02,MODE_UPDATE);
		 }
         result_code = db_update_ucf02( &db_recs.ucf02, ErrorMsg );
         break;


      case UAG01_DATA:

         memset( &db_recs.uag01, 0x00, sizeof(db_recs.uag01) );
         memcpy( &db_recs.uag01, (pUAG01)p_data, sizeof(UAG01) );
         if (DB_NullCheck_Flag==1)
		 {
			CheckNULLTerminatedinUAG01 (&db_recs.uag01,MODE_UPDATE);
		 }
         result_code = db_update_uag01( &db_recs.uag01, ErrorMsg );
         break;


      case ALL_DATA:
         PRINT( "request to update ALL_DATA record\n");
         result_code = process_alldata_msg (p_msg_in );                 
         break;

	case DB_PASSWORD_DATA:
		 PRINT( "request to update DB PASSWORDS IN tf.ini file\n");
		 memset( &db_recs.dbpass, 0, sizeof( db_recs.dbpass ) );
         memcpy( &db_recs.dbpass,(pDbPasswords)p_data, sizeof(DbPasswords) );        

         result_code = process_db_password_change_msg( &db_recs.dbpass, ErrorMsg );               
         break;

 	case DB_ARCH_PASSWORD_DATA:
		 PRINT( "request to update ARCHIVE DB PASSWORDS IN tf.ini file\n");
		 memset( &db_recs.dbpass1, 0, sizeof( db_recs.dbpass1 ) );
         memcpy( &db_recs.dbpass1,(pArchDbPasswords)p_data, sizeof(ArchDbPasswords) ); 

         result_code = process_arch_db_password_change_msg( &db_recs.dbpass1, ErrorMsg );               
         break;

  	case DB_MRA_PASSWORD_DATA:
		 PRINT( "request to update MRA DB PASSWORDS IN tf.ini file\n");
		 memset( &db_recs.mra_dbpass, 0, sizeof( db_recs.mra_dbpass ) );
         memcpy( &db_recs.mra_dbpass,(pMraDbPasswords)p_data, sizeof(MraDbPasswords) ); 

         result_code = process_mra_db_password_change_msg( &db_recs.mra_dbpass, ErrorMsg );               
         break;
 case MIDBINBLK01_DATA:

  	  PRINT( "request to update  midbinblk01 record\n" );

  	  /* retrieve the data into a gpcf01 structure and insert into database */
  	  memset( &db_recs.midbinblk01, 0, sizeof( db_recs.midbinblk01 ) );
  	  memcpy( &db_recs.midbinblk01, ( pMIDBINBLK01 )p_data, sizeof( MIDBINBLK01 ) );
  	  result_code = db_update_binblkmid01( &db_recs.midbinblk01, ErrorMsg );
  	  break;

 case CR01_DATA:
	 	 PRINT( "request to update  cr01 record\n" );
          memset( &db_recs.cr01, 0, sizeof( db_recs.cr01 ) );
          memcpy( &db_recs.cr01, ( pCR01)p_data, sizeof( CR01 ) );
          if (DB_NullCheck_Flag==1)
        	{
        	  CheckNULLTerminatedinCR01(&db_recs.cr01,MODE_UPDATE);
        	}

            result_code = db_update_cr01(&db_recs.cr01, ErrorMsg);
            if(result_code != PTEMSG_OK)
            {
                /* NewLog, Update failed into cr01 */
                memset(buffer,0x00,sizeof(buffer));
                sprintf(buffer,"Updating of cr01 failed, Rule ID:%s",
                                      		 db_recs.cr01.rule_id);
                trands_log_message(2,2,buffer,"process_update_msg");

            }
            break;

    case GPCF01_DATA:

  	  PRINT( "request to update  gpcf01 record\n" );

  	  /* retrieve the data into a gpcf01 structure and insert into database */
  	  memset( &db_recs.gpcf01, 0, sizeof( db_recs.gpcf01 ) );
  	  memcpy( &db_recs.gpcf01, ( pGPCF01 )p_data, sizeof( GPCF01 ) );

  	  if (DB_NullCheck_Flag==1)
  	  {
  		CheckNULLTerminatedinGPCF01 (&db_recs.gpcf01,MODE_UPDATE);
  	  }
  	  result_code = db_update_gpcf01( &db_recs.gpcf01, ErrorMsg );

  	  break;

    case AMEX3DS2_DATA:

  	  PRINT( "request to update  AMEX3DS2 record\n" );

  	  /* retrieve the data into a amex3ds2 structure and insert into database */
  	  memset( &db_recs.amex3ds2, 0, sizeof( db_recs.amex3ds2 ) );
  	  memcpy( &db_recs.amex3ds2, ( pAMEX3DS2 )p_data, sizeof( AMEX3DS2 ) );
  	  if (DB_NullCheck_Flag==1)
  	  {
  		CheckNULLTerminatedinAMEX3DS2 (&db_recs.amex3ds2,MODE_UPDATE);
  	  }
  	  result_code = db_update_amex3ds2( &db_recs.amex3ds2, ErrorMsg );

  	  break;

    case DINERS3DS2_DATA:

          	  PRINT( "request to update  DCI3DS2 record\n" );

          	  memset( &db_recs.dci3ds2, 0, sizeof( db_recs.dci3ds2 ) );
          	  memcpy( &db_recs.dci3ds2, ( pDCI3DS2 )p_data, sizeof( DCI3DS2 ) );
          	  if (DB_NullCheck_Flag==1)
          	  {
          		CheckNULLTerminatedinDCI3DS2 (&db_recs.dci3ds2,MODE_UPDATE);
          	  }
          	  result_code = db_update_dci3ds2( &db_recs.dci3ds2, ErrorMsg );

          	  break;

    case JCB3DS2_DATA:

              	  PRINT( "request to update  JCB3DS2 record\n" );

              	  memset( &db_recs.jcb3ds2, 0, sizeof( db_recs.jcb3ds2 ) );
              	  memcpy( &db_recs.jcb3ds2, ( pJCB3DS2 )p_data, sizeof( JCB3DS2 ) );
              	  if (DB_NullCheck_Flag==1)
              	  {
              		CheckNULLTerminatedinJCB3DS2 (&db_recs.jcb3ds2,MODE_UPDATE);
              	  }
              	  result_code = db_update_jcb3ds2( &db_recs.jcb3ds2, ErrorMsg );

              	  break;

    case CUP3DS2_DATA:

  	  PRINT( "request to update  CUP3DS2 record\n" );


  	  memset( &db_recs.cup3ds2, 0, sizeof( db_recs.cup3ds2 ) );
  	  memcpy( &db_recs.cup3ds2, ( pCUP3DS2 )p_data, sizeof( CUP3DS2 ) );
  	  if (DB_NullCheck_Flag==1)
  	  {
  		CheckNULLTerminatedinCUP3DS2 (&db_recs.cup3ds2,MODE_UPDATE);
  	  }
  	  result_code = db_update_cup3ds2( &db_recs.cup3ds2, ErrorMsg );

  	  break;

      default:

         /* unknown app_data_type received. Log Event and return error message to client */
         result_code = PTEMSG_INVALID_DATATYPE;
         sprintf (ErrorMsg, "Unknown app_data_type: %s for Update request from %s", app_data_type,reply_que);
         break;
   }


   if (result_code == PTEMSG_OK)
   {
      // shared mem updates are moved to func. MsgHandler() //
      /*if((app_data_type == CAF01_DATA) && (Caf01InMem == true))
      {
         OffSet = FindMemRecord(p_data, sizeof(db_recs.caf01.primary_key), sizeof(CAF01), "Caf01Table");
         if(OffSet >= 0L)
            OffSet = UpdateMemRecord("Caf01Table", OffSet, sizeof(CAF01), p_data);
      }*/     
      
      updates_to_db_made = true;
      
      p_msg_out = ptemsg_build_msg (MT_DB_REPLY,
                                    ptemsg_get_msg_subtype1 (p_msg_in),
                                    ptemsg_get_msg_subtype2 (p_msg_in),
                                    reply_que,
                                    application_que_name,
                                    (pBYTE)AtpBuffer,
                                    data_length,
                                    app_data_type);
  
   
      if (p_msg_out == NULL_PTR)
      {
         result_code = PTEMSG_INSUFFICIENT_MEMORY;
         sprintf (ErrorMsg, "Insufficient Memory to build Update response to %s", reply_que);
      }
   }

   return (p_msg_out);
}


/*************************************************************************************/
/* Function    :  process_db_cleanup
   Parameters  :  pointer to a message of type pPTEMSG
   Return      :  None  
   Description :  Depending on the data type of the message( tlf01,ngf01 etc...), delete 
                  all the records that are older than DATE (sent as p_data)in the
                  corresponding table 
   Authour     :  Nov 17th'98, Supriya.
*/
/*************************************************************************************/
/*************************************************************************************/
void process_db_cleanup( pPTE_MSG p_msg_in ) 
{
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   pBYTE          p_current;
  
   p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
   p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
   memcpy(AtpBuffer, p_data, sizeof(AUTH_TX) + ISO_MSG_LENGTH1);
   p_current     = AtpBuffer + ISO_MSG_LENGTH1 + sizeof(AUTH_TX);
   app_data_type = ptemsg_get_pte_msg_data_app_data_type( p_msg_data );
   p_data = p_data + ISO_MSG_LENGTH1 + sizeof(AUTH_TX);

   switch( app_data_type )
   {
      case CCF10_DATA:
         
         PRINT( "request to delete ccf10 records less than date\n" );

         result_code = db_delete_ccf10_by_date( ( pCHAR )p_data, ErrorMsg );
         break;


      case HST01_DATA:

         PRINT( "request to delete hst01 records less than date\n" );

         result_code = db_delete_hst01_by_date( ( pCHAR )p_data, ErrorMsg );
         break;
        
      default:
         result_code = PTEMSG_INVALID_DATATYPE;
         sprintf (ErrorMsg, "Unknown app_data_type for CleanUp request from %s", reply_que);
         break;
   }
}


/*************************************************************************************/
/*************************************************************************************/
pPTE_MSG process_delete_msg( pPTE_MSG p_msg_in ) 
{
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   pPTE_MSG       p_msg_out;
   pBYTE          p_current;
   BYTE			user_record_by_group_count[20];
  
   p_msg_out     = NULL_PTR;
   p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
   p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
   memcpy(AtpBuffer, p_data, sizeof(AUTH_TX) + ISO_MSG_LENGTH1);
   p_current     = AtpBuffer + ISO_MSG_LENGTH1 + sizeof(AUTH_TX);
   app_data_type = ptemsg_get_pte_msg_data_app_data_type( p_msg_data );
   p_data = p_data + ISO_MSG_LENGTH1 + sizeof(AUTH_TX);
   CHAR   buffer[526]= {0};

   switch( app_data_type )
   {
		case BIN02_DATA:
			// request to delete the entire BIN02 table, called when
			// the entire PLUS table load 
			PRINT( "request to delete the BIN02 table \n" );
			result_code = db_delete_bin02 (ErrorMsg) ;
		break ;
      case CLF01_DATA:

         PRINT( "request to delete clf01 record\n" );

         /* retrieve the clf01 name and delete record from the database */
         result_code = db_delete_clf01( ( pCLF01 )p_data, ErrorMsg );
         break;

      case BPF01_DATA:
         if(ptemsg_get_msg_subtype2 (p_msg_in) == ST2_DB_DELETE_BPF01_BY_JOB_ID)
        {
            PRINT( "request to delete bpf01 record by job_id\n" );

            /* retrieve the bpf01 name and delete record from the database */
            result_code = db_delete_bpf01_by_job_id( ( pBPF01 )p_data, ErrorMsg );
         }
         else if(ptemsg_get_msg_subtype2 (p_msg_in) == ST2_DB_DELETE_BPF01_BY_PROFILE_ID)
        {
            PRINT( "request to delete bpf01 record by job_id\n" );

            /* retrieve the bpf01 name and delete record from the database */
            result_code = db_delete_bpf01_by_profile_id( ( pBPF01 )p_data, ErrorMsg );
         }         else
         {
            PRINT( "request to delete bpf01 record\n" );

            /* retrieve the bpf01 name and delete record from the database */
            result_code = db_delete_bpf01( ( pBPF01 )p_data, ErrorMsg );
         }
       
         break;

      case JBF01_DATA:
         if(ptemsg_get_msg_subtype2 (p_msg_in) == ST2_DB_DELETE_JBF01_BY_APP_NAME)
         {
            PRINT( "request to delete jbf01 record by app_name\n" );

            /* retrieve the bpf01 name and delete record from the database */
            result_code = db_delete_jbf01_by_app_name( ( pJBF01 )p_data, ErrorMsg );
         }
         else
         {
            PRINT( "request to delete jbf01 record\n" );

            /* retrieve the jbf01 name and delete record from the database */
            result_code = db_delete_jbf01( ( pJBF01 )p_data, ErrorMsg );
         }
         break;

      case APF01_DATA:

         PRINT( "request to delete apf01 record\n" );

         /* retrieve the apf01 name and delete record from the database */
         result_code = db_delete_apf01( ( pAPF01 )p_data, ErrorMsg );
         break;

      case FAF01_DATA:

         PRINT( "request to delete faf01 record\n" );

         /* retrieve the faf01 name and delete record from the database */
         result_code = db_delete_faf01( ( pFAF01 )p_data, ErrorMsg );
         break;

      case CCF10_DATA:

         PRINT( "request to delete ccf10 record\n" );

         /* retrieve the ccf01 name and delete record from the database */
         result_code = db_delete_ccf10( ( pCCF10 )p_data, ErrorMsg );
         break;

		case BON01_DATA:

         PRINT( "request to delete bon01 record\n" );

         /* delete record from the database */
         result_code = db_delete_bon01( ( pBON01 )p_data, ErrorMsg );
         break;

		case BON02_DATA:

         PRINT( "request to delete bon02 record\n" );

         /* delete record from the database */
         result_code = db_delete_bon02( (pBON02)p_data, ErrorMsg );
         break;

      case CCF01_DATA:

         PRINT( "request to delete ccf01 record\n" );

         /* retrieve the ccf01 name and delete record from the database */
         result_code = db_delete_ccf01( ( pCCF01 )p_data, ErrorMsg );
         break;

	 case MPPARA01_DATA:

         PRINT( "request to delete MPPARA01 record\n" );

         /* retrieve the ccf01 name and delete record from the database */
         result_code = db_delete_mppara01( ( pMPPARA01)p_data, ErrorMsg );
         break;

      case ACF01_DATA:

         PRINT( "request to delete acf01 record\n" );

         /* retrieve the mcf01 name and delete record from the database */
         result_code = db_delete_acf01( ( pACF01 )p_data, ErrorMsg );
         break;

      case CCL01_DATA:

         PRINT( "request to delete ccl01 record\n" );

         /* retrieve the ccl01 name and delete record from the database */
         result_code = db_delete_ccl01( ( pCCL01 )p_data, ErrorMsg );
         break;

      case BDKKEY01_DATA:

         PRINT( "request to delete bdkkey01 record\n" );

         /* retrieve the bdkkey01 name and delete record from the database */
         result_code = db_delete_bdkkey01( ( pBDKKEY01 )p_data, ErrorMsg );
         break;

      case CAF01_KEYBLOCK_DATA:

         PRINT( "request to delete caf01_keyblock01 record\n" );

         /* retrieve the caf01_keyblock01 name and delete record from the database */
         result_code = db_delete_caf01_keyblock01( ( pCAF01_KEYBLOCK )p_data, ErrorMsg );
         break;

      case CRF01_DATA:

         PRINT( "request to delete crf01 record\n" );

         /* retrieve the mcf01 name and delete record from the database */
         result_code = db_delete_crf01( ( pCRF01 )p_data, ErrorMsg );
         break;

      case PAF01_DATA:

         PRINT( "request to delete paf01 record\n" );

         /* retrieve the mcf01 name and delete record from the database */
         result_code = db_delete_paf01( ( pPAF01 )p_data, ErrorMsg );
         break;

      case DEF01_DATA:

         PRINT( "request to delete def01 record\n" );

         /* retrieve the def01 name and delete record from the database */
         result_code = db_delete_def01( ( pDEF01 )p_data, ErrorMsg );
         break;


      case CSF01_DATA:

         PRINT( "request to delete csf01 record\n" );

         /* retrieve the mcf01 name and delete record from the database */
         result_code = db_delete_csf01( ( pCSF01 )p_data, ErrorMsg );
         break;

      case FCF02_DATA:

         PRINT( "request to delete fcf02 record\n" );

         /* retrieve the ccf03p name and delete record from the database */
         result_code = db_delete_fcf02( ( pFCF02 )p_data, ErrorMsg );
         break;

      case CCF02P_DATA:

         PRINT( "request to delete ccf02p record\n" );

         /* retrieve the ccf03p name and delete record from the database */
         result_code = db_delete_ccf02p( ( pCCF02P )p_data, ErrorMsg );
         break;

      case CCF04_DATA:
		 if(ptemsg_get_msg_subtype2 (p_msg_in) == ST2_DB_DELETE_ALL_CCF04_MCC_RECORDS)
         {
            PRINT( "request to delete all mcc records for CCF04 purchase card\n" );

            /* retrieve the ccf04 records and delete records from the database */
            result_code = db_delete_ccf04_by_card( ( pCCF04 )p_data, ErrorMsg );
         }
         else
		 {
            PRINT( "request to delete ccf04 record\n" );

            /* retrieve the ccf03p name and delete record from the database */
            result_code = db_delete_ccf04( ( pCCF04 )p_data, ErrorMsg );
		  }
         break;

      case CCF03P_DATA:

         PRINT( "request to delete ccf03p record\n" );

         /* retrieve the ccf03p name and delete record from the database */
         result_code = db_delete_ccf03p( ( pCCF03 )p_data, ErrorMsg );
         break;

      case MCT01_DATA:

         PRINT( "request to delete mct01 record\n" );

         /* retrieve the ctf01 name and delete record from the database */
         result_code = db_delete_mct01( ( pMCT01 )p_data, ErrorMsg );
         break;

      case CTF01_DATA:

         PRINT( "request to delete ctf01 record\n" );

         /* retrieve the ctf01 name and delete record from the database */
         result_code = db_delete_ctf01( ( pCTF01 )p_data, ErrorMsg );
         break;

      case CAF01_DATA:

         PRINT( "request to delete caf01 record\n" );

         /* retrieve the caf01 name and delete record from the database */
         result_code = db_delete_caf01( ( pCAF01 )p_data, ErrorMsg );
         break;

       case KEY3DS01_DATA:

                    PRINT( "request to delete ctf01 record\n" );

              result_code = db_delete_key3ds01( ( pKEY3DS01 )p_data, ErrorMsg );
              break;

      case JCB3DS2_DATA:

               PRINT( "request to delete JCB3DS2 record\n" );

               /* retrieve the caf01 name and delete record from the database */
               result_code = db_delete_jcb3ds2( ( pJCB3DS2 )p_data, ErrorMsg );
               break;

	   case CAF01_EMV_DATA:

         PRINT( "request to delete caf01_EMV record\n" );

         /* retrieve the caf01 name and delete record from the database */
         result_code = db_delete_caf01_emv( ( pCAF01_EMV)p_data, ErrorMsg );
         break;

      case MCC_LIMITS_DATA:

         result_code = db_delete_mcc_limits( (pMCC_LIMITS)p_data, ErrorMsg );
         break;

	  case COUNTRY_AMT_LIMITS_DATA:
		  result_code = db_delete_cntry_amt_limits( (pCOUNTRY_AMT_LIMITS)p_data, ErrorMsg );
		  break;

      case ATP02_DATA:

         PRINT( "request to delete atp02 record\n" );

         /* retrieve the ucf01 name and delete record from the database */
         result_code = db_delete_atp02( ( pATP02 )p_data, ErrorMsg );
         break;

      case ATP01_DATA:

         PRINT( "request to delete atp01 record\n" );

         /* retrieve the ucf01 name and delete record from the database */
         result_code = db_delete_atp01( ( pATP01 )p_data, ErrorMsg );
         break;

      case BIN01_DATA:

         PRINT( "request to delete bin01 record\n" );

         /* retrieve the ucf01 name and delete record from the database */
         result_code = db_delete_bin01( ( pBIN01 )p_data, ErrorMsg );
         break;


      case UCF01_DATA:

         PRINT( "request to delete ucf01 record\n" );

         /* retrieve the ucf01 name and delete record from the database */
         result_code = db_delete_ucf01( ( pUCF01 )p_data, ErrorMsg );
         break;
	 /*
	 //Comented Ucf03 as it isnot required in ATPGen2
	  //Added UCF03 Table Code for PCI Changes by TF-Srikanth 
      case UCF03_DATA:

         PRINT( "request to delete ucf03 record\n" );

         // retrieve the ucf03 name and delete record from the database 
         result_code = db_delete_ucf03( ( pUCF03 )p_data, ErrorMsg );
         break;
	*/
      case UCF02_DATA:

         /* retrieve the ucf02 name and delete record from the database */
         PRINT( "request to delete ucf02 record\n" );
		 result_code = db_get_ucf01_count_by_group(( pUCF02 )p_data, user_record_by_group_count, ErrorMsg );
	   {
		   int count = atoi(user_record_by_group_count);
		   if(count > 0)
		   {
			   result_code = PTEMSG_COULD_NOT_FORWARD;
			   sprintf(ErrorMsg, "%d User(s) are associated for this group. Please delete all the users and delete this group",count);
			   break;
		   }

	   }
         result_code = db_delete_ucf02( ( pUCF02 )p_data, ErrorMsg );
         break;

      case OMC01_DATA:

         /* retrieve the omc01 name and delete record from the database */
         PRINT( "request to delete omc01 record\n" );
	     result_code = db_delete_omc01( ( pOMC01 )p_data, ErrorMsg );

         break;
	case MCC01_DATA:
		 
			/* retrieve the omc01 name and delete record from the database */
			PRINT( "request to delete omc01 record\n" );
			result_code = db_delete_mcc01( ( pMCC01 )p_data, ErrorMsg );
		 
			break;

	case CR01_DATA:

		PRINT( "request to delete cr01 record\n" );
	     result_code = db_delete_cr01( ( pCR01 )p_data, ErrorMsg );
	      if(result_code != PTEMSG_OK)
	       {
	          /* NewLog, Delete failed into cr01 */
	          memset(buffer,0x00,sizeof(buffer));
	          sprintf(buffer,"Deletion of cr01 failed, RuleID:%s",
	                                    		 db_recs.cr01.rule_id);
	          trands_log_message(2,2,buffer,"process_delete_msg");

	       }
	       break;

	   case MIDBINBLK01_DATA:

	         PRINT( "request to delete caf01 record\n" );

	         /* retrieve the caf01 name and delete record from the database */
	         result_code = db_delete_binblkmid01( (pMIDBINBLK01) p_data,  ErrorMsg);
	         break;

      default:
         /* unknown app_data_type received. Log Event and return error message to client */
         result_code = PTEMSG_INVALID_DATATYPE;
         sprintf (ErrorMsg, "Unknown app_data_type for Delete request from %s", reply_que);
         break;

   }


   if (result_code == PTEMSG_OK)
   {
      /* shared mem deletes moved to function PinnacleMsgHdlr */      
      updates_to_db_made = true;

      p_msg_out = ptemsg_build_msg (MT_DB_REPLY,
                                    ptemsg_get_msg_subtype1 (p_msg_in),
                                    ptemsg_get_msg_subtype2 (p_msg_in),
                                    reply_que,
                                    application_que_name,
                                    (pBYTE)AtpBuffer,
                                    ISO_MSG_LENGTH1 + sizeof(AUTH_TX),
                                    app_data_type);
   
      if (p_msg_out == NULL_PTR)
      {
         result_code = PTEMSG_INSUFFICIENT_MEMORY;
         sprintf (ErrorMsg, "Insufficient Memory to build Delete response to %s", reply_que);
      }
   }

   return (p_msg_out);
}

/*************************************************************************************/
/*************************************************************************************/

pPTE_MSG process_check_data_in_tables( pPTE_MSG p_msg_in )
{
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   pPTE_MSG       p_msg_out;
   pBYTE          p_current;

   p_msg_out     = NULL_PTR;
   p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
   p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
   memcpy(AtpBuffer, p_data, sizeof(AUTH_TX) + ISO_MSG_LENGTH1);
   p_current     = AtpBuffer + ISO_MSG_LENGTH1 + sizeof(AUTH_TX);
   app_data_type = ptemsg_get_pte_msg_data_app_data_type( p_msg_data );
   p_data = p_data + ISO_MSG_LENGTH1 + sizeof(AUTH_TX);
   switch( app_data_type )
   {
      case JBF01_DATA:
         PRINT(" request to check if data associated with app_name exists ");
         PRINT("\n");
         result_code = db_check_jbf01_by_app_name ( ( pCHAR)p_data,  ErrorMsg ) ;
         break;
      
      case BPF01_DATA:
         PRINT("request to check if data associated with job_id exists ");
         result_code = db_check_bpf01_by_job_id ( (pBPF01) p_data, ErrorMsg ); 
         break;
      
      default:
         /* unknown app_data_type received. Log Event and return error message to client */
       result_code = PTEMSG_INVALID_DATATYPE;
         sprintf (ErrorMsg, "Unknown app_data_type for check_data request from %s", reply_que);
         break;
   }
    p_msg_out = ptemsg_build_msg (MT_DB_REPLY,
                                    ptemsg_get_msg_subtype1 (p_msg_in),
                                    ptemsg_get_msg_subtype2 (p_msg_in),
                                    reply_que,
                                    application_que_name,
                                    (pBYTE)AtpBuffer,
                                    ISO_MSG_LENGTH1 + sizeof(AUTH_TX),
                                    app_data_type);
   
      if (p_msg_out == NULL_PTR)
      {
         result_code = PTEMSG_INSUFFICIENT_MEMORY;
         sprintf (ErrorMsg, "Insufficient Memory to build Delete response to %s", reply_que);
      }
       return (p_msg_out);
      
   }


/*************************************************************************************/
/*************************************************************************************/
pPTE_MSG process_select_msg( pPTE_MSG p_msg_in )
{
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   pPTE_MSG       p_msg_out;
   pBYTE          p_current;
   pAUTH_TX       auth_tx;
   BYTE           subtype1;
   BYTE           subtype2;
   INT            st2;
   int ipc_rcode =0;
   char temp_str[256]= {0};
   CHAR temp_mid[16] = {0};
   int  temp_mid_len = 0;
   int  db_mid_len   = 0;

   p_msg_out     = NULL_PTR;
   p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
   p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
   memcpy(AtpBuffer, p_data, sizeof(AUTH_TX) + ISO_MSG_LENGTH1);
   p_current     = AtpBuffer + ISO_MSG_LENGTH1 + sizeof(AUTH_TX);
   app_data_type = ptemsg_get_pte_msg_data_app_data_type( p_msg_data );
   auth_tx = (pAUTH_TX)p_data;
   p_data = p_data + ISO_MSG_LENGTH1 + sizeof(AUTH_TX);
   switch( app_data_type )
   {
#ifdef VISA7
   	   case MERCH_INT_PARAM_DATA:
   		PRINT( "request to retrieve merchant integrity parameters data\n" );
   		PRINT(p_data);
   		PRINT("\n");

		if( ( result_code = db_select_merchant_integrity_param( (pMERCH_INT_PARAMS)p_data, &db_recs.merch_int_param, ErrorMsg ) ) == PTEMSG_OK )
		{
			memcpy(p_current, (BYTE *)&db_recs.merch_int_param, sizeof( db_recs.merch_int_param ));
			p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
										  ptemsg_get_msg_subtype1 (p_msg_in),
										  ptemsg_get_msg_subtype2 (p_msg_in),
										  reply_que,
										  application_que_name,
										  (pBYTE)AtpBuffer,
										  sizeof(AUTH_TX) + sizeof(db_recs.merch_int_param),
										  MERCH_INT_PARAM_DATA);

		}
	   break;
#endif

#if(defined MC12_OCT2023 || defined MC13_OCT2023)
   	   case MERCH_INT_PARAM_MC_MAC_DATA:
   		PRINT( "request to retrieve merchant integrity parameters data\n" );
   		PRINT(p_data);
   		PRINT("\n");

		if( ( result_code = db_select_merchant_integrity_param_mc_and_mac( (pMERCH_INT_PARAMS)p_data, &db_recs.merch_int_param, ErrorMsg ) ) == PTEMSG_OK )
		{
			memcpy(p_current, (BYTE *)&db_recs.merch_int_param, sizeof( db_recs.merch_int_param ));
			p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
										  ptemsg_get_msg_subtype1 (p_msg_in),
										  ptemsg_get_msg_subtype2 (p_msg_in),
										  reply_que,
										  application_que_name,
										  (pBYTE)AtpBuffer,
										  sizeof(AUTH_TX) + sizeof(db_recs.merch_int_param),
										  MERCH_INT_PARAM_MC_MAC_DATA);

		}
	   break;
#endif

	   case MCF01_ID_DATA:
		PRINT( "request to retrieve mcf01 record\n" );

		PRINT(p_data);
		PRINT("\n");
		if( ( result_code = db_select_id_mcf01( ( pCHAR)p_data, &db_recs.mcf01, ErrorMsg ) ) == PTEMSG_OK )
		{
			memcpy(p_current, (BYTE *)&db_recs.mcf01, sizeof( db_recs.mcf01 ));
			p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
										  ptemsg_get_msg_subtype1 (p_msg_in),
										  ptemsg_get_msg_subtype2 (p_msg_in),
										  reply_que,
										  application_que_name,
										  (pBYTE)AtpBuffer,
										  sizeof(AUTH_TX) + sizeof(db_recs.mcf01),
										  MCF01_ID_DATA);

		}
		break;

		 case OMC01_DATA:
			 PRINT (" request to retrieve omc01 record\n");
			 
			 PRINT ("\n");
		 if( ( result_code = db_select_omc01( ( pCHAR)p_data, &db_recs.omc01, ErrorMsg ) ) == PTEMSG_OK )
		  {
			 memcpy(p_current, (BYTE *)&db_recs.omc01, sizeof( db_recs.omc01 ));
			 p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
										   ptemsg_get_msg_subtype1 (p_msg_in),
										   ptemsg_get_msg_subtype2 (p_msg_in),
										   reply_que,
										   application_que_name,
										   (pBYTE)AtpBuffer,
										   ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_recs.omc01),
										   OMC01_DATA);
		
		  }
		break;
		 case MCC01_DATA:
			 PRINT (" request to retrieve mcc01 record\n");
			 
			 PRINT ("\n");
         if(ptemsg_get_msg_subtype2 (p_msg_in) == ST2_DB_SELECT_MCC_CODES)
		{
		printf("ST2_DB_SELECT_MCC_CODES\n");
		memset(&db_lists.Mcc01_List, 0, sizeof( db_lists.Mcc01_List));
		
			if( ( result_code = db_select_mcc01_list( ( pCHAR)p_data, &db_lists.Mcc01_List, ErrorMsg ) ) == PTEMSG_OK )	
			{
				memcpy(p_current, (BYTE *)&db_lists.Mcc01_List, sizeof( db_lists.Mcc01_List ));
			 p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
										   ptemsg_get_msg_subtype1 (p_msg_in),
										   ptemsg_get_msg_subtype2 (p_msg_in),
										   reply_que,
										   application_que_name,
										   (pBYTE)AtpBuffer,
										   ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_lists.Mcc01_List),
										   MCC01_DATA);
			}
		}
			 else if( ( result_code = db_select_mcc01( ( pCHAR)p_data, &db_recs.mcc01, ErrorMsg ) ) == PTEMSG_OK )
		  {
			 memcpy(p_current, (BYTE *)&db_recs.mcc01, sizeof( db_recs.mcc01 ));
			 p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
										   ptemsg_get_msg_subtype1 (p_msg_in),
										   ptemsg_get_msg_subtype2 (p_msg_in),
										   reply_que,
										   application_que_name,
										   (pBYTE)AtpBuffer,
										   ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_recs.mcc01),
										   MCC01_DATA);
		
		  }
		break;
       case CLF01_DATA:

         PRINT( "request to retrieve clf01 record\n" );

         PRINT(p_data);
         PRINT("\n");
         if( ( result_code = db_select_clf01( ( pCHAR)p_data, &db_recs.clf01, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_recs.clf01, sizeof( db_recs.clf01 ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_recs.clf01),
                                          CLF01_DATA);

         }
         break;

      case BPF01_DATA:

         PRINT( "request to retrieve bpf01 record\n" );

         PRINT(p_data);
         PRINT("\n");
         if( ( result_code = db_select_bpf01( ( pCHAR)p_data, &db_recs.bpf01, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_recs.bpf01, sizeof( db_recs.bpf01 ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_recs.bpf01),
                                          BPF01_DATA);

         }
         break;

      case JBF01_DATA:

         PRINT( "request to retrieve jbf01 record\n" );

         PRINT(p_data);
         PRINT("\n");
         if( ( result_code = db_select_jbf01( ( pCHAR)p_data, &db_recs.jbf01, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_recs.jbf01, sizeof( db_recs.jbf01 ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_recs.jbf01),
                                          JBF01_DATA);

         }
         break;

      case APF01_DATA:

         PRINT( "request to retrieve apf01 record\n" );

         PRINT(p_data);
         PRINT("\n");
         if( ( result_code = db_select_apf01( ( pCHAR)p_data, &db_recs.apf01, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_recs.apf01, sizeof( db_recs.apf01 ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_recs.apf01),
                                          APF01_DATA);

         }
         break;

      case CR01_DATA:

                PRINT( "request to retrieve cr01 record\n" );

                PRINT(p_data);
                PRINT("\n");

          	            memset( &db_recs.cr01, 0, sizeof( db_recs.ucf01 ) );
          	            memcpy( &db_recs.cr01, ( pCR01 )p_data, sizeof( CR01 ) );

          	            if(ptemsg_get_msg_subtype2 (p_msg_in) == ST2_DB_SELECT_BIN_RANGE_CFB_TXN)
          	            {
          	            	if(0==strncmp(auth_tx->TLF01_details.handler_queue,"dcpimp",6))
          	            	{
          	            		if( ( result_code = db_select_bin_range_cr01_mp( auth_tx,(pCHAR)p_data, &db_recs.cr01, ErrorMsg ) ) == PTEMSG_OK )
          	            		{

          	          	              memcpy(p_current, (BYTE *)&db_recs.cr01, sizeof( db_recs.cr01 ));
          	          	              p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
          	          	                                            ptemsg_get_msg_subtype1 (p_msg_in),
          	          	                                            ptemsg_get_msg_subtype2 (p_msg_in),
          	          	                                            reply_que,
          	          	                                            application_que_name,
          	          	                                            (pBYTE)AtpBuffer,
          	          	                                            ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_recs.cr01),
          	          	                                            CR01_DATA);


          	            		}
          	            	}
          	            	else
          	            	{
          	            		if( ( result_code = db_select_bin_range_cr01_retail( auth_tx,(pCHAR)p_data, &db_recs.cr01, ErrorMsg ) ) == PTEMSG_OK )
          	            		{

          	          	              memcpy(p_current, (BYTE *)&db_recs.cr01, sizeof( db_recs.cr01 ));
          	          	              p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
          	          	                                            ptemsg_get_msg_subtype1 (p_msg_in),
          	          	                                            ptemsg_get_msg_subtype2 (p_msg_in),
          	          	                                            reply_que,
          	          	                                            application_que_name,
          	          	                                            (pBYTE)AtpBuffer,
          	          	                                            ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_recs.cr01),
          	          	                                            CR01_DATA);


          	            		}





          	            	}

          	            }
          	            else if( PTEMSG_OK == ( result_code = db_select_cr01( (pCHAR)p_data,
                                                                 &db_recs.cr01,
                                                                  ErrorMsg ) ) )
          	            {
          	            	memcpy(p_current, (BYTE *)&db_recs.cr01, sizeof( db_recs.cr01 ));
          	            	p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                                 ptemsg_get_msg_subtype1( p_msg_in ),
                                                 ptemsg_get_msg_subtype2( p_msg_in ),
                                                 reply_que,
                                                 application_que_name,
                                                 (pBYTE)AtpBuffer,
                                                  sizeof(AUTH_TX)+sizeof(db_recs.cr01),
            										CR01_DATA);

          	            }
          	            break;


      case FAF01_DATA:

         PRINT( "request to retrieve faf01 record\n" );

         PRINT(p_data);
         PRINT("\n");
         if( ( result_code = db_select_faf01( ( pCHAR)p_data, &db_recs.faf01, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_recs.faf01, sizeof( db_recs.faf01 ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_recs.faf01),
                                          FAF01_DATA);

         }
         break;

      case CCF10_DATA:

         PRINT( "request to retrieve ccf10 record\n" );

         PRINT(p_data);
         PRINT("\n");
         if( ( result_code = db_select_ccf10( ( pCHAR)p_data, &db_recs.ccf10, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_recs.ccf10, sizeof( db_recs.ccf10 ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_recs.ccf10),
                                          CCF10_DATA);

         }
         break;

      case MCF03P_DATA:

         PRINT( "request to retrieve MCF03P record\n" );

         PRINT(p_data);
         PRINT("\n");
         if( ( result_code = db_select_mcf03p( ( pCHAR)p_data, &db_recs.mcf03p, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_recs.mcf03p, sizeof( db_recs.mcf03p ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_recs.ccf10),
										  MCF03P_DATA);

         }
         break;

		case BIN02_DATA:
			PRINT( "request to retrieve bin02 record\n" );

         PRINT(p_data);
         PRINT("\n");
         if( ( result_code = db_select_bin02( (pCHAR)p_data, &db_recs.bin02, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_recs.bin02, sizeof( db_recs.bin02 ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_recs.bin02),
                                          BIN02_DATA);

         }


		break ;

      case BON01_DATA:

         PRINT( "request to retrieve bon01 record\n" );

         PRINT(p_data);
         PRINT("\n");
         if( ( result_code = db_select_bon01( &db_recs.bon01, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_recs.bon01, sizeof( db_recs.bon01 ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_recs.bon01),
                                          BON01_DATA);
         }
         break;


      case BON02_DATA:

         result_code = db_select_bon02((pCHAR)p_data, &db_recs.bon02, ErrorMsg);
         if ( result_code  == PTEMSG_OK )
         {
            memcpy( p_current, (BYTE *)&db_recs.bon02, sizeof(db_recs.bon02) );
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_recs.bon02),
                                          BON02_DATA);
         }
         break;


      case CCF01_DATA:

         PRINT( "request to retrieve ccf01 record\n" );

         PRINT(p_data);
         PRINT("\n");
         if( ( result_code = db_select_ccf01( ( pCHAR)p_data, &db_recs.ccf01, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_recs.ccf01, sizeof( db_recs.ccf01 ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_recs.ccf01),
                                          CCF01_DATA);

         }
         break;

	 case MPPARA01_DATA:

         PRINT( "request to retrieve mppara01 record\n" );

         PRINT(p_data);
         PRINT("\n");
         if( ( result_code = db_select_mppara01( ( pCHAR)p_data, &db_recs.mppara01, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_recs.mppara01, sizeof( db_recs.mppara01 ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_recs.mppara01),
                                          MPPARA01_DATA);

         }
         break;

      case ACF01_DATA:

         PRINT( "request to retrieve acf01 record\n" );

         PRINT(p_data);
         PRINT("\n");
         if( ( result_code = db_select_acf01( ( pCHAR)p_data, &db_recs.acf01, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_recs.acf01, sizeof( db_recs.acf01 ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_recs.acf01),
                                          ACF01_DATA);

         }
         break;

      case ACF01_DATA_RT:

          PRINT( "request to retrieve acf01 record\n" );

          PRINT(p_data);
          PRINT("\n");
          if( ( result_code = db_select_acf01( ( pCHAR)p_data, &db_recs.acf01, ErrorMsg ) ) == PTEMSG_OK )
          {
             memcpy(p_current, (BYTE *)&db_recs.acf01, sizeof( db_recs.acf01 ));
             p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                           ptemsg_get_msg_subtype1 (p_msg_in),
                                           ptemsg_get_msg_subtype2 (p_msg_in),
                                           reply_que,
                                           application_que_name,
                                           (pBYTE)AtpBuffer,
                                           ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_recs.acf01),
                                           ACF01_DATA_RT);

          }
          break;

      case CCL01_DATA:

          PRINT( "request to retrieve ccl01 record\n" );

          PRINT(p_data);
          PRINT("\n");
          if( ( result_code = db_select_ccl01( ( pCHAR)p_data, &db_recs.ccl01, ErrorMsg ) ) == PTEMSG_OK )
          {
             memcpy(p_current, (BYTE *)&db_recs.ccl01, sizeof( db_recs.ccl01 ));
             p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                           ptemsg_get_msg_subtype1 (p_msg_in),
                                           ptemsg_get_msg_subtype2 (p_msg_in),
                                           reply_que,
                                           application_que_name,
                                           (pBYTE)AtpBuffer,
                                           ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_recs.ccl01),
                                           CCL01_DATA);

          }
          break;

      case BDKKEY01_DATA:

          PRINT( "request to retrieve bdkkey01 record\n" );

          PRINT(p_data);
          PRINT("\n");
          if( ( result_code = db_select_bdkkey01( ( pCHAR)p_data, &db_recs.bdkkey01, ErrorMsg ) ) == PTEMSG_OK )
          {
             memcpy(p_current, (BYTE *)&db_recs.bdkkey01, sizeof( db_recs.bdkkey01 ));
             p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                           ptemsg_get_msg_subtype1 (p_msg_in),
                                           ptemsg_get_msg_subtype2 (p_msg_in),
                                           reply_que,
                                           application_que_name,
                                           (pBYTE)AtpBuffer,
                                           ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_recs.bdkkey01),
										   BDKKEY01_DATA);

          }
          break;

      case CAF01_KEYBLOCK_DATA:

          PRINT( "request to retrieve caf01_keyblock01 record\n" );

          PRINT(p_data);
          PRINT("\n");
          if( ( result_code = db_select_caf01_keyblock01( ( pCHAR)p_data, &db_recs.caf01_keyblock01, ErrorMsg ) ) == PTEMSG_OK )
          {
             memcpy(p_current, (BYTE *)&db_recs.caf01_keyblock01, sizeof( db_recs.caf01_keyblock01 ));
             p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                           ptemsg_get_msg_subtype1 (p_msg_in),
                                           ptemsg_get_msg_subtype2 (p_msg_in),
                                           reply_que,
                                           application_que_name,
                                           (pBYTE)AtpBuffer,
                                           ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_recs.caf01_keyblock01),
										   CAF01_KEYBLOCK_DATA);

          }
          break;
      case CRF01_DATA:

         PRINT( "request to retrieve crf01 record\n" );

         PRINT(p_data);
         PRINT("\n");
         if( ( result_code = db_select_crf01( ( pCHAR)p_data, &db_recs.crf01, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_recs.crf01, sizeof( db_recs.crf01 ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_recs.crf01),
                                          CRF01_DATA);

         }
         break;

      case PAF01_DATA:

         PRINT( "request to retrieve paf01 record\n" );

         PRINT(p_data);
         PRINT("\n");
         if( ( result_code = db_select_paf01( ( pCHAR)p_data, &db_recs.paf01, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_recs.paf01, sizeof( db_recs.paf01 ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_recs.paf01),
                                          PAF01_DATA);

         }
         break;

      case PAF01_DATA_RT:

         PRINT( "request to retrieve paf01 record\n" );

         PRINT(p_data);
         PRINT("\n");
         if( ( result_code = db_select_paf01( ( pCHAR)p_data, &db_recs.paf01, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_recs.paf01, sizeof( db_recs.paf01 ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_recs.paf01),
										  PAF01_DATA_RT);

         }
         break;
      case DEF01_DATA:

         PRINT( "request to retrieve def01 record\n" );

         PRINT(p_data);
         PRINT("\n");
         if( ( result_code = db_select_def01( ( pCHAR)p_data, &db_recs.def01, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_recs.def01, sizeof( db_recs.def01 ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_recs.def01),
                                          DEF01_DATA);

        }
         break;

      case CSF01_DATA:

         PRINT( "request to retrieve csf01 record\n" );

         PRINT(p_data);
         PRINT("\n");
         if( ( result_code = db_select_csf01( ( pCHAR)p_data, &db_recs.csf01, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_recs.csf01, sizeof( db_recs.csf01 ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_recs.csf01),
                                          CSF01_DATA);

         }
         break;

      case CCF03P_DATA:

         PRINT( "request to retrieve ccf03p record\n" );

         PRINT(p_data);
         PRINT("\n");
         if( ( result_code = db_select_ccf03p( ( pCHAR)p_data, &db_recs.ccf03p, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_recs.ccf03p, sizeof( db_recs.ccf03p ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_recs.ccf03p),
                                          CCF03P_DATA);

         }
         break;

      case OFFUS_VELOCITY_DATA:

         PRINT( "request to retrieve offus_velocity record\n" );

         PRINT(p_data);
         PRINT("\n");
         if( ( result_code = db_select_offus_velocity( ( pCHAR)p_data, &db_recs.offus_velocity, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_recs.offus_velocity, sizeof( db_recs.offus_velocity ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_recs.offus_velocity),
                                          OFFUS_VELOCITY_DATA);

              }
         break;

      case MCT01_DATA:

         PRINT( "request to retrieve mct01 record\n" );

         PRINT(p_data);
         PRINT("\n");
         /*Let us check if its CUP or Retail transction*/
         if(true == trands_Check_If_CUP_Transaction(auth_tx))
         {
        	result_code = db_select_mct01_for_CUP( ( pCHAR)p_data, &db_recs.mct01, ErrorMsg );
		    if( PTEMSG_OK == result_code)
        	{
				 memcpy(p_current, (BYTE *)&db_recs.mct01, sizeof( db_recs.mct01 ));
				 p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
											   ptemsg_get_msg_subtype1 (p_msg_in),
											   ptemsg_get_msg_subtype2 (p_msg_in),
											   reply_que,
											   application_que_name,
											   (pBYTE)AtpBuffer,
											   ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_recs.mct01),
											   MCT01_DATA);
        	}
         }
         else if( ( result_code = db_select_mct01( ( pCHAR)p_data, &db_recs.mct01, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_recs.mct01, sizeof( db_recs.mct01 ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_recs.mct01),
                                          MCT01_DATA);

         }
         break;

       case CTF01_DATA:

         PRINT( "request to retrieve ctf01 record\n" );

         PRINT(p_data);
         PRINT("\n");
         if( ( result_code = db_select_ctf01( ( pCHAR)p_data, &db_recs.ctf01, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_recs.ctf01, sizeof( db_recs.ctf01 ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_recs.ctf01),
                                          CTF01_DATA);

         }
         break;

       case CAF01_DATA:

         PRINT( "request to retrieve caf01 record\n" );

         PRINT(p_data);
         PRINT("\n");
         if( ( result_code = db_select_caf01( ( pCHAR)p_data, &db_recs.caf01, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_recs.caf01, sizeof( db_recs.caf01 ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_recs.caf01),
                                          CAF01_DATA);

         }
         break;

       case KEY3DS01_DATA:

             PRINT( "request to retrieve key3ds01 record\n" );

            PRINT(p_data);
               PRINT("\n");
               if( ( result_code = db_select_key3ds01( ( pCHAR)p_data, &db_recs.key3ds01, ErrorMsg ) ) == PTEMSG_OK )
               {
                   memcpy(p_current, (BYTE *)&db_recs.key3ds01, sizeof( db_recs.key3ds01 ));
                   p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                                ptemsg_get_msg_subtype1 (p_msg_in),
                                                ptemsg_get_msg_subtype2 (p_msg_in),
                                                reply_que,
                                                application_que_name,
                                                (pBYTE)AtpBuffer,
                                                ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_recs.key3ds01),
             									  KEY3DS01_DATA);

               }
              break;


		 case CAF01_EMV_DATA:

         PRINT( "request to retrieve caf01 EMV record\n" );

         PRINT(p_data);
         PRINT("\n");
         if( ( result_code = db_select_caf01_emv( ( pCHAR)p_data, &db_recs.caf01_emv, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_recs.caf01_emv, sizeof( db_recs.caf01_emv ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_recs.caf01_emv),
                                          CAF01_EMV_DATA);

         }
         break;
       case MCC_LIMITS_DATA:

         if( ( result_code = db_select_mcc_limits( (pCHAR)p_data, &db_recs.mcc_limits, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_recs.mcc_limits, sizeof( db_recs.mcc_limits ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_recs.mcc_limits),
                                          MCC_LIMITS_DATA);

         }
         break;

	  case COUNTRY_AMT_LIMITS_DATA:

         if( ( result_code = db_select_cntry_amt_limits( (pCHAR)p_data, &db_recs.country_amt_limits, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_recs.country_amt_limits, sizeof( db_recs.country_amt_limits ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_recs.country_amt_limits),
                                          COUNTRY_AMT_LIMITS_DATA);

         }
         break;

       case FCF02_DATA:

         PRINT( "request to retrieve fcf02 record\n" );

         PRINT(p_data);
         PRINT("\n");
         if( ( result_code = db_select_fcf02( ( pCHAR)p_data, &db_recs.fcf02, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_recs.fcf02, sizeof( db_recs.fcf02 ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_recs.fcf02),
                                          FCF02_DATA);

         }
         break;

       case CCF02P_DATA:

         PRINT( "request to retrieve ccf02p record\n" );

         PRINT(p_data);
         PRINT("\n");
         if( ( result_code = db_select_ccf02p( ( pCHAR)p_data, &db_recs.ccf02p, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_recs.ccf02p, sizeof( db_recs.ccf02p ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_recs.ccf02p),
                                          CCF02P_DATA);

         }
         break;
       case CCF02P_DATA_RT:

    	    PRINT( "request to retrieve ccf02p record\n" );

			PRINT(p_data);
			PRINT("\n");
			if( ( result_code = db_select_ccf02p( ( pCHAR)p_data, &db_recs.ccf02p, ErrorMsg ) ) == PTEMSG_OK )
			{
			   memcpy(p_current, (BYTE *)&db_recs.ccf02p, sizeof( db_recs.ccf02p ));
			   p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
											 ptemsg_get_msg_subtype1 (p_msg_in),
											 ptemsg_get_msg_subtype2 (p_msg_in),
											 reply_que,
											 application_que_name,
											 (pBYTE)AtpBuffer,
											 ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_recs.ccf02p),
											 CCF02P_DATA_RT);

			}
			break;

       case CCF04_DATA:

         PRINT( "request to retrieve ccf04 record\n" );

         PRINT(p_data);
         PRINT("\n");
         if( ( result_code = db_select_ccf04( ( pCHAR)p_data, &db_recs.ccf04, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_recs.ccf04, sizeof( db_recs.ccf04 ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_recs.ccf04),
                                          CCF04_DATA);

         }
         break;

       case ATP02_DATA:

         PRINT( "request to retrieve atp02 record\n" );

         PRINT(p_data);
         PRINT("\n");
         if( ( result_code = db_select_atp02( ( pCHAR )p_data, &db_recs.atp02, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_recs.atp02, sizeof( db_recs.atp02 ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_recs.atp02),
                                          ATP02_DATA);

         }
         break;

       case ATP01_DATA:

         PRINT( "request to retrieve atp01 record\n" );

         PRINT(p_data);
         PRINT("\n");
         if( ( result_code = db_select_atp01( ( pCHAR )p_data, &db_recs.atp01, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_recs.atp01, sizeof( db_recs.atp01 ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_recs.atp01),
                                          ATP01_DATA);

         }
         break;

      case UCF01_DATA:

         PRINT( "request to retrieve ucf01 record\n" );
         
         PRINT(p_data);
         PRINT("\n");
		 
         subtype2 =  ptemsg_get_msg_subtype2( p_msg_in );
		 if(subtype2 == ST2_DB_SELECT_FOR_DUALENCRYPTN_KEY_UPDAT_COUNT)
		  {
			  if( ( result_code = db_select_ucf01_ekey_updated_count( ( pCHAR )p_data, &db_recs.ucf01, ErrorMsg ) ) == PTEMSG_OK )
				  {
				  memcpy(p_current, (BYTE *)&db_recs.ucf01, sizeof( db_recs.ucf01 ));
				  p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
												ptemsg_get_msg_subtype1 (p_msg_in),
												ptemsg_get_msg_subtype2 (p_msg_in),
												reply_que,
												application_que_name,
												(pBYTE)AtpBuffer,
												ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_recs.ucf01),
												UCF01_DATA);
				 }
		  }

		else if(subtype2 == ST2_DB_SELECT_FOR_DUALENCRYPTN_OTHER_USER)
		  {
			  if( ( result_code = db_select_ucf01_other_dual_user( ( pCHAR )p_data, &db_recs.ucf01, ErrorMsg ) ) == PTEMSG_OK )
				  {
				  memcpy(p_current, (BYTE *)&db_recs.ucf01, sizeof( db_recs.ucf01 ));
				  p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
												ptemsg_get_msg_subtype1 (p_msg_in),
												ptemsg_get_msg_subtype2 (p_msg_in),
												reply_que,
												application_que_name,
												(pBYTE)AtpBuffer,
												ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_recs.ucf01),
												UCF01_DATA);
				 }
		  }

		else if(subtype2 == ST2_DB_SELECT_FOR_DUALENCRYPTN)
		 {
			 if( ( result_code = db_select_ucf01_dual_enc_count( ( pCHAR )p_data, &db_recs.ucf01, ErrorMsg ) ) == PTEMSG_OK )
				 {
				 memcpy(p_current, (BYTE *)&db_recs.ucf01, sizeof( db_recs.ucf01 ));
				 p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
											   ptemsg_get_msg_subtype1 (p_msg_in),
											   ptemsg_get_msg_subtype2 (p_msg_in),
											   reply_que,
											   application_que_name,
											   (pBYTE)AtpBuffer,
											   ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_recs.ucf01),
											   UCF01_DATA);
			 	}
		 }
		 else if( ( result_code = db_select_ucf01( ( pCHAR )p_data, &db_recs.ucf01, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_recs.ucf01, sizeof( db_recs.ucf01 ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_recs.ucf01),
                                          UCF01_DATA);

         }
         break;

//Comented Ucf03 as it is not being used in ATP Gen2
		/*Added UCF03 Table Data for PCI Changes by TF-Srikanth */
/*	 case UCF03_DATA:

         PRINT( "request to retrieve ucf03 record\n" );

         PRINT(p_data);
         PRINT("\n");
         if( ( result_code = db_select_ucf03( ( pCHAR )p_data, &db_recs.ucf03, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_recs.ucf03, sizeof( db_recs.ucf03 ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_recs.ucf03),
                                          UCF03_DATA);

         }
         break;
*/
/*Added DB Passwords Structure for Getting User ids , Old Passwords from tf.ini  file by TF-Srikanth */

	case DB_PASSWORD_DATA:
		 PRINT( "request to retrieve DB PASSWORDS IN tf.ini file\n");
		 PRINT(p_data);
         PRINT("\n");
		 memset(&db_recs.dbpass,0x00,sizeof(db_recs.dbpass));
		 if( ( result_code = process_db_select_dbpasswords(  &db_recs.dbpass, ErrorMsg ) ) == PTEMSG_OK )
         {

			memcpy(p_current, (BYTE *)&db_recs.dbpass, sizeof( db_recs.dbpass ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_recs.dbpass),
                                          DB_PASSWORD_DATA);

         }
         break;
       
	   case DB_ARCH_PASSWORD_DATA:
			PRINT( "request to retrieve ARCH DB PASSWORDS IN tf.ini file\n");
			PRINT(p_data);
			PRINT("\n");
			memset(&db_recs.dbpass1,0x00,sizeof(db_recs.dbpass1));
			if( ( result_code = process_arch_db_select_dbpasswords(	&db_recs.dbpass1, ErrorMsg ) ) == PTEMSG_OK )
			{
	   
			   memcpy(p_current, (BYTE *)&db_recs.dbpass1, sizeof( db_recs.dbpass1 ));
			   p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
											 ptemsg_get_msg_subtype1 (p_msg_in),
											 ptemsg_get_msg_subtype2 (p_msg_in),
											 reply_que,
											 application_que_name,
											 (pBYTE)AtpBuffer,
											 ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_recs.dbpass1),
											 DB_ARCH_PASSWORD_DATA);
	   
			}
			break;
		case DB_MRA_PASSWORD_DATA:
				 PRINT( "request to retrieve MRA DB PASSWORDS IN tf.ini file\n");
				 PRINT(p_data);
				 PRINT("\n");
				 memset(&db_recs.mra_dbpass,0x00,sizeof(db_recs.mra_dbpass));
				 if( ( result_code = process_mra_db_select_dbpasswords( &db_recs.mra_dbpass, ErrorMsg ) ) == PTEMSG_OK )
				 {
			
					memcpy(p_current, (BYTE *)&db_recs.mra_dbpass, sizeof( db_recs.mra_dbpass ));
					p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
												  ptemsg_get_msg_subtype1 (p_msg_in),
												  ptemsg_get_msg_subtype2 (p_msg_in),
												  reply_que,
												  application_que_name,
												  (pBYTE)AtpBuffer,
												  ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_recs.mra_dbpass),
												  DB_MRA_PASSWORD_DATA);
			
				 }
				 break;
		  

      case UCF02_DATA:

         PRINT( "request to retrieve ucf02 record\n" );

         PRINT(p_data);
         PRINT("\n");
         if( ( result_code = db_select_ucf02( ( pCHAR )p_data, &db_recs.ucf02, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_recs.ucf02, sizeof( db_recs.ucf02 ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_recs.ucf02),
                                          UCF02_DATA);

         }
         break;

      case BIN01_DATA:

         PRINT( "request to retrieve bin01 record\n" );

         PRINT(p_data);
         PRINT("\n");
         if( ( result_code = db_select_bin01( ( pCHAR )p_data, &db_recs.bin01, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_recs.bin01, sizeof( db_recs.bin01 ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_recs.bin01),
                                          BIN01_DATA);

         }
         break;

      case NEGATIVE_FRAUD_DATA:

         if( ( result_code = db_select_negative_fraud( (pCHAR)p_data, &db_recs.negative_fraud, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_recs.negative_fraud, sizeof( db_recs.negative_fraud ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_recs.negative_fraud),
                                          NEGATIVE_FRAUD_DATA);
         }
         break;

      case FGRAW_DATA:

         subtype1 =  ptemsg_get_msg_subtype1( p_msg_in );
         subtype2 =  ptemsg_get_msg_subtype2( p_msg_in );
         if ( subtype2 == ST2_NONE )
         {
            result_code = db_select_fgraw_single((pCHAR)p_data, &db_recs.fgraw, ErrorMsg );
            if( result_code == PTEMSG_OK )
            {
               memcpy( p_current, (pBYTE)&db_recs.fgraw, sizeof(db_recs.fgraw));
               p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                             subtype1,
                                             subtype2,
                                             reply_que,
                                             application_que_name,
                                             (pBYTE)AtpBuffer,
                                             ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_recs.fgraw),
                                             FGRAW_DATA);
            }
         }
         else
         {
            if ( (subtype2 == ST2_GET_ALL_FG_FAILED) || (subtype2 == ST2_GET_10_MORE_FG) )
               st2 = 1;
            else if ( subtype2 == ST2_GET_10_FG_FAILED )
               st2 = 2;

            result_code = db_select_fgraw_multiple((pCHAR)p_data, &db_lists.Fgraw_Gui_List, st2, ErrorMsg );
            if( result_code == PTEMSG_OK )
            {
               memcpy( p_current, (pBYTE)&db_lists.Fgraw_Gui_List, sizeof(db_lists.Fgraw_Gui_List));
               p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                             subtype1,
                                             subtype2,
                                             reply_que,
                                             application_que_name,
                                             (pBYTE)AtpBuffer,
                                             ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_lists.Fgraw_Gui_List),
                                             FGRAW_DATA);
            }
         }
         break;

      case MIDBINBLK01_DATA:


      		PRINT( "request to retrieve midbinblk01 record\n" );

      		PRINT(p_data);
      		PRINT("\n");

			if(0 == (strncmp(reply_que,"txcntl",6)))
			{
				result_code = db_select_binblkmid01(auth_tx, &db_recs.midbinblk01, ErrorMsg );
				// Compare the MID (Auth_Tx.TLF01_details.merchant.id)
				//50 time check, It its matach , then result as PTE_MSG_OK
				//Send message
               // strncpy(temp_mid,auth_tx->TLF01_details.merchant_id, (sizeof(temp_mid)-1));
                if( PTEMSG_OK == result_code)
                {
                	result_code = PTEMSG_NOT_FOUND;
                	strTrim_For_Mid_blocking(auth_tx->TLF01_details.merchant_id,&temp_mid);
					temp_mid_len = strlen(temp_mid);
					if(0 == temp_mid_len)
					{
						result_code = PTEMSG_NOT_FOUND;
					}
					else
					{
						for(int i=0;i<50;i++)
						{
							db_mid_len = strlen (db_recs.midbinblk01.merchant_blocking[i].merchant_id);
							if(db_mid_len == temp_mid_len)
							{
								if(0 == strncmp(temp_mid,db_recs.midbinblk01.merchant_blocking[i].merchant_id,
												 temp_mid_len))
								{
									 result_code = PTEMSG_OK;
									 break;
								}
								else if(db_recs.midbinblk01.merchant_blocking[i].merchant_id[0]==0x00)
								{
									result_code = PTEMSG_NOT_FOUND;
									break;
								}
							}
							else if(db_mid_len == 0)
							{
								result_code = PTEMSG_NOT_FOUND;
								break;
							}
						}
					}
                }
				 p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
											   ST1_DB_SELECT,
											   0,
											   reply_que,
											   application_que_name,
											   (pBYTE)AtpBuffer,
											   sizeof(AUTH_TX) ,
											   MIDBINBLK01_DATA);
				 ptemsg_set_result_code (p_msg_out, result_code);
			}
			else
			{
				result_code = db_select_gui_binblkmid01((pCHAR)p_data, &db_recs.midbinblk01, ErrorMsg );
				memcpy(p_current, (BYTE *)&db_recs.midbinblk01, sizeof( db_recs.midbinblk01 ));
					// Send message with entire data
				p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
											 ST1_DB_SELECT,
											  0,
											  reply_que,
											  application_que_name,
											  (pBYTE)AtpBuffer,
											  ISO_MSG_LENGTH1+sizeof(AUTH_TX) + sizeof(db_recs.midbinblk01),
											  MIDBINBLK01_DATA);
				ptemsg_set_result_code (p_msg_out, result_code);
			}


      		break;

      case GPCF01_DATA:

    	  PRINT( "request to retrieve gpcf01 record\n" );

    	  PRINT(p_data);
    	  PRINT("\n");
    	  if( ( result_code = db_select_gpcf01( ( pCHAR )p_data, &db_recs.gpcf01, ErrorMsg ) ) == PTEMSG_OK )
    	  {
    	        memcpy(p_current, (BYTE *)&db_recs.gpcf01, sizeof( db_recs.gpcf01 ));
    	        p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
    	                                ptemsg_get_msg_subtype1 (p_msg_in),
    	                                ptemsg_get_msg_subtype2 (p_msg_in),
    	                                reply_que,
    	                                application_que_name,
    	                                (pBYTE)AtpBuffer,
    	                                ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_recs.gpcf01),
										GPCF01_DATA);

    	  }

    	  break;

      case AMEX3DS2_DATA:

          	  PRINT( "request to retrieve AMEX3DS2 record\n" );

          	  PRINT(p_data);
          	  PRINT("\n");
          	  if( ( result_code = db_select_amex3ds2( ( pCHAR )p_data, &db_recs.amex3ds2, ErrorMsg ) ) == PTEMSG_OK )
          	  {
          	        memcpy(p_current, (BYTE *)&db_recs.amex3ds2, sizeof( db_recs.amex3ds2 ));
          	        p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
          	                                ptemsg_get_msg_subtype1 (p_msg_in),
          	                                ptemsg_get_msg_subtype2 (p_msg_in),
          	                                reply_que,
          	                                application_que_name,
          	                                (pBYTE)AtpBuffer,
          	                                ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_recs.amex3ds2),
											AMEX3DS2_DATA);

          	  }

          	  break;


      case DINERS3DS2_DATA:

               	  PRINT( "request to retrieve DINERS3DS2 record\n" );

               	  PRINT(p_data);
               	  PRINT("\n");
               	  if( ( result_code = db_select_dci3ds2( ( pCHAR )p_data, &db_recs.dci3ds2, ErrorMsg ) ) == PTEMSG_OK )
               	  {
               	        memcpy(p_current, (BYTE *)&db_recs.dci3ds2, sizeof( db_recs.dci3ds2 ));
               	        p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
               	                                ptemsg_get_msg_subtype1 (p_msg_in),
               	                                ptemsg_get_msg_subtype2 (p_msg_in),
               	                                reply_que,
               	                                application_que_name,
               	                                (pBYTE)AtpBuffer,
               	                                ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_recs.dci3ds2),
												DINERS3DS2_DATA);

               	  }
               	  break;


      case JCB3DS2_DATA:
                if( PTEMSG_OK == ( result_code = db_select_jcb3ds2( (pCHAR)p_data,
                                                                  &db_recs.jcb3ds2,
                                                                  ErrorMsg ) ) )
                {
                   memcpy(p_current, (BYTE *)&db_recs.jcb3ds2, sizeof( db_recs.jcb3ds2 ));
                   p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                                 ptemsg_get_msg_subtype1( p_msg_in ),
                                                 ptemsg_get_msg_subtype2( p_msg_in ),
                                                 reply_que,
                                                 application_que_name,
                                                 (pBYTE)AtpBuffer,
                                                 sizeof(AUTH_TX)+sizeof(db_recs.jcb3ds2),
                                                 JCB3DS2_DATA);

                }
                break;

      case CUP3DS2_DATA:

                	  PRINT( "request to retrieve CUP3DS2 record\n" );

                	  PRINT(p_data);
                	  PRINT("\n");
                	  if( ( result_code = db_select_cup3ds2( ( pCHAR )p_data, &db_recs.cup3ds2, ErrorMsg ) ) == PTEMSG_OK )
                	  {
                	        memcpy(p_current, (BYTE *)&db_recs.cup3ds2, sizeof( db_recs.cup3ds2 ));
                	        p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                	                                ptemsg_get_msg_subtype1 (p_msg_in),
                	                                ptemsg_get_msg_subtype2 (p_msg_in),
                	                                reply_que,
                	                                application_que_name,
                	                                (pBYTE)AtpBuffer,
                	                                ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_recs.cup3ds2),
													CUP3DS2_DATA);

                	  }
      break;



      default:

         /* unknown app_data_type received. Log Event and return error message to client */
         result_code = PTEMSG_INVALID_DATATYPE;
         sprintf (ErrorMsg, "Unknown app_data_type for Select request from %s", reply_que);
         break;

   }


   if ((result_code == PTEMSG_OK) && (p_msg_out == NULL_PTR) )
   {
      result_code = PTEMSG_INSUFFICIENT_MEMORY;
      sprintf (ErrorMsg, "Insufficient Memory to build Select response to %s", reply_que);
   }

   return (p_msg_out);
}


/*************************************************************************************/
/*************************************************************************************/
pPTE_MSG process_get_sch_list_msg( pPTE_MSG p_msg_in )
{
   LONG           data_len;
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   pPTE_MSG       p_msg_out;
   pBYTE          p_current;


   p_msg_out     = NULL_PTR;
   p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
   p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
   memcpy(AtpBuffer, p_data, sizeof(AUTH_TX) + ISO_MSG_LENGTH1);
   p_current     = AtpBuffer + ISO_MSG_LENGTH1 + sizeof(AUTH_TX);
   data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );
   app_data_type = ptemsg_get_pte_msg_data_app_data_type( p_msg_data );
   p_data = p_data + ISO_MSG_LENGTH1 + sizeof(AUTH_TX);

   switch( app_data_type )
   {  

      case BPF01_DATA:

         PRINT( "request to retrieve bpf01 list\n" );
         // copy the last caf01.name retrieved into the structure 
         if( ( result_code = db_get_bpf01_sch_list( ( pBPF01 )p_data, &db_lists.Bpf01_Sch_List, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_lists.Bpf01_Sch_List, sizeof( db_lists.Bpf01_Sch_List ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_lists.Bpf01_Sch_List),
                                          BPF01_DATA);
         }
         break;

      default:

         // unknown app_data_type received. Log Event and return error message to client
         result_code = PTEMSG_INVALID_DATATYPE;
         sprintf (ErrorMsg, "Unknown datatype for GetList request from %s", reply_que);
         break;
   }


   if ((result_code == PTEMSG_OK) && (p_msg_out == NULL_PTR) )
   {
      result_code = PTEMSG_INSUFFICIENT_MEMORY;
      sprintf (ErrorMsg, "Insufficient Memory to build GetAll response to %s", reply_que);
   }

   return (p_msg_out);
}

//	
/****************************added *************************************/
//Code to Store Encrypted password in tf.ini file
BYTE process_db_password_change_msg( pDbPasswords dbpass_ptr, pCHAR ErrorMsg )
{


	WORD key[] = {0x13A7, 0x6135, 0x9CDF, 0xA852};
    CHAR msgbuf[100] = "";
 	//Encrypt the Core password. 
	des_encryption( (unsigned char *)(dbpass_ptr->core_password),
						strlen((dbpass_ptr->core_password)), key, 1 );
	//Update the tf.ini with the user_id and password 
	SetCorePassword((dbpass_ptr->core_password)); 


	//Encrypt the Application password.
	des_encryption( (unsigned char *)(dbpass_ptr->app_password),
						strlen((dbpass_ptr->app_password)), key, 1 ); 
	//Update the tf.ini with the user_id and password  
	 SetAppPassword((dbpass_ptr->app_password)); 

   return PTEMSG_OK;
}

BYTE process_arch_db_password_change_msg( pArchDbPasswords dbpass_ptr, pCHAR ErrorMsg )
{
	WORD key[] = {0x13A7, 0x6135, 0x9CDF, 0xA852};
    CHAR msgbuf[100] = "";
 	//Encrypt the Arch password. 
	des_encryption( (unsigned char *)(dbpass_ptr->arch_password),
						strlen((dbpass_ptr->arch_password)), key, 1 );
	//Update the tf.ini with the user_id and password 
	SetArchPassword((dbpass_ptr->arch_password)); 
  return PTEMSG_OK;
}

BYTE process_mra_db_password_change_msg( pMraDbPasswords dbpass_ptr, pCHAR ErrorMsg )
{
	WORD key[] = {0x13A7, 0x6135, 0x9CDF, 0xA852};
    CHAR msgbuf[100] = "";
 	//Encrypt the Mra password. 
	printf("%s\n",dbpass_ptr->mra_password);
	des_encryption( (unsigned char *)(dbpass_ptr->mra_password),
						strlen((dbpass_ptr->mra_password)), key, 1 );
		printf("%s\n",dbpass_ptr->mra_password);
	//Update the tf.ini with the user_id and password 
	SetMraPassword((dbpass_ptr->mra_password)); 
  return PTEMSG_OK;
}

/****************************added *************************************/
BYTE process_alldata_msg (pPTE_MSG p_msg_in)
{
    BYTE           app_data_type;
    LONG           data_len;
    pBYTE          p_data;
    pPTE_MSG_DATA  p_msg_data;
    pPTE_MSG       p_msg_out;
    pBYTE          p_current;
    pAUTH_TX		   auth_tx;
    CHAR           primary_key[16] = "";
    CHAR           transaction_id [21] = "";
    CHAR           date_time[16] = "";
    CHAR           bch01_open_date[9]="";
    CHAR           bch01_open_time[7]="";

    double nConvertedTxAmount = 0.0 ;
    char strProcessingCode [20] = {0} ;
      

    p_msg_out     = NULL_PTR;
    p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
    p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
    memcpy(AtpBuffer, p_data, sizeof(AUTH_TX) + ISO_MSG_LENGTH1);
    p_current     = AtpBuffer + ISO_MSG_LENGTH1 + sizeof(AUTH_TX); 
    data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );
    app_data_type = ptemsg_get_pte_msg_data_app_data_type( p_msg_data );

    
	

	/***************************************/
    auth_tx = (pAUTH_TX)p_data;
	/***************************************/

    strncpy (strProcessingCode, auth_tx->TLF01_details.processing_code, 2) ;
   
	/* If the response code is TRANSACTION_APPROVED ("00") or if the response 
	   code is INCORRECT_PIN (55) or PIN_TRIES_EXCEEDED (75) update the tables 
       depending on the mask settings */
	/* If there was a problem with verifying the pin or if the number of failed
	   pin attempts exceeds the maximum allowable, response code will be set to
		INCORRECT_PIN (55) or (PIN_TRIES_EXCEEDED (75) - in this casem only CCF03P_MASK will be set */

   if ((!strcmp (auth_tx->TLF01_details.response_code ,"00")) ||
        (!strcmp (auth_tx->TLF01_details.response_code ,"75")) ||
		(!strcmp (auth_tx->TLF01_details.response_code ,"55")))
   {   
		/* if the CCF02P_MASK is set,Update the CCF02P table */
		/* if the CCF03P_MASK is set,Update the CCF03P table */
		/* if the ACF01_MASK is set,Update the ACF01 table */
		/* if the BON01_MASK is set,Update the BON02 table */
		
        if(auth_tx->TLF01_details.update_mask[0] & CCF02P_MASK)
        {
         
            PRINT("Before update CCF02P.....");
            if (DB_NullCheck_Flag==1)
   		 {
   			CheckNULLTerminatedinCCF02P (&db_recs.ccf02p,MODE_UPDATE);
   		 }

			result_code = db_update_ccf02p (&auth_tx->CCF02P_details, ErrorMsg) ;

            if (result_code != PTEMSG_OK)
                return(PTEMSG_UPDATE_FAILED);

            PRINT("After update CCF02P\n");
        }

        if ((auth_tx->TLF01_details.update_mask[0]) & CCF03P_MASK)
        {
            PRINT("Before CCF03P.....");
			// Depending on the ccf03_insert flag, either do a insert or an update  
			
			if (auth_tx->ccf03_insert)
			{
		  	     if (DB_NullCheck_Flag==1)
		  	     {
		  			  CheckNULLTerminatedinCCF03 (&db_recs.ccf03p,MODE_INSERT);
		  	     }
				result_code = db_insert_ccf03p (&auth_tx->CCF03P_details, ErrorMsg) ;

			}
				else
			{
		         if (DB_NullCheck_Flag==1)
				 {
					CheckNULLTerminatedinCCF03 (&db_recs.ccf03p,MODE_UPDATE);
				 }
				result_code = db_update_ccf03p (&auth_tx->CCF03P_details, ErrorMsg) ;
			}
			if (result_code != PTEMSG_OK)
				return (PTEMSG_UPDATE_FAILED);

			PRINT("After CCF03P.....\n"); 
		}
	

        if (auth_tx->TLF01_details.update_mask[0] & ACF01_MASK)
        {
            PRINT("Before update ACF01.....\n");
        
         if (DB_NullCheck_Flag==1)
   		 {
   			CheckNULLTerminatedinACF01 (&db_recs.acf01,MODE_UPDATE);
   		 }
            result_code = db_update_acf01 (&auth_tx->ACF01_details, ErrorMsg) ;
            if (result_code != PTEMSG_OK)
	            return(PTEMSG_UPDATE_FAILED);
            

            /* Update the deferred account of the cardholder if the transaction is either
                a AUTH_DEFERRED_PURCHASE_AUTHORIZATION or a REVERSAL of a 
                AUTH_DEFERRED_PURCHASE_AUTHORIZATION 
            */
            if (auth_tx->TLF01_details.tx_key == AUTH_DEFERRED_PURCHASE_AUTHORIZATION_RESPONSE ||
                auth_tx->TLF01_details.tx_key == AUTH_DEFERRED_PURCHASE_AUTHORIZATION_OVERRIDE_RESPONSE ||
               (auth_tx->TLF01_details.tx_key == AUTH_REVERSAL_RESPONSE && !strcmp (strProcessingCode, "70")))
            {
                PRINT ("Updating the deferred account ... \n") ;
                result_code = db_update_acf01_for_deferred_account (auth_tx, &auth_tx->ACF01_details, ErrorMsg) ;
                if (result_code != PTEMSG_OK)
	                return(PTEMSG_UPDATE_FAILED);
            }
            PRINT("After update ACF01 .....\n") ;
            
        }

		if (auth_tx->TLF01_details.update_mask[0] & BON01_MASK)
        {
            PRINT("Before update BON01.....");
            if (DB_NullCheck_Flag==1)
      		 {
      			CheckNULLTerminatedinBON01 (&db_recs.bon01,MODE_UPDATE);
      		 }
		    result_code = db_update_bon01 (&auth_tx->BON01_details, ErrorMsg) ;
		    if (result_code != PTEMSG_OK)
			    return(PTEMSG_UPDATE_FAILED);
		    PRINT("After update ACF01\n");
        }
    }
    return (PTEMSG_OK);
} /* process_alldata_msg */

/*************************************************************************************/
/*************************************************************************************/
pPTE_MSG process_get_gui_list_msg( pPTE_MSG p_msg_in )
{
   LONG           data_len;
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   pPTE_MSG       p_msg_out;
   pBYTE          p_current;


   p_msg_out     = NULL_PTR;
   p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
   p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
   memcpy(AtpBuffer, p_data, sizeof(AUTH_TX) + ISO_MSG_LENGTH1);
   p_current     = AtpBuffer + ISO_MSG_LENGTH1 + sizeof(AUTH_TX);
   data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );
   app_data_type = ptemsg_get_pte_msg_data_app_data_type( p_msg_data );
   p_data = p_data + ISO_MSG_LENGTH1 + sizeof(AUTH_TX);

   switch( app_data_type )
   {
     case ACF01_DATA:
         PRINT( "request to retrieve acf01 list\n" );
         // copy the last ucf02.group_id retrieved into the structure 
         
		if ( ST2_DB_SELECT_LIST_BY_PARAM == ptemsg_get_msg_subtype2 (p_msg_in) )
		{
			 if( ( result_code = db_get_acf01_gui_list_by_type( ( pACF01 )p_data, &db_lists.Acf01_Gui_List, ErrorMsg ) ) == PTEMSG_OK )
			 {
				memcpy(p_current, (BYTE *)&db_lists.Acf01_Gui_List, sizeof( db_lists.Acf01_Gui_List ));
				p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
											  ptemsg_get_msg_subtype1 (p_msg_in),
											  ptemsg_get_msg_subtype2 (p_msg_in),
											  reply_que,
											  application_que_name,
											  (pBYTE)AtpBuffer,
											  ISO_MSG_LENGTH + sizeof(AUTH_TX) + sizeof(db_lists.Acf01_Gui_List),
											  ACF01_DATA);
			 }
		}
		else
		{
			 if( ( result_code = db_get_acf01_gui_list( ( pACF01 )p_data, &db_lists.Acf01_Gui_List, ErrorMsg ) ) == PTEMSG_OK )
			 {
				memcpy(p_current, (BYTE *)&db_lists.Acf01_Gui_List, sizeof( db_lists.Acf01_Gui_List ));
				p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
											  ptemsg_get_msg_subtype1 (p_msg_in),
											  ptemsg_get_msg_subtype2 (p_msg_in),
											  reply_que,
											  application_que_name,
											  (pBYTE)AtpBuffer,
											  ISO_MSG_LENGTH + sizeof(AUTH_TX) + sizeof(db_lists.Acf01_Gui_List),
											  ACF01_DATA);
			 }
			 
		}
	  break;

     case DEF01_DATA:
         PRINT( "request to retrieve def01 list\n" );
         // copy the last ucf02.group_id retrieved into the structure 
		 if ( ST2_DB_SELECT_LIST_BY_PARAM == ptemsg_get_msg_subtype2 (p_msg_in) )
		 {
			 if( ( result_code = db_get_def01_gui_list_by_merchant_and_org( ( pDEF01 )p_data, &db_lists.Def01_Gui_List, ErrorMsg ) ) == PTEMSG_OK )
			 {
				memcpy(p_current, (BYTE *)&db_lists.Def01_Gui_List, sizeof( db_lists.Def01_Gui_List ));
				p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
											  ptemsg_get_msg_subtype1 (p_msg_in),
											  ptemsg_get_msg_subtype2 (p_msg_in),
											  reply_que,
											  application_que_name,
											  (pBYTE)AtpBuffer,
											  ISO_MSG_LENGTH + sizeof(AUTH_TX) + sizeof(db_lists.Def01_Gui_List),
											  DEF01_DATA);
			 }
		 }
		 else
		 {
			 if( ( result_code = db_get_def01_gui_list( ( pDEF01 )p_data, &db_lists.Def01_Gui_List, ErrorMsg ) ) == PTEMSG_OK )
			 {
				memcpy(p_current, (BYTE *)&db_lists.Def01_Gui_List, sizeof( db_lists.Def01_Gui_List ));
				p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
											  ptemsg_get_msg_subtype1 (p_msg_in),
											  ptemsg_get_msg_subtype2 (p_msg_in),
											  reply_que,
											  application_que_name,
											  (pBYTE)AtpBuffer,
											  ISO_MSG_LENGTH + sizeof(AUTH_TX) + sizeof(db_lists.Def01_Gui_List),
											  DEF01_DATA);
			 }
		 }
		        

	  break;

     case JCB3DS2_DATA:


              if( PTEMSG_OK == ( result_code =
                                 db_get_jcb3ds2_gui_list( (pJCB3DS2)p_data,
                                               &db_lists.JCB3DS2_Gui_List, ErrorMsg )))
              {
                 memcpy(p_current,
                        (BYTE *)&db_lists.JCB3DS2_Gui_List,
                        sizeof( db_lists.JCB3DS2_Gui_List ));
                 p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                   ptemsg_get_msg_subtype1 (p_msg_in),
                                   ptemsg_get_msg_subtype2 (p_msg_in),
                                   reply_que,
                                   application_que_name,
                                   (pBYTE)AtpBuffer,
                                   sizeof(AUTH_TX) + sizeof(db_lists.JCB3DS2_Gui_List),
								   JCB3DS2_DATA);
              }
              break;

     case CCL01_DATA:


              if( PTEMSG_OK == ( result_code =
                                 db_get_ccl01_gui_list( (pCCL01)p_data,
                                               &db_lists.CCL01_Gui_List, ErrorMsg )))
              {
                 memcpy(p_current,
                        (BYTE *)&db_lists.CCL01_Gui_List,
                        sizeof( db_lists.CCL01_Gui_List ));
                 p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                   ptemsg_get_msg_subtype1 (p_msg_in),
                                   ptemsg_get_msg_subtype2 (p_msg_in),
                                   reply_que,
                                   application_que_name,
                                   (pBYTE)AtpBuffer,
                                   sizeof(AUTH_TX) + sizeof(db_lists.CCL01_Gui_List),
								   CCL01_DATA);
              }
              break;

     case BDKKEY01_DATA:


              if( PTEMSG_OK == ( result_code =
                                 db_get_bdkkey01_gui_list( (pBDKKEY01)p_data,
                                               &db_lists.BDKKEY01_Gui_List, ErrorMsg )))
              {
                 memcpy(p_current,
                        (BYTE *)&db_lists.BDKKEY01_Gui_List,
                        sizeof( db_lists.BDKKEY01_Gui_List ));
                 p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                   ptemsg_get_msg_subtype1 (p_msg_in),
                                   ptemsg_get_msg_subtype2 (p_msg_in),
                                   reply_que,
                                   application_que_name,
                                   (pBYTE)AtpBuffer,
                                   sizeof(AUTH_TX) + sizeof(db_lists.BDKKEY01_Gui_List),
								   BDKKEY01_DATA);
              }
              break;

     case CAF01_KEYBLOCK_DATA:
         if(MsgSubtype2 == ST2_DB_GET_CAF01_LIST_BY_TYPE)
		 {
			 PRINT( "request to retrieve caf01_keyblock list by type\n" );
			 // copy the last caf01.name retrieved into the structure
			 if( ( result_code = db_get_caf01_keyblock01_gui_list_by_type( ( pCHAR )p_data, &db_lists.CAF01_KEYBLOCK01_Gui_List, ErrorMsg ) ) == PTEMSG_OK )
			 {
				memcpy(p_current, (BYTE *)&db_lists.CAF01_KEYBLOCK01_Gui_List, sizeof( db_lists.CAF01_KEYBLOCK01_Gui_List ));
				p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
											  ptemsg_get_msg_subtype1 (p_msg_in),
											  ptemsg_get_msg_subtype2 (p_msg_in),
											  reply_que,
											  application_que_name,
											  (pBYTE)AtpBuffer,
											  ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_lists.CAF01_KEYBLOCK01_Gui_List),
											  CAF01_KEYBLOCK_DATA);
			 }
		 }
		 else
		 {

              if( PTEMSG_OK == ( result_code =
                                 db_get_caf01_keyblock01_gui_list( (pCHAR)p_data,
                                               &db_lists.CAF01_KEYBLOCK01_Gui_List, ErrorMsg )))
              {
                 memcpy(p_current,
                        (BYTE *)&db_lists.CAF01_KEYBLOCK01_Gui_List,
                        sizeof( db_lists.CAF01_KEYBLOCK01_Gui_List ));
                 p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                   ptemsg_get_msg_subtype1 (p_msg_in),
                                   ptemsg_get_msg_subtype2 (p_msg_in),
                                   reply_que,
                                   application_que_name,
                                   (pBYTE)AtpBuffer,
                                   sizeof(AUTH_TX) + sizeof(db_lists.CAF01_KEYBLOCK01_Gui_List),
								   CAF01_KEYBLOCK_DATA);
              }
		 }
              break;

     case CCF04_DATA:
         PRINT( "request to retrieve ccf04 list\n" );
         // copy the last ccf04.group_id retrieved into the structure 
		if ( ST2_DB_SELECT_LIST_BY_PARAM == ptemsg_get_msg_subtype2 (p_msg_in) )
		{
			 if( ( result_code = db_get_ccf04_gui_list_by_type( ( pCCF04 )p_data, &db_lists.Ccf04_Gui_List, ErrorMsg ) ) == PTEMSG_OK )
			 {
				memcpy(p_current, (BYTE *)&db_lists.Ccf04_Gui_List, sizeof( db_lists.Ccf04_Gui_List ));
				p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
											  ptemsg_get_msg_subtype1 (p_msg_in),
											  ptemsg_get_msg_subtype2 (p_msg_in),
											  reply_que,
											  application_que_name,
											  (pBYTE)AtpBuffer,
											  ISO_MSG_LENGTH + sizeof(AUTH_TX) + sizeof(db_lists.Ccf04_Gui_List),
											  CCF04_DATA);
			 }
		}
		else
		{
			 if( ( result_code = db_get_ccf04_gui_list( ( pCCF04 )p_data, &db_lists.Ccf04_Gui_List, ErrorMsg ) ) == PTEMSG_OK )
			 {
				memcpy(p_current, (BYTE *)&db_lists.Ccf04_Gui_List, sizeof( db_lists.Ccf04_Gui_List ));
				p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
											  ptemsg_get_msg_subtype1 (p_msg_in),
											  ptemsg_get_msg_subtype2 (p_msg_in),
											  reply_que,
											  application_que_name,
											  (pBYTE)AtpBuffer,
											  ISO_MSG_LENGTH + sizeof(AUTH_TX) + sizeof(db_lists.Ccf04_Gui_List),
											  CCF04_DATA);
			 }
         
		}
	  break;

      case CCF10_DATA:

         PRINT( "request to retrieve ccf10 list\n" );
         // copy the last caf01.name retrieved into the structure 
         if( ( result_code = db_get_ccf10_gui_list( ( pCCF10 )p_data, &db_lists.Ccf10_Gui_List, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_lists.Ccf10_Gui_List, sizeof( db_lists.Ccf10_Gui_List ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_lists.Ccf10_Gui_List),
                                          CCF10_DATA);
         }
         break;


      case CR01_DATA:

    	  PRINT( "request to retrieve cr01 list\n" );
          if( PTEMSG_OK == ( result_code =db_get_CR01_gui_list( (pCR01)p_data,
                                                     &db_lists.CR01_Gui_List, ErrorMsg )))
         {
            memcpy(p_current,(BYTE *)&db_lists.CR01_Gui_List,sizeof( db_lists.CR01_Gui_List ));
             p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                         ptemsg_get_msg_subtype1 (p_msg_in),
                                         ptemsg_get_msg_subtype2 (p_msg_in),
                                         reply_que,
                                         application_que_name,
                                         (pBYTE)AtpBuffer,
                                         sizeof(AUTH_TX) + sizeof(db_lists.CR01_Gui_List),
     									CR01_DATA);
         }
         break;

      case MCT01_DATA:
         if(ptemsg_get_msg_subtype2 (p_msg_in) == ST2_DB_GET_MCT01_LIST_BY_TYPE)
         {
         
            PRINT( "request to retrieve mct01 list by type\n" );
            // copy the last caf01.name retrieved into the structure 
            if( ( result_code = db_get_mct01_gui_list_by_type( ( pMCT01 )p_data, &db_lists.Mct01_Gui_List, ErrorMsg ) ) == PTEMSG_OK )
            {
               memcpy(p_current, (BYTE *)&db_lists.Mct01_Gui_List, sizeof( db_lists.Mct01_Gui_List ));
               p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_lists.Mct01_Gui_List),
                                          MCT01_DATA);
            }
         }
         else
         {  
            PRINT( "request to retrieve mct01 list\n" );
            // copy the last caf01.name retrieved into the structure 
            if( ( result_code = db_get_mct01_gui_list( ( pMCT01 )p_data, &db_lists.Mct01_Gui_List, ErrorMsg ) ) == PTEMSG_OK )
            {
               memcpy(p_current, (BYTE *)&db_lists.Mct01_Gui_List, sizeof( db_lists.Mct01_Gui_List ));
               p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_lists.Mct01_Gui_List),
                                          MCT01_DATA);
            }
         }
          break;

      case PAF01_DATA:

         PRINT( "request to retrieve paf01 list\n" );
         // copy the last caf01.name retrieved into the structure 
         if( ( result_code = db_get_paf01_gui_list( ( pCHAR )p_data, &db_lists.Paf01_Gui_List, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_lists.Paf01_Gui_List, sizeof( db_lists.Paf01_Gui_List ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_lists.Paf01_Gui_List),
                                          PAF01_DATA);
         }
         break;

      case CSF01_DATA:

         PRINT( "request to retrieve csf01 list\n" );
         // copy the last csf01.name retrieved into the structure 
         if( ( result_code = db_get_csf01_gui_list( ( pCHAR )p_data, &db_lists.Csf01_Gui_List, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_lists.Csf01_Gui_List, sizeof( db_lists.Csf01_Gui_List ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_lists.Csf01_Gui_List),
                                          CSF01_DATA);
         }
         break;
		 case OMC01_DATA:
		 
			PRINT( "request to retrieve omc01 list\n" );
			// copy the last caf01.name retrieved into the structure 
		   {
				if( ( result_code = db_get_omc01_gui_list( ( pCHAR )p_data, &db_lists.Omc01_Gui_List, ErrorMsg ) ) == PTEMSG_OK )
				{
				   memcpy(p_current, (BYTE *)&db_lists.Omc01_Gui_List, sizeof( db_lists.Omc01_Gui_List ));
				   p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
												 ptemsg_get_msg_subtype1 (p_msg_in),
												 ptemsg_get_msg_subtype2 (p_msg_in),
												 reply_que,
												 application_que_name,
												 (pBYTE)AtpBuffer,
												 ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_lists.Omc01_Gui_List),
												 OMC01_DATA);
				}
		   }
			break;
			case MCC01_DATA:
			
			   PRINT( "request to retrieve mcc01 list\n" );
			   // copy the last caf01.name retrieved into the structure 
			  {
				   if( ( result_code = db_get_mcc01_gui_list( ( pCHAR )p_data, &db_lists.Mcc01_Gui_List, ErrorMsg ) ) == PTEMSG_OK )
				   {
					  memcpy(p_current, (BYTE *)&db_lists.Mcc01_Gui_List, sizeof( db_lists.Mcc01_Gui_List ));
					  p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
													ptemsg_get_msg_subtype1 (p_msg_in),
													ptemsg_get_msg_subtype2 (p_msg_in),
													reply_que,
													application_que_name,
													(pBYTE)AtpBuffer,
													ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_lists.Mcc01_Gui_List),
													MCC01_DATA);
				   }
			  }
			   break;

      case CCF02P_DATA:

         PRINT( "request to retrieve ccf02p list\n" );
         // copy the last caf01.name retrieved into the structure 
		if ( ST2_DB_SELECT_LIST_BY_PARAM == ptemsg_get_msg_subtype2 (p_msg_in) )
		{
			 if( ( result_code = db_get_ccf02p_gui_list_by_type( ( pCHAR )p_data, &db_lists.Ccf02p_Gui_List, ErrorMsg ) ) == PTEMSG_OK )
			 {
				memcpy(p_current, (BYTE *)&db_lists.Ccf02p_Gui_List, sizeof( db_lists.Ccf02p_Gui_List ));
				p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
											  ptemsg_get_msg_subtype1 (p_msg_in),
											  ptemsg_get_msg_subtype2 (p_msg_in),
											  reply_que,
											  application_que_name,
											  (pBYTE)AtpBuffer,
											  ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_lists.Ccf02p_Gui_List),
											  CCF02P_DATA);
			 }
		}
		else
		{
			 if( ( result_code = db_get_ccf02p_gui_list( ( pCHAR )p_data, &db_lists.Ccf02p_Gui_List, ErrorMsg ) ) == PTEMSG_OK )
			 {
				memcpy(p_current, (BYTE *)&db_lists.Ccf02p_Gui_List, sizeof( db_lists.Ccf02p_Gui_List ));
				p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
											  ptemsg_get_msg_subtype1 (p_msg_in),
											  ptemsg_get_msg_subtype2 (p_msg_in),
											  reply_que,
											  application_que_name,
											  (pBYTE)AtpBuffer,
											  ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_lists.Ccf02p_Gui_List),
											  CCF02P_DATA);
			 }
		}
         break;

	case MPPARA01_DATA:

         PRINT( "request to retrieve MPPARA01 list\n" );
         if( ( result_code = db_get_mppara01_gui_list( ( pCHAR )p_data,1, &db_lists.Mppara01_Gui_List, ErrorMsg ) ) == PTEMSG_OK )
		 {
				memcpy(p_current, (BYTE *)&db_lists.Mppara01_Gui_List, sizeof( db_lists.Mppara01_Gui_List ));
				p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
											  ptemsg_get_msg_subtype1 (p_msg_in),
											  ptemsg_get_msg_subtype2 (p_msg_in),
											  reply_que,
											  application_que_name,
											  (pBYTE)AtpBuffer,
											  ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_lists.Mppara01_Gui_List),
											  MPPARA01_DATA);
		}
		break;

      case BPF01_DATA:

         PRINT( "request to retrieve bpf01 list\n" );
         // copy the last caf01.name retrieved into the structure 
         if( ( result_code = db_get_bpf01_gui_list( ( pCHAR )p_data, &db_lists.Bpf01_Gui_List, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_lists.Bpf01_Gui_List, sizeof( db_lists.Bpf01_Gui_List ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_lists.Bpf01_Gui_List),
                                          BPF01_DATA);
         }
         break;

      case APF01_DATA:

         PRINT( "request to retrieve apf01 list\n" );
         // copy the last caf01.name retrieved into the structure 
         if( ( result_code = db_get_apf01_gui_list( ( pCHAR )p_data, &db_lists.Apf01_Gui_List, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_lists.Apf01_Gui_List, sizeof( db_lists.Apf01_Gui_List ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_lists.Apf01_Gui_List),
                                          APF01_DATA);
         }
         break;

      case JBF01_DATA:

         PRINT( "request to retrieve jbf01 list\n" );
         // copy the last caf01.name retrieved into the structure 
         if( ( result_code = db_get_jbf01_gui_list( ( pCHAR )p_data, &db_lists.Jbf01_Gui_List, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_lists.Jbf01_Gui_List, sizeof( db_lists.Jbf01_Gui_List ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_lists.Jbf01_Gui_List),
                                          JBF01_DATA);
         }
         break;

      case CAF01_DATA:
         if(MsgSubtype2 == ST2_DB_GET_CAF01_LIST_BY_TYPE)
		 {
			 PRINT( "request to retrieve caf01 list by type\n" );
			 // copy the last caf01.name retrieved into the structure 
			 if( ( result_code = db_get_caf01_gui_list_by_type( ( pCHAR )p_data, &db_lists.Caf01_Gui_List, ErrorMsg ) ) == PTEMSG_OK )
			 {
				memcpy(p_current, (BYTE *)&db_lists.Caf01_Gui_List, sizeof( db_lists.Caf01_Gui_List ));
				p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
											  ptemsg_get_msg_subtype1 (p_msg_in),
											  ptemsg_get_msg_subtype2 (p_msg_in),
											  reply_que,
											  application_que_name,
											  (pBYTE)AtpBuffer,
											  ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_lists.Caf01_Gui_List),
											  CAF01_DATA);
			 }
		 }
		 else
		 {
			 PRINT( "request to retrieve caf01 list\n" );
			 // copy the last caf01.name retrieved into the structure 
			 if( ( result_code = db_get_caf01_gui_list( ( pCHAR )p_data, &db_lists.Caf01_Gui_List, ErrorMsg ) ) == PTEMSG_OK )
			 {
				memcpy(p_current, (BYTE *)&db_lists.Caf01_Gui_List, sizeof( db_lists.Caf01_Gui_List ));
				p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
											  ptemsg_get_msg_subtype1 (p_msg_in),
											  ptemsg_get_msg_subtype2 (p_msg_in),
											  reply_que,
											  application_que_name,
											  (pBYTE)AtpBuffer,
											  ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_lists.Caf01_Gui_List),
											  CAF01_DATA);
			 }
		 }
         break;

      case MCC_LIMITS_DATA:
         if( ( result_code = db_get_mcc_limits_gui_list( (pCHAR)p_data, &db_lists.MCC_Limits_Gui_List, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_lists.MCC_Limits_Gui_List, sizeof(db_lists.MCC_Limits_Gui_List));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                   ptemsg_get_msg_subtype1 (p_msg_in),
                                   ptemsg_get_msg_subtype2 (p_msg_in),
                                   reply_que,
                                   application_que_name,
                                   (pBYTE)AtpBuffer,
                                   ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_lists.MCC_Limits_Gui_List),
                                   MCC_LIMITS_DATA );
         }
         break;

      case COUNTRY_AMT_LIMITS_DATA:
         if( ( result_code = db_get_cntry_amt_limits_gui_list( (pCHAR)p_data, &db_lists.Country_Amt_Gui_List, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_lists.Country_Amt_Gui_List, sizeof(db_lists.Country_Amt_Gui_List));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                   ptemsg_get_msg_subtype1 (p_msg_in),
                                   ptemsg_get_msg_subtype2 (p_msg_in),
                                   reply_que,
                                   application_que_name,
                                   (pBYTE)AtpBuffer,
                                   ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_lists.Country_Amt_Gui_List),
                                   COUNTRY_AMT_LIMITS_DATA );
         }
         break;


      case CRF01_DATA:
		if ( ST2_DB_SELECT_LIST_BY_PARAM == ptemsg_get_msg_subtype2 (p_msg_in) )
      {
         PRINT( "request to retrieve crf01 gui list\n" );
         // copy the last ptf01.name retrieved into the structure 
         if( ( result_code = db_get_crf01_list( &db_lists.Crf01_List, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_lists.Crf01_List, sizeof( db_lists.Crf01_List ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_lists.Crf01_List),
                                          CRF01_DATA);
         }
      }
      else
      {
         PRINT( "request to retrieve crf01 list\n" );
         // copy the last ptf01.name retrieved into the structure 
         if( ( result_code = db_get_crf01_gui_list( ( pCHAR )p_data, &db_lists.Crf01_Gui_List, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_lists.Crf01_Gui_List, sizeof( db_lists.Crf01_Gui_List ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_lists.Crf01_Gui_List),
                                          CRF01_DATA);
         }
       }

         break;

      case CTF01_DATA:

         PRINT( "request to retrieve ctf01 list\n" );
         // copy the last ctf01.name retrieved into the structure 
         if( ( result_code = db_get_ctf01_gui_list( ( pCHAR )p_data, &db_lists.Ctf01_Gui_List, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_lists.Ctf01_Gui_List, sizeof( db_lists.Ctf01_Gui_List ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_lists.Ctf01_Gui_List),
                                          CTF01_DATA);
         }
         break;

      case ATP02_DATA:

         PRINT( "request to retrieve atp02 list\n" );
         // copy the last ucf02.workstation_id retrieved into the structure 
         if( ( result_code = db_get_atp02_gui_list( ( pATP02)p_data, &db_lists.Atp02_Gui_List, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_lists.Atp02_Gui_List, sizeof( db_lists.Atp02_Gui_List ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_lists.Atp02_Gui_List),
                                          ATP02_DATA);
         }
         break;

      case UCF01_DATA:
       
         // copy the last ucf01.name retrieved into the structure 
         if(ptemsg_get_msg_subtype2 (p_msg_in) == ST2_DB_GET_UCF01_LIST_BY_TERMINAL)
		   {
			   PRINT( "request to retrieve ucf01 list by terminal\n" );
			   if( ( result_code = db_get_ucf01_list_by_terminal( ( pUCF01 )p_data, &db_lists.Ucf01_List, ErrorMsg ) ) == PTEMSG_OK )
			   {
				   memcpy(p_current, (BYTE *)&db_lists.Ucf01_List, sizeof( db_lists.Ucf01_List ));
				   p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
											  ptemsg_get_msg_subtype1 (p_msg_in),
											  ptemsg_get_msg_subtype2 (p_msg_in),
											  reply_que,
											  application_que_name,
											  (pBYTE)AtpBuffer,
											  ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_lists.Ucf01_List),
											  UCF01_DATA);
			   }
         }
         else if(ptemsg_get_msg_subtype2 (p_msg_in) == ST2_DB_GET_UCF01_USERLIST)
         {
             PRINT( "request to retrieve ucf01 list by for user list\n" );
             if( ( result_code = db_get_ucf01_Userlist( ( pUCF01 )p_data, &db_lists.Ucf01_List, ErrorMsg ) ) == PTEMSG_OK )
             {
                 memcpy(p_current, (BYTE *)&db_lists.Ucf01_List, sizeof( db_lists.Ucf01_List ));
                 p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                          						ptemsg_get_msg_subtype1 (p_msg_in),
                          						ptemsg_get_msg_subtype2 (p_msg_in),
                          						reply_que,
                          						application_que_name,
                          						(pBYTE)AtpBuffer,
                          						ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_lists.Ucf01_List),
                          						UCF01_DATA);
              }

        }
        else
        {
            PRINT( "request to retrieve ucf01 list\n" );

            if( ( result_code = db_get_ucf01_list( ( pCHAR )p_data, &db_lists.Ucf01_List, ErrorMsg ) ) == PTEMSG_OK )
            {
               memcpy(p_current, (BYTE *)&db_lists.Ucf01_List, sizeof( db_lists.Ucf01_List ));
               p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_lists.Ucf01_List),
                                          UCF01_DATA);
            }

        }
            break;


			/*Added UCF03 Table for PCI Changes by TF-Srikanth 
					added additional fields for UCF01 */
  /*   case UCF03_DATA:
       
         // copy the last ucf03.name retrieved into the structure 
         if(ptemsg_get_msg_subtype2 (p_msg_in) == ST2_DB_GET_UCF03_LIST_BY_TERMINAL)
		   {
			   PRINT( "request to retrieve ucf03 list by terminal\n" );
			   if( ( result_code = db_get_ucf03_list_by_terminal( ( pUCF03 )p_data, &db_lists.Ucf03_List, ErrorMsg ) ) == PTEMSG_OK )
			   {
				   memcpy(p_current, (BYTE *)&db_lists.Ucf03_List, sizeof( db_lists.Ucf03_List ));
				   p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
											  ptemsg_get_msg_subtype1 (p_msg_in),
											  ptemsg_get_msg_subtype2 (p_msg_in),
											  reply_que,
											  application_que_name,
											  (pBYTE)AtpBuffer,
											  ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_lists.Ucf03_List),
											  UCF03_DATA);
			   }
         }
        else
        {
            PRINT( "request to retrieve Ucf03 list\n" );
            if( ( result_code = db_get_ucf03_list( ( pCHAR )p_data, &db_lists.Ucf03_List, ErrorMsg ) ) == PTEMSG_OK )
            {
               memcpy(p_current, (BYTE *)&db_lists.Ucf03_List, sizeof( db_lists.Ucf03_List ));
               p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_lists.Ucf03_List),
                                          UCF03_DATA);
            }
        }
            break;
*/
	 case UCF02_DATA:     

         if((ptemsg_get_msg_subtype2 (p_msg_in)) == ST2_DB_GET_UCF02_LIST_BY_TERMINAL)
		   {
			   PRINT( "request to retrieve ucf02 list by terminal\n" );
			   if( ( result_code = db_get_ucf02_list_by_terminal( ( pUCF02 )p_data, &db_lists.Ucf02_List, ErrorMsg ) ) == PTEMSG_OK )
			   {
				   memcpy(p_current, (BYTE *)&db_lists.Ucf02_List, sizeof( db_lists.Ucf02_List ));
				   p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
											  ptemsg_get_msg_subtype1 (p_msg_in),
											  ptemsg_get_msg_subtype2 (p_msg_in),
											  reply_que,
											  application_que_name,
											  (pBYTE)AtpBuffer,
											  ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_lists.Ucf02_List),
											  UCF02_DATA);
			   }
         }
         else
         {
            PRINT( "request to retrieve ucf02 list\n" );
            // copy the last ucf02.group_id retrieved into the structure 
            if( ( result_code = db_get_ucf02_list( ( pCHAR )p_data, &db_lists.Ucf02_List, ErrorMsg ) ) == PTEMSG_OK )
            {
               memcpy(p_current, (BYTE *)&db_lists.Ucf02_List, sizeof( db_lists.Ucf02_List ));
               p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_lists.Ucf02_List),
                                          UCF02_DATA);
            }
         }
         break;

      case BIN01_DATA:

         PRINT( "request to retrieve bin01 list\n" );
         // copy the last ucf02.group_id retrieved into the structure 
         if( ST2_DB_SELECT_LIST_BY_PARAM == ptemsg_get_msg_subtype2 (p_msg_in) )
		 {
			 if( ( result_code = db_get_bin01_gui_list_by_type( ( pBIN01)p_data, &db_lists.Bin01_Gui_List, ErrorMsg ) ) == PTEMSG_OK )
			 {
				memcpy(p_current, (BYTE *)&db_lists.Bin01_Gui_List, sizeof( db_lists.Bin01_Gui_List ));
				p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
											  ptemsg_get_msg_subtype1 (p_msg_in),
											  ptemsg_get_msg_subtype2 (p_msg_in),
											  reply_que,
											  application_que_name,
											  (pBYTE)AtpBuffer,
											  ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_lists.Bin01_Gui_List),
											  BIN01_DATA);
			 }
		 }
		 else
		 {
			 if( ( result_code = db_get_bin01_gui_list( ( pBIN01)p_data, &db_lists.Bin01_Gui_List, ErrorMsg ) ) == PTEMSG_OK )
			 {
				memcpy(p_current, (BYTE *)&db_lists.Bin01_Gui_List, sizeof( db_lists.Bin01_Gui_List ));
				p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
											  ptemsg_get_msg_subtype1 (p_msg_in),
											  ptemsg_get_msg_subtype2 (p_msg_in),
											  reply_que,
											  application_que_name,
											  (pBYTE)AtpBuffer,
											  ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_lists.Bin01_Gui_List),
											  BIN01_DATA);
			 }
		 }
         break;

      case HST01_DATA:

         switch (ptemsg_get_msg_subtype2 (p_msg_in))
         {
            case ST2_GET_HST01_BY_FILE_NAME :
               PRINT( "request to retrieve hst01 list by file name\n" );
               // copy the last ucf02.group_id retrieved into the structure 
               if( ( result_code = db_get_hst01_list_by_file_name( ( pHST01 )p_data, &db_lists.Hst01_List, ErrorMsg ) ) == PTEMSG_OK )
               {
                  memcpy(p_current, (BYTE *)&db_lists.Hst01_List, sizeof( db_lists.Hst01_List ));
                  p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                                ptemsg_get_msg_subtype1 (p_msg_in),
                                                ptemsg_get_msg_subtype2 (p_msg_in),
                                                reply_que,
                                                application_que_name,
                                                (pBYTE)AtpBuffer,
                                                ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_lists.Hst01_List),
                                                HST01_DATA);
               }
               break;


            case ST2_GET_HST01_BY_FILE_NAME_KEY :
               PRINT( "request to retrieve hst01 list by file name and key\n" );
               // copy the last ucf02.group_id retrieved into the structure 
               if( ( result_code = db_get_hst01_list_by_file_name_key( ( pHST01 )p_data, &db_lists.Hst01_List, ErrorMsg ) ) == PTEMSG_OK )
               {
                  memcpy(p_current, (BYTE *)&db_lists.Hst01_List, sizeof( db_lists.Hst01_List ));
                  p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                                ptemsg_get_msg_subtype1 (p_msg_in),
                                                ptemsg_get_msg_subtype2 (p_msg_in),
                                                reply_que,
                                                application_que_name,
                                                (pBYTE)AtpBuffer,
                                                ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_lists.Hst01_List),
                                                HST01_DATA);
               }
               break;


            case ST2_GET_HST01_BY_USER_NAME :
               PRINT( "request to retrieve hst01 list by user name\n" );
               // copy the last ucf02.group_id retrieved into the structure 
               if( ( result_code = db_get_hst01_list_by_user_name( ( pHST01 )p_data, &db_lists.Hst01_List, ErrorMsg ) ) == PTEMSG_OK )
               {
                  memcpy(p_current, (BYTE *)&db_lists.Hst01_List, sizeof( db_lists.Hst01_List ));
                  p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                                ptemsg_get_msg_subtype1 (p_msg_in),
                                                ptemsg_get_msg_subtype2 (p_msg_in),
                                                reply_que,
                                                application_que_name,
                                                (pBYTE)AtpBuffer,
                                                ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_lists.Hst01_List),
                                                HST01_DATA);
               }
               break;

            case ST2_GET_HST01_BY_USER_NAME+1 :
               PRINT( "request to retrieve hst01 list by user name\n" );
               // copy the last ucf02.group_id retrieved into the structure 
               if( ( result_code = db_get_hst01_list_all( ( pHST01 )p_data, &db_lists.Hst01_List, ErrorMsg ) ) == PTEMSG_OK )
               {
                  memcpy(p_current, (BYTE *)&db_lists.Hst01_List, sizeof( db_lists.Hst01_List ));
                  p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                                ptemsg_get_msg_subtype1 (p_msg_in),
                                                ptemsg_get_msg_subtype2 (p_msg_in),
                                                reply_que,
                                                application_que_name,
                                                (pBYTE)AtpBuffer,
                                                ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_lists.Hst01_List),
                                                HST01_DATA);
               }
               break;

            case ST2_GET_HST01_BY_USER_NAME+2 :
               PRINT( "request to retrieve hst01 list by user name\n" );
               // copy the last ucf02.group_id retrieved into the structure 
               if( ( result_code = db_get_hst01_list_by_user( ( pHST01 )p_data, &db_lists.Hst01_List, ErrorMsg ) ) == PTEMSG_OK )
               {
                  memcpy(p_current, (BYTE *)&db_lists.Hst01_List, sizeof( db_lists.Hst01_List ));
                  p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                                ptemsg_get_msg_subtype1 (p_msg_in),
                                                ptemsg_get_msg_subtype2 (p_msg_in),
                                                reply_que,
                                                application_que_name,
                                                (pBYTE)AtpBuffer,
                                                ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_lists.Hst01_List),
                                                HST01_DATA);
               }
               break;

            case ST2_GET_HST01_BY_USER_NAME+3 :
               PRINT( "request to retrieve hst01 list by user name\n" );
               // copy the last ucf02.group_id retrieved into the structure
               if( ( result_code = db_get_hst01_list_all_same_date( ( pHST01 )p_data, &db_lists.Hst01_List, ErrorMsg ) ) == PTEMSG_OK )
               {
                  memcpy(p_current, (BYTE *)&db_lists.Hst01_List, sizeof( db_lists.Hst01_List ));
                  p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                                ptemsg_get_msg_subtype1 (p_msg_in),
                                                ptemsg_get_msg_subtype2 (p_msg_in),
                                                reply_que,
                                                application_que_name,
                                                (pBYTE)AtpBuffer,
                                                ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_lists.Hst01_List),
                                                HST01_DATA);
               }
               break;

            case ST2_GET_HST01_BY_USER_NAME+4 :
               PRINT( "request to retrieve hst01 list by user name\n" );
               // copy the last ucf02.group_id retrieved into the structure
               if( ( result_code = db_get_hst01_list_by_user_same_date( ( pHST01 )p_data, &db_lists.Hst01_List, ErrorMsg ) ) == PTEMSG_OK )
               {
                  memcpy(p_current, (BYTE *)&db_lists.Hst01_List, sizeof( db_lists.Hst01_List ));
                  p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                                ptemsg_get_msg_subtype1 (p_msg_in),
                                                ptemsg_get_msg_subtype2 (p_msg_in),
                                                reply_que,
                                                application_que_name,
                                                (pBYTE)AtpBuffer,
                                                ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_lists.Hst01_List),
                                                HST01_DATA);
               }
               break;

            default:
               // unknown subtype_2 received. Log Event and return error message to client
               result_code = PTEMSG_INVALID_SUBTYPE2;
               sprintf (ErrorMsg, "Unknown subtype_2 for get hst01 list request from %s", reply_que);
               break;
         }
         break;

      case BON02_DATA:

         PRINT( "Request to retrieve Bon02 list\n" );

         if( ( result_code = db_get_bon02_gui_list( ( pCHAR )p_data, &db_lists.Bon02_Gui_List, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_lists.Bon02_Gui_List, sizeof( db_lists.Bon02_Gui_List ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_lists.Bon02_Gui_List),
                                          BON02_DATA);
         }
         break;


      default:

         // unknown app_data_type received. Log Event and return error message to client
         result_code = PTEMSG_INVALID_DATATYPE;
         sprintf (ErrorMsg, "Unknown datatype for GetList request from %s", reply_que);
         break;
   }


   if ((result_code == PTEMSG_OK) && (p_msg_out == NULL_PTR) )
   {
      result_code = PTEMSG_INSUFFICIENT_MEMORY;
      sprintf (ErrorMsg, "Insufficient Memory to build GetAll response to %s", reply_que);
   }

   return (p_msg_out);
}

/*************************************************************************************/
/*************************************************************************************/
pPTE_MSG process_get_service_list_msg( pPTE_MSG p_msg_in )
{
   LONG           data_len;
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   pPTE_MSG       p_msg_out;
   pBYTE          p_current;


   p_msg_out     = NULL_PTR;
   p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
   p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
   memcpy(AtpBuffer, p_data, sizeof(AUTH_TX) + ISO_MSG_LENGTH1);
   p_current     = AtpBuffer + ISO_MSG_LENGTH1 + sizeof(AUTH_TX);
   data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );
   app_data_type = ptemsg_get_pte_msg_data_app_data_type( p_msg_data );
   p_data = p_data + ISO_MSG_LENGTH1 + sizeof(AUTH_TX);

   switch( app_data_type )
   {

      case CTF01_DATA:

         PRINT( "request to retrieve ctf01 list\n" );
         // copy the last ctf01.name retrieved into the structure 
         if( ( result_code = db_get_ctf01_service_list( ( pCHAR )p_data, &db_lists.Ctf01_List, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_lists.Ctf01_List, sizeof( db_lists.Ctf01_List ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_lists.Ctf01_List),
                                          CTF01_DATA);
         }
         break;

      case JCB3DS2_DATA:

          PRINT( "request to retrieve JCB3DS2 service list\n" );
          if( ( result_code = db_get_jcb3ds2_service_list( ( pJCB3DS2 )p_data, &db_lists.JCB3DS2_Gui_List, ErrorMsg ) ) == PTEMSG_OK )
           {
               memcpy(p_current, (BYTE *)&db_lists.JCB3DS2_Gui_List, sizeof( db_lists.JCB3DS2_Gui_List ));
               p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                                ptemsg_get_msg_subtype1 (p_msg_in),
                                                ptemsg_get_msg_subtype2 (p_msg_in),
                                                reply_que,
                                                application_que_name,
                                                (pBYTE)AtpBuffer,
                                                sizeof(AUTH_TX) + sizeof(db_lists.JCB3DS2_Gui_List),
												JCB3DS2_DATA);
          }
          break;

      case CCL01_DATA:

          PRINT( "request to retrieve CCL01 service list\n" );
          if( ( result_code = db_get_ccl01_service_list( ( pCCL01 )p_data, &db_lists.CCL01_Gui_List, ErrorMsg ) ) == PTEMSG_OK )
           {
               memcpy(p_current, (BYTE *)&db_lists.CCL01_Gui_List, sizeof( db_lists.CCL01_Gui_List ));
               p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                                ptemsg_get_msg_subtype1 (p_msg_in),
                                                ptemsg_get_msg_subtype2 (p_msg_in),
                                                reply_que,
                                                application_que_name,
                                                (pBYTE)AtpBuffer,
                                                sizeof(AUTH_TX) + sizeof(db_lists.CCL01_Gui_List),
												CCL01_DATA);
          }
          break;

      case BDKKEY01_DATA:

          PRINT( "request to retrieve BDKKEY01 service list\n" );
          if( ( result_code = db_get_bdkkey01_service_list( ( pBDKKEY01 )p_data, &db_lists.BDKKEY01_Gui_List, ErrorMsg ) ) == PTEMSG_OK )
           {
               memcpy(p_current, (BYTE *)&db_lists.BDKKEY01_Gui_List, sizeof( db_lists.BDKKEY01_Gui_List ));
               p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                                ptemsg_get_msg_subtype1 (p_msg_in),
                                                ptemsg_get_msg_subtype2 (p_msg_in),
                                                reply_que,
                                                application_que_name,
                                                (pBYTE)AtpBuffer,
                                                sizeof(AUTH_TX) + sizeof(db_lists.BDKKEY01_Gui_List),
												BDKKEY01_DATA);
          }
          break;

/*      case CAF01_KEYBLOCK_DATA:

          PRINT( "request to retrieve CAF01_KEYBLOCK service list\n" );
          if( ( result_code = db_get_caf01_keyblock01_service_list( ( pCAF01_KEYBLOCK )p_data, &db_lists.BDKKEY01_Gui_List, ErrorMsg ) ) == PTEMSG_OK )
           {
               memcpy(p_current, (BYTE *)&db_lists.CAF01_KEYBLOCK01_Gui_List, sizeof( db_lists.CAF01_KEYBLOCK01_Gui_List ));
               p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                                ptemsg_get_msg_subtype1 (p_msg_in),
                                                ptemsg_get_msg_subtype2 (p_msg_in),
                                                reply_que,
                                                application_que_name,
                                                (pBYTE)AtpBuffer,
                                                sizeof(AUTH_TX) + sizeof(db_lists.CAF01_KEYBLOCK01_Gui_List),
												CAF01_KEYBLOCK_DATA);
          }
          break;
*/

      case ATP02_DATA:

         PRINT( "request to retrieve atp02 list\n" );
         // copy the last ucf02.workstation_id retrieved into the structure 
         if( ( result_code = db_get_atp02_service_list( ( pATP02 )p_data, &db_lists.Atp02_List, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_lists.Atp02_List, sizeof( db_lists.Atp02_List ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_lists.Atp02_List),
                                          ATP02_DATA);
         }
         break;

      case BIN01_DATA:

         PRINT( "request to retrieve bin01 list\n" );
         // copy the last ucf02.group_id retrieved into the structure 
         if( ( result_code = db_get_bin01_service_list( ( pBIN01 )p_data, &db_lists.Bin01_List, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, (BYTE *)&db_lists.Bin01_List, sizeof( db_lists.Bin01_List ));
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + sizeof(db_lists.Bin01_List),
                                          BIN01_DATA);
         }
         break;

      default:

         // unknown app_data_type received. Log Event and return error message to client
         result_code = PTEMSG_INVALID_DATATYPE;
         sprintf (ErrorMsg, "Unknown datatype for GetList request from %s", reply_que);
         break;
   }


   if ((result_code == PTEMSG_OK) && (p_msg_out == NULL_PTR) )
   {
      result_code = PTEMSG_INSUFFICIENT_MEMORY;
      sprintf (ErrorMsg, "Insufficient Memory to build GetAll response to %s", reply_que);
   }

   return (p_msg_out);
}


/*************************************************************************************/
/*************************************************************************************/
pPTE_MSG process_get_count( pPTE_MSG p_msg_in )
{
   pPTE_MSG_DATA  p_msg_data;
   pPTE_MSG       p_msg_out;
   pBYTE          p_current;
   pBYTE          p_data;
   CHAR           table_count[5] = "";

   p_msg_out     = NULL_PTR;
   p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
   p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
   memcpy(AtpBuffer, p_data, sizeof(AUTH_TX) + ISO_MSG_LENGTH1);
   p_current     = AtpBuffer + ISO_MSG_LENGTH1 + sizeof(AUTH_TX);
   app_data_type = ptemsg_get_pte_msg_data_app_data_type( p_msg_data );

   switch( app_data_type )
   {

      case ATP02_DATA:

         PRINT( "request to retrieve atp02 count\n" );
         if( ( result_code = db_get_atp02_count( table_count, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, table_count, strlen(table_count) + 1);
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + strlen(table_count) + 1,
                                          ATP02_DATA);
         }
         break;

      case JCB3DS2_DATA:

               PRINT( "request to retrieve obs01 count\n" );
               if( ( result_code = db_get_jcb3ds2_count( table_count, ErrorMsg ) ) == PTEMSG_OK )
               {
                  memcpy(p_current, table_count, strlen(table_count) + 1);
                  p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                                ptemsg_get_msg_subtype1 (p_msg_in),
                                                ptemsg_get_msg_subtype2 (p_msg_in),
                                                reply_que,
                                                application_que_name,
                                                (pBYTE)AtpBuffer,
                                                sizeof(AUTH_TX) + strlen(table_count) + 1,
												JCB3DS2_DATA);
               }
               break;

      case CCL01_DATA:

               PRINT( "request to retrieve ccl01 count\n" );
               if( ( result_code = db_get_ccl01_count( table_count, ErrorMsg ) ) == PTEMSG_OK )
               {
                  memcpy(p_current, table_count, strlen(table_count) + 1);
                  p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                                ptemsg_get_msg_subtype1 (p_msg_in),
                                                ptemsg_get_msg_subtype2 (p_msg_in),
                                                reply_que,
                                                application_que_name,
                                                (pBYTE)AtpBuffer,
                                                sizeof(AUTH_TX) + strlen(table_count) + 1,
												CCL01_DATA);
               }
               break;

      case BDKKEY01_DATA:

               PRINT( "request to retrieve bdkkey01 count\n" );
               if( ( result_code = db_get_bdkkey01_count( table_count, ErrorMsg ) ) == PTEMSG_OK )
               {
                  memcpy(p_current, table_count, strlen(table_count) + 1);
                  p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                                ptemsg_get_msg_subtype1 (p_msg_in),
                                                ptemsg_get_msg_subtype2 (p_msg_in),
                                                reply_que,
                                                application_que_name,
                                                (pBYTE)AtpBuffer,
                                                sizeof(AUTH_TX) + strlen(table_count) + 1,
												BDKKEY01_DATA);
               }
               break;

      case CAF01_KEYBLOCK_DATA:

               PRINT( "request to retrieve caf01_keyblock01 count\n" );
               if( ( result_code = db_get_caf01_keyblock01_count( table_count, ErrorMsg ) ) == PTEMSG_OK )
               {
                  memcpy(p_current, table_count, strlen(table_count) + 1);
                  p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                                ptemsg_get_msg_subtype1 (p_msg_in),
                                                ptemsg_get_msg_subtype2 (p_msg_in),
                                                reply_que,
                                                application_que_name,
                                                (pBYTE)AtpBuffer,
                                                sizeof(AUTH_TX) + strlen(table_count) + 1,
												CAF01_KEYBLOCK_DATA);
               }
               break;
 
       case BIN01_DATA:

         PRINT( "request to retrieve bin01 count\n" );
         if( ( result_code = db_get_bin01_count( table_count, ErrorMsg ) ) == PTEMSG_OK )
         {
            memcpy(p_current, table_count, strlen(table_count) + 1);
            p_msg_out = ptemsg_build_msg( MT_DB_REPLY,
                                          ptemsg_get_msg_subtype1 (p_msg_in),
                                          ptemsg_get_msg_subtype2 (p_msg_in),
                                          reply_que,
                                          application_que_name,
                                          (pBYTE)AtpBuffer,
                                          ISO_MSG_LENGTH1 + sizeof(AUTH_TX) + strlen(table_count) + 1,
                                          BIN01_DATA);
         }
         break;

     default:

         // unknown app_data_type received. Log Event and return error message to client
         result_code = PTEMSG_INVALID_DATATYPE;
         sprintf (ErrorMsg, "Unknown datatype for GetList request from %s", reply_que);
         break;
   }


   if ((result_code == PTEMSG_OK) && (p_msg_out == NULL_PTR) )
   {
      result_code = PTEMSG_INSUFFICIENT_MEMORY;
      sprintf (ErrorMsg, "Insufficient Memory to build GetAll response to %s", reply_que);
   }

   return (p_msg_out);
}



/*************************************************************************************/
/*************************************************************************************/
pPTE_MSG process_echo_msg ( pPTE_MSG p_msg_in )
{
   LONG           data_len;
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   pPTE_MSG       p_msg_out;


   PRINT ("processing Echo request\n");

   p_msg_data     = ptemsg_get_pte_msg_data              (p_msg_in);
   p_data         = ptemsg_get_pte_msg_data_data         (p_msg_data);
   data_len       = ptemsg_get_pte_msg_data_data_length  (p_msg_data);
   app_data_type  = ptemsg_get_pte_msg_data_app_data_type(p_msg_data);


   p_msg_out = ptemsg_build_msg (MT_DB_REPLY,
                                 ptemsg_get_msg_subtype1 (p_msg_in),
                                 ptemsg_get_msg_subtype2 (p_msg_in),
                                 reply_que,
                                 application_que_name,
                                 p_data,
                                 data_len,
                                 app_data_type);

   if (p_msg_out == NULL_PTR)
   {
      result_code = PTEMSG_INSUFFICIENT_MEMORY;
      sprintf (ErrorMsg, "Insufficient Memory to build Echo response to %s", reply_que);
   }

   return (p_msg_out);
}



/*************************************************************************************/
/*************************************************************************************/
pPTE_MSG process_db_request ( pPTE_MSG p_msg_in )
{
   pPTE_MSG       p_msg_out;
   CHAR           query_start_time[20]="";
   CHAR           query_end_time  [20]="";
   CHAR           errbuf[120];
   INT            ret_val;
   double         start_time;
   double         end_time;
   double         duration;

   memset( ErrorMsg, 0x00, sizeof(ErrorMsg) );
   MsgSubtype2 = ptemsg_get_msg_subtype2 (p_msg_in);
   MsgSubtype1 = ptemsg_get_msg_subtype1(p_msg_in);

   /* For DB statics added by raghu*/
      DB_MsgSubtype2 = '0';
      DB_MsgSubtype1 = '0';
      DB_MsgSubtype2 = MsgSubtype2 ;
      DB_MsgSubtype1 = MsgSubtype1 ;


   memset(AtpBuffer, 0, sizeof(AtpBuffer));
   if ( ds_timer_flag == DS_TIMER_ON )
   {
      /* The functionality to check the time of a query is turned on.
       * Get the time before the query. Will check it after the query.
       */
      ptetime_get_strtime( query_start_time );
   }

   if ( DB_Timing_Flag == true )
   {
      /* The functionality to gather DB Query Timing Statistics is ON.
       * Get system time, then do again after the query.  This is
       * different than the above timings.  That one flags long queries;
       * this one gather stats.
       */
      start_time = ptetime_get_time();
   }

   switch (ptemsg_get_msg_subtype1 (p_msg_in))
   {
      case ST1_DB_SELECT_AUTH_ID    : p_msg_out = process_select_auth_id_msg (p_msg_in);
                                      break;
      case ST1_DB_SELECT            : p_msg_out = process_select_msg       (p_msg_in);
                                      break;
      case ST1_DB_UPDATE            : p_msg_out = process_update_msg       (p_msg_in);
                                      PRINT("returned from process_update_msg\n");                  
                                      break;
      case ST1_DB_INSERT            : p_msg_out = process_insert_msg       (p_msg_in);
                                      break;
      case ST1_DB_DELETE            : p_msg_out = process_delete_msg       (p_msg_in);
                                      break;
      case ST1_DB_GET_GUI_LIST      : p_msg_out = process_get_gui_list_msg (p_msg_in);
                                      break;
      case ST1_DB_GET_SERVICE_LIST  : p_msg_out = process_get_service_list_msg (p_msg_in);
                                      break;
      case ST1_DB_GET_SCH_LIST      : p_msg_out = process_get_sch_list_msg (p_msg_in);
                                      break;
      case ST1_DB_CHECK_DATA        : p_msg_out = process_check_data_in_tables (p_msg_in);
                                      break;                                     
      case ST1_DB_CHECK_BIN_RANGE   : p_msg_out = process_check_binn_range (p_msg_in);
                                      break;
      case ST1_DB_CHECK_BIN02_RANGE : p_msg_out = process_check_binn02_range (p_msg_in);
                                      break;
      case ST1_DB_ECHO              : p_msg_out = process_echo_msg         (p_msg_in);
                                     break;
      case ST1_DB_GET_COUNT         : p_msg_out = process_get_count        (p_msg_in);
                                      break;
      default:
         p_msg_out   = NULL_PTR;
         result_code = PTEMSG_INVALID_SUBTYPE1;
         sprintf (ErrorMsg, "Unknown subtype1 for database request from %s", reply_que);
         break;
   }

   if ( DB_Timing_Flag == true )
   {
      /* The functionality to gather DB Query Timing Statistics is ON.
       * Start time has been obtained. Now get end time and calculate
       * the duration and update the statistics.
       */
      end_time = ptetime_get_time();

      duration = end_time - start_time;

      update_timing_stats( MsgSubtype1, MsgSubtype2, app_data_type,
                           (float)duration, &TimingStats );
   }

   if ( ds_timer_flag == DS_TIMER_ON )
   {
      /* The functionality to check the time of a query is turned on.
       * See if the query exceeded desired time length.
       */
      ptetime_get_strtime( query_end_time );
      memset( errbuf, 0x00, sizeof(errbuf) );
      ret_val = compare_query_time_against_criteria( query_start_time,
                                                     query_end_time,
                                                     ds_timer_value,
                                                     p_msg_in,
                                                     AppName,
                                                     errbuf );
      if ( ret_val == false )
      {
         /* The query exceeded the time criteria.  Log informational message. */
        trands_log_message(2,2,errbuf," process_db_request ");
      }
   }
   return (p_msg_out);
}



/*************************************************************************************/
/*************************************************************************************/
void process_monitor_request(pPTE_MSG p_msg_in )
{
   CHAR           monitor_flag[100]="";
   pCHAR          orig_comm;
   CHAR           MonAdd[100]="";
   //pPTE_MSG       p_msg;
   //LONG           ipc_rcode;
   
   memset(AtpBuffer, 0, sizeof(AtpBuffer));
   switch (ptemsg_get_msg_subtype1 (p_msg_in))
   {
       case ST1_MONITOR_UP   : 
         orig_comm = ptemsg_get_msg_orig_comms_info(p_msg_in);
         WriteMemMonitorTable( (BYTE *)orig_comm );  
         /*
         if(ReadMemMonitorTable() != NULL)
         {
            strcpy(MonAdd, ReadMemMonitorTable());
            p_msg = ptemsg_build_msg(MT_SYS_MONITOR, ST1_OPRMSG, 0,applnk_que_name, application_que_name, (pBYTE) "Dataserver ready", 16,0);
            ptemsg_set_orig_comms_info(p_msg, MonAdd);
            ipc_rcode = pteipc_send(p_msg, applnk_que_name);
            free(p_msg);
       
         }
         else
            printf("Null value in MonAdd\n");*/
         break;
      case ST1_MONITOR_DOWN  :
         WriteMemMonitorTable((BYTE *)monitor_flag); 
         break;
      default:
         result_code = PTEMSG_INVALID_SUBTYPE1;
         sprintf (ErrorMsg, "Unknown subtype1 for database request from %s", reply_que);
         break;
   }
}

/*************************************************************************************/
/*************************************************************************************/
void process_logger_reply ( pPTE_MSG p_msg_in )
{
   pCHAR       p_msg_trid;


   switch (ptemsg_get_msg_subtype1 (p_msg_in))
   {
      case ST1_LOG_REFEED_COMPLETE : 
         in_refeed_mode = false;
         p_msg_trid     = ptemsg_get_current_msg_tran_id (p_msg_in);

         if (p_msg_trid [0] != '\0')
         {
            strncpy (checkpoint_rec.checkpoint_trid, p_msg_trid, sizeof(checkpoint_rec.checkpoint_trid));
            strncpy (last_trid, p_msg_trid, sizeof(last_trid));

            result_code = dbcommon_update_checkpoint (&checkpoint_rec, ErrorMsg);

            if (result_code == PTEMSG_OK)
            {
               printf ("Refeed is complete; ready for normal database activity\n");
               dbcommon_commit ();
            }
            else
            {
               dbcommon_rollback ();

               trands_log_message(1,3,ErrorMsg," process_logger_reply ");
               if (dbcommon_select_checkpoint (AppName, &checkpoint_rec, ErrorMsg) != PTEMSG_OK)
               {
                  trands_log_message(1,3,ErrorMsg," process_logger_reply ");
               }
            }
         }
                                     
         break;
      
      default:
         result_code = PTEMSG_INVALID_SUBTYPE1;
         sprintf (ErrorMsg, " Unknown subtype1 for logger reply from %s ", reply_que);

         trands_log_message(1,2,ErrorMsg," process_logger_reply ");
         break;
   }
}



/*************************************************************************************/
/*************************************************************************************/
BOOLEAN perform_refeed ( pCHAR trid )
{
   CHAR     Buffer[256];


   if (trid [0] != '\0')
   {
      /* send refeed command to TFLogger */
      if (SendRefeedCmd (trid, ErrorMsg) == false)
      {
         sprintf  (Buffer, " Error sending refeed command to TFLogger: %s ", ErrorMsg);
         trands_log_message(1,3,ErrorMsg," perform_refeed ");
         return (false);
      }

      in_refeed_mode = true;

      PRINT (" refeed command sent to logger\n");
   }

   return (true);
}



/*************************************************************************************/
/*************************************************************************************/
BYTE validate_last_trid ( pPTE_MSG p_msg_in )
{
   int         cmp_result;
   LONG        ipc_rcode;
   CHAR        temp_str [80];
   pCHAR       p_msg_last_trid;
   pPTE_MSG    p_msg_out;


   /* 
    * get a pointer to the msg last_trid.  if the pointer is null, error 
    */
   p_msg_last_trid = ptemsg_get_last_msg_tran_id (p_msg_in);

   /*
    * If the msg last_trid is empty, OK.  This is a request that came 
    * directly from an application (select, etc).  If it is not empty,
    * need to compare it with my last_trid.
    */
   if (p_msg_last_trid [0] != '\0')
   {
      if (last_trid [0] == '\0')
      {
         /*
          * My last_trid is empty.  Update it from this message.
          */
         strncpy (last_trid, p_msg_last_trid, sizeof(last_trid));
      }
      else
      {
         /*
          * My last_trid is not empty.  If it doesn't match the msg last_trid,
          * we have an out of sequence error.  Need to initiate a refeed from
          * my last_trid.
          */
         cmp_result = strncmp (last_trid, p_msg_last_trid, sizeof(last_trid));

         if (cmp_result != 0)
         {
            perform_refeed (last_trid);

            result_code = PTEMSG_INVALID_TRID;
            sprintf  (ErrorMsg, "invalid last-trid received: %s.   expected: %s",
                      p_msg_last_trid, last_trid); 
         }
      }
   }
   else
   {
      if (in_refeed_mode == true)
      {
         result_code = PTEMSG_IN_REFEED_MODE;
         strcpy (ErrorMsg, 
                 "In refeed mode; cannot accept application requests until complete");
      }
   }

   if (result_code != PTEMSG_OK)
   {
	   trands_log_message(1,2,ErrorMsg," validate_last_trid ");
	   /*
       * Try to create an error response message to send back to the 
       * originating application.  If it is a memory related error, this
       * may fail also (but I'll at least try).
       */
      reply_que = ptemsg_get_msg_orig_queue (p_msg_in);

      p_msg_out = ptemsg_build_msg (MT_DB_REPLY,        
                                    ptemsg_get_msg_subtype1 (p_msg_in),
                                    ptemsg_get_msg_subtype2 (p_msg_in),
                                    reply_que,
                                    application_que_name,
                                    (pBYTE) ErrorMsg,
                                    strlen(ErrorMsg) + 1,
                                    0);

      if (p_msg_out == NULL_PTR)
      {
         sprintf  (ErrorMsg, " Insufficient Memory to build error response to %s ", reply_que);
         trands_log_message(1,2,ErrorMsg," validate_last_trid ");
         return (result_code);
      }

      ptemsg_set_result_code (p_msg_out, result_code);
      ptemsg_set_orig_comms_info( p_msg_out, orig_comm_inf );
      /*
       * Send the response.
       */
      ipc_rcode = pteipc_send (p_msg_out, reply_que);

      free (p_msg_out);

      if (ipc_rcode < 0)
      {
    	  trands_log_message(1,2,ErrorMsg," validate_last_trid ");
    	  pteipc_get_errormsg (ipc_rcode, temp_str);
    	  sprintf (ErrorMsg, "error sending response to %s: %s ", reply_que, temp_str);
      }
   }

   return (result_code);
}



/*************************************************************************************/
/*************************************************************************************/
void rollback_the_update ()
{
   if (updates_to_db_made == true)
   {
      /* first, rollback the database update(s) */
      dbcommon_rollback ();

      /* then rollback any timer requests (makes sure memory table matches db) */
      dstimer_rollback ();

      updates_to_db_made = false;
   }
}  /* rollback_the_update */



/*************************************************************************************/
/*************************************************************************************/
void commit_the_update ()
{
   if (updates_to_db_made == true)
   {
      /* commit the database update(s) */
      dbcommon_commit ();

      /* let the timer know a commit has occurred */
      dstimer_commit ();

      updates_to_db_made = false;
   }
}  /* rollback_the_update */



/*************************************************************************************/
/*************************************************************************************/
BYTE PinnacleMsgHandler ( pPTE_MSG p_msg_in )
{
   LONG        i;
   LONG        num_messages;
   LONG        ipc_rcode;
   CHAR        temp_str [80];
   pPTE_MSG    p_cur_msg;
   pPTE_MSG    p_tmp_msg;
   pPTE_MSG    p_chn_out;
   pPTE_MSG    p_new_chn;
   pBYTE       p_data;
   LONG        ret_code;
   LONG        SectionSize;
   pPTE_MSG_DATA  p_msg_data;
   INT            ret_val;
   INT            num_sql;
   int         refresh_flag = 0;  // If an error in update_mem_record or delete_mem_record,
                              // delete all recs. in mem and recreate all tables over again 
                              // from the database.
   static DB_reconnect = 0;

   #ifdef USING_CHECKPOINTS
      pCHAR       p_msg_trid;
   #endif


   ptestats_accumulate_msg_stats (p_msg_in);

   result_code = PTEMSG_OK;

   #ifdef USING_CHECKPOINTS
      if ( (result_code = validate_last_trid (p_msg_in)) != PTEMSG_OK )
         return (result_code);
   #endif

   p_cur_msg    = p_msg_in;
   p_tmp_msg    = NULL_PTR;
   p_chn_out    = NULL_PTR;
   i            = 1;
   num_messages = ptemsg_get_num_chained_messages (p_msg_in);
   /* Added for UpdateMemRecord stuff */
   p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
   p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
   app_data_type = ptemsg_get_pte_msg_data_app_data_type( p_msg_data );
   p_data = p_data + ISO_MSG_LENGTH1 + sizeof(AUTH_TX);


   while ( (i <= num_messages) && (p_cur_msg != NULL_PTR) )
   {
      reply_que = ptemsg_get_msg_orig_queue (p_cur_msg);
      strcpy(orig_comm_inf, ptemsg_get_msg_orig_comms_info( p_cur_msg ));
      app_data_type = 0;
      DB_reconnect = 0;
      switch( ptemsg_get_msg_type( p_cur_msg ) )
      {
         case MT_DB_REQUEST     : p_tmp_msg = process_db_request (p_cur_msg);
                                  break;
         case MT_TIMER_REQUEST  : p_tmp_msg = dstimer_process_request (p_cur_msg, &updates_to_db_made);       
                                  break;            

         case MT_SYSTEM_REQUEST :
                  ptesystm_system_msg_handler (p_cur_msg);
                  if ( ST1_SYS_PING == ptemsg_get_msg_subtype1(p_msg_in) )
                  {
                     if ( ST2_NO_VERSION != ptemsg_get_msg_subtype2(p_msg_in) )
                     {
                        /* When pinged, display application version on Monitor. */
                        sprintf( temp_str,
                                "Pinged -> Data Server: %s, version %s ",
                                 ServiceName, Version );
                        trands_log_message(1,1,temp_str," PinnacleMsgHandler ");

                        /* Get Database Timing Parameters to gather
                         * statistics about each query.
                         */
                        memset( ReportTime, 0x00, sizeof(ReportTime) );
                        memset( temp_str,   0x00, sizeof(temp_str)   );
                        ret_val = get_timing_parameters( &DB_Timing_Flag,
                                                          ReportTime, 
                                                         &ForceReport,
                                                          temp_str );
                        trands_log_message(2,2,temp_str," PinnacleMsgHandler ");
                        if ( DB_Timing_Flag == true )
                        {
                           /* Get the time for when the DB Timing
                            * Statistical Report is to be logged.
                            */
                           NextReportTime = set_next_report_time_at_startup( ReportTime );

                           if ( ForceReport == true )
                           {
                              /* User wants to log the stats
                               * right now.  Log them then
                               * reset the statistics.
                               */
                              num_sql = log_timing_statistics_report( &TimingStats );
                              memset( temp_str, 0x00, sizeof(temp_str) );
                              sprintf( temp_str,
                                      "%s logged stats for %d SQLs",
                                       AppName, num_sql );
                              trands_log_message(2,1,temp_str," PinnacleMsgHandler ");
                              reset_timing_stats( &TimingStats );
                           }
                        }
                     }
                  }
                  else if ( ST1_SYS_NULLCHECK_OFF == ptemsg_get_msg_subtype1(p_msg_in))
                  {
                	  DB_NullCheck_Flag = 0 ;
                	  send_nullcheck_dbstatics_response(p_msg_in);
                  }
                  else if ( ST1_SYS_NULLCHECK_ON == ptemsg_get_msg_subtype1(p_msg_in))
                  {
                	  DB_NullCheck_Flag = 1 ;
                	  send_nullcheck_dbstatics_response(p_msg_in);
                  }
                  else if ( ST1_SYS_DBSTATICS_OFF == ptemsg_get_msg_subtype1(p_msg_in))
                  {
                	  db_error_statics_flag = 0 ;
                	  send_nullcheck_dbstatics_response(p_msg_in);
                  }
                  else if ( ST1_SYS_DBSTATICS_ON == ptemsg_get_msg_subtype1(p_msg_in))
                  {
                	  db_error_statics_flag = 1 ;
                	  send_nullcheck_dbstatics_response(p_msg_in);
                  }
				#ifdef HCTS2
                  else if(ST1_VERTEXON_CARD_STATUS_FLAG == ptemsg_get_msg_subtype1(p_msg_in))
                  {
                	  trands_read_atp_card_activation_parameters();
                	  trands_log_message(3, 1, "Vertexon card status parameters reloaded", " PinnacleMsgHandler ");
                	  send_nullcheck_dbstatics_response(p_msg_in);
                  }
				#endif
                  break;

         case MT_LOGGER_REPLY   : process_logger_reply (p_cur_msg);
                                  break;
         case MT_SYS_MONITOR    : process_monitor_request (p_cur_msg);
                                  break;
         case MT_CLEANUP        : process_db_cleanup    (p_cur_msg);
                                  break;
         default :
            /*
             * Received an unknown msg type.  If the request was destined for
             * this dataserver, log an error because I can't process it.
             * Otherwise, try to forward it on to the actual destination.
             */
            if ( (strncmp (ptemsg_get_msg_dest_queue (p_cur_msg),
                           application_que_name, strlen(application_que_name)) == 0) ||
                 (strncmp (ptemsg_get_msg_dest_queue (p_cur_msg),
                           control_que_name,     strlen(control_que_name)    ) == 0)   )
            {
               result_code = PTEMSG_INVALID_MSGTYPE;
               sprintf (ErrorMsg, "Unknown msgtype for request from %s", reply_que);
            }
            else
            {
               /*
                * Forward the request to its destination (if not in refeed mode).
                */
               if (in_refeed_mode == false)
               {
                  sprintf (temp_str, "forwarding message to queue %s\n", 
                           ptemsg_get_msg_dest_queue (p_cur_msg));
                  PRINT (temp_str);

                  ptemsg_set_orig_comms_info( p_cur_msg, orig_comm_inf );
                  ipc_rcode = pteipc_send (p_cur_msg, ptemsg_get_msg_dest_queue (p_cur_msg));

                  if (ipc_rcode < 0)
                  {
                     result_code = PTEMSG_COULD_NOT_FORWARD;
                     pteipc_get_errormsg (ipc_rcode, temp_str);
                     sprintf  (ErrorMsg, "error forwarding to queue %s: %s", 
                               ptemsg_get_msg_dest_queue (p_cur_msg), temp_str);
                  }
               }
            }

            break;
      }  /* switch msg_type */


      if (result_code != PTEMSG_OK)
      {
     	 if( NOT_CONNECTED_TO_DB 				== result_code ||
     		 NOT_LOGGED_INTO_DB 				== result_code ||
			 TNS_PACKET_WRITER_FAILED 			== result_code ||
			 SHUTDOWN_OR_STARTUP_IN_PROGRESS	== result_code  ||
			 PTEMSG_NOT_CONNECTED_TO_DB			== result_code )
     	 {
     		 DB_reconnect = 1;
     	 }
     	 else
     	 {
     		DB_reconnect = 0;
     	 }
         break;
      }
      if (p_tmp_msg != NULL_PTR)
      {
         result_code = ptemsg_get_result_code (p_tmp_msg);
         
         if (result_code != PTEMSG_OK)
         {
        	 if( NOT_CONNECTED_TO_DB 				== result_code ||
        		 NOT_LOGGED_INTO_DB 				== result_code ||
				 TNS_PACKET_WRITER_FAILED 			== result_code ||
				 SHUTDOWN_OR_STARTUP_IN_PROGRESS	== result_code  ||
				 PTEMSG_NOT_CONNECTED_TO_DB			== result_code )
        	 {
        		 DB_reconnect = 1;
        	 }
        	 else
			 {
				DB_reconnect = 0;
			 }
            free (p_chn_out);
            p_chn_out = p_tmp_msg;
            p_tmp_msg = NULL_PTR;
            break;
         }
         
         if (p_chn_out == NULL_PTR)
         {
            p_chn_out = p_tmp_msg;
            p_tmp_msg = NULL_PTR;
         }
         else
         {
            p_new_chn = ptemsg_chain_message (p_chn_out, p_tmp_msg);

            if (p_new_chn == NULL_PTR)
            {
               result_code = PTEMSG_INSUFFICIENT_MEMORY;
               sprintf  (ErrorMsg, "Insufficient Memory to chain response to %s", reply_que);
               break;
            }
            else
            {
               p_chn_out = p_new_chn;
               p_tmp_msg = NULL_PTR;
            }
         }
      }

      if ((++i) <= num_messages)
         p_cur_msg = ptemsg_get_chained_pte_msg (p_msg_in, i);
   }  /* while */


   /*
    * Be sure to free up the temp message.
    */
   if (p_tmp_msg != NULL_PTR)
   {
      free (p_tmp_msg);
      p_tmp_msg = NULL_PTR;
   }

   #ifdef USING_CHECKPOINTS
      /*
       * Update the checkpoint.
       */
      if (result_code == PTEMSG_OK)
      {
         p_msg_trid = ptemsg_get_current_msg_tran_id (p_msg_in);

         if (p_msg_trid [0] != '\0')
         {
            strncpy (checkpoint_rec.checkpoint_trid, p_msg_trid, sizeof(checkpoint_rec.checkpoint_trid));
            strncpy (last_trid, p_msg_trid, sizeof(last_trid));

            result_code = dbcommon_update_checkpoint (&checkpoint_rec, ErrorMsg);
         }
      }
   #endif

   /*
    * If there was an error, back out the changes.
    * Otherwise commit the changes.
    */
   reply_que = ptemsg_get_msg_orig_queue (p_msg_in);

   if (result_code != PTEMSG_OK)
   {
      rollback_the_update ();

      if (p_chn_out != NULL_PTR)
      {
         free (p_chn_out);
         p_chn_out = NULL_PTR;
      }

      /*
       * Try to create an error response message to send back to the 
       * originating application.  If it is a memory related error, this
       * may fail also (but I'll at least try).
       */
      memcpy(AtpBuffer + sizeof(AUTH_TX) + ISO_MSG_LENGTH1, ErrorMsg, strlen(ErrorMsg) + 1);
      p_chn_out = ptemsg_build_msg (MT_DB_REPLY,        
                                    ptemsg_get_msg_subtype1 (p_cur_msg),
                                    ptemsg_get_msg_subtype2 (p_cur_msg),
                                    reply_que,
                                    application_que_name,
                                    (pBYTE) AtpBuffer,
                                    strlen(ErrorMsg) + 1 + sizeof(AUTH_TX) + ISO_MSG_LENGTH1,
                                    app_data_type);

      if (p_chn_out == NULL_PTR)
      {
    	  trands_log_message(1,2,ErrorMsg," PinnacleMsgHandler ");
    	  result_code = PTEMSG_INSUFFICIENT_MEMORY;
    	  sprintf (ErrorMsg, "Insufficient Memory to build error response to %s ", reply_que);
      }
   }
   else
   {
      commit_the_update (); /* We need uncomment below code- once XCOM fixed on memory creation*/
    
      if(  MsgSubtype1 == ST1_DB_UPDATE || MsgSubtype1 == ST1_DB_INSERT || MsgSubtype1 == ST1_DB_DELETE )
	 //if(  MsgSubtype1 == ST1_DB_UPDATE )	
	  {
         if((app_data_type == BIN01_DATA) && (Bin01InMem == true))
         {
            SectionSize = GetSectionSize("Bin01Table");
            ret_code = CreateTable(app_data_type);
            if(ret_code >=0)
            {
               MemAvailable = MemAvailable - SectionSize + ret_code;
               ret_code = PopulateTable(app_data_type);
               if(ret_code >=0)
                  Bin01InMem = true;
               else
                  Bin01InMem = false;
            }
         }
         if((app_data_type == CAF01_DATA) && (Caf01InMem == true))
         {
            SectionSize = GetSectionSize("Caf01Table");
            ret_code = CreateTable(app_data_type);
            if(ret_code >=0)
            {
               MemAvailable = MemAvailable - SectionSize + ret_code;
               ret_code = PopulateTable(app_data_type);
               if(ret_code >=0)
                  Caf01InMem = true;
               else
                  Caf01InMem = false;
            }
         }
         if((app_data_type == FAF01_DATA) && (Faf01InMem == true))
         {
            SectionSize = GetSectionSize("Faf01Table");
            ret_code = CreateTable(app_data_type);
            if(ret_code >=0)
            {
               MemAvailable = MemAvailable - SectionSize + ret_code;
               ret_code = PopulateTable(app_data_type);
               if(ret_code >=0)
                  Faf01InMem = true;
               else
                  Faf01InMem = false;
            }
         }
         if((app_data_type == CRF01_DATA) && (Crf01InMem == true))
         {
            SectionSize = GetSectionSize("Crf01Table");
            ret_code = CreateTable(app_data_type);
            if(ret_code >=0)
            {
               MemAvailable = MemAvailable - SectionSize + ret_code;
               ret_code = PopulateTable(app_data_type);
               if(ret_code >=0)
                  Crf01InMem = true;
               else
                  Crf01InMem = false;
            }
         }
         if((app_data_type == CSF01_DATA) && (Csf01InMem == true))
         {
            SectionSize = GetSectionSize("Csf01Table");
            ret_code = CreateTable(app_data_type);
            if(ret_code >=0)
            {
               MemAvailable = MemAvailable - SectionSize + ret_code;
               ret_code = PopulateTable(app_data_type);
               if(ret_code >=0)
                  Csf01InMem = true;
               else
                  Csf01InMem = false;
            }
         }
         if((app_data_type == BON02_DATA) && (Bon02InMem == true))
         {
            SectionSize = GetSectionSize("Bon02Table");
            ret_code = CreateTable(app_data_type);
            if(ret_code >=0)
            {
               MemAvailable = MemAvailable - SectionSize + ret_code;
               ret_code = PopulateTable(app_data_type);
               if(ret_code >=0)
                  Bon02InMem = true;
               else
                  Bon02InMem = false;
            }
         }
         if((app_data_type == BON01_DATA) && (Bon01InMem == true))
         {
            SectionSize = GetSectionSize("Bon01Table");
            ret_code = CreateTable(app_data_type);
            if(ret_code >=0)
            {
               MemAvailable = MemAvailable - SectionSize + ret_code;
               ret_code = PopulateTable(app_data_type);
               if(ret_code >=0)
                  Bon01InMem = true;
               else
                  Bon01InMem = false;
            }
         }
      }
      
   }

   if (p_chn_out != NULL_PTR)
   {
      /*
       * If in refeed mode, don't send the response.
       */
      if (in_refeed_mode == true)
         return (PTEMSG_OK);

      /*
       * If the result is OK, don't send the response if they don't want it.
       */
      if ( (result_code                                == PTEMSG_OK                 ) &&
           (ptemsg_get_type_of_reply_needed (p_msg_in) == PTEMSG_REPLY_ONLY_IF_FAILS) )
      {
         free (p_chn_out);
         return (PTEMSG_OK);
      }

      ptemsg_set_result_code (p_chn_out, result_code);
      ptemsg_set_orig_comms_info( p_chn_out, orig_comm_inf );

      /*
       * Send the response.
       */
      ipc_rcode = pteipc_send (p_chn_out, reply_que);

      free (p_chn_out);

      if (ipc_rcode < 0)
      {
         trands_log_message(1,2,ErrorMsg," PinnacleMsgHandler ");
    	 result_code = PTEMSG_IPC_ERROR;
         pteipc_get_errormsg (ipc_rcode, temp_str);
         sprintf (ErrorMsg, "error sending response to %s: %s ", reply_que, temp_str);
      }
   }
   //Database errors (ex: select failed) are no more sent to Eventlog
   /*if (result_code != PTEMSG_OK)
   {
      trands_log_message(2,2,ErrorMsg,"PinnacleMsgHandler");
   }*/
   if(DB_reconnect == 1)
   {
	   dbcommon_disconnect (ErrorMsg);
	   if (dbcommon_connect ("equitable", "equitable", "equitable", "ORCL", ErrorMsg) != PTEMSG_OK)
	   {
		   strcat (ErrorMsg, " DB Reconnection failed");
		   trands_log_message(1, 3, ErrorMsg, "PinnacleMsgHandler");
			return (false);
	   }
	   else
	   {
		   trands_log_message(1, 1, "DataBase Recoonected", "PinnacleMsgHandler");
	   }
   }
   else
   {
	   DB_reconnect = 0;
   }
   return (result_code);
}



/*************************************************************************************/
/*************************************************************************************/
BOOLEAN perform_startup ()
{
   CHAR     XipcInstanceName[12];
   INT      ret_val;
   CHAR     Buffer[300] = "";

   memset (last_trid, 0, sizeof(last_trid));

   /* Try to connect to the database */
   if (dbcommon_connect ("equitable", "equitable", "equitable", "ORCL", ErrorMsg) != PTEMSG_OK)
   {
      printf   ( "%s", ErrorMsg );
      trands_log_message(1,3,ErrorMsg," perform_startup ");
      return (false);
   }

   printf ("Connected to ORACLE\n");


   /* Try to initialise the xipc stuff */
   GetAppName          (AppName);
   GetXipcInstanceName (XipcInstanceName);

#ifndef WIN32
   /* Diagnostic code to trap the following Unix signals. */
   sigset( SIGALRM, TrapFunction );
   sigset( SIGBUS,  TrapFunction );
   sigset( SIGEMT,  TrapFunction );
   sigset( SIGFPE,  TrapFunction );
   sigset( SIGILL,  TrapFunction );
   sigset( SIGIOT,  TrapFunction );
   sigset( SIGKILL, TrapFunction );
   sigset( SIGPIPE, TrapFunction );
   sigset( SIGPOLL, TrapFunction );
   sigset( SIGPWR,  TrapFunction );
   sigset( SIGQUIT, TrapFunction );
   sigset( SIGSEGV, TrapFunction );
   sigset( SIGSYS,  TrapFunction );
   sigset( SIGTRAP, TrapFunction );
   sigset( SIGUSR1, TrapFunction );
   sigset( SIGUSR2, TrapFunction );
#endif



   //12-07-98 gabriel multiple instance

//   if (pteipc_init_single_instance_app (AppName, XipcInstanceName) == false)
   if( !pteipc_init_multiple_instance_app( AppName, ServiceName, XipcInstanceName ) )
   {     
      trands_log_message(1,3,"Failed to create XIPC queues  "," perform_startup ");
	  pteipc_shutdown_multiple_instance_app();
      dbcommon_disconnect (ErrorMsg);
      return (false);
   }
   
   printf ("pteipc init successful\n");


   #ifdef USING_LOGGER_REPLY
      /* send restart command to TFLogger */
      if (SendRestartCmd (ErrorMsg) == false)
      {
         trands_log_message(1,3,"Error sending restart command to TFLogger "," perform_startup ");
         dstimer_shutdown ();
         //pteipc_shutdown_single_instance_app();
         pteipc_shutdown_multiple_instance_app();
         dbcommon_disconnect (ErrorMsg);
         return (false);
      }

      printf ("restart command sent to logger\n");

      #ifdef USING_CHECKPOINTS
         /* get the checkpoint record from the database */
         if (dbcommon_select_checkpoint (AppName, &checkpoint_rec, ErrorMsg) != PTEMSG_OK)
         {
            trands_log_message(1,3,ErrorMsg," perform_startup ");
            dstimer_shutdown ();
            pteipc_shutdown_multiple_instance_app();
            dbcommon_disconnect (ErrorMsg);
            return (false);
         }

         /* 
          * refeed from my last known trid.
          */
         strncpy (last_trid, checkpoint_rec.checkpoint_trid, sizeof(last_trid));

         if (perform_refeed (checkpoint_rec.checkpoint_trid) == false)
         {
            dstimer_shutdown ();
            //pteipc_shutdown_single_instance_app();
            pteipc_shutdown_multiple_instance_app();
            dbcommon_disconnect (ErrorMsg);
            return (false);
         }
      #endif
   #endif

   ret_val = get_dataserver_ini_timer_value( &ds_timer_flag, &ds_timer_value,
                                             AppName,         Buffer );
   if ( false == ret_val )
   {
	   trands_log_message(1,2,Buffer," perform_startup ");
   }
   else if ( ds_timer_flag == DS_TIMER_ON )
   {
      sprintf( Buffer,
              "%s:Data Server timing of queries is turned on, value is %d ",
               AppName, ds_timer_value );
      trands_log_message(2,1,Buffer," perform_startup ");
   }

   /* Get Database Timing Parameters to gather statistics about each query. */
   memset( ReportTime, 0x00, sizeof(ReportTime) );
   memset( Buffer,     0x00, sizeof(Buffer)     );
   ret_val = get_timing_parameters( &DB_Timing_Flag, ReportTime, &ForceReport, Buffer );
   trands_log_message(2,2,Buffer," perform_startup ");

   /* Get the time for when the DB Timing Statistical Report is to be logged. */
   if ( DB_Timing_Flag == true )
   {
      reset_timing_stats( &TimingStats );
      NextReportTime = set_next_report_time_at_startup( ReportTime );
   }


   /* Cache BON02 Cash Bonus table into Oracle memory. */
   if( PTEMSG_OK != db_cache_bon02(Buffer) )
   {
       trands_log_message(2,2,Buffer," perform_startup ");
       trands_log_message(2,2,"Unable to cache BON02 into Oracle memory. "," perform_startup ");
   }

   return (true);
}
/*************************************************************************************/
/*************************************************************************************/
/*int create_generic_table(pCHAR TableName)
{
   LONG                 xipc_code;
   char                 Flag;

   MemSize = GetMemorySize();
   MemAvailable = MemSize;

   if(MemAvailable > 0L)
   {
      xipc_code = CreateXipcTable(1, 10, TableName);
      if(xipc_code > 0L)
      {
         MemAvailable = MemAvailable - xipc_code;
         xipc_code = WriteTable(TableName, 10 + 1, &Flag);
         if(xipc_code >= 0L)
            GenTablInMem = true;      
         else
            GenTablInMem = false;
      }
      return 0;
   }
   else
      return (-1);
}*/
/*************************************************************************************/
/*************************************************************************************/

#ifndef WIN32

void TrapFunction( INT signum )
{
   CHAR Buffer[100];

   XIPC_TRAP_FUNCTION_TEST( TrapFunction, signum );

   memset( Buffer, 0x00, sizeof(Buffer) );
   sprintf( Buffer, "%s:  Caught signal %d", AppName, signum );
   trands_log_message(2,3,Buffer," TrapFunction ");
}

#endif

void MainProcessor()
{
	LONG     ipc_rcode, xipc_code;
	pPTE_MSG p_msg;
	CHAR     Buffer[256];
	CHAR     MonAdd[100]="";
	INT      rt;
	INT      num_sql;
	INT      err_ctr = 0;
	CHAR     strError[512] = {0} ;
	CHAR  time_date[25] = "";
	CHAR  current_mmdd[5]={0};
	INT   len=0;
	int 		ret_val=0;


	   GetAppName( AppName );

   #ifdef _DEBUG
      pteipc_trace_on();
   #endif
   sprintf( Buffer,
           "Starting the Service: %s, version %s ",
            AppName, Version );
   	trands_log_message(2,1,Buffer," MainProcessor ");
	printf( "%s\n", Buffer );

   if (perform_startup () == false)
   {
      MainProcessDone = 1;
	  	     sprintf( Buffer,
           "Shutting down the Service: %s, version %s ",
            AppName, Version );
	  	    trands_log_message(2,1,Buffer," MainProcessor ");
			printf( "%s\n", Buffer );

      return;
   }

   ret_val=get_dataserver_ini_db_error_statics_value( &db_error_statics_flag, &db_error_statics_value,
                                                   ServiceName,     Buffer );
   if ( false == ret_val )
   {
       trands_log_message(2,2,Buffer,"MainProcessor");
   }
   else if ( db_error_statics_flag == DS_TIMER_ON )
   {
        sprintf( Buffer,
               "%s:Data Server DB error statics for queries is turned on, value is %d ",
               ServiceName, db_error_statics_value );
      trands_log_message(2,1,Buffer," MainProcessor ");
   }


   Read_Max_BIN01_Table_Size_For_XCOM_Memory();
   MemSize = GetMemorySize();
   MemAvailable = MemSize;
   if(MemAvailable > 0L)
   {
      xipc_code = CreateTable(OPRMON_DATA);
      if(xipc_code > 0L)
      {
         MemAvailable = MemAvailable - xipc_code;
         xipc_code = PopulateTable(OPRMON_DATA);
        if(xipc_code >= 0L)
            OprMonInMem = true;
         else
            OprMonInMem = false;
      }
   }

   if(MemAvailable > 0L)
   {
      xipc_code = CreateTable(BIN01_DATA);
      if(xipc_code > 0L)
      {
         MemAvailable = MemAvailable - xipc_code;
         xipc_code = PopulateTable(BIN01_DATA);
         if(xipc_code >= 0L)
            Bin01InMem = true;
         else
            Bin01InMem = false;
      }
   }
   if(MemAvailable > 0L)
   {
      xipc_code = CreateTable(CAF01_DATA);
      if(xipc_code > 0L)
      {
         MemAvailable = MemAvailable - xipc_code;
         xipc_code = PopulateTable(CAF01_DATA);
         if(xipc_code >= 0L)
            Caf01InMem = true;
         else
            Caf01InMem = false;
      }
   }
   if(MemAvailable > 0L)
   {
      xipc_code = CreateTable(FAF01_DATA);
      if(xipc_code > 0L)
      {
         MemAvailable = MemAvailable - xipc_code;
         xipc_code = PopulateTable(FAF01_DATA);
         if(xipc_code >= 0L)
            Faf01InMem = true;
         else
            Faf01InMem = false;
      }
   }
   if(MemAvailable > 0L)
   {
      xipc_code = CreateTable(CRF01_DATA);
      if(xipc_code > 0L)
      {
         MemAvailable = MemAvailable - xipc_code;
         xipc_code = PopulateTable(CRF01_DATA);
         if(xipc_code >= 0L)
            Crf01InMem = true;
         else
            Crf01InMem = false;
      }
   }
   if(MemAvailable > 0L)
   {
      xipc_code = CreateTable(CSF01_DATA);
      if(xipc_code > 0L)
      {
         MemAvailable = MemAvailable - xipc_code;
         //printf("Csf01 populate %d\n", xipc_code);
         if(xipc_code >= 0L)
            Csf01InMem = true;
         else
            Csf01InMem = false;
      }
   }
   if(MemAvailable > 0L)
   {
      xipc_code = CreateTable(BON02_DATA);
      if(xipc_code > 0L)
      {
         MemAvailable = MemAvailable - xipc_code;
         xipc_code = PopulateTable(BON02_DATA);
         if(xipc_code >= 0L)
            Bon02InMem = true;
         else
            Bon02InMem = false;
      }
   }

   if(MemAvailable > 0L)
   {
      xipc_code = CreateTable(BON01_DATA);
      if(xipc_code > 0L)
      {
         MemAvailable = MemAvailable - xipc_code;
         xipc_code = PopulateTable(BON01_DATA);
         if(xipc_code >= 0L)
            Bon01InMem = true;
         else
            Bon01InMem = false;
      }
   }

   if ( MemAvailable > 0L )
   {
      rt = CreateSQL01_Table();
      if ( rt < 0 )
      {
         sprintf( Buffer, "Unable to create SQL01 in memory. Xipc err %d", rt );
         trands_log_message(2,2,Buffer,"MainProcessor");
      }
   }

   if ( MemAvailable > 0L )
   {
      rt = CreateTable(UAG01_DATA);
      if ( rt < 0 )
      {
         sprintf( Buffer, "Unable to create Auth table in memory. Xipc err %d ", rt );
         trands_log_message(2,2,Buffer," MainProcessor ");
      }
   }

   sprintf( Buffer,
           "Starting the Service: %s, version %s ",
            ServiceName, Version );
   trands_log_message(2,1,Buffer,"MainProcessor ");
   printf( "%s\n", Buffer );

   HstFilePtr=NULL;
   printf ( "DataServer is ready. press <ENTER> to exit\n" );
   get_HST01_file_name_path();

   trands_read_CUP_merchant_indicator();

#ifdef HCTS2
   trands_read_atp_card_activation_parameters();
#endif

   		HstFilePtr = fopen(HST01_Filename,"a");
   		    	if(HstFilePtr != NULL)
   		    	{
   		    		long int File_Size = findHST01FileSize(HST01_Filename);
   		    		if(File_Size >= HEADER_SIZE)
   		    		{

   		    			/*it means the header is present so just continue with normal flow and let
   		    			   		    			the hst01Handler function handle the data appending into the file*/
   		    		}
   		    		else
   		    		{
   		    			//The header is not present. Create the header.
   		    			WriteDSVHeader();
   		    			fclose(HstFilePtr);

   		    		}
   		    	}
   		    	else
   		    	{

   		 	    	sprintf(Buffer,"Cannot open trands log file before while loop: %s",HST01_Filename);
   		 	    	trands_log_message( 2, 1, Buffer, "MainProcessor");
   		    	}

   while ( !EndProcessSignalled )
   {
      /* You are blocked here waiting for a message on either app queue or control que  */
      /* if there is no message on either que for the specified time, the blocking call returns   */

		ptetime_get_timestamp( time_date );

	    memcpy( current_mmdd,   time_date+5, 2 );
	    memcpy( current_mmdd+2, time_date+8, 2 );
	    len = strlen(HST01_Filename);

     	    	if( 0 != strncmp(&HST01_Filename[len-8], current_mmdd,4) )
     	    	{

     	     	    trands_create_Error_Filename();
     	     	   sprintf(Buffer,"the file name after the while loop is: %s",HST01_Filename);
     	     	   trands_log_message(2,1,Buffer,"MainProcessor ");

     		   		HstFilePtr = fopen(HST01_Filename,"a");
     		   		 if(HstFilePtr != NULL)
     		   		  {
     		   		    long int File_Size = findHST01FileSize(HST01_Filename);
     			    	if(File_Size >= HEADER_SIZE)
     			    	{
     			    		/*it means the header is present so just continue with normal flow and let
     			    		  the hst01Handler function handle the data appending into the file*/

     			    	}
     		   		    else
     		   		    {
     		   		    	//The header is not present. Create the header.
     		   		    	WriteDSVHeader();
     		   		    	fclose(HstFilePtr);

     		   		    }
     		   		  }
     		   		 else
     		   		 {

     		 	    	sprintf(Buffer,"Cannot open trands log file after while loop: %s",HST01_Filename);
     		 	    	trands_log_message( 2, 1, Buffer, "MainProcessor");
     		   		 }

     	    	}




      p_msg = pteipc_receive( application_que_name, control_que_name, 
                              TIME_BETWEEN_CHECKS, &ipc_rcode );
 
      if( p_msg != NULL_PTR )
      {
         PinnacleMsgHandler( p_msg );
         free (p_msg);
      }
      else if( ipc_rcode != QUE_ER_TIMEOUT ) 
      {
         pteipc_get_errormsg ( ipc_rcode, ErrorMsg );
         sprintf ( Buffer, "Error on pteipc_receive %s ", ErrorMsg );
         trands_log_message(1,2,Buffer," MainProcessor ");
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
                    "SHUT DOWN %s. UNABLE TO ACCESS QUEUE!!!! ",
                     ServiceName );
            trands_log_message(1,3,strError," MainProcessor ");
            #ifdef WIN32
               Sleep (5000);
            #else
               usleep (5000000);
            #endif
         }

         /* Comment the following out after the production stability
          * problem is resolved.
          */
         /* This line is to gather XIPC info *
         memset( XipcErrBuffer, 0x00, sizeof(XipcErrBuffer) );
         XipcSystemErrorReport( XipcErrBuffer, sizeof(XipcErrBuffer)-1 );
         trands_log_message(2,3,XipcErrBuffer," MainProcessor ");
         exit(1);
         */
      }
      else
      {
         in_refeed_mode = false;

         /* The queue read has timed out.  This means there is no
          * activity for this application.  Take this opportunity
          * to see if it is time to log the database timing stats.
          */
         if ( is_it_report_time(NextReportTime) )
         {
            /* Time to report the DB timing measurement statistics. */
            num_sql = log_timing_statistics_report( &TimingStats );

            memset( Buffer, 0x00, sizeof(Buffer) );
            sprintf( Buffer,
                    "%s logged stats for %d SQLs ",
                     AppName, num_sql );
            trands_log_message(2,1,Buffer," MainProcessor ");

            reset_timing_stats( &TimingStats );
            NextReportTime = set_next_report_time_at_startup( ReportTime );
         }
      }
   }

   dstimer_shutdown ();

   if(Bin01InMem)
   {
      xipc_code = DestroyXipcTable("Bin01Table");
   }
   if(Caf01InMem)
   {
      xipc_code = DestroyXipcTable("Caf01Table");
   }
   if(Faf01InMem)
   {
      xipc_code = DestroyXipcTable("Faf01Table");
   }
   if(Crf01InMem)
   {
      xipc_code = DestroyXipcTable("Crf01Table");
   }
   if(Csf01InMem)
   {
      xipc_code = DestroyXipcTable("Csf01Table");
   }
   if(OprMonInMem)
   {
      xipc_code = DestroyXipcTable("MonitorTable");
   }
   if(Bon01InMem)
   {
      xipc_code = DestroyXipcTable("Bon01Table");
   }
   if(Bon02InMem)
   {
      xipc_code = DestroyXipcTable("Bon02Table");
   }

   /* shut down ipc */
   //pteipc_shutdown_single_instance_app(); 
   pteipc_shutdown_multiple_instance_app(); 

   printf ("pteipc shutdown successful\n");

   /* disconnect from database */
   if (dbcommon_disconnect (ErrorMsg) != PTEMSG_OK)
      printf ( "%s\n", ErrorMsg );
   else
      printf ( "Disconnected from ORACLE\n" );

   /* signal that we are ready for shutdown */
   MainProcessDone = 1;
}



/*************************************************************************************/
/*************************************************************************************/
void EndProcess()
{
   CHAR Buffer[100] = "";

   sprintf( Buffer, "Shutting down the %s Service, version %s ",
            ServiceName, Version );
   trands_log_message(2,1,Buffer," EndProcess ");
   strcat( Buffer, "\n" );
   PRINT( Buffer );

   pteipc_sleep (3000);
}

INT trands_Check_If_CUP_Transaction (pAUTH_TX auth_tx)
{
	if ((0 == strncmp(auth_tx->TLF01_details.product_code,CUP_CARD_INDICATOR_FLAG,2))||
		(0 == strncmp(auth_tx->TLF01_details.merchant_id,CUP_merchant_indicator,CUP_MID_INDICATOR_LEN)))
	{
		return true ;
	}
	else
	{
		return false;
	}
}

void strTrim_For_Mid_blocking (char* strSource, char* strDestination)
{
	int Src_index1 = 0, Dst_index2 = 0, last_pace_index3 = 0, CurrentChar = 0, firstValidCharacter = 0 ,Nextchar = 0, space_falg=0;
	char temp_mid[15] = {0} ;

	memset (strDestination, 0, sizeof (strDestination)) ;

	while ((CurrentChar = strSource[Src_index1++]) != 0)
	{
		if (CurrentChar > 0x20 ) // valid character
		{
			if(space_falg == 1)
			{
				space_falg =   0 ;
				//last_pace_index3     =   0 ;
			}
			//firstValidCharacter = 1 ;
			strDestination[Dst_index2++] = CurrentChar ;
		}
		else if(CurrentChar == 0x20)
		{
			if(space_falg == 0)
			{
				last_pace_index3 = Dst_index2;
			}
			space_falg = 1;
			strDestination[Dst_index2++] = CurrentChar ;
		}
	}
	if(space_falg == 1)
	{
		strDestination[last_pace_index3] = '\0' ;
	}
	else
	{
		strDestination[Dst_index2] = '\0' ;
	}

} /* strTrim */



/*void CheckNULLTerminatedinUCF01(pUCF01 ucf01)
{
	checknull(ucf01->primary_key.name,sizeof(ucf01->primary_key.name));
	checknull(ucf01->password			  			   ,sizeof(ucf01->password));
	checknull(ucf01->date_added                           ,sizeof(ucf01->date_added));
	checknull(ucf01->group_id                         ,sizeof(ucf01->group_id));
	checknull(ucf01->control[0].subsystem_name          ,sizeof(ucf01->control[0].subsystem_name));
	checknull(ucf01->control[0].action          ,sizeof(ucf01->control[0].action));
	checknull(ucf01->control[1].subsystem_name          ,sizeof(ucf01->control[1].subsystem_name));
	checknull(ucf01->control[1].action          ,sizeof(ucf01->control[1].action));
	checknull(ucf01->control[2].subsystem_name          ,sizeof(ucf01->control[2].subsystem_name));
	checknull(ucf01->control[2].action          ,sizeof(ucf01->control[2].action));
	checknull(ucf01->control[3].subsystem_name          ,sizeof(ucf01->control[3].subsystem_name));
	checknull(ucf01->control[3].action          ,sizeof(ucf01->control[3].action));
	checknull(ucf01->control[4].subsystem_name          ,sizeof(ucf01->control[4].subsystem_name));
	checknull(ucf01->control[4].action          ,sizeof(ucf01->control[4].action));
	checknull(ucf01->control[5].subsystem_name          ,sizeof(ucf01->control[5].subsystem_name));
	checknull(ucf01->control[5].action          ,sizeof(ucf01->control[5].action));
	checknull(ucf01->control[6].subsystem_name          ,sizeof(ucf01->control[6].subsystem_name));
	checknull(ucf01->control[6].action          ,sizeof(ucf01->control[6].action));
	checknull(ucf01->control[7].subsystem_name          ,sizeof(ucf01->control[7].subsystem_name));
	checknull(ucf01->control[7].action          ,sizeof(ucf01->control[7].action));
	checknull(ucf01->control[8].subsystem_name          ,sizeof(ucf01->control[8].subsystem_name));
	checknull(ucf01->control[8].action          ,sizeof(ucf01->control[8].action));
	checknull(ucf01->control[9].subsystem_name          ,sizeof(ucf01->control[9].subsystem_name));
	checknull(ucf01->control[9].action          ,sizeof(ucf01->control[9].action));
	checknull(ucf01->control[10].subsystem_name          ,sizeof(ucf01->control[10].subsystem_name));
	checknull(ucf01->control[10].action          ,sizeof(ucf01->control[10].action));
	checknull(ucf01->control[11].subsystem_name          ,sizeof(ucf01->control[11].subsystem_name));
	checknull(ucf01->control[11].action          ,sizeof(ucf01->control[11].action));
	checknull(ucf01->control[12].subsystem_name          ,sizeof(ucf01->control[12].subsystem_name));
	checknull(ucf01->control[12].action          ,sizeof(ucf01->control[12].action));
	checknull(ucf01->control[13].subsystem_name          ,sizeof(ucf01->control[13].subsystem_name));
	checknull(ucf01->control[13].action          ,sizeof(ucf01->control[13].action));
	checknull(ucf01->control[14].subsystem_name          ,sizeof(ucf01->control[14].subsystem_name));
	checknull(ucf01->control[14].action          ,sizeof(ucf01->control[14].action));
	checknull(ucf01->control[15].subsystem_name          ,sizeof(ucf01->control[15].subsystem_name));
	checknull(ucf01->control[15].action          ,sizeof(ucf01->control[15].action));
	checknull(ucf01->control[16].subsystem_name          ,sizeof(ucf01->control[16].subsystem_name));
	checknull(ucf01->control[16].action          ,sizeof(ucf01->control[16].action));
	checknull(ucf01->control[17].subsystem_name          ,sizeof(ucf01->control[17].subsystem_name));
	checknull(ucf01->control[17].action          ,sizeof(ucf01->control[17].action));
	checknull(ucf01->control[18].subsystem_name          ,sizeof(ucf01->control[18].subsystem_name));
	checknull(ucf01->control[18].action          ,sizeof(ucf01->control[18].action));
	checknull(ucf01->control[19].subsystem_name          ,sizeof(ucf01->control[19].subsystem_name));
	checknull(ucf01->control[19].action          ,sizeof(ucf01->control[19].action));

	checknull(ucf01->login[0].terminals          ,sizeof(ucf01->login[0].terminals));
	checknull(ucf01->login[1].terminals          ,sizeof(ucf01->login[1].terminals));
	checknull(ucf01->login[2].terminals          ,sizeof(ucf01->login[2].terminals));
	checknull(ucf01->login[3].terminals          ,sizeof(ucf01->login[3].terminals));
	checknull(ucf01->login[4].terminals          ,sizeof(ucf01->login[4].terminals));
	checknull(ucf01->login[5].terminals          ,sizeof(ucf01->login[5].terminals));
	checknull(ucf01->login[6].terminals          ,sizeof(ucf01->login[6].terminals));
	checknull(ucf01->login[7].terminals          ,sizeof(ucf01->login[7].terminals));
	checknull(ucf01->login[8].terminals          ,sizeof(ucf01->login[8].terminals));
	checknull(ucf01->login[9].terminals          ,sizeof(ucf01->login[9].terminals));

	checknull(ucf01->password_1          ,sizeof(ucf01->password_1));
	checknull(ucf01->password_2          ,sizeof(ucf01->password_2));
	checknull(ucf01->password_3          ,sizeof(ucf01->password_3));
	checknull(ucf01->password_4          ,sizeof(ucf01->password_4));

	checknull(ucf01->accountstatus          ,sizeof(ucf01->accountstatus));
	checknull(ucf01->password_modified_date          ,sizeof(ucf01->password_modified_date));

	checknull(ucf01->user_info_1          ,sizeof(ucf01->user_info_1));
	checknull(ucf01->user_info_2          ,sizeof(ucf01->user_info_2));
	checknull(ucf01->user_info_3          ,sizeof(ucf01->user_info_3));
	checknull(ucf01->user_info_4          ,sizeof(ucf01->user_info_4));
	checknull(ucf01->user_info_5          ,sizeof(ucf01->user_info_5));
	checknull(ucf01->user_info_6          ,sizeof(ucf01->user_info_6));
	checknull(ucf01->user_info_7          ,sizeof(ucf01->user_info_7));


	checknull(ucf01->ekey          ,sizeof(ucf01->ekey));
	checknull(ucf01->dekey          ,sizeof(ucf01->dekey));
	checknull(ucf01->buffer          ,sizeof(ucf01->buffer));

	checknull(ucf01->Login_WorkStationId          ,sizeof(ucf01->Login_WorkStationId));
	checknull(ucf01->Login_Status          ,sizeof(ucf01->Login_Status));
	checknull(ucf01->User_Status          ,sizeof(ucf01->User_Status));
	checknull(ucf01->Days_Inactive          ,sizeof(ucf01->Days_Inactive));
	checknull(ucf01->Login_Date          ,sizeof(ucf01->Login_Date));
	checknull(ucf01->User_Expiry_Date          ,sizeof(ucf01->User_Expiry_Date));
	checknull(ucf01->Date_Of_Deactivation          ,sizeof(ucf01->Date_Of_Deactivation));

	checknull(ucf01->password_5          ,sizeof(ucf01->password_5));
	checknull(ucf01->password_6          ,sizeof(ucf01->password_6));
	checknull(ucf01->password_7          ,sizeof(ucf01->password_7));
	checknull(ucf01->password_8          ,sizeof(ucf01->password_8));
	checknull(ucf01->password_9          ,sizeof(ucf01->password_9));
	checknull(ucf01->password_10          ,sizeof(ucf01->password_10));
	checknull(ucf01->Logout_Date          ,sizeof(ucf01->Logout_Date));

}

void checknull(pCHAR data , INT Len)
{
	int len = 0;
	char DATA[100]="";
	strncpy(DATA,data,sizeof(DATA));
	while ( *data++ !='\0')
	{
		len ++;
	}
	if(len > Len -1)
	{
		printf("Erroe in data %s\n",DATA);
	}

}*/

/*******************************************************
 * This function will get the values from tf.ini file
 * from the section DATASERVER with keyname DB_ERROR_STATICS_FLAG &
 *	DB_ERROR_STATICS_VALUE which will help to decide the db statics
 *	logging mechanism.
 * *******************************************************/

INT get_dataserver_ini_db_error_statics_value(pINT  ds_timer_flag,
                                   double *  ds_timer_value,
                                   pCHAR appname, pCHAR errbuf )
{
   #define sON  "1"
   #define sOFF "0"
   #define sDEFAULT_DS_ERROR_STATICS_VALUE  "0.5"

   DWORD rc;
   CHAR  filename   [80];
   CHAR  tmpstr     [80];
   CHAR  timer_flag [5] = {0};
   CHAR  timer_value[5] = {0};
   INT   ret_val = true;
   CHAR db_nullchk_flag[2] = {0};
   CHAR  trands_error_warning_file_size[5] = {0};

   /* Get path to the tf.ini file. */
   memset( tmpstr, 0x00, sizeof(tmpstr) );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, "tf.ini" );

   /*----------------------------------------*/
   /* Get Data Server error statics  Flag - On or Off */
   /*----------------------------------------*/
   /*   memcpy( timer_flag, sOFF, sizeof(sOFF) );
      rc = GetPrivateProfileString(
               "DATASERVERS",        // points to section name
               "DB_STATICS_FLAG",       // points to key name
                sOFF,                //Default string (Off)
                timer_flag,          //points to destination buffer
                sizeof(timer_flag)-1,//size of destination buffer
                filename             // points to ini filename
      );

       if ( 0 == strcmp(timer_flag, sOFF) )
      {

     	   *ds_timer_flag = DS_TIMER_OFF;
      }
      else if ( 0 == strcmp(timer_flag, sON) )
      {

         *ds_timer_flag = DS_TIMER_ON;*/
      memcpy( timer_value,
              sDEFAULT_DS_ERROR_STATICS_VALUE,
              sizeof(sDEFAULT_DS_ERROR_STATICS_VALUE) );

      rc = GetPrivateProfileString(
               "DATASERVERS",             /* points to section name         */
               "DB_STATICS_VALUE",       /* points to key name             */
                sDEFAULT_DS_ERROR_STATICS_VALUE,  /* Default string                 */
                timer_value,              /* points to destination buffer   */
                sizeof(timer_value)-1,    /* size of destination buffer     */
                filename                  /* points to ini filename         */
      );

      *ds_timer_value = atof(timer_value);
      if ( (*ds_timer_value < 0) || (*ds_timer_value > 999) )
      {
         sprintf( errbuf,
                 "%s:Invalid value DB error statics Value (%s) in tf.ini. Use 0 to 999",
                  appname, timer_value );
         *ds_timer_flag = DS_TIMER_OFF;
         ret_val = false;
      }
      /* }
       else
      {
      	// The tf.ini file contains an invalid value for the timer flag.
           // Log this fact, set timer off, and exit.

          sprintf( errbuf,
                 "%s:Invalid value (%s) in tf.ini file for Data Server DB_ERROR_STATICS_FLAG flag.",
                 appname, timer_flag );
          *ds_timer_flag = DS_TIMER_OFF;
          ret_val = false;
       }*/

         /*******************To check for NULL in every structure member************************/
          /*----------------------------------------*/
             /* Get Data Server error statics  Flag - On or Off */
             /*----------------------------------------*/
         /*    memcpy( db_nullchk_flag, 0x00, sizeof(db_nullchk_flag) );
             rc = GetPrivateProfileString(
                      "DBNULLCHECK",         		//points to section name
                      "DB_NULL_CHECK_FLAG",       //points to key name
                       "0",                 		//Default string (Off)
      				  db_nullchk_flag,          //points to destination buffer
                       sizeof(db_nullchk_flag)-1, //size of destination buffer
                       filename              		//points to ini filename
             );
             DB_NullCheck_Flag = atoi(db_nullchk_flag); */



   /*Read path for creating file in order to log db statics and db oracle error messages */
   rc = GetPrivateProfileString(
                  "DATASERVERS",             /* points to section name         */
                  "DB_STATICS_PATH",       	/* points to key name             */
                   "",  					/* Default string                 */
				   DB_file_path,              	/* points to destination buffer   */
                   sizeof(DB_file_path)-1,   	 /* size of destination buffer     */
                   filename                  /* points to ini filename         */
         );

   rc = GetPrivateProfileString(
                      "ERROR_WARNING",             /* points to section name         */
					  appname,       	/* points to key name             */
                      "",  					/* Default string                 */
					  DB_module_file_name,              	/* points to destination buffer   */
                      sizeof(DB_module_file_name)-1,   	 /* size of destination buffer     */
                      filename                  /* points to ini filename         */
            );

   rc = GetPrivateProfileString(
							  "ERROR_WARNING",             /* points to section name         */
							  "ERROR_WARNING_FILE_SIZE",       	/* points to key name             */
							  "500",  					/* Default string                 */
							  trands_error_warning_file_size,              	/* points to destination buffer   */
							  sizeof(trands_error_warning_file_size)-1,   	 /* size of destination buffer     */
							  filename                  /* points to ini filename         */
							);

	/* File size size conversion */
	Max_File_Size_Defined = atoi(trands_error_warning_file_size);
	if(Max_File_Size_Defined <= 0)
	{
		Max_File_Size_Defined = 500 ;
	}
	Max_File_Size_Defined = Max_File_Size_Defined * ONE_MB_IN_BYTES ; /* 1 MB = 1048576 BYTES*/


   if((strlen(DB_file_path) > 0) &&
	  (strlen(DB_module_file_name)> 0))
   {
	   Create_Db_Statics_And_Db_Oracle_Error_Filename();
   }

   return( ret_val );
}


void Create_Db_Statics_And_Db_Oracle_Error_Filename()
{
	CHAR  system_date[25] = "";

   /* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
	ptetime_get_timestamp( system_date );

    strcpy( DB_Logging_Filename, DB_file_path );
    strcat( DB_Logging_Filename, DB_module_file_name );
	strcat( DB_Logging_Filename, ".EW." );
    strncat(DB_Logging_Filename, system_date,   4 );  /* YYYY */
    strncat(DB_Logging_Filename, system_date+5, 2 );  /* MM   */
    strncat(DB_Logging_Filename, system_date+8, 2 );  /* DD   */

    return;
}

void Log_Db_Statics_And_Oracle_Db_Error_To_File(pCHAR Error_Buf)
{
	INT   len=0;
	CHAR  time_date[25]  ={0};
	CHAR  timestamp[14]  ={0};
	CHAR  current_mmdd[5]={0};
	CHAR path[256]={0};
	CHAR Buffer[1024]={0};
	int path_len=0;
	FILE *fp;
	#define NUM_SIZE_WRITES  1
	/* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
	ptetime_get_timestamp( time_date );
	strcpy(Buffer,time_date);
   /* Pick out the month and day to compare to filename extenstion. */
    memcpy( current_mmdd,   time_date+5, 2 );
    memcpy( current_mmdd+2, time_date+8, 2 );

	/* Compare filename extension to current date. */
    len = strlen(DB_Logging_Filename);
    path_len =strlen(DB_file_path);
    if(len==0 || path_len==0)
    {
    	LogEvent(Error_Buf,INFO_MSG);
    	return;
    }
    if ( 0 != strcmp(&DB_Logging_Filename[len-4], current_mmdd) )
    {

			/* Now create the new filename.
          * ----------------------------
          */
			Create_Db_Statics_And_Db_Oracle_Error_Filename();

	}
	if((fp = fopen(DB_Logging_Filename,"a+b"))==NULL)
	{
		LogEvent(Error_Buf,INFO_MSG);
		return;
	}

	strcat(Buffer,":");
	strcat(Buffer, ServiceName );
	strcat(Buffer,":");
	strcat(Buffer,Error_Buf);
	strcat(Buffer,"\n");
	len=0;
	len=strlen(Buffer);


	if(fwrite(Buffer, len, NUM_SIZE_WRITES, fp)==NUM_SIZE_WRITES)
	{
		// fal below
	}
	else
	{
		LogEvent(Buffer,INFO_MSG);
	}

	fclose(fp);
	return ;
}

/******************************************************************************
 *
 *  NAME:         trands_log_message
 *
 *  DESCRIPTION:  This function takes a string and input parameters and
 *                formats a message to be sent to either Monitor, Event Log
 *                or both.  Based on inputs, it sets the severity.
 *
 *  INPUTS:       dest - 1 = Monitor, 2 = Event Log, 3 = Both
 *                sev  - 1 = Info,    2 = Warning,   3 = Error
 *                msg  - Text message to be logged
 *                func - Name of function logging the error
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Raghavendra
 *
 ******************************************************************************/
void trands_log_message( INT dest, INT sev, pCHAR msg, pCHAR func)
{
   INT  monitor_flag;
   INT  eventlog_flag;
   INT  msg_type;
   BYTE severity;
   BYTE log_type[20]={0};
   CHAR text_message[512]={0};
   AUTH_TX Auth_Tx;

   /* Set Monitor flag */
   if ( dest == 1  ||  dest == 3 )
      monitor_flag = 1;
   else
      monitor_flag = 0;

   /* Set Event Log flag */
   /*if ( dest > 1 )
      eventlog_flag = 1;
   else
      eventlog_flag = 0;*/

   /* Set message type and severity */
   if ( sev == 2 )
   {
      msg_type = WARN_MSG;
      severity = '3';
      strcpy( log_type, WARNING_ERROR );
   }
   else if ( sev == 1 )
   {
      msg_type = INFO_MSG;
      severity = '0';
      strcpy( log_type, INFO_ERROR );
   }
   else if ( sev == 3 )
   {
      msg_type = ALERT_MSG;
      severity = '4';
      strcpy( log_type, FATAL_ERROR );
   }

   /* Make sure text message is not too long. */
   memcpy( text_message, msg, sizeof(text_message)-1 );
   trands_Log_error_warning_to_File(text_message,sev,func);
   /* Call function to post the message. */
   if(monitor_flag == 1)
   {

		   TxUtils_Send_Msg_To_Operator( monitor_flag, text_message,
		   										 eventlog_flag, msg_type, func,
		   										 severity, log_type,
		   										 NULL, NULL,NULL );

   }
   return;
}

INT trands_Log_error_warning_to_File(pCHAR Error_Buf,int sev,pCHAR func)
{
	INT   len=0;
	INT   path_len = 0;
	CHAR  time_date[25]  ={0};
	CHAR  current_mmdd[5]={0};
	CHAR Buffer[1024]={0};
	CHAR  card_tid_mid_details [256] = {0};
	INT   cardlen = 0;
	INT   nIndex = 0;
	CHAR  tempcard[21] = {0};
	FILE *fp;
	UINT file_size = 0 ;
	char buf[256] = {0} ;

	/* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
	ptetime_get_timestamp( time_date );

	/* Pick out the month and day to compare to filename extenstion. */
    memcpy( current_mmdd,   time_date+5, 2 );
    memcpy( current_mmdd+2, time_date+8, 2 );

	/* Compare filename extension to current date. */
    len = strlen(DB_Logging_Filename);
    path_len = strlen(DB_file_path);
    if( len == 0 ||
    	path_len==0 )
    {
    	LogEvent(Error_Buf,INFO_MSG);
    	return 0;
    }
    if ( 0 != strcmp(&DB_Logging_Filename[len-4], current_mmdd) )
    {
		/* Now create the new filename.*/
    	Create_Db_Statics_And_Db_Oracle_Error_Filename();
    }
	if((fp = fopen(DB_Logging_Filename,"a+b"))==NULL)
	{
		LogEvent(Error_Buf,INFO_MSG);
		return 0;
	}
	strcpy(Buffer,time_date);
	strcat(Buffer,":");
	if(sev == 1)
	{
		strcat(Buffer," INFO");
	}
	else if (sev == 2)
	{
		strcat(Buffer," WARN");
	}
	else
	{
		strcat(Buffer," ERROR");
	}
	strcat(Buffer,": ");
	strcat(Buffer,Error_Buf);
	strcat(Buffer," ");
	if (strlen(auth_tx.TLF01_details.card_num)> 0)
		{
			/* Masking card number first 5 and last 4 in clear form remaining all masked. */
			cardlen = strlen( auth_tx.TLF01_details.card_num );
			strncpy( tempcard, auth_tx.TLF01_details.card_num, cardlen);
			for ( nIndex = cardlen-5; nIndex >= cardlen-11; nIndex-- )
			{
				tempcard[nIndex] = '*';
			}
			sprintf(card_tid_mid_details,"CNBR %s TID %s MID %s STAN %s RRN %s",
					tempcard,
					auth_tx.TLF01_details.terminal_id,
					auth_tx.TLF01_details.merchant_id,
					auth_tx.TLF01_details.sys_trace_audit_num,
					auth_tx.TLF01_details.retrieval_ref_num);
		}
		else
		{
			sprintf(card_tid_mid_details,"TID %s MID %s STAN %s RRN %s",
					auth_tx.TLF01_details.terminal_id,
					auth_tx.TLF01_details.merchant_id,
					auth_tx.TLF01_details.sys_trace_audit_num,
					auth_tx.TLF01_details.retrieval_ref_num);
		}
		strcat(Buffer,card_tid_mid_details);
		strcat(Buffer," ");
	strcat(Buffer, func);
	strcat(Buffer,"\n");
	len=strlen(Buffer);

	if(fwrite(Buffer, len, NUM_SIZE_WRITES, fp)==NUM_SIZE_WRITES)
	{
		// Do nothing fall below
	}
	else
	{
		LogEvent(Buffer,INFO_MSG);
		fclose(fp);
		return  0;
	}
	fseek (fp, 0, SEEK_END);
	file_size=ftell (fp);

	if( file_size >= Max_File_Size_Defined )
	{
		sprintf(buf,"ERROR_WARNING_FILE  size is exceeding the permissible size, need attention" );
		TxUtils_Send_Msg_To_Operator( 1, buf,1, ALERT_MSG, "", 4, INFO_ERROR,NULL,NULL,NULL );
	}
	fclose(fp);
	return  0;
}

void Create_userstatuscontrol_tool_report()
{
	char date[25]={0};
	CHAR ascendent_path[256]={0};
	CHAR file_date[20] = {0};
	DWORD rc;
	char filename[256] = {0};
	char tmpstr[256]   = {0};

   /* Get path from the tf.ini file. */
   memset( tmpstr, 0x00, sizeof(tmpstr) );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, "tf.ini" );

	memset(User_status_Control_Report_file_name,0x00,sizeof(User_status_Control_Report_file_name));
	rc = GetPrivateProfileString(
	               "USERSTATUSCONTROL",             /* points to section name         */
	               "USERSTATUSCONTROLFILEPATH",       /* points to key name             */
	                "",  /* Default string                 */
					User_status_Control_Report_file_name,              /* points to destination buffer   */
	                sizeof(User_status_Control_Report_file_name)-1,    /* size of destination buffer     */
	                filename                  /* points to ini filename         */
	      );

	strcat(User_status_Control_Report_file_name,"UserStatusControlFile.");
	ptetime_get_timestamp(date);
	strncat(User_status_Control_Report_file_name, date,   4 );  /* YYYY */
    strncat(User_status_Control_Report_file_name, date+5, 2 );  /* MM   */
    strncat(User_status_Control_Report_file_name, date+8, 2 );  /* DD   */
}

/* Do not remove the following lines. These are the Custom build settings for the Pro C pre-compiler */
/*proc $(ProjDir)\$(InputName).pc dbms=native sqlcheck=semantics userid=atp/atp@atp include=$(ProjDir)\..\..\include*/
/*$(ProjDir)\$(InputName).c*/

void Mask_Fields(char * fields, char * masked_fields)
{
	int fieldslen = 0;
	int nIndex 	= 0;
	memset(masked_fields ,0x00, sizeof(masked_fields));
	fieldslen = strlen( fields );
	if (fieldslen > 0)
	{
		/* Masking card number first 5 and last 4 in clear form remaining all masked. */
		strncpy( masked_fields, fields, fieldslen);
		for ( nIndex = fieldslen-5; nIndex >= fieldslen-11; nIndex-- )
		{
			masked_fields[nIndex] = '*';
		}
	}
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

BOOLEAN OpenDSV_WriteHST01(HST01 *hst01)
{

	char masked_field_Previous[151]={0};
	char masked_field_Current[151]={0};
	CHAR  time_date[25] = "";
	CHAR  current_mmdd[5]={0};
	INT   len=0;
	char buffer[1024]={0};


	ptetime_get_timestamp( time_date );

    memcpy( current_mmdd,   time_date+5, 2 );
    memcpy( current_mmdd+2, time_date+8, 2 );

    len = strlen(HST01_Filename);
    if ( 0 != strncmp(&HST01_Filename[len-8], current_mmdd,4) )
    {

    	trands_create_Error_Filename();
 	   sprintf(buffer,"the file name is: %s",HST01_Filename);
 	   trands_log_message(2,1,buffer,"OpenDSV_WriteHST01 ");
    }

	    HstFilePtr = fopen(HST01_Filename,"a");
		if(HstFilePtr != NULL)
		{
			long int File_Size = findHST01FileSize(HST01_Filename);
			if(File_Size >= HEADER_SIZE)
			{


				WriteString(&hst01->primary_key.unique_id, sizeof(hst01->primary_key.unique_id), FILLERCHAR, SEPERATORCHAR, &HstFilePtr, "unique_id");
				WriteString(&hst01->file_name, sizeof(hst01->file_name), FILLERCHAR, SEPERATORCHAR, &HstFilePtr, "file_name");
				WriteString(&hst01->file_primary_key, sizeof(hst01->file_primary_key), FILLERCHAR, SEPERATORCHAR, &HstFilePtr, "file_primary_key");
				WriteString(&hst01->changed_by, sizeof(hst01->changed_by), FILLERCHAR, SEPERATORCHAR, &HstFilePtr, "changed_by");
				WriteString(&hst01->date_modified, sizeof(hst01->date_modified), FILLERCHAR, SEPERATORCHAR, &HstFilePtr, "date_modified");
				WriteString(&hst01->time_modified, sizeof(hst01->time_modified), FILLERCHAR, SEPERATORCHAR, &HstFilePtr, "time_modified");
				WriteString(&hst01->field_tag, sizeof(hst01->field_tag), FILLERCHAR, SEPERATORCHAR, &HstFilePtr, "field_tag");
				Mask_sensitive_fields(hst01,masked_field_Previous,masked_field_Current);
				if(masked_field_Previous[0]!='\0' || masked_field_Current[0]!='\0')
				{
					WriteString(&masked_field_Previous, sizeof(masked_field_Previous), FILLERCHAR, SEPERATORCHAR, &HstFilePtr, "previous_data");
					WriteString(&masked_field_Current, sizeof(masked_field_Current), FILLERCHAR, SEPERATORCHAR, &HstFilePtr, "current_data");
				}
				else
				{
					WriteString(&hst01->previous_data, sizeof(hst01->previous_data), FILLERCHAR, SEPERATORCHAR, &HstFilePtr, "previous_data");
					WriteString(&hst01->current_data, sizeof(hst01->current_data), FILLERCHAR, SEPERATORCHAR, &HstFilePtr, "current_data");

				}
				WriteString(&hst01->reserved, sizeof(hst01->reserved), FILLERCHAR, SEPERATORCHAR, &HstFilePtr, "reserved");

				fprintf(HstFilePtr,"\n");
				fclose(HstFilePtr);

			}
			else
			{
				//The header is not present. Create the header.
				WriteDSVHeader();

				fclose(HstFilePtr);


			}
		}
		else
		{

	 	    	sprintf(buffer,"Cannot open trands log file for writing data: %s",HST01_Filename);
	 	    	trands_log_message( 2, 1, buffer, "OpenDSV_WriteHST01");
		}

	return TRUE;


}

void WriteDSVHeader()
{
	HST01 hst01;


	if(HstFilePtr != NULL)
	{
		WriteString("unique_id", sizeof(hst01.primary_key.unique_id), FILLERCHAR, SEPERATORCHAR, &HstFilePtr, "unique_id");					//1
		WriteString("file_name",sizeof(hst01.file_name), FILLERCHAR, SEPERATORCHAR, &HstFilePtr, "file_name");							//2
		WriteString("file_primary_key", sizeof(hst01.file_primary_key), FILLERCHAR, SEPERATORCHAR, &HstFilePtr, "file_primary_key");
		WriteString("changed_by", sizeof(hst01.changed_by), FILLERCHAR, SEPERATORCHAR, &HstFilePtr, "changed_by");	//5
		WriteString("date_modified", 13, FILLERCHAR, SEPERATORCHAR, &HstFilePtr, "date_modified");						//7
		WriteString("time_modified", 13, FILLERCHAR, SEPERATORCHAR, &HstFilePtr, "time_modified");						//9						//10
		WriteString("field_tag", sizeof(hst01.field_tag), FILLERCHAR, SEPERATORCHAR, &HstFilePtr, "field_tag");							//11
		WriteString("previous_data", sizeof(hst01.previous_data), FILLERCHAR, SEPERATORCHAR, &HstFilePtr, "previous_data");							//12
		WriteString("current_data", sizeof(hst01.current_data), FILLERCHAR, SEPERATORCHAR, &HstFilePtr, "current_data");						//13
		WriteString("reserved", sizeof(hst01.reserved), FILLERCHAR, SEPERATORCHAR, &HstFilePtr, "reserved");
		fprintf(HstFilePtr, "\n");//16
	}





}

void trim_spaces( pBYTE pString )
{

   INT len, position=0;

   len = strlen(pString) - 1;
   for(position = len; position >= 0; position--)
   {
      if(pString[position] == ' ')
         pString[position] = 0;
      else
         break;
   }
}


BOOLEAN WriteString(char *str, int size, char fillerChar, char seperator, FILE** hstptr, char *header)
{
	char space[1024] = {0};
	char filler[1024] = {0};
	char line[1024] = {0};
	char buffer[1024] = {0};
	int n = size;
	register int index=0;
	int header_len = 0;
	int data_len = 0;
	char data[512] = {0};
	char final_str[512]={0};

	trim_spaces(str);
	data_len = strlen(str);
	header_len = strlen(header);
	if(data_len< 512)
	{
		strncpy(data,str,data_len);
	}
	else
	{
		sprintf(buffer,"Unexpected length:%d",data_len);
		trands_log_message(3,3,buffer,"WriteString");
		strncpy(data,str,512);
	}

	/*for(index=0;index<data_len;index++)
	{
		if(data[index] != '\0')
		{
			final_str[index] = data[index];
		}
		else
		{
			final_str[index] ='\0';
			printf("Null present in between the data:%s\n",data);
			break;
		}
	}

	data_len = strlen(final_str);*/

	//if(header != NULL)
	if(header_len != 0)
	{
		if(header_len > size)
	{
			size = header_len;
		}
	}

	memset(space, fillerChar, sizeof(space));


	if(data_len> size)
	{
		//memcpy(filler, space, size );
		/* size -1 to make the data size equal to header*/
		final_str[size-1] = '\0';
	}
	else
	{
		memcpy(filler, space, (size - data_len));
	}

	if(seperator != 0x00)
	{
		if(data_len> size)
		{
			sprintf(line, "%s%c", data,  seperator);
		}
		else
		{
			sprintf(line, "%s%s%c", data, filler, seperator);
		}

	}
	else
	{
		sprintf(line, "%s%s", data, filler);
	}

	if(hstptr != NULL && *hstptr != NULL)
	{
		fprint_retval = fprintf(*hstptr, "%s", line);

	}
	else
	{
		printf("%s\n",line);
	}

	if(fprint_retval < 0)
	{
		sprintf( buffer,"File write failed, ret value= %d,  Field: %s -> %s",fprint_retval,header, str);
		trands_log_message(3,3,buffer,"WriteString");
		fprint_retval = 0;
	}
	return TRUE;
}


long int findHST01FileSize(char file_name[])
{
    // opening the file in read mode
    FILE* fp = fopen(file_name, "r");

    // checking if the file exist or not
    if (fp == NULL) {
        printf("File Not Found!\n");
        return -1;
    }

    fseek(fp, 0L, SEEK_END);

    // calculating the size of the file
    long int File_Size = ftell(fp);

    // closing the file
    fclose(fp);

    return File_Size;
}

void trands_create_Error_Filename()
{
	CHAR  system_date[25] = {0};

/* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
	ptetime_get_timestamp( system_date );

	strcpy(HST01_Filename,trands_hst01_file_path);
	strcat( HST01_Filename, "HST01" );
	strcat( HST01_Filename, "_" );
	strncat(HST01_Filename, system_date,   4 );  /* YYYY */
	strncat(HST01_Filename, system_date+5, 2 );  /* MM   */
	strncat(HST01_Filename, system_date+8, 2 );  /* DD   */
	strcat(HST01_Filename, ".txt");  /* DD   */

}

void get_HST01_file_name_path(void )
{
   DWORD rc;
   CHAR  filename   [80] = {0};
   CHAR  tmpstr     [80] = {0};
   CHAR  appname[512] = {0};
   char buffer[1024] = {0};

   /* Get path to the tf.ini file. */
   memset( tmpstr, 0x00, sizeof(tmpstr) );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, "tf.ini" );

   GetAppName (appname) ;

   /*Read path for creating file in order to log db statics and db oracle error messages */
   rc = GetPrivateProfileString(
								  "HST01",             /* points to section name         */
								  "HST01_LOGGING_PATH",       	/* points to key name             */
								   "",  					/* Default string                 */
								   trands_hst01_file_path,              	/* points to destination buffer   */
								   sizeof(trands_hst01_file_path)-1,   	 /* size of destination buffer     */
								   filename                  /* points to ini filename         */
						 	   );

   if((strlen(trands_hst01_file_path) > 0))
   {
	   trands_create_Error_Filename();
	   sprintf(buffer,"the file name is: %s",HST01_Filename);
	   trands_log_message(2,1,buffer,"get_HST01_file_name_path ");
   }
}

void Mask_sensitive_fields(HST01 *hst01, CHAR * masked_field_Previous, CHAR *masked_field_Current)
{
	if(0==strncmp(hst01->file_name,"PAF01",5))
			{
				if(0==strncmp(hst01->field_tag,"Name",4))
				{
					if(hst01->previous_data[0]!='\0')
					{

						Mask_Fields(hst01->previous_data,masked_field_Previous);

					}
					if(hst01->current_data[0]!='\0')
					{

						Mask_Fields(hst01->current_data,masked_field_Current);
					}

				}
				else if(0==strncmp(hst01->field_tag,"Account Name",12))
				{
					if(hst01->previous_data[0]!='\0')
					{

						Mask_Fields(hst01->previous_data,masked_field_Previous);

					}
					if(hst01->current_data[0]!='\0')
					{

						Mask_Fields(hst01->current_data,masked_field_Current);
					}

				}
			}
			if(0==strncmp(hst01->file_name,"CCF02P",6))
			{
				if(0==strncmp(hst01->field_tag,"Bal.Acct.Nbr",4))
				{
					if(hst01->previous_data[0]!='\0')
					{

						Mask_Fields(hst01->previous_data,masked_field_Previous);

					}
					if(hst01->current_data[0]!='\0')
					{

						Mask_Fields(hst01->current_data,masked_field_Current);
					}

				}
				if(0==strncmp(hst01->field_tag,"Primary Acc Nbr",15))
				{
					if(hst01->previous_data[0]!='\0')
					{

						Mask_Fields(hst01->previous_data,masked_field_Previous);

					}
					if(hst01->current_data[0]!='\0')
					{

						Mask_Fields(hst01->current_data,masked_field_Current);
					}

				}
				else if(0==strncmp(hst01->field_tag,"Name",4))
				{
					if(hst01->previous_data[0]!='\0')
					{

						Mask_Fields(hst01->previous_data,masked_field_Previous);

					}
					if(hst01->current_data[0]!='\0')
					{

						Mask_Fields(hst01->current_data,masked_field_Current);
					}

				}
			}
			if(0==strncmp(hst01->file_name,"NCF01",5))
			{
				if(0==strncmp(hst01->field_tag,"ZMK",3))
				{
					if(hst01->previous_data[0]!='\0')
					{

						Mask_Fields(hst01->previous_data,masked_field_Previous);

					}
					if(hst01->current_data[0]!='\0')
					{

						Mask_Fields(hst01->current_data,masked_field_Current);
					}

				}
				else if(0==strncmp(hst01->field_tag,"CWK",3))
				{
					if(hst01->previous_data[0]!='\0')
					{

						Mask_Fields(hst01->previous_data,masked_field_Previous);

					}
					if(hst01->current_data[0]!='\0')
					{

						Mask_Fields(hst01->current_data,masked_field_Current);
					}

				}
				else if(0==strncmp(hst01->field_tag,"KEK",3))
				{
					if(hst01->previous_data[0]!='\0')
					{

						Mask_Fields(hst01->previous_data,masked_field_Previous);

					}
					if(hst01->current_data[0]!='\0')
					{

						Mask_Fields(hst01->current_data,masked_field_Current);
					}

				}
				else if(0==strncmp(hst01->field_tag,"Name",4))
				{
					if(hst01->previous_data[0]!='\0')
					{

						Mask_Fields(hst01->previous_data,masked_field_Previous);

					}
					if(hst01->current_data[0]!='\0')
					{

						Mask_Fields(hst01->current_data,masked_field_Current);
					}

				}
			}
			if(0==strncmp(hst01->file_name,"CAF01",5))
			{

				if(0==strncmp(hst01->field_tag,"MKAC",4)) 
				{
					if(hst01->previous_data[0]!='\0')
					{

						Mask_Fields(hst01->previous_data,masked_field_Previous);

					}
					if(hst01->current_data[0]!='\0')
					{

						Mask_Fields(hst01->current_data,masked_field_Current);
					}

				}
				else if(0==strncmp(hst01->field_tag,"3D_CAK",5)) 
				{
					if(hst01->previous_data[0]!='\0')
					{

						Mask_Fields(hst01->previous_data,masked_field_Previous);

					}
					if(hst01->current_data[0]!='\0')
					{

						Mask_Fields(hst01->current_data,masked_field_Current);
					}

				}
				else if(0==strncmp(hst01->field_tag,"3DS-2_Code",7)) 
				{
					if(hst01->previous_data[0]!='\0')
					{

						Mask_Fields(hst01->previous_data,masked_field_Previous);

					}
					if(hst01->current_data[0]!='\0')
					{

						Mask_Fields(hst01->current_data,masked_field_Current);
					}

				}
				else if(0==strncmp(hst01->field_tag,"MC_CAK",6))
				{
					if(hst01->previous_data[0]!='\0')
					{

						Mask_Fields(hst01->previous_data,masked_field_Previous);

					}
					if(hst01->current_data[0]!='\0')
					{

						Mask_Fields(hst01->current_data,masked_field_Current);
					}

				}
				else if(0==strncmp(hst01->field_tag,"Extd 3ds2.0 Key",14)) 
				{
					if(hst01->previous_data[0]!='\0')
					{

						Mask_Fields(hst01->previous_data,masked_field_Previous);

					}
					if(hst01->current_data[0]!='\0')
					{

						Mask_Fields(hst01->current_data,masked_field_Current);
					}

				}
				else if(0==strncmp(hst01->field_tag,"CVK A",5))
				{
					if(hst01->previous_data[0]!='\0')
					{

						Mask_Fields(hst01->previous_data,masked_field_Previous);

					}
					if(hst01->current_data[0]!='\0')
					{

						Mask_Fields(hst01->current_data,masked_field_Current);
					}

				}
				else if(0==strncmp(hst01->field_tag,"CVK B",5))
				{
					if(hst01->previous_data[0]!='\0')
					{

						Mask_Fields(hst01->previous_data,masked_field_Previous);

					}
					if(hst01->current_data[0]!='\0')
					{

						Mask_Fields(hst01->current_data,masked_field_Current);
					}

				}
				else if(0==strncmp(hst01->field_tag,"Name",4))
				{
					if(hst01->previous_data[0]!='\0')
					{

						Mask_Fields(hst01->previous_data,masked_field_Previous);

					}
					if(hst01->current_data[0]!='\0')
					{

						Mask_Fields(hst01->current_data,masked_field_Current);
					}

				}
			}
}


/******************************************************************************
 *
 *  NAME:         Read_Max_BIN01_Table_Size_For_XCOM_Memory
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
INT Read_Max_BIN01_Table_Size_For_XCOM_Memory( void )
{
   DWORD rc = 0;
   CHAR filename     [MAX_FILENAME_SIZE] = {0};
   CHAR tmpstr       [MAX_FILENAME_SIZE] = {0};
   CHAR tmpstring    [MAX_INI_INFO_SIZE] = {0};
   CHAR ini_info     [MAX_INI_INFO_SIZE] = {0};
   CHAR buffer		 [200] = {0};
   INT  ret_val = true;
   INT bin01_count=0;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
   rc = GetPrivateProfileString(
             "XCOM_MEMORY_TABLE_SIZE",    /* points to section name       */
             "BIN01_COUNT",  /* points to key name           */
             "0",                       /* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   strncpy( Bin01_Tablle_Size,tmpstring, SHM_TABLE_SIZE_LEN );
   bin01_count = atoi(Bin01_Tablle_Size);
   if(0 == bin01_count)
   {
	   strncpy( Bin01_Tablle_Size,
			    DEFAULT_BIN01_TABLE_SIZE,
				SHM_TABLE_SIZE_LEN);
   }
   bin01_count = atoi(Bin01_Tablle_Size);
   sprintf(buffer,"Bin table size in SHM MEM is:%d",bin01_count);
   trands_log_message(2,1,buffer,"Read_Max_BIN01_Table_Size_For_XCOM_Memory ");;
   return( ret_val );
}
