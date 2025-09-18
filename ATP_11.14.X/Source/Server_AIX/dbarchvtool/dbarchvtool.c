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
#include "dbarchvtool.h"
#include "ntutils.h"

/********************************/

/* Global variables declaration */

CHAR p_archive_start_date[9]={0};
CHAR p_archive_end_date[9]={0};
INT look_back_days=0;
CHAR p_batch_status[2]="C";
CHAR  batch_number[7]={0};
CHAR lookBack_days_str[5]={0};

BYTE DB_MsgSubtype2 ;
BYTE DB_MsgSubtype1 ;
CHAR DB_Logging_Filename[256];
CHAR  DB_module_file_name[256];
CHAR  DB_file_path[256];
INT DB_NullCheck_Flag;

CHAR Version[] = "ATP_11.1.0";
CHAR dbarchvtool_error_warning_file_path[256] = {0};
CHAR dbarchvtool_Erro_warning_Filename[256] = {0};
CHAR appname[512]  = "dbarchvtool";
CHAR Error_Buf[1024]={0};
CHAR Buffer[1024]={0};
CHAR BackuptableName[30] = {0};
CHAR BackuptableName_BCH10[30]={0};
CHAR ErrorMsg[1024]	={0};
CHAR    oprmon_que_name[];
TABLES_LIST Tables_List[MAX_TABLES] = {0};

/********************************/
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

void EndProcess()
{
	char      ErrorMsg[512] = {0};
	sprintf(ErrorMsg," DB Archive Tool version: %s Exiting\n",Version);
    printf("%s\n",ErrorMsg);
    write_into_log_file(ErrorMsg);
	dbarchvtool_disconnect(ErrorMsg);
	printf("Database disconnected successfully\n");
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
INT get_inputfrom_inifilefor_TLF01_STIP()
{
	DWORD rc=0;
	CHAR sectionname  [MAX_APPNAME_SIZE] = {0};
	CHAR filename     [MAX_FILENAME_SIZE]= {0};
	CHAR tmpstr       [MAX_FILENAME_SIZE]= {0};
	CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
	INT  ret_val = true;

	/* Determine directory and filename of the .ini file. */
	memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
	GetPinnacleConfigDirectory(tmpstr);
	sprintf(filename, "%s%s", tmpstr, DBARCHVTOOL_INI);

   memset(ini_info, 0x00, MAX_INI_INFO_SIZE);
   rc = GetPrivateProfileString(
	   "TLF01_STIP",                   /* points to section name       */
	   "TABLE_BACKUP",              /* points to key name           */
	   "TLF01_STIP_backup",            /* points to default string     */
	   ini_info,                      /* points to destination Error_Buf */
	   MAX_INI_INFO_SIZE - 1,         /* size of destination Error_Buf   */
	   filename                       /* points to ini filename       */
	   );
    strncpy(BackuptableName, ini_info, strlen(ini_info));

	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
	rc = GetPrivateProfileString(
			"TLF01_STIP",                   /* points to section name       */
			"START_DATE_YYYYMMDD",  /* points to key name           */
			"",                       /* points to default string     */
			 ini_info,                      /* points to destination Error_Buf */
			 MAX_INI_INFO_SIZE - 1,         /* size of destination Error_Buf   */
			 filename                       /* points to ini filename       */
	);
	strncpy( p_archive_start_date, ini_info, sizeof(p_archive_start_date)-1 );

	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
	rc = GetPrivateProfileString(
			"TLF01_STIP",                   /* points to section name       */
			"END_DATE_YYYYMMDD",  			/* points to key name           */
			"",                       		/* points to default string     */
			 ini_info,                      /* points to destination Error_Buf */
			 MAX_INI_INFO_SIZE - 1,         /* size of destination Error_Buf   */
			 filename                       /* points to ini filename       */
	);
	strncpy( p_archive_end_date, ini_info, sizeof(p_archive_end_date)-1 );

	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
	rc = GetPrivateProfileString(
			"TLF01_STIP",                   /* points to section name       */
			"LOOK_BACK_DAYS",  /* points to key name           */
			"",                       /* points to default string     */
			 ini_info,                      /* points to destination Error_Buf */
			 MAX_INI_INFO_SIZE - 1,         /* size of destination Error_Buf   */
			 filename                       /* points to ini filename       */
	);
	/*Validate the lookback days*/
	if(false == isnum(ini_info))
	{
		sprintf(Error_Buf," Invalid Look back day entered, check the ini file.\n");
		write_into_log_file(ErrorMsg);
		printf("\n%s\n",Error_Buf);
		return false;
	}
	look_back_days = atoi(ini_info);

	return rc;


}
INT get_inputfrom_inifilefor_BCH01_MP()
{
	DWORD rc=0;
	CHAR sectionname  [MAX_APPNAME_SIZE] = {0};
	CHAR filename     [MAX_FILENAME_SIZE]= {0};
	CHAR tmpstr       [MAX_FILENAME_SIZE]= {0};
	CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
	INT  ret_val = true;

	/* Determine directory and filename of the .ini file. */
	memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
	GetPinnacleConfigDirectory(tmpstr);
	sprintf(filename, "%s%s", tmpstr, DBARCHVTOOL_INI);

   memset(ini_info, 0x00, MAX_INI_INFO_SIZE);
   rc = GetPrivateProfileString(
	   "BCH01_MP",                   /* points to section name       */
	   "TABLE_BACKUP",              /* points to key name           */
	   "BCH01_MP_backup",            /* points to default string     */
	   ini_info,                      /* points to destination Error_Buf */
	   MAX_INI_INFO_SIZE - 1,         /* size of destination Error_Buf   */
	   filename                       /* points to ini filename       */
	   );
    strncpy(BackuptableName, ini_info, strlen(ini_info));

    memset(ini_info, 0x00, MAX_INI_INFO_SIZE);
       rc = GetPrivateProfileString(
    	   "BCH01_MP",                   /* points to section name       */
    	   "TABLE_BACKUP_BCH10",              /* points to key name           */
    	   "BCH10_backup",            /* points to default string     */
    	   ini_info,                      /* points to destination Error_Buf */
    	   MAX_INI_INFO_SIZE - 1,         /* size of destination Error_Buf   */
    	   filename                       /* points to ini filename       */
    	   );
        strncpy(BackuptableName_BCH10, ini_info, strlen(ini_info));

	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
	rc = GetPrivateProfileString(
			"BCH01_MP",                   /* points to section name       */
			"START_RELEASE_DATE",  /* points to key name           */
			"",                       /* points to default string     */
			 ini_info,                      /* points to destination Error_Buf */
			 MAX_INI_INFO_SIZE - 1,         /* size of destination Error_Buf   */
			 filename                       /* points to ini filename       */
	);
	strncpy( p_archive_start_date, ini_info, sizeof(p_archive_start_date)-1 );

	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
	rc = GetPrivateProfileString(
			"BCH01_MP",                   /* points to section name       */
			"END_RELEASE_DATE",  			/* points to key name           */
			"",                       		/* points to default string     */
			 ini_info,                      /* points to destination Error_Buf */
			 MAX_INI_INFO_SIZE - 1,         /* size of destination Error_Buf   */
			 filename                       /* points to ini filename       */
	);
	strncpy( p_archive_end_date, ini_info, sizeof(p_archive_end_date)-1 );

	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
	rc = GetPrivateProfileString(
			"BCH01_MP",                   /* points to section name       */
			"LOOK_BACK_DAYS",  /* points to key name           */
			"",                       /* points to default string     */
			 ini_info,                      /* points to destination Error_Buf */
			 MAX_INI_INFO_SIZE - 1,         /* size of destination Error_Buf   */
			 filename                       /* points to ini filename       */
	);
	/*Look back days validation*/
	if(false == isnum(ini_info))
			{
				sprintf(Error_Buf," Invalid Look back day entered, check the ini file. Exiting..\n");
				write_into_log_file(ErrorMsg);
				printf("\n%s\n",Error_Buf);
				return false;
			}

	look_back_days = atoi(ini_info);

	return rc;

}
INT get_inputfrom_inifilefor_BCH10()
{
	DWORD rc=0;
	CHAR sectionname  [MAX_APPNAME_SIZE] = {0};
	CHAR filename     [MAX_FILENAME_SIZE]= {0};
	CHAR tmpstr       [MAX_FILENAME_SIZE]= {0};
	CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
	INT  ret_val = true;

	/* Determine directory and filename of the .ini file. */
	memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
	GetPinnacleConfigDirectory(tmpstr);
	sprintf(filename, "%s%s", tmpstr, DBARCHVTOOL_INI);

   memset(ini_info, 0x00, MAX_INI_INFO_SIZE);
   rc = GetPrivateProfileString(
	   "BCH10",                   /* points to section name       */
	   "TABLE_BACKUP",              /* points to key name           */
	   "BCH10_bckupTable",            /* points to default string     */
	   ini_info,                      /* points to destination Error_Buf */
	   MAX_INI_INFO_SIZE - 1,         /* size of destination Error_Buf   */
	   filename                       /* points to ini filename       */
	   );
    strncpy(BackuptableName, ini_info, strlen(ini_info));

	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
	rc = GetPrivateProfileString(
			"BCH10",                   /* points to section name       */
			"START_TRAN_DATE",  /* points to key name           */
			"",                       /* points to default string     */
			 ini_info,                      /* points to destination Error_Buf */
			 MAX_INI_INFO_SIZE - 1,         /* size of destination Error_Buf   */
			 filename                       /* points to ini filename       */
	);
	strncpy( p_archive_start_date, ini_info, sizeof(p_archive_start_date)-1 );

	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
	rc = GetPrivateProfileString(
			"BCH10",                   /* points to section name       */
			"END_TRAN_DATE",  			/* points to key name           */
			"",                       		/* points to default string     */
			 ini_info,                      /* points to destination Error_Buf */
			 MAX_INI_INFO_SIZE - 1,         /* size of destination Error_Buf   */
			 filename                       /* points to ini filename       */
	);
	strncpy( p_archive_end_date, ini_info, sizeof(p_archive_end_date)-1 );

	memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
	rc = GetPrivateProfileString(
			"BCH10",                   /* points to section name       */
			"LOOK_BACK_DAYS",  /* points to key name           */
			"",                       /* points to default string     */
			 ini_info,                      /* points to destination Error_Buf */
			 MAX_INI_INFO_SIZE - 1,         /* size of destination Error_Buf   */
			 filename                       /* points to ini filename       */
	);
	if(false == isnum(ini_info))
	{
		sprintf(Error_Buf," Invalid Look back day entered, check the ini file. Exiting..\n");
		write_into_log_file(ErrorMsg);
		printf("\n%s\n",Error_Buf);
		return false;
	}
	look_back_days = atoi(ini_info);


	return rc;

}

void write_into_log_file(pCHAR Error_Buf)
{
	INT   len=0;
	CHAR  time_date[25]  ={0};
	CHAR  current_mmdd[5]={0};
	FILE *fp;

	/* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
	ptetime_get_timestamp( time_date );

	/* Pick out the month and day to compare to filename extenstion. */
	memcpy( current_mmdd,   time_date+5, 2 );
	memcpy( current_mmdd+2, time_date+8, 2 );
	if((fp = fopen(dbarchvtool_Erro_warning_Filename,"a+b"))==NULL)
	{
		LogEvent(Error_Buf,INFO_MSG);
		return 0;
	}
	strcpy(Buffer,time_date);
	strcat(Buffer,":");
	strcat(Buffer,Error_Buf);
	strcat(Buffer," ");
	len=strlen(Buffer);
	if(fwrite(Buffer, len, NUM_SIZE_WRITES, fp)==NUM_SIZE_WRITES)
	{
		// Do nothing fall below
	}
	else
	{
		LogEvent(Error_Buf,INFO_MSG);
		fclose(fp);
		return  0;
	}
	fclose(fp);
	return  0;
}


void dbarchvtool_create_Error_Filename(  )
{
	CHAR  timestamp[25] = {0};
   // Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj"
	ptetime_get_timestamp( timestamp );

    strcpy( dbarchvtool_Erro_warning_Filename, dbarchvtool_error_warning_file_path );
    strcat( dbarchvtool_Erro_warning_Filename, appname);
	strcat( dbarchvtool_Erro_warning_Filename, "_log." );
    strcat( dbarchvtool_Erro_warning_Filename, timestamp);
}
void dbarchvtool_create_log_File( )
{
		DWORD rc;
	   CHAR  filename   [80] = {0};
	   CHAR  tmpstr     [80] = {0};
	   CHAR  dbarchvtool_error_warning_file_size[5] = {0};

	   // Get path to the tf.ini file.
	   memset( tmpstr, 0x00, sizeof(tmpstr) );
	   GetPinnacleConfigDirectory(tmpstr);
	   sprintf(filename, "%s%s", tmpstr, "tf.ini" );

	   //Read path for creating file in order to log db statics and db oracle error messages
	   rc = GetPrivateProfileString( "DATASERVERS",
									  "DB_STATICS_PATH",
									   "",
									   dbarchvtool_error_warning_file_path,
									   sizeof(dbarchvtool_error_warning_file_path)-1,
									   filename	);

	   dbarchvtool_create_Error_Filename();
}

INT main()
{

	INT   rc 			= 0;
	INT   DB_rc 		= 0;
	CHAR  ch			= 0;
	CHAR  instr[20]     = {0};
	INT   entry 		= INVALID;
	INT   switch_entry  = 0;
	INT   user_input	= 0;
	INT   Table_Count	= 0;
    CHAR  filename[80] 	= {0};
    CHAR  tmpstr[80] 	= {0};
    INT   index			= 0;
    CHAR  input[10] 	= {0};

    /*create log file*/
    dbarchvtool_create_log_File();
	   /* Get path to the tf.ini file*/
   memset( tmpstr, 0x00, sizeof(tmpstr) );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, DBARCHVTOOL_INI );

	memset( tmpstr, 0x00, sizeof(tmpstr) );

	sprintf(Error_Buf,"Started DB Archive Tool version: %s\n",Version);
	printf("\n%s\n",Error_Buf);
	write_into_log_file(Error_Buf);

	memset(Tables_List,0x00,sizeof(Tables_List));
	Table_Count = GetPrivateProfileList( filename,
			TABLE_LIST_SECTION,
			(pCHAR)&Tables_List, sizeof(Tables_List[0].table_name));

	if (dbarchvtool_connect("equitable", "equitable", "equitable", "ORCL", ErrorMsg) == 0)
	{
		sprintf(Error_Buf," Database connected successfully!\n");
		write_into_log_file(Error_Buf);

		while(entry == INVALID)
		{
			printf( "\nTables can be archived are:\n");
			/*
			 * print the table list
			 * */
			for(index =0 ; index < Table_Count ;index++)
			{
				printf("%d. %s\n",index+1,Tables_List[index].table_name);
			}
	        printf("\nEnter a number to choose a Table (or 'e' to 'E'): ");
	        scanf( "%s", instr );
	        switch_entry = atoi(instr);

	        if((instr[0] == 'e') || (instr[0] == 'E') )
	        {
	        	entry = VALID;
	        }
	        else
	        {
	        	switch_entry = atoi(instr);
				if ( (switch_entry <= 0)          ||
					 (switch_entry > Table_Count) ||
					 (strlen(instr) > 2)          ||
					 (false == isnum(instr) ))
				{
					entry = INVALID;
				   printf("\n\nInvalid entry '%s'. Please try again.", instr );
				}
				else
				{
					entry = VALID;
			        switch(switch_entry)
					{
						case 1:
							rc=get_inputfrom_inifilefor_TLF01_STIP();
							if(rc ==0)
							{
								EndProcess();
								return 0;
							}
							rc = validate_inputs();
							if(rc == 0)
							{
							    break; /* break from switch */
							}
							printf("\n\nInputs validated succesfully!");
							sprintf(ErrorMsg," Configuration read from ini file for TLF01_STIP archiving are as follows: \n"
									" Back up table name: %s, Start date: %s, End Date: %s, Look back days: %d \n",
									BackuptableName,p_archive_start_date,p_archive_end_date,look_back_days);
							write_into_log_file(ErrorMsg);
							printf("\n\n%s\n",ErrorMsg);
							printf("\n\nAre you sure you want to continue archiving with above details?[Press Y/y to continue or press any other character to exit]\n");
							scanf("%s", input);
							if(input[0] != 'Y' && input[0] != 'y')
							{
								sprintf(ErrorMsg," User aborted the process\n");
								write_into_log_file(ErrorMsg);
								printf("%s\n",ErrorMsg);
								entry = VALID;
								break; /* switch break*/
							} /* User continued with archiving */
							DB_rc = db_table_archive_tlf01_stip(ErrorMsg);
							if(DB_rc != PTEMSG_OK)
							{
								sprintf(ErrorMsg,"%s\n"," Archiving failed and DB rolled back!\n");
								write_into_log_file(ErrorMsg);
								printf("\n%s\n",ErrorMsg);
							}
							break;

						case 2:
							rc=get_inputfrom_inifilefor_BCH01_MP();
							if(rc == 0)
							{
								EndProcess();
								return 0;
							}
							rc = validate_inputs();
							if(rc == 0)
							{
								break; /* break from switch */
							}
							printf("\nInputs validated succesfully!");
							sprintf(ErrorMsg," Configuration read from ini file for BCH01_MP archiving are as follows: \n"
									" BackupTable Name for BCH01_MP: %s , BackupTable Name for BCH10: %s , Start date: %s, End Date: %s, Look back days: %d, \n",
									BackuptableName,BackuptableName_BCH10, p_archive_start_date,p_archive_end_date,look_back_days);;
							write_into_log_file(ErrorMsg);
							printf("\n\n%s",ErrorMsg);
							printf("\nAre you sure you want to continue archiving with above details?[Press Y/y to continue or press any other character to exit]\n");
							scanf("%s", input);
							if(input[0] != 'Y' && input[0] != 'y')
							{
								sprintf(ErrorMsg," User aborted the process\n");
								write_into_log_file(ErrorMsg);
								printf("%s\n",ErrorMsg);
								entry = VALID;
								break; /* switch break*/
							} /* User continued with archiving */
							DB_rc = db_table_archive_bch01_mp(ErrorMsg);
							if(DB_rc != PTEMSG_OK)
							{
								sprintf(ErrorMsg,"%s\n"," Archiving failed and DB rolled back!\n");
								write_into_log_file(ErrorMsg);
								printf("\n%s\n",ErrorMsg);
							}
							break;
						case 3:
							rc=get_inputfrom_inifilefor_BCH10();
							if(rc ==0)
							{

								EndProcess();
								return 0;
							}
							rc = validate_inputs();
							if(rc == 0)
							{
								break; /* break from switch */
							}
							printf("\n\nInputs validated succesfully!");
							sprintf(ErrorMsg," Configuration read from ini file for BCH10 archiving are as follows: \n"
												" Back up table name: %s, Start date: %s, End Date: %s, Look back days: %d \n",
												BackuptableName,p_archive_start_date,p_archive_end_date,look_back_days);
							write_into_log_file(ErrorMsg);
							printf("\n\n%s",ErrorMsg);
							printf("\n\nAre you sure you want to continue archiving with above details?[Press Y/y to continue or press any other character to exit]\n");
							scanf("%s", input);
							if(input[0] != 'Y' && input[0] != 'y')
							{
								sprintf(ErrorMsg," User aborted the process\n");
								write_into_log_file(ErrorMsg);
								printf("%s\n",ErrorMsg);
								entry = VALID;
								break; /* switch break*/
							} /* User continued with archiving */
							DB_rc = db_table_archive_bch10(ErrorMsg);
							if(DB_rc != PTEMSG_OK)
							{
								sprintf(ErrorMsg,"%s\n"," Archiving failed and DB rolled back!\n");
								write_into_log_file(ErrorMsg);
								printf("%s\n",ErrorMsg);
							}
							break;
						default:
							entry = INVALID;
							printf("\n\nWrong option selected by user, try again\n");
							break;
					}
				}
	        }
		}
	}

	else
		{
			sprintf(Error_Buf," Database connect failed::%s\n",ErrorMsg );
			printf("%s\n",Error_Buf);
			write_into_log_file(Error_Buf);
		}


	EndProcess();
	return 0;
}


/******************************************************************************
 *
 *  NAME:         isnum
 *
 *  DESCRIPTION:  This function checks for a string to be all numbers.
 *
 *  INPUTS:       in_str - String to be checked
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if string is all numbers, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT isnum(pCHAR in_str)
{
   INT i;
   INT length;

   length = strlen(in_str);
   if(length <= 0)
   {
	   return false;
   }

   for (i = 0; i < length; i++)
   {
      if (!isdigit(in_str[i]))
        return(false);
   }
   return(true);
}
INT validate_inputs()
{
	CHAR temp[5] = {0};
	CHAR strSystemDateTime[50] = {0};
	LONG juldate_today = 0;
	LONG juldate_startday = 0;
	CHAR strSystemDate[9] = {0};


	genutil_format_date (strSystemDateTime) ;

	strncpy(strSystemDate,strSystemDateTime,8);

	if (('\0'!=p_archive_start_date[0]) && ('\0'!=p_archive_end_date[0]))
	{
		if(false == isnum(p_archive_start_date) ||
		   false == isnum(p_archive_end_date))
		{
			sprintf(Error_Buf," Invalid Start/End Date, check the ini file\n");
			write_into_log_file(ErrorMsg);
			printf("\n%s",Error_Buf);
			return false;

		}
		if(atoi(p_archive_start_date) < atoi(p_archive_end_date))
		{
			sprintf(Error_Buf," Start date should be greater than End Date\n");
			write_into_log_file(ErrorMsg);
			printf("\n%s",Error_Buf);
			return false;
		}

		if(false == validate_date(p_archive_start_date) ||
		   false == validate_date(p_archive_end_date))
		{
			sprintf(Error_Buf," Date Validation Failed\n");
			write_into_log_file(ErrorMsg);
			printf("\n%s",Error_Buf);
			return false;
		}

		/* 2. check with lookup days*/
		juldate_startday = Txutils_Calculate_Julian_Date(p_archive_start_date);
		juldate_today	 = Txutils_Calculate_Julian_Date(strSystemDate);

		if(juldate_startday > juldate_today ||
		   (juldate_today - juldate_startday) < look_back_days)
		{
			sprintf(Error_Buf," Date range is not valid!, check the ini file\n");
			write_into_log_file(ErrorMsg);
			printf("\n%s",Error_Buf);
			return false;
		}
		itoa(look_back_days,lookBack_days_str,10);
		if(false == isnum(lookBack_days_str))
		{
			sprintf(Error_Buf," Invalid Look back day entered, check the ini file\n");
			write_into_log_file(ErrorMsg);
			printf("\n%s",Error_Buf);
			return false;
		}
	}
	else
	{
		sprintf(Error_Buf," Invalid Start Date and End date. Please check again!\n");
		write_into_log_file(ErrorMsg);
		printf("\n%s",Error_Buf);
		return false;
	}

	return true;
}



INT validate_date(pCHAR ndate)
{
	INT nYear   = 0 ;
	INT nMonth  = 0 ;
	INT nDay	= 0 ;
	CHAR temp[5]= {0};

	strncpy(temp,ndate,4);
	nYear = atoi(temp);

	memset(temp,0x00,sizeof(temp));
	strncpy(temp,ndate+4,2);
	nMonth = atoi(temp);

	nDay = atoi(ndate+6);

    if (nYear > MAX_VALID_YR ||
        nYear < MIN_VALID_YR)
    return false;
    if (nMonth < 1 || nMonth > 12)
    return false;
    if (nDay < 1 || nDay > 31)
    return false;

    // Handle February month
    // with leap year
    if (nMonth == 2)
    {
        if (isLeap(nYear))
        return (nDay <= 29);
        else
        return (nDay <= 28);
    }

    // Months of April, June,
    // Sept and Nov must have
    // number of days less than
    // or equal to 30.
    if (nMonth == 4 || nMonth == 6 ||
        nMonth == 9 || nMonth == 11)
        return (nDay <= 30);

    return true;
}

INT isLeap(int year)
{
	if (((year % 4   == 0)    &&
         (year % 100 != 0)) ||
         (year % 400 == 0))
	{
		return true;
	}
	return false;
}
