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

CHAR strAppName [80];
CHAR ServiceName[80];
AUTH_TX     Auth_Tx;
char javaEndProcessSignalled[1];

INT Version =5;//TF_veena version Intial
static CHAR message[1023]= {0};
static long count = 0;
INT  MainProcessDone;
INT  EndProcessSignalled;
CHAR  date_str [ 9] = "";
CHAR  time_str [ 7] = "";
CHAR  time_date[25] = "";
CHAR  time_milliseconds[4] = "";
CHAR profile_str [17]="";

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

	//CRTRANS_FIELDS Cr_Fld;
	CHAR tem_date[2]={0};
	CHAR tem_month[3]={0};
	CHAR tem_year[4]={0};
	CHAR temp_exp[9]={0};
	UINT  item_month=0;
	UINT  item_year=0;
	UINT  icvc_data=0;
	char dollar_peso[8]="0.023889";
	char temp_totalamt[14]={0};
	 double   conv_rate   = 0.0;
	pBYTE pATEMessageDataData = 0 ;
	pPTE_MSG_DATA pATEMessageData = 0 ;
	CHAR resp_buf[15] = {0} ;
	memset (&Auth_Tx, 0, sizeof (AUTH_TX)) ;
	memset(&Cr_Fld,0,sizeof(Cr_Fld));
	memset(&message,0,sizeof(message));

	int i =0;
	int len=0;
	char msg[5]={0};
	char temp_iso_ration[25] = {0};
	char temp_iso_ration_value[13] = {0};
	double temp_iso = 0.0;double temp_conv_amount;
	CRF01  crf01 = {0} ;  LONG  lReturnCode;
	char temp_without_decimal[13]="";
	char temp_Tran_id[33]={0};
	static INT   falcon_tx_counter=0;

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

	memcpy(Cr_Fld.HeaderFile,"000009710000100000102tdriver   PMAX      0000000000                  ",69);
	sprintf(message,"%-69s\n",Cr_Fld.HeaderFile);
	// field 1
	memcpy(Cr_Fld.WORKFLOW_XCD,profile_str,16);
	sprintf(&message[69],"%-16s\n",Cr_Fld.WORKFLOW_XCD);
	memcpy(Cr_Fld.RECORD_TYPE_XCD,"CRTRAN24",8);
	sprintf(&message[85],"%-8s\n",Cr_Fld.RECORD_TYPE_XCD);
	memcpy(Cr_Fld.DATA_SPECIFICATION_VERSION_XCD,"2.4",5);
	sprintf(&message[93],"%-5s\n",Cr_Fld.DATA_SPECIFICATION_VERSION_XCD);
	memcpy(Cr_Fld.CLIENT_XID,"DT_BDO_CR",16);
	sprintf(&message[98],"%-16s\n",Cr_Fld.CLIENT_XID);
	memcpy(Cr_Fld.RCD_CRT_DATE,date_str,8);
	sprintf(&message[114],"%-8s\n",Cr_Fld.RCD_CRT_DATE);
	memcpy(Cr_Fld.RCD_CRT_TIME,time_str,6);
	sprintf(&message[122],"%-6s\n",Cr_Fld.RCD_CRT_TIME);
	memcpy(Cr_Fld.RCD_CRT_MSCND,time_milliseconds,3);
	sprintf(&message[128],"%-3s\n",Cr_Fld.RCD_CRT_MSCND);
	memcpy(Cr_Fld.GMT_OFFSET,"8.00",6);
	sprintf(&message[131],"%-6s\n",Cr_Fld.GMT_OFFSET);

	memcpy(Cr_Fld.CUSTOMER_XID,Auth_Tx.TLF01_details.card_num,20);
	sprintf(&message[137],"%-20s\n",Cr_Fld.CUSTOMER_XID);
	memcpy(Cr_Fld.ACCOUNT_REFERENCE_XID,Auth_Tx.TLF01_details.card_num,19);
	sprintf(&message[157],"%-40s\n",Cr_Fld.ACCOUNT_REFERENCE_XID);
	memset(temp_Tran_id,' ',sizeof(temp_Tran_id)-1);
	sprintf(temp_Tran_id,"%-20s-CRT-%07d",Auth_Tx.TLF01_details.primary_key.transaction_id,falcon_tx_counter++);
	//strncpy(temp_Tran_id,Auth_Tx.TLF01_details.primary_key.transaction_id,20);
	//strcat(temp_Tran_id,"-CRT-");
	//sprintf(&temp_Tran_id[25],"%07d",falcon_tx_counter++);
	strncpy(Cr_Fld.EXTERNAL_TRANSACTION_XID,temp_Tran_id,32);
	sprintf(&message[197],"%-32s\n",Cr_Fld.EXTERNAL_TRANSACTION_XID);

	memcpy(Cr_Fld.SCORE_CUSTOMER_ACCOUNT_XID,Auth_Tx.TLF01_details.card_num,19);
	sprintf(&message[229],"%-19s\n",Cr_Fld.SCORE_CUSTOMER_ACCOUNT_XID);

	memcpy(Cr_Fld.AUTHORIZATION_POSTING_XCD,"A",1);
	sprintf(&message[248],"%-1s\n",Cr_Fld.AUTHORIZATION_POSTING_XCD);
	memset(Cr_Fld.CARD_POSTAL_XCD,' ',9);
	sprintf(&message[249],"%-9s\n",Cr_Fld.CARD_POSTAL_XCD);
	memset(Cr_Fld.CARD_COUNTRY_XCD,' ',3);
	sprintf(&message[258],"%-3s\n",Cr_Fld.CARD_COUNTRY_XCD);
	memset(Cr_Fld.CARD_OPEN_DT,' ',8);
	sprintf(&message[261],"%-8s\n",Cr_Fld.CARD_OPEN_DT);
	memset(Cr_Fld.CARD_ISSUE_DT,' ',8);
	sprintf(&message[269],"%-8s\n",Cr_Fld.CARD_ISSUE_DT);
	memset(Cr_Fld.CARD_ISSUE_TYPE_XCD,' ',1);
	sprintf(&message[277],"%-1s\n",Cr_Fld.CARD_ISSUE_TYPE_XCD);
	memset(Cr_Fld.ACCOUNT_EXPIRATION_DT,' ',8);
	sprintf(&message[278],"%-8s\n",Cr_Fld.ACCOUNT_EXPIRATION_DT);

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
	memset(Cr_Fld.CARD_CREDIT_LIMIT_AMT,' ',10);
	sprintf(&message[304],"%-10s\n",Cr_Fld.CARD_CREDIT_LIMIT_AMT);
	memset(Cr_Fld.CUSTOMER_GENDER_XCD,' ',1);
	sprintf(&message[314],"%-1s\n",Cr_Fld.CUSTOMER_GENDER_XCD);
	memset(Cr_Fld.CUSTOMER_BIRTH_DT,' ',8);
	sprintf(&message[315],"%-8s\n",Cr_Fld.CUSTOMER_BIRTH_DT);
	memset(Cr_Fld.CUSTOMER_CARD_CNT,' ',3);
	sprintf(&message[323],"%-3s\n",Cr_Fld.CUSTOMER_CARD_CNT);
	memset(Cr_Fld.INCOME_AMT,' ',10);
	sprintf(&message[326],"%-10s\n",Cr_Fld.INCOME_AMT);
	memset(Cr_Fld.CARD_TYPE_XCD,' ',1);
	sprintf(&message[336],"%-1s\n",Cr_Fld.CARD_TYPE_XCD);
	memset(Cr_Fld.PRIMARY_CARD_USE_XCD,' ',1);
	sprintf(&message[337],"%-1s\n",Cr_Fld.PRIMARY_CARD_USE_XCD);
	memcpy(Cr_Fld.TRANSACTION_DATE,Auth_Tx.TLF01_details.date_yyyymmdd,8);
	sprintf(&message[338],"%-8s\n",Cr_Fld.TRANSACTION_DATE);
	memcpy(Cr_Fld.TRANSACTION_TIME,Auth_Tx.TLF01_details.time_hhmmss,6);
	sprintf(&message[346],"%-6s\n",Cr_Fld.TRANSACTION_TIME);

	if(strcmp(Auth_Tx.TLF01_details.currency_code,"608") == 0) // PESO local currecny
	{
		sprintf(Cr_Fld.TRANSACTION_AMT,"%010d.%02d",atoi(Auth_Tx.TLF01_details.total_amount)/100, atoi(Auth_Tx.TLF01_details.total_amount)%100);

	}
	else if(strcmp(Auth_Tx.TLF01_details.currency_code,"840") == 0)// Dollor tx
	{
		if(strlen(Auth_Tx.ch_billing_amt) > 0 && atoi(Auth_Tx.ch_billing_amt) > 0)// DE6 and DE51 is avialbe .Use them , Since DE6 and 51 are in PESO values
		{
			sprintf(Cr_Fld.TRANSACTION_AMT,"%010d.%02d",atoi(Auth_Tx.ch_billing_amt)/100, atoi(Auth_Tx.ch_billing_amt)%100);
		}
		else
		{
			temp_conv_amount = ((strtod( (char *)Auth_Tx.TLF01_details.total_amount, (char **)NULL )/100) * conv_rate);
			//sprintf(temp_without_decimal,"%10ld%2ld",(long)temp_conv_amount,((long)(temp_conv_amount*100)%100));
			strncpy(temp_without_decimal,doubletostr(temp_conv_amount),12);
			sprintf(Cr_Fld.TRANSACTION_AMT,"%010d.%02d",atoi(temp_without_decimal)/100,atoi(temp_without_decimal)%100);
		}
	}
	else
	{
		if(strlen(Auth_Tx.ch_billing_amt) > 0  && atoi(Auth_Tx.ch_billing_amt) > 0 )// DE6 and DE51 is avialbe .Use them , Since DE6 and 51 are in PESO values
		{
			sprintf(Cr_Fld.TRANSACTION_AMT,"%010d.%02d",atoi(Auth_Tx.ch_billing_amt)/100, atoi(Auth_Tx.ch_billing_amt)%100);
		}
		else
		{
			double temp_conv_amount = ((strtod( (char *)Auth_Tx.TLF01_details.total_amount, (char **)NULL )/100) * conv_rate);
			//sprintf(temp_without_decimal,"%10ld%2ld",(long)temp_conv_amount,((long)(temp_conv_amount*100)%100));
			strncpy(temp_without_decimal,doubletostr(temp_conv_amount),12);
			sprintf(Cr_Fld.TRANSACTION_AMT,"%010d.%02d",atoi(temp_without_decimal)/100,atoi(temp_without_decimal)%100);
		}
	}

	if(atoi(Cr_Fld.TRANSACTION_AMT) == 0 && strcmp(Auth_Tx.TLF01_details.currency_code,"840") == 0)
	{
		 temp_conv_amount = ((strtod( (char *)Auth_Tx.TLF01_details.total_amount, (char **)NULL )/100) * conv_rate);
		 //sprintf(temp_without_decimal,"%10ld%2ld",(long)temp_conv_amount,((long)(temp_conv_amount*100)%100));
		 strncpy(temp_without_decimal,doubletostr(temp_conv_amount),12);
		 sprintf(Cr_Fld.TRANSACTION_AMT,"%010d.%02d",atoi(temp_without_decimal)/100,atoi(temp_without_decimal)%100);
	}
	strncpy(temp_iso_ration_value,"00000",5);
	strncat(temp_iso_ration_value,temp_iso_ration,8);
	strncpy(Cr_Fld.TRAN_ISO_CURRENCY_CNVRSN_RATIO,temp_iso_ration_value,13);
	strncpy(Cr_Fld.TRANSACTION_ISO_CURRENCY_XCD,"608",3);

	sprintf(&message[352],"%-13s\n",Cr_Fld.TRANSACTION_AMT);
	sprintf(&message[365],"%-3s\n",Cr_Fld.TRANSACTION_ISO_CURRENCY_XCD);
	sprintf(&message[368],"%-13s\n",Cr_Fld.TRAN_ISO_CURRENCY_CNVRSN_RATIO);

	/*
	Pls. map:
	A = 00, 10, 11, 32, 85
	D = 03, 05, 06, 12, 13, 14, 19, 20, 30, 33, 39, 42, 44, 51, 52, 53, 54, 55, 56, 57, 58, 61, 63, 64, 82, 92, N7
	P = 04, 07, 41, 43
	R = 01, 02, 08,34, 35, 36, 37, 38, 60, 62, 65, 66, 75,
	Else “BLANK”
	*/
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
	if( Auth_Tx.TLF01_details.tx_key == AUTH_SALE          || Auth_Tx.TLF01_details.tx_key == AUTH_SALE_RESPONSE ||
		Auth_Tx.TLF01_details.tx_key == AUTH_AUTHORIZATION || Auth_Tx.TLF01_details.tx_key == AUTH_AUTHORIZATION_RESPONSE||
		Auth_Tx.TLF01_details.tx_key == AUTH_ADVICE        || Auth_Tx.TLF01_details.tx_key == AUTH_ADVICE_RESPONSE||
		Auth_Tx.TLF01_details.tx_key == AUTH_ADVICE_REPEAT || Auth_Tx.TLF01_details.tx_key == AUTH_ADVICE_REPEAT_RESPONSE)
	{
		strncpy(Cr_Fld.TRANSACTION_TYPE_XCD,"M",1);
	}
	else if(Auth_Tx.TLF01_details.tx_key == AUTH_CASH_ADVANCE || Auth_Tx.TLF01_details.tx_key == AUTH_CASH_ADVANCE_RESPONSE ||
		    Auth_Tx.TLF01_details.tx_key == AUTH_CASH_ADVANCE_AUTHORIZATION || Auth_Tx.TLF01_details.tx_key == AUTH_CASH_ADVANCE_AUTHORIZATION_RESPONSE)
	{// CASH advance
		strncpy(Cr_Fld.TRANSACTION_TYPE_XCD,"C",1);
	}
	else if(Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL ||Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_RESPONSE||
			Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_ADVICE || Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_ADVICE_RESPONSE)
	{//Reversal
		strncpy(Cr_Fld.TRANSACTION_TYPE_XCD,"V",1);
	}
	else if(Auth_Tx.TLF01_details.tx_key == AUTH_VOID_SALE || Auth_Tx.TLF01_details.tx_key == AUTH_VOID_SALE_RESPONSE)
	{//VOID
		strncpy(Cr_Fld.TRANSACTION_TYPE_XCD,"X",1);
	}
	else if(Auth_Tx.TLF01_details.tx_key == AUTH_PRE_AUTHORIZATION || Auth_Tx.TLF01_details.tx_key == AUTH_PRE_AUTHORIZATION_RESPONSE)
	{//Pre AUTH
		strncpy(Cr_Fld.TRANSACTION_TYPE_XCD,"P",1);
	}
	else if(Auth_Tx.TLF01_details.tx_key == AUTH_OFFLINE_SALE || Auth_Tx.TLF01_details.tx_key == AUTH_OFFLINE_SALE_RESPONSE)
	{//Offline sale
		strncpy(Cr_Fld.TRANSACTION_TYPE_XCD,"D",1);
	}
	else
	{
		strncpy(Cr_Fld.TRANSACTION_TYPE_XCD," ",1);
	}

	sprintf(&message[382],"%-1s\n",Cr_Fld.TRANSACTION_TYPE_XCD);

	memcpy(Cr_Fld.MERCHANT_CATEGORY_XCD,Auth_Tx.TLF01_details.category_code,4);
	sprintf(&message[383],"%-4s\n",Cr_Fld.MERCHANT_CATEGORY_XCD);
	/*******memcpy(Cr_Fld.MERCHANT_POSTAL_XCD,Auth_Tx.MCF01_details.zip_code,9);*****/
	//memcpy(Cr_Fld.MERCHANT_POSTAL_XCD,"000560000",9);
	if(strncmp(Auth_Tx.MCF01_details.zip_code,"9999",4)== 0)
	{
		memset(Cr_Fld.MERCHANT_POSTAL_XCD,' ',9);
	}
	else
	{
		strncpy(Cr_Fld.MERCHANT_POSTAL_XCD,Auth_Tx.MCF01_details.zip_code,9);
	}
	sprintf(&message[387],"%-9s\n",Cr_Fld.MERCHANT_POSTAL_XCD);

	if(strlen(Auth_Tx.MCF01_details.primary_key.merchant_id)> 0)
	{
		strncpy(Cr_Fld.MERCHANT_COUNTRY_XCD,"608",3);
	}
	else if(strlen(Auth_Tx.reconciliation_cur_code)> 0)//reconciliation_cur_code
	{
		strncpy(Cr_Fld.MERCHANT_COUNTRY_XCD,Auth_Tx.reconciliation_cur_code,3);
	}
	else if(strlen(Auth_Tx.TLF01_details.num_dbsales)> 0)//nfi_seq_nbr
	{
		strncpy(Cr_Fld.MERCHANT_COUNTRY_XCD,Auth_Tx.TLF01_details.num_dbsales,3);
	}
	else
		strncpy(Cr_Fld.MERCHANT_COUNTRY_XCD,Auth_Tx.TLF01_details.currency_code,3);

	sprintf(&message[396],"%-3s\n",Cr_Fld.MERCHANT_COUNTRY_XCD);


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


/*else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_MASTERCARD)== 0)
{
	if(strcmp(Auth_Tx.TLF01_details.response_code, "00") == 0)
	memcpy(Cr_Fld.TRANSACTION_PIN_VERIFY_XCD,"V",1);
	else
	memcpy(Cr_Fld.TRANSACTION_PIN_VERIFY_XCD,"I",1);

}
else
memcpy(Cr_Fld.TRANSACTION_PIN_VERIFY_XCD," ",1);*/
	sprintf(&message[399],"%-1s\n",Cr_Fld.TRANSACTION_PIN_VERIFY_XCD);
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

/*	Please refer to POS netry mode mapping doc : # */
	char temp_pos_entry_mode[3]="";
	if(strlen(Auth_Tx.TLF01_details.pos_entry_mode)== 3)
	{
		strncpy(temp_pos_entry_mode,Auth_Tx.TLF01_details.pos_entry_mode,2);
	}
	else
	{
		strncpy(temp_pos_entry_mode,&Auth_Tx.TLF01_details.pos_entry_mode[1],2);
	}

	if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_AMEX)== 0 )
	{
		if(strncmp(Auth_Tx.TLF01_details.handler_queue,"dcp",3) != 0)
		{
			if( (Auth_Tx.amex_pos_entry_mode[5]== 'X' ) && (Auth_Tx.amex_pos_entry_mode[6] == '2'))
			{
				strncpy(temp_pos_entry_mode,"2W",2); // Contact less mag stripe
			}
			else if( Auth_Tx.amex_pos_entry_mode[4]  == 'S' ||
					 Auth_Tx.amex_pos_entry_mode[4]  == 'T')
			{
				strncpy(temp_pos_entry_mode,"81",2);
			}
			else if (Auth_Tx.amex_pos_entry_mode[6]  == '5')
			{
				strncpy(temp_pos_entry_mode,"05",2);
			}
			else if( (Auth_Tx.amex_pos_entry_mode[6] == 'W' || Auth_Tx.amex_pos_entry_mode[6] == '2') )
			{
				strncpy(temp_pos_entry_mode,"90",2); // Contact less mag stripe
			}
			else if( (Auth_Tx.amex_pos_entry_mode[6]== '9')	)
			{
					strncpy(temp_pos_entry_mode,"80",2);
			}
			else if( (Auth_Tx.amex_pos_entry_mode[5]== 'X' && Auth_Tx.amex_pos_entry_mode[6] == '5'))
			{
				strncpy(temp_pos_entry_mode,"X5",2); // Contact less micro chip
			}
			else if( (Auth_Tx.amex_pos_entry_mode[5]== '1' && Auth_Tx.amex_pos_entry_mode[6] == '5'))
			{
				strncpy(temp_pos_entry_mode,"05",2); //  chip
			}
			else if( (Auth_Tx.amex_pos_entry_mode[0]== '5' && Auth_Tx.amex_pos_entry_mode[5] == '1' && Auth_Tx.amex_pos_entry_mode[6] == '2') ||
					 (Auth_Tx.amex_pos_entry_mode[0]== '5' && Auth_Tx.amex_pos_entry_mode[5] == '1' && Auth_Tx.amex_pos_entry_mode[6] == 'W') ||
					 (Auth_Tx.amex_pos_entry_mode[6] == '9'))
			{
				strncpy(temp_pos_entry_mode,"80",2); // Fall back Mag stripe
			}
			else if( (Auth_Tx.amex_pos_entry_mode[0]== '5' && Auth_Tx.amex_pos_entry_mode[5] == '1' && Auth_Tx.amex_pos_entry_mode[6] == 'S') ||
					 (Auth_Tx.amex_pos_entry_mode[0]== '5' && Auth_Tx.amex_pos_entry_mode[5] == '1' && Auth_Tx.amex_pos_entry_mode[6] == '6') ||
					 (Auth_Tx.amex_pos_entry_mode[0]== '5' && Auth_Tx.amex_pos_entry_mode[5] == '1' && Auth_Tx.amex_pos_entry_mode[6] == '1') ||
					 (Auth_Tx.amex_pos_entry_mode[0]== '5' && Auth_Tx.amex_pos_entry_mode[5] == '1' && Auth_Tx.amex_pos_entry_mode[6] == 'V'))
			{
				strncpy(temp_pos_entry_mode,"79",2); // Fall back keyed entry
			}
			else if( Auth_Tx.amex_pos_entry_mode[0] ==  '1' ||
					 Auth_Tx.amex_pos_entry_mode[6] ==  '6' ||
					 Auth_Tx.amex_pos_entry_mode[6] ==  'S' ||
					 Auth_Tx.amex_pos_entry_mode[6] ==  'V' )
			{
				strncpy(temp_pos_entry_mode,"01",2); // keyed in
			}
			else if(strncmp(Auth_Tx.amex_pos_entry_mode,"6609S06S4140",12)== 0 ||
					strncmp(Auth_Tx.amex_pos_entry_mode,"6609S0SS4140",12)== 0 ||
					strncmp(Auth_Tx.amex_pos_entry_mode,"660101654140",12)== 0 ||
					strncmp(Auth_Tx.amex_pos_entry_mode,"2009106S0040",12)== 0 ||
					(Auth_Tx.amex_pos_entry_mode[4]== 'S'))
			{
				   strncpy(temp_pos_entry_mode,"81",2); // E-commerce5
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
				strncpy(temp_pos_entry_mode,"81",2); // E-commerce
			}
			else if(strncmp(temp_pos_entry_mode,"00",2) == 0)
			{
				strncpy(temp_pos_entry_mode,"81",2); // E-commerce
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
	else if(strcmp(Auth_Tx.BIN01_details.card_family,CARD_DINERS) == 0 )
	{
		if(strncmp(Auth_Tx.TLF01_details.handler_queue,"dcp",3) == 0)
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
	else if( strcmp(Auth_Tx.BIN01_details.card_family,CARD_JCB) == 0 )
	{
		if(strncmp(Auth_Tx.TLF01_details.handler_queue,"dcp",3)==0)
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
	else if( strcmp(Auth_Tx.BIN01_details.card_family,CARD_CUP) == 0)
	{
		if(strncmp(Auth_Tx.TLF01_details.handler_queue,"dcp",3)==0)
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
				strncpy(temp_pos_entry_mode,"90",2); // U
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
	/*else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_JCB)   == 0 &&
			strcmp(Auth_Tx.TLF01_details.handler_queue,"dcpisoA") != 0 &&
			strncmp(Auth_Tx.TLF01_details.handler_queue,"VOICE",5)!= 0)
	{
		if(strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"9720",4) == 0)
		{
			strncpy(temp_pos_entry_mode,"80",2);
		}
		else if(strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"02",2) == 0)
		{
			strncpy(temp_pos_entry_mode,"90",2);
		}
		else
		{
			strncpy(temp_pos_entry_mode,&Auth_Tx.TLF01_details.pos_entry_mode[0],2);
		}
	}*/
	else if((strcmp(Auth_Tx.BIN01_details.card_family, CARD_VISA)== 0        ||
			 strcmp(Auth_Tx.BIN01_details.card_family, CARD_MASTERCARD)== 0) &&
			 strcmp(temp_pos_entry_mode,"01") == 0 && strcmp(Auth_Tx.TLF01_details.pos_condition_code,"59") == 0)
	{
		strncpy(temp_pos_entry_mode,"81",2);
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_VISA)== 0 &&
			strcmp(Auth_Tx.TLF01_details.pos_entry_mode,"0022") == 0)
	{
			strncpy(temp_pos_entry_mode,"90",2); // U


	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_VISA)== 0 &&
			strcmp(Auth_Tx.TLF01_details.pos_entry_mode,"0021") == 0)
	{
		if( Auth_Tx.TLF01_details.product_codes[16].amount[2] == '1' ||
		    Auth_Tx.TLF01_details.product_codes[16].amount[2] == '2')
		{
			strncpy(temp_pos_entry_mode,"80",2); // F
		}
		else if (Auth_Tx.TLF01_details.product_codes[16].amount[2] == '0')
		{
			strncpy(temp_pos_entry_mode,"90",2); // U
		}
		// Need to check the DE60.3 chip condition code: If 1 0r 2 : then it should be fall back :  F
		// else if 60.3 = 0 then value should be U
	}
	/*else if(((strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"0022",3) == 0  ||
			 strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"090",3) == 0 ) &&
			 strcmp(Auth_Tx.BIN01_details.card_family, CARD_JCB)!= 0 ))*/
	else if(((strncmp(temp_pos_entry_mode,"02",2) == 0  ||
			  strncmp(temp_pos_entry_mode,"90",2) == 0) &&
			  strcmp(Auth_Tx.BIN01_details.card_family, CARD_JCB)!= 0 ))
	{ /* POS 02 or 90 + Field 60.2.2 = 5 or 6 + Field 60.2.3 = 2 */
		if( strncmp(Auth_Tx.TLF01_details.product_codes[16].amount,"052",3)== 0 ||
			(strncmp(Auth_Tx.TLF01_details.product_codes[16].amount,"062",3)== 0))
		{
			strncpy(temp_pos_entry_mode,"80",2);
		}
		else if(strncmp(Auth_Tx.TLF01_details.product_codes[16].amount[10],"4",1)== 0) // Need to check 61.11 for MC if the value is 4 = Contact less mag stripe
		{
			strncpy(temp_pos_entry_mode,"91",2);
		}else
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



	if(strncmp(temp_pos_entry_mode,"91",2) == 0 || strncmp(temp_pos_entry_mode,"2W",2) == 0)
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
	else if(strncmp(temp_pos_entry_mode,"81",2) == 0)
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


	//memcpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"S",1);
	sprintf(&message[401],"%-1s\n",Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG);

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

	memset(Cr_Fld.AUTHORIZATION_POSTING_MIS_XCD,' ',1);
	sprintf(&message[410],"%-1s\n",Cr_Fld.AUTHORIZATION_POSTING_MIS_XCD);

	memset(Cr_Fld.TRANS_POSTING_MISMATCH_XFLG,' ',1);
	sprintf(&message[411],"%-1s\n",Cr_Fld.TRANS_POSTING_MISMATCH_XFLG);


	memset(Cr_Fld.CREATE_CASE_XFLG,' ',1);
	sprintf(&message[412],"%-1s\n",Cr_Fld.CREATE_CASE_XFLG);

	/******memset(Cr_Fld.USER_INDICATOR_1_XCD,' ',1);*****/
	/****memset(Cr_Fld.USER_INDICATOR_2_XCD,' ',1);*****/
	/****memset(Cr_Fld.USER_DATA_1_STRG,' ',10);*******/

	memset(Cr_Fld.USER_INDICATOR_1_XCD,' ',1);
	//strncpy(Auth_Tx.BON01_details.minimum_amount,sPAF01.APP_Source,sizeof(sPAF01.APP_Source));
	//strncpy(Auth_Tx.BON01_details.response_message,sPAF01.COMP_Name,sizeof(sPAF01.COMP_Name));
	sprintf(&message[413],"%-1s\n",Cr_Fld.USER_INDICATOR_1_XCD);
	memset(Cr_Fld.USER_INDICATOR_2_XCD,' ',1);
	sprintf(&message[414],"%-1s\n",Cr_Fld.USER_INDICATOR_2_XCD);
	memset(Cr_Fld.USER_DATA_1_STRG,' ',10); // UCAF indicator DE48 SE43

	/*Field 48 for Terminal type*/
	if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_VISA)	== 0)//VISA
	{
		memcpy(Cr_Fld.USER_DATA_1_STRG,&Auth_Tx.additional_pos_information[0],1);
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_MASTERCARD)	== 0)//MC
	{
		memcpy(Cr_Fld.USER_DATA_1_STRG,&Auth_Tx.acceptor_term_name[9],1);
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_AMEX)	== 0)//AMEX
	{
		strncpy(Cr_Fld.USER_DATA_1_STRG,&Auth_Tx.amex_pos_entry_mode[3],1);
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_CUP)	== 0)//CUP
	{
		if(strncmp(Auth_Tx.TLF01_details.handler_queue,"VOICE",5)== 0)
		{
			if(0 ==strncmp(Auth_Tx.TLF01_details.pos_condition_code,"08",2))
			{
				memcpy(Cr_Fld.USER_DATA_1_STRG,"08", 2);//Mobile phone Voice transaction
			}
			else
			{
				memcpy(Cr_Fld.USER_DATA_1_STRG,"07", 2);//PC Voice transaction
			}
		}
		else
		{
			memcpy(Cr_Fld.USER_DATA_1_STRG,"03", 2);//POS-Point Of Sale 03
		}
	}

	sprintf(&message[415],"%-10s\n",Cr_Fld.USER_DATA_1_STRG);

	memset(Cr_Fld.USER_DATA_2_STRG,' ',10);
	/*Field 49 for Terminal Entry capa*/
	if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_VISA)	== 0)//VISA
	{
		memcpy(Cr_Fld.USER_DATA_2_STRG,&Auth_Tx.additional_pos_information[1],1);
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_MASTERCARD)	== 0)//MC
	{
		memcpy(Cr_Fld.USER_DATA_2_STRG,&Auth_Tx.acceptor_term_name[10],1);
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_AMEX)	== 0)//AMEX
	{
		/*if(Auth_Tx.TLF01_details.product_codes[16].amount[0]=='5')
			strncpy(Cr_Fld.USER_DATA_2_STRG,"C",1);
		else*/
			memcpy(Cr_Fld.USER_DATA_2_STRG,&Auth_Tx.TLF01_details.product_codes[16].amount[0],1);
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_CUP)	== 0)//CUP
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
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_DINERS)	== 0)//DCI
	{
		Cr_Fld.USER_DATA_2_STRG[0] = Auth_Tx.amex_pos_entry_mode[0] ;
		Cr_Fld.USER_DATA_2_STRG[1] = '\0';
	}
	/*if(strcmp(Auth_Tx.BON01_details.minimum_amount,"")!= 0)
	{
		strncpy(Cr_Fld.USER_DATA_2_STRG,Auth_Tx.BON01_details.minimum_amount,10);
	}*/
	sprintf(&message[425],"%-10s\n",Cr_Fld.USER_DATA_2_STRG);
	memset(Cr_Fld.MERCHANT_XIDD,' ',10);
	sprintf(&message[435],"%-10s\n",Cr_Fld.MERCHANT_XIDD);
	memset(Cr_Fld.MERCHANT_DATA_XFLG,' ',1);
	sprintf(&message[445],"%-1s\n",Cr_Fld.MERCHANT_DATA_XFLG);
	memset(Cr_Fld.CARD_DATA_AVAILABLE_XFLG,' ',1);
	sprintf(&message[446],"%-1s\n",Cr_Fld.CARD_DATA_AVAILABLE_XFLG);
	memcpy(Cr_Fld.EXTERNAL_1_SCR,"0000",4);
	sprintf(&message[447],"%-4s\n",Cr_Fld.EXTERNAL_1_SCR);
	memcpy(Cr_Fld.EXTERNAL_2_SCR,"0000",4);
	sprintf(&message[451],"%-4s\n",Cr_Fld.EXTERNAL_2_SCR);
	memcpy(Cr_Fld.EXTERNAL_3_SCR,"0000",4);
	sprintf(&message[455],"%-4s\n",Cr_Fld.EXTERNAL_3_SCR);

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

	if(strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"E",1) == 0)
	{
		memcpy(Cr_Fld.CAT_TYPE_XFLG,"E",1);// POS Condition code
	}
	else
	{
		memcpy(Cr_Fld.CAT_TYPE_XFLG,"S",1);// POS Condition code
	}
	sprintf(&message[460],"%-1s\n",Cr_Fld.CAT_TYPE_XFLG);

	memset(Cr_Fld.TESTING_RANDOM_DIGITS_STRG,' ',2);
	sprintf(&message[461],"%-2s\n",Cr_Fld.TESTING_RANDOM_DIGITS_STRG);
	memset(Cr_Fld.PORTFOLIO_NAME,' ',14);
	sprintf(&message[463],"%-14s\n",Cr_Fld.PORTFOLIO_NAME);
	memset(Cr_Fld.CLIENT_2_XID,' ',14);
	sprintf(&message[477],"%-14s\n",Cr_Fld.CLIENT_2_XID);
	memset(Cr_Fld.INTERBANK_CARD_ASSOCIATION_NUM,' ',6);
	sprintf(&message[491],"%-6s\n",Cr_Fld.INTERBANK_CARD_ASSOCIATION_NUM);
	if(strlen(Auth_Tx.MCF01_details.name02)> 0)
	{
		memcpy(Cr_Fld.MERCHANT_NAME,Auth_Tx.MCF01_details.name02,30);
	}
	else if(strlen(Auth_Tx.TLF01_details.card_holder_name) > 0)
	{
		memcpy(Cr_Fld.MERCHANT_NAME,Auth_Tx.TLF01_details.card_holder_name,30);
	}

	sprintf(&message[497],"%-40s\n",Cr_Fld.MERCHANT_NAME);

	if(strlen(Auth_Tx.MCF01_details.city)== 0)
	{
		memcpy(Cr_Fld.MERCHANT_CITY_NAME,Auth_Tx.TLF01_details.ncf01_retired_cwk,13);
	}
	else
	{
		memcpy(Cr_Fld.MERCHANT_CITY_NAME,Auth_Tx.MCF01_details.city,20);
	}
	sprintf(&message[537],"%-30s\n",Cr_Fld.MERCHANT_CITY_NAME);

	if(strlen(Auth_Tx.MCF01_details.intl_code)== 0)
	{
		memcpy(Cr_Fld.MERCHANT_STATE_XCD,Auth_Tx.TLF01_details.ncf01_retired_cwk+13,3);
	}
	else
	{
		memcpy(Cr_Fld.MERCHANT_STATE_XCD,Auth_Tx.MCF01_details.intl_code,3);
	}
	sprintf(&message[567],"%-3s\n",Cr_Fld.MERCHANT_STATE_XCD);
	memset(Cr_Fld.SUPPRES_CASE_XFLG,' ',1);
	sprintf(&message[570],"%-1s\n",Cr_Fld.SUPPRES_CASE_XFLG);
	/***memcpy(Cr_Fld.USER_INDICATOR_3_XCD,' ' ,5);****/
	memcpy(Cr_Fld.USER_INDICATOR_3_XCD,"",5);
	sprintf(&message[571],"%-5s\n",Cr_Fld.USER_INDICATOR_3_XCD);

	/**len**strcpy(Cr_Fld.USER_INDICATOR_4_XCD,"00");**/
	//strncpy(Cr_Fld.USER_INDICATOR_4_XCD,"",5);
	//strncat(Cr_Fld.USER_INDICATOR_4_XCD,Auth_Tx.TLF01_details.currency_code,3);
	strncpy(Cr_Fld.USER_INDICATOR_4_XCD,SPACES,FIELD67_SIZE);
	sprintf(&message[576],"%-5s\n",Cr_Fld.USER_INDICATOR_4_XCD);
	/* Dual currrecny logic*/

	/**len**strcpy(Cr_Fld.USER_DATA_3_STRG,"000");**/
	strncpy(Cr_Fld.USER_DATA_3_STRG,"00",2);
	if(strlen(Auth_Tx.reconciliation_amount) > 0)
	{
		sprintf(temp_totalamt,"%010d.%02d",atoi(Auth_Tx.reconciliation_amount)/100, atoi(Auth_Tx.reconciliation_amount)%100);
	}
	else
	{
		sprintf(temp_totalamt,"%010d.%02d",atoi(Auth_Tx.TLF01_details.total_amount)/100, atoi(Auth_Tx.TLF01_details.total_amount)%100);
	}
	strncat(Cr_Fld.USER_DATA_3_STRG,temp_totalamt,13);
	sprintf(&message[581],"%-15s\n",Cr_Fld.USER_DATA_3_STRG);

	//memcpy(Cr_Fld.USER_DATA_4_STRG,Auth_Tx.ACF01_details.primary_key.account_nbr,20);
/*	if(strlen(Auth_Tx.reconciliation_cur_code) > 0)
	{
		strncpy(Cr_Fld.USER_DATA_4_STRG,Auth_Tx.reconciliation_cur_code,3);
	}
	else
	{
		strncpy(Cr_Fld.USER_DATA_4_STRG,Auth_Tx.TLF01_details.currency_code,3);
	}*/
	//strncpy(Cr_Fld.USER_DATA_4_STRG,Auth_Tx.TLF01_details.response_text,20);
	len = strlen(Auth_Tx.TLF01_details.currency_code);
	strncpy(Cr_Fld.USER_DATA_4_STRG,Auth_Tx.TLF01_details.currency_code,len);
	sprintf(&message[596],"%-20s\n",Cr_Fld.USER_DATA_4_STRG);

	memset(Cr_Fld.USER_DATA_5_STRG,' ',40);
	/*Field 70 for Response message*/
	//Auth_Tx.TLF01_details.response_text
/*	if(strlen(Auth_Tx.TLF01_details.response_text)> 0)
	{
		strncpy(Cr_Fld.USER_DATA_5_STRG,Auth_Tx.TLF01_details.response_text,30);
	}
	else if(strcmp(Auth_Tx.BON01_details.response_message,"")!= 0)
	{
		strncpy(Cr_Fld.USER_DATA_5_STRG,Auth_Tx.BON01_details.response_message,30);
	}*/

	//strncpy(Cr_Fld.USER_DATA_5_STRG,Auth_Tx.TLF01_details.response_text,30);

	if (atoi(Auth_Tx.TLF01_details.tx_key) > 100)
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

	memset(Cr_Fld.REAL_TIME_REQUEST_XFLG,' ',1);
	sprintf(&message[656],"%-1s\n",Cr_Fld.REAL_TIME_REQUEST_XFLG);
	memset(Cr_Fld.PRIOR_ACTION_DATABASE_XCD,' ',1);
	sprintf(&message[657],"%-1s\n",Cr_Fld.PRIOR_ACTION_DATABASE_XCD);
	memset(Cr_Fld.PRIOR_ACTION_DB_EXPIRATION_DT,' ',8);
	sprintf(&message[658],"%-8s\n",Cr_Fld.PRIOR_ACTION_DB_EXPIRATION_DT);
	memset(Cr_Fld.MASTER_ACCOUNT_NUMBER_XID,' ',19);
	sprintf(&message[666],"%-19s\n",Cr_Fld.MASTER_ACCOUNT_NUMBER_XID);
	memset(Cr_Fld.CARD_OFFLN_STATIC_AUTHCTN_XFLG,' ',1);
	sprintf(&message[685],"%-1s\n",Cr_Fld.CARD_OFFLN_STATIC_AUTHCTN_XFLG);
	memset(Cr_Fld.CARD_DYNAMIC_AUTHCTN_XCD,' ',1);
	sprintf(&message[686],"%-1s\n",Cr_Fld.CARD_DYNAMIC_AUTHCTN_XCD);
	memset(Cr_Fld.RESERVED_02,' ',1);
	sprintf(&message[687],"%-1s\n",Cr_Fld.RESERVED_02);
	memset(Cr_Fld.CARD_AIP_VERIFICATION_XFLG,' ',1);
	sprintf(&message[688],"%-1s\n",Cr_Fld.CARD_AIP_VERIFICATION_XFLG);
	memset(Cr_Fld.CARD_AIP_RISK_XFLG,' ',1);
	sprintf(&message[689],"%-1s\n",Cr_Fld.CARD_AIP_RISK_XFLG);
	memset(Cr_Fld.CARD_ISSUER_AUTH_SUPPORT_XFLG,' ',1);
	sprintf(&message[690],"%-1s\n",Cr_Fld.CARD_ISSUER_AUTH_SUPPORT_XFLG);

	memset(Cr_Fld.CARD_COMBINED_AUTHCTN_XFLG,' ',1);
	sprintf(&message[691],"%-1s\n",Cr_Fld.CARD_COMBINED_AUTHCTN_XFLG);
	memset(Cr_Fld.TRANSACTION_DELINQUENT_CYC,' ',1);
	sprintf(&message[692],"%-1s\n",Cr_Fld.TRANSACTION_DELINQUENT_CYC);
	memset(Cr_Fld.TRANS_TOTAL_DELINQUENT_AMT,' ',13);
	sprintf(&message[693],"%-13s\n",Cr_Fld.TRANS_TOTAL_DELINQUENT_AMT);
	memset(Cr_Fld.CRD_STATEMENT_CASH_BALANCE_AMT,' ',13);
	sprintf(&message[706],"%-13s\n",Cr_Fld.CRD_STATEMENT_CASH_BALANCE_AMT);
	memset(Cr_Fld.MERCHANT_BALANCE_AMT,' ',13);
	sprintf(&message[719],"%-13s\n",Cr_Fld.MERCHANT_BALANCE_AMT);
	memset(Cr_Fld.CUSTOMER_POSTAL_XCD,' ',12);
	sprintf(&message[732],"%-12s\n",Cr_Fld.CUSTOMER_POSTAL_XCD);
	memset(Cr_Fld.CARD_MEDIA_TYPE_XCD,' ',1);
	sprintf(&message[744],"%-1s\n",Cr_Fld.CARD_MEDIA_TYPE_XCD);
	// FOR MC p_auth_tx->TLF01_details.cvc
	// 	if DE48 TAG 87 value is P = 0, M or N = 1, U = Blank , Any value = 9
	// For VISA
	// DE44.10 CVV2 result code : p_auth_tx->TLF01_details.product_codes[17].quantity[0]
	//CUP : DE61.4
	// AMEX : 61 Sub 5 : 1 = if not space else balnk
	/*icvc_data=atoi(Auth_Tx.TLF01_details.cvc_data);
	switch(icvc_data)
	{
	case 0:memcpy(Cr_Fld.CVV2_PRESENT_XFLG,"0",1);
		   sprintf(&message[745],"%-1s\n",Cr_Fld.CVV2_PRESENT_XFLG);
		   break;
	case 1:memcpy(Cr_Fld.CVV2_PRESENT_XFLG,"1",1);
		   sprintf(&message[745],"%-1s\n",Cr_Fld.CVV2_PRESENT_XFLG);
		   break;
	case 2:memcpy(Cr_Fld.CVV2_PRESENT_XFLG,"2",1);
		   sprintf(&message[745],"%-1s\n",Cr_Fld.CVV2_PRESENT_XFLG);
		   break;
	case 9:memcpy(Cr_Fld.CVV2_PRESENT_XFLG,"9",1);
		   sprintf(&message[745],"%-1s\n",Cr_Fld.CVV2_PRESENT_XFLG);
		   break;
	default:
			if(strcmp(Cr_Fld.CVV2_RESPONSE_XFLG,"N") == 0)
			{
				strncpy(Cr_Fld.CVV2_PRESENT_XFLG,"2",1);
			}
			else if(strcmp(Cr_Fld.CVV2_RESPONSE_XFLG,"M") == 0)
			{
				strncpy(Cr_Fld.CVV2_PRESENT_XFLG,"1",1);
			}
			else if(strncmp(Auth_Tx.TLF01_details.response_code,"N7",2) == 0)
			{
				Cr_Fld.CVV2_PRESENT_XFLG[0]  = '1';
				Cr_Fld.CVV2_RESPONSE_XFLG[0] = 'N';
			}
			else
			{
				memset(Cr_Fld.CVV2_PRESENT_XFLG,' ',1);
			}
			sprintf(&message[745],"%-1s\n",Cr_Fld.CVV2_PRESENT_XFLG);
			break;
	}*/

	/****memset(Cr_Fld.CVV2_RESPONSE_XFLG,' ',1);****/
	/*if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_JCB)== 0 ||
	   strcmp(Auth_Tx.BIN01_details.card_family, CARD_AMEX)== 0 )
	{
		memset(Cr_Fld.CVV2_RESPONSE_XFLG,' ',1);
	}
	else if(isalphanum(Cr_Fld.CVV2_PRESENT_XFLG) == true)
	{
		if(strcmp(Cr_Fld.CVV2_PRESENT_XFLG,"0") == 0)
		{
			strcpy(Cr_Fld.CVV2_RESPONSE_XFLG," ");
		}
		else if(strlen(Cr_Fld.CVV2_RESPONSE_XFLG) == 0)
		{
			strcpy(Cr_Fld.CVV2_RESPONSE_XFLG," ");
		}
	}*/

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
		// Not getting value
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_CUP)== 0)
	{
		// Not getting value
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_DINERS)== 0)
	{
		// Not getting value
	}


	sprintf(&message[745],"%-1s\n",Cr_Fld.CVV2_PRESENT_XFLG);

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
		// Not getting value
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_CUP)== 0)
	{
		// Not getting value
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_DINERS)== 0)
	{
		// Not getting value
	}


	sprintf(&message[746],"%-1s\n",Cr_Fld.CVV2_RESPONSE_XFLG);
	/*****memset(Cr_Fld.AVS_RESPONSE_XCD,' ',1);*******/
	memset(Cr_Fld.AVS_RESPONSE_XCD,' ',1);
	sprintf(&message[747],"%-1s\n",Cr_Fld.AVS_RESPONSE_XCD);
	/***memcpy(Cr_Fld.TRANSACTION_CATEGORY_XCD,"O",1);*****/
	//memcpy(Cr_Fld.TRANSACTION_CATEGORY_XCD,"P",1);
	if(strlen(Cr_Fld.TRANSACTION_CATEGORY_XCD) == 0 || strcmp(Cr_Fld.TRANSACTION_CATEGORY_XCD, " ") == 0)
	strncpy(Cr_Fld.TRANSACTION_CATEGORY_XCD,"O",1);
	sprintf(&message[748],"%-1s\n",Cr_Fld.TRANSACTION_CATEGORY_XCD);
	/*Auth_Tx.TLF01_details.acquirer_id*/
	/******memset(Cr_Fld.ACQUIRER_XID,' ',12);*****/
	//memcpy(Cr_Fld.ACQUIRER_XID,"211111111112",12);
	strncpy(Cr_Fld.ACQUIRER_XID,Auth_Tx.TLF01_details.acquiring_id,12);
	sprintf(&message[749],"%-12s\n",Cr_Fld.ACQUIRER_XID);
	/*If it's a local transactions*/

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
	/*********memcpy(Cr_Fld.TERMINAL_TYPE_XCD,"O",1);************/
	/*Field 95 Terminal Type*/
	// Need to use the new logic from CTRNAS
	memset(Cr_Fld.TERMINAL_TYPE_XCD,' ',1);
	if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_VISA)	== 0)//VISA
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
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_MASTERCARD) == 0)// MC
	{
	   //memcpy(Cr_Fld.TERMINAL_TYPE_XCD,&Auth_Tx.TLF01_details.product_codes[16].amount[10],1);
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
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_AMEX)== 0)// AMEX
	{
		//Auth_Tx.amex_pos_entry_mode
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
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_CUP)== 0)// CUP
	{
		if( 0 == strncmp(Auth_Tx.TLF01_details.handler_queue,"VOICE",5))
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"I",1);
		}
		/*else if(strncmp(Auth_Tx.TLF01_details.pos_condition_code,"59",2)== 0)
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"U",1);
		}
		else
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"L",1);
		}*/
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
	/*********memcpy(Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD,"O",1);*****************/
	/*Field 96 Terminal Entry capability */
	memset(Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD,' ',1);
	if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_VISA)	== 0)//VISA
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
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_MASTERCARD) == 0)// MC
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
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_AMEX)== 0)// AMEX
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
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_CUP) == 0)// CUP
	{
	/*	if(strncmp(Cr_Fld.USER_DATA_2_STRG,"5",1)== 0 || strncmp(Cr_Fld.USER_DATA_2_STRG,"6",1)== 0  )
		{
			strncpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"C",1);
		}
		else if(strncmp(Cr_Fld.USER_DATA_2_STRG,"2",1)== 0)
		{
			strncpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"M",1);
		}*/
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
	//memcpy(Cr_Fld.TRANSACTION_POS_CONDITION_XCD,"00",2);
	strncpy(Cr_Fld.TRANSACTION_POS_CONDITION_XCD,Auth_Tx.TLF01_details.pos_condition_code,2);

	sprintf(&message[782],"%-2s\n",Cr_Fld.TRANSACTION_POS_CONDITION_XCD);
	/***************memcpy(Cr_Fld.ATM_NETWORK_XID,,);*************/
	memcpy(Cr_Fld.ATM_NETWORK_XID," ",1);
	sprintf(&message[784],"%-1s\n",Cr_Fld.ATM_NETWORK_XID);
	memset(Cr_Fld.RESERVED_01,' ',1);
	sprintf(&message[785],"%-1s\n",Cr_Fld.RESERVED_01);
	memset(Cr_Fld.CHECK_NUM,' ',6);
	sprintf(&message[786],"%-6s\n",Cr_Fld.CHECK_NUM);
	memset(Cr_Fld.TERML_VERIFICATION_RESULTS_XCD,' ',10);
	sprintf(&message[792],"%-10s\n",Cr_Fld.TERML_VERIFICATION_RESULTS_XCD);
	memset(Cr_Fld.CARD_VERIFICATION_RESULTS_XCD,' ',10);
	sprintf(&message[802],"%-10s\n",Cr_Fld.CARD_VERIFICATION_RESULTS_XCD);
	/*********memset(Cr_Fld.AUTHZN_RQST_CRYPTO_VALID_XFLG,' ',1);**********/
	memset(Cr_Fld.AUTHZN_RQST_CRYPTO_VALID_XFLG,' ',1);
	sprintf(&message[812],"%-1s\n",Cr_Fld.AUTHZN_RQST_CRYPTO_VALID_XFLG);
	/************memset(Cr_Fld.ATC_CARD_CNT,' ',5);***************/
	memset(Cr_Fld.ATC_CARD_CNT,' ',5);
	sprintf(&message[813],"%-5s\n",Cr_Fld.ATC_CARD_CNT);
	/*********memset(Cr_Fld.ATC_HOST_CNT,' ',5);**************/
	memset(Cr_Fld.ATC_HOST_CNT,' ',5);
	sprintf(&message[818],"%-5s\n",Cr_Fld.ATC_HOST_CNT);
	memset(Cr_Fld.RESERVED_03,' ',2);
	sprintf(&message[823],"%-2s\n",Cr_Fld.RESERVED_03);
	memset(Cr_Fld.TERML_TO_CRD_OFFLINE_LIMIT_XCD,' ',2);
	sprintf(&message[825],"%-2s\n",Cr_Fld.TERML_TO_CRD_OFFLINE_LIMIT_XCD);
	memset(Cr_Fld.TERMINAL_OFFLINE_LIMIT_MAX_XCD,' ',2);
	sprintf(&message[827],"%-2s\n",Cr_Fld.TERMINAL_OFFLINE_LIMIT_MAX_XCD);
	memset(Cr_Fld.RECURRING_AUTHORIZATION_XCD,' ',2);
	sprintf(&message[829],"%-2s\n",Cr_Fld.RECURRING_AUTHORIZATION_XCD);
	memset(Cr_Fld.RECURRING_AUTHZN_EXPIRATION_DT,' ',8);
	sprintf(&message[831],"%-8s\n",Cr_Fld.RECURRING_AUTHZN_EXPIRATION_DT);
	memset(Cr_Fld.CARD_ASSOCIATION_XFLG,' ',1);
	sprintf(&message[839],"%-1s\n",Cr_Fld.CARD_ASSOCIATION_XFLG);
	memset(Cr_Fld.CARD_INCENTIVE_XCD,' ',1);
	sprintf(&message[840],"%-1s\n",Cr_Fld.CARD_INCENTIVE_XCD);
	memset(Cr_Fld.CARD_STATUS_XCD,' ',2);
	sprintf(&message[841],"%-2s\n",Cr_Fld.CARD_STATUS_XCD);
	memset(Cr_Fld.CARD_STATUS_DT,' ',8);
	sprintf(&message[843],"%-8s\n",Cr_Fld.CARD_STATUS_DT);
	/********memset(Cr_Fld.PROCESSOR_REASON_XCD,' ',5);*****
	*****memcpy(&Cr_Fld.PROCESSOR_REASON_XCD[3],Auth_Tx.TLF01_details.response_code,2);**************/
	memset(Cr_Fld.PROCESSOR_REASON_XCD,' ',5);
	memcpy(&Cr_Fld.PROCESSOR_REASON_XCD[3],Auth_Tx.TLF01_details.response_code,2);
	sprintf(&message[851],"%-5s\n",Cr_Fld.PROCESSOR_REASON_XCD);

	if(strcmp(Auth_Tx.TLF01_details.product_codes[1].code,"stndin") == 0) // STAND IN
	{
		memcpy(Cr_Fld.TRANSACTION_ADVICE_XCD,"A",1);
		sprintf(&message[856],"%-1s\n",Cr_Fld.TRANSACTION_ADVICE_XCD);
	}
	else
	{
		memset(Cr_Fld.TRANSACTION_ADVICE_XCD,' ',1);
		sprintf(&message[856],"%-1s\n",Cr_Fld.TRANSACTION_ADVICE_XCD);
	}
	memset(Cr_Fld.MERCHANT_XID,' ',16);
	memcpy(Cr_Fld.MERCHANT_XID,Auth_Tx.TLF01_details.merchant_id,15);
	sprintf(&message[857],"%-16s\n",Cr_Fld.MERCHANT_XID);

	memset(Cr_Fld.CARD_ORDER_TYPE_XCD,' ',1);
	sprintf(&message[873],"%-1s\n",Cr_Fld.CARD_ORDER_TYPE_XCD);
	memset(Cr_Fld.TRANSACTION_CASHBACK_AMT,' ',13);
	sprintf(&message[874],"%-13s\n",Cr_Fld.TRANSACTION_CASHBACK_AMT);
	/***********memset(Cr_Fld.USER_DATA_6_NUM,' ',13);************/
	/*Field 120 ECI value*/
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
	/* User data 7 filled with AMEX POS DATA - Auth_Tx.amex_pos_entry_mode*/
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
		//memset(Cr_Fld.USER_DATA_7_STRG,' ',40);
	}

	sprintf(&message[900],"%-40s\n",Cr_Fld.USER_DATA_7_STRG);
	memset(Cr_Fld.PAYMENT_INSTRUMENT_XID,' ',30);
	sprintf(&message[940],"%-30s\n",Cr_Fld.PAYMENT_INSTRUMENT_XID);
	/*if(Cr_Fld.TRANSACTION_TYPE_XCD == "A" && Cr_Fld.TRANSACTION_AMT == "0000000000.00")
	{
		memcpy(Cr_Fld.AVS_REQUEST_XCD,"1",1);
		sprintf(&message[970],"%-1s\n",Cr_Fld.AVS_REQUEST_XCD);
	}
	else if(Cr_Fld.TRANSACTION_TYPE_XCD == "A" && Cr_Fld.TRANSACTION_TYPE_XCD == "P" && Cr_Fld.TRANSACTION_AMT == "0000000000.00")
	{
		memcpy(Cr_Fld.AVS_REQUEST_XCD,"2",1);
		sprintf(&message[970],"%-1s\n",Cr_Fld.AVS_REQUEST_XCD);
	}
	else
	{
		memset(Cr_Fld.AVS_REQUEST_XCD,' ',1);
		sprintf(&message[970],"%-1s\n",Cr_Fld.AVS_REQUEST_XCD);
	}
	*/
		memset(Cr_Fld.AVS_REQUEST_XCD,' ',1);
		sprintf(&message[970],"%-1s\n",Cr_Fld.AVS_REQUEST_XCD);

	/**********memset(Cr_Fld.CVR_OFFLINEPIN_VERIFYPERF_XFLG,' ',1);******/
	memset(Cr_Fld.CVR_OFFLINEPIN_VERIFYPERF_XFLG,' ',1);
	sprintf(&message[971],"%-1s\n",Cr_Fld.CVR_OFFLINEPIN_VERIFYPERF_XFLG);
	/**********memset(Cr_Fld.CVR_OFFLINEPIN_VERIFYSTAT_XFLG,' ',1);**********/
	memset(Cr_Fld.CVR_OFFLINEPIN_VERIFYSTAT_XFLG,' ',1);
	sprintf(&message[972],"%-1s\n",Cr_Fld.CVR_OFFLINEPIN_VERIFYSTAT_XFLG);

	memcpy(Cr_Fld.CVR_PIN_TRYLIMIT_EXCEED_XFLG,"0",1);
	sprintf(&message[973],"%-1s\n",Cr_Fld.CVR_PIN_TRYLIMIT_EXCEED_XFLG);

	/********memset(Cr_Fld.POS_TERMINAL_ATTEND_XFLG,' ',1);******/
	memset(Cr_Fld.POS_TERMINAL_ATTEND_XFLG,' ',1);
	sprintf(&message[974],"%-1s\n",Cr_Fld.POS_TERMINAL_ATTEND_XFLG);
	/********memset(Cr_Fld.POS_OFF_PREMISES_XFLG,' ',1);******/
	memset(Cr_Fld.POS_OFF_PREMISES_XFLG,' ',1);
	sprintf(&message[975],"%-1s\n",Cr_Fld.POS_OFF_PREMISES_XFLG);
	/********memset(Cr_Fld.POS_CARD_CAPTURE_XFLG,' ',1);**********/
	memset(Cr_Fld.POS_CARD_CAPTURE_XFLG,' ',1);
	sprintf(&message[976],"%-1s\n",Cr_Fld.POS_CARD_CAPTURE_XFLG);
	/********memset(Cr_Fld.POS_SECURITY_XFLG,' ',1);***********/
	memset(Cr_Fld.POS_SECURITY_XFLG,' ',1);
	sprintf(&message[977],"%-1s\n",Cr_Fld.POS_SECURITY_XFLG);
	memcpy(Cr_Fld.AUTHORIZATION_XID,Auth_Tx.TLF01_details.auth_number,6);
	sprintf(&message[978],"%-6s\n",Cr_Fld.AUTHORIZATION_XID);
	/****memset(Cr_Fld.USER_DATA_8_STRG,' ',10);*******/
	memset(Cr_Fld.USER_DATA_8_STRG,' ',10);
	/*		Get the value from:
			MasterCard = ISO DE22 subelement 1
			VISA = ISO Field22 Position 1-2
			JCB = ISO POS ENTRY
			AMEX = blank
	*/
	/*if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_AMEX)	!= 0)
	{
		if(strcmp(Auth_Tx.BIN01_details.card_family,CARD_JCB) == 0)
		{
			strncpy(Cr_Fld.USER_DATA_8_STRG,&Auth_Tx.TLF01_details.pos_entry_mode[1],2);
		}
		else if(strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"0802",4)== 0)//Fall back
		{
			if(strcmp(Auth_Tx.BIN01_details.card_family,CARD_VISA) == 0)
			{
				strncpy(Cr_Fld.USER_DATA_8_STRG,"90",2);
			}
			else if(strcmp(Auth_Tx.BIN01_details.card_family,CARD_MASTERCARD) == 0)
			{
				strncpy(Cr_Fld.USER_DATA_8_STRG,"80",2);
			}
		}
		else if(strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"0022",4)== 0 ||
			strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"080",3)== 0 )
		{
			strncpy(Cr_Fld.USER_DATA_8_STRG,"90",2);
		}
		else if(strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"0021",4)== 0)
		{
			strncpy(Cr_Fld.USER_DATA_8_STRG,"91",2);
		}
		else
		{
			strncpy(Cr_Fld.USER_DATA_8_STRG,&Auth_Tx.TLF01_details.pos_entry_mode[1],2);
		}
	}*/
	// DE22 - Field132 stro only DE22.1 value
	//0022 -> 90 , 0802 ->90,0021 ->90
	memcpy(Cr_Fld.USER_DATA_8_STRG,&Auth_Tx.TLF01_details.pos_entry_mode,4);
	sprintf(&message[984],"%-10s\n",Cr_Fld.USER_DATA_8_STRG);
	/*******memset(Cr_Fld.USER_DATA_9_STRG,' ',10);********/
	memset(Cr_Fld.USER_DATA_9_STRG,' ',10);
	sprintf(&message[994],"%-10s\n",Cr_Fld.USER_DATA_9_STRG);
	/*******memset(Cr_Fld.USER_INDICATOR_5_XCD,' ',1);********/
	memset(Cr_Fld.USER_INDICATOR_5_XCD,' ',1);
	sprintf(&message[1004],"%-1s\n",Cr_Fld.USER_INDICATOR_5_XCD);
	/*******memset(Cr_Fld.USER_INDICATOR_6_XCD,' ',1);********/
	memset(Cr_Fld.USER_INDICATOR_6_XCD,' ',1);
	sprintf(&message[1005],"%-1s\n",Cr_Fld.USER_INDICATOR_6_XCD);
	/*******memset(Cr_Fld.USER_INDICATOR_7_XCD,' ',5);********/
	/*Field 136 Service code*/
	memset(Cr_Fld.USER_INDICATOR_7_XCD,' ',5);
	strncpy(Cr_Fld.USER_INDICATOR_7_XCD,Auth_Tx.TLF01_details.service_code,3);
	sprintf(&message[1006],"%-5s\n",Cr_Fld.USER_INDICATOR_7_XCD);
	/*******memset(Cr_Fld.USER_INDICATOR_8_XCD,' ',5);********/
	memset(Cr_Fld.USER_INDICATOR_8_XCD,' ',5);
	sprintf(&message[1011],"%-5s\n",Cr_Fld.USER_INDICATOR_8_XCD);

	memset(Cr_Fld.MODEL_CONTROL_1_XCD,' ',1);
	sprintf(&message[1016],"%-1s\n",Cr_Fld.MODEL_CONTROL_1_XCD);
	memset(Cr_Fld.MODEL_CONTROL_2_XCD,' ',1);
	sprintf(&message[1017],"%-1s\n",Cr_Fld.MODEL_CONTROL_2_XCD);
	memset(Cr_Fld.MODEL_CONTROL_3_XCD,' ',1);
	sprintf(&message[1018],"%-1s\n",Cr_Fld.MODEL_CONTROL_3_XCD);
	memset(Cr_Fld.MODEL_CONTROL_4_XCD,' ',1);
	sprintf(&message[1019],"%-1s\n",Cr_Fld.MODEL_CONTROL_4_XCD);

	/*if(strcmp(Auth_Tx.CAF01_details.expiration_date_check,"Y") == 0)
	{
		memcpy(Cr_Fld.AUTHORIZATION_EXPIRE_DT_XFLG,"V",1);
		sprintf(&message[1020],"%-1s\n",Cr_Fld.AUTHORIZATION_EXPIRE_DT_XFLG);
	}
	else if(strcmp(Auth_Tx.CAF01_details.expiration_date_check,"N") == 0)
	{
		memcpy(Cr_Fld.AUTHORIZATION_EXPIRE_DT_XFLG,"I",1);
		sprintf(&message[1020],"%-1s\n",Cr_Fld.AUTHORIZATION_EXPIRE_DT_XFLG);
	}
	else if(Auth_Tx.CAF01_details.expiration_date_check == 0)
	{
		memset(Cr_Fld.AUTHORIZATION_EXPIRE_DT_XFLG,' ',1);
		sprintf(&message[1020],"%-1s\n",Cr_Fld.AUTHORIZATION_EXPIRE_DT_XFLG);
	}*/

	if(strcmp(Auth_Tx.TLF01_details.response_code,"54") == 0)
	{
		strncpy(Cr_Fld.AUTHORIZATION_EXPIRE_DT_XFLG,"I",1);
	}
	else
	{
		strncpy(Cr_Fld.AUTHORIZATION_EXPIRE_DT_XFLG,"V",1);
	}

	sprintf(&message[1020],"%-1s\n",Cr_Fld.AUTHORIZATION_EXPIRE_DT_XFLG);


	/**************memset(Cr_Fld.AUTH_SECONDARY_VERIFY_XCD,"V",1);************/

	memcpy(Cr_Fld.AUTH_SECONDARY_VERIFY_XCD," ",1);


	sprintf(&message[1021],"%-1s\n",Cr_Fld.AUTH_SECONDARY_VERIFY_XCD);
	strncpy(Cr_Fld.AUTHORIZATION_RESPONSE_XCD," ",1);
	sprintf(&message[1022],"%s",Cr_Fld.AUTHORIZATION_RESPONSE_XCD);


}

void EndProcess()
{
      CHAR Buffer[100] = "";

   sprintf( Buffer, "Shutting down %s Service, version %d",
            ServiceName, Version );
   LogEvent( Buffer, INFO_MSG );
   strcat( Buffer, "\n" );
   pteipc_shutdown_multiple_instance_app();
   //PRINT( Buffer );


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

	

   MessageType = ptemsg_get_msg_type (p_cur_msg) ;
    switch (MessageType)
   {
   /*case MT_GET_STATS:
   	send_transaction_statistics( p_msg_in );
   	break;*/
   case MT_AUTH_REQUEST:
					RqMssg(p_cur_msg);
					msglen=strlen(message);
					/*sprintf(msgBuffer,"count = %d, msglen:%dapplication_que_name:%scontrol_que_name:%smessage:%s",count, msglen,application_que_name,control_que_name,message);
					sprintf(msgBuffer,"msglen:%dmessage:%s",msglen,message);
					sprintf(msgBuffer,"message:%s",message);
					LogEvent( msgBuffer, INFO_MSG );
					fprintf (fp1, "count:%dmsglen:%d message:%s\n",count,msglen,message);*/
		
						/*if (!fp1) 
							{
								sprintf(fileinfo,"There was a problem opening %s for writing\n",outname);
								LogEvent( fileinfo, INFO_MSG );
							}
						else 
							{*/
								 /*****if(tracing == 1)
								{
									fprintf (fp1, "count:%d message:%s\n",count,message);
								}*****/
								 //sprintf(fileinfo,"There was a problem opening %d for writing\n",name);
								//fprintf (fp1, "%s\n", fileinfo);
								 //fclose(fp);
							//}
						 
					
						 /*else if(lReturnCode == QUE_ER_TIMEOUT)
							{
							sprintf( strBuffer,"Error:Timed Out on Receive,  error code: %ld",strTemp, lReturnCode );
								 LogEvent( strBuffer, ALERT_MSG );
								 memcpy(message,strBuffer,sizeof(strBuffer));
							}*/
			
         break;

     /* case MT_AUTH_RESPONSE: 

         ReturnCode = TxService_Process_Auth_Response (p_cur_msg) ;
         if (ReturnCode != NO_AUTH_ERROR)
         TxService_Forward_Auth_Tx_To_Originator (Auth_Tx.TLF01_details.handler_queue, 
                                                  MT_AUTH_RESPONSE, 
                                                  0, 
                                                  0) ;
		 
      break ;	*/
		       
	
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
															"Pinged -> Falcon Service: %s, version %d",
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
											/*GetAscendentConfigDirectory(buffer);
											strcat(buffer,"properties.txt");
											read_prop=fopen(buffer,"r+");
											if(!feof(read_prop)) 
												{
													while(fgets(prop_buf, 1000, read_prop) != NULL)
													{
														if(strstr("logCreateStatus=false",prop_buf)==NULL)
														
														//if(strcmp('logCreateStatus=false',prop_temp) == NULL)
														{
															fputs("logCreateStatus=true",read_prop);
														}
														
													}
														
												}
												
											 else
												{
												  strcpy(prop_file_buff,"The properties.txt file is not present in the config folder of the ascendent");
												  LogEvent(prop_file_buff,ALERT_MSG);
												}*/


										}
							            break;

			case ST1_SYS_IPC_TRACE_OFF: if ( ST1_SYS_IPC_TRACE_OFF == ptemsg_get_msg_subtype1(p_msg_in) )
										{
											pteipc_trace_off    ();
											memset(&message,0,sizeof(message));
											memcpy(message,"101",3);
											/*GetAscendentConfigDirectory(buffer);
											strcat(buffer,"properties.txt");
											read_prop=fopen(buffer,"r+");
											if(!feof(read_prop)) 
												{
													while(fgets(prop_buf, 1000, read_prop) != NULL)
													{
														if(strstr("logCreateStatus=true",prop_buf)==NULL)
														//if(strcmp("logCreateStatus=true",prop_buf) == NULL)
														{
															fputs("logCreateStatus=false",read_prop);
														}
														
													}
														
												}
												
											 else
												{
												  strcpy(prop_file_buff,"The properties.txt file is not present in the config folder of the ascendent");
												  LogEvent(prop_file_buff,ALERT_MSG);
												}*/
										}
										break;
			default:
				break;
		}
	      
        
		

      default:
          strcpy (strTemp, "Unknown message type received in the Pinnacle Message Handler") ;
          TxUtils_Send_Msg_To_Operator 
				  (1, strTemp,
					1, ERROR_MSG,"FalconHandler.c", 4, FATAL_ERROR,0, 0, 0) ;
		break ;
    }  /* switch msg_type */
}



//int javaEndProcessSignalled = 0;
void java_signal_handler( int signal )
{
   /*printf("Falcon:Recived Signal Handler\n");*/
   memcpy(javaEndProcessSignalled,"1",1);
   sleep( 1 );
}
 

JNIEXPORT jint JNICALL Java_FalconHandler_initialize
(JNIEnv *env, jobject obj, jstring AppName, jstring SerName)
{
		 INT    ret_val = true;
		 CHAR   err_buf[100] = "";
         CHAR tf_filename [256]={0};;
	     CHAR tmpstr	   [256]={0};
   		  /*long td_val;
		  td_val=pthread_self();

		  printf("\nIntialize:%ld\n",td_val);*/
		  
		  GetAscendentConfigDirectory(tmpstr);
		  sprintf(tf_filename, "%stf.ini", tmpstr);
		  
		  /*get record length from ini file*/
	     GetPrivateProfileString(
								"FALCON",	// points to section name
								"WORKFLOW", 	 // points to key name 
								"APC80", 				 // points to default string 
								profile_str,		 // points to destination buffer 
								sizeof(profile_str) - 1,// size of destination buffer 
								tf_filename );		 // points to initialization filename
		  
		
		  memcpy(javaEndProcessSignalled,"0",1);
		 if( signal( SIGTERM, SIG_IGN ) != SIG_IGN )
		 {
			   signal( SIGTERM,java_signal_handler );
			   /*printf("Signal Handler Has Been Set\n");*/
		 }
		 EndProcessSignalled = 0;
		 CHAR strTemp [256] = {0} ;
		 memset (&Auth_Tx, 0, sizeof Auth_Tx) ;
		 memset( strAppName, 0x00, sizeof(strAppName) );
		 memset( ServiceName, 0x00, sizeof(ServiceName) );
		 const char *str;
		 const char *str1;
		 /*fp1 = fopen(fpath, "w");
		 fp2 = fopen(fpath1, "w");*/

		   
		 char temp[200]={0};
		 memset(&message,0,sizeof(message));
		 str = (*env)->GetStringUTFChars(env, AppName, NULL);
		 if (str == NULL) {
				return INTIALIZE_EXCEPTION_ERROR; /* OutOfMemoryError already thrown */
		 }
		 memcpy(strAppName,str,sizeof(str));
		 (*env)->ReleaseStringUTFChars(env, AppName, str);
		 
		 str1 = (*env)->GetStringUTFChars(env, SerName, NULL);
		 if (str1 == NULL) 
		 {
				return INTIALIZE_EXCEPTION_ERROR; /* OutOfMemoryError already thrown */
		 }
		 memcpy(ServiceName,str1,sizeof(str1));
		 (*env)->ReleaseStringUTFChars(env, SerName, str1);

		 /* Display Application version. */
		 sprintf(strTemp, "Attempting to start %s, version %d", strAppName, Version);
		 LogEvent( strTemp, INFO_MSG );
		
		 /*  Create a table to be used for shared memory between multiple
			*  instances of this  Module.  It contains three fields:
			*  # of consecutive timeouts, # of active transactions, 
			*  and the current state of the host.
		 */



		//#ifdef _DEBUG
		 if( !pteipc_init_multiple_instance_app( strAppName, ServiceName,"pte_ipc"))
		//#endif
		 {	
				sprintf (strTemp,
						  "%s: Intialize: Failed to Create XIPC queues",
						   strAppName) ;
				LogEvent(strTemp,ERROR_MSG);
				/* signal that we are ready to shut down and quit */
				//#ifdef _DEBUG
				pteipc_shutdown_multiple_instance_app();
			   // #endif
				MainProcessDone = 1 ;

				return INTIALIZE_QUEUES_ERROR;
			
		 }

		  //**** ret_val = create_shared_mem_table( err_buf );
  

		 sprintf(strTemp,
				  "Started the FalconHandler Service: %s",
				   ServiceName);
		 LogEvent( strTemp, INFO_MSG );

 return INTIALIZE_QUEUES_NO_ERROR;
}


JNIEXPORT jstring JNICALL Java_FalconHandler_sendRequestMessage
  (JNIEnv *env1, jobject obj1, jstring srvNam)
{
		LONG     lReturnCode = 0L ;
		CHAR     strTemp [256] = {0} ;
		pPTE_MSG p_msg = 0 ; 
		INT      err_ctr = 0;
		CHAR     strBuffer [256] = {0} ;
		CHAR     strVal[1]={0};
		INT      msglen1=0;
		CHAR     fileinfo[256]={0};
		CHAR      ErrorMsg[100] = "";


		/*long td_val;
		  td_val=pthread_self();
		  printf("\nSend:%ld\n",td_val);*/

		 /* long td_val;
		static int cond = 0;

		if ( !cond )
		{
		$AS
		}*/
		if(strcmp(javaEndProcessSignalled,"1") != 0)
		{
			
			/*msglen1=strlen(message);
			sprintf( strBuffer,"msglen1:%d",msglen1);
			LogEvent( strBuffer, ALERT_MSG );*/
			
			p_msg = pteipc_receive( application_que_name, control_que_name,5, &lReturnCode);
			
			if(p_msg != NULL_PTR)
			{
					count++;
					err_ctr = 0;
					PinnacleMsgHandler (p_msg);
					free(p_msg);
					//Read_Write_into_table();
			}
			else if (lReturnCode != QUE_ER_TIMEOUT)
			{

			 
					pteipc_get_errormsg(lReturnCode, strTemp);
					 sprintf( strBuffer,
							 "Error on pteipc_receive OR Error in Receving Message %s , error code: %ld",
							  strTemp, lReturnCode );
					 LogEvent( strBuffer, ALERT_MSG );
					 memcpy(message,strBuffer,sizeof(strBuffer));
					  /* There is a problem with the queue.  Delay 1 second
					  * so as to not flood Applnk's queue and syslog.
					  */
					/* #ifdef WIN32
						Sleep( 1000 );
					 #else
						usleep( 1000000 );
					 #endif*/


					 /* Inform Operators if service's queue is no longer readable. */
						if ( ++err_ctr > 10 )
						{
								err_ctr = 0;
								 sprintf( strBuffer,
									   "%s. UNABLE TO ACCESS QUEUE!!!!",
									 ServiceName );
								LogEvent( strBuffer, ALERT_MSG );
							 /*#ifdef WIN32
							   Sleep (5000);
							#else
							   usleep (5000000);
							#endif*/
						
						}
			}
			else
			{
				
				memset(&message,0,sizeof(message));

			}
			
			//pteipc_xipclogout();
				/* Update transaction statistics */
           //**** update_txn_stats(&Txn_Counts,&Auth_Tx,SENT );
			return (*env1)->NewStringUTF(env1,message);
	   }
	   else
	   {
		
			/* Shutting down, need to change state before exiting Xipc. */
			//****(void)set_host_state( DOWN, ErrorMsg );
			pteipc_shutdown_multiple_instance_app();
			return (*env1)->NewStringUTF(env1,javaEndProcessSignalled);
	   }

}

JNIEXPORT void JNICALL Java_FalconHandler_receiveResponseMsg
 (JNIEnv *env2, jobject obj2,jstring respmsg)
 {
			 CHAR respbuffer[5000]={0};
			 CHAR respn_msg[5000]={0};
			 CHAR strTemp [256] = {0} ;
			 INT      msglen=0;
			 const char *str2;
	         CHAR      ErrorMsg[100] = "";
			/*long td_val;
		  td_val=pthread_self();
		  printf("\nReceive:%ld\n",td_val);*/

			if(strcmp(javaEndProcessSignalled,"1") != 0)
			{
				str2 = (*env2)->GetStringUTFChars(env2, respmsg, NULL);
				if (str2 == NULL) 
				{
					strcpy (strTemp,"receiveResponseMsg:Parameter passed from Java is NULL");
					LogEvent(strTemp,ERROR_MSG);
				}
				memcpy(respn_msg,str2,sizeof(str2));
				msglen=strlen(str2);
			
				sprintf(respbuffer,"msglen:%drespn_msg:%s",msglen,str2);
				LogEvent( respbuffer, INFO_MSG );
				(*env2)->ReleaseStringUTFChars(env2, respmsg, str2);
			
				/* Update transaction statistics. */
			 //**** update_txn_stats( &Txn_Counts, (pAUTH_TX)NULL_PTR, RECEIVED );

				/*static long count = 0;
				count++;
				fprintf (fp2, "count:%d respbuffer:%s\n",count,respbuffer);*/

		  }
		  else
		  {	
				 /* Shutting down, need to change state before exiting Xipc. */
				//****(void)set_host_state( DOWN, ErrorMsg );
				pteipc_shutdown_multiple_instance_app();
		  }
	
 }
 
 JNIEXPORT void JNICALL Java_FalconHandler_terminate
  (JNIEnv *env3, jobject obj3)
{
		   /*long td_val;
		  td_val=pthread_self();
		  printf("\n%ld\n",td_val);*/

		  CHAR strTemp [256] = {0} ;	
		  CHAR SerName[20]={0};
		  strcpy(SerName,"FalconHandler");
		  sprintf( strTemp,
				   "Shutting down the FalconHandler Service: %s",
					SerName);
		  LogEvent( strTemp, INFO_MSG );

		   /* Log out of XIPC */
		 // #ifdef _DEBUG
		  pteipc_shutdown_multiple_instance_app();
		 //#endif
		 MainProcessDone = 1 ;	  
}

JNIEXPORT jint JNICALL Java_FalconHandler_getLibFileVersion
(JNIEnv *env4, jobject obj4)
{
	 CHAR ver_buf[5]={0};
	 sprintf(ver_buf,"Falcon_Version:%d",Version);
	 LogEvent(ver_buf,INFO_MSG);
	 //return (*env4)->NewStringUTF(env4,Version);
	//return(Version);
		return(1);
 
 }


