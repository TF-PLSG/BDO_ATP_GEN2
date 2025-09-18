/*******************************************************************************
*  
* MODULE:           sbatch.c
*
* Copyright (c) 2008, ThoughtFocus Technologies, Inc. All Rights Reserved.   
*  
* TITLE:             
*  
* DESCRIPTION:        
*
* APPLICATION:      sbatch.exe
*
* AUTHOR:           Ravikumar Nageshaiah, ThoughtFocus
*
* REVISION HISTORY:
*
* $Log:   SVN\EPICPORTZ\Equitable\sbatch.c  $
*  
*  
		Rev 1.9  Dec 23 2009   TF-Ajay 
	Changed variable declration place from .H file to .C file 
	Updated version 1.0.0.10
	
	  Rev 1.8  Dec 17 2009   TF-Ajay 
	Changed Sbatch to retreive txn from TLF01_STIP table based on txn date with network controller name
	Now second parameter is date
	If date not provided by the user. Sbatch will read system data by default
	Updated version 1.0.0.9
	
	  
		Rev 1.7  Aug 31 2009   TF-Ajay 
	It should not change for other brands except VISA
	In the 0120 message field 4 and field 49 is in peso. 
	The value should be in field 4 and field 6 should be as received from VISA, in this case the 3rd currency value.
	Updated version 1.0.0.8
	
	  Rev 1.6   Aug 27 2009   TF-Ajay 
	In the 0120 message field 4 and field 49 is in peso. 
	The value should be in field 4 and field 6 should be as received from VISA, in this case the 3rd currency value.
	Updated version 1.0.0.7
	
	Rev 1.5   Aug 03 2009   TF-Ajay 
	If originated from the DCPISO or Voice, no need send fld 6 & 51 
	Updated version 1.0.0.6
	
	 Rev 1.4   Jul 28 2009   TF-Ajay 
	For Amex, Advice repeat send as a 0120 only 
	Updated version 1.0.0.5


	Rev 1.3   Jul 22 2009   TF-Ajay 
	If txn  gets non-approval response code of more than three times. That txn make it is processed
	So That txn no more  send to host 
	Updated version 1.0.0.4


	Rev 1.2   Jul 13 2009   TF-Ajay 
	Txn process as a STIP and After process update ncf30/bch20/bch10 table
	Updated version 1.0.0.3

	Rev 1.1   Jun 03 2009 Ajay - ThoughtFocus
	Change the code to update BCH10 table also. when it  process the txns
	Updated version 1.0.0.2

	Rev 1.0   Jun 26 2008 Ravikumar Nageshaiah - ThoughtFocus
	initial release

*******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <memory.h>
#include <math.h>
#include "basictyp.h"
#include "pte.h"
#include "ptemsg.h"
#include "pteipc.h"
#include "ptestats.h"
#include "ptesystm.h"
#include "ptetime.h"
#include "ntutils.h"
#include "app_info.h"
#include "equitdb.h"
#include "dc_dbstruct.h"
#include "sbatch.h"
#include "global_definitions.h"
#include "txutils.h"
#include "dc_database.h"
#include "memmnger.h"
#include "ptetimer.h"
#include "dbcommon.h"
#include "timings.h"
#include "txtrace.h"

#define MAXCOUNT	"2"
#define NOT_CONNECTED_TO_DB             -3114
#define NOT_LOGGED_INTO_DB              -1012
#define TNS_PACKET_WRITER_FAILED        -12571
#define SHUTDOWN_OR_STARTUP_IN_PROGRESS -1033

/***** External variables */
extern volatile int  EndProcessSignalled;
extern volatile int  MainProcessDone;
extern int           Start();
extern void          Stop ();

/***** Global variables *****/
extern char          ExeName[100];
static char			 ServiceName[100];

static long			 total_tx_num;
static long			 advice_repeat_ctr;


BYTE  DB_MsgSubtype2 ;
BYTE  DB_MsgSubtype1 ;
CHAR  DB_Logging_Filename[256];
CHAR  DB_module_file_name[256];
CHAR  DB_file_path[256];

UINT Max_File_Size_Defined = 0 ;

/* End   */
/* This variable will get the value from the tf.ini
 * from the section DATASERVER with key name DB_ERROR_STATICS_FLAG
 * */
INT    db_error_statics_flag;
double db_error_statics_value;
INT DB_NullCheck_Flag = 0;
#define MODE_INSERT "INSERT"
#define MODE_UPDATE "UPDATE"

//static TLF01			Auth_Tx;
TLF01	tlf01_tmp_struct, db_tlf01_struct;
AUTH_TX Auth_Tx ;
pTLF01 tlf01_ptr;
pNCF01	ncf01_ptr;
NCF01 ncf01_tmp_struct;
BYTE db_select_ncf01 (char[], pNCF01 , char*);
/* declared variable in .C file TF-Ajay 23 Dec 2009 */
char    NetCntlName[12];
char	authorizing_host_queue[9];
CHAR	TxnDate[9];
CHAR    LookUpDate[9];
 INT    dateFlag;
CHAR    AppName   [MAX_APP_NAME_SIZE];


CHAR  sbatch_Erro_warning_Filename[256] = {0};
CHAR  sbatch_module_error_warning_file_name[256] = {0};
CHAR  sbatch_error_warning_file_path[256] = {0};

/* Currency Codes ..TF Phani*/
#define  PESO     "608"
#define  USD      "840"
#define  DEFAULT_REPEAT_COUNT 3

#ifdef WIN32
   #define DEFAULT_DIRECTORY  "\\ascendent\\sbatch"
#else
   #define DEFAULT_DIRECTORY "/ascendent/sbatch"
#endif

BYTE db_get_tlf01_stip_standin_rec(char *, pTLF01 , char *);

BYTE p_buffer [MAX_DATASERVER_BUFFER_LENGTH] ; /* defined to be 25000 */

#ifdef HCTS4
int MaxRetryCount = 0;
void update_atp_stip_tables();
void sbatch_get_max_retry_count();
#endif

CHAR  Version[] = "ATP_11.8.0";

/****************************************************************************
*
*  NAME:             main
*
*  DESCRIPTION:      This function gets optional initial parameter
*                    Network Controller.
*
*  INPUTS:           int argc - number of parameters
*                    pchar argv[1] - The network controller to which the 
*					 standin processed  transactions to be sent
*                  
*  OUTPUTS:          None
*
*  RETURNS:          None
*
*  AUTHOR:           Ravikumar Nageshaiah	
*   
*
****************************************************************************/
void main(int argc, char *argv[])
{
   	int rc = 0;
    memset(NetCntlName, 0, sizeof(NetCntlName));
	memset(TxnDate, 0, sizeof(TxnDate));
	#ifndef WIN32
		strcpy(ExeName, argv[0]);
	#endif

   	if (argc > 1)
	{
      if(strcmp(argv[1], "?") == 0)
		{
         display_log_info();
         return;
		}
		else
		 strcpy(NetCntlName, argv[1]);

		/* If date not provided, It will read system current date 17 Dec,2009 TF-Ajay*/
		if(argv[2]!=NULL)
			strcpy(TxnDate, argv[2]);
    }
	else
	{
		strcpy(NetCntlName,"");
	    strcpy(TxnDate,"");
	}

   if ((rc = MainProcessor()) != 0)
	   printf("Error Running sbatch: send standin batch to host program \n\n");
	
	Stop();

} /* main */

/*****************************************************************************

  Function:    asc_to_bcd
  Description: This function will convert an ascii string to bcd.
  Author:
      unknown
  Inputs:
      afield - a pointer to the ascii string to convert
      how_many - number of characters in p_astr to convert
  Outputs:
      bstring - the bcd string
  Return values:
      None

******************************************************************************/
static  void asc_to_bcd (afield, how_many, bstring)
   pCHAR         afield;
   INT           how_many;
   BYTE          bstring[];
{
   pCHAR  astr;
   UINT   i = 0;

   astr = afield;

   for (i = (strlen(afield) - how_many); i < strlen(afield); i=i+2)
   {
      if ('\0' == astr [i])
         break;

      if (astr[i] != ' ')
         bstring[i/2] = ((astr[i]-'0')*16);

      if (((i+1) < strlen(afield)) && (astr[i+1] != ' '))
         bstring[i/2] += (astr[i+1]-'0');

   }

} /* asc_to_bcd */

/****************************************************************************
*
*  NAME:             MainProcessor
*
*  DESCRIPTION:      This function handles high-level application logic.
*
*  INPUTS:           None
*         
*  OUTPUTS:          None
*
*  RETURNS:          None
*
*  AUTHOR:           Ravikumar Nageshaiah
*   
*  MODIFIED BY:      
*
****************************************************************************/
int MainProcessor()
{
	CHAR     msg[256] = {0} ;
	CHAR     strTemp [256] = {0} ;
	CHAR 	 time_date[25] = {0};
	INT      ret_val = 0;
	CHAR     Buffer[256] = {0};
	//new variable to get lookup days
	int	     lookup_days;

	memset (&Auth_Tx, 0, sizeof Auth_Tx) ;


	GetAppName(AppName) ;
	
    strcpy(ServiceName, AppName);
    sbatch_get_error_warning_file_name_path();

#ifdef HCTS4
    sbatch_get_max_retry_count();
#endif


	/* Display Application version. */
	sprintf(strTemp, "Attempting to start %s, version %s", ServiceName, Version);
	sbatch_log_message( 2, 1, strTemp, "MainProcessor", 0 );
	printf("Attempting to start %s, version %s\n", ServiceName, Version);

	#ifdef _DEBUG
	pteipc_trace_on(); 
	#endif
	
	/* Create queues sbatchA, sbatchI, sbatchC */
	if( !pteipc_init_single_instance_app( AppName, "pte_ipc" ) )
	{	
		sprintf (strTemp,
		  "%s: MainProcessor: Failed to Create XIPC queues",
		   ServiceName) ;
		sbatch_log_message( 1, 3, strTemp, "MainProcessor", 0 );
		/* signal that we are ready to shut down and quit */
		pteipc_shutdown_single_instance_app() ;
		MainProcessDone = 1 ;
		return 1;
	}
	sprintf(strTemp,
	  "Started the %s Service for %s Host",
	   NetCntlName, ServiceName);
	sbatch_log_message( 1, 1, strTemp, "MainProcessor", 0 );
	printf("Started the sbatch Service for %s Host\n",NetCntlName);

	ret_val = get_dataserver_ini_db_error_statics_value( &db_error_statics_flag, &db_error_statics_value,
	                                                      ServiceName,     Buffer );

	/* Check that NetCntlName is not NULL */
	if (strcmp(NetCntlName, "") == 0)
	{
		strcpy(msg, "Please enter a valid Network/Host Controller Name ");
		sbatch_log_message( 1, 3, msg, "MainProcessor", 0 );
		#ifdef _DEBUG
			scanf("%s", &NetCntlName);
		#else
		display_log_info();
		shut_down();
		return 1;
		#endif
	}
	/* Changes done in sbatch to retrieve txn irresepctive of date- Phani 25 march */ 
	/* Check that, if Date is  NULL, Put the msg */
	if (strcmp(TxnDate, "") == 0)
		{
		dateFlag = 1;
		strcpy(msg, "User not provided Date for Sbatch, getting current system date");
		sbatch_log_message( 2, 1, msg, "MainProcessor", 0 );

			/* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
        ptetime_get_timestamp( time_date );

            /* Reformat to get date "YYYYMMDD". */
        get_date( time_date, TxnDate);

    	//read ini and get the STIP lookup days count for NetCntlName
    	lookup_days = get_lookup_days_from_ini(NetCntlName);
    	printf("Lookup days : %d\n", lookup_days);

    	//subtract the lookup days to get the lookup date
    	get_lookup_date(lookup_days);
    	sprintf(msg, "Current System Date : %s\tLookup Date : %s", TxnDate, LookUpDate);
    	printf("%s\n\n", msg);
    	sbatch_log_message( 2, 1, msg, "ProcessTransactions", 0 );
	}
	else
	{
		dateFlag = 0;
		printf("User entered Date : %s\n", TxnDate);
	}

	/* Try to connect to the database */
	if (dbcommon_connect("equitable", "equitable", "equitable", "ORCL", msg) != PTEMSG_OK)
	{
		strcpy(msg, "Unable to Connect to Oracle ");
		sbatch_log_message( 1, 3, msg, "MainProcessor", 0 );
		shut_down();
		return 1;
	}
	/* Initialize structure used to build TLF01 detail */
	memset(&Auth_Tx, 0, sizeof(Auth_Tx)); 
	ProcessTransactions();
  
   shut_down();
   return 0;
          
} /* MainProcessor */


/******************************************************************************
*
*  NAME:         GET_DATE
*
*  DESCRIPTION:  This procedure takes an input string that contains a 
                date and
*                time.  It copies the date portion into an output 
                string,
*                formatted slightly different.
*
*  INPUTS:       time_date : Format = "YYYY-0M-0D-0H.0I.0S.JJJ"
*
*  OUTPUTS:      date_str  : Format = "YYYY0M0D"
*
*  RTRN VALUE:   None
*
*  AUTHOR:      Ajay S.T.
*
*  MODIFICATIONS:

******************************************************************************/
void get_date( pCHAR time_date, pCHAR date_str )
{
	memset ( date_str,  0x00,         9 );
	strncpy( date_str,  time_date,    4 );
	strncat( date_str, &time_date[5], 2 );
	strncat( date_str, &time_date[8], 2 );
}
/****************************************************************************
*
*  NAME:             display_error_message
*
*  DESCRIPTION:      This function sends an error message to the system 
*                    monitor and to standard output if executed in debug mode.
*
*  INPUTS:           message and function name
*
*  OUTPUTS:          system monitor message and standard output message
*
*  RETURNS:          None
*
*  AUTHOR:           Darcy Person
*
****************************************************************************/
void display_error_message(pBYTE msg, pBYTE function)
{
   sbatch_log_message( 1, 1, msg, function, 0 );
   /* The print macro only displays messages in debug mode */
   strcat(msg, "\n");
   PRINT(msg);

   return;

} /* display_error_message */


/****************************************************************************
*
*  NAME:             display_log_info
*
*  DESCRIPTION:      This function displays information concerning the 
*                    execution of the Transaction Log File program.
*
*  INPUTS:           None
*         
*  OUTPUTS:          Display to standard output
*
*  RETURNS:          None
*
*  AUTHOR:           Darcy Person
*   
****************************************************************************/
void display_log_info(void)
{
   printf("\n\n");
   printf("sbatch - version %s\n\n", Version );
   printf("The Standin Batch To Host program sends all the ATP standin    \n");
   printf("approved transactions to host when the host link is down. If a \n");
   printf("valid Network Controller name and Date of transaction are specified on the command line,\n");
   printf("all the transactions that were processed by ATP for that Network for purticular given date\n");
   printf("Host are sent resent to the specified host. \n");
   printf(" sbatch  - Network Controller Name is  Mandatory and Date is optional, \n\n");
   printf(" If Date not provide. Sbatch will read System current date, \n\n");
   printf(" Example: sbatch  ncvis2 -  this would send");
   printf(" all the ATP standin approved transactions on 20091210(YYYYMMDD) for Cadience's VISA Host Link\n\n");
   
   return;

} /* display_log_info */

/****************************************************************************
*
*  NAME:             EndProcess
*
*  DESCRIPTION:      
*
*  INPUTS:           None
*         
*  OUTPUTS:          None
*
*  RETURNS:          BOOLEAN true for success or false for failure.
*
*  AUTHOR:           Ravikumar Nageshaiah
*   
*  MODIFIED BY:    
****************************************************************************/
void EndProcess(void)
{
   PRINT("\n");

} /* EndProcess */
/****************************************************************************/
BYTE Check_Is_Host_Up (BYTE* destination_name)
{
   XINT  ReturnCode ;
   CHAR  strTemp [256] = {0} ;
   BYTE  tmouts[100] = {0}, txns[100] = {0}, state[100] = {0} ;
   BYTE  ret_val = false;int i;

   if (destination_name[0] != 0)
   {
      for( i = 0; destination_name[i] != '\0'; i++);   
	  destination_name[--i];	
	  if(destination_name[i] == 'A')
	  {
		  destination_name[i] = '\0';
	  }
	  strcpy (strTemp, destination_name) ;
      strcat (strTemp, "Table") ;

      ReturnCode = ReadGenericTable (strTemp, tmouts, txns, state) ;
      if (ReturnCode == MEMACCESSERROR)
      {
         memset (strTemp, 0, sizeof strTemp) ;
         sprintf (strTemp, 
                 "Err accessing shared memory (%s)\n",
                  destination_name) ;
         sbatch_log_message( 1, 3, strTemp, Check_Is_Host_Up, 0 );
      }
      else
      {
         /* Forward request to network controller
          * if the network controller is Online.
          */

		  if (0 ==  strncmp(state, ONLINE,1) )
		  {
            ret_val = true;
      }
   }
   }
   return( ret_val );

} /* TxService_Is_Host_Up */

INT sbatch_Check_If_VISA_Transaction ()
{
	if (Auth_Tx.TLF01_details.card_num[0]=='4')
	{
		return true ;
	}
	else
	{
		return false;
	}
}

/****************************************************************************
*
*  NAME:             ProcessTransactions
*
*  DESCRIPTION:      This function retrieves TLF01_STIP records. 
*
*  INPUTS:           NetCntlName (user-provided)
*         
*  OUTPUTS:          None
*
*  RETURNS:          None
*
*  AUTHOR:           Ravikumar Nageshaiah 
*   
*  MODIFIED BY:			Phani Kumar 
*
****************************************************************************/
void ProcessTransactions(void)
{
	char           msg[256];
	char		   strBuffer[256];
	BYTE           rc;
	pPTE_MSG       p_msg = 0 ;
	int  		   dest_len=0;
	LONG           lReturnCode = 0L ;
	int            next_portion = 0;
	INT            err_ctr = 0;
	CHAR           strTemp [256] = {0} ;
	BYTE           tranid[21];
    EMV_RAW        emvraw;
    PAD_RAW        pad_raw_data;
	BYTE		   emv_result_code;
	BYTE           padraw_result_code;
	int 		   temp_len = 0;
	char 		   str_temp_len [4] = {0};
	BYTE 		   bstring[12];
	BYTE 		   ReturnCode = true;
	INT			   retryCount = 0;

	memset(&Auth_Tx, 0, sizeof(Auth_Tx)); 
	memset(&tlf01_tmp_struct,0,sizeof(TLF01));
	memset(&ncf01_tmp_struct,0,sizeof(NCF01));
	
	rc = db_select_ncf01(NetCntlName,&ncf01_tmp_struct, msg);
	if ( rc == PTEMSG_OK)
	{
		advice_repeat_ctr = atoi(ncf01_tmp_struct.advice_repeat_counter);
		if (advice_repeat_ctr == 0)
		{
			advice_repeat_ctr = DEFAULT_REPEAT_COUNT;
		}
		printf("Obtained advice_repeat_counter from NCF01: %d\n", advice_repeat_ctr);
	}
	else
	{
		printf("The Oracle error number to get NCF01.advice_repeat_counter %d::%s \n",dbcommon_oracle_to_pte_rcode( rc, msg), msg);
		printf("Setting the advice_repeat_counter to 3\n");
		
	}

	//Check for host status
	if(Check_Is_Host_Up(NetCntlName) == false)
	{
			printf("Sbatch: NC is not ONLINE, Plz make network ONLINE and START again : Network controller-  %s\n",NetCntlName);
			sprintf(msg,"Sbatch: NC is not ONLINE, Plz make network ONLINE and START again : Network controller-  %s",NetCntlName);
			sbatch_log_message( 2, 1, msg, "ProcessTransactions", 0 );
			display_error_message(msg,"ProcessTransactions");
			return;
	}

	while (next_portion == 0)
	{
		strcpy(authorizing_host_queue, NetCntlName);
		memset(&tlf01_tmp_struct,0,sizeof(TLF01));
		#ifdef WIN32
			Sleep (10000);
		#else
			usleep (1000000);// 1 sec delay
		#endif
		rc = db_get_tlf01_stip_standin_rec(authorizing_host_queue, &tlf01_tmp_struct, msg );

		if ( rc == PTEMSG_OK)
		{
			memset(&Auth_Tx, 0, sizeof(Auth_Tx));
			memcpy (&Auth_Tx.TLF01_details, &tlf01_tmp_struct, sizeof (TLF01)) ;
			strcpy(Auth_Tx.TLF01_details.orig_message,Auth_Tx.TLF01_details.message_type);
			strcpy(Auth_Tx.TLF01_details.orig_retrieval_ref_num, Auth_Tx.TLF01_details.retrieval_ref_num);
			strcpy(Auth_Tx.TLF01_details.retrieval_ref_num,"");

			if( ((strncmp(Auth_Tx.TLF01_details.handler_queue,"ncmcr3",6) == 0  ||
				  strncmp(Auth_Tx.TLF01_details.handler_queue,"ncmcrd",6) == 0) ||
				 strncmp(Auth_Tx.TLF01_details.handler_queue,"ncjcb",5)  == 0 ||
				 strncmp(Auth_Tx.TLF01_details.handler_queue,"ncvis",5)  == 0) &&
			     (strncmp(Auth_Tx.TLF01_details.message_type,  "0400", 4) == 0   ||
				 strncmp(Auth_Tx.TLF01_details.message_type,   "0420", 4) == 0 ))
			{
				Auth_Tx.TLF01_details.tx_key = AUTH_REVERSAL_ADVICE;
				strcpy(Auth_Tx.TLF01_details.message_type,"0420");
			   if(Auth_Tx.TLF01_details.orig_sys_trace_num[0] == 0x00)
			   {
				 memcpy(Auth_Tx.TLF01_details.orig_sys_trace_num,Auth_Tx.TLF01_details.sys_trace_audit_num,6);
			   }
			}
			else if((strncmp(Auth_Tx.TLF01_details.handler_queue,"ncpos",5)  == 0)&&
				     (strncmp(Auth_Tx.TLF01_details.message_type,  "0400", 4) == 0   ||
					 strncmp(Auth_Tx.TLF01_details.message_type,   "0420", 4) == 0 ||
					 strncmp(Auth_Tx.TLF01_details.message_type,   "1420", 4) == 0))
			{
				Auth_Tx.TLF01_details.tx_key = AUTH_REVERSAL_ADVICE;
				strcpy(Auth_Tx.TLF01_details.message_type,"1420");
			   if(Auth_Tx.TLF01_details.orig_sys_trace_num[0] == 0x00)
			   {
				 memcpy(Auth_Tx.TLF01_details.orig_sys_trace_num,Auth_Tx.TLF01_details.sys_trace_audit_num,6);
			   }
			}
			else
			{
			Auth_Tx.TLF01_details.tx_key = AUTH_ADVICE;
			
			if((strcmp(Auth_Tx.TLF01_details.message_type,"0120") == 0))
				strcpy(Auth_Tx.TLF01_details.message_type,"0121");
			else
				strcpy(Auth_Tx.TLF01_details.message_type,"0120");
			}
			if((0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncmcrd",6)||
				0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncmcr3",6)||
				0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncvis",5)	) &&
			    0 == strncmp(Auth_Tx.TLF01_details.processing_code,"20",2))
			{
				//Do nothing, send the response code stored as it for inc refund txn
			}
			else
			{
			#ifndef HCTS4
				strcpy(Auth_Tx.TLF01_details.response_code,"");
			#endif
			}
			strcpy(Auth_Tx.TLF01_details.originator_queue,application_que_name);
			strcpy(Auth_Tx.TLF01_details.handler_queue, application_que_name);

		/*	strcpy(Auth_Tx.TLF01_details.message_type,"0120");  This line no need TF, Ajay*/
			if(0 != strncmp(Auth_Tx.TLF01_details.response_text,"DE48 SE84:",10) &&
			   0 != strncmp(Auth_Tx.TLF01_details.message_type,"0120",4) &&
			  (0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"ncmcrd",6) || 0== strncmp(Auth_Tx.TLF01_details.handler_queue,"ncmcr3",6)))
			{
				strcpy(Auth_Tx.TLF01_details.response_text,"");
			}

			strcpy(Auth_Tx.TLF01_details.authorizing_host_queue, NetCntlName);
		/* TF Phani- Copy indutry code*/
			strcpy(Auth_Tx.industry_code, Auth_Tx.TLF01_details.product_codes[8].code);
		/* If txn from DCPSIO or voice no need to send fld 6 and 51.
			Make it null, if it is there TF-Ajay Jul-31-2009 */
			if((strcmp(Auth_Tx.TLF01_details.dispensation.origin,"LO")==0)||
			   (strcmp(Auth_Tx.TLF01_details.voice_auth,"Y")==0))
				{
				 strcpy(Auth_Tx.ch_billing_curr_code,"");
				 strcpy(Auth_Tx.ch_billing_amt,"");
				}
			else
			{	/* Copy Billing Conversion rate back*/
				 strcpy(Auth_Tx.ch_billing_conv_rate,Auth_Tx.TLF01_details.product_codes[8].amount);
				 strcpy(Auth_Tx.ch_billing_amt,Auth_Tx.TLF01_details.product_codes[7].amount);
				 strcpy(Auth_Tx.ch_billing_curr_code,Auth_Tx.TLF01_details.product_codes[7].code);

				/* In case of 3rd currency, BDO needs to populates 4,6 and 49 and 51 as it is.. TF PHANI */
				if ( (strncmp(Auth_Tx.TLF01_details.acquirer_id,"ncvis2",5)==0))
				{
					if(0 != strcmp( PESO, Auth_Tx.TLF01_details.nfi_seq_nbr)&& 
					   0 != strcmp( USD,  Auth_Tx.TLF01_details.nfi_seq_nbr)&&
					  (0 != strcmp( USD,  Auth_Tx.TLF01_details.num_dbsales)&&
					   0 != strcmp( PESO, Auth_Tx.TLF01_details.num_dbsales) ) )
      				{	/*Copy DE4 to sales_amount, Copy DE6 to total_amount.,Copy DE49 to nfi_seq_nbr, Copy DE51 to currency_code..*/

						
					   /* Copy DE6 to total_amount. */
					   memcpy( Auth_Tx.ch_billing_amt,Auth_Tx.TLF01_details.total_amount,12 );
				   		  /* Copy DE4 to sales_amount.  */
					   if(strlen(Auth_Tx.TLF01_details.sales_amount) > 2 && atoi(Auth_Tx.TLF01_details.sales_amount) > 0)
					   memcpy( Auth_Tx.TLF01_details.total_amount,Auth_Tx.TLF01_details.sales_amount,12 );
						  /* Copy DE51 to currency_code. */
					   memcpy( Auth_Tx.ch_billing_curr_code,Auth_Tx.TLF01_details.currency_code,3);
					   /* Copy DE49 to nfi_seq_nbr. */
					   if(strlen(Auth_Tx.TLF01_details.nfi_seq_nbr) > 2)
					   memcpy( Auth_Tx.TLF01_details.currency_code,Auth_Tx.TLF01_details.nfi_seq_nbr,3 );
					}
					else
					{/* its not 3rd currency don't change anything */}
				}
				else if((strncmp(Auth_Tx.TLF01_details.acquirer_id,"ncjcb2",5)==0))
				{

				}
				else if((strncmp(Auth_Tx.TLF01_details.acquirer_id,"ncmcr2",4)==0))
				{		 /* Copy DE9 to coversion rate. */
					memcpy( Auth_Tx.TLF01_details.conversion_rate,Auth_Tx.TLF01_details.product_codes[12].amount,8 );
						  /* Copy DE50 to settlement currency_code. */
					memcpy( Auth_Tx.settlement_curr_code,Auth_Tx.TLF01_details.product_codes[12].code,3);
					// DE 61 value for 0120
					strcpy(Auth_Tx.acceptor_term_name,Auth_Tx.TLF01_details.source_key);
				}
				else if((strncmp(Auth_Tx.TLF01_details.acquirer_id,"nposa2",5)==0))
				{
				}
				else
				{ /* Don't do anything */
				}

			}

			dest_len = strlen(authorizing_host_queue);
			if (authorizing_host_queue[dest_len - 1] == 'A'){}
			else
			strcat(authorizing_host_queue,"A");
// Get EMV data
			 if ( 0 == strncmp("005", Auth_Tx.TLF01_details.pos_entry_mode, 3) ||
				 0 == strncmp("007", Auth_Tx.TLF01_details.pos_entry_mode, 3)) // Check for EMV tx
              {
                        /* Fetch any EMV data associated with this txn. */
                        memset( &emvraw, 0x00, sizeof(EMV_RAW) );
                        memset( tranid, 0x00, sizeof(tranid) );
                        memcpy( tranid, Auth_Tx.TLF01_details.primary_key.transaction_id, sizeof(tranid));
                        emv_result_code = db_select_emvraw(tranid,&emvraw,strBuffer);
                        if ( emv_result_code == PTEMSG_OK )
                        {
                           memcpy(Auth_Tx.EMV_details.emv_block,
                                   emvraw.RAW_DATA,
                                   atoi(emvraw.DATA_LENGTH) );

                          
                        }
                        else
                        {
                           /* Orig txn has been found, so even if
                            * EMV data is not found, still return success.
                            */
                           emv_result_code = PTEMSG_OK;
                        }
              }

/*End of EMV data*/
			/* Validate amount*/
			if(strlen(Auth_Tx.TLF01_details.total_amount)<12)
			{
				//Some thing wrong with conversion Make sure length will be 12
				INT i = atoi(Auth_Tx.TLF01_details.total_amount);
				sprintf(Auth_Tx.TLF01_details.total_amount,"%012d",i);
			}
			// Lets check for VISA DE56 value.
			if(0 == strncmp(Auth_Tx.TLF01_details.product_codes[0].quantity,"56",2) &&
			   true == sbatch_Check_If_VISA_Transaction())
			{
				padraw_result_code = db_select_padraw(tranid,&pad_raw_data,strBuffer);
				temp_len = atoi(pad_raw_data.DATA_LENGTH);
                Auth_Tx.future_use_1[0] =  temp_len ;
			    memcpy( Auth_Tx.future_use_1 + 1,pad_raw_data.RAW_DATA,temp_len );
			}
			err_ctr = 0;
			printf("Processing transaction with tlf01_stip ID: %s\t", Auth_Tx.TLF01_details.primary_key.transaction_id);
			printf("Date_yyyymmdd : %s\n", Auth_Tx.TLF01_details.date_yyyymmdd);

			lReturnCode = Send_Request_To_Service (  authorizing_host_queue,
											MT_AUTH_REQUEST, 
											0, 
											0, 
											0, 
											(pBYTE) (&Auth_Tx), 
											sizeof(AUTH_TX)) ;
#ifdef HCTS4
			//new changes - don't wait for the response from Vertexon, send and update the stip table, process next transaction
			//update stip table, if lReturnCode = 0
			if(lReturnCode == DEVICE_OK)
			{
				retryCount = 0;
				update_atp_stip_tables();
				sprintf( msg, "ATP STIP tables updated for : %s\n", Auth_Tx.TLF01_details.primary_key.transaction_id );
				sbatch_log_message( 2, 1, msg, "ProcessTransactions", 0 );
			}
			else
			{
				retryCount++;
				printf("Failed to send request to %s for : %s\n",
						authorizing_host_queue,
						Auth_Tx.TLF01_details.primary_key.transaction_id);

				if(retryCount > MaxRetryCount)
				{
					sprintf( msg, "Reached maximum retry count %d, Shutting down Sbatch", retryCount );
					display_error_message(msg, "ProcessTransactions");
					shut_down();
					exit(0);
				}
			}
#else
			while (err_ctr <= advice_repeat_ctr )
			{
				/* You are blocked here waiting for a message on either app queue or
				* control que.  If there is no message on either que for 5 seconds,
				* the blocking call returns.  Application_que_name and control_que_name
				* are populated in pteipc_init_multiple_instance_app (pteipc.c).
				*/
				p_msg = pteipc_receive( application_que_name, control_que_name,
							  20, &lReturnCode);

				if(p_msg != NULL_PTR)
				{
					err_ctr = 0;
					PinnacleMsgHandler (p_msg);
					total_tx_num++;
					free(p_msg);
					break;
				}
				else if(lReturnCode == QUE_ER_TIMEOUT)
				{
					/* Queue has timed out. There is nothing in it. */
					/*increment the retry count and try sending 121 message now*/
					err_ctr++;
					/*Amex host does not identifys 0121 msg. So send as 0120 msg even though its repeat. TF-Ajay 28-Jul-09*/
					if((strncmp(Auth_Tx.TLF01_details.acquirer_id,"nposa2",5)==0)||
						(0== strncmp(Auth_Tx.TLF01_details.handler_queue,"ncposA",5)))
					{		/* Don't change the message type */

					}
					else if( strncmp( Auth_Tx.TLF01_details.message_type,  "0420", 4 ) == 0 ||
							 strncmp( Auth_Tx.TLF01_details.message_type,  "0400", 4 ) == 0)
					{
						Auth_Tx.TLF01_details.tx_key = AUTH_REVERSAL_ADVICE;
						strcpy(Auth_Tx.TLF01_details.message_type,"0420");
					}
					else
					{
						strcpy(Auth_Tx.TLF01_details.message_type,"0121");
						Auth_Tx.TLF01_details.tx_key = AUTH_ADVICE_REPEAT;
					}

					lReturnCode = Send_Request_To_Service (  authorizing_host_queue,
								MT_AUTH_REQUEST, 
								0, 
								0, 
								0, 
								(pBYTE) (&Auth_Tx), 
								sizeof(AUTH_TX));
					
				}
				else
				{
					err_ctr++;
					pteipc_get_errormsg(lReturnCode, strTemp);
					sprintf( strBuffer,
					 "Error on pteipc_receive %s, error code: %ld, retry attempt:%d",
					  strTemp, lReturnCode, err_ctr );
					sbatch_log_message( 1, 3, strTemp, ProcessTransactions, 0 );
					/* There is a problem with the queue.  Delay 1 second
					* so as to not flood Applnk's queue and syslog.
					*/
					#ifdef WIN32
						Sleep( 1000 );
					#else
						usleep( 1000000 );
					#endif

					/* Inform Operators if service's queue is no longer readable. */
					if ( err_ctr > 10 )
					{
						err_ctr = 0;
						sprintf( strBuffer,
							   "SHUT DOWN %s. UNABLE TO ACCESS QUEUE!!!!",
							 ServiceName );
						sbatch_log_message( 1, 3, strBuffer, ProcessTransactions, 0 );
						#ifdef WIN32
							Sleep (50000);
						#else
							usleep (50000000);
						#endif
					}
				}
				if( err_ctr >= advice_repeat_ctr)
				{
					sprintf( msg, "Reached maximum auth advice retry count %d , No Repsone from Cadience ",advice_repeat_ctr );
					display_error_message(msg,"ProcessTransactions");
					shut_down();
					exit(0);
				}
			}//while advice_repeat_ctr
#endif
 		} /* endif */
	 
		else if (rc == PTEMSG_NOT_FOUND)
		{
			next_portion = 1;
			continue;
		}
		else if( NOT_CONNECTED_TO_DB 				== rc ||
				 NOT_LOGGED_INTO_DB 				== rc ||
				 TNS_PACKET_WRITER_FAILED 			== rc ||
				 SHUTDOWN_OR_STARTUP_IN_PROGRESS	== rc  ||
				 PTEMSG_NOT_CONNECTED_TO_DB			== rc )
		 {
			dbcommon_disconnect (msg);
		    if (dbcommon_connect("equitable", "equitable", "equitable", "ORCL", msg) != PTEMSG_OK)
		    {
		    	strcat (msg, " DB Reconnection failed");
			   sbatch_log_message(1, 3, msg, "ProcessTransactions", 0);
			   next_portion = 1;
			   continue;
		   }
		   else
		   {
			   sbatch_log_message(1, 1, "DataBase Recoonected", "ProcessTransactions", 0);
		   }
		 }
		else
		{/* TF Phani.. Break the loop even for DB errors*/
			next_portion = 1;
			continue;
		}

	} // end while - next_portion

	//new changes
	//update the non lookup days txns as PROCESSED in TLF01_STIP table to avoid posting
	//only when user not provided the date
	if(dateFlag == 1)
	{
		ReturnCode = db_update_tlf01_stip_non_lookup( NetCntlName, strTemp );
		if(ReturnCode == PTEMSG_OK)
		{
			sprintf( msg, "Updated the Transactions beyond lookup days as PROCESSED for %s", NetCntlName );
			sbatch_log_message( 2, 1, msg, "ProcessTransactions", 0 );
		}
		else
		{
			sprintf( msg, "Failed to update as PROCESSED for Transactions beyond lookup days for %s", NetCntlName );
			sbatch_log_message( 3, 3, msg, "ProcessTransactions", 0 );
		}
	}

	if(total_tx_num == 0)
	{
			sprintf( msg, "Completed Process Transactions for %s. Could not find any records", NetCntlName );
			sbatch_log_message( 2, 1, msg, "ProcessTransactions", 0 );
	}
	else
	{		
			sprintf(msg,"Sbatch processed %ld number of transactions for %s ", total_tx_num,NetCntlName);
			display_error_message(msg,"ProcessTransactions");
			sbatch_log_message( 2, 1, msg, "ProcessTransactions", 0 );
	}
	printf("%s\n",msg);
	display_error_message(msg,"ProcessTransactions");

} // ProcessTransactions 

#ifdef HCTS4
void update_atp_stip_tables()
{
	int ReturnCode;
	char strTemp[256] = {0};
	char msg[256] = {0};

	if (Auth_Tx.TLF01_details.tx_key == AUTH_ADVICE 		 ||
		Auth_Tx.TLF01_details.tx_key == AUTH_ADVICE_REPEAT	 ||
		Auth_Tx.TLF01_details.tx_key == AUTH_ADVICE_RESPONSE ||
		Auth_Tx.TLF01_details.tx_key == AUTH_ADVICE_REPEAT_RESPONSE)
	{
		strcpy(Auth_Tx.TLF01_details.product_codes[1].code, PROCESSED);
	}
	else
	{
		strcpy(Auth_Tx.TLF01_details.product_codes[1].code, REVERSED);
	}

	if (DB_NullCheck_Flag==1)
	{
		CheckNULLTerminatedinTLF01(&Auth_Tx.TLF01_details, MODE_UPDATE);
	}

	ReturnCode = db_update_tlf01_stip(&Auth_Tx.TLF01_details, strTemp);
	if(ReturnCode != PTEMSG_OK)
	{
		printf("DB TLF01 STIP TABLE UPDATE ERROR for %s\n\n", Auth_Tx.TLF01_details.primary_key.transaction_id);
		sbatch_log_message( 1, 1, strTemp, "update_atp_stip_tables", 0 );
		shut_down();
		exit(0);
	}

	if(strcmp(Auth_Tx.TLF01_details.dispensation.origin, "RO")==0)
	{
		ReturnCode = db_update_ncf30_stip(&Auth_Tx.TLF01_details, strTemp);
	}
	else if(strcmp(Auth_Tx.TLF01_details.voice_auth, "Y")==0)
	{
		ReturnCode = db_update_bch20_stip(&Auth_Tx.TLF01_details, strTemp);
	}
	else
	{
		ReturnCode = db_update_bch10_stip(&Auth_Tx.TLF01_details, strTemp);
	}
}
#endif

//new function to get lookup days from tf.ini - Amith
int get_lookup_days_from_ini(char network_name)
{
	CHAR filename[128]   = {0}, tmpstr[128] = {0};
	CHAR network_key[20] = {0}, buffer[10]  = {0};

	GetPinnacleConfigDirectory(tmpstr);
	sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

	//copy the network key based on Cadencie/Vertexon
	if(strcmp(network_name, "ncvis2") == 0)
		strcpy(network_key, "VISA_LOOKUP");
	else if(strcmp(network_name, "ncmcr2") == 0)
		strcpy(network_key, "MCRD_LOOKUP");
	else if(strcmp(network_name, "ncjcb2") == 0)
		strcpy(network_key, "JCB_LOOKUP");
	else if(strcmp(network_name, "nccup2") == 0)
		strcpy(network_key, "CUP_LOOKUP");
	else if(strcmp(network_name, "ncdci2") == 0)
		strcpy(network_key, "DCI_LOOKUP");
	else if(strcmp(network_name, "natma2") == 0 || strcmp(network_name, "nposa2") == 0)
		strcpy(network_key, "AMEX_LOOKUP");

	GetPrivateProfileString(
					"SBATCH_LOOKUP_SECTION",
					network_key,
					"1",
					buffer,
					strlen(buffer) - 1,
					filename
	             );

	return (atoi(buffer));
}

void get_lookup_date(int days)
{
	char buf_date[9] = {0};
	long julian_date = 0;

	strcpy(buf_date, TxnDate);
	//convert to julian, subtract and convert back to normal
    julian_date = Txutils_Calculate_Julian_Date ((BYTE*)buf_date);
    julian_date = julian_date - days;
    Txutils_Calculate_Gregorian_Date( julian_date, buf_date);

    memset(LookUpDate, 0, sizeof(LookUpDate));
    memcpy(LookUpDate, buf_date, strlen(buf_date));
}

/****************************************************************************
*
*  NAME:             shut_down
*
*  DESCRIPTION:      This function prepares the application to shut down.
*
*  INPUTS:           None
*
*  OUTPUTS:          None
*
*  RETURNS:          None
*
*  AUTHOR:           Darcy Person
*
****************************************************************************/

void shut_down(void)
{
   CHAR Buffer[100] = "";

   /* Added to fix a timing problem */
	#ifdef WIN32
		Sleep(2000);
	#else
		sleep(2);
	#endif

   /* Added to fix Equitable error report #852 */
   sprintf( Buffer,"Stopping sbatch program, version %s",Version);
   display_error_message( Buffer, "MainProcessor");
			
   pteipc_shutdown_single_instance_app(); 
   MainProcessDone = 1;

   return;

} /* shut_down */


BYTE Send_Request_To_Service (BYTE* que_name, BYTE nType, BYTE nSubType1, 
							  BYTE nSubType2, BYTE nAppDataType, 
							  pBYTE p_data,   INT nDataSize)
{
	LONG retCode = 0L ;
	pPTE_MSG p_msg_out = NULL_PTR ;
	INT nBufferSize = 0 ;
	char strError[512] = {0} ;

	memset (p_buffer, 0, sizeof p_buffer) ;
	memcpy (p_buffer, &Auth_Tx, sizeof Auth_Tx) ;
	
	if (p_data != 0)
		memcpy (p_buffer + sizeof Auth_Tx, p_data, nDataSize) ;
		
	nBufferSize = sizeof Auth_Tx + nDataSize ;

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
        sbatch_log_message( 1, 3, strError, Send_Request_To_Service, 0 );
		return SYSTEM_ERROR ;
 	} 

	retCode = pteipc_send (p_msg_out, que_name) ;
	free (p_msg_out) ;

	if (retCode != 0)
	{
		sprintf (strError,"Error %ld has occurred sending message to %s\n", retCode, que_name) ;
        sbatch_log_message( 1, 3, strError, Send_Request_To_Service, 0 );
		return SYSTEM_ERROR ;
	}

	return DEVICE_OK ;
} /* Send_Request_To_Service */




/*************************************************************************************
NAME:			PinnacleMsgHandler
DESCRIPTION:	This function gets called when the primary thread (MainProcessor) of the
				application gets a message in its queue. When the thread receives a message,
				this function dispatches it to a function depending on the message type
INPUTS:			p_msg_in: Pointer to a PTE_MSG
OUTPUTS:		None
RETURNS:        Void
AUTHOR:         Ravikumar K N  
MODIFIED BY:	
*************************************************************************************/
void PinnacleMsgHandler(pPTE_MSG p_msg_in)

{
   pPTE_MSG p_cur_msg = 0 ;
   pPTE_MSG p_msg_out = 0 ;
   BYTE     MessageType = 0 ;
   BYTE     ReturnCode = 0 ;
   CHAR     strTemp[256] = {0} ;


	memset (&Auth_Tx, 0, sizeof (AUTH_TX)) ;
    

    p_cur_msg = p_msg_in ;
    p_msg_out = 0 ;

   MessageType = ptemsg_get_msg_type (p_cur_msg) ;
   switch (MessageType)
   {
      case MT_AUTH_RESPONSE: 

         ReturnCode = Process_Auth_Advice_Response (p_cur_msg) ;
      break ;	


	  
      default:
          strcpy (strTemp, "Unknown message type received in the Pinnacle Message Handler") ;
          sbatch_log_message( 1, 3, strTemp, PinnacleMsgHandler, 0 );
		break ;
    }  /* switch msg_type */
} /* PinnacleMsgHandler */


BYTE Process_Auth_Advice_Response (pPTE_MSG p_msg_in)
{
	BYTE ReturnCode = true;
	pBYTE pATEMessageDataData = 0 ;
	pPTE_MSG_DATA pATEMessageData = 0 ;
	char strTemp[256] = {0} ;
    int txnCount=0;
	char strBuffer[50]={0};

	
	pATEMessageData			= ptemsg_get_pte_msg_data (p_msg_in) ;
	pATEMessageDataData		= ptemsg_get_pte_msg_data_data (pATEMessageData) ;
	
	/*******************************************************************/
    memset (&Auth_Tx, 0, sizeof Auth_Tx) ;
	memcpy (&Auth_Tx, (AUTH_TX *)pATEMessageDataData, sizeof Auth_Tx) ;
	/*******************************************************************/
	sprintf (strTemp,"Received msg from host-RespCode:%s  ",
		      Auth_Tx.TLF01_details.response_code) ;
	sbatch_log_message( 2, 1, strTemp, Process_Auth_Advice_Response, 1 );

	 if( (0== strncmp(Auth_Tx.TLF01_details.response_code,
			          REFER_TO_CARD_ISSUER,ISO_RESP_LEN)) ||
		 (0== strncmp(Auth_Tx.TLF01_details.response_code,
			          DESTINATION_UNAVAILABLE,ISO_RESP_LEN)))
	 {
		 sprintf( strBuffer, "Host is not online sbatch is going down");
		 sbatch_log_message( 1, 3, strBuffer, Process_Auth_Advice_Response, 0 );
		 shut_down();
		 exit(0);

	 }
	 else if (Auth_Tx.TLF01_details.tx_key==AUTH_ADVICE_RESPONSE ||
			 Auth_Tx.TLF01_details.tx_key==AUTH_ADVICE_REPEAT_RESPONSE)
	 {

		 if ((0== strncmp(Auth_Tx.TLF01_details.response_code,
				 	      TRANSACTION_APPROVED,ISO_RESP_LEN)) ||
			 (0== strncmp(Auth_Tx.TLF01_details.response_code,
						  DUPLICATE_TRANSACTION,ISO_RESP_LEN)) ||
		 	 (0== strncmp(Auth_Tx.TLF01_details.product_codes[12].code,
		 				  DUPLICATE_TRANSACTION_AMEX_ACTION_CODE,ACTION_CODE_LEN)) ||
		 	 (0== strncmp(Auth_Tx.TLF01_details.product_codes[12].code,
		 				  DUPLICATE_TRANSACTION_DCI_ACTION_CODE,ACTION_CODE_LEN)))
		 {
			 printf("Response returned from Host for Auth Advice \n");
			 memset( &db_tlf01_struct, 0, sizeof( db_tlf01_struct ) );
			 memcpy(&db_tlf01_struct, (pTLF01) pATEMessageDataData,	sizeof(TLF01));
			 strcpy(Auth_Tx.TLF01_details.product_codes[1].code,PROCESSED);

			 if (DB_NullCheck_Flag==1)
			 {
				 CheckNULLTerminatedinTLF01 (&Auth_Tx.TLF01_details,MODE_UPDATE);
			 }

			 ReturnCode = db_update_tlf01_stip( &Auth_Tx.TLF01_details, strTemp );

			 if(strcmp(Auth_Tx.TLF01_details.dispensation.origin,"RO")==0)
			 {
				 ReturnCode = db_update_ncf30_stip(&Auth_Tx.TLF01_details, strTemp);
			 }
			 else if(strcmp(Auth_Tx.TLF01_details.voice_auth,"Y")==0)
			 {
				ReturnCode = db_update_bch20_stip(&Auth_Tx.TLF01_details, strTemp);
			 }
			 else
			 {
				ReturnCode = db_update_bch10_stip(&Auth_Tx.TLF01_details,strTemp);
			}
		 }
		 else if(0 == strncmp(Auth_Tx.TLF01_details.response_code,
				 	 	 	  REENTER_TRANSACTION,ISO_RESP_LEN))
		 {
			 sprintf( strBuffer, "Timeout Recieved, transaction is not processed");
			 sbatch_log_message( 1, 3, strBuffer, Process_Auth_Advice_Response, 0 );
		 }
	 }
	 else if ((strncmp(Auth_Tx.TLF01_details.authorizing_host_queue,"ncm",3)   == 0  ||
			   strncmp(Auth_Tx.TLF01_details.authorizing_host_queue,"ncjcb",5) == 0	||
			   strncmp(Auth_Tx.TLF01_details.authorizing_host_queue,"ncvis",5) == 0 	||
			   strncmp(Auth_Tx.TLF01_details.authorizing_host_queue,"nposa2",6) == 0 )&&
			  (Auth_Tx.TLF01_details.tx_key == AUTH_VOID_SALE_RESPONSE 		   		||
			   Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_ADVICE_RESPONSE   		||
			   Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_ADVICE_REPEAT_RESPONSE ||
			   Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_RESPONSE               ||
			   Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_REPEAT_RESPONSE))
	{
		if ((0== strncmp(Auth_Tx.TLF01_details.response_code,
						 TRANSACTION_APPROVED,ISO_RESP_LEN)) ||
			(0== strncmp(Auth_Tx.TLF01_details.response_code,
						 DUPLICATE_TRANSACTION,ISO_RESP_LEN)) ||
			(0== strncmp(Auth_Tx.TLF01_details.product_codes[12].code,
						 DUPLICATE_TRANSACTION_AMEX_ACTION_CODE,ACTION_CODE_LEN)) ||
			(0== strncmp(Auth_Tx.TLF01_details.product_codes[12].code,
						 DUPLICATE_TRANSACTION_DCI_ACTION_CODE,ACTION_CODE_LEN)))
		{
			printf("Response returned from Host for Auth Reversal Advice \n");
			memset( &db_tlf01_struct, 0, sizeof( db_tlf01_struct ) );
			memcpy( &db_tlf01_struct, ( pTLF01 )pATEMessageDataData, sizeof( TLF01 ) );
			strcpy(Auth_Tx.TLF01_details.product_codes[1].code,REVERSED);

			if (DB_NullCheck_Flag==1)
			{
				CheckNULLTerminatedinTLF01 (&Auth_Tx.TLF01_details,MODE_UPDATE);
			}

			ReturnCode = db_update_tlf01_stip( &Auth_Tx.TLF01_details, strTemp );

			if(strcmp(Auth_Tx.TLF01_details.dispensation.origin,"RO")==0)
			{
				ReturnCode= db_update_ncf30_stip(&Auth_Tx.TLF01_details, strTemp );
			}
			else if(strcmp(Auth_Tx.TLF01_details.voice_auth,"Y")==0)
			{
				ReturnCode = db_update_bch20_stip(&Auth_Tx.TLF01_details, strTemp );
			}
			else
			{
				ReturnCode = db_update_bch10_stip(&Auth_Tx.TLF01_details, strTemp );
			}
		}
		else if(0 == strncmp(Auth_Tx.TLF01_details.response_code,
							  REENTER_TRANSACTION,ISO_RESP_LEN))
		{
			sprintf( strBuffer, "Timeout Recieved, Reversal transaction is not processed");
			sbatch_log_message( 1, 3, strBuffer, Process_Auth_Advice_Response, 0 );
		}
	}
   else
   {
	   sprintf( strBuffer, "Its not a valid response-%s with -%s ",Auth_Tx.TLF01_details.tx_key,Auth_Tx.TLF01_details.message_type);
	   sbatch_log_message( 1, 3, strBuffer, Process_Auth_Advice_Response, 0 );
   }

	return 0;
}


/*******************************************************
 * This function will get the values from tf.ini file
 * from the section DATASERVER with keyname DB_ERROR_STATICS_FLAG &
 *	DB_ERROR_STATICS_VALUE which will help to decide the db statics
 *	logging mechanism.
 * *******************************************************/

void sbatch_get_error_warning_file_name_path(void )
{
   DWORD rc;
   CHAR  filename   [80] = {0};
   CHAR  tmpstr     [80] = {0};
   CHAR  sbatch_error_warning_file_size[5] = {0};
   /* Get path to the tf.ini file. */
   memset( tmpstr, 0x00, sizeof(tmpstr) );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, "tf.ini" );

   /*Read path for creating file in order to log db statics and db oracle error messages */
   rc = GetPrivateProfileString(
								  "DATASERVERS",             /* points to section name         */
								  "DB_STATICS_PATH",       	/* points to key name             */
								   "",  					/* Default string                 */
								   sbatch_error_warning_file_path,              	/* points to destination buffer   */
								   sizeof(sbatch_error_warning_file_path)-1,   	 /* size of destination buffer     */
								   filename                  /* points to ini filename         */
						 	   );

   rc = GetPrivateProfileString(
								  "ERROR_WARNING",             /* points to section name         */
								  AppName,       	/* points to key name             */
								  "",  					/* Default string                 */
								  sbatch_module_error_warning_file_name,              	/* points to destination buffer   */
								  sizeof(sbatch_module_error_warning_file_name)-1,   	 /* size of destination buffer     */
								  filename                  /* points to ini filename         */
								);

   rc = GetPrivateProfileString(
							  "ERROR_WARNING",             /* points to section name         */
							  "ERROR_WARNING_FILE_SIZE",       	/* points to key name             */
							  "500",  					/* Default string                 */
							  sbatch_error_warning_file_size,              	/* points to destination buffer   */
							  sizeof(sbatch_error_warning_file_size)-1,   	 /* size of destination buffer     */
							  filename                  /* points to ini filename         */
							);

	/* File size size conversion */
	Max_File_Size_Defined = atoi(sbatch_error_warning_file_size);
	if(Max_File_Size_Defined <= 0)
	{
		Max_File_Size_Defined = 500 ;
	}
	Max_File_Size_Defined = Max_File_Size_Defined * ONE_MB_IN_BYTES ; /* 1 MB = 1048576 BYTES*/


   if((strlen(sbatch_error_warning_file_path) > 0) &&
	  (strlen(sbatch_module_error_warning_file_name)> 0))
   {
	   sbatch_create_Error_Filename();
   }
}

#ifdef HCTS4
void sbatch_get_max_retry_count()
{
	DWORD rc;
	CHAR  filename   [80] = {0};
	CHAR  tmpstr     [80] = {0};
	CHAR  buffer	 [5]  = {0};

	/* Get path to the tf.ini file. */
	memset( tmpstr, 0x00, sizeof(tmpstr) );
	GetPinnacleConfigDirectory(tmpstr);
	sprintf(filename, "%s%s", tmpstr, "tf.ini" );

	/*Read path for creating file in order to log db statics and db oracle error messages */
	rc = GetPrivateProfileString(
								 "SBATCH",             /* points to section name         */
								 "MAX_RETRY_COUNT",       	/* points to key name             */
								 "3",  					/* Default string                 */
								 buffer,              	/* points to destination buffer   */
								 sizeof(buffer)-1,   	 /* size of destination buffer     */
								 filename                  /* points to ini filename         */
							 	);

	MaxRetryCount = atoi(buffer);
}
#endif

INT sbatch_Log_error_warning_to_File(pCHAR Error_Buf,int sev,pCHAR func, int detail)
{
	INT   ret_val = 0;
	INT   len=0;
	INT   cardlen = 0;
	INT   path_len = 0;
	INT   nIndex = 0;
	CHAR  time_date[25]  ={0};
	CHAR  timestamp[14]  ={0};
	CHAR  current_mmdd[5]={0};
	CHAR Buffer[1024]={0};
	CHAR card_tid_mid_details [256] = {0};
	CHAR tempcard[21] = {0};
	CHAR masked_cardnum[21] = {0};
	char *cardIndex = NULL ;
	FILE *fp;
	UINT file_size = 0 ;
	char buf[256] = {0} ;

	/* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
	ptetime_get_timestamp( time_date );

	/* Pick out the month and day to compare to filename extenstion. */
    memcpy( current_mmdd,   time_date+5, 2 );
    memcpy( current_mmdd+2, time_date+8, 2 );

	/* Compare filename extension to current date. */
    len = strlen(sbatch_Erro_warning_Filename);
    path_len = strlen(sbatch_error_warning_file_path);
    if( len == 0 ||
    	path_len==0 )
    {
    	LogEvent(Error_Buf,INFO_MSG);
    	return 0;
    }
    if ( 0 != strcmp(&sbatch_Erro_warning_Filename[len-4], current_mmdd) )
    {
		/* Now create the new filename.*/
    	sbatch_create_Error_Filename();
    }
	if((fp = fopen(sbatch_Erro_warning_Filename,"a+b"))==NULL)
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

void sbatch_create_Error_Filename(  )
{
	CHAR  system_date[25] = {0};

   /* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
	ptetime_get_timestamp( system_date );

    strcpy( sbatch_Erro_warning_Filename, sbatch_error_warning_file_path );
    strcat( sbatch_Erro_warning_Filename, sbatch_module_error_warning_file_name );
	strcat( sbatch_Erro_warning_Filename, ".EW." );
    strncat(sbatch_Erro_warning_Filename, system_date,   4 );  /* YYYY */
    strncat(sbatch_Erro_warning_Filename, system_date+5, 2 );  /* MM   */
    strncat(sbatch_Erro_warning_Filename, system_date+8, 2 );  /* DD   */
}
/******************************************************************************
 *
 *  NAME:         sbatch_log_message
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
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void sbatch_log_message( INT dest, INT sev, pCHAR msg, pCHAR func, int details )
{
   INT  monitor_flag = 0;
   INT  eventlog_flag = 0;
   INT  msg_type = 0;
   BYTE severity = 0;
   BYTE log_type[20] = {0};
   CHAR text_message [200] = {0};
   INT  Appname_len = 0;
   CHAR  appname[512] = {0};

   GetAppName (appname) ;
   /* Set Monitor flag */
   if ( dest == 1  ||  dest == 3 )
      monitor_flag = 1;
   else
      monitor_flag = 0;

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
   sprintf(text_message,"%s ,",appname);
   Appname_len = strlen(text_message);

   memcpy( text_message + Appname_len, msg, (sizeof(text_message)-1 -Appname_len));
   /* Call function to post the message. */
   sbatch_Log_error_warning_to_File(text_message,sev,func,details);
   if(monitor_flag == 1)
   {
	    TxUtils_Send_Msg_To_Operator( monitor_flag, text_message,
									  eventlog_flag, msg_type, func,
									  severity, log_type,
									 NULL, NULL,NULL );
   }

   return;
}


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

   /* Get path to the tf.ini file. */
   memset( tmpstr, 0x00, sizeof(tmpstr) );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, "tf.ini" );

   /*----------------------------------------*/
   /* Get Data Server error statics  Flag - On or Off */
   /*----------------------------------------*/
   memcpy( timer_flag, sOFF, sizeof(sOFF) );
   rc = GetPrivateProfileString(
            "DATASERVERS",        /* points to section name         */
            "DB_STATICS_FLAG",       /* points to key name             */
             sOFF,                /* Default string (Off)           */
             timer_flag,          /* points to destination buffer   */
             sizeof(timer_flag)-1,/* size of destination buffer     */
             filename             /* points to ini filename         */
   );

   if ( 0 == strcmp(timer_flag, sOFF) )
   {
      /* Timer is Off; no need to get the value. */
	   *ds_timer_flag = DS_TIMER_OFF;
   }
   else if ( 0 == strcmp(timer_flag, sON) )
   {
      /* Timer is On; get the number of seconds for query criteria. */
      *ds_timer_flag = DS_TIMER_ON;
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
   }
   else
   {
      /* The tf.ini file contains an invalid value for the timer flag.
       * Log this fact, set timer off, and exit.
       */
      sprintf( errbuf,
              "%s:Invalid value (%s) in tf.ini file for Data Server DB_ERROR_STATICS_FLAG flag.",
               appname, timer_flag );
      *ds_timer_flag = DS_TIMER_OFF;
      ret_val = false;
   }


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
	strcat(Buffer,Error_Buf);

	strcat(Buffer,"\n");
	len=0;
	len=strlen(Buffer);


	if(fwrite(Buffer, len, NUM_SIZE_WRITES, fp)==NUM_SIZE_WRITES)
	{
		// fall below
	}
	else
	{
		LogEvent(Buffer,INFO_MSG);
	}
	fclose(fp);
	return ;
}
