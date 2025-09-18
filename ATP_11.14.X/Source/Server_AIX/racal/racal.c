/******************************************************************************
 *  
 *  Module: Racal.c
 *   
 *   
 ******************************************************************************/

/* Includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifdef WIN32
#include <conio.h>
#include <process.h>
#endif 

#include "xipc.h"
#include "ntutils.h"
#include "app_info.h"
#include "nc_dbstruct.h"
#include "equitdb.h"
#include "pteipc.h"
#include "ptesystm.h"
#include "cdsapi.h"
#include "txutils.h"
#include "cup.h"
#include "diners.h"
#include "tx_dbstruct.h"
//positions for TX data for ARQC

#define TX_DATA_9F02_POS 	 0
#define TX_DATA_9F03_POS 	 6
#define TX_DATA_9F1A_POS 	 12
#define TX_DATA_95_POS		 14
#define TX_DATA_5F2A_POS	 19
#define TX_DATA_9A_POS		 21
#define TX_DATA_9C_POS	     24
#define TX_DATA_9F37_POS     25
#define TX_DATA_82_POS		 29
#define TX_DATA_9F36_POS	 31
#define TX_DATA_9F10_POS	 33
#define TX_DATA_PADDING		 39
#define TX_DATA_PADDING_VISA_CVN_18		 40

#define BIN_LEN_CVN10  40
#define BIN_LEN_CVN18  41
#define BIN_LEN_CVN17  16

#define ARRAY_SIZE	20
#define SHA256_LEN	32

/* ASCII and EBCDIC conversion */
#define   A2E    "A2E"    /*  ASCII to EBCDIC conversion */
#define   E2A    "E2A"    /*  EBCDIC to ASCII conversion */

#define DCP_UCAF_LEN_POS  51
#define DCP_UCAF_DATA_POS 53
#define ECI_SLI_FULLYAUTHENTICATED "12"

/******************************************************/
/* DATA STRUCTURE FOR STORING DATA IN FINAL FORMAT... */
/******************************************************/
struct iso_msg_rec
   {
        unsigned char msgtype_bcd   [2];
        unsigned char bitmap_bin    [8];
        unsigned char pan_len       [1];
        unsigned char pan_bin       [10];
        unsigned char proccode      [3];
        unsigned char amount_bin    [6];
        unsigned char stan_bin      [3];
        unsigned char time_bin      [3];
        unsigned char date_bin      [2];
        unsigned char date_expd     [2];
        unsigned char date_settle   [2];
        unsigned char posentry      [2];
        unsigned char nii           [2];
        unsigned char pos_condcd    [1];
        unsigned char track2_len    [1];
        unsigned char track2        [19];
        unsigned char rrn           [12];
        unsigned char authcode      [6];
        unsigned char responsecd    [2];
        unsigned char termid        [8];
        unsigned char cardaccid     [15];
        unsigned char track1_len    [1];
        unsigned char track1        [76];
        unsigned char adddata_len   [2]; 
        unsigned char adddata       [16];
        unsigned char pin_block     [8];
        unsigned char addl_len      [2];
        unsigned char addlamts      [50];
        unsigned char priv60_len    [2];
        unsigned char priv60        [100];
        unsigned char priv61_len    [2];
        unsigned char priv61        [100];
        unsigned char priv62_len    [2];
        unsigned char priv62        [100];
        unsigned char priv63_len    [2];
        unsigned char priv63        [200];
   };

/*pte messageing to work.*/
struct iso_msg_rec ISO_MESSAGE;

/* Extern definition section */

CHAR  racal_Erro_warning_Filename[256] = {0} ;
CHAR  racal_module_error_warning_file_name[256] = {0};
CHAR  racal_error_warning_file_path[256] = {0};

extern INT EndProcessSignalled;
extern INT MainProcessDone;

extern void PrintBuffer();
extern void delay_seconds();
long long Hex_to_Dec(char * buf_ptr);
void racal_Get_Sha256_Encryption(char * Suppl_Amt);
INT get_EMV_Data_AMEX(INT emv_len_in, pBYTE emv_data_in,pBYTE tag_data );
/*extern void generateCustomerPin(char indata[],char derivedPinNumber[]);*/
extern INT  volatile connection_up;
extern CHAR ServiceName[12];

#define TIMEOUT_TICKS			5 /* SECONDS */
#define Non_Atalla_Compatible	'0'
#define Atalla_Compatible		'1'

#define NSP_IDENTIKEY			1
#define NSP_EIDENTIKEY			2
#define NSP_IBM3624				3
#define NSP_VISA				4
#define NSP_DESBILEVEL			5
#define NSP_DEIBOLD				6
#define NSP_NCR					7
#define NSP_CLEARPIN			8
#define NSP_CLEARPINBLOCK		9
#define NSP_CVVCVC				10
#define TRANSACTION_VERIFIED			1
#define TRANSACTION_NOT_VERIFIED		0
#define CVC2_TRANSACTION_NOT_VERIFIED	2
#define CVC2_TRANSACTION_VERIFIED		3
#define  DE48_TAG43    "43"         // Tag 43, Max lenngth -> 32 AN characters
#ifdef  _DEBUG
   #define MYPRINT(BUFFER)  printf("%s ",BUFFER);           
#else
   #define MYPRINT(BUFFER)  ;
#endif                              

/*farad 1/13/1999.  The next modules are added for the Equitable Bank project.*/
int   racalEquitVerifyPin();
int   racalEquitTranlatePin();
int   racalEquitDecryptPin();
int   racalEquitVerifyCVV_CVC_CSC_CAV();
int   racalEquitKeyExchangeKey();
INT   get_EMV_CVN_18_data( INT emv_len_in, pBYTE emv_data_in,pBYTE tag_data );
int   racalSendToPort(char output[]);
int   racalSendToPort_MC(char output[], int len);
void  generateMsgHeader(char returnStr[]);
int   parseTrackOne(char trackOne[], char retStr[], char fieldSeperator[], char position[], char length[]);
int   parseTrackTwo(char trackTwo[], char retStr[], char fieldSeperator[], char position[], char length[]);
BYTE ascii_ebcdic_conv( unsigned char conv_char, char *conv_type );


/* Global definition section */
#define MAX_CMD_LEN          150
#define ORIGIN_LOCAL           1
#define ORIGIN_REMOTE          2
#define ORIGIN_UNKNOWN         3

/* KEY USAGE
 *
 * The HSM host commands support single, double, and triple length DES keys.
 * The command set is completely backward compatible with earlier versions of
 * firmware.  The commands support extensions to enable the specification of
 * key length and key encryption scheme to use.
 *
 * If the first character of the key is a hexadecimal character (0-9, or A-F)
 * or K or S, the commands will operate as previously specified.  In most
 * circumstances, the key is single length except for ZMKs when the ZMK length
 * is configured for double length or for specific keys that are double length
 * by definition.  This is the 16H or 32H length and types.
 *
 * To support double and triple length keys throughout the command set, key
 * scheme tags have been defined.  These enable the HSM to determine the key
 * length and encryption mechanism used for a key.  The key scheme tag prefixes
 * the key.  This is the 1A+32H or 1A+48H length and types.
 *
 *    1A  = One ASCII Character - This is the key tag.
 *    nnH = 32 or 48  Hexadecimal characters
 *
 * There are currently two key encryption schemes supported by the HSM.
 *
 * ANSI X9.17 method
 * -----------------
 *
 * The tags for this scheme are:
 *    X = Double length DES keys.
 *    Y = Triple length DES keys.
 *
 * These are used for the key exchange with other parties/systems such as
 * ATM Switch, Mastercard, and Visa.  For example, key encrypted under ZMK
 * for transmission purpose.
 *
 * Variant method
 * --------------
 *
 * The tags for this scheme are:
 *    U = Double length DES keys.
 *    T = Triple length DES keys.
 *
 * The U and T tag scheme have a better encryption security, but only Thales
 * HSM uses them.  Usually keys protected under LMK will always be using the
 * U and T tag key scheme.
 *
 */
#define DOUBLE_LEN_KEY        		"U"
#define TRIPLE_LEN_KEY        		"T"
#define DOUBLE_LEN_KEY_X9_17  		"X"
#define TRIPLE_LEN_KEY_X9_17  		"Y"

#define SOUURCE_TPK_KEYBLOCK  		"S"
#define DESTINATION_TPK_KEYBLOCK  	"S"
#define KEYBLOCK_KEY_FORMAT_LEN		1
#define KEYLOCK_LMK_FOR_KEY_GEN		"FFF"
#define KEYLOCK_LMK_FOR_KEY_GEN_LEN 3

#define cDOUBLE_LEN_KEY       		'U'
#define cTRIPLE_LEN_KEY       		'T'
#define cDOUBLE_LEN_KEY_X9_17 		'X'
#define cTRIPLE_LEN_KEY_X9_17 		'Y'
#define cSOUURCE_KEK_KEYBLOCK  		'S'

/*CUP Key Verification field length */

#define DECIMALISATION_TABLE_LENGTH   		16
#define PIN_VALIDATION_DATA_LENGTH     		12
#define IBM_OFFSET_DATA_LENGTH           	12
#define KEYBLOCK_KEY_LEN					128
#define MAX_KEYBLOCK_CMD_LEN			    512

AUTH_TX 				auth_tx;
NCF01   				ncf01;
CAF01_KEYBLOCK 	 		caf01_keyblock;
NCF01_KEYBLOCK     	 	Ncf01_Keyblock;

BDKKEY01				bdk_key01;

CHAR    ZMK[49];
CHAR    dummy[60];
CHAR    dummy1[60];

INT   Zmk_Size          = 0;
INT   Auth_Tx_Size      = 0;
INT   Outgoing_Buf_Size = 0;
INT   Ncf01_Size        = 0;
INT   Dummy_Size        = 0;
INT   ReturnString_Size = 0;
INT   msgHeaderCnt      = 0;
CHAR	AppName [8];
CHAR	MyName [9];
CHAR	MyQueName[9];
CHAR  MyInteractiveQueName[9];
CHAR	oprmon_que_name[9];
CHAR  returnString[200];

INT   length_of_router_scroll;

WORD  incoming_buf_length;

pCHAR		orig_queue;
pCHAR    orig_info;

BYTE  outgoing_buf[sizeof(AUTH_TX) + sizeof(NCF01)+ 10];

CHAR      Version[] = "ATP_11.12.3";



void racal_create_Error_Filename(  )
{
	CHAR  system_date[25] = {0};

   /* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
	ptetime_get_timestamp( system_date );

    strcpy( racal_Erro_warning_Filename, racal_error_warning_file_path );
    strcat( racal_Erro_warning_Filename, racal_module_error_warning_file_name );
	strcat( racal_Erro_warning_Filename, ".EW." );
    strncat(racal_Erro_warning_Filename, system_date,   4 );  /* YYYY */
    strncat(racal_Erro_warning_Filename, system_date+5, 2 );  /* MM   */
    strncat(racal_Erro_warning_Filename, system_date+8, 2 );  /* DD   */
}

/*******************************************************
 * This function will get the values from tf.ini file
 * from the section DATASERVER with keyname DB_ERROR_STATICS_FLAG &
 *	DB_ERROR_STATICS_VALUE which will help to decide the db statics
 *	logging mechanism.
 * *******************************************************/

void racal_get_error_warning_file_name_path(void )
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
								  "DATASERVERS",             /* points to section name         */
								  "DB_STATICS_PATH",       	/* points to key name             */
								   "",  					/* Default string                 */
								   racal_error_warning_file_path,              	/* points to destination buffer   */
								   sizeof(racal_error_warning_file_path)-1,   	 /* size of destination buffer     */
								   filename                  /* points to ini filename         */
						 	   );

   rc = GetPrivateProfileString(
								  "ERROR_WARNING",             /* points to section name         */
								  appname,       	/* points to key name             */
								  "",  					/* Default string                 */
								  racal_module_error_warning_file_name,              	/* points to destination buffer   */
								  sizeof(racal_module_error_warning_file_name)-1,   	 /* size of destination buffer     */
								  filename                  /* points to ini filename         */
								);

   if((strlen(racal_error_warning_file_path) > 0) &&
	  (strlen(racal_module_error_warning_file_name)> 0))
   {
	   racal_create_Error_Filename();
   }
}

INT racal_Log_error_warning_to_File(pCHAR Error_Buf,int sev, int detail)
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

	#define NUM_SIZE_WRITES  1

	/* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
	ptetime_get_timestamp( time_date );

	/* Pick out the month and day to compare to filename extenstion. */
    memcpy( current_mmdd,   time_date+5, 2 );
    memcpy( current_mmdd+2, time_date+8, 2 );

	/* Compare filename extension to current date. */
    len = strlen(racal_Erro_warning_Filename);
    path_len = strlen(racal_error_warning_file_path);
    if( len == 0 ||
    	path_len==0 )
    {
    	LogEvent(Error_Buf,INFO_MSG);
    	return 0;
    }
    if ( 0 != strcmp(&racal_Erro_warning_Filename[len-4], current_mmdd) )
    {
		/* Now create the new filename.*/
    	racal_create_Error_Filename();
    }
	if((fp = fopen(racal_Erro_warning_Filename,"a+b"))==NULL)
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
	}
	strcat(Buffer,"\n");
	len=strlen(Buffer);

	if(fwrite(Buffer, len, NUM_SIZE_WRITES, fp)==NUM_SIZE_WRITES)
	{
		// Do nothing fall below
	}
	else
	{
		LogEvent(Buffer,INFO_MSG);
	}
	fclose(fp);
	return  0;
}

/******************************************************************************
 *
 *  NAME:         racal_log_message
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
void racal_log_message( INT dest, INT sev, pCHAR msg, pCHAR func, int details )
{
   INT  monitor_flag = 0;
   INT  eventlog_flag = 0;
   INT  msg_type = 0;
   BYTE severity = 0;
   BYTE log_type[20] = {0};
   CHAR text_message [512] = {0};
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
   racal_Log_error_warning_to_File(text_message,sev,details);
   if(monitor_flag == 1)
   {
	   if(details == 1)
	   {
		   TxUtils_Send_Msg_To_Operator( monitor_flag, text_message,
										 eventlog_flag, msg_type, func,
										 severity, log_type,
										 auth_tx.TLF01_details.terminal_id,
										 auth_tx.TLF01_details.card_num,
										 auth_tx.TLF01_details.merchant_id );
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
/*************************************************************************************
       NAME:           Isallzeros(pCHAR astr, INT max_len)

       DESCRIPTION:    This function checks whether the string contains all zeros in it.
	   					   
                       
       INPUTS: 		   string and its length


       OUTPUTS: 	true : if all the char in the string have value zero or it is null.
					false: If any one char have numeric value
       RETURNS:        
	              BOOLEAN value
                               
      AUTHOR:         Raghavendra H R .
      MODIFIED BY:    
*************************************************************************************/

BOOLEAN Isallzeros(pCHAR astr , INT max_len)
{
	INT  i           = 0;
	INT int_val          = 0 ;
	BOOLEAN  ret_val = false;
	CHAR temp_str[256]= {0};

	if (max_len == 0)
	{
		return true;
	}
	else
	{
		strncpy(temp_str,astr,max_len);
	}

	if(isnum(temp_str) == true)
	{
		 int_val = atoi(temp_str);
		 if ( int_val == 0)
		 {
			ret_val = true;
		 }
		 else
		 {
			ret_val = false;
		 }
	}
	else
	{
		return true;
	}

	return( ret_val );
}
/*************************************************************************************
       NAME:           send_response_asynchronous()

       DESCRIPTION:    This module sends a response back to the destination queue.


       INPUTS:
					   msg_type :  It is the type of message needs to be built.  For
					   example, for a pin verification, it takes the value of
					   MT_NSP_VERIFYPIN_RESPONSE.

       OUTPUTS:
           None.
       RETURNS:
		   None.

      AUTHOR:         Faramarz Arad 11/18/1998.
      MODIFIED BY:
*************************************************************************************/
void send_response_asynchronous(BYTE msg_type, char * dest_queue)
{
  pPTE_MSG  p_msg_out;
  LONG      ret_code;
  CHAR	   ErrorMsg[100];
  /*CHAR	    Buffer  [256];*/

  MYPRINT("send_response_asynchronous\n");

  p_msg_out = ptemsg_build_msg( msg_type,
								  0,
								  0,
								  dest_queue,
								  MyQueName,
								  (pBYTE) outgoing_buf ,
								  Outgoing_Buf_Size,
								  0 );

	if ( p_msg_out == NULL_PTR )
	{
		racal_log_message( 1, 3, "insufficient memory to build message",
							"send_response_asynchronous",0 );
	return;
	}

	ret_code = pteipc_send( p_msg_out, dest_queue );
	if( ret_code < 0 )
	{
		pteipc_get_errormsg( ret_code, ErrorMsg );
		racal_log_message( 1, 3, "Failed: at pteipc_send",
								"send_response_asynchronous",0 );
	}
	free( p_msg_out );

}
static void bcd_to_asc (bstring, blen, astr, astr_len)
   BYTE bstring[];
   INT          blen;
   pCHAR        astr;
   INT          astr_len;
{
   int  i = 0;

   for (i = 0; i < (blen * 2); i=i+2)
      {
      astr[i] = (bstring[i/2] / 16) + '0';
      astr[i+1] = (bstring[i/2] & 0x0F) + '0';
      }

   astr [(blen * 2)] = '\0';
} /* bcd_to_asc */

/*********************************************************************/
BYTE ascii_ebcdic_conv( unsigned char conv_char, char *conv_type )
{
   INT            i;
   INT    found = 0;
   BYTE  ReturnChar;

      /*
        +------------------------------------------------------------+
        |   0     1     2     3     4     5     6     7     8     9  |
        |   a     b     c     d     e     f     g     h     i     j  |
        |   k     l     m     n     o     p     q     r     s     t  |
        |   u     v     w     x     y     z     A     B     C     D  |
        |   E     F     G     H     I     J     K     L     M     N  |
        |   O     P     Q     R     S     T     U     V     W     X  |
        |   Y     Z           !     "     #     $     %     &     '  |
        |   (     )     *     +     ,     -     .     /     :     ;  |
        |   <     =     >     ?     @     \     _     `     {     |  |
        |   }     ~                                                  |
        +------------------------------------------------------------+
      */

   static unsigned char ebcdic_chart[] = 
      {
         0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9,
         0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x91,
         0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0xA2, 0xA3,
         0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xC1, 0xC2, 0xC3, 0xC4,
         0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5,
         0xD6, 0xD7, 0xD8, 0xD9, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7,
         0xE8, 0xE9, 0x40, 0x5A, 0x7F, 0x7B, 0x5B, 0x6C, 0x50, 0x7D,
         0x4D, 0x5D, 0x5C, 0x4E, 0x6B, 0x60, 0x4B, 0x61, 0x7A, 0x5E,
         0x4C, 0x7E, 0x6E, 0x6F, 0x7C, 0xE0, 0x6D, 0x79, 0xC0, 0x4F,
         0xD0, 0xA1, 0x5F
      };

   static unsigned char ascii_chart[] = 
      {
         0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
         0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A,
         0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73, 0x74,
         0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x41, 0x42, 0x43, 0x44,
         0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E,
         0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
         0x59, 0x5A, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
         0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x3A, 0x3B,
         0x3C, 0x3D, 0x3E, 0x3F, 0x40, 0x5C, 0x5F, 0x60, 0x7B, 0x7C,
         0x7D, 0x7E, 0x5E
      };


   if ( 0 == strcmp( conv_type, A2E ) )
   {
      /* CONVERT ASCII TO EBCDIC */
      //the next line is modified by farad 8/13/98
	  for( i=0; i<93 && !found; i++ )
      {
         if ( conv_char == ascii_chart[i] )
         {
            ReturnChar = ebcdic_chart[i];
            found = 1;
         }
      }
   }
   else if ( 0 == strcmp( conv_type, E2A ) )
   {
      /* CONVERT EBCDIC TO ASCII */
      //the next line is modified by farad 8/13/98
      for( i=0; i<93 && !found; i++ )
      {
         if ( conv_char == ebcdic_chart[i] )
         {
            ReturnChar = ascii_chart[i];
            found = 1;
         }
      }
   }

   if ( 0 == found )
      ReturnChar = 0x00;

   return( ReturnChar );
}   //end of the ascii_ebcdic_conv


/*
This should be Called only for CUP - TF Phani

*/
/*********************************************************************/
INT racalCUPVerifyPin()
{
   INT   len;
   CHAR  msgHeader[5];
   CHAR  cmdCode[3];
   CHAR  output[200];
   CHAR  strTemp[256]={0};
   BYTE asciiPIN[20]={0};
   INT  blen=0, astr_len=0;
   BYTE IBMoffset[13]={0};
   memset (&msgHeader,0,sizeof(msgHeader));
   memset (&cmdCode,0,sizeof(cmdCode));
   memset (&output,0,sizeof(output));
   memset (&returnString,0,ReturnString_Size);

   strcpy (msgHeader ,"HDRS");


   strcpy (cmdCode,"EA"); 
   /* Add the command code and the message header.*/
   strcat(output,msgHeader);
   strcat(output,cmdCode);

   /* Add the ZPK to the message.*/
   len = strlen( auth_tx.TLF01_details.source_key );
   if ( len > 0 )
   {
      switch( len )
      {
         case 32:  strcat( output, DOUBLE_LEN_KEY ); break;
         case 48:  strcat( output, TRIPLE_LEN_KEY ); break;
      }
      strcat(output,auth_tx.TLF01_details.source_key);

      //printf("CWK - %s \n",auth_tx.TLF01_details.source_key);
   }
   else
   {
      racal_log_message( 1, 2, "PV: source_key(ZPK) is blank","racalCUPVerifyPin",1 );
      return(TRANSACTION_NOT_VERIFIED);
   }

   /* Add the pvk for CUP , we are storing it in AUTH_PROFILE For IBMOFFSET PIN verification page.*/
   if (8 == strlen(auth_tx.CAF01_details.pin.Visa.key_left) )
   	{
      strcat(output,auth_tx.CAF01_details.pin.Visa.key_left);
     // printf("PVK - %s \n",auth_tx.CAF01_details.pin.Visa.key_left);
   	}
   else
   {
      racal_log_message( 1, 2, "PVK:Left is blank or More than 8 BYTE","racalCUPVerifyPin",1 );
      return(TRANSACTION_NOT_VERIFIED);
   }
   
   if (8 == strlen(auth_tx.CAF01_details.pin.Visa.key_right))
   {
	 	strcat(output,auth_tx.CAF01_details.pin.Visa.key_right);
	   // printf("PVK - %s \n",auth_tx.CAF01_details.pin.Visa.key_right);
   }
	else
	{
	 	racal_log_message( 1, 2, "PVK:Right is blank or More than 8 BYTE","racalCUPVerifyPin",1 );
		return(TRANSACTION_NOT_VERIFIED);
	}

	/* Max PIN length - */

	strcat(output,"12");

   /* Add the pin block to the message.*/
   if(strlen(auth_tx.TLF01_details.pin_block) != 0)
   {
	   //we should send ASCII value.COnvert binary into ACSII
	   if(strlen(auth_tx.TLF01_details.pin_block) == 16)
	   {
		   strcat(output,auth_tx.TLF01_details.pin_block);
	   }
	   else
	   {
		   blen = 8;
		   astr_len =16;
		   genutil_bin_to_ascii (auth_tx.TLF01_details.pin_block, blen, asciiPIN,astr_len);
		   strcat(output,asciiPIN);
		   }
	   //printf("PIN BLOCK -in Ascii %s \n",asciiPIN);
   }
   else 
   {
      racal_log_message( 1, 2, "PV: pin_block is blan","racalCUPVerifyPin",1 );
      return(TRANSACTION_NOT_VERIFIED);   
   }


   /*add the source pin format to the message.*/
   //  This field is 00, 01, 02, and so on.
   strcat(output,"01");
   // Check field length

   strcat(output,"06"); //Min PIN len
   
   //take the 12 right most digit from the account number.
   if(strlen(auth_tx.TLF01_details.card_num) >= 12 )
   {
      memset(&dummy,0,Dummy_Size);
      strncpy(dummy,auth_tx.TLF01_details.card_num + (strlen(auth_tx.TLF01_details.card_num) - 13),12);
      dummy[12] = '\0';
      strcat(output,dummy);
   }
   else
   {
      racal_log_message( 1, 2, "PV: card_num is < 12","racalCUPVerifyPin",1 );
      return (TRANSACTION_NOT_VERIFIED);
   }

   //16 byte of Decimalisation Table which will be taken from AUTH_PROFILE page
   if(DECIMALISATION_TABLE_LENGTH==strlen(auth_tx.CAF01_details.cvk[2].cvkA))
   {
      strcat(output,auth_tx.CAF01_details.cvk[2].cvkA);
   }
   else 
   {
         racal_log_message( 1, 2, "Decimalisation Table is blank","racalCUPVerifyPin",1 );
         return(TRANSACTION_NOT_VERIFIED);
   }
   //12 byte of PIN Validation Data which will be taken from AUTH_PROFILE page
   if(PIN_VALIDATION_DATA_LENGTH ==strlen(auth_tx.CAF01_details.cvk[2].cvkB))
   {
      strcat(output,auth_tx.CAF01_details.cvk[2].cvkB);
   }
   else 
   {
         racal_log_message( 1, 2, "PIN Validation data is blank","racalCUPVerifyPin",1 );
         return(TRANSACTION_NOT_VERIFIED);
   }
   //printf("Pin validation data - %s \n",auth_tx.CAF01_details.cvk[2].cvkB);
    //12 byte of IBM Offset data which will be stored in CCF02P records for each card
   if(strlen(auth_tx.CCF02P_details.pin) > 0 )
   {
	   memcpy(IBMoffset,auth_tx.CCF02P_details.pin,12);
	   strncat(output,IBMoffset,12);
   }
   else 
   {
         racal_log_message( 2, 2, "IBM Offset data is blank","racalCUPVerifyPin",1 );
         return(TRANSACTION_NOT_VERIFIED);
   }

   //if the message lenght is more then 150 there is some thing wrong.
   if (strlen(output) > 150)
   {
      racal_log_message( 2, 2, "PV: Cmd Msg to long","racalCUPVerifyPin",1 );
      return(TRANSACTION_NOT_VERIFIED);
   }
   else
   {
	   sprintf(strTemp, " input of racal EA: %s", output);
	   racal_log_message( 2, 2,strTemp,"racalCUPVerifyPin",1 );
   }

   //send a message to the Racal box.
   if (racalSendToPort(output) == TRANSACTION_VERIFIED)
   {
      memset(&dummy,0,Dummy_Size);
      strncpy(dummy,returnString,4);

      sprintf(strTemp, " output of racal cmnd EB: %s", returnString);
      racal_log_message( 2, 2,strTemp,"racalCUPVerifyPin",1 );

      //check to see if the same header msg is back.
      if(strcmp(msgHeader,dummy) != 0)
      {
         racal_log_message( 2, 2, "PV: Msg Header mis-match.","racalCUPVerifyPin",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      memset(&dummy,0,Dummy_Size);
      strncpy(dummy,returnString + 4,2);

      //check to see if the "ED" or "DD" response code is back.
      if ((strcmp(dummy,"EB") != 0) )
      {
         racal_log_message( 2, 2, "PV: Cmd Code mis-match.","racalCUPVerifyPin",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      memset(&dummy,0,Dummy_Size);
      strncpy(dummy,returnString + 6,2);
      
      if(strcmp(dummy,"00") == 0)
      {
#ifdef _DEBUG
         racal_log_message( 2, 1, "PIN-Verify PASSED.","racalCUPVerifyPin",1 );
#endif
         return(TRANSACTION_VERIFIED);
      }
      if(strcmp(dummy,"01") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify failure.","racalCUPVerifyPin",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }
      if(strcmp(dummy,"10") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify TPK parity error.","racalCUPVerifyPin",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"11") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify PVK parity error.","racalCUPVerifyPin",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"12") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify No keys laoded in user storage.","racalCUPVerifyPin",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"13") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify LMK error report to supervisor","racalCUPVerifyPin",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"15") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify Error in input data.","racalCUPVerifyPin",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"20") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify PIN block does not contain valid values.","racalCUPVerifyPin",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"21") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify Invalid user storage index","racalCUPVerifyPin",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"23") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify Invalid PIN block format code.","racalCUPVerifyPin",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"24") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify PIN is less than 4 or more than 12","racalCUPVerifyPin",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      return(TRANSACTION_NOT_VERIFIED);
   }
   else
      return(TRANSACTION_NOT_VERIFIED);


}/*racalCUPVerifyPin*/


/*********************************************************************/
INT racalEquitVerifyPin()
{
   INT   len;
   CHAR  msgHeader[5];
   CHAR  cmdCode[3];
   CHAR  output[200];

   memset (&msgHeader,0,sizeof(msgHeader));
   memset (&cmdCode,0,sizeof(cmdCode));
   memset (&output,0,sizeof(output));
   memset (&returnString,0,ReturnString_Size);

   strcpy (msgHeader ,"2468");

   /* The command is different for local versus network origin. */
   if(strcmp(auth_tx.TLF01_details.dispensation.origin, "LO") == 0)
      strcpy (cmdCode,"DC"); 
   else if(strcmp(auth_tx.TLF01_details.dispensation.origin, "RO") == 0)
      strcpy (cmdCode,"EC"); 
   else
   {
      racal_log_message( 2, 2, "PV: origin is unkown.","racalEquitVerifyPin",1 );
      return(TRANSACTION_NOT_VERIFIED);
   }


   /* Add the command code and the message header.*/
   strcat(output,msgHeader);
   strcat(output,cmdCode);

   /* Add the tpk to the message.*/
   len = strlen( auth_tx.TLF01_details.source_key );
   if ( len > 0 )
   {
      switch( len )
      {
         case 32:  strcat( output, DOUBLE_LEN_KEY ); break;
         case 48:  strcat( output, TRIPLE_LEN_KEY ); break;
      }
      strcat(output,auth_tx.TLF01_details.source_key);
   }
   else
   {
      racal_log_message( 2, 2, "PV: source_key is blank","racalEquitVerifyPin",1 );
      return(TRANSACTION_NOT_VERIFIED);
   }

   /* Add the pvk pairs to the message.*/
   if (strlen(auth_tx.CAF01_details.pin.Visa.key_left) != 0)
      strcat(output,auth_tx.CAF01_details.pin.Visa.key_left);
   else
   {
      racal_log_message( 2, 2, "PV: Key_left is blank","racalEquitVerifyPin",1 );
      return(TRANSACTION_NOT_VERIFIED);
   }

   /* Add the pvk pairs to the message.*/
   if (strlen(auth_tx.CAF01_details.pin.Visa.key_right) != 0)
      strcat(output,auth_tx.CAF01_details.pin.Visa.key_right);
   else
   {
      racal_log_message( 2, 2, "PV: Key_right is blank","racalEquitVerifyPin",1 );
      return(TRANSACTION_NOT_VERIFIED);
   }


   /* Add the pin block to the message.*/
   if(strlen(auth_tx.TLF01_details.pin_block) != 0)
      strcat(output,auth_tx.TLF01_details.pin_block);
   else 
   {
      racal_log_message( 2, 2, "PV: pin_block is blank","racalEquitVerifyPin",1 );
      return(TRANSACTION_NOT_VERIFIED);   
   }

   /*add the source pin format to the message.*/
   //the values coming in are 0 to 6.  This field is 00, 01, 02, and so on.
   strcat(output,"0");
   if(strlen(auth_tx.TLF01_details.source_pin_format) != 0)
      strcat(output,auth_tx.TLF01_details.source_pin_format);
   else
   {
      racal_log_message( 2, 2, "PV: source pin format is blank","racalEquitVerifyPin",1 );
      return(TRANSACTION_NOT_VERIFIED);
   }



   //modified the code such that it takes out the check digit 3/3/1999 by farad.
   //take the 12 right most digit from the account number.
   if(strlen(auth_tx.TLF01_details.card_num) >= 12 )
   {
      memset(&dummy,0,Dummy_Size);
      strcpy(dummy,auth_tx.TLF01_details.card_num + (strlen(auth_tx.TLF01_details.card_num) - 13));
      dummy[12] = '\0';
      strcat(output,dummy);
   }
   else
   {
      racal_log_message( 2, 2, "PV: card_num is < 12","racalEquitVerifyPin",1 );
      return (TRANSACTION_NOT_VERIFIED);
   }

   //look at the value of auth_tx.CAF01_details.pin_offset_location if it is "C" check the data server
   if(strcmp(auth_tx.CAF01_details.pin_offset_location,"C") == 0)
   {
      //Get the PVKI and the PVV from the data server.
      /*add the PVKI value to the message.*/
      if(strlen(auth_tx.CCF02P_details.pvki) != 0)
         strcat(output,auth_tx.CCF02P_details.pvki);
      else 
      {
         racal_log_message( 2, 2, "PV: pvki is blank","racalEquitVerifyPin",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      /*add the PVV value to the message.*/
      if(strlen(auth_tx.CCF02P_details.pin_offset) != 0)
         strcat(output,auth_tx.CCF02P_details.pin_offset);
      else 
      {
         racal_log_message( 2, 2, "PV: pin_offset is blank","racalEquitVerifyPin",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }
   }
   else
      {
         //Get the PVKI and the PVV from the TRACK 1 &or 2.
         if(strlen(auth_tx.TLF01_details.track1) != 0)
         {
            /*if you have made it this far, info is on track one.*/
            /*check track one.  make sure it is not empty.*/
            /*get the value of the PVKI from track one*/
            memset(&dummy,0,Dummy_Size);
            if (1 == parseTrackOne(auth_tx.TLF01_details.track1,dummy ,"^",
                     auth_tx.CAF01_details.pin.Visa.pvki_location_tr1, auth_tx.CAF01_details.pin.Visa.pvki_length_tr1))
            {
               strcat(output,dummy);
            }
            else
               return(TRANSACTION_NOT_VERIFIED);

            /*get the value of the PVV from track one.*/   
            memset(&dummy,0,Dummy_Size);
            if (1 == parseTrackOne(auth_tx.TLF01_details.track1,dummy ,"^",
                     auth_tx.CAF01_details.pin.Visa.pvv_location_tr1, auth_tx.CAF01_details.pin.Visa.pvv_offset_tr1))
            {
               strcat(output,dummy);
            }
            else
               return(TRANSACTION_NOT_VERIFIED);
         }
         else
            if(strlen(auth_tx.TLF01_details.track2) != 0)
            {
               /*if you have made it this far, info is on track two.*/
               /*check track two.  make sure it is not empty.*/
               /*get the value of the PVKI from track one*/
               memset(&dummy,0,Dummy_Size);
               if (1 == parseTrackTwo(auth_tx.TLF01_details.track2,dummy ,"=",
                        auth_tx.CAF01_details.pin.Visa.pvki_location_tr2, auth_tx.CAF01_details.pin.Visa.pvki_length_tr2))
               {
                  strcat(output,dummy);
               }
               else
                  return(TRANSACTION_NOT_VERIFIED);

               /*get the value of the PVV from track one.*/   
               memset(&dummy,0,Dummy_Size);
               if (1 == parseTrackTwo(auth_tx.TLF01_details.track2,dummy ,"=",
                        auth_tx.CAF01_details.pin.Visa.pvv_location_tr2, auth_tx.CAF01_details.pin.Visa.pvv_offset_tr2))
               {
                  strcat(output,dummy);
               }
               else
                  return(TRANSACTION_NOT_VERIFIED);
            }
            else
               {
                  racal_log_message( 2, 2, "PV: Track 1&2 blank","racalEquitVerifyPin",1 );
                  return(TRANSACTION_NOT_VERIFIED);
               }
      }  //end of the else for data server or track option.

   //if the message lenght is more then 150 there is some thing wrong.
   if (strlen(output) > 150)
   {
      racal_log_message( 2, 2, "PV: Cmd Msg to long","racalEquitVerifyPin",1 );
      return(TRANSACTION_NOT_VERIFIED);
   }

   //send a message to the Racal box.
   if (racalSendToPort(output) == TRANSACTION_VERIFIED)
   {
      memset(&dummy,0,Dummy_Size);
      strncpy(dummy,returnString,4);

      //check to see if the same header msg is back.
      if(strcmp(msgHeader,dummy) != 0)
      {
         racal_log_message( 2, 2, "PV: Msg Header mis-match.","racalEquitVerifyPin",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      memset(&dummy,0,Dummy_Size);
      strncpy(dummy,returnString + 4,2);

      //check to see if the "ED" or "DD" response code is back.
      if ((strcmp(dummy,"DD") != 0) && (strcmp(dummy,"ED") != 0) )
      {
         racal_log_message( 2, 2, "PV: Cmd Code mis-match.","racalEquitVerifyPin",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      memset(&dummy,0,Dummy_Size);
      strncpy(dummy,returnString + 6,2);
      
      if(strcmp(dummy,"00") == 0)
      {
#ifdef _DEBUG
         racal_log_message( 2, 1, "PIN-Verify PASSED.","racalEquitVerifyPin",1 );
#endif
         return(TRANSACTION_VERIFIED);
      }
      if(strcmp(dummy,"01") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify PASSED.","racalEquitVerifyPin",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }
      if(strcmp(dummy,"10") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify TPK parity error.","racalEquitVerifyPin",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"11") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify PVK parity error.","racalEquitVerifyPin",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"12") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify No keys laoded in user storage.","racalEquitVerifyPin",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"13") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify LMK error report to supervisor.","racalEquitVerifyPin",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"15") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify Error in input data.","racalEquitVerifyPin",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"20") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify PIN block does not contain valid values.","racalEquitVerifyPin",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"21") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify Invalid user storage index.","racalEquitVerifyPin",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"23") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify Invalid PIN block format code.","racalEquitVerifyPin",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"24") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify PIN is less than 4 or more than 12.","racalEquitVerifyPin",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      return(TRANSACTION_NOT_VERIFIED);
   }
   else
      return(TRANSACTION_NOT_VERIFIED);


}/*racalEquitVerifyPin*/

/************************************************************************
*************************************************************************/
INT racalEquitTranlatePin()
{
   INT   len;
   CHAR  msgHeader[5]={0};
   CHAR  cmdCode[3]  ={0};
   CHAR  output[200] ={0};
   CHAR  strTemp[256]={0};

   BYTE asciiPIN[20]={0};
   INT  blen=0, astr_len=0;

   memset (&msgHeader,0,sizeof(msgHeader));
   memset (&cmdCode,0,sizeof(cmdCode));
   memset (&output,0,sizeof(output));
   memset (&returnString,0,ReturnString_Size);

   strcpy (msgHeader ,"5678");


   /*the value of "LO" represents terminal for origin.*/
   /*The value of "RO" represents network for origin.*/
   /*if(strcmp(auth_tx.BIN01_details.card_family,CARD_DINERS) == 0)
	   strcpy (cmdCode,"CA");
   else*/
	   if(strcmp(auth_tx.TLF01_details.dispensation.origin, "RO") == 0 ||
	  	 strcmp(auth_tx.BIN01_details.card_family,CARD_CUP) == 0 ||
		 0 == strncmp(auth_tx.TLF01_details.product_code,CUP_CARD_INDICATOR_FLAG,2)
		 )
         strcpy (cmdCode,"CC"); 
	  else if(strcmp(auth_tx.TLF01_details.dispensation.origin, "LO") == 0)
      strcpy (cmdCode,"CA"); 
   
      else
      {
         racal_log_message( 2, 2, "PIN-TRANX dispen-origin mis-match.","racalEquitTranlatePin",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

   strcpy(output,msgHeader);
   strcat(output,cmdCode);
   
   /* Add the source TPK or ZPK to the message. */
   len = strlen( auth_tx.TLF01_details.source_key );
   if ( len > 0 )
   {
      switch( len )
      {
         case 32:  strcat( output, DOUBLE_LEN_KEY ); break;
         case 48:  strcat( output, TRIPLE_LEN_KEY ); break;
      }
      strcat( output, auth_tx.TLF01_details.source_key );
   }
   else
   {
      racal_log_message( 2, 2, "PIN-TRANX source_key blank.","racalEquitTranlatePin",1 );
      return(TRANSACTION_NOT_VERIFIED);
   }

   /* Add the destination TPK or ZPK to the message.*/
   len = strlen( auth_tx.TLF01_details.dest_key );
   if ( len > 0 )
   {
      switch( len )
      {
         case 32:  strcat( output, DOUBLE_LEN_KEY ); break;
         case 48:  strcat( output, TRIPLE_LEN_KEY ); break;
      }
      strcat( output, auth_tx.TLF01_details.dest_key );
   }
   else
   {
      racal_log_message( 2, 2, "PIN-TRANX dest_key blank.","racalEquitTranlatePin",1 );
      return(TRANSACTION_NOT_VERIFIED);
   }

   /*add the max pin length*/
   strcat(output,"12");

   /*add the pin block to the message.*/
   if(strlen(auth_tx.TLF01_details.pin_block) != 0)
   {
	   if((0 == strncmp(auth_tx.TLF01_details.issuer_id,"nccup3",5))
		   || 		  (0 == strncmp(auth_tx.TLF01_details.issuer_id,"ncdci",5))
		  )
	   {
			//we should send ASCII value.COnvert binary into ACSII
			blen = 8;
			astr_len =16;
			genutil_bin_to_ascii (auth_tx.TLF01_details.pin_block, blen, asciiPIN,astr_len);
			strcat(output,asciiPIN);

	   }
	   else
		   strcat(output,auth_tx.TLF01_details.pin_block);
   }
   else 
   {
      racal_log_message( 2, 2, "PIN-TRANX pin_block blank.","racalEquitTranlatePin",1 );
      return(TRANSACTION_NOT_VERIFIED);   
   }
   //add the next line because the net work handler send in a single character value.
   strcat(output,"0");
   
   /*add the source pin format to the message.*/
   if(strlen(auth_tx.TLF01_details.source_pin_format) != 0)
   {

	   strcat(output,auth_tx.TLF01_details.source_pin_format);
   }
   else
   {
      racal_log_message( 2, 2, "PIN-TRANX source_pin_format.","racalEquitTranlatePin",1 );
      return(TRANSACTION_NOT_VERIFIED);
   }
   //add the next line because the net work handler send in a single character value.
   strcat(output,"0");

   /*add the destination pin format to the message.*/
   if(strlen(auth_tx.TLF01_details.dest_pin_format) != 0)
      strcat(output,auth_tx.TLF01_details.dest_pin_format);
   else
   {
      racal_log_message( 2, 2, "PIN-TRANX dest_pin_format blank.","racalEquitTranlatePin",1 );
      return(TRANSACTION_NOT_VERIFIED);
   }


   /*farad 7/19/1999  Added the next line to take care of nccirs 12 digit pans*/
   if (  (0 == strcmp(auth_tx.TLF01_details.acquirer_id,"nccirs") || 
          0 == strcmp(auth_tx.TLF01_details.acquirer_id,"ncvisa") ||
          0 == strcmp(auth_tx.TLF01_details.acquirer_id,"ncvsms")) &&
         (strlen(auth_tx.TLF01_details.card_num) <= 12)
      )
   {
      memset(&dummy1,0,Dummy_Size);
      memset(&dummy,'\0',Dummy_Size);
      memset(&dummy,'0',Dummy_Size);   
      strcpy(dummy + (16 - strlen(auth_tx.TLF01_details.card_num)),auth_tx.TLF01_details.card_num);

      strcpy(dummy1,dummy + (strlen(dummy) - 13)); 
      dummy1[12] = '\0';
      strcat(output,dummy1);
   }
   else if(strlen(auth_tx.TLF01_details.card_num) > 12 )
   {
      /*take the 12 right most digit from the account number.*/
      memset(&dummy,0,Dummy_Size);
      strcpy(dummy,auth_tx.TLF01_details.card_num + (strlen(auth_tx.TLF01_details.card_num) - 13));
      dummy[12] = '\0';
      strcat(output,dummy);
   }
   else
   {
      racal_log_message( 2, 2, "PIN-TRANX card_num < 12.","racalEquitTranlatePin",1 );
      return (TRANSACTION_NOT_VERIFIED);
   }

   //if the message lenght is more then 150 there is some thing wrong.
   if (strlen(output) > 150)
   {
      racal_log_message( 2, 2, "PIN_TRANX Cmd Msg to long.","racalEquitTranlatePin",1 );
      return(TRANSACTION_NOT_VERIFIED);
   }
   else
   {
	   sprintf(strTemp, " input of racal CC: %s", output);
	   racal_log_message( 2, 2,strTemp,"racalEquitTranlatePin",1 );
   }

   //send the message to the racal box.
   if (racalSendToPort(output) == TRANSACTION_VERIFIED)
   {
      memset(&dummy,0,Dummy_Size);
      strncpy(dummy,returnString,4);
      sprintf(strTemp, " output of racal cmnd CD: %s", returnString);
      racal_log_message( 2, 2,strTemp,"racalEquitTranlatePin",1 );
      //check the value of the msg header
      if(strcmp(msgHeader,dummy) != 0)
      {
         racal_log_message( 2, 2, "PIN-TRANX Msg header mis-match.","racalEquitTranlatePin",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      //check the value of the command code.
      memset(&dummy,0,Dummy_Size);
      strncpy(dummy,returnString + 4,2);
      if ((strcmp(dummy,"CB") != 0) && (strcmp(dummy,"CD") != 0) )
      {
         racal_log_message( 2, 2, "PIN-TRANX cmd code mis-match.","racalEquitTranlatePin",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      memset(&dummy,0,Dummy_Size);
      strncpy(dummy,returnString + 6,2);
      if(strcmp(dummy,"00") == 0)
      {
         memset(&dummy,0,Dummy_Size);
         strncpy(dummy,returnString + 10, 16);
         strcpy(auth_tx.TLF01_details.pin_block,dummy);
		 //("OUT GOING PIN BLOCK IS %s \n",dummy);
#ifdef _DEBUG
         racal_log_message( 2, 1, "PPIN-TRANX No problemo.","racalEquitTranlatePin",1 );
#endif
         return(TRANSACTION_VERIFIED);
      }

      if(strcmp(dummy,"01") == 0)
      {
         racal_log_message( 2, 2, "PIN-TRANX failure","racalEquitTranlatePin",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"10") == 0)
      {
         racal_log_message( 2, 2, "PIN-TRANSLATE TPK parity error.","racalEquitTranlatePin",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"11") == 0)
      {
         racal_log_message( 2, 2, "PIN-TRANSLATE Dest ZPK parity error.","racalEquitTranlatePin",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"12") == 0)
      {
         racal_log_message( 2, 2, "PIN-TRANSLATE No keys laoded in user storage.","racalEquitTranlatePin",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"13") == 0)
      {
         racal_log_message( 2, 2, "PIN-TRANSLATE LMK error report to supervisor.","racalEquitTranlatePin",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"15") == 0)
      {
         racal_log_message( 2, 2, "PIN-TRANSLATE Error in input data.","racalEquitTranlatePin",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"20") == 0)
      {
         racal_log_message( 2, 2, "PIN-TRANSLATE PIN block data Error.","racalEquitTranlatePin",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"21") == 0)
      {
         racal_log_message( 2, 2, "PIN-TRANSLATE Invalid user storage index.","racalEquitTranlatePin",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"22") == 0)
      {
         racal_log_message( 2, 2, "PIN-TRANSLATE Invalid Account number.","racalEquitTranlatePin",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"23") == 0)
      {
         racal_log_message( 2, 2, "PIN-TRANSLATE Invalid PIN block format code.","racalEquitTranlatePin",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"24") == 0)
      {
         racal_log_message( 2, 2, "PIN-TRANSLATE PIN is less than 4 or more than 12.","racalEquitTranlatePin",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      return(TRANSACTION_NOT_VERIFIED);

   }
   else
      return(TRANSACTION_NOT_VERIFIED);


}/*racalEquitTranlatePin*/

/*************************************************************************************
       NAME:           racalEquitVerifyCVV_CVC_CSC_CAV()

       DESCRIPTION:    This module will deal with the magnatic strip verification.


       INPUTS:
		 	None.

       OUTPUTS:
         None
       RETURNS:
		   None.

      AUTHOR:         Faramarz Arad 1/20/1999
      MODIFIED BY:
*************************************************************************************/
int racalEquitDecryptPin()
{
	   INT   len;
	   CHAR  msgHeader[5]={0};
	   CHAR  cmdCode[3]  ={0};
	   CHAR	 modeFlag[3] ={0};
	   CHAR	 inputoutputFormatFlag[3] ={0};
	   CHAR	 BDKkeytype[4] ={0};
	   CHAR	 KSNdyscrptr[4] ={0};
	   CHAR	 KSNkey[65]	   ={0};
	   CHAR  output[200] ={0};
	   CHAR  strTemp[256]={0};

	   BYTE asciiPIN[20]={0};
	   INT  blen=0, astr_len=0;

	   memset (&msgHeader,0,sizeof(msgHeader));
	   memset (&cmdCode,0,sizeof(cmdCode));
	   memset (&output,0,sizeof(output));
	   memset (&returnString,0,ReturnString_Size);

	   strcpy (msgHeader ,"1234");

		strcpy(cmdCode,"M2");
		/* "00": ECB Decryption */
		strcpy(modeFlag, "00");
		/* '1' - HexEncoded binary */
		strcpy(inputoutputFormatFlag, "11");
		/* "009"- BDK-1 LMK28/29 variant */
		strcpy(BDKkeytype, "009");
		/* KSN Descriptor,"609" if BDK-1*/
		strcpy(KSNdyscrptr,"609");
		/* KSN key */
		strncpy(KSNkey, auth_tx.TLF01_details.product_codes[4].amount, 12);
		strncat(KSNkey, auth_tx.TLF01_details.product_codes[6].amount, 8);


	   strcpy(output,msgHeader);
	   strcat(output,cmdCode);
	   strcat(output,modeFlag);
	   strcat(output,inputoutputFormatFlag);
	   strcat(output,BDKkeytype);

	   /* Add the source BDK key to the message. */
	   len = strlen( auth_tx.TLF01_details.dcf01_retired_cwk );
	   if ( len > 0 )
	   {
	      switch( len )
	      {
	         case 32:  strcat( output, DOUBLE_LEN_KEY ); break;
	         case 48:  strcat( output, TRIPLE_LEN_KEY ); break;
	      }
	      strcat( output, auth_tx.TLF01_details.dcf01_retired_cwk );
	   }
	   else
	   {
	      racal_log_message( 2, 2, "BDK KEY source_key blank.","racalEquitDecryptPin",1 );
	      return(TRANSACTION_NOT_VERIFIED);
	   }

	   strcat(output,KSNdyscrptr);
	   /* Add the destination KSN key to the message.*/
	   len = strlen( KSNkey );
	   if ( len > 0 )
	   {
	      strcat( output, KSNkey );
	   }
	   else
	   {
	      racal_log_message( 2, 2, "PIN-TRANX KSN key is blank.","racalEquitDecryptPin",1 );
	      return(TRANSACTION_NOT_VERIFIED);
	   }

	   /*add the pin block to the message.*/
	   if(strlen(auth_tx.TLF01_details.pin_block) != 0)
	   {
		   /* Add PIN length */
		   strcat(output,"0010");
		   strcat(output,auth_tx.TLF01_details.pin_block);
	   }
	   else
	   {
	      racal_log_message( 2, 2, "PIN-TRANX pin_block blank.","racalEquitDecryptPin",1 );
	      return(TRANSACTION_NOT_VERIFIED);
	   }

	   //if the message lenght is more then 150 there is some thing wrong.
	   if (strlen(output) > 150)
	   {
	      racal_log_message( 2, 2, "PIN_TRANX Cmd Msg to long.","racalEquitDecryptPin",1 );
	      return(TRANSACTION_NOT_VERIFIED);
	   }
	   else
	   {
		   sprintf(strTemp, " input of racal M2: %s", output);
		   racal_log_message( 2, 2,strTemp,"racalEquitDecryptPin",1 );
	   }

	   //send the message to the racal box.
	   if (racalSendToPort(output) == TRANSACTION_VERIFIED)
	   {
	      memset(&dummy,0,Dummy_Size);
	      strncpy(dummy,returnString,4);
	      sprintf(strTemp, " output of racal cmnd CD: %s", returnString);
	      racal_log_message( 2, 2,strTemp,"racalEquitDecryptPin",1 );
	      //check the value of the msg header
	      if(strcmp(msgHeader,dummy) != 0)
	      {
	         racal_log_message( 2, 2, "PIN-TRANX Msg header mis-match.","racalEquitDecryptPin",1 );
	         return(TRANSACTION_NOT_VERIFIED);
	      }

	      //check the value of the command code.
	      memset(&dummy,0,Dummy_Size);
	      strncpy(dummy,returnString + 4,2);
	      if ((strcmp(dummy,"M3") != 0))
	      {
	         racal_log_message( 2, 2, "PIN-TRANX cmd code mis-match.","racalEquitDecryptPin",1 );
	         return(TRANSACTION_NOT_VERIFIED);
	      }

	      memset(&dummy,0,Dummy_Size);
	      strncpy(dummy,returnString + 6,2);
	      if(strcmp(dummy,"00") == 0)
	      {
	         memset(&dummy,0,Dummy_Size);
	         strncpy(dummy,returnString + 10, 16);
	         strcpy(auth_tx.TLF01_details.pin_block,dummy);

	#ifdef _DEBUG
	         racal_log_message( 2, 1, "PPIN-TRANX No problemo.","racalEquitDecryptPin",1 );
	#endif
	         return(TRANSACTION_VERIFIED);
	      }

	      if(strcmp(dummy,"02") == 0)
	      {
	         racal_log_message( 2, 2, "Invalid mode flag","racalEquitDecryptPin",1 );
	         return(TRANSACTION_NOT_VERIFIED);
	      }
	      if(strcmp(dummy,"03") == 0 || strcmp(dummy,"04") == 0)
	      {
	         racal_log_message( 2, 2, "Invalid i/p or o/p format flag","racalEquitDecryptPin",1 );
	         return(TRANSACTION_NOT_VERIFIED);
	      }
	      if(strcmp(dummy,"05") == 0)
	      {
	         racal_log_message( 2, 2, "Invalid key type field","racalEquitDecryptPin",1 );
	         return(TRANSACTION_NOT_VERIFIED);
	      }
	      if(strcmp(dummy,"06") == 0)
	      {
	         racal_log_message( 2, 2, "Invalid msg/blk len","racalEquitDecryptPin",1 );
	         return(TRANSACTION_NOT_VERIFIED);
	      }
	      if(strcmp(dummy,"10") == 0)
	      {
	         racal_log_message( 2, 2, "PIN-TRANSLATE Decrypn parity error.","racalEquitDecryptPin",1 );
	         return(TRANSACTION_NOT_VERIFIED);
	      }

	      if(strcmp(dummy,"35") == 0)
	      {
	         racal_log_message( 2, 2, "PIN-TRANSLATE msg format error.","racalEquitDecryptPin",1 );
	         return(TRANSACTION_NOT_VERIFIED);
	      }


	      return(TRANSACTION_NOT_VERIFIED);

	   }
	   else
	      return(TRANSACTION_NOT_VERIFIED);


}/*racalEquitDecryptPin*/

/*************************************************************************************
       NAME:           racalEquitVerifyCVV_CVC_CSC_CAV()

       DESCRIPTION:    This module will deal with the magnatic strip verification.

                       
       INPUTS:
		 	None.

       OUTPUTS:
         None
       RETURNS:        
		   None.                       
                               
      AUTHOR:         Faramarz Arad 1/20/1999
      MODIFIED BY:    
*************************************************************************************/
int racalEquitVerifyCVV_CVC_CSC_CAV()
{
   char msgHeader[5];
   char cmdCode[3]={0},cvc1_serviceCode[4]={0}, cvc2_serviceCode[4]={0}; 
   char delimiter[2];
   char output[200],cardType = 'Z',csc[6]={0};
   char cvc1_data[4]={0}, cvc2_data[4]={0};
   int  track_present = 0, cvc2_present = 0, cvc2_sent = 0;
   int  loopidx = 0;
   int  cnt=0,result=0,jcnt=0;
   char BUFFER [256]={0};
   char strTemp[50] = {0};
   CHAR *tempptr,fieldSeperator[2]={0};
   CHAR csc_result5[6];
   CHAR csc_result4[5];
   CHAR csc_result3[4];
   INT 	flag=0;
   INT  AEVV_result_code;
   CHAR AEVV_second_factor[3]={0},AEVV_bin_second_factor[2];
   CHAR AEVV_iCSC[5]={0},AEVV_bin_iCSC[3];
   CHAR AEVV_UPN[5]={0},AEVV_bin_UPN[3];
   INT temp_out_len=0;

   memset (&msgHeader,0,sizeof(msgHeader));
   memset (&cmdCode,0,sizeof(cmdCode));
   memset (&delimiter,0,sizeof(delimiter));
   memset (&output,0,sizeof(output));
   memset (&returnString,0,ReturnString_Size);
   memset (&csc,0,sizeof(csc));

   strcpy (msgHeader ,"1234");

   if (strcmp(auth_tx.BIN01_details.card_family,CARD_VISA) == 0)
      cardType = 'V';

   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_MASTERCARD) == 0)
      cardType = 'M';

   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_AMEX) == 0)
      cardType = 'A';

   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_JCB) == 0)
      cardType = 'J';
   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_CUP) == 0)
		cardType = 'C';
   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_FLEET) == 0)
  		cardType = 'F';
   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_DINERS) == 0)
     		cardType = 'D';

   switch(cardType)
   {
      case 'A':

         /***********************Begin AMEX*********************************/
         strcat(output,msgHeader);
         strcat(output,"RY"); /* Changed Command code RY4- TF Phani*/
         strcat(output,"4");
		 if(strlen(auth_tx.int_net_facility_data) > 8)
		 { 
		 // AEVV verification value
		 	strcat(output,"3");
			flag = 3;
			AEVV_result_code =  auth_tx.int_net_facility_data[11];
			memcpy(AEVV_bin_second_factor,&auth_tx.int_net_facility_data[12],1);
			bcd_to_asc(AEVV_bin_second_factor,1,AEVV_second_factor,2);
			
			memcpy(AEVV_bin_iCSC,&auth_tx.int_net_facility_data[14],2);
			bcd_to_asc(AEVV_bin_iCSC,2,AEVV_iCSC,4);
			memcpy(AEVV_bin_UPN,&auth_tx.int_net_facility_data[16],2);
			bcd_to_asc(AEVV_bin_UPN,2,AEVV_UPN,4);
				
		 }
		 else
         strcat(output,"0");

         /*check to see if the value of the CSC is in on the TRACK one.*/
         if(strlen(auth_tx.TLF01_details.track1) != 0)
         {
            /*if you have made it this far, info is on track one.*/
            /*check track one.  make sure it is not empty.*/
               //farad 4/20/1999.  The value of the field seperator could changed.
               // Find field separator first, then pass on to the parseTrack1 function.
            //strcpy(tempptr,auth_tx.TLF01_details.track1);
            tempptr = auth_tx.TLF01_details.track1;
            for(jcnt=0; jcnt<(INT)strlen(auth_tx.TLF01_details.track1); jcnt++ ) 
            {
               if ( isalnum(*tempptr) )
                 ++tempptr;
               else
                break;
            }

            if (tempptr != NULL)
               strncpy(fieldSeperator,tempptr,1); 
            else
               return(TRANSACTION_NOT_VERIFIED);
            
            memset(&dummy,0,Dummy_Size);
            if (1 == parseTrackOne(auth_tx.TLF01_details.track1,dummy ,fieldSeperator,auth_tx.CAF01_details.cvv_offset_tr1, auth_tx.CAF01_details.cvv_length))
            {
               strcpy(csc,dummy);
            }
            else
               return(TRANSACTION_NOT_VERIFIED);
         }
         else if(strlen(auth_tx.TLF01_details.track2) != 0)
         {
            /*if you have made it this far, info is on track two.*/
            /*check track two, make sure it is not empty.*/

            //farad 4/19/1999.  The value of the field seperator could changed.
            // Find field separator first, then pass on to the parseTrack2 function.
            //strcpy(tempptr,auth_tx.TLF01_details.track2);
            tempptr = auth_tx.TLF01_details.track2;
            for(jcnt=0; jcnt<(INT)strlen(auth_tx.TLF01_details.track2); jcnt++ ) 
            {
               if ( isalnum(*tempptr) )
                 ++tempptr;
               else
                break;
            }

            if (tempptr != NULL)
               strncpy(fieldSeperator,tempptr,1); 
            else
               return(TRANSACTION_NOT_VERIFIED);


            memset(&dummy,0,Dummy_Size);
            if (1 == parseTrackTwo(auth_tx.TLF01_details.track2,dummy ,fieldSeperator,auth_tx.CAF01_details.cvv_offset_tr2, auth_tx.CAF01_details.cvv_length))
            {
               strcpy(csc,dummy);
            }
            else
               return(TRANSACTION_NOT_VERIFIED);
         }
         else
         {
            /* This is a manually entered transaction.
             * Note that Amex can have 4-digit CSC's. Since
             * cvc_data is not large enough for these, the
             * field 'interest_rate' is used.
             */
            if ( auth_tx.TLF01_details.cvc_data[0] != 0x00 )
               strcpy( csc, auth_tx.TLF01_details.cvc_data );
            else if ( auth_tx.TLF01_details.interest_rate[0] != 0x00 )
               strcpy( csc, auth_tx.TLF01_details.interest_rate );
				
			if(csc[0] == 'S') // TF Phani First byte is "S" means manulally keyed txn
				strcpy(csc,&csc[1]);
		

         }
		//TF Phani - CSCK value should follow by the U or T, U for Double , T for triple length
	//	case 32: key scheme = "U"
	//	case 48: key scheme = "T"
			
         //enter the CSCK value 32H
         if(strlen(auth_tx.CAF01_details.csck) != 0)
         {	if(auth_tx.CAF01_details.aba_nbr[0] == '1' || auth_tx.CAF01_details.aba_nbr[0] == 1)
         	  strcat(output,"U");
            strcat(output,auth_tx.CAF01_details.csck);
         }
         else
         {
            racal_log_message( 2, 2, "mag-strip caf01-csck blank.","racalEquitVerifyCVV_CVC_CSC_CAV",1 );
            return(TRANSACTION_NOT_VERIFIED);
         }

         /*the account number is left-justified and zero-filled if less then 19.*/
         if (strlen(auth_tx.TLF01_details.card_num) != 0)
         {
            strcat(output,auth_tx.TLF01_details.card_num);
            if(strlen(auth_tx.TLF01_details.card_num) != 19)
               for(cnt = 19 ; (INT)strlen(auth_tx.TLF01_details.card_num) < cnt ;cnt--)
                  strcat(output,"0");
         }
         else
         {
            racal_log_message( 2, 2, "mag-strip card_num blank.","racalEquitVerifyCVV_CVC_CSC_CAV",1 );
            return(TRANSACTION_NOT_VERIFIED);
         }
         /*do the expiration date.*/
			if(flag == 3 && strlen(AEVV_UPN) > 0)
			{
			strncat(output,AEVV_UPN,4);
			}
		 else if(strlen(auth_tx.TLF01_details.exp_date) != 0)
            strcat(output,auth_tx.TLF01_details.exp_date);
         else
         {
        	 racal_log_message( 2, 2, "mag-strip exp_date blank.","racalEquitVerifyCVV_CVC_CSC_CAV",1 );
        	 return(TRANSACTION_NOT_VERIFIED);
         }
		/* TF Phani- Add CSC value*/
		if(flag == 3)
		{	
			// AEVV value
			temp_out_len = strlen(output);

			sprintf(&output[temp_out_len],"%1d",AEVV_result_code);
			strcat(output,AEVV_second_factor);
			strncat(output,&AEVV_iCSC[1],3);
		}
        else if(strlen(csc) != 0)
		{ if(strlen(csc) == 5)
		  {strcat(output,csc);
		   strcat(output,"FFFFFFF");
          }else if(strlen(csc) == 4)
		  {strcat(output,"FFFFF");
		  strcat(output,csc);
		  strcat(output,"FFF");
          }else if(strlen(csc) == 3)
		  {strcat(output,"FFFFF");
		   strcat(output,"FFFF");
		   strcat(output,csc);
          }
		 
		}
		 else
			strcat(output,"FFFFFFFFFFFF");
		printf("CSC value : %s \n",csc);
		printf("Output : %s \n",output);
	
		sprintf(strTemp, " AMEX 3CSC to racal RY: %s", output);
		racal_log_message( 2, 2,strTemp,"racalEquitVerifyCVV_CVC_CSC_CAV",1 );
	
         //if the output is bigger then 150 do not send it out.
         if (strlen(output) > 150)
         {
            racal_log_message( 2, 2, "mag-strip Cmd Msg to long.","racalEquitVerifyCVV_CVC_CSC_CAV",1 );
            return(TRANSACTION_NOT_VERIFIED);
         }

         //send the message to the racal box.
         if (racalSendToPort(output) == TRANSACTION_VERIFIED)
         {
        	 sprintf(strTemp, " output of racal cmnd RY: %s", returnString);
        	 racal_log_message( 2, 2,strTemp,"racalEquitVerifyCVV_CVC_CSC_CAV",1 );

            /*parse the message check for the message header.*/
            memset(&dummy,0,Dummy_Size);
            strncpy(dummy,returnString,4);
            if(strcmp(dummy,msgHeader) != 0)
            {
               racal_log_message( 2, 2, "mag-strip Cmd header mis-match.","racalEquitVerifyCVV_CVC_CSC_CAV",1 );
               return(TRANSACTION_NOT_VERIFIED);
            }
            /*parse the message check for the response code.*/
            memset(&dummy,0,Dummy_Size);
            strncpy(dummy,returnString + 4, 2);
            if(strcmp(dummy,"RZ") !=0)
            {
               racal_log_message( 2, 2, "mag-strip Cmd Code mis-match.","racalEquitVerifyCVV_CVC_CSC_CAV",1 );
               return(TRANSACTION_NOT_VERIFIED);
            }

            /*parse the message check for the error code.*/
            memset(&dummy,0,Dummy_Size);
            strncpy(dummy,returnString + 6, 2);
			
			printf("Return from Racal : %s \n",returnString);
            //if this value is not 00 reject the transaction.
            if(strcmp(dummy,"00") != 0)
            {
               if(strcmp(dummy,"10") == 0)
               {
            	  racal_log_message( 2, 2, "mag-strip CSCK parity error.","racalEquitVerifyCVV_CVC_CSC_CAV",1 );
			   }
               if(strcmp(dummy,"12") == 0)
                  racal_log_message( 2, 2, "mag-strip No Keys in user storage.","racalEquitVerifyCVV_CVC_CSC_CAV",1 );
               if(strcmp(dummy,"13") == 0)
                  racal_log_message( 2, 2, "mag-strip LMK parity error.","racalEquitVerifyCVV_CVC_CSC_CAV",1 );
               if(strcmp(dummy,"15") == 0)
                  racal_log_message( 2, 2, "mag-strip Error in input data.","racalEquitVerifyCVV_CVC_CSC_CAV",1 );
			   if(strcmp(dummy,"01") == 0)
			   	{
				   strcpy(auth_tx.TLF01_details.response_text,"Invalid 4 CSC");
			   	   racal_log_message( 2, 2, "Invalid CSC/CVV/3D sec.","racalEquitVerifyCVV_CVC_CSC_CAV",1 );
			   	}
// Invalid      CSC           01             
//INvalid  27 CSCK not doubel lenght
// Invalid  68 HSM command disabled

               return(TRANSACTION_NOT_VERIFIED);
            }

            /*parse the message mode for the error code.*/
            memset(&dummy,0,Dummy_Size);
            strncpy(dummy,returnString + 8, 1);
            if(strcmp(dummy,"4") !=0)
            {
               racal_log_message( 2, 2, "mag-strip Msg Mode mis-match.","racalEquitVerifyCVV_CVC_CSC_CAV",1 );
               return(TRANSACTION_NOT_VERIFIED);
            }

            /* Racal returns a 5-digit CSC, 4-digit CSC, and a 3-digit CSC.
             * Compare our CSC against all three, just to make sure we are
             * comparing against the correct one.
             */
/*            memset( csc_result5, 0x00, sizeof(csc_result5) );
            memset( csc_result4, 0x00, sizeof(csc_result4) );
            memset( csc_result3, 0x00, sizeof(csc_result3) );

            memcpy( csc_result5, returnString+ 9, 5 );
            memcpy( csc_result4, returnString+14, 4 );
            memcpy( csc_result3, returnString+18, 3 );

            if ( (0 != strcmp(csc_result5, csc)) &&
                 (0 != strcmp(csc_result4, csc)) &&
                 (0 != strcmp(csc_result3, csc))  )
            {
               racal_log_message( 2, 2, "mag-strip Calculate CSC mis-match.","racalEquitVerifyCVV_CVC_CSC_CAV",1 );
               memset(BUFFER,0,sizeof(BUFFER));
               sprintf(BUFFER,
                      "mag-strip R5:%s, R4:%s, R3:%s - T:%s",
                       csc_result5, csc_result4, csc_result3 , csc);
               racal_log_message( 2, 2, BUFFER,"racalEquitVerifyCVV_CVC_CSC_CAV",1 );
               return(TRANSACTION_NOT_VERIFIED);
            }*/

#ifdef _DEBUG            
            racal_log_message( 2, 1, "mag-strip PASSED.","racalEquitVerifyCVV_CVC_CSC_CAV",1 );
#endif 
            return(TRANSACTION_VERIFIED);

         }/*end of if racalSendToPort*/
         else
            return(TRANSACTION_NOT_VERIFIED);


         /****************************end AMEX***************************************/

         break;
	  case 'C':
	  case 'D':
      case 'J':
         /*drop down to VISA for this function.*/
         /*break;*/

      case 'M':

         /*drop down to VISA for this function.*/
         /*break;*/
      case 'F':
    	  /*This is Private lable transaction.*/
    	  /*drop down to VISA for this function.*/
    	  /*break;*/
      case 'V':

         /*****************************begin Visa***************************************/
         /*The VISA, MC, and the JCB are all the same.*/
         /*The rest of this code would deal with them.*/


    		  strcpy (cmdCode,"CY");

         strcpy (delimiter,";");
         
         /*edit CVK A */
         if(strlen(auth_tx.CAF01_details.cvk[0].cvkA) == 0)
		 {
            racal_log_message( 2, 2, "mag-strip cvkA is blank.","racalEquitVerifyCVV_CVC_CSC_CAV",1 );
            return(TRANSACTION_NOT_VERIFIED);
         }
         /*edit CVK B */
         if(strlen(auth_tx.CAF01_details.cvk[0].cvkB) == 0)
         {
            racal_log_message( 2, 2, "mag-strip cvkB is blank.","racalEquitVerifyCVV_CVC_CSC_CAV",1 );
            return(TRANSACTION_NOT_VERIFIED);
         }

		 /*edit card number*/
	 	 if(strlen(auth_tx.TLF01_details.card_num) == 0 )
         {
            racal_log_message( 2, 2, "mag-strip card_num is blank.","racalEquitVerifyCVV_CVC_CSC_CAV",1 );
            return(TRANSACTION_NOT_VERIFIED);
         }

		 /*edit expiration date*/
         if(strlen(auth_tx.TLF01_details.exp_date) == 0)
         {
            racal_log_message( 2, 2, "mag-strip exp_date is blank.","racalEquitVerifyCVV_CVC_CSC_CAV",1 );
            return(TRANSACTION_NOT_VERIFIED);
         }
		// Fall Back CVV postion is 14
		 if (strcmp(auth_tx.BIN01_details.card_family,CARD_MASTERCARD) == 0 && 
		 	(strncmp(auth_tx.TLF01_details.pos_entry_mode,"080",3) == 0 || strncmp(auth_tx.TLF01_details.pos_entry_mode,"008",3) == 0)
			)
		 //	strncmp(auth_tx.TLF01_details.pos_entry_mode,"002",3) == 0 )

		 	{
		 		strcpy(auth_tx.CAF01_details.cvv_offset_tr1,"14");
		 		strcpy(auth_tx.CAF01_details.cvv_offset_tr2,"14");				
		 	}

         /*get TrackI CVV and service code*/
         if(strlen(auth_tx.TLF01_details.track1) != 0)	/*check track one.  make sure it is not empty.*/
		 {
            track_present = 1;
			//get the CVV value
            memset(&dummy,0,Dummy_Size);
            if (1 == parseTrackOne(auth_tx.TLF01_details.track1,dummy ,"^",
                   auth_tx.CAF01_details.cvv_offset_tr1, auth_tx.CAF01_details.cvv_length))
                strcpy(cvc1_data,dummy);
            else
             return(TRANSACTION_NOT_VERIFIED);

            //get service code
            memset(&dummy,0,Dummy_Size);
            if (1 == parseTrackOne(auth_tx.TLF01_details.track1,dummy ,"^",
						auth_tx.CAF01_details.service_code_offset_tr1, "3"))
				   strcpy(cvc1_serviceCode,dummy);
			else
               return(TRANSACTION_NOT_VERIFIED);
		 }
         else 
         /*or get TrackII CVV and service code*/
         if(strlen(auth_tx.TLF01_details.track2) != 0) /*check track two. Make sure it is not empty.*/
		 {
			track_present = 1;
			//get the CVV value
            memset(&dummy,0,Dummy_Size);
            if (1 == parseTrackTwo(auth_tx.TLF01_details.track2,dummy ,"=",
					auth_tx.CAF01_details.cvv_offset_tr2, auth_tx.CAF01_details.cvv_length))
           	   strcpy(cvc1_data,dummy);
            else
                  return(TRANSACTION_NOT_VERIFIED);
            
               //get the service code.
               memset(&dummy,0,Dummy_Size);
               if (1 == parseTrackTwo(auth_tx.TLF01_details.track2,dummy ,"=",
                     auth_tx.CAF01_details.service_code_offset_tr2, "3"))
                  strcpy(cvc1_serviceCode,dummy);
               else
                  return(TRANSACTION_NOT_VERIFIED);
		 
		 }

		 /*Get cvc2 if present*/
		 if(strlen(auth_tx.TLF01_details.cvc_data) != 0)
		 {

				 strcpy(cvc2_data, auth_tx.TLF01_details.cvc_data);
				 strcpy(cvc2_serviceCode, "000");
				 cvc2_present = 1;

		 }
		 /*Get cvc2 if present, if txn is STAND IN or come with other value 29-Jul-2009 
		   If first byte is space(0x20) cvv data copied in the cvc_data 
		   other wise cvv data copied in interest_rate. That values ATP should send as CVV2 data*/

		 else if(strlen(auth_tx.TLF01_details.interest_rate) != 0) 
		 {


				 strcpy(cvc2_data, &auth_tx.TLF01_details.interest_rate[1]);
				 strcpy(cvc2_serviceCode, "000");
				 cvc2_present = 1;

		 }
		
		 /*If neither cvc1 or cvc2 is present, return no value*/
		 if( !track_present && !cvc2_present)
			 return (TRANSACTION_VERIFIED);
		 /* check the CVC data and interest rate which have the cvv2 value,
		 	If it has all zeros, it is not a valid cvv2 hence make cvv2_present has 0*/
		 if(strlen(cvc1_data)!=0)
		 {

			 if(Isallzeros(&auth_tx.TLF01_details.cvc_data,strlen(auth_tx.TLF01_details.cvc_data)) == true)
			 {
			 				 cvc2_present = 0;

			 }
			 if(Isallzeros(&auth_tx.TLF01_details.interest_rate,strlen(auth_tx.TLF01_details.interest_rate)) == true)
			 {
			 				 cvc2_present = 0;

			 }
		 }
		 /*message loop to send cvc1 and/or cvc2 verification messages to Racal and receive responses*/
		 for ( loopidx = 0; loopidx < 2; loopidx++) 
		 {
			 if (loopidx == 0 && track_present) /*cvc1 is present and needs to be verified*/
			 {
				 /*Build message and send to Racal.  */
				 memset (&output,0,sizeof(output));
				 strcat(output, msgHeader);
				 strcat(output, cmdCode);
				 strcat(output, auth_tx.CAF01_details.cvk[0].cvkA);
				 strcat(output, auth_tx.CAF01_details.cvk[0].cvkB);
				 strcat(output, cvc1_data);
				 strcat(output, auth_tx.TLF01_details.card_num);
				 strcat(output, delimiter);
				 strcat(output, auth_tx.TLF01_details.exp_date);
				 strcat(output, cvc1_serviceCode);
			 }
			 else /*cvc2 needs to be verifed if present*/
			 {
				 /*Build message for cvc2 verification and send to Racal*/
				 memset (&output,0,sizeof(output));
				 strcat(output, msgHeader);
				 strcat(output, cmdCode);
				 strcat(output, auth_tx.CAF01_details.cvk[0].cvkA);
				 strcat(output, auth_tx.CAF01_details.cvk[0].cvkB);
				 strcat(output, cvc2_data);
				 strcat(output, auth_tx.TLF01_details.card_num);
				 strcat(output, delimiter);
				 strcat(output, auth_tx.TLF01_details.exp_date);
				 strcat(output, cvc2_serviceCode);

				  cvc2_sent = 1;
			 }
			      
                 
			 //if the output is bigger then 150 do not send it out.
			 if (strlen(output) > 150)
			 {
				racal_log_message( 2, 2, "mag-strip Cmd Msg to long.","racalEquitVerifyCVV_CVC_CSC_CAV",1 );
				if (cvc2_sent)
					return(CVC2_TRANSACTION_NOT_VERIFIED);
				else
					return(TRANSACTION_NOT_VERIFIED);
			 }

			 memset(BUFFER,0,sizeof(BUFFER));
			 sprintf(BUFFER, "BEFORE-PORT: %s", output);
			 MYPRINT(BUFFER);

			 /*The message is built.  Send it over to the Racal box.*/
			 /*Once the a response is recieved, check for the message header,*/
			 /*response code, and the error code.  If it does not exactly match,*/
			 /*send TRANSACTION_NOT_VERIFIED back.*/
			 racal_log_message( 2, 2, output,"racalEquitVerifyCVV_CVC_CSC_CAV",1 );
			 if (racalSendToPort(output) == TRANSACTION_VERIFIED)
			 {
				/*parse the message check for the message header.*/
				memset(&dummy,0,Dummy_Size);
				strncpy(dummy,returnString,4);
				if(strcmp(dummy,msgHeader) != 0)
				{
				   racal_log_message( 2, 2, "mag-strip Msg Header mis-match.","racalEquitVerifyCVV_CVC_CSC_CAV",1 );
				   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}

				/*parse the message check for the response code.*/
				memset(&dummy,0,Dummy_Size);
				strncpy(dummy,returnString + 4, 2);
				if(strcmp(dummy,"CZ") !=0)
				{
				   racal_log_message( 2, 2, "mag-strip Cmd Msg mis-match.","racalEquitVerifyCVV_CVC_CSC_CAV",1 );
				   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}

            /* Parse the message check for the error code. */
            memset(&dummy,0,Dummy_Size);
            strncpy(dummy,returnString + 6, 2);
            if(strcmp(dummy,"00") == 0)
            {
               if (cvc2_sent)
                  return(CVC2_TRANSACTION_VERIFIED);
               else
                  if(!cvc2_present)
                     return (TRANSACTION_VERIFIED); /*cvc1 valid*/
                  else
                     continue; /*continue with loop to verify cvc2*/
            }

				if(strcmp(dummy,"01") == 0)
				{
				   racal_log_message( 2, 2, "CVV-VERIFY failure.","racalEquitVerifyCVV_CVC_CSC_CAV",1 );
				   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}
				if(strcmp(dummy,"10") == 0)
            {
					racal_log_message( 2, 2, "CVV-VERIFY CVK A or B parity error.","racalEquitVerifyCVV_CVC_CSC_CAV",1 );
               	   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}

				if(strcmp(dummy,"12") == 0)
				{
				   racal_log_message( 2, 2, "CVV-VERIFY No Key loaded in user storage.","racalEquitVerifyCVV_CVC_CSC_CAV",1 );
				   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}

				if(strcmp(dummy,"13") == 0)
				{
				   racal_log_message( 2, 2, "CVV-VERIFY LMK error report to superviser.","racalEquitVerifyCVV_CVC_CSC_CAV",1 );
				   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}

				if(strcmp(dummy,"15") == 0)
				{
				   racal_log_message( 2, 2, "CVV-VERIFY Error in input data.","racalEquitVerifyCVV_CVC_CSC_CAV",1 );
				   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}

				if(strcmp(dummy,"21") == 0)
				{
				   racal_log_message( 2, 2, "CVV-VERIFY Invalid user storage index.","racalEquitVerifyCVV_CVC_CSC_CAV",1 );
				   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}
                
			 }
			 else  /* else for: if (racalSendToPort(output) == TRANSACTION_VERIFIED)*/
			 {
				   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
			 }
						   
		 }/*end for loop*/
		 
		 return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
         break;
		         /********************end of Visa Case*****************************************/

   
   default:
      racal_log_message( 2, 2, "Card Type Undefined.","racalEquitVerifyCVV_CVC_CSC_CAV",1 );
      return(TRANSACTION_NOT_VERIFIED);


   }/*end of switch statement.*/

}/*racalEquitVerifyCVV_CVC_CSC_CAV*/



int racalEquitVerify_4_5CSC_AMEX()
{
	   char msgHeader[5];
	   char cmdCode[3]={0},cvc1_serviceCode[4]={0}, cvc2_serviceCode[4]={0};
	   char delimiter[2];
	   char output[200],cardType = 'Z',csc[6]={0};
	   char cvc1_data[4]={0}, cvc2_data[4]={0};
	   int  track_present = 0, cvc2_present = 0, cvc2_sent = 0;
	   int  loopidx = 0;
	   int  cnt=0,result=0,jcnt=0;
	   char BUFFER [256]={0};
	   char strTemp[50] = {0};
	   CHAR *tempptr,fieldSeperator[2]={0};
	   CHAR csc_result5[6];
	   CHAR csc_result4[5];
	   CHAR csc_result3[4];
	   INT 	flag=0;
	   INT  AEVV_result_code;
	   CHAR AEVV_second_factor[3]={0},AEVV_bin_second_factor[2];
	   CHAR AEVV_iCSC[5]={0},AEVV_bin_iCSC[3];
	   CHAR AEVV_UPN[5]={0},AEVV_bin_UPN[3];
	   INT temp_out_len=0;

	   memset (&msgHeader,0,sizeof(msgHeader));
	   memset (&cmdCode,0,sizeof(cmdCode));
	   memset (&delimiter,0,sizeof(delimiter));
	   memset (&output,0,sizeof(output));
	   memset (&returnString,0,ReturnString_Size);
	   memset (&csc,0,sizeof(csc));

	   strcpy (msgHeader ,"1234");


	         /***********************Begin AMEX*********************************/
	         strcat(output,msgHeader);
	         strcat(output,"RY"); /* Changed Command code RY4- TF Phani*/
	         strcat(output,"4");

			 // 4CSC verification
			 	strcat(output,"0");
				flag = 0;

	            /* This is a manually entered transaction.
	             * Note that Amex can have 4-digit CSC's. Since
	             * cvc_data is not large enough for these, the
	             * field 'interest_rate' is used.
	             */

			  if ( auth_tx.TLF01_details.interest_rate[0] != 0x00 )
			   strcpy( csc, auth_tx.TLF01_details.interest_rate );

			  if(auth_tx.TLF01_details.product_codes[18].quantity[0] != 0x00)
				  strncat( csc, auth_tx.TLF01_details.product_codes[18].quantity, 2);

			 if(csc[0] == 'S') // TF Phani First byte is "S" means manulally keyed txn
				strcpy(csc,&csc[1]);

			//TF Phani - CSCK value should follow by the U or T, U for Double , T for triple length
				//	case 32: key scheme = "U"
				//	case 48: key scheme = "T"

			 //enter the CSCK value 32H
			 if(strlen(auth_tx.CAF01_details.csck) != 0)
			 {
				  if(auth_tx.CAF01_details.aba_nbr[0] == '1' || auth_tx.CAF01_details.aba_nbr[0] == 1)
					  strcat(output,"U");
				 strcat(output,auth_tx.CAF01_details.csck);
			 }
	         else
	         {
	            racal_log_message( 2, 2, "mag-strip caf01-csck blank.","racalEquitVerify_4_5CSC_AMEX",1 );
	            return(TRANSACTION_NOT_VERIFIED);
	         }

	         /*the account number is left-justified and zero-filled if less then 19.*/
	         if (strlen(auth_tx.TLF01_details.card_num) != 0)
	         {
	            strcat(output,auth_tx.TLF01_details.card_num);
	            if(strlen(auth_tx.TLF01_details.card_num) != 19)
	               for(cnt = 19 ; (INT)strlen(auth_tx.TLF01_details.card_num) < cnt ;cnt--)
	                  strcat(output,"0");
	         }
	         else
	         {
	            racal_log_message( 2, 2, "mag-strip card_num blank.","racalEquitVerify_4_5CSC_AMEX",1 );
	            return(TRANSACTION_NOT_VERIFIED);
	         }

	         /*do the expiration date.*/
			  if (strlen(auth_tx.TLF01_details.exp_date) != 0)
			  {
	             strcat(output, auth_tx.TLF01_details.exp_date);
			  }
	         else
	         {
	        	 racal_log_message( 2, 2, "mag-strip exp_date blank.","racalEquitVerify_4_5CSC_AMEX",1 );
	        	 return(TRANSACTION_NOT_VERIFIED);
	         }

			  //Copy the 1 digit Rtjusfd Authn result code and 2 digit SF Authn code
			/*strncat(output, auth_tx.TLF01_details.service_code, 3);*/

	        if(strlen(csc) != 0)
			{
	          if(strlen(csc) == 5)
			  {
	        	  strcat(output,csc);
	        	  strcat(output,"FFFFFFF");
	          }
	          else if(strlen(csc) == 4)
			  {
	        	  strcat(output,"FFFFF");
	        	  strcat(output,csc);
	        	  strcat(output,"FFF");
	          }
			}
			 else
				strcat(output,"FFFFFFFFFFFF");

			printf("CSC value : %s \n",csc);

	         //if the output is bigger then 150 do not send it out.
	         if (strlen(output) > 150)
	         {
	            racal_log_message( 2, 2, "mag-strip Cmd Msg to long.","racalEquitVerify_4_5CSC_AMEX",1 );
	            return(TRANSACTION_NOT_VERIFIED);
	         }

	         sprintf(strTemp, " AMEX 4CSC to racal RY: %s", output);
			 racal_log_message( 2, 2,strTemp,"racalEquitVerify_4_5CSC_AMEX",1 );

	         //send the message to the racal box.
	         if (racalSendToPort(output) == TRANSACTION_VERIFIED)
	         {
	        	 sprintf(strTemp, " output of racal cmnd RY: %s", returnString);
	        	 racal_log_message( 2, 2,strTemp,"racalEquitVerify_4_5CSC_AMEX",1 );
	            /*parse the message check for the message header.*/
	            memset(&dummy,0,Dummy_Size);
	            strncpy(dummy,returnString,4);
	            if(strcmp(dummy,msgHeader) != 0)
	            {
	               racal_log_message( 2, 2, "mag-strip Cmd header mis-match.","racalEquitVerify_4_5CSC_AMEX",1 );
	               return(TRANSACTION_NOT_VERIFIED);
	            }
	            /*parse the message check for the response code.*/
	            memset(&dummy,0,Dummy_Size);
	            strncpy(dummy,returnString + 4, 2);
	            if(strcmp(dummy,"RZ") !=0)
	            {
	               racal_log_message( 2, 2, "mag-strip Cmd Code mis-match.","racalEquitVerify_4_5CSC_AMEX",1 );
	               return(TRANSACTION_NOT_VERIFIED);
	            }

	            /*parse the message check for the error code.*/
	            memset(&dummy,0,Dummy_Size);
	            strncpy(dummy,returnString + 6, 2);

				printf("Return from Racal : %s \n",returnString);
	            //if this value is not 00 reject the transaction.
	            if(strcmp(dummy,"00") != 0)
	            {
	               if(strcmp(dummy,"10") == 0)
	               {
	            	  racal_log_message( 2, 2, "mag-strip CSCK parity error.","racalEquitVerify_4_5CSC_AMEX",1 );
				   }
	               if(strcmp(dummy,"12") == 0)
	                  racal_log_message( 2, 2, "mag-strip No Keys in user storage.","racalEquitVerify_4_5CSC_AMEX",1 );
	               if(strcmp(dummy,"13") == 0)
	                  racal_log_message( 2, 2, "mag-strip LMK parity error.","racalEquitVerify_4_5CSC_AMEX",1 );
	               if(strcmp(dummy,"15") == 0)
	                  racal_log_message( 2, 2, "mag-strip Error in input data.","racalEquitVerify_4_5CSC_AMEX",1 );
				   if(strcmp(dummy,"01") == 0)
					{
					   strcpy(auth_tx.TLF01_details.response_text,"Invalid 4 CSC");
					   racal_log_message( 2, 2, "Invalid CSC/CVV/3D sec.","racalEquitVerify_4_5CSC_AMEX",1 );
					}
	// Invalid      CSC           01
	//INvalid  27 CSCK not doubel lenght
	// Invalid  68 HSM command disabled

	               return(TRANSACTION_NOT_VERIFIED);
	            }

	            /*parse the message mode for the error code.*/
	            memset(&dummy,0,Dummy_Size);
	            strncpy(dummy,returnString + 8, 1);
	            if(strcmp(dummy,"4") !=0)
	            {
	               racal_log_message( 2, 2, "mag-strip Msg Mode mis-match.","racalEquitVerify_4_5CSC_AMEX",1 );
	               return(TRANSACTION_NOT_VERIFIED);
	            }

	            if (strlen(csc) == 5)
	            {
	            	memset(&dummy,0,Dummy_Size);
					strncpy(dummy,returnString + 9, 1);
					if(strcmp(dummy,"1") == 0)
					{
						racal_log_message( 2, 2, "5CSC not present.","racalEquitVerify_4_5CSC_AMEX",1 );
						return(TRANSACTION_NOT_VERIFIED);
					}
					else if (strcmp(dummy,"2") == 0)
					{
						racal_log_message( 2, 2, "5CSC verification failed.","racalEquitVerify_4_5CSC_AMEX",1 );
						return(TRANSACTION_NOT_VERIFIED);
					}
					else if (strcmp(dummy,"0") == 0)
					{
						racal_log_message( 2, 2, "5CSC verification passed.","racalEquitVerify_4_5CSC_AMEX",1 );
						return(TRANSACTION_VERIFIED);
					}
	            }

	            if (strlen(csc) == 4)
	            {
	            	memset(&dummy,0,Dummy_Size);
					strncpy(dummy,returnString + 10, 1);
					if(strcmp(dummy,"1") == 0)
					{
						racal_log_message( 2, 2, "4CSC not present.","racalEquitVerify_4_5CSC_AMEX",1 );
						return(TRANSACTION_NOT_VERIFIED);
					}
					else if (strcmp(dummy,"2") == 0)
					{
						racal_log_message( 2, 2, "4CSC verification failed.","racalEquitVerify_4_5CSC_AMEX",1 );
						return(TRANSACTION_NOT_VERIFIED);
					}
					else if (strcmp(dummy,"0") == 0)
					{
						racal_log_message( 2, 2, "4CSC verification passed.","racalEquitVerify_4_5CSC_AMEX",1 );
						return(TRANSACTION_VERIFIED);
					}
	            }



	            /* Racal returns a 5-digit CSC, 4-digit CSC, and a 3-digit CSC.
	             * Compare our CSC against all three, just to make sure we are
	             * comparing against the correct one.
	             */
	/*            memset( csc_result5, 0x00, sizeof(csc_result5) );
	            memset( csc_result4, 0x00, sizeof(csc_result4) );
	            memset( csc_result3, 0x00, sizeof(csc_result3) );

	            memcpy( csc_result5, returnString+ 9, 5 );
	            memcpy( csc_result4, returnString+14, 4 );
	            memcpy( csc_result3, returnString+18, 3 );

	            if ( (0 != strcmp(csc_result5, csc)) &&
	                 (0 != strcmp(csc_result4, csc)) &&
	                 (0 != strcmp(csc_result3, csc))  )
	            {
	               racal_log_message( 2, 2, "mag-strip Calculate CSC mis-match.","racalEquitVerify_4_5CSC_AMEX",1 );
	               memset(BUFFER,0,sizeof(BUFFER));
	               sprintf(BUFFER,
	                      "mag-strip R5:%s, R4:%s, R3:%s - T:%s",
	                       csc_result5, csc_result4, csc_result3 , csc);
	               racal_log_message( 2, 2, BUFFER,"racalEquitVerify_4_5CSC_AMEX",1 );
	               return(TRANSACTION_NOT_VERIFIED);
	            }*/

	#ifdef _DEBUG
	            racal_log_message( 2, 1, "mag-strip PASSED.","racalEquitVerify_4_5CSC_AMEX",1 );
	#endif
	            return(TRANSACTION_VERIFIED);

	         }/*end of if racalSendToPort*/
	         else
	            return(TRANSACTION_NOT_VERIFIED);


	         /****************************end AMEX***************************************/
}


// racalGenarateDinersKey
/******************************************************************/
/*
This Function is specific to Diners - to generate  the Diners key
ATP should be able generate the key with ZMK ,

Input  : ZMK given by Diner
Outpot:  1. generated key enc under LMK
		 2. generated key enc under ZMK

*/
/****************************************************************/
INT racalGenarateDinersKey()
{
   INT   ret_val = TRANSACTION_NOT_VERIFIED;
   INT   len;
   INT   i=0,j=0,k=0;
   INT   racal_ret_code;
   CHAR  key_len_indicator[2] = "";
   CHAR  errbuf[100]  = "";
   CHAR  msgHeader[5];
   CHAR  cmdCode[3];
   CHAR  output[200];
   char delimiter[2];

   memset( &output,       0x00, sizeof(output)    );
   memset( &returnString, 0x00, ReturnString_Size );
   strcpy(delimiter,";");
   strcpy( msgHeader ,"1234" );
   strcpy( cmdCode,   "A0"   );

   strcat( output, msgHeader );
   strcat( output, cmdCode   );

   // Mode 01
   strcat( output, "1"   );
   // Key type : 001
   strcat( output, "001"   );
   // Key SCheme LMK
   strcat( output, "U" ); // update the filed from appendix C pay shield 9000
   // ; delimiter
   strcat( output, delimiter  );
   // ZMK flag : 0
   strcat( output, "0"  );
   //

   len = strlen( ncf01.zmk );

  // printf("ZMK value : %s \n",ncf01.zmk);
   if ( len < 16)
   {
      strcpy( errbuf, "Key Change: ZMK is < 16." );
      racal_log_message( 2, 2,errbuf,"racalGenarateDinersKey",1 );
      return TRANSACTION_NOT_VERIFIED;
   }
   else
   {
	   strcat( output, "U"  );
	   strcat( output,  ncf01.zmk  );
   }

   // Key Scheme
   strcat( output, "X"  );

		if ( strlen(output) > MAX_CMD_LEN )
		{
			len = strlen(output);
			sprintf( errbuf,"Diners Key Gen: Cmd Msg too long: %d, max = %d",
			len, MAX_CMD_LEN );
		}/* Send a message to the Racal box. */
		else if ( racalSendToPort(output) == TRANSACTION_VERIFIED )
		{
			/* The command was successfully sent and returned here.
			* Check the results.
			*/
			memset( &dummy, 0x00, Dummy_Size );
			memcpy( dummy,  returnString, 4 );

			/* Check to see if the same header msg is back. Should be echoed. */
			if ( strcmp(msgHeader,dummy) != 0 )
			{
				sprintf( errbuf,
				"Diners Key Generate: Msg Header mismatch. Sent = %s, Returned = %s",
				msgHeader, dummy );
			} else
			{
				/* Message header was successfully echoed back to us.
				* Verify the Command Code.
				*/
				memset( &dummy, 0x00, Dummy_Size );
				memcpy( dummy,  returnString + 4, 2 );

				if ( strcmp(dummy,"A1") != 0 )
				{
				  strcpy( errbuf, " Diners Key generate: Cmd Code mismatch. Expected A1") ;
				}else
				{

				   /* Message Header and Command Code are good.
					* Check the return code.
					*/
				   memset( &dummy,0,Dummy_Size );
				   strncpy( dummy,returnString + 6,2 );
				   racal_ret_code = atoi( dummy );

				   switch( racal_ret_code )
				   {
					  case 1:

						 /* This is a special error code.  It is only an advice.
						  * So we will post a warning, but then flow the code
						  * down through 'case 0' and treat this as if it was
						  * successful.
						  */
						  sprintf( errbuf,"Diner Key Generate: retcode=%s, ZPK parity error, advice only",dummy );

					break;
					  case 0:  /* Successful Action - Get encrypted key. */

						 /* Determine length of key. */
						 memset( &dummy,0,Dummy_Size );
						 key_len_indicator[0] = returnString[8];

						 switch( key_len_indicator[0] )
						 {
							case cDOUBLE_LEN_KEY:
							case cDOUBLE_LEN_KEY_X9_17:

							   len = 32;
							   i   =  1;
							   break;
							default:
							   len = 16;
							   i   =  0;
							   break;
						 }

						 memcpy( dummy, returnString + 8 + i, len );
						 strcpy( auth_tx.TLF01_details.source_key,dummy );
						 memset( &dummy,0,Dummy_Size );
						 j = 9+len;
						 key_len_indicator[0] = returnString[j];

						 switch( key_len_indicator[0] )
						 {
							case cDOUBLE_LEN_KEY:
							case cDOUBLE_LEN_KEY_X9_17:

							   len = 32;
							   i   =  1;
							   break;

							default:
							   len = 16;
							   i   =  0;
							   break;
						 }

						 memcpy( dummy, returnString + j + i, len );
						 strcpy( auth_tx.TLF01_details.dest_key,dummy );
						 k = j+i+len;
						 memcpy( auth_tx.TLF01_details.kek,returnString+k,6 );
						 ret_val = TRANSACTION_VERIFIED;
						 break;

					  case 10:

						 sprintf( errbuf,
								 "Diner Key Generate: retcode=%s, ZMK parity error",
								  dummy );
						 break;

					  case 11:

						 sprintf( errbuf,
								 "Diner Key Generate: retcode=%s, All zero ZPK with even parity.",
								  dummy );
						 break;

					  case 12:

						 sprintf( errbuf,
								 "Diner Key Generate: retcode=%s, No keys loaded in user storage.",
								  dummy );
						 break;

					  case 13:

						 sprintf( errbuf,
								 "Diner Key Generate: retcode=%s, LMK error; report to supervisor.",
								  dummy );
						 break;

						 default:

						 sprintf( errbuf,
								 "Diner Key Generate: retcode=%d, Unexpected return code.",
								  dummy );
						 break;
				   }

				}
			}
		}
		if ( errbuf[0] != 0x00 )
		{
		racal_log_message( 2, 2,errbuf,"racalGenarateDinersKey",1 );
		return TRANSACTION_NOT_VERIFIED;
		}
   return( ret_val );

}//end of racalGenarateDinersKey();


/******************************************************************/
/*
This Function is specifc to CUP - to change the CUP key to LMK
ATP should be able to translate the ZPK received in the Key Exchange message from ZMK to LMK.

*/
/****************************************************************/
INT racalCUPExchangeKey_ZMK_LMK()
{
   INT   ret_val = TRANSACTION_NOT_VERIFIED;
   INT   len;
   INT   i;
   INT   racal_ret_code;
   CHAR  key_len_indicator[2] = "";
   CHAR  errbuf[100]  = "";
   CHAR  msgHeader[5];
   CHAR  cmdCode[3];
   CHAR  output[200];
   CHAR  delimiter[3]={0};
   CHAR  strTemp[256] = {0};

   memset( &output,       0x00, sizeof(output)    );
   memset( &returnString, 0x00, ReturnString_Size );
   strcpy(delimiter,";");
   strcpy( msgHeader ,"8543" );
   strcpy( cmdCode,   "FA"   );

   strcat( output, msgHeader );
   strcat( output, cmdCode   );

   len = strlen( auth_tx.TLF01_details.dest_key );
   if ( len < 16)
   {
      strcpy( errbuf, "Key Change: ZMK is < 16." );
   }
   else
   {
      switch( len )
      {
         case 32:  strcat( output, DOUBLE_LEN_KEY ); break;
         case 48:  strcat( output, TRIPLE_LEN_KEY ); break;
      }
      strcat( output, auth_tx.TLF01_details.dest_key );

      len = strlen( auth_tx.TLF01_details.source_key );
      if ( len <= 0 )
      {
         strcpy( errbuf, "Key Change: Source Key (ZPK) is blank" );
      }
      else
      {
         switch( len )
         {
            case 32:  strcat( output, DOUBLE_LEN_KEY_X9_17 ); break;
            case 48:  strcat( output, TRIPLE_LEN_KEY_X9_17 ); break;
         }
         strcat(output,auth_tx.TLF01_details.source_key);
         /*Specific code for PULSe - Phani*/
         if(strncmp(auth_tx.TLF01_details.handler_queue,"ncpul",4) == 0)
         {
        	 /*Atala variant*/
        	 strcat(output,"1");

        	/* strcat(output,delimiter);
        	 strcat(output,"0");
        	 strcat(output,"0");
        	 strcat(output,auth_tx.TLF01_details.nfi_name);*/ // KCV

         }
         /* The command is now composed. Check the length then send it. */
         sprintf(strTemp, " input of racal FA: %s", output);
     	 racal_log_message( 2, 2,strTemp,"racalCUPExchangeKey_ZMK_LMK",1 );

        // printf("Sending to RACAL : Test remove me later\n");
         if ( strlen(output) > MAX_CMD_LEN )
         {
            sprintf( errbuf,
                    "Key Change: Cmd Msg too long: %d, max = %d",
                     len, MAX_CMD_LEN );
         }

         /* Send a message to the Racal box. */
         else if ( racalSendToPort(output) == TRANSACTION_VERIFIED )
         {
            /* The command was successfully sent and returned here.
             * Check the results.
             */
            memset( &dummy, 0x00, Dummy_Size );
            memcpy( dummy,  returnString, 4 );

            sprintf(strTemp, " output of racal FB: %s", output);
            racal_log_message( 2, 2,strTemp,"racalCUPExchangeKey_ZMK_LMK",1 );

            /* Check to see if the same header msg is back. Should be echoed. */
            if ( strcmp(msgHeader,dummy) != 0 )
            {
               sprintf( errbuf,
                       "Key Change: Msg Header mismatch. Sent = %s, Returned = %s",
                        msgHeader, dummy );
            }
            else
            {
               /* Message header was successfully echoed back to us.
                * Verify the Command Code.
                */
               memset( &dummy, 0x00, Dummy_Size );
               memcpy( dummy,  returnString + 4, 2 );

               if ( strcmp(dummy,"FB") != 0 )
               {
                  strcpy( errbuf, "Key Change: Cmd Code mismatch. Expected FB") ;
               }
               else
               {
                  /* Message Header and Command Code are good.
                   * Check the return code.
                   */
                  memset( &dummy,0,Dummy_Size );
                  strncpy( dummy,returnString + 6,2 );
                  racal_ret_code = atoi( dummy );

                  switch( racal_ret_code )
                  {
                     case 1:

                        /* This is a special error code.  It is only an advice.
                         * So we will post a warning, but then flow the code
                         * down through 'case 0' and treat this as if it was
                         * successful.
                         */
                        sprintf( errbuf,
                                "Key Change: retcode=%s, ZPK parity error, advice only",
                                 dummy );

                     case 0:  /* Successful Action - Get encrypted key. */

                        /* Determine length of key. */
                        memset( &dummy,0,Dummy_Size );
                        key_len_indicator[0] = returnString[8];

                        switch( key_len_indicator[0] )
                        {
                           case cDOUBLE_LEN_KEY:
                           case cDOUBLE_LEN_KEY_X9_17:

                              len = 32;
                              i   =  1;
                              break;

                           case cTRIPLE_LEN_KEY:
                           case cTRIPLE_LEN_KEY_X9_17:

                              len = 48;
                              i   =  1;
                              break;

                           default:
                              len = 16;
                              i   =  0;
                              break;
                        }

                        memcpy( dummy, returnString + 8 + i, len );
                        strcpy( auth_tx.TLF01_details.dest_key,dummy );
                        ret_val = TRANSACTION_VERIFIED;
                        break;

                     case 10:

                        sprintf( errbuf,
                                "Key Change: retcode=%s, ZMK parity error",
                                 dummy );
                        break;

                     case 11:

                        sprintf( errbuf,
                                "Key Change: retcode=%s, All zero ZPK with even parity.",
                                 dummy );
                        break;

                     case 12:

                        sprintf( errbuf,
                                "Key Change: retcode=%s, No keys loaded in user storage.",
                                 dummy );
                        break;

                     case 13:

                        sprintf( errbuf,
                                "Key Change: retcode=%s, LMK error; report to supervisor.",
                                 dummy );
                        break;

                     case 15:

                        sprintf( errbuf,
                                "Key Change: retcode=%s, Error in input data.",
                                 dummy );
                        break;

                     case 21:

                        sprintf( errbuf,
                                "Key Change: retcode=%s, Invalid user storage index.",
                                 dummy );
                        break;

                     default:

                        sprintf( errbuf,
                                "Key Change: retcode=%d, Unexpected return code.",
                                 dummy );
                        break;
                  }
               }
            }
         }
         else
         {
            strcpy( errbuf, "Key Change: Unable to send request to racal box.");
         }
      }
   }

   /* Is there an error message to log? */
   if ( errbuf[0] != 0x00 )
   {
      /* YES */
      if ( auth_tx.TLF01_details.terminal_id[0] == 0x00 )
      {
         racal_log_message( 2, 2,errbuf,"racalCUPExchangeKey_ZMK_LMK",1 );
      }
      else
      {
         racal_log_message( 2, 2,errbuf,"racalCUPExchangeKey_ZMK_LMK",1 );
      }
   }
   return( ret_val );

}//end of racalEquitKeyExchangeKey();


/****************************************************************/
/****************************************************************/
INT racalEquitKeyExchangeKey()
{
   INT   ret_val = TRANSACTION_NOT_VERIFIED;
   INT   len;
   INT   i;
   INT   racal_ret_code;
   CHAR  key_len_indicator[2] = "";
   CHAR  errbuf[100]  = "";
   CHAR  msgHeader[5];
   CHAR  cmdCode[3];
   CHAR  output[200];

   memset( &output,       0x00, sizeof(output)    );
   memset( &returnString, 0x00, ReturnString_Size );

   strcpy( msgHeader ,"8543" );
   strcpy( cmdCode,   "FA"   );

   strcat( output, msgHeader );
   strcat( output, cmdCode   );

   len = strlen( ZMK );
   if ( len < 16)
   {
      strcpy( errbuf, "Key Change: ZMK is < 16." );
   }
   else
   {
      switch( len )
      {
         case 32:  strcat( output, DOUBLE_LEN_KEY ); break;
         case 48:  strcat( output, TRIPLE_LEN_KEY ); break;
      }
      strcat( output, ZMK );

      len = strlen( auth_tx.TLF01_details.source_key );
      if ( len <= 0 )
      {
         strcpy( errbuf, "Key Change: Source Key (ZPK) is blank" );
      }
      else
      {
         switch( len )
         {
            case 32:  strcat( output, DOUBLE_LEN_KEY_X9_17 ); break;
            case 48:  strcat( output, TRIPLE_LEN_KEY_X9_17 ); break;
         }
         strcat(output,auth_tx.TLF01_details.source_key);

         /* The command is now composed. Check the length then send it. */
         if ( strlen(output) > MAX_CMD_LEN )
         {
            sprintf( errbuf,
                    "Key Change: Cmd Msg too long: %d, max = %d",
                     len, MAX_CMD_LEN );
         }

         /* Send a message to the Racal box. */
         else if ( racalSendToPort(output) == TRANSACTION_VERIFIED )
         {
            /* The command was successfully sent and returned here.
             * Check the results.
             */
            memset( &dummy, 0x00, Dummy_Size );
            memcpy( dummy,  returnString, 4 );

            /* Check to see if the same header msg is back. Should be echoed. */
            if ( strcmp(msgHeader,dummy) != 0 )
            {
               sprintf( errbuf,
                       "Key Change: Msg Header mismatch. Sent = %s, Returned = %s",
                        msgHeader, dummy );
            }
            else
            {
               /* Message header was successfully echoed back to us.
                * Verify the Command Code.
                */
               memset( &dummy, 0x00, Dummy_Size );
               memcpy( dummy,  returnString + 4, 2 );

               if ( strcmp(dummy,"FB") != 0 )
               {
                  strcpy( errbuf, "Key Change: Cmd Code mismatch. Expected FB") ;
               }
               else
               {
                  /* Message Header and Command Code are good.
                   * Check the return code.
                   */
                  memset( &dummy,0,Dummy_Size );
                  strncpy( dummy,returnString + 6,2 );
                  racal_ret_code = atoi( dummy );

                  switch( racal_ret_code )
                  {
                     case 1:

                        /* This is a special error code.  It is only an advice.
                         * So we will post a warning, but then flow the code
                         * down through 'case 0' and treat this as if it was
                         * successful.
                         */
                        sprintf( errbuf,
                                "Key Change: retcode=%s, ZPK parity error, advice only",
                                 dummy );

                     case 0:  /* Successful Action - Get encrypted key. */

                        /* Determine length of key. */
                        memset( &dummy,0,Dummy_Size );
                        key_len_indicator[0] = returnString[8];

                        switch( key_len_indicator[0] )
                        {
                           case cDOUBLE_LEN_KEY:
                           case cDOUBLE_LEN_KEY_X9_17:

                              len = 32;
                              i   =  1;
                              break;

                           case cTRIPLE_LEN_KEY:
                           case cTRIPLE_LEN_KEY_X9_17:

                              len = 48;
                              i   =  1;
                              break;

                           default:
                              len = 16;
                              i   =  0;
                              break;
                        }

                        memcpy( dummy, returnString + 8 + i, len );
                        strcpy( auth_tx.TLF01_details.dest_key,dummy );
                        ret_val = TRANSACTION_VERIFIED;
                        break;

                     case 10:

                        sprintf( errbuf,
                                "Key Change: retcode=%s, ZMK parity error",
                                 dummy );
                        break;

                     case 11:

                        sprintf( errbuf,
                                "Key Change: retcode=%s, All zero ZPK with even parity.",
                                 dummy );
                        break;

                     case 12:

                        sprintf( errbuf,
                                "Key Change: retcode=%s, No keys loaded in user storage.",
                                 dummy );
                        break;

                     case 13:

                        sprintf( errbuf,
                                "Key Change: retcode=%s, LMK error; report to supervisor.",
                                 dummy );
                        break;

                     case 15:

                        sprintf( errbuf,
                                "Key Change: retcode=%s, Error in input data.",
                                 dummy );
                        break;

                     case 21:

                        sprintf( errbuf,
                                "Key Change: retcode=%s, Invalid user storage index.",
                                 dummy );
                        break;

                     default:

                        sprintf( errbuf,
                                "Key Change: retcode=%d, Unexpected return code.",
                                 dummy );
                        break;
                  }
               }
            }
         }
         else
         {
            strcpy( errbuf, "Key Change: Unable to send request to racal box.");
         }
      }
   }

   /* Is there an error message to log? */
   if ( errbuf[0] != 0x00 )
   {
      /* YES */
      if ( auth_tx.TLF01_details.terminal_id[0] == 0x00 )
      {
         racal_log_message( 2, 2,errbuf,"racalEquitKeyExchangeKey",1 );
      }
      else
      {
         racal_log_message( 2, 2,errbuf,"racalEquitKeyExchangeKey",1 );
      }
   }
   return( ret_val );

}//end of racalEquitKeyExchangeKey();



/****************************************************************/
INT racalSendToPort_Bin(char output[],INT len)
{
   pPTE_MSG        p_msg_out;
   pPTE_MSG        p_msg_port_in;
   LONG            ret_code=0;
   CHAR            ErrorMsg[100];
   CHAR            Buffer  [256];
   CHAR            dest_queue[] = "racalconA";
   pPTE_MSG_DATA   p_msg_data;
   pBYTE           p_data;
   INT             number;
   INT             ret_val = TRANSACTION_VERIFIED;

   /* Build and send a message to the racal box. */
   p_msg_out = ptemsg_build_msg( MT_OUTGOING,
                                 0,
                                 0,
                                 dest_queue,
                                 MyQueName,
                                (pBYTE)output,
                                 len,
                                 0);
   if ( p_msg_out != NULL_PTR )
   {
      /* Send and wait for a response for the Racal box. */
      ptemsg_set_orig_comms_info(p_msg_out,"*");
      p_msg_port_in = pteipc_send_receive( p_msg_out,
                                           dest_queue,
                                           MyInteractiveQueName,
                                          &ret_code );
      if( ret_code < 0 )
      {
         pteipc_get_errormsg( ret_code, ErrorMsg );
         sprintf( Buffer,
                 "racalSendToPort: Unable to send request to racal box, que %s: %s",
                  dest_queue, ErrorMsg );
         racal_log_message( 1, 3,Buffer,"racalSendToPort_Bin",1 );
         ret_val = TRANSACTION_NOT_VERIFIED;
      }
      else
      {
         /* Parse the response and send back. */
         p_msg_data = ptemsg_get_pte_msg_data(p_msg_port_in);
         p_data     = ptemsg_get_pte_msg_data_data(p_msg_data);
         number     = ptemsg_get_pte_msg_data_data_length(p_msg_data);

         /* This prevents racal from crashing. */
         if((number > ReturnString_Size-1))
         {
            sprintf( Buffer,
                    "Msg returned from Racal too long: %d, expected max of %d",
                     number, ReturnString_Size-1 );
            racal_log_message( 2, 2,Buffer,"racalSendToPort_Bin",1 );
            ret_val = TRANSACTION_NOT_VERIFIED;
         }
         else
         {
            /* Transaction has been successfully sent and received. */
            memcpy( returnString, p_data, ReturnString_Size );
            returnString[number] = '\0';
         }
      }
      free( p_msg_port_in );
   }
   else
   {
      racal_log_message( 2, 2,"Insufficient memory to build request to racal box","racalSendToPort_Bin",1 );
      ret_val = TRANSACTION_NOT_VERIFIED;
	}

   return( ret_val );
}

/****************************************************************/
INT racalSendToPort(char output[])
{
   pPTE_MSG        p_msg_out;
   pPTE_MSG        p_msg_port_in;
   LONG            ret_code=0;
   CHAR            ErrorMsg[100];
   CHAR            Buffer  [256];
   CHAR            dest_queue[] = "racalconA";
   pPTE_MSG_DATA   p_msg_data;
   pBYTE           p_data;
   INT             number;
   INT             ret_val = TRANSACTION_VERIFIED;

   /* Build and send a message to the racal box. */
   p_msg_out = ptemsg_build_msg( MT_OUTGOING,
                                 0,
                                 0,
                                 dest_queue,
                                 MyQueName,
                                (pBYTE)output,
                                 strlen(output),
                                 0);
   if ( p_msg_out != NULL_PTR )
   {
      /* Send and wait for a response for the Racal box. */
      ptemsg_set_orig_comms_info(p_msg_out,"*");
      p_msg_port_in = pteipc_send_receive( p_msg_out,
                                           dest_queue,
                                           MyInteractiveQueName,
                                          &ret_code );
      if( ret_code < 0 )
      {
         pteipc_get_errormsg( ret_code, ErrorMsg );
         sprintf( Buffer,
                 "racalSendToPort: Unable to send request to racal box, que %s: %s",
                  dest_queue, ErrorMsg );
         racal_log_message( 2, 2,Buffer,"racalSendToPort",1 );
         ret_val = TRANSACTION_NOT_VERIFIED;
      }
      else
      {
         /* Parse the response and send back. */
         p_msg_data = ptemsg_get_pte_msg_data(p_msg_port_in);
         p_data     = ptemsg_get_pte_msg_data_data(p_msg_data);
         number     = ptemsg_get_pte_msg_data_data_length(p_msg_data);

         /* This prevents racal from crashing. */
         if((number > ReturnString_Size-1))
         {
            sprintf( Buffer,
                    "Msg returned from Racal too long: %d, expected max of %d",
                     number, ReturnString_Size-1 );
            racal_log_message( 2, 2,Buffer,"racalSendToPort",1 );
            ret_val = TRANSACTION_NOT_VERIFIED;
         }
         else
         {
            /* Transaction has been successfully sent and received. */
            memcpy( returnString, p_data, ReturnString_Size );
            returnString[number] = '\0';
         }
      }
      free( p_msg_port_in );
   }
   else
   {
	   racal_log_message( 2, 2,"Insufficient memory to build request to racal box","racalSendToPort",1 );
	   ret_val = TRANSACTION_NOT_VERIFIED;
	}

   return( ret_val );
}

INT racalSendToPort_MC(char output[], int data_len)
{
   pPTE_MSG        p_msg_out;
   pPTE_MSG        p_msg_port_in;
   LONG            ret_code=0;
   CHAR            ErrorMsg[100];
   CHAR            Buffer  [256];
   CHAR            dest_queue[] = "racalconA";
   pPTE_MSG_DATA   p_msg_data;
   pBYTE           p_data;
   INT             number;
   INT             ret_val = TRANSACTION_VERIFIED;

   /* Build and send a message to the racal box. */
   p_msg_out = ptemsg_build_msg( MT_OUTGOING,
                                 0,
                                 0,
                                 dest_queue,
                                 MyQueName,
                                (pBYTE)output,
								data_len,
                                 0);
   if ( p_msg_out != NULL_PTR )
   {
      /* Send and wait for a response for the Racal box. */
      ptemsg_set_orig_comms_info(p_msg_out,"*");
      p_msg_port_in = pteipc_send_receive( p_msg_out,
                                           dest_queue,
                                           MyInteractiveQueName,
                                          &ret_code );
      if( ret_code < 0 )
      {
         pteipc_get_errormsg( ret_code, ErrorMsg );
         sprintf( Buffer,
                 "racalSendToPort: Unable to send request to racal box, que %s: %s",
                  dest_queue, ErrorMsg );
         racal_log_message( 2, 2,Buffer,"racalSendToPort",1 );
         ret_val = TRANSACTION_NOT_VERIFIED;
      }
      else
      {
         /* Parse the response and send back. */
         p_msg_data = ptemsg_get_pte_msg_data(p_msg_port_in);
         p_data     = ptemsg_get_pte_msg_data_data(p_msg_data);
         number     = ptemsg_get_pte_msg_data_data_length(p_msg_data);

         /* This prevents racal from crashing. */
         if((number > ReturnString_Size-1))
         {
            sprintf( Buffer,
                    "Msg returned from Racal too long: %d, expected max of %d",
                     number, ReturnString_Size-1 );
            racal_log_message( 2, 2,Buffer,"racalSendToPort",1 );
            ret_val = TRANSACTION_NOT_VERIFIED;
         }
         else
         {
            /* Transaction has been successfully sent and received. */
            memcpy( returnString, p_data, ReturnString_Size );
            returnString[number] = '\0';
         }
      }
      free( p_msg_port_in );
   }
   else
   {
	   racal_log_message( 2, 2,"Insufficient memory to build request to racal box","racalSendToPort",1 );
	   ret_val = TRANSACTION_NOT_VERIFIED;
	}

   return( ret_val );
}

/*************************************************************************************
       NAME:           generateMsgHeader()

       DESCRIPTION:    Generate a brand new message header.  The max value is 9999.
                       
       INPUTS:
			
       OUTPUTS:        return the message header.
           None
       RETURNS:        
		     None.                       
                               
      AUTHOR:         Faramarz Arad 3/11/1999
      MODIFIED BY:    
*************************************************************************************/
void  generateMsgHeader(char returnStr[])
{
   int cnt = 0;
   CHAR msgHeader[5]= {0};
   
   returnStr[0] = '\0';

   if(9999 == msgHeaderCnt)
      msgHeaderCnt = 0;
   
   msgHeaderCnt++;

   itoa(msgHeaderCnt,msgHeader,10);  

   if(strlen(msgHeader) != 4)
      for(cnt = 4 ; (INT)strlen(msgHeader) < cnt ;cnt--)
         strcat(returnStr,"0");

   strcat(returnStr,msgHeader);
   
}



//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
int   parseTrackTwo(char trackTwo[], char retStr[], char fieldSeperator[], char position[], char length[])
{
   char *pdest,returnString [20]={0};
   pdest = strstr(trackTwo,fieldSeperator);

   if( (atoi(position) < 1) || (atoi(length) < 1))
      racal_log_message( 2, 2,"Bad position or length to parse on Track 2.","parseTrackTwo",1 );
   if(pdest != NULL)
      ++pdest ;
   else
   {
      racal_log_message( 2, 2,"mag-strip Track 1 bad info..","parseTrackTwo",1 );
      return(FALSE);
   }
     
   memset(&returnString,0,sizeof(returnString));
   strncpy(returnString, pdest + atoi(position) ,atoi(length));

   strcpy(retStr,returnString);

   return(TRUE);
}  //end of parseTrackTwo


//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
int   parseTrackOne(char trackOne[], char retStr[], char fieldSeperator[], char position[], char length[])
{
   char *pdest,*pdest2,returnString [20]={0};
   
   if( (atoi(position) < 1) || (atoi(length) < 1))
      racal_log_message( 2, 2,"Bad position or length to parse on Track 1.","parseTrackOne",1 );
   pdest = strstr(trackOne,fieldSeperator);
   if(pdest != NULL)
   {
      ++pdest;
      pdest2 = strstr(pdest,fieldSeperator);
      if (pdest2 != NULL)
      {
         ++pdest2;
         memset(&returnString,0,sizeof(returnString));
         strncpy(returnString, pdest2 + atoi(position),atoi(length));
      }
      else
         {
            racal_log_message( 2, 2,"mag-strip Track 1 bad info.","parseTrackOne",1 );
            return(FALSE);
         }
   }
   else
      {
         racal_log_message( 2, 2,"mag-strip Track 1 bad info.","parseTrackOne",1 );
         return(FALSE);
      }

    strcpy(retStr,returnString);
    return(TRUE);
}  //end of parseTrackOne

/******************************************/
/*Tags '9F02', '9F03', '9F1A', '95', '5F2A', '9A', '9C', '9F37', '82'
,'9F36', '9F34'
*/

INT get_EMV_CVN_01_data_CUP( INT emv_len_in, pBYTE emv_data_in,pBYTE tag_data )
{
   INT   idx_in;
   INT   idx_out = 0,emv_org_len;
   INT   tag_len;
   INT   data_len;
   INT   supported;
   BYTE  byteOneOfTag;
   BYTE  byteTwoOfTag;
   BYTE  emv_out_len[4] = "", TAGS_temp[50]={0};
   BYTE temp_tag_data[100]={0};


  if(0 == strcmp(auth_tx.TLF01_details.handler_queue,"dcpisoA"))
  {
	 /* Step past 2-byte length field. */
	   idx_in = 2;
  }
  else
  {
	   idx_in = 3;

  }

   /* Step through input, searching for subfields supported . */
   while( idx_in < emv_len_in )
   {
      /* Get the 1 or 2 byte subfield (tag) identifier. */
      byteOneOfTag = emv_data_in[idx_in++];
      if ( (byteOneOfTag & 0x1F) == 0x1F )
      {
         byteTwoOfTag = emv_data_in[idx_in++];
         tag_len = 2;
      }
      else
      {
         byteTwoOfTag = 0x00;
         tag_len = 1;
      }

      /* Get the length of this tag. */
      data_len = emv_data_in[idx_in++];

      /* Check to see if this tag is supported */
      supported = true;
      if ( byteOneOfTag == 0x5F )
      {
         if ( byteTwoOfTag != 0x2A )
            supported = false;
		 else
		 	memcpy( &tag_data[TX_DATA_5F2A_POS],
						&emv_data_in[idx_in],
						 data_len );
      }
      else if (byteOneOfTag == 0x9F )
      {
         switch( byteTwoOfTag )
         {
            case 0x02:
				//1-6
				memcpy( &tag_data[TX_DATA_9F02_POS],
						&emv_data_in[idx_in],
						 data_len );

				break;
            case 0x03:
				//6-12
				memcpy( &tag_data[TX_DATA_9F03_POS],
										&emv_data_in[idx_in],
										 data_len );

								break;

            case 0x10:
							memcpy( &tag_data[TX_DATA_9F10_POS],
							&emv_data_in[idx_in],
							data_len );


								break;
            case 0x1A:
				//6-12
				memcpy( &tag_data[TX_DATA_9F1A_POS],
										&emv_data_in[idx_in],
										 data_len );

								break;
            case 0x36:
				//6-12
				memcpy( &tag_data[TX_DATA_9F36_POS],
										&emv_data_in[idx_in],
										 data_len );

								break;
            case 0x37:
				//6-12
				memcpy( &tag_data[TX_DATA_9F37_POS],
										&emv_data_in[idx_in],
										 data_len );

								break;

            default:   supported = false;
                       break;
         }
      }
      else
      {
         switch( byteOneOfTag )
         {
            case 0x82:
				memcpy( &tag_data[TX_DATA_82_POS],
										&emv_data_in[idx_in],
										 data_len );

            break;
            case 0x95:
				memcpy( &tag_data[TX_DATA_95_POS],
										&emv_data_in[idx_in],
										 data_len );

            break;
            case 0x9A: // 21-24
				memcpy( &tag_data[TX_DATA_9A_POS],
										&emv_data_in[idx_in],
										 data_len );

            break;
            case 0x9C:
				memcpy( &tag_data[TX_DATA_9C_POS],
										&emv_data_in[idx_in],
										 data_len );

			break;

            default: supported = false;
                     break;
         }
      }

      /* Copy supported  tags into outgoing host buffer. */
      if ( supported )
      {
         /* Subfield Tag */
         temp_tag_data[idx_out] = byteOneOfTag;
         if ( byteTwoOfTag != 0x00 )
            temp_tag_data[idx_out+1] = byteTwoOfTag;

         /* Subfield Length */
         temp_tag_data[idx_out+tag_len] = data_len;


         /* Increment the Outgoing Buffer index (tag+length+data) */
         idx_out += tag_len + 1 + data_len;
      }

      idx_in += data_len;
   }
  		  if (strcmp(auth_tx.BIN01_details.card_family,CARD_CUP) == 0 )
  		  {
			   tag_data[TX_DATA_9F10_POS + 3]=0x80;
			   tag_data[TX_DATA_9F10_POS + 4]=0x80;
			   tag_data[TX_DATA_9F10_POS + 5]=0x80;
			   tag_data[TX_DATA_9F10_POS + 6]=0x80;
  		  }

   return idx_out;
}

INT get_EMV_CVN_05_data_Diners( INT emv_len_in, pBYTE emv_data_in,pBYTE tag_data)
{

	   INT   idx_in;
	   INT   idx_out = 0,emv_org_len;
	   INT 	 tx_data_len = 0;
	   INT   tag_len;
	   INT   data_len;
	   INT   supported;
	   BYTE  byteOneOfTag;
	   BYTE  byteTwoOfTag;
	   BYTE  emv_out_len[4] = "", TAGS_temp[50]={0};
	   BYTE temp_tag_data[100]={0};


	  if(0 == strcmp(auth_tx.TLF01_details.handler_queue,"dcpisoA"))
	  {
		 /* Step past 2-byte length field. */
		   idx_in = 2;
	  }
	  else
	  {
		 	 /* Step past 2-byte length field. */
		  		   idx_in = 3;

	  }

	   /* Step through input, searching for subfields supported . */
	   while( idx_in < emv_len_in )
	   {
	      /* Get the 1 or 2 byte subfield (tag) identifier. */
	      byteOneOfTag = emv_data_in[idx_in++];
	      if ( (byteOneOfTag & 0x1F) == 0x1F )
	      {
	         byteTwoOfTag = emv_data_in[idx_in++];
	         tag_len = 2;
	      }
	      else
	      {
	         byteTwoOfTag = 0x00;
	         tag_len = 1;
	      }

	      /* Get the length of this tag. */
	      data_len = emv_data_in[idx_in++];

	      /* Check to see if this tag is supported */
	      supported = true;
	      if ( byteOneOfTag == 0x5F )
	      {
	         if ( byteTwoOfTag != 0x2A )
	            supported = false;
			 else //19-20
			 	memcpy( &tag_data[TX_DATA_5F2A_POS],
							&emv_data_in[idx_in],
							 data_len );
	      }
	      else if (byteOneOfTag == 0x9F )
	      {
	         switch( byteTwoOfTag )
	         {
	            case 0x02:
					//0-5
					memcpy( &tag_data[TX_DATA_9F02_POS],
							&emv_data_in[idx_in],
							 data_len );

					break;
	           case 0x03:
	        	   //6-11
					memcpy( &tag_data[TX_DATA_9F03_POS],
											&emv_data_in[idx_in],
											 data_len );

									break;

	            case 0x10:
					//33-40

				memcpy( &tag_data[TX_DATA_9F10_POS],&emv_data_in[idx_in], data_len );
				tag_data[TX_DATA_9F10_POS+data_len] = 0x80;
				tx_data_len = TX_DATA_9F10_POS+data_len+1;
									break;
	            case 0x1A:
					//12-13
					memcpy( &tag_data[TX_DATA_9F1A_POS],
											&emv_data_in[idx_in],
											 data_len );

									break;
	            case 0x36:
					//31-32
					memcpy( &tag_data[TX_DATA_9F36_POS],
											&emv_data_in[idx_in],
											 data_len );

									break;
	            case 0x37:
					//25-28
					memcpy( &tag_data[TX_DATA_9F37_POS],
											&emv_data_in[idx_in],
											 data_len );

									break;

	            default:   supported = false;
	                       break;
	         }
	      }
	      else
	      {
	         switch( byteOneOfTag )
	         {
	            case 0x82: //29-30
					memcpy( &tag_data[TX_DATA_82_POS],
											&emv_data_in[idx_in],
											 data_len );

	            break;
	            case 0x95: //14-18
					memcpy( &tag_data[TX_DATA_95_POS],
											&emv_data_in[idx_in],
											 data_len );

	            break;
	            case 0x9A: //21-23
					memcpy( &tag_data[TX_DATA_9A_POS],
											&emv_data_in[idx_in],
											 data_len );

	            break;
	            case 0x9C: //24
					memcpy( &tag_data[TX_DATA_9C_POS],
											&emv_data_in[idx_in],
											 data_len );

				break;

	            default: supported = false;
	                     break;
	         }
	      }

	      /* Copy supported  tags into outgoing host buffer. */
	      if ( supported )
	      {
	         /* Subfield Tag */
	         temp_tag_data[idx_out] = byteOneOfTag;
	         if ( byteTwoOfTag != 0x00 )
	            temp_tag_data[idx_out+1] = byteTwoOfTag;

	         /* Subfield Length */
	         temp_tag_data[idx_out+tag_len] = data_len;


	         /* Increment the Outgoing Buffer index (tag+length+data) */
	         idx_out += tag_len + 1 + data_len;
	      }

	      idx_in += data_len;
	   }


	   return tx_data_len;

}// End of Diners CVN 05
/******************************************/
/*Tags '9F02', '9F03', '9F1A', '95', '5F2A', '9A', '9C', '9F37', '82'
,'9F36', '9F10' (CVR part)
*/
/**/
INT get_EMV_CVN_10_16_data( INT emv_len_in, pBYTE emv_data_in,pBYTE tag_data )
{
   INT   idx_in;
   INT   idx_out = 0,emv_org_len;
   INT   tag_len;
   INT   data_len;
   INT   supported;
   BYTE  byteOneOfTag;
   BYTE  byteTwoOfTag;
   BYTE  emv_out_len[4] = "", TAGS_temp[50]={0};
   BYTE temp_tag_data[100]={0};

  
  if(0 == strcmp(auth_tx.TLF01_details.handler_queue,"dcpisoA"))
  {
	 /* Step past 2-byte length field. */
	   idx_in = 2;
  }
  else		  
  {
	 	 /* Step past 2-byte length field. */
	   if (strcmp(auth_tx.BIN01_details.card_family,CARD_VISA) == 0)  
		   idx_in = 4;
	   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_MASTERCARD) == 0 ||
			   strcmp(auth_tx.BIN01_details.card_family,CARD_JCB) == 0)
		   idx_in = 2;
	   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_AMEX) == 0)  
		   idx_in = 2;
	   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_CUP) == 0 ||
			   strcmp(auth_tx.BIN01_details.card_family,CARD_DINERS) == 0)
		   idx_in = 3;
	   
  } 
  
   /* Step through input, searching for subfields supported . */
   while( idx_in < emv_len_in )
   {
      /* Get the 1 or 2 byte subfield (tag) identifier. */
      byteOneOfTag = emv_data_in[idx_in++];
      if ( (byteOneOfTag & 0x1F) == 0x1F )
      {
         byteTwoOfTag = emv_data_in[idx_in++];
         tag_len = 2;
      }
      else
      {
         byteTwoOfTag = 0x00;
         tag_len = 1;
      }

      /* Get the length of this tag. */
      data_len = emv_data_in[idx_in++];

      /* Check to see if this tag is supported */
      supported = true;
      if ( byteOneOfTag == 0x5F )
      {
         if ( byteTwoOfTag != 0x2A )
            supported = false;
		 else //19-21
		 	memcpy( &tag_data[TX_DATA_5F2A_POS],
						&emv_data_in[idx_in],
						 data_len );
      }
      else if (byteOneOfTag == 0x9F )
      {
         switch( byteTwoOfTag )
         {
            case 0x02:
				//1-6
				memcpy( &tag_data[TX_DATA_9F02_POS],
						&emv_data_in[idx_in],
						 data_len );
								
				break;
            case 0x03:
				//6-12
				memcpy( &tag_data[TX_DATA_9F03_POS],
										&emv_data_in[idx_in],
										 data_len );
												
								break;
				
            case 0x10:
				//6-12
				if (strcmp(auth_tx.BIN01_details.card_family,CARD_VISA) == 0)  
					{memcpy( &tag_data[TX_DATA_9F10_POS],
									&emv_data_in[idx_in+3],
									 4 );
					 //memcpy( &tag_data[TX_DATA_9F10_POS+4],
					//				"00",
					//				 2);
					
					}

			    else if (strcmp(auth_tx.BIN01_details.card_family,CARD_MASTERCARD) == 0)  
		   				memcpy( &tag_data[TX_DATA_9F10_POS],
										&emv_data_in[idx_in+2],
										 6 );
				else if (strcmp(auth_tx.BIN01_details.card_family,CARD_CUP) == 0)  {
					memcpy( &tag_data[TX_DATA_9F10_POS],
									&emv_data_in[idx_in+3],
									 4 );
					   tag_data[TX_DATA_9F10_POS + 4 ]=0x80;

					}
				else if (strcmp(auth_tx.BIN01_details.card_family,CARD_JCB) == 0)  {

					memcpy( &tag_data[TX_DATA_9F10_POS],
													&emv_data_in[idx_in+3],
													 5 );


									}
				else if (strcmp(auth_tx.BIN01_details.card_family,CARD_DINERS) == 0)  {

									memcpy( &tag_data[TX_DATA_9F10_POS],
																	&emv_data_in[idx_in],
																	 6 );


													}
				else{
					memcpy( &tag_data[TX_DATA_9F10_POS],
									&emv_data_in[idx_in+3],
									 4 );
					}
												
								break;
            case 0x1A:
				//12-14
				memcpy( &tag_data[TX_DATA_9F1A_POS],
										&emv_data_in[idx_in],
										 data_len );
												
								break;
            case 0x36:
				//6-12
				memcpy( &tag_data[TX_DATA_9F36_POS],
										&emv_data_in[idx_in],
										 data_len );
												
								break;
            case 0x37:
				//6-12
				memcpy( &tag_data[TX_DATA_9F37_POS],
										&emv_data_in[idx_in],
										 data_len );
												
								break;
        
            default:   supported = false;
                       break;
         }
      }
      else
      {
         switch( byteOneOfTag )
         {
            case 0x82: //29-30
				memcpy( &tag_data[TX_DATA_82_POS],
										&emv_data_in[idx_in],
										 data_len );
												
            break;
            case 0x95: //14-19
				memcpy( &tag_data[TX_DATA_95_POS],
										&emv_data_in[idx_in],
										 data_len );
												
            break;
            case 0x9A: //21-24
				memcpy( &tag_data[TX_DATA_9A_POS],
										&emv_data_in[idx_in],
										 data_len );
												
            break;
            case 0x9C: //24-25
				memcpy( &tag_data[TX_DATA_9C_POS],
										&emv_data_in[idx_in],
										 data_len );
												
			break;

            default: supported = false;
                     break;
         }
      }

      /* Copy supported  tags into outgoing host buffer. */
      if ( supported )
      {
         /* Subfield Tag */
         temp_tag_data[idx_out] = byteOneOfTag;
         if ( byteTwoOfTag != 0x00 )
            temp_tag_data[idx_out+1] = byteTwoOfTag;

         /* Subfield Length */
         temp_tag_data[idx_out+tag_len] = data_len;

         
         /* Increment the Outgoing Buffer index (tag+length+data) */
         idx_out += tag_len + 1 + data_len;
      }

      idx_in += data_len;
   }
  	  	  if (strcmp(auth_tx.BIN01_details.card_family,CARD_MASTERCARD) == 0)
			   tag_data[TX_DATA_PADDING]=0x80;
  	  	   else
			  tag_data[TX_DATA_PADDING]=0x00;

   return idx_out;
}
INT get_EMV_CVN_18_data( INT emv_len_in, pBYTE emv_data_in,pBYTE tag_data )
{
   INT   idx_in;
   INT   idx_out = 0,emv_org_len;
   INT   tag_len;
   INT   data_len;
   INT   supported;
   BYTE  byteOneOfTag;
   BYTE  byteTwoOfTag;
   BYTE  emv_out_len[4] = "", TAGS_temp[50]={0};
   BYTE temp_tag_data[100]={0};


  if(0 == strcmp(auth_tx.TLF01_details.handler_queue,"dcpisoA"))
  {
	 /* Step past 2-byte length field. */
	   idx_in = 2;
  }
  else
  {
	 	 /* Step past 2-byte length field. */
	   if (strcmp(auth_tx.BIN01_details.card_family,CARD_VISA) == 0)
		   idx_in = 4;
	   /*else if (strcmp(auth_tx.BIN01_details.card_family,CARD_MASTERCARD) == 0 ||
			   strcmp(auth_tx.BIN01_details.card_family,CARD_JCB) == 0)
		   idx_in = 2;
	   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_AMEX) == 0)
		   idx_in = 2;
	   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_CUP) == 0 ||
			   strcmp(auth_tx.BIN01_details.card_family,CARD_DINERS) == 0)
		   idx_in = 3;*/

  }

   /* Step through input, searching for subfields supported . */
   while( idx_in < emv_len_in )
   {
      /* Get the 1 or 2 byte subfield (tag) identifier. */
      byteOneOfTag = emv_data_in[idx_in++];
      if ( (byteOneOfTag & 0x1F) == 0x1F )
      {
         byteTwoOfTag = emv_data_in[idx_in++];
         tag_len = 2;
      }
      else
      {
         byteTwoOfTag = 0x00;
         tag_len = 1;
      }

      /* Get the length of this tag. */
      data_len = emv_data_in[idx_in++];

      /* Check to see if this tag is supported */
      supported = true;
      if ( byteOneOfTag == 0x5F )
      {
         if ( byteTwoOfTag != 0x2A )
            supported = false;
		 else //19-21
		 	memcpy( &tag_data[TX_DATA_5F2A_POS],
						&emv_data_in[idx_in],
						 data_len );
      }
      else if (byteOneOfTag == 0x9F )
      {
         switch( byteTwoOfTag )
         {
            case 0x02:
				//1-6
				memcpy( &tag_data[TX_DATA_9F02_POS],
						&emv_data_in[idx_in],
						 data_len );

				break;
            case 0x03:
				//6-12
				memcpy( &tag_data[TX_DATA_9F03_POS],
										&emv_data_in[idx_in],
										 data_len );

								break;

            case 0x10:
				//6-12
				if (strcmp(auth_tx.BIN01_details.card_family,CARD_VISA) == 0)
					{memcpy( &tag_data[TX_DATA_9F10_POS],
									&emv_data_in[idx_in],
									 7 );
					 //memcpy( &tag_data[TX_DATA_9F10_POS+4],
					//				"00",
					//				 2);

					}

			    else if (strcmp(auth_tx.BIN01_details.card_family,CARD_MASTERCARD) == 0)
		   				memcpy( &tag_data[TX_DATA_9F10_POS],
										&emv_data_in[idx_in+2],
										 6 );
				else if (strcmp(auth_tx.BIN01_details.card_family,CARD_CUP) == 0)  {
					memcpy( &tag_data[TX_DATA_9F10_POS],
									&emv_data_in[idx_in+3],
									 4 );
					   tag_data[TX_DATA_9F10_POS + 4 ]=0x80;

					}
				else if (strcmp(auth_tx.BIN01_details.card_family,CARD_JCB) == 0)  {

					memcpy( &tag_data[TX_DATA_9F10_POS],
													&emv_data_in[idx_in+3],
													 5 );


									}
				else if (strcmp(auth_tx.BIN01_details.card_family,CARD_DINERS) == 0)  {

									memcpy( &tag_data[TX_DATA_9F10_POS],
																	&emv_data_in[idx_in],
																	 6 );


													}
				else{
					memcpy( &tag_data[TX_DATA_9F10_POS],
									&emv_data_in[idx_in+3],
									 4 );
					}

								break;
            case 0x1A:
				//12-14
				memcpy( &tag_data[TX_DATA_9F1A_POS],
										&emv_data_in[idx_in],
										 data_len );

								break;
            case 0x36:
				//6-12
				memcpy( &tag_data[TX_DATA_9F36_POS],
										&emv_data_in[idx_in],
										 data_len );

								break;
            case 0x37:
				//6-12
				memcpy( &tag_data[TX_DATA_9F37_POS],
										&emv_data_in[idx_in],
										 data_len );

								break;

            default:   supported = false;
                       break;
         }
      }
      else
      {
         switch( byteOneOfTag )
         {
            case 0x82: //29-30
				memcpy( &tag_data[TX_DATA_82_POS],
										&emv_data_in[idx_in],
										 data_len );

            break;
            case 0x95: //14-19
				memcpy( &tag_data[TX_DATA_95_POS],
										&emv_data_in[idx_in],
										 data_len );

            break;
            case 0x9A: //21-24
				memcpy( &tag_data[TX_DATA_9A_POS],
										&emv_data_in[idx_in],
										 data_len );

            break;
            case 0x9C: //24-25
				memcpy( &tag_data[TX_DATA_9C_POS],
										&emv_data_in[idx_in],
										 data_len );

			break;

            default: supported = false;
                     break;
         }
      }

      /* Copy supported  tags into outgoing host buffer. */
      if ( supported )
      {
         /* Subfield Tag */
         temp_tag_data[idx_out] = byteOneOfTag;
         if ( byteTwoOfTag != 0x00 )
            temp_tag_data[idx_out+1] = byteTwoOfTag;

         /* Subfield Length */
         temp_tag_data[idx_out+tag_len] = data_len;


         /* Increment the Outgoing Buffer index (tag+length+data) */
         idx_out += tag_len + 1 + data_len;
      }

      idx_in += data_len;
   }

	 tag_data[TX_DATA_PADDING_VISA_CVN_18]=0x80;

   return idx_out;
}
//Diners CVN 15
/*
|--------------------------+-----------------|
|Contactless (CVN 15)      |                 |
|--------------------------+-----------------|
|Field                     |       Tag       |
|--------------------------+-----------------|
|Amount, Authorized        |      9F02       |
|--------------------------+-----------------|
|Transaction Currency Code |      5F2A       |
|--------------------------+-----------------|
|Unpredictable Number      |      9F37       |
|--------------------------+-----------------|
|Application Transaction   |      9F36       | Counter
||                 |
|--------------------------+-----------------|
|Issuer Application Data   |      9F10       |
|(Full)                    |                 |
|--------------------------+-----------------|

 */

INT get_EMV_CVN_15_data_Diners( INT emv_len_in, pBYTE emv_data_in,pBYTE tag_data )
{
	   INT   idx_in;
	   INT   idx_out = 0,emv_org_len=0;
	   INT 	 tx_data_len =0;
	   INT   tag_len;
	   INT   data_len;
	   INT   supported;
	   BYTE  byteOneOfTag;
	   BYTE  byteTwoOfTag;
	   BYTE  emv_out_len[4] = "", TAGS_temp[50]={0};
	   BYTE temp_tag_data[100]={0};


	  if(0 == strcmp(auth_tx.TLF01_details.handler_queue,"dcpisoA"))
	  {
		 /* Step past 2-byte length field. */
		   idx_in = 2;
	  }
	  else
	  {		 	 /* Step past 2-byte length field. */
		  			   idx_in = 3;
	  }

	   /* Step through input, searching for subfields supported . */
	   while( idx_in < emv_len_in )
	   {
	      /* Get the 1 or 2 byte subfield (tag) identifier. */
	      byteOneOfTag = emv_data_in[idx_in++];
	      if ( (byteOneOfTag & 0x1F) == 0x1F )
	      {
	         byteTwoOfTag = emv_data_in[idx_in++];
	         tag_len = 2;
	      }
	      else
	      {
	         byteTwoOfTag = 0x00;
	         tag_len = 1;
	      }

	      /* Get the length of this tag. */
	      data_len = emv_data_in[idx_in++];

	      /* Check to see if this tag is supported */
	      supported = true;
	      if ( byteOneOfTag == 0x5F )
	      {
	         if ( byteTwoOfTag != 0x2A )
	            supported = false;
			 else //6-7
			 	memcpy( &tag_data[6],
							&emv_data_in[idx_in],
							 data_len );
	      }
	      else if (byteOneOfTag == 0x9F )
	      {
	         switch( byteTwoOfTag )
	         {
	            case 0x02:
					//0-5
					memcpy( &tag_data[0],
							&emv_data_in[idx_in],
							 data_len );

					break;

	            case 0x10:
					//14-23

						memcpy( &tag_data[14],&emv_data_in[idx_in], data_len );
						tag_data[14+data_len]=0x80;
						tx_data_len = 14+data_len+1;
									break;

	            case 0x36:
					//12-13
					memcpy( &tag_data[12],
											&emv_data_in[idx_in],
											 data_len );

									break;
	            case 0x37:
					//8-11
					memcpy( &tag_data[8],
											&emv_data_in[idx_in],
											 data_len );

									break;

	            default:   supported = false;
	                       break;
	         }
	      }
	      else
	      {
	         switch( byteOneOfTag )
	         {

	            default: supported = false;
	                     break;
	         }
	      }

	      /* Copy supported  tags into outgoing host buffer. */
	      if ( supported )
	      {
	         /* Subfield Tag */
	         temp_tag_data[idx_out] = byteOneOfTag;
	         if ( byteTwoOfTag != 0x00 )
	            temp_tag_data[idx_out+1] = byteTwoOfTag;

	         /* Subfield Length */
	         temp_tag_data[idx_out+tag_len] = data_len;


	         /* Increment the Outgoing Buffer index (tag+length+data) */
	         idx_out += tag_len + 1 + data_len;
	      }

	      idx_in += data_len;
	   }


	   return tx_data_len;
}


/*******************************************/
/******************************************/
/*Visa CVN 17

Tags '9F02', '9F37','9F36', '9F10' ( IAD Byte 5) .
*/
/**/
INT get_EMV_CVN_17_data( INT emv_len_in, pBYTE emv_data_in,pBYTE tag_data )
{
   INT   idx_in;
   INT   idx_out = 0,emv_org_len=0;
   INT   tag_len;
   INT   data_len;
   INT   supported;
   BYTE  byteOneOfTag;
   BYTE  byteTwoOfTag;
   BYTE  emv_out_len[4] = "";
   BYTE temp_tag_data[100]={0};
   	

   if(0 == strcmp(auth_tx.TLF01_details.handler_queue,"dcpisoA"))
   {
	   emv_org_len = genutil_bcd_to_int( auth_tx.EMV_details.emv_block, 2 );
	     /* Step past 2-byte length field. */
   		idx_in = 2;
   }
   else 	   
   {
		emv_org_len = auth_tx.EMV_details.emv_block[0];
		  /* Step past 2-byte length field. */
		if (strcmp(auth_tx.BIN01_details.card_family,CARD_VISA) == 0)  
   	  		idx_in = 4;
		else if (strcmp(auth_tx.BIN01_details.card_family,CARD_MASTERCARD) == 0)  
   	  		idx_in = 2;
		else if (strcmp(auth_tx.BIN01_details.card_family,CARD_AMEX) == 0)  
		   idx_in = 2;
   } 

 

   /* Step through input, searching for subfields supported . */
   while( idx_in < emv_len_in )
   {
      /* Get the 1 or 2 byte subfield (tag) identifier. */
      byteOneOfTag = emv_data_in[idx_in++];
      if ( (byteOneOfTag & 0x1F) == 0x1F )
      {
         byteTwoOfTag = emv_data_in[idx_in++];
         tag_len = 2;
      }
      else
      {
         byteTwoOfTag = 0x00;
         tag_len = 1;
      }

      /* Get the length of this tag. */
      data_len = emv_data_in[idx_in++];

      /* Check to see if this tag is supported */
      supported = true;
      if ( byteOneOfTag == 0x5F )
      {
         if ( byteTwoOfTag != 0x2A )
            supported = false;
      }
      else if (byteOneOfTag == 0x9F )
      {
         switch( byteTwoOfTag )
         {
           case 0x02:
				//1-6
				memcpy( &tag_data[0],
						&emv_data_in[idx_in],
						 data_len );
								
				break;
           
            case 0x10:
				//12-13
				
				 if (strcmp(auth_tx.BIN01_details.card_family,CARD_AMEX) == 0)	
				 memcpy( &tag_data[12],
										&emv_data_in[idx_in+3],
										 4 );
				 else
				 memcpy( &tag_data[12],
										&emv_data_in[idx_in+4],
										 1 );
												
								break;
            case 0x36:
				//10-12
				memcpy( &tag_data[10],
										&emv_data_in[idx_in],
										 data_len );
												
								break;
            case 0x37:
				//6-10
				memcpy( &tag_data[6],
										&emv_data_in[idx_in],
										 data_len );
												
								break;

            default:   supported = false;
                       break;
         }
      }
      else
      {
         switch( byteOneOfTag )
         {
           
            default: supported = false;
                     break;
         }
      }

      /* Copy supported  tags into outgoing host buffer. */
      if ( supported )
      {
         /* Subfield Tag */
         temp_tag_data[idx_out] = byteOneOfTag;
         if ( byteTwoOfTag != 0x00 )
            temp_tag_data[idx_out+1] = byteTwoOfTag;

         /* Subfield Length */
         temp_tag_data[idx_out+tag_len] = data_len;

         
         /* Increment the Outgoing Buffer index (tag+length+data) */
         idx_out += tag_len + 1 + data_len;
      }

      idx_in += data_len;
   }
							

   return idx_out;
}

INT get_EMV_Data_AMEX(INT emv_len_in, pBYTE emv_data_in,pBYTE tag_data )

{
	
	   INT   idx_in;
	   INT	 idx_out = 0,emv_org_len;
	   INT	 tag_len;
	   INT	 data_len;
	   INT	 supported;
	   BYTE  byteOneOfTag;
	   BYTE  byteTwoOfTag;
	   BYTE  emv_out_len[4] = "", TAGS_temp[50]={0};
	   BYTE temp_tag_data[100]={0};INT contact_ind=1;
		int iad_len,bin_len=0;
		BYTE temp_text[100]="";
		/*if(strncmp(auth_tx.TLF01_details.pos_entry_mode,"007",3) == 0 || 
						   strncmp(auth_tx.TLF01_details.pos_entry_mode,"07",2) == 0 
		  ){
		  	contact_ind =0;
			}*/

	/*Maximum Length Fixed/variable		Source		Feild Name
			4				F			A			Header Version Name "AGNS"
			2				F			A			Header Version Number  "0001"
			8				F			C			Application Cryptogram
			33				V			C			Issuer Application Data (IAD)
			4				F			T			Unpredictable Number
			2				F			C			Application Transaction Counter (ATC)
			5				F			T			Terminal Verification Results (TVR)
			3				F			T			Transaction Date
			1				F			T			Transaction Type
			6				F			T			Amount, Authorized
			2				F			T			Transaction Currency Code
			2				F			T			Terminal Country Code
			2				F			C			Application Interchange Profile (AIP)
			6				F			T			Amount, Other
			1				F			C			Application PAN Sequence Number
			1				F			C			Cryptogram Information Data
		*/
	  
	if(contact_ind == 0) // Contact less
	{//Tags '9F02', '9F37','9F36', '9F10' ( IAD Byte 5) .

		memcpy( &temp_tag_data[TX_DATA_9F02_POS],
						auth_tx.EMV_details.tran_amount,
						 12 );
		
		memcpy( &temp_tag_data[12],
						auth_tx.EMV_details.unpredictable_number,
						 8 );

		memcpy( &temp_tag_data[20],
						auth_tx.EMV_details.atc,
						 4 );
		 iad_len = strlen(auth_tx.EMV_details.iad);
		memcpy( &temp_tag_data[24],
						&auth_tx.EMV_details.iad[2],
						 12);

		idx_out = 24+iad_len;

		genutil_asc_to_bin( temp_tag_data,temp_text,&bin_len);
		bin_len = 16;
		memcpy(tag_data,temp_text,bin_len);
		return bin_len;
		
	}else // Conatct
	{

	/*Tags '9F02', '9F03', '9F1A', '95', '5F2A', '9A', '9C', '9F37', '82'
	,'9F36', '9F10' (CVR part)
	*/		idx_out = TX_DATA_9F02_POS;
			memcpy( &temp_tag_data[TX_DATA_9F02_POS],
							auth_tx.EMV_details.tran_amount,
							 12 );
			//idx_out = idx_out + 12;
			memcpy( &temp_tag_data[12],
							auth_tx.EMV_details.other_amount,
							 12 );
		
		 	memcpy( &temp_tag_data[24],
								auth_tx.EMV_details.country_code,
								 4 );
			memcpy( &temp_tag_data[28],
											auth_tx.EMV_details.tvr,
											 10);
						
			memcpy( &temp_tag_data[38],
											auth_tx.EMV_details.currency_code,
											 4);
			
			memcpy( &temp_tag_data[42],
											auth_tx.EMV_details.tran_date,
											 6);
			
			memcpy( &temp_tag_data[48],
											auth_tx.EMV_details.tran_type,
											 2);

			memcpy( &temp_tag_data[50],
							auth_tx.EMV_details.unpredictable_number,
							 8 );
		
			memcpy( &temp_tag_data[58],
							auth_tx.EMV_details.interchange_profile,
							 4 );
		
			memcpy( &temp_tag_data[62],
							auth_tx.EMV_details.atc,
							 4 );
			 iad_len = strlen(auth_tx.EMV_details.iad);
			 memcpy( &temp_tag_data[66],
							&auth_tx.EMV_details.iad[6],
							  8);
			memcpy( &temp_tag_data[74],
							"000000",
							  6);
			  
			
			idx_out = 66+iad_len;
		
			genutil_asc_to_bin( temp_tag_data,temp_text,&bin_len);
			bin_len = 40;
			memcpy(tag_data,temp_text,bin_len);
			return bin_len;
		

	
	}

		 
}
int racalVerifyARQCfor_CUPEMV()
{
	char msgHeader[5];
	char delimiter[2];
	char output[500];
	INT bin_len=0, out_len=0;
	int	cnt=0,result=0,jcnt=0;
	BYTE MK_AC[33]="";
	BYTE TX_DATA[256]="",temp_txt[100]="";
	BYTE ARPC_RAW[20]="";
	INT resp_len=0,i;
	BYTE temp_msg_len[10]="";

	memset (&msgHeader,0,sizeof(msgHeader));
	memset (&delimiter,0,sizeof(delimiter));
	memset (&output,0,sizeof(output));
	memset (&returnString,0,ReturnString_Size);
	strcpy (delimiter,";");
	strcpy (msgHeader ,"1234");

	strcat(output,msgHeader);
	strcat(output,"JS"); /* Changed Command code JS- TF Phani*/
	/*Mode flag
	Mode of operation:
	'0' : Perform ARQC Verification only
	'1' : Perform ARQC Verification and ARPC Generation
	'2' : Perform ARPC Generation only.
	*/
	strcat(output,"1");
	/*Scheme ID*/
	strcat(output,"1");


	/* Issuer MAster key - MK-AC */
	strcpy(MK_AC, auth_tx.CAF01_details.cvk[5].cvkA);
	strcat(MK_AC, auth_tx.CAF01_details.cvk[5].cvkB);

	if(strlen(MK_AC) == 0)
	{
		racal_log_message( 2, 2,"Master key is blank.","racalVerifyARQCfor_CUPEMV",1 );
		return(TRANSACTION_NOT_VERIFIED);
	}else
	{	strcat(output,"U");
		strcat(output,MK_AC);
	}
	out_len = strlen(output);
	/* PAN SEQ NUMBER*/
	if(strlen(auth_tx.TLF01_details.card_num) >= 14 )
	{
	memset(&dummy,0,Dummy_Size);

	strncpy(dummy,&auth_tx.TLF01_details.card_num[2] ,14);
	/* need to validate PAN SEQ no  */
		if(strlen(auth_tx.host2_message_seq_nbr) > 0)
		{
				if(strlen(auth_tx.host2_message_seq_nbr) == 4)
				strncat(dummy,&auth_tx.host2_message_seq_nbr[2],2);
				else if(strlen(auth_tx.host2_message_seq_nbr) == 3)
				strncat(dummy,&auth_tx.host2_message_seq_nbr[1],2);
		}
		else if(strlen(auth_tx.EMV_details.pan_sequence_number) > 0)
		{
			if(strlen(auth_tx.EMV_details.pan_sequence_number) == 2)
			strncat(dummy,&auth_tx.EMV_details.pan_sequence_number[0],2);
			else if(strlen(auth_tx.EMV_details.pan_sequence_number) == 3)
			strncat(dummy,&auth_tx.EMV_details.pan_sequence_number[1],2);
		}
		else
			strcat(dummy,"00");
	dummy[16] = '\0';
	genutil_asc_to_bin( dummy,temp_txt,&bin_len);
	memcpy(&output[out_len],temp_txt,8);
	out_len += 8;

	}
	else
	{
		racal_log_message( 2, 2,"ARQC validation: card_num is < 14.","racalVerifyARQCfor_CUPEMV",1 );
		return (TRANSACTION_NOT_VERIFIED);
	}


	// ONLY For MC , for VISA and AMEX  0000

	if(strlen(auth_tx.EMV_details.atc) == 0)
	{
		racal_log_message( 2, 2,"ATC is blank.","racalVerifyARQCfor_CUPEMV",1 );
		return(TRANSACTION_NOT_VERIFIED);

	}else
	{
	genutil_asc_to_bin( auth_tx.EMV_details.atc,temp_txt,&bin_len);
	printf("ATC - %s \n",auth_tx.EMV_details.atc);
	memcpy(&output[out_len],temp_txt,2);
	out_len += 2;
	}


	/* Padding Flag : 0 - no padding,  1- padding*/
	memcpy(&output[out_len],"1",1);
	out_len ++;
	/* Tx data Len*/
	/* Tx Data gather for CVN
	#1
	Visa CVN 10

	Tags '9F02', '9F03', '9F1A', '95', '5F2A', '9A', '9C', '9F37', '82' ,'9F36', '9F34',[�9F10� (CVR part)]

	Visa CVN 17

	Tags �9F02�, �9F37�,�9F36�, '9F34',[�9F10� (CVR part)]

	MC CVN 16

	Tags '9F02', '9F03', '9F1A', '95', '5F2A', '9A', '9C', '9F37', '82' ,'9F36', �9F10� (CVR part)
	*/
	int emv_org_len =0;
	BYTE tag_data[255]= {0};BYTE temp_ascii_len[10]={0};
	if(0 == strcmp(auth_tx.TLF01_details.handler_queue,"dcpisoA"))
	{
	emv_org_len = genutil_bcd_to_int( auth_tx.EMV_details.emv_block, 2 );
	}
	else
	{
	strncpy(temp_ascii_len, auth_tx.EMV_details.emv_block,3);
	emv_org_len = atoi(temp_ascii_len);

	}
	printf("EMV LEN - %d \n",emv_org_len);
	memset(tag_data,0x00,sizeof(tag_data));

	printf("calling get_EMV_CVN_16_data - \n");
	bin_len=get_EMV_CVN_10_16_data(emv_org_len,&auth_tx.EMV_details.emv_block,&tag_data);
	bin_len = 40;

	int pad_len = bin_len%8;
	if (pad_len > 0)
	bin_len += (8-pad_len); // Pad with Zero's

	printf("EMV TX DATA LEN - %d \n",bin_len);
	printf("EMV TX DATA  - %s \n",tag_data);
	sprintf(&output[out_len],"%2x",bin_len);
	out_len +=2;
	memcpy(&output[out_len],tag_data,bin_len);
	out_len += bin_len;
	/* Tx data*/ // TAGs

	/* Deliemter*/

	memcpy(&output[out_len],delimiter,1);
	out_len += 1;
	/* ARQC/ARPC*/

	genutil_asc_to_bin( auth_tx.EMV_details.app_crypto,temp_txt,&bin_len);

	printf("APP CRYPTO - %s \n",auth_tx.EMV_details.app_crypto);
	memcpy(&output[out_len],temp_txt,8);
	out_len += 8;
	/* ARC*/
	genutil_asc_to_bin( "3030",temp_txt,&bin_len);
	memcpy(&output[out_len],temp_txt,2);
	out_len += 2;

	/* End of msg*/
	//if the output is bigger then 500 do not send it out.
	if (out_len > 500)
	{
		racal_log_message( 2, 2,"ARQC Cmd Msg to long.","racalVerifyARQCfor_CUPEMV",1 );
		return(TRANSACTION_NOT_VERIFIED);
	}
	//send the message to the racal box.
	if (racalSendToPort_Bin(output,out_len) == TRANSACTION_VERIFIED)
	{
	/*parse the message check for the message header.*/
	memset(&dummy,0,Dummy_Size);
	strncpy(dummy,returnString,4);
	if(strcmp(dummy,msgHeader) != 0)
	{
		racal_log_message( 2, 2,"ARQC Cmd header mis-match.","racalVerifyARQCfor_CUPEMV",1 );
		return(TRANSACTION_NOT_VERIFIED);
	}
	/*parse the message check for the response code.*/
	memset(&dummy,0,Dummy_Size);
	strncpy(dummy,returnString + 4, 2);

	if(strcmp(dummy,"JT") !=0)
	{
		racal_log_message( 2, 2,"ARQC Cmd Code mis-match.","racalVerifyARQCfor_CUPEMV",1 );
		return(TRANSACTION_NOT_VERIFIED);
	}

	/*parse the message check for the error code.*/
	memset(&dummy,0,Dummy_Size);
	strncpy(dummy,returnString + 6, 2);

	//if this value is not 00 reject the transaction.
	/* 00 : No error
	01 : ARQC/TC/AAC verification failed
	03 : Invalid Padding Flag
	04 : Mode Flag not 0, 1 or 2
	05 : Unrecognized Scheme ID
	10 : MK parity error
	67 : Command not licensed
	80 : Data length error
	81 : Zero length Transaction Data
	82 : Transaction Data length not multiple of 8 bytes
	*/
	if(strcmp(dummy,"00") != 0)
	{
	if(strcmp(dummy,"01") == 0)
	{
		racal_log_message( 2, 2,"ARPC AAC validation failed.","racalVerifyARQCfor_CUPEMV",1 );
	}
	if(strcmp(dummy,"04") == 0)
		racal_log_message( 2, 2,"Mode flag not 0 1 or 2.","racalVerifyARQCfor_CUPEMV",1 );
	if(strcmp(dummy,"05") == 0)
		racal_log_message( 2, 2,"Unrecognised Scheme ID.","racalVerifyARQCfor_CUPEMV",1 );
	if(strcmp(dummy,"10") == 0)
		racal_log_message( 2, 2,"MK parity error.","racalVerifyARQCfor_CUPEMV",1 );
	if(strcmp(dummy,"67") == 0)
		racal_log_message( 2, 2,"Command Not licensed.","racalVerifyARQCfor_CUPEMV",1 );
	if(strcmp(dummy,"80") == 0)
		racal_log_message( 2, 2,"Data len error.","racalVerifyARQCfor_CUPEMV",1 );
	if(strcmp(dummy,"81") == 0)
		racal_log_message( 2, 2,"Zero length Transaction Data.","racalVerifyARQCfor_CUPEMV",1 );
	if(strcmp(dummy,"82") == 0)
		racal_log_message( 2, 2,"Transaction Data length not multiple of 8 bytes.","racalVerifyARQCfor_CUPEMV",1 );
	return(TRANSACTION_NOT_VERIFIED);
	}

	CHAR temp_response_tag[35]={0};
	memcpy(dummy,returnString + 8, 8);		 // This is the ARPC value.
	if (strcmp(auth_tx.BIN01_details.card_family,CARD_MASTERCARD) == 0)
	{
	dummy[8]=0x00;
	dummy[9]=0x10;
	}
	else  if (strcmp(auth_tx.BIN01_details.card_family,CARD_VISA) == 0 ||
	strcmp(auth_tx.BIN01_details.card_family,CARD_AMEX) == 0 ||
	strcmp(auth_tx.BIN01_details.card_family,CARD_CUP) == 0)
	{
	dummy[8]=0x30;
	dummy[9]=0x30;
	}
	// Prepare EMV block for response
	strcpy(ARPC_RAW,"91");
	strcat(ARPC_RAW,"0A");

	if(0 == strcmp(auth_tx.TLF01_details.handler_queue,"dcpisoA") )
	{
	auth_tx.EMV_details.future_field1[0]=0x00;
	//int resp_dcp_len =12;
	auth_tx.EMV_details.future_field1[1]=0x12;
	strcpy(temp_response_tag,ARPC_RAW);
	genutil_asc_to_bin( temp_response_tag,temp_txt,&bin_len);
	memcpy(&auth_tx.EMV_details.future_field1[2],temp_txt,2);
	memcpy(&auth_tx.EMV_details.future_field1[4],dummy,10);
	}
	else
	{
	if (strcmp(auth_tx.BIN01_details.card_family,CARD_MASTERCARD) == 0 ||
	 strcmp(auth_tx.BIN01_details.card_family,CARD_CUP) == 0 )
	{	sprintf(auth_tx.EMV_details.future_field1,"%03d",12);
	strcpy(temp_response_tag,ARPC_RAW);
	genutil_asc_to_bin( temp_response_tag,temp_txt,&bin_len);
	memcpy(&auth_tx.EMV_details.future_field1[3],temp_txt,2);
	memcpy(&auth_tx.EMV_details.future_field1[5],dummy,10);
	}
	else  if (strcmp(auth_tx.BIN01_details.card_family,CARD_VISA) == 0)
	{
	auth_tx.EMV_details.future_field1[0]=0x0F;
	strcpy(temp_response_tag,"01000C");
	strcat(temp_response_tag,ARPC_RAW);
	genutil_asc_to_bin( temp_response_tag,temp_txt,&bin_len);
	memcpy(&auth_tx.EMV_details.future_field1[1],temp_txt,5);
	memcpy(&auth_tx.EMV_details.future_field1[6],dummy,10);
	}else if (strcmp(auth_tx.BIN01_details.card_family,CARD_AMEX) == 0)
	{
	auth_tx.EMV_details.future_field1[0]=0x0A;
	memcpy(&auth_tx.EMV_details.future_field1[1],dummy,10);

	}

	}

	}

	return (TRANSACTION_VERIFIED);
}
// TF phani
int racalVerifyARQC_for_JCB()
{
	   char msgHeader[5];
	   char delimiter[2];
	   char output[500];
	   INT bin_len=0, out_len=0;
	   int	cnt=0,result=0,jcnt=0;
	  // CHAR mode_falg={0};
	   BYTE MK_AC[33]="";
	   BYTE TX_DATA[256]="",temp_txt[100]="";
	   BYTE ARPC_RAW[20]="";
	   INT resp_len=0,i;
	   BYTE temp_msg_len[10]="";

	   memset (&msgHeader,0,sizeof(msgHeader));
	   memset (&delimiter,0,sizeof(delimiter));
	   memset (&output,0,sizeof(output));
	   memset (&returnString,0,ReturnString_Size);
	   strcpy (delimiter,";");
	   strcpy (msgHeader ,"1234");

			 strcat(output,msgHeader);
			 strcat(output,"US"); /* Changed Command code KQ- TF Phani*/
			/*Mode flag
				Mode of operation:
			 '0' : Perform ARQC Verification only
			 '1' : Perform ARQC Verification and ARPC Generation
			 '2' : Perform ARPC Generation only.
			*/
			strcat(output,"1");

			/* CVN of the JCB:
			 */
			strcat(output,"01");

			 /* Issuer MAster key - MK-AC */
			 strcpy(MK_AC, auth_tx.CAF01_details.cvk[5].cvkA);
			 strcat(MK_AC, auth_tx.CAF01_details.cvk[5].cvkB);

			 if(strlen(MK_AC) == 0)
			{
				racal_log_message( 2, 2,"Master key is blank.","racalVerifyARQC_for_JCB",1 );
				return(TRANSACTION_NOT_VERIFIED);

			}else
			{	strcat(output,"U");
				strcat(output,MK_AC);
			}
			 out_len = strlen(output);
			 /* PAN SEQ NUMBER*/
			 if(strlen(auth_tx.TLF01_details.card_num) >= 14 )
			 {
				  memset(&dummy,0,Dummy_Size);

				  strncpy(dummy,&auth_tx.TLF01_details.card_num[2] ,14);
				  /* need to validate PAN SEQ no  */
				  if(strlen(auth_tx.host2_message_seq_nbr) > 0)
				  {
					  if(strlen(auth_tx.host2_message_seq_nbr) == 4)
						strncat(dummy,&auth_tx.host2_message_seq_nbr[2],2);
					  else if(strlen(auth_tx.host2_message_seq_nbr) == 3)
						strncat(dummy,&auth_tx.host2_message_seq_nbr[1],2);
				  }
				  else if(strlen(auth_tx.EMV_details.pan_sequence_number) > 0)
				  {
					if(strlen(auth_tx.EMV_details.pan_sequence_number) == 2)
						strncat(dummy,&auth_tx.EMV_details.pan_sequence_number[0],2);
					else if(strlen(auth_tx.EMV_details.pan_sequence_number) == 3)
						strncat(dummy,&auth_tx.EMV_details.pan_sequence_number[1],2);
				  }
				  else
							strcat(dummy,"00");
				  dummy[16] = '\0';
				  genutil_asc_to_bin( dummy,temp_txt,&bin_len);
				  //strncat(output,temp_txt,bin_len);
				  memcpy(&output[out_len],temp_txt,8);
				  out_len += 8;
				  //strcat(output,dummy);
			  }
			  else
			  {
				  racal_log_message( 2, 2,"ARQC validation: card_num is < 14.","racalVerifyARQC_for_JCB",1 );
				  return (TRANSACTION_NOT_VERIFIED);
			   }
			 /* ATC*/	//	 auth_tx.EMV_details.atc
			 // ONLY For MC , for VISA and AMEX  0000
		  	/*				genutil_asc_to_bin("000000000000",temp_txt,&bin_len);
					memcpy(&output[out_len],temp_txt,6);
						  out_len += 6;
			*/
			 /* Tx data Len*/
			 /* Tx Data gather for CVN
		#1
				Visa CVN 10

				Tags '9F02', '9F03', '9F1A', '95', '5F2A', '9A', '9C', '9F37', '82' ,'9F36', �9F10� (CVR part)

				Visa CVN 17

				Tags �9F02�, �9F37�,�9F36�, �9F10� ( IAD Byte 5) .

				MC CVN 16

				Tags '9F02', '9F03', '9F1A', '95', '5F2A', '9A', '9C', '9F37', '82' ,'9F36', �9F10� (CVR part)
			*/
				int emv_org_len =0;
				BYTE tag_data[255]= {0};

					emv_org_len = genutil_bcd_to_int( auth_tx.EMV_details.emv_block, 2 );

				printf("EMV LEN - %d \n",emv_org_len);
				memset(tag_data,0x00,sizeof(tag_data));

				printf("calling get_EMV_CVN_16_data - \n");
				bin_len=get_EMV_CVN_10_16_data(emv_org_len,&auth_tx.EMV_details.emv_block,&tag_data);
				bin_len = 40;


				printf("EMV TX DATA LEN - %d \n",bin_len);
				printf("EMV TX DATA  - %s \n",tag_data);
				sprintf(&output[out_len],"%2x",bin_len);
					out_len +=2;
				memcpy(&output[out_len],tag_data,bin_len);
				out_len += bin_len;
			 /* Tx data*/ // TAGs

			 /* Deliemter*/

				memcpy(&output[out_len],delimiter,1);
				 out_len += 1;
			 /* ARQC/ARPC*/

			 genutil_asc_to_bin( auth_tx.EMV_details.app_crypto,temp_txt,&bin_len);
			 //strncat(output, temp_txt,bin_len);
			 //printf("APP CRYPTO - %s \n",auth_tx.EMV_details.app_crypto);
			 memcpy(&output[out_len],temp_txt,8);
				 out_len += 8;
			 /* ARC*/
					 genutil_asc_to_bin( "3030",temp_txt,&bin_len);
					 memcpy(&output[out_len],temp_txt,2);
					 out_len += 2;

			 /* End of msg*/
			 //if the output is bigger then 500 do not send it out.
			 if (out_len > 500)
			 {
				racal_log_message( 2, 2,"ARQC Cmd Msg to long.","racalVerifyARQC_for_JCB",1 );
				return(TRANSACTION_NOT_VERIFIED);
			 }
			 //send the message to the racal box.
			 if (racalSendToPort_Bin(output,out_len) == TRANSACTION_VERIFIED)
			 {
				/*parse the message check for the message header.*/
				memset(&dummy,0,Dummy_Size);
				strncpy(dummy,returnString,4);
				if(strcmp(dummy,msgHeader) != 0)
				{
				   racal_log_message( 2, 2,"ARQC Cmd header mis-match.","racalVerifyARQC_for_JCB",1 );
				   return(TRANSACTION_NOT_VERIFIED);
				}
				/*parse the message check for the response code.*/
				memset(&dummy,0,Dummy_Size);
				strncpy(dummy,returnString + 4, 2);

				if(strcmp(dummy,"UT") !=0)
				{
				   racal_log_message( 2, 2,"ARQC Cmd Code mis-match.","racalVerifyARQC_for_JCB",1 );
				   return(TRANSACTION_NOT_VERIFIED);
				}

				/*parse the message check for the error code.*/
				memset(&dummy,0,Dummy_Size);
				strncpy(dummy,returnString + 6, 2);

				  //if this value is not 00 reject the transaction.
				/* '00' : No error
				'01' : ARQC/TC/AAC verification failed
				'04' : Mode Flag not 0, 1 or 2
				'05' : Unrecognised Scheme ID
				'10' : MK parity error
				53   : invalid CVN
				80   : Data length error
				81   : zero len tx data
				'68' : Command disabled
				*/
				if(strcmp(dummy,"00") != 0)
				{
				   if(strcmp(dummy,"01") == 0)
					{
					   racal_log_message( 2, 2,"ARPC AAC validation failed.","racalVerifyARQC_for_JCB",1 );
				   }
				   if(strcmp(dummy,"04") == 0)
					   racal_log_message( 2, 2,"Mode flag not 0 1 or 2.","racalVerifyARQC_for_JCB",1 );
				   if(strcmp(dummy,"05") == 0)
					   racal_log_message( 2, 2,"Unrecognised Scheme ID.","racalVerifyARQC_for_JCB",1 );
				   if(strcmp(dummy,"10") == 0)
					   racal_log_message( 2, 2,"MK parity error.","racalVerifyARQC_for_JCB",1 );
				   if(strcmp(dummy,"53") == 0)
					   racal_log_message( 2, 2,"Invalid CVN.","racalVerifyARQC_for_JCB",1 );
				   if(strcmp(dummy,"80") == 0)
					   racal_log_message( 2, 2,"Data Length error.","racalVerifyARQC_for_JCB",1 );
				   if(strcmp(dummy,"81") == 0)
					   racal_log_message( 2, 2,"Zero len tx data.","racalVerifyARQC_for_JCB",1 );
				   if(strcmp(dummy,"68") == 0)
					   racal_log_message( 2, 2,"Command disabled.","racalVerifyARQC_for_JCB",1 );
					 return(TRANSACTION_NOT_VERIFIED);
				}

				CHAR temp_response_tag[35]={0};
				memcpy(dummy,returnString + 8, 8);		 // This is the ARPC value.
					 dummy[8]=0x30;
					 dummy[9]=0x30;

				 // Prepare EMV block for response
				 strcpy(ARPC_RAW,"91");
				 strcat(ARPC_RAW,"0A");

					if(0 == strcmp(auth_tx.TLF01_details.handler_queue,"dcpisoA") )
					{
						auth_tx.EMV_details.future_field1[0]=0x00;
						//int resp_dcp_len =12;
						auth_tx.EMV_details.future_field1[1]=0x12;
						strcpy(temp_response_tag,ARPC_RAW);
						genutil_asc_to_bin( temp_response_tag,temp_txt,&bin_len);
						memcpy(&auth_tx.EMV_details.future_field1[2],temp_txt,2);
						memcpy(&auth_tx.EMV_details.future_field1[4],dummy,10);
					}
					else
					{
						auth_tx.EMV_details.future_field1[0]=0x00;
						auth_tx.EMV_details.future_field1[1]=0x0C;
						//strcpy(temp_response_tag,"01000C");
						//strcpy(temp_response_tag,"000C");
						strcat(temp_response_tag,ARPC_RAW);
						genutil_asc_to_bin( temp_response_tag,temp_txt,&bin_len);
						memcpy(&auth_tx.EMV_details.future_field1[2],temp_txt,2);
						memcpy(&auth_tx.EMV_details.future_field1[4],dummy,10);
					}

		   }

	  return (TRANSACTION_VERIFIED);


}
int racalVerifyARQC_For_VisaCVN18()
{

	   char msgHeader[5];
	   char delimiter[2];
	   char output[500];
	   INT bin_len=0, out_len=0;
	   INT pad_len = 0;
	   int	cnt=0,result=0,jcnt=0;
	   BYTE MK_AC[33]="";
	   BYTE TX_DATA[256]="",temp_txt[100]="";
	   BYTE ARPC_RAW[20]="";
	   INT resp_len=0,i;
	   BYTE temp_msg_len[10]="";
	   INT cardLen =0;
	   INT emv_org_len =0;
	   BYTE tag_data[255]= {0};
	   CHAR err_buf[100] = {0};
	   CHAR emv_len[5] = {0};

	   memset (&msgHeader,0,sizeof(msgHeader));
	   memset (&delimiter,0,sizeof(delimiter));
	   memset (&output,0,sizeof(output));
	   memset (&returnString,0,ReturnString_Size);
	   strcpy (delimiter,";");
	   strcpy (msgHeader ,"1234");

	   strcat(output,msgHeader);
	   strcat(output,"KW"); /* Changed Command code KW- TF Phani*/
			/*Mode flag
				Mode of operation:
				'0' : Perform ARQC verification only
				'1' : Perform ARQC verification and EMV 4.x method 1 ARPC generation
				'2' : Perform EMV 4.x method 1 ARPC generation only
				'3' : Perform ARQC verification and EMV 4.x Method 2 ARPC Generation
				'4' : Perform EMV 4.x Method 2 ARPC generation only
				�5� : Perform ARQC verification and D-PAS Method 1 ARPC generation
				�6� : Perform D-PAS Method 1 ARPC generation only.
			*/
			strcat(output,"3");
			/* Identifier of the EMV scheme:
			 	 Specifies the Key Derivation Method to use
				'0' : VIS 1.4.0 and M/Chip 4 using Card Key Derivation Method A and EMV2000 Session Key Method
				'1' : VIS 1.4.0 and M/Chip 4 using Card Key Derivation Method B and EMV2000 Session Key Method
				'2' : VIS 1.4.0 and M/Chip 4 and D-PAS using Card Key Derivation Method A and EMV Common Session Key Derivation Method
				'3' : VIS 1.4.0 and M/Chip 4 using Card Key Derivation Method B and EMV Common Session Key Derivation Method
			 */
			strcat(output,"2");
			 /* Issuer MAster key - MK-AC */
			 strcpy(MK_AC, auth_tx.CAF01_details.cvk[5].cvkA);
			 strcat(MK_AC, auth_tx.CAF01_details.cvk[5].cvkB);

			if(strlen(MK_AC) == 0)
			{
				racal_log_message( 2, 2,"Master key is blank.","racalVerifyARQC_For_VisaCVN18",1 );
				return(TRANSACTION_NOT_VERIFIED);

			}
			else
			{	strcat(output,"U");
				strcat(output,MK_AC);
			}
			 out_len = strlen(output);
			 /* PAN SEQ NUMBER*/
			 if(strlen(auth_tx.TLF01_details.card_num) >= 14 )
			 {
				  memset(&dummy,0,Dummy_Size);

				  if(strlen(auth_tx.TLF01_details.card_num) == 16)
				  strncpy(dummy,&auth_tx.TLF01_details.card_num[2] ,14);
				  else
				  {	  cardLen= strlen(auth_tx.TLF01_details.card_num);
					  strncpy(dummy,&auth_tx.TLF01_details.card_num[cardLen-14] ,14);
				  }
				  /* need to validate PAN SEQ no  */
				  if(strlen(auth_tx.host2_message_seq_nbr) > 0)
				  { if(strlen(auth_tx.host2_message_seq_nbr) == 4)
						strncat(dummy,&auth_tx.host2_message_seq_nbr[2],2);
					  else if(strlen(auth_tx.host2_message_seq_nbr) == 3)
						strncat(dummy,&auth_tx.host2_message_seq_nbr[1],2);
				  }
				  else if(strlen(auth_tx.EMV_details.pan_sequence_number) > 0)
				  {
					if(strlen(auth_tx.EMV_details.pan_sequence_number) == 2)
						strncat(dummy,&auth_tx.EMV_details.pan_sequence_number[0],2);
					else if(strlen(auth_tx.EMV_details.pan_sequence_number) == 3)
						strncat(dummy,&auth_tx.EMV_details.pan_sequence_number[1],2);
				  }
				  else
				  {
							strcat(dummy,"00");
				  }
				  dummy[16] = '\0';
				  genutil_asc_to_bin( dummy,temp_txt,&bin_len);
				  memcpy(&output[out_len],temp_txt,8);
				  out_len += 8;

			  }
			  	else
			    {
				  racal_log_message( 2, 2,"ARQC validation: card_num is < 14.","racalVerifyARQC_For_VisaCVN18",1 );
				  return (TRANSACTION_NOT_VERIFIED);
			    }
			 /* ATC*/	//	 auth_tx.EMV_details.atc


				if(strlen(auth_tx.EMV_details.atc) == 0)
				{
					racal_log_message( 2, 2,"ATC is blank.","racalVerifyARQC_For_VisaCVN18",1 );
					return(TRANSACTION_NOT_VERIFIED);

				}
				else
				{
					genutil_asc_to_bin( auth_tx.EMV_details.atc,temp_txt,&bin_len);
					printf("ATC - %s \n",auth_tx.EMV_details.atc);
					//strncat(output,temp_txt,bin_len);
					 memcpy(&output[out_len],temp_txt,2);
					 out_len += 2;
				}

				if(0 == strcmp(auth_tx.TLF01_details.handler_queue,"dcpisoA"))
				{
					emv_org_len = genutil_bcd_to_int( auth_tx.EMV_details.emv_block, 2 );
				}
				else
				{
					//strncpy(temp_msg_len,auth_tx.EMV_details.emv_block,3);
					//emv_org_len = atoi(temp_msg_len);

					emv_org_len = auth_tx.EMV_details.emv_block[0];

				}
				memset(tag_data,0x00,sizeof(tag_data));
				bin_len=get_EMV_CVN_18_data(emv_org_len,&auth_tx.EMV_details.emv_block,&tag_data);
				bin_len = BIN_LEN_CVN18;

				pad_len = bin_len%8;
				if (pad_len > 0)
					bin_len += (7 - pad_len); // Pad with Zero's

				printf("EMV TX DATA LEN - %d \n",bin_len);
				printf("EMV TX DATA  - %s \n",tag_data);
				sprintf(&output[out_len],"%2X",bin_len);
					out_len +=2;
				memcpy(&output[out_len],tag_data,bin_len);
				out_len += bin_len;
			 /* Tx data*/ // TAGs

			 /* Deliemter*/

				memcpy(&output[out_len],delimiter,1);
				out_len += 1;
			 /* ARQC/ARPC*/

				genutil_asc_to_bin( auth_tx.EMV_details.app_crypto,temp_txt,&bin_len);
				memcpy(&output[out_len],temp_txt,8);
				out_len += 8;
			 /* CSU*/

				output[out_len] = 0x00;out_len += 1;
				output[out_len] = 0x00;out_len += 1;
				output[out_len] = 0x00;out_len += 1;
				output[out_len] = 0x00;out_len += 1;

				/*Proprietary Authentication Data Length */
				pad_len = 7;
				sprintf(&output[out_len],"%1x",pad_len);
				out_len +=1;

				/*Proprietary Authentication Data*/
				memset(temp_txt,0x00, sizeof(temp_txt));
				genutil_asc_to_bin( auth_tx.EMV_details.iad ,temp_txt,&pad_len);
				memcpy(&output[out_len],temp_txt,7);
				out_len += 7;


			 /* End of msg*/
			 //if the output is bigger then 500 do not send it out.
			 if (out_len > 500)
			 {
				racal_log_message( 2, 2,"ARQC Cmd Msg to long.","racalVerifyARQC_For_VisaCVN18",1 );
				return(TRANSACTION_NOT_VERIFIED);
			 }
			 //send the message to the racal box.
			 if (racalSendToPort_Bin(output,out_len) == TRANSACTION_VERIFIED)
			 {
				/*parse the message check for the message header.*/
				memset(&dummy,0,Dummy_Size);
				strncpy(dummy,returnString,4);
				if(strcmp(dummy,msgHeader) != 0)
				{
				   racal_log_message( 2, 2,"ARQC Cmd header mis-match.","racalVerifyARQC_For_VisaCVN18",1 );
				   return(TRANSACTION_NOT_VERIFIED);
				}
				/*parse the message check for the response code.*/
				memset(&dummy,0,Dummy_Size);
				strncpy(dummy,returnString + 4, 2);

				if(strcmp(dummy,"KX") !=0)
				{
				   racal_log_message( 2, 2,"ARQC Cmd Code mis-match.","racalVerifyARQC_For_VisaCVN18",1 );
				   return(TRANSACTION_NOT_VERIFIED);
				}
				/*parse the message check for the error code.*/
				memset(&dummy,0,Dummy_Size);
				strncpy(dummy,returnString + 6, 2);

				  //if this value is not 00 reject the transaction.
				/* '00' : No error
				'01' : ARQC/TC/AAC verification failed
				'04' : Mode Flag not 0, 1 or 2
				'05' : Unrecognised Scheme ID
				'10' : MK parity error
				'68' : Command disabled
				*/
				if(strcmp(dummy,"00") != 0)
				{
				   if(strcmp(dummy,"01") == 0)
					{
					   racal_log_message( 2, 2,"ARPC AAC validation failed.","racalVerifyARQC_For_VisaCVN18",1 );
				   }
				   if(strcmp(dummy,"04") == 0)
					  racal_log_message( 2, 2,"Mode flag not 0 1 or 2.","racalVerifyARQC_For_VisaCVN18",1 );
				   if(strcmp(dummy,"05") == 0)
					  racal_log_message( 2, 2,"Unrecognised Scheme ID.","racalVerifyARQC_For_VisaCVN18",1 );
				   if(strcmp(dummy,"10") == 0)
					  racal_log_message( 2, 2,"MK parity error.","racalVerifyARQC_For_VisaCVN18",1 );
				   if(strcmp(dummy,"68") == 0)
					  racal_log_message( 2, 2,"Command disabled.","racalVerifyARQC_For_VisaCVN18",1 );
					 return(TRANSACTION_NOT_VERIFIED);
				}
				CHAR temp_response_tag[35]={0};
				memcpy(dummy,returnString + 8, 8);		 // This is the ARPC value.

     			 // Prepare EMV block for response
				 strcpy(ARPC_RAW,"91");
				 strcat(ARPC_RAW,"0A");
				 memset( auth_tx.EMV_details.future_field1,0x00,
						 sizeof(auth_tx.EMV_details.future_field1));
				if(0 == strcmp(auth_tx.TLF01_details.handler_queue,"dcpisoA") )
				{
					auth_tx.EMV_details.future_field1[0]=0x00;
					//int resp_dcp_len =12;
					auth_tx.EMV_details.future_field1[1]=0x12;
					strncpy(temp_response_tag,ARPC_RAW,4);
					genutil_asc_to_bin( temp_response_tag,temp_txt,&bin_len);
					memcpy(&auth_tx.EMV_details.future_field1[2],temp_txt,2);
					memcpy(&auth_tx.EMV_details.future_field1[4],dummy,10);
				}
				else
				{
					auth_tx.EMV_details.future_field1[0]=0x0F;
					strcpy(temp_response_tag,"01000C");
					strcat(temp_response_tag,ARPC_RAW);
					genutil_asc_to_bin( temp_response_tag,temp_txt,&bin_len);
					memcpy(&auth_tx.EMV_details.future_field1[1],temp_txt,5);
					memcpy(&auth_tx.EMV_details.future_field1[6],dummy,10);
				}
		   }
	  return (TRANSACTION_VERIFIED);
}


int racalVerifyARQC_For_Diners()
{

	   char msgHeader[5];
	   char delimiter[2];
	   char output[500];
	   INT bin_len=0, out_len=0;
	   int	cnt=0,result=0,jcnt=0;
	   BYTE MK_AC[33]="";
	   BYTE TX_DATA[256]="",temp_txt[100]="";
	   BYTE ARPC_RAW[20]="";
	   INT resp_len=0,i;
	   BYTE temp_msg_len[10]="";
	   INT cardLen =0;
	   INT emv_org_len =0;
	   BYTE tag_data[255]= {0};
	   CHAR err_buf[100] = {0};
	   CHAR emv_len[5] = {0};

	   memset (&msgHeader,0,sizeof(msgHeader));
	   memset (&delimiter,0,sizeof(delimiter));
	   memset (&output,0,sizeof(output));
	   memset (&returnString,0,ReturnString_Size);
	   strcpy (delimiter,";");
	   strcpy (msgHeader ,"1234");

	   strcat(output,msgHeader);
	   strcat(output,"KW"); /* Changed Command code KW- TF Phani*/
			/*Mode flag
				Mode of operation:
			 '5' : Perform ARQC verification and D-PAS Method 1 ARPC generation
			*/
			strcat(output,"5");
			/* Identifier of the EMV scheme:

			 '2' : American Express AEIPS 4.1
			 */
			strcat(output,"2");
			 /* Issuer MAster key - MK-AC */
			 strcpy(MK_AC, auth_tx.CAF01_details.cvk[5].cvkA);
			 strcat(MK_AC, auth_tx.CAF01_details.cvk[5].cvkB);

			 if(strlen(MK_AC) == 0)
			{
				racal_log_message( 2, 2,"Master key is blank.","racalVerifyARQC_for_Diners",1 );
				return(TRANSACTION_NOT_VERIFIED);

			}else
			{	strcat(output,"U");
				strcat(output,MK_AC);
			}
			 out_len = strlen(output);
			 /* PAN SEQ NUMBER*/
			 if(strlen(auth_tx.TLF01_details.card_num) >= 14 )
			 {
				  memset(&dummy,0,Dummy_Size);

				  if(strlen(auth_tx.TLF01_details.card_num) == 16)
				  strncpy(dummy,&auth_tx.TLF01_details.card_num[2] ,14);
				  else
				  {	  cardLen= strlen(auth_tx.TLF01_details.card_num);
					  strncpy(dummy,&auth_tx.TLF01_details.card_num[cardLen-14] ,14);
				  }
				  /* need to validate PAN SEQ no  */
				 if(strlen(auth_tx.EMV_details.pan_sequence_number) > 0)
				  {
					if(strlen(auth_tx.EMV_details.pan_sequence_number) == 2)
						strncat(dummy,&auth_tx.EMV_details.pan_sequence_number[0],2);
					else if(strlen(auth_tx.EMV_details.pan_sequence_number) == 3)
						strncat(dummy,&auth_tx.EMV_details.pan_sequence_number[1],2);
				  }
				  else if(strlen(auth_tx.host2_message_seq_nbr) > 0)
				  { if(strlen(auth_tx.host2_message_seq_nbr) == 4)
						strncat(dummy,&auth_tx.host2_message_seq_nbr[2],2);
					  else if(strlen(auth_tx.host2_message_seq_nbr) == 3)
						strncat(dummy,&auth_tx.host2_message_seq_nbr[1],2);
				   }
				  else
							strcat(dummy,"00");
				  dummy[16] = '\0';
				  genutil_asc_to_bin( dummy,temp_txt,&bin_len);
				  memcpy(&output[out_len],temp_txt,8);
				  out_len += 8;

			  }
			  else
			  {
				  racal_log_message( 2, 2,"ARQC validation: card_num is < 14.","racalVerifyARQC_for_Diners",1 );
				  return (TRANSACTION_NOT_VERIFIED);
			   }
			 /* ATC*/	//	 auth_tx.EMV_details.atc


				if(strlen(auth_tx.EMV_details.atc) == 0)
				{
					racal_log_message( 2, 2,"ATC is blank.","racalVerifyARQC_for_Diners",1 );
					return(TRANSACTION_NOT_VERIFIED);

				}else
				{
					genutil_asc_to_bin( auth_tx.EMV_details.atc,temp_txt,&bin_len);
					printf("ATC - %s \n",auth_tx.EMV_details.atc);
					//strncat(output,temp_txt,bin_len);
					 memcpy(&output[out_len],temp_txt,2);
					 out_len += 2;
				}

			 /* Tx data Len*/
			 /* Tx Data gather for CVN
		#1
				 CVN 05

				Tags '9F02', '9F03', '9F1A', '95', '5F2A', '9A', '9C', '9F37', '82' ,'9F36', �9F10� (CVR part)

				CVN 15

				Tags '9F02',  '5F2A',  '9F37', '9F36', �9F10� (Full)
			*/

				if(0 == strcmp(auth_tx.TLF01_details.handler_queue,"dcpisoA"))
				{
					emv_org_len = genutil_bcd_to_int( auth_tx.EMV_details.emv_block, 2 );
				}
				else
				{
					strncpy(temp_msg_len,auth_tx.EMV_details.emv_block,3);
					emv_org_len = atoi(temp_msg_len);

				}
				printf("EMV LEN - %d \n",emv_org_len);
				memset(tag_data,0x00,sizeof(tag_data));
				if(strncmp(auth_tx.TLF01_details.pos_entry_mode ,"005",3) == 0)
				{ // contact
					bin_len=get_EMV_CVN_05_data_Diners(emv_org_len,&auth_tx.EMV_details.emv_block,&tag_data);
					//bin_len = 48;
				}else
				{// contactless
					bin_len=get_EMV_CVN_15_data_Diners(emv_org_len,&auth_tx.EMV_details.emv_block,&tag_data);
					//bin_len = 32;
				}
				 int pad_len = bin_len%8;
				 if (pad_len > 0)
					bin_len += (8-pad_len); // Pad with Zero's


				printf("EMV TX DATA LEN - %d \n",bin_len);
				printf("EMV TX DATA  - %s \n",tag_data);
				sprintf(&output[out_len],"%2x",bin_len);
					out_len +=2;
				memcpy(&output[out_len],tag_data,bin_len);
				out_len += bin_len;
			 /* Tx data*/ // TAGs

			 /* Deliemter*/

				 memcpy(&output[out_len],delimiter,1);
				 out_len += 1;
			 /* ARQC/ARPC*/

			 genutil_asc_to_bin( auth_tx.EMV_details.app_crypto,temp_txt,&bin_len);

			 memcpy(&output[out_len],temp_txt,8);
				 out_len += 8;
			 /* CSU*/
			// 0x38 /
			//contact
			//if(0 == strncmp(auth_tx.TLF01_details.handler_queue,"ncdci",4) ||
			//  (strncmp(auth_tx.TLF01_details.pos_entry_mode ,"0051",4) == 0))
			if(strncmp(auth_tx.TLF01_details.pos_entry_mode ,"005",3) == 0)
			{
				output[out_len] = 0x00;out_len += 1;
				output[out_len] = 0x32;out_len += 1;
				output[out_len] = 0x00;out_len += 1;
				output[out_len] = 0x00;out_len += 1;
			}
			else
			{
				 out_len += 4;
			}

			 /* End of msg*/
			 //if the output is bigger then 500 do not send it out.
			 if (out_len > 500)
			 {
				racal_log_message( 2, 2,"ARQC Cmd Msg to long.","racalVerifyARQC_for_Diners",1 );
				return(TRANSACTION_NOT_VERIFIED);
			 }
			 //send the message to the racal box.
			 if (racalSendToPort_Bin(output,out_len) == TRANSACTION_VERIFIED)
			 {
				/*parse the message check for the message header.*/
				memset(&dummy,0,Dummy_Size);
				strncpy(dummy,returnString,4);
				if(strcmp(dummy,msgHeader) != 0)
				{
				   racal_log_message( 2, 2,"ARQC Cmd header mis-match.","racalVerifyARQC_for_Diners",1 );
				   return(TRANSACTION_NOT_VERIFIED);
				}
				/*parse the message check for the response code.*/
				memset(&dummy,0,Dummy_Size);
				strncpy(dummy,returnString + 4, 2);

				if(strcmp(dummy,"KX") !=0)
				{
				   racal_log_message( 2, 2,"ARQC Cmd Code mis-match.","racalVerifyARQC_for_Diners",1 );
				   return(TRANSACTION_NOT_VERIFIED);
				}
				/*parse the message check for the error code.*/
				memset(&dummy,0,Dummy_Size);
				strncpy(dummy,returnString + 6, 2);

				  //if this value is not 00 reject the transaction.
				/* '00' : No error
				'01' : ARQC/TC/AAC verification failed
				'04' : Mode Flag not 0, 1 or 2
				'05' : Unrecognised Scheme ID
				'10' : MK parity error
				'68' : Command disabled
				*/
				if(strcmp(dummy,"00") != 0)
				{
				   if(strcmp(dummy,"01") == 0)
					{
					   racal_log_message( 2, 2,"ARPC AAC validation failed.","racalVerifyARQC_for_Diners",1 );
				   }
				   if(strcmp(dummy,"04") == 0)
					  racal_log_message( 2, 2,"Mode flag not 0 1 or 2.","racalVerifyARQC_for_Diners",1 );
				   if(strcmp(dummy,"05") == 0)
					  racal_log_message( 2, 2,"Unrecognised Scheme ID.","racalVerifyARQC_for_Diners",1 );
				   if(strcmp(dummy,"10") == 0)
					  racal_log_message( 2, 2,"MK parity error.","racalVerifyARQC_for_Diners",1 );
				   if(strcmp(dummy,"68") == 0)
					  racal_log_message( 2, 2,"Command disabled.","racalVerifyARQC_for_Diners",1 );
					 return(TRANSACTION_NOT_VERIFIED);
				}
				CHAR temp_response_tag[35]={0};
				memcpy(dummy,returnString + 8, 8);		 // This is the ARPC value.

				if(strncmp(auth_tx.TLF01_details.pos_entry_mode ,"005",3) == 0)
				{
					 dummy[8]=0x00;
					 dummy[9]=0x32;
				}
				else
				{
					 dummy[8]=0x00;
					 dummy[9]=0x00;
				}
				 // Prepare EMV block for response
				 strcpy(ARPC_RAW,"91");
				 strcat(ARPC_RAW,"0A");
				 memset( auth_tx.EMV_details.future_field1,0x00,
						 sizeof(auth_tx.EMV_details.future_field1));
				if(0 == strcmp(auth_tx.TLF01_details.handler_queue,"dcpisoA") )
				{
					auth_tx.EMV_details.future_field1[0]=0x00;
					//int resp_dcp_len =12;
					auth_tx.EMV_details.future_field1[1]=0x12;
					strncpy(temp_response_tag,ARPC_RAW,4);
					genutil_asc_to_bin( temp_response_tag,temp_txt,&bin_len);
					memcpy(&auth_tx.EMV_details.future_field1[2],temp_txt,2);
					memcpy(&auth_tx.EMV_details.future_field1[4],dummy,10);
				}
				else
				{
					// This is issuing tx: Prepare the full repsone msg with TAG 91
					strncpy(auth_tx.EMV_details.future_field1,"012",3);
					strncpy(temp_response_tag,ARPC_RAW,4);
					genutil_asc_to_bin( temp_response_tag,temp_txt,&bin_len);
					memcpy(&auth_tx.EMV_details.future_field1[3],temp_txt,2);
					memcpy(&auth_tx.EMV_details.future_field1[5],dummy,10);
				}
		   }
	  return (TRANSACTION_VERIFIED);
}  // end of Diners_EMV_ARQC


int racalVerifyARQC_for_EMV()
{
	   char msgHeader[5];
	   char delimiter[2];
	   char output[500];
	   INT bin_len=0, out_len=0;
	   int	cnt=0,result=0,jcnt=0;
	  // CHAR mode_falg={0};
	   BYTE MK_AC[33]="";
	   BYTE TX_DATA[256]="",temp_txt[100]="";
	   BYTE ARPC_RAW[20]="";
	   INT resp_len=0,i;
	   BYTE temp_msg_len[10]="";
	  
	   memset (&msgHeader,0,sizeof(msgHeader));
	   memset (&delimiter,0,sizeof(delimiter));
	   memset (&output,0,sizeof(output));
	   memset (&returnString,0,ReturnString_Size);
	   strcpy (delimiter,";");
	   strcpy (msgHeader ,"1234");
	
			 strcat(output,msgHeader);
			 strcat(output,"KQ"); /* Changed Command code KQ- TF Phani*/
			/*Mode flag
				Mode of operation:
			 '0' : Perform ARQC Verification only 
			 '1' : Perform ARQC Verification and ARPC Generation 
			 '2' : Perform ARPC Generation only.	
			*/ 
			strcat(output,"1");
			// strcat(output,"2");
			//mode_falg = '2';
			/* Identifier of the EMV scheme:
			'0' : Visa VSDC or UKIS 
			 '1' : Europay or MasterCard M/Chip 
			 '2' : American Express AEIPS 4.1 */
			if (strcmp(auth_tx.BIN01_details.card_family,CARD_VISA) == 0)
				strcat(output,"0");
			else if (strcmp(auth_tx.BIN01_details.card_family,CARD_MASTERCARD) == 0)
				strcat(output,"1");
			else if (strcmp(auth_tx.BIN01_details.card_family,CARD_AMEX) == 0)
				strcat(output,"2");
				
		
			 /* Issuer MAster key - MK-AC */
			 strcpy(MK_AC, auth_tx.CAF01_details.cvk[5].cvkA);
			 strcat(MK_AC, auth_tx.CAF01_details.cvk[5].cvkB);
		
			 if(strlen(MK_AC) == 0)
			{
				racal_log_message( 2, 2,"Master key is blank.","racalVerifyARQC_for_EMV",1 );
				return(TRANSACTION_NOT_VERIFIED);
	
			}else
			{	strcat(output,"U");
				strcat(output,MK_AC);
			}
			 out_len = strlen(output);
			 /* PAN SEQ NUMBER*/
			 if(strlen(auth_tx.TLF01_details.card_num) >= 14 )
			 {
				  memset(&dummy,0,Dummy_Size);
				  
				  if (strcmp(auth_tx.BIN01_details.card_family,CARD_AMEX) == 0)
				  strncpy(dummy,&auth_tx.TLF01_details.card_num[1] ,14);
				  else
				  strncpy(dummy,&auth_tx.TLF01_details.card_num[2] ,14);
				  /* need to validate PAN SEQ no  */
				  if(strcmp(auth_tx.BIN01_details.card_family,CARD_AMEX) == 0)
				  {
				 if(strlen(auth_tx.EMV_details.pan_sequence_number) > 0)
				  {
					if(strlen(auth_tx.EMV_details.pan_sequence_number) == 2)
						strncat(dummy,&auth_tx.EMV_details.pan_sequence_number[0],2);
					else if(strlen(auth_tx.EMV_details.pan_sequence_number) == 3)
						strncat(dummy,&auth_tx.EMV_details.pan_sequence_number[1],2);
				  }
				  else if(strlen(auth_tx.host2_message_seq_nbr) > 0)
					  {
						  if(strlen(auth_tx.host2_message_seq_nbr) == 4)
							  strncat(dummy,&auth_tx.host2_message_seq_nbr[2],2);
						  else if(strlen(auth_tx.host2_message_seq_nbr) == 3)
							  strncat(dummy,&auth_tx.host2_message_seq_nbr[1],2);
					  }
					  else
					  	strcat(dummy,"00");
				}
				else
				{
					if(strlen(auth_tx.host2_message_seq_nbr) > 0)
					{
						if(strlen(auth_tx.host2_message_seq_nbr) == 4)
						strncat(dummy,&auth_tx.host2_message_seq_nbr[2],2);
					  else if(strlen(auth_tx.host2_message_seq_nbr) == 3)
						strncat(dummy,&auth_tx.host2_message_seq_nbr[1],2); 
					}
				    else if(strlen(auth_tx.EMV_details.pan_sequence_number) > 0)
				    {
				    	if(strlen(auth_tx.EMV_details.pan_sequence_number) == 2)
				    		strncat(dummy,&auth_tx.EMV_details.pan_sequence_number[0],2);
				    	else if(strlen(auth_tx.EMV_details.pan_sequence_number) == 3)
				    		strncat(dummy,&auth_tx.EMV_details.pan_sequence_number[1],2);
				    }
				  else 
							strcat(dummy,"00");
				}
				  dummy[16] = '\0';
				  genutil_asc_to_bin( dummy,temp_txt,&bin_len); 	 
				  //strncat(output,temp_txt,bin_len);
				  memcpy(&output[out_len],temp_txt,8);
				  out_len += 8;
				  //strcat(output,dummy);
			  }
			  else
			  {
				  racal_log_message( 2, 2,"ARQC validation: card_num is < 14.","racalVerifyARQC_for_EMV",1 );
				  return (TRANSACTION_NOT_VERIFIED);
			   }
			 /* ATC*/	//	 auth_tx.EMV_details.atc
			 // ONLY For MC , for VISA and AMEX  0000
		   if (strcmp(auth_tx.BIN01_details.card_family,CARD_MASTERCARD) == 0)
			{	
				if(strlen(auth_tx.EMV_details.atc) == 0)
				{
					racal_log_message( 2, 2,"ATC is blank.","racalVerifyARQC_for_EMV",1 );
					return(TRANSACTION_NOT_VERIFIED);
	
				}else
				{ 
					genutil_asc_to_bin( auth_tx.EMV_details.atc,temp_txt,&bin_len); 	
					printf("ATC - %s \n",auth_tx.EMV_details.atc);
					//strncat(output,temp_txt,bin_len);
					 memcpy(&output[out_len],temp_txt,2);
					 out_len += 2;
				}
				if(strlen(auth_tx.EMV_details.unpredictable_number) == 0)
				{
					racal_log_message( 2, 2,"Unpredctible No is blank.","racalVerifyARQC_for_EMV",1 );
					return(TRANSACTION_NOT_VERIFIED);
				}else
				{	genutil_asc_to_bin( auth_tx.EMV_details.unpredictable_number,temp_txt,&bin_len);	 
					memcpy(&output[out_len],temp_txt,4);
					 out_len += 4;
				}
			}else
			{
					genutil_asc_to_bin("000000000000",temp_txt,&bin_len);
					memcpy(&output[out_len],temp_txt,6);
						  out_len += 6;
			}
			 /* Tx data Len*/ 
			 /* Tx Data gather for CVN 
		#1 
				Visa CVN 10
	 
				Tags '9F02', '9F03', '9F1A', '95', '5F2A', '9A', '9C', '9F37', '82' ,'9F36', �9F10� (CVR part)
	 
				Visa CVN 17
	 
				Tags �9F02�, �9F37�,�9F36�, �9F10� ( IAD Byte 5) .
	 
				MC CVN 16
	 
				Tags '9F02', '9F03', '9F1A', '95', '5F2A', '9A', '9C', '9F37', '82' ,'9F36', �9F10� (CVR part)
			*/
				int emv_org_len =0;
				BYTE tag_data[255]= {0};
				if(0 == strcmp(auth_tx.TLF01_details.handler_queue,"dcpisoA"))
				{
					emv_org_len = genutil_bcd_to_int( auth_tx.EMV_details.emv_block, 2 );
				}
				else		
				{
					 if (strcmp(auth_tx.BIN01_details.card_family,CARD_MASTERCARD) == 0)
						 emv_org_len = genutil_bcd_to_int( auth_tx.EMV_details.emv_block, 2 );
					 else if (strcmp(auth_tx.BIN01_details.card_family,CARD_VISA) == 0)
						  emv_org_len = auth_tx.EMV_details.emv_block[0];
					 else if (strcmp(auth_tx.BIN01_details.card_family,CARD_AMEX) == 0)
					 	{
					 		for( i=0; i<3; i++ )
					      {
					         temp_msg_len[i] = ascii_ebcdic_conv (auth_tx.EMV_details.emv_block[i], E2A);
					      }
					 	emv_org_len = atoi(temp_msg_len);
					 	}
				} 
				printf("EMV LEN - %d \n",emv_org_len);
				memset(tag_data,0x00,sizeof(tag_data));
				if (strcmp(auth_tx.BIN01_details.card_family,CARD_AMEX) == 0 )
				{
				
					if(0 == strcmp(auth_tx.TLF01_details.handler_queue,"dcpisoA") )
					{
						/*if(strncmp(auth_tx.TLF01_details.pos_entry_mode,"007",3) == 0 || 
						   strncmp(auth_tx.TLF01_details.pos_entry_mode,"07",2) == 0 
						   )
						{
						bin_len=	get_EMV_CVN_17_data(emv_org_len,&auth_tx.EMV_details.emv_block,&tag_data);
						bin_len= 16;	
						}
						else */
						{
						bin_len=get_EMV_CVN_10_16_data(emv_org_len,&auth_tx.EMV_details.emv_block,&tag_data);
					 	bin_len = 37;
						}
					}
					else
					{
						bin_len = get_EMV_Data_AMEX(emv_org_len,&auth_tx.EMV_details.emv_block,&tag_data);

					}

				}
				/*else if (strcmp(auth_tx.BIN01_details.card_family,CARD_VISA) == 0 &&
					(strncmp(auth_tx.TLF01_details.pos_entry_mode,"007",3) == 0 || 
 					strncmp(auth_tx.TLF01_details.pos_entry_mode,"07",2) == 0 ))
				 {
						printf("calling get_EMV_CVN_17_data - \n");
						
						bin_len=get_EMV_CVN_17_data(emv_org_len,&auth_tx.EMV_details.emv_block,&tag_data);
						bin_len= 16;
				 }*/
				else if (strcmp(auth_tx.BIN01_details.card_family,CARD_VISA) == 0 )
				{
					if(strncmp(auth_tx.EMV_details.iad+4,"0A",2) == 0)
					{
						bin_len=get_EMV_CVN_10_16_data(emv_org_len,&auth_tx.EMV_details.emv_block,&tag_data);
						bin_len = BIN_LEN_CVN10;
					}
					else
					{
						bin_len=get_EMV_CVN_17_data(emv_org_len,&auth_tx.EMV_details.emv_block,&tag_data);
						bin_len= BIN_LEN_CVN17;
					}

				}
				 else
				 {
						printf("calling get_EMV_CVN_16_data - \n"); 		
						bin_len=get_EMV_CVN_10_16_data(emv_org_len,&auth_tx.EMV_details.emv_block,&tag_data);
						bin_len = 40;
				 }
				 int pad_len = bin_len%8;
				 if (pad_len > 0)
				 {
						bin_len += (8-pad_len);
				 }// Pad with Zero's

				/*if (strcmp(auth_tx.BIN01_details.card_family,CARD_AMEX) == 0 &&
					(strncmp(auth_tx.TLF01_details.pos_entry_mode,"007",3) == 0 || 
 					strncmp(auth_tx.TLF01_details.pos_entry_mode,"07",2) == 0 ))
					bin_len = 16;
				else */if(strcmp(auth_tx.BIN01_details.card_family,CARD_AMEX) == 0)
					bin_len = 37;
				
				printf("EMV TX DATA LEN - %d \n",bin_len);
				printf("EMV TX DATA  - %s \n",tag_data);	

				sprintf(&output[out_len],"%2x",bin_len);

					out_len +=2;
				memcpy(&output[out_len],tag_data,bin_len);	
				out_len += bin_len;
			 /* Tx data*/ // TAGs 
			
			 /* Deliemter*/
			 
				 memcpy(&output[out_len],delimiter,1);
				 out_len += 1;
			 /* ARQC/ARPC*/
			
			 genutil_asc_to_bin( auth_tx.EMV_details.app_crypto,temp_txt,&bin_len); 	 
			 //strncat(output, temp_txt,bin_len);
			 //printf("APP CRYPTO - %s \n",auth_tx.EMV_details.app_crypto);
			 memcpy(&output[out_len],temp_txt,8);
				 out_len += 8;
			 /* ARC*/
			if (strcmp(auth_tx.BIN01_details.card_family,CARD_VISA) == 0)
			 {
					 //genutil_asc_to_bin( "0000",temp_txt,&bin_len);
					 memcpy(&output[out_len],"00",2);
					 out_len += 2;			
			 }else if (strcmp(auth_tx.BIN01_details.card_family,CARD_MASTERCARD) == 0)
			 {
					 genutil_asc_to_bin( "0010",temp_txt,&bin_len); 	 
					 memcpy(&output[out_len],temp_txt,2);
					 out_len += 2;
			 }else if (strcmp(auth_tx.BIN01_details.card_family,CARD_AMEX) == 0)
			 	{
			 	// genutil_asc_to_bin( "0000",temp_txt,&bin_len); 	 
					 memcpy(&output[out_len],"00",2);
					 out_len += 2;
			 	}
			

			 /* End of msg*/
			 //if the output is bigger then 500 do not send it out.
			 if (out_len > 500)
			 {
				racal_log_message( 2, 2,"ARQC Cmd Msg to long.","racalVerifyARQC_for_EMV",1 );
				return(TRANSACTION_NOT_VERIFIED);
			 }
			 //send the message to the racal box.
			 if (racalSendToPort_Bin(output,out_len) == TRANSACTION_VERIFIED)
			 {
				/*parse the message check for the message header.*/
				memset(&dummy,0,Dummy_Size);
				strncpy(dummy,returnString,4);
				if(strcmp(dummy,msgHeader) != 0)
				{
				   racal_log_message( 2, 2,"ARQC Cmd header mis-match.","racalVerifyARQC_for_EMV",1 );
				   return(TRANSACTION_NOT_VERIFIED);
				}
				/*parse the message check for the response code.*/
				memset(&dummy,0,Dummy_Size);
				strncpy(dummy,returnString + 4, 2);
	
				if(strcmp(dummy,"KR") !=0)
				{
				   racal_log_message( 2, 2,"ARQC Cmd Code mis-match.","racalVerifyARQC_for_EMV",1 );
				   return(TRANSACTION_NOT_VERIFIED);
				}
	
				/*parse the message check for the error code.*/
				memset(&dummy,0,Dummy_Size);
				strncpy(dummy,returnString + 6, 2);
				
				  //if this value is not 00 reject the transaction.
				/* '00' : No error 
				'01' : ARQC/TC/AAC verification failed 
				'04' : Mode Flag not 0, 1 or 2 
				'05' : Unrecognised Scheme ID 
				'10' : MK parity error 
				'68' : Command disabled
				*/
				if(strcmp(dummy,"00") != 0)
				{
				   if(strcmp(dummy,"01") == 0)
					{
					   racal_log_message( 2, 2,"ARPC AAC validation failed.","racalVerifyARQC_for_EMV",1 );
				   }
				   if(strcmp(dummy,"04") == 0)
					  racal_log_message( 2, 2,"Mode flag not 0 1 or 2.","racalVerifyARQC_for_EMV",1 );
				   if(strcmp(dummy,"05") == 0)
					  racal_log_message( 2, 2,"Unrecognised Scheme ID.","racalVerifyARQC_for_EMV",1 );
				   if(strcmp(dummy,"10") == 0)
					  racal_log_message( 2, 2,"MK parity error.","racalVerifyARQC_for_EMV",1 );
				   if(strcmp(dummy,"68") == 0)
					  racal_log_message( 2, 2,"Command disabled.","racalVerifyARQC_for_EMV",1 );
					 return(TRANSACTION_NOT_VERIFIED);
				}
	
				CHAR temp_response_tag[35]={0};
				memcpy(dummy,returnString + 8, 8);		 // This is the ARPC value.
				 if (strcmp(auth_tx.BIN01_details.card_family,CARD_MASTERCARD) == 0)
				 {	
					 dummy[8]=0x00;
					 dummy[9]=0x10; 
				 }
				 else  if (strcmp(auth_tx.BIN01_details.card_family,CARD_VISA) == 0 || 
				 	strcmp(auth_tx.BIN01_details.card_family,CARD_AMEX) == 0)
				 {
					 dummy[8]=0x30;
					 dummy[9]=0x30; 
				 }
				 // Prepare EMV block for response
				 strcpy(ARPC_RAW,"91");
				 strcat(ARPC_RAW,"0A");
	
					if(0 == strcmp(auth_tx.TLF01_details.handler_queue,"dcpisoA") )
					{
						auth_tx.EMV_details.future_field1[0]=0x00;
						//int resp_dcp_len =12; 
						auth_tx.EMV_details.future_field1[1]=0x12;
						strcpy(temp_response_tag,ARPC_RAW);
						genutil_asc_to_bin( temp_response_tag,temp_txt,&bin_len);
						memcpy(&auth_tx.EMV_details.future_field1[2],temp_txt,2);
						memcpy(&auth_tx.EMV_details.future_field1[4],dummy,10);
					}
					else		
					{
						 if (strcmp(auth_tx.BIN01_details.card_family,CARD_MASTERCARD) == 0)
						 {	sprintf(auth_tx.EMV_details.future_field1,"%03d",12);
							strcpy(temp_response_tag,ARPC_RAW);
							genutil_asc_to_bin( temp_response_tag,temp_txt,&bin_len);
							memcpy(&auth_tx.EMV_details.future_field1[3],temp_txt,2);
							memcpy(&auth_tx.EMV_details.future_field1[5],dummy,10);
						 }
						 else  if (strcmp(auth_tx.BIN01_details.card_family,CARD_VISA) == 0)
						 {
							auth_tx.EMV_details.future_field1[0]=0x0F;
							 strcpy(temp_response_tag,"01000C");
							 strcat(temp_response_tag,ARPC_RAW);
							genutil_asc_to_bin( temp_response_tag,temp_txt,&bin_len);
							memcpy(&auth_tx.EMV_details.future_field1[1],temp_txt,5);
							memcpy(&auth_tx.EMV_details.future_field1[6],dummy,10);
						 }else if (strcmp(auth_tx.BIN01_details.card_family,CARD_AMEX) == 0)
						 {
							auth_tx.EMV_details.future_field1[0]=0x0A;
							memcpy(&auth_tx.EMV_details.future_field1[1],dummy,10); 	
						
						 }
				
					}
			
		   }
	
	  return (TRANSACTION_VERIFIED);
}

/*************************************************************************************
       NAME:           racalVerify_CVN2_for_CUP_Ecom()

       DESCRIPTION:    This module will deal with the 3D secure CAVV verification.


       INPUTS:
		 	None.

       OUTPUTS:
         None
       RETURNS:
		   None.

      AUTHOR:        Abhishek Kumar Verma
*************************************************************************************/
int racalVerify_CVN2_for_CUP_Ecom()
{
	char msgHeader[5] = {0};
	char cmdCode[3] = {0};
	char cvn2_value[5] = {0};
	char delimiter[2] = {0};
	char output[200]= {0};
	BYTE temp_len[4] = {0};
	char BUFFER [256] = {0};
	int len = 0;
	memset (&msgHeader,0,sizeof(msgHeader));
	memset (&cmdCode,0,sizeof(cmdCode));
	memset (&delimiter,0,sizeof(delimiter));
	memset (&output,0,sizeof(output));

	strcpy (msgHeader ,"1234");
	strcpy (cmdCode,"CY");
	strcpy (delimiter,";");

	/*edit CVK A */
	if(strlen(auth_tx.CAF01_details.cvk[0].cvkA) == 0)
	{
		racal_log_message( 2, 2, "CVN2-validation cvkA is blank.","racalVerify_CVN2_for_CUP_Ecom",1 );
		return(TRANSACTION_NOT_VERIFIED);
	}
	/*edit CVK B */
	if(strlen(auth_tx.CAF01_details.cvk[0].cvkB) == 0)
	{
		racal_log_message( 2, 2, "CVN2-validation cvkB is blank.","racalVerify_CVN2_for_CUP_Ecom",1 );
		return(TRANSACTION_NOT_VERIFIED);
	}
	/*edit card number*/
	if(strlen(auth_tx.TLF01_details.card_num) == 0 )
	{
		racal_log_message( 2, 2,"CVN2-validation: card_num is blank.","racalVerify_CVN2_for_CUP_Ecom",1 );
		return(TRANSACTION_NOT_VERIFIED);
	}
	/*edit expiration date*/
	if(strlen(auth_tx.TLF01_details.exp_date) == 0)
	{
		 racal_log_message( 2, 2, "CVN2-validation: exp_date is blank.","racalVerify_CVN2_for_CUP_Ecom",1 );
		 return(TRANSACTION_NOT_VERIFIED);
	}
	// Lets get CVN2 value present in DE61.4
	memcpy( temp_len, auth_tx.TLF01_details.product_codes[13].code,CUP_DE61_LEN_SIZE);
	len = atoi(temp_len);
	if(len >= CUP_DE61_CVN2_TOTAL_LEN)
	{
		strncpy(cvn2_value,auth_tx.future_use_2 + CUP_DE61_CVN2_POS,CUP_CVN2_LEN);
	}

	/*Build message and send to Racal.  */
	memset(&output,0,sizeof(output));
	strcat(output, msgHeader);
	strcat(output, cmdCode);
	//strcat(output, "U");
	strcat(output, auth_tx.CAF01_details.cvk[0].cvkA);
	strcat(output, auth_tx.CAF01_details.cvk[0].cvkB);
	strncat(output, cvn2_value,3);

	strcat(output, auth_tx.TLF01_details.card_num);
	strcat(output, delimiter);
	strcat(output, auth_tx.TLF01_details.exp_date);
	strcat(output, "000");

	//if the output is bigger then 150 do not send it out.
	if (strlen(output) > 150)
	{
		racal_log_message( 2, 2,"CVN2 Cmd Msg too long.","racalVerify_CVN2_for_CUP_Ecom",1 );
		return(TRANSACTION_NOT_VERIFIED);
	}

	memset(BUFFER,0,sizeof(BUFFER));
	sprintf(BUFFER, "BEFORE-PORT: %s", output);
	MYPRINT(BUFFER);

	/*The message is built.  Send it over to the Racal box.*/
	/*Once the a response is recieved, check for the message header,*/
	/*response code, and the error code.  If it does not exactly match,*/
	/*send TRANSACTION_NOT_VERIFIED back.*/
	if (racalSendToPort(output) == TRANSACTION_VERIFIED)
	{
		/*parse the message check for the message header.*/
		memset(&dummy,0,Dummy_Size);
		strncpy(dummy,returnString,4);
		if(strcmp(dummy,msgHeader) != 0)
		{
			racal_log_message( 2, 2,"CVN2 Msg Header mis-match.","racalVerify_CVN2_for_CUP_Ecom",1 );
			return (TRANSACTION_NOT_VERIFIED); /*CVN2 invalid*/
		}

		/*parse the message check for the response code.*/
		memset(&dummy,0,Dummy_Size);
		strncpy(dummy,returnString + 4, 2);
		if(strcmp(dummy,"CZ") !=0)
		{
			racal_log_message( 2, 2,"CVN2 Cmd Msg mis-match.","racalVerify_CVN2_for_CUP_Ecom",1 );
			return (TRANSACTION_NOT_VERIFIED); /*CVN2 invalid*/
		}

		/* Parse the message check for the error code. */
		memset(&dummy,0,Dummy_Size);
		strncpy(dummy,returnString + 6, 2);
		if(strcmp(dummy,"00") == 0)
		{
			return (TRANSACTION_VERIFIED); /*CVN2 valid*/
		}

		if(strcmp(dummy,"01") == 0)
		{
			racal_log_message( 2, 2,"CVN2-VERIFY failure.","racalVerify_CVN2_for_CUP_Ecom",1 );
			return (TRANSACTION_NOT_VERIFIED); /*CVN2 invalid*/
		}

		if(strcmp(dummy,"10") == 0)
		{
			racal_log_message( 2, 2,"CVN2-VERIFY CVK A or B parity error.","racalVerify_CVN2_for_CUP_Ecom",1 );
			return (TRANSACTION_NOT_VERIFIED); /*CVN2 invalid*/
		}

		if(strcmp(dummy,"12") == 0)
		{
			racal_log_message( 2, 2,"CVN2-VERIFY No Key loaded in user storage.","racalVerify_CVN2_for_CUP_Ecom",1 );
			return (TRANSACTION_NOT_VERIFIED); /*CVN2 invalid*/
		}

		if(strcmp(dummy,"13") == 0)
		{
			racal_log_message( 2, 2,"CVN2-VERIFY LMK error report to superviser.","racalVerify_CVN2_for_CUP_Ecom",1 );
			return (TRANSACTION_NOT_VERIFIED); /*CVN2 invalid*/
		}

		if(strcmp(dummy,"15") == 0)
		{
			racal_log_message( 2, 2,"CVN2-VERIFY Error in input data.","racalVerify_CVN2_for_CUP_Ecom",1 );
			return (TRANSACTION_NOT_VERIFIED); /*CVN2 invalid*/
		}

		if(strcmp(dummy,"21") == 0)
		{
			racal_log_message( 2, 2,"CVN2-VERIFY Invalid user storage index.","racalVerify_CVN2_for_CUP_Ecom",1 );
			return (TRANSACTION_NOT_VERIFIED); /*CVN2 invalid*/
		}

	}
	else
	{
		return (TRANSACTION_NOT_VERIFIED); /*CVN2 invalid*/
	}

	return (TRANSACTION_NOT_VERIFIED); /*CVN2 invalid*/

}/*racalVerify_CVN2_for_CUP_Ecom*/



INT parse_field_48( int src_len, char *src, pBYTE ucaf_data )
{
   #define  FIELD48_MAX_LEN  100

   INT  ret_val = 0;
   INT  sub_field_len;
   INT  MainIndex;
   INT  TotalLen;
   CHAR tag[3];
   CHAR len[3];


   TotalLen =  src_len ;

   /* MainIndex starts at 1 to skip over the Transaction Category Code. */
   for( MainIndex=1; MainIndex<TotalLen && MainIndex<FIELD48_MAX_LEN; )
   {
      /* Get subfield tag (name of subfield). */
      memset( tag, 0x00, sizeof( tag ) );
      strncpy( tag, &src[MainIndex], 2 );
      MainIndex += 2;

 //     /* Get length of subfield. */
 //     if ( 0 == strcmp(tag, DE48_TAG43) )
 //     {
 //        /* Subfield 43 is fixed length. */
 //        sub_field_len = DE48_TAG43_LEN;

 //        /* Currently, do not need the data from this subfield. */
 //     }
//      else
 //     {
         /* Other subfields are variable length. */
         memset( len, 0x00, sizeof(len) );
         memcpy( len, &src[MainIndex], 2 );
         sub_field_len = atoi( len );
         MainIndex += 2;

         /* Get the data from these subfields. */
		if ( 0 == strncmp(tag, DE48_TAG43, 2) )
		 {
			memcpy(ucaf_data,&src[MainIndex],sub_field_len);
		 }
         
 //     }
      MainIndex += sub_field_len;
   }
   return( ret_val );
}  /* End parse_field_48 */


/*************************************************************************************
       NAME:           racalVerify_CAVV_for_3Dsecure()

       DESCRIPTION:    This module will deal with the 3D secure CAVV verification.

                       
       INPUTS:
		 	None.

       OUTPUTS:
         None
       RETURNS:        
		   None.                       
                               
      AUTHOR:         Phani  16th AUG 2013
      MODIFIED BY:    
*************************************************************************************/
int racalVerify_CAVV_for_3Dsecure()
{
   char msgHeader[5] = {0};
   char cmdCode[3] = {0};
   char cvv_value[5] = {0};
   char cavv2_atn_Code[5] = {0};
   char delimiter[2] = {0};
   char output[200]= {0};
   char cardType = 'Z';
   char csc[6] = {0};

   INT de48_retcode= 0;
   INT ucaf_data_64_len =0;
   BYTE HEX_ATN_value[9]={0};
   BYTE UCAF_data_ascii[41]={0},UCAF_data_bin[41]={0};
   INT ucaf_data_len=0,dcp_len = 0;
   int  cnt=0,result=0,jcnt=0;
   char BUFFER [256] = {0};
   BYTE temp_len[4] = {0};
   CHAR *tempptr,fieldSeperator[2] = {0};
   BYTE UCAF_data[33]={0};	
   int len = 0;
   int atn_int = 0,astr_len = 0;
   BYTE ascii_atn[9]={0};
   memset (&msgHeader,0,sizeof(msgHeader));
   memset (&cmdCode,0,sizeof(cmdCode));
   memset (&delimiter,0,sizeof(delimiter));
   memset (&output,0,sizeof(output));
   memset (&returnString,0,ReturnString_Size);
   memset (&csc,0,sizeof(csc));
   /* For DCP UCAF data */
  #define DCP_UCAF_LEN_POS  51
  #define DCP_UCAF_DATA_POS 53
   /* ENDS */
   strcpy (msgHeader ,"1234");

   if (strcmp(auth_tx.BIN01_details.card_family,CARD_VISA) == 0)
      cardType = 'V';

   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_MASTERCARD) == 0)
      cardType = 'M';

   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_AMEX) == 0)
      cardType = 'A';

   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_JCB) == 0)
      cardType = 'J';
   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_CUP) == 0)
        cardType = 'C';
   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_DINERS) == 0)
           cardType = 'D';

   switch(cardType)
   {
      case 'A':

 			 racal_log_message( 2, 2,"AMEX card - 3D secure code not implemented.","racalVerify_CAVV_for_3Dsecure",1 );
			 return(TRANSACTION_NOT_VERIFIED);

         /****************************end AMEX***************************************/

         break;

      case 'J':
         /*drop down to VISA for this function.*/
         /*break;*/

      case 'M':

         /*drop down to VISA for this function.*/
         /*break;*/
      case 'C':

		 /*drop down to VISA for this function.*/
		 /*break;*/
      case 'D':

		 /*drop down to VISA for this function.*/
		 /*break;*/
      case 'V':

         /*****************************begin Visa***************************************/
         /*The VISA, MC, and the JCB are all the same.*/
         /*The rest of this code would deal with them.*/
         strcpy (cmdCode,"CY"); 
         strcpy (delimiter,";");
         
         /*edit CVK A */
         if(strlen(auth_tx.CAF01_details.cvk[4].cvkA) == 0)
		 {
            racal_log_message( 2, 2,"CAVV-validation: CAK is blank.","racalVerify_CAVV_for_3Dsecure",1 );
            return(TRANSACTION_NOT_VERIFIED);
         }
         /*edit CVK B */
         if(strlen(auth_tx.CAF01_details.cvk[4].cvkB) == 0)
         {
            racal_log_message( 2, 2,"CAVV-validation: CAK is blank.","racalVerify_CAVV_for_3Dsecure",1 );
            return(TRANSACTION_NOT_VERIFIED);
         }

		 /*edit card number*/
	 	 if(strlen(auth_tx.TLF01_details.card_num) == 0 )
         {
            racal_log_message( 2, 2,"CAVV-validation: card_num is blank.","racalVerify_CAVV_for_3Dsecure",1 );
            return(TRANSACTION_NOT_VERIFIED);
         }

		  /*Get DE126.9 CAVV from Message*/
        if(cardType == 'V' || cardType == 'J')
		{	
			if(strlen(auth_tx.TLF01_details.dcf01_retired_cwk) != 0  && strcmp(auth_tx.TLF01_details.nfi_end_date,"1" == 0))	
			 {
				 strncpy(cvv_value,&auth_tx.TLF01_details.dcf01_retired_cwk[6],4);
				 strncpy(cavv2_atn_Code,&auth_tx.TLF01_details.dcf01_retired_cwk[10],4);
				 if(cardType == 'J')
				 {
					 /* From JCB CAVV value will be sent 40, which is assumend as spaces,
					  * hence check the raw data which stored in auth_tx.acct_id_2 without converting.
					  * position 3 and 4 contains CVV verifiation value(4 bytes after convertion)
					  * position 5 and 6 contains unpredictable number(4 bytes after convertion)
					  * 40 binary value is viewd as @, hence compare with it to send spaces.
					  * */
					 if(0 == strncmp(&auth_tx.acct_id_2[3],"@@",2))
					 {
						 memcpy(cvv_value,"    ",4);
					 }
					 if(0 == strncmp(&auth_tx.acct_id_2[5],"@@",2))
					 {
						 memcpy(cavv2_atn_Code,"    ",4);
					 }
				 }
			 }		 /*If neither cvc1 or cvc2 is present, return no value*/
			 else if(strlen(auth_tx.TLF01_details.dcf01_retired_cwk) == 0)
			 {
				racal_log_message( 2, 2,"CAVV-validation: CAVV data 126.9 is blank.","racalVerify_CAVV_for_3Dsecure",1 );
				return(TRANSACTION_NOT_VERIFIED);
			 }
		}
        else if(cardType == 'M')
		{
		   
			if (strncmp(auth_tx.TLF01_details.handler_queue,"dcp",3) == 0)
			{
				/* 3D secure data structre of AsiaPay transaction:
				 *
				 * Total length : 2 bytes : N 4
				 * Actual data length: 2 bytes : N 4
				 * Table ID (MC ECOM indicator value): 2 bytes : N 4
				 * MC merchant/ CH certificate serial number :  2 bytes length (N 4)+ 40 bytes (N 80)
				 * MC security level indicator : 2 bytes length (N 4)+ 7 bytes (N 14)
				 * MC UCAF data length : 2 bytes length (N 4)+ 28 bytes (N 56)
				 *
				 * DCPISO will send data in auth_tx.int_net_facility_data, Data after table ID will be
				 * received by racal.
				 *
				 * After the conversion  UCAF data will be populated as follows for both
				 * incoming as well as Asiapay
				 *
				 * Total 40 Bytes:
				 *	Bit 1 & 2   cByte 8C = Auth Success 86 = Auth Attempt
				 *	Bit 3 - 18  Merchant Name First 16 digits Hexa format of SHA Hased Merchant Name
				 *	Bit 19 & 20 08 for ACS
				 *	Bit 21 - 22 Authentication Method
				 * 		Indates how the cardholder was authenticated to the ACS;
				 *  	0 = No cardholder authentication performed (only valid for AAV with X'86' - Attempts Processin
				 *  	1 = Password
				 *  	2 = Secret Key (e.g. Chip Card) 3 = PKI (pending further discussions)
				 *  	01 = Auth Attempt
				 *  	11 = Others
				 *  Bit 23 - 30 ATN  Last 8 digits of AcctID
				 *  Bit 31 - 34 CVV for Authentication Leading zero filled CVV
				 *  Bit 35 - 40 Filler Zero fill
				 * */
				if(strlen(auth_tx.int_net_facility_len) !=0)
				 {
					dcp_len = genutil_bcd_to_int (&auth_tx.int_net_facility_data[DCP_UCAF_LEN_POS], 2);
					memcpy(UCAF_data,&auth_tx.int_net_facility_data[DCP_UCAF_DATA_POS],dcp_len);
					ucaf_data_64_len =strlen(UCAF_data);
					ucaf_data_len = genutil_base64_to_binary( UCAF_data, strlen(UCAF_data),&UCAF_data_bin );
					genutil_bin_to_ascii (UCAF_data_bin, ucaf_data_len, UCAF_data_ascii, astr_len);
					printf("UCAF dat: %s, len %d \n",UCAF_data_ascii,astr_len);
					strncpy(cvv_value,&UCAF_data_ascii[30],4);
					strncpy(HEX_ATN_value,&UCAF_data_ascii[22],8);

					atn_int =  genutil_hex_to_int(HEX_ATN_value,8);
					sprintf(ascii_atn,"%010u",atn_int);
					strncpy(cavv2_atn_Code,&ascii_atn[6],4);
				 }		 /*If neither cvc1 or cvc2 is present, return no value*/
				 else if(strlen(auth_tx.int_net_facility_len) == 0)
				 {
					racal_log_message( 2, 2,"CAVV-validation: CAVV data is blank.","racalVerify_CAVV_for_3Dsecure",1 );
					return(TRANSACTION_NOT_VERIFIED);
				 }
			}
			else
			{
				if(strlen(auth_tx.EMV_details.issuer_script_2) !=0)
				 {
					de48_retcode = parse_field_48( strlen(auth_tx.EMV_details.issuer_script_2),&auth_tx.EMV_details.issuer_script_2,&UCAF_data );
					ucaf_data_64_len =strlen(UCAF_data);
					ucaf_data_len = genutil_base64_to_binary( UCAF_data, strlen(UCAF_data),&UCAF_data_bin );
					genutil_bin_to_ascii (UCAF_data_bin, ucaf_data_len, UCAF_data_ascii, astr_len);
					printf("UCAF dat: %s, len %d \n",UCAF_data_ascii,astr_len);
					strncpy(cvv_value,&UCAF_data_ascii[30],4);
					strncpy(HEX_ATN_value,&UCAF_data_ascii[22],8);

					atn_int =  genutil_hex_to_int(HEX_ATN_value,8);
					sprintf(ascii_atn,"%010u",atn_int);
					strncpy(cavv2_atn_Code,&ascii_atn[6],4);
				 }		 /*If neither cvc1 or cvc2 is present, return no value*/
				 else if(strlen(auth_tx.EMV_details.issuer_script_2) == 0)
				 {
					racal_log_message( 2, 2,"CAVV-validation: CAVV data is blank.","racalVerify_CAVV_for_3Dsecure",1 );
					return(TRANSACTION_NOT_VERIFIED);
				 }
			}

			
		}
        else if(cardType == 'D')
	    {
        	if(0 == strncmp(auth_tx.TLF01_details.handler_queue,
        					"dcp",3))
        	{
        		//Asiapay 3D validation.
        		strncpy(cvv_value,
        				auth_tx.TLF01_details.cvc_data,
        				DINERS_DE122_CAVV_LEN);
				strncpy(cavv2_atn_Code,
						auth_tx.TLF01_details.exp_date,
						DINERS_DE14_SIZE);

        	}
        	else if(0 == strncmp(auth_tx.TLF01_details.handler_queue,
        			              "ncdci",5))
        	{
        		if(strlen(auth_tx.future_use_4)> DINERS_DE122_AUTH_TRACKING_NUM_POS)
        		{
				 // Lets retrive CVVV value present in DE122
					strncpy(cvv_value,
							auth_tx.future_use_4 + DINERS_DE122_CAVV_POS,
							DINERS_DE122_CAVV_LEN);
					strncpy(cavv2_atn_Code,
							auth_tx.future_use_4 + DINERS_DE122_UNPRIDICATABLE_NUM_POS,
							DINERS_DE122_UNPRIDICATABLE_NUM_LEN);
				}
        	}
	   }
		
		 /*Build message and send to Racal.  */
		 memset (&output,0,sizeof(output));
		 strcat(output, msgHeader);
		 strcat(output, cmdCode);
		 if( 'D' ==  cardType &&
			(auth_tx.CAF01_details.aba_nbr[1] == '1' || auth_tx.CAF01_details.aba_nbr[1] == 1))
		 { /* ticked: single length key validation*/
			 // Don't send U for single length validation
		 }
		 else
		 {/* un-ticked: double length key validation*/
		 strcat(output, "U");
		 }
		 strcat(output, auth_tx.CAF01_details.cvk[4].cvkA);
		 strcat(output, auth_tx.CAF01_details.cvk[4].cvkB);

		 if('D' ==  cardType)
		 {
			 if(0 == strncmp(auth_tx.TLF01_details.handler_queue,
 					"dcp",3))
			 {
				 strncat(output, &cvv_value[1],3);
			 }
			 else
			 {
				 strncat(output, cvv_value,3);
			 }
		 }
		 else
		 {
		 strncat(output, &cvv_value[1],3);
		 }

		 strcat(output, auth_tx.TLF01_details.card_num);
		 strcat(output, delimiter);
		 strcat(output, cavv2_atn_Code);

		 if (strcmp(auth_tx.BIN01_details.card_family,CARD_MASTERCARD) == 0)
		 {
			 strcat(output, "140");
		 }
		 else if(strcmp(auth_tx.BIN01_details.card_family,CARD_JCB) == 0)
		 {
			 if(0 == strncmp(auth_tx.TLF01_details.dcf01_retired_cwk,"07",2))
			 {
				 strcat(output, "700");
			 }
			 else
			 {
				 strcat(output, "000");
			 }

		 }
		 else
		 {
			 strcat(output, "000");
		 }


	 //if the output is bigger then 150 do not send it out.
	 if (strlen(output) > 150)
	 {
		racal_log_message( 2, 2,"CAVV Cmd Msg too long.","racalVerify_CAVV_for_3Dsecure",1 );
		return(TRANSACTION_NOT_VERIFIED);
	 }

	 memset(BUFFER,0,sizeof(BUFFER));
	 sprintf(BUFFER, "BEFORE-PORT: %s", output);
	 MYPRINT(BUFFER);

	 /*The message is built.  Send it over to the Racal box.*/
	 /*Once the a response is recieved, check for the message header,*/
	 /*response code, and the error code.  If it does not exactly match,*/
	 /*send TRANSACTION_NOT_VERIFIED back.*/
	 if (racalSendToPort(output) == TRANSACTION_VERIFIED)
	 {
		/*parse the message check for the message header.*/
		memset(&dummy,0,Dummy_Size);
		strncpy(dummy,returnString,4);
		if(strcmp(dummy,msgHeader) != 0)
	{
		   racal_log_message( 2, 2,"CAVV Msg Header mis-match.","racalVerify_CAVV_for_3Dsecure",1 );
		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
	}

		/*parse the message check for the response code.*/
		memset(&dummy,0,Dummy_Size);
		strncpy(dummy,returnString + 4, 2);
		if(strcmp(dummy,"CZ") !=0)
		{
		   racal_log_message( 2, 2,"CAVV Cmd Msg mis-match.","racalVerify_CAVV_for_3Dsecure",1 );
		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
		}

	/* Parse the message check for the error code. */
	memset(&dummy,0,Dummy_Size);
	strncpy(dummy,returnString + 6, 2);
	if(strcmp(dummy,"00") == 0)
	{
		  return (TRANSACTION_VERIFIED); /*cavv valid*/
	}

		if(strcmp(dummy,"01") == 0)
		{
		   racal_log_message( 2, 2,"CAVV-VERIFY failure.","racalVerify_CAVV_for_3Dsecure",1 );
		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
		}
		if(strcmp(dummy,"10") == 0)
		{
			racal_log_message( 2, 2,"CAVV-VERIFY CVK A or B parity error.","racalVerify_CAVV_for_3Dsecure",1 );
			return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
		}

		if(strcmp(dummy,"12") == 0)
		{
		   racal_log_message( 2, 2,"CAVV-VERIFY No Key loaded in user storage.","racalVerify_CAVV_for_3Dsecure",1 );
		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
		}

		if(strcmp(dummy,"13") == 0)
		{
		   racal_log_message( 2, 2,"CAVV-VERIFY LMK error report to superviser.","racalVerify_CAVV_for_3Dsecure",1 );
		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
		}

		if(strcmp(dummy,"15") == 0)
		{
		   racal_log_message( 2, 2,"CAVV-VERIFY Error in input data.","racalVerify_CAVV_for_3Dsecure",1 );
		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
		}

		if(strcmp(dummy,"21") == 0)
		{
		   racal_log_message( 2, 2,"CAVV-VERIFY Invalid user storage index.","racalVerify_CAVV_for_3Dsecure",1 );
		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
		}

	 }
	 else  /* else for: if (racalSendToPort(output) == TRANSACTION_VERIFIED)*/
	 {
			   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
	 }



 return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
 break;
		         /********************end of Visa Case*****************************************/

   
   default:
	   {
		   racal_log_message( 2, 2,"Card Type Undefined.","racalVerify_CAVV_for_3Dsecure",1 );
		   return(TRANSACTION_NOT_VERIFIED);
	   }


   }/*end of switch statement.*/

}/*racalEquitVerifyCVV_CVC_CSC_CAV*/

long long Hex_to_Dec(char * buf_ptr)
{

    CHAR hex[ARRAY_SIZE];
    long long decimal = 0, base = 1;
    INT i = 0, value, length;
    
    strncpy(hex, buf_ptr, strlen(buf_ptr)); 
    length = strlen(hex);
    for(i = length--; i >= 0; i--)
    {
        if(hex[i] >= '0' && hex[i] <= '9')
        {
            decimal += (hex[i] - 48) * base;
            base *= 16;
        }
        else if(hex[i] >= 'A' && hex[i] <= 'F')
        {
            decimal += (hex[i] - 55) * base;
            base *= 16;
        }
        else if(hex[i] >= 'a' && hex[i] <= 'f')
        {
            decimal += (hex[i] - 87) * base;
            base *= 16;
        }
    }
    return (decimal);
}

void racal_Get_Sha256_Encryption(char * Suppl_Amt)
{
	char Input_Sha256_Buffer[50] ={0};
	int len=0;
	char HexBuffer[500]={0};
	unsigned char Output_Sha256_Buffer[100]={0};
	char strTemp[512] = {0};

	int i;
	char strSystemDate[9] = {0};
	long juldate_today = 0;
	char julian_year[3]={0};
	char strSystemDateTime[50] = {0};
	char juldate_today_str[50]={0};
	char year[5]={0};
	char month[3]={0};
	char day[3]={0};
	int iyear=0,yyyy=0,mm=0,dd=0,JulianDate=0;
	char julday_req[5]={0};
	char Supplemental_data_out[65]={0};
	BYTE hex_txn_amt[20] = {0};
	BYTE ascii_txn_amt[20] = {0};
	long long dec_txn_amt = 0;
	int Date_compare=0;
	INT sysJD=0, tranJD=0;
	char tranYY[5]= {0};

	/*char Supplemental_output[40]={0};*/

	/*Prepare the Input to the Sha256 encryption
	*1.15 digits of card num+non zero digit of the eci value appended.
	* (126.9 subfield 6( supplement data)
	*2.purchase amount 12 bytes
	*3.puchase currency code
	*4.Authentication date Julian date(yyddd) 11 november 2021( 21315)
	*/

	genutil_format_date (strSystemDateTime) ;

	strncpy(strSystemDate,strSystemDateTime,8);
	strncpy(year,strSystemDate,4);
	strncpy(month,strSystemDate+4,2);
	strncpy(day,strSystemDate+6,2);
	yyyy=atoi(year);
	mm=atoi(month);
	dd=atoi(day);

	JulianDate=calculate_julian_date(mm,dd,yyyy);
	strncpy(julday_req,&auth_tx.TLF01_details.dcf01_retired_cwk[27],3);

	sprintf(juldate_today_str,"%03d",JulianDate);

    /*Date_compare=strncmp(juldate_today_str,julday_req,3);

	if(Date_compare>=0)
	{

	  strncpy(julian_year,year+2,2);

	}
	else
	{
		strncpy(julian_year,year+2,2);
		iyear=atoi(julian_year)-1;
		sprintf(julian_year,"%d",iyear);

	}*/

	sysJD = JulianDate;
	tranJD = atoi(julday_req);

	iyear = getYear_from_JulianDate(sysJD, tranJD, yyyy);
	sprintf(tranYY,"%d",iyear);
	strncpy(julian_year, &tranYY[2], 2);

	strncpy(Input_Sha256_Buffer,auth_tx.TLF01_details.card_num,15);
	strncat(Input_Sha256_Buffer,auth_tx.TLF01_details.profile+1,1);
	//strcat(Input_Sha256_Buffer,"00");

	strncpy(hex_txn_amt,&auth_tx.TLF01_details.dcf01_retired_cwk[14],10);
	dec_txn_amt =  Hex_to_Dec(hex_txn_amt);
	sprintf(ascii_txn_amt,"%012lld",dec_txn_amt);

	strncat(Input_Sha256_Buffer,&ascii_txn_amt,12);
	strncat(Input_Sha256_Buffer,&auth_tx.TLF01_details.dcf01_retired_cwk[24],3);
	strncat(Input_Sha256_Buffer,julian_year,2);
	//strncat(Input_Sha256_Buffer,"21",2);
	strncat(Input_Sha256_Buffer,&auth_tx.TLF01_details.dcf01_retired_cwk[27],3);
	//strncat(Input_Sha256_Buffer,juldate_today_str,3);
	
	/*memset(Input_Sha256_Buffer, 0x00, sizeof(Input_Sha256_Buffer));
	strcpy(Input_Sha256_Buffer, "464995000108795500000007586184021309");*/

	sprintf(strTemp, "input to SHA256 CW: %s", Input_Sha256_Buffer);
	racal_log_message( 2, 2,strTemp,"racalVerify_CAVV_for_3Dsecure",1 );

	len= strlen(Input_Sha256_Buffer);
	char * hash= sha256(Input_Sha256_Buffer, len, Output_Sha256_Buffer);


	 for (i = 0; i<SHA256_LEN; i++)
	 {
		 byte2hexchar(&HexBuffer[2*i], &hash[i], 1);
	 }

	 GetOnlyNumber(HexBuffer,Supplemental_data_out);
	 strncpy(Suppl_Amt,Supplemental_data_out,16);

	 if (strlen(Suppl_Amt) < 16)
	 {
		 racal_log_message( 2, 2,"unable to extract 16 digit hash number","racalVerify_CAVV_for_3Dsecure",1 );
	 }

	 sprintf(strTemp, "output of SHA256 CW: %s", Suppl_Amt);
	 racal_log_message( 2, 2,strTemp,"racalVerify_CAVV_for_3Dsecure",1 );

	 /*strncat(Suppl_Amt,&auth_tx.TLF01_details.dcf01_retired_cwk[10],4);
	 strncat(Suppl_Amt,&auth_tx.TLF01_details.dcf01_retired_cwk[1],1);
	 strncat(Suppl_Amt,&auth_tx.TLF01_details.dcf01_retired_cwk[2],2);
	 strcat(Suppl_Amt,PADDED_ZEROES);*/



}
int racalVerify_CAVV_for_3Dsecure_VISA()
{
   char msgHeader[5] = {0};
   char cmdCode[3] = {0};
   char cvv_value[5] = {0};
   char cavv2_atn_Code[5] = {0};
   char delimiter[2] = {0};
   char output[200]= {0};
   char cardType = 'Z';
   char csc[6] = {0};
   char Suppl_Amt[50] = {0};
   char strTemp[512] = {0}; 

   INT de48_retcode= 0;
   INT ucaf_data_64_len =0;
   BYTE HEX_ATN_value[9]={0};
   BYTE UCAF_data_ascii[41]={0},UCAF_data_bin[41]={0};
   INT ucaf_data_len=0,dcp_len = 0;
   int  cnt=0,result=0,jcnt=0;
   char BUFFER [256] = {0};
   BYTE temp_len[4] = {0};
   CHAR *tempptr,fieldSeperator[2] = {0};
   BYTE UCAF_data[33]={0};
   int len = 0;
   int atn_int = 0,astr_len = 0;
   BYTE ascii_atn[9]={0};
   BYTE HMAC_key[33];
   CHAR hash_identier[4] = {0};
   CHAR hmac_length[10] = {0};
   CHAR hmac_key_len[4] = {0};
   memset (&msgHeader,0,sizeof(msgHeader));
   memset (&cmdCode,0,sizeof(cmdCode));
   memset (&delimiter,0,sizeof(delimiter));
   memset (&output,0,sizeof(output));
   memset (&returnString,0,ReturnString_Size);
   memset (&csc,0,sizeof(csc));
   /* For DCP UCAF data */
  #define DCP_UCAF_LEN_POS  51
  #define DCP_UCAF_DATA_POS 53
   /* ENDS */
   strcpy (msgHeader ,"1234");

   strcpy (cmdCode,"CW");
   //strcpy(hash_identier,"06");
   //strncpy(hmac_length, "0064",4);
   	 strcpy (delimiter,";");
   	 strncpy(hmac_key_len,"32",2);
	 strcpy(HMAC_key, auth_tx.CAF01_details.csck);
	 strcpy (delimiter,";");
	 racal_Get_Sha256_Encryption( &Suppl_Amt);

	 if(strlen(auth_tx.TLF01_details.card_num) >= 14 )
	 {
		 memset(&dummy,0,Dummy_Size);
		 strncpy(dummy,auth_tx.TLF01_details.card_num,16);
	 }

	 if(strlen(auth_tx.TLF01_details.dcf01_retired_cwk) != 0  && strcmp(auth_tx.TLF01_details.nfi_end_date,"1" == 0))
	 {
	 		strncpy(cavv2_atn_Code,&auth_tx.TLF01_details.dcf01_retired_cwk[10],4);
	 		strncpy(cvv_value,&auth_tx.TLF01_details.dcf01_retired_cwk[0],4);
	 }
	 /*Build message and send to Racal.  */
			 memset (&output,0,sizeof(output));
			 strcat(output, msgHeader);
			 strcat(output, cmdCode);
			 strcat(output, "U");
			 strcat(output, HMAC_key);
			 strcat(output, Suppl_Amt);
			 //strcat(output, auth_tx.TLF01_details.card_num);
			 strcat(output, delimiter);
			 strncat(output, cavv2_atn_Code, 4);
			 strncat(output, &cvv_value[1], 3);
			 //strcat(output, "%");
			 //strcat(output, "02");
			 /*strcat(output, "19");*/


			 if (strlen(output) > 150)
			 {
				racal_log_message( 2, 2,"CAVV Cmd Msg too long.","racalVerify_CAVV_for_3Dsecure",1 );
				return(TRANSACTION_NOT_VERIFIED);
			 }
			 /*printf("\n racal input for 10.6 CW: %s \n", output);*/
			 sprintf(strTemp, " input of racal CW: %s", output);
			 racal_log_message( 2, 2,strTemp,"racalVerify_CAVV_for_3Dsecure",1 );
			 if (racalSendToPort(output) == TRANSACTION_VERIFIED)
			 {
			 sprintf(strTemp, " output of racal cmnd CW: %s", returnString);
			 racal_log_message( 2, 2,strTemp,"racalVerify_CAVV_for_3Dsecure",1 );
			 		/*parse the message check for the message header.*/
			 		memset(&dummy,0,Dummy_Size);
			 		strncpy(dummy,returnString,4);
			 		if(strcmp(dummy,msgHeader) != 0)
			 		{
			 		   racal_log_message( 2, 2,"CAVV Msg Header mis-match.","racalVerify_CAVV_for_3Dsecure",1 );
			 		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
			 		}

			 		/*parse the message check for the response code.*/
			 		memset(&dummy,0,Dummy_Size);
			 		strncpy(dummy,returnString + 4, 2);
			 		if(strcmp(dummy,"CX") !=0)
			 		{
			 		   racal_log_message( 2, 2,"CAVV Cmd Msg mis-match.","racalVerify_CAVV_for_3Dsecure",1 );
			 		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
			 		}

			 		/* Parse the message check for the error code. */
			 		memset(&dummy,0,Dummy_Size);
			 		strncpy(dummy,returnString + 6, 2);
			 		if(strcmp(dummy,"00") == 0)
			 		{
			 		  /* copy Cvv to check at txcntl */
			 		 strncpy(auth_tx.conv_date, returnString + 8, 3);
			 		  return (TRANSACTION_VERIFIED); /*cavv valid*/
			 		}

			 		if(strcmp(dummy,"01") == 0)
			 		{
			 		   racal_log_message( 2, 2,"CAVV-VERIFY failure.","racalVerify_CAVV_for_3Dsecure",1 );
			 		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
			 		}
			 		if(strcmp(dummy,"10") == 0)
			 		{
			 			racal_log_message( 2, 2,"CAVV-VERIFY CVK A or B parity error.","racalVerify_CAVV_for_3Dsecure",1 );
			 			return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
			 		}

			 		if(strcmp(dummy,"12") == 0)
			 		{
			 		   racal_log_message( 2, 2,"CAVV-VERIFY No Key loaded in user storage.","racalVerify_CAVV_for_3Dsecure",1 );
			 		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
			 		}

			 		if(strcmp(dummy,"13") == 0)
			 		{
			 		   racal_log_message( 2, 2,"CAVV-VERIFY LMK error report to superviser.","racalVerify_CAVV_for_3Dsecure",1 );
			 		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
			 		}

			 		if(strcmp(dummy,"15") == 0)
			 		{
			 		   racal_log_message( 2, 2,"CAVV-VERIFY Error in input data.","racalVerify_CAVV_for_3Dsecure",1 );
			 		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
			 		}

			 		if(strcmp(dummy,"21") == 0)
			 		{
			 		   racal_log_message( 2, 2,"CAVV-VERIFY Invalid user storage index.","racalVerify_CAVV_for_3Dsecure",1 );
			 		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
			 		}

			 }
			 else  /* else for: if (racalSendToPort(output) == TRANSACTION_VERIFIED)*/
			 {
			 			   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
			 }

			 return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/

}/*racalVerify_CAVV_for_3Dsecure_VISA*/

void Racal_RemoveHyphes(char *input, char *output)
{
	int i=0,j=0;

	for( i=0;i<strlen(input);i++)
	{
		if(input[i]!='-')
		{
			output[j]=input[i];
			j++;
		}
	}
	output[i]='\0';
}
void Racal_CUP_Input(char * Supplemental_output)
{

	long double decnum=0.00f;
			long double decnum_parta=0.00f;
			long double rem= 0.00f;
		    long double total = 0.00f;

		    int i=0;
			int len=0;
			int length=0;
			int Length_of_string=0;
			int last_four_digit=0;

		    char hexnum[30] = {0};
		    char hexnum_parta[10] ={0};
		    char string[25] = {0};
		    char string_parta[25] = {0};
		    char string_original[25] ={0};
			char string_original_new[25];
			char string_last_four_bytes[5] = {0};
		    char string_parta_original[25] = {0};
			char string_last_four_bytes_parta[5] = {0};
			char Derived_DS_Transaction_ID_Value[5] = {0};
			char Transaction_ID_input[37]={0};
			char Transaction_ID[37]={0};
			char input_desEncryption[50]={0};
			char strTemp[512]={0};

		    /*printf("Enter any Hexadecimal Number: ");
		    scanf("%s", hexnum);*/
			strncpy(Transaction_ID_input,auth_tx.TLF01_details.product_codes[13].amount,12);
			strncat(Transaction_ID_input,auth_tx.TLF01_details.product_codes[18].amount,12);
			strncat(Transaction_ID_input,auth_tx.TLF01_details.product_codes[19].amount,12);
			Racal_RemoveHyphes(Transaction_ID_input,Transaction_ID);
			strncpy(hexnum,Transaction_ID+12,20);
		    while(hexnum[i]!='\0')
		    {
		        len++;
		        i++;
		    }
		    length = len-8;

		    printf("Entered Hexadecimal Number[%s]: ",hexnum);
		    strncpy(hexnum_parta,hexnum+length,8);
		    memcpy(hexnum+length,"00000000",8);

		    len--;
		    i=0;
		    printf("Length[%d]",len);
		    while(len>=0)
		    {
		        rem = hexnum[len];
		        if(rem>=48 && rem<=57)
		            rem = rem-48;
		        else if(rem>=65 && rem<=70)
		            rem = rem-55;
		        else if(rem>=97 && rem<=102)
		            rem = rem-87;
		        else
		        {
		            printf("\nYou've entered an invalid Hexadecimal digit");
		            return 0;
		        }
		        decnum = decnum + (rem*pow(16.0, i));
		        len--;
		        i++;
		    }
		        printf("\nEquivalent Decimal Value = %Lf", decnum);

		   length = 8 ;
		   length--;
		   i=0;

		   while(length>=0)
		    {
		        rem = hexnum_parta[length];
		        if(rem>=48 && rem<=57)
		            rem = rem-48;
		        else if(rem>=65 && rem<=70)
		            rem = rem-55;
		        else if(rem>=97 && rem<=102)
		            rem = rem-87;
		        else
		        {
		            printf("\nYou've entered an invalid Hexadecimal digit");
		            return 0;
		        }
		        decnum_parta = decnum_parta + (rem*pow(16, i));
		        length--;
		        i++;
		    }
		        printf("\nEquivalent Decimal Value = %Lf", decnum_parta);

		    sprintf(string,"%Lf",decnum);
		    sprintf(string_parta,"%Lf",decnum_parta);

		    for(int j=0;j<strlen(string);j++)
		    {
		       if(string[j] == '.')
		           {
		                        break;
		           }
		           else
		           {
		                strncpy(string_original+j,string+j,1);
		           }
		    }
		   // printf("\nstring_original[%s]",string_original);

		   for(int j=0;j<strlen(string_parta);j++)
		    {
		       if(string_parta[j] == '.')
		           {
		                        break;
		           }
		       else
		           {
		                        strncpy(string_parta_original+j,string_parta+j,1);
		           }
		    }
		   //printf("\nstring_parta[%s]",string_parta_original);
		   if(strlen(string_original) > 20 )
		   {
		     Length_of_string = strlen(string_original);
		     memcpy(string_original_new,string_original+(Length_of_string -20),20);
		     memcpy(string_last_four_bytes,string_original_new+(strlen(string_original_new) - 4 ),4);
		     memcpy(string_last_four_bytes_parta,string_parta_original+(strlen(string_parta_original) - 4 ),4);
		     last_four_digit = (atoi(string_last_four_bytes) + atoi(string_last_four_bytes_parta));
		     if(last_four_digit > 9999)
		         {
		                last_four_digit = last_four_digit - 10000;
		         }
		     sprintf(Derived_DS_Transaction_ID_Value,"%04d", last_four_digit);
		     printf("\nLast Four Digit Value[%s]", Derived_DS_Transaction_ID_Value);
		     sprintf(strTemp, "Derived_DS_Transaction_ID_Value: %s", Derived_DS_Transaction_ID_Value);
		     racal_log_message( 2, 2,strTemp,"Racal_CUP_Input",1 );

		   }


		   if(Derived_DS_Transaction_ID_Value[0]=='\0')
		   {

			   strncpy(Supplemental_output,auth_tx.TLF01_details.exp_date,4);
		   }
		   else
		   {
			   strncpy(Supplemental_output,Derived_DS_Transaction_ID_Value,4);
		   }
		   strncat(Supplemental_output,auth_tx.TLF01_details.product_codes[1].amount+1,1);
		   strncat(Supplemental_output,"0",1);
		   strncat(Supplemental_output,auth_tx.TLF01_details.product_codes[1].amount+3,1);

		   sprintf(strTemp, "Supplemental output value: %s", Supplemental_output);
		   racal_log_message( 2, 2,strTemp,"Racal_CUP_Input",1 );


}

int racalVerify_CAVV_for_3Dsecure_CUP()
{
   char msgHeader[5] = {0};
   char Command[3] = {0};
   char cvv_value[5] = {0};
   char cavv2_atn_Code[5] = {0};
   char Delimiter[2] = {0};
   char output[200]= {0};
   char cardType = 'Z';
   char csc[6] = {0};
   char Supplement_Data[50] = {0};
   char strTemp[512] = {0};
   BYTE CAVV_key[33];


   memset (&msgHeader,0,sizeof(msgHeader));
   memset (&Command,0,sizeof(Command));
   memset (&Delimiter,0,sizeof(Delimiter));
   memset (&output,0,sizeof(output));
   memset (&returnString,0,ReturnString_Size);
   memset (&csc,0,sizeof(csc));

   strcpy (msgHeader ,"1234");

   strcpy (Command,"CW");

   	 strcpy (Delimiter,";");

   	 if(strlen(auth_tx.CAF01_details.csck) != 0)
	 {
		 strcpy(CAVV_key, auth_tx.CAF01_details.csck);
	 }
	 else
	 {
		racal_log_message( 2, 2, "JCB 3DS2 HSM key is blank.","racalVerify_CAVV_for_3Dsecure_JCB",1 );
		return(TRANSACTION_NOT_VERIFIED);
	 }

	 strcpy (Delimiter,";");
	 Racal_CUP_Input( &Supplement_Data);


	 /*Build message and send to Racal.  */
			 memset (&output,0,sizeof(output));
			 strcat(output, msgHeader);
			 strcat(output, Command);
			 strcat(output, "U");
			 strcat(output, CAVV_key);
		     if (strlen(auth_tx.TLF01_details.card_num) != 0)
			 {
				strcat(output,auth_tx.TLF01_details.card_num);
			 }
			 strcat(output, Delimiter);

			 strcat(output, Supplement_Data);



			 if (strlen(output) > 150)
			 {
				racal_log_message( 2, 2,"CAVV Cmd Msg too long.","racalVerify_CAVV_for_3Dsecure",1 );
				return(TRANSACTION_NOT_VERIFIED);
			 }
			 /*printf("\n racal input for 10.6 CW: %s \n", output);*/
			 sprintf(strTemp, " input of racal CW: %s", output);
			 racal_log_message( 2, 2,strTemp,"racalVerify_CAVV_for_3Dsecure",1 );
			 if (racalSendToPort(output) == TRANSACTION_VERIFIED)
			 {
			 sprintf(strTemp, " output of racal cmnd CW: %s", returnString);
			 racal_log_message( 2, 2,strTemp,"racalVerify_CAVV_for_3Dsecure",1 );
			 		/*parse the message check for the message header.*/
			 		memset(&dummy,0,Dummy_Size);
			 		strncpy(dummy,returnString,4);
			 		if(strcmp(dummy,msgHeader) != 0)
			 		{
			 		   racal_log_message( 2, 2,"CAVV Msg Header mis-match.","racalVerify_CAVV_for_3Dsecure",1 );
			 		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
			 		}

			 		/*parse the message check for the response code.*/
			 		memset(&dummy,0,Dummy_Size);
			 		strncpy(dummy,returnString + 4, 2);
			 		if(strcmp(dummy,"CX") !=0)
			 		{
			 		   racal_log_message( 2, 2,"CAVV Cmd Msg mis-match.","racalVerify_CAVV_for_3Dsecure",1 );
			 		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
			 		}

			 		/* Parse the message check for the error code. */
			 		memset(&dummy,0,Dummy_Size);
			 		strncpy(dummy,returnString + 6, 2);
			 		if(strcmp(dummy,"00") == 0)
			 		{
			 		  /* copy Cvv to check at txcntl */
			 		 strncpy(auth_tx.conv_date, returnString + 8, 3);
			 		  return (TRANSACTION_VERIFIED); /*cavv valid*/
			 		}

			 		if(strcmp(dummy,"01") == 0)
			 		{
			 		   racal_log_message( 2, 2,"CAVV-VERIFY failure.","racalVerify_CAVV_for_3Dsecure",1 );
			 		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
			 		}
			 		if(strcmp(dummy,"10") == 0)
			 		{
			 			racal_log_message( 2, 2,"CAVV-VERIFY CVK A or B parity error.","racalVerify_CAVV_for_3Dsecure",1 );
			 			return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
			 		}

			 		if(strcmp(dummy,"12") == 0)
			 		{
			 		   racal_log_message( 2, 2,"CAVV-VERIFY No Key loaded in user storage.","racalVerify_CAVV_for_3Dsecure",1 );
			 		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
			 		}

			 		if(strcmp(dummy,"13") == 0)
			 		{
			 		   racal_log_message( 2, 2,"CAVV-VERIFY LMK error report to superviser.","racalVerify_CAVV_for_3Dsecure",1 );
			 		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
			 		}

			 		if(strcmp(dummy,"15") == 0)
			 		{
			 		   racal_log_message( 2, 2,"CAVV-VERIFY Error in input data.","racalVerify_CAVV_for_3Dsecure",1 );
			 		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
			 		}

			 		if(strcmp(dummy,"21") == 0)
			 		{
			 		   racal_log_message( 2, 2,"CAVV-VERIFY Invalid user storage index.","racalVerify_CAVV_for_3Dsecure",1 );
			 		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
			 		}

			 }
			 else  /* else for: if (racalSendToPort(output) == TRANSACTION_VERIFIED)*/
			 {
			 			   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
			 }

			 return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/

}/*racalVerify_CAVV_for_3Dsecure_VISA*/

void Racal_JCB_Input(char * Supplemental_output)
{


	strncpy(Supplemental_output,&auth_tx.TLF01_details.product_codes[3].amount[10],2);
	strncat(Supplemental_output,&auth_tx.TLF01_details.product_codes[6].amount[0],2);
	strncat(Supplemental_output,&auth_tx.TLF01_details.product_codes[3].amount[1],1);
	strncat(Supplemental_output,&auth_tx.TLF01_details.product_codes[3].amount[2],2);
	//strcat(Supplemental_output,JCB_PADDED_ZEROES);
}
int racalVerify_CAVV_for_3Dsecure_JCB()
{
   char msgHeader[5] = {0};
   char Command[3] = {0};
   char Delimiter[2] = {0};
   char output[200]= {0};
   BYTE CAVV_key[33];
   char cardType = 'Z';
   char Supplement_Data[50] = {0};
   char strTemp[512] = {0};


   memset (&msgHeader,0,sizeof(msgHeader));
   memset (&Command,0,sizeof(Command));
   memset (&Delimiter,0,sizeof(Delimiter));
   memset (&output,0,sizeof(output));
   memset (&returnString,0,ReturnString_Size);





   strcpy (msgHeader ,"1234");

   strcpy (Command,"CW");


   	 if(strlen(auth_tx.CAF01_details.csck) != 0)
	 {
		 strcpy(CAVV_key, auth_tx.CAF01_details.csck);
	 }
	 else
	 {
		racal_log_message( 2, 2, "JCB 3DS2 HSM key is blank.","racalVerify_CAVV_for_3Dsecure_JCB",1 );
		return(TRANSACTION_NOT_VERIFIED);
	 }


	 strcpy (Delimiter,";");
	 Racal_JCB_Input( &Supplement_Data);


	 /*if(strlen(auth_tx.TLF01_details.card_num) >= 14 )
	 {
		 memset(&dummy,0,Dummy_Size);
		 strncpy(dummy,auth_tx.TLF01_details.card_num,16);
	 }*/

	 /*Build message and send to Racal.  */
			 memset (&output,0,sizeof(output));
			 strcat(output, msgHeader);
			 strcat(output, Command);
			 strcat(output, "U");   //keyTag
			 strcat(output, CAVV_key);
		     if (strlen(auth_tx.TLF01_details.card_num) != 0)
			 {
				strcat(output,auth_tx.TLF01_details.card_num);
			 }
			 strcat(output, Delimiter);
			 strcat(output, Supplement_Data);


			 if (strlen(output) > 150)
			 {
				racal_log_message( 2, 2,"CAVV Cmd Msg too long.","racalVerify_CAVV_for_3Dsecure",1 );
				return(TRANSACTION_NOT_VERIFIED);
			 }
			 /*printf("\n racal input for 10.6 CW: %s \n", output);*/
			 sprintf(strTemp, " input of racal CW: %s", output);
			 racal_log_message( 2, 2,strTemp,"racalVerify_CAVV_for_3Dsecure",1 );
			 if (racalSendToPort(output) == TRANSACTION_VERIFIED)
			 {
			 sprintf(strTemp, " output of racal cmnd CW: %s", returnString);
			 racal_log_message( 2, 2,strTemp,"racalVerify_CAVV_for_3Dsecure",1 );
			 		/*parse the message check for the message header.*/
			 		memset(&dummy,0,Dummy_Size);
			 		strncpy(dummy,returnString,4);
			 		if(strcmp(dummy,msgHeader) != 0)
			 		{
			 		   racal_log_message( 2, 2,"CAVV Msg Header mis-match.","racalVerify_CAVV_for_3Dsecure",1 );
			 		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
			 		}

			 		/*parse the message check for the response code.*/
			 		memset(&dummy,0,Dummy_Size);
			 		strncpy(dummy,returnString + 4, 2);
			 		if(strcmp(dummy,"CX") !=0)
			 		{
			 		   racal_log_message( 2, 2,"CAVV Cmd Msg mis-match.","racalVerify_CAVV_for_3Dsecure",1 );
			 		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
			 		}

			 		/* Parse the message check for the error code. */
			 		memset(&dummy,0,Dummy_Size);
			 		strncpy(dummy,returnString + 6, 2);
			 		if(strcmp(dummy,"00") == 0)
			 		{
			 		  /* copy Cvv to check at txcntl */
			 		 strncpy(auth_tx.conv_date, returnString + 8, 3);
			 		  return (TRANSACTION_VERIFIED); /*cavv valid*/
			 		}

			 		if(strcmp(dummy,"01") == 0)
			 		{
			 		   racal_log_message( 2, 2,"CAVV-VERIFY failure.","racalVerify_CAVV_for_3Dsecure",1 );
			 		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
			 		}
			 		if(strcmp(dummy,"10") == 0)
			 		{
			 			racal_log_message( 2, 2,"CAVV-VERIFY CVK A or B parity error.","racalVerify_CAVV_for_3Dsecure",1 );
			 			return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
			 		}

			 		if(strcmp(dummy,"12") == 0)
			 		{
			 		   racal_log_message( 2, 2,"CAVV-VERIFY No Key loaded in user storage.","racalVerify_CAVV_for_3Dsecure",1 );
			 		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
			 		}

			 		if(strcmp(dummy,"13") == 0)
			 		{
			 		   racal_log_message( 2, 2,"CAVV-VERIFY LMK error report to superviser.","racalVerify_CAVV_for_3Dsecure",1 );
			 		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
			 		}

			 		if(strcmp(dummy,"15") == 0)
			 		{
			 		   racal_log_message( 2, 2,"CAVV-VERIFY Error in input data.","racalVerify_CAVV_for_3Dsecure",1 );
			 		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
			 		}

			 		if(strcmp(dummy,"21") == 0)
			 		{
			 		   racal_log_message( 2, 2,"CAVV-VERIFY Invalid user storage index.","racalVerify_CAVV_for_3Dsecure",1 );
			 		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
			 		}

			 }
			 else  /* else for: if (racalSendToPort(output) == TRANSACTION_VERIFIED)*/
			 {
			 			   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
			 }

			 return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/

}/*racalVerify_CAVV_for_3Dsecure_VISA*/

int racalVerify_CAVV_for_3Dsecure_AMEX()
{

	   char msgHeader[5];
	   char cmdCode[3]={0},cvc1_serviceCode[4]={0}, cvc2_serviceCode[4]={0};
	   char delimiter[2];
	   char output[200],cardType = 'Z',csc[6]={0};
	   char cvc1_data[4]={0}, cvc2_data[4]={0};
	   int  track_present = 0, cvc2_present = 0, cvc2_sent = 0;
	   int  loopidx = 0;
	   int  cnt=0,result=0,jcnt=0;
	   char strTemp [256]={0};
	   CHAR *tempptr,fieldSeperator[2]={0};
	   CHAR csc_result5[6];
	   CHAR csc_result4[5];
	   CHAR csc_result3[4];
	   INT 	flag=0;
	   INT  AEVV_result_code;
	   CHAR AEVV_second_factor[3]={0},AEVV_bin_second_factor[2];
	   CHAR AEVV_iCSC[5]={0},AEVV_bin_iCSC[3];
	   CHAR AEVV_UPN[5]={0},AEVV_bin_UPN[3];
	   INT temp_out_len=0;

	   memset (&msgHeader,0,sizeof(msgHeader));
	   memset (&cmdCode,0,sizeof(cmdCode));
	   memset (&delimiter,0,sizeof(delimiter));
	   memset (&output,0,sizeof(output));
	   memset (&returnString,0,ReturnString_Size);
	   memset (&csc,0,sizeof(csc));

	   strcpy (msgHeader ,"1234");


	         /***********************Begin AMEX*********************************/
	         strcat(output,msgHeader);
	         strcat(output,"RY"); /* Changed Command code RY4- TF Phani*/
	         strcat(output,"3");
	         strcat(output,"3");

	         if(strlen(auth_tx.CAF01_details.cvk[3].cvkA) != 0 &&
	            strlen(auth_tx.CAF01_details.cvk[3].cvkB) != 0 	 )
			 {
				strcat(output,"U");
				strcat(output,auth_tx.CAF01_details.cvk[3].cvkA);
				strcat(output,auth_tx.CAF01_details.cvk[3].cvkB);

			 }
			 else
			 {
				racal_log_message( 2, 2, "AMEX 3DS2 HSM key is blank.","racalVerify_CAVV_for_3Dsecure_AMEX",1 );
				return(TRANSACTION_NOT_VERIFIED);
			 }

	         if (strlen(auth_tx.TLF01_details.card_num) != 0)
			 {
				strcat(output,auth_tx.TLF01_details.card_num);
				if(strlen(auth_tx.TLF01_details.card_num) != 19)
				   for(cnt = 19 ; (INT)strlen(auth_tx.TLF01_details.card_num) < cnt ;cnt--)
					  strcat(output,"0");
			 }
			 else
			 {
				racal_log_message( 2, 2, "AMEX 3DS2 ,card_num blank.","racalVerify_CAVV_for_3Dsecure_AMEX",1 );
				return(TRANSACTION_NOT_VERIFIED);
			 }

	         strncat(output, auth_tx.TLF01_details.dcf01_retired_cwk+26, 4);

	         //Copy Authn Result code and SF Authn code
	         strncat(output, &auth_tx.TLF01_details.dcf01_retired_cwk[1], 3);

	         //if the output is bigger then 150 do not send it out.
	         if (strlen(output) > 150)
	         {
	            racal_log_message( 2, 2, "AMEX 3DS2 HSM Cmd Msg to long.","racalVerify_CAVV_for_3Dsecure_AMEX",1 );
	            return(TRANSACTION_NOT_VERIFIED);
	         }

	         sprintf(strTemp, " AMEX to racal RY: %s", output);
	         racal_log_message( 2, 2,strTemp,"racalVerify_CAVV_for_3Dsecure_AMEX",1 );
	         //send the message to the racal box.
	         if (racalSendToPort(output) == TRANSACTION_VERIFIED)
	         {
	        	sprintf(strTemp, " output of racal cmnd RZ: %s", returnString);
	        	racal_log_message( 2, 2,strTemp,"racalVerify_CAVV_for_3Dsecure_AMEX",1 );

	        	/*parse the message check for the message header.*/
	            memset(&dummy,0,Dummy_Size);
	            strncpy(dummy,returnString,4);
	            if(strcmp(dummy,msgHeader) != 0)
	            {
	               racal_log_message( 2, 2, "AMEX 3DS2 HSM Cmd header mis-match.","racalVerify_CAVV_for_3Dsecure_AMEX",1 );
	               return(TRANSACTION_NOT_VERIFIED);
	            }
	            /*parse the message check for the response code.*/
	            memset(&dummy,0,Dummy_Size);
	            strncpy(dummy,returnString + 4, 2);
	            if(strcmp(dummy,"RZ") !=0)
	            {
	               racal_log_message( 2, 2, "AMEX 3DS2 HSM Cmd Code mis-match.","racalVerify_CAVV_for_3Dsecure_AMEX",1 );
	               return(TRANSACTION_NOT_VERIFIED);
	            }

	            /*parse the message check for the error code.*/
	            memset(&dummy,0,Dummy_Size);
	            strncpy(dummy,returnString + 6, 2);

				printf("Return from Racal : %s \n",returnString);
	            //if this value is not 00 reject the transaction.
	            if(strcmp(dummy,"00") != 0)
	            {
	               if(strcmp(dummy,"10") == 0)
	               {
	            	  racal_log_message( 2, 2, "AMEX 3DS2 HSM CSCK parity error.","racalVerify_CAVV_for_3Dsecure_AMEX",1 );
				   }
	               if(strcmp(dummy,"12") == 0)
	                  racal_log_message( 2, 2, "AMEX 3DS2 HSM No Keys in user storage.","racalVerify_CAVV_for_3Dsecure_AMEX",1 );
	               if(strcmp(dummy,"13") == 0)
	                  racal_log_message( 2, 2, "AMEX 3DS2 HSM LMK parity error.","racalVerify_CAVV_for_3Dsecure_AMEX",1 );
	               if(strcmp(dummy,"15") == 0)
	                  racal_log_message( 2, 2, "AMEX 3DS2 HSM Error in input data.","racalVerify_CAVV_for_3Dsecure_AMEX",1 );
				   if(strcmp(dummy,"01") == 0)
				   	{
					   strcpy(auth_tx.TLF01_details.response_text,"Invalid 4 CSC");
				   	   racal_log_message( 2, 2, "Invalid CSC/CVV/3D sec.","racalVerify_CAVV_for_3Dsecure_AMEX",1 );
				   	}
	// Invalid      CSC           01
	//INvalid  27 CSCK not doubel lenght
	// Invalid  68 HSM command disabled

	               return(TRANSACTION_NOT_VERIFIED);
	            }
	            else if(strcmp(dummy,"00") == 0)
				{
				  /* copy avop to check at txcntl */
				  strncpy(auth_tx.conv_date, returnString + 9, 3);
				  return (TRANSACTION_VERIFIED); /*cavv valid*/
				}

	#ifdef _DEBUG
	            racal_log_message( 2, 1, "mag-strip PASSED.","racalVerify_CAVV_for_3Dsecure_AMEX",1 );
	#endif
	            return(TRANSACTION_VERIFIED);

	         }/*end of if racalSendToPort*/
	         else
	            return(TRANSACTION_NOT_VERIFIED);


	         /****************************end AMEX***************************************/


}

int racalVerify_CAVV_for_3Dsecure_DCI()
{
	   char msgHeader[5] = {0};
	   char cmdCode[3] = {0};
	   char cvv_value[5] = {0};
	   char cavv2_atn_Code[5] = {0};
	   char delimiter[2] = {0};
	   char output[200]= {0};
	   char cardType = 'Z';
	   char csc[6] = {0};
	   char strTemp[512] = {0};

	   BYTE HMAC_key[33];
	   CHAR hmac_key_len[4] = {0};

	   memset (&msgHeader,0,sizeof(msgHeader));
	   memset (&cmdCode,0,sizeof(cmdCode));
	   memset (&delimiter,0,sizeof(delimiter));
	   memset (&output,0,sizeof(output));
	   memset (&returnString,0,ReturnString_Size);
	   memset (&csc,0,sizeof(csc));

	   strcpy (msgHeader ,"1234");
	   strcpy (cmdCode,"CW");
	   strcpy (delimiter,";");
		 //racal_Get_Sha256_Encryption( &Suppl_Amt);

		 if(strlen(auth_tx.CAF01_details.cvk[3].cvkA) != 0 &&
		   strlen(auth_tx.CAF01_details.cvk[3].cvkB) != 0)
		 {
			strcat(HMAC_key,auth_tx.CAF01_details.cvk[3].cvkA);
			strcat(HMAC_key,auth_tx.CAF01_details.cvk[3].cvkB);
		 }
		 else
		 {
			racal_log_message( 2, 2, "DCI 3DS2 HSM key is blank.","racalVerify_CAVV_for_3Dsecure_DCI",1 );
			return(TRANSACTION_NOT_VERIFIED);
		 }

		 if(strlen(auth_tx.TLF01_details.dcf01_retired_cwk) != 0)
		 {
		 		strncpy(cavv2_atn_Code,&auth_tx.TLF01_details.dcf01_retired_cwk[12],4);
		 		strncpy(cvv_value,&auth_tx.TLF01_details.dcf01_retired_cwk[2],4);
		 }

		 /*Build message and send to Racal.  */
		 strcat(output, msgHeader);
		 strcat(output, cmdCode);
		 strcat(output, "U");
		 strcat(output, HMAC_key);
		 strcat(output, auth_tx.TLF01_details.card_num);
		 strcat(output, delimiter);
		 strncat(output, cavv2_atn_Code, 4);
		 strncat(output, &cvv_value[1], 3);

		 if (strlen(output) > 150)
		 {
			racal_log_message( 2, 2,"CAVV Cmd Msg too long.","racalVerify_CAVV_for_3Dsecure_DCI",1 );
			return(TRANSACTION_NOT_VERIFIED);
		 }

		 sprintf(strTemp, " DCI 3ds2 to racal CW: %s", output);
		 racal_log_message( 2, 2,strTemp,"racalVerify_CAVV_for_3Dsecure_DCI",1 );
		 if (racalSendToPort(output) == TRANSACTION_VERIFIED)
		 {

			 sprintf(strTemp, " output of racal cmnd CW: %s", returnString);
			 racal_log_message( 2, 2,strTemp,"racalVerify_CAVV_for_3Dsecure_DCI",1 );

			/*parse the message check for the message header.*/
			memset(&dummy,0,Dummy_Size);
			strncpy(dummy,returnString,4);
			if(strcmp(dummy,msgHeader) != 0)
			{
			   racal_log_message( 2, 2,"CAVV Msg Header mis-match.","racalVerify_CAVV_for_3Dsecure_DCI",1 );
			   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
			}

			/*parse the message check for the response code.*/
			memset(&dummy,0,Dummy_Size);
			strncpy(dummy,returnString + 4, 2);
			if(strcmp(dummy,"CX") !=0)
			{
			   racal_log_message( 2, 2,"CAVV Cmd Msg mis-match.","racalVerify_CAVV_for_3Dsecure_DCI",1 );
			   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
			}

			/* Parse the message check for the error code. */
			memset(&dummy,0,Dummy_Size);
			strncpy(dummy,returnString + 6, 2);
			if(strcmp(dummy,"00") == 0)
			{
			  /* copy Cvv to check at txcntl */
			 strncpy(auth_tx.conv_date, returnString + 8, 3);
			  return (TRANSACTION_VERIFIED); /*cavv valid*/
			}

			if(strcmp(dummy,"01") == 0)
			{
			   racal_log_message( 2, 2,"CAVV-VERIFY failure.","racalVerify_CAVV_for_3Dsecure_DCI",1 );
			   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
			}
			if(strcmp(dummy,"10") == 0)
			{
				racal_log_message( 2, 2,"CAVV-VERIFY CVK A or B parity error.","racalVerify_CAVV_for_3Dsecure_DCI",1 );
				return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
			}

			if(strcmp(dummy,"12") == 0)
			{
			   racal_log_message( 2, 2,"CAVV-VERIFY No Key loaded in user storage.","racalVerify_CAVV_for_3Dsecure_DCI",1 );
			   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
			}

			if(strcmp(dummy,"13") == 0)
			{
			   racal_log_message( 2, 2,"CAVV-VERIFY LMK error report to superviser.","racalVerify_CAVV_for_3Dsecure_DCI",1 );
			   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
			}

			if(strcmp(dummy,"15") == 0)
			{
			   racal_log_message( 2, 2,"CAVV-VERIFY Error in input data.","racalVerify_CAVV_for_3Dsecure_DCI",1 );
			   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
			}

			if(strcmp(dummy,"21") == 0)
			{
			   racal_log_message( 2, 2,"CAVV-VERIFY Invalid user storage index.","racalVerify_CAVV_for_3Dsecure_DCI",1 );
			   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
			}

		 }
		 else
		 {
			   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
		 }

		 return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/

}

INT racal_parse_field_48( int src_len, char *src, pBYTE ucaf_data )
{
   #define  FIELD48_MAX_LEN  100

   INT  ret_val = 0;
   INT  sub_field_len;
   INT  MainIndex;
   INT  TotalLen;
   CHAR tag[3];
   CHAR len[3];


   TotalLen =  src_len ;

   /* MainIndex starts at 1 to skip over the Transaction Category Code. */
   for( MainIndex=1; MainIndex<TotalLen && MainIndex<FIELD48_MAX_LEN; )
   {
      /* Get subfield tag (name of subfield). */
      memset( tag, 0x00, sizeof( tag ) );
      strncpy( tag, &src[MainIndex], 2 );
      MainIndex += 2;

 //     /* Get length of subfield. */
 //     if ( 0 == strcmp(tag, DE48_TAG43) )
 //     {
 //        /* Subfield 43 is fixed length. */
 //        sub_field_len = DE48_TAG43_LEN;

 //        /* Currently, do not need the data from this subfield. */
 //     }
//      else
 //     {
         /* Other subfields are variable length. */
         memset( len, 0x00, sizeof(len) );
         memcpy( len, &src[MainIndex], 2 );
         sub_field_len = atoi( len );
         MainIndex += 2;

         /* Get the data from these subfields. */
		if ( 0 == strncmp(tag, DE48_TAG43, 2) )
		 {
			memcpy(ucaf_data,&src[MainIndex],sub_field_len);
		 }

 //     }
      MainIndex += sub_field_len;
   }
   return( ret_val );
}  /* End parse_field_48 */

BOOLEAN racal_check_control_bytes_validation()
{
	if( 0 == strncmp(auth_tx.TLF01_details.product_codes[5].quantity, "kA" ,2) ||
		0 == strncmp(auth_tx.TLF01_details.product_codes[5].quantity, "kB" ,2) ||
		0 == strncmp(auth_tx.TLF01_details.product_codes[5].quantity, "kG" ,2) ||
		0 == strncmp(auth_tx.TLF01_details.product_codes[5].quantity, "kH" ,2) ||
		0 == strncmp(auth_tx.TLF01_details.product_codes[5].quantity, "kO" ,2) ||
		0 == strncmp(auth_tx.TLF01_details.product_codes[5].quantity, "kP" ,2) )
	{
		return true;
	}
	return false;

}
INT racalVerify_IAV_for_3Dsecure_MC()
{
	char output[526]	= {0};
	char msgHeader[5]   = {0};
	INT key_length,bin_key_length= 0;
	char skeylen[5]		= {0};
	INT data_length	,bin_data_length	= 0;
	char sdatalen[5]	= {0};
	char outputloger[526]={0};
	char strTemp[1024]   = {0};
	int i=0;
	char hsm_generated_IAV[20]={0}, ascii_hsm_generated_IAV[33]={0};
	char key[81]={0}, data[100]={0}, bin_key[128]={0}, bin_data[128]={0};
	int return_code=0;
	int len_data=0, bin_hmac_len=0,ascii_hmac_len=0;
	char temp_rc[3]={0};
	INT de48_retcode= 0,hmac_ret_val=1;
	   	INT ucaf_data_64_len =0;
	   	BYTE UCAF_data[33]={0};
	   	INT ucaf_data_len,astr_len = 0;
	char cvv_value[9] = {0};
	int De48len=0;
		   	BYTE DE48_data[256]={0};
	   	BYTE UCAF_data_ascii[41]={0},UCAF_data_bin[41]={0};


#define ECI_SLI_FULLYAUTHENTICATED "12"

	memset (msgHeader,0x00,sizeof(msgHeader));
	memset (&output,0x00,sizeof(output));
	memset (&returnString,0x00,ReturnString_Size);

	/* Default msg header*/
	strncpy(msgHeader,"1234",4);
	strncpy (output ,msgHeader,4);
	len_data +=4;
	/* CMD to build the IAV*/
	strncat (output,"LQ",2);
	len_data +=2;
	/* hash identifier for LMK 06 for SHA-256*/
	strncat(output,"06",2);
	len_data +=2;
	/*Output length expected by HSM*/
	strncat(output,"0016",4);
	len_data +=4;
	/*HMAC key format*/
	strncat(output,"00",2);
	len_data +=2;
	/*HMAC key length and KEY */
	key_length= strlen(auth_tx.EMV_details.future_field2);

	if(key_length == 0)
	{
		racal_log_message( 2, 2,"Key is not available.","racalVerify_IAV_for_3Dsecure_MC",1 );
	    return (TRANSACTION_NOT_VERIFIED);
	}

	strncpy(key,auth_tx.EMV_details.future_field2,key_length);
	sprintf(strTemp, "key used for MC 3DS 2.0 %s", key);
	racal_log_message( 2, 2,strTemp,"racalVerify_IAV_for_3Dsecure_MC",1 );
	/*convert the key into binary and copy the binary lenght into the command*/
	genutil_asc_to_bin(key,bin_key,&bin_key_length);
	sprintf(skeylen,"%04d",bin_key_length);
	strncat(output,skeylen,4);
	len_data +=4;
	strncat(output,bin_key ,bin_key_length);
	len_data +=bin_key_length;
	/*Delimiter*/
	strncat(output,";",1);
	len_data +=1;
	/*Data length*/
	data_length= strlen(auth_tx.EMV_details.future_field1);

	if(data_length == 0)
	{
		racal_log_message( 2, 2,"Data is not available.","racalVerify_IAV_for_3Dsecure_MC",1 );
		return (TRANSACTION_NOT_VERIFIED);
	}
	/*Data*/
	strncat(data,auth_tx.EMV_details.future_field1,data_length);
	genutil_asc_to_bin(data,bin_data,&bin_data_length);

	sprintf(sdatalen,"%05d",bin_data_length);
	strncat(output,sdatalen,5);
	len_data +=5;
	//strncat(output,bin_data,sdatalen);
	memcpy(&output[len_data],bin_data,bin_data_length);
	//printf("\n racal input for MC 3DS 2.0 %s \n", output);
	len_data+=bin_data_length;
	sprintf(strTemp, "racal input for MC 3DS 2.0 %s", output);
	racal_log_message( 2, 2,strTemp,"racalVerify_IAV_for_3Dsecure_MC",1 );
	if (racalSendToPort_MC(output,len_data) == TRANSACTION_VERIFIED)
	{
		memset(&dummy,0,Dummy_Size);
		strncpy(dummy,returnString,4);
		if(strcmp(dummy,msgHeader) != 0)
		{
			   racal_log_message( 2, 2,"CAVV Msg Header mis-match.","racalVerify_IAV_for_3Dsecure_MC",1 );
			   return (TRANSACTION_NOT_VERIFIED);
		}

		/*parse the message check for the response code.*/
		memset(&dummy,0,Dummy_Size);
		strncpy(dummy,returnString + 4, 2);
		if(strcmp(dummy,"LR") !=0)
		{
			sprintf(strTemp, "Resp Cmd Msg mis-match%s", dummy);
			racal_log_message( 2, 2,strTemp,"racalVerify_IAV_for_3Dsecure_MC",1 );
			return (TRANSACTION_NOT_VERIFIED);
		}

		/* Parse the message check for the error code. */
		memset(&dummy,0,Dummy_Size);
		strncpy(dummy,returnString + 6, 2);
		strncpy(temp_rc,dummy,2);
		if(strcmp(dummy,"00") != 0)
		{
			sprintf(strTemp, "Response code received from HSM %s", dummy);
			racal_log_message( 2, 2,strTemp,"racalVerify_IAV_for_3Dsecure_MC",1 );
			if('\0' != auth_tx.TLF01_details.cvc_data[0])
			{
				 racal_log_message( 2, 2,"CAVV failed, ATP trying to do CVV2 validation", "racalVerify_IAV_for_3Dsecure_MC", 1 );
				 return_code = racalEquitVerifyCVV_CVC_CSC_CAV();

				 if (return_code == TRANSACTION_NOT_VERIFIED)
				 {
					  strcpy(auth_tx.TLF01_details.security_response_code,"01");
					  auth_tx.TLF01_details.cvc = 'N';
				 }
				 else if (return_code == CVC2_TRANSACTION_NOT_VERIFIED)
				 {
					  strcpy(auth_tx.TLF01_details.security_response_code,"02");
					  auth_tx.TLF01_details.cvc = 'N';
				 }
				 else if (return_code == CVC2_TRANSACTION_VERIFIED)
				 {
					   strcpy(auth_tx.TLF01_details.security_response_code,"03");
					   auth_tx.TLF01_details.cvc = 'M';
				 }
				 else
				 {
					 auth_tx.TLF01_details.cvc = 'M';
					 strcpy( auth_tx.TLF01_details.security_response_code, "00" );
				 }
				 memset(strTemp,0x00,sizeof(strTemp));
				 sprintf( strTemp,"result code of CVV2 validation %d",return_code);

				 return return_code;
			}
			else
			{
				/*strncpy(auth_tx.pos_pin_capt_code,temp_rc,2);
				strcpy(auth_tx.TLF01_details.security_response_code,"05");
				racal_log_message( 2, 2,"CAVV validation failed and CVV2 data is not present", "racalVerify_IAV_for_3Dsecure_MC", 1 );
				return (TRANSACTION_NOT_VERIFIED);*/

				strcpy(auth_tx.TLF01_details.security_response_code,"00");
				racal_log_message( 2, 2,"CAVV validation failed and CVV2 data is not present go for BAU-case 5", "racalVerify_IAV_for_3Dsecure_MC", 1 );
				return (TRANSACTION_VERIFIED);

			}
		}


		memset(&dummy,0,Dummy_Size);
		strncpy(dummy,returnString + 8, 4);
		if(strncmp(dummy,"0016",4) !=0 )
		{
			sprintf(strTemp, "HMAC output length mismatch %s", dummy);
			racal_log_message( 2, 2,strTemp,"racalVerify_IAV_for_3Dsecure_MC",1 );
			return (TRANSACTION_NOT_VERIFIED);
		}

		memset(&dummy,0,Dummy_Size);
		strncpy(dummy,returnString + 12, 16);

		strncpy(hsm_generated_IAV,dummy,16);
		bin_hmac_len=16;
		genutil_bin_to_ascii (hsm_generated_IAV, bin_hmac_len, ascii_hsm_generated_IAV, ascii_hmac_len);

		if(auth_tx.EMV_details.issuer_script_2[0] == '\0')
		{
			De48len = auth_tx.future_use_3[0];
			memcpy(DE48_data,&auth_tx.future_use_3[1],De48len);
		}
		else
		{
			De48len = strlen(auth_tx.EMV_details.issuer_script_2);
			memcpy(DE48_data,auth_tx.EMV_details.issuer_script_2,strlen(auth_tx.EMV_details.issuer_script_2));
		}
		de48_retcode = parse_field_48( De48len,&DE48_data,&UCAF_data );
		ucaf_data_64_len =strlen(UCAF_data);
		ucaf_data_len = genutil_base64_to_binary( UCAF_data, strlen(UCAF_data),&UCAF_data_bin );
		genutil_bin_to_ascii (UCAF_data_bin, ucaf_data_len, UCAF_data_ascii, astr_len);
		strncpy(cvv_value,&UCAF_data_ascii[4],8);

		memset(strTemp,0x00,sizeof(strTemp));
		sprintf( strTemp,"CAVV out put from HSM(%s), CAVV generated from transaction(%s)\n",ascii_hsm_generated_IAV,cvv_value);
		racal_log_message( 2, 2,strTemp, "racalVerify_IAV_for_3Dsecure_MC", 1 );

		hmac_ret_val = strncmp(cvv_value,ascii_hsm_generated_IAV,8);
		if(hmac_ret_val == 0)
		{
			racal_log_message( 2, 2,"HSM generated CAVV and ATP generated CAVV does matched, ATP trying to do CVV2 validation", "racalVerify_IAV_for_3Dsecure_MC", 1 );
		}
		else
		{
			racal_log_message( 2, 2,"HSM generated CAVV and ATP generated CAVV does not match, ATP trying to do CVV2 validation", "racalVerify_IAV_for_3Dsecure_MC", 1 );
		}

		if('\0' != auth_tx.TLF01_details.cvc_data[0] )
		{
				 memset(strTemp,0x00,sizeof(strTemp));
				sprintf( strTemp,"SLI value(%s), hmac_ret_val(CAVV generated by ATP and HSM compared result value):(%d), \n",auth_tx.TLF01_details.profile,hmac_ret_val);
				racal_log_message( 2, 2,strTemp, "racalVerify_IAV_for_3Dsecure_MC", 1 );
				 return_code = racalEquitVerifyCVV_CVC_CSC_CAV();

		         if (return_code == TRANSACTION_NOT_VERIFIED)
		         {
		        	  strcpy(auth_tx.TLF01_details.security_response_code,"01");
		        	  auth_tx.TLF01_details.cvc = 'N';
		         }
		         else if (return_code == CVC2_TRANSACTION_NOT_VERIFIED)
		         {
		        	  strcpy(auth_tx.TLF01_details.security_response_code,"02");
		        	  auth_tx.TLF01_details.cvc = 'N';
		         }
		         else if (return_code == CVC2_TRANSACTION_VERIFIED)
		         {
		        	   strcpy(auth_tx.TLF01_details.security_response_code,"03");
		        	   auth_tx.TLF01_details.cvc = 'M';
		         }
		         else
		         {
		        	 auth_tx.TLF01_details.cvc = 'M';
		        	 strcpy( auth_tx.TLF01_details.security_response_code, "00" );

		         }
		         memset(strTemp,0x00,sizeof(strTemp));
		         sprintf( strTemp,"result code of CVV2 validation %d",return_code);

		         return return_code;
		}
		else
		{
			/*if(hmac_ret_val == 0)
			{
				strcpy(auth_tx.TLF01_details.security_response_code,"00");
				racal_log_message( 2, 2,"CAVV matched and no CVV2 present", "racalVerify_IAV_for_3Dsecure_MC", 1 );
				return (TRANSACTION_VERIFIED);
			}
			else
			{
				strncpy(auth_tx.pos_pin_capt_code,temp_rc,2);
				strcpy(auth_tx.TLF01_details.security_response_code,"05");
				racal_log_message( 2, 2,"CAVV does not match and no CVV2 present", "racalVerify_IAV_for_3Dsecure_MC", 1 );
				return (TRANSACTION_NOT_VERIFIED);
			}*/
			if(hmac_ret_val == 0)
			{
				racal_log_message( 2, 2,"CAVV matched and no CVV2 present go for BAU case 6", "racalVerify_IAV_for_3Dsecure_MC", 1 );
			}
			else
			{
				racal_log_message( 2, 2,"CAVV does not match and no CVV2 present go for BAU case 5", "racalVerify_IAV_for_3Dsecure_MC", 1 );
			}

			strcpy(auth_tx.TLF01_details.security_response_code,"00");
			return (TRANSACTION_VERIFIED);
		}
	}
	else
	{
		racal_log_message( 2, 2,"CAVV verification send failed, could not send or receive the command to/from HSM", "racalVerify_IAV_for_3Dsecure_MC", 1 );
		strcpy(auth_tx.TLF01_details.security_response_code,"05");
	}
	return (TRANSACTION_NOT_VERIFIED);
}

/*************************************Keyblock Function *******************************************/

INT racal_Check_If_UPI_Transaction (void)
{
	if ((0 == strncmp(auth_tx.TLF01_details.product_code,CUP_CARD_INDICATOR_FLAG,2))||
		(0 == strncmp(auth_tx.BIN01_details.card_family,CARD_CUP,CARD_TYPE_LEN)))
	{
		return (true) ;
	}
	else
	{
		return (false);
	}
}

INT racal_Check_If_DCI_Transaction (void)
{
	if ((0 == strcmp(auth_tx.BIN01_details.card_family,CARD_DINERS)))
	{
		return (true) ;
	}
	else
	{
		return (false);
	}
}
INT racal_Check_If_VISA_Transaction (void)
{
	if ( auth_tx.TLF01_details.card_num[0]=='4' ||
		(0 == strcmp(auth_tx.BIN01_details.card_family,CARD_VISA)||
		0 == strcmp(auth_tx.BIN01_details.card_family,CARD_DUAL)))

	{
		return (true) ;
	}
	else
	{
		return (false);
	}
}
INT racal_Check_If_MC_Transaction (void)
{
	char MC_bin_series[7]   = {0};
	int  MC_Bin_series_low  = 222100;
	int  MC_Bin_series_high = 272099;
	int  MC_bin_value 		= 0;
	int  MC_Bin_serice_len  = 6 ;

	strncpy(MC_bin_series,auth_tx.TLF01_details.card_num,MC_Bin_serice_len);
	MC_bin_value = atoi(MC_bin_series);

	if(	auth_tx.TLF01_details.card_num[0]=='5'||
		(0 == strcmp(auth_tx.BIN01_details.card_family,CARD_MASTERCARD)))
	{
		return (true) ;
	}
	else if( (MC_bin_value >= MC_Bin_series_low) && (MC_bin_value <= MC_Bin_series_high))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}
INT racal_Check_If_JCB_Transaction (void)
{
	if((auth_tx.TLF01_details.card_num[0]=='3') &&
	   (auth_tx.TLF01_details.card_num[1] == '5'))
	{
		return (true) ;
	}
	else if(0 == strcmp(auth_tx.BIN01_details.card_family,CARD_JCB))
	{
		return (true) ;
	}
	else
	{
		return (false);
	}
}
INT racal_Check_If_AMEX_Transaction (void)
{
	if((auth_tx.TLF01_details.card_num[0]=='3') &&
	   ((auth_tx.TLF01_details.card_num[1] == '4') ||
		(auth_tx.TLF01_details.card_num[1] == '7')))
	{
		return (true) ;
	}
	else if(0 == strcmp(auth_tx.BIN01_details.card_family,CARD_AMEX))
	{
		return (true) ;
	}
	else
	{
		return (false);
	}
}

/*************************************************************************/

INT racal_TranlatePin_Keyblock_MC()
{
   INT   len = 0;
   CHAR  msgHeader[5]={0};
   CHAR  cmdCode[3]  ={0};
   CHAR  output[200] ={0};
   CHAR  strTemp[256]={0};

   BYTE asciiPIN[20]={0};
   INT  blen =0 ;
   INT  astr_len=0;

   memset (&msgHeader,0,sizeof(msgHeader));
   memset (&cmdCode,0,sizeof(cmdCode));
   memset (&output,0,sizeof(output));
   memset (&returnString,0,ReturnString_Size);

   strcpy (msgHeader ,"5678");


   /*the value of "LO" represents terminal for origin.*/
   /*The value of "RO" represents network for origin.*/
   /*if(strcmp(auth_tx.BIN01_details.card_family,CARD_DINERS) == 0)
	   strcpy (cmdCode,"CA");
   else*/
	   if(strcmp(auth_tx.TLF01_details.dispensation.origin, "RO") == 0 ||
	  	 strcmp(auth_tx.BIN01_details.card_family,CARD_CUP) == 0 ||
		 0 == strncmp(auth_tx.TLF01_details.product_code,CUP_CARD_INDICATOR_FLAG,2)
		 )
         strcpy (cmdCode,"CC");
	  else if(strcmp(auth_tx.TLF01_details.dispensation.origin, "LO") == 0)
      strcpy (cmdCode,"CA");

      else
      {
         racal_log_message( 2, 2, "PIN-TRANX dispen-origin mis-match.","racal_TranlatePin_Keyblock_MC",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

   strcpy(output,msgHeader);
   strcat(output,cmdCode);

   /* Add the source TPK or ZPK to the message. */
   len = strlen( auth_tx.TLF01_details.source_key );
   if ( len > 0 )
   {
      switch( len )
      {
         case 32:
        	 strcat( output, DOUBLE_LEN_KEY );
        	 break;
         case 48:
        	 strcat( output, TRIPLE_LEN_KEY );
        	 break;
         default :
        	 strcat( output, TRIPLE_LEN_KEY );
        	 break;
      }
      strcat( output, auth_tx.TLF01_details.source_key );
   }
   else
   {
      racal_log_message( 2, 2, "PIN-TRANX source_key blank.","racal_TranlatePin_Keyblock_MC",1 );
      return(TRANSACTION_NOT_VERIFIED);
   }

   /* Add the destination TPK or ZPK to the message.*/
   len = strlen( auth_tx.TLF01_details.dest_key );
   if ( len > 0 )
   {
      switch( len )
      {
         case 32:
        	 strcat( output, DOUBLE_LEN_KEY );
        	 break;
         case 48:
        	 strcat( output, TRIPLE_LEN_KEY );
        	 break;
         default :
        	 strcat( output, TRIPLE_LEN_KEY );
        	 break;
      }
      strcat( output, auth_tx.TLF01_details.dest_key );
   }
   else
   {
      racal_log_message( 2, 2, "PIN-TRANX dest_key blank.","racal_TranlatePin_Keyblock_MC",1 );
      return(TRANSACTION_NOT_VERIFIED);
   }

   /*add the max pin length*/
   strcat(output,"12");

   /*add the pin block to the message.*/
   if(strlen(auth_tx.TLF01_details.pin_block) != 0)
   {
	   if((0 == strncmp(auth_tx.TLF01_details.issuer_id,"nccup3",5))
		   || 		  (0 == strncmp(auth_tx.TLF01_details.issuer_id,"ncdci",5))
		  )
	   {
			/*we should send ASCII value.COnvert binary into ACSII*/
			blen = 8;
			astr_len =16;
			genutil_bin_to_ascii (auth_tx.TLF01_details.pin_block, blen, asciiPIN,astr_len);
			strcat(output,asciiPIN);

	   }
	   else
		   strcat(output,auth_tx.TLF01_details.pin_block);
   }
   else
   {
      racal_log_message( 2, 2, "PIN-TRANX pin_block blank.","racal_TranlatePin_Keyblock_MC",1 );
      return(TRANSACTION_NOT_VERIFIED);
   }
   /*add the next line because the net work handler send in a single character value.*/
   strcat(output,"0");

   /*add the source pin format to the message.*/
   if(strlen(auth_tx.TLF01_details.source_pin_format) != 0)
   {

	   strcat(output,auth_tx.TLF01_details.source_pin_format);
   }
   else
   {
      racal_log_message( 2, 2, "PIN-TRANX source_pin_format.","racal_TranlatePin_Keyblock_MC",1 );
      return(TRANSACTION_NOT_VERIFIED);
   }
   /*add the next line because the net work handler send in a single character value. */
   strcat(output,"0");

   /*add the destination pin format to the message.*/
   if(strlen(auth_tx.TLF01_details.dest_pin_format) != 0)
      strcat(output,auth_tx.TLF01_details.dest_pin_format);
   else
   {
      racal_log_message( 2, 2, "PIN-TRANX dest_pin_format blank.","racal_TranlatePin_Keyblock_MC",1 );
      return(TRANSACTION_NOT_VERIFIED);
   }


   /*farad 7/19/1999  Added the next line to take care of nccirs 12 digit pans*/
   if (  (0 == strcmp(auth_tx.TLF01_details.acquirer_id,"nccirs") ||
          0 == strcmp(auth_tx.TLF01_details.acquirer_id,"ncvisa") ||
          0 == strcmp(auth_tx.TLF01_details.acquirer_id,"ncvsms")) &&
         (strlen(auth_tx.TLF01_details.card_num) <= 12)
      )
   {
      memset(&dummy1,0,Dummy_Size);
      memset(&dummy,'\0',Dummy_Size);
      memset(&dummy,'0',Dummy_Size);
      strcpy(dummy + (16 - strlen(auth_tx.TLF01_details.card_num)),auth_tx.TLF01_details.card_num);

      strcpy(dummy1,dummy + (strlen(dummy) - 13));
      dummy1[12] = '\0';
      strcat(output,dummy1);
   }
   else if(strlen(auth_tx.TLF01_details.card_num) > 12 )
   {
      /*take the 12 right most digit from the account number.*/
      memset(&dummy,0,Dummy_Size);
      strcpy(dummy,auth_tx.TLF01_details.card_num + (strlen(auth_tx.TLF01_details.card_num) - 13));
      dummy[12] = '\0';
      strcat(output,dummy);
   }
   else
   {
      racal_log_message( 2, 2, "PIN-TRANX card_num < 12.","racal_TranlatePin_Keyblock_MC",1 );
      return (TRANSACTION_NOT_VERIFIED);
   }

   /*if the message lenght is more then 150 there is some thing wrong. */
   if (strlen(output) > 150)
   {
      racal_log_message( 2, 2, "PIN_TRANX Cmd Msg to long.","racal_TranlatePin_Keyblock_MC",1 );
      return(TRANSACTION_NOT_VERIFIED);
   }
   else
   {
	   sprintf(strTemp, " input of racal CC: %s", output);
	   racal_log_message( 2, 2,strTemp,"racal_TranlatePin_Keyblock_MC",1 );
   }

   /*send the message to the racal box. */
   if (racalSendToPort(output) == TRANSACTION_VERIFIED)
   {
      memset(&dummy,0,Dummy_Size);
      strncpy(dummy,returnString,4);
      sprintf(strTemp, " output of racal cmnd CD: %s", returnString);
      racal_log_message( 2, 2,strTemp,"racal_TranlatePin_Keyblock_MC",1 );
      /*check the value of the msg header */
      if(strcmp(msgHeader,dummy) != 0)
      {
         racal_log_message( 2, 2, "PIN-TRANX Msg header mis-match.","racal_TranlatePin_Keyblock_MC",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      /*check the value of the command code. */
      memset(&dummy,0,Dummy_Size);
      strncpy(dummy,returnString + 4,2);
      if ((strcmp(dummy,"CB") != 0) && (strcmp(dummy,"CD") != 0) )
      {
         racal_log_message( 2, 2, "PIN-TRANX cmd code mis-match.","racal_TranlatePin_Keyblock_MC",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      memset(&dummy,0,Dummy_Size);
      strncpy(dummy,returnString + 6,2);
      if(strcmp(dummy,"00") == 0)
      {
         memset(&dummy,0,Dummy_Size);
         strncpy(dummy,returnString + 10, 16);
         strcpy(auth_tx.TLF01_details.pin_block,dummy);

#ifdef _DEBUG
         racal_log_message( 2, 1, "PPIN-TRANX No problemo.","racal_TranlatePin_Keyblock_MC",1 );
#endif
         return(TRANSACTION_VERIFIED);
      }

      if(strcmp(dummy,"01") == 0)
      {
         racal_log_message( 2, 2, "PIN-TRANX failure","racal_TranlatePin_Keyblock_MC",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"10") == 0)
      {
         racal_log_message( 2, 2, "PIN-TRANSLATE TPK parity error.","racal_TranlatePin_Keyblock_MC",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"11") == 0)
      {
         racal_log_message( 2, 2, "PIN-TRANSLATE Dest ZPK parity error.","racal_TranlatePin_Keyblock_MC",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"12") == 0)
      {
         racal_log_message( 2, 2, "PIN-TRANSLATE No keys laoded in user storage.","racal_TranlatePin_Keyblock_MC",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"13") == 0)
      {
         racal_log_message( 2, 2, "PIN-TRANSLATE LMK error report to supervisor.","racal_TranlatePin_Keyblock_MC",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"15") == 0)
      {
         racal_log_message( 2, 2, "PIN-TRANSLATE Error in input data.","racal_TranlatePin_Keyblock_MC",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"20") == 0)
      {
         racal_log_message( 2, 2, "PIN-TRANSLATE PIN block data Error.","racal_TranlatePin_Keyblock_MC",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"21") == 0)
      {
         racal_log_message( 2, 2, "PIN-TRANSLATE Invalid user storage index.","racal_TranlatePin_Keyblock_MC",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"22") == 0)
      {
         racal_log_message( 2, 2, "PIN-TRANSLATE Invalid Account number.","racal_TranlatePin_Keyblock_MC",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"23") == 0)
      {
         racal_log_message( 2, 2, "PIN-TRANSLATE Invalid PIN block format code.","racal_TranlatePin_Keyblock_MC",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"24") == 0)
      {
         racal_log_message( 2, 2, "PIN-TRANSLATE PIN is less than 4 or more than 12.","racal_TranlatePin_Keyblock_MC",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      return(TRANSACTION_NOT_VERIFIED);

   }
   else
      return(TRANSACTION_NOT_VERIFIED);


}/*racal_TranlatePin_Keyblock_MC*/
/**********************************************/
INT racal_TranlatePin_Keyblock_VISA()
{
   INT   len = 0;
   CHAR  msgHeader[5]={0};
   CHAR  cmdCode[3]  ={0};
   CHAR  output[200] ={0};
   CHAR  strTemp[256]={0};

   BYTE asciiPIN[20]={0};
   INT  blen=0 ;
   INT  astr_len=0;

   memset (&msgHeader,0,sizeof(msgHeader));
   memset (&cmdCode,0,sizeof(cmdCode));
   memset (&output,0,sizeof(output));
   memset (&returnString,0,ReturnString_Size);

   strcpy (msgHeader ,"5678");


   /*the value of "LO" represents terminal for origin.*/
   /*The value of "RO" represents network for origin.*/
   /*if(strcmp(auth_tx.BIN01_details.card_family,CARD_DINERS) == 0)
	   strcpy (cmdCode,"CA");
   else*/
	   if(strcmp(auth_tx.TLF01_details.dispensation.origin, "RO") == 0 ||
	  	 strcmp(auth_tx.BIN01_details.card_family,CARD_CUP) == 0 ||
		 0 == strncmp(auth_tx.TLF01_details.product_code,CUP_CARD_INDICATOR_FLAG,2)
		 )
         strcpy (cmdCode,"CC");
	  else if(strcmp(auth_tx.TLF01_details.dispensation.origin, "LO") == 0)
      strcpy (cmdCode,"CA");

      else
      {
         racal_log_message( 2, 2, "PIN-TRANX dispen-origin mis-match.","racal_TranlatePin_Keyblock_VISA",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

   strcpy(output,msgHeader);
   strcat(output,cmdCode);

   /* Add the source TPK or ZPK to the message. */
   len = strlen( auth_tx.TLF01_details.source_key );
   if ( len > 0 )
   {
      switch( len )
      {
         case 32:
        	 strcat( output, DOUBLE_LEN_KEY );
        	 break;
         case 48:
        	 strcat( output, TRIPLE_LEN_KEY );
        	 break;
         default :
        	 strcat( output, TRIPLE_LEN_KEY );
        	 break;
      }
      strcat( output, auth_tx.TLF01_details.source_key );
   }
   else
   {
      racal_log_message( 2, 2, "PIN-TRANX source_key blank.","racal_TranlatePin_Keyblock_VISA",1 );
      return(TRANSACTION_NOT_VERIFIED);
   }

   /* Add the destination TPK or ZPK to the message.*/
   len = strlen( auth_tx.TLF01_details.dest_key );
   if ( len > 0 )
   {
      switch( len )
      {
         case 32:
        	 strcat( output, DOUBLE_LEN_KEY );
        	 break;
         case 48:
        	 strcat( output, TRIPLE_LEN_KEY );
        	 break;
         default :
        	 strcat( output, TRIPLE_LEN_KEY );
        	 break;
      }
      strcat( output, auth_tx.TLF01_details.dest_key );
   }
   else
   {
      racal_log_message( 2, 2, "PIN-TRANX dest_key blank.","racal_TranlatePin_Keyblock_VISA",1 );
      return(TRANSACTION_NOT_VERIFIED);
   }

   /*add the max pin length*/
   strcat(output,"12");

   /*add the pin block to the message.*/
   if(strlen(auth_tx.TLF01_details.pin_block) != 0)
   {
	   if((0 == strncmp(auth_tx.TLF01_details.issuer_id,"nccup3",5))
		   || 		  (0 == strncmp(auth_tx.TLF01_details.issuer_id,"ncdci",5))
		  )
	   {
			/*we should send ASCII value.COnvert binary into ACSII*/
			blen = 8;
			astr_len =16;
			genutil_bin_to_ascii (auth_tx.TLF01_details.pin_block, blen, asciiPIN,astr_len);
			strcat(output,asciiPIN);

	   }
	   else
		   strcat(output,auth_tx.TLF01_details.pin_block);
   }
   else
   {
      racal_log_message( 2, 2, "PIN-TRANX pin_block blank.","racal_TranlatePin_Keyblock_VISA",1 );
      return(TRANSACTION_NOT_VERIFIED);
   }
   /*add the next line because the net work handler send in a single character value.*/
   strcat(output,"0");

   /*add the source pin format to the message.*/
   if(strlen(auth_tx.TLF01_details.source_pin_format) != 0)
   {

	   strcat(output,auth_tx.TLF01_details.source_pin_format);
   }
   else
   {
      racal_log_message( 2, 2, "PIN-TRANX source_pin_format.","racal_TranlatePin_Keyblock_VISA",1 );
      return(TRANSACTION_NOT_VERIFIED);
   }
   /*add the next line because the net work handler send in a single character value. */
   strcat(output,"0");

   /*add the destination pin format to the message.*/
   if(strlen(auth_tx.TLF01_details.dest_pin_format) != 0)
      strcat(output,auth_tx.TLF01_details.dest_pin_format);
   else
   {
      racal_log_message( 2, 2, "PIN-TRANX dest_pin_format blank.","racal_TranlatePin_Keyblock_VISA",1 );
      return(TRANSACTION_NOT_VERIFIED);
   }


   /*farad 7/19/1999  Added the next line to take care of nccirs 12 digit pans*/
   if (  (0 == strcmp(auth_tx.TLF01_details.acquirer_id,"nccirs") ||
          0 == strcmp(auth_tx.TLF01_details.acquirer_id,"ncvisa") ||
          0 == strcmp(auth_tx.TLF01_details.acquirer_id,"ncvsms")) &&
         (strlen(auth_tx.TLF01_details.card_num) <= 12)
      )
   {
      memset(&dummy1,0,Dummy_Size);
      memset(&dummy,'\0',Dummy_Size);
      memset(&dummy,'0',Dummy_Size);
      strcpy(dummy + (16 - strlen(auth_tx.TLF01_details.card_num)),auth_tx.TLF01_details.card_num);

      strcpy(dummy1,dummy + (strlen(dummy) - 13));
      dummy1[12] = '\0';
      strcat(output,dummy1);
   }
   else if(strlen(auth_tx.TLF01_details.card_num) > 12 )
   {
      /*take the 12 right most digit from the account number.*/
      memset(&dummy,0,Dummy_Size);
      strcpy(dummy,auth_tx.TLF01_details.card_num + (strlen(auth_tx.TLF01_details.card_num) - 13));
      dummy[12] = '\0';
      strcat(output,dummy);
   }
   else
   {
      racal_log_message( 2, 2, "PIN-TRANX card_num < 12.","racal_TranlatePin_Keyblock_VISA",1 );
      return (TRANSACTION_NOT_VERIFIED);
   }

   /*if the message lenght is more then 150 there is some thing wrong. */
   if (strlen(output) > 150)
   {
      racal_log_message( 2, 2, "PIN_TRANX Cmd Msg to long.","racal_TranlatePin_Keyblock_VISA",1 );
      return(TRANSACTION_NOT_VERIFIED);
   }
   else
   {
	   sprintf(strTemp, " input of racal CC: %s", output);
	   racal_log_message( 2, 2,strTemp,"racal_TranlatePin_Keyblock_VISA",1 );
   }

   /*send the message to the racal box. */
   if (racalSendToPort(output) == TRANSACTION_VERIFIED)
   {
      memset(&dummy,0,Dummy_Size);
      strncpy(dummy,returnString,4);
      sprintf(strTemp, " output of racal cmnd CD: %s", returnString);
      racal_log_message( 2, 2,strTemp,"racal_TranlatePin_Keyblock_VISA",1 );
      /*check the value of the msg header */
      if(strcmp(msgHeader,dummy) != 0)
      {
         racal_log_message( 2, 2, "PIN-TRANX Msg header mis-match.","racal_TranlatePin_Keyblock_VISA",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      /*check the value of the command code. */
      memset(&dummy,0,Dummy_Size);
      strncpy(dummy,returnString + 4,2);
      if ((strcmp(dummy,"CB") != 0) && (strcmp(dummy,"CD") != 0) )
      {
         racal_log_message( 2, 2, "PIN-TRANX cmd code mis-match.","racal_TranlatePin_Keyblock_VISA",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      memset(&dummy,0,Dummy_Size);
      strncpy(dummy,returnString + 6,2);
      if(strcmp(dummy,"00") == 0)
      {
         memset(&dummy,0,Dummy_Size);
         strncpy(dummy,returnString + 10, 16);
         strcpy(auth_tx.TLF01_details.pin_block,dummy);

#ifdef _DEBUG
         racal_log_message( 2, 1, "PPIN-TRANX No problemo.","racal_TranlatePin_Keyblock_VISA",1 );
#endif
         return(TRANSACTION_VERIFIED);
      }

      if(strcmp(dummy,"01") == 0)
      {
         racal_log_message( 2, 2, "PIN-TRANX failure","racal_TranlatePin_Keyblock_VISA",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"10") == 0)
      {
         racal_log_message( 2, 2, "PIN-TRANSLATE TPK parity error.","racal_TranlatePin_Keyblock_VISA",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"11") == 0)
      {
         racal_log_message( 2, 2, "PIN-TRANSLATE Dest ZPK parity error.","racal_TranlatePin_Keyblock_VISA",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"12") == 0)
      {
         racal_log_message( 2, 2, "PIN-TRANSLATE No keys laoded in user storage.","racal_TranlatePin_Keyblock_VISA",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"13") == 0)
      {
         racal_log_message( 2, 2, "PIN-TRANSLATE LMK error report to supervisor.","racal_TranlatePin_Keyblock_VISA",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"15") == 0)
      {
         racal_log_message( 2, 2, "PIN-TRANSLATE Error in input data.","racal_TranlatePin_Keyblock_VISA",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"20") == 0)
      {
         racal_log_message( 2, 2, "PIN-TRANSLATE PIN block data Error.","racal_TranlatePin_Keyblock_VISA",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"21") == 0)
      {
         racal_log_message( 2, 2, "PIN-TRANSLATE Invalid user storage index.","racal_TranlatePin_Keyblock_VISA",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"22") == 0)
      {
         racal_log_message( 2, 2, "PIN-TRANSLATE Invalid Account number.","racal_TranlatePin_Keyblock_VISA",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"23") == 0)
      {
         racal_log_message( 2, 2, "PIN-TRANSLATE Invalid PIN block format code.","racal_TranlatePin_Keyblock_VISA",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"24") == 0)
      {
         racal_log_message( 2, 2, "PIN-TRANSLATE PIN is less than 4 or more than 12.","racal_TranlatePin_Keyblock_VISA",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      return(TRANSACTION_NOT_VERIFIED);

   }
   else
      return(TRANSACTION_NOT_VERIFIED);


}/*racal_TranlatePin_Keyblock_VISA*/
/**********************************************/
INT racal_TranlatePin_Keyblock_AMEX()
{
   INT   len = 0;
   CHAR  msgHeader[5]={0};
   CHAR  cmdCode[3]  ={0};
   CHAR  output[200] ={0};
   CHAR  strTemp[256]={0};

   BYTE asciiPIN[20]={0};
   INT  blen=0 ;
   INT  astr_len=0;

   memset (&msgHeader,0,sizeof(msgHeader));
   memset (&cmdCode,0,sizeof(cmdCode));
   memset (&output,0,sizeof(output));
   memset (&returnString,0,ReturnString_Size);

   strcpy (msgHeader ,"5678");


   /*the value of "LO" represents terminal for origin.*/
   /*The value of "RO" represents network for origin.*/
   /*if(strcmp(auth_tx.BIN01_details.card_family,CARD_DINERS) == 0)
	   strcpy (cmdCode,"CA");
   else*/
	   if(strcmp(auth_tx.TLF01_details.dispensation.origin, "RO") == 0 ||
	  	 strcmp(auth_tx.BIN01_details.card_family,CARD_CUP) == 0 ||
		 0 == strncmp(auth_tx.TLF01_details.product_code,CUP_CARD_INDICATOR_FLAG,2)
		 )
         strcpy (cmdCode,"CC");
	  else if(strcmp(auth_tx.TLF01_details.dispensation.origin, "LO") == 0)
      strcpy (cmdCode,"CA");

      else
      {
         racal_log_message( 2, 2, "PIN-TRANX dispen-origin mis-match.","racal_TranlatePin_Keyblock_AMEX",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

   strcpy(output,msgHeader);
   strcat(output,cmdCode);

   /* Add the source TPK or ZPK to the message. */
   len = strlen( auth_tx.TLF01_details.source_key );
   if ( len > 0 )
   {
      switch( len )
      {
         case 32:
        	 strcat( output, DOUBLE_LEN_KEY );
        	 break;
         case 48:
        	 strcat( output, TRIPLE_LEN_KEY );
        	 break;
         default :
        	 strcat( output, TRIPLE_LEN_KEY );
        	 break;
      }
      strcat( output, auth_tx.TLF01_details.source_key );
   }
   else
   {
      racal_log_message( 2, 2, "PIN-TRANX source_key blank.","racal_TranlatePin_Keyblock_AMEX",1 );
      return(TRANSACTION_NOT_VERIFIED);
   }

   /* Add the destination TPK or ZPK to the message.*/
   len = strlen( auth_tx.TLF01_details.dest_key );
   if ( len > 0 )
   {
      switch( len )
      {
         case 32:
        	 strcat( output, DOUBLE_LEN_KEY );
        	 break;
         case 48:
        	 strcat( output, TRIPLE_LEN_KEY );
        	 break;
         default :
        	 strcat( output, TRIPLE_LEN_KEY );
        	 break;
      }
      strcat( output, auth_tx.TLF01_details.dest_key );
   }
   else
   {
      racal_log_message( 2, 2, "PIN-TRANX dest_key blank.","racal_TranlatePin_Keyblock_AMEX",1 );
      return(TRANSACTION_NOT_VERIFIED);
   }

   /*add the max pin length*/
   strcat(output,"12");

   /*add the pin block to the message.*/
   if(strlen(auth_tx.TLF01_details.pin_block) != 0)
   {
	   if((0 == strncmp(auth_tx.TLF01_details.issuer_id,"nccup3",5))
		   || 		  (0 == strncmp(auth_tx.TLF01_details.issuer_id,"ncdci",5))
		  )
	   {
			/*we should send ASCII value.COnvert binary into ACSII*/
			blen = 8;
			astr_len =16;
			genutil_bin_to_ascii (auth_tx.TLF01_details.pin_block, blen, asciiPIN,astr_len);
			strcat(output,asciiPIN);

	   }
	   else
		   strcat(output,auth_tx.TLF01_details.pin_block);
   }
   else
   {
      racal_log_message( 2, 2, "PIN-TRANX pin_block blank.","racal_TranlatePin_Keyblock_AMEX",1 );
      return(TRANSACTION_NOT_VERIFIED);
   }
   /*add the next line because the net work handler send in a single character value.*/
   strcat(output,"0");

   /*add the source pin format to the message.*/
   if(strlen(auth_tx.TLF01_details.source_pin_format) != 0)
   {

	   strcat(output,auth_tx.TLF01_details.source_pin_format);
   }
   else
   {
      racal_log_message( 2, 2, "PIN-TRANX source_pin_format.","racal_TranlatePin_Keyblock_AMEX",1 );
      return(TRANSACTION_NOT_VERIFIED);
   }
   /*add the next line because the net work handler send in a single character value. */
   strcat(output,"0");

   /*add the destination pin format to the message.*/
   if(strlen(auth_tx.TLF01_details.dest_pin_format) != 0)
      strcat(output,auth_tx.TLF01_details.dest_pin_format);
   else
   {
      racal_log_message( 2, 2, "PIN-TRANX dest_pin_format blank.","racal_TranlatePin_Keyblock_AMEX",1 );
      return(TRANSACTION_NOT_VERIFIED);
   }


   /*farad 7/19/1999  Added the next line to take care of nccirs 12 digit pans*/
   if (  (0 == strcmp(auth_tx.TLF01_details.acquirer_id,"nccirs") ||
          0 == strcmp(auth_tx.TLF01_details.acquirer_id,"ncvisa") ||
          0 == strcmp(auth_tx.TLF01_details.acquirer_id,"ncvsms")) &&
         (strlen(auth_tx.TLF01_details.card_num) <= 12)
      )
   {
      memset(&dummy1,0,Dummy_Size);
      memset(&dummy,'\0',Dummy_Size);
      memset(&dummy,'0',Dummy_Size);
      strcpy(dummy + (16 - strlen(auth_tx.TLF01_details.card_num)),auth_tx.TLF01_details.card_num);

      strcpy(dummy1,dummy + (strlen(dummy) - 13));
      dummy1[12] = '\0';
      strcat(output,dummy1);
   }
   else if(strlen(auth_tx.TLF01_details.card_num) > 12 )
   {
      /*take the 12 right most digit from the account number.*/
      memset(&dummy,0,Dummy_Size);
      strcpy(dummy,auth_tx.TLF01_details.card_num + (strlen(auth_tx.TLF01_details.card_num) - 13));
      dummy[12] = '\0';
      strcat(output,dummy);
   }
   else
   {
      racal_log_message( 2, 2, "PIN-TRANX card_num < 12.","racal_TranlatePin_Keyblock_AMEX",1 );
      return (TRANSACTION_NOT_VERIFIED);
   }

   /*if the message lenght is more then 150 there is some thing wrong. */
   if (strlen(output) > 150)
   {
      racal_log_message( 2, 2, "PIN_TRANX Cmd Msg to long.","racal_TranlatePin_Keyblock_AMEX",1 );
      return(TRANSACTION_NOT_VERIFIED);
   }
   else
   {
	   sprintf(strTemp, " input of racal CC: %s", output);
	   racal_log_message( 2, 2,strTemp,"racal_TranlatePin_Keyblock_AMEX",1 );
   }

   /*send the message to the racal box. */
   if (racalSendToPort(output) == TRANSACTION_VERIFIED)
   {
      memset(&dummy,0,Dummy_Size);
      strncpy(dummy,returnString,4);
      sprintf(strTemp, " output of racal cmnd CD: %s", returnString);
      racal_log_message( 2, 2,strTemp,"racal_TranlatePin_Keyblock_AMEX",1 );
      /*check the value of the msg header */
      if(strcmp(msgHeader,dummy) != 0)
      {
         racal_log_message( 2, 2, "PIN-TRANX Msg header mis-match.","racal_TranlatePin_Keyblock_AMEX",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      /*check the value of the command code. */
      memset(&dummy,0,Dummy_Size);
      strncpy(dummy,returnString + 4,2);
      if ((strcmp(dummy,"CB") != 0) && (strcmp(dummy,"CD") != 0) )
      {
         racal_log_message( 2, 2, "PIN-TRANX cmd code mis-match.","racal_TranlatePin_Keyblock_AMEX",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      memset(&dummy,0,Dummy_Size);
      strncpy(dummy,returnString + 6,2);
      if(strcmp(dummy,"00") == 0)
      {
         memset(&dummy,0,Dummy_Size);
         strncpy(dummy,returnString + 10, 16);
         strcpy(auth_tx.TLF01_details.pin_block,dummy);

#ifdef _DEBUG
         racal_log_message( 2, 1, "PPIN-TRANX No problemo.","racal_TranlatePin_Keyblock_AMEX",1 );
#endif
         return(TRANSACTION_VERIFIED);
      }

      if(strcmp(dummy,"01") == 0)
      {
         racal_log_message( 2, 2, "PIN-TRANX failure","racal_TranlatePin_Keyblock_AMEX",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"10") == 0)
      {
         racal_log_message( 2, 2, "PIN-TRANSLATE TPK parity error.","racal_TranlatePin_Keyblock_AMEX",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"11") == 0)
      {
         racal_log_message( 2, 2, "PIN-TRANSLATE Dest ZPK parity error.","racal_TranlatePin_Keyblock_AMEX",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"12") == 0)
      {
         racal_log_message( 2, 2, "PIN-TRANSLATE No keys laoded in user storage.","racal_TranlatePin_Keyblock_AMEX",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"13") == 0)
      {
         racal_log_message( 2, 2, "PIN-TRANSLATE LMK error report to supervisor.","racal_TranlatePin_Keyblock_AMEX",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"15") == 0)
      {
         racal_log_message( 2, 2, "PIN-TRANSLATE Error in input data.","racal_TranlatePin_Keyblock_AMEX",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"20") == 0)
      {
         racal_log_message( 2, 2, "PIN-TRANSLATE PIN block data Error.","racal_TranlatePin_Keyblock_AMEX",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"21") == 0)
      {
         racal_log_message( 2, 2, "PIN-TRANSLATE Invalid user storage index.","racal_TranlatePin_Keyblock_AMEX",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"22") == 0)
      {
         racal_log_message( 2, 2, "PIN-TRANSLATE Invalid Account number.","racal_TranlatePin_Keyblock_AMEX",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"23") == 0)
      {
         racal_log_message( 2, 2, "PIN-TRANSLATE Invalid PIN block format code.","racal_TranlatePin_Keyblock_AMEX",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"24") == 0)
      {
         racal_log_message( 2, 2, "PIN-TRANSLATE PIN is less than 4 or more than 12.","racal_TranlatePin_Keyblock_AMEX",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      return(TRANSACTION_NOT_VERIFIED);

   }
   else
      return(TRANSACTION_NOT_VERIFIED);


}/*racal_TranlatePin_Keyblock_AMEX*/
/**********************************************/
INT racal_TranlatePin_Keyblock_JCB()
{
   INT   len;
   CHAR  msgHeader[5]={0};
   CHAR  cmdCode[3]  ={0};
   CHAR  output[200] ={0};
   CHAR  strTemp[256]={0};

   BYTE asciiPIN[20]={0};
   INT  blen=0 ;
   INT  astr_len=0;

   memset (&msgHeader,0,sizeof(msgHeader));
   memset (&cmdCode,0,sizeof(cmdCode));
   memset (&output,0,sizeof(output));
   memset (&returnString,0,ReturnString_Size);

   strcpy (msgHeader ,"5678");


   /*the value of "LO" represents terminal for origin.*/
   /*The value of "RO" represents network for origin.*/
   /*if(strcmp(auth_tx.BIN01_details.card_family,CARD_DINERS) == 0)
	   strcpy (cmdCode,"CA");
   else*/
	   if(strcmp(auth_tx.TLF01_details.dispensation.origin, "RO") == 0 ||
		 strcmp(auth_tx.BIN01_details.card_family,CARD_CUP) == 0 ||
		 0 == strncmp(auth_tx.TLF01_details.product_code,CUP_CARD_INDICATOR_FLAG,2)
		 )
		 strcpy (cmdCode,"CC");
	  else if(strcmp(auth_tx.TLF01_details.dispensation.origin, "LO") == 0)
	  strcpy (cmdCode,"CA");

	  else
	  {
		 racal_log_message( 2, 2, "PIN-TRANX dispen-origin mis-match.","racal_TranlatePin_Keyblock_JCB",1 );
		 return(TRANSACTION_NOT_VERIFIED);
	  }

   strcpy(output,msgHeader);
   strcat(output,cmdCode);

   /* Add the source TPK or ZPK to the message. */
   len = strlen( auth_tx.TLF01_details.source_key );
   if ( len > 0 )
   {
	  switch( len )
	  {
		 case 32:
			 strcat( output, DOUBLE_LEN_KEY );
			 break;
		 case 48:
			 strcat( output, TRIPLE_LEN_KEY );
			 break;
		 default :
			 strcat( output, TRIPLE_LEN_KEY );
			 break;
	  }
	  strcat( output, auth_tx.TLF01_details.source_key );
   }
   else
   {
	  racal_log_message( 2, 2, "PIN-TRANX source_key blank.","racal_TranlatePin_Keyblock_JCB",1 );
	  return(TRANSACTION_NOT_VERIFIED);
   }

   /* Add the destination TPK or ZPK to the message.*/
   len = strlen( auth_tx.TLF01_details.dest_key );
   if ( len > 0 )
   {
	  switch( len )
	  {
		 case 32:
			 strcat( output, DOUBLE_LEN_KEY );
			 break;
		 case 48:
			 strcat( output, TRIPLE_LEN_KEY );
			 break;
		 default :
			 strcat( output, TRIPLE_LEN_KEY );
			 break;
	  }
	  strcat( output, auth_tx.TLF01_details.dest_key );
   }
   else
   {
	  racal_log_message( 2, 2, "PIN-TRANX dest_key blank.","racal_TranlatePin_Keyblock_JCB",1 );
	  return(TRANSACTION_NOT_VERIFIED);
   }

   /*add the max pin length*/
   strcat(output,"12");

   /*add the pin block to the message.*/
   if(strlen(auth_tx.TLF01_details.pin_block) != 0)
   {
	   if((0 == strncmp(auth_tx.TLF01_details.issuer_id,"nccup3",5))
		   || 		  (0 == strncmp(auth_tx.TLF01_details.issuer_id,"ncdci",5))
		  )
	   {
			/*we should send ASCII value.COnvert binary into ACSII*/
			blen = 8;
			astr_len =16;
			genutil_bin_to_ascii (auth_tx.TLF01_details.pin_block, blen, asciiPIN,astr_len);
			strcat(output,asciiPIN);

	   }
	   else
		   strcat(output,auth_tx.TLF01_details.pin_block);
   }
   else
   {
	  racal_log_message( 2, 2, "PIN-TRANX pin_block blank.","racal_TranlatePin_Keyblock_JCB",1 );
	  return(TRANSACTION_NOT_VERIFIED);
   }
   /*add the next line because the net work handler send in a single character value.*/
   strcat(output,"0");

   /*add the source pin format to the message.*/
   if(strlen(auth_tx.TLF01_details.source_pin_format) != 0)
   {

	   strcat(output,auth_tx.TLF01_details.source_pin_format);
   }
   else
   {
	  racal_log_message( 2, 2, "PIN-TRANX source_pin_format.","racal_TranlatePin_Keyblock_JCB",1 );
	  return(TRANSACTION_NOT_VERIFIED);
   }
   /*add the next line because the net work handler send in a single character value. */
   strcat(output,"0");

   /*add the destination pin format to the message.*/
   if(strlen(auth_tx.TLF01_details.dest_pin_format) != 0)
	  strcat(output,auth_tx.TLF01_details.dest_pin_format);
   else
   {
	  racal_log_message( 2, 2, "PIN-TRANX dest_pin_format blank.","racal_TranlatePin_Keyblock_JCB",1 );
	  return(TRANSACTION_NOT_VERIFIED);
   }


   /*farad 7/19/1999  Added the next line to take care of nccirs 12 digit pans*/
   if (  (0 == strcmp(auth_tx.TLF01_details.acquirer_id,"nccirs") ||
		  0 == strcmp(auth_tx.TLF01_details.acquirer_id,"ncvisa") ||
		  0 == strcmp(auth_tx.TLF01_details.acquirer_id,"ncvsms")) &&
		 (strlen(auth_tx.TLF01_details.card_num) <= 12)
	  )
   {
	  memset(&dummy1,0,Dummy_Size);
	  memset(&dummy,'\0',Dummy_Size);
	  memset(&dummy,'0',Dummy_Size);
	  strcpy(dummy + (16 - strlen(auth_tx.TLF01_details.card_num)),auth_tx.TLF01_details.card_num);

	  strcpy(dummy1,dummy + (strlen(dummy) - 13));
	  dummy1[12] = '\0';
	  strcat(output,dummy1);
   }
   else if(strlen(auth_tx.TLF01_details.card_num) > 12 )
   {
	  /*take the 12 right most digit from the account number.*/
	  memset(&dummy,0,Dummy_Size);
	  strcpy(dummy,auth_tx.TLF01_details.card_num + (strlen(auth_tx.TLF01_details.card_num) - 13));
	  dummy[12] = '\0';
	  strcat(output,dummy);
   }
   else
   {
	  racal_log_message( 2, 2, "PIN-TRANX card_num < 12.","racal_TranlatePin_Keyblock_JCB",1 );
	  return (TRANSACTION_NOT_VERIFIED);
   }

   /*if the message lenght is more then 150 there is some thing wrong. */
   if (strlen(output) > 150)
   {
	  racal_log_message( 2, 2, "PIN_TRANX Cmd Msg to long.","racal_TranlatePin_Keyblock_JCB",1 );
	  return(TRANSACTION_NOT_VERIFIED);
   }
   else
   {
	   sprintf(strTemp, " input of racal CC: %s", output);
	   racal_log_message( 2, 2,strTemp,"racalEquitTranlatePin",1 );
   }

   /*send the message to the racal box. */
   if (racalSendToPort(output) == TRANSACTION_VERIFIED)
   {
	  memset(&dummy,0,Dummy_Size);
	  strncpy(dummy,returnString,4);
	  sprintf(strTemp, " output of racal cmnd CD: %s", returnString);
	  racal_log_message( 2, 2,strTemp,"racalEquitTranlatePin",1 );
	  /*check the value of the msg header */
	  if(strcmp(msgHeader,dummy) != 0)
	  {
		 racal_log_message( 2, 2, "PIN-TRANX Msg header mis-match.","racal_TranlatePin_Keyblock_JCB",1 );
		 return(TRANSACTION_NOT_VERIFIED);
	  }

	  /*check the value of the command code. */
	  memset(&dummy,0,Dummy_Size);
	  strncpy(dummy,returnString + 4,2);
	  if ((strcmp(dummy,"CB") != 0) && (strcmp(dummy,"CD") != 0) )
	  {
		 racal_log_message( 2, 2, "PIN-TRANX cmd code mis-match.","racal_TranlatePin_Keyblock_JCB",1 );
		 return(TRANSACTION_NOT_VERIFIED);
	  }

	  memset(&dummy,0,Dummy_Size);
	  strncpy(dummy,returnString + 6,2);
	  if(strcmp(dummy,"00") == 0)
	  {
		 memset(&dummy,0,Dummy_Size);
		 strncpy(dummy,returnString + 10, 16);
		 strcpy(auth_tx.TLF01_details.pin_block,dummy);

#ifdef _DEBUG
		 racal_log_message( 2, 1, "PPIN-TRANX No problemo.","racal_TranlatePin_Keyblock_JCB",1 );
#endif
		 return(TRANSACTION_VERIFIED);
	  }

	  if(strcmp(dummy,"01") == 0)
	  {
		 racal_log_message( 2, 2, "PIN-TRANX failure","racal_TranlatePin_Keyblock_JCB",1 );
		 return(TRANSACTION_NOT_VERIFIED);
	  }

	  if(strcmp(dummy,"10") == 0)
	  {
		 racal_log_message( 2, 2, "PIN-TRANSLATE TPK parity error.","racal_TranlatePin_Keyblock_JCB",1 );
		 return(TRANSACTION_NOT_VERIFIED);
	  }

	  if(strcmp(dummy,"11") == 0)
	  {
		 racal_log_message( 2, 2, "PIN-TRANSLATE Dest ZPK parity error.","racal_TranlatePin_Keyblock_JCB",1 );
		 return(TRANSACTION_NOT_VERIFIED);
	  }

	  if(strcmp(dummy,"12") == 0)
	  {
		 racal_log_message( 2, 2, "PIN-TRANSLATE No keys laoded in user storage.","racal_TranlatePin_Keyblock_JCB",1 );
		 return(TRANSACTION_NOT_VERIFIED);
	  }

	  if(strcmp(dummy,"13") == 0)
	  {
		 racal_log_message( 2, 2, "PIN-TRANSLATE LMK error report to supervisor.","racal_TranlatePin_Keyblock_JCB",1 );
		 return(TRANSACTION_NOT_VERIFIED);
	  }

	  if(strcmp(dummy,"15") == 0)
	  {
		 racal_log_message( 2, 2, "PIN-TRANSLATE Error in input data.","racal_TranlatePin_Keyblock_JCB",1 );
		 return(TRANSACTION_NOT_VERIFIED);
	  }

	  if(strcmp(dummy,"20") == 0)
	  {
		 racal_log_message( 2, 2, "PIN-TRANSLATE PIN block data Error.","racal_TranlatePin_Keyblock_JCB",1 );
		 return(TRANSACTION_NOT_VERIFIED);
	  }

	  if(strcmp(dummy,"21") == 0)
	  {
		 racal_log_message( 2, 2, "PIN-TRANSLATE Invalid user storage index.","racal_TranlatePin_Keyblock_JCB",1 );
		 return(TRANSACTION_NOT_VERIFIED);
	  }

	  if(strcmp(dummy,"22") == 0)
	  {
		 racal_log_message( 2, 2, "PIN-TRANSLATE Invalid Account number.","racal_TranlatePin_Keyblock_JCB",1 );
		 return(TRANSACTION_NOT_VERIFIED);
	  }

	  if(strcmp(dummy,"23") == 0)
	  {
		 racal_log_message( 2, 2, "PIN-TRANSLATE Invalid PIN block format code.","racal_TranlatePin_Keyblock_JCB",1 );
		 return(TRANSACTION_NOT_VERIFIED);
	  }

	  if(strcmp(dummy,"24") == 0)
	  {
		 racal_log_message( 2, 2, "PIN-TRANSLATE PIN is less than 4 or more than 12.","racal_TranlatePin_Keyblock_JCB",1 );
		 return(TRANSACTION_NOT_VERIFIED);
	  }

	  return(TRANSACTION_NOT_VERIFIED);

   }
   else
	  return(TRANSACTION_NOT_VERIFIED);

}/*racal_TranlatePin_Keyblock_JCB*/
/**********************************************/
INT racal_TranlatePin_Keyblock_UPI()
{
   INT   len;
   CHAR  msgHeader[5]={0};
   CHAR  cmdCode[3]  ={0};
   CHAR  output[200] ={0};
   CHAR  strTemp[256]={0};

   BYTE asciiPIN[20]={0};
   INT  blen=0 ;
   INT  astr_len=0;

   memset (&msgHeader,0,sizeof(msgHeader));
   memset (&cmdCode,0,sizeof(cmdCode));
   memset (&output,0,sizeof(output));
   memset (&returnString,0,ReturnString_Size);

   strcpy (msgHeader ,"5678");


   /*the value of "LO" represents terminal for origin.*/
   /*The value of "RO" represents network for origin.*/
   /*if(strcmp(auth_tx.BIN01_details.card_family,CARD_DINERS) == 0)
	   strcpy (cmdCode,"CA");
   else*/
	   if(strcmp(auth_tx.TLF01_details.dispensation.origin, "RO") == 0 ||
		 strcmp(auth_tx.BIN01_details.card_family,CARD_CUP) == 0 ||
		 0 == strncmp(auth_tx.TLF01_details.product_code,CUP_CARD_INDICATOR_FLAG,2)
		 )
		 strcpy (cmdCode,"CC");
	  else if(strcmp(auth_tx.TLF01_details.dispensation.origin, "LO") == 0)
	  strcpy (cmdCode,"CA");

	  else
	  {
		 racal_log_message( 2, 2, "PIN-TRANX dispen-origin mis-match.","racal_TranlatePin_Keyblock_UPI",1 );
		 return(TRANSACTION_NOT_VERIFIED);
	  }

   strcpy(output,msgHeader);
   strcat(output,cmdCode);

   /* Add the source TPK or ZPK to the message. */
   len = strlen( auth_tx.TLF01_details.source_key );
   if ( len > 0 )
   {
	  switch( len )
	  {
		 case 32:
			 strcat( output, DOUBLE_LEN_KEY );
			 break;
		 case 48:
			 strcat( output, TRIPLE_LEN_KEY );
			 break;
		 default :
			 strcat( output, TRIPLE_LEN_KEY );
			 break;
	  }
	  strcat( output, auth_tx.TLF01_details.source_key );
   }
   else
   {
	  racal_log_message( 2, 2, "PIN-TRANX source_key blank.","racal_TranlatePin_Keyblock_UPI",1 );
	  return(TRANSACTION_NOT_VERIFIED);
   }

   /* Add the destination TPK or ZPK to the message.*/
   len = strlen( auth_tx.TLF01_details.dest_key );
   if ( len > 0 )
   {
	  switch( len )
	  {
		 case 32:
			 strcat( output, DOUBLE_LEN_KEY );
			 break;
		 case 48:
			 strcat( output, TRIPLE_LEN_KEY );
			 break;
		 default :
			 strcat( output, TRIPLE_LEN_KEY );
			 break;
	  }
	  strcat( output, auth_tx.TLF01_details.dest_key );
   }
   else
   {
	  racal_log_message( 2, 2, "PIN-TRANX dest_key blank.","racal_TranlatePin_Keyblock_UPI",1 );
	  return(TRANSACTION_NOT_VERIFIED);
   }

   /*add the max pin length*/
   strcat(output,"12");

   /*add the pin block to the message.*/
   if(strlen(auth_tx.TLF01_details.pin_block) != 0)
   {
	   if((0 == strncmp(auth_tx.TLF01_details.issuer_id,"nccup3",5))
		   || 		  (0 == strncmp(auth_tx.TLF01_details.issuer_id,"ncdci",5))
		  )
	   {
			/*we should send ASCII value.COnvert binary into ACSII*/
			blen = 8;
			astr_len =16;
			genutil_bin_to_ascii (auth_tx.TLF01_details.pin_block, blen, asciiPIN,astr_len);
			strcat(output,asciiPIN);

	   }
	   else
		   strcat(output,auth_tx.TLF01_details.pin_block);
   }
   else
   {
	  racal_log_message( 2, 2, "PIN-TRANX pin_block blank.","racal_TranlatePin_Keyblock_UPI",1 );
	  return(TRANSACTION_NOT_VERIFIED);
   }
   /*add the next line because the net work handler send in a single character value.*/
   strcat(output,"0");

   /*add the source pin format to the message.*/
   if(strlen(auth_tx.TLF01_details.source_pin_format) != 0)
   {

	   strcat(output,auth_tx.TLF01_details.source_pin_format);
   }
   else
   {
	  racal_log_message( 2, 2, "PIN-TRANX source_pin_format.","racal_TranlatePin_Keyblock_UPI",1 );
	  return(TRANSACTION_NOT_VERIFIED);
   }
   /*add the next line because the net work handler send in a single character value. */
   strcat(output,"0");

   /*add the destination pin format to the message.*/
   if(strlen(auth_tx.TLF01_details.dest_pin_format) != 0)
	  strcat(output,auth_tx.TLF01_details.dest_pin_format);
   else
   {
	  racal_log_message( 2, 2, "PIN-TRANX dest_pin_format blank.","racal_TranlatePin_Keyblock_UPI",1 );
	  return(TRANSACTION_NOT_VERIFIED);
   }


   /*farad 7/19/1999  Added the next line to take care of nccirs 12 digit pans*/
   if (  (0 == strcmp(auth_tx.TLF01_details.acquirer_id,"nccirs") ||
		  0 == strcmp(auth_tx.TLF01_details.acquirer_id,"ncvisa") ||
		  0 == strcmp(auth_tx.TLF01_details.acquirer_id,"ncvsms")) &&
		 (strlen(auth_tx.TLF01_details.card_num) <= 12)
	  )
   {
	  memset(&dummy1,0,Dummy_Size);
	  memset(&dummy,'\0',Dummy_Size);
	  memset(&dummy,'0',Dummy_Size);
	  strcpy(dummy + (16 - strlen(auth_tx.TLF01_details.card_num)),auth_tx.TLF01_details.card_num);

	  strcpy(dummy1,dummy + (strlen(dummy) - 13));
	  dummy1[12] = '\0';
	  strcat(output,dummy1);
   }
   else if(strlen(auth_tx.TLF01_details.card_num) > 12 )
   {
	  /*take the 12 right most digit from the account number.*/
	  memset(&dummy,0,Dummy_Size);
	  strcpy(dummy,auth_tx.TLF01_details.card_num + (strlen(auth_tx.TLF01_details.card_num) - 13));
	  dummy[12] = '\0';
	  strcat(output,dummy);
   }
   else
   {
	  racal_log_message( 2, 2, "PIN-TRANX card_num < 12.","racal_TranlatePin_Keyblock_UPI",1 );
	  return (TRANSACTION_NOT_VERIFIED);
   }

   /*if the message lenght is more then 150 there is some thing wrong. */
   if (strlen(output) > 150)
   {
	  racal_log_message( 2, 2, "PIN_TRANX Cmd Msg to long.","racal_TranlatePin_Keyblock_UPI",1 );
	  return(TRANSACTION_NOT_VERIFIED);
   }
   else
   {
	   sprintf(strTemp, " input of racal CC: %s", output);
	   racal_log_message( 2, 2,strTemp,"racal_TranlatePin_Keyblock_UPI",1 );
   }

   /*send the message to the racal box. */
   if (racalSendToPort(output) == TRANSACTION_VERIFIED)
   {
	  memset(&dummy,0,Dummy_Size);
	  strncpy(dummy,returnString,4);
	  sprintf(strTemp, " output of racal cmnd CD: %s", returnString);
	  racal_log_message( 2, 2,strTemp,"racal_TranlatePin_Keyblock_UPI",1 );
	  /*check the value of the msg header */
	  if(strcmp(msgHeader,dummy) != 0)
	  {
		 racal_log_message( 2, 2, "PIN-TRANX Msg header mis-match.","racal_TranlatePin_Keyblock_UPI",1 );
		 return(TRANSACTION_NOT_VERIFIED);
	  }

	  /*check the value of the command code. */
	  memset(&dummy,0,Dummy_Size);
	  strncpy(dummy,returnString + 4,2);
	  if ((strcmp(dummy,"CB") != 0) && (strcmp(dummy,"CD") != 0) )
	  {
		 racal_log_message( 2, 2, "PIN-TRANX cmd code mis-match.","racal_TranlatePin_Keyblock_UPI",1 );
		 return(TRANSACTION_NOT_VERIFIED);
	  }

	  memset(&dummy,0,Dummy_Size);
	  strncpy(dummy,returnString + 6,2);
	  if(strcmp(dummy,"00") == 0)
	  {
		 memset(&dummy,0,Dummy_Size);
		 strncpy(dummy,returnString + 10, 16);
		 strcpy(auth_tx.TLF01_details.pin_block,dummy);

#ifdef _DEBUG
		 racal_log_message( 2, 1, "PPIN-TRANX No problemo.","racal_TranlatePin_Keyblock_UPI",1 );
#endif
		 return(TRANSACTION_VERIFIED);
	  }

	  if(strcmp(dummy,"01") == 0)
	  {
		 racal_log_message( 2, 2, "PIN-TRANX failure","racal_TranlatePin_Keyblock_UPI",1 );
		 return(TRANSACTION_NOT_VERIFIED);
	  }

	  if(strcmp(dummy,"10") == 0)
	  {
		 racal_log_message( 2, 2, "PIN-TRANSLATE TPK parity error.","racal_TranlatePin_Keyblock_UPI",1 );
		 return(TRANSACTION_NOT_VERIFIED);
	  }

	  if(strcmp(dummy,"11") == 0)
	  {
		 racal_log_message( 2, 2, "PIN-TRANSLATE Dest ZPK parity error.","racal_TranlatePin_Keyblock_UPI",1 );
		 return(TRANSACTION_NOT_VERIFIED);
	  }

	  if(strcmp(dummy,"12") == 0)
	  {
		 racal_log_message( 2, 2, "PIN-TRANSLATE No keys laoded in user storage.","racal_TranlatePin_Keyblock_UPI",1 );
		 return(TRANSACTION_NOT_VERIFIED);
	  }

	  if(strcmp(dummy,"13") == 0)
	  {
		 racal_log_message( 2, 2, "PIN-TRANSLATE LMK error report to supervisor.","racal_TranlatePin_Keyblock_UPI",1 );
		 return(TRANSACTION_NOT_VERIFIED);
	  }

	  if(strcmp(dummy,"15") == 0)
	  {
		 racal_log_message( 2, 2, "PIN-TRANSLATE Error in input data.","racal_TranlatePin_Keyblock_UPI",1 );
		 return(TRANSACTION_NOT_VERIFIED);
	  }

	  if(strcmp(dummy,"20") == 0)
	  {
		 racal_log_message( 2, 2, "PIN-TRANSLATE PIN block data Error.","racal_TranlatePin_Keyblock_UPI",1 );
		 return(TRANSACTION_NOT_VERIFIED);
	  }

	  if(strcmp(dummy,"21") == 0)
	  {
		 racal_log_message( 2, 2, "PIN-TRANSLATE Invalid user storage index.","racal_TranlatePin_Keyblock_UPI",1 );
		 return(TRANSACTION_NOT_VERIFIED);
	  }

	  if(strcmp(dummy,"22") == 0)
	  {
		 racal_log_message( 2, 2, "PIN-TRANSLATE Invalid Account number.","racal_TranlatePin_Keyblock_UPI",1 );
		 return(TRANSACTION_NOT_VERIFIED);
	  }

	  if(strcmp(dummy,"23") == 0)
	  {
		 racal_log_message( 2, 2, "PIN-TRANSLATE Invalid PIN block format code.","racal_TranlatePin_Keyblock_UPI",1 );
		 return(TRANSACTION_NOT_VERIFIED);
	  }

	  if(strcmp(dummy,"24") == 0)
	  {
		 racal_log_message( 2, 2, "PIN-TRANSLATE PIN is less than 4 or more than 12.","racal_TranlatePin_Keyblock_UPI",1 );
		 return(TRANSACTION_NOT_VERIFIED);
	  }

	  return(TRANSACTION_NOT_VERIFIED);

   }
   else
	  return(TRANSACTION_NOT_VERIFIED);


}/*racal_TranlatePin_Keyblock_UPI*/
/**********************************************/
INT racal_TranlatePin_Keyblock_DCI()
{
	INT   len = 0;
	CHAR  msgHeader[5]={0};
	CHAR  cmdCode[3]  ={0};
	CHAR  output[513] ={0};
	CHAR  strTemp[256]={0};
	CHAR  source_key[129]={0};
	CHAR  dest_key[129]={0};

	BYTE asciiPIN[20]={0};
	INT  blen=0 ;
	INT  astr_len=0;

	memset (&returnString,0,ReturnString_Size);

	strcpy (msgHeader ,"5678");

	if(strcmp(auth_tx.TLF01_details.dispensation.origin, "RO")==0)
	{
		strcpy (cmdCode,"CC");
	}
	else if(strcmp(auth_tx.TLF01_details.dispensation.origin, "LO") == 0)
	{
		strcpy (cmdCode,"CA");
	}
	else
	{
		racal_log_message( 2, 2, "PIN-TRANX dispen-origin mis-match.","racal_TranlatePin_Keyblock_DCI",1 );
		return(TRANSACTION_NOT_VERIFIED);
	}

   strcpy(output,msgHeader);
   strcat(output,cmdCode);

   /* Add the source TPK or ZPK to the message. */
   memcpy(source_key,Ncf01_Keyblock.iss_cwk_keyblock,    KEYBLOCK_KEY_LEN);
   memcpy(dest_key,  Ncf01_Keyblock.iss_rtdcwk_keyblock, KEYBLOCK_KEY_LEN);

   len = strlen( source_key );
   if ( len > 0 )
   {
	  strncat( output, SOUURCE_TPK_KEYBLOCK,KEYBLOCK_KEY_FORMAT_LEN );
	  strncat( output, source_key,KEYBLOCK_KEY_LEN );
   }
   else
   {
	  racal_log_message( 2, 2, "PIN-TRANX source_key blank.","racal_TranlatePin_Keyblock_DCI",1 );
	  return(TRANSACTION_NOT_VERIFIED);
   }

   /* Add the destination TPK or ZPK to the message.*/
   len = strlen( dest_key );
   if ( len > 0 )
   {
	   strncat( output, DESTINATION_TPK_KEYBLOCK,KEYBLOCK_KEY_FORMAT_LEN );
	   strncat( output, dest_key,KEYBLOCK_KEY_LEN );
   }
   else
   {
	  racal_log_message( 2, 2, "PIN-TRANX dest_key blank.","racal_TranlatePin_Keyblock_DCI",1 );
	  return(TRANSACTION_NOT_VERIFIED);
   }

   /*add the max pin length*/
   strcat(output,"12");

   /*add the pin block to the message.*/
   if(strlen(auth_tx.TLF01_details.pin_block) != 0)
   {
	   if((0 == strncmp(auth_tx.TLF01_details.issuer_id,"nccup3",5)) ||
		  (0 == strncmp(auth_tx.TLF01_details.issuer_id,"ncdci",5)) )
	   {
			/*we should send ASCII value.COnvert binary into ACSII*/
			blen = 8;
			astr_len =16;
			genutil_bin_to_ascii (auth_tx.TLF01_details.pin_block, blen, asciiPIN,astr_len);
			strcat(output,asciiPIN);

	   }
	   else
	   {
		   strcat(output,auth_tx.TLF01_details.pin_block);
	   }
   }
   else
   {
	  racal_log_message( 2, 2, "PIN-TRANX pin_block blank.","racal_TranlatePin_Keyblock_DCI",1 );
	  return(TRANSACTION_NOT_VERIFIED);
   }
   /*add the next line because the net work handler send in a single character value.*/
   strcat(output,"0");

   /*add the source pin format to the message.*/
   if(strlen(auth_tx.TLF01_details.source_pin_format) != 0)
   {
	   strcat(output,auth_tx.TLF01_details.source_pin_format);
   }
   else
   {
	  racal_log_message( 2, 2, "PIN-TRANX source_pin_format.","racal_TranlatePin_Keyblock_DCI",1 );
	  return(TRANSACTION_NOT_VERIFIED);
   }
   /*add the next line because the net work handler send in a single character value. */
   strcat(output,"0");

   /*add the destination pin format to the message.*/
   if(strlen(auth_tx.TLF01_details.dest_pin_format) != 0)
   {
	  strcat(output,auth_tx.TLF01_details.dest_pin_format);
   }
   else
   {
	  racal_log_message( 2, 2, "PIN-TRANX dest_pin_format blank.","racal_TranlatePin_Keyblock_DCI",1 );
	  return(TRANSACTION_NOT_VERIFIED);
   }

   if(strlen(auth_tx.TLF01_details.card_num) > 12 )
   {
	  /*take the 12 right most digit from the account number.*/
	  memset(&dummy,0,Dummy_Size);
	  strcpy(dummy,auth_tx.TLF01_details.card_num + (strlen(auth_tx.TLF01_details.card_num) - 13));
	  dummy[12] = '\0';
	  strcat(output,dummy);
   }
   else
   {
	  racal_log_message( 2, 2, "PIN-TRANX card_num < 12.","racal_TranlatePin_Keyblock_DCI",1 );
	  return (TRANSACTION_NOT_VERIFIED);
   }

   /*if the message lenght is more then 150 there is some thing wronMAX_KEYBLOCK_CMD_LENg. */
   if (strlen(output) > MAX_KEYBLOCK_CMD_LEN)
   {
	  racal_log_message( 2, 2, "PIN_TRANX Cmd Msg to long.","racal_TranlatePin_Keyblock_DCI",1 );
	  return(TRANSACTION_NOT_VERIFIED);
   }
   else
   {
	   sprintf(strTemp, " input of racal CC: %s", output);
	   racal_log_message( 2, 2,strTemp,"racal_TranlatePin_Keyblock_DCI",1 );
   }

   /*send the message to the racal box. */
   if (racalSendToPort(output) == TRANSACTION_VERIFIED)
   {
	  memset(&dummy,0,Dummy_Size);
	  strncpy(dummy,returnString,4);
	  sprintf(strTemp, " output of racal cmnd CD: %s", returnString);
	  racal_log_message( 2, 2,strTemp,"racal_TranlatePin_Keyblock_DCI",1 );
	  /*check the value of the msg header */
	  if(strcmp(msgHeader,dummy) != 0)
	  {
		 racal_log_message( 2, 2, "PIN-TRANX Msg header mis-match.","racal_TranlatePin_Keyblock_DCI",1 );
		 return(TRANSACTION_NOT_VERIFIED);
	  }

	  /*check the value of the command code. */
	  memset(&dummy,0,Dummy_Size);
	  strncpy(dummy,returnString + 4,2);
	  if ((strcmp(dummy,"CB") != 0) && (strcmp(dummy,"CD") != 0) )
	  {
		 racal_log_message( 2, 2, "PIN-TRANX cmd code mis-match.","racal_TranlatePin_Keyblock_DCI",1 );
		 return(TRANSACTION_NOT_VERIFIED);
	  }

	  memset(&dummy,0,Dummy_Size);
	  strncpy(dummy,returnString + 6,2);
	  if(strcmp(dummy,"00") == 0)
	  {
		 memset(&dummy,0,Dummy_Size);
		 strncpy(dummy,returnString + 10, 16);
		 strcpy(auth_tx.TLF01_details.pin_block,dummy);

#ifdef _DEBUG
		 racal_log_message( 2, 1, "PPIN-TRANX No problemo.","racal_TranlatePin_Keyblock_DCI",1 );
#endif
		 return(TRANSACTION_VERIFIED);
	  }

	  if(strcmp(dummy,"01") == 0)
	  {
		 racal_log_message( 2, 2, "PIN-TRANX failure","racal_TranlatePin_Keyblock_DCI",1 );
		 return(TRANSACTION_NOT_VERIFIED);
	  }

	  if(strcmp(dummy,"10") == 0)
	  {
		 racal_log_message( 2, 2, "PIN-TRANSLATE TPK parity error.","racal_TranlatePin_Keyblock_DCI",1 );
		 return(TRANSACTION_NOT_VERIFIED);
	  }

	  if(strcmp(dummy,"11") == 0)
	  {
		 racal_log_message( 2, 2, "PIN-TRANSLATE Dest ZPK parity error.","racal_TranlatePin_Keyblock_DCI",1 );
		 return(TRANSACTION_NOT_VERIFIED);
	  }

	  if(strcmp(dummy,"12") == 0)
	  {
		 racal_log_message( 2, 2, "PIN-TRANSLATE No keys laoded in user storage.","racal_TranlatePin_Keyblock_DCI",1 );
		 return(TRANSACTION_NOT_VERIFIED);
	  }

	  if(strcmp(dummy,"13") == 0)
	  {
		 racal_log_message( 2, 2, "PIN-TRANSLATE LMK error report to supervisor.","racal_TranlatePin_Keyblock_DCI",1 );
		 return(TRANSACTION_NOT_VERIFIED);
	  }

	  if(strcmp(dummy,"15") == 0)
	  {
		 racal_log_message( 2, 2, "PIN-TRANSLATE Error in input data.","racal_TranlatePin_Keyblock_DCI",1 );
		 return(TRANSACTION_NOT_VERIFIED);
	  }

	  if(strcmp(dummy,"20") == 0)
	  {
		 racal_log_message( 2, 2, "PIN-TRANSLATE PIN block data Error.","racal_TranlatePin_Keyblock_DCI",1 );
		 return(TRANSACTION_NOT_VERIFIED);
	  }

	  if(strcmp(dummy,"21") == 0)
	  {
		 racal_log_message( 2, 2, "PIN-TRANSLATE Invalid user storage index.","racal_TranlatePin_Keyblock_DCI",1 );
		 return(TRANSACTION_NOT_VERIFIED);
	  }

	  if(strcmp(dummy,"22") == 0)
	  {
		 racal_log_message( 2, 2, "PIN-TRANSLATE Invalid Account number.","racal_TranlatePin_Keyblock_DCI",1 );
		 return(TRANSACTION_NOT_VERIFIED);
	  }

	  if(strcmp(dummy,"23") == 0)
	  {
		 racal_log_message( 2, 2, "PIN-TRANSLATE Invalid PIN block format code.","racal_TranlatePin_Keyblock_DCI",1 );
		 return(TRANSACTION_NOT_VERIFIED);
	  }

	  if(strcmp(dummy,"24") == 0)
	  {
		 racal_log_message( 2, 2, "PIN-TRANSLATE PIN is less than 4 or more than 12.","racal_TranlatePin_Keyblock_DCI",1 );
		 return(TRANSACTION_NOT_VERIFIED);
	  }

	  return(TRANSACTION_NOT_VERIFIED);

   }
   else
	  return(TRANSACTION_NOT_VERIFIED);


}/*racal_TranlatePin_Keyblock_DCI*/
/**********************************************/

/****************************************************************/
INT racal_UPI_ExchangeKey_ZMK_LMK_Keyblock()
{
   INT   ret_val = TRANSACTION_NOT_VERIFIED;
   INT   len = 0;
   INT   i = 0;
   INT   racal_ret_code = 0;
   CHAR  key_len_indicator[2] ={0};
   CHAR  errbuf[100]  ={0};
   CHAR  msgHeader[5]={0};
   CHAR  cmdCode[3]={0};
   CHAR  output[200]={0};
   CHAR  delimiter[3]={0};
   CHAR  strTemp[256] = {0};

   memset( &output,       0x00, sizeof(output)    );
   memset( &returnString, 0x00, ReturnString_Size );
   strcpy(delimiter,";");
   strcpy( msgHeader ,"8543" );
   strcpy( cmdCode,   "FA"   );

   strcat( output, msgHeader );
   strcat( output, cmdCode   );

   len = strlen( auth_tx.TLF01_details.dest_key );
   if ( len < 16)
   {
      strcpy( errbuf, "Key Change: ZMK is < 16." );
   }
   else
   {
      switch( len )
      {
         case 32:
        	 strcat( output, DOUBLE_LEN_KEY );
        	 break;
         case 48:
        	 strcat( output, TRIPLE_LEN_KEY );
        	 break;
         default:
			strcat( output, TRIPLE_LEN_KEY );
			break;
      }
      strcat( output, auth_tx.TLF01_details.dest_key );

      len = strlen( auth_tx.TLF01_details.source_key );
      if ( len <= 0 )
      {
         strcpy( errbuf, "Key Change: Source Key (ZPK) is blank" );
      }
      else
      {
         switch( len )
         {
            case 32:
            	strcat( output, DOUBLE_LEN_KEY_X9_17 );
            	break;
            case 48:
            	strcat( output, TRIPLE_LEN_KEY_X9_17 );
            	break;
            default:
            	strcat( output, TRIPLE_LEN_KEY_X9_17 );
            	break;
         }
         strcat(output,auth_tx.TLF01_details.source_key);
         /*Specific code for PULSe - Phani*/
         if(strncmp(auth_tx.TLF01_details.handler_queue,"ncpul",4) == 0)
         {
        	 /*Atala variant*/
        	 strcat(output,"1");

        	/* strcat(output,delimiter);
        	 strcat(output,"0");
        	 strcat(output,"0");
        	 strcat(output,auth_tx.TLF01_details.nfi_name); KCV */

         }
         /* The command is now composed. Check the length then send it. */
         sprintf(strTemp, " input of racal FA: %s", output);
     	 racal_log_message( 2, 2,strTemp,"racalCUPExchangeKey_ZMK_LMK",1 );

         if ( strlen(output) > MAX_CMD_LEN )
         {
            sprintf( errbuf,
                    "Key Change: Cmd Msg too long: %d, max = %d",
                     len, MAX_CMD_LEN );
         }

         /* Send a message to the Racal box. */
         else if ( racalSendToPort(output) == TRANSACTION_VERIFIED )
         {
            /* The command was successfully sent and returned here.
             * Check the results.
             */
            memset( &dummy, 0x00, Dummy_Size );
            memcpy( dummy,  returnString, 4 );

            sprintf(strTemp, " output of racal FB: %s", output);
            racal_log_message( 2, 2,strTemp,"racalCUPExchangeKey_ZMK_LMK",1 );

            /* Check to see if the same header msg is back. Should be echoed. */
            if ( strcmp(msgHeader,dummy) != 0 )
            {
               sprintf( errbuf,
                       "Key Change: Msg Header mismatch. Sent = %s, Returned = %s",
                        msgHeader, dummy );
            }
            else
            {
               /* Message header was successfully echoed back to us.
                * Verify the Command Code.
                */
               memset( &dummy, 0x00, Dummy_Size );
               memcpy( dummy,  returnString + 4, 2 );

               if ( strcmp(dummy,"FB") != 0 )
               {
                  strcpy( errbuf, "Key Change: Cmd Code mismatch. Expected FB") ;
               }
               else
               {
                  /* Message Header and Command Code are good.
                   * Check the return code.
                   */
                  memset( &dummy,0,Dummy_Size );
                  strncpy( dummy,returnString + 6,2 );
                  racal_ret_code = atoi( dummy );

                  switch( racal_ret_code )
                  {
                     case 1:

                        /* This is a special error code.  It is only an advice.
                         * So we will post a warning, but then flow the code
                         * down through 'case 0' and treat this as if it was
                         * successful.
                         */
                        sprintf( errbuf,
                                "Key Change: retcode=%s, ZPK parity error, advice only",
                                 dummy );

                     case 0:  /* Successful Action - Get encrypted key. */

                        /* Determine length of key. */
                        memset( &dummy,0,Dummy_Size );
                        key_len_indicator[0] = returnString[8];

                        switch( key_len_indicator[0] )
                        {
                           case cDOUBLE_LEN_KEY:
                           case cDOUBLE_LEN_KEY_X9_17:

                              len = 32;
                              i   =  1;
                              break;

                           case cTRIPLE_LEN_KEY:
                           case cTRIPLE_LEN_KEY_X9_17:

                              len = 48;
                              i   =  1;
                              break;

                           default:
                              len = 16;
                              i   =  0;
                              break;
                        }

                        memcpy( dummy, returnString + 8 + i, len );
                        strcpy( auth_tx.TLF01_details.dest_key,dummy );
                        ret_val = TRANSACTION_VERIFIED;
                        break;

                     case 10:

                        sprintf( errbuf,
                                "Key Change: retcode=%s, ZMK parity error",
                                 dummy );
                        break;

                     case 11:

                        sprintf( errbuf,
                                "Key Change: retcode=%s, All zero ZPK with even parity.",
                                 dummy );
                        break;

                     case 12:

                        sprintf( errbuf,
                                "Key Change: retcode=%s, No keys loaded in user storage.",
                                 dummy );
                        break;

                     case 13:

                        sprintf( errbuf,
                                "Key Change: retcode=%s, LMK error; report to supervisor.",
                                 dummy );
                        break;

                     case 15:

                        sprintf( errbuf,
                                "Key Change: retcode=%s, Error in input data.",
                                 dummy );
                        break;

                     case 21:

                        sprintf( errbuf,
                                "Key Change: retcode=%s, Invalid user storage index.",
                                 dummy );
                        break;

                     default:

                        sprintf( errbuf,
                                "Key Change: retcode=%d, Unexpected return code.",
                                 dummy );
                        break;
                  }
               }
            }
         }
         else
         {
            strcpy( errbuf, "Key Change: Unable to send request to racal box.");
         }
      }
   }

   /* Is there an error message to log? */
   if ( errbuf[0] != 0x00 )
   {
      /* YES */
      if ( auth_tx.TLF01_details.terminal_id[0] == 0x00 )
      {
         racal_log_message( 2, 2,errbuf,"racalCUPExchangeKey_ZMK_LMK",1 );
      }
      else
      {
         racal_log_message( 2, 2,errbuf,"racalCUPExchangeKey_ZMK_LMK",1 );
      }
   }
   return( ret_val );

}/*end of racalEquitKeyExchangeKey*/


/******************************************************************/
/*
This Function is specific to Diners - to generate  the Diners key
ATP should be able generate the key with ZMK ,

Input  : ZMK given by Diner
Outpot:  1. generated key enc under LMK
		 2. generated key enc under ZMK

*/
/****************************************************************/
INT racal_Genarate_DinersKey_Keyblock()
{
   INT   ret_val = TRANSACTION_NOT_VERIFIED;
   INT   len;
   INT   i=0;
   INT   j=0;
   INT   k=0;
   INT   racal_ret_code;
   CHAR  key_len_indicator[2] = {0};
   CHAR  errbuf[100]  = {0};
   CHAR  msgHeader[5]= {0};
   CHAR  cmdCode[3]= {0};
   CHAR  output[200]= {0};
   char  delimiter[2]= {0};

   memset( &returnString, 0x00, ReturnString_Size );
   strcpy(delimiter,";");
   strcpy( msgHeader ,"1234" );
   strcpy( cmdCode,   "A0"   );

   strcat( output, msgHeader );
   strcat( output, cmdCode   );

   /* Mode 01 */
   strcat( output, "1"   );

   /* Key type : FFF For KEY BLOCK*/
   strncat( output, KEYLOCK_LMK_FOR_KEY_GEN, KEYLOCK_LMK_FOR_KEY_GEN_LEN );

   /* Key Scheme LMK Keyblock Format*/
   strncat( output, SOUURCE_TPK_KEYBLOCK,KEYBLOCK_KEY_FORMAT_LEN );

   /* ; delimiter */
   strcat( output, delimiter  );

   /* ZMK flag : 0 */
   strcat( output, "0"  );


   len = strlen( Ncf01_Keyblock.iss_cwk_keyblock );

   if ( len < KEYBLOCK_KEY_LEN)
   {
      strcpy( errbuf, "Key Change: ZMK is < 16." );
      racal_log_message( 2, 2,errbuf,"racal_Genarate_DinersKey_Keyblock",1 );
      return (TRANSACTION_NOT_VERIFIED);
   }
   else
   {
	   strncat( output, SOUURCE_TPK_KEYBLOCK,KEYBLOCK_KEY_FORMAT_LEN );
	   strcat ( output,  Ncf01_Keyblock.iss_cwk_keyblock  );
   }

   /* Key Scheme */
   strcat( output, "X"  );

		if ( strlen(output) > MAX_KEYBLOCK_CMD_LEN )
		{
			len = strlen(output);
			sprintf( errbuf,"Diners Key Gen: Cmd Msg too long: %d, max = %d",
			len, MAX_KEYBLOCK_CMD_LEN );
		}
		else if ( racalSendToPort(output) == TRANSACTION_VERIFIED )
		{
			memset( &dummy, 0x00, Dummy_Size );
			memcpy( dummy,  returnString, 4 );

			/* Check to see if the same header msg is back. Should be echoed. */
			if ( strcmp(msgHeader,dummy) != 0 )
			{
				sprintf( errbuf,
				"Diners Key Generate: Msg Header mismatch. Sent = %s, Returned = %s",
				msgHeader, dummy );
			} else
			{
				/* Message header was successfully echoed back to us.
				* Verify the Command Code.
				*/
				memset( &dummy, 0x00, Dummy_Size );
				memcpy( dummy,  returnString + 4, 2 );

				if ( strcmp(dummy,"A1") != 0 )
				{
				  strcpy( errbuf, " Diners Key generate: Cmd Code mismatch. Expected A1") ;
				}
				else
				{

				   /* Message Header and Command Code are good.
					* Check the return code.
					*/
				   memset( &dummy,0,Dummy_Size );
				   strncpy( dummy,returnString + 6,2 );
				   racal_ret_code = atoi( dummy );

				   switch( racal_ret_code )
				   {
					  case 1:

						 /* This is a special error code.  It is only an advice.
						  * So we will post a warning, but then flow the code
						  * down through 'case 0' and treat this as if it was
						  * successful.
						  */
						  sprintf( errbuf,"Diner Key Generate: retcode=%s, ZPK parity error, advice only",dummy );

					break;
					  case 0:  /* Successful Action - Get encrypted key. */

						 /* Determine length of key. */
						 memset( &dummy,0,Dummy_Size );
						 key_len_indicator[0] = returnString[8];

						 switch( key_len_indicator[0] )
						 {
							case cSOUURCE_KEK_KEYBLOCK:

								len = KEYBLOCK_KEY_LEN;
								i   =  1;
								memcpy( dummy, returnString + 8 + i, len );
								/*strcpy( auth_tx.TLF01_details.source_key,dummy );*/
								/*Store Source key 128 Byte coming from HSM*/
								strcpy( Ncf01_Keyblock.iss_rtdcwk_keyblock,dummy );
								memset( &dummy,0,Dummy_Size );
								j = 9+len;
								key_len_indicator[0] = returnString[j];

								memcpy( dummy, returnString + j + i, len );

								/*strcpy( auth_tx.TLF01_details.dest_key,dummy );*/
								/*Store 128 Byte Source key coming from HSM*/
								strcpy( Ncf01_Keyblock.iss_cwk_keyblock,dummy );
								k = j+i+len;
								/*memcpy( auth_tx.TLF01_details.kek,returnString+k,6 );*/
								/*Store 6 byte Keycheck Valuefrom HSM*/
								memcpy( Ncf01_Keyblock.iss_kek_keyblock,returnString+k,6 );

								ret_val = TRANSACTION_VERIFIED;
								break;

							default:

								 sprintf( errbuf,
										 "Diner Key Generate: retcode=%d, Unexpected return code.",
										  dummy );
							   break;
						 }
						 break;

					  case 10:

						 sprintf( errbuf,
								 "Diner Key Generate: retcode=%s, ZMK parity error",
								  dummy );
						 break;

					  case 11:

						 sprintf( errbuf,
								 "Diner Key Generate: retcode=%s, All zero ZPK with even parity.",
								  dummy );
						 break;

					  case 12:

						 sprintf( errbuf,
								 "Diner Key Generate: retcode=%s, No keys loaded in user storage.",
								  dummy );
						 break;

					  case 13:

						 sprintf( errbuf,
								 "Diner Key Generate: retcode=%s, LMK error; report to supervisor.",
								  dummy );
						 break;

						 default:

						 sprintf( errbuf,
								 "Diner Key Generate: retcode=%d, Unexpected return code.",
								  dummy );
						 break;
				   }

				}
			}
		}
		if ( errbuf[0] != 0x00 )
		{
			racal_log_message( 2, 2,errbuf,"racal_Genarate_DinersKey_Keyblock",1 );
			return (TRANSACTION_NOT_VERIFIED);
		}
   return( ret_val );

}/*end of racal_Genarate_DinersKey_Keyblock; */



/*********************************************************************/
INT racal_VerifyPin_MC_Keyblock()
{
   INT   len = 0;
   CHAR  msgHeader[5] = {0};
   CHAR  cmdCode[3] = {0};
   CHAR  output[200] = {0};

   memset (&msgHeader,0,sizeof(msgHeader));
   memset (&cmdCode,0,sizeof(cmdCode));
   memset (&output,0,sizeof(output));
   memset (&returnString,0,ReturnString_Size);

   strcpy (msgHeader ,"2468");

   /* The command is different for local versus network origin. */
   if(strcmp(auth_tx.TLF01_details.dispensation.origin, "LO") == 0)
   {
      strcpy (cmdCode,"DC");
   }
   else if(strcmp(auth_tx.TLF01_details.dispensation.origin, "RO") == 0)
   {
      strcpy (cmdCode,"EC");
   }
   else
   {
      racal_log_message( 2, 2, "PV: origin is unkown.","racal_VerifyPin_MC_Keyblock",1 );
      return(TRANSACTION_NOT_VERIFIED);
   }


   /* Add the command code and the message header.*/
   strcat(output,msgHeader);
   strcat(output,cmdCode);

   /* Add the tpk to the message.*/
   len = strlen( auth_tx.TLF01_details.source_key );
   if ( len > 0 )
   {
      switch( len )
      {
         case 32:
        	 strcat( output, DOUBLE_LEN_KEY );
        	 break;
         case 48:
        	 strcat( output, TRIPLE_LEN_KEY );
        	 break;
         default:
			 strcat( output, TRIPLE_LEN_KEY );
			 break;
      }
      strcat(output,auth_tx.TLF01_details.source_key);
   }
   else
   {
      racal_log_message( 2, 2, "PV: source_key is blank","racal_VerifyPin_MC_Keyblock",1 );
      return(TRANSACTION_NOT_VERIFIED);
   }

   /* Add the pvk pairs to the message.*/
   if (strlen(auth_tx.CAF01_details.pin.Visa.key_left) != 0)
      strcat(output,auth_tx.CAF01_details.pin.Visa.key_left);
   else
   {
      racal_log_message( 2, 2, "PV: Key_left is blank","racal_VerifyPin_MC_Keyblock",1 );
      return(TRANSACTION_NOT_VERIFIED);
   }

   /* Add the pvk pairs to the message.*/
   if (strlen(auth_tx.CAF01_details.pin.Visa.key_right) != 0)
      strcat(output,auth_tx.CAF01_details.pin.Visa.key_right);
   else
   {
      racal_log_message( 2, 2, "PV: Key_right is blank","racal_VerifyPin_MC_Keyblock",1 );
      return(TRANSACTION_NOT_VERIFIED);
   }


   /* Add the pin block to the message.*/
   if(strlen(auth_tx.TLF01_details.pin_block) != 0)
      strcat(output,auth_tx.TLF01_details.pin_block);
   else
   {
      racal_log_message( 2, 2, "PV: pin_block is blank","racal_VerifyPin_MC_Keyblock",1 );
      return(TRANSACTION_NOT_VERIFIED);
   }

   /*add the source pin format to the message.
   the values coming in are 0 to 6.  This field is 00, 01, 02, and so on.*/
   strcat(output,"0");
   if(strlen(auth_tx.TLF01_details.source_pin_format) != 0)
      strcat(output,auth_tx.TLF01_details.source_pin_format);
   else
   {
      racal_log_message( 2, 2, "PV: source pin format is blank","racal_VerifyPin_MC_Keyblock",1 );
      return(TRANSACTION_NOT_VERIFIED);
   }

   /*take the 12 right most digit from the account number.*/
   if(strlen(auth_tx.TLF01_details.card_num) >= 12 )
   {
      memset(&dummy,0,Dummy_Size);
      strcpy(dummy,auth_tx.TLF01_details.card_num + (strlen(auth_tx.TLF01_details.card_num) - 13));
      dummy[12] = '\0';
      strcat(output,dummy);
   }
   else
   {
      racal_log_message( 2, 2, "PV: card_num is < 12","racal_VerifyPin_MC_Keyblock",1 );
      return (TRANSACTION_NOT_VERIFIED);
   }

   /*look at the value of auth_tx.CAF01_details.pin_offset_location if it is "C" check the data server*/
   if(strcmp(auth_tx.CAF01_details.pin_offset_location,"C") == 0)
   {
      /*Get the PVKI and the PVV from the data server.
      add the PVKI value to the message.*/
      if(strlen(auth_tx.CCF02P_details.pvki) != 0)
         strcat(output,auth_tx.CCF02P_details.pvki);
      else
      {
         racal_log_message( 2, 2, "PV: pvki is blank","racal_VerifyPin_MC_Keyblock",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      /*add the PVV value to the message.*/
      if(strlen(auth_tx.CCF02P_details.pin_offset) != 0)
         strcat(output,auth_tx.CCF02P_details.pin_offset);
      else
      {
         racal_log_message( 2, 2, "PV: pin_offset is blank","racal_VerifyPin_MC_Keyblock",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }
   }
   else
      {
         /*Get the PVKI and the PVV from the TRACK 1 &or 2.*/
         if(strlen(auth_tx.TLF01_details.track1) != 0)
         {
            /*if you have made it this far, info is on track one.*/
            /*check track one.  make sure it is not empty.*/
            /*get the value of the PVKI from track one*/
            memset(&dummy,0,Dummy_Size);
            if (1 == parseTrackOne(auth_tx.TLF01_details.track1,dummy ,"^",
                     auth_tx.CAF01_details.pin.Visa.pvki_location_tr1, auth_tx.CAF01_details.pin.Visa.pvki_length_tr1))
            {
               strcat(output,dummy);
            }
            else
               return(TRANSACTION_NOT_VERIFIED);

            /*get the value of the PVV from track one.*/
            memset(&dummy,0,Dummy_Size);
            if (1 == parseTrackOne(auth_tx.TLF01_details.track1,dummy ,"^",
                     auth_tx.CAF01_details.pin.Visa.pvv_location_tr1, auth_tx.CAF01_details.pin.Visa.pvv_offset_tr1))
            {
               strcat(output,dummy);
            }
            else
               return(TRANSACTION_NOT_VERIFIED);
         }
         else
            if(strlen(auth_tx.TLF01_details.track2) != 0)
            {
               /*if you have made it this far, info is on track two.*/
               /*check track two.  make sure it is not empty.*/
               /*get the value of the PVKI from track one*/
               memset(&dummy,0,Dummy_Size);
               if (1 == parseTrackTwo(auth_tx.TLF01_details.track2,dummy ,"=",
                        auth_tx.CAF01_details.pin.Visa.pvki_location_tr2, auth_tx.CAF01_details.pin.Visa.pvki_length_tr2))
               {
                  strcat(output,dummy);
               }
               else
                  return(TRANSACTION_NOT_VERIFIED);

               /*get the value of the PVV from track one.*/
               memset(&dummy,0,Dummy_Size);
               if (1 == parseTrackTwo(auth_tx.TLF01_details.track2,dummy ,"=",
                        auth_tx.CAF01_details.pin.Visa.pvv_location_tr2, auth_tx.CAF01_details.pin.Visa.pvv_offset_tr2))
               {
                  strcat(output,dummy);
               }
               else
                  return(TRANSACTION_NOT_VERIFIED);
            }
            else
               {
                  racal_log_message( 2, 2, "PV: Track 1&2 blank","racal_VerifyPin_MC_Keyblock",1 );
                  return(TRANSACTION_NOT_VERIFIED);
               }
      }

   /*if the message lenght is more then 150 there is some thing wrong.*/
   if (strlen(output) > 150)
   {
      racal_log_message( 2, 2, "PV: Cmd Msg to long","racal_VerifyPin_MC_Keyblock",1 );
      return(TRANSACTION_NOT_VERIFIED);
   }

   if (racalSendToPort(output) == TRANSACTION_VERIFIED)
   {
      memset(&dummy,0,Dummy_Size);
      strncpy(dummy,returnString,4);

      /*check to see if the same header msg is back. */
      if(strcmp(msgHeader,dummy) != 0)
      {
         racal_log_message( 2, 2, "PV: Msg Header mis-match.","racal_VerifyPin_MC_Keyblock",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      memset(&dummy,0,Dummy_Size);
      strncpy(dummy,returnString + 4,2);

      /*check to see if the "ED" or "DD" response code is back.*/
      if ((strcmp(dummy,"DD") != 0) && (strcmp(dummy,"ED") != 0) )
      {
         racal_log_message( 2, 2, "PV: Cmd Code mis-match.","racal_VerifyPin_MC_Keyblock",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      memset(&dummy,0,Dummy_Size);
      strncpy(dummy,returnString + 6,2);

      if(strcmp(dummy,"00") == 0)
      {
#ifdef _DEBUG
         racal_log_message( 2, 1, "PIN-Verify PASSED.","racal_VerifyPin_MC_Keyblock",1 );
#endif
         return(TRANSACTION_VERIFIED);
      }
      if(strcmp(dummy,"01") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify PASSED.","racal_VerifyPin_MC_Keyblock",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }
      if(strcmp(dummy,"10") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify TPK parity error.","racal_VerifyPin_MC_Keyblock",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"11") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify PVK parity error.","racal_VerifyPin_MC_Keyblock",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"12") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify No keys laoded in user storage.","racal_VerifyPin_MC_Keyblock",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"13") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify LMK error report to supervisor.","racal_VerifyPin_MC_Keyblock",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"15") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify Error in input data.","racal_VerifyPin_MC_Keyblock",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"20") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify PIN block does not contain valid values.","racal_VerifyPin_MC_Keyblock",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"21") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify Invalid user storage index.","racal_VerifyPin_MC_Keyblock",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"23") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify Invalid PIN block format code.","racal_VerifyPin_MC_Keyblock",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"24") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify PIN is less than 4 or more than 12.","racal_VerifyPin_MC_Keyblock",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      return(TRANSACTION_NOT_VERIFIED);
   }
   else
      return(TRANSACTION_NOT_VERIFIED);


}/*racal_VerifyPin_MC_Keyblock*/


/*********************************************************************/
INT racal_VerifyPin_VISA_Keyblock()
{
   INT   len = 0;
   CHAR  msgHeader[5] = {0};
   CHAR  cmdCode[3] = {0};
   CHAR  output[200] = {0};

   memset (&msgHeader,0,sizeof(msgHeader));
   memset (&cmdCode,0,sizeof(cmdCode));
   memset (&output,0,sizeof(output));
   memset (&returnString,0,ReturnString_Size);

   strcpy (msgHeader ,"2468");

   /* The command is different for local versus network origin. */
   if(strcmp(auth_tx.TLF01_details.dispensation.origin, "LO") == 0)
   {
      strcpy (cmdCode,"DC");
   }
   else if(strcmp(auth_tx.TLF01_details.dispensation.origin, "RO") == 0)
   {
      strcpy (cmdCode,"EC");
   }
   else
   {
      racal_log_message( 2, 2, "PV: origin is unkown.","racal_VerifyPin_VISA_Keyblock",1 );
      return(TRANSACTION_NOT_VERIFIED);
   }


   /* Add the command code and the message header.*/
   strcat(output,msgHeader);
   strcat(output,cmdCode);

   /* Add the tpk to the message.*/
   len = strlen( auth_tx.TLF01_details.source_key );
   if ( len > 0 )
   {
      switch( len )
      {
         case 32:
        	 strcat( output, DOUBLE_LEN_KEY );
        	 break;
         case 48:
        	 strcat( output, TRIPLE_LEN_KEY );
        	 break;
         default:
			 strcat( output, TRIPLE_LEN_KEY );
			 break;
      }
      strcat(output,auth_tx.TLF01_details.source_key);
   }
   else
   {
      racal_log_message( 2, 2, "PV: source_key is blank","racal_VerifyPin_VISA_Keyblock",1 );
      return(TRANSACTION_NOT_VERIFIED);
   }

   /* Add the pvk pairs to the message.*/
   if (strlen(auth_tx.CAF01_details.pin.Visa.key_left) != 0)
      strcat(output,auth_tx.CAF01_details.pin.Visa.key_left);
   else
   {
      racal_log_message( 2, 2, "PV: Key_left is blank","racal_VerifyPin_VISA_Keyblock",1 );
      return(TRANSACTION_NOT_VERIFIED);
   }

   /* Add the pvk pairs to the message.*/
   if (strlen(auth_tx.CAF01_details.pin.Visa.key_right) != 0)
      strcat(output,auth_tx.CAF01_details.pin.Visa.key_right);
   else
   {
      racal_log_message( 2, 2, "PV: Key_right is blank","racal_VerifyPin_VISA_Keyblock",1 );
      return(TRANSACTION_NOT_VERIFIED);
   }


   /* Add the pin block to the message.*/
   if(strlen(auth_tx.TLF01_details.pin_block) != 0)
      strcat(output,auth_tx.TLF01_details.pin_block);
   else
   {
      racal_log_message( 2, 2, "PV: pin_block is blank","racal_VerifyPin_VISA_Keyblock",1 );
      return(TRANSACTION_NOT_VERIFIED);
   }

   /*add the source pin format to the message.
   the values coming in are 0 to 6.  This field is 00, 01, 02, and so on.*/
   strcat(output,"0");
   if(strlen(auth_tx.TLF01_details.source_pin_format) != 0)
      strcat(output,auth_tx.TLF01_details.source_pin_format);
   else
   {
      racal_log_message( 2, 2, "PV: source pin format is blank","racal_VerifyPin_VISA_Keyblock",1 );
      return(TRANSACTION_NOT_VERIFIED);
   }

   /*take the 12 right most digit from the account number.*/
   if(strlen(auth_tx.TLF01_details.card_num) >= 12 )
   {
      memset(&dummy,0,Dummy_Size);
      strcpy(dummy,auth_tx.TLF01_details.card_num + (strlen(auth_tx.TLF01_details.card_num) - 13));
      dummy[12] = '\0';
      strcat(output,dummy);
   }
   else
   {
      racal_log_message( 2, 2, "PV: card_num is < 12","racal_VerifyPin_VISA_Keyblock",1 );
      return (TRANSACTION_NOT_VERIFIED);
   }

   /*look at the value of auth_tx.CAF01_details.pin_offset_location if it is "C" check the data server*/
   if(strcmp(auth_tx.CAF01_details.pin_offset_location,"C") == 0)
   {
      /*Get the PVKI and the PVV from the data server.
      add the PVKI value to the message.*/
      if(strlen(auth_tx.CCF02P_details.pvki) != 0)
         strcat(output,auth_tx.CCF02P_details.pvki);
      else
      {
         racal_log_message( 2, 2, "PV: pvki is blank","racal_VerifyPin_VISA_Keyblock",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      /*add the PVV value to the message.*/
      if(strlen(auth_tx.CCF02P_details.pin_offset) != 0)
         strcat(output,auth_tx.CCF02P_details.pin_offset);
      else
      {
         racal_log_message( 2, 2, "PV: pin_offset is blank","racal_VerifyPin_VISA_Keyblock",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }
   }
   else
      {
         /*Get the PVKI and the PVV from the TRACK 1 &or 2.*/
         if(strlen(auth_tx.TLF01_details.track1) != 0)
         {
            /*if you have made it this far, info is on track one.*/
            /*check track one.  make sure it is not empty.*/
            /*get the value of the PVKI from track one*/
            memset(&dummy,0,Dummy_Size);
            if (1 == parseTrackOne(auth_tx.TLF01_details.track1,dummy ,"^",
                     auth_tx.CAF01_details.pin.Visa.pvki_location_tr1, auth_tx.CAF01_details.pin.Visa.pvki_length_tr1))
            {
               strcat(output,dummy);
            }
            else
               return(TRANSACTION_NOT_VERIFIED);

            /*get the value of the PVV from track one.*/
            memset(&dummy,0,Dummy_Size);
            if (1 == parseTrackOne(auth_tx.TLF01_details.track1,dummy ,"^",
                     auth_tx.CAF01_details.pin.Visa.pvv_location_tr1, auth_tx.CAF01_details.pin.Visa.pvv_offset_tr1))
            {
               strcat(output,dummy);
            }
            else
               return(TRANSACTION_NOT_VERIFIED);
         }
         else
            if(strlen(auth_tx.TLF01_details.track2) != 0)
            {
               /*if you have made it this far, info is on track two.*/
               /*check track two.  make sure it is not empty.*/
               /*get the value of the PVKI from track one*/
               memset(&dummy,0,Dummy_Size);
               if (1 == parseTrackTwo(auth_tx.TLF01_details.track2,dummy ,"=",
                        auth_tx.CAF01_details.pin.Visa.pvki_location_tr2, auth_tx.CAF01_details.pin.Visa.pvki_length_tr2))
               {
                  strcat(output,dummy);
               }
               else
                  return(TRANSACTION_NOT_VERIFIED);

               /*get the value of the PVV from track one.*/
               memset(&dummy,0,Dummy_Size);
               if (1 == parseTrackTwo(auth_tx.TLF01_details.track2,dummy ,"=",
                        auth_tx.CAF01_details.pin.Visa.pvv_location_tr2, auth_tx.CAF01_details.pin.Visa.pvv_offset_tr2))
               {
                  strcat(output,dummy);
               }
               else
                  return(TRANSACTION_NOT_VERIFIED);
            }
            else
               {
                  racal_log_message( 2, 2, "PV: Track 1&2 blank","racal_VerifyPin_VISA_Keyblock",1 );
                  return(TRANSACTION_NOT_VERIFIED);
               }
      }

   /*if the message lenght is more then 150 there is some thing wrong.*/
   if (strlen(output) > 150)
   {
      racal_log_message( 2, 2, "PV: Cmd Msg to long","racal_VerifyPin_VISA_Keyblock",1 );
      return(TRANSACTION_NOT_VERIFIED);
   }

   if (racalSendToPort(output) == TRANSACTION_VERIFIED)
   {
      memset(&dummy,0,Dummy_Size);
      strncpy(dummy,returnString,4);

      /*check to see if the same header msg is back. */
      if(strcmp(msgHeader,dummy) != 0)
      {
         racal_log_message( 2, 2, "PV: Msg Header mis-match.","racal_VerifyPin_VISA_Keyblock",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      memset(&dummy,0,Dummy_Size);
      strncpy(dummy,returnString + 4,2);

      /*check to see if the "ED" or "DD" response code is back.*/
      if ((strcmp(dummy,"DD") != 0) && (strcmp(dummy,"ED") != 0) )
      {
         racal_log_message( 2, 2, "PV: Cmd Code mis-match.","racal_VerifyPin_VISA_Keyblock",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      memset(&dummy,0,Dummy_Size);
      strncpy(dummy,returnString + 6,2);

      if(strcmp(dummy,"00") == 0)
      {
#ifdef _DEBUG
         racal_log_message( 2, 1, "PIN-Verify PASSED.","racal_VerifyPin_VISA_Keyblock",1 );
#endif
         return(TRANSACTION_VERIFIED);
      }
      if(strcmp(dummy,"01") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify PASSED.","racal_VerifyPin_VISA_Keyblock",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }
      if(strcmp(dummy,"10") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify TPK parity error.","racal_VerifyPin_VISA_Keyblock",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"11") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify PVK parity error.","racal_VerifyPin_VISA_Keyblock",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"12") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify No keys laoded in user storage.","racal_VerifyPin_VISA_Keyblock",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"13") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify LMK error report to supervisor.","racal_VerifyPin_VISA_Keyblock",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"15") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify Error in input data.","racal_VerifyPin_VISA_Keyblock",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"20") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify PIN block does not contain valid values.","racal_VerifyPin_VISA_Keyblock",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"21") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify Invalid user storage index.","racal_VerifyPin_VISA_Keyblock",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"23") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify Invalid PIN block format code.","racal_VerifyPin_VISA_Keyblock",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"24") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify PIN is less than 4 or more than 12.","racal_VerifyPin_VISA_Keyblock",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      return(TRANSACTION_NOT_VERIFIED);
   }
   else
      return(TRANSACTION_NOT_VERIFIED);


}/*racal_VerifyPin_VISA_Keyblock*/

/*********************************************************************/
INT racal_VerifyPin_AMEX_Keyblock()
{
   INT   len = 0;
   CHAR  msgHeader[5] = {0};
   CHAR  cmdCode[3] = {0};
   CHAR  output[512] = {0};

   memset (&msgHeader,0,sizeof(msgHeader));
   memset (&cmdCode,0,sizeof(cmdCode));
   memset (&output,0,sizeof(output));
   memset (&returnString,0,ReturnString_Size);

   strcpy (msgHeader ,"2468");

   /* The command is different for local versus network origin. */
   if(strcmp(auth_tx.TLF01_details.dispensation.origin, "LO") == 0)
   {
      strcpy (cmdCode,"DC");
   }
   else if(strcmp(auth_tx.TLF01_details.dispensation.origin, "RO") == 0)
   {
      strcpy (cmdCode,"EC");
   }
   else
   {
      racal_log_message( 2, 2, "PV: origin is unkown.","racal_VerifyPin_AMEX_Keyblock",1 );
      return(TRANSACTION_NOT_VERIFIED);
   }


   /* Add the command code and the message header.*/
   strcat(output,msgHeader);
   strcat(output,cmdCode);

   /* Add the tpk to the message.*/
   len = strlen( auth_tx.TLF01_details.source_key );
   if ( len > 0 )
   {
      switch( len )
      {
         case 32:
        	 strcat( output, DOUBLE_LEN_KEY );
        	 break;
         case 48:
        	 strcat( output, TRIPLE_LEN_KEY );
        	 break;
         default:
			 strcat( output, TRIPLE_LEN_KEY );
			 break;
      }
      strcat(output,auth_tx.TLF01_details.source_key);
   }
   else
   {
      racal_log_message( 2, 2, "PV: source_key is blank","racal_VerifyPin_AMEX_Keyblock",1 );
      return(TRANSACTION_NOT_VERIFIED);
   }

   /* Add the pvk pairs to the message.*/
   if (strlen(auth_tx.CAF01_details.pin.Visa.key_left) != 0)
      strcat(output,auth_tx.CAF01_details.pin.Visa.key_left);
   else
   {
      racal_log_message( 2, 2, "PV: Key_left is blank","racal_VerifyPin_AMEX_Keyblock",1 );
      return(TRANSACTION_NOT_VERIFIED);
   }

   /* Add the pvk pairs to the message.*/
   if (strlen(auth_tx.CAF01_details.pin.Visa.key_right) != 0)
      strcat(output,auth_tx.CAF01_details.pin.Visa.key_right);
   else
   {
      racal_log_message( 2, 2, "PV: Key_right is blank","racal_VerifyPin_AMEX_Keyblock",1 );
      return(TRANSACTION_NOT_VERIFIED);
   }


   /* Add the pin block to the message.*/
   if(strlen(auth_tx.TLF01_details.pin_block) != 0)
      strcat(output,auth_tx.TLF01_details.pin_block);
   else
   {
      racal_log_message( 2, 2, "PV: pin_block is blank","racal_VerifyPin_AMEX_Keyblock",1 );
      return(TRANSACTION_NOT_VERIFIED);
   }

   /*add the source pin format to the message.
   the values coming in are 0 to 6.  This field is 00, 01, 02, and so on.*/
   strcat(output,"0");
   if(strlen(auth_tx.TLF01_details.source_pin_format) != 0)
      strcat(output,auth_tx.TLF01_details.source_pin_format);
   else
   {
      racal_log_message( 2, 2, "PV: source pin format is blank","racal_VerifyPin_AMEX_Keyblock",1 );
      return(TRANSACTION_NOT_VERIFIED);
   }

   /*take the 12 right most digit from the account number.*/
   if(strlen(auth_tx.TLF01_details.card_num) >= 12 )
   {
      memset(&dummy,0,Dummy_Size);
      strcpy(dummy,auth_tx.TLF01_details.card_num + (strlen(auth_tx.TLF01_details.card_num) - 13));
      dummy[12] = '\0';
      strcat(output,dummy);
   }
   else
   {
      racal_log_message( 2, 2, "PV: card_num is < 12","racal_VerifyPin_AMEX_Keyblock",1 );
      return (TRANSACTION_NOT_VERIFIED);
   }

   /*look at the value of auth_tx.CAF01_details.pin_offset_location if it is "C" check the data server*/
   if(strcmp(auth_tx.CAF01_details.pin_offset_location,"C") == 0)
   {
      /*Get the PVKI and the PVV from the data server.
      add the PVKI value to the message.*/
      if(strlen(auth_tx.CCF02P_details.pvki) != 0)
         strcat(output,auth_tx.CCF02P_details.pvki);
      else
      {
         racal_log_message( 2, 2, "PV: pvki is blank","racal_VerifyPin_AMEX_Keyblock",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      /*add the PVV value to the message.*/
      if(strlen(auth_tx.CCF02P_details.pin_offset) != 0)
         strcat(output,auth_tx.CCF02P_details.pin_offset);
      else
      {
         racal_log_message( 2, 2, "PV: pin_offset is blank","racal_VerifyPin_AMEX_Keyblock",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }
   }
   else
      {
         /*Get the PVKI and the PVV from the TRACK 1 &or 2.*/
         if(strlen(auth_tx.TLF01_details.track1) != 0)
         {
            /*if you have made it this far, info is on track one.*/
            /*check track one.  make sure it is not empty.*/
            /*get the value of the PVKI from track one*/
            memset(&dummy,0,Dummy_Size);
            if (1 == parseTrackOne(auth_tx.TLF01_details.track1,dummy ,"^",
                     auth_tx.CAF01_details.pin.Visa.pvki_location_tr1, auth_tx.CAF01_details.pin.Visa.pvki_length_tr1))
            {
               strcat(output,dummy);
            }
            else
               return(TRANSACTION_NOT_VERIFIED);

            /*get the value of the PVV from track one.*/
            memset(&dummy,0,Dummy_Size);
            if (1 == parseTrackOne(auth_tx.TLF01_details.track1,dummy ,"^",
                     auth_tx.CAF01_details.pin.Visa.pvv_location_tr1, auth_tx.CAF01_details.pin.Visa.pvv_offset_tr1))
            {
               strcat(output,dummy);
            }
            else
               return(TRANSACTION_NOT_VERIFIED);
         }
         else
            if(strlen(auth_tx.TLF01_details.track2) != 0)
            {
               /*if you have made it this far, info is on track two.*/
               /*check track two.  make sure it is not empty.*/
               /*get the value of the PVKI from track one*/
               memset(&dummy,0,Dummy_Size);
               if (1 == parseTrackTwo(auth_tx.TLF01_details.track2,dummy ,"=",
                        auth_tx.CAF01_details.pin.Visa.pvki_location_tr2, auth_tx.CAF01_details.pin.Visa.pvki_length_tr2))
               {
                  strcat(output,dummy);
               }
               else
                  return(TRANSACTION_NOT_VERIFIED);

               /*get the value of the PVV from track one.*/
               memset(&dummy,0,Dummy_Size);
               if (1 == parseTrackTwo(auth_tx.TLF01_details.track2,dummy ,"=",
                        auth_tx.CAF01_details.pin.Visa.pvv_location_tr2, auth_tx.CAF01_details.pin.Visa.pvv_offset_tr2))
               {
                  strcat(output,dummy);
               }
               else
                  return(TRANSACTION_NOT_VERIFIED);
            }
            else
               {
                  racal_log_message( 2, 2, "PV: Track 1&2 blank","racal_VerifyPin_AMEX_Keyblock",1 );
                  return(TRANSACTION_NOT_VERIFIED);
               }
      }

   /*if the message lenght is more then 150 there is some thing wrong.*/
   if (strlen(output) > 150)
   {
      racal_log_message( 2, 2, "PV: Cmd Msg to long","racal_VerifyPin_AMEX_Keyblock",1 );
      return(TRANSACTION_NOT_VERIFIED);
   }

   if (racalSendToPort(output) == TRANSACTION_VERIFIED)
   {
      memset(&dummy,0,Dummy_Size);
      strncpy(dummy,returnString,4);

      /*check to see if the same header msg is back. */
      if(strcmp(msgHeader,dummy) != 0)
      {
         racal_log_message( 2, 2, "PV: Msg Header mis-match.","racal_VerifyPin_AMEX_Keyblock",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      memset(&dummy,0,Dummy_Size);
      strncpy(dummy,returnString + 4,2);

      /*check to see if the "ED" or "DD" response code is back.*/
      if ((strcmp(dummy,"DD") != 0) && (strcmp(dummy,"ED") != 0) )
      {
         racal_log_message( 2, 2, "PV: Cmd Code mis-match.","racal_VerifyPin_AMEX_Keyblock",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      memset(&dummy,0,Dummy_Size);
      strncpy(dummy,returnString + 6,2);

      if(strcmp(dummy,"00") == 0)
      {
#ifdef _DEBUG
         racal_log_message( 2, 1, "PIN-Verify PASSED.","racal_VerifyPin_AMEX_Keyblock",1 );
#endif
         return(TRANSACTION_VERIFIED);
      }
      if(strcmp(dummy,"01") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify PASSED.","racal_VerifyPin_AMEX_Keyblock",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }
      if(strcmp(dummy,"10") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify TPK parity error.","racal_VerifyPin_AMEX_Keyblock",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"11") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify PVK parity error.","racal_VerifyPin_AMEX_Keyblock",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"12") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify No keys laoded in user storage.","racal_VerifyPin_AMEX_Keyblock",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"13") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify LMK error report to supervisor.","racal_VerifyPin_AMEX_Keyblock",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"15") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify Error in input data.","racal_VerifyPin_AMEX_Keyblock",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"20") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify PIN block does not contain valid values.","racal_VerifyPin_AMEX_Keyblock",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"21") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify Invalid user storage index.","racal_VerifyPin_AMEX_Keyblock",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"23") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify Invalid PIN block format code.","racal_VerifyPin_AMEX_Keyblock",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"24") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify PIN is less than 4 or more than 12.","racal_VerifyPin_AMEX_Keyblock",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      return(TRANSACTION_NOT_VERIFIED);
   }
   else
      return(TRANSACTION_NOT_VERIFIED);


}/*racal_VerifyPin_AMEX_Keyblock*/

/*********************************************************************/
INT racal_VerifyPin_JCB_Keyblock()
{
   INT   len = 0;
   CHAR  msgHeader[5] = {0};
   CHAR  cmdCode[3] = {0};
   CHAR  output[200] = {0};

   memset (&msgHeader,0,sizeof(msgHeader));
   memset (&cmdCode,0,sizeof(cmdCode));
   memset (&output,0,sizeof(output));
   memset (&returnString,0,ReturnString_Size);

   strcpy (msgHeader ,"2468");

   /* The command is different for local versus network origin. */
   if(strcmp(auth_tx.TLF01_details.dispensation.origin, "LO") == 0)
   {
      strcpy (cmdCode,"DC");
   }
   else if(strcmp(auth_tx.TLF01_details.dispensation.origin, "RO") == 0)
   {
      strcpy (cmdCode,"EC");
   }
   else
   {
      racal_log_message( 2, 2, "PV: origin is unkown.","racal_VerifyPin_JCB_Keyblock",1 );
      return(TRANSACTION_NOT_VERIFIED);
   }


   /* Add the command code and the message header.*/
   strcat(output,msgHeader);
   strcat(output,cmdCode);

   /* Add the tpk to the message.*/
   len = strlen( auth_tx.TLF01_details.source_key );
   if ( len > 0 )
   {
      switch( len )
      {
         case 32:
        	 strcat( output, DOUBLE_LEN_KEY );
        	 break;
         case 48:
        	 strcat( output, TRIPLE_LEN_KEY );
        	 break;
         default:
			 strcat( output, TRIPLE_LEN_KEY );
			 break;
      }
      strcat(output,auth_tx.TLF01_details.source_key);
   }
   else
   {
      racal_log_message( 2, 2, "PV: source_key is blank","racal_VerifyPin_JCB_Keyblock",1 );
      return(TRANSACTION_NOT_VERIFIED);
   }

   /* Add the pvk pairs to the message.*/
   if (strlen(auth_tx.CAF01_details.pin.Visa.key_left) != 0)
      strcat(output,auth_tx.CAF01_details.pin.Visa.key_left);
   else
   {
      racal_log_message( 2, 2, "PV: Key_left is blank","racal_VerifyPin_JCB_Keyblock",1 );
      return(TRANSACTION_NOT_VERIFIED);
   }

   /* Add the pvk pairs to the message.*/
   if (strlen(auth_tx.CAF01_details.pin.Visa.key_right) != 0)
      strcat(output,auth_tx.CAF01_details.pin.Visa.key_right);
   else
   {
      racal_log_message( 2, 2, "PV: Key_right is blank","racal_VerifyPin_JCB_Keyblock",1 );
      return(TRANSACTION_NOT_VERIFIED);
   }


   /* Add the pin block to the message.*/
   if(strlen(auth_tx.TLF01_details.pin_block) != 0)
      strcat(output,auth_tx.TLF01_details.pin_block);
   else
   {
      racal_log_message( 2, 2, "PV: pin_block is blank","racal_VerifyPin_JCB_Keyblock",1 );
      return(TRANSACTION_NOT_VERIFIED);
   }

   /*add the source pin format to the message.
   the values coming in are 0 to 6.  This field is 00, 01, 02, and so on.*/
   strcat(output,"0");
   if(strlen(auth_tx.TLF01_details.source_pin_format) != 0)
      strcat(output,auth_tx.TLF01_details.source_pin_format);
   else
   {
      racal_log_message( 2, 2, "PV: source pin format is blank","racal_VerifyPin_JCB_Keyblock",1 );
      return(TRANSACTION_NOT_VERIFIED);
   }

   /*take the 12 right most digit from the account number.*/
   if(strlen(auth_tx.TLF01_details.card_num) >= 12 )
   {
      memset(&dummy,0,Dummy_Size);
      strcpy(dummy,auth_tx.TLF01_details.card_num + (strlen(auth_tx.TLF01_details.card_num) - 13));
      dummy[12] = '\0';
      strcat(output,dummy);
   }
   else
   {
      racal_log_message( 2, 2, "PV: card_num is < 12","racal_VerifyPin_JCB_Keyblock",1 );
      return (TRANSACTION_NOT_VERIFIED);
   }

   /*look at the value of auth_tx.CAF01_details.pin_offset_location if it is "C" check the data server*/
   if(strcmp(auth_tx.CAF01_details.pin_offset_location,"C") == 0)
   {
      /*Get the PVKI and the PVV from the data server.
      add the PVKI value to the message.*/
      if(strlen(auth_tx.CCF02P_details.pvki) != 0)
         strcat(output,auth_tx.CCF02P_details.pvki);
      else
      {
         racal_log_message( 2, 2, "PV: pvki is blank","racal_VerifyPin_JCB_Keyblock",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      /*add the PVV value to the message.*/
      if(strlen(auth_tx.CCF02P_details.pin_offset) != 0)
         strcat(output,auth_tx.CCF02P_details.pin_offset);
      else
      {
         racal_log_message( 2, 2, "PV: pin_offset is blank","racal_VerifyPin_JCB_Keyblock",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }
   }
   else
      {
         /*Get the PVKI and the PVV from the TRACK 1 &or 2.*/
         if(strlen(auth_tx.TLF01_details.track1) != 0)
         {
            /*if you have made it this far, info is on track one.*/
            /*check track one.  make sure it is not empty.*/
            /*get the value of the PVKI from track one*/
            memset(&dummy,0,Dummy_Size);
            if (1 == parseTrackOne(auth_tx.TLF01_details.track1,dummy ,"^",
                     auth_tx.CAF01_details.pin.Visa.pvki_location_tr1, auth_tx.CAF01_details.pin.Visa.pvki_length_tr1))
            {
               strcat(output,dummy);
            }
            else
               return(TRANSACTION_NOT_VERIFIED);

            /*get the value of the PVV from track one.*/
            memset(&dummy,0,Dummy_Size);
            if (1 == parseTrackOne(auth_tx.TLF01_details.track1,dummy ,"^",
                     auth_tx.CAF01_details.pin.Visa.pvv_location_tr1, auth_tx.CAF01_details.pin.Visa.pvv_offset_tr1))
            {
               strcat(output,dummy);
            }
            else
               return(TRANSACTION_NOT_VERIFIED);
         }
         else
            if(strlen(auth_tx.TLF01_details.track2) != 0)
            {
               /*if you have made it this far, info is on track two.*/
               /*check track two.  make sure it is not empty.*/
               /*get the value of the PVKI from track one*/
               memset(&dummy,0,Dummy_Size);
               if (1 == parseTrackTwo(auth_tx.TLF01_details.track2,dummy ,"=",
                        auth_tx.CAF01_details.pin.Visa.pvki_location_tr2, auth_tx.CAF01_details.pin.Visa.pvki_length_tr2))
               {
                  strcat(output,dummy);
               }
               else
                  return(TRANSACTION_NOT_VERIFIED);

               /*get the value of the PVV from track one.*/
               memset(&dummy,0,Dummy_Size);
               if (1 == parseTrackTwo(auth_tx.TLF01_details.track2,dummy ,"=",
                        auth_tx.CAF01_details.pin.Visa.pvv_location_tr2, auth_tx.CAF01_details.pin.Visa.pvv_offset_tr2))
               {
                  strcat(output,dummy);
               }
               else
                  return(TRANSACTION_NOT_VERIFIED);
            }
            else
               {
                  racal_log_message( 2, 2, "PV: Track 1&2 blank","racal_VerifyPin_JCB_Keyblock",1 );
                  return(TRANSACTION_NOT_VERIFIED);
               }
      }

   /*if the message lenght is more then 150 there is some thing wrong.*/
   if (strlen(output) > 150)
   {
      racal_log_message( 2, 2, "PV: Cmd Msg to long","racal_VerifyPin_JCB_Keyblock",1 );
      return(TRANSACTION_NOT_VERIFIED);
   }

   if (racalSendToPort(output) == TRANSACTION_VERIFIED)
   {
      memset(&dummy,0,Dummy_Size);
      strncpy(dummy,returnString,4);

      /*check to see if the same header msg is back. */
      if(strcmp(msgHeader,dummy) != 0)
      {
         racal_log_message( 2, 2, "PV: Msg Header mis-match.","racal_VerifyPin_JCB_Keyblock",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      memset(&dummy,0,Dummy_Size);
      strncpy(dummy,returnString + 4,2);

      /*check to see if the "ED" or "DD" response code is back.*/
      if ((strcmp(dummy,"DD") != 0) && (strcmp(dummy,"ED") != 0) )
      {
         racal_log_message( 2, 2, "PV: Cmd Code mis-match.","racal_VerifyPin_JCB_Keyblock",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      memset(&dummy,0,Dummy_Size);
      strncpy(dummy,returnString + 6,2);

      if(strcmp(dummy,"00") == 0)
      {
#ifdef _DEBUG
         racal_log_message( 2, 1, "PIN-Verify PASSED.","racal_VerifyPin_JCB_Keyblock",1 );
#endif
         return(TRANSACTION_VERIFIED);
      }
      if(strcmp(dummy,"01") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify PASSED.","racal_VerifyPin_JCB_Keyblock",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }
      if(strcmp(dummy,"10") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify TPK parity error.","racal_VerifyPin_JCB_Keyblock",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"11") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify PVK parity error.","racal_VerifyPin_JCB_Keyblock",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"12") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify No keys laoded in user storage.","racal_VerifyPin_JCB_Keyblock",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"13") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify LMK error report to supervisor.","racal_VerifyPin_JCB_Keyblock",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"15") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify Error in input data.","racal_VerifyPin_JCB_Keyblock",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"20") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify PIN block does not contain valid values.","racal_VerifyPin_JCB_Keyblock",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"21") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify Invalid user storage index.","racal_VerifyPin_JCB_Keyblock",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"23") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify Invalid PIN block format code.","racal_VerifyPin_JCB_Keyblock",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"24") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify PIN is less than 4 or more than 12.","racal_VerifyPin_JCB_Keyblock",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      return(TRANSACTION_NOT_VERIFIED);
   }
   else
      return(TRANSACTION_NOT_VERIFIED);


}/*racal_VerifyPin_JCB_Keyblock*/

/*********************************************************************/
INT racal_VerifyPin_UPI_Keybloack()
{
   INT   len = 0;
   CHAR  msgHeader[5]={0};
   CHAR  cmdCode[3]={0};
   CHAR  output[513]={0};
   CHAR  strTemp[256]={0};
   BYTE  asciiPIN[20]={0};
   INT   blen=0, astr_len=0;
   BYTE  IBMoffset[13]={0};
   memset (&msgHeader,0,sizeof(msgHeader));
   memset (&cmdCode,0,sizeof(cmdCode));
   memset (&output,0,sizeof(output));
   memset (&returnString,0,ReturnString_Size);

   strcpy (msgHeader ,"HDRS");


   strcpy (cmdCode,"EA");
   /* Add the command code and the message header.*/
   strcat(output,msgHeader);
   strcat(output,cmdCode);

   /* Add the ZPK to the message.*/
   len = strlen( auth_tx.TLF01_details.source_key );
   if ( len > 0 )
   {
      switch( len )
      {
         case 32:
        	 strcat( output, DOUBLE_LEN_KEY );
        	 break;
         case 48:
        	 strcat( output, TRIPLE_LEN_KEY );
        	 break;
         default:
			 strcat( output, TRIPLE_LEN_KEY );
			 break;
      }
      strcat(output,auth_tx.TLF01_details.source_key);

   }
   else
   {
      racal_log_message( 1, 2, "PV: source_key(ZPK) is blank","racal_VerifyPin_UPI_Keybloack",1 );
      return(TRANSACTION_NOT_VERIFIED);
   }

   /* Add the pvk for CUP , we are storing it in AUTH_PROFILE For IBMOFFSET PIN verification page.*/
   if (8 == strlen(auth_tx.CAF01_details.pin.Visa.key_left) )
   	{
      strcat(output,auth_tx.CAF01_details.pin.Visa.key_left);
   	}
   else
   {
      racal_log_message( 1, 2, "PVK:Left is blank or More than 8 BYTE","racal_VerifyPin_UPI_Keybloack",1 );
      return(TRANSACTION_NOT_VERIFIED);
   }

   if (8 == strlen(auth_tx.CAF01_details.pin.Visa.key_right))
   {
	 	strcat(output,auth_tx.CAF01_details.pin.Visa.key_right);
   }
	else
	{
	 	racal_log_message( 1, 2, "PVK:Right is blank or More than 8 BYTE","racal_VerifyPin_UPI_Keybloack",1 );
		return(TRANSACTION_NOT_VERIFIED);
	}

	/* Max PIN length - */

	strcat(output,"12");

   /* Add the pin block to the message.*/
   if(strlen(auth_tx.TLF01_details.pin_block) != 0)
   {
	   /*we should send ASCII value.COnvert binary into ACSII */
	   if(strlen(auth_tx.TLF01_details.pin_block) == 16)
	   {
		   strcat(output,auth_tx.TLF01_details.pin_block);
	   }
	   else
	   {
		   blen = 8;
		   astr_len =16;
		   genutil_bin_to_ascii (auth_tx.TLF01_details.pin_block, blen, asciiPIN,astr_len);
		   strcat(output,asciiPIN);
		   }

   }
   else
   {
      racal_log_message( 1, 2, "PV: pin_block is blan","racal_VerifyPin_UPI_Keybloack",1 );
      return(TRANSACTION_NOT_VERIFIED);
   }


   /*add the source pin format to the message.
     This field is 00, 01, 02, and so on.*/
   strcat(output,"01");

   strcat(output,"06"); /*Min PIN len */

   /*take the 12 right most digit from the account number.*/
   if(strlen(auth_tx.TLF01_details.card_num) >= 12 )
   {
      memset(&dummy,0,Dummy_Size);
      strncpy(dummy,auth_tx.TLF01_details.card_num + (strlen(auth_tx.TLF01_details.card_num) - 13),12);
      dummy[12] = '\0';
      strcat(output,dummy);
   }
   else
   {
      racal_log_message( 1, 2, "PV: card_num is < 12","racal_VerifyPin_UPI_Keybloack",1 );
      return (TRANSACTION_NOT_VERIFIED);
   }

   /*16 byte of Decimalisation Table which will be taken from AUTH_PROFILE page */
   if(DECIMALISATION_TABLE_LENGTH==strlen(auth_tx.CAF01_details.cvk[2].cvkA))
   {
      strcat(output,auth_tx.CAF01_details.cvk[2].cvkA);
   }
   else
   {
         racal_log_message( 1, 2, "Decimalisation Table is blank","racal_VerifyPin_UPI_Keybloack",1 );
         return(TRANSACTION_NOT_VERIFIED);
   }
   /*12 byte of PIN Validation Data which will be taken from AUTH_PROFILE page */
   if(PIN_VALIDATION_DATA_LENGTH ==strlen(auth_tx.CAF01_details.cvk[2].cvkB))
   {
      strcat(output,auth_tx.CAF01_details.cvk[2].cvkB);
   }
   else
   {
         racal_log_message( 1, 2, "PIN Validation data is blank","racal_VerifyPin_UPI_Keybloack",1 );
         return(TRANSACTION_NOT_VERIFIED);
   }

    /*12 byte of IBM Offset data which will be stored in CCF02P records for each card */
   if(strlen(auth_tx.CCF02P_details.pin) > 0 )
   {
	   memcpy(IBMoffset,auth_tx.CCF02P_details.pin,12);
	   strncat(output,IBMoffset,12);
   }
   else
   {
         racal_log_message( 2, 2, "IBM Offset data is blank","racal_VerifyPin_UPI_Keybloack",1 );
         return(TRANSACTION_NOT_VERIFIED);
   }

   /*if the message lenght is more then 150 there is some thing wrong.*/
   if (strlen(output) > 150)
   {
      racal_log_message( 2, 2, "PV: Cmd Msg to long","racal_VerifyPin_UPI_Keybloack",1 );
      return(TRANSACTION_NOT_VERIFIED);
   }
   else
   {
	   sprintf(strTemp, " input of racal EA: %s", output);
	   racal_log_message( 2, 2,strTemp,"racal_VerifyPin_UPI_Keybloack",1 );
   }

   /*send a message to the Racal box.*/
   if (racalSendToPort(output) == TRANSACTION_VERIFIED)
   {
      memset(&dummy,0,Dummy_Size);
      strncpy(dummy,returnString,4);

      sprintf(strTemp, " output of racal cmnd EB: %s", returnString);
      racal_log_message( 2, 2,strTemp,"racalCUPVerifyPin",1 );

      /*check to see if the same header msg is back*/
      if(strcmp(msgHeader,dummy) != 0)
      {
         racal_log_message( 2, 2, "PV: Msg Header mis-match.","racal_VerifyPin_UPI_Keybloack",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      memset(&dummy,0,Dummy_Size);
      strncpy(dummy,returnString + 4,2);

      /*check to see if the "ED" or "DD" response code is back. */
      if ((strcmp(dummy,"EB") != 0) )
      {
         racal_log_message( 2, 2, "PV: Cmd Code mis-match.","racal_VerifyPin_UPI_Keybloack",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      memset(&dummy,0,Dummy_Size);
      strncpy(dummy,returnString + 6,2);

      if(strcmp(dummy,"00") == 0)
      {
#ifdef _DEBUG
         racal_log_message( 2, 1, "PIN-Verify PASSED.","racal_VerifyPin_UPI_Keybloack",1 );
#endif
         return(TRANSACTION_VERIFIED);
      }
      if(strcmp(dummy,"01") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify failure.","racal_VerifyPin_UPI_Keybloack",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }
      if(strcmp(dummy,"10") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify TPK parity error.","racal_VerifyPin_UPI_Keybloack",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"11") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify PVK parity error.","racal_VerifyPin_UPI_Keybloack",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"12") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify No keys laoded in user storage.","racal_VerifyPin_UPI_Keybloack",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"13") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify LMK error report to supervisor","racal_VerifyPin_UPI_Keybloack",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"15") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify Error in input data.","racal_VerifyPin_UPI_Keybloack",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"20") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify PIN block does not contain valid values.","racal_VerifyPin_UPI_Keybloack",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"21") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify Invalid user storage index","racal_VerifyPin_UPI_Keybloack",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"23") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify Invalid PIN block format code.","racal_VerifyPin_UPI_Keybloack",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      if(strcmp(dummy,"24") == 0)
      {
         racal_log_message( 2, 2, "PIN-Verify PIN is less than 4 or more than 12","racal_VerifyPin_UPI_Keybloack",1 );
         return(TRANSACTION_NOT_VERIFIED);
      }

      return(TRANSACTION_NOT_VERIFIED);
   }
   else
      return(TRANSACTION_NOT_VERIFIED);


}/*racal_VerifyPin_UPI_Keybloack*/



/*********************************************************************/
INT racal_VerifyPin_DCI_Keyblock()
{
	INT   len = 0;
	CHAR  msgHeader[5] = {0};
	CHAR  cmdCode[3] = {0};
	CHAR  output[513] = {0};

	memset (&returnString,0,ReturnString_Size);

	strcpy (msgHeader ,"2468");

	/* The command is different for local versus network origin. */
	if(strcmp(auth_tx.TLF01_details.dispensation.origin, "LO") == 0)
	{
		strcpy (cmdCode,"DC");
	}
	else if(strcmp(auth_tx.TLF01_details.dispensation.origin, "RO") == 0)
	{
		strcpy (cmdCode,"EC");
	}
	else
	{
		racal_log_message( 2, 2, "PV: origin is unkown.","racal_VerifyPin_DCI_Keyblock",1 );
		return(TRANSACTION_NOT_VERIFIED);
	}

	/* Add the command code and the message header.*/
	strcat(output,msgHeader);
	strcat(output,cmdCode);

	/* Add the tpk to the message.*/
	len = strlen(auth_tx.future_use_2 );
	if ( len > 0 )
	{
		  strncat( output, SOUURCE_TPK_KEYBLOCK,KEYBLOCK_KEY_FORMAT_LEN );
		  strcat ( output, auth_tx.future_use_2 );
		  strcat ( output, auth_tx.future_use_3 );
	}
	else
	{
		racal_log_message( 2, 2, "PV: source_key is blank","racal_VerifyPin_DCI_Keyblock",1 );
		return(TRANSACTION_NOT_VERIFIED);
	}

	/* Add the pvk pairs to the message.*/
	if (strlen(caf01_keyblock.Cvk_Pin_Verification) != 0)
	{
		strcat(output,caf01_keyblock.Cvk_Pin_Verification);
	}
	else
	{
		racal_log_message( 2, 2, "PV: Key_left is blank","racal_VerifyPin_DCI_Keyblock",1 );
		return(TRANSACTION_NOT_VERIFIED);
	}

	/* Add the pin block to the message.*/
	if(strlen(auth_tx.TLF01_details.pin_block) != 0)
	{
		strcat(output,auth_tx.TLF01_details.pin_block);
	}
	else
	{
		racal_log_message( 2, 2, "PV: pin_block is blank","racal_VerifyPin_DCI_Keyblock",1 );
		return(TRANSACTION_NOT_VERIFIED);
	}

	strcat(output,"0");
	if(strlen(auth_tx.TLF01_details.source_pin_format) != 0)
	{
		strcat(output,auth_tx.TLF01_details.source_pin_format);
	}
	else
	{
		racal_log_message( 2, 2, "PV: source pin format is blank","racal_VerifyPin_DCI_Keyblock",1 );
		return(TRANSACTION_NOT_VERIFIED);
	}

	/*take the 12 right most digit from the account number.*/
	if(strlen(auth_tx.TLF01_details.card_num) >= 12 )
	{
		memset(&dummy,0,Dummy_Size);
		strcpy(dummy,auth_tx.TLF01_details.card_num + (strlen(auth_tx.TLF01_details.card_num) - 13));
		dummy[12] = '\0';
		strcat(output,dummy);
	}
	else
	{
		racal_log_message( 2, 2, "PV: card_num is < 12","racal_VerifyPin_DCI_Keyblock",1 );
		return (TRANSACTION_NOT_VERIFIED);
	}

	/*look at the value of auth_tx.CAF01_details.pin_offset_location if it is "C" check the data server*/
	if(strcmp(auth_tx.CAF01_details.pin_offset_location,"C") == 0)
	{
		/*Get the PVKI and the PVV from the data server.
		add the PVKI value to the message.*/
		if(strlen(auth_tx.CCF02P_details.pvki) != 0)
		{
			strcat(output,auth_tx.CCF02P_details.pvki);
		}
		else
		{
			racal_log_message( 2, 2, "PV: pvki is blank","racal_VerifyPin_DCI_Keyblock",1 );
			return(TRANSACTION_NOT_VERIFIED);
		}

		/*add the PVV value to the message.*/
		if(strlen(auth_tx.CCF02P_details.pin_offset) != 0)
		{
			strcat(output,auth_tx.CCF02P_details.pin_offset);
		}
		else
		{
			racal_log_message( 2, 2, "PV: pin_offset is blank","racal_VerifyPin_DCI_Keyblock",1 );
			return(TRANSACTION_NOT_VERIFIED);
		}
	}
	else
	{
		/*Get the PVKI and the PVV from the TRACK 1 &or 2.*/
		if(strlen(auth_tx.TLF01_details.track1) != 0)
		{
			memset(&dummy,0,Dummy_Size);
			if (1 == parseTrackOne(auth_tx.TLF01_details.track1,dummy ,"^",
					auth_tx.CAF01_details.pin.Visa.pvki_location_tr1, auth_tx.CAF01_details.pin.Visa.pvki_length_tr1))
			{
				strcat(output,dummy);
			}
			else
			{
				return(TRANSACTION_NOT_VERIFIED);
			}

			/*get the value of the PVV from track one.*/
			memset(&dummy,0,Dummy_Size);
			if (1 == parseTrackOne(auth_tx.TLF01_details.track1,dummy ,"^",
					auth_tx.CAF01_details.pin.Visa.pvv_location_tr1, auth_tx.CAF01_details.pin.Visa.pvv_offset_tr1))
			{
				strcat(output,dummy);
			}
			else
			{
				return(TRANSACTION_NOT_VERIFIED);
			}
		}
		else if(strlen(auth_tx.TLF01_details.track2) != 0)
		{
			/*if you have made it this far, info is on track two.*/
			/*check track two.  make sure it is not empty.*/
			/*get the value of the PVKI from track one*/
			memset(&dummy,0,Dummy_Size);
			if (1 == parseTrackTwo(auth_tx.TLF01_details.track2,dummy ,"=",
					auth_tx.CAF01_details.pin.Visa.pvki_location_tr2, auth_tx.CAF01_details.pin.Visa.pvki_length_tr2))
			{
				strcat(output,dummy);
			}
			else
			{
				return(TRANSACTION_NOT_VERIFIED);
			}

			/*get the value of the PVV from track one.*/
			memset(&dummy,0,Dummy_Size);
			if (1 == parseTrackTwo(auth_tx.TLF01_details.track2,dummy ,"=",
					auth_tx.CAF01_details.pin.Visa.pvv_location_tr2, auth_tx.CAF01_details.pin.Visa.pvv_offset_tr2))
			{
				strcat(output,dummy);
			}
			else
			{
				return(TRANSACTION_NOT_VERIFIED);
			}
		}
		else
		{
			racal_log_message( 2, 2, "PV: Track 1&2 blank","racal_VerifyPin_DCI_Keyblock",1 );
			return(TRANSACTION_NOT_VERIFIED);
		}
	}

	/*if the message lenght is more then 150 there is some thing wrong.*/
	if (strlen(output) > MAX_KEYBLOCK_CMD_LEN)
	{
		racal_log_message( 2, 2, "PV: Cmd Msg to long","racal_VerifyPin_DCI_Keyblock",1 );
		return(TRANSACTION_NOT_VERIFIED);
	}

	if (racalSendToPort(output) == TRANSACTION_VERIFIED)
	{
		memset(&dummy,0,Dummy_Size);
		strncpy(dummy,returnString,4);

		/*check to see if the same header msg is back. */
		if(strcmp(msgHeader,dummy) != 0)
		{
			racal_log_message( 2, 2, "PV: Msg Header mis-match.","racal_VerifyPin_DCI_Keyblock",1 );
			return(TRANSACTION_NOT_VERIFIED);
		}

		memset(&dummy,0,Dummy_Size);
		strncpy(dummy,returnString + 4,2);

		/*check to see if the "ED" or "DD" response code is back.*/
		if ((strcmp(dummy,"DD") != 0) && (strcmp(dummy,"ED") != 0) )
		{
			racal_log_message( 2, 2, "PV: Cmd Code mis-match.","racal_VerifyPin_DCI_Keyblock",1 );
			return(TRANSACTION_NOT_VERIFIED);
		}

		memset(&dummy,0,Dummy_Size);
		strncpy(dummy,returnString + 6,2);

		if(strcmp(dummy,"00") == 0)
		{
			return(TRANSACTION_VERIFIED);
		}
		else if(strcmp(dummy,"01") == 0)
		{
			racal_log_message( 2, 2, "PIN-Verify PASSED.","racal_VerifyPin_DCI_Keyblock",1 );
			return(TRANSACTION_NOT_VERIFIED);
		}
		else if(strcmp(dummy,"10") == 0)
		{
			racal_log_message( 2, 2, "PIN-Verify TPK parity error.","racal_VerifyPin_DCI_Keyblock",1 );
			return(TRANSACTION_NOT_VERIFIED);
		}
		else if(strcmp(dummy,"11") == 0)
		{
			racal_log_message( 2, 2, "PIN-Verify PVK parity error.","racal_VerifyPin_DCI_Keyblock",1 );
			return(TRANSACTION_NOT_VERIFIED);
		}
		else if(strcmp(dummy,"12") == 0)
		{
			racal_log_message( 2, 2, "PIN-Verify No keys laoded in user storage.","racal_VerifyPin_DCI_Keyblock",1 );
			return(TRANSACTION_NOT_VERIFIED);
		}
		else if(strcmp(dummy,"13") == 0)
		{
			racal_log_message( 2, 2, "PIN-Verify LMK error report to supervisor.","racal_VerifyPin_DCI_Keyblock",1 );
			return(TRANSACTION_NOT_VERIFIED);
		}
		else if(strcmp(dummy,"15") == 0)
		{
			racal_log_message( 2, 2, "PIN-Verify Error in input data.","racal_VerifyPin_DCI_Keyblock",1 );
			return(TRANSACTION_NOT_VERIFIED);
		}
		else if(strcmp(dummy,"20") == 0)
		{
			racal_log_message( 2, 2, "PIN-Verify PIN block does not contain valid values.","racal_VerifyPin_DCI_Keyblock",1 );
			return(TRANSACTION_NOT_VERIFIED);
		}
		else if(strcmp(dummy,"21") == 0)
		{
			racal_log_message( 2, 2, "PIN-Verify Invalid user storage index.","racal_VerifyPin_DCI_Keyblock",1 );
			return(TRANSACTION_NOT_VERIFIED);
		}
		else if(strcmp(dummy,"23") == 0)
		{
			racal_log_message( 2, 2, "PIN-Verify Invalid PIN block format code.","racal_VerifyPin_DCI_Keyblock",1 );
			return(TRANSACTION_NOT_VERIFIED);
		}
		else if(strcmp(dummy,"24") == 0)
		{
			racal_log_message( 2, 2, "PIN-Verify PIN is less than 4 or more than 12.","racal_VerifyPin_DCI_Keyblock",1 );
			return(TRANSACTION_NOT_VERIFIED);
		}
		return(TRANSACTION_NOT_VERIFIED);
	}
	else
	{
		return(TRANSACTION_NOT_VERIFIED);
	}
}/*racal_VerifyPin_DCI_Keyblock*/

int racal_Verify_4_5CSC_AMEX_Keyblock()
{
	   char msgHeader[5]={0};
	   char cmdCode[3]={0};
	   char cvc1_serviceCode[4]={0};
	   char cvc2_serviceCode[4]={0};
	   char delimiter[2]={0};
	   char output[200]={0};
	   char cardType = 'Z';
	   char csc[6]={0};
	   char cvc1_data[4]={0};
	   char cvc2_data[4]={0};
	   int  track_present = 0;
	   int  cvc2_present = 0;
	   int  cvc2_sent = 0;
	   int  loopidx = 0;
	   int  cnt=0;
	   int  result=0;
	   char BUFFER [256]={0};
	   char strTemp[50] = {0};
	   CHAR *tempptr;
	   CHAR fieldSeperator[2]={0};
	   CHAR csc_result5[6]={0};
	   CHAR csc_result4[5]={0};
	   CHAR csc_result3[4]={0};
	   INT 	flag=0;
	   INT  AEVV_result_code;
	   CHAR AEVV_second_factor[3]={0};
	   CHAR AEVV_bin_second_factor[2];
	   CHAR AEVV_iCSC[5]={0};
	   CHAR AEVV_bin_iCSC[3];
	   CHAR AEVV_UPN[5]={0};
	   CHAR AEVV_bin_UPN[3];
	   INT  temp_out_len=0;

	   memset (&msgHeader,0,sizeof(msgHeader));
	   memset (&cmdCode,0,sizeof(cmdCode));
	   memset (&delimiter,0,sizeof(delimiter));
	   memset (&output,0,sizeof(output));
	   memset (&returnString,0,ReturnString_Size);
	   memset (&csc,0,sizeof(csc));

	   strcpy (msgHeader ,"1234");


	         /***********************Begin AMEX*********************************/
	         strcat(output,msgHeader);
	         strcat(output,"RY"); /* Changed Command code RY4- TF Phani*/
	         strcat(output,"4");

			 /* 4CSC verification */
			 	strcat(output,"0");
				flag = 0;

	            /* This is a manually entered transaction.
	             * Note that Amex can have 4-digit CSC's. Since
	             * cvc_data is not large enough for these, the
	             * field 'interest_rate' is used.
	             */

			  if ( auth_tx.TLF01_details.interest_rate[0] != 0x00 )
			   strcpy( csc, auth_tx.TLF01_details.interest_rate );

			  if(auth_tx.TLF01_details.product_codes[18].quantity[0] != 0x00)
				  strncat( csc, auth_tx.TLF01_details.product_codes[18].quantity, 2);

			 if(csc[0] == 'S') /* TF Phani First byte is "S" means manulally keyed txn */
				strcpy(csc,&csc[1]);

			/*TF Phani - CSCK value should follow by the U or T, U for Double , T for triple length
					case 32: key scheme = "U"
					case 48: key scheme = "T" */

			 /*enter the CSCK value 32H */
			 if(strlen(auth_tx.CAF01_details.csck) != 0)
			 {
				  if(auth_tx.CAF01_details.aba_nbr[0] == '1' || auth_tx.CAF01_details.aba_nbr[0] == 1)
					  strcat(output,"U");
				 strcat(output,auth_tx.CAF01_details.csck);
			 }
	         else
	         {
	            racal_log_message( 2, 2, "mag-strip caf01-csck blank.","racal_Verify_4_5CSC_AMEX_Keyblock",1 );
	            return(TRANSACTION_NOT_VERIFIED);
	         }

	         /*the account number is left-justified and zero-filled if less then 19.*/
	         if (strlen(auth_tx.TLF01_details.card_num) != 0)
	         {
	            strcat(output,auth_tx.TLF01_details.card_num);
	            if(strlen(auth_tx.TLF01_details.card_num) != 19)
	               for(cnt = 19 ; (INT)strlen(auth_tx.TLF01_details.card_num) < cnt ;cnt--)
	                  strcat(output,"0");
	         }
	         else
	         {
	            racal_log_message( 2, 2, "mag-strip card_num blank.","racal_Verify_4_5CSC_AMEX_Keyblock",1 );
	            return(TRANSACTION_NOT_VERIFIED);
	         }

	         /*do the expiration date.*/
			  if (strlen(auth_tx.TLF01_details.exp_date) != 0)
			  {
	             strcat(output, auth_tx.TLF01_details.exp_date);
			  }
	         else
	         {
	        	 racal_log_message( 2, 2, "mag-strip exp_date blank.","racal_Verify_4_5CSC_AMEX_Keyblock",1 );
	        	 return(TRANSACTION_NOT_VERIFIED);
	         }

			  /*Copy the 1 digit Rtjusfd Authn result code and 2 digit SF Authn code */
			/*strncat(output, auth_tx.TLF01_details.service_code, 3);*/

	        if(strlen(csc) != 0)
			{
	          if(strlen(csc) == 5)
			  {
	        	  strcat(output,csc);
	        	  strcat(output,"FFFFFFF");
	          }
	          else if(strlen(csc) == 4)
			  {
	        	  strcat(output,"FFFFF");
	        	  strcat(output,csc);
	        	  strcat(output,"FFF");
	          }
			}
			 else
				strcat(output,"FFFFFFFFFFFF");

			printf("CSC value : %s \n",csc);

	         /*if the output is bigger then 150 do not send it out.*/
	         if (strlen(output) > 150)
	         {
	            racal_log_message( 2, 2, "mag-strip Cmd Msg to long.","racal_Verify_4_5CSC_AMEX_Keyblock",1 );
	            return(TRANSACTION_NOT_VERIFIED);
	         }

	         sprintf(strTemp, " AMEX 4CSC to racal RY: %s", output);
			 racal_log_message( 2, 2,strTemp,"racal_Verify_4_5CSC_AMEX_Keyblock",1 );

	         /*send the message to the racal box.*/
	         if (racalSendToPort(output) == TRANSACTION_VERIFIED)
	         {
	        	 sprintf(strTemp, " output of racal cmnd RY: %s", returnString);
	        	 racal_log_message( 2, 2,strTemp,"racal_Verify_4_5CSC_AMEX_Keyblock",1 );
	            /*parse the message check for the message header.*/
	            memset(&dummy,0,Dummy_Size);
	            strncpy(dummy,returnString,4);
	            if(strcmp(dummy,msgHeader) != 0)
	            {
	               racal_log_message( 2, 2, "mag-strip Cmd header mis-match.","racal_Verify_4_5CSC_AMEX_Keyblock",1 );
	               return(TRANSACTION_NOT_VERIFIED);
	            }
	            /*parse the message check for the response code.*/
	            memset(&dummy,0,Dummy_Size);
	            strncpy(dummy,returnString + 4, 2);
	            if(strcmp(dummy,"RZ") !=0)
	            {
	               racal_log_message( 2, 2, "mag-strip Cmd Code mis-match.","racal_Verify_4_5CSC_AMEX_Keyblock",1 );
	               return(TRANSACTION_NOT_VERIFIED);
	            }

	            /*parse the message check for the error code.*/
	            memset(&dummy,0,Dummy_Size);
	            strncpy(dummy,returnString + 6, 2);

				printf("Return from Racal : %s \n",returnString);
	            /*if this value is not 00 reject the transaction.*/
	            if(strcmp(dummy,"00") != 0)
	            {
	               if(strcmp(dummy,"10") == 0)
	               {
	            	  racal_log_message( 2, 2, "mag-strip CSCK parity error.","racal_Verify_4_5CSC_AMEX_Keyblock",1 );
				   }
	               if(strcmp(dummy,"12") == 0)
	                  racal_log_message( 2, 2, "mag-strip No Keys in user storage.","racal_Verify_4_5CSC_AMEX_Keyblock",1 );
	               if(strcmp(dummy,"13") == 0)
	                  racal_log_message( 2, 2, "mag-strip LMK parity error.","racal_Verify_4_5CSC_AMEX_Keyblock",1 );
	               if(strcmp(dummy,"15") == 0)
	                  racal_log_message( 2, 2, "mag-strip Error in input data.","racal_Verify_4_5CSC_AMEX_Keyblock",1 );
				   if(strcmp(dummy,"01") == 0)
					{
					   strcpy(auth_tx.TLF01_details.response_text,"Invalid 4 CSC");
					   racal_log_message( 2, 2, "Invalid CSC/CVV/3D sec.","racal_Verify_4_5CSC_AMEX_Keyblock",1 );
					}
	/* Invalid      CSC           01
	   INvalid  27 CSCK not doubel lenght
	   Invalid  68 HSM command disabled */

	               return(TRANSACTION_NOT_VERIFIED);
	            }

	            /*parse the message mode for the error code.*/
	            memset(&dummy,0,Dummy_Size);
	            strncpy(dummy,returnString + 8, 1);
	            if(strcmp(dummy,"4") !=0)
	            {
	               racal_log_message( 2, 2, "mag-strip Msg Mode mis-match.","racal_Verify_4_5CSC_AMEX_Keyblock",1 );
	               return(TRANSACTION_NOT_VERIFIED);
	            }

	            if (strlen(csc) == 5)
	            {
	            	memset(&dummy,0,Dummy_Size);
					strncpy(dummy,returnString + 9, 1);
					if(strcmp(dummy,"1") == 0)
					{
						racal_log_message( 2, 2, "5CSC not present.","racal_Verify_4_5CSC_AMEX_Keyblock",1 );
						return(TRANSACTION_NOT_VERIFIED);
					}
					else if (strcmp(dummy,"2") == 0)
					{
						racal_log_message( 2, 2, "5CSC verification failed.","racal_Verify_4_5CSC_AMEX_Keyblock",1 );
						return(TRANSACTION_NOT_VERIFIED);
					}
					else if (strcmp(dummy,"0") == 0)
					{
						racal_log_message( 2, 2, "5CSC verification passed.","racal_Verify_4_5CSC_AMEX_Keyblock",1 );
						return(TRANSACTION_VERIFIED);
					}
	            }

	            if (strlen(csc) == 4)
	            {
	            	memset(&dummy,0,Dummy_Size);
					strncpy(dummy,returnString + 10, 1);
					if(strcmp(dummy,"1") == 0)
					{
						racal_log_message( 2, 2, "4CSC not present.","racal_Verify_4_5CSC_AMEX_Keyblock",1 );
						return(TRANSACTION_NOT_VERIFIED);
					}
					else if (strcmp(dummy,"2") == 0)
					{
						racal_log_message( 2, 2, "4CSC verification failed.","racal_Verify_4_5CSC_AMEX_Keyblock",1 );
						return(TRANSACTION_NOT_VERIFIED);
					}
					else if (strcmp(dummy,"0") == 0)
					{
						racal_log_message( 2, 2, "4CSC verification passed.","racal_Verify_4_5CSC_AMEX_Keyblock",1 );
						return(TRANSACTION_VERIFIED);
					}
	            }


	#ifdef _DEBUG
	            racal_log_message( 2, 1, "mag-strip PASSED.","racal_Verify_4_5CSC_AMEX_Keyblock",1 );
	#endif
	            return(TRANSACTION_VERIFIED);

	         }/*end of if racalSendToPort*/
	         else
	            return(TRANSACTION_NOT_VERIFIED);


	         /****************************end AMEX***************************************/
}


/*************************************************************************************/
int racal_Verify_CVV_CVC_CSC_CAV_MC_Keyblock()
{
   char msgHeader[5]={0};
   char cmdCode[3]={0};
   char cvc1_serviceCode[4]={0};
   char cvc2_serviceCode[4]={0};
   char delimiter[2]={0};
   char output[200]={0};
   char cardType = 'Z';
   char csc[6]={0};
   char cvc1_data[4]={0};
   char cvc2_data[4]={0};
   int  track_present = 0;
   int  cvc2_present = 0;
   int  cvc2_sent = 0;
   int  loopidx = 0;
   int  cnt=0;
   int  result=0;
   int  jcnt=0;
   char BUFFER [256]={0};
   char strTemp[50] = {0};
   CHAR *tempptr;
   CHAR fieldSeperator[2]={0};
   CHAR csc_result5[6]={0};
   CHAR csc_result4[5]={0};
   CHAR csc_result3[4]={0};
   INT 	flag=0;
   INT  AEVV_result_code;
   CHAR AEVV_second_factor[3]={0};
   CHAR AEVV_bin_second_factor[2]={0};
   CHAR AEVV_iCSC[5]={0};
   CHAR AEVV_bin_iCSC[3]={0};
   CHAR AEVV_UPN[5]={0};
   CHAR AEVV_bin_UPN[3]={0};
   INT  temp_out_len=0;

   memset (&msgHeader,0,sizeof(msgHeader));
   memset (&cmdCode,0,sizeof(cmdCode));
   memset (&delimiter,0,sizeof(delimiter));
   memset (&output,0,sizeof(output));
   memset (&returnString,0,ReturnString_Size);
   memset (&csc,0,sizeof(csc));

   strcpy (msgHeader ,"1234");

   if (strcmp(auth_tx.BIN01_details.card_family,CARD_VISA) == 0)
      cardType = 'V';

   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_MASTERCARD) == 0)
      cardType = 'M';

   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_AMEX) == 0)
      cardType = 'A';

   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_JCB) == 0)
      cardType = 'J';
   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_CUP) == 0)
		cardType = 'C';
   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_FLEET) == 0)
  		cardType = 'F';
   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_DINERS) == 0)
     		cardType = 'D';

   switch(cardType)
   {
      case 'A':

         /***********************Begin AMEX*********************************/
         strcat(output,msgHeader);
         strcat(output,"RY"); /* Changed Command code RY4- TF Phani*/
         strcat(output,"4");
		 if(strlen(auth_tx.int_net_facility_data) > 8)
		 {
		 /* AEVV verification value */
		 	strcat(output,"3");
			flag = 3;
			AEVV_result_code =  auth_tx.int_net_facility_data[11];
			memcpy(AEVV_bin_second_factor,&auth_tx.int_net_facility_data[12],1);
			bcd_to_asc(AEVV_bin_second_factor,1,AEVV_second_factor,2);

			memcpy(AEVV_bin_iCSC,&auth_tx.int_net_facility_data[14],2);
			bcd_to_asc(AEVV_bin_iCSC,2,AEVV_iCSC,4);
			memcpy(AEVV_bin_UPN,&auth_tx.int_net_facility_data[16],2);
			bcd_to_asc(AEVV_bin_UPN,2,AEVV_UPN,4);

		 }
		 else
         strcat(output,"0");

         /*check to see if the value of the CSC is in on the TRACK one.*/
         if(strlen(auth_tx.TLF01_details.track1) != 0)
         {
            /*if you have made it this far, info is on track one.*/
            /*check track one.  make sure it is not empty.
               farad 4/20/1999.  The value of the field seperator could changed.
                Find field separator first, then pass on to the parseTrack1 function.
            strcpy(tempptr,auth_tx.TLF01_details.track1);*/
            tempptr = auth_tx.TLF01_details.track1;
            for(jcnt=0; jcnt<(INT)strlen(auth_tx.TLF01_details.track1); jcnt++ )
            {
               if ( isalnum(*tempptr) )
                 ++tempptr;
               else
                break;
            }

            if (tempptr != NULL)
               strncpy(fieldSeperator,tempptr,1);
            else
               return(TRANSACTION_NOT_VERIFIED);

            memset(&dummy,0,Dummy_Size);
            if (1 == parseTrackOne(auth_tx.TLF01_details.track1,dummy ,fieldSeperator,auth_tx.CAF01_details.cvv_offset_tr1, auth_tx.CAF01_details.cvv_length))
            {
               strcpy(csc,dummy);
            }
            else
               return(TRANSACTION_NOT_VERIFIED);
         }
         else if(strlen(auth_tx.TLF01_details.track2) != 0)
         {
            /*if you have made it this far, info is on track two.*/
            /*check track two, make sure it is not empty.

            farad 4/19/1999.  The value of the field seperator could changed.
             Find field separator first, then pass on to the parseTrack2 function.
            strcpy(tempptr,auth_tx.TLF01_details.track2);*/
            tempptr = auth_tx.TLF01_details.track2;
            for(jcnt=0; jcnt<(INT)strlen(auth_tx.TLF01_details.track2); jcnt++ )
            {
               if ( isalnum(*tempptr) )
                 ++tempptr;
               else
                break;
            }

            if (tempptr != NULL)
               strncpy(fieldSeperator,tempptr,1);
            else
               return(TRANSACTION_NOT_VERIFIED);


            memset(&dummy,0,Dummy_Size);
            if (1 == parseTrackTwo(auth_tx.TLF01_details.track2,dummy ,fieldSeperator,auth_tx.CAF01_details.cvv_offset_tr2, auth_tx.CAF01_details.cvv_length))
            {
               strcpy(csc,dummy);
            }
            else
               return(TRANSACTION_NOT_VERIFIED);
         }
         else
         {
            /* This is a manually entered transaction.
             * Note that Amex can have 4-digit CSC's. Since
             * cvc_data is not large enough for these, the
             * field 'interest_rate' is used.
             */
            if ( auth_tx.TLF01_details.cvc_data[0] != 0x00 )
               strcpy( csc, auth_tx.TLF01_details.cvc_data );
            else if ( auth_tx.TLF01_details.interest_rate[0] != 0x00 )
               strcpy( csc, auth_tx.TLF01_details.interest_rate );

			if(csc[0] == 'S') /* TF Phani First byte is "S" means manulally keyed txn */
				strcpy(csc,&csc[1]);


         }
		/*TF Phani - CSCK value should follow by the U or T, U for Double , T for triple length
		case 32: key scheme = "U"
		case 48: key scheme = "T" */

         /*enter the CSCK value 32H */
         if(strlen(auth_tx.CAF01_details.csck) != 0)
         {	if(auth_tx.CAF01_details.aba_nbr[0] == '1' || auth_tx.CAF01_details.aba_nbr[0] == 1)
         	  strcat(output,"U");
            strcat(output,auth_tx.CAF01_details.csck);
         }
         else
         {
            racal_log_message( 2, 2, "mag-strip caf01-csck blank.","racal_Verify_CVV_CVC_CSC_CAV_MC_Keyblock",1 );
            return(TRANSACTION_NOT_VERIFIED);
         }

         /*the account number is left-justified and zero-filled if less then 19.*/
         if (strlen(auth_tx.TLF01_details.card_num) != 0)
         {
            strcat(output,auth_tx.TLF01_details.card_num);
            if(strlen(auth_tx.TLF01_details.card_num) != 19)
               for(cnt = 19 ; (INT)strlen(auth_tx.TLF01_details.card_num) < cnt ;cnt--)
                  strcat(output,"0");
         }
         else
         {
            racal_log_message( 2, 2, "mag-strip card_num blank.","racal_Verify_CVV_CVC_CSC_CAV_MC_Keyblock",1 );
            return(TRANSACTION_NOT_VERIFIED);
         }
         /*do the expiration date.*/
			if(flag == 3 && strlen(AEVV_UPN) > 0)
			{
			strncat(output,AEVV_UPN,4);
			}
		 else if(strlen(auth_tx.TLF01_details.exp_date) != 0)
            strcat(output,auth_tx.TLF01_details.exp_date);
         else
         {
        	 racal_log_message( 2, 2, "mag-strip exp_date blank.","racal_Verify_CVV_CVC_CSC_CAV_MC_Keyblock",1 );
        	 return(TRANSACTION_NOT_VERIFIED);
         }
		/* TF Phani- Add CSC value*/
		if(flag == 3)
		{
			/* AEVV value */
			temp_out_len = strlen(output);

			sprintf(&output[temp_out_len],"%1d",AEVV_result_code);
			strcat(output,AEVV_second_factor);
			strncat(output,&AEVV_iCSC[1],3);
		}
        else if(strlen(csc) != 0)
		{ if(strlen(csc) == 5)
		  {strcat(output,csc);
		   strcat(output,"FFFFFFF");
          }else if(strlen(csc) == 4)
		  {strcat(output,"FFFFF");
		  strcat(output,csc);
		  strcat(output,"FFF");
          }else if(strlen(csc) == 3)
		  {strcat(output,"FFFFF");
		   strcat(output,"FFFF");
		   strcat(output,csc);
          }

		}
		 else
			strcat(output,"FFFFFFFFFFFF");
		printf("CSC value : %s \n",csc);
		printf("Output : %s \n",output);

		sprintf(strTemp, " AMEX 3CSC to racal RY: %s", output);
		racal_log_message( 2, 2,strTemp,"racal_Verify_CVV_CVC_CSC_CAV_MC_Keyblock",1 );

         /*if the output is bigger then 150 do not send it out.*/
         if (strlen(output) > 150)
         {
            racal_log_message( 2, 2, "mag-strip Cmd Msg to long.","racal_Verify_CVV_CVC_CSC_CAV_MC_Keyblock",1 );
            return(TRANSACTION_NOT_VERIFIED);
         }

         /*send the message to the racal box.*/
         if (racalSendToPort(output) == TRANSACTION_VERIFIED)
         {
        	 sprintf(strTemp, " output of racal cmnd RY: %s", returnString);
        	 racal_log_message( 2, 2,strTemp,"racal_Verify_CVV_CVC_CSC_CAV_MC_Keyblock",1 );

            /*parse the message check for the message header.*/
            memset(&dummy,0,Dummy_Size);
            strncpy(dummy,returnString,4);
            if(strcmp(dummy,msgHeader) != 0)
            {
               racal_log_message( 2, 2, "mag-strip Cmd header mis-match.","racal_Verify_CVV_CVC_CSC_CAV_MC_Keyblock",1 );
               return(TRANSACTION_NOT_VERIFIED);
            }
            /*parse the message check for the response code.*/
            memset(&dummy,0,Dummy_Size);
            strncpy(dummy,returnString + 4, 2);
            if(strcmp(dummy,"RZ") !=0)
            {
               racal_log_message( 2, 2, "mag-strip Cmd Code mis-match.","racal_Verify_CVV_CVC_CSC_CAV_MC_Keyblock",1 );
               return(TRANSACTION_NOT_VERIFIED);
            }

            /*parse the message check for the error code.*/
            memset(&dummy,0,Dummy_Size);
            strncpy(dummy,returnString + 6, 2);

			printf("Return from Racal : %s \n",returnString);
            /*if this value is not 00 reject the transaction.*/
            if(strcmp(dummy,"00") != 0)
            {
               if(strcmp(dummy,"10") == 0)
               {
            	  racal_log_message( 2, 2, "mag-strip CSCK parity error.","racal_Verify_CVV_CVC_CSC_CAV_MC_Keyblock",1 );
			   }
               if(strcmp(dummy,"12") == 0)
                  racal_log_message( 2, 2, "mag-strip No Keys in user storage.","racal_Verify_CVV_CVC_CSC_CAV_MC_Keyblock",1 );
               if(strcmp(dummy,"13") == 0)
                  racal_log_message( 2, 2, "mag-strip LMK parity error.","racal_Verify_CVV_CVC_CSC_CAV_MC_Keyblock",1 );
               if(strcmp(dummy,"15") == 0)
                  racal_log_message( 2, 2, "mag-strip Error in input data.","racal_Verify_CVV_CVC_CSC_CAV_MC_Keyblock",1 );
			   if(strcmp(dummy,"01") == 0)
			   	{
				   strcpy(auth_tx.TLF01_details.response_text,"Invalid 4 CSC");
			   	   racal_log_message( 2, 2, "Invalid CSC/CVV/3D sec.","racal_Verify_CVV_CVC_CSC_CAV_MC_Keyblock",1 );
			   	}

			   return(TRANSACTION_NOT_VERIFIED);
            }

            /*parse the message mode for the error code.*/
            memset(&dummy,0,Dummy_Size);
            strncpy(dummy,returnString + 8, 1);
            if(strcmp(dummy,"4") !=0)
            {
               racal_log_message( 2, 2, "mag-strip Msg Mode mis-match.","racal_Verify_CVV_CVC_CSC_CAV_MC_Keyblock",1 );
               return(TRANSACTION_NOT_VERIFIED);
            }


#ifdef _DEBUG
            racal_log_message( 2, 1, "mag-strip PASSED.","racal_Verify_CVV_CVC_CSC_CAV_MC_Keyblock",1 );
#endif
            return(TRANSACTION_VERIFIED);

         }/*end of if racalSendToPort*/
         else
            return(TRANSACTION_NOT_VERIFIED);


         /****************************end AMEX***************************************/

         break;
	  case 'C':
	  case 'D':
      case 'J':
         /*drop down to VISA for this function.*/
         /*break;*/

      case 'M':

         /*drop down to VISA for this function.*/
         /*break;*/
      case 'F':
    	  /*This is Private lable transaction.*/
    	  /*drop down to VISA for this function.*/
    	  /*break;*/
      case 'V':

         /*****************************begin Visa***************************************/
         /*The VISA, MC, and the JCB are all the same.*/
         /*The rest of this code would deal with them.*/


    		  strcpy (cmdCode,"CY");

         strcpy (delimiter,";");

         /*edit CVK A */
         if(strlen(auth_tx.CAF01_details.cvk[0].cvkA) == 0)
		 {
            racal_log_message( 2, 2, "mag-strip cvkA is blank.","racal_Verify_CVV_CVC_CSC_CAV_MC_Keyblock",1 );
            return(TRANSACTION_NOT_VERIFIED);
         }
         /*edit CVK B */
         if(strlen(auth_tx.CAF01_details.cvk[0].cvkB) == 0)
         {
            racal_log_message( 2, 2, "mag-strip cvkB is blank.","racal_Verify_CVV_CVC_CSC_CAV_MC_Keyblock",1 );
            return(TRANSACTION_NOT_VERIFIED);
         }

		 /*edit card number*/
	 	 if(strlen(auth_tx.TLF01_details.card_num) == 0 )
         {
            racal_log_message( 2, 2, "mag-strip card_num is blank.","racal_Verify_CVV_CVC_CSC_CAV_MC_Keyblock",1 );
            return(TRANSACTION_NOT_VERIFIED);
         }

		 /*edit expiration date*/
         if(strlen(auth_tx.TLF01_details.exp_date) == 0)
         {
            racal_log_message( 2, 2, "mag-strip exp_date is blank.","racal_Verify_CVV_CVC_CSC_CAV_MC_Keyblock",1 );
            return(TRANSACTION_NOT_VERIFIED);
         }
		/* Fall Back CVV postion is 14*/
		 if (strcmp(auth_tx.BIN01_details.card_family,CARD_MASTERCARD) == 0 &&
		 	(strncmp(auth_tx.TLF01_details.pos_entry_mode,"080",3) == 0 || strncmp(auth_tx.TLF01_details.pos_entry_mode,"008",3) == 0)
			)
		 /*	strncmp(auth_tx.TLF01_details.pos_entry_mode,"002",3) == 0 ) */

		 	{
		 		strcpy(auth_tx.CAF01_details.cvv_offset_tr1,"14");
		 		strcpy(auth_tx.CAF01_details.cvv_offset_tr2,"14");
		 	}

         /*get TrackI CVV and service code*/
         if(strlen(auth_tx.TLF01_details.track1) != 0)	/*check track one.  make sure it is not empty.*/
		 {
            track_present = 1;
			/*get the CVV value */
            memset(&dummy,0,Dummy_Size);
            if (1 == parseTrackOne(auth_tx.TLF01_details.track1,dummy ,"^",
                   auth_tx.CAF01_details.cvv_offset_tr1, auth_tx.CAF01_details.cvv_length))
                strcpy(cvc1_data,dummy);
            else
             return(TRANSACTION_NOT_VERIFIED);

            /*get service code */
            memset(&dummy,0,Dummy_Size);
            if (1 == parseTrackOne(auth_tx.TLF01_details.track1,dummy ,"^",
						auth_tx.CAF01_details.service_code_offset_tr1, "3"))
				   strcpy(cvc1_serviceCode,dummy);
			else
               return(TRANSACTION_NOT_VERIFIED);
		 }
         else
         /*or get TrackII CVV and service code*/
         if(strlen(auth_tx.TLF01_details.track2) != 0) /*check track two. Make sure it is not empty.*/
		 {
			track_present = 1;
			/*get the CVV value */
            memset(&dummy,0,Dummy_Size);
            if (1 == parseTrackTwo(auth_tx.TLF01_details.track2,dummy ,"=",
					auth_tx.CAF01_details.cvv_offset_tr2, auth_tx.CAF01_details.cvv_length))
           	   strcpy(cvc1_data,dummy);
            else
                  return(TRANSACTION_NOT_VERIFIED);

               /*get the service code.*/
               memset(&dummy,0,Dummy_Size);
               if (1 == parseTrackTwo(auth_tx.TLF01_details.track2,dummy ,"=",
                     auth_tx.CAF01_details.service_code_offset_tr2, "3"))
                  strcpy(cvc1_serviceCode,dummy);
               else
                  return(TRANSACTION_NOT_VERIFIED);

		 }

		 /*Get cvc2 if present*/
		 if(strlen(auth_tx.TLF01_details.cvc_data) != 0)
		 {

				 strcpy(cvc2_data, auth_tx.TLF01_details.cvc_data);
				 strcpy(cvc2_serviceCode, "000");
				 cvc2_present = 1;

		 }
		 /*Get cvc2 if present, if txn is STAND IN or come with other value 29-Jul-2009
		   If first byte is space(0x20) cvv data copied in the cvc_data
		   other wise cvv data copied in interest_rate. That values ATP should send as CVV2 data*/

		 else if(strlen(auth_tx.TLF01_details.interest_rate) != 0)
		 {


				 strcpy(cvc2_data, &auth_tx.TLF01_details.interest_rate[1]);
				 strcpy(cvc2_serviceCode, "000");
				 cvc2_present = 1;

		 }

		 /*If neither cvc1 or cvc2 is present, return no value*/
		 if( !track_present && !cvc2_present)
			 return (TRANSACTION_VERIFIED);
		 /* check the CVC data and interest rate which have the cvv2 value,
		 	If it has all zeros, it is not a valid cvv2 hence make cvv2_present has 0*/
		 if(strlen(cvc1_data)!=0)
		 {

			 if(Isallzeros(&auth_tx.TLF01_details.cvc_data,strlen(auth_tx.TLF01_details.cvc_data)) == true)
			 {
			 				 cvc2_present = 0;

			 }
			 if(Isallzeros(&auth_tx.TLF01_details.interest_rate,strlen(auth_tx.TLF01_details.interest_rate)) == true)
			 {
			 				 cvc2_present = 0;

			 }
		 }
		 /*message loop to send cvc1 and/or cvc2 verification messages to Racal and receive responses*/
		 for ( loopidx = 0; loopidx < 2; loopidx++)
		 {
			 if (loopidx == 0 && track_present) /*cvc1 is present and needs to be verified*/
			 {
				 /*Build message and send to Racal.  */
				 memset (&output,0,sizeof(output));
				 strcat(output, msgHeader);
				 strcat(output, cmdCode);
				 strcat(output, auth_tx.CAF01_details.cvk[0].cvkA);
				 strcat(output, auth_tx.CAF01_details.cvk[0].cvkB);
				 strcat(output, cvc1_data);
				 strcat(output, auth_tx.TLF01_details.card_num);
				 strcat(output, delimiter);
				 strcat(output, auth_tx.TLF01_details.exp_date);
				 strcat(output, cvc1_serviceCode);
			 }
			 else /*cvc2 needs to be verifed if present*/
			 {
				 /*Build message for cvc2 verification and send to Racal*/
				 memset (&output,0,sizeof(output));
				 strcat(output, msgHeader);
				 strcat(output, cmdCode);
				 strcat(output, auth_tx.CAF01_details.cvk[0].cvkA);
				 strcat(output, auth_tx.CAF01_details.cvk[0].cvkB);
				 strcat(output, cvc2_data);
				 strcat(output, auth_tx.TLF01_details.card_num);
				 strcat(output, delimiter);
				 strcat(output, auth_tx.TLF01_details.exp_date);
				 strcat(output, cvc2_serviceCode);

				  cvc2_sent = 1;
			 }


			 /*if the output is bigger then 150 do not send it out. */
			 if (strlen(output) > 150)
			 {
				racal_log_message( 2, 2, "mag-strip Cmd Msg to long.","racal_Verify_CVV_CVC_CSC_CAV_MC_Keyblock",1 );
				if (cvc2_sent)
					return(CVC2_TRANSACTION_NOT_VERIFIED);
				else
					return(TRANSACTION_NOT_VERIFIED);
			 }

			 memset(BUFFER,0,sizeof(BUFFER));
			 sprintf(BUFFER, "BEFORE-PORT: %s", output);
			 MYPRINT(BUFFER);

			 /*The message is built.  Send it over to the Racal box.*/
			 /*Once the a response is recieved, check for the message header,*/
			 /*response code, and the error code.  If it does not exactly match,*/
			 /*send TRANSACTION_NOT_VERIFIED back.*/
			 racal_log_message( 2, 2, output,"racal_Verify_CVV_CVC_CSC_CAV_MC_Keyblock",1 );
			 if (racalSendToPort(output) == TRANSACTION_VERIFIED)
			 {
				/*parse the message check for the message header.*/
				memset(&dummy,0,Dummy_Size);
				strncpy(dummy,returnString,4);
				if(strcmp(dummy,msgHeader) != 0)
				{
				   racal_log_message( 2, 2, "mag-strip Msg Header mis-match.","racal_Verify_CVV_CVC_CSC_CAV_MC_Keyblock",1 );
				   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}

				/*parse the message check for the response code.*/
				memset(&dummy,0,Dummy_Size);
				strncpy(dummy,returnString + 4, 2);
				if(strcmp(dummy,"CZ") !=0)
				{
				   racal_log_message( 2, 2, "mag-strip Cmd Msg mis-match.","racal_Verify_CVV_CVC_CSC_CAV_MC_Keyblock",1 );
				   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}

            /* Parse the message check for the error code. */
            memset(&dummy,0,Dummy_Size);
            strncpy(dummy,returnString + 6, 2);
            if(strcmp(dummy,"00") == 0)
            {
               if (cvc2_sent)
                  return(CVC2_TRANSACTION_VERIFIED);
               else
                  if(!cvc2_present)
                     return (TRANSACTION_VERIFIED); /*cvc1 valid*/
                  else
                     continue; /*continue with loop to verify cvc2*/
            }

				if(strcmp(dummy,"01") == 0)
				{
				   racal_log_message( 2, 2, "CVV-VERIFY failure.","racal_Verify_CVV_CVC_CSC_CAV_MC_Keyblock",1 );
				   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}
				if(strcmp(dummy,"10") == 0)
            {
					racal_log_message( 2, 2, "CVV-VERIFY CVK A or B parity error.","racal_Verify_CVV_CVC_CSC_CAV_MC_Keyblock",1 );
               	   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}

				if(strcmp(dummy,"12") == 0)
				{
				   racal_log_message( 2, 2, "CVV-VERIFY No Key loaded in user storage.","racal_Verify_CVV_CVC_CSC_CAV_MC_Keyblock",1 );
				   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}

				if(strcmp(dummy,"13") == 0)
				{
				   racal_log_message( 2, 2, "CVV-VERIFY LMK error report to superviser.","racal_Verify_CVV_CVC_CSC_CAV_MC_Keyblock",1 );
				   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}

				if(strcmp(dummy,"15") == 0)
				{
				   racal_log_message( 2, 2, "CVV-VERIFY Error in input data.","racal_Verify_CVV_CVC_CSC_CAV_MC_Keyblock",1 );
				   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}

				if(strcmp(dummy,"21") == 0)
				{
				   racal_log_message( 2, 2, "CVV-VERIFY Invalid user storage index.","racal_Verify_CVV_CVC_CSC_CAV_MC_Keyblock",1 );
				   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}

			 }
			 else  /* else for: if (racalSendToPort(output) == TRANSACTION_VERIFIED)*/
			 {
				   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
			 }

		 }/*end for loop*/

		 return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
         break;
		         /********************end of Visa Case*****************************************/


   default:
      racal_log_message( 2, 2, "Card Type Undefined.","racal_Verify_CVV_CVC_CSC_CAV_MC_Keyblock",1 );
      return(TRANSACTION_NOT_VERIFIED);


   }/*end of switch statement.*/

}/*racal_Verify_CVV_CVC_CSC_CAV_MC_Keyblock*/

/*************************************************************************************/
int racal_Verify_CVV_CVC_CSC_CAV_VISA_Keyblock()
{
   char msgHeader[5]={0};
   char cmdCode[3]={0};
   char cvc1_serviceCode[4]={0};
   char cvc2_serviceCode[4]={0};
   char delimiter[2]={0};
   char output[200]={0};
   char cardType = 'Z';
   char csc[6]={0};
   char cvc1_data[4]={0};
   char cvc2_data[4]={0};
   int  track_present = 0;
   int  cvc2_present = 0;
   int  cvc2_sent = 0;
   int  loopidx = 0;
   int  cnt=0;
   int  result=0;
   int  jcnt=0;
   char BUFFER [256]={0};
   char strTemp[50] = {0};
   CHAR *tempptr;
   CHAR fieldSeperator[2]={0};
   CHAR csc_result5[6]={0};
   CHAR csc_result4[5]={0};
   CHAR csc_result3[4]={0};
   INT 	flag=0;
   INT  AEVV_result_code;
   CHAR AEVV_second_factor[3]={0};
   CHAR AEVV_bin_second_factor[2]={0};
   CHAR AEVV_iCSC[5]={0};
   CHAR AEVV_bin_iCSC[3]={0};
   CHAR AEVV_UPN[5]={0};
   CHAR AEVV_bin_UPN[3]={0};
   INT  temp_out_len=0;

   memset (&msgHeader,0,sizeof(msgHeader));
   memset (&cmdCode,0,sizeof(cmdCode));
   memset (&delimiter,0,sizeof(delimiter));
   memset (&output,0,sizeof(output));
   memset (&returnString,0,ReturnString_Size);
   memset (&csc,0,sizeof(csc));

   strcpy (msgHeader ,"1234");

   if (strcmp(auth_tx.BIN01_details.card_family,CARD_VISA) == 0)
      cardType = 'V';

   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_MASTERCARD) == 0)
      cardType = 'M';

   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_AMEX) == 0)
      cardType = 'A';

   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_JCB) == 0)
      cardType = 'J';
   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_CUP) == 0)
		cardType = 'C';
   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_FLEET) == 0)
  		cardType = 'F';
   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_DINERS) == 0)
     		cardType = 'D';

   switch(cardType)
   {
      case 'A':

         /***********************Begin AMEX*********************************/
         strcat(output,msgHeader);
         strcat(output,"RY"); /* Changed Command code RY4- TF Phani*/
         strcat(output,"4");
		 if(strlen(auth_tx.int_net_facility_data) > 8)
		 {
		 /* AEVV verification value */
		 	strcat(output,"3");
			flag = 3;
			AEVV_result_code =  auth_tx.int_net_facility_data[11];
			memcpy(AEVV_bin_second_factor,&auth_tx.int_net_facility_data[12],1);
			bcd_to_asc(AEVV_bin_second_factor,1,AEVV_second_factor,2);

			memcpy(AEVV_bin_iCSC,&auth_tx.int_net_facility_data[14],2);
			bcd_to_asc(AEVV_bin_iCSC,2,AEVV_iCSC,4);
			memcpy(AEVV_bin_UPN,&auth_tx.int_net_facility_data[16],2);
			bcd_to_asc(AEVV_bin_UPN,2,AEVV_UPN,4);

		 }
		 else
         strcat(output,"0");

         /*check to see if the value of the CSC is in on the TRACK one.*/
         if(strlen(auth_tx.TLF01_details.track1) != 0)
         {
            /*if you have made it this far, info is on track one.*/
            /*check track one.  make sure it is not empty.
               farad 4/20/1999.  The value of the field seperator could changed.
                Find field separator first, then pass on to the parseTrack1 function.
            strcpy(tempptr,auth_tx.TLF01_details.track1);*/
            tempptr = auth_tx.TLF01_details.track1;
            for(jcnt=0; jcnt<(INT)strlen(auth_tx.TLF01_details.track1); jcnt++ )
            {
               if ( isalnum(*tempptr) )
                 ++tempptr;
               else
                break;
            }

            if (tempptr != NULL)
               strncpy(fieldSeperator,tempptr,1);
            else
               return(TRANSACTION_NOT_VERIFIED);

            memset(&dummy,0,Dummy_Size);
            if (1 == parseTrackOne(auth_tx.TLF01_details.track1,dummy ,fieldSeperator,auth_tx.CAF01_details.cvv_offset_tr1, auth_tx.CAF01_details.cvv_length))
            {
               strcpy(csc,dummy);
            }
            else
               return(TRANSACTION_NOT_VERIFIED);
         }
         else if(strlen(auth_tx.TLF01_details.track2) != 0)
         {
            /*if you have made it this far, info is on track two.*/
            /*check track two, make sure it is not empty.

            farad 4/19/1999.  The value of the field seperator could changed.
             Find field separator first, then pass on to the parseTrack2 function.
            strcpy(tempptr,auth_tx.TLF01_details.track2);*/
            tempptr = auth_tx.TLF01_details.track2;
            for(jcnt=0; jcnt<(INT)strlen(auth_tx.TLF01_details.track2); jcnt++ )
            {
               if ( isalnum(*tempptr) )
                 ++tempptr;
               else
                break;
            }

            if (tempptr != NULL)
               strncpy(fieldSeperator,tempptr,1);
            else
               return(TRANSACTION_NOT_VERIFIED);


            memset(&dummy,0,Dummy_Size);
            if (1 == parseTrackTwo(auth_tx.TLF01_details.track2,dummy ,fieldSeperator,auth_tx.CAF01_details.cvv_offset_tr2, auth_tx.CAF01_details.cvv_length))
            {
               strcpy(csc,dummy);
            }
            else
               return(TRANSACTION_NOT_VERIFIED);
         }
         else
         {
            /* This is a manually entered transaction.
             * Note that Amex can have 4-digit CSC's. Since
             * cvc_data is not large enough for these, the
             * field 'interest_rate' is used.
             */
            if ( auth_tx.TLF01_details.cvc_data[0] != 0x00 )
               strcpy( csc, auth_tx.TLF01_details.cvc_data );
            else if ( auth_tx.TLF01_details.interest_rate[0] != 0x00 )
               strcpy( csc, auth_tx.TLF01_details.interest_rate );

			if(csc[0] == 'S') /* TF Phani First byte is "S" means manulally keyed txn */
				strcpy(csc,&csc[1]);


         }
		/*TF Phani - CSCK value should follow by the U or T, U for Double , T for triple length
		case 32: key scheme = "U"
		case 48: key scheme = "T" */

         /*enter the CSCK value 32H */
         if(strlen(auth_tx.CAF01_details.csck) != 0)
         {	if(auth_tx.CAF01_details.aba_nbr[0] == '1' || auth_tx.CAF01_details.aba_nbr[0] == 1)
         	  strcat(output,"U");
            strcat(output,auth_tx.CAF01_details.csck);
         }
         else
         {
            racal_log_message( 2, 2, "mag-strip caf01-csck blank.","racal_Verify_CVV_CVC_CSC_CAV_VISA_Keyblock",1 );
            return(TRANSACTION_NOT_VERIFIED);
         }

         /*the account number is left-justified and zero-filled if less then 19.*/
         if (strlen(auth_tx.TLF01_details.card_num) != 0)
         {
            strcat(output,auth_tx.TLF01_details.card_num);
            if(strlen(auth_tx.TLF01_details.card_num) != 19)
               for(cnt = 19 ; (INT)strlen(auth_tx.TLF01_details.card_num) < cnt ;cnt--)
                  strcat(output,"0");
         }
         else
         {
            racal_log_message( 2, 2, "mag-strip card_num blank.","racal_Verify_CVV_CVC_CSC_CAV_VISA_Keyblock",1 );
            return(TRANSACTION_NOT_VERIFIED);
         }
         /*do the expiration date.*/
			if(flag == 3 && strlen(AEVV_UPN) > 0)
			{
			strncat(output,AEVV_UPN,4);
			}
		 else if(strlen(auth_tx.TLF01_details.exp_date) != 0)
            strcat(output,auth_tx.TLF01_details.exp_date);
         else
         {
        	 racal_log_message( 2, 2, "mag-strip exp_date blank.","racal_Verify_CVV_CVC_CSC_CAV_VISA_Keyblock",1 );
        	 return(TRANSACTION_NOT_VERIFIED);
         }
		/* TF Phani- Add CSC value*/
		if(flag == 3)
		{
			/* AEVV value */
			temp_out_len = strlen(output);

			sprintf(&output[temp_out_len],"%1d",AEVV_result_code);
			strcat(output,AEVV_second_factor);
			strncat(output,&AEVV_iCSC[1],3);
		}
        else if(strlen(csc) != 0)
		{ if(strlen(csc) == 5)
		  {strcat(output,csc);
		   strcat(output,"FFFFFFF");
          }else if(strlen(csc) == 4)
		  {strcat(output,"FFFFF");
		  strcat(output,csc);
		  strcat(output,"FFF");
          }else if(strlen(csc) == 3)
		  {strcat(output,"FFFFF");
		   strcat(output,"FFFF");
		   strcat(output,csc);
          }

		}
		 else
			strcat(output,"FFFFFFFFFFFF");
		printf("CSC value : %s \n",csc);
		printf("Output : %s \n",output);

		sprintf(strTemp, " AMEX 3CSC to racal RY: %s", output);
		racal_log_message( 2, 2,strTemp,"racal_Verify_CVV_CVC_CSC_CAV_VISA_Keyblock",1 );

         /*if the output is bigger then 150 do not send it out.*/
         if (strlen(output) > 150)
         {
            racal_log_message( 2, 2, "mag-strip Cmd Msg to long.","racal_Verify_CVV_CVC_CSC_CAV_VISA_Keyblock",1 );
            return(TRANSACTION_NOT_VERIFIED);
         }

         /*send the message to the racal box.*/
         if (racalSendToPort(output) == TRANSACTION_VERIFIED)
         {
        	 sprintf(strTemp, " output of racal cmnd RY: %s", returnString);
        	 racal_log_message( 2, 2,strTemp,"racal_Verify_CVV_CVC_CSC_CAV_VISA_Keyblock",1 );

            /*parse the message check for the message header.*/
            memset(&dummy,0,Dummy_Size);
            strncpy(dummy,returnString,4);
            if(strcmp(dummy,msgHeader) != 0)
            {
               racal_log_message( 2, 2, "mag-strip Cmd header mis-match.","racal_Verify_CVV_CVC_CSC_CAV_VISA_Keyblock",1 );
               return(TRANSACTION_NOT_VERIFIED);
            }
            /*parse the message check for the response code.*/
            memset(&dummy,0,Dummy_Size);
            strncpy(dummy,returnString + 4, 2);
            if(strcmp(dummy,"RZ") !=0)
            {
               racal_log_message( 2, 2, "mag-strip Cmd Code mis-match.","racal_Verify_CVV_CVC_CSC_CAV_VISA_Keyblock",1 );
               return(TRANSACTION_NOT_VERIFIED);
            }

            /*parse the message check for the error code.*/
            memset(&dummy,0,Dummy_Size);
            strncpy(dummy,returnString + 6, 2);

			printf("Return from Racal : %s \n",returnString);
            /*if this value is not 00 reject the transaction.*/
            if(strcmp(dummy,"00") != 0)
            {
               if(strcmp(dummy,"10") == 0)
               {
            	  racal_log_message( 2, 2, "mag-strip CSCK parity error.","racal_Verify_CVV_CVC_CSC_CAV_VISA_Keyblock",1 );
			   }
               if(strcmp(dummy,"12") == 0)
                  racal_log_message( 2, 2, "mag-strip No Keys in user storage.","racal_Verify_CVV_CVC_CSC_CAV_VISA_Keyblock",1 );
               if(strcmp(dummy,"13") == 0)
                  racal_log_message( 2, 2, "mag-strip LMK parity error.","racal_Verify_CVV_CVC_CSC_CAV_VISA_Keyblock",1 );
               if(strcmp(dummy,"15") == 0)
                  racal_log_message( 2, 2, "mag-strip Error in input data.","racal_Verify_CVV_CVC_CSC_CAV_VISA_Keyblock",1 );
			   if(strcmp(dummy,"01") == 0)
			   	{
				   strcpy(auth_tx.TLF01_details.response_text,"Invalid 4 CSC");
			   	   racal_log_message( 2, 2, "Invalid CSC/CVV/3D sec.","racal_Verify_CVV_CVC_CSC_CAV_VISA_Keyblock",1 );
			   	}

			   return(TRANSACTION_NOT_VERIFIED);
            }

            /*parse the message mode for the error code.*/
            memset(&dummy,0,Dummy_Size);
            strncpy(dummy,returnString + 8, 1);
            if(strcmp(dummy,"4") !=0)
            {
               racal_log_message( 2, 2, "mag-strip Msg Mode mis-match.","racal_Verify_CVV_CVC_CSC_CAV_VISA_Keyblock",1 );
               return(TRANSACTION_NOT_VERIFIED);
            }


#ifdef _DEBUG
            racal_log_message( 2, 1, "mag-strip PASSED.","racal_Verify_CVV_CVC_CSC_CAV_VISA_Keyblock",1 );
#endif
            return(TRANSACTION_VERIFIED);

         }/*end of if racalSendToPort*/
         else
            return(TRANSACTION_NOT_VERIFIED);


         /****************************end AMEX***************************************/

         break;
	  case 'C':
	  case 'D':
      case 'J':
         /*drop down to VISA for this function.*/
         /*break;*/

      case 'M':

         /*drop down to VISA for this function.*/
         /*break;*/
      case 'F':
    	  /*This is Private lable transaction.*/
    	  /*drop down to VISA for this function.*/
    	  /*break;*/
      case 'V':

         /*****************************begin Visa***************************************/
         /*The VISA, MC, and the JCB are all the same.*/
         /*The rest of this code would deal with them.*/


    		  strcpy (cmdCode,"CY");

         strcpy (delimiter,";");

         /*edit CVK A */
         if(strlen(auth_tx.CAF01_details.cvk[0].cvkA) == 0)
		 {
            racal_log_message( 2, 2, "mag-strip cvkA is blank.","racal_Verify_CVV_CVC_CSC_CAV_VISA_Keyblock",1 );
            return(TRANSACTION_NOT_VERIFIED);
         }
         /*edit CVK B */
         if(strlen(auth_tx.CAF01_details.cvk[0].cvkB) == 0)
         {
            racal_log_message( 2, 2, "mag-strip cvkB is blank.","racal_Verify_CVV_CVC_CSC_CAV_VISA_Keyblock",1 );
            return(TRANSACTION_NOT_VERIFIED);
         }

		 /*edit card number*/
	 	 if(strlen(auth_tx.TLF01_details.card_num) == 0 )
         {
            racal_log_message( 2, 2, "mag-strip card_num is blank.","racal_Verify_CVV_CVC_CSC_CAV_VISA_Keyblock",1 );
            return(TRANSACTION_NOT_VERIFIED);
         }

		 /*edit expiration date*/
         if(strlen(auth_tx.TLF01_details.exp_date) == 0)
         {
            racal_log_message( 2, 2, "mag-strip exp_date is blank.","racal_Verify_CVV_CVC_CSC_CAV_VISA_Keyblock",1 );
            return(TRANSACTION_NOT_VERIFIED);
         }
		/* Fall Back CVV postion is 14*/
		 if (strcmp(auth_tx.BIN01_details.card_family,CARD_MASTERCARD) == 0 &&
		 	(strncmp(auth_tx.TLF01_details.pos_entry_mode,"080",3) == 0 || strncmp(auth_tx.TLF01_details.pos_entry_mode,"008",3) == 0)
			)
		 /*	strncmp(auth_tx.TLF01_details.pos_entry_mode,"002",3) == 0 ) */

		 	{
		 		strcpy(auth_tx.CAF01_details.cvv_offset_tr1,"14");
		 		strcpy(auth_tx.CAF01_details.cvv_offset_tr2,"14");
		 	}

         /*get TrackI CVV and service code*/
         if(strlen(auth_tx.TLF01_details.track1) != 0)	/*check track one.  make sure it is not empty.*/
		 {
            track_present = 1;
			/*get the CVV value */
            memset(&dummy,0,Dummy_Size);
            if (1 == parseTrackOne(auth_tx.TLF01_details.track1,dummy ,"^",
                   auth_tx.CAF01_details.cvv_offset_tr1, auth_tx.CAF01_details.cvv_length))
                strcpy(cvc1_data,dummy);
            else
             return(TRANSACTION_NOT_VERIFIED);

            /*get service code */
            memset(&dummy,0,Dummy_Size);
            if (1 == parseTrackOne(auth_tx.TLF01_details.track1,dummy ,"^",
						auth_tx.CAF01_details.service_code_offset_tr1, "3"))
				   strcpy(cvc1_serviceCode,dummy);
			else
               return(TRANSACTION_NOT_VERIFIED);
		 }
         else
         /*or get TrackII CVV and service code*/
         if(strlen(auth_tx.TLF01_details.track2) != 0) /*check track two. Make sure it is not empty.*/
		 {
			track_present = 1;
			/*get the CVV value */
            memset(&dummy,0,Dummy_Size);
            if (1 == parseTrackTwo(auth_tx.TLF01_details.track2,dummy ,"=",
					auth_tx.CAF01_details.cvv_offset_tr2, auth_tx.CAF01_details.cvv_length))
           	   strcpy(cvc1_data,dummy);
            else
                  return(TRANSACTION_NOT_VERIFIED);

               /*get the service code.*/
               memset(&dummy,0,Dummy_Size);
               if (1 == parseTrackTwo(auth_tx.TLF01_details.track2,dummy ,"=",
                     auth_tx.CAF01_details.service_code_offset_tr2, "3"))
                  strcpy(cvc1_serviceCode,dummy);
               else
                  return(TRANSACTION_NOT_VERIFIED);

		 }

		 /*Get cvc2 if present*/
		 if(strlen(auth_tx.TLF01_details.cvc_data) != 0)
		 {

				 strcpy(cvc2_data, auth_tx.TLF01_details.cvc_data);
				 strcpy(cvc2_serviceCode, "000");
				 cvc2_present = 1;

		 }
		 /*Get cvc2 if present, if txn is STAND IN or come with other value 29-Jul-2009
		   If first byte is space(0x20) cvv data copied in the cvc_data
		   other wise cvv data copied in interest_rate. That values ATP should send as CVV2 data*/

		 else if(strlen(auth_tx.TLF01_details.interest_rate) != 0)
		 {


				 strcpy(cvc2_data, &auth_tx.TLF01_details.interest_rate[1]);
				 strcpy(cvc2_serviceCode, "000");
				 cvc2_present = 1;

		 }

		 /*If neither cvc1 or cvc2 is present, return no value*/
		 if( !track_present && !cvc2_present)
			 return (TRANSACTION_VERIFIED);
		 /* check the CVC data and interest rate which have the cvv2 value,
		 	If it has all zeros, it is not a valid cvv2 hence make cvv2_present has 0*/
		 if(strlen(cvc1_data)!=0)
		 {

			 if(Isallzeros(&auth_tx.TLF01_details.cvc_data,strlen(auth_tx.TLF01_details.cvc_data)) == true)
			 {
			 				 cvc2_present = 0;

			 }
			 if(Isallzeros(&auth_tx.TLF01_details.interest_rate,strlen(auth_tx.TLF01_details.interest_rate)) == true)
			 {
			 				 cvc2_present = 0;

			 }
		 }
		 /*message loop to send cvc1 and/or cvc2 verification messages to Racal and receive responses*/
		 for ( loopidx = 0; loopidx < 2; loopidx++)
		 {
			 if (loopidx == 0 && track_present) /*cvc1 is present and needs to be verified*/
			 {
				 /*Build message and send to Racal.  */
				 memset (&output,0,sizeof(output));
				 strcat(output, msgHeader);
				 strcat(output, cmdCode);
				 strcat(output, auth_tx.CAF01_details.cvk[0].cvkA);
				 strcat(output, auth_tx.CAF01_details.cvk[0].cvkB);
				 strcat(output, cvc1_data);
				 strcat(output, auth_tx.TLF01_details.card_num);
				 strcat(output, delimiter);
				 strcat(output, auth_tx.TLF01_details.exp_date);
				 strcat(output, cvc1_serviceCode);
			 }
			 else /*cvc2 needs to be verifed if present*/
			 {
				 /*Build message for cvc2 verification and send to Racal*/
				 memset (&output,0,sizeof(output));
				 strcat(output, msgHeader);
				 strcat(output, cmdCode);
				 strcat(output, auth_tx.CAF01_details.cvk[0].cvkA);
				 strcat(output, auth_tx.CAF01_details.cvk[0].cvkB);
				 strcat(output, cvc2_data);
				 strcat(output, auth_tx.TLF01_details.card_num);
				 strcat(output, delimiter);
				 strcat(output, auth_tx.TLF01_details.exp_date);
				 strcat(output, cvc2_serviceCode);

				  cvc2_sent = 1;
			 }


			 /*if the output is bigger then 150 do not send it out. */
			 if (strlen(output) > 150)
			 {
				racal_log_message( 2, 2, "mag-strip Cmd Msg to long.","racal_Verify_CVV_CVC_CSC_CAV_VISA_Keyblock",1 );
				if (cvc2_sent)
					return(CVC2_TRANSACTION_NOT_VERIFIED);
				else
					return(TRANSACTION_NOT_VERIFIED);
			 }

			 memset(BUFFER,0,sizeof(BUFFER));
			 sprintf(BUFFER, "BEFORE-PORT: %s", output);
			 MYPRINT(BUFFER);

			 /*The message is built.  Send it over to the Racal box.*/
			 /*Once the a response is recieved, check for the message header,*/
			 /*response code, and the error code.  If it does not exactly match,*/
			 /*send TRANSACTION_NOT_VERIFIED back.*/
			 racal_log_message( 2, 2, output,"racal_Verify_CVV_CVC_CSC_CAV_VISA_Keyblock",1 );
			 if (racalSendToPort(output) == TRANSACTION_VERIFIED)
			 {
				/*parse the message check for the message header.*/
				memset(&dummy,0,Dummy_Size);
				strncpy(dummy,returnString,4);
				if(strcmp(dummy,msgHeader) != 0)
				{
				   racal_log_message( 2, 2, "mag-strip Msg Header mis-match.","racal_Verify_CVV_CVC_CSC_CAV_VISA_Keyblock",1 );
				   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}

				/*parse the message check for the response code.*/
				memset(&dummy,0,Dummy_Size);
				strncpy(dummy,returnString + 4, 2);
				if(strcmp(dummy,"CZ") !=0)
				{
				   racal_log_message( 2, 2, "mag-strip Cmd Msg mis-match.","racal_Verify_CVV_CVC_CSC_CAV_VISA_Keyblock",1 );
				   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}

            /* Parse the message check for the error code. */
            memset(&dummy,0,Dummy_Size);
            strncpy(dummy,returnString + 6, 2);
            if(strcmp(dummy,"00") == 0)
            {
               if (cvc2_sent)
                  return(CVC2_TRANSACTION_VERIFIED);
               else
                  if(!cvc2_present)
                     return (TRANSACTION_VERIFIED); /*cvc1 valid*/
                  else
                     continue; /*continue with loop to verify cvc2*/
            }

				if(strcmp(dummy,"01") == 0)
				{
				   racal_log_message( 2, 2, "CVV-VERIFY failure.","racal_Verify_CVV_CVC_CSC_CAV_VISA_Keyblock",1 );
				   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}
				if(strcmp(dummy,"10") == 0)
            {
					racal_log_message( 2, 2, "CVV-VERIFY CVK A or B parity error.","racal_Verify_CVV_CVC_CSC_CAV_VISA_Keyblock",1 );
               	   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}

				if(strcmp(dummy,"12") == 0)
				{
				   racal_log_message( 2, 2, "CVV-VERIFY No Key loaded in user storage.","racal_Verify_CVV_CVC_CSC_CAV_VISA_Keyblock",1 );
				   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}

				if(strcmp(dummy,"13") == 0)
				{
				   racal_log_message( 2, 2, "CVV-VERIFY LMK error report to superviser.","racal_Verify_CVV_CVC_CSC_CAV_VISA_Keyblock",1 );
				   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}

				if(strcmp(dummy,"15") == 0)
				{
				   racal_log_message( 2, 2, "CVV-VERIFY Error in input data.","racal_Verify_CVV_CVC_CSC_CAV_VISA_Keyblock",1 );
				   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}

				if(strcmp(dummy,"21") == 0)
				{
				   racal_log_message( 2, 2, "CVV-VERIFY Invalid user storage index.","racal_Verify_CVV_CVC_CSC_CAV_VISA_Keyblock",1 );
				   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}

			 }
			 else  /* else for: if (racalSendToPort(output) == TRANSACTION_VERIFIED)*/
			 {
				   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
			 }

		 }/*end for loop*/

		 return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
         break;
		         /********************end of Visa Case*****************************************/


   default:
      racal_log_message( 2, 2, "Card Type Undefined.","racal_Verify_CVV_CVC_CSC_CAV_VISA_Keyblock",1 );
      return(TRANSACTION_NOT_VERIFIED);


   }/*end of switch statement.*/

}/*racal_Verify_CVV_CVC_CSC_CAV_VISA_Keyblock*/

/*************************************************************************************/
int racal_Verify_CVV_CVC_CSC_CAV_AMEX_Keyblock()
{
   char msgHeader[5]={0};
   char cmdCode[3]={0};
   char cvc1_serviceCode[4]={0};
   char cvc2_serviceCode[4]={0};
   char delimiter[2]={0};
   char output[200]={0};
   char cardType = 'Z';
   char csc[6]={0};
   char cvc1_data[4]={0};
   char cvc2_data[4]={0};
   int  track_present = 0;
   int  cvc2_present = 0;
   int  cvc2_sent = 0;
   int  loopidx = 0;
   int  cnt=0;
   int  result=0;
   int  jcnt=0;
   char BUFFER [256]={0};
   char strTemp[50] = {0};
   CHAR *tempptr;
   CHAR fieldSeperator[2]={0};
   CHAR csc_result5[6]={0};
   CHAR csc_result4[5]={0};
   CHAR csc_result3[4]={0};
   INT 	flag=0;
   INT  AEVV_result_code;
   CHAR AEVV_second_factor[3]={0};
   CHAR AEVV_bin_second_factor[2]={0};
   CHAR AEVV_iCSC[5]={0};
   CHAR AEVV_bin_iCSC[3]={0};
   CHAR AEVV_UPN[5]={0};
   CHAR AEVV_bin_UPN[3]={0};
   INT  temp_out_len=0;

   memset (&msgHeader,0,sizeof(msgHeader));
   memset (&cmdCode,0,sizeof(cmdCode));
   memset (&delimiter,0,sizeof(delimiter));
   memset (&output,0,sizeof(output));
   memset (&returnString,0,ReturnString_Size);
   memset (&csc,0,sizeof(csc));

   strcpy (msgHeader ,"1234");

   if (strcmp(auth_tx.BIN01_details.card_family,CARD_VISA) == 0)
      cardType = 'V';

   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_MASTERCARD) == 0)
      cardType = 'M';

   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_AMEX) == 0)
      cardType = 'A';

   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_JCB) == 0)
      cardType = 'J';
   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_CUP) == 0)
		cardType = 'C';
   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_FLEET) == 0)
  		cardType = 'F';
   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_DINERS) == 0)
     		cardType = 'D';

   switch(cardType)
   {
      case 'A':

         /***********************Begin AMEX*********************************/
         strcat(output,msgHeader);
         strcat(output,"RY"); /* Changed Command code RY4- TF Phani*/
         strcat(output,"4");
		 if(strlen(auth_tx.int_net_facility_data) > 8)
		 {
		 /* AEVV verification value */
		 	strcat(output,"3");
			flag = 3;
			AEVV_result_code =  auth_tx.int_net_facility_data[11];
			memcpy(AEVV_bin_second_factor,&auth_tx.int_net_facility_data[12],1);
			bcd_to_asc(AEVV_bin_second_factor,1,AEVV_second_factor,2);

			memcpy(AEVV_bin_iCSC,&auth_tx.int_net_facility_data[14],2);
			bcd_to_asc(AEVV_bin_iCSC,2,AEVV_iCSC,4);
			memcpy(AEVV_bin_UPN,&auth_tx.int_net_facility_data[16],2);
			bcd_to_asc(AEVV_bin_UPN,2,AEVV_UPN,4);

		 }
		 else
         strcat(output,"0");

         /*check to see if the value of the CSC is in on the TRACK one.*/
         if(strlen(auth_tx.TLF01_details.track1) != 0)
         {
            /*if you have made it this far, info is on track one.*/
            /*check track one.  make sure it is not empty.
               farad 4/20/1999.  The value of the field seperator could changed.
                Find field separator first, then pass on to the parseTrack1 function.
            strcpy(tempptr,auth_tx.TLF01_details.track1);*/
            tempptr = auth_tx.TLF01_details.track1;
            for(jcnt=0; jcnt<(INT)strlen(auth_tx.TLF01_details.track1); jcnt++ )
            {
               if ( isalnum(*tempptr) )
                 ++tempptr;
               else
                break;
            }

            if (tempptr != NULL)
               strncpy(fieldSeperator,tempptr,1);
            else
               return(TRANSACTION_NOT_VERIFIED);

            memset(&dummy,0,Dummy_Size);
            if (1 == parseTrackOne(auth_tx.TLF01_details.track1,dummy ,fieldSeperator,auth_tx.CAF01_details.cvv_offset_tr1, auth_tx.CAF01_details.cvv_length))
            {
               strcpy(csc,dummy);
            }
            else
               return(TRANSACTION_NOT_VERIFIED);
         }
         else if(strlen(auth_tx.TLF01_details.track2) != 0)
         {
            /*if you have made it this far, info is on track two.*/
            /*check track two, make sure it is not empty.

            farad 4/19/1999.  The value of the field seperator could changed.
             Find field separator first, then pass on to the parseTrack2 function.
            strcpy(tempptr,auth_tx.TLF01_details.track2);*/
            tempptr = auth_tx.TLF01_details.track2;
            for(jcnt=0; jcnt<(INT)strlen(auth_tx.TLF01_details.track2); jcnt++ )
            {
               if ( isalnum(*tempptr) )
                 ++tempptr;
               else
                break;
            }

            if (tempptr != NULL)
               strncpy(fieldSeperator,tempptr,1);
            else
               return(TRANSACTION_NOT_VERIFIED);


            memset(&dummy,0,Dummy_Size);
            if (1 == parseTrackTwo(auth_tx.TLF01_details.track2,dummy ,fieldSeperator,auth_tx.CAF01_details.cvv_offset_tr2, auth_tx.CAF01_details.cvv_length))
            {
               strcpy(csc,dummy);
            }
            else
               return(TRANSACTION_NOT_VERIFIED);
         }
         else
         {
            /* This is a manually entered transaction.
             * Note that Amex can have 4-digit CSC's. Since
             * cvc_data is not large enough for these, the
             * field 'interest_rate' is used.
             */
            if ( auth_tx.TLF01_details.cvc_data[0] != 0x00 )
               strcpy( csc, auth_tx.TLF01_details.cvc_data );
            else if ( auth_tx.TLF01_details.interest_rate[0] != 0x00 )
               strcpy( csc, auth_tx.TLF01_details.interest_rate );

			if(csc[0] == 'S') /* TF Phani First byte is "S" means manulally keyed txn */
				strcpy(csc,&csc[1]);


         }
		/*TF Phani - CSCK value should follow by the U or T, U for Double , T for triple length
		case 32: key scheme = "U"
		case 48: key scheme = "T" */

         /*enter the CSCK value 32H */
         if(strlen(auth_tx.CAF01_details.csck) != 0)
         {	if(auth_tx.CAF01_details.aba_nbr[0] == '1' || auth_tx.CAF01_details.aba_nbr[0] == 1)
         	  strcat(output,"U");
            strcat(output,auth_tx.CAF01_details.csck);
         }
         else
         {
            racal_log_message( 2, 2, "mag-strip caf01-csck blank.","racal_Verify_CVV_CVC_CSC_CAV_AMEX_Keyblock",1 );
            return(TRANSACTION_NOT_VERIFIED);
         }

         /*the account number is left-justified and zero-filled if less then 19.*/
         if (strlen(auth_tx.TLF01_details.card_num) != 0)
         {
            strcat(output,auth_tx.TLF01_details.card_num);
            if(strlen(auth_tx.TLF01_details.card_num) != 19)
               for(cnt = 19 ; (INT)strlen(auth_tx.TLF01_details.card_num) < cnt ;cnt--)
                  strcat(output,"0");
         }
         else
         {
            racal_log_message( 2, 2, "mag-strip card_num blank.","racal_Verify_CVV_CVC_CSC_CAV_AMEX_Keyblock",1 );
            return(TRANSACTION_NOT_VERIFIED);
         }
         /*do the expiration date.*/
			if(flag == 3 && strlen(AEVV_UPN) > 0)
			{
			strncat(output,AEVV_UPN,4);
			}
		 else if(strlen(auth_tx.TLF01_details.exp_date) != 0)
            strcat(output,auth_tx.TLF01_details.exp_date);
         else
         {
        	 racal_log_message( 2, 2, "mag-strip exp_date blank.","racal_Verify_CVV_CVC_CSC_CAV_AMEX_Keyblock",1 );
        	 return(TRANSACTION_NOT_VERIFIED);
         }
		/* TF Phani- Add CSC value*/
		if(flag == 3)
		{
			/* AEVV value */
			temp_out_len = strlen(output);

			sprintf(&output[temp_out_len],"%1d",AEVV_result_code);
			strcat(output,AEVV_second_factor);
			strncat(output,&AEVV_iCSC[1],3);
		}
        else if(strlen(csc) != 0)
		{ if(strlen(csc) == 5)
		  {strcat(output,csc);
		   strcat(output,"FFFFFFF");
          }else if(strlen(csc) == 4)
		  {strcat(output,"FFFFF");
		  strcat(output,csc);
		  strcat(output,"FFF");
          }else if(strlen(csc) == 3)
		  {strcat(output,"FFFFF");
		   strcat(output,"FFFF");
		   strcat(output,csc);
          }

		}
		 else
			strcat(output,"FFFFFFFFFFFF");
		printf("CSC value : %s \n",csc);
		printf("Output : %s \n",output);

		sprintf(strTemp, " AMEX 3CSC to racal RY: %s", output);
		racal_log_message( 2, 2,strTemp,"racal_Verify_CVV_CVC_CSC_CAV_AMEX_Keyblock",1 );

         /*if the output is bigger then 150 do not send it out.*/
         if (strlen(output) > 150)
         {
            racal_log_message( 2, 2, "mag-strip Cmd Msg to long.","racal_Verify_CVV_CVC_CSC_CAV_AMEX_Keyblock",1 );
            return(TRANSACTION_NOT_VERIFIED);
         }

         /*send the message to the racal box.*/
         if (racalSendToPort(output) == TRANSACTION_VERIFIED)
         {
        	 sprintf(strTemp, " output of racal cmnd RY: %s", returnString);
        	 racal_log_message( 2, 2,strTemp,"racal_Verify_CVV_CVC_CSC_CAV_AMEX_Keyblock",1 );

            /*parse the message check for the message header.*/
            memset(&dummy,0,Dummy_Size);
            strncpy(dummy,returnString,4);
            if(strcmp(dummy,msgHeader) != 0)
            {
               racal_log_message( 2, 2, "mag-strip Cmd header mis-match.","racal_Verify_CVV_CVC_CSC_CAV_AMEX_Keyblock",1 );
               return(TRANSACTION_NOT_VERIFIED);
            }
            /*parse the message check for the response code.*/
            memset(&dummy,0,Dummy_Size);
            strncpy(dummy,returnString + 4, 2);
            if(strcmp(dummy,"RZ") !=0)
            {
               racal_log_message( 2, 2, "mag-strip Cmd Code mis-match.","racal_Verify_CVV_CVC_CSC_CAV_AMEX_Keyblock",1 );
               return(TRANSACTION_NOT_VERIFIED);
            }

            /*parse the message check for the error code.*/
            memset(&dummy,0,Dummy_Size);
            strncpy(dummy,returnString + 6, 2);

			printf("Return from Racal : %s \n",returnString);
            /*if this value is not 00 reject the transaction.*/
            if(strcmp(dummy,"00") != 0)
            {
               if(strcmp(dummy,"10") == 0)
               {
            	  racal_log_message( 2, 2, "mag-strip CSCK parity error.","racal_Verify_CVV_CVC_CSC_CAV_AMEX_Keyblock",1 );
			   }
               if(strcmp(dummy,"12") == 0)
                  racal_log_message( 2, 2, "mag-strip No Keys in user storage.","racal_Verify_CVV_CVC_CSC_CAV_AMEX_Keyblock",1 );
               if(strcmp(dummy,"13") == 0)
                  racal_log_message( 2, 2, "mag-strip LMK parity error.","racal_Verify_CVV_CVC_CSC_CAV_AMEX_Keyblock",1 );
               if(strcmp(dummy,"15") == 0)
                  racal_log_message( 2, 2, "mag-strip Error in input data.","racal_Verify_CVV_CVC_CSC_CAV_AMEX_Keyblock",1 );
			   if(strcmp(dummy,"01") == 0)
			   	{
				   strcpy(auth_tx.TLF01_details.response_text,"Invalid 4 CSC");
			   	   racal_log_message( 2, 2, "Invalid CSC/CVV/3D sec.","racal_Verify_CVV_CVC_CSC_CAV_AMEX_Keyblock",1 );
			   	}

			   return(TRANSACTION_NOT_VERIFIED);
            }

            /*parse the message mode for the error code.*/
            memset(&dummy,0,Dummy_Size);
            strncpy(dummy,returnString + 8, 1);
            if(strcmp(dummy,"4") !=0)
            {
               racal_log_message( 2, 2, "mag-strip Msg Mode mis-match.","racal_Verify_CVV_CVC_CSC_CAV_AMEX_Keyblock",1 );
               return(TRANSACTION_NOT_VERIFIED);
            }


#ifdef _DEBUG
            racal_log_message( 2, 1, "mag-strip PASSED.","racal_Verify_CVV_CVC_CSC_CAV_AMEX_Keyblock",1 );
#endif
            return(TRANSACTION_VERIFIED);

         }/*end of if racalSendToPort*/
         else
            return(TRANSACTION_NOT_VERIFIED);


         /****************************end AMEX***************************************/

         break;
	  case 'C':
	  case 'D':
      case 'J':
         /*drop down to VISA for this function.*/
         /*break;*/

      case 'M':

         /*drop down to VISA for this function.*/
         /*break;*/
      case 'F':
    	  /*This is Private lable transaction.*/
    	  /*drop down to VISA for this function.*/
    	  /*break;*/
      case 'V':

         /*****************************begin Visa***************************************/
         /*The VISA, MC, and the JCB are all the same.*/
         /*The rest of this code would deal with them.*/


    		  strcpy (cmdCode,"CY");

         strcpy (delimiter,";");

         /*edit CVK A */
         if(strlen(auth_tx.CAF01_details.cvk[0].cvkA) == 0)
		 {
            racal_log_message( 2, 2, "mag-strip cvkA is blank.","racal_Verify_CVV_CVC_CSC_CAV_AMEX_Keyblock",1 );
            return(TRANSACTION_NOT_VERIFIED);
         }
         /*edit CVK B */
         if(strlen(auth_tx.CAF01_details.cvk[0].cvkB) == 0)
         {
            racal_log_message( 2, 2, "mag-strip cvkB is blank.","racal_Verify_CVV_CVC_CSC_CAV_AMEX_Keyblock",1 );
            return(TRANSACTION_NOT_VERIFIED);
         }

		 /*edit card number*/
	 	 if(strlen(auth_tx.TLF01_details.card_num) == 0 )
         {
            racal_log_message( 2, 2, "mag-strip card_num is blank.","racal_Verify_CVV_CVC_CSC_CAV_AMEX_Keyblock",1 );
            return(TRANSACTION_NOT_VERIFIED);
         }

		 /*edit expiration date*/
         if(strlen(auth_tx.TLF01_details.exp_date) == 0)
         {
            racal_log_message( 2, 2, "mag-strip exp_date is blank.","racal_Verify_CVV_CVC_CSC_CAV_AMEX_Keyblock",1 );
            return(TRANSACTION_NOT_VERIFIED);
         }
		/* Fall Back CVV postion is 14*/
		 if (strcmp(auth_tx.BIN01_details.card_family,CARD_MASTERCARD) == 0 &&
		 	(strncmp(auth_tx.TLF01_details.pos_entry_mode,"080",3) == 0 || strncmp(auth_tx.TLF01_details.pos_entry_mode,"008",3) == 0)
			)
		 /*	strncmp(auth_tx.TLF01_details.pos_entry_mode,"002",3) == 0 ) */

		 	{
		 		strcpy(auth_tx.CAF01_details.cvv_offset_tr1,"14");
		 		strcpy(auth_tx.CAF01_details.cvv_offset_tr2,"14");
		 	}

         /*get TrackI CVV and service code*/
         if(strlen(auth_tx.TLF01_details.track1) != 0)	/*check track one.  make sure it is not empty.*/
		 {
            track_present = 1;
			/*get the CVV value */
            memset(&dummy,0,Dummy_Size);
            if (1 == parseTrackOne(auth_tx.TLF01_details.track1,dummy ,"^",
                   auth_tx.CAF01_details.cvv_offset_tr1, auth_tx.CAF01_details.cvv_length))
                strcpy(cvc1_data,dummy);
            else
             return(TRANSACTION_NOT_VERIFIED);

            /*get service code */
            memset(&dummy,0,Dummy_Size);
            if (1 == parseTrackOne(auth_tx.TLF01_details.track1,dummy ,"^",
						auth_tx.CAF01_details.service_code_offset_tr1, "3"))
				   strcpy(cvc1_serviceCode,dummy);
			else
               return(TRANSACTION_NOT_VERIFIED);
		 }
         else
         /*or get TrackII CVV and service code*/
         if(strlen(auth_tx.TLF01_details.track2) != 0) /*check track two. Make sure it is not empty.*/
		 {
			track_present = 1;
			/*get the CVV value */
            memset(&dummy,0,Dummy_Size);
            if (1 == parseTrackTwo(auth_tx.TLF01_details.track2,dummy ,"=",
					auth_tx.CAF01_details.cvv_offset_tr2, auth_tx.CAF01_details.cvv_length))
           	   strcpy(cvc1_data,dummy);
            else
                  return(TRANSACTION_NOT_VERIFIED);

               /*get the service code.*/
               memset(&dummy,0,Dummy_Size);
               if (1 == parseTrackTwo(auth_tx.TLF01_details.track2,dummy ,"=",
                     auth_tx.CAF01_details.service_code_offset_tr2, "3"))
                  strcpy(cvc1_serviceCode,dummy);
               else
                  return(TRANSACTION_NOT_VERIFIED);

		 }

		 /*Get cvc2 if present*/
		 if(strlen(auth_tx.TLF01_details.cvc_data) != 0)
		 {

				 strcpy(cvc2_data, auth_tx.TLF01_details.cvc_data);
				 strcpy(cvc2_serviceCode, "000");
				 cvc2_present = 1;

		 }
		 /*Get cvc2 if present, if txn is STAND IN or come with other value 29-Jul-2009
		   If first byte is space(0x20) cvv data copied in the cvc_data
		   other wise cvv data copied in interest_rate. That values ATP should send as CVV2 data*/

		 else if(strlen(auth_tx.TLF01_details.interest_rate) != 0)
		 {


				 strcpy(cvc2_data, &auth_tx.TLF01_details.interest_rate[1]);
				 strcpy(cvc2_serviceCode, "000");
				 cvc2_present = 1;

		 }

		 /*If neither cvc1 or cvc2 is present, return no value*/
		 if( !track_present && !cvc2_present)
			 return (TRANSACTION_VERIFIED);
		 /* check the CVC data and interest rate which have the cvv2 value,
		 	If it has all zeros, it is not a valid cvv2 hence make cvv2_present has 0*/
		 if(strlen(cvc1_data)!=0)
		 {

			 if(Isallzeros(&auth_tx.TLF01_details.cvc_data,strlen(auth_tx.TLF01_details.cvc_data)) == true)
			 {
			 				 cvc2_present = 0;

			 }
			 if(Isallzeros(&auth_tx.TLF01_details.interest_rate,strlen(auth_tx.TLF01_details.interest_rate)) == true)
			 {
			 				 cvc2_present = 0;

			 }
		 }
		 /*message loop to send cvc1 and/or cvc2 verification messages to Racal and receive responses*/
		 for ( loopidx = 0; loopidx < 2; loopidx++)
		 {
			 if (loopidx == 0 && track_present) /*cvc1 is present and needs to be verified*/
			 {
				 /*Build message and send to Racal.  */
				 memset (&output,0,sizeof(output));
				 strcat(output, msgHeader);
				 strcat(output, cmdCode);
				 strcat(output, auth_tx.CAF01_details.cvk[0].cvkA);
				 strcat(output, auth_tx.CAF01_details.cvk[0].cvkB);
				 strcat(output, cvc1_data);
				 strcat(output, auth_tx.TLF01_details.card_num);
				 strcat(output, delimiter);
				 strcat(output, auth_tx.TLF01_details.exp_date);
				 strcat(output, cvc1_serviceCode);
			 }
			 else /*cvc2 needs to be verifed if present*/
			 {
				 /*Build message for cvc2 verification and send to Racal*/
				 memset (&output,0,sizeof(output));
				 strcat(output, msgHeader);
				 strcat(output, cmdCode);
				 strcat(output, auth_tx.CAF01_details.cvk[0].cvkA);
				 strcat(output, auth_tx.CAF01_details.cvk[0].cvkB);
				 strcat(output, cvc2_data);
				 strcat(output, auth_tx.TLF01_details.card_num);
				 strcat(output, delimiter);
				 strcat(output, auth_tx.TLF01_details.exp_date);
				 strcat(output, cvc2_serviceCode);

				  cvc2_sent = 1;
			 }


			 /*if the output is bigger then 150 do not send it out. */
			 if (strlen(output) > 150)
			 {
				racal_log_message( 2, 2, "mag-strip Cmd Msg to long.","racal_Verify_CVV_CVC_CSC_CAV_AMEX_Keyblock",1 );
				if (cvc2_sent)
					return(CVC2_TRANSACTION_NOT_VERIFIED);
				else
					return(TRANSACTION_NOT_VERIFIED);
			 }

			 memset(BUFFER,0,sizeof(BUFFER));
			 sprintf(BUFFER, "BEFORE-PORT: %s", output);
			 MYPRINT(BUFFER);

			 /*The message is built.  Send it over to the Racal box.*/
			 /*Once the a response is recieved, check for the message header,*/
			 /*response code, and the error code.  If it does not exactly match,*/
			 /*send TRANSACTION_NOT_VERIFIED back.*/
			 racal_log_message( 2, 2, output,"racal_Verify_CVV_CVC_CSC_CAV_AMEX_Keyblock",1 );
			 if (racalSendToPort(output) == TRANSACTION_VERIFIED)
			 {
				/*parse the message check for the message header.*/
				memset(&dummy,0,Dummy_Size);
				strncpy(dummy,returnString,4);
				if(strcmp(dummy,msgHeader) != 0)
				{
				   racal_log_message( 2, 2, "mag-strip Msg Header mis-match.","racal_Verify_CVV_CVC_CSC_CAV_AMEX_Keyblock",1 );
				   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}

				/*parse the message check for the response code.*/
				memset(&dummy,0,Dummy_Size);
				strncpy(dummy,returnString + 4, 2);
				if(strcmp(dummy,"CZ") !=0)
				{
				   racal_log_message( 2, 2, "mag-strip Cmd Msg mis-match.","racal_Verify_CVV_CVC_CSC_CAV_AMEX_Keyblock",1 );
				   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}

            /* Parse the message check for the error code. */
            memset(&dummy,0,Dummy_Size);
            strncpy(dummy,returnString + 6, 2);
            if(strcmp(dummy,"00") == 0)
            {
               if (cvc2_sent)
                  return(CVC2_TRANSACTION_VERIFIED);
               else
                  if(!cvc2_present)
                     return (TRANSACTION_VERIFIED); /*cvc1 valid*/
                  else
                     continue; /*continue with loop to verify cvc2*/
            }

				if(strcmp(dummy,"01") == 0)
				{
				   racal_log_message( 2, 2, "CVV-VERIFY failure.","racal_Verify_CVV_CVC_CSC_CAV_AMEX_Keyblock",1 );
				   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}
				if(strcmp(dummy,"10") == 0)
            {
					racal_log_message( 2, 2, "CVV-VERIFY CVK A or B parity error.","racal_Verify_CVV_CVC_CSC_CAV_AMEX_Keyblock",1 );
               	   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}

				if(strcmp(dummy,"12") == 0)
				{
				   racal_log_message( 2, 2, "CVV-VERIFY No Key loaded in user storage.","racal_Verify_CVV_CVC_CSC_CAV_AMEX_Keyblock",1 );
				   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}

				if(strcmp(dummy,"13") == 0)
				{
				   racal_log_message( 2, 2, "CVV-VERIFY LMK error report to superviser.","racal_Verify_CVV_CVC_CSC_CAV_AMEX_Keyblock",1 );
				   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}

				if(strcmp(dummy,"15") == 0)
				{
				   racal_log_message( 2, 2, "CVV-VERIFY Error in input data.","racal_Verify_CVV_CVC_CSC_CAV_AMEX_Keyblock",1 );
				   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}

				if(strcmp(dummy,"21") == 0)
				{
				   racal_log_message( 2, 2, "CVV-VERIFY Invalid user storage index.","racal_Verify_CVV_CVC_CSC_CAV_AMEX_Keyblock",1 );
				   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}

			 }
			 else  /* else for: if (racalSendToPort(output) == TRANSACTION_VERIFIED)*/
			 {
				   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
			 }

		 }/*end for loop*/

		 return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
         break;
		         /********************end of Visa Case*****************************************/


   default:
      racal_log_message( 2, 2, "Card Type Undefined.","racal_Verify_CVV_CVC_CSC_CAV_AMEX_Keyblock",1 );
      return(TRANSACTION_NOT_VERIFIED);


   }/*end of switch statement.*/

}/*racal_Verify_CVV_CVC_CSC_CAV_AMEX_Keyblock*/

/*************************************************************************************/
int racal_Verify_CVV_CVC_CSC_CAV_JCB_Keyblock()
{
   char msgHeader[5]={0};
   char cmdCode[3]={0};
   char cvc1_serviceCode[4]={0};
   char cvc2_serviceCode[4]={0};
   char delimiter[2]={0};
   char output[200]={0};
   char cardType = 'Z';
   char csc[6]={0};
   char cvc1_data[4]={0};
   char cvc2_data[4]={0};
   int  track_present = 0;
   int  cvc2_present = 0;
   int  cvc2_sent = 0;
   int  loopidx = 0;
   int  cnt=0;
   int  result=0;
   int  jcnt=0;
   char BUFFER [256]={0};
   char strTemp[50] = {0};
   CHAR *tempptr;
   CHAR fieldSeperator[2]={0};
   CHAR csc_result5[6]={0};
   CHAR csc_result4[5]={0};
   CHAR csc_result3[4]={0};
   INT 	flag=0;
   INT  AEVV_result_code;
   CHAR AEVV_second_factor[3]={0};
   CHAR AEVV_bin_second_factor[2]={0};
   CHAR AEVV_iCSC[5]={0};
   CHAR AEVV_bin_iCSC[3]={0};
   CHAR AEVV_UPN[5]={0};
   CHAR AEVV_bin_UPN[3]={0};
   INT  temp_out_len=0;

   memset (&msgHeader,0,sizeof(msgHeader));
   memset (&cmdCode,0,sizeof(cmdCode));
   memset (&delimiter,0,sizeof(delimiter));
   memset (&output,0,sizeof(output));
   memset (&returnString,0,ReturnString_Size);
   memset (&csc,0,sizeof(csc));

   strcpy (msgHeader ,"1234");

   if (strcmp(auth_tx.BIN01_details.card_family,CARD_VISA) == 0)
      cardType = 'V';

   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_MASTERCARD) == 0)
      cardType = 'M';

   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_AMEX) == 0)
      cardType = 'A';

   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_JCB) == 0)
      cardType = 'J';
   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_CUP) == 0)
		cardType = 'C';
   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_FLEET) == 0)
  		cardType = 'F';
   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_DINERS) == 0)
     		cardType = 'D';

   switch(cardType)
   {
      case 'A':

         /***********************Begin AMEX*********************************/
         strcat(output,msgHeader);
         strcat(output,"RY"); /* Changed Command code RY4- TF Phani*/
         strcat(output,"4");
		 if(strlen(auth_tx.int_net_facility_data) > 8)
		 {
		 /* AEVV verification value */
		 	strcat(output,"3");
			flag = 3;
			AEVV_result_code =  auth_tx.int_net_facility_data[11];
			memcpy(AEVV_bin_second_factor,&auth_tx.int_net_facility_data[12],1);
			bcd_to_asc(AEVV_bin_second_factor,1,AEVV_second_factor,2);

			memcpy(AEVV_bin_iCSC,&auth_tx.int_net_facility_data[14],2);
			bcd_to_asc(AEVV_bin_iCSC,2,AEVV_iCSC,4);
			memcpy(AEVV_bin_UPN,&auth_tx.int_net_facility_data[16],2);
			bcd_to_asc(AEVV_bin_UPN,2,AEVV_UPN,4);

		 }
		 else
         strcat(output,"0");

         /*check to see if the value of the CSC is in on the TRACK one.*/
         if(strlen(auth_tx.TLF01_details.track1) != 0)
         {
            /*if you have made it this far, info is on track one.*/
            /*check track one.  make sure it is not empty.
               farad 4/20/1999.  The value of the field seperator could changed.
                Find field separator first, then pass on to the parseTrack1 function.
            strcpy(tempptr,auth_tx.TLF01_details.track1);*/
            tempptr = auth_tx.TLF01_details.track1;
            for(jcnt=0; jcnt<(INT)strlen(auth_tx.TLF01_details.track1); jcnt++ )
            {
               if ( isalnum(*tempptr) )
                 ++tempptr;
               else
                break;
            }

            if (tempptr != NULL)
               strncpy(fieldSeperator,tempptr,1);
            else
               return(TRANSACTION_NOT_VERIFIED);

            memset(&dummy,0,Dummy_Size);
            if (1 == parseTrackOne(auth_tx.TLF01_details.track1,dummy ,fieldSeperator,auth_tx.CAF01_details.cvv_offset_tr1, auth_tx.CAF01_details.cvv_length))
            {
               strcpy(csc,dummy);
            }
            else
               return(TRANSACTION_NOT_VERIFIED);
         }
         else if(strlen(auth_tx.TLF01_details.track2) != 0)
         {
            /*if you have made it this far, info is on track two.*/
            /*check track two, make sure it is not empty.

            farad 4/19/1999.  The value of the field seperator could changed.
             Find field separator first, then pass on to the parseTrack2 function.
            strcpy(tempptr,auth_tx.TLF01_details.track2);*/
            tempptr = auth_tx.TLF01_details.track2;
            for(jcnt=0; jcnt<(INT)strlen(auth_tx.TLF01_details.track2); jcnt++ )
            {
               if ( isalnum(*tempptr) )
                 ++tempptr;
               else
                break;
            }

            if (tempptr != NULL)
               strncpy(fieldSeperator,tempptr,1);
            else
               return(TRANSACTION_NOT_VERIFIED);


            memset(&dummy,0,Dummy_Size);
            if (1 == parseTrackTwo(auth_tx.TLF01_details.track2,dummy ,fieldSeperator,auth_tx.CAF01_details.cvv_offset_tr2, auth_tx.CAF01_details.cvv_length))
            {
               strcpy(csc,dummy);
            }
            else
               return(TRANSACTION_NOT_VERIFIED);
         }
         else
         {
            /* This is a manually entered transaction.
             * Note that Amex can have 4-digit CSC's. Since
             * cvc_data is not large enough for these, the
             * field 'interest_rate' is used.
             */
            if ( auth_tx.TLF01_details.cvc_data[0] != 0x00 )
               strcpy( csc, auth_tx.TLF01_details.cvc_data );
            else if ( auth_tx.TLF01_details.interest_rate[0] != 0x00 )
               strcpy( csc, auth_tx.TLF01_details.interest_rate );

			if(csc[0] == 'S') /* TF Phani First byte is "S" means manulally keyed txn */
				strcpy(csc,&csc[1]);


         }
		/*TF Phani - CSCK value should follow by the U or T, U for Double , T for triple length
		case 32: key scheme = "U"
		case 48: key scheme = "T" */

         /*enter the CSCK value 32H */
         if(strlen(auth_tx.CAF01_details.csck) != 0)
         {	if(auth_tx.CAF01_details.aba_nbr[0] == '1' || auth_tx.CAF01_details.aba_nbr[0] == 1)
         	  strcat(output,"U");
            strcat(output,auth_tx.CAF01_details.csck);
         }
         else
         {
            racal_log_message( 2, 2, "mag-strip caf01-csck blank.","racal_Verify_CVV_CVC_CSC_CAV_JCB_Keyblock",1 );
            return(TRANSACTION_NOT_VERIFIED);
         }

         /*the account number is left-justified and zero-filled if less then 19.*/
         if (strlen(auth_tx.TLF01_details.card_num) != 0)
         {
            strcat(output,auth_tx.TLF01_details.card_num);
            if(strlen(auth_tx.TLF01_details.card_num) != 19)
               for(cnt = 19 ; (INT)strlen(auth_tx.TLF01_details.card_num) < cnt ;cnt--)
                  strcat(output,"0");
         }
         else
         {
            racal_log_message( 2, 2, "mag-strip card_num blank.","racal_Verify_CVV_CVC_CSC_CAV_JCB_Keyblock",1 );
            return(TRANSACTION_NOT_VERIFIED);
         }
         /*do the expiration date.*/
			if(flag == 3 && strlen(AEVV_UPN) > 0)
			{
			strncat(output,AEVV_UPN,4);
			}
		 else if(strlen(auth_tx.TLF01_details.exp_date) != 0)
            strcat(output,auth_tx.TLF01_details.exp_date);
         else
         {
        	 racal_log_message( 2, 2, "mag-strip exp_date blank.","racal_Verify_CVV_CVC_CSC_CAV_JCB_Keyblock",1 );
        	 return(TRANSACTION_NOT_VERIFIED);
         }
		/* TF Phani- Add CSC value*/
		if(flag == 3)
		{
			/* AEVV value */
			temp_out_len = strlen(output);

			sprintf(&output[temp_out_len],"%1d",AEVV_result_code);
			strcat(output,AEVV_second_factor);
			strncat(output,&AEVV_iCSC[1],3);
		}
        else if(strlen(csc) != 0)
		{ if(strlen(csc) == 5)
		  {strcat(output,csc);
		   strcat(output,"FFFFFFF");
          }else if(strlen(csc) == 4)
		  {strcat(output,"FFFFF");
		  strcat(output,csc);
		  strcat(output,"FFF");
          }else if(strlen(csc) == 3)
		  {strcat(output,"FFFFF");
		   strcat(output,"FFFF");
		   strcat(output,csc);
          }

		}
		 else
			strcat(output,"FFFFFFFFFFFF");
		printf("CSC value : %s \n",csc);
		printf("Output : %s \n",output);

		sprintf(strTemp, " AMEX 3CSC to racal RY: %s", output);
		racal_log_message( 2, 2,strTemp,"racal_Verify_CVV_CVC_CSC_CAV_JCB_Keyblock",1 );

         /*if the output is bigger then 150 do not send it out.*/
         if (strlen(output) > 150)
         {
            racal_log_message( 2, 2, "mag-strip Cmd Msg to long.","racal_Verify_CVV_CVC_CSC_CAV_JCB_Keyblock",1 );
            return(TRANSACTION_NOT_VERIFIED);
         }

         /*send the message to the racal box.*/
         if (racalSendToPort(output) == TRANSACTION_VERIFIED)
         {
        	 sprintf(strTemp, " output of racal cmnd RY: %s", returnString);
        	 racal_log_message( 2, 2,strTemp,"racal_Verify_CVV_CVC_CSC_CAV_JCB_Keyblock",1 );

            /*parse the message check for the message header.*/
            memset(&dummy,0,Dummy_Size);
            strncpy(dummy,returnString,4);
            if(strcmp(dummy,msgHeader) != 0)
            {
               racal_log_message( 2, 2, "mag-strip Cmd header mis-match.","racal_Verify_CVV_CVC_CSC_CAV_JCB_Keyblock",1 );
               return(TRANSACTION_NOT_VERIFIED);
            }
            /*parse the message check for the response code.*/
            memset(&dummy,0,Dummy_Size);
            strncpy(dummy,returnString + 4, 2);
            if(strcmp(dummy,"RZ") !=0)
            {
               racal_log_message( 2, 2, "mag-strip Cmd Code mis-match.","racal_Verify_CVV_CVC_CSC_CAV_JCB_Keyblock",1 );
               return(TRANSACTION_NOT_VERIFIED);
            }

            /*parse the message check for the error code.*/
            memset(&dummy,0,Dummy_Size);
            strncpy(dummy,returnString + 6, 2);

			printf("Return from Racal : %s \n",returnString);
            /*if this value is not 00 reject the transaction.*/
            if(strcmp(dummy,"00") != 0)
            {
               if(strcmp(dummy,"10") == 0)
               {
            	  racal_log_message( 2, 2, "mag-strip CSCK parity error.","racal_Verify_CVV_CVC_CSC_CAV_JCB_Keyblock",1 );
			   }
               if(strcmp(dummy,"12") == 0)
                  racal_log_message( 2, 2, "mag-strip No Keys in user storage.","racal_Verify_CVV_CVC_CSC_CAV_JCB_Keyblock",1 );
               if(strcmp(dummy,"13") == 0)
                  racal_log_message( 2, 2, "mag-strip LMK parity error.","racal_Verify_CVV_CVC_CSC_CAV_JCB_Keyblock",1 );
               if(strcmp(dummy,"15") == 0)
                  racal_log_message( 2, 2, "mag-strip Error in input data.","racal_Verify_CVV_CVC_CSC_CAV_JCB_Keyblock",1 );
			   if(strcmp(dummy,"01") == 0)
			   	{
				   strcpy(auth_tx.TLF01_details.response_text,"Invalid 4 CSC");
			   	   racal_log_message( 2, 2, "Invalid CSC/CVV/3D sec.","racal_Verify_CVV_CVC_CSC_CAV_JCB_Keyblock",1 );
			   	}

			   return(TRANSACTION_NOT_VERIFIED);
            }

            /*parse the message mode for the error code.*/
            memset(&dummy,0,Dummy_Size);
            strncpy(dummy,returnString + 8, 1);
            if(strcmp(dummy,"4") !=0)
            {
               racal_log_message( 2, 2, "mag-strip Msg Mode mis-match.","racal_Verify_CVV_CVC_CSC_CAV_JCB_Keyblock",1 );
               return(TRANSACTION_NOT_VERIFIED);
            }


#ifdef _DEBUG
            racal_log_message( 2, 1, "mag-strip PASSED.","racal_Verify_CVV_CVC_CSC_CAV_JCB_Keyblock",1 );
#endif
            return(TRANSACTION_VERIFIED);

         }/*end of if racalSendToPort*/
         else
            return(TRANSACTION_NOT_VERIFIED);


         /****************************end AMEX***************************************/

         break;
	  case 'C':
	  case 'D':
      case 'J':
         /*drop down to VISA for this function.*/
         /*break;*/

      case 'M':

         /*drop down to VISA for this function.*/
         /*break;*/
      case 'F':
    	  /*This is Private lable transaction.*/
    	  /*drop down to VISA for this function.*/
    	  /*break;*/
      case 'V':

         /*****************************begin Visa***************************************/
         /*The VISA, MC, and the JCB are all the same.*/
         /*The rest of this code would deal with them.*/


    		  strcpy (cmdCode,"CY");

         strcpy (delimiter,";");

         /*edit CVK A */
         if(strlen(auth_tx.CAF01_details.cvk[0].cvkA) == 0)
		 {
            racal_log_message( 2, 2, "mag-strip cvkA is blank.","racal_Verify_CVV_CVC_CSC_CAV_JCB_Keyblock",1 );
            return(TRANSACTION_NOT_VERIFIED);
         }
         /*edit CVK B */
         if(strlen(auth_tx.CAF01_details.cvk[0].cvkB) == 0)
         {
            racal_log_message( 2, 2, "mag-strip cvkB is blank.","racal_Verify_CVV_CVC_CSC_CAV_JCB_Keyblock",1 );
            return(TRANSACTION_NOT_VERIFIED);
         }

		 /*edit card number*/
	 	 if(strlen(auth_tx.TLF01_details.card_num) == 0 )
         {
            racal_log_message( 2, 2, "mag-strip card_num is blank.","racal_Verify_CVV_CVC_CSC_CAV_JCB_Keyblock",1 );
            return(TRANSACTION_NOT_VERIFIED);
         }

		 /*edit expiration date*/
         if(strlen(auth_tx.TLF01_details.exp_date) == 0)
         {
            racal_log_message( 2, 2, "mag-strip exp_date is blank.","racal_Verify_CVV_CVC_CSC_CAV_JCB_Keyblock",1 );
            return(TRANSACTION_NOT_VERIFIED);
         }
		/* Fall Back CVV postion is 14*/
		 if (strcmp(auth_tx.BIN01_details.card_family,CARD_MASTERCARD) == 0 &&
		 	(strncmp(auth_tx.TLF01_details.pos_entry_mode,"080",3) == 0 || strncmp(auth_tx.TLF01_details.pos_entry_mode,"008",3) == 0)
			)
		 /*	strncmp(auth_tx.TLF01_details.pos_entry_mode,"002",3) == 0 ) */

		 	{
		 		strcpy(auth_tx.CAF01_details.cvv_offset_tr1,"14");
		 		strcpy(auth_tx.CAF01_details.cvv_offset_tr2,"14");
		 	}

         /*get TrackI CVV and service code*/
         if(strlen(auth_tx.TLF01_details.track1) != 0)	/*check track one.  make sure it is not empty.*/
		 {
            track_present = 1;
			/*get the CVV value */
            memset(&dummy,0,Dummy_Size);
            if (1 == parseTrackOne(auth_tx.TLF01_details.track1,dummy ,"^",
                   auth_tx.CAF01_details.cvv_offset_tr1, auth_tx.CAF01_details.cvv_length))
                strcpy(cvc1_data,dummy);
            else
             return(TRANSACTION_NOT_VERIFIED);

            /*get service code */
            memset(&dummy,0,Dummy_Size);
            if (1 == parseTrackOne(auth_tx.TLF01_details.track1,dummy ,"^",
						auth_tx.CAF01_details.service_code_offset_tr1, "3"))
				   strcpy(cvc1_serviceCode,dummy);
			else
               return(TRANSACTION_NOT_VERIFIED);
		 }
         else
         /*or get TrackII CVV and service code*/
         if(strlen(auth_tx.TLF01_details.track2) != 0) /*check track two. Make sure it is not empty.*/
		 {
			track_present = 1;
			/*get the CVV value */
            memset(&dummy,0,Dummy_Size);
            if (1 == parseTrackTwo(auth_tx.TLF01_details.track2,dummy ,"=",
					auth_tx.CAF01_details.cvv_offset_tr2, auth_tx.CAF01_details.cvv_length))
           	   strcpy(cvc1_data,dummy);
            else
                  return(TRANSACTION_NOT_VERIFIED);

               /*get the service code.*/
               memset(&dummy,0,Dummy_Size);
               if (1 == parseTrackTwo(auth_tx.TLF01_details.track2,dummy ,"=",
                     auth_tx.CAF01_details.service_code_offset_tr2, "3"))
                  strcpy(cvc1_serviceCode,dummy);
               else
                  return(TRANSACTION_NOT_VERIFIED);

		 }

		 /*Get cvc2 if present*/
		 if(strlen(auth_tx.TLF01_details.cvc_data) != 0)
		 {

				 strcpy(cvc2_data, auth_tx.TLF01_details.cvc_data);
				 strcpy(cvc2_serviceCode, "000");
				 cvc2_present = 1;

		 }
		 /*Get cvc2 if present, if txn is STAND IN or come with other value 29-Jul-2009
		   If first byte is space(0x20) cvv data copied in the cvc_data
		   other wise cvv data copied in interest_rate. That values ATP should send as CVV2 data*/

		 else if(strlen(auth_tx.TLF01_details.interest_rate) != 0)
		 {


				 strcpy(cvc2_data, &auth_tx.TLF01_details.interest_rate[1]);
				 strcpy(cvc2_serviceCode, "000");
				 cvc2_present = 1;

		 }

		 /*If neither cvc1 or cvc2 is present, return no value*/
		 if( !track_present && !cvc2_present)
			 return (TRANSACTION_VERIFIED);
		 /* check the CVC data and interest rate which have the cvv2 value,
		 	If it has all zeros, it is not a valid cvv2 hence make cvv2_present has 0*/
		 if(strlen(cvc1_data)!=0)
		 {

			 if(Isallzeros(&auth_tx.TLF01_details.cvc_data,strlen(auth_tx.TLF01_details.cvc_data)) == true)
			 {
			 				 cvc2_present = 0;

			 }
			 if(Isallzeros(&auth_tx.TLF01_details.interest_rate,strlen(auth_tx.TLF01_details.interest_rate)) == true)
			 {
			 				 cvc2_present = 0;

			 }
		 }
		 /*message loop to send cvc1 and/or cvc2 verification messages to Racal and receive responses*/
		 for ( loopidx = 0; loopidx < 2; loopidx++)
		 {
			 if (loopidx == 0 && track_present) /*cvc1 is present and needs to be verified*/
			 {
				 /*Build message and send to Racal.  */
				 memset (&output,0,sizeof(output));
				 strcat(output, msgHeader);
				 strcat(output, cmdCode);
				 strcat(output, auth_tx.CAF01_details.cvk[0].cvkA);
				 strcat(output, auth_tx.CAF01_details.cvk[0].cvkB);
				 strcat(output, cvc1_data);
				 strcat(output, auth_tx.TLF01_details.card_num);
				 strcat(output, delimiter);
				 strcat(output, auth_tx.TLF01_details.exp_date);
				 strcat(output, cvc1_serviceCode);
			 }
			 else /*cvc2 needs to be verifed if present*/
			 {
				 /*Build message for cvc2 verification and send to Racal*/
				 memset (&output,0,sizeof(output));
				 strcat(output, msgHeader);
				 strcat(output, cmdCode);
				 strcat(output, auth_tx.CAF01_details.cvk[0].cvkA);
				 strcat(output, auth_tx.CAF01_details.cvk[0].cvkB);
				 strcat(output, cvc2_data);
				 strcat(output, auth_tx.TLF01_details.card_num);
				 strcat(output, delimiter);
				 strcat(output, auth_tx.TLF01_details.exp_date);
				 strcat(output, cvc2_serviceCode);

				  cvc2_sent = 1;
			 }


			 /*if the output is bigger then 150 do not send it out. */
			 if (strlen(output) > 150)
			 {
				racal_log_message( 2, 2, "mag-strip Cmd Msg to long.","racal_Verify_CVV_CVC_CSC_CAV_JCB_Keyblock",1 );
				if (cvc2_sent)
					return(CVC2_TRANSACTION_NOT_VERIFIED);
				else
					return(TRANSACTION_NOT_VERIFIED);
			 }

			 memset(BUFFER,0,sizeof(BUFFER));
			 sprintf(BUFFER, "BEFORE-PORT: %s", output);
			 MYPRINT(BUFFER);

			 /*The message is built.  Send it over to the Racal box.*/
			 /*Once the a response is recieved, check for the message header,*/
			 /*response code, and the error code.  If it does not exactly match,*/
			 /*send TRANSACTION_NOT_VERIFIED back.*/
			 racal_log_message( 2, 2, output,"racal_Verify_CVV_CVC_CSC_CAV_JCB_Keyblock",1 );
			 if (racalSendToPort(output) == TRANSACTION_VERIFIED)
			 {
				/*parse the message check for the message header.*/
				memset(&dummy,0,Dummy_Size);
				strncpy(dummy,returnString,4);
				if(strcmp(dummy,msgHeader) != 0)
				{
				   racal_log_message( 2, 2, "mag-strip Msg Header mis-match.","racal_Verify_CVV_CVC_CSC_CAV_JCB_Keyblock",1 );
				   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}

				/*parse the message check for the response code.*/
				memset(&dummy,0,Dummy_Size);
				strncpy(dummy,returnString + 4, 2);
				if(strcmp(dummy,"CZ") !=0)
				{
				   racal_log_message( 2, 2, "mag-strip Cmd Msg mis-match.","racal_Verify_CVV_CVC_CSC_CAV_JCB_Keyblock",1 );
				   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}

            /* Parse the message check for the error code. */
            memset(&dummy,0,Dummy_Size);
            strncpy(dummy,returnString + 6, 2);
            if(strcmp(dummy,"00") == 0)
            {
               if (cvc2_sent)
                  return(CVC2_TRANSACTION_VERIFIED);
               else
                  if(!cvc2_present)
                     return (TRANSACTION_VERIFIED); /*cvc1 valid*/
                  else
                     continue; /*continue with loop to verify cvc2*/
            }

				if(strcmp(dummy,"01") == 0)
				{
				   racal_log_message( 2, 2, "CVV-VERIFY failure.","racal_Verify_CVV_CVC_CSC_CAV_JCB_Keyblock",1 );
				   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}
				if(strcmp(dummy,"10") == 0)
            {
					racal_log_message( 2, 2, "CVV-VERIFY CVK A or B parity error.","racal_Verify_CVV_CVC_CSC_CAV_JCB_Keyblock",1 );
               	   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}

				if(strcmp(dummy,"12") == 0)
				{
				   racal_log_message( 2, 2, "CVV-VERIFY No Key loaded in user storage.","racal_Verify_CVV_CVC_CSC_CAV_JCB_Keyblock",1 );
				   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}

				if(strcmp(dummy,"13") == 0)
				{
				   racal_log_message( 2, 2, "CVV-VERIFY LMK error report to superviser.","racal_Verify_CVV_CVC_CSC_CAV_JCB_Keyblock",1 );
				   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}

				if(strcmp(dummy,"15") == 0)
				{
				   racal_log_message( 2, 2, "CVV-VERIFY Error in input data.","racal_Verify_CVV_CVC_CSC_CAV_JCB_Keyblock",1 );
				   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}

				if(strcmp(dummy,"21") == 0)
				{
				   racal_log_message( 2, 2, "CVV-VERIFY Invalid user storage index.","racal_Verify_CVV_CVC_CSC_CAV_JCB_Keyblock",1 );
				   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}

			 }
			 else  /* else for: if (racalSendToPort(output) == TRANSACTION_VERIFIED)*/
			 {
				   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
			 }

		 }/*end for loop*/

		 return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
         break;
		         /********************end of Visa Case*****************************************/


   default:
      racal_log_message( 2, 2, "Card Type Undefined.","racal_Verify_CVV_CVC_CSC_CAV_JCB_Keyblock",1 );
      return(TRANSACTION_NOT_VERIFIED);


   }/*end of switch statement.*/

}/*racal_Verify_CVV_CVC_CSC_CAV_JCB_Keyblock*/

/*************************************************************************************/
int racal_Verify_CVV_CVC_CSC_CAV_UPI_Keyblock()
{
   char msgHeader[5]={0};
   char cmdCode[3]={0};
   char cvc1_serviceCode[4]={0};
   char cvc2_serviceCode[4]={0};
   char delimiter[2]={0};
   char output[200]={0};
   char cardType = 'Z';
   char csc[6]={0};
   char cvc1_data[4]={0};
   char cvc2_data[4]={0};
   int  track_present = 0;
   int  cvc2_present = 0;
   int  cvc2_sent = 0;
   int  loopidx = 0;
   int  cnt=0;
   int  result=0;
   int  jcnt=0;
   char BUFFER [256]={0};
   char strTemp[50] = {0};
   CHAR *tempptr;
   CHAR fieldSeperator[2]={0};
   CHAR csc_result5[6]={0};
   CHAR csc_result4[5]={0};
   CHAR csc_result3[4]={0};
   INT 	flag=0;
   INT  AEVV_result_code;
   CHAR AEVV_second_factor[3]={0};
   CHAR AEVV_bin_second_factor[2]={0};
   CHAR AEVV_iCSC[5]={0};
   CHAR AEVV_bin_iCSC[3]={0};
   CHAR AEVV_UPN[5]={0};
   CHAR AEVV_bin_UPN[3]={0};
   INT  temp_out_len=0;

   memset (&msgHeader,0,sizeof(msgHeader));
   memset (&cmdCode,0,sizeof(cmdCode));
   memset (&delimiter,0,sizeof(delimiter));
   memset (&output,0,sizeof(output));
   memset (&returnString,0,ReturnString_Size);
   memset (&csc,0,sizeof(csc));

   strcpy (msgHeader ,"1234");

   if (strcmp(auth_tx.BIN01_details.card_family,CARD_VISA) == 0)
      cardType = 'V';

   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_MASTERCARD) == 0)
      cardType = 'M';

   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_AMEX) == 0)
      cardType = 'A';

   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_JCB) == 0)
      cardType = 'J';
   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_CUP) == 0)
		cardType = 'C';
   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_FLEET) == 0)
  		cardType = 'F';
   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_DINERS) == 0)
     		cardType = 'D';

   switch(cardType)
   {
      case 'A':

         /***********************Begin AMEX*********************************/
         strcat(output,msgHeader);
         strcat(output,"RY"); /* Changed Command code RY4- TF Phani*/
         strcat(output,"4");
		 if(strlen(auth_tx.int_net_facility_data) > 8)
		 {
		 /* AEVV verification value */
		 	strcat(output,"3");
			flag = 3;
			AEVV_result_code =  auth_tx.int_net_facility_data[11];
			memcpy(AEVV_bin_second_factor,&auth_tx.int_net_facility_data[12],1);
			bcd_to_asc(AEVV_bin_second_factor,1,AEVV_second_factor,2);

			memcpy(AEVV_bin_iCSC,&auth_tx.int_net_facility_data[14],2);
			bcd_to_asc(AEVV_bin_iCSC,2,AEVV_iCSC,4);
			memcpy(AEVV_bin_UPN,&auth_tx.int_net_facility_data[16],2);
			bcd_to_asc(AEVV_bin_UPN,2,AEVV_UPN,4);

		 }
		 else
         strcat(output,"0");

         /*check to see if the value of the CSC is in on the TRACK one.*/
         if(strlen(auth_tx.TLF01_details.track1) != 0)
         {
            /*if you have made it this far, info is on track one.*/
            /*check track one.  make sure it is not empty.
               farad 4/20/1999.  The value of the field seperator could changed.
                Find field separator first, then pass on to the parseTrack1 function.
            strcpy(tempptr,auth_tx.TLF01_details.track1);*/
            tempptr = auth_tx.TLF01_details.track1;
            for(jcnt=0; jcnt<(INT)strlen(auth_tx.TLF01_details.track1); jcnt++ )
            {
               if ( isalnum(*tempptr) )
                 ++tempptr;
               else
                break;
            }

            if (tempptr != NULL)
               strncpy(fieldSeperator,tempptr,1);
            else
               return(TRANSACTION_NOT_VERIFIED);

            memset(&dummy,0,Dummy_Size);
            if (1 == parseTrackOne(auth_tx.TLF01_details.track1,dummy ,fieldSeperator,auth_tx.CAF01_details.cvv_offset_tr1, auth_tx.CAF01_details.cvv_length))
            {
               strcpy(csc,dummy);
            }
            else
               return(TRANSACTION_NOT_VERIFIED);
         }
         else if(strlen(auth_tx.TLF01_details.track2) != 0)
         {
            /*if you have made it this far, info is on track two.*/
            /*check track two, make sure it is not empty.

            farad 4/19/1999.  The value of the field seperator could changed.
             Find field separator first, then pass on to the parseTrack2 function.
            strcpy(tempptr,auth_tx.TLF01_details.track2);*/
            tempptr = auth_tx.TLF01_details.track2;
            for(jcnt=0; jcnt<(INT)strlen(auth_tx.TLF01_details.track2); jcnt++ )
            {
               if ( isalnum(*tempptr) )
                 ++tempptr;
               else
                break;
            }

            if (tempptr != NULL)
               strncpy(fieldSeperator,tempptr,1);
            else
               return(TRANSACTION_NOT_VERIFIED);


            memset(&dummy,0,Dummy_Size);
            if (1 == parseTrackTwo(auth_tx.TLF01_details.track2,dummy ,fieldSeperator,auth_tx.CAF01_details.cvv_offset_tr2, auth_tx.CAF01_details.cvv_length))
            {
               strcpy(csc,dummy);
            }
            else
               return(TRANSACTION_NOT_VERIFIED);
         }
         else
         {
            /* This is a manually entered transaction.
             * Note that Amex can have 4-digit CSC's. Since
             * cvc_data is not large enough for these, the
             * field 'interest_rate' is used.
             */
            if ( auth_tx.TLF01_details.cvc_data[0] != 0x00 )
               strcpy( csc, auth_tx.TLF01_details.cvc_data );
            else if ( auth_tx.TLF01_details.interest_rate[0] != 0x00 )
               strcpy( csc, auth_tx.TLF01_details.interest_rate );

			if(csc[0] == 'S') /* TF Phani First byte is "S" means manulally keyed txn */
				strcpy(csc,&csc[1]);


         }
		/*TF Phani - CSCK value should follow by the U or T, U for Double , T for triple length
		case 32: key scheme = "U"
		case 48: key scheme = "T" */

         /*enter the CSCK value 32H */
         if(strlen(auth_tx.CAF01_details.csck) != 0)
         {	if(auth_tx.CAF01_details.aba_nbr[0] == '1' || auth_tx.CAF01_details.aba_nbr[0] == 1)
         	  strcat(output,"U");
            strcat(output,auth_tx.CAF01_details.csck);
         }
         else
         {
            racal_log_message( 2, 2, "mag-strip caf01-csck blank.","racal_Verify_CVV_CVC_CSC_CAV_UPI_Keyblock",1 );
            return(TRANSACTION_NOT_VERIFIED);
         }

         /*the account number is left-justified and zero-filled if less then 19.*/
         if (strlen(auth_tx.TLF01_details.card_num) != 0)
         {
            strcat(output,auth_tx.TLF01_details.card_num);
            if(strlen(auth_tx.TLF01_details.card_num) != 19)
               for(cnt = 19 ; (INT)strlen(auth_tx.TLF01_details.card_num) < cnt ;cnt--)
                  strcat(output,"0");
         }
         else
         {
            racal_log_message( 2, 2, "mag-strip card_num blank.","racal_Verify_CVV_CVC_CSC_CAV_UPI_Keyblock",1 );
            return(TRANSACTION_NOT_VERIFIED);
         }
         /*do the expiration date.*/
			if(flag == 3 && strlen(AEVV_UPN) > 0)
			{
			strncat(output,AEVV_UPN,4);
			}
		 else if(strlen(auth_tx.TLF01_details.exp_date) != 0)
            strcat(output,auth_tx.TLF01_details.exp_date);
         else
         {
        	 racal_log_message( 2, 2, "mag-strip exp_date blank.","racal_Verify_CVV_CVC_CSC_CAV_UPI_Keyblock",1 );
        	 return(TRANSACTION_NOT_VERIFIED);
         }
		/* TF Phani- Add CSC value*/
		if(flag == 3)
		{
			/* AEVV value */
			temp_out_len = strlen(output);

			sprintf(&output[temp_out_len],"%1d",AEVV_result_code);
			strcat(output,AEVV_second_factor);
			strncat(output,&AEVV_iCSC[1],3);
		}
        else if(strlen(csc) != 0)
		{ if(strlen(csc) == 5)
		  {strcat(output,csc);
		   strcat(output,"FFFFFFF");
          }else if(strlen(csc) == 4)
		  {strcat(output,"FFFFF");
		  strcat(output,csc);
		  strcat(output,"FFF");
          }else if(strlen(csc) == 3)
		  {strcat(output,"FFFFF");
		   strcat(output,"FFFF");
		   strcat(output,csc);
          }

		}
		 else
			strcat(output,"FFFFFFFFFFFF");
		printf("CSC value : %s \n",csc);
		printf("Output : %s \n",output);

		sprintf(strTemp, " AMEX 3CSC to racal RY: %s", output);
		racal_log_message( 2, 2,strTemp,"racal_Verify_CVV_CVC_CSC_CAV_UPI_Keyblock",1 );

         /*if the output is bigger then 150 do not send it out.*/
         if (strlen(output) > 150)
         {
            racal_log_message( 2, 2, "mag-strip Cmd Msg to long.","racal_Verify_CVV_CVC_CSC_CAV_UPI_Keyblock",1 );
            return(TRANSACTION_NOT_VERIFIED);
         }

         /*send the message to the racal box.*/
         if (racalSendToPort(output) == TRANSACTION_VERIFIED)
         {
        	 sprintf(strTemp, " output of racal cmnd RY: %s", returnString);
        	 racal_log_message( 2, 2,strTemp,"racal_Verify_CVV_CVC_CSC_CAV_UPI_Keyblock",1 );

            /*parse the message check for the message header.*/
            memset(&dummy,0,Dummy_Size);
            strncpy(dummy,returnString,4);
            if(strcmp(dummy,msgHeader) != 0)
            {
               racal_log_message( 2, 2, "mag-strip Cmd header mis-match.","racal_Verify_CVV_CVC_CSC_CAV_UPI_Keyblock",1 );
               return(TRANSACTION_NOT_VERIFIED);
            }
            /*parse the message check for the response code.*/
            memset(&dummy,0,Dummy_Size);
            strncpy(dummy,returnString + 4, 2);
            if(strcmp(dummy,"RZ") !=0)
            {
               racal_log_message( 2, 2, "mag-strip Cmd Code mis-match.","racal_Verify_CVV_CVC_CSC_CAV_UPI_Keyblock",1 );
               return(TRANSACTION_NOT_VERIFIED);
            }

            /*parse the message check for the error code.*/
            memset(&dummy,0,Dummy_Size);
            strncpy(dummy,returnString + 6, 2);

			printf("Return from Racal : %s \n",returnString);
            /*if this value is not 00 reject the transaction.*/
            if(strcmp(dummy,"00") != 0)
            {
               if(strcmp(dummy,"10") == 0)
               {
            	  racal_log_message( 2, 2, "mag-strip CSCK parity error.","racal_Verify_CVV_CVC_CSC_CAV_UPI_Keyblock",1 );
			   }
               if(strcmp(dummy,"12") == 0)
                  racal_log_message( 2, 2, "mag-strip No Keys in user storage.","racal_Verify_CVV_CVC_CSC_CAV_UPI_Keyblock",1 );
               if(strcmp(dummy,"13") == 0)
                  racal_log_message( 2, 2, "mag-strip LMK parity error.","racal_Verify_CVV_CVC_CSC_CAV_UPI_Keyblock",1 );
               if(strcmp(dummy,"15") == 0)
                  racal_log_message( 2, 2, "mag-strip Error in input data.","racal_Verify_CVV_CVC_CSC_CAV_UPI_Keyblock",1 );
			   if(strcmp(dummy,"01") == 0)
			   	{
				   strcpy(auth_tx.TLF01_details.response_text,"Invalid 4 CSC");
			   	   racal_log_message( 2, 2, "Invalid CSC/CVV/3D sec.","racal_Verify_CVV_CVC_CSC_CAV_UPI_Keyblock",1 );
			   	}

			   return(TRANSACTION_NOT_VERIFIED);
            }

            /*parse the message mode for the error code.*/
            memset(&dummy,0,Dummy_Size);
            strncpy(dummy,returnString + 8, 1);
            if(strcmp(dummy,"4") !=0)
            {
               racal_log_message( 2, 2, "mag-strip Msg Mode mis-match.","racal_Verify_CVV_CVC_CSC_CAV_UPI_Keyblock",1 );
               return(TRANSACTION_NOT_VERIFIED);
            }


#ifdef _DEBUG
            racal_log_message( 2, 1, "mag-strip PASSED.","racal_Verify_CVV_CVC_CSC_CAV_UPI_Keyblock",1 );
#endif
            return(TRANSACTION_VERIFIED);

         }/*end of if racalSendToPort*/
         else
            return(TRANSACTION_NOT_VERIFIED);


         /****************************end AMEX***************************************/

         break;
	  case 'C':
	  case 'D':
      case 'J':
         /*drop down to VISA for this function.*/
         /*break;*/

      case 'M':

         /*drop down to VISA for this function.*/
         /*break;*/
      case 'F':
    	  /*This is Private lable transaction.*/
    	  /*drop down to VISA for this function.*/
    	  /*break;*/
      case 'V':

         /*****************************begin Visa***************************************/
         /*The VISA, MC, and the JCB are all the same.*/
         /*The rest of this code would deal with them.*/


    		  strcpy (cmdCode,"CY");

         strcpy (delimiter,";");

         /*edit CVK A */
         if(strlen(auth_tx.CAF01_details.cvk[0].cvkA) == 0)
		 {
            racal_log_message( 2, 2, "mag-strip cvkA is blank.","racal_Verify_CVV_CVC_CSC_CAV_UPI_Keyblock",1 );
            return(TRANSACTION_NOT_VERIFIED);
         }
         /*edit CVK B */
         if(strlen(auth_tx.CAF01_details.cvk[0].cvkB) == 0)
         {
            racal_log_message( 2, 2, "mag-strip cvkB is blank.","racal_Verify_CVV_CVC_CSC_CAV_UPI_Keyblock",1 );
            return(TRANSACTION_NOT_VERIFIED);
         }

		 /*edit card number*/
	 	 if(strlen(auth_tx.TLF01_details.card_num) == 0 )
         {
            racal_log_message( 2, 2, "mag-strip card_num is blank.","racal_Verify_CVV_CVC_CSC_CAV_UPI_Keyblock",1 );
            return(TRANSACTION_NOT_VERIFIED);
         }

		 /*edit expiration date*/
         if(strlen(auth_tx.TLF01_details.exp_date) == 0)
         {
            racal_log_message( 2, 2, "mag-strip exp_date is blank.","racal_Verify_CVV_CVC_CSC_CAV_UPI_Keyblock",1 );
            return(TRANSACTION_NOT_VERIFIED);
         }
		/* Fall Back CVV postion is 14*/
		 if (strcmp(auth_tx.BIN01_details.card_family,CARD_MASTERCARD) == 0 &&
		 	(strncmp(auth_tx.TLF01_details.pos_entry_mode,"080",3) == 0 || strncmp(auth_tx.TLF01_details.pos_entry_mode,"008",3) == 0)
			)
		 /*	strncmp(auth_tx.TLF01_details.pos_entry_mode,"002",3) == 0 ) */

		 	{
		 		strcpy(auth_tx.CAF01_details.cvv_offset_tr1,"14");
		 		strcpy(auth_tx.CAF01_details.cvv_offset_tr2,"14");
		 	}

         /*get TrackI CVV and service code*/
         if(strlen(auth_tx.TLF01_details.track1) != 0)	/*check track one.  make sure it is not empty.*/
		 {
            track_present = 1;
			/*get the CVV value */
            memset(&dummy,0,Dummy_Size);
            if (1 == parseTrackOne(auth_tx.TLF01_details.track1,dummy ,"^",
                   auth_tx.CAF01_details.cvv_offset_tr1, auth_tx.CAF01_details.cvv_length))
                strcpy(cvc1_data,dummy);
            else
             return(TRANSACTION_NOT_VERIFIED);

            /*get service code */
            memset(&dummy,0,Dummy_Size);
            if (1 == parseTrackOne(auth_tx.TLF01_details.track1,dummy ,"^",
						auth_tx.CAF01_details.service_code_offset_tr1, "3"))
				   strcpy(cvc1_serviceCode,dummy);
			else
               return(TRANSACTION_NOT_VERIFIED);
		 }
         else
         /*or get TrackII CVV and service code*/
         if(strlen(auth_tx.TLF01_details.track2) != 0) /*check track two. Make sure it is not empty.*/
		 {
			track_present = 1;
			/*get the CVV value */
            memset(&dummy,0,Dummy_Size);
            if (1 == parseTrackTwo(auth_tx.TLF01_details.track2,dummy ,"=",
					auth_tx.CAF01_details.cvv_offset_tr2, auth_tx.CAF01_details.cvv_length))
           	   strcpy(cvc1_data,dummy);
            else
                  return(TRANSACTION_NOT_VERIFIED);

               /*get the service code.*/
               memset(&dummy,0,Dummy_Size);
               if (1 == parseTrackTwo(auth_tx.TLF01_details.track2,dummy ,"=",
                     auth_tx.CAF01_details.service_code_offset_tr2, "3"))
                  strcpy(cvc1_serviceCode,dummy);
               else
                  return(TRANSACTION_NOT_VERIFIED);

		 }

		 /*Get cvc2 if present*/
		 if(strlen(auth_tx.TLF01_details.cvc_data) != 0)
		 {

				 strcpy(cvc2_data, auth_tx.TLF01_details.cvc_data);
				 strcpy(cvc2_serviceCode, "000");
				 cvc2_present = 1;

		 }
		 /*Get cvc2 if present, if txn is STAND IN or come with other value 29-Jul-2009
		   If first byte is space(0x20) cvv data copied in the cvc_data
		   other wise cvv data copied in interest_rate. That values ATP should send as CVV2 data*/

		 else if(strlen(auth_tx.TLF01_details.interest_rate) != 0)
		 {


				 strcpy(cvc2_data, &auth_tx.TLF01_details.interest_rate[1]);
				 strcpy(cvc2_serviceCode, "000");
				 cvc2_present = 1;

		 }

		 /*If neither cvc1 or cvc2 is present, return no value*/
		 if( !track_present && !cvc2_present)
			 return (TRANSACTION_VERIFIED);
		 /* check the CVC data and interest rate which have the cvv2 value,
		 	If it has all zeros, it is not a valid cvv2 hence make cvv2_present has 0*/
		 if(strlen(cvc1_data)!=0)
		 {

			 if(Isallzeros(&auth_tx.TLF01_details.cvc_data,strlen(auth_tx.TLF01_details.cvc_data)) == true)
			 {
			 				 cvc2_present = 0;

			 }
			 if(Isallzeros(&auth_tx.TLF01_details.interest_rate,strlen(auth_tx.TLF01_details.interest_rate)) == true)
			 {
			 				 cvc2_present = 0;

			 }
		 }
		 /*message loop to send cvc1 and/or cvc2 verification messages to Racal and receive responses*/
		 for ( loopidx = 0; loopidx < 2; loopidx++)
		 {
			 if (loopidx == 0 && track_present) /*cvc1 is present and needs to be verified*/
			 {
				 /*Build message and send to Racal.  */
				 memset (&output,0,sizeof(output));
				 strcat(output, msgHeader);
				 strcat(output, cmdCode);
				 strcat(output, auth_tx.CAF01_details.cvk[0].cvkA);
				 strcat(output, auth_tx.CAF01_details.cvk[0].cvkB);
				 strcat(output, cvc1_data);
				 strcat(output, auth_tx.TLF01_details.card_num);
				 strcat(output, delimiter);
				 strcat(output, auth_tx.TLF01_details.exp_date);
				 strcat(output, cvc1_serviceCode);
			 }
			 else /*cvc2 needs to be verifed if present*/
			 {
				 /*Build message for cvc2 verification and send to Racal*/
				 memset (&output,0,sizeof(output));
				 strcat(output, msgHeader);
				 strcat(output, cmdCode);
				 strcat(output, auth_tx.CAF01_details.cvk[0].cvkA);
				 strcat(output, auth_tx.CAF01_details.cvk[0].cvkB);
				 strcat(output, cvc2_data);
				 strcat(output, auth_tx.TLF01_details.card_num);
				 strcat(output, delimiter);
				 strcat(output, auth_tx.TLF01_details.exp_date);
				 strcat(output, cvc2_serviceCode);

				  cvc2_sent = 1;
			 }


			 /*if the output is bigger then 150 do not send it out. */
			 if (strlen(output) > 150)
			 {
				racal_log_message( 2, 2, "mag-strip Cmd Msg to long.","racal_Verify_CVV_CVC_CSC_CAV_UPI_Keyblock",1 );
				if (cvc2_sent)
					return(CVC2_TRANSACTION_NOT_VERIFIED);
				else
					return(TRANSACTION_NOT_VERIFIED);
			 }

			 memset(BUFFER,0,sizeof(BUFFER));
			 sprintf(BUFFER, "BEFORE-PORT: %s", output);
			 MYPRINT(BUFFER);

			 /*The message is built.  Send it over to the Racal box.*/
			 /*Once the a response is recieved, check for the message header,*/
			 /*response code, and the error code.  If it does not exactly match,*/
			 /*send TRANSACTION_NOT_VERIFIED back.*/
			 racal_log_message( 2, 2, output,"racal_Verify_CVV_CVC_CSC_CAV_UPI_Keyblock",1 );
			 if (racalSendToPort(output) == TRANSACTION_VERIFIED)
			 {
				/*parse the message check for the message header.*/
				memset(&dummy,0,Dummy_Size);
				strncpy(dummy,returnString,4);
				if(strcmp(dummy,msgHeader) != 0)
				{
				   racal_log_message( 2, 2, "mag-strip Msg Header mis-match.","racal_Verify_CVV_CVC_CSC_CAV_UPI_Keyblock",1 );
				   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}

				/*parse the message check for the response code.*/
				memset(&dummy,0,Dummy_Size);
				strncpy(dummy,returnString + 4, 2);
				if(strcmp(dummy,"CZ") !=0)
				{
				   racal_log_message( 2, 2, "mag-strip Cmd Msg mis-match.","racal_Verify_CVV_CVC_CSC_CAV_UPI_Keyblock",1 );
				   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}

            /* Parse the message check for the error code. */
            memset(&dummy,0,Dummy_Size);
            strncpy(dummy,returnString + 6, 2);
            if(strcmp(dummy,"00") == 0)
            {
               if (cvc2_sent)
                  return(CVC2_TRANSACTION_VERIFIED);
               else
                  if(!cvc2_present)
                     return (TRANSACTION_VERIFIED); /*cvc1 valid*/
                  else
                     continue; /*continue with loop to verify cvc2*/
            }

				if(strcmp(dummy,"01") == 0)
				{
				   racal_log_message( 2, 2, "CVV-VERIFY failure.","racal_Verify_CVV_CVC_CSC_CAV_UPI_Keyblock",1 );
				   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}
				if(strcmp(dummy,"10") == 0)
            {
					racal_log_message( 2, 2, "CVV-VERIFY CVK A or B parity error.","racal_Verify_CVV_CVC_CSC_CAV_UPI_Keyblock",1 );
               	   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}

				if(strcmp(dummy,"12") == 0)
				{
				   racal_log_message( 2, 2, "CVV-VERIFY No Key loaded in user storage.","racal_Verify_CVV_CVC_CSC_CAV_UPI_Keyblock",1 );
				   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}

				if(strcmp(dummy,"13") == 0)
				{
				   racal_log_message( 2, 2, "CVV-VERIFY LMK error report to superviser.","racal_Verify_CVV_CVC_CSC_CAV_UPI_Keyblock",1 );
				   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}

				if(strcmp(dummy,"15") == 0)
				{
				   racal_log_message( 2, 2, "CVV-VERIFY Error in input data.","racal_Verify_CVV_CVC_CSC_CAV_UPI_Keyblock",1 );
				   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}

				if(strcmp(dummy,"21") == 0)
				{
				   racal_log_message( 2, 2, "CVV-VERIFY Invalid user storage index.","racal_Verify_CVV_CVC_CSC_CAV_UPI_Keyblock",1 );
				   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}

			 }
			 else  /* else for: if (racalSendToPort(output) == TRANSACTION_VERIFIED)*/
			 {
				   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
			 }

		 }/*end for loop*/

		 return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
         break;
		         /********************end of Visa Case*****************************************/


   default:
      racal_log_message( 2, 2, "Card Type Undefined.","racal_Verify_CVV_CVC_CSC_CAV_UPI_Keyblock",1 );
      return(TRANSACTION_NOT_VERIFIED);


   }/*end of switch statement.*/

}/*racal_Verify_CVV_CVC_CSC_CAV_UPI_Keyblock*/

/*************************************************************************************/
int racal_Verify_CVV_CVC_CSC_CAV_DCI_Keyblock()
{
	char msgHeader[5]={0};
	char cmdCode[3]={0};
	char cvc1_serviceCode[4]={0};
	char cvc2_serviceCode[4]={0};
	char delimiter[2]={0};
	char output[513]={0};
	char cvc1_data[4]={0};
	char cvc2_data[4]={0};
	int  track_present = 0;
	int  cvc2_present = 0;
	int  cvc2_sent = 0;
	int  loopidx = 0;

	memset (&returnString,0,ReturnString_Size);

	strcpy (msgHeader ,"1234");
	strcpy (cmdCode,"CY");
	strcpy (delimiter,";");

	if (strlen (caf01_keyblock.Cvk_Mag_Keyblock) == 0)
	{
	   racal_log_message( 2, 2, "mag-strip CKV is blank.","racal_Verify_CVV_CVC_CSC_CAV_DCI_Keyblock",1 );
	   return(TRANSACTION_NOT_VERIFIED);
	}

	if(strlen(auth_tx.TLF01_details.card_num) == 0 )
	{
		racal_log_message( 2, 2, "mag-strip card_num is blank.","racal_Verify_CVV_CVC_CSC_CAV_DCI_Keyblock",1 );
		return(TRANSACTION_NOT_VERIFIED);
	}

	if(strlen(auth_tx.TLF01_details.exp_date) == 0)
	{
		racal_log_message( 2, 2, "mag-strip exp_date is blank.","racal_Verify_CVV_CVC_CSC_CAV_DCI_Keyblock",1 );
		return(TRANSACTION_NOT_VERIFIED);
	}

	/*get TrackI CVV and service code*/
	if(strlen(auth_tx.TLF01_details.track1) != 0)	/*check track one.  make sure it is not empty.*/
	{
		track_present = 1;
		/*get the CVV value */
		memset(&dummy,0,Dummy_Size);
		if (1 == parseTrackOne(auth_tx.TLF01_details.track1,dummy ,"^",
				auth_tx.CAF01_details.cvv_offset_tr1, auth_tx.CAF01_details.cvv_length))
		{
			strcpy(cvc1_data,dummy);
		}
		else
		{
			return(TRANSACTION_NOT_VERIFIED);
		}

		/*get service code */
		memset(&dummy,0,Dummy_Size);
		if (1 == parseTrackOne(auth_tx.TLF01_details.track1,dummy ,"^",
				auth_tx.CAF01_details.service_code_offset_tr1, "3"))
		{
			strcpy(cvc1_serviceCode,dummy);
		}
		else
		{
			return(TRANSACTION_NOT_VERIFIED);
		}
	}
	else if(strlen(auth_tx.TLF01_details.track2) != 0) /*check track two. Make sure it is not empty.*/
	{
		track_present = 1;
		/*get the CVV value */
		memset(&dummy,0,Dummy_Size);
		if (1 == parseTrackTwo(auth_tx.TLF01_details.track2,dummy ,"=",
				auth_tx.CAF01_details.cvv_offset_tr2, auth_tx.CAF01_details.cvv_length))
		{
			strcpy(cvc1_data,dummy);
		}
		else
		{
			return(TRANSACTION_NOT_VERIFIED);
		}

		/*get the service code.*/
		memset(&dummy,0,Dummy_Size);
		if (1 == parseTrackTwo(auth_tx.TLF01_details.track2,dummy ,"=",
				auth_tx.CAF01_details.service_code_offset_tr2, "3"))
		{
			strcpy(cvc1_serviceCode,dummy);
		}
		else
		{
			return(TRANSACTION_NOT_VERIFIED);
		}
	}

	/*Get cvc2 if present*/
	if(strlen(auth_tx.TLF01_details.cvc_data) != 0)
	{
		strcpy(cvc2_data, auth_tx.TLF01_details.cvc_data);
		strcpy(cvc2_serviceCode, "000");
		cvc2_present = 1;
	}

	/*Get cvc2 if present, if txn is STAND IN or come with other value 29-Jul-2009
	If first byte is space(0x20) cvv data copied in the cvc_data
	other wise cvv data copied in interest_rate. That values ATP should send as CVV2 data*/
	else if(strlen(auth_tx.TLF01_details.interest_rate) != 0)
	{
		strcpy(cvc2_data, &auth_tx.TLF01_details.interest_rate[1]);
		strcpy(cvc2_serviceCode, "000");
		cvc2_present = 1;
	}

	/*If neither cvc1 or cvc2 is present, return no value*/
	if( !track_present && !cvc2_present)
	{
		return (TRANSACTION_VERIFIED);
	}

	/* check the CVC data and interest rate which have the cvv2 value,
	If it has all zeros, it is not a valid cvv2 hence make cvv2_present has 0*/
	if(strlen(cvc1_data)!=0)
	{
		if(Isallzeros(&auth_tx.TLF01_details.cvc_data,strlen(auth_tx.TLF01_details.cvc_data)) == true)
		{
			cvc2_present = 0;
		}
		if(Isallzeros(&auth_tx.TLF01_details.interest_rate,strlen(auth_tx.TLF01_details.interest_rate)) == true)
		{
			cvc2_present = 0;
		}
	}

	/*message loop to send cvc1 and/or cvc2 verification messages to Racal and receive responses*/
	for ( loopidx = 0; loopidx < 2; loopidx++)
	{
		if (loopidx == 0 && track_present) /*cvc1 is present and needs to be verified*/
		{
			/*Build message and send to Racal.  */
			memset (&output,0,sizeof(output));
			strcat(output, msgHeader);
			strcat(output, cmdCode);
			strncat(output, SOUURCE_TPK_KEYBLOCK,KEYBLOCK_KEY_FORMAT_LEN );
			strcat(output, caf01_keyblock.Cvk_Mag_Keyblock);
			strcat(output, cvc1_data);
			strcat(output, auth_tx.TLF01_details.card_num);
			strcat(output, delimiter);
			strcat(output, auth_tx.TLF01_details.exp_date);
			strcat(output, cvc1_serviceCode);
		}

		else /*cvc2 needs to be verifed if present*/
		{
			/*Build message for cvc2 verification and send to Racal*/
			memset (&output,0,sizeof(output));
			strcat(output, msgHeader);
			strcat(output, cmdCode);
			strncat( output, SOUURCE_TPK_KEYBLOCK,KEYBLOCK_KEY_FORMAT_LEN );
			strcat(output, caf01_keyblock.Cvk_Mag_Keyblock);
			strcat(output, cvc2_data);
			strcat(output, auth_tx.TLF01_details.card_num);
			strcat(output, delimiter);
			strcat(output, auth_tx.TLF01_details.exp_date);
			strcat(output, cvc2_serviceCode);

			cvc2_sent = 1;
		}

		/*if the output is bigger then 150 do not send it out. */
		if (strlen(output) > MAX_KEYBLOCK_CMD_LEN)
		{
			racal_log_message( 2, 2, "mag-strip Cmd Msg to long.","racal_Verify_CVV_CVC_CSC_CAV_DCI_Keyblock",1 );
			if (cvc2_sent)
			{
				return(CVC2_TRANSACTION_NOT_VERIFIED);
			}
			else
			{
				return(TRANSACTION_NOT_VERIFIED);
			}
		}

		/*The message is built.  Send it over to the Racal box.*/
		/*Once the a response is recieved, check for the message header,*/
		/*response code, and the error code.  If it does not exactly match,*/
		/*send TRANSACTION_NOT_VERIFIED back.*/
		racal_log_message( 2, 2, output,"racal_Verify_CVV_CVC_CSC_CAV_DCI_Keyblock",1 );
		if (racalSendToPort(output) == TRANSACTION_VERIFIED)
		{
			/*parse the message check for the message header.*/
			memset(&dummy,0,Dummy_Size);
			strncpy(dummy,returnString,4);
			if(strcmp(dummy,msgHeader) != 0)
			{
				racal_log_message( 2, 2, "mag-strip Msg Header mis-match.","racal_Verify_CVV_CVC_CSC_CAV_DCI_Keyblock",1 );
				if (cvc2_sent)
				{
					return(CVC2_TRANSACTION_NOT_VERIFIED);
				}
				else
				{
					return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}
			}

			/*parse the message check for the response code.*/
			memset(&dummy,0,Dummy_Size);
			strncpy(dummy,returnString + 4, 2);
			if(strcmp(dummy,"CZ") !=0)
			{
				racal_log_message( 2, 2, "mag-strip Cmd Msg mis-match.","racal_Verify_CVV_CVC_CSC_CAV_DCI_Keyblock",1 );
				if (cvc2_sent)
				{
					return(CVC2_TRANSACTION_NOT_VERIFIED);
				}
				else
				{
					return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}
			}

			/* Parse the message check for the error code. */
			memset(&dummy,0,Dummy_Size);
			strncpy(dummy,returnString + 6, 2);
			if(strcmp(dummy,"00") == 0)
			{
				if (cvc2_sent)
				{
					return(CVC2_TRANSACTION_VERIFIED);
				}
				else if(!cvc2_present)
				{
					return (TRANSACTION_VERIFIED); /*cvc1 valid*/
				}
				else
				{
					continue; /*continue with loop to verify cvc2*/
				}
			}

			if(strcmp(dummy,"01") == 0)
			{
				racal_log_message( 2, 2, "CVV-VERIFY failure.","racal_Verify_CVV_CVC_CSC_CAV_DCI_Keyblock",1 );
				if (cvc2_sent)
				{
					return(CVC2_TRANSACTION_NOT_VERIFIED);
				}
				else
				{
					return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}
			}

			if(strcmp(dummy,"10") == 0)
			{
				racal_log_message( 2, 2, "CVV-VERIFY CVK A or B parity error.","racal_Verify_CVV_CVC_CSC_CAV_DCI_Keyblock",1 );
				if (cvc2_sent)
				{
					return(CVC2_TRANSACTION_NOT_VERIFIED);
				}
				else
				{
					return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}
			}

			if(strcmp(dummy,"12") == 0)
			{
				racal_log_message( 2, 2, "CVV-VERIFY No Key loaded in user storage.","racal_Verify_CVV_CVC_CSC_CAV_DCI_Keyblock",1 );
				if (cvc2_sent)
				{
					return(CVC2_TRANSACTION_NOT_VERIFIED);
				}
				else
				{
					return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}
			}

			if(strcmp(dummy,"13") == 0)
			{
				racal_log_message( 2, 2, "CVV-VERIFY LMK error report to superviser.","racal_Verify_CVV_CVC_CSC_CAV_DCI_Keyblock",1 );
				if (cvc2_sent)
				{
					return(CVC2_TRANSACTION_NOT_VERIFIED);
				}
				else
				{
					return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}
			}

			if(strcmp(dummy,"15") == 0)
			{
				racal_log_message( 2, 2, "CVV-VERIFY Error in input data.","racal_Verify_CVV_CVC_CSC_CAV_DCI_Keyblock",1 );
				if (cvc2_sent)
				{
					return(CVC2_TRANSACTION_NOT_VERIFIED);
				}
				else
				{
					return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}
			}

			if(strcmp(dummy,"21") == 0)
			{
				racal_log_message( 2, 2, "CVV-VERIFY Invalid user storage index.","racal_Verify_CVV_CVC_CSC_CAV_DCI_Keyblock",1 );
				if (cvc2_sent)
				{
					return(CVC2_TRANSACTION_NOT_VERIFIED);
				}
				else
				{
					return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}
			}
		}

		else  /* else for: if (racalSendToPort(output) == TRANSACTION_VERIFIED)*/
		{
			if (cvc2_sent)
			{
				return(CVC2_TRANSACTION_NOT_VERIFIED);
			}
			else
			{
				return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
			}
		}

	}/*end for loop*/

	return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/

}/*racal_Verify_CVV_CVC_CSC_CAV_DCI_Keyblock*/


/*********************************************************/
int racal_Verify_JCB3DS2_CVC2_Keyblock()
{
   char msgHeader[5]={0};
   char cmdCode[3]={0};
   char cvc1_serviceCode[4]={0};
   char cvc2_serviceCode[4]={0};
   char delimiter[2]={0};
   char output[200];
   char cardType = 'Z';
   char csc[6]={0};
   char cvc1_data[4]={0};
   char cvc2_data[4]={0};
   int  track_present = 0;
   int  cvc2_present = 0;
   int  cvc2_sent = 0;
   int  loopidx = 0;
   int  cnt=0;
   int  result=0;
   int  jcnt=0;
   char BUFFER [256]={0};
   char strTemp[50] = {0};
   CHAR *tempptr;
   CHAR fieldSeperator[2]={0};
   CHAR csc_result5[6]={0};
   CHAR csc_result4[5]={0};
   CHAR csc_result3[4]={0};
   INT 	flag=0;
   INT  AEVV_result_code;
   CHAR AEVV_second_factor[3]={0};
   CHAR AEVV_bin_second_factor[2]={0};
   CHAR AEVV_iCSC[5]={0};
   CHAR AEVV_bin_iCSC[3];
   CHAR AEVV_UPN[5]={0};
   CHAR AEVV_bin_UPN[3];
   INT  temp_out_len=0;

   memset (&msgHeader,0,sizeof(msgHeader));
   memset (&cmdCode,0,sizeof(cmdCode));
   memset (&delimiter,0,sizeof(delimiter));
   memset (&output,0,sizeof(output));
   memset (&returnString,0,ReturnString_Size);
   memset (&csc,0,sizeof(csc));

   strcpy (msgHeader ,"1234");


   strcpy (cmdCode,"CY");
   strcpy (delimiter,";");

			           /*edit CVK A */
         if(strlen(auth_tx.CAF01_details.csck) == 0)
		 {
            racal_log_message( 2, 2, "mag-strip csck is blank.","racal_Verify_JCB3DS2_CVC2_Keyblock",1 );
            return(TRANSACTION_NOT_VERIFIED);
         }

         /*edit card number*/
	 	 if(strlen(auth_tx.TLF01_details.card_num) == 0 )
         {
            racal_log_message( 2, 2, "mag-strip card_num is blank.","racal_Verify_JCB3DS2_CVC2_Keyblock",1 );
            return(TRANSACTION_NOT_VERIFIED);
         }

		 /*edit expiration date*/
         if(strlen(auth_tx.TLF01_details.exp_date) == 0)
         {
            racal_log_message( 2, 2, "mag-strip exp_date is blank.","racal_Verify_JCB3DS2_CVC2_Keyblock",1 );
            return(TRANSACTION_NOT_VERIFIED);
         }

		 		 /*Get cvc2 if present*/
		 if(strlen(auth_tx.TLF01_details.cvc_data) != 0)
		 {

				 strcpy(cvc2_data, auth_tx.TLF01_details.cvc_data);
				 strcpy(cvc2_serviceCode, "000");
				 cvc2_present = 1;

		 }
		 else if(strlen(auth_tx.TLF01_details.interest_rate) != 0)
		 {


				 strcpy(cvc2_data, &auth_tx.TLF01_details.interest_rate[1]);
				 strcpy(cvc2_serviceCode, "000");
				 cvc2_present = 1;

		 }
		 		 /*If neither cvc1 or cvc2 is present, return no value*/
		 if( !track_present && !cvc2_present)
			 return (TRANSACTION_VERIFIED);

		for ( loopidx = 0; loopidx < 2; loopidx++)
		 {
			 if (loopidx == 0 && track_present) /*cvc1 is present and needs to be verified*/
			 {
				 /*Build message and send to Racal.  */
				 memset (&output,0,sizeof(output));
				 strcat(output, msgHeader);
				 strcat(output, cmdCode);
				 strcat(output, auth_tx.CAF01_details.csck);
				 strcat(output, cvc1_data);
				 strcat(output, auth_tx.TLF01_details.card_num);
				 strcat(output, delimiter);
				 strcat(output, auth_tx.TLF01_details.exp_date);
				 strcat(output, cvc1_serviceCode);
			 }
			 else /*cvc2 needs to be verifed if present*/
			 {
				 /*Build message for cvc2 verification and send to Racal*/
				 memset (&output,0,sizeof(output));
				 strcat(output, msgHeader);
				 strcat(output, cmdCode);
				 strcat(output, auth_tx.CAF01_details.cvk[0].cvkA);
				 strcat(output, auth_tx.CAF01_details.cvk[0].cvkB);
				 strcat(output, cvc2_data);
				 strcat(output, auth_tx.TLF01_details.card_num);
				 strcat(output, delimiter);
				 strcat(output, auth_tx.TLF01_details.exp_date);
				 strcat(output, cvc2_serviceCode);

				  cvc2_sent = 1;
			 }

			 if (strlen(output) > 150)
			 {
				racal_log_message( 2, 2, "mag-strip Cmd Msg to long.","racal_Verify_JCB3DS2_CVC2_Keyblock",1 );
				if (cvc2_sent)
					return(CVC2_TRANSACTION_NOT_VERIFIED);
				else
					return(TRANSACTION_NOT_VERIFIED);
			 }

			 memset(BUFFER,0,sizeof(BUFFER));
			 sprintf(BUFFER, "BEFORE-PORT: %s", output);
			 MYPRINT(BUFFER);

			 /*The message is built.  Send it over to the Racal box.*/
			 /*Once the a response is recieved, check for the message header,*/
			 /*response code, and the error code.  If it does not exactly match,*/
			 /*send TRANSACTION_NOT_VERIFIED back.*/
			 racal_log_message( 2, 2, output,"racal_Verify_JCB3DS2_CVC2_Keyblock",1 );
			 if (racalSendToPort(output) == TRANSACTION_VERIFIED)
			 {
				/*parse the message check for the message header.*/
				memset(&dummy,0,Dummy_Size);
				strncpy(dummy,returnString,4);
				if(strcmp(dummy,msgHeader) != 0)
				{
				   racal_log_message( 2, 2, "mag-strip Msg Header mis-match.","racal_Verify_JCB3DS2_CVC2_Keyblock",1 );
				   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}

				/*parse the message check for the response code.*/
				memset(&dummy,0,Dummy_Size);
				strncpy(dummy,returnString + 4, 2);
				if(strcmp(dummy,"CZ") !=0)
				{
				   racal_log_message( 2, 2, "mag-strip Cmd Msg mis-match.","racal_Verify_JCB3DS2_CVC2_Keyblock",1 );
				   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}

            /* Parse the message check for the error code. */
            memset(&dummy,0,Dummy_Size);
            strncpy(dummy,returnString + 6, 2);
            if(strcmp(dummy,"00") == 0)
            {
               if (cvc2_sent)
                  return(CVC2_TRANSACTION_VERIFIED);
               else
                  if(!cvc2_present)
                     return (TRANSACTION_VERIFIED); /*cvc1 valid*/
                  else
                     continue; /*continue with loop to verify cvc2*/
            }

				if(strcmp(dummy,"01") == 0)
				{
				   racal_log_message( 2, 2, "CVV-VERIFY failure.","racal_Verify_JCB3DS2_CVC2_Keyblock",1 );
				   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}
				if(strcmp(dummy,"10") == 0)
            {
					racal_log_message( 2, 2, "CVV-VERIFY CVK A or B parity error.","racal_Verify_JCB3DS2_CVC2_Keyblock",1 );
               	   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}

				if(strcmp(dummy,"12") == 0)
				{
				   racal_log_message( 2, 2, "CVV-VERIFY No Key loaded in user storage.","racal_Verify_JCB3DS2_CVC2_Keyblock",1 );
				   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}

				if(strcmp(dummy,"13") == 0)
				{
				   racal_log_message( 2, 2, "CVV-VERIFY LMK error report to superviser.","racal_Verify_JCB3DS2_CVC2_Keyblock",1 );
				   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}

				if(strcmp(dummy,"15") == 0)
				{
				   racal_log_message( 2, 2, "CVV-VERIFY Error in input data.","racal_Verify_JCB3DS2_CVC2_Keyblock",1 );
				   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}

				if(strcmp(dummy,"21") == 0)
				{
				   racal_log_message( 2, 2, "CVV-VERIFY Invalid user storage index.","racal_Verify_JCB3DS2_CVC2_Keyblock",1 );
				   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}

			 }
			 else  /* else for: if (racalSendToPort(output) == TRANSACTION_VERIFIED)*/
			 {
				   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
			 }

		 }/*end for loop*/

		 return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/


} /*racal_Verify_JCB3DS2_CVC2_Keyblock*/
/**********************************************/

int racal_Verify_ARQC_for_CUPEMV_Keyblock()
{
	char msgHeader[5]= {0};
	char delimiter[2]= {0};
	char output[500]= {0};
	INT  bin_len=0;
	INT  out_len=0;
	int	 cnt=0;
	int  result=0;
	int  jcnt=0;
	BYTE MK_AC[33]= {0};
	BYTE TX_DATA[256]= {0};
	BYTE temp_txt[100]= {0};
	BYTE ARPC_RAW[20]= {0};
	INT  resp_len=0;
	INT  i= 0;
	BYTE temp_msg_len[10]= {0};

	memset (&msgHeader,0,sizeof(msgHeader));
	memset (&delimiter,0,sizeof(delimiter));
	memset (&output,0,sizeof(output));
	memset (&returnString,0,ReturnString_Size);
	strcpy (delimiter,";");
	strcpy (msgHeader ,"1234");

	strcat(output,msgHeader);
	strcat(output,"JS"); /* Changed Command code JS- TF Phani*/
	/*Mode flag
	Mode of operation:
	'0' : Perform ARQC Verification only
	'1' : Perform ARQC Verification and ARPC Generation
	'2' : Perform ARPC Generation only.
	*/
	strcat(output,"1");
	/*Scheme ID*/
	strcat(output,"1");


	/* Issuer MAster key - MK-AC */
	strcpy(MK_AC, auth_tx.CAF01_details.cvk[5].cvkA);
	strcat(MK_AC, auth_tx.CAF01_details.cvk[5].cvkB);

	if(strlen(MK_AC) == 0)
	{
		racal_log_message( 2, 2,"Master key is blank.","racal_Verify_ARQC_for_CUPEMV_Keyblock",1 );
		return(TRANSACTION_NOT_VERIFIED);
	}else
	{	strcat(output,"U");
		strcat(output,MK_AC);
	}
	out_len = strlen(output);
	/* PAN SEQ NUMBER*/
	if(strlen(auth_tx.TLF01_details.card_num) >= 14 )
	{
	memset(&dummy,0,Dummy_Size);

	strncpy(dummy,&auth_tx.TLF01_details.card_num[2] ,14);
	/* need to validate PAN SEQ no  */
		if(strlen(auth_tx.host2_message_seq_nbr) > 0)
		{
				if(strlen(auth_tx.host2_message_seq_nbr) == 4)
				strncat(dummy,&auth_tx.host2_message_seq_nbr[2],2);
				else if(strlen(auth_tx.host2_message_seq_nbr) == 3)
				strncat(dummy,&auth_tx.host2_message_seq_nbr[1],2);
		}
		else if(strlen(auth_tx.EMV_details.pan_sequence_number) > 0)
		{
			if(strlen(auth_tx.EMV_details.pan_sequence_number) == 2)
			strncat(dummy,&auth_tx.EMV_details.pan_sequence_number[0],2);
			else if(strlen(auth_tx.EMV_details.pan_sequence_number) == 3)
			strncat(dummy,&auth_tx.EMV_details.pan_sequence_number[1],2);
		}
		else
			strcat(dummy,"00");
	dummy[16] = '\0';
	genutil_asc_to_bin( dummy,temp_txt,&bin_len);
	memcpy(&output[out_len],temp_txt,8);
	out_len += 8;

	}
	else
	{
		racal_log_message( 2, 2,"ARQC validation: card_num is < 14.","racal_Verify_ARQC_for_CUPEMV_Keyblock",1 );
		return (TRANSACTION_NOT_VERIFIED);
	}


	// ONLY For MC , for VISA and AMEX  0000

	if(strlen(auth_tx.EMV_details.atc) == 0)
	{
		racal_log_message( 2, 2,"ATC is blank.","racal_Verify_ARQC_for_CUPEMV_Keyblock",1 );
		return(TRANSACTION_NOT_VERIFIED);

	}else
	{
	genutil_asc_to_bin( auth_tx.EMV_details.atc,temp_txt,&bin_len);
	printf("ATC - %s \n",auth_tx.EMV_details.atc);
	memcpy(&output[out_len],temp_txt,2);
	out_len += 2;
	}


	/* Padding Flag : 0 - no padding,  1- padding*/
	memcpy(&output[out_len],"1",1);
	out_len ++;
	/* Tx data Len*/
	/* Tx Data gather for CVN
	#1
	Visa CVN 10

	Tags '9F02', '9F03', '9F1A', '95', '5F2A', '9A', '9C', '9F37', '82' ,'9F36', '9F34',[�9F10� (CVR part)]

	Visa CVN 17

	Tags �9F02�, �9F37�,�9F36�, '9F34',[�9F10� (CVR part)]

	MC CVN 16

	Tags '9F02', '9F03', '9F1A', '95', '5F2A', '9A', '9C', '9F37', '82' ,'9F36', �9F10� (CVR part)
	*/
	int emv_org_len =0;
	BYTE tag_data[255]= {0};BYTE temp_ascii_len[10]={0};
	if(0 == strcmp(auth_tx.TLF01_details.handler_queue,"dcpisoA"))
	{
	emv_org_len = genutil_bcd_to_int( auth_tx.EMV_details.emv_block, 2 );
	}
	else
	{
	strncpy(temp_ascii_len, auth_tx.EMV_details.emv_block,3);
	emv_org_len = atoi(temp_ascii_len);

	}
	printf("EMV LEN - %d \n",emv_org_len);
	memset(tag_data,0x00,sizeof(tag_data));

	printf("calling get_EMV_CVN_16_data - \n");
	bin_len=get_EMV_CVN_10_16_data(emv_org_len,&auth_tx.EMV_details.emv_block,&tag_data);
	bin_len = 40;

	int pad_len = bin_len%8;
	if (pad_len > 0)
	bin_len += (8-pad_len); // Pad with Zero's

	printf("EMV TX DATA LEN - %d \n",bin_len);
	printf("EMV TX DATA  - %s \n",tag_data);
	sprintf(&output[out_len],"%2x",bin_len);
	out_len +=2;
	memcpy(&output[out_len],tag_data,bin_len);
	out_len += bin_len;
	/* Tx data*/ // TAGs

	/* Deliemter*/

	memcpy(&output[out_len],delimiter,1);
	out_len += 1;
	/* ARQC/ARPC*/

	genutil_asc_to_bin( auth_tx.EMV_details.app_crypto,temp_txt,&bin_len);

	printf("APP CRYPTO - %s \n",auth_tx.EMV_details.app_crypto);
	memcpy(&output[out_len],temp_txt,8);
	out_len += 8;
	/* ARC*/
	genutil_asc_to_bin( "3030",temp_txt,&bin_len);
	memcpy(&output[out_len],temp_txt,2);
	out_len += 2;

	/* End of msg*/
	//if the output is bigger then 500 do not send it out.
	if (out_len > 500)
	{
		racal_log_message( 2, 2,"ARQC Cmd Msg to long.","racal_Verify_ARQC_for_CUPEMV_Keyblock",1 );
		return(TRANSACTION_NOT_VERIFIED);
	}
	//send the message to the racal box.
	if (racalSendToPort_Bin(output,out_len) == TRANSACTION_VERIFIED)
	{
	/*parse the message check for the message header.*/
	memset(&dummy,0,Dummy_Size);
	strncpy(dummy,returnString,4);
	if(strcmp(dummy,msgHeader) != 0)
	{
		racal_log_message( 2, 2,"ARQC Cmd header mis-match.","racal_Verify_ARQC_for_CUPEMV_Keyblock",1 );
		return(TRANSACTION_NOT_VERIFIED);
	}
	/*parse the message check for the response code.*/
	memset(&dummy,0,Dummy_Size);
	strncpy(dummy,returnString + 4, 2);

	if(strcmp(dummy,"JT") !=0)
	{
		racal_log_message( 2, 2,"ARQC Cmd Code mis-match.","racal_Verify_ARQC_for_CUPEMV_Keyblock",1 );
		return(TRANSACTION_NOT_VERIFIED);
	}

	/*parse the message check for the error code.*/
	memset(&dummy,0,Dummy_Size);
	strncpy(dummy,returnString + 6, 2);

	//if this value is not 00 reject the transaction.
	/* 00 : No error
	01 : ARQC/TC/AAC verification failed
	03 : Invalid Padding Flag
	04 : Mode Flag not 0, 1 or 2
	05 : Unrecognized Scheme ID
	10 : MK parity error
	67 : Command not licensed
	80 : Data length error
	81 : Zero length Transaction Data
	82 : Transaction Data length not multiple of 8 bytes
	*/
	if(strcmp(dummy,"00") != 0)
	{
	if(strcmp(dummy,"01") == 0)
	{
		racal_log_message( 2, 2,"ARPC AAC validation failed.","racal_Verify_ARQC_for_CUPEMV_Keyblock",1 );
	}
	if(strcmp(dummy,"04") == 0)
		racal_log_message( 2, 2,"Mode flag not 0 1 or 2.","racal_Verify_ARQC_for_CUPEMV_Keyblock",1 );
	if(strcmp(dummy,"05") == 0)
		racal_log_message( 2, 2,"Unrecognised Scheme ID.","racal_Verify_ARQC_for_CUPEMV_Keyblock",1 );
	if(strcmp(dummy,"10") == 0)
		racal_log_message( 2, 2,"MK parity error.","racal_Verify_ARQC_for_CUPEMV_Keyblock",1 );
	if(strcmp(dummy,"67") == 0)
		racal_log_message( 2, 2,"Command Not licensed.","racal_Verify_ARQC_for_CUPEMV_Keyblock",1 );
	if(strcmp(dummy,"80") == 0)
		racal_log_message( 2, 2,"Data len error.","racal_Verify_ARQC_for_CUPEMV_Keyblock",1 );
	if(strcmp(dummy,"81") == 0)
		racal_log_message( 2, 2,"Zero length Transaction Data.","racal_Verify_ARQC_for_CUPEMV_Keyblock",1 );
	if(strcmp(dummy,"82") == 0)
		racal_log_message( 2, 2,"Transaction Data length not multiple of 8 bytes.","racal_Verify_ARQC_for_CUPEMV_Keyblock",1 );
	return(TRANSACTION_NOT_VERIFIED);
	}

	CHAR temp_response_tag[35]={0};
	memcpy(dummy,returnString + 8, 8);		 // This is the ARPC value.
	if (strcmp(auth_tx.BIN01_details.card_family,CARD_MASTERCARD) == 0)
	{
	dummy[8]=0x00;
	dummy[9]=0x10;
	}
	else  if (strcmp(auth_tx.BIN01_details.card_family,CARD_VISA) == 0 ||
	strcmp(auth_tx.BIN01_details.card_family,CARD_AMEX) == 0 ||
	strcmp(auth_tx.BIN01_details.card_family,CARD_CUP) == 0)
	{
	dummy[8]=0x30;
	dummy[9]=0x30;
	}
	// Prepare EMV block for response
	strcpy(ARPC_RAW,"91");
	strcat(ARPC_RAW,"0A");

	if(0 == strcmp(auth_tx.TLF01_details.handler_queue,"dcpisoA") )
	{
	auth_tx.EMV_details.future_field1[0]=0x00;
	//int resp_dcp_len =12;
	auth_tx.EMV_details.future_field1[1]=0x12;
	strcpy(temp_response_tag,ARPC_RAW);
	genutil_asc_to_bin( temp_response_tag,temp_txt,&bin_len);
	memcpy(&auth_tx.EMV_details.future_field1[2],temp_txt,2);
	memcpy(&auth_tx.EMV_details.future_field1[4],dummy,10);
	}
	else
	{
	if (strcmp(auth_tx.BIN01_details.card_family,CARD_MASTERCARD) == 0 ||
	 strcmp(auth_tx.BIN01_details.card_family,CARD_CUP) == 0 )
	{	sprintf(auth_tx.EMV_details.future_field1,"%03d",12);
	strcpy(temp_response_tag,ARPC_RAW);
	genutil_asc_to_bin( temp_response_tag,temp_txt,&bin_len);
	memcpy(&auth_tx.EMV_details.future_field1[3],temp_txt,2);
	memcpy(&auth_tx.EMV_details.future_field1[5],dummy,10);
	}
	else  if (strcmp(auth_tx.BIN01_details.card_family,CARD_VISA) == 0)
	{
	auth_tx.EMV_details.future_field1[0]=0x0F;
	strcpy(temp_response_tag,"01000C");
	strcat(temp_response_tag,ARPC_RAW);
	genutil_asc_to_bin( temp_response_tag,temp_txt,&bin_len);
	memcpy(&auth_tx.EMV_details.future_field1[1],temp_txt,5);
	memcpy(&auth_tx.EMV_details.future_field1[6],dummy,10);
	}else if (strcmp(auth_tx.BIN01_details.card_family,CARD_AMEX) == 0)
	{
	auth_tx.EMV_details.future_field1[0]=0x0A;
	memcpy(&auth_tx.EMV_details.future_field1[1],dummy,10);

	}

	}

	}

	return (TRANSACTION_VERIFIED);
}/*racal_Verify_ARQC_for_CUPEMV_Keyblock*/
/*********************************************/

int racal_Verify_ARQC_for_JCB_Keyblock()
{
	char msgHeader[5]={0};
	char delimiter[2]={0};
	char output[500]={0};
	INT  bin_len=0;
	INT  out_len=0;
	int	cnt=0;
	int  result=0;
	int  jcnt=0;
	BYTE MK_AC[33]={0};
	BYTE TX_DATA[256]={0};
	BYTE temp_txt[100]={0};
	BYTE ARPC_RAW[20]={0};
	INT resp_len=0;
	INT i=0;
	BYTE temp_msg_len[10]={0};

	memset (&msgHeader,0,sizeof(msgHeader));
	memset (&delimiter,0,sizeof(delimiter));
	memset (&output,0,sizeof(output));
	memset (&returnString,0,ReturnString_Size);
	strcpy (delimiter,";");
	strcpy (msgHeader ,"1234");

	strcat(output,msgHeader);
	strcat(output,"US"); /* Changed Command code KQ- TF Phani*/
	/*Mode flag
	Mode of operation:
	'0' : Perform ARQC Verification only
	'1' : Perform ARQC Verification and ARPC Generation
	'2' : Perform ARPC Generation only.
	*/
	strcat(output,"1");

	/* CVN of the JCB:
	*/
	strcat(output,"01");

	/* Issuer MAster key - MK-AC */
	strcpy(MK_AC, auth_tx.CAF01_details.cvk[5].cvkA);
	strcat(MK_AC, auth_tx.CAF01_details.cvk[5].cvkB);

	if(strlen(MK_AC) == 0)
	{
		racal_log_message( 2, 2,"Master key is blank.","racal_Verify_ARQC_for_JCB_Keyblock",1 );
		return(TRANSACTION_NOT_VERIFIED);
	}
	else
	{
		strcat(output,"U");
		strcat(output,MK_AC);
	}

	out_len = strlen(output);
	/* PAN SEQ NUMBER*/
	if(strlen(auth_tx.TLF01_details.card_num) >= 14 )
	{
		memset(&dummy,0,Dummy_Size);

		strncpy(dummy,&auth_tx.TLF01_details.card_num[2] ,14);
		/* need to validate PAN SEQ no  */
		if(strlen(auth_tx.host2_message_seq_nbr) > 0)
		{
			if(strlen(auth_tx.host2_message_seq_nbr) == 4)
			{
				strncat(dummy,&auth_tx.host2_message_seq_nbr[2],2);
			}
			else if(strlen(auth_tx.host2_message_seq_nbr) == 3)
			{
				strncat(dummy,&auth_tx.host2_message_seq_nbr[1],2);
			}
		}
		else if(strlen(auth_tx.EMV_details.pan_sequence_number) > 0)
		{
			if(strlen(auth_tx.EMV_details.pan_sequence_number) == 2)
			{
				strncat(dummy,&auth_tx.EMV_details.pan_sequence_number[0],2);
			}
			else if(strlen(auth_tx.EMV_details.pan_sequence_number) == 3)
			{
				strncat(dummy,&auth_tx.EMV_details.pan_sequence_number[1],2);
			}
		}
		else
		{
			strcat(dummy,"00");
		}

		dummy[16] = '\0';
		genutil_asc_to_bin( dummy,temp_txt,&bin_len);
		//strncat(output,temp_txt,bin_len);
		memcpy(&output[out_len],temp_txt,8);
		out_len += 8;
		//strcat(output,dummy);
	}

	else
	{
		racal_log_message( 2, 2,"ARQC validation: card_num is < 14.","racal_Verify_ARQC_for_JCB_Keyblock",1 );
		return (TRANSACTION_NOT_VERIFIED);
	}

	int emv_org_len =0;
	BYTE tag_data[255]= {0};

	emv_org_len = genutil_bcd_to_int( auth_tx.EMV_details.emv_block, 2 );

	printf("EMV LEN - %d \n",emv_org_len);
	memset(tag_data,0x00,sizeof(tag_data));

	printf("calling get_EMV_CVN_16_data - \n");
	bin_len=get_EMV_CVN_10_16_data(emv_org_len,&auth_tx.EMV_details.emv_block,&tag_data);
	bin_len = 40;


	printf("EMV TX DATA LEN - %d \n",bin_len);
	printf("EMV TX DATA  - %s \n",tag_data);
	sprintf(&output[out_len],"%2x",bin_len);
	out_len +=2;
	memcpy(&output[out_len],tag_data,bin_len);
	out_len += bin_len;
	/* Tx data*/ // TAGs

	/* Deliemter*/

	memcpy(&output[out_len],delimiter,1);
	out_len += 1;
	/* ARQC/ARPC*/

	genutil_asc_to_bin( auth_tx.EMV_details.app_crypto,temp_txt,&bin_len);
	memcpy(&output[out_len],temp_txt,8);
	out_len += 8;

	/* ARC*/
	genutil_asc_to_bin( "3030",temp_txt,&bin_len);
	memcpy(&output[out_len],temp_txt,2);
	out_len += 2;

	/* End of msg*/

	if (out_len > 500)
	{
		racal_log_message( 2, 2,"ARQC Cmd Msg to long.","racal_Verify_ARQC_for_JCB_Keyblock",1 );
		return(TRANSACTION_NOT_VERIFIED);
	}

	if (racalSendToPort_Bin(output,out_len) == TRANSACTION_VERIFIED)
	{
		/*parse the message check for the message header.*/
		memset(&dummy,0,Dummy_Size);
		strncpy(dummy,returnString,4);
		if(strcmp(dummy,msgHeader) != 0)
		{
			racal_log_message( 2, 2,"ARQC Cmd header mis-match.","racal_Verify_ARQC_for_JCB_Keyblock",1 );
			return(TRANSACTION_NOT_VERIFIED);
		}
		/*parse the message check for the response code.*/
		memset(&dummy,0,Dummy_Size);
		strncpy(dummy,returnString + 4, 2);

		if(strcmp(dummy,"UT") !=0)
		{
			racal_log_message( 2, 2,"ARQC Cmd Code mis-match.","racal_Verify_ARQC_for_JCB_Keyblock",1 );
			return(TRANSACTION_NOT_VERIFIED);
		}

		/*parse the message check for the error code.*/
		memset(&dummy,0,Dummy_Size);
		strncpy(dummy,returnString + 6, 2);

		if(strcmp(dummy,"00") != 0)
		{
			if(strcmp(dummy,"01") == 0)
			{
				racal_log_message( 2, 2,"ARPC AAC validation failed.","racal_Verify_ARQC_for_JCB_Keyblock",1 );
			}
			if(strcmp(dummy,"04") == 0)
			{
				racal_log_message( 2, 2,"Mode flag not 0 1 or 2.","racal_Verify_ARQC_for_JCB_Keyblock",1 );
			}
			else if(strcmp(dummy,"05") == 0)
			{
				racal_log_message( 2, 2,"Unrecognised Scheme ID.","racal_Verify_ARQC_for_JCB_Keyblock",1 );
			}
			else if(strcmp(dummy,"10") == 0)
			{
				racal_log_message( 2, 2,"MK parity error.","racal_Verify_ARQC_for_JCB_Keyblock",1 );
			}
			else if(strcmp(dummy,"53") == 0)
			{
				racal_log_message( 2, 2,"Invalid CVN.","racal_Verify_ARQC_for_JCB_Keyblock",1 );
			}
			else if(strcmp(dummy,"80") == 0)
			{
				racal_log_message( 2, 2,"Data Length error.","racal_Verify_ARQC_for_JCB_Keyblock",1 );
			}
			else if(strcmp(dummy,"81") == 0)
			{
				racal_log_message( 2, 2,"Zero len tx data.","racal_Verify_ARQC_for_JCB_Keyblock",1 );
			}
			else if(strcmp(dummy,"68") == 0)
			{
				racal_log_message( 2, 2,"Command disabled.","racal_Verify_ARQC_for_JCB_Keyblock",1 );
			}
			return(TRANSACTION_NOT_VERIFIED);
		}

		CHAR temp_response_tag[35]={0};
		memcpy(dummy,returnString + 8, 8);		 // This is the ARPC value.
		dummy[8]=0x30;
		dummy[9]=0x30;

		// Prepare EMV block for response
		strcpy(ARPC_RAW,"91");
		strcat(ARPC_RAW,"0A");

		if(0 == strcmp(auth_tx.TLF01_details.handler_queue,"dcpisoA") )
		{
			auth_tx.EMV_details.future_field1[0]=0x00;
			//int resp_dcp_len =12;
			auth_tx.EMV_details.future_field1[1]=0x12;
			strcpy(temp_response_tag,ARPC_RAW);
			genutil_asc_to_bin( temp_response_tag,temp_txt,&bin_len);
			memcpy(&auth_tx.EMV_details.future_field1[2],temp_txt,2);
			memcpy(&auth_tx.EMV_details.future_field1[4],dummy,10);
		}
		else
		{
			auth_tx.EMV_details.future_field1[0]=0x00;
			auth_tx.EMV_details.future_field1[1]=0x0C;
			strcat(temp_response_tag,ARPC_RAW);
			genutil_asc_to_bin( temp_response_tag,temp_txt,&bin_len);
			memcpy(&auth_tx.EMV_details.future_field1[2],temp_txt,2);
			memcpy(&auth_tx.EMV_details.future_field1[4],dummy,10);
		}
	}

	return (TRANSACTION_VERIFIED);

}/*racal_Verify_ARQC_for_JCB_Keyblock*/
/*******************************************************/

int racal_Verify_ARQC_For_Diners_Keyblock()
{
	char msgHeader[5]= {0};
	char delimiter[2]= {0};
	char output[513]= {0};
	INT  bin_len=0;
	INT  out_len=0;

	BYTE MK_AC[129]= {0};
	BYTE TX_DATA[256]= {0};
	BYTE temp_txt[100]= {0};
	BYTE ARPC_RAW[20]= {0};
	INT  resp_len=0;
	INT  i =0;
	BYTE temp_msg_len[10]= {0};
	INT  cardLen =0;
	INT  emv_org_len =0;
	BYTE tag_data[255]= {0};
	CHAR err_buf[100] = {0};
	CHAR emv_len[5] = {0};
	CHAR temp_response_tag[35]={0};

	memset (&returnString,0,ReturnString_Size);
	strcpy (delimiter,";");
	strcpy (msgHeader ,"1234");

	strcat(output,msgHeader);
	strcat(output,"KW"); /* Changed Command code KW- TF Phani*/

	strcat(output,"2");
	/* Issuer MAster key - MK-AC */
	strcpy(MK_AC, caf01_keyblock.Cvk_Mkac_Keyblock);

	if(strlen(MK_AC) == 0)
	{
		racal_log_message( 2, 2,"Master key is blank.","racal_Verify_ARQC_For_Diners_Keyblock",1 );
		return(TRANSACTION_NOT_VERIFIED);
	}
	else
	{
		strncat(output,SOUURCE_TPK_KEYBLOCK,KEYBLOCK_KEY_FORMAT_LEN);
		strcat (output,MK_AC);
	}

	out_len = strlen(output);
	/* PAN SEQ NUMBER*/
	if(strlen(auth_tx.TLF01_details.card_num) >= 14 )
	{
		memset(&dummy,0,Dummy_Size);

		if(strlen(auth_tx.TLF01_details.card_num) == 16)
		{
			strncpy(dummy,&auth_tx.TLF01_details.card_num[2] ,14);
		}
		else
		{
			cardLen= strlen(auth_tx.TLF01_details.card_num);
			strncpy(dummy,&auth_tx.TLF01_details.card_num[cardLen-14] ,14);
		}

		if(strlen(auth_tx.EMV_details.pan_sequence_number) > 0)
		{
			if(strlen(auth_tx.EMV_details.pan_sequence_number) == 2)
			{
				strncat(dummy,&auth_tx.EMV_details.pan_sequence_number[0],2);
			}
			else if(strlen(auth_tx.EMV_details.pan_sequence_number) == 3)
			{
				strncat(dummy,&auth_tx.EMV_details.pan_sequence_number[1],2);
			}
		}
		else if(strlen(auth_tx.host2_message_seq_nbr) > 0)
		{
			if(strlen(auth_tx.host2_message_seq_nbr) == 4)
			{
				strncat(dummy,&auth_tx.host2_message_seq_nbr[2],2);
			}
			else if(strlen(auth_tx.host2_message_seq_nbr) == 3)
			{
				strncat(dummy,&auth_tx.host2_message_seq_nbr[1],2);
			}
		}
		else
		{
			strcat(dummy,"00");
		}
		dummy[16] = '\0';
		genutil_asc_to_bin( dummy,temp_txt,&bin_len);
		memcpy(&output[out_len],temp_txt,8);
		out_len += 8;
	}
	else
	{
		racal_log_message( 2, 2,"ARQC validation: card_num is < 14.","racal_Verify_ARQC_For_Diners_Keyblock",1 );
		return (TRANSACTION_NOT_VERIFIED);
	}

	if(strlen(auth_tx.EMV_details.atc) == 0)
	{
		racal_log_message( 2, 2,"ATC is blank.","racal_Verify_ARQC_For_Diners_Keyblock",1 );
		return(TRANSACTION_NOT_VERIFIED);
	}
	else
	{
		genutil_asc_to_bin( auth_tx.EMV_details.atc,temp_txt,&bin_len);
		memcpy(&output[out_len],temp_txt,2);
		out_len += 2;
	}

	if(0 == strcmp(auth_tx.TLF01_details.handler_queue,"dcpisoA"))
	{
		emv_org_len = genutil_bcd_to_int( auth_tx.EMV_details.emv_block, 2 );
	}
	else
	{
		strncpy(temp_msg_len,auth_tx.EMV_details.emv_block,3);
		emv_org_len = atoi(temp_msg_len);
	}

	memset(tag_data,0x00,sizeof(tag_data));
	if(strncmp(auth_tx.TLF01_details.pos_entry_mode ,"005",3) == 0)
	{
		/* contact */
		bin_len=get_EMV_CVN_05_data_Diners(emv_org_len,&auth_tx.EMV_details.emv_block,&tag_data);
	}
	else
	{	/* contactless */
		bin_len=get_EMV_CVN_15_data_Diners(emv_org_len,&auth_tx.EMV_details.emv_block,&tag_data);
	}

	int pad_len = bin_len%8;
	if (pad_len > 0)
	{
		bin_len += (8-pad_len); /* Pad with Zero's */
	}

	sprintf(&output[out_len],"%2x",bin_len);
	out_len +=2;
	memcpy(&output[out_len],tag_data,bin_len);
	out_len += bin_len;

	memcpy(&output[out_len],delimiter,1);
	out_len += 1;

	/* ARQC/ARPC*/
	genutil_asc_to_bin( auth_tx.EMV_details.app_crypto,temp_txt,&bin_len);
	memcpy(&output[out_len],temp_txt,8);
	out_len += 8;

	if(strncmp(auth_tx.TLF01_details.pos_entry_mode ,"005",3) == 0)
	{
		output[out_len] = 0x00;
		out_len += 1;
		output[out_len] = 0x32;
		out_len += 1;
		output[out_len] = 0x00;
		out_len += 1;
		output[out_len] = 0x00;
		out_len += 1;
	}
	else
	{
		out_len += 4;
	}

	/* End of msg*/
	if (out_len > MAX_KEYBLOCK_CMD_LEN)
	{
		racal_log_message( 2, 2,"ARQC Cmd Msg to long.","racal_Verify_ARQC_For_Diners_Keyblock",1 );
		return(TRANSACTION_NOT_VERIFIED);
	}

	if (racalSendToPort_Bin(output,out_len) == TRANSACTION_VERIFIED)
	{
		/*parse the message check for the message header.*/
		memset(&dummy,0,Dummy_Size);
		strncpy(dummy,returnString,4);
		if(strcmp(dummy,msgHeader) != 0)
		{
			racal_log_message( 2, 2,"ARQC Cmd header mis-match.","racal_Verify_ARQC_For_Diners_Keyblock",1 );
			return(TRANSACTION_NOT_VERIFIED);
		}

		memset(&dummy,0,Dummy_Size);
		strncpy(dummy,returnString + 4, 2);

		if(strcmp(dummy,"KX") !=0)
		{
			racal_log_message( 2, 2,"ARQC Cmd Code mis-match.","racal_Verify_ARQC_For_Diners_Keyblock",1 );
			return(TRANSACTION_NOT_VERIFIED);
		}

		memset(&dummy,0,Dummy_Size);
		strncpy(dummy,returnString + 6, 2);

		if(strcmp(dummy,"00") != 0)
		{
			if(strcmp(dummy,"01") == 0)
			{
				racal_log_message( 2, 2,"ARPC AAC validation failed.","racal_Verify_ARQC_For_Diners_Keyblock",1 );
			}
			else if(strcmp(dummy,"04") == 0)
			{
				racal_log_message( 2, 2,"Mode flag not 0 1 or 2.","racal_Verify_ARQC_For_Diners_Keyblock",1 );
			}
			else if(strcmp(dummy,"05") == 0)
			{
				racal_log_message( 2, 2,"Unrecognised Scheme ID.","racal_Verify_ARQC_For_Diners_Keyblock",1 );
			}
			else if(strcmp(dummy,"10") == 0)
			{
				racal_log_message( 2, 2,"MK parity error.","racal_Verify_ARQC_For_Diners_Keyblock",1 );
			}
			else if(strcmp(dummy,"68") == 0)
			{
				racal_log_message( 2, 2,"Command disabled.","racal_Verify_ARQC_For_Diners_Keyblock",1 );
			}
			return(TRANSACTION_NOT_VERIFIED);
		}

		memcpy(dummy,returnString + 8, 8);		 /* This is the ARPC value */

		if(strncmp(auth_tx.TLF01_details.pos_entry_mode ,"005",3) == 0)
		{
			dummy[8]=0x00;
			dummy[9]=0x32;
		}
		else
		{
			dummy[8]=0x00;
			dummy[9]=0x00;
		}

		strcpy(ARPC_RAW,"91");
		strcat(ARPC_RAW,"0A");
		memset( auth_tx.EMV_details.future_field1,0x00,
		sizeof(auth_tx.EMV_details.future_field1));
		if(0 == strcmp(auth_tx.TLF01_details.handler_queue,"dcpisoA") )
		{
			auth_tx.EMV_details.future_field1[0]=0x00;
			auth_tx.EMV_details.future_field1[1]=0x12;
			strncpy(temp_response_tag,ARPC_RAW,4);
			genutil_asc_to_bin( temp_response_tag,temp_txt,&bin_len);
			memcpy(&auth_tx.EMV_details.future_field1[2],temp_txt,2);
			memcpy(&auth_tx.EMV_details.future_field1[4],dummy,10);
		}
		else
		{
			strncpy(auth_tx.EMV_details.future_field1,"012",3);
			strncpy(temp_response_tag,ARPC_RAW,4);
			genutil_asc_to_bin( temp_response_tag,temp_txt,&bin_len);
			memcpy(&auth_tx.EMV_details.future_field1[3],temp_txt,2);
			memcpy(&auth_tx.EMV_details.future_field1[5],dummy,10);
		}
	}
	return (TRANSACTION_VERIFIED);
}  /*racal_Verify_ARQC_For_Diners_Keyblock*/

/***********************************************************/

int racal_Verify_ARQC_For_VisaCVN18_Keyblock()
{

	   char msgHeader[5]= {0};
	   char delimiter[2]= {0};
	   char output[500]= {0};
	   INT  bin_len=0;
	   INT  out_len=0;
	   INT  pad_len = 0;
	   int	cnt=0;
	   int  result=0;
	   int  jcnt=0;
	   BYTE MK_AC[33]= {0};
	   BYTE TX_DATA[256]= {0};
	   BYTE temp_txt[100]= {0};
	   BYTE ARPC_RAW[20]= {0};
	   INT  resp_len=0;
	   INT  i = 0;
	   BYTE temp_msg_len[10]= {0};
	   INT  cardLen =0;
	   INT  emv_org_len =0;
	   BYTE tag_data[255]= {0};
	   CHAR err_buf[100] = {0};
	   CHAR emv_len[5] = {0};

	   memset (&msgHeader,0,sizeof(msgHeader));
	   memset (&delimiter,0,sizeof(delimiter));
	   memset (&output,0,sizeof(output));
	   memset (&returnString,0,ReturnString_Size);
	   strcpy (delimiter,";");
	   strcpy (msgHeader ,"1234");

	   strcat(output,msgHeader);
	   strcat(output,"KW"); /* Changed Command code KW- TF Phani*/
			/*Mode flag
				Mode of operation:
				'0' : Perform ARQC verification only
				'1' : Perform ARQC verification and EMV 4.x method 1 ARPC generation
				'2' : Perform EMV 4.x method 1 ARPC generation only
				'3' : Perform ARQC verification and EMV 4.x Method 2 ARPC Generation
				'4' : Perform EMV 4.x Method 2 ARPC generation only
				�5� : Perform ARQC verification and D-PAS Method 1 ARPC generation
				�6� : Perform D-PAS Method 1 ARPC generation only.
			*/
			strcat(output,"3");
			/* Identifier of the EMV scheme:
			 	 Specifies the Key Derivation Method to use
				'0' : VIS 1.4.0 and M/Chip 4 using Card Key Derivation Method A and EMV2000 Session Key Method
				'1' : VIS 1.4.0 and M/Chip 4 using Card Key Derivation Method B and EMV2000 Session Key Method
				'2' : VIS 1.4.0 and M/Chip 4 and D-PAS using Card Key Derivation Method A and EMV Common Session Key Derivation Method
				'3' : VIS 1.4.0 and M/Chip 4 using Card Key Derivation Method B and EMV Common Session Key Derivation Method
			 */
			strcat(output,"2");
			 /* Issuer MAster key - MK-AC */
			 strcpy(MK_AC, auth_tx.CAF01_details.cvk[5].cvkA);
			 strcat(MK_AC, auth_tx.CAF01_details.cvk[5].cvkB);

			if(strlen(MK_AC) == 0)
			{
				racal_log_message( 2, 2,"Master key is blank.","racal_Verify_ARQC_For_VisaCVN18_Keyblock",1 );
				return(TRANSACTION_NOT_VERIFIED);

			}
			else
			{	strcat(output,"U");
				strcat(output,MK_AC);
			}
			 out_len = strlen(output);
			 /* PAN SEQ NUMBER*/
			 if(strlen(auth_tx.TLF01_details.card_num) >= 14 )
			 {
				  memset(&dummy,0,Dummy_Size);

				  if(strlen(auth_tx.TLF01_details.card_num) == 16)
				  strncpy(dummy,&auth_tx.TLF01_details.card_num[2] ,14);
				  else
				  {	  cardLen= strlen(auth_tx.TLF01_details.card_num);
					  strncpy(dummy,&auth_tx.TLF01_details.card_num[cardLen-14] ,14);
				  }
				  /* need to validate PAN SEQ no  */
				  if(strlen(auth_tx.host2_message_seq_nbr) > 0)
				  { if(strlen(auth_tx.host2_message_seq_nbr) == 4)
						strncat(dummy,&auth_tx.host2_message_seq_nbr[2],2);
					  else if(strlen(auth_tx.host2_message_seq_nbr) == 3)
						strncat(dummy,&auth_tx.host2_message_seq_nbr[1],2);
				  }
				  else if(strlen(auth_tx.EMV_details.pan_sequence_number) > 0)
				  {
					if(strlen(auth_tx.EMV_details.pan_sequence_number) == 2)
						strncat(dummy,&auth_tx.EMV_details.pan_sequence_number[0],2);
					else if(strlen(auth_tx.EMV_details.pan_sequence_number) == 3)
						strncat(dummy,&auth_tx.EMV_details.pan_sequence_number[1],2);
				  }
				  else
				  {
							strcat(dummy,"00");
				  }
				  dummy[16] = '\0';
				  genutil_asc_to_bin( dummy,temp_txt,&bin_len);
				  memcpy(&output[out_len],temp_txt,8);
				  out_len += 8;

			  }
			  	else
			    {
				  racal_log_message( 2, 2,"ARQC validation: card_num is < 14.","racal_Verify_ARQC_For_VisaCVN18_Keyblock",1 );
				  return (TRANSACTION_NOT_VERIFIED);
			    }
			 /* ATC*/	//	 auth_tx.EMV_details.atc


				if(strlen(auth_tx.EMV_details.atc) == 0)
				{
					racal_log_message( 2, 2,"ATC is blank.","racal_Verify_ARQC_For_VisaCVN18_Keyblock",1 );
					return(TRANSACTION_NOT_VERIFIED);

				}
				else
				{
					genutil_asc_to_bin( auth_tx.EMV_details.atc,temp_txt,&bin_len);
					printf("ATC - %s \n",auth_tx.EMV_details.atc);
					//strncat(output,temp_txt,bin_len);
					 memcpy(&output[out_len],temp_txt,2);
					 out_len += 2;
				}

				if(0 == strcmp(auth_tx.TLF01_details.handler_queue,"dcpisoA"))
				{
					emv_org_len = genutil_bcd_to_int( auth_tx.EMV_details.emv_block, 2 );
				}
				else
				{
					//strncpy(temp_msg_len,auth_tx.EMV_details.emv_block,3);
					//emv_org_len = atoi(temp_msg_len);

					emv_org_len = auth_tx.EMV_details.emv_block[0];

				}
				memset(tag_data,0x00,sizeof(tag_data));
				bin_len=get_EMV_CVN_18_data(emv_org_len,&auth_tx.EMV_details.emv_block,&tag_data);
				bin_len = BIN_LEN_CVN18;

				pad_len = bin_len%8;
				if (pad_len > 0)
					bin_len += (7 - pad_len); // Pad with Zero's

				printf("EMV TX DATA LEN - %d \n",bin_len);
				printf("EMV TX DATA  - %s \n",tag_data);
				sprintf(&output[out_len],"%2X",bin_len);
					out_len +=2;
				memcpy(&output[out_len],tag_data,bin_len);
				out_len += bin_len;
			 /* Tx data*/ // TAGs

			 /* Deliemter*/

				memcpy(&output[out_len],delimiter,1);
				out_len += 1;
			 /* ARQC/ARPC*/

				genutil_asc_to_bin( auth_tx.EMV_details.app_crypto,temp_txt,&bin_len);
				memcpy(&output[out_len],temp_txt,8);
				out_len += 8;
			 /* CSU*/

				output[out_len] = 0x00;out_len += 1;
				output[out_len] = 0x00;out_len += 1;
				output[out_len] = 0x00;out_len += 1;
				output[out_len] = 0x00;out_len += 1;

				/*Proprietary Authentication Data Length */
				pad_len = 7;
				sprintf(&output[out_len],"%1x",pad_len);
				out_len +=1;

				/*Proprietary Authentication Data*/
				memset(temp_txt,0x00, sizeof(temp_txt));
				genutil_asc_to_bin( auth_tx.EMV_details.iad ,temp_txt,&pad_len);
				memcpy(&output[out_len],temp_txt,7);
				out_len += 7;


			 /* End of msg*/
			 //if the output is bigger then 500 do not send it out.
			 if (out_len > 500)
			 {
				racal_log_message( 2, 2,"ARQC Cmd Msg to long.","racal_Verify_ARQC_For_VisaCVN18_Keyblock",1 );
				return(TRANSACTION_NOT_VERIFIED);
			 }
			 //send the message to the racal box.
			 if (racalSendToPort_Bin(output,out_len) == TRANSACTION_VERIFIED)
			 {
				/*parse the message check for the message header.*/
				memset(&dummy,0,Dummy_Size);
				strncpy(dummy,returnString,4);
				if(strcmp(dummy,msgHeader) != 0)
				{
				   racal_log_message( 2, 2,"ARQC Cmd header mis-match.","racal_Verify_ARQC_For_VisaCVN18_Keyblock",1 );
				   return(TRANSACTION_NOT_VERIFIED);
				}
				/*parse the message check for the response code.*/
				memset(&dummy,0,Dummy_Size);
				strncpy(dummy,returnString + 4, 2);

				if(strcmp(dummy,"KX") !=0)
				{
				   racal_log_message( 2, 2,"ARQC Cmd Code mis-match.","racal_Verify_ARQC_For_VisaCVN18_Keyblock",1 );
				   return(TRANSACTION_NOT_VERIFIED);
				}
				/*parse the message check for the error code.*/
				memset(&dummy,0,Dummy_Size);
				strncpy(dummy,returnString + 6, 2);

				  //if this value is not 00 reject the transaction.
				/* '00' : No error
				'01' : ARQC/TC/AAC verification failed
				'04' : Mode Flag not 0, 1 or 2
				'05' : Unrecognised Scheme ID
				'10' : MK parity error
				'68' : Command disabled
				*/
				if(strcmp(dummy,"00") != 0)
				{
				   if(strcmp(dummy,"01") == 0)
					{
					   racal_log_message( 2, 2,"ARPC AAC validation failed.","racal_Verify_ARQC_For_VisaCVN18_Keyblock",1 );
				   }
				   if(strcmp(dummy,"04") == 0)
					  racal_log_message( 2, 2,"Mode flag not 0 1 or 2.","racal_Verify_ARQC_For_VisaCVN18_Keyblock",1 );
				   if(strcmp(dummy,"05") == 0)
					  racal_log_message( 2, 2,"Unrecognised Scheme ID.","racal_Verify_ARQC_For_VisaCVN18_Keyblock",1 );
				   if(strcmp(dummy,"10") == 0)
					  racal_log_message( 2, 2,"MK parity error.","racal_Verify_ARQC_For_VisaCVN18_Keyblock",1 );
				   if(strcmp(dummy,"68") == 0)
					  racal_log_message( 2, 2,"Command disabled.","racal_Verify_ARQC_For_VisaCVN18_Keyblock",1 );
					 return(TRANSACTION_NOT_VERIFIED);
				}
				CHAR temp_response_tag[35]={0};
				memcpy(dummy,returnString + 8, 8);		 // This is the ARPC value.

     			 // Prepare EMV block for response
				 strcpy(ARPC_RAW,"91");
				 strcat(ARPC_RAW,"0A");
				 memset( auth_tx.EMV_details.future_field1,0x00,
						 sizeof(auth_tx.EMV_details.future_field1));
				if(0 == strcmp(auth_tx.TLF01_details.handler_queue,"dcpisoA") )
				{
					auth_tx.EMV_details.future_field1[0]=0x00;
					//int resp_dcp_len =12;
					auth_tx.EMV_details.future_field1[1]=0x12;
					strncpy(temp_response_tag,ARPC_RAW,4);
					genutil_asc_to_bin( temp_response_tag,temp_txt,&bin_len);
					memcpy(&auth_tx.EMV_details.future_field1[2],temp_txt,2);
					memcpy(&auth_tx.EMV_details.future_field1[4],dummy,10);
				}
				else
				{
					auth_tx.EMV_details.future_field1[0]=0x0F;
					strcpy(temp_response_tag,"01000C");
					strcat(temp_response_tag,ARPC_RAW);
					genutil_asc_to_bin( temp_response_tag,temp_txt,&bin_len);
					memcpy(&auth_tx.EMV_details.future_field1[1],temp_txt,5);
					memcpy(&auth_tx.EMV_details.future_field1[6],dummy,10);
				}
		   }
	  return (TRANSACTION_VERIFIED);
} /*racal_Verify_ARQC_For_VisaCVN18_Keyblock*/

/************************************************/

int racal_Verify_ARQC_for_MC_Keyblock()
{
	   char msgHeader[5]={0};
	   char delimiter[2]={0};
	   char output[500]={0};
	   INT  bin_len=0;
	   INT  out_len=0;
	   int	cnt=0;
	   int  result=0;
	   int  jcnt=0;
	   BYTE MK_AC[33]={0};
	   BYTE TX_DATA[256]={0};
	   BYTE temp_txt[100]={0};
	   BYTE ARPC_RAW[20]={0};
	   INT  resp_len = 0;
	   INT  i= 0;
	   BYTE temp_msg_len[10]={0};

	   memset (&msgHeader,0,sizeof(msgHeader));
	   memset (&delimiter,0,sizeof(delimiter));
	   memset (&output,0,sizeof(output));
	   memset (&returnString,0,ReturnString_Size);
	   strcpy (delimiter,";");
	   strcpy (msgHeader ,"1234");

			 strcat(output,msgHeader);
			 strcat(output,"KQ"); /* Changed Command code KQ- TF Phani*/
			/*Mode flag
				Mode of operation:
			 '0' : Perform ARQC Verification only
			 '1' : Perform ARQC Verification and ARPC Generation
			 '2' : Perform ARPC Generation only.
			*/
			strcat(output,"1");
			// strcat(output,"2");
			//mode_falg = '2';
			/* Identifier of the EMV scheme:
			'0' : Visa VSDC or UKIS
			 '1' : Europay or MasterCard M/Chip
			 '2' : American Express AEIPS 4.1 */
			if (strcmp(auth_tx.BIN01_details.card_family,CARD_VISA) == 0)
				strcat(output,"0");
			else if (strcmp(auth_tx.BIN01_details.card_family,CARD_MASTERCARD) == 0)
				strcat(output,"1");
			else if (strcmp(auth_tx.BIN01_details.card_family,CARD_AMEX) == 0)
				strcat(output,"2");


			 /* Issuer MAster key - MK-AC */
			 strcpy(MK_AC, auth_tx.CAF01_details.cvk[5].cvkA);
			 strcat(MK_AC, auth_tx.CAF01_details.cvk[5].cvkB);

			 if(strlen(MK_AC) == 0)
			{
				racal_log_message( 2, 2,"Master key is blank.","racal_Verify_ARQC_for_MC_Keyblock",1 );
				return(TRANSACTION_NOT_VERIFIED);

			}else
			{	strcat(output,"U");
				strcat(output,MK_AC);
			}
			 out_len = strlen(output);
			 /* PAN SEQ NUMBER*/
			 if(strlen(auth_tx.TLF01_details.card_num) >= 14 )
			 {
				  memset(&dummy,0,Dummy_Size);

				  if (strcmp(auth_tx.BIN01_details.card_family,CARD_AMEX) == 0)
				  strncpy(dummy,&auth_tx.TLF01_details.card_num[1] ,14);
				  else
				  strncpy(dummy,&auth_tx.TLF01_details.card_num[2] ,14);
				  /* need to validate PAN SEQ no  */
				  if(strcmp(auth_tx.BIN01_details.card_family,CARD_AMEX) == 0)
				  {
				 if(strlen(auth_tx.EMV_details.pan_sequence_number) > 0)
				  {
					if(strlen(auth_tx.EMV_details.pan_sequence_number) == 2)
						strncat(dummy,&auth_tx.EMV_details.pan_sequence_number[0],2);
					else if(strlen(auth_tx.EMV_details.pan_sequence_number) == 3)
						strncat(dummy,&auth_tx.EMV_details.pan_sequence_number[1],2);
				  }
				  else if(strlen(auth_tx.host2_message_seq_nbr) > 0)
					  {
						  if(strlen(auth_tx.host2_message_seq_nbr) == 4)
							  strncat(dummy,&auth_tx.host2_message_seq_nbr[2],2);
						  else if(strlen(auth_tx.host2_message_seq_nbr) == 3)
							  strncat(dummy,&auth_tx.host2_message_seq_nbr[1],2);
					  }
					  else
					  	strcat(dummy,"00");
				}
				else
				{
					if(strlen(auth_tx.host2_message_seq_nbr) > 0)
					{
						if(strlen(auth_tx.host2_message_seq_nbr) == 4)
						strncat(dummy,&auth_tx.host2_message_seq_nbr[2],2);
					  else if(strlen(auth_tx.host2_message_seq_nbr) == 3)
						strncat(dummy,&auth_tx.host2_message_seq_nbr[1],2);
					}
				    else if(strlen(auth_tx.EMV_details.pan_sequence_number) > 0)
				    {
				    	if(strlen(auth_tx.EMV_details.pan_sequence_number) == 2)
				    		strncat(dummy,&auth_tx.EMV_details.pan_sequence_number[0],2);
				    	else if(strlen(auth_tx.EMV_details.pan_sequence_number) == 3)
				    		strncat(dummy,&auth_tx.EMV_details.pan_sequence_number[1],2);
				    }
				  else
							strcat(dummy,"00");
				}
				  dummy[16] = '\0';
				  genutil_asc_to_bin( dummy,temp_txt,&bin_len);
				  //strncat(output,temp_txt,bin_len);
				  memcpy(&output[out_len],temp_txt,8);
				  out_len += 8;
				  //strcat(output,dummy);
			  }
			  else
			  {
				  racal_log_message( 2, 2,"ARQC validation: card_num is < 14.","racal_Verify_ARQC_for_MC_Keyblock",1 );
				  return (TRANSACTION_NOT_VERIFIED);
			   }
			 /* ATC*/	//	 auth_tx.EMV_details.atc
			 // ONLY For MC , for VISA and AMEX  0000
		   if (strcmp(auth_tx.BIN01_details.card_family,CARD_MASTERCARD) == 0)
			{
				if(strlen(auth_tx.EMV_details.atc) == 0)
				{
					racal_log_message( 2, 2,"ATC is blank.","racal_Verify_ARQC_for_MC_Keyblock",1 );
					return(TRANSACTION_NOT_VERIFIED);

				}else
				{
					genutil_asc_to_bin( auth_tx.EMV_details.atc,temp_txt,&bin_len);
					printf("ATC - %s \n",auth_tx.EMV_details.atc);
					//strncat(output,temp_txt,bin_len);
					 memcpy(&output[out_len],temp_txt,2);
					 out_len += 2;
				}
				if(strlen(auth_tx.EMV_details.unpredictable_number) == 0)
				{
					racal_log_message( 2, 2,"Unpredctible No is blank.","racal_Verify_ARQC_for_MC_Keyblock",1 );
					return(TRANSACTION_NOT_VERIFIED);
				}else
				{	genutil_asc_to_bin( auth_tx.EMV_details.unpredictable_number,temp_txt,&bin_len);
					memcpy(&output[out_len],temp_txt,4);
					 out_len += 4;
				}
			}else
			{
					genutil_asc_to_bin("000000000000",temp_txt,&bin_len);
					memcpy(&output[out_len],temp_txt,6);
						  out_len += 6;
			}
			 /* Tx data Len*/
			 /* Tx Data gather for CVN
		#1
				Visa CVN 10

				Tags '9F02', '9F03', '9F1A', '95', '5F2A', '9A', '9C', '9F37', '82' ,'9F36', �9F10� (CVR part)

				Visa CVN 17

				Tags �9F02�, �9F37�,�9F36�, �9F10� ( IAD Byte 5) .

				MC CVN 16

				Tags '9F02', '9F03', '9F1A', '95', '5F2A', '9A', '9C', '9F37', '82' ,'9F36', �9F10� (CVR part)
			*/
				int emv_org_len =0;
				BYTE tag_data[255]= {0};
				if(0 == strcmp(auth_tx.TLF01_details.handler_queue,"dcpisoA"))
				{
					emv_org_len = genutil_bcd_to_int( auth_tx.EMV_details.emv_block, 2 );
				}
				else
				{
					 if (strcmp(auth_tx.BIN01_details.card_family,CARD_MASTERCARD) == 0)
						 emv_org_len = genutil_bcd_to_int( auth_tx.EMV_details.emv_block, 2 );
					 else if (strcmp(auth_tx.BIN01_details.card_family,CARD_VISA) == 0)
						  emv_org_len = auth_tx.EMV_details.emv_block[0];
					 else if (strcmp(auth_tx.BIN01_details.card_family,CARD_AMEX) == 0)
					 	{
					 		for( i=0; i<3; i++ )
					      {
					         temp_msg_len[i] = ascii_ebcdic_conv (auth_tx.EMV_details.emv_block[i], E2A);
					      }
					 	emv_org_len = atoi(temp_msg_len);
					 	}
				}
				printf("EMV LEN - %d \n",emv_org_len);
				memset(tag_data,0x00,sizeof(tag_data));
				if (strcmp(auth_tx.BIN01_details.card_family,CARD_AMEX) == 0 )
				{

					if(0 == strcmp(auth_tx.TLF01_details.handler_queue,"dcpisoA") )
					{
						/*if(strncmp(auth_tx.TLF01_details.pos_entry_mode,"007",3) == 0 ||
						   strncmp(auth_tx.TLF01_details.pos_entry_mode,"07",2) == 0
						   )
						{
						bin_len=	get_EMV_CVN_17_data(emv_org_len,&auth_tx.EMV_details.emv_block,&tag_data);
						bin_len= 16;
						}
						else */
						{
						bin_len=get_EMV_CVN_10_16_data(emv_org_len,&auth_tx.EMV_details.emv_block,&tag_data);
					 	bin_len = 37;
						}
					}
					else
					{
						bin_len = get_EMV_Data_AMEX(emv_org_len,&auth_tx.EMV_details.emv_block,&tag_data);

					}

				}
				/*else if (strcmp(auth_tx.BIN01_details.card_family,CARD_VISA) == 0 &&
					(strncmp(auth_tx.TLF01_details.pos_entry_mode,"007",3) == 0 ||
 					strncmp(auth_tx.TLF01_details.pos_entry_mode,"07",2) == 0 ))
				 {
						printf("calling get_EMV_CVN_17_data - \n");

						bin_len=get_EMV_CVN_17_data(emv_org_len,&auth_tx.EMV_details.emv_block,&tag_data);
						bin_len= 16;
				 }*/
				else if (strcmp(auth_tx.BIN01_details.card_family,CARD_VISA) == 0 )
				{
					if(strncmp(auth_tx.EMV_details.iad+4,"0A",2) == 0)
					{
						bin_len=get_EMV_CVN_10_16_data(emv_org_len,&auth_tx.EMV_details.emv_block,&tag_data);
						bin_len = BIN_LEN_CVN10;
					}
					else
					{
						bin_len=get_EMV_CVN_17_data(emv_org_len,&auth_tx.EMV_details.emv_block,&tag_data);
						bin_len= BIN_LEN_CVN17;
					}

				}
				 else
				 {
						printf("calling get_EMV_CVN_16_data - \n");
						bin_len=get_EMV_CVN_10_16_data(emv_org_len,&auth_tx.EMV_details.emv_block,&tag_data);
						bin_len = 40;
				 }
				 int pad_len = bin_len%8;
				 if (pad_len > 0)
				 {
						bin_len += (8-pad_len);
				 }// Pad with Zero's

				/*if (strcmp(auth_tx.BIN01_details.card_family,CARD_AMEX) == 0 &&
					(strncmp(auth_tx.TLF01_details.pos_entry_mode,"007",3) == 0 ||
 					strncmp(auth_tx.TLF01_details.pos_entry_mode,"07",2) == 0 ))
					bin_len = 16;
				else */if(strcmp(auth_tx.BIN01_details.card_family,CARD_AMEX) == 0)
					bin_len = 37;

				printf("EMV TX DATA LEN - %d \n",bin_len);
				printf("EMV TX DATA  - %s \n",tag_data);

				sprintf(&output[out_len],"%2x",bin_len);

					out_len +=2;
				memcpy(&output[out_len],tag_data,bin_len);
				out_len += bin_len;
			 /* Tx data*/ // TAGs

			 /* Deliemter*/

				 memcpy(&output[out_len],delimiter,1);
				 out_len += 1;
			 /* ARQC/ARPC*/

			 genutil_asc_to_bin( auth_tx.EMV_details.app_crypto,temp_txt,&bin_len);
			 //strncat(output, temp_txt,bin_len);
			 //printf("APP CRYPTO - %s \n",auth_tx.EMV_details.app_crypto);
			 memcpy(&output[out_len],temp_txt,8);
				 out_len += 8;
			 /* ARC*/
			if (strcmp(auth_tx.BIN01_details.card_family,CARD_VISA) == 0)
			 {
					 //genutil_asc_to_bin( "0000",temp_txt,&bin_len);
					 memcpy(&output[out_len],"00",2);
					 out_len += 2;
			 }else if (strcmp(auth_tx.BIN01_details.card_family,CARD_MASTERCARD) == 0)
			 {
					 genutil_asc_to_bin( "0010",temp_txt,&bin_len);
					 memcpy(&output[out_len],temp_txt,2);
					 out_len += 2;
			 }else if (strcmp(auth_tx.BIN01_details.card_family,CARD_AMEX) == 0)
			 	{
			 	// genutil_asc_to_bin( "0000",temp_txt,&bin_len);
					 memcpy(&output[out_len],"00",2);
					 out_len += 2;
			 	}


			 /* End of msg*/
			 //if the output is bigger then 500 do not send it out.
			 if (out_len > 500)
			 {
				racal_log_message( 2, 2,"ARQC Cmd Msg to long.","racal_Verify_ARQC_for_MC_Keyblock",1 );
				return(TRANSACTION_NOT_VERIFIED);
			 }
			 //send the message to the racal box.
			 if (racalSendToPort_Bin(output,out_len) == TRANSACTION_VERIFIED)
			 {
				/*parse the message check for the message header.*/
				memset(&dummy,0,Dummy_Size);
				strncpy(dummy,returnString,4);
				if(strcmp(dummy,msgHeader) != 0)
				{
				   racal_log_message( 2, 2,"ARQC Cmd header mis-match.","racal_Verify_ARQC_for_MC_Keyblock",1 );
				   return(TRANSACTION_NOT_VERIFIED);
				}
				/*parse the message check for the response code.*/
				memset(&dummy,0,Dummy_Size);
				strncpy(dummy,returnString + 4, 2);

				if(strcmp(dummy,"KR") !=0)
				{
				   racal_log_message( 2, 2,"ARQC Cmd Code mis-match.","racal_Verify_ARQC_for_MC_Keyblock",1 );
				   return(TRANSACTION_NOT_VERIFIED);
				}

				/*parse the message check for the error code.*/
				memset(&dummy,0,Dummy_Size);
				strncpy(dummy,returnString + 6, 2);

				  //if this value is not 00 reject the transaction.
				/* '00' : No error
				'01' : ARQC/TC/AAC verification failed
				'04' : Mode Flag not 0, 1 or 2
				'05' : Unrecognised Scheme ID
				'10' : MK parity error
				'68' : Command disabled
				*/
				if(strcmp(dummy,"00") != 0)
				{
				   if(strcmp(dummy,"01") == 0)
					{
					   racal_log_message( 2, 2,"ARPC AAC validation failed.","racal_Verify_ARQC_for_MC_Keyblock",1 );
				   }
				   if(strcmp(dummy,"04") == 0)
					  racal_log_message( 2, 2,"Mode flag not 0 1 or 2.","racal_Verify_ARQC_for_MC_Keyblock",1 );
				   if(strcmp(dummy,"05") == 0)
					  racal_log_message( 2, 2,"Unrecognised Scheme ID.","racal_Verify_ARQC_for_MC_Keyblock",1 );
				   if(strcmp(dummy,"10") == 0)
					  racal_log_message( 2, 2,"MK parity error.","racal_Verify_ARQC_for_MC_Keyblock",1 );
				   if(strcmp(dummy,"68") == 0)
					  racal_log_message( 2, 2,"Command disabled.","racal_Verify_ARQC_for_MC_Keyblock",1 );
					 return(TRANSACTION_NOT_VERIFIED);
				}

				CHAR temp_response_tag[35]={0};
				memcpy(dummy,returnString + 8, 8);		 // This is the ARPC value.
				 if (strcmp(auth_tx.BIN01_details.card_family,CARD_MASTERCARD) == 0)
				 {
					 dummy[8]=0x00;
					 dummy[9]=0x10;
				 }
				 else  if (strcmp(auth_tx.BIN01_details.card_family,CARD_VISA) == 0 ||
				 	strcmp(auth_tx.BIN01_details.card_family,CARD_AMEX) == 0)
				 {
					 dummy[8]=0x30;
					 dummy[9]=0x30;
				 }
				 // Prepare EMV block for response
				 strcpy(ARPC_RAW,"91");
				 strcat(ARPC_RAW,"0A");

					if(0 == strcmp(auth_tx.TLF01_details.handler_queue,"dcpisoA") )
					{
						auth_tx.EMV_details.future_field1[0]=0x00;
						//int resp_dcp_len =12;
						auth_tx.EMV_details.future_field1[1]=0x12;
						strcpy(temp_response_tag,ARPC_RAW);
						genutil_asc_to_bin( temp_response_tag,temp_txt,&bin_len);
						memcpy(&auth_tx.EMV_details.future_field1[2],temp_txt,2);
						memcpy(&auth_tx.EMV_details.future_field1[4],dummy,10);
					}
					else
					{
						 if (strcmp(auth_tx.BIN01_details.card_family,CARD_MASTERCARD) == 0)
						 {	sprintf(auth_tx.EMV_details.future_field1,"%03d",12);
							strcpy(temp_response_tag,ARPC_RAW);
							genutil_asc_to_bin( temp_response_tag,temp_txt,&bin_len);
							memcpy(&auth_tx.EMV_details.future_field1[3],temp_txt,2);
							memcpy(&auth_tx.EMV_details.future_field1[5],dummy,10);
						 }
						 else  if (strcmp(auth_tx.BIN01_details.card_family,CARD_VISA) == 0)
						 {
							auth_tx.EMV_details.future_field1[0]=0x0F;
							 strcpy(temp_response_tag,"01000C");
							 strcat(temp_response_tag,ARPC_RAW);
							genutil_asc_to_bin( temp_response_tag,temp_txt,&bin_len);
							memcpy(&auth_tx.EMV_details.future_field1[1],temp_txt,5);
							memcpy(&auth_tx.EMV_details.future_field1[6],dummy,10);
						 }else if (strcmp(auth_tx.BIN01_details.card_family,CARD_AMEX) == 0)
						 {
							auth_tx.EMV_details.future_field1[0]=0x0A;
							memcpy(&auth_tx.EMV_details.future_field1[1],dummy,10);

						 }

					}

		   }

	  return (TRANSACTION_VERIFIED);
}/*racal_Verify_ARQC_for_MC_Keyblock*/

/**************************************************************/

int racal_Verify_ARQC_for_VISA_Keyblock()
{
	   char msgHeader[5]={0};
	   char delimiter[2]={0};
	   char output[500]={0};
	   INT  bin_len=0;
	   INT  out_len=0;
	   int	cnt=0;
	   int  result=0;
	   int  jcnt=0;
	   BYTE MK_AC[33]={0};
	   BYTE TX_DATA[256]={0};
	   BYTE temp_txt[100]={0};
	   BYTE ARPC_RAW[20]={0};
	   INT  resp_len = 0;
	   INT  i= 0;
	   BYTE temp_msg_len[10]={0};

	   memset (&msgHeader,0,sizeof(msgHeader));
	   memset (&delimiter,0,sizeof(delimiter));
	   memset (&output,0,sizeof(output));
	   memset (&returnString,0,ReturnString_Size);
	   strcpy (delimiter,";");
	   strcpy (msgHeader ,"1234");

			 strcat(output,msgHeader);
			 strcat(output,"KQ"); /* Changed Command code KQ- TF Phani*/
			/*Mode flag
				Mode of operation:
			 '0' : Perform ARQC Verification only
			 '1' : Perform ARQC Verification and ARPC Generation
			 '2' : Perform ARPC Generation only.
			*/
			strcat(output,"1");
			// strcat(output,"2");
			//mode_falg = '2';
			/* Identifier of the EMV scheme:
			'0' : Visa VSDC or UKIS
			 '1' : Europay or MasterCard M/Chip
			 '2' : American Express AEIPS 4.1 */
			if (strcmp(auth_tx.BIN01_details.card_family,CARD_VISA) == 0)
				strcat(output,"0");
			else if (strcmp(auth_tx.BIN01_details.card_family,CARD_MASTERCARD) == 0)
				strcat(output,"1");
			else if (strcmp(auth_tx.BIN01_details.card_family,CARD_AMEX) == 0)
				strcat(output,"2");


			 /* Issuer MAster key - MK-AC */
			 strcpy(MK_AC, auth_tx.CAF01_details.cvk[5].cvkA);
			 strcat(MK_AC, auth_tx.CAF01_details.cvk[5].cvkB);

			 if(strlen(MK_AC) == 0)
			{
				racal_log_message( 2, 2,"Master key is blank.","racal_Verify_ARQC_for_VISA_Keyblock",1 );
				return(TRANSACTION_NOT_VERIFIED);

			}else
			{	strcat(output,"U");
				strcat(output,MK_AC);
			}
			 out_len = strlen(output);
			 /* PAN SEQ NUMBER*/
			 if(strlen(auth_tx.TLF01_details.card_num) >= 14 )
			 {
				  memset(&dummy,0,Dummy_Size);

				  if (strcmp(auth_tx.BIN01_details.card_family,CARD_AMEX) == 0)
				  strncpy(dummy,&auth_tx.TLF01_details.card_num[1] ,14);
				  else
				  strncpy(dummy,&auth_tx.TLF01_details.card_num[2] ,14);
				  /* need to validate PAN SEQ no  */
				  if(strcmp(auth_tx.BIN01_details.card_family,CARD_AMEX) == 0)
				  {
				 if(strlen(auth_tx.EMV_details.pan_sequence_number) > 0)
				  {
					if(strlen(auth_tx.EMV_details.pan_sequence_number) == 2)
						strncat(dummy,&auth_tx.EMV_details.pan_sequence_number[0],2);
					else if(strlen(auth_tx.EMV_details.pan_sequence_number) == 3)
						strncat(dummy,&auth_tx.EMV_details.pan_sequence_number[1],2);
				  }
				  else if(strlen(auth_tx.host2_message_seq_nbr) > 0)
					  {
						  if(strlen(auth_tx.host2_message_seq_nbr) == 4)
							  strncat(dummy,&auth_tx.host2_message_seq_nbr[2],2);
						  else if(strlen(auth_tx.host2_message_seq_nbr) == 3)
							  strncat(dummy,&auth_tx.host2_message_seq_nbr[1],2);
					  }
					  else
					  	strcat(dummy,"00");
				}
				else
				{
					if(strlen(auth_tx.host2_message_seq_nbr) > 0)
					{
						if(strlen(auth_tx.host2_message_seq_nbr) == 4)
						strncat(dummy,&auth_tx.host2_message_seq_nbr[2],2);
					  else if(strlen(auth_tx.host2_message_seq_nbr) == 3)
						strncat(dummy,&auth_tx.host2_message_seq_nbr[1],2);
					}
				    else if(strlen(auth_tx.EMV_details.pan_sequence_number) > 0)
				    {
				    	if(strlen(auth_tx.EMV_details.pan_sequence_number) == 2)
				    		strncat(dummy,&auth_tx.EMV_details.pan_sequence_number[0],2);
				    	else if(strlen(auth_tx.EMV_details.pan_sequence_number) == 3)
				    		strncat(dummy,&auth_tx.EMV_details.pan_sequence_number[1],2);
				    }
				  else
							strcat(dummy,"00");
				}
				  dummy[16] = '\0';
				  genutil_asc_to_bin( dummy,temp_txt,&bin_len);
				  //strncat(output,temp_txt,bin_len);
				  memcpy(&output[out_len],temp_txt,8);
				  out_len += 8;
				  //strcat(output,dummy);
			  }
			  else
			  {
				  racal_log_message( 2, 2,"ARQC validation: card_num is < 14.","racal_Verify_ARQC_for_VISA_Keyblock",1 );
				  return (TRANSACTION_NOT_VERIFIED);
			   }
			 /* ATC*/	//	 auth_tx.EMV_details.atc
			 // ONLY For MC , for VISA and AMEX  0000
		   if (strcmp(auth_tx.BIN01_details.card_family,CARD_MASTERCARD) == 0)
			{
				if(strlen(auth_tx.EMV_details.atc) == 0)
				{
					racal_log_message( 2, 2,"ATC is blank.","racal_Verify_ARQC_for_VISA_Keyblock",1 );
					return(TRANSACTION_NOT_VERIFIED);

				}else
				{
					genutil_asc_to_bin( auth_tx.EMV_details.atc,temp_txt,&bin_len);
					printf("ATC - %s \n",auth_tx.EMV_details.atc);
					//strncat(output,temp_txt,bin_len);
					 memcpy(&output[out_len],temp_txt,2);
					 out_len += 2;
				}
				if(strlen(auth_tx.EMV_details.unpredictable_number) == 0)
				{
					racal_log_message( 2, 2,"Unpredctible No is blank.","racal_Verify_ARQC_for_VISA_Keyblock",1 );
					return(TRANSACTION_NOT_VERIFIED);
				}else
				{	genutil_asc_to_bin( auth_tx.EMV_details.unpredictable_number,temp_txt,&bin_len);
					memcpy(&output[out_len],temp_txt,4);
					 out_len += 4;
				}
			}else
			{
					genutil_asc_to_bin("000000000000",temp_txt,&bin_len);
					memcpy(&output[out_len],temp_txt,6);
						  out_len += 6;
			}
			 /* Tx data Len*/
			 /* Tx Data gather for CVN
		#1
				Visa CVN 10

				Tags '9F02', '9F03', '9F1A', '95', '5F2A', '9A', '9C', '9F37', '82' ,'9F36', �9F10� (CVR part)

				Visa CVN 17

				Tags �9F02�, �9F37�,�9F36�, �9F10� ( IAD Byte 5) .

				MC CVN 16

				Tags '9F02', '9F03', '9F1A', '95', '5F2A', '9A', '9C', '9F37', '82' ,'9F36', �9F10� (CVR part)
			*/
				int emv_org_len =0;
				BYTE tag_data[255]= {0};
				if(0 == strcmp(auth_tx.TLF01_details.handler_queue,"dcpisoA"))
				{
					emv_org_len = genutil_bcd_to_int( auth_tx.EMV_details.emv_block, 2 );
				}
				else
				{
					 if (strcmp(auth_tx.BIN01_details.card_family,CARD_MASTERCARD) == 0)
						 emv_org_len = genutil_bcd_to_int( auth_tx.EMV_details.emv_block, 2 );
					 else if (strcmp(auth_tx.BIN01_details.card_family,CARD_VISA) == 0)
						  emv_org_len = auth_tx.EMV_details.emv_block[0];
					 else if (strcmp(auth_tx.BIN01_details.card_family,CARD_AMEX) == 0)
					 	{
					 		for( i=0; i<3; i++ )
					      {
					         temp_msg_len[i] = ascii_ebcdic_conv (auth_tx.EMV_details.emv_block[i], E2A);
					      }
					 	emv_org_len = atoi(temp_msg_len);
					 	}
				}
				printf("EMV LEN - %d \n",emv_org_len);
				memset(tag_data,0x00,sizeof(tag_data));
				if (strcmp(auth_tx.BIN01_details.card_family,CARD_AMEX) == 0 )
				{

					if(0 == strcmp(auth_tx.TLF01_details.handler_queue,"dcpisoA") )
					{
						/*if(strncmp(auth_tx.TLF01_details.pos_entry_mode,"007",3) == 0 ||
						   strncmp(auth_tx.TLF01_details.pos_entry_mode,"07",2) == 0
						   )
						{
						bin_len=	get_EMV_CVN_17_data(emv_org_len,&auth_tx.EMV_details.emv_block,&tag_data);
						bin_len= 16;
						}
						else */
						{
						bin_len=get_EMV_CVN_10_16_data(emv_org_len,&auth_tx.EMV_details.emv_block,&tag_data);
					 	bin_len = 37;
						}
					}
					else
					{
						bin_len = get_EMV_Data_AMEX(emv_org_len,&auth_tx.EMV_details.emv_block,&tag_data);

					}

				}
				/*else if (strcmp(auth_tx.BIN01_details.card_family,CARD_VISA) == 0 &&
					(strncmp(auth_tx.TLF01_details.pos_entry_mode,"007",3) == 0 ||
 					strncmp(auth_tx.TLF01_details.pos_entry_mode,"07",2) == 0 ))
				 {
						printf("calling get_EMV_CVN_17_data - \n");

						bin_len=get_EMV_CVN_17_data(emv_org_len,&auth_tx.EMV_details.emv_block,&tag_data);
						bin_len= 16;
				 }*/
				else if (strcmp(auth_tx.BIN01_details.card_family,CARD_VISA) == 0 )
				{
					if(strncmp(auth_tx.EMV_details.iad+4,"0A",2) == 0)
					{
						bin_len=get_EMV_CVN_10_16_data(emv_org_len,&auth_tx.EMV_details.emv_block,&tag_data);
						bin_len = BIN_LEN_CVN10;
					}
					else
					{
						bin_len=get_EMV_CVN_17_data(emv_org_len,&auth_tx.EMV_details.emv_block,&tag_data);
						bin_len= BIN_LEN_CVN17;
					}

				}
				 else
				 {
						printf("calling get_EMV_CVN_16_data - \n");
						bin_len=get_EMV_CVN_10_16_data(emv_org_len,&auth_tx.EMV_details.emv_block,&tag_data);
						bin_len = 40;
				 }
				 int pad_len = bin_len%8;
				 if (pad_len > 0)
				 {
						bin_len += (8-pad_len);
				 }// Pad with Zero's

				/*if (strcmp(auth_tx.BIN01_details.card_family,CARD_AMEX) == 0 &&
					(strncmp(auth_tx.TLF01_details.pos_entry_mode,"007",3) == 0 ||
 					strncmp(auth_tx.TLF01_details.pos_entry_mode,"07",2) == 0 ))
					bin_len = 16;
				else */if(strcmp(auth_tx.BIN01_details.card_family,CARD_AMEX) == 0)
					bin_len = 37;

				printf("EMV TX DATA LEN - %d \n",bin_len);
				printf("EMV TX DATA  - %s \n",tag_data);

				sprintf(&output[out_len],"%2x",bin_len);

					out_len +=2;
				memcpy(&output[out_len],tag_data,bin_len);
				out_len += bin_len;
			 /* Tx data*/ // TAGs

			 /* Deliemter*/

				 memcpy(&output[out_len],delimiter,1);
				 out_len += 1;
			 /* ARQC/ARPC*/

			 genutil_asc_to_bin( auth_tx.EMV_details.app_crypto,temp_txt,&bin_len);
			 //strncat(output, temp_txt,bin_len);
			 //printf("APP CRYPTO - %s \n",auth_tx.EMV_details.app_crypto);
			 memcpy(&output[out_len],temp_txt,8);
				 out_len += 8;
			 /* ARC*/
			if (strcmp(auth_tx.BIN01_details.card_family,CARD_VISA) == 0)
			 {
					 //genutil_asc_to_bin( "0000",temp_txt,&bin_len);
					 memcpy(&output[out_len],"00",2);
					 out_len += 2;
			 }else if (strcmp(auth_tx.BIN01_details.card_family,CARD_MASTERCARD) == 0)
			 {
					 genutil_asc_to_bin( "0010",temp_txt,&bin_len);
					 memcpy(&output[out_len],temp_txt,2);
					 out_len += 2;
			 }else if (strcmp(auth_tx.BIN01_details.card_family,CARD_AMEX) == 0)
			 	{
			 	// genutil_asc_to_bin( "0000",temp_txt,&bin_len);
					 memcpy(&output[out_len],"00",2);
					 out_len += 2;
			 	}


			 /* End of msg*/
			 //if the output is bigger then 500 do not send it out.
			 if (out_len > 500)
			 {
				racal_log_message( 2, 2,"ARQC Cmd Msg to long.","racal_Verify_ARQC_for_VISA_Keyblock",1 );
				return(TRANSACTION_NOT_VERIFIED);
			 }
			 //send the message to the racal box.
			 if (racalSendToPort_Bin(output,out_len) == TRANSACTION_VERIFIED)
			 {
				/*parse the message check for the message header.*/
				memset(&dummy,0,Dummy_Size);
				strncpy(dummy,returnString,4);
				if(strcmp(dummy,msgHeader) != 0)
				{
				   racal_log_message( 2, 2,"ARQC Cmd header mis-match.","racal_Verify_ARQC_for_VISA_Keyblock",1 );
				   return(TRANSACTION_NOT_VERIFIED);
				}
				/*parse the message check for the response code.*/
				memset(&dummy,0,Dummy_Size);
				strncpy(dummy,returnString + 4, 2);

				if(strcmp(dummy,"KR") !=0)
				{
				   racal_log_message( 2, 2,"ARQC Cmd Code mis-match.","racal_Verify_ARQC_for_VISA_Keyblock",1 );
				   return(TRANSACTION_NOT_VERIFIED);
				}

				/*parse the message check for the error code.*/
				memset(&dummy,0,Dummy_Size);
				strncpy(dummy,returnString + 6, 2);

				  //if this value is not 00 reject the transaction.
				/* '00' : No error
				'01' : ARQC/TC/AAC verification failed
				'04' : Mode Flag not 0, 1 or 2
				'05' : Unrecognised Scheme ID
				'10' : MK parity error
				'68' : Command disabled
				*/
				if(strcmp(dummy,"00") != 0)
				{
				   if(strcmp(dummy,"01") == 0)
					{
					   racal_log_message( 2, 2,"ARPC AAC validation failed.","racal_Verify_ARQC_for_VISA_Keyblock",1 );
				   }
				   if(strcmp(dummy,"04") == 0)
					  racal_log_message( 2, 2,"Mode flag not 0 1 or 2.","racal_Verify_ARQC_for_VISA_Keyblock",1 );
				   if(strcmp(dummy,"05") == 0)
					  racal_log_message( 2, 2,"Unrecognised Scheme ID.","racal_Verify_ARQC_for_VISA_Keyblock",1 );
				   if(strcmp(dummy,"10") == 0)
					  racal_log_message( 2, 2,"MK parity error.","racal_Verify_ARQC_for_VISA_Keyblock",1 );
				   if(strcmp(dummy,"68") == 0)
					  racal_log_message( 2, 2,"Command disabled.","racal_Verify_ARQC_for_VISA_Keyblock",1 );
					 return(TRANSACTION_NOT_VERIFIED);
				}

				CHAR temp_response_tag[35]={0};
				memcpy(dummy,returnString + 8, 8);		 // This is the ARPC value.
				 if (strcmp(auth_tx.BIN01_details.card_family,CARD_MASTERCARD) == 0)
				 {
					 dummy[8]=0x00;
					 dummy[9]=0x10;
				 }
				 else  if (strcmp(auth_tx.BIN01_details.card_family,CARD_VISA) == 0 ||
				 	strcmp(auth_tx.BIN01_details.card_family,CARD_AMEX) == 0)
				 {
					 dummy[8]=0x30;
					 dummy[9]=0x30;
				 }
				 // Prepare EMV block for response
				 strcpy(ARPC_RAW,"91");
				 strcat(ARPC_RAW,"0A");

					if(0 == strcmp(auth_tx.TLF01_details.handler_queue,"dcpisoA") )
					{
						auth_tx.EMV_details.future_field1[0]=0x00;
						//int resp_dcp_len =12;
						auth_tx.EMV_details.future_field1[1]=0x12;
						strcpy(temp_response_tag,ARPC_RAW);
						genutil_asc_to_bin( temp_response_tag,temp_txt,&bin_len);
						memcpy(&auth_tx.EMV_details.future_field1[2],temp_txt,2);
						memcpy(&auth_tx.EMV_details.future_field1[4],dummy,10);
					}
					else
					{
						 if (strcmp(auth_tx.BIN01_details.card_family,CARD_MASTERCARD) == 0)
						 {	sprintf(auth_tx.EMV_details.future_field1,"%03d",12);
							strcpy(temp_response_tag,ARPC_RAW);
							genutil_asc_to_bin( temp_response_tag,temp_txt,&bin_len);
							memcpy(&auth_tx.EMV_details.future_field1[3],temp_txt,2);
							memcpy(&auth_tx.EMV_details.future_field1[5],dummy,10);
						 }
						 else  if (strcmp(auth_tx.BIN01_details.card_family,CARD_VISA) == 0)
						 {
							auth_tx.EMV_details.future_field1[0]=0x0F;
							 strcpy(temp_response_tag,"01000C");
							 strcat(temp_response_tag,ARPC_RAW);
							genutil_asc_to_bin( temp_response_tag,temp_txt,&bin_len);
							memcpy(&auth_tx.EMV_details.future_field1[1],temp_txt,5);
							memcpy(&auth_tx.EMV_details.future_field1[6],dummy,10);
						 }else if (strcmp(auth_tx.BIN01_details.card_family,CARD_AMEX) == 0)
						 {
							auth_tx.EMV_details.future_field1[0]=0x0A;
							memcpy(&auth_tx.EMV_details.future_field1[1],dummy,10);

						 }

					}

		   }

	  return (TRANSACTION_VERIFIED);
}/*racal_Verify_ARQC_for_VISA_Keyblock*/

/**************************************************************/

int racal_Verify_ARQC_for_AMEX_Keyblock()
{
	   char msgHeader[5]={0};
	   char delimiter[2]={0};
	   char output[500]={0};
	   INT  bin_len=0;
	   INT  out_len=0;
	   int	cnt=0;
	   int  result=0;
	   int  jcnt=0;
	   BYTE MK_AC[33]={0};
	   BYTE TX_DATA[256]={0};
	   BYTE temp_txt[100]={0};
	   BYTE ARPC_RAW[20]={0};
	   INT  resp_len = 0;
	   INT  i= 0;
	   BYTE temp_msg_len[10]={0};

	   memset (&msgHeader,0,sizeof(msgHeader));
	   memset (&delimiter,0,sizeof(delimiter));
	   memset (&output,0,sizeof(output));
	   memset (&returnString,0,ReturnString_Size);
	   strcpy (delimiter,";");
	   strcpy (msgHeader ,"1234");

			 strcat(output,msgHeader);
			 strcat(output,"KQ"); /* Changed Command code KQ- TF Phani*/
			/*Mode flag
				Mode of operation:
			 '0' : Perform ARQC Verification only
			 '1' : Perform ARQC Verification and ARPC Generation
			 '2' : Perform ARPC Generation only.
			*/
			strcat(output,"1");
			// strcat(output,"2");
			//mode_falg = '2';
			/* Identifier of the EMV scheme:
			'0' : Visa VSDC or UKIS
			 '1' : Europay or MasterCard M/Chip
			 '2' : American Express AEIPS 4.1 */
			if (strcmp(auth_tx.BIN01_details.card_family,CARD_VISA) == 0)
				strcat(output,"0");
			else if (strcmp(auth_tx.BIN01_details.card_family,CARD_MASTERCARD) == 0)
				strcat(output,"1");
			else if (strcmp(auth_tx.BIN01_details.card_family,CARD_AMEX) == 0)
				strcat(output,"2");


			 /* Issuer MAster key - MK-AC */
			 strcpy(MK_AC, auth_tx.CAF01_details.cvk[5].cvkA);
			 strcat(MK_AC, auth_tx.CAF01_details.cvk[5].cvkB);

			 if(strlen(MK_AC) == 0)
			{
				racal_log_message( 2, 2,"Master key is blank.","racal_Verify_ARQC_for_AMEX_Keyblock",1 );
				return(TRANSACTION_NOT_VERIFIED);

			}else
			{	strcat(output,"U");
				strcat(output,MK_AC);
			}
			 out_len = strlen(output);
			 /* PAN SEQ NUMBER*/
			 if(strlen(auth_tx.TLF01_details.card_num) >= 14 )
			 {
				  memset(&dummy,0,Dummy_Size);

				  if (strcmp(auth_tx.BIN01_details.card_family,CARD_AMEX) == 0)
				  strncpy(dummy,&auth_tx.TLF01_details.card_num[1] ,14);
				  else
				  strncpy(dummy,&auth_tx.TLF01_details.card_num[2] ,14);
				  /* need to validate PAN SEQ no  */
				  if(strcmp(auth_tx.BIN01_details.card_family,CARD_AMEX) == 0)
				  {
				 if(strlen(auth_tx.EMV_details.pan_sequence_number) > 0)
				  {
					if(strlen(auth_tx.EMV_details.pan_sequence_number) == 2)
						strncat(dummy,&auth_tx.EMV_details.pan_sequence_number[0],2);
					else if(strlen(auth_tx.EMV_details.pan_sequence_number) == 3)
						strncat(dummy,&auth_tx.EMV_details.pan_sequence_number[1],2);
				  }
				  else if(strlen(auth_tx.host2_message_seq_nbr) > 0)
					  {
						  if(strlen(auth_tx.host2_message_seq_nbr) == 4)
							  strncat(dummy,&auth_tx.host2_message_seq_nbr[2],2);
						  else if(strlen(auth_tx.host2_message_seq_nbr) == 3)
							  strncat(dummy,&auth_tx.host2_message_seq_nbr[1],2);
					  }
					  else
					  	strcat(dummy,"00");
				}
				else
				{
					if(strlen(auth_tx.host2_message_seq_nbr) > 0)
					{
						if(strlen(auth_tx.host2_message_seq_nbr) == 4)
						strncat(dummy,&auth_tx.host2_message_seq_nbr[2],2);
					  else if(strlen(auth_tx.host2_message_seq_nbr) == 3)
						strncat(dummy,&auth_tx.host2_message_seq_nbr[1],2);
					}
				    else if(strlen(auth_tx.EMV_details.pan_sequence_number) > 0)
				    {
				    	if(strlen(auth_tx.EMV_details.pan_sequence_number) == 2)
				    		strncat(dummy,&auth_tx.EMV_details.pan_sequence_number[0],2);
				    	else if(strlen(auth_tx.EMV_details.pan_sequence_number) == 3)
				    		strncat(dummy,&auth_tx.EMV_details.pan_sequence_number[1],2);
				    }
				  else
							strcat(dummy,"00");
				}
				  dummy[16] = '\0';
				  genutil_asc_to_bin( dummy,temp_txt,&bin_len);
				  //strncat(output,temp_txt,bin_len);
				  memcpy(&output[out_len],temp_txt,8);
				  out_len += 8;
				  //strcat(output,dummy);
			  }
			  else
			  {
				  racal_log_message( 2, 2,"ARQC validation: card_num is < 14.","racal_Verify_ARQC_for_AMEX_Keyblock",1 );
				  return (TRANSACTION_NOT_VERIFIED);
			   }
			 /* ATC*/	//	 auth_tx.EMV_details.atc
			 // ONLY For MC , for VISA and AMEX  0000
		   if (strcmp(auth_tx.BIN01_details.card_family,CARD_MASTERCARD) == 0)
			{
				if(strlen(auth_tx.EMV_details.atc) == 0)
				{
					racal_log_message( 2, 2,"ATC is blank.","racal_Verify_ARQC_for_AMEX_Keyblock",1 );
					return(TRANSACTION_NOT_VERIFIED);

				}else
				{
					genutil_asc_to_bin( auth_tx.EMV_details.atc,temp_txt,&bin_len);
					printf("ATC - %s \n",auth_tx.EMV_details.atc);
					//strncat(output,temp_txt,bin_len);
					 memcpy(&output[out_len],temp_txt,2);
					 out_len += 2;
				}
				if(strlen(auth_tx.EMV_details.unpredictable_number) == 0)
				{
					racal_log_message( 2, 2,"Unpredctible No is blank.","racal_Verify_ARQC_for_AMEX_Keyblock",1 );
					return(TRANSACTION_NOT_VERIFIED);
				}else
				{	genutil_asc_to_bin( auth_tx.EMV_details.unpredictable_number,temp_txt,&bin_len);
					memcpy(&output[out_len],temp_txt,4);
					 out_len += 4;
				}
			}else
			{
					genutil_asc_to_bin("000000000000",temp_txt,&bin_len);
					memcpy(&output[out_len],temp_txt,6);
						  out_len += 6;
			}
			 /* Tx data Len*/
			 /* Tx Data gather for CVN
		#1
				Visa CVN 10

				Tags '9F02', '9F03', '9F1A', '95', '5F2A', '9A', '9C', '9F37', '82' ,'9F36', �9F10� (CVR part)

				Visa CVN 17

				Tags �9F02�, �9F37�,�9F36�, �9F10� ( IAD Byte 5) .

				MC CVN 16

				Tags '9F02', '9F03', '9F1A', '95', '5F2A', '9A', '9C', '9F37', '82' ,'9F36', �9F10� (CVR part)
			*/
				int emv_org_len =0;
				BYTE tag_data[255]= {0};
				if(0 == strcmp(auth_tx.TLF01_details.handler_queue,"dcpisoA"))
				{
					emv_org_len = genutil_bcd_to_int( auth_tx.EMV_details.emv_block, 2 );
				}
				else
				{
					 if (strcmp(auth_tx.BIN01_details.card_family,CARD_MASTERCARD) == 0)
						 emv_org_len = genutil_bcd_to_int( auth_tx.EMV_details.emv_block, 2 );
					 else if (strcmp(auth_tx.BIN01_details.card_family,CARD_VISA) == 0)
						  emv_org_len = auth_tx.EMV_details.emv_block[0];
					 else if (strcmp(auth_tx.BIN01_details.card_family,CARD_AMEX) == 0)
					 	{
					 		for( i=0; i<3; i++ )
					      {
					         temp_msg_len[i] = ascii_ebcdic_conv (auth_tx.EMV_details.emv_block[i], E2A);
					      }
					 	emv_org_len = atoi(temp_msg_len);
					 	}
				}
				printf("EMV LEN - %d \n",emv_org_len);
				memset(tag_data,0x00,sizeof(tag_data));
				if (strcmp(auth_tx.BIN01_details.card_family,CARD_AMEX) == 0 )
				{

					if(0 == strcmp(auth_tx.TLF01_details.handler_queue,"dcpisoA") )
					{
						/*if(strncmp(auth_tx.TLF01_details.pos_entry_mode,"007",3) == 0 ||
						   strncmp(auth_tx.TLF01_details.pos_entry_mode,"07",2) == 0
						   )
						{
						bin_len=	get_EMV_CVN_17_data(emv_org_len,&auth_tx.EMV_details.emv_block,&tag_data);
						bin_len= 16;
						}
						else */
						{
						bin_len=get_EMV_CVN_10_16_data(emv_org_len,&auth_tx.EMV_details.emv_block,&tag_data);
					 	bin_len = 37;
						}
					}
					else
					{
						bin_len = get_EMV_Data_AMEX(emv_org_len,&auth_tx.EMV_details.emv_block,&tag_data);

					}

				}
				/*else if (strcmp(auth_tx.BIN01_details.card_family,CARD_VISA) == 0 &&
					(strncmp(auth_tx.TLF01_details.pos_entry_mode,"007",3) == 0 ||
 					strncmp(auth_tx.TLF01_details.pos_entry_mode,"07",2) == 0 ))
				 {
						printf("calling get_EMV_CVN_17_data - \n");

						bin_len=get_EMV_CVN_17_data(emv_org_len,&auth_tx.EMV_details.emv_block,&tag_data);
						bin_len= 16;
				 }*/
				else if (strcmp(auth_tx.BIN01_details.card_family,CARD_VISA) == 0 )
				{
					if(strncmp(auth_tx.EMV_details.iad+4,"0A",2) == 0)
					{
						bin_len=get_EMV_CVN_10_16_data(emv_org_len,&auth_tx.EMV_details.emv_block,&tag_data);
						bin_len = BIN_LEN_CVN10;
					}
					else
					{
						bin_len=get_EMV_CVN_17_data(emv_org_len,&auth_tx.EMV_details.emv_block,&tag_data);
						bin_len= BIN_LEN_CVN17;
					}

				}
				 else
				 {
						printf("calling get_EMV_CVN_16_data - \n");
						bin_len=get_EMV_CVN_10_16_data(emv_org_len,&auth_tx.EMV_details.emv_block,&tag_data);
						bin_len = 40;
				 }
				 int pad_len = bin_len%8;
				 if (pad_len > 0)
				 {
						bin_len += (8-pad_len);
				 }// Pad with Zero's

				/*if (strcmp(auth_tx.BIN01_details.card_family,CARD_AMEX) == 0 &&
					(strncmp(auth_tx.TLF01_details.pos_entry_mode,"007",3) == 0 ||
 					strncmp(auth_tx.TLF01_details.pos_entry_mode,"07",2) == 0 ))
					bin_len = 16;
				else */if(strcmp(auth_tx.BIN01_details.card_family,CARD_AMEX) == 0)
					bin_len = 37;

				printf("EMV TX DATA LEN - %d \n",bin_len);
				printf("EMV TX DATA  - %s \n",tag_data);

				sprintf(&output[out_len],"%2x",bin_len);

					out_len +=2;
				memcpy(&output[out_len],tag_data,bin_len);
				out_len += bin_len;
			 /* Tx data*/ // TAGs

			 /* Deliemter*/

				 memcpy(&output[out_len],delimiter,1);
				 out_len += 1;
			 /* ARQC/ARPC*/

			 genutil_asc_to_bin( auth_tx.EMV_details.app_crypto,temp_txt,&bin_len);
			 //strncat(output, temp_txt,bin_len);
			 //printf("APP CRYPTO - %s \n",auth_tx.EMV_details.app_crypto);
			 memcpy(&output[out_len],temp_txt,8);
				 out_len += 8;
			 /* ARC*/
			if (strcmp(auth_tx.BIN01_details.card_family,CARD_VISA) == 0)
			 {
					 //genutil_asc_to_bin( "0000",temp_txt,&bin_len);
					 memcpy(&output[out_len],"00",2);
					 out_len += 2;
			 }else if (strcmp(auth_tx.BIN01_details.card_family,CARD_MASTERCARD) == 0)
			 {
					 genutil_asc_to_bin( "0010",temp_txt,&bin_len);
					 memcpy(&output[out_len],temp_txt,2);
					 out_len += 2;
			 }else if (strcmp(auth_tx.BIN01_details.card_family,CARD_AMEX) == 0)
			 	{
			 	// genutil_asc_to_bin( "0000",temp_txt,&bin_len);
					 memcpy(&output[out_len],"00",2);
					 out_len += 2;
			 	}


			 /* End of msg*/
			 //if the output is bigger then 500 do not send it out.
			 if (out_len > 500)
			 {
				racal_log_message( 2, 2,"ARQC Cmd Msg to long.","racal_Verify_ARQC_for_AMEX_Keyblock",1 );
				return(TRANSACTION_NOT_VERIFIED);
			 }
			 //send the message to the racal box.
			 if (racalSendToPort_Bin(output,out_len) == TRANSACTION_VERIFIED)
			 {
				/*parse the message check for the message header.*/
				memset(&dummy,0,Dummy_Size);
				strncpy(dummy,returnString,4);
				if(strcmp(dummy,msgHeader) != 0)
				{
				   racal_log_message( 2, 2,"ARQC Cmd header mis-match.","racal_Verify_ARQC_for_AMEX_Keyblock",1 );
				   return(TRANSACTION_NOT_VERIFIED);
				}
				/*parse the message check for the response code.*/
				memset(&dummy,0,Dummy_Size);
				strncpy(dummy,returnString + 4, 2);

				if(strcmp(dummy,"KR") !=0)
				{
				   racal_log_message( 2, 2,"ARQC Cmd Code mis-match.","racal_Verify_ARQC_for_AMEX_Keyblock",1 );
				   return(TRANSACTION_NOT_VERIFIED);
				}

				/*parse the message check for the error code.*/
				memset(&dummy,0,Dummy_Size);
				strncpy(dummy,returnString + 6, 2);

				  //if this value is not 00 reject the transaction.
				/* '00' : No error
				'01' : ARQC/TC/AAC verification failed
				'04' : Mode Flag not 0, 1 or 2
				'05' : Unrecognised Scheme ID
				'10' : MK parity error
				'68' : Command disabled
				*/
				if(strcmp(dummy,"00") != 0)
				{
				   if(strcmp(dummy,"01") == 0)
					{
					   racal_log_message( 2, 2,"ARPC AAC validation failed.","racal_Verify_ARQC_for_AMEX_Keyblock",1 );
				   }
				   if(strcmp(dummy,"04") == 0)
					  racal_log_message( 2, 2,"Mode flag not 0 1 or 2.","racal_Verify_ARQC_for_AMEX_Keyblock",1 );
				   if(strcmp(dummy,"05") == 0)
					  racal_log_message( 2, 2,"Unrecognised Scheme ID.","racal_Verify_ARQC_for_AMEX_Keyblock",1 );
				   if(strcmp(dummy,"10") == 0)
					  racal_log_message( 2, 2,"MK parity error.","racal_Verify_ARQC_for_AMEX_Keyblock",1 );
				   if(strcmp(dummy,"68") == 0)
					  racal_log_message( 2, 2,"Command disabled.","racal_Verify_ARQC_for_AMEX_Keyblock",1 );
					 return(TRANSACTION_NOT_VERIFIED);
				}

				CHAR temp_response_tag[35]={0};
				memcpy(dummy,returnString + 8, 8);		 // This is the ARPC value.
				 if (strcmp(auth_tx.BIN01_details.card_family,CARD_MASTERCARD) == 0)
				 {
					 dummy[8]=0x00;
					 dummy[9]=0x10;
				 }
				 else  if (strcmp(auth_tx.BIN01_details.card_family,CARD_VISA) == 0 ||
				 	strcmp(auth_tx.BIN01_details.card_family,CARD_AMEX) == 0)
				 {
					 dummy[8]=0x30;
					 dummy[9]=0x30;
				 }
				 // Prepare EMV block for response
				 strcpy(ARPC_RAW,"91");
				 strcat(ARPC_RAW,"0A");

					if(0 == strcmp(auth_tx.TLF01_details.handler_queue,"dcpisoA") )
					{
						auth_tx.EMV_details.future_field1[0]=0x00;
						//int resp_dcp_len =12;
						auth_tx.EMV_details.future_field1[1]=0x12;
						strcpy(temp_response_tag,ARPC_RAW);
						genutil_asc_to_bin( temp_response_tag,temp_txt,&bin_len);
						memcpy(&auth_tx.EMV_details.future_field1[2],temp_txt,2);
						memcpy(&auth_tx.EMV_details.future_field1[4],dummy,10);
					}
					else
					{
						 if (strcmp(auth_tx.BIN01_details.card_family,CARD_MASTERCARD) == 0)
						 {	sprintf(auth_tx.EMV_details.future_field1,"%03d",12);
							strcpy(temp_response_tag,ARPC_RAW);
							genutil_asc_to_bin( temp_response_tag,temp_txt,&bin_len);
							memcpy(&auth_tx.EMV_details.future_field1[3],temp_txt,2);
							memcpy(&auth_tx.EMV_details.future_field1[5],dummy,10);
						 }
						 else  if (strcmp(auth_tx.BIN01_details.card_family,CARD_VISA) == 0)
						 {
							auth_tx.EMV_details.future_field1[0]=0x0F;
							 strcpy(temp_response_tag,"01000C");
							 strcat(temp_response_tag,ARPC_RAW);
							genutil_asc_to_bin( temp_response_tag,temp_txt,&bin_len);
							memcpy(&auth_tx.EMV_details.future_field1[1],temp_txt,5);
							memcpy(&auth_tx.EMV_details.future_field1[6],dummy,10);
						 }else if (strcmp(auth_tx.BIN01_details.card_family,CARD_AMEX) == 0)
						 {
							auth_tx.EMV_details.future_field1[0]=0x0A;
							memcpy(&auth_tx.EMV_details.future_field1[1],dummy,10);

						 }

					}

		   }

	  return (TRANSACTION_VERIFIED);
}/*racal_Verify_ARQC_for_AMEX_Keyblock*/

/**************************************************************/
int racal_Verify_CAVV_for_3Dsecure_MC_Keyblock()
{
   char msgHeader[5] = {0};
   char cmdCode[3] = {0};
   char cvv_value[5] = {0};
   char cavv2_atn_Code[5] = {0};
   char delimiter[2] = {0};
   char output[200]= {0};
   char cardType = 'Z';
   char csc[6] = {0};

   INT  de48_retcode= 0;
   INT  ucaf_data_64_len =0;
   BYTE HEX_ATN_value[9]={0};
   BYTE UCAF_data_ascii[41]={0};
   BYTE UCAF_data_bin[41]={0};
   INT  ucaf_data_len=0;
   INT  dcp_len = 0;
   int  cnt=0;
   int  result=0;
   int  jcnt=0;
   char BUFFER [256] = {0};
   BYTE temp_len[4] = {0};
   CHAR *tempptr;
   CHAR fieldSeperator[2] = {0};
   BYTE UCAF_data[33]={0};
   int  len = 0;
   int  atn_int = 0;
   int  astr_len = 0;
   BYTE ascii_atn[9]={0};

   memset (&msgHeader,0,sizeof(msgHeader));
   memset (&cmdCode,0,sizeof(cmdCode));
   memset (&delimiter,0,sizeof(delimiter));
   memset (&output,0,sizeof(output));
   memset (&returnString,0,ReturnString_Size);
   memset (&csc,0,sizeof(csc));
   /* For DCP UCAF data */

   /* ENDS */
   strcpy (msgHeader ,"1234");

   if (strcmp(auth_tx.BIN01_details.card_family,CARD_VISA) == 0)
      cardType = 'V';

   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_MASTERCARD) == 0)
      cardType = 'M';

   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_AMEX) == 0)
      cardType = 'A';

   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_JCB) == 0)
      cardType = 'J';
   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_CUP) == 0)
        cardType = 'C';
   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_DINERS) == 0)
           cardType = 'D';

   switch(cardType)
   {
      case 'A':

 			 racal_log_message( 2, 2,"AMEX card - 3D secure code not implemented.","racal_Verify_CAVV_for_3Dsecure_MC_Keyblock",1 );
			 return(TRANSACTION_NOT_VERIFIED);

         /****************************end AMEX***************************************/

         break;

      case 'J':
         /*drop down to VISA for this function.*/
         /*break;*/

      case 'M':

         /*drop down to VISA for this function.*/
         /*break;*/
      case 'C':

		 /*drop down to VISA for this function.*/
		 /*break;*/
      case 'D':

		 /*drop down to VISA for this function.*/
		 /*break;*/
      case 'V':

         /*****************************begin Visa***************************************/
         /*The VISA, MC, and the JCB are all the same.*/
         /*The rest of this code would deal with them.*/
         strcpy (cmdCode,"CY");
         strcpy (delimiter,";");

         /*edit CVK A */
         if(strlen(auth_tx.CAF01_details.cvk[4].cvkA) == 0)
		 {
            racal_log_message( 2, 2,"CAVV-validation: CAK is blank.","racal_Verify_CAVV_for_3Dsecure_MC_Keyblock",1 );
            return(TRANSACTION_NOT_VERIFIED);
         }
         /*edit CVK B */
         if(strlen(auth_tx.CAF01_details.cvk[4].cvkB) == 0)
         {
            racal_log_message( 2, 2,"CAVV-validation: CAK is blank.","racal_Verify_CAVV_for_3Dsecure_MC_Keyblock",1 );
            return(TRANSACTION_NOT_VERIFIED);
         }

		 /*edit card number*/
	 	 if(strlen(auth_tx.TLF01_details.card_num) == 0 )
         {
            racal_log_message( 2, 2,"CAVV-validation: card_num is blank.","racal_Verify_CAVV_for_3Dsecure_MC_Keyblock",1 );
            return(TRANSACTION_NOT_VERIFIED);
         }

		  /*Get DE126.9 CAVV from Message*/
        if(cardType == 'V' || cardType == 'J')
		{
			if(strlen(auth_tx.TLF01_details.dcf01_retired_cwk) != 0  && strcmp(auth_tx.TLF01_details.nfi_end_date,"1" == 0))
			 {
				 strncpy(cvv_value,&auth_tx.TLF01_details.dcf01_retired_cwk[6],4);
				 strncpy(cavv2_atn_Code,&auth_tx.TLF01_details.dcf01_retired_cwk[10],4);
				 if(cardType == 'J')
				 {
					 /* From JCB CAVV value will be sent 40, which is assumend as spaces,
					  * hence check the raw data which stored in auth_tx.acct_id_2 without converting.
					  * position 3 and 4 contains CVV verifiation value(4 bytes after convertion)
					  * position 5 and 6 contains unpredictable number(4 bytes after convertion)
					  * 40 binary value is viewd as @, hence compare with it to send spaces.
					  * */
					 if(0 == strncmp(&auth_tx.acct_id_2[3],"@@",2))
					 {
						 memcpy(cvv_value,"    ",4);
					 }
					 if(0 == strncmp(&auth_tx.acct_id_2[5],"@@",2))
					 {
						 memcpy(cavv2_atn_Code,"    ",4);
					 }
				 }
			 }		 /*If neither cvc1 or cvc2 is present, return no value*/
			 else if(strlen(auth_tx.TLF01_details.dcf01_retired_cwk) == 0)
			 {
				racal_log_message( 2, 2,"CAVV-validation: CAVV data 126.9 is blank.","racal_Verify_CAVV_for_3Dsecure_MC_Keyblock",1 );
				return(TRANSACTION_NOT_VERIFIED);
			 }
		}
        else if(cardType == 'M')
		{

			if (strncmp(auth_tx.TLF01_details.handler_queue,"dcp",3) == 0)
			{
				/* 3D secure data structre of AsiaPay transaction:
				 *
				 * Total length : 2 bytes : N 4
				 * Actual data length: 2 bytes : N 4
				 * Table ID (MC ECOM indicator value): 2 bytes : N 4
				 * MC merchant/ CH certificate serial number :  2 bytes length (N 4)+ 40 bytes (N 80)
				 * MC security level indicator : 2 bytes length (N 4)+ 7 bytes (N 14)
				 * MC UCAF data length : 2 bytes length (N 4)+ 28 bytes (N 56)
				 *
				 * DCPISO will send data in auth_tx.int_net_facility_data, Data after table ID will be
				 * received by racal.
				 *
				 * After the conversion  UCAF data will be populated as follows for both
				 * incoming as well as Asiapay
				 *
				 * Total 40 Bytes:
				 *	Bit 1 & 2   cByte 8C = Auth Success 86 = Auth Attempt
				 *	Bit 3 - 18  Merchant Name First 16 digits Hexa format of SHA Hased Merchant Name
				 *	Bit 19 & 20 08 for ACS
				 *	Bit 21 - 22 Authentication Method
				 * 		Indates how the cardholder was authenticated to the ACS;
				 *  	0 = No cardholder authentication performed (only valid for AAV with X'86' - Attempts Processin
				 *  	1 = Password
				 *  	2 = Secret Key (e.g. Chip Card) 3 = PKI (pending further discussions)
				 *  	01 = Auth Attempt
				 *  	11 = Others
				 *  Bit 23 - 30 ATN  Last 8 digits of AcctID
				 *  Bit 31 - 34 CVV for Authentication Leading zero filled CVV
				 *  Bit 35 - 40 Filler Zero fill
				 * */
				if(strlen(auth_tx.int_net_facility_len) !=0)
				 {
					dcp_len = genutil_bcd_to_int (&auth_tx.int_net_facility_data[DCP_UCAF_LEN_POS], 2);
					memcpy(UCAF_data,&auth_tx.int_net_facility_data[DCP_UCAF_DATA_POS],dcp_len);
					ucaf_data_64_len =strlen(UCAF_data);
					ucaf_data_len = genutil_base64_to_binary( UCAF_data, strlen(UCAF_data),&UCAF_data_bin );
					genutil_bin_to_ascii (UCAF_data_bin, ucaf_data_len, UCAF_data_ascii, astr_len);
					printf("UCAF dat: %s, len %d \n",UCAF_data_ascii,astr_len);
					strncpy(cvv_value,&UCAF_data_ascii[30],4);
					strncpy(HEX_ATN_value,&UCAF_data_ascii[22],8);

					atn_int =  genutil_hex_to_int(HEX_ATN_value,8);
					sprintf(ascii_atn,"%010u",atn_int);
					strncpy(cavv2_atn_Code,&ascii_atn[6],4);
				 }		 /*If neither cvc1 or cvc2 is present, return no value*/
				 else if(strlen(auth_tx.int_net_facility_len) == 0)
				 {
					racal_log_message( 2, 2,"CAVV-validation: CAVV data is blank.","racal_Verify_CAVV_for_3Dsecure_MC_Keyblock",1 );
					return(TRANSACTION_NOT_VERIFIED);
				 }
			}
			else
			{
				if(strlen(auth_tx.EMV_details.issuer_script_2) !=0)
				 {
					de48_retcode = parse_field_48( strlen(auth_tx.EMV_details.issuer_script_2),&auth_tx.EMV_details.issuer_script_2,&UCAF_data );
					ucaf_data_64_len =strlen(UCAF_data);
					ucaf_data_len = genutil_base64_to_binary( UCAF_data, strlen(UCAF_data),&UCAF_data_bin );
					genutil_bin_to_ascii (UCAF_data_bin, ucaf_data_len, UCAF_data_ascii, astr_len);
					printf("UCAF dat: %s, len %d \n",UCAF_data_ascii,astr_len);
					strncpy(cvv_value,&UCAF_data_ascii[30],4);
					strncpy(HEX_ATN_value,&UCAF_data_ascii[22],8);

					atn_int =  genutil_hex_to_int(HEX_ATN_value,8);
					sprintf(ascii_atn,"%010u",atn_int);
					strncpy(cavv2_atn_Code,&ascii_atn[6],4);
				 }		 /*If neither cvc1 or cvc2 is present, return no value*/
				 else if(strlen(auth_tx.EMV_details.issuer_script_2) == 0)
				 {
					racal_log_message( 2, 2,"CAVV-validation: CAVV data is blank.","racal_Verify_CAVV_for_3Dsecure_MC_Keyblock",1 );
					return(TRANSACTION_NOT_VERIFIED);
				 }
			}


		}
        else if(cardType == 'D')
	    {
        	if(0 == strncmp(auth_tx.TLF01_details.handler_queue,
        					"dcp",3))
        	{
        		//Asiapay 3D validation.
        		strncpy(cvv_value,
        				auth_tx.TLF01_details.cvc_data,
        				DINERS_DE122_CAVV_LEN);
				strncpy(cavv2_atn_Code,
						auth_tx.TLF01_details.exp_date,
						DINERS_DE14_SIZE);

        	}
        	else if(0 == strncmp(auth_tx.TLF01_details.handler_queue,
        			              "ncdci",5))
        	{
        		if(strlen(auth_tx.future_use_4)> DINERS_DE122_AUTH_TRACKING_NUM_POS)
        		{
				 // Lets retrive CVVV value present in DE122
					strncpy(cvv_value,
							auth_tx.future_use_4 + DINERS_DE122_CAVV_POS,
							DINERS_DE122_CAVV_LEN);
					strncpy(cavv2_atn_Code,
							auth_tx.future_use_4 + DINERS_DE122_UNPRIDICATABLE_NUM_POS,
							DINERS_DE122_UNPRIDICATABLE_NUM_LEN);
				}
        	}
	   }

		 /*Build message and send to Racal.  */
		 memset (&output,0,sizeof(output));
		 strcat(output, msgHeader);
		 strcat(output, cmdCode);
		 if( 'D' ==  cardType &&
			(auth_tx.CAF01_details.aba_nbr[1] == '1' || auth_tx.CAF01_details.aba_nbr[1] == 1))
		 { /* ticked: single length key validation*/
			 // Don't send U for single length validation
		 }
		 else
		 {/* un-ticked: double length key validation*/
		 strcat(output, "U");
		 }
		 strcat(output, auth_tx.CAF01_details.cvk[4].cvkA);
		 strcat(output, auth_tx.CAF01_details.cvk[4].cvkB);

		 if('D' ==  cardType)
		 {
			 if(0 == strncmp(auth_tx.TLF01_details.handler_queue,
 					"dcp",3))
			 {
				 strncat(output, &cvv_value[1],3);
			 }
			 else
			 {
				 strncat(output, cvv_value,3);
			 }
		 }
		 else
		 {
		 strncat(output, &cvv_value[1],3);
		 }

		 strcat(output, auth_tx.TLF01_details.card_num);
		 strcat(output, delimiter);
		 strcat(output, cavv2_atn_Code);

		 if (strcmp(auth_tx.BIN01_details.card_family,CARD_MASTERCARD) == 0)
		 {
			 strcat(output, "140");
		 }
		 else if(strcmp(auth_tx.BIN01_details.card_family,CARD_JCB) == 0)
		 {
			 if(0 == strncmp(auth_tx.TLF01_details.dcf01_retired_cwk,"07",2))
			 {
				 strcat(output, "700");
			 }
			 else
			 {
				 strcat(output, "000");
			 }

		 }
		 else
		 {
			 strcat(output, "000");
		 }


	 //if the output is bigger then 150 do not send it out.
	 if (strlen(output) > 150)
	 {
		racal_log_message( 2, 2,"CAVV Cmd Msg too long.","racalVerify_CAVV_for_3Dsecure",1 );
		return(TRANSACTION_NOT_VERIFIED);
	 }

	 memset(BUFFER,0,sizeof(BUFFER));
	 sprintf(BUFFER, "BEFORE-PORT: %s", output);
	 MYPRINT(BUFFER);

	 /*The message is built.  Send it over to the Racal box.*/
	 /*Once the a response is recieved, check for the message header,*/
	 /*response code, and the error code.  If it does not exactly match,*/
	 /*send TRANSACTION_NOT_VERIFIED back.*/
	 if (racalSendToPort(output) == TRANSACTION_VERIFIED)
	 {
		/*parse the message check for the message header.*/
		memset(&dummy,0,Dummy_Size);
		strncpy(dummy,returnString,4);
		if(strcmp(dummy,msgHeader) != 0)
	{
		   racal_log_message( 2, 2,"CAVV Msg Header mis-match.","racal_Verify_CAVV_for_3Dsecure_MC_Keyblock",1 );
		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
	}

		/*parse the message check for the response code.*/
		memset(&dummy,0,Dummy_Size);
		strncpy(dummy,returnString + 4, 2);
		if(strcmp(dummy,"CZ") !=0)
		{
		   racal_log_message( 2, 2,"CAVV Cmd Msg mis-match.","racal_Verify_CAVV_for_3Dsecure_MC_Keyblock",1 );
		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
		}

	/* Parse the message check for the error code. */
	memset(&dummy,0,Dummy_Size);
	strncpy(dummy,returnString + 6, 2);
	if(strcmp(dummy,"00") == 0)
	{
		  return (TRANSACTION_VERIFIED); /*cavv valid*/
	}

		if(strcmp(dummy,"01") == 0)
		{
		   racal_log_message( 2, 2,"CAVV-VERIFY failure.","racal_Verify_CAVV_for_3Dsecure_MC_Keyblock",1 );
		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
		}
		if(strcmp(dummy,"10") == 0)
		{
			racal_log_message( 2, 2,"CAVV-VERIFY CVK A or B parity error.","racal_Verify_CAVV_for_3Dsecure_MC_Keyblock",1 );
			return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
		}

		if(strcmp(dummy,"12") == 0)
		{
		   racal_log_message( 2, 2,"CAVV-VERIFY No Key loaded in user storage.","racal_Verify_CAVV_for_3Dsecure_MC_Keyblock",1 );
		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
		}

		if(strcmp(dummy,"13") == 0)
		{
		   racal_log_message( 2, 2,"CAVV-VERIFY LMK error report to superviser.","racal_Verify_CAVV_for_3Dsecure_MC_Keyblock",1 );
		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
		}

		if(strcmp(dummy,"15") == 0)
		{
		   racal_log_message( 2, 2,"CAVV-VERIFY Error in input data.","racal_Verify_CAVV_for_3Dsecure_MC_Keyblock",1 );
		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
		}

		if(strcmp(dummy,"21") == 0)
		{
		   racal_log_message( 2, 2,"CAVV-VERIFY Invalid user storage index.","racal_Verify_CAVV_for_3Dsecure_MC_Keyblock",1 );
		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
		}

	 }
	 else  /* else for: if (racalSendToPort(output) == TRANSACTION_VERIFIED)*/
	 {
			   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
	 }



 return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
 break;
		         /********************end of Visa Case*****************************************/


   default:
	   {
		   racal_log_message( 2, 2,"Card Type Undefined.","racal_Verify_CAVV_for_3Dsecure_MC_Keyblock",1 );
		   return(TRANSACTION_NOT_VERIFIED);
	   }


   }/*end of switch statement.*/

}/*racal_Verify_CAVV_for_3Dsecure_MC_Keyblock*/
/*******************************************/

/**************************************************************/
int racal_Verify_CAVV_for_3Dsecure_VISA_Keyblock()
{
   char msgHeader[5] = {0};
   char cmdCode[3] = {0};
   char cvv_value[5] = {0};
   char cavv2_atn_Code[5] = {0};
   char delimiter[2] = {0};
   char output[200]= {0};
   char cardType = 'Z';
   char csc[6] = {0};

   INT  de48_retcode= 0;
   INT  ucaf_data_64_len =0;
   BYTE HEX_ATN_value[9]={0};
   BYTE UCAF_data_ascii[41]={0};
   BYTE UCAF_data_bin[41]={0};
   INT  ucaf_data_len=0;
   INT  dcp_len = 0;
   int  cnt=0;
   int  result=0;
   int  jcnt=0;
   char BUFFER [256] = {0};
   BYTE temp_len[4] = {0};
   CHAR *tempptr;
   CHAR fieldSeperator[2] = {0};
   BYTE UCAF_data[33]={0};
   int  len = 0;
   int  atn_int = 0;
   int  astr_len = 0;
   BYTE ascii_atn[9]={0};

   memset (&msgHeader,0,sizeof(msgHeader));
   memset (&cmdCode,0,sizeof(cmdCode));
   memset (&delimiter,0,sizeof(delimiter));
   memset (&output,0,sizeof(output));
   memset (&returnString,0,ReturnString_Size);
   memset (&csc,0,sizeof(csc));
   /* For DCP UCAF data */

   /* ENDS */
   strcpy (msgHeader ,"1234");

   if (strcmp(auth_tx.BIN01_details.card_family,CARD_VISA) == 0)
      cardType = 'V';

   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_MASTERCARD) == 0)
      cardType = 'M';

   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_AMEX) == 0)
      cardType = 'A';

   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_JCB) == 0)
      cardType = 'J';
   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_CUP) == 0)
        cardType = 'C';
   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_DINERS) == 0)
           cardType = 'D';

   switch(cardType)
   {
      case 'A':

 			 racal_log_message( 2, 2,"AMEX card - 3D secure code not implemented.","racal_Verify_CAVV_for_3Dsecure_VISA_Keyblock",1 );
			 return(TRANSACTION_NOT_VERIFIED);

         /****************************end AMEX***************************************/

         break;

      case 'J':
         /*drop down to VISA for this function.*/
         /*break;*/

      case 'M':

         /*drop down to VISA for this function.*/
         /*break;*/
      case 'C':

		 /*drop down to VISA for this function.*/
		 /*break;*/
      case 'D':

		 /*drop down to VISA for this function.*/
		 /*break;*/
      case 'V':

         /*****************************begin Visa***************************************/
         /*The VISA, MC, and the JCB are all the same.*/
         /*The rest of this code would deal with them.*/
         strcpy (cmdCode,"CY");
         strcpy (delimiter,";");

         /*edit CVK A */
         if(strlen(auth_tx.CAF01_details.cvk[4].cvkA) == 0)
		 {
            racal_log_message( 2, 2,"CAVV-validation: CAK is blank.","racal_Verify_CAVV_for_3Dsecure_VISA_Keyblock",1 );
            return(TRANSACTION_NOT_VERIFIED);
         }
         /*edit CVK B */
         if(strlen(auth_tx.CAF01_details.cvk[4].cvkB) == 0)
         {
            racal_log_message( 2, 2,"CAVV-validation: CAK is blank.","racal_Verify_CAVV_for_3Dsecure_VISA_Keyblock",1 );
            return(TRANSACTION_NOT_VERIFIED);
         }

		 /*edit card number*/
	 	 if(strlen(auth_tx.TLF01_details.card_num) == 0 )
         {
            racal_log_message( 2, 2,"CAVV-validation: card_num is blank.","racal_Verify_CAVV_for_3Dsecure_VISA_Keyblock",1 );
            return(TRANSACTION_NOT_VERIFIED);
         }

		  /*Get DE126.9 CAVV from Message*/
        if(cardType == 'V' || cardType == 'J')
		{
			if(strlen(auth_tx.TLF01_details.dcf01_retired_cwk) != 0  && strcmp(auth_tx.TLF01_details.nfi_end_date,"1" == 0))
			 {
				 strncpy(cvv_value,&auth_tx.TLF01_details.dcf01_retired_cwk[6],4);
				 strncpy(cavv2_atn_Code,&auth_tx.TLF01_details.dcf01_retired_cwk[10],4);
				 if(cardType == 'J')
				 {
					 /* From JCB CAVV value will be sent 40, which is assumend as spaces,
					  * hence check the raw data which stored in auth_tx.acct_id_2 without converting.
					  * position 3 and 4 contains CVV verifiation value(4 bytes after convertion)
					  * position 5 and 6 contains unpredictable number(4 bytes after convertion)
					  * 40 binary value is viewd as @, hence compare with it to send spaces.
					  * */
					 if(0 == strncmp(&auth_tx.acct_id_2[3],"@@",2))
					 {
						 memcpy(cvv_value,"    ",4);
					 }
					 if(0 == strncmp(&auth_tx.acct_id_2[5],"@@",2))
					 {
						 memcpy(cavv2_atn_Code,"    ",4);
					 }
				 }
			 }		 /*If neither cvc1 or cvc2 is present, return no value*/
			 else if(strlen(auth_tx.TLF01_details.dcf01_retired_cwk) == 0)
			 {
				racal_log_message( 2, 2,"CAVV-validation: CAVV data 126.9 is blank.","racal_Verify_CAVV_for_3Dsecure_VISA_Keyblock",1 );
				return(TRANSACTION_NOT_VERIFIED);
			 }
		}
        else if(cardType == 'M')
		{

			if (strncmp(auth_tx.TLF01_details.handler_queue,"dcp",3) == 0)
			{
				/* 3D secure data structre of AsiaPay transaction:
				 *
				 * Total length : 2 bytes : N 4
				 * Actual data length: 2 bytes : N 4
				 * Table ID (MC ECOM indicator value): 2 bytes : N 4
				 * MC merchant/ CH certificate serial number :  2 bytes length (N 4)+ 40 bytes (N 80)
				 * MC security level indicator : 2 bytes length (N 4)+ 7 bytes (N 14)
				 * MC UCAF data length : 2 bytes length (N 4)+ 28 bytes (N 56)
				 *
				 * DCPISO will send data in auth_tx.int_net_facility_data, Data after table ID will be
				 * received by racal.
				 *
				 * After the conversion  UCAF data will be populated as follows for both
				 * incoming as well as Asiapay
				 *
				 * Total 40 Bytes:
				 *	Bit 1 & 2   cByte 8C = Auth Success 86 = Auth Attempt
				 *	Bit 3 - 18  Merchant Name First 16 digits Hexa format of SHA Hased Merchant Name
				 *	Bit 19 & 20 08 for ACS
				 *	Bit 21 - 22 Authentication Method
				 * 		Indates how the cardholder was authenticated to the ACS;
				 *  	0 = No cardholder authentication performed (only valid for AAV with X'86' - Attempts Processin
				 *  	1 = Password
				 *  	2 = Secret Key (e.g. Chip Card) 3 = PKI (pending further discussions)
				 *  	01 = Auth Attempt
				 *  	11 = Others
				 *  Bit 23 - 30 ATN  Last 8 digits of AcctID
				 *  Bit 31 - 34 CVV for Authentication Leading zero filled CVV
				 *  Bit 35 - 40 Filler Zero fill
				 * */
				if(strlen(auth_tx.int_net_facility_len) !=0)
				 {
					dcp_len = genutil_bcd_to_int (&auth_tx.int_net_facility_data[DCP_UCAF_LEN_POS], 2);
					memcpy(UCAF_data,&auth_tx.int_net_facility_data[DCP_UCAF_DATA_POS],dcp_len);
					ucaf_data_64_len =strlen(UCAF_data);
					ucaf_data_len = genutil_base64_to_binary( UCAF_data, strlen(UCAF_data),&UCAF_data_bin );
					genutil_bin_to_ascii (UCAF_data_bin, ucaf_data_len, UCAF_data_ascii, astr_len);
					printf("UCAF dat: %s, len %d \n",UCAF_data_ascii,astr_len);
					strncpy(cvv_value,&UCAF_data_ascii[30],4);
					strncpy(HEX_ATN_value,&UCAF_data_ascii[22],8);

					atn_int =  genutil_hex_to_int(HEX_ATN_value,8);
					sprintf(ascii_atn,"%010u",atn_int);
					strncpy(cavv2_atn_Code,&ascii_atn[6],4);
				 }		 /*If neither cvc1 or cvc2 is present, return no value*/
				 else if(strlen(auth_tx.int_net_facility_len) == 0)
				 {
					racal_log_message( 2, 2,"CAVV-validation: CAVV data is blank.","racal_Verify_CAVV_for_3Dsecure_VISA_Keyblock",1 );
					return(TRANSACTION_NOT_VERIFIED);
				 }
			}
			else
			{
				if(strlen(auth_tx.EMV_details.issuer_script_2) !=0)
				 {
					de48_retcode = parse_field_48( strlen(auth_tx.EMV_details.issuer_script_2),&auth_tx.EMV_details.issuer_script_2,&UCAF_data );
					ucaf_data_64_len =strlen(UCAF_data);
					ucaf_data_len = genutil_base64_to_binary( UCAF_data, strlen(UCAF_data),&UCAF_data_bin );
					genutil_bin_to_ascii (UCAF_data_bin, ucaf_data_len, UCAF_data_ascii, astr_len);
					printf("UCAF dat: %s, len %d \n",UCAF_data_ascii,astr_len);
					strncpy(cvv_value,&UCAF_data_ascii[30],4);
					strncpy(HEX_ATN_value,&UCAF_data_ascii[22],8);

					atn_int =  genutil_hex_to_int(HEX_ATN_value,8);
					sprintf(ascii_atn,"%010u",atn_int);
					strncpy(cavv2_atn_Code,&ascii_atn[6],4);
				 }		 /*If neither cvc1 or cvc2 is present, return no value*/
				 else if(strlen(auth_tx.EMV_details.issuer_script_2) == 0)
				 {
					racal_log_message( 2, 2,"CAVV-validation: CAVV data is blank.","racal_Verify_CAVV_for_3Dsecure_VISA_Keyblock",1 );
					return(TRANSACTION_NOT_VERIFIED);
				 }
			}


		}
        else if(cardType == 'D')
	    {
        	if(0 == strncmp(auth_tx.TLF01_details.handler_queue,
        					"dcp",3))
        	{
        		//Asiapay 3D validation.
        		strncpy(cvv_value,
        				auth_tx.TLF01_details.cvc_data,
        				DINERS_DE122_CAVV_LEN);
				strncpy(cavv2_atn_Code,
						auth_tx.TLF01_details.exp_date,
						DINERS_DE14_SIZE);

        	}
        	else if(0 == strncmp(auth_tx.TLF01_details.handler_queue,
        			              "ncdci",5))
        	{
        		if(strlen(auth_tx.future_use_4)> DINERS_DE122_AUTH_TRACKING_NUM_POS)
        		{
				 // Lets retrive CVVV value present in DE122
					strncpy(cvv_value,
							auth_tx.future_use_4 + DINERS_DE122_CAVV_POS,
							DINERS_DE122_CAVV_LEN);
					strncpy(cavv2_atn_Code,
							auth_tx.future_use_4 + DINERS_DE122_UNPRIDICATABLE_NUM_POS,
							DINERS_DE122_UNPRIDICATABLE_NUM_LEN);
				}
        	}
	   }

		 /*Build message and send to Racal.  */
		 memset (&output,0,sizeof(output));
		 strcat(output, msgHeader);
		 strcat(output, cmdCode);
		 if( 'D' ==  cardType &&
			(auth_tx.CAF01_details.aba_nbr[1] == '1' || auth_tx.CAF01_details.aba_nbr[1] == 1))
		 { /* ticked: single length key validation*/
			 // Don't send U for single length validation
		 }
		 else
		 {/* un-ticked: double length key validation*/
		 strcat(output, "U");
		 }
		 strcat(output, auth_tx.CAF01_details.cvk[4].cvkA);
		 strcat(output, auth_tx.CAF01_details.cvk[4].cvkB);

		 if('D' ==  cardType)
		 {
			 if(0 == strncmp(auth_tx.TLF01_details.handler_queue,
 					"dcp",3))
			 {
				 strncat(output, &cvv_value[1],3);
			 }
			 else
			 {
				 strncat(output, cvv_value,3);
			 }
		 }
		 else
		 {
		 strncat(output, &cvv_value[1],3);
		 }

		 strcat(output, auth_tx.TLF01_details.card_num);
		 strcat(output, delimiter);
		 strcat(output, cavv2_atn_Code);

		 if (strcmp(auth_tx.BIN01_details.card_family,CARD_MASTERCARD) == 0)
		 {
			 strcat(output, "140");
		 }
		 else if(strcmp(auth_tx.BIN01_details.card_family,CARD_JCB) == 0)
		 {
			 if(0 == strncmp(auth_tx.TLF01_details.dcf01_retired_cwk,"07",2))
			 {
				 strcat(output, "700");
			 }
			 else
			 {
				 strcat(output, "000");
			 }

		 }
		 else
		 {
			 strcat(output, "000");
		 }


	 //if the output is bigger then 150 do not send it out.
	 if (strlen(output) > 150)
	 {
		racal_log_message( 2, 2,"CAVV Cmd Msg too long.","racal_Verify_CAVV_for_3Dsecure_VISA_Keyblock",1 );
		return(TRANSACTION_NOT_VERIFIED);
	 }

	 memset(BUFFER,0,sizeof(BUFFER));
	 sprintf(BUFFER, "BEFORE-PORT: %s", output);
	 MYPRINT(BUFFER);

	 /*The message is built.  Send it over to the Racal box.*/
	 /*Once the a response is recieved, check for the message header,*/
	 /*response code, and the error code.  If it does not exactly match,*/
	 /*send TRANSACTION_NOT_VERIFIED back.*/
	 if (racalSendToPort(output) == TRANSACTION_VERIFIED)
	 {
		/*parse the message check for the message header.*/
		memset(&dummy,0,Dummy_Size);
		strncpy(dummy,returnString,4);
		if(strcmp(dummy,msgHeader) != 0)
	{
		   racal_log_message( 2, 2,"CAVV Msg Header mis-match.","racal_Verify_CAVV_for_3Dsecure_VISA_Keyblock",1 );
		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
	}

		/*parse the message check for the response code.*/
		memset(&dummy,0,Dummy_Size);
		strncpy(dummy,returnString + 4, 2);
		if(strcmp(dummy,"CZ") !=0)
		{
		   racal_log_message( 2, 2,"CAVV Cmd Msg mis-match.","racal_Verify_CAVV_for_3Dsecure_VISA_Keyblock",1 );
		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
		}

	/* Parse the message check for the error code. */
	memset(&dummy,0,Dummy_Size);
	strncpy(dummy,returnString + 6, 2);
	if(strcmp(dummy,"00") == 0)
	{
		  return (TRANSACTION_VERIFIED); /*cavv valid*/
	}

		if(strcmp(dummy,"01") == 0)
		{
		   racal_log_message( 2, 2,"CAVV-VERIFY failure.","racal_Verify_CAVV_for_3Dsecure_VISA_Keyblock",1 );
		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
		}
		if(strcmp(dummy,"10") == 0)
		{
			racal_log_message( 2, 2,"CAVV-VERIFY CVK A or B parity error.","racal_Verify_CAVV_for_3Dsecure_VISA_Keyblock",1 );
			return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
		}

		if(strcmp(dummy,"12") == 0)
		{
		   racal_log_message( 2, 2,"CAVV-VERIFY No Key loaded in user storage.","racal_Verify_CAVV_for_3Dsecure_VISA_Keyblock",1 );
		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
		}

		if(strcmp(dummy,"13") == 0)
		{
		   racal_log_message( 2, 2,"CAVV-VERIFY LMK error report to superviser.","racal_Verify_CAVV_for_3Dsecure_VISA_Keyblock",1 );
		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
		}

		if(strcmp(dummy,"15") == 0)
		{
		   racal_log_message( 2, 2,"CAVV-VERIFY Error in input data.","racal_Verify_CAVV_for_3Dsecure_VISA_Keyblock",1 );
		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
		}

		if(strcmp(dummy,"21") == 0)
		{
		   racal_log_message( 2, 2,"CAVV-VERIFY Invalid user storage index.","racal_Verify_CAVV_for_3Dsecure_VISA_Keyblock",1 );
		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
		}

	 }
	 else  /* else for: if (racalSendToPort(output) == TRANSACTION_VERIFIED)*/
	 {
			   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
	 }



 return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
 break;
		         /********************end of Visa Case*****************************************/


   default:
	   {
		   racal_log_message( 2, 2,"Card Type Undefined.","racal_Verify_CAVV_for_3Dsecure_VISA_Keyblock",1 );
		   return(TRANSACTION_NOT_VERIFIED);
	   }


   }/*end of switch statement.*/

}/*racal_Verify_CAVV_for_3Dsecure_VISA_Keyblock*/
/*******************************************/


/**************************************************************/
int racal_Verify_CAVV_for_3Dsecure_AMEX_Keyblock()
{
   char msgHeader[5] = {0};
   char cmdCode[3] = {0};
   char cvv_value[5] = {0};
   char cavv2_atn_Code[5] = {0};
   char delimiter[2] = {0};
   char output[200]= {0};
   char cardType = 'Z';
   char csc[6] = {0};

   INT  de48_retcode= 0;
   INT  ucaf_data_64_len =0;
   BYTE HEX_ATN_value[9]={0};
   BYTE UCAF_data_ascii[41]={0};
   BYTE UCAF_data_bin[41]={0};
   INT  ucaf_data_len=0;
   INT  dcp_len = 0;
   int  cnt=0;
   int  result=0;
   int  jcnt=0;
   char BUFFER [256] = {0};
   BYTE temp_len[4] = {0};
   CHAR *tempptr;
   CHAR fieldSeperator[2] = {0};
   BYTE UCAF_data[33]={0};
   int  len = 0;
   int  atn_int = 0;
   int  astr_len = 0;
   BYTE ascii_atn[9]={0};

   memset (&msgHeader,0,sizeof(msgHeader));
   memset (&cmdCode,0,sizeof(cmdCode));
   memset (&delimiter,0,sizeof(delimiter));
   memset (&output,0,sizeof(output));
   memset (&returnString,0,ReturnString_Size);
   memset (&csc,0,sizeof(csc));
   /* For DCP UCAF data */

   /* ENDS */
   strcpy (msgHeader ,"1234");

   if (strcmp(auth_tx.BIN01_details.card_family,CARD_VISA) == 0)
      cardType = 'V';

   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_MASTERCARD) == 0)
      cardType = 'M';

   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_AMEX) == 0)
      cardType = 'A';

   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_JCB) == 0)
      cardType = 'J';
   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_CUP) == 0)
        cardType = 'C';
   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_DINERS) == 0)
           cardType = 'D';

   switch(cardType)
   {
      case 'A':

 			 racal_log_message( 2, 2,"AMEX card - 3D secure code not implemented.","racal_Verify_CAVV_for_3Dsecure_AMEX_Keyblock",1 );
			 return(TRANSACTION_NOT_VERIFIED);

         /****************************end AMEX***************************************/

         break;

      case 'J':
         /*drop down to VISA for this function.*/
         /*break;*/

      case 'M':

         /*drop down to VISA for this function.*/
         /*break;*/
      case 'C':

		 /*drop down to VISA for this function.*/
		 /*break;*/
      case 'D':

		 /*drop down to VISA for this function.*/
		 /*break;*/
      case 'V':

         /*****************************begin Visa***************************************/
         /*The VISA, MC, and the JCB are all the same.*/
         /*The rest of this code would deal with them.*/
         strcpy (cmdCode,"CY");
         strcpy (delimiter,";");

         /*edit CVK A */
         if(strlen(auth_tx.CAF01_details.cvk[4].cvkA) == 0)
		 {
            racal_log_message( 2, 2,"CAVV-validation: CAK is blank.","racal_Verify_CAVV_for_3Dsecure_AMEX_Keyblock",1 );
            return(TRANSACTION_NOT_VERIFIED);
         }
         /*edit CVK B */
         if(strlen(auth_tx.CAF01_details.cvk[4].cvkB) == 0)
         {
            racal_log_message( 2, 2,"CAVV-validation: CAK is blank.","racal_Verify_CAVV_for_3Dsecure_AMEX_Keyblock",1 );
            return(TRANSACTION_NOT_VERIFIED);
         }

		 /*edit card number*/
	 	 if(strlen(auth_tx.TLF01_details.card_num) == 0 )
         {
            racal_log_message( 2, 2,"CAVV-validation: card_num is blank.","racal_Verify_CAVV_for_3Dsecure_AMEX_Keyblock",1 );
            return(TRANSACTION_NOT_VERIFIED);
         }

		  /*Get DE126.9 CAVV from Message*/
        if(cardType == 'V' || cardType == 'J')
		{
			if(strlen(auth_tx.TLF01_details.dcf01_retired_cwk) != 0  && strcmp(auth_tx.TLF01_details.nfi_end_date,"1" == 0))
			 {
				 strncpy(cvv_value,&auth_tx.TLF01_details.dcf01_retired_cwk[6],4);
				 strncpy(cavv2_atn_Code,&auth_tx.TLF01_details.dcf01_retired_cwk[10],4);
				 if(cardType == 'J')
				 {
					 /* From JCB CAVV value will be sent 40, which is assumend as spaces,
					  * hence check the raw data which stored in auth_tx.acct_id_2 without converting.
					  * position 3 and 4 contains CVV verifiation value(4 bytes after convertion)
					  * position 5 and 6 contains unpredictable number(4 bytes after convertion)
					  * 40 binary value is viewd as @, hence compare with it to send spaces.
					  * */
					 if(0 == strncmp(&auth_tx.acct_id_2[3],"@@",2))
					 {
						 memcpy(cvv_value,"    ",4);
					 }
					 if(0 == strncmp(&auth_tx.acct_id_2[5],"@@",2))
					 {
						 memcpy(cavv2_atn_Code,"    ",4);
					 }
				 }
			 }		 /*If neither cvc1 or cvc2 is present, return no value*/
			 else if(strlen(auth_tx.TLF01_details.dcf01_retired_cwk) == 0)
			 {
				racal_log_message( 2, 2,"CAVV-validation: CAVV data 126.9 is blank.","racal_Verify_CAVV_for_3Dsecure_AMEX_Keyblock",1 );
				return(TRANSACTION_NOT_VERIFIED);
			 }
		}
        else if(cardType == 'M')
		{

			if (strncmp(auth_tx.TLF01_details.handler_queue,"dcp",3) == 0)
			{
				/* 3D secure data structre of AsiaPay transaction:
				 *
				 * Total length : 2 bytes : N 4
				 * Actual data length: 2 bytes : N 4
				 * Table ID (MC ECOM indicator value): 2 bytes : N 4
				 * MC merchant/ CH certificate serial number :  2 bytes length (N 4)+ 40 bytes (N 80)
				 * MC security level indicator : 2 bytes length (N 4)+ 7 bytes (N 14)
				 * MC UCAF data length : 2 bytes length (N 4)+ 28 bytes (N 56)
				 *
				 * DCPISO will send data in auth_tx.int_net_facility_data, Data after table ID will be
				 * received by racal.
				 *
				 * After the conversion  UCAF data will be populated as follows for both
				 * incoming as well as Asiapay
				 *
				 * Total 40 Bytes:
				 *	Bit 1 & 2   cByte 8C = Auth Success 86 = Auth Attempt
				 *	Bit 3 - 18  Merchant Name First 16 digits Hexa format of SHA Hased Merchant Name
				 *	Bit 19 & 20 08 for ACS
				 *	Bit 21 - 22 Authentication Method
				 * 		Indates how the cardholder was authenticated to the ACS;
				 *  	0 = No cardholder authentication performed (only valid for AAV with X'86' - Attempts Processin
				 *  	1 = Password
				 *  	2 = Secret Key (e.g. Chip Card) 3 = PKI (pending further discussions)
				 *  	01 = Auth Attempt
				 *  	11 = Others
				 *  Bit 23 - 30 ATN  Last 8 digits of AcctID
				 *  Bit 31 - 34 CVV for Authentication Leading zero filled CVV
				 *  Bit 35 - 40 Filler Zero fill
				 * */
				if(strlen(auth_tx.int_net_facility_len) !=0)
				 {
					dcp_len = genutil_bcd_to_int (&auth_tx.int_net_facility_data[DCP_UCAF_LEN_POS], 2);
					memcpy(UCAF_data,&auth_tx.int_net_facility_data[DCP_UCAF_DATA_POS],dcp_len);
					ucaf_data_64_len =strlen(UCAF_data);
					ucaf_data_len = genutil_base64_to_binary( UCAF_data, strlen(UCAF_data),&UCAF_data_bin );
					genutil_bin_to_ascii (UCAF_data_bin, ucaf_data_len, UCAF_data_ascii, astr_len);
					printf("UCAF dat: %s, len %d \n",UCAF_data_ascii,astr_len);
					strncpy(cvv_value,&UCAF_data_ascii[30],4);
					strncpy(HEX_ATN_value,&UCAF_data_ascii[22],8);

					atn_int =  genutil_hex_to_int(HEX_ATN_value,8);
					sprintf(ascii_atn,"%010u",atn_int);
					strncpy(cavv2_atn_Code,&ascii_atn[6],4);
				 }		 /*If neither cvc1 or cvc2 is present, return no value*/
				 else if(strlen(auth_tx.int_net_facility_len) == 0)
				 {
					racal_log_message( 2, 2,"CAVV-validation: CAVV data is blank.","racal_Verify_CAVV_for_3Dsecure_AMEX_Keyblock",1 );
					return(TRANSACTION_NOT_VERIFIED);
				 }
			}
			else
			{
				if(strlen(auth_tx.EMV_details.issuer_script_2) !=0)
				 {
					de48_retcode = parse_field_48( strlen(auth_tx.EMV_details.issuer_script_2),&auth_tx.EMV_details.issuer_script_2,&UCAF_data );
					ucaf_data_64_len =strlen(UCAF_data);
					ucaf_data_len = genutil_base64_to_binary( UCAF_data, strlen(UCAF_data),&UCAF_data_bin );
					genutil_bin_to_ascii (UCAF_data_bin, ucaf_data_len, UCAF_data_ascii, astr_len);
					printf("UCAF dat: %s, len %d \n",UCAF_data_ascii,astr_len);
					strncpy(cvv_value,&UCAF_data_ascii[30],4);
					strncpy(HEX_ATN_value,&UCAF_data_ascii[22],8);

					atn_int =  genutil_hex_to_int(HEX_ATN_value,8);
					sprintf(ascii_atn,"%010u",atn_int);
					strncpy(cavv2_atn_Code,&ascii_atn[6],4);
				 }		 /*If neither cvc1 or cvc2 is present, return no value*/
				 else if(strlen(auth_tx.EMV_details.issuer_script_2) == 0)
				 {
					racal_log_message( 2, 2,"CAVV-validation: CAVV data is blank.","racal_Verify_CAVV_for_3Dsecure_AMEX_Keyblock",1 );
					return(TRANSACTION_NOT_VERIFIED);
				 }
			}


		}
        else if(cardType == 'D')
	    {
        	if(0 == strncmp(auth_tx.TLF01_details.handler_queue,
        					"dcp",3))
        	{
        		//Asiapay 3D validation.
        		strncpy(cvv_value,
        				auth_tx.TLF01_details.cvc_data,
        				DINERS_DE122_CAVV_LEN);
				strncpy(cavv2_atn_Code,
						auth_tx.TLF01_details.exp_date,
						DINERS_DE14_SIZE);

        	}
        	else if(0 == strncmp(auth_tx.TLF01_details.handler_queue,
        			              "ncdci",5))
        	{
        		if(strlen(auth_tx.future_use_4)> DINERS_DE122_AUTH_TRACKING_NUM_POS)
        		{
				 // Lets retrive CVVV value present in DE122
					strncpy(cvv_value,
							auth_tx.future_use_4 + DINERS_DE122_CAVV_POS,
							DINERS_DE122_CAVV_LEN);
					strncpy(cavv2_atn_Code,
							auth_tx.future_use_4 + DINERS_DE122_UNPRIDICATABLE_NUM_POS,
							DINERS_DE122_UNPRIDICATABLE_NUM_LEN);
				}
        	}
	   }

		 /*Build message and send to Racal.  */
		 memset (&output,0,sizeof(output));
		 strcat(output, msgHeader);
		 strcat(output, cmdCode);
		 if( 'D' ==  cardType &&
			(auth_tx.CAF01_details.aba_nbr[1] == '1' || auth_tx.CAF01_details.aba_nbr[1] == 1))
		 { /* ticked: single length key validation*/
			 // Don't send U for single length validation
		 }
		 else
		 {/* un-ticked: double length key validation*/
		 strcat(output, "U");
		 }
		 strcat(output, auth_tx.CAF01_details.cvk[4].cvkA);
		 strcat(output, auth_tx.CAF01_details.cvk[4].cvkB);

		 if('D' ==  cardType)
		 {
			 if(0 == strncmp(auth_tx.TLF01_details.handler_queue,
 					"dcp",3))
			 {
				 strncat(output, &cvv_value[1],3);
			 }
			 else
			 {
				 strncat(output, cvv_value,3);
			 }
		 }
		 else
		 {
		 strncat(output, &cvv_value[1],3);
		 }

		 strcat(output, auth_tx.TLF01_details.card_num);
		 strcat(output, delimiter);
		 strcat(output, cavv2_atn_Code);

		 if (strcmp(auth_tx.BIN01_details.card_family,CARD_MASTERCARD) == 0)
		 {
			 strcat(output, "140");
		 }
		 else if(strcmp(auth_tx.BIN01_details.card_family,CARD_JCB) == 0)
		 {
			 if(0 == strncmp(auth_tx.TLF01_details.dcf01_retired_cwk,"07",2))
			 {
				 strcat(output, "700");
			 }
			 else
			 {
				 strcat(output, "000");
			 }

		 }
		 else
		 {
			 strcat(output, "000");
		 }


	 //if the output is bigger then 150 do not send it out.
	 if (strlen(output) > 150)
	 {
		racal_log_message( 2, 2,"CAVV Cmd Msg too long.","racalVerify_CAVV_for_3Dsecure",1 );
		return(TRANSACTION_NOT_VERIFIED);
	 }

	 memset(BUFFER,0,sizeof(BUFFER));
	 sprintf(BUFFER, "BEFORE-PORT: %s", output);
	 MYPRINT(BUFFER);

	 /*The message is built.  Send it over to the Racal box.*/
	 /*Once the a response is recieved, check for the message header,*/
	 /*response code, and the error code.  If it does not exactly match,*/
	 /*send TRANSACTION_NOT_VERIFIED back.*/
	 if (racalSendToPort(output) == TRANSACTION_VERIFIED)
	 {
		/*parse the message check for the message header.*/
		memset(&dummy,0,Dummy_Size);
		strncpy(dummy,returnString,4);
		if(strcmp(dummy,msgHeader) != 0)
	{
		   racal_log_message( 2, 2,"CAVV Msg Header mis-match.","racal_Verify_CAVV_for_3Dsecure_AMEX_Keyblock",1 );
		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
	}

		/*parse the message check for the response code.*/
		memset(&dummy,0,Dummy_Size);
		strncpy(dummy,returnString + 4, 2);
		if(strcmp(dummy,"CZ") !=0)
		{
		   racal_log_message( 2, 2,"CAVV Cmd Msg mis-match.","racal_Verify_CAVV_for_3Dsecure_AMEX_Keyblock",1 );
		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
		}

	/* Parse the message check for the error code. */
	memset(&dummy,0,Dummy_Size);
	strncpy(dummy,returnString + 6, 2);
	if(strcmp(dummy,"00") == 0)
	{
		  return (TRANSACTION_VERIFIED); /*cavv valid*/
	}

		if(strcmp(dummy,"01") == 0)
		{
		   racal_log_message( 2, 2,"CAVV-VERIFY failure.","racal_Verify_CAVV_for_3Dsecure_AMEX_Keyblock",1 );
		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
		}
		if(strcmp(dummy,"10") == 0)
		{
			racal_log_message( 2, 2,"CAVV-VERIFY CVK A or B parity error.","racal_Verify_CAVV_for_3Dsecure_AMEX_Keyblock",1 );
			return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
		}

		if(strcmp(dummy,"12") == 0)
		{
		   racal_log_message( 2, 2,"CAVV-VERIFY No Key loaded in user storage.","racal_Verify_CAVV_for_3Dsecure_AMEX_Keyblock",1 );
		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
		}

		if(strcmp(dummy,"13") == 0)
		{
		   racal_log_message( 2, 2,"CAVV-VERIFY LMK error report to superviser.","racal_Verify_CAVV_for_3Dsecure_AMEX_Keyblock",1 );
		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
		}

		if(strcmp(dummy,"15") == 0)
		{
		   racal_log_message( 2, 2,"CAVV-VERIFY Error in input data.","racal_Verify_CAVV_for_3Dsecure_AMEX_Keyblock",1 );
		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
		}

		if(strcmp(dummy,"21") == 0)
		{
		   racal_log_message( 2, 2,"CAVV-VERIFY Invalid user storage index.","racal_Verify_CAVV_for_3Dsecure_AMEX_Keyblock",1 );
		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
		}

	 }
	 else  /* else for: if (racalSendToPort(output) == TRANSACTION_VERIFIED)*/
	 {
			   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
	 }



 return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
 break;
		         /********************end of Visa Case*****************************************/


   default:
	   {
		   racal_log_message( 2, 2,"Card Type Undefined.","racal_Verify_CAVV_for_3Dsecure_AMEX_Keyblock",1 );
		   return(TRANSACTION_NOT_VERIFIED);
	   }


   }/*end of switch statement.*/

}/*racal_Verify_CAVV_for_3Dsecure_AMEX_Keyblock*/
/*******************************************/

/**************************************************************/
int racal_Verify_CAVV_for_3Dsecure_JCB_Keyblock()
{
   char msgHeader[5] = {0};
   char cmdCode[3] = {0};
   char cvv_value[5] = {0};
   char cavv2_atn_Code[5] = {0};
   char delimiter[2] = {0};
   char output[200]= {0};
   char cardType = 'Z';
   char csc[6] = {0};

   INT  de48_retcode= 0;
   INT  ucaf_data_64_len =0;
   BYTE HEX_ATN_value[9]={0};
   BYTE UCAF_data_ascii[41]={0};
   BYTE UCAF_data_bin[41]={0};
   INT  ucaf_data_len=0;
   INT  dcp_len = 0;
   int  cnt=0;
   int  result=0;
   int  jcnt=0;
   char BUFFER [256] = {0};
   BYTE temp_len[4] = {0};
   CHAR *tempptr;
   CHAR fieldSeperator[2] = {0};
   BYTE UCAF_data[33]={0};
   int  len = 0;
   int  atn_int = 0;
   int  astr_len = 0;
   BYTE ascii_atn[9]={0};

   memset (&msgHeader,0,sizeof(msgHeader));
   memset (&cmdCode,0,sizeof(cmdCode));
   memset (&delimiter,0,sizeof(delimiter));
   memset (&output,0,sizeof(output));
   memset (&returnString,0,ReturnString_Size);
   memset (&csc,0,sizeof(csc));
   /* For DCP UCAF data */

   /* ENDS */
   strcpy (msgHeader ,"1234");

   if (strcmp(auth_tx.BIN01_details.card_family,CARD_VISA) == 0)
      cardType = 'V';

   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_MASTERCARD) == 0)
      cardType = 'M';

   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_AMEX) == 0)
      cardType = 'A';

   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_JCB) == 0)
      cardType = 'J';
   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_CUP) == 0)
        cardType = 'C';
   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_DINERS) == 0)
           cardType = 'D';

   switch(cardType)
   {
      case 'A':

 			 racal_log_message( 2, 2,"AMEX card - 3D secure code not implemented.","racal_Verify_CAVV_for_3Dsecure_JCB_Keyblock",1 );
			 return(TRANSACTION_NOT_VERIFIED);

         /****************************end AMEX***************************************/

         break;

      case 'J':
         /*drop down to VISA for this function.*/
         /*break;*/

      case 'M':

         /*drop down to VISA for this function.*/
         /*break;*/
      case 'C':

		 /*drop down to VISA for this function.*/
		 /*break;*/
      case 'D':

		 /*drop down to VISA for this function.*/
		 /*break;*/
      case 'V':

         /*****************************begin Visa***************************************/
         /*The VISA, MC, and the JCB are all the same.*/
         /*The rest of this code would deal with them.*/
         strcpy (cmdCode,"CY");
         strcpy (delimiter,";");

         /*edit CVK A */
         if(strlen(auth_tx.CAF01_details.cvk[4].cvkA) == 0)
		 {
            racal_log_message( 2, 2,"CAVV-validation: CAK is blank.","racal_Verify_CAVV_for_3Dsecure_JCB_Keyblock",1 );
            return(TRANSACTION_NOT_VERIFIED);
         }
         /*edit CVK B */
         if(strlen(auth_tx.CAF01_details.cvk[4].cvkB) == 0)
         {
            racal_log_message( 2, 2,"CAVV-validation: CAK is blank.","racal_Verify_CAVV_for_3Dsecure_JCB_Keyblock",1 );
            return(TRANSACTION_NOT_VERIFIED);
         }

		 /*edit card number*/
	 	 if(strlen(auth_tx.TLF01_details.card_num) == 0 )
         {
            racal_log_message( 2, 2,"CAVV-validation: card_num is blank.","racal_Verify_CAVV_for_3Dsecure_JCB_Keyblock",1 );
            return(TRANSACTION_NOT_VERIFIED);
         }

		  /*Get DE126.9 CAVV from Message*/
        if(cardType == 'V' || cardType == 'J')
		{
			if(strlen(auth_tx.TLF01_details.dcf01_retired_cwk) != 0  && strcmp(auth_tx.TLF01_details.nfi_end_date,"1" == 0))
			 {
				 strncpy(cvv_value,&auth_tx.TLF01_details.dcf01_retired_cwk[6],4);
				 strncpy(cavv2_atn_Code,&auth_tx.TLF01_details.dcf01_retired_cwk[10],4);
				 if(cardType == 'J')
				 {
					 /* From JCB CAVV value will be sent 40, which is assumend as spaces,
					  * hence check the raw data which stored in auth_tx.acct_id_2 without converting.
					  * position 3 and 4 contains CVV verifiation value(4 bytes after convertion)
					  * position 5 and 6 contains unpredictable number(4 bytes after convertion)
					  * 40 binary value is viewd as @, hence compare with it to send spaces.
					  * */
					 if(0 == strncmp(&auth_tx.acct_id_2[3],"@@",2))
					 {
						 memcpy(cvv_value,"    ",4);
					 }
					 if(0 == strncmp(&auth_tx.acct_id_2[5],"@@",2))
					 {
						 memcpy(cavv2_atn_Code,"    ",4);
					 }
				 }
			 }		 /*If neither cvc1 or cvc2 is present, return no value*/
			 else if(strlen(auth_tx.TLF01_details.dcf01_retired_cwk) == 0)
			 {
				racal_log_message( 2, 2,"CAVV-validation: CAVV data 126.9 is blank.","racal_Verify_CAVV_for_3Dsecure_JCB_Keyblock",1 );
				return(TRANSACTION_NOT_VERIFIED);
			 }
		}
        else if(cardType == 'M')
		{

			if (strncmp(auth_tx.TLF01_details.handler_queue,"dcp",3) == 0)
			{
				/* 3D secure data structre of AsiaPay transaction:
				 *
				 * Total length : 2 bytes : N 4
				 * Actual data length: 2 bytes : N 4
				 * Table ID (MC ECOM indicator value): 2 bytes : N 4
				 * MC merchant/ CH certificate serial number :  2 bytes length (N 4)+ 40 bytes (N 80)
				 * MC security level indicator : 2 bytes length (N 4)+ 7 bytes (N 14)
				 * MC UCAF data length : 2 bytes length (N 4)+ 28 bytes (N 56)
				 *
				 * DCPISO will send data in auth_tx.int_net_facility_data, Data after table ID will be
				 * received by racal.
				 *
				 * After the conversion  UCAF data will be populated as follows for both
				 * incoming as well as Asiapay
				 *
				 * Total 40 Bytes:
				 *	Bit 1 & 2   cByte 8C = Auth Success 86 = Auth Attempt
				 *	Bit 3 - 18  Merchant Name First 16 digits Hexa format of SHA Hased Merchant Name
				 *	Bit 19 & 20 08 for ACS
				 *	Bit 21 - 22 Authentication Method
				 * 		Indates how the cardholder was authenticated to the ACS;
				 *  	0 = No cardholder authentication performed (only valid for AAV with X'86' - Attempts Processin
				 *  	1 = Password
				 *  	2 = Secret Key (e.g. Chip Card) 3 = PKI (pending further discussions)
				 *  	01 = Auth Attempt
				 *  	11 = Others
				 *  Bit 23 - 30 ATN  Last 8 digits of AcctID
				 *  Bit 31 - 34 CVV for Authentication Leading zero filled CVV
				 *  Bit 35 - 40 Filler Zero fill
				 * */
				if(strlen(auth_tx.int_net_facility_len) !=0)
				 {
					dcp_len = genutil_bcd_to_int (&auth_tx.int_net_facility_data[DCP_UCAF_LEN_POS], 2);
					memcpy(UCAF_data,&auth_tx.int_net_facility_data[DCP_UCAF_DATA_POS],dcp_len);
					ucaf_data_64_len =strlen(UCAF_data);
					ucaf_data_len = genutil_base64_to_binary( UCAF_data, strlen(UCAF_data),&UCAF_data_bin );
					genutil_bin_to_ascii (UCAF_data_bin, ucaf_data_len, UCAF_data_ascii, astr_len);
					printf("UCAF dat: %s, len %d \n",UCAF_data_ascii,astr_len);
					strncpy(cvv_value,&UCAF_data_ascii[30],4);
					strncpy(HEX_ATN_value,&UCAF_data_ascii[22],8);

					atn_int =  genutil_hex_to_int(HEX_ATN_value,8);
					sprintf(ascii_atn,"%010u",atn_int);
					strncpy(cavv2_atn_Code,&ascii_atn[6],4);
				 }		 /*If neither cvc1 or cvc2 is present, return no value*/
				 else if(strlen(auth_tx.int_net_facility_len) == 0)
				 {
					racal_log_message( 2, 2,"CAVV-validation: CAVV data is blank.","racal_Verify_CAVV_for_3Dsecure_JCB_Keyblock",1 );
					return(TRANSACTION_NOT_VERIFIED);
				 }
			}
			else
			{
				if(strlen(auth_tx.EMV_details.issuer_script_2) !=0)
				 {
					de48_retcode = parse_field_48( strlen(auth_tx.EMV_details.issuer_script_2),&auth_tx.EMV_details.issuer_script_2,&UCAF_data );
					ucaf_data_64_len =strlen(UCAF_data);
					ucaf_data_len = genutil_base64_to_binary( UCAF_data, strlen(UCAF_data),&UCAF_data_bin );
					genutil_bin_to_ascii (UCAF_data_bin, ucaf_data_len, UCAF_data_ascii, astr_len);
					printf("UCAF dat: %s, len %d \n",UCAF_data_ascii,astr_len);
					strncpy(cvv_value,&UCAF_data_ascii[30],4);
					strncpy(HEX_ATN_value,&UCAF_data_ascii[22],8);

					atn_int =  genutil_hex_to_int(HEX_ATN_value,8);
					sprintf(ascii_atn,"%010u",atn_int);
					strncpy(cavv2_atn_Code,&ascii_atn[6],4);
				 }		 /*If neither cvc1 or cvc2 is present, return no value*/
				 else if(strlen(auth_tx.EMV_details.issuer_script_2) == 0)
				 {
					racal_log_message( 2, 2,"CAVV-validation: CAVV data is blank.","racal_Verify_CAVV_for_3Dsecure_JCB_Keyblock",1 );
					return(TRANSACTION_NOT_VERIFIED);
				 }
			}


		}
        else if(cardType == 'D')
	    {
        	if(0 == strncmp(auth_tx.TLF01_details.handler_queue,
        					"dcp",3))
        	{
        		//Asiapay 3D validation.
        		strncpy(cvv_value,
        				auth_tx.TLF01_details.cvc_data,
        				DINERS_DE122_CAVV_LEN);
				strncpy(cavv2_atn_Code,
						auth_tx.TLF01_details.exp_date,
						DINERS_DE14_SIZE);

        	}
        	else if(0 == strncmp(auth_tx.TLF01_details.handler_queue,
        			              "ncdci",5))
        	{
        		if(strlen(auth_tx.future_use_4)> DINERS_DE122_AUTH_TRACKING_NUM_POS)
        		{
				 // Lets retrive CVVV value present in DE122
					strncpy(cvv_value,
							auth_tx.future_use_4 + DINERS_DE122_CAVV_POS,
							DINERS_DE122_CAVV_LEN);
					strncpy(cavv2_atn_Code,
							auth_tx.future_use_4 + DINERS_DE122_UNPRIDICATABLE_NUM_POS,
							DINERS_DE122_UNPRIDICATABLE_NUM_LEN);
				}
        	}
	   }

		 /*Build message and send to Racal.  */
		 memset (&output,0,sizeof(output));
		 strcat(output, msgHeader);
		 strcat(output, cmdCode);
		 if( 'D' ==  cardType &&
			(auth_tx.CAF01_details.aba_nbr[1] == '1' || auth_tx.CAF01_details.aba_nbr[1] == 1))
		 { /* ticked: single length key validation*/
			 // Don't send U for single length validation
		 }
		 else
		 {/* un-ticked: double length key validation*/
		 strcat(output, "U");
		 }
		 strcat(output, auth_tx.CAF01_details.cvk[4].cvkA);
		 strcat(output, auth_tx.CAF01_details.cvk[4].cvkB);

		 if('D' ==  cardType)
		 {
			 if(0 == strncmp(auth_tx.TLF01_details.handler_queue,
 					"dcp",3))
			 {
				 strncat(output, &cvv_value[1],3);
			 }
			 else
			 {
				 strncat(output, cvv_value,3);
			 }
		 }
		 else
		 {
		 strncat(output, &cvv_value[1],3);
		 }

		 strcat(output, auth_tx.TLF01_details.card_num);
		 strcat(output, delimiter);
		 strcat(output, cavv2_atn_Code);

		 if (strcmp(auth_tx.BIN01_details.card_family,CARD_MASTERCARD) == 0)
		 {
			 strcat(output, "140");
		 }
		 else if(strcmp(auth_tx.BIN01_details.card_family,CARD_JCB) == 0)
		 {
			 if(0 == strncmp(auth_tx.TLF01_details.dcf01_retired_cwk,"07",2))
			 {
				 strcat(output, "700");
			 }
			 else
			 {
				 strcat(output, "000");
			 }

		 }
		 else
		 {
			 strcat(output, "000");
		 }


	 //if the output is bigger then 150 do not send it out.
	 if (strlen(output) > 150)
	 {
		racal_log_message( 2, 2,"CAVV Cmd Msg too long.","racal_Verify_CAVV_for_3Dsecure_JCB_Keyblock",1 );
		return(TRANSACTION_NOT_VERIFIED);
	 }

	 memset(BUFFER,0,sizeof(BUFFER));
	 sprintf(BUFFER, "BEFORE-PORT: %s", output);
	 MYPRINT(BUFFER);

	 /*The message is built.  Send it over to the Racal box.*/
	 /*Once the a response is recieved, check for the message header,*/
	 /*response code, and the error code.  If it does not exactly match,*/
	 /*send TRANSACTION_NOT_VERIFIED back.*/
	 if (racalSendToPort(output) == TRANSACTION_VERIFIED)
	 {
		/*parse the message check for the message header.*/
		memset(&dummy,0,Dummy_Size);
		strncpy(dummy,returnString,4);
		if(strcmp(dummy,msgHeader) != 0)
	{
		   racal_log_message( 2, 2,"CAVV Msg Header mis-match.","racal_Verify_CAVV_for_3Dsecure_JCB_Keyblock",1 );
		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
	}

		/*parse the message check for the response code.*/
		memset(&dummy,0,Dummy_Size);
		strncpy(dummy,returnString + 4, 2);
		if(strcmp(dummy,"CZ") !=0)
		{
		   racal_log_message( 2, 2,"CAVV Cmd Msg mis-match.","racal_Verify_CAVV_for_3Dsecure_JCB_Keyblock",1 );
		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
		}

	/* Parse the message check for the error code. */
	memset(&dummy,0,Dummy_Size);
	strncpy(dummy,returnString + 6, 2);
	if(strcmp(dummy,"00") == 0)
	{
		  return (TRANSACTION_VERIFIED); /*cavv valid*/
	}

		if(strcmp(dummy,"01") == 0)
		{
		   racal_log_message( 2, 2,"CAVV-VERIFY failure.","racal_Verify_CAVV_for_3Dsecure_JCB_Keyblock",1 );
		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
		}
		if(strcmp(dummy,"10") == 0)
		{
			racal_log_message( 2, 2,"CAVV-VERIFY CVK A or B parity error.","racal_Verify_CAVV_for_3Dsecure_JCB_Keyblock",1 );
			return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
		}

		if(strcmp(dummy,"12") == 0)
		{
		   racal_log_message( 2, 2,"CAVV-VERIFY No Key loaded in user storage.","racal_Verify_CAVV_for_3Dsecure_JCB_Keyblock",1 );
		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
		}

		if(strcmp(dummy,"13") == 0)
		{
		   racal_log_message( 2, 2,"CAVV-VERIFY LMK error report to superviser.","racal_Verify_CAVV_for_3Dsecure_JCB_Keyblock",1 );
		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
		}

		if(strcmp(dummy,"15") == 0)
		{
		   racal_log_message( 2, 2,"CAVV-VERIFY Error in input data.","racal_Verify_CAVV_for_3Dsecure_JCB_Keyblock",1 );
		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
		}

		if(strcmp(dummy,"21") == 0)
		{
		   racal_log_message( 2, 2,"CAVV-VERIFY Invalid user storage index.","racal_Verify_CAVV_for_3Dsecure_JCB_Keyblock",1 );
		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
		}

	 }
	 else  /* else for: if (racalSendToPort(output) == TRANSACTION_VERIFIED)*/
	 {
			   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
	 }



 return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
 break;
		         /********************end of Visa Case*****************************************/


   default:
	   {
		   racal_log_message( 2, 2,"Card Type Undefined.","racal_Verify_CAVV_for_3Dsecure_JCB_Keyblock",1 );
		   return(TRANSACTION_NOT_VERIFIED);
	   }


   }/*end of switch statement.*/

}/*racal_Verify_CAVV_for_3Dsecure_JCB_Keyblock*/
/*******************************************/

/**************************************************************/
int racal_Verify_CAVV_for_3Dsecure_UPI_Keyblock()
{
   char msgHeader[5] = {0};
   char cmdCode[3] = {0};
   char cvv_value[5] = {0};
   char cavv2_atn_Code[5] = {0};
   char delimiter[2] = {0};
   char output[200]= {0};
   char cardType = 'Z';
   char csc[6] = {0};

   INT  de48_retcode= 0;
   INT  ucaf_data_64_len =0;
   BYTE HEX_ATN_value[9]={0};
   BYTE UCAF_data_ascii[41]={0};
   BYTE UCAF_data_bin[41]={0};
   INT  ucaf_data_len=0;
   INT  dcp_len = 0;
   int  cnt=0;
   int  result=0;
   int  jcnt=0;
   char BUFFER [256] = {0};
   BYTE temp_len[4] = {0};
   CHAR *tempptr;
   CHAR fieldSeperator[2] = {0};
   BYTE UCAF_data[33]={0};
   int  len = 0;
   int  atn_int = 0;
   int  astr_len = 0;
   BYTE ascii_atn[9]={0};

   memset (&msgHeader,0,sizeof(msgHeader));
   memset (&cmdCode,0,sizeof(cmdCode));
   memset (&delimiter,0,sizeof(delimiter));
   memset (&output,0,sizeof(output));
   memset (&returnString,0,ReturnString_Size);
   memset (&csc,0,sizeof(csc));
   /* For DCP UCAF data */

   /* ENDS */
   strcpy (msgHeader ,"1234");

   if (strcmp(auth_tx.BIN01_details.card_family,CARD_VISA) == 0)
      cardType = 'V';

   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_MASTERCARD) == 0)
      cardType = 'M';

   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_AMEX) == 0)
      cardType = 'A';

   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_JCB) == 0)
      cardType = 'J';
   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_CUP) == 0)
        cardType = 'C';
   else if (strcmp(auth_tx.BIN01_details.card_family,CARD_DINERS) == 0)
           cardType = 'D';

   switch(cardType)
   {
      case 'A':

 			 racal_log_message( 2, 2,"AMEX card - 3D secure code not implemented.","racal_Verify_CAVV_for_3Dsecure_UPI_Keyblock",1 );
			 return(TRANSACTION_NOT_VERIFIED);

         /****************************end AMEX***************************************/

         break;

      case 'J':
         /*drop down to VISA for this function.*/
         /*break;*/

      case 'M':

         /*drop down to VISA for this function.*/
         /*break;*/
      case 'C':

		 /*drop down to VISA for this function.*/
		 /*break;*/
      case 'D':

		 /*drop down to VISA for this function.*/
		 /*break;*/
      case 'V':

         /*****************************begin Visa***************************************/
         /*The VISA, MC, and the JCB are all the same.*/
         /*The rest of this code would deal with them.*/
         strcpy (cmdCode,"CY");
         strcpy (delimiter,";");

         /*edit CVK A */
         if(strlen(auth_tx.CAF01_details.cvk[4].cvkA) == 0)
		 {
            racal_log_message( 2, 2,"CAVV-validation: CAK is blank.","racal_Verify_CAVV_for_3Dsecure_UPI_Keyblock",1 );
            return(TRANSACTION_NOT_VERIFIED);
         }
         /*edit CVK B */
         if(strlen(auth_tx.CAF01_details.cvk[4].cvkB) == 0)
         {
            racal_log_message( 2, 2,"CAVV-validation: CAK is blank.","racal_Verify_CAVV_for_3Dsecure_UPI_Keyblock",1 );
            return(TRANSACTION_NOT_VERIFIED);
         }

		 /*edit card number*/
	 	 if(strlen(auth_tx.TLF01_details.card_num) == 0 )
         {
            racal_log_message( 2, 2,"CAVV-validation: card_num is blank.","racal_Verify_CAVV_for_3Dsecure_UPI_Keyblock",1 );
            return(TRANSACTION_NOT_VERIFIED);
         }

		  /*Get DE126.9 CAVV from Message*/
        if(cardType == 'V' || cardType == 'J')
		{
			if(strlen(auth_tx.TLF01_details.dcf01_retired_cwk) != 0  && strcmp(auth_tx.TLF01_details.nfi_end_date,"1" == 0))
			 {
				 strncpy(cvv_value,&auth_tx.TLF01_details.dcf01_retired_cwk[6],4);
				 strncpy(cavv2_atn_Code,&auth_tx.TLF01_details.dcf01_retired_cwk[10],4);
				 if(cardType == 'J')
				 {
					 /* From JCB CAVV value will be sent 40, which is assumend as spaces,
					  * hence check the raw data which stored in auth_tx.acct_id_2 without converting.
					  * position 3 and 4 contains CVV verifiation value(4 bytes after convertion)
					  * position 5 and 6 contains unpredictable number(4 bytes after convertion)
					  * 40 binary value is viewd as @, hence compare with it to send spaces.
					  * */
					 if(0 == strncmp(&auth_tx.acct_id_2[3],"@@",2))
					 {
						 memcpy(cvv_value,"    ",4);
					 }
					 if(0 == strncmp(&auth_tx.acct_id_2[5],"@@",2))
					 {
						 memcpy(cavv2_atn_Code,"    ",4);
					 }
				 }
			 }		 /*If neither cvc1 or cvc2 is present, return no value*/
			 else if(strlen(auth_tx.TLF01_details.dcf01_retired_cwk) == 0)
			 {
				racal_log_message( 2, 2,"CAVV-validation: CAVV data 126.9 is blank.","racal_Verify_CAVV_for_3Dsecure_UPI_Keyblock",1 );
				return(TRANSACTION_NOT_VERIFIED);
			 }
		}
        else if(cardType == 'M')
		{

			if (strncmp(auth_tx.TLF01_details.handler_queue,"dcp",3) == 0)
			{
				/* 3D secure data structre of AsiaPay transaction:
				 *
				 * Total length : 2 bytes : N 4
				 * Actual data length: 2 bytes : N 4
				 * Table ID (MC ECOM indicator value): 2 bytes : N 4
				 * MC merchant/ CH certificate serial number :  2 bytes length (N 4)+ 40 bytes (N 80)
				 * MC security level indicator : 2 bytes length (N 4)+ 7 bytes (N 14)
				 * MC UCAF data length : 2 bytes length (N 4)+ 28 bytes (N 56)
				 *
				 * DCPISO will send data in auth_tx.int_net_facility_data, Data after table ID will be
				 * received by racal.
				 *
				 * After the conversion  UCAF data will be populated as follows for both
				 * incoming as well as Asiapay
				 *
				 * Total 40 Bytes:
				 *	Bit 1 & 2   cByte 8C = Auth Success 86 = Auth Attempt
				 *	Bit 3 - 18  Merchant Name First 16 digits Hexa format of SHA Hased Merchant Name
				 *	Bit 19 & 20 08 for ACS
				 *	Bit 21 - 22 Authentication Method
				 * 		Indates how the cardholder was authenticated to the ACS;
				 *  	0 = No cardholder authentication performed (only valid for AAV with X'86' - Attempts Processin
				 *  	1 = Password
				 *  	2 = Secret Key (e.g. Chip Card) 3 = PKI (pending further discussions)
				 *  	01 = Auth Attempt
				 *  	11 = Others
				 *  Bit 23 - 30 ATN  Last 8 digits of AcctID
				 *  Bit 31 - 34 CVV for Authentication Leading zero filled CVV
				 *  Bit 35 - 40 Filler Zero fill
				 * */
				if(strlen(auth_tx.int_net_facility_len) !=0)
				 {
					dcp_len = genutil_bcd_to_int (&auth_tx.int_net_facility_data[DCP_UCAF_LEN_POS], 2);
					memcpy(UCAF_data,&auth_tx.int_net_facility_data[DCP_UCAF_DATA_POS],dcp_len);
					ucaf_data_64_len =strlen(UCAF_data);
					ucaf_data_len = genutil_base64_to_binary( UCAF_data, strlen(UCAF_data),&UCAF_data_bin );
					genutil_bin_to_ascii (UCAF_data_bin, ucaf_data_len, UCAF_data_ascii, astr_len);
					printf("UCAF dat: %s, len %d \n",UCAF_data_ascii,astr_len);
					strncpy(cvv_value,&UCAF_data_ascii[30],4);
					strncpy(HEX_ATN_value,&UCAF_data_ascii[22],8);

					atn_int =  genutil_hex_to_int(HEX_ATN_value,8);
					sprintf(ascii_atn,"%010u",atn_int);
					strncpy(cavv2_atn_Code,&ascii_atn[6],4);
				 }		 /*If neither cvc1 or cvc2 is present, return no value*/
				 else if(strlen(auth_tx.int_net_facility_len) == 0)
				 {
					racal_log_message( 2, 2,"CAVV-validation: CAVV data is blank.","racal_Verify_CAVV_for_3Dsecure_UPI_Keyblock",1 );
					return(TRANSACTION_NOT_VERIFIED);
				 }
			}
			else
			{
				if(strlen(auth_tx.EMV_details.issuer_script_2) !=0)
				 {
					de48_retcode = parse_field_48( strlen(auth_tx.EMV_details.issuer_script_2),&auth_tx.EMV_details.issuer_script_2,&UCAF_data );
					ucaf_data_64_len =strlen(UCAF_data);
					ucaf_data_len = genutil_base64_to_binary( UCAF_data, strlen(UCAF_data),&UCAF_data_bin );
					genutil_bin_to_ascii (UCAF_data_bin, ucaf_data_len, UCAF_data_ascii, astr_len);
					printf("UCAF dat: %s, len %d \n",UCAF_data_ascii,astr_len);
					strncpy(cvv_value,&UCAF_data_ascii[30],4);
					strncpy(HEX_ATN_value,&UCAF_data_ascii[22],8);

					atn_int =  genutil_hex_to_int(HEX_ATN_value,8);
					sprintf(ascii_atn,"%010u",atn_int);
					strncpy(cavv2_atn_Code,&ascii_atn[6],4);
				 }		 /*If neither cvc1 or cvc2 is present, return no value*/
				 else if(strlen(auth_tx.EMV_details.issuer_script_2) == 0)
				 {
					racal_log_message( 2, 2,"CAVV-validation: CAVV data is blank.","racal_Verify_CAVV_for_3Dsecure_UPI_Keyblock",1 );
					return(TRANSACTION_NOT_VERIFIED);
				 }
			}


		}
        else if(cardType == 'D')
	    {
        	if(0 == strncmp(auth_tx.TLF01_details.handler_queue,
        					"dcp",3))
        	{
        		//Asiapay 3D validation.
        		strncpy(cvv_value,
        				auth_tx.TLF01_details.cvc_data,
        				DINERS_DE122_CAVV_LEN);
				strncpy(cavv2_atn_Code,
						auth_tx.TLF01_details.exp_date,
						DINERS_DE14_SIZE);

        	}
        	else if(0 == strncmp(auth_tx.TLF01_details.handler_queue,
        			              "ncdci",5))
        	{
        		if(strlen(auth_tx.future_use_4)> DINERS_DE122_AUTH_TRACKING_NUM_POS)
        		{
				 // Lets retrive CVVV value present in DE122
					strncpy(cvv_value,
							auth_tx.future_use_4 + DINERS_DE122_CAVV_POS,
							DINERS_DE122_CAVV_LEN);
					strncpy(cavv2_atn_Code,
							auth_tx.future_use_4 + DINERS_DE122_UNPRIDICATABLE_NUM_POS,
							DINERS_DE122_UNPRIDICATABLE_NUM_LEN);
				}
        	}
	   }

		 /*Build message and send to Racal.  */
		 memset (&output,0,sizeof(output));
		 strcat(output, msgHeader);
		 strcat(output, cmdCode);
		 if( 'D' ==  cardType &&
			(auth_tx.CAF01_details.aba_nbr[1] == '1' || auth_tx.CAF01_details.aba_nbr[1] == 1))
		 { /* ticked: single length key validation*/
			 // Don't send U for single length validation
		 }
		 else
		 {/* un-ticked: double length key validation*/
		 strcat(output, "U");
		 }
		 strcat(output, auth_tx.CAF01_details.cvk[4].cvkA);
		 strcat(output, auth_tx.CAF01_details.cvk[4].cvkB);

		 if('D' ==  cardType)
		 {
			 if(0 == strncmp(auth_tx.TLF01_details.handler_queue,
 					"dcp",3))
			 {
				 strncat(output, &cvv_value[1],3);
			 }
			 else
			 {
				 strncat(output, cvv_value,3);
			 }
		 }
		 else
		 {
		 strncat(output, &cvv_value[1],3);
		 }

		 strcat(output, auth_tx.TLF01_details.card_num);
		 strcat(output, delimiter);
		 strcat(output, cavv2_atn_Code);

		 if (strcmp(auth_tx.BIN01_details.card_family,CARD_MASTERCARD) == 0)
		 {
			 strcat(output, "140");
		 }
		 else if(strcmp(auth_tx.BIN01_details.card_family,CARD_JCB) == 0)
		 {
			 if(0 == strncmp(auth_tx.TLF01_details.dcf01_retired_cwk,"07",2))
			 {
				 strcat(output, "700");
			 }
			 else
			 {
				 strcat(output, "000");
			 }

		 }
		 else
		 {
			 strcat(output, "000");
		 }


	 //if the output is bigger then 150 do not send it out.
	 if (strlen(output) > 150)
	 {
		racal_log_message( 2, 2,"CAVV Cmd Msg too long.","racalVerify_CAVV_for_3Dsecure",1 );
		return(TRANSACTION_NOT_VERIFIED);
	 }

	 memset(BUFFER,0,sizeof(BUFFER));
	 sprintf(BUFFER, "BEFORE-PORT: %s", output);
	 MYPRINT(BUFFER);

	 /*The message is built.  Send it over to the Racal box.*/
	 /*Once the a response is recieved, check for the message header,*/
	 /*response code, and the error code.  If it does not exactly match,*/
	 /*send TRANSACTION_NOT_VERIFIED back.*/
	 if (racalSendToPort(output) == TRANSACTION_VERIFIED)
	 {
		/*parse the message check for the message header.*/
		memset(&dummy,0,Dummy_Size);
		strncpy(dummy,returnString,4);
		if(strcmp(dummy,msgHeader) != 0)
	{
		   racal_log_message( 2, 2,"CAVV Msg Header mis-match.","racal_Verify_CAVV_for_3Dsecure_UPI_Keyblock",1 );
		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
	}

		/*parse the message check for the response code.*/
		memset(&dummy,0,Dummy_Size);
		strncpy(dummy,returnString + 4, 2);
		if(strcmp(dummy,"CZ") !=0)
		{
		   racal_log_message( 2, 2,"CAVV Cmd Msg mis-match.","racal_Verify_CAVV_for_3Dsecure_UPI_Keyblock",1 );
		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
		}

	/* Parse the message check for the error code. */
	memset(&dummy,0,Dummy_Size);
	strncpy(dummy,returnString + 6, 2);
	if(strcmp(dummy,"00") == 0)
	{
		  return (TRANSACTION_VERIFIED); /*cavv valid*/
	}

		if(strcmp(dummy,"01") == 0)
		{
		   racal_log_message( 2, 2,"CAVV-VERIFY failure.","racal_Verify_CAVV_for_3Dsecure_UPI_Keyblock",1 );
		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
		}
		if(strcmp(dummy,"10") == 0)
		{
			racal_log_message( 2, 2,"CAVV-VERIFY CVK A or B parity error.","racal_Verify_CAVV_for_3Dsecure_UPI_Keyblock",1 );
			return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
		}

		if(strcmp(dummy,"12") == 0)
		{
		   racal_log_message( 2, 2,"CAVV-VERIFY No Key loaded in user storage.","racal_Verify_CAVV_for_3Dsecure_UPI_Keyblock",1 );
		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
		}

		if(strcmp(dummy,"13") == 0)
		{
		   racal_log_message( 2, 2,"CAVV-VERIFY LMK error report to superviser.","racal_Verify_CAVV_for_3Dsecure_UPI_Keyblock",1 );
		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
		}

		if(strcmp(dummy,"15") == 0)
		{
		   racal_log_message( 2, 2,"CAVV-VERIFY Error in input data.","racal_Verify_CAVV_for_3Dsecure_UPI_Keyblock",1 );
		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
		}

		if(strcmp(dummy,"21") == 0)
		{
		   racal_log_message( 2, 2,"CAVV-VERIFY Invalid user storage index.","racal_Verify_CAVV_for_3Dsecure_UPI_Keyblock",1 );
		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
		}

	 }
	 else  /* else for: if (racalSendToPort(output) == TRANSACTION_VERIFIED)*/
	 {
			   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
	 }



 return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
 break;
		         /********************end of Visa Case*****************************************/


   default:
	   {
		   racal_log_message( 2, 2,"Card Type Undefined.","racal_Verify_CAVV_for_3Dsecure_UPI_Keyblock",1 );
		   return(TRANSACTION_NOT_VERIFIED);
	   }


   }/*end of switch statement.*/

}/*racal_Verify_CAVV_for_3Dsecure_UPI_Keyblock*/
/*******************************************/

/**************************************************************/
int racal_Verify_CAVV_for_3Dsecure_DCI_Keyblock()
{
	char msgHeader[5] = {0};
	char cmdCode[3] = {0};
	char cvv_value[5] = {0};
	char cavv2_atn_Code[5] = {0};
	char delimiter[2] = {0};
	char output[500]= {0};

	memset (&returnString,0,ReturnString_Size);

	strcpy (msgHeader ,"1234");
	strcpy (cmdCode,"CY");
	strcpy (delimiter,";");

	if(0 == strncmp(auth_tx.TLF01_details.handler_queue,"dcp",3))
	{
		/*Asiapay 3D validation. */
		strncpy(cvv_value, auth_tx.TLF01_details.cvc_data,	DINERS_DE122_CAVV_LEN);
		strncpy(cavv2_atn_Code, auth_tx.TLF01_details.exp_date,	DINERS_DE14_SIZE);
	}
	else if(0 == strncmp(auth_tx.TLF01_details.handler_queue,"ncdci",5))
	{
		if(strlen(auth_tx.future_use_4)> DINERS_DE122_AUTH_TRACKING_NUM_POS)
		{
			/* Lets retrive CVVV value present in DE122 */
			strncpy(cvv_value,auth_tx.future_use_4 + DINERS_DE122_CAVV_POS,
					DINERS_DE122_CAVV_LEN);
			strncpy(cavv2_atn_Code,	auth_tx.future_use_4 + DINERS_DE122_UNPRIDICATABLE_NUM_POS,
					DINERS_DE122_UNPRIDICATABLE_NUM_LEN);
		}
	}

	strcat(output, msgHeader);
	strcat(output, cmdCode);

	if(strlen(caf01_keyblock.Cvk_Mag_Keyblock)== 0)
	{
		racal_log_message( 2, 2,"CAVV-validation: CAK is blank.","racal_Verify_CAVV_for_3Dsecure_MC_Keyblock",1 );
		return(TRANSACTION_NOT_VERIFIED);
	}
	else
	{
		strncat( output, SOUURCE_TPK_KEYBLOCK,KEYBLOCK_KEY_FORMAT_LEN );
		strcat(output, caf01_keyblock.Cvk_Mag_Keyblock);
	}

	if(0 == strncmp(auth_tx.TLF01_details.handler_queue,"dcp",3))
	{
		strncat(output, &cvv_value[1],3);
	}
	else
	{
		strncat(output, cvv_value,3);
	}

	if(strlen(auth_tx.TLF01_details.card_num) == 0 )
	{
		racal_log_message( 2, 2,"CAVV-validation: card_num is blank.","racal_Verify_CAVV_for_3Dsecure_DCI_Keyblock",1 );
		return(TRANSACTION_NOT_VERIFIED);
	}
	else
	{
		strcat(output, auth_tx.TLF01_details.card_num);
	}
	strcat(output, delimiter);
	strcat(output, cavv2_atn_Code);
	strcat(output, "000");

	if (strlen(output) > MAX_KEYBLOCK_CMD_LEN)
	{
		racal_log_message( 2, 2,"CAVV Cmd Msg too long.","racalVerify_CAVV_for_3Dsecure",1 );
		return(TRANSACTION_NOT_VERIFIED);
	}

	if (racalSendToPort(output) == TRANSACTION_VERIFIED)
	{
		memset(&dummy,0,Dummy_Size);
		strncpy(dummy,returnString,4);
		if(strcmp(dummy,msgHeader) != 0)
		{
			racal_log_message( 2, 2,"CAVV Msg Header mis-match.","racal_Verify_CAVV_for_3Dsecure_DCI_Keyblock",1 );
			return (TRANSACTION_NOT_VERIFIED);
		}

		memset(&dummy,0,Dummy_Size);
		strncpy(dummy,returnString + 4, 2);
		if(strcmp(dummy,"CZ") !=0)
		{
			racal_log_message( 2, 2,"CAVV Cmd Msg mis-match.","racal_Verify_CAVV_for_3Dsecure_DCI_Keyblock",1 );
			return (TRANSACTION_NOT_VERIFIED);
		}

		memset(&dummy,0,Dummy_Size);
		strncpy(dummy,returnString + 6, 2);
		if(strcmp(dummy,"00") == 0)
		{
			return (TRANSACTION_VERIFIED);
		}
		else if(strcmp(dummy,"01") == 0)
		{
			racal_log_message( 2, 2,"CAVV-VERIFY failure.","racal_Verify_CAVV_for_3Dsecure_DCI_Keyblock",1 );
			return (TRANSACTION_NOT_VERIFIED);
		}
		else if(strcmp(dummy,"10") == 0)
		{
			racal_log_message( 2, 2,"CAVV-VERIFY CVK A or B parity error.","racal_Verify_CAVV_for_3Dsecure_DCI_Keyblock",1 );
			return (TRANSACTION_NOT_VERIFIED);
		}
		else if(strcmp(dummy,"12") == 0)
		{
			racal_log_message( 2, 2,"CAVV-VERIFY No Key loaded in user storage.","racal_Verify_CAVV_for_3Dsecure_DCI_Keyblock",1 );
			return (TRANSACTION_NOT_VERIFIED);
		}
		else if(strcmp(dummy,"13") == 0)
		{
			racal_log_message( 2, 2,"CAVV-VERIFY LMK error report to superviser.","racal_Verify_CAVV_for_3Dsecure_DCI_Keyblock",1 );
			return (TRANSACTION_NOT_VERIFIED);
		}
		else if(strcmp(dummy,"15") == 0)
		{
			racal_log_message( 2, 2,"CAVV-VERIFY Error in input data.","racal_Verify_CAVV_for_3Dsecure_DCI_Keyblock",1 );
			return (TRANSACTION_NOT_VERIFIED);
		}
		else if(strcmp(dummy,"21") == 0)
		{
			racal_log_message( 2, 2,"CAVV-VERIFY Invalid user storage index.","racal_Verify_CAVV_for_3Dsecure_DCI_Keyblock",1 );
			return (TRANSACTION_NOT_VERIFIED);
		}
	}
	else
	{
		return (TRANSACTION_NOT_VERIFIED);
	}

	return (TRANSACTION_NOT_VERIFIED);

}/*racal_Verify_CAVV_for_3Dsecure_DCI_Keyblock*/
/*******************************************/

int racal_Verify_CAVV_for_3Dsecure2_VISA_Keyblock()
{
   char msgHeader[5] = {0};
   char cmdCode[3] = {0};
   char cvv_value[5] = {0};
   char cavv2_atn_Code[5] = {0};
   char delimiter[2] = {0};
   char output[200]= {0};
   char cardType = 'Z';
   char csc[6] = {0};
   char Suppl_Amt[50] = {0};
   char strTemp[512] = {0};

   INT  de48_retcode= 0;
   INT  ucaf_data_64_len =0;
   BYTE HEX_ATN_value[9]={0};
   BYTE UCAF_data_ascii[41]={0};
   BYTE UCAF_data_bin[41]={0};
   INT  ucaf_data_len=0;
   INT  dcp_len = 0;
   int  cnt=0;
   int  result=0;
   int  jcnt=0;
   char BUFFER [256] = {0};
   BYTE temp_len[4] = {0};
   CHAR *tempptr;
   CHAR fieldSeperator[2] = {0};
   BYTE UCAF_data[33]={0};
   int  len = 0;
   int  atn_int = 0;
   int  astr_len = 0;
   BYTE ascii_atn[9]={0};
   BYTE HMAC_key[33];
   CHAR hash_identier[4] = {0};
   CHAR hmac_length[10] = {0};
   CHAR hmac_key_len[4] = {0};
   memset (&msgHeader,0,sizeof(msgHeader));
   memset (&cmdCode,0,sizeof(cmdCode));
   memset (&delimiter,0,sizeof(delimiter));
   memset (&output,0,sizeof(output));
   memset (&returnString,0,ReturnString_Size);
   memset (&csc,0,sizeof(csc));

   /* ENDS */
   strcpy (msgHeader ,"1234");

   strcpy (cmdCode,"CW");
   //strcpy(hash_identier,"06");
   //strncpy(hmac_length, "0064",4);
   	 strcpy (delimiter,";");
   	 strncpy(hmac_key_len,"32",2);
	 strcpy(HMAC_key, auth_tx.CAF01_details.csck);
	 strcpy (delimiter,";");
	 racal_Get_Sha256_Encryption( &Suppl_Amt);

	 if(strlen(auth_tx.TLF01_details.card_num) >= 14 )
	 {
		 memset(&dummy,0,Dummy_Size);
		 strncpy(dummy,auth_tx.TLF01_details.card_num,16);
	 }

	 if(strlen(auth_tx.TLF01_details.dcf01_retired_cwk) != 0  && strcmp(auth_tx.TLF01_details.nfi_end_date,"1" == 0))
	 {
	 		strncpy(cavv2_atn_Code,&auth_tx.TLF01_details.dcf01_retired_cwk[10],4);
	 		strncpy(cvv_value,&auth_tx.TLF01_details.dcf01_retired_cwk[0],4);
	 }
	 /*Build message and send to Racal.  */
			 memset (&output,0,sizeof(output));
			 strcat(output, msgHeader);
			 strcat(output, cmdCode);
			 strcat(output, "U");
			 strcat(output, HMAC_key);
			 strcat(output, Suppl_Amt);
			 //strcat(output, auth_tx.TLF01_details.card_num);
			 strcat(output, delimiter);
			 strncat(output, cavv2_atn_Code, 4);
			 strncat(output, &cvv_value[1], 3);
			 //strcat(output, "%");
			 //strcat(output, "02");
			 /*strcat(output, "19");*/


			 if (strlen(output) > 150)
			 {
				racal_log_message( 2, 2,"CAVV Cmd Msg too long.","racal_Verify_CAVV_for_3Dsecure2_VISA_Keyblock",1 );
				return(TRANSACTION_NOT_VERIFIED);
			 }
			 /*printf("\n racal input for 10.6 CW: %s \n", output);*/
			 sprintf(strTemp, " input of racal CW: %s", output);
			 racal_log_message( 2, 2,strTemp,"racalVerify_CAVV_for_3Dsecure",1 );
			 if (racalSendToPort(output) == TRANSACTION_VERIFIED)
			 {
			 sprintf(strTemp, " output of racal cmnd CW: %s", returnString);
			 racal_log_message( 2, 2,strTemp,"racalVerify_CAVV_for_3Dsecure",1 );
			 		/*parse the message check for the message header.*/
			 		memset(&dummy,0,Dummy_Size);
			 		strncpy(dummy,returnString,4);
			 		if(strcmp(dummy,msgHeader) != 0)
			 		{
			 		   racal_log_message( 2, 2,"CAVV Msg Header mis-match.","racal_Verify_CAVV_for_3Dsecure2_VISA_Keyblock",1 );
			 		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
			 		}

			 		/*parse the message check for the response code.*/
			 		memset(&dummy,0,Dummy_Size);
			 		strncpy(dummy,returnString + 4, 2);
			 		if(strcmp(dummy,"CX") !=0)
			 		{
			 		   racal_log_message( 2, 2,"CAVV Cmd Msg mis-match.","racal_Verify_CAVV_for_3Dsecure2_VISA_Keyblock",1 );
			 		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
			 		}

			 		/* Parse the message check for the error code. */
			 		memset(&dummy,0,Dummy_Size);
			 		strncpy(dummy,returnString + 6, 2);
			 		if(strcmp(dummy,"00") == 0)
			 		{
			 		  /* copy Cvv to check at txcntl */
			 		 strncpy(auth_tx.conv_date, returnString + 8, 3);
			 		  return (TRANSACTION_VERIFIED); /*cavv valid*/
			 		}

			 		if(strcmp(dummy,"01") == 0)
			 		{
			 		   racal_log_message( 2, 2,"CAVV-VERIFY failure.","racal_Verify_CAVV_for_3Dsecure2_VISA_Keyblock",1 );
			 		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
			 		}
			 		if(strcmp(dummy,"10") == 0)
			 		{
			 			racal_log_message( 2, 2,"CAVV-VERIFY CVK A or B parity error.","racal_Verify_CAVV_for_3Dsecure2_VISA_Keyblock",1 );
			 			return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
			 		}

			 		if(strcmp(dummy,"12") == 0)
			 		{
			 		   racal_log_message( 2, 2,"CAVV-VERIFY No Key loaded in user storage.","racal_Verify_CAVV_for_3Dsecure2_VISA_Keyblock",1 );
			 		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
			 		}

			 		if(strcmp(dummy,"13") == 0)
			 		{
			 		   racal_log_message( 2, 2,"CAVV-VERIFY LMK error report to superviser.","racal_Verify_CAVV_for_3Dsecure2_VISA_Keyblock",1 );
			 		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
			 		}

			 		if(strcmp(dummy,"15") == 0)
			 		{
			 		   racal_log_message( 2, 2,"CAVV-VERIFY Error in input data.","racal_Verify_CAVV_for_3Dsecure2_VISA_Keyblock",1 );
			 		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
			 		}

			 		if(strcmp(dummy,"21") == 0)
			 		{
			 		   racal_log_message( 2, 2,"CAVV-VERIFY Invalid user storage index.","racal_Verify_CAVV_for_3Dsecure2_VISA_Keyblock",1 );
			 		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
			 		}

			 }
			 else  /* else for: if (racalSendToPort(output) == TRANSACTION_VERIFIED)*/
			 {
			 			   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
			 }

			 return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/

}/*racal_Verify_CAVV_for_3Dsecure2_VISA_Keyblock*/

/******************************************************/

int racal_Verify_CAVV_for_3Dsecure2_DCI_Keyblock()
{
	char msgHeader[5] = {0};
	char cmdCode[3] = {0};
	char cvv_value[5] = {0};
	char cavv2_atn_Code[5] = {0};
	char delimiter[2] = {0};
	char output[513]= {0};
	char strTemp[512] = {0};

	BYTE HMAC_key[129];
	CHAR hmac_key_len[4] = {0};

	memset (&returnString,0,ReturnString_Size);

	strcpy (msgHeader ,"1234");
	strcpy (cmdCode,"CW");
	strcpy (delimiter,";");

	if(strlen(caf01_keyblock.Dci_3ds_Keyblock) > 0)
	{
		strcat(HMAC_key,caf01_keyblock.Dci_3ds_Keyblock);
	}
	else
	{
		racal_log_message( 2, 2, "DCI 3DS2 HSM key is blank.","racal_Verify_CAVV_for_3Dsecure2_DCI_Keyblock",1 );
		return(TRANSACTION_NOT_VERIFIED);
	}

	if(strlen(auth_tx.TLF01_details.dcf01_retired_cwk) != 0)
	{
		strncpy(cavv2_atn_Code,&auth_tx.TLF01_details.dcf01_retired_cwk[12],4);
		strncpy(cvv_value,&auth_tx.TLF01_details.dcf01_retired_cwk[2],4);
	}

	/*Build message and send to Racal.  */
	strcat (output, msgHeader);
	strcat (output, cmdCode);
	strncat(output,SOUURCE_TPK_KEYBLOCK,KEYBLOCK_KEY_FORMAT_LEN);
	strcat (output, HMAC_key);
	strcat (output, auth_tx.TLF01_details.card_num);
	strcat (output, delimiter);
	strncat(output, cavv2_atn_Code, 4);
	strncat(output, &cvv_value[1], 3);

	if (strlen(output) > MAX_KEYBLOCK_CMD_LEN)
	{
		racal_log_message( 2, 2,"CAVV Cmd Msg too long.","racal_Verify_CAVV_for_3Dsecure2_DCI_Keyblock",1 );
		return(TRANSACTION_NOT_VERIFIED);
	}

	sprintf(strTemp, " DCI 3ds2 to racal CW: %s", output);
	racal_log_message( 2, 2,strTemp,"racal_Verify_CAVV_for_3Dsecure2_DCI_Keyblock",1 );

	if (racalSendToPort(output) == TRANSACTION_VERIFIED)
	{
		sprintf(strTemp, " output of racal cmnd CW: %s", returnString);
		racal_log_message( 2, 2,strTemp,"racal_Verify_CAVV_for_3Dsecure2_DCI_Keyblock",1 );

		/*parse the message check for the message header.*/
		memset(&dummy,0,Dummy_Size);
		strncpy(dummy,returnString,4);
		if(strcmp(dummy,msgHeader) != 0)
		{
			racal_log_message( 2, 2,"CAVV Msg Header mis-match.","racal_Verify_CAVV_for_3Dsecure2_DCI_Keyblock",1 );
			return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
		}

		memset(&dummy,0,Dummy_Size);
		strncpy(dummy,returnString + 4, 2);
		if(strcmp(dummy,"CX") !=0)
		{
			racal_log_message( 2, 2,"CAVV Cmd Msg mis-match.","racal_Verify_CAVV_for_3Dsecure2_DCI_Keyblock",1 );
			return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
		}

		memset(&dummy,0,Dummy_Size);
		strncpy(dummy,returnString + 6, 2);
		if(strcmp(dummy,"00") == 0)
		{
			strncpy(auth_tx.conv_date, returnString + 8, 3);
			return (TRANSACTION_VERIFIED);
		}
		else if(strcmp(dummy,"01") == 0)
		{
			racal_log_message( 2, 2,"CAVV-VERIFY failure.","racal_Verify_CAVV_for_3Dsecure2_DCI_Keyblock",1 );
			return (TRANSACTION_NOT_VERIFIED);
		}
		else if(strcmp(dummy,"10") == 0)
		{
			racal_log_message( 2, 2,"CAVV-VERIFY CVK A or B parity error.","racal_Verify_CAVV_for_3Dsecure2_DCI_Keyblock",1 );
			return (TRANSACTION_NOT_VERIFIED);
		}
		else if(strcmp(dummy,"12") == 0)
		{
			racal_log_message( 2, 2,"CAVV-VERIFY No Key loaded in user storage.","racal_Verify_CAVV_for_3Dsecure2_DCI_Keyblock",1 );
			return (TRANSACTION_NOT_VERIFIED);
		}
		else if(strcmp(dummy,"13") == 0)
		{
			racal_log_message( 2, 2,"CAVV-VERIFY LMK error report to superviser.","racal_Verify_CAVV_for_3Dsecure2_DCI_Keyblock",1 );
			return (TRANSACTION_NOT_VERIFIED);
		}
		else if(strcmp(dummy,"15") == 0)
		{
			racal_log_message( 2, 2,"CAVV-VERIFY Error in input data.","racal_Verify_CAVV_for_3Dsecure2_DCI_Keyblock",1 );
			return (TRANSACTION_NOT_VERIFIED);
		}
		else if(strcmp(dummy,"21") == 0)
		{
			racal_log_message( 2, 2,"CAVV-VERIFY Invalid user storage index.","racal_Verify_CAVV_for_3Dsecure2_DCI_Keyblock",1 );
			return (TRANSACTION_NOT_VERIFIED);
		}
	}
	else
	{
		return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
	}

	return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/

}/*racal_Verify_CAVV_for_3Dsecure2_DCI_Keyblock*/
/************************************************/

int racal_Verify_CAVV_for_3Dsecure3_JCB_Keyblock()
{
   char msgHeader[5] = {0};
   char Command[3] = {0};
   char Delimiter[2] = {0};
   char output[200]= {0};
   BYTE CAVV_key[33]= {0};
   char cardType = 'Z';
   char Supplement_Data[50] = {0};
   char strTemp[512] = {0};


   memset (&msgHeader,0,sizeof(msgHeader));
   memset (&Command,0,sizeof(Command));
   memset (&Delimiter,0,sizeof(Delimiter));
   memset (&output,0,sizeof(output));
   memset (&returnString,0,ReturnString_Size);





   strcpy (msgHeader ,"1234");

   strcpy (Command,"CW");


   	 if(strlen(auth_tx.CAF01_details.csck) != 0)
	 {
		 strcpy(CAVV_key, auth_tx.CAF01_details.csck);
	 }
	 else
	 {
		racal_log_message( 2, 2, "JCB 3DS2 HSM key is blank.","racal_Verify_CAVV_for_3Dsecure3_JCB_Keyblock",1 );
		return(TRANSACTION_NOT_VERIFIED);
	 }


	 strcpy (Delimiter,";");
	 Racal_JCB_Input( &Supplement_Data);


	 /*if(strlen(auth_tx.TLF01_details.card_num) >= 14 )
	 {
		 memset(&dummy,0,Dummy_Size);
		 strncpy(dummy,auth_tx.TLF01_details.card_num,16);
	 }*/

	 /*Build message and send to Racal.  */
			 memset (&output,0,sizeof(output));
			 strcat(output, msgHeader);
			 strcat(output, Command);
			 strcat(output, "U");   //keyTag
			 strcat(output, CAVV_key);
		     if (strlen(auth_tx.TLF01_details.card_num) != 0)
			 {
				strcat(output,auth_tx.TLF01_details.card_num);
			 }
			 strcat(output, Delimiter);
			 strcat(output, Supplement_Data);


			 if (strlen(output) > 150)
			 {
				racal_log_message( 2, 2,"CAVV Cmd Msg too long.","racal_Verify_CAVV_for_3Dsecure3_JCB_Keyblock",1 );
				return(TRANSACTION_NOT_VERIFIED);
			 }
			 /*printf("\n racal input for 10.6 CW: %s \n", output);*/
			 sprintf(strTemp, " input of racal CW: %s", output);
			 racal_log_message( 2, 2,strTemp,"racal_Verify_CAVV_for_3Dsecure_JCB_Keyblock",1 );
			 if (racalSendToPort(output) == TRANSACTION_VERIFIED)
			 {
			 sprintf(strTemp, " output of racal cmnd CW: %s", returnString);
			 racal_log_message( 2, 2,strTemp,"racal_Verify_CAVV_for_3Dsecure_JCB_Keyblock",1 );
			 		/*parse the message check for the message header.*/
			 		memset(&dummy,0,Dummy_Size);
			 		strncpy(dummy,returnString,4);
			 		if(strcmp(dummy,msgHeader) != 0)
			 		{
			 		   racal_log_message( 2, 2,"CAVV Msg Header mis-match.","racal_Verify_CAVV_for_3Dsecure3_JCB_Keyblock",1 );
			 		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
			 		}

			 		/*parse the message check for the response code.*/
			 		memset(&dummy,0,Dummy_Size);
			 		strncpy(dummy,returnString + 4, 2);
			 		if(strcmp(dummy,"CX") !=0)
			 		{
			 		   racal_log_message( 2, 2,"CAVV Cmd Msg mis-match.","racal_Verify_CAVV_for_3Dsecure3_JCB_Keyblock",1 );
			 		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
			 		}

			 		/* Parse the message check for the error code. */
			 		memset(&dummy,0,Dummy_Size);
			 		strncpy(dummy,returnString + 6, 2);
			 		if(strcmp(dummy,"00") == 0)
			 		{
			 		  /* copy Cvv to check at txcntl */
			 		 strncpy(auth_tx.conv_date, returnString + 8, 3);
			 		  return (TRANSACTION_VERIFIED); /*cavv valid*/
			 		}

			 		if(strcmp(dummy,"01") == 0)
			 		{
			 		   racal_log_message( 2, 2,"CAVV-VERIFY failure.","racal_Verify_CAVV_for_3Dsecure3_JCB_Keyblock",1 );
			 		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
			 		}
			 		if(strcmp(dummy,"10") == 0)
			 		{
			 			racal_log_message( 2, 2,"CAVV-VERIFY CVK A or B parity error.","racal_Verify_CAVV_for_3Dsecure3_JCB_Keyblock",1 );
			 			return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
			 		}

			 		if(strcmp(dummy,"12") == 0)
			 		{
			 		   racal_log_message( 2, 2,"CAVV-VERIFY No Key loaded in user storage.","racal_Verify_CAVV_for_3Dsecure3_JCB_Keyblock",1 );
			 		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
			 		}

			 		if(strcmp(dummy,"13") == 0)
			 		{
			 		   racal_log_message( 2, 2,"CAVV-VERIFY LMK error report to superviser.","racal_Verify_CAVV_for_3Dsecure3_JCB_Keyblock",1 );
			 		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
			 		}

			 		if(strcmp(dummy,"15") == 0)
			 		{
			 		   racal_log_message( 2, 2,"CAVV-VERIFY Error in input data.","racal_Verify_CAVV_for_3Dsecure3_JCB_Keyblock",1 );
			 		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
			 		}

			 		if(strcmp(dummy,"21") == 0)
			 		{
			 		   racal_log_message( 2, 2,"CAVV-VERIFY Invalid user storage index.","racal_Verify_CAVV_for_3Dsecure3_JCB_Keyblock",1 );
			 		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
			 		}

			 }
			 else  /* else for: if (racalSendToPort(output) == TRANSACTION_VERIFIED)*/
			 {
			 			   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
			 }

			 return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/

}/*racal_Verify_CAVV_for_3Dsecure3_JCB_Keyblock*/

/***************************************/

int racal_Verify_CAVV_for_3Dsecure2_UPI_Keyblock()
{
   char msgHeader[5] = {0};
   char Command[3] = {0};
   char cvv_value[5] = {0};
   char cavv2_atn_Code[5] = {0};
   char Delimiter[2] = {0};
   char output[200]= {0};
   char cardType = 'Z';
   char csc[6] = {0};
   char Supplement_Data[50] = {0};
   char strTemp[512] = {0};
   BYTE CAVV_key[33]= {0};


   memset (&msgHeader,0,sizeof(msgHeader));
   memset (&Command,0,sizeof(Command));
   memset (&Delimiter,0,sizeof(Delimiter));
   memset (&output,0,sizeof(output));
   memset (&returnString,0,ReturnString_Size);
   memset (&csc,0,sizeof(csc));

   strcpy (msgHeader ,"1234");

   strcpy (Command,"CW");

   	 strcpy (Delimiter,";");

   	 if(strlen(auth_tx.CAF01_details.csck) != 0)
	 {
		 strcpy(CAVV_key, auth_tx.CAF01_details.csck);
	 }
	 else
	 {
		racal_log_message( 2, 2, "JCB 3DS2 HSM key is blank.","racal_Verify_CAVV_for_3Dsecure2_UPI_Keyblock",1 );
		return(TRANSACTION_NOT_VERIFIED);
	 }

	 strcpy (Delimiter,";");
	 Racal_CUP_Input( &Supplement_Data);


	 /*Build message and send to Racal.  */
			 memset (&output,0,sizeof(output));
			 strcat(output, msgHeader);
			 strcat(output, Command);
			 strcat(output, "U");
			 strcat(output, CAVV_key);
		     if (strlen(auth_tx.TLF01_details.card_num) != 0)
			 {
				strcat(output,auth_tx.TLF01_details.card_num);
			 }
			 strcat(output, Delimiter);

			 strcat(output, Supplement_Data);



			 if (strlen(output) > 150)
			 {
				racal_log_message( 2, 2,"CAVV Cmd Msg too long.","racal_Verify_CAVV_for_3Dsecure2_UPI_Keyblock",1 );
				return(TRANSACTION_NOT_VERIFIED);
			 }
			 /*printf("\n racal input for 10.6 CW: %s \n", output);*/
			 sprintf(strTemp, " input of racal CW: %s", output);
			 racal_log_message( 2, 2,strTemp,"racal_Verify_CAVV_for_3Dsecure2_UPI_Keyblock",1 );
			 if (racalSendToPort(output) == TRANSACTION_VERIFIED)
			 {
			 sprintf(strTemp, " output of racal cmnd CW: %s", returnString);
			 racal_log_message( 2, 2,strTemp,"racal_Verify_CAVV_for_3Dsecure2_UPI_Keyblock",1 );
			 		/*parse the message check for the message header.*/
			 		memset(&dummy,0,Dummy_Size);
			 		strncpy(dummy,returnString,4);
			 		if(strcmp(dummy,msgHeader) != 0)
			 		{
			 		   racal_log_message( 2, 2,"CAVV Msg Header mis-match.","racal_Verify_CAVV_for_3Dsecure2_UPI_Keyblock",1 );
			 		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
			 		}

			 		/*parse the message check for the response code.*/
			 		memset(&dummy,0,Dummy_Size);
			 		strncpy(dummy,returnString + 4, 2);
			 		if(strcmp(dummy,"CX") !=0)
			 		{
			 		   racal_log_message( 2, 2,"CAVV Cmd Msg mis-match.","racal_Verify_CAVV_for_3Dsecure2_UPI_Keyblock",1 );
			 		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
			 		}

			 		/* Parse the message check for the error code. */
			 		memset(&dummy,0,Dummy_Size);
			 		strncpy(dummy,returnString + 6, 2);
			 		if(strcmp(dummy,"00") == 0)
			 		{
			 		  /* copy Cvv to check at txcntl */
			 		 strncpy(auth_tx.conv_date, returnString + 8, 3);
			 		  return (TRANSACTION_VERIFIED); /*cavv valid*/
			 		}

			 		if(strcmp(dummy,"01") == 0)
			 		{
			 		   racal_log_message( 2, 2,"CAVV-VERIFY failure.","racal_Verify_CAVV_for_3Dsecure2_UPI_Keyblock",1 );
			 		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
			 		}
			 		if(strcmp(dummy,"10") == 0)
			 		{
			 			racal_log_message( 2, 2,"CAVV-VERIFY CVK A or B parity error.","racal_Verify_CAVV_for_3Dsecure2_UPI_Keyblock",1 );
			 			return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
			 		}

			 		if(strcmp(dummy,"12") == 0)
			 		{
			 		   racal_log_message( 2, 2,"CAVV-VERIFY No Key loaded in user storage.","racal_Verify_CAVV_for_3Dsecure2_UPI_Keyblock",1 );
			 		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
			 		}

			 		if(strcmp(dummy,"13") == 0)
			 		{
			 		   racal_log_message( 2, 2,"CAVV-VERIFY LMK error report to superviser.","racal_Verify_CAVV_for_3Dsecure2_UPI_Keyblock",1 );
			 		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
			 		}

			 		if(strcmp(dummy,"15") == 0)
			 		{
			 		   racal_log_message( 2, 2,"CAVV-VERIFY Error in input data.","racal_Verify_CAVV_for_3Dsecure2_UPI_Keyblock",1 );
			 		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
			 		}

			 		if(strcmp(dummy,"21") == 0)
			 		{
			 		   racal_log_message( 2, 2,"CAVV-VERIFY Invalid user storage index.","racal_Verify_CAVV_for_3Dsecure2_UPI_Keyblock",1 );
			 		   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
			 		}

			 }
			 else  /* else for: if (racalSendToPort(output) == TRANSACTION_VERIFIED)*/
			 {
			 			   return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/
			 }

			 return (TRANSACTION_NOT_VERIFIED); /*cavv invalid*/

}/*racal_Verify_CAVV_for_3Dsecure2_UPI_Keyblock*/

/*****************************************************/

int racal_Verify_CAVV_for_3Dsecure2_AMEX_Keyblock()
{

	   char msgHeader[5]={0};
	   char cmdCode[3]={0};
	   char cvc1_serviceCode[4]={0};
	   char cvc2_serviceCode[4]={0};
	   char delimiter[2]={0};
	   char output[200]={0};
	   char cardType = 'Z';
	   char csc[6]={0};
	   char cvc1_data[4]={0};
	   char cvc2_data[4]={0};
	   int  track_present = 0;
	   int  cvc2_present = 0;
	   int  cvc2_sent = 0;
	   int  loopidx = 0;
	   int  cnt=0;
	   int  result=0;
	   int  jcnt=0;
	   char strTemp [256]={0};
	   CHAR *tempptr;
	   CHAR fieldSeperator[2]={0};
	   CHAR csc_result5[6]={0};
	   CHAR csc_result4[5]={0};
	   CHAR csc_result3[4]={0};
	   INT 	flag=0;
	   INT  AEVV_result_code;
	   CHAR AEVV_second_factor[3]={0};
	   CHAR AEVV_bin_second_factor[2];
	   CHAR AEVV_iCSC[5]={0};
	   CHAR AEVV_bin_iCSC[3];
	   CHAR AEVV_UPN[5]={0};
	   CHAR AEVV_bin_UPN[3];
	   INT  temp_out_len=0;

	   memset (&msgHeader,0,sizeof(msgHeader));
	   memset (&cmdCode,0,sizeof(cmdCode));
	   memset (&delimiter,0,sizeof(delimiter));
	   memset (&output,0,sizeof(output));
	   memset (&returnString,0,ReturnString_Size);
	   memset (&csc,0,sizeof(csc));

	   strcpy (msgHeader ,"1234");


	         /***********************Begin AMEX*********************************/
	         strcat(output,msgHeader);
	         strcat(output,"RY"); /* Changed Command code RY4- TF Phani*/
	         strcat(output,"3");
	         strcat(output,"3");

	         if(strlen(auth_tx.CAF01_details.cvk[3].cvkA) != 0 &&
	            strlen(auth_tx.CAF01_details.cvk[3].cvkB) != 0 	 )
			 {
				strcat(output,"U");
				strcat(output,auth_tx.CAF01_details.cvk[3].cvkA);
				strcat(output,auth_tx.CAF01_details.cvk[3].cvkB);

			 }
			 else
			 {
				racal_log_message( 2, 2, "AMEX 3DS2 HSM key is blank.","racal_Verify_CAVV_for_3Dsecure2_AMEX_Keyblock",1 );
				return(TRANSACTION_NOT_VERIFIED);
			 }

	         if (strlen(auth_tx.TLF01_details.card_num) != 0)
			 {
				strcat(output,auth_tx.TLF01_details.card_num);
				if(strlen(auth_tx.TLF01_details.card_num) != 19)
				   for(cnt = 19 ; (INT)strlen(auth_tx.TLF01_details.card_num) < cnt ;cnt--)
					  strcat(output,"0");
			 }
			 else
			 {
				racal_log_message( 2, 2, "AMEX 3DS2 ,card_num blank.","racal_Verify_CAVV_for_3Dsecure2_AMEX_Keyblock",1 );
				return(TRANSACTION_NOT_VERIFIED);
			 }

	         strncat(output, auth_tx.TLF01_details.dcf01_retired_cwk+26, 4);

	         //Copy Authn Result code and SF Authn code
	         strncat(output, &auth_tx.TLF01_details.dcf01_retired_cwk[1], 3);

	         //if the output is bigger then 150 do not send it out.
	         if (strlen(output) > 150)
	         {
	            racal_log_message( 2, 2, "AMEX 3DS2 HSM Cmd Msg to long.","racal_Verify_CAVV_for_3Dsecure2_AMEX_Keyblock",1 );
	            return(TRANSACTION_NOT_VERIFIED);
	         }

	         sprintf(strTemp, " AMEX to racal RY: %s", output);
	         racal_log_message( 2, 2,strTemp,"racal_Verify_CAVV_for_3Dsecure2_AMEX_Keyblock",1 );
	         //send the message to the racal box.
	         if (racalSendToPort(output) == TRANSACTION_VERIFIED)
	         {
	        	sprintf(strTemp, " output of racal cmnd RZ: %s", returnString);
	        	racal_log_message( 2, 2,strTemp,"racal_Verify_CAVV_for_3Dsecure2_AMEX_Keyblock",1 );

	        	/*parse the message check for the message header.*/
	            memset(&dummy,0,Dummy_Size);
	            strncpy(dummy,returnString,4);
	            if(strcmp(dummy,msgHeader) != 0)
	            {
	               racal_log_message( 2, 2, "AMEX 3DS2 HSM Cmd header mis-match.","racal_Verify_CAVV_for_3Dsecure2_AMEX_Keyblock",1 );
	               return(TRANSACTION_NOT_VERIFIED);
	            }
	            /*parse the message check for the response code.*/
	            memset(&dummy,0,Dummy_Size);
	            strncpy(dummy,returnString + 4, 2);
	            if(strcmp(dummy,"RZ") !=0)
	            {
	               racal_log_message( 2, 2, "AMEX 3DS2 HSM Cmd Code mis-match.","racal_Verify_CAVV_for_3Dsecure2_AMEX_Keyblock",1 );
	               return(TRANSACTION_NOT_VERIFIED);
	            }

	            /*parse the message check for the error code.*/
	            memset(&dummy,0,Dummy_Size);
	            strncpy(dummy,returnString + 6, 2);

				printf("Return from Racal : %s \n",returnString);
	            //if this value is not 00 reject the transaction.
	            if(strcmp(dummy,"00") != 0)
	            {
	               if(strcmp(dummy,"10") == 0)
	               {
	            	  racal_log_message( 2, 2, "AMEX 3DS2 HSM CSCK parity error.","racal_Verify_CAVV_for_3Dsecure2_AMEX_Keyblock",1 );
				   }
	               if(strcmp(dummy,"12") == 0)
	                  racal_log_message( 2, 2, "AMEX 3DS2 HSM No Keys in user storage.","racal_Verify_CAVV_for_3Dsecure2_AMEX_Keyblock",1 );
	               if(strcmp(dummy,"13") == 0)
	                  racal_log_message( 2, 2, "AMEX 3DS2 HSM LMK parity error.","racal_Verify_CAVV_for_3Dsecure2_AMEX_Keyblock",1 );
	               if(strcmp(dummy,"15") == 0)
	                  racal_log_message( 2, 2, "AMEX 3DS2 HSM Error in input data.","racal_Verify_CAVV_for_3Dsecure2_AMEX_Keyblock",1 );
				   if(strcmp(dummy,"01") == 0)
				   	{
					   strcpy(auth_tx.TLF01_details.response_text,"Invalid 4 CSC");
				   	   racal_log_message( 2, 2, "Invalid CSC/CVV/3D sec.","racal_Verify_CAVV_for_3Dsecure2_AMEX_Keyblock",1 );
				   	}
	// Invalid      CSC           01
	//INvalid  27 CSCK not doubel lenght
	// Invalid  68 HSM command disabled

	               return(TRANSACTION_NOT_VERIFIED);
	            }
	            else if(strcmp(dummy,"00") == 0)
				{
				  /* copy avop to check at txcntl */
				  strncpy(auth_tx.conv_date, returnString + 9, 3);
				  return (TRANSACTION_VERIFIED); /*cavv valid*/
				}

	#ifdef _DEBUG
	            racal_log_message( 2, 1, "mag-strip PASSED.","racal_Verify_CAVV_for_3Dsecure2_AMEX_Keyblock",1 );
	#endif
	            return(TRANSACTION_VERIFIED);

	         }/*end of if racalSendToPort*/
	         else
	            return(TRANSACTION_NOT_VERIFIED);


	         /****************************end AMEX***************************************/


}/*racal_Verify_CAVV_for_3Dsecure2_AMEX_Keyblock*/

/*********************************************************/

INT racal_Verify_IAV_for_3Dsecure2_MC_Keyblock()
{
	char output[526]	= {0};
	char msgHeader[5]   = {0};
	INT  key_length = 0;
	INT  bin_key_length= 0;
	char skeylen[5]		= {0};
	INT  data_length = 0;
	INT  bin_data_length	= 0;
	char sdatalen[5]	= {0};
	char outputloger[526]={0};
	char strTemp[1024]   = {0};
	int  i=0;
	char hsm_generated_IAV[20]={0};
	char ascii_hsm_generated_IAV[33]={0};
	char key[81]={0};
	char data[100]={0};
	char bin_key[128]={0};
	char bin_data[128]={0};
	int  return_code=0;
	int  len_data=0;
	int  bin_hmac_len=0;
	int  ascii_hmac_len=0;
	char temp_rc[3]={0};
	INT  de48_retcode= 0;
	INT  hmac_ret_val=1;
	INT  ucaf_data_64_len =0;
	BYTE UCAF_data[33]={0};
	INT  ucaf_data_len = 0;
	INT  astr_len = 0;
	char cvv_value[9] = {0};
	int  De48len=0;
	BYTE DE48_data[256]={0};
	BYTE UCAF_data_ascii[41]={0};
	BYTE UCAF_data_bin[41]={0};

	memset (msgHeader,0x00,sizeof(msgHeader));
	memset (&output,0x00,sizeof(output));
	memset (&returnString,0x00,ReturnString_Size);

	/* Default msg header*/
	strncpy(msgHeader,"1234",4);
	strncpy (output ,msgHeader,4);
	len_data +=4;
	/* CMD to build the IAV*/
	strncat (output,"LQ",2);
	len_data +=2;
	/* hash identifier for LMK 06 for SHA-256*/
	strncat(output,"06",2);
	len_data +=2;
	/*Output length expected by HSM*/
	strncat(output,"0016",4);
	len_data +=4;
	/*HMAC key format*/
	strncat(output,"00",2);
	len_data +=2;
	/*HMAC key length and KEY */
	key_length= strlen(auth_tx.EMV_details.future_field2);

	if(key_length == 0)
	{
		racal_log_message( 2, 2,"Key is not available.","racal_Verify_IAV_for_3Dsecure2_MC_Keyblock",1 );
	    return (TRANSACTION_NOT_VERIFIED);
	}

	strncpy(key,auth_tx.EMV_details.future_field2,key_length);
	sprintf(strTemp, "key used for MC 3DS 2.0 %s", key);
	racal_log_message( 2, 2,strTemp,"racal_Verify_IAV_for_3Dsecure2_MC_Keyblock",1 );
	/*convert the key into binary and copy the binary lenght into the command*/
	genutil_asc_to_bin(key,bin_key,&bin_key_length);
	sprintf(skeylen,"%04d",bin_key_length);
	strncat(output,skeylen,4);
	len_data +=4;
	strncat(output,bin_key ,bin_key_length);
	len_data +=bin_key_length;
	/*Delimiter*/
	strncat(output,";",1);
	len_data +=1;
	/*Data length*/
	data_length= strlen(auth_tx.EMV_details.future_field1);

	if(data_length == 0)
	{
		racal_log_message( 2, 2,"Data is not available.","racal_Verify_IAV_for_3Dsecure2_MC_Keyblock",1 );
		return (TRANSACTION_NOT_VERIFIED);
	}
	/*Data*/
	strncat(data,auth_tx.EMV_details.future_field1,data_length);
	genutil_asc_to_bin(data,bin_data,&bin_data_length);

	sprintf(sdatalen,"%05d",bin_data_length);
	strncat(output,sdatalen,5);
	len_data +=5;

	memcpy(&output[len_data],bin_data,bin_data_length);

	len_data+=bin_data_length;
	sprintf(strTemp, "racal input for MC 3DS 2.0 %s", output);
	racal_log_message( 2, 2,strTemp,"racal_Verify_IAV_for_3Dsecure2_MC_Keyblock",1 );
	if (racalSendToPort_MC(output,len_data) == TRANSACTION_VERIFIED)
	{
		memset(&dummy,0,Dummy_Size);
		strncpy(dummy,returnString,4);
		if(strcmp(dummy,msgHeader) != 0)
		{
			   racal_log_message( 2, 2,"CAVV Msg Header mis-match.","racal_Verify_IAV_for_3Dsecure2_MC_Keyblock",1 );
			   return (TRANSACTION_NOT_VERIFIED);
		}

		/*parse the message check for the response code.*/
		memset(&dummy,0,Dummy_Size);
		strncpy(dummy,returnString + 4, 2);
		if(strcmp(dummy,"LR") !=0)
		{
			sprintf(strTemp, "Resp Cmd Msg mis-match%s", dummy);
			racal_log_message( 2, 2,strTemp,"racal_Verify_IAV_for_3Dsecure2_MC_Keyblock",1 );
			return (TRANSACTION_NOT_VERIFIED);
		}

		/* Parse the message check for the error code. */
		memset(&dummy,0,Dummy_Size);
		strncpy(dummy,returnString + 6, 2);
		strncpy(temp_rc,dummy,2);
		if(strcmp(dummy,"00") != 0)
		{
			sprintf(strTemp, "Response code received from HSM %s", dummy);
			racal_log_message( 2, 2,strTemp,"racal_Verify_IAV_for_3Dsecure2_MC_Keyblock",1 );
			if('\0' != auth_tx.TLF01_details.cvc_data[0])
			{
				 racal_log_message( 2, 2,"CAVV failed, ATP trying to do CVV2 validation", "racal_Verify_IAV_for_3Dsecure2_MC_Keyblock", 1 );
				 return_code = racalEquitVerifyCVV_CVC_CSC_CAV();

				 if (return_code == TRANSACTION_NOT_VERIFIED)
				 {
					  strcpy(auth_tx.TLF01_details.security_response_code,"01");
					  auth_tx.TLF01_details.cvc = 'N';
				 }
				 else if (return_code == CVC2_TRANSACTION_NOT_VERIFIED)
				 {
					  strcpy(auth_tx.TLF01_details.security_response_code,"02");
					  auth_tx.TLF01_details.cvc = 'N';
				 }
				 else if (return_code == CVC2_TRANSACTION_VERIFIED)
				 {
					   strcpy(auth_tx.TLF01_details.security_response_code,"03");
					   auth_tx.TLF01_details.cvc = 'M';
				 }
				 else
				 {
					 auth_tx.TLF01_details.cvc = 'M';
					 strcpy( auth_tx.TLF01_details.security_response_code, "00" );
				 }
				 memset(strTemp,0x00,sizeof(strTemp));
				 sprintf( strTemp,"result code of CVV2 validation %d",return_code);
				 return return_code;
			}
			else
			{
				strcpy(auth_tx.TLF01_details.security_response_code,"00");
				racal_log_message( 2, 2,"CAVV validation failed and CVV2 data is not present go for BAU-case 5", "racal_Verify_IAV_for_3Dsecure2_MC_Keyblock", 1 );
				return (TRANSACTION_VERIFIED);
			}
		}


		memset(&dummy,0,Dummy_Size);
		strncpy(dummy,returnString + 8, 4);
		if(strncmp(dummy,"0016",4) !=0 )
		{
			sprintf(strTemp, "HMAC output length mismatch %s", dummy);
			racal_log_message( 2, 2,strTemp,"racal_Verify_IAV_for_3Dsecure2_MC_Keyblock",1 );
			return (TRANSACTION_NOT_VERIFIED);
		}

		memset(&dummy,0,Dummy_Size);
		strncpy(dummy,returnString + 12, 16);

		strncpy(hsm_generated_IAV,dummy,16);
		bin_hmac_len=16;
		genutil_bin_to_ascii (hsm_generated_IAV, bin_hmac_len, ascii_hsm_generated_IAV, ascii_hmac_len);

		if(auth_tx.EMV_details.issuer_script_2[0] == '\0')
		{
			De48len = auth_tx.future_use_3[0];
			memcpy(DE48_data,&auth_tx.future_use_3[1],De48len);
		}
		else
		{
			De48len = strlen(auth_tx.EMV_details.issuer_script_2);
			memcpy(DE48_data,auth_tx.EMV_details.issuer_script_2,strlen(auth_tx.EMV_details.issuer_script_2));
		}
		de48_retcode = parse_field_48( De48len,&DE48_data,&UCAF_data );
		ucaf_data_64_len =strlen(UCAF_data);
		ucaf_data_len = genutil_base64_to_binary( UCAF_data, strlen(UCAF_data),&UCAF_data_bin );
		genutil_bin_to_ascii (UCAF_data_bin, ucaf_data_len, UCAF_data_ascii, astr_len);
		strncpy(cvv_value,&UCAF_data_ascii[4],8);

		memset(strTemp,0x00,sizeof(strTemp));
		sprintf( strTemp,"CAVV out put from HSM(%s), CAVV generated from transaction(%s)\n",ascii_hsm_generated_IAV,cvv_value);
		racal_log_message( 2, 2,strTemp, "racal_Verify_IAV_for_3Dsecure2_MC_Keyblock", 1 );

		hmac_ret_val = strncmp(cvv_value,ascii_hsm_generated_IAV,8);
		if(hmac_ret_val == 0)
		{
			racal_log_message( 2, 2,"HSM generated CAVV and ATP generated CAVV does matched, ATP trying to do CVV2 validation", "racal_Verify_IAV_for_3Dsecure2_MC_Keyblock", 1 );
		}
		else
		{
			racal_log_message( 2, 2,"HSM generated CAVV and ATP generated CAVV does not match, ATP trying to do CVV2 validation", "racal_Verify_IAV_for_3Dsecure2_MC_Keyblock", 1 );
		}

		if('\0' != auth_tx.TLF01_details.cvc_data[0] )
		{
			 memset(strTemp,0x00,sizeof(strTemp));
			sprintf( strTemp,"SLI value(%s), hmac_ret_val(CAVV generated by ATP and HSM compared result value):(%d), \n",auth_tx.TLF01_details.profile,hmac_ret_val);
			racal_log_message( 2, 2,strTemp, "racal_Verify_IAV_for_3Dsecure2_MC_Keyblock", 1 );
			 return_code = racal_Verify_CVV_CVC_CSC_CAV_MC_Keyblock();

			 if (return_code == TRANSACTION_NOT_VERIFIED)
			 {
				  strcpy(auth_tx.TLF01_details.security_response_code,"01");
				  auth_tx.TLF01_details.cvc = 'N';
			 }
			 else if (return_code == CVC2_TRANSACTION_NOT_VERIFIED)
			 {
				  strcpy(auth_tx.TLF01_details.security_response_code,"02");
				  auth_tx.TLF01_details.cvc = 'N';
			 }
			 else if (return_code == CVC2_TRANSACTION_VERIFIED)
			 {
				   strcpy(auth_tx.TLF01_details.security_response_code,"03");
				   auth_tx.TLF01_details.cvc = 'M';
			 }
			 else
			 {
				 auth_tx.TLF01_details.cvc = 'M';
				 strcpy( auth_tx.TLF01_details.security_response_code, "00" );

			 }
			 memset(strTemp,0x00,sizeof(strTemp));
			 sprintf( strTemp,"result code of CVV2 validation %d",return_code);

			 return return_code;
		}
		else
		{
			if(hmac_ret_val == 0)
			{
				racal_log_message( 2, 2,"CAVV matched and no CVV2 present go for BAU case 6", "racal_Verify_IAV_for_3Dsecure2_MC_Keyblock", 1 );
			}
			else
			{
				racal_log_message( 2, 2,"CAVV does not match and no CVV2 present go for BAU case 5", "racal_Verify_IAV_for_3Dsecure2_MC_Keyblock", 1 );
			}

			strcpy(auth_tx.TLF01_details.security_response_code,"00");
			return (TRANSACTION_VERIFIED);
		}
	}
	else
	{
		racal_log_message( 2, 2,"CAVV verification send failed, could not send or receive the command to/from HSM", "racal_Verify_IAV_for_3Dsecure2_MC_Keyblock", 1 );
		strcpy(auth_tx.TLF01_details.security_response_code,"05");
	}
	return (TRANSACTION_NOT_VERIFIED);
}/*racal_Verify_IAV_for_3Dsecure2_MC_Keyblock*/

/***********************************************/

int racal_Verify_CVN2_for_UPI_Ecom_Keyblock()
{
	char msgHeader[5] = {0};
	char cmdCode[3] = {0};
	char cvn2_value[5] = {0};
	char delimiter[2] = {0};
	char output[200]= {0};
	BYTE temp_len[4] = {0};
	char BUFFER [256] = {0};
	int len = 0;
	memset (&msgHeader,0,sizeof(msgHeader));
	memset (&cmdCode,0,sizeof(cmdCode));
	memset (&delimiter,0,sizeof(delimiter));
	memset (&output,0,sizeof(output));

	strcpy (msgHeader ,"1234");
	strcpy (cmdCode,"CY");
	strcpy (delimiter,";");

	/*edit CVK A */
	if(strlen(auth_tx.CAF01_details.cvk[0].cvkA) == 0)
	{
		racal_log_message( 2, 2, "CVN2-validation cvkA is blank.","racal_Verify_CVN2_for_UPI_Ecom_Keyblock",1 );
		return(TRANSACTION_NOT_VERIFIED);
	}
	/*edit CVK B */
	if(strlen(auth_tx.CAF01_details.cvk[0].cvkB) == 0)
	{
		racal_log_message( 2, 2, "CVN2-validation cvkB is blank.","racal_Verify_CVN2_for_UPI_Ecom_Keyblock",1 );
		return(TRANSACTION_NOT_VERIFIED);
	}
	/*edit card number*/
	if(strlen(auth_tx.TLF01_details.card_num) == 0 )
	{
		racal_log_message( 2, 2,"CVN2-validation: card_num is blank.","racal_Verify_CVN2_for_UPI_Ecom_Keyblock",1 );
		return(TRANSACTION_NOT_VERIFIED);
	}
	/*edit expiration date*/
	if(strlen(auth_tx.TLF01_details.exp_date) == 0)
	{
		 racal_log_message( 2, 2, "CVN2-validation: exp_date is blank.","racal_Verify_CVN2_for_UPI_Ecom_Keyblock",1 );
		 return(TRANSACTION_NOT_VERIFIED);
	}
	// Lets get CVN2 value present in DE61.4
	memcpy( temp_len, auth_tx.TLF01_details.product_codes[13].code,CUP_DE61_LEN_SIZE);
	len = atoi(temp_len);
	if(len >= CUP_DE61_CVN2_TOTAL_LEN)
	{
		strncpy(cvn2_value,auth_tx.future_use_2 + CUP_DE61_CVN2_POS,CUP_CVN2_LEN);
	}

	/*Build message and send to Racal.  */
	memset(&output,0,sizeof(output));
	strcat(output, msgHeader);
	strcat(output, cmdCode);
	//strcat(output, "U");
	strcat(output, auth_tx.CAF01_details.cvk[0].cvkA);
	strcat(output, auth_tx.CAF01_details.cvk[0].cvkB);
	strncat(output, cvn2_value,3);

	strcat(output, auth_tx.TLF01_details.card_num);
	strcat(output, delimiter);
	strcat(output, auth_tx.TLF01_details.exp_date);
	strcat(output, "000");

	//if the output is bigger then 150 do not send it out.
	if (strlen(output) > 150)
	{
		racal_log_message( 2, 2,"CVN2 Cmd Msg too long.","racal_Verify_CVN2_for_UPI_Ecom_Keyblock",1 );
		return(TRANSACTION_NOT_VERIFIED);
	}

	memset(BUFFER,0,sizeof(BUFFER));
	sprintf(BUFFER, "BEFORE-PORT: %s", output);
	MYPRINT(BUFFER);

	/*The message is built.  Send it over to the Racal box.*/
	/*Once the a response is recieved, check for the message header,*/
	/*response code, and the error code.  If it does not exactly match,*/
	/*send TRANSACTION_NOT_VERIFIED back.*/
	if (racalSendToPort(output) == TRANSACTION_VERIFIED)
	{
		/*parse the message check for the message header.*/
		memset(&dummy,0,Dummy_Size);
		strncpy(dummy,returnString,4);
		if(strcmp(dummy,msgHeader) != 0)
		{
			racal_log_message( 2, 2,"CVN2 Msg Header mis-match.","racal_Verify_CVN2_for_UPI_Ecom_Keyblock",1 );
			return (TRANSACTION_NOT_VERIFIED); /*CVN2 invalid*/
		}

		/*parse the message check for the response code.*/
		memset(&dummy,0,Dummy_Size);
		strncpy(dummy,returnString + 4, 2);
		if(strcmp(dummy,"CZ") !=0)
		{
			racal_log_message( 2, 2,"CVN2 Cmd Msg mis-match.","racal_Verify_CVN2_for_UPI_Ecom_Keyblock",1 );
			return (TRANSACTION_NOT_VERIFIED); /*CVN2 invalid*/
		}

		/* Parse the message check for the error code. */
		memset(&dummy,0,Dummy_Size);
		strncpy(dummy,returnString + 6, 2);
		if(strcmp(dummy,"00") == 0)
		{
			return (TRANSACTION_VERIFIED); /*CVN2 valid*/
		}

		if(strcmp(dummy,"01") == 0)
		{
			racal_log_message( 2, 2,"CVN2-VERIFY failure.","racal_Verify_CVN2_for_UPI_Ecom_Keyblock",1 );
			return (TRANSACTION_NOT_VERIFIED); /*CVN2 invalid*/
		}

		if(strcmp(dummy,"10") == 0)
		{
			racal_log_message( 2, 2,"CVN2-VERIFY CVK A or B parity error.","racal_Verify_CVN2_for_UPI_Ecom_Keyblock",1 );
			return (TRANSACTION_NOT_VERIFIED); /*CVN2 invalid*/
		}

		if(strcmp(dummy,"12") == 0)
		{
			racal_log_message( 2, 2,"CVN2-VERIFY No Key loaded in user storage.","racal_Verify_CVN2_for_UPI_Ecom_Keyblock",1 );
			return (TRANSACTION_NOT_VERIFIED); /*CVN2 invalid*/
		}

		if(strcmp(dummy,"13") == 0)
		{
			racal_log_message( 2, 2,"CVN2-VERIFY LMK error report to superviser.","racal_Verify_CVN2_for_UPI_Ecom_Keyblock",1 );
			return (TRANSACTION_NOT_VERIFIED); /*CVN2 invalid*/
		}

		if(strcmp(dummy,"15") == 0)
		{
			racal_log_message( 2, 2,"CVN2-VERIFY Error in input data.","racal_Verify_CVN2_for_UPI_Ecom_Keyblock",1 );
			return (TRANSACTION_NOT_VERIFIED); /*CVN2 invalid*/
		}

		if(strcmp(dummy,"21") == 0)
		{
			racal_log_message( 2, 2,"CVN2-VERIFY Invalid user storage index.","racal_Verify_CVN2_for_UPI_Ecom_Keyblock",1 );
			return (TRANSACTION_NOT_VERIFIED); /*CVN2 invalid*/
		}

	}
	else
	{
		return (TRANSACTION_NOT_VERIFIED); /*CVN2 invalid*/
	}

	return (TRANSACTION_NOT_VERIFIED); /*CVN2 invalid*/

}/*racal_Verify_CVN2_for_UPI_Ecom_Keyblock*/

/********************************************/

/****************************************************************/
INT racal_KeyExchange_Key_for_MC_Keyblock()
{
   INT   ret_val = TRANSACTION_NOT_VERIFIED;
   INT   len = 0;
   INT   i = 0;
   INT   racal_ret_code = 0;
   CHAR  key_len_indicator[2] = {0};
   CHAR  errbuf[100]  = {0};
   CHAR  msgHeader[5]= {0};
   CHAR  cmdCode[3]= {0};
   CHAR  output[200]= {0};

   memset( &output,       0x00, sizeof(output)    );
   memset( &returnString, 0x00, ReturnString_Size );

   strcpy( msgHeader ,"8543" );
   strcpy( cmdCode,   "FA"   );

   strcat( output, msgHeader );
   strcat( output, cmdCode   );

   len = strlen( ZMK );
   if ( len < 16)
   {
      strcpy( errbuf, "Key Change: ZMK is < 16." );
   }
   else
   {
      switch( len )
      {
         case 32:
        	 strcat( output, DOUBLE_LEN_KEY );
        	 break;
         case 48:
        	 strcat( output, TRIPLE_LEN_KEY );
        	 break;
         default:
			 strcat( output, TRIPLE_LEN_KEY );
			 break;
      }
      strcat( output, ZMK );

      len = strlen( auth_tx.TLF01_details.source_key );
      if ( len <= 0 )
      {
         strcpy( errbuf, "Key Change: Source Key (ZPK) is blank" );
      }
      else
      {
         switch( len )
         {
            case 32:
            	strcat( output, DOUBLE_LEN_KEY_X9_17 );
            	break;
            case 48:
            	strcat( output, TRIPLE_LEN_KEY_X9_17 );
            	break;
            default:
				strcat( output, TRIPLE_LEN_KEY_X9_17 );
				break;
         }
         strcat(output,auth_tx.TLF01_details.source_key);

         /* The command is now composed. Check the length then send it. */
         if ( strlen(output) > MAX_CMD_LEN )
         {
            sprintf( errbuf,
                    "Key Change: Cmd Msg too long: %d, max = %d",
                     len, MAX_CMD_LEN );
         }

         /* Send a message to the Racal box. */
         else if ( racalSendToPort(output) == TRANSACTION_VERIFIED )
         {
            /* The command was successfully sent and returned here.
             * Check the results.
             */
            memset( &dummy, 0x00, Dummy_Size );
            memcpy( dummy,  returnString, 4 );

            /* Check to see if the same header msg is back. Should be echoed. */
            if ( strcmp(msgHeader,dummy) != 0 )
            {
               sprintf( errbuf,
                       "Key Change: Msg Header mismatch. Sent = %s, Returned = %s",
                        msgHeader, dummy );
            }
            else
            {
               /* Message header was successfully echoed back to us.
                * Verify the Command Code.
                */
               memset( &dummy, 0x00, Dummy_Size );
               memcpy( dummy,  returnString + 4, 2 );

               if ( strcmp(dummy,"FB") != 0 )
               {
                  strcpy( errbuf, "Key Change: Cmd Code mismatch. Expected FB") ;
               }
               else
               {
                  /* Message Header and Command Code are good.
                   * Check the return code.
                   */
                  memset( &dummy,0,Dummy_Size );
                  strncpy( dummy,returnString + 6,2 );
                  racal_ret_code = atoi( dummy );

                  switch( racal_ret_code )
                  {
                     case 1:

                        /* This is a special error code.  It is only an advice.
                         * So we will post a warning, but then flow the code
                         * down through 'case 0' and treat this as if it was
                         * successful.
                         */
                        sprintf( errbuf,
                                "Key Change: retcode=%s, ZPK parity error, advice only",
                                 dummy );

                     case 0:  /* Successful Action - Get encrypted key. */

                        /* Determine length of key. */
                        memset( &dummy,0,Dummy_Size );
                        key_len_indicator[0] = returnString[8];

                        switch( key_len_indicator[0] )
                        {
                           case cDOUBLE_LEN_KEY:
                           case cDOUBLE_LEN_KEY_X9_17:

                              len = 32;
                              i   =  1;
                              break;

                           case cTRIPLE_LEN_KEY:
                           case cTRIPLE_LEN_KEY_X9_17:

                              len = 48;
                              i   =  1;
                              break;

                           default:
                              len = 16;
                              i   =  0;
                              break;
                        }

                        memcpy( dummy, returnString + 8 + i, len );
                        strcpy( auth_tx.TLF01_details.dest_key,dummy );
                        ret_val = TRANSACTION_VERIFIED;
                        break;

                     case 10:

                        sprintf( errbuf,
                                "Key Change: retcode=%s, ZMK parity error",
                                 dummy );
                        break;

                     case 11:

                        sprintf( errbuf,
                                "Key Change: retcode=%s, All zero ZPK with even parity.",
                                 dummy );
                        break;

                     case 12:

                        sprintf( errbuf,
                                "Key Change: retcode=%s, No keys loaded in user storage.",
                                 dummy );
                        break;

                     case 13:

                        sprintf( errbuf,
                                "Key Change: retcode=%s, LMK error; report to supervisor.",
                                 dummy );
                        break;

                     case 15:

                        sprintf( errbuf,
                                "Key Change: retcode=%s, Error in input data.",
                                 dummy );
                        break;

                     case 21:

                        sprintf( errbuf,
                                "Key Change: retcode=%s, Invalid user storage index.",
                                 dummy );
                        break;

                     default:

                        sprintf( errbuf,
                                "Key Change: retcode=%d, Unexpected return code.",
                                 dummy );
                        break;
                  }
               }
            }
         }
         else
         {
            strcpy( errbuf, "Key Change: Unable to send request to racal box.");
         }
      }
   }

   /* Is there an error message to log? */
   if ( errbuf[0] != 0x00 )
   {
      /* YES */
      if ( auth_tx.TLF01_details.terminal_id[0] == 0x00 )
      {
         racal_log_message( 2, 2,errbuf,"racal_KeyExchange_Key_for_MC_Keyblock",1 );
      }
      else
      {
         racal_log_message( 2, 2,errbuf,"racal_KeyExchange_Key_for_MC_Keyblock",1 );
      }
   }
   return( ret_val );

}/*end of racal_KeyExchange_Key_for_MC()*/

/**************************************************/

/****************************************************************/
INT racal_KeyExchange_Key_for_VISA_Keyblock()
{
   INT   ret_val = TRANSACTION_NOT_VERIFIED;
   INT   len = 0;
   INT   i = 0;
   INT   racal_ret_code = 0;
   CHAR  key_len_indicator[2] = {0};
   CHAR  errbuf[100]  = {0};
   CHAR  msgHeader[5]= {0};
   CHAR  cmdCode[3]= {0};
   CHAR  output[200]= {0};

   memset( &output,       0x00, sizeof(output)    );
   memset( &returnString, 0x00, ReturnString_Size );

   strcpy( msgHeader ,"8543" );
   strcpy( cmdCode,   "FA"   );

   strcat( output, msgHeader );
   strcat( output, cmdCode   );

   len = strlen( ZMK );
   if ( len < 16)
   {
      strcpy( errbuf, "Key Change: ZMK is < 16." );
   }
   else
   {
      switch( len )
      {
         case 32:
        	 strcat( output, DOUBLE_LEN_KEY );
        	 break;
         case 48:
        	 strcat( output, TRIPLE_LEN_KEY );
        	 break;
         default:
			 strcat( output, TRIPLE_LEN_KEY );
			 break;
      }
      strcat( output, ZMK );

      len = strlen( auth_tx.TLF01_details.source_key );
      if ( len <= 0 )
      {
         strcpy( errbuf, "Key Change: Source Key (ZPK) is blank" );
      }
      else
      {
         switch( len )
         {
            case 32:
            	strcat( output, DOUBLE_LEN_KEY_X9_17 );
            	break;
            case 48:
            	strcat( output, TRIPLE_LEN_KEY_X9_17 );
            	break;
            default:
				strcat( output, TRIPLE_LEN_KEY_X9_17 );
				break;
         }
         strcat(output,auth_tx.TLF01_details.source_key);

         /* The command is now composed. Check the length then send it. */
         if ( strlen(output) > MAX_CMD_LEN )
         {
            sprintf( errbuf,
                    "Key Change: Cmd Msg too long: %d, max = %d",
                     len, MAX_CMD_LEN );
         }

         /* Send a message to the Racal box. */
         else if ( racalSendToPort(output) == TRANSACTION_VERIFIED )
         {
            /* The command was successfully sent and returned here.
             * Check the results.
             */
            memset( &dummy, 0x00, Dummy_Size );
            memcpy( dummy,  returnString, 4 );

            /* Check to see if the same header msg is back. Should be echoed. */
            if ( strcmp(msgHeader,dummy) != 0 )
            {
               sprintf( errbuf,
                       "Key Change: Msg Header mismatch. Sent = %s, Returned = %s",
                        msgHeader, dummy );
            }
            else
            {
               /* Message header was successfully echoed back to us.
                * Verify the Command Code.
                */
               memset( &dummy, 0x00, Dummy_Size );
               memcpy( dummy,  returnString + 4, 2 );

               if ( strcmp(dummy,"FB") != 0 )
               {
                  strcpy( errbuf, "Key Change: Cmd Code mismatch. Expected FB") ;
               }
               else
               {
                  /* Message Header and Command Code are good.
                   * Check the return code.
                   */
                  memset( &dummy,0,Dummy_Size );
                  strncpy( dummy,returnString + 6,2 );
                  racal_ret_code = atoi( dummy );

                  switch( racal_ret_code )
                  {
                     case 1:

                        /* This is a special error code.  It is only an advice.
                         * So we will post a warning, but then flow the code
                         * down through 'case 0' and treat this as if it was
                         * successful.
                         */
                        sprintf( errbuf,
                                "Key Change: retcode=%s, ZPK parity error, advice only",
                                 dummy );

                     case 0:  /* Successful Action - Get encrypted key. */

                        /* Determine length of key. */
                        memset( &dummy,0,Dummy_Size );
                        key_len_indicator[0] = returnString[8];

                        switch( key_len_indicator[0] )
                        {
                           case cDOUBLE_LEN_KEY:
                           case cDOUBLE_LEN_KEY_X9_17:

                              len = 32;
                              i   =  1;
                              break;

                           case cTRIPLE_LEN_KEY:
                           case cTRIPLE_LEN_KEY_X9_17:

                              len = 48;
                              i   =  1;
                              break;

                           default:
                              len = 16;
                              i   =  0;
                              break;
                        }

                        memcpy( dummy, returnString + 8 + i, len );
                        strcpy( auth_tx.TLF01_details.dest_key,dummy );
                        ret_val = TRANSACTION_VERIFIED;
                        break;

                     case 10:

                        sprintf( errbuf,
                                "Key Change: retcode=%s, ZMK parity error",
                                 dummy );
                        break;

                     case 11:

                        sprintf( errbuf,
                                "Key Change: retcode=%s, All zero ZPK with even parity.",
                                 dummy );
                        break;

                     case 12:

                        sprintf( errbuf,
                                "Key Change: retcode=%s, No keys loaded in user storage.",
                                 dummy );
                        break;

                     case 13:

                        sprintf( errbuf,
                                "Key Change: retcode=%s, LMK error; report to supervisor.",
                                 dummy );
                        break;

                     case 15:

                        sprintf( errbuf,
                                "Key Change: retcode=%s, Error in input data.",
                                 dummy );
                        break;

                     case 21:

                        sprintf( errbuf,
                                "Key Change: retcode=%s, Invalid user storage index.",
                                 dummy );
                        break;

                     default:

                        sprintf( errbuf,
                                "Key Change: retcode=%d, Unexpected return code.",
                                 dummy );
                        break;
                  }
               }
            }
         }
         else
         {
            strcpy( errbuf, "Key Change: Unable to send request to racal box.");
         }
      }
   }

   /* Is there an error message to log? */
   if ( errbuf[0] != 0x00 )
   {
      /* YES */
      if ( auth_tx.TLF01_details.terminal_id[0] == 0x00 )
      {
         racal_log_message( 2, 2,errbuf,"racal_KeyExchange_Key_for_VISA_Keyblock",1 );
      }
      else
      {
         racal_log_message( 2, 2,errbuf,"racal_KeyExchange_Key_for_VISA_Keyblock",1 );
      }
   }
   return( ret_val );

}/* racal_KeyExchange_Key_for_VISA_Keyblock*/

/****************************************************************/
INT racal_KeyExchange_Key_for_AMEX_Keyblock()
{
   INT   ret_val = TRANSACTION_NOT_VERIFIED;
   INT   len = 0;
   INT   i = 0;
   INT   racal_ret_code = 0;
   CHAR  key_len_indicator[2] = {0};
   CHAR  errbuf[100]  = {0};
   CHAR  msgHeader[5]= {0};
   CHAR  cmdCode[3]= {0};
   CHAR  output[200]= {0};

   memset( &output,       0x00, sizeof(output)    );
   memset( &returnString, 0x00, ReturnString_Size );

   strcpy( msgHeader ,"8543" );
   strcpy( cmdCode,   "FA"   );

   strcat( output, msgHeader );
   strcat( output, cmdCode   );

   len = strlen( ZMK );
   if ( len < 16)
   {
      strcpy( errbuf, "Key Change: ZMK is < 16." );
   }
   else
   {
      switch( len )
      {
         case 32:
        	 strcat( output, DOUBLE_LEN_KEY );
        	 break;
         case 48:
        	 strcat( output, TRIPLE_LEN_KEY );
        	 break;
         default:
			 strcat( output, TRIPLE_LEN_KEY );
			 break;
      }
      strcat( output, ZMK );

      len = strlen( auth_tx.TLF01_details.source_key );
      if ( len <= 0 )
      {
         strcpy( errbuf, "Key Change: Source Key (ZPK) is blank" );
      }
      else
      {
         switch( len )
         {
            case 32:
            	strcat( output, DOUBLE_LEN_KEY_X9_17 );
            	break;
            case 48:
            	strcat( output, TRIPLE_LEN_KEY_X9_17 );
            	break;
            default:
				strcat( output, TRIPLE_LEN_KEY_X9_17 );
				break;
         }
         strcat(output,auth_tx.TLF01_details.source_key);

         /* The command is now composed. Check the length then send it. */
         if ( strlen(output) > MAX_CMD_LEN )
         {
            sprintf( errbuf,
                    "Key Change: Cmd Msg too long: %d, max = %d",
                     len, MAX_CMD_LEN );
         }

         /* Send a message to the Racal box. */
         else if ( racalSendToPort(output) == TRANSACTION_VERIFIED )
         {
            /* The command was successfully sent and returned here.
             * Check the results.
             */
            memset( &dummy, 0x00, Dummy_Size );
            memcpy( dummy,  returnString, 4 );

            /* Check to see if the same header msg is back. Should be echoed. */
            if ( strcmp(msgHeader,dummy) != 0 )
            {
               sprintf( errbuf,
                       "Key Change: Msg Header mismatch. Sent = %s, Returned = %s",
                        msgHeader, dummy );
            }
            else
            {
               /* Message header was successfully echoed back to us.
                * Verify the Command Code.
                */
               memset( &dummy, 0x00, Dummy_Size );
               memcpy( dummy,  returnString + 4, 2 );

               if ( strcmp(dummy,"FB") != 0 )
               {
                  strcpy( errbuf, "Key Change: Cmd Code mismatch. Expected FB") ;
               }
               else
               {
                  /* Message Header and Command Code are good.
                   * Check the return code.
                   */
                  memset( &dummy,0,Dummy_Size );
                  strncpy( dummy,returnString + 6,2 );
                  racal_ret_code = atoi( dummy );

                  switch( racal_ret_code )
                  {
                     case 1:

                        /* This is a special error code.  It is only an advice.
                         * So we will post a warning, but then flow the code
                         * down through 'case 0' and treat this as if it was
                         * successful.
                         */
                        sprintf( errbuf,
                                "Key Change: retcode=%s, ZPK parity error, advice only",
                                 dummy );

                     case 0:  /* Successful Action - Get encrypted key. */

                        /* Determine length of key. */
                        memset( &dummy,0,Dummy_Size );
                        key_len_indicator[0] = returnString[8];

                        switch( key_len_indicator[0] )
                        {
                           case cDOUBLE_LEN_KEY:
                           case cDOUBLE_LEN_KEY_X9_17:

                              len = 32;
                              i   =  1;
                              break;

                           case cTRIPLE_LEN_KEY:
                           case cTRIPLE_LEN_KEY_X9_17:

                              len = 48;
                              i   =  1;
                              break;

                           default:
                              len = 16;
                              i   =  0;
                              break;
                        }

                        memcpy( dummy, returnString + 8 + i, len );
                        strcpy( auth_tx.TLF01_details.dest_key,dummy );
                        ret_val = TRANSACTION_VERIFIED;
                        break;

                     case 10:

                        sprintf( errbuf,
                                "Key Change: retcode=%s, ZMK parity error",
                                 dummy );
                        break;

                     case 11:

                        sprintf( errbuf,
                                "Key Change: retcode=%s, All zero ZPK with even parity.",
                                 dummy );
                        break;

                     case 12:

                        sprintf( errbuf,
                                "Key Change: retcode=%s, No keys loaded in user storage.",
                                 dummy );
                        break;

                     case 13:

                        sprintf( errbuf,
                                "Key Change: retcode=%s, LMK error; report to supervisor.",
                                 dummy );
                        break;

                     case 15:

                        sprintf( errbuf,
                                "Key Change: retcode=%s, Error in input data.",
                                 dummy );
                        break;

                     case 21:

                        sprintf( errbuf,
                                "Key Change: retcode=%s, Invalid user storage index.",
                                 dummy );
                        break;

                     default:

                        sprintf( errbuf,
                                "Key Change: retcode=%d, Unexpected return code.",
                                 dummy );
                        break;
                  }
               }
            }
         }
         else
         {
            strcpy( errbuf, "Key Change: Unable to send request to racal box.");
         }
      }
   }

   /* Is there an error message to log? */
   if ( errbuf[0] != 0x00 )
   {
      /* YES */
      if ( auth_tx.TLF01_details.terminal_id[0] == 0x00 )
      {
         racal_log_message( 2, 2,errbuf,"racal_KeyExchange_Key_for_AMEX_Keyblock",1 );
      }
      else
      {
         racal_log_message( 2, 2,errbuf,"racal_KeyExchange_Key_for_AMEX_Keyblock",1 );
      }
   }
   return( ret_val );

}/*racal_KeyExchange_Key_for_AMEX_Keyblock*/

/****************************************************************/
INT racal_KeyExchange_Key_for_JCB_Keyblock()
{
   INT   ret_val = TRANSACTION_NOT_VERIFIED;
   INT   len = 0;
   INT   i = 0;
   INT   racal_ret_code = 0;
   CHAR  key_len_indicator[2] = {0};
   CHAR  errbuf[100]  = {0};
   CHAR  msgHeader[5]= {0};
   CHAR  cmdCode[3]= {0};
   CHAR  output[200]= {0};

   memset( &output,       0x00, sizeof(output)    );
   memset( &returnString, 0x00, ReturnString_Size );

   strcpy( msgHeader ,"8543" );
   strcpy( cmdCode,   "FA"   );

   strcat( output, msgHeader );
   strcat( output, cmdCode   );

   len = strlen( ZMK );
   if ( len < 16)
   {
      strcpy( errbuf, "Key Change: ZMK is < 16." );
   }
   else
   {
      switch( len )
      {
         case 32:
        	 strcat( output, DOUBLE_LEN_KEY );
        	 break;
         case 48:
        	 strcat( output, TRIPLE_LEN_KEY );
        	 break;
         default:
			 strcat( output, TRIPLE_LEN_KEY );
			 break;
      }
      strcat( output, ZMK );

      len = strlen( auth_tx.TLF01_details.source_key );
      if ( len <= 0 )
      {
         strcpy( errbuf, "Key Change: Source Key (ZPK) is blank" );
      }
      else
      {
         switch( len )
         {
            case 32:
            	strcat( output, DOUBLE_LEN_KEY_X9_17 );
            	break;
            case 48:
            	strcat( output, TRIPLE_LEN_KEY_X9_17 );
            	break;
            default:
				strcat( output, TRIPLE_LEN_KEY_X9_17 );
				break;
         }
         strcat(output,auth_tx.TLF01_details.source_key);

         /* The command is now composed. Check the length then send it. */
         if ( strlen(output) > MAX_CMD_LEN )
         {
            sprintf( errbuf,
                    "Key Change: Cmd Msg too long: %d, max = %d",
                     len, MAX_CMD_LEN );
         }

         /* Send a message to the Racal box. */
         else if ( racalSendToPort(output) == TRANSACTION_VERIFIED )
         {
            /* The command was successfully sent and returned here.
             * Check the results.
             */
            memset( &dummy, 0x00, Dummy_Size );
            memcpy( dummy,  returnString, 4 );

            /* Check to see if the same header msg is back. Should be echoed. */
            if ( strcmp(msgHeader,dummy) != 0 )
            {
               sprintf( errbuf,
                       "Key Change: Msg Header mismatch. Sent = %s, Returned = %s",
                        msgHeader, dummy );
            }
            else
            {
               /* Message header was successfully echoed back to us.
                * Verify the Command Code.
                */
               memset( &dummy, 0x00, Dummy_Size );
               memcpy( dummy,  returnString + 4, 2 );

               if ( strcmp(dummy,"FB") != 0 )
               {
                  strcpy( errbuf, "Key Change: Cmd Code mismatch. Expected FB") ;
               }
               else
               {
                  /* Message Header and Command Code are good.
                   * Check the return code.
                   */
                  memset( &dummy,0,Dummy_Size );
                  strncpy( dummy,returnString + 6,2 );
                  racal_ret_code = atoi( dummy );

                  switch( racal_ret_code )
                  {
                     case 1:

                        /* This is a special error code.  It is only an advice.
                         * So we will post a warning, but then flow the code
                         * down through 'case 0' and treat this as if it was
                         * successful.
                         */
                        sprintf( errbuf,
                                "Key Change: retcode=%s, ZPK parity error, advice only",
                                 dummy );

                     case 0:  /* Successful Action - Get encrypted key. */

                        /* Determine length of key. */
                        memset( &dummy,0,Dummy_Size );
                        key_len_indicator[0] = returnString[8];

                        switch( key_len_indicator[0] )
                        {
                           case cDOUBLE_LEN_KEY:
                           case cDOUBLE_LEN_KEY_X9_17:

                              len = 32;
                              i   =  1;
                              break;

                           case cTRIPLE_LEN_KEY:
                           case cTRIPLE_LEN_KEY_X9_17:

                              len = 48;
                              i   =  1;
                              break;

                           default:
                              len = 16;
                              i   =  0;
                              break;
                        }

                        memcpy( dummy, returnString + 8 + i, len );
                        strcpy( auth_tx.TLF01_details.dest_key,dummy );
                        ret_val = TRANSACTION_VERIFIED;
                        break;

                     case 10:

                        sprintf( errbuf,
                                "Key Change: retcode=%s, ZMK parity error",
                                 dummy );
                        break;

                     case 11:

                        sprintf( errbuf,
                                "Key Change: retcode=%s, All zero ZPK with even parity.",
                                 dummy );
                        break;

                     case 12:

                        sprintf( errbuf,
                                "Key Change: retcode=%s, No keys loaded in user storage.",
                                 dummy );
                        break;

                     case 13:

                        sprintf( errbuf,
                                "Key Change: retcode=%s, LMK error; report to supervisor.",
                                 dummy );
                        break;

                     case 15:

                        sprintf( errbuf,
                                "Key Change: retcode=%s, Error in input data.",
                                 dummy );
                        break;

                     case 21:

                        sprintf( errbuf,
                                "Key Change: retcode=%s, Invalid user storage index.",
                                 dummy );
                        break;

                     default:

                        sprintf( errbuf,
                                "Key Change: retcode=%d, Unexpected return code.",
                                 dummy );
                        break;
                  }
               }
            }
         }
         else
         {
            strcpy( errbuf, "Key Change: Unable to send request to racal box.");
         }
      }
   }

   /* Is there an error message to log? */
   if ( errbuf[0] != 0x00 )
   {
      /* YES */
      if ( auth_tx.TLF01_details.terminal_id[0] == 0x00 )
      {
         racal_log_message( 2, 2,errbuf,"racal_KeyExchange_Key_for_JCB_Keyblock",1 );
      }
      else
      {
         racal_log_message( 2, 2,errbuf,"racal_KeyExchange_Key_for_JCB_Keyblock",1 );
      }
   }
   return( ret_val );

}/*racal_KeyExchange_Key_for_JCB_Keyblock*/

/****************************************************************/
INT racal_KeyExchange_Key_for_UPI_Keyblock()
{
   INT   ret_val = TRANSACTION_NOT_VERIFIED;
   INT   len = 0;
   INT   i = 0;
   INT   racal_ret_code = 0;
   CHAR  key_len_indicator[2] = {0};
   CHAR  errbuf[100]  = {0};
   CHAR  msgHeader[5]= {0};
   CHAR  cmdCode[3]= {0};
   CHAR  output[200]= {0};

   memset( &output,       0x00, sizeof(output)    );
   memset( &returnString, 0x00, ReturnString_Size );

   strcpy( msgHeader ,"8543" );
   strcpy( cmdCode,   "FA"   );

   strcat( output, msgHeader );
   strcat( output, cmdCode   );

   len = strlen( ZMK );
   if ( len < 16)
   {
      strcpy( errbuf, "Key Change: ZMK is < 16." );
   }
   else
   {
      switch( len )
      {
         case 32:
        	 strcat( output, DOUBLE_LEN_KEY );
        	 break;
         case 48:
        	 strcat( output, TRIPLE_LEN_KEY );
        	 break;
         default:
			 strcat( output, TRIPLE_LEN_KEY );
			 break;
      }
      strcat( output, ZMK );

      len = strlen( auth_tx.TLF01_details.source_key );
      if ( len <= 0 )
      {
         strcpy( errbuf, "Key Change: Source Key (ZPK) is blank" );
      }
      else
      {
         switch( len )
         {
            case 32:
            	strcat( output, DOUBLE_LEN_KEY_X9_17 );
            	break;
            case 48:
            	strcat( output, TRIPLE_LEN_KEY_X9_17 );
            	break;
            default:
				strcat( output, TRIPLE_LEN_KEY_X9_17 );
				break;
         }
         strcat(output,auth_tx.TLF01_details.source_key);

         /* The command is now composed. Check the length then send it. */
         if ( strlen(output) > MAX_CMD_LEN )
         {
            sprintf( errbuf,
                    "Key Change: Cmd Msg too long: %d, max = %d",
                     len, MAX_CMD_LEN );
         }

         /* Send a message to the Racal box. */
         else if ( racalSendToPort(output) == TRANSACTION_VERIFIED )
         {
            /* The command was successfully sent and returned here.
             * Check the results.
             */
            memset( &dummy, 0x00, Dummy_Size );
            memcpy( dummy,  returnString, 4 );

            /* Check to see if the same header msg is back. Should be echoed. */
            if ( strcmp(msgHeader,dummy) != 0 )
            {
               sprintf( errbuf,
                       "Key Change: Msg Header mismatch. Sent = %s, Returned = %s",
                        msgHeader, dummy );
            }
            else
            {
               /* Message header was successfully echoed back to us.
                * Verify the Command Code.
                */
               memset( &dummy, 0x00, Dummy_Size );
               memcpy( dummy,  returnString + 4, 2 );

               if ( strcmp(dummy,"FB") != 0 )
               {
                  strcpy( errbuf, "Key Change: Cmd Code mismatch. Expected FB") ;
               }
               else
               {
                  /* Message Header and Command Code are good.
                   * Check the return code.
                   */
                  memset( &dummy,0,Dummy_Size );
                  strncpy( dummy,returnString + 6,2 );
                  racal_ret_code = atoi( dummy );

                  switch( racal_ret_code )
                  {
                     case 1:

                        /* This is a special error code.  It is only an advice.
                         * So we will post a warning, but then flow the code
                         * down through 'case 0' and treat this as if it was
                         * successful.
                         */
                        sprintf( errbuf,
                                "Key Change: retcode=%s, ZPK parity error, advice only",
                                 dummy );

                     case 0:  /* Successful Action - Get encrypted key. */

                        /* Determine length of key. */
                        memset( &dummy,0,Dummy_Size );
                        key_len_indicator[0] = returnString[8];

                        switch( key_len_indicator[0] )
                        {
                           case cDOUBLE_LEN_KEY:
                           case cDOUBLE_LEN_KEY_X9_17:

                              len = 32;
                              i   =  1;
                              break;

                           case cTRIPLE_LEN_KEY:
                           case cTRIPLE_LEN_KEY_X9_17:

                              len = 48;
                              i   =  1;
                              break;

                           default:
                              len = 16;
                              i   =  0;
                              break;
                        }

                        memcpy( dummy, returnString + 8 + i, len );
                        strcpy( auth_tx.TLF01_details.dest_key,dummy );
                        ret_val = TRANSACTION_VERIFIED;
                        break;

                     case 10:

                        sprintf( errbuf,
                                "Key Change: retcode=%s, ZMK parity error",
                                 dummy );
                        break;

                     case 11:

                        sprintf( errbuf,
                                "Key Change: retcode=%s, All zero ZPK with even parity.",
                                 dummy );
                        break;

                     case 12:

                        sprintf( errbuf,
                                "Key Change: retcode=%s, No keys loaded in user storage.",
                                 dummy );
                        break;

                     case 13:

                        sprintf( errbuf,
                                "Key Change: retcode=%s, LMK error; report to supervisor.",
                                 dummy );
                        break;

                     case 15:

                        sprintf( errbuf,
                                "Key Change: retcode=%s, Error in input data.",
                                 dummy );
                        break;

                     case 21:

                        sprintf( errbuf,
                                "Key Change: retcode=%s, Invalid user storage index.",
                                 dummy );
                        break;

                     default:

                        sprintf( errbuf,
                                "Key Change: retcode=%d, Unexpected return code.",
                                 dummy );
                        break;
                  }
               }
            }
         }
         else
         {
            strcpy( errbuf, "Key Change: Unable to send request to racal box.");
         }
      }
   }

   /* Is there an error message to log? */
   if ( errbuf[0] != 0x00 )
   {
      /* YES */
      if ( auth_tx.TLF01_details.terminal_id[0] == 0x00 )
      {
         racal_log_message( 2, 2,errbuf,"racal_KeyExchange_Key_for_UPI_Keyblock",1 );
      }
      else
      {
         racal_log_message( 2, 2,errbuf,"racal_KeyExchange_Key_for_UPI_Keyblock",1 );
      }
   }
   return( ret_val );

}/*racal_KeyExchange_Key_for_UPI_Keyblock*/

/****************************************************************/
INT racal_KeyExchange_Key_for_DCI_Keyblock()
{
   INT   ret_val = TRANSACTION_NOT_VERIFIED;
   INT   len = 0;
   INT   i = 0;
   INT   racal_ret_code = 0;
   CHAR  key_len_indicator[2] = {0};
   CHAR  errbuf[100]  = {0};
   CHAR  msgHeader[5]= {0};
   CHAR  cmdCode[3]= {0};
   CHAR  output[513]= {0};

   memset( &returnString, 0x00, ReturnString_Size );

   strcpy( msgHeader ,"8543" );
   strcpy( cmdCode,   "FA"   );

   strcat( output, msgHeader );
   strcat( output, cmdCode   );

   len = strlen( ZMK );
   if ( len < 16)
   {
      strcpy( errbuf, "Key Change: ZMK is < 16." );
   }
   else
   {
      switch( len )
      {
         case 32:
        	 strcat( output, DOUBLE_LEN_KEY );
        	 break;
         case 48:
        	 strcat( output, TRIPLE_LEN_KEY );
        	 break;
         default:
			 strcat( output, TRIPLE_LEN_KEY );
			 break;
      }
      strcat( output, ZMK );

      len = strlen( auth_tx.TLF01_details.source_key );
      if ( len <= 0 )
      {
         strcpy( errbuf, "Key Change: Source Key (ZPK) is blank" );
      }
      else
      {
         switch( len )
         {
            case 32:
            	strcat( output, DOUBLE_LEN_KEY_X9_17 );
            	break;
            case 48:
            	strcat( output, TRIPLE_LEN_KEY_X9_17 );
            	break;
            default:
				strcat( output, TRIPLE_LEN_KEY_X9_17 );
				break;
         }
         strcat(output,auth_tx.TLF01_details.source_key);

         /* The command is now composed. Check the length then send it. */
         if ( strlen(output) > MAX_CMD_LEN )
         {
            sprintf( errbuf,
                    "Key Change: Cmd Msg too long: %d, max = %d",
                     len, MAX_CMD_LEN );
         }

         /* Send a message to the Racal box. */
         else if ( racalSendToPort(output) == TRANSACTION_VERIFIED )
         {
            /* The command was successfully sent and returned here.
             * Check the results.
             */
            memset( &dummy, 0x00, Dummy_Size );
            memcpy( dummy,  returnString, 4 );

            /* Check to see if the same header msg is back. Should be echoed. */
            if ( strcmp(msgHeader,dummy) != 0 )
            {
               sprintf( errbuf,
                       "Key Change: Msg Header mismatch. Sent = %s, Returned = %s",
                        msgHeader, dummy );
            }
            else
            {
               /* Message header was successfully echoed back to us.
                * Verify the Command Code.
                */
               memset( &dummy, 0x00, Dummy_Size );
               memcpy( dummy,  returnString + 4, 2 );

               if ( strcmp(dummy,"FB") != 0 )
               {
                  strcpy( errbuf, "Key Change: Cmd Code mismatch. Expected FB") ;
               }
               else
               {
                  /* Message Header and Command Code are good.
                   * Check the return code.
                   */
                  memset( &dummy,0,Dummy_Size );
                  strncpy( dummy,returnString + 6,2 );
                  racal_ret_code = atoi( dummy );

                  switch( racal_ret_code )
                  {
                     case 1:

                        /* This is a special error code.  It is only an advice.
                         * So we will post a warning, but then flow the code
                         * down through 'case 0' and treat this as if it was
                         * successful.
                         */
                        sprintf( errbuf,
                                "Key Change: retcode=%s, ZPK parity error, advice only",
                                 dummy );

                     case 0:  /* Successful Action - Get encrypted key. */

                        /* Determine length of key. */
                        memset( &dummy,0,Dummy_Size );
                        key_len_indicator[0] = returnString[8];

                        switch( key_len_indicator[0] )
                        {
                           case cDOUBLE_LEN_KEY:
                           case cDOUBLE_LEN_KEY_X9_17:

                              len = 32;
                              i   =  1;
                              break;

                           case cTRIPLE_LEN_KEY:
                           case cTRIPLE_LEN_KEY_X9_17:

                              len = 48;
                              i   =  1;
                              break;

                           default:
                              len = 16;
                              i   =  0;
                              break;
                        }

                        memcpy( dummy, returnString + 8 + i, len );
                        strcpy( auth_tx.TLF01_details.dest_key,dummy );
                        ret_val = TRANSACTION_VERIFIED;
                        break;

                     case 10:

                        sprintf( errbuf,
                                "Key Change: retcode=%s, ZMK parity error",
                                 dummy );
                        break;

                     case 11:

                        sprintf( errbuf,
                                "Key Change: retcode=%s, All zero ZPK with even parity.",
                                 dummy );
                        break;

                     case 12:

                        sprintf( errbuf,
                                "Key Change: retcode=%s, No keys loaded in user storage.",
                                 dummy );
                        break;

                     case 13:

                        sprintf( errbuf,
                                "Key Change: retcode=%s, LMK error; report to supervisor.",
                                 dummy );
                        break;

                     case 15:

                        sprintf( errbuf,
                                "Key Change: retcode=%s, Error in input data.",
                                 dummy );
                        break;

                     case 21:

                        sprintf( errbuf,
                                "Key Change: retcode=%s, Invalid user storage index.",
                                 dummy );
                        break;

                     default:

                        sprintf( errbuf,
                                "Key Change: retcode=%d, Unexpected return code.",
                                 dummy );
                        break;
                  }
               }
            }
         }
         else
         {
            strcpy( errbuf, "Key Change: Unable to send request to racal box.");
         }
      }
   }

   /* Is there an error message to log? */
   if ( errbuf[0] != 0x00 )
   {
      /* YES */
      if ( auth_tx.TLF01_details.terminal_id[0] == 0x00 )
      {
         racal_log_message( 2, 2,errbuf,"racal_KeyExchange_Key_for_DCI_Keyblock",1 );
      }
      else
      {
         racal_log_message( 2, 2,errbuf,"racal_KeyExchange_Key_for_DCI_Keyblock",1 );
      }
   }
   return( ret_val );

}/*racal_KeyExchange_Key_for_DCI_Keyblock*/
/********************************************************/



/*************************************************************************************
       NAME:           AscendentMsgHandler()

       DESCRIPTION:    This module waits in the loop waiting for a message to come in
					   from the GUI or any other application.  Once the messgae is in,
					   it will go througth the switch statement and determine what to
					   do with it.

       INPUTS:
			p_msg_in   The incoming message.

       OUTPUTS:
           None
       RETURNS:
		   None.

      AUTHOR:         Faramarz Arad 1/13/1999
      MODIFIED BY:
*************************************************************************************/
void AscendentMsgHandler( pPTE_MSG p_msg_in )
{
  pPTE_MSG        p_cur_msg;
  pPTE_MSG_DATA   p_msg_data;
  pBYTE           p_data;
  INT             return_code;
  CHAR            Buff[100]    = {0};
  CHAR			  err_buf[100] = {0};
  CHAR 			  emv_len[4]   = {0};
  INT			  csc_len;

  p_cur_msg   = p_msg_in;
  /*the queue name of the application which send the request.*/
  orig_queue  = ptemsg_get_msg_orig_queue(p_cur_msg);

  /*The following line might be important to get the communication with the NspGui going.farad 10/15/1998.*/
  orig_info = ptemsg_get_msg_orig_comms_info(p_cur_msg);

  MYPRINT("ASCENDENT: AscendentMsgHandler, begin switch\n");

   /*The big switch statements.*/
   switch( ptemsg_get_msg_type( p_cur_msg ) )
	{
      /*+---------------+
        | Pin Translate |
        +---------------+*/
      case MT_NSP_TRANSLATE_PIN:

         p_msg_data = ptemsg_get_pte_msg_data(p_msg_in);
         p_data     = ptemsg_get_pte_msg_data_data(p_msg_data);

         memset( &auth_tx, 0x00,   Auth_Tx_Size );
         memcpy( &auth_tx, p_data, Auth_Tx_Size );

         memset( &ncf01, 0x00, Ncf01_Size );
         memcpy( &ncf01, p_data + Auth_Tx_Size, Ncf01_Size );

         memset( outgoing_buf, 0x00, Outgoing_Buf_Size );

         racal_log_message( 2, 1, "Received Transaction Details","racalEquitTranlatePin",1 );
         /* Set response code to 01 for failure or 00 for success. */
         if( racalEquitTranlatePin() == TRANSACTION_NOT_VERIFIED)
         {
        	   strcpy( auth_tx.TLF01_details.security_response_code,"01" );
         }
         else
         {
        	  strcpy( auth_tx.TLF01_details.security_response_code,"00" );
         }

         memcpy( outgoing_buf,                &auth_tx, Auth_Tx_Size );
         memcpy( outgoing_buf + Auth_Tx_Size, &ncf01,   Ncf01_Size   );
         send_response_asynchronous(MT_NSP_TRANSLATE_PIN_RESPONSE,orig_queue);

         break;

         /*+---------------------------------------------------------+
           | Pin Decryption with BDK & KSN and Encryption with ZPK |
           +---------------------------------------------------------+*/
      case MT_NSP_DECRYPT_TRANSLATE_PIN:

         p_msg_data = ptemsg_get_pte_msg_data(p_msg_in);
         p_data     = ptemsg_get_pte_msg_data_data(p_msg_data);

         memset( &auth_tx, 0x00,   Auth_Tx_Size );
         memcpy( &auth_tx, p_data, Auth_Tx_Size );

         memset( &ncf01, 0x00, Ncf01_Size );
         memcpy( &ncf01, p_data + Auth_Tx_Size, Ncf01_Size );

         memset( outgoing_buf, 0x00, Outgoing_Buf_Size );

         racal_log_message( 2, 1, "Received Transaction Details","racalEquitDecryptPin",1 );
         /* Set response code to 01 for failure or 00 for success. */
         if( racalEquitDecryptPin() == TRANSACTION_NOT_VERIFIED)
         {
        	   strcpy( auth_tx.TLF01_details.security_response_code,"01" );
         }
         else
         {
        	 if (racalEquitTranlatePin() == TRANSACTION_NOT_VERIFIED)
        	 {
        		 strcpy( auth_tx.TLF01_details.security_response_code,"01" );
        	 }
        	 else
        	 {
        		 strcpy( auth_tx.TLF01_details.security_response_code,"00" );
        	 }
         }

         memcpy( outgoing_buf,                &auth_tx, Auth_Tx_Size );
         memcpy( outgoing_buf + Auth_Tx_Size, &ncf01,   Ncf01_Size   );
         send_response_asynchronous(MT_NSP_DECRYPT_TRANSLATE_PIN_RESPONSE,orig_queue);

         break;

      case MT_NSP_DECRYPT_PIN:

         p_msg_data = ptemsg_get_pte_msg_data(p_msg_in);
         p_data     = ptemsg_get_pte_msg_data_data(p_msg_data);

         memset( &auth_tx, 0x00,   Auth_Tx_Size );
         memcpy( &auth_tx, p_data, Auth_Tx_Size );

         memset( &ncf01, 0x00, Ncf01_Size );
         memcpy( &ncf01, p_data + Auth_Tx_Size, Ncf01_Size );

         memset( outgoing_buf, 0x00, Outgoing_Buf_Size );

         racal_log_message( 2, 1, "Received Transaction Details","racalEquitDecryptPin",1 );
         /* Set response code to 01 for failure or 00 for success. */
         if( racalEquitDecryptPin() == TRANSACTION_NOT_VERIFIED)
         {
        	   strcpy( auth_tx.TLF01_details.security_response_code,"01" );
         }
         else
         {
        	 strcpy( auth_tx.TLF01_details.security_response_code,"00" );
         }

         memcpy( outgoing_buf,                &auth_tx, Auth_Tx_Size );
         memcpy( outgoing_buf + Auth_Tx_Size, &ncf01,   Ncf01_Size   );
         send_response_asynchronous(MT_NSP_DECRYPT_PIN_RESPONSE,orig_queue);

         break;

      case MT_NSP_DINERS_GENERATE_KEY:

		   p_msg_data = ptemsg_get_pte_msg_data(p_msg_in);
		   p_data	  = ptemsg_get_pte_msg_data_data(p_msg_data);

		   memset( &auth_tx, 0x00,	 Auth_Tx_Size );
		   memcpy( &auth_tx, p_data, Auth_Tx_Size );

		   memset( &ncf01, 0x00, Ncf01_Size );
		   memcpy( &ncf01, p_data + Auth_Tx_Size, Ncf01_Size );

		   memset( outgoing_buf, 0x00, Outgoing_Buf_Size );

		   racal_log_message( 2, 1, "Received Transaction Details","racalGenarateDinersKey",1 );
		   /* Set response code to 01 for failure or 00 for success. */
		   if( racalGenarateDinersKey() == TRANSACTION_NOT_VERIFIED)
		   {
			  strcpy( auth_tx.TLF01_details.security_response_code,"01" );
		   }
		   else
		   {
			  strcpy( auth_tx.TLF01_details.security_response_code,"00" );
		   }

		   memcpy( outgoing_buf,				&auth_tx, Auth_Tx_Size );
		   memcpy( outgoing_buf + Auth_Tx_Size, &ncf01,   Ncf01_Size   );
		   send_response_asynchronous(MT_NSP_DINERS_GENERATE_KEY_RESPONSE,orig_queue);

		   break;

	  case MT_NSP_TRANSLATE_ZMK_LMK:

		   p_msg_data = ptemsg_get_pte_msg_data(p_msg_in);
		   p_data	  = ptemsg_get_pte_msg_data_data(p_msg_data);

		   memset( &auth_tx, 0x00,	 Auth_Tx_Size );
		   memcpy( &auth_tx, p_data, Auth_Tx_Size );

		   memset( &ncf01, 0x00, Ncf01_Size );
		   memcpy( &ncf01, p_data + Auth_Tx_Size, Ncf01_Size );

		   memset( outgoing_buf, 0x00, Outgoing_Buf_Size );

		   racal_log_message( 2, 1, "Received Transaction Details","racalCUPExchangeKey_ZMK_LMK",1 );
		   /* Set response code to 01 for failure or 00 for success. */
		   if( racalCUPExchangeKey_ZMK_LMK() == TRANSACTION_NOT_VERIFIED)
		   {
			   strcpy( auth_tx.TLF01_details.security_response_code,"01" );
		   }
		   else
		   {
			   strcpy( auth_tx.TLF01_details.security_response_code,"00" );
		   }

		   memcpy( outgoing_buf,				&auth_tx, Auth_Tx_Size );
		   memcpy( outgoing_buf + Auth_Tx_Size, &ncf01,   Ncf01_Size   );
		   send_response_asynchronous(MT_NSP_TRANSLATE_ZMK_LMK_RESPONSE,orig_queue);

			break;


      /*+------------------+
        | Pin Verification |
        +------------------+*/
      case MT_NSP_VERIFYPIN:

         memset( &auth_tx, 0x00, Auth_Tx_Size );
         p_msg_data = ptemsg_get_pte_msg_data(p_msg_in);
         p_data = ptemsg_get_pte_msg_data_data(p_msg_data);

         memcpy( &auth_tx, p_data, Auth_Tx_Size );

         /* Set response code to 01 for failure or 00 for success. */
         racal_log_message( 2, 1, "Received Transaction Details","racalCUPVerifyPin_racalEquitVerifyPin",1 );

    	 if (strcmp(auth_tx.BIN01_details.card_family,CARD_CUP) == 0 &&
    			 strlen(auth_tx.TLF01_details.pin_block) > 0)
    	 {/* FOR CUP PIN validation*/
			if ( racalCUPVerifyPin() == TRANSACTION_NOT_VERIFIED )
			{
				strcpy( auth_tx.TLF01_details.security_response_code, "01" );
			}
			else
			{
				strcpy( auth_tx.TLF01_details.security_response_code, "00" );
			}
    	 }
    	 else
    	 {
    		 if ( racalEquitVerifyPin() == TRANSACTION_NOT_VERIFIED )
    		 {
    			 strcpy( auth_tx.TLF01_details.security_response_code, "01" );
    		 }
    		 else
    		 {
    			 strcpy( auth_tx.TLF01_details.security_response_code, "00" );
    		 }
    	 }
         memset( outgoing_buf, 0x00, Outgoing_Buf_Size );
         memcpy( outgoing_buf, &auth_tx, Auth_Tx_Size );
         send_response_asynchronous( MT_NSP_VERIFYPIN_RESPONSE, orig_queue );

         break;


      /*+------------------+
        | Verify Magstripe |
        +------------------+*/
      case MT_NSP_VERIFY_MAGSTRIPE:

         /* This case statement is invoked for the Racal
          * CVV, CVC, CSD, and Jcb's verification.
          */
         p_msg_data = ptemsg_get_pte_msg_data(p_msg_in);
         p_data     = ptemsg_get_pte_msg_data_data(p_msg_data);

         memset( &auth_tx, 0x00,   Auth_Tx_Size );
         memcpy( &auth_tx, p_data, Auth_Tx_Size );

         memset( outgoing_buf, 0x00, Outgoing_Buf_Size );
         csc_len = strlen(auth_tx.TLF01_details.interest_rate) + strlen(auth_tx.TLF01_details.product_codes[18].quantity);

         racal_log_message( 2, 1, "Received Transaction Details","racalEquitVerifyCVV_CVC_CSC_CAV",1 );
		if (strcmp(auth_tx.BIN01_details.card_family, CARD_AMEX) == 0 &&
				((auth_tx.TLF01_details.interest_rate[0] != 'S' && csc_len >= 4) ||
				(auth_tx.TLF01_details.interest_rate[0] == 'S' && csc_len >= 5)))
		{
			return_code = racalEquitVerify_4_5CSC_AMEX();

		}
		else
		{
			return_code = racalEquitVerifyCVV_CVC_CSC_CAV();
		}

         /*+---------------------------------+
           | Set response code as follows:   |
           +---------------------------------+
           |    00 = Transaction is verified |
           |    01 = CVC1 is not verified    |
           |    02 = CVC2 is not verified    |
           |    03 = CVC2 verified           |
           +---------------------------------+*/
         if (return_code == TRANSACTION_NOT_VERIFIED)
         {
        	  strcpy(auth_tx.TLF01_details.security_response_code,"01");
         }
         else if (return_code == CVC2_TRANSACTION_NOT_VERIFIED)
         {
        	  strcpy(auth_tx.TLF01_details.security_response_code,"02");
         }
         else if (return_code == CVC2_TRANSACTION_VERIFIED)
         {
        	   strcpy(auth_tx.TLF01_details.security_response_code,"03");
         }
         else
         {
        	 strcpy(auth_tx.TLF01_details.security_response_code,"00");

        	 if (strcmp(auth_tx.BIN01_details.card_family,CARD_CUP) == 0 &&
        			 strlen(auth_tx.TLF01_details.pin_block) > 0)
        	 {/*
        	  We have already Approved CVN for this card.
        	  We shall check the PIN if PIN is exists.
        	  Set response code to 55 for failure or 00 for success. */
        		if ( racalCUPVerifyPin() == TRANSACTION_NOT_VERIFIED )
				{
					strcpy( auth_tx.TLF01_details.security_response_code, "55" );
				}
				else
				{
					strcpy( auth_tx.TLF01_details.security_response_code, "00" );
				}
        	 }

         }
         memcpy( outgoing_buf, &auth_tx, Auth_Tx_Size );
         send_response_asynchronous(MT_NSP_VERIFY_MAGSTRIPE_RESPONSE,orig_queue);

      break;

      case MT_NSP_VERIFY3DS2_MAGSTRIPE:

         /* This case statement is invoked for the Racal
          * CVV, CVC, CSD, and Jcb's verification.
          */
         p_msg_data = ptemsg_get_pte_msg_data(p_msg_in);
         p_data     = ptemsg_get_pte_msg_data_data(p_msg_data);

         memset( &auth_tx, 0x00,   Auth_Tx_Size );
         memcpy( &auth_tx, p_data, Auth_Tx_Size );

         memset( outgoing_buf, 0x00, Outgoing_Buf_Size );
         csc_len = strlen(auth_tx.TLF01_details.interest_rate) + strlen(auth_tx.TLF01_details.product_codes[18].quantity);

         racal_log_message( 2, 1, "Received Transaction Details","racalEquitVerifyCVV_CVC_CSC_CAV",1 );
		if (strcmp(auth_tx.BIN01_details.card_family, CARD_AMEX) == 0 &&
				((auth_tx.TLF01_details.interest_rate[0] != 'S' && csc_len >= 4) ||
				(auth_tx.TLF01_details.interest_rate[0] == 'S' && csc_len >= 5)))
		{
			return_code = racalEquitVerify_4_5CSC_AMEX();

		}
		else if (strcmp(auth_tx.BIN01_details.card_family, CARD_JCB) == 0)
		{
			return_code = racalEquitVerify_JCB3DS2_CVC2();
		}
		else
		{
			return_code = racalEquitVerifyCVV_CVC_CSC_CAV();
		}

         /*+---------------------------------+
           | Set response code as follows:   |
           +---------------------------------+
           |    00 = Transaction is verified |
           |    01 = CVC1 is not verified    |
           |    02 = CVC2 is not verified    |
           |    03 = CVC2 verified           |
           +---------------------------------+*/
         if (return_code == TRANSACTION_NOT_VERIFIED)
         {
        	  strcpy(auth_tx.TLF01_details.security_response_code,"01");
         }
         else if (return_code == CVC2_TRANSACTION_NOT_VERIFIED)
         {
        	  strcpy(auth_tx.TLF01_details.security_response_code,"02");
         }
         else if (return_code == CVC2_TRANSACTION_VERIFIED)
         {
        	   strcpy(auth_tx.TLF01_details.security_response_code,"03");
         }
         else
         {
        	 strcpy(auth_tx.TLF01_details.security_response_code,"00");

        	 if (strcmp(auth_tx.BIN01_details.card_family,CARD_CUP) == 0 &&
        			 strlen(auth_tx.TLF01_details.pin_block) > 0)
        	 {/*
        	  We have already Approved CVN for this card.
        	  We shall check the PIN if PIN is exists.
        	  Set response code to 55 for failure or 00 for success. */
        		if ( racalCUPVerifyPin() == TRANSACTION_NOT_VERIFIED )
				{
					strcpy( auth_tx.TLF01_details.security_response_code, "55" );
				}
				else
				{
					strcpy( auth_tx.TLF01_details.security_response_code, "00" );
				}
        	 }

         }
         memcpy( outgoing_buf, &auth_tx, Auth_Tx_Size );
         send_response_asynchronous(MT_NSP_VERIFY_MAGSTRIPE_RESPONSE,orig_queue);

      break;

	case MT_NSP_VERIFY_ARQC:

		 /* This case statement is invoked for the Racal
		  * CVV, CVC, CSD, and Jcb's verification.
		  */
		 p_msg_data = ptemsg_get_pte_msg_data(p_msg_in);
		 p_data     = ptemsg_get_pte_msg_data_data(p_msg_data);

		 memset( &auth_tx, 0x00,   Auth_Tx_Size );
		 memcpy( &auth_tx, p_data, Auth_Tx_Size );

		 memset( outgoing_buf, 0x00, Outgoing_Buf_Size );

		 racal_log_message( 2, 1, "Received Transaction Details","racalVerifyARQCfor_CUPEMV",1 );

		 if(( 0 == strcmp(auth_tx.BIN01_details.card_family,CARD_CUP)) ||
			  strncmp(auth_tx.TLF01_details.product_code,"CC",2) == 0 )
		 {
			 return_code = racalVerifyARQCfor_CUPEMV();
		 }
		 else  if( 0 == strcmp(auth_tx.BIN01_details.card_family,CARD_JCB))
		 {
			 return_code = racalVerifyARQC_for_JCB();
		 }
		 else  if( 0 == strcmp(auth_tx.BIN01_details.card_family,CARD_DINERS))
		 {
			 return_code = racalVerifyARQC_For_Diners();
		 }
		 else  if( (0 == strcmp(auth_tx.BIN01_details.card_family,CARD_VISA)) &&
				   (strncmp(auth_tx.EMV_details.iad+4,"12",2) == 0))
		 {
			 return_code= racalVerifyARQC_For_VisaCVN18();
		 }
		 else
		 {
			 return_code = racalVerifyARQC_for_EMV();
		 }
		 /*+---------------------------------+
		   | Set response code as follows:   |
		   +---------------------------------+
		   |    00 = Transaction is verified |
		   |    01 = CVC1 is not verified    |
		   |
		   +---------------------------------+*/
		 if (return_code == TRANSACTION_NOT_VERIFIED)
		 {
			 strcpy(auth_tx.TLF01_details.security_response_code,"01");
		 }
		 else
		 {
			 strcpy(auth_tx.TLF01_details.security_response_code,"00");
		 }

		 memcpy( outgoing_buf, &auth_tx, Auth_Tx_Size );
		 send_response_asynchronous(MT_NSP_VERIFY_ARQC_RESPONSE,orig_queue);

      break;

	case MT_NSP_VERIFY_3D_SECURE:

         /* This case statement is invoked for the Racal
          * CVV, CVC, CSD, and Jcb's verification.
          */
         p_msg_data = ptemsg_get_pte_msg_data(p_msg_in);
         p_data     = ptemsg_get_pte_msg_data_data(p_msg_data);

         memset( &auth_tx, 0x00,   Auth_Tx_Size );
         memcpy( &auth_tx, p_data, Auth_Tx_Size );

         memset( outgoing_buf, 0x00, Outgoing_Buf_Size );

         racal_log_message( 2, 1, "Received Transaction Details","racalVerify_CAVV_for_3Dsecure",1 );

         return_code = racalVerify_CAVV_for_3Dsecure();

         /*+---------------------------------+
           | Set response code as follows:   |
           +---------------------------------+
           |    00 = Transaction is verified |
           |    01 = CVC1 is not verified    |
           |
           +---------------------------------+*/
         if (return_code == TRANSACTION_NOT_VERIFIED)
         {
        	   strcpy(auth_tx.TLF01_details.security_response_code,"01");
         }
         else
         {
        	   strcpy(auth_tx.TLF01_details.security_response_code,"00");
         }

         memcpy( outgoing_buf, &auth_tx, Auth_Tx_Size );
         send_response_asynchronous(MT_NSP_VERIFY_3D_SECURE_RESPONSE,orig_queue);

      break;

	case MT_NSP_VERIFY_3D_SECURE_2:

		 /* This case statement is invoked for the Racal
		  * CVV, CVC, CSD, and Jcb's verification.
		  */
		 p_msg_data = ptemsg_get_pte_msg_data(p_msg_in);
		 p_data     = ptemsg_get_pte_msg_data_data(p_msg_data);

		 memset( &auth_tx, 0x00,   Auth_Tx_Size );
		 memcpy( &auth_tx, p_data, Auth_Tx_Size );

		 memset( outgoing_buf, 0x00, Outgoing_Buf_Size );

		 racal_log_message( 2, 1, "Received Transaction Details","racalVerify_CAVV_for_3Dsecure",1 );

		 if( (0 == strcmp(auth_tx.BIN01_details.card_family,CARD_VISA)))
		 {
			 return_code = racalVerify_CAVV_for_3Dsecure_VISA();
		 }
		 else if( 0 == strcmp(auth_tx.BIN01_details.card_family,CARD_DINERS))
		 {
			 return_code = racalVerify_CAVV_for_3Dsecure_DCI();
		 }
		 else  if( 0 == strcmp(auth_tx.BIN01_details.card_family,CARD_JCB))
		 {
			 return_code = racalVerify_CAVV_for_3Dsecure_JCB();
		 }
		 else if(( 0 == strcmp(auth_tx.BIN01_details.card_family,CARD_CUP)))
		 {
			 return_code = racalVerify_CAVV_for_3Dsecure_CUP();
		 }
		 else if(0 == strcmp(auth_tx.BIN01_details.card_family,CARD_AMEX))
		 {
			 return_code = racalVerify_CAVV_for_3Dsecure_AMEX();
		 }
		 else if(strcmp(auth_tx.BIN01_details.card_family,CARD_MASTERCARD) == 0)
		 {
			 return_code = racalVerify_IAV_for_3Dsecure_MC();
			 /* clear the input sent from txcntl from future field 1 and 2*/
			memset(auth_tx.EMV_details.future_field1,0x00,sizeof(auth_tx.EMV_details.future_field1)-1);
			memset(auth_tx.EMV_details.future_field2,0x00,sizeof(auth_tx.EMV_details.future_field2)-1);
		 }

		 /*+---------------------------------+
		   | Set response code as follows:   |
		   +---------------------------------+
		   |    00 = Transaction is verified |
		   |    01 = CVC1 is not verified    |
		   |
		   +---------------------------------+*/
		 if(strcmp(auth_tx.BIN01_details.card_family,CARD_MASTERCARD) != 0)
		 {
		 if (return_code == TRANSACTION_NOT_VERIFIED)
		 {
			   strcpy(auth_tx.TLF01_details.security_response_code,"01");
		 }
		 else
		 {
			   strcpy(auth_tx.TLF01_details.security_response_code,"00");
		 }
		 }


		 memcpy( outgoing_buf, &auth_tx, Auth_Tx_Size );
		 send_response_asynchronous(MT_NSP_VERIFY_3D_SECURE_RESPONSE_2,orig_queue);

	  break;

      /*+--------------------------+
      | CUP ECOM CVN2 Verification |
      +---------------------------*/
	case MT_NSP_VERIFY_CUP_ECOM_CVN2:

		 /* This case statement is invoked for CUP ECOM CVN2.*/
		 p_msg_data = ptemsg_get_pte_msg_data(p_msg_in);
		 p_data     = ptemsg_get_pte_msg_data_data(p_msg_data);

		 memset( &auth_tx, 0x00,   Auth_Tx_Size );
		 memcpy( &auth_tx, p_data, Auth_Tx_Size );

		 memset( outgoing_buf, 0x00, Outgoing_Buf_Size );
		 racal_log_message( 2, 1, "Received Transaction Details","racalVerify_CVN2_for_CUP_Ecom",1 );

		 return_code = racalVerify_CVN2_for_CUP_Ecom();

		 if (return_code == TRANSACTION_NOT_VERIFIED)
		 {
			   strcpy(auth_tx.TLF01_details.security_response_code,"01");
		 }
		 else
		 {
			   strcpy(auth_tx.TLF01_details.security_response_code,"00");
		 }

		 memcpy( outgoing_buf, &auth_tx, Auth_Tx_Size );
		 send_response_asynchronous(MT_NSP_VERIFY_CUP_ECOM_CVN2_RESPONSE,orig_queue);

	     break;

      /*+--------------------+
        | Dynamic Key Change |
        +--------------------+*/
      case MT_NSP_KEYCHANGE:

         memset( &ZMK,     0x00 ,Zmk_Size     );
         memset( &auth_tx, 0x00, Auth_Tx_Size );

         p_msg_data = ptemsg_get_pte_msg_data(p_msg_in);
         p_data = ptemsg_get_pte_msg_data_data(p_msg_data);

         memcpy( &auth_tx, p_data,  Auth_Tx_Size );
         memcpy( &ZMK,     p_data + Auth_Tx_Size, Zmk_Size );

         racal_log_message( 2, 1, "Received Transaction Details","racalEquitKeyExchangeKey",1 );
         /* Set response code to 01 for failure or 00 for success. */
         if (racalEquitKeyExchangeKey() == TRANSACTION_NOT_VERIFIED)
         {
        	   strcpy(auth_tx.TLF01_details.security_response_code,"01");
         }
         else
         {
        	    strcpy(auth_tx.TLF01_details.security_response_code,"00");
         }

         memset( outgoing_buf, 0x00, Outgoing_Buf_Size );
         memcpy( outgoing_buf, &auth_tx, Auth_Tx_Size  );
         send_response_asynchronous(MT_NSP_KEYCHANGE_RESPONSE,orig_queue);

      break;


      case MT_SYSTEM_REQUEST:
         ptesystm_system_msg_handler(p_msg_in);
         if ( ST1_SYS_PING == ptemsg_get_msg_subtype1(p_msg_in) )
         {
            /* When pinged, display application version on Monitor. */
            sprintf( Buff,
                    "Pinged -> Racal Service: %s, version %s",
                     ServiceName, Version );
            racal_log_message( 1, 1,Buff,"AscendentMsgHandler",0 );
         }
      break;

      /**************************************Keyblock Section Begins************/
      case MT_NSP_TRANSLATE_PIN_KEYBLOCK:

		  p_msg_data = ptemsg_get_pte_msg_data(p_msg_in);
		  p_data     = ptemsg_get_pte_msg_data_data(p_msg_data);

		  memset( &auth_tx, 0x00,   Auth_Tx_Size );
		  memcpy( &auth_tx, p_data, Auth_Tx_Size );

		  memset( &Ncf01_Keyblock, 0x00, sizeof(Ncf01_Keyblock) );
		  memcpy( &Ncf01_Keyblock, p_data + Auth_Tx_Size, sizeof(Ncf01_Keyblock) );

		  memset( outgoing_buf, 0x00, Outgoing_Buf_Size );

		  racal_log_message( 2, 1, "Received Transaction Details KEYBLOCK","racalEquitTranlatePin",1 );
		  /* Set response code to 01 for failure or 00 for success. */

		  if (true == racal_Check_If_MC_Transaction())
		  {
			 if( racal_TranlatePin_Keyblock_MC() == TRANSACTION_NOT_VERIFIED)
			  {
				   strcpy( auth_tx.TLF01_details.security_response_code,"01" );
			  }
			  else
			  {
				  strcpy( auth_tx.TLF01_details.security_response_code,"00" );
			  }
		  }
		  else if (true == racal_Check_If_VISA_Transaction())
		  {
			  if( racal_TranlatePin_Keyblock_VISA() == TRANSACTION_NOT_VERIFIED)
			  {
				   strcpy( auth_tx.TLF01_details.security_response_code,"01" );
			  }
			  else
			  {
				  strcpy( auth_tx.TLF01_details.security_response_code,"00" );
			  }
		  }
		  else if (true == racal_Check_If_AMEX_Transaction())
		  {
			 if( racal_TranlatePin_Keyblock_AMEX() == TRANSACTION_NOT_VERIFIED)
			  {
				   strcpy( auth_tx.TLF01_details.security_response_code,"01" );
			  }
			  else
			  {
				  strcpy( auth_tx.TLF01_details.security_response_code,"00" );
			  }
		  }
		  else if (true == racal_Check_If_JCB_Transaction())
		  {
			 if( racal_TranlatePin_Keyblock_JCB() == TRANSACTION_NOT_VERIFIED)
			 {
				   strcpy( auth_tx.TLF01_details.security_response_code,"01" );
			 }
			 else
			 {
				 strcpy( auth_tx.TLF01_details.security_response_code,"00" );
			 }
		  }
		  else if (true == racal_Check_If_UPI_Transaction())
		  {
			 if( racal_TranlatePin_Keyblock_UPI() == TRANSACTION_NOT_VERIFIED)
			 {
				   strcpy( auth_tx.TLF01_details.security_response_code,"01" );
			 }
			 else
			 {
				 strcpy( auth_tx.TLF01_details.security_response_code,"00" );
			 }
		  }
		  else if (true == racal_Check_If_DCI_Transaction())
		  {
			 if( racal_TranlatePin_Keyblock_DCI() == TRANSACTION_NOT_VERIFIED)
			 {
				   strcpy( auth_tx.TLF01_details.security_response_code,"01" );
			 }
			 else
			 {
				 strcpy( auth_tx.TLF01_details.security_response_code,"00" );
			 }
		  }
		  else if( racalEquitTranlatePin() == TRANSACTION_NOT_VERIFIED)
		  {
			   strcpy( auth_tx.TLF01_details.security_response_code,"01" );
		  }
		  else
		  {
			  strcpy( auth_tx.TLF01_details.security_response_code,"00" );
		  }

		  memcpy( outgoing_buf,                &auth_tx, Auth_Tx_Size );
		  memcpy( outgoing_buf + Auth_Tx_Size, &ncf01,   Ncf01_Size   );
		  send_response_asynchronous(MT_NSP_TRANSLATE_PIN_RESPONSE_KEYBLOCK,orig_queue);

	   break;

	   case MT_NSP_DINERS_GENERATE_KEY_KEYBLOCK:

		   p_msg_data = ptemsg_get_pte_msg_data(p_msg_in);
		   p_data	  = ptemsg_get_pte_msg_data_data(p_msg_data);

		   memset( &auth_tx, 0x00,	 Auth_Tx_Size );
		   memcpy( &auth_tx, p_data, Auth_Tx_Size );

		   memset( &Ncf01_Keyblock, 0x00, sizeof(Ncf01_Keyblock) );
		   memcpy( &Ncf01_Keyblock, p_data + Auth_Tx_Size, sizeof(Ncf01_Keyblock) );

		   memset( outgoing_buf, 0x00, Outgoing_Buf_Size );

		   racal_log_message( 2, 1, "Received Transaction Details KEYBLOCK","racalGenarateDinersKey",1 );
		   /* Set response code to 01 for failure or 00 for success. */
		   if( racal_Genarate_DinersKey_Keyblock() == TRANSACTION_NOT_VERIFIED)
		   {
			  strcpy( auth_tx.TLF01_details.security_response_code,"01" );
		   }
		   else
		   {
			  strcpy( auth_tx.TLF01_details.security_response_code,"00" );
		   }

		   memcpy( outgoing_buf,				&auth_tx, Auth_Tx_Size );
		   memcpy( outgoing_buf + Auth_Tx_Size, &ncf01,   Ncf01_Size   );
		   send_response_asynchronous(MT_NSP_DINERS_GENERATE_KEY_RESPONSE_KEYBLOCK,orig_queue);

		break;

	  case MT_NSP_TRANSLATE_ZMK_LMK_KEYBLOCK:

		   p_msg_data = ptemsg_get_pte_msg_data(p_msg_in);
		   p_data	  = ptemsg_get_pte_msg_data_data(p_msg_data);

		   memset( &auth_tx, 0x00,	 Auth_Tx_Size );
		   memcpy( &auth_tx, p_data, Auth_Tx_Size );

		   memset( &Ncf01_Keyblock, 0x00, sizeof(Ncf01_Keyblock) );
		   memcpy( &Ncf01_Keyblock, p_data + Auth_Tx_Size, sizeof(Ncf01_Keyblock) );

		   memset( outgoing_buf, 0x00, Outgoing_Buf_Size );

		   racal_log_message( 2, 1, "Received Transaction Details KEYBLOCK","racalCUPExchangeKey_ZMK_LMK",1 );
		   /* Set response code to 01 for failure or 00 for success. */
		   if( racalCUPExchangeKey_ZMK_LMK() == TRANSACTION_NOT_VERIFIED)
		   {
			   strcpy( auth_tx.TLF01_details.security_response_code,"01" );
		   }
		   else
		   {
			   strcpy( auth_tx.TLF01_details.security_response_code,"00" );
		   }

		   memcpy( outgoing_buf,				&auth_tx, Auth_Tx_Size );
		   memcpy( outgoing_buf + Auth_Tx_Size, &ncf01,   Ncf01_Size   );
		   send_response_asynchronous(MT_NSP_TRANSLATE_ZMK_LMK_RESPONSE_KEYBLOCK,orig_queue);

		break;


	   /*+------------------+
		 | Pin Verification |
		 +------------------+*/
	   case MT_NSP_VERIFYPIN_KEYBLOCK:

		  memset( &auth_tx, 0x00, Auth_Tx_Size );
		  memset( &caf01_keyblock, 0x00, sizeof(caf01_keyblock) );

		  p_msg_data = ptemsg_get_pte_msg_data(p_msg_in);
		  p_data = ptemsg_get_pte_msg_data_data(p_msg_data);

		  memcpy( &auth_tx, p_data, Auth_Tx_Size );
		  memcpy( &caf01_keyblock, p_data + Auth_Tx_Size, sizeof(caf01_keyblock) );

		  /* Set response code to 01 for failure or 00 for success. */
		  racal_log_message( 2, 1, "Received Transaction Details KEYBLOCK","racalCUPVerifyPin_racalEquitVerifyPin",1 );

		  if (true == racal_Check_If_MC_Transaction())
		  {
				if ( racal_VerifyPin_MC_Keyblock() == TRANSACTION_NOT_VERIFIED )
				{
					strcpy( auth_tx.TLF01_details.security_response_code, "01" );
				}
				else
				{
					strcpy( auth_tx.TLF01_details.security_response_code, "00" );
				}
		  }

		  else if (true == racal_Check_If_VISA_Transaction())
		  {
				if ( racal_VerifyPin_VISA_Keyblock() == TRANSACTION_NOT_VERIFIED )
				{
					strcpy( auth_tx.TLF01_details.security_response_code, "01" );
				}
				else
				{
					strcpy( auth_tx.TLF01_details.security_response_code, "00" );
				}
		  }

		  else if (true == racal_Check_If_AMEX_Transaction())
		  {
				if ( racal_VerifyPin_AMEX_Keyblock() == TRANSACTION_NOT_VERIFIED )
				{
					strcpy( auth_tx.TLF01_details.security_response_code, "01" );
				}
				else
				{
					strcpy( auth_tx.TLF01_details.security_response_code, "00" );
				}
		  }

		  else if (true == racal_Check_If_JCB_Transaction())
		  {
				if ( racal_VerifyPin_JCB_Keyblock() == TRANSACTION_NOT_VERIFIED )
				{
					strcpy( auth_tx.TLF01_details.security_response_code, "01" );
				}
				else
				{
					strcpy( auth_tx.TLF01_details.security_response_code, "00" );
				}
		  }

		  else if (true == racal_Check_If_UPI_Transaction())
		  {
				if ( racal_VerifyPin_UPI_Keybloack() == TRANSACTION_NOT_VERIFIED )
				{
					strcpy( auth_tx.TLF01_details.security_response_code, "01" );
				}
				else
				{
					strcpy( auth_tx.TLF01_details.security_response_code, "00" );
				}
		  }

		  else if (true == racal_Check_If_DCI_Transaction())
		  {
				if ( racal_VerifyPin_DCI_Keyblock() == TRANSACTION_NOT_VERIFIED )
				{
					strcpy( auth_tx.TLF01_details.security_response_code, "01" );
				}
				else
				{
					strcpy( auth_tx.TLF01_details.security_response_code, "00" );
				}
		  }

		  else
		  {
			 if ( racalEquitVerifyPin() == TRANSACTION_NOT_VERIFIED )
			 {
				 strcpy( auth_tx.TLF01_details.security_response_code, "01" );
			 }
			 else
			 {
				 strcpy( auth_tx.TLF01_details.security_response_code, "00" );
			 }
		  }
		  memset( outgoing_buf, 0x00, Outgoing_Buf_Size );
		  memcpy( outgoing_buf, &auth_tx, Auth_Tx_Size );
		  send_response_asynchronous( MT_NSP_VERIFYPIN_RESPONSE_KEYBLOCK, orig_queue );

	   break;


	   /*+------------------+
		 | Verify Magstripe |
		 +------------------+*/
	   case MT_NSP_VERIFY_MAGSTRIPE_KEYBLOCK:

		 /* This case statement is invoked for the Racal
		  * CVV, CVC, CSD, and Jcb's verification.
		 */
		 p_msg_data = ptemsg_get_pte_msg_data(p_msg_in);
		 p_data     = ptemsg_get_pte_msg_data_data(p_msg_data);

		 memset( &auth_tx, 0x00,   Auth_Tx_Size );
		 memcpy( &auth_tx, p_data, Auth_Tx_Size );

		 memset( &caf01_keyblock, 0x00, sizeof(caf01_keyblock) );
		 memcpy( &caf01_keyblock, p_data + Auth_Tx_Size, sizeof(caf01_keyblock) );

		 memset( outgoing_buf, 0x00, Outgoing_Buf_Size );
		 csc_len = strlen(auth_tx.TLF01_details.interest_rate) + strlen(auth_tx.TLF01_details.product_codes[18].quantity);

		 racal_log_message( 2, 1, "Received Transaction Details KEYBLOCK","racal_Verify_4_5CSC_AMEX_Keyblock",1 );

		 if (strcmp(auth_tx.BIN01_details.card_family, CARD_AMEX) == 0 &&
				((auth_tx.TLF01_details.interest_rate[0] != 'S' && csc_len >= 4) ||
				(auth_tx.TLF01_details.interest_rate[0] == 'S' && csc_len >= 5)))
		 {
		 	 return_code = racal_Verify_4_5CSC_AMEX_Keyblock();

		 }

		 else if (true == racal_Check_If_MC_Transaction())
		 {
			return_code = racal_Verify_CVV_CVC_CSC_CAV_MC_Keyblock();
		 }

		 else if (true == racal_Check_If_VISA_Transaction())
		 {
			return_code = racal_Verify_CVV_CVC_CSC_CAV_VISA_Keyblock();
		 }

		 else if (true == racal_Check_If_AMEX_Transaction())
		 {
			return_code = racal_Verify_CVV_CVC_CSC_CAV_AMEX_Keyblock();
		 }

		 else if (true == racal_Check_If_JCB_Transaction())
		 {
			return_code = racal_Verify_CVV_CVC_CSC_CAV_JCB_Keyblock();
		 }

		 else if (true == racal_Check_If_UPI_Transaction())
		 {
			return_code = racal_Verify_CVV_CVC_CSC_CAV_UPI_Keyblock();
		 }

		 else if (true == racal_Check_If_DCI_Transaction())
		 {
			return_code = racal_Verify_CVV_CVC_CSC_CAV_DCI_Keyblock();
		 }

		 else
		 {
			return_code = racalEquitVerifyCVV_CVC_CSC_CAV();
		 }

		  /*+---------------------------------+
			| Set response code as follows:   |
			+---------------------------------+
			|    00 = Transaction is verified |
			|    01 = CVC1 is not verified    |
			|    02 = CVC2 is not verified    |
			|    03 = CVC2 verified           |
			+---------------------------------+*/
		  if (return_code == TRANSACTION_NOT_VERIFIED)
		  {
			  strcpy(auth_tx.TLF01_details.security_response_code,"01");
		  }
		  else if (return_code == CVC2_TRANSACTION_NOT_VERIFIED)
		  {
			  strcpy(auth_tx.TLF01_details.security_response_code,"02");
		  }
		  else if (return_code == CVC2_TRANSACTION_VERIFIED)
		  {
			   strcpy(auth_tx.TLF01_details.security_response_code,"03");
		  }
		  else
		  {
			 strcpy(auth_tx.TLF01_details.security_response_code,"00");

			 if (strcmp(auth_tx.BIN01_details.card_family,CARD_CUP) == 0 &&
					 strlen(auth_tx.TLF01_details.pin_block) > 0)
			 {/*
			  We have already Approved CVN for this card.
			  We shall check the PIN if PIN is exists.
			  Set response code to 55 for failure or 00 for success. */
				if ( racal_VerifyPin_UPI_Keybloack() == TRANSACTION_NOT_VERIFIED )
				{
					strcpy( auth_tx.TLF01_details.security_response_code, "55" );
				}
				else
				{
					strcpy( auth_tx.TLF01_details.security_response_code, "00" );
				}
			 }

		  }
		  memcpy( outgoing_buf, &auth_tx, Auth_Tx_Size );
		  send_response_asynchronous(MT_NSP_VERIFY_MAGSTRIPE_RESPONSE_KEYBLOCK,orig_queue);

	   break;

	   case MT_NSP_VERIFY3DS2_MAGSTRIPE_KEYBLOCK:

		  /* This case statement is invoked for the Racal
		   * CVV, CVC, CSD, and Jcb's verification.
		   */
		  p_msg_data = ptemsg_get_pte_msg_data(p_msg_in);
		  p_data     = ptemsg_get_pte_msg_data_data(p_msg_data);

		  memset( &auth_tx, 0x00,   Auth_Tx_Size );
		  memcpy( &auth_tx, p_data, Auth_Tx_Size );

		  memset( &caf01_keyblock, 0x00, sizeof(caf01_keyblock) );
		  memcpy( &caf01_keyblock, p_data + Auth_Tx_Size, sizeof(caf01_keyblock) );

		  memset( outgoing_buf, 0x00, Outgoing_Buf_Size );
		  csc_len = strlen(auth_tx.TLF01_details.interest_rate) + strlen(auth_tx.TLF01_details.product_codes[18].quantity);

		  racal_log_message( 2, 1, "Received Transaction Details KEYBLOCK","racalEquitVerifyCVV_CVC_CSC_CAV",1 );
		if (strcmp(auth_tx.BIN01_details.card_family, CARD_AMEX) == 0 &&
				((auth_tx.TLF01_details.interest_rate[0] != 'S' && csc_len >= 4) ||
				(auth_tx.TLF01_details.interest_rate[0] == 'S' && csc_len >= 5)))
		{
			return_code = racal_Verify_4_5CSC_AMEX_Keyblock();

		}
		else if (strcmp(auth_tx.BIN01_details.card_family, CARD_JCB) == 0)
		{
			return_code = racal_Verify_JCB3DS2_CVC2_Keyblock();
		}
		else if (true == racal_Check_If_MC_Transaction())
		{
			return_code = racal_Verify_CVV_CVC_CSC_CAV_MC_Keyblock();
		}
		else if (true == racal_Check_If_VISA_Transaction())
		{
			return_code = racal_Verify_CVV_CVC_CSC_CAV_VISA_Keyblock();
		}
		else if (true == racal_Check_If_UPI_Transaction())
		{
			return_code = racal_Verify_CVV_CVC_CSC_CAV_UPI_Keyblock();
		}
		else if (true == racal_Check_If_DCI_Transaction())
		{
			return_code = racal_Verify_CVV_CVC_CSC_CAV_DCI_Keyblock();
		}

		  /*+---------------------------------+
			| Set response code as follows:   |
			+---------------------------------+
			|    00 = Transaction is verified |
			|    01 = CVC1 is not verified    |
			|    02 = CVC2 is not verified    |
			|    03 = CVC2 verified           |
			+---------------------------------+*/
		  if (return_code == TRANSACTION_NOT_VERIFIED)
		  {
			  strcpy(auth_tx.TLF01_details.security_response_code,"01");
		  }
		  else if (return_code == CVC2_TRANSACTION_NOT_VERIFIED)
		  {
			  strcpy(auth_tx.TLF01_details.security_response_code,"02");
		  }
		  else if (return_code == CVC2_TRANSACTION_VERIFIED)
		  {
			   strcpy(auth_tx.TLF01_details.security_response_code,"03");
		  }
		  else
		  {
			 strcpy(auth_tx.TLF01_details.security_response_code,"00");

			 if (strcmp(auth_tx.BIN01_details.card_family,CARD_CUP) == 0 &&
					 strlen(auth_tx.TLF01_details.pin_block) > 0)
			 {/*
			  We have already Approved CVN for this card.
			  We shall check the PIN if PIN is exists.
			  Set response code to 55 for failure or 00 for success. */
				if ( racal_VerifyPin_UPI_Keybloack() == TRANSACTION_NOT_VERIFIED )
				{
					strcpy( auth_tx.TLF01_details.security_response_code, "55" );
				}
				else
				{
					strcpy( auth_tx.TLF01_details.security_response_code, "00" );
				}
			 }

		  }
		  memcpy( outgoing_buf, &auth_tx, Auth_Tx_Size );
		  send_response_asynchronous(MT_NSP_VERIFY_MAGSTRIPE_RESPONSE_KEYBLOCK,orig_queue);

	   break;



	case MT_NSP_VERIFY_ARQC_KEYBLOCK:

		  /* This case statement is invoked for the Racal
		   * CVV, CVC, CSD, and Jcb's verification.
		   */
		  p_msg_data = ptemsg_get_pte_msg_data(p_msg_in);
		  p_data     = ptemsg_get_pte_msg_data_data(p_msg_data);

		  memset( &auth_tx, 0x00,   Auth_Tx_Size );
		  memcpy( &auth_tx, p_data, Auth_Tx_Size );

		  memset( &caf01_keyblock, 0x00, sizeof(caf01_keyblock) );
		  memcpy( &caf01_keyblock, p_data + Auth_Tx_Size, sizeof(caf01_keyblock) );

		  memset( outgoing_buf, 0x00, Outgoing_Buf_Size );

		  racal_log_message( 2, 1, "Received Transaction Details KEYBLOCK","racalVerifyARQCfor_CUPEMV",1 );

		  if (true == racal_Check_If_UPI_Transaction())
		  {
			 return_code = racal_Verify_ARQC_for_CUPEMV_Keyblock();
		  }

		  else if (true == racal_Check_If_JCB_Transaction())
		  {
			 return_code = racal_Verify_ARQC_for_JCB_Keyblock();
		  }

		  else if (true == racal_Check_If_DCI_Transaction())
		  {
			 return_code = racal_Verify_ARQC_For_Diners_Keyblock();
		  }

		  else  if( (0 == strcmp(auth_tx.BIN01_details.card_family,CARD_VISA)) &&
				   (strncmp(auth_tx.EMV_details.iad+4,"12",2) == 0))
		  {
			 return_code= racal_Verify_ARQC_For_VisaCVN18_Keyblock();
		  }

		  else if (true == racal_Check_If_MC_Transaction())
		  {
			return_code = racal_Verify_ARQC_for_MC_Keyblock();
		  }

		  else if (true == racal_Check_If_VISA_Transaction())
		  {
			return_code = racal_Verify_ARQC_for_VISA_Keyblock();
		  }

		  else if (true == racal_Check_If_AMEX_Transaction())
		  {
			 return_code = racal_Verify_ARQC_for_AMEX_Keyblock();
		  }
		  /*+---------------------------------+
			| Set response code as follows:   |
			+---------------------------------+
			|    00 = Transaction is verified |
			|    01 = CVC1 is not verified    |
			|
			+---------------------------------+*/
		  if (return_code == TRANSACTION_NOT_VERIFIED)
		  {
			 strcpy(auth_tx.TLF01_details.security_response_code,"01");
		  }
		  else
		  {
			 strcpy(auth_tx.TLF01_details.security_response_code,"00");
		  }

		  memcpy( outgoing_buf, &auth_tx, Auth_Tx_Size );
		  send_response_asynchronous(MT_NSP_VERIFY_ARQC_RESPONSE_KEYBLOCK,orig_queue);

	   break;

	case MT_NSP_VERIFY_3D_SECURE_KEYBLOCK:

		  /* This case statement is invoked for the Racal
		   * CVV, CVC, CSD, and Jcb's verification.
		   */
		  p_msg_data = ptemsg_get_pte_msg_data(p_msg_in);
		  p_data     = ptemsg_get_pte_msg_data_data(p_msg_data);

		  memset( &auth_tx, 0x00,   Auth_Tx_Size );
		  memcpy( &auth_tx, p_data, Auth_Tx_Size );

		  memset( &caf01_keyblock, 0x00, sizeof(caf01_keyblock) );
		  memcpy( &caf01_keyblock, p_data + Auth_Tx_Size, sizeof(caf01_keyblock) );

		  memset( outgoing_buf, 0x00, Outgoing_Buf_Size );

		  racal_log_message( 2, 1, "Received Transaction Details KEYBLOCK","racalVerify_CAVV_for_3Dsecure",1 );


		 if (true == racal_Check_If_MC_Transaction())
		 {
			 return_code = racal_Verify_CAVV_for_3Dsecure_MC_Keyblock();
		 }
		 else if (true == racal_Check_If_VISA_Transaction())
		 {
			 return_code = racal_Verify_CAVV_for_3Dsecure_VISA_Keyblock();
		 }
		 else if (true == racal_Check_If_AMEX_Transaction())
		 {
			 return_code = racal_Verify_CAVV_for_3Dsecure_AMEX_Keyblock();
		 }
		 else if (true == racal_Check_If_JCB_Transaction())
		 {
			 return_code = racal_Verify_CAVV_for_3Dsecure_JCB_Keyblock();
		 }
		 else if (true == racal_Check_If_UPI_Transaction())
		 {
			 return_code = racal_Verify_CAVV_for_3Dsecure_UPI_Keyblock();
		 }
		 else if (true == racal_Check_If_DCI_Transaction())
		 {
			 return_code = racal_Verify_CAVV_for_3Dsecure_DCI_Keyblock();
		 }

		  /*+---------------------------------+
			| Set response code as follows:   |
			+---------------------------------+
			|    00 = Transaction is verified |
			|    01 = CVC1 is not verified    |
			|
			+---------------------------------+*/
		  if (return_code == TRANSACTION_NOT_VERIFIED)
		  {
			   strcpy(auth_tx.TLF01_details.security_response_code,"01");
		  }
		  else
		  {
			   strcpy(auth_tx.TLF01_details.security_response_code,"00");
		  }

		  memcpy( outgoing_buf, &auth_tx, Auth_Tx_Size );
		  send_response_asynchronous(MT_NSP_VERIFY_3D_SECURE_RESPONSE_KEYBLOCK,orig_queue);

	   break;

	case MT_NSP_VERIFY_3D_SECURE_2_KEYBLOCK:

		 /* This case statement is invoked for the Racal
		  * CVV, CVC, CSD, and Jcb's verification.
		  */
		 p_msg_data = ptemsg_get_pte_msg_data(p_msg_in);
		 p_data     = ptemsg_get_pte_msg_data_data(p_msg_data);

		 memset( &auth_tx, 0x00,   Auth_Tx_Size );
		 memcpy( &auth_tx, p_data, Auth_Tx_Size );

		 memset( &caf01_keyblock, 0x00, sizeof(caf01_keyblock) );
		 memcpy( &caf01_keyblock, p_data + Auth_Tx_Size, sizeof(caf01_keyblock) );

		 memset( outgoing_buf, 0x00, Outgoing_Buf_Size );

		 racal_log_message( 2, 1, "Received Transaction Details KEYBLOCK","racalVerify_CAVV_for_3Dsecure",1 );

		 if (true == racal_Check_If_VISA_Transaction())
		 {
			 return_code = racal_Verify_CAVV_for_3Dsecure2_VISA_Keyblock();
		 }
		 else if (true == racal_Check_If_DCI_Transaction())
		 {
			 return_code = racal_Verify_CAVV_for_3Dsecure2_DCI_Keyblock();
		 }
		 else if (true == racal_Check_If_JCB_Transaction())
		 {
			 return_code = racal_Verify_CAVV_for_3Dsecure3_JCB_Keyblock();
		 }
		 else if (true == racal_Check_If_UPI_Transaction())
		 {
			 return_code = racal_Verify_CAVV_for_3Dsecure2_UPI_Keyblock();
		 }
		 else if (true == racal_Check_If_AMEX_Transaction())
		 {
			 return_code = racal_Verify_CAVV_for_3Dsecure2_AMEX_Keyblock();
		 }
		 else if (true == racal_Check_If_MC_Transaction())
		 {
			 return_code = racal_Verify_IAV_for_3Dsecure2_MC_Keyblock();
			 /* clear the input sent from txcntl from future field 1 and 2*/
			memset(auth_tx.EMV_details.future_field1,0x00,sizeof(auth_tx.EMV_details.future_field1)-1);
			memset(auth_tx.EMV_details.future_field2,0x00,sizeof(auth_tx.EMV_details.future_field2)-1);
		 }

		 /*+---------------------------------+
		   | Set response code as follows:   |
		   +---------------------------------+
		   |    00 = Transaction is verified |
		   |    01 = CVC1 is not verified    |
		   |
		   +---------------------------------+*/
		 if(strcmp(auth_tx.BIN01_details.card_family,CARD_MASTERCARD) != 0)
		 {
			 if (return_code == TRANSACTION_NOT_VERIFIED)
			 {
				 strcpy(auth_tx.TLF01_details.security_response_code,"01");
			 }
			 else
			 {
				 strcpy(auth_tx.TLF01_details.security_response_code,"00");
			 }
		 }


		 memcpy( outgoing_buf, &auth_tx, Auth_Tx_Size );
		 send_response_asynchronous(MT_NSP_VERIFY_3D_SECURE_RESPONSE_2_KEYBLOCK,orig_queue);

			  break;

	   /*+--------------------------+
	   | CUP ECOM CVN2 Verification |
	   +---------------------------*/
	case MT_NSP_VERIFY_CUP_ECOM_CVN2_KEYBLOCK:

		 /* This case statement is invoked for CUP ECOM CVN2.*/
		 p_msg_data = ptemsg_get_pte_msg_data(p_msg_in);
		 p_data     = ptemsg_get_pte_msg_data_data(p_msg_data);

		 memset( &auth_tx, 0x00,   Auth_Tx_Size );
		 memcpy( &auth_tx, p_data, Auth_Tx_Size );

		 memset( &caf01_keyblock, 0x00, sizeof(caf01_keyblock) );
		 memcpy( &caf01_keyblock, p_data + Auth_Tx_Size, sizeof(caf01_keyblock) );

		 memset( outgoing_buf, 0x00, Outgoing_Buf_Size );
		 racal_log_message( 2, 1, "Received Transaction Details KEYBLOCK","racalVerify_CVN2_for_CUP_Ecom",1 );

		 return_code = racal_Verify_CVN2_for_UPI_Ecom_Keyblock();

		 if (return_code == TRANSACTION_NOT_VERIFIED)
		 {
			   strcpy(auth_tx.TLF01_details.security_response_code,"01");
		 }
		 else
		 {
			   strcpy(auth_tx.TLF01_details.security_response_code,"00");
		 }

		 memcpy( outgoing_buf, &auth_tx, Auth_Tx_Size );
		 send_response_asynchronous(MT_NSP_VERIFY_CUP_ECOM_CVN2_RESPONSE_KEYBLOCK,orig_queue);

		 break;

	   /*+--------------------+
		 | Dynamic Key Change |
		 +--------------------+*/
	   case MT_NSP_KEYCHANGE_KEYBLOCK:

		  memset( &ZMK,     0x00 ,Zmk_Size     );
		  memset( &auth_tx, 0x00, Auth_Tx_Size );

		  p_msg_data = ptemsg_get_pte_msg_data(p_msg_in);
		  p_data = ptemsg_get_pte_msg_data_data(p_msg_data);

		  memcpy( &auth_tx, p_data,  Auth_Tx_Size );
		  memcpy( &ZMK,     p_data + Auth_Tx_Size, Zmk_Size );

		  racal_log_message( 2, 1, "Received Transaction Details KEYBLOCK","racalEquitKeyExchangeKey",1 );
		  /* Set response code to 01 for failure or 00 for success. */

		  if (true == racal_Check_If_MC_Transaction())
		  {
			 return_code = racal_KeyExchange_Key_for_MC_Keyblock ();
		  }
		  else if (true == racal_Check_If_VISA_Transaction())
		  {
			 return_code = racal_KeyExchange_Key_for_VISA_Keyblock ();
		  }
		  else if (true == racal_Check_If_AMEX_Transaction())
		  {
			 return_code = racal_KeyExchange_Key_for_AMEX_Keyblock ();
		  }
		  else if (true == racal_Check_If_JCB_Transaction())
		  {
			 return_code = racal_KeyExchange_Key_for_JCB_Keyblock ();
		  }
		  else if (true == racal_Check_If_UPI_Transaction())
		  {
			 return_code = racal_KeyExchange_Key_for_UPI_Keyblock ();
		  }
		  else if (true == racal_Check_If_DCI_Transaction())
		  {
			 return_code = racal_KeyExchange_Key_for_DCI_Keyblock ();
		  }

		  if (return_code == TRANSACTION_NOT_VERIFIED)
		  {
			   strcpy(auth_tx.TLF01_details.security_response_code,"01");
		  }
		  else
		  {
				strcpy(auth_tx.TLF01_details.security_response_code,"00");
		  }

		  memset( outgoing_buf, 0x00, Outgoing_Buf_Size );
		  memcpy( outgoing_buf, &auth_tx, Auth_Tx_Size  );
		  send_response_asynchronous(MT_NSP_KEYCHANGE_RESPONSE_KEYBLOCK,orig_queue);

	   break;



      default :
         racal_log_message( 1, 1,"unknown msg_type received","AscendentMsgHandler",0 );
      break;
	}  /* switch msg_type */

}

/*************************************************************************************
       NAME:           MainProcessor()

       DESCRIPTION:    This is the entry point for this application.  Every time this
	   application runs, the following happens:
	   1)  The initialization for xipc and application name and so on happens.
	   2)  The code enters a loop while it waits for an imcoming message.


       INPUTS:
			None.
       OUTPUTS:
            None.
       RETURNS:
		   None.

      AUTHOR:
      MODIFIED BY:    Faramarz Arad. 11/13/1998.
*************************************************************************************/
void MainProcessor()
{
	pPTE_MSG	  p_msg;
	LONG		  ret_code;
	CHAR		  ErrorMsg[100];
	CHAR		  Buffer  [256];

	memset(&Buffer,0,sizeof(Buffer));
	racal_get_error_warning_file_name_path();
	sprintf( Buffer,
		   "Starting the Service: %s, version %s",
			ServiceName, Version );
	racal_log_message( 2, 2,Buffer,"MainProcessor",0 );
	GetAppName( AppName );
	strcpy(MyName, AppName);
	strcpy(MyQueName, AppName);
	strcat(MyQueName, "A");
	strcpy(MyInteractiveQueName, AppName);
	strcat(MyInteractiveQueName, "I");
	strcpy(oprmon_que_name,"oprmonI");
   Zmk_Size = sizeof(ZMK);
   Auth_Tx_Size = sizeof(AUTH_TX);
   Ncf01_Size   = sizeof(NCF01);
   Dummy_Size   = sizeof(dummy);
   ReturnString_Size = sizeof(returnString);
   Outgoing_Buf_Size = Auth_Tx_Size + Ncf01_Size+ 10;

   GetXipcInstanceName( Buffer );

	/*Creates the QueuesA, QueuesI, QueuesC.  The QueuesA is used for other applications*/
	/*to send you a messages.  The QueuesI is used for interactive response to a message.*/
	/*The QueuesC (control) is used for system calls (for instance pinging).*/
	if( !pteipc_init_single_instance_app( ServiceName, Buffer ) )
	{
    	racal_log_message( 1, 1,"failed to create XIPC queues","MainProcessor",0 );
		pteipc_shutdown_single_instance_app();
		MainProcessDone = 1;
		return;
	}

/*This is the start of the main loop.*/
MYPRINT("ASCENDENT: MainProcessor, before the main loop.\n");

while(!EndProcessSignalled)
	{
		/* You are blocked here waiting for a message on either app queue or control que  */
		/* if there is no message on either que for 5 seconds, the blocking call returns   */
      p_msg = pteipc_receive( application_que_name, control_que_name, 5, &ret_code );

		if( p_msg != NULL_PTR )
		{
			MYPRINT("Ascendent: MainProcessor, got message\n");
			/*The next call, parses and handles the incoming message.*/
			AscendentMsgHandler( p_msg );
			free (p_msg);
		}
		else if( ret_code != QUE_ER_TIMEOUT )
		{
			pteipc_get_errormsg( ret_code, ErrorMsg );
			sprintf( Buffer, "PINNACLE: MainProcessor, error at pteipc_receive %s", ErrorMsg );
			racal_log_message( 1, 1,Buffer,"MainProcessor",0 );
		}
	}

	pteipc_shutdown_single_instance_app();
	MainProcessDone = 1;
}

/*************************************************************************************
       NAME:           EndProcess()

       DESCRIPTION:    This module is called after the main process is done.


       INPUTS:
			None.
       OUTPUTS:
            None.
       RETURNS:
		   None.

      AUTHOR:
      MODIFIED BY:    None.
*************************************************************************************/
void EndProcess()
{

   char Buffer[100] = "";

#ifdef WIN32
  Sleep(3000);
#else
  sleep(3);
#endif

   sprintf( Buffer, "Shutting down the %s Service, version %s",
            ServiceName, Version );
   racal_log_message( 1, 1,Buffer,"EndProcess",0 );

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



int calculate_julian_date(int mm,int dd, int year )
{
	int month[12] ={31,28,31,30,31,30,31,31,30,31,30,31};
	int Julian_date=0;

	for(int i=1;i<mm;i++)
	{
		Julian_date = Julian_date+month[i-1];
	}

	if (true==isLeap(year))
	{
		if(mm> 2)
		{
			Julian_date= Julian_date+dd+1;
		}
		else
		{
			Julian_date= Julian_date+dd;
		}
	}
	else
	{
		Julian_date= Julian_date+dd;
	}

	return Julian_date;

}

int getYear_from_JulianDate(int systemJD, int tranJD, int system_year)
{
	int tran_year = 0;

	if (systemJD == tranJD)
	{
		tran_year = system_year;

	}
	else if (systemJD > tranJD)
	{
		if (tranJD == 1 &&
				(systemJD == 365 || systemJD == 366))
		{
			tran_year = system_year + 1;
		}
		else
		{
			tran_year = system_year;
		}
	}
	else
	{
		if (systemJD  == 1 &&
				(tranJD == 365 || tranJD == 366))
		{
			tran_year = system_year - 1;
		}
		else
		{
			tran_year = system_year;
		}
	}

	return tran_year;

}

int racalEquitVerify_JCB3DS2_CVC2()
{
   char msgHeader[5];
   char cmdCode[3]={0},cvc1_serviceCode[4]={0}, cvc2_serviceCode[4]={0};
   char delimiter[2];
   char output[200],cardType = 'Z',csc[6]={0};
   char cvc1_data[4]={0}, cvc2_data[4]={0};
   int  track_present = 0, cvc2_present = 0, cvc2_sent = 0;
   int  loopidx = 0;
   int  cnt=0,result=0,jcnt=0;
   char BUFFER [256]={0};
   char strTemp[50] = {0};
   CHAR *tempptr,fieldSeperator[2]={0};
   CHAR csc_result5[6];
   CHAR csc_result4[5];
   CHAR csc_result3[4];
   INT 	flag=0;
   INT  AEVV_result_code;
   CHAR AEVV_second_factor[3]={0},AEVV_bin_second_factor[2];
   CHAR AEVV_iCSC[5]={0},AEVV_bin_iCSC[3];
   CHAR AEVV_UPN[5]={0},AEVV_bin_UPN[3];
   INT temp_out_len=0;

      memset (&msgHeader,0,sizeof(msgHeader));
   memset (&cmdCode,0,sizeof(cmdCode));
   memset (&delimiter,0,sizeof(delimiter));
   memset (&output,0,sizeof(output));
   memset (&returnString,0,ReturnString_Size);
   memset (&csc,0,sizeof(csc));

   strcpy (msgHeader ,"1234");


   strcpy (cmdCode,"CY");
   strcpy (delimiter,";");

			           /*edit CVK A */
         if(strlen(auth_tx.CAF01_details.csck) == 0)
		 {
            racal_log_message( 2, 2, "mag-strip csck is blank.","racalEquitVerifyCVV_CVC_CSC_CAV",1 );
            return(TRANSACTION_NOT_VERIFIED);
         }
         /*edit CVK B */
       /*  if(strlen(auth_tx.CAF01_details.cvk[0].cvkB) == 0)
         {
            racal_log_message( 2, 2, "mag-strip cvkB is blank.","racalEquitVerifyCVV_CVC_CSC_CAV",1 );
            return(TRANSACTION_NOT_VERIFIED);
         }*/

		 /*edit card number*/
	 	 if(strlen(auth_tx.TLF01_details.card_num) == 0 )
         {
            racal_log_message( 2, 2, "mag-strip card_num is blank.","racalEquitVerifyCVV_CVC_CSC_CAV",1 );
            return(TRANSACTION_NOT_VERIFIED);
         }

		 /*edit expiration date*/
         if(strlen(auth_tx.TLF01_details.exp_date) == 0)
         {
            racal_log_message( 2, 2, "mag-strip exp_date is blank.","racalEquitVerifyCVV_CVC_CSC_CAV",1 );
            return(TRANSACTION_NOT_VERIFIED);
         }

		 		 /*Get cvc2 if present*/
		 if(strlen(auth_tx.TLF01_details.cvc_data) != 0)
		 {

				 strcpy(cvc2_data, auth_tx.TLF01_details.cvc_data);
				 strcpy(cvc2_serviceCode, "000");
				 cvc2_present = 1;

		 }
		 else if(strlen(auth_tx.TLF01_details.interest_rate) != 0)
		 {


				 strcpy(cvc2_data, &auth_tx.TLF01_details.interest_rate[1]);
				 strcpy(cvc2_serviceCode, "000");
				 cvc2_present = 1;

		 }
		 		 /*If neither cvc1 or cvc2 is present, return no value*/
		 if( !track_present && !cvc2_present)
			 return (TRANSACTION_VERIFIED);

		for ( loopidx = 0; loopidx < 2; loopidx++)
		 {
			 if (loopidx == 0 && track_present) /*cvc1 is present and needs to be verified*/
			 {
				 /*Build message and send to Racal.  */
				 memset (&output,0,sizeof(output));
				 strcat(output, msgHeader);
				 strcat(output, cmdCode);
				 strcat(output, auth_tx.CAF01_details.csck);
				 strcat(output, cvc1_data);
				 strcat(output, auth_tx.TLF01_details.card_num);
				 strcat(output, delimiter);
				 strcat(output, auth_tx.TLF01_details.exp_date);
				 strcat(output, cvc1_serviceCode);
			 }
			 else /*cvc2 needs to be verifed if present*/
			 {
				 /*Build message for cvc2 verification and send to Racal*/
				 memset (&output,0,sizeof(output));
				 strcat(output, msgHeader);
				 strcat(output, cmdCode);
				 strcat(output, auth_tx.CAF01_details.cvk[0].cvkA);
				 strcat(output, auth_tx.CAF01_details.cvk[0].cvkB);
				 strcat(output, cvc2_data);
				 strcat(output, auth_tx.TLF01_details.card_num);
				 strcat(output, delimiter);
				 strcat(output, auth_tx.TLF01_details.exp_date);
				 strcat(output, cvc2_serviceCode);

				  cvc2_sent = 1;
			 }

			 			 //if the output is bigger then 150 do not send it out.
			 if (strlen(output) > 150)
			 {
				racal_log_message( 2, 2, "mag-strip Cmd Msg to long.","racalEquitVerifyCVV_CVC_CSC_CAV",1 );
				if (cvc2_sent)
					return(CVC2_TRANSACTION_NOT_VERIFIED);
				else
					return(TRANSACTION_NOT_VERIFIED);
			 }

			 memset(BUFFER,0,sizeof(BUFFER));
			 sprintf(BUFFER, "BEFORE-PORT: %s", output);
			 MYPRINT(BUFFER);

			 /*The message is built.  Send it over to the Racal box.*/
			 /*Once the a response is recieved, check for the message header,*/
			 /*response code, and the error code.  If it does not exactly match,*/
			 /*send TRANSACTION_NOT_VERIFIED back.*/
			 racal_log_message( 2, 2, output,"racalEquitVerifyCVV_CVC_CSC_CAV",1 );
			 if (racalSendToPort(output) == TRANSACTION_VERIFIED)
			 {
				/*parse the message check for the message header.*/
				memset(&dummy,0,Dummy_Size);
				strncpy(dummy,returnString,4);
				if(strcmp(dummy,msgHeader) != 0)
				{
				   racal_log_message( 2, 2, "mag-strip Msg Header mis-match.","racalEquitVerifyCVV_CVC_CSC_CAV",1 );
				   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}

				/*parse the message check for the response code.*/
				memset(&dummy,0,Dummy_Size);
				strncpy(dummy,returnString + 4, 2);
				if(strcmp(dummy,"CZ") !=0)
				{
				   racal_log_message( 2, 2, "mag-strip Cmd Msg mis-match.","racalEquitVerifyCVV_CVC_CSC_CAV",1 );
				   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}

            /* Parse the message check for the error code. */
            memset(&dummy,0,Dummy_Size);
            strncpy(dummy,returnString + 6, 2);
            if(strcmp(dummy,"00") == 0)
            {
               if (cvc2_sent)
                  return(CVC2_TRANSACTION_VERIFIED);
               else
                  if(!cvc2_present)
                     return (TRANSACTION_VERIFIED); /*cvc1 valid*/
                  else
                     continue; /*continue with loop to verify cvc2*/
            }

				if(strcmp(dummy,"01") == 0)
				{
				   racal_log_message( 2, 2, "CVV-VERIFY failure.","racalEquitVerifyCVV_CVC_CSC_CAV",1 );
				   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}
				if(strcmp(dummy,"10") == 0)
            {
					racal_log_message( 2, 2, "CVV-VERIFY CVK A or B parity error.","racalEquitVerifyCVV_CVC_CSC_CAV",1 );
               	   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}

				if(strcmp(dummy,"12") == 0)
				{
				   racal_log_message( 2, 2, "CVV-VERIFY No Key loaded in user storage.","racalEquitVerifyCVV_CVC_CSC_CAV",1 );
				   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}

				if(strcmp(dummy,"13") == 0)
				{
				   racal_log_message( 2, 2, "CVV-VERIFY LMK error report to superviser.","racalEquitVerifyCVV_CVC_CSC_CAV",1 );
				   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}

				if(strcmp(dummy,"15") == 0)
				{
				   racal_log_message( 2, 2, "CVV-VERIFY Error in input data.","racalEquitVerifyCVV_CVC_CSC_CAV",1 );
				   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}

				if(strcmp(dummy,"21") == 0)
				{
				   racal_log_message( 2, 2, "CVV-VERIFY Invalid user storage index.","racalEquitVerifyCVV_CVC_CSC_CAV",1 );
				   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
				}

			 }
			 else  /* else for: if (racalSendToPort(output) == TRANSACTION_VERIFIED)*/
			 {
				   if (cvc2_sent)
					   return(CVC2_TRANSACTION_NOT_VERIFIED);
				   else
					   return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/
			 }

		 }/*end for loop*/

		 return (TRANSACTION_NOT_VERIFIED); /*cvc1 invalid*/




}

