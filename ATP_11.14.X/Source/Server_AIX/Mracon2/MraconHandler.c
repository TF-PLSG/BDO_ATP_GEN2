/*
 * MraconHandler.c
 *
 *  Created on: Jan 31, 2018
 *      Author: 102838
 */



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
#include "MraconHandler.h"
#include "nc_dbstruct.h"
#include "ptetime.h"
#include "Mracon.h"
#include "global_definitions.h"


/*#ifdef	_DEBUG
	#define PRINT(x) printf(x)
#else
	#define PRINT
#endif */

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

CHAR mracon_que_name  [MAX_QUE_NAME_SIZE+1] = "mraconA" ;
CHAR   netds_que_name [] = "netdsA";
CHAR   updatds_que_name[] = "updatdsA";
CHAR   	fconds_que_name[] 						= "fcondsA";
BYTE p_buffer [MAX_DATASERVER_BUFFER_LENGTH] ;

static long int txn_cnt = 1;

CHAR  mracon_Erro_warning_Filename[256] = {0};
CHAR  mracon_module_error_warning_file_name[256] = {0};
CHAR  mracon_error_warning_file_path[256] = {0};
UINT Max_File_Size_Defined = 0 ;
CHAR  App_Name[7] = "mracon";


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


#define WORKFLOW_XCD_SIZE 16
#define RECORD_TYPE_XCD_SIZE 8
#define DATA_SPECIFICATION_VERSION_XCD_SIZE 5
#define CLIENT_XID_SIZE 16

#define INTIALIZE_QUEUES_NO_ERROR 00
#define INTIALIZE_QUEUES_ERROR 01
#define INTIALIZE_EXCEPTION_ERROR 02
#define MAX_INSTANCE_NAME_SIZE   51

CHAR strAppName [80];
CHAR ServiceName[80];
AUTH_TX     Auth_Tx;
pAUTH_TX auth_Tx;
char javaEndProcessSignalled[1];

/*INT Version =19; TF_veena version Intial */
CHAR Version[] = "ATP_11.6.0";
static CHAR message[1023]= {0};
static long count = 0;
INT  MainProcessDone;
INT  EndProcessSignalled;
CHAR  date_str [ 9] = "";
CHAR  time_str [ 7] = "";
CHAR  time_date[25] = "";
CHAR  time_milliseconds[4] = "";
CHAR profile_str [17]="";
char temp_iso_ration[25] = {0};
double   conv_rate   = 0.0;


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

	/*CRTRANS_FIELDS Cr_Fld;*/
	CHAR tem_date[2]={0};
	CHAR tem_month[3]={0};
	CHAR tem_year[4]={0};
	CHAR temp_exp[9]={0};
	UINT  item_month=0;
	UINT  item_year=0;
	UINT  icvc_data=0;
	char dollar_peso[8]="0.023889";
	char temp_totalamt[14]={0};

	pBYTE pATEMessageDataData = 0 ;
	pPTE_MSG_DATA pATEMessageData = 0 ;
	CHAR resp_buf[15] = {0} ;
	memset (&Auth_Tx, 0, sizeof (AUTH_TX)) ;
	memset(&Cr_Fld,0,sizeof(Cr_Fld));
	memset(&message,0,sizeof(message));

	int i =0;
	int len=0;
	char msg[5]={0};
	CHAR err_buf[100]  = {0};

	double temp_iso = 0.0;/*double temp_conv_amount;*/
	CRF01  crf01 = {0} ;  LONG  lReturnCode;
	/*char temp_without_decimal[13]="";*/

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

	MRACON_read_CUP_merchant_indicator();
	MRACON_read_CUP_MP_Bin_range_1_indicator();
	MRACON_read_CUP_MP_Bin_range_2_indicator();
	MRACON_read_CUP_MP_Bin_range_3_indicator();
	MRACON_read_CUP_MP_Bin_range_4_indicator();
	MRACON_read_CUP_MP_Bin_range_5_indicator();
	MRACON_read_CUP_MP_Bin_range_len_1();
	MRACON_read_CUP_MP_Bin_range_len_2();
	MRACON_read_CUP_MP_Bin_range_len_3();
	MRACON_read_CUP_MP_Bin_range_len_4();
	MRACON_read_CUP_MP_Bin_range_len_5();

	MRACON_build_request_HeaderFile();
	MRACON_build_request_field_1( );
	MRACON_build_request_field_2( );
	MRACON_build_request_field_3( );
	MRACON_build_request_field_4( );
	MRACON_build_request_field_5( );
	MRACON_build_request_field_6( );
	MRACON_build_request_field_7( );
	MRACON_build_request_field_8( );
	MRACON_build_request_field_9( );
	MRACON_build_request_field_10( );
	MRACON_build_request_field_11( );
	MRACON_build_request_field_12( );
	MRACON_build_request_field_13( );
	MRACON_build_request_field_14( );
	MRACON_build_request_field_15( );
	MRACON_build_request_field_16( );
	MRACON_build_request_field_17( );
	MRACON_build_request_field_18( );
	MRACON_build_request_field_19( );
	MRACON_build_request_field_20( );
	MRACON_build_request_field_21( );
	MRACON_build_request_field_22( );
	MRACON_build_request_field_23( );
	MRACON_build_request_field_24( );
	MRACON_build_request_field_25( );
	MRACON_build_request_field_26( );
	MRACON_build_request_field_27( );
	MRACON_build_request_field_28( );
	MRACON_build_request_field_29( );
	MRACON_build_request_field_30( );
	MRACON_build_request_field_31( );
	MRACON_build_request_field_32( );
	MRACON_build_request_field_33( );
	MRACON_build_request_field_34( );
	MRACON_build_request_field_35( );
	MRACON_build_request_field_36( );
	MRACON_build_request_field_37();
	MRACON_build_request_field_38( );
	MRACON_build_request_field_39( );
	MRACON_build_request_field_40( );
	MRACON_build_request_field_41( );
	MRACON_build_request_field_42( );
	MRACON_build_request_field_43( );
	MRACON_build_request_field_44( );
	MRACON_build_request_field_45( );
	MRACON_build_request_field_46( );
	MRACON_build_request_field_47( );
	MRACON_build_request_field_48( );
	MRACON_build_request_field_49( );
	MRACON_build_request_field_50( );
	MRACON_build_request_field_51( );
	MRACON_build_request_field_52( );
	MRACON_build_request_field_53( );
	MRACON_build_request_field_54( );
	MRACON_build_request_field_55( );
	MRACON_build_request_field_56( );
	MRACON_build_request_field_57( );
	MRACON_build_request_field_58( );
	MRACON_build_request_field_59( );
	MRACON_build_request_field_60( );
	MRACON_build_request_field_61( );
	MRACON_build_request_field_62( );
	MRACON_build_request_field_63( );
	MRACON_build_request_field_64( );
	MRACON_build_request_field_65( );
	MRACON_build_request_field_66( );
	MRACON_build_request_field_67( );
	MRACON_build_request_field_68( );
	MRACON_build_request_field_69( );
	MRACON_build_request_field_70( );
	MRACON_build_request_field_71( );
	MRACON_build_request_field_72( );
	MRACON_build_request_field_73( );
	MRACON_build_request_field_74( );
	MRACON_build_request_field_75( );
	MRACON_build_request_field_76( );
	MRACON_build_request_field_77( );
	MRACON_build_request_field_78( );
	MRACON_build_request_field_79( );
	MRACON_build_request_field_80( );
	MRACON_build_request_field_81( );
	MRACON_build_request_field_82( );
	MRACON_build_request_field_83( );
	MRACON_build_request_field_84( );
	MRACON_build_request_field_85( );
	MRACON_build_request_field_86( );
	MRACON_build_request_field_87( );
	MRACON_build_request_field_88( );
	MRACON_build_request_field_89( );
	MRACON_build_request_field_90( );
	MRACON_build_request_field_91( );
	MRACON_build_request_field_92( );
	MRACON_build_request_field_93( );
	MRACON_build_request_field_94( );
	MRACON_build_request_field_95( );
	MRACON_build_request_field_96( );
	MRACON_build_request_field_97( );
	MRACON_build_request_field_98( );
	MRACON_build_request_field_99( );
	MRACON_build_request_field_100( );
	MRACON_build_request_field_101( );
	MRACON_build_request_field_102( );
	MRACON_build_request_field_103( );
	MRACON_build_request_field_104( );
	MRACON_build_request_field_105( );
	MRACON_build_request_field_106( );
	MRACON_build_request_field_107( );
	MRACON_build_request_field_108( );
	MRACON_build_request_field_109( );
	MRACON_build_request_field_110( );
	MRACON_build_request_field_111( );
	MRACON_build_request_field_112( );
	MRACON_build_request_field_113( );
	MRACON_build_request_field_114( );
	MRACON_build_request_field_115( );
	MRACON_build_request_field_116( );
	MRACON_build_request_field_117( );
	MRACON_build_request_field_118( );
	MRACON_build_request_field_119( );
	MRACON_build_request_field_120( );
	MRACON_build_request_field_121( );
	MRACON_build_request_field_122( );
	MRACON_build_request_field_123( );
	MRACON_build_request_field_124( );
	MRACON_build_request_field_125( );
	MRACON_build_request_field_126( );
	MRACON_build_request_field_127( );
	MRACON_build_request_field_128( );
	MRACON_build_request_field_129( );
	MRACON_build_request_field_130( );
	MRACON_build_request_field_131( );
	MRACON_build_request_field_132( );
	MRACON_build_request_field_133( );
	MRACON_build_request_field_134( );
	MRACON_build_request_field_135( );
	MRACON_build_request_field_136( );
	MRACON_build_request_field_137( );
	MRACON_build_request_field_138( );
	MRACON_build_request_field_139( );
	MRACON_build_request_field_140( );
	MRACON_build_request_field_141( );
	MRACON_build_request_field_142( );
	MRACON_build_request_field_143( );
	MRACON_build_request_field_144();

}

void MRACON_build_request_HeaderFile()
{
	memcpy(Cr_Fld.HeaderFile,"000009710000000000102tdriver   PMAX      0000000000                  ",69);
	sprintf(message,"%-69s\n",Cr_Fld.HeaderFile);
}

void MRACON_build_request_field_1()
{
	memcpy(Cr_Fld.WORKFLOW_XCD,profile_str,MRACON_FIELD1_SIZE);
	sprintf(&message[69],"%-16s\n",Cr_Fld.WORKFLOW_XCD);
}

void MRACON_build_request_field_2( )
{
	memcpy(Cr_Fld.RECORD_TYPE_XCD,"CRTRAN24",MRACON_FIELD2_SIZE);
	sprintf(&message[85],"%-8s\n",Cr_Fld.RECORD_TYPE_XCD);
}

void MRACON_build_request_field_3( )
{
	memcpy(Cr_Fld.DATA_SPECIFICATION_VERSION_XCD,"2.4",MRACON_FIELD3_SIZE);
	sprintf(&message[93],"%-5s\n",Cr_Fld.DATA_SPECIFICATION_VERSION_XCD);
}

void MRACON_build_request_field_4( )
{
	memcpy(Cr_Fld.CLIENT_XID,"DT_BDO_MR",MRACON_FIELD4_SIZE);
	sprintf(&message[98],"%-16s\n",Cr_Fld.CLIENT_XID);
}

void MRACON_build_request_field_5( )
{
	memcpy(Cr_Fld.RCD_CRT_DATE,date_str,MRACON_FIELD5_SIZE);
	sprintf(&message[114],"%-8s\n",Cr_Fld.RCD_CRT_DATE);
}

void MRACON_build_request_field_6( )
{
	memcpy(Cr_Fld.RCD_CRT_TIME,time_str,MRACON_FIELD6_SIZE);
	sprintf(&message[122],"%-6s\n",Cr_Fld.RCD_CRT_TIME);
}

void MRACON_build_request_field_7( )
{
	memcpy(Cr_Fld.RCD_CRT_MSCND,time_milliseconds,MRACON_FIELD7_SIZE);
	sprintf(&message[128],"%-3s\n",Cr_Fld.RCD_CRT_MSCND);
}

void MRACON_build_request_field_8( )
{
	memcpy(Cr_Fld.GMT_OFFSET,"8.00",MRACON_FIELD8_SIZE);
	sprintf(&message[131],"%-6s\n",Cr_Fld.GMT_OFFSET);
}

void MRACON_build_request_field_9( )
{
	if(MRACON_check_If_CUP_Transaction ()  == true)
	{
		memcpy(Cr_Fld.CUSTOMER_XID,Auth_Tx.TLF01_details.visa_merchant_id,MRACON_FIELD9_SIZE);
	}
	else
	{
		memcpy(Cr_Fld.CUSTOMER_XID,Auth_Tx.TLF01_details.merchant_id,MRACON_FIELD9_SIZE);
	}
	sprintf(&message[137],"%-20s\n",Cr_Fld.CUSTOMER_XID);
}

void MRACON_build_request_field_10( )
{
	/*char temp_merchant_id_Card_No[40] = {0};
	strncpy(temp_merchant_id_Card_No,Auth_Tx.TLF01_details.merchant_id,MRACON_FIELD10_MERCHANT_ID_SIZE);
	strncat(temp_merchant_id_Card_No, Auth_Tx.TLF01_details.card_num, MRACON_FIELD10_CARD_NUM_SIZE);
	memcpy(Cr_Fld.ACCOUNT_REFERENCE_XID,temp_merchant_id_Card_No,MRACON_FIELD10_SIZE);*/
	/* Commented and set it as blank as per FICO Requirement */
	memset(Cr_Fld.ACCOUNT_REFERENCE_XID,SPACES,MRACON_FIELD10_SIZE);
	sprintf(&message[157],"%-40s\n",Cr_Fld.ACCOUNT_REFERENCE_XID);
}

void MRACON_build_request_field_11( )
{
	char temp_Tran_id[33]={0};
	strncpy(temp_Tran_id,Auth_Tx.TLF01_details.primary_key.transaction_id,20);
	strncat(temp_Tran_id,Auth_Tx.TLF01_details.retrieval_ref_num,12);
	strncpy(Cr_Fld.EXTERNAL_TRANSACTION_XID,temp_Tran_id,MRACON_FIELD11_SIZE);
	sprintf(&message[197],"%-32s\n",Cr_Fld.EXTERNAL_TRANSACTION_XID);
}

void MRACON_build_request_field_12( )
{
	if(MRACON_check_If_CUP_Transaction ()  == true)
	{
		memcpy(Cr_Fld.SCORE_CUSTOMER_ACCOUNT_XID,Auth_Tx.TLF01_details.visa_merchant_id,MERCHANT_ID_SIZE);
	}
	else
	{
		memcpy(Cr_Fld.SCORE_CUSTOMER_ACCOUNT_XID,Auth_Tx.TLF01_details.merchant_id,MERCHANT_ID_SIZE);
	}
	sprintf(&message[229],"%-19s\n",Cr_Fld.SCORE_CUSTOMER_ACCOUNT_XID);
}

void MRACON_build_request_field_13( )
{
	memcpy(Cr_Fld.AUTHORIZATION_POSTING_XCD,"A",MRACON_FIELD13_SIZE);
	sprintf(&message[248],"%-1s\n",Cr_Fld.AUTHORIZATION_POSTING_XCD);
}

void MRACON_build_request_field_14( )
{
	memset(Cr_Fld.CARD_POSTAL_XCD,SPACES,MRACON_FIELD14_SIZE);
	sprintf(&message[249],"%-9s\n",Cr_Fld.CARD_POSTAL_XCD);
}

void MRACON_build_request_field_15( )
{
	memset(Cr_Fld.CARD_COUNTRY_XCD,SPACES,MRACON_FIELD15_SIZE);
	sprintf(&message[258],"%-3s\n",Cr_Fld.CARD_COUNTRY_XCD);
}

void MRACON_build_request_field_16( )
{
	memset(Cr_Fld.CARD_OPEN_DT,SPACES,MRACON_FIELD16_SIZE);
	sprintf(&message[261],"%-8s\n",Cr_Fld.CARD_OPEN_DT);
}

void MRACON_build_request_field_17( )
{
	memset(Cr_Fld.CARD_ISSUE_DT,SPACES,MRACON_FIELD17_SIZE);
	sprintf(&message[269],"%-8s\n",Cr_Fld.CARD_ISSUE_DT);
}

void MRACON_build_request_field_18( )
{
	memset(Cr_Fld.CARD_ISSUE_TYPE_XCD,SPACES,MRACON_FIELD18_SIZE);
	sprintf(&message[277],"%-1s\n",Cr_Fld.CARD_ISSUE_TYPE_XCD);
}

void MRACON_build_request_field_19( )
{
	memset(Cr_Fld.ACCOUNT_EXPIRATION_DT,SPACES,MRACON_FIELD19_SIZE);
	sprintf(&message[278],"%-8s\n",Cr_Fld.ACCOUNT_EXPIRATION_DT);
}

void MRACON_build_request_field_20( )
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

	strncpy(Cr_Fld.CARD_EXPIRATION_DT,temp_exp,MRACON_FIELD20_SIZE);
	sprintf(&message[286],"%-8s\n",Cr_Fld.CARD_EXPIRATION_DT);
}


void MRACON_build_request_field_21( )
{
	memset(Cr_Fld.PRIOR_CREDIT_LIMIT_AMT,SPACES,MRACON_FIELD21_SIZE );
	sprintf(&message[294],"%-10s\n",Cr_Fld.PRIOR_CREDIT_LIMIT_AMT);
}

void MRACON_build_request_field_22( )
{
	memset(Cr_Fld.CARD_CREDIT_LIMIT_AMT,SPACES,MRACON_FIELD22_SIZE);
	sprintf(&message[304],"%-10s\n",Cr_Fld.CARD_CREDIT_LIMIT_AMT);
}

void MRACON_build_request_field_23( )
{
	memset(Cr_Fld.CUSTOMER_GENDER_XCD,SPACES,MRACON_FIELD23_SIZE);
	sprintf(&message[314],"%-1s\n",Cr_Fld.CUSTOMER_GENDER_XCD);
}

void MRACON_build_request_field_24( )
{
	memset(Cr_Fld.CUSTOMER_BIRTH_DT,SPACES,MRACON_FIELD24_SIZE);
	sprintf(&message[315],"%-8s\n",Cr_Fld.CUSTOMER_BIRTH_DT);
}

void MRACON_build_request_field_25( )
{
	memset(Cr_Fld.CUSTOMER_CARD_CNT,SPACES,MRACON_FIELD25_SIZE);
	sprintf(&message[323],"%-3s\n",Cr_Fld.CUSTOMER_CARD_CNT);
}

void MRACON_build_request_field_26( )
{
	memset(Cr_Fld.INCOME_AMT,SPACES,MRACON_FIELD26_SIZE);
	sprintf(&message[326],"%-10s\n",Cr_Fld.INCOME_AMT);
}

void MRACON_build_request_field_27( )
{
	memset(Cr_Fld.CARD_TYPE_XCD,SPACES,MRACON_FIELD27_SIZE);
	sprintf(&message[336],"%-1s\n",Cr_Fld.CARD_TYPE_XCD);
}

void MRACON_build_request_field_28( )
{
	memset(Cr_Fld.PRIMARY_CARD_USE_XCD,SPACES,MRACON_FIELD28_SIZE);
	sprintf(&message[337],"%-1s\n",Cr_Fld.PRIMARY_CARD_USE_XCD);
}

void MRACON_build_request_field_29( )
{
	memcpy(Cr_Fld.TRANSACTION_DATE,Auth_Tx.TLF01_details.date_yyyymmdd,MRACON_FIELD29_SIZE);
	sprintf(&message[338],"%-8s\n",Cr_Fld.TRANSACTION_DATE);
}

void MRACON_build_request_field_30( )
{
	memcpy(Cr_Fld.TRANSACTION_TIME,Auth_Tx.TLF01_details.time_hhmmss,MRACON_FIELD30_SIZE);
	sprintf(&message[346],"%-6s\n",Cr_Fld.TRANSACTION_TIME);
}

void MRACON_build_request_field_31( )
{
	double temp_conv_amount = 0.0;
    char temp_without_decimal[13] = {0};
    int len_billing_amount = 0;
    double billing_amount = 0.0;
    double settlement_total = 0.0;
    char last2_decimal[3]={0};
    char amount_digit[11]= {0};
    char str_temp_amount[13] = {0};
    double temp_amount = 0.0;

    len_billing_amount = strlen(Auth_Tx.ch_billing_amt);

    if(len_billing_amount == 0)
    {
    	billing_amount = atoi(Auth_Tx.TLF01_details.total_amount);
    	len_billing_amount = strlen(Auth_Tx.TLF01_details.total_amount);
    	strncpy(amount_digit,Auth_Tx.TLF01_details.total_amount,10);
    	strncpy(last2_decimal,Auth_Tx.TLF01_details.total_amount+10,2);
    	/* billing_amount = (strtod((char *)amount_digit,0)); */
    	billing_amount = (strtod(Auth_Tx.TLF01_details.total_amount, 0));
    }
    else
    {
    	  strncpy(amount_digit,Auth_Tx.ch_billing_amt,10);
    	  strncpy(last2_decimal,Auth_Tx.ch_billing_amt+10,2);
    	  /* billing_amount = (strtod(amount_digit,0)); */
    	  billing_amount = (strtod(Auth_Tx.ch_billing_amt, 0));
    }

    if((MRACON_check_If_MC_Transaction ()    == true)||
    	(MRACON_check_If_VISA_Transaction () == true)||
		(MRACON_check_If_JCB_Transaction ()  == true)||
		(MRACON_check_If_CUP_Transaction ()  == true)||
		(MRACON_check_If_DINERS_Transaction()== true)||
		(MRACON_check_If_Fleet_Transaction() == true)||
		(MRACON_Check_If_CUP_MP_Transaction() == true)||
		 MRACON_check_If_AMEX_Transaction ()==true)
    {
    	if(strcmp(Auth_Tx.TLF01_details.currency_code,"608") == 0) /*  PESO local currecny */
    	{
    		/*temp_amount = billing_amount/100;
    		sprintf(str_temp_amount, "%lf", temp_amount);
    		strncpy(last2_decimal,str_temp_amount+10,2);
    		sprintf(Cr_Fld.TRANSACTION_AMT,"%010lf.%02lf",billing_amount/100, billing_amount%100);
    		sprintf(Cr_Fld.TRANSACTION_AMT,"%010lf.%s",billing_amount/100, last2_decimal); */
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
				if(str_temp_amount[9]=='.' ||str_temp_amount[10]=='.')
				{

				}
				else
				{
	    		strncat(Cr_Fld.TRANSACTION_AMT,".",1);
				}
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

   /* else if(MRACON_check_If_AMEX_Transaction ()==true)
    {
    	settlement_total = atoi(Auth_Tx.TLF01_details.total_amount);
    	if(strcmp(Auth_Tx.TLF01_details.currency_code,"608") == 0) 
		{
    		temp_amount = settlement_total/100;
    		sprintf(str_temp_amount, "%lf", temp_amount);
    		strncpy(last2_decimal,str_temp_amount+10,2);
			sprintf(Cr_Fld.TRANSACTION_AMT,"%010lf.%s",settlement_total/100, last2_decimal);
		}
    	else if(strcmp(Auth_Tx.TLF01_details.currency_code,"840") == 0)
		{
    		if(len_billing_amount > 0 && billing_amount > 0)
			{
				temp_conv_amount = billing_amount *conv_rate;
				temp_amount = temp_conv_amount/100;
	    		sprintf(str_temp_amount, "%lf", temp_amount);
	    		strncpy(last2_decimal,str_temp_amount+10,2);
		
				sprintf(Cr_Fld.TRANSACTION_AMT,"%010lf.%s",
						temp_conv_amount/100, last2_decimal);
			}
			else
			{
				strncpy(Cr_Fld.TRANSACTION_AMT,"0000000000.00",13);
			}
		}
    }*/

    else
    {
    	strncpy(Cr_Fld.TRANSACTION_AMT,"0000000000.00",13);
    }

    sprintf(&message[352],"%-3s\n",Cr_Fld.TRANSACTION_AMT);

}

void MRACON_build_request_field_32( )
{
	strncpy(Cr_Fld.TRANSACTION_ISO_CURRENCY_XCD,"608",MRACON_FIELD32_SIZE);
	sprintf(&message[365],"%-3s\n",Cr_Fld.TRANSACTION_ISO_CURRENCY_XCD);
}

void MRACON_build_request_field_33( )
{

	char temp_iso_ration_value[13] = {0};

	strncpy(temp_iso_ration_value,"00000",5);
	strncat(temp_iso_ration_value,temp_iso_ration,8);
	strncpy(Cr_Fld.TRAN_ISO_CURRENCY_CNVRSN_RATIO,temp_iso_ration_value,MRACON_FIELD33_SIZE);
	sprintf(&message[368],"%-13s\n",Cr_Fld.TRAN_ISO_CURRENCY_CNVRSN_RATIO);
}

void MRACON_build_request_field_34( )
{
	if( strncmp(Auth_Tx.TLF01_details.response_code,"00",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"10",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"11",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"32",2)== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"85",2)== 0 )
		{
			strncpy(Cr_Fld.DECISION_XCD,"A",MRACON_FIELD34_SIZE);
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
				strncmp(Auth_Tx.TLF01_details.response_code,"64",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"76",2)== 0 ||
				strncmp(Auth_Tx.TLF01_details.response_code,"77",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"78",2)== 0 ||
				strncmp(Auth_Tx.TLF01_details.response_code,"82",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"92",2)== 0 ||
				strncmp(Auth_Tx.TLF01_details.response_code,"N7",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"44",2)== 0)
		{
				strncpy(Cr_Fld.DECISION_XCD,"D",MRACON_FIELD34_SIZE);
		}
		else if(strncmp(Auth_Tx.TLF01_details.response_code,"04",2)== 0 ||
				strncmp(Auth_Tx.TLF01_details.response_code,"07",2)== 0 ||
				strncmp(Auth_Tx.TLF01_details.response_code,"41",2)== 0 ||
				strncmp(Auth_Tx.TLF01_details.response_code,"43",2)== 0	)
		{
			strncpy(Cr_Fld.DECISION_XCD,"P",MRACON_FIELD34_SIZE);
		}
		else if(strncmp(Auth_Tx.TLF01_details.response_code,"01",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"02",2)== 0 ||
				strncmp(Auth_Tx.TLF01_details.response_code,"08",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"34",2)== 0 ||
				strncmp(Auth_Tx.TLF01_details.response_code,"35",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"36",2)== 0 ||
				strncmp(Auth_Tx.TLF01_details.response_code,"37",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"38",2)== 0 ||
				strncmp(Auth_Tx.TLF01_details.response_code,"60",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"62",2)== 0 ||
				strncmp(Auth_Tx.TLF01_details.response_code,"65",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"66",2)== 0 ||
		strncmp(Auth_Tx.TLF01_details.response_code,"75",2)== 0	)
		{
			strncpy(Cr_Fld.DECISION_XCD,"R",MRACON_FIELD34_SIZE);
		}
		else
		{
			strncpy(Cr_Fld.DECISION_XCD," ",MRACON_FIELD34_SIZE);
		}

		sprintf(&message[381],"%-1s\n",Cr_Fld.DECISION_XCD);
}

void MRACON_build_request_field_35( )
{

		if( Auth_Tx.TLF01_details.tx_key == AUTH_SALE          || Auth_Tx.TLF01_details.tx_key == AUTH_SALE_RESPONSE ||
			Auth_Tx.TLF01_details.tx_key == AUTH_AUTHORIZATION || Auth_Tx.TLF01_details.tx_key == AUTH_AUTHORIZATION_RESPONSE||
			Auth_Tx.TLF01_details.tx_key == AUTH_ADVICE        || Auth_Tx.TLF01_details.tx_key == AUTH_ADVICE_RESPONSE||
			Auth_Tx.TLF01_details.tx_key == AUTH_SALES_COMPLETION || Auth_Tx.TLF01_details.tx_key == AUTH_SALES_COMPLETION_RESPONSE ||
			Auth_Tx.TLF01_details.tx_key == AUTH_SALE_ADJUSTMENT || Auth_Tx.TLF01_details.tx_key == AUTH_SALE_ADJUSTMENT_RESPONSE||
			Auth_Tx.TLF01_details.tx_key == AUTH_OFFLINE_SALE ||Auth_Tx.TLF01_details.tx_key == AUTH_OFFLINE_SALE_RESPONSE||
			Auth_Tx.TLF01_details.tx_key == AUTH_SALE_CASH || Auth_Tx.TLF01_details.tx_key == AUTH_SALE_CASH_RESPONSE||
			Auth_Tx.TLF01_details.tx_key == AUTH_AUTHORIZATION_OVERRIDE || Auth_Tx.TLF01_details.tx_key == AUTH_AUTHORIZATION_OVERRIDE_RESPONSE||
			Auth_Tx.TLF01_details.tx_key == AUTH_PAYMENT || Auth_Tx.TLF01_details.tx_key == AUTH_PAYMENT_RESPONSE ||
			Auth_Tx.TLF01_details.tx_key == AUTH_SALE_CASH || Auth_Tx.TLF01_details.tx_key == AUTH_SALE_CASH_RESPONSE ||
			Auth_Tx.TLF01_details.tx_key == AUTH_EMV_ADVICE || Auth_Tx.TLF01_details.tx_key == AUTH_EMV_ADVICE_RESPONSE)
		{
			strncpy(Cr_Fld.TRANSACTION_TYPE_XCD,"M",MRACON_FIELD35_SIZE);
		}
		else if(Auth_Tx.TLF01_details.tx_key == AUTH_CASH_ADVANCE || Auth_Tx.TLF01_details.tx_key == AUTH_CASH_ADVANCE_RESPONSE ||
				Auth_Tx.TLF01_details.tx_key == AUTH_QUASI_CASH || Auth_Tx.TLF01_details.tx_key == AUTH_QUASI_CASH_RESPONSE ||
				Auth_Tx.TLF01_details.tx_key == AUTH_CASH_ADVANCE_AUTHORIZATION || Auth_Tx.TLF01_details.tx_key == AUTH_CASH_ADVANCE_AUTHORIZATION_RESPONSE||
				Auth_Tx.TLF01_details.tx_key == AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE || Auth_Tx.TLF01_details.tx_key == AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE_RESPONSE)
		{/* CASH advance */
			strncpy(Cr_Fld.TRANSACTION_TYPE_XCD,"C",MRACON_FIELD35_SIZE);
		}
		else if(Auth_Tx.TLF01_details.tx_key == AUTH_REFUND ||Auth_Tx.TLF01_details.tx_key == AUTH_REFUND_RESPONSE||
				Auth_Tx.TLF01_details.tx_key == AUTH_OFFLINE_REFUND ||Auth_Tx.TLF01_details.tx_key == AUTH_OFFLINE_REFUND_RESPONSE||
				Auth_Tx.TLF01_details.tx_key == AUTH_REFUND_ADJUSTMENT || Auth_Tx.TLF01_details.tx_key == AUTH_REFUND_ADJUSTMENT_RESPONSE||
				Auth_Tx.TLF01_details.tx_key == AUTH_PAYMENT_ADJUSTMENT ||Auth_Tx.TLF01_details.tx_key == AUTH_PAYMENT_ADJUSTMENT_RESPONSE ||
				Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL ||Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_RESPONSE ||
			Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_ADVICE ||Auth_Tx.TLF01_details.tx_key == AUTH_REVERSAL_ADVICE_RESPONSE)
		{/* Reversal */
			strncpy(Cr_Fld.TRANSACTION_TYPE_XCD,"V",MRACON_FIELD35_SIZE);
		}

		else if(Auth_Tx.TLF01_details.tx_key == AUTH_VOID_CASH_ADVANCE ||Auth_Tx.TLF01_details.tx_key == AUTH_VOID_CASH_ADVANCE_RESPONSE)
		{/* Reversal */
			strncpy(Cr_Fld.TRANSACTION_TYPE_XCD,"W",MRACON_FIELD35_SIZE);
		}

		else if(Auth_Tx.TLF01_details.tx_key == AUTH_VOID_SALE || Auth_Tx.TLF01_details.tx_key == AUTH_VOID_SALE_RESPONSE ||
				Auth_Tx.TLF01_details.tx_key == AUTH_VOID_REFUND || Auth_Tx.TLF01_details.tx_key == AUTH_VOID_REFUND_RESPONSE||
				Auth_Tx.TLF01_details.tx_key == AUTH_VOID_PAYMENT || Auth_Tx.TLF01_details.tx_key == AUTH_VOID_PAYMENT_RESPONSE)
		{/* VOID */
			strncpy(Cr_Fld.TRANSACTION_TYPE_XCD,"X",MRACON_FIELD35_SIZE);
		}
		else if(Auth_Tx.TLF01_details.tx_key == AUTH_DEFERRED_PURCHASE_AUTHORIZATION || Auth_Tx.TLF01_details.tx_key == AUTH_DEFERRED_PURCHASE_AUTHORIZATION_RESPONSE||
				Auth_Tx.TLF01_details.tx_key == AUTH_DEFERRED_PURCHASE_AUTHORIZATION_OVERRIDE || Auth_Tx.TLF01_details.tx_key == AUTH_DEFERRED_PURCHASE_AUTHORIZATION_OVERRIDE_RESPONSE)

		{/* Deffered Purchase */
			strncpy(Cr_Fld.TRANSACTION_TYPE_XCD,"L",MRACON_FIELD35_SIZE);
		}
		else if(Auth_Tx.TLF01_details.tx_key == AUTH_PRE_AUTHORIZATION || Auth_Tx.TLF01_details.tx_key == AUTH_PRE_AUTHORIZATION_RESPONSE)

		{/* Pre AUTH */
			strncpy(Cr_Fld.TRANSACTION_TYPE_XCD,"P",MRACON_FIELD35_SIZE);
		}
		else if(Auth_Tx.TLF01_details.tx_key == AUTH_OFFLINE_SALE || Auth_Tx.TLF01_details.tx_key == AUTH_OFFLINE_SALE_RESPONSE)
		{/* Offline sale */
			strncpy(Cr_Fld.TRANSACTION_TYPE_XCD,"D",MRACON_FIELD35_SIZE);
		}
		else
		{
			strncpy(Cr_Fld.TRANSACTION_TYPE_XCD," ",MRACON_FIELD35_SIZE);
		}


	sprintf(&message[382],"%-1s\n",Cr_Fld.TRANSACTION_TYPE_XCD);
}

void MRACON_build_request_field_36( )
{
	memcpy(Cr_Fld.MERCHANT_CATEGORY_XCD,Auth_Tx.TLF01_details.category_code,MRACON_FIELD36_SIZE);
	sprintf(&message[383],"%-4s\n",Cr_Fld.MERCHANT_CATEGORY_XCD);
}

void MRACON_build_request_field_37()
{
	/*if(strncmp(Auth_Tx.MCF01_details.zip_code,"9999",4)== 0)
	{
		memset(Cr_Fld.MERCHANT_POSTAL_XCD,SPACES,MRACON_FIELD37_SIZE);
	}
	else
	{
		strncpy(Cr_Fld.MERCHANT_POSTAL_XCD,Auth_Tx.MCF01_details.zip_code,MRACON_FIELD37_SIZE);
	}*/

   INT  len = 0;
   INT  F43_len = 0;
   INT  city_last_position = 0;
   BYTE temp_len[3] = {0};
   ;

	memset (Cr_Fld.MERCHANT_POSTAL_XCD,0x20,MRACON_FIELD37_SIZE);

	/*Prepare Merchant Name */
	/*if(0 != strcmp(Auth_Tx.MCF01_details.name02,NULL_STR))
	{
		len = strlen(Auth_Tx.MCF01_details.name02);

		memcpy( Cr_Fld.MERCHANT_POSTAL_XCD,
				Auth_Tx.MCF01_details.name02,
				MRACON_FIELD37_SIZE);

		F43_len = len +1 ;*/

		/*Add / after merchant name
		memset( ncdci_HOST_MESSAGE.cardaccname + F43_len,0x5c,1);
		Cr_Fld.MERCHANT_POSTAL_XCD[F43_len]=0x5c;
        
		F43_len = F43_len + 1;
	} */
/*	else if( 0 != strcmp(Auth_Tx.MCF01_details.name01,NULL_STR))
	{
		len = strlen(Auth_Tx.MCF01_details.name01);

		memcpy( Cr_Fld.MERCHANT_POSTAL_XCD,
				Auth_Tx.MCF01_details.name01,
				MRACON_FIELD37_SIZE);

		F43_len = len +1 ;*/

		/*Add / after merchant name
		memset( ncdci_HOST_MESSAGE.cardaccname + F43_len,0x5c,1);
		Cr_Fld.MERCHANT_POSTAL_XCD[F43_len]=0x5c;

		F43_len = F43_len + 1;
	}*/
	/*else
	{

		sprintf( err_buf,
				 "Invalid Merchant, without Merchant name (field 43): %s,",
				 Auth_Tx.TLF01_details.merchant_id );
		return ;
	}*/


	/*Prepare street Name */
	/*else if(0 != strcmp(Auth_Tx.MCF01_details.address01,NULL_STR))
	{
		len = strlen(Auth_Tx.MCF01_details.address01);
		memcpy( Cr_Fld.MERCHANT_POSTAL_XCD + F43_len,
				Auth_Tx.MCF01_details.address01,
				len);
		F43_len = F43_len + len;

		F43_len = F43_len + 1;
		memset( ncdci_HOST_MESSAGE.cardaccname + F43_len,0x5c,1);
		ncdci_HOST_MESSAGE.cardaccname[F43_len]=0x5c;

		F43_len = F43_len + 1;
	}*/
	/*if( 0 != strcmp(Auth_Tx.MCF01_details.address02,NULL_STR))
	{
		len = strlen(Auth_Tx.MCF01_details.address02);
		memcpy( ncdci_HOST_MESSAGE.cardaccname + F43_len,
				Auth_Tx.MCF01_details.address02,
				len);
		F43_len = F43_len + len;
	}*/
	/*Add / after street name*/



	/*Prepare city Name */
/*	else if(0 != strcmp(Auth_Tx.MCF01_details.city,NULL_STR))
	{
		len = strlen(Auth_Tx.MCF01_details.city);
		memcpy( ncdci_HOST_MESSAGE.cardaccname + F43_len,
				Auth_Tx.MCF01_details.city,
				len);
		F43_len = len +1 ;*/

		/*Add / after merchant name
		memset( ncdci_HOST_MESSAGE.cardaccname + F43_len,0x5c,1);
		Cr_Fld.MERCHANT_POSTAL_XCD[F43_len]=0x5c;

		F43_len = F43_len + 1;
	}*/

	if(0 != strcmp(Auth_Tx.MCF01_details.zip_code,NULL_STR))
		{
			len = strlen(Auth_Tx.MCF01_details.zip_code);
			memcpy( Cr_Fld.MERCHANT_POSTAL_XCD,
					Auth_Tx.MCF01_details.zip_code,
					len);
		}

	sprintf(&message[387],"%-9s\n",Cr_Fld.MERCHANT_POSTAL_XCD);
}

void MRACON_build_request_field_38( )
{
	int len_curr_code, len_TLF_curr_code = 0;
	len_curr_code = strlen(Auth_Tx.MCF01_details.currency_code);
	len_TLF_curr_code = strlen(Auth_Tx.TLF01_details.currency_code);
	if(len_curr_code>0)
	{
		strncpy(Cr_Fld.MERCHANT_COUNTRY_XCD,Auth_Tx.MCF01_details.currency_code,MRACON_FIELD38_SIZE);
	}
	else if (len_TLF_curr_code > 0)
	{
		strncpy(Cr_Fld.MERCHANT_COUNTRY_XCD,Auth_Tx.TLF01_details.currency_code,MRACON_FIELD38_SIZE);
	}


	sprintf(&message[396],"%-3s\n",Cr_Fld.MERCHANT_COUNTRY_XCD);
}

void MRACON_build_request_field_39( )
{
	 INT  len=0,data_len=0;
	 CHAR temp_len[6]="";
	 BYTE temp_str[5]="";

	if (MRACON_check_If_MC_Transaction () == true )
	{
		/*if(strncmp(Auth_Tx.TLF01_details.service_code,"PV",2))
		{
			memcpy(Cr_Fld.TRANSACTION_PIN_VERIFY_XCD,"V",MRACON_FIELD39_SIZE);
		}
		else if(strncmp(Auth_Tx.TLF01_details.service_code,"PI",2))
		{
			memcpy(Cr_Fld.TRANSACTION_PIN_VERIFY_XCD,"I",MRACON_FIELD39_SIZE);
		}*/
		memcpy(Cr_Fld.TRANSACTION_PIN_VERIFY_XCD," ",MRACON_FIELD39_SIZE);

	}
	if (MRACON_check_If_VISA_Transaction () == true||
		MRACON_check_If_JCB_Transaction () == true||
		MRACON_check_If_CUP_Transaction () == true||
		MRACON_Check_If_CUP_MP_Transaction() == true||
		MRACON_check_If_Fleet_Transaction() == true||
		MRACON_check_If_DINERS_Transaction () == true)
	{
		if(strncmp(Cr_Fld.TRANSACTION_TYPE_XCD,"C",MRACON_FIELD39_SIZE)== 0)
		{
			if(strncmp(Auth_Tx.TLF01_details.response_code, "55" ,2) == 0)
			{
				memcpy(Cr_Fld.TRANSACTION_PIN_VERIFY_XCD,"I",MRACON_FIELD39_SIZE);
			}
			else
			{
				memcpy(Cr_Fld.TRANSACTION_PIN_VERIFY_XCD,"V",MRACON_FIELD39_SIZE);
			}
		}
		else
		{
				memcpy(Cr_Fld.TRANSACTION_PIN_VERIFY_XCD," ",MRACON_FIELD39_SIZE);
		}
	}
	if (MRACON_check_If_AMEX_Transaction () == true )
	{
		

				memcpy(Cr_Fld.TRANSACTION_PIN_VERIFY_XCD,"I",MRACON_FIELD39_SIZE);
	
	}

	/*if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_MASTERCARD)== 0)
	{
		if(strcmp(Auth_Tx.TLF01_details.response_code, "00") == 0)
		{
			memcpy(Cr_Fld.TRANSACTION_PIN_VERIFY_XCD,"V",MRACON_FIELD39_SIZE);
		}
		else
		{
			memcpy(Cr_Fld.TRANSACTION_PIN_VERIFY_XCD,"I",MRACON_FIELD39_SIZE);
		}

	}
	else if(strncmp(Cr_Fld.TRANSACTION_TYPE_XCD,"C",MRACON_FIELD39_SIZE)== 0)
	{
		if(strncmp(Auth_Tx.TLF01_details.response_code, "55" ,2) == 0)
		{
			memcpy(Cr_Fld.TRANSACTION_PIN_VERIFY_XCD,"I",MRACON_FIELD39_SIZE);
		}
		else
		{
			memcpy(Cr_Fld.TRANSACTION_PIN_VERIFY_XCD,"V",MRACON_FIELD39_SIZE);
		}
	}
	else
	{
			memcpy(Cr_Fld.TRANSACTION_PIN_VERIFY_XCD," ",MRACON_FIELD39_SIZE);
	}*/
	sprintf(&message[399],"%-1s\n",Cr_Fld.TRANSACTION_PIN_VERIFY_XCD);
}

void MRACON_build_request_field_40( )
{
	if (MRACON_check_If_MC_Transaction () == true )
	{
		/* mapping variable need to check*/
		if(Auth_Tx.TLF01_details.cvc == 'Y' || Auth_Tx.TLF01_details.cvc == 'N')
		{
			memcpy(Cr_Fld.CVV_VERIFY_XCD,"I",MRACON_FIELD40_SIZE);
			
		}
		else if(Auth_Tx.TLF01_details.cvc == 'M')
		{
			memcpy(Cr_Fld.CVV_VERIFY_XCD,"V",MRACON_FIELD40_SIZE);
			
		}
		else
		{
			memset(Cr_Fld.CVV_VERIFY_XCD,SPACES,MRACON_FIELD40_SIZE);
			memset(Cr_Fld.CVV2_RESPONSE_XFLG,SPACES,MRACON_FIELD40_SIZE);
		}

	}
	else if (MRACON_check_If_VISA_Transaction () == true )
	{
		/* mapping variable need to check*/

		if(Auth_Tx.TLF01_details.cvc == '1')
		{
			memcpy(Cr_Fld.CVV_VERIFY_XCD,"I",MRACON_FIELD40_SIZE);
			
		}
		else if(Auth_Tx.TLF01_details.cvc == '2' || Auth_Tx.TLF01_details.cvc == '3' )
		{
			memcpy(Cr_Fld.CVV_VERIFY_XCD,"V",MRACON_FIELD40_SIZE);
			
		}
		else
		{
			memset(Cr_Fld.CVV_VERIFY_XCD,SPACES,MRACON_FIELD40_SIZE);
			
		}
	}
	else if((MRACON_check_If_CUP_Transaction()==true)||
			  (MRACON_Check_If_CUP_MP_Transaction() == true))
	{
		if (Auth_Tx.future_use_2[22] == '1' )
		{
			memcpy(Cr_Fld.CVV_VERIFY_XCD,"V",MRACON_FIELD40_SIZE);
		}
		else if(Auth_Tx.future_use_2[23] == '2' )
		{
			memcpy(Cr_Fld.CVV_VERIFY_XCD,"I",MRACON_FIELD40_SIZE);
		}
		else if (Auth_Tx.future_use_2[24] == '3' )
		{
			memcpy(Cr_Fld.CVV_VERIFY_XCD," ",MRACON_FIELD40_SIZE);
		}

	}
	else if(MRACON_check_If_DINERS_Transaction () == true )
	{
		if( strncmp(&Auth_Tx.acct_id_1[2],"001",3) == 0 ||
		    strncmp(&Auth_Tx.acct_id_1[2],"016",3) == 0	||
			strncmp(&Auth_Tx.acct_id_1[2],"014",3) == 0||
			strncmp(&Auth_Tx.acct_id_1[2],"010",3) == 0||
					strncmp(&Auth_Tx.acct_id_1[2],"006",3) == 0)
		{
			memcpy(Cr_Fld.CVV_VERIFY_XCD,"V",MRACON_FIELD40_SIZE);
		}
		else if( strncmp(&Auth_Tx.acct_id_1[2],"002",3) == 0 ||
			     strncmp(&Auth_Tx.acct_id_1[2],"007",3) == 0 ||
				 strncmp(&Auth_Tx.acct_id_1[2],"012",3) == 0||
				 strncmp(&Auth_Tx.acct_id_1[2],"013",3) == 0||
				 strncmp(&Auth_Tx.acct_id_1[2],"015",3) == 0||
				 strncmp(&Auth_Tx.acct_id_1[2],"011",3) == 0||
				 strncmp(&Auth_Tx.acct_id_1[2],"017",3) == 0)
		{
			memcpy(Cr_Fld.CVV_VERIFY_XCD,"I",MRACON_FIELD40_SIZE);
		}
		else
		{
			memcpy(Cr_Fld.CVV_VERIFY_XCD," ",MRACON_FIELD40_SIZE);
		}
	}
	/*else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_JCB)== 0 ||
		strcmp(Auth_Tx.BIN01_details.card_family, CARD_AMEX)== 0)
	{
		memset(Cr_Fld.CVV_VERIFY_XCD,SPACES,MRACON_FIELD40_SIZE);
	}
	else
	{
		memset(Cr_Fld.CVV_VERIFY_XCD,SPACES,MRACON_FIELD40_SIZE);
	}*/

	 sprintf(&message[400],"%-1s\n",Cr_Fld.CVV_VERIFY_XCD);

}

void MRACON_build_request_field_41( )
{
	char temp_pos_entry_mode[3]="";

	if(MRACON_check_If_MC_Transaction () == true ||
		MRACON_check_If_JCB_Transaction () == true ||
		MRACON_check_If_CUP_Transaction () == true ||
		MRACON_Check_If_CUP_MP_Transaction() == true||
		MRACON_check_If_DINERS_Transaction () == true)
	{
		if(strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"007" ,3)==0)
		{
			memcpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"D",1);
		}
		else if(strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"081" ,3)==0)
		{
			memcpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"E",1);
		}
		else if(strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"080" ,3)==0)
		{
			memcpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"F",1);
		}
		else if(strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"009" ,3)==0)
		{
			memcpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"G",1);
		}
		else if(strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"001" ,3)==0)
		{
			memcpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"K",1);
		}
		else if(strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"002" ,3)==0)
		{
			memcpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"U",1);
		}
		else if(strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"005" ,3)==0)
		{
			memcpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"V",1);
		}
		else if(strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"000" ,3)==0)
		{
			memcpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG," ",1);
		}
	}
	else if(MRACON_check_If_VISA_Transaction () == true ||
			MRACON_check_If_AMEX_Transaction () == true)
	{
		if(strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"007" ,3)==0)
		{
			memcpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"D",1);
		}
		else if(strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"081" ,3)==0 ||
				strncmp(Auth_Tx.TLF01_details.pos_condition_code,"59" ,2)==0)
		{
			memcpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"E",1);
		}
		else if(strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"080" ,3)==0)
		{
			memcpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"F",1);
		}
		else if(strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"009" ,3)==0)
		{
			memcpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"G",1);
		}
		else if(strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"001" ,3)==0)
		{
			memcpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"K",1);
		}
		else if(strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"002" ,3)==0)
		{
			memcpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"U",1);
		}
		else if(strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"005" ,3)==0)
		{
			memcpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"V",1);
		}
		else if(strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"000" ,3)==0)
		{
			memcpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG," ",1);
		}
	}

	
	sprintf(&message[401],"%-1s\n",Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG);
}

void MRACON_build_request_field_42( )
{
	/*if(Cr_Fld.AUTHORIZATION_POSTING_XCD == "P")
	{
		memcpy(Cr_Fld.TRANSACTION_POSTING_DT,Auth_Tx.TLF01_details.date_yyyymmdd,MRACON_FIELD42_SIZE);
		sprintf(&message[402],"%-8s\n",Cr_Fld.TRANSACTION_POSTING_DT);
	}
	else*/
	{
		memset(Cr_Fld.TRANSACTION_POSTING_DT,SPACES,MRACON_FIELD42_SIZE);
		sprintf(&message[402],"%-8s\n",Cr_Fld.TRANSACTION_POSTING_DT);
	}
}

void MRACON_build_request_field_43( )
{
	memset(Cr_Fld.AUTHORIZATION_POSTING_MIS_XCD,SPACES,MRACON_FIELD43_SIZE);
	sprintf(&message[410],"%-1s\n",Cr_Fld.AUTHORIZATION_POSTING_MIS_XCD);
}

void MRACON_build_request_field_44( )
{
	memset(Cr_Fld.TRANS_POSTING_MISMATCH_XFLG,SPACES,MRACON_FIELD44_SIZE);
	sprintf(&message[411],"%-1s\n",Cr_Fld.TRANS_POSTING_MISMATCH_XFLG);
}

void MRACON_build_request_field_45( )
{
	memset(Cr_Fld.CREATE_CASE_XFLG,SPACES,MRACON_FIELD45_SIZE);
	sprintf(&message[412],"%-1s\n",Cr_Fld.CREATE_CASE_XFLG);
}

void MRACON_build_request_field_46( )
{
	memset(Cr_Fld.USER_INDICATOR_1_XCD,SPACES,MRACON_FIELD46_SIZE);
	sprintf(&message[413],"%-1s\n",Cr_Fld.USER_INDICATOR_1_XCD);
}

void MRACON_build_request_field_47( )
{
	memset(Cr_Fld.USER_INDICATOR_2_XCD,SPACES,MRACON_FIELD47_SIZE);
	sprintf(&message[414],"%-1s\n",Cr_Fld.USER_INDICATOR_2_XCD);
}

void MRACON_build_request_field_48( )
{
	memset(Cr_Fld.USER_DATA_1_STRG,SPACES,MRACON_FIELD48_SIZE); /* UCAF indicator DE48 SE43 */

	/*Field 48 for Terminal type*/
	/* if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_VISA)	== 0)
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
				memcpy(Cr_Fld.USER_DATA_1_STRG,"08", 2);
			}
			else
			{
				memcpy(Cr_Fld.USER_DATA_1_STRG,"07", 2);
			}
		}
		else
		{
			memcpy(Cr_Fld.USER_DATA_1_STRG,"03", 2);
		}
	}*/

	memcpy(Cr_Fld.USER_DATA_1_STRG,Auth_Tx.TLF01_details.card_num,6);
	sprintf(&message[415],"%-10s\n",Cr_Fld.USER_DATA_1_STRG);

}

void MRACON_build_request_field_49( )
{

	memset(Cr_Fld.USER_DATA_2_STRG,SPACES,MRACON_FIELD49_SIZE);
	/*Field 49 for Terminal Entry capa*/
	/*if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_VISA)	== 0)
	{
		memcpy(Cr_Fld.USER_DATA_2_STRG,&Auth_Tx.additional_pos_information[1],1);
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_MASTERCARD)	== 0)
	{
		memcpy(Cr_Fld.USER_DATA_2_STRG,&Auth_Tx.acceptor_term_name[10],1);
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_AMEX)	== 0)
	{ */
		/*if(Auth_Tx.TLF01_details.product_codes[16].amount[0]=='5')
			strncpy(Cr_Fld.USER_DATA_2_STRG,"C",1);
		else*/
	/*	memcpy(Cr_Fld.USER_DATA_2_STRG,&Auth_Tx.TLF01_details.product_codes[16].amount[0],1);
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
	} */
	memcpy(Cr_Fld.USER_DATA_2_STRG,Auth_Tx.TLF01_details.card_num,8);
	sprintf(&message[425],"%-10s\n",Cr_Fld.USER_DATA_2_STRG);
}

void MRACON_build_request_field_50( )
{
	memset(Cr_Fld.MERCHANT_XIDD,SPACES,MRACON_FIELD50_SIZE);
	sprintf(&message[435],"%-10s\n",Cr_Fld.MERCHANT_XIDD);
}

void MRACON_build_request_field_51( )
{
	memset(Cr_Fld.MERCHANT_DATA_XFLG,SPACES,MRACON_FIELD51_SIZE);
	sprintf(&message[445],"%-1s\n",Cr_Fld.MERCHANT_DATA_XFLG);
}

void MRACON_build_request_field_52( )
{
	memset(Cr_Fld.CARD_DATA_AVAILABLE_XFLG,SPACES,MRACON_FIELD52_SIZE);
	sprintf(&message[446],"%-1s\n",Cr_Fld.CARD_DATA_AVAILABLE_XFLG);
}

void MRACON_build_request_field_53( )
{
	memcpy(Cr_Fld.EXTERNAL_1_SCR,"0000",MRACON_FIELD53_SIZE);
	sprintf(&message[447],"%-4s\n",Cr_Fld.EXTERNAL_1_SCR);
}

void MRACON_build_request_field_54( )
{
	memcpy(Cr_Fld.EXTERNAL_2_SCR,"0000",MRACON_FIELD54_SIZE);
	sprintf(&message[451],"%-4s\n",Cr_Fld.EXTERNAL_2_SCR);
}

void MRACON_build_request_field_55( )
{
	memcpy(Cr_Fld.EXTERNAL_3_SCR,"0000",MRACON_FIELD55_SIZE);
	sprintf(&message[455],"%-4s\n",Cr_Fld.EXTERNAL_3_SCR);
}

void MRACON_build_request_field_56( )
{
	if(strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"C",MRACON_FIELD56_SIZE) == 0 ||
	   strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"D",MRACON_FIELD56_SIZE) == 0 ||
	   strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"F",MRACON_FIELD56_SIZE) == 0 ||
	   strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"G",MRACON_FIELD56_SIZE) == 0 ||
	   strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"S",MRACON_FIELD56_SIZE) == 0 ||
	   strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"T",MRACON_FIELD56_SIZE) == 0 ||
	   strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"U",MRACON_FIELD56_SIZE) == 0 ||
	   strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"V",MRACON_FIELD56_SIZE) == 0 ||
	   strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG," ",MRACON_FIELD56_SIZE) == 0)
	{
		memcpy(Cr_Fld.CARDHOLDER_PRESENT_XFLG,"Y",MRACON_FIELD56_SIZE);
		
	}
	else if(strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"E",MRACON_FIELD56_SIZE) == 0 ||
	   strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"K",MRACON_FIELD56_SIZE) == 0 )

	{
		memcpy(Cr_Fld.CARDHOLDER_PRESENT_XFLG,"N",MRACON_FIELD56_SIZE);
		/* if(strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"E",MRACON_FIELD56_SIZE) == 0)
		{
			memcpy(Cr_Fld.TRANSACTION_CATEGORY_XCD,"I",MRACON_FIELD56_SIZE);
		}
		else
		{
			memcpy(Cr_Fld.TRANSACTION_CATEGORY_XCD,"M",MRACON_FIELD56_SIZE);
		} */
	}
	else
		memcpy(Cr_Fld.CARDHOLDER_PRESENT_XFLG," ",MRACON_FIELD56_SIZE);


	sprintf(&message[459],"%-1s\n",Cr_Fld.CARDHOLDER_PRESENT_XFLG);
}

void MRACON_build_request_field_57( )
{

		memcpy(Cr_Fld.CAT_TYPE_XFLG," ",MRACON_FIELD57_SIZE);/* POS Condition code */

	sprintf(&message[460],"%-1s\n",Cr_Fld.CAT_TYPE_XFLG);
}

void MRACON_build_request_field_58( )
{
	memset(Cr_Fld.TESTING_RANDOM_DIGITS_STRG,SPACES,MRACON_FIELD58_SIZE);
	sprintf(&message[461],"%-2s\n",Cr_Fld.TESTING_RANDOM_DIGITS_STRG);
}

void MRACON_build_request_field_59( )
{
	memset(Cr_Fld.PORTFOLIO_NAME,SPACES,MRACON_FIELD59_SIZE);
	sprintf(&message[463],"%-14s\n",Cr_Fld.PORTFOLIO_NAME);
}

void MRACON_build_request_field_60( )
{
	memset(Cr_Fld.CLIENT_2_XID,SPACES,MRACON_FIELD60_SIZE);
	sprintf(&message[477],"%-14s\n",Cr_Fld.CLIENT_2_XID);
}

void MRACON_build_request_field_61( )
{
	memset(Cr_Fld.INTERBANK_CARD_ASSOCIATION_NUM,SPACES,MRACON_FIELD61_SIZE);
	if((0 == strncmp(Auth_Tx.TLF01_details.product_codes[2].quantity,"1",1))||
	  (0 == strncmp(Auth_Tx.TLF01_details.card_num, "7",1)))
	{
		memcpy(Cr_Fld.INTERBANK_CARD_ASSOCIATION_NUM,Auth_Tx.TLF01_details.acquiring_id,MRACON_FIELD61_SIZE);
	}
	else
	{
		memcpy(Cr_Fld.INTERBANK_CARD_ASSOCIATION_NUM,Auth_Tx.TLF01_details.acquiring_id+5,MRACON_FIELD61_SIZE);
	}

	sprintf(&message[491],"%-6s\n",Cr_Fld.INTERBANK_CARD_ASSOCIATION_NUM);
}

void MRACON_build_request_field_62( )
{
	if(MRACON_check_If_Fleet_Transaction()==true ||
	   MRACON_Check_If_CUP_MP_Transaction() == true)
	{
		if(strlen(Auth_Tx.MCF01_details.name02)> 0)
		{
			memcpy(Cr_Fld.MERCHANT_NAME,Auth_Tx.MCF01_details.name02,30);
		}
		else if(strlen(Auth_Tx.MCF01_details.name01)>0)
		{
			memcpy(Cr_Fld.MERCHANT_NAME,Auth_Tx.MCF01_details.name01,30);
		}
		else if(strlen(Auth_Tx.TLF01_details.card_holder_name)>0)
		{
			memcpy(Cr_Fld.MERCHANT_NAME,Auth_Tx.TLF01_details.card_holder_name,30);
		}

	}
	if (MRACON_check_If_MC_Transaction () == true )
	{
		if(strlen(Auth_Tx.MCF01_details.name02)> 0)
		{
			memcpy(Cr_Fld.MERCHANT_NAME,Auth_Tx.MCF01_details.name02,30);
		}
	}
	else if (MRACON_check_If_VISA_Transaction () == true ||
			MRACON_check_If_AMEX_Transaction () == true )
	{
		if(strlen(Auth_Tx.TLF01_details.card_holder_name)>0)
		{
			memcpy(Cr_Fld.MERCHANT_NAME,Auth_Tx.TLF01_details.card_holder_name,30);
		}
	}
	else if (MRACON_check_If_JCB_Transaction () == true||
			MRACON_check_If_CUP_Transaction () == true||
			MRACON_check_If_DINERS_Transaction () == true)
	{
		if(strlen(Auth_Tx.MCF01_details.name02)> 0)
		{
			memcpy(Cr_Fld.MERCHANT_NAME,Auth_Tx.MCF01_details.name02,30);
		}
		else if(strlen(Auth_Tx.MCF01_details.name01)>0)
		{
			memcpy(Cr_Fld.MERCHANT_NAME,Auth_Tx.MCF01_details.name01,30);
		}
	}

	sprintf(&message[497],"%-40s\n",Cr_Fld.MERCHANT_NAME);
}

void MRACON_build_request_field_63( )
{
	/*if(strlen(Auth_Tx.MCF01_details.city)== 0)
	{
		memcpy(Cr_Fld.MERCHANT_CITY_NAME,Auth_Tx.TLF01_details.ncf01_retired_cwk,13);
	}
	else
	{*/
		memcpy(Cr_Fld.MERCHANT_CITY_NAME,Auth_Tx.MCF01_details.city,20);

	sprintf(&message[537],"%-30s\n",Cr_Fld.MERCHANT_CITY_NAME);
}

void MRACON_build_request_field_64( )
{
/*	if(strlen(Auth_Tx.MCF01_details.intl_code)== 0)
	{
		memcpy(Cr_Fld.MERCHANT_STATE_XCD,Auth_Tx.TLF01_details.ncf01_retired_cwk+13,MRACON_FIELD64_SIZE);
	}
	else
	{
		memcpy(Cr_Fld.MERCHANT_STATE_XCD,Auth_Tx.MCF01_details.intl_code,MRACON_FIELD64_SIZE);
	}*/
	memcpy(Cr_Fld.MERCHANT_STATE_XCD,Auth_Tx.MCF01_details.state,MRACON_FIELD64_SIZE);
	sprintf(&message[567],"%-3s\n",Cr_Fld.MERCHANT_STATE_XCD);
}

void MRACON_build_request_field_65( )
{
	memset(Cr_Fld.SUPPRES_CASE_XFLG,SPACES,MRACON_FIELD65_SIZE);
	sprintf(&message[570],"%-1s\n",Cr_Fld.SUPPRES_CASE_XFLG);
}

void MRACON_build_request_field_66( )
{
	memset(Cr_Fld.USER_INDICATOR_3_XCD,SPACES,MRACON_FIELD66_SIZE);
	sprintf(&message[571],"%-5s\n",Cr_Fld.USER_INDICATOR_3_XCD);
}

void MRACON_build_request_field_67( )
{
	strncat(Cr_Fld.USER_INDICATOR_4_XCD,Auth_Tx.TLF01_details.currency_code,3);
	
	sprintf(&message[576],"%-5s\n",Cr_Fld.USER_INDICATOR_4_XCD);
}

void MRACON_build_request_field_68( )
{
	char temp_totalamt[14]={0};
	int int_total_Amt = 0;
	int_total_Amt = atoi(Auth_Tx.TLF01_details.total_amount);
	strncpy(Cr_Fld.USER_DATA_3_STRG,"00",2);


		sprintf(temp_totalamt,"%010d.%02d",int_total_Amt/100, int_total_Amt%100);

	strncat(Cr_Fld.USER_DATA_3_STRG,temp_totalamt,13);
	sprintf(&message[581],"%-15s\n",Cr_Fld.USER_DATA_3_STRG);
}

void MRACON_build_request_field_69( )
{
	strncpy(Cr_Fld.USER_DATA_4_STRG,Auth_Tx.TLF01_details.response_text,MRACON_FIELD69_SIZE);
	/*int len_field69 = strlen(Auth_Tx.TLF01_details.currency_code);
	strncpy(Cr_Fld.USER_DATA_4_STRG,Auth_Tx.TLF01_details.currency_code,len_field69);*/
	sprintf(&message[596],"%-20s\n",Cr_Fld.USER_DATA_4_STRG);
}

void MRACON_build_request_field_70( )
{
	CHAR resp_buf[15] = {0} ;
	memset(Cr_Fld.USER_DATA_5_STRG,SPACES,MRACON_FIELD70_SIZE);
	CHAR     MPFlag[2] ="1";
/*	if (Auth_Tx.TLF01_details.tx_key > 100)
		{
			strncpy (resp_buf , " RESPONSE",10);
		}
		else
		{
			memset(resp_buf,0x00,sizeof(resp_buf)-1);
		}*/

		switch(Auth_Tx.TLF01_details.tx_key)
		{
			case AUTH_INVALID							  :

				strcpy(Cr_Fld.USER_DATA_5_STRG,"INVALID");
			break ;

			case AUTH_SALE                     	     	  :
			case AUTH_SALE_RESPONSE                       :
				if((0 == strncmp((char *)Auth_Tx.TLF01_details.product_codes[2].quantity,MPFlag,1)))
				{
					strcpy(Cr_Fld.USER_DATA_5_STRG,"MP SALE");
				}
				else
				{
					strcpy(Cr_Fld.USER_DATA_5_STRG,"SALE");
				}
			break ;

			case AUTH_CASH_ADVANCE				          :
			case AUTH_CASH_ADVANCE_RESPONSE               :
				strcpy(Cr_Fld.USER_DATA_5_STRG,"CASH ADVANCE");
			break ;

			case AUTH_OFFLINE_SALE                        :
			case AUTH_OFFLINE_SALE_RESPONSE               :
				if((0 == strncmp((char *)Auth_Tx.TLF01_details.product_codes[2].quantity,MPFlag,1)))
				{
					strcpy(Cr_Fld.USER_DATA_5_STRG,"MP OFFLINE SALE");
				}
				else
				{
					strcpy(Cr_Fld.USER_DATA_5_STRG,"OFFLINE SALE");
				}
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
				if((0 == strncmp((char *)Auth_Tx.TLF01_details.product_codes[2].quantity,MPFlag,1)))
				{
					strcpy(Cr_Fld.USER_DATA_5_STRG,"MP VOID SALE");
				}
				else
				{
					strcpy(Cr_Fld.USER_DATA_5_STRG,"VOID SALE");
				}
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
				strcpy(Cr_Fld.USER_DATA_5_STRG,"PRE AUTHORIZATION CANCEL");
			break ;

			case AUTH_AUTHORIZATION						  :
			case AUTH_AUTHORIZATION_RESPONSE			  :
				if ( ( strncmp((const char *)(Auth_Tx.TLF01_details.voice_auth), "Y",1) == 0) ||
						( strncmp((const char *)(Auth_Tx.TLF01_details.voice_auth), "y",1) == 0) )
				{
					strcpy(Cr_Fld.USER_DATA_5_STRG,"VOICE AUTHORIZATION");
				}
				else
				{
					strcpy(Cr_Fld.USER_DATA_5_STRG,"AUTHORIZATION");
				}
							break ;


		}


		sprintf(&message[616],"%-40s\n",Cr_Fld.USER_DATA_5_STRG);
}

void MRACON_build_request_field_71( )
{
	memset(Cr_Fld.REAL_TIME_REQUEST_XFLG,SPACES,MRACON_FIELD71_SIZE);
	sprintf(&message[656],"%-1s\n",Cr_Fld.REAL_TIME_REQUEST_XFLG);
}

void MRACON_build_request_field_72( )
{
	memset(Cr_Fld.PRIOR_ACTION_DATABASE_XCD,SPACES,MRACON_FIELD72_SIZE);
	sprintf(&message[657],"%-1s\n",Cr_Fld.PRIOR_ACTION_DATABASE_XCD);
}

void MRACON_build_request_field_73( )
{
	memset(Cr_Fld.PRIOR_ACTION_DB_EXPIRATION_DT,SPACES,MRACON_FIELD73_SIZE);
	sprintf(&message[658],"%-8s\n",Cr_Fld.PRIOR_ACTION_DB_EXPIRATION_DT);
}

void MRACON_build_request_field_74( )
{
	memcpy(Cr_Fld.MASTER_ACCOUNT_NUMBER_XID,Auth_Tx.TLF01_details.card_num,MRACON_FIELD74_SIZE);
	sprintf(&message[666],"%-19s\n",Cr_Fld.MASTER_ACCOUNT_NUMBER_XID);
}

void MRACON_build_request_field_75( )
{

	memset(Cr_Fld.CARD_OFFLN_STATIC_AUTHCTN_XFLG,SPACES,MRACON_FIELD75_SIZE);
	sprintf(&message[685],"%-1s\n",Cr_Fld.CARD_OFFLN_STATIC_AUTHCTN_XFLG);

}

void MRACON_build_request_field_76( )
{
	memset(Cr_Fld.CARD_DYNAMIC_AUTHCTN_XCD,SPACES,MRACON_FIELD76_SIZE);
	sprintf(&message[686],"%-1s\n",Cr_Fld.CARD_DYNAMIC_AUTHCTN_XCD);
}

void MRACON_build_request_field_77( )
{
	memset(Cr_Fld.RESERVED_02,SPACES,MRACON_FIELD77_SIZE);
	sprintf(&message[687],"%-1s\n",Cr_Fld.RESERVED_02);
}

void MRACON_build_request_field_78( )
{
	memset(Cr_Fld.CARD_AIP_VERIFICATION_XFLG,SPACES,MRACON_FIELD78_SIZE);
	sprintf(&message[688],"%-1s\n",Cr_Fld.CARD_AIP_VERIFICATION_XFLG);
}
void MRACON_build_request_field_79( )
{
	memset(Cr_Fld.CARD_AIP_RISK_XFLG,SPACES,MRACON_FIELD79_SIZE);
	sprintf(&message[689],"%-1s\n",Cr_Fld.CARD_AIP_RISK_XFLG);
}

void MRACON_build_request_field_80( )
{
	memset(Cr_Fld.CARD_ISSUER_AUTH_SUPPORT_XFLG,SPACES,MRACON_FIELD80_SIZE);
	sprintf(&message[690],"%-1s\n",Cr_Fld.CARD_ISSUER_AUTH_SUPPORT_XFLG);
}

void MRACON_build_request_field_81( )
{
	memset(Cr_Fld.CARD_COMBINED_AUTHCTN_XFLG,SPACES,MRACON_FIELD81_SIZE);
	sprintf(&message[691],"%-1s\n",Cr_Fld.CARD_COMBINED_AUTHCTN_XFLG);

}

void MRACON_build_request_field_82( )
{
	memset(Cr_Fld.TRANSACTION_DELINQUENT_CYC,SPACES,MRACON_FIELD82_SIZE);
	sprintf(&message[692],"%-1s\n",Cr_Fld.TRANSACTION_DELINQUENT_CYC);
}

void MRACON_build_request_field_83( )
{
	memset(Cr_Fld.TRANS_TOTAL_DELINQUENT_AMT,SPACES,MRACON_FIELD83_SIZE);
	sprintf(&message[693],"%-13s\n",Cr_Fld.TRANS_TOTAL_DELINQUENT_AMT);
}

void MRACON_build_request_field_84( )
{
	memset(Cr_Fld.CRD_STATEMENT_CASH_BALANCE_AMT,SPACES,MRACON_FIELD84_SIZE);
	sprintf(&message[706],"%-13s\n",Cr_Fld.CRD_STATEMENT_CASH_BALANCE_AMT);
}

void MRACON_build_request_field_85( )
{

	memset(Cr_Fld.MERCHANT_BALANCE_AMT,SPACES,MRACON_FIELD85_SIZE);
	sprintf(&message[719],"%-13s\n",Cr_Fld.MERCHANT_BALANCE_AMT);

}

void MRACON_build_request_field_86( )
{
	memset(Cr_Fld.CUSTOMER_POSTAL_XCD,SPACES,MRACON_FIELD86_SIZE);
	sprintf(&message[732],"%-12s\n",Cr_Fld.CUSTOMER_POSTAL_XCD);
}

void MRACON_build_request_field_87( )
{
	memset(Cr_Fld.CARD_MEDIA_TYPE_XCD,SPACES,MRACON_FIELD87_SIZE);
	sprintf(&message[744],"%-1s\n",Cr_Fld.CARD_MEDIA_TYPE_XCD);
}

void MRACON_build_request_field_88( )
{
	memset(Cr_Fld.CVV2_PRESENT_XFLG,SPACES,MRACON_FIELD88_SIZE);
	if (MRACON_check_If_MC_Transaction () == true )
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
				Cr_Fld.CVV2_PRESENT_XFLG[0] = SPACES ;
			}
			else
			{
				Cr_Fld.CVV2_PRESENT_XFLG[0] = '9' ;
			}
		}
		else if (MRACON_check_If_VISA_Transaction () == true )
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
		else if (MRACON_check_If_AMEX_Transaction () == true )
		{
			if (Auth_Tx.TLF01_details.product_codes[17].quantity[0] != SPACES)
			{
				Cr_Fld.CVV2_PRESENT_XFLG[0] = '1';
			}
			else if (Auth_Tx.TLF01_details.product_codes[17].quantity[0] == SPACES)
			{
				Cr_Fld.CVV2_PRESENT_XFLG[0] = SPACES;
			}
		}
		else if (MRACON_check_If_JCB_Transaction () == true )
		{
			Cr_Fld.CVV2_PRESENT_XFLG[0] = SPACES;
		}

		else if (MRACON_check_If_CUP_Transaction () == true ||
				MRACON_Check_If_CUP_MP_Transaction() == true)
		{
			if( Auth_Tx.TLF01_details.cvc_data[0] == '1' ||
				Auth_Tx.TLF01_details.cvc_data[0] == '2' ||
				Auth_Tx.TLF01_details.cvc_data[0] == '3' )
			/*if( Auth_Tx.future_use_2[24] == '1'||
				Auth_Tx.future_use_2[24] == '2'||
				Auth_Tx.future_use_2[24] == '3')*/
			{
				Cr_Fld.CVV2_PRESENT_XFLG[0] = '1';
			}
			else if( Auth_Tx.TLF01_details.cvc_data[0] == SPACES)
			
			{
				Cr_Fld.CVV2_PRESENT_XFLG[0] = '9';
			}
			else
			{
				Cr_Fld.CVV2_PRESENT_XFLG[0] = SPACES;
			}
		}
		else if (MRACON_check_If_DINERS_Transaction () == true )
		{
			if(strncmp(Auth_Tx.acct_id_1,"006",3) == 0||
			    strncmp(Auth_Tx.acct_id_1,"007",3)== 0||
				strncmp(Auth_Tx.acct_id_1,"010",3)== 0||
				strncmp(Auth_Tx.acct_id_1,"011",3)== 0||
				strncmp(Auth_Tx.acct_id_1,"012",3)== 0||
				strncmp(Auth_Tx.acct_id_1,"013",3)== 0||
				strncmp(Auth_Tx.acct_id_1,"014",3)== 0||
				strncmp(Auth_Tx.acct_id_1,"015",3)== 0)
			{
				Cr_Fld.CVV2_PRESENT_XFLG[0] = '1';
			}
			else
			{
				Cr_Fld.CVV2_PRESENT_XFLG[0] = SPACES;
			}
		}


		sprintf(&message[745],"%-1s\n",Cr_Fld.CVV2_PRESENT_XFLG);
}

void MRACON_build_request_field_89( )
{
		memset(Cr_Fld.CVV2_RESPONSE_XFLG,SPACES,MRACON_FIELD89_SIZE);
	 	if (MRACON_check_If_MC_Transaction () == true )
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
				Cr_Fld.CVV2_RESPONSE_XFLG[0] = SPACES ;
			}
	 	}

	else if (MRACON_check_If_VISA_Transaction () == true )
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
			Cr_Fld.CVV2_RESPONSE_XFLG[0] = SPACES;
		}
	}

	else if (MRACON_check_If_AMEX_Transaction () == true )
	{
	if( Auth_Tx.TLF01_details.product_codes[17].quantity[0] == '0')
			{
				Cr_Fld.CVV2_RESPONSE_XFLG[0] = 'M';
			}
			else if (Auth_Tx.TLF01_details.product_codes[17].quantity[0] == '7')
			{
				Cr_Fld.CVV2_RESPONSE_XFLG[0] = 'N';
			}
			else if (Auth_Tx.TLF01_details.product_codes[17].quantity[0] == '8')
			{
				Cr_Fld.CVV2_RESPONSE_XFLG[0] = 'P';
			}
			else
			{
				Cr_Fld.CVV2_RESPONSE_XFLG[0] = SPACES;
			}
	}
	else if (MRACON_check_If_JCB_Transaction () == true )
	{
		Cr_Fld.CVV2_RESPONSE_XFLG[0] = SPACES;
	}

	else if (MRACON_check_If_CUP_Transaction () == true ||
			MRACON_Check_If_CUP_MP_Transaction() == true)
	{

		
		if(Auth_Tx.TLF01_details.cvc_data[0] == 'M')
		{
			Cr_Fld.CVV2_RESPONSE_XFLG[0] = '1' ;
		}
		else if(Auth_Tx.TLF01_details.cvc_data[0] == 'N') 
		{
			Cr_Fld.CVV2_RESPONSE_XFLG[0] = '2' ;
		}
		else if(Auth_Tx.TLF01_details.cvc_data[0] == 'P')
		{
			Cr_Fld.CVV2_RESPONSE_XFLG[0] = '3' ;
		}
		else
		{
			Cr_Fld.CVV2_RESPONSE_XFLG[0] = SPACES ;
		}
	}
	else if (MRACON_check_If_DINERS_Transaction () == true )
	{
		if(strncmp(Auth_Tx.acct_id_1,"006",3)== 0||
			strncmp(Auth_Tx.acct_id_1,"010",3)== 0||
			strncmp(Auth_Tx.acct_id_1,"013",3)== 0||
			strncmp(Auth_Tx.acct_id_1,"014",3)== 0)
		{
			Cr_Fld.CVV2_RESPONSE_XFLG[0] = 'M' ;
		}
	   else if(strncmp(Auth_Tx.acct_id_1,"007",3)== 0||
				strncmp(Auth_Tx.acct_id_1,"011",3)== 0||
				strncmp(Auth_Tx.acct_id_1,"012",3)== 0||
				strncmp(Auth_Tx.acct_id_1,"015",3)== 0)
		{
			Cr_Fld.CVV2_RESPONSE_XFLG[0] = 'N' ;
		}
	   else if(strncmp(Auth_Tx.acct_id_1," ",3) == 0||
			 strncmp(Auth_Tx.acct_id_1,"018",3) == 0)
	   {
		   Cr_Fld.CVV2_RESPONSE_XFLG[0] = SPACES ;
	   }
	}


	sprintf(&message[746],"%-1s\n",Cr_Fld.CVV2_RESPONSE_XFLG);
}

void MRACON_build_request_field_90( )
{
	memset(Cr_Fld.AVS_RESPONSE_XCD,SPACES,MRACON_FIELD90_SIZE);
	sprintf(&message[747],"%-1s\n",Cr_Fld.AVS_RESPONSE_XCD);
}

void MRACON_build_request_field_91( )
{
	if (MRACON_check_If_MC_Transaction () == true )
	{
		if (Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]=='K' &&
				strncmp(Auth_Tx.TLF01_details.pos_condition_code,"08",2)==0)
		{
			strncpy(Cr_Fld.TRANSACTION_CATEGORY_XCD,"A",MRACON_FIELD91_SIZE);
		}

		else if(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]=='K' &&
				strncmp(Auth_Tx.TLF01_details.pos_condition_code,"08",2)!=0)
		{
			strncpy(Cr_Fld.TRANSACTION_CATEGORY_XCD,"M",MRACON_FIELD91_SIZE);
		}

		else if(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]=='C'||
				Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]=='D'||
				Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]=='F'||
				Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]=='G'||
				Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]=='S'||
				Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]=='T'||
				Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]=='U'||
				Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]=='V'||
				Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]==SPACES)
		{
			strncpy(Cr_Fld.TRANSACTION_CATEGORY_XCD,"P",MRACON_FIELD91_SIZE);
		}

		else if(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]=='E')
		{
			strncpy(Cr_Fld.TRANSACTION_CATEGORY_XCD,"I",MRACON_FIELD91_SIZE);
		}
	}
	else if (MRACON_check_If_VISA_Transaction () == true )
	{
		if (Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]=='K' &&
				strncmp(Auth_Tx.TLF01_details.pos_condition_code,"08",2)==0)
		{
			strncpy(Cr_Fld.TRANSACTION_CATEGORY_XCD,"A",MRACON_FIELD91_SIZE);
		}

		else if(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]=='K' &&
				strncmp(Auth_Tx.TLF01_details.pos_condition_code,"08",2)!=0)
		{
			strncpy(Cr_Fld.TRANSACTION_CATEGORY_XCD,"M",MRACON_FIELD91_SIZE);
		}

		else if(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]=='C'||
				Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]=='D'||
				Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]=='F'||
				Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]=='G'||
				Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]=='S'||
				Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]=='T'||
				Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]=='U'||
				Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]=='V'||
				Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]==SPACES)
		{
			strncpy(Cr_Fld.TRANSACTION_CATEGORY_XCD,"P",MRACON_FIELD91_SIZE);
		}

		else if(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]=='E')
		{
			strncpy(Cr_Fld.TRANSACTION_CATEGORY_XCD,"I",MRACON_FIELD91_SIZE);
		}
	}
	else if(MRACON_check_If_AMEX_Transaction () == true )
	{
		if (Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]=='K' &&
				strncmp(Auth_Tx.TLF01_details.pos_condition_code,"08",2)==0)
		{
			strncpy(Cr_Fld.TRANSACTION_CATEGORY_XCD,"A",MRACON_FIELD91_SIZE);
		}

		else if(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]=='K' &&
				strncmp(Auth_Tx.TLF01_details.pos_condition_code,"08",2)!=0)
		{
			strncpy(Cr_Fld.TRANSACTION_CATEGORY_XCD,"M",MRACON_FIELD91_SIZE);
		}

		else if(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]=='C'||
				Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]=='D'||
				Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]=='F'||
				Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]=='G'||
				Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]=='S'||
				Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]=='T'||
				Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]=='U'||
				Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]=='V'||
				Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]==SPACES)
		{
			strncpy(Cr_Fld.TRANSACTION_CATEGORY_XCD,"P",MRACON_FIELD91_SIZE);
		}

		else if(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]=='E')
		{
			strncpy(Cr_Fld.TRANSACTION_CATEGORY_XCD,"I",MRACON_FIELD91_SIZE);
		}
	}
	else if(MRACON_check_If_JCB_Transaction () == true )
	{
		if (Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]=='K' &&
				strncmp(Auth_Tx.TLF01_details.pos_condition_code,"08",2)==0)
		{
			strncpy(Cr_Fld.TRANSACTION_CATEGORY_XCD,"A",MRACON_FIELD91_SIZE);
		}

		else if(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]=='K' &&
				strncmp(Auth_Tx.TLF01_details.pos_condition_code,"08",2)!=0)
		{
			strncpy(Cr_Fld.TRANSACTION_CATEGORY_XCD,"M",MRACON_FIELD91_SIZE);
		}

		else if(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]=='C'||
				Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]=='D'||
				Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]=='F'||
				Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]=='G'||
				Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]=='S'||
				Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]=='T'||
				Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]=='U'||
				Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]=='V'||
				Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]==SPACES)
		{
			strncpy(Cr_Fld.TRANSACTION_CATEGORY_XCD,"P",MRACON_FIELD91_SIZE);
		}

		else if(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]=='E')
		{
			strncpy(Cr_Fld.TRANSACTION_CATEGORY_XCD,"I",MRACON_FIELD91_SIZE);
		}
	}
	else if(MRACON_check_If_CUP_Transaction () == true ||
			MRACON_Check_If_CUP_MP_Transaction() == true)
	{
		if (Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]=='K' &&
				strncmp(Auth_Tx.TLF01_details.pos_condition_code,"08",2)==0)
		{
			strncpy(Cr_Fld.TRANSACTION_CATEGORY_XCD,"A",MRACON_FIELD91_SIZE);
		}

		else if(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]=='K' &&
				strncmp(Auth_Tx.TLF01_details.pos_condition_code,"08",2)!=0)
		{
			strncpy(Cr_Fld.TRANSACTION_CATEGORY_XCD,"M",MRACON_FIELD91_SIZE);
		}

		else if(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]=='C'||
				Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]=='D'||
				Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]=='F'||
				Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]=='G'||
				Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]=='S'||
				Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]=='T'||
				Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]=='U'||
				Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]=='V'||
				Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]==SPACES)
		{
			strncpy(Cr_Fld.TRANSACTION_CATEGORY_XCD,"P",MRACON_FIELD91_SIZE);
		}

		else if(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]=='E')
		{
			strncpy(Cr_Fld.TRANSACTION_CATEGORY_XCD,"I",MRACON_FIELD91_SIZE);
		}
	}
	else if(MRACON_check_If_DINERS_Transaction () == true)
	{
		if (Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]=='K' &&
				strncmp(Auth_Tx.TLF01_details.pos_condition_code,"08",2)==0)
		{
			strncpy(Cr_Fld.TRANSACTION_CATEGORY_XCD,"A",MRACON_FIELD91_SIZE);
		}

		else if(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]=='K' &&
				strncmp(Auth_Tx.TLF01_details.pos_condition_code,"08",2)!=0)
		{
			strncpy(Cr_Fld.TRANSACTION_CATEGORY_XCD,"M",MRACON_FIELD91_SIZE);
		}

		else if(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]=='C'||
				Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]=='D'||
				Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]=='F'||
				Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]=='G'||
				Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]=='S'||
				Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]=='T'||
				Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]=='U'||
				Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]=='V'||
				Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]==SPACES)
		{
			strncpy(Cr_Fld.TRANSACTION_CATEGORY_XCD,"P",MRACON_FIELD91_SIZE);
		}

		else if(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG[0]=='E')
		{
			strncpy(Cr_Fld.TRANSACTION_CATEGORY_XCD,"I",MRACON_FIELD91_SIZE);
		}
	}
	sprintf(&message[748],"%-1s\n",Cr_Fld.TRANSACTION_CATEGORY_XCD);
	/*if(strlen(Cr_Fld.TRANSACTION_CATEGORY_XCD) == 0 || strcmp(Cr_Fld.TRANSACTION_CATEGORY_XCD, " ") == 0)
	strncpy(Cr_Fld.TRANSACTION_CATEGORY_XCD,"O",MRACON_FIELD91_SIZE);
	sprintf(&message[748],"%-1s\n",Cr_Fld.TRANSACTION_CATEGORY_XCD);*/
}

void MRACON_build_request_field_92( )
{
	int len = strlen(Auth_Tx.TLF01_details.acquiring_id);
	if(len > 0)
	{
		if((0 == strncmp(Auth_Tx.TLF01_details.product_codes[2].quantity,"1",1))||
		  (0 == strncmp(Auth_Tx.TLF01_details.card_num, "7",1)))
		{
			strncpy(Cr_Fld.ACQUIRER_XID,Auth_Tx.TLF01_details.acquiring_id,MRACON_FIELD92_SIZE);
		}
		else
		{
			strncpy(Cr_Fld.ACQUIRER_XID,Auth_Tx.TLF01_details.acquiring_id+5,MRACON_FIELD92_SIZE);
		}
	}
	sprintf(&message[749],"%-12s\n",Cr_Fld.ACQUIRER_XID);
}

void MRACON_build_request_field_93( )
{
	/*if(MRACON_check_If_MC_Transaction () == true)
	{
		if(strlen(Auth_Tx.TLF01_details.ncf01_retired_cwk)>0)
		{
			strncpy(Cr_Fld.ACQUIRER_COUNTRY_XCD,Auth_Tx.TLF01_details.ncf01_retired_cwk+13,3);
		}*/
	  /*  else if ( 0 != strcmp( NULL_STR, Auth_Tx->reconciliation_cur_code ) )
	    {
	    	memcpy( Cr_Fld.ACQUIRER_COUNTRY_XCD,
				 Auth_Tx->reconciliation_cur_code,
				 MRACON_FIELD93_SIZE );

	    	memcpy( Cr_Fld.ACQUIRER_COUNTRY_XCD,
				 Auth_Tx->TLF01_details.orig_amount,
				 MRACON_FIELD93_SIZE );
	    }
	    else
	    {
	    	memcpy( Cr_Fld.ACQUIRER_COUNTRY_XCD,
			 Auth_Tx->TLF01_details.currency_code,
			 MRACON_FIELD93_SIZE );
	    }*/
	/* } */

	/*if(MRACON_check_If_VISA_Transaction () == true)
	{
		 CHAR   temp_str[256]={0};
		if(strlen(Auth_Tx.TLF01_details.ncf01_retired_cwk)>0)
		{
			strncpy(Cr_Fld.ACQUIRER_COUNTRY_XCD,Auth_Tx.TLF01_details.ncf01_retired_cwk+38,2);
		}*/
		/*else if(Auth_Tx.TLF01_details.tx_key > 100)
		{
			   memset(temp_str, 0, sizeof(temp_str));


			  if ( Auth_Tx->MCF01_details.region_nbr[0] != 0x00 )
				 memcpy( temp_str, Auth_Tx->MCF01_details.region_nbr, 3 );
			  else
				 strcpy( temp_str, Auth_Tx->TLF01_details.currency_code );

		}*/
		 /*else
		 {
				strcpy (temp_str, Auth_Tx.TLF01_details.currency_code);
		 }

	 }*/
	/*if(MRACON_check_If_AMEX_Transaction () == true)
	{
		  if(Auth_Tx.country_code[0] != 0x00)	*/	/* Cadencie link changes - ThoughFocus Girija Y */
		  /*{
			if ( isnum(Auth_Tx.country_code) )
			{
			    memcpy( Cr_Fld.ACQUIRER_COUNTRY_XCD,Auth_Tx.country_code,
			    		MRACON_FIELD93_SIZE);
			}

		  }

		   else if(Auth_Tx.MCF01_details.currency_code[0] != 0x00)
		   {
				if ( true == isalphanum(Auth_Tx.MCF01_details.currency_code) )
				{
						memcpy( Cr_Fld.ACQUIRER_COUNTRY_XCD,
						Auth_Tx.MCF01_details.currency_code,
						MRACON_FIELD93_SIZE );

				}
		   }
		   else  if(Auth_Tx.TLF01_details.currency_code[0] != 0x00)
		   {
			   	if ( true == isalphanum(Auth_Tx.TLF01_details.currency_code) )
				{

						memcpy( Cr_Fld.ACQUIRER_COUNTRY_XCD,
						Auth_Tx.TLF01_details.currency_code,
						MRACON_FIELD93_SIZE );
				}
		   }
	}
	if(MRACON_check_If_JCB_Transaction () == true)
	{
		int len_curr_code = strlen (Auth_Tx.TLF01_details.currency_code);
		   if (len_curr_code > 0)
		   {
			   memcpy(Cr_Fld.ACQUIRER_COUNTRY_XCD,Auth_Tx.TLF01_details.currency_code,MRACON_FIELD93_SIZE);
		   }

	}
	if(MRACON_check_If_CUP_Transaction () == true ||
			(MRACON_check_If_DINERS_Transaction () == true))
	{
		int len_MCF_Currency_Code = strlen(Auth_Tx.MCF01_details.currency_code);
		int len_Country_code = strlen(Auth_Tx.country_code);

		if(len_Country_code>0)
		{
			memcpy(Cr_Fld.ACQUIRER_COUNTRY_XCD,Auth_Tx.country_code,MRACON_FIELD93_SIZE);
		}
		else if(len_MCF_Currency_Code > 0)
		{
			 memcpy(Cr_Fld.ACQUIRER_COUNTRY_XCD,Auth_Tx.MCF01_details.currency_code,MRACON_FIELD93_SIZE);
		}
		else if(strlen(Auth_Tx.TLF01_details.currency_code)> 0)
		{
		   	if ( true == isalphanum(Auth_Tx.TLF01_details.currency_code) )
		   	{
				memcpy( Cr_Fld.ACQUIRER_COUNTRY_XCD,
				Auth_Tx.TLF01_details.currency_code,
				MRACON_FIELD93_SIZE );
			}
		}

	}*/
	int len_curr_code, len_TLF_curr_code = 0;
	len_curr_code = strlen(Auth_Tx.MCF01_details.currency_code);
	len_TLF_curr_code = strlen(Auth_Tx.TLF01_details.currency_code);
	if(len_curr_code>0)
	{
		strncpy(Cr_Fld.ACQUIRER_COUNTRY_XCD,Auth_Tx.MCF01_details.currency_code,MRACON_FIELD38_SIZE);
	}
	else if (len_TLF_curr_code > 0)
	{
		strncpy(Cr_Fld.ACQUIRER_COUNTRY_XCD,Auth_Tx.TLF01_details.currency_code,MRACON_FIELD38_SIZE);
	}
	/*if(strncmp(Auth_Tx.TLF01_details.handler_queue,"dcpisoA",5) == 0)
	{
		strncpy(Cr_Fld.ACQUIRER_COUNTRY_XCD,"608",MRACON_FIELD93_SIZE);
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
	}*/
	sprintf(&message[761],"%-3s\n",Cr_Fld.ACQUIRER_COUNTRY_XCD);

}

void MRACON_build_request_field_94( )
{
	if(strcmp(Auth_Tx.TLF01_details.terminal_id,"") != 0)
	{
		memcpy(Cr_Fld.TERMINAL_XID,Auth_Tx.TLF01_details.terminal_id,sizeof(Auth_Tx.TLF01_details.terminal_id));
		sprintf(&message[764],"%-16s\n",Cr_Fld.TERMINAL_XID);
	}
	/*else
	{
	memset(Cr_Fld.TERMINAL_XID,SPACES,MRACON_FIELD94_SIZE);
	sprintf(&message[764],"%-16s\n",Cr_Fld.TERMINAL_XID);
	}*/
}

void MRACON_build_request_field_95( )
{
	memset(Cr_Fld.TERMINAL_TYPE_XCD,SPACES,MRACON_FIELD95_SIZE);
	if(MRACON_check_If_MC_Transaction () == true)
	{
	   
	   if( Auth_Tx.TLF01_details.product_codes[16].amount[9] == '4' ||
		   Auth_Tx.TLF01_details.product_codes[16].amount[9] == '6' ||
		   Auth_Tx.TLF01_details.product_codes[16].amount[9] == '7' )
	   {
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"I",MRACON_FIELD95_SIZE);
	   }
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[9] == '3'||
				Auth_Tx.TLF01_details.product_codes[16].amount[9] == '9')
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"L",MRACON_FIELD95_SIZE);
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[9] == '2' ||
				Auth_Tx.TLF01_details.product_codes[16].amount[9] == '1')
		{
			memset(Cr_Fld.TERMINAL_TYPE_XCD,'U',MRACON_FIELD95_SIZE);
		}
		else
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD," ",MRACON_FIELD95_SIZE);
		}
	}

	else if(MRACON_check_If_VISA_Transaction () == true)
	{
		if( Auth_Tx.TLF01_details.product_codes[16].amount[0] == '1' ||
			Auth_Tx.TLF01_details.product_codes[16].amount[0] == '4'||
			Auth_Tx.TLF01_details.product_codes[16].amount[0] == '9')
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"L",MRACON_FIELD95_SIZE);
		}

		else if(Auth_Tx.TLF01_details.product_codes[16].amount[0] == '5' ||
				Auth_Tx.TLF01_details.product_codes[16].amount[0] == '7' )
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"I",MRACON_FIELD95_SIZE);
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[0] == '3' ||
				Auth_Tx.TLF01_details.product_codes[16].amount[0] == '2' )
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"U",MRACON_FIELD95_SIZE);
		}
		else
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD," ",MRACON_FIELD95_SIZE);
		}
	}
	else if(MRACON_check_If_AMEX_Transaction () == true)
	{
		strncpy(Cr_Fld.TERMINAL_TYPE_XCD,&Auth_Tx.TLF01_details.product_codes[16].amount[3],MRACON_FIELD95_SIZE);
		if(strncmp(Cr_Fld.TERMINAL_TYPE_XCD,"1",1) == 0 ||
		   strncmp(Cr_Fld.TERMINAL_TYPE_XCD,"3",1) == 0 ||
		   strncmp(Cr_Fld.TERMINAL_TYPE_XCD,"X",1) == 0)
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"L",MRACON_FIELD95_SIZE);
		}
		else if(strncmp(Cr_Fld.TERMINAL_TYPE_XCD,"S",MRACON_FIELD95_SIZE) == 0 ||
		   strncmp(Cr_Fld.TERMINAL_TYPE_XCD,"T",MRACON_FIELD95_SIZE) == 0)
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"I",MRACON_FIELD95_SIZE);
		}
		else if(strncmp(Cr_Fld.TERMINAL_TYPE_XCD,"2",MRACON_FIELD95_SIZE) == 0 ||
		   strncmp(Cr_Fld.TERMINAL_TYPE_XCD,"4",1) == 0 ||
		   strncmp(Cr_Fld.TERMINAL_TYPE_XCD,"5",1) == 0 ||
		   strncmp(Cr_Fld.TERMINAL_TYPE_XCD,"Z",1) == 0)
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"U",1);
		}
		else
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD," ",MRACON_FIELD95_SIZE);
		}
	}
	else if(MRACON_check_If_CUP_Transaction () == true ||
			MRACON_Check_If_CUP_MP_Transaction() == true)
	{
		if( strncmp(&Auth_Tx.TLF01_details.product_codes[16].amount[4] , "03" ,2)== 0 ||
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
				else
				{
					Cr_Fld.TERMINAL_TYPE_XCD[0] = SPACES ;
				}
	}

	else if(MRACON_check_If_DINERS_Transaction () == true)
	{
		if(Auth_Tx.TLF01_details.product_codes[16].amount[3] == '1' ||
				Auth_Tx.TLF01_details.product_codes[16].amount[3] == '3'  )
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"L",MRACON_FIELD95_SIZE);
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[3] == '2' ||
				Auth_Tx.TLF01_details.product_codes[16].amount[3] == '4' ||
				Auth_Tx.TLF01_details.product_codes[16].amount[3] == '5' )
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"U",MRACON_FIELD95_SIZE);
		}
		else
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD," ",MRACON_FIELD95_SIZE);
		}
	}

		else if(MRACON_check_If_JCB_Transaction () == true)
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD," ",MRACON_FIELD95_SIZE);
		}

/*	if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_VISA)	== 0)
	{
		if( Auth_Tx.TLF01_details.product_codes[16].amount[1] == '1' ||
			Auth_Tx.TLF01_details.product_codes[16].amount[1] == '4')
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"L",MRACON_FIELD95_SIZE);
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[1] == '3' ||
				Auth_Tx.TLF01_details.product_codes[16].amount[1] == '2' )
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"U",MRACON_FIELD95_SIZE);
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[1] == '5' ||
				Auth_Tx.TLF01_details.product_codes[16].amount[1] == '7' )
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"I",MRACON_FIELD95_SIZE);
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[1] == '0' )
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD," ",MRACON_FIELD95_SIZE);
		}
		else
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"N",MRACON_FIELD95_SIZE);
		}
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_MASTERCARD) == 0)
	{
	   
	   if( Auth_Tx.TLF01_details.product_codes[16].amount[10] == '4' ||
		   Auth_Tx.TLF01_details.product_codes[16].amount[10] == '6' ||
		   Auth_Tx.TLF01_details.product_codes[16].amount[10] == '7' )
	   {
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"I",MRACON_FIELD95_SIZE);
	   }
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[10] == '3' )
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"L",MRACON_FIELD95_SIZE);
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[10] == '2' ||
				Auth_Tx.TLF01_details.product_codes[16].amount[10] == '1')
		{
			memset(Cr_Fld.TERMINAL_TYPE_XCD,'U',MRACON_FIELD95_SIZE);
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[10] == '0')
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD," ",MRACON_FIELD95_SIZE);
		}
		else
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"O",MRACON_FIELD95_SIZE);
		}
	}*/
	/*else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_AMEX)== 0)
	{
		
		strncpy(Cr_Fld.TERMINAL_TYPE_XCD,&Auth_Tx.amex_pos_entry_mode[3],MRACON_FIELD95_SIZE);
		if(strncmp(Cr_Fld.TERMINAL_TYPE_XCD,"1",1) == 0 ||
		   strncmp(Cr_Fld.TERMINAL_TYPE_XCD,"3",1) == 0 ||
		   strncmp(Cr_Fld.TERMINAL_TYPE_XCD,"X",1) == 0)
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"L",MRACON_FIELD95_SIZE);
		}
		else if(strncmp(Cr_Fld.TERMINAL_TYPE_XCD,"2",MRACON_FIELD95_SIZE) == 0 ||
		   strncmp(Cr_Fld.TERMINAL_TYPE_XCD,"4",1) == 0 ||
		   strncmp(Cr_Fld.TERMINAL_TYPE_XCD,"5",1) == 0 ||
		   strncmp(Cr_Fld.TERMINAL_TYPE_XCD,"Z",1) == 0)
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"U",1);
		}
		else if(strncmp(Cr_Fld.TERMINAL_TYPE_XCD,"S",MRACON_FIELD95_SIZE) == 0 ||
		   strncmp(Cr_Fld.TERMINAL_TYPE_XCD,"T",MRACON_FIELD95_SIZE) == 0)
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"I",MRACON_FIELD95_SIZE);
		}
		else if(strncmp(Cr_Fld.TERMINAL_TYPE_XCD,"0",MRACON_FIELD95_SIZE) == 0 ||
			strncmp(Cr_Fld.TERMINAL_TYPE_XCD,"9",MRACON_FIELD95_SIZE) == 0)
		{
			memset(Cr_Fld.TERMINAL_TYPE_XCD,SPACES,MRACON_FIELD95_SIZE);
		}
		else
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"O",MRACON_FIELD95_SIZE);
		}
	}*/
	/*else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_CUP)== 0)
	{
		if( strncmp(&Auth_Tx.TLF01_details.product_codes[16].amount[4] , "03" ,2)== 0 ||
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
		else
		{
			Cr_Fld.TERMINAL_TYPE_XCD[0] = SPACES ;
		}

	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_DINERS)== 0)
	{
		if(Auth_Tx.TLF01_details.product_codes[16].amount[3] == '1' ||
				Auth_Tx.TLF01_details.product_codes[16].amount[3] == '3'  )
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"L",MRACON_FIELD95_SIZE);
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[3] == '2' ||
				Auth_Tx.TLF01_details.product_codes[16].amount[3] == '4' ||
				Auth_Tx.TLF01_details.product_codes[16].amount[3] == '5' )
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"U",MRACON_FIELD95_SIZE);
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[3] == '9')
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD," ",MRACON_FIELD95_SIZE);
		}
		else
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"O",MRACON_FIELD95_SIZE);
		}
	}*/

	sprintf(&message[780],"%-1s\n",Cr_Fld.TERMINAL_TYPE_XCD);
}

void MRACON_build_request_field_96( )
{
	memset(Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD,SPACES,MRACON_FIELD96_SIZE);

	if(MRACON_check_If_JCB_Transaction () == true)
	{
		memset(Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD,SPACES,MRACON_FIELD96_SIZE);
	}

	if(MRACON_check_If_MC_Transaction () == true)
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
			   Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD[0] = SPACES ;
		   }
		   else
		   {
			   Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD[0] = 'O' ;
		   }
	}

	if(MRACON_check_If_VISA_Transaction () == true)
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
			Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD[0] = SPACES ;
		}
		else
		{
			Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD[0] = 'N' ;
		}
	}

	if(MRACON_check_If_AMEX_Transaction () == true)
	{
		if( Auth_Tx.TLF01_details.product_codes[16].amount[0] == '5')
		{
			strncpy(Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD,"C",MRACON_FIELD96_SIZE);
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[0] == '1' ||
				Auth_Tx.TLF01_details.product_codes[16].amount[0] == '6' )
		{
			strncpy(Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD,"K",MRACON_FIELD96_SIZE);
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[0] == '2')
		{
			strncpy(Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD,"M",MRACON_FIELD96_SIZE);
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[0] == '0')
		{
			strncpy(Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD," ",MRACON_FIELD96_SIZE);
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[0] == '3'||
				Auth_Tx.TLF01_details.product_codes[16].amount[0] == '4')
		{
			strncpy(Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD,"O",MRACON_FIELD96_SIZE);
		}
	}

	if(MRACON_check_If_CUP_Transaction () == true ||
			MRACON_Check_If_CUP_MP_Transaction() == true)
	{
		/*if(strncmp(Cr_Fld.USER_DATA_2_STRG,"5",1)== 0 || strncmp(Cr_Fld.USER_DATA_2_STRG,"6",1)== 0  )
			{
				strncpy(Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD,"C",1);
			}
			else if(strncmp(Cr_Fld.USER_DATA_2_STRG,"2",1)== 0)
			{
				strncpy(Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD,"M",1);
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
				Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD[0] = SPACES;
			}
			else
			{
				Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD[0] = 'O';
			}
	}

	if(MRACON_check_If_DINERS_Transaction () == true)
	{
		if(Auth_Tx.TLF01_details.product_codes[16].amount[0] == '5')
		{
			strncpy(Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD,"C",MRACON_FIELD96_SIZE);
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[0] == '8')
		{
			strncpy(Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD,"D",MRACON_FIELD96_SIZE);
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[0] == '6' ||
				Auth_Tx.TLF01_details.product_codes[16].amount[0] == '1')
		{
			strncpy(Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD,"K",MRACON_FIELD96_SIZE);
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[0] == '2')
		{
			strncpy(Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD,"M",MRACON_FIELD96_SIZE);
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[0] == '0')
		{
			strncpy(Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD," ",MRACON_FIELD96_SIZE);
		}
		else
		{
			strncpy(Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD,"O",MRACON_FIELD96_SIZE);
		}
	}

	/*if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_VISA)	== 0)
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
			Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD[0] = SPACES ;
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
		   Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD[0] = SPACES ;
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
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"C",MRACON_FIELD96_SIZE);
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[0] == '1' ||
				Auth_Tx.TLF01_details.product_codes[16].amount[0] == '6' )
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"K",MRACON_FIELD96_SIZE);
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[0] == '2')
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"M",MRACON_FIELD96_SIZE);
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[0] == '0')
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD," ",MRACON_FIELD96_SIZE);
		}
		else
		{
			strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"O",MRACON_FIELD96_SIZE);
		}
	}
	else if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_CUP) == 0)
	{*/
	/*	if(strncmp(Cr_Fld.USER_DATA_2_STRG,"5",1)== 0 || strncmp(Cr_Fld.USER_DATA_2_STRG,"6",1)== 0  )
		{
			strncpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"C",1);
		}
		else if(strncmp(Cr_Fld.USER_DATA_2_STRG,"2",1)== 0)
		{
			strncpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"M",1);
		}*/
		/*if(Auth_Tx.TLF01_details.product_codes[16].amount[1] == '2')
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
			Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD[0] = SPACES;
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
			strncpy(Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD,"C",MRACON_FIELD96_SIZE);
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[0] == '8')
		{
			strncpy(Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD,"D",MRACON_FIELD96_SIZE);
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[0] == '6' ||
				Auth_Tx.TLF01_details.product_codes[16].amount[0] == '1')
		{
			strncpy(Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD,"K",MRACON_FIELD96_SIZE);
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[0] == '2')
		{
			strncpy(Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD,"M",MRACON_FIELD96_SIZE);
		}
		else if(Auth_Tx.TLF01_details.product_codes[16].amount[0] == '0')
		{
			strncpy(Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD," ",MRACON_FIELD96_SIZE);
		}
		else
		{
			strncpy(Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD,"O",MRACON_FIELD96_SIZE);
		}
	}*/
	sprintf(&message[781],"%-1s\n",Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD);
}

void MRACON_build_request_field_97( )
{
	if(MRACON_check_If_MC_Transaction () == true ||
			MRACON_check_If_VISA_Transaction ()==true ||
			MRACON_check_If_CUP_Transaction ()==true ||
			MRACON_Check_If_CUP_MP_Transaction() == true ||
			MRACON_check_If_Fleet_Transaction() == true)
	{
		strncpy(Cr_Fld.TRANSACTION_POS_CONDITION_XCD,Auth_Tx.TLF01_details.pos_condition_code,MRACON_FIELD97_SIZE);

	}
	else
	{
		strncpy(Cr_Fld.TRANSACTION_POS_CONDITION_XCD," ",MRACON_FIELD97_SIZE);
	}
	sprintf(&message[782],"%-2s\n",Cr_Fld.TRANSACTION_POS_CONDITION_XCD);
}


/*********memset(Cr_Fld.AUTHZN_RQST_CRYPTO_VALID_XFLG,SPACES,1);**********/

/************memset(Cr_Fld.ATC_CARD_CNT,SPACES,5);***************/

/*********memset(Cr_Fld.ATC_HOST_CNT,SPACES,5);**************/

void MRACON_build_request_field_98( )
{
	memcpy(Cr_Fld.ATM_NETWORK_XID," ",MRACON_FIELD98_SIZE);
	sprintf(&message[784],"%-1s\n",Cr_Fld.ATM_NETWORK_XID);
}

void MRACON_build_request_field_99( )
{
	memset(Cr_Fld.RESERVED_01,SPACES,MRACON_FIELD99_SIZE);
	sprintf(&message[785],"%-1s\n",Cr_Fld.RESERVED_01);
}

void MRACON_build_request_field_100( )
{
	memset(Cr_Fld.CHECK_NUM,SPACES,MRACON_FIELD100_SIZE);
	sprintf(&message[786],"%-6s\n",Cr_Fld.CHECK_NUM);
}

void MRACON_build_request_field_101( )
{
	memset(Cr_Fld.TERML_VERIFICATION_RESULTS_XCD,SPACES,MRACON_FIELD101_SIZE);
	sprintf(&message[792],"%-10s\n",Cr_Fld.TERML_VERIFICATION_RESULTS_XCD);
}

void MRACON_build_request_field_102( )
{
	memset(Cr_Fld.CARD_VERIFICATION_RESULTS_XCD,SPACES,MRACON_FIELD102_SIZE);
	sprintf(&message[802],"%-10s\n",Cr_Fld.CARD_VERIFICATION_RESULTS_XCD);
}

void MRACON_build_request_field_103( )
{
	memset(Cr_Fld.AUTHZN_RQST_CRYPTO_VALID_XFLG,SPACES,MRACON_FIELD103_SIZE);
	sprintf(&message[812],"%-1s\n",Cr_Fld.AUTHZN_RQST_CRYPTO_VALID_XFLG);
}

void MRACON_build_request_field_104( )
{
	memset(Cr_Fld.ATC_CARD_CNT,SPACES,MRACON_FIELD104_SIZE);
	sprintf(&message[813],"%-5s\n",Cr_Fld.ATC_CARD_CNT);
}

void MRACON_build_request_field_105( )
{
	memset(Cr_Fld.ATC_HOST_CNT,SPACES,MRACON_FIELD105_SIZE);
	sprintf(&message[818],"%-5s\n",Cr_Fld.ATC_HOST_CNT);
}

void MRACON_build_request_field_106( )
{
	memset(Cr_Fld.RESERVED_03,SPACES,MRACON_FIELD106_SIZE);
	sprintf(&message[823],"%-2s\n",Cr_Fld.RESERVED_03);
}

void MRACON_build_request_field_107( )
{
	memset(Cr_Fld.TERML_TO_CRD_OFFLINE_LIMIT_XCD,SPACES,MRACON_FIELD107_SIZE);
	sprintf(&message[825],"%-2s\n",Cr_Fld.TERML_TO_CRD_OFFLINE_LIMIT_XCD);
}

void MRACON_build_request_field_108( )
{
	memset(Cr_Fld.TERMINAL_OFFLINE_LIMIT_MAX_XCD,SPACES,MRACON_FIELD108_SIZE);
	sprintf(&message[827],"%-2s\n",Cr_Fld.TERMINAL_OFFLINE_LIMIT_MAX_XCD);
}

void MRACON_build_request_field_109( )
{
	memset(Cr_Fld.RECURRING_AUTHORIZATION_XCD,SPACES,MRACON_FIELD109_SIZE);
	sprintf(&message[829],"%-2s\n",Cr_Fld.RECURRING_AUTHORIZATION_XCD);
}

void MRACON_build_request_field_110( )
{
	memset(Cr_Fld.RECURRING_AUTHZN_EXPIRATION_DT,SPACES,MRACON_FIELD110_SIZE);
	sprintf(&message[831],"%-8s\n",Cr_Fld.RECURRING_AUTHZN_EXPIRATION_DT);
}

void MRACON_build_request_field_111( )
{
	memset(Cr_Fld.CARD_ASSOCIATION_XFLG,SPACES,MRACON_FIELD111_SIZE);
	sprintf(&message[839],"%-1s\n",Cr_Fld.CARD_ASSOCIATION_XFLG);
}

void MRACON_build_request_field_112( )
{
	memset(Cr_Fld.CARD_INCENTIVE_XCD,SPACES,MRACON_FIELD112_SIZE);
	sprintf(&message[840],"%-1s\n",Cr_Fld.CARD_INCENTIVE_XCD);
}

void MRACON_build_request_field_113( )
{
	memset(Cr_Fld.CARD_STATUS_XCD,SPACES,MRACON_FIELD113_SIZE);
	sprintf(&message[841],"%-2s\n",Cr_Fld.CARD_STATUS_XCD);
}

void MRACON_build_request_field_114( )
{
	memset(Cr_Fld.CARD_STATUS_DT,SPACES,MRACON_FIELD114_SIZE);
	sprintf(&message[843],"%-8s\n",Cr_Fld.CARD_STATUS_DT);
}

	/********memset(Cr_Fld.PROCESSOR_REASON_XCD,SPACES,5);*****
	*****memcpy(&Cr_Fld.PROCESSOR_REASON_XCD[3],Auth_Tx.TLF01_details.response_code,2);**************/
void MRACON_build_request_field_115( )
{
	if(MRACON_check_If_MC_Transaction () == true ||
			MRACON_check_If_VISA_Transaction ()==true ||
			MRACON_check_If_CUP_Transaction ()==true||
			MRACON_check_If_JCB_Transaction ()==true ||
			MRACON_check_If_AMEX_Transaction () == true||
			MRACON_check_If_DINERS_Transaction () == true||
			MRACON_Check_If_CUP_MP_Transaction() == true ||
			MRACON_check_If_Fleet_Transaction() == true)
	{
		memset(Cr_Fld.PROCESSOR_REASON_XCD,SPACES,MRACON_FIELD115_SIZE);
		
		memcpy(Cr_Fld.PROCESSOR_REASON_XCD,Auth_Tx.TLF01_details.response_code,2);

	}

	sprintf(&message[851],"%-5s\n",Cr_Fld.PROCESSOR_REASON_XCD);
}

void MRACON_build_request_field_116( )
{
	if(strcmp(Auth_Tx.TLF01_details.product_codes[1].code,"stndin") == 0) 
	{
		memcpy(Cr_Fld.TRANSACTION_ADVICE_XCD,"A",MRACON_FIELD116_SIZE);
		sprintf(&message[856],"%-1s\n",Cr_Fld.TRANSACTION_ADVICE_XCD);
	}
	else
	{
		memset(Cr_Fld.TRANSACTION_ADVICE_XCD,SPACES,MRACON_FIELD116_SIZE);
		sprintf(&message[856],"%-1s\n",Cr_Fld.TRANSACTION_ADVICE_XCD);
	}
}

void MRACON_build_request_field_117( )
{
	memset(Cr_Fld.MERCHANT_XID,SPACES,MRACON_FIELD117_SIZE);
	if(MRACON_check_If_CUP_Transaction ()  == true)
	{
		memcpy(Cr_Fld.MERCHANT_XID,Auth_Tx.TLF01_details.visa_merchant_id,MRACON_FIELD117_SIZE);
	}
	else
	{
		memcpy(Cr_Fld.MERCHANT_XID,Auth_Tx.TLF01_details.merchant_id,MRACON_FIELD117_SIZE);
	}
	sprintf(&message[857],"%-16s\n",Cr_Fld.MERCHANT_XID);
}

void MRACON_build_request_field_118( )
{
	memset(Cr_Fld.CARD_ORDER_TYPE_XCD,SPACES,MRACON_FIELD118_SIZE);
	sprintf(&message[873],"%-1s\n",Cr_Fld.CARD_ORDER_TYPE_XCD);
}

void MRACON_build_request_field_119( )
{
	memset(Cr_Fld.TRANSACTION_CASHBACK_AMT,SPACES,MRACON_FIELD119_SIZE);
	sprintf(&message[874],"%-13s\n",Cr_Fld.TRANSACTION_CASHBACK_AMT);
}

void MRACON_build_request_field_120( )
{
	memset(Cr_Fld.USER_DATA_6_NUM,SPACES,MRACON_FIELD120_SIZE);

	if(MRACON_check_If_MC_Transaction () == true ||
	   MRACON_check_If_VISA_Transaction () == true ||
	   MRACON_check_If_AMEX_Transaction () == true ||
	   MRACON_check_If_CUP_Transaction () == true ||
	   MRACON_check_If_DINERS_Transaction () == true||
	   MRACON_Check_If_CUP_MP_Transaction()==true||
	   MRACON_check_If_Fleet_Transaction()==true)
	{
		memcpy(Cr_Fld.USER_DATA_6_NUM,Auth_Tx.TLF01_details.profile,MRACON_FIELD120_SIZE_ECI);
	}

	else if(MRACON_check_If_JCB_Transaction () == true)
	{
		memcpy(Cr_Fld.USER_DATA_6_NUM," ",MRACON_FIELD120_SIZE);
	}

/*	if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_AMEX)	== 0)
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
		memset(Cr_Fld.USER_DATA_6_NUM,SPACES,MRACON_FIELD120_SIZE);
	}
	else
	{
		Cr_Fld.USER_DATA_6_NUM[0]= '2';
		strncpy(&Cr_Fld.USER_DATA_6_NUM[1],Auth_Tx.TLF01_details.profile,2);
	}*/

	sprintf(&message[887],"%-13s\n",Cr_Fld.USER_DATA_6_NUM);
}

void MRACON_build_request_field_121( )
{
	memset(Cr_Fld.USER_DATA_7_STRG,SPACES,MRACON_FIELD121_SIZE);
	if(MRACON_check_If_MC_Transaction () == true ||
	   MRACON_check_If_VISA_Transaction () == true ||
	   MRACON_check_If_AMEX_Transaction () == true ||
	   MRACON_check_If_CUP_Transaction () == true ||
	   MRACON_check_If_DINERS_Transaction () == true ||
	   MRACON_Check_If_CUP_MP_Transaction()==true||
	   MRACON_check_If_Fleet_Transaction()==true)
	{
		memcpy(Cr_Fld.USER_DATA_7_STRG,Auth_Tx.TLF01_details.product_codes[16].amount,sizeof(Auth_Tx.TLF01_details.product_codes[16].amount));
	}
	else if(MRACON_check_If_JCB_Transaction () == true)
	{
		memcpy(Cr_Fld.USER_DATA_7_STRG," ",MRACON_FIELD121_SIZE);
	}
	/*if(strcmp(Auth_Tx.BIN01_details.card_family, CARD_AMEX)	== 0)
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
		
	}*/

	sprintf(&message[900],"%-40s\n",Cr_Fld.USER_DATA_7_STRG);
}

void MRACON_build_request_field_122( )
{
	memset(Cr_Fld.PAYMENT_INSTRUMENT_XID,SPACES,MRACON_FIELD122_SIZE);
	sprintf(&message[940],"%-30s\n",Cr_Fld.PAYMENT_INSTRUMENT_XID);
}

void MRACON_build_request_field_123( )
{
	memset(Cr_Fld.AVS_REQUEST_XCD,SPACES,MRACON_FIELD123_SIZE);
	sprintf(&message[970],"%-1s\n",Cr_Fld.AVS_REQUEST_XCD);
}

void MRACON_build_request_field_124( )
{
	memset(Cr_Fld.CVR_OFFLINEPIN_VERIFYPERF_XFLG,SPACES,MRACON_FIELD124_SIZE);
	sprintf(&message[971],"%-1s\n",Cr_Fld.CVR_OFFLINEPIN_VERIFYPERF_XFLG);
}

void MRACON_build_request_field_125( )
{
	memset(Cr_Fld.CVR_OFFLINEPIN_VERIFYSTAT_XFLG,SPACES,MRACON_FIELD125_SIZE);
	sprintf(&message[972],"%-1s\n",Cr_Fld.CVR_OFFLINEPIN_VERIFYSTAT_XFLG);
}

void MRACON_build_request_field_126( )
{
	memset(Cr_Fld.CVR_PIN_TRYLIMIT_EXCEED_XFLG,SPACES,MRACON_FIELD126_SIZE);
	sprintf(&message[973],"%-1s\n",Cr_Fld.CVR_PIN_TRYLIMIT_EXCEED_XFLG);
}

void MRACON_build_request_field_127( )
{
	memset(Cr_Fld.POS_TERMINAL_ATTEND_XFLG,SPACES,MRACON_FIELD127_SIZE);
	sprintf(&message[974],"%-1s\n",Cr_Fld.POS_TERMINAL_ATTEND_XFLG);
}

void MRACON_build_request_field_128( )
{
	memset(Cr_Fld.POS_OFF_PREMISES_XFLG,SPACES,MRACON_FIELD128_SIZE);
	sprintf(&message[975],"%-1s\n",Cr_Fld.POS_OFF_PREMISES_XFLG);
}

void MRACON_build_request_field_129( )
{
	memset(Cr_Fld.POS_CARD_CAPTURE_XFLG,SPACES,MRACON_FIELD129_SIZE);
	sprintf(&message[976],"%-1s\n",Cr_Fld.POS_CARD_CAPTURE_XFLG);
}

void MRACON_build_request_field_130( )
{
	memset(Cr_Fld.POS_SECURITY_XFLG,SPACES,MRACON_FIELD130_SIZE);
	sprintf(&message[977],"%-1s\n",Cr_Fld.POS_SECURITY_XFLG);
}

void MRACON_build_request_field_131( )
{
	memset(Cr_Fld.AUTHORIZATION_XID,SPACES,MRACON_FIELD131_SIZE);
	memcpy(Cr_Fld.AUTHORIZATION_XID,Auth_Tx.TLF01_details.auth_number,MRACON_FIELD131_SIZE);
	sprintf(&message[978],"%-6s\n",Cr_Fld.AUTHORIZATION_XID);
}

void MRACON_build_request_field_132( )
{
	memset(Cr_Fld.USER_DATA_8_STRG,SPACES,MRACON_FIELD132_SIZE);
	memcpy(Cr_Fld.USER_DATA_8_STRG,&Auth_Tx.TLF01_details.pos_entry_mode,4);
	sprintf(&message[984],"%-10s\n",Cr_Fld.USER_DATA_8_STRG);
}

void MRACON_build_request_field_133( )
{
	memset(Cr_Fld.USER_DATA_9_STRG,SPACES,MRACON_FIELD133_SIZE);
	memcpy(Cr_Fld.USER_DATA_9_STRG,Auth_Tx.TLF01_details.terminal_id,MRACON_FIELD133_SIZE);
	sprintf(&message[994],"%-10s\n",Cr_Fld.USER_DATA_9_STRG);
}

void MRACON_build_request_field_134( )
{
	memset(Cr_Fld.USER_INDICATOR_5_XCD,SPACES,MRACON_FIELD134_SIZE);
	sprintf(&message[1004],"%-1s\n",Cr_Fld.USER_INDICATOR_5_XCD);
}

void MRACON_build_request_field_135( )
{
	memset(Cr_Fld.USER_INDICATOR_6_XCD,SPACES,MRACON_FIELD135_SIZE);
	sprintf(&message[1005],"%-1s\n",Cr_Fld.USER_INDICATOR_6_XCD);
}

void MRACON_build_request_field_136( )
{
	memset(Cr_Fld.USER_INDICATOR_7_XCD,SPACES,MRACON_FIELD136_SIZE);
	strncpy(Cr_Fld.USER_INDICATOR_7_XCD,Auth_Tx.TLF01_details.service_code,3);
	sprintf(&message[1006],"%-5s\n",Cr_Fld.USER_INDICATOR_7_XCD);
}

void MRACON_build_request_field_137( )
{
	memset(Cr_Fld.USER_INDICATOR_8_XCD,SPACES,MRACON_FIELD137_SIZE);
	sprintf(&message[1011],"%-5s\n",Cr_Fld.USER_INDICATOR_8_XCD);
}

void MRACON_build_request_field_138( )
{
	memset(Cr_Fld.MODEL_CONTROL_1_XCD,SPACES,MRACON_FIELD138_SIZE);
	sprintf(&message[1016],"%-1s\n",Cr_Fld.MODEL_CONTROL_1_XCD);
}

void MRACON_build_request_field_139( )
{
	memset(Cr_Fld.MODEL_CONTROL_2_XCD,SPACES,MRACON_FIELD139_SIZE);
	sprintf(&message[1017],"%-1s\n",Cr_Fld.MODEL_CONTROL_2_XCD);
}

void MRACON_build_request_field_140( )
{
	memset(Cr_Fld.MODEL_CONTROL_3_XCD,SPACES,MRACON_FIELD140_SIZE);
	sprintf(&message[1018],"%-1s\n",Cr_Fld.MODEL_CONTROL_3_XCD);
}

void MRACON_build_request_field_141( )
{
	memset(Cr_Fld.MODEL_CONTROL_4_XCD,SPACES,MRACON_FIELD141_SIZE);
	sprintf(&message[1019],"%-1s\n",Cr_Fld.MODEL_CONTROL_4_XCD);
}

void MRACON_build_request_field_142( )
{
	if(strcmp(Auth_Tx.TLF01_details.response_code,"54") == 0)
	{
		strncpy(Cr_Fld.AUTHORIZATION_EXPIRE_DT_XFLG,"I",MRACON_FIELD142_SIZE);
	}
	else
	{
		strncpy(Cr_Fld.AUTHORIZATION_EXPIRE_DT_XFLG,"V",MRACON_FIELD142_SIZE);
	}

	sprintf(&message[1020],"%-1s\n",Cr_Fld.AUTHORIZATION_EXPIRE_DT_XFLG);
}

void MRACON_build_request_field_143( )
{
	memset(Cr_Fld.AUTH_SECONDARY_VERIFY_XCD,SPACES,MRACON_FIELD143_SIZE);
	sprintf(&message[1021],"%-1s\n",Cr_Fld.AUTH_SECONDARY_VERIFY_XCD);
}

void MRACON_build_request_field_144()
{
	memset(Cr_Fld.AUTHORIZATION_RESPONSE_XCD,SPACES,MRACON_FIELD144_SIZE);
	sprintf(&message[1022],"%s",Cr_Fld.AUTHORIZATION_RESPONSE_XCD);
	return ;
}

void EndProcess()
{
      CHAR Buffer[100] = "";

   sprintf( Buffer, "Shutting down %s Service, version %s",
            ServiceName, Version );
  /* LogEvent( Buffer, INFO_MSG ); */
   mracon_log_message(2,1,Buffer,"EndProcess",0);
   strcat( Buffer, "\n" );
   pteipc_shutdown_multiple_instance_app();
   /* PRINT( Buffer ); */


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
   CHAR           Buff[256] = {0};
   CHAR           ConnectionId[MAX_ORIG_INFO_SIZE+1] = {0} ;
   LONG           ret_code;
   INT            nIndex = 0 ;
   INT            ret_val;


   memset (&Auth_Tx, 0x00, sizeof (AUTH_TX)) ;

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
					sprintf(msgBuffer,"Msg to JMQ ==> msglen:%d message:%s",msglen,message);
					mracon_log_message(2,1,msgBuffer,"PinnacleMsgHandler",0);
					/*sprintf(msgBuffer,"count = %d, msglen:%dapplication_que_name:%scontrol_que_name:%smessage:%s",count, msglen,application_que_name,control_que_name,message);
					sprintf(msgBuffer,"msglen:%dmessage:%s",msglen,message);
					sprintf(msgBuffer,"message:%s",message);
					LogEvent( msgBuffer, INFO_MSG );
					mracon_log_message(2,1,msgBuffer,"PinnacleMsgHandler",0);
					fprintf (fp1, "count:%dmsglen:%d message:%s\n",count,msglen,message);*/

						/*if (!fp1)
							{
								sprintf(fileinfo,"There was a problem opening %s for writing\n",outname);
								LogEvent( fileinfo, INFO_MSG );
								mracon_log_message(2,1,fileinfo,"PinnacleMsgHandler",0);
							}
						else
							{*/
								 /*****if(tracing == 1)
								{
									fprintf (fp1, "count:%d message:%s\n",count,message);
								}*****/
								/*sprintf(fileinfo,"There was a problem opening %d for writing\n",name);
								fprintf (fp1, "%s\n", fileinfo);
								fclose(fp); */
							/* } */


						 /*else if(lReturnCode == QUE_ER_TIMEOUT)
							{
							sprintf( strBuffer,"Error:Timed Out on Receive,  error code: %ld",strTemp, lReturnCode );
								 LogEvent( strBuffer, ALERT_MSG );
								 mracon_log_message(2,1,strBuffer,"PinnacleMsgHandler",0);
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
												sprintf( buffer,
															"Pinged -> Mracon Service: %s, version %s",
															ServiceName, Version );
												TxUtils_Send_Msg_To_Operator( 1, buffer, 1, INFO_MSG,
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
       break;
	     case MT_GET_STATS:
			         (void)send_transaction_statistics( p_msg_in );
			         break;


      default:
          strcpy (strTemp, "Unknown message type received in the Pinnacle Message Handler") ;
          TxUtils_Send_Msg_To_Operator
				  (1, strTemp,
					1, ERROR_MSG,"MraconHandler.c", 4, FATAL_ERROR,0, 0, 0) ;
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
      mracon_log_message( 1, 2, err_msg, "send_transaction_statistics",1 );
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
   /*printf("Mracon:Recived Signal Handler\n");*/
   memcpy(javaEndProcessSignalled,"1",1);
   sleep( 1 );
}


INT MraconHandler_initialize(CHAR* appName, CHAR* serName) {
	INT ret_val = true;
	BYTE ret_code = true;
	CHAR err_buf[100] = "";
	CHAR tf_filename[256] = { 0 };

	CHAR tmpstr[256] = { 0 };
	CHAR ini_info[MAX_INI_INFO_SIZE] = { 0 };
	CHAR filename[MAX_FILENAME_SIZE] = { 0 };
	CHAR tmpstring[MAX_INI_INFO_SIZE] = { 0 };
	CHAR MRA_Version[] = "ATP_11.1.0";

	/*long td_val;
	 td_val=pthread_self();

	 printf("\nIntialize:%ld\n",td_val);*/
	GetAscendentConfigDirectory(tmpstr);
	sprintf(tf_filename, "%stf.ini", tmpstr);

	MRACON_read_CUP_merchant_indicator();

	mracon_get_error_warning_file_name_path();
	/*get record length from ini file*/
	GetPrivateProfileString("MRACON", /* points to section name       */
	"WORKFLOW", /* points to key name           */
	"modelSTUB", /* points to default string     */
	profile_str, /* points to destination buffer */
	sizeof(profile_str) - 1, /* size of destination buffer   */
	tf_filename); /* points to ini filename       */

	GetPrivateProfileString("CUPMIDSECTION", /* points to section name       */
	"CUP_MID_INDICATOR", /* points to key name           */
	"0", /* points to default string     */
	ini_info, /* points to destination buffer */
	MAX_INI_INFO_SIZE - 1, /* size of destination buffer   */
	filename /* points to ini filename       */
	);

	/* Convert entry from a string into a number. */
	memset(tmpstring, 0x00, MAX_INI_INFO_SIZE);
	strncpy(tmpstring, ini_info, MAX_INI_INFO_SIZE - 1);
	strncpy(CUP_merchant_indicator, tmpstring, CUP_MID_INDICATOR_LEN);
	if (strlen(CUP_merchant_indicator) == 0) {
		strncpy(CUP_merchant_indicator,
		CUP_MID_INDICATOR_DEFAULT,
		CUP_MID_INDICATOR_LEN);
	}

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
	sprintf(strTemp,
			"Attempting to start %s, Java_version %d and MRACON Version:%s",
			strAppName, Version, MRA_Version);
	/* LogEvent( strTemp, INFO_MSG ); */
	mracon_log_message(2, 1, strTemp, "MraconHandler_initialize", 0);
	/*  Create a table to be used for shared memory between multiple
	 *  instances of this  Module.  It contains three fields:
	 *  # of consecutive timeouts, # of active transactions,
	 *  and the current state of the host.
	 */

	/* Fetch MRA01 table to get the count during initialization */
	/* ret_code = Mracon_Get_mar01_table(); */
	if (ret_code == true) {
		/* LogEvent( "Read MRA01 table", INFO_MSG ); */
		mracon_log_message(2, 1, "Read MRA01 table",
				"MraconHandler_initialize", 0);
	} else {

		mracon_log_message(2, 1, "Failed to read MRA01 table",
				"MraconHandler_initialize", 0);
	}

	/*#ifdef _DEBUG */
	if (!pteipc_init_multiple_instance_app(strAppName, ServiceName, "pte_ipc"))
	/*#endif*/
	{
		sprintf(strTemp, "%s: Intialize: Failed to Create XIPC queues",
				strAppName);

		mracon_log_message(2, 3, strTemp, "MraconHandler_initialize", 0);
		/* signal that we are ready to shut down and quit */
		/* #ifdef _DEBUG*/
		pteipc_shutdown_multiple_instance_app();
		/* #endif */
		MainProcessDone = 1;

		return INTIALIZE_QUEUES_ERROR;

	}

	/* ret_val = create_shared_mem_table( err_buf );*/

	sprintf(strTemp, "Started the MraconHandler Service: %s", ServiceName);

	mracon_log_message(2, 1, strTemp, "MraconHandler_initialize", 0);

	return INTIALIZE_QUEUES_NO_ERROR;
}

CHAR* MraconHandler_sendRequestMessage(CHAR* srvNam) {
	LONG lReturnCode = 0L;
	CHAR strTemp[256] = { 0 };
	pPTE_MSG p_msg = 0;
	INT err_ctr = 0;
	CHAR strBuffer[256] = { 0 };
	CHAR strVal[1] = { 0 };
	INT msglen1 = 0;
	CHAR fileinfo[256] = { 0 };
	CHAR ErrorMsg[100] = { 0 };
	MRA01 sMRA01;
	pBYTE pATEMessageDataData = 0;
	pPTE_MSG_DATA pATEMessageData = 0;
	CHAR resp_buf[15] = { 0 };
	BYTE MessageType = 0;
	char unique_tran_str_id[21] = { 0 };
	CHAR temp_unique_tran[15] = { 0 };
	BYTE originator_queue[7] = { 0 };
	INT LEN_SENDER = 0;
	INT LEN_Txn_ID = 0;

	/* request to updatds */

	memset(&Auth_Tx, 0x00, sizeof(AUTH_TX));
	memset(&sMRA01, 0x00, sizeof(MRA01));
	memset(&message, 0x00, sizeof(message));

	if (strcmp(javaEndProcessSignalled, "1") != 0) {
		mracon_log_message(2, 1, "After MRACON receives the message",
				"MraconHandler_sendRequestMessage", 0);
		p_msg = pteipc_receive(application_que_name, control_que_name, 5,
				&lReturnCode);
		mracon_log_message(2, 1, "Xcom queue read",
				"MraconHandler_sendRequestMessage", 0);
		if (p_msg != NULL_PTR)
		{
			/* mracon_log_message(2,1,"Msg Read successfully","Java_MraconHandler_sendRequestMessage",0); */
			count++;
			err_ctr = 0;
			MessageType = ptemsg_get_msg_type(p_msg);
			PinnacleMsgHandler(p_msg);
			if (MessageType == MT_AUTH_REQUEST)
			{
				/* mracon_log_message(2,1,"It is an Auth Request Msg","Java_MraconHandler_sendRequestMessage",0); */
				pATEMessageData = ptemsg_get_pte_msg_data(p_msg);
				pATEMessageDataData = ptemsg_get_pte_msg_data_data(
						pATEMessageData);
				memset(&Auth_Tx, 0x00, sizeof(AUTH_TX));
				memcpy(&Auth_Tx, (AUTH_TX*) pATEMessageDataData,
						sizeof Auth_Tx);
				strcpy(originator_queue, ptemsg_get_msg_orig_queue(p_msg));
				LEN_SENDER = sizeof(sMRA01.mracon_sender_name) - 1;
				strncpy(sMRA01.mracon_sender_name, originator_queue,
						LEN_SENDER);
				if (strlen(Auth_Tx.TLF01_details.primary_key.transaction_id)
						!= '\0')
				{
					strncpy(sMRA01.transaction_id,
							Auth_Tx.TLF01_details.primary_key.transaction_id,
							TRANSACTION_ID);
				}
				else
				{
					ptetime_get_strtime(unique_tran_str_id);
					strcpy(Auth_Tx.TLF01_details.date_yyyymmdd, date_str);
					strcpy(Auth_Tx.TLF01_details.time_hhmmss, time_str);
					/* We have to now prepare 20 digit unique Transaction ID.
					 unique_tran_str_id will be having 14 digit length
					 now we will make it as 20 by adding YYMMDD in the beginning of it.
					 20 byte will look like
					 YYMMDD"unique_tran_str_id"
					 for example if unique_tran_str_id is "1444752896.334" then it will become
					 YYMMDD1444752896.334
					 Based on ther terminal length we will copy data from new unique_tran_str_id
					 */
					strncpy(temp_unique_tran, unique_tran_str_id, 14);
					memset(unique_tran_str_id, 0x00,
							sizeof(unique_tran_str_id));

					strncpy(unique_tran_str_id,
							Auth_Tx.TLF01_details.date_yyyymmdd + 2, 6); /* YYMMDD */
					strncat(unique_tran_str_id, temp_unique_tran, 14);
					LEN_Txn_ID = strlen(unique_tran_str_id);
					strncpy(sMRA01.transaction_id, unique_tran_str_id,
							LEN_Txn_ID);

				}

				if (strlen(time_date) != '\0')
				{
					strncpy(sMRA01.transaction_send_time, date_str, 8);
					strncat(sMRA01.transaction_send_time, time_str, 6);
					strncat(sMRA01.transaction_send_time, time_date + 20, 3);
				}

				if (strlen(Auth_Tx.TLF01_details.system_date) != '\0')
				{
					strncpy(sMRA01.system_date,
							Auth_Tx.TLF01_details.system_date, SYSTEM_DATE);
				}
				else
				{
					strncpy(sMRA01.system_date, date_str, 8);
				}

				mracon_Send_Request_To_Service(fconds_que_name,
												MT_DB_REQUEST,
														ST1_DB_INSERT,
															ST2_NONE,
															MRA01_DATA, (pBYTE) &sMRA01, sizeof sMRA01);
			}
			else
			{
				memset(strBuffer, 0x00, strlen(strBuffer));
				sprintf(strBuffer,
						" \n Message Type = %d and is not 100(MT_AUTH_REQUEST) ",
						MessageType);
				mracon_log_message(2, 1, strBuffer,
						"MraconHandler_sendRequestMessage", 0);

			}
			free(p_msg);

		}
		else if (lReturnCode != QUE_ER_TIMEOUT)
		{

			pteipc_get_errormsg(lReturnCode, strTemp);

			sprintf(strBuffer,
					"Error on pteipc_receive OR Error in Receving Message %s , error code: %ld",
					strTemp, lReturnCode);
			/* LogEvent( strBuffer, ALERT_MSG ); */
			mracon_log_message(2, 2, strBuffer,
					"MraconHandler_sendRequestMessage", 0);
			memcpy(message, strBuffer, sizeof(strBuffer));
			/* There is a problem with the queue.  Delay 1 second
			 * so as to not flood Applnk's queue and syslog.
			 */
			/* #ifdef WIN32
			 Sleep( 1000 );
			 #else
			 usleep( 1000000 );
			 #endif*/

			/* Inform Operators if service's queue is no longer readable. */
			if (++err_ctr > 10) {
				err_ctr = 0;
				sprintf(strBuffer, "%s. UNABLE TO ACCESS QUEUE!!!!",
						ServiceName);
				/* LogEvent( strBuffer, ALERT_MSG ); */
				mracon_log_message(2, 2, strBuffer,
						"MraconHandler_sendRequestMessage", 0);
				/*#ifdef WIN32
				 Sleep (5000);
				 #else
				 usleep (5000000);
				 #endif*/

			}
		}
		else
		{

			memset(&message, 0x00, sizeof(message));

		}
		mracon_log_message(2, 1, "Sendin message to JMS",
				"MraconHandler_sendRequestMessage", 0);
		return message;
	}
	else
	{

		/* Shutting down, need to change state before exiting Xipc. */
		/*(void)set_host_state( DOWN, ErrorMsg ); */
		pteipc_shutdown_multiple_instance_app();
		return javaEndProcessSignalled;
	}

}

void MraconHandler_receiveResponseMsg(char* ResponseMsg)
 {
	CHAR ResponseBuffer[5000]={0};
	/*CHAR respn_msg[5000]={0};*/
	CHAR strTemp [256] = {0} ;
	INT      msglen=0;



	if(strcmp(javaEndProcessSignalled,"1") != 0)
	{
		/*str2 = (*env2)->GetStringUTFChars(env2, ResponseMessage, NULL);*/
		if (ResponseMsg == NULL)
		{
			strcpy (strTemp,"receiveResponseMsg:ResponseMsg is NULL");
					
			mracon_log_message(2,3,strTemp,"MraconHandler_receiveResponseMsg",0);
		}
				/*memcpy(respn_msg,str2,sizeof(str2));*/
		msglen=strlen(ResponseMsg);

		sprintf(ResponseBuffer,"msglen:%dresponse_msg:%s",msglen,ResponseMsg);
				
		mracon_log_message(2,1,ResponseBuffer,"MraconHandler_receiveResponseMsg",0);
		/*(*env2)->ReleaseStringUTFChars(env2, respmsg, str2);*/

		/* Update transaction statistics. */
		/*update_txn_stats( &Txn_Counts, (pAUTH_TX)NULL_PTR, RECEIVED ); */

		/* static long count = 0; */
		/* count++; */
		/* fprintf (fp2, "count:%d respbuffer:%s\n",count,respbuffer);*/

	}
	else
	{
		/* Shutting down, need to change state before exiting Xipc. */
		/*(void)set_host_state( DOWN, ErrorMsg ); */
		pteipc_shutdown_multiple_instance_app();
	}

 }

void MraconHandler_terminate()
{

	CHAR strTemp [256] = {0} ;
	CHAR ServiceName[20]={0};


	strcpy(ServiceName,"MraconHandler");
	sprintf( strTemp,
				 "Shutting down the MraconHandler Service: %s",
				ServiceName);
	/* LogEvent( strTemp, INFO_MSG ); */
	mracon_log_message(2,1,strTemp,"MraconHandler_terminate",0);
	/* Log out of XIPC */
	/* #ifdef _DEBUG */
	pteipc_shutdown_multiple_instance_app();
	/* #endif */
	MainProcessDone = 1 ;
}

INT MraconHandler_getLibFileVersion()
{
	 CHAR version_buf[5]={0};
	 sprintf(version_buf,"Mracon_Version:%s",Version);
	 
	 mracon_log_message(2,1,version_buf,"MraconHandler_getLibFileVersion",0);
	/* return (*env4)->NewStringUTF(env4,Version);
	return(Version); */
		return(1);

 }

INT MRACON_check_If_CUP_Transaction (void)
{
	if (0 == strncmp(Auth_Tx.TLF01_details.product_code,CUP_CARD_INDICATOR_FLAG,2))
	{
		return true ;
	}
	else if (0 == strncmp(Auth_Tx.TLF01_details.merchant_id,CUP_merchant_indicator,CUP_MID_INDICATOR_LEN))
	{
		if((true == MRACON_check_If_VISA_Transaction ()) ||
		   (true == MRACON_check_If_MC_Transaction()))
		{
			return true ;
		}
		else
		{
			strncpy(Auth_Tx.TLF01_details.product_code,CUP_CARD_INDICATOR_FLAG,2);
			return true ;
		}
	}
/* check for VOICE Txn in GUI */
	else if(strncmp(Auth_Tx.BIN01_details.card_family,CARD_CUP,2)==0)
	{
		return true ;
	}
	else
	{
		return false;
	}
}

INT MRACON_Check_If_CUP_MP_Transaction (void)
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

INT MRACON_check_If_VISA_Transaction (void)
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

INT MRACON_check_If_MC_Transaction (void)
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
INT MRACON_check_If_JCB_Transaction (void)
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
INT MRACON_check_If_AMEX_Transaction (void)
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

INT MRACON_check_If_DINERS_Transaction (void)
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

INT MRACON_check_If_Fleet_Transaction (void)
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
 *  NAME:        MRACON_read_CUP_merchant_indicator
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
INT MRACON_read_CUP_merchant_indicator( void )
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
			    CUP_MID_INDICATOR_LEN);/* Default Value */
   }
   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         MRACON_read_CUP_MP_Bin_range_len_1
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
INT MRACON_read_CUP_MP_Bin_range_len_1( void )
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
 *  NAME:         MRACON_read_CUP_MP_Bin_range_len_2
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
INT MRACON_read_CUP_MP_Bin_range_len_2( void )
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
 *  NAME:         MRACON_read_CUP_MP_Bin_range_len_3
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
INT MRACON_read_CUP_MP_Bin_range_len_3( void )
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
 *  NAME:         MRACON_read_CUP_MP_Bin_range_len_4
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
INT MRACON_read_CUP_MP_Bin_range_len_4( void )
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
 *  NAME:         MRACON_read_CUP_MP_Bin_range_len_5
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
INT MRACON_read_CUP_MP_Bin_range_len_5( void )
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
 *  NAME:         MRACON_read_CUP_MP_Bin_range_1_indicator
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
INT MRACON_read_CUP_MP_Bin_range_1_indicator( void )
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
 *  NAME:         MRACON_read_CUP_MP_Bin_range_2_indicator
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
INT MRACON_read_CUP_MP_Bin_range_2_indicator( void )
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
   strncpy( CUP_MP_Bin_range_2, tmpstring, CUP_MP_Bin_range_len_2 );
   return( ret_val );
}
/******************************************************************************
 *
 *  NAME:         MRACON_read_CUP_MP_Bin_range_3_indicator
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
INT MRACON_read_CUP_MP_Bin_range_3_indicator( void )
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
 *  NAME:         MRACON_read_CUP_MP_Bin_range_4_indicator
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
INT MRACON_read_CUP_MP_Bin_range_4_indicator( void )
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
 *  NAME:         MRACON_read_CUP_MP_Bin_range_5_indicator
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
INT MRACON_read_CUP_MP_Bin_range_5_indicator( void )
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


BYTE mracon_Send_Request_To_Service (BYTE* que_name, BYTE nType, BYTE nSubType1,
							  BYTE nSubType2, BYTE nAppDataType,
							  pBYTE p_data,   INT nDataSize)
{
	LONG retCode = 0L ;
	pPTE_MSG p_msg_out = NULL_PTR ;
	INT nBufferSize = 0 ;
	char strError[512] = {0} ;
    CHAR logger_que_name[256] = {0} ;
	CHAR orig_comms_info [MAX_ORIG_INFO_SIZE] = {0} ;

	memset (p_buffer, 0, sizeof p_buffer) ;
	memcpy (p_buffer, &Auth_Tx, sizeof(Auth_Tx)) ;

	if (p_data != 0)
		memcpy (p_buffer + sizeof( Auth_Tx), p_data, nDataSize) ;

	nBufferSize = sizeof(Auth_Tx) + nDataSize ;

	p_msg_out = ptemsg_build_msg (  nType,
								    nSubType1,
								    nSubType2,
								    que_name,
								    application_que_name,
								    (pBYTE) p_buffer,
								    nBufferSize,
								    nAppDataType) ;

	if (p_msg_out == NULL_PTR)
	{
		strcpy (strError,"Insufficient Memory for building terminal message") ;
		mracon_log_message(1,3,strError,"mracon_Send_Request_To_Service",1);

		/*Generate_Usage_Error_Message (SET_SYSTEM_MONITOR, strError,
									"Send_Request_To_Service",
									GS_TXN_SYSTEM_ERROR,
									SYSTEM_MALFUNCTION,Auth_Tx) ;*/
		return SYSTEM_ERROR ;
 	}

	retCode = pteipc_send (p_msg_out, que_name) ;
	free (p_msg_out) ;

	if (retCode != 0)
	{
		sprintf (strError,"Error %ld has occurred sending message to %s\n", retCode, que_name) ;
		mracon_log_message(1,3,strError,"mracon_Send_Request_To_Service",1);
		/*Generate_Usage_Error_Message (	SET_SYSTEM_MONITOR, strError,
										"Send_Request_To_Service",
										GS_TXN_SYSTEM_ERROR,
										SYSTEM_MALFUNCTION,Auth_Tx) ;*/
		return SYSTEM_ERROR ;
	}
    else
    {
    	mracon_log_message( 2, 2,"Txn posted to mrads", "mracon_Send_Request_To_Service", 0 );
    }

	return DEVICE_OK ;
} /* Send_Request_To_Service */



BYTE Mracon_Get_mar01_table()
{
	pPTE_MSG      p_msg_out				= NULL_PTR;
	pPTE_MSG      p_msg_in				= NULL_PTR;;
	pBYTE         p_data     			= NULL_PTR;
	pPTE_MSG_DATA p_msg_data 			= NULL_PTR;
	CHAR        error_msg[256] 			= {0};
	CHAR        buffer[sizeof (MRA01)]	= {0};
	CHAR 		strBuffer[256] 			= {0};
	LONG 		retcode 				=  0;
	CHAR        temp_str[100] 			= {0};
	MRA01		mra01					;
	MONITOR_STATS monitor_stats			;
	CHAR		asci_mra_sent_count[21] = {0};
	INT			temp_count				= 0;
	LONG        data_len, len			= 0;
	p_msg_out = ptemsg_build_msg( MT_GET_STATS,
								  0,
								  0,
								  fconds_que_name,
								  interactive_que_name,
								  (pBYTE)buffer,
								  sizeof( buffer ),
								  MRA01_DATA );
	if( p_msg_out == NULL_PTR )
	{
		 sprintf( strBuffer,"Fetched the entire MAR01 Table" );
		 LogEvent( strBuffer, INFO_MSG );
		 return false;
	}

	 p_msg_in = pteipc_send_receive( p_msg_out, fconds_que_name, interactive_que_name, &retcode ) ;
	 if(p_msg_in ==  NULL)
	{
		 pteipc_get_errormsg( retcode, temp_str );
		 return false;
	}
	else
	{
        p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
        p_data     = ptemsg_get_pte_msg_data_data( p_msg_data );
		data_len   = ptemsg_get_pte_msg_data_data_length(p_msg_data);
		if ( sizeof(MONITOR_STATS) < data_len )
		{
			len = sizeof(MONITOR_STATS);
		}
		else
		{
			len = data_len;
		}
        memcpy( &monitor_stats, p_data, len );
        memcpy(asci_mra_sent_count,monitor_stats.cnt_to_network,strlen(monitor_stats.cnt_to_network));
        temp_count = atoi(asci_mra_sent_count);
        if(temp_count == 0)
        {
        	txn_cnt = 1;
        }
        else
        {
        	txn_cnt = temp_count ;
        }
    }
	 /*Copy the MRA sent count to global variable */

	 return true;
}

/*pPTE_MSG GetNCF01List(NCF01 m_ncf01)
{
	pPTE_MSG    p_msg_out;
	pPTE_MSG    p_msg_in;
	CHAR        error_msg[256], buffer[300];
	char        list_buffer[sizeof(AUTH_TX) + sizeof (NCF01)];
	char strBuffer[256] = {0};
	LONG retcode =0;
	memcpy ((list_buffer + sizeof(AUTH_TX)), &m_ncf01, sizeof (NCF01));
	 CHAR            temp_str[100];

	p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
		ST1_DB_GET_GUI_LIST,
		0,
		netds_que_name,
		interactive_que_name,
		(pBYTE)list_buffer,
		sizeof( list_buffer ),
		NCF01_DATA );
	if( p_msg_out == NULL_PTR )
	{
		 sprintf( strBuffer,"Fetched the entire NCF01 Table" );
		 LogEvent( strBuffer, INFO_MSG );
		 return false;
	}

	 p_msg_in = pteipc_send_receive( p_msg_out, netds_que_name, interactive_que_name, &retcode ) ;
	 if(p_msg_in ==  NULL)
	{
		 pteipc_get_errormsg( retcode, temp_str );
		       sprintf( error_msg,
		               "Communication Error during request to NetDS: %s",
		                temp_str );
		       return PTEMSG_IPC_ERROR;
	}
	return p_msg_in;
	free( p_msg_out );
}*/



/*******************************************************
 * This function will get the values from tf.ini file
 * from the section DATASERVER with keyname DB_ERROR_STATICS_FLAG &
 *	DB_ERROR_STATICS_VALUE which will help to decide the db statics
 *	logging mechanism.
 * *******************************************************/

/******************************************************************************
 *
 *  NAME:         mracon_log_message
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
void mracon_log_message( INT dest, INT sev, pCHAR msg, pCHAR func, int details )
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
   mracon_Log_error_warning_to_File(text_message,sev,func,details);
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

void mracon_get_error_warning_file_name_path(void )
{
   DWORD rc;
   CHAR  filename   [80] = {0};
   CHAR  tmpstr     [80] = {0};
   CHAR  mracon_error_warning_file_size[5] = {0};

   /* Get path to the tf.ini file. */
   memset( tmpstr, 0x00, sizeof(tmpstr) );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, "tf.ini" );

   /*Read path for creating file in order to log db statics and db oracle error messages */
   rc = GetPrivateProfileString(
								  "DATASERVERS",             /* points to section name         */
								  "DB_STATICS_PATH",       	/* points to key name             */
								   "",  					/* Default string                 */
								   mracon_error_warning_file_path,              	/* points to destination buffer   */
								   sizeof(mracon_error_warning_file_path)-1,   	 /* size of destination buffer     */
								   filename                  /* points to ini filename         */
						 	   );

   rc = GetPrivateProfileString(
								  "ERROR_WARNING",             /* points to section name         */
								  App_Name,       	/* points to key name             */
								  "",  					/* Default string                 */
								  mracon_module_error_warning_file_name,              	/* points to destination buffer   */
								  sizeof(mracon_module_error_warning_file_name)-1,   	 /* size of destination buffer     */
								  filename                  /* points to ini filename         */
								);

   rc = GetPrivateProfileString(
							  "ERROR_WARNING",             /* points to section name         */
							  "ERROR_WARNING_FILE_SIZE",       	/* points to key name             */
							  "500",  					/* Default string                 */
							  mracon_error_warning_file_size,              	/* points to destination buffer   */
							  sizeof(mracon_error_warning_file_size)-1,   	 /* size of destination buffer     */
							  filename                  /* points to ini filename         */
							);

	/* File size size conversion */
	Max_File_Size_Defined = atoi(mracon_error_warning_file_size);
	if(Max_File_Size_Defined <= 0)
	{
		Max_File_Size_Defined = 500 ;
	}
	Max_File_Size_Defined = Max_File_Size_Defined * ONE_MB_IN_BYTES ; /* 1 MB = 1048576 BYTES*/

   if((strlen(mracon_error_warning_file_path) > 0) &&
	  (strlen(mracon_module_error_warning_file_name)> 0))
   {
	   mracon_create_Error_Filename();
   }
}

INT mracon_Log_error_warning_to_File(pCHAR Error_Buf,int sev, pCHAR func ,int detail)
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
    len = strlen(mracon_Erro_warning_Filename);
    path_len = strlen(mracon_error_warning_file_path);
    if( len == 0 ||
    	path_len==0 )
    {
    	LogEvent(Error_Buf,INFO_MSG);
    	return 0;
    }
    if ( 0 != strcmp(&mracon_Erro_warning_Filename[len-4], current_mmdd) )
    {
		/* Now create the new filename.*/
    	mracon_create_Error_Filename();
    }
	if((fp = fopen(mracon_Erro_warning_Filename,"a+b"))==NULL)
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

void mracon_create_Error_Filename()
{
	CHAR  system_date[25] = {0};

   /* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
	ptetime_get_timestamp( system_date );

    strcpy( mracon_Erro_warning_Filename, mracon_error_warning_file_path );
    strcat( mracon_Erro_warning_Filename, mracon_module_error_warning_file_name );
	strcat( mracon_Erro_warning_Filename, ".EW." );
    strncat(mracon_Erro_warning_Filename, system_date,   4 );  /* YYYY */
    strncat(mracon_Erro_warning_Filename, system_date+5, 2 );  /* MM   */
    strncat(mracon_Erro_warning_Filename, system_date+8, 2 );  /* DD   */
}

int Convert_Amt_To_Auth_Tx_String_Format
                              (double nAmount, char* strAmount)
{
	char strTemp[256] = {0} ;
	int  nStringLength = 0 ;
	int  nIndex = 0 ;

	gcvt (nAmount, 13, strTemp) ; /*choice of 15 is arbitrary*/

	nStringLength = strlen (strTemp) ;
	 /*Discard all characters after the decimal point, if present*/
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
