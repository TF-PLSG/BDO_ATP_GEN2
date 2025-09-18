/*******************************************************************************
*
*  Copyright (c) 2011, TF, Inc. All Rights Reserved.
*
*  MODULE:           dbf.c
*
*  TITLE:            Credit/Debit DBF TLF Files
*
*  DESCRIPTION:      This module creates
*
*
*  APPLICATION:      BDO
*
*  AUTHOR:           Phani
*
*  REVISION HISTORY:
*
* $   initial version release
*
*******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <memory.h>
#include <math.h>
#include <string.h>


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
#include "dbf.h"
#include "txutils.h"
#include "dc_database.h"
#include "memmnger.h"
#include "ptetimer.h"
#include "ptetime.h"
#include "dbcommon.h"
#include "timings.h"

#define MAX_FIELDS		66
#define MAX_FIELDS_SIZE	2112	/*66 * 32*/
#define MAX_HEADER_SIZE	32		/* 32*/

#define MODE_INSERT "INSERT"
#define MODE_UPDATE "UPDATE"
INT DB_NullCheck_Flag;

/* For DB statics added by raghu */
BYTE DB_MsgSubtype2 ;
BYTE DB_MsgSubtype1 ;
CHAR DB_Logging_Filename[256];
CHAR  DB_module_file_name[256];
CHAR  DB_file_path[256];
INT DB_NullCheck_Flag;

/* End   */

UINT Max_File_Size_Defined = 0 ;

/*This variable will get the value from the tf.ini
 * from the section DATASERVER with key name DB_ERROR_STATICS_FLAG
 * */
INT db_error_statics_flag;
double db_error_statics_value;
/***** External variables */
INT fprint_retval = 0;

/***** External variables */

extern volatile INT  EndProcessSignalled;
extern volatile INT  MainProcessDone;
extern int           Start();
extern void          Stop ();

/***** Global variables *****/
static CHAR		trands_que_name[] = "devdsA";

TLF01			auth_tx_total;

static CHAR		cutoff_date[9];
static CHAR		path[30] = "";  /*hold root directory path*/

static CHAR		dbf_filename[256] = "";    /* dump file name */
FILE			*dbfFilePtr = 0;	         /* dump file ptr */


static CHAR		dir_str  [128];
extern CHAR		ExeName[100];

INT Falcon_RT_Flag = 0;
INT Dcpimp_RT_Flag = 0;

CHAR			EndProcessFlag;
CHAR			AppName[12]={0};
CHAR			lError_Msg[300];
CHAR			Version[] = "ATP_11.8.4";
CHAR			version_msb = 11;
CHAR			version_lsb = 8;
/* Globals used in the Exception File processing = records over 45 days old */
INT   ExceptionFlag = true;  /* Default so old records DO get processed */

INT   TempExRecCnt;
INT   ExceptRecCnt;
INT   ExceptFile = false;
INT   ExceptError;
CHAR  ExceptDate[9];
CHAR  CurrentDate[9];

LONG  Totalwrittenrecordcount = 0 ;

/* Database Timing Measurements */
TIMINGS_LIST  TimingStats;
INT           DB_Timing_Flag; /* True = Do DB Timing Stats, False = don't do */
INT           ForceReport;
CHAR          ReportTime[5];

PRIVATE CHAR   global_err_str[100];
PRIVATE INT    global_err_code;
BYTE	des_key[200];
BYTE	cur_key[200];
BYTE	recs[4];

CHAR FileMode[5];


void dbf_read_falconrt_flag_for_transactions();
void dbf_read_falconrt_flag_for_MP_transactions();

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
*  AUTHOR:       Dennis Irby
*
******************************************************************************/
void get_date( pCHAR time_date, pCHAR date_str )
{
	memset ( date_str,  0x00,         9 );
	strncpy( date_str,  time_date,    4 );
	strncat( date_str, &time_date[5], 2 );
	strncat( date_str, &time_date[8], 2 );
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
*  AUTHOR:           Darcy Person
*
*******************************************************************************/
int main(int argc, char *argv[])
{
	char filename[256] = {0};
	INT ret_val = 0 ;
	sprintf(filename,DBF_FILE_INI);
	WORD key[] = {0x13A7, 0x6135, 0x9CDF, 0xA852};
						CHAR msgbuf[100] = "";

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
	memset(cutoff_date,  0, sizeof(cutoff_date));

	EndProcessFlag = false;
#ifndef WIN32
	strcpy(ExeName, argv[0]);
#endif

	memset(FileMode, 0, sizeof(FileMode));

	GetPrivateProfileString(
							"FILE",  /* pointer to section name*/
							"MODE",		 /* pointer to key name*/
							"TXT",		/*default string */
							FileMode ,    /* pointer to read string into*/
							5,	/*buffer size*/
							filename	 /* pointer to initialization filename*/
							);
	/*** 1st Parameter - base file name ***/

	if(strlen(argv[1]) == 2)
	{
		if(argv[1][0] == '-')
		{
			switch(argv[1][1])
			{
				case 'k':
					if(strlen(argv[2]) > 8)
					{
						printf("\nERROR!!!\n");
						printf("DES key should not exceed more than 8 characters\n");
						return 0;
					}
					if(strlen(argv[2]) > 0)
					{

						memcpy(des_key, argv[2], 8);
						printf("\n");
						printf("original key = %s\n",des_key);
						des_encryption( (unsigned char *)(des_key),
											strlen(des_key), key, 1 );
						WritePrivateProfileString(
						"SECURITY",  /* pointer to section name*/
						"KEY",		 /* pointer to key name*/
						des_key ,    /* pointer to string to add*/
						filename	 /* pointer to initialization filename*/
						);
						printf("Encrypted key = %s\n",des_key);
						printf("Key successfully set\n");
						return 0;

					}
					else
					{
							printf("\nPlesae supply a key to set\n");
							return 0;
					}
					break;
				case 'f':
					//Descryption
					if(argc == 3)
					{
						if(strlen(argv[2]) > 0)
						{
							GetPrivateProfileString(
							"SECURITY",  /* pointer to section name*/
							"KEY",		 /* pointer to key name*/
							"",		/*default string */
							cur_key ,    /* pointer to read string into*/
							200,	/*buffer size*/
							filename	 /* pointer to initialization filename*/
							);
							des_decryption( (unsigned char *)(cur_key ),
												strlen(cur_key ), key, 1 );

							//printf("key = %s\n",cur_key);

							dbf_decrypt_file(argv[2]);
							return 0;

						}
						else
						{
								printf("\nPlesae give the dbf filename to decrypt.\n");
								return 0;
						}
					}
					else if(argc == 5)
					{
						if(strlen(argv[2]) > 0)
						{
							if(argv[3][1] == 'd')
							{
								if(strlen(argv[4]) > 0)
								{
									if(strlen(argv[4]) <=8)
									{
										strcpy(cur_key, argv[4]);
										dbf_decrypt_file(argv[2]);
										return 0;

									}
									else
									{
										printf("DES key should not exceed more than 8 characters.\n");
										return 0;
									}
								}
								else
								{
									printf("Please give key which was used for encrypting the file.\n");
									return 0;
								}
							}
							else
							{
								printf("Invalid syntax.\n");
								return 0;
							}
						}
						else
						{
								printf("\nPlesae give the dbf filename to decrypt.\n");
								return 0;
						}

					}
					else
					{
						printf("Invalid syntax.\n");
						return 0;
					}
					break;
				default:
					break;
			}
		}
		else
		{
			printf("\n\n");
			printf("Error in 2nd parameter.\n");
			printf("\n");
			return 0;
		}
	}
	else
	{
		if (strlen(argv[1]) != 8)
		{
			printf("\n\n");
			printf("Error in 2nd parameter.\n");
			printf("Date must be in YYYYMMDD format.\n");
			printf("\n");
			return 0;
		}
	}

	/*** 2nd Parameter - cutoff date ***/
	if (strcmp(argv[1], ",") != 0)
	{

	}
	strcpy(cutoff_date, argv[1]);
	/** Get the DES key and decrypt**/

	GetPrivateProfileString(
    "SECURITY",	            /*  points to section name  */
    "KEY",              /*  points to key name  */
    "DEFAULT",               /*  points to default string  */
    des_key,                         /*  points to destination buffer  */
    sizeof(des_key),             /*  size of destination buffer  */
    filename                        /*  points to initialization filename  */
   );
	if(strcmp(des_key, "DEFAULT") != 0)
	{
		des_decryption(des_key, strlen(des_key), key, 1);
		//printf("Decrypted key = %s\n", des_key);
		
	}
	else
	{
		printf("Key not found\n");
		return 0;
	}
	if(!Start())
		printf("Error starting ServiceManager\n\n" );

	Stop();

} /* main */

long get_first_block_size()
{
	long size = 0;
	WORKING_DBF_TLF01 w_dbf_tlf01;
	size = sizeof(w_dbf_tlf01.transaction_id) +
		sizeof(w_dbf_tlf01.entry_type) +
		sizeof(w_dbf_tlf01.acquirer_id) +
		sizeof(w_dbf_tlf01.terminal_id) +
		sizeof(w_dbf_tlf01.response_text) +
		sizeof(w_dbf_tlf01.card_num_len);
	return size;
}

BOOLEAN dbf_decrypt_file(char *filename)
{
	WORKING_DBF_TLF01 w_dbf_tlf01;
	BYTE header[10000] = {0};
	BOOLEAN bRet = 0;

	if(filename == NULL || strlen((char *)cur_key) == 0)
		return -1;

	char tmpFile[128] = {0};
	sprintf(tmpFile,"tmp_%s",filename);
	FILE *tmpPtr =NULL;

	FILE *fptr = fopen(filename,"rb");
	if(fptr == NULL)
		return -1;
	int card_size = sizeof(w_dbf_tlf01.card_num);
	long block_1 = get_first_block_size();
	long block_2 = sizeof(WORKING_DBF_TLF01) - block_1 - card_size;
	long count = 0;
	if(fptr != NULL)
	{
		rewind(fptr);
		printf("Decrypting the records......\n");
		memset(&w_dbf_tlf01, 0, sizeof(WORKING_DBF_TLF01));
		fread(header,1,MAX_HEADER_SIZE + MAX_FIELDS_SIZE +1, fptr);
		if(header[12] == 0x00)
		{
			tmpPtr = fopen(tmpFile,"wb");
			if(tmpPtr != NULL)
			{
				header[12] = 0x11;
				fwrite(header,1,MAX_HEADER_SIZE + MAX_FIELDS_SIZE +1, tmpPtr);
				while(!feof(fptr))
				{
					bRet = 1;
					fread(&w_dbf_tlf01, 1, sizeof(WORKING_DBF_TLF01), fptr);
					//printf("%s\n",w_dbf_tlf01.card_num);
					int i;
						for(i=0;i<sizeof(w_dbf_tlf01.card_num);i++)
						{
							if(w_dbf_tlf01.card_num[i] == 0x20)
								w_dbf_tlf01.card_num[i] = 0x00;
						}

					des_decryption(w_dbf_tlf01.card_num,strlen(w_dbf_tlf01.card_num),cur_key,1);
						for(i=0;i<sizeof(w_dbf_tlf01.card_num);i++)
						{
							if(w_dbf_tlf01.card_num[i] == 0x00)
								w_dbf_tlf01.card_num[i] = 0x20;
						}

					fwrite(&w_dbf_tlf01, 1, sizeof(WORKING_DBF_TLF01), tmpPtr);
					memset(&w_dbf_tlf01, 0, sizeof(WORKING_DBF_TLF01));
					count++;
				}
				if(bRet != 1)
				{
					remove(tmpFile);
				}
				fclose(tmpPtr);
			}
		}
		else
		{
			printf("The file '%s' is already decrypted.\n",filename);
			bRet = 0;
		}
		fclose(fptr);
	}


	if(bRet == 1)
	{

		tmpPtr =fopen(tmpFile,"rb");
		if(tmpPtr == NULL)
				return -1;
		fptr = fopen(filename,"wb");
		if(fptr == NULL)
				return -1;
		while(!feof(tmpPtr))
		{
			fread(header,10000,1,tmpPtr);
			fwrite(header,10000,1,fptr);
		}
		fclose(fptr);
		fclose(tmpPtr);
		remove(tmpFile);
		printf("%ld records decrypted.\n",count);

	}

		return bRet;
}

BOOLEAN tlf01_handler(pTLF01 tlf01_ptr)
{
	return true;
}

double Char2Hex(char *lpHex)
{
	double hexValue = 0;
	int len = strlen(lpHex);
	double temp = 0;
	double base = 16;
	int i = 0;
	for(i=len-1; i>=0;i--)
	{
		switch(lpHex[i])
		{
		case '0':
			temp = 0;
			break;
		case '1':
			temp = 1 * (pow(base,len-i-1));
			break;
		case '2':
			temp = 2 * (pow(base,len-i-1));
			break;
		case '3':
			temp = 3 * (pow(base,len-i-1));
			break;
		case '4':
			temp = 4 * (pow(base,len-i-1));
			break;
		case '5':
			temp = 5 * (pow(base,len-i-1));
			break;
		case '6':
			temp = 6 * (pow(base,len-i-1));
			break;
		case '7':
			temp = 7 * (pow(base,len-i-1));
			break;
		case '8':
			temp = 8 * (pow(base,len-i-1));
			break;
		case '9':
			temp = 9 * (pow(base,len-i-1));
			break;
		case 'A':
			temp = 10 * (pow(base,len-i-1));
			break;
		case 'B':
			temp = 11 * (pow(base,len-i-1));
			break;
		case 'C':
			temp = 12 * (pow(base,len-i-1));
			break;
		case 'D':
			temp = 13 * (pow(base,len-i-1));
			break;
		case 'E':
			temp = 14 * (pow(base,len-i-1));
			break;
		case 'F':
			temp = 15 * (pow(base,len-i-1));
			break;
		}
		hexValue += temp;
	}

	return hexValue;

}

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
*  AUTHOR:           Darcy Person
*
*******************************************************************************/
void MainProcessor(void)
{
	CHAR			szTotalRecords[20] = {0};
	INT				nTotalRecords;
	BOOLEAN			bContinue = false;
	CHAR              buffer[500] = {0};
	CHAR              err_msg[1024] = {0};
	CHAR              time_date[25] = "";
	INT               count_tran = 0;
	INT               next_portion = 0;
	INT               num_returned = 0;
	INT               ret_code = 0;
	LONG              julian = 0;
	BYTE              bch01_buffer[sizeof(AUTH_TX) + sizeof(BCH01)];
	INT               num_sql = 0;
	INT               err_ctr = 0;
	INT 			  ret_val = 0 ;
	double            start_time = 0;
	double            end_time = 0;
	double            duration = 0;

	/* Clear buffer (set to null) */
	memset(&bch01_buffer,0, sizeof(bch01_buffer));
	memset (&lError_Msg, 0x00, sizeof (lError_Msg));
	memset (&buffer, 0x00, sizeof(buffer));

	GetAppName (AppName);
	ret_val=get_dataserver_ini_db_error_statics_value( &db_error_statics_flag, &db_error_statics_value,
			AppName,     buffer );
	sprintf( buffer,
		"Starting DBF File creation: %s, version %s", AppName, Version );
	dbf_arch_log_message(2,2,buffer,"MainProcessor");

	/* Send message to DOS window in debug mode */
	strcat(buffer,"\n");
	printf(buffer);

	/*Get root directory path*/
	memset (path, 0x00, sizeof(path));
	GetAscendentRootDirectory(path);
	dbf_read_falconrt_flag_for_transactions();
	dbf_read_falconrt_flag_for_MP_transactions();

	/* Get Database Timing Parameters to gather statistics about each query. */
	memset( ReportTime, 0x00, sizeof(ReportTime) );
	memset( buffer,     0x00, sizeof(buffer)     );
	/* Try to connect to the database */
	if (dbcommon_connect_Archds("equitable", "equitable", "equitable", "ORCL", err_msg) != PTEMSG_OK)
	{
		memset(&buffer, 0x00, sizeof(buffer));
		strcpy(buffer, "Database Connect Error:  ");
		strcat(buffer, err_msg);
		printf(buffer);
		MainProcessDone = 1;

		return;
	}

	/* Send message to system monitor */
	printf("Connected to ORACLE \n");

	/* Set cutoff date and time */

	/* Set dates for Exception processing. */
	ptetime_get_timestamp( time_date );  /* YYYY-MM-DD-hh.mm.ss.jjj */
	get_date( time_date, CurrentDate );  /* YYYYMMDD */

	/* Exception Date = Current Date minus 45 days */

	{
		dbfFilePtr = NULL;
		char totalCount[20] = {0};
		long totalRecords = 0;
		if (ret_code =  (db_get_tlf01_count(totalCount, err_msg, cutoff_date)) != PTEMSG_OK)
		{
			sprintf (buffer, "Error processing TLF01.");
			sprintf(lError_Msg,"db_get_tlf01_count: %s",buffer);
			dbf_arch_log_message(2,3,lError_Msg,"MainProcessor");

		}
		totalRecords = atol(totalCount);
		//long f = 2147483647;
		//printf("%08X\n",f);
		printf("Total records found = %d\n", totalRecords);

		if(totalRecords >= 0)
		{
			char tempStr[3] = {0};
			sprintf(totalCount,"%08X",totalRecords);
			tempStr[0] = totalCount[0];
			tempStr[1] = totalCount[1];
			tempStr[2] = '\0';
			recs[0] = Char2Hex(tempStr);

			tempStr[0] = totalCount[2];
			tempStr[1] = totalCount[3];
			tempStr[2] = '\0';
			recs[1] = Char2Hex(tempStr);

			tempStr[0] = totalCount[4];
			tempStr[1] = totalCount[5];
			tempStr[2] = '\0';
			recs[2] = Char2Hex(tempStr);

			tempStr[0] = totalCount[6];
			tempStr[1] = totalCount[7];
			tempStr[2] = '\0';
			recs[3] = Char2Hex(tempStr);

			if(strcmp(FileMode, "DBF") == 0)
			{
				strcpy(dbf_filename,"TLF_DBF_");
				strcat(dbf_filename,cutoff_date);
				strcat(dbf_filename,".dbf");
				if(CreateDBF(dbf_filename))
				{
					if (ret_code =  (db_get_tlf01_list(err_msg, cutoff_date)) != PTEMSG_OK)
					{
						sprintf (buffer, "Error processing TLF01.");
						sprintf(lError_Msg,"db_get_tlf01_list: %s",buffer);
						dbf_arch_log_message(2,3,lError_Msg,"MainProcess");
					}
				}
			}
			else
			{
				strcpy(dbf_filename,"TLF_DBF_");
				strcat(dbf_filename,cutoff_date);
				strcat(dbf_filename,".TXT");
				Totalwrittenrecordcount = 0 ;
				if(CreateTSV(dbf_filename))
				{
					if (ret_code =  (db_get_tlf01_list(err_msg, cutoff_date)) != PTEMSG_OK)
					{
						sprintf (buffer, "Error processing TLF01.");
						sprintf(lError_Msg,"db_get_tlf01_list: %s",buffer);
						dbf_arch_log_message(2,3,lError_Msg,"MainProcessor");
					}
					printf("Total number of records written in dbf file is %ld \n",Totalwrittenrecordcount);
					if(Totalwrittenrecordcount !=totalRecords )
					{
						printf("Total records : %ld found from TLF01 and Total number of records : %ld written in dbf file are not equal \n",totalRecords, Totalwrittenrecordcount);
					}
					WriteTSVFooter(Totalwrittenrecordcount);
				}
				//TSV
			}
		}

	}
	/*disconnect from database*/
	dbcommon_disconnect (err_msg);

	printf("Completing %s \n", FileMode);

	if( dbfFilePtr != NULL)
	{
		fclose(dbfFilePtr);
		printf("'%s' DBF file created!!!\n",dbf_filename);
	}
	else
	{
		printf("Unable to create the DBF File!\n");
	}
	MainProcessDone = 1;

	return;

} /* MainProcessor */

BOOLEAN CreateDBF(char *filename)
{
	dbfFilePtr = fopen(filename,"rb");
	if(dbfFilePtr != NULL)
	{
		fclose(dbfFilePtr);
		dbfFilePtr = NULL;
		printf("the DBF file %s already exists!. Do you want to overwrite? [Y/N]:");
		char c = getchar();
		if(c != 'y' && c != 'Y')
		{
			return false;
		}
	}
	dbfFilePtr = fopen(filename, "wb");
	if(dbfFilePtr == NULL)
	{
		printf("Unable to create the file.\n");
		return false;
	}
	return WriteDBFHeader();

}

BOOLEAN WriteDBFHeader()
{
	char header[32] =  {0};
	char FieldArray[6080];
	int i=0;
	time_t now; struct tm* timeStamp;
	if(dbfFilePtr == NULL) { return FALSE; }

	memset(FieldArray, 0, 6080);
	now = time(0); timeStamp = localtime(&now);

//printf("DATE: %d - %d - %d\n",timeStamp->tm_year + 1900, timeStamp->tm_mon + 1, timeStamp->tm_mday);
	header[0] = 0x03;
	header[1] = timeStamp->tm_year + 1900;
	header[2] = timeStamp->tm_mon + 1;
	header[3] = timeStamp->tm_mday;

	//Maximum number of records. LSB first
//	header[4] = 0xFF;
//	header[5] = 0xFF;
//	header[6] = 0xFF;
//	header[7] = 0x7F;

	header[4] = recs[3];
	header[5] = recs[2];
	header[6] = recs[1];
	header[7] = recs[0];

	header[8] = 0x60;		//LSB of total size of the header
	header[9] = 0x08;		//HSB of total size of the header

	//Total records size must be the addition of size of all the variables of DBF_TLF that is to be written in the dbf file
	//Anyway computed below in this function
	header[10] = 0xA0;			//LSB of total size of records
	header[11] = 0x03;			//HSB of total size of records
	header[12] = 0x00;			// Used to find whether the file is encrypted or decrypted. 0- ENC 1-DEC
	header[13] = version_msb;
	header[14] = version_lsb;

	strcpy(&FieldArray[0], "TRAN_ID");		//1
	FieldArray[11] = 'C';
	strcpy(&FieldArray[32], "ENTRY_TYP");	//2
	FieldArray[43] = 'C';
	strcpy(&FieldArray[64], "ACQUIR_ID");	//5
	FieldArray[75] = 'C';
	strcpy(&FieldArray[96], "TERM_ID");		//7
	FieldArray[107] = 'C';
	strcpy(&FieldArray[128], "RESP_TEXT");	//9
	FieldArray[139] = 'C';
	strcpy(&FieldArray[160], "C_NUM_LEN");	//10
	FieldArray[171] = 'C';
	strcpy(&FieldArray[192], "CARD_NUM");	//11
	FieldArray[203] = 'C';
	strcpy(&FieldArray[224], "EXP_DATE");	//12
	FieldArray[235] = 'C';
	strcpy(&FieldArray[256], "TOT_AMT");	//13
	FieldArray[267] = 'C';
	strcpy(&FieldArray[288], "ORIG_AMT");	//16
	FieldArray[299] = 'C';
	strcpy(&FieldArray[320], "AUTH_NUM");	//17
	FieldArray[331] = 'C';
	strcpy(&FieldArray[352], "PROC_CODE");	//18
	FieldArray[363] = 'C';
	strcpy(&FieldArray[384], "SYS_AUDIT");	//19
	FieldArray[395] = 'C';
	strcpy(&FieldArray[416], "ORGSYSTRA");	//20
	FieldArray[427] = 'C';
	strcpy(&FieldArray[448], "POSENTMOD");	//21
	FieldArray[459] = 'C';
	strcpy(&FieldArray[480], "POSCONCOD");	//23
	FieldArray[491] = 'C';
	strcpy(&FieldArray[512], "RETREFNUM");	//26
	FieldArray[523] = 'C';
	strcpy(&FieldArray[544], "ORGRETNUM");	//27
	FieldArray[555] = 'C';
	strcpy(&FieldArray[576], "MERCH_ID");	//28
	FieldArray[587] = 'C';
	strcpy(&FieldArray[608], "ORG_MSG");	//29
	FieldArray[619] = 'C';
	strcpy(&FieldArray[640], "SET_TOTAL");	//30
	FieldArray[651] = 'C';
	strcpy(&FieldArray[672], "MSG_TYPE");	//31
	FieldArray[683] = 'C';
	strcpy(&FieldArray[704], "CHOLD_NAME");	//32
	FieldArray[715] = 'C';
	strcpy(&FieldArray[736], "GEN_STATUS");	//33
	FieldArray[747] = 'C';
	strcpy(&FieldArray[768], "BATCH_NUM");	//36
	FieldArray[779] = 'C';
	strcpy(&FieldArray[800], "RESP_CODE");	//37
	FieldArray[811] = 'C';
	strcpy(&FieldArray[832], "TRAN_DATE");	//38
	FieldArray[843] = 'C';
	strcpy(&FieldArray[864], "TRAN_TIME");	//39
	FieldArray[875] = 'C';
	strcpy(&FieldArray[896], "TRANSTTIME");	//51
	FieldArray[907] = 'C';
	strcpy(&FieldArray[928], "HOSTSTTIME");	//52
	FieldArray[939] = 'C';
	strcpy(&FieldArray[960], "HOSTFITIME");	//53
	FieldArray[971] = 'C';
	strcpy(&FieldArray[992], "TRANFITIME");	//54
	FieldArray[1003] = 'C';
	strcpy(&FieldArray[1024],"AGENT_ID");	//63
	FieldArray[1035] = 'C';
	strcpy(&FieldArray[1056],"CVC");		//64
	FieldArray[1067] = 'C';
	strcpy(&FieldArray[1088],"SETTLEDATE");	//65
	FieldArray[1099] = 'C';
	strcpy(&FieldArray[1120],"CATEG_CODE");	//65
	FieldArray[1131] = 'C';
	strcpy(&FieldArray[1152],"CONV_RATE");	//67
	FieldArray[1163] = 'C';
	strcpy(&FieldArray[1184],"PROD_CODE");	//68
	FieldArray[1195] = 'C';
	strcpy(&FieldArray[1216], "ACQ_ID");	//84
	FieldArray[1227] = 'C';
	strcpy(&FieldArray[1248], "OUT_BAL");	//85
	FieldArray[1259] = 'C';
	strcpy(&FieldArray[1280], "CRDT_LMT");	//86
	FieldArray[1291] = 'C';
	strcpy(&FieldArray[1312], "ORIGIN");	//92
	FieldArray[1323] = 'C';
	strcpy(&FieldArray[1344], "INT_RATE");	//99
	FieldArray[1355] = 'C';
	strcpy(&FieldArray[1376], "TOT_INT");	//100
	FieldArray[1387] = 'C';
	strcpy(&FieldArray[1408], "PROFILE");	//103
	FieldArray[1419] = 'C';
	strcpy(&FieldArray[1440], "NBRPRDCODE");//104
	FieldArray[1451] = 'C';
	strcpy(&FieldArray[1472], "PRDT1_CDE");	//105
	FieldArray[1483] = 'C';
	strcpy(&FieldArray[1504], "PRDT1_QTY");	//106
	FieldArray[1515] = 'C';
	strcpy(&FieldArray[1536], "PRDT1_AMT");	//107
	FieldArray[1547] = 'C';
	strcpy(&FieldArray[1568], "VISA_M_ID");	//165
	FieldArray[1579] = 'C';
	strcpy(&FieldArray[1600], "DEF_FACTOR");//167
	FieldArray[1611] = 'C';
	strcpy(&FieldArray[1632], "DEFTERMLEN");//168
	FieldArray[1643] = 'C';
	strcpy(&FieldArray[1664], "CURNCY_CDE");//169
	FieldArray[1675] = 'C';
	strcpy(&FieldArray[1696], "TRANS_TIME");//170
	FieldArray[1707] = 'C';
	strcpy(&FieldArray[1728], "CVC_DATA");	//172
	FieldArray[1739] = 'C';
	strcpy(&FieldArray[1760], "VERMAGSTR");	//173
	FieldArray[1771] = 'C';
	strcpy(&FieldArray[1792], "SECRESCODE");//175
	FieldArray[1803] = 'C';
	strcpy(&FieldArray[1824], "SERV_CODE");//176
	FieldArray[1835] = 'C';
	strcpy(&FieldArray[1856], "VOICE_AUTH");//179
	FieldArray[1867] = 'C';
	strcpy(&FieldArray[1888], "ACTUAL_AMT");//180
	FieldArray[1899] = 'C';
	strcpy(&FieldArray[1920], "REVERS_AMT");//181
	FieldArray[1931] = 'C';
	strcpy(&FieldArray[1952], "V_AUTH_TXT");//182
	FieldArray[1963] = 'C';
	strcpy(&FieldArray[1984], "OPERAT_ID");	//183
	FieldArray[1995] = 'C';
	strcpy(&FieldArray[2016], "ORGAUTHNUM");//185
	FieldArray[2027] = 'C';
	strcpy(&FieldArray[2048], "SYS_DATE");	//187
	FieldArray[2059] = 'C';
	strcpy(&FieldArray[2080],  "M_AMORT_AM");//189
	FieldArray[2091] = 'C';



	FieldArray[16]		=    21; 	//TRAN_ID		//1
	FieldArray[48]		=    1;  	//ENTRY_TYP		//2
	FieldArray[80]		=   21; 	//ACQUIR_ID		//5
	FieldArray[112]		=   9;  	//TERM_ID		//7
	FieldArray[144]		=   41; 	//RESP_TEXT		//9
	FieldArray[176]		=   3;  	//C_NUM_LEN		//10
	FieldArray[208]		=   65; 	//CARD_NUM		//11
	FieldArray[240]		=   5;  	//EXP_DATE		//12
	FieldArray[272]		=   13; 	//TOT_AMT		//13
	FieldArray[304]		=   13; 	//ORIG_AMT		//16
	FieldArray[336]		=   7;  	//AUTH_NUM		//17
	FieldArray[368]		=   7;  	//PROC_CODE		//18
	FieldArray[400]		=   7;  	//SYS_AUDIT		//19
	FieldArray[432]		=   7;  	//ORGSYSTRA		//20
	FieldArray[464]		=   5;  	//POSENTMOD		//21
	FieldArray[496]		=   3; 		//POSCONCOD		//23
	FieldArray[528]		=   13; 	//RETREFNUM		//26
	FieldArray[560]		=   13; 	//ORGRETNUM		//27
	FieldArray[592]		=   16; 	//MERC_ID		//28
	FieldArray[624]		=   5;  	//ORG_MSG		//29
	FieldArray[656]		=   17; 	//SET_TOTAL		//30
	FieldArray[688]		=   5;  	//MSG_TYPE		//31
	FieldArray[720]		=  31; 	//CHOLD_NAME	//32
	FieldArray[752]		=  2;  	//GEN_STATUS	//33
	FieldArray[784]		=  7;  	//BATCH_NUM		//36
	FieldArray[816]		=  3;  	//RESP_CODE		//37
	FieldArray[848]		=  9;  	//TRAN_DATE		//38
	FieldArray[880]		=  7;  	//TRAN_TIME		//39
	FieldArray[912]		=  17; 	//TRANSTTIME	//51
	FieldArray[944]		=  17; 	//HOSTSTTIME	//52
	FieldArray[976]		=  17; 	//HOSTFITIME	//53
	FieldArray[1008]	=  17; 	//TRANFITIME	//54
	FieldArray[1040]	=  12; 	//AGENT_ID		//63
	FieldArray[1072]	=  1;  	//CVC			//64
	FieldArray[1104]	=  5;  	//SETTLEDATE	//65
	FieldArray[1136]	=  5;  	//CATE_CODE		//66
	FieldArray[1168]	=  10; 	//CONV_RATE		//67
	FieldArray[1200]	=  9;  	//PROD_CODE		//68
	FieldArray[1232]	=  12; 	//ACQ_ID		//84
	FieldArray[1264]	=  13; 	//OUT_BAL		//85
	FieldArray[1296]	=  13; 	//CRDT_LMT		//86
	FieldArray[1328]	=  3;  	//ORIGIN		//92
	FieldArray[1360]	=  5;  	//INT_RATE		//99
	FieldArray[1392]	=  13; 	//TOT_INT		//100
	FieldArray[1424]	=  3;  	//PROFILE		//103
	FieldArray[1456]	=  3;  	//NBRPRDCODE	//104
	FieldArray[1488]	=  7;  	//PRDT1_CDE		//105
	FieldArray[1520]	=  3;  	//PRDT1_QTY		//106
	FieldArray[1552]	=  13; 	//PRDT1_AMT		//107
	FieldArray[1584]	=  16; 	//VISA_M_ID		//165
	FieldArray[1616]	=  8;  	//DEF_FACTOR	//167
	FieldArray[1648]	=  4;  	//DEFTERMLEN	//168
	FieldArray[1680]	=  4;  	//CURNCY_CDE	//169
	FieldArray[1712]	=  11; 	//TRANS_TIME	//170
	FieldArray[1744]	=  4;  	//CVC_DATA		//172
	FieldArray[1776]	=  2;  	//VERMAGSTR		//173
	FieldArray[1808]	=  3;  	//SECRESCODE	//175
	FieldArray[1840]	=  4;  	//SERV_CODE		//176
	FieldArray[1872]	=  2;  	//VOICE_AUTH	//179
	FieldArray[1904]	=  13; 	//ACTUAL_AMT	//180
	FieldArray[1936]	=  13; 	//REVERS_AMT	//181
	FieldArray[1968]	=  241;	//V_AUTH_TXT	//182
	FieldArray[2000]	=  21; 	//OPERAT_ID		//183
	FieldArray[2032]	=  7;  	//ORGAUTHNUM	//185
	FieldArray[2064]	=  9;  	//SYS_DATE		//187
	FieldArray[2096]	=  13; 	//M_AMORT_AM	//189

	{
		int i=0;
		int totalsize = 0;
		for(i=16;i<MAX_FIELDS_SIZE;i=i+32)
		{
			totalsize	+=	FieldArray[i];
		}
		char totalCount[10] = {0};
		char tempStr[3] = {0};
			sprintf(totalCount,"%04X",totalsize);
			tempStr[0] = totalCount[0];
			tempStr[1] = totalCount[1];
			tempStr[2] = '\0';
			recs[0] = Char2Hex(tempStr);
			tempStr[0] = totalCount[2];
			tempStr[1] = totalCount[3];
			tempStr[2] = '\0';
			recs[1] = Char2Hex(tempStr);
				//Total records size must be the addition of size of all the variables of DBF_TLF that is to be written in the dbf file
		header[10] = recs[1];			//LSB of total size of records
		header[11] = recs[0];			//HSB of total size of records
	}

	fwrite(header,1,32,dbfFilePtr);
	fwrite(FieldArray,1,MAX_FIELDS_SIZE,dbfFilePtr);
	char sep = 0x20;
	fwrite(&sep, 1, 1, dbfFilePtr);
	return TRUE;
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
		dbf_arch_log_message(3,3,buffer,"WriteString");
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
		sprintf( buffer,"File write failed, ret value= %d, written count %ld, Field: %s -> %s",fprint_retval,Totalwrittenrecordcount,header, str);
		dbf_arch_log_message(3,3,buffer,"WriteString");
		fprint_retval = 0;
	}
	return TRUE;
}

BOOLEAN WriteChar(char c)
{
	return TRUE;
}

BOOLEAN WriteTLF01TableIntoDBF(DBF_TLF01 *pte_tlf01)
{
	if(dbfFilePtr == NULL)
	{
		return FALSE;
	}
	fwrite(pte_tlf01->transaction_id, 1 ,	sizeof(pte_tlf01->transaction_id),	dbfFilePtr);	//1

	//Restrict entry type always to be one character.
	if(pte_tlf01->entry_type > 0x09)
	{
		pte_tlf01->entry_type = 0;	//Invalid entry type. 
	}
	fprintf(dbfFilePtr,"%x",pte_tlf01->entry_type);
	fwrite(pte_tlf01->acquirer_id, 1 ,		sizeof(pte_tlf01->acquirer_id),		dbfFilePtr);	//5
	fwrite(pte_tlf01->terminal_id, 1 ,		sizeof(pte_tlf01->terminal_id),		dbfFilePtr);	//7
	fwrite(pte_tlf01->response_text, 1 ,	sizeof(pte_tlf01->response_text),	dbfFilePtr);	//9
	fwrite(pte_tlf01->card_num_len, 1 ,		sizeof(pte_tlf01->card_num_len),	dbfFilePtr);	//10
	char str[65] = {0};
	memcpy(str, pte_tlf01->card_num,64);
	//printf("%s - %d\n",str,strlen(str));
	int i = 0;
	for(i=0;i<sizeof(str);i++)
	{
		if(str[i] == 0x20)
			str[i] = 0x00;
	}
	des_encryption((unsigned char *)str, strlen(str), des_key, 1);
	for(i=0;i<sizeof(str);i++)
	{
		if(str[i] == 0x00)
			str[i] = 0x20;
	}

	//fwrite(pte_tlf01->card_num, 1 ,		  sizeof(pte_tlf01->card_num),			dbfFilePtr);
	fwrite(str, 1 ,		sizeof(str),	dbfFilePtr);	//11

	fwrite(pte_tlf01->exp_date, 1 ,				sizeof(pte_tlf01->exp_date),			dbfFilePtr);						//12
	fwrite(pte_tlf01->total_amount, 1 ,			sizeof(pte_tlf01->total_amount),		dbfFilePtr);						//13
	fwrite(pte_tlf01->orig_amount, 1 ,			sizeof(pte_tlf01->orig_amount),		dbfFilePtr);							//16
	fwrite(pte_tlf01->auth_number, 1 ,			sizeof(pte_tlf01->auth_number),		dbfFilePtr);							//17
	fwrite(pte_tlf01->processing_code, 1 ,		sizeof(pte_tlf01->processing_code),		dbfFilePtr);						//18
	fwrite(pte_tlf01->sys_trace_audit_num, 1 ,	sizeof(pte_tlf01->sys_trace_audit_num),	dbfFilePtr);						//19
	fwrite(pte_tlf01->orig_sys_trace_num, 1 ,	sizeof(pte_tlf01->orig_sys_trace_num),		dbfFilePtr);					//20
	fwrite(pte_tlf01->pos_entry_mode, 1 ,		sizeof(pte_tlf01->pos_entry_mode),		dbfFilePtr);						//21
	fwrite(pte_tlf01->pos_condition_code, 1 ,	sizeof(pte_tlf01->pos_condition_code),		dbfFilePtr);					//23
	fwrite(pte_tlf01->retrieval_ref_num, 1 ,	sizeof(pte_tlf01->retrieval_ref_num),		dbfFilePtr);					//26
	fwrite(pte_tlf01->orig_retrieval_ref_num, 1,sizeof(pte_tlf01->orig_retrieval_ref_num),	dbfFilePtr);					//27
	fwrite(pte_tlf01->merchant_id, 1 ,			sizeof(pte_tlf01->merchant_id),		dbfFilePtr);							//28
	fwrite(pte_tlf01->orig_message, 1 ,			sizeof(pte_tlf01->orig_message),		dbfFilePtr);						//29
	fwrite(pte_tlf01->settlement_total, 1 ,		sizeof(pte_tlf01->settlement_total),		dbfFilePtr);					//30
	fwrite(pte_tlf01->message_type, 1 ,			sizeof(pte_tlf01->message_type),		dbfFilePtr);						//31
	fwrite(pte_tlf01->card_holder_name, 1 ,		sizeof(pte_tlf01->card_holder_name),		dbfFilePtr);					//32
	if(pte_tlf01->general_status == 0x00)
	{
		pte_tlf01->general_status = 0x20;
	}
	fprintf(dbfFilePtr,"%02d",pte_tlf01->general_status);
	//fwrite(pte_tlf01->general_status, 1 ,		sizeof(pte_tlf01->general_status),		dbfFilePtr);						//33
	fwrite(pte_tlf01->batch_number, 1 ,			sizeof(pte_tlf01->batch_number),		dbfFilePtr);						//36
	fwrite(pte_tlf01->response_code, 1 ,		sizeof(pte_tlf01->response_code),		dbfFilePtr);						//37
	date_to_string(pte_tlf01->date_yyyymmdd);
	fwrite(pte_tlf01->date_yyyymmdd, 1 ,		sizeof(pte_tlf01->date_yyyymmdd),		dbfFilePtr);						//38
	fwrite(pte_tlf01->time_hhmmss, 1 ,			sizeof(pte_tlf01->time_hhmmss),		dbfFilePtr);							//39
	fwrite(pte_tlf01->tran_start_time, 1 ,		sizeof(pte_tlf01->tran_start_time),		dbfFilePtr);						//51
	fwrite(pte_tlf01->host_start_time, 1 ,		sizeof(pte_tlf01->host_start_time),		dbfFilePtr);						//52
	fwrite(pte_tlf01->host_finish_time, 1 ,		sizeof(pte_tlf01->host_finish_time),		dbfFilePtr);					//53
	fwrite(pte_tlf01->tran_finish_time, 1 ,		sizeof(pte_tlf01->tran_finish_time),		dbfFilePtr);					//54
	fwrite(pte_tlf01->agent_id, 1 ,				sizeof(pte_tlf01->agent_id),			dbfFilePtr);						//63
	if(pte_tlf01->cvc == 0x00)
	{
		pte_tlf01->cvc = ' ';
	}
	fprintf(dbfFilePtr,"%c",pte_tlf01->cvc);
	fwrite(pte_tlf01->settlement_date, 1 ,			sizeof(pte_tlf01->settlement_date),		dbfFilePtr);						//65
	fwrite(pte_tlf01->category_code, 1 ,			sizeof(pte_tlf01->category_code),		dbfFilePtr);						//66
	fwrite(pte_tlf01->conversion_rate, 1 ,			sizeof(pte_tlf01->conversion_rate),		dbfFilePtr);						//67
	fwrite(pte_tlf01->product_code, 1 ,				sizeof(pte_tlf01->product_code),		dbfFilePtr);						//68
	fwrite(pte_tlf01->acquiring_id, 1 ,				sizeof(pte_tlf01->acquiring_id),		dbfFilePtr);					//85
	fwrite(pte_tlf01->outstanding_balance, 1 ,		sizeof(pte_tlf01->outstanding_balance),	dbfFilePtr);					//86
	fwrite(pte_tlf01->credit_limit, 1 ,				sizeof(pte_tlf01->credit_limit),	dbfFilePtr);					//86
	fwrite(pte_tlf01->origin, 1 ,					sizeof(pte_tlf01->origin),			dbfFilePtr);						//92
	fwrite(pte_tlf01->interest_rate, 1 ,			sizeof(pte_tlf01->interest_rate),		dbfFilePtr);					//99
	fwrite(pte_tlf01->total_interest, 1 ,			sizeof(pte_tlf01->total_interest),		dbfFilePtr);					//100
	fwrite(pte_tlf01->profile, 1 ,					sizeof(pte_tlf01->profile),			dbfFilePtr);						//103
	fwrite(pte_tlf01->nbr_of_prod_codes, 1 ,		sizeof(pte_tlf01->nbr_of_prod_codes),		dbfFilePtr);				//104
	fwrite(pte_tlf01->product1_code, 1 ,			sizeof(pte_tlf01->product1_code),		dbfFilePtr);					//105
	fwrite(pte_tlf01->product1_quantity, 1 ,		sizeof(pte_tlf01->product1_quantity),		dbfFilePtr);				//106
	fwrite(pte_tlf01->product1_amount, 1 ,			sizeof(pte_tlf01->product1_amount),		dbfFilePtr);					//107
	fwrite(pte_tlf01->visa_merchant_id, 1 ,			sizeof(pte_tlf01->visa_merchant_id),		dbfFilePtr);				//165
	fwrite(pte_tlf01->deferred_factor, 1 ,			sizeof(pte_tlf01->deferred_factor),		dbfFilePtr);					//167
	fwrite(pte_tlf01->deferred_term_length, 1 ,		sizeof(pte_tlf01->deferred_term_length),	dbfFilePtr);				//168
	fwrite(pte_tlf01->currency_code, 1 ,			sizeof(pte_tlf01->currency_code),		dbfFilePtr);					//169
	fwrite(pte_tlf01->transmission_timestamp, 1 ,	sizeof(pte_tlf01->transmission_timestamp),	dbfFilePtr);				//170
	fwrite(pte_tlf01->cvc_data, 1 ,					sizeof(pte_tlf01->cvc_data),			dbfFilePtr);					//172
	fwrite(pte_tlf01->verify_mag_stripe, 1 ,		sizeof(pte_tlf01->verify_mag_stripe),		dbfFilePtr);				//173
	fwrite(pte_tlf01->security_response_code, 1 ,	sizeof(pte_tlf01->security_response_code),	dbfFilePtr);				//175
	fwrite(pte_tlf01->service_code, 1 ,				sizeof(pte_tlf01->service_code),		dbfFilePtr);					//176
	fwrite(pte_tlf01->voice_auth, 1 ,				sizeof(pte_tlf01->voice_auth),			dbfFilePtr);					//179
	fwrite(pte_tlf01->actual_amount, 1 ,			sizeof(pte_tlf01->actual_amount),		dbfFilePtr);					//180
	fwrite(pte_tlf01->reversal_amount, 1 ,			sizeof(pte_tlf01->reversal_amount),		dbfFilePtr);					//181
	fwrite(pte_tlf01->voice_auth_text, 1 ,			sizeof(pte_tlf01->voice_auth_text),		dbfFilePtr);					//182
	fwrite(pte_tlf01->operator_id, 1 ,				sizeof(pte_tlf01->operator_id),		dbfFilePtr);						//183
	fwrite(pte_tlf01->orig_auth_number, 1 ,			sizeof(pte_tlf01->orig_auth_number),		dbfFilePtr);				//185
	fwrite(pte_tlf01->system_date, 1 ,				sizeof(pte_tlf01->system_date),		dbfFilePtr);						//187
	fwrite(pte_tlf01->monthly_amort_amt, 1 ,		sizeof(pte_tlf01->monthly_amort_amt),		dbfFilePtr);				//189
	pte_tlf01->monthly_amort_amt[sizeof(pte_tlf01->monthly_amort_amt) -1] = '\0';
//char sep = 0x20;
//fwrite(&sep, 1, 1, dbfFilePtr);
	return TRUE;
}

void date_to_string(pCHAR datestr)
{
	char yr[5] = {0};
	char mon[4] = {0};
	char day[3] = {0};
	int month = 0;

	char szDate[9] = {0};
	if( datestr == NULL )
		return;
	if( strlen(datestr) < 9 )
		return;
	strncpy(day, datestr, 2);
	strncpy(mon, datestr+3, 3);
	strncpy(yr, datestr+7, 2);

	if(strcmp(mon,"JAN") == 0)
		month = 1;
	else if(strcmp(mon,"FEB") == 0)
		month = 2;
	else if(strcmp(mon,"MAR") == 0)
		month = 3;
	else if(strcmp(mon,"APR") == 0)
		month = 4;
	else if(strcmp(mon,"MAY") == 0)
		month = 5;
	else if(strcmp(mon,"JUN") == 0)
		month = 6;
	else if(strcmp(mon,"JUL") == 0)
		month = 7;
	else if(strcmp(mon,"AUG") == 0)
		month = 8;
	else if(strcmp(mon,"SEP") == 0)
		month = 9;
	else if(strcmp(mon,"OCT") == 0)
		month = 10;
	else if(strcmp(mon,"NOV") == 0)
		month = 11;
	else if(strcmp(mon,"DEC") == 0)
		month = 12;
	
	if(month == 0)
		return;
	sprintf(szDate,"20%2s%02d%2s",yr,month,day);
	strcpy(datestr,szDate);

}
  /*****************************************************************************
  *                                                                            *
  *  Except for main and MainProcessor, functions are in alphabetical order.   *
  *                                                                            *
  *****************************************************************************/
  void ascii_to_bin (pBYTE src, pBYTE dst, INT len)
  {
	  INT  i, j, k;
	  for (i = 0; i < (len*2); i+=2)
	  {
		  /* if I hit a null terminator, quit.  */
		  if (src[i] == '\0')
			  break;
		  /* if these are leading blanks, leave the bcd zero.  */
		  if (src[i] != ' ')
		  {
			  if (src[i] <= '9')
				  dst[i/2] = ((src[i]-'0')*16);
			  else
				  dst[i/2] = ((src[i]-'A')+0x0A)*16;   /* handle 0a-0f */
		  }
		  if (((i+1) < (len*2)) && (src[i+1] != ' '))
		  {
			  if (src[i+1] <= '9')
				  dst[i/2] += (src[i+1]-'0');
			  else
			  {
				  j         = (toupper(src[i+1]) - 'A');
				  k         = j + 0x0A;
				  dst[i/2] += k;
			  }
		  }
	  }
  } /* ascii_to_bin */


INT isnum(pCHAR in_str)
{
   INT i;
   INT length;

   length = (INT)strlen(in_str);

   for (i = 0; i < length; i++)
   {
      if (!isdigit(in_str[i]))
        return(false);
   }
   return(true);
}

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
		  //      sprintf( Buffer, "Shutting down the %s Service, version %s",
		  //               AppName, Version );
			//dbf_arch_log_message(2,1,Buffer,"EndProcess");
		  //      strcat( Buffer, "\n" );
		  //      PRINT( Buffer );
	  }
	  else
	  {
		  EndProcessFlag = true;
	  }
	  return;

  } /* EndProcess */

BOOLEAN CreateTSV(char *filename)
{
		dbfFilePtr = fopen(filename,"rb");
	if(dbfFilePtr != NULL)
	{
		fclose(dbfFilePtr);
		dbfFilePtr = NULL;
		printf("The TSV file %s already exists!. Do you want to overwrite? [Y/N]:");
		char c = getchar();
		if(c != 'y' && c != 'Y')
		{
			return false;
		}
	}
	dbfFilePtr = fopen(filename, "wb");
	if(dbfFilePtr == NULL)
	{
		printf("Unable to create the file.\n");
		return false;
	}
	return WriteTSVHeader();
}

BOOLEAN WriteTSVHeader()
{
	DBF_TLF01 pte_tlf01;

	if(dbfFilePtr != NULL)
	{
		WriteString("transaction_id", sizeof(pte_tlf01.transaction_id), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "transaction_id");					//1   
		WriteString("entry_type", sizeof(pte_tlf01.entry_type), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "entry_type");							//2   
		WriteString("acquirer_id", sizeof(pte_tlf01.acquirer_id), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "acquirer_id");//5
		WriteString("terminal_id", sizeof(pte_tlf01.terminal_id), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "terminal_id");						//7   
		WriteString("response_text", sizeof(pte_tlf01.response_text), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "response_text");						//9   
		WriteString("card_num_len", sizeof(pte_tlf01.card_num_len), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "card_num_len");						//10  
		WriteString("card_num", sizeof(pte_tlf01.card_num), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "card_num");							//11  
		WriteString("exp_date", sizeof(pte_tlf01.exp_date), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "exp_date");							//12  
		WriteString("total_amount", sizeof(pte_tlf01.total_amount), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "total_amount");						//13  
		WriteString("orig_amount", sizeof(pte_tlf01.orig_amount), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "orig_amount");						//16  
		WriteString("auth_number", sizeof(pte_tlf01.auth_number), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "auth_number");						//17  
		WriteString("processing_code", sizeof(pte_tlf01.processing_code), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "processing_code");					//18  
		WriteString("sys_trace_audit_num", sizeof(pte_tlf01.sys_trace_audit_num), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "sys_trace_audit_num");			//19  
		WriteString("orig_sys_trace_num", sizeof(pte_tlf01.orig_sys_trace_num), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "orig_sys_trace_num");				//20  
		WriteString("pos_entry_mode", sizeof(pte_tlf01.pos_entry_mode), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "pos_entry_mode");					//21  
		WriteString("pos_condition_code", sizeof(pte_tlf01.pos_condition_code), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "pos_condition_code");				//23  
		WriteString("retrieval_ref_num", sizeof(pte_tlf01.retrieval_ref_num), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "retrieval_ref_num");				//26  
		WriteString("orig_retrieval_ref_num", sizeof(pte_tlf01.orig_retrieval_ref_num), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "orig_retrieval_ref_num");		//27  
		WriteString("merchant_id", sizeof(pte_tlf01.merchant_id), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "merchant_id");						//28  
		WriteString("orig_message", sizeof(pte_tlf01.orig_message), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "orig_message");						//29  
		WriteString("settlement_total", sizeof(pte_tlf01.settlement_total), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "settlement_total");				//30  
		WriteString("message_type", sizeof(pte_tlf01.message_type), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "message_type");						//31  
		WriteString("card_holder_name", sizeof(pte_tlf01.card_holder_name), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "card_holder_name");			//32          
		WriteString("general_status", sizeof(pte_tlf01.general_status), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "general_status");				//33          
		WriteString("batch_number", sizeof(pte_tlf01.batch_number), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "batch_number");						//36  
		WriteString("response_code", sizeof(pte_tlf01.response_code), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "response_code");						//37  
		WriteString("date_yyyymmdd", sizeof(pte_tlf01.date_yyyymmdd), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "date_yyyymmdd");						//38  
		WriteString("time_hhmmss", sizeof(pte_tlf01.time_hhmmss), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "time_hhmmss");						//39  
		WriteString("tran_start_time", sizeof(pte_tlf01.tran_start_time), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "tran_start_time");				//51          
		WriteString("host_start_time", sizeof(pte_tlf01.host_start_time), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "host_start_time");				//52          
		WriteString("host_finish_time", sizeof(pte_tlf01.host_finish_time), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "host_finish_time");			//53          
		WriteString("tran_finish_time", sizeof(pte_tlf01.tran_finish_time), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "tran_finish_time");			//54          
		WriteString("agent_id", sizeof(pte_tlf01.agent_id), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "agent_id");							//63  
		WriteString("cvc", sizeof(pte_tlf01.cvc), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "cvc");									//64  
		WriteString("settlement_date", sizeof(pte_tlf01.settlement_date), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "settlement_date");				//65          
		WriteString("category_code", sizeof(pte_tlf01.category_code), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "category_code");						//66  
		WriteString("conversion_rate", sizeof(pte_tlf01.conversion_rate), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "conversion_rate");					//67  
		WriteString("product_code", sizeof(pte_tlf01.product_code), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "product_code");						//68  
		WriteString("acquiring_id", sizeof(pte_tlf01.acquiring_id), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "acquiring_id");						//84  
		WriteString("outstanding_balance", sizeof(pte_tlf01.outstanding_balance), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "outstanding_balance");			//85  
		WriteString("credit_limit", sizeof(pte_tlf01.credit_limit), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "credit_limit");						//86  
		WriteString("origin", sizeof(pte_tlf01.origin), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "origin");								//92  
		WriteString("interest_rate", sizeof(pte_tlf01.interest_rate), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "interest_rate");						//99  
		WriteString("total_interest", sizeof(pte_tlf01.total_interest), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "total_interest");					//100 
		WriteString("profile", sizeof(pte_tlf01.profile), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "profile");								//103 
		WriteString("nbr_of_prod_codes", sizeof(pte_tlf01.nbr_of_prod_codes), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "nbr_of_prod_codes");			//104         
		WriteString("product1_code", sizeof(pte_tlf01.product1_code), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "product1_code");						//105 
		WriteString("product1_quantity", sizeof(pte_tlf01.product1_quantity), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "product1_quantity");				//106 
		WriteString("product1_amount", sizeof(pte_tlf01.product1_amount), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "product1_amount");					//107 
		WriteString("visa_merchant_id", sizeof(pte_tlf01.visa_merchant_id), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "visa_merchant_id");				//165 
		WriteString("deferred_factor", sizeof(pte_tlf01.deferred_factor), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "deferred_factor");				//167         
		WriteString("deferred_term_length", sizeof(pte_tlf01.deferred_term_length), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "deferred_term_length");		//168         
		WriteString("currency_code", sizeof(pte_tlf01.currency_code), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "currency_code");					//169         
		WriteString("transmission_timestamp", sizeof(pte_tlf01.transmission_timestamp), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "transmission_timestamp");	//170         
		WriteString("cvc_data", sizeof(pte_tlf01.cvc_data), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "cvc_data");							//172 
		WriteString("verify_mag_stripe", sizeof(pte_tlf01.verify_mag_stripe), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "verify_mag_stripe");				//173 
		WriteString("security_response_code", sizeof(pte_tlf01.security_response_code), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "security_response_code");	//175         
		WriteString("service_code", sizeof(pte_tlf01.service_code), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "service_code");						//176 
		WriteString("voice_auth", sizeof(pte_tlf01.voice_auth), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "voice_auth");						//179         
		WriteString("actual_amount", sizeof(pte_tlf01.actual_amount), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "actual_amount");					//180         
		WriteString("reversal_amount", sizeof(pte_tlf01.reversal_amount), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "reversal_amount");				//181         
		WriteString("voice_auth_text", sizeof(pte_tlf01.voice_auth_text), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "voice_auth_text");				//182         
		WriteString("operator_id", sizeof(pte_tlf01.operator_id), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "operator_id");						//183 
		WriteString("orig_auth_number", sizeof(pte_tlf01.orig_auth_number), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "orig_auth_number");			//185         
		WriteString("system_date", sizeof(pte_tlf01.system_date), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "system_date");						//187 
		WriteString("transaction_type", 75, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "transaction_type");						//187 
		WriteString("Original_Txn_Amount", sizeof(pte_tlf01.total_amount), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Original_Txn_Amount");
		WriteString("Original_Txn_Amt_Currency", sizeof(pte_tlf01.currency_code), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Original_Txn_Amt_Currency");
		WriteString("ECI", sizeof(pte_tlf01.profile), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "ECI");
		WriteString("Invoice_Number", sizeof(pte_tlf01.invoice_number), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Invoice_Number");
		WriteString("POS_Data", sizeof(pte_tlf01.product16_amount) , FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "POS_Data");
		WriteString("Earmarked_Amt", sizeof(pte_tlf01.product9_amount), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Earmarked_Amt");
		WriteString("Earmarked_Currency", sizeof(pte_tlf01.host_start_time), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Earmarked_Currency");
		WriteString("Acquiring_Country", sizeof(pte_tlf01.currency_code), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Acquiring_Country");
		WriteString("Forwarding_Inst_ID", sizeof(pte_tlf01.forwarding_institution_id), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Forwarding_Inst_ID");
		WriteString("Auth_Profile", sizeof(pte_tlf01.system_date), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Auth_Profile");
		WriteString("Gross_Amount", sizeof(pte_tlf01.def_gross_amt), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Gross_Amount");
		WriteString("Monthly_Amortization", sizeof(pte_tlf01.monthly_amort_amt), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Monthly_Amortization");
		WriteString("Term_Length", sizeof(pte_tlf01.deferred_term_length), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Term_Length");
		WriteString("Factor_Rate",sizeof(pte_tlf01.deferred_factor), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Factor_Rate");
		WriteString("Host_Processed",sizeof(pte_tlf01.product1_code), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Host_Processed");
		WriteString("ATP_STIP_Processed", sizeof(pte_tlf01.product1_code), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "ATP_STIP_Processed");
		WriteString("Voice_Override", sizeof(pte_tlf01.tx_key), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Voice_Override");
		WriteString("Override_Merchant_Denial", sizeof(pte_tlf01.nbr_of_prod_codes), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Override_Merchant_Denial");
		WriteString("CUP_Indicator", 13, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "CUP_Indicator");
		WriteString("Action_Code", 11, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Action_Code");
		WriteString("NII",4, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "NII");
		WriteString("Hostname",15, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Hostname");
		WriteString("Scheme_tran_ID", 25, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Scheme_tran_ID");
		WriteString("Orig_ECI_Reas", 13, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Orig_ECI_Reas");
		WriteString("POS_Data_Extd_Codes", 19, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "POS_Data_Extd_Codes");
		WriteString("Offline_validation_flag" , 23, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Offline_validation_flag");
		WriteString("FAI_DECISION_CODE_1" , 32, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "FAI_DECISION_CODE_1");
		WriteString("FAI_DECISION_CODE_2" , 32, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "FAI_DECISION_CODE_2");
		WriteString("FAI_DECISION_CODE_3" , 32, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "FAI_DECISION_CODE_3");
		WriteString("FAI_DECISION_CODE_4" , 32, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "FAI_DECISION_CODE_4");
		WriteString("FAI_DECISION_CODE_5" , 32, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "FAI_DECISION_CODE_5");
		WriteString("FAI_DECISION_CODE_6" , 32, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "FAI_DECISION_CODE_6");
		WriteString("FAI_DECISION_CODE_7", 32, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "FAI_DECISION_CODE_7");
	    WriteString("FAI_DECISION_CODE_8" , 32, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "FAI_DECISION_CODE_8");
		WriteString("FAI_DECISION_CODE_9" , 32, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "FAI_DECISION_CODE_9");
		WriteString("FAI_DECISION_CODE_10" , 32, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "FAI_DECISION_CODE_10");
		WriteString("Issuer_ID" , 20, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Issuer_ID");
		WriteString("ATP_SbatchTime" , 16, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "ATP_SbatchTime");
		WriteString("Merchant_cntry_of_Origin", 24, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Merchant_cntry_of_Origin");
		WriteString("VISA_Extended_Reason_code" , 25, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "VISA_Extended_Reason_code");
		WriteString("VISA_Response_Source_code" , 25, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "VISA_Response_Source_code");
		WriteString("VISA_Merchant_verification_Value" , 32, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "VISA_Merchant_verification_Value");
		WriteString("Terminal_Serial_Number" , 50, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Terminal_Serial_Number");
		//HCTS6
		WriteString("System_Date_Time" , 23, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "System_Date_Time");
		WriteString("Brand_Tran_Type_Identifier" , 26, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Brand_Tran_Type_Identifier");
		WriteString("VISA_F63.3_Message_Reason_Code" , 30, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "VISA_F63.3_Message_Reason_Code");
		WriteString("VISA_F62.1_Auth_Char_Indicator" , 30, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "VISA_F62.1_Auth_Char_Indicator");
		WriteString("VISA_F125_U2_D03_Add_Orig_Data_Elements" , 41, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "VISA_F125_U2_D03_Add_Orig_Data_Elements");

		fprintf(dbfFilePtr, "\n");

	}
	return TRUE;
}

BOOLEAN WriteTSVFooter(long lTotalRecords)
{
	if(dbfFilePtr != NULL)
	{
		fprintf(dbfFilePtr, "[(*)DBF TLF File created by DBF_FILE_ARCHIVE | version %s | Total Records %d]\n", Version, lTotalRecords);
	}
	return TRUE;
}

BOOLEAN WriteTLF01TableIntoTSV(DBF_TLF01 *pte_tlf01, FTLF01 *pte_ftlf01,PTSN01 *pte_ptsn01, char *local_kek)
{
	char str[65] = {0};
	char ch='\n';
	CHAR TRAN_TYPR[128]={0};
	DBF_TLF01 pte_tlf01_l;
	memcpy(&pte_tlf01_l,0x00, sizeof(DBF_TLF01));
	memcpy(&pte_tlf01_l,pte_tlf01, sizeof(DBF_TLF01));
	int temp = strlen(pte_tlf01->nbr_of_prod_codes);
	CHAR     strMPFlag[2]="1";
	double double_deferredfactor = 0;
	char local_deferredfactor[9] = {0};
	int len_deferredfactor = 0;
	BYTE strAmount[13]={0};
	BYTE strGrossAmount[13]={0};
	BYTE action_code[4] = {0};
	BYTE strMonthlyAmortAmount[13]={0};
	int Security_ResponseCode_len=0;

	BYTE visa_extended_reason_code[2]={0};
	BYTE visa_response_source_code[2]={0};

	double double_grossamt;
	int len_grossamt;

	double double_monthly_amort_amt;
	int len_monthly_amort_amt;

	char time_date[25] = {0};

	if(dbfFilePtr == NULL)
	{
		return FALSE;
	}
	if(isnum(pte_tlf01->card_num)== false || strlen(pte_tlf01->card_num)== 0)
	{
		/*Dont write to TSV file*/
		if(strlen(pte_tlf01->card_num)== 0)
		{
			printf("Card number is empty for transaction id: %s\n",pte_tlf01->transaction_id);
		}
		else
		{
			printf("Card number conatins non numeric number for transaction id: %s\n",pte_tlf01->transaction_id);
		}
		return TRUE;
	}
	//Restrict entry type always to be one character.
	if(pte_tlf01->entry_type > 0x09)
	{
		pte_tlf01->entry_type = 0;	//Invalid entry type. 
	}
	RemoveNonAlplaNumSpaceSpecail(pte_tlf01->transaction_id, strlen(pte_tlf01->transaction_id) );
	WriteString(pte_tlf01->transaction_id, sizeof(pte_tlf01->transaction_id), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "transaction_id");
	sprintf(str,"%x",pte_tlf01->entry_type);
	WriteString(str, sizeof(pte_tlf01->entry_type), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "entry_type");
	WriteString(pte_tlf01->acquirer_id, sizeof(pte_tlf01->acquirer_id), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "acquirer_id");
	RemoveNonAlplaNumSpaceSpecail(pte_tlf01->terminal_id, strlen(pte_tlf01->terminal_id) );
	WriteString(pte_tlf01->terminal_id, sizeof(pte_tlf01->terminal_id), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "terminal_id");
	RemoveChar(pte_tlf01->response_text, strlen(pte_tlf01->response_text),ch );
	RemoveNonAlplaNumSpaceSpecail(pte_tlf01->response_text, strlen(pte_tlf01->response_text) );
	WriteString(pte_tlf01->response_text, sizeof(pte_tlf01->response_text), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "response_text");
	WriteString(pte_tlf01->card_num_len, sizeof(pte_tlf01->card_num_len), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "card_num_len");

	memset(str, 0, sizeof(str));
	memcpy(str, pte_tlf01->card_num,64);
	//printf("%s - %d\n",str,strlen(str));
	int i = 0;
	for(i=0;i<sizeof(str);i++)
	{
		if(str[i] == 0x20)
			str[i] = 0x00;
	}
	des_encryption((unsigned char *)str, strlen(str), des_key, 1);
	WriteString(str, sizeof(str), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "card_num");
	WriteString(pte_tlf01->exp_date, sizeof(pte_tlf01->exp_date), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "exp_date");
	WriteString(pte_tlf01->total_amount, sizeof(pte_tlf01->total_amount), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "total_amount");
	WriteString(pte_tlf01->orig_amount, sizeof(pte_tlf01->orig_amount), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "orig_amount");
	RemoveNonAlplaNumSpaceSpecail(pte_tlf01->auth_number, strlen(pte_tlf01->auth_number) );
	WriteString(pte_tlf01->auth_number, sizeof(pte_tlf01->auth_number), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "auth_number");
	WriteString(pte_tlf01->processing_code, sizeof(pte_tlf01->processing_code), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "processing_code");
	WriteString(pte_tlf01->sys_trace_audit_num, sizeof(pte_tlf01->sys_trace_audit_num), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "sys_trace_audit_num");
	WriteString(pte_tlf01->orig_sys_trace_num, sizeof(pte_tlf01->orig_sys_trace_num), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "orig_sys_trace_num");
	WriteString(pte_tlf01->pos_entry_mode, sizeof(pte_tlf01->pos_entry_mode), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "pos_entry_mode");
	WriteString(pte_tlf01->pos_condition_code, sizeof(pte_tlf01->pos_condition_code), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "pos_condition_code");
	WriteString(pte_tlf01->retrieval_ref_num, sizeof(pte_tlf01->retrieval_ref_num), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "retrieval_ref_num");
	WriteString(pte_tlf01->orig_retrieval_ref_num, sizeof(pte_tlf01->orig_retrieval_ref_num), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "orig_retrieval_ref_num");
	RemoveNonAlplaNumSpaceSpecail(pte_tlf01->merchant_id, strlen(pte_tlf01->merchant_id) );
	WriteString(pte_tlf01->merchant_id, sizeof(pte_tlf01->merchant_id), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "merchant_id");
	WriteString(pte_tlf01->orig_message, sizeof(pte_tlf01->orig_message), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "orig_message");
	WriteString(pte_tlf01->settlement_total, sizeof(pte_tlf01->settlement_total), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "settlement_total");
	WriteString(pte_tlf01->message_type, sizeof(pte_tlf01->message_type), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "message_type");
	RemoveNonAlplaNumSpaceSpecail(pte_tlf01->card_holder_name, strlen(pte_tlf01->card_holder_name) );
	WriteString(pte_tlf01->card_holder_name, sizeof(pte_tlf01->card_holder_name), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "card_holder_name");

	memset(str, 0, sizeof(str));
	sprintf(str,"%02d",pte_tlf01->general_status);
	WriteString(str, sizeof(pte_tlf01->general_status), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "general_status");

	WriteString(pte_tlf01->batch_number, sizeof(pte_tlf01->batch_number), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "batch_number");
	WriteString(pte_tlf01->response_code, sizeof(pte_tlf01->response_code), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "response_code");
	date_to_string(pte_tlf01->date_yyyymmdd);
	WriteString(pte_tlf01->date_yyyymmdd, sizeof(pte_tlf01->date_yyyymmdd), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "date_yyyymmdd");
	WriteString(pte_tlf01->time_hhmmss, sizeof(pte_tlf01->time_hhmmss), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "time_hhmmss");
	WriteString(pte_tlf01->tran_start_time, sizeof(pte_tlf01->tran_start_time), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "tran_start_time");
	WriteString(pte_tlf01->host_start_time, sizeof(pte_tlf01->host_start_time), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "host_start_time");
	WriteString(pte_tlf01->host_finish_time, sizeof(pte_tlf01->host_finish_time), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "host_finish_time");
	WriteString(pte_tlf01->tran_finish_time, sizeof(pte_tlf01->tran_finish_time), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "tran_finish_time");
	WriteString(pte_tlf01->agent_id, sizeof(pte_tlf01->agent_id), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "agent_id");

	memset(str, 0, sizeof(str));
	sprintf(str,"%c",pte_tlf01->cvc);
	WriteString(str, sizeof(pte_tlf01->cvc), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "cvc");
	RemoveNonAlplaNumSpaceSpecail(pte_tlf01->settlement_date, strlen(pte_tlf01->settlement_date) );
	WriteString(pte_tlf01->settlement_date, sizeof(pte_tlf01->settlement_date), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "settlement_date");
	WriteString(pte_tlf01->category_code, sizeof(pte_tlf01->category_code), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "category_code");
	WriteString(pte_tlf01->conversion_rate, sizeof(pte_tlf01->conversion_rate), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "conversion_rate");
	WriteString(pte_tlf01->product_code, sizeof(pte_tlf01->product_code), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "product_code");
	WriteString(pte_tlf01->acquiring_id, sizeof(pte_tlf01->acquiring_id), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "acquiring_id");
	WriteString(pte_tlf01->outstanding_balance, sizeof(pte_tlf01->outstanding_balance), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "outstanding_balance");
	WriteString(pte_tlf01->credit_limit, sizeof(pte_tlf01->credit_limit), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "credit_limit");
	WriteString(pte_tlf01->origin, sizeof(pte_tlf01->origin), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "origin");
	WriteString(pte_tlf01->interest_rate, sizeof(pte_tlf01->interest_rate), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "interest_rate");
	WriteString(pte_tlf01->total_interest, sizeof(pte_tlf01->total_interest), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "total_interest");
	RemoveNonAlplaNumSpaceSpecail(pte_tlf01->profile, strlen(pte_tlf01->profile) );
	WriteString(pte_tlf01->profile, sizeof(pte_tlf01->profile), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "profile");
	WriteString(pte_tlf01->nbr_of_prod_codes, sizeof(pte_tlf01->nbr_of_prod_codes), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "nbr_of_prod_codes");
	WriteString(pte_tlf01->product1_code, sizeof(pte_tlf01->product1_code), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "product1_code");
	WriteString(pte_tlf01->product1_quantity, sizeof(pte_tlf01->product1_quantity), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "product1_quantity");
	WriteString(pte_tlf01->product1_amount, sizeof(pte_tlf01->product1_amount), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "product1_amount");
	WriteString(pte_tlf01->visa_merchant_id, sizeof(pte_tlf01->visa_merchant_id), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "visa_merchant_id");

	memcpy(local_deferredfactor,0x00,9);
	len_deferredfactor = strlen(pte_tlf01->deferred_factor);
	strncpy(local_deferredfactor,pte_tlf01->deferred_factor,7);
	if((0 == strncmp(pte_tlf01->product3_quantity,strMPFlag,1)))
	{
	if(len_deferredfactor > 0)
	{
			double_deferredfactor = atof( local_deferredfactor ) / 1000000.0;
			memcpy(local_deferredfactor,0x00,9);
			sprintf( local_deferredfactor, "%8.6f", double_deferredfactor );
			local_deferredfactor[9] = '\0';
		}
		else if(len_deferredfactor == 0)
		{
			strncpy(local_deferredfactor,"0.000000",8);
		}
	}
	else if(len_deferredfactor > 0)
		{
			double_deferredfactor = atof( local_deferredfactor ) / 10000.0;
			memcpy(local_deferredfactor,0x00,9);
			sprintf( local_deferredfactor, "%8.4f", double_deferredfactor );
			local_deferredfactor[9] = '\0';
		}

	WriteString(local_deferredfactor, sizeof(local_deferredfactor), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "deferred_factor");
	WriteString(pte_tlf01->deferred_term_length, sizeof(pte_tlf01->deferred_term_length), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "deferred_term_length");
	WriteString(pte_tlf01->currency_code, sizeof(pte_tlf01->currency_code), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "currency_code");
	WriteString(pte_tlf01->transmission_timestamp, sizeof(pte_tlf01->transmission_timestamp), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "transmission_timestamp");
	WriteString(pte_tlf01->cvc_data, sizeof(pte_tlf01->cvc_data), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "cvc_data");
	WriteString(pte_tlf01->verify_mag_stripe, sizeof(pte_tlf01->verify_mag_stripe), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "verify_mag_stripe");
	Security_ResponseCode_len=strlen(pte_tlf01->security_response_code);
		if(Security_ResponseCode_len==0)
		{
			//If The length is zero just copy the same to have the correct length format.
			WriteString(pte_tlf01->security_response_code, sizeof(pte_tlf01->security_response_code), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "security_response_code");
		}
		else if(Security_ResponseCode_len == 2 && true==isalphanum(pte_tlf01->security_response_code))
		{
			//This is the only valid case, where the length  must be 2 and it must be alphanumeric value.
	WriteString(pte_tlf01->security_response_code, sizeof(pte_tlf01->security_response_code), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "security_response_code");

		}
		else
		{
			//Incase of special characters present in the security response code, Dont copy the value and copy spaces with the same length.
			WriteString("", sizeof(pte_tlf01->security_response_code), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "security_response_code");

		}

	//WriteString(pte_tlf01->security_response_code, sizeof(pte_tlf01->security_response_code), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "security_response_code");
	WriteString(pte_tlf01->service_code, sizeof(pte_tlf01->service_code), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "service_code");
	WriteString(pte_tlf01->voice_auth, sizeof(pte_tlf01->voice_auth), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "voice_auth");
	WriteString(pte_tlf01->actual_amount, sizeof(pte_tlf01->actual_amount), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "actual_amount");
	WriteString(pte_tlf01->reversal_amount, sizeof(pte_tlf01->reversal_amount), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "reversal_amount");
	RemoveChar(pte_tlf01->voice_auth_text, strlen(pte_tlf01->voice_auth_text),ch );
	RemoveNonAlplaNumSpaceSpecail(pte_tlf01->voice_auth_text, strlen(pte_tlf01->voice_auth_text) );
	WriteString(pte_tlf01->voice_auth_text, sizeof(pte_tlf01->voice_auth_text), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "voice_auth_text");
	RemoveNonAlplaNumSpaceSpecail(pte_tlf01->operator_id, strlen(pte_tlf01->operator_id) );
	WriteString(pte_tlf01->operator_id, sizeof(pte_tlf01->operator_id), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "operator_id");
	RemoveNonAlplaNumSpaceSpecail(pte_tlf01->orig_auth_number, strlen(pte_tlf01->orig_auth_number) );
	WriteString(pte_tlf01->orig_auth_number, sizeof(pte_tlf01->orig_auth_number), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "orig_auth_number");
	WriteString(pte_tlf01->system_date, sizeof(pte_tlf01->system_date), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "system_date");
	PrepareTranType(&pte_tlf01_l,TRAN_TYPR);
	WriteString(TRAN_TYPR, 75, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "transaction_type");
	
	WriteString(&pte_tlf01->total_amount, sizeof(pte_tlf01->total_amount), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Original_Txn_Amount");
	WriteString(&pte_tlf01->currency_code, sizeof(pte_tlf01->currency_code), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Original_Txn_Amt_Currency");
	RemoveNonAlplaNumSpaceSpecail(pte_tlf01->profile, strlen(pte_tlf01->profile) );
	WriteString(&pte_tlf01->profile, sizeof(pte_tlf01->profile), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "ECI");
	WriteString(&pte_tlf01->invoice_number, sizeof(pte_tlf01->invoice_number), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Invoice_Number");
	WriteString(&pte_tlf01->product17_amount, sizeof(pte_tlf01->product17_amount), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "POS_Data");
	//WriteString(&pte_tlf01->product9_amount, sizeof(pte_tlf01->product9_amount), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Earmarked_Amt");
	//WriteString(&pte_tlf01->host_start_time, sizeof(pte_tlf01->host_start_time), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Earmarked_Currency");

	if( strcmp( pte_tlf01->host_start_time, "5.000") == 0)
	{
		if (pte_tlf01->product10_amount[0] == '\0')
		{
			WriteString("000000000000", sizeof(pte_tlf01->product10_amount), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Earmarked_Amt");
		}
		else
		{
			WriteString(&pte_tlf01->product10_amount, sizeof(pte_tlf01->product10_amount), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Earmarked_Amt");
		}
		WriteString("PHP", CURRENCY_CODE_LEN, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Earmarked_Currency");
	}
	else if( strcmp(  pte_tlf01->host_start_time, "7.000") == 0)
	{
		if (pte_tlf01->product10_amount[0] == '\0')
		{
			WriteString("000000000000", sizeof(pte_tlf01->product10_amount), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Earmarked_Amt");
		}
		else
		{
			WriteString(&pte_tlf01->product10_amount, sizeof(pte_tlf01->product10_amount), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Earmarked_Amt");
		}
		WriteString("USD", CURRENCY_CODE_LEN, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Earmarked_Currency");
	}
	else if( strcmp(  pte_tlf01->host_start_time, "0.000") == 0)
	{
		if (pte_tlf01->product10_amount[0] == '\0')
		{
			WriteString("000000000000", sizeof(pte_tlf01->product10_amount), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Earmarked_Amt");
		}
		else
		{
			WriteString(&pte_tlf01->product10_amount, sizeof(pte_tlf01->product10_amount), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Earmarked_Amt");
		}
		WriteString(&pte_tlf01->currency_code,sizeof(pte_tlf01->currency_code), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Earmarked_Currency");
	}
	else
	{
		WriteString(&pte_tlf01->total_amount, sizeof(pte_tlf01->total_amount), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Earmarked_Amt");
		WriteString(&pte_tlf01->currency_code,sizeof(pte_tlf01->currency_code), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Earmarked_Currency");
	}


	WriteString(&pte_tlf01->currency_code, sizeof(pte_tlf01->currency_code), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Acquiring_Country");
	WriteString(&pte_tlf01->forwarding_institution_id, sizeof(pte_tlf01->forwarding_institution_id), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Forwarding_Inst_ID");
	WriteString(&pte_tlf01->product4_code, sizeof(pte_tlf01->product4_code), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Auth_Profile");

	double_grossamt = atof(pte_tlf01->def_gross_amt);
	if(pte_tlf01->def_gross_amt[0] != '\0')
	{
		Convert_Amt_To_Auth_Tx_String_Format(double_grossamt, strGrossAmount) ;
		LeftFill_String_With_Character (sizeof(pte_tlf01->def_gross_amt)-1, strGrossAmount,'0') ;
		strGrossAmount[sizeof (pte_tlf01->def_gross_amt)-1] = '\0';
	}
	WriteString(strGrossAmount,sizeof(strGrossAmount), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Gross_Amount");

	double_monthly_amort_amt = atof(pte_tlf01->monthly_amort_amt);
	if(pte_tlf01->monthly_amort_amt[0] != '\0')
	{
		Convert_Amt_To_Auth_Tx_String_Format(double_monthly_amort_amt, strMonthlyAmortAmount) ;
		LeftFill_String_With_Character (sizeof (pte_tlf01->monthly_amort_amt)-1, strMonthlyAmortAmount,'0') ;
		strMonthlyAmortAmount[sizeof (pte_tlf01->monthly_amort_amt)-1] = '\0';
	}
	WriteString(strMonthlyAmortAmount,sizeof(strMonthlyAmortAmount), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Monthly_Amortization");
	WriteString(&pte_tlf01->deferred_term_length, sizeof(pte_tlf01->deferred_term_length), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Term_Length");

	memcpy(local_deferredfactor,0x00,9);
	len_deferredfactor = strlen(pte_tlf01->deferred_factor);
	strncpy(local_deferredfactor,pte_tlf01->deferred_factor,7);
	if((0 == strncmp(pte_tlf01->product3_quantity,strMPFlag,1)))
	{
	if(len_deferredfactor > 0)
	{
			double_deferredfactor = atof( local_deferredfactor ) / 1000000.0;
			memcpy(local_deferredfactor,0x00,9);
			sprintf( local_deferredfactor, "%8.6f", double_deferredfactor );
			local_deferredfactor[9] = '\0';
		}
		else if(len_deferredfactor == 0)
		{
			strncpy(local_deferredfactor,"0.000000",8);
		}
	}
	else if(len_deferredfactor > 0)
		{
			double_deferredfactor = atof( local_deferredfactor ) / 10000.0;
			memcpy(local_deferredfactor,0x00,9);
			sprintf( local_deferredfactor, "%8.4f", double_deferredfactor );
			local_deferredfactor[9] = '\0';
		}

	WriteString(local_deferredfactor,  sizeof(local_deferredfactor), FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Factor_Rate");

	if(strncmp(pte_tlf01->product2_code,STANDIN,6)== 0)
	{
		WriteString("NO", 2, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Host_Processed");
		WriteString("YES", 3, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "ATP_STIP_Processed");
	}
	else
	{
		WriteString("YES", 3, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Host_Processed");
		WriteString("NO", 2, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "ATP_STIP_Processed");
	}

	if (pte_tlf01->tx_key == AUTH_AUTHORIZATION_OVERRIDE || pte_tlf01->tx_key == AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE )
	{
		WriteString("YES", 3, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Voice_Override");
	}
	else
	{
		WriteString("NO", 2, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Voice_Override");
	}


	if ( temp > 0  )
	{
		WriteString("YES", 3, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Override_Merchant_Denial");
	}
	else
	{
		WriteString("NO", 2, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Override_Merchant_Denial");
	}

	WriteString(&pte_tlf01->product_code, 13, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "CUP_Indicator");

	strncpy(action_code,pte_tlf01->product13_code,3);
	WriteString(action_code, 11, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Action_Code");

	WriteString(&pte_tlf01->nii, 4, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "NII");

	WriteString(&pte_tlf01->redemption_amount , 15, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Hostname");

	/*Tranid*/
	if(true == DbfArchive_Check_If_AMEX_Transaction (pte_tlf01))
	{
		if(pte_tlf01->visa_tran_id[0] != '\0')
		{
			WriteString(&pte_tlf01->visa_tran_id /* mcard_banknet*/ , 25, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Scheme_tran_ID");
		}
		else
		{
			WriteString(&pte_tlf01->mcard_banknet /* mcard_banknet*/ , 25, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Scheme_tran_ID");
		}
	}
	else if(true == DbfArchive_Check_If_VISA_Transaction (pte_tlf01))
	{
		WriteString(&pte_tlf01->visa_tran_id , 25, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Scheme_tran_ID");
	}
	else if(true == DbfArchive_Check_If_MC_Transaction (pte_tlf01))
	{
		WriteString(&pte_tlf01->mcard_banknet , 25, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Scheme_tran_ID");
	}
	else if( true == DbfArchive_Check_If_CUP_Transaction (pte_tlf01) ||
			 true == DbfArchive_Check_If_JCB_Transaction (pte_tlf01))
	{
		WriteString("" , 25, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Scheme_tran_ID");
	}
	else
	{/*DCI*/

		RemoveNonAlplaNumSpaceSpecail(pte_tlf01->resp_source, strlen(pte_tlf01->resp_source) );
		WriteString(&pte_tlf01->resp_source , 25, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Scheme_tran_ID");
	}

	WriteString(&pte_tlf01->product15_quantity , 13, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Orig_ECI_Reas");
	if(true == DbfArchive_Check_If_MC_Transaction (pte_tlf01))
	{
		WriteString(&(pte_tlf01->product14_amount[4]) , 19, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "POS_Data_Extd_Codes");
	}
	else
	{
		WriteString("" , 19, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "POS_Data_Extd_Codes");
	}
	WriteString(&pte_tlf01->period , 23, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Offline_validation_flag");
	if(0==strncmp(pte_tlf01->product7_code,"ONUS",4))
	{

		WriteString(&pte_ftlf01->decision_code_1_value , 32, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "FAI_Decision_code_1");
		WriteString(&pte_ftlf01->decision_code_2_value ,32, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "FAI_Decision_code_2");
		WriteString(&pte_ftlf01->decision_code_3_value ,32, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "FAI_Decision_code_3");
		WriteString(&pte_ftlf01->decision_code_4_value , 32, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "FAI_Decision_code_4");
		WriteString(&pte_ftlf01->decision_code_5_value , 32, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "FAI_Decision_code_5");
		WriteString(&pte_ftlf01->decision_code_6_value ,32, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "FAI_Decision_code_6");
		WriteString(&pte_ftlf01->decision_code_7_value , 32, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "FAI_Decision_code_7");
		WriteString(&pte_ftlf01->decision_code_8_value , 32, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "FAI_Decision_code_8");
		WriteString(&pte_ftlf01->decision_code_9_value , 32, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "FAI_Decision_code_9");
		WriteString(&pte_ftlf01->decision_code_10_value , 32, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "FAI_Decision_code_10");

	}
	else
	{
		WriteString("" , 32, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "FAI_Decision_code_1");
		WriteString("" ,32, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "FAI_Decision_code_2");
		WriteString("" ,32, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "FAI_Decision_code_3");
		WriteString("" , 32, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "FAI_Decision_code_4");
		WriteString("", 32, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "FAI_Decision_code_5");
		WriteString("" ,32, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "FAI_Decision_code_6");
		WriteString("" , 32, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "FAI_Decision_code_7");
		WriteString("" , 32, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "FAI_Decision_code_8");
		WriteString("", 32, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "FAI_Decision_code_9");
		WriteString("" , 32, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "FAI_Decision_code_10");
	}
	WriteString(&pte_tlf01->issuer_id , 20, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Issuer_ID");
	WriteString(local_kek , 16, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "ATP_SbatchTime");
	WriteString(&pte_tlf01->orig_auth_number , 24, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Merchant_cntry_of_Origin");
	if(true == DbfArchive_Check_If_VISA_Transaction (pte_tlf01))
	{
		visa_extended_reason_code[0]=pte_tlf01->resp_source[3];
		visa_response_source_code[0]=pte_tlf01->resp_source[0];
		WriteString(visa_extended_reason_code , 1, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "VISA_Extended_Reason_code");
		if(true==isalphanum(pte_tlf01->resp_source))
		{
			WriteString(visa_response_source_code , 1, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "VISA_Response_Source_code");
		}
		else
		{
			WriteString("" , 1, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "VISA_Response_Source_code");
		}
		//HCTS4
		WriteString(&pte_tlf01->product3_amount , 32, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "VISA_Merchant_verification_Value");
	}
	else
	{
		WriteString("" , 25, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "VISA_Extended_Reason_code");
		WriteString("" , 25, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "VISA_Response_Source_code");
		WriteString("" , 32, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "VISA_Merchant_verification_Value");

	}

	if(pte_ptsn01->serial_number[0] != '\0')
	{
		WriteString(&pte_ptsn01->serial_number , 50, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Terminal_Serial_Number");
	}
	else
	{
		WriteString("" , 50, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Terminal_Serial_Number");
	}
	//HCTS6
	//YYYY-MM-DD HH:II:SS.JJJ
	if(strlen(pte_tlf01->system_date) > 0)
	{
		strncpy(time_date, pte_tlf01->system_date, 4);
		strncat(time_date, "-", 1);
		strncat(time_date, pte_tlf01->system_date+4, 2);
		strncat(time_date, "-", 1);
		strncat(time_date, pte_tlf01->system_date+6, 2);
		strncat(time_date, " ", 1);
	}
	if(strlen(pte_tlf01->time_hhmmss) > 0)
	{
		strncat(time_date, pte_tlf01->time_hhmmss, 2);
		strncat(time_date, ":", 1);
		strncat(time_date, pte_tlf01->time_hhmmss+2, 2);
		strncat(time_date, ":", 1);
		strncat(time_date, pte_tlf01->time_hhmmss+4, 2);
		strncat(time_date, pte_tlf01->transaction_id+16, 4);
	}
	if(strlen(time_date) > 0)
    {
		WriteString(time_date , 23, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "System_Date_Time");
    }
	else
	{
		WriteString("" , 23, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "System_Date_Time");
	}

	if(true == DbfArchive_Check_If_MC_Transaction(pte_tlf01) && strcmp(pte_tlf01->message_type, "0100") == 0 && pte_tlf01->product14_code[0] != '\0')
	{
		memset(str, 0, sizeof(str));
		memcpy(str, pte_tlf01->product14_code, 2);
		WriteString(str , 2, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Brand_Tran_Type_Identifier");
	}
	else if(true == DbfArchive_Check_If_VISA_Transaction(pte_tlf01) && strlen(pte_tlf01->product17_amount) >= 10)
	{
		memset(str, 0, sizeof(str));
		str[0] = pte_tlf01->product17_amount[9];
		WriteString(str , 1, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Brand_Tran_Type_Identifier");
	}
	else
	{
		WriteString("" , 26, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "Brand_Tran_Type_Identifier");
	}

	if(true == DbfArchive_Check_If_VISA_Transaction(pte_tlf01))
	{
		if(pte_tlf01->product12_code[0] != '\0')
		{
			memset(str, 0, sizeof(str));
			memcpy(str, pte_tlf01->product12_code, 4);
			WriteString(str , 4, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "VISA_F63.3_Message_Reason_Code");
		}
		else
		{
			WriteString("" , 30, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "VISA_F63.3_Message_Reason_Code");
		}

		if(pte_tlf01->visa_char_indicator != '\0')
		{
			memset(str, 0, sizeof(str));
			str[0] = pte_tlf01->visa_char_indicator;
			WriteString(str , 1, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "VISA_F62.1_Auth_Char_Indicator");
		}
		else
		{
			WriteString("" , 30, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "VISA_F62.1_Auth_Char_Indicator");
		}

		if( (strcmp(pte_tlf01->message_type, "0100") == 0 || strcmp(pte_tlf01->message_type, "0120") == 0) &&
		    (strncmp(pte_tlf01->product4_amount, "F125", 4) == 0)
		  )
		{
			memset(str, 0, sizeof(str));
			strncpy(str, pte_tlf01->product7_amount,  12);
			strncat(str, pte_tlf01->product19_amount, 12);
			strncat(str, pte_tlf01->product20_amount, 12);
			strncat(str, pte_tlf01->product4_amount+4, 5);
			WriteString(str , 41, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "VISA_F125_U2_D03_Add_Orig_Data_Elements");
		}
		else
		{
			WriteString("" , 41, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "VISA_F125_U2_D03_Add_Orig_Data_Elements");
		}
	}
	else
	{
		WriteString("" , 30, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "VISA_F63.3_Message_Reason_Code");
		WriteString("" , 30, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "VISA_F62.1_Auth_Char_Indicator");
		WriteString("" , 41, FILLERCHAR, SEPERATORCHAR, &dbfFilePtr, "VISA_F125_U2_D03_Add_Orig_Data_Elements");
	}

	fprintf(dbfFilePtr,"\n");

	//Increment Total written record count in dbf file;
	Totalwrittenrecordcount = Totalwrittenrecordcount + 1;

	return TRUE;
}

 /*******************************************************************************
*
*  FUNCTION:         RemoveChar
*
*  DESCRIPTION:      This function removes a charector from the string.
*
*  INPUTS:          string - Input string
*                        len  length of the string
*			    ch   char to be removed
*
*  OUTPUTS:          None
*
*  RETURN VALUE:     None
*
*  AUTHOR:           Abhishek
*
*******************************************************************************/

void RemoveChar(char *string, int len, char ch)
{
	char *tmp = (char *) malloc (len);
	memset(tmp, 0, len);
	char *ptr = NULL;
	do
	{
		ptr = strchr(string, ch);
		if(ptr != NULL)
		{
			strncpy(tmp, string, strlen(string)-strlen(ptr));
			strcat(tmp, ptr + 1);
			strcpy(string, tmp);
			memset(tmp, 0, len);
		}
	}while(ptr != NULL);
	free(tmp);
}

/*******************************************************************************
 *
 *  FUNCTION:         RemoveNonAlplaNumSpaceSpecail
 *
 *  DESCRIPTION:      This function removes non alpha and allow few specail numeric
 *                     charector from the string.
 *
 *  INPUTS:          string - Input string
 *                        len  length of the string
 *			    ch   char to be removed
 *
 *  OUTPUTS:          None
 *
 *  RETURN VALUE:     None
 *
 *  AUTHOR:           Abhishek
 *
 *******************************************************************************/

 void RemoveNonAlplaNumSpaceSpecail(pCHAR string, INT length)
 {
    INT i;
    char ch[2]={0};
    char ch_space[2]=" ";
    char *tmp = (char *) malloc (length);
    memset(tmp, 0, length);

    for (i = 0; i < length; i++)
    {
		 if(isalnum(string[i]))
		 {
			ch[0]=string[i];
			strncat(tmp,ch,1);
		 }
		 else if (!isalnum(string[i]))
		 {
			if (string[i] == ' ')
			{
				ch[0]=string[i];
				strncat(tmp,ch,1);
			}
			else if(string[i] == '/')
			{
				ch[0]=string[i];
				strncat(tmp,ch,1);
			}
			else if(string[i] == ',')
			{
				ch[0]=string[i];
				strncat(tmp,ch,1);
			}
			else if(string[i] == '.')
			{
				ch[0]=string[i];
				strncat(tmp,ch,1);
			}
			else if(string[i] == '@')
			{
				ch[0]=string[i];
				strncat(tmp,ch,1);
			}
			else if(string[i] == '#')
			{
				ch[0]=string[i];
				strncat(tmp,ch,1);
			}
			else if(string[i] == '&')
			{
				ch[0]=string[i];
				strncat(tmp,ch,1);
			}
			else if(string[i] == '-')
			{
				ch[0]=string[i];
				strncat(tmp,ch,1);
			}
			else if(string[i] == ':')
			{
				ch[0]=string[i];
				strncat(tmp,ch,1);
			}
			else if(string[i] == ';')
			{
				ch[0]=string[i];
				strncat(tmp,ch,1);
			}
			else if(string[i] == '"')
			{
				ch[0]=string[i];
				strncat(tmp,ch,1);
			}
			else if(string[i] == '\\')
			{
				ch[0]=string[i];
				strncat(tmp,ch,1);
			}
			else if(string[i] == '!')
			{
				ch[0]=string[i];
				strncat(tmp,ch,1);
			}
			else
			{
				strncat(tmp,ch_space,1);
			}
		}
 	}

    memset(string, 0, length);
    strncpy(string,tmp,length);
    free(tmp);
 }

void PrepareTranType (DBF_TLF01 *pte_tlf01, CHAR* TRAN_TYPR)
{
	CHAR m_StrTranTypeFld[40] = {0};
	CHAR strResponse[10] = {0};
	CHAR m_StrCurrencyCodeFld[5] = {0};
	CHAR m_StrAcquiringCountry[5] = {0};
	BYTE  TRAN_KEY;
	CHAR     strMPFlag[2]="1";
	TRAN_KEY =  pte_tlf01->tx_key;
	if ( TRAN_KEY > 100 )
		//strResponse = " RESPONSE";
		strcat(strResponse," RESPONSE");
	
	switch (pte_tlf01->tx_key)
	{
	case AUTH_INVALID:
		strcat(m_StrTranTypeFld,"INVALID");
		strcat(m_StrTranTypeFld,strResponse);
		break;
		
	case AUTH_SALE:
	case AUTH_SALE_RESPONSE:
			strcat(m_StrTranTypeFld,"SALE");
			if((0 == strncmp(pte_tlf01->product3_quantity,strMPFlag,1)))
			{
				strcat(m_StrTranTypeFld," MP");
			}
			strcat(m_StrTranTypeFld,strResponse);
		break;
		
	case  AUTH_REFUND:
	case  AUTH_REFUND_RESPONSE:
		strcat(m_StrTranTypeFld,"REFUND");
		if((0 == strncmp(pte_tlf01->product3_quantity,strMPFlag,1)))
		{
			strcat(m_StrTranTypeFld," MP");
		}
		strcat(m_StrTranTypeFld,strResponse);
		break;
		
	case  AUTH_CASH_ADVANCE:
	case  AUTH_CASH_ADVANCE_RESPONSE:
		strcat(m_StrTranTypeFld,"CASH ADVANCE");
		strcat(m_StrTranTypeFld,strResponse);
		break;
		
	case AUTH_PAYMENT:
	case AUTH_PAYMENT_RESPONSE:
		//m_StrTranTypeFld = "PAYMENT" + strResponse; 
		strcat(m_StrTranTypeFld,"PAYMENT");
		strcat(m_StrTranTypeFld,strResponse);
		break;
		
	case AUTH_OFFLINE_SALE:
	case AUTH_OFFLINE_SALE_RESPONSE:
		//m_StrTranTypeFld = "OFFLINE SALE" + strResponse; 
		strcat(m_StrTranTypeFld,"OFFLINE SALE");
		if((0 == strncmp(pte_tlf01->product3_quantity,strMPFlag,1)))
		{
			//m_StrTranTypeFld = "MP OFFLINE SALE" + strResponse;
			strcat(m_StrTranTypeFld," MP");
		}
		strcat(m_StrTranTypeFld,strResponse);
		break;
		
	case AUTH_OFFLINE_REFUND:
	case AUTH_OFFLINE_REFUND_RESPONSE:
		strcat(m_StrTranTypeFld,"OFFLINE REFUND");
		strcat(m_StrTranTypeFld,strResponse);
		//m_StrTranTypeFld = "OFFLINE REFUND" + strResponse; 
		break;
	case AUTH_OFFLINE_VOID_SALE:
	case AUTH_OFFLINE_VOID_SALE_RESPONSE:
		strcat(m_StrTranTypeFld,"OFFLINE VOID SALE");
		strcat(m_StrTranTypeFld,strResponse);
		break;
		
	case AUTH_SALE_ADJUSTMENT:
	case AUTH_SALE_ADJUSTMENT_RESPONSE:
		//m_StrTranTypeFld = "SALE ADJUSTMENT" + strResponse; 
		strcat(m_StrTranTypeFld,"SALE ADJUSTMENT");
		strcat(m_StrTranTypeFld,strResponse);
		break;
		
	case AUTH_REFUND_ADJUSTMENT:
	case AUTH_REFUND_ADJUSTMENT_RESPONSE:
		//m_StrTranTypeFld = "REFUND ADJUSTMENT" + strResponse; 
		strcat(m_StrTranTypeFld,"REFUND ADJUSTMENT");
		strcat(m_StrTranTypeFld,strResponse);
		break;
		
	case AUTH_PAYMENT_ADJUSTMENT:
	case AUTH_PAYMENT_ADJUSTMENT_RESPONSE:
		//m_StrTranTypeFld = "PAYMENT ADJUSTMENT" + strResponse; 
		strcat(m_StrTranTypeFld,"PAYMENT ADJUSTMENT");
		strcat(m_StrTranTypeFld,strResponse);
		break;
		
	case AUTH_PRE_AUTHORIZATION:
	case AUTH_PRE_AUTHORIZATION_RESPONSE:
		//m_StrTranTypeFld = "PRE AUTHORIZATION" + strResponse; 
		strcat(m_StrTranTypeFld,"PRE AUTHORIZATION");
		strcat(m_StrTranTypeFld,strResponse);
		break;
		
	case AUTH_AUTHORIZATION:
	case AUTH_AUTHORIZATION_RESPONSE:
		{ 
			if ( strcmp (pte_tlf01->processing_code, "180000" ) == 0)
			{
				//m_StrTranTypeFld = "QUASI CASH" + strResponse;
				strcat(m_StrTranTypeFld,"QUASI CASH");
			}
			else if((0 == strncmp(pte_tlf01->product3_quantity,strMPFlag,1)))
			{
				//m_StrTranTypeFld = "DEFERRED PURCHASE AUTHORIZATION RESPONSE";
				strcat(m_StrTranTypeFld,"DEFERRED PURCHASE AUTHORIZATION RESPONSE");
			}
			/*else if(m_StrCardNumberFld[0]== '6')
			{
		
				m_StrTranTypeFld = "CARD VERIFICATION" + strResponse; 
			}*/
			else
			{
				if ( ( strcmp(pte_tlf01->voice_auth, "Y") == 0) ||
					( strcmp(pte_tlf01->voice_auth, "y") == 0) )
				{
					//m_StrTranTypeFld = "VOICE AUTHORIZATION" + strResponse;
					strcat(m_StrTranTypeFld,"VOICE AUTHORIZATION");
				}
				else
				{
					//m_StrTranTypeFld = "AUTHORIZATION" + strResponse; 
					strcat(m_StrTranTypeFld,"AUTHORIZATION");
					
				}
			}
			strcat(m_StrTranTypeFld,strResponse);
		}
		break;
		
	case AUTH_BALANCE_INQUIRY:
	case AUTH_BALANCE_INQUIRY_RESPONSE:
		//m_StrTranTypeFld = "BALANCE INQUIRY" + strResponse; 
		strcat(m_StrTranTypeFld,"BALANCE INQUIRY");
		strcat(m_StrTranTypeFld,strResponse);
		break;
		
	case AUTH_VOID_SALE:
	case AUTH_VOID_SALE_RESPONSE:
		//m_StrTranTypeFld = "VOID SALE" + strResponse;
		strcat(m_StrTranTypeFld,"VOID SALE");
		if((0 == strncmp(pte_tlf01->product3_quantity,strMPFlag,1)))
		{
			//m_StrTranTypeFld = "VOID MP SALE" + strResponse;
			strcat(m_StrTranTypeFld," MP");
		}
		strcat(m_StrTranTypeFld,strResponse);
		break;
		
	case AUTH_VOID_REFUND:
	case AUTH_VOID_REFUND_RESPONSE:
		//m_StrTranTypeFld = "VOID REFUND" + strResponse; 
		strcat(m_StrTranTypeFld,"VOID REFUND");
		strcat(m_StrTranTypeFld,strResponse);
		break;
		
	case AUTH_SETTLE:
	case AUTH_SETTLE_RESPONSE:
		//m_StrTranTypeFld = "SETTLE" + strResponse; 
		strcat(m_StrTranTypeFld,"SETTLE");
		if((0 == strncmp(pte_tlf01->product3_quantity,strMPFlag,1)))
		{
			//m_StrTranTypeFld = "SETTLE MP" + strResponse;
			strcat(m_StrTranTypeFld," MP");
		}
		strcat(m_StrTranTypeFld,strResponse);
		break;
		
	case AUTH_BATCH_UPLOAD:
	case AUTH_BATCH_UPLOAD_RESPONSE:
		//m_StrTranTypeFld = "BATCH UPLOAD" + strResponse; 
		strcat(m_StrTranTypeFld,"BATCH UPLOAD");
		if((0 == strncmp(pte_tlf01->product3_quantity,strMPFlag,1)))
		{
			//m_StrTranTypeFld = "BATCH UPLOAD MP" + strResponse;
			strcat(m_StrTranTypeFld," MP");
			
		}
		strcat(m_StrTranTypeFld,strResponse);
		break;
		
	case AUTH_SETTLE_TRAILER:
	case AUTH_SETTLE_TRAILER_RESPONSE:
		//m_StrTranTypeFld = "SETTLE TRAILER" + strResponse; 
		strcat(m_StrTranTypeFld,"SETTLE TRAILER");
		if((0 == strncmp(pte_tlf01->product3_quantity,strMPFlag,1)))
		{
			//m_StrTranTypeFld = "SETTLE TRAILER MP" + strResponse;
			strcat(m_StrTranTypeFld," MP");
		
		}
		strcat(m_StrTranTypeFld,strResponse);
		break;
		
	case AUTH_STATISTICS:
	case AUTH_STATISTICS_RESPONSE:
		//m_StrTranTypeFld = "STATISTICS" + strResponse; 
		strcat(m_StrTranTypeFld,"STATISTICS");
		strcat(m_StrTranTypeFld,strResponse);
		break;
		
	case AUTH_REVERSAL:
	case AUTH_REVERSAL_RESPONSE:
		//m_StrTranTypeFld = "REVERSAL - " ;
		strcat(m_StrTranTypeFld,"REVERSAL - ");
		if((0 == strncmp(pte_tlf01->product3_quantity,strMPFlag,1)))
		{
			//m_StrTranTypeFld = "REVERSAL MP - " ;
			strcat(m_StrTranTypeFld," MP -");
		}
		
		if( 0 == strncmp(pte_tlf01->product1_quantity, "01",2))
		{
		   //m_StrCurrencyCodeFld = "840";
		   strncpy(m_StrCurrencyCodeFld,"840",3);
//		   m_StrAcquiringCountry = m_StrCurrencyCodeFld;
		   strncpy(m_StrCurrencyCodeFld,m_StrAcquiringCountry,3);
		}
		if ( strcmp (pte_tlf01->processing_code, "700000" ) == 0)
           // m_StrTranTypeFld +="DEFERRED";
			strcat(m_StrTranTypeFld,"DEFERRED");
		else if( strcmp (pte_tlf01->processing_code, "180000" ) == 0)
            //m_StrTranTypeFld +="QUASI CASH";
			strcat(m_StrTranTypeFld,"QUASI CASH");
		else if((strcmp(pte_tlf01->processing_code,"000000") == 0) &&
			(strcmp(pte_tlf01->orig_message, "0100" ) == 0))
            //m_StrTranTypeFld +="AUTHORIZATION";
			strcat(m_StrTranTypeFld,"AUTHORIZATION");
		else if((strcmp(pte_tlf01->processing_code,"000000") == 0) &&
			(strcmp(pte_tlf01->orig_message, "0200" ) == 0))
           // m_StrTranTypeFld +="PURCHASE";
			strcat(m_StrTranTypeFld,"PURCHASE");
		else if((strcmp(pte_tlf01->processing_code,"000000") == 0) &&
			(strcmp(pte_tlf01->orig_message, "0220" ) == 0))
            //m_StrTranTypeFld +="SALE COMPLETION";
			strcat(m_StrTranTypeFld,"SALE COMPLETION");
		else if( strcmp (pte_tlf01->processing_code, "200000" ) == 0) 
           // m_StrTranTypeFld +="REFUND";
			strcat(m_StrTranTypeFld,"REFUND");
		else if( strcmp (pte_tlf01->processing_code, "010000" ) == 0) 
            //m_StrTranTypeFld +="CASH";
			strcat(m_StrTranTypeFld,"CASH");
		else if((strcmp(pte_tlf01->processing_code,"020000") == 0) &&
			(strcmp(pte_tlf01->orig_message, "0200" ) == 0))
            //m_StrTranTypeFld +="VOID SALE";
			strcat(m_StrTranTypeFld,"VOID SALE");
		else if((strcmp(pte_tlf01->processing_code,"220000") == 0) &&
			(strcmp(pte_tlf01->orig_message, "0200" ) == 0))
				//m_StrTranTypeFld +="VOID REFUND";
			strcat(m_StrTranTypeFld,"VOID REFUND");
		else if((strcmp(pte_tlf01->processing_code,"020000") == 0) &&
			(strcmp(pte_tlf01->orig_message, "0220" ) == 0))
            //m_StrTranTypeFld +="SALE ADJUSTMENT";
			strcat(m_StrTranTypeFld,"SALE ADJUSTMENT");
		else if((strcmp(pte_tlf01->processing_code,"220000") == 0) &&
			(strcmp(pte_tlf01->orig_message, "0220" ) == 0))
           // m_StrTranTypeFld +="REFUND";
			strcat(m_StrTranTypeFld,"REFUND");
		if ( pte_tlf01->orig_message[0] == 0x00 )
		{
            /* No original transaction for this reversal. */
           // m_StrTranTypeFld +=" (without updates)";
			strcat(m_StrTranTypeFld," (without updates");
			strcat(m_StrTranTypeFld,strResponse);
		}
		else
            //m_StrTranTypeFld +=" (with updates)";
			strcat(m_StrTranTypeFld," (with updates)");
			strcat(m_StrTranTypeFld,strResponse);
		break;
		
	case AUTH_REVERSAL_REPEAT:
	case AUTH_REVERSAL_REPEAT_RESPONSE:
		strcat(m_StrTranTypeFld,"REVERSAL REPEAT");
		strcat(m_StrTranTypeFld,strResponse);
		break;

	case AUTH_LOGON:
	case AUTH_LOGON_RESPONSE:
		//m_StrTranTypeFld = "LOGON" + strResponse; 
		strcat(m_StrTranTypeFld,"LOGON");
		strcat(m_StrTranTypeFld,strResponse);
		break;
		
	case AUTH_TEST:
	case AUTH_TEST_RESPONSE:
		//m_StrTranTypeFld = "TEST" + strResponse; 
		strcat(m_StrTranTypeFld,"TEST");
		strcat(m_StrTranTypeFld,strResponse);
		break;
		
	case AUTH_PIN_CHANGE:
	case AUTH_PIN_CHANGE_RESPONSE:
		//m_StrTranTypeFld = "PIN CHANGE" + strResponse; 
		strcat(m_StrTranTypeFld,"PIN CHANGE");
		strcat(m_StrTranTypeFld,strResponse);
		break;
		
	case AUTH_VOID_CASH_ADVANCE:
	case AUTH_VOID_CASH_ADVANCE_RESPONSE:
		//m_StrTranTypeFld = "VOID CASH ADVANCE" + strResponse;
		strcat(m_StrTranTypeFld,"VOID CASH ADVANCE");
		strcat(m_StrTranTypeFld,strResponse);		
		break;
		
	case AUTH_SALE_CASH:
	case AUTH_SALE_CASH_RESPONSE:
		//m_StrTranTypeFld = "SALE CASH" + strResponse;
		strcat(m_StrTranTypeFld,"SALE CASH");
		strcat(m_StrTranTypeFld,strResponse);		
		break;
		
	case AUTH_CHECK_VERIFICATION:
	case AUTH_CHECK_VERIFICATION_RESPONSE:
		//m_StrTranTypeFld = "CHECK VERIFICATION" + strResponse; 
		strcat(m_StrTranTypeFld,"CHECK VERIFICATION");
		strcat(m_StrTranTypeFld,strResponse);
		break;
		
	case AUTH_CARD_VERIFICATION:
	case AUTH_CARD_VERIFICATION_RESPONSE:
		//m_StrTranTypeFld = "CARD VERIFICATION" + strResponse; 
		strcat(m_StrTranTypeFld,"CARD VERIFICATION");
		strcat(m_StrTranTypeFld,strResponse);
		break;
		
	case AUTH_VOID_PRE_AUTHORIZATION:
	case AUTH_VOID_PRE_AUTHORIZATION_RESPONSE:
		//m_StrTranTypeFld = "VOID PREAUTHORIZATION" + strResponse;
		strcat(m_StrTranTypeFld,"VOID PREAUTHORIZATION");
		strcat(m_StrTranTypeFld,strResponse);
		break;
		
	case AUTH_SALES_COMPLETION:
	case AUTH_SALES_COMPLETION_RESPONSE:
		//m_StrTranTypeFld = "SALES COMPLETION" + strResponse; 
		strcat(m_StrTranTypeFld,"SALES COMPLETION");
		strcat(m_StrTranTypeFld,strResponse);
		break;
		
	case AUTH_GUARANTEED_LATE_ARRIVAL:
		//m_StrTranTypeFld = "GUARANTEED LATE ARRIVAL" + strResponse; 
		strcat(m_StrTranTypeFld,"GUARANTEED LATE ARRIVAL");
		strcat(m_StrTranTypeFld,strResponse);
		break;
		
	case AUTH_BATCH_DOWN_LINE_LOAD:
	case AUTH_BATCH_DOWN_LINE_LOAD_RESPONSE:
		//m_StrTranTypeFld = "BATCH DOWN LINE LOAD" + strResponse; 
		strcat(m_StrTranTypeFld,"BATCH DOWN LINE LOAD");
		strcat(m_StrTranTypeFld,strResponse);
		break;
		
	case AUTH_SIGNATURE_DATA:
	case AUTH_SIGNATURE_DATA_RESPONSE:
		//m_StrTranTypeFld = "SIGNATURE DATA" + strResponse; 
		strcat(m_StrTranTypeFld,"SIGNATURE DATA");
		strcat(m_StrTranTypeFld,strResponse);
		break;
		
	case AUTH_INITIALIZATION:
	case AUTH_INITIALIZATION_RESPONSE:
		//m_StrTranTypeFld = "INITIALIZATION" + strResponse;
		strcat(m_StrTranTypeFld,"INITIALIZATION");
		strcat(m_StrTranTypeFld,strResponse);		
		break;
		
	case AUTH_PLEASE_WAIT_ADVICE:
	case AUTH_PLEASE_WAIT_ADVICE_RESPONSE:
		//m_StrTranTypeFld = "PLEASE WAIT ADVICE" + strResponse; 
		strcat(m_StrTranTypeFld,"PLEASE WAIT ADVICE");
		strcat(m_StrTranTypeFld,strResponse);
		break;
		
	case AUTH_NEGATIVE_DOWNLOAD:
	case AUTH_NEGATIVE_DOWNLOAD_RESPONSE:
		//m_StrTranTypeFld = "NEGATIVE DOWNLOAD" + strResponse; 
		strcat(m_StrTranTypeFld,"NEGATIVE DOWNLOAD");
		strcat(m_StrTranTypeFld,strResponse);
		break;
		
	case AUTH_ADMIN_ADVICE:
	case AUTH_ADMIN_ADVICE_RESPONSE:
		//m_StrTranTypeFld = "ADMIN ADVICE" + strResponse; 
		strcat(m_StrTranTypeFld,"ADMIN ADVICE");
		strcat(m_StrTranTypeFld,strResponse);
		break;
		
	case AUTH_VOID_PAYMENT:
	case AUTH_VOID_PAYMENT_RESPONSE:
		//m_StrTranTypeFld = "VOID PAYMENT" + strResponse; 
		strcat(m_StrTranTypeFld,"VOID PAYMENT");
		strcat(m_StrTranTypeFld,strResponse);
		break;
		
	case AUTH_REGISTRATION:
	case AUTH_REGISTRATION_RESPONSE:
		//m_StrTranTypeFld = "REGISTRATION" + strResponse; 
		strcat(m_StrTranTypeFld,"REGISTRATION");
		strcat(m_StrTranTypeFld,strResponse);
		break;
		
	case AUTH_CHECK_DL:
	case AUTH_CHECK_DL_RESPONSE:
		//m_StrTranTypeFld = "CHECK DL" + strResponse; 
		strcat(m_StrTranTypeFld,"CHECK DL");
		strcat(m_StrTranTypeFld,strResponse);
		break;
		
	case AUTH_BRANCH_PMT: 
		//m_StrTranTypeFld = "BRANCH PMT" + strResponse;
		strcat(m_StrTranTypeFld,"BRANCH PMT");
		strcat(m_StrTranTypeFld,strResponse);		
		break;
		
	case AUTH_QUASI_CASH:
	case AUTH_QUASI_CASH_RESPONSE: 
		//m_StrTranTypeFld = "QUASI CASH" + strResponse; 
		strcat(m_StrTranTypeFld,"QUASI CASH");
		strcat(m_StrTranTypeFld,strResponse);
		break;
		
	case AUTH_CASH_ADVANCE_AUTHORIZATION:
	case AUTH_CASH_ADVANCE_AUTHORIZATION_RESPONSE: 
		//m_StrTranTypeFld = "CASH ADVANCE AUTHORIZATION" + strResponse; 
		strcat(m_StrTranTypeFld,"CASH ADVANCE AUTHORIZATION");
		strcat(m_StrTranTypeFld,strResponse);
		break;
		
	case AUTH_DEFERRED_PURCHASE_AUTHORIZATION: 
	case AUTH_DEFERRED_PURCHASE_AUTHORIZATION_RESPONSE: 
		//m_StrTranTypeFld = "DEFERRED PURCHASE" + strResponse; 
		strcat(m_StrTranTypeFld,"DEFERRED PURCHASE");
		strcat(m_StrTranTypeFld,strResponse);
		break;
		
	case AUTH_DEFERRED_PURCHASE_AUTHORIZATION_OVERRIDE:
	case AUTH_DEFERRED_PURCHASE_AUTHORIZATION_OVERRIDE_RESPONSE:
		//m_StrTranTypeFld = "DEFERRED PURCHASE OVERRIDE" + strResponse; 
		strcat(m_StrTranTypeFld,"DEFERRED PURCHASE OVERRIDE");
		strcat(m_StrTranTypeFld,strResponse);
		break;
		
	case AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE:
	case AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE_RESPONSE:
		//m_StrTranTypeFld = "CASH ADVANCE AUTHORIZATION OVERRIDE" + strResponse;
		strcat(m_StrTranTypeFld,"CASH ADVANCE AUTHORIZATION OVERRIDE");
		strcat(m_StrTranTypeFld,strResponse);
		break;
		
	case AUTH_AUTHORIZATION_OVERRIDE:
	case AUTH_AUTHORIZATION_OVERRIDE_RESPONSE:
		//m_StrTranTypeFld = "AUTHORIZATION OVERRIDE" + strResponse;
		strcat(m_StrTranTypeFld,"AUTHORIZATION OVERRIDE");
		strcat(m_StrTranTypeFld,strResponse);		
		break;
		
	case AUTH_ADVICE:
	case AUTH_ADVICE_RESPONSE:
		//m_StrTranTypeFld = "ADVICE" + strResponse; 
		strcat(m_StrTranTypeFld,"ADVICE");
		strcat(m_StrTranTypeFld,strResponse);
		break;
		
	case AUTH_REVERSAL_ADVICE:
	case AUTH_REVERSAL_ADVICE_RESPONSE:
		//m_StrTranTypeFld = "REVERSAL ADVICE" + strResponse; 
		strcat(m_StrTranTypeFld,"REVERSAL ADVICE");
		strcat(m_StrTranTypeFld,strResponse);
		break;
		
	case AUTH_CARD_UPDATE:
	case AUTH_CARD_UPDATE_RESPONSE:
		//m_StrTranTypeFld = "CARD UPDATE" + strResponse; 
		strcat(m_StrTranTypeFld,"CARD UPDATE");
		strcat(m_StrTranTypeFld,strResponse);
		break;
		
	case AUTH_RELOAD:
	case AUTH_RELOAD_RESPONSE:
		//m_StrTranTypeFld = "CASH LOAD" + strResponse;
		strcat(m_StrTranTypeFld,"CASH LOAD");
		strcat(m_StrTranTypeFld,strResponse);
		break;
		
	case AUTH_RELOAD_REPEAT:
	case AUTH_RELOAD_REPEAT_RESPONSE:
		//m_StrTranTypeFld = "CASH LOAD REPEAT" + strResponse;
		strcat(m_StrTranTypeFld,"CASH LOAD REPEAT");
		strcat(m_StrTranTypeFld,strResponse);
		break;
		
	case AUTH_RELOAD_CONFIRM:
	case AUTH_RELOAD_CONFIRM_RESPONSE:
		//m_StrTranTypeFld = "CASH LOAD CONFIRM" + strResponse;
		strcat(m_StrTranTypeFld,"CASH LOAD CONFIRM");
		strcat(m_StrTranTypeFld,strResponse);
		break;
		
	case AUTH_VOID_RELOAD:
	case AUTH_VOID_RELOAD_RESPONSE:
		//m_StrTranTypeFld = "CASH LOAD VOID" + strResponse;
		strcat(m_StrTranTypeFld,"CASH LOAD VOID");
		strcat(m_StrTranTypeFld,strResponse);
		break;
		
	case AUTH_EMV_ADVICE:
	case AUTH_EMV_ADVICE_RESPONSE:
		//m_StrTranTypeFld = "EMV SCRIPT RESULTS" + strResponse;
		strcat(m_StrTranTypeFld,"EMV SCRIPT RESULTS");
		strcat(m_StrTranTypeFld,strResponse);
		break;
		
	case AUTH_EMV_UPLOAD:
	case AUTH_EMV_UPLOAD_RESPONSE:
		//m_StrTranTypeFld = "TC UPLOAD" + strResponse;
		strcat(m_StrTranTypeFld,"TC UPLOAD");
		strcat(m_StrTranTypeFld,strResponse);
		break;

	case AUTH_ATM_CONFIRM:
		//m_StrTranTypeFld = "REVERSAL" + strResponse; 
		strcat(m_StrTranTypeFld,"REVERSAL");
		strcat(m_StrTranTypeFld,strResponse);
	break;
   }
   
   strcpy(TRAN_TYPR,m_StrTranTypeFld);
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
   CHAR db_nullchk_flag[5] = {0};
   CHAR  dbf_file_archive_error_warning_file_size[5] = {0};

   /* Get path to the tf.ini file. */
   memset( tmpstr, 0x00, sizeof(tmpstr) );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, "tf.ini" );

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
							  dbf_file_archive_error_warning_file_size,              	/* points to destination buffer   */
							  sizeof(dbf_file_archive_error_warning_file_size)-1,   	 /* size of destination buffer     */
							  filename                  /* points to ini filename         */
							);

	/* File size size conversion */
	Max_File_Size_Defined = atoi(dbf_file_archive_error_warning_file_size);
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

/******************************************************************************
 *
 *  NAME:         dbf_arch_log_message
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
void dbf_arch_log_message( INT dest, INT sev, pCHAR msg, pCHAR func )
{
   INT  monitor_flag;
   INT  eventlog_flag;
   INT  msg_type;
   BYTE severity;
   BYTE log_type[20]={0};
   CHAR text_message[512]={0};

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
   dbf_arch_Log_error_warning_to_File(text_message,sev,func);
   /* Call function to post the message. */
   if(monitor_flag == 1)
   {
	   /* as of now passing null instead of auth info's..
    	* later it can be improved if required
    	* */

		   TxUtils_Send_Msg_To_Operator( monitor_flag, text_message,
		   										 eventlog_flag, msg_type, func,
		   										 severity, log_type,
		   										 NULL, NULL,NULL );

   }
   return;
}

INT dbf_arch_Log_error_warning_to_File(pCHAR Error_Buf,int sev,pCHAR func)
{
	INT   len=0;
	INT   path_len = 0;
	CHAR  time_date[25]  ={0};
	CHAR  current_mmdd[5]={0};
	CHAR Buffer[1024]={0};
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
		return 0;
	}
	fseek (fp, 0, SEEK_END);
	file_size=ftell (fp);

	if( file_size >= Max_File_Size_Defined )
	{
		sprintf(buf,"ERROR_WARNING_FILE  size is exceeding the permissible size, need attention" );
		TxUtils_Send_Msg_To_Operator( 1, buf,1, ALERT_MSG, "", 4, INFO_ERROR,NULL,NULL,NULL );
	}
	fclose(fp);
	return 0;
}

int Convert_Amt_To_Auth_Tx_String_Format
                              (double nAmount, char* strAmount)
{
	char strTemp[256] = {0} ;
	int  nStringLength = 0 ;
	int  nIndex = 0 ;

	gcvt (nAmount, 13, strTemp) ; // choice of 15 is arbitrary

	nStringLength = strlen (strTemp) ;
	// Discard all characters after the decimal point, if present
	for (nIndex = 0; nIndex < nStringLength ; nIndex++)
	{
		if ('.' == strTemp[nIndex])
		{
			strTemp[nIndex] = 0 ;
			break ;
		}
	}

	strcpy (strAmount, strTemp) ;
	return 1 ;
} /* Convert_Amt_To_Auth_Tx_String_Format */

void LeftFill_String_With_Character (unsigned int destination_array_size,
                                     char* strAmount,
                                     unsigned char fill_character)
{
   unsigned char strTemp[256] = {0} ;
   int len = 0 ;
   memset (strTemp, fill_character, sizeof strTemp) ;

   len = strlen (strAmount) ;
   strTemp [destination_array_size - len] = 0 ;
   strcat (strTemp, strAmount) ;
   strncpy (strAmount, strTemp,destination_array_size) ;
}

INT DbfArchive_Check_If_CUP_Transaction (DBF_TLF01 *pte_tlf01)
{
	if (0 == strncmp(pte_tlf01->product_code,CUP_CARD_INDICATOR_FLAG,2) ||
	    0 == strncmp(pte_tlf01->acquirer_id,"nccup",5) ||
		0 == strncmp(pte_tlf01->issuer_id,"nccup",5)||
		0 == strncmp(pte_tlf01->authorizing_host_queue,"nccup",5) )
	{
		return true ;
	}
	else
	{
		return false;
	}
}

INT DbfArchive_Check_If_VISA_Transaction (DBF_TLF01 *pte_tlf01)
{
	if ( pte_tlf01->card_num[0]=='4' )
	{
		return true ;
	}
	else
	{
		return false;
	}
}
INT DbfArchive_Check_If_MC_Transaction (DBF_TLF01 *pte_tlf01)
{
	char MC_bin_series[7]   = {0};
	int  MC_Bin_series_low  = 222100;
	int  MC_Bin_series_high = 272099;
	int  MC_bin_value 		= 0;
	int  MC_Bin_serice_len  = 6 ;

	strncpy(MC_bin_series,pte_tlf01->card_num,MC_Bin_serice_len);
	MC_bin_value = atoi(MC_bin_series);

	if(	pte_tlf01->card_num[0]=='5')
	{
		return true ;
	}
	else if( (MC_bin_value >= MC_Bin_series_low) && (MC_bin_value <= MC_Bin_series_high))
	{
		return true;
	}
	else
	{
		return false;
	}
}
INT DbfArchive_Check_If_JCB_Transaction (DBF_TLF01 *pte_tlf01)
{
	if((pte_tlf01->card_num[0]=='3') &&
	   (pte_tlf01->card_num[1] == '5'))
	{
		return true ;
	}
	else
	{
		return false;
	}
}
INT DbfArchive_Check_If_AMEX_Transaction (DBF_TLF01 *pte_tlf01)
{
	if((pte_tlf01->card_num[0]=='3') &&
	   ((pte_tlf01->card_num[1] == '4') ||
		(pte_tlf01->card_num[1] == '7')))
	{
		return true ;
	}
	else
	{
		return false;
	}
}
INT isalphanum(pCHAR string)
{
   INT i;
   INT length;

   length = (INT)strlen(string);

   for (i = 0; i < length; i++)
   {
      if (!isalnum(string[i]))
        return(false);
   }
   return(true);
}

BOOLEAN DBF_Archive_Check_is_Acquiring_Transaction(DBF_TLF01 *pte_tlf01)
{
	if(strncmp(pte_tlf01->handler_queue,"dcpiso",6)==0 ||
	strncmp(pte_tlf01->handler_queue,"dcpimp",6)==0 ||
	strncmp(pte_tlf01->handler_queue,"dcpos",5)==0)

	{
		return(true);
	}
	else
	{
		return(false);
	}

}


void dbf_read_falconrt_flag_for_transactions()
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
									  "TXCNTL_FALCONRT_FLAG",       				/* points to key name             */
									   "0",  						/* Default string                 */
									   reload_falcon_flag,              		/* points to destination buffer   */
									   sizeof(reload_falcon_flag)-1,   	 	/* size of destination buffer     */
									   filename                  	/* points to ini filename         */
							 	   );
	   Falcon_RT_Flag = atoi(reload_falcon_flag);

	   memset( reload_falcon_flag, 0x00, sizeof(reload_falcon_flag) );
}



void dbf_read_falconrt_flag_for_MP_transactions()
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
	   Dcpimp_RT_Flag = atoi(reload_falcon_flag);

	   memset( reload_falcon_flag, 0x00, sizeof(reload_falcon_flag) );
}
