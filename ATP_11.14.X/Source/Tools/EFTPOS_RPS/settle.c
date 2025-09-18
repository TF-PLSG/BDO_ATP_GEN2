/*********************************************************************************

                MERCHBATCH - EFTPOS - UTILITY - THOUGHTFOCUS 


  REVISION HISTORY
   ----------------

    Rev 1.2 Jun 25 2009 Ajay S.T. TF
   Some of the tags was missing in parsing the emv raw data
   Updated version 1.0.0.27

	 Rev 1.1 Jun 22 2009 Ajay S.T. TF
   Issue. MCC field was missing from the segment 10.
   Updated version 1.0.0.26 

   

/********************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <memory.h>
#include <math.h>
#include <limits.h>
#include "basictyp.h"
#include "pte.h"
#include "ptemsg.h"
#include "pteipc.h"
#include "ptestats.h"
#include "ptesystm.h"
#include "ntutils.h"

#include "app_info.h"
#include "equitdb.h"
#include "dc_dbstruct.h"
#include "settle.h"
#include "txutils.h"
#include "dc_database.h"
#include "memmnger.h"
#include "ptetimer.h"
#include "ptetime.h"
#include "dbcommon.h"
#include "timings.h"
#include "string.h"



/***** External variables */

extern volatile INT  EndProcessSignalled;
extern volatile INT  MainProcessDone;
extern int           Start();
extern void          Stop ();

/***** Global variables *****/

static BCH10_LIST    bch10_list;
static BCH11_LIST    bch11_list;

static BCH01_LIST    bch01_list;
static TLOG01_LIST   tlog01_list; /* ThoughtFocus - Chandra */
static EMV_LIST      Emv_list;    /* ThoughtFocus - Chandra */
CHAR	temp_filler[50]={0};
static CHAR          c_record [245]; 
static CHAR          d_record [245];
static CHAR          e_record [122]; 
static CHAR			 dump_record[256];
static CHAR			 r_record[256];


static TC_TOTALS     tc_totals;
DCF01                terminal;
BCH10				 auth_tx_detail;         
BCH11				 auth_tx_detail_arc;
BCH01			  	 auth_tx_total; 
EMV_RAW              emv_record;  /* ThoughtFocus - Chandra */         
CHAR				 test_temp_msg[15];	
BYTE MID[16]={0};
BYTE OPEN_DATE[9]={0};

static CHAR          cutoff_date[9]={0};
static CHAR          cutoff_hour[3];
static CHAR          day[4];
static CHAR          day_m[3];
static CHAR          extract_date[7] = "";
static CHAR          hour_offset[3];
static CHAR          minute[3];
static CHAR          month[3];
static CHAR          year[5];
static CHAR          jul_day[4];
static CHAR          offset[3];
static CHAR          process_code[2];
static CHAR          time_tag[20];
static CHAR          yyyymmdd_finish[9];
static CHAR          yyyymmdd_start[9];
static CHAR          path[30] = "";  /*hold root directory path*/
static INT           cre_inc_value=1,deb_inc_value=1,cseg10_records=0,dseg10_records=0,crec_count=0,drec_count=0; /* ThoughtFocus - Chandra - Increment value */
#ifdef WIN32 // TF Phani to build in winodws
static unsigned __int64  ctotal_amt=0,dtotal_amt=0;
#else
static unsigned long long int  ctotal_amt=0,dtotal_amt=0;
#endif

static FILE          *c_file_write = 0;
static FILE          *MID_file_write = 0;
static FILE          *e_file_write = 0;
static FILE          *r_file_write = 0,*err_file_write = 0;

static CHAR          base[17];
static CHAR          c_filename[256] = "";       /* credit file name */
static CHAR          d_filename[256] = "";       /* debit  file name */
static CHAR          r_filename[256] = "";       /* report  file name */
static CHAR          e_filename[256] = "";       /* error  file name */


static CHAR          dump_filename[256] = "";    /* dump file name */
static FILE          *dumpfile_ptr = 0;	         /* dump file ptr */


static CHAR          dir_str  [128];
static INT           max_tran = 0,len=0;
static double        lenf=0.0;
static INT           credit_txns_exist=0;
static INT           debit_txns_exist=0;
static CHAR          curr_code[4];
static CHAR          device_type[3];
static BYTE          max_reward[13],emv_data[576];
static CHAR          industry_code[3];			 
extern CHAR          ExeName[100];


       CHAR          EndProcessFlag;
       CHAR          AppName[12];
       CHAR          lError_Msg[300];
       CHAR          Version[] = "ATP_11.1.0";  /* ThoughtFocus - Phani -MC/VISA/JCB POS entry mode changes*/
											 

	   CHAR  emv_seg13[192],emv_seg14[192],emv_seg15[192],temp_filler_emv[500];


	   CHAR  CurrentDate[9],clen[2],elen[2],err_msg[100];

#define EMV_TAG_MAX_LEN 36 // Assuming MAX Tag lenght will be 36 - Phani

/* Database Timing Measurements */
TIMINGS_LIST  TimingStats;
INT           DB_Timing_Flag; /* True = Do DB Timing Stats, False = don't do */
INT           ForceReport;
CHAR          ReportTime[5];
//
/* Falg for Dulicate Extraction restrict - TF Phani
	DUPLICATE_RESTRICT flag from tf.ini file
	# 1 - Don't allow duplicates in EFTPOS file
	# 0 - Allow dulicates in EFTPOS file	*/
static INT Duplicate_Restrict;

void bin_to_ascii (pCHAR src, pCHAR dst, INT len)
{
   INT  i, j;

   for (i = 0; i < (len*2); i+=2)
   {
       j = (src[i/2] < 0) ? (src[i/2] + 256) : src[i/2];

      if ((j / 16) <= 9) 
        dst[i] = (j / 16) + '0';
      else
        dst[i] = (j/16 - 0x0a)+'A';

      if ((j & 0x0f) <= 9)
        dst[i+1] = (j & 0x0F) + '0'; 
      else
        dst[i+1] = (j & 0x0f) - 0x0a + 'A';    
   }
} 
/*******************************************************************************
*
*  FUNCTION:         main
*
*  DESCRIPTION:      This function gets the initial parameters.
*
*  INPUTS:           INT argc - number of parameters
*                    pCHAR argv[1] - path and filename 
*                  
*  OUTPUTS:          None
*
*  RETURN VALUE:     None 
*   
*******************************************************************************/
void main(int argc, char *argv[])
{

   CHAR correct_hours [24] [3] = {"00", "01", "02", "03", "04", "05",
                                  "06", "07", "08", "09", "10", "11",
                                  "12", "13", "14", "15", "16", "17",
                                  "18", "19", "20", "21", "22", "23"};

   CHAR correct_files [16] [7] = {"ALL", "all", "CREDIT", "credit", 
                                  "DEBIT", "debit", "RAFFLE", "raffle", 
                                  "A", "a", "C", "c", "D", "d", "R", "r"};

   CHAR correct_codes [16] [2] = {"A", "A", "C", "C", "D", "D", "R", "R",
                                  "A", "A", "C", "C", "D", "D", "R", "R"};

   int               hour_index;
   int               file_index;

   /*** Initialize global variables ***/
   memset(base,         0, sizeof(base));
   memset(cutoff_date,  0, sizeof(cutoff_date));
   memset(cutoff_hour,  0, sizeof(cutoff_hour));
   memset(offset,       0, sizeof(offset));

   EndProcessFlag = false;
	#ifndef WIN32
		strcpy(ExeName, argv[0]);
	#endif
   /*********************************************************
   *
   *  There are 5 required command-line parameters:
   *     Parameter 0    -  executable program name
   *     Parameter 1    -  settle file base file name 
   *     Parameter 2    -  cutoff date (yyyymmdd) or comma (',')
   *
   **********************************************************/

   if (argc != 3) 
   {
      display_settle_info();
      return;
   }

   if (strcmp(argv[1], "?") == 0)
   {
      display_settle_info();
      return;
   }

   /*** 1st Parameter - base file name ***/
   if (strlen(argv[1]) > 16)
	{
      printf("\n\n");
	   printf("Error in 1st parameter:\n");
      printf("Base of filename must be less than 17 characters.\n");
      printf("\n");
      return;
	}
   else
      strcpy(base, argv[1]);  

   /*** 2nd Parameter - cutoff date ***/
   if (strcmp(argv[2], ",") != 0)
   {
      if (strlen(argv[2]) != 8)
      {
         printf("\n\n");
         printf("Error in 2nd parameter.\n");
         printf("Date must be in YYYYMMDD format.\n");
         printf("\n");
         return;
      }
   }
   strcpy(cutoff_date, argv[2]);
   strcpy(cutoff_hour, "00");
   strcpy(offset, "23");
   strcpy(process_code, "A");
   cre_inc_value=1;

   if(!Start())
	   printf("Error starting ServiceManager\n\n" );
   	 
	Stop();

} /* main */

void create_reports();

/*******************************************************************************
*
*  FUNCTION:         MainProcessor
*
*  DESCRIPTION:      This function performs initial application setup.
*
*  INPUTS:           None
*         
*  OUTPUTS:          None
*
*  RETURN VALUE:     None
* 
*******************************************************************************/
void MainProcessor(void)
{
   CHAR              buffer[500];
   CHAR              err_msg[100];
   CHAR              time_date[25] = "";
   INT               count_tran = 0;
   INT               next_portion = 0; 
   INT               num_returned = 0;
   INT               ret_code = 0;   
   BYTE              bch01_buffer[sizeof(AUTH_TX) + sizeof(BCH01)];
   INT               num_sql;
   INT               err_ctr = 0;
   

   /* Clear buffer (set to null) */
   memset(&bch01_buffer,0, sizeof(bch01_buffer));
   memset (&lError_Msg, 0x00, sizeof (lError_Msg));
   memset (&buffer, 0x00, sizeof(buffer));
  
   GetAppName (AppName);

   sprintf( buffer,
           "Starting EFTPOS: %s,  version %s", AppName, Version );
   LogEvent( buffer, INFO_MSG );

   /* Send message to DOS window in debug mode */
   strcat(buffer,"\n");
   PRINT(buffer);

   /* Get root directory path */
   memset (path, 0x00, sizeof(path));
   GetAscendentRootDirectory(path);

   /* Get Database Timing Parameters to gather statistics about each query. */
   memset( ReportTime, 0x00,   sizeof(ReportTime) );
   memset( buffer,     0x00, sizeof(buffer)     );
   ret_code = get_timing_parameters( &DB_Timing_Flag, ReportTime, &ForceReport, buffer );
   LogEvent( buffer, INFO_MSG );
	/* Read Duplicate Extraction flag from  TF.ini . 
    If flag is 0 - duplicates are allowed 1- Duplicates are not allowed*/
	Get_duplicate_restrict_flag_from_ini();

   /* Initialize the DB Timing Statistics Data Structure. */
   if ( DB_Timing_Flag == true )
   {
      reset_timing_stats( &TimingStats );
   }

   /* Try to connect to the database */
#ifdef _DEBUG
   if (dbcommon_connect ("equitable", "equitable", "equitable", "ORCL", err_msg) != PTEMSG_OK)
#else
   if (dbcommon_connect ("equitable", "equitable", "equitable", "ORCL", err_msg) != PTEMSG_OK)
#endif
   {
      memset(&buffer, 0x00, sizeof(buffer));
	  strcpy(buffer, "Database Connect Error:  ");
	  strcat(buffer, err_msg);
      TxUtils_Send_Msg_To_Operator( 0, buffer, 1, ERROR_MSG, "MainProcessor",
         4, FATAL_ERROR, NULL_PTR, NULL_PTR, NULL_PTR);
	  write_to_dump_file(buffer);
	   /* pteipc_shutdown_single_instance_app(); */
	  MainProcessDone = 1;

       return;
   }

   /* Send message to system monitor */
   strcpy(buffer,"Connected to ORACLE ");
   TxUtils_Send_Msg_To_Operator ( 0, buffer, 1, INFO_MSG, "MainProcessor",
      0, INFO_ERROR, NULL_PTR, NULL_PTR, NULL_PTR);

   /* Send message to DOS window in debug mode */
   strcat(buffer,"\n");
   PRINT(buffer);

   /* Set cutoff date and time */
   set_date();

   /* Set dates for Exception processing. */
   ptetime_get_timestamp( time_date );  /* YYYY-MM-DD-hh.mm.ss.jjj */
   get_date( time_date, CurrentDate );  /* YYYYMMDD */   

   strcpy(CurrentDate,CurrentDate+2);     

 
   /* If we are going to generate all files or just credit file */
   if ((process_code[0] == 'A') || (process_code[0] == 'C'))
   {
      if (open_credit_file() == false)
      {
         /* Send message to system monitor */
         strcpy(buffer,"Stopping settlement ");
         TxUtils_Send_Msg_To_Operator ( 0, buffer, 1, INFO_MSG, "MainProcessor",
            0, INFO_ERROR, NULL_PTR, NULL_PTR, NULL_PTR);

         /* Send message to DOS window in debug mode */
         strcat(buffer,"\n");
         PRINT(buffer);

         /* pteipc_shutdown_single_instance_app(); */
         MainProcessDone = 1;
         return;
      }

   }

   
   memset(c_record, 0, sizeof(c_record));
   memset(d_record, 0, sizeof(d_record));

   create_headers();
   write_to_credit_settle_file();    
   
   memset(c_record, 0, sizeof(c_record));
   memset(d_record, 0, sizeof(d_record));

   /*
	Open MIT.txt file $ASCENDENTROOT/eftpos.

   */
   if (open_MID_file() == false)
   {
   		/* Send message to system monitor */
        strcpy(buffer,"Stopping EFTPOS_RPS ");
        TxUtils_Send_Msg_To_Operator ( 0, buffer, 1, INFO_MSG, "MainProcessor",
           0, INFO_ERROR, NULL_PTR, NULL_PTR, NULL_PTR);

        /* Send message to DOS window in debug mode */
        strcat(buffer,"\n");
        PRINT(buffer);

        /* pteipc_shutdown_single_instance_app(); */
        MainProcessDone = 1;
        return;
    }

   	/* Read entry from MID.txt to get the merchent id.*/

	char Temp_MID_data[1024];
	int fread_ret=0;
	int MID_counter =0 ;
	int MID_Index = 0;

	#define MID_COUNTER_MAX 50
	#define MID_LENGTH 15
	#define NEXT_MID_INDEX 16

	fread(Temp_MID_data, 1, 1024,MID_file_write);
	if (fread_ret <0)
	{
		/* Send message to system monitor */
       	strcpy(buffer,"Stopping EFTPOS_RPS Read Error ");
       	TxUtils_Send_Msg_To_Operator ( 0, buffer, 1, INFO_MSG, "MainProcessor",
       		0, INFO_ERROR, NULL_PTR, NULL_PTR, NULL_PTR);

		/* Send message to DOS window in debug mode */
    	strcat(buffer,"\n");
       	PRINT(buffer);

       	 /* pteipc_shutdown_single_instance_app(); */
      	 MainProcessDone = 1;
       	 return;
	}

	/*** Set initial data for BCH11 table ***/
   memset(&auth_tx_total, 0, sizeof(auth_tx_total));
   memset(&tc_totals, 0, sizeof(tc_totals));
   strcpy(auth_tx_total.primary_key.device_id, " ");
   strcpy(auth_tx_total.primary_key.batch_nbr, " ");
   next_portion = 0;

   strcpy(auth_tx_total.open_date, yyyymmdd_start);
   strcpy(auth_tx_total.release_date, yyyymmdd_finish);
   strcpy(auth_tx_total.open_time, offset);
   strcat(auth_tx_total.open_time, "0000");
   strcpy(auth_tx_total.release_time, cutoff_hour);
   strcat(auth_tx_total.release_time, "0000");  
   strcpy(OPEN_DATE,yyyymmdd_finish);
		
	for ( MID_counter; MID_counter < MID_COUNTER_MAX ; MID_counter++)
	{
		if(!(isdigit(Temp_MID_data[MID_Index])))
		{
			// No Merchant Record
			break;
		}
		strncpy(MID,Temp_MID_data+MID_Index,MID_LENGTH);
		MID[MID_LENGTH]='\0'; 
		
		// Point to the next MID in the array 
		MID_Index = MID_Index + NEXT_MID_INDEX;
		
		// Read the record for this Merchant ID.
	
		if (ret_code =  (db_get_bch01_service_list(&auth_tx_total, &bch01_list, err_msg)) != PTEMSG_OK)
   		{
		   sprintf (buffer, "Error processing bch01 batches.");
		   sprintf(lError_Msg,"db_get_bch11_service_list: %s",buffer);
		   LogEvent(lError_Msg,ERROR_MSG);
   		}
	}
			
   memset(c_record, 0, sizeof(c_record));
   memset(d_record, 0, sizeof(d_record));
    
   create_trailers();
   write_to_credit_settle_file();    
  
   fclose (c_file_write);
  
   create_reports();
 
   dbcommon_disconnect (err_msg);

   /* Report the DB timing measurement statistics before exiting. */
   num_sql = log_timing_statistics_report( &TimingStats );

   memset( buffer, 0x00, sizeof(buffer) );
   sprintf( buffer,
           "%s logged stats for %d SQLs",
            AppName, num_sql );
   LogEvent( buffer, INFO_MSG );

   /* Send message to system monitor */
   strcpy(buffer,"Stopping settlement ");
   TxUtils_Send_Msg_To_Operator ( 0, buffer, 1, INFO_MSG, "MainProcessor",
      0, INFO_ERROR, NULL_PTR, NULL_PTR, NULL_PTR);


   /* Send message to DOS window in debug mode */
   strcat(buffer,"\n");
   PRINT(buffer);

   /* pteipc_shutdown_single_instance_app(); */
   MainProcessDone = 1;
           
   return;  
   
} /* MainProcessor */
//#if 0
/******************************************************************************
 *
 *  NAME:         Get_duplicate_restrict_flag_from_ini
 *
 *  DESCRIPTION:  This function reads information whether dpulicate restrict flag is enabled.
 *
 *  INPUTS:       None
 *
 *  RTRN VALUE:   1 for enabled; 0 for disabled
 *
 *  AUTHOR:       TF PHANI - ThoughtFocus 
 *
 ******************************************************************************/
void Get_duplicate_restrict_flag_from_ini( void )
{
	#define MAX_INI_INFO_SIZE   20
   #define MAX_FILE_NAME_SZIE 128

	CHAR filename     [MAX_FILE_NAME_SZIE]={0};
	CHAR tmpstr       [MAX_FILE_NAME_SZIE]={0};

	CHAR tmpstring    [MAX_INI_INFO_SIZE]={0};
	CHAR ini_info     [MAX_INI_INFO_SIZE]={0};

   INT nARState_temp   = 0;
   INT nARState_retval = 1;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILE_NAME_SZIE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   /* See if duplicate flag is turned on */
   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
#ifndef WIN32
   GetPrivateProfileString(
             "EFTPOS",                   /* points to section name       */
             "DUPLICATE_RESTRICT",                         /* points to key name           */
             "1",                            /* points to default string     */
             ini_info,                       /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,          /* size of destination buffer   */
             filename                        /* points to ini filename       */
             );
#endif
   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );

   Duplicate_Restrict = atoi(tmpstring);

   return;
}
//#endif


/*****************************************************************************
*
*  FUNCTION:         calc_julian_day
*
*  DESCRIPTION:      This function will calculate the julian day
*
*  INPUTS:           date_str -  input string format (YYYYMMDD)
*
*  OUTPUTS:          julian_str - output string format (DDD)
*
*  RETURN VALUE:     None  
*
******************************************************************************/
void calc_julian_day(char date_str[], char julian_str[])
{
   char              year[5];
   char              month[3];
   char              day[3];
   int               iYear;
   int               iMonth;
   int               iDay;
   int               julian_day;
   int               i;
   int               DaysOfMonth[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };

   memset (&year,  0, sizeof(year));
   memset (&month, 0, sizeof(month));
   memset (&day,   0, sizeof(day));
   memcpy (&year,  &date_str[0], 4);
   memcpy (&month, &date_str[4], 2);
   memcpy (&day,   &date_str[6], 2);

   iYear  = atoi(year);
   iMonth = atoi(month);
   iDay   = atoi(day);

   for(i=0,julian_day=0; i<iMonth-1 && i<12; i++)
   {
		julian_day += DaysOfMonth[i];
   }
   julian_day += iDay;

   if (iMonth > 2)
      if (0 == (iYear % 4))
          julian_day++;
   
   itoa(julian_day, julian_str, 10);

   Rj_with_lead_zeros (julian_str, 3);

} /* calc_julian_day */

/************************* BIN 2 ASCII ****************************************/

void genutil_bin_to_ascii (BYTE bstring[], INT blen, pCHAR astr, INT astr_len)
{
   INT  i = 0;

   memset (astr, 0x00, astr_len);

   for (i = 0; i < (blen * 2); i+=2)
   {
      if ((bstring[i/2] / 16) <= 9) 
        astr[i] = (bstring[i/2] / 16) + '0';
      else
        astr[i] = (bstring[i/2] / 16 - 0x0a)+'A';

      if ((bstring[i/2] & 0x0f) <= 9)
        astr[i+1] = (bstring[i/2] & 0x0F) + '0'; 
      else
        astr[i+1] = (bstring[i/2] & 0x0F) - 0x0a + 'A';    
   }
} /* genutil_bin_to_ascii */

/*****************************************************************************/


/*******************************************************************************
*
*  FUNCTION:         create_credit_detail
*
*  DESCRIPTION:      This function creates detail structure for 
*                    the credit settlement file. 
*
*  INPUTS:           None
*         
*  OUTPUTS:          pCHAR c_record -  detail structure 
*
*  RETURN VALUE:     True if record should be written to file, else false
*
*   
*******************************************************************************/
INT create_credit_detail()
{
   INT   retval = true,max_etran=0,i,elength=0;
   BYTE  term_id[16]={0};BYTE term_filler[15]={0};
   CHAR  rrn[13] = "";
  // CHAR  emv_seg13[192],emv_seg14[192],emv_seg15[192];
   CHAR  descriptor_code[3] = "",cad_merchant_id[12]={0}; 
   CHAR  tran_dt[7]={0},temp_trandate[10]={0},entry_mode[5] ={0};	
   BYTE  pos_data[12]="";
   BYTE  temp_string[2]="";
   INT   iIndustry=0;
   BYTE  eci[4]={0};
   BYTE  tran_id[16]={0},temp_id[2]="",settle_date[5]="",temp_date[2]="";
   BYTE	 temp_entry_mode[4] = {0},temp_card[10]={0},card_nbr[20]={0};   
   BYTE  returncode=0;   
   BYTE  pan_sequence_number[4]={0};
   BYTE  perch_id[PERCH_ID_LENGTH]={0}; // ID to store De63 table id -12 of messages 27 charcters- TF Phani
     
   memset(tran_dt,0x00,7);   
   memset(c_record, 0x00, sizeof(c_record));
   memset(term_id, 0x00, sizeof(term_id));
   memset(card_nbr, 0x00, sizeof(card_nbr));
   memset(cad_merchant_id, 0x00, sizeof(cad_merchant_id));
   memset(eci, 0x00, sizeof(eci));
   memset(tran_id,0, sizeof(tran_id));
   memset(pos_data, 0x00, sizeof(pos_data));

    // Fill 6 byte with spaces for handling RPS traction.
   memset(perch_id,0x20,CUSTOM_IDENTIFIER_TYPE_LENGTH);
	strcpy(terminal.industry_code,"800");
	strcpy(terminal.category_code,"4784");
	strcpy(curr_code,"608");
	strcpy(device_type,"03");
	
   /* Message Type */
    if (auth_tx_detail.tx_key == AUTH_REFUND)
	   {
		  strcpy(auth_tx_detail.message_type,"0400"); /* For Reversal Transaction */
	   }
	else
	      strcpy(auth_tx_detail.message_type,"0000"); /* For Original Transaction */

	/* Terminal ID - Left Justified ; Space Filled */

    if((strncmp(auth_tx_detail.card_nbr,"34",2)==0) || (strncmp(auth_tx_detail.card_nbr,"37",2)==0)) /* AMEX Terminal Id - 15 chars + 1 Space Filled */
	 {
		memcpy(&term_id,auth_tx_detail.primary_key.device_id,15);
		
	}
	else   /* VISA,JCB,MCARD Terminal ID */
	{
		memcpy(&term_id,auth_tx_detail.primary_key.device_id,8);
		memset(term_id+8,' ',7);	
		
	}

	if((strncmp(auth_tx_detail.card_nbr,"34",2)==0) || (strncmp(auth_tx_detail.card_nbr,"37",2)==0)) /* AMEX Card Number - 15 numeric + 4 Space Filled */
	 {
		memcpy(&card_nbr,auth_tx_detail.card_nbr,15);
		memset(&card_nbr[15],0x20,4);		
	}
	else                                          /* VISA,JCB,MC Card Number - 16 numeric + 3 Space Filled */
	{
		memcpy(&card_nbr,auth_tx_detail.card_nbr,16);
		memset(&card_nbr[16],0x20,3);
		//printf("%s - %s ",card_nbr,auth_tx_detail.card_nbr);
	}

	  if(auth_tx_detail.tx_key == AUTH_REVERSAL)  /* Processing Code for REVERSAL */
		{
		  memcpy(&auth_tx_detail.processing_code, "20", 2 );
		}
	  //AUTH_QUASI_CASH
	  if(auth_tx_detail.tx_key == AUTH_QUASI_CASH)  /* Processing Code for QUASI CASH */
		{
		   memcpy(&auth_tx_detail.processing_code, "00", 2 );
		}

	 
	  /* TF PHANI,Chandra - Fix fo processing code for Batch Upload and CA Txns */

	  if((0 == strcmp("0320",auth_tx_detail.message_type)) || (auth_tx_detail.processing_code[5] == '1'))  /* Batch Upload */
	  {
		  memcpy(&auth_tx_detail.processing_code, "000000", 6 ); 
	  }

	  if((auth_tx_detail.tx_key == AUTH_CASH_ADVANCE_AUTHORIZATION) || (auth_tx_detail.tx_key == AUTH_CASH_ADVANCE) || (strcmp(auth_tx_detail.processing_code,"010000") == 0)) /* Cash Advance */

	  {
		  memcpy(&auth_tx_detail.processing_code, "010000", 6 );
	  }
    

     memcpy(cad_merchant_id,auth_tx_detail.merchant_id+5, 10 ); /* Cadence Merchant ID */ 

	 /* POS Data Logic */
	
	 
	   if((strncmp(auth_tx_detail.card_nbr,"35",2)==0)) /* JCB Card POS Data */
	{
		  	
		   		   
		   /* card swipe */

		   if(strncmp(auth_tx_detail.pos_entry_mode,"002",3) == 0)
               strncpy(pos_data,"0220000",7);

		   /* manual entry */

           else if(strncmp(auth_tx_detail.pos_entry_mode,"001",3) == 0)
               strncpy(pos_data,"0100000",7);
         
		   /* CHIP */

           else if(strncmp(auth_tx_detail.pos_entry_mode,"005",3) == 0)
               strncpy(pos_data,"0510000",7);

		   /* Fallback */

		   else if(strncmp(auth_tx_detail.pos_entry_mode,"008",3) == 0)
               strncpy(pos_data,"9700000",7);
		   
		   /* New POS entry mode for BDO - TF Phani*/
			 else if(strncmp(auth_tx_detail.pos_entry_mode,"0800",4) == 0)
				strncpy(pos_data,"9700000",7);
			 else if(strncmp(auth_tx_detail.pos_entry_mode,"0801",4) == 0)
				strncpy(pos_data,"9700000",7);
			 else if(strncmp(auth_tx_detail.pos_entry_mode,"0802",4) == 0)
				strncpy(pos_data,"9700000",7);

		    /* E-Commerce */

		   if((0==strncmp(auth_tx_detail.product_codes[6].amount,"59",2)) || (strncmp(auth_tx_detail.pos_entry_mode,"0081",4) == 0))
               strncpy(pos_data,"8120000",7);		   
		   

		   /* RPS  */

		    iIndustry = atoi(terminal.industry_code );

		   if((strncmp(auth_tx_detail.product_codes[6].amount,"08",2) == 0))
		   {
			   /*strncpy(pos_data,"0100800",7); TF PHANI - Do not copy this */
			   strncpy(pos_data+7,"11",2);			  
			   strncpy(pos_data+3,"08",2);			 
		   }			   
		 

		  else if (iIndustry == MAIL_PHONE)
			  {
				  strncpy(pos_data+7,"1",1);
				  strncpy(pos_data+3,"08",2);
				  strncpy(pos_data+8,"1",1);
			  } 
			  else
			  {
				  strncpy(pos_data+7,"2",1);				 
				  strncpy(pos_data+8,"2",1);
			  } 

			 if((strncmp(auth_tx_detail.product_codes[6].amount,"08",2) == 0) && (iIndustry != MAIL_PHONE)) /* For RPS and Non-MOTO */ 
			 {
				  strncpy(pos_data+7,"21",2);			  
			 }
				 
			  
			  /* ATM - For checking pin exists or not */ 

		   if(strncmp(auth_tx_detail.pos_entry_mode,"021",3) == 0)
               memset(&pos_data[2],0x31,1);
		   else
			   memset(&pos_data[2],0x32,1);         

		   
		   
	}

	  else if(auth_tx_detail.card_nbr[0]=='4') /* VISA POS Data */
	{
          		  
		  
		  
		  /* card swipe */

           if(strncmp(auth_tx_detail.pos_entry_mode,"002",3) == 0)
               strncpy(pos_data,"900050000",9);

		   /* manual entry */

           else if(strncmp(auth_tx_detail.pos_entry_mode,"001",3) == 0)
               strncpy(pos_data,"012050000",9);

         
		   /* CHIP */

           else if(strncmp(auth_tx_detail.pos_entry_mode,"005",3) == 0)
               strncpy(pos_data,"050050000",9);
		    else if(strncmp(auth_tx_detail.pos_entry_mode,"007",3) == 0)
               strncpy(pos_data,"072050000",9);

		   /* FALLBACK */

		   else if(strncmp(auth_tx_detail.pos_entry_mode,"008",3) == 0)
               strncpy(pos_data,"900052000",9);
          /* New POS entry mode for BDO - TF Phani*/
			else if(strncmp(auth_tx_detail.pos_entry_mode,"0800",4) == 0)
               strncpy(pos_data,"950000000",9);
            else if(strncmp(auth_tx_detail.pos_entry_mode,"0801",4) == 0)
               strncpy(pos_data,"950000000",9);
            else if(strncmp(auth_tx_detail.pos_entry_mode,"0802",4) == 0)
               strncpy(pos_data,"950000000",9);

		   /* E-Commerce */

		    if((0==strncmp(auth_tx_detail.product_codes[6].amount,"59",2)) || (strncmp(auth_tx_detail.pos_entry_mode,"0081",4) == 0))
            {   
				strncpy(pos_data,"010500007",9);	  
					/*ECI value for VISA - ECI value should be copied to 88-89 position*/
				strncpy(eci,"07 ",3);
				if(strcmp(auth_tx_detail.profile,"")!=0)
				{
				 strncpy(eci,auth_tx_detail.profile,2);
				 strncpy(&pos_data[7],auth_tx_detail.profile,2);
				}
			}
		   

		   /* RPS */
		   
		    else if(strncmp(auth_tx_detail.product_codes[6].amount,"08",2) == 0) 
		   {
			   strncpy(pos_data,"0100000",7);			   
			   strncpy(pos_data+7,"08",2);
			   memset(&pos_data[9],0x52,1);
		   }   
			 
		
		   if(strncmp(auth_tx_detail.product_codes[6].amount,"08",2) == 0) /* Rec Txn - R ,Non-Rec Txn - Space */
		   {
				memset(&pos_data[9],0x52,1);
		   }
		   else
                memset(&pos_data[9],0x20,1);

		   /* ATM - For checking pin exists or not */ 

		  if(strncmp(auth_tx_detail.pos_entry_mode,"021",3) == 0)
               memset(&pos_data[2],0x31,1);
		   else
			   memset(&pos_data[2],0x32,1); 

		    iIndustry = atoi(terminal.industry_code );

			if (iIndustry == MAIL_PHONE) /* MOTO */
			{
				strncpy(pos_data+7,"08",2);
			}
/*			else if(strcmp(auth_tx_detail.profile,"")!=0) // TF Phani- VISA 
			{
				strncpy(pos_data+7,auth_tx_detail.profile,2);
			}
			else
			{
				strncpy(pos_data+7,"00",2);
			}*/

		
	 }

     else if((strncmp(auth_tx_detail.card_nbr,"34",2)==0) || (strncmp(auth_tx_detail.card_nbr,"37",2)==0)) /* AMEX POS Data */
	 {

		 /* We need only last 3 characters of POS entry mode
          * coming from terminal for processing further.
          */ 
          if((strncmp(auth_tx_detail.processing_code,"0040",4) == 0) && (strncmp(auth_tx_detail.message_type,"0220",4) == 0) )
		 {
		 	strncpy(auth_tx_detail.processing_code,"0000",4); // Req from BDO for AMEX proc code
		 }

         if(strlen(auth_tx_detail.pos_entry_mode) > 3)
            strncpy(temp_entry_mode,&auth_tx_detail.pos_entry_mode[1],3);
         else
            strcpy(temp_entry_mode,auth_tx_detail.pos_entry_mode);
         
		   /* CHIP */

         if(strncmp(auth_tx_detail.pos_entry_mode,"005",3) == 0)
               strncpy(pos_data,"500U01501001",EPOS_POS_DATA_SIZE);
		 else if(strncmp(auth_tx_detail.pos_entry_mode,"007",3) == 0)
               strncpy(pos_data,"500U0X501001",EPOS_POS_DATA_SIZE);
         else if((strncmp(temp_entry_mode,"02",2) == 0) && (strncmp(temp_entry_mode,"021",3) != 0)) /* card swip without 4DBC */
         {                           
            strncpy(pos_data,"260101254140",EPOS_POS_DATA_SIZE);
         }

         else if(strncmp(temp_entry_mode,"06",2) == 0) /* card swip with 4DBC */
		 {
            strncpy(pos_data,"260101W54140",EPOS_POS_DATA_SIZE);
         }
         else if((strncmp(temp_entry_mode,"01",2) == 0) && (strncmp(auth_tx_detail.product_codes[6].amount,"08",2) != 0) && (strncmp(auth_tx_detail.product_codes[6].amount,"59",2) != 0)) /* manual key in without 4DBC */
         {
            strncpy(pos_data,"660101654140",EPOS_POS_DATA_SIZE);
         }
		 else if((strncmp(temp_entry_mode,"07",2) == 0) && (strncmp(auth_tx_detail.product_codes[6].amount,"08",2) != 0) && (strncmp(auth_tx_detail.product_codes[6].amount,"59",2) != 0)) /* manual key in with 4DBC */ 
		 {
            strncpy(pos_data,"660101S54140",EPOS_POS_DATA_SIZE);
         }
		 else if((strncmp(temp_entry_mode,"012",3) == 0) && (strncmp(auth_tx_detail.product_codes[6].amount,"08",2) == 0) &&
			      ((0 == strcmp(terminal.industry_code,"800")) )) /* RPS Transactions without 4DBC */ /* Changed the code for POS entry mode of position 5 from'2' to '9' */
				                                                                                      /* Girija Y TF, March 02, 2009 */ /* RPS Transactions without 4DBC */
         {
            strncpy(pos_data,"660390664130",EPOS_POS_DATA_SIZE);
		 }
         else if((strncmp(temp_entry_mode,"07",2) == 0) && (strncmp(auth_tx_detail.product_codes[6].amount,"08",2) == 0) &&
			      ((0 == strcmp(terminal.industry_code,"800")) )) /* RPS Transactions without 4DBC */ /* Changed the code for POS entry mode of position 5 from'2' to '9' */
				                                                                                      /* Girija Y TF, March 02, 2009 */ /* RPS Transactions with 4DBC */
		 {
            strncpy(pos_data,"660390S64130",EPOS_POS_DATA_SIZE);
         }
		  	 

		 else if((strncmp(temp_entry_mode,"01",2) == 0) && (strncmp(auth_tx_detail.product_codes[6].amount,"59",2) == 0)) /* Ecommerce without 4DBC */
         {
            strncpy(pos_data,"6609S06S4140",EPOS_POS_DATA_SIZE);
		 }
         else if((strncmp(temp_entry_mode,"07",2) == 0) && (strncmp(auth_tx_detail.product_codes[6].amount,"59",2) == 0)) /* Ecommerce with 4DBC */
		 {
            strncpy(pos_data,"6609S0SS4140",EPOS_POS_DATA_SIZE);
         }

		 else if(strncmp(temp_entry_mode,"021",3) == 0) /* ATM */
		 {
			strncpy(pos_data,"210111210141",EPOS_POS_DATA_SIZE);
		 }
		  else if(strncmp(temp_entry_mode,"80",2) == 0) /*Fall Back */
		 {
			strncpy(pos_data,"500U01901001",EPOS_POS_DATA_SIZE);
		 }	 


	}

	else  if(auth_tx_detail.card_nbr[0]=='5') /* Master Card POS Data */
	 {
	 
		   
		   
	   /* card swipe */

       if(strncmp(auth_tx_detail.pos_entry_mode,"002",3) == 0)
       strncpy(pos_data,"902000000800",EPOS_POS_DATA_SIZE);

	   /* manual entry */

	   else if(strncmp(auth_tx_detail.pos_entry_mode,"001",3) == 0)
       strncpy(pos_data,"010000000700",EPOS_POS_DATA_SIZE);
         
	   /* CHIP */

       else if(strncmp(auth_tx_detail.pos_entry_mode,"005",3) == 0)
       strncpy(pos_data,"051000000800",EPOS_POS_DATA_SIZE); 
	   else if(strncmp(auth_tx_detail.pos_entry_mode,"007",3) == 0) /*Pay pass CHip Contactless*/
	   strncpy(pos_data,"072000000300",EPOS_POS_DATA_SIZE);
	   else if(strncmp(auth_tx_detail.pos_entry_mode,"009",3) == 0) /*MC Pay pass -Contactless Mag stripe*/
	   strncpy(pos_data,"091000000400",EPOS_POS_DATA_SIZE);
	   
	   /* FALLBACK */
   	   
	   else if(strncmp(auth_tx_detail.pos_entry_mode,"008",3) == 0)
       strncpy(pos_data,"802000000800",EPOS_POS_DATA_SIZE); 
	   /* New POS entry mode added for MC */
	   else if(strncmp(auth_tx_detail.pos_entry_mode,"0800",4) == 0)
       strncpy(pos_data,"800000000800",EPOS_POS_DATA_SIZE);   

	   else if(strncmp(auth_tx_detail.pos_entry_mode,"0801",4) == 0)
       strncpy(pos_data,"801000000800",EPOS_POS_DATA_SIZE);   

	   else if(strncmp(auth_tx_detail.pos_entry_mode,"0802",4) == 0)
       strncpy(pos_data,"802000000800",EPOS_POS_DATA_SIZE);   

	   /* e-commerce  */
	   if((0==strncmp(auth_tx_detail.product_codes[6].amount,"59",2)) || (0==strncmp(auth_tx_detail.pos_entry_mode,"0081",4)))
	   {
	       strncpy(pos_data,"810125106000",EPOS_POS_DATA_SIZE); 	
	   } /* RPS */
	   else if(strncmp(auth_tx_detail.product_codes[6].amount,"08",2) == 0)
	   {
		   strncpy(pos_data,"010004100000",EPOS_POS_DATA_SIZE);
	   }
		 
		 	/* ATM - For checking pin exists or not */ 

		   if(strncmp(auth_tx_detail.pos_entry_mode,"021",3) == 0)
               memset(&pos_data[2],0x31,1);
		   else
			   memset(&pos_data[2],0x32,1);

		   if ((auth_tx_detail.pos_entry_mode[1] == '9') && (strncmp(auth_tx_detail.product_codes[6].amount,"00",2) == 0)) /* Magnetic stripe read capability */
			 {
				strncpy(pos_data,"91",2); // TF PHANI- contactless mag-stripe
			 }
           else if (terminal.device_type[1] == '7' ) /* Terminal has Chip Read capability. */
			 {
			     strncpy(pos_data,"07",2);
			 }
	   

	 }     

	
	/* Ecommerce Indicator Logic */

	 if((strncmp(auth_tx_detail.card_nbr,"34",2)==0) || (strncmp(auth_tx_detail.card_nbr,"37",2)==0) ) /* Space Filling for AMEX Cards ECOM Indicator */
	 { /* Removed VISA from this catageory- TF Phani*/
		 strncpy(eci," ",3);
	 }

	 else if(auth_tx_detail.card_nbr[0]=='5') /* Master Card ECOM Indicator */
	
	 {
           if((0==strncmp(auth_tx_detail.product_codes[6].amount,"59",2) || (0==strncmp(auth_tx_detail.pos_entry_mode,"0081",4)))) /* If E-commerce txn */
		   {
			   if(strcmp(auth_tx_detail.profile,"")!=0)
			   {
			     strncpy(eci,"21",2);
			     memcpy(eci+2,auth_tx_detail.profile+1,1);
			   }
			   else strcpy(eci,"210");
		   }
		   else
		   {
			   strcpy(eci,"000");			   
		   }
     }

	 else if(strncmp(auth_tx_detail.card_nbr,"35",2)==0) /* JCB Card ECOM Indicator */
	 {
		

	    if((0==strncmp(auth_tx_detail.product_codes[6].amount,"59",2))) /* If E-commerce txn */
		   {
			   if(strcmp(auth_tx_detail.profile,"")!=0)
			   {
				 strcpy(eci,auth_tx_detail.profile);
				 strncpy(eci+2," ",1);
				 
			   }
			   else strncpy(eci,"07 ",3);
		   }
		   else
		   {
			  strncpy(eci,"   ",3);			   
		   }
	 }


    /* Transaction ID */

	 if(auth_tx_detail.card_nbr[0]=='5') /* Transaction ID for MASTER CARDS */
	
	 {    
		 // Settlement date MMDD
			strncpy(settle_date,auth_tx_detail.product_codes[19].code,4);//MMDD
			/*If Settle date is NUll, copy tran date as settle date*/
			if(strlen(settle_date) == 0 || settle_date[0] ==  ' ' || settle_date[1] ==  ' ')
			memcpy(settle_date,&auth_tx_detail.tran_date[4],4);	 
			
			if(strlen(settle_date) == 0 || settle_date[0] ==  ' ' || settle_date[1] ==  ' ')
			memcpy(settle_date,CurrentDate+2,4);

		 if(strlen(auth_tx_detail.mcard_banknet)>0)
		 {
			 if(strlen(auth_tx_detail.mcard_banknet)>12)
			 {
			   strncpy(tran_id,auth_tx_detail.mcard_banknet,12);
			   strcat(tran_id,"   ");
			   strncpy(&tran_id[9],settle_date,4);
			 }
			 else 
			 {
                 memset(tran_id,' ',12);  
				 memset(tran_id+12,' ',3); 
				 strncpy(tran_id,auth_tx_detail.mcard_banknet,strlen(auth_tx_detail.mcard_banknet));
			   
				 strncpy(&tran_id[9],settle_date,4);
			   
			 }
		 }
		 else
		 {
			 memset(tran_id,' ',15);
			 //strcat(tran_id,"000");
			 
			 if(strlen(settle_date) == 0)
			 memcpy(settle_date,CurrentDate+2,4);
			 
			 strncpy(&tran_id[9],settle_date,4);
		 } 
	 }

	 else if(auth_tx_detail.card_nbr[0]=='4') /* Transaction ID for VISA */
	 {
	      memset(tran_id,0x30,15);	
		  strncpy(tran_id,auth_tx_detail.mcard_banknet,15);
	 }

	 else if((strncmp(auth_tx_detail.card_nbr,"34",2)==0) || (strncmp(auth_tx_detail.card_nbr,"37",2)==0)) /* Transaction ID for AMEX */
	 {
		 strncpy(tran_id,auth_tx_detail.mcard_banknet,15);
	 }
	 else if((strncmp(auth_tx_detail.card_nbr,"35",2)==0)) /* Transaction ID for JCB - Spaces */
	 {
		 memset(tran_id,' ',15); 
	 }


	 /* PAN Sequence Number */

/*	 if(strcmp(emv_record.tag,"5F34")==0)
	 {
		 strcpy(pan_sequence_number,emv_record.emv_data);
	 }
	 else */
		 strcpy(pan_sequence_number,"000");

	 	if((strncmp(auth_tx_detail.card_nbr,"34",2)==0) || (strncmp(auth_tx_detail.card_nbr,"37",2)==0)) /* For AMEX Card Sequence Number is Zero */
	 {
		 strcpy(pan_sequence_number,"000");		 
	 }

	  strncpy(tran_dt,auth_tx_detail.tran_date+2,6);

	  if(strncmp(auth_tx_detail.product_codes[6].amount+2,"",3)==0)
	  {
		  strncpy(auth_tx_detail.product_codes[6].amount+2,"000",3);
	  }
	
	
	 /* Form 27 byte message for DE 63- table Id 12, TF Phani- 
	  First 15 bytes in - BCh10 -> ticket_nbr 
	  Next  10 bytes in - BCh10 -> product_codes[14].amount*/
	/* TF-- Abhishek
	1) If Field 63 is present in the authorisation message and starts with "PRCHID", place the value of Field 63 in EPOS at segment 11 starting at
	    column 76 until 106.
	2) If Field 63 is present in the authorisation message and doesn't start with "PRCHID", place the values of Field 63 at segment 11 starting at column
	   82 until 106.
	Note :Item 1 is to address for RPS transactions. Item 2 is to address for PolicyNumber coming from POS terminal.
	*/
	if(strncmp(auth_tx_detail.ticket_nbr,"PRCHID",CUSTOM_IDENTIFIER_TYPE_LENGTH)== 0)
	{
		// Print 31 byte starting from 76 until 106
		strncpy(perch_id,auth_tx_detail.ticket_nbr,strlen(auth_tx_detail.ticket_nbr));
		strncat(perch_id,auth_tx_detail.product_codes[14].amount, (sizeof(perch_id)- strlen(auth_tx_detail.ticket_nbr) - CUSTOM_IDENTIFIER_TYPE_LENGTH +1));
	    perch_id[PERCH_ID_LENGTH -1]='\0';
	}
	else
	{
		// 1st 6 byte should be space as feild 63 is not starts whith PRCHID 
		// copy data after 6 byte which will be store from 82nd byte location
		//Print 25 byte starting from 82 until 106
		strncpy(perch_id + CUSTOM_IDENTIFIER_TYPE_LENGTH ,auth_tx_detail.ticket_nbr,strlen(auth_tx_detail.ticket_nbr));
		strncat(perch_id,auth_tx_detail.product_codes[14].amount, (sizeof(perch_id)- strlen(auth_tx_detail.ticket_nbr)- CUSTOM_IDENTIFIER_TYPE_LENGTH +1 ));
		perch_id[PERCH_ID_LENGTH -1]='\0';
	}
		
	 /* writing details to file */

	 /* Segment 10 */


      sprintf(c_record, "%1s%4s%-15s%-019s%6s%6s%2s%6s%012s%06s%4s%-12s%04s%11s%12s%06s%03s%40s%012s%4s%29s%11s%03s%3s%2s%3s%3s%6s",
         "1",
		 auth_tx_detail.message_type,
         term_id,         
         auth_tx_detail.card_nbr,
         tran_dt,
         auth_tx_detail.tran_time,
         "10",  /* Segment 10 Data Follows */
         auth_tx_detail.processing_code,
         auth_tx_detail.tran_amount,
		 "0",
		 terminal.category_code,
		 pos_data, 
		 "0",
		 " ",
		 auth_tx_detail.primary_key.retrieval_ref_num,
		 auth_tx_detail.auth_number,         
		 "0",
		 " ",
		 "0",
		 " ",
		 " ",
		 " ",
		 "0",
		 curr_code,
		 " ",
		 eci, /* E-Commerce Logic for MCRD,JCB ; VISA,AMEX - Spaces */
		 " ",
		 auth_tx_detail.primary_key.batch_nbr		 
         );
	 

      strncpy(c_record + 245, "\n\0", 2);

	  crec_count++;

	  if (write_to_credit_settle_file() == false)
      return (false);	  

	  cseg10_records++;

	  /* Segment 11 */

	  memset(c_record, 0, sizeof(c_record));

	  sprintf(c_record, "%1s%4s%-15s%-019s%6s%6s%2s%4s%012s%3s%3s%-031s%4s%10s%1s%11s%15s%03s%8s%-15s%72s",
         "1",
		 auth_tx_detail.message_type,
         term_id,         
         auth_tx_detail.card_nbr,
         tran_dt,
         auth_tx_detail.tran_time,         
		 "11", /* Segment 11 Data Follows */
         auth_tx_detail.exp_date,
		 "0",
		 auth_tx_detail.product_codes[6].amount+2,
	     pan_sequence_number,
	     perch_id,
		 " ",
         cad_merchant_id,
		 " ",
		 " ",
		 " ",
		 "0",
		 " ",
         tran_id, 
		 " "	 	          	 
         );

      strncpy(c_record + 245, "\n\0", 2);

	  crec_count++;
	  

	  if (write_to_credit_settle_file() == false)
      return (false);


	  /* Segment 13 - EMV Data */


     memset(emv_seg13, 0x00, sizeof(emv_seg13));
	 memset(emv_seg14, 0x00, sizeof(emv_seg14));
	 memset(emv_seg15, 0x00, sizeof(emv_seg15));
	     
     memset(c_record, 0x00, sizeof(c_record));

	// if((strncmp(auth_tx_detail.card_nbr,"34",2)!=0) && (strncmp(auth_tx_detail.card_nbr,"37",2)!=0) && ((strcmp(auth_tx_detail.pos_entry_mode,"0051")==0) || (strcmp(auth_tx_detail.pos_entry_mode,"0052")==0) || (strcmp(auth_tx_detail.pos_entry_mode,"0072")==0))) /* EMV Data is not present in AMEX */
	 if((strcmp(auth_tx_detail.pos_entry_mode,"0051")==0) || (strcmp(auth_tx_detail.pos_entry_mode,"0052")==0) || (strcmp(auth_tx_detail.pos_entry_mode,"0072")==0)) /* EMV Data is not present in AMEX */
	 {	  

        max_etran = atoi(Emv_list.num_returned);		
		
		if(max_etran==0) return false;

		for (i = 0; i < max_etran; i++)
		{
			memset(emv_data, 0, sizeof(emv_data));
			memset(&emv_record, 0, sizeof(emv_data));
			memcpy(&emv_record,&Emv_list.batch_list[i],sizeof(emv_record));			
         
			elength=2*atoi(emv_record.DATA_LENGTH);					

		//	bin_to_ascii(emv_data,emv_record.RAW_DATA,elength/2);	
		//	bin_to_ascii(emv_record.RAW_DATA,emv_data,elength/2);

			/* Parse the EMV raw data to wrtie it in  segments - Ajay-TF, 18-Jun-2009 */
        	parseEmvDataToSegment(elength, emv_seg13, emv_seg14, emv_seg15);	 

		}		
	 		
		 memset(&auth_tx_detail.tran_time[6],'\0',1);
		 memset(&term_id[15],'\0',1);
         memset(c_record, 0x00, sizeof(c_record));

		 if((strlen(emv_seg13)!=0))
		 {

		    sprintf(c_record, "%1s%04s%-15s%-019s%06s%06s%2s%-192s",
			 "1",
			 auth_tx_detail.message_type,
			 term_id,         
			 card_nbr,
			 tran_dt,
			 auth_tx_detail.tran_time,         
			 "13", /* Segment 13 Data Follows */		
			 emv_seg13			 	         
			 );

		     strncpy(c_record + 245, "\n\0", 2);

		     crec_count++;

			 if((strlen(emv_seg14)==0)) return(true);
			 
			 if (write_to_credit_settle_file() == false)
             return (false);

		 }

		  memset(c_record, 0x00, sizeof(c_record));
		   memset(&auth_tx_detail.tran_time[6],'\0',1);
		 memset(&term_id[15],'\0',1);	
		  if((strlen(emv_seg14)!=0))
		 {

			 sprintf(c_record, "%1s%04s%-15s%-019s%06s%06s%2s%-192s",
			 "1",
			 auth_tx_detail.message_type,
			 term_id,         
			 card_nbr,
			 tran_dt,
			 auth_tx_detail.tran_time,         
			 "14", /* Segment 14 Data Follows */		
			 emv_seg14			 	         
			 );

		     strncpy(c_record + 245, "\n\0", 2);

		     crec_count++;	

			 if((strlen(emv_seg15)==0)) return(true);
			 
			 if (write_to_credit_settle_file() == false)
             return (false);
		 }
		  memset(&auth_tx_detail.tran_time[6],'\0',1);
  	      memset(&term_id[15],'\0',1);
		  memset(c_record, 0x00, sizeof(c_record));
		 
		    if((strlen(emv_seg15)!=0))
		 {

			 sprintf(c_record, "%1s%04s%-15s%-019s%06s%06s%2s%-192s",
			 "1",
			 auth_tx_detail.message_type,
			 term_id,         
			 card_nbr,
			 tran_dt,
			 auth_tx_detail.tran_time,         
			 "15", /* Segment 15 Data Follows */		
			 emv_seg15			 	         
			 );

		     strncpy(c_record + 245, "\n\0", 2);

		     crec_count++;				   		 

		 }
	}
	       
	 
         return(true);

} /* create_credit_detail */

/*****************************************************************************
  Function : parseEmvDataToSegment

  Description: This function parse the emv raw data and writes in segment. 
               if length less than 192 it writes in segment 13
			   else if length is gretaer than 192 adn less than 384 
			   first 192 length data writes in segment 13 and remainiang data in segment 14
			   else 
			   first 192 length data writes in a segment 13, 
			   next 192 length of data writes in segment 14 and
			   remaining in segment 15
  Author: 
             Ajay @ ThoughtFocus, Bangaluru
  Inputs:
             elength- Data length
  Outputs:
             emv_seg13, emv_seg14 and emv_seg15
  Return values:
			 None
  
*******************************************************************************/
void parseEmvDataToSegment(INT elength, CHAR emv_seg13[], CHAR emv_seg14[], CHAR emv_seg15[])
{
   INT    fieldLength;
   INT    dataIndex;
   INT    bTwoBytesTag;
   BYTE   byteOneOfTag;
   BYTE   byteTwoOfTag;
   CHAR   tempstr[500] = {0};
   INT	  dataLen13=0;
   INT	  dataLen14=0;
   INT	  dataLen15=0;
   INT	  totLen=0;
  
	
	if(elength<192)
	{
		bin_to_ascii(emv_record.RAW_DATA,emv_data,elength/2);
		memcpy(&emv_seg13,emv_data,elength);
	}
	else 
	{
		 dataIndex=0;
		 dataLen13=0;
		 dataLen14=0;
		 dataLen15=0;
		 while (dataIndex<= elength/2)
		 {
			 bTwoBytesTag = 0;
			 byteOneOfTag = emv_record.RAW_DATA[dataIndex];
			 dataIndex++;
			 if ((byteOneOfTag & 0x1F) == 0x1F)
			 {
				bTwoBytesTag = 1;
				byteTwoOfTag = emv_record.RAW_DATA[dataIndex];
				dataIndex++;
			 }
			 fieldLength = emv_record.RAW_DATA[dataIndex];
			 // Field length validation ..considering worst case length 36
			 if(fieldLength > EMV_TAG_MAX_LEN)
			 	return; // Some thing wrong with length ..return
			 dataIndex++;
             if(fieldLength!=0)
			 {
				 if (bTwoBytesTag)
				 {
					bin_to_ascii (&emv_record.RAW_DATA[dataIndex-3],  &tempstr[0], fieldLength+3 );
					dataIndex += fieldLength;
					totLen+=fieldLength*2+6;
					if(totLen<=192)
					{ 
						 memcpy(emv_seg13+dataLen13,&tempstr,fieldLength*2+6);
						 dataLen13+=fieldLength*2+6;
					}
					else if(totLen>192 && totLen<384)
					{ 
						 memcpy(emv_seg14+dataLen14,&tempstr,fieldLength*2+6); 
						 dataLen14+=fieldLength*2+6;
					}
					else if(totLen>=384 && totLen<512)
					{
						 memcpy(emv_seg15+dataLen15,&tempstr,fieldLength*2+6); 
						 dataLen15+=fieldLength*2+6;
					}
					memset(&tempstr,0,sizeof(tempstr));
				}
				else
				{
					bin_to_ascii (&emv_record.RAW_DATA[dataIndex-2],  &tempstr[0], fieldLength+2 );
					dataIndex += fieldLength;
					totLen+=fieldLength*2+4;
					if(totLen<=192)
					{ 
						memcpy(emv_seg13+dataLen13,&tempstr,fieldLength*2+4);
						dataLen13+=fieldLength*2+4;
					}
					else if(totLen>192 && totLen<384)
					{ 
						memcpy(emv_seg14+dataLen14,&tempstr,fieldLength*2+4);
						dataLen14+=fieldLength*2+4;
					}
					else if(totLen>=384 && totLen<512)
					{
						memcpy(emv_seg15+dataLen15,&tempstr,fieldLength*2+4);
						dataLen15+=fieldLength*2+4;
					}
					memset(tempstr,0,sizeof(tempstr));
				}
			}
		}
	}	
}



/*******************************************************************************
*
*  FUNCTION:         create_credit_detail_arc
*
*  DESCRIPTION:      This function creates detail structure for the credit
*                    settlement file from the batch archive file (BCH11).
*
*  INPUTS:           None
*         
*  OUTPUTS:          pCHAR c_record -  detail structure 
*
*  RETURN VALUE:     True if record should be written to file, else false

*******************************************************************************/
INT create_credit_detail_arc()
{
   INT   retval = true,max_etran,i,elength;
   BYTE  term_id[16]={0};BYTE term_filler[15]={0};
   BYTE  pos_data[12]="";
   BYTE  temp_string[2]="";
   CHAR  rrn[13] = "";
//   CHAR  emv_seg13[192],emv_seg14[192],emv_seg15[192];
   CHAR  descriptor_code[3] = "",cad_merchant_id[12]={0}; 
   CHAR  tran_dt[7]={0},temp_trandate[10]={0},entry_mode[5] ={0};	
   INT   iIndustry=0;
   BYTE  eci[4]={0};
   BYTE  tran_id[16]={0},temp_id[2]="",settle_date[5]="",temp_date[2]="";
   BYTE	 temp_entry_mode[4] = {0},card_nbr[20]={0};   
   BYTE  returncode=0;  
   BYTE  pan_sequence_number[4]={0};
   BYTE  perch_id[PERCH_ID_LENGTH]={0}; // ID to store De63 table id -12 of messages 27 charcters- TF Phani
   BYTE temp_print[100];
   memset(tran_dt,0x00,7);   

   memset(c_record, 0x00, sizeof(c_record));
   memset(term_id, 0x00, sizeof(term_id));
   memset(card_nbr, 0x00, sizeof(card_nbr));
   memset(cad_merchant_id, 0x00, sizeof(cad_merchant_id));
   memset(eci, 0x00, sizeof(eci));
   memset(tran_id, 0x00, sizeof(tran_id));
   memset(pos_data, 0x00, sizeof(pos_data));
   // Fill 6 byte with spaces for handling RPS traction.
   memset(perch_id,0x20,CUSTOM_IDENTIFIER_TYPE_LENGTH);
	strcpy(terminal.industry_code,"800");
	strcpy(terminal.category_code,"4784");
	strcpy(curr_code,"608");
	strcpy(device_type,"03");
   /* Message Type */
    if (auth_tx_detail_arc.tx_key == AUTH_REFUND)
	   {
		  strcpy(auth_tx_detail_arc.message_type,"0400"); /* For Reversal Transaction */
	   }
	else
	      strcpy(auth_tx_detail_arc.message_type,"0000"); /* For Original Transaction */

	/* Terminal ID - Left Justified ; Space Filled */

	if((strncmp(auth_tx_detail_arc.card_nbr,"34",2)==0) || (strncmp(auth_tx_detail_arc.card_nbr,"37",2)==0)) /* AMEX Terminal Id - 15 chars + 1 Space Filled */	
	 {
		memcpy(&term_id,auth_tx_detail_arc.primary_key.device_id,15);
		
	}
	else    /* VISA,JCB,MCARD Terminal ID */
	{
		memcpy(&term_id,auth_tx_detail_arc.primary_key.device_id,8);
		memset(term_id+8,' ',7);	
		
	}

	if((strncmp(auth_tx_detail_arc.card_nbr,"34",2)==0) || (strncmp(auth_tx_detail_arc.card_nbr,"37",2)==0))  /* AMEX Card Number - 15 numeric + 4 Space Filled */
	 {
		memcpy(&card_nbr,auth_tx_detail_arc.card_nbr,15);
		memset(&card_nbr[15],0x20,4);		
	}
	else                                          /* VISA,JCB,MC Card Number - 16 numeric + 3 Space Filled */
	{
		memcpy(&card_nbr,auth_tx_detail_arc.card_nbr,16);
		memset(&card_nbr[16],0x20,3);
		
	}

	  if(auth_tx_detail_arc.tx_key == AUTH_REVERSAL)  /* Processing Code */
		{
		  memcpy(&auth_tx_detail_arc.processing_code, "20", 2 );
		}
		  //AUTH_QUASI_CASH
	  if(auth_tx_detail_arc.tx_key == AUTH_QUASI_CASH)  /* Processing Code for QUASI CASH */
		{
		   memcpy(&auth_tx_detail_arc.processing_code, "00", 2 );
		}
	  
	  // TF PHANI,Chandra - Fix fo processing code for Batch Upload and CA Txns

      if((0 == strcmp("0320",auth_tx_detail_arc.message_type)) || (auth_tx_detail_arc.processing_code[5] == '1'))  /* Batch Upload */
	  {
		  memcpy(&auth_tx_detail_arc.processing_code, "000000", 6 ); 
	  }

	   if((auth_tx_detail_arc.tx_key == AUTH_CASH_ADVANCE_AUTHORIZATION) || (auth_tx_detail_arc.tx_key == AUTH_CASH_ADVANCE) || (strcmp(auth_tx_detail_arc.processing_code,"010000") == 0)) /* Cash Advance */

	  {
		  memcpy(&auth_tx_detail_arc.processing_code, "010000", 6 );
	  }

    memcpy(cad_merchant_id,auth_tx_detail_arc.merchant_id+5, 10 ); /* Cadence Merchant ID */ 

	 /* POS Data Logic */
	 
	 if((strncmp(auth_tx_detail_arc.card_nbr,"35",2)==0)) /* JCB Card POS Data */
	{
		              
		   
		   /* card swipe */

		   if(strncmp(auth_tx_detail_arc.pos_entry_mode,"002",3) == 0)
               strncpy(pos_data,"0220000",7);

		   /* manual entry */

           else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"001",3) == 0)
               strncpy(pos_data,"0100000",7);
         
		   /* CHIP */

           else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"005",3) == 0)
               strncpy(pos_data,"0510000",7);

		   /* Fallback */

		   else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"008",3) == 0)
               strncpy(pos_data,"9700000",7);
		   /* New POS entry mode for BDO - TF Phani*/
			 else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"0800",4) == 0)
				strncpy(pos_data,"9700000",7);
			 else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"0801",4) == 0)
				strncpy(pos_data,"9700000",7);
			 else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"0802",4) == 0)
				strncpy(pos_data,"9700000",7);


		    /* E-Commerce */

		   if((0==strncmp(auth_tx_detail_arc.product_codes[6].amount,"59",2)) || (strncmp(auth_tx_detail_arc.pos_entry_mode,"0081",4) == 0))
               strncpy(pos_data,"8120000",7);

		   

		   /* RPS  */

		    iIndustry = atoi(terminal.industry_code );

		   if((strncmp(auth_tx_detail_arc.product_codes[6].amount,"08",2) == 0))
		   {
			   /*strncpy(pos_data,"0100800",7); TF PHANI - Do not copy this */
			   strncpy(pos_data+7,"11",2);			  
			   strncpy(pos_data+3,"08",2);			 
		   }			   
		 

            else  if (iIndustry == MAIL_PHONE) /* MOTO */
			  {
				  strncpy(pos_data+7,"1",1);
				  strncpy(pos_data+3,"08",2);
				  strncpy(pos_data+8,"1",1);
			  } 
			  else
			  {
				  strncpy(pos_data+7,"2",1);				 
				  strncpy(pos_data+8,"2",1);
			  } 

			 if((strncmp(auth_tx_detail_arc.product_codes[6].amount,"08",2) == 0) && (iIndustry != MAIL_PHONE)) /* For RPS and Non-MOTO */ 
			 {
				  strncpy(pos_data+7,"21",2);			  
			 }
		
			  
			   /* ATM - For checking pin exists or not */ 

		   if(strncmp(auth_tx_detail_arc.pos_entry_mode,"021",3) == 0)
               memset(&pos_data[2],0x31,1);
		   else
			   memset(&pos_data[2],0x32,1);

		   
		   
	}

	  else if(auth_tx_detail_arc.card_nbr[0]=='4') /* VISA POS Data */
	{
          	  
		  		  
		  /* card swipe */

           if(strncmp(auth_tx_detail_arc.pos_entry_mode,"002",3) == 0)
               strncpy(pos_data,"900050000",9);

		   /* manual entry */

           else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"001",3) == 0)
               strncpy(pos_data,"012050000",9);

         
		   /* CHIP */

           else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"005",3) == 0)
               strncpy(pos_data,"050050000",9);
	       else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"007",3) == 0)
               strncpy(pos_data,"072050000",9);
			
		   /* FALLBACK */

		   else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"008",3) == 0)
               strncpy(pos_data,"900052000",9);		    
			        /* New POS entry mode for BDO - TF Phani*/
			else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"0800",4) == 0)
               strncpy(pos_data,"950000000",9);
            else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"0801",4) == 0)
               strncpy(pos_data,"950000000",9);
            else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"0802",4) == 0)
               strncpy(pos_data,"950000000",9);  

		   /* E-Commerce */

		    if((0==strncmp(auth_tx_detail_arc.product_codes[6].amount,"59",2)) || (strncmp(auth_tx_detail_arc.pos_entry_mode,"0081",4) == 0))
			{  	
				strncpy(pos_data,"010500007",9);	  
					/*ECI value for VISA - ECI value should be copied to 88-89 position*/
				strncpy(eci,"07 ",3);
				if(strcmp(auth_tx_detail_arc.profile,"")!=0)
				{
				 strncpy(eci,auth_tx_detail_arc.profile,2);
				 strncpy(&pos_data[7],auth_tx_detail_arc.profile,2);
				}
	 
			}
		     /* RPS */
		    else if(strncmp(auth_tx_detail_arc.product_codes[6].amount,"08",2) == 0) 
		   {
			   strncpy(pos_data,"0100000",7);			   
			   strncpy(pos_data+7,"08",2);
			   memset(&pos_data[9],0x52,1);
		   } 		 
				 

		   if(strncmp(auth_tx_detail_arc.product_codes[6].amount,"08",2) == 0) /* Rec Txn - R ,Non-Rec Txn - Space */
		   {
				memset(&pos_data[9],0x52,1);
		   }
		   else
                memset(&pos_data[9],0x20,1);

		   /* ATM - For checking pin exists or not */ 

		   if(strncmp(auth_tx_detail_arc.pos_entry_mode,"021",3) == 0)
               memset(&pos_data[2],0x31,1);
		   else
			   memset(&pos_data[2],0x32,1); 

		   iIndustry = atoi(terminal.industry_code );

			if (iIndustry == MAIL_PHONE) /* MOTO */
			  {
				  strncpy(pos_data+7,"08",2);
			  }
		/*	else
			{
				  strncpy(pos_data+7,"00",2);
			  } */

							
	 }

     else if((strncmp(auth_tx_detail_arc.card_nbr,"34",2)==0) || (strncmp(auth_tx_detail_arc.card_nbr,"37",2)==0)) /* AMEX POS Data */
	 {
        
		 /* We need only last 3 characters of POS entry mode
          * coming from terminal for processing further.
          */
          if((strncmp(auth_tx_detail_arc.processing_code,"0040",4) == 0) && (strncmp(auth_tx_detail_arc.message_type,"0220",4) == 0) )
		 {
		 	strncpy(auth_tx_detail_arc.processing_code,"0000",4); // Req from BDO for AMEX proc code
		 }

         if(strlen(auth_tx_detail_arc.pos_entry_mode) > 3)
            strncpy(temp_entry_mode,&auth_tx_detail_arc.pos_entry_mode[1],3);
         else
            strcpy(temp_entry_mode,auth_tx_detail_arc.pos_entry_mode);
		 

         if((strncmp(temp_entry_mode,"02",2) == 0) && (strncmp(temp_entry_mode,"021",3) != 0)) /* card swip without 4DBC */
         {                           
            strncpy(pos_data,"260101254140",EPOS_POS_DATA_SIZE);
         }

         else if(strncmp(temp_entry_mode,"06",2) == 0) /* card swip with 4DBC */
		 {
            strncpy(pos_data,"260101W54140",EPOS_POS_DATA_SIZE);
         }

		 
         else if((strncmp(temp_entry_mode,"01",2) == 0) && (strncmp(auth_tx_detail_arc.product_codes[6].amount,"08",2) != 0) && (strncmp(auth_tx_detail_arc.product_codes[6].amount,"59",2) != 0)) /* manual key in without 4DBC */
         {
            strncpy(pos_data,"660101654140",EPOS_POS_DATA_SIZE);
         }
		 else if((strncmp(temp_entry_mode,"07",2) == 0) && (strncmp(auth_tx_detail_arc.product_codes[6].amount,"08",2) != 0) && (strncmp(auth_tx_detail_arc.product_codes[6].amount,"59",2) != 0)) /* manual key in with 4DBC */ 
		 {
            strncpy(pos_data,"660101S54140",EPOS_POS_DATA_SIZE);
         }
		 
		 		 
		 else if((strncmp(temp_entry_mode,"012",3) == 0) && (strncmp(auth_tx_detail_arc.product_codes[6].amount,"08",2) == 0) &&
			      ((0 == strcmp(terminal.industry_code,"800")) )) /* RPS Transactions without 4DBC */ /* Changed the code for POS entry mode of position 5 from'2' to '9' */
				                                                                                      /* Girija Y TF, March 02, 2009 */ /* RPS Transactions without 4DBC */
         {
            strncpy(pos_data,"660390664130",EPOS_POS_DATA_SIZE);
		 }
         else if((strncmp(temp_entry_mode,"07",2) == 0) && (strncmp(auth_tx_detail_arc.product_codes[6].amount,"08",2) == 0) &&
			      ((0 == strcmp(terminal.industry_code,"800")) )) /* RPS Transactions without 4DBC */ /* Changed the code for POS entry mode of position 5 from'2' to '9' */
				                                                                                      /* Girija Y TF, March 02, 2009 */ /* RPS Transactions with 4DBC */
		 {
            strncpy(pos_data,"660390S64130",EPOS_POS_DATA_SIZE);
         }
		  	 

		 else if((strncmp(temp_entry_mode,"01",2) == 0) && (strncmp(auth_tx_detail_arc.product_codes[6].amount,"59",2) == 0)) /* Ecommerce without 4DBC */
         {
            strncpy(pos_data,"6609S06S4140",EPOS_POS_DATA_SIZE);
		 }
         else if((strncmp(temp_entry_mode,"07",2) == 0) && (strncmp(auth_tx_detail_arc.product_codes[6].amount,"59",2) == 0)) /* Ecommerce with 4DBC */
		 {
            strncpy(pos_data,"6609S0SS4140",EPOS_POS_DATA_SIZE);
         }

		 else if(strncmp(temp_entry_mode,"021",3) == 0) /* ATM */
		 {
			strncpy(pos_data,"210111210141",EPOS_POS_DATA_SIZE);
		 }	 

	}

	else  if(auth_tx_detail_arc.card_nbr[0]=='5') /* Master Card POS Data */
	 {		

		   
		   /* card swipe */

       if(strncmp(auth_tx_detail_arc.pos_entry_mode,"002",3) == 0)
       strncpy(pos_data,"902000000800",EPOS_POS_DATA_SIZE);

	   /* manual entry */

	   else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"001",3) == 0)
       strncpy(pos_data,"010000000700",EPOS_POS_DATA_SIZE);
         
	   /* CHIP */

       else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"005",3) == 0)
       strncpy(pos_data,"051000000800",EPOS_POS_DATA_SIZE);    	   
	   else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"007",3) == 0) /*Pay pass CHip Contactless*/
	   strncpy(pos_data,"072000000300",EPOS_POS_DATA_SIZE);
	   else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"009",3) == 0) /*MC Pay pass -Contactless Mag stripe*/
	   strncpy(pos_data,"091000000400",EPOS_POS_DATA_SIZE);

	   /* FALLBACK */
   	   
	   else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"008",3) == 0)
       strncpy(pos_data,"802000000800",EPOS_POS_DATA_SIZE);   
	   /* New POS entry mode added for MC */
	   else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"0800",4) == 0)
       strncpy(pos_data,"800000000800",EPOS_POS_DATA_SIZE);   

	   else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"0801",4) == 0)
       strncpy(pos_data,"801000000800",EPOS_POS_DATA_SIZE);   

	   else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"0802",4) == 0)
       strncpy(pos_data,"802000000800",EPOS_POS_DATA_SIZE);   

	   /* e-commerce  */
	   if((0==strncmp(auth_tx_detail_arc.product_codes[6].amount,"59",2)) || (0==strncmp(auth_tx_detail_arc.pos_entry_mode,"0081",4)))
	   {
	       strncpy(pos_data,"810125106000",EPOS_POS_DATA_SIZE); 	
	   } 
	   
		
	    /* RPS */

		 else if(strncmp(auth_tx_detail_arc.product_codes[6].amount,"08",2) == 0)
		   {
			   strncpy(pos_data,"010004100000",EPOS_POS_DATA_SIZE);
		   }
		 
		 /* ATM - For checking pin exists or not */ 

		   if(strncmp(auth_tx_detail_arc.pos_entry_mode,"021",3) == 0)
               memset(&pos_data[2],0x31,1);
		   else
			   memset(&pos_data[2],0x32,1);

		   if ((auth_tx_detail_arc.pos_entry_mode[1] == '9') && (strncmp(auth_tx_detail_arc.product_codes[6].amount,"00",2) == 0)) /* Magnetic stripe read capability */
			 {
				strncpy(pos_data,"91",2); // TF PHANI- contactless mag-stripe
			 }
           else if (terminal.device_type[1] == '7' ) /* Terminal has Chip Read capability. */
			 {
			     strncpy(pos_data,"07",2);
			 }		   
	   

	 } 

	
	/* Ecommerce Indicator Logic */

	 if((strncmp(auth_tx_detail_arc.card_nbr,"34",2)==0) || (strncmp(auth_tx_detail_arc.card_nbr,"37",2)==0) ) /* Space Filling for AMEX Cards ECOM Indicator */
	/* Removed VISA from this catageory- TF Phani*/
	 {
		 strncpy(eci," ",3);
	 }

	 else if(auth_tx_detail_arc.card_nbr[0]=='5') /* Master Card ECOM Indicator */
	
	 {
           if((0==strncmp(auth_tx_detail_arc.product_codes[6].amount,"59",2)) || (strncmp(auth_tx_detail_arc.pos_entry_mode,"0081",4) == 0)) /* If E-commerce txn */
		   {
			   if(strcmp(auth_tx_detail_arc.profile,"")!=0)
			   {
			     strncpy(eci,"21",2);
			     strncpy(eci+2,auth_tx_detail_arc.profile+1,1);
			   }
			   else strcpy(eci,"210");
		   }
		   else
		   {
			   strcpy(eci,"000");			   
		   }
     }

	 else if(strncmp(auth_tx_detail_arc.card_nbr,"35",2)==0) /* JCB Card ECOM Indicator */
	 {

	    if((0==strncmp(auth_tx_detail_arc.product_codes[6].amount,"59",2) || (0==strncmp(auth_tx_detail_arc.pos_entry_mode,"0081",4)))) /* If E-commerce txn */
		   {
			   if(strcmp(auth_tx_detail_arc.profile,"")!=0)
			   {
				 strcpy(eci,auth_tx_detail_arc.profile);
				 strncpy(eci+2," ",1);				 
			   }
			   else strncpy(eci,"07 ",3);

		   }
		   else
		   {
			  strncpy(eci,"   ",3);
			   
		   }
	 }


    /* Transaction ID */

	 if(auth_tx_detail_arc.card_nbr[0]=='5') /* Transaction ID for MASTER CARDS */
	
	 {    
		 // Settlement date MMDD
			strncpy(settle_date,auth_tx_detail_arc.product_codes[19].code,4);//MMDD
			/*If Settle date is NUll, copy tran date as settle date*/
			if(strlen(settle_date) == 0 || settle_date[0] ==  ' ' || settle_date[1] ==  ' ')
			memcpy(settle_date,&auth_tx_detail_arc.tran_date[4],4); 
			
			if(strlen(settle_date) == 0 || settle_date[0] ==  ' ' || settle_date[1] ==  ' ')
			memcpy(settle_date,CurrentDate+2,4);
			
		  if(strlen(auth_tx_detail_arc.mcard_banknet)>0)
		 {
			 if(strlen(auth_tx_detail_arc.mcard_banknet)>12)
			 {
			   strncpy(tran_id,auth_tx_detail_arc.mcard_banknet,12);
			   strcat(tran_id,"   ");
			   strncpy(&tran_id[9],settle_date,4);
			 }
			 else 
			 {
                 memset(tran_id,' ',12);  
				 memset(tran_id+12,' ',3); 
				 strncpy(tran_id,auth_tx_detail_arc.mcard_banknet,strlen(auth_tx_detail_arc.mcard_banknet));
			     strncpy(&tran_id[9],settle_date,4);
			 }
		 }
		 else
		 {
			 memset(tran_id,' ',15);
 			if(strlen(settle_date) == 0)
			memcpy(settle_date,CurrentDate+2,4);  

			 strncpy(&tran_id[9],settle_date,4);
		 } 
	 }

	 else if(auth_tx_detail_arc.card_nbr[0]=='4') /* Transaction ID for VISA */
	 {
		memset(tran_id,0x30,15);	
		strncpy(tran_id,auth_tx_detail_arc.mcard_banknet,15);
	 }

	 else if((strncmp(auth_tx_detail_arc.card_nbr,"34",2)==0) || (strncmp(auth_tx_detail_arc.card_nbr,"37",2)==0)) /* Transaction ID for AMEX */
	 {
		 strncpy(tran_id,auth_tx_detail_arc.mcard_banknet,15); 
	 }
	 else if((strncmp(auth_tx_detail_arc.card_nbr,"35",2)==0)) /* Transaction ID for JCB - Spaces */
	 {
		 memset(&tran_id,' ',15); 
	 }


	 /* PAN Sequence Number */

	/* if(strcmp(emv_record.tag,"5F34")==0)
	 {
		 strcpy(pan_sequence_number,emv_record.emv_data);
	 }
	 else */
		strcpy(pan_sequence_number,"000");

	 if((strncmp(auth_tx_detail_arc.card_nbr,"34",2)==0) || (strncmp(auth_tx_detail_arc.card_nbr,"37",2)==0)) /* For AMEX Card Sequence Number is Zero */
	 {
		 strcpy(pan_sequence_number,"000");
	 }

	  strcpy(tran_dt,auth_tx_detail_arc.tran_date + 2);

	  if(strncmp(auth_tx_detail_arc.product_codes[6].amount+2,"",3)==0)
	  {
		  strncpy(auth_tx_detail_arc.product_codes[6].amount+2,"000",3);
	  }	 
	 /* Form 27 byte message for DE 63- table Id 12, TF Phani- 
	  First 15 bytes in - BCh10 -> ticket_nbr 
	  Next  10 bytes in - BCh10 -> product_codes[14].amount*/
	/* TF-- Abhishek
	1) If Field 63 is present in the authorisation message and starts with "PRCHID", place the value of Field 63 in EPOS at segment 11 starting at
	    column 76 until 106.
	2) If Field 63 is present in the authorisation message and doesn't start with "PRCHID", place the values of Field 63 at segment 11 starting at column
	   82 until 106.
	Note :Item 1 is to address for RPS transactions. Item 2 is to address for PolicyNumber coming from POS terminal.
	*/
	if(strncmp(auth_tx_detail_arc.ticket_nbr,"PRCHID",CUSTOM_IDENTIFIER_TYPE_LENGTH)== 0)
	{
		// Print 31 byte starting from 76 until 106
		strncpy(perch_id,auth_tx_detail_arc.ticket_nbr,strlen(auth_tx_detail_arc.ticket_nbr));
		strncat(perch_id,auth_tx_detail_arc.product_codes[14].amount, (sizeof(perch_id)- strlen(auth_tx_detail_arc.ticket_nbr) - CUSTOM_IDENTIFIER_TYPE_LENGTH +1));
	    perch_id[PERCH_ID_LENGTH -1]='\0';
	}
	else
	{
		// 1st 6 byte should be space as feild 63 is not starts whith PRCHID 
		// copy data after 6 byte which will be store from 82nd byte location
		//Print 25 byte starting from 82 until 106
		strncpy(perch_id + CUSTOM_IDENTIFIER_TYPE_LENGTH ,auth_tx_detail_arc.ticket_nbr,strlen(auth_tx_detail_arc.ticket_nbr));
		strncat(perch_id,auth_tx_detail_arc.product_codes[14].amount, (sizeof(perch_id)- strlen(auth_tx_detail_arc.ticket_nbr)- CUSTOM_IDENTIFIER_TYPE_LENGTH +1 ));
		perch_id[PERCH_ID_LENGTH -1]='\0';
	}

	  memset(c_record, 0, sizeof(c_record));
	

	 /* writing details to file */

	 /* Segment 10 */

	
      sprintf(c_record, "%1s%4s%-15s%-019s%6s%6s%2s%6s%012s%06s%4s%-12s%04s%11s%12s%06s%03s%40s%012s%4s%29s%11s%03s%3s%2s%3s%3s%6s",
         "1",
		 auth_tx_detail_arc.message_type,
         term_id,         
         auth_tx_detail_arc.card_nbr,
         tran_dt,
         auth_tx_detail_arc.tran_time,
         "10",  /* Segment 10 Data Follows */
         auth_tx_detail_arc.processing_code,
         auth_tx_detail_arc.tran_amount,
		 "0",
		 terminal.category_code,
		 pos_data, 
		 "0",
		 " ",
		 auth_tx_detail_arc.primary_key.retrieval_ref_num,
		 auth_tx_detail_arc.auth_number,         
		 "0",
		 " ",
		 "0",
		 " ",
		 " ",
		 " ",
		 "0",
		 curr_code,
		 " ",
		 eci, /* E-Commerce Logic for MCRD,JCB ; VISA,AMEX - Spaces */
		 " ",
		 auth_tx_detail_arc.primary_key.batch_nbr		 
         );

      strncpy(c_record + 245, "\n\0", 2);

	  crec_count++;

	  if (write_to_credit_settle_file() == false)
      return (false);	  

	  cseg10_records++;

	  /* Segment 11 */

	  memset(c_record, 0, sizeof(c_record));
/* TF PHANI- New DE63 message adititon*/
	  sprintf(c_record, "%1s%4s%-15s%-019s%6s%6s%2s%4s%012s%3s%3s%-031s%4s%10s%1s%11s%15s%03s%8s%-15s%72s",
         "1",
		 auth_tx_detail_arc.message_type,
         term_id,         
         auth_tx_detail_arc.card_nbr,
         tran_dt,
         auth_tx_detail_arc.tran_time,         
		 "11", /* Segment 11 Data Follows */
         auth_tx_detail_arc.exp_date,
		 "0",
		 auth_tx_detail_arc.product_codes[6].amount+2,
	     pan_sequence_number,
	     perch_id,
		 " ",
         cad_merchant_id,
		 " ",
		 " ",
		 " ",
		 "0",
		 " ",
         tran_id, 
		 " "	 	          	 
         );

      strncpy(c_record + 245, "\n\0", 2);

	  crec_count++;	 

	  if (write_to_credit_settle_file() == false)
      return (false);


	  /* Segment 13 - EMV Data */

	     
     memset(emv_seg13, 0, sizeof(emv_seg13));
	 memset(emv_seg14, 0, sizeof(emv_seg14));
	 memset(emv_seg15, 0, sizeof(emv_seg15));
	     
     memset(c_record, 0, sizeof(c_record));

	 if((strncmp(auth_tx_detail_arc.card_nbr,"34",2)!=0) && (strncmp(auth_tx_detail_arc.card_nbr,"37",2)!=0) && ((strcmp(auth_tx_detail_arc.pos_entry_mode,"0051")==0) || (strcmp(auth_tx_detail_arc.pos_entry_mode,"0052")==0) || (strcmp(auth_tx_detail_arc.pos_entry_mode,"0072")==0))) /* EMV Data is not present in AMEX */
	 {	  

		max_etran = atoi(Emv_list.num_returned);
		
		if(max_etran==0) return false;

	    for (i = 0; i < max_etran; i++)
		{
			memset(emv_data, 0, sizeof(emv_data));

			memcpy(&emv_record,&Emv_list.batch_list[i],sizeof(emv_record));			
			
			elength= atoi(emv_record.DATA_LENGTH);

			elength = elength * 2;

			/* Parse the EMV raw data to wrtie it in  segments - Ajay-TF, 18-Jun-2009 */

			parseEmvDataToSegment(elength, emv_seg13, emv_seg14, emv_seg15);	 
	/*		bin_to_ascii(emv_record.RAW_DATA,emv_data,elength/2);	

				if(elength<192)
					{						
						memcpy(emv_seg13,emv_data,elength);												
						
					}

				else if((elength>192) && (elength<384))

					{					     
						 memcpy(emv_seg13,emv_data,192);
						 memcpy(emv_seg14,emv_data+192,192);
					}

				else
					{
				 		memcpy(emv_seg13,emv_data,192);
						memcpy(emv_seg14,emv_data+192,192);
						memcpy(emv_seg15,emv_data+384,192);
					}
			*/

		}	
		  memset(&auth_tx_detail.tran_time[6],'\0',1);
		 memset(&term_id[15],'\0',1);
         memset(c_record, 0x00, sizeof(c_record));

		 if((strlen(emv_seg13)!=0))
		 {

		 sprintf(c_record, "%1s%4s%-15s%-019s%6s%6s%2s%-192s",
			 "1",
			 auth_tx_detail_arc.message_type,
			 term_id,         
			 card_nbr,
			 tran_dt,
			 auth_tx_detail_arc.tran_time,         
			 "13",  /*Segment 13 Data Follows 	*/	
			 emv_seg13			 	         
			 );

		     strncpy(c_record + 245, "\n\0", 2);

		     crec_count++;

			 if((strlen(emv_seg14)==0)) return(true);
			 
			 if (write_to_credit_settle_file() == false)
             return (false);

		 }
		 memset(&auth_tx_detail_arc.tran_time[6],'\0',1);
		 memset(&term_id[15],'\0',1);
		  memset(c_record, 0, sizeof(c_record));

		  if((strlen(emv_seg14)!=0))
		 {

			 sprintf(c_record, "%1s%4s%-15s%-019s%6s%6s%2s%-192s",
			 "1",
			 auth_tx_detail_arc.message_type,
			 term_id,         
			 card_nbr,
			 tran_dt,
			 auth_tx_detail_arc.tran_time,         
			 "14", /* Segment 14 Data Follows */		
			 emv_seg14			 	         
			 );

		     strncpy(c_record + 245, "\n\0", 2);

		     crec_count++;	

			 if((strlen(emv_seg15)==0)) return(true);
			 
			 if (write_to_credit_settle_file() == false)
             return (false);
		 }
		 memset(&auth_tx_detail.tran_time[6],'\0',1);
		 memset(&term_id[15],'\0',1);
		  memset(c_record, 0x00, sizeof(c_record));
		 
		    if((strlen(emv_seg15)!=0))
		 {

			 sprintf(c_record, "%1s%04s%-15s%-019s%06s%06s%2s%-192s",
			 "1",
			 auth_tx_detail_arc.message_type,
			 term_id,         
			 card_nbr,
			 tran_dt,
			 auth_tx_detail_arc.tran_time,         
			 "15", /* Segment 15 Data Follows */		
			 emv_seg15			 	         
			 );

		     strncpy(c_record + 245, "\n\0", 2);

		     crec_count++;				   		 

		 }
	}
   
         return(true);

} /* create_credit_detail_arc */


/*******************************************************************************
*
*  FUNCTION:         create_debit_detail
*
*  DESCRIPTION:      This function creates detail structure for 
*                    the debit settlement file.
*
*  INPUTS:           None
*         
*  OUTPUTS:          pCHAR d_record -  detail structure 
*
*  RETURN VALUE:     True if record should be written to file, else false
 
*******************************************************************************/
INT create_debit_detail()
{

   INT   retval = true,max_etran,i,elength;
   BYTE  term_id[16]={0};
   CHAR  rrn[13] = "";
//   CHAR  emv_seg13[192],emv_seg14[192],emv_seg15[192];
   CHAR  descriptor_code[3] = "",cad_merchant_id[12]={0}; 
   CHAR  tran_dt[7],entry_mode[5] ={0};	
   BYTE  pos_data[12]="";
   BYTE  temp_string[2]="";
   INT   iIndustry=0;
   BYTE  eci[4]={0};
   BYTE  tran_id[16]={0},temp_id[2]="",settle_date[5]="",temp_date[2]="";
   BYTE	 temp_entry_mode[4] = {0},card_nbr[20]={0};   
   BYTE  returncode=0;   
   BYTE  pan_sequence_number[4]={0};  
   BYTE  perch_id[PERCH_ID_LENGTH]={0}; // ID to store De63 table id -12 of messages 27 charcters- TF Phani	
   memset(tran_dt,0,7);   

   memset(d_record, 0, sizeof(d_record));
   memset(term_id, 0x00, sizeof(term_id));
   memset(card_nbr, 0x00, sizeof(card_nbr));
   memset(cad_merchant_id, 0x00, sizeof(cad_merchant_id));
   memset(eci, 0x00, sizeof(eci));
   memset(tran_id, 0x00, sizeof(tran_id));
   memset(pos_data,0x00,sizeof(pos_data));
    // Fill 6 byte with spaces for handling RPS traction.
   memset(perch_id,0x20,CUSTOM_IDENTIFIER_TYPE_LENGTH);

	strcpy(terminal.industry_code,"800");
	strcpy(terminal.category_code,"4784");
	
   /* Message Type */
    if (auth_tx_detail.tx_key == AUTH_REFUND)
	   {
		  strcpy(auth_tx_detail.message_type,"0400"); /* For Reversal Transaction */
	   }
	else
	      strcpy(auth_tx_detail.message_type,"0000"); /* For Original Transaction */

	/* Terminal ID - Left Justified ; Space Filled */

		if((strncmp(auth_tx_detail.card_nbr,"34",2)==0) || (strncmp(auth_tx_detail.card_nbr,"37",2)==0)) /* AMEX Terminal Id - 15 chars + 1 Space Filled */
	 {
		memcpy(&term_id,auth_tx_detail.primary_key.device_id,15);
		
	}
	else                                           /* VISA,JCB,MCARD Terminal ID */
	{
		memcpy(&term_id,auth_tx_detail.primary_key.device_id,8);
		memset(term_id+8,' ',7);	
		
	}

	if((strncmp(auth_tx_detail.card_nbr,"34",2)==0) || (strncmp(auth_tx_detail.card_nbr,"37",2)==0)) /* AMEX Card Number - 15 numeric + 4 Space Filled */
	{
		memcpy(&card_nbr,auth_tx_detail.card_nbr,15);
		memset(&card_nbr[15],0x20,4);		
	}
	else                                          /* VISA,JCB,MC Card Number - 16 numeric + 3 Space Filled */
	{
		memcpy(&card_nbr,auth_tx_detail.card_nbr,16);
		memset(&card_nbr[16],0x20,3);
		
	}

	  if(auth_tx_detail.tx_key == AUTH_REVERSAL)  /* Processing Code */
		{
		  memcpy(&auth_tx_detail.processing_code, "20", 2 );
		}


	   // TF PHANI,Chandra - Fix fo processing code for Batch Upload and CA Txns

	  if((strcmp(auth_tx_detail.processing_code,"000001") == 0) || (auth_tx_detail.tx_key == AUTH_BATCH_UPLOAD) || (0 == strcmp("0320",auth_tx_detail_arc.message_type))) /* Batch Upload */
	  {
		  memcpy(&auth_tx_detail.processing_code, "000000", 6 ); 
	  }

	   if((auth_tx_detail.tx_key == AUTH_CASH_ADVANCE_AUTHORIZATION) || (auth_tx_detail.tx_key == AUTH_CASH_ADVANCE) || (strcmp(auth_tx_detail.processing_code,"010000") == 0)) /* Cash Advance */

	  {
		  memcpy(&auth_tx_detail.processing_code, "010000", 6 );
	  }
    

    memcpy(cad_merchant_id,auth_tx_detail.merchant_id+5, 10 ); /* Cadence Merchant ID */ 

	 /* POS Data Logic */
	 
	  if((strncmp(auth_tx_detail.card_nbr,"35",2)==0)) /* JCB Card POS Data */
	{
		  	   
		   
		   /* card swipe */

		   if(strncmp(auth_tx_detail.pos_entry_mode,"002",3) == 0)
               strncpy(pos_data,"0220000",7);

		   /* manual entry */

           else if(strncmp(auth_tx_detail.pos_entry_mode,"001",3) == 0)
               strncpy(pos_data,"0100000",7);
         
		   /* CHIP */

           else if(strncmp(auth_tx_detail.pos_entry_mode,"005",3) == 0)
               strncpy(pos_data,"0510000",7);

		   /* Fallback */

		   else if(strncmp(auth_tx_detail.pos_entry_mode,"008",3) == 0)
               strncpy(pos_data,"9700000",7);
		   /* New POS entry mode for BDO - TF Phani*/
			 else if(strncmp(auth_tx_detail.pos_entry_mode,"0800",4) == 0)
				strncpy(pos_data,"9700000",7);
			 else if(strncmp(auth_tx_detail.pos_entry_mode,"0801",4) == 0)
				strncpy(pos_data,"9700000",7);
			 else if(strncmp(auth_tx_detail.pos_entry_mode,"0802",4) == 0)
				strncpy(pos_data,"9700000",7);

		    /* E-Commerce */

		   if((0==strncmp(auth_tx_detail.product_codes[6].amount,"59",2)) || (strncmp(auth_tx_detail.pos_entry_mode,"0081",4) == 0))
               strncpy(pos_data,"8120000",7);		   


		  /* RPS  */

		    iIndustry = atoi(terminal.industry_code );

		   if((strncmp(auth_tx_detail.product_codes[6].amount,"08",2) == 0))
		   {
			   /*strncpy(pos_data,"0100800",7); TF PHANI - Do not copy this */
			   strncpy(pos_data+7,"11",2);			  
			   strncpy(pos_data+3,"08",2);			 
		   }


            else if (iIndustry == MAIL_PHONE)
			  {
				  strncpy(pos_data+7,"1",1);
				  strncpy(pos_data+3,"08",2);
				  strncpy(pos_data+8,"1",1);
			  } 
			  else
			  {
				  strncpy(pos_data+7,"2",1);				 
				  strncpy(pos_data+8,"2",1);
			  } 

			 if((strncmp(auth_tx_detail.product_codes[6].amount,"08",2) == 0) && (iIndustry != MAIL_PHONE)) /* For RPS and Non-MOTO */ 
			 {
				  strncpy(pos_data+7,"21",2);			  
			 }
		
			  
			  /* ATM - For checking pin exists or not */ 

		   if(strncmp(auth_tx_detail.pos_entry_mode,"021",3) == 0)
               memset(&pos_data[2],0x31,1);
		   else
			   memset(&pos_data[2],0x32,1);
           

		   
		   
	}

	  else if(auth_tx_detail.card_nbr[0]=='4') /* VISA POS Data */
	{
          	  
		  
		  /* card swipe */

           if(strncmp(auth_tx_detail.pos_entry_mode,"002",3) == 0)
               strncpy(pos_data,"900050000",9);

		   /* manual entry */

           else if(strncmp(auth_tx_detail.pos_entry_mode,"001",3) == 0)
               strncpy(pos_data,"012050000",9);

         
		   /* CHIP */

           else if(strncmp(auth_tx_detail.pos_entry_mode,"005",3) == 0)
               strncpy(pos_data,"050050000",9);
		   
		   /* FALLBACK */

		   else if(strncmp(auth_tx_detail.pos_entry_mode,"008",3) == 0)
               strncpy(pos_data,"900052000",9);
		   
				   /* New POS entry mode for BDO - TF Phani*/
		   else if(strncmp(auth_tx_detail.pos_entry_mode,"0800",4) == 0)
			  strncpy(pos_data,"950000000",9);
		   else if(strncmp(auth_tx_detail.pos_entry_mode,"0801",4) == 0)
			  strncpy(pos_data,"950000000",9);
		   else if(strncmp(auth_tx_detail.pos_entry_mode,"0802",4) == 0)
			  strncpy(pos_data,"950000000",9);  

		   /* E-Commerce */

		  if((0==strncmp(auth_tx_detail.product_codes[6].amount,"59",2)) || (strncmp(auth_tx_detail.pos_entry_mode,"0081",4) == 0))
		  {
				strncpy(pos_data,"010500007",9);	  
					/*ECI value for VISA - ECI value should be copied to 88-89 position*/
				strncpy(eci,"07 ",3);
				if(strcmp(auth_tx_detail.profile,"")!=0)
				{
				 strncpy(eci,auth_tx_detail.profile,2);
				 strncpy(&pos_data[7],auth_tx_detail.profile,2);
				}
	 	  }
		  /* RPS */
		  else if(strncmp(auth_tx_detail.product_codes[6].amount,"08",2) == 0) 
		  {
			   strncpy(pos_data,"0100000",7);			   
			   strncpy(pos_data+7,"08",2);
			   memset(&pos_data[9],0x52,1);
		   }   
				 

		   if(strncmp(auth_tx_detail.product_codes[6].amount,"08",2) == 0) /* Rec Txn - R ,Non-Rec Txn - Space */
		   {
				memset(&pos_data[9],0x52,1);
		   }
		   else
                memset(&pos_data[9],0x20,1);

		   /* ATM - For checking pin exists or not */ 

		   if(strncmp(auth_tx_detail.pos_entry_mode,"021",3) == 0)
               memset(&pos_data[2],0x31,1);
		   else
			   memset(&pos_data[2],0x32,1); 

		   iIndustry = atoi(terminal.industry_code );


		     if (iIndustry == MAIL_PHONE) /* MOTO */
			  {
				  strncpy(pos_data+7,"08",2);
			  }
		/* else
			 {
				  strncpy(pos_data+7,"00",2);
			  }*/
				 
	 }

     else if((strncmp(auth_tx_detail.card_nbr,"34",2)==0) || (strncmp(auth_tx_detail.card_nbr,"37",2)==0)) /* AMEX POS Data */
	 {

		 /* We need only last 3 characters of POS entry mode
          * coming from terminal for processing further.
          */

         if(strlen(auth_tx_detail.pos_entry_mode) > 3)
            strncpy(temp_entry_mode,&auth_tx_detail.pos_entry_mode[1],3);
         else
            strcpy(temp_entry_mode,auth_tx_detail.pos_entry_mode);
        	

         if((strncmp(temp_entry_mode,"02",2) == 0) && (strncmp(temp_entry_mode,"021",3) != 0)) /* card swip without 4DBC */
         {                           
            strncpy(pos_data,"260101254140",EPOS_POS_DATA_SIZE);
         }

         else if(strncmp(temp_entry_mode,"06",2) == 0) /* card swip with 4DBC */
		 {
            strncpy(pos_data,"260101W54140",EPOS_POS_DATA_SIZE);
         }

		 
         else if((strncmp(temp_entry_mode,"01",2) == 0) && (strncmp(auth_tx_detail.product_codes[6].amount,"08",2) != 0) && (strncmp(auth_tx_detail.product_codes[6].amount,"59",2) != 0)) /* manual key in without 4DBC */
         {
            strncpy(pos_data,"660101654140",EPOS_POS_DATA_SIZE);
         }
		 else if((strncmp(temp_entry_mode,"07",2) == 0) && (strncmp(auth_tx_detail.product_codes[6].amount,"08",2) != 0) && (strncmp(auth_tx_detail.product_codes[6].amount,"59",2) != 0)) /* manual key in with 4DBC */ 
		 {
            strncpy(pos_data,"660101S54140",EPOS_POS_DATA_SIZE);
         }
		 
		 		 
		 else if((strncmp(temp_entry_mode,"012",3) == 0) && (strncmp(auth_tx_detail.product_codes[6].amount,"08",2) == 0) &&
			      ((0 == strcmp(terminal.industry_code,"800")) )) /* RPS Transactions without 4DBC */ /* Changed the code for POS entry mode of position 5 from'2' to '9' */
				                                                                                      /* Girija Y TF, March 02, 2009 */ /* RPS Transactions without 4DBC */
         {
            strncpy(pos_data,"660390664130",EPOS_POS_DATA_SIZE);
		 }
         else if((strncmp(temp_entry_mode,"07",2) == 0) && (strncmp(auth_tx_detail.product_codes[6].amount,"08",2) == 0) &&
			      ((0 == strcmp(terminal.industry_code,"800")) )) /* RPS Transactions without 4DBC */ /* Changed the code for POS entry mode of position 5 from'2' to '9' */
				                                                                                      /* Girija Y TF, March 02, 2009 */  /* RPS Transactions with 4DBC */
		 {
            strncpy(pos_data,"660390S64130",EPOS_POS_DATA_SIZE);
         }

		  	 

		 else if((strncmp(temp_entry_mode,"01",2) == 0) && (strncmp(auth_tx_detail.product_codes[6].amount,"59",2) == 0)) /* Ecommerce without 4DBC */
         {
            strncpy(pos_data,"6609S06S4140",EPOS_POS_DATA_SIZE);
		 }
         else if((strncmp(temp_entry_mode,"07",2) == 0) && (strncmp(auth_tx_detail.product_codes[6].amount,"59",2) == 0)) /* Ecommerce with 4DBC */
		 {
            strncpy(pos_data,"6609S0SS4140",EPOS_POS_DATA_SIZE);
         }

		 else if(strncmp(temp_entry_mode,"021",3) == 0) /* ATM */
		 {
			strncpy(pos_data,"210111210141",EPOS_POS_DATA_SIZE);
		 }	 



	}

	else  if(auth_tx_detail.card_nbr[0]=='5') /* Master Card POS Data */
	 {	 		

		   
		   /* card swipe */

       if(strncmp(auth_tx_detail.pos_entry_mode,"002",3) == 0)
       strncpy(pos_data,"902000000800",EPOS_POS_DATA_SIZE);

	   /* manual entry */

	   else if(strncmp(auth_tx_detail.pos_entry_mode,"001",3) == 0)
       strncpy(pos_data,"010000000700",EPOS_POS_DATA_SIZE);
         
	   /* CHIP */

       else if(strncmp(auth_tx_detail.pos_entry_mode,"005",3) == 0)
       strncpy(pos_data,"051000000800",EPOS_POS_DATA_SIZE);
	   
	   /* FALLBACK */
   	   
	   else if(strncmp(auth_tx_detail.pos_entry_mode,"008",3) == 0)
       strncpy(pos_data,"802000000800",EPOS_POS_DATA_SIZE);   
	   /* New POS entry mode added for MC */
	   else if(strncmp(auth_tx_detail.pos_entry_mode,"0800",4) == 0)
       strncpy(pos_data,"800000000800",EPOS_POS_DATA_SIZE);   

	   else if(strncmp(auth_tx_detail.pos_entry_mode,"0801",4) == 0)
       strncpy(pos_data,"801000000800",EPOS_POS_DATA_SIZE);   

	   else if(strncmp(auth_tx_detail.pos_entry_mode,"0802",4) == 0)
       strncpy(pos_data,"802000000800",EPOS_POS_DATA_SIZE);   

	   /* e-commerce  */
	   if((0==strncmp(auth_tx_detail.product_codes[6].amount,"59",2)) || (0==strncmp(auth_tx_detail.pos_entry_mode,"0081",4)))
	   {
	       strncpy(pos_data,"810125106000",EPOS_POS_DATA_SIZE); 	
	   }      
		
	    /* RPS */

		 else if(strncmp(auth_tx_detail.product_codes[6].amount,"08",2) == 0)
		   {
			   strncpy(pos_data,"010004100000",EPOS_POS_DATA_SIZE);
		   }
		 
		 /* ATM - For checking pin exists or not */ 

		   if(strncmp(auth_tx_detail.pos_entry_mode,"021",3) == 0)
               memset(&pos_data[2],0x31,1);
		   else
			   memset(&pos_data[2],0x32,1);

		      if ((auth_tx_detail.pos_entry_mode[1] == '9') && (strncmp(auth_tx_detail.product_codes[6].amount,"00",2) == 0)) /* Magnetic stripe read capability */
			 {
				strncpy(pos_data,"91",2); // TF PHANI- contactless mag-stripe
			 }
           else if (terminal.device_type[1] == '7' ) /* Terminal has Chip Read capability. */
			 {
			     strncpy(pos_data,"07",2);
			 }
	   

	 } 
	  


	/* Ecommerce Indicator Logic */

	 if((strncmp(auth_tx_detail.card_nbr,"34",2)==0) || (strncmp(auth_tx_detail.card_nbr,"37",2)==0) ) /* Space Filling for AMEX Cards ECOM Indicator */
	/* Removed VISA from this catageory- TF Phani*/
	 {
		 strncpy(eci," ",3);
	 }

	 else if(auth_tx_detail.card_nbr[0]=='5') /* Master Card ECOM Indicator */
	
	 {
           if((0==strncmp(auth_tx_detail.product_codes[6].amount,"59",2)) || (strncmp(auth_tx_detail.pos_entry_mode,"0081",4) == 0)) /* If E-commerce txn */ /* If E-commerce txn */
		   {
			   if(strcmp(auth_tx_detail.profile,"")!=0)
			   {
			     strncpy(eci,"21",2);
			     strncpy(eci+2,auth_tx_detail.profile+1,1);
			   }
			   else strcpy(eci,"210");
		   }
		   else
		   {
			   strcpy(eci,"000");			   
		   }
     }

	 else if(strncmp(auth_tx_detail.card_nbr,"35",2)==0) /* JCB Card ECOM Indicator */
	 {

	    if((0==strncmp(auth_tx_detail.product_codes[6].amount,"59",2) || (0==strncmp(auth_tx_detail.pos_entry_mode,"0081",4)))) /* If E-commerce txn */
		   {
			   if(strcmp(auth_tx_detail.profile,"")!=0)
			   {
			     strcpy(eci,auth_tx_detail.profile);
				 strncpy(eci+2," ",1);
				 
			   }
			   else strncpy(eci,"07 ",3);

		   }
		   else
		   {
			  strncpy(eci,"   ",3);			   
		   }
	 }



    /* Transaction ID */

	 if(auth_tx_detail.card_nbr[0]=='5') /* Transaction ID for MASTER CARDS */
	
	 {    
		 // Settlement date MMDD
			strncpy(settle_date,auth_tx_detail.product_codes[19].code,4);//MMDD
			/*If Settle date is NUll, copy tran date as settle date*/
			if(strlen(settle_date) == 0)
			memcpy(settle_date,&auth_tx_detail.tran_date[4],4);  
			
		  if(strlen(auth_tx_detail.mcard_banknet)>0)
		 {
			 if(strlen(auth_tx_detail.mcard_banknet)>12)
			 {
			   strncpy(tran_id,auth_tx_detail.mcard_banknet,12);
			   strcat(tran_id,"   ");
			   strncpy(&tran_id[9],settle_date,4);
			 }
			 else 
			 {
                 memset(tran_id,' ',12);  
				 memset(tran_id+12,' ',3); 
				 strncpy(tran_id,auth_tx_detail.mcard_banknet,strlen(auth_tx_detail.mcard_banknet));
			   
				 strncpy(&tran_id[9],settle_date,4);
			   
			 }
		 }
		 else
		 {
			 memset(tran_id,' ',15);
			 //strcat(tran_id,"000");
			 
			 strncpy(&tran_id[9],settle_date,4);
		 } 
	 }


	 else if(auth_tx_detail.card_nbr[0]=='4') /* Transaction ID for VISA */
	 {
		 memset(tran_id,0x30,15);
		 memcpy(tran_id,auth_tx_detail.mcard_banknet,15);
	 }

	 else	if((strncmp(auth_tx_detail.card_nbr,"34",2)==0) || (strncmp(auth_tx_detail.card_nbr,"37",2)==0)) /* Transaction ID for AMEX */
	 {
		  strncpy(tran_id,auth_tx_detail.mcard_banknet,15);
	 }
	 else if((strncmp(auth_tx_detail.card_nbr,"35",2)==0)) /* Transaction ID for JCB - Spaces */
	 {
		 memset(tran_id,' ',15); 
	 }


	 /* PAN Sequence Number */

/*	 if(strcmp(emv_record.tag,"5F34")==0)
	 {
		 strcpy(pan_sequence_number,emv_record.emv_data);
	 }
	 else */
		strcpy(pan_sequence_number,"000");

	 if((strncmp(auth_tx_detail.card_nbr,"34",2)==0) || (strncmp(auth_tx_detail.card_nbr,"37",2)==0)) /* For AMEX Card Sequence Number is Zero */
	 {
		 strcpy(pan_sequence_number,"000");
	 }

	  strcpy(tran_dt,auth_tx_detail.tran_date + 2);

	  if(strncmp(auth_tx_detail.product_codes[6].amount+2,"",3)==0)
	  {
		  strncpy(auth_tx_detail.product_codes[6].amount+2,"000",3);
	  }
      /* Form 27 byte message for DE 63- table Id 12, TF Phani- 
	  First 15 bytes in - BCh10 -> ticket_nbr 
	  Next  10 bytes in - BCh10 -> product_codes[14].amount*/
	/* TF-- Abhishek
	1) If Field 63 is present in the authorisation message and starts with "PRCHID", place the value of Field 63 in EPOS at segment 11 starting at
	    column 76 until 106.
	2) If Field 63 is present in the authorisation message and doesn't start with "PRCHID", place the values of Field 63 at segment 11 starting at column
	   82 until 106.
	Note :Item 1 is to address for RPS transactions. Item 2 is to address for PolicyNumber coming from POS terminal.
	*/
	if(strncmp(auth_tx_detail.ticket_nbr,"PRCHID",CUSTOM_IDENTIFIER_TYPE_LENGTH)== 0)
	{
		// Print 31 byte starting from 76 until 106
		strncpy(perch_id,auth_tx_detail.ticket_nbr,strlen(auth_tx_detail.ticket_nbr));
		strncat(perch_id,auth_tx_detail.product_codes[14].amount, (sizeof(perch_id)- strlen(auth_tx_detail.ticket_nbr) - CUSTOM_IDENTIFIER_TYPE_LENGTH +1));
	    perch_id[PERCH_ID_LENGTH -1]='\0';
	}
	else
	{
		// 1st 6 byte should be space as feild 63 is not starts whith PRCHID 
		// copy data after 6 byte which will be store from 82nd byte location
		//Print 25 byte starting from 82 until 106
		strncpy(perch_id + CUSTOM_IDENTIFIER_TYPE_LENGTH ,auth_tx_detail.ticket_nbr,strlen(auth_tx_detail.ticket_nbr));
		strncat(perch_id,auth_tx_detail.product_codes[14].amount, (sizeof(perch_id)- strlen(auth_tx_detail.ticket_nbr)- CUSTOM_IDENTIFIER_TYPE_LENGTH +1 ));
		perch_id[PERCH_ID_LENGTH -1]='\0';
	}

	 /* writing details to file */

	 /* Segment 10 */

      sprintf(d_record, "%1s%4s%-15s%-019s%6s%6s%2s%6s%012s%06s%4s%-12s%04s%11s%12s%06s%03s%40s%012s%4s%29s%11s%03s%3s%2s%3s%3s%6s",
         "1",
		 auth_tx_detail.message_type,
         term_id,         
         card_nbr,
         tran_dt,
         auth_tx_detail.tran_time,
         "10",  /* Segment 10 Data Follows */
         auth_tx_detail.processing_code,
         auth_tx_detail.tran_amount,
		 "0",
		 terminal.category_code,
		 pos_data, 
		 "0",
		 " ",
		 auth_tx_detail.primary_key.retrieval_ref_num,
		 auth_tx_detail.auth_number,         
		 "0",
		 " ",
		 "0",
		 " ",
		 " ",
		 " ",
		 "0",
		 curr_code,
		 " ",
		 eci, /* E-Commerce Logic for MCRD,JCB ; VISA,AMEX - Spaces */
		 " ",
		 auth_tx_detail.primary_key.batch_nbr		 
         );

	  drec_count++;
   

      strncpy(d_record + 245, "\n\0", 2);

	 // if (write_to_debit_settle_file() == false)
      //return (false);	  

	  dseg10_records++;

	  /* Segment 11 */

	  memset(d_record, 0, sizeof(d_record));

	  sprintf(d_record, "%1s%4s%-15s%-019s%6s%6s%2s%4s%012s%3s%3s%-027s%8s%10s%1s%11s%15s%03s%8s%-15s%72s",
         "1",
		 auth_tx_detail.message_type,
         term_id,         
         card_nbr,
         tran_dt,
         auth_tx_detail.tran_time,         
		 "11", /* Segment 11 Data Follows */
         auth_tx_detail.exp_date,
		 "0",
		 auth_tx_detail.product_codes[6].amount+2,
	     pan_sequence_number,
		 perch_id,
		 " ",
         cad_merchant_id,
		 " ",
		 " ",
		 " ",
		 "0",
		 " ",
         tran_id, 
		 " "	 	          	 
         );

	  drec_count++;		 

      strncpy(d_record + 245, "\n\0", 2);

	  //if (write_to_debit_settle_file() == false)
      //return (false);


	 /* Segment 13 - EMV Data */

	     
     memset(emv_seg13, 0, sizeof(emv_seg13));
	 memset(emv_seg14, 0, sizeof(emv_seg14));
	 memset(emv_seg15, 0, sizeof(emv_seg15));
	     
     memset(d_record, 0, sizeof(c_record));

	 if((strncmp(auth_tx_detail.card_nbr,"34",2)!=0) && (strncmp(auth_tx_detail.card_nbr,"37",2)!=0) && ((strcmp(auth_tx_detail.pos_entry_mode,"0051")==0) || (strcmp(auth_tx_detail.pos_entry_mode,"0052")==0) || (strcmp(auth_tx_detail.pos_entry_mode,"0072")==0))) /* EMV Data is not present in AMEX */
	 {	  

		max_etran = atoi(Emv_list.num_returned);
		
		if(max_etran==0) return false;

	    for (i = 0; i < max_etran; i++)
		{
			memset(emv_data, 0, sizeof(emv_data));

			memcpy(&emv_record,&Emv_list.batch_list[i],sizeof(emv_record));			     
			
			elength=2*atoi(emv_record.DATA_LENGTH);					

			/* Parse the EMV raw data to wrtie it in  segments - Ajay-TF, 18-Jun-2009 */

			parseEmvDataToSegment(elength, emv_seg13, emv_seg14, emv_seg15);

		/*	bin_to_ascii(emv_data,emv_record.RAW_DATA,elength/2);				

				if(elength<192)
					{						
						memcpy(emv_seg13,emv_data,elength);												
					}

				else if((elength>192) && (elength<384))

					{					     
						 memcpy(emv_seg13,emv_data,192);
						 memcpy(emv_seg14,emv_data+192,192);
					}

				else
					{
				 		memcpy(emv_seg13,emv_data,192);
						memcpy(emv_seg14,emv_data+192,192);
						memcpy(emv_seg15,emv_data+384,192);
					}	
		*/

		}
		
         memset(d_record, 0, sizeof(d_record));

		 if((strlen(emv_seg13)!=0))
		 {

		 sprintf(d_record, "%1s%4s%-15s%-019s%6s%6s%2s%-192s",
			 "1",
			 auth_tx_detail.message_type,
			 term_id,         
			 card_nbr,
			 tran_dt,
			 auth_tx_detail.tran_time,         
			 "13", /* Segment 13 Data Follows */		
			 emv_seg13			 	         
			 );

		     strncpy(d_record + 245, "\n\0", 2);

		     drec_count++;

			 if((strlen(emv_seg14)==0)) return(true);
			 
			 //if (write_to_debit_settle_file() == false)
			 
             //return (false);

		 }

		  memset(d_record, 0, sizeof(d_record));

		  if((strlen(emv_seg14)!=0))
		 {

			 sprintf(d_record, "%1s%4s%-15s%-019s%6s%6s%2s%-192s",
			 "1",
			 auth_tx_detail.message_type,
			 term_id,         
			 card_nbr,
			 tran_dt,
			 auth_tx_detail.tran_time,         
			 "14", /* Segment 14 Data Follows */		
			 emv_seg14			 	         
			 );

		     strncpy(d_record + 245, "\n\0", 2);

		     drec_count++;	

			 if((strlen(emv_seg15)==0)) return(true);
			 
			 //if (write_to_debit_settle_file() == false)
             //return (false);
		 }

		  memset(d_record, 0, sizeof(d_record));
		 
		    if((strlen(emv_seg15)!=0))
		 {

			 sprintf(d_record, "%1s%4s%-15s%-019s%6s%6s%2s%-192s",
			 "1",
			 auth_tx_detail.message_type,
			 term_id,         
			 card_nbr,
			 tran_dt,
			 auth_tx_detail.tran_time,         
			 "15", /* Segment 15 Data Follows */		
			 emv_seg15			 	         
			 );

		     strncpy(d_record + 245, "\n\0", 2);

		     drec_count++;				   		 

		 }
	}
   
         return(true);
 
} /* create_debit_detail */


/*******************************************************************************
*
*  FUNCTION:         create_debit_detail_arc
*
*  DESCRIPTION:      This function creates detail structure for the debit
*                    settlement file from the batch archive file (BCH11).
*
*  INPUTS:           None
*         
*  OUTPUTS:          pCHAR d_record -  detail structure 
*
*  RETURN VALUE:     True if record should be written to file, else false

*******************************************************************************/
INT create_debit_detail_arc()
{
   INT   retval = true,max_etran,i,elength;
   BYTE  term_id[16]={0};
   CHAR  rrn[13] = "";
//   CHAR  emv_seg13[192],emv_seg14[192],emv_seg15[192];
   CHAR  descriptor_code[3] = "",cad_merchant_id[12]={0}; 
   CHAR  tran_dt[7],entry_mode[5] ={0};	
   BYTE  pos_data[12]="";
   BYTE  temp_string[2]="";
   INT   iIndustry=0;
   BYTE  eci[4]={0};
   BYTE  tran_id[16]={0},settle_date[5]={0};
   BYTE	 temp_entry_mode[4] = {0},card_nbr[20]={0};   
   BYTE  returncode=0;   
   BYTE  pan_sequence_number[4]={0};
   BYTE  perch_id[PERCH_ID_LENGTH]={0}; // ID to store De63 table id -12 of messages 27 charcters- TF Phani
   memset(tran_dt,0,7);   

   memset(d_record, 0, sizeof(d_record));
   memset(term_id, 0x00, sizeof(term_id));
   memset(card_nbr, 0x00, sizeof(card_nbr));
   memset(cad_merchant_id, 0x00, sizeof(cad_merchant_id));
   memset(eci, 0x00, sizeof(eci));
   memset(tran_id, 0x00, sizeof(tran_id));
    // Fill 6 byte with spaces for handling RPS traction.
   memset(perch_id,0x20,CUSTOM_IDENTIFIER_TYPE_LENGTH);

   /* Message Type */
    if (auth_tx_detail_arc.tx_key == AUTH_REFUND)
	   {
		  strcpy(auth_tx_detail_arc.message_type,"0400"); /* For Reversal Transaction */
	   }
	else
	      strcpy(auth_tx_detail_arc.message_type,"0000"); /* For Original Transaction */

	/* Terminal ID - Left Justified ; Space Filled */

		if((strncmp(auth_tx_detail_arc.card_nbr,"34",2)==0) || (strncmp(auth_tx_detail_arc.card_nbr,"37",2)==0)) /* AMEX Terminal Id - 15 chars + 1 Space Filled */
	 {
		memcpy(&term_id,auth_tx_detail_arc.primary_key.device_id,15);
		
	}
	else                                           /* VISA,JCB,MCARD Terminal ID */
	{
		memcpy(&term_id,auth_tx_detail_arc.primary_key.device_id,8);
		memset(term_id+8,' ',7);	
		
	}

		if((strncmp(auth_tx_detail_arc.card_nbr,"34",2)==0) || (strncmp(auth_tx_detail_arc.card_nbr,"37",2)==0)) /* AMEX Card Number - 15 numeric + 4 Space Filled */
	 {
		memcpy(&card_nbr,auth_tx_detail_arc.card_nbr,15);
		memset(&card_nbr[15],0x20,4);		
	}
	else                                          /* VISA,JCB,MC Card Number - 16 numeric + 3 Space Filled */
	{
		memcpy(&card_nbr,auth_tx_detail_arc.card_nbr,16);
		memset(&card_nbr[16],0x20,3);
		
	}

	  if(auth_tx_detail_arc.tx_key == AUTH_REVERSAL)  /* Processing Code */
		{
		  memcpy(&auth_tx_detail_arc.processing_code, "20", 2 );
		}


	  // TF PHANI,Chandra - Fix fo processing code for Batch Upload and CA Txns

	 if((0 == strcmp("0320",auth_tx_detail_arc.message_type)) || (auth_tx_detail_arc.processing_code[5] == '1'))  /* Batch Upload */
	  {
		  memcpy(&auth_tx_detail_arc.processing_code, "000000", 6 ); 
	  }

	 if((auth_tx_detail_arc.tx_key == AUTH_CASH_ADVANCE_AUTHORIZATION) || (auth_tx_detail_arc.tx_key == AUTH_CASH_ADVANCE) || (strcmp(auth_tx_detail_arc.processing_code,"010000") == 0)) /* Cash Advance */

	  {
		  memcpy(&auth_tx_detail_arc.processing_code, "010000", 6 );
	  }

  

    memcpy(cad_merchant_id,auth_tx_detail_arc.merchant_id+5, 10 ); /* Cadence Merchant ID */ 

	 /* POS Data Logic */
	 
	  if((strncmp(auth_tx_detail_arc.card_nbr,"35",2)==0)) /* JCB Card POS Data */
	{
		  	              
		   
		   /* card swipe */

		   if(strncmp(auth_tx_detail_arc.pos_entry_mode,"002",3) == 0)
               strncpy(pos_data,"0220000",7);

		   /* manual entry */

           else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"001",3) == 0)
               strncpy(pos_data,"0100000",7);
         
		   /* CHIP */

           else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"005",3) == 0)
               strncpy(pos_data,"0510000",7);

		   
		   /* Fallback */

		   else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"008",3) == 0)
               strncpy(pos_data,"9700000",7);
		   /* New POS entry mode for BDO - TF Phani*/
			 else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"0800",4) == 0)
				strncpy(pos_data,"9700000",7);
			 else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"0801",4) == 0)
				strncpy(pos_data,"9700000",7);
			 else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"0802",4) == 0)
				strncpy(pos_data,"9700000",7);

		    /* E-Commerce */

		    if(strncmp(auth_tx_detail_arc.pos_entry_mode,"0081",4) == 0)
               strncpy(pos_data,"8120000",7);


		  /* RPS  */

		    iIndustry = atoi(terminal.industry_code );

		   if((strncmp(auth_tx_detail_arc.product_codes[6].amount,"08",2) == 0))
		   {
			   /*strncpy(pos_data,"0100800",7); TF PHANI - Do not copy this */
			   strncpy(pos_data+7,"11",2);			  
			   strncpy(pos_data+3,"08",2);			 
		   }else if (iIndustry == MAIL_PHONE) /* MOTO */
		   {
			  strncpy(pos_data+7,"1",1);
			  strncpy(pos_data+3,"08",2);
			  strncpy(pos_data+8,"1",1);
		   }else
		   {
			  strncpy(pos_data+7,"2",1);				 
			  strncpy(pos_data+8,"2",1);
		   } 

			 if((strncmp(auth_tx_detail_arc.product_codes[6].amount,"08",2) == 0) && (iIndustry != MAIL_PHONE)) /* For RPS and Non-MOTO */ 
			 {
				  strncpy(pos_data+7,"21",2);			  
			 }
		
			  
			  /* ATM - For checking pin exists or not */ 

		   if(strncmp(auth_tx_detail_arc.pos_entry_mode,"021",3) == 0)
               memset(&pos_data[2],0x31,1);
		   else
			   memset(&pos_data[2],0x32,1);

		   
		   
	}

	  else if(auth_tx_detail_arc.card_nbr[0]=='4') /* VISA POS Data */
	{
          	  
		  		  
		  /* card swipe */

           if(strncmp(auth_tx_detail_arc.pos_entry_mode,"002",3) == 0)
               strncpy(pos_data,"900050000",9);

		   /* manual entry */

           else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"001",3) == 0)
               strncpy(pos_data,"012050000",9);

         
		   /* CHIP */

           else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"005",3) == 0)
               strncpy(pos_data,"050050000",9);
		   
		   /* FALLBACK */

		   else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"008",3) == 0)
               strncpy(pos_data,"900052000",9);
		     
					 /* New POS entry mode for BDO - TF Phani*/
			 else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"0800",4) == 0)
				strncpy(pos_data,"950000000",9);
			 else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"0801",4) == 0)
				strncpy(pos_data,"950000000",9);
			 else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"0802",4) == 0)
				strncpy(pos_data,"950000000",9) ; 

		   /* E-Commerce */

		   if((0==strncmp(auth_tx_detail_arc.product_codes[6].amount,"59",2)) || (strncmp(auth_tx_detail_arc.pos_entry_mode,"0081",4) == 0))
           {    
				strncpy(pos_data,"010500007",9);	  
					/*ECI value for VISA - ECI value should be copied to 89-90 position-TF Phani*/
				strncpy(eci,"07 ",3);
				if(strcmp(auth_tx_detail_arc.profile,"")!=0)
				{
				 strncpy(eci,auth_tx_detail_arc.profile,2);
				 strncpy(&pos_data[7],auth_tx_detail_arc.profile,2);
				}
	 		}
		   /* RPS */
		   else if(strncmp(auth_tx_detail_arc.product_codes[6].amount,"08",2) == 0) 
		   {
			   strncpy(pos_data,"0100000",7);			   
			   strncpy(pos_data+7,"08",2);
			   memset(&pos_data[9],0x52,1);
		   }		 
			  
				 

		   if(strncmp(auth_tx_detail_arc.product_codes[6].amount,"08",2) == 0) /* Rec Txn - R ,Non-Rec Txn - Space */
		   {
				memset(&pos_data[9],0x52,1);
		   }
		   else
                memset(&pos_data[9],0x20,1);

		   /* ATM - For checking pin exists or not */ 

		   if(strncmp(auth_tx_detail_arc.pos_entry_mode,"021",3) == 0)
               memset(&pos_data[2],0x31,1);
		   else
			   memset(&pos_data[2],0x32,1); 

		   iIndustry = atoi(terminal.industry_code );

			  if (iIndustry == MAIL_PHONE) /* MOTO */
			  {
				  strncpy(pos_data+7,"08",2);
			  }
			 /* else
			  {
				  strncpy(pos_data+7,"00",2);
			  }*/


	 }

     else if((strncmp(auth_tx_detail_arc.card_nbr,"34",2)==0) || (strncmp(auth_tx_detail_arc.card_nbr,"37",2)==0)) /* AMEX POS Data */
	 {
       
		 /* We need only last 3 characters of POS entry mode
          * coming from terminal for processing further.
          */

         if(strlen(auth_tx_detail_arc.pos_entry_mode) > 3)
            strncpy(temp_entry_mode,&auth_tx_detail_arc.pos_entry_mode[1],3);
         else
            strcpy(temp_entry_mode,auth_tx_detail_arc.pos_entry_mode);


        if((strncmp(temp_entry_mode,"02",2) == 0) && (strncmp(temp_entry_mode,"021",3) != 0)) /* card swip without 4DBC */
         {                           
            strncpy(pos_data,"260101254140",EPOS_POS_DATA_SIZE);
         }

         else if(strncmp(temp_entry_mode,"06",2) == 0) /* card swip with 4DBC */
		 {
            strncpy(pos_data,"260101W54140",EPOS_POS_DATA_SIZE);
         }

		 
         else if((strncmp(temp_entry_mode,"01",2) == 0) && (strncmp(auth_tx_detail_arc.product_codes[6].amount,"08",2) != 0) && (strncmp(auth_tx_detail_arc.product_codes[6].amount,"59",2) != 0)) /* manual key in without 4DBC */
         {
            strncpy(pos_data,"660101654140",EPOS_POS_DATA_SIZE);
         }
		 else if((strncmp(temp_entry_mode,"07",2) == 0) && (strncmp(auth_tx_detail_arc.product_codes[6].amount,"08",2) != 0) && (strncmp(auth_tx_detail_arc.product_codes[6].amount,"59",2) != 0)) /* manual key in with 4DBC */ 
		 {
            strncpy(pos_data,"660101S54140",EPOS_POS_DATA_SIZE);
         }
		 
		 		 
		 else if((strncmp(temp_entry_mode,"012",3) == 0) && (strncmp(auth_tx_detail_arc.product_codes[6].amount,"08",2) == 0) &&
			      ((0 == strcmp(terminal.industry_code,"800")) )) /* RPS Transactions without 4DBC */ /* Changed the code for POS entry mode of position 5 from'2' to '9' */
				                                                                                      /* Girija Y TF, March 02, 2009 */
         {
            strncpy(pos_data,"660390664130",EPOS_POS_DATA_SIZE);
		 }
         else if((strncmp(temp_entry_mode,"07",2) == 0) && (strncmp(auth_tx_detail_arc.product_codes[6].amount,"08",2) == 0) &&
			      ((0 == strcmp(terminal.industry_code,"800")) )) /* RPS Transactions with 4DBC */ /* Changed the code for POS entry mode of position 5 from'2' to '9' */
				                                                                                      /* Girija Y TF, March 02, 2009 */
		 {
            strncpy(pos_data,"660390S64130",EPOS_POS_DATA_SIZE);
         }
		  	 

		 else if((strncmp(temp_entry_mode,"01",2) == 0) && (strncmp(auth_tx_detail_arc.product_codes[6].amount,"59",2) == 0)) /* Ecommerce without 4DBC */
         {
            strncpy(pos_data,"6609S06S4140",EPOS_POS_DATA_SIZE);
		 }
         else if((strncmp(temp_entry_mode,"07",2) == 0) && (strncmp(auth_tx_detail_arc.product_codes[6].amount,"59",2) == 0)) /* Ecommerce with 4DBC */
		 {
            strncpy(pos_data,"6609S0SS4140",EPOS_POS_DATA_SIZE);
         }

		 else if(strncmp(temp_entry_mode,"021",3) == 0) /* ATM */
		 {
			strncpy(pos_data,"210111210141",EPOS_POS_DATA_SIZE);
		 }	 

	}

	else  if(auth_tx_detail_arc.card_nbr[0]=='5') /* Master Card POS Data */
	 {
	  					   
		   /* card swipe */

       if(strncmp(auth_tx_detail_arc.pos_entry_mode,"002",3) == 0)
       strncpy(pos_data,"902000000800",EPOS_POS_DATA_SIZE);

	   /* manual entry */

	   else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"001",3) == 0)
       strncpy(pos_data,"010000000700",EPOS_POS_DATA_SIZE);
         
	   /* CHIP */

       else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"005",3) == 0)
       strncpy(pos_data,"051000000800",EPOS_POS_DATA_SIZE); 
   	   
	   /* FALLBACK */
   	   
	   else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"008",3) == 0)
       strncpy(pos_data,"802000000800",EPOS_POS_DATA_SIZE);   
	   /* New POS entry mode added for MC */
	   else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"0800",4) == 0)
       strncpy(pos_data,"800000000800",EPOS_POS_DATA_SIZE);   

	   else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"0801",4) == 0)
       strncpy(pos_data,"801000000800",EPOS_POS_DATA_SIZE);   

	   else if(strncmp(auth_tx_detail_arc.pos_entry_mode,"0802",4) == 0)
       strncpy(pos_data,"802000000800",EPOS_POS_DATA_SIZE);   


	   /* e-commerce  */
	   if((0==strncmp(auth_tx_detail_arc.product_codes[6].amount,"59",2)) || (0==strncmp(auth_tx_detail_arc.pos_entry_mode,"0081",4)))
	   {
	       strncpy(pos_data,"810125106000",EPOS_POS_DATA_SIZE); 	
	   }      
		
	    /* RPS */

		 else if(strncmp(auth_tx_detail_arc.product_codes[6].amount,"08",2) == 0)
		   {
			   strncpy(pos_data,"010004100000",EPOS_POS_DATA_SIZE);
		   }
		 
		 /* ATM - For checking pin exists or not */ 

		   if(strncmp(auth_tx_detail_arc.pos_entry_mode,"021",3) == 0)
               memset(&pos_data[2],0x31,1);
		   else
			   memset(&pos_data[2],0x32,1);


          if ((auth_tx_detail_arc.pos_entry_mode[1] == '9') && (strncmp(auth_tx_detail_arc.product_codes[6].amount,"00",2) == 0)) /* Magnetic stripe read capability */
			 {
				strncpy(pos_data,"91",2);// TF PHANI- contactless mag-stripe
			 }
           else if (terminal.device_type[1] == '7' ) /* Terminal has Chip Read capability. */
			 {
			     strncpy(pos_data,"07",2);
			 }
	   

	 }     
	
	/* Ecommerce Indicator Logic */

	 if((strncmp(auth_tx_detail_arc.card_nbr,"34",2)==0) || (strncmp(auth_tx_detail_arc.card_nbr,"37",2)==0) ) /* Space Filling for AMEX Cards ECOM Indicator */
	/* Removed VISA from this catageory- TF Phani*/
	 {
		 strncpy(eci," ",3);
	 }

	 else if(auth_tx_detail_arc.card_nbr[0]=='5') /* Master Card ECOM Indicator */
	
	 {
           if((0==strncmp(auth_tx_detail_arc.product_codes[6].amount,"59",2) || (0==strncmp(auth_tx_detail_arc.pos_entry_mode,"0081",4)))) /* If E-commerce txn */
		   {
			   if(strcmp(auth_tx_detail_arc.profile,"")!=0)
			   {
			     strncpy(eci,"21",2);
			     strncpy(eci+2,auth_tx_detail_arc.profile+1,1);
			   }
			   else strcpy(eci,"210");
		   }
		   else
		   {
			   strcpy(eci,"000");			   
		   }
     }

	 else if(strncmp(auth_tx_detail_arc.card_nbr,"35",2)==0) /* JCB Card ECOM Indicator */
	 {

	    if((0==strncmp(auth_tx_detail_arc.product_codes[6].amount,"59",2))) /* If E-commerce txn */
		   {
			   if(strcmp(auth_tx_detail_arc.profile,"")!=0)
			   {
			     strcpy(eci,auth_tx_detail_arc.profile);
				 strncpy(eci+2," ",1);
				 								 
			   }
			   else strncpy(eci,"07 ",3);
		   }
		   else
		   {
			  strncpy(eci,"   ",3);			   
		   }
	 }

    /* Transaction ID */

	if(auth_tx_detail_arc.card_nbr[0]=='5') /* Transaction ID for MASTER CARDS */
	
	 {    
		// Settlement date MMDD
		   strncpy(settle_date,auth_tx_detail_arc.product_codes[19].code,4);//MMDD
		   /*If Settle date is NUll, copy tran date as settle date*/
		   if(strlen(settle_date) == 0)
		   memcpy(settle_date,&auth_tx_detail_arc.tran_date[4],4);	
		  if(strlen(auth_tx_detail_arc.mcard_banknet)>0)
		 {
			 if(strlen(auth_tx_detail_arc.mcard_banknet)>12)
			 {
			   strncpy(tran_id,auth_tx_detail_arc.mcard_banknet,12);
			  strcat(tran_id,"	 ");
			  strncpy(&tran_id[9],settle_date,4);
			 }
			 else 
			 {
                 memset(tran_id,' ',12);  
				memset(tran_id+12,' ',3); 
				 strncpy(tran_id,auth_tx_detail_arc.mcard_banknet,strlen(auth_tx_detail_arc.mcard_banknet));
				strncpy(&tran_id[9],settle_date,4);
			 }
		 }
		 else
		 {
			 memset(tran_id,' ',12);
			strncpy(&tran_id[9],settle_date,4);
		 } 
	 }

	 else if(auth_tx_detail_arc.card_nbr[0]=='4') /* Transaction ID for VISA */
	 {
		 memset(tran_id,0x30,15);
		 memcpy(tran_id,auth_tx_detail_arc.mcard_banknet,strlen(auth_tx_detail_arc.mcard_banknet));
	 }

	 else	if((strncmp(auth_tx_detail_arc.card_nbr,"34",2)==0) || (strncmp(auth_tx_detail_arc.card_nbr,"37",2)==0)) /* Transaction ID for AMEX */
	 {
		  strncpy(tran_id,auth_tx_detail_arc.mcard_banknet,15);
	 }
	 else if((strncmp(auth_tx_detail_arc.card_nbr,"35",2)==0)) /* Transaction ID for JCB - Spaces */
	 {
		 memset(tran_id,' ',15); 
	 }


	 /* PAN Sequence Number */

/*	 if(strcmp(emv_record.tag,"5F34")==0)
	 {
		 strcpy(pan_sequence_number,emv_record.emv_data);
	 }
	 else */
		strcpy(pan_sequence_number,"000");

	 if((strncmp(auth_tx_detail_arc.card_nbr,"34",2)==0) || (strncmp(auth_tx_detail_arc.card_nbr,"37",2)==0)) /* For AMEX Card Sequence Number is Zero */
	 {
		 strcpy(pan_sequence_number,"000");
	 }


	  strcpy(tran_dt,auth_tx_detail_arc.tran_date + 2);

	  if(strncmp(auth_tx_detail_arc.product_codes[6].amount+2,"",3)==0)
	  {
		  strncpy(auth_tx_detail_arc.product_codes[6].amount+2,"000",3);
	  }

		 /* Form 27 byte message for DE 63- table Id 12, TF Phani- 
	  First 15 bytes in - BCh10 -> ticket_nbr 
	  Next  10 bytes in - BCh10 -> product_codes[14].amount*/
	/* TF-- Abhishek
	1) If Field 63 is present in the authorisation message and starts with "PRCHID", place the value of Field 63 in EPOS at segment 11 starting at
	    column 76 until 106.
	2) If Field 63 is present in the authorisation message and doesn't start with "PRCHID", place the values of Field 63 at segment 11 starting at column
	   82 until 106.
	Note :Item 1 is to address for RPS transactions. Item 2 is to address for PolicyNumber coming from POS terminal.
	*/
	if(strncmp(auth_tx_detail.ticket_nbr,"PRCHID",CUSTOM_IDENTIFIER_TYPE_LENGTH)== 0)
	{
		// Print 31 byte starting from 76 until 106
		strncpy(perch_id,auth_tx_detail.ticket_nbr,strlen(auth_tx_detail.ticket_nbr));
		strncat(perch_id,auth_tx_detail.product_codes[14].amount, (sizeof(perch_id)- strlen(auth_tx_detail.ticket_nbr) - CUSTOM_IDENTIFIER_TYPE_LENGTH +1));
	    perch_id[PERCH_ID_LENGTH -1]='\0';
	}
	else
	{
		// 1st 6 byte should be space as feild 63 is not starts whith PRCHID 
		// copy data after 6 byte which will be store from 82nd byte location
		//Print 25 byte starting from 82 until 106
		strncpy(perch_id + CUSTOM_IDENTIFIER_TYPE_LENGTH ,auth_tx_detail.ticket_nbr,strlen(auth_tx_detail.ticket_nbr));
		strncat(perch_id,auth_tx_detail.product_codes[14].amount, (sizeof(perch_id)- strlen(auth_tx_detail.ticket_nbr)- CUSTOM_IDENTIFIER_TYPE_LENGTH +1 ));
		perch_id[PERCH_ID_LENGTH -1]='\0';
	}

	 /* writing details to file */

	 /* Segment 10 */


      sprintf(d_record, "%1s%4s%-15s%-019s%6s%6s%2s%6s%012s%06s%4s%-12s%04s%11s%12s%06s%03s%40s%012s%4s%29s%11s%03s%3s%2s%3s%3s%6s",
         "1",
		 auth_tx_detail_arc.message_type,
         term_id,         
         card_nbr,
         tran_dt,
         auth_tx_detail_arc.tran_time,
         "10",  /* Segment 10 Data Follows */
         auth_tx_detail_arc.processing_code,
         auth_tx_detail_arc.tran_amount,
		 "0",
		 terminal.category_code,
		 pos_data, 
		 "0",
		 " ",
		 auth_tx_detail_arc.primary_key.retrieval_ref_num,
		 auth_tx_detail_arc.auth_number,         
		 "0",
		 " ",
		 "0",
		 " ",
		 " ",
		 " ",
		 "0",
		 curr_code,
		 " ",
		 eci, /* E-Commerce Logic for MCRD,JCB ; VISA,AMEX - Spaces */
		 " ",
		 auth_tx_detail_arc.primary_key.batch_nbr		 
         );

      strncpy(d_record + 245, "\n\0", 2);

	  drec_count++;

	  //if (write_to_debit_settle_file() == false)
      //return (false);	  

	  dseg10_records++;

	  /* Segment 11 */

	  memset(d_record, 0, sizeof(d_record));

	  sprintf(d_record, "%1s%4s%-15s%-019s%6s%6s%2s%4s%012s%3s%3s%-027s%8s%10s%1s%11s%15s%03s%8s%15s%72s",
         "1",
		 auth_tx_detail_arc.message_type,
         term_id,         
         card_nbr,
         tran_dt,
         auth_tx_detail_arc.tran_time,         
		 "11", /* Segment 11 Data Follows */
         auth_tx_detail_arc.exp_date,
		 "0",
		 auth_tx_detail_arc.product_codes[6].amount,
	     pan_sequence_number,
		 perch_id,
		 " ",
         cad_merchant_id,
		 " ",
		 " ",
		 " ",
		 "0",
		 " ",
         tran_id, 
		 " "	 	          	 
         );

      strncpy(d_record + 245, "\n\0", 2);

	  drec_count++;
	 

	  //if (write_to_debit_settle_file() == false)
     // return (false);


      /* Segment 13 - EMV Data */

	     
     memset(emv_seg13, 0, sizeof(emv_seg13));
	 memset(emv_seg14, 0, sizeof(emv_seg14));
	 memset(emv_seg15, 0, sizeof(emv_seg15));
	     
     memset(d_record, 0, sizeof(d_record));

	 if((strncmp(auth_tx_detail_arc.card_nbr,"34",2)!=0) && (strncmp(auth_tx_detail_arc.card_nbr,"37",2)!=0) && ((strcmp(auth_tx_detail_arc.pos_entry_mode,"0051")==0) || (strcmp(auth_tx_detail_arc.pos_entry_mode,"0052")==0) || (strcmp(auth_tx_detail_arc.pos_entry_mode,"0072")==0))) /* EMV Data is not present in AMEX */
	 {	  

		max_etran = atoi(Emv_list.num_returned);
		
		if(max_etran==0) return false;

		for (i = 0; i < max_etran; i++)
		{
			memset(emv_data, 0, sizeof(emv_data));
			memcpy(&emv_record,&Emv_list.batch_list[i],sizeof(emv_record));			
			elength=2*atoi(emv_record.DATA_LENGTH);					

            /* Parse the EMV raw data to wrtie it in  segments - Ajay-TF, 08-Jun-2009 */
	       	parseEmvDataToSegment(elength, emv_seg13, emv_seg14, emv_seg15);

			/*	bin_to_ascii(emv_data,emv_record.RAW_DATA,elength/2);						

						if(elength<192)
							{						
								memcpy(emv_seg13,emv_data,elength);												
							}

						else if((elength>192) && (elength<384))

							{					     
								 memcpy(emv_seg13,emv_data,192);
								 memcpy(emv_seg14,emv_data+192,192);
							}

						else
							{
				 				memcpy(emv_seg13,emv_data,192);
								memcpy(emv_seg14,emv_data+192,192);
								memcpy(emv_seg15,emv_data+384,192);
							}	
					*/

				}	

	      memset(d_record, 0, sizeof(d_record));

		 if((strlen(emv_seg13)!=0))
		 {

		 sprintf(c_record, "%1s%4s%-15s%-019s%6s%6s%2s%-192s",
			 "1",
			 auth_tx_detail_arc.message_type,
			 term_id,         
			 card_nbr,
			 tran_dt,
			 auth_tx_detail_arc.tran_time,         
			 "13", /* Segment 13 Data Follows */		
			 emv_seg13			 	         
			 );

		     strncpy(d_record + 245, "\n\0", 2);

		     drec_count++;

			 if((strlen(emv_seg14)==0)) return(true);
			 
			 //if (write_to_debit_settle_file() == false)
             //return (false);

		 }

		  memset(d_record, 0, sizeof(d_record));

		  if((strlen(emv_seg14)!=0))
		 {

			 sprintf(d_record, "%1s%4s%-15s%-019s%6s%6s%2s%-192s",
			 "1",
			 auth_tx_detail_arc.message_type,
			 term_id,         
			 card_nbr,
			 tran_dt,
			 auth_tx_detail_arc.tran_time,         
			 "14", /* Segment 14 Data Follows */		
			 emv_seg14			 	         
			 );

		     strncpy(d_record + 245, "\n\0", 2);

		     drec_count++;	

			 if((strlen(emv_seg15)==0)) return(true);
			 
			 if (write_to_debit_settle_file() == false)
             return (false);
		 }

		  memset(d_record, 0, sizeof(d_record));
		 
		    if((strlen(emv_seg15)!=0))
		 {

			 sprintf(d_record, "%1s%4s%-15s%-019s%6s%6s%2s%-192s",
			 "1",
			 auth_tx_detail_arc.message_type,
			 term_id,         
			 card_nbr,
			 tran_dt,
			 auth_tx_detail_arc.tran_time,         
			 "15", /* Segment 15 Data Follows */		
			 emv_seg15			 	         
			 );

		     strncpy(d_record + 245, "\n\0", 2);

		     drec_count++;				   		 

		 }
	}	   
   
         return(true);

} /* create_debit_detail_arc */


/*******************************************************************************
*
*  FUNCTION:         create_headers
*
*  DESCRIPTION:      This function creates header structures for 
*                    the credit and debit settlement files.
*
*  INPUTS:           None
*         
*  OUTPUTS:          pCHAR c_record -  header structure 
*
*  RETURN VALUE:     None  
*   
*******************************************************************************/
void create_headers()
{
  
   /* ThoughtFocus - Chandra - Credit Header Record Implementation */

	char currentTime[7]=" ";

	returnCurrentTime(currentTime);

   if (((process_code[0] == 'A') || (process_code[0] == 'C')))
        
       
   {
      memset(c_record, 0, sizeof(c_record)); 
      sprintf(c_record, "%1s%6s%6s%06s%3s%06s%06s%011s%-11s%10s%10s%1d%1d%167s",
         "0",         
         extract_date,
		 CurrentDate,
		 currentTime,
		 "300",
		 "0",
		 "0",
		 "0",
		 "500051",
		 " ",
		 " ",
		 cre_inc_value,
		 cre_inc_value++,         
         " ");
      strncpy(c_record + 245, "\n\0", 2);
   }
   
   /* ThoughtFocus - Chandra - Debit Header Record Implementation */

  if (((process_code[0] == 'A') || (process_code[0] == 'D')))        
       
   {
      memset(d_record, 0, sizeof(d_record)); 
      sprintf(d_record, "%1s%6s%6s%06s%3s%06s%06s%011s%-11s%10s%10s%1d%1d%167s",
         "0",         
         extract_date,
		 CurrentDate,
		 currentTime,
		 "300",
		 "0",
		 "0",
		 "0",
		 "500051",
		 " ",
		 " ",
		 deb_inc_value,
		 deb_inc_value++,         
         " ");
      strncpy(d_record + 245, "\n\0", 2);
   }

} /* create_headers */
//
/*******************************************************************************
*
*  FUNCTION:         create_reports
*
*  DESCRIPTION:      This function creates reports for EPOS 
*                    
*
*  INPUTS:           None
*         
*  OUTPUTS:          pCHAR c_record -  trailer structure 
*
*  RETURN VALUE:     None  
*   
*******************************************************************************/
void create_reports()
{
     

   CHAR time_date[25] = "";

  
  strcpy(r_filename, path);
#ifndef WIN32
	  strcat(r_filename, "eftpos/");
#else
	  strcat(r_filename, "eftpos\\");
#endif
  strcat(r_filename, "epos_report_");
  strcat(r_filename, cutoff_date);
  strcat(r_filename,"_");
  strcat(r_filename,cutoff_hour);
  strcat(r_filename, ".rpt"); 
  
  r_file_write = fopen (r_filename, "w+");
   memset(r_record, 0, sizeof(r_record)); 
    ptetime_get_timestamp( time_date );	
	  sprintf(r_record,"TIME and DATE			  :	 %s\n",time_date);
	  fputs(r_record, r_file_write);
	  sprintf(r_record,"FILE NAME		       : 	epos_report_%s_%s.rpt \n",cutoff_date,cutoff_hour);
	  fputs(r_record, r_file_write);
	  sprintf(r_record,"No of Credit Transactions      : 	%09d \n",crec_count);
	  fputs(r_record, r_file_write);
	  sprintf(r_record,"No of Debit Transactions       : 	%09d \n",drec_count);
	  fputs(r_record, r_file_write);
#ifdef WIN32 
	  sprintf(r_record,"Total Credit Tx Amount		   :	  %016I64u \n",ctotal_amt);
#else
	  sprintf(r_record,"Total Credit Tx Amount         : 	%016llu \n",ctotal_amt);
#endif
	  fputs(r_record, r_file_write);
#ifdef WIN32 
	  sprintf(r_record,"Total Debit Tx Amount         : 	%016I64u \n",dtotal_amt);
#else
 
	  sprintf(r_record,"Total Debit Tx Amount         : 	%016llu \n",dtotal_amt);
#endif
	  fputs(r_record, r_file_write);

#ifdef WIN32 
	  sprintf(r_record,"Grand Total Amount	           : 	%016I64u \n",ctotal_amt+dtotal_amt);
#else
	  
	  sprintf(r_record,"Grand Total Amount	        : 	%016llu \n",ctotal_amt+dtotal_amt);
#endif
	  fputs(r_record, r_file_write);
	  sprintf(r_record,"Total Transactions  		: 	%09d \n",crec_count+drec_count);
	  fputs(r_record, r_file_write);
	  fclose(r_file_write);		  
   
} /* create_reprots */


//
/*******************************************************************************
*
*  FUNCTION:         create_trailers
*
*  DESCRIPTION:      This function creates trailer structures for 
*                    the credit and debit settlement files.
*
*  INPUTS:           None
*         
*  OUTPUTS:          pCHAR c_record -  trailer structure 
*
*  RETURN VALUE:     None  
*   
*******************************************************************************/
void create_trailers()
{
     
   /* ThoughtFocus - Chandra - Credit Trailer Record Implementation */
	
  if ( ((process_code[0] == 'A') || (process_code[0] == 'C')))
        
       
   {
      memset(c_record, 0, sizeof(c_record)); 
#ifdef WIN32      
	  sprintf(c_record, "%1s%09d%016I64u%1s%016s%016d%1s%015I64u%170s",
#else
	  sprintf(c_record, "%1s%09d%016llu%1s%016s%016d%1s%015llu%170s",
#endif
         "9",         
         crec_count,
		 ctotal_amt,
		 " ",
		 "0",
		 cseg10_records,
		 "+",
		 ctotal_amt,         
         " ");
	 
      strncpy(c_record + 245, "\n\0", 2);
   } 

   
   
   /* ThoughtFocus - Chandra - Debit Trailer Record Implementation */

   if ( ((process_code[0] == 'A') || (process_code[0] == 'D')))
       
        
   {
      memset(d_record, 0, sizeof(d_record)); 
#ifdef WIN32
      sprintf(d_record, "%1s%09d%016I64u%1s%016s%016d%1s%015I64u%170s",
#else
	  sprintf(d_record, "%1s%09d%016llu%1s%016s%016d%1s%015llu%170s",
#endif
         "9",         
         drec_count,
		 dtotal_amt,
		 " ",
		 "0",
		 dseg10_records,
		 "+",
		 dtotal_amt,         
         " ");
      strncpy(d_record + 245, "\n\0", 2);
   } 
   
} /* create_trailers */


/*******************************************************************************
*
*  FUNCTION:         create_settle_files
*
*  DESCRIPTION:      This function creates records and 
*                    writes to the settlement files. 
*
*  INPUTS:           None
*         
*  OUTPUTS:          None
*
*  RETURN VALUE:     BOOLEAN true for success or false for failure.*
  
*******************************************************************************/

 BOOLEAN create_settle_files()

 {
 
   CHAR              bonus_char[2] = "Y";
   CHAR              err_msg[100], buffer[200]; 
   INT               count_tran;
   INT               max_tran;
   INT               next_portion = 0;
   INT               ret_code = 0,i=0,ctemp_amt=0,dtemp_amt=0;
   
 
   /*** Set initial detail data for BCH11 ***/
   memset(&auth_tx_detail_arc, 0x00, sizeof(auth_tx_detail_arc));
  // strcpy (auth_tx_detail_arc.primary_key.batch_nbr, tc_totals.batch_number) ;
   strncpy (auth_tx_detail_arc.merchant_id, MID, 15) ;
  	strncpy (auth_tx_detail_arc.primary_key.retrieval_ref_num, "000000000000", 12) ;
   
   //strcpy (auth_tx_detail_arc.primary_key.retrieval_ref_num, " ") ;
   //strcpy (auth_tx_detail_arc.open_date, tc_totals.open_date);
//   strcpy (auth_tx_detail_arc.open_time, tc_totals.open_time);
   
   memset(&emv_record, 0, sizeof(emv_record));
   
   /*** ThoughtFocus - Chandra  ***/

   while (next_portion == 0)
   {
	   max_tran = 0;
	     if (db_get_bch11_service_list(&auth_tx_detail_arc, &bch11_list, err_msg) != PTEMSG_OK)
         {
            TxUtils_Send_Msg_To_Operator ( 0, err_msg, 1, ERROR_MSG, "create_settle_files",
               4, FATAL_ERROR, tc_totals.terminal_id, NULL_PTR, NULL_PTR);
          
			write_to_dump_file(err_msg);

            return (false);
         }

         max_tran = atoi(bch11_list.num_returned);
         if (max_tran < BATCH_SERVICE_LIST_SIZE) //if less than 30 then 1. Apinto
            next_portion = 1;
         /*** Loop for all transactions in the list ***/ 
         for (count_tran = 0; count_tran < max_tran; count_tran++)
         {
            memset(&auth_tx_detail_arc, 0x00, sizeof(auth_tx_detail_arc));
            memcpy(&auth_tx_detail_arc, &bch11_list.batch_list[count_tran], 
               sizeof(auth_tx_detail_arc));

  			      ctemp_amt=atoi(auth_tx_detail_arc.tran_amount); /* To Check whether amount is proper or not */

					   
				      ctotal_amt= ctotal_amt+ctemp_amt;

				
                  /*** Write to credit file ***/

                  if ( true == create_credit_detail_arc() )
                  {
                     if (write_to_credit_settle_file() == false)
                        return (false);
                  }
           
            

         } 

      }
     return (true);
	  
  }
   



/*****************************************************************************

  FUNCTION:       display_settle_info   

  DESCRIPTION:    This function displays an informational message
                  on the console.
  
  AUTHOR:         Darcy Person

  INPUTS:         None

  OUTPUTS:        None

  RETURN VALUE:   None

******************************************************************************/
void display_settle_info(void)
{
   printf("EFTPOS_RPS requires two command line parameters to work correctly:        \n");
   printf("(1) the base file name                                                    \n");
   printf("(2) date in YYYYMMDD format           ,                                   \n");
   printf("                                                                          \n");
   printf("This tool will create an EPOS file for given merchant IDs 			     \n");
   printf("(MAX 50 merchant at a time) in MID.txt file which should be present under \n");
   printf("ascendant/eftpos/ folder for a given open date only in BCH11.             \n");
   printf("                                                                       	 \n"); 
   printf("                                                                       	 \n"); 
   printf("          eftpos_rps no  20141022                                      	 \n");
   printf("                                                                       	 \n");   
   printf("Output is placed in the eftpos subdirectory under the configured          \n");
   printf("Ascendent system directory.                                            	 \n");
   printf("                                                                       	 \n");   
   printf("                                                                       	 \n");
   printf("Credit EFTPOS_RPS Version: %s\n\n", Version                         );
 
} /* display_settle_info */


/*******************************************************************************
*
*  FUNCTION:         EndProcess
*
*  DESCRIPTION:      This function does shutdown and clean up functionalities.
*
*  INPUTS:           None
*         
*  OUTPUTS:          None
*
*  RETURN VALUE:     BOOLEAN true for success or false for failure.
*
*  AUTHOR:           Irene Goldfild
*   
*******************************************************************************/
void EndProcess(void)
{
   CHAR Buffer[100] = "";

   if ( EndProcessFlag == true )
   {
      sprintf( Buffer, "Shutting down the %s Service, version %s",
               AppName, Version );
      LogEvent( Buffer, INFO_MSG );
      strcat( Buffer, "\n" );
      PRINT( Buffer );
   }
   else
   {
      EndProcessFlag = true;
   }
   return;

} /* EndProcess */


/*******************************************************************************
*
*  FUNCTION:         get_currency_code
*
*  DESCRIPTION:      This function gets currency code using dcf01 (terminal) 
*                    and mcf01 (merchant) tables.    
*
*  INPUTS:           None
*         
*  OUTPUTS:          DCF01 terminal, MCF01 merchant.
*
*  RETURN VALUE:     BOOLEAN true for success or false for failure.
*
*  AUTHOR:           Irene Goldfild
*   
*******************************************************************************/
BOOLEAN get_currency_code()
{
   BYTE    rc;
   CHAR    err_msg[100] = "";
   CHAR    buffer[500];
   BOOLEAN retval = true;
   double  start_time;
   double  end_time;
   double  duration;

   memset( &terminal, 0x00, sizeof(terminal) );
   memcpy( terminal.primary_key.device_id, tc_totals.terminal_id, 8 );

   /*** Get terminal configuration table ***/
   memset( device_type, 0x00, sizeof(device_type) );

   if ( DB_Timing_Flag == true )
      start_time = ptetime_get_time();

   rc = db_select_curr_code( curr_code,device_type,&terminal,err_msg );

   if ( DB_Timing_Flag == true )
   {
      end_time = ptetime_get_time();
      duration = end_time - start_time;
      update_timing_stats( ST1_DB_SELECT, ST2_NONE, MCF01_DATA,
                           (float)duration, &TimingStats );
   }

   if ( rc != PTEMSG_OK )
   {
      memset( buffer, 0x00,sizeof(buffer) );
      sprintf( buffer,
              "DB Err getting Currency Code from Merchant record: %s",
               err_msg );
      TxUtils_Send_Msg_To_Operator( 1, buffer, 1, WARN_MSG,
                                   "get_currency_code", 2, WARNING_ERROR,
                                    tc_totals.terminal_id,
                                    NULL_PTR,
                                    tc_totals.merchant_id );
      retval = false;
	  return( retval );
   }
   /*TF- Phani Check for INDUSRY CODE and category_code value. At this point Quiry is success*/
   if(strlen(terminal.industry_code) == 0)
   	{
		memset( buffer, 0x00,sizeof(buffer) );
		sprintf( buffer,
				"Industry Code is NULL for Merchant record: %s",
				 err_msg );
		TxUtils_Send_Msg_To_Operator( 1, buffer, 1, WARN_MSG,
									 "get_currency_code", 2, WARNING_ERROR,
									  tc_totals.terminal_id,
									  NULL_PTR,
									  tc_totals.merchant_id );
		retval = false;
		return( retval );
   	}
   return( retval );
}


/*******************************************************************************
*
*  FUNCTION:         open_credit_file
*
*  DESCRIPTION:      This function opens the credit settlement file 
*                    or creates it if it doesn't exist. 
*
*  INPUTS:           None
*         
*  OUTPUTS:          None
*
*  RETURN VALUE:     BOOLEAN true for success or false for failure.
*
*  AUTHOR:           Darcy Person
*   
*******************************************************************************/
BOOLEAN open_credit_file()
{   
   CHAR              buffer[500];

   memset(buffer, 0, sizeof(buffer));   

   /* Send message to system monitor */
   strcpy(buffer,"Open credit settlement file ");
   TxUtils_Send_Msg_To_Operator ( 0, buffer, 1, INFO_MSG, "open_credit_file",
      0, INFO_ERROR, NULL_PTR, NULL_PTR, NULL_PTR);

   /* Send message to DOS window in debug mode */
   strcat(buffer,"\n");
   PRINT(buffer);

   /*** Open file ***/
   strcpy(c_filename, path);
   #ifndef WIN32
	   strcat(c_filename, "eftpos/");
   #else
	   strcat(c_filename, "eftpos\\");
   #endif
   strcat(c_filename, "credit_eftpos");
   strcat(c_filename, cutoff_date);
   strcat(c_filename, ".upl"); 

   c_file_write = fopen (c_filename, "w");

   if (c_file_write == NULL_PTR)
   {  
      strcpy(buffer, "Cannot open credit settlement file: ");
      strcat(buffer, c_filename); 
        
      TxUtils_Send_Msg_To_Operator ( 0,   /* 1 - write to sys. mon., 0 - not */
         buffer,                          /* the text of the message */ 
		   1,                               /* 1 - write to event log, 0 - not */
         ERROR_MSG,                       /* message type */ 
		   "open_credit_settle_file",       /* name of the function */
         4,                               /* severity */
         FATAL_ERROR,                     /* message type */              
         NULL_PTR,                        /* TID */
		   NULL_PTR,                        /* card number */
         NULL_PTR);                       /* merchant id */

	  write_to_dump_file(buffer);

      return(false);
   }

   return (true);

} /* open_credit_file */


/*******************************************************************************
*
*  FUNCTION:         open_MID_file
*
*  DESCRIPTION:      This function opens the MID present under $ASCENDENTROOT/eftpos
*
*  INPUTS:           None
*         
*  OUTPUTS:          None
*
*  RETURN VALUE:     BOOLEAN true for success or false for failure.
*
*  AUTHOR:           Abhishek
*   
*******************************************************************************/
BOOLEAN open_MID_file()
{   
   CHAR              buffer[500];

   memset(buffer, 0, sizeof(buffer));

   /* Send message to system monitor */
   strcpy(buffer,"Open MID.txt file ");
   TxUtils_Send_Msg_To_Operator ( 0, buffer, 1, INFO_MSG, "open_MID_file",
      0, INFO_ERROR, NULL_PTR, NULL_PTR, NULL_PTR);

   /* Send message to DOS window in debug mode */
   strcat(buffer,"\n");
   PRINT(buffer);

   /*** Open file ***/  
   strcpy(d_filename, path);
   #ifndef WIN32
	   strcat(d_filename, "eftpos/");
   #else
	   strcat(d_filename, "eftpos\\");
   #endif
   strcat(d_filename, "MID.txt"); 
   //strcat(d_filename, cutoff_date);
   //strcat(d_filename, ".upl"); 
   MID_file_write = fopen (d_filename, "r");

   if (MID_file_write == NULL_PTR)
   {  
      strcpy(buffer, "Cannot open MID.txt file: ");
      strcat(buffer, d_filename); 
        
      TxUtils_Send_Msg_To_Operator ( 0,   /* 1 - write to sys. mon., 0 - not */
         buffer,                          /* the text of the message */ 
		   1,                               /* 1 - write to event log, 0 - not */
         ERROR_MSG,                       /* message type */ 
			"open_MID_file",        /* name of the function */
         4,                               /* severity */
         FATAL_ERROR,                     /* message type */              
         NULL_PTR,                        /* TID */
			NULL_PTR,                        /* card number */
         NULL_PTR);                       /* merchant id */
     	 
	  write_to_dump_file(buffer);
      
      return(false);
   }

   return (true);

} /* open_debit_file */


/*************************************************************************************
*
*  NAME:           returnCurrentTime
*
*  DESCRIPTION:    This function return the current time in HHMMSS frormat.
*
*  INPUTS:         None
*         
*  OUTPUTS:        NONE
*
*  RETURNS:        None
*
*  AUTHOR:         Farad 10-28-1999  
*   
*************************************************************************************/
void returnCurrentTime(char *currentTime)
{

   time_t tme;
   struct tm *tm1;
   CHAR timeString[20]={0};

   /*Get the time in seconds since 0 hrs from 1/1/70*/
   tme = time(NULL_PTR);
   tm1 = localtime(&tme);
  
   sprintf(timeString  ,"%2.2d",tm1->tm_hour);
   sprintf(timeString+2,"%2.2d",tm1->tm_min);
   sprintf(timeString+4,"%2.2d",tm1->tm_sec);

   strcpy(currentTime,timeString);
}


/*****************************************************************************
*
*  FUNCTION:         Rj_with_lead_zeros
*
*  DESCRIPTION:      This function will right justify a string and 
*                    add leading zeros
*
*  INPUTS:           str - the ascii string to be right justified 
*                    FieldWidth
*
*  OUTPUTS:          str - the right justified ascii string
*
*  RETURN VALUE:     None
*
*  AUTHOR:           Unknown
*
******************************************************************************/
void Rj_with_lead_zeros( pCHAR str, INT FieldWidth )
{
   INT               i;
   INT               dif;
   INT               strlength;

   strlength = strlen(str);

   /* Convert any leading blanks to zero. */
   for (i = 0; i < strlength; i++)
   {
      if ( 0x20 == str[i] )
         str [i] = 0x30;
      else
         break;
   }

   /* Remove any trailing blanks. */
   for (i = strlength-1; i >= 0; i--)
   {
      if ( 0x20 == str[i] )
         str [i] = 0x00;
      else
         break;
   }

   strlength = strlen(str);
   dif       = FieldWidth - strlength;

   if (dif > 0)
   {
      /* Right justify, then prepend leading zeros. */
      memmove (&str [dif], &str[0], strlength+1);
      memset  (&str[0], 0x30, dif);
   }

} /* Rj_with_lead_zeros */


/*************************************************************************************
*
*  FUNCTION:         set_date
*
*  DESCRIPTION:      This function uses the cutoff date and time 
*                    along with the number of hours to look back 
*                    to compute the date/time values needed for 
*                    the retrieval of information from the database 
*                    table bch01.
*
*  INPUTS:           None
*         
*  OUTPUTS:          None
*
*  RETURN VALUE:     None
*
*  AUTHOR:           Darcy Person   
*
*************************************************************************************/
void set_date()
{
   CHAR              year_yyyy[5];

   INT               day_m_int;
   INT               month_int;
	INT               offset_tmp = 0;
	INT               temp_div = 0;
	INT               temp_rem = 0;
	INT               temp_var = 0;
   INT               year_int;

   LONG              julian;

   time_t            tme;
   struct            tm *tm1;

   PRINT ("Set date\n");

   /* Initialize variables */
   memset(time_tag,        0, sizeof(time_tag));
   memset(yyyymmdd_start,  0, sizeof(yyyymmdd_start));
   memset(yyyymmdd_finish, 0, sizeof(yyyymmdd_finish));

   /* Get time tag */
   returnCurrentTime(time_tag);

   /* Calculate today's date as the finish date   */
   /* if the cutoff date uses the comma shorthand */
   if (strcmp(cutoff_date, ",") == 0)
   {
      tme = time(NULL_PTR);
      tm1 = localtime(&tme);

      /* Put today's date in YYYYMMDD format */
      month_int = tm1->tm_mon + 1;
      day_m_int = tm1->tm_mday;
      year_int  = tm1->tm_year;
      year_int  = year_int + 1900;

      memset(day_m,           0, sizeof(day_m));
      memset(month,           0, sizeof(month));
      memset(year_yyyy,       0, sizeof(year_yyyy));

      sprintf(month, "%02d", month_int);
      sprintf(day_m, "%02d", day_m_int);
      itoa(year_int, year_yyyy, 10);
      strncpy(yyyymmdd_finish, year_yyyy, 4);
      strncpy(yyyymmdd_finish + 4, month, 2);
      strncpy(yyyymmdd_finish + 6, day_m, 2);
      strcpy(cutoff_date, yyyymmdd_finish);
   }
   else
   {
      strncpy(month, cutoff_date + 4, 2);
      strncpy(day_m, cutoff_date + 6, 2);
      strcpy(yyyymmdd_finish, cutoff_date);
   }

   /* Set values for extraction date */
   strcpy(extract_date, yyyymmdd_finish + 2); //+4
   //strncpy(extract_date + 4, yyyymmdd_finish + 2, 2);

   /* Determine start date */
   julian = Txutils_Calculate_Julian_Date(yyyymmdd_finish);

   temp_rem = atoi(offset) % 24;
	temp_div = (atoi(offset) / 24 );
	

	julian = julian - temp_div;

	if( temp_rem > atoi(cutoff_hour) )
	{
		julian = julian - 1;
		offset_tmp = 24 - (temp_rem - atoi(cutoff_hour));
	}
	else 
      offset_tmp = atoi(cutoff_hour) - temp_rem;

	Txutils_Calculate_Gregorian_Date(julian, yyyymmdd_start);

   /* Determine start time */
	if( (offset_tmp / 10 ) == 0)
			sprintf(offset,"0%d",offset_tmp);
		else 
			sprintf(offset,"%d",offset_tmp);

} /* set_date */


/*******************************************************************************
*
*  FUNCTION:         write_to_credit_settle_file
*
*  DESCRIPTION:      This function writes to credit settlement file. 
*
*  INPUTS:           None
*         
*  OUTPUTS:          None
*
*  RETURN VALUE:     BOOLEAN true for success or false for failure.
*   
*******************************************************************************/
BOOLEAN write_to_credit_settle_file()
{
   CHAR              tempstr[16];
   CHAR              buffer[100];   

   /* Send message to system monitor */
 /*  strcpy(buffer,"Writing to credit settlement file ");
   TxUtils_Send_Msg_To_Operator ( 0, buffer, 1, INFO_MSG,
                                 "write_to_credit_settle_file",
                                  0, INFO_ERROR, NULL_PTR, NULL_PTR, NULL_PTR); */

   /* Send message to DOS window in debug mode */
  /* strcat(buffer,"\n");
   PRINT(buffer); */

   /*** Write to file ***/
   if (fputs(c_record,c_file_write) < 0)
   {
      itoa (ferror(c_file_write), tempstr, 10);
      fclose (c_file_write);
      
      strcpy(buffer, "Cannot write to credit settlement file: ");
      strcat(buffer, tempstr);
       
      TxUtils_Send_Msg_To_Operator ( 0,   /* 1 - write to sys. mon., 0 - not */
         buffer,                          /* the text of the message */ 
         1,                               /* 1 - write to event log, 0 - not */
         ERROR_MSG,                       /* message type */ 
			"write_to_credit_settle_file",   /* name of the function */
         4,                               /* severity */
         FATAL_ERROR,                     /* message type */              
         NULL_PTR,                        /* TID */
			NULL_PTR,                        /* card number */
         NULL_PTR);                       /* merchant id */

	  write_to_dump_file(buffer);

      return(false);
   }   

   return (true);

} /* write_to_credit_settle_file */


/*******************************************************************************
*
*  FUNCTION:         write_to_dump_file() 
*
*  DESCRIPTION:      This function writes an error record to the credit dump file. 
*					 The file will be created if it does not already exist.
*  INPUTS:           None
*         
*  OUTPUTS:          None
*
*  RETURN VALUE:     BOOLEAN true for success or false for failure.
*
**********************************************************************************/
void write_to_dump_file(CHAR* dump_record)
{
   CHAR              tempstr[16] = "";
   CHAR              buffer[500] = ""; 
   CHAR				 time_hhmmss[7] = "";
   CHAR				 time_str[10] = "";


   memset (&buffer, 0x00, sizeof(buffer));

   returnCurrentTime(time_hhmmss);
   sprintf(time_str, "%2.2s:%2.2s:%2.2s:\n", time_hhmmss, time_hhmmss+2, time_hhmmss+4);

   /* Check if file exists*/
   if (dumpfile_ptr == NULL_PTR)
   {
       /*** Open dump file ***/
	   memset (&dump_filename, 0x00, sizeof(dump_filename));
       strcpy(dump_filename, path);
       #ifndef WIN32
	       strcat(dump_filename, "eftpos/MID_");
       #else
	       strcat(dump_filename, "eftpos\\MID_");
       #endif

	   strcat(dump_filename, cutoff_date);
	   strcat(dump_filename, "_dump.txt");


	   /* Send message to system monitor */
       sprintf(buffer,"Opening dump file %s.", dump_filename);
       TxUtils_Send_Msg_To_Operator ( 0, buffer, 1, INFO_MSG, "write_to_dump_file",
                                      0, INFO_ERROR, NULL_PTR, NULL_PTR, NULL_PTR);

       /* Send message to DOS window in debug mode */
       strcat(buffer,"\n");
       PRINT(buffer);
	   
	   dumpfile_ptr = fopen (dump_filename, "w");

	   if (dumpfile_ptr == NULL_PTR)
	   {  
		  memset (&buffer, 0x00, sizeof(buffer));
		  sprintf(buffer, "Cannot open dump file %s.", dump_filename);
        
		  TxUtils_Send_Msg_To_Operator ( 0,   /* 1 - write to sys. mon., 0 - not */
			 buffer,                          /* the text of the message */ 
			   1,                             /* 1 - write to event log, 0 - not */
			 ERROR_MSG,                       /* message type */ 
			   "open_dump_file",		      /* name of the function */
			 4,                               /* severity */
			 WARNING_ERROR,                   /* message type */              
			 NULL_PTR,                        /* TID */
			 NULL_PTR,                        /* card number */
			 NULL_PTR);                       /* merchant id */

		  return;					      /*will try to continue settlement process*/
	   }
	   else
	   {
	   	   sprintf(buffer, "MID Dump File: %s\n", dump_filename); 
		   fputs (buffer, dumpfile_ptr);
	   }
   }

   /*else file exists or was opened successfully*/
   /*** Write to file ***/
   fputs(time_str, dumpfile_ptr);
   if (fputs (dump_record, dumpfile_ptr) < 0)
   {
      itoa (ferror(dumpfile_ptr), tempstr, 10);
      fclose (dumpfile_ptr);
      
      sprintf(buffer, "Cannot write to dump file %s: %s", dump_filename, tempstr);
       
      TxUtils_Send_Msg_To_Operator ( 0,   /* 1 - write to sys. mon., 0 - not */
         buffer,                          /* the text of the message */ 
         1,                               /* 1 - write to event log, 0 - not */
         ERROR_MSG,                       /* message type */ 
			"write_to_dump_file",         /* name of the function */
         4,                               /* severity */
         WARNING_ERROR,                   /* message type */              
         NULL_PTR,                        /* TID */
		 NULL_PTR,                        /* card number */
         NULL_PTR);                       /* merchant id */

   }
   return;

} /* write_to_dump_file */



/*******************************************************************************
*
*  FUNCTION:         write_to_debit_settle_file
*
*  DESCRIPTION:      This function writes to debit settlement file. 
*
*  INPUTS:           None
*         
*  OUTPUTS:          None
*
*  RETURN VALUE:     BOOLEAN: true for success or false for failure.
     
*************************************************************************************/
BOOLEAN write_to_debit_settle_file()
{
   CHAR              tempstr[16];
   CHAR              buffer[100];  

   /*** Write to file ***/
   if (fputs (d_record, MID_file_write) < 0)
   {
      itoa (ferror(MID_file_write), tempstr, 10);
      fclose (MID_file_write);
      
      strcpy(buffer, "Cannot write to debit settlement file: ");
      strcat(buffer, tempstr);
       
      TxUtils_Send_Msg_To_Operator ( 0,   /* 1 - write to sys. mon., 0 - not */
         buffer,                          /* the text of the message */ 
			1,                               /* 1 - write to event log, 0 - not */
         ERROR_MSG,                       /* message type */ 
			"write_to_debit_settle_file",    /* name of the function */
         4,                               /* severity */
         FATAL_ERROR,                     /* message type */              
         NULL_PTR,                        /* TID */
			NULL_PTR,                        /* card number */
         NULL_PTR);                       /* merchant id */

	  write_to_dump_file(buffer);

      return(false);
   }
   return (true);

} /* write_to_debit_settle_file */

 
/*******************************************************************************
*
*  FUNCTION:         bch01_handler
*
*  DESCRIPTION:      This function processes each batch to the settlement files. 
*
*  INPUTS:           None
*         
*  OUTPUTS:          None
*
*  RETURN VALUE:     BOOLEAN : true successful, false unsuccessful
*
*             
*   
*******************************************************************************/
BOOLEAN bch01_handler()
{
   CHAR     err_msg[100];
   CHAR     buffer[500];
   BOOLEAN  ret_val = true;
   int cre_amt=0,deb_amt=0;

   memset( &err_msg,   0x00, sizeof(err_msg)  );
   memset( &buffer,    0x00, sizeof(buffer)   );
   memset( &tc_totals, 0x00, sizeof(tc_totals));

   
   /*** Set batch totals ***/
  /* strcpy(tc_totals.batch_number, auth_tx_total.primary_key.batch_nbr);
   strcpy(tc_totals.terminal_id, auth_tx_total.primary_key.device_id);
   strcpy(tc_totals.merchant_id, auth_tx_total.merchant_id);
   strcpy(tc_totals.open_date, auth_tx_total.open_date);
   strcpy(tc_totals.open_time, auth_tx_total.open_time);
   itoa(atoi(auth_tx_total.total_credit_cnt) + 
        atoi(auth_tx_total.total_credit_refund_cnt), 
		tc_totals.number_credit_tran_for_batch, 9);
   itoa(atoi(auth_tx_total.total_debit_cnt) + 
        atoi(auth_tx_total.total_debit_refund_cnt), 
        tc_totals.number_debit_tran_for_batch, 9);
   itoa(atoi(auth_tx_total.total_credit_amt) + 
        atoi(auth_tx_total.total_credit_refund_amt), 
        tc_totals.totals_credit_for_batch, 10);
   itoa(atoi(auth_tx_total.total_debit_amt) + 
        atoi(auth_tx_total.total_debit_refund_amt), 
         tc_totals.totals_debit_for_batch, 10);   
   */


   /* Set flags to show if there are credit and/or debit txns. */
  /* if ( atoi(tc_totals.number_credit_tran_for_batch) > 0 )
      credit_txns_exist = true;
   else
      credit_txns_exist = false;

   if ( atoi(tc_totals.number_debit_tran_for_batch) > 0 )
      debit_txns_exist = true;
   else
      debit_txns_exist = false;

*/

   if (create_settle_files() == false)
      ret_val = false; 
	
  return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         GET_DATE
 *
 *  DESCRIPTION:  This procedure takes an input string that contains a date and
 *                time.  It copies the date portion into an output string,
 *                formatted slightly different.
 *
 *  INPUTS:       time_date : Format = "YYYY-0M-0D-0H.0I.0S.JJJ"
 *
 *  OUTPUTS:      date_str  : Format = "YYYY0M0D"
 *
 *  RTRN VALUE:   None
 *
 *********************************************************************************/
void get_date( pCHAR time_date, pCHAR date_str )
{
   memset ( date_str,  0x00,         9 );
   strncpy( date_str,  time_date,    4 );
   strncat( date_str, &time_date[5], 2 );
   strncat( date_str, &time_date[8], 2 );
}
   

/****************************************************************************************/

