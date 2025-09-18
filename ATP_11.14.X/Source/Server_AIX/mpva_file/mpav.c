
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
#include "app_info.h"
#include "equitdb.h"
#include "dc_dbstruct.h"
#include "tx_dbstruct.h"
#include "dc_database.h"
#include "nc_dbstruct.h"
#include "memmnger.h"
#include "dstimer.h"
#include "txutils.h"
#include "genutil.h"
#include "timings.h"

TLF01   tlf01;

CHAR    app_name[256];

static FILE 		   	 *E_file_write = 0;
static FILE          	 *dumpfile_ptr = 0;	         /* dump file ptr */

static CHAR 			 E_filename[256] = "";      /* Error file name*/
static CHAR              dump_filename[256] = "";    /* dump file name */
static CHAR              record_preauth[256];
static CHAR         	 path[30] = "";

static CHAR  mpav_Erro_warning_Filename[256] = {0};
static CHAR  mpav_module_error_warning_file_name[256] = {0};
static CHAR  mpav_error_warning_file_path[256] = {0};

static CHAR  			 CurrentDate[9];
CHAR          Version[] = "ATP_11.14.3";

#define MODE_INSERT "INSERT"
#define MODE_UPDATE "UPDATE"
#define   MAX_APPNAME_SIZE   8
#define   MAX_FILENAME_SIZE  80
#define   MAX_INI_INFO_SIZE  20

#define NUM_SIZE_WRITES  1

#define sON  "1"
#define sOFF "0"
#define sDEFAULT_DS_ERROR_STATICS_VALUE  "0.5"

/* TF INI File Name */
#define  TF_INI_FILE   "tf.ini"
#define RESPONSE_CODE_LEN  2

#define EXPIRED_PREAUTH_DECLINED_RC					"12"
#define INSUFFICIENT_PREAUTH_DECLINED_RC			"12"
#define ORIGINAL_TXN_NOT_FOUND_FOR_VOID_PREAUTH_RC 	"12"
#define ISO_RESPONSE_CODE_LEN       				2

CHAR temp_response_code[3]={0};

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

INT DB_NullCheck_Flag;

/* For DB statics added by raghu */
BYTE DB_MsgSubtype2 ;
BYTE DB_MsgSubtype1 ;
CHAR DB_Logging_Filename[256];
CHAR  DB_module_file_name[256];
CHAR  DB_file_path[256];
INT DB_NullCheck_Flag;

UINT Max_File_Size_Defined = 0 ;

/* End   */
/*This variable will get the value from the tf.ini
 * from the section DATASERVER with key name DB_ERROR_STATICS_FLAG
 * */
INT db_error_statics_flag;
double db_error_statics_value;

INT get_dataserver_ini_db_error_statics_value(pINT  ds_timer_flag,
                                   double *  ds_timer_value,
                                   pCHAR appname, pCHAR errbuf );
void Create_Db_Statics_And_Db_Oracle_Error_Filename( );
void Log_Db_Statics_And_Oracle_Db_Error_To_File(pCHAR Error_Buf);

void DisplayVersions()
{

	printf("                                                                       \n");
	printf("                                                                       \n");
	printf("Merchant Preauth Validation tool requires 2 command line parameters to work correctly: \n");
	printf("                                                                       \n");
	printf("                                                                       \n");
	printf("(1) the  date in YYYYMMDD format                               		   \n");
	printf("(2) a flag , 0 indicate from TLF01 table and 1 to pull records from archive");
	printf("                                                                       \n");
	printf("                                                                       \n");
	printf("offline_tool Version: %s\n\n", Version                         			);

}

INT isnum(pCHAR string)
{
   INT i;
   INT length;

   length = (INT)strlen(string);
   for (i = 0; i < length; i++)
   {
      if (!isdigit(string[i]))
        return(false);
   }
   return(true);
}

void Strrev( char * string)
{
	int len = 0;
	int i = 0;
	int j =0 ;
    BYTE  temp_str[20]="";
	while (string[i++]!='\0')
	{
		len++;
	}

	 for(i=len-1 ;i>=0;i--)
	 {
	   temp_str[j]=string[i];
	   j++;
	 }
	  temp_str[j]='\0';
	  strcpy(string,temp_str);
}

/******************************************************************************
 *
 *  NAME:         VALIDATE_DATE_YYYYMMDD
 *
 *  DESCRIPTION:  This function verifies a date to be valid.  The date must
 *                be in format YYYYMMDD.  The following checks are performed:
 *                   1.  Must be numeric
 *                   2.  Year must be in the range of 2000 to 2200
 *                   3.  Month must be from 1 to 12
 *                   4.  Day cannot be more than the maximum for the month,
 *                       leap year is taken into account for February.
 *
 *  INPUTS:       date_str - Date in YYYYMMDD format
 *
 *  OUTPUTS:      err_buf - Explanation of what is wrong with the date
 *
 *  RTRN VALUE:   True if valid date, false if invalid date
 *
 *  AUTHOR:       PHani
 *
 ******************************************************************************/
INT  validate_date_yyyymmdd( pBYTE date_str, pCHAR err_buf )
{
   INT  ret_val = false;
   INT  imonth, iday, iyear;
   CHAR temp_month[3]="";
   CHAR temp_day[3]  ="";
   CHAR temp_year[5] ="";

   /* Validate date is numeric. */
   if ( (true == isnum(date_str)) && (8 == strlen(date_str)) )
   {
      /* Validate month */
      memcpy( temp_month, date_str+4, 2 );
      imonth = atoi(temp_month);
      if ( (1 <= imonth) && (imonth <= 12) )
      {
         /* Month is valid; check the day. */
         memcpy( temp_day, date_str+6, 2 );
         iday = atoi(temp_day);
         if ( (1 <= iday) && (iday <= 31) )
         {
            if ( iday == 31 )
            {
               if ( (imonth == 1) || (imonth == 3) || (imonth == 5) ||
                    (imonth == 7) || (imonth == 8) || (imonth == 10)||
                    (imonth == 12) )
               {
                  ret_val = true;
               }
            }
            else if ( (iday == 30) && (imonth != 2) )
            {
               ret_val = true;
            }
            else if ( (iday == 29) && (imonth == 2) )
            {
               /* This must be a leap year to be valid. */
               memcpy( temp_year, date_str, 4 );
               iyear = atoi(temp_year);
               if ( (iyear%4) == 0 )
                  ret_val = true;
               else
                  sprintf( err_buf, "Too many days for February (%s)",date_str);
            }
            else
               ret_val = true;

            /* Validate the year. */
            memcpy( temp_year, date_str, 4 );
            iyear = atoi(temp_year);
            if ( (iyear < 2000) || (iyear > 2200) )
            {
               ret_val = false;
               sprintf( err_buf, "ERROR: Invalid date (%s). Bad Year",date_str);
            }
         }
         else
         {
            sprintf( err_buf, "ERROR: Invalid date (%s). Bad Day.", date_str );
         }
      }
      else
      {
         sprintf( err_buf, "ERROR: Invalid date (%s). Bad Month.", date_str );
      }
   }
   else
   {
      sprintf( err_buf, "ERROR: Invalid date (%s).", date_str );
   }
   if ( true != ret_val )
      strcat(  err_buf, " Must be in format YYYYMMDD." );

   return( ret_val );
}

void mpav_create_Error_Filename(  )
{
	CHAR  system_date[25] = {0};

   /* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
	ptetime_get_timestamp( system_date );

    strcpy( mpav_Erro_warning_Filename, mpav_error_warning_file_path );
    strcat( mpav_Erro_warning_Filename, mpav_module_error_warning_file_name );
	strcat( mpav_Erro_warning_Filename, ".EW." );
    strncat(mpav_Erro_warning_Filename, system_date,   4 );  /* YYYY */
    strncat(mpav_Erro_warning_Filename, system_date+5, 2 );  /* MM   */
    strncat(mpav_Erro_warning_Filename, system_date+8, 2 );  /* DD   */
}
/*******************************************************
 * This function will get the values from tf.ini file
 * from the section DATASERVER with keyname DB_ERROR_STATICS_FLAG &
 *	DB_ERROR_STATICS_VALUE which will help to decide the db statics
 *	logging mechanism.
 * *******************************************************/

void mpav_get_error_warning_file_name_path(void )
{
   DWORD rc;
   CHAR  filename   [80] = {0};
   CHAR  tmpstr     [80] = {0};
   CHAR  appname[512] = {0};
   CHAR  mpav_error_warning_file_size[5] = {0};
   /* Get path to the tf.ini file. */
   memset( tmpstr, 0x00, sizeof(tmpstr) );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, "tf.ini" );
   GetAppName (appname) ;
   /*Read path for creating file in order to log db statics and db oracle error messages */
   rc = GetPrivateProfileString(
								  "DATASERVERS",             /* points to section name         */
								  "DB_STATICS_PATH",       	/* points to key name             */
								   "",  					/* Default string                 */
								   mpav_error_warning_file_path,              	/* points to destination buffer   */
								   sizeof(mpav_error_warning_file_path)-1,   	 /* size of destination buffer     */
								   filename                  /* points to ini filename         */
						 	   );

   rc = GetPrivateProfileString(
								  "ERROR_WARNING",             /* points to section name         */
								  appname,       	/* points to key name             */
								  "",  					/* Default string                 */
								  mpav_module_error_warning_file_name,              	/* points to destination buffer   */
								  sizeof(mpav_module_error_warning_file_name)-1,   	 /* size of destination buffer     */
								  filename                  /* points to ini filename         */
								);

   rc = GetPrivateProfileString(
							  "ERROR_WARNING",             /* points to section name         */
							  "ERROR_WARNING_FILE_SIZE",       	/* points to key name             */
							  "500",  					/* Default string                 */
							  mpav_error_warning_file_size,              	/* points to destination buffer   */
							  sizeof(mpav_error_warning_file_size)-1,   	 /* size of destination buffer     */
							  filename                  /* points to ini filename         */
							);

	/* File size size conversion */
	Max_File_Size_Defined = atoi(mpav_error_warning_file_size);
	if(Max_File_Size_Defined <= 0)
	{
		Max_File_Size_Defined = 500 ;
	}
	Max_File_Size_Defined = Max_File_Size_Defined * ONE_MB_IN_BYTES ; /* 1 MB = 1048576 BYTES*/


   if((strlen(mpav_error_warning_file_path) > 0) &&
	  (strlen(mpav_module_error_warning_file_name)> 0))
   {
	   mpav_create_Error_Filename();
   }
}

/******************************************************************************
 *
 *  NAME:         mpav_log_message
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
void mpav_log_message( INT dest, INT sev, pCHAR msg, pCHAR func, int details )
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
   sprintf(text_message,"%s ,",appname);
   Appname_len = strlen(text_message);

   memcpy( text_message + Appname_len, msg, (sizeof(text_message)-1 -Appname_len));
   /* Call function to post the message. */
   mpav_Log_error_warning_to_File(text_message,sev,func,details);
   if(monitor_flag == 1)
   {
	 TxUtils_Send_Msg_To_Operator( monitor_flag, text_message,
								   eventlog_flag, msg_type, func,
								   severity, log_type,
								   NULL, NULL,NULL );
   }

   return;
}
INT mpav_Log_error_warning_to_File(pCHAR Error_Buf,int sev,pCHAR func, int detail)
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
    len = strlen(mpav_Erro_warning_Filename);
    path_len = strlen(mpav_error_warning_file_path);
    if( len == 0 ||
    	path_len==0 )
    {
    	LogEvent(Error_Buf,INFO_MSG);
    	return 0;
    }
    if ( 0 != strcmp(&mpav_Erro_warning_Filename[len-4], current_mmdd) )
    {
		/* Now create the new filename.*/
    	mpav_create_Error_Filename();
    }
	if((fp = fopen(mpav_Erro_warning_Filename,"a+b"))==NULL)
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
*  AUTHOR:       Abhishek Kumar Verma
*
******************************************************************************/
void get_date( pCHAR time_date, pCHAR date_str )
{
	memset ( date_str,  0x00,         9 );
	strncpy( date_str,  time_date,    4 );
	strncat( date_str, &time_date[5], 2 );
	strncat( date_str, &time_date[8], 2 );
}


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
*  Author      :     Abhishek Kumar Verma
*
**********************************************************************************/
void write_to_dump_file(CHAR* dump_record)
{
   CHAR              tempstr[16] = "";
   CHAR              buffer[500] = "";
   CHAR				 time_hhmmss[7] = "";
   CHAR				 time_str[10] = "";
   CHAR              time_date[25] = "";

   memset (&buffer, 0x00, sizeof(buffer));

   /* Check if file exists*/
   if (dumpfile_ptr == NULL_PTR)
   {
       /*** Open dump file ***/
	   memset (&dump_filename, 0x00, sizeof(dump_filename));
       strcpy(dump_filename, path);
       #ifndef WIN32
	       strcat(dump_filename, "bin/Dump_Preauth_Validation_");
       #else
	       strcat(dump_filename, "bin\\Dump_Preauth_Validation_");
       #endif

	   	/* Set dates for Exception processing. */
	   	ptetime_get_timestamp( time_date );  /* YYYY-MM-DD-hh.mm.ss.jjj */
	   	get_date( time_date, CurrentDate );  /* YYYYMMDD */

	   	strcat(dump_filename,CurrentDate);
	   	strcat(dump_filename,".txt");


	   /* Send message to system monitor */
       sprintf(buffer,"Opening dump file %s.", dump_filename);
       mpav_log_message( 2, 1, buffer, "write_to_dump_file", 0 );
       /* Send message to DOS window in debug mode */
       strcat(buffer,"\n");
	   dumpfile_ptr = fopen (dump_filename, "w");

	   if (dumpfile_ptr == NULL_PTR)
	   {
		  memset (&buffer, 0x00, sizeof(buffer));
		  sprintf(buffer, "Cannot open dump file %s.", dump_filename);
		  mpav_log_message( 2, 3, buffer, "write_to_dump_file", 0 );
		  return;					      /*will try to continue settlement process*/
	   }
	   else
	   {
	   	   sprintf(buffer, " Pre auth validation Dump File: %s\n", dump_filename);
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
      mpav_log_message( 2, 3, buffer, "write_to_dump_file", 0 );
   }
   return;

} /* write_to_dump_file */


/*******************************************************************************
*
*  FUNCTION:         open_preauth_validation_file
*
*  DESCRIPTION:      This function opens the  Preauth Validation file file
*                    or creates it if it doesn't exist.
*
*  INPUTS:           None
*
*  OUTPUTS:          None
*
*  RETURN VALUE:     BOOLEAN true for success or false for failure.
*
*  AUTHOR:           Abhishek Kumar Verma
*
*******************************************************************************/
BOOLEAN open_preauth_validation_file(pCHAR date)
{
	CHAR               buffer[500];
	INT                file_name_len=0;
	CHAR               time_date[25] = "";
	char               temp_date[9]="";

	memset(buffer, 0, sizeof(buffer));
	strncpy(temp_date,date,9);

	/* Send message to system monitor */
	strcpy(buffer,"Open Preauth Validation file ");
	mpav_log_message( 2, 1, buffer, "open_preauth_validation_file", 0 );
	/* Send message to DOS window in debug mode */
	strcat(buffer,"\n");

	/*** Open file ***/

	strcpy(E_filename,"Preauth_Validation_");

	strcat(E_filename,temp_date);
	strcat(E_filename,".txt");

	E_file_write = fopen (E_filename, "w");

	if (E_file_write == NULL_PTR)
	{
		strcpy(buffer, "Cannot open preauth validation file: ");
		strcat(buffer, E_filename);
		mpav_log_message( 2, 1, buffer, "open_preauth_validation_file", 0 );
		write_to_dump_file(buffer);

	return(false);
	}

	return (true);

} /* open_credit_file */

/*******************************************************************************
*
*  FUNCTION:         create_write_merchant_preauth_validation_header
*
*  DESCRIPTION:      This function creates header structures for
*                    merchant preauth validations transaction.
*
*  INPUTS:           None
*
*  OUTPUTS:          pCHAR c_record -  header structure
*
*  RETURN VALUE:     None
*  AUTHOR:          ABHISHEK KUMAR VERMA
*
*******************************************************************************/
BOOLEAN create_write_merchant_preauth_validation_header(void)
{
	CHAR              tempstr[16]="";
	CHAR              buffer[100]="";
 	CHAR              header[512]=" ";

    memset(header, 0, sizeof(header));
    strcpy(header,"CARD NUMBER              | MERCHANT ID    | TRAN DATE | AUTH NBR | POS ENT MODE |TRAN AMT     |MCC |CURR CODE ");
	strncpy(header + 110, "\n\0", 2);

	/*** Write to file ***/
   if (fputs(header,E_file_write) < 0)
   {
	  itoa (ferror(E_file_write), tempstr, 10);
	  fclose (E_file_write);

	  strcpy(buffer, "Cannot write to preauth validation file: ");
	  strcat(buffer, tempstr);
	  mpav_log_message( 2, 3, buffer, "create_write_merchant_preauth_validation_header", 0 );
	  write_to_dump_file(buffer);

	  return(false);
   }

   return (true);
} /* create_exception_headers */

/*******************************************************************************
*
*  FUNCTION:         create_trailer_merchant_preauth_validation_headers
*
*  DESCRIPTION:      This function creates trailer structures for
*                    the credit and debit settlement files.
*
*  INPUTS:           None
*
*  OUTPUTS:          pCHAR c_record -  header structure
*
*  RETURN VALUE:     None
*  AUTHOR:          ABHISHEK KUMAR VERMA
*
*******************************************************************************/
BOOLEAN create_trailer_merchant_preauth_validation_headers(void)
{
	CHAR              tempstr[16]="";
	CHAR              buffer[100]="";
	CHAR              total_reocrds[256]=" ";
	CHAR              total_line[256]=" ";
	CHAR              trailer[256]="";
	INT					line_count=0;
	INT 				len=0;

	/*** Write to file ***/
	if (fputs(trailer,E_file_write) < 0)
	{
		itoa (ferror(E_file_write), tempstr, 10);
		fclose (E_file_write);

		strcpy(buffer, "Cannot write to preauth validation file: ");
		strcat(buffer, tempstr);
		mpav_log_message( 2, 3, buffer, "create_trailer_merchant_preauth_validation_headers", 0 );
		write_to_dump_file(buffer);

		return(false);
	}

   return (true);
} /* create_exception_headers */

void EndProcess()
{
	char      ErrorMsg[100] = "";
	DBTEST_disconnect(ErrorMsg);
	printf("Database disconnect sucess:\n");
}

INT main( INT argc, pCHAR argv[] )
{
	char ErrorMsg[256] = {0};
	BYTE date[9]={0};
	INT  mode=0;
	BYTE str_mpde[2]={0};
	INT ret_val = 0 ;

	GetAppName(app_name);
	ret_val=get_dataserver_ini_db_error_statics_value( &db_error_statics_flag, &db_error_statics_value,
			app_name,     ErrorMsg );
	mpav_get_error_warning_file_name_path();
	memset( &tlf01, 0x00, sizeof( tlf01 ) );
	if ( argc == 3 )
	{
		if (DBTEST_connect("equitable", "equitable", "equitable", "ORCL", ErrorMsg) == 0)
		{
			printf("Database connect sucess:\n");
		}
		else
		{
			printf("Database connect failed::%s\n",ErrorMsg );
			EndProcess();
		}

		/* Get root directory path */
		memset (path, 0x00, sizeof(path));
		GetAscendentRootDirectory(path);
		read_PREAUTH_COMPLETION_VALIDATION_Eror_Response_code();
		get_declined_RC_for_insufficient_Preauth();
		get_declined_rc_for_expired_Used_preauths();
		get_declined_rc_for_original_txn_not_present();
		/*Lets open preauth validation file*/
		 strncpy(date, argv[1],9);
		 if(false == validate_date_yyyymmdd(date,ErrorMsg))
		 {
			 strcpy(ErrorMsg,"Invalid date");
			 DisplayVersions();
			 write_to_dump_file(ErrorMsg);
		 }

		 if(false == open_preauth_validation_file(date))
		 {
			strcpy(ErrorMsg,"error while opening Perauth Error validation file");
			write_to_dump_file(ErrorMsg);
		 }

		 /* We are able to open the file. Lets create the header.*/
		 if(false == create_write_merchant_preauth_validation_header())
		 {
			 strcpy(ErrorMsg,"error while writing header information for Perauth Error validation file");
			 write_to_dump_file(ErrorMsg);
		 }

		/* Lets pull out the records*/
		 strncpy(str_mpde, argv[2],1);
		 mode =atoi(str_mpde);
		 if(mode == 0)
		 {
			 db_select_tlf01(&tlf01,date,ErrorMsg);
		 }
		 else
		 {
			 db_select_tlf01_archive(&tlf01,date,ErrorMsg);
		 }
		EndProcess();
    }
	else
	{
		DisplayVersions();
	}
	return 0;
}

void trimleadingZero(char * string1, char * string2)
{
	int i;
	int len=0;
	len = strlen(string1);

		for(i =0; i < len; i++)
		{
			if(0!=strncmp(string1+i,"0",1))
			{
				strcat(string2,string1+i);
				i = len;
			}
		}
}

void formatAmount(char * string1, char * string2)
{
	INT temp_amount_len = 0;
	BYTE amount[20]="";
	BYTE TEMP_AMT[20]="";
	INT i=0;

	temp_amount_len = strlen(string1);
		if(temp_amount_len>2)
		{
			strncpy(amount,string1,(temp_amount_len - 2));
			temp_amount_len= strlen(amount);
			strncpy(TEMP_AMT,amount,temp_amount_len);
			Strrev(TEMP_AMT);
			if(temp_amount_len > 3)
			{
				memset(amount,0x00,20);
				for(i =0; i < temp_amount_len; i++)
				{
					strncat(amount,TEMP_AMT+i,3);
					strcat (amount, ",");
					i=i+2;
				}
				Strrev(amount);
				if(0==strncmp(amount,",",1))
				{
					amount[0]=' ';
				}
			}
		}
		else
		{
			strncpy(amount,"0",1);
		}
		temp_amount_len = strlen(amount);
	    strncpy(string2,amount,temp_amount_len);

}

INT find_month(char * dateDD_MMM_YY)
{
	BYTE month[4]={0};

    strcpy(month,dateDD_MMM_YY+3,3);//11-MAR-13

	if((0 == strncmp(month,"JAN",3))||
	   (0 == strncmp(month,"jan",3)))
	{
		return 1;
	}
	else if((0 == strncmp(month,"FEB",3))||
	   (0 == strncmp(month,"feb",3)))
	{
		return 2;
	}
	else if((0 == strncmp(month,"MAR",3))||
	   (0 == strncmp(month,"mar",3)))
	{
		return 3;
	}
	else if((0 == strncmp(month,"APR",3))||
	   (0 == strncmp(month,"apr",3)))
	{
		return 4;
	}
	else if((0 == strncmp(month,"MAY",3))||
	   (0 == strncmp(month,"may",3)))
	{
		return 5;
	}
	else if((0 == strncmp(month,"JUN",3))||
	   (0 == strncmp(month,"jun",3)))
	{
		return 6;
	}
	else if((0 == strncmp(month,"JUL",3))||
	   (0 == strncmp(month,"jul",3)))
	{
		return 7;
	}
	else if((0 == strncmp(month,"AUG",3))||
	   (0 == strncmp(month,"aug",3)))
	{
		return 8;
	}
	else if((0 == strncmp(month,"SEP",3))||
	   (0 == strncmp(month,"sep",3)))
	{
		return 9;
	}
	else if((0 == strncmp(month,"OCT",3))||
	   (0 == strncmp(month,"oct",3)))
	{
		return 10;
	}
	else if((0 == strncmp(month,"NOV",3))||
	   (0 == strncmp(month,"nov",3)))
	{
		return 11;
	}
	else if((0 == strncmp(month,"DEC",3))||
	   (0 == strncmp(month,"dce",3)))
	{
		return 12;
	}

	return 0;

}

/*******************************************************************************
*
*  FUNCTION:         write_to_preauth_validation_file
*
*  DESCRIPTION:      This function writes to  Preauth Validation file.
*
*  INPUTS:           None
*
*  OUTPUTS:          None
*
*  RETURN VALUE:     BOOLEAN true for success or false for failure.
*
*******************************************************************************/
BOOLEAN write_to_preauth_validation_file(pTLF01 tlf01_ptr)
{
	CHAR    tempstr[16]="";
	CHAR    buffer[100]="";
	BYTE	temp_card_num[20]={0};
	BYTE	temp_merchant_id[16]={0};
	BYTE	temp_tran_date[10]={0};
	BYTE	temp_auth_num[7]={0};
	BYTE	temp_pos_entry_mode[4]={0};
	BYTE	temp_tran_amount[13]={0};
	BYTE    temp_total_amount[13]={0};
	BYTE 	formated_total_amount[13]={0};
	BYTE    temp_amount_dec[3]={3};
	BYTE    temp_MCC[5]={0};
	BYTE    temp_Currency_Code[4]={0};

	strncpy(temp_card_num,tlf01_ptr->card_num,(sizeof(temp_card_num) -1) );
	strncpy(temp_merchant_id,tlf01_ptr->merchant_id,(sizeof(temp_merchant_id) -1));
	strncpy(temp_tran_date,tlf01_ptr->date_yyyymmdd,(sizeof(temp_tran_date) -1));
	strncpy(temp_auth_num,tlf01_ptr->auth_number,(sizeof(temp_auth_num) -1));
	strncpy(temp_pos_entry_mode,tlf01_ptr->pos_entry_mode+1,(sizeof(temp_pos_entry_mode) -1));
	strncpy(temp_tran_amount,tlf01_ptr->total_amount,(sizeof(temp_tran_amount) -1));//Copy 1st 10 byte of amout
	strncpy(temp_amount_dec,tlf01_ptr->total_amount+10, (sizeof(temp_amount_dec)-1));
	strncpy(temp_MCC,tlf01_ptr->category_code,(sizeof(temp_MCC) -1));
	strncpy(temp_Currency_Code,tlf01_ptr->currency_code,(sizeof(temp_Currency_Code) -1));

	trimleadingZero(temp_tran_amount,temp_total_amount);
	formatAmount(temp_total_amount,formated_total_amount);

	sprintf(record_preauth, "%-19s%22s%10s%10s%9s%21s.%02s %4s%4s",
			temp_card_num,
			temp_merchant_id,
			temp_tran_date,
			temp_auth_num,
			temp_pos_entry_mode,
			formated_total_amount,
			temp_amount_dec,
			temp_MCC,
			temp_Currency_Code
			);

   strncpy(record_preauth + 103, "\n\0", 2);

   /*** Write to file ***/
   if (fputs(record_preauth,E_file_write) < 0)
   {
      itoa (ferror(E_file_write), tempstr, 10);
      fclose (E_file_write);

      strcpy(buffer, "Cannot write to preauth validation file: ");
      strcat(buffer, tempstr);
      mpav_log_message( 2, 3, buffer, "write_to_preauth_validation_file", 0 );
	  write_to_dump_file(buffer);

      return(false);
   }

   return (true);

} /* write_to_preauth_validation_file */


/*******************************************************************************
*
*  FUNCTION:         write_to_preauth_validation_file
*
*  DESCRIPTION:      This function writes to Preauth Validation file.
*
*  INPUTS:           None
*
*  OUTPUTS:          None
*
*  RETURN VALUE:     BOOLEAN true for success or false for failure.
*
*******************************************************************************/
BOOLEAN write_to_preauth_validation_archive_file(pTLF01 tlf01_ptr)
{
	CHAR    tempstr[16]="";
	CHAR    buffer[100]="";
	BYTE	temp_card_num[20]={0};
	BYTE	temp_merchant_id[16]={0};
	BYTE	temp_tran_date[10]={0};
	BYTE	temp_auth_num[7]={0};
	BYTE	temp_pos_entry_mode[4]={0};
	BYTE	temp_tran_amount[13]={0};
	BYTE    temp_total_amount[13]={0};
	BYTE 	formated_total_amount[13]={0};
	BYTE    temp_amount_dec[3]={3};
	BYTE    temp_MCC[5]={0};
	int     month=0;
	BYTE    str_month_MM[3]={0};
    BYTE    formate_tran_date_YYYYMMDD[10]={0};
    BYTE    temp_Currency_Code[4]={0};

	strncpy(temp_card_num,tlf01_ptr->card_num,(sizeof(temp_card_num) -1) );
	strncpy(temp_merchant_id,tlf01_ptr->merchant_id,(sizeof(temp_merchant_id) -1));
	strncpy(temp_tran_date,tlf01_ptr->date_yyyymmdd,(sizeof(temp_tran_date) -1));
	strncpy(temp_auth_num,tlf01_ptr->auth_number,(sizeof(temp_auth_num) -1));
	strncpy(temp_pos_entry_mode,tlf01_ptr->pos_entry_mode+1,(sizeof(temp_pos_entry_mode) -1));
	strncpy(temp_tran_amount,tlf01_ptr->total_amount,(sizeof(temp_tran_amount) -1));//Copy 1st 10 byte of amout
	strncpy(temp_amount_dec,tlf01_ptr->total_amount+10, (sizeof(temp_amount_dec)-1));
	strncpy(temp_MCC,tlf01_ptr->category_code,(sizeof(temp_MCC) -1));
	strncpy(temp_Currency_Code,tlf01_ptr->currency_code,(sizeof(temp_Currency_Code) -1));

	trimleadingZero(temp_tran_amount,temp_total_amount);
	formatAmount(temp_total_amount,formated_total_amount);

	month=find_month(temp_tran_date);
	if(month==0)
	{
		strncpy(formate_tran_date_YYYYMMDD,temp_tran_date,(sizeof(formate_tran_date_YYYYMMDD) -1));
	}
	else
	{
		//temp_tran_date having value as 11-MAR-13
		sprintf(str_month_MM,"%02d",month);
		strncpy(formate_tran_date_YYYYMMDD,tlf01_ptr->system_date,2);//Copy Century
		strncat(formate_tran_date_YYYYMMDD,temp_tran_date+7,2);//Copy Year
		strncat(formate_tran_date_YYYYMMDD,str_month_MM,2);//Copy Month
		strncat(formate_tran_date_YYYYMMDD,temp_tran_date,2);//Copy date
	}


	sprintf(record_preauth, "%-19s%22s%10s%10s%9s%21s.%02s %4s%4s",
			temp_card_num,
			temp_merchant_id,
			formate_tran_date_YYYYMMDD,
			temp_auth_num,
			temp_pos_entry_mode,
			formated_total_amount,
			temp_amount_dec,
			temp_MCC,
			temp_Currency_Code
			);

   strncpy(record_preauth + 103, "\n\0", 2);

   /*** Write to file ***/
   if (fputs(record_preauth,E_file_write) < 0)
   {
      itoa (ferror(E_file_write), tempstr, 10);
      fclose (E_file_write);

      strcpy(buffer, "Cannot write to preauth validation file: ");
      strcat(buffer, tempstr);
      mpav_log_message( 2, 3, buffer, "write_to_preauth_validation_archive_file", 0 );
	  write_to_dump_file(buffer);

      return(false);
   }

   return (true);

} /* write_to_preauth_validation_archive_file */


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


   DWORD rc;
   CHAR  filename   [80];
   CHAR  tmpstr     [80];
   CHAR  timer_flag [5] = {0};
   CHAR  timer_value[5] = {0};
   INT   ret_val = true;
   CHAR db_nullchk_flag[5] = {0};

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
      //memcpy( db_nullchk_flag, 0x00, sizeof(db_nullchk_flag) );
       //rc = GetPrivateProfileString(
         //       "DBNULLCHECK",        /* points to section name         */
           //     "DB_NULL_CHECK_FLAG",       /* points to key name             */
             //    "0",                /* Default string (Off)           */
				//  db_nullchk_flag,          /* points to destination buffer   */
               //  sizeof(db_nullchk_flag)-1,/* size of destination buffer     */
               //  filename             /* points to ini filename         */
      // );
      // DB_NullCheck_Flag = atoi(db_nullchk_flag);

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

INT read_PREAUTH_COMPLETION_VALIDATION_Eror_Response_code( )
{
	DWORD rc;
	CHAR filename     [MAX_FILENAME_SIZE];
	CHAR tmpstr       [MAX_FILENAME_SIZE];
	CHAR tmpstring    [MAX_INI_INFO_SIZE];
	CHAR ini_info     [MAX_INI_INFO_SIZE];
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
	strncpy( temp_response_code, tmpstring, 2 );
	if(strlen(temp_response_code)==0)
	{
	   strncpy( temp_response_code,
				"05",RESPONSE_CODE_LEN);// Default Value
	}
	return( ret_val );
	}
/*********************************************************/

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


