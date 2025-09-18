/*******************************************************************************
*  
* MODULE:      import.c
*  
* TITLE:       
*  
* DESCRIPTION:
*
* APPLICATION: Ascendent import Processor
*
* AUTHOR:  Faramarz Arad
*
* REVISION HISTORY:
*
* $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\equi_hpux\import\import.c  $
   
      
		Rev 1.33 Nov-03-2010 TF-Ajay
	Changed to update secondary expiry date in CCF02p.maint_date

      Rev 1.32   Jul 17 2006 13:26:14   dirby
   The database timing report was not going into syslog. So I modified
   it to send its report to the LOG file.
   SCR 12299
   
      Rev 1.31   Jul 12 2006 15:53:44   dirby
   Implemented functionality to gather statistics on database SQL calls.
   SCR 12299
   
      Rev 1.30   Feb 16 2006 17:52:14   dirby
   Modified to import device type from device records.  The device
   type is a one byte field on the import record.  Record size did not
   change because there were 3 filler bytes at the end. Using the first
   one for the device type.  Here are the device type definitions:
      2 = EDC
      3 = ISO
      4 = Visa I
      5 = VAT
      6 = CAT
      7 = EMV (Smart Card)
      8 = Pay Pass
      Default value is ISO.
   These one-byte values translate into 2 bytes in the database with
    the first byte being zero.  For example, EMV = '07'.
   SCR 19844
   
      Rev 1.29   Apr 05 2005 14:19:02   dirby
   Updated version to 4.4.1.1
   SCR 12785
   
      Rev 1.28   Apr 01 2005 10:16:10   lmitchel
   Initial Checking prior to integration testing for updates made to support import of new fields: ccf02p.peso_tran_limit and ccf02p.dollar_tran_limit(scr12785)
   
      Rev 1.27   Oct 04 2004 14:10:36   dirby
   Removed a line that was checking the Reload flag to be numeric.
   This caused the Reload flag to not get set for Reload merchants.
   SCR 12693
   
      Rev 1.26   Jul 16 2004 16:16:20   lmitchel
   Changed reload_merchant field valid values = Y or N
   
      Rev 1.25   Jul 14 2004 16:37:14   lmitchel
   Ver 4.4.0.1: Added new auto_init fields to the device import and an reload flag to the merchant import.
   
      Rev 1.24   Jul 08 2004 18:07:44   dirby
   Updated to 4.4.0.1
   SCRs 1287 & 1388
   
      Rev 1.23   Jun 09 2004 14:56:14   dirby
   Corrected bug when parsing negfraud trailer - in memset, use
   sizeof structure name instead of variable name.  Changed strncpy
   to memcpy.  Windows could handle the code as it was, but on AIX,
   the trailer parsing was not putting nulls after some fields.
   SCR 1373
   
      Rev 1.22   Jun 04 2004 14:36:22   lmitchel
   Ver 4.3.0.2 - Added support for processing Negative Fraud Import Files
   
      Rev 1.21   May 28 2004 07:53:22   dirby
   Updated version to 4.3.0.1
   SCR 1380
   
      Rev 1.20   May 28 2003 10:43:14   dirby
   Modified to use Rollback Segment RBS37 instead of RBS36.
   This is at Equitable's request.
   SCR 935
   
      Rev 1.19   Apr 02 2003 11:51:50   svelaga
   SCR 817: Merchant import counts are added.
   
   Version number is changed to 4.2.0.2.
   
      Rev 1.18   Mar 05 2003 17:31:34   dirby
   Changed 3rd parameter of GetPrivateProfileString from 0 to a
   Null string.  This is so it runs on unix.
   SCR 955
   
      Rev 1.17   Mar 04 2003 14:46:46   dirby
   Set version back to 4.2.0.1 - It only needs to be incremented
   once each time the customer receives a copy of it in a release.
   SCR 955
   
      Rev 1.16   Feb 27 2003 11:37:20   lmitchel
   Ver: 4.2.0.2: Modified process_terminal_import  to read import records of a configurable length and parse terminal CWK keys of a configurable length.  Added function, get_ini_info to get the record length and key_length.  The ini file is read during import of terminal records.
   
      Rev 1.15   Feb 14 2003 11:07:52   lmitchel
      Version 4.2.0.1
     Added function to use rollback segment RBS36 and set to an online state throught the import process.
   Set RBS36 to offline state when Import finishes or exits.  Replaced some exit(0) calls with new exit_import call to disconnect from db and set RBS36 offline before terminating program.
   
      Rev 1.14   Aug 19 2002 19:01:34   dirby
   1.  Added 4 fields to the merchant record:
     Deferred Start Date
     Deferred End Date
     Group Outlet COde
     Group Type
   SCR 255
   
   2.  Increased the size of the record count field in all trailer
   records from 6 to 9.
   SCR 727
   
      Rev 1.13   Nov 28 2001 09:19:46   dirby
   Removed some comments within other comments in order to
   get a clean compile on Unix.     SCR 256
   
      Rev 1.12   Nov 05 2001 16:39:26   dirby
   1.  Added Add_On_Rate and Add_On_Term to CCF02P card records.
   2.  Increased the size of card records from 520 to 540.
   
      Rev 1.11   Jul 12 2001 15:14:40   SYARLAGA
   Modified the main function.
   
        
      Rev 1.10   Jul 12 2001 10:42:06   SYARLAGA
   Updated version to 3.2.3.2.Added functions to compare 2 files and update only  those records which are modified.
   
      Rev 1.4   Apr 26 2001 13:38:36   dirby
   1.  Updated version to 3.2.2.2
   2.  Added date/time to all log and dump entries.
   3.  Changed some log messages to correctly reflect
   which database tables were updated.
   
      Rev 1.3   Feb 14 2001 12:22:46   dirby
   Version 3.2.2.1
   Modified to import without the use of 'select' statements.
   This speeds it up, since 98-99% of EB's imports are
   updates, not inserts.  This was built on HP UX 11 (DICK).
   
      Rev 1.2   Feb 22 2000 14:16:40   farad
   Modified the code such that it would take the file name and use that for the dump and the log file's name.
   
      Rev 1.0   Aug 05 1999 15:43:18   farad
   The initial version of the import app in HP.
   
      Rev 1.20   Jul 29 1999 14:08:42   farad
   Fixed some issues with the decimal points in value fields.  
   Fixed some issues with fields positions shifted.
   
      Rev 1.19   Jul 26 1999 14:37:20   MSALEH
   Modifications to address performance issues
   with import functionality
   All five import types are affected
   
      Rev 1.18   Jul 22 1999 15:56:42   MSALEH
   Modified the merhcant file processing to improve 
   performance
   
      Rev 1.17   Jul 17 1999 11:09:22   farad
   Made the code faster by taking out extra function calls, reducing the number of checks.
   
      Rev 1.16   Jul 07 1999 14:02:42   farad
   Fixed bug # 982.  The code should no longer populate table ccf03p.
   
      Rev 1.15   Jun 29 1999 15:15:10   farad
    
   
      Rev 1.14   Jun 10 1999 16:45:58   farad
   Modified the code to disconnect from the DS.
   
   
      Rev 1.13   Jun 08 1999 17:02:12   farad
   Modified the code so it does not scan a file twice any more.
   
      Rev 1.12   Jun 04 1999 16:49:00   farad
   Added the import_db.pc.  There is no need to run the trands, devds, and the netds to run this application any more.
   
      Rev 1.11   May 17 1999 16:44:08   farad
   Modified the code such that it does xipcshutdown instead of xipclogout
   
      Rev 1.10   Apr 28 1999 10:55:22   farad
   Looked at bug 460 and 560 again.  Made sure the class code is an optional field.
   
      Rev 1.9   Apr 15 1999 14:38:28   farad
   Took out some printfs,  Fixed some bugs.  Did some enhancements.
   
      Rev 1.8   Apr 08 1999 16:27:44   farad
   Added the ? command line option.  Took out the extra @ for the AIX machine.
   
      Rev 1.7   Apr 07 1999 15:06:46   farad
   Fixed bug #303, #335.  Added more error messages so the AIX issues over at Equitable could be detected.
   
      Rev 1.6   Apr 02 1999 14:47:50   farad
   Fixed bug #400,334,338, and 305.  Populates the CCF03P table.
   
      Rev 1.5   Mar 30 1999 13:17:04   farad
   Took out all of the prinft statements.  Worked on bug #401.
   
      Rev 1.4   Mar 18 1999 14:39:48   farad
   Fixed bugs #250,251,193,253,246,252,255,247
   
      Rev 1.3   Mar 16 1999 14:33:54   farad
   Fixed bug # 193, 118, 251.
   
      Rev 1.2   Mar 02 1999 10:44:02   farad
   The old pin off set in the card holder record included the pin_offset and the pvki.  This change was implemented on 3/2/1999 by farad.
   
      Rev 1.1   Feb 24 1999 17:42:18   farad
   Fixed bug #1,2,3,4,5,6,7
   Did enhancements requested by Tomi and Rod.
   
      Rev 1.0   Feb 22 1999 16:41:48   farad
   Initial release
*
*
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef WIN32
#include <conio.h>
#include <windows.h> /*LM 02/24/03: added to link sucessfully with "GetPrivateProfileString" */
#endif 

#include "ntutils.h"
#include "ptetimer.h"
#include "ptetime.h"
#include "dbcommon.h"
#include "app_info.h"
#include "equitdb.h"

#include "dc_dbstruct.h"
#include "dc_database.h"

#include "tx_dbstruct.h"
#include "tx_database.h"

#include "nc_dbstruct.h"
#include "nc_database.h"

#include "txutils.h"
#include "txdsapi.h"
#include "timings.h"

#include "import.h"


/* Database Timing Measurements */
TIMINGS_LIST  TimingStats;
INT           DB_Timing_Flag; /* True = Do DB Timing Stats, False = don't do */
INT           ForceReport;
static INT   Count = 0;
CHAR          ReportTime[5];

CHAR AppName   [80]  ={0};

#ifdef  _DEBUG
   #define MYPRINT(tempMsg)  printf("%s ",tempMsg);           
#else
   #define MYPRINT(tempMsg)  ;
#endif                              

#define TRUE	1
#define FALSE	0


FILE *fptr;
FILE *fptr1;
FILE *dumpPtr;
FILE *ImportLog;
FILE *skipRecordPtr;



FILE  *HstFilePtr;
INT fprint_retval = 0;
CHAR  HST01_Filename[256] ={0};
CHAR  import_hst01_file_path[256] = {0};
CHAR masked_fields[151] = {0};

FILE * HstMcF01Ptr;
CHAR  HST01MCF01_Filename[256] ={0};
CHAR  import_hst01mcf01_file_path[256] = {0};

char cardtype_details[820]={0};
/*********************** GLOBAL VARIABLES *********************************/

CHAR        trands_que_name[] = "trandsA";
CHAR        applnk_que_name[] = "applnkA";
INT			TermRecLength = 0;
INT			TermKeyLength = 0;


CHAR        Version[] = "ATP_11.10.0";
								   

/* These are text messages used when logging errors */
CHAR        merchant_record[] = "Merchant Records (MCF01)";
CHAR        account_record [] = "Account Records (ACF01)";
CHAR        primary_record [] = "Primary Account Records (PAF01)";
CHAR        device_record  [] = "Device Records (DCF01)";
CHAR        card1_record   [] = "Card Records (CCF01)";
CHAR        card2_record   [] = "Card Records (CCF02P)";
CHAR        card1_2_record [] = "Card Records (CCF01 & CCF02P)";
CHAR        card3_record   [] = "Card Records (CCF03)";
CHAR        deferred_record[] = "Deferred Records (DEF01)";
CHAR        card_type_record[]= "Merchant Card Type Records (MCT01)";
CHAR        merchant_records[]= "Merchant Records (MCF01 & MCT01 & DEF01)";
CHAR        negative_fraud_record[] = "Negative Fraud Records (NEGATIVE_FRAUD)";
CHAR        histort_record  [] = "History Records (HST01)";
CHAR        mpparameter_record [] = "MpParameter Records (MPPARA01)";
CHAR        merchantgroup_record [] = "MerchantGroup Records (MGF01)";


CHAR  ini_filename[256] = {0};
CHAR  config_record_length[10] = {0};
INT   int_record_length = 0;

ImportCount Import_count;
CHAR countFileName[40]={0};

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

/*****************************************************************************/
#define   NUM_DB_TIMING_TABLES     75

typedef struct
{
   BYTE  type;
   BYTE  name[24];

}DB_TIMING_TABLE_DEFS, *pDB_TIMING_TABLE_DEFS;


DB_TIMING_TABLE_DEFS  DB_Table_Defs[NUM_DB_TIMING_TABLES] =
{
   BIN01_DATA,                  "BIN01",
   OCF01_DATA,                  "OCF01",
   OCF20_DATA,                  "OCF20",
   MCF01_DATA,                  "MCF01",
   MCF20_DATA,                  "MCF20",
   DCF01_DATA,                  "DCF01",
   BCH01_DATA,                  "BCH01",
   BCH10_DATA,                  "BCH10",
   NCF01_DATA,                  "NCF01",
   NCF20_DATA,                  "NCF20",
   UCF01_DATA,                  "UCF01",
   UCF02_DATA,                  "UCF02",
   CAF01_DATA,                  "CAF01",
   CCF01_DATA,                  "CCF01",
   ACF01_DATA,                  "ACF01",
   CCF03N_DATA,                 "CCF03N",
   CCF03P_DATA,                 "CCF03P",
   CCF02H_DATA,                 "CCF02H",
   CCF02P_DATA,                 "CCF02P",
   CCF10_DATA,                  "CCF01",
   HST01_DATA,                  "HST01",
   ATP01_DATA,                  "ATP01",
   ATP02_DATA,                  "ATP02",
   CTF01_DATA,                  "CTF01",
   MCT01_DATA,                  "MCT01",
   PAF01_DATA,                  "PAF01",
           26,                  "",
   CRF01_DATA,                  "CRF01",
   JBF01_DATA,                  "JBF01",
   HCF01_DATA,                  "HCF01",
   FAF01_DATA,                  "FAF01",
   CLF01_DATA,                  "CLF01",
   BPF01_DATA,                  "BPF01",
   APF01_DATA,                  "APF01",
   FCF02_DATA,                  "FCF02",
   TLF01_DATA,                  "TLF01",
   CSF01_DATA,                  "CSF01",
   ALL_DATA,                    "ALL_DATA",
   NCF02_DATA,                  "NCF02",
   OPRMON_DATA,                 "OPRMON",
   NBF01_DATA,                  "NBF01",
   NGF01_DATA,                  "NGF01",
   NFI01_DATA,                  "NFI01",
   NMF01_DATA,                  "NMF01",
   NFI02_DATA,                  "NFI02",
   BCH11_DATA,                  "BCH11",
   NTF01_DATA,                  "NTF01",
   DRF01_DATA,                  "DRF01",
   DEF01_DATA,                  "DEF01",
   CCF04_DATA,                  "CCF04",
   BON01_DATA,                  "BON01",
   UAG01_DATA,                  "UAG01",
   BIN02_DATA,                  "BIN02",
   NCF21_DATA,                  "NCF21",
   NCF30_DATA,                  "NCF30",
   NEG01_DATA,                  "NEG01",
   ATM01_DATA,                  "ATM01",
   TLF01_SAF_DATA,              "TLF01_SAF",
   AUTH_TX_DATA,                "AUTH_TX",
   MCC102_DATA,                 "MCC102",
   MCC103_DATA,                 "MCC013",
   TLF01_GUI_LIST_DATA,         "TLF01_GUI_LIST",
   BCH01_BCH10_TLF01_DATA,      "BCH01_BCH10_TLF01",
   BCH20_DATA,                  "BCH20",
   SAF01_DATA,                  "SAF01",
   OFFUS_VELOCITY_DATA,         "OFFUS_VELOCITY",
   NEGATIVE_FRAUD_DATA,         "NEGATIVE_FRAUD",
   MCC_LIMITS_DATA,             "MCC_LIMITS",
   MCC_LIMITS_GUI_DATA,         "MCC_LIMITS_GUI",
   EMV01_DATA,                  "EMV01",
   COUNTRY_AMT_LIMITS_DATA,     "COUNTRY_AMT_LIMITS",
   COUNTRY_AMT_LIMITS_GUI_DATA, "COUNTRY_AMT_LIMITS_GUI",
   FG01_DATA,                   "FG01",
   FGUARD_DATA,                 "FGUARD",
   FGRAW_DATA,                  "FGRAW"
};
/*****************************************************************************/



/*************************************************************************************
*************************************************************************************/
void InitMsgLogging(char *fileName)
{
	ImportLog = fopen(fileName, "w");
	if (ImportLog == NULL)
	{ 
	  exit(0);
	}
}

/************************************************************************************
************************************************************************************/
void ShutDownMsgLogging()
{
	fclose(ImportLog);
}

/************************************************************************************
************************************************************************************/
void LogMsg(pCHAR msg, INT type)
{
	CHAR severity[16]  = "";
	CHAR errormsg[256] = "";
   CHAR time_date[25] = "";
	
	switch (type)
	{
		case ERROR_MSG:
			strcpy(severity, "ERROR");
			break;

		case WARN_MSG:
			strcpy(severity, "WARNING");
			break;

		case INFO_MSG:
			strcpy(severity, "FYI");
			break;

		default:
			strcpy(severity, "ERROR");
	}

   /* Get current system date & time */
   ptetime_get_timestamp( time_date );

	sprintf(errormsg, "%s  >>  Severity: %s, %s\n", time_date, severity, msg);
	fputs(errormsg, ImportLog);
	printf("%s\n",errormsg);
}

/* 
   type : indicates import file type 
     ( example : "merchant" , "card", "terminal" etc.. )
   indicator : indicates multiple files or single file
	   0 - indicates single
	   1 - indicates multiple files
*/
void LogCounts(pCHAR type, INT indicator)
{
	FILE *fcount;

	if ( NULL == (fcount = fopen(countFileName,"w")))
    {
      fputs("IMPORT: ImportCounts.txt file was not created",dumpPtr);
	  return;
    }

	fprintf(fcount,"*** IMPORT COUNT STATISTICS for %s ***\n\n\n", type);
	
	if(indicator)
	{
		fprintf(fcount," READ   COUNT : %ld \n", Import_count.readCount1);
		
		fprintf(fcount," INSERT COUNT : %ld \n", Import_count.insertCount);
		fprintf(fcount," UPDATE COUNT : %ld\n", Import_count.updateCount);

		fprintf(fcount," BYPASS COUNT : %ld\n", Import_count.skipCount1);

		fprintf(fcount," MATCH  COUNT : %ld\n", Import_count.matchCount);

		fprintf(fcount," ERROR  COUNT : %ld \n", Import_count.errorCount1);

		

	}
	else
	{
		fprintf(fcount," READ   COUNT : %ld\n", Import_count.readCount1);
		
		fprintf(fcount," INSERT COUNT : %ld\n", Import_count.insertCount);
		fprintf(fcount," UPDATE COUNT : %ld\n", Import_count.updateCount);

		fprintf(fcount," BYPASS COUNT : %ld\n", Import_count.skipCount1);

		fprintf(fcount," ERROR  COUNT : %ld\n", Import_count.errorCount1);
	}

	fclose(fcount);

}

INT isnum(pCHAR string , INT length)
{
   INT i;

   for (i = 0; i < length; i++)
   {
      if (!isdigit(string[i]))
        return(false);
   }
   return(true);
}

INT isalphanum(pCHAR string, INT length )
{
   INT i;

   for (i = 0; i < length; i++)
   {
      if (!isalnum(string[i]))
        return(false);
   }
   return(true);
}

INT isAlpha(pCHAR string , INT length)
{
   INT i;

   for (i = 0; i < length; i++)
   {
      if (!isalpha(string[i]))
        return(false);
   }
   return(true);
}

INT isnumSpace(pCHAR string , INT length)
{
   INT i;
   INT countNum = 0;

   for (i = 0; i < length; i++)
   {
   		if (!isdigit(string[i]))
  		{
  			if (string[i] == ' ')
			{
				if(countNum == 0 && i >= (length -1)  )
				{
					return(false);
				}
			}
			else
			{
				return(false);
  			}
  		}
		else if(isdigit(string[i]))
		{
			  	countNum ++;
		}
   }
   return(true);
}

INT isAlphaSpace(pCHAR string , INT length)
{
   INT i;
   INT countAlpha = 0;

   for (i = 0; i < length; i++)
   {
   		if (!isalpha(string[i]))
  		{
  			if (string[i] == ' ')
			{
				if(countAlpha == 0 && i >= (length -1)  )
				{
					return(false);
				}
			}
			else
			{
				return(false);
  			}
  		}
		else if(isalpha(string[i]))
		{
			  	countAlpha ++;
		}
	}
   
   return(true);
}

INT isalphanumSpaceSpecail(pCHAR string, INT length)
{
   INT i;
   INT countAlphaNum = 0;

   for (i = 0; i < length; i++)
   {
   		if (!isalnum(string[i]))
  		{
  			if (string[i] == ' ')
			{
				if(countAlphaNum == 0 && i >= (length -1)  )
				{
					return(false);
				}
			}
			else if(string[i] == '/')
			{
				countAlphaNum ++; // For Address we have to allow to add /
			}
			else if(string[i] == ',')
			{
				countAlphaNum ++; // For Address we have to allow to add /
			}
			else if(string[i] == '.')
			{
				countAlphaNum ++; // For Address we have to allow to add /
			}
			else if(string[i] == '@')
			{
				countAlphaNum ++; // For Address we have to allow to add /
			}
			else if(string[i] == '#')
			{
				countAlphaNum ++; // For Address we have to allow to add /
			}
			else if(string[i] == '&')
			{
				countAlphaNum ++; // For Address we have to allow to add /
			}
			else if(string[i] == '-')
			{
				countAlphaNum ++; // For Address we have to allow to add /
			}
			else if(string[i] == '|')
			{
				countAlphaNum ++; // For Address we have to allow to add /
			}
			else if(string[i] == ':')
			{
				countAlphaNum ++; // For Address we have to allow to add /
			}
			else if(string[i] == ';')
			{
				countAlphaNum ++; // For Address we have to allow to add /
			}
			else if(string[i] == '"')
			{
				countAlphaNum ++; // For Address we have to allow to add /
			}
			else if(string[i] == '\\')
			{
				countAlphaNum ++; // For Address we have to allow to add /
			}
			else if(countAlphaNum == 0 && i >= (length -1)  )
			{
				return(false);
  			}
  		}
		else if(isalnum(string[i]))
		{
			  	countAlphaNum ++;
		}
	}
   
   return(true);
}


INT isalphanumSpace(pCHAR string, INT length)
{
   INT i;
   INT countAlphaNum = 0;

   for (i = 0; i < length; i++)
   {
   		if (!isalnum(string[i]))
  		{
  			if (string[i] == ' ')
			{
				if(countAlphaNum == 0 && i >= (length -1)  )
				{
					return(false);
				}
			}
			else
			{
				return(false);
  			}
  		}
		else if(isalnum(string[i]))
		{
			  	countAlphaNum ++;
		}
	}
   
   return(true);
}
INT isalphanumSpaceDot(pCHAR string, INT length)
{
	INT i;
	INT countAlphaNum = 0;
	for (i = 0; i < length; i++)
	{
		if (!isalnum(string[i]))
		{
			if( string[i] == ' ')
			{
				if(countAlphaNum == 0 && i >= (length -1)  )
				{
					return(false);
				}
			}
			else if( string[i] == '.')
			{
				
			}
			else
			{
				return(false);
			}
		}
		else if(isalnum(string[i]))
		{
			countAlphaNum ++;
		}
	}
   return(true);
}
INT isnumdot(INT value )
{
	int i = 0;
    if (!isdigit(value))
    {
      		if( value != '.')
			   return(false);
     }
   return(true);
}

INT isNumSpaceDot(pCHAR string, INT length)
{
	INT i;
	INT countNum = 0;
	for (i = 0; i < length; i++)
	{
		if (!isdigit(string[i]))
		{
			if( string[i] == ' ')
			{
				if(countNum == 0 && i >= (length -1)  )
				{
					return(false);
				}
			}
			else if( string[i] == '.')
			{
				
			}
			else
			{
				return(false);
			}
		}
		else if(isdigit(string[i]))
		{
			countNum ++;
		}
	}
   return(true);
}

/*************************************************************************************/
void TrimLeadingZero(pCHAR input, INT cnt)
{
   int count =0;
   int digitflag = 0;
   int digitindex = 0;
   BYTE temp_str[13]="";
   strcpy(temp_str,input);
   while( cnt > 0 )
   {
		if((input[count]== '0')&& (digitflag==0))
		{
			input[count] = '\0';
		}
		else
		{
			digitflag = 1;
			//input[digitindex]=input[count];
			//digitindex++;
			strcpy(input,temp_str+count);
			cnt = 0;
		}
		cnt--;
		count++;
   }
}

/*************************************************************************************
       NAME:           returnNumdot()

       DESCRIPTION:    This module takes the character array.  It compares it
       against a number array.  When it enconters the first non number character or ., it 
       replaces it with a null.  This code requires the end of the input array 
       be the Null character.
					                               
       INPUTS:         input array of numbers.

	    OUTPUTS:        None.
                       
       RETURNS:        output array of numbers.
		           
                               
      AUTHOR:         Abhishek Verma.
      MODIFIED BY:     None.
*************************************************************************************/
void returnNumdot(pCHAR input)
{
   INT cnt ;
   cnt = strlen(input) -1;
   while( cnt > 0 )
   {
		if((!isnumdot(input[cnt])))
		{
			input[cnt] = '\0';
		}
		cnt--;
   }
} /* end of returnNumdot*/


/*************************************************************************************
       NAME:           returnNumdotMGRP()

       DESCRIPTION:    This module takes the character array.  It compares it
       against a number array.  When it enconters the first non number character or ., it 
       replaces it with a null.  This code requires the end of the input array 
       be the Null character.
					                               
       INPUTS:         input array of numbers.

	    OUTPUTS:        None.
                       
       RETURNS:        output array of numbers.
		           
                               
      AUTHOR:         Abhishek Verma.
      MODIFIED BY:     None.
*************************************************************************************/
void returnNumdotMGRP(pCHAR input)
{
   INT cnt ;
   int count = 0;
   cnt = strlen(input);
   BYTE temp_str[13]="";
   strcpy(temp_str,input);
   while( count < cnt)
   {
		if((!isnumdot(input[count])))
		{
			input[count] = '\0';
		}
		else
		{
			strcpy(input,temp_str+count);
			count = cnt;
		}
		count++;
   }
} /* end of returnNumdot*/


/*************************************************************************************
       NAME:           returnNumdotMP()

       DESCRIPTION:    This module takes the character array.  It compares it
       against a number array.  When it enconters the first non number character or ., it 
       replaces it with a null.  This code requires the end of the input array 
       be the Null character.
					                               
       INPUTS:         input array of numbers.

	    OUTPUTS:        None.
                       
       RETURNS:        output array of numbers.
		           
                               
      AUTHOR:         Abhishek Verma.
      MODIFIED BY:     None.
*************************************************************************************/
void returnNumdotMP(pCHAR input)
{
   INT cnt ;
   int count = 0;
   cnt = strlen(input);
   BYTE temp_str[13]="";
   strcpy(temp_str,input);
   while( count < cnt)
   {
		if((!isnumdot(input[count])))
		{
			input[count] = '\0';
		}
		else
		{
			strcpy(input,temp_str+count);
			count = cnt;
		}
		count++;
   }
} /* end of returnNumdot*/

/*************************************************************************************
       NAME:           returnNumMP()

       DESCRIPTION:    This module takes the character array.  It compares it
       against a number array.  When it enconters the first non number character , it 
       replaces it with a null.  This code requires the end of the input array 
       be the Null character.
					                               
       INPUTS:         input array of numbers.

	    OUTPUTS:        None.
                       
       RETURNS:        output array of numbers.
		           
                               
      AUTHOR:         Abhishek Verma.
      MODIFIED BY:     None.
*************************************************************************************/
void returnNumMP(pCHAR input)
{
   INT cnt ;
   int count = 0;
   cnt = strlen(input);
   BYTE temp_str[3]= {0};
   strcpy(temp_str,input);
   while( count < cnt)
   {
		if((!isdigit(input[count])))
		{
			input[count] = '\0';
		}
		else
		{
			strcpy(input,temp_str+count);
			count = cnt;
		}
		count++;
   }
} /* end of returnNumdot*/


/*************************************************************************************
       NAME:           returnNumAscii()

       DESCRIPTION:    This module takes the character array.  It compares it
       against a number array.  When it enconters the first non number character or ascii, it 
       replaces it with a null.  This code requires the end of the input array 
       be the Null character.
					                               
       INPUTS:         input array of numbers.

	    OUTPUTS:        None.
                       
       RETURNS:        output array of numbers.
		           
                               
      AUTHOR:         Abhishek Verma.
      MODIFIED BY:     None.
*************************************************************************************/
void returnNumAscii(pCHAR input)
{
   INT cnt ;
   cnt = strlen(input) -1;
   while( cnt > 0 )
   {
		if((!isalnum(input[cnt])))
		{
			input[cnt] = '\0';
		}
		cnt--;
   }
} /* end of returnNumdot*/


/*************************************************************************************
       NAME:           processMerchantImportCommand()

       DESCRIPTION:    This module processes the merchant information.
                       1)  It reads the the merchant file.
                       2)  It parses the Header,Detail,and the Trailer record.
                       3)  It maps the merchant information to the merchant table.
                       4)  It does the update/insert/select calls to the data server.
                       
       INPUTS:         filename is the name of the merchant file.
                       
                       choice could take on the value of 1 or 0.  If it is 0 it will
                       only read the file and determines if the file format is good.
                       If it is 1 it will make the calls to the data server with the
                       proper information.
                       
	    OUTPUTS:        If this value is POPULATE_DATA_SERVER, the record is good.  
                       The record could be used to make update/insert/select calls to 
                       the data server. If this value is NOT_POPULATE_DATA_SERVER, the 
                       record is not good.  The code prints an error message and go to the next
                       record.
  
       RETURNS:        
		   None.                       
                               
      AUTHOR:          Faramarz Arad.
      MODIFIED BY:     None.
*************************************************************************************/
INT processMerchantImportCommand (pCHAR filename,INT choice)
{
   BYTE  ret;

   CHAR dataRecord[merchantRecordLength + 10];

   myMerchantHeader  merchHeadRecord;
   myMerchantDetail  merchDetailRecord;
   myMerchantTrailer merchTrailerRecord;
   
   INT EndProcessImport = 0,recCnt = 0,returnValue=0;
   INT headerExist=0,trailerExist=0,recNumber=0;
   
   MCF01 mcf01;
   MCF01 mcf01_mpav;
   CHAR  temp_str[100]={0};
   CHAR  tempMsg [100]={0};
   CHAR  time_date[25] = "";

   double   start_time;
   double   end_time;
   double   duration;


   Import_count.readCount1 = Import_count.skipCount1 = Import_count.errorCount1 =0;
   Import_count.insertCount = Import_count.updateCount = 0;

   if ( NULL == (fptr = fopen(filename,"r")))
   {
      sprintf(tempMsg,"IMPORT: file %s was not opened.",filename);
      fputs(tempMsg,dumpPtr);
      exit_import(0);
   }

   ptetime_get_timestamp( time_date );
   sprintf(tempMsg,"%s: ",time_date);
   fputs(tempMsg,HstMcF01Ptr);
   fputs("Started Import utlity for Merch file update\n",HstMcF01Ptr);

   memset (&mcf01_mpav, 0x00, sizeof(mcf01_mpav)) ;
   /*This is the main loop.  Read a record at a time to parse.*/
   while(!EndProcessImport)
   {
      /*read a record.  If there is any problem Quit.            */
      if (NULL == fgets(dataRecord,merchantRecordLength+1,fptr))
      {
         EndProcessImport = 1;
         break;
      }

	  Import_count.readCount1++;

      /*read the detail record in the file.*/
      if (dataRecord[0] == 'D')
      {
         parseMerchantDetail(dataRecord, &merchDetailRecord);
         
         recCnt++;
         
         /*************populate the mcf01 table.************************/
         populateMcf01Table(&merchDetailRecord, &mcf01);

         /*added by farad on 2/23/1999 to fix bug #2.  Check to see if */
         /*there is a required field which is blank (all spaces or NULL).*/
         if(mcf01HasBlankReqField(&mcf01, &merchDetailRecord.merchCardType1) == CORRECT)
         {
			Import_count.errorCount1++;
            errorMsgBlankField(merchant_record, (pCHAR)mcf01.primary_key.merchant_id);
            continue;
         }

         if ( DB_Timing_Flag == true )
            start_time = ptetime_get_time();

         /*let us retrive Preauth validation flag details before updating it*/
         memcpy(mcf01_mpav.primary_key.merchant_id,     mcf01.primary_key.merchant_id,(sizeof(mcf01.primary_key.merchant_id)-1));
         memcpy(mcf01_mpav.primary_key.organization_id, mcf01.primary_key.merchant_id,(sizeof(mcf01.primary_key.organization_id)-1));
         ret = db_select_mcf01_MPAV(&mcf01_mpav, temp_str);
         if(ret == PTEMSG_OK)
		 {
        	 log_the_updated_fields_of_mcf01(&mcf01_mpav,&mcf01);
			 strcpy (mcf01.network[7].priority_routing   ,mcf01_mpav.network[7].priority_routing);
			 strcpy (mcf01.division_nbr   ,mcf01_mpav.division_nbr);
		 }
         /* Update the record.  If it fails due to not found, insert it.
          * Logically, the code could be structured better here,
          * but since speed is critical, and 98-99% of the records
          * are updates, leave the code structured this way.
          */
		 if (DB_NullCheck_Flag==1)
		 {
			CheckNULLTerminatedinMCF01 (&mcf01,MODE_UPDATE);
		 }
         ret = db_update_mcf01(&mcf01, temp_str);

         if ( DB_Timing_Flag == true )
         {
            end_time = ptetime_get_time();
            duration = end_time - start_time;
            update_timing_stats( ST1_DB_UPDATE, ST2_NONE, MCF01_DATA,
                                 (float)duration, &TimingStats );
         }

         if (PTEMSG_OK != ret)
         {
            if (PTEMSG_NOT_FOUND == ret)
            {
               if ( DB_Timing_Flag == true )
                  start_time = ptetime_get_time();

               if (DB_NullCheck_Flag==1)
               {
      			  CheckNULLTerminatedinMCF01 (&mcf01,MODE_INSERT);
               }
               ret = db_insert_mcf01(&mcf01, temp_str);

               if ( DB_Timing_Flag == true )
               {
                  end_time = ptetime_get_time();
                  duration = end_time - start_time;
                  update_timing_stats( ST1_DB_GET_COUNT, ST2_NONE, BCH10_DATA,
                                       (float)duration, &TimingStats );
               }
            }
            if (PTEMSG_OK != ret)
			{
			   Import_count.errorCount1++;
               errorMsgNotInsert(merchant_record, (pCHAR)mcf01.primary_key.merchant_id, temp_str);
			}
			else
				Import_count.insertCount++;
         }
		 else
			 Import_count.updateCount++;
         
         /*
          * memset the card type declared globally for logging
          * */
         memset(cardtype_details,0x00,sizeof(cardtype_details));
         /***************process the MCT01 table.**************************/
         if(TRUE == clearMCT01Table(&merchDetailRecord , &merchDetailRecord.merchCardType1))
		  if (TRUE  == processMCT01Table(&merchDetailRecord, &merchDetailRecord.merchCardType1))
		   if (TRUE  == processMCT01Table(&merchDetailRecord, &merchDetailRecord.merchCardType2))
			if (TRUE ==  processMCT01Table(&merchDetailRecord, &merchDetailRecord.merchCardType3))
			 if (TRUE == processMCT01Table(&merchDetailRecord, &merchDetailRecord.merchCardType4))
			  if (TRUE == processMCT01Table(&merchDetailRecord, &merchDetailRecord.merchCardType5))
			   if (TRUE == processMCT01Table(&merchDetailRecord, &merchDetailRecord.merchCardType6))
				if (TRUE == processMCT01Table(&merchDetailRecord, &merchDetailRecord.merchCardType7))
				 if (TRUE == processMCT01Table(&merchDetailRecord, &merchDetailRecord.merchCardType8))
				  if (TRUE == processMCT01Table(&merchDetailRecord, &merchDetailRecord.merchCardType9))
				   if (TRUE == processMCT01Table(&merchDetailRecord, &merchDetailRecord.merchCardType10))
					if (TRUE == processMCT01Table(&merchDetailRecord, &merchDetailRecord.merchCardType11))
					 if (TRUE == processMCT01Table(&merchDetailRecord, &merchDetailRecord.merchCardType12))
					  if (TRUE == processMCT01Table(&merchDetailRecord, &merchDetailRecord.merchCardType13))
					   if (TRUE == processMCT01Table(&merchDetailRecord, &merchDetailRecord.merchCardType14))
						if (TRUE == processMCT01Table(&merchDetailRecord, &merchDetailRecord.merchCardType15))
						 if (TRUE == processMCT01Table(&merchDetailRecord, &merchDetailRecord.merchCardType16))
						  if (TRUE == processMCT01Table(&merchDetailRecord, &merchDetailRecord.merchCardType17))
						   if (TRUE == processMCT01Table(&merchDetailRecord, &merchDetailRecord.merchCardType18))
							if (TRUE == processMCT01Table(&merchDetailRecord, &merchDetailRecord.merchCardType19))
							 if(TRUE == processMCT01Table(&merchDetailRecord, &merchDetailRecord.merchCardType20))
							  if(TRUE == processMCT01Table(&merchDetailRecord, &merchDetailRecord.merchCardType21))
							   if(TRUE == processMCT01Table(&merchDetailRecord, &merchDetailRecord.merchCardType22))
							    if(TRUE == processMCT01Table(&merchDetailRecord, &merchDetailRecord.merchCardType23))
								 if(TRUE == processMCT01Table(&merchDetailRecord, &merchDetailRecord.merchCardType24))
								  if(TRUE == processMCT01Table(&merchDetailRecord, &merchDetailRecord.merchCardType25))
								   if(TRUE == processMCT01Table(&merchDetailRecord, &merchDetailRecord.merchCardType26))
									if(TRUE == processMCT01Table(&merchDetailRecord, &merchDetailRecord.merchCardType27))
									 if(TRUE == processMCT01Table(&merchDetailRecord, &merchDetailRecord.merchCardType28))
									  if(TRUE == processMCT01Table(&merchDetailRecord, &merchDetailRecord.merchCardType29))
									   if(TRUE == processMCT01Table(&merchDetailRecord, &merchDetailRecord.merchCardType30))
										if(TRUE == processMCT01Table(&merchDetailRecord, &merchDetailRecord.merchCardType31))
										 if(TRUE == processMCT01Table(&merchDetailRecord, &merchDetailRecord.merchCardType32))
										  if(TRUE == processMCT01Table(&merchDetailRecord, &merchDetailRecord.merchCardType33))
										   if(TRUE == processMCT01Table(&merchDetailRecord, &merchDetailRecord.merchCardType34))
										    if(TRUE == processMCT01Table(&merchDetailRecord, &merchDetailRecord.merchCardType35))
										     if(TRUE == processMCT01Table(&merchDetailRecord, &merchDetailRecord.merchCardType36))
											  if(TRUE == processMCT01Table(&merchDetailRecord, &merchDetailRecord.merchCardType37))
											   if(TRUE == processMCT01Table(&merchDetailRecord, &merchDetailRecord.merchCardType38))
												if(TRUE == processMCT01Table(&merchDetailRecord, &merchDetailRecord.merchCardType39))
												 processMCT01Table(&merchDetailRecord, &merchDetailRecord.merchCardType40);

/*
 *  add time stamp and copy the all the card types stored in global variable.
 *  memset the global variable .
 * */
     	ptetime_get_timestamp( time_date );
     	sprintf(tempMsg,"%s: ",time_date);
     	fputs(tempMsg,HstMcF01Ptr);
        fputs(cardtype_details,HstMcF01Ptr);
        fputs("\n",HstMcF01Ptr);
         /***********process the DEF01 table.******************************/
         /*This looks pretty bad to have to repeat this function call 6 times,*/
         /*but I have no choice.*/
         if (TRUE == processDEF01Table(&merchDetailRecord, merchDetailRecord.merchDefPayTerm1,
             merchDetailRecord.merchDefFactor1, merchDetailRecord.merchDefMinAmount1))
			   if (TRUE == processDEF01Table(&merchDetailRecord, merchDetailRecord.merchDefPayTerm2,
				   merchDetailRecord.merchDefFactor2, merchDetailRecord.merchDefMinAmount2))
				   if (TRUE == processDEF01Table(&merchDetailRecord, merchDetailRecord.merchDefPayTerm3,
					   merchDetailRecord.merchDefFactor3, merchDetailRecord.merchDefMinAmount3))
					   if (TRUE == processDEF01Table(&merchDetailRecord, merchDetailRecord.merchDefPayTerm4,
						   merchDetailRecord.merchDefFactor4, merchDetailRecord.merchDefMinAmount4))
						   if (TRUE == processDEF01Table(&merchDetailRecord, merchDetailRecord.merchDefPayTerm5,
							   merchDetailRecord.merchDefFactor5, merchDetailRecord.merchDefMinAmount5))
						      processDEF01Table(&merchDetailRecord, merchDetailRecord.merchDefPayTerm6,
								   merchDetailRecord.merchDefFactor6, merchDetailRecord.merchDefMinAmount6);

      }  /*if (dataRecord[0]*/
      else if (dataRecord[0] == 'H')
      {
         /*read the header record in the file.*/
		   parseMerchantHeader(dataRecord, &merchHeadRecord);
         recCnt++;      /*added by farad on 2/18/1999 for bug #1*/
         headerExist = 1;         
      }
      else if (dataRecord[0] == 'T')
      {
		   /*read the trailer record in the file. */
			parseMerchantTrailer(dataRecord, &merchTrailerRecord);
         trailerExist = 1;
         recCnt++;      /*added by farad on 2/18/1999 for bug #1*/
         
         /*the next line will make sure that the file name in the trailer along with*/
         /*number of the records in the trailer record would match.*/
         if ( (recCnt == atoi(merchTrailerRecord.trailNumberofRecord)) &&
              (strcmp("MRCHFILE",merchTrailerRecord.trailFileName) == 0))
            recNumber = 1;
         break;
      }

	   /* Commit every 100 records. */
      if (0 == (recCnt % 100))
      {
	      dbcommon_commit ();
      }

      /* Log every 250 records to show progress */
      if (0 == (recCnt % 250))
      {
         sprintf(tempMsg,"IMPORT: Processed %ld %s",recCnt, merchant_record);
         LogMsg(tempMsg, INFO_MSG);
      }
   }
   
   fclose(HstMcF01Ptr);
   /* Catch All */
	dbcommon_commit ();

   fclose(fptr);   

   if( (headerExist == 1) && (trailerExist == 1) && (recNumber == 1) )
   {
      EndProcessImport = 0;
      returnValue = POPULATE_DATA_SERVER;
   }
   else
      {
		ptetime_get_timestamp( time_date );   
		sprintf(tempMsg," %s >> IMPORT: Number of records, header record, or trailer record error occurred\n",time_date);
		fputs(tempMsg,dumpPtr);
	}      

		sprintf(tempMsg,"IMPORT: Processed %ld %s",recCnt, merchant_records);
        LogMsg(tempMsg, INFO_MSG);

	/* Log Merchant count records */
	
	LogCounts("MERCHANT",0);

   return(returnValue);
}  /*end of processMerchantImportCommand*/

/*************************************************************************************
       NAME:           processMultipleMerchantImportCommand()

       DESCRIPTION:    This module processes the merchant information.
                       1)  It reads the the merchant file.
                       2)  It parses only those records that are needed 
					       to be inserted or updated of the Header,Detail,and 
						   the Trailer records.
                       3)  It maps the merchant information to the merchant table.
                       4)  It does the update/insert/select calls to the data server.
                       
       INPUTS:         filename is the name of the merchant file.
                       
                       choice could take on the value of 1 or 0.  If it is 0 it will
                       only read the file and determines if the file is in proper
					   format .If it is 1 it will make the calls to the 
					   data server with the proper information.
                       
	    OUTPUTS:        If this value is POPULATE_DATA_SERVER, the record is the record is in
						proper format.The record could be used to make update/insert/select calls to 
                        the data server. If this value is NOT_POPULATE_DATA_SERVER, the 
                        record is notin proper format.The code prints an error message and go to the next
                        record.
  
       RETURNS:        
		   None.                       
                               
      AUTHOR:          Sailaja Yarlagadda.
      MODIFIED BY:     



***********************************************************************************/
INT processMultipleMerchantImportCommand (pCHAR filename,pCHAR filename1,INT choice)
{
	 BYTE  ret;

   CHAR dataRecord[merchantRecordLength + 10];
   CHAR dataRecord1[merchantRecordLength + 10];
   CHAR olddataRecord[merchantRecordLength + 10];
   
   INT EndProcessImport = 0,EndProcessImport1 = 0,returnValue = 0;
   INT recCnt = 0;

   double   start_time;
   double   end_time;
   double   duration;

   
   myMerchantDetail  merchDetailRecord;

   
   INT headerExist=0,trailerExist=0,recNumber=0;
   
   MCF01 mcf01;
   CHAR  temp_str[100];
   CHAR  tempMsg [100];
   CHAR  count[7];
   INT i=0;
   CHAR time_date[25] = "";
   CHAR  merchid[16],merchid1[16];
   INT  updateinsertflag;

   Import_count.readCount1 = Import_count.readCount2 = Import_count.skipCount1 = Import_count.errorCount1 = 0;
   Import_count.insertCount = Import_count.updateCount = 0;
   Import_count.matchCount = 0;

   if ( NULL == (fptr = fopen(filename,"r")))
   {
      sprintf(tempMsg,"IMPORT: file %s was not opened.",filename);
      fputs(tempMsg,dumpPtr);
      exit_import(0);
   }

   /*This is the main loop.  Read a record at a time to parse.*/
   while(!EndProcessImport)
   {
      /*read a record.  If there is any problem Quit.            */
      if (NULL == fgets(dataRecord,merchantRecordLength+1,fptr))
      {
         EndProcessImport = 1;
         break;
      }
	  Import_count.readCount1++;

	  if (dataRecord[0] == 'H')     
			{        
		
				recCnt++;         
				headerExist = 1;
			} 
			else if(dataRecord[0] == 'D')
			{
				strncpy(merchid,&dataRecord[3],15);
				merchid[15]='\0';
				break;
			}
	}

    if ( NULL == (fptr1 = fopen(filename1,"r")))
	{
		sprintf(tempMsg,"IMPORT: file1 %s was not opened.",filename1);
		fputs(tempMsg,dumpPtr);
		exit_import(0);
	}

	/*This is the main loop.  Read a record at a time to parse.*/
	while(!EndProcessImport1)
	{
		/*read a record.  If there is any problem Quit.            */
		if (NULL == fgets(dataRecord1,merchantRecordLength+1,fptr1))
		{
			 EndProcessImport1 = 1;
			break;
		}
		Import_count.readCount2++;

		if (dataRecord1[0] == 'H')     
		{        		
			     
				headerExist = 1;
		} 
		else if(dataRecord1[0] == 'D')
		{
				strncpy(merchid1,&dataRecord1[3],15);
				merchid1[15]='\0';
				break;
		}
	}

    /*This is the main loop.  Read a record at a time to parse.*/
	while((!EndProcessImport)&&(!EndProcessImport1))
	{   
		    strcpy(olddataRecord,dataRecord);
			updateinsertflag=0;
			if(dataRecord[0]=='T')
			{
				break;
			}

			      /*read the detail record in the file.*/
			if ((dataRecord[0] == 'D') ||(dataRecord1[0] == 'D'))
			{
				
              	/*check whether the cardnumbers are equal or greater or less*/
				if(0==strcmp(merchid,merchid1))
				{
					if(0!=strcmp(dataRecord,dataRecord1))
					{
						updateinsertflag=1;
						
					}
					else
						Import_count.matchCount++;
					

					if (NULL == fgets(dataRecord,merchantRecordLength+1,fptr))
					{
						EndProcessImport = 1;
						break;
					}
					else 
					{  	
						Import_count.readCount1++;
						recCnt++;
						strncpy(merchid,&dataRecord[3],15);
		   				merchid[15]='\0';
		   			}
			 
					if (NULL == fgets(dataRecord1,merchantRecordLength+1,fptr1))
					{
						EndProcessImport1 = 1;
						break;
					}
					else 
					{ 					
						Import_count.readCount2++;
						strncpy(merchid1,&dataRecord1[3],15);
		   				merchid1[15]='\0';
		   			}			
				}

				else if(0>strcmp(merchid1,merchid))
				{
					if(NULL ==fgets(dataRecord1,merchantRecordLength+1,fptr1))
					{
						EndProcessImport1 = 1;
						break;			
					}				
					else 
					{
						Import_count.readCount2++;
						strncpy(merchid1,&dataRecord1[3],15);
		   				merchid1[15]='\0';			
		   			}		
				}
				 else if(0<strcmp(merchid1,merchid))
				{ 
					 updateinsertflag=1;
					
				
					if(NULL ==fgets(dataRecord,merchantRecordLength+1,fptr))
					{ 
						EndProcessImport = 1;
						break;			
					}				
					else 
					{
						Import_count.readCount1++;
						recCnt++;
						strncpy(merchid,&dataRecord[3],15);
		   				merchid[15]='\0';
		   			}   
				
				}
                 if(updateinsertflag==1)
				{
					parseMerchantDetail(olddataRecord, &merchDetailRecord);
				
         
					/*************populate the mcf01 table.************************/
					populateMcf01Table(&merchDetailRecord, &mcf01);
           
					/*Check to see if */
					/*there is a required field which is blank (all spaces or NULL).*/
					if(mcf01HasBlankReqField(&mcf01, &merchDetailRecord.merchCardType1) == CORRECT)
					{
						Import_count.errorCount1++;
						errorMsgBlankField(merchant_record, (pCHAR)mcf01.primary_key.merchant_id);
						continue;
					}

					if ( DB_Timing_Flag == true )
						start_time = ptetime_get_time();

					if (DB_NullCheck_Flag==1)
					{
						CheckNULLTerminatedinMCF01 (&mcf01,MODE_UPDATE);
					}

					ret = db_update_mcf01(&mcf01, temp_str);

					if ( DB_Timing_Flag == true )
					{
						end_time = ptetime_get_time();
						duration = end_time - start_time;
						update_timing_stats( ST1_DB_UPDATE, ST2_NONE, MCF01_DATA,
                                       (float)duration, &TimingStats );
					}

					if (PTEMSG_OK != ret)
					{
						if (PTEMSG_NOT_FOUND == ret)
						{
							 if ( DB_Timing_Flag == true )
								start_time = ptetime_get_time();

							 if (DB_NullCheck_Flag==1)
							 {
								CheckNULLTerminatedinMCF01 (&mcf01,MODE_INSERT);
							 }
							 ret = db_insert_mcf01(&mcf01, temp_str);

							 if ( DB_Timing_Flag == true )
							 {
								end_time = ptetime_get_time();
								duration = end_time - start_time;
								update_timing_stats( ST1_DB_INSERT, ST2_NONE, MCF01_DATA,
													 (float)duration, &TimingStats );
							 }
						}

                  if (PTEMSG_OK != ret)
                  {
                     Import_count.errorCount1++;
                     errorMsgNotInsert(merchant_record, (pCHAR)mcf01.primary_key.merchant_id, temp_str);
                  }
                  else 
                     Import_count.insertCount++;
					}
					else 
						Import_count.updateCount++;
				
					/***************process the MCT01 table.**************************/
			if(TRUE == clearMCT01Table(&merchDetailRecord , &merchDetailRecord.merchCardType1))
				if (TRUE  == processMCT01Table(&merchDetailRecord, &merchDetailRecord.merchCardType1))
				 if (TRUE  == processMCT01Table(&merchDetailRecord, &merchDetailRecord.merchCardType2))
				   if (TRUE ==  processMCT01Table(&merchDetailRecord, &merchDetailRecord.merchCardType3))
					   if (TRUE == processMCT01Table(&merchDetailRecord, &merchDetailRecord.merchCardType4))
						   if (TRUE == processMCT01Table(&merchDetailRecord, &merchDetailRecord.merchCardType5))
							   if (TRUE == processMCT01Table(&merchDetailRecord, &merchDetailRecord.merchCardType6))
								   if (TRUE == processMCT01Table(&merchDetailRecord, &merchDetailRecord.merchCardType7))
									   if (TRUE == processMCT01Table(&merchDetailRecord, &merchDetailRecord.merchCardType8))
										   if (TRUE == processMCT01Table(&merchDetailRecord, &merchDetailRecord.merchCardType9))
											   if (TRUE == processMCT01Table(&merchDetailRecord, &merchDetailRecord.merchCardType10))
												   if (TRUE == processMCT01Table(&merchDetailRecord, &merchDetailRecord.merchCardType11))
													   if (TRUE == processMCT01Table(&merchDetailRecord, &merchDetailRecord.merchCardType12))
														   if (TRUE == processMCT01Table(&merchDetailRecord, &merchDetailRecord.merchCardType13))
															   if (TRUE == processMCT01Table(&merchDetailRecord, &merchDetailRecord.merchCardType14))
																   if(TRUE == processMCT01Table(&merchDetailRecord, &merchDetailRecord.merchCardType15))

					/***********process the DEF01 table.******************************/
         
				if (TRUE == processDEF01Table(&merchDetailRecord, merchDetailRecord.merchDefPayTerm1,
					merchDetailRecord.merchDefFactor1, merchDetailRecord.merchDefMinAmount1))
			   if (TRUE == processDEF01Table(&merchDetailRecord, merchDetailRecord.merchDefPayTerm2,
				   merchDetailRecord.merchDefFactor2, merchDetailRecord.merchDefMinAmount2))
				   if (TRUE == processDEF01Table(&merchDetailRecord, merchDetailRecord.merchDefPayTerm3,
					   merchDetailRecord.merchDefFactor3, merchDetailRecord.merchDefMinAmount3))
					   if (TRUE == processDEF01Table(&merchDetailRecord, merchDetailRecord.merchDefPayTerm4,
						   merchDetailRecord.merchDefFactor4, merchDetailRecord.merchDefMinAmount4))
						   if (TRUE == processDEF01Table(&merchDetailRecord, merchDetailRecord.merchDefPayTerm5,
							   merchDetailRecord.merchDefFactor5, merchDetailRecord.merchDefMinAmount5))
						      processDEF01Table(&merchDetailRecord, merchDetailRecord.merchDefPayTerm6,
								   merchDetailRecord.merchDefFactor6, merchDetailRecord.merchDefMinAmount6);

			  
				 
				}/*End of updateinsertflag if*/	 
				
				
			}  /*end of if dataRecord*/
			         
         

		   /* Commit every 100 records. */
		if (0 == (recCnt % 100))
		{
	      dbcommon_commit ();
		}

		/* Log every 250 records to show progress */
		if (0 == (recCnt % 250))
		{
         sprintf(tempMsg,"IMPORT: Processed %ld %s",recCnt, merchant_record);
         LogMsg(tempMsg, INFO_MSG);
		}
   
			  

      }  /*end of while*/
      
       if (dataRecord[0] == 'T')
      {
		   /*read the trailer record in the file. */
		
		  strncpy(count,&dataRecord[17],6);
		   count[6]='\0';
         trailerExist = 1;
         recCnt++;      /*added by farad on 2/18/1999 for bug #1*/
         
          if  (recCnt == atoi(count))
            recNumber = 1;
         
      }

	  
   
   /* Catch All */
	dbcommon_commit ();

   fclose(fptr1);   

   if( (headerExist == 1) && (trailerExist == 1) && (recNumber == 1) )
   {
      EndProcessImport = 0;
      returnValue = POPULATE_DATA_SERVER;
   }
   else
   {
	ptetime_get_timestamp( time_date );   
	sprintf(tempMsg," %s >> IMPORT: Number of records, header record, or trailer record error occurred\n",time_date);
    fputs(tempMsg,dumpPtr);
   }
      
   sprintf(tempMsg,"IMPORT: Processed %ld %s",recCnt, merchant_records);
   LogMsg(tempMsg, INFO_MSG);

   /* Log counts for import */
   LogCounts("MERCHANT",1);

   return(returnValue);

}/*end of processMultipleMerchantImportCommand*/

/*************************************************************************************
       NAME:           processNegFraudImportCommand()

       DESCRIPTION:    This module processes the Negative Fraud Records
                       1)  It reads the the negative fraud import file defined in the filename argument..
                       2)  It parses the Header and detail record
                       3)  It maps the record data to the negative_fraud table.
                       4)  It does the update/insert/select calls to the data server.
                       
       INPUTS:         filename is the name of the primary file.
                       
                       choice could take on the value of 1 or 0.  If it is 0 it will
                       only read the file and determines if the file format is good.
                       If it is 1 it will make the calls to the data server with the
                       proper information.
                       
	    OUTPUTS:        If this value is POPULATE_DATA_SERVER, the record is good.  
                       The record could be used to make update/insert/select calls to 
                       the data server. If this value is NOT_POPULATE_DATA_SERVER, the 
                       record is not good.  The code prints an error message and go to the next
                       record.
                       
                       
       RETURNS:        
		   None.                       
                               
      AUTHOR:          L Mitchell 5/21/04
      MODIFIED BY:     
*************************************************************************************/
INT processNegFraudImportCommand (pCHAR filename,INT choice)
{
   CHAR dataRecord[negativeFraudRecordLength + 10];
   myNegFraudHeader  NegFraudHeaderRecord;
   myNegFraudTrailer NegFraudTrailerRecord;
   INT EndProcessImport = 0;
   INT returnValue = 0;
   long recCnt = 0;
   INT headerExist = 0, trailerExist = 0, recNumber = 0;
   NEGATIVE_FRAUD neg_fraud;
   CHAR  temp_str[250];
   BYTE  ret;
   CHAR  tempMsg[100];
   CHAR  time_date[25] = "";

   double   start_time;
   double   end_time;
   double   duration;
   
   Import_count.readCount1 = Import_count.skipCount1 = Import_count.errorCount1 = 0;
   Import_count.insertCount = Import_count.updateCount = 0;

   if (NULL == (fptr = fopen(filename,"r")))
   {
      sprintf(tempMsg,"IMPORT: Could not open file %s",filename);
	  fputs(tempMsg, dumpPtr);
      exit_import(0);
   }

   /*This is the main loop.  Read a record at a time to parse.*/
   while(!EndProcessImport)
   {
      /*read a record if there is any problem quit.*/
      if (NULL == fgets(dataRecord,negativeFraudRecordLength+1,fptr))
      {
         EndProcessImport = 1;
         break;
      }

	  Import_count.readCount1++;
      
      /*read a detail record of the file and process.*/
      if (dataRecord[0] == 'D')
      {
         parseNegFraudDetail(dataRecord, &neg_fraud);
         recCnt++;
         
         /*check to see if there are any required blank fields in neg_fraud.*/
         if(negfraudHasBlankReqField(&neg_fraud) == CORRECT)
         {
			 Import_count.errorCount1++;
             errorMsgBlankField(account_record, neg_fraud.primary_key.mcc); 
             continue;
         }
         
         if ( DB_Timing_Flag == true )
            start_time = ptetime_get_time();

         /*Update the record.  If it fails due to not found, insert it.*/
         /*Since most records will be updates, this will speed up imports 
           instead of selecting the record first*/
		 if (DB_NullCheck_Flag==1)
		 {
			CheckNULLTerminatedinNEGATIVE_FRAUD (&neg_fraud,MODE_UPDATE);
		 }
		 ret = db_update_neg_fraud(&neg_fraud, temp_str);

        if ( DB_Timing_Flag == true )
        {
           end_time = ptetime_get_time();
           duration = end_time - start_time;
           update_timing_stats( ST1_DB_UPDATE, ST2_NONE, NEGATIVE_FRAUD_DATA,
                                (float)duration, &TimingStats );
        }

	      if( ret !=PTEMSG_OK )
		  {
			  if ( ret == PTEMSG_NOT_FOUND)
              {
				  if ( DB_Timing_Flag == true )
					  start_time = ptetime_get_time();

				 if (DB_NullCheck_Flag==1)
				 {
					CheckNULLTerminatedinNEGATIVE_FRAUD (&neg_fraud,MODE_UPDATE);
				 }
			  	  ret = db_insert_neg_fraud(&neg_fraud, temp_str);

				  if ( DB_Timing_Flag == true )
				  {
					 end_time = ptetime_get_time();
					 duration = end_time - start_time;
					 update_timing_stats( ST1_DB_INSERT, ST2_NONE, NEGATIVE_FRAUD_DATA,
										  (float)duration, &TimingStats );
				  }
              }
			  if (ret != PTEMSG_OK)
			  {				
				  Import_count.errorCount1++;
				  errorMsgNotInsert(negative_fraud_record, neg_fraud.primary_key.bin_low, temp_str);
			  }
			  else
				  Import_count.insertCount++;

		  }
		  else  /*update ok*/
			  Import_count.updateCount++;
		  
	  }
      else if (dataRecord[0] == 'H')
      {
         /*read a header record in the file.*/
		 parseNegFraudHeader(dataRecord, &NegFraudHeaderRecord);
         recCnt++;
         headerExist = 1;
      }
	   else if (dataRecord[0] == 'T')
      {
         /*read the trailer record in the file.		*/
		 parseNegFraudTrailer(dataRecord, &NegFraudTrailerRecord);
         trailerExist = 1;
         recCnt++;      /*added by farad on 2/18/1999 for bug #1*/

         /*the next line will make sure that the file name in the trailer along with*/
         /*number of the records in the trailer record would match.*/
         if ( (recCnt == atoi(NegFraudTrailerRecord.trailNumberofRecords)) &&
              (strcmp("NEGFRAUD",NegFraudTrailerRecord.trailFileName) == 0) )
            recNumber = 1;
            break;
      }
      
	   /* Commit every 100 records. */
      if (0 == (recCnt % 100))
      {
	      dbcommon_commit ();
      }

      /* Log every 250 records to show progress */
      if (0 == (recCnt % 250))
      {
         sprintf(tempMsg,"IMPORT: Processed %ld %s",recCnt, negative_fraud_record);
         LogMsg(tempMsg, INFO_MSG);
      }
   }
   
	dbcommon_commit ();

   fclose(fptr);
   if ((headerExist == 1) && (trailerExist == 1) && recNumber == 1)
   {
      EndProcessImport = 0;
      returnValue = POPULATE_DATA_SERVER;
   }
   else
   {
	   ptetime_get_timestamp( time_date );
	   sprintf(tempMsg, " %s >> IMPORT: Number of records, header record, or trailer record error occurred\n", time_date);
	   fputs(tempMsg, dumpPtr);
   }

   sprintf(tempMsg, "IMPORT: Processed %ld %s", recCnt, negative_fraud_record);
   LogMsg(tempMsg, INFO_MSG);

   /*Log Negative Fraud record count */
   LogCounts("NEGATIVE_FRAUD", 0);

   return(returnValue);
}  /*end of processNegFraudImportCommand*/


/*************************************************************************************
       NAME:           processPrimaryImportCommand()

       DESCRIPTION:    This module processes the primary information.
                       1)  It reads the the primary file.
                       2)  It parses the Header,Detail,and the Trailer record.
                       3)  It maps the primary information to the primary table.
                       4)  It does the update/insert/select calls to the data server.
                       
       INPUTS:         filename is the name of the primary file.
                       
                       choice could take on the value of 1 or 0.  If it is 0 it will
                       only read the file and determines if the file format is good.
                       If it is 1 it will make the calls to the data server with the
                       proper information.
                       
	    OUTPUTS:        If this value is POPULATE_DATA_SERVER, the record is good.  
                       The record could be used to make update/insert/select calls to 
                       the data server. If this value is NOT_POPULATE_DATA_SERVER, the 
                       record is not good.  The code prints an error message and go to the next
                       record.
                       
                       
       RETURNS:        
		   None.                       
                               
      AUTHOR:          Faramarz Arad.
      MODIFIED BY:     None.
*************************************************************************************/
INT processPrimaryImportCommand (pCHAR filename,INT choice)
{
   CHAR dataRecord[primaryRecordLength + 10];
   myPrimaryHeader  primHeadRecord;
   myPrimaryTrailer primTrailerRecord;
   INT EndProcessImport = 0, returnValue = 0;
   long recCnt = 0;
   INT headerExist = 0, trailerExist = 0, recNumber = 0;
   PAF01 paf01;
   CHAR  temp_str[250];
   BYTE  ret;
   CHAR  tempMsg[100];
   CHAR time_date[25] = "";

   double   start_time;
   double   end_time;
   double   duration;


        
   if (NULL == (fptr = fopen(filename,"r")))
   {
      sprintf(tempMsg,"IMPORT: Could not open file %s",filename);     
	  fputs(tempMsg,dumpPtr);
      exit_import(0);
   }

   /*This is the main loop.  Read a record at a time to parse.*/
   while(!EndProcessImport)
   {
      /*read a record if there is any problem quit.*/
      if (NULL == fgets(dataRecord,primaryRecordLength+1,fptr))
      {
         EndProcessImport = 1;
         break;
      }

   
      /*read a detail record of the file and process.*/
      if (dataRecord[0] == 'D')
      {
         parsePrimaryDetail(dataRecord, &paf01);
         recCnt++;
         
         /*check to see if there are any required blank fields in paf01.*/
         if(paf01HasBlankReqField(&paf01) == CORRECT)
         {
            errorMsgBlankField(primary_record, (pCHAR)paf01.primary_key.primary_acct_nbr);
            continue;
         }

         if ( DB_Timing_Flag == true )
            start_time = ptetime_get_time();

         /* Update the record.  If it fails due to not found, insert it.
          * Logically, the code could be structured better here,
          * but since speed is critical, and 98-99% of the records
          * are updates, leave the code structured this way.
          */
		 if (DB_NullCheck_Flag==1)
		 {
			CheckNULLTerminatedinPAF01 (&paf01,MODE_UPDATE);
		 }
         ret = db_update_paf01(&paf01, temp_str);

         if ( DB_Timing_Flag == true )
         {
            end_time = ptetime_get_time();
            duration = end_time - start_time;
            update_timing_stats( ST1_DB_UPDATE, ST2_NONE, PAF01_DATA,
                                 (float)duration, &TimingStats );
         }

			if (PTEMSG_OK != ret)
           {
				if(PTEMSG_NOT_FOUND != ret || ret == RESOURCE_BUSY)
				{
					errorMsgNotUpdate(primary_record, (pCHAR)paf01.primary_key.primary_acct_nbr, temp_str, ret);
				}
				else
				if( PTEMSG_NOT_FOUND == ret )
				{
					if ( DB_Timing_Flag == true )
						start_time = ptetime_get_time();

					 if (DB_NullCheck_Flag==1)
					 {
						CheckNULLTerminatedinPAF01 (&paf01,MODE_INSERT);
					 }
					ret = db_insert_paf01(&paf01, temp_str);
					if ( DB_Timing_Flag == true )
					{
						end_time = ptetime_get_time();
						duration = end_time - start_time;
						update_timing_stats( ST1_DB_INSERT, ST2_NONE, PAF01_DATA,
                                       (float)duration, &TimingStats );
					}
				}
				if(PTEMSG_OK != ret)
				{
					errorMsgNotInsert(primary_record, (pCHAR)paf01.primary_key.primary_acct_nbr, temp_str);
				}
           }
      }
	   else if (dataRecord[0] == 'H')
      {
         /*read a header record in the file.*/
		   parsePrimaryHeader(dataRecord, &primHeadRecord);
         recCnt++;         /*added by farad on 2/18/1999 for bug #1*/
         headerExist = 1;
      }
	   else if (dataRecord[0] == 'T')
      {
         /*read the trailer record in the file.		*/
		   parsePrimaryTrailer(dataRecord, &primTrailerRecord);
         trailerExist = 1;
         recCnt++;      /*added by farad on 2/18/1999 for bug #1*/

         /*the next line will make sure that the file name in the trailer along with*/
         /*number of the records in the trailer record would match.*/
         if ( (recCnt == atoi(primTrailerRecord.trailNumberofRecord)) &&
              (strcmp("PRACFILE",primTrailerRecord.trailFileName) == 0) )
            recNumber = 1;
            break;
      }
      
	   /* Commit every 100 records. */
      if (0 == (recCnt % 100))
      {
	      dbcommon_commit ();
      }

      /* Log every 250 records to show progress */
      if (0 == (recCnt % 250))
      {
         sprintf(tempMsg,"IMPORT: Processed %ld %s",recCnt, primary_record);
         LogMsg(tempMsg, INFO_MSG);
      }
   }
   
	dbcommon_commit ();

   fclose(fptr);
   if ((headerExist == 1) && (trailerExist == 1) && recNumber == 1)
   {
      EndProcessImport = 0;
      returnValue = POPULATE_DATA_SERVER;
   }
   else
      {
		ptetime_get_timestamp( time_date );
   
		sprintf(tempMsg," %s >> IMPORT: Number of records, header record, or trailer record error occurred\n",time_date);
		fputs(tempMsg,dumpPtr);
	}
   sprintf(tempMsg,"IMPORT: Processed %ld %s",recCnt, primary_record);
   LogMsg(tempMsg, INFO_MSG);


   return(returnValue);
}  /*end of processPrimaryImportCommand*/

/**************************************************************************************
NAME:           processPrimaryImportCommand()

       DESCRIPTION:    This module processes the primary information.
                       1)  It reads the the primary file.
                       2)  It parses only those records that are needed to be inserted or updated.
					       of the Header,Detail,and the Trailer records.
                       3)  It maps the primary information to the primary table.
                       4)  It does the update/insert/select calls to the data server.
                       
       INPUTS:         filename is the name of the primary file.
                       
                       choice could take on the value of 1 or 0.  If it is 0 it will
                       only read the file and determines if the file is in proper format.
                       If it is 1 it will make the calls to the data server with the
                       proper information.
                       
	    OUTPUTS:        If this value is POPULATE_DATA_SERVER, the record is in proper format.  
                       The record could be used to make update/insert/select calls to 
                       the data server. If this value is NOT_POPULATE_DATA_SERVER, the 
                       record is not in proper format.  The code prints an error message and go to the next
                       record.
                       
                       
       RETURNS: 	   None.                       
                               
      AUTHOR:          Sailaja Yarlagadda.
      MODIFIED BY:     



**********************************************************************************/
INT processMultiplePrimaryImportCommand (pCHAR filename,pCHAR filename1,INT choice)
{ 
	CHAR dataRecord[primaryRecordLength + 10];
   CHAR dataRecord1[primaryRecordLength + 10];
   CHAR olddataRecord[primaryRecordLength + 10];

   
   INT EndProcessImport = 0,EndProcessImport1 = 0,returnValue = 0;
   long recCnt = 0;
   INT headerExist = 0, trailerExist = 0, recNumber = 0;
   PAF01 paf01;
   CHAR  temp_str[250];
   BYTE  ret;
   CHAR  tempMsg[100];
   CHAR  count[7];
   CHAR time_date[25] = "";
   CHAR  accnbr[20],accnbr1[20];
   INT  updateinsertflag;

   double   start_time;
   double   end_time;
   double   duration;

   
   if (NULL == (fptr = fopen(filename,"r")))
   {
      sprintf(tempMsg,"IMPORT: Error can not open file %s.",filename);
 	  fputs(tempMsg,dumpPtr);
      exit_import(0);
   }
    while(!EndProcessImport)
	{
			if(NULL ==fgets(dataRecord,primaryRecordLength+1,fptr))
			{
				EndProcessImport = 1;
				break;
			
			}


		    if (dataRecord[0] == 'H')     
			{        
		
				recCnt++;         
				headerExist = 1;
			} 
			else if(dataRecord[0] == 'D')
			{
				strncpy(accnbr,&dataRecord[3],19);
				accnbr[19]='\0';
				break;
			}
		
	 }
      
     if (NULL == (fptr1 = fopen(filename1,"r")))
	{
      sprintf(tempMsg,"IMPORT: Error can not open file1 %s.",filename1);      
	  fputs(tempMsg,dumpPtr);
      exit_import(0);
	 }  

	  while(!EndProcessImport1)
	  {
			if(NULL ==fgets(dataRecord1,primaryRecordLength+1,fptr1))
			{
				EndProcessImport1 = 1;
				break;
			
			}


		    if (dataRecord1[0] == 'H')     
			{   		
				
				headerExist = 1;
			} 
			   else if(dataRecord1[0] == 'D')
			   {
					strncpy(accnbr1,&dataRecord1[3],19);
					accnbr1[19]='\0';				
         			break;
				}
		
	  }
	
		/*This is the main loop.  Read a record at a time to parse.*/
		while((!EndProcessImport)&&(!EndProcessImport1))
		{   
			strcpy(olddataRecord,dataRecord);
			updateinsertflag=0;
			if(dataRecord[0]=='T')
			{
				break;
			}

			/*read the detail record of the file and parse it.*/
			if ((dataRecord[0] == 'D')|| (dataRecord1[0]=='D'))
			{               
					/*check whether the accountnumbers are equal or greater or less*/
				if(0==strcmp(accnbr,accnbr1))
				{
					if(0!=strcmp(dataRecord,dataRecord1))
					{
						updateinsertflag=1;
						
					}
					  
			  
					if (NULL == fgets(dataRecord,primaryRecordLength+1,fptr))
					{
						EndProcessImport = 1;
						break;
					}
					else 
					{  	
						recCnt++;
						strncpy(accnbr,&dataRecord[3],19);
		   				accnbr[19]='\0';
		   			}
			 
					if (NULL == fgets(dataRecord1,primaryRecordLength+1,fptr1))
					{
						EndProcessImport1 = 1;
						break;
					}
					else 
					{ 					
						strncpy(accnbr1,&dataRecord1[3],19);
		   				accnbr1[19]='\0';
		   			}			
				}

				else if(0>strcmp(accnbr1,accnbr))
				{
					if(NULL ==fgets(dataRecord1,primaryRecordLength+1,fptr1))
					{
						EndProcessImport1 = 1;
						break;			
					}				
					else 
					{
						strncpy(accnbr1,&dataRecord1[3],19);
		   				accnbr1[19]='\0';			
		   			}		
				}
				 else if(0<strcmp(accnbr1,accnbr))
				 { 	 
					 updateinsertflag=1;
									
					if(NULL ==fgets(dataRecord,primaryRecordLength+1,fptr))
					{
						EndProcessImport = 1;
						break;			
					}				
					else 
					{	
						recCnt++;
						strncpy(accnbr,&dataRecord[3],19);
		   				accnbr[19]='\0';
		   			}   
				
				}
				
                if(updateinsertflag==1)
				{
					parsePrimaryDetail(olddataRecord, &paf01);
					
         
					/* Check to see if */
					/*there is a required field which is blank (all spaces or NULL).*/
					if(paf01HasBlankReqField(&paf01) == CORRECT)
					{
						errorMsgBlankField(primary_record, (pCHAR)paf01.primary_key.primary_acct_nbr);
						continue;
					}         

					if ( DB_Timing_Flag == true )
						start_time = ptetime_get_time();

					 if (DB_NullCheck_Flag==1)
					 {
						CheckNULLTerminatedinPAF01 (&paf01,MODE_UPDATE);
					 }
					 ret = db_update_paf01(&paf01, temp_str);

					if ( DB_Timing_Flag == true )
					{
						end_time = ptetime_get_time();
						duration = end_time - start_time;
						update_timing_stats( ST1_DB_UPDATE, ST2_NONE, PAF01_DATA,
                                       (float)duration, &TimingStats );
					}

					if (PTEMSG_OK != ret)
					{
						if( PTEMSG_NOT_FOUND == ret )
						{
							if ( DB_Timing_Flag == true )
								start_time = ptetime_get_time();

							 if (DB_NullCheck_Flag==1)
							 {
								CheckNULLTerminatedinPAF01 (&paf01,MODE_INSERT);
							 }
							ret =db_insert_paf01(&paf01, temp_str);
							if ( DB_Timing_Flag == true )
							{
								end_time = ptetime_get_time();
								duration = end_time - start_time;
								update_timing_stats( ST1_DB_INSERT, ST2_NONE, PAF01_DATA,
                                             (float)duration, &TimingStats );
							}
						}
						if(PTEMSG_OK != ret)
						{
							errorMsgNotInsert(primary_record, (pCHAR)paf01.primary_key.primary_acct_nbr, temp_str);
						}
					 }

				}/*End of updateinsertflag if*/


			}  /*end of if dataRecord*/
			if (0 == (recCnt % 100))
			{
				dbcommon_commit ();
			}

			/*log every 250 records to show progress*/
			if ( 0 == (recCnt % 250))
			{
				sprintf(tempMsg,"IMPORT: Processed %ld %s",recCnt, primary_record);
				LogMsg(tempMsg,INFO_MSG);
			}
			
	 }     
       if (dataRecord[0] == 'T')
	   {
			/*read the trailer record in the file.*/		
			strncpy(count,&dataRecord[17],6);
			count[6]='\0';
		   
			trailerExist = 1;
			recCnt++;         

			/*the next line will make sure that the file name in the trailer along with*/
			/*number of the records in the trailer record would match.*/
			
			if  (recCnt == atoi(count))
				recNumber = 1;
		}  
        dbcommon_commit ();
		fclose(fptr1);
		if ((headerExist == 1) && (trailerExist == 1) && recNumber == 1)
		{
			EndProcessImport = 0;
			returnValue = POPULATE_DATA_SERVER;
		}
		else
		{
			ptetime_get_timestamp( time_date );   
			sprintf(tempMsg," %s >> IMPORT: Number of records, header record, or trailer record error occurred\n",time_date);
			fputs(tempMsg,dumpPtr);
		}
		sprintf(tempMsg,"IMPORT: Processed %ld %s",recCnt, primary_record);
		LogMsg(tempMsg, INFO_MSG);


		return(returnValue);

   
}  /*end of processMultiplePrimaryImportCommand*/

/*************************************************************************************
       NAME:           processCardHoldImportCommand()

       DESCRIPTION:    This module processes the cardholder information.
                       1)  It reads the the card holder file.
                       2)  It parses the Header,Detail,and the Trailer record.
                       3)  It maps the cardholder information to the cardholder table.
                       4)  It does the update/insert/select calls to the data server.
                       
       INPUTS:         filename is the name of the cardholder file.
                       
                       choice could take on the value of 1 or 0.  If it is 0 it will
                       only read the file and determines if the file format is good.
                       If it is 1 it will make the calls to the data server with the
                       proper information.
                       
	    OUTPUTS:        If this value is POPULATE_DATA_SERVER, the record is good.  
                       The record could be used to make update/insert/select calls to 
                       the data server. If this value is NOT_POPULATE_DATA_SERVER, the 
                       record is not good.  The code prints an error message and go to the next
                       record.
                       
                       
       RETURNS:        
		   None.                       
                               
      AUTHOR:          Faramarz Arad.
      MODIFIED BY:     None.
*************************************************************************************/
INT processCardHoldImportCommand(pCHAR filename, INT choice)
{
   CHAR dataRecord[cardHoldRecordLength + 10];
   myCardHoldHeader  cardHoldHeadRecord;
   myCardHoldDetail  cardHoldDetailRecord;
   myCardHoldTrailer cardHoldTrailerRecord;
   INT EndProcessImport = 0,returnValue = 0;
   long recCnt = 0;
   INT headerExist = 0, trailerExist = 0, recNumber = 0;
   CCF01  ccf01;
   CCF02P ccf02p;
   CHAR time_date[25] = "";
   CHAR  temp_str[100]={0},tempMsg[100]={0};
   BYTE  ret=0;

   double   start_time;
   double   end_time;
   double   duration;


   if (NULL == (fptr = fopen(filename,"r")))
   {
      sprintf(tempMsg,"IMPORT: Error can not open file %s .",filename);
      fputs(tempMsg,dumpPtr);
      exit_import(0);
   }

   /*This is the main loop.  Read a record at a time to parse.*/
   while(!EndProcessImport)
   {
            
      if (NULL == fgets(dataRecord,cardHoldRecordLength+1,fptr))
      {
         EndProcessImport = 1;
         break;
      }


      /*read the detail record of the file.*/
      if (dataRecord[0] == 'D')
      {
         parseCardHoldDetail(dataRecord, &cardHoldDetailRecord);
         recCnt++;
         /************process the CCF01 table.***********************/
         populateCcf01Table(&cardHoldDetailRecord, &ccf01);
         
         /*added by farad on 2/18/1999 to fix bug #2.  Check to see if */
         /*the ccf01 table does not have any blank in the required fields.*/
         if ( ccf01HasBlankReqField (&ccf01) == CORRECT)
         {
            errorMsgBlankField(card1_record, (pCHAR)ccf01.primary_key.card_nbr);
            continue;
         }
         
         /* Update the record.  If it fails due to not found, insert it.
          * Logically, the code could be structured better here,
          * but since speed is critical, and 98-99% of the records
          * are updates, leave the code structured this way.
          */
		 if (DB_NullCheck_Flag==1)
		 {
			CheckNULLTerminatedinCCF01 (&ccf01,MODE_UPDATE);
		 }
         ret = db_update_ccf01(&ccf01, temp_str);
         if (PTEMSG_OK != ret)
         {
            if( PTEMSG_NOT_FOUND == ret )
            {
				 if (DB_NullCheck_Flag==1)
				 {
					CheckNULLTerminatedinCCF01 (&ccf01,MODE_INSERT);
				 }
			     ret = db_insert_ccf01(&ccf01, temp_str);
            }
            if (PTEMSG_OK != ret)
			{
               errorMsgNotInsert(card1_record, (pCHAR)ccf01.primary_key.card_nbr, temp_str);         
			}
         }

         /***************process the CCF02P table.*************************/
         populateCcf02pTable(&cardHoldDetailRecord, &ccf02p);

         if ( ccf02pHasBlankReqField (&ccf02p) == CORRECT)
         {
			errorMsgBlankField(card2_record, (pCHAR)ccf02p.primary_key.card_nbr);
            continue;
         }
            
         if ( DB_Timing_Flag == true )
            start_time = ptetime_get_time();

         /* Update the record.  If it fails due to not found, insert it.
          * Logically, the code could be structured better here,
          * but since speed is critical, and 98-99% of the records
          * are updates, leave the code structured this way.
          */
		 if (DB_NullCheck_Flag==1)
		 {
			CheckNULLTerminatedinCCF02P (&ccf02p,MODE_UPDATE);
		 }
         ret = db_update_ccf02p(&ccf02p, temp_str);

         if ( DB_Timing_Flag == true )
         {
            end_time = ptetime_get_time();
            duration = end_time - start_time;
            update_timing_stats( ST1_DB_UPDATE, ST2_NONE, CCF02P_DATA,
                                 (float)duration, &TimingStats );
         }

         if(PTEMSG_OK != ret)
         {
			if(PTEMSG_NOT_FOUND != ret || ret == RESOURCE_BUSY)
			{
				errorMsgNotUpdate(card2_record,(pCHAR)ccf02p.primary_key.card_nbr, temp_str, ret);
			}
			else
            if( PTEMSG_NOT_FOUND == ret )
            {
               if ( DB_Timing_Flag == true )
                  start_time = ptetime_get_time();

               if (DB_NullCheck_Flag==1)
      		   {
            	   CheckNULLTerminatedinCCF02P (&ccf02p,MODE_INSERT);
      		   }
               ret = db_insert_ccf02p(&ccf02p, temp_str);

               if ( DB_Timing_Flag == true )
               {
                  end_time = ptetime_get_time();
                  duration = end_time - start_time;
                  update_timing_stats( ST1_DB_INSERT, ST2_NONE, CCF02P_DATA,
                                       (float)duration, &TimingStats );
               }
            }
            if(PTEMSG_OK != ret)
            {
               errorMsgNotInsert(card2_record,(pCHAR)ccf02p.primary_key.card_nbr, temp_str);                         
            }
         }
      }
      else if (dataRecord[0] == 'H')
      {
         /*read the header record of the file.*/
			parseCardHoldHeader(dataRecord, &cardHoldHeadRecord);
         recCnt++;         /*added by farad on 2/18/1999 for bug #1*/
         headerExist = 1;
      }
      else if (dataRecord[0] == 'T')
      {
         /*read the trailer record in the file. */
			parsecardHoldTrailer(dataRecord, &cardHoldTrailerRecord);
         trailerExist = 1;
         recCnt++;            /*added by farad on 2/18/1999 for bug #1*/

         /*the next line will make sure that the file name in the trailer along with*/
         /*number of the records in the trailer record would match.*/
         if ( (recCnt == atoi(cardHoldTrailerRecord.trailNumberofRecord)) &&
              (strcmp("CRDHFILE",cardHoldTrailerRecord.trailFileName) == 0) )
            recNumber = 1;
         break;
      }

	   /* Commit every 100 records. */
      if (0 == (recCnt % 100))
      {
	      dbcommon_commit ();
      }

      /* Log every 250 records to show progress */
      if (0 == (recCnt % 250))
      {
         sprintf(tempMsg,"IMPORT: Processed %ld %s",recCnt, card1_record);
         LogMsg(tempMsg, INFO_MSG);
      }
   }
   
   /* Catch All */
   dbcommon_commit ();

   fclose(fptr);
   if ((headerExist == 1) && (trailerExist == 1) && recNumber == 1)
   {
      EndProcessImport = 0;
      returnValue = POPULATE_DATA_SERVER;
   }
   else
      { 
		ptetime_get_timestamp( time_date );
   
		sprintf(tempMsg," %s >> IMPORT: Number of records, header record, or trailer record error occurred\n",time_date);
		fputs(tempMsg,dumpPtr);
   }
   sprintf(tempMsg,"IMPORT: Processed %ld %s",recCnt, card1_2_record);
   LogMsg(tempMsg, INFO_MSG);

   return(returnValue);
}  /*end of processCardHoldImportCommand*/
/*************************************************************************************
       NAME:           processMultipleCardHoldImportCommand()

       DESCRIPTION:    This module processes the cardholder information.
                       1)  It reads the the card holder file.
                       2)  It parses only those records that are needed to be inserted or updated.
					       of the Header,Detail,and the Trailer records.
                       3)  It maps the cardholder information to the cardholder table.
                       4)  It does the update/insert/select calls to the data server.
                       
       INPUTS:         filename is the name of the cardholder file.
                       
                       choice could take on the value of 1 or 0.  If it is 0 it will
                       only read the file and determines if the file is in proper format.
                       If it is 1 it will make the calls to the data server with the
                       proper information.
                       
	    OUTPUTS:        If this value is POPULATE_DATA_SERVER, the record is in proper format.  
                       The record could be used to make update/insert/select calls to 
                       the data server. If this value is NOT_POPULATE_DATA_SERVER, the 
                       record is notin proper format.  The code prints an error message 
					   and go to the next record.
                       
                       
       RETURNS:        None.                       
                               
      AUTHOR:         Sailaja Yarlgadda. 
      MODIFIED BY:     


***********************************************************************************/
INT processMultipleCardHoldImportCommand(pCHAR filename,pCHAR filename1, INT choice)
{
   CHAR dataRecord[cardHoldRecordLength + 10];
   CHAR dataRecord1[cardHoldRecordLength + 10];
   CHAR olddataRecord[cardHoldRecordLength + 10];

   myCardHoldDetail  cardHoldDetailRecord;
   INT EndProcessImport = 0,EndProcessImport1 = 0,returnValue = 0;
   long recCnt = 0;
   INT headerExist = 0, trailerExist = 0, recNumber = 0;
   CCF01  ccf01;
   CCF02P ccf02p;

   CHAR  temp_str[100]={0},tempMsg[100]={0};
   BYTE  ret=0;
   long   i=0;
   CHAR  count[7];
   CHAR time_date[25] = "";
   CHAR  cardnbr[20],cardnbr1[20];
   INT  updateinsertflag;

   double   start_time;
   double   end_time;
   double   duration;



   if (NULL == (fptr = fopen(filename,"r")))
   {
      sprintf(tempMsg,"IMPORT: Error can not open file %s.",filename);
 	  fputs(tempMsg,dumpPtr);
	  LogMsg(tempMsg, INFO_MSG);
      exit_import(0);
   }
    while(!EndProcessImport)
	{
			if(NULL ==fgets(dataRecord,cardHoldRecordLength+1,fptr))
			{
				EndProcessImport = 1;
				break;
			
			}


		    if (dataRecord[0] == 'H')     
			{        
		
				recCnt++;         
				headerExist = 1;
			} 
			else if(dataRecord[0] == 'D')
			{
				strncpy(cardnbr,&dataRecord[7],19);
				cardnbr[19]='\0';
				break;
			}
		
	 }
      
     if (NULL == (fptr1 = fopen(filename1,"r")))
	{
      sprintf(tempMsg,"IMPORT: Error can not open file1 %s.",filename1);      
	  fputs(tempMsg,dumpPtr);
      exit_import(0);
	 }  

	  while(!EndProcessImport1)
	  {
			if(NULL ==fgets(dataRecord1,cardHoldRecordLength+1,fptr1))
			{
				EndProcessImport1 = 1;
				break;
			
			}

		    if (dataRecord1[0] == 'H')     
			{   		
				
				headerExist = 1;
			} 
			   else if(dataRecord1[0] == 'D')
			   {
					strncpy(cardnbr1,&dataRecord1[7],19);
					cardnbr1[19]='\0';				
         			break;
				}
		
	  }
	
		/*This is the main loop.  Read a record at a time to parse.*/
		while((!EndProcessImport)&&(!EndProcessImport1))
		{   
			strcpy(olddataRecord,dataRecord);
			updateinsertflag=0;
			if(dataRecord[0]=='T')
			{
				break;
			}

			/*read the detail record of the file and parse it.*/
			if ((dataRecord[0] == 'D')|| (dataRecord1[0]=='D'))
			{               
					/*check whether the cardnumbers are equal or greater or less*/
				if(0==strcmp(cardnbr,cardnbr1))
				{
					if(0!=strcmp(dataRecord,dataRecord1))
					{
						updateinsertflag=1;
						
					}
				  
			  
					if (NULL == fgets(dataRecord,cardHoldRecordLength+1,fptr))
					{
						EndProcessImport = 1;
						break;
					}
					else 
					{
						recCnt++;
						strncpy(cardnbr,&dataRecord[7],19);
		   				cardnbr[19]='\0';
		   			}
			 
					if (NULL == fgets(dataRecord1,cardHoldRecordLength+1,fptr1))
					{
						EndProcessImport1 = 1;
						break;
					}
					else 
					{ 					
						strncpy(cardnbr1,&dataRecord1[7],19);
		   				cardnbr1[19]='\0';
		   			}			
				}

				else if(0>strcmp(cardnbr1,cardnbr))
				{
					if(NULL ==fgets(dataRecord1,cardHoldRecordLength+1,fptr1))
					{
						EndProcessImport1 = 1;
						break;			
					}				
					else 
					{
						strncpy(cardnbr1,&dataRecord1[7],19);
		   				cardnbr1[19]='\0';			
		   			}		
				}
				 else if(0<strcmp(cardnbr1,cardnbr))
				 { 	 
					 updateinsertflag=1;
									
					if(NULL ==fgets(dataRecord,cardHoldRecordLength+1,fptr))
					{
						EndProcessImport = 1;
						break;			
					}				
					else 
					{
						recCnt++;
						strncpy(cardnbr,&dataRecord[7],19);
		   				cardnbr[19]='\0';
		   			}   
				
				}
				
                if(updateinsertflag==1)
				{
					//populateCcfTable(olddataRecord, &ccf01, &ccf02p);
					parseCardHoldDetail(olddataRecord, &cardHoldDetailRecord);
					/************process the CCF01 table.***********************/
                    populateCcf01Table(&cardHoldDetailRecord, &ccf01);
         
         
					/*the ccf01 table does not have any blank in the required fields.*/
					if ( ccf01HasBlankReqField (&ccf01) == CORRECT)
					{
						 errorMsgBlankField(card1_record, (pCHAR)ccf01.primary_key.card_nbr);
						continue;
					}        

					if ( DB_Timing_Flag == true )
						start_time = ptetime_get_time();

					 if (DB_NullCheck_Flag==1)
					 {
						CheckNULLTerminatedinCCF01 (&ccf01,MODE_UPDATE);
					 }
					ret = db_update_ccf01(&ccf01, temp_str);

					if ( DB_Timing_Flag == true )
					{
						end_time = ptetime_get_time();
						duration = end_time - start_time;
						update_timing_stats( ST1_DB_UPDATE, ST2_NONE, CCF01_DATA,
                                       (float)duration, &TimingStats );
					}

					if (PTEMSG_OK != ret)
					{
						if( PTEMSG_NOT_FOUND == ret )
						{
							 if ( DB_Timing_Flag == true )
								start_time = ptetime_get_time();

							 if (DB_NullCheck_Flag==1)
							 {
								CheckNULLTerminatedinCCF01 (&ccf01,MODE_INSERT);
							 }
							 ret = db_insert_ccf01(&ccf01, temp_str);
							 if ( DB_Timing_Flag == true )
							 {
								end_time = ptetime_get_time();
								duration = end_time - start_time;
								update_timing_stats( ST1_DB_INSERT, ST2_NONE, CCF01_DATA,
													 (float)duration, &TimingStats );
							 }
						}
						if (PTEMSG_OK != ret)
						{
							errorMsgNotInsert(card1_record, (pCHAR)ccf01.primary_key.card_nbr, temp_str);         
						}
					}         
                    /************process the CCF02 table.***********************/
					populateCcf02pTable(&cardHoldDetailRecord, &ccf02p);
					if ( ccf02pHasBlankReqField (&ccf02p) == CORRECT)
					{
						errorMsgBlankField(card2_record, (pCHAR)ccf02p.primary_key.card_nbr);
						continue;
					}
            
				    if ( DB_Timing_Flag == true )
					  start_time = ptetime_get_time();

					 if (DB_NullCheck_Flag==1)
					 {
						CheckNULLTerminatedinCCF02P (&ccf02p,MODE_UPDATE);
					 }
					ret = db_update_ccf02p(&ccf02p, temp_str);

				   if ( DB_Timing_Flag == true )
				   {
					  end_time = ptetime_get_time();
					  duration = end_time - start_time;
					  update_timing_stats( ST1_DB_UPDATE, ST2_NONE, CCF02P_DATA,
										   (float)duration, &TimingStats );
				   }

					if(PTEMSG_OK != ret)
					{
						if( PTEMSG_NOT_FOUND == ret )
						{
							if ( DB_Timing_Flag == true )
								start_time = ptetime_get_time();

							 if (DB_NullCheck_Flag==1)
							 {
								CheckNULLTerminatedinCCF02P (&ccf02p,MODE_INSERT);
							 }
							ret = db_insert_ccf02p(&ccf02p, temp_str);
							if ( DB_Timing_Flag == true )
							{
								end_time = ptetime_get_time();
								duration = end_time - start_time;
								update_timing_stats( ST1_DB_INSERT, ST2_NONE, CCF02P_DATA,
												 (float)duration, &TimingStats );
							}
						}
						if(PTEMSG_OK != ret)
						{
							errorMsgNotInsert(card2_record,(pCHAR)ccf02p.primary_key.card_nbr, temp_str);
						}
					}


				}/*End of updateinsertflag if*/


			}  /*end of if dataRecord*/
			if (0 == (recCnt % 100))
			{
				dbcommon_commit ();
			}

			/*log every 250 records to show progress*/
			if ( 0 == (recCnt % 250))
			{
				sprintf(tempMsg,"IMPORT: Processed %ld %s",recCnt, card1_record);
				LogMsg(tempMsg,INFO_MSG);
			}
			
	 }     
       if (dataRecord[0] == 'T')
	   {
			/*read the trailer record in the file.*/		
			strncpy(count,&dataRecord[17],6);
			count[6]='\0';
			trailerExist = 1;
			recCnt++;         

			/*the next line will make sure that the file name in the trailer along with*/
			/*number of the records in the trailer record would match.*/
			if  (recCnt == atoi(count))
				recNumber = 1;              
		}  
         dbcommon_commit ();
		fclose(fptr1);
		if ((headerExist == 1) && (trailerExist == 1) && recNumber == 1)
		{
			EndProcessImport = 0;
			returnValue = POPULATE_DATA_SERVER;
		}
		else
		{
			ptetime_get_timestamp( time_date );   
			sprintf(tempMsg," %s >> IMPORT: Number of records, header record, or trailer record error occurred\n",time_date);
			fputs(tempMsg,dumpPtr);
		}
		sprintf(tempMsg,"IMPORT: Processed %ld %s",recCnt, card1_2_record);
		LogMsg(tempMsg, INFO_MSG);


		return(returnValue);

   
}  /*end of processMultipleCardHoldImportCommand*/


/*************************************************************************************
       NAME:           processBalanceImportCommand()

       DESCRIPTION:    This module processes the balance information.
                       1)  It reads the the balance file.
                       2)  It parses the Header,Detail,and the Balance record.
                       3)  It maps the Balance information to the balance table.
                       4)  It does the update/insert/select calls to the data server.
                       
       INPUTS:         filename is the name of the balance file.
                       
                       choice could take on the value of 1 or 0.  If it is 0 it will
                       only read the file and determines if the file format is good.
                       If it is 1 it will make the calls to the data server with the
                       proper information.
                       
	    OUTPUTS:        If this value is POPULATE_DATA_SERVER, the record is good.  
                       The record could be used to make update/insert/select calls to 
                       the data server. If this value is NOT_POPULATE_DATA_SERVER, the 
                       record is not good.  The code prints an error message and go to the next
                       record.
                       
                       
       RETURNS:        
		   None.                       
                               
      AUTHOR:          Faramarz Arad.
      MODIFIED BY:     None.
*************************************************************************************/
INT processBalanceImportCommand(pCHAR filename, INT choice)
{
   CHAR dataRecord[balanceRecordLength + 10];
   myBalanceHeader  balanceHeadRecord;
   myBalanceTrailer balanceTrailerRecord;
   INT EndProcessImport = 0,returnValue = 0;
   long recCnt = 0;
   INT headerExist = 0, trailerExist = 0, recNumber = 0;
   ACF01 acf01;
   CHAR  temp_str[100] = {0},tempMsg[100]={0};
   BYTE  ret=0;
   CHAR time_date[25] = "";

   double   start_time;
   double   end_time;
   double   duration;

   if (NULL == (fptr = fopen(filename,"r")))
   {
      sprintf(tempMsg,"IMPORT: Error can not open file %s.",filename);      
	  fputs(tempMsg,dumpPtr);
      exit_import(0);
   }

   /*This is the main loop.  Read a record at a time to parse.*/
   while(!EndProcessImport)
   {
      /*read a record if there is any problem quit.*/
      if (NULL == fgets(dataRecord,balanceRecordLength+1,fptr))
      {
         EndProcessImport = 1;
         break;
      }


      /*read the detail record of the file and parse it.*/
      if (dataRecord[0] == 'D')
      {
         parseBalanceDetail(dataRecord, &acf01);
         recCnt++;
         
         /*added by farad on 2/22/1999 to fix bug #2.  Check to see if */
         /*there is a required field which is blank (all spaces or NULL).*/
         if(acf01HasBlankReqField(&acf01) == CORRECT)
         {
            errorMsgBlankField(account_record, (pCHAR)acf01.primary_key.account_nbr);
            continue;
         }

         if ( DB_Timing_Flag == true )
            start_time = ptetime_get_time();

         /* Update the record.  If it fails due to not found, insert it.
          * Logically, the code could be structured better here,
          * but since speed is critical, and 98-99% of the records
          * are updates, leave the code structured this way.
          */
		 if (DB_NullCheck_Flag==1)
		 {
			CheckNULLTerminatedinACF01 (&acf01,MODE_UPDATE);
		 }
         ret = db_update_acf01(&acf01, temp_str);
         if ( DB_Timing_Flag == true )
         {
            end_time = ptetime_get_time();
            duration = end_time - start_time;
            update_timing_stats( ST1_DB_UPDATE, ST2_NONE, ACF01_DATA,
                                 (float)duration, &TimingStats );
         }

         if (PTEMSG_OK != ret)
         {
            if( PTEMSG_NOT_FOUND == ret )
            {
               if ( DB_Timing_Flag == true )
                  start_time = ptetime_get_time();

      		  if (DB_NullCheck_Flag==1)
      		  {
      			CheckNULLTerminatedinACF01 (&acf01,MODE_INSERT);
      		  }
			   ret = db_insert_acf01(&acf01, temp_str);
               if ( DB_Timing_Flag == true )
               {
                  end_time = ptetime_get_time();
                  duration = end_time - start_time;
                  update_timing_stats( ST1_DB_INSERT, ST2_NONE, ACF01_DATA,
                                       (float)duration, &TimingStats );
               }
            }
            if (PTEMSG_OK != ret)
            {
               errorMsgNotInsert(account_record, (pCHAR)acf01.primary_key.account_nbr, temp_str);                         
            }
         }
         
      }  /*end of if (dataReco*/
      else if (dataRecord[0] == 'H')     
      {
         /*read the header record of the file and parse it.*/
			parseBalanceHeader(dataRecord, &balanceHeadRecord);
         recCnt++;         /*added by farad on 2/18/1999 for bug #1  */
         headerExist = 1;
      } 
      else if (dataRecord[0] == 'T')
      {
         /*read the trailer record in the file.*/
			parseBalanceTrailer(dataRecord, &balanceTrailerRecord);
         trailerExist = 1;
         recCnt++;         /*added by farad on 2/18/1999 for bug #1*/

         /*the next line will make sure that the file name in the trailer along with*/
         /*number of the records in the trailer record would match.*/
         if ( (recCnt == atoi(balanceTrailerRecord.trailNumberofRecord)) && 
              (strcmp("BLNCFILE",balanceTrailerRecord.trailFileName) == 0 )   )
            recNumber = 1;
         break;         
      }

      if (0 == (recCnt % 100))
	  {
		  dbcommon_commit ();
	  }

	  /*log every 250 records to show progress*/
	  if ( 0 == (recCnt % 250))
	  {
		  sprintf(tempMsg,"IMPORT: processed Balance record number %ld ", recCnt);
	      LogMsg(tempMsg,INFO_MSG);
	  }		  


   }
   
   fclose(fptr);
   if ((headerExist == 1) && (trailerExist == 1) && recNumber == 1)
   {
      EndProcessImport = 0;
      returnValue = POPULATE_DATA_SERVER;
   }
   else
      {
		ptetime_get_timestamp( time_date );
   
		sprintf(tempMsg," %s >> IMPORT: Number of records, header record, or trailer record error occurred\n",time_date);
		fputs(tempMsg,dumpPtr);
	}

   sprintf(tempMsg,"IMPORT: Processed %ld %s",recCnt, account_record);
   LogMsg(tempMsg, INFO_MSG);


   return(returnValue);
}  /*end of processBalanceImportCommand*/

/***********************************************************************************
NAME:           processMultipleBalanceImportCommand()

       DESCRIPTION:    This module processes the balance information.
                       1)  It reads the two balance files.
                       2)  It parses only those records that are needed to be inserted or updated.
					       of the Header,Detail,and the Trailer records.
                       3)  It maps the Balance information to the balance table.
                       4)  It does the update/insert/select calls to the data server.
                       
       INPUTS:         filename is the name of the balance file.
                       
                       choice could take on the value of 1 or 0.  If it is 0 it will
                       only read the file and determines if the file is in proper format.
                       If it is 1 it will make the calls to the data server with the
                       proper information.
                       
	    OUTPUTS:        If this value is POPULATE_DATA_SERVER, the record is in proper format.  
                       The record could be used to make update/insert/select calls to 
                       the data server. If this value is NOT_POPULATE_DATA_SERVER, the 
                       record is not in proper format. The code prints an error message and go to the next
                       record.
                       
                       
       RETURNS:        
		   None.                       
                               
      AUTHOR:		Sailaja Yarlagadda.          
      MODIFIED BY:     

*********************************************************************************/

INT processMultipleBalanceImportCommand(pCHAR filename,pCHAR filename1, INT choice)
{
   CHAR dataRecord[balanceRecordLength + 10];
   CHAR dataRecord1[balanceRecordLength + 10];
   CHAR olddataRecord[balanceRecordLength + 10];
      
   INT EndProcessImport = 0,EndProcessImport1 = 0,returnValue = 0;
   long recCnt = 0;
   INT headerExist = 0, trailerExist = 0, recNumber = 0;
   ACF01 acf01;
   CHAR  temp_str[100] = {0},tempMsg[100]={0};
   BYTE  ret=0;
   CHAR count[7] = {0};
   CHAR time_date[25] = {0};
   CHAR  accnbr[20] = {0};
   CHAR  accnbr1[20] = {0};
   INT  updateinsertflag;

   double   start_time;
   double   end_time;
   double   duration;

   if (NULL == (fptr = fopen(filename,"r")))
   {
      sprintf(tempMsg,"IMPORT: Error can not open file %s.",filename);
      fputs(tempMsg,dumpPtr);
      exit_import(0);
   }
    while(!EndProcessImport)
	{
			if(NULL ==fgets(dataRecord,balanceRecordLength+1,fptr))
			{
				EndProcessImport = 1;
				break;
			
			}

		    if (dataRecord[0] == 'H')     
			{        
		
				recCnt++;         
				headerExist = 1;
			} 
			else if(dataRecord[0] == 'D')
			{
				strncpy(accnbr,&dataRecord[5],19);
				accnbr[19]='\0';
				break;
			}
		
	 }
      
     if (NULL == (fptr1 = fopen(filename1,"r")))
	{
      sprintf(tempMsg,"IMPORT: Error can not open file1 %s.",filename1);      
	  fputs(tempMsg,dumpPtr);
      exit_import(0);
	 }  

	  while(!EndProcessImport1)
	  {
			if(NULL ==fgets(dataRecord1,balanceRecordLength+1,fptr1))
			{
				EndProcessImport1 = 1;
				break;
			
			}

		    if (dataRecord1[0] == 'H')     
			{   		
				
				headerExist = 1;
			} 
			   else if(dataRecord1[0] == 'D')
			   {
					strncpy(accnbr1,&dataRecord1[5],19);
					accnbr1[19]='\0';				
         			break;
				}
		
	  }
	
		/*This is the main loop.  Read a record at a time to parse.*/
		while((!EndProcessImport)&&(!EndProcessImport1))
		{   
			strcpy(olddataRecord,dataRecord);
			updateinsertflag=0;
			if(dataRecord[0]=='T')
			{
				break;
			}

			/*read the detail record of the file and parse it.*/
			if ((dataRecord[0] == 'D')|| (dataRecord1[0]=='D'))
			{               
					/*check whether the cardnumbers are equal or greater or less*/
				if(0==strcmp(accnbr,accnbr1))
				{
					if(0!=strcmp(dataRecord,dataRecord1))
					{
						updateinsertflag=1;
						
					}
					  
			  
					if (NULL == fgets(dataRecord,balanceRecordLength+1,fptr))
					{
						EndProcessImport = 1;
						break;
					}
					else 
					{
						recCnt++;
						strncpy(accnbr,&dataRecord[5],19);
		   				accnbr[19]='\0';
		   			}
			 
					if (NULL == fgets(dataRecord1,balanceRecordLength+1,fptr1))
					{
						EndProcessImport1 = 1;
						break;
					}
					else 
					{ 	
						strncpy(accnbr1,&dataRecord1[5],19);
		   				accnbr1[19]='\0';
		   			}			
				}

				else if(0>strcmp(accnbr1,accnbr))
				{
					if(NULL ==fgets(dataRecord1,balanceRecordLength+1,fptr1))
					{
						EndProcessImport1 = 1;
						break;			
					}				
					else 
					{
						strncpy(accnbr1,&dataRecord1[5],19);
		   				accnbr1[19]='\0';			
		   			}		
				}
				else if(0<strcmp(accnbr1,accnbr))
				{
					 updateinsertflag=1;
									
					if(NULL ==fgets(dataRecord,balanceRecordLength+1,fptr))
					{
						EndProcessImport = 1;
						break;			
					}				
					else 
					{	
						recCnt++;
						strncpy(accnbr,&dataRecord[5],19);
		   				accnbr[19]='\0';
		   			}   
				
				}
				
                if(updateinsertflag==1)
				{
					parseBalanceDetail(olddataRecord, &acf01);
					
         
					/* Check to see if */
					/*there is a required field which is blank (all spaces or NULL).*/
					if(acf01HasBlankReqField(&acf01) == CORRECT)
					{
						errorMsgBlankField(account_record, (pCHAR)acf01.primary_key.account_nbr);
						continue;
					} 

				   if ( DB_Timing_Flag == true )
					  start_time = ptetime_get_time();

				   if (DB_NullCheck_Flag==1)
				  {
						CheckNULLTerminatedinACF01 (&acf01,MODE_UPDATE);
				  }
				  ret = db_update_acf01(&acf01, temp_str);

				   if ( DB_Timing_Flag == true )
				   {
					  end_time = ptetime_get_time();
					  duration = end_time - start_time;
					  update_timing_stats( ST1_DB_UPDATE, ST2_NONE, ACF01_DATA,
										   (float)duration, &TimingStats );
				   }

					if (PTEMSG_OK != ret)
					{
						if( PTEMSG_NOT_FOUND == ret )
						{
							 if ( DB_Timing_Flag == true )
								start_time = ptetime_get_time();

							 if (DB_NullCheck_Flag==1)
							 {
									CheckNULLTerminatedinACF01 (&acf01,MODE_INSERT);
							 }
							 ret = db_insert_acf01(&acf01, temp_str);

							 if ( DB_Timing_Flag == true )
							 {
								end_time = ptetime_get_time();
								duration = end_time - start_time;
								update_timing_stats( ST1_DB_INSERT, ST2_NONE, ACF01_DATA,
													 (float)duration, &TimingStats );
							 }
						}
						if (PTEMSG_OK != ret)
						{
							errorMsgNotInsert(account_record, (pCHAR)acf01.primary_key.account_nbr, temp_str);                         
						}
					}

				}/*End of updateinsertflag if*/


			}  /*end of if dataRecord*/
			if (0 == (recCnt % 100))
			{
				dbcommon_commit ();
			}

			/*log every 250 records to show progress*/
			if ( 0 == (recCnt % 250))
			{
				sprintf(tempMsg,"IMPORT: processed Balance record number %ld ", recCnt);
				LogMsg(tempMsg,INFO_MSG);
			}
			
	 }     
       if (dataRecord[0] == 'T')
	   {
			/*read the trailer record in the file.*/		
			strncpy(count,&dataRecord[17],6);
			count[6]='\0';
		    trailerExist = 1;
			recCnt++;         

			/*the next line will make sure that the file name in the trailer along with*/
			/*number of the records in the trailer record would match.*/
			if  (recCnt == atoi(count))
				recNumber = 1; 
			
		}  
  
		fclose(fptr1);
		if ((headerExist == 1) && (trailerExist == 1) && recNumber == 1)
		{
			EndProcessImport = 0;
			returnValue = POPULATE_DATA_SERVER;
		}
		else
		{
			ptetime_get_timestamp( time_date );   
			sprintf(tempMsg," %s >> IMPORT: Number of records, header record, or trailer record error occurred\n",time_date);
			fputs(tempMsg,dumpPtr);
		}
		sprintf(tempMsg,"IMPORT: Processed %ld %s",recCnt, account_record);
		LogMsg(tempMsg, INFO_MSG);

		return(returnValue);

   
} 
/*end of processMultipleBalanceImportCommand*/

/*************************************************************************************

NAME:           processMpParameterImportCommand()


/*************************************************************************************/
INT processMpParameterImportCommand (pCHAR filename,INT choice)
{
   BYTE  ret;

   CHAR dataRecord[mpparameterRecordLength + 10];

   myMpParameterHeader  mpparaHeadRecord;
   myMpParameterDetail  mpparaDetailRecord;
   myMpParameterTrailer mpparaTrailerRecord;
   
   INT EndProcessImport = 0,recCnt = 0,returnValue=0;
   INT headerExist=0,trailerExist=0,recNumber=0;
   
   MPPARA01 mppara01;
   CHAR  temp_str[100] = {0};
   CHAR  tempMsg [100] = {0};
   CHAR  time_date[25] = {0};

   double   start_time;
   double   end_time;
   double   duration;


   Import_count.readCount1 = Import_count.skipCount1 = Import_count.errorCount1 =0;
   Import_count.insertCount = Import_count.updateCount = 0;

   if ( NULL == (fptr = fopen(filename,"r")))
   {
      sprintf(tempMsg,"IMPORT: file %s was not opened.",filename);
      fputs(tempMsg,dumpPtr);
      exit_import(0);
   }

   /*This is the main loop.  Read a record at a time to parse.*/
   while(!EndProcessImport)
   {
      /*read a record.  If there is any problem Quit.            */
      if (NULL == fgets(dataRecord,mpparameterRecordLength+1,fptr))
      {
         EndProcessImport = 1;
         break;
      }

	  Import_count.readCount1++;

      /*read the detail record in the file.*/
      if (dataRecord[0] == 'D')
      {
         parseMpParameterDetail(dataRecord, &mpparaDetailRecord);
         
         recCnt++;
         
         /*************populate the mppara0101 table.************************/
         populateMPPARA01Table(&mpparaDetailRecord, &mppara01);

         /*added by farad on 2/23/1999 to fix bug #2.  Check to see if */
         /*there is a required field which is blank (all spaces or NULL).*/
         if(mppara01HasBlankReqField(&mppara01) == CORRECT)
         {
			Import_count.errorCount1++;
            errorMsgBlankField(mpparameter_record, (pCHAR)mppara01.primary_key.product_code);
            continue;
         }

         if ( DB_Timing_Flag == true )
            start_time = ptetime_get_time();

         /* Update the record.  If it fails due to not found, insert it.
          * Logically, the code could be structured better here,
          * but since speed is critical, and 98-99% of the records
          * are updates, leave the code structured this way.
          */
		   if (DB_NullCheck_Flag==1)
		  {
				CheckNULLTerminatedinMPPARA01 (&mppara01,MODE_UPDATE);
		  }
         ret = db_update_mppara01(&mppara01, temp_str);

         if ( DB_Timing_Flag == true )
         {
            end_time = ptetime_get_time();
            duration = end_time - start_time;
            update_timing_stats( ST1_DB_UPDATE, ST2_NONE, MPPARA01_DATA,
                                 (float)duration, &TimingStats );
         }

         if (PTEMSG_OK != ret)
         {
            if (PTEMSG_NOT_FOUND == ret)
            {
               if ( DB_Timing_Flag == true )
                  start_time = ptetime_get_time();

    		   if (DB_NullCheck_Flag==1)
    		  {
    				CheckNULLTerminatedinMPPARA01 (&mppara01,MODE_INSERT);
    		  }
               ret = db_insert_mppara01(&mppara01, temp_str);

               if ( DB_Timing_Flag == true )
               {
                  end_time = ptetime_get_time();
                  duration = end_time - start_time;
                  update_timing_stats( ST1_DB_GET_COUNT, ST2_NONE, BCH10_DATA,
                                       (float)duration, &TimingStats );
               }
            }
            if (PTEMSG_OK != ret)
			{
			   Import_count.errorCount1++;
               errorMsgNotInsert(mpparameter_record, (pCHAR)mppara01.primary_key.product_code, temp_str);
			}
			else
				Import_count.insertCount++;
         }
		 else
			 Import_count.updateCount++;




	  }
      else if (dataRecord[0] == 'H')
      {
         /*read the header record in the file.*/
		   parseMpParameterHeader(dataRecord, &mpparaHeadRecord);
         recCnt++;      /*added by farad on 2/18/1999 for bug #1*/
         headerExist = 1;         
      }
      else if (dataRecord[0] == 'T')
      {
		   /*read the trailer record in the file. */
		parseMpParameterTrailer(dataRecord, &mpparaTrailerRecord);
         trailerExist = 1;
         recCnt++;      /*added by farad on 2/18/1999 for bug #1*/
         
         /*the next line will make sure that the file name in the trailer along with*/
         /*number of the records in the trailer record would match.*/
         if ( (recCnt == atoi(mpparaTrailerRecord.trailNumberofRecord)) &&
              (strcmp("MPPAFILE",mpparaTrailerRecord.trailFileName) == 0))
            recNumber = 1;
         break;
      }

	   /* Commit every 100 records. */
      if (0 == (recCnt % 100))
      {
	      dbcommon_commit ();
      }

      /* Log every 250 records to show progress */
      if (0 == (recCnt % 250))
      {
         sprintf(tempMsg,"IMPORT: Processed %ld %s",recCnt, mpparameter_record);
         LogMsg(tempMsg, INFO_MSG);
      }
   }
   
   /* Catch All */
	dbcommon_commit ();

   fclose(fptr);   

   if( (headerExist == 1) && (trailerExist == 1) && (recNumber == 1) )
   {
      EndProcessImport = 0;
      returnValue = POPULATE_DATA_SERVER;
   }
   else
   {
		ptetime_get_timestamp( time_date );   
		sprintf(tempMsg," %s >> IMPORT: Number of records, header record, or trailer record error occurred\n",time_date);
		fputs(tempMsg,dumpPtr);
	}      

		sprintf(tempMsg,"IMPORT: Processed %ld %s",recCnt, mpparameter_record);
        LogMsg(tempMsg, INFO_MSG);

	/* Log MpParameter count records */
	
	LogCounts("MPPARAMETER",0);

   return(returnValue);
}  


/*end of processMpParameterImportCommand*/


/*************************************************************************************

NAME:           processMerchantGroupImportCommand()


/*************************************************************************************/

INT processMerchantGroupImportCommand (pCHAR filename,INT choice)
{
   BYTE  ret;

   CHAR dataRecord[merchantgroupRecordLength + 10];

   myMerchantGroupHeader  mgfHeadRecord;
   myMerchantGroupDetail  mgfDetailRecord;
   myMerchantGroupTrailer mgfTrailerRecord;
   
   INT EndProcessImport = 0,recCnt = 0,returnValue=0;
   INT headerExist=0,trailerExist=0,recNumber=0;
   
   MGF01 mgf01;
   CHAR  temp_str[100] = {0};
   CHAR  tempMsg [100] = {0};
   CHAR  time_date[25] = {0};

   double   start_time;
   double   end_time;
   double   duration;


   Import_count.readCount1 = Import_count.skipCount1 = Import_count.errorCount1 =0;
   Import_count.insertCount = Import_count.updateCount = 0;

   if ( NULL == (fptr = fopen(filename,"r")))
   {
      sprintf(tempMsg,"IMPORT: file %s was not opened.",filename);
      fputs(tempMsg,dumpPtr);
      exit_import(0);
   }

   /*This is the main loop.  Read a record at a time to parse.*/
   while(!EndProcessImport)
   {
      /*read a record.  If there is any problem Quit.            */
      if (NULL == fgets(dataRecord,merchantgroupRecordLength+1,fptr))
      {
         EndProcessImport = 1;
         break;
      }

	   /*read the detail record in the file.*/
      if (dataRecord[0] == 'D')
      {
         parseMerchantGroupDetail(dataRecord, &mgfDetailRecord);
         
         recCnt++;
          Import_count.readCount1++;
         /*************populate the mgf01 table.************************/
         populateMGF01Table(&mgfDetailRecord, &mgf01);

         /*added by farad on 2/23/1999 to fix bug #2.  Check to see if */
         /*there is a required field which is blank (all spaces or NULL).*/
         if(mgf01HasBlankReqField(&mgf01) == CORRECT)
         {
			Import_count.errorCount1++;
            errorMsgBlankField(merchantgroup_record, (pCHAR)mgf01.primary_key. merchant_group_id);
            continue;
         }

         if ( DB_Timing_Flag == true )
            start_time = ptetime_get_time();

         /* Update the record.  If it fails due to not found, insert it.
          * Logically, the code could be structured better here,
          * but since speed is critical, and 98-99% of the records
          * are updates, leave the code structured this way.
          */
		   if (DB_NullCheck_Flag==1)
		  {
				CheckNULLTerminatedinMGF01 (&mgf01,MODE_UPDATE);
		  }
         ret = db_update_mgf01(&mgf01, temp_str);

         if ( DB_Timing_Flag == true )
         {
            end_time = ptetime_get_time();
            duration = end_time - start_time;
            update_timing_stats( ST1_DB_UPDATE, ST2_NONE, MGF01_DATA,
                                 (float)duration, &TimingStats );
         }

         if (PTEMSG_OK != ret)
         {
            if (PTEMSG_NOT_FOUND == ret)
            {
               if ( DB_Timing_Flag == true )
                  start_time = ptetime_get_time();

    		   if (DB_NullCheck_Flag==1)
    		  {
    				CheckNULLTerminatedinMGF01 (&mgf01,MODE_INSERT);
    		  }
               ret = db_insert_mgf01(&mgf01, temp_str);

               if ( DB_Timing_Flag == true )
               {
                  end_time = ptetime_get_time();
                  duration = end_time - start_time;
                  update_timing_stats( ST1_DB_GET_COUNT, ST2_NONE, BCH10_DATA,
                                       (float)duration, &TimingStats );
               }
            }
            if (PTEMSG_OK != ret)
			{
			   Import_count.errorCount1++;
               errorMsgNotInsert(merchantgroup_record, (pCHAR)mgf01.primary_key. merchant_group_id, temp_str);
			}
			else
				Import_count.insertCount++;
         }
		 else
			 Import_count.updateCount++;




	  }
      else if (dataRecord[0] == 'H')
      {
         /*read the header record in the file.*/
		   parseMerchantGroupHeader(dataRecord, &mgfHeadRecord);
         recCnt++;      /*added by farad on 2/18/1999 for bug #1*/
		  Import_count.readCount1++;
         headerExist = 1;         
      }
      else if (dataRecord[0] == 'T')
      {
		   /*read the trailer record in the file. */
		parseMerchantGroupTrailer(dataRecord, &mgfTrailerRecord);
         trailerExist = 1;
         recCnt++;      /*added by farad on 2/18/1999 for bug #1*/
          Import_count.readCount1++;
         /*the next line will make sure that the file name in the trailer along with*/
         /*number of the records in the trailer record would match.*/
         if ( (recCnt == atoi(mgfTrailerRecord.trailNumberofRecord)) &&
              (strcmp("MGRPFILE",mgfTrailerRecord.trailFileName) == 0))
            recNumber = 1;
         break;
      }

	   /* Commit every 100 records. */
      if (0 == (recCnt % 100))
      {
	      dbcommon_commit ();
      }

      /* Log every 250 records to show progress */
      if (0 == (recCnt % 250))
      {
         sprintf(tempMsg,"IMPORT: Processed %ld %s",recCnt, merchantgroup_record);
         LogMsg(tempMsg, INFO_MSG);
      }
   }
   
   /* Catch All */
	dbcommon_commit ();

   fclose(fptr);   

   if( (headerExist == 1) && (trailerExist == 1) && (recNumber == 1) )
   {
      EndProcessImport = 0;
      returnValue = POPULATE_DATA_SERVER;
   }
   else
   {
		ptetime_get_timestamp( time_date );   
		sprintf(tempMsg," %s >> IMPORT: Number of records, header record, or trailer record error occurred\n",time_date);
		fputs(tempMsg,dumpPtr);
	}      

	sprintf(tempMsg,"IMPORT: Processed %ld %s",recCnt, merchantgroup_record);
	LogMsg(tempMsg, INFO_MSG);

	/* Log MerchantGroup count records */
	
	LogCounts("MERCHANTGROUP",0);

   return(returnValue);
}  


/*end of processMerchantGroupImportCommand*/

/*************************************************************************************

NAME:           processEDTRImportCommand()


/*************************************************************************************/

/*************************************************************************************/
INT  processEDTRImportCommand (pCHAR filename,INT choice)
{
	CHAR             dataRecord[terminalRecordLength+10]={0};
	myEDTRHeader  	 EDTRHeadRecord;
	myEDTRTrailer 	 EDTRTrailerRecord;
	INT              EndProcessImport = 0;
	INT              returnValue  = 0;
	INT              headerExist  = 0;
	INT              trailerExist = 0;
	INT              recNumber    = 0;
	LONG             recCnt       = 0;
	DCF01            dcf01;
	CHAR             temp_str[200]={0};
	BYTE             ret = 0;
	CHAR             tempMsg[100] = {0};
	CHAR             time_date[25] = {0};
	BYTE			 fileprimarykey[51] = {0};
    CHAR             ChipActiveFlag[2] = {0};

	double   start_time;
	double   end_time;
	double   duration;

	if (NULL == (fptr = fopen(filename,"r")))
	{
		sprintf(tempMsg,"IMPORT: Could not open file %s",filename);
		fputs(tempMsg,dumpPtr);
		exit_import(0);
	}

	get_term_ini_info();

	/*This is the main loop.  Read a record at a time to parse.*/
	while(!EndProcessImport)
	{
		/* Read a record of the file. */
		if( NULL == fgets(dataRecord, TermRecLength+3, fptr) )
		{
			EndProcessImport = 1;
			break;
		}

		/*parse the detail record of the file.*/
		if( dataRecord[0] == 'D')
		{
			parseEDTRDetail(dataRecord, &dcf01,&ChipActiveFlag);         
			recCnt++;

			/* Check to see if there is a required field which
			* is blank (all spaces or NULL).
			*/
			if(dcf01HasBlankReqField_EDTR(&dcf01) == CORRECT)
			{
				errorMsgBlankField(device_record, (pCHAR)dcf01.primary_key.device_id);
				continue;
			}

			if ( DB_Timing_Flag == true )
			start_time = ptetime_get_time();

			/*
			Check if record is already present or Not
			*/
			ret = db_select_dcf01_EDTR(&dcf01, temp_str);
			if(ret ==PTEMSG_OK)
			{
				if(0==strncmp(ChipActiveFlag,"1",1))
				{
					memcpy( dcf01.device_type, "07", 2 ); // Default to ISO Device
					memcpy(dcf01.status ,"A", 1); // Make devce active.
				}
				if (DB_NullCheck_Flag==1)
				{
					CheckNULLTerminatedinDCF01 (&dcf01,MODE_UPDATE);
				}
				ret = db_update_dcf01(&dcf01, temp_str);
				if ( DB_Timing_Flag == true )
				{
					end_time = ptetime_get_time();
					duration = end_time - start_time;
					update_timing_stats( ST1_DB_UPDATE, ST2_NONE, DCF01_DATA,
					(float)duration, &TimingStats );
				}
			}
			else 
			{
				if ( DB_Timing_Flag == true )
					start_time = ptetime_get_time();
				
				 if(0==strncmp(ChipActiveFlag,"1",1))
				 {
					memcpy( dcf01.device_type, "07", 2 ); //SMART DEVICE
				 }
				 else
				 {
					memcpy( dcf01.device_type, "03", 2 ); //EFTPOS_ISO
				 }
					memcpy(dcf01.status ,"A", 1); // Make devce active.
												
					/* We need to populate valid transaction field based on below requirment
						if MCC value = 6010, the Valid Transactions field value will be
						0011000010001011111111100000011111111111
						if MCC value <> 6010, the Valid Transactions field value will be
						1101111111111111111111111111111111111111*/
	
					if(0==strncmp(dcf01.category_code,"6010",4))
					{
						memcpy(dcf01.valid_transactions    , "0011000010001011111111100000011111111111"    ,  sizeof(dcf01.valid_transactions    ) - 1);
					}
					else
					{
						memcpy(dcf01.valid_transactions    , "1101111111111111111111111111111111111111"    ,  sizeof(dcf01.valid_transactions    ) - 1);
					}
					if (DB_NullCheck_Flag==1)
					{
						CheckNULLTerminatedinDCF01 (&dcf01,MODE_INSERT);
					}
					ret = db_insert_dcf01_EDTR(&dcf01 , temp_str);
					if ( DB_Timing_Flag == true )
					{
						end_time = ptetime_get_time();
						duration = end_time - start_time;
						update_timing_stats( ST1_DB_INSERT, ST2_NONE, DCF01_DATA,
						(float)duration, &TimingStats );
					}
					if (PTEMSG_OK == ret)
					{
						strcpy(fileprimarykey,(pCHAR)dcf01.primary_key.device_id);
						strcat(fileprimarykey,(pCHAR)dcf01.merchant_id);
						hst01update(&fileprimarykey);
					}
				}
			if (PTEMSG_OK != ret)
			{
				errorMsgNotInsert(device_record, (pCHAR)dcf01.primary_key.device_id, temp_str);
			}
		}  /*end if( dataR */

		else if( dataRecord[0] == 'H')
		{
			/*parse the header record of the file.*/
			parseEDTRHeader(dataRecord, &EDTRHeadRecord);
			recCnt++;
			headerExist = 1;
		} 
		else if (dataRecord[0] == 'T')
		{
			/*parse the trailer record in the file.*/
			parseEDTRTrailer(dataRecord, &EDTRTrailerRecord);
			trailerExist = 1;
			recCnt++;

			/* The next line will make sure that the file name in the
			* trailer along with number of the records in the trailerrecord would match.	*/
			
			if ( (recCnt == atoi(EDTRTrailerRecord.trailNumberofRecords))  &&
			(strcmp("EDTRFILE",EDTRTrailerRecord.trailFileName) == 0) )
			recNumber = 1;

			break;
		}

		/* Commit every 100 records. */
		if (0 == (recCnt % 100))
		{
			dbcommon_commit ();
		}

		/* Log every 250 records to show progress */
		if (0 == (recCnt % 250))
		{
			sprintf(tempMsg,"IMPORT: Processed %ld %s",recCnt, device_record);
			LogMsg(tempMsg, INFO_MSG);
		}
	}  /*while(!EndProcessImport)*/

	/* Catch All */
	dbcommon_commit ();

	fclose(fptr);

	if((headerExist == 1) && (trailerExist == 1) && recNumber == 1)
	{
		EndProcessImport = 0;
		returnValue = POPULATE_DATA_SERVER;
	}
	else
	{ 
		ptetime_get_timestamp( time_date );
		sprintf( tempMsg,
		" %s >> IMPORT: Number of records, header record, or"
		" trailer record error occurred\n",
		time_date );
		fputs( tempMsg,dumpPtr );
	}

	sprintf(tempMsg,"IMPORT: Processed %ld %s",recCnt, device_record);
	LogMsg(tempMsg, INFO_MSG);

	return( returnValue );  

}  /*end of processEDTRImportCommand*/

/*************************************************************************************
       NAME:           processMPAVImportCommand()

       DESCRIPTION:    This module processes the merchant information.
                       1)  It reads the the merchant Pre auth validation file.
                       2)  It parses the Header,Detail,and the Trailer record.
                       3)  It maps the merchant information to the merchant table.
                       4)  It does the update calls to the data server.

      AUTHOR:          Abhishek Kumar Verma.
      MODIFIED BY:     None.
*************************************************************************************/
void processMPAVImportCommand (pCHAR filename,INT choice)
{
	CHAR             dataRecord[terminalRecordLength+10]={0};
	myMPAVHeader  	 MPAVHeadRecord;
	myMPAVTrailer 	 MPAVTrailerRecord;
	myMPAVDetail     merchDetailRec;
	MCF01 mcf01;
	INT               EndProcessImport = 0;
	INT               returnValue  = 0;
	INT               headerExist  = 0;
	INT               trailerExist = 0;
	INT               recNumber    = 0;
	LONG              recCnt       = 0;
	CHAR              temp_str[200] = {0};
	BYTE              ret = 0;
	CHAR              tempMsg[100] = {0};
	CHAR              time_date[25] = {0};
	BYTE			  fileprimarykey[51] = {0};


	double   start_time;
	double   end_time;
	double   duration;

	memset(&mcf01,0,sizeof(mcf01));
	memset(&merchDetailRec,0,sizeof(merchDetailRec));

	if (NULL == (fptr = fopen(filename,"r")))
	{
		sprintf(tempMsg,"IMPORT: Could not open file %s",filename);
		fputs(tempMsg,dumpPtr);
		exit_import(0);
	}

	get_term_ini_info();

	/*This is the main loop.  Read a record at a time to parse.*/
	while(!EndProcessImport)
	{
		/* Read a record of the file. */
		if( NULL == fgets(dataRecord, TermRecLength+3, fptr) )
		{
			EndProcessImport = 1;
			break;
		}

		/*parse the detail record of the file.*/
		if( dataRecord[0] == 'D')
		{
			parseMPAVDetail(dataRecord,&merchDetailRec);
			recCnt++;

			/* Check to see if there is a required field which
			* is blank (all spaces or NULL).
			*/
			if(mcf01HasBlankReqField_MPAV(&merchDetailRec) == CORRECT)
			{
				errorMsgBlankField(merchant_record, (pCHAR)merchDetailRec.detailMerchantNumber);
				continue;
			}

			if ( DB_Timing_Flag == true )
			start_time = ptetime_get_time();

			/*
			Check if record is already present or Not
			*/
			memcpy(mcf01.primary_key.merchant_id,     merchDetailRec.detailMerchantNumber,(sizeof(mcf01.primary_key.merchant_id)-1));
			memcpy(mcf01.primary_key.organization_id, merchDetailRec.detailOrganizaNumber,(sizeof(mcf01.primary_key.organization_id)-1));

			ret = db_select_mcf01_MPAV(&mcf01, temp_str);
			if(ret ==PTEMSG_OK)
			{
				strncpy (mcf01.network[7].priority_routing   ,merchDetailRec.PreAuthValidationFlag,sizeof(merchDetailRec.PreAuthValidationFlag) -1);
				if (DB_NullCheck_Flag==1)
				{
					CheckNULLTerminatedinMCF01 (&mcf01,MODE_UPDATE);
				}
				ret = db_update_mcf01(&mcf01, temp_str);
				if ( DB_Timing_Flag == true )
				{
					end_time = ptetime_get_time();
					duration = end_time - start_time;
					update_timing_stats( ST1_DB_UPDATE, ST2_NONE, DCF01_DATA,
					(float)duration, &TimingStats );
				}
			}

			if (PTEMSG_OK != ret)
			{
				errorMsgNotInsert(merchant_record, (pCHAR)mcf01.primary_key.merchant_id, temp_str);
			}
		}  /*end if( dataR */

		else if( dataRecord[0] == 'H')
		{
			/*parse the header record of the file.*/
			parseMPAVHeader(dataRecord, &MPAVHeadRecord);
			recCnt++;
			headerExist = 1;
		}
		else if (dataRecord[0] == 'T')
		{
			/*parse the trailer record in the file.*/
			parseMPAVTrailer(dataRecord, &MPAVTrailerRecord);
			trailerExist = 1;
			recCnt++;

			/* The next line will make sure that the file name in the
			* trailer along with number of the records in the trailerrecord would match.	*/

			if ( (recCnt == atoi(MPAVTrailerRecord.trailNumberofRecords))  &&
			(strcmp("MPAVFILE",MPAVTrailerRecord.trailFileName) == 0) )
			recNumber = 1;

			break;
		}

		/* Commit every 100 records. */
		if (0 == (recCnt % 100))
		{
			dbcommon_commit ();
		}

		/* Log every 250 records to show progress */
		if (0 == (recCnt % 250))
		{
			sprintf(tempMsg,"IMPORT: Processed %ld %s",recCnt, merchant_record);
			LogMsg(tempMsg, INFO_MSG);
		}
	}  /*while(!EndProcessImport)*/

	/* Catch All */
	dbcommon_commit ();

	fclose(fptr);

	if((headerExist == 1) && (trailerExist == 1) && recNumber == 1)
	{
		EndProcessImport = 0;
		returnValue = POPULATE_DATA_SERVER;
	}
	else
	{
		ptetime_get_timestamp( time_date );
		sprintf( tempMsg,
		" %s >> IMPORT: Number of records, header record, or"
		" trailer record error occurred\n",
		time_date );
		fputs( tempMsg,dumpPtr );
	}

	sprintf(tempMsg,"IMPORT: Processed %ld %s",recCnt, merchant_record);
	LogMsg(tempMsg, INFO_MSG);

}  /*end of processMPAVImportCommand*/

/*************************************************************************************

NAME:           processEMVCImportCommand()


/*************************************************************************************/

/*************************************************************************************/
INT  processEMVCImportCommand (pCHAR filename,INT choice)
{
	CHAR             dataRecord[terminalRecordLength+10]={0};
	myEMVCHeader  	 EMVCHeadRecord;
	myEMVCTrailer 	 EMVCTrailerRecord;
	INT              EndProcessImport = 0;
	INT              returnValue  = 0;
	INT              headerExist  = 0;
	INT              trailerExist = 0;
	INT              recNumber    = 0;
	LONG             recCnt       = 0;
	DCF01            dcf01;
	CHAR             temp_str[200]={0};
	BYTE             ret = 0;
	CHAR             tempMsg[100]={0};
	CHAR             time_date[25] = {0};
	BYTE			 fileprimarykey[51]= {0};
    CHAR             EMVCpabilityindicator[7]= {0};

	double   start_time;
	double   end_time;
	double   duration;

	if (NULL == (fptr = fopen(filename,"r")))
	{
		sprintf(tempMsg,"IMPORT: Could not open file %s",filename);
		fputs(tempMsg,dumpPtr);
		exit_import(0);
	}

	get_term_ini_info();

	/*This is the main loop.  Read a record at a time to parse.*/
	while(!EndProcessImport)
	{
		/* Read a record of the file. */
		if( NULL == fgets(dataRecord, TermRecLength+3, fptr) )
		{
			EndProcessImport = 1;
			break;
		}

		/*parse the detail record of the file.*/
		if( dataRecord[0] == 'D')
		{
			parseEMVCDetail(dataRecord, &dcf01,&EMVCpabilityindicator);
			recCnt++;
			memcpy(dcf01.current_bus_date,EMVCpabilityindicator, (sizeof(EMVCpabilityindicator) -1 ));
			/* Check to see if there is a required field which
			 *  is blank (all spaces or NULL).
			*/
			if(dcf01HasBlankReqField_EMVC(&dcf01) == CORRECT)
			{
				errorMsgBlankField(device_record, (pCHAR)dcf01.primary_key.device_id);
				continue;
			}

			if ( DB_Timing_Flag == true )
				start_time = ptetime_get_time();

			/*
			Check if record is already present or Not
			*/
			ret = db_select_dcf01_EMVC(&dcf01, temp_str);
			if(ret ==PTEMSG_OK)
			{
				memcpy(dcf01.current_bus_date,EMVCpabilityindicator, (sizeof(EMVCpabilityindicator) -1 ));
				if (DB_NullCheck_Flag==1)
				{
					CheckNULLTerminatedinDCF01 (&dcf01,MODE_UPDATE);
				}
				ret = db_update_dcf01(&dcf01, temp_str);
				if ( DB_Timing_Flag == true )
				{
					end_time = ptetime_get_time();
					duration = end_time - start_time;
					update_timing_stats( ST1_DB_UPDATE, ST2_NONE, DCF01_DATA,
					(float)duration, &TimingStats );
				}
			}
			/*else
			{
				if ( DB_Timing_Flag == true )
					start_time = ptetime_get_time();
				ret = db_insert_dcf01_EMVC(&dcf01 , temp_str);
				if ( DB_Timing_Flag == true )
				{
					end_time = ptetime_get_time();
					duration = end_time - start_time;
					update_timing_stats( ST1_DB_INSERT, ST2_NONE, DCF01_DATA,
					(float)duration, &TimingStats );
				}
				if (PTEMSG_OK == ret)
				{
					strcpy(fileprimarykey,(pCHAR)dcf01.primary_key.device_id);
					strcat(fileprimarykey,(pCHAR)dcf01.merchant_id);
					hst01update(&fileprimarykey);
				}
			}*/
			if (PTEMSG_OK != ret)
			{
				errorMsgNotInsert(device_record, (pCHAR)dcf01.primary_key.device_id, temp_str);
			}
		}  /*end if( dataR */

		else if( dataRecord[0] == 'H')
		{
			/*parse the header record of the file.*/
			parseEMVCHeader(dataRecord, &EMVCHeadRecord);
			recCnt++;
			headerExist = 1;
		}
		else if (dataRecord[0] == 'T')
		{
			/*parse the trailer record in the file.*/
			parseEMVCTrailer(dataRecord, &EMVCTrailerRecord);
			trailerExist = 1;
			recCnt++;

			/* The next line will make sure that the file name in the
			* trailer along with number of the records in the trailerrecord would match.	*/

			if ( (recCnt == atoi(EMVCTrailerRecord.trailNumberofRecords))  &&
			(strcmp("EMVCFILE",EMVCTrailerRecord.trailFileName) == 0) )
				recNumber = 1;

			break;
		}

		/* Commit every 100 records. */
		if (0 == (recCnt % 100))
		{
			dbcommon_commit ();
		}

		/* Log every 250 records to show progress */
		if (0 == (recCnt % 250))
		{
			sprintf(tempMsg,"IMPORT: Processed %ld %s",recCnt, device_record);
			LogMsg(tempMsg, INFO_MSG);
		}
	}  /*while(!EndProcessImport)*/

	/* Catch All */
	dbcommon_commit ();

	fclose(fptr);

	if((headerExist == 1) && (trailerExist == 1) && recNumber == 1)
	{
		EndProcessImport = 0;
		returnValue = POPULATE_DATA_SERVER;
	}
	else
	{
		ptetime_get_timestamp( time_date );
		sprintf( tempMsg,
		" %s >> IMPORT: Number of records, header record, or"
		" trailer record error occurred\n",
		time_date );
		fputs( tempMsg,dumpPtr );
	}

	sprintf(tempMsg,"IMPORT: Processed %ld %s",recCnt, device_record);
	LogMsg(tempMsg, INFO_MSG);

	return( returnValue );

}  /*end of processEMVCImportCommand*/

/******************************************************************************
 *
 * Function:     getTime
 *
 * Description:  This procedure takes an input string that contains a date and
 *               time.  It copies the time portion into an output string,
 *               formatted slightly different.
 *
 * Input:        time_date : Format = "YYYY-0M-0D-0H.0I.0S.JJJ"
 *
 * Output:       time_str  : Format = "0H0I0S"
 *
 * Return Value: None
 *
 ******************************************************************************/
void getTime( pCHAR time_date, pCHAR time_str )
{
   memset ( time_str,  0x00,          7 );
   strncpy( time_str, &time_date[11], 2 );
   strncat( time_str, &time_date[14], 2 );
   strncat( time_str, &time_date[17], 2 );
   return;
}

/******************************************************************************
 *
 * Function:     getDate
 *
 * Description:  This procedure takes an input string that contains a date and
 *               time.  It copies the date portion into an output string,
 *               formatted slightly different.
 *
 * Input:        time_date : Format = "YYYY-0M-0D-0H.0I.0S.JJJ"
 *
 * Output:       date_str  : Format = "YYYY0M0D"
 *
 * Return Value: None
 *
 ******************************************************************************/
void getDate( pCHAR time_date, pCHAR date_str )
{
   memset ( date_str,  0x00,         9 );
   strncpy( date_str,  time_date,    4 );
   strncat( date_str, &time_date[5], 2 );
   strncat( date_str, &time_date[8], 2 );
   return;
}
   
/***********************************************************************************
		NAME:           hst01update()

		DESCRIPTION:    This function populate hst01 table and insert it 
						when a new device is imported through import utility
                       
		INPUTS:         None
                       
	    OUTPUTS:        None           
                       
        RETURNS:        
		   None.                       
                               
       AUTHOR:		Abhishek Verma.          
       MODIFIED BY:     

*********************************************************************************/
void hst01update ( BYTE * fileprimarykey)
{
	CHAR   ErrorMsg  [100];
	HST01	hst01 = {0};
 	char milisecond[4]="";
	CHAR	tempdate[25] = ""; 
	BYTE	file_name[10]="";
	BYTE	file_primary_key[51]="";
	BYTE	changed_by[21]="";
	BYTE	date_modified[9]="";
	BYTE	time_modified[7]="";
	BYTE	field_tag[40]="";
	BYTE	previous_data[51]="";
	BYTE	current_data[51]="";
	BYTE	reserved[21]="";
	BYTE    ret = 0;
	CHAR  count_str[5]="";
	
	/* Prepare file_name in HST01 table*/
	strcpy(file_name,"DCF01_i"); 

	/* Prepare change_by in HST01 table*/
	strcpy(changed_by,(char*)CHANGE_BY); 

 	/* Prepare date_modified in HST01 table*/
	ptetime_get_timestamp( tempdate ); // YYYY-0M-0D-0H.0I.0S.JJJ
	getDate( tempdate, date_modified ); // YYYYMMDD

	/* Prepare time_modified in HST01 table*/
	getTime( tempdate, time_modified ); // hhmmss

	/* Prepare hst01.primary_key.unique_id in HST01 table*/
	strncpy(milisecond,tempdate+20,3);
	if(Count >9999)
	{
		Count = 0;
	}
	else
	{
		Count ++;
		sprintf( count_str, "%04d", Count );
	}
	strcpy(hst01.primary_key.unique_id,date_modified+4);
	strcat(hst01.primary_key.unique_id,time_modified);
	strcat(hst01.primary_key.unique_id,milisecond);
	strcat(hst01.primary_key.unique_id,count_str);
	hst01.primary_key.unique_id[17]='\0';

	/* Prepare file_primary_key in HST01 table*/
	strcpy(file_primary_key,fileprimarykey); 
	
 	/* Prepare field_tag in HST01 table*/
	strcpy(field_tag,(char*)FEILD_TAG);

	/* previous_data in hst01 is null*/
	memset(previous_data,'\0',51);

	/* Prepare current_data in HST01 table*/
	strcpy(current_data,(char*)CURRENT_DATA); 
	strcat(current_data,fileprimarykey);
	 
	/* reserved feild in hst01 is null*/
	memset(reserved,'\0',21);

	/*Prepare HST01 table entery for inseting*/
	memcpy(hst01.file_name ,file_name ,  sizeof(hst01.file_name) - 1);
	memcpy(hst01.changed_by ,changed_by ,  sizeof(hst01.changed_by) - 1);
	memcpy(hst01.date_modified ,date_modified ,  sizeof(hst01.date_modified) - 1);
	memcpy(hst01.time_modified ,time_modified ,  sizeof(hst01.time_modified) - 1);
	memcpy(hst01.file_primary_key ,file_primary_key ,  sizeof(hst01.file_primary_key) - 1);
	memcpy(hst01.field_tag ,field_tag ,  sizeof(hst01.field_tag) - 1);
	memcpy(hst01.previous_data ,previous_data ,  sizeof(hst01.previous_data) - 1);
	memcpy(hst01.current_data ,current_data ,  sizeof(hst01.current_data) - 1);
	memcpy(hst01.reserved ,reserved ,  sizeof(hst01.reserved) - 1);

	// Insert device into hst01 through import utility
	if (DB_NullCheck_Flag==1)
	{
		CheckNULLTerminatedinHST01 (&hst01,MODE_INSERT);
	}
	ret = db_insert_hst01(&hst01, ErrorMsg); 
	if (PTEMSG_OK != ret)
    {
		errorMsgNotInsert(histort_record, file_primary_key, ErrorMsg);
	}
	else
	{
		OpenDSV_WriteHST01(&hst01);
	}
}



void hst01update_with_current_and_previous_data (BYTE * file_name, BYTE * fileprimarykey, BYTE * previousdata,BYTE * currentdata, BYTE * field_tag )
{
	CHAR   ErrorMsg  [100]={0};
	HST01	hst01 = {0};
 	char milisecond[4]="";
	CHAR	tempdate[25] = "";
	BYTE	file_primary_key[51]="";
	BYTE	changed_by[21]="";
	BYTE	date_modified[9]="";
	BYTE	time_modified[7]="";
	BYTE	previous_data[51]="";
	BYTE	current_data[51]="";
	BYTE	reserved[21]="";
	BYTE    ret = 0;
	CHAR  count_str[5]="";


	/* Prepare change_by in HST01 table*/
	strcpy(changed_by,(char*)CHANGE_BY);

 	/* Prepare date_modified in HST01 table*/
	ptetime_get_timestamp( tempdate ); // YYYY-0M-0D-0H.0I.0S.JJJ
	getDate( tempdate, date_modified ); // YYYYMMDD

	/* Prepare time_modified in HST01 table*/
	getTime( tempdate, time_modified ); // hhmmss

	/* Prepare hst01.primary_key.unique_id in HST01 table*/
	strncpy(milisecond,tempdate+20,3);
	if(Count >9999)
	{
		Count = 0;
	}
	else
	{
		Count ++;
		sprintf( count_str, "%04d", Count );
	}
	strcpy(hst01.primary_key.unique_id,date_modified+4);
	strcat(hst01.primary_key.unique_id,time_modified);
	strcat(hst01.primary_key.unique_id,milisecond);
	strcat(hst01.primary_key.unique_id,count_str);
	hst01.primary_key.unique_id[17]='\0';

	/* Prepare file_primary_key in HST01 table*/
	strcpy(file_primary_key,fileprimarykey);

	/* previous_data in hst01 is null*/
	memcpy(previous_data,previousdata,sizeof(previous_data)-1);

	/* Prepare current_data in HST01 table*/
	memcpy(current_data,currentdata,sizeof(current_data)-1);
	//strcat(current_data,fileprimarykey);

	/* reserved feild in hst01 is null*/
	memset(reserved,'\0',21);

	/*Prepare HST01 table entery for inseting*/
	memcpy(hst01.file_name ,file_name ,  sizeof(hst01.file_name) - 1);
	memcpy(hst01.changed_by ,changed_by ,  sizeof(hst01.changed_by) - 1);
	memcpy(hst01.date_modified ,date_modified ,  sizeof(hst01.date_modified) - 1);
	memcpy(hst01.time_modified ,time_modified ,  sizeof(hst01.time_modified) - 1);
	memcpy(hst01.file_primary_key ,file_primary_key ,  sizeof(hst01.file_primary_key) - 1);
	memcpy(hst01.field_tag ,field_tag ,  sizeof(hst01.field_tag) - 1);
	memcpy(hst01.previous_data ,previous_data ,  sizeof(hst01.previous_data) - 1);
	memcpy(hst01.current_data ,current_data ,  sizeof(hst01.current_data) - 1);
	memcpy(hst01.reserved ,reserved ,  sizeof(hst01.reserved) - 1);

	// Insert device into hst01 through import utility
	if (DB_NullCheck_Flag==1)
	{
		CheckNULLTerminatedinHST01 (&hst01,MODE_INSERT);
	}
	ret = db_insert_hst01(&hst01, ErrorMsg);
	if (PTEMSG_OK != ret)
    {
		errorMsgNotInsert(histort_record, file_primary_key, ErrorMsg);
	}
	else
	{
		OpenDSV_WriteHST01(&hst01);
	}
}

/*************************************************************************************
       NAME:           processTerminalImportCommand()

       DESCRIPTION:    This module processes the terminal information.
                       1)  It reads the the terminal file.
                       2)  It parses the Header,Detail,and the terminal record.
                       3)  It maps the terminal information to the terminal table.
                       4)  It does the update/insert/select calls to the data server.
                       
       INPUTS:         filename is the name of the terminal file.
                       
                       choice could take on the value of 1 or 0.  If it is 0 it will
                       only read the file and determines if the file format is good.
                       If it is 1 it will make the calls to the data server with the
                       proper information.
                       
	    OUTPUTS:        If this value is POPULATE_DATA_SERVER, the record is good.  
                       The record could be used to make update/insert/select calls to 
                       the data server. If this value is NOT_POPULATE_DATA_SERVER, the 
                       record is not good.  The code prints an error message and go to the next
                       record.
                       
                       
       RETURNS:        
		   None.                       
                               
      AUTHOR:          Faramarz Arad.
      MODIFIED BY:     None.
*************************************************************************************/
INT  processTerminalImportCommand (pCHAR filename,INT choice)
{
   CHAR              dataRecord[terminalRecordLength+10]={0};
   myTerminalHeader  termHeadRecord;
   myTerminalTrailer termTrailerRecord;
   INT               EndProcessImport = 0;
   INT               returnValue  = 0;
   INT               headerExist  = 0;
   INT               trailerExist = 0;
   INT               recNumber    = 0;
   LONG              recCnt       = 0;
   DCF01             dcf01;
   CHAR              temp_str[200]={0};
   BYTE              ret = 0;
   CHAR              tempMsg[100]={0};
   CHAR              time_date[25] = "";
   BYTE				 fileprimarykey[51]="";

   double   start_time;
   double   end_time;
   double   duration;

   if (NULL == (fptr = fopen(filename,"r")))
   {
      sprintf(tempMsg,"IMPORT: Could not open file %s",filename);
      fputs(tempMsg,dumpPtr);
      exit_import(0);
   }

   get_term_ini_info();

   /*This is the main loop.  Read a record at a time to parse.*/
   while(!EndProcessImport)
   {
      /* Read a record of the file. */
      if( NULL == fgets(dataRecord, TermRecLength+3, fptr) )
      {
         EndProcessImport = 1;
         break;
      }

      /*parse the detail record of the file.*/
      if( dataRecord[0] == 'D')
      {
         parseTerminalDetail(dataRecord, &dcf01);         
         recCnt++;

         /* Check to see if there is a required field which
          * is blank (all spaces or NULL).
          */
         if(dcf01HasBlankReqField(&dcf01) == CORRECT)
         {
            errorMsgBlankField(device_record, (pCHAR)dcf01.primary_key.device_id);
            continue;
         }

         if ( DB_Timing_Flag == true )
            start_time = ptetime_get_time();

         /* Update the record.  If it fails due to not found, insert it.
          * Logically, the code could be structured better here,
          * but since speed is critical, and 98-99% of the records
          * are updates, leave the code structured this way.
          */
		if (DB_NullCheck_Flag==1)
		{
			CheckNULLTerminatedinDCF01 (&dcf01,MODE_UPDATE);
		}
         ret = db_update_dcf01(&dcf01, temp_str);

         if ( DB_Timing_Flag == true )
         {
            end_time = ptetime_get_time();
            duration = end_time - start_time;
            update_timing_stats( ST1_DB_UPDATE, ST2_NONE, DCF01_DATA,
                                 (float)duration, &TimingStats );
         }

         if (PTEMSG_OK != ret)
         {
            if( PTEMSG_NOT_FOUND == ret )
            {
               if ( DB_Timing_Flag == true )
                  start_time = ptetime_get_time();

				if (DB_NullCheck_Flag==1)
				{
					CheckNULLTerminatedinDCF01 (&dcf01,MODE_INSERT);
				}
               ret = db_insert_dcf01(&dcf01 , temp_str);

               if ( DB_Timing_Flag == true )
               {
                  end_time = ptetime_get_time();
                  duration = end_time - start_time;
                  update_timing_stats( ST1_DB_INSERT, ST2_NONE, DCF01_DATA,
                                       (float)duration, &TimingStats );
               }
			   if (PTEMSG_OK == ret)
			   {
					strcpy(fileprimarykey,(pCHAR)dcf01.primary_key.device_id);
					strcat(fileprimarykey,(pCHAR)dcf01.merchant_id);
					hst01update(&fileprimarykey);

			   }
            }
            if (PTEMSG_OK != ret)
            {
               errorMsgNotInsert(device_record, (pCHAR)dcf01.primary_key.device_id, temp_str);
            }
         }
      }  /*end if( dataR */
      else if( dataRecord[0] == 'H')
      {
         /*parse the header record of the file.*/
		   parseTerminalHeader(dataRecord, &termHeadRecord);
         recCnt++;
         headerExist = 1;
      } 
      else if (dataRecord[0] == 'T')
      {
         /*parse the trailer record in the file.*/
         parseTerminalTrailer(dataRecord, &termTrailerRecord);
         trailerExist = 1;
         recCnt++;

         /* The next line will make sure that the file name in the
          * trailer along with number of the records in the trailer
          * record would match.
          */
         if ( (recCnt == atoi(termTrailerRecord.trailNumberofRecords))  &&
              (strcmp("TRMLFILE",termTrailerRecord.trailFileName) == 0) )
            recNumber = 1;

         break;
      }

      /* Commit every 100 records. */
      if (0 == (recCnt % 100))
      {
         dbcommon_commit ();
      }

      /* Log every 250 records to show progress */
      if (0 == (recCnt % 250))
      {
         sprintf(tempMsg,"IMPORT: Processed %ld %s",recCnt, device_record);
         LogMsg(tempMsg, INFO_MSG);
      }

   }  /*while(!EndProcessImport)*/

   /* Catch All */
   dbcommon_commit ();

   fclose(fptr);

   if((headerExist == 1) && (trailerExist == 1) && recNumber == 1)
   {
      EndProcessImport = 0;
      returnValue = POPULATE_DATA_SERVER;
   }
   else
   { 
      ptetime_get_timestamp( time_date );
      sprintf( tempMsg,
              " %s >> IMPORT: Number of records, header record, or"
              " trailer record error occurred\n",
               time_date );
      fputs( tempMsg,dumpPtr );
   }

   sprintf(tempMsg,"IMPORT: Processed %ld %s",recCnt, device_record);
   LogMsg(tempMsg, INFO_MSG);

   return( returnValue );  

}  /*end of processTerminalImportCommand*/

/*************************************************************************************
       NAME:           processMultipleTerminalImportCommand()

       DESCRIPTION:    This module processes the terminal information.
                       1)  It reads the the terminal file.
                       2)  It parses only those records that are needed to be inserted or updated.
					       of the Header,Detail,and the Trailer records.
                       3)  It maps the terminal information to the terminal table.
                       4)  It does the update/insert/select calls to the data server.
                       
       INPUTS:         filename is the name of the terminal file.
                       
                       choice could take on the value of 1 or 0.  If it is 0 it will
                       only read the file and determines if the file is in proper format .
                       If it is 1 it will make the calls to the data server with the
                       proper information.
                       
	    OUTPUTS:        If this value is POPULATE_DATA_SERVER, the record is in proper format.  
                       The record could be used to make update/insert/select calls to 
                       the data server. If this value is NOT_POPULATE_DATA_SERVER, the 
                       record is not in proper format. The code prints an error message and go to the next
                       record.
                       
       RETURNS:        None.                       
                               
      AUTHOR:         Sailaja Yarlagadda.
      MODIFIED BY:     

***********************************************************************************/
INT  processMultipleTerminalImportCommand (pCHAR filename,pCHAR filename1,INT choice)
{
   CHAR dataRecord   [terminalRecordLength+10] = {0};
   CHAR dataRecord1  [terminalRecordLength+10] = {0};
   CHAR olddataRecord[terminalRecordLength+10] = {0};

   INT   EndProcessImport  = 0;
   INT   EndProcessImport1 = 0;
   INT   returnValue       = 0;
   INT   headerExist  = 0;
   INT   trailerExist = 0;
   INT   recNumber    = 0;
   INT   updateinsertflag;
   LONG  recCnt = 0;
   DCF01 dcf01;
   BYTE  ret = 0;
   CHAR  temp_str[200] = {0};
   CHAR  tempMsg[100]  = {0};
   CHAR  count[7];
   CHAR  time_date[25] = "";
   CHAR  deviceid [9];
   CHAR  deviceid1[9];

   double   start_time;
   double   end_time;
   double   duration;


   if ( NULL == (fptr = fopen(filename,"r")) )
   {
      sprintf(tempMsg,"IMPORT: Error can not open file %s.",filename);
      fputs(tempMsg,dumpPtr);
      exit_import(0);
   }

   get_term_ini_info();

   while(!EndProcessImport)
   {
      if ( NULL ==fgets(dataRecord,TermRecLength+3,fptr) )
      {
         EndProcessImport = 1;
         break;
      }

      if (dataRecord[0] == 'H')
      {
         recCnt++;
         headerExist = 1;
      }
      else if(dataRecord[0] == 'D')
      {
         strncpy(deviceid,&dataRecord[3],8);
         deviceid[8]='\0';
         break;
      }
   }

   if (NULL == (fptr1 = fopen(filename1,"r")))
   {
      sprintf(tempMsg,"IMPORT: Error can not open file1 %s.",filename1);
      fputs(tempMsg,dumpPtr);
      exit_import(0);
   }

   while( !EndProcessImport1 )
   {
      if ( NULL ==fgets(dataRecord1,TermRecLength+3,fptr1) )
      {
         EndProcessImport1 = 1;
         break;
      }

      if ( dataRecord1[0] == 'H' )
      {
         headerExist = 1;
      }
      else if(dataRecord1[0] == 'D')
      {
         strncpy( deviceid1,&dataRecord1[3],8 );
         deviceid1[8]='\0';
         break;
      }
   }

   /* This is the main loop.  Read a record at a time to parse. */
   while((!EndProcessImport)&&(!EndProcessImport1))
   {
      strcpy(olddataRecord,dataRecord);
      updateinsertflag=0;
      if ( dataRecord[0]=='T' )
      {
         break;
      }

      /* Read the detail record of the file and parse it. */
      if ((dataRecord[0] == 'D')|| (dataRecord1[0]=='D'))
      {
         /* Check whether the cardnumbers are equal or greater or less */
         if ( 0==strcmp(deviceid,deviceid1) )
         {
            if ( 0!=strcmp(dataRecord,dataRecord1) )
            {
               updateinsertflag=1;
            }

            if ( NULL == fgets(dataRecord,TermRecLength+3,fptr) )
            {
               EndProcessImport = 1;
               break;
            }
            else
            {
               recCnt++;
               strncpy(deviceid,&dataRecord[3],8);
               deviceid[8]='\0';
            }

            if ( NULL == fgets(dataRecord1,TermRecLength+3,fptr1) )
            {
               EndProcessImport1 = 1;
               break;
            }
            else 
            {
               strncpy(deviceid1,&dataRecord1[3],8);
               deviceid1[8]='\0';
            }
         }
         else if( 0>strcmp(deviceid1,deviceid) )
         {
            if ( NULL ==fgets(dataRecord1,TermRecLength+3,fptr1) )
            {
               EndProcessImport1 = 1;
               break;
            }
            else 
            {
               strncpy( deviceid1,&dataRecord1[3],8 );
               deviceid1[8]='\0';
            }
         }
         else if( 0<strcmp(deviceid1,deviceid) )
         {
            updateinsertflag = 1;
            if ( NULL ==fgets(dataRecord,TermRecLength+3,fptr) )
            {
               EndProcessImport = 1;
               break;
            }
            else 
            {
               recCnt++;
               strncpy(deviceid,&dataRecord[3],8);
               deviceid[8]='\0';
            }
         }

         if ( updateinsertflag == 1 )
         {
            parseTerminalDetail( olddataRecord, &dcf01 );

            /* Check to see if there is a required field which is
             * blank (all spaces or NULL).
             */
            if ( dcf01HasBlankReqField(&dcf01) == CORRECT )
            {
               errorMsgBlankField(device_record, (pCHAR)dcf01.primary_key.device_id);
               continue;
            }

            if ( DB_Timing_Flag == true )
               start_time = ptetime_get_time();

			if (DB_NullCheck_Flag==1)
			{
				CheckNULLTerminatedinDCF01 (&dcf01,MODE_UPDATE);
			}
            ret = db_update_dcf01(&dcf01, temp_str);

            if ( DB_Timing_Flag == true )
            {
               end_time = ptetime_get_time();
               duration = end_time - start_time;
               update_timing_stats( ST1_DB_UPDATE, ST2_NONE, DCF01_DATA,
                                    (float)duration, &TimingStats );
            }

            if (PTEMSG_OK != ret)
            {
               if( PTEMSG_NOT_FOUND == ret )
               {
                  if ( DB_Timing_Flag == true )
                     start_time = ptetime_get_time();

      			if (DB_NullCheck_Flag==1)
      			{
      				CheckNULLTerminatedinDCF01 (&dcf01,MODE_INSERT);
      			}
                  ret = db_insert_dcf01( &dcf01 , temp_str );

                  if ( DB_Timing_Flag == true )
                  {
                     end_time = ptetime_get_time();
                     duration = end_time - start_time;
                     update_timing_stats( ST1_DB_INSERT, ST2_NONE, DCF01_DATA,
                                          (float)duration, &TimingStats );
                  }
               }

               if ( PTEMSG_OK != ret )
               {
                  errorMsgNotInsert(device_record, (pCHAR)dcf01.primary_key.device_id, temp_str);
               }
            }
         }/*End of updateinsertflag if*/
      }  /*end of if dataRecord*/

      if ( 0 == (recCnt % 100) )
      {
         dbcommon_commit ();
      }

      /* Log every 250 records to show progress */
      if ( 0 == (recCnt % 250) )
      {
         sprintf(tempMsg,"IMPORT: Processed %ld %s",recCnt, device_record);
         LogMsg(tempMsg,INFO_MSG);
      }
   }

   if (dataRecord[0] == 'T')
   {
      /* Read the trailer record in the file. */
      strncpy( count,&dataRecord[17],6 );
      count[6]='\0';
      trailerExist = 1;
      recCnt++;

      /* The next line will make sure that the file name in
       * the trailer along with number of the records in the
       * trailer record would match.
       */
      if  (recCnt == atoi(count))
         recNumber = 1;
   }

   dbcommon_commit ();
   fclose(fptr1);
   if ((headerExist == 1) && (trailerExist == 1) && recNumber == 1)
   {
      EndProcessImport = 0;
      returnValue = POPULATE_DATA_SERVER;
   }
   else
   {
      ptetime_get_timestamp( time_date );   
      sprintf(tempMsg," %s >> IMPORT: Number of records, header record, or trailer record error occurred\n",time_date);
      fputs(tempMsg,dumpPtr);
   }

   sprintf(tempMsg,"IMPORT: Processed %ld %s",recCnt, device_record);
   LogMsg(tempMsg, INFO_MSG);

   return( returnValue );
}  /*end of processMultipleTerminalImportCommand*/

/*************************************************************************************
       NAME:           main

       DESCRIPTION:    This module is the entry point for the application.
                       
       INPUTS:         argv[1] is the name of the ascii file which would get parsed.
			   None.
       OUTPUTS:
            None.       
       RETURNS:        
		      None.                       
                               
      AUTHOR:         Faramarz Arad 
      MODIFIED BY:    None.
*************************************************************************************/
void main( INT argc, pCHAR argv[] )
{
   CHAR buffer    [256] ="";
   CHAR temp_str  [30]  ="";

   CHAR Buffer    [20]  ="";
   CHAR Buffer1   [20]  ="";
   CHAR tempMsg   [100] ="";
   CHAR dumpFileName [100]={0};
   CHAR logFileName  [50]={0};
   CHAR skipRecordsFile[50]={0};
   INT               num_sql;
   INT               ret_code;
	INT      ret_val = 0;
	CHAR Err_Buf [256] = {0};

   /*The following items are used for time only.  Should not be deliverd to customer.*/
   CHAR       date_str  [11]  = "";
   CHAR       time_str  [9]   = "";
   CHAR       time_date [25]  = "";

   if( argc < 2 )
   {
      printf( "IMPORT: Wrong number of parameters: \n");
      printf( "import [input file name1][input file name2] \n");
      printf( "[input file name1] : File to be imported\n");
      printf( "[input file name2] : Previous imported file[optional]\n");
      printf( "Example: import ax_bal1.txt(new) ax_bal.txt(old) \n");
      printf( "The application would produce a dump and log file \n");
      printf( "based on the input file name.\n");
      exit( 0 );
   }
   else
   {
      strcpy( AppName, argv[0] );
   }

   strcpy(dumpFileName,"Dump");
   strcat(dumpFileName,argv[1]);
   
   strcpy(logFileName,"Log");
   strcat(logFileName,argv[1]);

   strcat(countFileName,"ImpCnt");
   strcat(countFileName,argv[1]);

   strcat(skipRecordsFile,"SkippedRecords_");
   strcat(skipRecordsFile,argv[1]);

   InitMsgLogging(logFileName);
   HstFilePtr=NULL;
   get_HST01_file_name_path();

   get_HST01_MCF01_file_name_path();


   /* Get current system date & time */
   ptetime_get_timestamp( time_date );
   get_time(time_date, time_str);
   get_date(time_date, date_str);


   sprintf( buffer,
           "IMPORT: Process started, version %s, date %s, time %s",
          Version, date_str, time_str );
   LogMsg( buffer, INFO_MSG );

   /* Get Database Timing Parameters to gather statistics about each query. */
   memset( ReportTime, 0x00, sizeof(ReportTime) );
   memset( buffer,     0x00, sizeof(buffer)     );
   ret_code = get_timing_parameters( &DB_Timing_Flag, ReportTime, &ForceReport, buffer );
   LogMsg( buffer, INFO_MSG );

   /* Initialize the DB Timing Statistics Data Structure. */
   if ( DB_Timing_Flag == true )
   {
      reset_timing_stats( &TimingStats );
   }
	ret_val = get_dataserver_ini_db_error_statics_value( &db_error_statics_flag, &db_error_statics_value,
			AppName,     Err_Buf );



	if( argc <3 )
	{
		/* Log into monitor the filename and application name started.*/
		sprintf(tempMsg,"IMPORT: Process of file %s was started.",argv[1]);
		LogMsg(tempMsg, INFO_MSG);

		/*open the dump text file.*/
		if (NULL == (dumpPtr = fopen(dumpFileName,"w")))
		{
			sprintf(tempMsg,"IMPORT: Error - Can not open the Dump file");
			fputs(tempMsg,dumpPtr);
			ShutDownMsgLogging();
			exit(0);
		}

		if (NULL == (skipRecordPtr = fopen(skipRecordsFile,"a")))
		{
			sprintf(tempMsg,"IMPORT: Error - Can not open the Update Skipped Records log file");
			fputs(tempMsg,dumpPtr);
			ShutDownMsgLogging();
			exit(0);
		}

		if (NULL == (HstMcF01Ptr = fopen(HST01MCF01_Filename,"a")))
		{
			sprintf(tempMsg,"IMPORT: Error - Can not open the HST01MCF01 file");
			fputs(tempMsg,dumpPtr);
		}


		/*Determine file type:  Read the first 20 CHAR and extract the file type*/
		if (NULL == (fptr = fopen(argv[1],"r")))
		{
			sprintf(tempMsg,"IMPORT: Could not open Import file %s",argv[1]);
      
			fputs(tempMsg,dumpPtr);
			ShutDownMsgLogging();
			exit(0);
		}

		if(NULL == fgets(Buffer,20,fptr))
		{
			sprintf(tempMsg,"IMPORT: Error - Could not determine file type");
			fputs(tempMsg,dumpPtr);      
			fclose(fptr);
			ShutDownMsgLogging();
			exit(0);
		}
		else
		{
			/*close the file so it could be parsed from the first record later.*/
			fclose(fptr);  

			/* Try to connect to the database */
			if (dbcommon_connect ("equitable", "equitable", "equitable", "ORCL", tempMsg) != PTEMSG_OK)
			{
				fputs(tempMsg,dumpPtr);
				ShutDownMsgLogging();
				exit(0);
			}
			else
				LogMsg("IMPORT: Connected to Database", INFO_MSG);

			/*Call to set large rollback segment RBS37 defined for imports to online state - SCR935 - LM */
			if (db_set_rollback_segment_online( tempMsg) )
			{
				sprintf( buffer, "IMPORT: Unable to set RBS37 online: %s, Continues with next available.", tempMsg);
				LogMsg( buffer, INFO_MSG );
			}
			else
			{
				sprintf( buffer, "IMPORT: Set RBS37 Online.");
				LogMsg( buffer, INFO_MSG );
			}

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


			if (strncmp(Buffer,"HDRMRCHFILE",11) == 0)                     /* merchant file type.*/
				processMerchantImportCommand(argv[1],POPULATE_DATA_SERVER);
			else if (strncmp(Buffer,"HDRTRMLFILE",11) == 0)				   /* terminal file type.*/
				processTerminalImportCommand(argv[1],POPULATE_DATA_SERVER);
			else if (strncmp(Buffer,"HDRPRACFILE",11) == 0)                /* Primary  file type.*/
				processPrimaryImportCommand(argv[1],POPULATE_DATA_SERVER);
			else if (strncmp(Buffer,"HDRCRDHFILE",11) == 0)                /* card holder file type.*/
				processCardHoldImportCommand(argv[1],POPULATE_DATA_SERVER);
			else if (strncmp(Buffer,"HDRBLNCFILE",11) == 0)                /* balance file type.*/
				processBalanceImportCommand(argv[1],POPULATE_DATA_SERVER);
			else if (strncmp(Buffer,"HDRNEGFRAUD",11) == 0)				 /*negative fraud type*/
				processNegFraudImportCommand(argv[1], POPULATE_DATA_SERVER);
			else if (strncmp(Buffer,"HDRMPPAFILE",11) == 0)                     /* mp parameter file type.*/
				processMpParameterImportCommand(argv[1],POPULATE_DATA_SERVER);
			else if (strncmp(Buffer,"HDRMGRPFILE",11) == 0)                     /* merchant group file type.*/
				processMerchantGroupImportCommand(argv[1], POPULATE_DATA_SERVER);
			else if (strncmp(Buffer,"HDREDTRFILE",11) == 0)                     /* EDTRM Extracts  file type.*/
				processEDTRImportCommand(argv[1], POPULATE_DATA_SERVER);
			else if (strncmp(Buffer,"HDRMPAVFILE",11) == 0)                     /* EDTRM Extracts  file type.*/
				processMPAVImportCommand(argv[1], POPULATE_DATA_SERVER);
			else if (strncmp(Buffer,"HDREMVCFILE",11) == 0)                     /* EDTRM Extracts  file type.*/
				processEMVCImportCommand(argv[1], POPULATE_DATA_SERVER);
			else
			{
				sprintf(tempMsg,"IMPORT: file %s HDR is not proper file type.",argv[1]);
				fputs(tempMsg,dumpPtr);
			}
		}

		/* Log event indicating completion*/
		fflush(dumpPtr);
		fflush(skipRecordPtr);
		fclose(dumpPtr);
		fclose(skipRecordPtr);
		sprintf(tempMsg,"IMPORT: Process of file %s has ended.",argv[1]);
		LogMsg(tempMsg, INFO_MSG);
   }
   else
   {
		/* Log into monitor the filename and application name started.*/
		sprintf(tempMsg,"IMPORT: Process of file %s and file1 %s was started.",argv[1],argv[2]);
		LogMsg(tempMsg, INFO_MSG);

		/*open the dump text file.*/
		if (NULL == (dumpPtr = fopen(dumpFileName,"w")))
		{
      
			sprintf(tempMsg,"IMPORT: Error - Can not open the Dump file");
			fputs(tempMsg,dumpPtr);
			ShutDownMsgLogging();
			exit(0);
		}   
		if (NULL == (fptr = fopen(argv[1],"r")))
		{
			sprintf(tempMsg,"IMPORT: Could not open file %s",argv[1]);
      
			fputs(tempMsg,dumpPtr);
			ShutDownMsgLogging();
			exit(0);
		}
	  
		if(NULL == fgets(Buffer,20,fptr))
		{
      
			sprintf(tempMsg,"IMPORT: Error - Could not determine file type");
			fputs(tempMsg,dumpPtr);
			fclose(fptr);
			ShutDownMsgLogging();
			exit(0);
		}
		if (NULL == (fptr1 = fopen(argv[2],"r")))
		{
			sprintf(tempMsg,"IMPORT: Could not open file %s",argv[2]);
      
			fputs(tempMsg,dumpPtr);
			ShutDownMsgLogging();
			exit(0);
		}
		if(NULL == fgets(Buffer1,20,fptr1))
		{
      
			sprintf(tempMsg,"IMPORT: Error - Could not determine file type");
			fputs(tempMsg,dumpPtr);
			fclose(fptr1);
			ShutDownMsgLogging();
			exit(0);
		}
		else
		{
			/*close the file so it could be parsed from the first record later.*/
			fclose(fptr); 
			fclose(fptr1);

			/* Try to connect to the database */
			if (dbcommon_connect ("equitable", "equitable", "equitable", "ORCL", tempMsg) != PTEMSG_OK)
			{
				fputs(tempMsg,dumpPtr);
				ShutDownMsgLogging();
				exit(0);
			}
			else
				LogMsg("IMPORT: Connected to Database", INFO_MSG);

			/*Call to set large rollback segment RBS37 defined for imports to online state - SCR935 - LM */
			if (db_set_rollback_segment_online( tempMsg) )
			{
				sprintf( buffer, "IMPORT: Unable to set RBS37 online: %s, Continues with next available.", tempMsg);
				LogMsg( buffer, INFO_MSG );
			}
			else
			{
				sprintf( buffer, "IMPORT: Set RBS37 Online.");
				LogMsg( buffer, INFO_MSG );
			}

			if ((strncmp(Buffer,"HDRMRCHFILE",11) == 0)&& (strncmp(Buffer1,"HDRMRCHFILE",11) == 0))                    /* merchant file type.*/
				processMultipleMerchantImportCommand(argv[1],argv[2],POPULATE_DATA_SERVER);
			else if ((strncmp(Buffer,"HDRTRMLFILE",11) == 0)&&(strncmp(Buffer1,"HDRTRMLFILE",11) == 0))                /* terminal file type.*/
				processMultipleTerminalImportCommand(argv[1],argv[2],POPULATE_DATA_SERVER);
			else if ((strncmp(Buffer,"HDRPRACFILE",11) == 0)&&(strncmp(Buffer1,"HDRPRACFILE",11) == 0))                /* Primary  file type.*/
				processMultiplePrimaryImportCommand(argv[1],argv[2],POPULATE_DATA_SERVER);
			else if ((strncmp(Buffer,"HDRCRDHFILE",11) == 0)&&(strncmp(Buffer1,"HDRCRDHFILE",11) == 0))                /* card holder file type.*/
				processMultipleCardHoldImportCommand(argv[1],argv[2],POPULATE_DATA_SERVER);
			else if ((strncmp(Buffer,"HDRBLNCFILE",11) == 0)&&(strncmp(Buffer,"HDRBLNCFILE",11) == 0))                /* balance file type.*/
				processMultipleBalanceImportCommand(argv[1],argv[2],POPULATE_DATA_SERVER);
			else if ((strncmp(Buffer,"HDRNEGFRAUD",11) == 0)&&(strncmp(Buffer,"HDRNEGFRAUD",11) == 0))                /* negative fraud file type.*/
				processNegFraudImportCommand(argv[1], POPULATE_DATA_SERVER);
			else
			{
				sprintf(tempMsg,"IMPORT: file %s HDR and file1 %s HDR are not proper file types.",argv[1],argv[2]);
     			fputs(tempMsg,dumpPtr);
			}
		}

		/* Log event indicating completion*/
		fflush(dumpPtr);
		fclose(dumpPtr);
		sprintf(tempMsg,"IMPORT: Process of file %s and file1 %s has ended.",argv[1],argv[2]);
		LogMsg(tempMsg, INFO_MSG);
	} 


	/*Do a commit if there are some which did not get committed.*/
	dbcommon_commit ();

	/* Get current system date & time */
	ptetime_get_timestamp( time_date );
	get_time(time_date, time_str);
	get_date(time_date, date_str);

	/*Call to set rollback segment RBS37 to offline state - SCR935 - LM */
	if (db_set_rollback_segment_offline( tempMsg) )
	{
		sprintf( buffer, "IMPORT: Unable to set RBS37 offline: %s.", tempMsg);
		LogMsg( buffer, INFO_MSG );
	}
	else
	{
		sprintf( buffer, "IMPORT: Set RBS37 offline.");
		LogMsg( buffer, INFO_MSG );
	}

	dbcommon_disconnect (tempMsg);

   /* Report the DB timing measurement statistics before exiting. */
   if ( DB_Timing_Flag == true )
   {
      num_sql = log_db_timing_statistics_report( &TimingStats );

      memset( buffer, 0x00, sizeof(buffer) );
      sprintf( buffer,
              "%s logged stats for %d SQLs",
               AppName, num_sql );
      LogMsg( buffer, INFO_MSG );
   }

	sprintf( buffer, "IMPORT: Process completed, version %s, date %s, time %s",
			Version, date_str, time_str );
	LogMsg( buffer, INFO_MSG );
	

	ShutDownMsgLogging();

} /*end of main.*/ 


/**************************farad*********************/


/*************************************************************************************
       NAME:           parseTerminalHeader()

       DESCRIPTION:    This module is parses the header of the terminal ascii file.
					        It populates the myTerminalHeader structure.
                       
       INPUTS:         dataRecord is CHAR array which containes the header record 
                       from the terminal file.
	    OUTPUTS:
                       structure of type myTerminalHeader 
       RETURNS:        
		   None.                       
                               
      AUTHOR:          Faramarz Arad.
      MODIFIED BY:     None.
*************************************************************************************/
void parseTerminalHeader(pCHAR dataRecord, pmyTerminalHeader termHeadRecord)
{
   memset(termHeadRecord,0,sizeof(myTerminalHeader));

   strncpy(termHeadRecord->headerRecordType     , dataRecord      ,sizeof(termHeadRecord->headerRecordType     ) - 1);
   strncpy(termHeadRecord->headerFileName       , dataRecord+3    ,sizeof(termHeadRecord->headerFileName       ) - 1);
   strncpy(termHeadRecord->headerFileCreateDate , dataRecord + 11 ,sizeof(termHeadRecord->headerFileCreateDate ) - 1);
   strncpy(termHeadRecord->headerFiller         , dataRecord+17   ,sizeof(termHeadRecord->headerFiller         ) - 1);     
 
   /*the next command will make sure the input has numbers only.*/
   returnNumbers(termHeadRecord->headerFileCreateDate);

}  /*end of parseTerminalHeader*/

/*************************************************************************************
       NAME:           parseTerminalDetail()

       DESCRIPTION:    This module parses the detail record.
					   
                       
       INPUTS:         dataRecord is CHAR array which containes the detail record 
                       from the terminal file.
       OUTPUTS:
                       structure of type myTerminalDetail.
       RETURNS:        
		   None.                       
                               
      AUTHOR:         
      MODIFIED BY:    None.
*************************************************************************************/
void parseTerminalDetail(pCHAR dataRecord, pDCF01 dcf01)
{
	pRawTerminalDetail pRTD;
	BYTE               device_type[2] = "";
	CHAR   temp_data[100] = {0};

	memset( dcf01, 0x00, sizeof(DCF01) );
	pRTD = (pRawTerminalDetail)dataRecord;


	if(true==isalphanumSpace(pRTD->detailTerminalNumber, 8))//N -> 8
	{
		memcpy((pCHAR)dcf01->primary_key.device_id ,pRTD->detailTerminalNumber ,  sizeof(dcf01->primary_key.device_id ) - 1);
	}
	if(true==isnumSpace(pRTD->detailMerchantNumber, 15))//N -> 15
	{
		memcpy((pCHAR)dcf01->merchant_id           ,pRTD->detailMerchantNumber ,  sizeof(dcf01->merchant_id           ) - 1);
	}
	if(true==isnumSpace(pRTD->detailOrganizaNumber, 4))//N -> 4
	{
		memcpy((pCHAR)dcf01->organization_id       ,pRTD->detailOrganizaNumber ,  sizeof(dcf01->organization_id       ) - 1); 
	}
	if(true==isnumSpace(pRTD->detailIndustryCode, 3))//N -> 3
	{
		memcpy((pCHAR)dcf01->industry_code         ,pRTD->detailIndustryCode   ,  sizeof(dcf01->industry_code         ) - 1);
	}
	if(true==isnumSpace(pRTD->detailMCC , 4))//N -> 4
	{
		memcpy((pCHAR)dcf01->category_code         ,pRTD->detailMCC            ,  sizeof(dcf01->category_code         ) - 1);
	}
	if(true==isAlpha(pRTD->detailStatus, 1))//A -> 1
	{
		memcpy((pCHAR)dcf01->status                ,pRTD->detailStatus         ,  sizeof(dcf01->status                ) - 1);
	}
	if(true==isAlpha(pRTD->detailTotalIndicator, 1))//A -> 1
	{
		memcpy((pCHAR)dcf01->totals_ind            ,pRTD->detailTotalIndicator ,  sizeof(dcf01->totals_ind            ) - 1);
	}
	if(true==isAlpha(pRTD->detailStatusIndicator, 1))//A -> 1
	{
		memcpy((pCHAR)dcf01->statistics_ind        ,pRTD->detailStatusIndicator,  sizeof(dcf01->statistics_ind        ) - 1);
	}
	if(true==isalphanumSpace(pRTD->detailAmexCharDesc, 23))//A/N -> 23
	{
		memcpy((pCHAR)dcf01->amex_charge_desc      ,pRTD->detailAmexCharDesc   ,  sizeof(dcf01->amex_charge_desc      ) - 1);
	}
	if(true==isalphanumSpace(pRTD->detailDinnCharDesc, 23))//A/N -> 23
	{
		memcpy((pCHAR)dcf01->diners_charge_desc    ,pRTD->detailDinnCharDesc   ,  sizeof(dcf01->diners_charge_desc    ) - 1);
	}
	if(true==isalphanumSpace(pRTD->detailValidTrans , 40))//A/N -> 40
	{
		memcpy((pCHAR)dcf01->valid_transactions    ,pRTD->detailValidTrans     ,  sizeof(dcf01->valid_transactions    ) - 1);
	}
	if(true==isnumSpace(pRTD->detailAutoInitAct, 1))//N -> 1
	{
		memcpy((pCHAR)dcf01->auto_init_activation	        ,pRTD->detailAutoInitAct          , sizeof(dcf01->auto_init_activation         ) - 1);	
	}
	if(true==isnumSpace(pRTD->detailAutoInitTime, 4))//N -> 4
	{
		memcpy((pCHAR)dcf01->auto_init_time			        ,pRTD->detailAutoInitTime         , sizeof(dcf01->auto_init_time               ) - 2);
	}
	if(true==isalphanumSpace(pRTD->detailAutoInitRefNbr, 6))//A/N -> 6
	{
		memcpy((pCHAR)dcf01->auto_init_ref_number	        ,pRTD->detailAutoInitRefNbr       , sizeof(dcf01->auto_init_ref_number         ) - 1);
	}
	if(true==isnumSpace(pRTD->detailAutoProgramLoadAct, 1))//N ->1
	{
		memcpy((pCHAR)dcf01->auto_program_load_activation ,pRTD->detailAutoProgramLoadAct   , sizeof(dcf01->auto_program_load_activation ) - 1);
	}
	if(true==isnumSpace(pRTD->detailAutoProgramLoadTime, 4))//N -> 4
	{
		memcpy((pCHAR)dcf01->auto_program_load_time       ,pRTD->detailAutoProgramLoadTime  , sizeof(dcf01->auto_program_load_time       ) - 2);
	}
	if(true==isalphanumSpace(pRTD->detailAutoProgramLoadRefNbr, 6))//A/N -> 6
	{
		memcpy((pCHAR)dcf01->auto_program_load_ref_number ,pRTD->detailAutoProgramLoadRefNbr, sizeof(dcf01->auto_program_load_ref_number ) - 1);
	}
	if(true==isnumSpace(pRTD->detailAutoIcepacAct, 1))//N -> 1
	{
		memcpy((pCHAR)dcf01->auto_icepac_activation       ,pRTD->detailAutoIcepacAct        , sizeof(dcf01->auto_icepac_activation       ) - 1);
	}
	//if(true==isalphanumSpace(pRTD->detailTPKValue,TermKeyLength ))//A/N ->
	//{
		memcpy((pCHAR)dcf01->cwk				   ,pRTD->detailTPKValue	   ,  TermKeyLength);
		/*if(TermKeyLength == MAX_TPKV_LEN)
		{
			// No need to read data as data allinment with pRTD structure
		}
		else
		{
			// We need to store other filed data
			memcpy(temp_data,&pRTD->detailTPKValue[TermKeyLength], (sizeof (pRTD->detailTPKValue) -TermKeyLength));
			if( isalnum(temp_data[0]))
			{
				memcpy(pRTD->detailDeviceType,temp_data, sizeof(pRTD->detailDeviceType));
				memcpy(pRTD->detailEmvCapability,temp_data + sizeof(pRTD->detailDeviceType), sizeof(pRTD->detailEmvCapability));
			}
			else
			{
				memcpy(pRTD->detailEmvCapability,temp_data , sizeof(pRTD->detailEmvCapability));
			}
		}*/
	//}

	strcpy((pCHAR)dcf01->pin_block_format	   ,"1");

	/* Device Type
	*
	* This consist of 2 chars. We get only the last char
	* in the import file. In database, first char is 0.
	*/
	memcpy( device_type, pRTD->detailDeviceType, 1 );
	if ( isalnum(device_type[0]) )
	{
		dcf01->device_type[0] = '0';            // Set 1st char to zero.
		dcf01->device_type[1] = device_type[0]; // Value from file.
	}
	else
	{
		memcpy( dcf01->device_type, "03", 2 ); // Default to ISO Device
	}

	//Store EMV capability indicator
	//memcpy((pCHAR)dcf01->current_bus_date, pRTD->detailEmvCapability, sizeof(pRTD->detailEmvCapability));
	/* The next set of commands will make sure the inputs are all numbers. */
	returnNumbers((pCHAR)dcf01->primary_key.device_id       );
	returnNumbers((pCHAR)dcf01->merchant_id                 );
	returnNumbers((pCHAR)dcf01->organization_id             );
	returnNumbers((pCHAR)dcf01->industry_code               );
	returnNumbers((pCHAR)dcf01->category_code               );
	returnNumbers((pCHAR)dcf01->valid_transactions          );
	returnNumbers((pCHAR)dcf01->auto_init_activation        );
	returnNumbers((pCHAR)dcf01->auto_init_time              );
	returnNumbers((pCHAR)dcf01->auto_program_load_activation);
	returnNumbers((pCHAR)dcf01->auto_program_load_time      );
	returnNumbers((pCHAR)dcf01->auto_icepac_activation      );
	returnNumbers((pCHAR)dcf01->rental_program[0]      );

	return;
} /*end of parseTerminalDetail*/

/*************************************************************************************
       NAME:           parseTerminalTrailer()

       DESCRIPTION:    This module parses the trailer record. 
					   
                       
       INPUTS:         dataRecord is CHAR array which containes the detail record 
                       from the terminal file.
       OUTPUTS:
                       structure of type myTerminalDetail.
       RETURNS:        
		   None.                       
                               
      AUTHOR:         
      MODIFIED BY:    None.
*************************************************************************************/
void parseTerminalTrailer(pCHAR dataRecord, pmyTerminalTrailer termTrailRecord)
{
   memset(termTrailRecord,0,sizeof(myTerminalTrailer));

   strncpy((pCHAR)termTrailRecord->trailRecordType     , dataRecord     , sizeof(termTrailRecord->trailRecordType     ) - 1);
   strncpy((pCHAR)termTrailRecord->trailFileName       , dataRecord + 3 , sizeof(termTrailRecord->trailFileName       ) - 1);
   strncpy((pCHAR)termTrailRecord->trailFileCreateDate , dataRecord + 11, sizeof(termTrailRecord->trailFileCreateDate ) - 1);
   strncpy((pCHAR)termTrailRecord->trailNumberofRecords, dataRecord + 17, sizeof(termTrailRecord->trailNumberofRecords) - 1);
   strncpy((pCHAR)termTrailRecord->trailFiller         , dataRecord + 26, sizeof(termTrailRecord->trailFiller         ) - 1);

   /*the next command would make sure all of the input are numbers.*/
   returnNumbers((pCHAR)termTrailRecord->trailFileCreateDate);
   returnNumbers((pCHAR)termTrailRecord->trailNumberofRecords);

}  /*end of parseTerminalTrailer*/


/*************************************************************************************
       NAME:           parsePrimaryHeader()

       DESCRIPTION:    This module is parses the header of the terminal ascii file.
					        It populates the myPrimarylHeader structure.
                       
       INPUTS:         dataRecord is CHAR array which containes the header record 
                       from the primary file.
	    OUTPUTS:
                       structure of type myPrimaryHeader 
       RETURNS:        
		   None.                       
                               
      AUTHOR:          Faramarz Arad.
      MODIFIED BY:     None.
*************************************************************************************/
void parsePrimaryHeader(pCHAR dataRecord, pmyPrimaryHeader primHeadRecord)
{
   memset(primHeadRecord,0,sizeof(myPrimaryHeader));

   strncpy((pCHAR)primHeadRecord->headRecordType ,    dataRecord,      sizeof(primHeadRecord->headRecordType    ) - 1);
   strncpy((pCHAR)primHeadRecord->headFileName,       dataRecord + 3,  sizeof(primHeadRecord->headFileName      ) - 1);
   strncpy((pCHAR)primHeadRecord->headFileCreateDate, dataRecord + 11, sizeof(primHeadRecord->headFileCreateDate) - 1);
   strncpy((pCHAR)primHeadRecord->headFiller,         dataRecord + 17, sizeof(primHeadRecord->headFiller        ) - 1);     

   /*the next command would make sure the inputs are numbers.*/
   returnNumbers(primHeadRecord->headFileCreateDate);

}  /*end of parsePrimaryHeader*/
   
/*************************************************************************************
       NAME:           parsePrimaryDetail()

       DESCRIPTION:    This module is parses the detail of the primary ascii file.
					        It populates the myPrimarylDetail structure.
                       
       INPUTS:         dataRecord is CHAR array which containes the header record 
                       from the primary file.
	    OUTPUTS:
                       structure of type myPrimaryDetail 
       RETURNS:        
		   None.                       
                               
      AUTHOR:          Faramarz Arad.
      MODIFIED BY:     None.
*************************************************************************************/
void parsePrimaryDetail(pCHAR dataRecord, pPAF01 paf01)
{
   pRawPrimaryDetail pRPD;

   memset(paf01, 0, sizeof(PAF01));
   pRPD = (pRawPrimaryDetail)dataRecord;

	if(true==isalphanumSpace(pRPD->detailAccount, 19))
	{
		strncpy((pCHAR)paf01->primary_key.primary_acct_nbr  , pRPD->detailAccount      ,sizeof(paf01->primary_key.primary_acct_nbr) - 1);
	}
	if(true==isalphanumSpace(pRPD->detailNameDBA, 150 ))
	{
		strncpy((pCHAR)paf01->name_dba                      , pRPD->detailNameDBA      ,sizeof(paf01->name_dba       ) - 1);
	}
	if(true==isalphanumSpace(pRPD->detailAddres1, 30))
	{
		strncpy((pCHAR)paf01->address1                      , pRPD->detailAddres1      ,sizeof(paf01->address1       ) - 1);
	}
	if(true==isalphanumSpace(pRPD->detailAddres2, 30))
	{
		strncpy((pCHAR)paf01->address2                      , pRPD->detailAddres2      ,sizeof(paf01->address2       ) - 1);
	}
	if(true==isalphanumSpace(pRPD->detailCity, 20))
	{
		strncpy((pCHAR)paf01->city                          , pRPD->detailCity         ,sizeof(paf01->city           ) - 1);
	}
	if(true==isalphanumSpace(pRPD->detailState, 10))
	{
		strncpy((pCHAR)paf01->state                         , pRPD->detailState        ,sizeof(paf01->state          ) - 1);
	}
	if(true==isalphanumSpace(pRPD->detailZipCode, 10))
	{
		strncpy((pCHAR)paf01->zip_code                      , pRPD->detailZipCode      ,sizeof(paf01->zip_code       ) - 1);
	}
	if(true==isalphanumSpace(pRPD->detailHomePhone, 30))
	{
		strncpy((pCHAR)paf01->home_phone                    , pRPD->detailHomePhone    ,sizeof(paf01->home_phone     ) - 1);
	}
	if(true==isalphanumSpace(pRPD->detailWorkPhone, 30))
	{
		strncpy((pCHAR)paf01->work_phone                    , pRPD->detailWorkPhone    ,sizeof(paf01->work_phone     ) - 1);
	}
	if(true==isnum(pRPD->detailStatus, 2))
	{
		strncpy((pCHAR)paf01->status                        , pRPD->detailStatus       ,sizeof(paf01->status         ) - 1);
	}
	if(true==isalphanum(pRPD->detailCafProfileId, 4))
	{
		strncpy((pCHAR)paf01->caf_profile_id                , pRPD->detailCafProfileId ,sizeof(paf01->caf_profile_id   ) - 1);
	}
	if(true==isalphanum(pRPD->detailtype, 1))
	{
		strncpy((pCHAR)paf01->type                  		   , pRPD->detailtype  		  ,sizeof(paf01->type   ) - 1);
	}
	if(true==isalphanumSpace(pRPD->detailCoorAccountNo, 10))
	{
		strncpy((pCHAR)paf01->corp_acct_nbr                 , pRPD->detailCoorAccountNo,sizeof(paf01->corp_acct_nbr  ) - 1);
	}
	if(true==isalphanumSpace(pRPD->detailBlockStatus, 4))
	{
		strncpy((pCHAR)paf01->block_status                  , pRPD->detailBlockStatus  ,sizeof(paf01->block_status   ) - 1);
	}
	if(true==isalphanumSpace(pRPD->detailAcountName, 30))
	{
		strncpy((pCHAR)paf01->account_name                  , pRPD->detailAcountName   ,sizeof(paf01->account_name   ) - 1);
	}
	if(true==isalphanumSpace(pRPD->detailPostDueAmount, 12))
	{
		strncpy((pCHAR)paf01->past_due_amount               , pRPD->detailPostDueAmount,sizeof(paf01->past_due_amount) - 1);
	}
	if(true==isnum(pRPD->detailMinPayCode, 1))
	{
		strncpy((pCHAR)paf01->min_pay_code                  , pRPD->detailMinPayCode   ,sizeof(paf01->min_pay_code   ) - 1);
	}
	if(true==isnumSpace(pRPD->detailCredit_limit, 10))
	{
		strncpy((pCHAR)paf01->Credit_limit				   , pRPD->detailCredit_limit ,sizeof(paf01->Credit_limit   ) - 1);
	}
	if(true==isalphanumSpace(pRPD->detailCustr_nbr, 12))
	{
		strncpy((pCHAR)paf01->Custr_nbr					   , pRPD->detailCustr_nbr    ,sizeof(paf01->Custr_nbr      ) - 1);
	}
	if(true==isalphanumSpace(pRPD->detailAPP_Source, 10))
	{
		strncpy((pCHAR)paf01->APP_Source					   , pRPD->detailAPP_Source   ,sizeof(paf01->APP_Source     ) - 1);
	}
	if(true==isalphanumSpace(pRPD->detailCOMP_Name, 100))
	{
		strncpy((pCHAR)paf01->COMP_Name					   , pRPD->detailCOMP_Name    ,sizeof(paf01->COMP_Name      ) - 1);
	}

   /*the next command would make sure the input are numbers.*/
   returnNumbers((pCHAR)paf01->primary_key.primary_acct_nbr);
   returnNumbers((pCHAR)paf01->corp_acct_nbr);
   returnNumbers((pCHAR)paf01->past_due_amount);
   returnNumbers((pCHAR)paf01->Credit_limit);

}  /*end of parsePrimaryDetail*/

/*************************************************************************************
       NAME:           parsePrimaryTrailer()

       DESCRIPTION:    This module parses the trailer of the primary ascii file.
					        It populates the myPrimaryTrailer structure.
                       
       INPUTS:         dataRecord is CHAR array which containes the header record 
                       from the primary file.
	    OUTPUTS:
                       structure of type myPrimaryTrailer
       RETURNS:        
		   None.                       
                               
      AUTHOR:          Faramarz Arad.
      MODIFIED BY:     None.
*************************************************************************************/
void parsePrimaryTrailer(pCHAR dataRecord, pmyPrimaryTrailer primTrailRecord )
{
   memset(primTrailRecord,0,sizeof(myPrimaryTrailer));

   strncpy((pCHAR)primTrailRecord->trailRecordType     , dataRecord     , sizeof(primTrailRecord->trailRecordType)     -1);
   strncpy((pCHAR)primTrailRecord->trailFileName       , dataRecord + 3 , sizeof(primTrailRecord->trailFileName)       -1);
   strncpy((pCHAR)primTrailRecord->trailFileCreateDate , dataRecord + 11, sizeof(primTrailRecord->trailFileCreateDate) -1);
   strncpy((pCHAR)primTrailRecord->trailNumberofRecord , dataRecord + 17, sizeof(primTrailRecord->trailNumberofRecord) -1);
   strncpy((pCHAR)primTrailRecord->trailFiller         , dataRecord + 26, sizeof(primTrailRecord->trailFiller)         -1);

   /*the next command would make sure the inputs are numbers.*/
   returnNumbers((pCHAR)primTrailRecord->trailFileCreateDate);
   returnNumbers((pCHAR)primTrailRecord->trailNumberofRecord);

}  /*end of parsePrimaryTrailer*/

/*************************************************************************************
       NAME:           parseCardHoldHeader()

       DESCRIPTION:    This module parses the header of the cardholder ascii file.
					        It populates the myCardHoldHeader structure.
                       
       INPUTS:         dataRecord is CHAR array which containes the header record 
                       from the cardholder file.
	    OUTPUTS:
                       structure of type myCardHoldHeader
       RETURNS:        
		   None.                       
                               
      AUTHOR:          Faramarz Arad.
      MODIFIED BY:     None.
*************************************************************************************/
void parseCardHoldHeader(pCHAR dataRecord, pmyCardHoldHeader cardHoldHeadRecord )
{
   memset(cardHoldHeadRecord, 0, sizeof(myCardHoldHeader));

   strncpy((pCHAR)cardHoldHeadRecord->headRecordType ,    dataRecord,      sizeof(cardHoldHeadRecord->headRecordType)-1);
   strncpy((pCHAR)cardHoldHeadRecord->headFileName,       dataRecord + 3,  sizeof(cardHoldHeadRecord->headFileName)-1);
   strncpy((pCHAR)cardHoldHeadRecord->headFileCreateDate, dataRecord + 11, sizeof(cardHoldHeadRecord->headFileCreateDate)-1);
   strncpy((pCHAR)cardHoldHeadRecord->headFiller,         dataRecord + 17, sizeof(cardHoldHeadRecord->headFiller)-1);     

   /*the next command would make sure the inputs are numbers.*/
   returnNumbers((pCHAR)cardHoldHeadRecord->headFileCreateDate);

}  /*end of parseCardHoldHeader*/

/*************************************************************************************
       NAME:           parseCardHoldHeader()

       DESCRIPTION:    This module parses the header of the cardholder ascii file.
					        It populates the myCardHoldHeader structure.
                       
       INPUTS:         dataRecord is CHAR array which containes the header record 
                       from the cardholder file.
	    OUTPUTS:
                       structure of type myCardHoldHeader
       RETURNS:        
		   None.                       
                               
      AUTHOR:          Faramarz Arad.
      MODIFIED BY:     None.
*************************************************************************************/
void parseCardHoldDetail(pCHAR dataRecord, pmyCardHoldDetail cardHoldDetailRec)
{
   pRawCardHoldDetail pRCHD;

   memset(cardHoldDetailRec,0,sizeof(myCardHoldDetail));
   pRCHD = (pRawCardHoldDetail)dataRecord;

   strncpy((pCHAR)cardHoldDetailRec->detailRecordType      ,pRCHD->detailRecordType      ,sizeof(cardHoldDetailRec->detailRecordType      ) - 1);
   strncpy((pCHAR)cardHoldDetailRec->detailCardType        ,pRCHD->detailCardType        ,sizeof(cardHoldDetailRec->detailCardType        ) - 1);
   strncpy((pCHAR)cardHoldDetailRec->detailCardNumber      ,pRCHD->detailCardNumber      ,sizeof(cardHoldDetailRec->detailCardNumber      ) - 1);
   strncpy((pCHAR)cardHoldDetailRec->detailStatus          ,pRCHD->detailStatus          ,sizeof(cardHoldDetailRec->detailStatus          ) - 1);
   strncpy((pCHAR)cardHoldDetailRec->detailPrimaryAccount  ,pRCHD->detailPrimaryAccount  ,sizeof(cardHoldDetailRec->detailPrimaryAccount  ) - 1);
   strncpy((pCHAR)cardHoldDetailRec->detailNameDBA         ,pRCHD->detailNameDBA         ,sizeof(cardHoldDetailRec->detailNameDBA         ) - 1);
   strncpy((pCHAR)cardHoldDetailRec->detailAddres1         ,pRCHD->detailAddres1         ,sizeof(cardHoldDetailRec->detailAddres1         ) - 1);
   strncpy((pCHAR)cardHoldDetailRec->detailAddres2         ,pRCHD->detailAddres2         ,sizeof(cardHoldDetailRec->detailAddres2         ) - 1);
   strncpy((pCHAR)cardHoldDetailRec->detailCity            ,pRCHD->detailCity            ,sizeof(cardHoldDetailRec->detailCity            ) - 1);
   strncpy((pCHAR)cardHoldDetailRec->detailState           ,pRCHD->detailState           ,sizeof(cardHoldDetailRec->detailState           ) - 1);
   strncpy((pCHAR)cardHoldDetailRec->detailZipCode         ,pRCHD->detailZipCode         ,sizeof(cardHoldDetailRec->detailZipCode         ) - 1);
   strncpy((pCHAR)cardHoldDetailRec->detailCounty          ,pRCHD->detailCounty          ,sizeof(cardHoldDetailRec->detailCounty          ) - 1);
   strncpy((pCHAR)cardHoldDetailRec->detailHomePhone       ,pRCHD->detailHomePhone       ,sizeof(cardHoldDetailRec->detailHomePhone       ) - 1);
   strncpy((pCHAR)cardHoldDetailRec->detailWorkPhone       ,pRCHD->detailWorkPhone       ,sizeof(cardHoldDetailRec->detailWorkPhone       ) - 1);
   strncpy((pCHAR)cardHoldDetailRec->detailDateBirth       ,pRCHD->detailDateBirth       ,sizeof(cardHoldDetailRec->detailDateBirth       ) - 1);
   strncpy((pCHAR)cardHoldDetailRec->detailProfession      ,pRCHD->detailProfession      ,sizeof(cardHoldDetailRec->detailProfession      ) - 1);
   strncpy((pCHAR)cardHoldDetailRec->detailSpeciCode       ,pRCHD->detailSpeciCode       ,sizeof(cardHoldDetailRec->detailSpeciCode       ) - 1);
   strncpy((pCHAR)cardHoldDetailRec->detailVIPAccountCode  ,pRCHD->detailVIPAccountCode  ,sizeof(cardHoldDetailRec->detailVIPAccountCode  ) - 1);
   strncpy((pCHAR)cardHoldDetailRec->detailSex             ,pRCHD->detailSex             ,sizeof(cardHoldDetailRec->detailSex             ) - 1);
   strncpy((pCHAR)cardHoldDetailRec->detailMothMaidenName  ,pRCHD->detailMothMaidenName  ,sizeof(cardHoldDetailRec->detailMothMaidenName  ) - 1);
   strncpy((pCHAR)cardHoldDetailRec->detailMembershipID    ,pRCHD->detailMembershipID    ,sizeof(cardHoldDetailRec->detailMembershipID    ) - 1);
   strncpy((pCHAR)cardHoldDetailRec->detailCardHolder      ,pRCHD->detailCardHolder      ,sizeof(cardHoldDetailRec->detailCardHolder      ) - 1);
   strncpy((pCHAR)cardHoldDetailRec->detailBlockStatus     ,pRCHD->detailBlockStatus     ,sizeof(cardHoldDetailRec->detailBlockStatus     ) - 1);
   strncpy((pCHAR)cardHoldDetailRec->detailPinOffSet       ,pRCHD->detailPinOffSet       ,sizeof(cardHoldDetailRec->detailPinOffSet       ) - 1);
   strncpy((pCHAR)cardHoldDetailRec->detailPesoSaveAccount ,pRCHD->detailPesoSaveAccount ,sizeof(cardHoldDetailRec->detailPesoSaveAccount ) - 1);
   strncpy((pCHAR)cardHoldDetailRec->detailPesoCurrAccount ,pRCHD->detailPesoCurrAccount ,sizeof(cardHoldDetailRec->detailPesoCurrAccount ) - 1);
   strncpy((pCHAR)cardHoldDetailRec->detailDollarSaveAcc   ,pRCHD->detailDollarSaveAcc   ,sizeof(cardHoldDetailRec->detailDollarSaveAcc   ) - 1);
   strncpy((pCHAR)cardHoldDetailRec->detailDollarCurrAcc   ,pRCHD->detailDollarCurrAcc   ,sizeof(cardHoldDetailRec->detailDollarCurrAcc   ) - 1);
   strncpy((pCHAR)cardHoldDetailRec->detailProductCode     ,pRCHD->detailProductCode     ,sizeof(cardHoldDetailRec->detailProductCode     ) - 1);
   strncpy((pCHAR)cardHoldDetailRec->detailNumberofCard    ,pRCHD->detailNumberofCard    ,sizeof(cardHoldDetailRec->detailNumberofCard    ) - 1);
   strncpy((pCHAR)cardHoldDetailRec->detailDateAdded       ,pRCHD->detailDateAdded       ,sizeof(cardHoldDetailRec->detailDateAdded       ) - 1);
   strncpy((pCHAR)cardHoldDetailRec->detailExpirDate       ,pRCHD->detailExpirDate       ,sizeof(cardHoldDetailRec->detailExpirDate       ) - 1);
   strncpy((pCHAR)cardHoldDetailRec->detailEffectiveDate   ,pRCHD->detailEffectiveDate   ,sizeof(cardHoldDetailRec->detailEffectiveDate   ) - 1);
   strncpy((pCHAR)cardHoldDetailRec->detailPesoAccount     ,pRCHD->detailPesoAccount     ,sizeof(cardHoldDetailRec->detailPesoAccount     ) - 1);
   strncpy((pCHAR)cardHoldDetailRec->detailUSDolAccount    ,pRCHD->detailUSDolAccount    ,sizeof(cardHoldDetailRec->detailUSDolAccount    ) - 1);
   strncpy((pCHAR)cardHoldDetailRec->detailDefferedAccont  ,pRCHD->detailDefferedAccont  ,sizeof(cardHoldDetailRec->detailDefferedAccont  ) - 1);
   strncpy((pCHAR)cardHoldDetailRec->detailHouseCardAcc    ,pRCHD->detailHouseCardAcc    ,sizeof(cardHoldDetailRec->detailHouseCardAcc    ) - 1);
   strncpy((pCHAR)cardHoldDetailRec->detailCashBounEligib  ,pRCHD->detailCashBounEligib  ,sizeof(cardHoldDetailRec->detailCashBounEligib  ) - 1);
   strncpy((pCHAR)cardHoldDetailRec->detailVisaPurchCard   ,pRCHD->detailVisaPurchCard   ,sizeof(cardHoldDetailRec->detailVisaPurchCard   ) - 1);
   
   /*added by farad on 3/18/1999 to fix bug #247.*/
   strncpy((pCHAR)cardHoldDetailRec->detailTransAllowed    ,pRCHD->detailTransAllowed    ,sizeof(cardHoldDetailRec->detailTransAllowed    ) - 1);
   memcpy ((pCHAR)cardHoldDetailRec->detailAddOnRate       ,pRCHD->detailAddOnRate       ,sizeof(cardHoldDetailRec->detailAddOnRate       ) - 1);
   memcpy ((pCHAR)cardHoldDetailRec->detailAddOnTerm       ,pRCHD->detailAddOnTerm       ,sizeof(cardHoldDetailRec->detailAddOnTerm       ) - 1);
   memcpy ((pCHAR)cardHoldDetailRec->detailPesoTranLimit   ,pRCHD->detailPesoTranLimit   ,sizeof(cardHoldDetailRec->detailPesoTranLimit   ) - 1);
   memcpy ((pCHAR)cardHoldDetailRec->detailDollarTranLimit ,pRCHD->detailDollarTranLimit ,sizeof(cardHoldDetailRec->detailDollarTranLimit   ) - 1);
   /*TF-Ajay Additional expiry date checking changes Nov-2010 */
   strncpy((pCHAR)cardHoldDetailRec->detailMaint_date       ,pRCHD->detailMaint_date ,SECONDARY_EXP_DATE_LEN);
   strncpy((pCHAR)cardHoldDetailRec->detailPanSeqNbr,		pRCHD->detailPanSeqNbr,2);

//   strncpy((pCHAR)cardHoldDetailRec->detailFiller          ,pRCHD->detailFiller          ,sizeof(cardHoldDetailRec->detailFiller          ) - 1);
	//Store IBM offset for CUP cards
	if(strlen(pRCHD->IBMOffset) > 0)
	{
		memcpy((pCHAR)cardHoldDetailRec->IBMOffset,pRCHD->IBMOffset,sizeof(pRCHD->IBMOffset));
	}
	// Retrive mobile number if its present in card records.
	if(strlen(pRCHD->Mobilenumber))
	{
		memcpy((pCHAR)cardHoldDetailRec->Mobilenumber,pRCHD->Mobilenumber,sizeof(pRCHD->Mobilenumber));
	}

   /*the next command would make sure the inputs are all numbers.*/
   returnNumbers((pCHAR)cardHoldDetailRec->detailCardNumber);
   returnNumbers((pCHAR)cardHoldDetailRec->detailPrimaryAccount);
   returnNumbers((pCHAR)cardHoldDetailRec->detailDateBirth);
   returnNumbers((pCHAR)cardHoldDetailRec->detailCardHolder);
                 
   returnNumbers((pCHAR)cardHoldDetailRec->detailPinOffSet);
   returnNumbers((pCHAR)cardHoldDetailRec->detailPesoSaveAccount);
   returnNumbers((pCHAR)cardHoldDetailRec->detailPesoCurrAccount);
   returnNumbers((pCHAR)cardHoldDetailRec->detailDollarSaveAcc);
   returnNumbers((pCHAR)cardHoldDetailRec->detailDollarCurrAcc);
                 
   returnNumbers((pCHAR)cardHoldDetailRec->detailNumberofCard);
   returnNumbers((pCHAR)cardHoldDetailRec->detailDateAdded);
   returnNumbers((pCHAR)cardHoldDetailRec->detailExpirDate);
   returnNumbers((pCHAR)cardHoldDetailRec->detailEffectiveDate);
   returnNumbers((pCHAR)cardHoldDetailRec->detailPesoAccount);
   returnNumbers((pCHAR)cardHoldDetailRec->detailUSDolAccount);
   returnNumbers((pCHAR)cardHoldDetailRec->detailDefferedAccont);
   returnNumbers((pCHAR)cardHoldDetailRec->detailHouseCardAcc);
   returnNumbers((pCHAR)cardHoldDetailRec->detailAddOnRate);
   returnNumbers((pCHAR)cardHoldDetailRec->detailAddOnTerm);
   returnNumbers((pCHAR)cardHoldDetailRec->detailPesoTranLimit);
   returnNumbers((pCHAR)cardHoldDetailRec->detailDollarTranLimit);
   // TF Phani
   returnNumbers((pCHAR)cardHoldDetailRec->detailMaint_date);
   returnNumbers((pCHAR)cardHoldDetailRec->detailPanSeqNbr);
   returnNumbers((pCHAR)cardHoldDetailRec->Mobilenumber);

}  /*end of parseCardHoldDetail*/

/*************************************************************************************
       NAME:           parsecardHoldTrailer()

       DESCRIPTION:    This module parses the trailer of the cardholder ascii file.
					        It populates the myCardHoldTrailer structure.
                       
       INPUTS:         dataRecord is CHAR array which containes the trailer record 
                       from the cardholder file.
	    OUTPUTS:
                       structure of type myCardHoldHeader
       RETURNS:        
		   None.                       
                               
      AUTHOR:          Faramarz Arad.
      MODIFIED BY:     None.
*************************************************************************************/
void parsecardHoldTrailer(pCHAR dataRecord, pmyCardHoldTrailer cardHoldTrailRec)
{
   memset(cardHoldTrailRec, 0, sizeof(myCardHoldTrailer));

   strncpy((pCHAR)cardHoldTrailRec->trailRecordType     , dataRecord     , sizeof(cardHoldTrailRec->trailRecordType)     -1);
   strncpy((pCHAR)cardHoldTrailRec->trailFileName       , dataRecord + 3 , sizeof(cardHoldTrailRec->trailFileName)       -1);
   strncpy((pCHAR)cardHoldTrailRec->trailFileCreateDate , dataRecord + 11, sizeof(cardHoldTrailRec->trailFileCreateDate) -1);
   strncpy((pCHAR)cardHoldTrailRec->trailNumberofRecord , dataRecord + 17, sizeof(cardHoldTrailRec->trailNumberofRecord) -1);
   strncpy((pCHAR)cardHoldTrailRec->trailFiller         , dataRecord + 26, sizeof(cardHoldTrailRec->trailFiller)         -1);

   /*the next command will make sure the inputs are numbers.*/
   returnNumbers((pCHAR)cardHoldTrailRec->trailFileCreateDate);
   returnNumbers((pCHAR)cardHoldTrailRec->trailNumberofRecord);

}  /*end of parsecardHoldTrailer*/

/*************************************************************************************
       NAME:           parsecardHoldTrailer()

       DESCRIPTION:    This module parses the header of the balance ascii file.
					        It populates the myBalanceHeader structure.
                       
       INPUTS:         dataRecord is CHAR array which containes the header record 
                       from the balance file.
	    OUTPUTS:
                       structure of type myBalanceHeader
       RETURNS:        
		   None.                       
                               
      AUTHOR:          Faramarz Arad.
      MODIFIED BY:     None.
*************************************************************************************/
void parseBalanceHeader(pCHAR dataRecord, pmyBalanceHeader balanceHeadRec)
{
   memset(balanceHeadRec,0,sizeof(myBalanceHeader));

   strncpy((pCHAR)balanceHeadRec->headRecordType ,    dataRecord,      sizeof(balanceHeadRec->headRecordType)-1);
   strncpy((pCHAR)balanceHeadRec->headFileName,       dataRecord + 3,  sizeof(balanceHeadRec->headFileName)-1);
   strncpy((pCHAR)balanceHeadRec->headFileCreateDate, dataRecord + 11, sizeof(balanceHeadRec->headFileCreateDate)-1);
   strncpy((pCHAR)balanceHeadRec->headFiller,         dataRecord + 17, sizeof(balanceHeadRec->headFiller)-1);     

   /*the next command would make sure the inputs are numbers.*/
   returnNumbers((pCHAR)balanceHeadRec->headFileCreateDate);

}  /*end of praseBalanceHeader*/

/*************************************************************************************
       NAME:           parseBalanceDetail()

       DESCRIPTION:    This module parses the detail of the balance ascii file.
					        It populates the myBalanceDetail structure.
                       
       INPUTS:         dataRecord is CHAR array which containes the Detail record 
                       from the balance file.
	    OUTPUTS:
                       structure of type myBalanceDetail
       RETURNS:        
		   None.                       
                               
      AUTHOR:          Faramarz Arad.
      MODIFIED BY:     None.
*************************************************************************************/
void parseBalanceDetail(pCHAR dataRecord, pACF01 acf01)
{
   double   number1=0.0,number2=0.0,number=0.0;
   pCHAR    stopstring;
   pRawBalanceDetail pRBDR;

   memset(acf01, 0, sizeof(ACF01));
   pRBDR = (pRawBalanceDetail)dataRecord;
                 
   strncpy ((pCHAR)acf01->primary_key.account_nbr  ,pRBDR->detailAccountNum , sizeof(acf01->primary_key.account_nbr)  - 1);
   strncpy ((pCHAR)acf01->primary_key.account_type ,pRBDR->detailAccType    , sizeof(acf01->primary_key.account_type) - 1);

   /*added by farad on 3/16/1999 to fix bug #193.*/
   if(pRBDR->detailStatus[0] == 'V')
      strcpy((pCHAR)acf01->status,"00");
   else if(pRBDR->detailStatus[0] == 'I')
      strcpy((pCHAR)acf01->status,"05");
   else  
      strcpy((pCHAR)acf01->status,"");

   strncpy((pCHAR)acf01->date_added        , pRBDR->detailDateAdded  ,sizeof(acf01->date_added      ) - 1);
   strncpy((pCHAR)acf01->credit_limit      , pRBDR->detailCreditLimit,sizeof(acf01->credit_limit    ) - 1);
   strncpy((pCHAR)acf01->outstanding       , pRBDR->detailOutBalance ,sizeof(acf01->outstanding     ) - 1);
   strncpy((pCHAR)acf01->cash_limit        , pRBDR->detailCashLimit  ,sizeof(acf01->cash_limit      ) - 1);
   strncpy((pCHAR)acf01->cash_outstanding  , pRBDR->detailCashOutBal ,sizeof(acf01->cash_outstanding) - 1);

   /*the next commands would make sure all of the inputs are numbers.*/
   returnNumbers((pCHAR)acf01->primary_key.account_nbr );
   returnNumbers((pCHAR)acf01->primary_key.account_type);
   returnNumbers((pCHAR)acf01->date_added              );
   returnNumbers((pCHAR)acf01->credit_limit            );
   returnNumbers((pCHAR)acf01->outstanding             );
   returnNumbers((pCHAR)acf01->cash_limit              );
   returnNumbers((pCHAR)acf01->cash_outstanding        );

   /*convert the values from string to double.*/
   number1 = strtod((pCHAR)acf01->credit_limit,  &stopstring);
   number2 = strtod((pCHAR)acf01->outstanding,   &stopstring);

   /*evalue the acf01.available value and convert it back from double to ascii.*/
   /*The value of number is mutltiplied by 100 to push the decimal points out.*/
   number = (number1 - number2);

   /*modified by farad on 3/16/1999 to fix bug #118*/
   sprintf((pCHAR)acf01->available, "%012.0lf", ((number >= 0.0) ? number : 0.0)  );

   number1 = strtod((pCHAR)acf01->cash_limit        , &stopstring);
   number2 = strtod((pCHAR)acf01->cash_outstanding  , &stopstring);

   number = (number1 - number2);
   
   /*modified by farad on 3/16/1999 to fix bug #118*/
   sprintf((pCHAR)acf01->cash_available, "%012.0lf", ((number >= 0.0) ? number : 0.0));

}  /*end of parseBalanceDetail*/


/*************************************************************************************
       NAME:           parseBalanceTrailer()

       DESCRIPTION:    This module parses the Trailer of the balance ascii file.
					        It populates the myBalanceTrailer structure.
                       
       INPUTS:         dataRecord is CHAR array which containes the trailer record 
                       from the balance file.
	    OUTPUTS:
                       structure of type myBalanceTrailer
       RETURNS:        
		   None.                       
                               
      AUTHOR:          Faramarz Arad.
      MODIFIED BY:     None.
*************************************************************************************/
void parseBalanceTrailer(pCHAR dataRecord, pmyBalanceTrailer balanceTrailRec)
{
   memset(balanceTrailRec,0,sizeof(myBalanceTrailer));

   strncpy((pCHAR)balanceTrailRec->trailRecordType     , dataRecord     , sizeof(balanceTrailRec->trailRecordType)     -1);
   strncpy((pCHAR)balanceTrailRec->trailFileName       , dataRecord + 3 , sizeof(balanceTrailRec->trailFileName)       -1);
   strncpy((pCHAR)balanceTrailRec->trailFileCreateDate , dataRecord + 11, sizeof(balanceTrailRec->trailFileCreateDate) -1);
   strncpy((pCHAR)balanceTrailRec->trailNumberofRecord , dataRecord + 17, sizeof(balanceTrailRec->trailNumberofRecord) -1);
   strncpy((pCHAR)balanceTrailRec->trailFiller         , dataRecord + 26, sizeof(balanceTrailRec->trailFiller)         -1);

   /*the next command would make sure the input are all numbers.*/
   returnNumbers((pCHAR)balanceTrailRec->trailFileCreateDate);
   returnNumbers((pCHAR)balanceTrailRec->trailNumberofRecord);

}  /*end of parseBalanceTrailer*/

/*************************************************************************************
       NAME:           parseMerchantHeader()

       DESCRIPTION:    This module parses the Header of the merchatn ascii file.
					        It populates the myMerchantHeader structure.
                       
       INPUTS:         dataRecord is CHAR array which containes the header record 
                       from the merchatn file.
	    OUTPUTS:
                       structure of type myMerchantHeader
       RETURNS:        
		   None.                       
                               
      AUTHOR:          Faramarz Arad.
      MODIFIED BY:     None.
*************************************************************************************/
void parseMerchantHeader(pCHAR dataRecord, pmyMerchantHeader merchHeadRec)
{
   memset(merchHeadRec,0,sizeof(myMerchantHeader));

   strncpy((pCHAR)merchHeadRec->headRecordType ,    dataRecord,      sizeof(merchHeadRec->headRecordType     ) - 1);
   strncpy((pCHAR)merchHeadRec->headFileName,       dataRecord + 3,  sizeof(merchHeadRec->headFileName       ) - 1);
   strncpy((pCHAR)merchHeadRec->headFileCreateDate, dataRecord + 11, sizeof(merchHeadRec->headFileCreateDate ) - 1);
   strncpy((pCHAR)merchHeadRec->headFiller,         dataRecord + 17, sizeof(merchHeadRec->headFiller         ) - 1);     

   /*the next command would make sure the input is all numbers.*/
   returnNumbers((pCHAR)merchHeadRec->headFileCreateDate);

}  /*end of myMerchantHeader*/

/*************************************************************************************
       NAME:           parseMerchantDetail()

       DESCRIPTION:    This module parses the detail of the merchant ascii file.
					        It populates the myMerchantDetail structure.
                       
       INPUTS:         dataRecord is CHAR array which containes the detail record 
                       from the merchant file.
	    OUTPUTS:
                       structure of type myMerchantDetail
       RETURNS:        
		   None.                       
                               
      AUTHOR:          Faramarz Arad.
      MODIFIED BY:     None.
*************************************************************************************/

void parseMerchantDetail(pCHAR dataRecord, pmyMerchantDetail merchDetailRec)
{
   INT cardTypeAccLength;       
   INT cardTypeMerchIDLength;  
   INT cardTypeCorpIDLength;    
   INT cardTypeAcquiIDLength;   
   INT cardTypeFloorLimitLength;

   pRawMerchantDetail pRMD;

   memset(merchDetailRec,0,sizeof(myMerchantDetail));
   pRMD = (pRawMerchantDetail)dataRecord;

	if(true==isalphanum(pRMD->merchRecordType, 3)) //A/N -> 3
	{
		strncpy((pCHAR)merchDetailRec->merchRecordType       ,pRMD->merchRecordType,    sizeof(merchDetailRec->merchRecordType) - 1);
	}
	if(true==isnumSpace(pRMD->merchantNumber, 15))//N -> 15
	{
		strncpy((pCHAR)merchDetailRec->merchantNumber        ,pRMD->merchantNumber,     sizeof(merchDetailRec->merchantNumber ) - 1);
	}
	if(true==isnumSpace(pRMD->merchOrginNumb ,4))//N -> 4
	{
		strncpy((pCHAR)merchDetailRec->merchOrginNumb        ,pRMD->merchOrginNumb,     sizeof(merchDetailRec->merchOrginNumb ) - 1);
	}
	if(true==isAlpha(pRMD->merchStatus, 1))//A -> 1
	{
		strncpy((pCHAR)merchDetailRec->merchStatus           ,pRMD->merchStatus,        sizeof(merchDetailRec->merchStatus    ) - 1);
	}
	if(true==isalphanumSpaceSpecail(pRMD->merchLegalName, 30)) //A/N -> 30
	{
		strncpy((pCHAR)merchDetailRec->merchLegalName        ,pRMD->merchLegalName,     sizeof(merchDetailRec->merchLegalName ) - 1);
	}
	if(true==isalphanumSpaceSpecail(pRMD->merchDBA, 30))//A/N -> 30
	{
		strncpy((pCHAR)merchDetailRec->merchDBA              ,pRMD->merchDBA,           sizeof(merchDetailRec->merchDBA       ) - 1);
	}
	if(true==isalphanumSpaceSpecail(pRMD->merchAddres1, 30))//A/N -> 30
	{
		strncpy((pCHAR)merchDetailRec->merchAddres1          ,pRMD->merchAddres1,       sizeof(merchDetailRec->merchAddres1   ) - 1);
	}

	if(true==isalphanumSpaceSpecail(pRMD->merchAddres2, 30))//A/N -> 30
	{
		strncpy((pCHAR)merchDetailRec->merchAddres2          ,pRMD->merchAddres2,       sizeof(merchDetailRec->merchAddres2   ) - 1);
	}
	if(true==isalphanumSpaceSpecail(pRMD->merchCity, 18))//A/N -> 18
	{
		strncpy((pCHAR)merchDetailRec->merchCity             ,pRMD->merchCity,          sizeof(merchDetailRec->merchCity      ) - 1);
	}
	if(true==isalphanumSpace(pRMD->merchCounty, 15))//A/N -> 15
	{
		strncpy((pCHAR)merchDetailRec->merchCounty           ,pRMD->merchCounty,        sizeof(merchDetailRec->merchCounty    ) - 1);
	}
	if(true==isalphanumSpace(pRMD->merchState, 2))//A/N -> 2
	{
		strncpy((pCHAR)merchDetailRec->merchState            ,pRMD->merchState,         sizeof(merchDetailRec->merchState     ) - 1);
	}
	if(true==isalphanumSpace(pRMD->merchZipCode, 10))//A/N -> 10
	{
		strncpy((pCHAR)merchDetailRec->merchZipCode          ,pRMD->merchZipCode,       sizeof(merchDetailRec->merchZipCode   ) - 1);
	}
	if(true==isnumSpace(pRMD->merchCurrencyCode, 3))//N -> 3
	{
		strncpy((pCHAR)merchDetailRec->merchCurrencyCode     ,pRMD->merchCurrencyCode,  sizeof(merchDetailRec->merchCurrencyCode   ) - 1);
	}
	if(true==isalphanumSpaceSpecail(pRMD->merchContact, 30))//A/N ->30
	{
		strncpy((pCHAR)merchDetailRec->merchContact          ,pRMD->merchContact,       sizeof(merchDetailRec->merchContact   ) - 1);
	}
	if(true==isnumSpace(pRMD->merchTelphone1,17))//N -> 17
	{
		strncpy((pCHAR)merchDetailRec->merchTelphone1        ,pRMD->merchTelphone1,     sizeof(merchDetailRec->merchTelphone1 ) - 1);
	}
	if(true==isnumSpace(pRMD->merchTelphone2, 17))//N -> 17
	{
		strncpy((pCHAR)merchDetailRec->merchTelphone2        ,pRMD->merchTelphone2,     sizeof(merchDetailRec->merchTelphone2 ) - 1);
	}
	if(true==isnumSpace(pRMD->merchInvoiceBatch, 3))//N -> 3
	{
		strncpy((pCHAR)merchDetailRec->merchInvoiceBatch     ,pRMD->merchInvoiceBatch,  sizeof(merchDetailRec->merchInvoiceBatch   ) - 1);
	}
	if(true==isnumSpace(pRMD->merchInvoiceSubcode, 2))//N ->2
	{
		strncpy((pCHAR)merchDetailRec->merchInvoiceSubcode   ,pRMD->merchInvoiceSubcode,sizeof(merchDetailRec->merchInvoiceSubcode ) - 1);			
	}
	if(true==isnumSpace(pRMD->merchProcContId, 6))//N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchProcContId       ,pRMD->merchProcContId,    sizeof(merchDetailRec->merchProcContId     ) - 1);
	}
	if(true==isnumSpace(pRMD->merchTotalCardType, 3))//N -> 3
	{
		strncpy((pCHAR)merchDetailRec->merchTotalCardType    ,pRMD->merchTotalCardType, sizeof(merchDetailRec->merchTotalCardType  ) - 1);
	}
	if(true==isnumSpace(pRMD->merchRegionNumber, 3))//N -> 3
	{
		strncpy((pCHAR)merchDetailRec->merchRegionNumber     ,pRMD->merchRegionNumber,  sizeof(merchDetailRec->merchRegionNumber   ) - 1);
	}
	if(true==isnumSpace(pRMD->merchDiviNumber,3))//N -> 3
	{
		strncpy((pCHAR)merchDetailRec->merchDiviNumber       ,pRMD->merchDiviNumber,    sizeof(merchDetailRec->merchDiviNumber     ) - 1);
	}
	   /*actual password postion is from 276-283 total 7 bytes, but its not a mandaotry field,
	   		 * therefore password parsing is commented and this password field is used to store th MC_MCO(merchant country of origin)
	   		 *  see the code below of copying MCO*/
	/*if(true==isalphanumSpace(pRMD->merchPassWord,7))//A/N ->7
	{
		strncpy((pCHAR)merchDetailRec->merchPassWord         ,pRMD->merchPassWord,      sizeof(merchDetailRec->merchPassWord       ) - 1);
	}*/
	if(true==isAlpha(pRMD->merchRPS, 1))//A -> 1
	{
		strncpy((pCHAR)merchDetailRec->merchRPS              ,pRMD->merchRPS,           sizeof(merchDetailRec->merchRPS            ) - 1);
	}
	if(true==isAlpha(pRMD->merchHouCardMerch, 1))//A -> 1
	{
		strncpy((pCHAR)merchDetailRec->merchHouCardMerch     ,pRMD->merchHouCardMerch,  sizeof(merchDetailRec->merchHouCardMerch   ) - 1);
	}
	if(true==isAlpha(pRMD->merchProcMerch, 1))//A -> 1
	{
		strncpy((pCHAR)merchDetailRec->merchProcMerch        ,pRMD->merchProcMerch,     sizeof(merchDetailRec->merchProcMerch      ) - 1);	
	}
	if(true==isAlpha(pRMD->merchDeffMerch,1))//A -> 1
	{
		strncpy((pCHAR)merchDetailRec->merchDeffMerch        ,pRMD->merchDeffMerch,     sizeof(merchDetailRec->merchDeffMerch      ) - 1);
	}
	if(true==isAlpha(pRMD->merchCashBonusElg, 1))//A -> 1
	{
		strncpy((pCHAR)merchDetailRec->merchCashBonusElg     ,pRMD->merchCashBonusElg,  sizeof(merchDetailRec->merchCashBonusElg   ) - 1);
	}
	if(true==isnumSpace(pRMD->merchDefMaxAcc, 10))//N -> 10
	{
		strncpy((pCHAR)merchDetailRec->merchDefMaxAcc        ,pRMD->merchDefMaxAcc,     sizeof(merchDetailRec->merchDefMaxAcc      ) - 1);
	}
	if(true==isnumSpace(pRMD->merchDefStartDate, 8))//N -> 8
	{
		strncpy((pCHAR)merchDetailRec->merchDefStartDate     ,pRMD->merchDefStartDate,  sizeof(merchDetailRec->merchDefStartDate   ) - 1);
	}
	if(true==isnumSpace(pRMD->merchDefEndDate, 8))//N -> 8
	{
		strncpy((pCHAR)merchDetailRec->merchDefEndDate       ,pRMD->merchDefEndDate,    sizeof(merchDetailRec->merchDefEndDate     ) - 1);
	}
	if(true==isnumSpace(pRMD->merchDefPayTerm1,3))//N -> 3
	{
		strncpy((pCHAR)merchDetailRec->merchDefPayTerm1      ,pRMD->merchDefPayTerm1,   sizeof(merchDetailRec->merchDefPayTerm1    ) - 1);
	}
	if(true==isnumSpace(pRMD->merchDefFactor1, 7))//N -> 7
	{
		strncpy((pCHAR)merchDetailRec->merchDefFactor1       ,pRMD->merchDefFactor1,    sizeof(merchDetailRec->merchDefFactor1     ) - 1);
	}
	if(true==isnumSpace(pRMD->merchDefMinAmount1, 10))//N -> 10
	{
		strncpy((pCHAR)merchDetailRec->merchDefMinAmount1    ,pRMD->merchDefMinAmount1, sizeof(merchDetailRec->merchDefMinAmount1  ) - 1);
	}
	if(true==isnumSpace(pRMD->merchDefPayTerm2,3))//N -> 3
	{
		strncpy((pCHAR)merchDetailRec->merchDefPayTerm2      ,pRMD->merchDefPayTerm2,   sizeof(merchDetailRec->merchDefPayTerm2    ) - 1);
	}
	if(true==isnumSpace(pRMD->merchDefFactor2, 7))//N -> 7
	{
		strncpy((pCHAR)merchDetailRec->merchDefFactor2       ,pRMD->merchDefFactor2,    sizeof(merchDetailRec->merchDefFactor2     ) - 1);
	}
	if(true==isnumSpace(pRMD->merchDefMinAmount2, 10))//N -> 10
	{
		strncpy((pCHAR)merchDetailRec->merchDefMinAmount2    ,pRMD->merchDefMinAmount2, sizeof(merchDetailRec->merchDefMinAmount2  ) - 1);
	}
	if(true==isnumSpace(pRMD->merchDefPayTerm3, 3))//N -> 3
	{
		strncpy((pCHAR)merchDetailRec->merchDefPayTerm3      ,pRMD->merchDefPayTerm3,   sizeof(merchDetailRec->merchDefPayTerm3    ) - 1);
	}
	if(true==isnumSpace(pRMD->merchDefFactor3, 7))//N -> 7
	{
		strncpy((pCHAR)merchDetailRec->merchDefFactor3       ,pRMD->merchDefFactor3,    sizeof(merchDetailRec->merchDefFactor3     ) - 1);
	}
	if(true==isnumSpace(pRMD->merchDefMinAmount3, 10))//N -> 10
	{
		strncpy((pCHAR)merchDetailRec->merchDefMinAmount3    ,pRMD->merchDefMinAmount3, sizeof(merchDetailRec->merchDefMinAmount3  ) - 1);
	}
	if(true==isnumSpace(pRMD->merchDefPayTerm4, 3))//N -> 3
	{
		strncpy((pCHAR)merchDetailRec->merchDefPayTerm4      ,pRMD->merchDefPayTerm4,   sizeof(merchDetailRec->merchDefPayTerm4    ) - 1);
	}
	if(true==isnumSpace(pRMD->merchDefFactor4, 7))//N -> 7
	{
		strncpy((pCHAR)merchDetailRec->merchDefFactor4       ,pRMD->merchDefFactor4,    sizeof(merchDetailRec->merchDefFactor4     ) - 1);
	}
	if(true==isnumSpace(pRMD->merchDefMinAmount4, 10))//N -> 10
	{
		strncpy((pCHAR)merchDetailRec->merchDefMinAmount4    ,pRMD->merchDefMinAmount4, sizeof(merchDetailRec->merchDefMinAmount4  ) - 1);
	}
	if(true==isnumSpace(pRMD->merchDefPayTerm5, 3))//N -> 3
	{
		strncpy((pCHAR)merchDetailRec->merchDefPayTerm5      ,pRMD->merchDefPayTerm5,   sizeof(merchDetailRec->merchDefPayTerm5    ) - 1);
	}
	if(true==isnumSpace(pRMD->merchDefFactor5, 7))//N  -> 7
	{
		strncpy((pCHAR)merchDetailRec->merchDefFactor5       ,pRMD->merchDefFactor5,    sizeof(merchDetailRec->merchDefFactor5     ) - 1);
	}
	if(true==isnumSpace(pRMD->merchDefMinAmount5, 10))//N -> 10
	{
		strncpy((pCHAR)merchDetailRec->merchDefMinAmount5    ,pRMD->merchDefMinAmount5, sizeof(merchDetailRec->merchDefMinAmount5  ) - 1);
	}
	if(true==isnumSpace(pRMD->merchDefPayTerm6, 3))//N -> 3
	{
		strncpy((pCHAR)merchDetailRec->merchDefPayTerm6      ,pRMD->merchDefPayTerm6,   sizeof(merchDetailRec->merchDefPayTerm6    ) - 1);
	}
	if(true==isnumSpace(pRMD->merchDefFactor6, 7))//N -> 7
	{
		strncpy((pCHAR)merchDetailRec->merchDefFactor6       ,pRMD->merchDefFactor6,    sizeof(merchDetailRec->merchDefFactor6     ) - 1);
	}
	if(true==isnumSpace(pRMD->merchDefMinAmount6, 10))//N -> 10
	{
		strncpy((pCHAR)merchDetailRec->merchDefMinAmount6    ,pRMD->merchDefMinAmount6, sizeof(merchDetailRec->merchDefMinAmount6  ) - 1);
	}
	if(true==isnumSpace(pRMD->merchCredPurcMax, 12))//N -> 12
	{
		strncpy((pCHAR)merchDetailRec->merchCredPurcMax      ,pRMD->merchCredPurcMax,   sizeof(merchDetailRec->merchCredPurcMax    ) - 1);
	}
	if(true==isnumSpace(pRMD->merchCredCashBack, 12))//N -> 12
	{
		strncpy((pCHAR)merchDetailRec->merchCredCashBack     ,pRMD->merchCredCashBack,  sizeof(merchDetailRec->merchCredCashBack   ) - 1);
	}
	if(true==isnumSpace(pRMD->merchCredAuthMax, 12))//N -> 12
	{
		strncpy((pCHAR)merchDetailRec->merchCredAuthMax      ,pRMD->merchCredAuthMax,   sizeof(merchDetailRec->merchCredAuthMax    ) - 1);
	}
	if(true==isnumSpace(pRMD->merchCredRetMax, 12))//N -> 12
	{
		strncpy((pCHAR)merchDetailRec->merchCredRetMax       ,pRMD->merchCredRetMax,    sizeof(merchDetailRec->merchCredRetMax     ) - 1);
	}
	if(true==isnumSpace(pRMD->merchCredCashAdvMax, 12))//N -> 12
	{
		strncpy((pCHAR)merchDetailRec->merchCredCashAdvMax   ,pRMD->merchCredCashAdvMax,sizeof(merchDetailRec->merchCredCashAdvMax ) - 1);
	}
	if(true==isnumSpace(pRMD->merchCredManEntMax, 12))//N -> 12
	{
		strncpy((pCHAR)merchDetailRec->merchCredManEntMax    ,pRMD->merchCredManEntMax, sizeof(merchDetailRec->merchCredManEntMax  ) - 1);
	}
	if(true==isnumSpace(pRMD->merchDebitPurchMax, 12))//N -> 12
	{
		strncpy((pCHAR)merchDetailRec->merchDebitPurchMax    ,pRMD->merchDebitPurchMax, sizeof(merchDetailRec->merchDebitPurchMax  ) - 1);
	}
	if(true==isnumSpace(pRMD->merchDebitCashBackMax, 12))//N -> 12
	{
		strncpy((pCHAR)merchDetailRec->merchDebitCashBackMax ,pRMD->merchDebitCashBackMax, sizeof(merchDetailRec->merchDebitCashBackMax ) - 1);
	}
	if(true==isnumSpace(pRMD->merchDebitAuthMax, 12))//N -> 12
	{
		strncpy((pCHAR)merchDetailRec->merchDebitAuthMax     ,pRMD->merchDebitAuthMax,  sizeof(merchDetailRec->merchDebitAuthMax   ) - 1);
	}
	if(true==isnumSpace(pRMD->merchDebitRetMax, 12))//N -> 12
	{
		strncpy((pCHAR)merchDetailRec->merchDebitRetMax      ,pRMD->merchDebitRetMax,   sizeof(merchDetailRec->merchDebitRetMax    ) - 1);
	}
	if(true==isnumSpace(pRMD->merchDebitCashAdvMax, 12))//N -> 12
	{
		strncpy((pCHAR)merchDetailRec->merchDebitCashAdvMax  ,pRMD->merchDebitCashAdvMax,  sizeof(merchDetailRec->merchDebitCashAdvMax  ) - 1);
	}
	if(true==isnumSpace(pRMD->merchDebitManEntMax, 12))//N -> 12
	{
		strncpy((pCHAR)merchDetailRec->merchDebitManEntMax   ,pRMD->merchDebitManEntMax,   sizeof(merchDetailRec->merchDebitManEntMax   ) - 1);
	}
	if(true==isnumSpace(pRMD->merchDateAdded, 8))//N -> 8
	{
		strncpy((pCHAR)merchDetailRec->merchDateAdded        ,pRMD->merchDateAdded,     sizeof(merchDetailRec->merchDateAdded      ) - 1);
	}
	if(true==isAlpha(pRMD->merchTotalIndicator, 1))//A -> 1
	{
		strncpy((pCHAR)merchDetailRec->merchTotalIndicator   ,pRMD->merchTotalIndicator,   sizeof(merchDetailRec->merchTotalIndicator   ) - 1);
	}
	if(true==isAlpha(pRMD->merchStatsIndicator, 1))//A -> 1
	{
		strncpy((pCHAR)merchDetailRec->merchStatsIndicator   ,pRMD->merchStatsIndicator,   sizeof(merchDetailRec->merchStatsIndicator   ) - 1);
	}
	if(true==isAlphaSpace(pRMD->merchVipcode, 4))//A -> 4
	{
		strncpy((pCHAR)merchDetailRec->merchVipcode          ,pRMD->merchVipcode,       sizeof(merchDetailRec->merchVipcode        ) - 1);
	}
	if(true==isAlpha(pRMD->merchPosCode, 1))//A -> 1
	{
		strncpy((pCHAR)merchDetailRec->merchPosCode          ,pRMD->merchPosCode,       sizeof(merchDetailRec->merchPosCode        ) - 1);
	}
	if(true==isnumSpace(pRMD->merchMCC, 4))//N -> 4
	{
		strncpy((pCHAR)merchDetailRec->merchMCC              ,pRMD->merchMCC,           sizeof(merchDetailRec->merchMCC            ) - 1);
	}
	if(true==isAlpha(pRMD->merchMSTCCcode, 1))//A -> 1
	{
		strncpy((pCHAR)merchDetailRec->merchMSTCCcode        ,pRMD->merchMSTCCcode,     sizeof(merchDetailRec->merchMSTCCcode      ) - 1);
	}
	if(true==isAlpha(pRMD->merchMCFCCcode, 1))//A -> 1
	{
		strncpy((pCHAR)merchDetailRec->merchMCFCCcode        ,pRMD->merchMCFCCcode,     sizeof(merchDetailRec->merchMCFCCcode      ) - 1);
	}
	if(true==isAlphaSpace(pRMD->merchJCPSubCode, 2))//A -> 2
	{
		strncpy((pCHAR)merchDetailRec->merchJCPSubCode       ,pRMD->merchJCPSubCode,    sizeof(merchDetailRec->merchJCPSubCode     ) - 1);
	}
	if(true==isAlphaSpace(pRMD->merchMallCode, 3))//A -> 3
	{
		strncpy((pCHAR)merchDetailRec->merchMallCode         ,pRMD->merchMallCode,      sizeof(merchDetailRec->merchMallCode       ) - 1);
	}
	if(true==isnumSpace(pRMD->merchHCCode, 3))//N -> 3
	{
		strncpy((pCHAR)merchDetailRec->merchHCCode           ,pRMD->merchHCCode,       sizeof(merchDetailRec->merchHCCode         ) - 1);
	}
	if(true==isnumSpace(pRMD->merchSolicCode, 3))//N -> 3
	{
		strncpy((pCHAR)merchDetailRec->merchSolicCode		 ,pRMD->merchSolicCode, 	sizeof(merchDetailRec->merchSolicCode	   ) - 1);	   
	}
	if(true==isalphanumSpace(pRMD->classCode, 3))//A/N -> 3
	{
		strncpy((pCHAR)merchDetailRec->classCode             ,pRMD->classCode,          sizeof(merchDetailRec->classCode           ) - 1);
	}
	if(true==isalphanumSpace(pRMD->merchGroupOutletCode, 5))//A/N -> 5
	{
		strncpy((pCHAR)merchDetailRec->merchGroupOutletCode  ,pRMD->merchGroupOutletCode,sizeof(merchDetailRec->merchGroupOutletCode) - 1);
	}
	if(true==isnumSpace(pRMD->merchGroupType, 4))//N -> 4
	{
		strncpy((pCHAR)merchDetailRec->merchGroupType        ,pRMD->merchGroupType,      sizeof(merchDetailRec->merchGroupType      ) - 1);
	}
   cardTypeAccLength       = sizeof(merchDetailRec->merchCardType1.cardTypeAcc      ) - 1;
   cardTypeMerchIDLength   = sizeof(merchDetailRec->merchCardType1.cardTypeMerchID  ) - 1;
   cardTypeCorpIDLength    = sizeof(merchDetailRec->merchCardType1.cardTypeCorpID   ) - 1;
   cardTypeAcquiIDLength   = sizeof(merchDetailRec->merchCardType1.cardTypeAcquiID  ) - 1;
   cardTypeFloorLimitLength= sizeof(merchDetailRec->merchCardType1.cardTypeFloorLimit)- 1;

	if(true==isAlphaSpace(pRMD->merchCardType1.cardTypeAcc, 4))//A -> 4
	{
		strncpy((pCHAR)merchDetailRec->merchCardType1.cardTypeAcc        , pRMD->merchCardType1.cardTypeAcc     ,  cardTypeAccLength       );
	}
	if(true==isnumSpace(pRMD->merchCardType1.cardTypeMerchID, 16))//N -> 16
	{
		strncpy((pCHAR)merchDetailRec->merchCardType1.cardTypeMerchID    , pRMD->merchCardType1.cardTypeMerchID ,  cardTypeMerchIDLength   );
	}
	if(true==isalphanumSpace(pRMD->merchCardType1.cardTypeCorpID, 6))//A/N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType1.cardTypeCorpID     , pRMD->merchCardType1.cardTypeCorpID  ,  cardTypeCorpIDLength    );
	}
	if(true==isnumSpace(pRMD->merchCardType1.cardTypeAcquiID, 6))//N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType1.cardTypeAcquiID    , pRMD->merchCardType1.cardTypeAcquiID ,  cardTypeAcquiIDLength   );
	}
	if(true==isnumSpace(pRMD->merchCardType1.cardTypeFloorLimit, 12))//N -> 12
	{
		strncpy((pCHAR)merchDetailRec->merchCardType1.cardTypeFloorLimit , pRMD->merchCardType1.cardTypeFloorLimit,cardTypeFloorLimitLength);
	}

	if(true==isAlphaSpace(pRMD->merchCardType2.cardTypeAcc, 4))//A -> 4
	{
		strncpy((pCHAR)merchDetailRec->merchCardType2.cardTypeAcc        , pRMD->merchCardType2.cardTypeAcc     ,  cardTypeAccLength       );
	}
	if(true==isnumSpace(pRMD->merchCardType2.cardTypeMerchID, 16))//N -> 16
	{
		strncpy((pCHAR)merchDetailRec->merchCardType2.cardTypeMerchID    , pRMD->merchCardType2.cardTypeMerchID ,  cardTypeMerchIDLength   );
	}
	if(true==isalphanumSpace(pRMD->merchCardType2.cardTypeCorpID, 6))//A/N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType2.cardTypeCorpID     , pRMD->merchCardType2.cardTypeCorpID  ,  cardTypeCorpIDLength    );
	}
	if(true==isnumSpace(pRMD->merchCardType2.cardTypeAcquiID, 6))//N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType2.cardTypeAcquiID    , pRMD->merchCardType2.cardTypeAcquiID ,  cardTypeAcquiIDLength   );
	}
	if(true==isnumSpace(pRMD->merchCardType2.cardTypeFloorLimit, 12))//N -> 12
	{
		strncpy((pCHAR)merchDetailRec->merchCardType2.cardTypeFloorLimit , pRMD->merchCardType2.cardTypeFloorLimit,cardTypeFloorLimitLength);
	}

	if(true==isAlphaSpace(pRMD->merchCardType3.cardTypeAcc, 4))//A -> 4
	{
		strncpy((pCHAR)merchDetailRec->merchCardType3.cardTypeAcc        , pRMD->merchCardType3.cardTypeAcc     ,  cardTypeAccLength       );
	}
	if(true==isnumSpace(pRMD->merchCardType3.cardTypeMerchID, 16))//N -> 16
	{
		strncpy((pCHAR)merchDetailRec->merchCardType3.cardTypeMerchID    , pRMD->merchCardType3.cardTypeMerchID ,  cardTypeMerchIDLength   );
	}
	if(true==isalphanumSpace(pRMD->merchCardType3.cardTypeCorpID, 6))//A/N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType3.cardTypeCorpID     , pRMD->merchCardType3.cardTypeCorpID  ,  cardTypeCorpIDLength    );
	}
	if(true==isnumSpace(pRMD->merchCardType3.cardTypeAcquiID, 6))//N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType3.cardTypeAcquiID    , pRMD->merchCardType3.cardTypeAcquiID ,  cardTypeAcquiIDLength   );
	}
	if(true==isnumSpace(pRMD->merchCardType3.cardTypeFloorLimit, 12))//N -> 12
	{
		strncpy((pCHAR)merchDetailRec->merchCardType3.cardTypeFloorLimit , pRMD->merchCardType3.cardTypeFloorLimit,cardTypeFloorLimitLength);
	}

	if(true==isAlphaSpace(pRMD->merchCardType4.cardTypeAcc, 4))//A -> 4
	{
		strncpy((pCHAR)merchDetailRec->merchCardType4.cardTypeAcc        , pRMD->merchCardType4.cardTypeAcc     ,  cardTypeAccLength       );
	}
	if(true==isnumSpace(pRMD->merchCardType4.cardTypeMerchID, 16))//N -> 16
	{
		strncpy((pCHAR)merchDetailRec->merchCardType4.cardTypeMerchID    , pRMD->merchCardType4.cardTypeMerchID ,  cardTypeMerchIDLength   );
	}
	if(true==isalphanumSpace(pRMD->merchCardType4.cardTypeCorpID, 6))//A/N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType4.cardTypeCorpID     , pRMD->merchCardType4.cardTypeCorpID  ,  cardTypeCorpIDLength    );
	}
	if(true==isnumSpace(pRMD->merchCardType4.cardTypeAcquiID, 6))//N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType4.cardTypeAcquiID    , pRMD->merchCardType4.cardTypeAcquiID ,  cardTypeAcquiIDLength   );
	}
	if(true==isnumSpace(pRMD->merchCardType4.cardTypeFloorLimit, 12))//N -> 12
	{
		strncpy((pCHAR)merchDetailRec->merchCardType4.cardTypeFloorLimit , pRMD->merchCardType4.cardTypeFloorLimit,cardTypeFloorLimitLength);
	}

	if(true==isAlphaSpace(pRMD->merchCardType5.cardTypeAcc, 4))//A -> 4
	{
		strncpy((pCHAR)merchDetailRec->merchCardType5.cardTypeAcc        , pRMD->merchCardType5.cardTypeAcc     ,  cardTypeAccLength       );
	}
	if(true==isnumSpace(pRMD->merchCardType5.cardTypeMerchID, 16))//N -> 16
	{
		strncpy((pCHAR)merchDetailRec->merchCardType5.cardTypeMerchID    , pRMD->merchCardType5.cardTypeMerchID ,  cardTypeMerchIDLength   );
	}
	if(true==isalphanumSpace(pRMD->merchCardType5.cardTypeCorpID,6))//A/N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType5.cardTypeCorpID     , pRMD->merchCardType5.cardTypeCorpID  ,  cardTypeCorpIDLength    );
	}
	if(true==isnumSpace(pRMD->merchCardType5.cardTypeAcquiID, 6))//N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType5.cardTypeAcquiID    , pRMD->merchCardType5.cardTypeAcquiID ,  cardTypeAcquiIDLength   );
	}
	if(true==isnumSpace(pRMD->merchCardType5.cardTypeFloorLimit,12))//N -> 12
	{
		strncpy((pCHAR)merchDetailRec->merchCardType5.cardTypeFloorLimit , pRMD->merchCardType5.cardTypeFloorLimit,cardTypeFloorLimitLength);
	}

	if(true==isAlphaSpace(pRMD->merchCardType6.cardTypeAcc, 4))//A -> 4
	{
		strncpy((pCHAR)merchDetailRec->merchCardType6.cardTypeAcc        , pRMD->merchCardType6.cardTypeAcc     ,  cardTypeAccLength       );
	}
	if(true==isnumSpace(pRMD->merchCardType6.cardTypeMerchID, 16))//N -> 16
	{
		strncpy((pCHAR)merchDetailRec->merchCardType6.cardTypeMerchID    , pRMD->merchCardType6.cardTypeMerchID ,  cardTypeMerchIDLength   );
	}
	if(true==isalphanumSpace(pRMD->merchCardType6.cardTypeCorpID, 6))//A/N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType6.cardTypeCorpID     , pRMD->merchCardType6.cardTypeCorpID  ,  cardTypeCorpIDLength    );
	}
	if(true==isnumSpace(pRMD->merchCardType6.cardTypeAcquiID, 6))//N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType6.cardTypeAcquiID    , pRMD->merchCardType6.cardTypeAcquiID ,  cardTypeAcquiIDLength   );
	}
	if(true==isnumSpace(pRMD->merchCardType6.cardTypeFloorLimit, 12))//N -> 12
	{
		strncpy((pCHAR)merchDetailRec->merchCardType6.cardTypeFloorLimit , pRMD->merchCardType6.cardTypeFloorLimit,cardTypeFloorLimitLength);
	}

	if(true==isAlphaSpace(pRMD->merchCardType7.cardTypeAcc, 4))//A -> 4
	{
		strncpy((pCHAR)merchDetailRec->merchCardType7.cardTypeAcc        , pRMD->merchCardType7.cardTypeAcc     ,  cardTypeAccLength       );
	}
	if(true==isnumSpace(pRMD->merchCardType7.cardTypeMerchID, 16))//N -> 16
	{
		strncpy((pCHAR)merchDetailRec->merchCardType7.cardTypeMerchID    , pRMD->merchCardType7.cardTypeMerchID ,  cardTypeMerchIDLength   );
	}
	if(true==isalphanumSpace(pRMD->merchCardType7.cardTypeCorpID, 6))//A/N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType7.cardTypeCorpID     , pRMD->merchCardType7.cardTypeCorpID  ,  cardTypeCorpIDLength    );
	}
	if(true==isnumSpace(pRMD->merchCardType7.cardTypeAcquiID, 6))//N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType7.cardTypeAcquiID    , pRMD->merchCardType7.cardTypeAcquiID ,  cardTypeAcquiIDLength   );
	}
	if(true==isnumSpace(pRMD->merchCardType7.cardTypeFloorLimit, 12))//N -> 12
	{
		strncpy((pCHAR)merchDetailRec->merchCardType7.cardTypeFloorLimit , pRMD->merchCardType7.cardTypeFloorLimit,cardTypeFloorLimitLength);
	}

	if(true==isAlphaSpace(pRMD->merchCardType8.cardTypeAcc, 4))//A -> 4
	{
		strncpy((pCHAR)merchDetailRec->merchCardType8.cardTypeAcc        , pRMD->merchCardType8.cardTypeAcc     ,  cardTypeAccLength       );
	}
	if(true==isnumSpace(pRMD->merchCardType8.cardTypeMerchID, 16))//N -> 16
	{
		strncpy((pCHAR)merchDetailRec->merchCardType8.cardTypeMerchID    , pRMD->merchCardType8.cardTypeMerchID ,  cardTypeMerchIDLength   );
	}
	if(true==isalphanumSpace(pRMD->merchCardType8.cardTypeCorpID, 6))//A/N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType8.cardTypeCorpID     , pRMD->merchCardType8.cardTypeCorpID  ,  cardTypeCorpIDLength    );
	}
	if(true==isnumSpace(pRMD->merchCardType8.cardTypeAcquiID, 6))//N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType8.cardTypeAcquiID    , pRMD->merchCardType8.cardTypeAcquiID ,  cardTypeAcquiIDLength   );
	}
	if(true==isnumSpace(pRMD->merchCardType8.cardTypeFloorLimit, 12))//N -> 12
	{
		strncpy((pCHAR)merchDetailRec->merchCardType8.cardTypeFloorLimit , pRMD->merchCardType8.cardTypeFloorLimit,cardTypeFloorLimitLength);
	}


	if(true==isAlphaSpace(pRMD->merchCardType9.cardTypeAcc, 4))//A -> 4
	{
		strncpy((pCHAR)merchDetailRec->merchCardType9.cardTypeAcc        , pRMD->merchCardType9.cardTypeAcc     ,  cardTypeAccLength       );
	}
	if(true==isnumSpace(pRMD->merchCardType9.cardTypeMerchID, 16))//N -> 16
	{
		strncpy((pCHAR)merchDetailRec->merchCardType9.cardTypeMerchID    , pRMD->merchCardType9.cardTypeMerchID ,  cardTypeMerchIDLength   );
	}
	if(true==isalphanumSpace(pRMD->merchCardType9.cardTypeCorpID, 6))//A/N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType9.cardTypeCorpID     , pRMD->merchCardType9.cardTypeCorpID  ,  cardTypeCorpIDLength    );
	}
	if(true==isnumSpace(pRMD->merchCardType9.cardTypeAcquiID, 6))//N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType9.cardTypeAcquiID    , pRMD->merchCardType9.cardTypeAcquiID ,  cardTypeAcquiIDLength   );
	}
	if(true==isnumSpace(pRMD->merchCardType9.cardTypeFloorLimit,12))//N -> 12
	{
		strncpy((pCHAR)merchDetailRec->merchCardType9.cardTypeFloorLimit , pRMD->merchCardType9.cardTypeFloorLimit,cardTypeFloorLimitLength);
	}

	if(true==isAlphaSpace(pRMD->merchCardType10.cardTypeAcc, 4))//A -> 4
	{
		strncpy((pCHAR)merchDetailRec->merchCardType10.cardTypeAcc        , pRMD->merchCardType10.cardTypeAcc     ,  cardTypeAccLength       );
	}
	if(true==isnumSpace(pRMD->merchCardType10.cardTypeMerchID, 16))//N -> 16
	{
		strncpy((pCHAR)merchDetailRec->merchCardType10.cardTypeMerchID    , pRMD->merchCardType10.cardTypeMerchID ,  cardTypeMerchIDLength   );
	}
	if(true==isalphanumSpace(pRMD->merchCardType10.cardTypeCorpID, 6))//A/N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType10.cardTypeCorpID     , pRMD->merchCardType10.cardTypeCorpID  ,  cardTypeCorpIDLength    );
	}
	if(true==isnumSpace(pRMD->merchCardType10.cardTypeAcquiID, 6))//N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType10.cardTypeAcquiID    , pRMD->merchCardType10.cardTypeAcquiID ,  cardTypeAcquiIDLength   );
	}
	if(true==isnumSpace(pRMD->merchCardType10.cardTypeFloorLimit,12))//N -> 12
	{
		strncpy((pCHAR)merchDetailRec->merchCardType10.cardTypeFloorLimit , pRMD->merchCardType10.cardTypeFloorLimit,cardTypeFloorLimitLength);
	}

	if(true==isAlphaSpace(pRMD->merchCardType11.cardTypeAcc, 4))//A -> 4
	{
		strncpy((pCHAR)merchDetailRec->merchCardType11.cardTypeAcc        , pRMD->merchCardType11.cardTypeAcc     ,  cardTypeAccLength       );
	}
	if(true==isnumSpace(pRMD->merchCardType11.cardTypeMerchID, 16))//N -> 16
	{
		strncpy((pCHAR)merchDetailRec->merchCardType11.cardTypeMerchID    , pRMD->merchCardType11.cardTypeMerchID ,  cardTypeMerchIDLength   );
	}
	if(true==isalphanumSpace(pRMD->merchCardType11.cardTypeCorpID, 6))//A/N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType11.cardTypeCorpID     , pRMD->merchCardType11.cardTypeCorpID  ,  cardTypeCorpIDLength    );
	}
	if(true==isnumSpace(pRMD->merchCardType11.cardTypeAcquiID, 6))//N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType11.cardTypeAcquiID    , pRMD->merchCardType11.cardTypeAcquiID ,  cardTypeAcquiIDLength   );
	}
	if(true==isnumSpace(pRMD->merchCardType11.cardTypeFloorLimit, 12))//N -> 12
	{
		strncpy((pCHAR)merchDetailRec->merchCardType11.cardTypeFloorLimit , pRMD->merchCardType11.cardTypeFloorLimit,cardTypeFloorLimitLength);
	}

	if(true==isAlphaSpace(pRMD->merchCardType12.cardTypeAcc, 4))//A -> 4
	{
		strncpy((pCHAR)merchDetailRec->merchCardType12.cardTypeAcc        , pRMD->merchCardType12.cardTypeAcc     ,  cardTypeAccLength       );
	}
	if(true==isnumSpace(pRMD->merchCardType12.cardTypeMerchID, 16))//N -> 16
	{
		strncpy((pCHAR)merchDetailRec->merchCardType12.cardTypeMerchID    , pRMD->merchCardType12.cardTypeMerchID ,  cardTypeMerchIDLength   );
	}
	if(true==isalphanumSpace(pRMD->merchCardType12.cardTypeCorpID, 6))//A/N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType12.cardTypeCorpID     , pRMD->merchCardType12.cardTypeCorpID  ,  cardTypeCorpIDLength    );
	}
	if(true==isnumSpace(pRMD->merchCardType12.cardTypeAcquiID, 6))//N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType12.cardTypeAcquiID    , pRMD->merchCardType12.cardTypeAcquiID ,  cardTypeAcquiIDLength   );
	}
	if(true==isnumSpace(pRMD->merchCardType12.cardTypeFloorLimit, 12))//N -> 12
	{
		strncpy((pCHAR)merchDetailRec->merchCardType12.cardTypeFloorLimit , pRMD->merchCardType12.cardTypeFloorLimit,cardTypeFloorLimitLength);
	}

	if(true==isAlphaSpace(pRMD->merchCardType13.cardTypeAcc, 4))//A -> 4
	{
		strncpy((pCHAR)merchDetailRec->merchCardType13.cardTypeAcc        , pRMD->merchCardType13.cardTypeAcc     ,  cardTypeAccLength       );
	}
	if(true==isnumSpace(pRMD->merchCardType13.cardTypeMerchID, 16))//N -> 16
	{
		strncpy((pCHAR)merchDetailRec->merchCardType13.cardTypeMerchID    , pRMD->merchCardType13.cardTypeMerchID ,  cardTypeMerchIDLength   );
	}
	if(true==isalphanumSpace(pRMD->merchCardType13.cardTypeCorpID, 6))//A/N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType13.cardTypeCorpID     , pRMD->merchCardType13.cardTypeCorpID  ,  cardTypeCorpIDLength    );
	}
	if(true==isnumSpace(pRMD->merchCardType13.cardTypeAcquiID, 6))//N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType13.cardTypeAcquiID    , pRMD->merchCardType13.cardTypeAcquiID ,  cardTypeAcquiIDLength   );
	}
	if(true==isnumSpace(pRMD->merchCardType13.cardTypeFloorLimit,12))//N -> 12
	{
		strncpy((pCHAR)merchDetailRec->merchCardType13.cardTypeFloorLimit , pRMD->merchCardType13.cardTypeFloorLimit,cardTypeFloorLimitLength);
	}

	if(true==isAlphaSpace(pRMD->merchCardType14.cardTypeAcc, 4))//A -> 4
	{
		strncpy((pCHAR)merchDetailRec->merchCardType14.cardTypeAcc        , pRMD->merchCardType14.cardTypeAcc     ,  cardTypeAccLength       );
	}
	if(true==isnumSpace(pRMD->merchCardType14.cardTypeMerchID, 16))//N -> 16
	{
		strncpy((pCHAR)merchDetailRec->merchCardType14.cardTypeMerchID    , pRMD->merchCardType14.cardTypeMerchID ,  cardTypeMerchIDLength   );
	}
	if(true==isalphanumSpace(pRMD->merchCardType14.cardTypeCorpID, 6))//A/N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType14.cardTypeCorpID     , pRMD->merchCardType14.cardTypeCorpID  ,  cardTypeCorpIDLength    );
	}
	if(true==isnumSpace(pRMD->merchCardType14.cardTypeAcquiID, 6))//N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType14.cardTypeAcquiID    , pRMD->merchCardType14.cardTypeAcquiID ,  cardTypeAcquiIDLength   );
	}
	if(true==isnumSpace(pRMD->merchCardType14.cardTypeFloorLimit, 12))//N -> 12
	{
		strncpy((pCHAR)merchDetailRec->merchCardType14.cardTypeFloorLimit , pRMD->merchCardType14.cardTypeFloorLimit,cardTypeFloorLimitLength);
	}


	if(true==isAlphaSpace(pRMD->merchCardType15.cardTypeAcc, 4))//A -> 4
	{
		strncpy((pCHAR)merchDetailRec->merchCardType15.cardTypeAcc        , pRMD->merchCardType15.cardTypeAcc     ,  cardTypeAccLength       );
	}
	if(true==isnumSpace(pRMD->merchCardType15.cardTypeMerchID, 16))//N -> 16
	{
		strncpy((pCHAR)merchDetailRec->merchCardType15.cardTypeMerchID    , pRMD->merchCardType15.cardTypeMerchID ,  cardTypeMerchIDLength   );
	}
	if(true==isalphanumSpace(pRMD->merchCardType15.cardTypeCorpID, 6))//A/N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType15.cardTypeCorpID     , pRMD->merchCardType15.cardTypeCorpID  ,  cardTypeCorpIDLength    );
	}
	if(true==isnumSpace(pRMD->merchCardType15.cardTypeAcquiID, 6))//N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType15.cardTypeAcquiID    , pRMD->merchCardType15.cardTypeAcquiID ,  cardTypeAcquiIDLength   );
	}
	if(true==isnumSpace(pRMD->merchCardType15.cardTypeFloorLimit, 12))//N -> 12
	{
		strncpy((pCHAR)merchDetailRec->merchCardType15.cardTypeFloorLimit , pRMD->merchCardType15.cardTypeFloorLimit,cardTypeFloorLimitLength);
	}

	
	if(true==isAlphaSpace(pRMD->merchCardType16.cardTypeAcc, 4))//A -> 4
	{
		strncpy((pCHAR)merchDetailRec->merchCardType16.cardTypeAcc		 , pRMD->merchCardType16.cardTypeAcc	 ,	cardTypeAccLength		);
	}
	if(true==isnumSpace(pRMD->merchCardType16.cardTypeMerchID, 16))//N -> 16
	{
		strncpy((pCHAR)merchDetailRec->merchCardType16.cardTypeMerchID	 , pRMD->merchCardType16.cardTypeMerchID ,	cardTypeMerchIDLength	);
	}
	if(true==isalphanumSpace(pRMD->merchCardType16.cardTypeCorpID, 6))//A/N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType16.cardTypeCorpID	 , pRMD->merchCardType16.cardTypeCorpID  ,	cardTypeCorpIDLength	);
	}
	if(true==isnumSpace(pRMD->merchCardType16.cardTypeAcquiID, 6))//N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType16.cardTypeAcquiID	 , pRMD->merchCardType16.cardTypeAcquiID ,	cardTypeAcquiIDLength	);
	}
	if(true==isnumSpace(pRMD->merchCardType16.cardTypeFloorLimit, 12))//N -> 12
	{
		strncpy((pCHAR)merchDetailRec->merchCardType16.cardTypeFloorLimit , pRMD->merchCardType16.cardTypeFloorLimit,cardTypeFloorLimitLength);
	}

	if(true==isAlphaSpace(pRMD->merchCardType17.cardTypeAcc, 4))//A -> 4
	{
		strncpy((pCHAR)merchDetailRec->merchCardType17.cardTypeAcc		 , pRMD->merchCardType17.cardTypeAcc	 ,	cardTypeAccLength		);
	}
	if(true==isnumSpace(pRMD->merchCardType17.cardTypeMerchID, 16))//N -> 16
	{
		strncpy((pCHAR)merchDetailRec->merchCardType17.cardTypeMerchID	 , pRMD->merchCardType17.cardTypeMerchID ,	cardTypeMerchIDLength	);
	}
	if(true==isalphanumSpace(pRMD->merchCardType17.cardTypeCorpID, 6))//A/N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType17.cardTypeCorpID	 , pRMD->merchCardType17.cardTypeCorpID  ,	cardTypeCorpIDLength	);
	}
	if(true==isnumSpace(pRMD->merchCardType17.cardTypeAcquiID, 6))//N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType17.cardTypeAcquiID	 , pRMD->merchCardType17.cardTypeAcquiID ,	cardTypeAcquiIDLength	);
	}
	if(true==isnumSpace(pRMD->merchCardType17.cardTypeFloorLimit, 12))//N -> 12
	{
		strncpy((pCHAR)merchDetailRec->merchCardType17.cardTypeFloorLimit , pRMD->merchCardType17.cardTypeFloorLimit,cardTypeFloorLimitLength);
	}

	if(true==isAlphaSpace(pRMD->merchCardType18.cardTypeAcc, 4))//A -> 4
	{
		strncpy((pCHAR)merchDetailRec->merchCardType18.cardTypeAcc		 , pRMD->merchCardType18.cardTypeAcc	 ,	cardTypeAccLength		);
	}
	if(true==isnumSpace(pRMD->merchCardType18.cardTypeMerchID, 16))//N -> 16
	{
		strncpy((pCHAR)merchDetailRec->merchCardType18.cardTypeMerchID	 , pRMD->merchCardType18.cardTypeMerchID ,	cardTypeMerchIDLength	);
	}
	if(true==isalphanumSpace(pRMD->merchCardType18.cardTypeCorpID, 6))//A/N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType18.cardTypeCorpID	 , pRMD->merchCardType18.cardTypeCorpID  ,	cardTypeCorpIDLength	);
	}
	if(true==isnumSpace(pRMD->merchCardType18.cardTypeAcquiID, 6))//N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType18.cardTypeAcquiID	 , pRMD->merchCardType18.cardTypeAcquiID ,	cardTypeAcquiIDLength	);
	}
	if(true==isnumSpace(pRMD->merchCardType18.cardTypeFloorLimit, 12))//N -> 12
	{
		strncpy((pCHAR)merchDetailRec->merchCardType18.cardTypeFloorLimit , pRMD->merchCardType18.cardTypeFloorLimit,cardTypeFloorLimitLength);
	}

	if(true==isAlphaSpace(pRMD->merchCardType19.cardTypeAcc, 4))//A -> 4
	{
		strncpy((pCHAR)merchDetailRec->merchCardType19.cardTypeAcc		 , pRMD->merchCardType19.cardTypeAcc	 ,	cardTypeAccLength		);
	}
	if(true==isnumSpace(pRMD->merchCardType19.cardTypeMerchID, 16))//N -> 16
	{
		strncpy((pCHAR)merchDetailRec->merchCardType19.cardTypeMerchID	 , pRMD->merchCardType19.cardTypeMerchID ,	cardTypeMerchIDLength	);
	}
	if(true==isalphanumSpace(pRMD->merchCardType19.cardTypeCorpID, 6))//A/N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType19.cardTypeCorpID	 , pRMD->merchCardType19.cardTypeCorpID  ,	cardTypeCorpIDLength	);
	}
	if(true==isnumSpace(pRMD->merchCardType19.cardTypeAcquiID, 6))//N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType19.cardTypeAcquiID	 , pRMD->merchCardType19.cardTypeAcquiID ,	cardTypeAcquiIDLength	);
	}
	if(true==isnumSpace(pRMD->merchCardType19.cardTypeFloorLimit, 12))//N -> 12
	{
		strncpy((pCHAR)merchDetailRec->merchCardType19.cardTypeFloorLimit , pRMD->merchCardType19.cardTypeFloorLimit,cardTypeFloorLimitLength);
	}

	
	if(true==isAlphaSpace(pRMD->merchCardType20.cardTypeAcc, 4))//A -> 4
	{
		strncpy((pCHAR)merchDetailRec->merchCardType20.cardTypeAcc		 , pRMD->merchCardType20.cardTypeAcc	 ,	cardTypeAccLength		);
	}
	if(true==isnumSpace(pRMD->merchCardType20.cardTypeMerchID, 16))//N -> 16
	{
		strncpy((pCHAR)merchDetailRec->merchCardType20.cardTypeMerchID	 , pRMD->merchCardType20.cardTypeMerchID ,	cardTypeMerchIDLength	);
	}
	if(true==isalphanumSpace(pRMD->merchCardType20.cardTypeCorpID, 6))//A/N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType20.cardTypeCorpID	 , pRMD->merchCardType20.cardTypeCorpID  ,	cardTypeCorpIDLength	);
	}
	if(true==isnumSpace(pRMD->merchCardType20.cardTypeAcquiID, 6))//N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType20.cardTypeAcquiID	 , pRMD->merchCardType20.cardTypeAcquiID ,	cardTypeAcquiIDLength	);
	}
	if(true==isnumSpace(pRMD->merchCardType20.cardTypeFloorLimit, 12))//N -> 12
	{
		strncpy((pCHAR)merchDetailRec->merchCardType20.cardTypeFloorLimit , pRMD->merchCardType20.cardTypeFloorLimit,cardTypeFloorLimitLength);
	}
	
	if(true==isAlphaSpace(pRMD->merchCardType21.cardTypeAcc, 4))//A -> 4
	{
		strncpy((pCHAR)merchDetailRec->merchCardType21.cardTypeAcc		 , pRMD->merchCardType21.cardTypeAcc	 ,	cardTypeAccLength		);
	}
	if(true==isnumSpace(pRMD->merchCardType21.cardTypeMerchID, 16))//N -> 16
	{
		strncpy((pCHAR)merchDetailRec->merchCardType21.cardTypeMerchID	 , pRMD->merchCardType21.cardTypeMerchID ,	cardTypeMerchIDLength	);
	}
	if(true==isalphanumSpace(pRMD->merchCardType21.cardTypeCorpID, 6))//A/N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType21.cardTypeCorpID	 , pRMD->merchCardType21.cardTypeCorpID  ,	cardTypeCorpIDLength	);
	}
	if(true==isnumSpace(pRMD->merchCardType21.cardTypeAcquiID, 6))//N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType21.cardTypeAcquiID	 , pRMD->merchCardType21.cardTypeAcquiID ,	cardTypeAcquiIDLength	);
	}
	if(true==isnumSpace(pRMD->merchCardType21.cardTypeFloorLimit, 12))//N -> 12
	{
		strncpy((pCHAR)merchDetailRec->merchCardType21.cardTypeFloorLimit , pRMD->merchCardType21.cardTypeFloorLimit,cardTypeFloorLimitLength);
	}

	if(true==isAlphaSpace(pRMD->merchCardType22.cardTypeAcc, 4))//A -> 4
	{
		strncpy((pCHAR)merchDetailRec->merchCardType22.cardTypeAcc		 , pRMD->merchCardType22.cardTypeAcc	 ,	cardTypeAccLength		);
	}
	if(true==isnumSpace(pRMD->merchCardType22.cardTypeMerchID, 16))//N -> 16
	{
		strncpy((pCHAR)merchDetailRec->merchCardType22.cardTypeMerchID	 , pRMD->merchCardType22.cardTypeMerchID ,	cardTypeMerchIDLength	);
	}
	if(true==isalphanumSpace(pRMD->merchCardType22.cardTypeCorpID, 6))//A/N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType22.cardTypeCorpID	 , pRMD->merchCardType22.cardTypeCorpID  ,	cardTypeCorpIDLength	);
	}
	if(true==isnumSpace(pRMD->merchCardType22.cardTypeAcquiID, 6))//N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType22.cardTypeAcquiID	 , pRMD->merchCardType22.cardTypeAcquiID ,	cardTypeAcquiIDLength	);
	}
	if(true==isnumSpace(pRMD->merchCardType22.cardTypeFloorLimit, 12))//N -> 12
	{
		strncpy((pCHAR)merchDetailRec->merchCardType22.cardTypeFloorLimit , pRMD->merchCardType22.cardTypeFloorLimit,cardTypeFloorLimitLength);
	}

	if(true==isAlphaSpace(pRMD->merchCardType23.cardTypeAcc, 4))//A -> 4
	{
		strncpy((pCHAR)merchDetailRec->merchCardType23.cardTypeAcc		 , pRMD->merchCardType23.cardTypeAcc	 ,	cardTypeAccLength		);
	}
	if(true==isnumSpace(pRMD->merchCardType23.cardTypeMerchID, 16))//N -> 16
	{
		strncpy((pCHAR)merchDetailRec->merchCardType23.cardTypeMerchID	 , pRMD->merchCardType23.cardTypeMerchID ,	cardTypeMerchIDLength	);
	}
	if(true==isalphanumSpace(pRMD->merchCardType23.cardTypeCorpID, 6))//A/N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType23.cardTypeCorpID	 , pRMD->merchCardType23.cardTypeCorpID  ,	cardTypeCorpIDLength	);
	}
	if(true==isnumSpace(pRMD->merchCardType23.cardTypeAcquiID, 6))//N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType23.cardTypeAcquiID	 , pRMD->merchCardType23.cardTypeAcquiID ,	cardTypeAcquiIDLength	);
	}
	if(true==isnumSpace(pRMD->merchCardType23.cardTypeFloorLimit, 12))//N -> 12
	{
		strncpy((pCHAR)merchDetailRec->merchCardType23.cardTypeFloorLimit , pRMD->merchCardType23.cardTypeFloorLimit,cardTypeFloorLimitLength);
	}

	if(true==isAlphaSpace(pRMD->merchCardType24.cardTypeAcc, 4))//A -> 4
	{
		strncpy((pCHAR)merchDetailRec->merchCardType24.cardTypeAcc		 , pRMD->merchCardType24.cardTypeAcc	 ,	cardTypeAccLength		);
	}
	if(true==isnumSpace(pRMD->merchCardType24.cardTypeMerchID, 16))//N -> 16
	{
		strncpy((pCHAR)merchDetailRec->merchCardType24.cardTypeMerchID	 , pRMD->merchCardType24.cardTypeMerchID ,	cardTypeMerchIDLength	);
	}
	if(true==isalphanumSpace(pRMD->merchCardType24.cardTypeCorpID, 6))//A/N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType24.cardTypeCorpID	 , pRMD->merchCardType24.cardTypeCorpID  ,	cardTypeCorpIDLength	);
	}
	if(true==isnumSpace(pRMD->merchCardType24.cardTypeAcquiID, 6))//N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType24.cardTypeAcquiID	 , pRMD->merchCardType24.cardTypeAcquiID ,	cardTypeAcquiIDLength	);
	}
	if(true==isnumSpace(pRMD->merchCardType24.cardTypeFloorLimit, 12))//N -> 12
	{
		strncpy((pCHAR)merchDetailRec->merchCardType24.cardTypeFloorLimit , pRMD->merchCardType24.cardTypeFloorLimit,cardTypeFloorLimitLength);
	}

	if(true==isAlphaSpace(pRMD->merchCardType25.cardTypeAcc, 4))//A -> 4
	{
		strncpy((pCHAR)merchDetailRec->merchCardType25.cardTypeAcc		 , pRMD->merchCardType25.cardTypeAcc	 ,	cardTypeAccLength		);
	}
	if(true==isnumSpace(pRMD->merchCardType25.cardTypeMerchID, 16))//N -> 16
	{
		strncpy((pCHAR)merchDetailRec->merchCardType25.cardTypeMerchID	 , pRMD->merchCardType25.cardTypeMerchID ,	cardTypeMerchIDLength	);
	}
	if(true==isalphanumSpace(pRMD->merchCardType25.cardTypeCorpID, 6))//A/N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType25.cardTypeCorpID	 , pRMD->merchCardType25.cardTypeCorpID  ,	cardTypeCorpIDLength	);
	}
	if(true==isnumSpace(pRMD->merchCardType25.cardTypeAcquiID, 6))//N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType25.cardTypeAcquiID	 , pRMD->merchCardType25.cardTypeAcquiID ,	cardTypeAcquiIDLength	);
	}
	if(true==isnumSpace(pRMD->merchCardType25.cardTypeFloorLimit, 12))//N -> 12
	{
		strncpy((pCHAR)merchDetailRec->merchCardType25.cardTypeFloorLimit , pRMD->merchCardType25.cardTypeFloorLimit,cardTypeFloorLimitLength);
	}

	if(true==isAlphaSpace(pRMD->merchCardType26.cardTypeAcc, 4))//A -> 4
	{
		strncpy((pCHAR)merchDetailRec->merchCardType26.cardTypeAcc		 , pRMD->merchCardType26.cardTypeAcc	 ,	cardTypeAccLength		);
	}
	if(true==isnumSpace(pRMD->merchCardType26.cardTypeMerchID, 16))//N -> 16
	{
		strncpy((pCHAR)merchDetailRec->merchCardType26.cardTypeMerchID	 , pRMD->merchCardType26.cardTypeMerchID ,	cardTypeMerchIDLength	);
	}
	if(true==isalphanumSpace(pRMD->merchCardType26.cardTypeCorpID, 6))//A/N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType26.cardTypeCorpID	 , pRMD->merchCardType26.cardTypeCorpID  ,	cardTypeCorpIDLength	);
	}
	if(true==isnumSpace(pRMD->merchCardType26.cardTypeAcquiID, 6))//N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType26.cardTypeAcquiID	 , pRMD->merchCardType26.cardTypeAcquiID ,	cardTypeAcquiIDLength	);
	}
	if(true==isnumSpace(pRMD->merchCardType26.cardTypeFloorLimit, 12))//N -> 12
	{
		strncpy((pCHAR)merchDetailRec->merchCardType26.cardTypeFloorLimit , pRMD->merchCardType26.cardTypeFloorLimit,cardTypeFloorLimitLength);
	}

	if(true==isAlphaSpace(pRMD->merchCardType27.cardTypeAcc, 4))//A -> 4
	{
		strncpy((pCHAR)merchDetailRec->merchCardType27.cardTypeAcc		 , pRMD->merchCardType27.cardTypeAcc	 ,	cardTypeAccLength		);
	}
	if(true==isnumSpace(pRMD->merchCardType27.cardTypeMerchID, 16))//N -> 16
	{
		strncpy((pCHAR)merchDetailRec->merchCardType27.cardTypeMerchID	 , pRMD->merchCardType27.cardTypeMerchID ,	cardTypeMerchIDLength	);
	}
	if(true==isalphanumSpace(pRMD->merchCardType27.cardTypeCorpID, 6))//A/N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType27.cardTypeCorpID	 , pRMD->merchCardType27.cardTypeCorpID  ,	cardTypeCorpIDLength	);
	}
	if(true==isnumSpace(pRMD->merchCardType27.cardTypeAcquiID, 6))//N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType27.cardTypeAcquiID	 , pRMD->merchCardType27.cardTypeAcquiID ,	cardTypeAcquiIDLength	);
	}
	if(true==isnumSpace(pRMD->merchCardType27.cardTypeFloorLimit, 12))//N -> 12
	{
		strncpy((pCHAR)merchDetailRec->merchCardType27.cardTypeFloorLimit , pRMD->merchCardType27.cardTypeFloorLimit,cardTypeFloorLimitLength);
	}

	if(true==isAlphaSpace(pRMD->merchCardType28.cardTypeAcc, 4))//A -> 4
	{
		strncpy((pCHAR)merchDetailRec->merchCardType28.cardTypeAcc		 , pRMD->merchCardType28.cardTypeAcc	 ,	cardTypeAccLength		);
	}
	if(true==isnumSpace(pRMD->merchCardType28.cardTypeMerchID, 16))//N -> 16
	{
		strncpy((pCHAR)merchDetailRec->merchCardType28.cardTypeMerchID	 , pRMD->merchCardType28.cardTypeMerchID ,	cardTypeMerchIDLength	);
	}
	if(true==isalphanumSpace(pRMD->merchCardType28.cardTypeCorpID, 6))//A/N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType28.cardTypeCorpID	 , pRMD->merchCardType28.cardTypeCorpID  ,	cardTypeCorpIDLength	);
	}
	if(true==isnumSpace(pRMD->merchCardType28.cardTypeAcquiID, 6))//N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType28.cardTypeAcquiID	 , pRMD->merchCardType28.cardTypeAcquiID ,	cardTypeAcquiIDLength	);
	}
	if(true==isnumSpace(pRMD->merchCardType28.cardTypeFloorLimit, 12))//N -> 12
	{
		strncpy((pCHAR)merchDetailRec->merchCardType28.cardTypeFloorLimit , pRMD->merchCardType28.cardTypeFloorLimit,cardTypeFloorLimitLength);
	}

	if(true==isAlphaSpace(pRMD->merchCardType29.cardTypeAcc, 4))//A -> 4
	{
		strncpy((pCHAR)merchDetailRec->merchCardType29.cardTypeAcc		 , pRMD->merchCardType29.cardTypeAcc	 ,	cardTypeAccLength		);
	}
	if(true==isnumSpace(pRMD->merchCardType29.cardTypeMerchID, 16))//N -> 16
	{
		strncpy((pCHAR)merchDetailRec->merchCardType29.cardTypeMerchID	 , pRMD->merchCardType29.cardTypeMerchID ,	cardTypeMerchIDLength	);
	}
	if(true==isalphanumSpace(pRMD->merchCardType29.cardTypeCorpID, 6))//A/N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType29.cardTypeCorpID	 , pRMD->merchCardType29.cardTypeCorpID  ,	cardTypeCorpIDLength	);
	}
	if(true==isnumSpace(pRMD->merchCardType29.cardTypeAcquiID, 6))//N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType29.cardTypeAcquiID	 , pRMD->merchCardType29.cardTypeAcquiID ,	cardTypeAcquiIDLength	);
	}
	if(true==isnumSpace(pRMD->merchCardType29.cardTypeFloorLimit, 12))//N -> 12
	{
		strncpy((pCHAR)merchDetailRec->merchCardType29.cardTypeFloorLimit , pRMD->merchCardType29.cardTypeFloorLimit,cardTypeFloorLimitLength);
	}

	if(true==isAlphaSpace(pRMD->merchCardType30.cardTypeAcc, 4))//A -> 4
	{
		strncpy((pCHAR)merchDetailRec->merchCardType30.cardTypeAcc		 , pRMD->merchCardType30.cardTypeAcc	 ,	cardTypeAccLength		);
	}
	if(true==isnumSpace(pRMD->merchCardType30.cardTypeMerchID, 16))//N -> 16
	{
		strncpy((pCHAR)merchDetailRec->merchCardType30.cardTypeMerchID	 , pRMD->merchCardType30.cardTypeMerchID ,	cardTypeMerchIDLength	);
	}
	if(true==isalphanumSpace(pRMD->merchCardType30.cardTypeCorpID, 6))//A/N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType30.cardTypeCorpID	 , pRMD->merchCardType30.cardTypeCorpID  ,	cardTypeCorpIDLength	);
	}
	if(true==isnumSpace(pRMD->merchCardType30.cardTypeAcquiID, 6))//N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType30.cardTypeAcquiID	 , pRMD->merchCardType30.cardTypeAcquiID ,	cardTypeAcquiIDLength	);
	}
	if(true==isnumSpace(pRMD->merchCardType30.cardTypeFloorLimit, 12))//N -> 12
	{
		strncpy((pCHAR)merchDetailRec->merchCardType30.cardTypeFloorLimit , pRMD->merchCardType30.cardTypeFloorLimit,cardTypeFloorLimitLength);
	}

	if(true==isAlphaSpace(pRMD->merchCardType31.cardTypeAcc, 4))//A -> 4
	{
		strncpy((pCHAR)merchDetailRec->merchCardType31.cardTypeAcc		 , pRMD->merchCardType31.cardTypeAcc	 ,	cardTypeAccLength		);
	}
	if(true==isnumSpace(pRMD->merchCardType31.cardTypeMerchID, 16))//N -> 16
	{
		strncpy((pCHAR)merchDetailRec->merchCardType31.cardTypeMerchID	 , pRMD->merchCardType31.cardTypeMerchID ,	cardTypeMerchIDLength	);
	}
	if(true==isalphanumSpace(pRMD->merchCardType31.cardTypeCorpID, 6))//A/N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType31.cardTypeCorpID	 , pRMD->merchCardType31.cardTypeCorpID  ,	cardTypeCorpIDLength	);
	}
	if(true==isnumSpace(pRMD->merchCardType31.cardTypeAcquiID, 6))//N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType31.cardTypeAcquiID	 , pRMD->merchCardType31.cardTypeAcquiID ,	cardTypeAcquiIDLength	);
	}
	if(true==isnumSpace(pRMD->merchCardType31.cardTypeFloorLimit, 12))//N -> 12
	{
		strncpy((pCHAR)merchDetailRec->merchCardType31.cardTypeFloorLimit , pRMD->merchCardType31.cardTypeFloorLimit,cardTypeFloorLimitLength);
	}

	if(true==isAlphaSpace(pRMD->merchCardType32.cardTypeAcc, 4))//A -> 4
	{
		strncpy((pCHAR)merchDetailRec->merchCardType32.cardTypeAcc		 , pRMD->merchCardType32.cardTypeAcc	 ,	cardTypeAccLength		);
	}
	if(true==isnumSpace(pRMD->merchCardType32.cardTypeMerchID, 16))//N -> 16
	{
		strncpy((pCHAR)merchDetailRec->merchCardType32.cardTypeMerchID	 , pRMD->merchCardType32.cardTypeMerchID ,	cardTypeMerchIDLength	);
	}
	if(true==isalphanumSpace(pRMD->merchCardType32.cardTypeCorpID, 6))//A/N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType32.cardTypeCorpID	 , pRMD->merchCardType32.cardTypeCorpID  ,	cardTypeCorpIDLength	);
	}
	if(true==isnumSpace(pRMD->merchCardType32.cardTypeAcquiID, 6))//N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType32.cardTypeAcquiID	 , pRMD->merchCardType32.cardTypeAcquiID ,	cardTypeAcquiIDLength	);
	}
	if(true==isnumSpace(pRMD->merchCardType32.cardTypeFloorLimit, 12))//N -> 12
	{
		strncpy((pCHAR)merchDetailRec->merchCardType32.cardTypeFloorLimit , pRMD->merchCardType32.cardTypeFloorLimit,cardTypeFloorLimitLength);
	}

	if(true==isAlphaSpace(pRMD->merchCardType33.cardTypeAcc, 4))//A -> 4
	{
		strncpy((pCHAR)merchDetailRec->merchCardType33.cardTypeAcc		 , pRMD->merchCardType33.cardTypeAcc	 ,	cardTypeAccLength		);
	}
	if(true==isnumSpace(pRMD->merchCardType33.cardTypeMerchID, 16))//N -> 16
	{
		strncpy((pCHAR)merchDetailRec->merchCardType33.cardTypeMerchID	 , pRMD->merchCardType33.cardTypeMerchID ,	cardTypeMerchIDLength	);
	}
	if(true==isalphanumSpace(pRMD->merchCardType33.cardTypeCorpID, 6))//A/N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType33.cardTypeCorpID	 , pRMD->merchCardType33.cardTypeCorpID  ,	cardTypeCorpIDLength	);
	}
	if(true==isnumSpace(pRMD->merchCardType33.cardTypeAcquiID, 6))//N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType33.cardTypeAcquiID	 , pRMD->merchCardType33.cardTypeAcquiID ,	cardTypeAcquiIDLength	);
	}
	if(true==isnumSpace(pRMD->merchCardType33.cardTypeFloorLimit, 12))//N -> 12
	{
		strncpy((pCHAR)merchDetailRec->merchCardType33.cardTypeFloorLimit , pRMD->merchCardType33.cardTypeFloorLimit,cardTypeFloorLimitLength);
	}

	if(true==isAlphaSpace(pRMD->merchCardType34.cardTypeAcc, 4))//A -> 4
	{
		strncpy((pCHAR)merchDetailRec->merchCardType34.cardTypeAcc		 , pRMD->merchCardType34.cardTypeAcc	 ,	cardTypeAccLength		);
	}
	if(true==isnumSpace(pRMD->merchCardType34.cardTypeMerchID, 16))//N -> 16
	{
		strncpy((pCHAR)merchDetailRec->merchCardType34.cardTypeMerchID	 , pRMD->merchCardType34.cardTypeMerchID ,	cardTypeMerchIDLength	);
	}
	if(true==isalphanumSpace(pRMD->merchCardType34.cardTypeCorpID, 6))//A/N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType34.cardTypeCorpID	 , pRMD->merchCardType34.cardTypeCorpID  ,	cardTypeCorpIDLength	);
	}
	if(true==isnumSpace(pRMD->merchCardType34.cardTypeAcquiID, 6))//N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType34.cardTypeAcquiID	 , pRMD->merchCardType34.cardTypeAcquiID ,	cardTypeAcquiIDLength	);
	}
	if(true==isnumSpace(pRMD->merchCardType34.cardTypeFloorLimit, 12))//N -> 12
	{
		strncpy((pCHAR)merchDetailRec->merchCardType34.cardTypeFloorLimit , pRMD->merchCardType34.cardTypeFloorLimit,cardTypeFloorLimitLength);
	}

	if(true==isAlphaSpace(pRMD->merchCardType35.cardTypeAcc, 4))//A -> 4
	{
		strncpy((pCHAR)merchDetailRec->merchCardType35.cardTypeAcc		 , pRMD->merchCardType35.cardTypeAcc	 ,	cardTypeAccLength		);
	}
	if(true==isnumSpace(pRMD->merchCardType35.cardTypeMerchID, 16))//N -> 16
	{
		strncpy((pCHAR)merchDetailRec->merchCardType35.cardTypeMerchID	 , pRMD->merchCardType35.cardTypeMerchID ,	cardTypeMerchIDLength	);
	}
	if(true==isalphanumSpace(pRMD->merchCardType35.cardTypeCorpID, 6))//A/N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType35.cardTypeCorpID	 , pRMD->merchCardType35.cardTypeCorpID  ,	cardTypeCorpIDLength	);
	}
	if(true==isnumSpace(pRMD->merchCardType35.cardTypeAcquiID, 6))//N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType35.cardTypeAcquiID	 , pRMD->merchCardType35.cardTypeAcquiID ,	cardTypeAcquiIDLength	);
	}
	if(true==isnumSpace(pRMD->merchCardType35.cardTypeFloorLimit, 12))//N -> 12
	{
		strncpy((pCHAR)merchDetailRec->merchCardType35.cardTypeFloorLimit , pRMD->merchCardType35.cardTypeFloorLimit,cardTypeFloorLimitLength);
	}

	if(true==isAlphaSpace(pRMD->merchCardType36.cardTypeAcc, 4))//A -> 4
	{
		strncpy((pCHAR)merchDetailRec->merchCardType36.cardTypeAcc		 , pRMD->merchCardType36.cardTypeAcc	 ,	cardTypeAccLength		);
	}
	if(true==isnumSpace(pRMD->merchCardType36.cardTypeMerchID, 16))//N -> 16
	{
		strncpy((pCHAR)merchDetailRec->merchCardType36.cardTypeMerchID	 , pRMD->merchCardType36.cardTypeMerchID ,	cardTypeMerchIDLength	);
	}
	if(true==isalphanumSpace(pRMD->merchCardType36.cardTypeCorpID, 6))//A/N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType36.cardTypeCorpID	 , pRMD->merchCardType36.cardTypeCorpID  ,	cardTypeCorpIDLength	);
	}
	if(true==isnumSpace(pRMD->merchCardType36.cardTypeAcquiID, 6))//N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType36.cardTypeAcquiID	 , pRMD->merchCardType36.cardTypeAcquiID ,	cardTypeAcquiIDLength	);
	}
	if(true==isnumSpace(pRMD->merchCardType36.cardTypeFloorLimit, 12))//N -> 12
	{
		strncpy((pCHAR)merchDetailRec->merchCardType36.cardTypeFloorLimit , pRMD->merchCardType36.cardTypeFloorLimit,cardTypeFloorLimitLength);
	}

	if(true==isAlphaSpace(pRMD->merchCardType37.cardTypeAcc, 4))//A -> 4
	{
		strncpy((pCHAR)merchDetailRec->merchCardType37.cardTypeAcc		 , pRMD->merchCardType37.cardTypeAcc	 ,	cardTypeAccLength		);
	}
	if(true==isnumSpace(pRMD->merchCardType37.cardTypeMerchID, 16))//N -> 16
	{
		strncpy((pCHAR)merchDetailRec->merchCardType37.cardTypeMerchID	 , pRMD->merchCardType37.cardTypeMerchID ,	cardTypeMerchIDLength	);
	}
	if(true==isalphanumSpace(pRMD->merchCardType37.cardTypeCorpID, 6))//A/N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType37.cardTypeCorpID	 , pRMD->merchCardType37.cardTypeCorpID  ,	cardTypeCorpIDLength	);
	}
	if(true==isnumSpace(pRMD->merchCardType37.cardTypeAcquiID, 6))//N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType37.cardTypeAcquiID	 , pRMD->merchCardType37.cardTypeAcquiID ,	cardTypeAcquiIDLength	);
	}
	if(true==isnumSpace(pRMD->merchCardType37.cardTypeFloorLimit, 12))//N -> 12
	{
		strncpy((pCHAR)merchDetailRec->merchCardType37.cardTypeFloorLimit , pRMD->merchCardType37.cardTypeFloorLimit,cardTypeFloorLimitLength);
	}

	if(true==isAlphaSpace(pRMD->merchCardType38.cardTypeAcc, 4))//A -> 4
	{
		strncpy((pCHAR)merchDetailRec->merchCardType38.cardTypeAcc		 , pRMD->merchCardType38.cardTypeAcc	 ,	cardTypeAccLength		);
	}
	if(true==isnumSpace(pRMD->merchCardType38.cardTypeMerchID, 16))//N -> 16
	{
		strncpy((pCHAR)merchDetailRec->merchCardType38.cardTypeMerchID	 , pRMD->merchCardType38.cardTypeMerchID ,	cardTypeMerchIDLength	);
	}
	if(true==isalphanumSpace(pRMD->merchCardType38.cardTypeCorpID, 6))//A/N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType38.cardTypeCorpID	 , pRMD->merchCardType38.cardTypeCorpID  ,	cardTypeCorpIDLength	);
	}
	if(true==isnumSpace(pRMD->merchCardType38.cardTypeAcquiID, 6))//N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType38.cardTypeAcquiID	 , pRMD->merchCardType38.cardTypeAcquiID ,	cardTypeAcquiIDLength	);
	}
	if(true==isnumSpace(pRMD->merchCardType38.cardTypeFloorLimit, 12))//N -> 12
	{
		strncpy((pCHAR)merchDetailRec->merchCardType38.cardTypeFloorLimit , pRMD->merchCardType38.cardTypeFloorLimit,cardTypeFloorLimitLength);
	}

	if(true==isAlphaSpace(pRMD->merchCardType39.cardTypeAcc, 4))//A -> 4
	{
		strncpy((pCHAR)merchDetailRec->merchCardType39.cardTypeAcc		 , pRMD->merchCardType39.cardTypeAcc	 ,	cardTypeAccLength		);
	}
	if(true==isnumSpace(pRMD->merchCardType39.cardTypeMerchID, 16))//N -> 16
	{
		strncpy((pCHAR)merchDetailRec->merchCardType39.cardTypeMerchID	 , pRMD->merchCardType39.cardTypeMerchID ,	cardTypeMerchIDLength	);
	}
	if(true==isalphanumSpace(pRMD->merchCardType39.cardTypeCorpID, 6))//A/N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType39.cardTypeCorpID	 , pRMD->merchCardType39.cardTypeCorpID  ,	cardTypeCorpIDLength	);
	}
	if(true==isnumSpace(pRMD->merchCardType39.cardTypeAcquiID, 6))//N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType39.cardTypeAcquiID	 , pRMD->merchCardType39.cardTypeAcquiID ,	cardTypeAcquiIDLength	);
	}
	if(true==isnumSpace(pRMD->merchCardType39.cardTypeFloorLimit, 12))//N -> 12
	{
		strncpy((pCHAR)merchDetailRec->merchCardType39.cardTypeFloorLimit , pRMD->merchCardType39.cardTypeFloorLimit,cardTypeFloorLimitLength);
	}

	if(true==isAlphaSpace(pRMD->merchCardType40.cardTypeAcc, 4))//A -> 4
	{
		strncpy((pCHAR)merchDetailRec->merchCardType40.cardTypeAcc		 , pRMD->merchCardType40.cardTypeAcc	 ,	cardTypeAccLength		);
	}
	if(true==isnumSpace(pRMD->merchCardType40.cardTypeMerchID, 16))//N -> 16
	{
		strncpy((pCHAR)merchDetailRec->merchCardType40.cardTypeMerchID	 , pRMD->merchCardType40.cardTypeMerchID ,	cardTypeMerchIDLength	);
	}
	if(true==isalphanumSpace(pRMD->merchCardType40.cardTypeCorpID, 6))//A/N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType40.cardTypeCorpID	 , pRMD->merchCardType40.cardTypeCorpID  ,	cardTypeCorpIDLength	);
	}
	if(true==isnumSpace(pRMD->merchCardType40.cardTypeAcquiID, 6))//N -> 6
	{
		strncpy((pCHAR)merchDetailRec->merchCardType40.cardTypeAcquiID	 , pRMD->merchCardType40.cardTypeAcquiID ,	cardTypeAcquiIDLength	);
	}
	if(true==isnumSpace(pRMD->merchCardType40.cardTypeFloorLimit, 12))//N -> 12
	{
		strncpy((pCHAR)merchDetailRec->merchCardType40.cardTypeFloorLimit , pRMD->merchCardType40.cardTypeFloorLimit,cardTypeFloorLimitLength);
	}

	if(true==isnumSpace(pRMD->reload_merchant_flag, 1))//N -> 1
	{
		strncpy((pCHAR)merchDetailRec->reload_merchant_flag	             , pRMD->reload_merchant_flag , sizeof(merchDetailRec->reload_merchant_flag) - 1);
	}
	if(true==isnumSpace(pRMD->own_merchant, 10))//N -> 10
	{
		strncpy((pCHAR)merchDetailRec->own_merchant	             , pRMD->own_merchant , sizeof(merchDetailRec->own_merchant) - 1);
	}
	if(true==isalphanumSpace( pRMD->mastercard_assigned_id, 6))//A/N -> 6
	{
		strncpy((pCHAR)merchDetailRec->mastercard_assigned_id, pRMD->mastercard_assigned_id, sizeof(merchDetailRec->mastercard_assigned_id) - 1);	
	}

	if(true==isalphanumSpace( pRMD->cup_acceptor_id, 15))//A/N -> 15
	{
		strncpy ((pCHAR)merchDetailRec->cup_acceptor_id       ,pRMD->cup_acceptor_id, sizeof(merchDetailRec->cup_acceptor_id) -1);
	}
	if(true==isalphanumSpace( pRMD->merchGroupId, 15))//A/N -> 15
	{
		strncpy ((pCHAR)merchDetailRec->merchGroupId       ,pRMD->merchGroupId, sizeof(merchDetailRec->merchGroupId) -1);
	}
	if(' ' != pRMD->visa_mvv[0] && true==isalphanumSpace( pRMD->visa_mvv, 10))
	{
		strncpy ((pCHAR)merchDetailRec->visa_mvv       ,pRMD->visa_mvv, sizeof(merchDetailRec->visa_mvv) -1);
	}
	if(' ' != pRMD->mc_mco[0] && true==isnum( pRMD->mc_mco, 3))
	{
		strncpy ((pCHAR)merchDetailRec->mc_mco       ,pRMD->mc_mco, sizeof(merchDetailRec->mc_mco) -1);
	}

   /*the next command would make sure the input is all numbers.*/
   returnNumbers((pCHAR)merchDetailRec->merchantNumber);
   returnNumbers((pCHAR)merchDetailRec->merchOrginNumb);
   returnNumbers((pCHAR)merchDetailRec->merchCurrencyCode);
   returnNumbers((pCHAR)merchDetailRec->merchTelphone1);
   returnNumbers((pCHAR)merchDetailRec->merchTelphone2);
   returnNumbers((pCHAR)merchDetailRec->merchInvoiceBatch);
   returnNumbers((pCHAR)merchDetailRec->merchInvoiceSubcode);
   returnNumbers((pCHAR)merchDetailRec->merchProcContId);
   returnNumbers((pCHAR)merchDetailRec->merchTotalCardType);
   returnNumbers((pCHAR)merchDetailRec->merchRegionNumber);
   returnNumbers((pCHAR)merchDetailRec->merchDiviNumber);
   returnNumbers((pCHAR)merchDetailRec->merchDefMaxAcc);
   returnNumbers((pCHAR)merchDetailRec->merchDefPayTerm1);
   returnNumbers((pCHAR)merchDetailRec->merchDefFactor1);
   returnNumbers((pCHAR)merchDetailRec->merchDefMinAmount1);
   returnNumbers((pCHAR)merchDetailRec->merchDefPayTerm2);
   returnNumbers((pCHAR)merchDetailRec->merchDefFactor2);
   returnNumbers((pCHAR)merchDetailRec->merchDefMinAmount2);
   returnNumbers((pCHAR)merchDetailRec->merchDefPayTerm3);
   returnNumbers((pCHAR)merchDetailRec->merchDefFactor3);
   returnNumbers((pCHAR)merchDetailRec->merchDefMinAmount3);
   returnNumbers((pCHAR)merchDetailRec->merchDefPayTerm4);
   returnNumbers((pCHAR)merchDetailRec->merchDefFactor4);
   returnNumbers((pCHAR)merchDetailRec->merchDefMinAmount4);
   returnNumbers((pCHAR)merchDetailRec->merchDefPayTerm5);
   returnNumbers((pCHAR)merchDetailRec->merchDefFactor5);
   returnNumbers((pCHAR)merchDetailRec->merchDefMinAmount5);
   returnNumbers((pCHAR)merchDetailRec->merchDefPayTerm6);
   returnNumbers((pCHAR)merchDetailRec->merchDefFactor6);
   returnNumbers((pCHAR)merchDetailRec->merchDefMinAmount6);
   returnNumbers((pCHAR)merchDetailRec->merchCredPurcMax);
   returnNumbers((pCHAR)merchDetailRec->merchCredCashBack);
   returnNumbers((pCHAR)merchDetailRec->merchCredAuthMax);
   returnNumbers((pCHAR)merchDetailRec->merchCredRetMax);
   returnNumbers((pCHAR)merchDetailRec->merchCredCashAdvMax);
   returnNumbers((pCHAR)merchDetailRec->merchCredManEntMax);
   returnNumbers((pCHAR)merchDetailRec->merchDebitPurchMax);
   returnNumbers((pCHAR)merchDetailRec->merchDebitCashBackMax);
   returnNumbers((pCHAR)merchDetailRec->merchDebitAuthMax);
   returnNumbers((pCHAR)merchDetailRec->merchDebitRetMax);
   returnNumbers((pCHAR)merchDetailRec->merchDebitCashAdvMax);
   returnNumbers((pCHAR)merchDetailRec->merchDebitManEntMax);
   returnNumbers((pCHAR)merchDetailRec->merchDateAdded);
   returnNumbers((pCHAR)merchDetailRec->merchHCCode);
   returnNumbers((pCHAR)merchDetailRec->merchSolicCode);
   /*added by farad on 3/17/1999 for bug # 252.*/
   returnNumbers((pCHAR)merchDetailRec->merchMCC);

   returnNumbers((pCHAR)merchDetailRec->merchCardType1.cardTypeMerchID     );
   returnNumbers((pCHAR)merchDetailRec->merchCardType1.cardTypeAcquiID     );
   returnNumbers((pCHAR)merchDetailRec->merchCardType1.cardTypeFloorLimit  );
   returnNumbers((pCHAR)merchDetailRec->merchCardType2.cardTypeMerchID     );
   returnNumbers((pCHAR)merchDetailRec->merchCardType2.cardTypeAcquiID     );
   returnNumbers((pCHAR)merchDetailRec->merchCardType2.cardTypeFloorLimit  );
   returnNumbers((pCHAR)merchDetailRec->merchCardType3.cardTypeMerchID     );
   returnNumbers((pCHAR)merchDetailRec->merchCardType3.cardTypeAcquiID     );
   returnNumbers((pCHAR)merchDetailRec->merchCardType3.cardTypeFloorLimit  );
   returnNumbers((pCHAR)merchDetailRec->merchCardType4.cardTypeMerchID     );
   returnNumbers((pCHAR)merchDetailRec->merchCardType4.cardTypeAcquiID     );
   returnNumbers((pCHAR)merchDetailRec->merchCardType4.cardTypeFloorLimit  );
   returnNumbers((pCHAR)merchDetailRec->merchCardType5.cardTypeMerchID     );
   returnNumbers((pCHAR)merchDetailRec->merchCardType5.cardTypeAcquiID     );
   returnNumbers((pCHAR)merchDetailRec->merchCardType5.cardTypeFloorLimit  );
   returnNumbers((pCHAR)merchDetailRec->merchCardType6.cardTypeMerchID     );
   returnNumbers((pCHAR)merchDetailRec->merchCardType6.cardTypeAcquiID     );
   returnNumbers((pCHAR)merchDetailRec->merchCardType6.cardTypeFloorLimit  );
   returnNumbers((pCHAR)merchDetailRec->merchCardType7.cardTypeMerchID     );
   returnNumbers((pCHAR)merchDetailRec->merchCardType7.cardTypeAcquiID     );
   returnNumbers((pCHAR)merchDetailRec->merchCardType7.cardTypeFloorLimit  );
   returnNumbers((pCHAR)merchDetailRec->merchCardType8.cardTypeMerchID     );
   returnNumbers((pCHAR)merchDetailRec->merchCardType8.cardTypeAcquiID     );
   returnNumbers((pCHAR)merchDetailRec->merchCardType8.cardTypeFloorLimit  );
   returnNumbers((pCHAR)merchDetailRec->merchCardType9.cardTypeMerchID     );
   returnNumbers((pCHAR)merchDetailRec->merchCardType9.cardTypeAcquiID     );
   returnNumbers((pCHAR)merchDetailRec->merchCardType9.cardTypeFloorLimit  );
   returnNumbers((pCHAR)merchDetailRec->merchCardType10.cardTypeMerchID    );
   returnNumbers((pCHAR)merchDetailRec->merchCardType10.cardTypeAcquiID    );
   returnNumbers((pCHAR)merchDetailRec->merchCardType10.cardTypeFloorLimit );
   returnNumbers((pCHAR)merchDetailRec->merchCardType11.cardTypeMerchID    );
   returnNumbers((pCHAR)merchDetailRec->merchCardType11.cardTypeAcquiID    );
   returnNumbers((pCHAR)merchDetailRec->merchCardType11.cardTypeFloorLimit );
   returnNumbers((pCHAR)merchDetailRec->merchCardType12.cardTypeMerchID    );
   returnNumbers((pCHAR)merchDetailRec->merchCardType12.cardTypeAcquiID    );
   returnNumbers((pCHAR)merchDetailRec->merchCardType12.cardTypeFloorLimit );
   returnNumbers((pCHAR)merchDetailRec->merchCardType13.cardTypeMerchID    );
   returnNumbers((pCHAR)merchDetailRec->merchCardType13.cardTypeAcquiID    );
   returnNumbers((pCHAR)merchDetailRec->merchCardType13.cardTypeFloorLimit );
   returnNumbers((pCHAR)merchDetailRec->merchCardType14.cardTypeMerchID    );
   returnNumbers((pCHAR)merchDetailRec->merchCardType14.cardTypeAcquiID    );
   returnNumbers((pCHAR)merchDetailRec->merchCardType14.cardTypeFloorLimit );
   returnNumbers((pCHAR)merchDetailRec->merchCardType15.cardTypeMerchID    );
   returnNumbers((pCHAR)merchDetailRec->merchCardType15.cardTypeAcquiID    );
   returnNumbers((pCHAR)merchDetailRec->merchCardType15.cardTypeFloorLimit );
   returnNumbers((pCHAR)merchDetailRec->merchCardType16.cardTypeMerchID    );
   returnNumbers((pCHAR)merchDetailRec->merchCardType16.cardTypeAcquiID	 );
   returnNumbers((pCHAR)merchDetailRec->merchCardType16.cardTypeFloorLimit );
   returnNumbers((pCHAR)merchDetailRec->merchCardType17.cardTypeMerchID    );
   returnNumbers((pCHAR)merchDetailRec->merchCardType17.cardTypeAcquiID    );
   returnNumbers((pCHAR)merchDetailRec->merchCardType17.cardTypeFloorLimit );
   returnNumbers((pCHAR)merchDetailRec->merchCardType18.cardTypeMerchID    );
   returnNumbers((pCHAR)merchDetailRec->merchCardType18.cardTypeAcquiID    );
   returnNumbers((pCHAR)merchDetailRec->merchCardType18.cardTypeFloorLimit );
   returnNumbers((pCHAR)merchDetailRec->merchCardType19.cardTypeMerchID    );
   returnNumbers((pCHAR)merchDetailRec->merchCardType19.cardTypeAcquiID    );
   returnNumbers((pCHAR)merchDetailRec->merchCardType19.cardTypeFloorLimit );
   returnNumbers((pCHAR)merchDetailRec->merchCardType20.cardTypeMerchID    );
   returnNumbers((pCHAR)merchDetailRec->merchCardType20.cardTypeAcquiID    );
   returnNumbers((pCHAR)merchDetailRec->merchCardType20.cardTypeFloorLimit );
   returnNumbers((pCHAR)merchDetailRec->merchCardType21.cardTypeMerchID    );
   returnNumbers((pCHAR)merchDetailRec->merchCardType21.cardTypeAcquiID    );
   returnNumbers((pCHAR)merchDetailRec->merchCardType21.cardTypeFloorLimit );
   returnNumbers((pCHAR)merchDetailRec->merchCardType22.cardTypeMerchID    );
   returnNumbers((pCHAR)merchDetailRec->merchCardType22.cardTypeAcquiID    );
   returnNumbers((pCHAR)merchDetailRec->merchCardType22.cardTypeFloorLimit );
   returnNumbers((pCHAR)merchDetailRec->merchCardType23.cardTypeMerchID    );
   returnNumbers((pCHAR)merchDetailRec->merchCardType23.cardTypeAcquiID    );
   returnNumbers((pCHAR)merchDetailRec->merchCardType23.cardTypeFloorLimit );
   returnNumbers((pCHAR)merchDetailRec->merchCardType24.cardTypeMerchID    );
   returnNumbers((pCHAR)merchDetailRec->merchCardType24.cardTypeAcquiID    );
   returnNumbers((pCHAR)merchDetailRec->merchCardType24.cardTypeFloorLimit );
   returnNumbers((pCHAR)merchDetailRec->merchCardType25.cardTypeMerchID    );
   returnNumbers((pCHAR)merchDetailRec->merchCardType25.cardTypeAcquiID    );
   returnNumbers((pCHAR)merchDetailRec->merchCardType25.cardTypeFloorLimit );
   returnNumbers((pCHAR)merchDetailRec->merchCardType26.cardTypeMerchID    );
   returnNumbers((pCHAR)merchDetailRec->merchCardType26.cardTypeAcquiID    );
   returnNumbers((pCHAR)merchDetailRec->merchCardType26.cardTypeFloorLimit );
   returnNumbers((pCHAR)merchDetailRec->merchCardType27.cardTypeMerchID    );
   returnNumbers((pCHAR)merchDetailRec->merchCardType27.cardTypeAcquiID    );
   returnNumbers((pCHAR)merchDetailRec->merchCardType27.cardTypeFloorLimit );
   returnNumbers((pCHAR)merchDetailRec->merchCardType28.cardTypeMerchID    );
   returnNumbers((pCHAR)merchDetailRec->merchCardType28.cardTypeAcquiID    );
   returnNumbers((pCHAR)merchDetailRec->merchCardType28.cardTypeFloorLimit );
   returnNumbers((pCHAR)merchDetailRec->merchCardType29.cardTypeMerchID    );
   returnNumbers((pCHAR)merchDetailRec->merchCardType29.cardTypeAcquiID    );
   returnNumbers((pCHAR)merchDetailRec->merchCardType29.cardTypeFloorLimit );
   returnNumbers((pCHAR)merchDetailRec->merchCardType30.cardTypeMerchID    );
   returnNumbers((pCHAR)merchDetailRec->merchCardType30.cardTypeAcquiID    );
   returnNumbers((pCHAR)merchDetailRec->merchCardType30.cardTypeFloorLimit );
   returnNumbers((pCHAR)merchDetailRec->merchCardType31.cardTypeMerchID    );
   returnNumbers((pCHAR)merchDetailRec->merchCardType31.cardTypeAcquiID    );
   returnNumbers((pCHAR)merchDetailRec->merchCardType31.cardTypeFloorLimit );
   returnNumbers((pCHAR)merchDetailRec->merchCardType32.cardTypeMerchID    );
   returnNumbers((pCHAR)merchDetailRec->merchCardType32.cardTypeAcquiID    );
   returnNumbers((pCHAR)merchDetailRec->merchCardType32.cardTypeFloorLimit );
   returnNumbers((pCHAR)merchDetailRec->merchCardType33.cardTypeMerchID    );
   returnNumbers((pCHAR)merchDetailRec->merchCardType33.cardTypeAcquiID    );
   returnNumbers((pCHAR)merchDetailRec->merchCardType33.cardTypeFloorLimit );
   returnNumbers((pCHAR)merchDetailRec->merchCardType34.cardTypeMerchID    );
   returnNumbers((pCHAR)merchDetailRec->merchCardType34.cardTypeAcquiID    );
   returnNumbers((pCHAR)merchDetailRec->merchCardType34.cardTypeFloorLimit );
   returnNumbers((pCHAR)merchDetailRec->merchCardType35.cardTypeMerchID    );
   returnNumbers((pCHAR)merchDetailRec->merchCardType35.cardTypeAcquiID    );
   returnNumbers((pCHAR)merchDetailRec->merchCardType35.cardTypeFloorLimit );
   returnNumbers((pCHAR)merchDetailRec->merchCardType36.cardTypeMerchID    );
   returnNumbers((pCHAR)merchDetailRec->merchCardType36.cardTypeAcquiID    );
   returnNumbers((pCHAR)merchDetailRec->merchCardType36.cardTypeFloorLimit );
   returnNumbers((pCHAR)merchDetailRec->merchCardType37.cardTypeMerchID    );
   returnNumbers((pCHAR)merchDetailRec->merchCardType37.cardTypeAcquiID    );
   returnNumbers((pCHAR)merchDetailRec->merchCardType37.cardTypeFloorLimit );
   returnNumbers((pCHAR)merchDetailRec->merchCardType38.cardTypeMerchID    );
   returnNumbers((pCHAR)merchDetailRec->merchCardType38.cardTypeAcquiID    );
   returnNumbers((pCHAR)merchDetailRec->merchCardType38.cardTypeFloorLimit );
   returnNumbers((pCHAR)merchDetailRec->merchCardType39.cardTypeMerchID    );
   returnNumbers((pCHAR)merchDetailRec->merchCardType39.cardTypeAcquiID    );
   returnNumbers((pCHAR)merchDetailRec->merchCardType39.cardTypeFloorLimit );
   returnNumbers((pCHAR)merchDetailRec->merchCardType40.cardTypeMerchID    );
   returnNumbers((pCHAR)merchDetailRec->merchCardType40.cardTypeAcquiID    );
   returnNumbers((pCHAR)merchDetailRec->merchCardType40.cardTypeFloorLimit );
   returnNumAscii((pCHAR)merchDetailRec->merchGroupId);
   returnNumbers((pCHAR)merchDetailRec->mc_mco );

	}/*end of parseMerchantDetail*/

/*************************************************************************************
       NAME:           parseMerchantTrailer()

       DESCRIPTION:    This module parses the trailer of the merchant ascii file.
					        It populates the myMerchantTrailer structure.
                       
       INPUTS:         dataRecord is CHAR array which containes the trailer record 
                       from the merchant file.
	    OUTPUTS:
                       structure of type myMerchantTrailer
       RETURNS:        
		   None.                       
                               
      AUTHOR:          Faramarz Arad.
      MODIFIED BY:     None.
*************************************************************************************/
void parseMerchantTrailer(pCHAR dataRecord, pmyMerchantTrailer merchTrailRec)
{
   memset(merchTrailRec,0,sizeof(myMerchantTrailer));

   strncpy((pCHAR)merchTrailRec->trailRecordType     , dataRecord     , sizeof(merchTrailRec->trailRecordType)     -1);
   strncpy((pCHAR)merchTrailRec->trailFileName       , dataRecord + 3 , sizeof(merchTrailRec->trailFileName)       -1);
   strncpy((pCHAR)merchTrailRec->trailFileCreateDate , dataRecord + 11, sizeof(merchTrailRec->trailFileCreateDate) -1);
   strncpy((pCHAR)merchTrailRec->trailNumberofRecord , dataRecord + 17, sizeof(merchTrailRec->trailNumberofRecord) -1);
   strncpy((pCHAR)merchTrailRec->trailFiller         , dataRecord + 26, sizeof(merchTrailRec->trailFiller)         -1);

   /*The next command will make sure the inputs are numbers only.*/
   returnNumbers((pCHAR)merchTrailRec->trailFileCreateDate);
   returnNumbers((pCHAR)merchTrailRec->trailNumberofRecord);

}  /*end of parseMerchantTrailer.*/






/*************************************************************************************
       NAME:           parseMpParameterHeader()

       DESCRIPTION:    This module parses the Header of the MpParameter ascii file.
					        It populates the myMpParameterHeader structure.
                       
       INPUTS:         dataRecord is CHAR array which containes the header record 
                       from the MpParameter file.
	    OUTPUTS:
                       structure of type myMpParameterHeader
       RETURNS:        
		   None.                       
                               
      AUTHOR:          Faramarz Arad.
      MODIFIED BY:     None.
*************************************************************************************/
void parseMpParameterHeader(pCHAR dataRecord, pmyMpParameterHeader mpparaHeadRec)
{
   memset(mpparaHeadRec,0,sizeof(myMpParameterHeader));

   strncpy((pCHAR)mpparaHeadRec->headRecordType ,    dataRecord,      sizeof(mpparaHeadRec->headRecordType     ) - 1);
   strncpy((pCHAR)mpparaHeadRec->headFileName,       dataRecord + 3,  sizeof(mpparaHeadRec->headFileName       ) - 1);
   strncpy((pCHAR)mpparaHeadRec->headFileCreateDate, dataRecord + 11, sizeof(mpparaHeadRec->headFileCreateDate ) - 1);
   strncpy((pCHAR)mpparaHeadRec->headFiller,         dataRecord + 17, sizeof(mpparaHeadRec->headFiller         ) - 1);     

   /*the next command would make sure the input is all numbers.*/
   returnNumbers((pCHAR)mpparaHeadRec->headFileCreateDate);


}  /*end of myMpParameterHeader*/



/*************************************************************************************
       NAME:           parseMpParameterDetail()

       DESCRIPTION:    This module parses the detail of the MpParameter ascii file.
					        It populates the myMpParameterDetail structure.
                       
       INPUTS:         dataRecord is CHAR array which containes the detail record 
                       from the merchant file.
	    OUTPUTS:
                       structure of type myMpParameterDetail
       RETURNS:        
		   None.                       
                               
      AUTHOR:          Faramarz Arad.
      MODIFIED BY:     None.
*************************************************************************************/


void parseMpParameterDetail(pCHAR dataRecord, pmyMpParameterDetail mpparaDetailRec)
{

   pRawMpParameterDetail pRMPD;

   memset(mpparaDetailRec,0,sizeof(myMpParameterDetail));
   pRMPD = (pRawMpParameterDetail)dataRecord;

	if(true==isalphanumSpace(pRMPD->mpparaRecordType, 3))
	{
		strncpy((pCHAR)mpparaDetailRec->mpparaRecordType       ,pRMPD->mpparaRecordType,    sizeof(mpparaDetailRec->mpparaRecordType) - 1);
	}
	if(true==isnumSpace(pRMPD->mpparaProductCode, 4))
	{
		strncpy((pCHAR)mpparaDetailRec->mpparaProductCode       ,pRMPD->mpparaProductCode,    sizeof(mpparaDetailRec->mpparaProductCode) - 1);
	}
	if(true==isalphanumSpaceDot(pRMPD->mpparaMinMajorPurchaseAmount, 12))
	{
		strncpy((pCHAR)mpparaDetailRec->mpparaMinMajorPurchaseAmount       ,pRMPD->mpparaMinMajorPurchaseAmount,    sizeof(mpparaDetailRec->mpparaMinMajorPurchaseAmount) - 1);
	}
	if(true==isalphanumSpaceDot(pRMPD->mpparaMaxMajorPurchaseAmount, 12))
	{
		strncpy((pCHAR)mpparaDetailRec->mpparaMaxMajorPurchaseAmount       ,pRMPD->mpparaMaxMajorPurchaseAmount,    sizeof(mpparaDetailRec->mpparaMaxMajorPurchaseAmount) - 1);
	}
	if(true==isalphanumSpaceDot(pRMPD->mpparaMinInstallmentAmount, 12))
	{
		strncpy((pCHAR)mpparaDetailRec->mpparaMinInstallmentAmount       ,pRMPD->mpparaMinInstallmentAmount,    sizeof(mpparaDetailRec->mpparaMinInstallmentAmount) - 1);
	}
	if(true==isalphanumSpaceDot(pRMPD->mpparaMaxInstallmentAmount, 12))
	{
		strncpy((pCHAR)mpparaDetailRec->mpparaMaxInstallmentAmount       ,pRMPD->mpparaMaxInstallmentAmount,    sizeof(mpparaDetailRec->mpparaMaxInstallmentAmount) - 1);
	}
	if(true==isalphanumSpaceDot(pRMPD->mpparaMinMajorPurchaseLimit, 10))
	{
		strncpy((pCHAR)mpparaDetailRec->mpparaMinMajorPurchaseLimit       ,pRMPD->mpparaMinMajorPurchaseLimit,    sizeof(mpparaDetailRec->mpparaMinMajorPurchaseLimit) - 1);
	}
	if(true==isalphanumSpaceDot(pRMPD->mpparaMinPenaltyAmount, 15))
	{
		strncpy((pCHAR)mpparaDetailRec->mpparaMinPenaltyAmount       ,pRMPD->mpparaMinPenaltyAmount,    sizeof(mpparaDetailRec->mpparaMinPenaltyAmount) - 1);
	}
	if(true==isalphanumSpaceDot(pRMPD->mpparaMaxPenaltyAmount, 15))
	{
		strncpy((pCHAR)mpparaDetailRec->mpparaMaxPenaltyAmount       ,pRMPD->mpparaMaxPenaltyAmount,    sizeof(mpparaDetailRec->mpparaMaxPenaltyAmount) - 1);
	}
	if(true==isalphanumSpaceDot(pRMPD->mpparaPenaltyPercentage, 7))
	{
		strncpy((pCHAR)mpparaDetailRec->mpparaPenaltyPercentage       ,pRMPD->mpparaPenaltyPercentage,    sizeof(mpparaDetailRec->mpparaPenaltyPercentage) - 1);
	}
	if(true==isalphanumSpace(pRMPD->mpparaPenaltyDays , 3))
	{
		strncpy((pCHAR)mpparaDetailRec->mpparaPenaltyDays        ,pRMPD->mpparaPenaltyDays ,    sizeof(mpparaDetailRec->mpparaPenaltyDays ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMPD->mpparaMinAmtForAging , 18))
	{
		strncpy((pCHAR)mpparaDetailRec->mpparaMinAmtForAging        ,pRMPD->mpparaMinAmtForAging ,    sizeof(mpparaDetailRec->mpparaMinAmtForAging ) - 1);
	}
	if(true==isalphanumSpace(pRMPD->mpparaMonthsToWriteOff , 2))
	{
		strncpy((pCHAR)mpparaDetailRec->mpparaMonthsToWriteOff        ,pRMPD->mpparaMonthsToWriteOff ,    sizeof(mpparaDetailRec->mpparaMonthsToWriteOff ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMPD->mpparaBPIMinimumAmount , 12))
	{
		strncpy((pCHAR)mpparaDetailRec->mpparaBPIMinimumAmount        ,pRMPD->mpparaBPIMinimumAmount ,    sizeof(mpparaDetailRec->mpparaBPIMinimumAmount ) - 1);
	}
	if(true==isalphanumSpace(pRMPD->mpparaBrokenPeriodInterest , 1))
	{
		strncpy((pCHAR)mpparaDetailRec->mpparaBrokenPeriodInterest        ,pRMPD->mpparaBrokenPeriodInterest ,    sizeof(mpparaDetailRec->mpparaBrokenPeriodInterest ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMPD->mpparaMinPurchaseAmtForFee , 12))
	{
		strncpy((pCHAR)mpparaDetailRec->mpparaMinPurchaseAmtForFee        ,pRMPD->mpparaMinPurchaseAmtForFee ,    sizeof(mpparaDetailRec->mpparaMinPurchaseAmtForFee ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMPD->mpparaErrorUnauthorizedAmount , 15))
	{
		strncpy((pCHAR)mpparaDetailRec->mpparaErrorUnauthorizedAmount        ,pRMPD->mpparaErrorUnauthorizedAmount ,    sizeof(mpparaDetailRec->mpparaErrorUnauthorizedAmount ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMPD->mpparaFixedMPFee , 15))
	{
		strncpy((pCHAR)mpparaDetailRec->mpparaFixedMPFee        ,pRMPD->mpparaFixedMPFee ,    sizeof(mpparaDetailRec->mpparaFixedMPFee ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMPD->mpparaMPFeePercentage , 6))
	{
		strncpy((pCHAR)mpparaDetailRec->mpparaMPFeePercentage        ,pRMPD->mpparaMPFeePercentage ,    sizeof(mpparaDetailRec->mpparaMPFeePercentage ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMPD->mpparaEarlyRepaymentFeePercentage , 8))
	{
		strncpy((pCHAR)mpparaDetailRec->mpparaEarlyRepaymentFeePercentage        ,pRMPD->mpparaEarlyRepaymentFeePercentage ,    sizeof(mpparaDetailRec->mpparaEarlyRepaymentFeePercentage ) - 1);
	}
	if(true==isnum(pRMPD->mpparaEffectiveDate , 8))
	{
		strncpy((pCHAR)mpparaDetailRec->mpparaEffectiveDate        ,pRMPD->mpparaEffectiveDate ,    sizeof(mpparaDetailRec->mpparaEffectiveDate ) - 1);
	}
	if(true==isalphanumSpace(pRMPD->mpparaMaxNoOfTransPerDay , 2))
	{
		strncpy((pCHAR)mpparaDetailRec->mpparaMaxNoOfTransPerDay        ,pRMPD->mpparaMaxNoOfTransPerDay ,    sizeof(mpparaDetailRec->mpparaMaxNoOfTransPerDay ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMPD->mpparaMaxAmountPerDay , 15))
	{
		strncpy((pCHAR)mpparaDetailRec->mpparaMaxAmountPerDay        ,pRMPD->mpparaMaxAmountPerDay ,    sizeof(mpparaDetailRec->mpparaMaxAmountPerDay ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMPD->mpparaAuthPercentOverlimit , 7))
	{
		strncpy((pCHAR)mpparaDetailRec->mpparaAuthPercentOverlimit        ,pRMPD->mpparaAuthPercentOverlimit ,    sizeof(mpparaDetailRec->mpparaAuthPercentOverlimit ) - 1);
	}
	if(true==isalphanumSpace(pRMPD->mpparaRespCode1  , 2))
	{
		strncpy((pCHAR)mpparaDetailRec->mpparaRespCode1         ,pRMPD->mpparaRespCode1  ,    sizeof(mpparaDetailRec->mpparaRespCode1  ) - 1);
	}
	if(true==isalphanumSpace(pRMPD->mpparaMessage1  , 2))
	{
		strncpy((pCHAR)mpparaDetailRec->mpparaMessage1         ,pRMPD->mpparaMessage1  ,    sizeof(mpparaDetailRec->mpparaMessage1  ) - 1);
	}
	if(true==isalphanumSpace(pRMPD->mpparaRespCode2  , 2))
	{
		strncpy((pCHAR)mpparaDetailRec->mpparaRespCode2         ,pRMPD->mpparaRespCode2  ,    sizeof(mpparaDetailRec->mpparaRespCode2  ) - 1);
	}
	if(true==isalphanumSpace(pRMPD->mpparaMessage2  , 2))
	{
		strncpy((pCHAR)mpparaDetailRec->mpparaMessage2         ,pRMPD->mpparaMessage2  ,    sizeof(mpparaDetailRec->mpparaMessage2  ) - 1);
	}
	if(true==isalphanumSpace(pRMPD->mpparaRespCode3  , 2))
	{
		strncpy((pCHAR)mpparaDetailRec->mpparaRespCode3         ,pRMPD->mpparaRespCode3  ,    sizeof(mpparaDetailRec->mpparaRespCode3  ) - 1);
	}
	if(true==isalphanumSpace(pRMPD->mpparaMessage3 , 2))
	{
		strncpy((pCHAR)mpparaDetailRec->mpparaMessage3         ,pRMPD->mpparaMessage3  ,    sizeof(mpparaDetailRec->mpparaMessage3  ) - 1);
	}
	if(true==isalphanumSpace(pRMPD->mpparaFiller , 3))
	{
		strncpy((pCHAR)mpparaDetailRec->mpparaFiller         ,pRMPD->mpparaFiller  ,    sizeof(mpparaDetailRec->mpparaFiller  ) - 1);
	}

     /*the next command would make sure the input is all numbers.*/
 	returnNumMP((pCHAR)mpparaDetailRec->mpparaProductCode);
	returnNumdotMP((pCHAR)mpparaDetailRec->mpparaMinMajorPurchaseAmount);
	returnNumdotMP((pCHAR)mpparaDetailRec->mpparaMaxMajorPurchaseAmount);
	returnNumdotMP((pCHAR)mpparaDetailRec->mpparaMinInstallmentAmount);
	returnNumdotMP((pCHAR)mpparaDetailRec->mpparaMaxInstallmentAmount);
	returnNumdotMP((pCHAR)mpparaDetailRec->mpparaMinMajorPurchaseLimit);
	returnNumdotMP((pCHAR)mpparaDetailRec->mpparaMinPenaltyAmount);
	returnNumdotMP((pCHAR)mpparaDetailRec->mpparaMaxPenaltyAmount);
	returnNumdotMP((pCHAR)mpparaDetailRec->mpparaPenaltyPercentage);
	returnNumbers((pCHAR)mpparaDetailRec->mpparaPenaltyDays);
	returnNumdotMP((pCHAR)mpparaDetailRec->mpparaMinAmtForAging);
	returnNumbers((pCHAR)mpparaDetailRec->mpparaMonthsToWriteOff);
	returnNumdotMP((pCHAR)mpparaDetailRec->mpparaBPIMinimumAmount);
	returnNumdotMP((pCHAR)mpparaDetailRec->mpparaMinPurchaseAmtForFee);
	returnNumdotMP((pCHAR)mpparaDetailRec->mpparaErrorUnauthorizedAmount);
	returnNumdotMP((pCHAR)mpparaDetailRec->mpparaFixedMPFee);
	returnNumdotMP((pCHAR)mpparaDetailRec->mpparaMPFeePercentage);
	returnNumdotMP((pCHAR)mpparaDetailRec->mpparaEarlyRepaymentFeePercentage);
	returnNumbers((pCHAR)mpparaDetailRec->mpparaEffectiveDate);
	returnNumdotMP((pCHAR)mpparaDetailRec->mpparaMaxNoOfTransPerDay);
	returnNumdotMP((pCHAR)mpparaDetailRec->mpparaMaxAmountPerDay);
	returnNumdotMP((pCHAR)mpparaDetailRec->mpparaAuthPercentOverlimit);
	returnNumbers((pCHAR)mpparaDetailRec->mpparaRespCode1);
	returnNumbers((pCHAR)mpparaDetailRec->mpparaMessage1);
	returnNumbers((pCHAR)mpparaDetailRec->mpparaRespCode2);
	returnNumbers((pCHAR)mpparaDetailRec->mpparaMessage2);
	returnNumbers((pCHAR)mpparaDetailRec->mpparaRespCode3);
	returnNumbers((pCHAR)mpparaDetailRec->mpparaMessage3);
}
	
	
/*end of parseMpParameterDetail*/



/*************************************************************************************
       NAME:           parseMpParameterTrailer()

       DESCRIPTION:    This module parses the trailer of the MpParameter ascii file.
					        It populates the myMpParameterTrailer structure.
                       
       INPUTS:         dataRecord is CHAR array which containes the trailer record 
                       from the MpParameter file.
	    OUTPUTS:
                       structure of type myMpParameterTrailer
       RETURNS:        
		   None.                       
                               
      AUTHOR:          Faramarz Arad.
      MODIFIED BY:     None.
*************************************************************************************/
void parseMpParameterTrailer(pCHAR dataRecord, pmyMpParameterTrailer mpparaTrailRec)
{
   memset(mpparaTrailRec,0,sizeof(myMpParameterTrailer));

   strncpy((pCHAR)mpparaTrailRec->trailRecordType     , dataRecord     , sizeof(mpparaTrailRec->trailRecordType)     -1);
   strncpy((pCHAR)mpparaTrailRec->trailFileName       , dataRecord + 3 , sizeof(mpparaTrailRec->trailFileName)       -1);
   strncpy((pCHAR)mpparaTrailRec->trailFileCreateDate , dataRecord + 11, sizeof(mpparaTrailRec->trailFileCreateDate) -1);
   strncpy((pCHAR)mpparaTrailRec->trailNumberofRecord , dataRecord + 17, sizeof(mpparaTrailRec->trailNumberofRecord) -1);
   strncpy((pCHAR)mpparaTrailRec->trailFiller         , dataRecord + 26, sizeof(mpparaTrailRec->trailFiller)         -1);

   /*The next command will make sure the inputs are numbers only.*/
  
   returnNumbers((pCHAR)mpparaTrailRec->trailFileCreateDate);
   returnNumbers((pCHAR)mpparaTrailRec->trailNumberofRecord);


}  /*end of parseMpParameterTrailer.*/


/*************************************************************************************
       NAME:           parseMerchantGroupHeader()

       DESCRIPTION:    This module parses the Header of the MerchantGroup ascii file.
					        It populates the myMerchantGroupHeader structure.
                       
       INPUTS:         dataRecord is CHAR array which containes the header record 
                       from the MerchantGroup file.
	    OUTPUTS:
                       structure of type myMerchantGroupHeader
       RETURNS:        
		   None.                       
                               
      AUTHOR:          Faramarz Arad.
      MODIFIED BY:     None.
*************************************************************************************/


void parseMerchantGroupHeader(pCHAR dataRecord, pmyMerchantGroupHeader mgfHeadRec)
{
   memset(mgfHeadRec,0,sizeof(myMerchantGroupHeader));

   strncpy((pCHAR)mgfHeadRec->headRecordType ,    dataRecord,      sizeof(mgfHeadRec->headRecordType     ) - 1);
   strncpy((pCHAR)mgfHeadRec->headFileName,       dataRecord + 3,  sizeof(mgfHeadRec->headFileName       ) - 1);
   strncpy((pCHAR)mgfHeadRec->headFileCreateDate, dataRecord + 11, sizeof(mgfHeadRec->headFileCreateDate ) - 1);
   strncpy((pCHAR)mgfHeadRec->headFiller,         dataRecord + 17, sizeof(mgfHeadRec->headFiller         ) - 1);     

   /*the next command would make sure the input is all numbers.*/
   returnNumbers((pCHAR)mgfHeadRec->headFileCreateDate);

}  /*end of myMerchantGroupHeader*/


/*************************************************************************************
       NAME:           parseMerchantGroupDetail()

       DESCRIPTION:    This module parses the detail of the MerchantGroup ascii file.
					        It populates the myMerchantGroupDetail structure.
                       
       INPUTS:         dataRecord is CHAR array which containes the detail record 
                       from the merchant file.
	    OUTPUTS:
                       structure of type myMerchantGroupDetail
       RETURNS:        
		   None.                       
                               
      AUTHOR:          Faramarz Arad.
      MODIFIED BY:     None.
*************************************************************************************/


void parseMerchantGroupDetail(pCHAR dataRecord, pmyMerchantGroupDetail mgfDetailRec)
{

	pRawMerchantGroupDetail pRMGD;

	memset(mgfDetailRec,0,sizeof(myMerchantGroupDetail));
	pRMGD = (pRawMerchantGroupDetail)dataRecord;

	if(true==isalphanumSpace(pRMGD->mgfRecordType, 3)) //A/N -> 3
	{
		strncpy((pCHAR)mgfDetailRec->mgfRecordType       ,pRMGD->mgfRecordType,    sizeof(mgfDetailRec->mgfRecordType) - 1);
	}
	if(true==isalphanumSpace(pRMGD->mgfMerchantGroupID, 15)) //N -> 15
	{
		strncpy((pCHAR)mgfDetailRec->mgfMerchantGroupID       ,pRMGD->mgfMerchantGroupID,    sizeof(mgfDetailRec->mgfMerchantGroupID) - 1);
	}
	if(true==isnum(pRMGD->mgfEffectiveDate, 8)) //N -> 8
	{
		strncpy((pCHAR)mgfDetailRec->mgfEffectiveDate       ,pRMGD->mgfEffectiveDate,    sizeof(mgfDetailRec->mgfEffectiveDate) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfAmount_limit_1, 12)) //A/N -> 12
	{
		strncpy((pCHAR)mgfDetailRec->mgfAmount_limit_1       ,pRMGD->mgfAmount_limit_1,    sizeof(mgfDetailRec->mgfAmount_limit_1) - 1);
		TrimLeadingZero(mgfDetailRec->mgfAmount_limit_1, 12);
	}
	if(true==isnum(pRMGD->mgfnbr_of_months_1, 2)) //A/N -> 2
	{
		strncpy((pCHAR)mgfDetailRec->mgfnbr_of_months_1       ,pRMGD->mgfnbr_of_months_1,    sizeof(mgfDetailRec->mgfnbr_of_months_1) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfINT_rate_1, 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfINT_rate_1       ,pRMGD->mgfINT_rate_1,    sizeof(mgfDetailRec->mgfINT_rate_1) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfMSF_rate_1, 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfMSF_rate_1       ,pRMGD->mgfMSF_rate_1,    sizeof(mgfDetailRec->mgfMSF_rate_1) - 1);
	}
	if(true==isnum(pRMGD->mgfnbr_of_months_2, 2)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfnbr_of_months_2       ,pRMGD->mgfnbr_of_months_2,    sizeof(mgfDetailRec->mgfnbr_of_months_2) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfINT_rate_2, 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfINT_rate_2       ,pRMGD->mgfINT_rate_2,    sizeof(mgfDetailRec->mgfINT_rate_2) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfMSF_rate_2, 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfMSF_rate_2       ,pRMGD->mgfMSF_rate_2,    sizeof(mgfDetailRec->mgfMSF_rate_2) - 1);
	}
	if(true==isnum(pRMGD->mgfnbr_of_months_3 , 2)) //A/N -> 2
	{
		strncpy((pCHAR)mgfDetailRec->mgfnbr_of_months_3        ,pRMGD->mgfnbr_of_months_3 ,    sizeof(mgfDetailRec->mgfnbr_of_months_3 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfINT_rate_3 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfINT_rate_3        ,pRMGD->mgfINT_rate_3 ,    sizeof(mgfDetailRec->mgfINT_rate_3 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfMSF_rate_3 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfMSF_rate_3        ,pRMGD->mgfMSF_rate_3 ,    sizeof(mgfDetailRec->mgfMSF_rate_3 ) - 1);
	}
	if(true==isnum(pRMGD->mgfnbr_of_months_4 , 2)) //A/N -> 2
	{
		strncpy((pCHAR)mgfDetailRec->mgfnbr_of_months_4        ,pRMGD->mgfnbr_of_months_4 ,    sizeof(mgfDetailRec->mgfnbr_of_months_4 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfINT_rate_4 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfINT_rate_4        ,pRMGD->mgfINT_rate_4 ,    sizeof(mgfDetailRec->mgfINT_rate_4 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfMSF_rate_4 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfMSF_rate_4        ,pRMGD->mgfMSF_rate_4 ,    sizeof(mgfDetailRec->mgfMSF_rate_4 ) - 1);
	}
	if(true==isnum(pRMGD->mgfnbr_of_months_5 , 2)) //A/N -> 2
	{
		strncpy((pCHAR)mgfDetailRec->mgfnbr_of_months_5        ,pRMGD->mgfnbr_of_months_5 ,    sizeof(mgfDetailRec->mgfnbr_of_months_5 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfINT_rate_5 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfINT_rate_5        ,pRMGD->mgfINT_rate_5 ,    sizeof(mgfDetailRec->mgfINT_rate_5 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfMSF_rate_5 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfMSF_rate_5        ,pRMGD->mgfMSF_rate_5 ,    sizeof(mgfDetailRec->mgfMSF_rate_5 ) - 1);
	}
	if(true==isnum(pRMGD->mgfnbr_of_months_6 , 2)) //A/N -> 2
	{
		strncpy((pCHAR)mgfDetailRec->mgfnbr_of_months_6        ,pRMGD->mgfnbr_of_months_6 ,    sizeof(mgfDetailRec->mgfnbr_of_months_6 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfINT_rate_6 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfINT_rate_6        ,pRMGD->mgfINT_rate_6 ,    sizeof(mgfDetailRec->mgfINT_rate_6 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfMSF_rate_6 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfMSF_rate_6        ,pRMGD->mgfMSF_rate_6 ,    sizeof(mgfDetailRec->mgfMSF_rate_6 ) - 1);
	}
	if(true==isnum(pRMGD->mgfnbr_of_months_7 , 2)) //A/N -> 2
	{
		strncpy((pCHAR)mgfDetailRec->mgfnbr_of_months_7        ,pRMGD->mgfnbr_of_months_7 ,    sizeof(mgfDetailRec->mgfnbr_of_months_7 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfINT_rate_7 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfINT_rate_7        ,pRMGD->mgfINT_rate_7 ,    sizeof(mgfDetailRec->mgfINT_rate_7 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfMSF_rate_7 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfMSF_rate_7        ,pRMGD->mgfMSF_rate_7 ,    sizeof(mgfDetailRec->mgfMSF_rate_7 ) - 1);
	}
	if(true==isnum(pRMGD->mgfnbr_of_months_8 , 2)) //A/N -> 2
	{
		strncpy((pCHAR)mgfDetailRec->mgfnbr_of_months_8        ,pRMGD->mgfnbr_of_months_8 ,    sizeof(mgfDetailRec->mgfnbr_of_months_8 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfINT_rate_8 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfINT_rate_8        ,pRMGD->mgfINT_rate_8 ,    sizeof(mgfDetailRec->mgfINT_rate_8 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfMSF_rate_8 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfMSF_rate_8        ,pRMGD->mgfMSF_rate_8 ,    sizeof(mgfDetailRec->mgfMSF_rate_8 ) - 1);
	}
	if(true==isnum(pRMGD->mgfnbr_of_months_9 , 2)) //A/N -> 2
	{
		strncpy((pCHAR)mgfDetailRec->mgfnbr_of_months_9        ,pRMGD->mgfnbr_of_months_9 ,    sizeof(mgfDetailRec->mgfnbr_of_months_9 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfINT_rate_9 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfINT_rate_9        ,pRMGD->mgfINT_rate_9 ,    sizeof(mgfDetailRec->mgfINT_rate_9 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfMSF_rate_9 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfMSF_rate_9        ,pRMGD->mgfMSF_rate_9 ,    sizeof(mgfDetailRec->mgfMSF_rate_9 ) - 1);
	}
	if(true==isnum(pRMGD->mgfnbr_of_months_10 , 2)) //A/N -> 2
	{
		strncpy((pCHAR)mgfDetailRec->mgfnbr_of_months_10        ,pRMGD->mgfnbr_of_months_10 ,    sizeof(mgfDetailRec->mgfnbr_of_months_10 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfINT_rate_10 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfINT_rate_10        ,pRMGD->mgfINT_rate_10 ,    sizeof(mgfDetailRec->mgfINT_rate_10 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfMSF_rate_10 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfMSF_rate_10        ,pRMGD->mgfMSF_rate_10 ,    sizeof(mgfDetailRec->mgfMSF_rate_10 ) - 1);
	}

	if(true==isnum(pRMGD->mgfnbr_of_months_10A , 2)) //A/N -> 2
	{
		strncpy((pCHAR)mgfDetailRec->mgfnbr_of_months_10A		,pRMGD->mgfnbr_of_months_10A ,	 sizeof(mgfDetailRec->mgfnbr_of_months_10A ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfINT_rate_10A , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfINT_rate_10A 	   ,pRMGD->mgfINT_rate_10A,    sizeof(mgfDetailRec->mgfINT_rate_10A) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfMSF_rate_10A, 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfMSF_rate_10A 	   ,pRMGD->mgfMSF_rate_10A ,    sizeof(mgfDetailRec->mgfMSF_rate_10A ) - 1);
	}
	
	if(true==isnum(pRMGD->mgfnbr_of_months_10B, 2)) //A/N -> 2
	{
		strncpy((pCHAR)mgfDetailRec->mgfnbr_of_months_10B	,pRMGD->mgfnbr_of_months_10B,    sizeof(mgfDetailRec->mgfnbr_of_months_10B) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfINT_rate_10B, 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfINT_rate_10B	,pRMGD->mgfINT_rate_10B,    sizeof(mgfDetailRec->mgfINT_rate_10B) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfMSF_rate_10B, 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfMSF_rate_10B,	pRMGD->mgfMSF_rate_10B ,    sizeof(mgfDetailRec->mgfMSF_rate_10B) - 1);
	}

	/*********************************************************************************************************************************/
	
	if(true==isalphanumSpaceDot(pRMGD->mgfAmount_limit_2 , 12)) //A/N -> 12
	{
		strncpy((pCHAR)mgfDetailRec->mgfAmount_limit_2       ,pRMGD->mgfAmount_limit_2 ,    sizeof(mgfDetailRec->mgfAmount_limit_2 ) - 1);
		TrimLeadingZero(mgfDetailRec->mgfAmount_limit_2, 12);
	}
	if(true==isnum(pRMGD->mgfnbr_of_months_11 , 2)) //A/N -> 2
	{
		strncpy((pCHAR)mgfDetailRec->mgfnbr_of_months_11        ,pRMGD->mgfnbr_of_months_11 ,    sizeof(mgfDetailRec->mgfnbr_of_months_11 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfINT_rate_11 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfINT_rate_11        ,pRMGD->mgfINT_rate_11 ,    sizeof(mgfDetailRec->mgfINT_rate_11 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfMSF_rate_11 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfMSF_rate_11        ,pRMGD->mgfMSF_rate_11 ,    sizeof(mgfDetailRec->mgfMSF_rate_11 ) - 1);
	}
	if(true==isnum(pRMGD->mgfnbr_of_months_12 , 2)) //A/N -> 2
	{
		strncpy((pCHAR)mgfDetailRec->mgfnbr_of_months_12        ,pRMGD->mgfnbr_of_months_12 ,    sizeof(mgfDetailRec->mgfnbr_of_months_12 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfINT_rate_12 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfINT_rate_12        ,pRMGD->mgfINT_rate_12 ,    sizeof(mgfDetailRec->mgfINT_rate_12 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfMSF_rate_12 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfMSF_rate_12        ,pRMGD->mgfMSF_rate_12 ,    sizeof(mgfDetailRec->mgfMSF_rate_12 ) - 1);
	}
	if(true==isnum(pRMGD->mgfnbr_of_months_13 , 2)) //A/N -> 2
	{
		strncpy((pCHAR)mgfDetailRec->mgfnbr_of_months_13        ,pRMGD->mgfnbr_of_months_13 ,    sizeof(mgfDetailRec->mgfnbr_of_months_13 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfINT_rate_13 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfINT_rate_13        ,pRMGD->mgfINT_rate_13 ,    sizeof(mgfDetailRec->mgfINT_rate_13 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfMSF_rate_13 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfMSF_rate_13        ,pRMGD->mgfMSF_rate_13 ,    sizeof(mgfDetailRec->mgfMSF_rate_13 ) - 1);
	}
	if(true==isnum(pRMGD->mgfnbr_of_months_14 , 2)) //A/N -> 2
	{
		strncpy((pCHAR)mgfDetailRec->mgfnbr_of_months_14        ,pRMGD->mgfnbr_of_months_14 ,    sizeof(mgfDetailRec->mgfnbr_of_months_14 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfINT_rate_14 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfINT_rate_14        ,pRMGD->mgfINT_rate_14 ,    sizeof(mgfDetailRec->mgfINT_rate_14 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfMSF_rate_14 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfMSF_rate_14        ,pRMGD->mgfMSF_rate_14 ,    sizeof(mgfDetailRec->mgfMSF_rate_14 ) - 1);
	}
	if(true==isnum(pRMGD->mgfnbr_of_months_15 , 2)) //A/N -> 2
	{
		strncpy((pCHAR)mgfDetailRec->mgfnbr_of_months_15        ,pRMGD->mgfnbr_of_months_15 ,    sizeof(mgfDetailRec->mgfnbr_of_months_15 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfINT_rate_15 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfINT_rate_15        ,pRMGD->mgfINT_rate_15 ,    sizeof(mgfDetailRec->mgfINT_rate_15 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfMSF_rate_15 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfMSF_rate_15        ,pRMGD->mgfMSF_rate_15 ,    sizeof(mgfDetailRec->mgfMSF_rate_15 ) - 1);
	}
	if(true==isnum(pRMGD->mgfnbr_of_months_16 , 2)) //A/N -> 2
	{
		strncpy((pCHAR)mgfDetailRec->mgfnbr_of_months_16        ,pRMGD->mgfnbr_of_months_16 ,    sizeof(mgfDetailRec->mgfnbr_of_months_16 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfINT_rate_16 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfINT_rate_16        ,pRMGD->mgfINT_rate_16 ,    sizeof(mgfDetailRec->mgfINT_rate_16 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfMSF_rate_16 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfMSF_rate_16        ,pRMGD->mgfMSF_rate_16 ,    sizeof(mgfDetailRec->mgfMSF_rate_16 ) - 1);
	}
	if(true==isnum(pRMGD->mgfnbr_of_months_17 , 2)) //A/N -> 2
	{
		strncpy((pCHAR)mgfDetailRec->mgfnbr_of_months_17        ,pRMGD->mgfnbr_of_months_17 ,    sizeof(mgfDetailRec->mgfnbr_of_months_17 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfINT_rate_17 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfINT_rate_17        ,pRMGD->mgfINT_rate_17 ,    sizeof(mgfDetailRec->mgfINT_rate_17 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfMSF_rate_17 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfMSF_rate_17        ,pRMGD->mgfMSF_rate_17 ,    sizeof(mgfDetailRec->mgfMSF_rate_17 ) - 1);
	}
	if(true==isnum(pRMGD->mgfnbr_of_months_18 , 2)) //A/N -> 2
	{
		strncpy((pCHAR)mgfDetailRec->mgfnbr_of_months_18        ,pRMGD->mgfnbr_of_months_18 ,    sizeof(mgfDetailRec->mgfnbr_of_months_18 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfINT_rate_18 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfINT_rate_18        ,pRMGD->mgfINT_rate_18 ,    sizeof(mgfDetailRec->mgfINT_rate_18 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfMSF_rate_18 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfMSF_rate_18        ,pRMGD->mgfMSF_rate_18 ,    sizeof(mgfDetailRec->mgfMSF_rate_18 ) - 1);
	}
	if(true==isnum(pRMGD->mgfnbr_of_months_19 , 2)) //A/N -> 2
	{
		strncpy((pCHAR)mgfDetailRec->mgfnbr_of_months_19        ,pRMGD->mgfnbr_of_months_19 ,    sizeof(mgfDetailRec->mgfnbr_of_months_19 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfINT_rate_19 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfINT_rate_19        ,pRMGD->mgfINT_rate_19 ,    sizeof(mgfDetailRec->mgfINT_rate_19 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfMSF_rate_19 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfMSF_rate_19        ,pRMGD->mgfMSF_rate_19 ,    sizeof(mgfDetailRec->mgfMSF_rate_19 ) - 1);
	}
	if(true==isnum(pRMGD->mgfnbr_of_months_20 , 2)) //A/N -> 2
	{
		strncpy((pCHAR)mgfDetailRec->mgfnbr_of_months_20        ,pRMGD->mgfnbr_of_months_20 ,    sizeof(mgfDetailRec->mgfnbr_of_months_20 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfINT_rate_20 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfINT_rate_20        ,pRMGD->mgfINT_rate_20 ,    sizeof(mgfDetailRec->mgfINT_rate_20 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfMSF_rate_20 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfMSF_rate_20        ,pRMGD->mgfMSF_rate_20 ,    sizeof(mgfDetailRec->mgfMSF_rate_20 ) - 1);
	}

	if(true==isnum(pRMGD->mgfnbr_of_months_20A, 2)) //A/N -> 2
	{
		strncpy((pCHAR)mgfDetailRec->mgfnbr_of_months_20A	,pRMGD->mgfnbr_of_months_20A,	 sizeof(mgfDetailRec->mgfnbr_of_months_20A) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfINT_rate_20A, 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfINT_rate_20A	,pRMGD->mgfINT_rate_20A,    sizeof(mgfDetailRec->mgfINT_rate_20A) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfMSF_rate_20A, 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfMSF_rate_20A	,pRMGD->mgfMSF_rate_20A,    sizeof(mgfDetailRec->mgfMSF_rate_20A ) - 1);
	}
	
	if(true==isnum(pRMGD->mgfnbr_of_months_20B, 2)) //A/N -> 2
	{
		strncpy((pCHAR)mgfDetailRec->mgfnbr_of_months_20B	,pRMGD->mgfnbr_of_months_20B,    sizeof(mgfDetailRec->mgfnbr_of_months_20B) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfINT_rate_20B, 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfINT_rate_20B	,pRMGD->mgfINT_rate_20B,    sizeof(mgfDetailRec->mgfINT_rate_20B) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfMSF_rate_20B, 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfMSF_rate_20B	,pRMGD->mgfMSF_rate_20B,    sizeof(mgfDetailRec->mgfMSF_rate_20B) - 1);
	}

	/*********************************************************************************************************************************/
	
	if(true==isalphanumSpaceDot(pRMGD->mgfAmount_limit_3 , 12)) //A/N -> 12
	{
		strncpy((pCHAR)mgfDetailRec->mgfAmount_limit_3       ,pRMGD->mgfAmount_limit_3 ,    sizeof(mgfDetailRec->mgfAmount_limit_3 ) - 1);
		TrimLeadingZero(mgfDetailRec->mgfAmount_limit_3, 12);
	}
	if(true==isnum(pRMGD->mgfnbr_of_months_21 , 2)) //A/N -> 2
	{
		strncpy((pCHAR)mgfDetailRec->mgfnbr_of_months_21        ,pRMGD->mgfnbr_of_months_21 ,    sizeof(mgfDetailRec->mgfnbr_of_months_21 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfINT_rate_21 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfINT_rate_21        ,pRMGD->mgfINT_rate_21 ,    sizeof(mgfDetailRec->mgfINT_rate_21 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfMSF_rate_21 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfMSF_rate_21        ,pRMGD->mgfMSF_rate_21 ,    sizeof(mgfDetailRec->mgfMSF_rate_21 ) - 1);
	}
	if(true==isnum(pRMGD->mgfnbr_of_months_22 , 2)) //A/N -> 2
	{
		strncpy((pCHAR)mgfDetailRec->mgfnbr_of_months_22        ,pRMGD->mgfnbr_of_months_22 ,    sizeof(mgfDetailRec->mgfnbr_of_months_22 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfINT_rate_22 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfINT_rate_22        ,pRMGD->mgfINT_rate_22 ,    sizeof(mgfDetailRec->mgfINT_rate_22 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfMSF_rate_22 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfMSF_rate_22        ,pRMGD->mgfMSF_rate_22 ,    sizeof(mgfDetailRec->mgfMSF_rate_22 ) - 1);
	}
	if(true==isnum(pRMGD->mgfnbr_of_months_23 , 2)) //A/N -> 2
	{
		strncpy((pCHAR)mgfDetailRec->mgfnbr_of_months_23        ,pRMGD->mgfnbr_of_months_23 ,    sizeof(mgfDetailRec->mgfnbr_of_months_23 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfINT_rate_23 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfINT_rate_23        ,pRMGD->mgfINT_rate_23 ,    sizeof(mgfDetailRec->mgfINT_rate_23 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfMSF_rate_23 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfMSF_rate_23        ,pRMGD->mgfMSF_rate_23 ,    sizeof(mgfDetailRec->mgfMSF_rate_23 ) - 1);
	}
	if(true==isnum(pRMGD->mgfnbr_of_months_24 , 2)) //A/N -> 2
	{
		strncpy((pCHAR)mgfDetailRec->mgfnbr_of_months_24        ,pRMGD->mgfnbr_of_months_24 ,    sizeof(mgfDetailRec->mgfnbr_of_months_24 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfINT_rate_24 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfINT_rate_24        ,pRMGD->mgfINT_rate_24 ,    sizeof(mgfDetailRec->mgfINT_rate_24 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfMSF_rate_24 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfMSF_rate_24        ,pRMGD->mgfMSF_rate_24 ,    sizeof(mgfDetailRec->mgfMSF_rate_24 ) - 1);
	}
	if(true==isnum(pRMGD->mgfnbr_of_months_25 , 2)) //A/N -> 2
	{
		strncpy((pCHAR)mgfDetailRec->mgfnbr_of_months_25        ,pRMGD->mgfnbr_of_months_25 ,    sizeof(mgfDetailRec->mgfnbr_of_months_25 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfINT_rate_25 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfINT_rate_25        ,pRMGD->mgfINT_rate_25 ,    sizeof(mgfDetailRec->mgfINT_rate_25 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfMSF_rate_25 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfMSF_rate_25        ,pRMGD->mgfMSF_rate_25 ,    sizeof(mgfDetailRec->mgfMSF_rate_25 ) - 1);
	}
	if(true==isnum(pRMGD->mgfnbr_of_months_26 , 2)) //A/N -> 2
	{
		strncpy((pCHAR)mgfDetailRec->mgfnbr_of_months_26        ,pRMGD->mgfnbr_of_months_26 ,    sizeof(mgfDetailRec->mgfnbr_of_months_26 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfINT_rate_26 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfINT_rate_26        ,pRMGD->mgfINT_rate_26 ,    sizeof(mgfDetailRec->mgfINT_rate_26 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfMSF_rate_26 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfMSF_rate_26        ,pRMGD->mgfMSF_rate_26 ,    sizeof(mgfDetailRec->mgfMSF_rate_26 ) - 1);
	}
	if(true==isnum(pRMGD->mgfnbr_of_months_27 , 2)) //A/N -> 2
	{
		strncpy((pCHAR)mgfDetailRec->mgfnbr_of_months_27        ,pRMGD->mgfnbr_of_months_27 ,    sizeof(mgfDetailRec->mgfnbr_of_months_27 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfINT_rate_27 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfINT_rate_27        ,pRMGD->mgfINT_rate_27 ,    sizeof(mgfDetailRec->mgfINT_rate_27 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfMSF_rate_27 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfMSF_rate_27        ,pRMGD->mgfMSF_rate_27 ,    sizeof(mgfDetailRec->mgfMSF_rate_27 ) - 1);
	}
	if(true==isnum(pRMGD->mgfnbr_of_months_28 , 2)) //A/N -> 2
	{
		strncpy((pCHAR)mgfDetailRec->mgfnbr_of_months_28        ,pRMGD->mgfnbr_of_months_28 ,    sizeof(mgfDetailRec->mgfnbr_of_months_28 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfINT_rate_28 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfINT_rate_28        ,pRMGD->mgfINT_rate_28 ,    sizeof(mgfDetailRec->mgfINT_rate_28 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfMSF_rate_28 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfMSF_rate_28        ,pRMGD->mgfMSF_rate_28 ,    sizeof(mgfDetailRec->mgfMSF_rate_28 ) - 1);
	}
	if(true==isnum(pRMGD->mgfnbr_of_months_29 , 2)) //A/N -> 2
	{
		strncpy((pCHAR)mgfDetailRec->mgfnbr_of_months_29        ,pRMGD->mgfnbr_of_months_29 ,    sizeof(mgfDetailRec->mgfnbr_of_months_29 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfINT_rate_29 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfINT_rate_29        ,pRMGD->mgfINT_rate_29 ,    sizeof(mgfDetailRec->mgfINT_rate_29 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfMSF_rate_29 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfMSF_rate_29        ,pRMGD->mgfMSF_rate_29 ,    sizeof(mgfDetailRec->mgfMSF_rate_29 ) - 1);
	}
	if(true==isnum(pRMGD->mgfnbr_of_months_30 , 2)) //A/N -> 2
	{
		strncpy((pCHAR)mgfDetailRec->mgfnbr_of_months_30        ,pRMGD->mgfnbr_of_months_30 ,    sizeof(mgfDetailRec->mgfnbr_of_months_30 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfINT_rate_30 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfINT_rate_30        ,pRMGD->mgfINT_rate_30 ,    sizeof(mgfDetailRec->mgfINT_rate_30 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfMSF_rate_30 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfMSF_rate_30        ,pRMGD->mgfMSF_rate_30 ,    sizeof(mgfDetailRec->mgfMSF_rate_30 ) - 1);
	}

	if(true==isnum(pRMGD->mgfnbr_of_months_30A, 2)) //A/N -> 2
	{
		strncpy((pCHAR)mgfDetailRec->mgfnbr_of_months_30A	,pRMGD->mgfnbr_of_months_30A,	 sizeof(mgfDetailRec->mgfnbr_of_months_30A) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfINT_rate_30A, 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfINT_rate_30A	,pRMGD->mgfINT_rate_30A,    sizeof(mgfDetailRec->mgfINT_rate_30A) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfMSF_rate_30A, 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfMSF_rate_30A	,pRMGD->mgfMSF_rate_30A,    sizeof(mgfDetailRec->mgfMSF_rate_30A) - 1);
	}
		
	if(true==isnum(pRMGD->mgfnbr_of_months_30B, 2)) //A/N -> 2
	{
		strncpy((pCHAR)mgfDetailRec->mgfnbr_of_months_30B	,pRMGD->mgfnbr_of_months_30B,    sizeof(mgfDetailRec->mgfnbr_of_months_30B) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfINT_rate_30B, 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfINT_rate_30B	,pRMGD->mgfINT_rate_30B,    sizeof(mgfDetailRec->mgfINT_rate_30B) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfMSF_rate_30B, 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfMSF_rate_30B	,pRMGD->mgfMSF_rate_30B,    sizeof(mgfDetailRec->mgfMSF_rate_30B) - 1);
	}

	/*********************************************************************************************************************************/
	if(true==isalphanumSpaceDot(pRMGD->mgfAmount_limit_4 , 12)) //A/N -> 12
	{
		strncpy((pCHAR)mgfDetailRec->mgfAmount_limit_4       ,pRMGD->mgfAmount_limit_4 ,    sizeof(mgfDetailRec->mgfAmount_limit_4 ) - 1);
		TrimLeadingZero(mgfDetailRec->mgfAmount_limit_4, 12);
	}
	if(true==isnum(pRMGD->mgfnbr_of_months_31 , 2)) //A/N -> 2
	{
		strncpy((pCHAR)mgfDetailRec->mgfnbr_of_months_31        ,pRMGD->mgfnbr_of_months_31 ,    sizeof(mgfDetailRec->mgfnbr_of_months_31 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfINT_rate_31 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfINT_rate_31        ,pRMGD->mgfINT_rate_31 ,    sizeof(mgfDetailRec->mgfINT_rate_31 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfMSF_rate_31 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfMSF_rate_31        ,pRMGD->mgfMSF_rate_31 ,    sizeof(mgfDetailRec->mgfMSF_rate_31 ) - 1);
	}
	if(true==isnum(pRMGD->mgfnbr_of_months_32 , 2)) //A/N -> 2
	{
		strncpy((pCHAR)mgfDetailRec->mgfnbr_of_months_32        ,pRMGD->mgfnbr_of_months_32 ,    sizeof(mgfDetailRec->mgfnbr_of_months_32 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfINT_rate_32 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfINT_rate_32        ,pRMGD->mgfINT_rate_32 ,    sizeof(mgfDetailRec->mgfINT_rate_32 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfMSF_rate_32 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfMSF_rate_32        ,pRMGD->mgfMSF_rate_32 ,    sizeof(mgfDetailRec->mgfMSF_rate_32 ) - 1);
	}
	if(true==isnum(pRMGD->mgfnbr_of_months_33 , 2)) //A/N -> 2
	{
		strncpy((pCHAR)mgfDetailRec->mgfnbr_of_months_33        ,pRMGD->mgfnbr_of_months_33 ,    sizeof(mgfDetailRec->mgfnbr_of_months_33 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfINT_rate_33 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfINT_rate_33        ,pRMGD->mgfINT_rate_33 ,    sizeof(mgfDetailRec->mgfINT_rate_33 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfMSF_rate_33 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfMSF_rate_33        ,pRMGD->mgfMSF_rate_33 ,    sizeof(mgfDetailRec->mgfMSF_rate_33 ) - 1);
	}
	if(true==isnum(pRMGD->mgfnbr_of_months_34 , 2)) //A/N -> 2
	{
		strncpy((pCHAR)mgfDetailRec->mgfnbr_of_months_34        ,pRMGD->mgfnbr_of_months_34 ,    sizeof(mgfDetailRec->mgfnbr_of_months_34 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfINT_rate_34 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfINT_rate_34        ,pRMGD->mgfINT_rate_34 ,    sizeof(mgfDetailRec->mgfINT_rate_34 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfMSF_rate_34 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfMSF_rate_34        ,pRMGD->mgfMSF_rate_34 ,    sizeof(mgfDetailRec->mgfMSF_rate_34 ) - 1);
	}
	if(true==isnum(pRMGD->mgfnbr_of_months_35 , 2)) //A/N -> 2
	{
		strncpy((pCHAR)mgfDetailRec->mgfnbr_of_months_35        ,pRMGD->mgfnbr_of_months_35 ,    sizeof(mgfDetailRec->mgfnbr_of_months_35 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfINT_rate_35 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfINT_rate_35        ,pRMGD->mgfINT_rate_35 ,    sizeof(mgfDetailRec->mgfINT_rate_35 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfMSF_rate_35 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfMSF_rate_35        ,pRMGD->mgfMSF_rate_35 ,    sizeof(mgfDetailRec->mgfMSF_rate_35 ) - 1);
	}
	if(true==isnum(pRMGD->mgfnbr_of_months_36 , 2)) //A/N -> 2
	{
		strncpy((pCHAR)mgfDetailRec->mgfnbr_of_months_36        ,pRMGD->mgfnbr_of_months_36 ,    sizeof(mgfDetailRec->mgfnbr_of_months_36 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfINT_rate_36 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfINT_rate_36        ,pRMGD->mgfINT_rate_36 ,    sizeof(mgfDetailRec->mgfINT_rate_36 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfMSF_rate_36 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfMSF_rate_36        ,pRMGD->mgfMSF_rate_36 ,    sizeof(mgfDetailRec->mgfMSF_rate_36 ) - 1);
	}
	if(true==isnum(pRMGD->mgfnbr_of_months_37 , 2)) //A/N -> 2
	{
		strncpy((pCHAR)mgfDetailRec->mgfnbr_of_months_37        ,pRMGD->mgfnbr_of_months_37 ,    sizeof(mgfDetailRec->mgfnbr_of_months_37 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfINT_rate_37 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfINT_rate_37        ,pRMGD->mgfINT_rate_37 ,    sizeof(mgfDetailRec->mgfINT_rate_37 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfMSF_rate_37 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfMSF_rate_37        ,pRMGD->mgfMSF_rate_37 ,    sizeof(mgfDetailRec->mgfMSF_rate_37 ) - 1);
	}
	if(true==isnum(pRMGD->mgfnbr_of_months_38 , 2)) //A/N -> 2
	{
		strncpy((pCHAR)mgfDetailRec->mgfnbr_of_months_38        ,pRMGD->mgfnbr_of_months_38 ,    sizeof(mgfDetailRec->mgfnbr_of_months_38 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfINT_rate_38 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfINT_rate_38        ,pRMGD->mgfINT_rate_38 ,    sizeof(mgfDetailRec->mgfINT_rate_38 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfMSF_rate_38 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfMSF_rate_38        ,pRMGD->mgfMSF_rate_38 ,    sizeof(mgfDetailRec->mgfMSF_rate_38 ) - 1);
	}
	if(true==isnum(pRMGD->mgfnbr_of_months_39 , 2)) //A/N -> 2
	{
		strncpy((pCHAR)mgfDetailRec->mgfnbr_of_months_39        ,pRMGD->mgfnbr_of_months_39 ,    sizeof(mgfDetailRec->mgfnbr_of_months_39 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfINT_rate_39 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfINT_rate_39        ,pRMGD->mgfINT_rate_39 ,    sizeof(mgfDetailRec->mgfINT_rate_39 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfMSF_rate_39 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfMSF_rate_39        ,pRMGD->mgfMSF_rate_39 ,    sizeof(mgfDetailRec->mgfMSF_rate_39 ) - 1);
	}
	if(true==isnum(pRMGD->mgfnbr_of_months_40 , 2)) //A/N -> 2
	{
		strncpy((pCHAR)mgfDetailRec->mgfnbr_of_months_40        ,pRMGD->mgfnbr_of_months_40 ,    sizeof(mgfDetailRec->mgfnbr_of_months_40 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfINT_rate_40 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfINT_rate_40        ,pRMGD->mgfINT_rate_40 ,    sizeof(mgfDetailRec->mgfINT_rate_40 ) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfMSF_rate_40 , 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfMSF_rate_40        ,pRMGD->mgfMSF_rate_40 ,    sizeof(mgfDetailRec->mgfMSF_rate_40 ) - 1);
	}

	if(true==isnum(pRMGD->mgfnbr_of_months_40A, 2)) //A/N -> 2
	{
		strncpy((pCHAR)mgfDetailRec->mgfnbr_of_months_40A	,pRMGD->mgfnbr_of_months_40A,	 sizeof(mgfDetailRec->mgfnbr_of_months_40A) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfINT_rate_40A, 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfINT_rate_40A	,pRMGD->mgfINT_rate_40A,    sizeof(mgfDetailRec->mgfINT_rate_40A) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfMSF_rate_40A, 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfMSF_rate_40A	,pRMGD->mgfMSF_rate_40A,    sizeof(mgfDetailRec->mgfMSF_rate_40A) - 1);
	}

	if(true==isnum(pRMGD->mgfnbr_of_months_40B, 2)) //A/N -> 2
	{
		strncpy((pCHAR)mgfDetailRec->mgfnbr_of_months_40B	,pRMGD->mgfnbr_of_months_40B,    sizeof(mgfDetailRec->mgfnbr_of_months_40B) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfINT_rate_40B, 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfINT_rate_40B	,pRMGD->mgfINT_rate_40B,    sizeof(mgfDetailRec->mgfINT_rate_40B) - 1);
	}
	if(true==isalphanumSpaceDot(pRMGD->mgfMSF_rate_40B, 7)) //A/N -> 7
	{
		strncpy((pCHAR)mgfDetailRec->mgfMSF_rate_40B,	pRMGD->mgfMSF_rate_40B,    sizeof(mgfDetailRec->mgfMSF_rate_40B) - 1);
	}

	/*********************************************************************************************************************************/
		
	if(true==isalphanumSpaceDot(pRMGD->mgfFiller, 10)) //A/N -> 10
	{
		strncpy((pCHAR)mgfDetailRec->mgfFiller        ,pRMGD->mgfFiller ,    sizeof(mgfDetailRec->mgfFiller ) - 1);
	}

	returnNumAscii((pCHAR)mgfDetailRec->mgfMerchantGroupID);
	returnNumbers((pCHAR)mgfDetailRec->mgfEffectiveDate);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfAmount_limit_1);
	returnNumbers((pCHAR)mgfDetailRec->mgfnbr_of_months_1);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfINT_rate_1);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfMSF_rate_1);
	returnNumbers((pCHAR)mgfDetailRec->mgfnbr_of_months_2);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfINT_rate_2);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfMSF_rate_2);
	returnNumbers((pCHAR)mgfDetailRec->mgfnbr_of_months_3);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfINT_rate_3);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfMSF_rate_3);
	returnNumbers((pCHAR)mgfDetailRec->mgfnbr_of_months_4);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfINT_rate_4);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfMSF_rate_4);
	returnNumbers((pCHAR)mgfDetailRec->mgfnbr_of_months_5);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfINT_rate_5);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfMSF_rate_5);
	returnNumbers((pCHAR)mgfDetailRec->mgfnbr_of_months_6);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfINT_rate_6);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfMSF_rate_6);
	returnNumbers((pCHAR)mgfDetailRec->mgfnbr_of_months_7);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfINT_rate_7);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfMSF_rate_7);
	returnNumbers((pCHAR)mgfDetailRec->mgfnbr_of_months_8);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfINT_rate_8);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfMSF_rate_8);
	returnNumbers((pCHAR)mgfDetailRec->mgfnbr_of_months_9);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfINT_rate_9);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfMSF_rate_9);
	returnNumbers((pCHAR)mgfDetailRec->mgfnbr_of_months_10);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfINT_rate_10);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfMSF_rate_10);
	returnNumbers((pCHAR)mgfDetailRec->mgfnbr_of_months_10A);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfINT_rate_10A);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfMSF_rate_10A);
	returnNumbers((pCHAR)mgfDetailRec->mgfnbr_of_months_10B);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfINT_rate_10B);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfMSF_rate_10B);
	
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfAmount_limit_2);
	returnNumbers((pCHAR)mgfDetailRec->mgfnbr_of_months_11);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfINT_rate_11);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfMSF_rate_11);
	returnNumbers((pCHAR)mgfDetailRec->mgfnbr_of_months_12);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfINT_rate_12);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfMSF_rate_12);
	returnNumbers((pCHAR)mgfDetailRec->mgfnbr_of_months_13);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfINT_rate_13);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfMSF_rate_13);
	returnNumbers((pCHAR)mgfDetailRec->mgfnbr_of_months_14);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfINT_rate_14);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfMSF_rate_14);
	returnNumbers((pCHAR)mgfDetailRec->mgfnbr_of_months_15);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfINT_rate_15);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfMSF_rate_15);
	returnNumbers((pCHAR)mgfDetailRec->mgfnbr_of_months_16);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfINT_rate_16);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfMSF_rate_16);
	returnNumbers((pCHAR)mgfDetailRec->mgfnbr_of_months_17);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfINT_rate_17);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfMSF_rate_17);
	returnNumbers((pCHAR)mgfDetailRec->mgfnbr_of_months_18);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfINT_rate_18);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfMSF_rate_18);
	returnNumbers((pCHAR)mgfDetailRec->mgfnbr_of_months_19);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfINT_rate_19);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfMSF_rate_19);
	returnNumbers((pCHAR)mgfDetailRec->mgfnbr_of_months_20);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfINT_rate_20);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfMSF_rate_20);
	returnNumbers((pCHAR)mgfDetailRec->mgfnbr_of_months_20A);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfINT_rate_20A);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfMSF_rate_20A);
	returnNumbers((pCHAR)mgfDetailRec->mgfnbr_of_months_20B);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfINT_rate_20B);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfMSF_rate_20B);
	
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfAmount_limit_3);
	returnNumbers((pCHAR)mgfDetailRec->mgfnbr_of_months_21);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfINT_rate_21);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfMSF_rate_21);
	returnNumbers((pCHAR)mgfDetailRec->mgfnbr_of_months_22);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfINT_rate_22);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfMSF_rate_22);
	returnNumbers((pCHAR)mgfDetailRec->mgfnbr_of_months_23);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfINT_rate_23);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfMSF_rate_23);
	returnNumbers((pCHAR)mgfDetailRec->mgfnbr_of_months_24);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfINT_rate_24);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfMSF_rate_24);
	returnNumbers((pCHAR)mgfDetailRec->mgfnbr_of_months_25);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfINT_rate_25);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfMSF_rate_25);
	returnNumbers((pCHAR)mgfDetailRec->mgfnbr_of_months_26);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfINT_rate_26);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfMSF_rate_26);
	returnNumbers((pCHAR)mgfDetailRec->mgfnbr_of_months_27);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfINT_rate_27);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfMSF_rate_27);
	returnNumbers((pCHAR)mgfDetailRec->mgfnbr_of_months_28);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfINT_rate_28);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfMSF_rate_28);
	returnNumbers((pCHAR)mgfDetailRec->mgfnbr_of_months_29);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfINT_rate_29);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfMSF_rate_29);
	returnNumbers((pCHAR)mgfDetailRec->mgfnbr_of_months_30);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfINT_rate_30);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfMSF_rate_30);
	returnNumbers((pCHAR)mgfDetailRec->mgfnbr_of_months_30A);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfINT_rate_30A);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfMSF_rate_30A);
	returnNumbers((pCHAR)mgfDetailRec->mgfnbr_of_months_30B);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfINT_rate_30B);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfMSF_rate_30B);
	
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfAmount_limit_4);
	returnNumbers((pCHAR)mgfDetailRec->mgfnbr_of_months_31);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfINT_rate_31);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfMSF_rate_31);
	returnNumbers((pCHAR)mgfDetailRec->mgfnbr_of_months_32);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfINT_rate_32);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfMSF_rate_32);
	returnNumbers((pCHAR)mgfDetailRec->mgfnbr_of_months_33);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfINT_rate_33);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfMSF_rate_33);
	returnNumbers((pCHAR)mgfDetailRec->mgfnbr_of_months_34);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfINT_rate_34);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfMSF_rate_34);
	returnNumbers((pCHAR)mgfDetailRec->mgfnbr_of_months_35);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfINT_rate_35);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfMSF_rate_35);
	returnNumbers((pCHAR)mgfDetailRec->mgfnbr_of_months_36);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfINT_rate_36);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfMSF_rate_36);
	returnNumbers((pCHAR)mgfDetailRec->mgfnbr_of_months_37);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfINT_rate_37);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfMSF_rate_37);
	returnNumbers((pCHAR)mgfDetailRec->mgfnbr_of_months_38);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfINT_rate_38);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfMSF_rate_38);
	returnNumbers((pCHAR)mgfDetailRec->mgfnbr_of_months_39);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfINT_rate_39);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfMSF_rate_39);
	returnNumbers((pCHAR)mgfDetailRec->mgfnbr_of_months_40);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfINT_rate_40);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfMSF_rate_40);
	returnNumbers((pCHAR)mgfDetailRec->mgfnbr_of_months_40A);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfINT_rate_40A);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfMSF_rate_40A);
	returnNumbers((pCHAR)mgfDetailRec->mgfnbr_of_months_40B);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfINT_rate_40B);
	returnNumdotMGRP((pCHAR)mgfDetailRec->mgfMSF_rate_40B);
}
	
	
/*end of parseMerchantGroupDetail*/



/*************************************************************************************
       NAME:           parseMerchantGroupTrailer()

       DESCRIPTION:    This module parses the trailer of the MerchantGroup ascii file.
					        It populates the myMerchantGroupTrailer structure.
                       
       INPUTS:         dataRecord is CHAR array which containes the trailer record 
                       from the MerchantGroup file.
	    OUTPUTS:
                       structure of type myMerchantGroupTrailer
       RETURNS:        
		   None.                       
                               
      AUTHOR:          Faramarz Arad.
      MODIFIED BY:     None.
*************************************************************************************/
void parseMerchantGroupTrailer(pCHAR dataRecord, pmyMerchantGroupTrailer mgfTrailRec)
{
	memset(mgfTrailRec,0,sizeof(myMerchantGroupTrailer));

	strncpy((pCHAR)mgfTrailRec->trailRecordType     , dataRecord     , sizeof(mgfTrailRec->trailRecordType)     -1);
	strncpy((pCHAR)mgfTrailRec->trailFileName       , dataRecord + 3 , sizeof(mgfTrailRec->trailFileName)       -1);
	strncpy((pCHAR)mgfTrailRec->trailFileCreateDate , dataRecord + 11, sizeof(mgfTrailRec->trailFileCreateDate) -1);
	strncpy((pCHAR)mgfTrailRec->trailNumberofRecord , dataRecord + 17, sizeof(mgfTrailRec->trailNumberofRecord) -1);
	strncpy((pCHAR)mgfTrailRec->trailFiller         , dataRecord + 26, sizeof(mgfTrailRec->trailFiller)         -1);

	/*The next command will make sure the inputs are numbers only.*/
	returnNumbers((pCHAR)mgfTrailRec->trailFileCreateDate);
	returnNumbers((pCHAR)mgfTrailRec->trailNumberofRecord);

}  /*end of parseMerchantGroupTrailer.*/




/*************************************************************************************
       NAME:           parseNegFraudDetail()

       DESCRIPTION:    This module is parses the detail of the Negative Fraud ascii file.
					   It populates the myNegFraud Detail structure.
                       
       INPUTS:         dataRecord is CHAR array which containes the detail record

	   OUTPUTS:
                       structure of type myNegFraudDetail 
       RETURNS:        
		   None.                       
                               
      AUTHOR:          L Mitchell 5/21/04
      MODIFIED BY:     
*************************************************************************************/
void parseNegFraudDetail(pCHAR dataRecord, pNEGATIVE_FRAUD neg_fraud)
{
	pRawNegFraudDetail p_rNegFraud;

	char panlen[3] = {0}; 

	memset(neg_fraud, 0, sizeof(NEGATIVE_FRAUD));
	p_rNegFraud = (pRawNegFraudDetail)dataRecord;

	strncpy((pCHAR)neg_fraud->primary_key.mcc	    , p_rNegFraud->detailMCC            ,sizeof(neg_fraud->primary_key.mcc ) - 1);
	strncpy(neg_fraud->primary_key.bin_low           , p_rNegFraud->detailBinLow         ,sizeof(neg_fraud->primary_key.bin_low)      - 1);
	strncpy(neg_fraud->primary_key.bin_high          , p_rNegFraud->detailBinHigh        ,sizeof(neg_fraud->primary_key.bin_high)     - 1);
	strncpy(neg_fraud->country_code                  , p_rNegFraud->detailCountryCode    , sizeof(neg_fraud->country_code )  -1); 
	memcpy(panlen, p_rNegFraud->detailPanLength, sizeof(panlen) -1);
	neg_fraud->primary_key.pan_length = atoi(panlen);

	/*the next command would make sure the input are numbers.*/
	returnNumbers(neg_fraud->primary_key.mcc);
	returnNumbers(neg_fraud->primary_key.bin_low);
	returnNumbers(neg_fraud->primary_key.bin_high);

	/*calculate bin_length field using the number of digits in the bin_low field*/
	neg_fraud->bin_length = strlen(neg_fraud->primary_key.bin_low);

	/* If country code = spaces, for the database, store nulls*/



}  /*end of parseNegFraudDetail*/


/*************************************************************************************
       NAME:           parseNegFraudHeader()

       DESCRIPTION:    This module is parses the header of the negative fraud ascii file.
					   It populates the myNegFraudHeader structure.
                       
       INPUTS:         dataRecord is CHAR array which containes the header record 
                       from the terminal import file.
	    OUTPUTS:
                       structure of type myNegFraudHeader 
       RETURNS:        
		   None.                       
                               
      AUTHOR:         L Mitchell 5/24/04
      MODIFIED BY:     None.
*************************************************************************************/
void parseNegFraudHeader(pCHAR dataRecord, pmyNegFraudHeader NegFraudHeadRecord)
{
	memset(NegFraudHeadRecord,0,sizeof(myNegFraudHeader));

	memcpy((pCHAR)NegFraudHeadRecord->headRecordType     , dataRecord      ,sizeof(NegFraudHeadRecord->headRecordType     ) - 1);
	memcpy((pCHAR)NegFraudHeadRecord->headFileName       , dataRecord+3    ,sizeof(NegFraudHeadRecord->headFileName       ) - 1);
	memcpy((pCHAR)NegFraudHeadRecord->headFileCreateDate , dataRecord + 11 ,sizeof(NegFraudHeadRecord->headFileCreateDate ) - 1);

	/*the next command will make sure the input has numbers only.*/
	returnNumbers(NegFraudHeadRecord->headFileCreateDate);

}  /*end of parseNegFraudHeader*/

/*************************************************************************************/
/*************************************************************************************
       NAME:           parseNegFraudTrailer()

       DESCRIPTION:    This module parses the trailer record. 
					   
                       
       INPUTS:         dataRecord is CHAR array which containes the negative fraud record 
                       from the negative fraud import file.
       OUTPUTS:
                       structure of type myNetativeFraudDetail.
       RETURNS:        
		   None.                       
                               
      AUTHOR:         
      MODIFIED BY:    None.
*************************************************************************************/
void parseNegFraudTrailer(pCHAR dataRecord, pmyNegFraudTrailer NegFraudTrailRecord)
{
   memset(NegFraudTrailRecord,0,sizeof(myNegFraudTrailer));

   memcpy(NegFraudTrailRecord->trailRecordType     , dataRecord     , sizeof(NegFraudTrailRecord->trailRecordType     ) - 1);
   memcpy(NegFraudTrailRecord->trailFileName       , dataRecord + 3 , sizeof(NegFraudTrailRecord->trailFileName       ) - 1);
   memcpy(NegFraudTrailRecord->trailFileCreateDate , dataRecord + 11, sizeof(NegFraudTrailRecord->trailFileCreateDate ) - 1);
   memcpy(NegFraudTrailRecord->trailNumberofRecords, dataRecord + 17, sizeof(NegFraudTrailRecord->trailNumberofRecords) - 1);

   /*the next command would make sure all of the input are numbers.*/
   returnNumbers(NegFraudTrailRecord->trailFileCreateDate);
   returnNumbers(NegFraudTrailRecord->trailNumberofRecords);

}  /*end of parseNegFraudTrailer*/


/*************************************************************************************/
void parseEDTRDetail(pCHAR dataRecord, pDCF01 dcf01, pCHAR ChipActiveFlag)
{
	pRawEDTRDetail pRTD;
	BYTE               device_type[2] = "";
	CHAR	             temp_organization_number[5]="0000";
	CHAR				 temp_merchant_id[16]="";

	memset( dcf01, 0x00, sizeof(DCF01) );
	pRTD = (pRawEDTRDetail)dataRecord;

	
	if(true==isnumSpace(pRTD->detailMerchantNumber, 10))//N -> 15
	{
		memcpy((pCHAR)dcf01->merchant_id           ,pRTD->detailMerchantNumber ,  10) ;
		memcpy((pCHAR)dcf01->organization_id       ,temp_organization_number ,  sizeof(dcf01->organization_id       ) - 1);
		sprintf(temp_merchant_id, "%015s", dcf01->merchant_id);
		memcpy((pCHAR)dcf01->merchant_id           ,temp_merchant_id ,  (sizeof(dcf01->merchant_id  )- 1));
	}
	if(true==isalphanumSpace(pRTD->detailTerminalNumber, 8))//N -> 8
	{
		memcpy((pCHAR)dcf01->primary_key.device_id ,pRTD->detailTerminalNumber ,  sizeof(dcf01->primary_key.device_id ) - 1);
	}
	if(true==isnumSpace(pRTD->detailMCC , 4))//N -> 4
	{
		memcpy((pCHAR)dcf01->category_code         ,pRTD->detailMCC            ,  sizeof(dcf01->category_code         ) - 1);
	}

	if(true==isnum(pRTD->ChipActivatedFlag))
	{
		memcpy (ChipActiveFlag ,pRTD->ChipActivatedFlag,1);
		if(0==strncmp(ChipActiveFlag,"1",1))
		{
			memcpy( dcf01->device_type, "07", 2 ); // Default to ISO Device
			memcpy((pCHAR)dcf01->status ,"A", 1); // Make devce active.
		}
		else
		{
			strncpy(ChipActiveFlag,"0",1);
		}
	}
	else
	{
		strncpy(ChipActiveFlag,"0",1);
	}
	
	/* The next set of commands will make sure the inputs are all numbers. */
	returnNumbers((pCHAR)dcf01->primary_key.device_id       );
	returnNumbers((pCHAR)dcf01->merchant_id                 );
	returnNumbers((pCHAR)dcf01->organization_id             );
	returnNumbers((pCHAR)dcf01->category_code               );

	return;
} /*end of parseEDTRDetail*/

/*************************************************************************************/
void parseEDTRHeader(pCHAR dataRecord, pmyEDTRHeader EDTRHeadRecord)
{
   memset(EDTRHeadRecord,0,sizeof(myEDTRHeader));

   strncpy(EDTRHeadRecord->headerRecordType     , dataRecord      ,sizeof(EDTRHeadRecord->headerRecordType     ) - 1);
   strncpy(EDTRHeadRecord->headerFileName       , dataRecord+3    ,sizeof(EDTRHeadRecord->headerFileName       ) - 1);
   strncpy(EDTRHeadRecord->headerFileCreateDate , dataRecord + 11 ,sizeof(EDTRHeadRecord->headerFileCreateDate ) - 1);
   strncpy(EDTRHeadRecord->headerFiller         , dataRecord+17   ,sizeof(EDTRHeadRecord->headerFiller         ) - 1);     
 
   /*the next command will make sure the input has numbers only.*/
   returnNumbers(EDTRHeadRecord->headerFileCreateDate);

}  /*end of parseEDTRHeader*/


/*************************************************************************************/
void parseEDTRTrailer(pCHAR dataRecord, pmyEDTRTrailer EDTRTrailRecord)
{
   memset(EDTRTrailRecord,0,sizeof(myTerminalTrailer));

   strncpy((pCHAR)EDTRTrailRecord->trailRecordType     , dataRecord     , sizeof(EDTRTrailRecord->trailRecordType     ) - 1);
   strncpy((pCHAR)EDTRTrailRecord->trailFileName       , dataRecord + 3 , sizeof(EDTRTrailRecord->trailFileName       ) - 1);
   strncpy((pCHAR)EDTRTrailRecord->trailFileCreateDate , dataRecord + 11, sizeof(EDTRTrailRecord->trailFileCreateDate ) - 1);
   strncpy((pCHAR)EDTRTrailRecord->trailNumberofRecords, dataRecord + 17, sizeof(EDTRTrailRecord->trailNumberofRecords) - 1);
   strncpy((pCHAR)EDTRTrailRecord->trailFiller         , dataRecord + 26, sizeof(EDTRTrailRecord->trailFiller         ) - 1);

   /*the next command would make sure all of the input are numbers.*/
   returnNumbers((pCHAR)EDTRTrailRecord->trailFileCreateDate);
   returnNumbers((pCHAR)EDTRTrailRecord->trailNumberofRecords);

}  /*end of parseEDTRTrailer*/


/*************************************************************************************/
void parseMPAVDetail(pCHAR dataRecord, pmyMPAVDetail merchDetailRec)
{
	pRawMPAVDetail pRMD;
	int 		temp_PreAuthValidationFlag=0;
	CHAR 		str_temp_PreAuthValidationFlag[2]={0};
	CHAR		temp_organization_number[5]="0000";
	pRMD = (pRawMPAVDetail)dataRecord;

	strncpy((pCHAR)merchDetailRec->detailOrganizaNumber
					 ,temp_organization_number,
					 sizeof(merchDetailRec->detailOrganizaNumber) - 1);
	if(true==isnumSpace(pRMD->detailMerchantNumber, 15))//N -> 15
	{
		 strncpy((pCHAR)merchDetailRec->detailMerchantNumber
				 ,pRMD->detailMerchantNumber,
				 sizeof(merchDetailRec->detailMerchantNumber) - 1);
	}
	if(true==isnum( pRMD->PreAuthValidationFlag, 1))//A ->
	{
		strncpy(str_temp_PreAuthValidationFlag,pRMD->PreAuthValidationFlag,1);
		temp_PreAuthValidationFlag = atoi(str_temp_PreAuthValidationFlag);
		if(temp_PreAuthValidationFlag == 1)
		{
			strncpy ((pCHAR)merchDetailRec->PreAuthValidationFlag ,"Y", 1);
		}
		else
		{
			strncpy ((pCHAR)merchDetailRec->PreAuthValidationFlag ,"N", 1);
		}
	}
	else
	{
		strncpy ((pCHAR)merchDetailRec->PreAuthValidationFlag ,"N", 1);
	}

	/* The next set of commands will make sure the inputs are all numbers. */
	 returnNumbers((pCHAR)merchDetailRec->detailMerchantNumber);
	 returnNumbers((pCHAR)merchDetailRec->detailOrganizaNumber);

} /*end of parseMPAVDetail*/

/*************************************************************************************/
void parseMPAVHeader(pCHAR dataRecord, pmyMPAVHeader MPAVHeadRecord)
{
   memset(MPAVHeadRecord,0,sizeof(myMPAVHeader));

   strncpy(MPAVHeadRecord->headerRecordType     , dataRecord      ,sizeof(MPAVHeadRecord->headerRecordType     ) - 1);
   strncpy(MPAVHeadRecord->headerFileName       , dataRecord+3    ,sizeof(MPAVHeadRecord->headerFileName       ) - 1);
   strncpy(MPAVHeadRecord->headerFileCreateDate , dataRecord + 11 ,sizeof(MPAVHeadRecord->headerFileCreateDate ) - 1);
   strncpy(MPAVHeadRecord->headerFiller         , dataRecord+17   ,sizeof(MPAVHeadRecord->headerFiller         ) - 1);

   /*the next command will make sure the input has numbers only.*/
   returnNumbers(MPAVHeadRecord->headerFileCreateDate);

}  /*end of parseMPAVHeader*/


/*************************************************************************************/
void parseMPAVTrailer(pCHAR dataRecord, pmyMPAVTrailer MPAVTrailRecord)
{
   memset(MPAVTrailRecord,0,sizeof(myMPAVTrailer));

   strncpy((pCHAR)MPAVTrailRecord->trailRecordType     , dataRecord     , sizeof(MPAVTrailRecord->trailRecordType     ) - 1);
   strncpy((pCHAR)MPAVTrailRecord->trailFileName       , dataRecord + 3 , sizeof(MPAVTrailRecord->trailFileName       ) - 1);
   strncpy((pCHAR)MPAVTrailRecord->trailFileCreateDate , dataRecord + 11, sizeof(MPAVTrailRecord->trailFileCreateDate ) - 1);
   strncpy((pCHAR)MPAVTrailRecord->trailNumberofRecords, dataRecord + 17, sizeof(MPAVTrailRecord->trailNumberofRecords) - 1);
   strncpy((pCHAR)MPAVTrailRecord->trailFiller         , dataRecord + 26, sizeof(MPAVTrailRecord->trailFiller         ) - 1);

   /*the next command would make sure all of the input are numbers.*/
   returnNumbers((pCHAR)MPAVTrailRecord->trailFileCreateDate);
   returnNumbers((pCHAR)MPAVTrailRecord->trailNumberofRecords);

}  /*end of parseMPAVTrailer*/

/*************************************************************************************/
void parseEMVCDetail(pCHAR dataRecord, pDCF01 dcf01, pCHAR EMVCpabilityindicator)
{
	pRawEMVCDetail 		pRTD;
	CHAR	            temp_organization_number[5] = "0000";
	CHAR				temp_merchant_id[16] = {0};

	memset( dcf01, 0x00, sizeof(DCF01) );
	pRTD = (pRawEMVCDetail)dataRecord;
	if(true==isalphanumSpace(pRTD->detailTerminalNumber, 8))//N -> 8
	{
		memcpy((pCHAR)dcf01->primary_key.device_id ,pRTD->detailTerminalNumber ,  sizeof(dcf01->primary_key.device_id ) - 1);
	}
	if(true==isnumSpace(pRTD->detailMerchantNumber, 15))//N -> 15
	{
		memcpy((pCHAR)dcf01->merchant_id           ,pRTD->detailMerchantNumber ,  15) ;
	}
	memcpy((pCHAR)dcf01->organization_id       		,temp_organization_number ,  sizeof(dcf01->organization_id       ) - 1);

	//Store EMV capability indicator
	if(true==isalphanumSpace(pRTD->detailEmvCapability, 6))
	{
		memcpy(EMVCpabilityindicator, 			pRTD->detailEmvCapability, sizeof(pRTD->detailEmvCapability));
	}
	/* The next set of commands will make sure the inputs are all numbers. */
	returnNumbers((pCHAR)dcf01->primary_key.device_id       );
	returnNumbers((pCHAR)dcf01->merchant_id                 );
	returnNumbers((pCHAR)dcf01->organization_id             );

	return;
} /*end of parseEMVCDetail*/

/*************************************************************************************/
void parseEMVCHeader(pCHAR dataRecord, pmyEMVCHeader EMVCHeadRecord)
{
   memset(EMVCHeadRecord,0,sizeof(myEMVCHeader));

   strncpy(EMVCHeadRecord->headerRecordType     , dataRecord      ,sizeof(EMVCHeadRecord->headerRecordType     ) - 1);
   strncpy(EMVCHeadRecord->headerFileName       , dataRecord+3    ,sizeof(EMVCHeadRecord->headerFileName       ) - 1);
   strncpy(EMVCHeadRecord->headerFileCreateDate , dataRecord + 11 ,sizeof(EMVCHeadRecord->headerFileCreateDate ) - 1);
   strncpy(EMVCHeadRecord->headerFiller         , dataRecord+17   ,sizeof(EMVCHeadRecord->headerFiller         ) - 1);

   /*the next command will make sure the input has numbers only.*/
   returnNumbers(EMVCHeadRecord->headerFileCreateDate);

}  /*end of parseEMVCHeader*/


/*************************************************************************************/
void parseEMVCTrailer(pCHAR dataRecord, pmyEMVCTrailer EMVCTrailRecord)
{
   memset(EMVCTrailRecord,0,sizeof(myTerminalTrailer));

   strncpy((pCHAR)EMVCTrailRecord->trailRecordType     , dataRecord     , sizeof(EMVCTrailRecord->trailRecordType     ) - 1);
   strncpy((pCHAR)EMVCTrailRecord->trailFileName       , dataRecord + 3 , sizeof(EMVCTrailRecord->trailFileName       ) - 1);
   strncpy((pCHAR)EMVCTrailRecord->trailFileCreateDate , dataRecord + 11, sizeof(EMVCTrailRecord->trailFileCreateDate ) - 1);
   strncpy((pCHAR)EMVCTrailRecord->trailNumberofRecords, dataRecord + 17, sizeof(EMVCTrailRecord->trailNumberofRecords) - 1);
   strncpy((pCHAR)EMVCTrailRecord->trailFiller         , dataRecord + 26, sizeof(EMVCTrailRecord->trailFiller         ) - 1);

   /*the next command would make sure all of the input are numbers.*/
   returnNumbers((pCHAR)EMVCTrailRecord->trailFileCreateDate);
   returnNumbers((pCHAR)EMVCTrailRecord->trailNumberofRecords);

}  /*end of parseEMVCTrailer*/

/******************************************************************************
 *       NAME:           dcf01HasBlankReqField()
 *
 *       DESCRIPTION:    This module determines if the terminal has blank
 *                       requuired fields.
 *
 *       INPUTS:         termDetailRecord is the structure containing the terminal
 *                       information
 *
 *       OUTPUTS:        if it has blank fields:  CORRECT
 *                       if it does not have blank fields:  NOT_CORRECT
 *
 *       RETURNS:        None.
 *
 *      AUTHOR:          Faramarz Arad.
 *
 ******************************************************************************/
INT  dcf01HasBlankReqField (pDCF01    dcf01)
{
   CHAR tempStr  [50] = {0};
   /*Varriable added for �Date Added� field */
   CHAR tempdate[25] = ""; 
   CHAR yearmonthdate[9] ="";
   /*****************************************/
   memset(tempStr, ' ', sizeof(tempStr));
   
   /*check to see if the dcf01->primary_key.device_id is blank or null.*/
   if(strlen((pCHAR)dcf01->primary_key.device_id) != 0)
   {
      if (strncmp((pCHAR)dcf01->primary_key.device_id, tempStr, sizeof(dcf01->primary_key.device_id) - 1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);

   /*check to see if the dcf01->merchant_id field is blank or null.*/
   if(strlen((pCHAR)dcf01->merchant_id) != 0)
   {
      if (strncmp((pCHAR)dcf01->merchant_id, tempStr, sizeof(dcf01->merchant_id) - 1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);

   /*check to see if the dcf01->organization_id field is blank or null.*/
   if(strlen((pCHAR)dcf01->organization_id) != 0)
   {
      if (strncmp((pCHAR)dcf01->organization_id, tempStr, sizeof(dcf01->organization_id) - 1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);

   /*check to see if the dcf01->industry_code field is blank or null.*/
   if(strlen((pCHAR)dcf01->industry_code) != 0)
   {
      if (strncmp((pCHAR)dcf01->industry_code, tempStr, sizeof(dcf01->industry_code) - 1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);

   /*check to see if the dcf01->category_code field is blank or null.*/
   if(strlen((pCHAR)dcf01->category_code) != 0)
   {
      if (strncmp((pCHAR)dcf01->category_code, tempStr, sizeof(dcf01->category_code)-1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);

   if ( NULL == strchr("ARI", dcf01->status[0]))
      return(CORRECT);

   /*check to see if the dcf01->totals_ind field is N or Y*/
   if (NULL == strchr("NY", dcf01->totals_ind[0]))
      return(CORRECT);

   /*check to see if the dcf01->statistics_ind field is N or Y*/
   if (NULL == strchr("NY", dcf01->statistics_ind[0]))
      return(CORRECT);

   /*check to make sure the valid_transactions is not null.*/
   /*added by farad on 3/17/1999 fo fix bug #246*/
   if (strlen((pCHAR)dcf01->valid_transactions) == 0)
      return(CORRECT);
   
   /*added by farad on 3/30/1999 to fix bug #401.*/
   /*check to make sure the detailTPKValue is not null.*/
   if(strlen((pCHAR)dcf01->cwk) != 0)
   {
      if (strncmp((pCHAR)dcf01->cwk,tempStr, sizeof(dcf01->cwk)-1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);
   
   /* Check to make sure date_added field is blank or null.*/
   if(strlen((pCHAR)dcf01->date_added) == 0)
   {
   	  /* date_added feild is empty, get system date*/
	  ptetime_get_timestamp( tempdate ); 
	  get_date( tempdate, tempStr ); /* tempStr will have date  YYYY-MM-DD formate.*/

	  strncpy(yearmonthdate,tempStr,4); /* Copy YYYY in yearmonthdate.*/
	  strncpy(yearmonthdate+4,tempStr+5,2); /* Copy MM in yearmonthdate.*/
	  strncpy(yearmonthdate+6,tempStr+8,2); /* Copy DD in yearmonthdate.*/

	  /*yearmonthdate have date in YYYYMMDD formate*/
	  memcpy((pCHAR)dcf01->date_added ,yearmonthdate ,  sizeof(dcf01->date_added ) - 1);
	  return (NOT_CORRECT);
   }
   else
	{
	  return(CORRECT);
	}
	
   /*could not find a single required field in the terminal, so return NOT_CORRECT.*/
   return (NOT_CORRECT);

}  /*end of dcf01HasBlankReqField*/



/******************************************************************************
 *       NAME:           mppara01HasBlankReqField()
 *
 *       DESCRIPTION:    This module determines if the mpparameter has blank
 *                       requuired fields.
 *
 *       INPUTS:         mpparaDetailRecord is the structure containing the mpparameter
 *                       information
 *
 *       OUTPUTS:        if it has blank fields:  CORRECT
 *                       if it does not have blank fields:  NOT_CORRECT
 *
 *       RETURNS:        None.
 *
 *      AUTHOR:          Faramarz Arad.
 *
 ******************************************************************************/
INT  mppara01HasBlankReqField (pMPPARA01    mppara01)
{
   CHAR tempStr  [50] = {0};
   CHAR tempdate[25] = {0};
   CHAR yearmonthdate[9] = {0};

   memset(tempStr, ' ', sizeof(tempStr));
   
   /*check to see if the mppara01->primary_key.ProductCode is blank or null.*/
   if(strlen((pCHAR)mppara01->primary_key.product_code)>0)
   {
      if (strncmp((pCHAR)mppara01->primary_key.product_code, tempStr, sizeof(mppara01->primary_key.product_code) - 1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);

   /*check to see if the mppara01->MinMajorPurchaseAmount is blank or null.*/
   if(strlen((pCHAR)mppara01->minmajpurchamt) != 0)
   {
      if (strncmp((pCHAR)mppara01->minmajpurchamt, tempStr, sizeof(mppara01->minmajpurchamt) - 1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);
   /*check to see if the mppara01->MaxMajorPurchaseAmount is blank or null.*/
   if(strlen((pCHAR)mppara01->maxmajpurchamt) != 0)
   {
      if (strncmp((pCHAR)mppara01->maxmajpurchamt, tempStr, sizeof(mppara01->maxmajpurchamt) - 1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);
   /*check to see if the mppara01->MinInstallmentAmount is blank or null.*/
   if(strlen((pCHAR)mppara01->mininstamt) != 0)
   {
      if (strncmp((pCHAR)mppara01->mininstamt, tempStr, sizeof(mppara01->mininstamt) - 1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);
   /*check to see if the mppara01->MaxInstallmentAmount is blank or null.*/
   if(strlen((pCHAR)mppara01->maxinstamt) != 0)
   {
      if (strncmp((pCHAR)mppara01->maxinstamt, tempStr, sizeof(mppara01->maxinstamt) - 1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);
   /*check to see if the mppara01->MinMajorPurchaseLimit is blank or null.*/
   if(strlen((pCHAR)mppara01->minmajpurchlmt) != 0)
   {
      if (strncmp((pCHAR)mppara01->minmajpurchlmt, tempStr, sizeof(mppara01->minmajpurchlmt) - 1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);
   /*check to see if the mppara01->EffectiveDate is blank or null.*/
   if(strlen((pCHAR)mppara01->effective_date) == 8)
   {
      if (strncmp((pCHAR)mppara01->effective_date, tempStr, sizeof(mppara01->effective_date) - 1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);
   /*check to see if the mppara01->MaxNoOfTransPerDay is blank or null.*/
   if(strlen((pCHAR)mppara01->maxtrnperday) != 0)
   {
      if (strncmp((pCHAR)mppara01->maxtrnperday, tempStr, sizeof(mppara01->maxtrnperday) - 1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);
   /*check to see if the mppara01->MaxAmountPerDay is blank or null.*/
   if(strlen((pCHAR)mppara01->maxamtperday) != 0)
   {
      if (strncmp((pCHAR)mppara01->maxamtperday, tempStr, sizeof(mppara01->maxamtperday) - 1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);
   /*check to see if the mppara01->AuthPercentOverlimit is blank or null.*/
   if(strlen((pCHAR)mppara01->authoverlimit) != 0)
   {
      if (strncmp((pCHAR)mppara01->authoverlimit, tempStr, sizeof(mppara01->authoverlimit) - 1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);
  
   return (NOT_CORRECT);

}  /*end of mppara01HasBlankReqField*/



  /******************************************************************************
 *       NAME:           mgf01HasBlankReqField()
 *
 *       DESCRIPTION:    This module determines if the mgfmeter has blank
 *                       requuired fields.
 *
 *       INPUTS:         mgfDetailRecord is the structure containing the mgfmeter
 *                       information
 *
 *       OUTPUTS:        if it has blank fields:  CORRECT
 *                       if it does not have blank fields:  NOT_CORRECT
 *
 *       RETURNS:        None.
 *
 *      AUTHOR:          Faramarz Arad.
 *
 ******************************************************************************/
INT  mgf01HasBlankReqField (pMGF01    mgf01)
{
   CHAR tempStr  [50] = {0};
   /*Varriable added for ?Date Added? field */
   CHAR tempdate[25] = ""; 
   CHAR yearmonthdate[9] ="";
   /*****************************************/
   memset(tempStr, ' ', sizeof(tempStr));
   
   /*check to see if the mgf01->primary_key.merchant_group_id  is blank or null.*/
   if(strlen((pCHAR)mgf01->primary_key.merchant_group_id )> 0)
   {
      if (strncmp((pCHAR)mgf01->primary_key.merchant_group_id , tempStr, sizeof(mgf01->primary_key.merchant_group_id ) - 1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);

   /*check to see if the mgf01->EffectiveDate is blank or null.*/
   if(strlen((pCHAR)mgf01->effective_date) == 8)
   {
      if (strncmp((pCHAR)mgf01->effective_date, tempStr, sizeof(mgf01->effective_date) - 1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);
   /*check to see if the mgf01->nbr_of_months_1	 is blank or null.*/
   if(strlen((pCHAR)mgf01->merchgrp[0].merchgrepint[0].nbr_of_months) != 0)
   {
      if (strncmp((pCHAR)mgf01->merchgrp[0].merchgrepint[0].nbr_of_months	, tempStr, sizeof(mgf01->merchgrp[0].merchgrepint[0].nbr_of_months) - 1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);
      /*check to see if the mgf01->nbr_of_months_2	 is blank or null.*/
   if(strlen((pCHAR)mgf01->merchgrp[0].merchgrepint[1].nbr_of_months) != 0)
   {
      if (strncmp((pCHAR)mgf01->merchgrp[0].merchgrepint[1].nbr_of_months	, tempStr, sizeof(mgf01->merchgrp[0].merchgrepint[1].nbr_of_months) - 1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);
  /*check to see if the mgf01->nbr_of_months_3	 is blank or null.*/
   if(strlen((pCHAR)mgf01->merchgrp[0].merchgrepint[2].nbr_of_months) != 0)
   {
      if (strncmp((pCHAR)mgf01->merchgrp[0].merchgrepint[2].nbr_of_months	, tempStr, sizeof(mgf01->merchgrp[0].merchgrepint[2].nbr_of_months) - 1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);
   /*check to see if the mgf01->nbr_of_months_4	 is blank or null.*/
   if(strlen((pCHAR)mgf01->merchgrp[0].merchgrepint[3].nbr_of_months) != 0)
   {
      if (strncmp((pCHAR)mgf01->merchgrp[0].merchgrepint[3].nbr_of_months	, tempStr, sizeof(mgf01->merchgrp[0].merchgrepint[3].nbr_of_months) - 1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);
      /*check to see if the mgf01->nbr_of_months_5	 is blank or null.*/
   if(strlen((pCHAR)mgf01->merchgrp[0].merchgrepint[4].nbr_of_months) != 0)
   {
      if (strncmp((pCHAR)mgf01->merchgrp[0].merchgrepint[4].nbr_of_months	, tempStr, sizeof(mgf01->merchgrp[0].merchgrepint[4].nbr_of_months) - 1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);
      /*check to see if the mgf01->nbr_of_months_6	 is blank or null.*/
   if(strlen((pCHAR)mgf01->merchgrp[0].merchgrepint[5].nbr_of_months) != 0)
   {
      if (strncmp((pCHAR)mgf01->merchgrp[0].merchgrepint[5].nbr_of_months	, tempStr, sizeof(mgf01->merchgrp[0].merchgrepint[5].nbr_of_months) - 1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);
      /*check to see if the mgf01->nbr_of_months_7	 is blank or null.*/
   if(strlen((pCHAR)mgf01->merchgrp[0].merchgrepint[6].nbr_of_months) != 0)
   {
      if (strncmp((pCHAR)mgf01->merchgrp[0].merchgrepint[6].nbr_of_months	, tempStr, sizeof(mgf01->merchgrp[0].merchgrepint[6].nbr_of_months) - 1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);
      /*check to see if the mgf01->nbr_of_months_8	 is blank or null.*/
   if(strlen((pCHAR)mgf01->merchgrp[0].merchgrepint[7].nbr_of_months) != 0)
   {
      if (strncmp((pCHAR)mgf01->merchgrp[0].merchgrepint[7].nbr_of_months	, tempStr, sizeof(mgf01->merchgrp[0].merchgrepint[7].nbr_of_months) - 1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);
      /*check to see if the mgf01->nbr_of_months_9	 is blank or null.*/
   if(strlen((pCHAR)mgf01->merchgrp[0].merchgrepint[8].nbr_of_months) != 0)
   {
      if (strncmp((pCHAR)mgf01->merchgrp[0].merchgrepint[8].nbr_of_months	, tempStr, sizeof(mgf01->merchgrp[0].merchgrepint[8].nbr_of_months) - 1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);
      /*check to see if the mgf01->nbr_of_months_10	 is blank or null.*/
   if(strlen((pCHAR)mgf01->merchgrp[0].merchgrepint[9].nbr_of_months) != 0)
   {
      if (strncmp((pCHAR)mgf01->merchgrp[0].merchgrepint[9].nbr_of_months	, tempStr, sizeof(mgf01->merchgrp[0].merchgrepint[9].nbr_of_months) - 1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);
      /*check to see if the mgf01->nbr_of_months_11	 is blank or null.*/
   if(strlen((pCHAR)mgf01->merchgrp[1].merchgrepint[0].nbr_of_months) != 0)
   {
      if (strncmp((pCHAR)mgf01->merchgrp[1].merchgrepint[0].nbr_of_months	, tempStr, sizeof(mgf01->merchgrp[1].merchgrepint[0].nbr_of_months) - 1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);
      /*check to see if the mgf01->nbr_of_months_12	 is blank or null.*/
   if(strlen((pCHAR)mgf01->merchgrp[1].merchgrepint[1].nbr_of_months) != 0)
   {
      if (strncmp((pCHAR)mgf01->merchgrp[1].merchgrepint[1].nbr_of_months	, tempStr, sizeof(mgf01->merchgrp[1].merchgrepint[1].nbr_of_months) - 1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);
      /*check to see if the mgf01->nbr_of_months_13	 is blank or null.*/
   if(strlen((pCHAR)mgf01->merchgrp[1].merchgrepint[2].nbr_of_months) != 0)
   {
      if (strncmp((pCHAR)mgf01->merchgrp[1].merchgrepint[2].nbr_of_months	, tempStr, sizeof(mgf01->merchgrp[1].merchgrepint[2].nbr_of_months) - 1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);
      /*check to see if the mgf01->nbr_of_months_14	 is blank or null.*/
   if(strlen((pCHAR)mgf01->merchgrp[1].merchgrepint[3].nbr_of_months) != 0)
   {
      if (strncmp((pCHAR)mgf01->merchgrp[1].merchgrepint[3].nbr_of_months	, tempStr, sizeof(mgf01->merchgrp[1].merchgrepint[3].nbr_of_months) - 1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);
      /*check to see if the mgf01->nbr_of_months_15	 is blank or null.*/
   if(strlen((pCHAR)mgf01->merchgrp[1].merchgrepint[4].nbr_of_months) != 0)
   {
      if (strncmp((pCHAR)mgf01->merchgrp[1].merchgrepint[4].nbr_of_months	, tempStr, sizeof(mgf01->merchgrp[1].merchgrepint[4].nbr_of_months) - 1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);
      /*check to see if the mgf01->nbr_of_months_16	 is blank or null.*/
   if(strlen((pCHAR)mgf01->merchgrp[1].merchgrepint[5].nbr_of_months) != 0)
   {
      if (strncmp((pCHAR)mgf01->merchgrp[1].merchgrepint[5].nbr_of_months	, tempStr, sizeof(mgf01->merchgrp[1].merchgrepint[5].nbr_of_months) - 1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);
      /*check to see if the mgf01->nbr_of_months_17	 is blank or null.*/
   if(strlen((pCHAR)mgf01->merchgrp[1].merchgrepint[6].nbr_of_months) != 0)
   {
      if (strncmp((pCHAR)mgf01->merchgrp[1].merchgrepint[6].nbr_of_months	, tempStr, sizeof(mgf01->merchgrp[1].merchgrepint[6].nbr_of_months) - 1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);
      /*check to see if the mgf01->nbr_of_months_18	 is blank or null.*/
   if(strlen((pCHAR)mgf01->merchgrp[1].merchgrepint[7].nbr_of_months) != 0)
   {
      if (strncmp((pCHAR)mgf01->merchgrp[1].merchgrepint[7].nbr_of_months	, tempStr, sizeof(mgf01->merchgrp[1].merchgrepint[7].nbr_of_months) - 1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);
      /*check to see if the mgf01->nbr_of_months_19	 is blank or null.*/
   if(strlen((pCHAR)mgf01->merchgrp[1].merchgrepint[8].nbr_of_months) != 0)
   {
      if (strncmp((pCHAR)mgf01->merchgrp[1].merchgrepint[8].nbr_of_months	, tempStr, sizeof(mgf01->merchgrp[1].merchgrepint[8].nbr_of_months) - 1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);
      /*check to see if the mgf01->nbr_of_months_20	 is blank or null.*/
   if(strlen((pCHAR)mgf01->merchgrp[1].merchgrepint[9].nbr_of_months) != 0)
   {
      if (strncmp((pCHAR)mgf01->merchgrp[1].merchgrepint[9].nbr_of_months	, tempStr, sizeof(mgf01->merchgrp[1].merchgrepint[9].nbr_of_months) - 1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);
      if(strlen((pCHAR)mgf01->merchgrp[2].merchgrepint[0].nbr_of_months) != 0)
   {
      if (strncmp((pCHAR)mgf01->merchgrp[2].merchgrepint[0].nbr_of_months	, tempStr, sizeof(mgf01->merchgrp[2].merchgrepint[0].nbr_of_months) - 1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);
      /*check to see if the mgf01->nbr_of_months_22	 is blank or null.*/
   if(strlen((pCHAR)mgf01->merchgrp[2].merchgrepint[1].nbr_of_months) != 0)
   {
      if (strncmp((pCHAR)mgf01->merchgrp[2].merchgrepint[1].nbr_of_months	, tempStr, sizeof(mgf01->merchgrp[2].merchgrepint[1].nbr_of_months) - 1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);
      /*check to see if the mgf01->nbr_of_months_23	 is blank or null.*/
   if(strlen((pCHAR)mgf01->merchgrp[2].merchgrepint[2].nbr_of_months) != 0)
   {
      if (strncmp((pCHAR)mgf01->merchgrp[2].merchgrepint[2].nbr_of_months	, tempStr, sizeof(mgf01->merchgrp[2].merchgrepint[2].nbr_of_months) - 1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);
      /*check to see if the mgf01->nbr_of_months_24	 is blank or null.*/
   if(strlen((pCHAR)mgf01->merchgrp[2].merchgrepint[3].nbr_of_months) != 0)
   {
      if (strncmp((pCHAR)mgf01->merchgrp[2].merchgrepint[3].nbr_of_months	, tempStr, sizeof(mgf01->merchgrp[2].merchgrepint[3].nbr_of_months) - 1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);
      /*check to see if the mgf01->nbr_of_months_25	 is blank or null.*/
   if(strlen((pCHAR)mgf01->merchgrp[2].merchgrepint[4].nbr_of_months) != 0)
   {
      if (strncmp((pCHAR)mgf01->merchgrp[2].merchgrepint[4].nbr_of_months	, tempStr, sizeof(mgf01->merchgrp[2].merchgrepint[4].nbr_of_months) - 1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);
      /*check to see if the mgf01->nbr_of_months_26	 is blank or null.*/
   if(strlen((pCHAR)mgf01->merchgrp[2].merchgrepint[5].nbr_of_months) != 0)
   {
      if (strncmp((pCHAR)mgf01->merchgrp[2].merchgrepint[5].nbr_of_months	, tempStr, sizeof(mgf01->merchgrp[2].merchgrepint[5].nbr_of_months) - 1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);
      /*check to see if the mgf01->nbr_of_months_27	 is blank or null.*/
   if(strlen((pCHAR)mgf01->merchgrp[2].merchgrepint[6].nbr_of_months) != 0)
   {
      if (strncmp((pCHAR)mgf01->merchgrp[2].merchgrepint[6].nbr_of_months	, tempStr, sizeof(mgf01->merchgrp[2].merchgrepint[6].nbr_of_months) - 1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);
      /*check to see if the mgf01->nbr_of_months_28	 is blank or null.*/
   if(strlen((pCHAR)mgf01->merchgrp[2].merchgrepint[7].nbr_of_months) != 0)
   {
      if (strncmp((pCHAR)mgf01->merchgrp[2].merchgrepint[7].nbr_of_months	, tempStr, sizeof(mgf01->merchgrp[2].merchgrepint[7].nbr_of_months) - 1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);
   /*check to see if the mgf01->nbr_of_months_29	 is blank or null.*/
   if(strlen((pCHAR)mgf01->merchgrp[2].merchgrepint[8].nbr_of_months) != 0)
   {
      if (strncmp((pCHAR)mgf01->merchgrp[2].merchgrepint[8].nbr_of_months	, tempStr, sizeof(mgf01->merchgrp[2].merchgrepint[8].nbr_of_months) - 1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);
      /*check to see if the mgf01->nbr_of_months_30	 is blank or null.*/
   if(strlen((pCHAR)mgf01->merchgrp[2].merchgrepint[9].nbr_of_months) != 0)
   {
      if (strncmp((pCHAR)mgf01->merchgrp[2].merchgrepint[9].nbr_of_months	, tempStr, sizeof(mgf01->merchgrp[2].merchgrepint[9].nbr_of_months) - 1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);
      /*check to see if the mgf01->nbr_of_months_31	 is blank or null.*/
   if(strlen((pCHAR)mgf01->merchgrp[3].merchgrepint[0].nbr_of_months) != 0)
   {
      if (strncmp((pCHAR)mgf01->merchgrp[3].merchgrepint[0].nbr_of_months	, tempStr, sizeof(mgf01->merchgrp[3].merchgrepint[0].nbr_of_months) - 1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);
      /*check to see if the mgf01->nbr_of_months_32	 is blank or null.*/
   if(strlen((pCHAR)mgf01->merchgrp[3].merchgrepint[1].nbr_of_months) != 0)
   {
      if (strncmp((pCHAR)mgf01->merchgrp[3].merchgrepint[1].nbr_of_months	, tempStr, sizeof(mgf01->merchgrp[3].merchgrepint[1].nbr_of_months) - 1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);
      /*check to see if the mgf01->nbr_of_months_3	 is blank or null.*/
   if(strlen((pCHAR)mgf01->merchgrp[3].merchgrepint[2].nbr_of_months) != 0)
   {
      if (strncmp((pCHAR)mgf01->merchgrp[3].merchgrepint[2].nbr_of_months	, tempStr, sizeof(mgf01->merchgrp[3].merchgrepint[2].nbr_of_months) - 1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);
      /*check to see if the mgf01->nbr_of_months_3	 is blank or null.*/
   if(strlen((pCHAR)mgf01->merchgrp[3].merchgrepint[3].nbr_of_months) != 0)
   {
      if (strncmp((pCHAR)mgf01->merchgrp[3].merchgrepint[3].nbr_of_months	, tempStr, sizeof(mgf01->merchgrp[3].merchgrepint[3].nbr_of_months) - 1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);
      /*check to see if the mgf01->nbr_of_months_35	 is blank or null.*/
   if(strlen((pCHAR)mgf01->merchgrp[3].merchgrepint[4].nbr_of_months) != 0)
   {
      if (strncmp((pCHAR)mgf01->merchgrp[3].merchgrepint[4].nbr_of_months	, tempStr, sizeof(mgf01->merchgrp[3].merchgrepint[4].nbr_of_months) - 1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);
      /*check to see if the mgf01->merchgrp[3].merchgrepint[4].nbr_of_months_36	 is blank or null.*/
   if(strlen((pCHAR)mgf01->merchgrp[3].merchgrepint[5].nbr_of_months) != 0)
   {
      if (strncmp((pCHAR)mgf01->merchgrp[3].merchgrepint[5].nbr_of_months    , tempStr, sizeof(mgf01->merchgrp[3].merchgrepint[5].nbr_of_months) - 1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);
      /*check to see if the mgf01->nbr_of_months_37	 is blank or null.*/
   if(strlen((pCHAR)mgf01->merchgrp[3].merchgrepint[6].nbr_of_months) != 0)
   {
      if (strncmp((pCHAR)mgf01->merchgrp[3].merchgrepint[6].nbr_of_months	, tempStr, sizeof(mgf01->merchgrp[3].merchgrepint[6].nbr_of_months) - 1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);
      /*check to see if the mgf01->nbr_of_months_38	 is blank or null.*/
   if(strlen((pCHAR)mgf01->merchgrp[3].merchgrepint[7].nbr_of_months) != 0)
   {
      if (strncmp((pCHAR)mgf01->merchgrp[3].merchgrepint[7].nbr_of_months	, tempStr, sizeof(mgf01->merchgrp[3].merchgrepint[7].nbr_of_months) - 1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);
      /*check to see if the mgf01->nbr_of_months_39	 is blank or null.*/
   if(strlen((pCHAR)mgf01->merchgrp[3].merchgrepint[8].nbr_of_months) != 0)
   {
      if (strncmp((pCHAR)mgf01->merchgrp[3].merchgrepint[8].nbr_of_months	, tempStr, sizeof(mgf01->merchgrp[3].merchgrepint[8].nbr_of_months) - 1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);
      /*check to see if the mgf01->nbr_of_months_40	 is blank or null.*/
   if(strlen((pCHAR)mgf01->merchgrp[3].merchgrepint[9].nbr_of_months) != 0)
   {
      if (strncmp((pCHAR)mgf01->merchgrp[3].merchgrepint[9].nbr_of_months	, tempStr, sizeof(mgf01->merchgrp[3].merchgrepint[9].nbr_of_months) - 1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);
      
  return (NOT_CORRECT);
}  

/*end of mgf01HasBlankReqField*/

/*****************************************************/
/***************************************************************/
INT  mcf01HasBlankReqField        (pMCF01    mcf01,pcardType subStruct)
{
   CHAR tempStr  [50];
   
   memset(tempStr,' ',sizeof(tempStr));

   /*check to see if the mcf01->primary_key.merchant_id is blank or null.*/
   if(strlen((pCHAR)mcf01->primary_key.merchant_id))
   {
      if (strncmp((pCHAR)mcf01->primary_key.merchant_id, tempStr, sizeof(mcf01->primary_key.merchant_id)-1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);

   /*check to see if the mcf01->primary_key.organization field is blank or null.*/
   if(strlen((pCHAR)mcf01->primary_key.organization_id))
   {
      if (strncmp((pCHAR)mcf01->primary_key.organization_id, tempStr, sizeof(mcf01->primary_key.organization_id)-1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);
      
   /*check to see if the mcf01->status field is A,R,I*/
   if ( NULL == strchr("ARI", mcf01->status[0]))
      return(CORRECT);

   /*check to see if the mcf01->currency_code field is blank or null.*/
   if(strlen((pCHAR)mcf01->currency_code))
   {
      if (strncmp((pCHAR)mcf01->currency_code, tempStr, sizeof(mcf01->currency_code)-1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);

   /*check to see if the mcf01->rps_merchant field is N or Y*/
   if (NULL == strchr("NY", mcf01->rps_merchant[0]))
      return(CORRECT);

   /*check to see if the mcf01->house_card_merchant field is N or Y*/
   if (NULL == strchr("NY", mcf01->house_card_merchant[0])) 
      return(CORRECT);

   /*check to see if the mcf01->procurement_merchant field is N or Y*/
   if (NULL == strchr("NY", mcf01->procurement_merchant[0])) 
      return(CORRECT);

   /*check to see if the mcf01->deferred_merchant field is N or Y*/
   if (NULL == strchr("NY", mcf01->deferred_merchant[0]))
      return(CORRECT);

   /*check to see if the mcf01->cash_bonus_eligible field is N or Y*/
   if (NULL == strchr("NY", mcf01->cash_bonus_eligible[0])) 
      return(CORRECT);

   /*check to see if the mcf01->totals_ind field is N or Y*/
   if (NULL == strchr("NY", mcf01->totals_ind[0])) 
      return(CORRECT);

   /*check to see if the mcf01->statistics_ind field is N or Y*/
   if (NULL == strchr("NY", mcf01->statistics_ind[0])) 
      return(CORRECT);

   /*check to see if the mcf01->pos_code field is blank or null.*/
   if(strlen((pCHAR)mcf01->pos_code) != 0)
   {
      if (strncmp((pCHAR)mcf01->pos_code, tempStr, sizeof(mcf01->pos_code)-1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);


   /* The next check was marked out by farad on 3/24/1999 to fix bug #338.
      check to see if the mcf01->vs_tcc_code field is blank or null.

   if(strlen(mcf01->vs_tcc_code) != 0)
   {
      if (strncmp(mcf01->vs_tcc_code,tempStr, sizeof(mcf01->vs_tcc_code)-1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);

   */

   /*check to see if the mcf01->mc_fcc_codefield is blank or null.*/
   if(strlen((pCHAR)mcf01->mc_fcc_code))
   {
      if (strncmp((pCHAR)mcf01->mc_fcc_code, tempStr, sizeof(mcf01->mc_fcc_code)-1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);

   /* The next check is added by farad on 3/24/1999 to fix bug #338.
      check to see if the mcf01->vs_tcc_code field is blank or null.

   if(strlen(mcf01->class_code) != 0)
   {
      if (strcmp(mcf01->class_code, tempStr, sizeof(mcf01->class_code)-1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);

   */
   
   /*check to see if the mct01.cardTypeAccepted1 is blank or NULL.*/
   if(strlen((pCHAR)subStruct->cardTypeAcc))
   {
      if (strncmp((pCHAR)subStruct->cardTypeAcc, tempStr, sizeof(subStruct->cardTypeAcc)-1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);

   /*could not find a single required field in the terminal, so return NOT_CORRECT.*/
   return (NOT_CORRECT);

}

/*************************************************************************************
       NAME:           negfraudHasBlankReqField()

       DESCRIPTION:    This module determines if the any required fields are missing
       fields.		   If required fields are missing, the record will be written to 
					   the dump file and an error message logged.  Required fields:
					   mcc, bin_low, bin_high, pan_length
					        
                       
       INPUTS:         The input is an instance of negative fraud table.
       information
                       
                       
	    OUTPUTS:       if it has blank fields:            CORRECT (1)
                       if it does not have blank fields:  NOT_CORRECT (0)
       RETURNS:        
		   None.                       
                               
      AUTHOR:          Faramarz Arad.
      MODIFIED BY:     None.
*************************************************************************************/
INT  negfraudHasBlankReqField(pNEGATIVE_FRAUD neg_fraud)
{
   CHAR tempStr  [50] = {0};
   
   memset(tempStr, ' ', sizeof(tempStr));

   /* MCC blank or null? */
   if(strlen(neg_fraud->primary_key.mcc) != 0)
   {
      if (strncmp(neg_fraud->primary_key.mcc,tempStr, sizeof(neg_fraud->primary_key.mcc)-1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);

   /* Bin Low blank or null? */
   if(strlen(neg_fraud->primary_key.bin_low) != 0)
   {
      if (strncmp(neg_fraud->primary_key.bin_low,tempStr, sizeof(neg_fraud->primary_key.bin_low)-1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);

   /* Bin High blank or null? */
   if(strlen(neg_fraud->primary_key.bin_high) != 0)
   {
      if (strncmp(neg_fraud->primary_key.bin_high,tempStr, sizeof(neg_fraud->primary_key.bin_high)-1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);

      /* Pan Length blank or null? */
   if(neg_fraud->primary_key.pan_length == 0x00 || neg_fraud->primary_key.pan_length == 0x20)
      return(CORRECT);


   return(NOT_CORRECT);

} /*end of  negfraudHasBlankReqField    */


/*****************************************************/
/*****************************************************/
INT  acf01HasBlankReqField        (pACF01    acf01)
{
   CHAR tempStr  [50];
   
   strcpy(tempStr, (pCHAR)acf01->primary_key.account_type);
   /*check to see if the acf01.primary_key.account_type is ok.*/
   if (!( strcmp(tempStr,"05") == 0 || 
          strcmp(tempStr,"06") == 0 ||
          strcmp(tempStr,"07") == 0 ||
          strcmp(tempStr,"08") == 0  ))
      return(CORRECT);

   /*check to see if the acf01.primary_key.account_nbr is all spaces or null.*/
   memset(tempStr, ' ',sizeof(tempStr));
   if(strlen((pCHAR)acf01->primary_key.account_nbr) != 0)
   {
      if (strncmp((pCHAR)acf01->primary_key.account_nbr, tempStr, sizeof(acf01->primary_key.account_nbr)-1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);

   /*modified by farad on 3/16/1999 to fix bug #193.*/
   /*check to see if the acf01.status is V='00' or I='05' */
   if (!( strcmp((pCHAR)acf01->status,"00") == 0 || 
          strcmp((pCHAR)acf01->status,"05") == 0 ))
      return(CORRECT);

   /*check to see if the outstanding balance is null or blank.*/
   if(strlen((pCHAR)acf01->outstanding) != 0)
   {
      if (strncmp((pCHAR)acf01->outstanding,tempStr, sizeof(acf01->outstanding)-1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);


   /*check to see if the cash_outstanding balance is null or blank.*/
   if(strlen((pCHAR)acf01->cash_outstanding) != 0)
   {
      if (strncmp((pCHAR)acf01->cash_outstanding, tempStr, sizeof(acf01->cash_outstanding)-1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);

   return (NOT_CORRECT);
} /*end of acf01HasBlankReqField */

/**************************************************/
/*********************************************************/
INT  ccf02pHasBlankReqField       (pCCF02P   ccf02p)
{
   CHAR tempStr  [50] = {0};
   
   /*check to see if the card type is "C"*/
   if ( ( ccf02p->primary_key.card_type[0] != 'C') )
      return(CORRECT);

   memset(tempStr,' ',sizeof(tempStr));

   /*check to see if the card number is all spaces or null.*/
   if(strlen((pCHAR)ccf02p->primary_key.card_nbr) != 0)
   {
      if (strncmp((pCHAR)ccf02p->primary_key.card_nbr, tempStr, sizeof(ccf02p->primary_key.card_nbr)-1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);

   /*check to see if the primary account number is null or blank.*/
   if(strlen((pCHAR)ccf02p->primary_acct_nbr) != 0)
   {
      if (strncmp((pCHAR)ccf02p->primary_acct_nbr, tempStr, sizeof(ccf02p->primary_acct_nbr)-1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);

   /*check to see if the status is null or blank.*/
   if(strlen((pCHAR)ccf02p->status) != 0)
   {
      if (strncmp((pCHAR)ccf02p->status, tempStr, sizeof(ccf02p->status)-1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);

   /*check to see if the status is null or blank.*/
   if(strlen((pCHAR)ccf02p->expire_date) != 0)
   {
      if (strncmp((pCHAR)ccf02p->expire_date, tempStr, sizeof(ccf02p->expire_date)-1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);

   /*added by farad on 3/18/1999 to fix bug #247.*/
   if (0 == strlen((pCHAR)ccf02p->allowed) )
      return(CORRECT);

   return(NOT_CORRECT);
}  /*end of ccf02pHasBlankReqField*/

/*****************************************************/
/***********************************************************/
INT  ccf01HasBlankReqField        (pCCF01    ccf01)
{
   CHAR tempStr  [50] = {0};
   
   /*check to see if the card type is "C"*/
   if (ccf01->primary_key.card_type[0] != 'C')
      return(CORRECT);

   memset(tempStr,' ',sizeof(tempStr));

   /*check to see if the card number is all spaces or null.*/
   if(strlen((pCHAR)ccf01->primary_key.card_nbr) != 0)
   {
      if (strncmp((pCHAR)ccf01->primary_key.card_nbr, tempStr, sizeof(ccf01->primary_key.card_nbr)-1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);

   /*check to see if the primary account number is null or blank.*/
   if(strlen((pCHAR)ccf01->primary_acct_nbr) != 0)
   {
      if (strncmp((pCHAR)ccf01->primary_acct_nbr, tempStr, sizeof(ccf01->primary_acct_nbr)-1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);

   /*check to see if the status is null or blank.*/
   if(strlen((pCHAR)ccf01->status) != 0)
   {
      if (strncmp((pCHAR)ccf01->status, tempStr, sizeof(ccf01->status)-1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);

   return(NOT_CORRECT);

}/*  ccf01HasBlankReqField  */




/*************************************************************************************
       NAME:           paf01HasBlankReqField()

       DESCRIPTION:    This module determines if the primary has blank requuired
       fields.
					        
                       
       INPUTS:         The input is an instance of paf01 table.
       information
                       
                       
	    OUTPUTS:        if it has blank fields:  CORRECT
                       if it does not have blank fields:  NOT_CORRECT

       RETURNS:        
		   None.                       
                               
      AUTHOR:          Faramarz Arad.
      MODIFIED BY:     None.
*************************************************************************************/
INT  paf01HasBlankReqField      (pPAF01    paf01)
{
   CHAR tempStr  [50] = {0};
   
   memset(tempStr, ' ', sizeof(tempStr));

   /*check to see if the primary account is blank or null.*/
   if(strlen((pCHAR)paf01->primary_key.primary_acct_nbr) != 0)
   {
      if (strncmp((pCHAR)paf01->primary_key.primary_acct_nbr,tempStr, sizeof(paf01->primary_key.primary_acct_nbr)-1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);

   /*check to see if the status field is blank or NULL.*/
   if(strlen((pCHAR)paf01->status) != 0)
   {
      if (strncmp((pCHAR)paf01->status,tempStr, sizeof(paf01->status)-1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);

   return(NOT_CORRECT);

} /*end of  paf01HasBlankReqField    */

INT  dcf01HasBlankReqField_EDTR (pDCF01    dcf01)
{
   CHAR tempStr  [50] = {0};
   /*Varriable added for �Date Added� field */
   CHAR tempdate[25] = ""; 
   CHAR yearmonthdate[9] ="";
   /*****************************************/
   memset(tempStr, ' ', sizeof(tempStr));
   
   /*check to see if the dcf01->primary_key.device_id is blank or null.*/
   if(strlen((pCHAR)dcf01->primary_key.device_id) != 0)
   {
      if (strncmp((pCHAR)dcf01->primary_key.device_id, tempStr, sizeof(dcf01->primary_key.device_id) - 1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);

   /*check to see if the dcf01->merchant_id field is blank or null.*/
   if(strlen((pCHAR)dcf01->merchant_id) != 0)
   {
      if (strncmp((pCHAR)dcf01->merchant_id, tempStr, sizeof(dcf01->merchant_id) - 1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);

   /*check to see if the dcf01->organization_id field is blank or null.*/
   if(strlen((pCHAR)dcf01->organization_id) != 0)
   {
      if (strncmp((pCHAR)dcf01->organization_id, tempStr, sizeof(dcf01->organization_id) - 1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);

   /*check to see if the dcf01->category_code field is blank or null.*/
   if(strlen((pCHAR)dcf01->category_code) != 0)
   {
      if (strncmp((pCHAR)dcf01->category_code, tempStr, sizeof(dcf01->category_code)-1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);
  
  /*could not find a single required field in the terminal, so return NOT_CORRECT.*/
   return (NOT_CORRECT);

}  /*end of dcf01HasBlankReqField_EDTR	*/

INT  mcf01HasBlankReqField_MPAV (pmyMPAVDetail    merchDetailRec)
{
   CHAR tempStr  [50] = {0};

   /*****************************************/
   memset(tempStr, ' ', sizeof(tempStr));

   /*check to see if the dcf01->merchant_id field is blank or null.*/
   if(strlen((pCHAR)merchDetailRec->detailMerchantNumber) != 0)
   {
      if (strncmp((pCHAR)merchDetailRec->detailMerchantNumber, tempStr, sizeof(merchDetailRec->detailMerchantNumber) - 1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);

   /*check to see if the dcf01->organization_id field is blank or null.*/
   if(strlen((pCHAR)merchDetailRec->detailOrganizaNumber) != 0)
   {
      if (strncmp((pCHAR)merchDetailRec->detailOrganizaNumber, tempStr, sizeof(merchDetailRec->detailOrganizaNumber) - 1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);

  /*could not find a single required field in the terminal, so return NOT_CORRECT.*/
   return (NOT_CORRECT);

}  /*end of mcf01HasBlankReqField_MPAV	*/

INT  dcf01HasBlankReqField_EMVC (pDCF01    dcf01)
{
   CHAR tempStr  [50] = {0};
   CHAR tempdate[25] =  {0};
   CHAR yearmonthdate[9] = {0};
   /*****************************************/
   memset(tempStr, ' ', sizeof(tempStr));

   /*check to see if the dcf01->primary_key.device_id is blank or null.*/
   if(strlen((pCHAR)dcf01->primary_key.device_id) != 0)
   {
      if (strncmp((pCHAR)dcf01->primary_key.device_id, tempStr, sizeof(dcf01->primary_key.device_id) - 1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);

   /*check to see if the dcf01->merchant_id field is blank or null.*/
   if(strlen((pCHAR)dcf01->merchant_id) != 0)
   {
      if (strncmp((pCHAR)dcf01->merchant_id, tempStr, sizeof(dcf01->merchant_id) - 1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);

   /*check to see if the dcf01->organization_id field is blank or null.*/
   if(strlen((pCHAR)dcf01->organization_id) != 0)
   {
      if (strncmp((pCHAR)dcf01->organization_id, tempStr, sizeof(dcf01->organization_id) - 1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);

   /*check to see if the dcf01->category_code field is blank or null.*/
   if(strlen((pCHAR)dcf01->current_bus_date) != 0)
   {
      if (strncmp((pCHAR)dcf01->current_bus_date, tempStr, sizeof(dcf01->current_bus_date)-1) == 0)
         return(CORRECT);
   }
   else
      return(CORRECT);

  /*could not find a single required field in the terminal, so return NOT_CORRECT.*/
   return (NOT_CORRECT);

}  /*end of dcf01HasBlankReqField_EMVC	*/

/*************************************************************************************
       NAME:           populateCcf01Table()

       DESCRIPTION:    This module maps the cardholdDetailRecord which contains the 
                       cardholder information to the ccf01 table in the Data Server.
					                               
       INPUTS:         

	    OUTPUTS:        populated ccf01 structure.
                       
       RETURNS:        
		   None.                       
                               
      AUTHOR:          Faramarz Arad.
      MODIFIED BY:     None.
*************************************************************************************/
void populateCcf01Table(pmyCardHoldDetail   cardHoldDetailRecord, pCCF01 ccf01)
{
   memset(ccf01,0,sizeof(CCF01));
   
   /*farad 1/15/1999.  Hard code the "C" value for the primary_key.card_type.*/
   /*strncpy (ccf01.primary_key.card_type      ,cardHoldDetailRecord.detailCardType,sizeof(ccf01.primary_key.card_type)-1);*/
   strcpy  ((pCHAR)ccf01->primary_key.card_type      ,"C");
   strcpy  ((pCHAR)ccf01->primary_key.card_nbr       ,cardHoldDetailRecord->detailCardNumber);
   strcpy  ((pCHAR)ccf01->status                     ,cardHoldDetailRecord->detailStatus);
   strcpy  ((pCHAR)ccf01->primary_acct_nbr           ,cardHoldDetailRecord->detailPrimaryAccount);
   strcpy  ((pCHAR)ccf01->name_dba                   ,cardHoldDetailRecord->detailNameDBA);
   strcpy  ((pCHAR)ccf01->address1                   ,cardHoldDetailRecord->detailAddres1);
   strcpy  ((pCHAR)ccf01->address2                   ,cardHoldDetailRecord->detailAddres2);
   strcpy  ((pCHAR)ccf01->city                       ,cardHoldDetailRecord->detailCity);
   strcpy  ((pCHAR)ccf01->state                      ,cardHoldDetailRecord->detailState);
   strcpy  ((pCHAR)ccf01->zip_code                   ,cardHoldDetailRecord->detailZipCode);
   strcpy  ((pCHAR)ccf01->home_phone                 ,cardHoldDetailRecord->detailHomePhone);
   strcpy  ((pCHAR)ccf01->work_phone                 ,cardHoldDetailRecord->detailWorkPhone);
   strcpy  ((pCHAR)ccf01->dob                        ,cardHoldDetailRecord->detailDateBirth);
   strcpy  ((pCHAR)ccf01->profession                 ,cardHoldDetailRecord->detailProfession);
   strcpy  ((pCHAR)ccf01->specialty_code             ,cardHoldDetailRecord->detailSpeciCode);
   strcpy  ((pCHAR)ccf01->vip_acct_code              ,cardHoldDetailRecord->detailVIPAccountCode);
   strcpy  ((pCHAR)ccf01->sex                        ,cardHoldDetailRecord->detailSex);
   strcpy  ((pCHAR)ccf01->mother_maiden_name         ,cardHoldDetailRecord->detailMothMaidenName);
   strcpy  ((pCHAR)ccf01->membership_id              ,cardHoldDetailRecord->detailMembershipID);
   strcpy  ((pCHAR)ccf01->cardholder_id              ,cardHoldDetailRecord->detailCardHolder);
   strcpy  ((pCHAR)ccf01->block_status               ,cardHoldDetailRecord->detailBlockStatus);
   strcpy  ((pCHAR)ccf01->nbr_cards                  ,cardHoldDetailRecord->detailNumberofCard);

}  /*end of populateCcf01Table*/



/*************************************************************************************
       NAME:           populateCcf02pTable()

       DESCRIPTION:    This module maps the cardholdDetailRecord which contains the 
                       cardholder information to the ccf02p table in the Data Server.
					                               
       INPUTS:         

	    OUTPUTS:        populated ccf02p structure.
                       
       RETURNS:        
		   None.                       
                               
      AUTHOR:          Faramarz Arad 1/25/1998.
      MODIFIED BY:     None.
*************************************************************************************/
void populateCcf02pTable(pmyCardHoldDetail cardHoldDetailRecord, pCCF02P ccf02p)
{
   INT cnt=0;

   memset(ccf02p,0,sizeof(CCF02P));

   strcpy  ((pCHAR)ccf02p->primary_key.card_type      ,"C");   
   strcpy  ((pCHAR)ccf02p->primary_key.card_nbr       ,cardHoldDetailRecord->detailCardNumber);
   strcpy  ((pCHAR)ccf02p->primary_acct_nbr           ,cardHoldDetailRecord->detailPrimaryAccount);   
   strcpy  ((pCHAR)ccf02p->status                     ,cardHoldDetailRecord->detailStatus);

   /*Due to the spec change the old pin offset is broken up to three parts.*/
   /*1)  Reserved CHAR of length 1.  This field would be ignored for now. */
   /*2)  pin offset CHAR of length 4.  This field would be used to populate the pin offset.*/
   /*3)  pvki CHAR of length 1.  This field would be used to populate the pvki */

   strncpy ((pCHAR)ccf02p->pin_offset                 ,cardHoldDetailRecord->detailPinOffSet+1 , 4);
   strncpy ((pCHAR)ccf02p->pvki                       ,cardHoldDetailRecord->detailPinOffSet+5 , 1);

   strcpy  ((pCHAR)ccf02p->peso_saving_acct           ,cardHoldDetailRecord->detailPesoSaveAccount);
   strcpy  ((pCHAR)ccf02p->peso_current_acct          ,cardHoldDetailRecord->detailPesoCurrAccount);
   strcpy  ((pCHAR)ccf02p->dollar_saving_acct         ,cardHoldDetailRecord->detailDollarSaveAcc);
   strcpy  ((pCHAR)ccf02p->dollar_current_acct        ,cardHoldDetailRecord->detailDollarCurrAcc);
   strcpy  ((pCHAR)ccf02p->product_codes              ,cardHoldDetailRecord->detailProductCode);
   strcpy  ((pCHAR)ccf02p->date_added                 ,cardHoldDetailRecord->detailDateAdded);
   strcpy  ((pCHAR)ccf02p->expire_date                ,cardHoldDetailRecord->detailExpirDate);
   /* TF-Phani Additional expiry date checking nov-03-2010 */
   strncpy  ((pCHAR)ccf02p->maint_date                ,cardHoldDetailRecord->detailMaint_date,SECONDARY_EXP_DATE_LEN);
   strncpy	((pCHAR)ccf02p->percent_deposit,			cardHoldDetailRecord->detailPanSeqNbr,2);

   strcpy  ((pCHAR)ccf02p->effect_date                ,cardHoldDetailRecord->detailEffectiveDate);
   /*added by farad on 3/16/1999 to fix bug # */
   strcpy  ((pCHAR)ccf02p->cash_bonus_eligible        ,cardHoldDetailRecord->detailCashBounEligib);
   strcpy  ((pCHAR)ccf02p->visa_purchase_card         ,cardHoldDetailRecord->detailVisaPurchCard);
   /*added by farad on 3/18/1999 to fix bug #247.*/
   strcpy  ((pCHAR)ccf02p->allowed                    ,cardHoldDetailRecord->detailTransAllowed);
   strcpy  ((pCHAR)ccf02p->add_on_rate                ,cardHoldDetailRecord->detailAddOnRate);
   strcpy  ((pCHAR)ccf02p->add_on_term                ,cardHoldDetailRecord->detailAddOnTerm);
   strcpy  ((pCHAR)ccf02p->peso_tran_limit			  ,cardHoldDetailRecord->detailPesoTranLimit);
   strcpy  ((pCHAR)ccf02p->dollar_tran_limit		  ,cardHoldDetailRecord->detailDollarTranLimit);

   /*Note:*/
   /*The related_acct array has to be populate in order.  The values of Peso Account,*/
   /*US dollar account, Deferred account, and House Card Account are checked.  If any of*/
   /*these values are blank (in this case NULL), go to the next one. */

   /*The function parseCardHolderDetail calles returnNumbers() functions for these 4 values,*/
   /*because they are numbers.  If they are all space, returnNumbers() would return NULL.*/

   /*make sure the detailPesoAccount is not empty.*/
   if (strlen((pCHAR)cardHoldDetailRecord->detailPesoAccount) != 0)
   {
      strcpy ((pCHAR)ccf02p->related_acct[cnt].type  ,"06");
      strcpy ((pCHAR)ccf02p->related_acct[cnt].nbr   ,(pCHAR)cardHoldDetailRecord->detailPesoAccount);
      cnt++;
   }

   /*make sure the detailUSDolAccount is not empty*/
   if (strlen((pCHAR)cardHoldDetailRecord->detailUSDolAccount) != 0)
   {
      strcpy ((pCHAR)ccf02p->related_acct[cnt].type  , "05");
      strcpy ((pCHAR)ccf02p->related_acct[cnt].nbr   ,(pCHAR)cardHoldDetailRecord->detailUSDolAccount);
      cnt++;
   }
   
   /*make sure the detailDefferedAccont is not empty.*/
   if (strlen((pCHAR)cardHoldDetailRecord->detailDefferedAccont) != 0)
   {
      strcpy ((pCHAR)ccf02p->related_acct[cnt].type  , "07");
      strcpy ((pCHAR)ccf02p->related_acct[cnt].nbr   ,(pCHAR)cardHoldDetailRecord->detailDefferedAccont);
      cnt++;
   }

   /*make sure the detailHouseCardAcc is not empty.*/
   if (strlen((pCHAR)cardHoldDetailRecord->detailHouseCardAcc) != 0)
   {
      strcpy ((pCHAR)ccf02p->related_acct[cnt].type  , "08");
      strcpy ((pCHAR)ccf02p->related_acct[cnt].nbr   ,(pCHAR)cardHoldDetailRecord->detailHouseCardAcc);
      cnt++;
   }
   //Store IBM offset for CUP cards
   memset((pCHAR)ccf02p->pin, 0x20, (sizeof(ccf02p->pin) -1 ));
   if(strlen(cardHoldDetailRecord->IBMOffset)> 0)
   {
   		memcpy((pCHAR)ccf02p->pin		  ,cardHoldDetailRecord->IBMOffset,
												 (sizeof(cardHoldDetailRecord->IBMOffset)-1) );
   }
 
   // Store the coustmer mobile number.
   if(strlen(cardHoldDetailRecord->Mobilenumber)> 0)
   {
		memcpy((pCHAR)&ccf02p->pin[20]		  ,cardHoldDetailRecord->Mobilenumber,
											 (sizeof(cardHoldDetailRecord->Mobilenumber)-1) );
   }

 
}/*end of populateCcf02pTable*/

/*************************************************************************************
       NAME:           populateMcf01Table()

       DESCRIPTION:    This module maps the merchantDetailRecord which contains the 
                       merchant information to the mcf01 table in the Data Server.
					                               
       INPUTS:         

	    OUTPUTS:        populated mcf01 structure.
                       
       RETURNS:        
		   None.                       
                               
      AUTHOR:          Faramarz Arad.
      MODIFIED BY:     None.
*************************************************************************************/
void populateMcf01Table(pmyMerchantDetail merchDetailRecord, pMCF01 mcf01)
{
   memset(mcf01,0,sizeof(MCF01));
   int cup_acp_id_len=0;
   int merchGroupIdLen=0;
   strcpy  ((pCHAR)mcf01->primary_key.merchant_id       ,(pCHAR)merchDetailRecord->merchantNumber);
   strcpy  ((pCHAR)mcf01->primary_key.organization_id   ,(pCHAR)merchDetailRecord->merchOrginNumb);
   strcpy  ((pCHAR)mcf01->status                        ,(pCHAR)merchDetailRecord->merchStatus);
   strcpy  ((pCHAR)mcf01->name01                        ,(pCHAR)merchDetailRecord->merchLegalName);
   strcpy  ((pCHAR)mcf01->name02                        ,(pCHAR)merchDetailRecord->merchDBA);
   strcpy  ((pCHAR)mcf01->address01                     ,(pCHAR)merchDetailRecord->merchAddres1);
   strcpy  ((pCHAR)mcf01->address02                     ,(pCHAR)merchDetailRecord->merchAddres2);
   strcpy  ((pCHAR)mcf01->city                          ,(pCHAR)merchDetailRecord->merchCity);
   strcpy  ((pCHAR)mcf01->country                       ,(pCHAR)merchDetailRecord->merchCounty);
   strncpy  ((pCHAR)mcf01->intl_code                    ,(pCHAR)merchDetailRecord->merchCounty  ,sizeof(mcf01->intl_code)-1);
   strcpy  ((pCHAR)mcf01->state                         ,(pCHAR)merchDetailRecord->merchState);
   strcpy  ((pCHAR)mcf01->zip_code                      ,(pCHAR)merchDetailRecord->merchZipCode);
   strcpy  ((pCHAR)mcf01->currency_code                 ,(pCHAR)merchDetailRecord->merchCurrencyCode);
   strcpy  ((pCHAR)mcf01->contact                       ,(pCHAR)merchDetailRecord->merchContact);
   strcpy  ((pCHAR)mcf01->phone01                       ,(pCHAR)merchDetailRecord->merchTelphone1);
   strcpy  ((pCHAR)mcf01->phone02                       ,(pCHAR)merchDetailRecord->merchTelphone2);
   strcpy  ((pCHAR)mcf01->invoice_batch                 ,(pCHAR)merchDetailRecord->merchInvoiceBatch);
   strcpy  ((pCHAR)mcf01->invoice_subcode               ,(pCHAR)merchDetailRecord->merchInvoiceSubcode);
   strcpy  ((pCHAR)mcf01->process_control_id            ,(pCHAR)merchDetailRecord->merchProcContId);
   strcpy  ((pCHAR)mcf01->region_nbr                    ,(pCHAR)merchDetailRecord->merchRegionNumber);
   strcpy  ((pCHAR)mcf01->division_nbr                  ,(pCHAR)merchDetailRecord->merchDiviNumber);
   /*actual password postion is from 276-283 total 7 bytes, but its not a mandaotry field,
   		 * therefore password parsing is commented and this password field is used to store th MC_MCO(merchant country of origin)
   		 *  see the code below of copying MCO*/
  /* strcpy  ((pCHAR)mcf01->password                      ,(pCHAR)merchDetailRecord->merchPassWord);
   * */
   strcpy  ((pCHAR)mcf01->rps_merchant                  ,(pCHAR)merchDetailRecord->merchRPS);
   strcpy  ((pCHAR)mcf01->house_card_merchant           ,(pCHAR)merchDetailRecord->merchHouCardMerch);
   strcpy  ((pCHAR)mcf01->procurement_merchant          ,(pCHAR)merchDetailRecord->merchProcMerch);
   strcpy  ((pCHAR)mcf01->deferred_merchant             ,(pCHAR)merchDetailRecord->merchDeffMerch);
   strcpy  ((pCHAR)mcf01->cash_bonus_eligible           ,(pCHAR)merchDetailRecord->merchCashBonusElg);
   strcpy  ((pCHAR)mcf01->credit_purch_max              ,(pCHAR)merchDetailRecord->merchCredPurcMax);
   strcpy  ((pCHAR)mcf01->credit_cash_back_max          ,(pCHAR)merchDetailRecord->merchCredCashBack);
   strcpy  ((pCHAR)mcf01->credit_auth_max               ,(pCHAR)merchDetailRecord->merchCredAuthMax);
   strcpy  ((pCHAR)mcf01->credit_return_max             ,(pCHAR)merchDetailRecord->merchCredRetMax);
   strcpy  ((pCHAR)mcf01->credit_cash_adv_max           ,(pCHAR)merchDetailRecord->merchCredCashAdvMax);
   strcpy  ((pCHAR)mcf01->credit_man_entry_max          ,(pCHAR)merchDetailRecord->merchCredManEntMax);
   strcpy  ((pCHAR)mcf01->debit_purch_max               ,(pCHAR)merchDetailRecord->merchDebitPurchMax);
   strcpy  ((pCHAR)mcf01->debit_cash_back_max           ,(pCHAR)merchDetailRecord->merchDebitCashBackMax);
   strcpy  ((pCHAR)mcf01->debit_auth_max                ,(pCHAR)merchDetailRecord->merchDebitAuthMax);
   strcpy  ((pCHAR)mcf01->debit_return_max              ,(pCHAR)merchDetailRecord->merchDebitRetMax);
   strcpy  ((pCHAR)mcf01->debit_cash_adv_max            ,(pCHAR)merchDetailRecord->merchDebitCashAdvMax);
   strcpy  ((pCHAR)mcf01->debit_man_entry_max           ,(pCHAR)merchDetailRecord->merchDebitManEntMax);
   strcpy  ((pCHAR)mcf01->date_added                    ,(pCHAR)merchDetailRecord->merchDateAdded);
   strcpy  ((pCHAR)mcf01->totals_ind                    ,(pCHAR)merchDetailRecord->merchTotalIndicator);
   strcpy  ((pCHAR)mcf01->statistics_ind                ,(pCHAR)merchDetailRecord->merchStatsIndicator);
   strcpy  ((pCHAR)mcf01->vip_code                      ,(pCHAR)merchDetailRecord->merchVipcode);
   strcpy  ((pCHAR)mcf01->pos_code                      ,(pCHAR)merchDetailRecord->merchPosCode);
   strcpy  ((pCHAR)mcf01->voice_category_code           ,(pCHAR)merchDetailRecord->merchMCC);
   strcpy  ((pCHAR)mcf01->vs_tcc_code                   ,(pCHAR)merchDetailRecord->merchMSTCCcode);
   strcpy  ((pCHAR)mcf01->mc_fcc_code                   ,(pCHAR)merchDetailRecord->merchMCFCCcode);
   strcpy  ((pCHAR)mcf01->jcb_sub_code                  ,(pCHAR)merchDetailRecord->merchJCPSubCode);
   strcpy  ((pCHAR)mcf01->mall_code                     ,(pCHAR)merchDetailRecord->merchMallCode);
   strcpy  ((pCHAR)mcf01->hc_code                       ,(pCHAR)merchDetailRecord->merchHCCode);
   strcpy  ((pCHAR)mcf01->solicit_code                  ,(pCHAR)merchDetailRecord->merchSolicCode);
   strcpy  ((pCHAR)mcf01->class_code                    ,(pCHAR)merchDetailRecord->classCode);
   strcpy  ((pCHAR)mcf01->deferred_start_date           ,(pCHAR)merchDetailRecord->merchDefStartDate);
   strcpy  ((pCHAR)mcf01->deferred_end_date             ,(pCHAR)merchDetailRecord->merchDefEndDate);
   strcpy  ((pCHAR)mcf01->group_outlet_code             ,(pCHAR)merchDetailRecord->merchGroupOutletCode);
   strcpy  ((pCHAR)mcf01->group_type                    ,(pCHAR)merchDetailRecord->merchGroupType);
   strcpy  ((pCHAR)mcf01->network[2].priority_routing   ,(pCHAR)merchDetailRecord->mastercard_assigned_id);
   
   cup_acp_id_len = strlen(merchDetailRecord->cup_acceptor_id);
   if ( cup_acp_id_len >= 7)
   {
   		strncpy ((pCHAR)mcf01->network[3].priority_routing   ,(pCHAR)merchDetailRecord->cup_acceptor_id, 7);
		strncpy ((pCHAR)mcf01->network[4].priority_routing   ,(pCHAR)merchDetailRecord->cup_acceptor_id + 7 ,(cup_acp_id_len - 7 ));
   }
   else
   {
	    strncpy ((pCHAR)mcf01->network[3].priority_routing   ,(pCHAR)merchDetailRecord->cup_acceptor_id,cup_acp_id_len);
   }
   
   merchGroupIdLen = strlen(merchDetailRecord->merchGroupId);
   if ( merchGroupIdLen > 9)
   {
   		strncpy ((pCHAR)mcf01->network[5].priority_routing   ,(pCHAR)merchDetailRecord->merchGroupId, 9);
		strncpy ((pCHAR)mcf01->network[6].priority_routing   ,(pCHAR)merchDetailRecord->merchGroupId + 9 ,(merchGroupIdLen - 9 ));
   }
   else
   {
	    strncpy ((pCHAR)mcf01->network[5].priority_routing   ,(pCHAR)merchDetailRecord->merchGroupId,merchGroupIdLen);
   }
   if(merchDetailRecord->reload_merchant_flag[0] == 'Y')
	   mcf01->reload_merchant[0] = 'Y';
   else
	   mcf01->reload_merchant[0] = 'N';
   strcpy  ((pCHAR)mcf01->negative_group_id,(pCHAR)merchDetailRecord->own_merchant); /* Added for falcon OWN Merchant id*/

   /* visa Mvv */
   strncpy((pCHAR)mcf01->network[8].priority_routing,(pCHAR)merchDetailRecord->visa_mvv,9);
   mcf01->network[9].priority_routing[0] = merchDetailRecord->visa_mvv[9];

   /* MC MCO */
   strncpy((pCHAR)&mcf01->password,(pCHAR)merchDetailRecord->mc_mco,3);



}     /*end of  populateMcf01Table()*/


/*************************************************************************************
       NAME:           populateMPPARA01Table()

       DESCRIPTION:    This module maps the mpparameterDetailRecord which contains the 
                       mpparameter information to the MPPARA01 table in the Data Server.
					                               
       INPUTS:         

	    OUTPUTS:        populated MPPARA01 structure.
                       
       RETURNS:        
		   None.                       
                               
      AUTHOR:          Faramarz Arad.
      MODIFIED BY:     None.
*************************************************************************************/

void populateMPPARA01Table(pmyMpParameterDetail mpparaDetailRecord, pMPPARA01 mppara01)
{
   memset(mppara01,0,sizeof(MPPARA01));
   strcpy  ((pCHAR)mppara01->primary_key.product_code       ,(pCHAR)mpparaDetailRecord->mpparaProductCode);
   strcpy  ((pCHAR)mppara01->minmajpurchamt                 ,(pCHAR)mpparaDetailRecord->mpparaMinMajorPurchaseAmount );
   strcpy  ((pCHAR)mppara01->maxmajpurchamt                        ,(pCHAR)mpparaDetailRecord->mpparaMaxMajorPurchaseAmount);
   strcpy  ((pCHAR)mppara01->mininstamt                        ,(pCHAR)mpparaDetailRecord->mpparaMinInstallmentAmount);
   strcpy  ((pCHAR)mppara01->maxinstamt                        ,(pCHAR)mpparaDetailRecord->mpparaMaxInstallmentAmount);
   strcpy  ((pCHAR)mppara01->minmajpurchlmt                     ,(pCHAR)mpparaDetailRecord->mpparaMinMajorPurchaseLimit);
   strcpy  ((pCHAR)mppara01->minpenaltyamt                     ,(pCHAR)mpparaDetailRecord->mpparaMinPenaltyAmount);
   strcpy  ((pCHAR)mppara01->maxpenaltyamt                          ,(pCHAR)mpparaDetailRecord->mpparaMaxPenaltyAmount);
   strcpy  ((pCHAR)mppara01->penaltyper                      ,(pCHAR)mpparaDetailRecord->mpparaPenaltyPercentage);
   strcpy  ((pCHAR)mppara01->penaltyday                         ,(pCHAR)mpparaDetailRecord->mpparaPenaltyDays);
   strcpy  ((pCHAR)mppara01->minamtofaging                      ,(pCHAR)mpparaDetailRecord->mpparaMinAmtForAging);
   strcpy  ((pCHAR)mppara01->mntowrtof                     ,(pCHAR)mpparaDetailRecord->mpparaMonthsToWriteOff);
   strcpy  ((pCHAR)mppara01->BPIminamt                     ,(pCHAR)mpparaDetailRecord->mpparaBPIMinimumAmount);
   strcpy  ((pCHAR)mppara01->brokenperdinst                       ,(pCHAR)mpparaDetailRecord->mpparaBrokenPeriodInterest);
   strcpy  ((pCHAR)mppara01->minfreepurchamt                       ,(pCHAR)mpparaDetailRecord->mpparaMinPurchaseAmtForFee);
   strcpy  ((pCHAR)mppara01->errorunauthamt                 ,(pCHAR)mpparaDetailRecord->mpparaErrorUnauthorizedAmount);
   strcpy  ((pCHAR)mppara01->fixedmpfee                     ,(pCHAR)mpparaDetailRecord->mpparaFixedMPFee);
   strcpy  ((pCHAR)mppara01->mpfeepercnt                    ,(pCHAR)mpparaDetailRecord->mpparaMPFeePercentage);
   strcpy  ((pCHAR)mppara01->earlyrepayfeeper                    ,(pCHAR)mpparaDetailRecord->mpparaEarlyRepaymentFeePercentage);
   strcpy  ((pCHAR)mppara01->effective_date                  ,(pCHAR)mpparaDetailRecord->mpparaEffectiveDate);
   strcpy  ((pCHAR)mppara01->maxtrnperday                      ,(pCHAR)mpparaDetailRecord->mpparaMaxNoOfTransPerDay);
   strcpy  ((pCHAR)mppara01->maxamtperday                  ,(pCHAR)mpparaDetailRecord->mpparaMaxAmountPerDay);
   strcpy  ((pCHAR)mppara01->authoverlimit                  ,(pCHAR)mpparaDetailRecord->mpparaAuthPercentOverlimit);
   strcpy  ((pCHAR)mppara01->authpara[0].response_code          ,(pCHAR)mpparaDetailRecord->mpparaRespCode1);
   strcpy  ((pCHAR)mppara01->authpara[0].message             ,(pCHAR)mpparaDetailRecord->mpparaMessage1);
   strcpy  ((pCHAR)mppara01->authpara[1].response_code           ,(pCHAR)mpparaDetailRecord->mpparaRespCode2);
   strcpy  ((pCHAR)mppara01->authpara[1].message              ,(pCHAR)mpparaDetailRecord->mpparaMessage2);
   strcpy  ((pCHAR)mppara01->authpara[2].response_code          ,(pCHAR)mpparaDetailRecord->mpparaRespCode3);
   strcpy  ((pCHAR)mppara01->authpara[2].message               ,(pCHAR)mpparaDetailRecord->mpparaMessage3);
   
   
  }
  /*end of  populateMppara01Table()*/




/*************************************************************************************
       NAME:           populateMGF01Table()

       DESCRIPTION:    This module maps the MerchantGroupDetailRecord which contains the 
                      MerchantGroup information to the MGF01 table in the Data Server.
					                               
       INPUTS:         

	    OUTPUTS:        populated MGF01 structure.
                       
       RETURNS:        
		   None.                       
                               
      AUTHOR:          Faramarz Arad.
      MODIFIED BY:     None.
*************************************************************************************/

void populateMGF01Table(pmyMerchantGroupDetail mgfDetailRecord, pMGF01 mgf01)
{
   memset(mgf01,0,sizeof(MGF01));


     strcpy((pCHAR)mgf01->primary_key.merchant_group_id,               (pCHAR)mgfDetailRecord->mgfMerchantGroupID);
	 strcpy((pCHAR)mgf01->effective_date,                              (pCHAR)mgfDetailRecord->mgfEffectiveDate);
	 
	 strcpy((pCHAR)mgf01->merchgrp[0].Amount_limit,                    (pCHAR)mgfDetailRecord->mgfAmount_limit_1);
	 
	 strcpy((pCHAR)mgf01->merchgrp[0].merchgrepint[0].nbr_of_months,   (pCHAR)mgfDetailRecord->mgfnbr_of_months_1 );
	 strcpy((pCHAR)mgf01->merchgrp[0].merchgrepint[0].INT_rate,  	   (pCHAR)mgfDetailRecord->mgfINT_rate_1 );
	 strcpy((pCHAR)mgf01->merchgrp[0].merchgrepint[0].MSF_rate,        (pCHAR)mgfDetailRecord->mgfMSF_rate_1);
	  
	 strcpy((pCHAR)mgf01->merchgrp[0].merchgrepint[1].nbr_of_months,   (pCHAR)mgfDetailRecord->mgfnbr_of_months_2);
	 strcpy((pCHAR)mgf01->merchgrp[0].merchgrepint[1].INT_rate,        (pCHAR)mgfDetailRecord->mgfINT_rate_2);
	 strcpy((pCHAR)mgf01->merchgrp[0].merchgrepint[1].MSF_rate,        (pCHAR)mgfDetailRecord->mgfMSF_rate_2);
	  
	 strcpy((pCHAR)mgf01->merchgrp[0].merchgrepint[2].nbr_of_months,   (pCHAR)mgfDetailRecord->mgfnbr_of_months_3);
	 strcpy((pCHAR)mgf01->merchgrp[0].merchgrepint[2].INT_rate,        (pCHAR)mgfDetailRecord->mgfINT_rate_3);
	 strcpy((pCHAR)mgf01->merchgrp[0].merchgrepint[2].MSF_rate,        (pCHAR)mgfDetailRecord->mgfMSF_rate_3);
	  
	 strcpy((pCHAR)mgf01->merchgrp[0].merchgrepint[3].nbr_of_months,   (pCHAR)mgfDetailRecord->mgfnbr_of_months_4);
	 strcpy((pCHAR)mgf01->merchgrp[0].merchgrepint[3].INT_rate,        (pCHAR)mgfDetailRecord->mgfINT_rate_4);
	 strcpy((pCHAR)mgf01->merchgrp[0].merchgrepint[3].MSF_rate,        (pCHAR)mgfDetailRecord->mgfMSF_rate_4);
	 
	 strcpy((pCHAR)mgf01->merchgrp[0].merchgrepint[4].nbr_of_months,   (pCHAR)mgfDetailRecord->mgfnbr_of_months_5);
	 strcpy((pCHAR)mgf01->merchgrp[0].merchgrepint[4].INT_rate,        (pCHAR)mgfDetailRecord->mgfINT_rate_5);
	 strcpy((pCHAR)mgf01->merchgrp[0].merchgrepint[4].MSF_rate,        (pCHAR)mgfDetailRecord->mgfMSF_rate_5);
	 
	 strcpy((pCHAR)mgf01->merchgrp[0].merchgrepint[5].nbr_of_months,   (pCHAR)mgfDetailRecord->mgfnbr_of_months_6);
	 strcpy((pCHAR)mgf01->merchgrp[0].merchgrepint[5].INT_rate,        (pCHAR)mgfDetailRecord->mgfINT_rate_6);
	 strcpy((pCHAR)mgf01->merchgrp[0].merchgrepint[5].MSF_rate,        (pCHAR)mgfDetailRecord->mgfMSF_rate_6);
	 
	 strcpy((pCHAR)mgf01->merchgrp[0].merchgrepint[6].nbr_of_months,   (pCHAR)mgfDetailRecord->mgfnbr_of_months_7);
	 strcpy((pCHAR)mgf01->merchgrp[0].merchgrepint[6].INT_rate,        (pCHAR)mgfDetailRecord->mgfINT_rate_7);
	 strcpy((pCHAR)mgf01->merchgrp[0].merchgrepint[6].MSF_rate,        (pCHAR)mgfDetailRecord->mgfMSF_rate_7);
	 
	 strcpy((pCHAR)mgf01->merchgrp[0].merchgrepint[7].nbr_of_months,   (pCHAR)mgfDetailRecord->mgfnbr_of_months_8);
	 strcpy((pCHAR)mgf01->merchgrp[0].merchgrepint[7].INT_rate,        (pCHAR)mgfDetailRecord->mgfINT_rate_8);
	 strcpy((pCHAR)mgf01->merchgrp[0].merchgrepint[7].MSF_rate,        (pCHAR)mgfDetailRecord->mgfMSF_rate_8);
	 
	 strcpy((pCHAR)mgf01->merchgrp[0].merchgrepint[8].nbr_of_months,   (pCHAR)mgfDetailRecord->mgfnbr_of_months_9);
	 strcpy((pCHAR)mgf01->merchgrp[0].merchgrepint[8].INT_rate,        (pCHAR)mgfDetailRecord->mgfINT_rate_9);
	 strcpy((pCHAR)mgf01->merchgrp[0].merchgrepint[8].MSF_rate,        (pCHAR)mgfDetailRecord->mgfMSF_rate_9);
	 
	 strcpy((pCHAR)mgf01->merchgrp[0].merchgrepint[9].nbr_of_months,   (pCHAR)mgfDetailRecord->mgfnbr_of_months_10);
	 strcpy((pCHAR)mgf01->merchgrp[0].merchgrepint[9].INT_rate,        (pCHAR)mgfDetailRecord->mgfINT_rate_10);
	 strcpy((pCHAR)mgf01->merchgrp[0].merchgrepint[9].MSF_rate,        (pCHAR)mgfDetailRecord->mgfMSF_rate_10);

	 strcpy((pCHAR)mgf01->merchgrp[0].merchgrepint[10].nbr_of_months,   (pCHAR)mgfDetailRecord->mgfnbr_of_months_10A);
	 strcpy((pCHAR)mgf01->merchgrp[0].merchgrepint[10].INT_rate,        (pCHAR)mgfDetailRecord->mgfINT_rate_10A);
	 strcpy((pCHAR)mgf01->merchgrp[0].merchgrepint[10].MSF_rate,        (pCHAR)mgfDetailRecord->mgfMSF_rate_10A);

	 strcpy((pCHAR)mgf01->merchgrp[0].merchgrepint[11].nbr_of_months,   (pCHAR)mgfDetailRecord->mgfnbr_of_months_10B);
	 strcpy((pCHAR)mgf01->merchgrp[0].merchgrepint[11].INT_rate,        (pCHAR)mgfDetailRecord->mgfINT_rate_10B);
	 strcpy((pCHAR)mgf01->merchgrp[0].merchgrepint[11].MSF_rate,        (pCHAR)mgfDetailRecord->mgfMSF_rate_10B);
		  
	 strcpy((pCHAR)mgf01->merchgrp[1].Amount_limit,                    (pCHAR)mgfDetailRecord->mgfAmount_limit_2);
	 
	 strcpy((pCHAR)mgf01->merchgrp[1].merchgrepint[0].nbr_of_months,   (pCHAR)mgfDetailRecord->mgfnbr_of_months_11);
	 strcpy((pCHAR)mgf01->merchgrp[1].merchgrepint[0].INT_rate,	   (pCHAR)mgfDetailRecord->mgfINT_rate_11);
	 strcpy((pCHAR)mgf01->merchgrp[1].merchgrepint[0].MSF_rate,	   (pCHAR)mgfDetailRecord->mgfMSF_rate_11);
		   
	 strcpy((pCHAR)mgf01->merchgrp[1].merchgrepint[1].nbr_of_months,    (pCHAR)mgfDetailRecord->mgfnbr_of_months_12);
	 strcpy((pCHAR)mgf01->merchgrp[1].merchgrepint[1].INT_rate,	    (pCHAR)mgfDetailRecord->mgfINT_rate_12);
	 strcpy((pCHAR)mgf01->merchgrp[1].merchgrepint[1].MSF_rate,	    (pCHAR)mgfDetailRecord->mgfMSF_rate_12);
	  
	 strcpy((pCHAR)mgf01->merchgrp[1].merchgrepint[2].nbr_of_months,	(pCHAR)mgfDetailRecord->mgfnbr_of_months_13);
	 strcpy((pCHAR)mgf01->merchgrp[1].merchgrepint[2].INT_rate,		(pCHAR)mgfDetailRecord->mgfINT_rate_13);
	 strcpy((pCHAR)mgf01->merchgrp[1].merchgrepint[2].MSF_rate,		(pCHAR)mgfDetailRecord->mgfMSF_rate_13);
		   
	 strcpy((pCHAR)mgf01->merchgrp[1].merchgrepint[3].nbr_of_months,	(pCHAR)mgfDetailRecord->mgfnbr_of_months_14 );
	 strcpy((pCHAR)mgf01->merchgrp[1].merchgrepint[3].INT_rate,		(pCHAR)mgfDetailRecord->mgfINT_rate_14);
	 strcpy((pCHAR)mgf01->merchgrp[1].merchgrepint[3].MSF_rate,		(pCHAR)mgfDetailRecord->mgfMSF_rate_14);
		  
	 strcpy((pCHAR)mgf01->merchgrp[1].merchgrepint[4].nbr_of_months,	(pCHAR)mgfDetailRecord->mgfnbr_of_months_15);
	 strcpy((pCHAR)mgf01->merchgrp[1].merchgrepint[4].INT_rate,		(pCHAR)mgfDetailRecord->mgfINT_rate_15);
	 strcpy((pCHAR)mgf01->merchgrp[1].merchgrepint[4].MSF_rate,		(pCHAR)mgfDetailRecord->mgfMSF_rate_15);
		  
	 strcpy((pCHAR)mgf01->merchgrp[1].merchgrepint[5].nbr_of_months,	(pCHAR)mgfDetailRecord->mgfnbr_of_months_16);
	 strcpy((pCHAR)mgf01->merchgrp[1].merchgrepint[5].INT_rate,		(pCHAR)mgfDetailRecord->mgfINT_rate_16);
	 strcpy((pCHAR)mgf01->merchgrp[1].merchgrepint[5].MSF_rate,		(pCHAR)mgfDetailRecord->mgfMSF_rate_16);
		  
	 strcpy((pCHAR)mgf01->merchgrp[1].merchgrepint[6].nbr_of_months,	(pCHAR)mgfDetailRecord->mgfnbr_of_months_17);
	 strcpy((pCHAR)mgf01->merchgrp[1].merchgrepint[6].INT_rate,		(pCHAR)mgfDetailRecord->mgfINT_rate_17);
	 strcpy((pCHAR)mgf01->merchgrp[1].merchgrepint[6].MSF_rate,		(pCHAR)mgfDetailRecord->mgfMSF_rate_17);
		  
	 strcpy((pCHAR)mgf01->merchgrp[1].merchgrepint[7].nbr_of_months,	(pCHAR)mgfDetailRecord->mgfnbr_of_months_18);
	 strcpy((pCHAR)mgf01->merchgrp[1].merchgrepint[7].INT_rate,		(pCHAR)mgfDetailRecord->mgfINT_rate_18);
	 strcpy((pCHAR)mgf01->merchgrp[1].merchgrepint[7].MSF_rate,		(pCHAR)mgfDetailRecord->mgfMSF_rate_18);
		  
	 strcpy((pCHAR)mgf01->merchgrp[1].merchgrepint[8].nbr_of_months,	(pCHAR)mgfDetailRecord->mgfnbr_of_months_19);
	 strcpy((pCHAR)mgf01->merchgrp[1].merchgrepint[8].INT_rate,		(pCHAR)mgfDetailRecord->mgfINT_rate_19);
	 strcpy((pCHAR)mgf01->merchgrp[1].merchgrepint[8].MSF_rate,		(pCHAR)mgfDetailRecord->mgfMSF_rate_19);
		  
	 strcpy((pCHAR)mgf01->merchgrp[1].merchgrepint[9].nbr_of_months,	(pCHAR)mgfDetailRecord->mgfnbr_of_months_20);
	 strcpy((pCHAR)mgf01->merchgrp[1].merchgrepint[9].INT_rate,		(pCHAR)mgfDetailRecord->mgfINT_rate_20);
	 strcpy((pCHAR)mgf01->merchgrp[1].merchgrepint[9].MSF_rate,		(pCHAR)mgfDetailRecord->mgfMSF_rate_20);

	 strcpy((pCHAR)mgf01->merchgrp[1].merchgrepint[10].nbr_of_months,	(pCHAR)mgfDetailRecord->mgfnbr_of_months_20A);
	 strcpy((pCHAR)mgf01->merchgrp[1].merchgrepint[10].INT_rate,		(pCHAR)mgfDetailRecord->mgfINT_rate_20A);
	 strcpy((pCHAR)mgf01->merchgrp[1].merchgrepint[10].MSF_rate,		(pCHAR)mgfDetailRecord->mgfMSF_rate_20A);

	 strcpy((pCHAR)mgf01->merchgrp[1].merchgrepint[11].nbr_of_months,	(pCHAR)mgfDetailRecord->mgfnbr_of_months_20B);
	 strcpy((pCHAR)mgf01->merchgrp[1].merchgrepint[11].INT_rate,		(pCHAR)mgfDetailRecord->mgfINT_rate_20B);
	 strcpy((pCHAR)mgf01->merchgrp[1].merchgrepint[11].MSF_rate,		(pCHAR)mgfDetailRecord->mgfMSF_rate_20B);
			   
	 strcpy((pCHAR)mgf01->merchgrp[2].Amount_limit,                         (pCHAR)mgfDetailRecord->mgfAmount_limit_3);
	 
	 strcpy((pCHAR)mgf01->merchgrp[2].merchgrepint[0].nbr_of_months,        (pCHAR)mgfDetailRecord->mgfnbr_of_months_21);
	 strcpy((pCHAR)mgf01->merchgrp[2].merchgrepint[0].INT_rate,		(pCHAR)mgfDetailRecord->mgfINT_rate_21);
	 strcpy((pCHAR)mgf01->merchgrp[2].merchgrepint[0].MSF_rate,		(pCHAR)mgfDetailRecord->mgfMSF_rate_21);
		   
	 strcpy((pCHAR)mgf01->merchgrp[2].merchgrepint[1].nbr_of_months,	(pCHAR)mgfDetailRecord->mgfnbr_of_months_22);
	 strcpy((pCHAR)mgf01->merchgrp[2].merchgrepint[1].INT_rate,		(pCHAR)mgfDetailRecord->mgfINT_rate_22);
	 strcpy((pCHAR)mgf01->merchgrp[2].merchgrepint[1].MSF_rate,		(pCHAR)mgfDetailRecord->mgfMSF_rate_22);
	  
	 strcpy((pCHAR)mgf01->merchgrp[2].merchgrepint[2].nbr_of_months,	(pCHAR)mgfDetailRecord->mgfnbr_of_months_23);
	 strcpy((pCHAR)mgf01->merchgrp[2].merchgrepint[2].INT_rate,		(pCHAR)mgfDetailRecord->mgfINT_rate_23);
	 strcpy((pCHAR)mgf01->merchgrp[2].merchgrepint[2].MSF_rate,		(pCHAR)mgfDetailRecord->mgfMSF_rate_23);
		   
	 strcpy((pCHAR)mgf01->merchgrp[2].merchgrepint[3].nbr_of_months,	(pCHAR)mgfDetailRecord->mgfnbr_of_months_24);
	 strcpy((pCHAR)mgf01->merchgrp[2].merchgrepint[3].INT_rate,		(pCHAR)mgfDetailRecord->mgfINT_rate_24);
	 strcpy((pCHAR)mgf01->merchgrp[2].merchgrepint[3].MSF_rate,		(pCHAR)mgfDetailRecord->mgfMSF_rate_24);
		  
	 strcpy((pCHAR)mgf01->merchgrp[2].merchgrepint[4].nbr_of_months,	(pCHAR)mgfDetailRecord->mgfnbr_of_months_25);
	 strcpy((pCHAR)mgf01->merchgrp[2].merchgrepint[4].INT_rate,		(pCHAR)mgfDetailRecord->mgfINT_rate_25);
	 strcpy((pCHAR)mgf01->merchgrp[2].merchgrepint[4].MSF_rate,		(pCHAR)mgfDetailRecord->mgfMSF_rate_25);
		  
	 strcpy((pCHAR)mgf01->merchgrp[2].merchgrepint[5].nbr_of_months,	(pCHAR)mgfDetailRecord->mgfnbr_of_months_26);
	 strcpy((pCHAR)mgf01->merchgrp[2].merchgrepint[5].INT_rate,		(pCHAR)mgfDetailRecord->mgfINT_rate_26);
	 strcpy((pCHAR)mgf01->merchgrp[2].merchgrepint[5].MSF_rate,		(pCHAR)mgfDetailRecord->mgfMSF_rate_26);
		  
	 strcpy((pCHAR)mgf01->merchgrp[2].merchgrepint[6].nbr_of_months,	(pCHAR)mgfDetailRecord->mgfnbr_of_months_27);
	 strcpy((pCHAR)mgf01->merchgrp[2].merchgrepint[6].INT_rate,		(pCHAR)mgfDetailRecord->mgfINT_rate_27);
	 strcpy((pCHAR)mgf01->merchgrp[2].merchgrepint[6].MSF_rate,		(pCHAR)mgfDetailRecord->mgfMSF_rate_27);
		  
	 strcpy((pCHAR)mgf01->merchgrp[2].merchgrepint[7].nbr_of_months,	(pCHAR)mgfDetailRecord->mgfnbr_of_months_28);
	 strcpy((pCHAR)mgf01->merchgrp[2].merchgrepint[7].INT_rate,		(pCHAR)mgfDetailRecord->mgfINT_rate_28);
	 strcpy((pCHAR)mgf01->merchgrp[2].merchgrepint[7].MSF_rate,		(pCHAR)mgfDetailRecord->mgfMSF_rate_28);
		  
	 strcpy((pCHAR)mgf01->merchgrp[2].merchgrepint[8].nbr_of_months,	(pCHAR)mgfDetailRecord->mgfnbr_of_months_29);
	 strcpy((pCHAR)mgf01->merchgrp[2].merchgrepint[8].INT_rate,		(pCHAR)mgfDetailRecord->mgfINT_rate_29);
	 strcpy((pCHAR)mgf01->merchgrp[2].merchgrepint[8].MSF_rate,		(pCHAR)mgfDetailRecord->mgfMSF_rate_29);
		  
	 strcpy((pCHAR)mgf01->merchgrp[2].merchgrepint[9].nbr_of_months,	(pCHAR)mgfDetailRecord->mgfnbr_of_months_30);
	 strcpy((pCHAR)mgf01->merchgrp[2].merchgrepint[9].INT_rate,		(pCHAR)mgfDetailRecord->mgfINT_rate_30);
	 strcpy((pCHAR)mgf01->merchgrp[2].merchgrepint[9].MSF_rate,		(pCHAR)mgfDetailRecord->mgfMSF_rate_30);

	 strcpy((pCHAR)mgf01->merchgrp[2].merchgrepint[10].nbr_of_months,	(pCHAR)mgfDetailRecord->mgfnbr_of_months_30A);
	 strcpy((pCHAR)mgf01->merchgrp[2].merchgrepint[10].INT_rate,		(pCHAR)mgfDetailRecord->mgfINT_rate_30A);
	 strcpy((pCHAR)mgf01->merchgrp[2].merchgrepint[10].MSF_rate,		(pCHAR)mgfDetailRecord->mgfMSF_rate_30A);

	 strcpy((pCHAR)mgf01->merchgrp[2].merchgrepint[11].nbr_of_months,	(pCHAR)mgfDetailRecord->mgfnbr_of_months_30B);
	 strcpy((pCHAR)mgf01->merchgrp[2].merchgrepint[11].INT_rate,		(pCHAR)mgfDetailRecord->mgfINT_rate_30B);
	 strcpy((pCHAR)mgf01->merchgrp[2].merchgrepint[11].MSF_rate,		(pCHAR)mgfDetailRecord->mgfMSF_rate_30B);
	
	 strcpy((pCHAR)mgf01->merchgrp[3].Amount_limit,                         (pCHAR)mgfDetailRecord->mgfAmount_limit_4 );
	 
	 strcpy((pCHAR)mgf01->merchgrp[3].merchgrepint[0].nbr_of_months,        (pCHAR)mgfDetailRecord->mgfnbr_of_months_31  );
	 strcpy((pCHAR)mgf01->merchgrp[3].merchgrepint[0].INT_rate,		(pCHAR)mgfDetailRecord->mgfINT_rate_31);
	 strcpy((pCHAR)mgf01->merchgrp[3].merchgrepint[0].MSF_rate,		(pCHAR)mgfDetailRecord->mgfMSF_rate_31);
		   
	 strcpy((pCHAR)mgf01->merchgrp[3].merchgrepint[1].nbr_of_months,	(pCHAR)mgfDetailRecord->mgfnbr_of_months_32);
	 strcpy((pCHAR)mgf01->merchgrp[3].merchgrepint[1].INT_rate,		(pCHAR)mgfDetailRecord->mgfINT_rate_32);
	 strcpy((pCHAR)mgf01->merchgrp[3].merchgrepint[1].MSF_rate,		(pCHAR)mgfDetailRecord->mgfMSF_rate_32);
	  
	 strcpy((pCHAR)mgf01->merchgrp[3].merchgrepint[2].nbr_of_months,	(pCHAR)mgfDetailRecord->mgfnbr_of_months_33);
	 strcpy((pCHAR)mgf01->merchgrp[3].merchgrepint[2].INT_rate,		(pCHAR)mgfDetailRecord->mgfINT_rate_33);
	 strcpy((pCHAR)mgf01->merchgrp[3].merchgrepint[2].MSF_rate,		(pCHAR)mgfDetailRecord->mgfMSF_rate_33);
		   
	 strcpy((pCHAR)mgf01->merchgrp[3].merchgrepint[3].nbr_of_months,	(pCHAR)mgfDetailRecord->mgfnbr_of_months_34);
	 strcpy((pCHAR)mgf01->merchgrp[3].merchgrepint[3].INT_rate,		(pCHAR)mgfDetailRecord->mgfINT_rate_34);
	 strcpy((pCHAR)mgf01->merchgrp[3].merchgrepint[3].MSF_rate,		(pCHAR)mgfDetailRecord->mgfMSF_rate_34);
		  
	 strcpy((pCHAR)mgf01->merchgrp[3].merchgrepint[4].nbr_of_months,	(pCHAR)mgfDetailRecord->mgfnbr_of_months_35);
	 strcpy((pCHAR)mgf01->merchgrp[3].merchgrepint[4].INT_rate,		(pCHAR)mgfDetailRecord->mgfINT_rate_35);
	 strcpy((pCHAR)mgf01->merchgrp[3].merchgrepint[4].MSF_rate,		(pCHAR)mgfDetailRecord->mgfMSF_rate_35);
		  
	 strcpy((pCHAR)mgf01->merchgrp[3].merchgrepint[5].nbr_of_months,	(pCHAR)mgfDetailRecord->mgfnbr_of_months_36);
	 strcpy((pCHAR)mgf01->merchgrp[3].merchgrepint[5].INT_rate,		(pCHAR)mgfDetailRecord->mgfINT_rate_36);
	 strcpy((pCHAR)mgf01->merchgrp[3].merchgrepint[5].MSF_rate,		(pCHAR)mgfDetailRecord->mgfMSF_rate_36);
		  
	 strcpy((pCHAR)mgf01->merchgrp[3].merchgrepint[6].nbr_of_months,	(pCHAR)mgfDetailRecord->mgfnbr_of_months_37);
	 strcpy((pCHAR)mgf01->merchgrp[3].merchgrepint[6].INT_rate,		(pCHAR)mgfDetailRecord->mgfINT_rate_37);
	 strcpy((pCHAR)mgf01->merchgrp[3].merchgrepint[6].MSF_rate,		(pCHAR)mgfDetailRecord->mgfMSF_rate_37);
		  
	 strcpy((pCHAR)mgf01->merchgrp[3].merchgrepint[7].nbr_of_months,	(pCHAR)mgfDetailRecord->mgfnbr_of_months_38);
	 strcpy((pCHAR)mgf01->merchgrp[3].merchgrepint[7].INT_rate,		(pCHAR)mgfDetailRecord->mgfINT_rate_38);
	 strcpy((pCHAR)mgf01->merchgrp[3].merchgrepint[7].MSF_rate,		(pCHAR)mgfDetailRecord->mgfMSF_rate_38);
		  
	 strcpy((pCHAR)mgf01->merchgrp[3].merchgrepint[8].nbr_of_months,	(pCHAR)mgfDetailRecord->mgfnbr_of_months_39);
	 strcpy((pCHAR)mgf01->merchgrp[3].merchgrepint[8].INT_rate,		(pCHAR)mgfDetailRecord->mgfINT_rate_39);
	 strcpy((pCHAR)mgf01->merchgrp[3].merchgrepint[8].MSF_rate,		(pCHAR)mgfDetailRecord->mgfMSF_rate_39);
		  
	 strcpy((pCHAR)mgf01->merchgrp[3].merchgrepint[9].nbr_of_months,	(pCHAR)mgfDetailRecord->mgfnbr_of_months_40);
	 strcpy((pCHAR)mgf01->merchgrp[3].merchgrepint[9].INT_rate,		(pCHAR)mgfDetailRecord->mgfINT_rate_40);
	 strcpy((pCHAR)mgf01->merchgrp[3].merchgrepint[9].MSF_rate,		(pCHAR)mgfDetailRecord->mgfMSF_rate_40);

  	 strcpy((pCHAR)mgf01->merchgrp[3].merchgrepint[10].nbr_of_months, (pCHAR)mgfDetailRecord->mgfnbr_of_months_40A);
	 strcpy((pCHAR)mgf01->merchgrp[3].merchgrepint[10].INT_rate,	  (pCHAR)mgfDetailRecord->mgfINT_rate_40A);
	 strcpy((pCHAR)mgf01->merchgrp[3].merchgrepint[10].MSF_rate,	  (pCHAR)mgfDetailRecord->mgfMSF_rate_40A);

	 strcpy((pCHAR)mgf01->merchgrp[3].merchgrepint[11].nbr_of_months,	(pCHAR)mgfDetailRecord->mgfnbr_of_months_40B);
	 strcpy((pCHAR)mgf01->merchgrp[3].merchgrepint[11].INT_rate,		(pCHAR)mgfDetailRecord->mgfINT_rate_40B);
	 strcpy((pCHAR)mgf01->merchgrp[3].merchgrepint[11].MSF_rate,		(pCHAR)mgfDetailRecord->mgfMSF_rate_40B);

   
  }
  /*end of  populatemgf01Table()*/


/*************************************************************************************
       NAME:           populateMct01Table()

       DESCRIPTION:    This module maps the merchantDetailRecord which contains the 
                       merchant information to the mct01 table in the Data Server.
					                               
       INPUTS:         

	    OUTPUTS:        populated mct01 structure.
                       
       RETURNS:        
		   None.                       
                               
      AUTHOR:          Faramarz Arad.
      MODIFIED BY:     None.
*************************************************************************************/
void  populateMct01Table (pmyMerchantDetail merchDetailRecord, pcardType subStruct, pMCT01 mct01)
{
   memset(mct01,0,sizeof(MCT01));
   
   strncpy ((pCHAR)mct01->primary_key.card_type        ,(pCHAR)subStruct->cardTypeAcc,sizeof(mct01->primary_key.card_type)-1);
   strcpy  ((pCHAR)mct01->primary_key.merchant_id      ,(pCHAR)merchDetailRecord->merchantNumber);
   strcpy  ((pCHAR)mct01->primary_key.organization_id  ,(pCHAR)merchDetailRecord->merchOrginNumb);
   strcpy  ((pCHAR)mct01->acquirer_id                  ,(pCHAR)subStruct->cardTypeAcquiID);
   strcpy  ((pCHAR)mct01->corporate_id                 ,(pCHAR)subStruct->cardTypeCorpID);
   strncpy ((pCHAR)mct01->settlement_id                ,(pCHAR)subStruct->cardTypeMerchID,sizeof(mct01->settlement_id)-1);
   strcpy  ((pCHAR)mct01->floor_limit                  ,(pCHAR)subStruct->cardTypeFloorLimit);

}  /*end of populateMct01Table*/


/*************************************************************************************
       NAME:           processDEF01Table()

       DESCRIPTION:    This module populates the DEF01 table and sends it 
                       to the Data Server.
					                               
       INPUTS:         myMerchantDetail structure.
                       term,
                       factor,
                       minAmount,

	    OUTPUTS:        
                       
       RETURNS:        
		   None.                       
                               
      AUTHOR:          Faramarz Arad 1/26/1999.
      MODIFIED BY:     None.
*************************************************************************************/

INT processDEF01Table(pmyMerchantDetail merchDetailRecord ,pCHAR term,
                                                           pCHAR factor,
                                                           pCHAR minAmount)
{
   DEF01 def01;
   CHAR  temp_str[100];
   BYTE  ret;
   /*CHAR tempMsg[100];*/

   /*make sure the term is not blank.  If it is return without processing.*/
   if (!strlen(term))
      return(FALSE);

   memset(&def01,0,sizeof(def01));

   /*populate the def01 table before sending it to the data server.*/
   /*I should take the next few strcpy and put them in a function.  I have no time */
   /*to do that.  I just repeated the same steps in this function, which is a bad idea.*/
   strcpy ((pCHAR)def01.primary_key.organization_id    , (pCHAR)merchDetailRecord->merchOrginNumb);
   strcpy ((pCHAR)def01.primary_key.merchant_id        , (pCHAR)merchDetailRecord->merchantNumber);
   strcpy ((pCHAR)def01.primary_key.term_length        , term);
   strcpy ((pCHAR)def01.factor                         , factor);
   strcpy ((pCHAR)def01.deferred_min_amount            , minAmount);
   strcpy ((pCHAR)def01.deferred_max_amount            , (pCHAR)merchDetailRecord->merchDefMaxAcc);
 
   /* Update the record.  If it fails due to not found, insert it.
    * Logically, the code could be structured better here,
    * but since speed is critical, and 98-99% of the records
    * are updates, leave the code structured this way.
    */
	if (DB_NullCheck_Flag==1)
	{
		CheckNULLTerminatedinDEF01 (&def01,MODE_UPDATE);
	}
   ret = db_update_def01(&def01,temp_str);
   if (PTEMSG_OK != ret)
   {
      if (PTEMSG_NOT_FOUND == ret)
      {
		if (DB_NullCheck_Flag==1)
		{
			CheckNULLTerminatedinDEF01 (&def01,MODE_INSERT);
		}
         ret = db_insert_def01(&def01,temp_str);
      }
      if (PTEMSG_OK != ret)
         errorMsgNotInsert(deferred_record,(pCHAR)def01.primary_key.organization_id, temp_str); 
   }

   return(TRUE);
} /*end of processDEF01Table function.*/
/************************************/
/****************************************/

INT clearMCT01Table(pmyMerchantDetail merchDetailRecord, pcardType subStruct)
{
	MCT01 mct01;
   CHAR  temp_str[256],time_date[25]={0};
   BYTE  ret;
   CHAR  tempMsg[256]={0};
  if(strlen(subStruct->cardTypeAcc) != 0)
   {
      memset(temp_str, ' ', sizeof(temp_str));
      if (strncmp(subStruct->cardTypeAcc, temp_str, sizeof(subStruct->cardTypeAcc)-1) == 0)
         return(FALSE);
   }
   else
      return(FALSE);
   
   memset(&mct01,0,sizeof(MCT01));

   strcpy  ((pCHAR)mct01.primary_key.merchant_id      ,(pCHAR)merchDetailRecord->merchantNumber);
   strcpy  ((pCHAR)mct01.primary_key.organization_id  ,(pCHAR)merchDetailRecord->merchOrginNumb);
	ret = db_clear_mct01(&mct01, temp_str);
	if (PTEMSG_OK != ret)
    {
      // Populate message in dump file and continue..
		sprintf(temp_str,"MCT01 delete failed for card type(%s)",subStruct->cardTypeAcc);
		errorMsgNotInsert(card_type_record, (pCHAR)mct01.primary_key.merchant_id, temp_str);
    }
	ptetime_get_timestamp( time_date );
	sprintf(tempMsg,"%s: ",time_date);
	fputs(tempMsg,HstMcF01Ptr);
	sprintf(tempMsg,"MCT01 deleted for MID(%s) \n",mct01.primary_key.merchant_id);
	fputs(tempMsg,HstMcF01Ptr);
	return(TRUE);
}
/************************************/
/****************************************/
INT processMCT01Table(pmyMerchantDetail merchDetailRecord, pcardType subStruct)
{
   MCT01 mct01;
   CHAR  temp_str[256]={0},time_date[25]={0};
   BYTE  ret;
   char  tempMsg[256]={0};
   CHAR primary_key[35]={0};
   /*check to see if the cardTypeAcc is populated.  If this field is blank, there is */
   /*no need to populate the data server with the rest of the information.  This is done */
   /*to fix bug #335.*/
   if(strlen(subStruct->cardTypeAcc) != 0)
   {
      memset(temp_str, ' ', sizeof(temp_str));
      if (strncmp(subStruct->cardTypeAcc, temp_str, sizeof(subStruct->cardTypeAcc)-1) == 0)
         return(FALSE);
   }
   else
      return(FALSE);

   memset(&mct01,0,sizeof(MCT01));
   
   strncpy ((pCHAR)mct01.primary_key.card_type        ,(pCHAR)subStruct->cardTypeAcc,sizeof(mct01.primary_key.card_type)-1);
   strcpy  ((pCHAR)mct01.primary_key.merchant_id      ,(pCHAR)merchDetailRecord->merchantNumber);
   strcpy  ((pCHAR)mct01.primary_key.organization_id  ,(pCHAR)merchDetailRecord->merchOrginNumb);
   strcpy  ((pCHAR)mct01.acquirer_id                  ,(pCHAR)subStruct->cardTypeAcquiID);
   strcpy  ((pCHAR)mct01.corporate_id                 ,(pCHAR)subStruct->cardTypeCorpID);
   strncpy ((pCHAR)mct01.settlement_id                ,(pCHAR)subStruct->cardTypeMerchID,sizeof(mct01.settlement_id)-1);
   strcpy  ((pCHAR)mct01.floor_limit                  ,(pCHAR)subStruct->cardTypeFloorLimit);

   /* Update the record.  If it fails due to not found, insert it.
    * Logically, the code could be structured better here,
    * but since speed is critical, and 98-99% of the records
    * are updates, leave the code structured this way.
    */
	if (DB_NullCheck_Flag==1)
	{
		CheckNULLTerminatedinMCT01 (&mct01,MODE_UPDATE);
	}
   ret = db_update_mct01(&mct01, temp_str);
   if(PTEMSG_OK == ret)
   {
	   sprintf(tempMsg,"Updated card type:%s for MID:%s and organization_id:%s",mct01.primary_key.card_type,mct01.primary_key.merchant_id ,mct01.primary_key.organization_id);
	   Log_Db_Statics_And_Oracle_Db_Error_To_File(tempMsg);
   }
   if (PTEMSG_OK != ret)
   {
      if (PTEMSG_NOT_FOUND == ret)
      {
		if (DB_NullCheck_Flag==1)
		{
			CheckNULLTerminatedinMCT01 (&mct01,MODE_INSERT);
		}
         ret = db_insert_mct01(&mct01, temp_str);
      }
      if (PTEMSG_OK != ret)
      {
    	  errorMsgNotInsert(card_type_record, (pCHAR)mct01.primary_key.merchant_id, temp_str);
      }
      else
      {
		  if(strlen(cardtype_details) < sizeof(cardtype_details))
		  {
			  strncat(cardtype_details,"card type:",11);
			  strncat(cardtype_details,mct01.primary_key.card_type,4);
			  strncat(cardtype_details,"|",1);
		  }

      }
   }

	return(TRUE);

}  /*end of void processMCT01Table(m*/



/*************************************************************************************
       NAME:           returnNumbers()

       DESCRIPTION:    This module takes the character array.  It compares it
       against a number array.  When it enconters the first non number character, it 
       replaces it with a null.  This code requires the end of the input array 
       be the Null character.
					                               
       INPUTS:         input array of numbers.

	    OUTPUTS:        None.
                       
       RETURNS:        output array of numbers.
		           
                               
      AUTHOR:          Faramarz Arad.
      MODIFIED BY:     None.
*************************************************************************************/
void returnNumbers(pCHAR input)
{
   INT cnt;


   cnt = strlen(input) - 1;
   while( (cnt >= 0) && (!isdigit(input[cnt])) )
   {
      input[cnt] = '\0';
      cnt--;
   }
} /* end of returnNumbers*/


/*************************************************************************************
       NAME:           errorMsgBlankField()

       DESCRIPTION:    This module is used to unify all of the entries to the
                       dump file.
					                               
       INPUTS:         None.

       OUTPUTS:        None.
       RETURNS:        None.
		           
                               
      AUTHOR:          Faramarz Arad.
      MODIFIED BY:     None.
*************************************************************************************/
void errorMsgBlankField(pCHAR record, pCHAR recordid)
{
   CHAR errormsg[256] = "";
   CHAR time_date[25] = "";

   /* Get current system date & time */
   ptetime_get_timestamp( time_date );

   sprintf(errormsg, "%s  >>  Blank Required Field on %s: %s\n",
           time_date, record, recordid);
   fputs(errormsg,dumpPtr);

}/*errorMsgBlankField()*/

/**********************************************************
************************************************************/
void errorMsgNotInsert(pCHAR record, pCHAR recordid, pCHAR errorMsg)
{
   CHAR errormsg[526] = "";
   CHAR time_date[25] = "";

   /* Get current system date & time */
   ptetime_get_timestamp( time_date );

   sprintf(errormsg, "%s  >>  Insert/Update Failed on %s: %s, %s\n",
           time_date, record, recordid, errorMsg);
   fputs(errormsg,dumpPtr);

} /* errorMsgNotInsert()*/

void errorMsgNotUpdate(pCHAR record, pCHAR recordid, pCHAR errorMsg, int ret)
{
   CHAR errormsg[526] = "";
   CHAR time_date[25] = "";
   char masked_card_num[20] = "";

   /* Get current system date & time */
   ptetime_get_timestamp( time_date );

   mask_card_number_import(recordid, &masked_card_num);
   if(ret == RESOURCE_BUSY)
   {
	   sprintf(errormsg, "%s  >>  Failed to update with NOWAIT flag on %s: %s\n",
			   time_date, record, masked_card_num);
   }
   else
   {
	   sprintf(errormsg, "%s  >>  Update Failed on %s: %s, %s\n",
			   time_date, record, masked_card_num, errorMsg);
   }
   fputs(errormsg,skipRecordPtr);
   fflush(skipRecordPtr);

} /* errorMsgNotInsert()*/

void mask_card_number_import(char * card_num, char *maskedCard)
{
	int cardlen = 0;
	int nIndex 	= 0;
	char masked_card_num[20];

	memset(masked_card_num ,0x00, sizeof(masked_card_num));
	cardlen = strlen( card_num );
	if (cardlen > 0)
	{
		/* Masking card number first 5 and last 4 in clear form remaining all masked. */
		strncpy( masked_card_num, card_num, cardlen);
		for ( nIndex = cardlen-5; nIndex >= cardlen-11; nIndex-- )
		{
			masked_card_num[nIndex] = '*';
		}
	}

	memcpy(&maskedCard[0], &masked_card_num[0], strlen(masked_card_num));
}

void log_the_updated_fields_of_mcf01(pMCF01 DB_mcf01, pMCF01 File_mcf01)
{
	CHAR infomsg[256],tempMsg[100] = {0};
	char temp_buff[30], time_date[25]={0};
	int i =0;
	char primary_key[35]={0};

	sprintf(primary_key,"%s-%s",DB_mcf01->primary_key.merchant_id,DB_mcf01->primary_key.organization_id);

	ptetime_get_timestamp( time_date );
	sprintf(tempMsg,"%s: ",time_date);
	fputs(tempMsg,HstMcF01Ptr);
	sprintf(infomsg,"Updating MCF01 with MID(%s)\n",DB_mcf01->primary_key.merchant_id);
	fputs(infomsg,HstMcF01Ptr);

	/* Only log updated details in HST01 MCF01 file*/
	ptetime_get_timestamp( time_date );
	sprintf(tempMsg,"%s: ",time_date);
	fputs(tempMsg,HstMcF01Ptr);
	sprintf(infomsg, "MID:%s |", DB_mcf01->primary_key.merchant_id);
	fputs(infomsg,HstMcF01Ptr);

	if( 0 != strncmp(DB_mcf01->status,File_mcf01->status,strlen(DB_mcf01->status)))
	{
		memset(infomsg,0x00,sizeof(infomsg));
		sprintf(infomsg, "DB status:%s | file status:%s |", DB_mcf01->status,File_mcf01->status);
		fputs(infomsg,HstMcF01Ptr);
	}

	if( 0 != strncmp(DB_mcf01->name01,File_mcf01->name01,strlen(DB_mcf01->name01)))
	{
		memset(infomsg,0x00,sizeof(infomsg));
		sprintf(infomsg, " DB name01:%s | file name01:%s |",  DB_mcf01->name01,File_mcf01->name01);
		fputs(infomsg,HstMcF01Ptr);
	}

	if( 0 != strncmp(DB_mcf01->name02,File_mcf01->name02,strlen(DB_mcf01->name02)))
	{
		memset(infomsg,0x00,sizeof(infomsg));
		sprintf(infomsg, "DB name02:%s | file name02:%s |",  DB_mcf01->name02,File_mcf01->name02);
		fputs(infomsg,HstMcF01Ptr);
	}

	if( 0 != strncmp(DB_mcf01->address01,File_mcf01->address01,strlen(DB_mcf01->address01)))
	{
		memset(infomsg,0x00,sizeof(infomsg));
		sprintf(infomsg, "DB address01:%s | file address01:%s |",   DB_mcf01->address01,File_mcf01->address01);
		fputs(infomsg,HstMcF01Ptr);
	}

	if( 0 != strncmp(DB_mcf01->address02,File_mcf01->address02,strlen(DB_mcf01->address02)))
	{
		memset(infomsg,0x00,sizeof(infomsg));
		sprintf(infomsg, "DB address02:%s | file address02:%s |",  DB_mcf01->address02,File_mcf01->address02);
		fputs(infomsg,HstMcF01Ptr);
	}

	if( 0 != strncmp(DB_mcf01->city,File_mcf01->city,strlen(DB_mcf01->city)))
	{
		memset(infomsg,0x00,sizeof(infomsg));
		sprintf(infomsg, "DB city:%s | file city:%s |",  DB_mcf01->city,File_mcf01->city);
		fputs(infomsg,HstMcF01Ptr);
	}

	if( 0 != strncmp(DB_mcf01->country,File_mcf01->country,strlen(DB_mcf01->country)))
	{
		memset(infomsg,0x00,sizeof(infomsg));
		sprintf(infomsg, "DB country:%s | file country:%s |",   DB_mcf01->country,File_mcf01->country);
		fputs(infomsg,HstMcF01Ptr);
	}

	if( 0 != strncmp(DB_mcf01->state,File_mcf01->state,strlen(DB_mcf01->state)))
	{
		memset(infomsg,0x00,sizeof(infomsg));
		sprintf(infomsg, "DB state:%s | file state:%s |",   DB_mcf01->state,File_mcf01->state);
		fputs(infomsg,HstMcF01Ptr);
	}

	if( 0 != strncmp(DB_mcf01->zip_code,File_mcf01->zip_code,strlen(DB_mcf01->zip_code)))
	{
		memset(infomsg,0x00,sizeof(infomsg));
		sprintf(infomsg, "DB zip_code:%s | file zip_code:%s |",    DB_mcf01->zip_code,File_mcf01->zip_code);
		fputs(infomsg,HstMcF01Ptr);
	}

	if( 0 != strncmp(DB_mcf01->intl_code,File_mcf01->intl_code,strlen(DB_mcf01->intl_code)))
	{
		memset(infomsg,0x00,sizeof(infomsg));
		sprintf(infomsg, "DB intl_code:%s | file intl_code:%s |",    DB_mcf01->intl_code,File_mcf01->intl_code);
		fputs(infomsg,HstMcF01Ptr);
	}

	if( 0 != strncmp(DB_mcf01->currency_code,File_mcf01->currency_code,strlen(DB_mcf01->currency_code)))
	{
		memset(infomsg,0x00,sizeof(infomsg));
		sprintf(infomsg, "DB currency_code:%s | file currency_code:%s |",    DB_mcf01->currency_code,File_mcf01->currency_code);
		fputs(infomsg,HstMcF01Ptr);
	}

	if( 0 != strncmp(DB_mcf01->contact,File_mcf01->contact,strlen(DB_mcf01->contact)))
	{
		memset(infomsg,0x00,sizeof(infomsg));
		sprintf(infomsg, "DB contact:%s | file contact:%s |",    DB_mcf01->contact,File_mcf01->contact);
		fputs(infomsg,HstMcF01Ptr);
	}

	if( 0 != strncmp(DB_mcf01->phone01,File_mcf01->phone01,strlen(DB_mcf01->phone01)))
	{
		memset(infomsg,0x00,sizeof(infomsg));
		sprintf(infomsg, "DB phone01:%s | file phone01:%s |",    DB_mcf01->phone01,File_mcf01->phone01);
		fputs(infomsg,HstMcF01Ptr);
	}

	if( 0 != strncmp(DB_mcf01->phone02,File_mcf01->phone02,strlen(DB_mcf01->phone02)))
	{
		memset(infomsg,0x00,sizeof(infomsg));
		sprintf(infomsg, "DB phone02:%s | file phone02:%s |",    DB_mcf01->phone02,File_mcf01->phone02);
		fputs(infomsg,HstMcF01Ptr);
	}

	if( 0 != strncmp(DB_mcf01->invoice_batch,File_mcf01->invoice_batch,strlen(DB_mcf01->invoice_batch)))
	{
		memset(infomsg,0x00,sizeof(infomsg));
		sprintf(infomsg, "DB invoice_batch:%s | file invoice_batch:%s |",    DB_mcf01->invoice_batch,File_mcf01->invoice_batch);
		fputs(infomsg,HstMcF01Ptr);
	}

	if( 0 != strncmp(DB_mcf01->invoice_subcode,File_mcf01->invoice_subcode,strlen(DB_mcf01->invoice_subcode)))
	{
		memset(infomsg,0x00,sizeof(infomsg));
		sprintf(infomsg, "DB invoice_subcode:%s | file invoice_subcode:%s |",    DB_mcf01->invoice_subcode,File_mcf01->invoice_subcode);
		fputs(infomsg,HstMcF01Ptr);
	}

	if( 0 != strncmp(DB_mcf01->process_control_id,File_mcf01->process_control_id,strlen(DB_mcf01->process_control_id)))
	{
		memset(infomsg,0x00,sizeof(infomsg));
		sprintf(infomsg, "DB process_control_id:%s | file process_control_id:%s |",    DB_mcf01->process_control_id,File_mcf01->process_control_id);
		fputs(infomsg,HstMcF01Ptr);
	}

	if( 0 != strncmp(DB_mcf01->date_added,File_mcf01->date_added,strlen(DB_mcf01->date_added)))
	{
		memset(infomsg,0x00,sizeof(infomsg));
		sprintf(infomsg, "DB date_added:%s | file date_added:%s |",    DB_mcf01->date_added,File_mcf01->date_added);
		fputs(infomsg,HstMcF01Ptr);
	}

	if( 0 != strncmp(DB_mcf01->region_nbr,File_mcf01->region_nbr,strlen(DB_mcf01->region_nbr)))
	{
		memset(infomsg,0x00,sizeof(infomsg));
		sprintf(infomsg, "DB region_nbr:%s | file region_nbr:%s |",    DB_mcf01->region_nbr,File_mcf01->region_nbr);
		fputs(infomsg,HstMcF01Ptr);
	}

	if( 0 != strncmp(DB_mcf01->division_nbr,File_mcf01->division_nbr,strlen(DB_mcf01->division_nbr)))
	{
		memset(infomsg,0x00,sizeof(infomsg));
		sprintf(infomsg, "DB division_nbr:%s | file division_nbr:%s |",    DB_mcf01->division_nbr,File_mcf01->division_nbr);
		fputs(infomsg,HstMcF01Ptr);
	}

	if( 0 != strncmp(DB_mcf01->password,File_mcf01->password,strlen(DB_mcf01->password)))
	{
		/*actual password postion is from 276-283 total 7 bytes, but its not a mandaotry field,
		 * therefore password parsing is commented and this password field is used to store th MC_MCO(merchant country of origin)
		 * */
		memset(infomsg,0x00,sizeof(infomsg));
		sprintf(infomsg, "DB MC MCO:%s | file MC MCO:%s |",    DB_mcf01->password,File_mcf01->password);
		fputs(infomsg,HstMcF01Ptr);
	}

	if( 0 != strncmp(DB_mcf01->rps_merchant,File_mcf01->rps_merchant,strlen(DB_mcf01->rps_merchant)))
	{
		memset(infomsg,0x00,sizeof(infomsg));
		sprintf(infomsg, "DB rps_merchant:%s | file rps_merchant:%s |",    DB_mcf01->rps_merchant,File_mcf01->rps_merchant);
		fputs(infomsg,HstMcF01Ptr);
	}

	if( 0 != strncmp(DB_mcf01->house_card_merchant,File_mcf01->house_card_merchant,strlen(DB_mcf01->house_card_merchant)))
	{
		memset(infomsg,0x00,sizeof(infomsg));
		sprintf(infomsg, "DB house_card_merchant:%s | file house_card_merchant:%s |",    DB_mcf01->house_card_merchant,File_mcf01->house_card_merchant);
		fputs(infomsg,HstMcF01Ptr);
	}

	if( 0 != strncmp(DB_mcf01->procurement_merchant,File_mcf01->procurement_merchant,strlen(DB_mcf01->procurement_merchant)))
	{
		memset(infomsg,0x00,sizeof(infomsg));
		sprintf(infomsg, "DB procurement_merchant:%s | file procurement_merchant:%s |",    DB_mcf01->procurement_merchant,File_mcf01->procurement_merchant);
		fputs(infomsg,HstMcF01Ptr);
	}

	if( 0 != strncmp(DB_mcf01->deferred_merchant,File_mcf01->deferred_merchant,strlen(DB_mcf01->deferred_merchant)))
	{
		memset(infomsg,0x00,sizeof(infomsg));
		sprintf(infomsg, "DB deferred_merchant:%s | file deferred_merchant:%s |",    DB_mcf01->deferred_merchant,File_mcf01->deferred_merchant);
		fputs(infomsg,HstMcF01Ptr);
	}

	if( 0 != strncmp(DB_mcf01->cash_bonus_eligible,File_mcf01->cash_bonus_eligible,strlen(DB_mcf01->cash_bonus_eligible)))
	{
		memset(infomsg,0x00,sizeof(infomsg));
		sprintf(infomsg, "DB cash_bonus_eligible:%s | file cash_bonus_eligible:%s |",    DB_mcf01->cash_bonus_eligible,File_mcf01->cash_bonus_eligible);
		fputs(infomsg,HstMcF01Ptr);
	}

	if( 0 != strncmp(DB_mcf01->vip_code,File_mcf01->vip_code,strlen(DB_mcf01->vip_code)))
	{
		memset(infomsg,0x00,sizeof(infomsg));
		sprintf(infomsg, "DB vip_code:%s | file vip_code:%s |",   DB_mcf01->vip_code,File_mcf01->vip_code);
		fputs(infomsg,HstMcF01Ptr);
	}

	if( 0 != strncmp(DB_mcf01->pos_code,File_mcf01->pos_code,strlen(DB_mcf01->pos_code)))
	{
		memset(infomsg,0x00,sizeof(infomsg));
		sprintf(infomsg, "DB pos_code(%s) | file pos_code(%s)",    DB_mcf01->pos_code,File_mcf01->pos_code);
		fputs(infomsg,HstMcF01Ptr);
	}

	if( 0 != strncmp(DB_mcf01->vs_tcc_code,File_mcf01->vs_tcc_code,strlen(DB_mcf01->vs_tcc_code)))
	{
		memset(infomsg,0x00,sizeof(infomsg));
		sprintf(infomsg, "DB vs_tcc_code(%s) | file vs_tcc_code(%s)",    DB_mcf01->vs_tcc_code,File_mcf01->vs_tcc_code);
		fputs(infomsg,HstMcF01Ptr);
	}

	if( 0 != strncmp(DB_mcf01->mc_fcc_code,File_mcf01->mc_fcc_code,strlen(DB_mcf01->mc_fcc_code)))
	{
		memset(infomsg,0x00,sizeof(infomsg));
		sprintf(infomsg, "DB mc_fcc_code(%s) | file mc_fcc_code(%s)",    DB_mcf01->mc_fcc_code,File_mcf01->mc_fcc_code);
		fputs(infomsg,HstMcF01Ptr);
	}

	if( 0 != strncmp(DB_mcf01->jcb_sub_code,File_mcf01->jcb_sub_code,strlen(DB_mcf01->jcb_sub_code)))
	{
		memset(infomsg,0x00,sizeof(infomsg));
		sprintf(infomsg, "DB mc_fcc_code(%s) | file mc_fcc_code(%s)",    DB_mcf01->mc_fcc_code,File_mcf01->mc_fcc_code);
		fputs(infomsg,HstMcF01Ptr);
	}

	if( 0 != strncmp(DB_mcf01->mall_code,File_mcf01->mall_code,strlen(DB_mcf01->mall_code)))
	{
		memset(infomsg,0x00,sizeof(infomsg));
		sprintf(infomsg, "DB mall_code(%s) | file mall_code(%s)", DB_mcf01->mall_code,File_mcf01->mall_code);
		fputs(infomsg,HstMcF01Ptr);
	}

	if( 0 != strncmp(DB_mcf01->hc_code,File_mcf01->hc_code,strlen(DB_mcf01->hc_code)))
	{
		memset(infomsg,0x00,sizeof(infomsg));
		sprintf(infomsg, "DB hc_code(%s) | file hc_code(%s)", DB_mcf01->hc_code,File_mcf01->hc_code);
		fputs(infomsg,HstMcF01Ptr);
	}

	if( 0 != strncmp(DB_mcf01->solicit_code,File_mcf01->solicit_code,strlen(DB_mcf01->solicit_code)))
	{
		memset(infomsg,0x00,sizeof(infomsg));
		sprintf(infomsg, "DB solicit_code:%s | file solicit_code:%s |", DB_mcf01->solicit_code,File_mcf01->solicit_code);
		fputs(infomsg,HstMcF01Ptr);
	}

	if( 0 != strncmp(DB_mcf01->block_start_date,File_mcf01->block_start_date,strlen(DB_mcf01->block_start_date)))
	{
		memset(infomsg,0x00,sizeof(infomsg));
		sprintf(infomsg, "DB block_start_date:%s | file block_start_date:%s |", DB_mcf01->block_start_date,File_mcf01->block_start_date);
		fputs(infomsg,HstMcF01Ptr);
	}

	if( 0 != strncmp(DB_mcf01->block_end_date,File_mcf01->block_end_date,strlen(DB_mcf01->block_end_date)))
	{
		memset(infomsg,0x00,sizeof(infomsg));
		sprintf(infomsg, "DB block_end_date:%s | file block_end_date:%s |", DB_mcf01->block_end_date,File_mcf01->block_end_date);
		fputs(infomsg,HstMcF01Ptr);
	}

	if( 0 != strncmp(DB_mcf01->credit_purch_max,File_mcf01->credit_purch_max,strlen(DB_mcf01->credit_purch_max)))
	{
		memset(infomsg,0x00,sizeof(infomsg));
		sprintf(infomsg, "DB credit_purch_max:%s | file credit_purch_max:%s |", DB_mcf01->credit_purch_max,File_mcf01->credit_purch_max);
		fputs(infomsg,HstMcF01Ptr);
	}

	if( 0 != strncmp(DB_mcf01->credit_auth_max,File_mcf01->credit_auth_max,strlen(DB_mcf01->credit_auth_max)))
	{
		memset(infomsg,0x00,sizeof(infomsg));
		sprintf(infomsg, "DB credit_auth_max:%s | file credit_auth_max:%s |", DB_mcf01->credit_auth_max,File_mcf01->credit_auth_max);
		fputs(infomsg,HstMcF01Ptr);
	}

	if( 0 != strncmp(DB_mcf01->credit_return_max,File_mcf01->credit_return_max,strlen(DB_mcf01->credit_return_max)))
	{
		memset(infomsg,0x00,sizeof(infomsg));
		sprintf(infomsg, "DB credit_return_max:%s | file credit_return_max:%s |", DB_mcf01->credit_return_max,File_mcf01->credit_return_max);
		fputs(infomsg,HstMcF01Ptr);
	}

	if( 0 != strncmp(DB_mcf01->credit_cash_adv_max,File_mcf01->credit_cash_adv_max,strlen(DB_mcf01->credit_cash_adv_max)))
	{
		memset(infomsg,0x00,sizeof(infomsg));
		sprintf(infomsg, "DB credit_cash_adv_max:%s | file credit_cash_adv_max:%s |", DB_mcf01->credit_cash_adv_max,File_mcf01->credit_cash_adv_max);
		fputs(infomsg,HstMcF01Ptr);
	}

	if( 0 != strncmp(DB_mcf01->credit_man_entry_max,File_mcf01->credit_man_entry_max,strlen(DB_mcf01->credit_man_entry_max)))
	{
		memset(infomsg,0x00,sizeof(infomsg));
		sprintf(infomsg, "DB credit_man_entry_max:%s | file credit_man_entry_max:%s |", DB_mcf01->credit_man_entry_max,File_mcf01->credit_man_entry_max);
		fputs(infomsg,HstMcF01Ptr);
	}

	if( 0 != strncmp(DB_mcf01->credit_cash_back_max,File_mcf01->credit_cash_back_max,strlen(DB_mcf01->credit_cash_back_max)))
	{
		memset(infomsg,0x00,sizeof(infomsg));
		sprintf(infomsg, "DB credit_cash_back_max:%s | file credit_cash_back_max:%s |", DB_mcf01->credit_cash_back_max,File_mcf01->credit_cash_back_max);
		fputs(infomsg,HstMcF01Ptr);
	}

	if( 0 != strncmp(DB_mcf01->debit_purch_max,File_mcf01->debit_purch_max,strlen(DB_mcf01->debit_purch_max)))
	{
		memset(infomsg,0x00,sizeof(infomsg));
		sprintf(infomsg, "DB debit_purch_max:%s | file debit_purch_max:%s |", DB_mcf01->debit_purch_max,File_mcf01->debit_purch_max);
		fputs(infomsg,HstMcF01Ptr);
	}

	if( 0 != strncmp(DB_mcf01->debit_auth_max,File_mcf01->debit_auth_max,strlen(DB_mcf01->debit_auth_max)))
	{
		memset(infomsg,0x00,sizeof(infomsg));
		sprintf(infomsg, "DB debit_auth_max:%s | file debit_auth_max:%s |",    DB_mcf01->debit_auth_max,File_mcf01->debit_auth_max);
		fputs(infomsg,HstMcF01Ptr);
	}

	if( 0 != strncmp(DB_mcf01->debit_return_max,File_mcf01->debit_return_max,strlen(DB_mcf01->debit_return_max)))
	{
		memset(infomsg,0x00,sizeof(infomsg));
		sprintf(infomsg, "DB debit_return_max:%s | file debit_return_max:%s |",    DB_mcf01->debit_return_max,File_mcf01->debit_return_max);
		fputs(infomsg,HstMcF01Ptr);
	}

	if( 0 != strncmp(DB_mcf01->debit_cash_adv_max,File_mcf01->debit_cash_adv_max,strlen(DB_mcf01->debit_cash_adv_max)))
	{
		memset(infomsg,0x00,sizeof(infomsg));
		sprintf(infomsg, "DB debit_cash_adv_max:%s | file debit_cash_adv_max:%s |",    DB_mcf01->debit_cash_adv_max,File_mcf01->debit_cash_adv_max);
		fputs(infomsg,HstMcF01Ptr);
	}

	if( 0 != strncmp(DB_mcf01->debit_man_entry_max,File_mcf01->debit_man_entry_max,strlen(DB_mcf01->debit_man_entry_max)))
	{
		memset(infomsg,0x00,sizeof(infomsg));
		sprintf(infomsg, "DB debit_man_entry_max:%s | file debit_man_entry_max:%s |",    DB_mcf01->debit_man_entry_max,File_mcf01->debit_man_entry_max);
		fputs(infomsg,HstMcF01Ptr);
	}

	if( 0 != strncmp(DB_mcf01->debit_cash_back_max,File_mcf01->debit_cash_back_max,strlen(DB_mcf01->debit_cash_back_max)))
	{
		memset(infomsg,0x00,sizeof(infomsg));
		sprintf(infomsg, "DB debit_cash_back_max:%s | file debit_cash_back_max:%s ",    DB_mcf01->debit_cash_back_max,File_mcf01->debit_cash_back_max);
		fputs(infomsg,HstMcF01Ptr);
	}

	if( 0 != strncmp(DB_mcf01->aver_purch_amount,File_mcf01->aver_purch_amount,strlen(DB_mcf01->aver_purch_amount)))
	{
		memset(infomsg,0x00,sizeof(infomsg));
		sprintf(infomsg, "DB aver_purch_amount:%s | file aver_purch_amount:%s |",    DB_mcf01->aver_purch_amount,File_mcf01->aver_purch_amount);
		fputs(infomsg,HstMcF01Ptr);
	}

	if( 0 != strncmp(DB_mcf01->max_num_returns,File_mcf01->max_num_returns,strlen(DB_mcf01->max_num_returns)))
	{
		memset(infomsg,0x00,sizeof(infomsg));
		sprintf(infomsg, "DB max_num_returns:%s | file max_num_returns:%s |",    DB_mcf01->max_num_returns,File_mcf01->max_num_returns);
		fputs(infomsg,HstMcF01Ptr);
	}

	if( 0 != strncmp(DB_mcf01->amount_returns,File_mcf01->amount_returns,strlen(DB_mcf01->amount_returns)))
	{
		memset(infomsg,0x00,sizeof(infomsg));
		sprintf(infomsg, "DB amount_returns:%s | file amount_returns:%s |",    DB_mcf01->amount_returns,File_mcf01->amount_returns);
		fputs(infomsg,HstMcF01Ptr);
	}

	if( 0 != strncmp(DB_mcf01->number_man_entries,File_mcf01->number_man_entries,strlen(DB_mcf01->number_man_entries)))
	{
		memset(infomsg,0x00,sizeof(infomsg));
		sprintf(infomsg, "DB number_man_entries:%s | file number_man_entries:%s |",    DB_mcf01->number_man_entries,File_mcf01->number_man_entries);
		fputs(infomsg,HstMcF01Ptr);
	}

	if( 0 != strncmp(DB_mcf01->amount_deposits,File_mcf01->amount_deposits,strlen(DB_mcf01->amount_deposits)))
	{
		memset(infomsg,0x00,sizeof(infomsg));
		sprintf(infomsg, "DB amount_deposits:%s | file amount_deposits:%s |",    DB_mcf01->amount_deposits,File_mcf01->amount_deposits);
		fputs(infomsg,HstMcF01Ptr);
	}

	if( 0 != strncmp(DB_mcf01->pan_same,File_mcf01->pan_same,strlen(DB_mcf01->pan_same)))
	{
		memset(infomsg,0x00,sizeof(infomsg));
		sprintf(infomsg, "DB pan_same:%s | file pan_same:%s |",    DB_mcf01->pan_same,File_mcf01->pan_same);
		fputs(infomsg,HstMcF01Ptr);
	}

	if( 0 != strncmp(DB_mcf01->totals_ind,File_mcf01->totals_ind,strlen(DB_mcf01->totals_ind)))
	{
		memset(infomsg,0x00,sizeof(infomsg));
		sprintf(infomsg, "DB totals_ind:%s | file totals_ind:%s |",    DB_mcf01->totals_ind,File_mcf01->totals_ind);
		fputs(infomsg,HstMcF01Ptr);
	}

	if( 0 != strncmp(DB_mcf01->statistics_ind,File_mcf01->statistics_ind,strlen(DB_mcf01->totals_ind)))
	{
		memset(infomsg,0x00,sizeof(infomsg));
		sprintf(infomsg, "DB statistics_ind:%s | file statistics_ind:%s |",    DB_mcf01->statistics_ind,File_mcf01->statistics_ind);
		fputs(infomsg,HstMcF01Ptr);
	}

	for(i=0;i<=9;i++)
	{
		if( 0 != strncmp(DB_mcf01->network[i].priority_routing,File_mcf01->network[i].priority_routing,strlen(DB_mcf01->network[i].priority_routing)))
		{
			memset(infomsg,0x00,sizeof(infomsg));
			sprintf(infomsg, "DB network.priority_routing[%d]:%s | file network.priority_routing[%d]:%s |", i ,DB_mcf01->network[i].priority_routing, i, File_mcf01->network[i].priority_routing);
			fputs(infomsg,HstMcF01Ptr);
		}
	}

	if( 0 != strncmp(DB_mcf01->negative_group_id,File_mcf01->negative_group_id,strlen(DB_mcf01->negative_group_id)))
	{
		memset(infomsg,0x00,sizeof(infomsg));
		sprintf(infomsg, "DB negative_group_id:%s | file negative_group_id:%s |", DB_mcf01->negative_group_id,File_mcf01->negative_group_id);
		fputs(infomsg,HstMcF01Ptr);
	}

	if( 0 != strncmp(DB_mcf01->voice_category_code,File_mcf01->voice_category_code,strlen(DB_mcf01->voice_category_code)))
	{
		memset(infomsg,0x00,sizeof(infomsg));
		sprintf(infomsg, "DB voice_category_code:%s | file voice_category_code:%s |", DB_mcf01->voice_category_code,File_mcf01->voice_category_code);
		fputs(infomsg,HstMcF01Ptr);
	}

	if( 0 != strncmp(DB_mcf01->class_code,File_mcf01->class_code,strlen(DB_mcf01->class_code)))
	{
		memset(infomsg,0x00,sizeof(infomsg));
		sprintf(infomsg, "DB class_code:%s | file class_code:%s |", DB_mcf01->class_code,File_mcf01->class_code);
		fputs(infomsg,HstMcF01Ptr);
	}

	if( 0 != strncmp(DB_mcf01->deferred_start_date,File_mcf01->deferred_start_date,strlen(DB_mcf01->deferred_start_date)))
	{
		memset(infomsg,0x00,sizeof(infomsg));
		sprintf(infomsg, "DB deferred_start_date:%s | file deferred_start_date:%s |", DB_mcf01->deferred_start_date,File_mcf01->deferred_start_date);
		fputs(infomsg,HstMcF01Ptr);
	}

	if( 0 != strncmp(DB_mcf01->deferred_end_date,File_mcf01->deferred_end_date,strlen(DB_mcf01->deferred_end_date)))
	{
		memset(infomsg,0x00,sizeof(infomsg));
		sprintf(infomsg, "DB deferred_end_date:%s | file deferred_end_date:%s |", DB_mcf01->deferred_end_date,File_mcf01->deferred_end_date);
		fputs(infomsg,HstMcF01Ptr);
	}

	if( 0 != strncmp(DB_mcf01->group_outlet_code,File_mcf01->group_outlet_code,strlen(DB_mcf01->group_outlet_code)))
	{
		memset(infomsg,0x00,sizeof(infomsg));
		sprintf(infomsg, "DB group_outlet_code:%s | file group_outlet_code:%s |", DB_mcf01->group_outlet_code,File_mcf01->group_outlet_code);
		fputs(infomsg,HstMcF01Ptr);
	}

	if( 0 != strncmp(DB_mcf01->group_type,File_mcf01->group_type,strlen(DB_mcf01->group_type)))
	{
		memset(infomsg,0x00,sizeof(infomsg));
		sprintf(infomsg, "DB group_type:%s | file group_type:%s |", DB_mcf01->group_type,File_mcf01->group_type);
		fputs(infomsg,HstMcF01Ptr);
	}

	if( 0 != strncmp(DB_mcf01->reload_merchant,File_mcf01->reload_merchant,strlen(DB_mcf01->reload_merchant)))
	{
		memset(infomsg,0x00,sizeof(infomsg));
		sprintf(infomsg, "DB reload_merchant:%s | file reload_merchant:%s |", DB_mcf01->reload_merchant,File_mcf01->reload_merchant);
		fputs(infomsg,HstMcF01Ptr);
	}

	fputs("\n",HstMcF01Ptr);

}

/******************************************************************************
 *
 *  NAME:         GET_TIME
 *
 *  DESCRIPTION:  This procedure takes an input string that contains a date and
 *                time.  It copies the time portion into an output string,
 *                formatted slightly different.
 *
 *  INPUTS:       time_date : Format = "YYYY-0M-0D-0H.0I.0S.JJJ"
 *
 *  OUTPUTS:      time_str  : Format = "0H0I0S"
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
void get_time( pCHAR time_date, pCHAR time_str )
{
   memset( time_str,  0x00,          9 );
   memcpy( time_str, &time_date[11], 8 );
}

/******************************************************************************
 *
 *  NAME:         GET_DATE
 *
 *  DESCRIPTION:  This procedure takes an input string that contains a date and
 *                time.  It copies the date portion into an output string, 
 *                formatted slightly different.
 *
 *  AUTHOR:       Dennis Irby
 *
 *  INPUTS:       time_date : Format = "YYYY-0M-0D-0H.0I.0S.JJJ"
 *  OUTPUTS:      date_str  : Format = "YYYY-0M-0D"
 *  RTRN VALUE:   None
 *
 *  MODIFICATIONS:
 ******************************************************************************/
void get_date( pCHAR time_date, pCHAR date_str ) 
{
   memset( date_str,  0x00,        11 ); 
   memcpy( date_str,  time_date,   10 ); 
}


/******************************************************************************
 *
 *  NAME:         exit_import()
 *
 *  DESCRIPTION:  This procedure completes some cleanup tasks before calling exit(0);
 *                set rollback segment for import to offline and disconnect from the  
 *                database
 *
 *  AUTHOR:       Linda Mitchell
 *
 *  INPUTS:       None
 *  OUTPUTS:      None
 *  RTRN VALUE:   None
 *
 *  MODIFICATIONS:
 ******************************************************************************/
void exit_import() 
{
	 CHAR  tempMsg[100]={0};

	/*Call to set rollback segment RBS37 to offline state - SCR935 - LM */
	if (db_set_rollback_segment_offline( tempMsg) )
	{
		sprintf(tempMsg, "Unable to set rollback RBS37 to offline" );
		fputs(tempMsg, dumpPtr);
		LogMsg(tempMsg, INFO_MSG);
	}

	/*disconnect from database*/
	dbcommon_disconnect (tempMsg);
	
	exit(0);
}

/******************************************************************************
 *
 *  NAME:         get_term_ini_info()
 *
 *  DESCRIPTION:  This function reads the tf.ini file to retrieve the configuration 
 *				  parameters RECORD_LENGTH and KEY_LENGTH values.  If not present, 
 *				  default record_length is 150 and default key_length = 16.  These two fields 
 *			      are used to determine the record size and key length in the terminal detail 
 *			      record.  Key values can be 16, 32, or 48.  Record length values could be 150,
 *				  166, or 182.
 *
 *  AUTHOR:       Linda Mitchell - 02/24/03
 *
 *  INPUTS:       None
 *  OUTPUTS:      This function logs error message to the log file on receiving error.
 *  RTRN VALUE:   None
 *
 *  MODIFICATIONS:
 ******************************************************************************/
void get_term_ini_info()
{
   INT	 result = 0;
   CHAR  temp_str[200];
   CHAR  tempMsg[200]={0};

   memset(temp_str, 0x00, sizeof(temp_str));

   GetAscendentConfigDirectory( ini_filename );
   strcat( ini_filename, "tf.ini" );

   /*get record length from ini file*/
   result =  GetPrivateProfileString(
                       "TERMINAL_IMPORT",   // points to section name
                       "RECORD_LENGTH",     // points to key name 
                       "",                  // points to default string 
                       temp_str,            // points to destination buffer 
                       sizeof(temp_str) - 1,// size of destination buffer 
                       ini_filename );      // points to initialization filename

   if(result == 0) /*failed to retrieve record length*/
   {
      TermRecLength = atoi(DEFAULT_RECORD_LENGTH);
      sprintf( tempMsg,
              "IMPORT: Error retrieving Record Length from tf.ini: Using default value %s ",
               DEFAULT_RECORD_LENGTH );
      LogMsg(tempMsg, INFO_MSG);
   }
   else
   {
      temp_str[sizeof(temp_str)] = 0;    //Make sure string is terminated
      TermRecLength = atoi(temp_str); 
   }

   if(TermRecLength > MAX_RECORD_LENGTH)
   {
      TermRecLength = atoi(DEFAULT_RECORD_LENGTH);
      sprintf( tempMsg,
              "IMPORT: Record Length from tf.ini > 182: Using default value %s ",
               DEFAULT_RECORD_LENGTH );
      LogMsg(tempMsg, INFO_MSG);
   }

   memset(temp_str, 0x00, sizeof(temp_str));

   /*get key length from ini file*/
   result =  GetPrivateProfileString(
                     "TERMINAL_IMPORT",     // points to section name 
                     "KEY_LENGTH",          // points to key name 
                     "",                    // points to default string 
                     temp_str,              // points to destination buffer 
                     sizeof(temp_str) - 1,  // size of destination buffer 
                     ini_filename );        // points to initialization filename

   if(result == 0)  /*failed to retrieve key lenght*/
   {
      TermKeyLength = atoi(DEFAULT_KEY_LENGTH);
      sprintf( tempMsg,
              "IMPORT: Error retrieving Key Length from tf.ini: Using default value %s ",
               DEFAULT_KEY_LENGTH );
      LogMsg(tempMsg, INFO_MSG);
   }
   else
   {
      temp_str[sizeof(temp_str)] = 0;    //Make sure string is terminated
      TermKeyLength = atoi(temp_str);
   }

   if(TermKeyLength > MAX_KEY_LENGTH)
   {
      TermKeyLength = atoi(DEFAULT_KEY_LENGTH);
      sprintf( tempMsg,
              "IMPORT: Key Length from tf.ini > 48: Using default value %s ",
               DEFAULT_KEY_LENGTH );
      LogMsg(tempMsg, INFO_MSG);
   }

   return;
}

/******************************************************************************
 *
 * MODULE:	    LOG_DB_TIMING_STATISTIC_REPORT
 *
 * DESCRIPTION: This function logs the TIMINGS_LIST structure to the
 *              system log.  Each log event contains the statistics for
 *              a single query, such as inserts into TLF01 or selects
 *              from DCF01.
 *
 *              This function is exactly the same as the one in the ATP
 *              library except it sends its output to the Import Log file.
 *
 * AUTHOR:      D. Irby
 *
 * INPUTS:      timing_list - structure holding the timing statistics
 *
 * OUTPUTS:     None
 *
 * RTRN VALUE:  Number of queries logged to the system log
 *
 ******************************************************************************/
INT log_db_timing_statistics_report( pTIMINGS_LIST  timing_list )
{
   INT  i;
   INT  st1_flag;
   CHAR st1[20];
   CHAR tmpstr[150];


   for( i=0; i<timing_list->num_entries; i++ )
   {
      /* Make sure this is a valid entry. */
      if ( (timing_list->st_timings[i].table)-100 <= NUM_DB_TIMING_TABLES )
      {
         /* Get text for subtype 1. */
         st1_flag = true;
         memset( st1, 0x00, sizeof(st1) );
         switch( timing_list->st_timings[i].sql_type )
         {
            case ST1_DB_SELECT:            strcpy( st1, "SELECT"            ); break;
            case ST1_DB_INSERT:            strcpy( st1, "INSERT"            ); break;
            case ST1_DB_UPDATE:            strcpy( st1, "UPDATE"            ); break;
            case ST1_DB_DELETE:            strcpy( st1, "DELETE"            ); break;
            case ST1_DB_GET_GUI_LIST:      strcpy( st1, "GET GUI LIST"      ); break;
            case ST1_DB_GET_SERVICE_LIST:  strcpy( st1, "GET SERVICE LIST"  ); break;
            case ST1_DB_ECHO:              strcpy( st1, "ECHO"              ); break;
            case ST1_DB_GET_COUNT:         strcpy( st1, "GET COUNT"         ); break;
            case ST1_DB_MOVE_TRANS:        strcpy( st1, "MOVE TRANS"        ); break;
            case ST1_DB_CHECK_BIN_RANGE:   strcpy( st1, "GET BIN RANGE"     ); break;
            case ST1_DB_GET_SETTLE_TOTALS: strcpy( st1, "GET SETTLE TOTALS" ); break;
            case ST1_DB_GET_ALL:           strcpy( st1, "GET ALL"           ); break;
            case ST1_DB_SEL_FOR_UPD:       strcpy( st1, "SELECT FOR UPDATE" ); break;
            case ST1_DB_SELECT_AUTH_ID:    strcpy( st1, "SELECT AUTH ID"    ); break;
            case ST1_DB_CHECK_BIN02_RANGE: strcpy( st1, "GET BIN02 RANGE"   ); break;
            default:  st1_flag = false;
         }

         if ( st1_flag == true )
         {
            /* Yes it is a valid table and subtype1. */
            memset( tmpstr, 0x00, sizeof(tmpstr) );
            sprintf( tmpstr,
                    "STATS >> Table %s, SQL %s, type %d, Hits %d, Avg %.2f, Max %.2f",
                     DB_Table_Defs[(timing_list->st_timings[i].table)-100].name,
                     st1,
                     timing_list->st_timings[i].subtype2,
                     timing_list->st_timings[i].num_queries,
                     timing_list->st_timings[i].avg_duration,
                     timing_list->st_timings[i].max_duration );

            LogMsg(tmpstr, INFO_MSG);
         }
      }
   }
   return( timing_list->num_entries );
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


BOOLEAN OpenDSV_WriteHST01(HST01 *hst01)
{

	char masked_field_Previous[151]={0};
		char masked_field_Current[151]={0};
		CHAR  time_date[25] = "";
		CHAR  current_mmdd[5]={0};
		INT   len=0;

		ptetime_get_timestamp( time_date );

	    memcpy( current_mmdd,   time_date+5, 2 );
	    memcpy( current_mmdd+2, time_date+8, 2 );

	    len = strlen(HST01_Filename);
	    if ( 0 != strncmp(&HST01_Filename[len-8], current_mmdd,4) )
	    {

	    	import_create_Error_Filename();
	    }

		    HstFilePtr = fopen(HST01_Filename,"a");
			if(HstFilePtr != NULL)
			{
				long int File_Size = findHST01FileSize(HST01_Filename);
				if(File_Size >= HEADER_SIZE)
				{


					WriteString(&hst01->primary_key.unique_id, sizeof(hst01->primary_key.unique_id), FILLERCHAR, SEPERATORCHAR, &HstFilePtr, "unique_id");
					WriteString(&hst01->file_name, sizeof(hst01->file_name), FILLERCHAR, SEPERATORCHAR, &HstFilePtr, "file_name");
					WriteString(&hst01->changed_by, sizeof(hst01->changed_by), FILLERCHAR, SEPERATORCHAR, &HstFilePtr, "changed_by");
					WriteString(&hst01->date_modified, sizeof(hst01->date_modified), FILLERCHAR, SEPERATORCHAR, &HstFilePtr, "date_modified");
					WriteString(&hst01->time_modified, sizeof(hst01->time_modified), FILLERCHAR, SEPERATORCHAR, &HstFilePtr, "time_modified");
					WriteString(&hst01->file_primary_key, sizeof(hst01->file_primary_key), FILLERCHAR, SEPERATORCHAR, &HstFilePtr, "file_primary_key");
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

		return TRUE;

}


/*BOOLEAN CreateDSV(char *filename)
{
		HstFilePtr = fopen(filename,"rb");
	if(HstFilePtr != NULL)
	{
		fclose(HstFilePtr);
		HstFilePtr = NULL;
		printf("The DSV file %s already exists!. Do you want to overwrite? [Y/N]:");
		log
		return false;
	}
	HstFilePtr = fopen(filename, "wb");
	if(HstFilePtr == NULL)
	{
		printf("Unable to create the file.\n");
		log
		return false;
	}
	return WriteDSVHeader();
}*/

/*BOOLEAN OpenDSV_WriteHST01()
{
		HstFilePtr = fopen(HST01_Filename,"rb");
	if(HstFilePtr != NULL)
	{
		fclose(HstFilePtr);
		HstFilePtr = NULL;
		printf("The DSV file %s already exists!. Do you want to overwrite? [Y/N]:");
		log
		return false;
	}
	HstFilePtr = fopen(filename, "wb");
	if(HstFilePtr == NULL)
	{
		printf("Unable to create the file.\n");
		log
		return false;
	}
	else
	{
		WriteHST01TableIntoDSV(&hst01);
	}
}*/

void WriteDSVHeader()
{
	HST01 pte_hst01;


	if(HstFilePtr != NULL)
	{
		WriteString("unique_id", sizeof(pte_hst01.primary_key.unique_id), FILLERCHAR, SEPERATORCHAR, &HstFilePtr, "unique_id");					//1
		WriteString("file_name", sizeof(pte_hst01.file_name), FILLERCHAR, SEPERATORCHAR, &HstFilePtr, "file_name");							//2
		WriteString("changed_by", sizeof(pte_hst01.changed_by), FILLERCHAR, SEPERATORCHAR, &HstFilePtr, "changed_by");						//5
		WriteString("date_modified", 13, FILLERCHAR, SEPERATORCHAR, &HstFilePtr, "date_modified");						//7
		WriteString("time_modified", 13, FILLERCHAR, SEPERATORCHAR, &HstFilePtr, "time_modified");						//9
		WriteString("file_primary_key", sizeof(pte_hst01.file_primary_key), FILLERCHAR, SEPERATORCHAR, &HstFilePtr, "file_primary_key");						//10
		WriteString("field_tag", sizeof(pte_hst01.field_tag), FILLERCHAR, SEPERATORCHAR, &HstFilePtr, "field_tag");							//11
		WriteString("previous_data", 150, FILLERCHAR, SEPERATORCHAR, &HstFilePtr, "previous_data");							//12
		WriteString("current_data", 150, FILLERCHAR, SEPERATORCHAR, &HstFilePtr, "current_data");						//13
		WriteString("reserved", sizeof(pte_hst01.reserved), FILLERCHAR, SEPERATORCHAR, &HstFilePtr, "reserved");
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

BOOLEAN WriteString(char *str, int size, char fillerChar, char seperator, FILE** fptr, char *header)
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
		LogMsg(buffer, INFO_MSG);
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

	if(fptr != NULL && *fptr != NULL)
	{
		fprint_retval = fprintf(*fptr, "%s", line);
	}
	else
	{
		printf("%s\n",line);
	}

	if(fprint_retval < 0)
	{
		sprintf( buffer,"File write failed, ret value= %d,  Field: %s -> %s",fprint_retval,header, str);
		LogMsg(buffer, ERROR_MSG);
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
void import_create_Error_Filename()
{
	CHAR  system_date[25] = {0};

/* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
	ptetime_get_timestamp( system_date );

	strcpy(HST01_Filename,import_hst01_file_path);
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
								   import_hst01_file_path,              	/* points to destination buffer   */
								   sizeof(import_hst01_file_path)-1,   	 /* size of destination buffer     */
								   filename                  /* points to ini filename         */
						 	   );

   if((strlen(import_hst01_file_path) > 0))
   {
	   import_create_Error_Filename();
   }
}

void import_HSTMCF01_create_Error_Filename()
{
	CHAR  system_date[25] = {0};

/* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
	ptetime_get_timestamp( system_date );

	strcpy(HST01MCF01_Filename,import_hst01mcf01_file_path);
	strcat( HST01MCF01_Filename, "HST01MCF01" );
	strcat( HST01MCF01_Filename, "_" );
	strncat(HST01MCF01_Filename, system_date,   4 );  /* YYYY */
	strncat(HST01MCF01_Filename, system_date+5, 2 );  /* MM   */
	strncat(HST01MCF01_Filename, system_date+8, 2 );  /* DD   */
	strcat(HST01MCF01_Filename, ".txt");  /* DD   */

}

void get_HST01_MCF01_file_name_path(void )
{
   DWORD rc;
   CHAR  filename   [80] = {0};
   CHAR  tmpstr     [80] = {0};
   CHAR  appname[512] = {0};


   /* Get path to the tf.ini file. */
   memset( tmpstr, 0x00, sizeof(tmpstr) );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, "tf.ini" );

   GetAppName (appname) ;

   /*Read path for creating file in order to log db statics and db oracle error messages */
   rc = GetPrivateProfileString(
								  "HST01",             /* points to section name         */
								  "HST01MCF01_LOGGING_PATH",       	/* points to key name             */
								   "",  					/* Default string                 */
								   import_hst01mcf01_file_path,              	/* points to destination buffer   */
								   sizeof(import_hst01mcf01_file_path)-1,   	 /* size of destination buffer     */
								   filename                  /* points to ini filename         */
						 	   );

   if((strlen(import_hst01mcf01_file_path) > 0))
   {
	   import_HSTMCF01_create_Error_Filename();
   }
}

void Mask_sensitive_fields(pHST01 hst01, CHAR * masked_field_Previous, CHAR *masked_field_Current)
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

