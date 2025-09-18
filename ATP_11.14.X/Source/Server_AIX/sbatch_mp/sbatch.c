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
* APPLICATION:      sbatmp.exe
*
* AUTHOR:          Abhishek Kumar Verma
*
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
static int           sale_tran_found_0200 = 0;

BYTE  DB_MsgSubtype2 ;
BYTE  DB_MsgSubtype1 ;
CHAR  DB_Logging_Filename[256];
CHAR  DB_module_file_name[256];
CHAR  DB_file_path[256];
/* End   */
/* This variable will get the value from the tf.ini
 * from the section DATASERVER with key name DB_ERROR_STATICS_FLAG
 * */
INT    db_error_statics_flag;
double db_error_statics_value;
INT DB_NullCheck_Flag = 0;
#define MODE_INSERT "INSERT"
#define MODE_UPDATE "UPDATE"

UINT Max_File_Size_Defined = 0 ;

//static TLF01			Auth_Tx;
TLF01	tlf01_tmp_struct, db_tlf01_struct,Auth_Tx_TLFstip;
AUTH_TX Auth_Tx,Auth_Tx_BCH10;
BCH10 bch10;
BCH11 bch11;
pTLF01 tlf01_ptr;
pNCF01	ncf01_ptr;
NCF01 ncf01_tmp_struct;
BYTE db_select_ncf01 (char[], pNCF01 , char*);
/* declared variable in .C file TF-Ajay 23 Dec 2009 */
char    NetCntlName[12];
char	authorizing_host_queue[9];
CHAR	TxnDate[9];
CHAR    AppName   [MAX_APP_NAME_SIZE];
char    nciftr_timer_value[4] = {0};
INT 	sbatmp_timer_value = 0;
INT     sbatmp_advice_repeat_timer = 0;
INT     sbatmp_max_retry_count = 0;
INT     sbatmp_max_retry_erro_count= 0;
CHAR  sbatmp_Erro_warning_Filename[256] = {0};
CHAR  sbatmp_module_error_warning_file_name[256] = {0};
CHAR  sbatmp_error_warning_file_path[256] = {0};

/* Currency Codes ..TF Phani*/
#define  PESO     "608"
#define  USD      "840"


#ifdef WIN32
   #define DEFAULT_DIRECTORY  "\\ascendent\\sbatmp"
#else
   #define DEFAULT_DIRECTORY "/ascendent/sbatmp"
#endif

BYTE db_get_tlf01_stip_standin_rec(char *, pTLF01 , char *);

BYTE p_buffer [MAX_DATASERVER_BUFFER_LENGTH] ; /* defined to be 25000 */


CHAR  Version[] = "ATP_11.10.0";


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
		/* If date not provided, It will read system currnet date 17 Dec,2009 TF-Ajay*/
		if(argv[2]!=NULL)
			strcpy(TxnDate, argv[2]);
		
    }
	else
	{
		strcpy(NetCntlName,"");
	    strcpy(TxnDate,"");
	}

   if ((rc = MainProcessor()) != 0)
	   printf("Error Running sbatmp: send standin batch to host program \n");
	
	Stop();

} /* main */


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
	CHAR time_date[25];
	INT      ret_val = 0;
	CHAR     Buffer[256] = {0};

	memset (&Auth_Tx, 0, sizeof Auth_Tx) ;


	GetAppName(AppName) ;
	
    strcpy(ServiceName, AppName);
    sbatmp_get_error_warning_file_name_path();
	/* Display Application version. */
	sprintf(strTemp, "Attempting to start %s, version %s", ServiceName, Version);
	sbatmp_log_message( 2, 1, strTemp, "MainProcessor", 0 );
	printf("Attempting to start %s, version %s\n", ServiceName, Version);

	#ifdef _DEBUG
	pteipc_trace_on(); 
	#endif
	
	/* Create queues sbatmpA, sbatmpI, sbatmpC */
	if( !pteipc_init_single_instance_app( AppName, "pte_ipc" ) )
	{	
		sprintf (strTemp,
		  "%s: MainProcessor: Failed to Create XIPC queues",
		   ServiceName) ;
		sbatmp_log_message( 1, 3, strTemp, "MainProcessor", 0 );
		/* signal that we are ready to shut down and quit */
		pteipc_shutdown_single_instance_app() ;
		MainProcessDone = 1 ;
		return 1;
	}
	if( (0 != strncmp(NetCntlName,"nciftr",6)))
	{
		sprintf(strTemp,
				"nvalid network module %s Service for %s Host",
				NetCntlName, ServiceName);
		sbatmp_log_message( 1, 2, strTemp, "MainProcessor", 0 );
		printf("Invalid network module %s Host\n",NetCntlName);
		shut_down();
		exit(0);
	}
	sprintf(strTemp,
	  "Started the %s Service for %s Host",
	   NetCntlName, ServiceName);
	sbatmp_log_message( 2, 1, strTemp, "MainProcessor", 0 );
	printf("Started the sbatmp Service for %s Host\n",NetCntlName);

	ret_val = get_dataserver_ini_db_error_statics_value( &db_error_statics_flag, &db_error_statics_value,
	                                                      ServiceName,     Buffer );

	/* Check that NetCntlName is not NULL */
	if (strcmp(NetCntlName, "") == 0)
	{
		strcpy(msg, "Please enter a valid Network/Host Controller Name ");
		sbatmp_log_message( 1, 1, msg, "MainProcessor", 0 );
		#ifdef _DEBUG
			scanf("%s", &NetCntlName);
		#else
		display_log_info();
		shut_down();
		return 1;
		#endif
	}
	/* Changes done in sbatmp to retrieve txn irresepctive of date- Phani 25 march */ 
	/* Check that, if Date is  NULL, Put the msg */
	if (strcmp(TxnDate, "") == 0)
		{
			strcpy(msg, "User not provided Date for sbatmp, considering all tx irrespective of date");
			sbatmp_log_message( 1, 1, msg, "MainProcessor", 0 );
			/* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
          //  ptetime_get_timestamp( time_date );

            /* Reformat to get date "YYYYMMDD". */
           // get_date( time_date, TxnDate);            
		}



	/* Try to connect to the database */
	if (dbcommon_connect("equitable", "equitable", "equitable", "ORCL", msg) != PTEMSG_OK)
	{
		strcpy(msg, "Unable to Connect to Oracle ");
		sbatmp_log_message( 1, 3, msg, "MainProcessor", 0 );
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
   sbatmp_log_message( 1, 1, msg, "display_error_message", 0 );
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
   printf("sbatmp - version %s\n\n", Version );
   printf("The Standin Batch To Host program sends all the ATP standin    \n");
   printf("approved transactions to host when the host link is down. If a \n");
   printf("valid Network Controller name and Date of transaction are specified on the command line,\n");
   printf("all the transactions that were processed by ATP for that Network for purticular given date\n");
   printf("Host are sent resent to the specified host. \n");
   printf(" sbatmp  - Network Controller Name is  Mandatory and Date is optional, \n\n");
   printf(" If Date not provide. sbatmp will read System current date, \n\n");
   printf(" Example: sbatmp  nciftr 20091210-  this would send");
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
         sbatmp_log_message( 1, 3, strTemp, "Check_Is_Host_Up", 0 );

      }
      else
      {
         /* Forward request to network controller
          * if the network controller is Online.
          */

		  if ( strEqual(state, ONLINE) )
            ret_val = true;
      }
   }
   return( ret_val );

} /* TxService_Is_Host_Up */
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
	char              msg[256]={0};
	char			  strBuffer[256]={0};
	BYTE              rc;
	pPTE_MSG p_msg = 0 ;int  dest_len=0;
	LONG     lReturnCode = 0L ;
	int               next_portion = 0;
	INT      err_ctr = 0;
	CHAR     strTemp [256] = {0} ;
	 BYTE           tranid[21]={0};
     EMV_RAW        emvraw;
	 BYTE			emv_result_code;
	 char    strTemtidlog[1000]={0};
	memset(&Auth_Tx, 0, sizeof(Auth_Tx)); 
	memset(&tlf01_tmp_struct,0,sizeof(TLF01));
	memset(&ncf01_tmp_struct,0,sizeof(NCF01));
	
	rc = db_select_ncf01(NetCntlName,&ncf01_tmp_struct, msg);
	if ( rc == PTEMSG_OK)
	{
		advice_repeat_ctr = atoi(ncf01_tmp_struct.advice_repeat_counter);
		printf("Obtained advice_repeat_counter from NCF01: %d\n", advice_repeat_ctr);
	}
	else
	{
		printf("The Oracle error number to get NCF01.advice_repeat_counter %d::%s \n",dbcommon_oracle_to_pte_rcode( rc, msg), msg);
		printf("Setting the advice_repeat_counter to 3\n");
		
	}

	// Lets store nciftr timer value and advice count.
	strncpy( nciftr_timer_value,ncf01_tmp_struct.request_timer,
			(sizeof(nciftr_timer_value)-1));
	sbatmp_timer_value = atoi(nciftr_timer_value) + 5;
	sbatmp_advice_repeat_timer = (sbatmp_timer_value * advice_repeat_ctr);
	sbatmp_max_retry_count = advice_repeat_ctr;

	//Check for host status

	if(Check_Is_Host_Up(NetCntlName) == false)
	{
			printf("sbatmp: NC is not ONLINE, Plz make network ONLINE and START again : Network controller-  %s",NetCntlName);
			sprintf(msg,"sbatmp: NC is not ONLINE, Plz make network ONLINE and START again : Network controller-  %s",NetCntlName);
			sbatmp_log_message( 2, 1, msg, "ProcessTransactions", 0 );
			display_error_message(msg,"ProcessTransactions");
			return;
	}
	strncpy(Auth_Tx.TLF01_details.handler_queue,"sbatmpA",7);
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
			sale_tran_found_0200 = 0;
			memcpy (&Auth_Tx.TLF01_details, &tlf01_tmp_struct, sizeof (TLF01)) ;
			memset(strTemtidlog, 0x00 , sizeof(strTemtidlog));
			sprintf(strTemtidlog,"Sbatch MP TRID: %s Batch num: %s",
					Auth_Tx.TLF01_details.primary_key.transaction_id,
					Auth_Tx.TLF01_details.batch_number);
			sbatmp_log_message( 2, 1, strTemtidlog, "ProcessTransactions", 1 );
			printf("%s\n",strTemtidlog);
			/*	We have to handle MP/PL 500 Message separtly. */
			if( (0 == strncmp(authorizing_host_queue,"nciftr",6)) &&
				(0 == strncmp(Auth_Tx.TLF01_details.message_type,"05",2)))
			{
				/*
					Send 500 message to nciftr to forward to candencie
				*/
				dest_len = strlen(authorizing_host_queue);
				if (authorizing_host_queue[dest_len - 1] == 'A')
				{
					//Do nothing
				}
				else
				{
					strcat(authorizing_host_queue,"A");
				}
				strncpy(Auth_Tx.TLF01_details.handler_queue,"sbatmpA",7);
				if(Auth_Tx.TLF01_details.tx_key> 100)
				{
					Auth_Tx.TLF01_details.tx_key = Auth_Tx.TLF01_details.tx_key - 100;
				}
				lReturnCode = Send_Request_To_Service (  authorizing_host_queue,
									MT_AUTH_REQUEST, 
									0, 
									0, 
									0, 
									(pBYTE) (&Auth_Tx), 
									sizeof(AUTH_TX));
				while (err_ctr <= advice_repeat_ctr )
				{
					/* You are blocked here waiting for a message on either app queue or
					* control que.  If there is no message on either que for 5 seconds,
					* the blocking call returns.  Application_que_name and control_que_name
					* are populated in pteipc_init_multiple_instance_app (pteipc.c).
					*/
					p_msg = pteipc_receive( application_que_name, control_que_name,
							sbatmp_timer_value, &lReturnCode);

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
						sbatmp_log_message( 1, 3, strBuffer, "ProcessTransactions", 0 );
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
							sbatmp_log_message( 1, 3, strBuffer, "ProcessTransactions", 0 );
							#ifdef WIN32
								Sleep (50000);
							#else
								usleep (50000000);
							#endif
						}
					}
					if( err_ctr >= advice_repeat_ctr)
					{
						sprintf( msg, "Reached maximum retry count for Batch upload, No Repsone from Cadience ");
						display_error_message(msg,"ProcessTransactions");
						shut_down();
						exit(0);
					}
				}//while advice_repeat_ctr
				
			}
			else if( (0 == strncmp(authorizing_host_queue,"nciftr",6)) &&
				(0 == strncmp(Auth_Tx.TLF01_details.message_type,"03",2)))
			{
				/*
					Send 0320 message to nciftr to forward to candencie
				*/
				dest_len = strlen(authorizing_host_queue);
				if (authorizing_host_queue[dest_len - 1] == 'A')
				{
					//Do nothing
				}
				else
				{
					strcat(authorizing_host_queue,"A");
				}
				if(Auth_Tx.TLF01_details.tx_key> 100)
				{
					Auth_Tx.TLF01_details.tx_key = Auth_Tx.TLF01_details.tx_key - 100;
				}
				strncpy(Auth_Tx.TLF01_details.handler_queue,"sbatmpA",7);
				lReturnCode = Send_Request_To_Service (  authorizing_host_queue,
									MT_AUTH_REQUEST, 
									0, 
									0, 
									0, 
									(pBYTE) (&Auth_Tx), 
									sizeof(AUTH_TX));
				while (err_ctr <= advice_repeat_ctr )
				{
					/* You are blocked here waiting for a message on either app queue or
					* control que.  If there is no message on either que for 5 seconds,
					* the blocking call returns.  Application_que_name and control_que_name
					* are populated in pteipc_init_multiple_instance_app (pteipc.c).
					*/
					p_msg = pteipc_receive( application_que_name, control_que_name,
							sbatmp_timer_value, &lReturnCode);

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
						sbatmp_log_message( 1, 3, strBuffer, "ProcessTransactions", 0 );
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
							sbatmp_log_message( 1, 3, strBuffer, "ProcessTransactions", 0 );
							#ifdef WIN32
								Sleep (50000);
							#else
								usleep (50000000);
							#endif
						}
					}
					if( err_ctr >= advice_repeat_ctr)
					{
						sprintf( msg, "Reached maximum retry count for Batch upload, No Repsone from Cadience ");
						display_error_message(msg,"ProcessTransactions");
						shut_down();
						exit(0);
					}
				}//while advice_repeat_ctr
			}
			else
			{
				strcpy(Auth_Tx.TLF01_details.orig_message,Auth_Tx.TLF01_details.message_type);
				if(strlen(Auth_Tx.TLF01_details.orig_retrieval_ref_num) == 0)
				{
					strcpy(Auth_Tx.TLF01_details.orig_retrieval_ref_num, Auth_Tx.TLF01_details.retrieval_ref_num);
				}
				strcpy(Auth_Tx.TLF01_details.retrieval_ref_num,"");
				if (0 != strncmp(Auth_Tx.TLF01_details.product_codes[1].code,STANDIN,6))
				{
					// This can be Void or reversal, but we have send as reversal.
					Auth_Tx.TLF01_details.tx_key = AUTH_REVERSAL_ADVICE;
					strcpy(Auth_Tx.TLF01_details.message_type,"0420");
				}
				else
				{
				Auth_Tx.TLF01_details.tx_key = AUTH_ADVICE;
			
				if((strcmp(Auth_Tx.TLF01_details.message_type,"0120") == 0))
					strcpy(Auth_Tx.TLF01_details.message_type,"0121");
				else
				strcpy(Auth_Tx.TLF01_details.message_type,"0120");
				}
				strcpy(Auth_Tx.TLF01_details.originator_queue,application_que_name);
				strcpy(Auth_Tx.TLF01_details.handler_queue, application_que_name);
				strcpy(Auth_Tx.TLF01_details.response_code,"");
				/*	strcpy(Auth_Tx.TLF01_details.message_type,"0120");  This line no need TF, Ajay*/
				strcpy(Auth_Tx.TLF01_details.response_text,"");
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
				err_ctr = 0;
				printf("Processing transaction with tlf01_stip ID: %s\n", Auth_Tx.TLF01_details.primary_key.transaction_id);
				lReturnCode = Send_Request_To_Service (  authorizing_host_queue,
												MT_AUTH_REQUEST, 
												0, 
												0, 
												0, 
												(pBYTE) (&Auth_Tx), 
												sizeof(AUTH_TX)) ;
				while (err_ctr <= advice_repeat_ctr )
				{
					/* You are blocked here waiting for a message on either app queue or
					* control que.  If there is no message on either que for 5 seconds,
					* the blocking call returns.  Application_que_name and control_que_name
					* are populated in pteipc_init_multiple_instance_app (pteipc.c).
					*/
					p_msg = pteipc_receive( application_que_name, control_que_name,
							sbatmp_timer_value, &lReturnCode);

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
						if (0 != strncmp(Auth_Tx.TLF01_details.product_codes[1].code,STANDIN,6))
						{
							// This can be Void or reversal, but we have send as reversal.
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
						sbatmp_log_message( 1, 3, strBuffer, "ProcessTransactions", 0 );
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
							sbatmp_log_message( 1, 3, strBuffer, "ProcessTransactions", 0 );
							#ifdef WIN32
								Sleep (50000);
							#else
								usleep (50000000);
							#endif
						}
					}
					if( err_ctr >= advice_repeat_ctr)
					{
						sprintf( msg, "Reached maximum auth advice retry count, No Repsone from Cadience ");
						display_error_message(msg,"ProcessTransactions");
						//shut_down();
						//exit(0);
					}
				}//while advice_repeat_ctr
			}/*End of else*/
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
		       sbatmp_log_message(1, 3, msg, "ProcessTransactions", 0);
			   next_portion = 1;
			   continue;
		   }
		   else
		   {
			   sbatmp_log_message(1, 1, "DataBase Recoonected", "ProcessTransactions", 0);
		   }
		 }
		else
		{/* TF Phani.. Break the loop even for DB errors*/
			next_portion = 1;
			continue;
		
		}

	} // end while - next_portion
	if(total_tx_num == 0)
	{
			sprintf( msg, "Completed Process Transactions for %s. Could not find any records", NetCntlName );
			sbatmp_log_message( 2, 1, msg, "ProcessTransactions", 0 );
	}
	else
	{		
			sprintf(msg,"sbatmp processed %ld number of transactions for %s ", total_tx_num,NetCntlName);
			display_error_message(msg,"ProcessTransactions");
			sbatmp_log_message( 2, 1, msg, "ProcessTransactions", 0 );
	}
	printf("%s\n",msg);
	display_error_message(msg,"ProcessTransactions");

} // ProcessTransactions 



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
   sprintf( Buffer,"Stopping sbatmp program, version %s",Version);
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
		sbatmp_log_message( 1, 3, strError, "Send_Request_To_Service", 0 );
		return SYSTEM_ERROR ;
 	} 

	retCode = pteipc_send (p_msg_out, que_name) ;
	free (p_msg_out) ;

	if (retCode != 0)
	{
		sprintf (strError,"Error %ld has occurred sending message to %s\n", retCode, que_name) ;
        sbatmp_log_message( 1, 3, strError, "Send_Request_To_Service", 0 );
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
          sbatmp_log_message( 1, 3, strTemp, "PinnacleMsgHandler", 0 );
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
	char strBuffer[256]={0};
	int next_data=0;
	int error_batch_status = 0;

	pATEMessageData			= ptemsg_get_pte_msg_data (p_msg_in) ;
	pATEMessageDataData		= ptemsg_get_pte_msg_data_data (pATEMessageData) ;
	
	/*******************************************************************/
    memset (&Auth_Tx, 0, sizeof Auth_Tx) ;
	memcpy (&Auth_Tx, (AUTH_TX *)pATEMessageDataData, sizeof Auth_Tx) ;
	/*******************************************************************/
	sprintf (strTemp,"Received msg from host-RespCode:%s  ",
		      Auth_Tx.TLF01_details.response_code) ;

	sprintf(strBuffer,"Received response form nciftr TRID %s stan:%s Key:%d message_type:%s and response code:%s",
			Auth_Tx.TLF01_details.primary_key.transaction_id,
			Auth_Tx.TLF01_details.sys_trace_audit_num,
			Auth_Tx.TLF01_details.tx_key,
			Auth_Tx.TLF01_details.message_type,
			Auth_Tx.TLF01_details.response_code);
	sbatmp_log_message( 2, 1, strBuffer, "Process_Auth_Advice_Response", 1 );
    if( (0== strncmp(Auth_Tx.TLF01_details.response_code,"01",2)) ||
		(0== strncmp(Auth_Tx.TLF01_details.response_code,"91",2)))
	{
		 sbatmp_max_retry_erro_count = 0;
		 memset(strBuffer,0x00,sizeof(strBuffer));
		 sprintf( strBuffer, "Host is not online sbatmp is going down");
		 sbatmp_log_message( 1, 3, strBuffer, "Process_Auth_Advice_Response", 1 );
		 shut_down();
		 exit(0);

	 }
	 else if (Auth_Tx.TLF01_details.tx_key==AUTH_ADVICE_RESPONSE ||
			  Auth_Tx.TLF01_details.tx_key==AUTH_ADVICE_REPEAT_RESPONSE)
	 {
		 if (0 == strncmp(Auth_Tx.TLF01_details.response_code,"19",2))
		 {
			 memset(strBuffer,0x00,sizeof(strBuffer));
			 sprintf( strBuffer, "Received timed out RC %s for Advice transaction,sbatmp will wait for %d before retry,",
					  Auth_Tx.TLF01_details.response_code,sbatmp_advice_repeat_timer);
			 sbatmp_log_message( 1, 3, strBuffer, "Process_Auth_Advice_Response", 1 );
			 sbatmp_max_retry_erro_count = sbatmp_max_retry_erro_count + 1;
			 if(sbatmp_max_retry_erro_count > sbatmp_max_retry_count)
			 {
				 memset(strBuffer,0x00,sizeof(strBuffer));
				 sprintf( strBuffer, "Maximum Retry reached after advice timeout, sbatmp will go down");
				 sbatmp_log_message( 1, 3, strBuffer, "Process_Auth_Advice_Response", 1 );
				 shut_down();
				 exit(0);
			 }
			 else
			 {
				 memset(strBuffer,0x00,sizeof(strBuffer));
				 sleep(sbatmp_advice_repeat_timer);
				 sprintf( strBuffer, "Retry after %d Sec for Advice transaction,",sbatmp_advice_repeat_timer);
				 sbatmp_log_message( 1, 3, strBuffer, "Process_Auth_Advice_Response", 1 );
			 }
		 }
		 else
		 {
			memset( &db_tlf01_struct, 0, sizeof( db_tlf01_struct ) );
			memcpy( &db_tlf01_struct, ( pTLF01 )pATEMessageDataData, sizeof( TLF01 ) );
				sbatmp_max_retry_erro_count = 0;
			strcpy(Auth_Tx.TLF01_details.product_codes[1].code,PROCESSED);
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
	 }

	 else if(Auth_Tx.TLF01_details.tx_key == AUTH_VOID_SALE_RESPONSE ||
			 Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_ADVICE_RESPONSE ||
			 Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_ADVICE_REPEAT_RESPONSE ||
			 Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_RESPONSE ||
			 Auth_Tx.TLF01_details.tx_key==AUTH_REVERSAL_REPEAT_RESPONSE)
	 {
		 if (0 == strncmp(Auth_Tx.TLF01_details.response_code,"19",2))
		 {
			 memset(strBuffer,0x00,sizeof(strBuffer));
			 sprintf( strBuffer, "Received timed out RC %s for reversal transaction,sbatmp will wait for %d before retry,",
					  Auth_Tx.TLF01_details.response_code,sbatmp_advice_repeat_timer);
			 sbatmp_log_message( 1, 3, strBuffer, "Process_Auth_Advice_Response", 1 );
			 sbatmp_max_retry_erro_count = sbatmp_max_retry_erro_count + 1;
			 if(sbatmp_max_retry_erro_count > sbatmp_max_retry_count)
			 {
				 memset(strBuffer,0x00,sizeof(strBuffer));
				 sprintf( strBuffer, "Maximum Retry reached after reversal timeout , sbatmp will go down");
				 sbatmp_log_message( 1, 3, strBuffer, "Process_Auth_Advice_Response", 1 );
				 shut_down();
				 exit(0);
			 }
			 else
			 {
				 memset(strBuffer,0x00,sizeof(strBuffer));
				 sleep(sbatmp_advice_repeat_timer);
				 sprintf( strBuffer, "Retry after %d Sec for reversal transaction,",sbatmp_advice_repeat_timer);
				 sbatmp_log_message( 1, 3, strBuffer, "Process_Auth_Advice_Response", 1 );
			 }
		 }
		 else
		 {
			memset(strBuffer,0x00,sizeof(strBuffer));
				sbatmp_max_retry_erro_count = 0;
			sprintf(strBuffer,"Response received for Auth Reversal advicee");
			sbatmp_log_message( 2, 1, strBuffer, "Process_Auth_Advice_Response", 1 );
			memset( &db_tlf01_struct, 0, sizeof( db_tlf01_struct ) );
			memcpy( &db_tlf01_struct, ( pTLF01 )pATEMessageDataData, sizeof( TLF01 ) );
			strcpy(Auth_Tx.TLF01_details.product_codes[1].code,PROCESSED);
			 if (DB_NullCheck_Flag==1)
			 {
				CheckNULLTerminatedinTLF01 (&Auth_Tx.TLF01_details,MODE_UPDATE);
			 }
			ReturnCode = db_update_tlf01_stip( &Auth_Tx.TLF01_details, strTemp );
			if(strcmp(Auth_Tx.TLF01_details.voice_auth,"Y")==0)
			{
				ReturnCode = db_update_bch20_stip(&Auth_Tx.TLF01_details, strTemp );
			}
			else
			{
				ReturnCode = db_update_bch10_stip(&Auth_Tx.TLF01_details, strTemp );
			}
		 }
	 }

	else if ( 0 == strncmp(Auth_Tx.TLF01_details.message_type,"05",02))
	{
		
		memset( &db_tlf01_struct, 0, sizeof( db_tlf01_struct ) );
		memcpy( &db_tlf01_struct, ( pTLF01 )pATEMessageDataData, sizeof( TLF01 ) );
		if (0 == strncmp(Auth_Tx.TLF01_details.response_code,"19",2))
		{
			 memset(strBuffer,0x00,sizeof(strBuffer));
			 sprintf( strBuffer, "Received timed out RC %s for settlement transaction,sbatmp will wait for %d before retry,",
						 Auth_Tx.TLF01_details.response_code,sbatmp_advice_repeat_timer);
			 sbatmp_log_message( 1, 2, strBuffer, "Process_Auth_Advice_Response", 1 );
			 sbatmp_max_retry_erro_count = sbatmp_max_retry_erro_count + 1;
			 if(sbatmp_max_retry_erro_count > sbatmp_max_retry_count)
			 {
				 memset(strBuffer,0x00,sizeof(strBuffer));
				 sprintf( strBuffer, "Maximum Retry reached after settlement timeout, sbatmp will go down");
				 sbatmp_log_message( 1, 2, strBuffer, "Process_Auth_Advice_Response", 1 );
				 shut_down();
				 exit(0);
			 }
			 else
			 {
				 sleep(sbatmp_advice_repeat_timer);
				 memset(strBuffer,0x00,sizeof(strBuffer));
				 sprintf( strBuffer, "Retry after %d Sec for settlement transaction,",sbatmp_advice_repeat_timer);
				 sbatmp_log_message( 1, 1, strBuffer, "Process_Auth_Advice_Response", 1 );
			 }
		}
		else if( (0== strncmp(Auth_Tx.TLF01_details.response_code,"00",2)))
		{
			// We have to update the batch status
			sbatmp_max_retry_erro_count = 0;
			memset(strBuffer,0x00,sizeof(strBuffer));
			sprintf(strBuffer,"Response received for successful settelment");
			sbatmp_log_message( 2, 1, strBuffer, "Process_Auth_Advice_Response", 1 );
			strcpy(Auth_Tx.TLF01_details.product_codes[1].code,PROCESSED);
			 if (DB_NullCheck_Flag==1)
			 {
				CheckNULLTerminatedinTLF01 (&Auth_Tx.TLF01_details,MODE_UPDATE);
			 }
			ReturnCode = db_update_tlf01_stip_500_success( &Auth_Tx.TLF01_details, strTemp );
			//ReturnCode = db_update_bch01_mp_batch_status( &Auth_Tx.TLF01_details, strTemp );
			printf("Response received for Settlement Transaction \n");
		}
		else if( 0== strncmp(Auth_Tx.TLF01_details.response_code,"95",2))
		{
			memset(strBuffer,0x00,sizeof(strBuffer));
			sbatmp_max_retry_erro_count = 0;
			sprintf(strBuffer,"Reconciliation  response  for settlement ");
			sbatmp_log_message( 2, 1, strBuffer, "Process_Auth_Advice_Response", 1 );
			strcpy(Auth_Tx.TLF01_details.product_codes[1].code,PROCESSED);
			 if (DB_NullCheck_Flag==1)
			 {
				CheckNULLTerminatedinTLF01 (&Auth_Tx.TLF01_details,MODE_UPDATE);
			 }
			// Lets Mark the 0500 as processed. We will use this 0500 to send again if ATP is doing batch and
			db_update_tlf01_stip_500_after_RC_95( &Auth_Tx.TLF01_details, strTemp );

		    /*Let us check do we have any 320 transaction with batch.*/
			ReturnCode=db_select_tlf01_stip_320_msg( &Auth_Tx.TLF01_details, strTemp );
			if( ReturnCode == PTEMSG_OK )
			{
				memset(strBuffer,0x00,sizeof(strBuffer));
				sprintf(strBuffer,"Batch uload will start");
				sbatmp_log_message( 2, 1, strBuffer, "Process_Auth_Advice_Response", 1 );
				 if (DB_NullCheck_Flag==1)
				 {
					CheckNULLTerminatedinTLF01 (&Auth_Tx.TLF01_details,MODE_UPDATE);
				 }
				ReturnCode = db_update_tlf01_stip_320_found( &Auth_Tx.TLF01_details, strTemp );
				// We have batch upload for this batch.
				// No need to do any thing. sbatmp will send next transaction as batch upload
			}
			else
			{
				memset(strBuffer,0x00,sizeof(strBuffer));
				sprintf( strBuffer, "NO 320 found in TLF01_STIP table");
				sbatmp_log_message( 2, 1, strBuffer, "Process_Auth_Advice_Response", 1 );
				// We did not found batch upload , we need to pull all 200 message and send it as batch upload.
				ReturnCode = db_get_bch10_stip_service_list( &Auth_Tx.TLF01_details, strTemp );
				if(ReturnCode != PTEMSG_OK)
				{
					//Record details are not in bch10 or partial records are present, lets check in bch11
					ReturnCode = db_get_bch11_stip_service_list( &Auth_Tx.TLF01_details, strTemp );
				}
                printf(" lets find settlement transaction for this batch ");
				ReturnCode = db_get_tlf01_settlement_trn( &Auth_Tx.TLF01_details, strTemp );
				if( ReturnCode == PTEMSG_OK)
				{
					if(sale_tran_found_0200 == 1)
					{
						memset(strBuffer,0x00,sizeof(strBuffer));
						sprintf( strBuffer, "Send Settlment Transaction after batch upload");
						sbatmp_log_message( 2, 1, strBuffer, "Process_Auth_Advice_Response", 1 );
						// We will send 500 to close this batch.
						sale_tran_found_0200 = 0;
						tlf01_stip_Settlement_handler();

					}
					else
					{
						// No 0200 records were present in bch10. bch11 table marked as batch error
						memset(strBuffer,0x00,sizeof(strBuffer));
						sprintf( strBuffer, "No 0200  records in BCH10/BCH11, Mark batch up batchE");
						sbatmp_log_message( 2, 1, strBuffer, "Process_Auth_Advice_Response", 1 );
						sale_tran_found_0200 = 0;
						 if (DB_NullCheck_Flag==1)
						 {
							CheckNULLTerminatedinTLF01 (&Auth_Tx.TLF01_details,MODE_UPDATE);
						 }
						db_update_tlf01_stip_320_not_found( &Auth_Tx.TLF01_details, strTemp );
					}
				}
				else
				{
					memset(strBuffer,0x00,sizeof(strBuffer));
					sprintf(strBuffer,"Unable to send Settlment Transaction for terminal_id %s merchant_id %s batch_number %s",
							Auth_Tx.TLF01_details.terminal_id,
							Auth_Tx.TLF01_details.merchant_id,
							Auth_Tx.TLF01_details.batch_number);
					sbatmp_log_message( 1, 1, strBuffer, "Process_Auth_Advice_Response", 0 );
				}
			}
		}
		else if( 0  == strncmp(Auth_Tx.TLF01_details.response_code,"80",2))
		{

			sbatmp_max_retry_erro_count = 0;
			sprintf( strBuffer, "RC received is 80");
			sbatmp_log_message( 2, 1, strBuffer, "Process_Auth_Advice_Response", 0 );
			strcpy(Auth_Tx.TLF01_details.response_text,"Merchant is RESTRICTED");
			if (DB_NullCheck_Flag==1)
			{
				CheckNULLTerminatedinTLF01 (&Auth_Tx.TLF01_details,MODE_UPDATE);
			}
			ReturnCode = db_update_tlf01_stip_RC_80( &Auth_Tx.TLF01_details, strTemp );

			/*********************RC 80 previoue batch uploadcode commented as per BDO request ********
			sbatmp_max_retry_erro_count = 0;
			sprintf( strBuffer, "Lets Pull 0200 message for batch num %s",Auth_Tx.TLF01_details.batch_number);
			sbatmp_log_message( 2, 1, strBuffer, "Process_Auth_Advice_Response", 1 );
			ReturnCode=db_select_tlf01_stip_0200_msg( &Auth_Tx.TLF01_details, strTemp );
			if( ReturnCode == PTEMSG_OK )
			{
				printf("ATP will send 0200 message for batch %s\n", Auth_Tx.TLF01_details.batch_number);
				ReturnCode = db_get_TLF10_stip_0200_service_list( &Auth_Tx.TLF01_details, strTemp );
			}
			else
			{
					sprintf( strBuffer, "NO 0200 found in TLF01_STIP table for batch %s",
							Auth_Tx.TLF01_details.batch_number);
					sbatmp_log_message( 2, 2, strBuffer, "Process_Auth_Advice_Response", 1 );
					 if (DB_NullCheck_Flag==1)
					 {
						CheckNULLTerminatedinTLF01 (&Auth_Tx.TLF01_details,MODE_UPDATE);
					 }
					ReturnCode = db_update_tlf01_stip_0200_not_found( &Auth_Tx.TLF01_details, strTemp );
			}*/
		}
		else if( 0== strncmp(Auth_Tx.TLF01_details.response_code,"03",2))
		{
			 sbatmp_max_retry_erro_count = 0;
			 sprintf( strBuffer, "Merchant with MID %s is RESTRICTED",Auth_Tx.TLF01_details.merchant_id);
			 sbatmp_log_message( 2, 1, strBuffer, "Process_Auth_Advice_Response", 0 );
			 strcpy(Auth_Tx.TLF01_details.response_text,"Merchant is RESTRICTED");
			 if (DB_NullCheck_Flag==1)
			 {
				CheckNULLTerminatedinTLF01 (&Auth_Tx.TLF01_details,MODE_UPDATE);
			 }
		    ReturnCode = db_update_tlf01_stip_MID_RESTRICTED( &Auth_Tx.TLF01_details, strTemp );

		}
		else 
		{
			 sbatmp_max_retry_erro_count = 0;
			 sprintf( strBuffer, "Its not a valid response- %s  date %s with -%s ",
					 Auth_Tx.TLF01_details.response_code,
					 Auth_Tx.TLF01_details.message_type,
					 Auth_Tx.TLF01_details.date_yyyymmdd
					 );
			 sbatmp_log_message( 1, 1, strBuffer, "Process_Auth_Advice_Response", 1 );
			 strcpy(Auth_Tx.TLF01_details.response_text,"Invalid Response");
			 if (DB_NullCheck_Flag==1)
			 {
				CheckNULLTerminatedinTLF01 (&Auth_Tx.TLF01_details,MODE_UPDATE);
			 }
		    ReturnCode = db_update_tlf01_stip_INVALID_RSP( &Auth_Tx.TLF01_details, strTemp );
		}
	}
	else if ( 0 == strncmp(Auth_Tx.TLF01_details.message_type,"03",02))
	{
		if (0 == strncmp(Auth_Tx.TLF01_details.response_code,"19",2))
		{
			 memset(strBuffer,0x00,sizeof(strBuffer));
			 sprintf( strBuffer, "Received timed out RC %s for batch upload transaction,sbatmp will wait for %d before retry,",
						 Auth_Tx.TLF01_details.response_code,sbatmp_advice_repeat_timer);
			 sbatmp_log_message( 1, 1, strBuffer, "Process_Auth_Advice_Response", 1 );
			 sbatmp_max_retry_erro_count = sbatmp_max_retry_erro_count + 1;
			 if(sbatmp_max_retry_erro_count > sbatmp_max_retry_count)
			 {
				 memset(strBuffer,0x00,sizeof(strBuffer));
				 sprintf( strBuffer, "Maximum Retry reached after batch upload timeout, sbatmp will go down");
				 sbatmp_log_message( 1, 1, strBuffer, "Process_Auth_Advice_Response", 1 );
				 shut_down();
				 exit(0);
			 }
			 else
			 {
				 memset(strBuffer,0x00,sizeof(strBuffer));
				 sleep(sbatmp_advice_repeat_timer);
				 sprintf( strBuffer, "Retry after %d Sec for batch upload transaction,",sbatmp_advice_repeat_timer);
				 sbatmp_log_message( 1, 1, strBuffer, "Process_Auth_Advice_Response", 1 );
			 }
		}
		else
		{
			sbatmp_max_retry_erro_count = 0;
			printf("Response received for batch upload Transaction");
			memset( &db_tlf01_struct, 0, sizeof( db_tlf01_struct ) );
			memcpy( &db_tlf01_struct, ( pTLF01 )pATEMessageDataData, sizeof( TLF01 ) );
			strcpy(Auth_Tx.TLF01_details.product_codes[1].code,PROCESSED);
			 if (DB_NullCheck_Flag==1)
			 {
				CheckNULLTerminatedinTLF01 (&Auth_Tx.TLF01_details,MODE_UPDATE);
			 }
			 ReturnCode = db_update_tlf01_stip( &Auth_Tx.TLF01_details, strTemp );
		}
	}
	
   else
   {
	   if (0 == strncmp(Auth_Tx.TLF01_details.response_code,"19",2))
	   {
			  memset(strBuffer,0x00,sizeof(strBuffer));
			  sprintf( strBuffer, "Received timed out RC %s,sbatmp will wait for %d before retry,",
						 Auth_Tx.TLF01_details.response_code,sbatmp_advice_repeat_timer);
			 sbatmp_log_message( 1, 1, strBuffer, "Process_Auth_Advice_Response", 1 );
			 sbatmp_max_retry_erro_count = sbatmp_max_retry_erro_count + 1;
			 if(sbatmp_max_retry_erro_count > sbatmp_max_retry_count)
			 {
				 memset(strBuffer,0x00,sizeof(strBuffer));
				 sprintf( strBuffer, "Maximum Retry reached after timeout, sbatmp will go down");
				 sbatmp_log_message( 1, 1, strBuffer, "Process_Auth_Advice_Response", 1 );
				 shut_down();
				 exit(0);
			 }
			 else
			 {
				 memset(strBuffer,0x00,sizeof(strBuffer));
				 sleep(sbatmp_advice_repeat_timer);
				 sprintf( strBuffer, "Retry after %d Sec ,",sbatmp_advice_repeat_timer);
				 sbatmp_log_message( 1, 1, strBuffer, "Process_Auth_Advice_Response", 0 );
			 }
	   	}
	   else
	   {
		   sbatmp_max_retry_erro_count = 0;
		   sprintf( strBuffer, "Its not a valid response-%s with -%s ",Auth_Tx.TLF01_details.response_code,Auth_Tx.TLF01_details.message_type);
		   sbatmp_log_message( 1, 1, strBuffer, "Process_Auth_Advice_Response", 1 );
	   }
   }

	return 0;
}

BYTE Process_ATP_Created_Batch_Response (pPTE_MSG p_msg_in)
{
	BYTE ReturnCode = true;
	pBYTE pATEMessageDataData = 0 ;
	pPTE_MSG_DATA pATEMessageData = 0 ;
	char strTemp[256] = {0} ;
    int txnCount=0;
	char strBuffer[50]={0};
	int next_data=0;

	printf("Inside Process_ATP_Created_Batch_Responsefunc \n ");
	pATEMessageData			= ptemsg_get_pte_msg_data (p_msg_in) ;
	pATEMessageDataData		= ptemsg_get_pte_msg_data_data (pATEMessageData) ;
	
	/*******************************************************************/
    memset (&Auth_Tx, 0, sizeof Auth_Tx) ;
	memcpy (&Auth_Tx, (AUTH_TX *)pATEMessageDataData, sizeof Auth_Tx) ;
	/*******************************************************************/
	sprintf(strBuffer,"Received response form nciftr TRID %s stan:%s Key:%d message_type:%s and response code:%s ",
				Auth_Tx.TLF01_details.primary_key.transaction_id,
				Auth_Tx.TLF01_details.sys_trace_audit_num,
				Auth_Tx.TLF01_details.tx_key,
				Auth_Tx.TLF01_details.message_type,
		      Auth_Tx.TLF01_details.response_code) ;
		 sbatmp_log_message( 2, 1, strBuffer, "Process_ATP_Created_Batch_Response", 1 );

	if ( 0 == strncmp(Auth_Tx.TLF01_details.message_type,"03",02))
	 {
		if( (0== strncmp(Auth_Tx.TLF01_details.response_code,"01",2)) ||
			(0== strncmp(Auth_Tx.TLF01_details.response_code,"91",2)))
		{
			memset(strBuffer,0x00,sizeof(strBuffer));
			sprintf( strBuffer, "Host is not online sbatmp is going down");
			sbatmp_log_message( 1, 1, strBuffer, "Process_ATP_Created_Batch_Response", 1 );
			shut_down();
			exit(0);
			
		}
		else
		{
			memset(strBuffer,0x00,sizeof(strBuffer));
			sprintf( strBuffer, "Response returned from Host for batch upload Transaction");
			sbatmp_log_message( 2, 1, strBuffer, "Process_ATP_Created_Batch_Response", 1 );
			sale_tran_found_0200 = 1;
		}
	}
	
   else
   {
	   sprintf( strBuffer, "Its not a valid response-%s with -%s ",Auth_Tx.TLF01_details.tx_key,Auth_Tx.TLF01_details.message_type);
	   sbatmp_log_message( 1, 1, strBuffer, "Process_ATP_Created_Batch_Response", 1 );
   }

	return 0;
}

BYTE Process_ATP_Created_Settlenet_response(pPTE_MSG p_msg_in)
{
	BYTE ReturnCode = true;
	pBYTE pATEMessageDataData = 0 ;
	pPTE_MSG_DATA pATEMessageData = 0 ;
	char strTemp[256] = {0} ;
    int txnCount=0;
	char strBuffer[50]={0};
	int next_data=0;

	pATEMessageData			= ptemsg_get_pte_msg_data (p_msg_in) ;
	pATEMessageDataData		= ptemsg_get_pte_msg_data_data (pATEMessageData) ;
	
	printf("Inside Process_ATP_Created_Settlenet_response \n");
	/*******************************************************************/
    memset (&Auth_Tx, 0, sizeof Auth_Tx) ;
	memcpy (&Auth_Tx, (AUTH_TX *)pATEMessageDataData, sizeof Auth_Tx) ;
	/*******************************************************************/
	sprintf(strBuffer,"Received response form nciftr TRID %s stan:%s Key:%d message_type:%s and response code:%s \n",
			Auth_Tx.TLF01_details.primary_key.transaction_id,
			Auth_Tx.TLF01_details.sys_trace_audit_num,
			Auth_Tx.TLF01_details.tx_key,
			Auth_Tx.TLF01_details.message_type,
		      Auth_Tx.TLF01_details.response_code) ;
	sbatmp_log_message( 2, 1, strBuffer, "Process_ATP_Created_Settlenet_response", 1 );

	if( (0== strncmp(Auth_Tx.TLF01_details.response_code,"01",2)) ||
		(0== strncmp(Auth_Tx.TLF01_details.response_code,"91",2)))
	{
		memset(strBuffer,0x00,sizeof(strBuffer));
		sprintf( strBuffer, "Host is not online sbatmp is going down");
		sbatmp_log_message( 1, 1, strBuffer, "Process_ATP_Created_Settlenet_response", 1 );
		shut_down();
		exit(0);

	}
	else if ( 0 == strncmp(Auth_Tx.TLF01_details.message_type,"05",02))
	{
		printf("Response returned from Host for Settlement Transaction \n");
		memset( &db_tlf01_struct, 0, sizeof( db_tlf01_struct ) );
		memcpy( &db_tlf01_struct, ( pTLF01 )pATEMessageDataData, sizeof( TLF01 ) );
		if( (0== strncmp(Auth_Tx.TLF01_details.response_code,"00",2)))
		{
			strcpy(Auth_Tx.TLF01_details.product_codes[1].code,PROCESSED);
			 if (DB_NullCheck_Flag==1)
			 {
				CheckNULLTerminatedinTLF01 (&Auth_Tx.TLF01_details,MODE_UPDATE);
			 }
			db_update_tlf01_stip_500_success(&Auth_Tx.TLF01_details, strTemp );
		}
		else if( 0== strncmp(Auth_Tx.TLF01_details.response_code,"95",2))
		{
			strcpy(Auth_Tx.TLF01_details.product_codes[1].code,"bathSE");
			 if (DB_NullCheck_Flag==1)
			 {
				CheckNULLTerminatedinTLF01 (&Auth_Tx.TLF01_details,MODE_UPDATE);
			 }
			ReturnCode = db_update_tlf01_stip_settle_batch_error( &Auth_Tx.TLF01_details, strTemp );
			sprintf( strBuffer, "NOT able to Do BATCH UPLOAD");
			sbatmp_log_message( 2, 1, strBuffer, "Process_ATP_Created_Settlenet_response", 1 );
			//shut_down();
			//exit(0);
		}
		else if( 0== strncmp(Auth_Tx.TLF01_details.response_code,"01",2))
		{
			sprintf( strBuffer, "Host is not online sbatmp is going down");
			sbatmp_log_message( 1, 1, strBuffer, "Process_ATP_Created_Settlenet_response", 1 );
			shut_down();
			exit(0);
			
		}
		else 
		{
			sprintf( strBuffer, "Its not a valid response-%s with -%s ",Auth_Tx.TLF01_details.tx_key,Auth_Tx.TLF01_details.message_type);
			sbatmp_log_message( 1, 1, strBuffer, "Process_ATP_Created_Settlenet_response", 1 );
		}
	}
   else
   {
	   sprintf( strBuffer, "Its not a valid response-%s with -%s ",Auth_Tx.TLF01_details.tx_key,Auth_Tx.TLF01_details.message_type);
	   sbatmp_log_message( 1, 1, strBuffer, "Process_ATP_Created_Settlenet_response", 1 );
   }

	return 0;
}

int tlf01_stip_handler_0320(void)
{
	char              msg[100];
	char			  strBuffer[256];
	pPTE_MSG p_msg = 0 ;
	int  dest_len=0;
	LONG     lReturnCode = 0L ;
	INT      err_ctr = 0;
	CHAR     strTemp [256] = {0} ;
	BYTE     MessageType = 0 ;
	int     factor_len  = 7;
	int     i = 0;
	char    temp_zero[2]="0";
	int     nfactor_len =0;
	char    strTemtidlog[1000]={0};
	#define BATCH_UPLOAD_RESPONSE 16
 
	memset (&Auth_Tx, 0, sizeof Auth_Tx) ;
	printf("Inside tlf01_stip_handler_0320 handler");
	strncpy(Auth_Tx_BCH10.TLF01_details.pos_condition_code,"00", 2);
	strncpy(Auth_Tx_BCH10.TLF01_details.handler_queue,"sbatmpA",7);

	strcpy(Auth_Tx_BCH10.TLF01_details.message_type,"0320");
	Auth_Tx_BCH10.TLF01_details.tx_key = AUTH_BATCH_UPLOAD;
	
	nfactor_len= strlen(Auth_Tx_BCH10.TLF01_details.deferred_factor);
	if(nfactor_len > 0)
	{
		/*while(i <=factor_len)
		{	
			if(Auth_Tx_BCH10.TLF01_details.deferred_factor[i]=='.')
			{
				Auth_Tx_BCH10.TLF01_details.deferred_factor[i] =temp_zero[0];
			}
			i++;
		}*/
	}
	else
	{
		// Its Zero INT rate
		strncpy(Auth_Tx_BCH10.TLF01_details.deferred_factor  ,"0000000", 7 );
	}
	dest_len = strlen(authorizing_host_queue);
	if (authorizing_host_queue[dest_len - 1] == 'A')
	{
		//Do nothing
	}
	else
	{
		strcat(authorizing_host_queue,"A");
	}


	memcpy (&Auth_Tx, &Auth_Tx_BCH10, sizeof Auth_Tx) ;
	sprintf(strTemtidlog,"Sbatch_MP TRID: %s Batch num: %s",
					Auth_Tx_BCH10.TLF01_details.primary_key.transaction_id,
					Auth_Tx_BCH10.TLF01_details.batch_number);

	sbatmp_log_message( 2, 1, strTemtidlog, "tlf01_stip_handler_0320", 1 );

	lReturnCode = Send_Request_To_Service (  authorizing_host_queue,
						MT_AUTH_REQUEST, 
						0, 
						0, 
						0, 
						(pBYTE) (&Auth_Tx),
						sizeof(AUTH_TX));
	while (err_ctr <= advice_repeat_ctr )
	{
		/* You are blocked here waiting for a message on either app queue or
		* control que.  If there is no message on either que for 5 seconds,
		* the blocking call returns.  Application_que_name and control_que_name
		* are populated in pteipc_init_multiple_instance_app (pteipc.c).
		*/
		printf("0320 message send to host, wating for response\n");
		p_msg = pteipc_receive( application_que_name, control_que_name,
				sbatmp_timer_value, &lReturnCode);
	
       	if(p_msg != NULL_PTR)
		{	
			err_ctr = 0;
			Process_ATP_Created_Batch_Response (p_msg);
			total_tx_num++;
			free(p_msg);
			break;
		}
		else if(lReturnCode == QUE_ER_TIMEOUT)
		{
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
			sbatmp_log_message( 1, 1, strTemtidlog, "tlf01_stip_handler_0320", 0 );
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
				sbatmp_log_message( 1, 1, strBuffer, "tlf01_stip_handler_0320", 0 );
				#ifdef WIN32
					Sleep (50000);
				#else
					usleep (50000000);
				#endif
			}
		}			
		if( err_ctr >= advice_repeat_ctr)
		{
			sprintf( msg, "Reached maximum retry count for Batch upload, No Repsone from Cadience ");
			display_error_message(msg,"tlf01_stip_handler_0320");
			shut_down();
			exit(0);
		}
	}//while advice_repeat_ctr

}


int tlf01_stip_Settlement_handler(void)
{
	char              msg[100];
	char			  strBuffer[256];
	pPTE_MSG p_msg = 0 ;
	int  dest_len=0;
	LONG     lReturnCode = 0L ;
	INT      err_ctr = 0;
	CHAR     strTemp [256] = {0} ;
 
	printf("Inside tlf01_stip_Settlement_handler function\n ");
	strncpy(Auth_Tx.TLF01_details.message_type,"0500",4);
	strncpy(Auth_Tx.TLF01_details.processing_code, "960000",6);
	Auth_Tx.TLF01_details.tx_key = AUTH_SETTLE;
	strncpy(Auth_Tx.TLF01_details.handler_queue,"sbatmpA",7);
	dest_len = strlen(authorizing_host_queue);
	if (authorizing_host_queue[dest_len - 1] == 'A')
	{
		//Do nothing
	}
	else
	{
		strcat(authorizing_host_queue,"A");
	}
	printf("Sending 0500 after batch upload\n");
	lReturnCode = Send_Request_To_Service (  authorizing_host_queue,
						MT_AUTH_REQUEST, 
						0, 
						0, 
						0, 
						(pBYTE) (&Auth_Tx), 
						sizeof(AUTH_TX));
	while (err_ctr <= advice_repeat_ctr )
	{
		/* You are blocked here waiting for a message on either app queue or
		* control que.  If there is no message on either que for 5 seconds,
		* the blocking call returns.  Application_que_name and control_que_name
		* are populated in pteipc_init_multiple_instance_app (pteipc.c).
		*/
		p_msg = pteipc_receive( application_que_name, control_que_name,
				sbatmp_timer_value, &lReturnCode);
        printf("waiting for response for 0500\n");
		if(p_msg != NULL_PTR)
		{
			err_ctr = 0;
			Process_ATP_Created_Settlenet_response (p_msg);
			total_tx_num++;
			free(p_msg);
			break;
		}
		else if(lReturnCode == QUE_ER_TIMEOUT)
		{
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
			sbatmp_log_message( 1, 1, strBuffer, "tlf01_stip_Settlement_handler", 0 );

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
				sbatmp_log_message( 1, 1, strBuffer, "tlf01_stip_Settlement_handler", 0 );
				#ifdef WIN32
					Sleep (50000);
				#else
					usleep (50000000);
				#endif
			}
		}
		if( err_ctr >= advice_repeat_ctr)
		{
			sprintf( msg, "Reached maximum retry count for Settlement, No Repsone from Cadience ");
			display_error_message(msg,"tlf01_stip_Settlement_handler");
			shut_down();
			exit(0);
		}
	}//while advice_repeat_ctr

}


int tlf01_stip_handler_0200(void)
{
	char        msg[100];
	char		strBuffer[256];
	pPTE_MSG	p_msg = 0 ;
	int  		dest_len=0;
	LONG     	lReturnCode = 0L ;
	INT      	err_ctr = 0;
	CHAR     	strTemp [256] = {0} ;
	BYTE    	 MessageType = 0 ;
	char    	temp_zero[2]="0";
	int     	nfactor_len =0;
	char    strTemtidlog[1000]={0};
	char    temp_authorizing_host_queue[8]="nciftrA";

	Auth_Tx.TLF01_details.tx_key = AUTH_ADVICE;
	strncpy(Auth_Tx.TLF01_details.handler_queue,"sbatmpA",7);

	if((strcmp(Auth_Tx.TLF01_details.message_type,"0120") == 0))
	{
		strcpy(Auth_Tx.TLF01_details.message_type,"0121");
	}
	else
	{
		strcpy(Auth_Tx.TLF01_details.message_type,"0120");
	}

	sprintf(strTemtidlog,"Sbatch_MP TRID: %s Batch number: %s STAN %s , key:%d , Message_type :%s",
						Auth_Tx.TLF01_details.primary_key.transaction_id,
						Auth_Tx.TLF01_details.batch_number,
						Auth_Tx.TLF01_details.sys_trace_audit_num,
						Auth_Tx.TLF01_details.tx_key,
						Auth_Tx.TLF01_details.message_type);
	sbatmp_log_message( 2, 1, strTemtidlog, "tlf01_stip_handler_0200", 0 );
	printf("%s\n",strTemtidlog);

	lReturnCode = Send_Request_To_Service (  temp_authorizing_host_queue,
						MT_AUTH_REQUEST,
						0,
						0,
						0,
						(pBYTE) (&Auth_Tx),
						sizeof(AUTH_TX));
	if(lReturnCode == DEVICE_OK)
	{
		printf("Transaction is send to Auth_Queue %s , TRID %s \n",temp_authorizing_host_queue,
				Auth_Tx.TLF01_details.primary_key.transaction_id);
	}
	else
	{
		printf("Error is sendig message to Auth_queue %s\n",temp_authorizing_host_queue);
	}

	while (err_ctr <= advice_repeat_ctr )
	{
		/* You are blocked here waiting for a message on either app queue or
		* control que.  If there is no message on either que for 5 seconds,
		* the blocking call returns.  Application_que_name and control_que_name
		* are populated in pteipc_init_multiple_instance_app (pteipc.c).
		*/
		p_msg = pteipc_receive( application_que_name, control_que_name,
				sbatmp_timer_value, &lReturnCode);

       	if(p_msg != NULL_PTR)
		{
			err_ctr = 0;
			Process_Auth_Advice_Response (p_msg);
			total_tx_num++;
			free(p_msg);
			break;
		}
		else if(lReturnCode == QUE_ER_TIMEOUT)
		{
			lReturnCode = Send_Request_To_Service (  temp_authorizing_host_queue,
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
			sbatmp_log_message( 1, 1, strBuffer, "tlf01_stip_handler_0200", 0 );
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
				sbatmp_log_message( 1, 1, strBuffer, "tlf01_stip_handler_0200", 0 );
				#ifdef WIN32
					Sleep (50000);
				#else
					usleep (50000000);
				#endif
			}
		}
		if( err_ctr >= advice_repeat_ctr)
		{
			sprintf( msg, "Reached maximum retry count for Batch upload, No Repsone from Cadience ");
			display_error_message(msg,"tlf01_stip_handler_0200");
			shut_down();
			exit(0);
		}
	}//while advice_repeat_ctr

}

/*******************************************************
 * This function will get the values from tf.ini file
 * from the section DATASERVER with keyname DB_ERROR_STATICS_FLAG &
 *	DB_ERROR_STATICS_VALUE which will help to decide the db statics
 *	logging mechanism.
 * *******************************************************/

void sbatmp_get_error_warning_file_name_path(void )
{
   DWORD rc;
   CHAR  filename   [80] = {0};
   CHAR  tmpstr     [80] = {0};
   CHAR  sbatmp_error_warning_file_size[5] = {0};

   /* Get path to the tf.ini file. */
   memset( tmpstr, 0x00, sizeof(tmpstr) );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, "tf.ini" );

   /*Read path for creating file in order to log db statics and db oracle error messages */
   rc = GetPrivateProfileString(
								  "DATASERVERS",             /* points to section name         */
								  "DB_STATICS_PATH",       	/* points to key name             */
								   "",  					/* Default string                 */
								   sbatmp_error_warning_file_path,              	/* points to destination buffer   */
								   sizeof(sbatmp_error_warning_file_path)-1,   	 /* size of destination buffer     */
								   filename                  /* points to ini filename         */
						 	   );

   rc = GetPrivateProfileString(
								  "ERROR_WARNING",             /* points to section name         */
								  AppName,       	/* points to key name             */
								  "",  					/* Default string                 */
								  sbatmp_module_error_warning_file_name,              	/* points to destination buffer   */
								  sizeof(sbatmp_module_error_warning_file_name)-1,   	 /* size of destination buffer     */
								  filename                  /* points to ini filename         */
								);

   rc = GetPrivateProfileString(
							  "ERROR_WARNING",             /* points to section name         */
							  "ERROR_WARNING_FILE_SIZE",       	/* points to key name             */
							  "500",  					/* Default string                 */
							  sbatmp_error_warning_file_size,              	/* points to destination buffer   */
							  sizeof(sbatmp_error_warning_file_size)-1,   	 /* size of destination buffer     */
							  filename                  /* points to ini filename         */
							);

	/* File size size conversion */
	Max_File_Size_Defined = atoi(sbatmp_error_warning_file_size);
	if(Max_File_Size_Defined <= 0)
	{
		Max_File_Size_Defined = 500 ;
	}
	Max_File_Size_Defined = Max_File_Size_Defined * ONE_MB_IN_BYTES ; /* 1 MB = 1048576 BYTES*/


   if((strlen(sbatmp_error_warning_file_path) > 0) &&
	  (strlen(sbatmp_module_error_warning_file_name)> 0))
   {
	   sbatmp_create_Error_Filename();
   }
}

INT sbatmp_Log_error_warning_to_File(pCHAR Error_Buf,int sev,pCHAR func,int detail)
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
    len = strlen(sbatmp_Erro_warning_Filename);
    path_len = strlen(sbatmp_error_warning_file_path);
    if( len == 0 ||
    	path_len==0 )
    {
    	LogEvent(Error_Buf,INFO_MSG);
    	return 0;
    }
    if ( 0 != strcmp(&sbatmp_Erro_warning_Filename[len-4], current_mmdd) )
    {
		/* Now create the new filename.*/
    	sbatmp_create_Error_Filename();
    }
	if((fp = fopen(sbatmp_Erro_warning_Filename,"a+b"))==NULL)
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
	if(detail == 1)
	{
		if (strlen(Auth_Tx.TLF01_details.card_num)> 0)
		{
			/* Masking card number first 5 and last 4 in clear form remaining all masked. */
			cardlen = strlen( Auth_Tx.TLF01_details.card_num );
			strncpy( tempcard, Auth_Tx.TLF01_details.card_num, cardlen);
			for ( nIndex = cardlen-5; nIndex >= cardlen-11; nIndex-- )
			{
				tempcard[nIndex] = '*';
			}
			sprintf(card_tid_mid_details,"CNBR %s TID %s MID %s STAN %s RRN %s",
					tempcard,
					Auth_Tx.TLF01_details.terminal_id,
					Auth_Tx.TLF01_details.merchant_id,
					Auth_Tx.TLF01_details.sys_trace_audit_num,
					Auth_Tx.TLF01_details.retrieval_ref_num);
		}
		else
		{
			sprintf(card_tid_mid_details,"TID %s MID %s STAN %s RRN %s",
					Auth_Tx.TLF01_details.terminal_id,
					Auth_Tx.TLF01_details.merchant_id,
					Auth_Tx.TLF01_details.sys_trace_audit_num,
					Auth_Tx.TLF01_details.retrieval_ref_num);
		}
		strcat(Buffer,card_tid_mid_details);
		strcat(Buffer," ");
	}
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

void sbatmp_create_Error_Filename(  )
{
	CHAR  system_date[25] = {0};

   /* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
	ptetime_get_timestamp( system_date );

    strcpy( sbatmp_Erro_warning_Filename, sbatmp_error_warning_file_path );
    strcat( sbatmp_Erro_warning_Filename, sbatmp_module_error_warning_file_name );
	strcat( sbatmp_Erro_warning_Filename, ".EW." );
    strncat(sbatmp_Erro_warning_Filename, system_date,   4 );  /* YYYY */
    strncat(sbatmp_Erro_warning_Filename, system_date+5, 2 );  /* MM   */
    strncat(sbatmp_Erro_warning_Filename, system_date+8, 2 );  /* DD   */
}
/******************************************************************************
 *
 *  NAME:         sbatmp_log_message
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
void sbatmp_log_message( INT dest, INT sev, pCHAR msg, pCHAR func, int details )
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
   sbatmp_Log_error_warning_to_File(text_message,sev,func,details);
   if(monitor_flag == 1)
   {
	   if(details == 1)
	   {
		   TxUtils_Send_Msg_To_Operator( monitor_flag, text_message,
										 eventlog_flag, msg_type, func,
										 severity, log_type,
										 Auth_Tx.TLF01_details.terminal_id,
										 Auth_Tx.TLF01_details.card_num,
										 Auth_Tx.TLF01_details.merchant_id );
	   }
	   else
	   {
		   TxUtils_Send_Msg_To_Operator( monitor_flag, text_message,
										 eventlog_flag, msg_type, func,
										 severity, log_type,
										 NULL, NULL,NULL );
	   }
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
