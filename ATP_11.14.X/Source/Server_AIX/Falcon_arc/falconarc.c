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
#include "nc_dbstruct.h"
#include "tx_dbstruct.h"
#include "falconarc.h"
#include "txutils.h"
#include "dc_database.h"
#include "memmnger.h"
#include "ptetimer.h"
#include "ptetime.h"
#include "dbcommon.h"
#include "timings.h"
#include "string.h"
extern volatile INT  EndProcessSignalled;
extern volatile INT  MainProcessDone;
extern int           Start();
extern void          Stop ();
AUTH_TX     Auth_Tx;
char Version[] ="ATP_11.1.0";
static char message[1023]= {0};
char	temp_msg[1000]="";
static char mra_mssg[1040]={0};
//static CHAR mssg_buffer[1030]={0};
TLF01	tlf01_tmp_struct;
pTLF01 tlf01_ptr;
CRF01 crf01;
/*CHAR  date_str [ 9] = "";
CHAR  time_str [ 7] = "";
CHAR  time_date[25] = "";*/
CHAR  time_milliseconds[4] = "";
CHAR ServiceName[80];
char Fromdate[9]="";
char Todate[9]="";
char Fromtime[7]="";
char Totime[7]="";
extern int more_data;
LONG tx_count=0;
int header=0;
   char *file_path;
   char file_buffer[1000]={0};
   static CHAR  c_record [245];
   static FILE  *c_file_write = 0;

char *mra_path;
   char mra_buffer[1000]={0};
   static CHAR  mra_record [245];
   static FILE  *mra_file_write = 0;
int falcon_tx_counter = 0;
CHAR profile_str [17]="";    


/*CHAR mssg_type[8] = {"0500","0320","0800","0400","0220","0830","0530","0200"};*/
/*CHAR pr_cd[10]={"920000","960000","910000","990000","040000","900000","950000","930000","200000","000000");*/

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

INT Check_If_CUP_Transaction_falconarc (void)
{
	if ((0 == strncmp(Auth_Tx.TLF01_details.product_code,CUP_CARD_INDICATOR_FLAG,2)))
	{
		return true ;
	}
	else
	{
		return false;
	}
}
INT Check_If_VISA_Transaction (void)
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
INT Check_If_MC_Transaction (void)
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
INT Check_If_JCB_Transaction (void)
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
INT Check_If_AMEX_Transaction (void)
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

INT check_If_Diners_Transaction (void)
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
void RqMssg()
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
char temp_totalamt[14]={0};double temp_conv_amount;
char temp_without_decimal[13]="";  
double   conv_rate   = 0.0; 
CHAR  date_str [ 9] = "";
CHAR  time_str [ 7] = "";
CHAR  time_date[25] = "";
CHAR temp_amex_pos_entry[13]={0};
memset(&Cr_Fld,0,sizeof(Cr_Fld));
memset(&message,0,sizeof(message));

int i =0;
int len=0;
char msg[5]={0};

memset(&Cr_Fld, ' ', sizeof(Cr_Fld) );

ptetime_get_timestamp( time_date );
get_date( time_date, date_str );
get_time( time_date, time_str );

memcpy(Cr_Fld.HeaderFile,"000009710000100000102tdriver   PMAX      0000000000                  ",69);
sprintf(message,"%-69s\n",Cr_Fld.HeaderFile);

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
memcpy(Cr_Fld.GMT_OFFSET,"+8.00",6);
sprintf(&message[131],"%-6s\n",Cr_Fld.GMT_OFFSET);

memcpy(Cr_Fld.CUSTOMER_XID,Auth_Tx.TLF01_details.card_num,20);
sprintf(&message[137],"%-20s\n",Cr_Fld.CUSTOMER_XID);

memcpy(Cr_Fld.ACCOUNT_REFERENCE_XID,Auth_Tx.TLF01_details.card_num,19);
sprintf(&message[157],"%-40s\n",Cr_Fld.ACCOUNT_REFERENCE_XID);
char temp_Tran_id[33]="";
	if(falcon_tx_counter == 999999)
	{
		falcon_tx_counter = 1;
	}
sprintf(temp_Tran_id,"A",1);
sprintf(&temp_Tran_id[1],"%020s",Auth_Tx.TLF01_details.primary_key.transaction_id);
sprintf(&temp_Tran_id[21],"-CRT-",5);
sprintf(&temp_Tran_id[26],"%06d",falcon_tx_counter++);
sprintf(Cr_Fld.EXTERNAL_TRANSACTION_XID,"%32s",temp_Tran_id);
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
strncat(temp_exp,Auth_Tx.TLF01_details.exp_date,4);
else
{strncpy(temp_exp,Auth_Tx.TLF01_details.date_yyyymmdd,6);
 strncpy(tem_month,&Auth_Tx.TLF01_details.date_yyyymmdd[4],2);
}
	
if(strncmp(tem_month,"02",2) == 0 || strncmp(tem_month,"",2) == 0 || strncmp(tem_month,"  ",2) == 0 || strncmp(tem_month,"00",2) == 0)
	strncat(temp_exp,"28",2);
else
	strncat(temp_exp,"30",2);	
strncpy(Cr_Fld.CARD_EXPIRATION_DT,temp_exp,8);
sprintf(&message[286],"%-8s\n",Cr_Fld.CARD_EXPIRATION_DT);

if(strcmp(Auth_Tx.ACF01_details.available,"") != 0)
{ char temp_avail_amt[11]="";
strncpy(temp_avail_amt,&Auth_Tx.ACF01_details.available[2],10);
sprintf(Cr_Fld.PRIOR_CREDIT_LIMIT_AMT,"%07d.%02d",atoi(temp_avail_amt)/100, atoi(temp_avail_amt)%100);

sprintf(&message[294],"%-10s\n",Cr_Fld.PRIOR_CREDIT_LIMIT_AMT);
}
else if(strcmp(Auth_Tx.ACF01_details.available,"") == 0)
{
	memset(Cr_Fld.PRIOR_CREDIT_LIMIT_AMT,' ',10);
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
char temp_iso_ration[8] = {0};
char temp_iso_ration_value[13] = {0};
double temp_iso = 0.0;
if(strcmp(Auth_Tx.TLF01_details.conversion_rate,"")!= 0)
		conv_rate = strtod( (char *)Auth_Tx.TLF01_details.conversion_rate, (char **)NULL );
else
	conv_rate = 42.00000;
sprintf(temp_iso_ration,"%f",1/conv_rate);
if(strcmp(Auth_Tx.TLF01_details.currency_code,"608") == 0) // PESO local currecny
{
	sprintf(Cr_Fld.TRANSACTION_AMT,"%010d.%02d",atoi(Auth_Tx.TLF01_details.total_amount)/100, atoi(Auth_Tx.TLF01_details.total_amount)%100);

}else if(strcmp(Auth_Tx.TLF01_details.currency_code,"840") == 0)// Dollor tx
{
	 temp_conv_amount = ((strtod( (char *)Auth_Tx.TLF01_details.total_amount, (char **)NULL )/100) * conv_rate);
	//sprintf(temp_without_decimal,"%10ld%2ld",(long)temp_conv_amount,((long)(temp_conv_amount*100)%100));
	strncpy(temp_without_decimal,doubletostr(temp_conv_amount),12);
	sprintf(Cr_Fld.TRANSACTION_AMT,"%010d.%02d",atoi(temp_without_decimal)/100,atoi(temp_without_decimal)%100);

}
if(atoi(Cr_Fld.TRANSACTION_AMT) == 0 && strcmp(Auth_Tx.TLF01_details.currency_code,"840") == 0)
{		temp_conv_amount = ((strtod( (char *)Auth_Tx.TLF01_details.total_amount, (char **)NULL )/100) * conv_rate);
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
if(strncmp(Auth_Tx.TLF01_details.response_code,"00",2)== 0 || 
	strncmp(Auth_Tx.TLF01_details.response_code,"10",2)== 0 ||
	strncmp(Auth_Tx.TLF01_details.response_code,"11",2)== 0 ||
	strncmp(Auth_Tx.TLF01_details.response_code,"32",2)== 0 ||
	strncmp(Auth_Tx.TLF01_details.response_code,"85",2)== 0 )
{	strncpy(Cr_Fld.DECISION_XCD,"A",1);
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
	strncmp(Auth_Tx.TLF01_details.response_code,"44",2)== 0
)
{	strncpy(Cr_Fld.DECISION_XCD,"D",1);
}else if(strncmp(Auth_Tx.TLF01_details.response_code,"04",2)== 0 || 
	strncmp(Auth_Tx.TLF01_details.response_code,"07",2)== 0 ||
	strncmp(Auth_Tx.TLF01_details.response_code,"41",2)== 0 ||
	strncmp(Auth_Tx.TLF01_details.response_code,"43",2)== 0 
	)
{	strncpy(Cr_Fld.DECISION_XCD,"P",1);
}else if(strncmp(Auth_Tx.TLF01_details.response_code,"01",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"02",2)== 0 ||
	strncmp(Auth_Tx.TLF01_details.response_code,"08",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"34",2)== 0 ||
	strncmp(Auth_Tx.TLF01_details.response_code,"35",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"36",2)== 0 ||
	strncmp(Auth_Tx.TLF01_details.response_code,"37",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"38",2)== 0 ||
	strncmp(Auth_Tx.TLF01_details.response_code,"60",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"62",2)== 0 ||
	strncmp(Auth_Tx.TLF01_details.response_code,"65",2)== 0 || strncmp(Auth_Tx.TLF01_details.response_code,"66",2)== 0 ||
	strncmp(Auth_Tx.TLF01_details.response_code,"75",2)== 0
	)
{	strncpy(Cr_Fld.DECISION_XCD,"R",1);
}else
{
		strncpy(Cr_Fld.DECISION_XCD," ",1);
}

sprintf(&message[381],"%-1s\n",Cr_Fld.DECISION_XCD);
if(Auth_Tx.TLF01_details.tx_key == AUTH_SALE || Auth_Tx.TLF01_details.tx_key == AUTH_SALE_RESPONSE ||
	Auth_Tx.TLF01_details.tx_key == AUTH_AUTHORIZATION || Auth_Tx.TLF01_details.tx_key == AUTH_AUTHORIZATION_RESPONSE||
	Auth_Tx.TLF01_details.tx_key == AUTH_ADVICE|| Auth_Tx.TLF01_details.tx_key == AUTH_ADVICE_RESPONSE||
	Auth_Tx.TLF01_details.tx_key == AUTH_ADVICE_REPEAT|| Auth_Tx.TLF01_details.tx_key == AUTH_ADVICE_REPEAT_RESPONSE
	)	
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

/*strncpy(Cr_Fld.MERCHANT_POSTAL_XCD,Auth_Tx.MCF01_details.zip_code,9);
sprintf(&message[387],"%-9s\n",Cr_Fld.MERCHANT_POSTAL_XCD);*/
strncpy(Cr_Fld.MERCHANT_POSTAL_XCD," ",9);// data is not avaliable in the TLF01 table need to cross verify with the client to fill proper value,as of now leaving it blank
sprintf(&message[387],"%-9s\n",Cr_Fld.MERCHANT_POSTAL_XCD);

if(strlen(Auth_Tx.MCF01_details.primary_key.merchant_id)> 0)
	strncpy(Cr_Fld.MERCHANT_COUNTRY_XCD,"608",3);
else if(strlen(Auth_Tx.TLF01_details.num_dbsales)> 0)
	strncpy(Cr_Fld.MERCHANT_COUNTRY_XCD,Auth_Tx.TLF01_details.num_dbsales,3);
else 
	strncpy(Cr_Fld.MERCHANT_COUNTRY_XCD,Auth_Tx.TLF01_details.currency_code,3);

sprintf(&message[396],"%-3s\n",Cr_Fld.MERCHANT_COUNTRY_XCD);
if(Auth_Tx.TLF01_details.tx_key == AUTH_CASH_ADVANCE || Auth_Tx.TLF01_details.tx_key == AUTH_CASH_ADVANCE_RESPONSE) 
{	if(true == Check_If_MC_Transaction())
	{
		if( strcmp(Auth_Tx.TLF01_details.response_code, "00") == 0)
		{
			memcpy(Cr_Fld.TRANSACTION_PIN_VERIFY_XCD,"V",1);
		}
		else
		{
			memcpy(Cr_Fld.TRANSACTION_PIN_VERIFY_XCD,"I",1);
		}
	}
	else
		memcpy(Cr_Fld.TRANSACTION_PIN_VERIFY_XCD," ",1);



}
else
memcpy(Cr_Fld.TRANSACTION_PIN_VERIFY_XCD," ",1);
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

		char temp_pos_entry_mode[3]="";
	strncpy(temp_pos_entry_mode,&Auth_Tx.TLF01_details.pos_entry_mode[1],2);
	strncpy(temp_amex_pos_entry,Auth_Tx.TLF01_details.product_codes[16].amount,12);
	if(true == Check_If_AMEX_Transaction())
	{	if( (temp_amex_pos_entry[5]== 'X' && temp_amex_pos_entry[6] == 'W') ||
			(temp_amex_pos_entry[5]== 'X' && temp_amex_pos_entry[6] == '2') 
		)
		{
			strncpy(temp_pos_entry_mode,"2W",2); // Contact less mag stripe
		}
		else if( (temp_amex_pos_entry[5]== 'X' && temp_amex_pos_entry[6] == '5')
			 
		)
		{
			strncpy(temp_pos_entry_mode,"X5",2); // Contact less micro chip
		}
		else if( (temp_amex_pos_entry[5]== '1' && temp_amex_pos_entry[6] == '5')
			 
		)
		{
			strncpy(temp_pos_entry_mode,"05",2); //  chip
		}
		else if( (temp_amex_pos_entry[0]== '5' && temp_amex_pos_entry[5] == '1' && temp_amex_pos_entry[6] == '2') ||
			 	 (temp_amex_pos_entry[0]== '5' && temp_amex_pos_entry[5] == '1' && temp_amex_pos_entry[6] == 'W') ||
			 	 (temp_amex_pos_entry[6] == '9')
		)
		{
			strncpy(temp_pos_entry_mode,"80",2); // Fall back Mag stripe
		}
		else if( (temp_amex_pos_entry[0]== '5' && temp_amex_pos_entry[5] == '1' && temp_amex_pos_entry[6] == 'S') ||
			 	 (temp_amex_pos_entry[0]== '5' && temp_amex_pos_entry[5] == '1' && temp_amex_pos_entry[6] == '6') ||
			 	 (temp_amex_pos_entry[0]== '5' && temp_amex_pos_entry[5] == '1' && temp_amex_pos_entry[6] == '1') ||
			 	 (temp_amex_pos_entry[0]== '5' && temp_amex_pos_entry[5] == '1' && temp_amex_pos_entry[6] == 'V') 
		)
		{
			strncpy(temp_pos_entry_mode,"79",2); // Fall back keyed entry
		}
		else if( (temp_amex_pos_entry[0]== '6' && temp_amex_pos_entry[6] == '1') ||
				 (temp_amex_pos_entry[0]== '1' && temp_amex_pos_entry[6] == '1') ||
			     (temp_amex_pos_entry[0]== '6' && temp_amex_pos_entry[6] == 'V') ||
			     (temp_amex_pos_entry[0]== '1' && temp_amex_pos_entry[6] == 'V')
		)
		{
			strncpy(temp_pos_entry_mode,"01",2); // keyed in
		}
	    else if(strncmp(temp_amex_pos_entry,"6609S06S4140",12)== 0 ||
		   strncmp(temp_amex_pos_entry,"6609S0SS4140",12)== 0 ||
	       strncmp(temp_amex_pos_entry,"660101654140",12)== 0 ||
	       strncmp(temp_amex_pos_entry,"2009106S0040",12)== 0
		   )
		   strncpy(temp_pos_entry_mode,"81",2); // E-commerce5
		else if(strncmp(temp_amex_pos_entry,"660101S54140",12)== 0	||
			   strncmp(temp_amex_pos_entry,"660390664130",12)== 0	||
			   strncmp(temp_amex_pos_entry,"660390S64130",12)== 0	||
			   strncmp(temp_amex_pos_entry,"660320664130",12)== 0	||
			   strncmp(temp_amex_pos_entry,"660320S64130",12)== 0	||
			   strncmp(temp_amex_pos_entry,"660101S54130",12)== 0	||
			   strncmp(temp_amex_pos_entry,"660101V54130",12)== 0	
		   )
		   strncpy(temp_pos_entry_mode,"01",2); 
		else if(strncmp(temp_amex_pos_entry,"260101254140",12)== 0 ||
		   strncmp(temp_amex_pos_entry,"260101W54140",12)== 0		||
		   strncmp(temp_amex_pos_entry,"201101W00001",12)== 0
		   	   )
		   strncpy(temp_pos_entry_mode,"90",2); 
		else if(strcmp(Auth_Tx.TLF01_details.pos_entry_mode,"0072") == 0 ||
			strcmp(Auth_Tx.TLF01_details.pos_entry_mode,"0070") == 0 )
			{
			   strncpy(temp_pos_entry_mode,"01",2); 
			}
		else if(strcmp(Auth_Tx.TLF01_details.pos_entry_mode,"0022") == 0 ||
				strcmp(Auth_Tx.TLF01_details.pos_entry_mode,"0062") == 0 
			)
			{
			   strncpy(temp_pos_entry_mode,"90",2); 
			}
		else if( (strcmp(temp_pos_entry_mode,"01") == 0 || strcmp(temp_pos_entry_mode,"07") == 0)  && 
						strcmp(Auth_Tx.TLF01_details.pos_condition_code,"59") == 0
					)
			{
				strncpy(temp_pos_entry_mode,"81",2);
			}
		
	}	else if((true == Check_If_JCB_Transaction()) &&
			strcmp(Auth_Tx.TLF01_details.handler_queue,"dcpisoA") != 0 &&
			strncmp(Auth_Tx.TLF01_details.handler_queue,"VOICE",5) != 0 
		)
	{
		strncpy(temp_pos_entry_mode,&Auth_Tx.TLF01_details.pos_entry_mode[1],2);
	}else if(((true == Check_If_VISA_Transaction())  ||(true == Check_If_MC_Transaction())) &&
			 strcmp(temp_pos_entry_mode,"01") == 0 && strcmp(Auth_Tx.TLF01_details.pos_condition_code,"59") == 0)
	{
		strncpy(temp_pos_entry_mode,"81",2);
	}else if(strcmp(Auth_Tx.TLF01_details.pos_entry_mode,"0022") == 0 && (true == Check_If_JCB_Transaction()) )
	{	
		strncpy(temp_pos_entry_mode,"90",2); 
	}else if((strcmp(Auth_Tx.TLF01_details.pos_entry_mode,"0021") == 0)&&(true == Check_If_JCB_Transaction()) )
	{	
		strncpy(temp_pos_entry_mode,"91",2); 
	}
	else if(strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"080",3) == 0 )
	{	
		strncpy(temp_pos_entry_mode,"80",2);
	}
	if(strncmp(temp_pos_entry_mode,"91",2) == 0 || strncmp(temp_pos_entry_mode,"2W",2) == 0)
		strncpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"C",1);
	else if(strncmp(temp_pos_entry_mode,"92",2) == 0   ||
			strncmp(temp_pos_entry_mode,"07",2) == 0   ||
			strncmp(temp_pos_entry_mode,"06",2) == 0   ||
			strncmp(temp_pos_entry_mode,"X5",2) == 0	)
		strncpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"D",1);
	else if(strncmp(temp_pos_entry_mode,"81",2) == 0)
		strncpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"E",1);
	else if(strncmp(temp_pos_entry_mode,"80",2) == 0)
		strncpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"F",1);
	else if(strncmp(temp_pos_entry_mode,"79",2) == 0)
		strncpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"G",1);
	else if(strncmp(temp_pos_entry_mode,"01",2) == 0 )
		strncpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"K",1);
	else if(strncmp(temp_pos_entry_mode,"02",2) == 0)
		strncpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"S",1);
	else if(strncmp(temp_pos_entry_mode,"90",2) == 0 ||
			strncmp(temp_pos_entry_mode,"97",2) == 0 )
		strncpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"U",1);
	else if(strncmp(temp_pos_entry_mode,"05",2) == 0)
		strncpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"V",1);
	else
		strncpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG," ",1);
		
	
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



memset(Cr_Fld.USER_INDICATOR_1_XCD,' ',1);
//strncpy(Auth_Tx.BON01_details.minimum_amount,sPAF01.APP_Source,sizeof(sPAF01.APP_Source));
//strncpy(Auth_Tx.BON01_details.response_message,sPAF01.COMP_Name,sizeof(sPAF01.COMP_Name));
sprintf(&message[413],"%-1s\n",Cr_Fld.USER_INDICATOR_1_XCD);
memset(Cr_Fld.USER_INDICATOR_2_XCD,' ',1);
sprintf(&message[414],"%-1s\n",Cr_Fld.USER_INDICATOR_2_XCD);
memset(Cr_Fld.USER_DATA_1_STRG,' ',10); // UCAF indicator DE48 SE43 
/*Field 48 for Terminal type*/
if(true == Check_If_VISA_Transaction())//VISA
{
	memcpy(Cr_Fld.USER_DATA_1_STRG,&Auth_Tx.additional_pos_information[0],1);
	
}else if(true == Check_If_MC_Transaction())//MC
{
	memcpy(Cr_Fld.USER_DATA_1_STRG,&Auth_Tx.acceptor_term_name[9],1);
	
}else if(true == Check_If_AMEX_Transaction())//AMEX
{
	strncpy(Cr_Fld.USER_DATA_1_STRG,&temp_amex_pos_entry[3],1);
}

sprintf(&message[415],"%-10s\n",Cr_Fld.USER_DATA_1_STRG);

memset(Cr_Fld.USER_DATA_2_STRG,' ',10);
/*Field 49 for Terminal Entry capa*/
if(true == Check_If_VISA_Transaction())//VISA
{
	memcpy(Cr_Fld.USER_DATA_2_STRG,&Auth_Tx.additional_pos_information[1],1);
	
}else if(true == Check_If_MC_Transaction())//MC
{
	memcpy(Cr_Fld.USER_DATA_2_STRG,&Auth_Tx.acceptor_term_name[10],1);
	
}else if(true == Check_If_AMEX_Transaction())//AMEX
{
	memcpy(Cr_Fld.USER_DATA_2_STRG,&Auth_Tx.additional_pos_information[0],1);
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
   strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"V",1) == 0 
)
{	memcpy(Cr_Fld.CARDHOLDER_PRESENT_XFLG,"Y",1);
	memcpy(Cr_Fld.TRANSACTION_CATEGORY_XCD,"P",1);
}
else if(strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"E",1) == 0 ||
   strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"K",1) == 0 )
{	
	memcpy(Cr_Fld.CARDHOLDER_PRESENT_XFLG,"N",1);   
	if(strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"E",1) == 0)
	memcpy(Cr_Fld.TRANSACTION_CATEGORY_XCD,"I",1);
	else
	memcpy(Cr_Fld.TRANSACTION_CATEGORY_XCD,"M",1);		
}
else
{
	memcpy(Cr_Fld.CARDHOLDER_PRESENT_XFLG," ",1);
	memcpy(Cr_Fld.TRANSACTION_CATEGORY_XCD,"O",1);
}

sprintf(&message[459],"%-1s\n",Cr_Fld.CARDHOLDER_PRESENT_XFLG);

if(strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"E",1) == 0)
memcpy(Cr_Fld.CAT_TYPE_XFLG,"E",1);// POS Condition code
else
memcpy(Cr_Fld.CAT_TYPE_XFLG,"S",1);// POS Condition code
sprintf(&message[460],"%-1s\n",Cr_Fld.CAT_TYPE_XFLG);

memset(Cr_Fld.TESTING_RANDOM_DIGITS_STRG,' ',2);
sprintf(&message[461],"%-2s\n",Cr_Fld.TESTING_RANDOM_DIGITS_STRG);
memset(Cr_Fld.PORTFOLIO_NAME,' ',14);
sprintf(&message[463],"%-14s\n",Cr_Fld.PORTFOLIO_NAME);
memset(Cr_Fld.CLIENT_2_XID,' ',14);
sprintf(&message[477],"%-14s\n",Cr_Fld.CLIENT_2_XID);
memset(Cr_Fld.INTERBANK_CARD_ASSOCIATION_NUM,' ',6);
sprintf(&message[491],"%-6s\n",Cr_Fld.INTERBANK_CARD_ASSOCIATION_NUM);
if(strlen(Auth_Tx.TLF01_details.card_holder_name) > 0)
	memcpy(Cr_Fld.MERCHANT_NAME,Auth_Tx.TLF01_details.card_holder_name,30);
sprintf(&message[497],"%-40s\n",Cr_Fld.MERCHANT_NAME);
if(strlen(Auth_Tx.TLF01_details.ncf01_retired_cwk) > 0)
	{
		memcpy(Cr_Fld.MERCHANT_CITY_NAME,Auth_Tx.TLF01_details.ncf01_retired_cwk,20);
	}
sprintf(&message[537],"%-30s\n",Cr_Fld.MERCHANT_CITY_NAME);

memcpy(Cr_Fld.MERCHANT_STATE_XCD,"608",3);
sprintf(&message[567],"%-3s\n",Cr_Fld.MERCHANT_STATE_XCD);
memset(Cr_Fld.SUPPRES_CASE_XFLG,' ',1);
sprintf(&message[570],"%-1s\n",Cr_Fld.SUPPRES_CASE_XFLG);
/***memcpy(Cr_Fld.USER_INDICATOR_3_XCD,' ' ,5);****/
memcpy(Cr_Fld.USER_INDICATOR_3_XCD,"",5);
sprintf(&message[571],"%-5s\n",Cr_Fld.USER_INDICATOR_3_XCD);

/**len**strcpy(Cr_Fld.USER_INDICATOR_4_XCD,"00");**/
strncpy(Cr_Fld.USER_INDICATOR_4_XCD,"",5);
//strncat(Cr_Fld.USER_INDICATOR_4_XCD,Auth_Tx.TLF01_details.currency_code,3);
sprintf(&message[576],"%-5s\n",Cr_Fld.USER_INDICATOR_4_XCD);
/* Dual currrecny logic*/

/**len**strcpy(Cr_Fld.USER_DATA_3_STRG,"000");**/
char temp_user_data_3_strg[15] ={0};
strncpy(temp_user_data_3_strg,"00",2);
sprintf(temp_totalamt,"%010d.%02d",atoi(Auth_Tx.TLF01_details.total_amount)/100, atoi(Auth_Tx.TLF01_details.total_amount)%100);
strncat(temp_user_data_3_strg,temp_totalamt,13);
strncpy(Cr_Fld.USER_DATA_3_STRG,temp_user_data_3_strg,15);
sprintf(&message[581],"%-15s\n",Cr_Fld.USER_DATA_3_STRG);

//memcpy(Cr_Fld.USER_DATA_4_STRG,Auth_Tx.ACF01_details.primary_key.account_nbr,20);
strncpy(Cr_Fld.USER_DATA_4_STRG,Auth_Tx.TLF01_details.currency_code,3);
sprintf(&message[596],"%-20s\n",Cr_Fld.USER_DATA_4_STRG);
memset(Cr_Fld.USER_DATA_5_STRG,' ',40);
/*Field 70 for Response message*/
//Auth_Tx.TLF01_details.response_text
if(strlen(Auth_Tx.TLF01_details.response_text)> 0)
{
	strncpy(Cr_Fld.USER_DATA_5_STRG,Auth_Tx.TLF01_details.response_text,30);
}
else if(strcmp(Auth_Tx.BON01_details.response_message,"")!= 0)
{
	strncpy(Cr_Fld.USER_DATA_5_STRG,Auth_Tx.BON01_details.response_message,30);
}
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
icvc_data=atoi(Auth_Tx.TLF01_details.cvc_data);
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
		}else if(strcmp(Cr_Fld.CVV2_RESPONSE_XFLG,"M") == 0)
		{	
			strncpy(Cr_Fld.CVV2_PRESENT_XFLG,"1",1);
		}
		else
		{	memset(Cr_Fld.CVV2_PRESENT_XFLG,' ',1);
		}
        sprintf(&message[745],"%-1s\n",Cr_Fld.CVV2_PRESENT_XFLG);
	    break;
}

/****memset(Cr_Fld.CVV2_RESPONSE_XFLG,' ',1);****/
if((true == Check_If_JCB_Transaction()) || (true == Check_If_AMEX_Transaction()))
memset(Cr_Fld.CVV2_RESPONSE_XFLG,' ',1);
else if(isalphanum(Cr_Fld.CVV2_PRESENT_XFLG) == true)
{
	if(strcmp(Cr_Fld.CVV2_PRESENT_XFLG,"0") == 0)
	strcpy(Cr_Fld.CVV2_RESPONSE_XFLG," ");
	else if(strlen(Cr_Fld.CVV2_RESPONSE_XFLG) == 0)
	strcpy(Cr_Fld.CVV2_RESPONSE_XFLG," ");	
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
if(strncmp(Auth_Tx.TLF01_details.handler_queue,"dcpisoA",5) == 0)/*If it's a local transactions*/
{
	strncpy(Cr_Fld.ACQUIRER_COUNTRY_XCD,"608",3);
}
	else
{
	memcpy(Cr_Fld.ACQUIRER_COUNTRY_XCD,Auth_Tx.TLF01_details.nfi_seq_nbr,3);
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

memset(Cr_Fld.TERMINAL_TYPE_XCD,' ',1);
if(true == Check_If_VISA_Transaction())//VISA
{
	memcpy(Cr_Fld.TERMINAL_TYPE_XCD,&Auth_Tx.additional_pos_information[0],1);
	if(strncmp(Cr_Fld.TERMINAL_TYPE_XCD,"1",1) == 0 ||
	   strncmp(Cr_Fld.TERMINAL_TYPE_XCD,"4",1) == 0)
		strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"L",1);
	else if(strncmp(Cr_Fld.TERMINAL_TYPE_XCD,"2",1) == 0 ||
	   strncmp(Cr_Fld.TERMINAL_TYPE_XCD,"3",1) == 0)
		strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"U",1);
	else if(strncmp(Cr_Fld.TERMINAL_TYPE_XCD,"5",1) == 0 ||
	   strncmp(Cr_Fld.TERMINAL_TYPE_XCD,"7",1) == 0)
		strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"I",1);
	else if(strncmp(Cr_Fld.TERMINAL_TYPE_XCD,"0",1) == 0)
		memset(Cr_Fld.TERMINAL_TYPE_XCD,' ',1);
	else 
		strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"O",1);

}
else if(true == Check_If_MC_Transaction())// MC
{
   memcpy(Cr_Fld.TERMINAL_TYPE_XCD,&Auth_Tx.acceptor_term_name[9],1);
   if(strncmp(Cr_Fld.TERMINAL_TYPE_XCD,"3",1) == 0 )
		strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"L",1);
	else if(strncmp(Cr_Fld.TERMINAL_TYPE_XCD,"1",1) == 0 ||
	   strncmp(Cr_Fld.TERMINAL_TYPE_XCD,"2",1) == 0)
		strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"U",1);
	else if(strncmp(Cr_Fld.TERMINAL_TYPE_XCD,"6",1) == 0 ||
	   strncmp(Cr_Fld.TERMINAL_TYPE_XCD,"7",1) == 0 ||
	   strncmp(Cr_Fld.TERMINAL_TYPE_XCD,"4",1) == 0)
		strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"I",1);
	else if(strncmp(Cr_Fld.TERMINAL_TYPE_XCD,"0",1) == 0)
memset(Cr_Fld.TERMINAL_TYPE_XCD,' ',1);
	else 
		strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"O",1);
}
else if(true == Check_If_AMEX_Transaction())// AMEX
{
	//temp_amex_pos_entry
	strncpy(Cr_Fld.TERMINAL_TYPE_XCD,&temp_amex_pos_entry[3],1);
	if(strncmp(Cr_Fld.TERMINAL_TYPE_XCD,"1",1) == 0 ||
	   strncmp(Cr_Fld.TERMINAL_TYPE_XCD,"3",1) == 0 ||
	   strncmp(Cr_Fld.TERMINAL_TYPE_XCD,"X",1) == 0)
		strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"L",1);
	else if(strncmp(Cr_Fld.TERMINAL_TYPE_XCD,"2",1) == 0 ||
	   strncmp(Cr_Fld.TERMINAL_TYPE_XCD,"4",1) == 0 ||
	   strncmp(Cr_Fld.TERMINAL_TYPE_XCD,"5",1) == 0 ||
	   strncmp(Cr_Fld.TERMINAL_TYPE_XCD,"Z",1) == 0)
		strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"U",1);
	else if(strncmp(Cr_Fld.TERMINAL_TYPE_XCD,"S",1) == 0 ||
	   strncmp(Cr_Fld.TERMINAL_TYPE_XCD,"T",1) == 0)
		strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"I",1);
	else if(strncmp(Cr_Fld.TERMINAL_TYPE_XCD,"0",1) == 0 ||
		strncmp(Cr_Fld.TERMINAL_TYPE_XCD,"9",1) == 0)
		memset(Cr_Fld.TERMINAL_TYPE_XCD,' ',1);
	else 
		strncpy(Cr_Fld.TERMINAL_TYPE_XCD,"O",1);
}

sprintf(&message[780],"%-1s\n",Cr_Fld.TERMINAL_TYPE_XCD);
/*********memcpy(Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD,"O",1);*****************/
/*Field 96 Terminal Entry capability */
memset(Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD,' ',1);
if(true == Check_If_VISA_Transaction())//VISA
{
	memcpy(Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD,&Auth_Tx.additional_pos_information[1],1);
	if(strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"5",1) == 0)
		strncpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"C",1);
	else if(strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"8",1) == 0)
		strncpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"D",1);
	else if(strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"9",1) == 0)
		strncpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"K",1);
	else if(strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"2",1) == 0)
		strncpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"M",1);
	else if(strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"3",1) == 0 ||
		strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"4",1) == 0 )
		strncpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"O",1);
	else if(strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"1",1) == 0)
		strncpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"Z",1);
	else if(strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"0",1) == 0)
		memset(Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD,' ',1);
	else 
		strncpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"N",1);
}
else if(true == Check_If_MC_Transaction())// MC
{
   memcpy(Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD,&Auth_Tx.acceptor_term_name[10],1);
   if(strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"5",1) == 0||
		strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"8",1) == 0 ||
		strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"9",1) == 0 )
		strncpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"C",1);
	else if(strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"3",1) == 0)
		strncpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"D",1);
	else if(strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"6",1) == 0)
		strncpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"K",1);
	else if(strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"2",1) == 0||
		strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"7",1) == 0 )
		strncpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"M",1);
	else if(strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"4",1) == 0 )
		strncpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"N",1);
	else if(strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"1",1) == 0)
		strncpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"Z",1);
	else if(strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"0",1) == 0)
		memset(Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD,' ',1);
	else 
		strncpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"O",1);
}
else if(true == Check_If_AMEX_Transaction())// AMEX
{
	//temp_amex_pos_entry
	strncpy(Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD,temp_amex_pos_entry,1);
	if(strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"0",1) == 0)
		memset(Cr_Fld.TERMINAL_ENTRY_CAPABILITY_XCD,' ',1);
	else if(strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"5",1) == 0)
		strncpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"C",1);
	else if(strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"6",1) == 0 ||
		strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"1",1) == 0)
		strncpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"K",1);
	else if(strncmp(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"2",1) == 0)
		strncpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"M",1);
	else 
		strncpy(Cr_Fld.TRANSACTION_POSTING_ENTRY_XFLG,"O",1);
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
strncpy(Cr_Fld.USER_DATA_6_NUM,Auth_Tx.TLF01_details.profile,2);
sprintf(&message[887],"%-13s\n",Cr_Fld.USER_DATA_6_NUM);
/*************memset(Cr_Fld.USER_DATA_7_STRG,' ',40);********/
/* User data 7 filled with AMEX POS DATA - temp_amex_pos_entry*/
memset(Cr_Fld.USER_DATA_7_STRG,' ',40);
strncpy(Cr_Fld.USER_DATA_7_STRG,temp_amex_pos_entry,12);
if(true == Check_If_AMEX_Transaction())// AMEX
{	
	strncpy(Cr_Fld.USER_DATA_7_STRG,temp_amex_pos_entry,12);
}else if(true == Check_If_VISA_Transaction())//VISA
{	
	strncpy(Cr_Fld.USER_DATA_7_STRG,Auth_Tx.TLF01_details.nfi_name,10);
}
else if(true == Check_If_MC_Transaction())
{	
	strncpy(Cr_Fld.USER_DATA_7_STRG,Auth_Tx.TLF01_details.nfi_name,10);
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
if((true == Check_If_VISA_Transaction()) || (true == Check_If_MC_Transaction()) || (true == Check_If_JCB_Transaction()) )
{	
	if(true == Check_If_JCB_Transaction())
	{
		strncpy(Cr_Fld.USER_DATA_8_STRG,&Auth_Tx.TLF01_details.pos_entry_mode[1],2);
	}
	else if(strncmp(Auth_Tx.TLF01_details.pos_entry_mode,"0802",4)== 0)//Fall back
	{
		if(true == Check_If_VISA_Transaction())
		{
			strncpy(Cr_Fld.USER_DATA_8_STRG,"90",2);
		}
		else if(true == Check_If_MC_Transaction())
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
}
// DE22 - Field132
//0022 -> 90 , 0802 ->90,0021 ->90
//memcpy(Cr_Fld.USER_DATA_8_STRG,Auth_Tx.TLF01_details.pos_entry_mode,sizeof(Auth_Tx.TLF01_details.pos_entry_mode));
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

if(strcmp(Auth_Tx.CAF01_details.expiration_date_check,"Y") == 0)
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
}

/**************memset(Cr_Fld.AUTH_SECONDARY_VERIFY_XCD,"V",1);************/
memcpy(Cr_Fld.AUTH_SECONDARY_VERIFY_XCD,"V",1);
sprintf(&message[1021],"%-1s\n",Cr_Fld.AUTH_SECONDARY_VERIFY_XCD);
strncpy(Cr_Fld.AUTHORIZATION_RESPONSE_XCD," ",1);
sprintf(&message[1022],"%s",Cr_Fld.AUTHORIZATION_RESPONSE_XCD);
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
   printf("Falcon requires two command line parameters to work correctly:        \n");
   printf("(1) the base file name,                                                \n");
   printf("(2) the From date in YYYYMMDD format,                                \n");
   printf("(3) the To date in YYYYMMDD format                              \n");
   printf("(4) the From time in HHMMSS format                             \n");
   printf("(5) the To time in HHMMSS format                             \n");
   printf("                                                                       \n");
   printf("For example, to create the Crtrans message format file from October 1,2012 to October 11,2012 with time from 11:00:00 to 16:00:00  use the parameters: \n");
   printf("                                                                       \n");
   printf("          falcon_archive 20121001 20121011 110000 160000                                       \n");
   printf("                                                                       \n");
   printf("Output is placed in the log folder with the name Falcon_Archive.txt   \n");
   printf("                                                                       \n");
   printf("Falcon Version: %s\n\n", Version);
 
} /* display_settle_info */


/*******************************************************************************
*
*  FUNCTION:         write_to_falcon_archive_file
*
*  DESCRIPTION:      This function writes to falcon archive file. 
*
*  INPUTS:           None
*         
*  OUTPUTS:          None
*
*  RETURN VALUE:     BOOLEAN true for success or false for failure.
*   
*******************************************************************************/
BOOLEAN  *write_to_falcon_archive_file(char *mssg)
{
   CHAR              tempstr[16];
   CHAR              buffer[100];
   //char  mssg_copy[1024] ={0};

   //memcpy(mssg_copy,mssg,1023);
	

   /* Send message to system monitor */
 /*  strcpy(buffer,"Writing to credit settlement file ");
   TxUtils_Send_Msg_To_Operator ( 0, buffer, 1, INFO_MSG,
                                 "write_to_credit_settle_file",
                                  0, INFO_ERROR, NULL_PTR, NULL_PTR, NULL_PTR); */

   /* Send message to DOS window in debug mode */
  /* strcat(buffer,"\n");
   PRINT(buffer); */
   //sleep(1000);
   /*** Open file ***/
	file_path=getenv( "ASCENDENTROOT" );
	strcpy(file_buffer,file_path);
	strcat(file_buffer,"/log/Falcon_Archive.txt");
   
    if(tx_count == 1)
	{
		c_file_write = fopen (file_buffer, "w");
	}

   if (c_file_write == NULL_PTR)
   {  
      strcpy(buffer, "Cannot open Falcon Archive Utility file: ");
      strcat(buffer, file_buffer); 
        
      TxUtils_Send_Msg_To_Operator ( 0,   /* 1 - write to sys. mon., 0 - not */
         buffer,                          /* the text of the message */ 
		   1,                               /* 1 - write to event log, 0 - not */
         ERROR_MSG,                       /* message type */ 
		   "write_to_falcon_archive_file",       /* name of the function */
         4,                               /* severity */
         FATAL_ERROR,                     /* message type */              
         NULL_PTR,                        /* TID */
		   NULL_PTR,                        /* card number */
         NULL_PTR);                       /* merchant id */

	  return(false);
   }

   /*** Write to file ***/
   
   
   //if (fputs(mssg,c_file_write) < 0)
   
	if((fprintf (c_file_write,"%s\n",mssg)) <0)
   {
      itoa (ferror(c_file_write), tempstr, 10);
      fclose (c_file_write);
      
      strcpy(buffer, "Cannot write to Falcon Archive file: ");
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

	 // write_to_dump_file(buffer);

      return(false);
   }   
    return (true);

} /* write_to_falcon_archive_file */




void mra_rqmssg()
{
		
		char temp_mratotalamt[12]={0};
		
		memset(&Mra_fld,0,sizeof(Mra_fld));
	memcpy(Mra_fld.AUTH_SEQ_ID,Auth_Tx.TLF01_details.primary_key.transaction_id,20);

	memcpy(Mra_fld.ACCOUNT_NUMBER , Auth_Tx.TLF01_details.merchant_id, sizeof ( Auth_Tx.TLF01_details.merchant_id));	
//memcpy(Mra_fld.ACCOUNT_NUMBER,Auth_Tx.TLF01_details.merchant_id,15);
	memcpy(Mra_fld.AUTH_ACCT_NUM, Auth_Tx.TLF01_details.card_num, sizeof(Auth_Tx.TLF01_details.card_num));	
	
	Mra_fld.AUTH_AUTH_POST_FLAG = 'A';

	//strncpy(Mra_fld.AUTH_EXP_DATE, (pCHAR) Auth_Tx.TLF01_details.date_yyyymmdd,2);  
	//strncat(Mra_fld.AUTH_EXP_DATE, (pCHAR) Auth_Tx.TLF01_details.exp_date,4);
	//strcat(Mra_fld.AUTH_EXP_DATE, "30");
	strcpy(Mra_fld.AUTH_EXP_DATE, (pCHAR) Auth_Tx.TLF01_details.date_yyyymmdd);
	strcpy(Mra_fld.AUTH_CNTRY_CODE,"608");	

	Mra_fld.AUTH_CARD_TYPE = 0;

	if(true == Check_If_VISA_Transaction())
	{
		Mra_fld.AUTH_CARD_ASSOC = 'V';
	}
	else if(true == Check_If_JCB_Transaction())
	{
		Mra_fld.AUTH_CARD_ASSOC = 'J';
	}
	else if(true == Check_If_AMEX_Transaction())
	{
		Mra_fld.AUTH_CARD_ASSOC = 'A';
	}
	else if(true == Check_If_MC_Transaction())
	{
		Mra_fld.AUTH_CARD_ASSOC = 'M';
	}
	else
	{
		Mra_fld.AUTH_CARD_ASSOC = 'O';
	}
	Mra_fld.AUTH_TYPE = 'M';
	memcpy(Mra_fld.AUTH_MCC,Auth_Tx.TLF01_details.category_code,4);
	//Mra_fld.AUTH_AMT = atol(Auth_Tx.TLF01_details.total_amount);
	/*strncpy(temp_mratotalamt,Auth_Tx.TLF01_details.total_amount,10);
	strncat(temp_mratotalamt,".",1);
	strncat(temp_mratotalamt,&Auth_Tx.TLF01_details.total_amount[10],2);
	strncpy(Mra_fld.AUTH_AMT,temp_mratotalamt,13);*/
	sprintf(Mra_fld.AUTH_AMT,"%d.%02d",atoi(&Auth_Tx.TLF01_details.total_amount)/100, atoi(&Auth_Tx.TLF01_details.total_amount)%100);

	strncpy(Mra_fld.AUTH_CRNCY_CODE,"608",3);		
		
	Mra_fld.AUTH_CRNCY_RATE = 1;
	
	strcpy(Mra_fld.AUTH_CREATE_DATE ,Auth_Tx.TLF01_details.date_yyyymmdd);		
	strcpy(Mra_fld.AUTH_CREATE_TIME ,Auth_Tx.TLF01_details.date_yyyymmdd);
	strcat (Mra_fld.AUTH_CREATE_TIME ,Auth_Tx.TLF01_details.time_hhmmss);
	strcpy(Mra_fld.AUTH_DATE ,Auth_Tx.TLF01_details.date_yyyymmdd);				
	strcpy(Mra_fld.AUTH_TIME , Auth_Tx.TLF01_details.date_yyyymmdd);
	strcat (Mra_fld.AUTH_TIME ,Auth_Tx.TLF01_details.time_hhmmss);
	if(Auth_Tx.TLF01_details.pos_condition_code[0] == '0')
		Mra_fld.AUTH_CATEGORY = 'P'; 
	else 
		Mra_fld.AUTH_CATEGORY = 'O'; 
	memcpy(Mra_fld.AUTH_TERM_ID,Auth_Tx.TLF01_details.terminal_id,sizeof(Auth_Tx.TLF01_details.terminal_id));
	memcpy(Mra_fld.AUTH_POS_CONDITION,	Auth_Tx.TLF01_details.pos_condition_code,2);
	Mra_fld.AUTH_TERM_TYPE = (CHAR) Auth_Tx.TLF01_details.terminal_type[0];
	Mra_fld.AUTH_TERM_CAPAB = ' ';
	//Mra_fld.AUTH_POS_ENTRY = (CHAR)Auth_Tx.TLF01_details.pos_entry_mode[0];
// POS entry modes
	
	if(strncmp((CHAR)Auth_Tx.TLF01_details.pos_entry_mode,"0000",4)==0 ||
		strncmp((CHAR)Auth_Tx.TLF01_details.pos_entry_mode,"0001",4)==0 ||
		strncmp((CHAR)Auth_Tx.TLF01_details.pos_entry_mode,"0002",4)==0 ||
		strncmp((CHAR)Auth_Tx.TLF01_details.pos_entry_mode,"0010",4)==0 ||
		strncmp((CHAR)Auth_Tx.TLF01_details.pos_entry_mode,"0011",4)==0 ||
		strncmp((CHAR)Auth_Tx.TLF01_details.pos_entry_mode,"0012",4)==0 ||
		strncmp((CHAR)Auth_Tx.TLF01_details.pos_entry_mode,"0070",4)==0 ||
		strncmp((CHAR)Auth_Tx.TLF01_details.pos_entry_mode,"0071",4)==0 ||
		strncmp((CHAR)Auth_Tx.TLF01_details.pos_entry_mode,"0072",4)==0 ||
		strncmp((CHAR)Auth_Tx.TLF01_details.pos_entry_mode,"0100",4)==0 ||
		strncmp((CHAR)Auth_Tx.TLF01_details.pos_entry_mode,"0110",4)==0 ||
		strncmp((CHAR)Auth_Tx.TLF01_details.pos_entry_mode,"0120",4)==0 
		)
	{
		Mra_fld.AUTH_POS_ENTRY='K';
	}
	else if(strncmp((CHAR)Auth_Tx.TLF01_details.pos_entry_mode,"0007",4)==0 ||
			strncmp((CHAR)Auth_Tx.TLF01_details.pos_entry_mode,"0070",4)==0	)
	{
		Mra_fld.AUTH_POS_ENTRY='D';
	}
	else if(strncmp((CHAR)Auth_Tx.TLF01_details.pos_entry_mode,"0020",4)==0 ||
		strncmp((CHAR)Auth_Tx.TLF01_details.pos_entry_mode,"0021",4)==0 ||
		strncmp((CHAR)Auth_Tx.TLF01_details.pos_entry_mode,"0022",4)==0 ||
		strncmp((CHAR)Auth_Tx.TLF01_details.pos_entry_mode,"0028",4)==0 ||
		strncmp((CHAR)Auth_Tx.TLF01_details.pos_entry_mode,"0029",4)==0 ||
		strncmp((CHAR)Auth_Tx.TLF01_details.pos_entry_mode,"0060",4)==0 ||
		strncmp((CHAR)Auth_Tx.TLF01_details.pos_entry_mode,"0061",4)==0 ||
		strncmp((CHAR)Auth_Tx.TLF01_details.pos_entry_mode,"0062",4)==0 ||
		strncmp((CHAR)Auth_Tx.TLF01_details.pos_entry_mode,"0200",4)==0 ||
		strncmp((CHAR)Auth_Tx.TLF01_details.pos_entry_mode,"0210",4)==0 ||
		strncmp((CHAR)Auth_Tx.TLF01_details.pos_entry_mode,"0220",4)==0 ||
		strncmp((CHAR)Auth_Tx.TLF01_details.pos_entry_mode,"0912",4)==0 )
	{
		Mra_fld.AUTH_POS_ENTRY='U';
	}
	else if(strncmp((CHAR)Auth_Tx.TLF01_details.pos_entry_mode,"0802",4)==0 ||
			strncmp((CHAR)Auth_Tx.TLF01_details.pos_entry_mode,"0080",4)==0 ||
			strncmp((CHAR)Auth_Tx.TLF01_details.pos_entry_mode,"0081",4)==0)
		{
			Mra_fld.AUTH_POS_ENTRY='F';
		}
	else if(strncmp((CHAR)Auth_Tx.TLF01_details.pos_entry_mode,"0810",4)==0 ||
		strncmp((CHAR)Auth_Tx.TLF01_details.pos_entry_mode,"0812",4)==0 ||
		strncmp((CHAR)Auth_Tx.TLF01_details.pos_entry_mode,"8100",4)==0 ||
		strncmp((CHAR)Auth_Tx.TLF01_details.pos_entry_mode,"8120",4)==0)
		{
			Mra_fld.AUTH_POS_ENTRY='E';
		}
	else if(strncmp((CHAR)Auth_Tx.TLF01_details.pos_entry_mode,"0051",4)==0 ||
		strncmp((CHAR)Auth_Tx.TLF01_details.pos_entry_mode,"0050",4)==0 ||
		strncmp((CHAR)Auth_Tx.TLF01_details.pos_entry_mode,"0052",4)==0 )
		{
			Mra_fld.AUTH_POS_ENTRY='V';
		}

	else if(strncmp((CHAR)Auth_Tx.TLF01_details.pos_entry_mode,"0091",4)==0)
		{Mra_fld.AUTH_POS_ENTRY='C';
		}

	else 
		{
			Mra_fld.AUTH_POS_ENTRY=' ';
		}

	if(strncmp(Auth_Tx.TLF01_details.security_response_code,"00",2 )== 0)
		Mra_fld.AUTH_CVV = 'V'; 
	else if(strncmp(Auth_Tx.TLF01_details.security_response_code,"01" ,2)== 0)
		Mra_fld.AUTH_CVV = 'I'; 
	else 
		Mra_fld.AUTH_CVV = ' '; 
	Mra_fld.AUTH_AVS = 0;	
	Mra_fld.AUTH_CARD_MEDIA_TYPE = ' ' ;	
	Mra_fld.AUTH_CVV2_PRESENT = '0';		
	Mra_fld.AUTH_CVV2_RESPONSE = ' ';		
	Mra_fld.AUTH_AVS_RESPONSE = 0;		
	Mra_fld.AUTH_PIN_VER= ' ';			
	memcpy(Mra_fld.AUTH_ACQ_REF_NUM,Auth_Tx.TLF01_details.retrieval_ref_num,sizeof(Auth_Tx.TLF01_details.retrieval_ref_num));		
	strcpy(Mra_fld.AUTH_FALCON_SCORE," ");
	//strncpy(Mra_fld.AUTH_EXTERNAL_ID,"A",1);
	strncat(Mra_fld.AUTH_EXTERNAL_ID,Auth_Tx.TLF01_details.primary_key.transaction_id,20);
	/*if(strncmp(Auth_Tx.TLF01_details.response_code,"00",2 )== 0)
		Mra_fld.AUTH_AUTH_DEC = 'A'; 
	else
		Mra_fld.AUTH_AUTH_DEC = 'A'; */
	if(strncmp(Auth_Tx.TLF01_details.response_code,"00",2 )== 0)
		Mra_fld.AUTH_AUTH_DEC  = 'A'; 
	else if(strncmp(Auth_Tx.TLF01_details.response_code,"01",2 )== 0)
		Mra_fld.AUTH_AUTH_DEC  = 'R'; 
	else if(strncmp(Auth_Tx.TLF01_details.response_code,"04",2 )== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"07",2 )== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"34",2 )== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"41",2 )== 0 ||
			strncmp(Auth_Tx.TLF01_details.response_code,"43",2 )== 0 
			)
		Mra_fld.AUTH_AUTH_DEC  = 'P';
		else			
		Mra_fld.AUTH_AUTH_DEC  = 'D';

	//memcpy(Mra_fld.AUTH_BATCH_ID, 	Auth_Tx.TLF01_details.batch_number,sizeof(Auth_Tx.TLF01_details.batch_number));
    	if(strcmp(Auth_Tx.TLF01_details.batch_number,"ACTIVE") == 0)
			strcpy(Auth_Tx.TLF01_details.batch_number,"0");
	memcpy(Mra_fld.AUTH_BATCH_ID, 	Auth_Tx.TLF01_details.batch_number,sizeof(Auth_Tx.TLF01_details.batch_number));

	Mra_fld.AUTH_USER_IND_2 = ' ';
	Mra_fld.AUTH_USER_IND_3 = ' ';
	Mra_fld.AUTH_USER_IND_4 = ' ';
	Mra_fld.AUTH_USER_IND_5 = ' ';
	strcpy(Mra_fld.AUTH_USER_SHORT_1, " ");
	strcpy(Mra_fld.AUTH_USER_SHORT_2, " ");
	strcpy(Mra_fld.AUTH_USER_SHORT_3, " ");
	strcpy(Mra_fld.AUTH_USER_LONG_1, " ");
	strcpy(Mra_fld.AUTH_USER_LONG_2, " ");
	strcpy(Mra_fld.AUTH_USER_LONG_3, " ");
	Mra_fld.AUTH_USER_NUM_1 = 0;		
	Mra_fld.AUTH_USER_NUM_2 = 0;		
	Mra_fld.AUTH_USER_NUM_3 = 0;
	strcpy(Mra_fld.AUTH_USER_DATE_1,(pCHAR) Auth_Tx.TLF01_details.date_yyyymmdd);	
	strcpy(Mra_fld.AUTH_USER_DATE_2,(pCHAR)Auth_Tx.TLF01_details.date_yyyymmdd);	
	strcpy(Mra_fld.AUTH_USER_DATE_3,(pCHAR)Auth_Tx.TLF01_details.date_yyyymmdd);
	strcpy(Mra_fld.MRA_TXN_START_TIME,(pCHAR)Auth_Tx.TLF01_details.date_yyyymmdd);
	strcat(Mra_fld.MRA_TXN_START_TIME,(pCHAR)Auth_Tx.TLF01_details.time_hhmmss);
	strcpy(Mra_fld.MRA_TXN_END_TIME,(pCHAR)Auth_Tx.TLF01_details.date_yyyymmdd);
	strcat(Mra_fld.MRA_TXN_END_TIME,(pCHAR)Auth_Tx.TLF01_details.time_hhmmss);
	Mra_fld.MRA_RESULT_FLAG = ' ';

	
	
}

/*******************************************************************************
*
*  FUNCTION:         write_to_mra_data_file
*
*  DESCRIPTION:      This function writes to falcon archive file. 
*
*  INPUTS:           None
*         
*  OUTPUTS:          None
*
*  RETURN VALUE:     BOOLEAN true for success or false for failure.
*   
*******************************************************************************/
BOOLEAN  write_to_mra_data_file()
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

   /*** Open file ***/
	mra_path=getenv( "ASCENDENTROOT" );
	strcpy(mra_buffer,mra_path);
	strcat(mra_buffer,"/log/MRA_Archive.csv");

    if(header == 0)
	{
		
		mra_file_write = fopen (mra_buffer, "w");
		fprintf(mra_file_write, "auth_seq_id,account_number,auth_acct_num,auth_auth_post_flag,auth_exp_date,auth_cntry_code,auth_card_type ,\                          
auth_card_assoc,auth_type , auth_mcc,auth_amt,auth_crncy_code,auth_crncy_rate ,auth_create_date ,\
auth_create_time,auth_date,auth_time ,auth_category ,auth_term_id ,auth_pos_condition,auth_term_type ,\
auth_term_capab  ,auth_pos_entry ,auth_cvv ,auth_avs ,auth_card_media_type  ,auth_cvv2_present ,auth_cvv2_response  ,\
auth_avs_response,auth_pin_ver ,auth_acq_ref_num,auth_falcon_score ,auth_external_id ,auth_auth_dec ,auth_batch_id   ,\
auth_user_ind_2 ,auth_user_ind_3 ,auth_user_ind_4 ,auth_user_ind_5 ,auth_user_short_1  ,auth_user_short_2,auth_user_short_3,auth_user_long_1 ,\                        
,auth_user_long_2,auth_user_long_3,auth_user_num_1,auth_user_num_2,auth_user_num_3  ,auth_user_date_1 , auth_user_date_2 ,\
auth_user_date_3 ,mra_txn_start_time , mra_txn_end_time ,mra_result_flag \n");
	header =1;
	}

   if (mra_file_write == NULL_PTR)
   {  
      strcpy(buffer, "Cannot open Falcon Archive Utility file: ");
      strcat(buffer, mra_buffer); 
        
      TxUtils_Send_Msg_To_Operator ( 0,   /* 1 - write to sys. mon., 0 - not */
         buffer,                          /* the text of the message */ 
		   1,                               /* 1 - write to event log, 0 - not */
         ERROR_MSG,                       /* message type */ 
		   "write_to_falcon_archive_file",       /* name of the function */
         4,                               /* severity */
         FATAL_ERROR,                     /* message type */              
         NULL_PTR,                        /* TID */
		   NULL_PTR,                        /* card number */
         NULL_PTR);                       /* merchant id */

	  return(false);
   }

   /*** Write to file ***/
   
   
   //if (fputs(mssg,c_file_write) < 0)
   
   //printf("%c - %d\n",Mra_fld.auth_auth_post_flag[0],	Mra_fld.auth_auth_post_flag[0]	);
//printf("%s\n",(char *) &Mra_fld.auth_auth_post_flag);
//printf("%s\n",Mra_fld.ACCOUNT_NUMBER);

//if((fprintf (mra_file_write,"%-20s,%-15s,%-19s,%-2s,%-8s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n",
if((fprintf (mra_file_write,"%s, %s, %s,%c,%s,%s ,%c ,%c,%c,%s,  %s,%s,  %d,  %s,  %s,  %s,  %s,  %c,  %s,  %s,  %c,%c, %c, %c,  %c, %c,  %c, %c,  %c,  %c,%s,%s,%s,%c,%s,%c,%c,%c,%c,%s,%s,%s, %s,%s,%s,%c,%c,%c,%s,%s,%s,%s,%s,%c\n",             
Mra_fld.AUTH_SEQ_ID,						    
		Mra_fld.ACCOUNT_NUMBER,					 
		Mra_fld.AUTH_ACCT_NUM,					           
		Mra_fld.AUTH_AUTH_POST_FLAG,				          
		Mra_fld.AUTH_EXP_DATE,					         
		Mra_fld.AUTH_CNTRY_CODE,					        
		Mra_fld.AUTH_CARD_TYPE,					         
		Mra_fld.AUTH_CARD_ASSOC,					          
		Mra_fld.AUTH_TYPE,					                   
		Mra_fld.AUTH_MCC,					                                        
		Mra_fld.AUTH_AMT,					                                
		Mra_fld.AUTH_CRNCY_CODE,					
		Mra_fld.AUTH_CRNCY_RATE,					               
		Mra_fld.AUTH_CREATE_DATE,				                
		Mra_fld.AUTH_CREATE_TIME,				                        
		Mra_fld.AUTH_DATE,					                                        
		Mra_fld.AUTH_TIME,					                                       
		Mra_fld.AUTH_CATEGORY,					                           
		Mra_fld.AUTH_TERM_ID,					                             
		Mra_fld.AUTH_POS_CONDITION,				                       
		Mra_fld.AUTH_TERM_TYPE,					                           
		Mra_fld.AUTH_TERM_CAPAB,					                         
		Mra_fld.AUTH_POS_ENTRY,					                           
		Mra_fld.AUTH_CVV,					                                         
		Mra_fld.AUTH_AVS,					                                        
		Mra_fld.AUTH_CARD_MEDIA_TYPE,				           
		Mra_fld.AUTH_CVV2_PRESENT,				                        
		Mra_fld.AUTH_CVV2_RESPONSE,				                      
		Mra_fld.AUTH_AVS_RESPONSE,				                        
		Mra_fld.AUTH_PIN_VER,					                            
		Mra_fld.AUTH_ACQ_REF_NUM,				                        
		Mra_fld.AUTH_FALCON_SCORE,				                        
		Mra_fld.AUTH_EXTERNAL_ID,				                      
		Mra_fld.AUTH_AUTH_DEC,					                            
		Mra_fld.AUTH_BATCH_ID,					                            
		Mra_fld.AUTH_USER_IND_2,					                          
		Mra_fld.AUTH_USER_IND_3,						                         
		Mra_fld.AUTH_USER_IND_4,						                         
		Mra_fld.AUTH_USER_IND_5,						                          
		Mra_fld.AUTH_USER_SHORT_1,					       
		Mra_fld.AUTH_USER_SHORT_2,					       
		Mra_fld.AUTH_USER_SHORT_3,					       
		Mra_fld.AUTH_USER_LONG_1,					        
		Mra_fld.AUTH_USER_LONG_2,					        
		Mra_fld.AUTH_USER_LONG_3,					        
		Mra_fld.AUTH_USER_NUM_1,						         
		Mra_fld.AUTH_USER_NUM_2,						         
		Mra_fld.AUTH_USER_NUM_3,						         
		Mra_fld.AUTH_USER_DATE_1,					        
		Mra_fld.AUTH_USER_DATE_2,					        
		Mra_fld.AUTH_USER_DATE_3,					        
		Mra_fld.MRA_TXN_START_TIME,					      
		Mra_fld.MRA_TXN_END_TIME,					        
		Mra_fld.MRA_RESULT_FLAG)) <0)	

	
{
		 
		  itoa (ferror(mra_file_write), tempstr, 10);
		  fclose (mra_file_write);
		  
		  strcpy(buffer, "Cannot write to Falcon Archive file: ");
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

		 // write_to_dump_file(buffer);

		return(false);
   }   
    return (true);

} /* write_to_falcon_archive_file */


void ProcessTransactions(void)
{
	
	
	/*int msglen=0;
	char temp_count[15]={0};
	char temp_message[1030]={0};*/

			memset(&Auth_Tx, 0, sizeof(Auth_Tx));
			memcpy (&Auth_Tx.TLF01_details, &tlf01_tmp_struct, sizeof (TLF01)) ;
		    tx_count++;
			RqMssg();
			/*sprintf(temp_count,"count %d:",count);
			strncpy(temp_message,temp_count,sizeof(count));
			strncat(temp_message,message,sizeof(message));
			sprintf(mssg_buffer,"%s\n",temp_message);*/
			write_to_falcon_archive_file(message);
			//printf("Porcessing tx -Count- : %ld\n",tx_count);
			if ( 0 == strcmp(Auth_Tx.TLF01_details.handler_queue, "dcpisoA") )
			{	mra_rqmssg();
				write_to_mra_data_file();
			}
			
}


/****************************************************************************
*
*  NAME:             MainProcessor
*
*  DESCRIPTION:      This function handles high-level application logic.
*
*  INPUTS:           None
*         
*  OUTPUTS:          None
*
*  RETURNS:          None
*
*  AUTHOR:           Ravikumar Nageshaiah
*   
*  MODIFIED BY:      
*
****************************************************************************/
int MainProcessor()
{
	CHAR     msg[256] = {0} ;
	CHAR     strTemp [256] = {0} ;
	CHAR time_date[25];
	char   msg_db[100];
	BYTE   rc;
	CHAR tf_filename [256]={0};;
    CHAR tmpstr	   [256]={0};

	memset (&Auth_Tx, 0, sizeof Auth_Tx) ;
	strcpy(ServiceName, "Falcon_Utility");

	/* Try to connect to the database */
	if (dbcommon_connect("equitable", "equitable", "equitable", "ORCL", msg) != PTEMSG_OK)
	{
		strcpy(msg, "Unable to Connect to Oracle ");
		TxUtils_Send_Msg_To_Operator (1,msg,1, ERROR_MSG,"MainProcessor",
							 1, FATAL_ERROR,0,0,0);
		return 1;
	}
//sleep(1000);
	
	/* Display Application version. */
	sprintf(strTemp, "Started %s, version %s", ServiceName, Version);
	TxUtils_Send_Msg_To_Operator (0,strTemp,1, INFO_MSG,"MainProcessor",
								 4, INFO_ERROR,0,0,0) ;
	printf(" started %s, version %s\n", ServiceName, Version);
	/* Initialize structure used to build TLF01 detail */
	memset(&Auth_Tx, 0, sizeof(Auth_Tx)); 
	memset(&tlf01_tmp_struct,0,sizeof(TLF01));
	printf("Calling TLF01 Details \n");
	tx_count =0;
	printf("FromDate : %s \n",Fromdate);
	printf("Todate :   %s  \n",Todate);

    GetAscendentConfigDirectory(tmpstr);
	sprintf(tf_filename, "%stf.ini", tmpstr);
		  
	/*get record length from ini file*/
	GetPrivateProfileString(
							"FALCON",	// points to section name
							"WORKFLOW", // points to key name 
							"APC80", 	// points to default string 
							profile_str,	 // points to destination buffer 
							sizeof(profile_str) - 1,// size of destination buffer 
							tf_filename );		 // points to initialization filename
	if(strcmp(Fromdate,Todate)==0)
	rc = db_get_tlf01_details_same_Date(&tlf01_tmp_struct,Fromdate,Todate,Fromtime,Totime,msg_db );
	else
	rc = db_get_tlf01_details_diff_Date(&tlf01_tmp_struct,Fromdate,Todate,Fromtime,Totime,msg_db );
	
	if (rc != PTEMSG_OK)
	{
		printf("\n\nError in accessing the database\n");
	}
	
	if(tx_count == 0 && rc == PTEMSG_OK)
	{
			sprintf( msg, "No Rows Selected. Could not find any records from %s to %s dates\n",Fromdate,Todate,ServiceName );
			printf("\n%s\n",msg);
	}
	else
	{
			printf("\nWriting into file........\n");
			sprintf(msg,"Falcon Archive Utility processed %ld number of transactions for %s\n", tx_count,ServiceName);
			printf("\n%s\n",msg);
	}
  	tx_count =0;
   return 0;
} /* MainProcessor */






/****************************************************************************
*
*  NAME:             main
*
*  DESCRIPTION:      This function gets optional initial parameter
*                    Network Controller.
*
*  INPUTS:           int argc - number of parameters
*                    pchar argv[1] - The network controller to which the 
*					 standin processed  transactions to be sent
*                  
*  OUTPUTS:          None
*
*  RETURNS:          None
*
*  AUTHOR:           Ravikumar Nageshaiah	
*   
*
****************************************************************************/
void main(int argc, char *argv[])
{
   	int rc = 0;
	int retval =0;
	char mess[256]={0};
    memset(Fromdate, 0, sizeof(Fromdate));
	memset(Todate, 0, sizeof(Todate));
	memset(Fromtime,0,sizeof(Fromtime));
	memset(Totime,0,sizeof(Totime));
	
	if ((argc > 1) && (argv[1] != NULL) && (argv[2] != NULL) && (argv[3] != NULL) && (argv[4] != NULL))
	{
		strcpy(Fromdate,argv[1]);
		strcpy(Todate,argv[2]);
		strcpy(Fromtime,argv[3]);
		strcpy(Totime,argv[4]);
		MainProcessor();
	}
	
	else if (strcmp(argv[1], "?") == 0)
   {
      display_settle_info();
   }
	else
	{
		printf("Error Running Falcon_Aarchive: Please Enter From and To Date(FORMAT:YYYYMMDD) and TIME(FORMAT:HHMMSS)\n\n");
	}
	
} /* main */

