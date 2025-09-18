
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <signal.h>
#include "ptemsg.h"
#include "pteipc.h"
#include "txtrace.h"
#include "ptesystm.h"
#include "ntutils.h"
#include "txutils.h"
#include "memmnger.h"
#include "genutil.h"
#include "txutils.h"
#include "basictyp.h" 
#include "app_info.h"
#include "equitdb.h"
#include "tx_dbstruct.h"
#include "dc_dbstruct.h"
#include "nc_dbstruct.h"
#include "ptetime.h"
#include "FalconHandler.h"
#include "global_definitions.h"
#include "Falcon.h"

#define   AMX_INVALID_TXN                "12"
#ifndef  false
#define  false    0x00
#endif

#ifndef  true
#define  true     0x01
#endif
INT FALCON_read_CUP_merchant_indicator( void );
void Falcon_build_CRTRAN_field_31_MC();
void Falcon_build_CRTRAN_field_31_VISA();
void Falcon_build_CRTRAN_field_31_CUP();
void Falcon_build_CRTRAN_field_31_JCB();
void Falcon_build_CRTRAN_field_31_DINERS();
void Falcon_build_CRTRAN_field_31_AMEX();
void Falcon_build_CRTRAN_field_31_Fleet();
INT FALCON_Read_BDO_PAY_QR_NII_value(void);
INT Check_If_CUP_Transaction(void);
INT Check_If_VISA_Transaction (void);
INT Check_If_MC_Transaction (void);
INT Check_If_JCB_Transaction (void);
INT Check_If_AMEX_Transaction (void);
INT Check_If_DINERS_Transaction (void);








CHAR  CUP_merchant_indicator[8]={0};
CHAR  CUP_MP_Bin_range_1[11]={0};
CHAR  CUP_MP_Bin_range_2[11]={0};
CHAR  CUP_MP_Bin_range_3[11]={0};
CHAR  CUP_MP_Bin_range_4[11]={0};
CHAR  CUP_MP_Bin_range_5[11]={0};
INT   CUP_MP_Bin_range_len_1 = 0;
INT   CUP_MP_Bin_range_len_2 = 0;
INT   CUP_MP_Bin_range_len_3 = 0;
INT   CUP_MP_Bin_range_len_4 = 0;
INT   CUP_MP_Bin_range_len_5 = 0;

/*BDO pay QR transaction*/
CHAR  BDO_PAY_QR_NII_1[5]={0};
CHAR  BDO_PAY_QR_NII_2[5]={0};
CHAR  BDO_PAY_QR_NII_3[5]={0};
CHAR  BDO_PAY_QR_NII_4[5]={0};
CHAR  BDO_PAY_QR_NII_5[5]={0};



/*#ifdef	_DEBUG
	#define PRINT(x) printf(x) 
#else	
	#define PRINT
#endif */

struct
{
	char    HeaderFile                      [69];
	char	WORKFLOW_XCD					[16];
	char	RECORD_TYPE_XCD					[8];
	char	DATA_SPECIFICATION_VERSION_XCD	[5];
	char	CLIENT_XID						[16];
	char	RCD_CRT_DATE					[8];
	char	RCD_CRT_TIME					[6];
	char	RCD_CRT_MSCND					[3];
	char	GMT_OFFSET						[6];
	char	CUSTOMER_XID					[20];
	char	ACCOUNT_REFERENCE_XID			[40];
	char	EXTERNAL_TRANSACTION_XID		[32];
	char	SCORE_CUSTOMER_ACCOUNT_XID		[19];
	char	AUTHORIZATION_POSTING_XCD		[1];
	char	CARD_POSTAL_XCD					[9];
	char	CARD_COUNTRY_XCD				[3];
	char	CARD_OPEN_DT					[8];
	char	CARD_ISSUE_DT					[8];
	char	CARD_ISSUE_TYPE_XCD				[1];
	char	ACCOUNT_EXPIRATION_DT			[8];
	char	CARD_EXPIRATION_DT				[8];	
	char	PRIOR_CREDIT_LIMIT_AMT			[10];
	char	CARD_CREDIT_LIMIT_AMT			[10];
	char	CUSTOMER_GENDER_XCD				[1];
	char	CUSTOMER_BIRTH_DT				[8];
	char	CUSTOMER_CARD_CNT				[3];
	char	INCOME_AMT						[10];
	char	CARD_TYPE_XCD					[1];
	char	PRIMARY_CARD_USE_XCD			[1];
	char	TRANSACTION_DATE				[8];
	char	TRANSACTION_TIME				[6];
	char	TRANSACTION_AMT					[13];
	char	TRANSACTION_ISO_CURRENCY_XCD	[3];
	char	TRAN_ISO_CURRENCY_CNVRSN_RATIO	[13];
	char	DECISION_XCD					[1];
	char	TRANSACTION_TYPE_XCD			[1];
	char	MERCHANT_CATEGORY_XCD			[4];
	char	MERCHANT_POSTAL_XCD				[9];
	char	MERCHANT_COUNTRY_XCD			[3];
	char	TRANSACTION_PIN_VERIFY_XCD		[1];
	char	CVV_VERIFY_XCD					[1];
	char	TRANSACTION_POSTING_ENTRY_XFLG	[1];
	char	TRANSACTION_POSTING_DT			[8];
	char	AUTHORIZATION_POSTING_MIS_XCD	[1];
	char	TRANS_POSTING_MISMATCH_XFLG		[1];
	char	CREATE_CASE_XFLG				[1];
	char	USER_INDICATOR_1_XCD			[1];
	char	USER_INDICATOR_2_XCD			[1];
	char	USER_DATA_1_STRG				[10];
	char	USER_DATA_2_STRG				[10];
	char	MERCHANT_XIDD					[10];
	char	MERCHANT_DATA_XFLG				[1];
	char	CARD_DATA_AVAILABLE_XFLG		[1];
	char	EXTERNAL_1_SCR					[4];
	char	EXTERNAL_2_SCR					[4];
	char	EXTERNAL_3_SCR					[4];
	char	CARDHOLDER_PRESENT_XFLG			[1];
	char	CAT_TYPE_XFLG					[1];
	char	TESTING_RANDOM_DIGITS_STRG		[2];
	char	PORTFOLIO_NAME					[14];
	char	CLIENT_2_XID					[14];
	char	INTERBANK_CARD_ASSOCIATION_NUM	[6];
	char	MERCHANT_NAME					[40];
	char	MERCHANT_CITY_NAME				[30];
	char	MERCHANT_STATE_XCD				[3];
	char	SUPPRES_CASE_XFLG				[1];
	char	USER_INDICATOR_3_XCD			[5];
	char	USER_INDICATOR_4_XCD			[5];
	char	USER_DATA_3_STRG				[15];
	char	USER_DATA_4_STRG				[20];
	char	USER_DATA_5_STRG				[40];
	char	REAL_TIME_REQUEST_XFLG			[1];
	char	PRIOR_ACTION_DATABASE_XCD		[1];
	char	PRIOR_ACTION_DB_EXPIRATION_DT	[8];
	char	MASTER_ACCOUNT_NUMBER_XID		[19];
	char	CARD_OFFLN_STATIC_AUTHCTN_XFLG	[1];
	char	CARD_DYNAMIC_AUTHCTN_XCD		[1];
	char	RESERVED_02						[1];
	char	CARD_AIP_VERIFICATION_XFLG		[1];
	char	CARD_AIP_RISK_XFLG				[1];
	char	CARD_ISSUER_AUTH_SUPPORT_XFLG	[1];
	char	CARD_COMBINED_AUTHCTN_XFLG		[1];
	char	TRANSACTION_DELINQUENT_CYC		[1];
	char	TRANS_TOTAL_DELINQUENT_AMT		[13];
	char	CRD_STATEMENT_CASH_BALANCE_AMT	[13];
	char	MERCHANT_BALANCE_AMT			[13];
	char	CUSTOMER_POSTAL_XCD				[12];
	char	CARD_MEDIA_TYPE_XCD				[1];
	char	CVV2_PRESENT_XFLG				[1];
	char	CVV2_RESPONSE_XFLG				[1];
	char	AVS_RESPONSE_XCD				[1];
	char	TRANSACTION_CATEGORY_XCD		[1];
	char	ACQUIRER_XID					[12];
	char	ACQUIRER_COUNTRY_XCD			[3];
	char	TERMINAL_XID					[16];
	char	TERMINAL_TYPE_XCD				[1];
	char	TERMINAL_ENTRY_CAPABILITY_XCD	[1];
	char	TRANSACTION_POS_CONDITION_XCD	[2];
	char	ATM_NETWORK_XID					[1];
	char	RESERVED_01						[1];
	char	CHECK_NUM						[6];
	char	TERML_VERIFICATION_RESULTS_XCD	[10];
	char	CARD_VERIFICATION_RESULTS_XCD	[10];
	char	AUTHZN_RQST_CRYPTO_VALID_XFLG	[1];
	char	ATC_CARD_CNT					[5];
	char	ATC_HOST_CNT					[5];
	char	RESERVED_03						[2];
	char	TERML_TO_CRD_OFFLINE_LIMIT_XCD	[2];
	char	TERMINAL_OFFLINE_LIMIT_MAX_XCD	[2];
	char	RECURRING_AUTHORIZATION_XCD		[2];
	char	RECURRING_AUTHZN_EXPIRATION_DT	[8];
	char	CARD_ASSOCIATION_XFLG			[1];
	char	CARD_INCENTIVE_XCD				[1];
	char	CARD_STATUS_XCD					[2];
	char	CARD_STATUS_DT					[8];
	char	PROCESSOR_REASON_XCD			[5];
	char	TRANSACTION_ADVICE_XCD			[1];
	char	MERCHANT_XID					[16];
	char	CARD_ORDER_TYPE_XCD				[1];
	char	TRANSACTION_CASHBACK_AMT		[13];
	char	USER_DATA_6_NUM					[13];
	char	USER_DATA_7_STRG				[40];
	char	PAYMENT_INSTRUMENT_XID			[30];
	char	AVS_REQUEST_XCD					[1];
	char	CVR_OFFLINEPIN_VERIFYPERF_XFLG	[1];
	char	CVR_OFFLINEPIN_VERIFYSTAT_XFLG	[1];
	char	CVR_PIN_TRYLIMIT_EXCEED_XFLG	[1];
	char	POS_TERMINAL_ATTEND_XFLG		[1];
	char	POS_OFF_PREMISES_XFLG			[1];
	char	POS_CARD_CAPTURE_XFLG			[1];
	char	POS_SECURITY_XFLG				[1];
	char	AUTHORIZATION_XID				[6];
	char	USER_DATA_8_STRG				[10];
	char	USER_DATA_9_STRG				[10];
	char	USER_INDICATOR_5_XCD			[1];
	char	USER_INDICATOR_6_XCD			[1];
	char	USER_INDICATOR_7_XCD			[5];
	char	USER_INDICATOR_8_XCD			[5];
	char	MODEL_CONTROL_1_XCD				[1];
	char	MODEL_CONTROL_2_XCD				[1];
	char	MODEL_CONTROL_3_XCD				[1];
	char	MODEL_CONTROL_4_XCD				[1];
	char	AUTHORIZATION_EXPIRE_DT_XFLG	[1];
	char	AUTH_SECONDARY_VERIFY_XCD		[1];
	char	AUTHORIZATION_RESPONSE_XCD		[1];
}Cr_Fld;


#define INTIALIZE_QUEUES_NO_ERROR 00
#define INTIALIZE_QUEUES_ERROR 01
#define INTIALIZE_EXCEPTION_ERROR 02
#define MAX_INSTANCE_NAME_SIZE   51
#define FIELD67_SIZE 			 5
#define SPACES 					 ' '
/* Currency Codes */
#define  PESO     "608"
#define  USD      "840"
#define CURRENCY_LENGTH "3"
CHAR strAppName [80];
CHAR ServiceName[80];
AUTH_TX     Auth_Tx;
char javaEndProcessSignalled[1];

CHAR  Version[] = "ATP_11.1.0";
static CHAR message[1023]= {0};
static long count = 0;
INT  MainProcessDone;
INT  EndProcessSignalled;
CHAR  date_str [ 9] = "";
CHAR  time_str [ 7] = "";
CHAR  time_date[25] = "";
CHAR  time_milliseconds[4] = "";
CHAR profile_str [17]="";

double   conv_rate   = 0.0;
double temp_conv_amount = 0.0;
char temp_without_decimal[13] = {0};
int len_billing_amount = 0;
int len_billing_currency_code = 0;
int len_settle_currency_code = 0;
double billing_amount = 0.0;
double settlement_total = 0.0;
char last2_decimal[3]={0};
char amount_digit[11]= {0};
char str_temp_amount[13] = {0};
double temp_amount = 0.0;
char temp_iso_ration[25] = {0};

CHAR  falcon_Erro_warning_Filename[256] = {0};
CHAR  falcon_module_error_warning_file_name[256] = {0};
CHAR  falcon_error_warning_file_path[256] = {0};
UINT Max_File_Size_Defined = 0 ;
CHAR  App_Name[7] = "falcon";


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

/******************************************************************************
*
*  NAME:         GET_TIME
*
*  DESCRIPTION:  This procedure takes an input string that contains a 
date and
*                time.  It copies the time portion into an output 
string,
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
    memset ( time_str,0x00,7 );
    memset(time_milliseconds, 0x00,4 );
    strncpy( time_str, &time_date[11], 2 );
    strncat( time_str, &time_date[14], 2 );
    strncat( time_str, &time_date[17], 2 );
    strncpy( time_milliseconds, &time_date[20], 3 );
 }



/*FILE *fp1;
char *fpath ="/home/veena/ascendent/log/Recdsent_java.log";

FILE *fp2;
char *fpath1 ="/home/veena/ascendent/log/Recdrec_java.log";*/



/*CHAR mssg_type[8] = {"0500","0320","0800","0400","0220","0830","0530","0200"};*/
/*CHAR pr_cd[10]={"920000","960000","910000","990000","040000","900000","950000","930000","200000","000000");*/

char *doubletostr(double f)
{

     static char s[13];
     int i, j = 0;
     sprintf(s,"%012.2lf",f);
     for ( i = 0; s[i] != '\0'; i++ )
     {
         if ( s[i] != '.')
         {
             s[j++] = s[i];
         }
     }
     s[j] = '\0';
     return s;
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

INT Is_BDO_Acquiring_Transaction(void)
{
	if((strncmp(Auth_Tx.TLF01_details.handler_queue,"dcp",3) == 0)||
		(strncmp(Auth_Tx.TLF01_details.handler_queue,"VOICE",5) == 0))
	 {
	    return true;
	 }
	 else
	 {
		 return false;
	 }

}

INT Is_BDO_Pos_Acquiring_Transaction(void)
{
	if((strncmp(Auth_Tx.TLF01_details.handler_queue,"dcp",3) == 0))
	{

		return true;
	}
	else
	{
		return false;
	}

}

INT Falcon_is_Issuing_Transaction(void)
{
	if((strncmp(Auth_Tx.TLF01_details.handler_queue,"dcp",3) != 0) &&
			(strncmp(Auth_Tx.TLF01_details.handler_queue,"VOICE",5) != 0))
	{
		return true;
	}
	else
	{
		return false;
	}
}
INT FALCON_is_Credential_On_File_VISA(pCHAR temp_pos_entry_mode)
{

	if((Falcon_is_Issuing_Transaction()) == true)
	{
		if((strncmp(temp_pos_entry_mode,"10",2)== 0))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if((Is_BDO_Pos_Acquiring_Transaction()) == true)
	{
		if((strncmp(temp_pos_entry_mode,"01",2)==0) &&
			(strcmp(Auth_Tx.TLF01_details.pos_condition_code,"08") == 0))
		{
			return true;
		}
		else
		{
			return false;
		}


	}


}

INT FALCON_is_Credential_On_File_MC(pCHAR temp_pos_entry_mode)
{

	if((Falcon_is_Issuing_Transaction()) == true)
	{
		if((strncmp(temp_pos_entry_mode,"10",2) == 0))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if((Is_BDO_Pos_Acquiring_Transaction()) == true)
	{
		if((strncmp(temp_pos_entry_mode,"01",2)==0) &&
					(strcmp(Auth_Tx.TLF01_details.pos_condition_code,"08") == 0))
		{
			return true;
		}
		else
		{
			return false;
		}


	}


}

INT FALCON_is_Credential_On_File_AMEX(pCHAR temp_pos_entry_mode)
{
	if((Falcon_is_Issuing_Transaction()) == true)
	{
		if( (Auth_Tx.amex_pos_entry_mode[6]== 'A' ))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if((Is_BDO_Pos_Acquiring_Transaction()) == true)
	{
		if((strncmp(temp_pos_entry_mode,"01",2)==0) &&
			(strcmp(Auth_Tx.TLF01_details.pos_condition_code,"08") == 0))
		{
			return true;
		}
		else
		{
			return false;
		}


	}
}

INT FALCON_is_Credential_On_File_DINERS(pCHAR temp_pos_entry_mode)
{

	if((Falcon_is_Issuing_Transaction()) == true)
	{
		if((Auth_Tx.TLF01_details.product_codes[16].amount[6])== 'B' )
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if((Is_BDO_Pos_Acquiring_Transaction()) == true)
	{
		if((strncmp(temp_pos_entry_mode,"01",2) == 0) &&
			(strcmp(Auth_Tx.TLF01_details.pos_condition_code,"08") == 0))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}

INT FALCON_is_Credential_On_File_CUP(pCHAR temp_pos_entry_mode)
{

	if((Is_BDO_Pos_Acquiring_Transaction()) == true)
	{
		if((strncmp(temp_pos_entry_mode,"01",2) == 0) &&
			(strcmp(Auth_Tx.TLF01_details.pos_condition_code,"08") == 0))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}

INT FALCON_is_Credential_On_File_JCB(pCHAR temp_pos_entry_mode)
{

	if((Is_BDO_Pos_Acquiring_Transaction()) == true)
	{
		if((strncmp(temp_pos_entry_mode,"01",2) == 0) &&
				(strcmp(Auth_Tx.TLF01_details.pos_condition_code,"08") == 0))
		{
			return true;
		}
		else
		{
			return false;
		}
	}

}

void Falcon_AMEX_posEntryMode_F41(pCHAR temp_pos_entry_mode)
{
		if((Falcon_is_Issuing_Transaction()) == true)
		{
			if( (Auth_Tx.amex_pos_entry_mode[5]== 'X' ) && (Auth_Tx.amex_pos_entry_mode[6] == '2'))
			{
				strncpy(temp_pos_entry_mode,"2W",2); /* Contact less mag stripe*/
			}
			else if( (Auth_Tx.amex_pos_entry_mode[4]  == 'S') ||
					(Auth_Tx.amex_pos_entry_mode[4]  == 'T'))
			{
				strncpy(temp_pos_entry_mode,"81",2);
			}
			else if (Auth_Tx.amex_pos_entry_mode[6]  == '5')
			{
				strncpy(temp_pos_entry_mode,"05",2);
			}
			else if( (Auth_Tx.amex_pos_entry_mode[6] == 'W' || Auth_Tx.amex_pos_entry_mode[6] == '2') )
			{
				strncpy(temp_pos_entry_mode,"90",2); /* Contact less mag stripe*/
			}
			else if( (Auth_Tx.amex_pos_entry_mode[6]== '9')	)
			{
				strncpy(temp_pos_entry_mode,"80",2);
			}
			else if( (Auth_Tx.amex_pos_entry_mode[5]== 'X' && Auth_Tx.amex_pos_entry_mode[6] == '5'))
			{
				strncpy(temp_pos_entry_mode,"X5",2); /* Contact less micro chip*/
			}
			else if( (Auth_Tx.amex_pos_entry_mode[5]== '1' && Auth_Tx.amex_pos_entry_mode[6] == '5'))
			{
				strncpy(temp_pos_entry_mode,"05",2); /*  chip*/
			}
			else if( (Auth_Tx.amex_pos_entry_mode[0]== '5' && Auth_Tx.amex_pos_entry_mode[5] == '1' && Auth_Tx.amex_pos_entry_mode[6] == '2') ||
			 (Auth_Tx.amex_pos_entry_mode[0]== '5' && Auth_Tx.amex_pos_entry_mode[5] == '1' && Auth_Tx.amex_pos_entry_mode[6] == 'W') ||
			 (Auth_Tx.amex_pos_entry_mode[6] == '9'))
			{
				strncpy(temp_pos_entry_mode,"80",2); /* Fall back Mag stripe*/
			}
			else if( (Auth_Tx.amex_pos_entry_mode[0]== '5' && Auth_Tx.amex_pos_entry_mode[5] == '1' && Auth_Tx.amex_pos_entry_mode[6] == 'S') ||
			 (Auth_Tx.amex_pos_entry_mode[0]== '5' && Auth_Tx.amex_pos_entry_mode[5] == '1' && Auth_Tx.amex_pos_entry_mode[6] == '6') ||
			 (Auth_Tx.amex_pos_entry_mode[0]== '5' && Auth_Tx.amex_pos_entry_mode[5] == '1' && Auth_Tx.amex_pos_entry_mode[6] == '1') ||
			 (Auth_Tx.amex_pos_entry_mode[0]== '5' && Auth_Tx.amex_pos_entry_mode[5] == '1' && Auth_Tx.amex_pos_entry_mode[6] == 'V'))
			{
				strncpy(temp_pos_entry_mode,"79",2); /* Fall back keyed entry*/
			}
			else if( Auth_Tx.amex_pos_entry_mode[0] ==  '1' ||
			 Auth_Tx.amex_pos_entry_mode[6] ==  '6' ||
			 Auth_Tx.amex_pos_entry_mode[6] ==  'S' ||
			 Auth_Tx.amex_pos_entry_mode[6] ==  'V' )
			{
				strncpy(temp_pos_entry_mode,"01",2); /* keyed in*/
			}
			else if(strncmp(Auth_Tx.amex_pos_entry_mode,"6609S06S4140",12)== 0 ||
			strncmp(Auth_Tx.amex_pos_entry_mode,"6609S0SS4140",12)== 0 ||
			strncmp(Auth_Tx.amex_pos_entry_mode,"660101654140",12)== 0 ||
			strncmp(Auth_Tx.amex_pos_entry_mode,"2009106S0040",12)== 0 ||
			(Auth_Tx.amex_pos_entry_mode[4]== 'S'))
			{
				strncpy(temp_pos_entry_mode,"81",2); /* E-commerce5*/
			}
			else if(strncmp(Auth_Tx.amex_pos_entry_mode,"660101S54140",12)== 0	||
					strncmp(Auth_Tx.amex_pos_entry_mode,"660390664130",12)== 0	||
					strncmp(Auth_Tx.amex_pos_entry_mode,"660390S64130",12)== 0	||
					strncmp(Auth_Tx.amex_pos_entry_mode,"660320664130",12)== 0	||
					strncmp(Auth_Tx.amex_pos_entry_mode,"660320S64130",12)== 0	||
					strncmp(Auth_Tx.amex_pos_entry_mode,"660101S54130",12)== 0	||
					strncmp(Auth_Tx.amex_pos_entry_mode,"660101V54130",12)== 0)
			{
				strncpy(temp_pos_entry_mode,"01",2);
			}
			else if(strncmp(Auth_Tx.amex_pos_entry_mode,"260101254140",12)== 0 ||
					strncmp(Auth_Tx.amex_pos_entry_mode,"260101W54140",12)== 0	   ||
					strncmp(Auth_Tx.amex_pos_entry_mode,"201101W00001",12)== 0)
			{
				strncpy(temp_pos_entry_mode,"90",2);
			}
			else if(strcmp(Auth_Tx.TLF01_details.pos_entry_mode,"0072") == 0 ||
					strcmp(Auth_Tx.TLF01_details.pos_entry_mode,"0070") == 0 )
			{
				strncpy(temp_pos_entry_mode,"01",2);
			}
			else if(strcmp(Auth_Tx.TLF01_details.pos_entry_mode,"0022") == 0 ||
					strcmp(Auth_Tx.TLF01_details.pos_entry_mode,"0062") == 0)
			{
				strncpy(temp_pos_entry_mode,"90",2);
			}
			else if( (strcmp(temp_pos_entry_mode,"01") == 0 || strcmp(temp_pos_entry_mode,"07") == 0)  &&
					strcmp(Auth_Tx.TLF01_details.pos_condition_code,"59") == 0)
			{
				strncpy(temp_pos_entry_mode,"81",2);
			}
			else if( (Auth_Tx.amex_pos_entry_mode[0]== '1' && Auth_Tx.amex_pos_entry_mode[6] == 'S') ||
					(Auth_Tx.amex_pos_entry_mode[0]== '6' && Auth_Tx.amex_pos_entry_mode[6] == 'S') ||
					(Auth_Tx.amex_pos_entry_mode[0]== '1' && Auth_Tx.amex_pos_entry_mode[6] == '6') ||
					(Auth_Tx.amex_pos_entry_mode[0]== '6' && Auth_Tx.amex_pos_entry_mode[6] == '6') ||
					(Auth_Tx.amex_pos_entry_mode[4]== 'S'))
			{
				strncpy(temp_pos_entry_mode,"81",2); /* E-commerce*/
			}
			else if(strncmp(temp_pos_entry_mode,"00",2) == 0)
			{
				strncpy(temp_pos_entry_mode,"81",2); /* E-commerce*/
			}
		}
		else
		{
			if( strncmp(&Auth_Tx.TLF01_details.pos_entry_mode[1],"07",2) == 0 )
			{
				strncpy(temp_pos_entry_mode,"07",2);
			}
			else if( strncmp(&Auth_Tx.TLF01_details.pos_entry_mode[1],"07",2) == 0 &&
			 strncmp(Auth_Tx.TLF01_details.pos_condition_code,"59",2)== 0)
			{
				strncpy(temp_pos_entry_mode,"81",2);
			}
			else if ( strncmp(&Auth_Tx.TLF01_details.pos_entry_mode[1],"80",2) == 0)
			{
				strncpy(temp_pos_entry_mode,"80",2);
			}
			else if ( strncmp(&Auth_Tx.TLF01_details.pos_entry_mode[1],"09",2) == 0)
			{
				strncpy(temp_pos_entry_mode,"79",2);
			}
			else if ( strncmp(&Auth_Tx.TLF01_details.pos_entry_mode[1],"01",2) == 0)
			{
				strncpy(temp_pos_entry_mode,"01",2);
			}
			else if ( strncmp(&Auth_Tx.TLF01_details.pos_entry_mode[1],"02",2) == 0)
			{
				strncpy(temp_pos_entry_mode,"90",2);
			}
			else if ( strncmp(&Auth_Tx.TLF01_details.pos_entry_mode[1],"05",2) == 0)
			{
				strncpy(temp_pos_entry_mode,"05",2);
			}
			else
			{
				strcpy(temp_pos_entry_mode,"");
			}
		}

}

void Falcon_DINERS_posEntryMode_F41(pCHAR temp_pos_entry_mode)
{
			if((Is_BDO_Acquiring_Transaction()) == true)
			{
				if( strncmp(&Auth_Tx.TLF01_details.pos_entry_mode[1],"07",2) == 0 )
				{
					strncpy(temp_pos_entry_mode,"07",2);
				}
				else if ( strncmp(&Auth_Tx.TLF01_details.pos_entry_mode[1],"81",2) == 0)
				{
					strncpy(temp_pos_entry_mode,"81",2);
				}
				else if ( strncmp(&Auth_Tx.TLF01_details.pos_entry_mode[1],"80",2) == 0)
				{
					strncpy(temp_pos_entry_mode,"80",2);
				}
				else if ( strncmp(&Auth_Tx.TLF01_details.pos_entry_mode[1],"09",2) == 0)
				{
					strncpy(temp_pos_entry_mode,"79",2);
				}
				else if ( strncmp(&Auth_Tx.TLF01_details.pos_entry_mode[1],"01",2) == 0)
				{
					strncpy(temp_pos_entry_mode,"01",2);
				}
				else if ( strncmp(&Auth_Tx.TLF01_details.pos_entry_mode[1],"02",2) == 0)
				{
					strncpy(temp_pos_entry_mode,"90",2);
				}
				else if ( strncmp(&Auth_Tx.TLF01_details.pos_entry_mode[1],"05",2) == 0)
				{
					strncpy(temp_pos_entry_mode,"05",2);
				}
				else
				{
					strcpy(temp_pos_entry_mode,"");
				}
			}
			else
			{
				if( Auth_Tx.TLF01_details.product_codes[16].amount[6] == '7' ||
					Auth_Tx.TLF01_details.product_codes[16].amount[6] == 'T' ||
					Auth_Tx.TLF01_details.product_codes[16].amount[6] == 't' )
				{
					strncpy(temp_pos_entry_mode,"91",2);
				}
				else if( Auth_Tx.TLF01_details.product_codes[16].amount[6] == 'S')
				{
					strncpy(temp_pos_entry_mode,"92",2);
				}
				else if( Auth_Tx.TLF01_details.product_codes[16].amount[4] == '9')
				{
					strncpy(temp_pos_entry_mode,"81",2);
				}
				else if( Auth_Tx.TLF01_details.product_codes[16].amount[6] == '9')
				{
					strncpy(temp_pos_entry_mode,"80",2);
				}
				else if( Auth_Tx.TLF01_details.product_codes[16].amount[6] == '1' ||
						 Auth_Tx.TLF01_details.product_codes[16].amount[6] == '6' )
				{
					strncpy(temp_pos_entry_mode,"01",2);
				}
				else if( Auth_Tx.TLF01_details.product_codes[16].amount[6] == '2')
				{
					strncpy(temp_pos_entry_mode,"90",2);
				}
				else if( Auth_Tx.TLF01_details.product_codes[16].amount[6] == '5')
				{
					strncpy(temp_pos_entry_mode,"05",2);
				}
				else
				{
					strcpy(temp_pos_entry_mode,"");
				}
			}

}

void Falcon_CUP_posEntryMode_F41(pCHAR temp_pos_entry_mode)
{
			if((Is_BDO_Acquiring_Transaction()) == true)
			{
				if( strncmp(&Auth_Tx.TLF01_details.pos_entry_mode[1] , "07" ,2) == 0)
				{
					strncpy(temp_pos_entry_mode,"07",2);
				}
				else if (	strncmp(&Auth_Tx.TLF01_details.pos_entry_mode[1] , "80" ,2) == 0 )
				{
					strncpy(temp_pos_entry_mode,"80",2);
				}
				else if (strncmp(&Auth_Tx.TLF01_details.pos_entry_mode[1] , "01" ,2) == 0 )
				{
					strncpy(temp_pos_entry_mode,"01",2);
				}
				else if (strncmp(&Auth_Tx.TLF01_details.pos_entry_mode[1] , "05" ,2) == 0)
				{
					strncpy(temp_pos_entry_mode,"05",2);
				}
				else if (strncmp(&Auth_Tx.TLF01_details.pos_entry_mode[1] , "81" ,2) == 0)
				{
					strncpy(temp_pos_entry_mode,"81",2);
				}
				else if(strncmp(&Auth_Tx.TLF01_details.pos_entry_mode[1] , "02" ,2) == 0)
				{
					strncpy(temp_pos_entry_mode,"90",2); /* U*/
				}
				else if (strncmp(&Auth_Tx.TLF01_details.pos_entry_mode[1] , "09" ,2) == 0)
				{
					strncpy(temp_pos_entry_mode,"79",2);
				}
				else
				{
					strcpy(temp_pos_entry_mode,"");
				}
			}
			else
			{
				if( strncmp(Auth_Tx.TLF01_details.pos_entry_mode , "98" ,2) == 0)
				{
					strncpy(temp_pos_entry_mode,"91",2);
				}
				else if (strncmp(Auth_Tx.TLF01_details.pos_entry_mode , "07" ,2) == 0 )
				{
					strncpy(temp_pos_entry_mode,"07",2);
				}
				else if (strncmp(Auth_Tx.TLF01_details.pos_entry_mode , "01" ,2) == 0 &&
						 strncmp(Auth_Tx.TLF01_details.pos_condition_code , "42" ,2) == 0)
				{
					strncpy(temp_pos_entry_mode,"81",2);
				}
				else if((strncmp(Auth_Tx.TLF01_details.pos_entry_mode , "02" ,2) == 0 ||
						 strncmp(Auth_Tx.TLF01_details.pos_entry_mode , "90" ,2) == 0) &&
						(strncmp(Auth_Tx.TLF01_details.product_codes[16].amount,"052",3)== 0))
				{
					strncpy(temp_pos_entry_mode,"80",2);
				}
				else if (strncmp(Auth_Tx.TLF01_details.pos_entry_mode , "01" ,2) == 0 )
				{
					strncpy(temp_pos_entry_mode,"01",2);
				}
				else if (strncmp(Auth_Tx.TLF01_details.pos_entry_mode , "02" ,2) == 0 )
				{
					strncpy(temp_pos_entry_mode,"02",2);
				}
				else if (strncmp(Auth_Tx.TLF01_details.pos_entry_mode , "90" ,2) == 0 )
				{
					strncpy(temp_pos_entry_mode,"90",2);
				}
				else if (strncmp(Auth_Tx.TLF01_details.pos_entry_mode , "05" ,2) == 0 ||
						 strncmp(Auth_Tx.TLF01_details.pos_entry_mode , "95" ,2) == 0)
				{
					strncpy(temp_pos_entry_mode,"05",2);
				}
				else
				{
					strcpy(temp_pos_entry_mode,"");
				}
			}

}

void Falcon_JCB_posEntryMode_F41(pCHAR temp_pos_entry_mode)
{

			if((Is_BDO_Acquiring_Transaction()) == true)
			{
				if( strncmp(&Auth_Tx.TLF01_details.pos_entry_mode[1],"07",2) == 0 )
				{
					strncpy(temp_pos_entry_mode,"07",2);
				}
				else if ( strncmp(&Auth_Tx.TLF01_details.pos_entry_mode[1],"81",2) == 0)
				{
					strncpy(temp_pos_entry_mode,"81",2);
				}
				else if ( strncmp(&Auth_Tx.TLF01_details.pos_entry_mode[1],"80",2) == 0)
				{
					strncpy(temp_pos_entry_mode,"80",2);
				}
				else if ( strncmp(&Auth_Tx.TLF01_details.pos_entry_mode[1],"09",2) == 0)
				{
					strncpy(temp_pos_entry_mode,"79",2);
				}
				else if ( strncmp(&Auth_Tx.TLF01_details.pos_entry_mode[1],"01",2) == 0)
				{
					strncpy(temp_pos_entry_mode,"01",2);
				}
				else if ( strncmp(&Auth_Tx.TLF01_details.pos_entry_mode[1],"02",2) == 0)
				{
					strncpy(temp_pos_entry_mode,"90",2);
				}
				else if ( strncmp(&Auth_Tx.TLF01_details.pos_entry_mode[1],"05",2) == 0)
				{
					strncpy(temp_pos_entry_mode,"05",2);
				}
				else
				{
					strcpy(temp_pos_entry_mode,"");
				}
			}
			else
			{
				if( strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"07",2) == 0 ||
					strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"92",2) == 0 ||
					strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"06",2) == 0)
				{
					strncpy(temp_pos_entry_mode,"07",2);
				}
				else if ( strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"81",2) == 0)
				{
					strncpy(temp_pos_entry_mode,"81",2);
				}
				else if ( strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"97",2) == 0)
				{
					strncpy(temp_pos_entry_mode,"80",2);
				}
				else if ( strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"79",2) == 0)
				{
					strncpy(temp_pos_entry_mode,"79",2);
				}
				else if ( strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"01",2) == 0)
				{
					strncpy(temp_pos_entry_mode,"01",2);
				}
				else if ( strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"02",2) == 0)
				{
					strncpy(temp_pos_entry_mode,"02",2);
				}
				else if ( strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"90",2) == 0)
				{
					strncpy(temp_pos_entry_mode,"90",2);
				}
				else if ( strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"05",2) == 0)
				{
					strncpy(temp_pos_entry_mode,"05",2);
				}
				else
				{
					strcpy(temp_pos_entry_mode,"");
				}
			}

}

void Falcon_MC_posEntryMode_F41(pCHAR temp_pos_entry_mode)
{
		if((Is_BDO_Acquiring_Transaction())== true)
		{
			if((strcmp(temp_pos_entry_mode,"01") == 0) && (strcmp(Auth_Tx.TLF01_details.pos_condition_code,"59") == 0))
			{
				strncpy(temp_pos_entry_mode,"81",2);
			}
		}
}



void Falcon_VISA_posEntryMode_F41(pCHAR temp_pos_entry_mode)
{
				if((strcmp(temp_pos_entry_mode,"01") == 0) && (strcmp(Auth_Tx.TLF01_details.pos_condition_code,"59") == 0))
				{
					strncpy(temp_pos_entry_mode,"81",2);
				}
				else if(strcmp(Auth_Tx.TLF01_details.pos_entry_mode,"0022") == 0)
				{
						strncpy(temp_pos_entry_mode,"90",2); /* U*/
				}
				else if(strcmp(Auth_Tx.TLF01_details.pos_entry_mode,"0021") == 0)
				{
					if( Auth_Tx.TLF01_details.product_codes[16].amount[2] == '1' ||
					    Auth_Tx.TLF01_details.product_codes[16].amount[2] == '2')
					{
						strncpy(temp_pos_entry_mode,"80",2); /* F */
					}
					else if (Auth_Tx.TLF01_details.product_codes[16].amount[2] == '0')
					{
						strncpy(temp_pos_entry_mode,"90",2); /* U */
					}
				}
				/* Need to check the DE60.3 chip condition code: If 1 0r 2 : then it should be fall back :  F
				else if 60.3 = 0 then value should be U*/


}

INT FALCON_is_BDO_PAY_QR_Transaction(void)
{

	if(0 == strlen(Auth_Tx.TLF01_details.nii))
	{
		return false;
	}
	else if( (0 == strncmp(Auth_Tx.TLF01_details.nii+1,BDO_PAY_QR_NII_1,3) )||
		(0 == strncmp(Auth_Tx.TLF01_details.nii+1,BDO_PAY_QR_NII_2,3) )||
		(0 == strncmp(Auth_Tx.TLF01_details.nii+1,BDO_PAY_QR_NII_3,3) )||
		(0 == strncmp(Auth_Tx.TLF01_details.nii+1,BDO_PAY_QR_NII_4,3) )||
		(0 == strncmp(Auth_Tx.TLF01_details.nii+1,BDO_PAY_QR_NII_5,3) ) )
	{
		return true;
	}
	else
	{
		return false;

	}
}

INT Falcon_Is_Advice_or_Reversal_Transaction(void)
{
	if(	strncmp(Auth_Tx.TLF01_details.message_type,"0120",4) == 0 ||
		strncmp(Auth_Tx.TLF01_details.message_type,"0130",4) == 0 ||
		strncmp(Auth_Tx.TLF01_details.message_type,"1120",4) == 0 ||
		strncmp(Auth_Tx.TLF01_details.message_type,"1130",4) == 0 ||
		strncmp(Auth_Tx.TLF01_details.message_type,"0420",4) == 0 ||
		strncmp(Auth_Tx.TLF01_details.message_type,"0430",4) == 0 ||
		strncmp(Auth_Tx.TLF01_details.message_type,"1420",4) == 0 ||
		strncmp(Auth_Tx.TLF01_details.message_type,"1430",4) == 0 )
	{
		return true;
	}
	else if((Check_If_VISA_Transaction() == true) && (Falcon_is_Issuing_Transaction() ==true) &&
			(strncmp(Auth_Tx.TLF01_details.message_type,"0400",4) == 0 ))
	{
		return true;
	}
	else
	{
		return false;
	}
}


INT FALCON_is_Credential_On_File(void)
{

	char temp_pos_entry_mode[3]={0};

	if(strlen(Auth_Tx.TLF01_details.pos_entry_mode)== 3)
	{
		strncpy(temp_pos_entry_mode,Auth_Tx.TLF01_details.pos_entry_mode,2);
	}
	else
	{
		strncpy(temp_pos_entry_mode,&Auth_Tx.TLF01_details.pos_entry_mode[1],2);
	}

		if((Check_If_AMEX_Transaction())== true)
		{
			FALCON_is_Credential_On_File_AMEX(temp_pos_entry_mode);
		}
		else if((Check_If_DINERS_Transaction())== true)
		{
			FALCON_is_Credential_On_File_DINERS(temp_pos_entry_mode);
		}
		else if((Check_If_CUP_Transaction())== true)
		{
			FALCON_is_Credential_On_File_CUP(temp_pos_entry_mode);
		}
		else if((Check_If_JCB_Transaction())== true)
		{
			FALCON_is_Credential_On_File_JCB(temp_pos_entry_mode);
		}
		else if((Check_If_MC_Transaction())== true)
		{
			FALCON_is_Credential_On_File_MC(temp_pos_entry_mode);
		}
		else if((Check_If_VISA_Transaction())== true)
		{
			FALCON_is_Credential_On_File_VISA(temp_pos_entry_mode);
		}
}

INT isnum(pCHAR in_str)
{
   INT i;
   INT length =0;

   length = (INT)strlen(in_str);
	if(length == 0)
	{
		return(false);
	}
	for (i = 0; i < length; i++)
	{
	  if (!isdigit(in_str[i]))
	    return(false);
	}
   return(true);
}



void RqMssg(pPTE_MSG p_msg_in)
{

	CHAR tem_date[2]={0};
	CHAR tem_month[3]={0};
	CHAR tem_year[4]={0};
	CHAR temp_exp[9]={0};
	UINT  item_month=0;
	UINT  item_year=0;
	UINT  icvc_data=0;
	char dollar_peso[8]="0.023889";
	char temp_totalamt[14]={0};
	CHAR msgBuffer [400] = {0} ;
	pBYTE pATEMessageDataData = 0 ;
	pPTE_MSG_DATA pATEMessageData = 0 ;




	int i =0;
	int len=0;
	char msg[5]={0};
	char temp_iso_ration[25] = {0};
	char temp_iso_ration_value[13] = {0};
	double temp_iso = 0.0;

	CRF01  crf01 = {0} ;  LONG  lReturnCode;

	char temp_Tran_id[33]={0};


	memset (&Auth_Tx, 0, sizeof (AUTH_TX)) ;
	memset(&Cr_Fld,0,sizeof(Cr_Fld));
	memset(&message,0,sizeof(message));



	memset( &crf01, 0x00, sizeof(CRF01) );

	strcpy( crf01.primary_key.currency_code, "608");
	lReturnCode = FindMemRecord((pBYTE)&crf01, sizeof(crf01.primary_key.currency_code),sizeof(CRF01), "Crf01Table");
	if (lReturnCode >= 0)
	{
			conv_rate = strtod( (char *)crf01.conversion_rate, (char **)NULL );
			sprintf(temp_iso_ration,"%f",1/conv_rate);
	}

	pATEMessageData     = ptemsg_get_pte_msg_data (p_msg_in) ;
	pATEMessageDataData = ptemsg_get_pte_msg_data_data (pATEMessageData) ;
	memcpy (&Auth_Tx, (AUTH_TX*) pATEMessageDataData, sizeof Auth_Tx) ;

	ptetime_get_timestamp( time_date );
	get_date( time_date, date_str );
	get_time( time_date, time_str );

	FALCON_read_CUP_merchant_indicator();
	FALCON_read_CUP_MP_Bin_range_len_1();
	FALCON_read_CUP_MP_Bin_range_len_2();
	FALCON_read_CUP_MP_Bin_range_len_3();
	FALCON_read_CUP_MP_Bin_range_len_4();
	FALCON_read_CUP_MP_Bin_range_len_5();
	FALCON_read_CUP_MP_Bin_range_1_indicator();
	FALCON_read_CUP_MP_Bin_range_2_indicator();
	FALCON_read_CUP_MP_Bin_range_3_indicator();
	FALCON_read_CUP_MP_Bin_range_4_indicator();
	FALCON_read_CUP_MP_Bin_range_5_indicator();
	sprintf(msgBuffer,"Bin Values read for CUP MP from tf.ini: \nBin_range_1_indicator:%s , Bin_range_1_len:%d\n Bin_range_2_indicator:%s , Bin_range_2_len:%d\n Bin_range_3_indicator:%s , Bin_range_3_len:%d\n Bin_range_4_indicator:%s , Bin_range_4_len:%d\n Bin_range_5_indicator:%s , Bin_range_5_len:%d\n",
			CUP_MP_Bin_range_1,CUP_MP_Bin_range_len_1,CUP_MP_Bin_range_2,CUP_MP_Bin_range_len_2,CUP_MP_Bin_range_3,CUP_MP_Bin_range_len_3,
			CUP_MP_Bin_range_4,CUP_MP_Bin_range_len_4,CUP_MP_Bin_range_5,CUP_MP_Bin_range_len_5);
	falcon_log_message(2,1,msgBuffer,"RqMssg",0);


	FALCON_Read_BDO_PAY_QR_NII_value();

	FALCON_build_request_HeaderFile();
	FALCON_build_request_field_1( );
	FALCON_build_request_field_2( );
	FALCON_build_request_field_3( );
	FALCON_build_request_field_4( );
	FALCON_build_request_field_5( );
	FALCON_build_request_field_6( );
	FALCON_build_request_field_7( );
	FALCON_build_request_field_8( );
	FALCON_build_request_field_9( );
	FALCON_build_request_field_10( );
	FALCON_build_request_field_11( );
	FALCON_build_request_field_12( );
	FALCON_build_request_field_13( );
	FALCON_build_request_field_14( );
	FALCON_build_request_field_15( );
	FALCON_build_request_field_16( );
	FALCON_build_request_field_17( );
	FALCON_build_request_field_18( );
	FALCON_build_request_field_19( );
	FALCON_build_request_field_20( );
	FALCON_build_request_field_21( );
	FALCON_build_request_field_22( );
	FALCON_build_request_field_23( );
	FALCON_build_request_field_24( );
	FALCON_build_request_field_25( );
	FALCON_build_request_field_26( );
	FALCON_build_request_field_27( );
	FALCON_build_request_field_28( );
	FALCON_build_request_field_29( );
	FALCON_build_request_field_30( );
	FALCON_build_request_field_31( );
	FALCON_build_request_field_32( );
	FALCON_build_request_field_33( );
	FALCON_build_request_field_34( );
	FALCON_build_request_field_35( );
	FALCON_build_request_field_36( );
	FALCON_build_request_field_37( );
	FALCON_build_request_field_38( );
	FALCON_build_request_field_39( );
	FALCON_build_request_field_40( );
	FALCON_build_request_field_41( );
	FALCON_build_request_field_42( );
	FALCON_build_request_field_43( );
	FALCON_build_request_field_44( );
	FALCON_build_request_field_45( );
	FALCON_build_request_field_46( );
	FALCON_build_request_field_47( );
	FALCON_build_request_field_48( );
	FALCON_build_request_field_49( );
	FALCON_build_request_field_50( );
	FALCON_build_request_field_51( );
	FALCON_build_request_field_52( );
	FALCON_build_request_field_53( );
	FALCON_build_request_field_54( );
	FALCON_build_request_field_55( );
	FALCON_build_request_field_56( );
	FALCON_build_request_field_57( );
	FALCON_build_request_field_58( );
	FALCON_build_request_field_59( );
	FALCON_build_request_field_60( );
	FALCON_build_request_field_61( );
	FALCON_build_request_field_62( );
	FALCON_build_request_field_63( );
	FALCON_build_request_field_64( );
	FALCON_build_request_field_65( );
	FALCON_build_request_field_66( );
	FALCON_build_request_field_67( );
	FALCON_build_request_field_68( );
	FALCON_build_request_field_69( );
	FALCON_build_request_field_70( );
	FALCON_build_request_field_71( );
	FALCON_build_request_field_72( );
	FALCON_build_request_field_73( );
	FALCON_build_request_field_74( );
	FALCON_build_request_field_75( );
	FALCON_build_request_field_76( );
	FALCON_build_request_field_77( );
	FALCON_build_request_field_78( );
	FALCON_build_request_field_79( );
	FALCON_build_request_field_80( );
	FALCON_build_request_field_81( );
	FALCON_build_request_field_82( );
	FALCON_build_request_field_83( );
	FALCON_build_request_field_84( );
	FALCON_build_request_field_85( );
	FALCON_build_request_field_86( );
	FALCON_build_request_field_87( );
	FALCON_build_request_field_88( );
	FALCON_build_request_field_89( );
	FALCON_build_request_field_90( );
	FALCON_build_request_field_91( );
	FALCON_build_request_field_92( );
	FALCON_build_request_field_93( );
	FALCON_build_request_field_94( );
	FALCON_build_request_field_95( );
	FALCON_build_request_field_96( );
	FALCON_build_request_field_97( );
	FALCON_build_request_field_98( );
	FALCON_build_request_field_99( );
	FALCON_build_request_field_100( );
	FALCON_build_request_field_101( );
	FALCON_build_request_field_102( );
	FALCON_build_request_field_103( );
	FALCON_build_request_field_104( );
	FALCON_build_request_field_105( );
	FALCON_build_request_field_106( );
	FALCON_build_request_field_107( );
	FALCON_build_request_field_108( );
	FALCON_build_request_field_109( );
	FALCON_build_request_field_110( );
	FALCON_build_request_field_111( );
	FALCON_build_request_field_112( );
	FALCON_build_request_field_113( );
	FALCON_build_request_field_114( );
	FALCON_build_request_field_115( );
	FALCON_build_request_field_116( );
	FALCON_build_request_field_117( );
	FALCON_build_request_field_118( );
	FALCON_build_request_field_119( );
	FALCON_build_request_field_120( );
	FALCON_build_request_field_121( );
	FALCON_build_request_field_122( );
	FALCON_build_request_field_123( );
	FALCON_build_request_field_124( );
	FALCON_build_request_field_125( );
	FALCON_build_request_field_126( );
	FALCON_build_request_field_127( );
	FALCON_build_request_field_128( );
	FALCON_build_request_field_129( );
	FALCON_build_request_field_130( );
	FALCON_build_request_field_131( );
	FALCON_build_request_field_132( );
	FALCON_build_request_field_133( );
	FALCON_build_request_field_134( );
	FALCON_build_request_field_135( );
	FALCON_build_request_field_136( );
	FALCON_build_request_field_137( );
	FALCON_build_request_field_138( );
	FALCON_build_request_field_139( );
	FALCON_build_request_field_140( );
	FALCON_build_request_field_141( );
	FALCON_build_request_field_142( );
	FALCON_build_request_field_143( );
	FALCON_build_request_field_144( );
}

void FALCON_build_request_HeaderFile( )
{
	memcpy(Cr_Fld.HeaderFile,"000009710000100000102tdriver   PMAX      0000000000                  ",69);
	sprintf(message,"%-69s\n",Cr_Fld.HeaderFile);
}
/* field 1*/
void FALCON_build_request_field_1( )
{
	memcpy(Cr_Fld.WORKFLOW_XCD,profile_str,16);
	sprintf(&message[69],"%-16s\n",Cr_Fld.WORKFLOW_XCD);
}
void FALCON_build_request_field_2( )
{
	memcpy(Cr_Fld.RECORD_TYPE_XCD,"CRTRAN24",8);
	sprintf(&message[85],"%-8s\n",Cr_Fld.RECORD_TYPE_XCD);
}
void FALCON_build_request_field_3(  )
{
	memcpy(Cr_Fld.DATA_SPECIFICATION_VERSION_XCD,"2.4",5);
	sprintf(&message[93],"%-5s\n",Cr_Fld.DATA_SPECIFICATION_VERSION_XCD);
}
void FALCON_build_request_field_4( )
{
	memcpy(Cr_Fld.CLIENT_XID,"DT_BDO_CR",16);
	sprintf(&message[98],"%-16s\n",Cr_Fld.CLIENT_XID);
}
void FALCON_build_request_field_5( )
{
	memcpy(Cr_Fld.RCD_CRT_DATE,date_str,8);
	sprintf(&message[114],"%-8s\n",Cr_Fld.RCD_CRT_DATE);
}
void FALCON_build_request_field_6( )
{
	memcpy(Cr_Fld.RCD_CRT_TIME,time_str,6);
	sprintf(&message[122],"%-6s\n",Cr_Fld.RCD_CRT_TIME);
}
void FALCON_build_request_field_7( )
{
	memcpy(Cr_Fld.RCD_CRT_MSCND,time_milliseconds,3);
	sprintf(&message[128],"%-3s\n",Cr_Fld.RCD_CRT_MSCND);
}
void FALCON_build_request_field_8( )
{
	memcpy(Cr_Fld.GMT_OFFSET,"8.00",6);
	sprintf(&message[131],"%-6s\n",Cr_Fld.GMT_OFFSET);
}
void FALCON_build_request_field_9( )
{
	memcpy(Cr_Fld.CUSTOMER_XID,Auth_Tx.TLF01_details.card_num,20);
	sprintf(&message[137],"%-20s\n",Cr_Fld.CUSTOMER_XID);
}
void FALCON_build_request_field_10( )
{
	memcpy(Cr_Fld.ACCOUNT_REFERENCE_XID,Auth_Tx.TLF01_details.card_num,19);
	sprintf(&message[157],"%-40s\n",Cr_Fld.ACCOUNT_REFERENCE_XID);
}
void FALCON_build_request_field_11( )
{
	char temp_Tran_id[33]={0};
	static INT   falcon_tx_counter=0;
	memset(temp_Tran_id,' ',sizeof(temp_Tran_id)-1);
	sprintf(temp_Tran_id,"%-20s-CRT-%07d",Auth_Tx.TLF01_details.primary_key.transaction_id,falcon_tx_counter++);
	/*strncpy(temp_Tran_id,Auth_Tx.TLF01_details.primary_key.transaction_id,20);
	strcat(temp_Tran_id,"-CRT-");
	sprintf(&temp_Tran_id[25],"%07d",falcon_tx_counter++);*/
	strncpy(Cr_Fld.EXTERNAL_TRANSACTION_XID,temp_Tran_id,32);
	sprintf(&message[197],"%-32s\n",Cr_Fld.EXTERNAL_TRANSACTION_XID);
}
void FALCON_build_request_field_12( )
{
	memcpy(Cr_Fld.SCORE_CUSTOMER_ACCOUNT_XID,Auth_Tx.TLF01_details.card_num,19);
	sprintf(&message[229],"%-19s\n",Cr_Fld.SCORE_CUSTOMER_ACCOUNT_XID);
}
void FALCON_build_request_field_13( )
{
	memcpy(Cr_Fld.AUTHORIZATION_POSTING_XCD,"A",1);
	sprintf(&message[248],"%-1s\n",Cr_Fld.AUTHORIZATION_POSTING_XCD);
}
void FALCON_build_request_field_14( )
{
	memset(Cr_Fld.CARD_POSTAL_XCD,' ',9);
	sprintf(&message[249],"%-9s\n",Cr_Fld.CARD_POSTAL_XCD);
}
void FALCON_build_request_field_15( )
{
	memset(Cr_Fld.CARD_COUNTRY_XCD,' ',3);
	sprintf(&message[258],"%-3s\n",Cr_Fld.CARD_COUNTRY_XCD);
}
void FALCON_build_request_field_16( )
{
	memset(Cr_Fld.CARD_OPEN_DT,' ',8);
	sprintf(&message[261],"%-8s\n",Cr_Fld.CARD_OPEN_DT);
}
void FALCON_build_request_field_17( )
{
	memset(Cr_Fld.CARD_ISSUE_DT,' ',8);
	sprintf(&message[269],"%-8s\n",Cr_Fld.CARD_ISSUE_DT);
}
void FALCON_build_request_field_18( )
{
	memset(Cr_Fld.CARD_ISSUE_TYPE_XCD,' ',1);
	sprintf(&message[277],"%-1s\n",Cr_Fld.CARD_ISSUE_TYPE_XCD);
}
void FALCON_build_request_field_19( )
{
	memset(Cr_Fld.ACCOUNT_EXPIRATION_DT,' ',8);
	sprintf(&message[278],"%-8s\n",Cr_Fld.ACCOUNT_EXPIRATION_DT);
}

void FALCON_build_request_field_20( )
{

	CHAR tem_date[2]={0};
	CHAR tem_month[3]={0};
	CHAR tem_year[4]={0};
	CHAR temp_exp[8]={0};

	strncpy(tem_month,&Auth_Tx.TLF01_details.exp_date[2],2);
	strncpy(temp_exp,"20",2);
	if(strlen(Auth_Tx.TLF01_details.exp_date) > 0)
	{
		strncat(temp_exp,Auth_Tx.TLF01_details.exp_date,4);
	}
	else
	{
		strncpy(temp_exp,Auth_Tx.TLF01_details.date_yyyymmdd,6);
		strncpy(tem_month,&Auth_Tx.TLF01_details.date_yyyymmdd[4],2);
	}
	if( strncmp(tem_month,"",2) == 0 || strncmp(tem_month,"  ",2) == 0 || strncmp(tem_month,"00",2) == 0)
	{
		strncpy(temp_exp,"        ",8);
	}
	else if(strncmp(tem_month,"02",2) == 0)
	{
		strncat(temp_exp,"28",2);
	}
	else
	{
		strncat(temp_exp,"30",2);
	}

	strncpy(Cr_Fld.CARD_EXPIRATION_DT,temp_exp,8);
	sprintf(&message[286],"%-8s\n",Cr_Fld.CARD_EXPIRATION_DT);
}
void FALCON_build_request_field_21( )
{
	if(strcmp(Auth_Tx.ACF01_details.available,"") != 0)
	{
		char temp_avail_amt[11]="";
		strncpy(temp_avail_amt,&Auth_Tx.ACF01_details.available[2],10);
		sprintf(Cr_Fld.PRIOR_CREDIT_LIMIT_AMT,"%07d.%02d",atoi(temp_avail_amt)/100, atoi(temp_avail_amt)%100);
		sprintf(&message[294],"%-10s\n",Cr_Fld.PRIOR_CREDIT_LIMIT_AMT);
	}
	else
	{
		strcpy(Cr_Fld.PRIOR_CREDIT_LIMIT_AMT,"0000000.00");
		sprintf(&message[294],"%-10s\n",Cr_Fld.PRIOR_CREDIT_LIMIT_AMT);
	}
}
void FALCON_build_request_field_22( )
{
	memset(Cr_Fld.CARD_CREDIT_LIMIT_AMT,' ',10);
	sprintf(&message[304],"%-10s\n",Cr_Fld.CARD_CREDIT_LIMIT_AMT);
}
void FALCON_build_request_field_23( )
{
	memset(Cr_Fld.CUSTOMER_GENDER_XCD,' ',1);
	sprintf(&message[314],"%-1s\n",Cr_Fld.CUSTOMER_GENDER_XCD);
}
void FALCON_build_request_field_24( )
{
	memset(Cr_Fld.CUSTOMER_BIRTH_DT,' ',8);
	sprintf(&message[315],"%-8s\n",Cr_Fld.CUSTOMER_BIRTH_DT);
}
void FALCON_build_request_field_25( )
{
	memset(Cr_Fld.CUSTOMER_CARD_CNT,' ',3);
	sprintf(&message[323],"%-3s\n",Cr_Fld.CUSTOMER_CARD_CNT);
}
void FALCON_build_request_field_26( )
{
	memset(Cr_Fld.INCOME_AMT,' ',10);
	sprintf(&message[326],"%-10s\n",Cr_Fld.INCOME_AMT);
}
void FALCON_build_request_field_27( )
{
	memset(Cr_Fld.CARD_TYPE_XCD,' ',1);
	sprintf(&message[336],"%-1s\n",Cr_Fld.CARD_TYPE_XCD);
}
void FALCON_build_request_field_28( )
{
	memset(Cr_Fld.PRIMARY_CARD_USE_XCD,' ',1);
	sprintf(&message[337],"%-1s\n",Cr_Fld.PRIMARY_CARD_USE_XCD);
}
void FALCON_build_request_field_29( )
{
	memcpy(Cr_Fld.TRANSACTION_DATE,Auth_Tx.TLF01_details.date_yyyymmdd,8);
	sprintf(&message[338],"%-8s\n",Cr_Fld.TRANSACTION_DATE);
}
void FALCON_build_request_field_30( )
{
	memcpy(Cr_Fld.TRANSACTION_TIME,Auth_Tx.TLF01_details.time_hhmmss,6);
	sprintf(&message[346],"%-6s\n",Cr_Fld.TRANSACTION_TIME);
}

void FALCON_build_request_field_31( )
{
    if (Check_If_MC_Transaction ()    == true)
	{
    	Falcon_build_CRTRAN_field_31_MC();
	}
    else if (Check_If_VISA_Transaction ()    == true)
	{
    	Falcon_build_CRTRAN_field_31_VISA();
	}
    else if (Check_If_CUP_Transaction ()    == true)
	{
    	Falcon_build_CRTRAN_field_31_CUP();
	}
    else if (Check_If_JCB_Transaction ()    == true)
	{
    	Falcon_build_CRTRAN_field_31_JCB();
	}
    else if (Check_If_DINERS_Transaction ()    == true)
	{
    	Falcon_build_CRTRAN_field_31_DINERS();
	}
    else if (Check_If_AMEX_Transaction ()    == true)
	{

    	len_billing_amount = strlen(Auth_Tx.TLF01_details.settlement_total);

			if(len_billing_amount == 0)
			{
				billing_amount = atoi(Auth_Tx.reconciliation_amount);
				if(billing_amount > 0)
				{
					len_billing_amount = strlen(Auth_Tx.reconciliation_amount);
					strncpy(amount_digit,Auth_Tx.reconciliation_amount,10);
					strncpy(last2_decimal,Auth_Tx.reconciliation_amount+10,2);
					/*billing_amount = (strtod((char *)amount_digit,0));*/
					billing_amount = (strtod(Auth_Tx.reconciliation_amount, 0));
				}
				else
				{
					strncpy(amount_digit,Auth_Tx.TLF01_details.total_amount,10);
					strncpy(last2_decimal,Auth_Tx.TLF01_details.total_amount+10,2);
					/*billing_amount = (strtod(amount_digit,0));*/
					billing_amount = (strtod(Auth_Tx.TLF01_details.total_amount, 0));
				}
			}
			else
			{
				  strncpy(amount_digit,Auth_Tx.TLF01_details.settlement_total,10);
				  strncpy(last2_decimal,Auth_Tx.TLF01_details.settlement_total+10,2);
				  /*billing_amount = (strtod(amount_digit,0));*/
				  billing_amount = (strtod(Auth_Tx.TLF01_details.settlement_total, 0));
			}

    	Falcon_build_CRTRAN_field_31_AMEX();
	}
    else if (Check_If_Fleet_Transaction ()    == true)
	{
    	Falcon_build_CRTRAN_field_31_Fleet();
	}
    else
    {
    	strncpy(Cr_Fld.TRANSACTION_AMT,"0000000000.00",13);
    }

    sprintf(&message[352],"%-13s\n",Cr_Fld.TRANSACTION_AMT);

}

void FALCON_build_request_field_32( )
{
	strncpy(Cr_Fld.TRANSACTION_ISO_CURRENCY_XCD,"608",3);
	sprintf(&message[365],"%-3s\n",Cr_Fld.TRANSACTION_ISO_CURRENCY_XCD);
}

void FALCON_build_request_field_33( )
{
	char temp_iso_ration_value[13] = {0};

	strncpy(temp_iso_ration_value,"00000",5);
	strncat(temp_iso_ration_value,temp_iso_ration,8);
	strncpy(Cr_Fld.TRAN_ISO_CURRENCY_CNVRSN_RATIO,temp_iso_ration_value,13);
	sprintf(&message[368],"%-13s\n",Cr_Fld.TRAN_ISO_CURRENCY_CNVRSN_RATIO);
}


	/*
	Pls. map:
	A = 00, 10, 11, 32, 85
	D = 03, 05, 06, 12, 13, 14, 19, 20, 30, 33, 39, 42, 44, 51, 52, 53, 54, 55, 56, 57, 58, 61, 63, 64, 82, 92, N7
	P = 04, 07, 41, 43
	R = 01, 02, 08,34, 35, 36, 37, 38, 60, 62, 65, 66, 75,
	Else “BLANK”
	*/
void FALCON_build_request_field_34( )
{
	if( strncmp(Auth_Tx.TLF01_details.response_code,"00",2)== 0 ||
		strncmp(Auth_Tx.TLF01_details.response_code,"10",2)== 0 ||
		strncmp(Auth_Tx.TLF01_details.response_code,"11",2)== 0 ||
		strncmp(Auth_Tx.TLF01_details.response_code,"32",2)== 0 ||
		strncmp(Auth_Tx.TLF01_details.response_code,"85",2)== 0 )
	{
		strncpy(Cr_Fld.DECISION_XCD,"A",1);
	}
	else if(strncmp(Auth_Tx.TLF01_details.response_code,"03",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"05",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"06",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"12",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"13",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"14",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"19",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"20",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"30",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"33",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"39",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"42",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"51",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"52",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"53",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"54",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"55",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"56",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"57",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"58",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"61",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"63",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"64",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"82",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"92",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"N7",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"44",2)== 0)
	{
			strncpy(Cr_Fld.DECISION_XCD,"D",1);
	}
	else if(strncmp(Auth_Tx.TLF01_details.response_code,"04",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"07",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"41",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"43",2)== 0	)
	{
		strncpy(Cr_Fld.DECISION_XCD,"P",1);
	}
	else if(strncmp(Auth_Tx.TLF01_details.response_code,"01",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"02",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"08",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"34",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"35",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"36",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"37",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"38",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"60",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"62",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"65",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"66",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"75",2)== 0	)
	{
		strncpy(Cr_Fld.DECISION_XCD,"R",1);
	}
	else
	{
		strncpy(Cr_Fld.DECISION_XCD," ",1);
	}

	sprintf(&message[381],"%-1s\n",Cr_Fld.DECISION_XCD);
}
void FALCON_build_request_field_35()
{
	if( Auth_Tx.TLF01_details.tx_key == AUTH_SALE          || Auth_Tx.TLF01_details.tx_key == AUTH_SALE_RESPONSE ||
		Auth_Tx.TLF01_details.tx_key == AUTH_AUTHORIZATION || Auth_Tx.TLF01_details.tx_key == AUTH_AUTHORIZATION_RESPONSE||
		Auth_Tx.TLF01_details.tx_key == AUTH_ADVICE        || Auth_Tx.TLF01_details.tx_key == AUTH_ADVICE_RESPONSE||
		Auth_Tx.TLF01_details.tx_key == AUTH_ADVICE_REPEAT || Auth_Tx.TLF01_details.tx_key == AUTH_ADVICE_REPEAT_RESPONSE)
	{
		strncpy(Cr_Fld.TRANSACTION_TYPE_XCD,"M",1);
	}
	else if(Auth_Tx.TLF01_details.tx_key == AUTH_CASH_ADVANCE || Auth_Tx.TLF01_details.tx_key == AUTH_CASH_ADVANCE_RESPONSE ||
		    Auth_Tx.TLF01_details.tx_key == AUTH_CASH_ADVANCE_AUTHORIZATION || Auth_Tx.TLF01_details.tx_key == AUTH_CASH_ADVANCE_AUTHORIZATION_RESPONSE)
	{/* CASH advance*/
		strncpy(Cr_Fld.TRANSACTION_TYPE_XCD,"C",1);
	}
	else if(Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL ||Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_RESPONSE||
			Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_ADVICE || Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_ADVICE_RESPONSE)
	{/*Reversal*/
		strncpy(Cr_Fld.TRANSACTION_TYPE_XCD,"V",1);
	}
	else if(Auth_Tx.TLF01_details.tx_key == AUTH_VOID_SALE || Auth_Tx.TLF01_details.tx_key == AUTH_VOID_SALE_RESPONSE)
	{/*VOID */
		strncpy(Cr_Fld.TRANSACTION_TYPE_XCD,"X",1);
	}
	else if(Auth_Tx.TLF01_details.tx_key == AUTH_PRE_AUTHORIZATION || Auth_Tx.TLF01_details.tx_key == AUTH_PRE_AUTHORIZATION_RESPONSE)
	{/*Pre AUTH*/
		strncpy(Cr_Fld.TRANSACTION_TYPE_XCD,"P",1);
	}
	else if(Auth_Tx.TLF01_details.tx_key == AUTH_OFFLINE_SALE || Auth_Tx.TLF01_details.tx_key == AUTH_OFFLINE_SALE_RESPONSE)
	{/*Offline sale*/
		strncpy(Cr_Fld.TRANSACTION_TYPE_XCD,"D",1);
	}
	else
	{
		strncpy(Cr_Fld.TRANSACTION_TYPE_XCD," ",1);
	}

	sprintf(&message[382],"%-1s\n",Cr_Fld.TRANSACTION_TYPE_XCD);
}
void FALCON_build_request_field_36()
{
	memcpy(Cr_Fld.MERCHANT_CATEGORY_XCD,Auth_Tx.TLF01_details.category_code,4);
	sprintf(&message[383],"%-4s\n",Cr_Fld.MERCHANT_CATEGORY_XCD);
}/*******memcpy(Cr_Fld.MERCHANT_POSTAL_XCD,Auth_Tx.MCF01_details.zip_code,9);*****/
void FALCON_build_request_field_37()
{
	if(strncmp(Auth_Tx.MCF01_details.zip_code,"9999",4)== 0)
	{
		memset(Cr_Fld.MERCHANT_POSTAL_XCD,' ',9);
	}
	else
	{
		strncpy(Cr_Fld.MERCHANT_POSTAL_XCD,Auth_Tx.MCF01_details.zip_code,9);
	}
	sprintf(&message[387],"%-9s\n",Cr_Fld.MERCHANT_POSTAL_XCD);
}
void FALCON_build_request_field_38()
{
	if(strlen(Auth_Tx.MCF01_details.primary_key.merchant_id)> 0)
	{
		strncpy(Cr_Fld.MERCHANT_COUNTRY_XCD,"608",3);
	}
	else if(strlen(Auth_Tx.reconciliation_cur_code)> 0)/*reconciliation_cur_code*/
	{
		strncpy(Cr_Fld.MERCHANT_COUNTRY_XCD,Auth_Tx.reconciliation_cur_code,3);
	}
	else if(strlen(Auth_Tx.TLF01_details.num_dbsales)> 0)/*nfi_seq_nbr*/
	{
		strncpy(Cr_Fld.MERCHANT_COUNTRY_XCD,Auth_Tx.TLF01_details.num_dbsales,3);
	}
	else
		strncpy(Cr_Fld.MERCHANT_COUNTRY_XCD,Auth_Tx.TLF01_details.currency_code,3);

	sprintf(&message[396],"%-3s\n",Cr_Fld.MERCHANT_COUNTRY_XCD);
}
void FALCON_build_request_field_39()
{
	if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_MASTERCARD)== 0)
	{
		if(strcmp(Auth_Tx.TLF01_details.response_code, "00") == 0)
		{
			memcpy(Cr_Fld.TRANSACTION_PIN_VERIFY_XCD,"V",1);
		}
		else
		{
			memcpy(Cr_Fld.TRANSACTION_PIN_VERIFY_XCD,"I",1);
		}

	}
	else if(strncmp(Cr_Fld.TRANSACTION_TYPE_XCD,"C",1)== 0)
	{
		if(strncmp(Auth_Tx.TLF01_details.response_code, "55" ,2) == 0)
		{
			memcpy(Cr_Fld.TRANSACTION_PIN_VERIFY_XCD,"I",1);
		}
		else
		{
				memcpy(Cr_Fld.TRANSACTION_PIN_VERIFY_XCD,"V",1);
		}
	}
	else
	{
			memcpy(Cr_Fld.TRANSACTION_PIN_VERIFY_XCD," ",1);
	}

	sprintf(&message[399],"%-1s\n",Cr_Fld.TRANSACTION_PIN_VERIFY_XCD);
}
/*switch(Auth_Tx.TLF01_details.cvc)
{
case 'Y': memcpy(Cr_Fld.CVV_VERIFY_XCD,"I",1);
		  sprintf(&message[400],"%-1s\n",Cr_Fld.CVV_VERIFY_XCD);
		  break;
case 'N': memcpy(Cr_Fld.CVV_VERIFY_XCD,"I",1);
		  sprintf(&message[400],"%-1s\n",Cr_Fld.CVV_VERIFY_XCD);
		  break;
case 'M': memcpy(Cr_Fld.CVV_VERIFY_XCD,"V",1);
		  sprintf(&message[400],"%-1s\n",Cr_Fld.CVV_VERIFY_XCD);
		  break;
default:memset(Cr_Fld.CVV_VERIFY_XCD,' ',1);
		sprintf(&message[400],"%-1s\n",Cr_Fld.CVV_VERIFY_XCD);
		break;
}*/
void FALCON_build_request_field_40()
{
	if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_MASTERCARD)== 0)
	{
		/* mapping variable need to check*/
		if(Auth_Tx.TLF01_details.cvc == 'Y' || Auth_Tx.TLF01_details.cvc == 'N' || Auth_Tx.TLF01_details.cvc == 'E' )
		{
			memcpy(Cr_Fld.CVV_VERIFY_XCD,"I",1);
			memcpy(Cr_Fld.CVV2_RESPONSE_XFLG,"N",1);
		}
		else if(Auth_Tx.TLF01_details.cvc == 'M')
		{
			memcpy(Cr_Fld.CVV_VERIFY_XCD,"V",1);
			memcpy(Cr_Fld.CVV2_RESPONSE_XFLG,"M",1);
		}
		else
		{
			memset(Cr_Fld.CVV_VERIFY_XCD,' ',1);
			memset(Cr_Fld.CVV2_RESPONSE_XFLG,' ',1);
		}

	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_VISA)== 0)
	{
		/* mapping variable need to check*/

		if(Auth_Tx.TLF01_details.cvc == '1')
		{
			memcpy(Cr_Fld.CVV_VERIFY_XCD,"I",1);
			memcpy(Cr_Fld.CVV2_RESPONSE_XFLG,"N",1);
		}
		else if(Auth_Tx.TLF01_details.cvc == '2' || Auth_Tx.TLF01_details.cvc == '3' )
		{
			memcpy(Cr_Fld.CVV_VERIFY_XCD,"V",1);
			memcpy(Cr_Fld.CVV2_RESPONSE_XFLG,"M",1);
		}
		else
		{
			memset(Cr_Fld.CVV_VERIFY_XCD,' ',1);
			memset(Cr_Fld.CVV2_RESPONSE_XFLG,' ',1);
		}
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_CUP)== 0)
	{
		if (Auth_Tx.future_use_2[22] == '1' || Auth_Tx.future_use_2[23] == '1')
		{
			memcpy(Cr_Fld.CVV_VERIFY_XCD,"V",1);
		}
		else if(Auth_Tx.future_use_2[22] == '2' || Auth_Tx.future_use_2[23] == '2')
		{
			memcpy(Cr_Fld.CVV_VERIFY_XCD,"I",1);
		}
		else if (Auth_Tx.future_use_2[22] == '3' || Auth_Tx.future_use_2[23] == '3'||
				 Auth_Tx.future_use_2[22] == ' ' || Auth_Tx.future_use_2[23] == ' ')
		{
			memcpy(Cr_Fld.CVV_VERIFY_XCD," ",1);
		}

	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_DINERS)== 0)
	{
		if( strncmp(&Auth_Tx.acct_id_1[2],"001",3) == 0 ||
		    strncmp(&Auth_Tx.acct_id_1[2],"016",3) == 0	)
		{
			memcpy(Cr_Fld.CVV_VERIFY_XCD,"V",1);
		}
		else if( strncmp(&Auth_Tx.acct_id_1[2],"002",3) == 0 ||
			     strncmp(&Auth_Tx.acct_id_1[2],"017",3) == 0 )
		{
			memcpy(Cr_Fld.CVV_VERIFY_XCD,"I",1);
		}
		else
		{
			memcpy(Cr_Fld.CVV_VERIFY_XCD," ",1);
		}
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_JCB)== 0 ||
			strcmp(Auth_Tx.BIN01_details.card_family, CARD_AMEX)== 0)
	{
		memset(Cr_Fld.CVV_VERIFY_XCD,' ',1);
	}
	else
	{
		memset(Cr_Fld.CVV_VERIFY_XCD,' ',1);
	}

	 sprintf(&message[400],"%-1s\n",Cr_Fld.CVV_VERIFY_XCD);
}

/*	Please refer to POS netry mode mapping doc : # */
void FALCON_build_request_field_41()
{
	char temp_pos_entry_mode[3]={0};
	if(strlen(Auth_Tx.TLF01_details.pos_entry_mode)== 3)
	{
		strncpy(temp_pos_entry_mode,Auth_Tx.TLF01_details.pos_entry_mode,2);
	}
	else
	{
		strncpy(temp_pos_entry_mode,&Auth_Tx.TLF01_details.pos_entry_mode[1],2);
	}

	if((Check_If_AMEX_Transaction())== true)
	{
		Falcon_AMEX_posEntryMode_F41(temp_pos_entry_mode);
	}
	else if((Check_If_DINERS_Transaction())== true)
	{
		Falcon_DINERS_posEntryMode_F41(temp_pos_entry_mode);
	}
	else if((Check_If_CUP_Transaction())== true)
	{
		Falcon_CUP_posEntryMode_F41(temp_pos_entry_mode);
	}
	else if((Check_If_JCB_Transaction()) == true)
	{
		Falcon_JCB_posEntryMode_F41(temp_pos_entry_mode);
	}
	else if((Check_If_MC_Transaction())== true)
	{
		Falcon_MC_posEntryMode_F41(temp_pos_entry_mode);
	}
	else if((Check_If_VISA_Transaction())== true)
	{
		Falcon_VISA_posEntryMode_F41(temp_pos_entry_mode);
	}

	if(((strncmp(temp_pos_entry_mode,"02",2) == 0  ||
			  strncmp(temp_pos_entry_mode,"90",2) == 0) &&
			  strcmp(Auth_Tx.BIN01_details.card_family, CARD_JCB)!= 0 ))
	{ /* POS 02 or 90 + Field 60.2.2 = 5 or 6 + Field 60.2.3 = 2 */
		if( strncmp(Auth_Tx.TLF01_details.product_codes[16].amount,"052",3)== 0 ||
			(strncmp(Auth_Tx.TLF01_details.product_codes[16].amount,"062",3)== 0))
		{
			strncpy(temp_pos_entry_mode,"80",2);
		}
		else if(strncmp(Auth_Tx.TLF01_details.product_codes[16].amount[10],"4",1)== 0)
		{
			strncpy(temp_pos_entry_mode,"91",2);
		}
		else
		{
			strncpy(temp_pos_entry_mode,"90",2);
		}
	}
	else if( strcmp(Auth_Tx.TLF01_details.pos_entry_mode,"0021") == 0)
	{
		strncpy(temp_pos_entry_mode,"90",2);
	}
	else if(strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"092",3) == 0  ||
			strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"0092",4) == 0 )
	{
			strncpy(temp_pos_entry_mode,"79",2);
	}
	else if((strcmp(Auth_Tx.TLF01_details.pos_entry_mode,"0021") == 0)&&
			(strcmp(Auth_Tx.BIN01_details.card_family, CARD_JCB)!= 0) )
	{
		strncpy(temp_pos_entry_mode,"91",2);
	}
	else if(strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"080",3) == 0 )
	{
		strncpy(temp_pos_entry_mode,"80",2);
	}
	else if(strcmp(Auth_Tx.TLF01_details.pos_entry_mode,"0021") == 0 )
	{
		strncpy(temp_pos_entry_mode,"80",2);
	}

	if(FALCON_is_BDO_PAY_QR_Transaction() == true)
	{
		if((Is_BDO_Pos_Acquiring_Transaction()) == true)
		{
			strncpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"X",1);
		}
	}

	else if(strncmp(temp_pos_entry_mode,"91",2) == 0 || strncmp(temp_pos_entry_mode,"2W",2) == 0)
	{
		strncpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"C",1);
	}
	else if(strncmp(temp_pos_entry_mode,"92",2) == 0   ||
			strncmp(temp_pos_entry_mode,"07",2) == 0   ||
			strncmp(temp_pos_entry_mode,"06",2) == 0   ||
			strncmp(temp_pos_entry_mode,"X5",2) == 0	)
	{
		strncpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"D",1);
	}
	else if((FALCON_is_Credential_On_File() ==true) ||(strncmp(temp_pos_entry_mode,"81",2) == 0))
	{
		strncpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"E",1);
	}
	else if(strncmp(temp_pos_entry_mode,"80",2) == 0)
	{
		strncpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"F",1);
	}
	else if(strncmp(temp_pos_entry_mode,"79",2) == 0)
	{
		strncpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"G",1);
	}
	else if(strncmp(temp_pos_entry_mode,"01",2) == 0 )
	{
		strncpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"K",1);
	}
	else if(strncmp(temp_pos_entry_mode,"02",2) == 0)
	{
		strncpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"S",1);
	}
	else if(strncmp(temp_pos_entry_mode,"90",2) == 0 ||
			strncmp(temp_pos_entry_mode,"97",2) == 0 )
	{
		strncpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"U",1);
	}
	else if(strncmp(temp_pos_entry_mode,"05",2) == 0 || strncmp(temp_pos_entry_mode,"52",2) == 0 ||
			strncmp(temp_pos_entry_mode,"51",2) == 0)
	{
		strncpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"V",1);
	}
	else if(strncmp(temp_pos_entry_mode,"09",2) == 0)
	{
		strncpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"F",1);
	}

	else
	{
		strncpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG," ",1);
	}

	sprintf(&message[401],"%-1s\n",Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG);
}
void FALCON_build_request_field_42( )
{
	if(Cr_Fld.AUTHORIZATION_POSTING_XCD == "P")
	{
		memcpy(Cr_Fld.TRANSACTION_POSTING_DT,Auth_Tx.TLF01_details.date_yyyymmdd,8);
		sprintf(&message[402],"%-8s\n",Cr_Fld.TRANSACTION_POSTING_DT);
	}
	else
	{
		memset(Cr_Fld.TRANSACTION_POSTING_DT,' ',8);
		sprintf(&message[402],"%-8s\n",Cr_Fld.TRANSACTION_POSTING_DT);
	}
}
void FALCON_build_request_field_43( )
{
	memset(Cr_Fld.AUTHORIZATION_POSTING_MIS_XCD,' ',1);
	sprintf(&message[410],"%-1s\n",Cr_Fld.AUTHORIZATION_POSTING_MIS_XCD);
}
void FALCON_build_request_field_44( )
{
	memset(Cr_Fld.TRANS_POSTING_MISMATCH_XFLG,' ',1);
	sprintf(&message[411],"%-1s\n",Cr_Fld.TRANS_POSTING_MISMATCH_XFLG);
}
void FALCON_build_request_field_45( )
{
	memset(Cr_Fld.CREATE_CASE_XFLG,' ',1);
	sprintf(&message[412],"%-1s\n",Cr_Fld.CREATE_CASE_XFLG);
}
void FALCON_build_request_field_46( )
{
	memset(Cr_Fld.USER_INDICATOR_1_XCD,' ',1);
	sprintf(&message[413],"%-1s\n",Cr_Fld.USER_INDICATOR_1_XCD);
}
void FALCON_build_request_field_47( )
{
	memset(Cr_Fld.USER_INDICATOR_2_XCD,' ',1);
	sprintf(&message[414],"%-1s\n",Cr_Fld.USER_INDICATOR_2_XCD);
}
void FALCON_build_request_field_48( )
{
	memset(Cr_Fld.USER_DATA_1_STRG,' ',10); /* UCAF indicator DE48 SE43*/

	/*Field 48 for Terminal type*/
	if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_VISA)	== 0)
	{
		memcpy(Cr_Fld.USER_DATA_1_STRG,&Auth_Tx.additional_pos_information[0],1);
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_MASTERCARD)	== 0)
	{
		memcpy(Cr_Fld.USER_DATA_1_STRG,&Auth_Tx.acceptor_term_name[9],1);
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_AMEX)	== 0)
	{
		strncpy(Cr_Fld.USER_DATA_1_STRG,&Auth_Tx.amex_pos_entry_mode[3],1);
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_CUP)	== 0)
	{
		if(strncmp(Auth_Tx.TLF01_details.handler_queue,"VOICE",5)== 0)
		{
			if(0 ==strncmp(Auth_Tx.TLF01_details.pos_condition_code,"08",2))
			{
				memcpy(Cr_Fld.USER_DATA_1_STRG,"08", 2);/*Mobile phone Voice transaction*/
			}
			else
			{
				memcpy(Cr_Fld.USER_DATA_1_STRG,"07", 2);/*PC Voice transaction*/
			}
		}
		else
		{
			memcpy(Cr_Fld.USER_DATA_1_STRG,"03", 2);/*POS-Point Of Sale 03*/
		}
	}

	sprintf(&message[415],"%-10s\n",Cr_Fld.USER_DATA_1_STRG);
}
void FALCON_build_request_field_49( )
{
	memset(Cr_Fld.USER_DATA_2_STRG,' ',10);
	/*Field 49 for Terminal Entry capa*/
	if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_VISA)	== 0)
	{
		memcpy(Cr_Fld.USER_DATA_2_STRG,&Auth_Tx.additional_pos_information[1],1);
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_MASTERCARD)	== 0)
	{
		memcpy(Cr_Fld.USER_DATA_2_STRG,&Auth_Tx.acceptor_term_name[10],1);
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_AMEX)	== 0)
	{
		/*if(Auth_Tx.TLF01_details.product_codes[16].amount[0]=='5')
			strncpy(Cr_Fld.USER_DATA_2_STRG,"C",1);
		else*/
			memcpy(Cr_Fld.USER_DATA_2_STRG,&Auth_Tx.TLF01_details.product_codes[16].amount[0],1);
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_CUP)	== 0)
	{
		if(strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"K",1) == 0  ||
			strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"S",1) == 0 ||
			strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"T",1) == 0 ||
			strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"U",1) == 0)
		{
			strncpy(Cr_Fld.USER_DATA_2_STRG,"2",1);
		}
		else if(strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"V",1) == 0)
		{
			strncpy(Cr_Fld.USER_DATA_2_STRG,"5",1);
		}
		else
		{
			strncpy(Cr_Fld.USER_DATA_2_STRG,"0",1);
		}
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_DINERS)	== 0)
	{
		Cr_Fld.USER_DATA_2_STRG[0] = Auth_Tx.amex_pos_entry_mode[0] ;
		Cr_Fld.USER_DATA_2_STRG[1] = '\0';
	}
	/*if(strcmp(Auth_Tx.BON01_details.minimum_amount,"")!= 0)
	{
		strncpy(Cr_Fld.USER_DATA_2_STRG,Auth_Tx.BON01_details.minimum_amount,10);
	}*/
	sprintf(&message[425],"%-10s\n",Cr_Fld.USER_DATA_2_STRG);
}
void FALCON_build_request_field_50( )
{
	memset(Cr_Fld.MERCHANT_XIDD,' ',10);
	sprintf(&message[435],"%-10s\n",Cr_Fld.MERCHANT_XIDD);
}
void FALCON_build_request_field_51( )
{
	memset(Cr_Fld.MERCHANT_DATA_XFLG,' ',1);
	sprintf(&message[445],"%-1s\n",Cr_Fld.MERCHANT_DATA_XFLG);
}
void FALCON_build_request_field_52( )
{
	memset(Cr_Fld.CARD_DATA_AVAILABLE_XFLG,' ',1);
	sprintf(&message[446],"%-1s\n",Cr_Fld.CARD_DATA_AVAILABLE_XFLG);
}
void FALCON_build_request_field_53( )
{
	memcpy(Cr_Fld.EXTERNAL_1_SCR,"0000",4);
	sprintf(&message[447],"%-4s\n",Cr_Fld.EXTERNAL_1_SCR);
}
void FALCON_build_request_field_54( )
{
	memcpy(Cr_Fld.EXTERNAL_2_SCR,"0000",4);
	sprintf(&message[451],"%-4s\n",Cr_Fld.EXTERNAL_2_SCR);
}
void FALCON_build_request_field_55( )
{
	memcpy(Cr_Fld.EXTERNAL_3_SCR,"0000",4);
	sprintf(&message[455],"%-4s\n",Cr_Fld.EXTERNAL_3_SCR);

}
void FALCON_build_request_field_56( )
{
	if(strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"C",1) == 0 ||
	   strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"D",1) == 0 ||
	   strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"F",1) == 0 ||
	   strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"G",1) == 0 ||
	   strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"S",1) == 0 ||
	   strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"T",1) == 0 ||
	   strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"U",1) == 0 ||
	   strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"V",1) == 0)
	{
		memcpy(Cr_Fld.CARDHOLDER_PRESENT_XFLG,"Y",1);
		memcpy(Cr_Fld.TRANSACTION_CATEGORY_XCD,"P",1);
	}
	else if(strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"E",1) == 0 ||
	   strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"K",1) == 0 )
	{
		memcpy(Cr_Fld.CARDHOLDER_PRESENT_XFLG,"N",1);
		if(strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"E",1) == 0)
		{
			memcpy(Cr_Fld.TRANSACTION_CATEGORY_XCD,"I",1);
		}
		else
		{
			memcpy(Cr_Fld.TRANSACTION_CATEGORY_XCD,"M",1);
		}
	}
	else
	{
		memcpy(Cr_Fld.CARDHOLDER_PRESENT_XFLG," ",1);
		memcpy(Cr_Fld.TRANSACTION_CATEGORY_XCD,"O",1);
	}

	sprintf(&message[459],"%-1s\n",Cr_Fld.CARDHOLDER_PRESENT_XFLG);
}

void FALCON_build_request_field_57( )
{
	if(strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"E",1) == 0)
	{
		memcpy(Cr_Fld.CAT_TYPE_XFLG,"E",1);/* POS Condition code*/
	}
	else
	{
		memcpy(Cr_Fld.CAT_TYPE_XFLG,"S",1);/* POS Condition code*/
	}
	sprintf(&message[460],"%-1s\n",Cr_Fld.CAT_TYPE_XFLG);
}
void FALCON_build_request_field_58( )
{
	memset(Cr_Fld.TESTING_RANDOM_DIGITS_STRG,' ',2);
	sprintf(&message[461],"%-2s\n",Cr_Fld.TESTING_RANDOM_DIGITS_STRG);
}
void FALCON_build_request_field_59( )
{
	memset(Cr_Fld.PORTFOLIO_NAME,' ',14);
	sprintf(&message[463],"%-14s\n",Cr_Fld.PORTFOLIO_NAME);
}
void FALCON_build_request_field_60( )
{
	memset(Cr_Fld.CLIENT_2_XID,' ',14);
	sprintf(&message[477],"%-14s\n",Cr_Fld.CLIENT_2_XID);
}
void FALCON_build_request_field_61( )
{
	memset(Cr_Fld.INTERBANK_CARD_ASSOCIATION_NUM,' ',6);
	sprintf(&message[491],"%-6s\n",Cr_Fld.INTERBANK_CARD_ASSOCIATION_NUM);
}
void FALCON_build_request_field_62( )
{
	if(strlen(Auth_Tx.MCF01_details.name02)> 0)
	{
		memcpy(Cr_Fld.MERCHANT_NAME,Auth_Tx.MCF01_details.name02,30);
	}
	else if(strlen(Auth_Tx.TLF01_details.card_holder_name) > 0)
	{
		memcpy(Cr_Fld.MERCHANT_NAME,Auth_Tx.TLF01_details.card_holder_name,30);
	}

	sprintf(&message[497],"%-40s\n",Cr_Fld.MERCHANT_NAME);
}
void FALCON_build_request_field_63( )
{
	if(strlen(Auth_Tx.MCF01_details.city)== 0)
	{
		memcpy(Cr_Fld.MERCHANT_CITY_NAME,Auth_Tx.TLF01_details.ncf01_retired_cwk,13);
	}
	else
	{
		memcpy(Cr_Fld.MERCHANT_CITY_NAME,Auth_Tx.MCF01_details.city,20);
	}
	sprintf(&message[537],"%-30s\n",Cr_Fld.MERCHANT_CITY_NAME);
}
void FALCON_build_request_field_64( )
{
	if(strlen(Auth_Tx.MCF01_details.intl_code)== 0)
	{
		memcpy(Cr_Fld.MERCHANT_STATE_XCD,Auth_Tx.TLF01_details.ncf01_retired_cwk+13,3);
	}
	else
	{
		memcpy(Cr_Fld.MERCHANT_STATE_XCD,Auth_Tx.MCF01_details.intl_code,3);
	}
	sprintf(&message[567],"%-3s\n",Cr_Fld.MERCHANT_STATE_XCD);
}
void FALCON_build_request_field_65( )
{
	memset(Cr_Fld.SUPPRES_CASE_XFLG,' ',1);
	sprintf(&message[570],"%-1s\n",Cr_Fld.SUPPRES_CASE_XFLG);
}
void FALCON_build_request_field_66( )
{
	/***memcpy(Cr_Fld.USER_INDICATOR_3_XCD,' ' ,5);****/
	memcpy(Cr_Fld.USER_INDICATOR_3_XCD,"",5);
	sprintf(&message[571],"%-5s\n",Cr_Fld.USER_INDICATOR_3_XCD);
}
	/*********************UserIndicator04****************************/
void FALCON_build_request_field_67( )
{
	int len_curr_code, len_TLF_curr_code = 0, len_recon_curr_code=0 ;

	len_TLF_curr_code = strlen(Auth_Tx.TLF01_details.currency_code);
	len_curr_code = strlen(Auth_Tx.MCF01_details.currency_code);
	len_recon_curr_code = strlen(Auth_Tx.reconciliation_cur_code);

	if((0 != strncmp(Auth_Tx.TLF01_details.handler_queue, "dcp", 3))&&
	    	   (0 != strncmp(Auth_Tx.TLF01_details.handler_queue, "VOICE",5)))
	{
		if(len_recon_curr_code > 0)
		{
			strncpy(Cr_Fld.USER_INDICATOR_4_XCD,Auth_Tx.reconciliation_cur_code,3);
		}
		else if (len_TLF_curr_code > 0)
		{
			strncpy(Cr_Fld.USER_INDICATOR_4_XCD,Auth_Tx.TLF01_details.currency_code,3);
		}

	}
	else if(len_curr_code>0)
	{
		strncpy(Cr_Fld.USER_INDICATOR_4_XCD,Auth_Tx.MCF01_details.currency_code,3);
	}
	else if (len_TLF_curr_code > 0)
	{
		strncpy(Cr_Fld.USER_INDICATOR_4_XCD,Auth_Tx.TLF01_details.currency_code,3);
	}
	else
	{
		memset(Cr_Fld.USER_INDICATOR_4_XCD,SPACES,FIELD67_SIZE);
	}
	sprintf(&message[576],"%-5s\n",Cr_Fld.USER_INDICATOR_4_XCD);
	/***************************************************************/
}

	/* Dual currrecny logic*/

	/**len**strcpy(Cr_Fld.USER_DATA_3_STRG,"000");**/
void FALCON_build_request_field_68( )
{
	char temp_totalamt[14]={0};
	strncpy(Cr_Fld.USER_DATA_3_STRG,"00",2);
	if((Check_If_DINERS_Transaction ()  == true)||
		(Check_If_CUP_Transaction ()  == true))
	{
		int original_tx_amt = 0;
		if(strlen(Auth_Tx.TLF01_details.orig_amount)>0)
		{
			original_tx_amt = atoi(Auth_Tx.TLF01_details.orig_amount);
		}
		else
		{
			original_tx_amt = atoi(Auth_Tx.TLF01_details.total_amount);
		}
		sprintf(temp_totalamt,"%010d.%02d",original_tx_amt/100, original_tx_amt%100);
	}
	else if((Check_If_VISA_Transaction () == true)||
			(Check_If_JCB_Transaction ()  == true)||
			(Check_If_MC_Transaction ()  == true)||
			(Check_If_Fleet_Transaction() == true)||
			 Check_If_AMEX_Transaction ()==true)
	{
		if(strlen(Auth_Tx.reconciliation_amount) > 0)
		{
			sprintf(temp_totalamt,"%010d.%02d",atoi(Auth_Tx.reconciliation_amount)/100, atoi(Auth_Tx.reconciliation_amount)%100);
		}
		else
		{
			sprintf(temp_totalamt,"%010d.%02d",atoi(Auth_Tx.TLF01_details.total_amount)/100, atoi(Auth_Tx.TLF01_details.total_amount)%100);
		}
	}
	strncat(Cr_Fld.USER_DATA_3_STRG,temp_totalamt,13);
	sprintf(&message[581],"%-15s\n",Cr_Fld.USER_DATA_3_STRG);
}
void FALCON_build_request_field_69( )
{
	if(strlen(Auth_Tx.TLF01_details.response_text)>0)
	{
		strncpy(Cr_Fld.USER_DATA_4_STRG,Auth_Tx.TLF01_details.response_text,20);
	}
	else
	{
		memset(Cr_Fld.USER_DATA_4_STRG,SPACES,20);
	}
	sprintf(&message[596],"%-20s\n",Cr_Fld.USER_DATA_4_STRG);
}
void FALCON_build_request_field_70( )
{
	CHAR resp_buf[15] = {0} ;
	memset(Cr_Fld.USER_DATA_5_STRG,' ',40);
	if ((Auth_Tx.TLF01_details.tx_key) > 100)
	{
		strncpy (resp_buf , " RESPONSE",10);
	}
	else
	{
		memset(resp_buf,0x00,sizeof(resp_buf)-1);
	}

	switch(Auth_Tx.TLF01_details.tx_key)
	{
		case AUTH_INVALID							  :

			strcpy(Cr_Fld.USER_DATA_5_STRG,"INVALID");
		break ;

		case AUTH_SALE                     	     	  :
		case AUTH_SALE_RESPONSE                       :
			strcpy(Cr_Fld.USER_DATA_5_STRG,"SALE");
		break ;

		case AUTH_AUTHORIZATION						  :
		case AUTH_AUTHORIZATION_RESPONSE			  :
			strcpy(Cr_Fld.USER_DATA_5_STRG,"AUTHORIZATION");
		break ;

		case AUTH_CASH_ADVANCE				          :
		case AUTH_CASH_ADVANCE_RESPONSE               :
			strcpy(Cr_Fld.USER_DATA_5_STRG,"CASH ADVANCE");
		break ;

		case AUTH_OFFLINE_SALE                        :
		case AUTH_OFFLINE_SALE_RESPONSE               :
			strcpy(Cr_Fld.USER_DATA_5_STRG,"OFFLINE SALE");
		break ;

		case AUTH_SALE_ADJUSTMENT                     :
		case AUTH_SALE_ADJUSTMENT_RESPONSE            :
			strcpy(Cr_Fld.USER_DATA_5_STRG,"SALE ADJUSTMENT");
		break ;

		case AUTH_BALANCE_INQUIRY                     :
		case AUTH_BALANCE_INQUIRY_RESPONSE            :
			strcpy(Cr_Fld.USER_DATA_5_STRG,"BALANCE INQUIRY");
		break ;

		case AUTH_PRE_AUTHORIZATION                   :
		case AUTH_PRE_AUTHORIZATION_RESPONSE          :
			strcpy(Cr_Fld.USER_DATA_5_STRG,"PRE AUTHORIZATION");
		break ;

		case AUTH_VOID_SALE                           :
		case AUTH_VOID_SALE_RESPONSE                  :
			strcpy(Cr_Fld.USER_DATA_5_STRG,"VOID SALE");
		break ;

		case AUTH_VOID_REFUND                         :
		case AUTH_VOID_REFUND_RESPONSE                :
			strcpy(Cr_Fld.USER_DATA_5_STRG,"VOID REFUND");
		break ;

		case AUTH_REVERSAL                            :
		case AUTH_REVERSAL_RESPONSE                   :
			strcpy(Cr_Fld.USER_DATA_5_STRG,"REVERSAL");
		break ;

		case AUTH_VOID_CASH_ADVANCE                   :
		case AUTH_VOID_CASH_ADVANCE_RESPONSE          :
			strcpy(Cr_Fld.USER_DATA_5_STRG,"VOID CASH ADVANCE");
		break ;

		case AUTH_SALE_CASH                           :
		case AUTH_SALE_CASH_RESPONSE                  :
			strcpy(Cr_Fld.USER_DATA_5_STRG,"SALE CASH");
		break ;

		case AUTH_SALES_COMPLETION                    :
		case AUTH_SALES_COMPLETION_RESPONSE           :
			strcpy(Cr_Fld.USER_DATA_5_STRG,"SALE COMPLETION");
		break ;

		case AUTH_QUASI_CASH                          :
		case AUTH_QUASI_CASH_RESPONSE                 :
			strcpy(Cr_Fld.USER_DATA_5_STRG,"QUASI CASH");
		break ;

		case AUTH_ADVICE                              :
		case AUTH_ADVICE_RESPONSE                     :
			strcpy(Cr_Fld.USER_DATA_5_STRG,"ADVICE");
		break ;

		case AUTH_REVERSAL_ADVICE                     :
		case AUTH_REVERSAL_ADVICE_RESPONSE            :
			strcpy(Cr_Fld.USER_DATA_5_STRG,"REVERSAL ADVICE");
		break ;

		case AUTH_ADVICE_REPEAT                       :
		case AUTH_ADVICE_REPEAT_RESPONSE              :
			strcpy(Cr_Fld.USER_DATA_5_STRG,"ADVICE REPEAT");
		break ;

		case AUTH_REVERSAL_REPEAT                     :
		case AUTH_REVERSAL_REPEAT_RESPONSE            :
			strcpy(Cr_Fld.USER_DATA_5_STRG,"REVERSAL REPEAT");
		break ;

		case AUTH_CASH_ADVANCE_AUTHORIZATION          :
		case AUTH_CASH_ADVANCE_AUTHORIZATION_RESPONSE :
			strcpy(Cr_Fld.USER_DATA_5_STRG,"CASH ADVANCE AUTHORIZATION");
		break ;

		case AUTH_AUTHORIZATION_OVERRIDE              :
		case AUTH_AUTHORIZATION_OVERRIDE_RESPONSE     :
			strcpy(Cr_Fld.USER_DATA_5_STRG,"AUTHORIZATION OVERRIDE");
		break ;

		case AUTH_OFFLINE_VOID_SALE                   :
		case AUTH_OFFLINE_VOID_SALE_RESPONSE          :
			strcpy(Cr_Fld.USER_DATA_5_STRG,"OFFLINE VOID SALE");
		break ;

		case AUTH_PRE_AUTHORIZATION_CANCLE            :
		case AUTH_PRE_AUTHORIZATION_CANCLE_RESPONSE   :
			strcpy(Cr_Fld.USER_DATA_5_STRG,"PRE AUTHORIZATION CANCLE");
		break ;

	}
	strcat(Cr_Fld.USER_DATA_5_STRG,resp_buf);


	sprintf(&message[616],"%-40s\n",Cr_Fld.USER_DATA_5_STRG);
}
void FALCON_build_request_field_71( )
{
	memset(Cr_Fld.REAL_TIME_REQUEST_XFLG,' ',1);
	sprintf(&message[656],"%-1s\n",Cr_Fld.REAL_TIME_REQUEST_XFLG);
}
void FALCON_build_request_field_72( )
{
	memset(Cr_Fld.PRIOR_ACTION_DATABASE_XCD,' ',1);
	sprintf(&message[657],"%-1s\n",Cr_Fld.PRIOR_ACTION_DATABASE_XCD);
}
void FALCON_build_request_field_73( )
{
	memset(Cr_Fld.PRIOR_ACTION_DB_EXPIRATION_DT,' ',8);
	sprintf(&message[658],"%-8s\n",Cr_Fld.PRIOR_ACTION_DB_EXPIRATION_DT);
}
void FALCON_build_request_field_74( )
{
	memset(Cr_Fld.MASTER_ACCOUNT_NUMBER_XID,' ',19);
	sprintf(&message[666],"%-19s\n",Cr_Fld.MASTER_ACCOUNT_NUMBER_XID);
}
void FALCON_build_request_field_75( )
{
	memset(Cr_Fld.CARD_OFFLN_STATIC_AUTHCTN_XFLG,' ',1);
	sprintf(&message[685],"%-1s\n",Cr_Fld.CARD_OFFLN_STATIC_AUTHCTN_XFLG);
}
void FALCON_build_request_field_76( )
{
	memset(Cr_Fld.CARD_DYNAMIC_AUTHCTN_XCD,' ',1);
	sprintf(&message[686],"%-1s\n",Cr_Fld.CARD_DYNAMIC_AUTHCTN_XCD);
}
void FALCON_build_request_field_77( )
{
	memset(Cr_Fld.RESERVED_02,' ',1);
	sprintf(&message[687],"%-1s\n",Cr_Fld.RESERVED_02);
}
void FALCON_build_request_field_78( )
{
	memset(Cr_Fld.CARD_AIP_VERIFICATION_XFLG,' ',1);
	sprintf(&message[688],"%-1s\n",Cr_Fld.CARD_AIP_VERIFICATION_XFLG);
}
void FALCON_build_request_field_79( )
{
	memset(Cr_Fld.CARD_AIP_RISK_XFLG,' ',1);
	sprintf(&message[689],"%-1s\n",Cr_Fld.CARD_AIP_RISK_XFLG);
}
void FALCON_build_request_field_80( )
{
	memset(Cr_Fld.CARD_ISSUER_AUTH_SUPPORT_XFLG,' ',1);
	sprintf(&message[690],"%-1s\n",Cr_Fld.CARD_ISSUER_AUTH_SUPPORT_XFLG);
}
void FALCON_build_request_field_81( )
{
	memset(Cr_Fld.CARD_COMBINED_AUTHCTN_XFLG,' ',1);
	sprintf(&message[691],"%-1s\n",Cr_Fld.CARD_COMBINED_AUTHCTN_XFLG);
}
void FALCON_build_request_field_82( )
{
	memset(Cr_Fld.TRANSACTION_DELINQUENT_CYC,' ',1);
	sprintf(&message[692],"%-1s\n",Cr_Fld.TRANSACTION_DELINQUENT_CYC);
}
void FALCON_build_request_field_83( )
{
	memset(Cr_Fld.TRANS_TOTAL_DELINQUENT_AMT,' ',13);
	sprintf(&message[693],"%-13s\n",Cr_Fld.TRANS_TOTAL_DELINQUENT_AMT);
}
void FALCON_build_request_field_84( )
{
	memset(Cr_Fld.CRD_STATEMENT_CASH_BALANCE_AMT,' ',13);
	sprintf(&message[706],"%-13s\n",Cr_Fld.CRD_STATEMENT_CASH_BALANCE_AMT);
}
void FALCON_build_request_field_85( )
{
	memset(Cr_Fld.MERCHANT_BALANCE_AMT,' ',13);
	sprintf(&message[719],"%-13s\n",Cr_Fld.MERCHANT_BALANCE_AMT);
}
void FALCON_build_request_field_86( )
{
	memset(Cr_Fld.CUSTOMER_POSTAL_XCD,' ',12);
	sprintf(&message[732],"%-12s\n",Cr_Fld.CUSTOMER_POSTAL_XCD);
}
void FALCON_build_request_field_87( )
{
	memset(Cr_Fld.CARD_MEDIA_TYPE_XCD,' ',1);
	sprintf(&message[744],"%-1s\n",Cr_Fld.CARD_MEDIA_TYPE_XCD);
}
void FALCON_build_request_field_88( )
{
	if (strcmp(Auth_Tx.BIN01_details.card_family, CARD_MASTERCARD)== 0)
	{
		if(Auth_Tx.TLF01_details.cvc == 'P')
		{
			Cr_Fld.CVV2_PRESENT_XFLG[0] = '0' ;
		}
		else if(Auth_Tx.TLF01_details.cvc == 'M' ||
				Auth_Tx.TLF01_details.cvc == 'N')
		{
			Cr_Fld.CVV2_PRESENT_XFLG[0] = '1' ;
		}
		else if(Auth_Tx.TLF01_details.cvc == 'U')
		{
			Cr_Fld.CVV2_PRESENT_XFLG[0] = ' ' ;
		}
		else
		{
			Cr_Fld.CVV2_PRESENT_XFLG[0] = '9' ;
		}
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_VISA)== 0)
	{
		if( Auth_Tx.TLF01_details.product_codes[17].quantity[0] == 'C' ||
		    Auth_Tx.TLF01_details.product_codes[17].quantity[0] == 'D' ||
			Auth_Tx.TLF01_details.product_codes[17].quantity[0] == 'K' ||
			Auth_Tx.TLF01_details.product_codes[17].quantity[0] == 'L' ||
			Auth_Tx.TLF01_details.product_codes[17].quantity[0] == 'M' ||
			Auth_Tx.TLF01_details.product_codes[17].quantity[0] == 'N' ||
			Auth_Tx.TLF01_details.product_codes[17].quantity[0] == 'P')
		{
			Cr_Fld.CVV2_PRESENT_XFLG[0] = '1';
		}
		else if (Auth_Tx.TLF01_details.product_codes[17].quantity[0] == 'U')
		{
			Cr_Fld.CVV2_PRESENT_XFLG[0] = '9';
		}
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_JCB)== 0)
	{
		Cr_Fld.CVV2_PRESENT_XFLG[0] = ' ';
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_AMEX)== 0)
	{
		/* Not getting value*/
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_CUP)== 0)
	{
		/* Not getting value*/
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_DINERS)== 0)
	{
		/*Not getting value*/
	}


	sprintf(&message[745],"%-1s\n",Cr_Fld.CVV2_PRESENT_XFLG);
}
void FALCON_build_request_field_89( )
{
	if (strcmp(Auth_Tx.BIN01_details.card_family, CARD_MASTERCARD)== 0)
	{
		if(Auth_Tx.TLF01_details.cvc == 'P')
		{
			Cr_Fld.CVV2_RESPONSE_XFLG[0] = 'P' ;
		}
		else if(Auth_Tx.TLF01_details.cvc == 'M')
		{
			Cr_Fld.CVV2_RESPONSE_XFLG[0] = 'M' ;
		}
		else if(Auth_Tx.TLF01_details.cvc == 'N')
		{
			Cr_Fld.CVV2_RESPONSE_XFLG[0] = 'N' ;
		}
		else
		{
			Cr_Fld.CVV2_RESPONSE_XFLG[0] = ' ' ;
		}
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_VISA)== 0)
	{
		if( Auth_Tx.TLF01_details.product_codes[17].quantity[0] == 'C' ||
			Auth_Tx.TLF01_details.product_codes[17].quantity[0] == 'K' ||
			Auth_Tx.TLF01_details.product_codes[17].quantity[0] == 'M' )
		{
			Cr_Fld.CVV2_RESPONSE_XFLG[0] = 'M';
		}
		else if ( Auth_Tx.TLF01_details.product_codes[17].quantity[0] == 'D' ||
				  Auth_Tx.TLF01_details.product_codes[17].quantity[0] == 'L' ||
				  Auth_Tx.TLF01_details.product_codes[17].quantity[0] == 'N')
		{
			Cr_Fld.CVV2_RESPONSE_XFLG[0] = 'N';
		}
		else if ( Auth_Tx.TLF01_details.product_codes[17].quantity[0] == 'P')
		{
			Cr_Fld.CVV2_RESPONSE_XFLG[0] = 'P';
		}
		else if( Auth_Tx.TLF01_details.product_codes[17].quantity[0] == 'S')
		{
			Cr_Fld.CVV2_RESPONSE_XFLG[0] = 'S';
		}
		else if( Auth_Tx.TLF01_details.product_codes[17].quantity[0] == 'U')
		{
			Cr_Fld.CVV2_RESPONSE_XFLG[0] = 'U';
		}
		else
		{
			Cr_Fld.CVV2_RESPONSE_XFLG[0] = ' ';
		}
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_JCB)== 0)
	{
		Cr_Fld.CVV2_RESPONSE_XFLG[0] = ' ';
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_AMEX)== 0)
	{
		/* Not getting value*/
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_CUP)== 0)
	{
		/* Not getting value*/
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_DINERS)== 0)
	{
		/* Not getting value*/
	}


	sprintf(&message[746],"%-1s\n",Cr_Fld.CVV2_RESPONSE_XFLG);
}
void FALCON_build_request_field_90( )
{
	/*****memset(Cr_Fld.AVS_RESPONSE_XCD,' ',1);*******/
	memset(Cr_Fld.AVS_RESPONSE_XCD,' ',1);
	sprintf(&message[747],"%-1s\n",Cr_Fld.AVS_RESPONSE_XCD);
	/***memcpy(Cr_Fld.TRANSACTION_CATEGORY_XCD,"O",1);*****/
}
void FALCON_build_request_field_91( )
{
	if(strlen(Cr_Fld.TRANSACTION_CATEGORY_XCD) == 0 || strcmp(Cr_Fld.TRANSACTION_CATEGORY_XCD, " ") == 0)
	strncpy(Cr_Fld.TRANSACTION_CATEGORY_XCD,"O",1);
	sprintf(&message[748],"%-1s\n",Cr_Fld.TRANSACTION_CATEGORY_XCD);
	/*Auth_Tx.TLF01_details.acquirer_id*/
	/******memset(Cr_Fld.ACQUIRER_XID,' ',12);*****/
}
void FALCON_build_request_field_92( )
{
	strncpy(Cr_Fld.ACQUIRER_XID,Auth_Tx.TLF01_details.acquiring_id,12);
	sprintf(&message[749],"%-12s\n",Cr_Fld.ACQUIRER_XID);
	/*If it's a local transactions*/
}
void FALCON_build_request_field_93( )
{
	if(strncmp(Auth_Tx.TLF01_details.handler_queue,"dcpisoA",5) == 0)
	{
		strncpy(Cr_Fld.ACQUIRER_COUNTRY_XCD,"608",3);
	}
	else
	{
		if(isnum(Auth_Tx.country_code))
		{
			strncpy(Cr_Fld.ACQUIRER_COUNTRY_XCD,Auth_Tx.country_code,3);
		}
		else if (isnum(Auth_Tx.reconciliation_cur_code))
		{
			strncpy(Cr_Fld.ACQUIRER_COUNTRY_XCD,Auth_Tx.reconciliation_cur_code,3);
		}
		else if (isnum(Auth_Tx.TLF01_details.currency_code))
		{
			strncpy(Cr_Fld.ACQUIRER_COUNTRY_XCD,Auth_Tx.TLF01_details.currency_code,3);
		}
		else
		{
			strncpy(Cr_Fld.ACQUIRER_COUNTRY_XCD,"608",3);
		}
	}
	sprintf(&message[761],"%-3s\n",Cr_Fld.ACQUIRER_COUNTRY_XCD);
}
void FALCON_build_request_field_94( )
{
	if(strcmp(Auth_Tx.TLF01_details.terminal_id,"") != 0)
	{
		memcpy(Cr_Fld.TERMINAL_XID,Auth_Tx.TLF01_details.terminal_id,16);
		sprintf(&message[764],"%-16s\n",Cr_Fld.TERMINAL_XID);
	}
	else
	{
		memset(Cr_Fld.TERMINAL_XID,' ',16);
		sprintf(&message[764],"%-16s\n",Cr_Fld.TERMINAL_XID);
	}
}
void FALCON_build_request_field_95( )
{
	memset(Cr_Fld.TERMINAL_TYPE_XCD,' ',1);
	if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_VISA)	== 0)
	{
		if( Auth_Tx.TLF01_details.product_codes[16].amount[1] == '1' ||
			Auth_Tx.TLF01_details.product_codes[16].amount[1] == '4')
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"L",1);
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[1] == '3' ||
				Auth_Tx.TLF01_details.product_codes[16].amount[1] == '2' )
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"U",1);
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[1] == '5' ||
				Auth_Tx.TLF01_details.product_codes[16].amount[1] == '7' )
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"I",1);
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[1] == '0' )
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD," ",1);
		}
		else
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"N",1);
		}
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_MASTERCARD) == 0)
	{
		if( Auth_Tx.TLF01_details.product_codes[16].amount[10] == '4' ||
		   Auth_Tx.TLF01_details.product_codes[16].amount[10] == '6' ||
		   Auth_Tx.TLF01_details.product_codes[16].amount[10] == '7' )
	   {
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"I",1);
	   }
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[10] == '3' )
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"L",1);
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[10] == '2' ||
				Auth_Tx.TLF01_details.product_codes[16].amount[10] == '1')
		{
			memset(Cr_Fld.TERMINAL_TYPE_XCD,'U',1);
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[10] == '0')
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD," ",1);
		}
		else
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"O",1);
		}
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_AMEX)== 0)
	{
		strncpy(Cr_Fld.TERMINAL_TYPE_XCD,&Auth_Tx.amex_pos_entry_mode[3],1);
		if(strncmp(Cr_Fld.TERMINAL_TYPE_XCD,"1",1) == 0 ||
		   strncmp(Cr_Fld.TERMINAL_TYPE_XCD,"3",1) == 0 ||
		   strncmp(Cr_Fld.TERMINAL_TYPE_XCD,"X",1) == 0)
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"L",1);
		}
		else if(strncmp(Cr_Fld.TERMINAL_TYPE_XCD,"2",1) == 0 ||
		   strncmp(Cr_Fld.TERMINAL_TYPE_XCD,"4",1) == 0 ||
		   strncmp(Cr_Fld.TERMINAL_TYPE_XCD,"5",1) == 0 ||
		   strncmp(Cr_Fld.TERMINAL_TYPE_XCD,"Z",1) == 0)
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"U",1);
		}
		else if(strncmp(Cr_Fld.TERMINAL_TYPE_XCD,"S",1) == 0 ||
		   strncmp(Cr_Fld.TERMINAL_TYPE_XCD,"T",1) == 0)
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"I",1);
		}
		else if(strncmp(Cr_Fld.TERMINAL_TYPE_XCD,"0",1) == 0 ||
			strncmp(Cr_Fld.TERMINAL_TYPE_XCD,"9",1) == 0)
		{
			memset(Cr_Fld.TERMINAL_TYPE_XCD,' ',1);
		}
		else
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"O",1);
		}
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_CUP)== 0)
	{
		if( 0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"VOICE",5))
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"I",1);
		}
		else if( strncmp(&Auth_Tx.TLF01_details.product_codes[16].amount[4] , "03" ,2)== 0 ||
				 strncmp(&Auth_Tx.TLF01_details.product_codes[16].amount[4] , "06" ,2)== 0 ||
				 strncmp(&Auth_Tx.TLF01_details.product_codes[16].amount[4] , "11" ,2)== 0 )
		{
			Cr_Fld.TERMINAL_TYPE_XCD[0] = 'L' ;
		}
		else if( strncmp(&Auth_Tx.TLF01_details.product_codes[16].amount[4] , "01" ,2)== 0 )
		{
			Cr_Fld.TERMINAL_TYPE_XCD[0] = 'U' ;
		}
		else if( strncmp(&Auth_Tx.TLF01_details.product_codes[16].amount[4] , "05" ,2)== 0 ||
				 strncmp(&Auth_Tx.TLF01_details.product_codes[16].amount[4] , "07" ,2)== 0 ||
				 strncmp(&Auth_Tx.TLF01_details.product_codes[16].amount[4] , "08" ,2)== 0 ||
				 strncmp(&Auth_Tx.TLF01_details.product_codes[16].amount[4] , "09" ,2)== 0 ||
				 strncmp(&Auth_Tx.TLF01_details.product_codes[16].amount[4] , "12" ,2)== 0 ||
				 strncmp(&Auth_Tx.TLF01_details.product_codes[16].amount[4] , "14" ,2)== 0 ||
				 strncmp(&Auth_Tx.TLF01_details.product_codes[16].amount[4] , "16" ,2)== 0 ||
				 strncmp(&Auth_Tx.TLF01_details.product_codes[16].amount[4] , "17" ,2)== 0)
		{
			Cr_Fld.TERMINAL_TYPE_XCD[0] = 'I' ;
		}
		else if ( strncmp(&Auth_Tx.TLF01_details.product_codes[16].amount[4] , "00" ,2)== 0)
		{
			Cr_Fld.TERMINAL_TYPE_XCD[0] = ' ' ;
		}
		else
		{
			Cr_Fld.TERMINAL_TYPE_XCD[0] = 'O' ;
		}

	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_DINERS)== 0)
	{
		if(Auth_Tx.TLF01_details.product_codes[16].amount[3] == '1' ||
				Auth_Tx.TLF01_details.product_codes[16].amount[3] == '3'  )
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"L",1);
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[3] == '2' ||
				Auth_Tx.TLF01_details.product_codes[16].amount[3] == '4' ||
				Auth_Tx.TLF01_details.product_codes[16].amount[3] == '5' )
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"U",1);
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[3] == '9')
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD," ",1);
		}
		else
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"O",1);
		}
	}

	sprintf(&message[780],"%-1s\n",Cr_Fld.TERMINAL_TYPE_XCD);
}

	/*********memcpy(Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD,"O",1);*****************/
	/*Field 96 Terminal Entry capability */
void FALCON_build_request_field_96( )
{
	memset(Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD,' ',1);
	if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_VISA)	== 0)
	{
		if(Auth_Tx.TLF01_details.product_codes[16].amount[1] == '5')
		{
			Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD[0] = 'C' ;
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[1] == '8')
		{
			Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD[0] = 'D' ;
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[1] == '9')
		{
			Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD[0] = 'K' ;
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[1] == '2')
		{
			Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD[0] = 'M' ;
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[1] == '3' ||
				Auth_Tx.TLF01_details.product_codes[16].amount[1] == '4')
		{
			Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD[0] = 'O' ;
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[1] == '1')
		{
			Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD[0] = 'Z' ;
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[1] == '0')
		{
			Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD[0] = ' ' ;
		}
		else
		{
			Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD[0] = 'N' ;
		}
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_MASTERCARD) == 0)
	{
	   if(Auth_Tx.TLF01_details.product_codes[16].amount[10] == '5' ||
		  Auth_Tx.TLF01_details.product_codes[16].amount[10] == '8'	||
		  Auth_Tx.TLF01_details.product_codes[16].amount[10] == '9'	 )
	   {
		   Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD[0] = 'C' ;
	   }
	   else if(Auth_Tx.TLF01_details.product_codes[16].amount[10] == '3')
	   {
		   Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD[0] = 'D' ;
	   }
	   else if(Auth_Tx.TLF01_details.product_codes[16].amount[10] == '6')
	   {
		   Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD[0] = 'K' ;
	   }
	   else if(Auth_Tx.TLF01_details.product_codes[16].amount[10] == '2' ||
			   Auth_Tx.TLF01_details.product_codes[16].amount[10] == '7')
	   {
		   Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD[0] = 'M' ;
	   }
	   else if(Auth_Tx.TLF01_details.product_codes[16].amount[10] == '4')
	   {
		   Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD[0] = 'N' ;
	   }
	   else if(Auth_Tx.TLF01_details.product_codes[16].amount[10] == '1')
	   {
		   Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD[0] = 'Z' ;
	   }
	   else if(Auth_Tx.TLF01_details.product_codes[16].amount[10] == '0')
	   {
		   Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD[0] = ' ' ;
	   }
	   else
	   {
		   Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD[0] = 'O' ;
	   }
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_AMEX)== 0)
	{
		if( Auth_Tx.TLF01_details.product_codes[16].amount[0] == '5')
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"C",1);
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[0] == '1' ||
				Auth_Tx.TLF01_details.product_codes[16].amount[0] == '6' )
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"K",1);
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[0] == '2')
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"M",1);
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[0] == '0')
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD," ",1);
		}
		else
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"O",1);
		}
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_CUP) == 0)
	{
		if(Auth_Tx.TLF01_details.product_codes[16].amount[1] == '2')
		{
			Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD[0] = 'M';
		}
		else if (Auth_Tx.TLF01_details.product_codes[16].amount[1] == '5' ||
				Auth_Tx.TLF01_details.product_codes[16].amount[1]  == '6')
		{
			Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD[0] = 'C';
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[1] == '0')
		{
			Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD[0] = ' ';
		}
		else
		{
			Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD[0] = 'O';
		}
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_DINERS) == 0)
	{
		if(Auth_Tx.TLF01_details.product_codes[16].amount[0] == '5')
		{
			strncpy(Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD,"C",1);
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[0] == '8')
		{
			strncpy(Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD,"D",1);
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[0] == '6' ||
				Auth_Tx.TLF01_details.product_codes[16].amount[0] == '1')
		{
			strncpy(Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD,"K",1);
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[0] == '2')
		{
			strncpy(Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD,"M",1);
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[0] == '0')
		{
			strncpy(Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD," ",1);
		}
		else
		{
			strncpy(Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD,"O",1);
		}
	}
	sprintf(&message[781],"%-1s\n",Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD);
}
void FALCON_build_request_field_97( )
{
	strncpy(Cr_Fld.TRANSACTION_POS_CONDITION_XCD,Auth_Tx.TLF01_details.pos_condition_code,2);

	sprintf(&message[782],"%-2s\n",Cr_Fld.TRANSACTION_POS_CONDITION_XCD);
	/***************memcpy(Cr_Fld.ATM_NETWORK_XID,,);*************/
}
void FALCON_build_request_field_98( )
{
	memcpy(Cr_Fld.ATM_NETWORK_XID," ",1);
	sprintf(&message[784],"%-1s\n",Cr_Fld.ATM_NETWORK_XID);
}
void FALCON_build_request_field_99( )
{
	memset(Cr_Fld.RESERVED_01,' ',1);
	sprintf(&message[785],"%-1s\n",Cr_Fld.RESERVED_01);
}
void FALCON_build_request_field_100( )
{
	memset(Cr_Fld.CHECK_NUM,' ',6);
	sprintf(&message[786],"%-6s\n",Cr_Fld.CHECK_NUM);
}
void FALCON_build_request_field_101( )
{
	memset(Cr_Fld.TERML_VERIFICATION_RESULTS_XCD,' ',10);
	sprintf(&message[792],"%-10s\n",Cr_Fld.TERML_VERIFICATION_RESULTS_XCD);
}
void FALCON_build_request_field_102( )
{
	memset(Cr_Fld.CARD_VERIFICATION_RESULTS_XCD,' ',10);
	sprintf(&message[802],"%-10s\n",Cr_Fld.CARD_VERIFICATION_RESULTS_XCD);
}
	/*********memset(Cr_Fld.AUTHZN_RQST_CRYPTO_VALID_XFLG,' ',1);**********/
void FALCON_build_request_field_103( )
{
	memset(Cr_Fld.AUTHZN_RQST_CRYPTO_VALID_XFLG,' ',1);
	sprintf(&message[812],"%-1s\n",Cr_Fld.AUTHZN_RQST_CRYPTO_VALID_XFLG);
}
	/************memset(Cr_Fld.ATC_CARD_CNT,' ',5);***************/
void FALCON_build_request_field_104( )
{
	memset(Cr_Fld.ATC_CARD_CNT,' ',5);
	sprintf(&message[813],"%-5s\n",Cr_Fld.ATC_CARD_CNT);
}
	/*********memset(Cr_Fld.ATC_HOST_CNT,' ',5);**************/
void FALCON_build_request_field_105( )
{
	memset(Cr_Fld.ATC_HOST_CNT,' ',5);
	sprintf(&message[818],"%-5s\n",Cr_Fld.ATC_HOST_CNT);
}
void FALCON_build_request_field_106( )
{
	memset(Cr_Fld.RESERVED_03,' ',2);
	sprintf(&message[823],"%-2s\n",Cr_Fld.RESERVED_03);
}
void FALCON_build_request_field_107( )
{
	memset(Cr_Fld.TERML_TO_CRD_OFFLINE_LIMIT_XCD,' ',2);
	sprintf(&message[825],"%-2s\n",Cr_Fld.TERML_TO_CRD_OFFLINE_LIMIT_XCD);
}
void FALCON_build_request_field_108( )
{
	memset(Cr_Fld.TERMINAL_OFFLINE_LIMIT_MAX_XCD,' ',2);
	sprintf(&message[827],"%-2s\n",Cr_Fld.TERMINAL_OFFLINE_LIMIT_MAX_XCD);
}
void FALCON_build_request_field_109( )
{
	memset(Cr_Fld.RECURRING_AUTHORIZATION_XCD,' ',2);
	sprintf(&message[829],"%-2s\n",Cr_Fld.RECURRING_AUTHORIZATION_XCD);
}
void FALCON_build_request_field_110( )
{
	memset(Cr_Fld.RECURRING_AUTHZN_EXPIRATION_DT,' ',8);
	sprintf(&message[831],"%-8s\n",Cr_Fld.RECURRING_AUTHZN_EXPIRATION_DT);
}
void FALCON_build_request_field_111( )
{
	memset(Cr_Fld.CARD_ASSOCIATION_XFLG,' ',1);
	sprintf(&message[839],"%-1s\n",Cr_Fld.CARD_ASSOCIATION_XFLG);
}
void FALCON_build_request_field_112( )
{
	memset(Cr_Fld.CARD_INCENTIVE_XCD,' ',1);
	sprintf(&message[840],"%-1s\n",Cr_Fld.CARD_INCENTIVE_XCD);
}
void FALCON_build_request_field_113( )
{
	memset(Cr_Fld.CARD_STATUS_XCD,' ',2);
	sprintf(&message[841],"%-2s\n",Cr_Fld.CARD_STATUS_XCD);
}
void FALCON_build_request_field_114( )
{
	memset(Cr_Fld.CARD_STATUS_DT,' ',8);
	sprintf(&message[843],"%-8s\n",Cr_Fld.CARD_STATUS_DT);
}
	/********memset(Cr_Fld.PROCESSOR_REASON_XCD,' ',5);*****/
	/*****memcpy(&Cr_Fld.PROCESSOR_REASON_XCD[3],Auth_Tx.TLF01_details.response_code,2);**************/

void FALCON_build_request_field_115( )
{
		memset(Cr_Fld.PROCESSOR_REASON_XCD,' ',5);

	if(Falcon_is_Issuing_Transaction()== true)
	{
		if(Falcon_Is_Advice_or_Reversal_Transaction() == true)
		{
			if(true == Check_If_AMEX_Transaction())
			{

				memcpy(&Cr_Fld.PROCESSOR_REASON_XCD[2], Auth_Tx.TLF01_details.product_codes[12].code,3);
			}

			else if( true == Check_If_DINERS_Transaction())
			{

				memcpy(&Cr_Fld.PROCESSOR_REASON_XCD[2], Auth_Tx.TLF01_details.product_codes[12].code, 3);

			}
			else if( true == Check_If_CUP_Transaction() )
			{

				memcpy(&Cr_Fld.PROCESSOR_REASON_XCD[3],Auth_Tx.TLF01_details.product_codes[13].quantity,2);

			}
			else if( true == Check_If_MC_Transaction ())
			{

				memcpy(&Cr_Fld.PROCESSOR_REASON_XCD[3],Auth_Tx.TLF01_details.product_codes[13].quantity,2);

			}
			else if(true == Check_If_VISA_Transaction())
			{

				memcpy(&Cr_Fld.PROCESSOR_REASON_XCD[3],Auth_Tx.TLF01_details.product_codes[13].quantity,2);

			}
			else if(true == Check_If_JCB_Transaction ())
			{

				memcpy(&Cr_Fld.PROCESSOR_REASON_XCD[3],Auth_Tx.TLF01_details.product_codes[13].quantity,2);

			}
		}
		else
		{
			memcpy(&Cr_Fld.PROCESSOR_REASON_XCD[3],Auth_Tx.TLF01_details.response_code,2);
		}
	}
	else
	{
		memcpy(&Cr_Fld.PROCESSOR_REASON_XCD[3],Auth_Tx.TLF01_details.response_code,2);
	}

	 sprintf(&message[851],"%-5s\n",Cr_Fld.PROCESSOR_REASON_XCD);
}
void FALCON_build_request_field_116( )
{
	if(strcmp(Auth_Tx.TLF01_details.product_codes[1].code,"stndin") == 0)
	{
		memcpy(Cr_Fld.TRANSACTION_ADVICE_XCD,"A",1);
		sprintf(&message[856],"%-1s\n",Cr_Fld.TRANSACTION_ADVICE_XCD);
	}
	else
	{
		memset(Cr_Fld.TRANSACTION_ADVICE_XCD,' ',1);
		sprintf(&message[856],"%-1s\n",Cr_Fld.TRANSACTION_ADVICE_XCD);
	}
}
void FALCON_build_request_field_117( )
{
	memset(Cr_Fld.MERCHANT_XID,' ',16);
	memcpy(Cr_Fld.MERCHANT_XID,Auth_Tx.TLF01_details.merchant_id,15);
	sprintf(&message[857],"%-16s\n",Cr_Fld.MERCHANT_XID);
}
void FALCON_build_request_field_118( )
{
	memset(Cr_Fld.CARD_ORDER_TYPE_XCD,' ',1);
	sprintf(&message[873],"%-1s\n",Cr_Fld.CARD_ORDER_TYPE_XCD);
}
void FALCON_build_request_field_119( )
{
	memset(Cr_Fld.TRANSACTION_CASHBACK_AMT,' ',13);
	sprintf(&message[874],"%-13s\n",Cr_Fld.TRANSACTION_CASHBACK_AMT);
}
	/***********memset(Cr_Fld.USER_DATA_6_NUM,' ',13);************/
	/*Field 120 ECI value*/
void FALCON_build_request_field_120( )
{
	memset(Cr_Fld.USER_DATA_6_NUM,' ',13);

	if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_AMEX)	== 0)
	{
		memcpy(Cr_Fld.USER_DATA_6_NUM,&Auth_Tx.int_net_facility_data[5],2);
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_MASTERCARD)	== 0 ||
			strcmp(Auth_Tx.BIN01_details.card_family, CARD_VISA)	    == 0)
	{
		memcpy(Cr_Fld.USER_DATA_6_NUM,&Auth_Tx.TLF01_details.profile,2);
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_CUP)	== 0)
	{
		memcpy(Cr_Fld.USER_DATA_6_NUM,"00",2);
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_DINERS)	== 0)
	{
		memcpy(Cr_Fld.USER_DATA_6_NUM,&Auth_Tx.TLF01_details.product_codes[16].amount[1],2);
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_JCB)	== 0)
	{
		memset(Cr_Fld.USER_DATA_6_NUM,' ',13);
	}
	else
	{
		Cr_Fld.USER_DATA_6_NUM[0]= '2';
		strncpy(&Cr_Fld.USER_DATA_6_NUM[1],Auth_Tx.TLF01_details.profile,2);
	}


	sprintf(&message[887],"%-13s\n",Cr_Fld.USER_DATA_6_NUM);
	/*************memset(Cr_Fld.USER_DATA_7_STRG,' ',40);********/
}
	/* User data 7 filled with AMEX POS DATA - Auth_Tx.amex_pos_entry_mode*/
void FALCON_build_request_field_121( )
{
	memset(Cr_Fld.USER_DATA_7_STRG,' ',40);
	if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_AMEX)	== 0)
	{
		strncpy(Cr_Fld.USER_DATA_7_STRG,Auth_Tx.amex_pos_entry_mode,12);
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_VISA) == 0)
	{
		strncpy(Cr_Fld.USER_DATA_7_STRG,Auth_Tx.TLF01_details.product_codes[16].amount,12);
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_MASTERCARD) == 0)
	{
		strncpy(Cr_Fld.USER_DATA_7_STRG,Auth_Tx.TLF01_details.product_codes[16].amount,12);
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_CUP) == 0)
	{
		strncpy(Cr_Fld.USER_DATA_7_STRG,Auth_Tx.TLF01_details.product_codes[16].amount,12);
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_DINERS) == 0)
	{
			strncpy(Cr_Fld.USER_DATA_7_STRG,Auth_Tx.TLF01_details.product_codes[16].amount,12);
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_JCB) == 0)
	{
			strncpy(Cr_Fld.USER_DATA_7_STRG,Auth_Tx.TLF01_details.product_codes[16].amount,strlen(Auth_Tx.TLF01_details.product_codes[16].amount));
	}

	sprintf(&message[900],"%-40s\n",Cr_Fld.USER_DATA_7_STRG);
}
void FALCON_build_request_field_122( )
{
	memset(Cr_Fld.PAYMENT_INSTRUMENT_XID,' ',30);
	sprintf(&message[940],"%-30s\n",Cr_Fld.PAYMENT_INSTRUMENT_XID);
}
void FALCON_build_request_field_123( )
{
	memset(Cr_Fld.AVS_REQUEST_XCD,' ',1);
	sprintf(&message[970],"%-1s\n",Cr_Fld.AVS_REQUEST_XCD);
}
void FALCON_build_request_field_124( )
{
	memset(Cr_Fld.CVR_OFFLINEPIN_VERIFYPERF_XFLG,' ',1);
	sprintf(&message[971],"%-1s\n",Cr_Fld.CVR_OFFLINEPIN_VERIFYPERF_XFLG);
}
void FALCON_build_request_field_125( )
{
	memset(Cr_Fld.CVR_OFFLINEPIN_VERIFYSTAT_XFLG,' ',1);
	sprintf(&message[972],"%-1s\n",Cr_Fld.CVR_OFFLINEPIN_VERIFYSTAT_XFLG);
}
void FALCON_build_request_field_126( )
{
	memcpy(Cr_Fld.CVR_PIN_TRYLIMIT_EXCEED_XFLG,"0",1);
	sprintf(&message[973],"%-1s\n",Cr_Fld.CVR_PIN_TRYLIMIT_EXCEED_XFLG);
}
void FALCON_build_request_field_127( )
{
	memset(Cr_Fld.POS_TERMINAL_ATTEND_XFLG,' ',1);
	sprintf(&message[974],"%-1s\n",Cr_Fld.POS_TERMINAL_ATTEND_XFLG);
}
void FALCON_build_request_field_128( )
{
	memset(Cr_Fld.POS_OFF_PREMISES_XFLG,' ',1);
	sprintf(&message[975],"%-1s\n",Cr_Fld.POS_OFF_PREMISES_XFLG);
}
void FALCON_build_request_field_129( )
{
	memset(Cr_Fld.POS_CARD_CAPTURE_XFLG,' ',1);
	sprintf(&message[976],"%-1s\n",Cr_Fld.POS_CARD_CAPTURE_XFLG);
}
void FALCON_build_request_field_130( )
{
	memset(Cr_Fld.POS_SECURITY_XFLG,' ',1);
	sprintf(&message[977],"%-1s\n",Cr_Fld.POS_SECURITY_XFLG);
}
void FALCON_build_request_field_131( )
{
	memcpy(Cr_Fld.AUTHORIZATION_XID,Auth_Tx.TLF01_details.auth_number,6);
	sprintf(&message[978],"%-6s\n",Cr_Fld.AUTHORIZATION_XID);
}
void FALCON_build_request_field_132( )
{
	memset(Cr_Fld.USER_DATA_8_STRG,' ',10);

	memcpy(Cr_Fld.USER_DATA_8_STRG,&Auth_Tx.TLF01_details.pos_entry_mode,4);
	sprintf(&message[984],"%-10s\n",Cr_Fld.USER_DATA_8_STRG);
}
void FALCON_build_request_field_133( )
{
	memset(Cr_Fld.USER_DATA_9_STRG,' ',10);
	sprintf(&message[994],"%-10s\n",Cr_Fld.USER_DATA_9_STRG);
}
void FALCON_build_request_field_134( )
{
	memset(Cr_Fld.USER_INDICATOR_5_XCD,' ',1);
	sprintf(&message[1004],"%-1s\n",Cr_Fld.USER_INDICATOR_5_XCD);
}
void FALCON_build_request_field_135( )
{
	memset(Cr_Fld.USER_INDICATOR_6_XCD,' ',1);
	sprintf(&message[1005],"%-1s\n",Cr_Fld.USER_INDICATOR_6_XCD);
}

	/*Field 136 Service code*/
void FALCON_build_request_field_136( )
{
	memset(Cr_Fld.USER_INDICATOR_7_XCD,' ',5);

	strncpy(Cr_Fld.USER_INDICATOR_7_XCD,Auth_Tx.TLF01_details.service_code,3);
	sprintf(&message[1006],"%-5s\n",Cr_Fld.USER_INDICATOR_7_XCD);
}
void FALCON_build_request_field_137( )
{
	memset(Cr_Fld.USER_INDICATOR_8_XCD,' ',5);
	if(Is_BDO_Pos_Acquiring_Transaction() == true)
	{
		if(FALCON_is_BDO_PAY_QR_Transaction() ==true )
		{
			strncpy(Cr_Fld.USER_INDICATOR_8_XCD,Auth_Tx.TLF01_details.nii+1,3);
		}
	}
	sprintf(&message[1011],"%-5s\n",Cr_Fld.USER_INDICATOR_8_XCD);
}
void FALCON_build_request_field_138( )
{
	memset(Cr_Fld.MODEL_CONTROL_1_XCD,' ',1);
	sprintf(&message[1016],"%-1s\n",Cr_Fld.MODEL_CONTROL_1_XCD);
}
void FALCON_build_request_field_139( )
{
	memset(Cr_Fld.MODEL_CONTROL_2_XCD,' ',1);
	sprintf(&message[1017],"%-1s\n",Cr_Fld.MODEL_CONTROL_2_XCD);
}
void FALCON_build_request_field_140( )
{
	memset(Cr_Fld.MODEL_CONTROL_3_XCD,' ',1);
	sprintf(&message[1018],"%-1s\n",Cr_Fld.MODEL_CONTROL_3_XCD);
}
void FALCON_build_request_field_141( )
{
	memset(Cr_Fld.MODEL_CONTROL_4_XCD,' ',1);
	sprintf(&message[1019],"%-1s\n",Cr_Fld.MODEL_CONTROL_4_XCD);
}
void FALCON_build_request_field_142( )
{
	if(strcmp(Auth_Tx.TLF01_details.response_code,"54") == 0)
	{
		strncpy(Cr_Fld.AUTHORIZATION_EXPIRE_DT_XFLG,"I",1);
	}
	else
	{
		strncpy(Cr_Fld.AUTHORIZATION_EXPIRE_DT_XFLG,"V",1);
	}

	sprintf(&message[1020],"%-1s\n",Cr_Fld.AUTHORIZATION_EXPIRE_DT_XFLG);
}
void FALCON_build_request_field_143( )
{
	memcpy(Cr_Fld.AUTH_SECONDARY_VERIFY_XCD," ",1);

	sprintf(&message[1021],"%-1s\n",Cr_Fld.AUTH_SECONDARY_VERIFY_XCD);
}
void FALCON_build_request_field_144( )
{
	strncpy(Cr_Fld.AUTHORIZATION_RESPONSE_XCD," ",1);
	sprintf(&message[1022],"%s",Cr_Fld.AUTHORIZATION_RESPONSE_XCD);
}


INT Check_If_CUP_Transaction(void)
{
	if (0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"dcpimp",6))
	{
		if ((0 == strncmp(Auth_Tx.TLF01_details.card_num,CUP_MP_Bin_range_1,CUP_MP_Bin_range_len_1))||
			(0 == strncmp(Auth_Tx.TLF01_details.card_num,CUP_MP_Bin_range_2,CUP_MP_Bin_range_len_2))||
			(0 == strncmp(Auth_Tx.TLF01_details.card_num,CUP_MP_Bin_range_3,CUP_MP_Bin_range_len_3))||
			(0 == strncmp(Auth_Tx.TLF01_details.card_num,CUP_MP_Bin_range_4,CUP_MP_Bin_range_len_4))||
			(0 == strncmp(Auth_Tx.TLF01_details.card_num,CUP_MP_Bin_range_5,CUP_MP_Bin_range_len_5)))
		{
			return true ;
		}
		else
		{
			return false;
		}
	}
	else
	{
	if (0 == strncmp(Auth_Tx.TLF01_details.product_code,CUP_CARD_INDICATOR_FLAG,2))
	{
		return true ;
	}
	else if (0 == strncmp(Auth_Tx.TLF01_details.merchant_id,CUP_merchant_indicator,CUP_MID_INDICATOR_LEN))
	{
		if((true == Check_If_VISA_Transaction ()) ||
		   (true == Check_If_MC_Transaction()))
		{
			return true ;
		}
		else
		{
			strncpy(Auth_Tx.TLF01_details.product_code,CUP_CARD_INDICATOR_FLAG,2);
			return true ;
		}
	}
/*check for VOICE Txn in GUI*/
	else if(strncmp(Auth_Tx.BIN01_details.card_family,CARD_CUP,2)==0)
	{
		return true ;
	}
	else
	{
		return false;
	}
}
}


INT Check_If_VISA_Transaction(void)
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

INT Check_If_MC_Transaction(void)
{
	char MC_bin_series[7]   = {0};
	int  MC_Bin_series_low  = 222100;
	int  MC_Bin_series_high = 272099;
	int  MC_bin_value 		= 0;
	int  MC_Bin_serice_len  = 6 ;

	strncpy(MC_bin_series,Auth_Tx.TLF01_details.card_num,MC_Bin_serice_len);
	MC_bin_value = atoi(MC_bin_series);

	if(	Auth_Tx.TLF01_details.card_num[0]=='5')
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
INT Check_If_JCB_Transaction(void)
{
	if((Auth_Tx.TLF01_details.card_num[0]=='3') &&
	   (Auth_Tx.TLF01_details.card_num[1] == '5'))
	{
		return true ;
	}
	else
	{
		return false;
	}
}
INT Check_If_AMEX_Transaction(void)
{
	if((Auth_Tx.TLF01_details.card_num[0]=='3') &&
	   ((Auth_Tx.TLF01_details.card_num[1] == '4') ||
		(Auth_Tx.TLF01_details.card_num[1] == '7')))
	{
		return true ;
	}
	else
	{
		return false;
	}
}

INT Check_If_DINERS_Transaction(void)
{
	if((Auth_Tx.TLF01_details.card_num[0]=='3') &&
	   ((Auth_Tx.TLF01_details.card_num[1] == '0') ||
		(Auth_Tx.TLF01_details.card_num[1] == '6') ||
		(Auth_Tx.TLF01_details.card_num[1] == '8') ||
		(Auth_Tx.TLF01_details.card_num[1] == '9') ))
	{
		return true ;
	}
	else if ((Auth_Tx.TLF01_details.card_num[0]=='6') &&
			 ((Auth_Tx.TLF01_details.card_num[1] == '0') ||
			 (Auth_Tx.TLF01_details.card_num[1] == '4') ||
			 (Auth_Tx.TLF01_details.card_num[1] == '5')))
	{
		return true ;
	}
	else
	{
		return false;
	}
}

INT Check_If_Fleet_Transaction(void)
{
	char fleet_card_series[5]   = "7128";
	char private_card_series[5]  = "7188";
	int  fllet_Private_series  = 4 ;


	if(	0 == strncmp(Auth_Tx.TLF01_details.card_num,
					 fleet_card_series,fllet_Private_series))
	{
		return true ;
	}
	else if(0 == strncmp(Auth_Tx.TLF01_details.card_num,
						 private_card_series,fllet_Private_series))
	{
		return true ;
	}
	else
	{
		return false;
	}
}


/******************************************************************************
 *
 *  NAME:        FALCON_read_CUP_merchant_indicator
 *
 *  DESCRIPTION:  This function reads information from the tf ini file.
 *
 *  INPUTS:       None
 *  OUTPUTS:      err_buf - Text message explaining why this function failed
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Ramya
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT FALCON_read_CUP_merchant_indicator( void )
{
   DWORD rc = 0;
   CHAR filename     [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstr       [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstring    [MAX_INI_INFO_SIZE]= {0};
   CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
   INT  ret_val = true;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
   rc = GetPrivateProfileString(
             "CUPMIDSECTION",     /* points to section name       */
             "CUP_MID_INDICATOR",  /* points to key name           */
             "0",                       		/* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   strncpy( CUP_merchant_indicator, tmpstring, CUP_MID_INDICATOR_LEN );
   if(strlen(CUP_merchant_indicator)==0)
   {
	   strncpy( CUP_merchant_indicator,
			    CUP_MID_INDICATOR_DEFAULT,
			    CUP_MID_INDICATOR_LEN);/* Default Value*/
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:        FALCON_read_Workflow_indicator
 *
 *  DESCRIPTION:  This function reads information from the tf ini file.
 *
 *  INPUTS:       None
 *  OUTPUTS:      err_buf - Text message explaining why this function failed
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Abhishek
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT FALCON_read_Workflow_indicator( void )
{
   DWORD rc = 0;
   CHAR filename     [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstr       [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstring    [MAX_INI_INFO_SIZE]= {0};
   CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
   INT  ret_val = true;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   GetPrivateProfileString(
   							"FALCON",		/* points to section name*/
   							"WORKFLOW", 	/* points to key name*/
   							"", 		/* points to default string*/
							tmpstring,	/* points to destination buffer*/
   							sizeof(tmpstring) - 1,/* size of destination buffer*/
							filename );

   /* Convert entry from a string into a number. */
   strncpy( profile_str, tmpstring, strlen(tmpstring) );
   if(strlen(profile_str)==0)
   {
	   strncpy( profile_str,
			   WORKFLOW_DEFAULT_STRING,
			   WORKFLOW_DEFAULT_LEN);/* Default Value*/
   }
   return( ret_val );
}

int Convert_Amt_To_Auth_Tx_String_Format
                              (double nAmount, char* strAmount)
{
	char strTemp[256] = {0} ;
	int  nStringLength = 0 ;
	int  nIndex = 0 ;

	gcvt (nAmount, 13, strTemp) ; /* choice of 15 is arbitrary*/

	nStringLength = strlen (strTemp) ;
	/* Discard all characters after the decimal point, if present*/
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
/******************************************************************************
 *
 *  NAME:         falcon_log_message
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
 *  AUTHOR:       Raghu
 *
 ******************************************************************************/
void falcon_log_message( INT dest, INT sev, pCHAR msg, pCHAR func, int details )
{
   INT  monitor_flag = 0;
   INT  eventlog_flag = 0;
   INT  msg_type = 0;
   BYTE severity = 0;
   BYTE log_type[20] = {0};
   CHAR text_message [2000] = {0};
   INT  Appname_len = 0;

   /* Set Monitor flag */
   if ( dest == 1  ||  dest == 3 )
      monitor_flag = 1;
   else
      monitor_flag = 0;

   /* Set Event Log flag
   if ( dest > 1 )
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
   sprintf(text_message,"%s ,",App_Name);
   Appname_len = strlen(text_message);

   memcpy( text_message + Appname_len, msg, (sizeof(text_message)-1 -Appname_len));
   falcon_Log_error_warning_to_File(text_message,sev,func,details);
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

INT falcon_Log_error_warning_to_File(pCHAR Error_Buf,int sev, pCHAR func ,int detail)
{
	INT   ret_val = 0;
	INT   len=0;
	INT   cardlen = 0;
	INT   path_len = 0;
	INT   nIndex = 0;
	CHAR  time_date[25]  ={0};
	CHAR  timestamp[14]  ={0};
	CHAR  current_mmdd[5]={0};
	CHAR Buffer[2000]={0};
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
    len = strlen(falcon_Erro_warning_Filename);
    path_len = strlen(falcon_error_warning_file_path);
    if( len == 0 ||
    	path_len==0 )
    {
    	LogEvent(Error_Buf,INFO_MSG);
    	return 0;
    }
    if ( 0 != strcmp(&falcon_Erro_warning_Filename[len-4], current_mmdd) )
    {
		/* Now create the new filename.*/
    	falcon_create_Error_Filename();
    }
	if((fp = fopen(falcon_Erro_warning_Filename,"a+b"))==NULL)
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
		/* Do nothing fall below */
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

void falcon_create_Error_Filename()
{
	CHAR  system_date[25] = {0};

   /* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
	ptetime_get_timestamp( system_date );

    strcpy( falcon_Erro_warning_Filename, falcon_error_warning_file_path );
    strcat( falcon_Erro_warning_Filename, falcon_module_error_warning_file_name );
	strcat( falcon_Erro_warning_Filename, ".EW." );
    strncat(falcon_Erro_warning_Filename, system_date,   4 );  /* YYYY */
    strncat(falcon_Erro_warning_Filename, system_date+5, 2 );  /* MM   */
    strncat(falcon_Erro_warning_Filename, system_date+8, 2 );  /* DD   */
}


void EndProcess()
{
      CHAR Buffer[100] = "";

   sprintf( Buffer, "Shutting down %s Service, version %s",
            ServiceName, Version );
   LogEvent( Buffer, INFO_MSG );
   strcat( Buffer, "\n" );
   pteipc_shutdown_multiple_instance_app();
   pteipc_trace_off();

}


void PinnacleMsgHandler(pPTE_MSG p_msg_in)
{
   pPTE_MSG_DATA     p_msg_data = NULL_PTR;
   pBYTE			 p_data;
   pPTE_MSG			 p_cur_msg = 0 ;
   pPTE_MSG			 p_msg_out = 0 ;
   pPTE_MSG			 p_msg_ins  = NULL_PTR;
   pBYTE			 p_data_data = NULL_PTR;
   pBYTE			 pCurrent;

   BYTE     MessageType = 0 ;
   BYTE     ReturnCode = 0 ;
   CHAR     destination_que[7];
   CHAR     strTemp[256] = {0} ;
   INT      len=0;int dest_len = 0;
   BYTE     subtype1;
   BYTE     subtype2;
   CHAR		cmd[100];
   CHAR		bindir[256];  
   
   INT      msglen=0;
   CHAR     msgBuffer [2000] = {0} ;
   int 		tracing;
   char 	buffer[150]={0};
   BYTE           net_consec_tmouts_ctr[4];
   BYTE           active_txns_ctr[4];
   BYTE           current_state[2];
   BYTE           prev_src_station[4];
   BYTE           timer_result;
   CHAR           ErrorMsg[200] = "";
   CHAR           Buff[256];
   CHAR           ConnectionId[MAX_ORIG_INFO_SIZE+1] = {0} ;
   LONG           ret_code;
   INT            nIndex = 0 ;
   INT            ret_val;
  

   memset (&Auth_Tx, 0, sizeof (AUTH_TX)) ;
    
   p_cur_msg = p_msg_in ;

	
   FALCON_read_Workflow_indicator();
   MessageType = ptemsg_get_msg_type (p_cur_msg) ;
    switch (MessageType)
   {

   /*case MT_GET_STATS:
   	send_transaction_statistics( p_msg_in );
   	break;*/

   case MT_AUTH_REQUEST:
					RqMssg(p_cur_msg);
					msglen=strlen(message);
					sprintf(msgBuffer,"Msg to JMQ ==> msglen:%d message:%s",msglen,message);
					falcon_log_message(2,1,msgBuffer,"PinnacleMsgHandler",0);
				    break;

	case MT_SYSTEM_REPLY:
	break ;

	case MT_TIMER_REPLY:
	break ;

	case MT_TIMER_TIMEOUT:
     break ;

     case MT_SYSTEM_REQUEST:
          ptesystm_system_msg_handler (p_msg_in);
          switch (ptemsg_get_msg_subtype1 (p_msg_in))
          {
          	  case ST1_SYS_PING	:		if ( ST1_SYS_PING == ptemsg_get_msg_subtype1(p_msg_in) )
										{
												/* When pinged, display application version on Monitor. */
												sprintf( strTemp,
															"Pinged -> Falcon Service: %s, version %s",
															ServiceName, Version );
												TxUtils_Send_Msg_To_Operator( 1, strTemp, 1, INFO_MSG,
																	"PinnacleMsgHandler",
																	 0, INFO_ERROR, 0, 0, 0 );
										}
										break ;
          	  case ST1_SYS_IPC_TRACE_ON : if ( ST1_SYS_IPC_TRACE_ON == ptemsg_get_msg_subtype1(p_msg_in) )
										{
											pteipc_trace_on     ();
											memset(&message,0,sizeof(message));
											memcpy(message,"100",3);
										}
							            break;

          	  case ST1_SYS_IPC_TRACE_OFF: if ( ST1_SYS_IPC_TRACE_OFF == ptemsg_get_msg_subtype1(p_msg_in) )
										{
											pteipc_trace_off    ();
											memset(&message,0,sizeof(message));
											memcpy(message,"101",3);
										}
										break;
          	  	  default:
          	  		  break;
          }


      case MT_GET_STATS:
	         (void)send_transaction_statistics( p_msg_in );
	         break;


      default:
          strcpy (strTemp, "Unknown message type received in the Pinnacle Message Handler") ;
          TxUtils_Send_Msg_To_Operator 
				  (1, strTemp,
					1, ERROR_MSG,"FalconHandler.c", 4, FATAL_ERROR,0, 0, 0) ;
		break ;
    }  /* switch msg_type */
}
INT send_transaction_statistics( pPTE_MSG p_msg )
{
   INT   ret_code;
   BYTE  originator_queue[17]="";
   BYTE  originator_info[33] ="";
   CHAR  err_msg[100]        ="";

   /* Gather the statistics */
   /*(void)get_txn_stats( &Txn_Counts, &Monitor_Stats ); */

   /* Gather originator information */
   strcpy( originator_queue, ptemsg_get_msg_orig_queue ( p_msg ) );
   strcpy( originator_info,  ptemsg_get_msg_orig_comms_info (p_msg) );

   /* Send statistics to Monitor */
   ret_code = build_and_send_request_message( MT_GET_STATS_RESPONSE,
                                              ST1_NONE,
                                              ST2_NONE,
                                              originator_queue,
                                              0,
                                              0,
                                              0,
                                              originator_info,
                                              err_msg );
   if ( false == ret_code  )
   {
      falcon_log_message( 1, 2, err_msg, "send_transaction_statistics",1 );
   }
   return( ret_code );
}
INT build_and_send_request_message( BYTE     msgtype,
                                    BYTE     subtype1,
                                    BYTE     subtype2,
                                    pBYTE    que_name,
                                    pBYTE    buffer,
                                    INT      length,
                                    BYTE     app_type,
                                    pCHAR    comm_info,
                                    pCHAR    err_msg )
{
   CHAR            Buff[200];
   pPTE_MSG        p_msg    = NULL_PTR;
   INT             ret_code = true;
   LONG            msg_ret_code;

   p_msg = ptemsg_build_msg( msgtype,
                             subtype1,
                             subtype2,
                             que_name,
                             application_que_name,
                             buffer,
                             length,
                             app_type );

   if ( NULL_PTR == p_msg  )
   {
      strcpy( err_msg, "Insufficient Memory to build PTE message" );
      ret_code = false;
   }
   else
   {
      /* Set originator communications information. */
      if ( 0 != strcmp( NULL_STR, comm_info ) )
         ptemsg_set_orig_comms_info( p_msg, comm_info );

      /* Send message que_name */
      msg_ret_code = pteipc_send(p_msg, que_name);
      if(msg_ret_code < 0)
	  {
         pteipc_get_errormsg(msg_ret_code, err_msg);
         memset( Buff, 0x00, sizeof(Buff) );
         strcpy( Buff, "Failed transmit to queue: " );
         strcat( Buff, que_name );
         strncat(Buff, err_msg, 65 );
         ret_code = false;
	  }
      free(p_msg);
   }
   return( ret_code );
}

void java_signal_handler( int signal )
{
   /*printf("Falcon:Recived Signal Handler\n");*/
   memcpy(javaEndProcessSignalled,"1",1);
   sleep( 1 );
}
 

INT FalconHandler_initialize(CHAR* appName, CHAR* serName)
{
	INT ret_val = true;
	CHAR err_buf[100] = { 0 };
	CHAR tf_filename[256] = { 0 };
	CHAR tmpstr[256] = { 0 };
	/*long td_val;
	 td_val=pthread_self();

	 printf("\nIntialize:%ld\n",td_val);*/

	CHAR ini_info[MAX_INI_INFO_SIZE] = { 0 };
	CHAR filename[MAX_FILENAME_SIZE] = { 0 };
	CHAR tmpstring[MAX_INI_INFO_SIZE] = { 0 };

	GetAscendentConfigDirectory(tmpstr);
	sprintf(tf_filename, "%stf.ini", tmpstr);

	FALCON_read_CUP_merchant_indicator();
	falcon_get_error_warning_file_name_path();

	/*get record length from ini file*/
	GetPrivateProfileString("FALCON", /* points to section name*/
	"WORKFLOW", /* points to key name*/
	"APC80", /* points to default string*/
	profile_str, /* points to destination buffer*/
	sizeof(profile_str) - 1,/* size of destination buffer*/
	tf_filename); /* points to initialization filename*/

	/* This peace of code is alredy there in FALCON_read_CUP_merchant_indicator function
	 GetPrivateProfileString(
	 "CUPMIDSECTION",
	 "CUP_MID_INDICATOR",
	 "0",
	 ini_info,
	 MAX_INI_INFO_SIZE - 1,
	 filename
	 );


	 memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
	 strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
	 strncpy( CUP_merchant_indicator, tmpstring, CUP_MID_INDICATOR_LEN );
	 if(strlen(CUP_merchant_indicator)==0)
	 {
	 strncpy( CUP_merchant_indicator,
	 CUP_MID_INDICATOR_DEFAULT,
	 CUP_MID_INDICATOR_LEN);
	 }*/

	memcpy(javaEndProcessSignalled, "0", 1);
	if (signal(SIGTERM, SIG_IGN) != SIG_IGN) {
		signal(SIGTERM, java_signal_handler);
		/*printf("Signal Handler Has Been Set\n");*/
	}
	EndProcessSignalled = 0;
	CHAR strTemp[256] = { 0 };
	memset(&Auth_Tx, 0, sizeof Auth_Tx);
	memset(strAppName, 0x00, sizeof(strAppName));
	memset(ServiceName, 0x00, sizeof(ServiceName));
	const char *str;
	const char *str1;
	/*fp1 = fopen(fpath, "w");
	 fp2 = fopen(fpath1, "w");*/

	char temp[200] = { 0 };
	memset(&message, 0, sizeof(message));
	str = appName;
	if (str == NULL) {
		return INTIALIZE_EXCEPTION_ERROR; /* OutOfMemoryError already thrown */
	}
	memcpy(strAppName, str, sizeof(str));

	str1 = serName;
	if (str1 == NULL) {
		return INTIALIZE_EXCEPTION_ERROR; /* OutOfMemoryError already thrown */
	}
	memcpy(ServiceName, str1, sizeof(str1));

	/* Display Application version. */
	sprintf(strTemp, "Attempting to start %s, version %s", strAppName, Version);
	LogEvent(strTemp, INFO_MSG);

	/*  Create a table to be used for shared memory between multiple
	 *  instances of this  Module.  It contains three fields:
	 *  # of consecutive timeouts, # of active transactions,
	 *  and the current state of the host.
	 */

	/*#ifdef _DEBUG*/
	if (!pteipc_init_multiple_instance_app(strAppName, ServiceName, "pte_ipc"))
	/*#endif*/
	{
		sprintf(strTemp, "%s: Intialize: Failed to Create XIPC queues",
				strAppName);
		LogEvent(strTemp, ERROR_MSG);
		/* signal that we are ready to shut down and quit */
		/*#ifdef _DEBUG*/
		pteipc_shutdown_multiple_instance_app();
		/* #endif*/
		MainProcessDone = 1;

		return INTIALIZE_QUEUES_ERROR;

	}

	/**** ret_val = create_shared_mem_table( err_buf );*/

	sprintf(strTemp, "Started the FalconHandler Service: %s", ServiceName);
	LogEvent(strTemp, INFO_MSG);

	return INTIALIZE_QUEUES_NO_ERROR;
}


void falcon_get_error_warning_file_name_path(void )
{
   DWORD rc;
   CHAR  filename   [80] = {0};
   CHAR  tmpstr     [80] = {0};
   CHAR  falcon_error_warning_file_size[5] = {0};

   /* Get path to the tf.ini file. */
   memset( tmpstr, 0x00, sizeof(tmpstr) );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, "tf.ini" );

   /*Read path for creating file in order to log db statics and db oracle error messages */
   rc = GetPrivateProfileString(
								  "DATASERVERS",             /* points to section name         */
								  "DB_STATICS_PATH",       	/* points to key name             */
								   "",  					/* Default string                 */
								   falcon_error_warning_file_path,              	/* points to destination buffer   */
								   sizeof(falcon_error_warning_file_path)-1,   	 /* size of destination buffer     */
								   filename                  /* points to ini filename         */
						 	   );

   rc = GetPrivateProfileString(
								  "ERROR_WARNING",             /* points to section name         */
								  App_Name,       	/* points to key name             */
								  "",  					/* Default string                 */
								  falcon_module_error_warning_file_name,              	/* points to destination buffer   */
								  sizeof(falcon_module_error_warning_file_name)-1,   	 /* size of destination buffer     */
								  filename                  /* points to ini filename         */
								);

   rc = GetPrivateProfileString(
							  "ERROR_WARNING",             /* points to section name         */
							  "ERROR_WARNING_FILE_SIZE",       	/* points to key name             */
							  "500",  					/* Default string                 */
							  falcon_error_warning_file_size,              	/* points to destination buffer   */
							  sizeof(falcon_error_warning_file_size)-1,   	 /* size of destination buffer     */
							  filename                  /* points to ini filename         */
							);

	/* File size size conversion */
	Max_File_Size_Defined = atoi(falcon_error_warning_file_size);
	if(Max_File_Size_Defined <= 0)
	{
		Max_File_Size_Defined = 500 ;
	}
	Max_File_Size_Defined = Max_File_Size_Defined * ONE_MB_IN_BYTES ; /* 1 MB = 1048576 BYTES*/

   if((strlen(falcon_error_warning_file_path) > 0) &&
	  (strlen(falcon_module_error_warning_file_name)> 0))
   {
	   falcon_create_Error_Filename();
   }
}

CHAR* FalconHandler_sendRequestMessage(CHAR* srvNam)
{
	LONG lReturnCode = 0L;
	CHAR strTemp[256] = { 0 };
	pPTE_MSG p_msg = 0;
	INT err_ctr = 0;
	CHAR strBuffer[256] = { 0 };
	CHAR strVal[1] = { 0 };
	INT msglen1 = 0;
	CHAR fileinfo[256] = { 0 };
	CHAR ErrorMsg[100] = "";

	if (strcmp(javaEndProcessSignalled, "1") != 0) {

		p_msg = pteipc_receive(application_que_name, control_que_name, 5,
				&lReturnCode);

		if (p_msg != NULL_PTR) {
			count++;
			err_ctr = 0;
			PinnacleMsgHandler(p_msg);
			free(p_msg);
		} else if (lReturnCode != QUE_ER_TIMEOUT) {

			pteipc_get_errormsg(lReturnCode, strTemp);
			sprintf(strBuffer,
					"Error on pteipc_receive OR Error in Receving Message %s , error code: %ld",
					strTemp, lReturnCode);
			LogEvent(strBuffer, ALERT_MSG);
			memcpy(message, strBuffer, sizeof(strBuffer));

			/* Inform Operators if service's queue is no longer readable. */
			if (++err_ctr > 10) {
				err_ctr = 0;
				sprintf(strBuffer, "%s. UNABLE TO ACCESS QUEUE!!!!",
						ServiceName);
				LogEvent(strBuffer, ALERT_MSG);

			}
		} else {

			memset(&message, 0, sizeof(message));

		}

		return message;
	} else {
		pteipc_shutdown_multiple_instance_app();
		return javaEndProcessSignalled;
	}

}

void FalconHandler_receiveResponseMsg(char* ResponseMsg)
 {
	CHAR respbuffer[5000] = { 0 };
	CHAR respn_msg[5000] = { 0 };
	CHAR strTemp[256] = { 0 };
	INT msglen = 0;
	const char *str2;
	CHAR ErrorMsg[100] = "";

	if (strcmp(javaEndProcessSignalled, "1") != 0) {

		str2=ResponseMsg;
		if (str2 == NULL) {
			strcpy(strTemp,
					"receiveResponseMsg:Parameter passed from Java is NULL");
			LogEvent(strTemp, ERROR_MSG);
		}

		msglen = strlen(str2);

		sprintf(respbuffer, "msglen:%dResponseMsg:%s", msglen, str2);
		LogEvent(respbuffer, INFO_MSG);

	} else {
		/* Shutting down, need to change state before exiting Xipc. */
		pteipc_shutdown_multiple_instance_app();
	}

}
 
void FalconHandler_terminate()
{

	CHAR strTemp[256] = { 0 };
	CHAR SerName[20] = { 0 };
	strcpy(SerName, "FalconHandler");
	sprintf(strTemp, "Shutting down the FalconHandler Service: %s", SerName);
	LogEvent(strTemp, INFO_MSG);

	/* Log out of XIPC
	 #ifdef _DEBUG*/
	pteipc_shutdown_multiple_instance_app();
	/* #endif*/
	MainProcessDone = 1;
}

INT FalconHandler_getLibFileVersion()
{
	CHAR ver_buf[5] = { 0 };
	sprintf(ver_buf, "Falcon_Version:%s", Version);
	LogEvent(ver_buf, INFO_MSG);
	return (1);

}

void Falcon_build_CRTRAN_field_31_MC()
{

	len_billing_amount = strlen(Auth_Tx.ch_billing_amt);

	if(len_billing_amount == 0)
	{
		billing_amount = atoi(Auth_Tx.TLF01_details.total_amount);
		len_billing_amount = strlen(Auth_Tx.TLF01_details.total_amount);
		strncpy(amount_digit,Auth_Tx.TLF01_details.total_amount,10);
		strncpy(last2_decimal,Auth_Tx.TLF01_details.total_amount+10,2);
		/*billing_amount = (strtod((char *)amount_digit,0));*/
		billing_amount = (strtod(Auth_Tx.TLF01_details.total_amount, 0));
	}
	else
	{
		strncpy(amount_digit,Auth_Tx.ch_billing_amt,10);
		strncpy(last2_decimal,Auth_Tx.ch_billing_amt+10,2);
		/*billing_amount = (strtod(amount_digit,0));*/
		billing_amount = (strtod(Auth_Tx.ch_billing_amt, 0));
	}

	if((0 != strncmp(Auth_Tx.TLF01_details.handler_queue, "dcp", 3))&&
	   (0 != strncmp(Auth_Tx.TLF01_details.handler_queue, "VOICE",5)))
	{
		/*if ((0 != strncmp(Auth_Tx.TLF01_details.currency_code,"608",3)) &&
			(0 != strncmp(Auth_Tx.TLF01_details.currency_code,"840",3)))*/

		if(strcmp(Auth_Tx.ch_billing_curr_code,"608") == 0)
		{
			strncpy(Cr_Fld.TRANSACTION_AMT,amount_digit,10);
			strncat(Cr_Fld.TRANSACTION_AMT,".",1);
			strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);

		}
		else if((strcmp(Auth_Tx.ch_billing_curr_code,"840") == 0)||
				(0 != strncmp(Auth_Tx.ch_billing_curr_code,"608",3)))
		{
			if(len_billing_amount > 0 && billing_amount > 0)/* DE6 and DE51 is avialbe .Use them , Since DE6 and 51 are in PESO values*/
			{
				temp_conv_amount = billing_amount *conv_rate;
				Convert_Amt_To_Auth_Tx_String_Format(temp_conv_amount, str_temp_amount) ;
				LeftFill_String_With_Character (sizeof(Auth_Tx.TLF01_details.total_amount)-1, str_temp_amount,'0') ;
				memset(last2_decimal,0x00,sizeof(last2_decimal));
				strncpy(last2_decimal,str_temp_amount+10,2);

				strncpy(Cr_Fld.TRANSACTION_AMT,str_temp_amount,10);
				strncat(Cr_Fld.TRANSACTION_AMT,".",1);
				strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);
			}
			else
			{
				strncpy(Cr_Fld.TRANSACTION_AMT,"0000000000.00",13);
			}
		}


		else
		{
			strncpy(Cr_Fld.TRANSACTION_AMT,"0000000000.00",13);
		}
	}
	else if(strcmp(Auth_Tx.TLF01_details.currency_code,"608") == 0)
	{
		strncpy(Cr_Fld.TRANSACTION_AMT,amount_digit,10);
		strncat(Cr_Fld.TRANSACTION_AMT,".",1);
		strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);

	}
	else if(strcmp(Auth_Tx.TLF01_details.currency_code,"840") == 0)
	{
		if(len_billing_amount > 0 && billing_amount > 0)/* DE6 and DE51 is avialbe .Use them , Since DE6 and 51 are in PESO values*/
		{
			temp_conv_amount = billing_amount *conv_rate;
    		Convert_Amt_To_Auth_Tx_String_Format(temp_conv_amount, str_temp_amount) ;
			LeftFill_String_With_Character (sizeof(Auth_Tx.TLF01_details.total_amount)-1, str_temp_amount,'0') ;
			memset(last2_decimal,0x00,sizeof(last2_decimal));
			strncpy(last2_decimal,str_temp_amount+10,2);

			strncpy(Cr_Fld.TRANSACTION_AMT,str_temp_amount,10);
    		strncat(Cr_Fld.TRANSACTION_AMT,".",1);
    		strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);
		}
		else
		{
			strncpy(Cr_Fld.TRANSACTION_AMT,"0000000000.00",13);
		}
	}
	else
	{
		strncpy(Cr_Fld.TRANSACTION_AMT,"0000000000.00",13);
	}

}

void Falcon_build_CRTRAN_field_31_VISA()
{

	len_billing_amount = strlen(Auth_Tx.ch_billing_amt);

	if(len_billing_amount == 0)
	{
		billing_amount = atoi(Auth_Tx.TLF01_details.total_amount);
		len_billing_amount = strlen(Auth_Tx.TLF01_details.total_amount);
		strncpy(amount_digit,Auth_Tx.TLF01_details.total_amount,10);
		strncpy(last2_decimal,Auth_Tx.TLF01_details.total_amount+10,2);
				/*billing_amount = (strtod((char *)amount_digit,0));*/
		billing_amount = (strtod(Auth_Tx.TLF01_details.total_amount, 0));
	}
	else
	{
		strncpy(amount_digit,Auth_Tx.ch_billing_amt,10);
		strncpy(last2_decimal,Auth_Tx.ch_billing_amt+10,2);
		/*billing_amount = (strtod(amount_digit,0));*/
		billing_amount = (strtod(Auth_Tx.ch_billing_amt, 0));
	}

	if((0 != strncmp(Auth_Tx.TLF01_details.handler_queue, "dcp", 3))&&
	   (0 != strncmp(Auth_Tx.TLF01_details.handler_queue, "VOICE",5)))
	{
		if ((0 != strncmp(Auth_Tx.TLF01_details.currency_code,"608",3)) &&
			(0 != strncmp(Auth_Tx.TLF01_details.currency_code,"840",3)))
		{
			temp_conv_amount = billing_amount *conv_rate;
			Convert_Amt_To_Auth_Tx_String_Format(temp_conv_amount, str_temp_amount) ;
			LeftFill_String_With_Character (sizeof(Auth_Tx.TLF01_details.total_amount)-1, str_temp_amount,'0') ;
			memset(last2_decimal,0x00,sizeof(last2_decimal));
			strncpy(last2_decimal,str_temp_amount+10,2);

			strncpy(Cr_Fld.TRANSACTION_AMT,str_temp_amount,10);
			strncat(Cr_Fld.TRANSACTION_AMT,".",1);
			strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);

		}

		else if(strcmp(Auth_Tx.TLF01_details.currency_code,"608") == 0)
		{
			if(0==strncmp(Auth_Tx.ch_billing_curr_code,USD,CURRENCY_LENGTH))
			{
				temp_conv_amount = billing_amount *conv_rate;
				Convert_Amt_To_Auth_Tx_String_Format(temp_conv_amount, str_temp_amount) ;
				LeftFill_String_With_Character (sizeof(Auth_Tx.TLF01_details.total_amount)-1, str_temp_amount,'0') ;
				memset(last2_decimal,0x00,sizeof(last2_decimal));
				strncpy(last2_decimal,str_temp_amount+10,2);
				strncpy(Cr_Fld.TRANSACTION_AMT,str_temp_amount,10);
				strncat(Cr_Fld.TRANSACTION_AMT,".",1);
				strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);
			}
			else
			{
			strncpy(Cr_Fld.TRANSACTION_AMT,amount_digit,10);
			strncat(Cr_Fld.TRANSACTION_AMT,".",1);
			strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);
			}

		}
		else if(strcmp(Auth_Tx.TLF01_details.currency_code,"840") == 0)
		{
			if(0==strncmp(Auth_Tx.ch_billing_curr_code,USD,CURRENCY_LENGTH))
			{

				temp_conv_amount = billing_amount *conv_rate;
				Convert_Amt_To_Auth_Tx_String_Format(temp_conv_amount, str_temp_amount) ;
				LeftFill_String_With_Character (sizeof(Auth_Tx.TLF01_details.total_amount)-1, str_temp_amount,'0') ;
				memset(last2_decimal,0x00,sizeof(last2_decimal));
				strncpy(last2_decimal,str_temp_amount+10,2);

				strncpy(Cr_Fld.TRANSACTION_AMT,str_temp_amount,10);
				strncat(Cr_Fld.TRANSACTION_AMT,".",1);
				strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);
			}
			else
			{
				strncpy(Cr_Fld.TRANSACTION_AMT,amount_digit,10);
				strncat(Cr_Fld.TRANSACTION_AMT,".",1);
				strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);
			}
		}
		else
		{
			strncpy(Cr_Fld.TRANSACTION_AMT,"0000000000.00",13);
		}
	}
	else if(strcmp(Auth_Tx.TLF01_details.currency_code,"608") == 0)
	{
		strncpy(Cr_Fld.TRANSACTION_AMT,amount_digit,10);
		strncat(Cr_Fld.TRANSACTION_AMT,".",1);
		strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);

	}
	else if(strcmp(Auth_Tx.TLF01_details.currency_code,"840") == 0)
	{
		if(len_billing_amount > 0 && billing_amount > 0)/* DE6 and DE51 is avialbe .Use them , Since DE6 and 51 are in PESO values*/
		{
			temp_conv_amount = billing_amount *conv_rate;
    		Convert_Amt_To_Auth_Tx_String_Format(temp_conv_amount, str_temp_amount) ;
			LeftFill_String_With_Character (sizeof(Auth_Tx.TLF01_details.total_amount)-1, str_temp_amount,'0') ;
			memset(last2_decimal,0x00,sizeof(last2_decimal));
			strncpy(last2_decimal,str_temp_amount+10,2);

			strncpy(Cr_Fld.TRANSACTION_AMT,str_temp_amount,10);
    		strncat(Cr_Fld.TRANSACTION_AMT,".",1);
    		strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);
		}
		else
		{
			strncpy(Cr_Fld.TRANSACTION_AMT,"0000000000.00",13);
		}
	}
	else
	{
		strncpy(Cr_Fld.TRANSACTION_AMT,"0000000000.00",13);
	}

}

void Falcon_build_CRTRAN_field_31_JCB()
{
	len_billing_amount = strlen(Auth_Tx.ch_billing_amt);

	if(len_billing_amount == 0)
	{
		billing_amount = atoi(Auth_Tx.TLF01_details.total_amount);
		len_billing_amount = strlen(Auth_Tx.TLF01_details.total_amount);
		strncpy(amount_digit,Auth_Tx.TLF01_details.total_amount,10);
		strncpy(last2_decimal,Auth_Tx.TLF01_details.total_amount+10,2);
		/*billing_amount = (strtod((char *)amount_digit,0));*/
		billing_amount = (strtod(Auth_Tx.TLF01_details.total_amount, 0));
	}
	else
	{
		strncpy(amount_digit,Auth_Tx.ch_billing_amt,10);
		strncpy(last2_decimal,Auth_Tx.ch_billing_amt+10,2);
		/*billing_amount = (strtod(amount_digit,0));*/
		billing_amount = (strtod(Auth_Tx.ch_billing_amt, 0));
	}

	if((0 != strncmp(Auth_Tx.TLF01_details.handler_queue, "dcp", 3))&&
	   (0 != strncmp(Auth_Tx.TLF01_details.handler_queue, "VOICE",5)))
	{
		if ((0 != strncmp(Auth_Tx.TLF01_details.currency_code,"608",3)) &&
			(0 != strncmp(Auth_Tx.TLF01_details.currency_code,"840",3)))
		{
			temp_conv_amount = billing_amount *conv_rate;
			Convert_Amt_To_Auth_Tx_String_Format(temp_conv_amount, str_temp_amount) ;
			LeftFill_String_With_Character (sizeof(Auth_Tx.TLF01_details.total_amount)-1, str_temp_amount,'0') ;
			memset(last2_decimal,0x00,sizeof(last2_decimal));
			strncpy(last2_decimal,str_temp_amount+10,2);

			strncpy(Cr_Fld.TRANSACTION_AMT,str_temp_amount,10);
			strncat(Cr_Fld.TRANSACTION_AMT,".",1);
			strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);

		}
		else if(strcmp(Auth_Tx.TLF01_details.currency_code,"608") == 0)
		{
			if(0==strncmp(Auth_Tx.ch_billing_curr_code,USD,CURRENCY_LENGTH))
			{
				temp_conv_amount = billing_amount *conv_rate;
				Convert_Amt_To_Auth_Tx_String_Format(temp_conv_amount, str_temp_amount) ;
				LeftFill_String_With_Character (sizeof(Auth_Tx.TLF01_details.total_amount)-1, str_temp_amount,'0') ;
				memset(last2_decimal,0x00,sizeof(last2_decimal));
				strncpy(last2_decimal,str_temp_amount+10,2);
				strncpy(Cr_Fld.TRANSACTION_AMT,str_temp_amount,10);
				strncat(Cr_Fld.TRANSACTION_AMT,".",1);
				strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);
			}
			else
			{
			strncpy(Cr_Fld.TRANSACTION_AMT,amount_digit,10);
			strncat(Cr_Fld.TRANSACTION_AMT,".",1);
			strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);
			}
		}
		else if(strcmp(Auth_Tx.TLF01_details.currency_code,"840") == 0)
		{
			if(0==strncmp(Auth_Tx.ch_billing_curr_code,USD,CURRENCY_LENGTH))
			{
				temp_conv_amount = billing_amount *conv_rate;
				Convert_Amt_To_Auth_Tx_String_Format(temp_conv_amount, str_temp_amount) ;
				LeftFill_String_With_Character (sizeof(Auth_Tx.TLF01_details.total_amount)-1, str_temp_amount,'0') ;
				memset(last2_decimal,0x00,sizeof(last2_decimal));
				strncpy(last2_decimal,str_temp_amount+10,2);
				strncpy(Cr_Fld.TRANSACTION_AMT,str_temp_amount,10);
				strncat(Cr_Fld.TRANSACTION_AMT,".",1);
				strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);
			}
			else
			{
				strncpy(Cr_Fld.TRANSACTION_AMT,amount_digit,10);
				strncat(Cr_Fld.TRANSACTION_AMT,".",1);
				strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);
			}
		}
		else
		{
			strncpy(Cr_Fld.TRANSACTION_AMT,"0000000000.00",13);
		}
	}
	else if(strcmp(Auth_Tx.TLF01_details.currency_code,"608") == 0)
	{
		strncpy(Cr_Fld.TRANSACTION_AMT,amount_digit,10);
		strncat(Cr_Fld.TRANSACTION_AMT,".",1);
		strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);

	}
	else if(strcmp(Auth_Tx.TLF01_details.currency_code,"840") == 0)
	{
		if(len_billing_amount > 0 && billing_amount > 0)/* DE6 and DE51 is avialbe .Use them , Since DE6 and 51 are in PESO values*/
		{
			temp_conv_amount = billing_amount *conv_rate;
    		Convert_Amt_To_Auth_Tx_String_Format(temp_conv_amount, str_temp_amount) ;
			LeftFill_String_With_Character (sizeof(Auth_Tx.TLF01_details.total_amount)-1, str_temp_amount,'0') ;
			memset(last2_decimal,0x00,sizeof(last2_decimal));
			strncpy(last2_decimal,str_temp_amount+10,2);

			strncpy(Cr_Fld.TRANSACTION_AMT,str_temp_amount,10);
    		strncat(Cr_Fld.TRANSACTION_AMT,".",1);
    		strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);
		}
		else
		{
			strncpy(Cr_Fld.TRANSACTION_AMT,"0000000000.00",13);
		}
	}
	else
	{
		strncpy(Cr_Fld.TRANSACTION_AMT,"0000000000.00",13);
	}

}

void Falcon_build_CRTRAN_field_31_CUP()
{
	len_billing_amount = strlen(Auth_Tx.ch_billing_amt);

	if(len_billing_amount == 0)
	{
		billing_amount = atoi(Auth_Tx.TLF01_details.total_amount);
		len_billing_amount = strlen(Auth_Tx.TLF01_details.total_amount);
		strncpy(amount_digit,Auth_Tx.TLF01_details.total_amount,10);
		strncpy(last2_decimal,Auth_Tx.TLF01_details.total_amount+10,2);
		/*billing_amount = (strtod((char *)amount_digit,0));*/
		billing_amount = (strtod(Auth_Tx.TLF01_details.total_amount, 0));
	}
	else
	{
		strncpy(amount_digit,Auth_Tx.ch_billing_amt,10);
		strncpy(last2_decimal,Auth_Tx.ch_billing_amt+10,2);
		/*billing_amount = (strtod(amount_digit,0));*/
		billing_amount = (strtod(Auth_Tx.ch_billing_amt, 0));
	}

	len_billing_currency_code = strlen(Auth_Tx.ch_billing_curr_code);
	if((0 != strncmp(Auth_Tx.TLF01_details.handler_queue, "dcp", 3))&&
	   (0 != strncmp(Auth_Tx.TLF01_details.handler_queue, "VOICE",5)))
	{

			if(strcmp(Auth_Tx.ch_billing_curr_code,"608") == 0)
			{
				strncpy(Cr_Fld.TRANSACTION_AMT,amount_digit,10);
				strncat(Cr_Fld.TRANSACTION_AMT,".",1);
				strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);

			}
			else if((len_billing_currency_code > 0)&&
					((strcmp(Auth_Tx.ch_billing_curr_code,"840") == 0)||
					(0 != strncmp(Auth_Tx.ch_billing_curr_code,"608",3))))
			{
				if(len_billing_amount > 0 && billing_amount > 0)/* DE6 and DE51 is avialbe .Use them , Since DE6 and 51 are in PESO values*/
		{
			temp_conv_amount = billing_amount *conv_rate;
			Convert_Amt_To_Auth_Tx_String_Format(temp_conv_amount, str_temp_amount) ;
			LeftFill_String_With_Character (sizeof(Auth_Tx.TLF01_details.total_amount)-1, str_temp_amount,'0') ;
			memset(last2_decimal,0x00,sizeof(last2_decimal));
			strncpy(last2_decimal,str_temp_amount+10,2);

			strncpy(Cr_Fld.TRANSACTION_AMT,str_temp_amount,10);
			strncat(Cr_Fld.TRANSACTION_AMT,".",1);
			strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);
				}
				else
				{
					strncpy(Cr_Fld.TRANSACTION_AMT,"0000000000.00",13);
				}
			}

			else if (len_billing_currency_code == 0)
			{
				if(strncmp(Auth_Tx.TLF01_details.currency_code,"608",3) == 0)
		{
			strncpy(Cr_Fld.TRANSACTION_AMT,amount_digit,10);
			strncat(Cr_Fld.TRANSACTION_AMT,".",1);
			strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);
		}
				else if ((strcmp(Auth_Tx.TLF01_details.currency_code,"840") == 0)||
						(0 != strncmp(Auth_Tx.TLF01_details.currency_code,"608",3)))
			{
				temp_conv_amount = billing_amount *conv_rate;
				Convert_Amt_To_Auth_Tx_String_Format(temp_conv_amount, str_temp_amount) ;
				LeftFill_String_With_Character (sizeof(Auth_Tx.TLF01_details.total_amount)-1, str_temp_amount,'0') ;
				memset(last2_decimal,0x00,sizeof(last2_decimal));
				strncpy(last2_decimal,str_temp_amount+10,2);

				strncpy(Cr_Fld.TRANSACTION_AMT,str_temp_amount,10);
				strncat(Cr_Fld.TRANSACTION_AMT,".",1);
				strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);
			}
		}

		else
		{
			strncpy(Cr_Fld.TRANSACTION_AMT,"0000000000.00",13);
		}
	}
	else if(strcmp(Auth_Tx.TLF01_details.currency_code,"608") == 0)
	{
		strncpy(Cr_Fld.TRANSACTION_AMT,amount_digit,10);
		strncat(Cr_Fld.TRANSACTION_AMT,".",1);
		strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);

	}
		else if ((strcmp(Auth_Tx.TLF01_details.currency_code,"840") == 0)||
				(0 != strncmp(Auth_Tx.TLF01_details.currency_code,"608",3)))
	{
		if(len_billing_amount > 0 && billing_amount > 0)/* DE6 and DE51 is avialbe .Use them , Since DE6 and 51 are in PESO values*/
		{
			temp_conv_amount = billing_amount *conv_rate;
    		Convert_Amt_To_Auth_Tx_String_Format(temp_conv_amount, str_temp_amount) ;
			LeftFill_String_With_Character (sizeof(Auth_Tx.TLF01_details.total_amount)-1, str_temp_amount,'0') ;
			memset(last2_decimal,0x00,sizeof(last2_decimal));
			strncpy(last2_decimal,str_temp_amount+10,2);

			strncpy(Cr_Fld.TRANSACTION_AMT,str_temp_amount,10);
    		strncat(Cr_Fld.TRANSACTION_AMT,".",1);
    		strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);
		}
		else
		{
			strncpy(Cr_Fld.TRANSACTION_AMT,"0000000000.00",13);
		}
	}
	else
	{
		strncpy(Cr_Fld.TRANSACTION_AMT,"0000000000.00",13);
	}


}

void Falcon_build_CRTRAN_field_31_DINERS()
{
	len_billing_amount = strlen(Auth_Tx.ch_billing_amt);

	if(len_billing_amount == 0)
	{
		billing_amount = atoi(Auth_Tx.TLF01_details.total_amount);
		len_billing_amount = strlen(Auth_Tx.TLF01_details.total_amount);
		strncpy(amount_digit,Auth_Tx.TLF01_details.total_amount,10);
		strncpy(last2_decimal,Auth_Tx.TLF01_details.total_amount+10,2);
		/*billing_amount = (strtod((char *)amount_digit,0));*/
		billing_amount = (strtod(Auth_Tx.TLF01_details.total_amount, 0));
	}
	else
	{
		strncpy(amount_digit,Auth_Tx.ch_billing_amt,10);
		strncpy(last2_decimal,Auth_Tx.ch_billing_amt+10,2);
		/*billing_amount = (strtod(amount_digit,0));*/
		billing_amount = (strtod(Auth_Tx.ch_billing_amt, 0));
	}

	if((0 != strncmp(Auth_Tx.TLF01_details.handler_queue, "dcp", 3))&&
	   (0 != strncmp(Auth_Tx.TLF01_details.handler_queue, "VOICE",5)))
	{

		if(strcmp(Auth_Tx.ch_billing_curr_code,"608") == 0)
		{
			strncpy(Cr_Fld.TRANSACTION_AMT,amount_digit,10);
			strncat(Cr_Fld.TRANSACTION_AMT,".",1);
			strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);

		}
		else if((strcmp(Auth_Tx.ch_billing_curr_code,"840") == 0)||
				(0 != strncmp(Auth_Tx.ch_billing_curr_code,"608",3)))
		{
			if(len_billing_amount > 0 && billing_amount > 0)/* DE6 and DE51 is avialbe .Use them , Since DE6 and 51 are in PESO values*/
			{
				temp_conv_amount = billing_amount *conv_rate;
				Convert_Amt_To_Auth_Tx_String_Format(temp_conv_amount, str_temp_amount) ;
				LeftFill_String_With_Character (sizeof(Auth_Tx.TLF01_details.total_amount)-1, str_temp_amount,'0') ;
				memset(last2_decimal,0x00,sizeof(last2_decimal));
				strncpy(last2_decimal,str_temp_amount+10,2);

				strncpy(Cr_Fld.TRANSACTION_AMT,str_temp_amount,10);
				strncat(Cr_Fld.TRANSACTION_AMT,".",1);
				strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);
			}
			else
			{
				strncpy(Cr_Fld.TRANSACTION_AMT,"0000000000.00",13);
			}
		}


		else
		{
			strncpy(Cr_Fld.TRANSACTION_AMT,"0000000000.00",13);
		}
	}

	else if(strcmp(Auth_Tx.TLF01_details.currency_code,"608") == 0)
	{
		strncpy(Cr_Fld.TRANSACTION_AMT,amount_digit,10);
		strncat(Cr_Fld.TRANSACTION_AMT,".",1);
		strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);

	}
	else if(strcmp(Auth_Tx.TLF01_details.currency_code,"840") == 0)
	{
		if(len_billing_amount > 0 && billing_amount > 0)/* DE6 and DE51 is avialbe .Use them , Since DE6 and 51 are in PESO values*/
		{
			temp_conv_amount = billing_amount *conv_rate;
    		Convert_Amt_To_Auth_Tx_String_Format(temp_conv_amount, str_temp_amount) ;
			LeftFill_String_With_Character (sizeof(Auth_Tx.TLF01_details.total_amount)-1, str_temp_amount,'0') ;
			memset(last2_decimal,0x00,sizeof(last2_decimal));
			strncpy(last2_decimal,str_temp_amount+10,2);

			strncpy(Cr_Fld.TRANSACTION_AMT,str_temp_amount,10);
    		strncat(Cr_Fld.TRANSACTION_AMT,".",1);
    		strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);
		}
		else
		{
			strncpy(Cr_Fld.TRANSACTION_AMT,"0000000000.00",13);
		}
	}
	else
	{
		strncpy(Cr_Fld.TRANSACTION_AMT,"0000000000.00",13);
	}

}

void Falcon_build_CRTRAN_field_31_AMEX()
{
	memset(Cr_Fld.TRANSACTION_AMT,0x00,sizeof(Cr_Fld.TRANSACTION_AMT));
	if((0 != strncmp(Auth_Tx.TLF01_details.handler_queue, "dcp", 3))&&
	   (0 != strncmp(Auth_Tx.TLF01_details.handler_queue, "VOICE",5)))
	{

			len_settle_currency_code = strlen(Auth_Tx.settlement_curr_code);

			if(len_settle_currency_code == 0)
			{
				if(strcmp(Auth_Tx.TLF01_details.currency_code,"608") == 0)
				{
					strncpy(Cr_Fld.TRANSACTION_AMT,amount_digit,10);
					strncat(Cr_Fld.TRANSACTION_AMT,".",1);
					strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);

				}
				else if((strcmp(Auth_Tx.TLF01_details.currency_code,"840") == 0)||
						(0 != strncmp(Auth_Tx.TLF01_details.currency_code,"608",3)))
					{
						if(len_billing_amount > 0 && billing_amount > 0)/* DE6 and DE51 is avialbe .Use them , Since DE6 and 51 are in PESO values*/
		{
			temp_conv_amount = billing_amount *conv_rate;
			Convert_Amt_To_Auth_Tx_String_Format(temp_conv_amount, str_temp_amount) ;
			LeftFill_String_With_Character (sizeof(Auth_Tx.TLF01_details.total_amount)-1, str_temp_amount,'0') ;
			memset(last2_decimal,0x00,sizeof(last2_decimal));
			strncpy(last2_decimal,str_temp_amount+10,2);

			strncpy(Cr_Fld.TRANSACTION_AMT,str_temp_amount,10);
			strncat(Cr_Fld.TRANSACTION_AMT,".",1);
			strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);
						}
						else
						{
							strncpy(Cr_Fld.TRANSACTION_AMT,"0000000000.00",13);
						}
					}
			}

			else if(strcmp(Auth_Tx.settlement_curr_code,"608") == 0)
		{
			strncpy(Cr_Fld.TRANSACTION_AMT,amount_digit,10);
			strncat(Cr_Fld.TRANSACTION_AMT,".",1);
			strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);

		}
			else if((strcmp(Auth_Tx.settlement_curr_code,"840") == 0)||
					(0 != strncmp(Auth_Tx.settlement_curr_code,"608",3)))
		{
			if(len_billing_amount > 0 && billing_amount > 0)/* DE6 and DE51 is avialbe .Use them , Since DE6 and 51 are in PESO values*/
			{
				temp_conv_amount = billing_amount *conv_rate;
				Convert_Amt_To_Auth_Tx_String_Format(temp_conv_amount, str_temp_amount) ;
				LeftFill_String_With_Character (sizeof(Auth_Tx.TLF01_details.total_amount)-1, str_temp_amount,'0') ;
				memset(last2_decimal,0x00,sizeof(last2_decimal));
				strncpy(last2_decimal,str_temp_amount+10,2);

				strncpy(Cr_Fld.TRANSACTION_AMT,str_temp_amount,10);
				strncat(Cr_Fld.TRANSACTION_AMT,".",1);
				strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);
			}
			else
			{
				strncpy(Cr_Fld.TRANSACTION_AMT,"0000000000.00",13);
			}
		}


		else
		{
			strncpy(Cr_Fld.TRANSACTION_AMT,"0000000000.00",13);
		}
	}

	else if(strcmp(Auth_Tx.TLF01_details.currency_code,"608") == 0)
	{
		strncpy(Cr_Fld.TRANSACTION_AMT,amount_digit,10);
		strncat(Cr_Fld.TRANSACTION_AMT,".",1);
		strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);

	}
	else if(strcmp(Auth_Tx.TLF01_details.currency_code,"840") == 0)
	{
		if(len_billing_amount > 0 || billing_amount > 0)/* DE6 and DE51 is avialbe .Use them , Since DE6 and 51 are in PESO values*/
		{
			temp_conv_amount = billing_amount *conv_rate;
    		Convert_Amt_To_Auth_Tx_String_Format(temp_conv_amount, str_temp_amount) ;
			LeftFill_String_With_Character (sizeof(Auth_Tx.TLF01_details.total_amount)-1, str_temp_amount,'0') ;
			memset(last2_decimal,0x00,sizeof(last2_decimal));
			strncpy(last2_decimal,str_temp_amount+10,2);

			strncpy(Cr_Fld.TRANSACTION_AMT,str_temp_amount,10);
    		strncat(Cr_Fld.TRANSACTION_AMT,".",1);
    		strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);
		}
		else
		{
			strncpy(Cr_Fld.TRANSACTION_AMT,"0000000000.00",13);
		}
	}
	else
	{
		strncpy(Cr_Fld.TRANSACTION_AMT,"0000000000.00",13);
	}


}



void Falcon_build_CRTRAN_field_31_Fleet()
{
	len_billing_amount = strlen(Auth_Tx.ch_billing_amt);

	if(len_billing_amount == 0)
	{
		billing_amount = atoi(Auth_Tx.TLF01_details.total_amount);
		len_billing_amount = strlen(Auth_Tx.TLF01_details.total_amount);
		strncpy(amount_digit,Auth_Tx.TLF01_details.total_amount,10);
		strncpy(last2_decimal,Auth_Tx.TLF01_details.total_amount+10,2);
		/*billing_amount = (strtod((char *)amount_digit,0));*/
		billing_amount = (strtod(Auth_Tx.TLF01_details.total_amount, 0));
	}
	else
	{
		strncpy(amount_digit,Auth_Tx.ch_billing_amt,10);
		strncpy(last2_decimal,Auth_Tx.ch_billing_amt+10,2);
		/*billing_amount = (strtod(amount_digit,0));*/
		billing_amount = (strtod(Auth_Tx.ch_billing_amt, 0));
	}

	if((0 != strncmp(Auth_Tx.TLF01_details.handler_queue, "dcp", 3))&&
	   (0 != strncmp(Auth_Tx.TLF01_details.handler_queue, "VOICE",5)))
	{
		if ((0 != strncmp(Auth_Tx.TLF01_details.currency_code,"608",3)) &&
			(0 != strncmp(Auth_Tx.TLF01_details.currency_code,"840",3)))
		{
			temp_conv_amount = billing_amount *conv_rate;
			Convert_Amt_To_Auth_Tx_String_Format(temp_conv_amount, str_temp_amount) ;
			LeftFill_String_With_Character (sizeof(Auth_Tx.TLF01_details.total_amount)-1, str_temp_amount,'0') ;
			memset(last2_decimal,0x00,sizeof(last2_decimal));
			strncpy(last2_decimal,str_temp_amount+10,2);

			strncpy(Cr_Fld.TRANSACTION_AMT,str_temp_amount,10);
			strncat(Cr_Fld.TRANSACTION_AMT,".",1);
			strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);

		}
		else if(strcmp(Auth_Tx.TLF01_details.currency_code,"608") == 0)
		{
			strncpy(Cr_Fld.TRANSACTION_AMT,amount_digit,10);
			strncat(Cr_Fld.TRANSACTION_AMT,".",1);
			strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);

		}
		else if(strcmp(Auth_Tx.TLF01_details.currency_code,"840") == 0)
		{
			if(len_billing_amount > 0 && billing_amount > 0)/* DE6 and DE51 is avialbe .Use them , Since DE6 and 51 are in PESO values*/
			{
				temp_conv_amount = billing_amount *conv_rate;
				Convert_Amt_To_Auth_Tx_String_Format(temp_conv_amount, str_temp_amount) ;
				LeftFill_String_With_Character (sizeof(Auth_Tx.TLF01_details.total_amount)-1, str_temp_amount,'0') ;
				memset(last2_decimal,0x00,sizeof(last2_decimal));
				strncpy(last2_decimal,str_temp_amount+10,2);

				strncpy(Cr_Fld.TRANSACTION_AMT,str_temp_amount,10);
				strncat(Cr_Fld.TRANSACTION_AMT,".",1);
				strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);
			}
			else
			{
				strncpy(Cr_Fld.TRANSACTION_AMT,"0000000000.00",13);
			}
		}
		else
		{
			strncpy(Cr_Fld.TRANSACTION_AMT,"0000000000.00",13);
		}
	}
	else if(strcmp(Auth_Tx.TLF01_details.currency_code,"608") == 0)
	{
		strncpy(Cr_Fld.TRANSACTION_AMT,amount_digit,10);
		strncat(Cr_Fld.TRANSACTION_AMT,".",1);
		strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);

	}
	else if(strcmp(Auth_Tx.TLF01_details.currency_code,"840") == 0)
	{
		if(len_billing_amount > 0 && billing_amount > 0)/* DE6 and DE51 is avialbe .Use them , Since DE6 and 51 are in PESO values*/
		{
			temp_conv_amount = billing_amount *conv_rate;
    		Convert_Amt_To_Auth_Tx_String_Format(temp_conv_amount, str_temp_amount) ;
			LeftFill_String_With_Character (sizeof(Auth_Tx.TLF01_details.total_amount)-1, str_temp_amount,'0') ;
			memset(last2_decimal,0x00,sizeof(last2_decimal));
			strncpy(last2_decimal,str_temp_amount+10,2);

			strncpy(Cr_Fld.TRANSACTION_AMT,str_temp_amount,10);
    		strncat(Cr_Fld.TRANSACTION_AMT,".",1);
    		strncat(Cr_Fld.TRANSACTION_AMT,last2_decimal,2);
		}
		else
		{
			strncpy(Cr_Fld.TRANSACTION_AMT,"0000000000.00",13);
		}
	}
	else
	{
		strncpy(Cr_Fld.TRANSACTION_AMT,"0000000000.00",13);
	}

}

INT FALCON_read_CUP_MP_Bin_range_len_1( void )
{
   DWORD rc;
   CHAR filename     [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstr       [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstring    [MAX_INI_INFO_SIZE]= {0};
   CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
   INT  ret_val = true;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
   rc = GetPrivateProfileString(
             "CUPMPBINDEFINATION",     /* points to section name       */
             "CUP_MP_BIN_RANGE_LEN_1",  /* points to key name           */
             "0",                       		/* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   CUP_MP_Bin_range_len_1= atoi(tmpstring);
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         FALCON_read_CUP_MP_Bin_range_len_2
 *
 *  DESCRIPTION:  This function reads information from the tf ini file.
 *
 *  INPUTS:       None
 *  OUTPUTS:      err_buf - Text message explaining why this function failed
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT FALCON_read_CUP_MP_Bin_range_len_2( void )
{
   DWORD rc;
   CHAR filename     [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstr       [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstring    [MAX_INI_INFO_SIZE]= {0};
   CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
   INT  ret_val = true;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
   rc = GetPrivateProfileString(
             "CUPMPBINDEFINATION",     /* points to section name       */
             "CUP_MP_BIN_RANGE_LEN_2",  /* points to key name           */
             "0",                       		/* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   CUP_MP_Bin_range_len_2= atoi(tmpstring);
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         FALCON_read_CUP_MP_Bin_range_len_3
 *
 *  DESCRIPTION:  This function reads information from the tf ini file.
 *
 *  INPUTS:       None
 *  OUTPUTS:      err_buf - Text message explaining why this function failed
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT FALCON_read_CUP_MP_Bin_range_len_3( void )
{
   DWORD rc;
   CHAR filename     [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstr       [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstring    [MAX_INI_INFO_SIZE]= {0};
   CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
   INT  ret_val = true;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
   rc = GetPrivateProfileString(
             "CUPMPBINDEFINATION",     /* points to section name       */
             "CUP_MP_BIN_RANGE_LEN_3",  /* points to key name           */
             "0",                       		/* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   CUP_MP_Bin_range_len_3= atoi(tmpstring);
   return( ret_val );
}
/******************************************************************************
 *
 *  NAME:         FALCON_read_CUP_MP_Bin_range_len_4
 *
 *  DESCRIPTION:  This function reads information from the tf ini file.
 *
 *  INPUTS:       None
 *  OUTPUTS:      err_buf - Text message explaining why this function failed
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT FALCON_read_CUP_MP_Bin_range_len_4( void )
{
   DWORD rc;
   CHAR filename     [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstr       [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstring    [MAX_INI_INFO_SIZE]= {0};
   CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
   INT  ret_val = true;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
   rc = GetPrivateProfileString(
             "CUPMPBINDEFINATION",     /* points to section name       */
             "CUP_MP_BIN_RANGE_LEN_4",  /* points to key name           */
             "0",                       		/* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   CUP_MP_Bin_range_len_4= atoi(tmpstring);
   return( ret_val );
}
/******************************************************************************
 *
 *  NAME:         FALCON_read_CUP_MP_Bin_range_len_5
 *
 *  DESCRIPTION:  This function reads information from the tf ini file.
 *
 *  INPUTS:       None
 *  OUTPUTS:      err_buf - Text message explaining why this function failed
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT FALCON_read_CUP_MP_Bin_range_len_5( void )
{
   DWORD rc;
   CHAR filename     [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstr       [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstring    [MAX_INI_INFO_SIZE]= {0};
   CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
   INT  ret_val = true;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
   rc = GetPrivateProfileString(
             "CUPMPBINDEFINATION",     /* points to section name       */
             "CUP_MP_BIN_RANGE_LEN_5",  /* points to key name           */
             "0",                       		/* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   CUP_MP_Bin_range_len_5= atoi(tmpstring);
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         FALCON_read_CUP_MP_Bin_range_1_indicator
 *
 *  DESCRIPTION:  This function reads information from the tf ini file.
 *
 *  INPUTS:       None
 *  OUTPUTS:      err_buf - Text message explaining why this function failed
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT FALCON_read_CUP_MP_Bin_range_1_indicator( void )
{
   DWORD rc;
   CHAR filename     [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstr       [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstring    [MAX_INI_INFO_SIZE]= {0};
   CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
   INT  ret_val = true;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
   rc = GetPrivateProfileString(
             "CUPMPBINDEFINATION",     /* points to section name       */
             "CUP_MP_BIN_RANGE_1",  /* points to key name           */
             "0",                       		/* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   strncpy( CUP_MP_Bin_range_1, tmpstring, CUP_MP_Bin_range_len_1 );
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         FALCON_read_CUP_MP_Bin_range_2_indicator
 *
 *  DESCRIPTION:  This function reads information from the tf ini file.
 *
 *  INPUTS:       None
 *  OUTPUTS:      err_buf - Text message explaining why this function failed
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT FALCON_read_CUP_MP_Bin_range_2_indicator( void )
{
   DWORD rc;
   CHAR filename     [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstr       [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstring    [MAX_INI_INFO_SIZE]= {0};
   CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
   INT  ret_val = true;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
   rc = GetPrivateProfileString(
             "CUPMPBINDEFINATION",     /* points to section name       */
             "CUP_MP_BIN_RANGE_2",  /* points to key name           */
             "0",                       		/* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   strncpy( CUP_MP_Bin_range_2, tmpstring, CUP_MP_Bin_range_len_2 );
   return( ret_val );
}
/******************************************************************************
 *
 *  NAME:         FALCON_read_CUP_MP_Bin_range_3_indicator
 *
 *  DESCRIPTION:  This function reads information from the tf ini file.
 *
 *  INPUTS:       None
 *  OUTPUTS:      err_buf - Text message explaining why this function failed
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT FALCON_read_CUP_MP_Bin_range_3_indicator( void )
{
   DWORD rc;
   CHAR filename     [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstr       [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstring    [MAX_INI_INFO_SIZE]= {0};
   CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
   INT  ret_val = true;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
   rc = GetPrivateProfileString(
             "CUPMPBINDEFINATION",     /* points to section name       */
             "CUP_MP_BIN_RANGE_3",  /* points to key name           */
             "0",                       		/* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   strncpy( CUP_MP_Bin_range_3, tmpstring, CUP_MP_Bin_range_len_3 );
   return( ret_val );
}
/******************************************************************************
 *
 *  NAME:         FALCON_read_CUP_MP_Bin_range_4_indicator
 *
 *  DESCRIPTION:  This function reads information from the tf ini file.
 *
 *  INPUTS:       None
 *  OUTPUTS:      err_buf - Text message explaining why this function failed
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT FALCON_read_CUP_MP_Bin_range_4_indicator( void )
{
   DWORD rc;
   CHAR filename     [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstr       [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstring    [MAX_INI_INFO_SIZE]= {0};
   CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
   INT  ret_val = true;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
   rc = GetPrivateProfileString(
             "CUPMPBINDEFINATION",     /* points to section name       */
             "CUP_MP_BIN_RANGE_4",  /* points to key name           */
             "0",                       		/* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   strncpy( CUP_MP_Bin_range_4, tmpstring, CUP_MP_Bin_range_len_4 );
   return( ret_val );
}
/******************************************************************************
 *
 *  NAME:         FALCON_read_CUP_MP_Bin_range_5_indicator
 *
 *  DESCRIPTION:  This function reads information from the tf ini file.
 *
 *  INPUTS:       None
 *  OUTPUTS:      err_buf - Text message explaining why this function failed
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT FALCON_read_CUP_MP_Bin_range_5_indicator( void )
{
   DWORD rc;
   CHAR filename     [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstr       [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstring    [MAX_INI_INFO_SIZE]= {0};
   CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
   INT  ret_val = true;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
   rc = GetPrivateProfileString(
             "CUPMPBINDEFINATION",     /* points to section name       */
             "CUP_MP_BIN_RANGE_5",  /* points to key name           */
             "0",                       		/* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );

   /* Convert entry from a string into a number. */
   memset(  tmpstring, 0x00, MAX_INI_INFO_SIZE );
   strncpy( tmpstring, ini_info, MAX_INI_INFO_SIZE-1 );
   strncpy( CUP_MP_Bin_range_5, tmpstring, CUP_MP_Bin_range_len_5 );
   return( ret_val );
}
/******************************************************************************
 *
 *  NAME:         FALCON_Read_BDO_PAY_QR_NII_value
 *
 *  DESCRIPTION:  This function reads the Network International Identifier value from
 *  			  the tf.ini file which will be used to identify whether an incoming
 *  			  transaction is a BDO Pay QR transaction or not.
 *
 *  INPUTS:       None
 *  OUTPUTS:      err_buf - Text message explaining why this function failed
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Akshara Hanumesh
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT FALCON_Read_BDO_PAY_QR_NII_value( void )
{
   DWORD rc;
   CHAR filename     [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstr       [MAX_FILENAME_SIZE]= {0};
   CHAR tmpstring    [MAX_INI_INFO_SIZE]= {0};
   CHAR ini_info     [MAX_INI_INFO_SIZE]= {0};
   INT  ret_val = true;

   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_FILENAME_SIZE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
   rc = GetPrivateProfileString(
             "FALCON",     /* points to section name       */
             "BDO_PAY_QR_NII_1",  /* points to key name           */
             "",                       		/* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );


   strncpy(BDO_PAY_QR_NII_1, ini_info, strlen(ini_info));

   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
   rc = GetPrivateProfileString(
             "FALCON",     /* points to section name       */
             "BDO_PAY_QR_NII_2",  /* points to key name           */
             "",                       		/* points to default string     */
             ini_info,                      /* points to destination buffer */
             MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
             filename                       /* points to ini filename       */
   );


   strncpy(BDO_PAY_QR_NII_2, ini_info, strlen(ini_info));

   memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
   rc = GetPrivateProfileString(
                "FALCON",     /* points to section name       */
                "BDO_PAY_QR_NII_3",  /* points to key name           */
                "",                       		/* points to default string     */
                ini_info,                      /* points to destination buffer */
                MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
                filename                       /* points to ini filename       */
    );


    strncpy(BDO_PAY_QR_NII_3, ini_info, strlen(ini_info));

    memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
    rc = GetPrivateProfileString(
                   "FALCON",     /* points to section name       */
                   "BDO_PAY_QR_NII_4",  /* points to key name           */
                   "",                       		/* points to default string     */
                   ini_info,                      /* points to destination buffer */
                   MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
                   filename                       /* points to ini filename       */
     );


     strncpy(BDO_PAY_QR_NII_4, ini_info, strlen(ini_info));

     memset( ini_info, 0x00, MAX_INI_INFO_SIZE );
     rc = GetPrivateProfileString(
                      "FALCON",     /* points to section name       */
                      "BDO_PAY_QR_NII_5",  /* points to key name           */
                      "",                       		/* points to default string     */
                      ini_info,                      /* points to destination buffer */
                      MAX_INI_INFO_SIZE - 1,         /* size of destination buffer   */
                      filename                       /* points to ini filename       */
      );


     strncpy(BDO_PAY_QR_NII_5, ini_info, strlen(ini_info));

   return( rc );
}

