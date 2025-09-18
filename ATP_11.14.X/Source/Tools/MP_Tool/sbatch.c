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

//static TLF01			Auth_Tx;
TLF01	tlf01_tmp_struct, db_tlf01_struct;
AUTH_TX Auth_Tx;
AUTH_TX Auth_Tx_BCH10 ;
BCH10 bch10;
pTLF01 tlf01_ptr;
pNCF01	ncf01_ptr;
NCF01 ncf01_tmp_struct;
BYTE db_select_ncf01 (char[], pNCF01 , char*);
/* declared variable in .C file TF-Ajay 23 Dec 2009 */
char    NetCntlName[12];
CHAR	TxnDate[9];
CHAR    AppName   [MAX_APP_NAME_SIZE];

/* Currency Codes ..TF Phani*/
#define  PESO     "608"
#define  USD      "840"
#define TID_LEN    8
#define MID_LEN_10    10
#define RRN_LEN    12
#define RECORD_LEN 
#define MID_POS 9
#define RRN_POS 20

#ifdef WIN32
   #define DEFAULT_DIRECTORY  "\\ascendent\\sbatmp"
#else
   #define DEFAULT_DIRECTORY "/ascendent/sbatmp"
#endif

BYTE p_buffer [MAX_DATASERVER_BUFFER_LENGTH] ; /* defined to be 25000 */


CHAR  Version[] = "ATP_11.1.0";

FILE *fp0;
FILE *fpL;
FILE *fpE;

BOOLEAN Create_output_header_file(void)
{
	CHAR              tempstr[16]="";
	CHAR              buffer[100]=""; 
 	CHAR              header[512]=" ";
	
    memset(header, 0, sizeof(header)); 
	strcpy(header,"Terminal ID |Merchant ID       |RRN              |CARD NUMBER         |TRANSACTION AMOUNT |TERM LEN |FACTOR  |AMORTIZATIAN AMOUNT |GROSS AMOUNT        |");
	strncpy(header + 152, "\n\0", 2);
	
	/*** Write to file ***/
   if (fputs(header,fp0) < 0)
   {
	  strcpy(buffer, "Cannot Open output file: ");
	  return(false);
   }   

   return (true);
} /* create_exception_headers */

int main(int argc, char *argv[])
{
	char              msg[100]={0};
	char			  strBuffer[256]={0};
	char			  errorbuf[256]={0};
	BYTE              rc;
	pPTE_MSG p_msg = 0 ;int  dest_len=0;
	LONG     lReturnCode = 0L ;
	int               next_portion = 0;
	INT      err_ctr = 0;
	CHAR     strTemp [256] = {0} ;
	BYTE           tranid[21];
    EMV_RAW        emvraw;
	BYTE			emv_result_code;
	memset(&Auth_Tx, 0, sizeof(Auth_Tx)); 
	memset(&tlf01_tmp_struct,0,sizeof(TLF01));
	memset(&ncf01_tmp_struct,0,sizeof(NCF01));
	CHAR Line_record[100]={0};
	CHAR MIDstart[5]="00000";
	CHAR MID10[10]={0};
	CHAR TID[9]={0};
	CHAR MID[16]={0};
	CHAR RRN[13]={0};
	INT count = 0;
	CHAR output_record[300]={0};
		
	if(argc<3)
   {	
       printf("MP tool Two command line parameters to work correctly:			\n");
       printf("(1) Request File name SerachFile_12Mar.txt.															\n");
       printf("(2) output file name Output.txt.																		\n");
   	   printf("This two input files should be present in the director where this tool is installed. \n");   
   	   printf("                           						                                    \n");
	   printf("This tool create output file 											                \n");
       printf("1)output.txt														                \n");
       exit(1);
   }
  /* Try to connect to the database */
  if (dbcommon_connect("equitable", "equitable", "equitable", "ORCL", msg) != PTEMSG_OK)
  {
		exit(1);
  }
	/* Open the Lookup file*/
   fpL = fopen(argv[1],"r"); 
   if (fpL == NULL)
   {
	  printf("EPOS File open failed\n");
	  exit(1);
   }

   /* Open Output File*/
   fp0 = fopen(argv[2],"w"); 
   if (fp0 == NULL)
   {
	  printf("EPOS Lookup File open failed\n");
	  exit(1);
   }
   /* Open error File*/
   fpE = fopen("error.txt","w"); 
   if (fpE == NULL)
   {
	  printf("EPOS Lookup File open failed\n");
	  exit(1);
   }
   /*Creat the headfer for the output file*/
   if(false==Create_output_header_file())
   {
	   exit(1);
   }
   
   while (next_portion == 0)
   {
        if(fgets(Line_record,sizeof(Line_record)- 1,fpL)!=NULL)
		{
			memset(TID,0,sizeof(TID));
			memset(MID,0,sizeof(MID));
			memset(RRN,0,sizeof(RRN));
			strncpy(TID,Line_record,TID_LEN);
			strncpy(MID10,Line_record+MID_POS,MID_LEN_10);
			strncpy(RRN,Line_record+RRN_POS ,RRN_LEN);
			strncat(MID,MIDstart,5);
			strncat(MID,MID10,10);
			memset(Line_record,0,sizeof(Line_record));
			memset(&tlf01_tmp_struct,0,sizeof(TLF01));
			//printf("Get details for TID :%s MID :%s RRN :%s \n",TID,MID,RRN);
			rc = db_get_tlf01_stip_standin_rec_320(&tlf01_tmp_struct, TID, MID,RRN);

			if ( rc == PTEMSG_OK)
			{
				memset(&Auth_Tx, 0, sizeof(Auth_Tx));
				memcpy (&Auth_Tx.TLF01_details, &tlf01_tmp_struct, sizeof (TLF01)) ;
				count ++;
				sprintf(output_record, "%8s%19s%17s%21s%18s%10s%15s%14s%21s%",
						Auth_Tx.TLF01_details.terminal_id,
						Auth_Tx.TLF01_details.merchant_id, 
						Auth_Tx.TLF01_details.retrieval_ref_num,				
						Auth_Tx.TLF01_details.card_num,
						Auth_Tx.TLF01_details.total_amount,
						Auth_Tx.TLF01_details.deferred_term_length,
						Auth_Tx.TLF01_details.deferred_factor,
						Auth_Tx.TLF01_details.monthly_amort_amt,
						Auth_Tx.TLF01_details.def_gross_amt								
					   );
			  strncpy(output_record + 143, "\n\0", 2);

				/*** Write to file ***/
				if (fputs(output_record,fp0) < 0)
				{
					printf("Error in writing to output file\n");
				}
				memset(output_record,0,sizeof(output_record));
			} /* endif */
			else
			{
				sprintf(errorbuf,"Record not found with TID:%s MID:%s RRN:%s\n",TID,MID,RRN);
				/*** Write to file ***/
				if (fputs(errorbuf,fpE) < 0)
				{
					printf("Error in writing to error file\n");
				}
				memset(errorbuf,0,sizeof(errorbuf));
			}
		}
		else
		{
			next_portion = 1;
		}

	} // end while - next_portion
	printf("Total record generated is %d \n",count);
	dbcommon_disconnect(errorbuf);
	
} //  
