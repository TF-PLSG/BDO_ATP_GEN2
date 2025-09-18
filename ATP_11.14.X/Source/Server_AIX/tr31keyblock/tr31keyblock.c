
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
#include "nc_database.h"
#include "nc_dbstruct.h"
#include "memmnger.h"
#include "dstimer.h"
#include "txutils.h"
#include "genutil.h"
#include "timings.h"

#include "aes_256.h"

CAF01   		caf01;
NCF01   		ncf01;
CAF01_KEYBLOCK 	caf01_keyblock;
NCF01_KEYBLOCK 	ncf01_keyblock;

CHAR    app_name[256];

static CHAR         	 path[30] = "";

static CHAR  			 CurrentDate[9];
CHAR          Version[] = "ATP_11.12.0";

#define   MAX_FILENAME_SIZE  80
#define   MAX_INI_INFO_SIZE  20

#define NUM_SIZE_WRITES  1

#define sDEFAULT_DS_ERROR_STATICS_VALUE  "0.5"

/* TF INI File Name */
#define  TF_INI_FILE   "tf.ini"

#define SECRET_KEY_FOR_AES_ENCRYPTION_IN_KEYBLOCK_FOR_NETWORK_PROFILE			"12345678912345678912345678912345"
#define HEADER_FOR_KEYBLOCK_FOR_NETWORK_PROFILE                                 "1234567891234567"
#define SECRET_KEY_FOR_AES_ENCRYPTION_IN_KEYBLOCK_FOR_CARD_PROFILE		    	"12345678912345678912345678912345"
#define HEADER_FOR_KEYBLOCK_FOR_CARD_PROFILE                                    "1234567891234567"

INT DB_NullCheck_Flag;

/* For DB statics added by raghu */
BYTE 	DB_MsgSubtype2 ;
BYTE 	DB_MsgSubtype1 ;
CHAR 	DB_Logging_Filename[256];
CHAR  	DB_module_file_name[256];
CHAR  	DB_file_path[256];
INT 	DB_NullCheck_Flag;

UINT Max_File_Size_Defined = 0 ;

CHAR Secret_Key_for_AES_Encryption_in_Keyblock_DCI[33]={0};
CHAR Secret_Key_for_AES_Encryption_in_Keyblock_VISA[33]={0};
CHAR Secret_Key_for_AES_Encryption_in_Keyblock_MC[33]={0};
CHAR Secret_Key_for_AES_Encryption_in_Keyblock_JCB[33]={0};
CHAR Secret_Key_for_AES_Encryption_in_Keyblock_AMEX[33]={0};
CHAR Secret_Key_for_AES_Encryption_in_Keyblock_UPI[33]={0};

CHAR header_for_Keyblock_DCI[17]={0};
CHAR header_for_Keyblock_VISA[17]={0};
CHAR header_for_Keyblock_MC[17]={0};
CHAR header_for_Keyblock_JCB[17]={0};
CHAR header_for_Keyblock_AMEX[17]={0};
CHAR header_for_Keyblock_UPI[17]={0};
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
	printf("\n:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n");
	printf("\n::::::::Key Encryption,Preparing Keyblock and Insert it to DB::::::::\n");
	printf("\nChoose Command Line parameters for KeyBlock Encryption:-   \n");
	printf("1. CAF01 Command Syntax:-[tool] [1] [Crad brand] [Profile_ID]   \n");
	printf("CAF01 Example command:[ tr31keyblock 1 DINR ] \n");
	printf("Options and Arguments for [ tr31keyblock 1 DCI DINR ]:\n");
	printf("[tr31keyblock: tool] \t['1':Indicates CAF01] \t[DCI:Indicates card brand ] \t[DINR: Indicates card profile ID ]\n");
	printf("\n[Crad brand] should be given as below:\n");
	printf("[1.DCI\n2.VISA\n3.MC\n4.JCB\n5.AMEX\n6.UPI\n\n");
	printf("2. NCF01 Command Syntax:-  [tool] [2] [I/A] [Network ID]                  \n");
	printf("NCF01 Example command:[ tr31keyblock 2 I ncdci3 ]            \n");
	printf("NCF01 Example command:[ tr31keyblock 2 A ncdci2 ]            \n");
	printf("[tr31keyblock: tool] \t['2':Indicates NCF01] \t[I/A: Indicates I-for Issuing Page A-for Acquiring Page]\t[ncdci3/ncdci:Indicates Network profile ID ]\n");
	printf("\n\nTR-31 Keyblock Tool Version: %s\n\n", Version       	);
	printf("\n:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n");

}

/******************************************************************************/

INT get_SecretKey_for_KeyBlock_AES_Encryption_for_network_profile(void)
{
   CHAR  secret_key[33]={0};
   CHAR  date[9]={0};
   INT   ret_val = true;
   CHAR  filename   [80] = {0};
   CHAR  tmpstr     [80] = {0};

   /* Get path to the tf.ini file.*/
   memset( tmpstr, 0x00, sizeof(tmpstr) );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, "tf.ini" );
//For DINERS
   ret_val = GetPrivateProfileString(
									  "SECRET_KEY_FOR_AES_ENCRYPTION_IN_KEYBLOCK_FOR_NETWORK_PROFILE",             /* points to section name         */
									  "SECRET_KEY_FOR_AES_ENCRYPTION_IN_KEYBLOCK_DCI",       	/* points to key name             */
									  "123456789",  					/* Default string                 */
									  secret_key,              	/* points to destination buffer   */
									  sizeof(secret_key)-1,   	 /* size of destination buffer     */
									  filename                  /* points to ini filename         */
									);

	if(strlen(secret_key)==0)
	{
	   strncpy( Secret_Key_for_AES_Encryption_in_Keyblock_DCI,
			   SECRET_KEY_FOR_AES_ENCRYPTION_IN_KEYBLOCK_FOR_NETWORK_PROFILE,
				32 );
	   printf("secret key From tf.ini-%s\n",Secret_Key_for_AES_Encryption_in_Keyblock_DCI);
	}
	else
	{
		strncpy(Secret_Key_for_AES_Encryption_in_Keyblock_DCI,secret_key, 32);
		printf("secret key From tf.ini-%s\n",Secret_Key_for_AES_Encryption_in_Keyblock_DCI);
	}
	//for VISA
	ret_val = GetPrivateProfileString(
										  "SECRET_KEY_FOR_AES_ENCRYPTION_IN_KEYBLOCK_FOR_NETWORK_PROFILE",             /* points to section name         */
										  "SECRET_KEY_FOR_AES_ENCRYPTION_IN_KEYBLOCK_VISA",       	/* points to key name             */
										  "123456789",  					/* Default string                 */
										  secret_key,              	/* points to destination buffer   */
										  sizeof(secret_key)-1,   	 /* size of destination buffer     */
										  filename                  /* points to ini filename         */
										);

		if(strlen(secret_key)==0)
		{
		   strncpy( Secret_Key_for_AES_Encryption_in_Keyblock_VISA,
				   SECRET_KEY_FOR_AES_ENCRYPTION_IN_KEYBLOCK_FOR_NETWORK_PROFILE,
					32 );
		   printf("secret key From tf.ini-%s\n",Secret_Key_for_AES_Encryption_in_Keyblock_VISA);
		}
		else
		{
			strncpy(Secret_Key_for_AES_Encryption_in_Keyblock_VISA,secret_key, 32);
			printf("secret key From tf.ini-%s\n",Secret_Key_for_AES_Encryption_in_Keyblock_VISA);
		}
    //For MC
	ret_val = GetPrivateProfileString(
										  "SECRET_KEY_FOR_AES_ENCRYPTION_IN_KEYBLOCK_FOR_NETWORK_PROFILE",             /* points to section name         */
										  "SECRET_KEY_FOR_AES_ENCRYPTION_IN_KEYBLOCK_MC",       	/* points to key name             */
										  "123456789",  					/* Default string                 */
										  secret_key,              	/* points to destination buffer   */
										  sizeof(secret_key)-1,   	 /* size of destination buffer     */
										  filename                  /* points to ini filename         */
										);

		if(strlen(secret_key)==0)
		{
		   strncpy( Secret_Key_for_AES_Encryption_in_Keyblock_MC,
				   SECRET_KEY_FOR_AES_ENCRYPTION_IN_KEYBLOCK_FOR_NETWORK_PROFILE,
					32 );
		   printf("secret key From tf.ini-%s\n",Secret_Key_for_AES_Encryption_in_Keyblock_MC);
		}
		else
		{
			strncpy(Secret_Key_for_AES_Encryption_in_Keyblock_MC,secret_key, 32);
			printf("secret key From tf.ini-%s\n",Secret_Key_for_AES_Encryption_in_Keyblock_MC);
		}
	//For JCB
	ret_val = GetPrivateProfileString(
										  "SECRET_KEY_FOR_AES_ENCRYPTION_IN_KEYBLOCK_FOR_NETWORK_PROFILE",             /* points to section name         */
										  "SECRET_KEY_FOR_AES_ENCRYPTION_IN_KEYBLOCK_JCB",       	/* points to key name             */
										  "123456789",  					/* Default string                 */
										  secret_key,              	/* points to destination buffer   */
										  sizeof(secret_key)-1,   	 /* size of destination buffer     */
										  filename                  /* points to ini filename         */
										);

		if(strlen(secret_key)==0)
		{
		   strncpy( Secret_Key_for_AES_Encryption_in_Keyblock_JCB,
				   SECRET_KEY_FOR_AES_ENCRYPTION_IN_KEYBLOCK_FOR_NETWORK_PROFILE,
					32 );
		   printf("secret key From tf.ini-%s\n",Secret_Key_for_AES_Encryption_in_Keyblock_JCB);
		}
		else
		{
			strncpy(Secret_Key_for_AES_Encryption_in_Keyblock_JCB,secret_key, 32);
			printf("secret key From tf.ini-%s\n",Secret_Key_for_AES_Encryption_in_Keyblock_JCB);
		}
//For AMEX
	ret_val = GetPrivateProfileString(
										  "SECRET_KEY_FOR_AES_ENCRYPTION_IN_KEYBLOCK_FOR_NETWORK_PROFILE",             /* points to section name         */
										  "SECRET_KEY_FOR_AES_ENCRYPTION_IN_KEYBLOCK_AMEX",       	/* points to key name             */
										  "123456789",  					/* Default string                 */
										  secret_key,              	/* points to destination buffer   */
										  sizeof(secret_key)-1,   	 /* size of destination buffer     */
										  filename                  /* points to ini filename         */
										);

		if(strlen(secret_key)==0)
		{
		   strncpy( Secret_Key_for_AES_Encryption_in_Keyblock_AMEX,
				   SECRET_KEY_FOR_AES_ENCRYPTION_IN_KEYBLOCK_FOR_NETWORK_PROFILE,
					32 );
		   printf("secret key From tf.ini-%s\n",Secret_Key_for_AES_Encryption_in_Keyblock_AMEX);
		}
		else
		{
			strncpy(Secret_Key_for_AES_Encryption_in_Keyblock_AMEX,secret_key, 32);
			printf("secret key From tf.ini-%s\n",Secret_Key_for_AES_Encryption_in_Keyblock_AMEX);
		}
//For UPI
	ret_val = GetPrivateProfileString(
										  "SECRET_KEY_FOR_AES_ENCRYPTION_IN_KEYBLOCK_FOR_NETWORK_PROFILE",             /* points to section name         */
										  "SECRET_KEY_FOR_AES_ENCRYPTION_IN_KEYBLOCK_UPI",       	/* points to key name             */
										  "123456789",  					/* Default string                 */
										  secret_key,              	/* points to destination buffer   */
										  sizeof(secret_key)-1,   	 /* size of destination buffer     */
										  filename                  /* points to ini filename         */
										);

		if(strlen(secret_key)==0)
		{
		   strncpy( Secret_Key_for_AES_Encryption_in_Keyblock_UPI,
				   SECRET_KEY_FOR_AES_ENCRYPTION_IN_KEYBLOCK_FOR_NETWORK_PROFILE,
					32 );
		   printf("secret key for UPI From tf.ini-%s\n",Secret_Key_for_AES_Encryption_in_Keyblock_UPI);
		}
		else
		{
			strncpy(Secret_Key_for_AES_Encryption_in_Keyblock_UPI,secret_key, 32);
			printf("secret key for UPI From tf.ini-%s\n",Secret_Key_for_AES_Encryption_in_Keyblock_UPI);
		}
	return (ret_val);
}

INT get_Header_for_KeyBlock_for_network_profile(void)
{
   CHAR  keyblock_header[17]={0};
   CHAR  date[9]={0};
   INT   ret_val = true;
   CHAR  filename   [80] = {0};
   CHAR  tmpstr     [80] = {0};

   /* Get path to the tf.ini file.*/
   memset( tmpstr, 0x00, sizeof(tmpstr) );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, "tf.ini" );

   ret_val = GetPrivateProfileString(
									  "HEADER_FOR_KEYBLOCK_FOR_NETWORK_PROFILE",             /* points to section name         */
									  "HEADER_FOR_KEYBLOCK_DCI",       	/* points to key name             */
									  "D123456789",  					/* Default string                 */
									  keyblock_header,              	/* points to destination buffer   */
									  sizeof(keyblock_header)-1,   	 /* size of destination buffer     */
									  filename                  /* points to ini filename         */
									);

	if(strlen(keyblock_header)==0)
	{
	   strncpy( header_for_Keyblock_DCI,
			   HEADER_FOR_KEYBLOCK_FOR_NETWORK_PROFILE,
				16 );
	   printf("Key Block Header for DCI From tf.ini-%s\n",header_for_Keyblock_DCI);
	}
	else
	{
		strncpy(header_for_Keyblock_DCI,keyblock_header, 16);
		printf("Key Block Header for DCI From tf.ini-%s\n",header_for_Keyblock_DCI);
	}
	//for VISA
	ret_val = GetPrivateProfileString(
										  "HEADER_FOR_KEYBLOCK_FOR_NETWORK_PROFILE",             /* points to section name         */
										  "HEADER_FOR_KEYBLOCK_VISA",       	/* points to key name             */
										  "D123456789",  					/* Default string                 */
										  keyblock_header,              	/* points to destination buffer   */
										  sizeof(keyblock_header)-1,   	 /* size of destination buffer     */
										  filename                  /* points to ini filename         */
										);

		if(strlen(keyblock_header)==0)
		{
		   strncpy( header_for_Keyblock_VISA,
				   HEADER_FOR_KEYBLOCK_FOR_NETWORK_PROFILE,
					16 );
		   printf("Key Block Header for VISA From tf.ini-%s\n",header_for_Keyblock_VISA);
		}
		else
		{
			strncpy(header_for_Keyblock_VISA,keyblock_header, 16);
			printf("Key Block Header for VISA From tf.ini-%s\n",header_for_Keyblock_VISA);
		}
		//for MC
			ret_val = GetPrivateProfileString(
												  "HEADER_FOR_KEYBLOCK_FOR_NETWORK_PROFILE",             /* points to section name         */
												  "HEADER_FOR_KEYBLOCK_MC",       	/* points to key name             */
												  "D123456789",  					/* Default string                 */
												  keyblock_header,              	/* points to destination buffer   */
												  sizeof(keyblock_header)-1,   	 /* size of destination buffer     */
												  filename                  /* points to ini filename         */
												);

				if(strlen(keyblock_header)==0)
				{
				   strncpy( header_for_Keyblock_MC,
						   HEADER_FOR_KEYBLOCK_FOR_NETWORK_PROFILE,
							16 );
				   printf("Key Block Header for MC From tf.ini-%s\n",header_for_Keyblock_MC);
				}
				else
				{
					strncpy(header_for_Keyblock_MC,keyblock_header, 16);
					printf("Key Block Header for MC From tf.ini-%s\n",header_for_Keyblock_MC);
				}
		//for JCB
			ret_val = GetPrivateProfileString(
												  "HEADER_FOR_KEYBLOCK_FOR_NETWORK_PROFILE",             /* points to section name         */
												  "HEADER_FOR_KEYBLOCK_JCB",       	/* points to key name             */
												  "D123456789",  					/* Default string                 */
												  keyblock_header,              	/* points to destination buffer   */
												  sizeof(keyblock_header)-1,   	 /* size of destination buffer     */
												  filename                  /* points to ini filename         */
												);

				if(strlen(keyblock_header)==0)
				{
				   strncpy( header_for_Keyblock_JCB,
						   HEADER_FOR_KEYBLOCK_FOR_NETWORK_PROFILE,
							16 );
				   printf("Key Block Header for JCB From tf.ini-%s\n",header_for_Keyblock_JCB);
				}
				else
				{
					strncpy(header_for_Keyblock_JCB,keyblock_header, 16);
					printf("Key Block Header for JCB From tf.ini-%s\n",header_for_Keyblock_JCB);
				}
		//for AMEX
			ret_val = GetPrivateProfileString(
												  "HEADER_FOR_KEYBLOCK_FOR_NETWORK_PROFILE",             /* points to section name         */
												  "HEADER_FOR_KEYBLOCK_AMEX",       	/* points to key name             */
												  "D123456789",  					/* Default string                 */
												  keyblock_header,              	/* points to destination buffer   */
												  sizeof(keyblock_header)-1,   	 /* size of destination buffer     */
												  filename                  /* points to ini filename         */
												);

				if(strlen(keyblock_header)==0)
				{
				   strncpy( header_for_Keyblock_AMEX,
						   HEADER_FOR_KEYBLOCK_FOR_NETWORK_PROFILE,
							16 );
				   printf("Key Block Header for AMEX From tf.ini-%s\n",header_for_Keyblock_AMEX);
				}
				else
				{
					strncpy(header_for_Keyblock_AMEX,keyblock_header, 16);
					printf("Key Block Header for AMEX From tf.ini-%s\n",header_for_Keyblock_AMEX);
				}
		//for UPI
			ret_val = GetPrivateProfileString(
												  "HEADER_FOR_KEYBLOCK_FOR_NETWORK_PROFILE",             /* points to section name         */
												  "HEADER_FOR_KEYBLOCK_UPI",       	/* points to key name             */
												  "D123456789",  					/* Default string                 */
												  keyblock_header,              	/* points to destination buffer   */
												  sizeof(keyblock_header)-1,   	 /* size of destination buffer     */
												  filename                  /* points to ini filename         */
												);

				if(strlen(keyblock_header)==0)
				{
				   strncpy( header_for_Keyblock_UPI,
						   HEADER_FOR_KEYBLOCK_FOR_NETWORK_PROFILE,
							16 );
				   printf("Key Block Header for UPI From tf.ini-%s\n",header_for_Keyblock_UPI);
				}
				else
				{
					strncpy(header_for_Keyblock_UPI,keyblock_header, 16);
					printf("Key Block Header for UPI From tf.ini-%s\n",header_for_Keyblock_UPI);
				}
	return (ret_val);
}
INT get_SecretKey_for_KeyBlock_AES_Encryption_for_card_profile(void)
{
   CHAR  secret_key[33]={0};
   CHAR  date[9]={0};
   INT   ret_val = true;
   CHAR  filename   [80] = {0};
   CHAR  tmpstr     [80] = {0};

   /* Get path to the tf.ini file.*/
   memset( tmpstr, 0x00, sizeof(tmpstr) );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, "tf.ini" );
//For DINERS
   ret_val = GetPrivateProfileString(
									  "SECRET_KEY_FOR_AES_ENCRYPTION_IN_KEYBLOCK_FOR_CARD_PROFILE",             /* points to section name         */
									  "SECRET_KEY_FOR_AES_ENCRYPTION_IN_KEYBLOCK_DCI",       	/* points to key name             */
									  "123456789",  					/* Default string                 */
									  secret_key,              	/* points to destination buffer   */
									  sizeof(secret_key)-1,   	 /* size of destination buffer     */
									  filename                  /* points to ini filename         */
									);

	if(strlen(secret_key)==0)
	{
	   strncpy( Secret_Key_for_AES_Encryption_in_Keyblock_DCI,
			   SECRET_KEY_FOR_AES_ENCRYPTION_IN_KEYBLOCK_FOR_CARD_PROFILE,
				32 );
	   printf("secret key for DCI From tf.ini-%s\n",Secret_Key_for_AES_Encryption_in_Keyblock_DCI);
	}
	else
	{
		strncpy(Secret_Key_for_AES_Encryption_in_Keyblock_DCI,secret_key, 32);
		printf("secret key for DCI From tf.ini-%s\n",Secret_Key_for_AES_Encryption_in_Keyblock_DCI);
	}
	//For VISA
	ret_val = GetPrivateProfileString(
										  "SECRET_KEY_FOR_AES_ENCRYPTION_IN_KEYBLOCK_FOR_CARD_PROFILE",             /* points to section name         */
										  "SECRET_KEY_FOR_AES_ENCRYPTION_IN_KEYBLOCK_VISA",       	/* points to key name             */
										  "123456789",  					/* Default string                 */
										  secret_key,              	/* points to destination buffer   */
										  sizeof(secret_key)-1,   	 /* size of destination buffer     */
										  filename                  /* points to ini filename         */
										);

		if(strlen(secret_key)==0)
		{
		   strncpy( Secret_Key_for_AES_Encryption_in_Keyblock_VISA,
				   SECRET_KEY_FOR_AES_ENCRYPTION_IN_KEYBLOCK_FOR_CARD_PROFILE,
					32 );
		   printf("secret key for VISA From tf.ini-%s\n",Secret_Key_for_AES_Encryption_in_Keyblock_VISA);
		}
		else
		{
			strncpy(Secret_Key_for_AES_Encryption_in_Keyblock_VISA,secret_key, 32);
			printf("secret key for VISA From tf.ini-%s\n",Secret_Key_for_AES_Encryption_in_Keyblock_VISA);
		}
	//For MC
	ret_val = GetPrivateProfileString(
										  "SECRET_KEY_FOR_AES_ENCRYPTION_IN_KEYBLOCK_FOR_CARD_PROFILE",             /* points to section name         */
										  "SECRET_KEY_FOR_AES_ENCRYPTION_IN_KEYBLOCK_MC",       	/* points to key name             */
										  "123456789",  					/* Default string                 */
										  secret_key,              	/* points to destination buffer   */
										  sizeof(secret_key)-1,   	 /* size of destination buffer     */
										  filename                  /* points to ini filename         */
										);

		if(strlen(secret_key)==0)
		{
		   strncpy( Secret_Key_for_AES_Encryption_in_Keyblock_MC,
				   SECRET_KEY_FOR_AES_ENCRYPTION_IN_KEYBLOCK_FOR_CARD_PROFILE,
					32 );
		   printf("secret key for MC From tf.ini-%s\n",Secret_Key_for_AES_Encryption_in_Keyblock_MC);
		}
		else
		{
			strncpy(Secret_Key_for_AES_Encryption_in_Keyblock_MC,secret_key, 32);
			printf("secret key for MC From tf.ini-%s\n",Secret_Key_for_AES_Encryption_in_Keyblock_MC);
		}
	//For JCB
	ret_val = GetPrivateProfileString(
										  "SECRET_KEY_FOR_AES_ENCRYPTION_IN_KEYBLOCK_FOR_CARD_PROFILE",             /* points to section name         */
										  "SECRET_KEY_FOR_AES_ENCRYPTION_IN_KEYBLOCK_JCB",       	/* points to key name             */
										  "123456789",  					/* Default string                 */
										  secret_key,              	/* points to destination buffer   */
										  sizeof(secret_key)-1,   	 /* size of destination buffer     */
										  filename                  /* points to ini filename         */
										);

		if(strlen(secret_key)==0)
		{
		   strncpy( Secret_Key_for_AES_Encryption_in_Keyblock_JCB,
				   SECRET_KEY_FOR_AES_ENCRYPTION_IN_KEYBLOCK_FOR_CARD_PROFILE,
					32 );
		   printf("secret key for JCB From tf.ini-%s\n",Secret_Key_for_AES_Encryption_in_Keyblock_JCB);
		}
		else
		{
			strncpy(Secret_Key_for_AES_Encryption_in_Keyblock_JCB,secret_key, 32);
			printf("secret key for JCB From tf.ini-%s\n",Secret_Key_for_AES_Encryption_in_Keyblock_JCB);
		}
	//For AMEX
	ret_val = GetPrivateProfileString(
										  "SECRET_KEY_FOR_AES_ENCRYPTION_IN_KEYBLOCK_FOR_CARD_PROFILE",             /* points to section name         */
										  "SECRET_KEY_FOR_AES_ENCRYPTION_IN_KEYBLOCK_AMEX",       	/* points to key name             */
										  "123456789",  					/* Default string                 */
										  secret_key,              	/* points to destination buffer   */
										  sizeof(secret_key)-1,   	 /* size of destination buffer     */
										  filename                  /* points to ini filename         */
										);

		if(strlen(secret_key)==0)
		{
		   strncpy( Secret_Key_for_AES_Encryption_in_Keyblock_AMEX,
				   SECRET_KEY_FOR_AES_ENCRYPTION_IN_KEYBLOCK_FOR_CARD_PROFILE,
					32 );
		   printf("secret key for AMEX From tf.ini-%s\n",Secret_Key_for_AES_Encryption_in_Keyblock_AMEX);
		}
		else
		{
			strncpy(Secret_Key_for_AES_Encryption_in_Keyblock_AMEX,secret_key, 32);
			printf("secret key for AMEX From tf.ini-%s\n",Secret_Key_for_AES_Encryption_in_Keyblock_AMEX);
		}
	//For UPI
	ret_val = GetPrivateProfileString(
										  "SECRET_KEY_FOR_AES_ENCRYPTION_IN_KEYBLOCK_FOR_CARD_PROFILE",             /* points to section name         */
										  "SECRET_KEY_FOR_AES_ENCRYPTION_IN_KEYBLOCK_UPI",       	/* points to key name             */
										  "123456789",  					/* Default string                 */
										  secret_key,              	/* points to destination buffer   */
										  sizeof(secret_key)-1,   	 /* size of destination buffer     */
										  filename                  /* points to ini filename         */
										);

		if(strlen(secret_key)==0)
		{
		   strncpy( Secret_Key_for_AES_Encryption_in_Keyblock_UPI,
				   SECRET_KEY_FOR_AES_ENCRYPTION_IN_KEYBLOCK_FOR_CARD_PROFILE,
					32 );
		   printf("secret key for UPI From tf.ini-%s\n",Secret_Key_for_AES_Encryption_in_Keyblock_UPI);
		}
		else
		{
			strncpy(Secret_Key_for_AES_Encryption_in_Keyblock_UPI,secret_key, 32);
			printf("secret key for UPI From tf.ini-%s\n",Secret_Key_for_AES_Encryption_in_Keyblock_UPI);
		}

	return (ret_val);
}
INT get_Header_for_KeyBlock_for_card_profile(void)
{
   CHAR  keyblock_header[17]={0};
   CHAR  date[9]={0};
   INT   ret_val = true;
   CHAR  filename   [80] = {0};
   CHAR  tmpstr     [80] = {0};

   /* Get path to the tf.ini file.*/
   memset( tmpstr, 0x00, sizeof(tmpstr) );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, "tf.ini" );

   ret_val = GetPrivateProfileString(
									  "HEADER_FOR_KEYBLOCK_FOR_CARD_PROFILE",             /* points to section name         */
									  "HEADER_FOR_KEYBLOCK_DCI",       	/* points to key name             */
									  "D123456789",  					/* Default string                 */
									  keyblock_header,              	/* points to destination buffer   */
									  sizeof(keyblock_header)-1,   	 /* size of destination buffer     */
									  filename                  /* points to ini filename         */
									);

	if(strlen(keyblock_header)==0)
	{
	   strncpy( header_for_Keyblock_DCI,
			   HEADER_FOR_KEYBLOCK_FOR_CARD_PROFILE,
				16 );
	   printf("Key Block Header for DCI From tf.ini-%s\n",header_for_Keyblock_DCI);
	}
	else
	{
		strncpy(header_for_Keyblock_DCI,keyblock_header, 16);
		printf("Key Block Header for DCI From tf.ini-%s\n",header_for_Keyblock_DCI);
	}
	//For VISA
	ret_val = GetPrivateProfileString(
										  "HEADER_FOR_KEYBLOCK_FOR_CARD_PROFILE",             /* points to section name         */
										  "HEADER_FOR_KEYBLOCK_VISA",       	/* points to key name             */
										  "D123456789",  					/* Default string                 */
										  keyblock_header,              	/* points to destination buffer   */
										  sizeof(keyblock_header)-1,   	 /* size of destination buffer     */
										  filename                  /* points to ini filename         */
										);

		if(strlen(keyblock_header)==0)
		{
		   strncpy( header_for_Keyblock_VISA,
				   HEADER_FOR_KEYBLOCK_FOR_CARD_PROFILE,
					16 );
		   printf("Key Block Header for VISA From tf.ini-%s\n",header_for_Keyblock_VISA);
		}
		else
		{
			strncpy(header_for_Keyblock_VISA,keyblock_header, 16);
			printf("Key Block Header for VISAFrom tf.ini-%s\n",header_for_Keyblock_VISA);
		}
		//For MC
		ret_val = GetPrivateProfileString(
											  "HEADER_FOR_KEYBLOCK_FOR_CARD_PROFILE",             /* points to section name         */
											  "HEADER_FOR_KEYBLOCK_MC",       	/* points to key name             */
											  "D123456789",  					/* Default string                 */
											  keyblock_header,              	/* points to destination buffer   */
											  sizeof(keyblock_header)-1,   	 /* size of destination buffer     */
											  filename                  /* points to ini filename         */
											);

			if(strlen(keyblock_header)==0)
			{
			   strncpy( header_for_Keyblock_MC,
					   HEADER_FOR_KEYBLOCK_FOR_CARD_PROFILE,
						16 );
			   printf("Key Block Header for MC From tf.ini-%s\n",header_for_Keyblock_MC);
			}
			else
			{
				strncpy(header_for_Keyblock_MC,keyblock_header, 16);
				printf("Key Block Header for MC From tf.ini-%s\n",header_for_Keyblock_MC);
			}

		//For JCB
		ret_val = GetPrivateProfileString(
											  "HEADER_FOR_KEYBLOCK_FOR_CARD_PROFILE",             /* points to section name         */
											  "HEADER_FOR_KEYBLOCK_JCB",       	/* points to key name             */
											  "D123456789",  					/* Default string                 */
											  keyblock_header,              	/* points to destination buffer   */
											  sizeof(keyblock_header)-1,   	 /* size of destination buffer     */
											  filename                  /* points to ini filename         */
											);

			if(strlen(keyblock_header)==0)
			{
			   strncpy( header_for_Keyblock_JCB,
					   HEADER_FOR_KEYBLOCK_FOR_CARD_PROFILE,
						16 );
			   printf("Key Block Header for JCB From tf.ini-%s\n",header_for_Keyblock_JCB);
			}
			else
			{
				strncpy(header_for_Keyblock_JCB,keyblock_header, 16);
				printf("Key Block Header for JCB From tf.ini-%s\n",header_for_Keyblock_JCB);
			}

		//For AMEX
		ret_val = GetPrivateProfileString(
											  "HEADER_FOR_KEYBLOCK_FOR_CARD_PROFILE",             /* points to section name         */
											  "HEADER_FOR_KEYBLOCK_AMEX",       	/* points to key name             */
											  "D123456789",  					/* Default string                 */
											  keyblock_header,              	/* points to destination buffer   */
											  sizeof(keyblock_header)-1,   	 /* size of destination buffer     */
											  filename                  /* points to ini filename         */
											);

			if(strlen(keyblock_header)==0)
			{
			   strncpy( header_for_Keyblock_AMEX,
					   HEADER_FOR_KEYBLOCK_FOR_CARD_PROFILE,
						16 );
			   printf("Key Block Header for AMEX From tf.ini-%s\n",header_for_Keyblock_AMEX);
			}
			else
			{
				strncpy(header_for_Keyblock_AMEX,keyblock_header, 16);
				printf("Key Block Header for AMEX From tf.ini-%s\n",header_for_Keyblock_AMEX);
			}

		//For UPI
		ret_val = GetPrivateProfileString(
											  "HEADER_FOR_KEYBLOCK_FOR_CARD_PROFILE",             /* points to section name         */
											  "HEADER_FOR_KEYBLOCK_UPI",       	/* points to key name             */
											  "D123456789",  					/* Default string                 */
											  keyblock_header,              	/* points to destination buffer   */
											  sizeof(keyblock_header)-1,   	 /* size of destination buffer     */
											  filename                  /* points to ini filename         */
											);

			if(strlen(keyblock_header)==0)
			{
			   strncpy( header_for_Keyblock_UPI,
					   HEADER_FOR_KEYBLOCK_FOR_CARD_PROFILE,
						16 );
			   printf("Key Block Header for UPI From tf.ini-%s\n",header_for_Keyblock_UPI);
			}
			else
			{
				strncpy(header_for_Keyblock_UPI,keyblock_header, 16);
				printf("Key Block Header for UPI From tf.ini-%s\n",header_for_Keyblock_UPI);
			}

	return (ret_val);
}

void byte_to_hex_string(unsigned char byte, char* hex_str) {
    snprintf(hex_str, 3, "%02X", byte);  // Convert byte to hex and store as a string
}

INT ncf01_key_block_encryption(INT input_mode, BYTE input_primary_id[10], pCHAR arg )
{
	char ErrorMsg[256] = {0};
	char key_str[32];
	char iv_str[16];
	uint8_t key[AES_KEY_SIZE];
	uint8_t iv[AES_BLOCK_SIZE];

	BYTE input[32];
	uint8_t mac[8];
	size_t padded_len;
	uint8_t *padded_input;
	char keyblock_output[256];
	char encrypted_key[128];
	BYTE mac_auth[16];
	char mac_temp[8];
	char hex_str[3];
	char output[150];
	CHAR header[16];

	if(input_mode == 2)
	{
		if(input_primary_id!='\0')
		{
			if (strcmp(arg, "I") == 0)
			{
				//for source_key ,ISS_CWK_Keyblock
				if((ncf01.cwk !='\0')){
					if (strcmp(input_primary_id, "ncdci2") == 0 ||
							strcmp(input_primary_id, "ncdci3") == 0||
							strcmp(input_primary_id, "ncdci") == 0) {
						memset(header,0,sizeof(header));
						memcpy(header, header_for_Keyblock_DCI, 16);
						memcpy(key_str, Secret_Key_for_AES_Encryption_in_Keyblock_DCI, 32);
						key_str[32]='\0';

					}
					else if (strcmp(input_primary_id, "ncmcr3") == 0 ||
							strcmp(input_primary_id, "ncmcr2") == 0||
							strcmp(input_primary_id, "ncmcrd") == 0) {
							memset(header,0,sizeof(header));
							memcpy(header, header_for_Keyblock_MC, 16);
							memcpy(key_str, Secret_Key_for_AES_Encryption_in_Keyblock_MC, 32);
							key_str[32]='\0';
					}
					else if (strcmp(input_primary_id, "ncvis3") == 0 ||
							strcmp(input_primary_id, "ncvis2") == 0||
							strcmp(input_primary_id, "ncvisa") == 0) {
							memset(header,0,sizeof(header));
							memcpy(header, header_for_Keyblock_VISA, 16);
							memcpy(key_str, Secret_Key_for_AES_Encryption_in_Keyblock_VISA, 32);
							key_str[32]='\0';
					}
					else if (strcmp(input_primary_id, "ncjcb3") == 0 ||
							strcmp(input_primary_id, "ncjcb2") == 0||
							strcmp(input_primary_id, "ncjcb") == 0) {
							memset(header,0,sizeof(header));
							memcpy(header, header_for_Keyblock_JCB, 16);
							memcpy(key_str, Secret_Key_for_AES_Encryption_in_Keyblock_JCB, 32);
							key_str[32]='\0';
					}
					else if (strcmp(input_primary_id, "ncpos") == 0 ||
							strcmp(input_primary_id, "nposa2") == 0||
							strcmp(input_primary_id, "ncposa") == 0) {
							memset(header,0,sizeof(header));
							memcpy(header, header_for_Keyblock_AMEX, 16);
							memcpy(key_str, Secret_Key_for_AES_Encryption_in_Keyblock_AMEX, 32);
							key_str[32]='\0';
					}
					else if (strcmp(input_primary_id, "nccup3") == 0 ||
							strcmp(input_primary_id, "nccup2") == 0||
							strcmp(input_primary_id, "nccup") == 0) {
							memset(header,0,sizeof(header));
							memcpy(header, header_for_Keyblock_UPI, 16);
							memcpy(key_str, Secret_Key_for_AES_Encryption_in_Keyblock_UPI, 32);
							key_str[32]='\0';
					}

					//NCF01
					printf("\n::::::::::::::::::::::::::::::::::::::::NCF01:Encryption and Keyblock::::::::::::::::::::::::::::::::::::::::\n");
					memset(input,0,sizeof(input));
					memcpy(input, ncf01.cwk, 32);
					input[32]='\0';
					size_t input_len = sizeof(input);

					memcpy(key, key_str, AES_KEY_SIZE);
					memcpy(iv, header, AES_BLOCK_SIZE);
					pkcs7_pad(input, input_len, &padded_input, &padded_len);
					uint8_t *encrypted = malloc(padded_len);

					if (encrypted == NULL) {
						perror("Memory allocation failed");
						exit(EXIT_FAILURE);
					}

					/*AES 256 Encryption in CBC Mode*/
					aes_encrypt_cbc(input, key, iv, encrypted, input_len);

					for (size_t i = 0; i < padded_len; i++) {
					sprintf((char *)(encrypted_key + i * 2), "%02x", encrypted[i]);
					}
					printf("\n");

					/* MAC generation using CMAC*/
					compute_cmac(encrypted, padded_len, key, mac);

					for (int i = 0; i < 8; i++) {byte_to_hex_string(mac[i], hex_str);  // Convert each byte to hex string
						mac_auth[i * 2] = hex_str[0];         // Store the first hex character as raw byte
						mac_auth[i * 2 + 1] = hex_str[1];     // Store the second hex character as raw byte
					}
					mac_auth[16]='\0';
					/*
					printf("\nHeader:%s\n",header);
					printf("Encrypted Key:%s\n",encrypted_key);
					printf("MAC:%s\n",mac_auth);*/

					/*Preparing KeyBlock */
					memset (&output,0,sizeof(output));
					//printf("\n[NCF01] Keyblock (Header+Encrypted Key data+ MAC) :\n");
					memcpy(output, header, 16);
					memcpy(output+16, encrypted_key, 96);
					memcpy(output+16+96, mac_auth, 8);
					output[128]="\0";
					//printf("[NCF01]Key Block output: %s\n", output);
					/* Insert into DB */

					memcpy(ncf01_keyblock.iss_cwk_keyblock, output, sizeof(ncf01_keyblock.iss_cwk_keyblock));
					//printf("ISS_CWK_Keyblock:%s",ncf01_keyblock.iss_cwk_keyblock);
					if(output!='\0' && ncf01_keyblock.iss_cwk_keyblock!='\0'){
					printf("ISS_CWK_Keyblock: Migrating to Keyblock Successful!");
					}
					else
					{
						printf("ISS_CWK_Keyblock: Migrating to Keyblock Failed!!");
					}
					free(encrypted);
					free(padded_input);
				}
			}
			if (strcmp(arg, "A") == 0)
			{
				//for source_key ,ACQ_CWK_Keyblock
				if((ncf01.cwk !='\0')){
					if (strcmp(input_primary_id, "ncdci2") == 0 ||
							strcmp(input_primary_id, "ncdci") == 0) {
							memset(header,0,sizeof(header));
							memcpy(header, header_for_Keyblock_DCI, 16);
							memcpy(key_str, Secret_Key_for_AES_Encryption_in_Keyblock_DCI, 32);
							key_str[32]='\0';
					}
					else if ( strcmp(input_primary_id, "ncmcr2") == 0||
								strcmp(input_primary_id, "ncmcrd") == 0) {
								memset(header,0,sizeof(header));
								memcpy(header, header_for_Keyblock_MC, 16);
								memcpy(key_str, Secret_Key_for_AES_Encryption_in_Keyblock_MC, 32);
								key_str[32]='\0';
					}
					else if ( strcmp(input_primary_id, "ncvis2") == 0||
								strcmp(input_primary_id, "ncvisa") == 0) {
								memset(header,0,sizeof(header));
								memcpy(header, header_for_Keyblock_VISA, 16);
								memcpy(key_str, Secret_Key_for_AES_Encryption_in_Keyblock_VISA, 32);
								key_str[32]='\0';
					}
					else if (strcmp(input_primary_id, "ncjcb2") == 0||
								strcmp(input_primary_id, "ncjcb") == 0) {
								memset(header,0,sizeof(header));
								memcpy(header, header_for_Keyblock_JCB, 16);
								memcpy(key_str, Secret_Key_for_AES_Encryption_in_Keyblock_JCB, 32);
								key_str[32]='\0';
					}
					else if (strcmp(input_primary_id, "nposa2") == 0||
								strcmp(input_primary_id, "ncposa") == 0) {
								memset(header,0,sizeof(header));
								memcpy(header, header_for_Keyblock_AMEX, 16);
								memcpy(key_str, Secret_Key_for_AES_Encryption_in_Keyblock_AMEX, 32);
								key_str[32]='\0';
					}
					if ( strcmp(input_primary_id, "nccup2") == 0||
								strcmp(input_primary_id, "nccup") == 0) {
								memset(header,0,sizeof(header));
								memcpy(header, header_for_Keyblock_UPI, 16);
								memcpy(key_str, Secret_Key_for_AES_Encryption_in_Keyblock_UPI, 32);
								key_str[32]='\0';
					}
					//NCF01
					printf("\n::::::::::::::::::::::::::::::::::::::::NCF01:Encryption and Keyblock::::::::::::::::::::::::::::::::::::::::\n");
					memset(input,0,sizeof(input));
					memcpy(input, ncf01.cwk, 32);
					input[32]='\0';
					size_t input_len = sizeof(input);

					memcpy(key, key_str, AES_KEY_SIZE);
					memcpy(iv, header, AES_BLOCK_SIZE);
					pkcs7_pad(input, input_len, &padded_input, &padded_len);
					uint8_t *encrypted = malloc(padded_len);

					if (encrypted == NULL) {
						perror("Memory allocation failed");
						exit(EXIT_FAILURE);
					}

					/*AES 256 Encryption in CBC Mode*/
					aes_encrypt_cbc(input, key, iv, encrypted, input_len);

					for (size_t i = 0; i < padded_len; i++) {
					sprintf((char *)(encrypted_key + i * 2), "%02x", encrypted[i]);
					}
					printf("\n");

					/* MAC generation using CMAC*/
					compute_cmac(encrypted, padded_len, key, mac);

					for (int i = 0; i < 8; i++) {byte_to_hex_string(mac[i], hex_str);  // Convert each byte to hex string
						mac_auth[i * 2] = hex_str[0];         // Store the first hex character as raw byte
						mac_auth[i * 2 + 1] = hex_str[1];     // Store the second hex character as raw byte
					}
					mac_auth[16]='\0';
					/*printf("\nHeader:%s\n",header);
					printf("Encrypted Key:%s\n",encrypted_key);
					printf("MAC:%s\n",mac_auth);*/

					/*Preparing KeyBlock */
					memset (&output,0,sizeof(output));
					//printf("\n[NCF01] Keyblock (Header+Encrypted Key data+ MAC) :\n");
					memcpy(output, header, 16);
					memcpy(output+16, encrypted_key, 96);
					memcpy(output+16+96, mac_auth, 8);
					output[128]="\0";
					//printf("[NCF01]Key Block output: %s\n", output);
					memcpy(ncf01_keyblock.acq_cwk_keyblock, output, sizeof(ncf01_keyblock.acq_cwk_keyblock));
					if(output!='\0' && ncf01_keyblock.acq_cwk_keyblock!='\0'){
					printf("ACQ_CWK_Keyblock: Migrating to Keyblock Successful!");
					}
					else
					{
						printf("ACQ_CWK_Keyblock: Migrating to Keyblock Failed!!");
					}
					free(encrypted);
					free(padded_input);
				}
			}
		db_insert_ncf01_keyblock(&ncf01_keyblock, ErrorMsg);
	    dbcommon_commit();
		}
	}
	return 0;
}
INT caf01_key_block_encryption(INT input_mode, BYTE input_primary_id[10], pCHAR card_brand )
{
	CHAR ErrorMsg[256] = {0};
	CHAR key_str[32];
	CHAR iv_str[16];
	uint8_t key[AES_KEY_SIZE];
	uint8_t iv[AES_BLOCK_SIZE];

	BYTE input[32];
	uint8_t mac[8];
	size_t padded_len;
	uint8_t *padded_input;
	CHAR keyblock_output[256];
	CHAR encrypted_key[128];
	BYTE mac_auth[16];
	CHAR mac_temp[8];
	CHAR hex_str[3];
	CHAR output[150];
	CHAR header[16];
	if(input_mode == 1)
	{
		if(input_primary_id!='\0')
		{
			if (strcmp(card_brand, "DCI") == 0) {
				memcpy(key_str, Secret_Key_for_AES_Encryption_in_Keyblock_DCI, 32);
				key_str[32]='\0';
				memset(header, 0, sizeof(header));
				memcpy(header, header_for_Keyblock_DCI, 16);
			}
			else if(strcmp(card_brand, "VISA") == 0)
			{
				memset(key_str, 0, sizeof(key_str));
				memcpy(key_str, Secret_Key_for_AES_Encryption_in_Keyblock_VISA, 32);
				key_str[32]='\0';
				memset(header, 0, sizeof(header));
				memcpy(header, header_for_Keyblock_VISA, 16);
			}
			else if(strcmp(card_brand, "MC") == 0)
			{
				memset(key_str, 0, sizeof(key_str));
				memcpy(key_str, Secret_Key_for_AES_Encryption_in_Keyblock_MC, 32);
				key_str[32]='\0';
				memset(header, 0, sizeof(header));
				memcpy(header, header_for_Keyblock_MC, 16);
			}
			else if(strcmp(card_brand, "JCB") == 0)
			{
				memset(key_str, 0, sizeof(key_str));
				memcpy(key_str, Secret_Key_for_AES_Encryption_in_Keyblock_JCB, 32);
				key_str[32]='\0';
				memset(header, 0, sizeof(header));
				memcpy(header, header_for_Keyblock_JCB, 16);
			}
			else if(strcmp(card_brand, "AMEX") == 0)
			{
				memset(key_str, 0, sizeof(key_str));
				memcpy(key_str, Secret_Key_for_AES_Encryption_in_Keyblock_AMEX, 32);
				key_str[32]='\0';
				memset(header, 0, sizeof(header));
				memcpy(header, header_for_Keyblock_AMEX, 16);
			}
			else if(strcmp(card_brand, "UPI") == 0)
			{
				memset(key_str, 0, sizeof(key_str));
				memcpy(key_str, Secret_Key_for_AES_Encryption_in_Keyblock_UPI, 32);
				key_str[32]='\0';
				memset(header, 0, sizeof(header));
				memcpy(header, header_for_Keyblock_UPI, 16);
			}
			else
			{
				printf("\n!!!Invalid card brand!!!\n");
				return;
			}

			printf("\n::::::::::::::::::::::::::::::::::CAF01:Encryption and Keyblock::::::::::::::::::::::::::::::::::\n");
			//MAGSTRIPE
			if((caf01.cvk[0].cvkA !='\0') && (caf01.cvk[0].cvkB!='\0'))
			{
				if (strcmp(card_brand, "DCI") == 0 || strcmp(card_brand, "VISA") == 0||
						strcmp(card_brand, "MC") == 0||strcmp(card_brand, "JCB") == 0||
						strcmp(card_brand, "UPI") == 0) {
					memset(input,0,sizeof(input));
					memcpy(input, caf01.cvk[0].cvkA, 16);
					memcpy(input + 16, caf01.cvk[0].cvkB, 16);
					input[32]='\0';
				}
				else if(strcmp(card_brand, "AMEX") == 0)
				{
					memset(input,0,sizeof(input));
					memcpy(input, caf01.csck, 32);
					input[32]='\0';
				}
				size_t input_len = sizeof(input);
				memcpy(key, key_str, AES_KEY_SIZE);
				memcpy(iv, header, AES_BLOCK_SIZE);
				pkcs7_pad(input, input_len, &padded_input, &padded_len);
				uint8_t *encrypted = malloc(padded_len);

				if (encrypted == NULL) {
					perror("Memory allocation failed");
					exit(EXIT_FAILURE);
				}

				/*AES 256 Encryption in CBC Mode*/
				aes_encrypt_cbc(input, key, iv, encrypted, input_len);

				for (size_t i = 0; i < padded_len; i++) {
					sprintf((char *)(encrypted_key + i * 2), "%02x", encrypted[i]);
				}
				printf("\n");

				/* MAC generation using CMAC*/
				compute_cmac(encrypted, padded_len, key, mac);

				for (int i = 0; i < 8; i++) {
					byte_to_hex_string(mac[i], hex_str);  // Convert each byte to hex string
					mac_auth[i * 2] = hex_str[0];         // Store the first hex character as raw byte
					mac_auth[i * 2 + 1] = hex_str[1];     // Store the second hex character as raw byte
					}
				mac_auth[16]='\0';
				/*printf("\nHeader:%s\n",header);
				printf("Encrypted Key:%s\n",encrypted_key);
				printf("MAC:%s\n",mac_auth);*/

				/*Preparing KeyBlock */
				memset (&output,0,sizeof(output));
				//printf("\nKeyblock (Header+Encrypted Key data+ MAC) :\n");
				memcpy(output, header, 16);
				memcpy(output+16, encrypted_key, 96);
				memcpy(output+16+96, mac_auth, 8);
				output[128]="\0";
				//printf("Key Block output: %s\n", output);
				if (strcmp(card_brand, "DCI") == 0 || strcmp(card_brand, "VISA") == 0||
						strcmp(card_brand, "MC") == 0||strcmp(card_brand, "JCB") == 0||
						strcmp(card_brand, "UPI") == 0) {
					memcpy(caf01_keyblock.Cvk_Mag_Keyblock, output, sizeof(caf01_keyblock.Cvk_Mag_Keyblock));
					if(output!='\0' && caf01_keyblock.Cvk_Mag_Keyblock!='\0'){
					printf("CVK_MAG_KEYBLOCK: Migrating to Keyblock Successful!");
					}
					else
					{
						printf("CVK_MAG_KEYBLOCK: Migrating to Keyblock Failed!!");
					}
				}

				else if(strcmp(card_brand, "AMEX") == 0)
				{
					memcpy(caf01_keyblock.Csck_Mag_Keyblock, output, sizeof(caf01_keyblock.Csck_Mag_Keyblock));
					if(output!='\0' && caf01_keyblock.Csck_Mag_Keyblock!='\0'){
					printf("Csck_Mag_Keyblock: Migrating to Keyblock Successful!");
					}
					else
					{
						printf("Csck_Mag_Keyblock: Migrating to Keyblock Failed!!");
					}
				}

				free(encrypted);
				free(padded_input);
			}
			//ARQC
			if((caf01.cvk[5].cvkA!='\0') && (caf01.cvk[5].cvkB!='\0'))
			{
				memset(input,0,sizeof(input));
				memcpy(input, caf01.cvk[5].cvkA, 16);
				memcpy(input + 16, caf01.cvk[5].cvkB, 16);
				input[32]='\0';
				size_t input_len = sizeof(input);

				memcpy(key, key_str, AES_KEY_SIZE);
				memcpy(iv, header, AES_BLOCK_SIZE);
				pkcs7_pad(input, input_len, &padded_input, &padded_len);
				uint8_t *encrypted = malloc(padded_len);

				if (encrypted == NULL) {
					perror("Memory allocation failed");
					exit(EXIT_FAILURE);
				}

				/*AES 256 Encryption in CBC Mode*/
				aes_encrypt_cbc(input, key, iv, encrypted, input_len);

				for (size_t i = 0; i < padded_len; i++) {
					sprintf((char *)(encrypted_key + i * 2), "%02x", encrypted[i]);
				}
				printf("\n");

				/* MAC generation using CMAC*/
				compute_cmac(encrypted, padded_len, key, mac);

				for (int i = 0; i < 8; i++) {
					byte_to_hex_string(mac[i], hex_str);  // Convert each byte to hex string
					mac_auth[i * 2] = hex_str[0];         // Store the first hex character as raw byte
					mac_auth[i * 2 + 1] = hex_str[1];     // Store the second hex character as raw byte
					}
				mac_auth[16]='\0';
				/*printf("\nHeader:%s\n",header);
				printf("Encrypted Key:%s\n",encrypted_key);
				printf("MAC:%s\n",mac_auth);*/

				/*Preparing KeyBlock */
				memset (&output,0,sizeof(output));
				//printf("\nKeyblock (Header+Encrypted Key data+ MAC) :\n");
				memcpy(output, header, 16);
				memcpy(output+16, encrypted_key, 96);
				memcpy(output+16+96, mac_auth, 8);
				output[128]="\0";
				//printf("Key Block output: %s\n", output);
				//For CAF01 page, Insert Keyblock into CAF01_Keyblock Table
				memcpy(caf01_keyblock.Cvk_Mkac_Keyblock, output, sizeof(caf01_keyblock.Cvk_Mkac_Keyblock));
				if(output!='\0' && caf01_keyblock.Cvk_Mkac_Keyblock!='\0'){
					printf("CVK_MKAC_KEYBLOCK:Migrating to Keyblock Successful!");
				}
				else
				{
					printf("CVK_MKAC_KEYBLOCK: Migrating to Keyblock Failed!!");
				}
				free(encrypted);
				free(padded_input);
			}
			//ACQ 3DS
			if((caf01.cvk[4].cvkA!='\0') && (caf01.cvk[4].cvkB!='\0'))
			{
				if (strcmp(card_brand, "DCI") == 0 || strcmp(card_brand, "VISA") == 0||strcmp(card_brand, "JCB") == 0) {
					memset(input,0,sizeof(input));
					memcpy(input, caf01.cvk[4].cvkA, 16);
					memcpy(input + 16, caf01.cvk[4].cvkB, 16);
					input[32]='\0';
					size_t input_len = sizeof(input);

					memcpy(key, key_str, AES_KEY_SIZE);
					memcpy(iv, header, AES_BLOCK_SIZE);
					pkcs7_pad(input, input_len, &padded_input, &padded_len);
					uint8_t *encrypted = malloc(padded_len);

					if (encrypted == NULL) {
						perror("Memory allocation failed");
						exit(EXIT_FAILURE);
					}

					/*AES 256 Encryption in CBC Mode*/
					aes_encrypt_cbc(input, key, iv, encrypted, input_len);

					for (size_t i = 0; i < padded_len; i++) {
						sprintf((char *)(encrypted_key + i * 2), "%02x", encrypted[i]);
					}
					printf("\n");

					/* MAC generation using CMAC*/
					compute_cmac(encrypted, padded_len, key, mac);

					for (int i = 0; i < 8; i++) {
						byte_to_hex_string(mac[i], hex_str);  // Convert each byte to hex string
						mac_auth[i * 2] = hex_str[0];         // Store the first hex character as raw byte
						mac_auth[i * 2 + 1] = hex_str[1];     // Store the second hex character as raw byte
						}
					mac_auth[16]='\0';
					/*printf("\nHeader:%s\n",header);
					printf("Encrypted Key:%s\n",encrypted_key);
					printf("MAC:%s\n",mac_temp);*/

					/*Preparing KeyBlock */
					memset (&output,0,sizeof(output));
					//printf("\nKeyblock (Header+Encrypted Key data+ MAC) :\n");
					memcpy(output, header, 16);
					memcpy(output+16, encrypted_key, 96);
					memcpy(output+16+96, mac_auth, 8);
					output[128]="\0";
					//printf("Key Block output: %s\n", output);
					memcpy(caf01_keyblock.Cvk_3ds_Cak_Keyblock, output, sizeof(caf01_keyblock.Cvk_3ds_Cak_Keyblock));
					if(output!='\0' && caf01_keyblock.Cvk_3ds_Cak_Keyblock!='\0'){
					printf("CVK_3DS_CAK_KEYBLOCK: Migrating to Keyblock Successful!");
					}
					else
					{
						printf("CVK_3DS_CAK_KEYBLOCK: Migrating to Keyblock Failed!!");
					}
					free(encrypted);
					free(padded_input);
				}
			}

			if((caf01.cvk[3].cvkA!='\0') && (caf01.cvk[3].cvkB!='\0'))
			{
				if (strcmp(card_brand, "DCI") == 0 || strcmp(card_brand, "VISA") == 0||
						strcmp(card_brand, "MC") == 0||strcmp(card_brand, "JCB") == 0||
						strcmp(card_brand, "UPI") == 0) {
					memset(input,0,sizeof(input));
					memcpy(input, caf01.cvk[3].cvkA, 16);
					memcpy(input + 16, caf01.cvk[3].cvkB, 16);
					input[32]='\0';
					size_t input_len = sizeof(input);

					memcpy(key, key_str, AES_KEY_SIZE);
					memcpy(iv, header, AES_BLOCK_SIZE);
					pkcs7_pad(input, input_len, &padded_input, &padded_len);
					uint8_t *encrypted = malloc(padded_len);

					if (encrypted == NULL) {
						perror("Memory allocation failed");
						exit(EXIT_FAILURE);
					}

					/*AES 256 Encryption in CBC Mode*/
					aes_encrypt_cbc(input, key, iv, encrypted, input_len);

					for (size_t i = 0; i < padded_len; i++) {
						sprintf((char *)(encrypted_key + i * 2), "%02x", encrypted[i]);
					}
					printf("\n");

					/* MAC generation using CMAC*/
					compute_cmac(encrypted, padded_len, key, mac);

					for (int i = 0; i < 8; i++) {
						byte_to_hex_string(mac[i], hex_str);  // Convert each byte to hex string
						mac_auth[i * 2] = hex_str[0];         // Store the first hex character as raw byte
						mac_auth[i * 2 + 1] = hex_str[1];     // Store the second hex character as raw byte
						}
					mac_auth[16]='\0';
					/*printf("\nHeader:%s\n",header);
					printf("Encrypted Key:%s\n",encrypted_key);
					printf("MAC:%s\n",mac_auth);*/

					/*Preparing KeyBlock */
					memset (&output,0,sizeof(output));
					//printf("\nKeyblock (Header+Encrypted Key data+ MAC) :\n");
					memcpy(output, header, 16);
					memcpy(output+16, encrypted_key, 96);
					memcpy(output+16+96, mac_auth, 8);
					output[128]="\0";
					//printf("Key Block output: %s\n", output);
					if (strcmp(card_brand, "DCI") == 0 ) {
						memcpy(caf01_keyblock.Dci_3ds_Keyblock, output, sizeof(caf01_keyblock.Dci_3ds_Keyblock));
					    if(output!='\0' && caf01_keyblock.Dci_3ds_Keyblock!='\0'){
					    printf("DCI_3DS_KEYBLOCK: Migrating to Keyblock Successful!");
					    }
					    else
					    {
					    	printf("DCI_3DS_KEYBLOCK: Migrating to Keyblock Failed!!");
					    }
					}
					else if(strcmp(card_brand, "AMEX") == 0||strcmp(card_brand, "VISA") == 0||
							strcmp(card_brand, "MC") == 0||strcmp(card_brand, "JCB") == 0||
							strcmp(card_brand, "UPI") == 0)
					{
						memcpy(caf01_keyblock.Csck_Iss_3ds_Keyblock, output, sizeof(caf01_keyblock.Csck_Iss_3ds_Keyblock));
						if(output!='\0' && caf01_keyblock.Csck_Iss_3ds_Keyblock!='\0'){
						printf("Csck_Iss_3ds_Keyblock: Migrating to Keyblock Successful!");
						}
						else
						{
							printf("Csck_Iss_3ds_Keyblock: Migrating to Keyblock Failed!!");
						}
					}
					free(encrypted);
					free(padded_input);
				}
			}
			if((caf01.pin.IbmOffset.pin_key!='\0') && (caf01.pin.IbmOffset.decimalization_tbl!='\0'))
			{
				if (strcmp(card_brand, "DCI") == 0 || strcmp(card_brand, "VISA") == 0||
						strcmp(card_brand, "MC") == 0||strcmp(card_brand, "JCB") == 0||
						strcmp(card_brand, "UPI") == 0) {
					memset(input,0,sizeof(input));
					memcpy(input, caf01.pin.IbmOffset.pin_key, 16);
					memcpy(input + 16, caf01.pin.IbmOffset.decimalization_tbl, 16);
					input[32]='\0';
					size_t input_len = sizeof(input);

					memcpy(key, key_str, AES_KEY_SIZE);
					memcpy(iv, header, AES_BLOCK_SIZE);
					pkcs7_pad(input, input_len, &padded_input, &padded_len);
					uint8_t *encrypted = malloc(padded_len);

					if (encrypted == NULL) {
						perror("Memory allocation failed");
						exit(EXIT_FAILURE);
					}

					/*AES 256 Encryption in CBC Mode*/
					aes_encrypt_cbc(input, key, iv, encrypted, input_len);

					for (size_t i = 0; i < padded_len; i++) {
						sprintf((char *)(encrypted_key + i * 2), "%02x", encrypted[i]);
					}
					printf("\n");

					/* MAC generation using CMAC*/
					compute_cmac(encrypted, padded_len, key, mac);

					for (int i = 0; i < 8; i++) {
						byte_to_hex_string(mac[i], hex_str);  // Convert each byte to hex string
						mac_auth[i * 2] = hex_str[0];         // Store the first hex character as raw byte
						mac_auth[i * 2 + 1] = hex_str[1];     // Store the second hex character as raw byte
						}
					mac_auth[16]='\0';
					/*printf("\nHeader:%s\n",header);
					printf("Encrypted Key:%s\n",encrypted_key);
					printf("MAC:%s\n",mac_auth);*/

					/*Preparing KeyBlock */
					memset (&output,0,sizeof(output));
					//printf("\nKeyblock (Header+Encrypted Key data+ MAC) :\n");
					memcpy(output, header, 16);
					memcpy(output+16, encrypted_key, 96);
					memcpy(output+16+96, mac_auth, 8);
					output[128]="\0";
					//printf("Key Block output: %s\n", output);
					memcpy(caf01_keyblock.Cvk_Pin_Verification, output, sizeof(caf01_keyblock.Cvk_Pin_Verification));
					if(output!='\0' && caf01_keyblock.Cvk_Pin_Verification!='\0'){
					printf("CVK_PIN_VERIFICATION: Migrating to Keyblock Successful!");
					}
					else
					{
						printf("CVK_PIN_VERIFICATION: Migrating to Keyblock Failed!!");
					}
					free(encrypted);
					free(padded_input);
				}
			}
			db_insert_caf01_keyblock(&caf01_keyblock, ErrorMsg);
			dbcommon_commit();
		}
	}
	return 0;
}

void EndProcess()
{
	char      ErrorMsg[100] = "";
	DBTEST_disconnect(ErrorMsg);
	printf("\nDatabase disconnect sucess:\n\n");

}

INT main( INT argc, pCHAR argv[] )
{
	char ErrorMsg[256] = {0};
	BYTE caf01_page[6]={0};
	BYTE ncf01_page[2]={0};
	BYTE user_input_profile_id[5]={0};
	BYTE user_input_network_id[10]={0};
	pCHAR card_brand_argv;
	INT  mode=0;
	INT field;
	INT ret_val = 0 ;

	GetAppName(app_name);
	ret_val=get_dataserver_ini_db_error_statics_value( &db_error_statics_flag, &db_error_statics_value,
			app_name,     ErrorMsg );
	memset( &caf01, 0x00, sizeof( caf01 ) );
	memset( &ncf01, 0x00, sizeof( ncf01 ) );
	if ( argc == 4)
	{
		if (DBTEST_connect("equitable", "equitable", "equitable", "ORCL", ErrorMsg) == 0)
		{
			printf("\nDatabase connection successful!\n\n");
		}
		else
		{
			printf("Database connect failed:%s\n",ErrorMsg );
			EndProcess();
		}

		/* Get root directory path */
		memset (path, 0x00, sizeof(path));
		GetAscendentRootDirectory(path);

		if (strcmp(argv[1], "1") == 0) {
		strncpy(caf01_page, argv[1],1);
		strncpy(caf01_page+1, argv[3],4);
		caf01_page[6]='\0';
		mode =atoi(caf01_page);
		}
		if (strcmp(argv[1], "2") == 0) {
			strncpy(ncf01_page, argv[1],1);
			ncf01_page[1]='\0';
			mode =atoi(ncf01_page);
			}
		 //based on Mode 1,2,3 should prepare keyblock
		 //for CAF01
		card_brand_argv=argv[2];
		 if(mode == 1)
		 {
			 /*Get the Secret Key and header from tf.ini*/
			 printf("::::::::::::::::::::::::::::::::::::::::TF.INI:::::::::::::::::::::::::::::::::::::::::\n\n");
			 get_SecretKey_for_KeyBlock_AES_Encryption_for_card_profile();
			 get_Header_for_KeyBlock_for_card_profile();

			 caf01_keyblock.primary_key.Type[0]='C';
			 caf01_keyblock.primary_key.Type[1]='\0';
			 //Fetches caf01 Details
			 db_select_caf01_for_keyblock(&caf01, caf01_page+1, ErrorMsg);

			 //Based on Card brands it encrypts key and prepares key block
			 if( strcmp(argv[2], "DCI") == 0 )
			 {
			 	memcpy(caf01_keyblock.primary_key.Caf_Profile_id, argv[3], sizeof(caf01_keyblock.primary_key.Caf_Profile_id));
			 	memcpy(user_input_profile_id, argv[3], sizeof(user_input_profile_id));
			 	caf01_key_block_encryption(mode,user_input_profile_id,card_brand_argv);
			 }
			 else if( strcmp(argv[2], "VISA") == 0)
			 {
			 	memcpy(caf01_keyblock.primary_key.Caf_Profile_id, argv[3], sizeof(caf01_keyblock.primary_key.Caf_Profile_id));
			 	memcpy(user_input_profile_id, argv[3], sizeof(user_input_profile_id));
			 	caf01_key_block_encryption(mode,user_input_profile_id,card_brand_argv);
			 }
			 else if( strcmp(argv[2], "MC") == 0)
			 {
			 	memcpy(caf01_keyblock.primary_key.Caf_Profile_id, argv[3], sizeof(caf01_keyblock.primary_key.Caf_Profile_id));
			 	memcpy(user_input_profile_id, argv[3], sizeof(user_input_profile_id));
			 	caf01_key_block_encryption(mode,user_input_profile_id,card_brand_argv);
			 }
			 else if( strcmp(argv[2], "JCB") == 0)
			 {
			 	memcpy(caf01_keyblock.primary_key.Caf_Profile_id, argv[3], sizeof(caf01_keyblock.primary_key.Caf_Profile_id));
			 	memcpy(user_input_profile_id, argv[3], sizeof(user_input_profile_id));
			 	caf01_key_block_encryption(mode,user_input_profile_id,card_brand_argv);
			 }
			 else if( strcmp(argv[2], "AMEX") == 0)
			 {
			 	memcpy(caf01_keyblock.primary_key.Caf_Profile_id, argv[3], sizeof(caf01_keyblock.primary_key.Caf_Profile_id));
			 	memcpy(user_input_profile_id, argv[3], sizeof(user_input_profile_id));
			 	caf01_key_block_encryption(mode,user_input_profile_id,card_brand_argv);
			 }
			 else if( strcmp(argv[2], "UPI") == 0)
			 {
			 	memcpy(caf01_keyblock.primary_key.Caf_Profile_id, argv[3], sizeof(caf01_keyblock.primary_key.Caf_Profile_id));
			 	memcpy(user_input_profile_id, argv[3], sizeof(user_input_profile_id));
			 	caf01_key_block_encryption(mode,user_input_profile_id,card_brand_argv);
			 }
			 else{
				 printf("\n!!!Invalid CARD BRAND!!!\n");
			 }
		 }
		 //for NCF01
		 if(mode == 2)
		 {
			 /*Get the Secret Key and header from tf.ini*/
			 printf("::::::::::::::::::::::::::::::::::::::::TF.INI:::::::::::::::::::::::::::::::::::::::::\n\n");
			 get_SecretKey_for_KeyBlock_AES_Encryption_for_network_profile();
			 get_Header_for_KeyBlock_for_network_profile();

			 strncpy(ncf01.primary_key.network_type, argv[2], sizeof(ncf01.primary_key.network_type) - 1);
			 strncpy(ncf01.primary_key.network_id, argv[3], sizeof(ncf01.primary_key.network_type) - 1);
			 ncf01.primary_key.network_type[1]='\0';
			 memcpy(user_input_network_id, argv[3], sizeof(user_input_network_id));
			 if (ncf01.primary_key.network_type[0]=='I')
			 {
				 memcpy(ncf01_keyblock.network_id, argv[3], sizeof(ncf01_keyblock.network_id));
				 memset (ErrorMsg, 0x00, sizeof(ErrorMsg));
				 db_select_ncf01_for_keyblock(&ncf01, argv[3],argv[2],ErrorMsg);
				 printf("\nNetwork_id :%s\n",ncf01.primary_key.network_id);
				 printf("\nNetwork_type :%s\n",ncf01.primary_key.network_type);
				 ncf01_key_block_encryption(mode,user_input_network_id,argv[2]);
			 }
			 if (ncf01.primary_key.network_type[0]=='A')
			 {
				 memcpy(ncf01_keyblock.network_id, argv[3], sizeof(ncf01_keyblock.network_id));
				 memset (ErrorMsg, 0x00, sizeof(ErrorMsg));
				 db_select_ncf01_for_keyblock(&ncf01,argv[3],argv[2],ErrorMsg);
				 printf("\nNetwork_id :%s\n",ncf01.primary_key.network_id);
				 printf("\nNetwork_type :%s\n",ncf01.primary_key.network_type);
				 ncf01_key_block_encryption(mode,user_input_network_id,argv[2]);
			 }
		 }

		EndProcess();
    }
	else
	{
		DisplayVersions();
	}
	return 0;
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
