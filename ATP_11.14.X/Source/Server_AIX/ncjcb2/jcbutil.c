/*******************************************************************************
* Copyright (c) 2007, Hypercom, Inc.
* All Rights Reserved.
*  
* MODULE:       jcbutil.c
*  
* TITLE:       
*  
*	DESCRIPTION: This module contains all the utilities used my the 
*               host handler to  parse and manipulate all incoming 
*               messages from the terminal and the host.
*
* APPLICATION: 
*
* AUTHOR:       B. Gable
REVISION HISTORY:

*******************************************************************/

#include <stdlib.h>
#include <stdio.h>

#include <time.h>
//#include <timeb.h>
#include "basictyp.h"
#include "pte.h"
#include "ptemsg.h"
#include "pteipc.h"
#include "ptetime.h"
#include "ptetimer.h"
#include "ntutils.h"

#include "app_info.h"
#include "equitdb.h"
#include "nc_dbstruct.h"
#include "tx_dbstruct.h"
#include "jcbmain.h"
#include "memmnger.h"
#include "txutils.h"
#include "genutil.h"
#include "txtrace.h"
#include "jcbutil.h"
#include "jcbtran.h"


/* functions in other parts of host handler */
extern void   perform_error_processing(pAUTH_TX);
extern BOOLEAN send_it_back_to_terminal(BYTE[3], pAUTH_TX);
extern void process_0302_from_host(pAUTH_TX p_auth_tx);


/**********************************
     VARIABLES
**********************************/
/* local variables */
INT    reqidx;                  /* next open position in request buffer */
INT    buflen;                  /* total size of buffer */
INT    bitmapidx;               /* offset of bitmap... */
INT    invalid_msg_format;
INT    de23_present;
CHAR   applnk_que_name [] = "applnkA" ;
CHAR   security_que_name [] = "racalA" ;

INT rvrsl_attmpts_value_fr_timeout_jcb=0;
INT rvrsl_attmpts_value_fr_laterspns_jcb=0;

/* global variables */
extern NCF01    ncf01_i;
extern NCF01    ncf01_a;
extern BYTE     current_state[2];

extern CHAR  ncjcb2_Error_warning_Filename[256];
extern CHAR  ncjcb2_module_error_warning_file_name[256];
extern CHAR  ncjcb2_error_warning_file_path[256];
extern CHAR  Log_Bufr[512]; /* used to copy term id, card num and merch id to error msg
	   	   	   	   	   	   	   	   and passed it to print in File and monitor */
CHAR masked_card_num[20] = {0};
UINT Max_File_Size_Defined = 0 ;
extern JCB_ONLINE_UPDATES  Online_Updates;

/* Database structure sizes */
extern INT    Tlf01_Size;
extern INT    Ncf01_Size;
extern INT    Ncf30_Size;
extern INT    Crf01_Size;
extern INT    Auth_Tx_Size;

extern CHAR  trands_que_name [];
extern CHAR  authorizer_que_name [];
extern CHAR  netds_que_name[];
extern CHAR  devds_que_name[];
extern CHAR  updatds_que_name[];
extern CHAR  cur_app_name[];
extern CHAR  oprmon_que_name[];
extern CHAR  dialog_que_name [];
extern CHAR  vLogon_Resp_Indicator[];
extern CHAR  vLogon_NMI_Code[];
extern BYTE  orig_host_start_time[];
extern CHAR  AppName[];
CHAR Dest_Stat_Id[7];

/* Define direction of transaction flow - switched in or switched out. */
extern INT  Tran_Flow;
extern INT  Txn_Type;

/* Statistics for Monitor */
extern TXN_COUNTS     Txn_Counts;
extern MONITOR_STATS  Monitor_Stats;

/* Txn Tracing Info */
extern CHAR   TraceFile[];
extern INT    Tracing;
extern FILE   TraceFile_fp;
extern FILE   *pTraceFile_fp;
extern FILE   **ppTraceFile_fp;

extern CHAR  ServiceCode[];
/*******************************************************/
/* REQUEST BUILD BUFFER                                */
/*******************************************************/
BYTE ReqRsp_Buffer [COMM_BUF_SZE];

char  A2E[4] = "A2E";    /*  ASCII to EBCDIC conversion */
char  E2A[4] = "E2A";    /*  EBCDIC to ASCII conversion */


/********************************************************/
/*  STRUCTURE FOR PRIVATE FIELD 63, TABLE 73			*/
/********************************************************/
typedef struct 
{
	char data_length       [3];
	char table_id          [3];
	char registration_flag [2];
	char account_selection [2];
	char card_data         [17];
	char challenge1        [5];
	char challenge2        [5];
	char cresponse         [3];
}Table73_Type;
  
/********************************************************/
/*  STRUCTURE FOR PRIVATE FIELD 63, TABLE 74			*/
/********************************************************/
typedef struct 
{
	char data_length       [3];
	char table_id          [3];
	char challenge1        [5];
	char challenge2        [5];
}Table74_Type;
  

/******************************************************/
/* DATA STRUCTURE FOR STORING DATA IN FINAL FORMAT... */
/******************************************************/
struct iso_msg_rec HOST_MESSAGE;



static  char proccode_str [7];

static  int fs_challenge_number = 1;

struct totals_record
   {
   BYTE captured_sales_count   [3];
   BYTE captured_sales_amount  [12];
   BYTE captured_refund_count  [3];
   BYTE captured_refund_amount [12];
   BYTE debit_sales_count      [3];
   BYTE debit_sales_amount     [12];
   BYTE debit_refund_count     [3];
   BYTE debit_refund_amount    [12];
   };


/*******************************************************/
/*  TRANSACTION ENUMERATION                            */
/*******************************************************/
enum trantypes {
   tt_sale,             /* sale */
   tt_check_dl,         /* check - driv lice verify */
   tt_debit,            /* debit */
   tt_offline,          /* offline txn */
   tt_refund,           /* refund */
   tt_settle,           /* settlement */
   tt_void,             /* void */
   tt_test,             /* test */
   tt_sale_response     /* Sale Response , Chipstipe Demo */
   };

enum tranopts1 {
   topt1_notip       = 1,      /* no tip amount */
   topt1_noacct      = 2,      /* no account required in proc code */
   topt1_nocapture   = 4,      /* no capture required */
   topt1_nosystime   = 8,      /* no system time required */
   topt1_credit      = 16,     /* credit type transaction */
   topt1_preadv      = 32,     /* send all advises prior */
   topt1_settle      = 64,     /* settle type transaction */
   topt1_postadv     = 128     /* send single advise after */
   };

enum tranopts2 {
   topt2_noprerev     = 1,      /* no reversal prior */
   topt2_crfl         = 2,      /* CRFL processing allowed */
   topt2_floor        = 4,      /* floor limit processing allowed */
   topt2_referral     = 8,      /* referral processing allowed */
   topt2_batpend      = 16,     /* set batch pending flag for this transaction */
   topt2_debit        = 32,     /* debit transaction */
   topt2_reversal     = 64,     /* send reversal if failure */
   topt2_new_proccode = 128     /* use msg's original proccode */
   };


//farad 8/10/1998 The bitfields enum was copied out of the passport version.
//The mcrd handler in passport was much different from the visa handler in passport.

enum bitfields {
   bf_done           = 0,      /* end of list    */
   bf_1xbitfield     = 1,      /* extended bit field */
   bf_2pan           = 2,      /* pan            */
   bf_3proc          = 3,      /* processing code */
   bf_4amount        = 4,      /* amount   */
   bf_5amount        = 5,      /* settelment mount   */
   bf_6crdbillamt    = 6,      /* amount cardholder billing */
   bf_7trntime       = 7,      /* transmission date-time */
   bf_9convrate      = 9,      /* Conversion rate, settlement*/
   bf_10convrate     = 10,     /* Conversion rate, cardholder billing */
   bf_11stan         = 11,     /* system trace number */
   bf_12time         = 12,     /* time    */
   bf_13date         = 13,     /* date, trans  */
   bf_14expiry       = 14,     /* expiry date  */
   bf_15datestl      = 15,     /* settlement date */
   bf_16convdate	 = 16,	   /* Conversion date */
   bf_18sicnumber    = 18,     /* Merchant's type of business */
   bf_19ctrycd       = 19,     /* Acquiring institution's country code */
   bf_20panctrycd    = 20,     /* pan extended, country code */
   bf_22pose         = 22,     /* pos entry mode */
   bf_23csn          = 23,     /* Card Sequence Number                 */
   bf_24nii          = 24,     /* nii      */
   bf_25pocc         = 25,     /* pos condition code */
   bf_26pincc        = 26,     /* pin capture code */
   bf_28feeamount	 = 28,	   /* Tran fee amount */
   bf_32idcode       = 32,     /* Acquiring institution's ID code */
   bf_33fwdidcode    = 33,     /* Forwarding institution's ID code */
   bf_34panext       = 34,     /* pan extended */
   bf_35track2       = 35,     /* track 2 data */
   bf_37rrn          = 37,     /* retrieval reference number */
   bf_38authcode     = 38,     /* auth code   */
   bf_39respcode     = 39,     /* response code*/
   bf_40src          = 40,     /* service restriction code */
   bf_41termid       = 41,     /* terminal id  */
   bf_42cardacc      = 42,     /* card acceptor id */
   bf_43cardaccname  = 43,     /* card acceptor name/location */
   bf_44respdata     = 44,     /* additional response data */
   bf_45track1       = 45,     /* track 1 data   */
   bf_48addl_data    = 48,     /* Additional Data - Private */
   bf_49trncurcd     = 49,     /* transaction currency code */
   bf_50sttlcurcd    = 50,     /* settlement transaction currency code */
   bf_51crdcurcd     = 51,     /* card currency code */
   bf_52pinblock     = 52,     /* pin block    */
   bf_53seccntl      = 53,     /* security related control info */
   bf_54addlamts     = 54,     /* additional amounts */
   bf_55iccdata      = 55,     /* .Integrated Circuit Card (ICC) System-related Data*/
   bf_59posdata      = 59,     /* national point-of-service geographic data */
   bf_60priv60       = 60,     /* private field 60 */
   bf_61priv61       = 61,     /* private field 61 */
   bf_62priv62       = 62,     /* private field 62 */
   bf_63priv63       = 63,     /* private field 63 */
   bf_70nmicode      = 70,     /* network management information code */
   bf_73action       = 73,     /* Date, action */
   bf_90orgdata      = 90,     /* original data elements   */
   bf_91fileupdate   = 91,     /* file update code         */
   bf_93rspind       = 93,     /* response indicator       */
   bf_95replamts     = 95,     /* replacement amounts      */
   bf_96msgsec       = 96,     /* message security code    */
   bf_99stlinstid    = 99,     /* settlement institution id code */
   bf_100rcvid       = 100,    /* receiving inst id code   */
   bf_101fname       = 101,    /* file name                */
   bf_102acct1       = 102,    /* acct id 1                */
   bf_103acct2       = 103,    /* acct id 2                */
   bf_104trandesc    = 104,    /* transaction description  */
   bf_118intractry   = 118,    /* intra country data       */
   bf_120record      = 120,    /* original message type id */
   bf_121issid       = 121,    /* issuing inst id code     */
   bf_122openuse     = 122,    /* remaining open-to-use    */
   bf_123addverf     = 123,    /* address verification data*/
   bf_124freeform    = 124,    /* free form text - Japan   */
   bf_127accesscode  = 127     /* free form text - Japan   */
   }; 

/* LENGTHs FOR EACH BIT FIELD */
//farad 8/10/1998 The next set of defines were remarked out becuase they did not
//exit in the visa handler in the passport version.


#define bf2_len       2
#define bf3_len       6
#define bf4_len      12
#define bf5_len      12
#define bf6_len      12
#define bf7_len      10
#define bf9_len       8
#define bf10_len      8
#define bf11_len      6
#define bf14_len      4
#define bf15_len      4
#define bf16_len      4
#define bf18_len      4
#define bf19_len      3
#define bf20_len      3
#define bf22_len      3
#define bf23_len      2
#define bf32_len      2
#define bf32data_len 11
#define bf33_len      2
#define bf35_len      2
#define bf37_len     12
#define bf38_len      6
#define bf39_len      2
#define bf41_len      8
#define bf42_len     15
#define bf44_len      2
#define bf45_len      2
#define bf48_len      3
#define bf49_len      3
#define bf50_len      3
#define bf51_len      3
#define bf55_len      3
#define bf54_len      3
#define bf60_len      3
#define bf61_len      3
#define bf62_len      3
#define bf63_len      3
#define bf70_len      3
#define bf90_len     42
#define bf94_len      7
#define bf102_len     2
#define bf103_len     2
#define bf120_len     3
#define bf121_len     3
#define bf127_len     3


       
#define EXTENDED_BITFIELD 128
#define NORMAL_BITFIELD    64
int     BitFieldSize = NORMAL_BITFIELD;


struct trandef
{
   BYTE   td_trantype;  /* tran type for this record */
   BYTE   td_msgid[4];   /* iso 8583 message id       */
   BYTE   td_proccode[3]; /* iso 8583 processing code  */
   enum	  tranopts1  td_opts1;   /* options */
   enum   tranopts2  td_opts2;
   enum   bitfields  td_bitfields[EXTENDED_BITFIELD]; /* fields to put in message */
};

static  struct trandef TRANTABLE[] = {
      { 
      /********************************************
         S A L E                                       JCB
      ********************************************/
      AUTH_SALE,
      0xF0,0xF1,0xF0,0xF0,    /* message id, EBCDIC => 0100 */
      0x00,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,       /* MAND */
      bf_3proc,		/* MAND */
      bf_4amount, 	/* MAND */
      bf_7trntime,   /* MAND */
      bf_11stan,	   /* MAND */
      bf_12time,	   /* MAND */
      bf_13date,     /* MAND */
      bf_14expiry,	/* COND */
      bf_18sicnumber,/* MAND */
      bf_22pose,	   /* COND */
      bf_23csn,
      bf_25pocc,	   /* MAND */
      bf_26pincc,    /* MAND */
	  bf_28feeamount,
      bf_32idcode,   /* MAND */
      bf_33fwdidcode,/* MAND */
      bf_35track2,	/* COND */
      bf_37rrn,      /* MAND */
      bf_41termid,	/* COND */
      bf_42cardacc,	/* MAND */
	  bf_43cardaccname, /* MAND */
      bf_45track1,	/* COND */
      bf_48addl_data,/* COND */
      bf_49trncurcd, /* MAND */
      bf_52pinblock, /* COND */
      bf_53seccntl,  /* COND */
      bf_55iccdata,
      bf_61priv61,   /* COND */
      bf_done      /* end of fields to move */
      },
      { 
      /********************************************
         S A L E  R E S P O N S E                     JCB
      ********************************************/
      AUTH_SALE_RESPONSE,
      0xF0,0xF1,0xF1,0xF0,    /* message id, EBCDIC => 0110 */
      0x00,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,      /* MAND */
      bf_3proc,		/* MAND */
      bf_4amount,	/* MAND */
      bf_7trntime,  /* MAND */
      bf_11stan,	/* MAND */
      bf_12time,	   /* MAND */
      bf_13date,     /* MAND */
      bf_14expiry,	/* COND */
      bf_32idcode,  /* MAND */
      bf_33fwdidcode, /* MAND */
      bf_37rrn,     /* MAND */
      bf_38authcode, /* COND */
      bf_39respcode, /* MAND */
      bf_42cardacc,	/* MAND */
      bf_49trncurcd,/* MAND */
      bf_done      /* end of fields to move */
      },
      { 
      /********************************************
         A U T H O R I Z A T I O N           JCB
      ********************************************/
      AUTH_AUTHORIZATION,
      0xF0,0xF1,0xF0,0xF0,    /* message id, EBCDIC => 0100 */
      0x00,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,       /* MAND */
      bf_3proc,		/* MAND */
      bf_4amount, 	/* MAND */
      bf_7trntime,   /* MAND */
      bf_11stan,	   /* MAND */
      bf_12time,	   /* MAND */
      bf_13date,     /* MAND */
      bf_14expiry,	/* COND */
      bf_18sicnumber,/* MAND */
      bf_22pose,	   /* COND */
      bf_23csn,
      bf_25pocc,	   /* MAND */
      bf_26pincc,    /* MAND */
	  bf_28feeamount,
      bf_32idcode,   /* MAND */
      bf_33fwdidcode, /* MAND */
      bf_35track2,	/* COND */
      bf_37rrn,      /* MAND */
      bf_41termid,	/* COND */
      bf_42cardacc,	/* MAND */
	  bf_43cardaccname, /* MAND */
      bf_45track1,	/* COND */
      bf_48addl_data,/* COND */
      bf_49trncurcd, /* MAND */
      bf_52pinblock, /* COND */
      bf_53seccntl,  /* COND */
      bf_55iccdata,
      bf_61priv61,   /* COND */
      bf_done      /* end of fields to move */
      },
      { 
      /********************************************
         A U T H O R I Z A T I O N   R E S P O N S E                     
      ********************************************/
      AUTH_AUTHORIZATION_RESPONSE,
      0xF0,0xF1,0xF1,0xF0,    /* message id, EBCDIC => 0110 */
      0x00,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,      /* MAND */
      bf_3proc,		/* MAND */
      bf_4amount,	/* MAND */
      bf_7trntime,  /* MAND */
      bf_11stan,	/* MAND */
      bf_12time,	   /* MAND */
      bf_13date,     /* MAND */
      bf_14expiry,	/* COND */
      bf_32idcode,  /* MAND */
      bf_33fwdidcode, /* MAND */
      bf_37rrn,     /* MAND */
      bf_38authcode, /* COND */
      bf_39respcode, /* MAND */
      bf_42cardacc,	/* MAND */
      bf_49trncurcd,/* MAND */
      bf_done      /* end of fields to move */
      },
      { 
      /********************************************
         AUTH_CARD_VERIFICATION           JCB
      ********************************************/
      AUTH_CARD_VERIFICATION,
      0xF0,0xF1,0xF0,0xF0,    /* message id, EBCDIC => 0100 */
      0x00,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,       /* MAND */
      bf_3proc,		/* MAND */
      bf_4amount, 	/* MAND */
      bf_7trntime,   /* MAND */
      bf_11stan,	   /* MAND */
      bf_12time,	   /* MAND */
      bf_13date,     /* MAND */
      bf_14expiry,	/* COND */
      bf_18sicnumber,/* MAND */
      bf_22pose,	   /* COND */
      bf_23csn,
      bf_25pocc,	   /* MAND */
      bf_26pincc,    /* MAND */
      bf_32idcode,   /* MAND */
      bf_33fwdidcode,/* MAND */
      bf_35track2,	/* COND */
      bf_37rrn,      /* MAND */
      bf_41termid,	/* COND */
      bf_42cardacc,	/* MAND */
	  bf_43cardaccname, /* MAND */
      bf_45track1,	/* COND */
      bf_48addl_data,/* COND */
      bf_49trncurcd, /* MAND */
      bf_52pinblock, /* COND */
      bf_53seccntl,  /* COND */
      bf_55iccdata,
      bf_61priv61,   /* COND */
      bf_done      /* end of fields to move */
      },
      { 
      /********************************************
         AUTH_CARD_VERIFICATION   R E S P O N S E                     
      ********************************************/
      AUTH_CARD_VERIFICATION_RESPONSE,
      0xF0,0xF1,0xF1,0xF0,    /* message id, EBCDIC => 0110 */
      0x00,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,      /* MAND */
      bf_3proc,		/* MAND */
      bf_4amount,	/* MAND */
      bf_7trntime,  /* MAND */
      bf_11stan,	/* MAND */
      bf_12time,	   /* MAND */
      bf_13date,     /* MAND */
      bf_14expiry,	/* COND */
      bf_32idcode,  /* MAND */
      bf_33fwdidcode, /* MAND */
      bf_37rrn,     /* MAND */
      bf_38authcode, /* COND */
      bf_39respcode, /* MAND */
      bf_42cardacc,	/* MAND */
      bf_49trncurcd,/* MAND */
      bf_done      /* end of fields to move */
      },
      {
      /********************************************
         AUTH_PRE_AUTHORIZATION           JCB
      ********************************************/
       AUTH_PRE_AUTHORIZATION,
      0xF0,0xF1,0xF0,0xF0,    /* message id, EBCDIC => 0100 */
      0x00,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,       /* MAND */
      bf_3proc,		/* MAND */
      bf_4amount, 	/* MAND */
      bf_7trntime,   /* MAND */
      bf_11stan,	   /* MAND */
      bf_12time,	   /* MAND */
      bf_13date,     /* MAND */
      bf_14expiry,	/* COND */
      bf_18sicnumber,/* MAND */
      bf_22pose,	   /* COND */
      bf_23csn,
      bf_25pocc,	   /* MAND */
      bf_26pincc,    /* MAND */
      bf_32idcode,   /* MAND */
      bf_33fwdidcode,/* MAND */
      bf_35track2,	/* COND */
      bf_37rrn,      /* MAND */
      bf_41termid,	/* COND */
      bf_42cardacc,	/* MAND */
	  bf_43cardaccname, /* MAND */
      bf_45track1,	/* COND */
      bf_48addl_data,/* COND */
      bf_49trncurcd, /* MAND */
      bf_52pinblock, /* COND */
      bf_53seccntl,  /* COND */
      bf_55iccdata,
      bf_61priv61,   /* COND */
      bf_done      /* end of fields to move */
      },
      {
      /********************************************
         AUTH_PRE_AUTHORIZATION   R E S P O N S E
      ********************************************/
      AUTH_PRE_AUTHORIZATION_RESPONSE,
      0xF0,0xF1,0xF1,0xF0,    /* message id, EBCDIC => 0110 */
      0x00,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,      /* MAND */
      bf_3proc,		/* MAND */
      bf_4amount,	/* MAND */
      bf_7trntime,  /* MAND */
      bf_11stan,	/* MAND */
      bf_12time,	   /* MAND */
      bf_13date,     /* MAND */
      bf_14expiry,	/* COND */
      bf_32idcode,  /* MAND */
      bf_33fwdidcode, /* MAND */
      bf_37rrn,     /* MAND */
      bf_38authcode, /* COND */
      bf_39respcode, /* MAND */
      bf_42cardacc,	/* MAND */
      bf_49trncurcd,/* MAND */
      bf_done      /* end of fields to move */
      },
#if BDOR_62_PREAUTH_VOID
	  {
      /********************************************
         P R E A U T H  V O I D               JCB
      ********************************************/
	  AUTH_VOID_PRE_AUTHORIZATION,
      0xF0,0xF4,0xF2,0xF0,    /* message id, EBCDIC => 0420 */
      0x00,0x00,0x00,         /* processing code  (use original) */
      /**** OPTIONS ****/
      topt1_credit+topt1_postadv,
      topt2_batpend,          /* topt2_batpend is not used, if really used, need to change this line */
      /**** BIT FIELDS ****/
      bf_2pan,        /* COND */
      bf_3proc,       /* MAND */
      bf_4amount,     /* MAND */
      bf_5amount,     /* COND */
      bf_7trntime,    /* MAND */
      bf_9convrate,   /* COND */
      bf_11stan,      /* MAND */
      bf_12time,	   /* MAND */
      bf_13date,     /* MAND */
      bf_14expiry,		/* COND */
      bf_22pose,      /* COND */
	  bf_23csn,       /* MAND */
      bf_25pocc,      /* MAND */
      bf_32idcode,    /* MAND */
      bf_33fwdidcode, /* MAND */
      bf_37rrn,       /* MAND */
      bf_38authcode, /* COND */
      bf_39respcode, /* MAND */
      bf_42cardacc,   /* MAND */
	  bf_43cardaccname, /* MAND */
      bf_49trncurcd,  /* MAND */
      bf_90orgdata,   /* MAND */
      bf_95replamts,  /* MAND */
      bf_done         /* end of fields to move */
      },
      {
      /***************************************************
         P R E A U T H V O I D  R E S P O N S E       JCB
      ****************************************************/
      AUTH_VOID_PRE_AUTHORIZATION_RESPONSE,
      0xF0,0xF4,0xF3,0xF0,    /* message id, EBCDIC => 0430 */
      0x00,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_credit+topt1_postadv,
      topt2_batpend,          /* topt2_batpend is not used, if really used, need to change this line */
      /**** BIT FIELDS ****/
      bf_2pan,        /* COND */
      bf_3proc,       /* MAND */
      bf_4amount,     /* MAND */
      bf_7trntime,    /* MAND */
      bf_11stan,      /* MAND */
      bf_12time,	   /* MAND */
      bf_13date,     /* MAND */
      bf_32idcode,    /* MAND */
      bf_33fwdidcode, /* MAND */
      bf_37rrn,       /* MAND */
      bf_39respcode, /* MAND */
      bf_42cardacc,   /* MAND */
      bf_49trncurcd,  /* MAND */
      bf_90orgdata,   /* MAND */
      bf_95replamts,  /* MAND */
      bf_done         /* end of fields to move */
      },
#endif
	  /* Add 0120 for MCRD   Advices .. TF Phani */
	  { 
      /*************************************************************
         A U T H O R I Z A T I O N   A D V I C E   
       *************************************************************/
      AUTH_ADVICE,
      0xF0,0xF1,0xF2,0xF0,                  /* message id, EBCDIC => 0130 */
      0x00,0x00,0x00,						/* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,         /* MAND */
      bf_3proc,        /* MAND */
      bf_4amount,      /* MAND */
      bf_5amount,   /* COND */
      bf_6crdbillamt,  /* COND */
      bf_7trntime,     /* MAND */
      bf_9convrate,/* COND */
      bf_10convrate,   /* COND */
      bf_11stan,       /* MAND */
      bf_12time,       /* OPT  */
      bf_13date,       /* OPT  */
      bf_14expiry,     /* MAND */
      bf_15datestl,    /* COND */
      bf_19ctrycd,     /* MAND */
      bf_22pose,	   /* COND */
	  bf_25pocc,        /* MAND */
	  bf_28feeamount,
      bf_32idcode,     /* MAND */
      bf_33fwdidcode,      /* COND */
      bf_37rrn,        /* MAND */
	  bf_38authcode,    /* COND */ // TF Phani
      bf_39respcode,   /* MAND */
      bf_41termid,     /* COND */
      bf_42cardacc,    /* MAND */
      bf_43cardaccname, /*MAND */
	  bf_48addl_data,  /* MAND */ // TF Phani
      bf_49trncurcd,   /* MAND */
      bf_50sttlcurcd,/* COND */
      bf_51crdcurcd,   /* COND */
      bf_55iccdata,    /* COND */
	  bf_61priv61,   /* MAND */
      bf_62priv62,        /* COND */
      bf_63priv63,    /* COND */
      bf_done      /* end of fields to move */
      },

      { 
      /********************************************
         A U T H   A D V I C E   R E S P O N S E                     JCB
      ********************************************/
      AUTH_ADVICE_RESPONSE,
      0xF0,0xF1,0xF3,0xF0,    /* message id, EBCDIC => 0130 */
      0x00,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,      /* MAND */
      bf_3proc,		/* MAND */
      bf_4amount,	/* MAND */
      bf_7trntime,  /* MAND */
      bf_11stan,	/* MAND */
      bf_12time,	   /* MAND */
      bf_13date,     /* MAND */
      bf_14expiry,	/* COND */
      bf_32idcode,  /* MAND */
      bf_33fwdidcode, /* MAND */
      bf_37rrn,     /* MAND */
      bf_38authcode, /* COND */
      bf_39respcode, /* MAND */
      bf_42cardacc,	/* MAND */
      bf_49trncurcd,/* MAND */
      bf_done      /* end of fields to move */
      },
      {
	  /****************************************************
        A U T H   A D V I C E   R E P E A T   R E Q U E S T
      *****************************************************/
	  AUTH_ADVICE_REPEAT,
      0xF0,0xF1,0xF2,0xF1,             /* message id ==> 0121*/
      0x00,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
	  /**** BIT FIELDS ****/
      bf_2pan,			/* MAND */
      bf_3proc,			/* MAND */
      bf_4amount,		/* MAND */
      bf_7trntime,		/* MAND */
      bf_9convrate,     /* COND */    //void sale
      bf_11stan,        /* MAND */
      bf_12time,		/* MAND */
      bf_13date,        /* MAND */
      bf_14expiry,		/* COND */
      bf_18sicnumber,   /* MAND */
      bf_22pose,		/* COND */
      bf_23csn,			/* COND */
      bf_25pocc,        /* MAND */
      bf_26pincc,       /* MAND */
	  bf_28feeamount,	/* COND */
      bf_32idcode,      /* MAND */
      bf_33fwdidcode,   /* MAND */
      bf_35track2,		/* COND */
      bf_37rrn,         /* MAND */
      bf_41termid,		/* COND */
      bf_42cardacc,		/* MAND */
	  bf_43cardaccname, /* MAND */
      bf_45track1,		/* COND */
      bf_48addl_data,   /* COND */
      bf_49trncurcd,    /* MAND */
      bf_52pinblock,    /* COND */
      bf_53seccntl,     /* COND */
      bf_55iccdata,		/* COND */
      bf_61priv61,      /* COND */
      bf_done      /* end of fields to move */
      },
	  {
	  /******************************************************
         A U T H   A D V I C E   R P E A T   R E S P O N S E
      ******************************************************/
      AUTH_ADVICE_REPEAT_RESPONSE,
      0xF0,0xF1,0xF3,0xF1,      /* message id ==>0131 */
      0x00,0x00,0x00,         /* processing code ==> 000000 */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
	  bf_2pan,          /* MAND */
      bf_3proc,			/* MAND */
      bf_4amount,		/* MAND */
      bf_7trntime,  	/* MAND */
      bf_11stan,		/* MAND */
      bf_12time,		/* MAND */
      bf_13date,        /* MAND */
      bf_14expiry,		/* COND */
      bf_32idcode,  	/* MAND */
      bf_33fwdidcode,   /* MAND */
      bf_37rrn,     	/* MAND */
      bf_38authcode,    /* COND */
      bf_39respcode,    /* MAND */
      bf_42cardacc,		/* MAND */
      bf_49trncurcd,	/* MAND */
      bf_done      /* end of fields to move */
      },
	  {
		/********************************************
		   BALANCE EQRY                       JCB
		********************************************/
		AUTH_BALANCE_INQUIRY,
		0xF0,0xF1,0xF0,0xF0,    /* message id, EBCDIC => 0100 */
		0x01,0x00,0x00,         /* processing code */
		/**** OPTIONS ****/
		topt1_postadv,
		topt2_referral+topt2_reversal+topt2_floor,
		/**** BIT FIELDS ****/
		bf_2pan,       /* MAND */
		bf_3proc,		/* MAND */
		bf_4amount, 	/* MAND */
		bf_7trntime,   /* MAND */
		bf_11stan,	   /* MAND */
		bf_12time,	   /* MAND */
		bf_13date,     /* MAND */
		bf_14expiry,	/* COND */
		bf_18sicnumber,/* MAND */
		bf_22pose,	   /* COND */
		bf_23csn,
		bf_25pocc,	   /* MAND */
		bf_26pincc,    /* MAND */
		bf_32idcode,   /* MAND */
		bf_33fwdidcode,/* MAND */
		bf_35track2,	/* COND */
		bf_37rrn,      /* MAND */
		bf_41termid,	/* COND */
		bf_42cardacc,	/* MAND */
		bf_43cardaccname, /* MAND */
		bf_45track1,	/* COND */
		bf_48addl_data,/* COND */
		bf_49trncurcd, /* MAND */
		bf_52pinblock, /* COND */
		bf_53seccntl,  /* COND */
		bf_55iccdata,
		bf_61priv61,   /* COND */
		bf_done      /* end of fields to move */
		},
		{
		/********************************************
		   BALANCE EQRY   R E S P O N S E     JCB
		********************************************/
		AUTH_BALANCE_INQUIRY_RESPONSE,
		0xF0,0xF1,0xF1,0xF0,    /* message id, EBCDIC => 0110 */
		0x01,0x00,0x00,         /* processing code */
		/**** OPTIONS ****/
		topt1_postadv,
		topt2_referral+topt2_reversal+topt2_floor,
		/**** BIT FIELDS ****/
		bf_2pan,      /* MAND */
		bf_3proc,		/* MAND */
		bf_4amount,	/* MAND */
		bf_7trntime,  /* MAND */
		bf_11stan,	/* MAND */
		bf_12time,	   /* MAND */
		bf_13date,     /* MAND */
		bf_14expiry,	/* COND */
		bf_32idcode,  /* MAND */
		bf_33fwdidcode, /* MAND */
		bf_37rrn,     /* MAND */
		bf_38authcode, /* COND */
		bf_39respcode, /* MAND */
		bf_42cardacc,	/* MAND */
		bf_49trncurcd,/* MAND */
		bf_done      /* end of fields to move */
		},
      { 
      /*****************************************************
         A U T H   A D M I N   A D V I C E   R E S P O N S E    JCB
      ******************************************************/
      AUTH_ADMIN_ADVICE_RESPONSE,
      0xF0,0xF6,0xF3,0xF0,    /* message id, EBCDIC => 0630 */
      0x00,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_7trntime,   /* MAND */
      bf_11stan,	   /* MAND */
      bf_48addl_data /* COND? */
      },
      {   
      /********************************************
         V O I D   S A L E
      ********************************************/
      AUTH_VOID_SALE,
      0xF0,0xF4,0xF2,0xF0,    /* message id, EBCDIC => 0420 */
      0x00,0x00,0x00,         /* processing code  (use original) */
      /**** OPTIONS ****/
      topt1_credit+topt1_postadv,
      topt2_batpend,          /* topt2_batpend is not used, if really used, need to change this line */
      /**** BIT FIELDS ****/
      bf_2pan,        /* COND */
      bf_3proc,       /* MAND */
      bf_4amount,     /* MAND */
      bf_5amount,     /* COND */
      bf_7trntime,    /* MAND */
      bf_9convrate,   /* COND */
      bf_11stan,      /* MAND */
      bf_12time,      /* MAND */
      bf_13date,      /* MAND */
      bf_14expiry,		/* COND */
      bf_22pose,      /* COND */
	  bf_23csn,       /* MAND */
      bf_25pocc,      /* MAND */
      bf_32idcode,    /* MAND */
      bf_33fwdidcode, /* MAND */
      bf_37rrn,       /* MAND */
      bf_38authcode,  /* COND */
      bf_39respcode,  /* MAND */
      bf_42cardacc,   /* MAND */
	  bf_43cardaccname, /* MAND */
      bf_49trncurcd,  /* MAND */
      bf_90orgdata,   /* MAND */
      bf_95replamts,  /* MAND */
      bf_done         /* end of fields to move */
      },
	   {   
      /********************************************
         R E V E R S A L                 JCB
      ********************************************/
      AUTH_REVERSAL,
      0xF0,0xF4,0xF0,0xF0,    /* message id, EBCDIC => 0400 */
      0x00,0x00,0x00,         /* processing code  (use original) */
      /**** OPTIONS ****/
      topt1_credit+topt1_postadv,
      topt2_batpend,          /* topt2_batpend is not used, if really used, need to change this line */
      /**** BIT FIELDS ****/
      bf_2pan,        /* COND */
      bf_3proc,       /* MAND */
      bf_4amount,     /* MAND */
      bf_5amount,     /* COND */
      bf_7trntime,    /* MAND */
      bf_9convrate,   /* COND */
      bf_11stan,      /* MAND */
      bf_12time,	   /* MAND */
      bf_13date,     /* MAND */
      bf_14expiry,		/* COND */
      bf_22pose,      /* COND */
	  bf_23csn,       /* MAND */
      bf_25pocc,      /* MAND */
      bf_32idcode,    /* MAND */
      bf_33fwdidcode, /* MAND */
      bf_37rrn,       /* MAND */
      bf_38authcode, /* COND */
      bf_39respcode, /* MAND */
      bf_42cardacc,   /* MAND */
	  bf_43cardaccname, /* MAND */
      bf_49trncurcd,  /* MAND */
      bf_90orgdata,   /* MAND */
      bf_95replamts,  /* MAND */
      bf_done         /* end of fields to move */
      },
      {
      /***************************************************
         R E V E R S A L      R E S P O N S E       JCB
      ****************************************************/
      AUTH_REVERSAL_RESPONSE,
      0xF0,0xF4,0xF1,0xF0,    /* message id, EBCDIC => 0410 */
      0x00,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_credit+topt1_postadv,
      topt2_batpend,          /* topt2_batpend is not used, if really used, need to change this line */
      /**** BIT FIELDS ****/
      bf_2pan,        /* COND */
      bf_3proc,       /* MAND */
      bf_4amount,     /* MAND */
      bf_7trntime,    /* MAND */
      bf_11stan,      /* MAND */
      bf_12time,	   /* MAND */
      bf_13date,     /* MAND */
      bf_32idcode,    /* MAND */
      bf_33fwdidcode, /* MAND */
      bf_37rrn,       /* MAND */
      bf_39respcode, /* MAND */
      bf_42cardacc,   /* MAND */
      bf_49trncurcd,  /* MAND */
      bf_90orgdata,   /* MAND */
      bf_95replamts,  /* MAND */
      bf_done         /* end of fields to move */
      },
     
      {   
      /********************************************
         R E V E R S A L   A D V I C E                JCB
      ********************************************/
      AUTH_REVERSAL_ADVICE,
      0xF0,0xF4,0xF2,0xF0,    /* message id, EBCDIC => 0420 */
      0x00,0x00,0x00,         /* processing code  (use original) */
      /**** OPTIONS ****/
      topt1_credit+topt1_postadv,
      topt2_batpend,          /* topt2_batpend is not used, if really used, need to change this line */
      /**** BIT FIELDS ****/
      bf_2pan,        /* COND */
      bf_3proc,       /* MAND */
      bf_4amount,     /* MAND */
      bf_5amount,     /* COND */
      bf_7trntime,    /* MAND */
      bf_9convrate,   /* COND */
      bf_11stan,      /* MAND */
      bf_12time,	   /* MAND */
      bf_13date,     /* MAND */
      bf_14expiry,		/* COND */
      bf_22pose,      /* COND */
	  bf_23csn,       /* MAND */
      bf_25pocc,      /* MAND */
      bf_32idcode,    /* MAND */
      bf_33fwdidcode, /* MAND */
      bf_37rrn,       /* MAND */
      bf_38authcode, /* COND */
      bf_39respcode, /* MAND */
      bf_42cardacc,   /* MAND */
	  bf_43cardaccname, /* MAND */
      bf_49trncurcd,  /* MAND */
      bf_90orgdata,   /* MAND */
      bf_95replamts,  /* MAND */
      bf_done         /* end of fields to move */
      },
      {
      /***************************************************
         R E V E R S A L    A D V I C E   R E S P O N S E       JCB
      ****************************************************/
      AUTH_REVERSAL_ADVICE_RESPONSE,
      0xF0,0xF4,0xF3,0xF0,    /* message id, EBCDIC => 0430 */
      0x00,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_credit+topt1_postadv,
      topt2_batpend,          /* topt2_batpend is not used, if really used, need to change this line */
      /**** BIT FIELDS ****/
      bf_2pan,        /* COND */
      bf_3proc,       /* MAND */
      bf_4amount,     /* MAND */
      bf_7trntime,    /* MAND */
      bf_11stan,      /* MAND */
      bf_12time,	   /* MAND */
      bf_13date,     /* MAND */
      bf_32idcode,    /* MAND */
      bf_33fwdidcode, /* MAND */
      bf_37rrn,       /* MAND */
      bf_39respcode, /* MAND */
      bf_42cardacc,   /* MAND */
      bf_49trncurcd,  /* MAND */
      bf_90orgdata,   /* MAND */
      bf_95replamts,  /* MAND */
      bf_done         /* end of fields to move */
      },
      {   
      /**********************************************
         R E V E R S A L   R E P E A T                  JCB
      ***********************************************/
      AUTH_REVERSAL_REPEAT,
      0xF0,0xF4,0xF2,0xF1,    /* message id, EBCDIC => 0421 */
      0x00,0x00,0x00,         /* processing code  (use original) */
      /**** OPTIONS ****/
      topt1_credit+topt1_postadv,
      topt2_batpend,          /* topt2_batpend is not used, if really used, need to change this line */
      /**** BIT FIELDS ****/
      bf_2pan,        /* COND */
      bf_3proc,       /* MAND */
      bf_4amount,     /* MAND */
      bf_5amount,     /* COND */
      bf_7trntime,    /* MAND */
      bf_9convrate,   /* COND */
      bf_11stan,      /* MAND */
      bf_12time,	   /* MAND */
      bf_13date,     /* MAND */
      bf_14expiry,		/* COND */
      bf_22pose,      /* COND */
	  bf_23csn,       /* MAND */
      bf_25pocc,      /* MAND */
      bf_32idcode,    /* MAND */
      bf_33fwdidcode, /* MAND */
      bf_37rrn,       /* MAND */
      bf_38authcode, /* COND */
      bf_39respcode, /* MAND */
      bf_42cardacc,   /* MAND */
	  bf_43cardaccname, /* MAND */
      bf_49trncurcd,  /* MAND */
      bf_90orgdata,   /* MAND */
      bf_95replamts,  /* MAND */
      bf_done         /* end of fields to move */
      },
      { 
      /********************************************
         C A S H   A D V A N C E                       JCB
      ********************************************/
      AUTH_CASH_ADVANCE,
      0xF0,0xF1,0xF0,0xF0,    /* message id, EBCDIC => 0100 */
      0x01,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,       /* MAND */
      bf_3proc,		/* MAND */
      bf_4amount, 	/* MAND */
      bf_7trntime,   /* MAND */
      bf_11stan,	   /* MAND */
      bf_12time,	   /* MAND */
      bf_13date,     /* MAND */
      bf_14expiry,	/* COND */
      bf_18sicnumber,/* MAND */
      bf_22pose,	   /* COND */
      bf_23csn,
      bf_25pocc,	   /* MAND */
      bf_26pincc,    /* MAND */
	  bf_28feeamount,/* COND */
      bf_32idcode,   /* MAND */
      bf_33fwdidcode,/* MAND */
      bf_35track2,	/* COND */
      bf_37rrn,      /* MAND */
      bf_41termid,	/* COND */
      bf_42cardacc,	/* MAND */
	  bf_43cardaccname, /* MAND */
      bf_45track1,	/* COND */
      bf_48addl_data,/* COND */
      bf_49trncurcd, /* MAND */
      bf_52pinblock, /* COND */
      bf_53seccntl,  /* COND */
      bf_55iccdata,
      bf_61priv61,   /* COND */
      bf_done      /* end of fields to move */
      },
      { 
      /********************************************
         C A S H   A D V A N C E   R E S P O N S E     JCB
      ********************************************/
      AUTH_CASH_ADVANCE_RESPONSE,
      0xF0,0xF1,0xF1,0xF0,    /* message id, EBCDIC => 0110 */
      0x01,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,      /* MAND */
      bf_3proc,		/* MAND */
      bf_4amount,	/* MAND */
      bf_7trntime,  /* MAND */
      bf_11stan,	/* MAND */
      bf_12time,	   /* MAND */
      bf_13date,     /* MAND */
      bf_14expiry,	/* COND */
      bf_32idcode,  /* MAND */
      bf_33fwdidcode, /* MAND */
      bf_37rrn,     /* MAND */
      bf_38authcode, /* COND */
      bf_39respcode, /* MAND */
      bf_42cardacc,	/* MAND */
      bf_49trncurcd,/* MAND */
      bf_done      /* end of fields to move */
      },
      { 
      /*******************************************************
         C A S H   A D V A N C E    A U T H O R I Z A T I O N
      *******************************************************/
      AUTH_CASH_ADVANCE_AUTHORIZATION,
      0xF0,0xF1,0xF0,0xF0,    /* message id, EBCDIC => 0100 */
      0x01,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,       /* MAND */
      bf_3proc,		/* MAND */
      bf_4amount, 	/* MAND */
      bf_7trntime,   /* MAND */
      bf_11stan,	   /* MAND */
      bf_12time,	   /* MAND */
      bf_13date,     /* MAND */
      bf_14expiry,	/* COND */
      bf_18sicnumber,/* MAND */
      bf_22pose,	   /* COND */
      bf_23csn,
      bf_25pocc,	   /* MAND */
      bf_26pincc,    /* MAND */
	  bf_28feeamount,/* COND */
      bf_32idcode,   /* MAND */
      bf_33fwdidcode,/* MAND */
      bf_35track2,	/* COND */
      bf_37rrn,      /* MAND */
      bf_41termid,	/* COND */
      bf_42cardacc,	/* MAND */
	  bf_43cardaccname, /* MAND */
      bf_45track1,	/* COND */
      bf_48addl_data,/* COND */
      bf_49trncurcd, /* MAND */
      bf_52pinblock, /* COND */
      bf_53seccntl,  /* COND */
      bf_55iccdata,
      bf_61priv61,   /* COND */
      bf_done      /* end of fields to move */
      },
      { 
      /***************************************************
         Q U A S I   C A S H   (Really an Authorization)
      ****************************************************/
      AUTH_QUASI_CASH,
      0xF0,0xF1,0xF0,0xF0,    /* message id, EBCDIC => 0100 */
      0x00,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,       /* MAND */
      bf_3proc,		/* MAND */
      bf_4amount, 	/* MAND */
      bf_7trntime,   /* MAND */
      bf_11stan,	   /* MAND */
      bf_12time,	   /* MAND */
      bf_13date,     /* MAND */
      bf_14expiry,	/* COND */
      bf_18sicnumber,/* MAND */
      bf_22pose,	   /* COND */
      bf_23csn,
      bf_25pocc,	   /* MAND */
      bf_26pincc,    /* MAND */
	  bf_28feeamount,/* COND */
      bf_32idcode,   /* MAND */
      bf_33fwdidcode,/* MAND */
      bf_35track2,	/* COND */
      bf_37rrn,      /* MAND */
      bf_41termid,	/* COND */
      bf_42cardacc,	/* MAND */
	  bf_43cardaccname, /* MAND */
      bf_45track1,	/* COND */
      bf_48addl_data,/* COND */
      bf_49trncurcd, /* MAND */
      bf_52pinblock, /* COND */
      bf_53seccntl,  /* COND */
      bf_55iccdata,
      bf_61priv61,   /* COND */
      bf_done      /* end of fields to move */
      },
      { 
      /********************************************
         O N L I N E   F I L E   U P D A T E
      ********************************************/
      AUTH_CARD_UPDATE,
      0xF0,0xF3,0xF0,0xF2,    /* message id, EBCDIC => 0302 */
      0x00,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_7trntime,     /* MAND */
      bf_11stan,       /* MAND */
      bf_32idcode,     /* MAND */
      bf_33fwdidcode,  /* MAND */
      bf_41termid,     /* COND */
      bf_63priv63,     /* MAND */
      bf_100rcvid,     /* COND */
      bf_101fname,     /* MAND */
      bf_120record,    /* MAND */
      bf_127accesscode,/* MAND */
      bf_done          /* end of fields to move */
      },
	  { 
      /********************************************
         O N L I N E   F I L E   U P D A T E RESPONSE .. TF PHANI
      ********************************************/
      AUTH_CARD_UPDATE_RESPONSE,
      0xF0,0xF3,0xF1,0xF2,    /* message id, EBCDIC => 0312 */
      0x00,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_7trntime,     /* MAND */
      bf_11stan,       /* MAND */
      bf_32idcode,     /* MAND */
      bf_33fwdidcode,  /* MAND */
	  bf_39respcode,				// TF Phani ..Prodcution issue.
      bf_41termid,     /* COND */
      bf_63priv63,     /* MAND */
      bf_100rcvid,     /* COND */
      bf_101fname,     /* MAND */
      bf_120record,    /* MAND */
      bf_127accesscode,/* MAND */
      bf_done          /* end of fields to move */
      },
      {
      /********************************************
         L O G O N                                    JCB
      ********************************************/
      AUTH_LOGON,
      0xF0,0xF8,0xF0,0xF0,    /* message id, EBCDIC => 0800 */
      0x92,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_noacct+topt1_nocapture,
      topt2_noprerev,
      /**** BIT FIELDS ****/
      bf_7trntime,    /* MAND */
	   bf_11stan,	    /* MAND */
      bf_33fwdidcode, /* MAND */
      bf_53seccntl,  /* COND */
      bf_70nmicode,   /* MAND */
      bf_96msgsec,    /* COND */
      bf_99stlinstid, /* COND */
      bf_100rcvid,    /* MAND */
      bf_done       /* end of fields to move */
      },
      {
      /********************************************
         L O G O N    R E S P O N S E                JCB
      ********************************************/
      AUTH_LOGON_RESPONSE,
      0xF0,0xF8,0xF1,0xF0,    /* message id, EBCDIC => 0810 */
      0x92,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_noacct+topt1_nocapture,
      topt2_noprerev,
      /**** BIT FIELDS ****/
      bf_7trntime,    /* MAND */
	   bf_11stan,	    /* MAND */
      bf_33fwdidcode, /* MAND */
      bf_39respcode, /* MAND */
      bf_53seccntl,  /* COND */
      bf_70nmicode,   /* MAND */
      bf_99stlinstid, /* COND */
      bf_100rcvid,    /* MAND */
      bf_done       /* end of fields to move */
      }
   };

#define N_TRANENTRY (sizeof(TRANTABLE) / sizeof(struct trandef))

/********************************************************/
/* BIT SETTING TABLE                                    */
/********************************************************/
struct isobit 
   {
   INT byte_no;         /* byte number for this bit field */
   INT bit_mask;        /* bit to set in this byte        */
   };

const struct isobit BITTAB[128+1] =
   {
   { 0, 0 }, /*zero justify */
   { 0, 0x80 },   /* bit 1  */   { 0, 0x40 },   /* bit 2  */   { 0, 0x20 },   /* bit 3  */   { 0, 0x10 },   /* bit 4  */
   { 0, 0x08 },   /* bit 5  */   { 0, 0x04 },   /* bit 6  */   { 0, 0x02 },   /* bit 7  */   { 0, 0x01 },   /* bit 8  */
   { 1, 0x80 },   /* bit 9  */   { 1, 0x40 },   /* bit 10 */   { 1, 0x20 },   /* bit 11 */   { 1, 0x10 },   /* bit 12 */
   { 1, 0x08 },   /* bit 13 */   { 1, 0x04 },   /* bit 14 */   { 1, 0x02 },   /* bit 15 */   { 1, 0x01 },   /* bit 16 */
   { 2, 0x80 },   /* bit 17 */   { 2, 0x40 },   /* bit 18 */   { 2, 0x20 },   /* bit 19 */   { 2, 0x10 },   /* bit 20 */
   { 2, 0x08 },   /* bit 21 */   { 2, 0x04 },   /* bit 22 */   { 2, 0x02 },   /* bit 23 */   { 2, 0x01 },   /* bit 24 */
   { 3, 0x80 },   /* bit 25 */   { 3, 0x40 },   /* bit 26 */   { 3, 0x20 },   /* bit 27 */   { 3, 0x10 },   /* bit 28 */
   { 3, 0x08 },   /* bit 29 */   { 3, 0x04 },   /* bit 30 */   { 3, 0x02 },   /* bit 31 */   { 3, 0x01 },   /* bit 32 */
   { 4, 0x80 },   /* bit 33 */   { 4, 0x40 },   /* bit 34 */   { 4, 0x20 },   /* bit 35 */   { 4, 0x10 },   /* bit 36 */
   { 4, 0x08 },   /* bit 37 */   { 4, 0x04 },   /* bit 38 */   { 4, 0x02 },   /* bit 39 */   { 4, 0x01 },   /* bit 40 */
   { 5, 0x80 },   /* bit 41 */   { 5, 0x40 },   /* bit 42 */   { 5, 0x20 },   /* bit 43 */   { 5, 0x10 },   /* bit 44 */
   { 5, 0x08 },   /* bit 45 */   { 5, 0x04 },   /* bit 46 */   { 5, 0x02 },   /* bit 47 */   { 5, 0x01 },   /* bit 48 */
   { 6, 0x80 },   /* bit 49 */   { 6, 0x40 },   /* bit 50 */   { 6, 0x20 },   /* bit 51 */   { 6, 0x10 },   /* bit 52 */
   { 6, 0x08 },   /* bit 53 */   { 6, 0x04 },   /* bit 54 */   { 6, 0x02 },   /* bit 55 */   { 6, 0x01 },   /* bit 56 */
   { 7, 0x80 },   /* bit 57 */   { 7, 0x40 },   /* bit 58 */   { 7, 0x20 },   /* bit 59 */   { 7, 0x10 },   /* bit 60 */
   { 7, 0x08 },   /* bit 61 */   { 7, 0x04 },   /* bit 62 */   { 7, 0x02 },   /* bit 63 */   { 7, 0x01 },    /* bit 64 */

   { 8, 0x80 },    /* bit 65 */   { 8, 0x40 },    /* bit 66 */   { 8, 0x20 },    /* bit 67 */   { 8, 0x10 },    /* bit 68 */
   { 8, 0x08 },    /* bit 69 */   { 8, 0x04 },    /* bit 70 */   { 8, 0x02 },    /* bit 71 */   { 8, 0x01 },    /* bit 72 */
   { 9, 0x80 },    /* bit 73 */   { 9, 0x40 },    /* bit 74 */   { 9, 0x20 },    /* bit 75 */   { 9, 0x10 },    /* bit 76 */
   { 9, 0x08 },    /* bit 77 */   { 9, 0x04 },    /* bit 78 */   { 9, 0x02 },    /* bit 79 */   { 9, 0x01 },    /* bit 80 */
   { 10, 0x80 },   /* bit 81 */   { 10, 0x40 },   /* bit 82 */   { 10, 0x20 },   /* bit 83 */   { 10, 0x10 },   /* bit 84 */
   { 10, 0x08 },   /* bit 85 */   { 10, 0x04 },   /* bit 86 */   { 10, 0x02 },   /* bit 87 */   { 10, 0x01 },   /* bit 88 */
   { 11, 0x80 },   /* bit 89 */   { 11, 0x40 },   /* bit 90 */   { 11, 0x20 },   /* bit 91 */   { 11, 0x10 },   /* bit 92 */
   { 11, 0x08 },   /* bit 93 */   { 11, 0x04 },   /* bit 94 */   { 11, 0x02 },   /* bit 95 */   { 11, 0x01 },   /* bit 96 */
   { 12, 0x80 },   /* bit 97 */   { 12, 0x40 },   /* bit 98 */   { 12, 0x20 },   /* bit 99 */   { 12, 0x10 },   /* bit100 */
   { 12, 0x08 },   /* bit 37 */   { 12, 0x04 },   /* bit 38 */   { 12, 0x02 },   /* bit 39 */   { 12, 0x01 },   /* bit104 */
   { 13, 0x80 },   /* bit 41 */   { 13, 0x40 },   /* bit 42 */   { 13, 0x20 },   /* bit 43 */   { 13, 0x10 },   /* bit108 */
   { 13, 0x08 },   /* bit 45 */   { 13, 0x04 },   /* bit 46 */   { 13, 0x02 },   /* bit 47 */   { 13, 0x01 },   /* bit112 */
   { 14, 0x80 },   /* bit 49 */   { 14, 0x40 },   /* bit 50 */   { 14, 0x20 },   /* bit 51 */   { 14, 0x10 },   /* bit116 */
   { 14, 0x08 },   /* bit 53 */   { 14, 0x04 },   /* bit 54 */   { 14, 0x02 },   /* bit 55 */   { 14, 0x01 },   /* bit120 */
   { 15, 0x80 },   /* bit 57 */   { 15, 0x40 },   /* bit 58 */   { 15, 0x20 },   /* bit 59 */   { 15, 0x10 },   /* bit124 */
   { 15, 0x08 },   /* bit 61 */   { 15, 0x04 },   /* bit 62 */   { 15, 0x02 },   /* bit 63 */   { 15, 0x01 }   /* bit 130*/
   };


  //farad 8/13/1998 the last modifications.

/************************************************
  Bit Field Data Record
*************************************************/
struct bitfield_data 
   {
   enum bitfields  bd_fieldid;                           /* id for this field */
   pBYTE           p_bd_pdata;                           /* ptr to data to move */
   INT             bd_fieldlen;                          /* # of bytes in iso8583 field */
   INT             (*bd_BFCond)(enum bitfields, BYTE);  /* ptr to conditional function */
   void            (*bd_BFMove)(enum bitfields);         /* ptr to move function */
   void            (*bd_BFRsp)(enum bitfields);          /* ptr to response handler function */
   };


//struct bitfield_data {
  // enum bitfields  bd_fieldid;                           /* id for this field */
  // unsigned char   *bd_pdata;                            /* ptr to data to move */
  // int             bd_fieldlen;                          /* # of bytes in visa iso field */
  // int             (*bd_BFCond)(enum bitfields, char *); /* ptr to conditional function */
  // void            (*bd_BFMove)(enum bitfields);         /* ptr to move function */
  // void            (*bd_BFRsp)(enum bitfields);          /* ptr to response handler function */
  // };

/**********************************
      FUNCTION PROTOTYPES
**********************************/
/* functions local */
#ifdef USE_ORIGINAL
INT CondProc(enum bitfields, BYTE);
INT CondExpd(enum bitfields, BYTE);
INT CondPan(enum bitfields, BYTE);
INT CondTrack1(enum bitfields, BYTE);
INT Move_Always(enum bitfields, BYTE);
INT Move_IfThere(enum bitfields, BYTE);
INT vMove_IfThere(enum bitfields, BYTE);
INT Move23_IfThere(enum bitfields, BYTE);
INT Move_Never(enum bitfields, BYTE);
void EBCDICMove(enum bitfields);
void vEBCDICMove(enum bitfields);
void RspEBCDICvMove(enum bitfields);
void RspEBCDICMove(enum bitfields);
void RspIgnore(enum bitfields);
void vRspIgnore(enum bitfields);
void Rj_with_lead_zeros( pCHAR str, INT FieldLen );

//farad 8/13/98 
void asc_to_vbcd (pCHAR, INT, BYTE [] );
void asc_to_bcd (pCHAR, INT, BYTE [] );
void str_asc_to_bcd(pCHAR, INT, BYTE [] ); 
void vBCDMove(enum bitfields fieldid);
void RspMovevBCD (enum bitfields fieldid);
void GenericMove(enum bitfields fieldid);
void RspMoveGeneric(enum bitfields fieldid);
void vDataMove(enum bitfields fieldid);
void RspMovevData (enum bitfields fieldid);

void find_cvc_flag( pCHAR, pCHAR, pCHAR);

void Field62Response(enum bitfields);

#else
	static INT CondProc(enum bitfields, BYTE);
	static INT CondExpd(enum bitfields, BYTE);
	static INT CondPan(enum bitfields, BYTE);
	static INT CondTrack1(enum bitfields, BYTE);
	static INT Move_Always(enum bitfields, BYTE);
	static INT Move_IfThere(enum bitfields, BYTE);
	static INT vMove_IfThere(enum bitfields, BYTE);
	static INT Move23_IfThere(enum bitfields, BYTE);
	static INT Move_Never(enum bitfields, BYTE);
	static void EBCDICMove(enum bitfields);
	static void vEBCDICMove(enum bitfields);
	static void RspEBCDICvMove(enum bitfields);
	static void RspEBCDICMove(enum bitfields);
	static void RspIgnore(enum bitfields);
	static void vRspIgnore(enum bitfields);
	void Rj_with_lead_zeros( pCHAR str, INT FieldLen );

	//farad 8/13/98 
	static void asc_to_vbcd (pCHAR, INT, BYTE [] );
	static void asc_to_bcd (pCHAR, INT, BYTE [] );
	static void str_asc_to_bcd(pCHAR, INT, BYTE [] ); 
	static void vBCDMove(enum bitfields fieldid);
	static void RspMovevBCD (enum bitfields fieldid);
	static void GenericMove(enum bitfields fieldid);
	static void RspMoveGeneric(enum bitfields fieldid);
	static void vDataMove(enum bitfields fieldid);
	static void RspMovevData (enum bitfields fieldid);

	void find_cvc_flag( pCHAR, pCHAR, pCHAR);

	static void Field62Response(enum bitfields);

#endif

//The next struct was modified on 8/13/98 by farad.

static  struct bitfield_data BFData[] = {
   {
   bf_2pan,                      /* field */
   HOST_MESSAGE.pan_len,         /* data to move */
   1,                            /* # of bytes in pan len field */
   Move_Always,                  /* mandatory    */
   vBCDMove,                     /* move routine */
   RspMovevBCD                   /* response routine */
   },
   {
   bf_3proc,                     /* field */
   HOST_MESSAGE.proccode,        /* data to move */
   3,                            /* # of bytes in field */
   Move_Always,                  /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_4amount,                   /* field */
   HOST_MESSAGE.amount_bin,      /* data to move */
   6,                            /* # of bytes in  field */
   Move_Always,                  /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_5amount,                   /* field */
   HOST_MESSAGE.settle_amt,      /* data to move */
   6,                            /* # of bytes in  field */
   Move_IfThere,                  /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_6crdbillamt,               /* field */
   HOST_MESSAGE.card_bill_amt,   /* data to move */
   6,                            /* # of bytes in field */
   Move_IfThere,                  /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_7trntime,                  /* field */
   HOST_MESSAGE.gmt_time,        /* data to move */
   5,                            /* # of bytes in field */
   Move_Always,                  /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_9convrate,                /* field */
   HOST_MESSAGE.conv_rate_stl,       /* data to move */
   4,                            /* # of bytes in field */
   Move_IfThere,                  /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_10convrate,                /* field */
   HOST_MESSAGE.conv_rate,       /* data to move */
   4,                            /* # of bytes in field */
   Move_IfThere,                  /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_11stan,                    /* field */
   HOST_MESSAGE.stan_bin,        /* data to move */
   3,                            /* # of bytes in field */
   Move_Always,                  /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_12time,                    /* field */
   HOST_MESSAGE.time_bin,        /* data to move */
   3,                            /* # of bytes in field */
   Move_Always,                 /* Mandatory */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_13date,                    /* field */
   HOST_MESSAGE.date_bin,         /* data to move */
   2,                            /* # of bytes in field */
   Move_Always,                 /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_14expiry,                  /* field */
   HOST_MESSAGE.date_expd,       /* data to move */
   2,                            /* # of bytes in field */
   Move_IfThere,                     /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_15datestl,                 /* field */
   HOST_MESSAGE.date_settle,     /* data to move */
   2,                            /* # of bytes in field */
   Move_IfThere,                 /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_16convdate,                 /* field */
   HOST_MESSAGE.conv_date,       /* data to move */
   2,                            /* # of bytes in field */
   Move_IfThere,                 /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_18sicnumber,               /* field */
   HOST_MESSAGE.sic_number,      /* data to move */
   2,                            /* # of bytes in field */
   Move_Always,                  /* mandatory    */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_19ctrycd,                  /* field */
   HOST_MESSAGE.ctry_code,       /* data to move */
   2,                            /* # of bytes in field */
   Move_Always,                  /* mandatory    */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_20panctrycd,                    /* field */
   HOST_MESSAGE.pan_ctry_cd,     /* data to move */
   2,                            /* # of bytes in field */
   Move_IfThere,                 /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_22pose,                    /* field */
   HOST_MESSAGE.posentry,         /* data to move */
   2,                            /* # of bytes in field */
   Move_Always,                  /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_23csn ,                    /* field */
   HOST_MESSAGE.card_seq_num,    /* data to move */
   2,                            /* # of bytes in visa iso field */
   Move23_IfThere,               /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_25pocc,                    /* field */
   HOST_MESSAGE.pos_condcd,      /* data to move */
   1,                            /* # of bytes in field */
   Move_Always,                  /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_26pincc,                 /* field */
   HOST_MESSAGE.pin_captcd,      /* data to move */
   1,                            /* # of bytes in field */
   Move_IfThere,                 /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_28feeamount,				 /* field */
   HOST_MESSAGE.tran_fee_amount,	     /* data to move */
   9,							 /* # of bytes in visa iso field */
   Move_IfThere,                  /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_32idcode,                  /* field */
   HOST_MESSAGE.acq_inst_len,    /* data to move */
   1,                            /* # of bytes in field */
   Move_Always,                  /* conditional */
   vBCDMove,                     /* move routine */
   RspMovevBCD                   /* response routine */
   },
   {
   bf_33fwdidcode,                    /* field */
   HOST_MESSAGE.fwd_inst_len,    /* data to move */
   1,                            /* # of bytes in field */
   Move_Always,                 /* conditional */
   vBCDMove,                     /* move routine */
   RspMovevBCD                   /* response routine */
   },
   {
   bf_34panext,                    /* field */
   HOST_MESSAGE.pan_ext_len,    /* data to move */
   1,                            /* # of bytes in field */
   Move_IfThere,                 /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_35track2,                  /* field */
   HOST_MESSAGE.track2_len,      /* data to move */
   1,                            /* # of bytes in len field */
   vMove_IfThere,                 /* conditional */
   vBCDMove,                     /* move routine */
   RspMovevBCD                   /* response routine */
   },
   {
   bf_37rrn,                     /* field */
   HOST_MESSAGE.rrn,             /* data to move */
   12,                           /* # of bytes in field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_38authcode,                /* field */
   HOST_MESSAGE.authcode,        /* data to move */
   6,                            /* # of bytes in field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_39respcode,                /* field */
   HOST_MESSAGE.responsecd,      /* data to move */
   2,                            /* # of bytes in field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_40src,                     /* field */
   HOST_MESSAGE.src,             /* data to move */
   3,                            /* # of bytes in field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_41termid,                  /* field */
   HOST_MESSAGE.termid,          /* data to move */
   8,                            /* # of bytes in field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_42cardacc,                 /* field */
   HOST_MESSAGE.cardaccid,       /* data to move */
   15,                           /* # of bytes in field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_43cardaccname,             /* field */
   HOST_MESSAGE.cardaccname,     /* data to move */
   40,                           /* # of bytes in field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_44respdata,                /* field */
   HOST_MESSAGE.addl_rsp_len,    /* data to move */
   1,                            /* # of bytes in len field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_45track1,                  /* field */
   HOST_MESSAGE.track1_len,      /* data to move */
   1,                            /* # of bytes in len field */
   CondTrack1,                   /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_48addl_data,               /* field */
   HOST_MESSAGE.addl_data_len,   /* data to move */
   1,                            /* # of bytes in len field */
   Move_IfThere,                 /* mandatory    */
   vDataMove,                    /* move routine */
   RspMovevData                  /* response routine */
   },
   {
   bf_49trncurcd,                /* field */
   HOST_MESSAGE.tran_curr_cd,    /* data to move */
   3,                            /* # of bytes in field */
   Move_Always,                  /* mandatory    */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_50sttlcurcd,                /* field */
   HOST_MESSAGE.settle_cur_cd,    /* data to move */
   3,                            /* # of bytes in field */
   Move_IfThere,                  /* conditional    */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_51crdcurcd,                /* field */
   HOST_MESSAGE.card_curr_cd,    /* data to move */
   3,                            /* # of bytes in field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                  /* move routine */
   RspEBCDICMove                /* response routine */
   },
   {
   bf_52pinblock,                /* field */
   HOST_MESSAGE.pin_block,       /* data to move */
   8,                            /* # of bytes in field */
   Move_IfThere,                 /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_53seccntl,                 /* field */
   HOST_MESSAGE.sec_cntl,        /* data to move */
   8,                            /* # of bytes in field */
   Move_IfThere,                 /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_54addlamts,                /* field */
   HOST_MESSAGE.addl_len,        /* data to move */
   1,                            /* # of bytes in len field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_55iccdata,                 /* field */
   HOST_MESSAGE.iccdata_len,     /* data to move */
   2,                            /* # of bytes in addl len field */
   Move_IfThere,                 /* conditional */
   vDataMove,                    /* move routine */
   RspMovevData                  /* response routine */
   },
   {
   bf_59posdata,                 /* field */
   HOST_MESSAGE.posdata_len,     /* data to move */
   1,                            /* # of bytes in len field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_60priv60,                  /* field */
   HOST_MESSAGE.priv60_len,      /* data to move */
   1,                            /* # of bytes in len field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_61priv61,                  /* field */
   HOST_MESSAGE.priv61_len,      /* data to move */
   1,                            /* # of bytes in priv61 len field */
   Move_Always,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_62priv62,                  /* field */
   HOST_MESSAGE.priv62_len,      /* data to move */
   1,                            /* # of bytes in priv62 len field */
   Move_IfThere,                 /* conditional */
   vDataMove,                    /* move routine */
   Field62Response               /* response routine */
   },
   {
   bf_63priv63,                  /* field */
   HOST_MESSAGE.priv63_len,      /* data to move */
   1,                            /* # of bytes in priv63 len field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_70nmicode,                 /* field */
   HOST_MESSAGE.nmi_code,        /* data to move */
   2,                            /* # of bytes in field */
   Move_Always,                  /* mandatory    */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_73action,                  /* field */
   HOST_MESSAGE.action,          /* data to move */
   3,                            /* # of bytes in field */
   Move_Always,                  /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_90orgdata,                 /* field */
   HOST_MESSAGE.orig_data,       /* data to move */
   21,                           /* # of bytes in field */
   Move_Always,                  /* mandatory    */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_91fileupdate,              /* field */
   HOST_MESSAGE.file_update,     /* data to move */
   1,                            /* # of bytes in field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_93rspind,                  /* field */
   HOST_MESSAGE.resp_ind,        /* data to move */
   5,                            /* # of bytes in field */
   Move_IfThere,                 /* conditional  */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_95replamts,                /* field */
   HOST_MESSAGE.repl_amts,       /* data to move */
   42,                           /* # of bytes in field */
   Move_IfThere,                 /* conditional  */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_96msgsec,                  /* field */
   HOST_MESSAGE.msgsec,          /* data to move */
   8,                            /* # of bytes in field */
   Move_IfThere,                 /* conditional  */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_99stlinstid,               /* field */
   HOST_MESSAGE.setl_inst_len,   /* data to move */
   1,                            /* # of bytes in field */
   Move_IfThere,                 /* mandatory    */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_100rcvid,                  /* field */
   HOST_MESSAGE.recv_inst_len,   /* data to move */
   1,                            /* # of bytes in field */
   Move_IfThere,                 /* conditional */
   vBCDMove,                     /* move routine */
   RspMovevBCD                   /* response routine */
   },
   {
   bf_101fname,                  /* field */
   HOST_MESSAGE.fname_len,       /* data to move */
   1,                            /* # of bytes in field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_102acct1,                  /* field */
   HOST_MESSAGE.acct_id_1_len,   /* data to move */
   1,                            /* # of bytes in field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_103acct2,                  /* field */
   HOST_MESSAGE.acct_id_2_len,   /* data to move */
   1,                            /* # of bytes in field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_104trandesc,               /* field */
   HOST_MESSAGE.trandesc_len,    /* data to move */
   1,                            /* # of bytes in field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_118intractry,              /* field */
   HOST_MESSAGE.intra_ctry_len,  /* data to move */
   1,                            /* # of bytes in field */
   Move_Never,                   /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_120record,                 /* field */
   HOST_MESSAGE.record_len,          /* data to move */
   2,                            /* 2 byte length field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_121issid,                  /* field */
   HOST_MESSAGE.iss_inst_len,    /* data to move */
   1,                            /* # of bytes in field */
   Move_IfThere,                   /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_122openuse,                /* field */
   HOST_MESSAGE.open_use_len,    /* data to move */
   1,                            /* # of bytes in field */
   Move_IfThere,                 /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_123addverf,                /* field */
   HOST_MESSAGE.addr_verf_len,   /* data to move */
   1,                            /* # of bytes in field */
   Move_IfThere,                 /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },

   {
   bf_124freeform,               /* field */
   HOST_MESSAGE.free_text_len,   /* data to move */
   1,                            /* # of bytes in field */
   Move_IfThere,                 /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },

   {
   bf_127accesscode,             /* field */
   HOST_MESSAGE.access_cd_len,   /* data to move */
   2,                            /* # of bytes in field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },

   {
   bf_done,                      /* field */
   NULL,                         /* data to move */
   99,                           /* # of bytes in field */
   NULL,                         /* conditional */
   NULL,                         /* move routine */
   NULL                          /* response routine */
   }
   }; /* end BFData */


#define N_BFData ( sizeof(BFData) / sizeof (struct bitfield_data) )
          

/*******************************************************************************************************/
/* ROUTINES                                                                                            */
/*******************************************************************************************************/
#ifdef USE_ORIGINAL

struct bitfield_data *Find_Bitfield_Table (enum bitfields);
struct trandef       *Find_Tran_Table     (BYTE trantype);
#else
	static struct bitfield_data *Find_Bitfield_Table (enum bitfields);
	static struct trandef       *Find_Tran_Table     (BYTE trantype);
#endif 


/*****************************************************************************

  Function:    GenericMove

  Description: This function is the generic move function that will move
               data from the HOST_MESSAGE structure to the the raw ISO message.
  Author: 
      unknown
  Inputs:
      fieldid                - the bit field of the data to be moved
      HOST_MESSAGE (global)  - structure that contains data to go into the 
                               outgoing ISO message
  Outputs:
      ReqRsp_Buffer (global) - the buffer that contains the ISO formatted message
  Return values:
      None
  Modified by:
******************************************************************************/
static  void GenericMove(enum bitfields fieldid)
{
   struct bitfield_data *pBF;


   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
        /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   if (pBF != NULL)
   {
      memcpy(&ReqRsp_Buffer[reqidx],      /* move to request buffer */
             pBF->p_bd_pdata,         /* from data pointer      */
             pBF->bd_fieldlen);      /* for length of field    */

      reqidx = reqidx + pBF->bd_fieldlen;   /* increment buffer pointer */
   }
} /* end GenericMove */

/*****************************************************************************

  Function:    bcd_to_asc

  Description: This function will convert bcd string to an ascii string 
  Author: 
      Farad       8/13/98
  Inputs:
      bstring  - the bcd string to conver to ascii
      blen     - the length of bstring
  Outputs:
      astr     - the new ascii string
      astr_len - the length of astr
  Return values:
      None
  Modified by:
******************************************************************************/
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

/*****************************************************************************

  Function:    RspMoveGeneric

  Description: This function is the generic move function that will move
               data from the raw ISO message to the HOST_MESSAGE structure.
  Author: 
      Farad       8/13/98
  Inputs:
      ReqRsp_Buffer (global) - the buffer that contains the ISO formatted message
      fieldid                - the bit field of the data to be moved
  Outputs:
      HOST_MESSAGE (global)  - structure that contains data from the incominging ISO message
  Return values:
      None
  Modified by:
******************************************************************************/
static  void RspMoveGeneric(enum bitfields fieldid)
{
   struct bitfield_data *pBF;


   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
        /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   if (pBF != NULL)
   {
      if((reqidx + pBF->bd_fieldlen) <= buflen)
      {
         memcpy(pBF->p_bd_pdata,         /* to data pointer      */
                &ReqRsp_Buffer[reqidx],      /* move from request buffer */
                pBF->bd_fieldlen);      /* for length of field    */

         reqidx = reqidx + pBF->bd_fieldlen;   /* increment buffer pointer */
      }
      else
         invalid_msg_format = true;
   }
} /* end RspMoveGeneric */

/*****************************************************************************

  Function:    vEBCDICMove

  Description: This function is the  move function that will move data
               from the HOST_MESSAGE structure (after converting it to 
               EBCDIC) to the the raw ISO message.  Note: it moves the 
               number indicated by the length variable pointed to by 
               pBF->p_db_data[0].
  Author: 
      unknown
  Inputs:
      fieldid                - the bit field of the data to be moved
      HOST_MESSAGE (global)  - structure that contains data to go into the 
                               outgoing ISO message
  Outputs:
      ReqRsp_Buffer (global) - the buffer that contains the ISO formatted message
  Return values:
      None
  Modified by:
******************************************************************************/
static  void vEBCDICMove( enum bitfields fieldid )
{
   struct bitfield_data *pBF;
   INT                  movelen;
   INT                  i;
   INT                  j;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   if (pBF != NULL)
   {
      /* Fields 120 and 127 have 2 byte lengths.
       * But neither of them use the first byte.
       */
      if ( (fieldid == 120) || (fieldid == 127) )
      {
         j = 1;
         ReqRsp_Buffer[reqidx++] = pBF->p_bd_pdata[0];
         ReqRsp_Buffer[reqidx++] = pBF->p_bd_pdata[1];
      }
      else
      {
         j = 0;
         ReqRsp_Buffer[reqidx++] = pBF->p_bd_pdata[0];
      }

      movelen = pBF->p_bd_pdata[j];

      for( i=1; i<=movelen; i++ )
      {
         ReqRsp_Buffer[reqidx] = ascii_ebcdic_conv (pBF->p_bd_pdata[i+j], A2E);
         reqidx++;
      }
   }
} /* end vEBCDICMove */

/*****************************************************************************

  Function:    EBCDICMove

  Description: This function is the  move function that will move data
               from the HOST_MESSAGE structure (after converting it to 
               EBCDIC) to the the raw ISO message.  Note: it moves the 
               number indicated by the length variable pointed to by 
               pBF->bd_fieldlen.
  Author: 
      unknown
  Inputs:
      fieldid                - the bit field of the data to be moved
      HOST_MESSAGE (global)  - structure that contains data to go into the 
                               outgoing ISO message
  Outputs:
      ReqRsp_Buffer (global) - the buffer that contains the ISO formatted message
  Return values:
      None
  Modified by:
******************************************************************************/
static  void EBCDICMove( enum bitfields fieldid )
{
   struct bitfield_data *pBF;
   INT                  movelen;
   INT                  i;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   if (pBF != NULL)
   {
      movelen = pBF->bd_fieldlen;

      for( i=0; i<movelen; i++ )
      {
         ReqRsp_Buffer[reqidx] = ascii_ebcdic_conv (pBF->p_bd_pdata[i], A2E);
         reqidx++;
      }
   }
} /* end EBCDICMove */

/*****************************************************************************

  Function:    RspEBCDICvMove

  Description: This function will move data from the the raw ISO message 
               (after converting it from EBCDIC) to the HOST_MESSAGE structure.  
               Note: it moves the number indicated by the length variable pointed 
               to by pBF->p_db_data[0].
  Author: 
      unknown
  Inputs:
      fieldid                - the bit field of the data to be moved
      ReqRsp_Buffer (global) - the buffer that contains the ISO formatted message
  Outputs:
      HOST_MESSAGE (global)  - structure that contains data to go into the 
                               outgoing ISO message
  Return values:
      None
  Modified by:
******************************************************************************/
static  void RspEBCDICvMove (enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  i;
   INT                  j;
   INT                  movelen;

   /*---------------------------------------------------*/
   /* Find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   /* Determine number of bytes to move. */
   if (pBF != NULL)
   {
      /* Fields 120 and 127 have 2 byte lengths.
       * But neither field uses the first byte.
       */
      if ( (fieldid == 120) || (fieldid == 127) )
      {
         j = 1;
         pBF->p_bd_pdata[0] = ReqRsp_Buffer[reqidx++];
         pBF->p_bd_pdata[1] = ReqRsp_Buffer[reqidx];
      }
      else
      {
         j = 0;
         pBF->p_bd_pdata[0] = ReqRsp_Buffer[reqidx];
      }
      movelen = ReqRsp_Buffer[reqidx++];

      if ((reqidx+movelen) <= buflen)
      {
         /* Move the data, converting it from EBCDIC to ASCII. */
         for( i=1; i<=movelen; i++)
         {
            pBF->p_bd_pdata [i+j] = ascii_ebcdic_conv (ReqRsp_Buffer[reqidx], E2A);
            reqidx++;
         }
      }
      else
         invalid_msg_format = true;
   }
} /* end RspEBCDICvMove */

/*****************************************************************************

  Function:    RspEBCDICMove

  Description: This function will move data from the the raw ISO message 
               (after converting it from EBCDIC) to the HOST_MESSAGE structure.  
               Note: it moves the number indicated by the length variable pointed
               to by pBF->bd_fieldlen.
  Author: 
      unknown
  Inputs:
      fieldid                - the bit field of the data to be moved
      HOST_MESSAGE (global)  - structure that contains data to go into the 
                               outgoing ISO message
  Outputs:
      ReqRsp_Buffer (global) - the buffer that contains the ISO formatted message
  Return values:
      None
  Modified by:
      BGable      6/30/98
******************************************************************************/
static  void RspEBCDICMove (enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   int                  i;
   int                  movelen;

   /*---------------------------------------------------*/
   /* Find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);
   if (pBF != NULL)
   {
      movelen = pBF->bd_fieldlen;

      if ((reqidx + movelen) <= buflen)
      {
         /* Move the data, converting it from EBCDIC to ASCII. */
         for( i=0; i<movelen; i++)
         {
            pBF->p_bd_pdata [i] = ascii_ebcdic_conv (ReqRsp_Buffer[reqidx], E2A);
            reqidx++;
         }
      }
      else
         invalid_msg_format = true;
   }
} /* end RspEBCDICMove */

/*****************************************************************************

  Function:    Rj_with_lead_zeros

  Description: This function will right justify a string and add leading zeros
  Author: 
      unknown
  Inputs:
      str        - the ascii string to be right justified 
      FieldWidth
  Outputs:
      str        - the right justified ascii string
  Return values:
      None
  Modified by:
******************************************************************************/
void Rj_with_lead_zeros( pCHAR str, INT FieldWidth )
{
   INT i;
   INT dif;
   INT strlength;

   strlength = strlen(str);

   /* Convert any leading blanks to zero. */
   for (i = 0; i < strlength; i++)
   {
      if ( 0x20 == str[i] )
         str [i] = 0x30;
      else
         break;
   }

   /* Remove any trailing blanks. */
   for (i = strlength-1; i >= 0; i--)
   {
      if ( 0x20 == str[i] )
         str [i] = 0x00;
      else
         break;
   }

   strlength = strlen(str);
   dif       = FieldWidth - strlength;

   if (dif > 0)
   {
      /* Right justify, then prepend leading zeros. */
      memmove (&str [dif], &str[0], strlength+1);
      memset  (&str[0], 0x30, dif);
   }
}

/*****************************************************************************

  Function:    RspIgnore  

  Description: This function will not copy the response field, but will increment
               the index by the length of the response field.
  Author: 
      unknown
  Inputs:
      fieldid   - the bit field of the data not to be moved
  Outputs:
      None
  Return values:
      None
  Modified by:
******************************************************************************/
static  void RspIgnore(enum bitfields fieldid)
{
   struct bitfield_data *pBF;


   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   if (pBF != NULL)
   {
      if (( reqidx + pBF->bd_fieldlen) <= buflen)
         reqidx = reqidx + pBF->bd_fieldlen;   /* increment buffer pointer */
      else
         invalid_msg_format = true;
   }
} /* end RspIgnore */

/*****************************************************************************

  Function:    RspIgnore  

  Description: This function will not copy the response field, but will increment
               the index by the variable length of the response field.
  Author: 
      unknown
  Inputs:
      fieldid   - the bit field of the data not to be moved
  Outputs:
      None
  Return values:
      None
  Modified by:
      Farad      8/13/98
******************************************************************************/
static  void vRspIgnore(enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   int                  movelen;

   /*---------------------------------------------------*/
   /* Find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   /* Determine number of bytes to move. */
   if (pBF != NULL)
   {
      movelen = ReqRsp_Buffer[reqidx] + pBF->bd_fieldlen;
      if ((reqidx + movelen) <= buflen)
         reqidx = reqidx + movelen;   /* increment buffer pointer */
      else
         invalid_msg_format = true;
   }
} /* end vRspIgnore */

/*****************************************************************************

  Function:    bcd_to_int   

  Description: This function will convert a bcd string into an integer. 
  Author: 
      unknown
  Inputs:
      bcd_data  - a pointer to the bcd string to be converted
      bcd_len   - the length of the string pointed to by bcd_data
  Outputs:
      None
  Return values:
      INT - the integer created by converting bcd_data to an integer.
  Modified by:
******************************************************************************/
static  INT bcd_to_int ( BYTE *bcd_data,
                 int           bcd_len    )
{
   INT   temp_int;
   INT   multiplier;
   INT   i;


   multiplier = 1;
   temp_int   = 0;
   
   for (i = (bcd_len-1); i >= 0; i--)
      {
      temp_int+= (bcd_data [i] & 0x0f) * multiplier;
      multiplier = multiplier * 10;
      temp_int+= (bcd_data [i] / 16) * multiplier;
      multiplier = multiplier * 10;
      }

   return (temp_int);
}

/*****************************************************************************

  Function:    vBCDMove 

  Description: This function will move data from the HOST_MESSAGE structure to 
               the the raw ISO message.  Note: it moves the number indicated 
               by the length variable pointed to by pBF->p_db_data[0].
  Author: 
      unknown
  Inputs:
      fieldid                - the bit field of the data to be moved
      HOST_MESSAGE (global)  - structure that contains data to go into the 
                               outgoing ISO message
  Outputs:
      ReqRsp_Buffer (global) - the buffer that contains the ISO formatted message
  Return values:
      None
  Modified by:
      BGable      6/30/98
******************************************************************************/
static  void vBCDMove(enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  movelen;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   /* Determine number of bytes to move. */
   if (pBF != NULL)
   {
      movelen = pBF->p_bd_pdata[0];
      ReqRsp_Buffer[reqidx] = pBF->p_bd_pdata[0];
      reqidx++;

      //if ( 60 == fieldid )
      //   movelen *= 2;

      if ((movelen % 2) != 0)
         movelen++;

      movelen = (movelen / 2);

      memcpy(&ReqRsp_Buffer[reqidx], &pBF->p_bd_pdata[1], movelen);

      reqidx = reqidx + movelen;   /* increment buffer pointer */
   }
} /* end vBCDMove */

/*****************************************************************************

  Function:    

  Description: This function will move data from the raw ISO message to 
               the HOST_MESSAGE structure.  
  Author: 
      unknown
  Inputs:
      fieldid                - the bit field of the data to be moved
      ReqRsp_Buffer (global) - the buffer that contains the ISO formatted message
  Outputs:
      HOST_MESSAGE (global)  - structure for the outgoing ISO message
  Return values:
      None
  Modified by:
      BGable      6/30/98
******************************************************************************/
static  void RspMovevBCD (enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  movelen;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   if (pBF != NULL)
   {
      movelen = ReqRsp_Buffer[reqidx];
      pBF->p_bd_pdata[0] = ReqRsp_Buffer[reqidx];
      reqidx++;

      //if ( 60 == fieldid )
      //   movelen *= 2;

      if ((movelen % 2) != 0)
         movelen++;

      movelen/= 2;
      if ((reqidx + movelen) <= buflen)
      {
         memcpy(&pBF->p_bd_pdata[1], &ReqRsp_Buffer[reqidx], movelen);

         reqidx = reqidx + movelen;   /* increment buffer pointer */
      }
      else
         invalid_msg_format = true;
   }
} /* end RspMovevBCD */

/*****************************************************************************

  Function:    vDataMove

  Description: This function will move data from the HOST_MESSAGE structure to 
               the the raw ISO message.  Note: it moves the number indicated 
               by the length variable pointed to by pBF->p_db_data[0].
  Author: 
      unknown
  Inputs:
      fieldid                - the bit field of the data to be moved
      HOST_MESSAGE (global)  - structure that contains data to go into the 
                               outgoing ISO message
  Outputs:
      ReqRsp_Buffer (global) - the buffer that contains the ISO formatted message
  Return values:
      None
  Modified by:
******************************************************************************/
static  void vDataMove(enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  movelen;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   if (pBF != NULL)
   {
      if ( fieldid == 55 )
      {
         movelen = pBF->p_bd_pdata [1];

         ReqRsp_Buffer [reqidx+1] = pBF->p_bd_pdata [1];
         reqidx += 2;

         memcpy(&ReqRsp_Buffer[reqidx], &pBF->p_bd_pdata[2], movelen);
      }
      else
      {
         movelen = pBF->p_bd_pdata [0];

         ReqRsp_Buffer [reqidx] = pBF->p_bd_pdata [0];
         reqidx++;

         memcpy(&ReqRsp_Buffer[reqidx], &pBF->p_bd_pdata[1], movelen);
      }
      reqidx = reqidx + movelen;   /* increment buffer pointer */
   }
} /* end vDataMove */

/*****************************************************************************

  Function:    RspMovevData  

  Description: This function will move data from the the raw ISO message 
               to the HOST_MESSAGE structure.  Note: it moves the number 
               indicated by the length variable pointed to by pBF->p_db_data[0].
  Author: 
      unknown
  Inputs:
      fieldid                - the bit field of the data to be moved
      ReqRsp_Buffer (global) - the buffer that contains the ISO formatted message
  Outputs:
      HOST_MESSAGE (global)  - structure that contains data to go into the 
                               outgoing ISO message
  Return values:
      None
  Modified by:
******************************************************************************/
static  void RspMovevData (enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  movelen;
   INT                  j = 0;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   if (pBF != NULL)
   {
      /* Field 55 is a 2-byte length field.  First byte is NULL. */
      if ( fieldid == 55 )
      {
         reqidx++;
         j = 1;
      }

      movelen = ReqRsp_Buffer [reqidx];
      pBF->p_bd_pdata[j] = ReqRsp_Buffer [reqidx];
      reqidx++;
      if ((reqidx + movelen) <= buflen)
      {
         memcpy(&pBF->p_bd_pdata[1+j], &ReqRsp_Buffer[reqidx], movelen);

         reqidx = reqidx + movelen;   /* increment buffer pointer */
      }
      else
         invalid_msg_format = true;
   }
} /* end RspMovevData */

/*****************************************************************************

  Function:    CondPan   

  Description: This function will move the account number if it is there.
  Author: 
      unknown
  Inputs:
      bitfield   - the bit field of the account number 
      p_trantype - pointer to the tran type
  Outputs:
      None
  Return values:
      true  - account number moved
      false - account number not moved
  Modified by:
      BGable      6/30/98
******************************************************************************/
static  INT CondPan(enum bitfields bitfield, BYTE p_trantype)
{
/* JH Apr 29  changed procedure to only send the pan if it is there */
   if (HOST_MESSAGE.pan_len [0] != 0x00)   /* JH Apr 29  Fixed bug */
       return(Move_IfThere(bitfield, p_trantype));
  
return(false);
} /* end CondPan */

/*****************************************************************************

  Function:    CondExpd

  Description: This function will move the expiration dat if it is there.
  Author: 
      unknown
  Inputs:
      bitfield   - the bit field of the account number 
      p_trantype - pointer to the tran type
  Outputs:
      None
  Return values:
      true  - expiration date moved
      false - expiration date not moved
  Modified by:
      BGable      6/30/98
******************************************************************************/
static  INT CondExpd(enum bitfields bitfield, BYTE p_trantype)
{
        /* if track2 data here, no need for expdate field */
   if (HOST_MESSAGE.date_expd [1] != 0x00)    /* JH Apr 29  Fixed bug */
       return(true);
  
return(false);
} /* end CondExpd */

/*****************************************************************************

  Function:    CondProc

  Description: This function will determine whether the message type is not 
               a reversal. 
  Author: 
      unknown
  Inputs:
      bitfield - the bit field of the account number 
      trantype - tran type
  Outputs:
      None
  Return values:
      true  - not reversal
      false - reversal
  Modified by:
      BGable      6/30/98
      DI - 04/14/99
******************************************************************************/
static  INT CondProc(enum bitfields bitfield, BYTE trantype)
{
   if (HOST_MESSAGE.msgtype_bcd [1] != '4')
      return (true);

   return(false);
} /* end CondProc */

/*****************************************************************************

  Function:    CondTrack1

  Description: This function will determine whether or not there is a track1. 
  Author: 
      unknown
  Inputs:
      bitfield   - the bit field of track1
      p_trantype - a pointer to tran type
  Outputs:
      None
  Return values:
      true  - track1 
      false - no track1
  Return values:
      None
  Modified by:
******************************************************************************/
static  INT CondTrack1 (enum bitfields bitfield, BYTE p_trantype)
{
   if (HOST_MESSAGE.track1_len [0] != 0x00)   /* JH Apr 29  Fixed bug */
       return(true);

return(false);
} /* end CondTrack1 */

/*****************************************************************************

  Function:    Move_IfThere

  Description: This function will determine if the data for fieldid is there.
  Author: 
      unknown
  Inputs:
      fieldid    - the field id of the data to be moved
      p_trantype - a pointer to tran type
  Outputs:
      None
  Return values:
      true  - field is there 
      false - field not there
  Modified by:
******************************************************************************/
static  INT Move_IfThere(enum bitfields fieldid, BYTE p_trantype)
{
   struct bitfield_data *pBF;
        INT i;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   /*---------------------------------------------------*/
   /* in any byte non-zero, it's there, return true     */
   /*---------------------------------------------------*/
   if (pBF != NULL)
   {
      for (i=0; i < pBF->bd_fieldlen; i++)
      {
         if ( (pBF->p_bd_pdata [i] != 0) && (pBF->p_bd_pdata [i] != ' ') ) 
            return(true);
      }
   }
   return(false);
} /* end Move_IfThere */

/*****************************************************************************

  Function:    vMove_IfThere 
  Description: This function will determine if the data for fieldid is there for
               fields with variable lengths.
  Author: 
      unknown
  Inputs:
      fieldid    - the field id of the data to be moved
      p_trantype - a pointer to tran type
  Outputs:
      None
  Return values:
      true  - field is there 
      false - field not there
  Modified by:
******************************************************************************/
static  INT vMove_IfThere(enum bitfields fieldid, BYTE p_trantype)
{
   struct bitfield_data *pBF;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   /* if length byte is non-zero */
   if (pBF != NULL)
   {
      if (pBF->p_bd_pdata [0] != 0)
         return(true);
   }
   return(false);
} /* end Move_IfThere */

/*****************************************************************************

  Function:    Move23_IfThere

  Description: This function will determine if field 23 is present
  Author: 
      unknown
  Inputs:
      fieldid    - the field id of the data to be moved
      p_trantype - a pointer to tran type
  Outputs:
      None
  Return values:
      true  - field is  there 
      false - field not there
******************************************************************************/
static INT Move23_IfThere( enum bitfields fieldid, BYTE p_trantype )
{
   struct bitfield_data *pBF;
   INT    retval = false;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   /*---------------------------------------------------*/
   /* in any byte non-zero, it's there, return true     */
   /*---------------------------------------------------*/
   if (pBF != NULL)
   {
      retval = de23_present;
      de23_present = false;
   }
   return( retval );
} /* end Move23_IfThere */


/*****************************************************************************

  Function:    Move_Always

  Description: This function will cause data to be move always by always 
               returning true.
  Author: 
      unknown
  Inputs:
      fieldid    - the field id of the data to be moved
      p_trantype - a pointer to tran type
  Outputs:
      None
  Return values:
      true  - field should be moved
  Modified by:
******************************************************************************/
static  INT Move_Always(enum bitfields bitfield, BYTE p_trantype)
{
   return(true);
} /* end Move_Always */

/*****************************************************************************

  Function:    Move_Never

  Description: This function will cause data to never be moved by always 
               returning false.
  Author: 
      unknown
  Inputs:
      fieldid    - the field id of the data to be moved
      p_trantype - a pointer to tran type
  Outputs:
      None
  Return values:
      false  - field should not be moved
  Modified by:
******************************************************************************/
static  INT Move_Never(enum bitfields bitfield, BYTE p_trantype)
{
   return(false);
} /* end Move_Never */

/*****************************************************************************

  Function:    Find_Tran_Table

  Description: This function will determine whether the given trantype is in
               the tran table.  If it is, it will return a pointer to the data
               for that trantype.
  Author: 
      unknown
  Inputs:
      trantype - the transaction type
  Outputs:
      None
  Return values:
      pointer to the trandef for the trantype
  Modified by:
******************************************************************************/
static  struct trandef *Find_Tran_Table (BYTE trantype)
{
   struct trandef *ptrandef = NULL;   /* point to return record */
   int      i;                        /* loop index */

   
   for ( i=0; (i <= N_TRANENTRY) && (TRANTABLE[i].td_trantype > 0); 
         i++)
   {
      if ( TRANTABLE[i].td_trantype == trantype )
      {
         ptrandef = &TRANTABLE[i];
         return(ptrandef);
         break;
      }
   }
   if (NULL == ptrandef)
   {
      ncjcb2_log_message( 3, 2, "This trantype not found in TRANTABLE" , "Find_Tran_Table" , 0 ,"");
      return(ptrandef);
   }
   return(NULL);

} /* Find_Tran_Table */   

/*****************************************************************************

  Function:    Find_Bitfield_Table

  Description: This function will determine whether the bitfield is in the
               bitfield table.
  Author: 
      unknown
  Inputs:
      bitfield - the bitfield to search for
  Outputs:
      None
  Return values:
      pointer to the bitfield data for the bitfield entered.
  Modified by:
******************************************************************************/
static  struct bitfield_data *Find_Bitfield_Table (enum bitfields bitfield)
{
   struct bitfield_data *pBF = NULL;      /* point to return record       */
   INT      i;            /* loop index         */
   CHAR     temp_str[50] = "";

   for (i=0; i <= N_BFData; i++)
      if ( BFData[i].bd_fieldid == bitfield)
      {
         pBF = &BFData[i];
         return (pBF);
         break;
      }
 
   if (NULL == pBF)
   {
      sprintf(temp_str, "Bitfield: %d not found in BFData", bitfield);
      ncjcb2_log_message( 3, 2, temp_str , "Find_Bitfield_Table" , 0 ,"");
   }
   return (pBF);
 
} /* Find_Bitfield_Table */

/*****************************************************************************

  Function:    ascii_ebcdic_conv

  Description: This function will convert from ascii to EBCDIC or from 
               EBCDIC to ascii depending on conv_type.
  Author: 
      unknown
  Inputs:
      conv_char - the character to convert
      conv_type - A2E - convert from ascii to EBCDIC
                  E2A - convert from EBCDIC to ascii
  Outputs:
      None
  Return values:
      BYTE - the converted character
  Modified by:
      FArad      8/13/98
******************************************************************************/
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
        |   Y     Z           .     <     (     +     |     &     !  |
        |   $     *     )     ;     -     /     ,     %     _     >  |
        |   ?     :     #     @     '     =     "                    |
        +------------------------------------------------------------+
      */

   static unsigned char ebcdic_chart[87] = 
      {
         0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9,
         0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x91,
         0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0xA2, 0xA3,
         0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xC1, 0xC2, 0xC3, 0xC4,
         0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5,
         0xD6, 0xD7, 0xD8, 0xD9, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7,
         0xE8, 0xE9, 0x40, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x5A,
         0x5B, 0x5C, 0x5D, 0x5E, 0x60, 0x61, 0x6B, 0x6C, 0x6D, 0x6E,
         0x6F, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F
      };

   static unsigned char ascii_chart[87] = 
      {
         0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
         0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A,
         0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73, 0x74,
         0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x41, 0x42, 0x43, 0x44,
         0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E,
         0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
         0x59, 0x5A, 0x20, 0x2E, 0x3C, 0x28, 0x2B, 0x7C, 0x26, 0x21,
         0x24, 0x2A, 0x29, 0x3B, 0x2D, 0x2F, 0x2C, 0x25, 0x5F, 0x3E,
         0x3F, 0x3A, 0x23, 0x40, 0x27, 0x3D, 0x22
      };


   if ( 0 == strcmp( conv_type, A2E ) )
   {
      /* CONVERT ASCII TO EBCDIC */
      //the next line is modified by farad 8/13/98
	  for( i=0; i<87 && !found; i++ )
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
      for( i=0; i<87 && !found; i++ )
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

/*****************************************************************************

  Function:    hhutil_send_net_mngmnt_response

  Description: This function will send the network management response to the host
  Author: 
      unknown
  Inputs:
      p_auth_tx - a pointer to the auth_tx structure
  Outputs:
      None
  Return values:
      true  - no error occurred
      false - an error occurred
  Modified by:
******************************************************************************/
INT hhutil_send_net_mngmnt_response(pAUTH_TX p_auth_tx)
{
   pPTE_MSG	 p_msg_host_out        = NULL_PTR;
   LONG      ret_code;
   CHAR	    ErrorMsg[100]= {0};
   CHAR	    Buffer[512]  = {0};

   build_host_structure(p_auth_tx);
   Build_Host_Request(p_auth_tx);

   p_msg_host_out = ptemsg_build_msg( MT_OUTGOING, 
                                      ST1_NONE,
                                      ST2_NONE,
								              dialog_que_name,
                                      application_que_name,
                                      ReqRsp_Buffer,
                                      buflen,
                                      0 );
       
	   
	ptemsg_set_orig_comms_info(p_msg_host_out, p_auth_tx->TLF01_details.tpdu_id);
     
   if ( NULL_PTR == p_msg_host_out )
   {
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Insufficient Memory to build PTE message Term Id:%s ,Merch Id: %s RRN: %s ",
      					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      ncjcb2_log_message( 3, 2, Log_Bufr , "hhutil_send_net_mngmnt_response" , 0 ,p_auth_tx->TLF01_details.card_num);
	   return(false);           
	}
   
   ret_code = pteipc_send(p_msg_host_out, dialog_que_name );


 	if( ret_code < 0 )
	{
		pteipc_get_errormsg( ret_code, ErrorMsg );
       sprintf( Buffer, "pteipc error sending to que %s: %s", dialog_que_name, ErrorMsg );
       memcpy( p_auth_tx->TLF01_details.response_text, Buffer, 40 );
       memset(Log_Bufr,0x00,sizeof(Log_Bufr));
       sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
       					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
       strcat(Buffer,Log_Bufr);
       ncjcb2_log_message( 3, 2, Buffer , "hhutil_send_net_mngmnt_response" , 0 ,p_auth_tx->TLF01_details.card_num);
       free(p_msg_host_out);
       return(false);
	}
   else
   {
      /* Update transaction statistics. */
      update_txn_stats( &Txn_Counts, p_auth_tx, MGT );//praneeth changed SENT to MGT

      /* Log transaction to trace file. */
      write_to_txn_file();
   }
   free(p_msg_host_out);
   return(true);
} /* hhutil_send_net_mngmnt_response */

/*****************************************************************************

  Function:    hhutil_send_net_mngmnt_request

  Description: This function will send the network management request to the host
  Author: 
      unknown
  Inputs:
      p_auth_tx - a pointer to the auth_tx structure
  Outputs:
      None
  Return values:
      true  - no error occurred
      false - an error occurred
  Modified by:
******************************************************************************/
INT hhutil_send_net_mngmnt_request(pAUTH_TX p_auth_tx)
{
   pPTE_MSG	 p_msg_host_out        = NULL_PTR;
   LONG      ret_code;
   CHAR	    ErrorMsg[512]={0};
   CHAR	    Buffer[512]={0};
   
   /* Current Greenwich Mean Time for the transmission-to-host time. */
   get_gmt_time( p_auth_tx->TLF01_details.transmission_timestamp );

   build_host_structure(p_auth_tx);
   Build_Host_Request(p_auth_tx);

   p_msg_host_out = ptemsg_build_msg( MT_OUTGOING, 
                                      ST1_NONE,
                                      ST2_NONE,
								              dialog_que_name,
                                      application_que_name,
                                      ReqRsp_Buffer,
                                      buflen,
                                      0 );
       
	   
	ptemsg_set_orig_comms_info(p_msg_host_out, p_auth_tx->TLF01_details.tpdu_id);
     
   if ( NULL_PTR == p_msg_host_out )
   {
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Insufficient Memory to build host message Term Id:%s ,Merch Id: %s RRN: %s ",
      					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      ncjcb2_log_message( 3, 2, Log_Bufr , "hhutil_send_net_mngmnt_request" , 0 ,p_auth_tx->TLF01_details.card_num);
	   return(false);           
	}
   
   ret_code = pteipc_send(p_msg_host_out, dialog_que_name );

   free( p_msg_host_out );

 	if( ret_code < 0 )
	{
		pteipc_get_errormsg( ret_code, ErrorMsg );
      sprintf( Buffer, "pteipc error sending to que %s: %s", dialog_que_name, ErrorMsg );
      memcpy( p_auth_tx->TLF01_details.response_text, Buffer, 40 );
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncjcb2_log_message( 3, 2, Buffer , "hhutil_send_net_mngmnt_request" , 0 ,p_auth_tx->TLF01_details.card_num);
      return(false);
	}
   else
   {
      /* Update transaction statistics. */
      update_txn_stats( &Txn_Counts, p_auth_tx, MGT );//praneeth changed SENT to MGT

      /* Log transaction to trace file. */
      write_to_txn_file();
   }
   return(true);
} /* hhutil_send_net_mngmnt_request */


/*****************************************************************************

  Function:    Field62Response

  Description: This function will decode field 62 data and put it in HOST_MESSAGE
  Author: 
      unknown
  Inputs:
      fieldid - the index into the bitfield data table
      ReqRsp_Buffer (global) - the buffer that contains the ISO formatted message
  Outputs:
      HOST_MESSAGE (global)  - structure that contains data to go into the 
                               ISO message
  Return values:
      None
  Modified by:
******************************************************************************/
static  void Field62Response(enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT             i;
   INT             Len;
   INT             TotalLen;
   BYTE   BitMap[8];
   BYTE   TempStr[20];

   /*+-------------------------------+
     | Determine length of field 62: |
     |    Length:    1 byte          |
     |    Bit map:   8 bytes         |
     |    Field 1:   1 byte   EBCDIC |
     |    Field 2:   8 bytes  BCD    |
     |    Field 3:   4 bytes  EBCDIC |
     +-------------------------------+*/

   pBF      = Find_Bitfield_Table(fieldid);
   if (pBF != NULL)
   {
      TotalLen = ReqRsp_Buffer[reqidx] + pBF->bd_fieldlen;
      if ( TotalLen > 0 )
      {
         for( i=1; i<9; i++ )
            BitMap[i-1] = ReqRsp_Buffer[reqidx+i];

         memcpy( &pBF->p_bd_pdata[0], &ReqRsp_Buffer[reqidx], 9 );
      }
    
      /* Only the first 3 fields are use in JCB. */
      Len = 9;
      memset( TempStr, 0x00, sizeof( TempStr ) );
      if ( BitMap[0] & 0x80 )   /* Field 1 */
      {
         pBF->p_bd_pdata[Len] = ascii_ebcdic_conv( ReqRsp_Buffer[reqidx+Len], E2A );
         Len++;
      }

      if ( BitMap[0] & 0x40 )   /* Field 2 */
      {
         bcd_to_asc(&ReqRsp_Buffer[reqidx+Len], 8, &pBF->p_bd_pdata[Len], 16 );
         Len += 8;
      }

      if ( BitMap[0] & 0x20 )   /* Field 3 */
      {
         memcpy( TempStr, &ReqRsp_Buffer[reqidx+Len], 4 );
         for( i=0; i<4; i++ )
            TempStr[i] = ascii_ebcdic_conv( TempStr[i], E2A );
         Len += 8;
         memcpy( &pBF->p_bd_pdata[Len], TempStr, 4 );
      }
      reqidx += TotalLen;
   }
}

/*****************************************************************************

  Function:    convert_from_julian_day

  Description: This function will 
     convert_from_julian_day - Get MMDD from YYDDD
     input string format  = YYDDD
     output string format = MMDD
  Author: 
      unknown
  Inputs:
      julian_str - the julian date to be converted
  Outputs:
      date_time  - MMDD
  Return values:
      None
  Modified by:
******************************************************************************/
void convert_from_julian_day( pCHAR julian_str, pCHAR date_time )
{
   CHAR  Year[3];
   CHAR  Month[3];
   CHAR  Days[4];
   INT   iYear;
   INT   NumDays;
   INT   JulianDay;
   INT   iMonth;
   INT   i;
   INT   DaysOfMonth[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };

   /* Account for Leap Year */
   strncpy( Year, julian_str, 2 );
   iYear = atoi( Year );
   if ( 0 == ( iYear % 4 ) )
      DaysOfMonth[1]++;

   strncpy( Days, julian_str+2, 3 );
   JulianDay = atoi( Days );
   NumDays = 0;
   for( i=0; i<12; i++ )
   {
      if ( NumDays+DaysOfMonth[i] >= JulianDay )
      {
         iMonth = i+1;
         NumDays = JulianDay - NumDays;
         break;
      }
      else
      {
         NumDays += DaysOfMonth[i];
      }
   }
	
	itoa(iMonth, Month, 10);
   Rj_with_lead_zeros( Month, 2 );
	itoa(NumDays, Days, 10);
   Rj_with_lead_zeros( Days, 2 );
	strncpy( date_time, Month, 2 );
   strncat( date_time+2, Days,  2 );
}
/*****************************************************************************

  Function:    Build_Host_Request

  Description: This function will build the host request buffer.
  Author: 
      unknown
  Inputs:
      p_auth_tx - a pointer to the auth_tx structure
      HOST_MESSAGE (global)  - structure that contains data to go into the 
                               ISO message
  Outputs:
      ReqRsp_Buffer (global) - buffer that contains the ISO formatted message
  Return values:
      true  - no error occurred
      false - an error occurred
  Modified by:
******************************************************************************/
BOOLEAN Build_Host_Request ( pAUTH_TX p_auth_tx)
{
   struct trandef       *pTranDef;
   struct bitfield_data *pBF;
   INT                  bfidx;     /* index to bitfields for this transaction */

   /* Get pointer to tran table */
   pTranDef = Find_Tran_Table(p_auth_tx->TLF01_details.tx_key);
   if (pTranDef != NULL)
   {    
      /*-----------------------------------------*/
      /* Copy in Message Id for this transaction */
      /*-----------------------------------------*/
      memset(ReqRsp_Buffer, 0, COMM_BUF_SZE);   /* zero out the buffer */
      bitmapidx = 4;                            /* point to bitmap     */

      /* Point past bitmap, but 1st check for extended bitmap. */
      if (( AUTH_REVERSAL				  == p_auth_tx->TLF01_details.tx_key) || 
          ( AUTH_REVERSAL_RESPONSE        == p_auth_tx->TLF01_details.tx_key) ||
		  ( AUTH_REVERSAL_ADVICE_RESPONSE == p_auth_tx->TLF01_details.tx_key) || 
          ( AUTH_REVERSAL_ADVICE          == p_auth_tx->TLF01_details.tx_key) ||
          ( AUTH_VOID_SALE                == p_auth_tx->TLF01_details.tx_key) ||
          ( AUTH_LOGON                    == p_auth_tx->TLF01_details.tx_key) ||
          ( AUTH_LOGON_RESPONSE           == p_auth_tx->TLF01_details.tx_key) ||
          ( AUTH_CARD_UPDATE              == p_auth_tx->TLF01_details.tx_key) ||
		  ( AUTH_CARD_UPDATE_RESPONSE     == p_auth_tx->TLF01_details.tx_key) || 
          ( AUTH_REVERSAL_REPEAT          == p_auth_tx->TLF01_details.tx_key)
#if BDOR_62_PREAUTH_VOID
	   || ( AUTH_VOID_PRE_AUTHORIZATION == p_auth_tx->TLF01_details.tx_key)
#endif
		  )/* ADD 0312 also.. TF Phani */
      {
         reqidx = 20;                         /* Extended     */
         ReqRsp_Buffer[bitmapidx] |= BITTAB[bf_1xbitfield].bit_mask;   /* 2 bitmaps */
         BitFieldSize = EXTENDED_BITFIELD;
      }
      else
      {
         reqidx = 12;                        /* Not Extended */
         BitFieldSize = NORMAL_BITFIELD;
      }

      memcpy(&ReqRsp_Buffer[0], pTranDef->td_msgid, sizeof(pTranDef->td_msgid));

      /*   
         if this msg doesn't require the original proccode (its already in the ISO
         field), overwrite it with the one from the trandef table.
       */
   
      if ((pTranDef->td_opts2 & topt2_new_proccode) != 0) 
         memcpy (&HOST_MESSAGE.proccode,
                  pTranDef->td_proccode,
                  sizeof(pTranDef->td_proccode));

      /*-----------------------------------------------------*/
      /* for all requested bitfields for this transaction... */
      /*-----------------------------------------------------*/
      for (bfidx=0; bfidx < BitFieldSize; bfidx++)
      {
         /*----------------------------*/
         /* if not end of bitfields... */
         /*----------------------------*/
         if (pTranDef->td_bitfields[bfidx] != bf_done)
         {
            /*---------------------------------------------------*/
            /* find pointer to bitfield info for this bitfield...*/
            /*---------------------------------------------------*/
            pBF = Find_Bitfield_Table(pTranDef->td_bitfields[bfidx]);

            if (pBF != NULL)
            {
               /*-----------------------------------------------------------*/
               /* if conditional routine returns true, move the bitfield... */
               /*-----------------------------------------------------------*/
               if ((pBF->bd_BFCond)(pBF->bd_fieldid,p_auth_tx->TLF01_details.tx_key) == true)
               {
                  /*------------------------------*/
                  /* set the bit in the bitmap... */
                  /*------------------------------*/        
                  ReqRsp_Buffer[bitmapidx + BITTAB[pTranDef->td_bitfields[bfidx]].byte_no] |=
                        BITTAB[pTranDef->td_bitfields[bfidx]].bit_mask;

                  /*--------------------------*/
                  /* call the move routine... */
                  /*--------------------------*/
                  (pBF->bd_BFMove)(pBF->bd_fieldid);
               }  /* end if */
            }
         }
         else
         {
            bfidx = BitFieldSize;
		   }  /* end if */
      } /* end for */
   
      buflen = reqidx;
      return(true);
   }
   else
      return(false);
} /* end Build_Request_Message */

/*****************************************************************************

  Function:    hhutil_parse_host_response_msg

  Description: This function will parse a host response message
  Author: 
      unknown
  Inputs:
      p_auth_tx - a pointer to the auth_tx structure
      ReqRsp_Buffer(global) - the buffer that contains the ISO formatted message
  Outputs:
      HOST_MESSAGE (global) - structure that contains data to go into the 
                              ISO message
  Return values:
      true  - no error occurred
      false - an error occurred
  Modified by:
      FArad      8/13/98
      DI - 04/14/99 - Added EBCDIC message type
******************************************************************************/

INT hhutil_parse_host_response_msg(pAUTH_TX p_auth_tx)
{
   struct bitfield_data *pBF;
   INT                  bfidx;     /* index to bitfields for this transaction */
   INT                  i;


   memset(&HOST_MESSAGE, 0x00, sizeof(HOST_MESSAGE));

   /* Extract the message type from the host message. */
   memcpy( HOST_MESSAGE.msgtype_bcd, &ReqRsp_Buffer[0],
           sizeof(HOST_MESSAGE.msgtype_bcd) );

   /* Convert Message Type from EBCDIC to ASCII. */
   for( i=0; i<4; i++ )
      HOST_MESSAGE.msgtype_bcd[i] = 
                        ascii_ebcdic_conv(HOST_MESSAGE.msgtype_bcd[i], E2A);

   bitmapidx = 4;       /* point to bitmap */

   /* Point past bitmap, but 1st check for extended bitmap. */
   if ( ReqRsp_Buffer[bitmapidx]  &  BITTAB[bf_1xbitfield].bit_mask )
   {
      reqidx = 20;                         /* Extended     */
      BitFieldSize = EXTENDED_BITFIELD;
   }
   else
   {
      reqidx = 12;                        /* Not Extended */
      BitFieldSize = NORMAL_BITFIELD;
   }

   /*------------------------------------------------------------------
      For every bitfield present in message ( except first bit ) . . .
   -------------------------------------------------------------------*/
   for (bfidx = 2; bfidx <= BitFieldSize; bfidx++)
   {
      if (ReqRsp_Buffer[bitmapidx+BITTAB[bfidx].byte_no] & BITTAB[bfidx].bit_mask)
      {
         /* Bit is on, field is there.... */
         pBF = Find_Bitfield_Table(bfidx);       

         if (pBF != NULL)
         {
            /*---------------------------------------*/
            /* call the Response Handling routine... */
            /*---------------------------------------*/
	   
            (pBF->bd_BFRsp)(pBF->bd_fieldid);
            if (true == invalid_msg_format)
            {
               ncjcb2_log_message( 3, 2, "Invalid message format received. Partial message. Check message source" , "hhutil_parse_host_response_msg" , 0 ,"");
               return(false);
            }
         }
      }
   }

   return(true);
} /* hhutil_parse_host_response_msg */

/*****************************************************************************

  Function:    move_iso_response_to_auth_struct

  Description: This function will convert ISO8583 Fields from their binary fields
               to their ascii fields... 
  Author:
      unknown
  Inputs:
      nmi_code  - 
      p_auth_tx - a pointer to the auth_tx structure
  Outputs:
      None
  Return values:
      true  - no error occurred
      false - an error occurred
  Modified by:
******************************************************************************/
INT move_iso_response_to_auth_struct(CHAR nmi_code[5], pAUTH_TX p_auth_tx)
{
   CHAR   time_date[27];
   CHAR   tempstr [512]={0};
   pCHAR  tempptr;    
   INT    i;
   INT    length;
   CRF01  crf01;
   double conv_rate;
   INT    int_total_amount;
   double double_total_amount;
   LONG   returncode;
   CHAR   Buffer[512]={0};
   CHAR  itemp[5];

   //memset(p_auth_tx, 0x00, Auth_Tx_Size);

   /* MESSAGE TYPE */
   memcpy( p_auth_tx->TLF01_details.message_type,
            HOST_MESSAGE.msgtype_bcd, sizeof(HOST_MESSAGE.msgtype_bcd ) );

   /* FIELD 2: PAN */
   //  This code is right after FIELD 35, don't move it back

   /* FIELD 3: PROCESSING CODE */
   bcd_to_asc (HOST_MESSAGE.proccode,   sizeof(HOST_MESSAGE.proccode),
               p_auth_tx->TLF01_details.processing_code,
               sizeof(p_auth_tx->TLF01_details.processing_code));

   /* FIELD 4: AMOUNT */
   bcd_to_asc (HOST_MESSAGE.amount_bin, sizeof(HOST_MESSAGE.amount_bin),
               p_auth_tx->TLF01_details.total_amount,
               (sizeof(p_auth_tx->TLF01_details.total_amount)));

   /* Store amt here as original amount prior to currency conversion. SCR 565 */
   memcpy( p_auth_tx->TLF01_details.sales_amount,
           p_auth_tx->TLF01_details.total_amount, 12 );

   /* FIELD 6: Cardholder billing amount J link*/
	   bcd_to_asc (HOST_MESSAGE.card_bill_amt, sizeof(HOST_MESSAGE.card_bill_amt),
	               &p_auth_tx->ch_billing_amt,
	               (sizeof(p_auth_tx->ch_billing_amt)-1));
   /* FIELD 7: GREENWICH TIME - MMDDhhmmss */

   bcd_to_asc (HOST_MESSAGE.gmt_time, sizeof(HOST_MESSAGE.gmt_time),
               &p_auth_tx->TLF01_details.transmission_timestamp,
               sizeof(p_auth_tx->TLF01_details.transmission_timestamp));

   /* FIELD 10: Conversion rate, cardholder billing J link */
   bcd_to_asc (HOST_MESSAGE.conv_rate, sizeof(HOST_MESSAGE.conv_rate),
	               &p_auth_tx->ch_billing_conv_rate,
	               sizeof(p_auth_tx->ch_billing_conv_rate)-1);
   /* FIELD 11: SYSTEMS TRACE AUDIT NUMBER */
   bcd_to_asc (HOST_MESSAGE.stan_bin, sizeof(HOST_MESSAGE.stan_bin),
               p_auth_tx->TLF01_details.sys_trace_audit_num,
               sizeof(p_auth_tx->TLF01_details.sys_trace_audit_num));

   /* FIELD 12: LOCAL TRANSACTION TIME  */
   bcd_to_asc (HOST_MESSAGE.time_bin, sizeof(HOST_MESSAGE.time_bin),
               p_auth_tx->TLF01_details.time_hhmmss,
               sizeof(p_auth_tx->TLF01_details.time_hhmmss));
   
   /*  FIELD 13: LOCAL TRANSACTION DATE  */
   memset(tempstr, 0, sizeof(tempstr));
   genutil_format_date (time_date);
   memcpy (p_auth_tx->TLF01_details.date_yyyymmdd, time_date, 4);  // get year
   bcd_to_asc (HOST_MESSAGE.date_bin, sizeof(HOST_MESSAGE.date_bin),
               tempstr, sizeof(p_auth_tx->TLF01_details.date_yyyymmdd));
   memcpy (p_auth_tx->TLF01_details.date_yyyymmdd+4, &tempstr, 4);

   
   /* FIELD 14: DATE EXPIRED */
   // Moved to below FIELD 35
   
   /* FIELD 15: SETTLEMENT DATE (MMDD) */
   bcd_to_asc (HOST_MESSAGE.date_settle, sizeof(HOST_MESSAGE.date_settle),
               p_auth_tx->TLF01_details.settlement_date,
               sizeof(p_auth_tx->TLF01_details.settlement_date));

   /* FIELD 16: Conversion date; present if Bit 6 is present. J link */
   bcd_to_asc (HOST_MESSAGE.conv_date, sizeof(HOST_MESSAGE.conv_date),
	               p_auth_tx->conv_date,
	               sizeof(p_auth_tx->conv_date)-1);


   /* FIELD 18: SIC NUMBER (MERCHANT TYPE) */
   bcd_to_asc (HOST_MESSAGE.sic_number, sizeof(HOST_MESSAGE.sic_number),
               p_auth_tx->TLF01_details.category_code,
               sizeof(p_auth_tx->TLF01_details.category_code));

   /* FIELD 22: POS ENTRY MODE  */
   bcd_to_asc (HOST_MESSAGE.posentry,   sizeof(HOST_MESSAGE.posentry),
               p_auth_tx->TLF01_details.pos_entry_mode,
               sizeof(p_auth_tx->TLF01_details.pos_entry_mode));

   /*+-----------------------------------------------------------------+
     | Need to convert field 22 to the Hypercom Standard.              |
     |                                                                 |
     |   JCB matches the standard except filler is at the end for JCB. |
     |   For standard, filler should be at the beginning:              |
     |                                                                 |
     |   JCB = xyz0    convert to    Standard = 0xyz                   |
     +-----------------------------------------------------------------+*/
   for( i=2; i>=0; i-- )
      p_auth_tx->TLF01_details.pos_entry_mode[i+1] =
          p_auth_tx->TLF01_details.pos_entry_mode[i];
   p_auth_tx->TLF01_details.pos_entry_mode[0] = '0';
   
   /* FIELD 25: POS CONDITION CODE */
   bcd_to_asc (HOST_MESSAGE.pos_condcd, sizeof(HOST_MESSAGE.pos_condcd),
               p_auth_tx->TLF01_details.pos_condition_code,
               sizeof(p_auth_tx->TLF01_details.pos_condition_code));

   /* FIELD 26: POS PIN capture code */
   bcd_to_asc (HOST_MESSAGE.pin_captcd, sizeof(HOST_MESSAGE.pin_captcd),
               p_auth_tx->pos_pin_capt_code,
               sizeof(p_auth_tx->pos_pin_capt_code));

   /* FIELD 28: Transaction fee amount J link */
	   /* Tran fee amount defined in TLF01 with 8 byte. But we will get 9 byte of value from the host
		  So first byte we do copy to product[2].code[0]*/
    	p_auth_tx->TLF01_details.product_codes[2].code[0]=HOST_MESSAGE.tran_fee_amount[0];
    	memcpy( &p_auth_tx->TLF01_details.tran_fee_amount, &HOST_MESSAGE.tran_fee_amount[1],
           sizeof(HOST_MESSAGE.tran_fee_amount)-1);

   /* FIELD 32: ACQUIRING INSTITUTION ID CODE  */
   length = (HOST_MESSAGE.acq_inst_len[0]+1)/2;
   p_auth_tx->acq_inst_id_len[0] =  HOST_MESSAGE.acq_inst_len[0];
   if (length >= sizeof(p_auth_tx->acq_inst_id_cd)) 
      length = (sizeof(p_auth_tx->acq_inst_id_cd) - 1);
   bcd_to_asc( HOST_MESSAGE.acq_inst_id, length, p_auth_tx->acq_inst_id_cd,
               HOST_MESSAGE.acq_inst_len[0]);

   p_auth_tx->acq_inst_id_cd[HOST_MESSAGE.acq_inst_len[0]] = '\0';

   /* Store here for TLF01. */
   strcpy( p_auth_tx->TLF01_details.acquiring_id, p_auth_tx->acq_inst_id_cd );

   /* FIELD 33: FORWARDING INSTITUTION ID CODE  */
   length = (HOST_MESSAGE.fwd_inst_len[0]+1)/2;
   p_auth_tx->forwarding_inst_id_len[0] =  HOST_MESSAGE.fwd_inst_len[0];
   if (length >= sizeof(p_auth_tx->forwarding_inst_id_cd)) 
      length = (sizeof(p_auth_tx->forwarding_inst_id_cd) - 1);
   bcd_to_asc( HOST_MESSAGE.fwd_inst_id, length,
               p_auth_tx->forwarding_inst_id_cd,
               HOST_MESSAGE.fwd_inst_len[0]);

   p_auth_tx->forwarding_inst_id_cd[HOST_MESSAGE.fwd_inst_len[0]] = '\0';

   /* Store here for TLF01. */
   strcpy( p_auth_tx->TLF01_details.forwarding_institution_id,
           p_auth_tx->forwarding_inst_id_cd );

   /* FIELD 35: TRACK 2 */
   memset(tempstr, 0, sizeof(tempstr));
   bcd_to_asc (HOST_MESSAGE.track2, (HOST_MESSAGE.track2_len[0]+1)/2,   
               tempstr, sizeof(tempstr));
   if ( 1 == (HOST_MESSAGE.track2_len[0] % 2) )
   {
	   if (HOST_MESSAGE.track2_len[0] >= sizeof(p_auth_tx->TLF01_details.track2))
		  memcpy (p_auth_tx->TLF01_details.track2, tempstr+1,
                sizeof(p_auth_tx->TLF01_details.track2)-1);
	   else
		  memcpy (p_auth_tx->TLF01_details.track2, tempstr+1,
                HOST_MESSAGE.track2_len[0]);
   }
   else
   {
      if (HOST_MESSAGE.track2_len[0] >= sizeof(p_auth_tx->TLF01_details.track2))
         memcpy (p_auth_tx->TLF01_details.track2, tempstr,
                 sizeof(p_auth_tx->TLF01_details.track2)-1);
      else
         memcpy (p_auth_tx->TLF01_details.track2, tempstr,
                 HOST_MESSAGE.track2_len[0]);
   }

   if ((tempptr = strchr(p_auth_tx->TLF01_details.track2, '=')) != 0x00)
   {
 	   memcpy( p_auth_tx->TLF01_details.exp_date, tempptr+1,
              sizeof(p_auth_tx->TLF01_details.exp_date)-1);
      i = (((int)tempptr) - ((int)p_auth_tx->TLF01_details.track2));
      memcpy (p_auth_tx->TLF01_details.card_num,
              p_auth_tx->TLF01_details.track2, i);
      itoa((i), p_auth_tx->TLF01_details.card_num_len,  10);
   }

   /* FIELD 2: PAN */
   // If the card number came in, use it instead of track2
   if (0 != strlen(HOST_MESSAGE.pan_bin))
   {
      memset (p_auth_tx->TLF01_details.card_num_len, 0,
              sizeof(p_auth_tx->TLF01_details.card_num_len));
      memset (p_auth_tx->TLF01_details.card_num, 0,
              sizeof(p_auth_tx->TLF01_details.card_num));
      memset(tempstr, 0, sizeof(tempstr));
      bcd_to_asc( HOST_MESSAGE.pan_bin, (HOST_MESSAGE.pan_len[0]+1)/2, tempstr,
                  sizeof(p_auth_tx->TLF01_details.card_num) );
      itoa((HOST_MESSAGE.pan_len[0]),p_auth_tx->TLF01_details.card_num_len,10);
      if ( 1 == (HOST_MESSAGE.pan_len[0] % 2) )
      {
         memcpy( p_auth_tx->TLF01_details.card_num, tempstr+1,
                 sizeof(p_auth_tx->TLF01_details.card_num) );
      }
      else
      {
         memcpy( p_auth_tx->TLF01_details.card_num, tempstr,
                 sizeof(p_auth_tx->TLF01_details.card_num) );
      }
   }
   
   /* FIELD 14 EXPIRATION DATE  */
   if ( HOST_MESSAGE.date_expd[0] != 0x00 )
   {
      /* If the expiration date came in, use it instead of track2. */
      memset( tempstr, 0x00, sizeof(tempstr) );
      bcd_to_asc( HOST_MESSAGE.date_expd,
                  sizeof(HOST_MESSAGE.date_expd),
                  tempstr, sizeof(tempstr) );

      memcpy( p_auth_tx->TLF01_details.exp_date, tempstr,
              sizeof(p_auth_tx->TLF01_details.exp_date) );
   }

   /* FIELD 37: RETRIEVAL REFERENCE NUMBER */
   memcpy( p_auth_tx->TLF01_details.retrieval_ref_num, HOST_MESSAGE.rrn,
           sizeof(HOST_MESSAGE.rrn));

   /* FIELD 38: AUTH NUMBER */
   memcpy( p_auth_tx->TLF01_details.auth_number, HOST_MESSAGE.authcode,
           sizeof(HOST_MESSAGE.authcode));

   /* FIELD 39: RESPONSE CODE */
   memcpy( p_auth_tx->TLF01_details.response_code, HOST_MESSAGE.responsecd,
           sizeof(HOST_MESSAGE.responsecd));

   /* FIELD 41: TERMINAL ID */
   memcpy( p_auth_tx->TLF01_details.terminal_id, HOST_MESSAGE.termid,
           sizeof(HOST_MESSAGE.termid));

   /* Do this in case terminal Id is not 8 characters.
    * For example, DBS has 7 character terminal Ids.
    */
   trim_spaces( p_auth_tx->TLF01_details.terminal_id );

   /* FIELD 42: CARD ACCEPTOR ID CODE */
   memcpy( p_auth_tx->TLF01_details.merchant_id, HOST_MESSAGE.cardaccid,
           sizeof(HOST_MESSAGE.cardaccid));

   /* FIELD 43: CARD ACCEPTOR NAME/LOCATION */
   memcpy( p_auth_tx->acceptor_term_name, HOST_MESSAGE.cardaccname,    25);
   memcpy( p_auth_tx->acceptor_city_name, HOST_MESSAGE.cardaccname+25, 13);
   memcpy( p_auth_tx->acceptor_country,   HOST_MESSAGE.cardaccname+38, 2);

   /* Store DE43 here so it gets logged to TLF01:
    *   Merchant Name         --> TLF01.card_holder_name
    *   City and Country Code --> TLF01.ncf01_retired_cwk
    * SCR 775
    */
   memcpy( p_auth_tx->TLF01_details.card_holder_name,
           HOST_MESSAGE.cardaccname,
           25 );
   memcpy( p_auth_tx->TLF01_details.ncf01_retired_cwk,
           HOST_MESSAGE.cardaccname+25,
           15 );

   /* FIELD 44,J link*/
      if ( HOST_MESSAGE.addl_rsp_len[0] > 0 )
      {
   	      if ( HOST_MESSAGE.addl_rsp_len[0] > 25)
   	         HOST_MESSAGE.addl_rsp_len[0] = 25;

   	      memcpy( &p_auth_tx->TLF01_details.resp_source,
   	              &HOST_MESSAGE.addlrspdata,
   	               HOST_MESSAGE.addl_rsp_len[0] );
   	      /* BYTE 1 of this field contains CAV2 verification result code and/or AVS result code */
   	      p_auth_tx->TLF01_details.cvc = HOST_MESSAGE.addlrspdata[0];
   	      if(p_auth_tx->TLF01_details.cvc != '2')
   	      {
   	    	    get_cavv_result_error( p_auth_tx->TLF01_details.cvc, Buffer );
   				memset(Log_Bufr,0x00,sizeof(Log_Bufr));
   				sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
   									p_auth_tx->TLF01_details.terminal_id,
   									p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
   				strcat(Buffer,Log_Bufr);
   				ncjcb2_log_message( 1, 2, Buffer , "move_iso_response_to_auth_struct" , 0 ,p_auth_tx->TLF01_details.card_num);
   	      }
   	      /* BYTE 2 BIT44 CAV2 result code */
   	      p_auth_tx->TLF01_details.product_codes[17].quantity[0] = HOST_MESSAGE.addlrspdata[1];
   	      get_cav2_result_msg( p_auth_tx->TLF01_details.product_codes[17].quantity[0], Buffer );
   	      memcpy(p_auth_tx->TLF01_details.response_text,Buffer,strlen(Buffer));
   		  memset(Log_Bufr,0x00,sizeof(Log_Bufr));
   		  sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
   							p_auth_tx->TLF01_details.terminal_id,
   							p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
   		 strcat(Buffer,Log_Bufr);
   		 ncjcb2_log_message( 1, 2, Buffer , "move_iso_response_to_auth_struct" , 0 ,p_auth_tx->TLF01_details.card_num);

   		 /* BYTE 3 BIT44 CAV2 result code */
   		// where to store and what action need to take;
      }

      /* FIELD 45,J link*/
      if ( HOST_MESSAGE.track1_len[0] != 0x00 )
      {
         memset( tempstr, 0x00, sizeof(tempstr) );
         memcpy( p_auth_tx->TLF01_details.track1, &HOST_MESSAGE.track1,
                 HOST_MESSAGE.track1_len[0]);
         strcpy( tempstr, p_auth_tx->TLF01_details.track1 );

         /* Get card number */
         if (0x00 != (tempptr = strchr(tempstr, '^')))
         {
            /* Get length of card number.  Need to account for the first character,
             * a Format Code (B), which is not part of the card number.
             */
            i = tempptr-tempstr-1;
            memcpy(p_auth_tx->TLF01_details.card_num, tempstr+1, i );
            sprintf( p_auth_tx->TLF01_details.card_num_len, "%02d", i);
         }

         /* Get exp date */
         /* Search for ^ from the END of track1 because track1 contains two ^s. */
         if ((tempptr = strrchr(tempstr, '^')) != 0x00)
         {
            memcpy( p_auth_tx->TLF01_details.exp_date, tempptr+1,
                    sizeof(p_auth_tx->TLF01_details.exp_date)-1);

            /* Get the service code - used for Visa Electron. */
            memset( ServiceCode, 0x00, SERVICE_CODE_SIZE_PLUS_1 );
            memcpy( ServiceCode, tempptr+5, 3 );
         }
      }

   /* FIELD 48: ADDITIONAL DATA - NEED THE CVC STATUS FLAG AND CAV2. */
   ////  CVC flag is in 44, what do we need out of this???
   find_cvc_flag( HOST_MESSAGE.addl_data_len, HOST_MESSAGE.addl_data,
                  &p_auth_tx->TLF01_details.cvc);

   find_cav2( HOST_MESSAGE.addl_data_len[0], HOST_MESSAGE.addl_data,
              p_auth_tx->TLF01_details.cvc_data );

   /* FIELD 49 TRANSACTION CURRENCY CODE */
	memcpy (&p_auth_tx->TLF01_details.currency_code, &HOST_MESSAGE.tran_curr_cd,
           sizeof(HOST_MESSAGE.tran_curr_cd));

   /* Store currency here as original, prior to currency conversion. SCR 565 */
   memcpy( p_auth_tx->TLF01_details.nfi_seq_nbr,
           p_auth_tx->TLF01_details.currency_code, 3 );

   /* FIELD 51 Cardholder billing currency code J link */
   memcpy( p_auth_tx->ch_billing_curr_code, HOST_MESSAGE.card_curr_cd,sizeof(HOST_MESSAGE.card_curr_cd));

   /* FIELD 52 PIN DATA */
   if (false == blank_or_null_string(HOST_MESSAGE.pin_block,
                                     sizeof(HOST_MESSAGE.pin_block)))
   {
      memset(tempstr, 0, sizeof(tempstr));
      genutil_bin_to_ascii (HOST_MESSAGE.pin_block,
                            sizeof(HOST_MESSAGE.pin_block),
                           &tempstr[0], sizeof(tempstr));   
      memcpy (p_auth_tx->TLF01_details.pin_block, tempstr,
              sizeof(p_auth_tx->TLF01_details.pin_block)-1);
   }

   /* FIELD 54: Additional amounts J link*/
   if(HOST_MESSAGE.addl_len[0] != '\0')
   {
	   if(HOST_MESSAGE.addl_len[0] <= 99)
	   {
		   p_auth_tx->future_use_1[0] = HOST_MESSAGE.addl_len[0];
		   memcpy(p_auth_tx->future_use_1+1,HOST_MESSAGE.addlamts,HOST_MESSAGE.addl_len[0]);
	   }
   }

   /* FIELD 55: Integrated Circuit Card (ICC) System-related Data */
   if (false == blank_or_null_string(HOST_MESSAGE.iccdata,
                                     sizeof(HOST_MESSAGE.iccdata)))
   {
      /* Convert length to BCD. */
      memset( itemp,   0x00, sizeof(itemp)   );
      memset( tempstr, 0x00, sizeof(tempstr) );

      i = HOST_MESSAGE.iccdata_len[1];
      sprintf( itemp, "%04d", i );
      genutil_asc_to_bcd( itemp, 4, tempstr );

      /* Copy the BCD length into AUTH_TX's EMV Block */
      memcpy( p_auth_tx->EMV_details.emv_block, tempstr, 2 );

      /* Copy the rest of the data in, untouched. */
      memcpy( p_auth_tx->EMV_details.emv_block+2,
              HOST_MESSAGE.iccdata, i );
   }

   /* FIELD 63: BANKNET DATA */

   /* This field is used for 0302 File Updates.  But
    * no need to get it.  We do not store it, nor do
    * we used it.  It is echoed from the request.
    */
	
   /* FIELD 70: NETWORK MANAGEMENT INFORMATION CODE */
   bcd_to_asc (HOST_MESSAGE.nmi_code,   sizeof(HOST_MESSAGE.nmi_code),
               nmi_code, sizeof(nmi_code));
   memcpy (p_auth_tx->function_code, nmi_code, sizeof(nmi_code));

   /* FIELD 90 ORIGINAL DATA ELEMENTS */
   memset(tempstr, 0, sizeof(tempstr));
   bcd_to_asc (HOST_MESSAGE.orig_data,   sizeof(HOST_MESSAGE.orig_data),
               tempstr, sizeof(tempstr));
   memcpy(p_auth_tx->TLF01_details.orig_message, tempstr, 4);
   memcpy(p_auth_tx->TLF01_details.orig_sys_trace_num, tempstr+4, 6);
   memcpy(p_auth_tx->orig_local_time_date, tempstr+10,10);
   memcpy(p_auth_tx->orig_acq_inst_id,tempstr+20,11);

   // Actually forwarding institution id code
   memcpy(p_auth_tx->amex_pos_entry_mode,tempstr+31,11);


   /* FIELD 95 REPLACEMENT AMOUNTS */
   // moved to the end of this function

   /* FIELD 96 MESSAGE SECURITY CODE */
   ///  In the future, this will contain the new encryption key.

   /* FIELD 100: RECEIVING INSTITUTION ID CODE  */
   memset(tempstr, 0, sizeof(tempstr));
   bcd_to_asc (HOST_MESSAGE.recv_inst_id, (HOST_MESSAGE.recv_inst_len[0]+1)/2,
				  &tempstr[0], sizeof(tempstr));
	memcpy(p_auth_tx->receiving_inst_id_code, tempstr,
          sizeof(p_auth_tx->receiving_inst_id_code)-1);

   /* FIELD 101: File Name for 0302 Txns */

   /* This field is used for 0302 File Updates.  But
    * no need to get it.  We do not store it, nor do
    * we used it.  It is echoed from the request.
    */

   /* FIELD 120: Record Data */
   length = HOST_MESSAGE.record_len[1];
   if ( length > 0 )
   {
      /*+---------------------------------------------------+
        | Field 120 is record data for Online File Updates. |
        | Format is:                                        |
        |    Transaction Type ( 1 byte ) (Mandatory)        |
        |    Card Number      (16 bytes) (Mandatory)        |
        |    Action Code      ( 2 bytes) (Conditional)      |
        |    Purge Date       ( 6 bytes) (Conditional)      |
        |    Stop Regions     ( 5 bytes) (Conditional)      |
        |                                                   |
        | This order must be preserved.  In other words, if |
        | you have stop regions, you must have purge date   |
        | and action code.  If you have purge date, you     |
        | must have action code.                            |
        +---------------------------------------------------+*/

      memset( tempstr, 0x00, sizeof(tempstr) );
      memcpy( tempstr, HOST_MESSAGE.record, length );

      /* Transaction Type */
      p_auth_tx->TLF01_details.processing_code[0] = tempstr[0];

      /* Card Number */
      memcpy( p_auth_tx->TLF01_details.card_num, tempstr+1, 16 );

      /* Action Code */
      memcpy( p_auth_tx->TLF01_details.type_of_data, tempstr+17, 2 );

      /* Purge Date */
      memcpy( p_auth_tx->TLF01_details.product_codes[0].amount, tempstr+19, 6);

      /* Stop Regions */
      memcpy( p_auth_tx->TLF01_details.product_codes[1].amount, tempstr+25, 5);
   }


   // Save this information to output to the host (our system needs it in PESOs, not USD)
   memcpy(p_auth_tx->reconciliation_cur_code,p_auth_tx->TLF01_details.currency_code, sizeof(p_auth_tx->TLF01_details.currency_code));
   memcpy(p_auth_tx->reconciliation_amount,p_auth_tx->TLF01_details.total_amount, sizeof(p_auth_tx->TLF01_details.total_amount));

   /*+------------------------------------------------------------------+
   | Here is a requirement unique to Equitable:                      
   |
   | ------------------------------------------                      
   |
   |   Field 49 must be "840" (USD).  Then use field 4 for the amount 
   |   field to convert to PESOs, and use the PESO Balance Account for 
   |   authorization purposes.  
   |
   | This is for switched in, or Issuing, transactions only.         
   |
   +------------------------------------------------------------------+*/
   // store the original total_amount and currency code
   if ((0 == strncmp(p_auth_tx->TLF01_details.message_type, "0100", 4)) ||
       (0 == strncmp(p_auth_tx->TLF01_details.message_type, "0120", 4)) ||
       (0 == strncmp(p_auth_tx->TLF01_details.message_type, "0121", 4)) ||
       (0 == strncmp(p_auth_tx->TLF01_details.message_type, "0420", 4)) ||
       (0 == strncmp(p_auth_tx->TLF01_details.message_type, "0421", 4)) ||
       (0 == strncmp(p_auth_tx->TLF01_details.message_type, "0620", 4)))
   {
      if ( 0 == strcmp( USD, p_auth_tx->TLF01_details.currency_code ) ) 
      {
         strcpy( crf01.primary_key.currency_code, PESO);// convert the total_amount from USD to PESOs

         /* Get conversion value to convert from USD to PESOs */
         returncode = FindMemRecord( (pBYTE)&crf01,
                                     sizeof(crf01.primary_key.currency_code),
                                     sizeof(crf01),
                                     "Crf01Table");
         if ( returncode < 0 )
         {
            /* There is either a problem with shared memory
             * or the record is missing.  Go to the database.
             */
            returncode = get_conv_rate_from_database( &crf01, p_auth_tx );
         }

         if (returncode >= 0)
         {
            // currency code found in CRF01 table
            strcpy( p_auth_tx->TLF01_details.currency_code, PESO );
            strcpy( p_auth_tx->TLF01_details.conversion_rate, crf01.conversion_rate);
            conv_rate = strtod(crf01.conversion_rate, 0);
            if (conv_rate <= 0)
            {
               sprintf( Buffer,
                       "Conversion rate is 0 for currency code: %s",
                        p_auth_tx->TLF01_details.conversion_rate );
               memcpy( p_auth_tx->TLF01_details.response_text, Buffer, 40 );
               ncjcb2_log_message( 3, 3, Buffer , "move_iso_response_to_auth_struct" , 0 ,"");
               return(false);
            }

            int_total_amount = atoi(p_auth_tx->TLF01_details.total_amount);
            double_total_amount = ((double)int_total_amount*conv_rate);
            int_total_amount = (int)double_total_amount;
            itoa(int_total_amount, p_auth_tx->TLF01_details.total_amount, 10);
            Rj_with_lead_zeros(p_auth_tx->TLF01_details.total_amount, 12);
            if ((0 == strncmp(p_auth_tx->TLF01_details.message_type, "0420", 4)) ||
               (0 == strncmp(p_auth_tx->TLF01_details.message_type, "0421", 4)))
            {
               strncpy (p_auth_tx->TLF01_details.actual_amount, p_auth_tx->TLF01_details.total_amount, sizeof(p_auth_tx->TLF01_details.actual_amount));

       /* FIELD 95 REPLACEMENT AMOUNTS */
               if (0 != strlen(HOST_MESSAGE.repl_amts))
               {
                  //strncpy (p_auth_tx->TLF01_details.total_amount, HOST_MESSAGE.repl_amts, 12);
                  strncpy(p_auth_tx->TLF01_details.reversal_amount,HOST_MESSAGE.repl_amts, 12);

                  if( atoi(p_auth_tx->TLF01_details.reversal_amount) != 0)
                  {
                      //Partial Reversal

                     /* Save the partial reversal amount into a field that does
                      * not get modified.
                      */
                     memcpy( p_auth_tx->TLF01_details.refund_amount,
                             p_auth_tx->TLF01_details.reversal_amount, 12 );

                      strncpy (p_auth_tx->TLF01_details.orig_amount, p_auth_tx->TLF01_details.total_amount, sizeof(p_auth_tx->TLF01_details.total_amount));
                      int_total_amount = atoi(p_auth_tx->TLF01_details.reversal_amount);
                      double_total_amount = ((double)int_total_amount*conv_rate);
                      int_total_amount = (int)double_total_amount;
                      itoa(int_total_amount, p_auth_tx->TLF01_details.reversal_amount, 10);
                      Rj_with_lead_zeros(p_auth_tx->TLF01_details.reversal_amount, 12);

                  }
                  else
                  {
                      //Full Reversal
                     strncpy (p_auth_tx->TLF01_details.reversal_amount, p_auth_tx->TLF01_details.total_amount, sizeof(p_auth_tx->TLF01_details.total_amount));

                     /* Save the reversal amount into a field that does
                      * not get modified.
                      */
                     memcpy( p_auth_tx->TLF01_details.refund_amount,
                             p_auth_tx->TLF01_details.reversal_amount, 12 );
                  }
               }
            }
         }
         else if (-1L == returncode)
         {
            sprintf( tempstr,
                    "Currency conversion (CRF01) record not found for currency %s",
                     PESO );
            memcpy( p_auth_tx->TLF01_details.response_text, tempstr, 40 );
            ncjcb2_log_message( 3, 3, tempstr , "move_iso_response_to_auth_struct" , 0 ,"");
         }
      }
      else
      {
         strncpy(p_auth_tx->TLF01_details.response_code, "91", 2);  // This is used to determine if the currency code is ok
      }

   }

   if ((0 == strncmp(p_auth_tx->TLF01_details.message_type, "0420", 4)) ||
      (0 == strncmp(p_auth_tx->TLF01_details.message_type, "0421", 4)))
   {
     /*
      total_int = atoi(p_auth_tx->TLF01_details.total_amount);
      actual_int = atoi(p_auth_tx->TLF01_details.actual_amount);
      reversal_int = total_int - actual_int;
      itoa(reversal_int, p_auth_tx->TLF01_details.reversal_amount,10);
      Rj_with_lead_zeros(p_auth_tx->TLF01_details.reversal_amount, 12);
      */
   }

   if (false == verify_auth_tx_data(p_auth_tx))
      return(false);

   return(true);

} /* move_iso_response_to_auth_struct */

/*****************************************************************************

  Function:    build_host_structure

  Description: This function will use the data in the auth_tx entry to build 
               the HOST_MESSAGE structure that will be required to build the 
               JCB message for the host.

  Author: 
      unknown
  Inputs:
      p_auth_tx - pointer to the auth_tx structure
  Outputs:
      None
  Return values:
      None
  Modified by:
      BGable      6/30/98
******************************************************************************/
void build_host_structure( pAUTH_TX p_auth_tx )
{	
   CHAR   temp_str[50]= {0};
   CHAR   orig_data[43]= {0};
   CHAR   new_string[43]= {0};
   CHAR   time_date[25] = {0};
   CHAR   tempstr [256]={0};
   CHAR   num_cc[4] = {0};
   BYTE   processing_code [7]={0};
   BYTE   pos_entry_mode  [5]={0};
   BYTE   pos_cc[3]={0};
   BYTE   stan[7]={0};
   UINT   count = 0;
   INT    temp_int1 = 0;
   INT    i = 0;
   INT    mcc = 0;
   INT    iIndustry = 0;
   INT    int_total_amount = 0;
   INT    int_rev_amount = 0;
   INT    int_refund_amount =0;
   INT    cav2 = 0;
   INT    emv_txn =0 ;
   INT    emv_len = 0;
   INT    ret_val =0;
   INT    pos_entry_type = POS_ENTRY_NORMAL;
   BYTE   hex_tag[5]={0};

	CHAR   VbV_data[150]={0x00};
   BYTE   tempECI[2]={0};
   BYTE   eCommerce_Indcator[3]={0} ;
   BYTE   card_auth_value[2]={0} ;
   BYTE   trans_id[50]={0} ;
   BYTE   cvv_data[50]={0} ;
   BYTE   bcavv_data[41]={0};
   BYTE   f48_buf[256]={0};
   INT    f48_len = 0;
   INT    len = 0;
   INT    num_binary_bytes = 0;
   INT    cavv_data_exists = false;
   CHAR   IST_emv_hex_len[3] = {0};
   int   priv61_len=0;
   char len_priv61[3]={0};
   long	 remain_amt = 0;


   /* Initialize the host structure before filling it. */
   memset( &HOST_MESSAGE, 0x00, sizeof(HOST_MESSAGE) );
   iIndustry = atoi( p_auth_tx->industry_code );

   /* Determine if this is switched in (response) or switched out (request). */
   if ( (p_auth_tx->TLF01_details.tx_key >= AUTH_SALE)  &&
        (p_auth_tx->TLF01_details.tx_key < AUTH_INVALID) )
   {
      Tran_Flow = SWITCHED_OUT;
   }
   else
   {
      Tran_Flow = SWITCHED_IN;
   }

   /* Get currency code.  Use MCF01 if it is available.  There is
    * a chance that txcntl overwrote currency code in TLF01; this
    * happens for local JCB authorization with remote destination.
    */
   if ( p_auth_tx->MCF01_details.currency_code[0] != 0x00 )
   {
      memcpy( p_auth_tx->TLF01_details.currency_code,
              p_auth_tx->MCF01_details.currency_code,
              3 );
   }
 /* If Original txn process as STIP and later send into host.
      After sent host If ATP gets reversal txn for that.
	  It should go as 420 msg - TF-Ajay 08-Jul-09 */
   if(strcmp(p_auth_tx->TLF01_details.product_codes[1].code,PROCESSED)==0) 
   {
		if(p_auth_tx->TLF01_details.tx_key == AUTH_VOID_SALE)
			memcpy(processing_code, "000000", 6); /* Void sale send as 000000- 20 COt2010*/

		p_auth_tx->TLF01_details.tx_key = AUTH_REVERSAL_ADVICE; 

		strcpy(p_auth_tx->TLF01_details.message_type, "0420" ); 
		printf("sending as 420");

   }

   /* Determine if txn is EMV (Smart Card). */
   if ( p_auth_tx->EMV_details.tran_date[0] == 0x00 )
      emv_txn = false;
   else
   {
      emv_txn = true;
      emv_len = genutil_bcd_to_int( p_auth_tx->EMV_details.emv_block, 2 );
   }

   /* Verified by visa - VbV */
   if ((true == ncjcb2_Is_AsiaPay_Transaction(p_auth_tx)))
   {
      /* All the data is in auth_tx.int_net_facility_data */
      memset( eCommerce_Indcator, 0x00, sizeof(eCommerce_Indcator) );
      memset( card_auth_value,    0x00, sizeof(card_auth_value)    );
      memset( trans_id,           0x00, sizeof(trans_id)           );
      memset( cvv_data,           0x00, sizeof(cvv_data)           );

      memcpy( VbV_data,
              p_auth_tx->int_net_facility_data,
              sizeof(p_auth_tx->int_net_facility_data) );
      len = 0;
      memcpy( eCommerce_Indcator, VbV_data+8, 2 );
      len += 10;
      memcpy( card_auth_value, VbV_data + len, 1 );
      len += 1;
      memcpy( trans_id, VbV_data + len, 20 );
      len += 20;
      memcpy( cvv_data, VbV_data + len, 40 );

      if ( false == blank_or_null_string(trans_id, sizeof(trans_id)) )
         cavv_data_exists = true;
      else if ( false == blank_or_null_string(cvv_data, sizeof(cvv_data)) )
         cavv_data_exists = true;
      else if ( false == blank_or_null_string(eCommerce_Indcator,
                                              sizeof(eCommerce_Indcator)) )
         cavv_data_exists = true;
   }
	
   /* FIELD 2: PAN - Prepend leading zero if length is odd. */
	HOST_MESSAGE.pan_len[0] = strlen( p_auth_tx->TLF01_details.card_num );

   if ( (HOST_MESSAGE.pan_len[0] % 2) == 1 )
   {
      memset (temp_str, 0, sizeof(temp_str));
      temp_str[0] = '0';
      memcpy (&temp_str[1], p_auth_tx->TLF01_details.card_num, HOST_MESSAGE.pan_len[0]);
      str_asc_to_bcd( temp_str, HOST_MESSAGE.pan_len[0] + 1, HOST_MESSAGE.pan_bin );
   }
   else
   {
	   str_asc_to_bcd (p_auth_tx->TLF01_details.card_num, sizeof(HOST_MESSAGE.pan_bin)*2, HOST_MESSAGE.pan_bin);
   }

   /* FIELD 3: PROCESSING CODE */
   // Don't change the original processing code, use a temporary variable
   memset (processing_code, 0, sizeof(processing_code));
   memcpy (processing_code, p_auth_tx->TLF01_details.processing_code, sizeof(processing_code));
   if((p_auth_tx->TLF01_details.tx_key >= AUTH_SALE) && (p_auth_tx->TLF01_details.tx_key < AUTH_INVALID))
   { // request
      if ( (processing_code[0] == 0x00) ||
           (AUTH_VOID_SALE == p_auth_tx->TLF01_details.tx_key) )
      {
         memcpy(processing_code, "000000", 6);
      }
      switch (iIndustry)
      {      
         case 900:   // Cash
            strncpy(processing_code, "01", 2);
            break;
         case 901:   // ATM
            strncpy(processing_code, "01", 2);
            break;

         case 902:   // Quasi Cash    JCB doesn't support quasi cash; use AUTH
            strncpy(processing_code, "00", 2);
            break;
         default:
            break;
      }
      if (AUTH_BALANCE_INQUIRY == p_auth_tx->TLF01_details.tx_key) 
      {
         memcpy(processing_code, "30", 2);
      }
      else if (AUTH_CARD_VERIFICATION == p_auth_tx->TLF01_details.tx_key 		||
    		   AUTH_PRE_AUTHORIZATION == p_auth_tx->TLF01_details.tx_key 		||
			   0 == strncmp(p_auth_tx->TLF01_details.processing_code , "30" ,2) ||
			   0 == strncmp(p_auth_tx->TLF01_details.processing_code , "38" ,2))
      {
         memcpy(processing_code, "00", 2);
      }
      else if (AUTH_QUASI_CASH == p_auth_tx->TLF01_details.tx_key) 
      {
         memcpy(processing_code, "00", 2);
      }

      /* As we should not alter the 3rd byte commenting the below code*/
      /* if (0 == strncmp(processing_code ,"01",2))
         memcpy(processing_code, "010", 3);*/

     if (0 == strncmp(processing_code ,"11",2))
         memcpy( processing_code, "00", 2 );

      else if (0 == strncmp(processing_code ,"18",2))
      {
         /* Quasi Cash from Voice */
         memcpy( processing_code, "00", 2 );
      }

      if ( 0 == strcmp(MCC_QUASI_CASH,p_auth_tx->TLF01_details.category_code))
         memcpy( processing_code, "00", 2 );

#if BDOR_62_PREAUTH_VOID
      if(AUTH_VOID_PRE_AUTHORIZATION == p_auth_tx->TLF01_details.tx_key)
      {
    	  memcpy( processing_code, "00", 2 );
      }
#endif
   }
   asc_to_bcd (processing_code, sizeof(HOST_MESSAGE.proccode)*2,
               HOST_MESSAGE.proccode);
   /* In order to show proceesing code in transaction lookup page( OCT-2017 Mandated changes) */
	memcpy(p_auth_tx->TLF01_details.product_codes[17].amount,processing_code,sizeof(HOST_MESSAGE.proccode)*2);

   /* FIELD 4: AMOUNT */
   /* TF Phani .. For Cadience We need to forward whatever we recvied from HOST*/
	if(0 == strncmp(p_auth_tx->TLF01_details.product_codes[3].quantity,"PR",2))
	{
		asc_to_bcd( p_auth_tx->TLF01_details.product_codes[19].amount,
					sizeof(HOST_MESSAGE.amount_bin)*2,
					HOST_MESSAGE.amount_bin);
	}
	else if(0 == strncmp(p_auth_tx->TLF01_details.handler_queue,"ncjcbA",5))
	 {
		asc_to_bcd( p_auth_tx->reconciliation_amount,
				   sizeof(HOST_MESSAGE.amount_bin)*2,
				   HOST_MESSAGE.amount_bin);
	 }
	 else if((strlen(p_auth_tx->TLF01_details.nfi_seq_nbr) > 0) &&
			 0!=strncmp(p_auth_tx->TLF01_details.nfi_seq_nbr, PESO,3) &&
			 0!=strncmp (p_auth_tx->TLF01_details.nfi_seq_nbr, USD,3) &&
			  0==strncmp(p_auth_tx->TLF01_details.handler_queue,"sbatch",6))
	{
  		asc_to_bcd( p_auth_tx->TLF01_details.sales_amount,
  				   sizeof(HOST_MESSAGE.amount_bin)*2,
  				   HOST_MESSAGE.amount_bin);
	}
	 else
	 {
	   asc_to_bcd( p_auth_tx->TLF01_details.total_amount,
				   sizeof(HOST_MESSAGE.amount_bin)*2,
				   HOST_MESSAGE.amount_bin);
	 }

#if BDOR_62_PREAUTH_VOID
  if (p_auth_tx->TLF01_details.tx_key == AUTH_VOID_PRE_AUTHORIZATION)
  {
	  asc_to_bcd( p_auth_tx->TLF01_details.total_interest,
			  	  sizeof(HOST_MESSAGE.amount_bin)*2,
				  HOST_MESSAGE.amount_bin );
  }
#endif

   /* FIELD 7: GREENWICH TIME - MMDDhhmmss from the YYYYMMDDhhmmss date */
	if( 0  == strncmp(p_auth_tx->TLF01_details.product_codes[3].quantity,"PR",2) &&
		'\0' != p_auth_tx->orig_local_time_date[0]	   )
	{
		memcpy( HOST_MESSAGE.gmt_time,
				p_auth_tx->orig_local_time_date,
				bf7_len );
	}
   asc_to_bcd (p_auth_tx->TLF01_details.transmission_timestamp, sizeof(HOST_MESSAGE.gmt_time)*2,
               HOST_MESSAGE.gmt_time);

/* FIELD 11: STAN */
   /* EQIT INTERFACE changes,In case of Time-out reversals use STAN from original request */
   /* Girija Y, TF Dec 31 2008 */
   if (( p_auth_tx->TLF01_details.tx_key == AUTH_VOID_SALE ) ||
	   ( p_auth_tx->TLF01_details.tx_key == AUTH_REVERSAL ) ||
	   (p_auth_tx->TLF01_details.tx_key == AUTH_REVERSAL_ADVICE) ||
	   (p_auth_tx->TLF01_details.tx_key == AUTH_REVERSAL_REPEAT)
#if BDOR_62_PREAUTH_VOID
	|| (p_auth_tx->TLF01_details.tx_key == AUTH_VOID_PRE_AUTHORIZATION)
#endif
	   )
   {
      strcpy( stan, p_auth_tx->TLF01_details.orig_sys_trace_num );
   }
   else
   {
      strcpy( stan, p_auth_tx->TLF01_details.sys_trace_audit_num );
   }
   asc_to_bcd( stan,
               sizeof(HOST_MESSAGE.stan_bin)*2,
               HOST_MESSAGE.stan_bin );

   /* FIELD 12: TIME */
  /* if (0 == strncmp(processing_code ,"01",2))
   {
      if ((AUTH_REVERSAL_REPEAT != p_auth_tx->TLF01_details.tx_key) &&
          (AUTH_REVERSAL_ADVICE != p_auth_tx->TLF01_details.tx_key))
      {
         if((p_auth_tx->TLF01_details.tx_key >= AUTH_SALE) && (p_auth_tx->TLF01_details.tx_key < AUTH_INVALID) &&
			 true == blank_or_null_string(p_auth_tx->TLF01_details.time_hhmmss, sizeof(p_auth_tx->TLF01_details.time_hhmmss)) )
         {
            ptetime_get_timestamp( time_date );
            get_time( time_date, p_auth_tx->TLF01_details.time_hhmmss );
         }
      }
   } */
   asc_to_bcd (p_auth_tx->TLF01_details.time_hhmmss, sizeof(HOST_MESSAGE.time_bin)*2,
                 HOST_MESSAGE.time_bin);
  	
   
   /* FIELD 13: DATE */
   memset(temp_str, 0, sizeof(temp_str));
   strncpy(temp_str, p_auth_tx->TLF01_details.date_yyyymmdd+4, 4);
	asc_to_bcd (temp_str, sizeof(HOST_MESSAGE.date_bin)*2,
               HOST_MESSAGE.date_bin);

   /* FIELD 14: EXPIRATION DATE */
   asc_to_bcd (p_auth_tx->TLF01_details.exp_date, sizeof(HOST_MESSAGE.date_expd)*2,
               HOST_MESSAGE.date_expd);

   /* FIELD 18: SIC NUMBER */
   asc_to_bcd (p_auth_tx->TLF01_details.category_code , sizeof(HOST_MESSAGE.sic_number)*2,
               HOST_MESSAGE.sic_number);
   
   /*FIELD 19:Acquiring Institution Country Code */
  /* For Sbatch transaction*/
  if ( 0 == strncmp(p_auth_tx->TLF01_details.handler_queue, "sbatchA", 6))
  {
	   if (strlen (p_auth_tx->TLF01_details.currency_code)> 0)
	   {
		   memcpy(HOST_MESSAGE.ctry_code,p_auth_tx->TLF01_details.currency_code,bf19_len);
	   }
	   else if (strlen(ncf01_i.country_code)>0)
	   {
		   memcpy(HOST_MESSAGE.ctry_code,ncf01_i.country_code,bf19_len);

	   }
  }

   /* FIELD 22: POS ENTRY MODE */
   /* TF Phani....Dont change POS MOde of it is coming from JCB HOST */
   if(0 == strncmp(p_auth_tx->TLF01_details.handler_queue,"ncjcbA",5))
   {
	   // First we need to convert.. TF Phani
	   //  Standard = 0xyz convert to  JCB = xyz0
	for( i=0; i<=2; i++ )
      p_auth_tx->TLF01_details.pos_entry_mode[i] =
          p_auth_tx->TLF01_details.pos_entry_mode[i+1];
	p_auth_tx->TLF01_details.pos_entry_mode[3] = '0';
	memcpy( pos_entry_mode, p_auth_tx->TLF01_details.pos_entry_mode,sizeof(p_auth_tx->TLF01_details.pos_entry_mode) );
   }
   else
   {
	   if ( Tran_Flow == SWITCHED_OUT )
	   {
		  /* Set the POS Entry Mode flag for use in other fields. */
		  if ( emv_txn == true )
			 pos_entry_type = POS_ENTRY_EMV_CHIP;

		  else if ( 0 == strncmp("080",p_auth_tx->TLF01_details.pos_entry_mode,3) )
			 pos_entry_type = POS_ENTRY_EMV_MAGSTRIPE;

		  else if ( 0 == strncmp("008",p_auth_tx->TLF01_details.pos_entry_mode,3) )
			 pos_entry_type = POS_ENTRY_EMV_FALLBACK;
	   }

	   if ((true == ncjcb2_Is_AsiaPay_Transaction(p_auth_tx)))
	   {
		  /* Always use this POS Entry Mode for e-commerce. */
		  strcpy( pos_entry_mode, "81x0" );
		  pos_entry_mode[2] = p_auth_tx->TLF01_details.pos_entry_mode[3];
	   }
	   else if(true == ncjcb_check_RPS_Transaction((p_auth_tx)))
	   {
	      	 strcpy( pos_entry_mode, "10" );
	        	 strncat(pos_entry_mode, p_auth_tx->TLF01_details.pos_entry_mode+3 ,1 );
	        	 strncat(pos_entry_mode,"0",1);
	   }
	   else if ( emv_txn == true )
	   {
		  /* This is a chip (EMV/Smart Card) txn.  Check if
		   * card was swiped or if the chip was used.
		   */
		  if (0 == strncmp("080",p_auth_tx->TLF01_details.pos_entry_mode,3) )
		  {
			 /* POS Entry indicates magstripe txn at a chip card terminal . 97X0*/
			 strcpy( pos_entry_mode, "97" );
			 strncat(pos_entry_mode, p_auth_tx->TLF01_details.pos_entry_mode+3 ,1 );
			 strncat(pos_entry_mode,"0",1);
		  }
		  // For JCB  EMV Contact less transaction 07X0
		  else if (0 == strncmp("007",p_auth_tx->TLF01_details.pos_entry_mode,3) )
		  {
			  /* POS Entry indicates Contact less at a chip card terminal. */
			  strncpy( pos_entry_mode, p_auth_tx->TLF01_details.pos_entry_mode+1,3 );
			  strncat(pos_entry_mode,"0",1);
		  }
		  else
		  {
			 /* Normal Chip Card Txn 05X0*/
			 strncpy( pos_entry_mode, p_auth_tx->TLF01_details.pos_entry_mode+1,3 );
			 strncat(pos_entry_mode,"0",1);
		  }
	   }
	   else if ( pos_entry_type == POS_ENTRY_EMV_FALLBACK )
	   {
			 /* POS Entry indicates magstripe txn at a chip card terminal . 97X0*/
			 strcpy( pos_entry_mode, "97" );
			 strncat(pos_entry_mode, p_auth_tx->TLF01_details.pos_entry_mode+3 ,1 );
			 strncat(pos_entry_mode,"0",1);
	   }
	   else if ( pos_entry_type == POS_ENTRY_EMV_MAGSTRIPE )
	   {
	   	 /* Fall Back transaction. 97X0*/
		 strcpy( pos_entry_mode, "97" );
		 strncat(pos_entry_mode, p_auth_tx->TLF01_details.pos_entry_mode+3 ,1 );
		 strncat(pos_entry_mode,"0",1);
	   }

	   // For JCB Magstrip Contact less transaction 91X0
	   else if (0 == strncmp("091",p_auth_tx->TLF01_details.pos_entry_mode,3) )
	   {
		   /* POS Entry indicates Contact less at a chip card terminal. */
		   strncpy( pos_entry_mode, p_auth_tx->TLF01_details.pos_entry_mode+1,3 );
		   strncat(pos_entry_mode,"0",1);
	   }
	   else if (0 == strncmp("002",p_auth_tx->TLF01_details.pos_entry_mode,3) )
	   {
	  	   /* Mag strip Transaction. 02X0 */
		   strncpy( pos_entry_mode, p_auth_tx->TLF01_details.pos_entry_mode+1,3 );
		   strncat(pos_entry_mode,"0",1);
	   }
	   else if ((0 == strncmp("001", p_auth_tx->TLF01_details.pos_entry_mode, 3)) &&
	   			   	(0 == strncmp(p_auth_tx->TLF01_details.pos_condition_code, "00", 2)))
	   {
	   	   /* Manual Keyin Transaction. 01X0 */
	   	   strncpy( pos_entry_mode, p_auth_tx->TLF01_details.pos_entry_mode+1, 3);
	   	   strncat(pos_entry_mode, "0", 1);
	   }
#if BDOR_68_REQ_1_JCB
	   /*Lets populate the POS Entry mode for MobilePOS-NFC Terminal for Tap2Phone Transactions With Pin*/
	   else if (true == ncjcb2_check_if_sbatch_qualify_for_MobilePOS_NFC_Tap2Phone_trx_with_pin(p_auth_tx))
	   {
		   strncpy( pos_entry_mode,
					JCB_MOBILE_NFC_TAP2PHONE_TXN_WITH_PIN,
					JCB_DE22_PEM_LEN );
	   }
	   else if (true == ncjcb2_check_if_sbatch_qualify_for_MobilePOS_NFC_Tap2Phone_trx_without_pin (p_auth_tx))
	   {
		   strncpy( pos_entry_mode,
					JCB_MOBILE_NFC_TAP2PHONE_TXN_WITHOUT_PIN,
					JCB_DE22_PEM_LEN );
	   }
	   else if(true == ncjcb2_check_if_trx_is_jcb_MobilePOS_NFC_Tap2Phone_trx_with_pin(p_auth_tx))
	   {
		   strncpy( pos_entry_mode,
					JCB_MOBILE_NFC_TAP2PHONE_TXN_WITH_PIN,
					JCB_DE22_PEM_LEN );
	   }
	   /*Lets populate the POS Entry mode for MobilePOS-NFC Terminal for Tap2Phone Transactions Without Pin*/
	   else if(true == ncjcb2_check_if_trx_is_jcb_MobilePOS_NFC_Tap2Phone_trx_without_pin(p_auth_tx))
	   {
		   strncpy( pos_entry_mode,
					JCB_MOBILE_NFC_TAP2PHONE_TXN_WITHOUT_PIN,
					JCB_DE22_PEM_LEN );
	   }
#endif

#if BDOR_68_REQ_3_JCB   /* Swiftpass BDO Pay Credit */
   else if (true == ncjcb2_check_if_sbatch_qualify_for_Swiftpass_BDO_Pay_credit_trx_with_pin(p_auth_tx))
   {
	   strncpy( pos_entry_mode,
			    JCB_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITH_PIN_PEM,
				JCB_DE22_PEM_LEN );
   }
   else if (true == ncjcb2_check_if_sbatch_qualify_for_Swiftpass_BDO_Pay_credit_trx_without_pin(p_auth_tx))
   {
	   strncpy( pos_entry_mode,
			    JCB_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITHOUT_PIN_PEM,
				JCB_DE22_PEM_LEN );
   }
   else if(true == ncjcb2_check_if_trx_is_jcb_Swiftpass_BDO_Pay_credit_trx_with_pin(p_auth_tx))
   {
	   strncpy( pos_entry_mode,
			    JCB_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITH_PIN_PEM,
				JCB_DE22_PEM_LEN );
   }
   else if(true == ncjcb2_check_if_trx_is_jcb_Swiftpass_BDO_Pay_credit_trx_without_pin(p_auth_tx))
   {
	   strncpy( pos_entry_mode,
			    JCB_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITHOUT_PIN_PEM,
				JCB_DE22_PEM_LEN );
   }
#endif
#if BDOR_68_REQ_5_JCB /*Ecommerce Entry Mode From Swiftpass BDOPay Transactions*/
    else if (true == ncjcb2_check_if_sbatch_qualify_for_ecommerce_swiftpass_BDOPay_trx (p_auth_tx))
    {
 	   strncpy( pos_entry_mode,
 			    JCB_ECOMMERCE_SWIFTPASS_BDOPAY_TXN_PIN_PEM,
 				JCB_DE22_PEM_LEN );
    }
	else if(true == ncjcb2_check_if_trx_is_jcb_ecommerce_swiftpass_BDOPay_trx (p_auth_tx))
	{
	   strncpy( pos_entry_mode,
			    JCB_ECOMMERCE_SWIFTPASS_BDOPAY_TXN_PIN_PEM,
				JCB_DE22_PEM_LEN );
    }
#endif
	   else
	   {
		  /* Use a temporary variable to create the new pos entry mode so
		   * that the original value is inserted into TLF01.
		   */
		  i = sizeof(pos_entry_mode);
		  memset( pos_entry_mode, 0x00, i );
		  memcpy( pos_entry_mode, p_auth_tx->TLF01_details.pos_entry_mode, i);
		  if ('Y' != p_auth_tx->TLF01_details.voice_auth[0])
		  {
			 if (pos_entry_mode[0] != '\0')  
			 {
				/*
				 * input is Hypercom iso8583: 0aab   where
				 *    aa = PAN entry mode
				 *    b  = PIN entry capabitlity
				 *
				 * convert to JCB iso8583:    aab0   where
				 *    aa = PAN entry mode
				 *    b  = PIN entry capabitlity
				 */
				pos_entry_mode [0] = 0x30;     /* set to '0' */
				pos_entry_mode [1] = pos_entry_mode [2];
				pos_entry_mode [2] = 0x31;     /* Always set to 1 (SCR 15990) */
				pos_entry_mode [3] = 0x30;     /* set to '0' */
			 }
			 if(p_auth_tx->TLF01_details.pos_entry_mode[3] == '2')
			 {
				 pos_entry_mode [2] = 0x32;
			 }
		  }
		  else
		  {
			 strcpy(pos_entry_mode, "0120");
		  }
	   }
   }
   asc_to_bcd (pos_entry_mode, sizeof(HOST_MESSAGE.posentry)*2, 
               HOST_MESSAGE.posentry);

   memcpy(p_auth_tx->TLF01_details.product_codes[9].code,
		   pos_entry_mode,sizeof(HOST_MESSAGE.posentry)*2);

   /* FIELD 23: CARD SEQUENCE NUMBER */
   de23_present = false;
   if ( emv_txn == true )
   {
      memset( tempstr,  0x00, sizeof(tempstr) );
      memset( hex_tag,  0x00, sizeof(hex_tag) );
      hex_tag[0] = 0x5F;
      hex_tag[1] = 0x34;
      ret_val = genutil_get_emv_tag( hex_tag,
                                     p_auth_tx->EMV_details.emv_block+2,
                                     emv_len, tempstr, &len );
      if ( ret_val == true )
      {
         /* Convert the value to ASCII to get the numeric value. */
         de23_present = true;
         bcd_to_asc( tempstr, len,
                     temp_str, len+len );
         i = atoi( temp_str );
      }
   }

  if ((emv_txn == false) && (de23_present==false) )
  {
	   /* POS message contain DE23 but no DE55. Do not change the POS entry mode
	   Send as it coming from POS */
	   if((p_auth_tx->host2_message_seq_nbr[0] != 0x00))
	   {
		   memset(pos_entry_mode,0x00, (sizeof(pos_entry_mode)-1));
		   strncpy( pos_entry_mode, p_auth_tx->TLF01_details.pos_entry_mode, (sizeof(pos_entry_mode)-1) );
		   asc_to_bcd (pos_entry_mode, sizeof(HOST_MESSAGE.posentry)*2,
					   HOST_MESSAGE.posentry);
	   }
   }

   if ((de23_present == false) && (p_auth_tx->host2_message_seq_nbr[0] != 0x00))
   {
      /* Card Seq Nbr not in DE55. It came in DE23 from the terminal. */
      i = atoi( p_auth_tx->host2_message_seq_nbr );
      de23_present = true;
   }

   if ( de23_present == true )
   {
      /* Value must be right justified, zero filled. */
      sprintf( temp_str, "%04d", i );
      asc_to_bcd( temp_str, sizeof(HOST_MESSAGE.card_seq_num)*2, 
                  HOST_MESSAGE.card_seq_num);
   }

   /* FIELD 25: POS CONDITION CODE */
   memset( pos_cc, 0x00, sizeof(pos_cc) );
#if BDOR_68_REQ_5_JCB /*Ecommerce Entry Mode From Swiftpass BDOPay Transactions*/

	if(true == ncjcb2_check_if_trx_is_jcb_ecommerce_swiftpass_BDOPay_trx (p_auth_tx))
	{
	   strncpy( pos_cc,
			    JCB_ECOMMERCE_SWIFTPASS_BDOPAY_TXN_POS_CONDITION_CODE,
				POS_CONDITION_CODE_LEN );
	}
#endif
   if ( (p_auth_tx->TLF01_details.tx_key >= AUTH_SALE) &&
        (p_auth_tx->TLF01_details.tx_key <  AUTH_INVALID) )
   {
      /* Request */
	   /* JLINK9 request july 26 2018, paolo requested */
		if((strncmp(p_auth_tx->TLF01_details.processing_code,"30",2)     == 0   ||
		    strncmp(p_auth_tx->TLF01_details.processing_code,"38",2)     == 0 ) &&
		    (strncmp(p_auth_tx->TLF01_details.handler_queue,"dcp",3 ) == 0    ||
		     strncmp(p_auth_tx->TLF01_details.processing_code,"VOICE",5) == 0 ))
		{
			strcpy( pos_cc, POS_CC_PRE_AUTH_CARD_VERFICATION );
		}
		else if ((true == ncjcb2_Is_AsiaPay_Transaction(p_auth_tx)))
		{
		 /* Always use this POS Condition Code for e-commerce. */
			strcpy( pos_cc, "00" );
		}
		else if ((strncmp(p_auth_tx->TLF01_details.handler_queue, "sbatchA", 6) == 0 ||
					strncmp(p_auth_tx->TLF01_details.message_type, "0200", 4) == 0) &&
					(true == ncjcb_check_RPS_Transaction(p_auth_tx)))
		{
			strcpy( pos_cc, "00" );
			strcpy(p_auth_tx->TLF01_details.product_codes[4].quantity, pos_cc);
		}
		else if ( ('Y' == p_auth_tx->TLF01_details.voice_auth[0]))
		{
		 /* Phone Order */
			strcpy( pos_cc, "00" );
		}
		else
			strcpy( pos_cc, p_auth_tx->TLF01_details.pos_condition_code );
   }
   else
   {
      strcpy( pos_cc, p_auth_tx->TLF01_details.pos_condition_code );
   }
   asc_to_bcd(pos_cc,sizeof(HOST_MESSAGE.pos_condcd)*2,HOST_MESSAGE.pos_condcd);

   /* FIELD 26: POS PIN capture code */
   if (true == hhutil_verify_pin_block(p_auth_tx))
   {
	   if(0 == strncmp(p_auth_tx->TLF01_details.handler_queue,"nceqit" , 6) ||
		  0 == strncmp(p_auth_tx->TLF01_details.handler_queue,"neqit2" , 6))
	   {
		   HOST_MESSAGE.pin_captcd[0] = 6;
	   }
	   else
	   {
		   HOST_MESSAGE.pin_captcd[0] = 4;
	   }
   }

   /* FIELD 28: Tran fee amount*/
   if(p_auth_tx->TLF01_details.tran_fee_amount[0] != '\0')
   {
	   HOST_MESSAGE.tran_fee_amount[0] = 'D';
	   memcpy(HOST_MESSAGE.tran_fee_amount+1,p_auth_tx->TLF01_details.tran_fee_amount,sizeof(HOST_MESSAGE.tran_fee_amount)-1);
   }
   else
   {
	   ncjcb2_log_message( 2, 1, "No DE28 receieved" , "build_host_structure" , 0 ,p_auth_tx->TLF01_details.card_num);
   }
   /* FIELD 32: ACQUIRING INSTITUTION ID CODE */
   memset (new_string, 0, sizeof(new_string));
   if((p_auth_tx->TLF01_details.tx_key >= AUTH_SALE) && (p_auth_tx->TLF01_details.tx_key < AUTH_INVALID)
	   && (strncmp(p_auth_tx->TLF01_details.handler_queue,"ncjcbA",5)))
   {	/* TF Phani ..Check if req is from JCB host */
	   // request 
      strcpy(new_string,ncf01_i.acquirer_inst_id_code);
      HOST_MESSAGE.acq_inst_len[0] = strlen(new_string);

      /* Store here for TLF01. */
      if ( p_auth_tx->TLF01_details.acquiring_id[0] == 0x00 )
         strcpy( p_auth_tx->TLF01_details.acquiring_id, new_string );
   }
   else
   { // response
      HOST_MESSAGE.acq_inst_len[0] = strlen(p_auth_tx->acq_inst_id_cd);
      strcpy(new_string,p_auth_tx->acq_inst_id_cd);
   }
   asc_to_vbcd (new_string, sizeof(HOST_MESSAGE.acq_inst_id)*2,
                HOST_MESSAGE.acq_inst_id);

   /* FIELD 33: FORWARDING INSTITUTION ID CODE */
   memset (new_string, 0, sizeof(new_string));
  if((p_auth_tx->TLF01_details.tx_key >= AUTH_SALE) && (p_auth_tx->TLF01_details.tx_key < AUTH_INVALID)
	   && (strncmp(p_auth_tx->TLF01_details.handler_queue,"ncjcbA",5)))
   {	/* TF Phani ..Check if req is from JCB host */
	  // request
      strcpy(new_string,ncf01_i.forwarding_inst_id_code);
   	HOST_MESSAGE.fwd_inst_len[0] = strlen(new_string);

      /* Store here for TLF01. */
      if ( p_auth_tx->TLF01_details.forwarding_institution_id[0] == 0x00 )
         strcpy(p_auth_tx->TLF01_details.forwarding_institution_id, new_string);
   }
   else
   { // response
      HOST_MESSAGE.fwd_inst_len[0]  = strlen(p_auth_tx->forwarding_inst_id_cd);
      strcpy(new_string,p_auth_tx->forwarding_inst_id_cd);
   }
   asc_to_vbcd (new_string, sizeof(HOST_MESSAGE.fwd_inst_id)*2,
                HOST_MESSAGE.fwd_inst_id);

   /* FIELD 35: TRACK 2 */
	if (p_auth_tx->TLF01_details.track2 [0] != '\0')
   {
      /* If odd length, prepend leading zero. */
      if ( 1 == (strlen(p_auth_tx->TLF01_details.track2) % 2) )
      {
         memset (temp_str, 0, sizeof(temp_str));
         temp_str [0] = '0';
         memcpy (&temp_str[1], p_auth_tx->TLF01_details.track2, strlen(p_auth_tx->TLF01_details.track2));
         asc_to_vbcd (temp_str, sizeof(HOST_MESSAGE.track2)*2, HOST_MESSAGE.track2);
      }
      else
      {
         asc_to_vbcd (p_auth_tx->TLF01_details.track2, sizeof(HOST_MESSAGE.track2)*2, HOST_MESSAGE.track2);
      }
      HOST_MESSAGE.track2_len[0] = strlen( p_auth_tx->TLF01_details.track2);
   }
    
   /* FIELD 37: RETRIEVAL REFERENCE NUMBER */
	if (p_auth_tx->TLF01_details.orig_retrieval_ref_num [0] != '\0')
      memcpy (HOST_MESSAGE.rrn, p_auth_tx->TLF01_details.orig_retrieval_ref_num, sizeof(HOST_MESSAGE.rrn));
   else
      memcpy (HOST_MESSAGE.rrn, p_auth_tx->TLF01_details.retrieval_ref_num, sizeof(HOST_MESSAGE.rrn));

   /* FIELD 38: AUTHORIZATION IDENTIFICATION RESPONSE */
   memcpy( HOST_MESSAGE.authcode,
           p_auth_tx->TLF01_details.auth_number,
           sizeof(HOST_MESSAGE.authcode));

   if ( (AUTH_REVERSAL		  != p_auth_tx->TLF01_details.tx_key) &&
	    (AUTH_REVERSAL_ADVICE != p_auth_tx->TLF01_details.tx_key) &&
        (AUTH_VOID_SALE       != p_auth_tx->TLF01_details.tx_key) &&
        (AUTH_REVERSAL_REPEAT != p_auth_tx->TLF01_details.tx_key) &&
		(AUTH_ADVICE		  != p_auth_tx->TLF01_details.tx_key)
#if BDOR_62_PREAUTH_VOID
	 && (AUTH_VOID_PRE_AUTHORIZATION != p_auth_tx->TLF01_details.tx_key)
#endif
		)
   { 
      if ( 00 != strcmp(p_auth_tx->TLF01_details.response_code, "00"))
      {
         memcpy (HOST_MESSAGE.authcode, "000000", sizeof(HOST_MESSAGE.authcode));
      }
   }
   else
   {
      if ( 00 == strcmp(p_auth_tx->TLF01_details.response_code, LATE_RESPONSE))
      {
         memcpy (HOST_MESSAGE.authcode, "000000", sizeof(HOST_MESSAGE.authcode));
      }
   }


   /* FIELD 39: RESPONSE CODE */

   	
	/* TF Phani  */
		if ( (strncmp( p_auth_tx->TLF01_details.handler_queue,"ncjcb",5) == 0) &&
		     (p_auth_tx->TLF01_details.tx_key == AUTH_ADVICE)		)
		{
			memcpy( p_auth_tx->TLF01_details.response_code,
						p_auth_tx->TLF01_details.voice_auth_text,2);

			memcpy( HOST_MESSAGE.responsecd, 
						p_auth_tx->TLF01_details.response_code,
														bf39_len );
		}
		/* check sbatch queue with only frist 4 character */
		else if(((strncmp( p_auth_tx->TLF01_details.handler_queue,"sbatchA",4) == 0)||
				 (strncmp(p_auth_tx->TLF01_details.originator_queue,"stpmgr",6) == 0)) &&
				 ((p_auth_tx->TLF01_details.tx_key == AUTH_ADVICE)||
				  (p_auth_tx->TLF01_details.tx_key == AUTH_REVERSAL )|| (p_auth_tx->TLF01_details.tx_key == AUTH_REVERSAL_ADVICE)))
		{   /* if txn comes from the sbatch, make response code as 00 because sbatch made response code as null.
			otherwise response code will pass as 00F0 to cadience instead of "F0F0" */
			memcpy(p_auth_tx->TLF01_details.response_code,"00",2);
			memcpy (HOST_MESSAGE.responsecd, p_auth_tx->TLF01_details.response_code, sizeof(HOST_MESSAGE.responsecd));
		}
		else
		{
			memcpy (HOST_MESSAGE.responsecd, p_auth_tx->TLF01_details.response_code, sizeof(HOST_MESSAGE.responsecd));
		}

		if((strncmp( p_auth_tx->TLF01_details.handler_queue,"sbatchA",7) == 0))
		{
		if (0 == strncmp(p_auth_tx->TLF01_details.response_code, "19", 2)) {
			/*Lets log this transaction in ewlog for MC Duplicate PROD issue*/

			memset(Log_Bufr, 0x00, sizeof(Log_Bufr));
			sprintf(Log_Bufr,
					" Timeout Advice_Sbatch Transaction with MSG_TYPE:%s STAN:% Term Id:%s ,Merch Id: %s RRN: %s ",
					p_auth_tx->TLF01_details.message_type,
					p_auth_tx->TLF01_details.sys_trace_audit_num,
					p_auth_tx->TLF01_details.terminal_id,
					p_auth_tx->TLF01_details.merchant_id,
					p_auth_tx->TLF01_details.retrieval_ref_num);

			ncjcb2_log_message(2, 1, Log_Bufr, "build_host_structure", 0,
					p_auth_tx->TLF01_details.card_num);
		} else {

			/*Lets log this transaction in ewlog for MC Duplicate PROD issue*/

			memset(Log_Bufr, 0x00, sizeof(Log_Bufr));
			sprintf(Log_Bufr,
					" Advice_Sbatch Transaction with MSG_TYPE:%s STAN:% Term Id:%s ,Merch Id: %s RRN: %s ",
					p_auth_tx->TLF01_details.message_type,
					p_auth_tx->TLF01_details.sys_trace_audit_num,
					p_auth_tx->TLF01_details.terminal_id,
					p_auth_tx->TLF01_details.merchant_id,
					p_auth_tx->TLF01_details.retrieval_ref_num);

			ncjcb2_log_message(2, 1, Log_Bufr, "build_host_structure", 0,
					p_auth_tx->TLF01_details.card_num);

		}

		}
   /* FIELD 41: CARD ACCEPTOR TERMINAL ID */
   if (strlen(p_auth_tx->TLF01_details.terminal_id) < 8)
   {
      memset(temp_str, 0, sizeof(temp_str));
      strcpy(temp_str, p_auth_tx->TLF01_details.terminal_id);
      for (i = strlen(p_auth_tx->TLF01_details.terminal_id); i <8 ; i++)
         temp_str[i] = ' ';
      memcpy(HOST_MESSAGE.termid,temp_str, sizeof(HOST_MESSAGE.termid));
   }
   else
      memcpy(HOST_MESSAGE.termid,p_auth_tx->TLF01_details.terminal_id, sizeof(HOST_MESSAGE.termid));

   /* FIELD 42: CARD ACCEPTOR IDENTIFICATION CODE */
   if (p_auth_tx->TLF01_details.merchant_id [0] != '\0')
   {
      memset(HOST_MESSAGE.cardaccid, 0x20, sizeof(HOST_MESSAGE.cardaccid));
      memcpy(HOST_MESSAGE.cardaccid, p_auth_tx->TLF01_details.merchant_id, strlen(p_auth_tx->TLF01_details.merchant_id));	
   }

   /* FIELD 43: CARD ACCEPTOR NAME/LOCATION */
   if(0 == strncmp(p_auth_tx->TLF01_details.handler_queue,"sbatchA",6) ||
	  0 == strncmp(p_auth_tx->TLF01_details.handler_queue,parvbh_que_name,7))
   {
	 /* Store the Merchant name */
      memset(HOST_MESSAGE.cardaccname, 0x20, sizeof(HOST_MESSAGE.cardaccname));
	  memcpy(HOST_MESSAGE.cardaccname, p_auth_tx->TLF01_details.card_holder_name,22);
	  HOST_MESSAGE.cardaccname[23]= ' ';
      memcpy( HOST_MESSAGE.cardaccname+23,p_auth_tx->TLF01_details.ncf01_retired_cwk,12 );
      HOST_MESSAGE.cardaccname[37]= ' ';
	  memcpy( HOST_MESSAGE.cardaccname+37, "PHL", 3);
   }
   else if (strncmp(p_auth_tx->TLF01_details.handler_queue,"nceqit",6) == 0 ||
			strncmp(p_auth_tx->TLF01_details.handler_queue,"neqit2",6) == 0 ||
			strncmp(p_auth_tx->TLF01_details.handler_queue,"ncjcb",5)  == 0 )
   {
		memset(HOST_MESSAGE.cardaccname, 0x20, sizeof(HOST_MESSAGE.cardaccname));
		memcpy(HOST_MESSAGE.cardaccname, p_auth_tx->acceptor_term_name, 22);
		HOST_MESSAGE.cardaccname[23]= ' ';
		memcpy(HOST_MESSAGE.cardaccname+23, p_auth_tx->acceptor_city_name,12);
		HOST_MESSAGE.cardaccname[37]= ' ';
		memcpy(HOST_MESSAGE.cardaccname+37, "PHL",3);
   }
   else
   {
      memset(HOST_MESSAGE.cardaccname, 0x20, sizeof(HOST_MESSAGE.cardaccname));
      memcpy(HOST_MESSAGE.cardaccname, p_auth_tx->MCF01_details.name02, 22);
      HOST_MESSAGE.cardaccname[23]= ' ';
      memcpy(HOST_MESSAGE.cardaccname+23, p_auth_tx->MCF01_details.city,12);
      HOST_MESSAGE.cardaccname[37]= ' ';
      memcpy(HOST_MESSAGE.cardaccname+37,"PHL",3);

      /* Store DE43 here so it gets logged to TLF01:
       *   Merchant Name         --> TLF01.card_holder_name
       *   City and Country Code --> TLF01.ncf01_retired_cwk
       * SCR 775
       */

      /* SCR 16694 - ThoughtFocus
       * Merchant name/Location (DE43) info is now logged into TLF01 for all
       * transactions and not just for Debit transactions.
       */

      /*
      memcpy( p_auth_tx->TLF01_details.card_holder_name,
              HOST_MESSAGE.cardaccname,
              25 );
      memcpy( p_auth_tx->TLF01_details.ncf01_retired_cwk,
              HOST_MESSAGE.cardaccname+25,
              15 );  
      */
   }

   /* SCR 16694 - ThoughtFocus
    * Store DE43 here so it gets logged to TLF01:
    *   Merchant Name         --> TLF01.card_holder_name
    *   City and Country Code --> TLF01.ncf01_retired_cwk
    */

    if (strlen(p_auth_tx->MCF01_details.name02)>0)
    {
		memcpy( p_auth_tx->TLF01_details.card_holder_name,
				p_auth_tx->MCF01_details.name02,
				25 );
		memcpy( p_auth_tx->TLF01_details.ncf01_retired_cwk,
				p_auth_tx->MCF01_details.city, 
				13 );
		memcpy( p_auth_tx->TLF01_details.ncf01_retired_cwk+13,
				"PHL",
				3 );
	}


   /* FIELD 48: ADDITIONAL DATA */
   f48_len = 0;
   memset( f48_buf, 0x00, sizeof(f48_buf) );

   /* Get CVC data if it exists. */
   cav2 = atoi( p_auth_tx->TLF01_details.cvc_data );
   memset( temp_str, 0x00, sizeof(temp_str) );
   //if ( cav2 >= 0 )
   if(isnum( p_auth_tx->TLF01_details.cvc_data ) && strlen(p_auth_tx->TLF01_details.cvc_data) > 0)
   {
      strncpy( temp_str, CAV2_TAG_01,2 );
      strncat( temp_str, CAV2_TAG_01_LEN,2 );
      strncat( temp_str, p_auth_tx->TLF01_details.cvc_data,3 );

      f48_len += 7;
   }
   if(0 == strncmp(p_auth_tx->TLF01_details.handler_queue,"sbatch",6))
   {
	   if('\0' != p_auth_tx->TLF01_details.product_codes[2].amount[0])
	   {
		   strncpy( temp_str, DE48_TAG30,4 );
		   strncat( temp_str, p_auth_tx->TLF01_details.product_codes[2].amount,DE48_TAG30_31_LEN);
		   f48_len += 12;
	   }
	   if('\0' != p_auth_tx->TLF01_details.product_codes[5].amount[0])
	   {
		    strncat( temp_str, DE48_TAG31,4 );
			strncat( temp_str, p_auth_tx->TLF01_details.product_codes[5].amount,DE48_TAG30_31_LEN);
			f48_len += 12;
	   }

   }
/*  TF Phani - Below DE48 tag02 code is commented, Kenneth requested to drop
   this value for NCJCB2- date 08th JUNE 2011 pl refer to Porject id 11-08 */
#if 0
   if ((true == ncjcb2_Is_AsiaPay_Transaction(p_auth_tx)))
   {
      /*+-----------------------------------------------------------------+
        | For ecommerce, field 48 will contain the 3-D Secure             |
        | information in tag '02'.  It will have a length of 47,          |
        | broken down as follows:                                         |
        |                                                                 |
        | XID  - 20 bytes: Transaction Identifier                         |
        | CAVV - 20 bytes: Cardholder Authentication Verification value   |
        | ECI  -  1 byte:  Electronic Commerce Indicator                  |
        |                    '05' = 3-D Secure authenticated txn          |
        |                    '06' = Merchant attempted to receive 3-D     |
        |                           Secure authentication, and it failed  |
        |                           due to issuer or cardholder non-      |
        |                           participation for 3-D Secure          |
        |                    '07' = Unauthenticated Secured Txn           |
        |                           (This value is no longer used.)       |
        |                                                                 |
        | There are 6 bytes of filler after ECI.                          |
        |                                                                 |
        | CAVV needs to be decoded from Base64 into 20 byte binary.       |
        | Tran ID comes into ATP as binary, but dcpiso puts it in ASCII.  |
        |                                                                 |
        | Tag '02' must be present if anyone of the above 3 fields        |
        | contains data.                                                  |
        +-----------------------------------------------------------------+*/

      if ( cavv_data_exists )
      {
         /* Data is present.  Populate tag 02. */
         memset( temp_str, 0x00, sizeof(temp_str) );
         strcpy( temp_str, CAVV_TAG_02 );
         strcat( temp_str, CAVV_TAG_02_LEN );

         /* Convert CAVV Tag and Length to EBCDIC. */
         for( i=0; i<4; i++ )
            temp_str[i] = ascii_ebcdic_conv( temp_str[i], A2E );

         memcpy( f48_buf+f48_len, temp_str, 4 );

         f48_len += 4;

         /* -------------------- */
         /* TRANSACTION ID (XID) */
         /* -------------------- */
         if ( true == blank_or_null_string(trans_id, 20) )
         {
            /* Blank fill (EBCDIC format) when not present. */
            memset( f48_buf+f48_len, 0x40, 20 );
         }
         else
         {
            /* Send Trans ID to JCB as it is received from the Gateway. */
            memcpy( f48_buf+f48_len, trans_id, 20 );
         }
         f48_len += 20;

         /* --------------------------------------------------- */
         /* CARDHOLDER AUTHENTICATION VERIFICATION VALUE (CAVV) */
         /* --------------------------------------------------- */
         if ( true == blank_or_null_string(cvv_data, 20) )
         {
            /* Blank fill (EBCDIC format) when not present. */
            memset( f48_buf+f48_len, 0x40, 20 );
         }
         else
         {
            memset( bcavv_data, 0x00, sizeof(bcavv_data) );
            trim_spaces ( cvv_data );
            len = strlen( cvv_data );

            /* Convert Base64 to binary. */
            num_binary_bytes = genutil_base64_to_binary(cvv_data, len, bcavv_data);

            memcpy( f48_buf+f48_len, bcavv_data, 20 );
         }
         f48_len += 20;

         /* ----------------------------------- */
         /* ELECTRONIC COMMERCE INDICATOR (ECI) */
         /* ----------------------------------- */
         if ( eCommerce_Indcator[0] == 0x00 )
         {
            /* Blank fill when not present. */
            memset( f48_buf+f48_len, ' ', 1 );
         }
         else
         {
            memset( tempECI, 0x00, sizeof(tempECI) );
            asc_to_bcd( eCommerce_Indcator, 2, tempECI );
            memcpy( f48_buf+f48_len, tempECI, 1 );
         }
         f48_len++;

         /* ----------------- */
         /* 3-D Secure Filler */
         /* ----------------- */
         memset( f48_buf+f48_len, 0x40, 6 ); /* EBCDIC Spaces */
         f48_len += 6;

         /* Store CAVV data into TLF01 so Equitable Bank can access it.
          * Use voice_auth_text since it is not used for ecommerce.
          * Store Tran ID (40), CAVV (40).
          * Use profile for E-Commerce Ind. (2).
          */

         /* First, convert CAVV to ASCII.  This will be Hex characters. */
         memset( temp_str, 0x00, sizeof(temp_str) );
         genutil_bcd_to_asc(bcavv_data, 20, temp_str, sizeof(temp_str) );

         memcpy( p_auth_tx->TLF01_details.voice_auth_text,    trans_id, 20 );
         memcpy( p_auth_tx->TLF01_details.voice_auth_text+20, temp_str, 40 );
         memcpy( p_auth_tx->TLF01_details.profile, eCommerce_Indcator,   2 );
      }
   }
#endif

   /* Convert CAV2 to EBCDIC. */
   for( i=0; i<f48_len; i++ )
   {
	   f48_buf[i] = ascii_ebcdic_conv( temp_str[i], A2E );
   }

#if BDOR_68_REQ_5_JCB
   if (true == ncjcb2_check_if_trx_is_jcb_ecommerce_swiftpass_BDOPay_trx(p_auth_tx))
   {
		f48_len = 0;
		memset( f48_buf, 0x00, sizeof(f48_buf) );

        /* Data is present.  Populate tag 02. */
        memset( temp_str, 0x00, sizeof(temp_str) );
        strcpy( temp_str, CAVV_TAG_02 );
        strcat( temp_str, CAVV_TAG_02_LEN );

        /* Convert CAVV Tag and Length to EBCDIC. */
        for( i=0; i<4; i++ )
           temp_str[i] = ascii_ebcdic_conv( temp_str[i], A2E );
        memcpy( f48_buf+f48_len, temp_str, 4 );
        f48_len += 4;

        /* -------------------- */
        /* TRANSACTION ID (XID) */
        /* -------------------- */
		/* Blank fill (EBCDIC format) when not present. */
		memset( f48_buf+f48_len, 0x40, 20 );
        f48_len += 20;

        /* --------------------------------------------------- */
        /* CARDHOLDER AUTHENTICATION VERIFICATION VALUE (CAVV) */
        /* --------------------------------------------------- */
		/* Blank fill (EBCDIC format) when not present. */
		memset( f48_buf+f48_len, 0x40, 20 );
		f48_len += 20;

        /* ----------------------------------- */
        /* ELECTRONIC COMMERCE INDICATOR (ECI) */
        /* ----------------------------------- */
		strncpy(eCommerce_Indcator,
			 JCB_ECOMMERCE_SWIFTPASS_BDOPAY_TXN_ECI_NOT_AUTHENITICATED, 2);
		memset( tempECI, 0x00, sizeof(tempECI) );
		asc_to_bcd( eCommerce_Indcator, 2, tempECI );
		memcpy( f48_buf+f48_len, tempECI, 1 );
        f48_len++;

        /* ----------------- */
        /* 3-D Secure Filler */
        /* ----------------- */
        memset( f48_buf+f48_len, 0x40, 6 ); /* EBCDIC Spaces */
        f48_len += 6;

        /* store ECI value in Profile of TLF01 */
        memcpy( p_auth_tx->TLF01_details.profile, eCommerce_Indcator,   2 );
   }
#endif

   if ( f48_len > 0 )
   {
      /* Set the length and put data into Host buffer. */
      HOST_MESSAGE.addl_data_len[0] = f48_len;
      memcpy( HOST_MESSAGE.addl_data, f48_buf, f48_len );
   }
   /* Copy CAVV data to display it on Tranlookup page*/
   if(p_auth_tx->TLF01_details.voice_auth_text[0] == '\0')
   {
	   strcpy(p_auth_tx->TLF01_details.voice_auth_text,p_auth_tx->TLF01_details.dcf01_retired_cwk);
   }

   /* FIELD 49: TRANSACTION CURRENCY CODE */
   /* TF Phani .. For Cadience We need to forward whatever we recvied from HOST*/
	if(0 == strncmp(p_auth_tx->TLF01_details.handler_queue,"ncjcb",5))
	{		
		memcpy( HOST_MESSAGE.tran_curr_cd,
           p_auth_tx->reconciliation_cur_code,
           sizeof(HOST_MESSAGE.tran_curr_cd));   
	}
	 else if (	(strlen(p_auth_tx->TLF01_details.nfi_seq_nbr) > 0) &&
			 	0!=strncmp (p_auth_tx->TLF01_details.nfi_seq_nbr, PESO,3) &&
			 0!=strncmp (p_auth_tx->TLF01_details.nfi_seq_nbr, USD,3) &&
			  0==strncmp(p_auth_tx->TLF01_details.handler_queue,"sbatch",6))
	{
			memcpy( HOST_MESSAGE.tran_curr_cd,
	           p_auth_tx->TLF01_details.nfi_seq_nbr,
	           sizeof(HOST_MESSAGE.tran_curr_cd));
	}
	else
	{
		memcpy( HOST_MESSAGE.tran_curr_cd,
           p_auth_tx->TLF01_details.currency_code,
           sizeof(HOST_MESSAGE.tran_curr_cd));   
	}

   /* FIELD 52: PIN BLOCK */
   if (true == hhutil_verify_pin_block(p_auth_tx))
   {
      memset(tempstr, 0, sizeof(tempstr));
      ascii_to_bin( p_auth_tx->TLF01_details.pin_block, strlen(p_auth_tx->TLF01_details.pin_block),
                    &tempstr[0], sizeof(tempstr));
      memcpy (HOST_MESSAGE.pin_block, tempstr, sizeof(HOST_MESSAGE.pin_block));
   }

   /* FIELD 53: SECURITY RELATED CONTROL INFORMATION */
   if (true == hhutil_verify_pin_block(p_auth_tx))
   {
      asc_to_bcd ("9901010000000000", sizeof(HOST_MESSAGE.sec_cntl)*2,   
               HOST_MESSAGE.sec_cntl);

      /* These values must be set like this.
       * asc_to_bcd does not work for alpha.
       * DE53 values for production is:  99 01 01 00 00 DD 4B 00
       */
      HOST_MESSAGE.sec_cntl[5] = 0xDD;
      HOST_MESSAGE.sec_cntl[6] = 0x4B;

      /* Test Values - Comment out these 2 lines for production. *
      HOST_MESSAGE.sec_cntl[5] = 0x84;
      HOST_MESSAGE.sec_cntl[6] = 0xA6; - - - */
   }


   /* FIELD 55 .Integrated Circuit Card (ICC) System-related Data */
   if (p_auth_tx->EMV_details.emv_block[0] != 0x00 && p_auth_tx->EMV_details.emv_block[1] != 0x00)
   {

	   if( 0 == strncmp(p_auth_tx->TLF01_details.handler_queue,"nceqit",6) ||
				 0 == strncmp(p_auth_tx->TLF01_details.handler_queue,"neqit2",6) )
		{
		   emv_len = p_auth_tx->EMV_details.emv_block[0];
		   if(emv_len > 0 && emv_len < 255 )
		   {   HOST_MESSAGE.iccdata_len[1]= emv_len-3;
			   memcpy(HOST_MESSAGE.iccdata,p_auth_tx->EMV_details.emv_block+4,emv_len-3);
		   }
		}
	   else if(strncmp(p_auth_tx->TLF01_details.handler_queue, "sbatch", 5) == 0 )
	   {
		   emv_len = genutil_bcd_to_int( p_auth_tx->EMV_details.emv_block, 2 );
		   			//emv_len= p_auth_tx->EMV_details.emv_block[1];
		   HOST_MESSAGE.iccdata_len[1]= emv_len;
		   memcpy(HOST_MESSAGE.iccdata,p_auth_tx->EMV_details.emv_block+2,emv_len);
	   }
	    else if( strncmp(p_auth_tx->TLF01_details.handler_queue, "ncjcb", 5) == 0 || strcmp(p_auth_tx->TLF01_details.dispensation.origin,"RO")==0)
		{
			emv_len = genutil_bcd_to_int( p_auth_tx->EMV_details.emv_block, 2 );
			//emv_len= p_auth_tx->EMV_details.emv_block[1];
			HOST_MESSAGE.iccdata_len[1]= emv_len;
			memcpy(HOST_MESSAGE.iccdata,p_auth_tx->EMV_details.emv_block,emv_len);
		}
		else
		{   /* Determine the block size */
			i = genutil_bcd_to_int( p_auth_tx->EMV_details.emv_block, 2 );
			/* Copy only the EMV Tags that MC accepts. */
			get_JCB_EMV_data( i, p_auth_tx->EMV_details.emv_block );
		}
   }

   /* FIELD 61: POINT OF SERVICE INFORMATION */
   if ( Tran_Flow == SWITCHED_OUT )
   {

		if(0 == strncmp(p_auth_tx->TLF01_details.handler_queue,"ncjcb",5) ||
			0 == strncmp(p_auth_tx->TLF01_details.handler_queue,"ncjcb3",6)) //ncjcb3
	    {
			HOST_MESSAGE.priv61_len[0]= 8;
			memcpy( HOST_MESSAGE.priv61,p_auth_tx->TLF01_details.product_codes[16].amount,sizeof(p_auth_tx->TLF01_details.product_codes[16].amount));
			//Store Bit 61 data to be show for POS data on TranLookup
		  	//strncpy(p_auth_tx->TLF01_details.product_codes[16].amount,HOST_MESSAGE.priv61,sizeof(p_auth_tx->TLF01_details.product_codes[16].amount));


			//HOST_MESSAGE.priv61_len[0]=8;
		}
		else if(strncmp(p_auth_tx->TLF01_details.handler_queue, "sbatchA", 6)==0 &&
				strncmp(p_auth_tx->TLF01_details.product_codes[4].code, "ISSUE", 5)==0)
		{
			HOST_MESSAGE.priv61_len[0]= 8;
				memcpy( HOST_MESSAGE.priv61,p_auth_tx->TLF01_details.product_codes[16].amount,sizeof(p_auth_tx->TLF01_details.product_codes[16].amount));

		}
		else
		{
			/* Default Values */
			memset( temp_str, 0x00, sizeof(temp_str) );
			strcpy( temp_str, "222" );

			if((ncjcb2_Is_AsiaPay_Transaction(p_auth_tx) == true))
			{
				iIndustry=ECOMMERCE;
			}

			/* Subfield 61.1 - Mail/Telephone Order Transaction Sign */
			if ( iIndustry == MAIL_PHONE ||  ('Y' == p_auth_tx->TLF01_details.voice_auth[0]))
				temp_str[0] = '2';

			/* Subfield 61.2 - Recurring Transaction Sign */
			//else if ( iIndustry == RPS || strncmp(p_auth_tx->TLF01_details.pos_condition_code,"08",2) == 0 )
			else if ((strncmp(p_auth_tx->TLF01_details.handler_queue, "sbatchA", 6) == 0 ||
    		  	  	    strncmp(p_auth_tx->TLF01_details.message_type, "0100", 4) == 0 ||
      					strncmp(p_auth_tx->TLF01_details.message_type, "0200", 4) == 0) &&
      					(true == ncjcb_check_RPS_Transaction(p_auth_tx)))
			{
				temp_str[0] = '2';
				temp_str[1] = '1';

				/* Check for MOTO Merchant */
				mcc = atoi( p_auth_tx->TLF01_details.category_code );
				if ( (MCC_RANGE2_LOW <= mcc)  &&  (mcc <= MCC_RANGE2_HIGH) )
					temp_str[0] = '1';
			}
			//else if (strncmp (p_auth_tx->TLF01_details.category_code,"59",2) == 0 ||iIndustry  == ECOMMERCE)
			else if((strncmp(p_auth_tx->TLF01_details.pos_condition_code,"59",2) == 0) ||
					(strncmp(p_auth_tx->TLF01_details.pos_condition_code,"05",2) == 0))
			{
				temp_str[0] = '1';
			}

			/* Subfield 61.3 - Pre Authorization Sign */
			if ( (p_auth_tx->TLF01_details.tx_key == AUTH_PRE_AUTHORIZATION) ||
				 (p_auth_tx->TLF01_details.tx_key == AUTH_VOID_PRE_AUTHORIZATION) ||
				 (p_auth_tx->TLF01_details.tx_key == AUTH_CARD_VERIFICATION) ||
				 (p_auth_tx->mac_key[0]           == AUTH_CARD_VERIFICATION) 		 ||
				 0 == strncmp(p_auth_tx->TLF01_details.processing_code , "30" ,2) ||
				 0 == strncmp(p_auth_tx->TLF01_details.processing_code , "38" ,2))
			{
				/* Note: Dcpiso changes Card Verifies to Auths.
				 * The original tx_key is stored in mac_key.
				 */
				temp_str[2] = '1';
			}
#if BDOR_68_REQ_1_JCB

		  if (true == ncjcb2_check_if_sbatch_qualify_for_MobilePOS_NFC_Tap2Phone_trx_with_pin(p_auth_tx))
		  {
			  memset( temp_str, 0x00, sizeof(temp_str) );
			  strncpy( temp_str, JCB_MOBILE_NFC_TAP2PHONE_TXN_POS_DATA,JCB_POS_DATA_LEN );
		  }
		  else if (true == ncjcb2_check_if_sbatch_qualify_for_MobilePOS_NFC_Tap2Phone_trx_without_pin(p_auth_tx))
		  {
			  memset( temp_str, 0x00, sizeof(temp_str) );
			  strncpy( temp_str, JCB_MOBILE_NFC_TAP2PHONE_TXN_POS_DATA,JCB_POS_DATA_LEN );
		  }
		  else if ((true == ncjcb2_check_if_trx_is_jcb_MobilePOS_NFC_Tap2Phone_trx_with_pin( p_auth_tx )))
		  {
			  memset( temp_str, 0x00, sizeof(temp_str) );
			  strncpy( temp_str, JCB_MOBILE_NFC_TAP2PHONE_TXN_POS_DATA,JCB_POS_DATA_LEN );
		  }
		  else if ((true == ncjcb2_check_if_trx_is_jcb_MobilePOS_NFC_Tap2Phone_trx_without_pin( p_auth_tx )))
		  {
			  memset( temp_str, 0x00, sizeof(temp_str) );
			  strncpy( temp_str, JCB_MOBILE_NFC_TAP2PHONE_TXN_POS_DATA,JCB_POS_DATA_LEN );
		  }

#endif

#if BDOR_68_REQ_3_JCB   /* Swiftpass BDO Pay Credit */
		  else if (true == ncjcb2_check_if_sbatch_qualify_for_Swiftpass_BDO_Pay_credit_trx_with_pin(p_auth_tx))
		  {
			  strncpy( temp_str,
					   JCB_SWIFTPASS_BDO_PAY_CREDIT_TXN_POS_DATA,
					   JCB_POS_DATA_LEN );
		  }
		  else if (true == ncjcb2_check_if_sbatch_qualify_for_Swiftpass_BDO_Pay_credit_trx_without_pin(p_auth_tx))
		  {
			  strncpy( temp_str,
					   JCB_SWIFTPASS_BDO_PAY_CREDIT_TXN_POS_DATA,
					   JCB_POS_DATA_LEN );
		  }
		  else if(true == ncjcb2_check_if_trx_is_jcb_Swiftpass_BDO_Pay_credit_trx_with_pin(p_auth_tx))
		  {
			  strncpy( temp_str,
					   JCB_SWIFTPASS_BDO_PAY_CREDIT_TXN_POS_DATA,
					   JCB_POS_DATA_LEN );
		  }
		  else if(true == ncjcb2_check_if_trx_is_jcb_Swiftpass_BDO_Pay_credit_trx_without_pin(p_auth_tx))
		  {
			  strncpy( temp_str,
					   JCB_SWIFTPASS_BDO_PAY_CREDIT_TXN_POS_DATA,
					   JCB_POS_DATA_LEN );
		  }

#endif
#if BDOR_68_REQ_5_JCB /*Ecommerce Entry Mode From Swiftpass BDOPay Transactions*/

		else if (true == ncjcb2_check_if_sbatch_qualify_for_ecommerce_swiftpass_BDOPay_trx(p_auth_tx))
		{
		   strncpy( temp_str,
				    JCB_ECOMMERCE_SWIFTPASS_BDOPAY_TXN_POS_DATA,
					JCB_POS_DATA_LEN );
		}
		else if(true == ncjcb2_check_if_trx_is_jcb_ecommerce_swiftpass_BDOPay_trx (p_auth_tx))
		{
		   strncpy( temp_str,
				    JCB_ECOMMERCE_SWIFTPASS_BDOPAY_TXN_POS_DATA,
					JCB_POS_DATA_LEN );
		}
#endif
			else
			{
				memset( tempstr, 0x00, sizeof(tempstr) );
				memcpy( tempstr, p_auth_tx->MCF01_details.intl_code, 2 );

				/* Convert 2-char alpha country code to 3-char numeric country code. */
				genutil_get_country_code( A2N, tempstr, num_cc );
				if ( num_cc[0] == 0x00 )
				{
					/* Default to Peso for Equitable. Normally we
					 * would default to 900 for an unknown value.
					 */
					strcpy( num_cc, PESO );
				}
				strcpy( num_cc, PESO );  /*to make sure that PESO is default.*/
				memcpy( temp_str+3, num_cc, 3 );
			}

			HOST_MESSAGE.priv61_len[0] = 6;
			memcpy( HOST_MESSAGE.priv61, temp_str, 6 );

			/*Store Bit 61 data to be show for POS data on TranLookup*/
			strncpy(p_auth_tx->TLF01_details.product_codes[16].amount,
					HOST_MESSAGE.priv61,strlen(HOST_MESSAGE.priv61));
		}
	}
	
   /* FIELD 63: Transaction Code */
   if ( Txn_Type == ONLINE_UPDATE )
   {
      i = strlen(TRANSACTION_CODE);
      HOST_MESSAGE.priv63_len[0] = i;
      memcpy( HOST_MESSAGE.priv63, TRANSACTION_CODE, i );
   }

   /* FIELD 70: Network Management Info Code */
   if (0 == strlen(vLogon_NMI_Code))  
   {
      asc_to_bcd (p_auth_tx->function_code, sizeof(HOST_MESSAGE.nmi_code)*2,
                  HOST_MESSAGE.nmi_code);  
   }
   else
   {
      memset(temp_str, 0, sizeof(temp_str));
      strncpy(temp_str, vLogon_NMI_Code, 4);
      asc_to_bcd (temp_str, sizeof(HOST_MESSAGE.nmi_code)*2,
                  HOST_MESSAGE.nmi_code);  
   }

   /* FIELD 90: Get original data elements for field 90. */
   memset (orig_data, 0, sizeof(orig_data));
   if ((0 == strcmp(p_auth_tx->TLF01_details.orig_message, "0200")) || 
       (0 == strcmp(p_auth_tx->TLF01_details.orig_message, "0110")))
   {
      strcpy (orig_data, "0100");
   }
   else
   {
      strcpy (orig_data, p_auth_tx->TLF01_details.orig_message);
   }
   strcat(orig_data, p_auth_tx->TLF01_details.orig_sys_trace_num);

   if((p_auth_tx->TLF01_details.tx_key >= AUTH_SALE) && (p_auth_tx->TLF01_details.tx_key < AUTH_INVALID))
   { // request
	   if(0    == strncmp(p_auth_tx->TLF01_details.product_codes[3].quantity,"PR",2) &&
		  '\0' != p_auth_tx->orig_local_time_date[0])
	   {
		   memcpy( &orig_data[10], p_auth_tx->orig_local_time_date, 10);
	   }
	   else
	   {
		   strcat (orig_data, p_auth_tx->orig_local_time_date);
	   }

	   for (count = 1; count <= (11-strlen(ncf01_i.acquirer_inst_id_code)); count++)
		   strcat(orig_data, "0");
	   strcat (orig_data, ncf01_i.acquirer_inst_id_code);

      for (count = 1; count <= (11-strlen(ncf01_i.forwarding_inst_id_code)); count++)
		   strcat(orig_data, "0");
	   strcat (orig_data, ncf01_i.forwarding_inst_id_code);
   }
   else
   { // response
      strcat (orig_data, p_auth_tx->orig_local_time_date);
      strcat(orig_data, p_auth_tx->orig_acq_inst_id);
      strcat(orig_data, p_auth_tx->amex_pos_entry_mode);  /// Actually forwarding institution id code
	  //strcat(orig_data, p_auth_tx->forwarding_inst_id_cd);  
   }

   str_asc_to_bcd (orig_data, strlen(orig_data), HOST_MESSAGE.orig_data );

   memcpy( HOST_MESSAGE.resp_ind, vLogon_Resp_Indicator, sizeof(HOST_MESSAGE.resp_ind ));

   /* FIELD 95 REPLACEMENT AMOUNTS */
#if BDOR_62_PREAUTH_VOID
   if(p_auth_tx->TLF01_details.tx_key == AUTH_VOID_PRE_AUTHORIZATION)
   {
	   if(strlen(p_auth_tx->TLF01_details.total_interest) > 0 &&
		  p_auth_tx->TLF01_details.saf[0] == INCREMENTAL_AUTH)
	   {
		   temp_int1 = strlen(p_auth_tx->TLF01_details.total_interest);
		   remain_amt = atol(p_auth_tx->TLF01_details.total_interest) - atol(p_auth_tx->TLF01_details.total_amount);
		   sprintf(new_string, "%012d", remain_amt);
		   memset(p_auth_tx->TLF01_details.total_interest, 0x00, sizeof(p_auth_tx->TLF01_details.total_interest));
	   }
	   else
	   {
		   temp_int1 = 0;
	   }

	   for (count = temp_int1; count < 42; count++)
		   new_string[count] = '0';    // zero fill the last 30+ digits
	   memcpy (HOST_MESSAGE.repl_amts, new_string, sizeof(HOST_MESSAGE.repl_amts));
   }
#endif

   else if(0 == strncmp(p_auth_tx->TLF01_details.product_codes[3].quantity,"PR",2))
   {
	   memset( HOST_MESSAGE.repl_amts,'0', 42 );
	   strncpy( HOST_MESSAGE.repl_amts,p_auth_tx->TLF01_details.total_amount,12);
   }

   else if ( (0 == strncmp( p_auth_tx->TLF01_details.message_type, "04", 2 )) ||
        (p_auth_tx->TLF01_details.tx_key == AUTH_VOID_SALE) )
   {
      int_total_amount    = atoi(p_auth_tx->TLF01_details.total_amount);

      if((int_total_amount != atoi(p_auth_tx->TLF01_details.reversal_amount)) &&
         (p_auth_tx->TLF01_details.tx_key != AUTH_VOID_SALE) )
      {
         /* This is a Partial Reversal */
         /* -------------------------- */
         int_rev_amount = atoi(p_auth_tx->TLF01_details.reversal_amount);

         if ( Tran_Flow == SWITCHED_OUT )
         {
            /* This field contains refund amount, not reversal amount. */
            int_refund_amount = int_total_amount - int_rev_amount;
            sprintf( new_string, "%012d", int_refund_amount );
            temp_int1 = strlen(new_string);
         }
         else
         {
            temp_int1 = strlen(p_auth_tx->TLF01_details.reversal_amount);
            strcpy(new_string, p_auth_tx->TLF01_details.reversal_amount);
         }
      }
      else
      {
         /* This is a Full Reversal */
         /* ----------------------- */

         // Force all zeros into the replacement amounts for full reversals
         temp_int1 = 0;
      }
      for (count = temp_int1; count < 42; count++)
         new_string[count] = '0';    // zero fill the last 30+ digits
      memcpy (HOST_MESSAGE.repl_amts, new_string, sizeof(HOST_MESSAGE.repl_amts));
   }

   /* FIELD 100: RECEIVING INSTITUTION ID CODE */
   memset( new_string, 0x00, sizeof(new_string) );
   if ((p_auth_tx->TLF01_details.tx_key >= AUTH_SALE  ) &&
       (p_auth_tx->TLF01_details.tx_key < AUTH_INVALID)  )
   {
      /* Request */
      if ( Txn_Type == ONLINE_UPDATE )
      {
         /* For Online Updates, this value must be hard-coded
          * so it does not affect sign-on and sign-off requests.
          * Different values are used for them.
          */
         strcpy( new_string, "88060000" );
      }
	  
      else
         strcpy( new_string, ncf01_i.receiving_inst_id_code );
   }
   else if( p_auth_tx->TLF01_details.tx_key == AUTH_CARD_UPDATE_RESPONSE )
   {
		  strcpy( new_string, "88060000" );/* TF Phani.. Dont Copy  */
   }
   else
   {
      /* Response */
      strcpy( new_string,p_auth_tx->receiving_inst_id_code );
   }
    
   /* If odd length, provide a leading zero. */
	if (1 == (HOST_MESSAGE.recv_inst_len[0] % 2) )
   {
      memset (temp_str, 0, sizeof(temp_str));
      temp_str [0] = '0';
      strcpy( &temp_str[1], new_string );
      memset (new_string, 0, sizeof(new_string));
      strcpy( new_string,temp_str);
   }

	HOST_MESSAGE.recv_inst_len[0] = strlen(new_string);
   asc_to_vbcd (new_string, sizeof(HOST_MESSAGE.recv_inst_id)*2,
                HOST_MESSAGE.recv_inst_id);

   /* FIELD 101: File Name for 0302 Txns */
   if ( Txn_Type == ONLINE_UPDATE )
   {
      i = strlen(FILE_NAME);
      HOST_MESSAGE.fname_len[0] = i;
      memcpy( HOST_MESSAGE.filename, FILE_NAME, i );
   }

   /* FIELD 120: Record Data */
   if ( Txn_Type == ONLINE_UPDATE )
   {
      /*+---------------------------------------------------+
        | Field 120 is record data for Online File Updates. |
        | Format is:                                        |
        |    Transaction Type ( 1 byte ) (Mandatory)        |
        |    Card Number      (16 bytes) (Mandatory)        |
        |    Action Code      ( 2 bytes) (Conditional)      |
        |    Purge Date       ( 6 bytes) (Conditional)      |
        |    Stop Regions     ( 5 bytes) (Conditional)      |
        |                                                   |
        | This order must be preserved.  In other words, if |
        | you have stop regions, you must have purge date   |
        | and action code.  If you have purge date, you     |
        | must have action code.                            |
        +---------------------------------------------------+*/

      memset( temp_str, 0x00, sizeof(temp_str) );
      temp_str[0] = Online_Updates.txn_type;
      memcpy( temp_str+1, Online_Updates.card_nbr, 16 );

      if ( Online_Updates.action_code[0] != 0x00 )
      {
         memcpy( temp_str+17, Online_Updates.action_code, 2 );

         if ( Online_Updates.purge_date[0] != 0x00 )
         {
            memcpy( temp_str+19, Online_Updates.purge_date, 6 );

            if ( Online_Updates.stop_regions[0] != 0x00 )
               memcpy( temp_str+25, Online_Updates.stop_regions, 5 );
         }
      }
      i = strlen( temp_str );
      HOST_MESSAGE.record_len[0] = 0;
      HOST_MESSAGE.record_len[1] = i;
      memcpy( HOST_MESSAGE.record, temp_str, i );
   }

   /* FIELD 127: Negative Data Access Code */
   if ( Txn_Type == ONLINE_UPDATE ||  p_auth_tx->TLF01_details.tx_key == AUTH_CARD_UPDATE_RESPONSE )
   {
      i = strlen(ncf01_i.neg_access_code);
      HOST_MESSAGE.access_cd_len[0] = 0;
      HOST_MESSAGE.access_cd_len[1] = i;
      memcpy( HOST_MESSAGE.access_cd, ncf01_i.neg_access_code, i );
   }

}// end Build_Host_Structure

/*****************************************************************************

  Function:    copy_to_req_resp_buffer

  Description: This function will copy the incoming request buffer to the 
               ReqRsp_Buffer.
  Author: 
      unknown
  Inputs:
      indata - incoming request buffer.
      msglen - the length of indata.
  Outputs:
      None
  Return values:
      None
  Modified by:
******************************************************************************/
void copy_to_req_resp_buffer (pCHAR indata, INT  msglen   )
{
   memset(ReqRsp_Buffer, 0, COMM_BUF_SZE);   /* zero out the buffer */
   buflen = msglen;
   reqidx = 0; 
   memcpy(ReqRsp_Buffer, indata, msglen);
   invalid_msg_format = false;
} /* end copy_to_req_resp_buffer */

/*****************************************************************************

  Function:    get_tran_type_2

  Description: This function will get the transaction type using the HOST_MESSAGE
               structure
  Author: 
      unknown
  Inputs:
      tx_key - the current transaction type
      HOST_MESSAGE (global) - the request buffer containing the message from the host
  Outputs:
      None
  Return values:
      BYTE - the calculated transaction type
  Modified by:
      FArad     8/13/98
******************************************************************************/
BYTE  get_tran_type_2(BYTE tx_key)
{

  /* Check if transaction is a financial response or an authorization */
  /* response.  These are sent from the MCard host as 0110's.         */
  if ( 0 == strncmp( "0110", HOST_MESSAGE.msgtype_bcd, 4 ) )
  {
    /* now verify the procesing code to know what kind */
	//The next line is changed by farad 8/13/98
    switch(HOST_MESSAGE.proccode[0])
	 {
      case 0x00:	
               if (AUTH_SALE == tx_key)
						return(AUTH_SALE_RESPONSE);
					else
						return(AUTH_AUTHORIZATION_RESPONSE);
					  break;

      case 0x20:	return(AUTH_REFUND_RESPONSE);
					  break;

      case 0x02:	return(AUTH_VOID_SALE_RESPONSE);
					  break;

      case 0x22:	return(AUTH_VOID_REFUND_RESPONSE);
					  break;

      case 0x01:	return(AUTH_CASH_ADVANCE_RESPONSE);
					  break;

      case 0x30:	return(AUTH_PRE_AUTHORIZATION_RESPONSE);
      					break;

      case 0x31: 	return(AUTH_BALANCE_INQUIRY_RESPONSE);
     					break;

 	  case 0x38:	return(AUTH_CARD_VERIFICATION_RESPONSE);
					  break;

	   case 0x40:	return(AUTH_PAYMENT_RESPONSE);
					  break;

      case 0x42:	return(AUTH_VOID_PAYMENT_RESPONSE);
					  break;

      case 0x51:	return(AUTH_CARD_VERIFICATION_RESPONSE);  // 38 from terminal, 51 from host
					  break;

     default:	return(AUTH_INVALID);
					  break;
    }
  }  
   /* or  an advise response */
  else if ( 0 == strncmp( "0230", HOST_MESSAGE.msgtype_bcd, 4 ) )
  {
    switch(HOST_MESSAGE.proccode[0])
    {
      case 0x00:	return(AUTH_OFFLINE_SALE_RESPONSE);
					  break;

      case 0x20:	return(AUTH_OFFLINE_REFUND_RESPONSE);
					  break;

      case 0x02:	return(AUTH_SALE_ADJUSTMENT_RESPONSE);
					  break;

      case 0x22:	return(AUTH_REFUND_ADJUSTMENT_RESPONSE);
					  break;

	  case 0x42:	return(AUTH_PAYMENT_ADJUSTMENT_RESPONSE);
					  break;

      default:	return(AUTH_INVALID);
					  break;
    }
  } 
  
  /* or a Reversal response */  
  else if ( 0 == strncmp( "0410", HOST_MESSAGE.msgtype_bcd, 4 ) )
	   return( AUTH_REVERSAL_RESPONSE );

  /* or a ATM confirmation  response */  
  else if ( 0 == strncmp( "01120", HOST_MESSAGE.msgtype_bcd, 4 ) )
	   return( AUTH_ATM_CONFIRM_RESPONSE );

  /* or a ATM confirmation  response */  
  else if ( 0 == strncmp( "0130", HOST_MESSAGE.msgtype_bcd, 4 ) )
	   return( AUTH_ADVICE_RESPONSE );

  /* or a ATM confirmation  response */  
  else if ( 0 == strncmp( "0430", HOST_MESSAGE.msgtype_bcd, 4 ) )
	   return( AUTH_REVERSAL_ADVICE_RESPONSE );

  /* or a statistics response */ 
  else if ( 0 == strncmp( "0810", HOST_MESSAGE.msgtype_bcd, 4 ) )
  {
      return(AUTH_LOGON_RESPONSE);
  }	

     /* Check for an Authorization request from another host */
   else if ( 0 == strncmp( "0100", HOST_MESSAGE.msgtype_bcd, 4 ) )
   {
      switch( HOST_MESSAGE.proccode[0] )
      {
         case 0x00:
            return( AUTH_AUTHORIZATION );
         break;

         case 0x30:
        		 return( AUTH_PRE_AUTHORIZATION );
         			break;

         case 0x31:

        		 return( AUTH_BALANCE_INQUIRY);
        	 break;
         case 0x38:	                               // 38 from terminal, 51 from host
            return(AUTH_CARD_VERIFICATION);
			break;

         case 0x01:
            return(AUTH_CASH_ADVANCE);
			break;

         default:
            return( AUTH_INVALID );
         break;
      }
   }

   /* Check for a Reversal request from another host */  
   else if ( 0 == strncmp( "0400", HOST_MESSAGE.msgtype_bcd, 4 ) )
      return( AUTH_REVERSAL );
		
  /* or a ATM confirmation  request */  
  else if ( 0 == strncmp( "0102", HOST_MESSAGE.msgtype_bcd, 4 ) )
	   return( AUTH_ATM_CONFIRM );

  /* or a ATM confirmation  request */  
  else if ( 0 == strncmp( "0120", HOST_MESSAGE.msgtype_bcd, 4 ) )
	   return( AUTH_ADVICE );

  else if ( 0 == strncmp( "0121", HOST_MESSAGE.msgtype_bcd, 4 ) )
	   return( AUTH_ADVICE_REPEAT );

  /* or a ATM confirmation  request */  
  else if ( 0 == strncmp( "0420", HOST_MESSAGE.msgtype_bcd, 4 ) )
	   return( AUTH_REVERSAL_ADVICE );

  else if ( 0 == strncmp( "0421", HOST_MESSAGE.msgtype_bcd, 4 ) )
	   return( AUTH_REVERSAL_ADVICE );

  else if ( 0 == strncmp( "0620", HOST_MESSAGE.msgtype_bcd, 4 ) )
	   return( AUTH_ADMIN_ADVICE );

   /* Check for a batch upload from another host */
   else if ( 0 == strncmp( "0320", HOST_MESSAGE.msgtype_bcd, 4 ) )
      return( AUTH_BATCH_UPLOAD ); 
 
   /* Check for a settlement transaction from another host */
   else if ( 0 == strncmp( "0500", HOST_MESSAGE.msgtype_bcd, 4 ) )
   {
      return( AUTH_INVALID );
   }

   /* Check for a statistics transaction from another host */
   else if ( 0 == strncmp( "0800", HOST_MESSAGE.msgtype_bcd, 4 ) )
   {
      return( AUTH_LOGON );
   }

   /*-----------------------------------------------------------------*/
   /* Check for response message types when dealing with other hosts. */
   /*-----------------------------------------------------------------*/
   else if ( 0 == strncmp( "0210", HOST_MESSAGE.msgtype_bcd, 4 ) )
   {
      /* 0210 */
      switch( HOST_MESSAGE.proccode[0] )
      {
         case 0x00: return( AUTH_SALE_RESPONSE );
                    break;

         case 0x20: return( AUTH_REFUND_RESPONSE );
                    break;

         case 0x02: return( AUTH_VOID_SALE_RESPONSE );
                    break;

         case 0x22: return( AUTH_VOID_REFUND_RESPONSE );
                    break;

         case 0x01: return( AUTH_CASH_ADVANCE_RESPONSE );
                    break;

         case 0x40: return( AUTH_PAYMENT_RESPONSE );
                    break;

         default:   return( AUTH_INVALID );
                    break;
      }
   }

   /* Online File Update */
   else if ( 0 == strncmp("0312", HOST_MESSAGE.msgtype_bcd, 4) )
      return( AUTH_CARD_UPDATE_RESPONSE );
	 /* Online File Update request from Cadience.. TF Phani */
  else if ( 0 == strncmp("0302", HOST_MESSAGE.msgtype_bcd, 4) )
      return( AUTH_CARD_UPDATE );
  
  /* or a transaction that the financial server does not understand */
  else
     return(AUTH_INVALID);
  
}

/*****************************************************************************

  Function:    get_tran_type

  Description: This function will gets the transaction type from a
                 message based on message type and processing code. 
  Author: 
      Dennis Irby
  Inputs:
      p_auth_tx - Transaction record
  Outputs:
      None
  Return values:
      None
  Modified by:
******************************************************************************/
BYTE  get_tran_type( pAUTH_TX p_auth_tx ) 
{
   INT  ProcCode;
   BYTE TranType;

   ProcCode = atoi( p_auth_tx->TLF01_details.processing_code );
   ProcCode = ProcCode / 10000;   /* Get just the 1st 2 digits. */
   TranType = p_auth_tx->TLF01_details.tx_key;

   /* Check if transaction is a financial response or an authorization */
   /* response.  These are sent from the MCard host as 0110's.         */
   if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0110" ) )
   {
      /* It is, now verify the procesing code to know what kind */
      switch( ProcCode )
      {
         case 0:
            if ( AUTH_SALE == TranType )
               return( AUTH_SALE_RESPONSE );
            else
               return( AUTH_AUTHORIZATION_RESPONSE);
         break;

         case 20:
            return( AUTH_REFUND_RESPONSE );
         break;

         case 2:
            return( AUTH_VOID_SALE_RESPONSE );
         break;

         case 22:
            return( AUTH_VOID_REFUND_RESPONSE );
         break;

//         case 0x11:	
//            return(AUTH_QUASI_CASH_RESPONSE);
//			break;

         case 1:
            return( AUTH_CASH_ADVANCE_RESPONSE );
         break;

         case 30:
        		 return( AUTH_PRE_AUTHORIZATION_RESPONSE );
         break;

         case 31:
        		 return( AUTH_BALANCE_INQUIRY_RESPONSE);
        	 break;

         case 40:
            return( AUTH_PAYMENT_RESPONSE );
         break;

         default:
            return( AUTH_INVALID );
         break;
      } /* End MCard's version of 0110's */
   }  
  
   /* Check for an advise response from MCard host */
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0230" ) )
   {
      switch( ProcCode )
      {

         case 00:
            return( AUTH_OFFLINE_SALE_RESPONSE );
         break;

         case 20:
            return( AUTH_OFFLINE_REFUND_RESPONSE );
         break;

         case 2:
            return( AUTH_SALE_ADJUSTMENT_RESPONSE );
         break;

         case 22:
            return( AUTH_REFUND_ADJUSTMENT_RESPONSE );
         break;

         case 42:
            return( AUTH_PAYMENT_ADJUSTMENT_RESPONSE );
         break;

         default:
            return( AUTH_INVALID );
         break;
      }
   } 
 
   /* Check for a Reversal response from MCard host */
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0410" ) )
      return( AUTH_REVERSAL_RESPONSE );

  /* or a ATM confirmation  response */  
  //The next line was modified by egable on 12/23/98
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0112" ) )
	   return( AUTH_ATM_CONFIRM_RESPONSE );

  /* or a ATM confirmation  response */  
  //The next line was modified by egable on 12/23/98
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0130" ) )
	   return( AUTH_ADVICE_RESPONSE );

  /* or a ATM confirmation  response */  
  //The next line was modified by egable on 12/23/98
  else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0430" ) )
	   return( AUTH_REVERSAL_ADVICE_RESPONSE );

   /* Check for a statistics response from MCard host */
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0810" ) )
   {
      return( AUTH_LOGON_RESPONSE );
   }	

   /* 0200 from another host */
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0200" ) )
   {
      switch( ProcCode )
      {
	     case 00:
	        return( AUTH_SALE );
         break;

         case 20:
            return( AUTH_REFUND );
         break;

         case 02:
            return( AUTH_VOID_SALE );
         break;

         case 22:
            return( AUTH_VOID_REFUND );
         break;

         case 01:
            return( AUTH_CASH_ADVANCE );
         break;

         case 40:
            return( AUTH_PAYMENT );
         break;

         default:
            return( AUTH_INVALID );
         break;
      }
   }

   /* Check for an advise request from another host */
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0220" ) )
   {
      switch( ProcCode )
      {
         case 00:
            return( AUTH_OFFLINE_SALE );
         break;

         case 20:
            return( AUTH_OFFLINE_REFUND );
         break;

         case 02:
            return( AUTH_SALE_ADJUSTMENT );
         break;

         case 22:
            return( AUTH_REFUND_ADJUSTMENT );
         break;

         case 42:
            return( AUTH_PAYMENT_ADJUSTMENT );
         break;

         default:
            return( AUTH_INVALID );
         break;
      }
   }
  
   /* Check for an Authorization request from another host */
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0100" ) )
   {
      switch( ProcCode )
      {
         case 00:
            return( AUTH_AUTHORIZATION );
         break;

         case 30:
        		 return( AUTH_PRE_AUTHORIZATION );
         break;

         case 31:
        	 return( AUTH_BALANCE_INQUIRY);
		break;
        
		 case 38:	                               // 38 from terminal, 51 from host
            return(AUTH_CARD_VERIFICATION);
			break;
			//TF PHANI ..Added case for CASH_ADVANCE 
		case 01:	
            return(AUTH_CASH_ADVANCE);
			break;

///      case 0x11:	return(AUTH_QUASI_CASH);
///					  break;

         default:
            return( AUTH_INVALID );
         break;
      }
   }

   /* Check for a Reversal request from another host */  
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0400" ) )
      return( AUTH_REVERSAL );

   /* or a ATM confirmation  request */  
  //The next line was modified by egable on 12/23/98
  else if ( 0 == strncmp( "0102", HOST_MESSAGE.msgtype_bcd, 4 ) )
	   return( AUTH_ATM_CONFIRM );

  /* or a ATM confirmation  request */  
  //The next line was modified by egable on 12/23/98
  else if ( 0 == strncmp( "0120", HOST_MESSAGE.msgtype_bcd, 4 ) )
      return( AUTH_ADVICE );
  // TF Phani ..it can be the 0120 coming from JCB and forwarded to cadience
   else if ( 0 == strncmp( "0120", p_auth_tx->TLF01_details.message_type, 4 ) )
      return( AUTH_ADVICE );

  else if ( 0 == strncmp( "0121", HOST_MESSAGE.msgtype_bcd, 4 ) )
      return( AUTH_ADVICE_REPEAT );

  /* or a ATM confirmation  request */  
  //The next line was modified by egable on 12/23/98
  else if ( 0 == strncmp( "0420", HOST_MESSAGE.msgtype_bcd, 4 ) )
	   return( AUTH_REVERSAL_ADVICE );

  else if ( 0 == strncmp( "0421", HOST_MESSAGE.msgtype_bcd, 4 ) )
	   return( AUTH_REVERSAL_ADVICE );

  else if ( 0 == strncmp( "0620", HOST_MESSAGE.msgtype_bcd, 4 ) )
	   return( AUTH_ADMIN_ADVICE );

   /* Check for a batch upload from another host */
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0320" ) )
      return( AUTH_BATCH_UPLOAD ); 
 
   /* Check for a settlement transaction from another host */
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0500" ) )
   {
/*      switch( ProcCode )
      {
         case 92:
            return( AUTH_SETTLE );
         break;

         case 96:
            return( AUTH_SETTLE_TRAILER );
         break;

         default: */
            return( AUTH_INVALID );
/*       break; 
      }  */
   }

   /* Check for a statistics transaction from another host */
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0800" ) )
   {
      return( AUTH_LOGON );
   }

   /*-----------------------------------------------------------------*/
   /* Check for response message types when dealing with other hosts. */
   /*-----------------------------------------------------------------*/
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0210" ) )
   {
      /* 0210 */
      switch( ProcCode )
      {
         case 00: return( AUTH_SALE_RESPONSE );
                    break;

         case 20: return( AUTH_REFUND_RESPONSE );
                    break;

         case 02: return( AUTH_VOID_SALE_RESPONSE );
                    break;

         case 22: return( AUTH_VOID_REFUND_RESPONSE );
                    break;

         case 01: return( AUTH_CASH_ADVANCE_RESPONSE );
                    break;

         case 40: return( AUTH_PAYMENT_RESPONSE );
                    break;

         default:   return( AUTH_INVALID );
                    break;
      }
   }  
   /* 0230 */
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0230" ) )
   {
      switch( ProcCode )
      {
         case 00: return( AUTH_OFFLINE_SALE_RESPONSE );
                    break;

         case 20: return( AUTH_OFFLINE_REFUND_RESPONSE );
                    break;

         case 02: return( AUTH_SALE_ADJUSTMENT_RESPONSE );
                    break;

         case 22: return( AUTH_REFUND_ADJUSTMENT_RESPONSE );
                    break;

         case 42: return( AUTH_PAYMENT_ADJUSTMENT_RESPONSE );
                    break;

         default:   return( AUTH_INVALID );
                    break;
      }
   } 

   /* 0110 */
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0110" ) )
   {
      switch( ProcCode )
      {
         case 00: return( AUTH_AUTHORIZATION_RESPONSE );
                    break;

         case 30: return( AUTH_PRE_AUTHORIZATION_RESPONSE );
                    break;

         case 31: return(AUTH_BALANCE_INQUIRY_RESPONSE );
                    break;

         default:   return( AUTH_INVALID );
                    break;
      }
   }

   /* 0410 */
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0410" ) )
      return( AUTH_REVERSAL_RESPONSE );

   /* 0810 */ 
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0810" ) )
   {
      return( AUTH_LOGON_RESPONSE );
   }

   /* Type Unknown */
   else
      return( AUTH_INVALID );
}


/*****************************************************************************

  Function:    str_asc_to_bcd

  Description: This function will convert an ascii string to bcd.
  Author: 
      unknown
  Inputs:
      p_astr - a pointer to the ascii string to convert
      how_many - number of characters in p_astr to convert
  Outputs:
      bstring - the bcd string 
  Return values:
      None
  Modified by:
      FArad     8/14/98
******************************************************************************/
static  void str_asc_to_bcd (p_astr, how_many, bstring)
   pCHAR        p_astr;
   INT          how_many;
   BYTE			 bstring[];
{
   UINT  i = 0;
   UINT  start;

   if (strlen(p_astr) > (UINT)how_many)
      start = strlen(p_astr) - how_many;
   else
      start = 0;

   //for (i = (strlen(p_astr) - how_many); i < strlen(p_astr); i=i+2)
   for (i = start; i < strlen(p_astr); i=i+2)
   { 
      /* if I hit a null terminator, quit.  */
      if ('\0' == p_astr[i])
         break;

      if (p_astr[i] != ' ')
         bstring[i/2] = ((p_astr[i]-'0')*16);

      if (((i+1) < strlen(p_astr)) && (p_astr[i+1] != ' ') )
		   bstring[i/2] += (p_astr[i+1]-'0');

   }

} /* str_asc_to_bcd */

/*****************************************************************************

  Function:    asc_to_bcd
  Description: This function will convert an ascii string to bcd.
  Author: 
      unknown
  Inputs:
      afield - a pointer to the ascii string to convert
      how_many - number of characters in p_astr to convert
  Outputs:
      bstring - the bcd string 
  Return values:
      None
  Modified by:
      FArad      8/14/98
******************************************************************************/
static  void asc_to_bcd (afield, how_many, bstring)
   pCHAR         afield;
   INT           how_many;
   BYTE          bstring[];
{  
   pCHAR  astr;
   UINT   i = 0;

   astr = afield;
   
   for (i = (strlen(afield) - how_many); i < strlen(afield); i=i+2)
   {
      if ('\0' == astr [i])
         break;

      if (astr[i] != ' ')
         bstring[i/2] = ((astr[i]-'0')*16);

      if (((i+1) < strlen(afield)) && (astr[i+1] != ' '))
         bstring[i/2] += (astr[i+1]-'0');

   }
   
} /* asc_to_bcd */

/*****************************************************************************

  Function:    

  Description: This function will convert a variable length ascii string to bcd.
  Author: 
      unknown
  Inputs:
      afield - a pointer to the ascii string to convert
      fld_len - the length of the field
  Outputs:
      bstring - the bcd string 
  Return values:
      None
  Modified by:
      FArad      8/14/98
******************************************************************************/
static  void asc_to_vbcd (afield, fld_len, bcd_buf)
   pCHAR         afield;
   INT           fld_len;   
   BYTE bcd_buf[];
{
   pCHAR astr;
   INT  i = 0;
   INT  temp;
   

   temp = strlen(afield);
   if (1==(temp%2))
      strcpy(afield+temp,"0");

   astr = afield;

   for (i = 0; i < fld_len; i=i+2)
      {
      /* if I hit a null terminator, quit.  */
      if ('\0' == astr[i])
         break;

      bcd_buf [i/2] = ((astr[i]-'0')*16);

      if ((i+1) < fld_len)
         bcd_buf [i/2] += (astr[i+1]-'0');
      else
         bcd_buf [i/2] += 0x0f;
      }
} /* asc_to_vbcd */

/*****************************************************************************

  Function:    blank_string

  Description: This function will determine if a string is blank
  Author: 
      unknown
  Inputs:
      astr - the string to test
      max_len - the length of the string to test.
  Outputs:
      None
  Return values:
      true - the string is blank
      false - the string is not blank
  Modified by:
******************************************************************************/
BOOLEAN blank_string(pCHAR astr, INT max_len)
{
   int i;
   for(i = 0;i < max_len; i++)
   {
      if (astr[i] != ' ')
         return(false);
   }
   return(true);    
}

/*****************************************************************************

  Function:    blank_or_null_string

  Description: This function will determine if a string is blank or contains
               only nulls
  Author: 
      E. Gable  3/7/99
  Inputs:
      astr - the string to test
      max_len - the length of the string to test.
  Outputs:
      None
  Return values:
      true - the string is blank
      false - the string is not blank
  Modified by:
******************************************************************************/
BOOLEAN blank_or_null_string(pCHAR astr, INT max_len)
{
   int i;
   for(i = 0;i < max_len; i++)
   {
      if ((astr[i] != ' ') && (astr[i] != '\0'))
         return(false);
   }
   return(true);    
}

/*****************************************************************************

  Function:    hhutil_get_strtime

  Description: This function will get the start time
  Author: 
      unknown
  Inputs:
      None
  Outputs:
      deststr - the destination of the start time.
  Return values:
      None
  Modified by:
******************************************************************************/
void hhutil_get_strtime ( pCHAR deststr )
{
   time_t seconds;

   seconds = time(NULL_PTR);        /* get the current date and time in seconds */
   itoa(seconds, deststr, 10);    /* convert the long integer to a string */
}

/*****************************************************************************

  Function:    hhutil_verify_pin_block

  Description: This function will determine whether or not there is a pin block
               in the auth_tx
  Author: 
      unknown
  Inputs:
      p_auth_tx - the auth_tx structure
  Outputs:
      None
  Return values:
      true  - pin block
      false - no pin block 
  Modified by:
      BGable      6/30/98
******************************************************************************/
BOOLEAN hhutil_verify_pin_block(pAUTH_TX p_auth_tx)
{

   if(0 == strlen(p_auth_tx->TLF01_details.pin_block))
      return(false);
   if(blank_string(p_auth_tx->TLF01_details.pin_block, strlen(p_auth_tx->TLF01_details.pin_block)))
      return(false);
   return(true);
}

/*****************************************************************************

  Function:    hhutil_process_debit_transaction

  Description: This function will request pin translation.  (The pin translation
               will convert the pin that is encrypted using the terminal key to 
               a pin encrypted using the host key).
  Author: 
      unknown
  Inputs:
      p_auth_tx - pointer to the auth_tx structure 
  Outputs:
      None
  Return values:
      true  - successful 
      false - an error occurred when trying to send message to security function.
  Modified by:
******************************************************************************/
BOOLEAN hhutil_process_debit_transaction(pAUTH_TX p_auth_tx)
{
   pPTE_MSG	 p_msg = NULL_PTR;
   LONG      ret_code;
   CHAR	    Buffer[512]={0};
   CHAR	    ErrorMsg[512]={0};
   BYTE      buffer1[sizeof(AUTH_TX)+sizeof(NCF01)];
   BOOLEAN   ret_val = true;

 
   /* Build message for crypto. */
   memcpy( buffer1,p_auth_tx, Auth_Tx_Size );
   memcpy( buffer1+Auth_Tx_Size,&ncf01_i, Ncf01_Size );

   p_msg  =    ptemsg_build_msg( MT_NSP_TRANSLATE_PIN, 
                                 ST1_NONE,
                                 ST2_NONE,
                                 security_que_name, 
                                 application_que_name,
                                 (pBYTE)&buffer1,
                                 sizeof(buffer1), 
                                 0 );
   if ( NULL_PTR == p_msg )
	{
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Unable to translate pin. Err: Insufficient Memory to build PTE message Term Id:%s ,Merch Id: %s RRN: %s ",
      					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      ncjcb2_log_message( 3, 2, Buffer , "hhutil_process_debit_transaction" , 0 ,p_auth_tx->TLF01_details.card_num);

		ret_val = false;
	}
   else
   {
      ret_code = pteipc_send(p_msg, security_que_name);
      if( ret_code < 0 )
      {
         pteipc_get_errormsg( ret_code, ErrorMsg );
         sprintf( Buffer,
             "Unable to translate pin. Err: error on pteipc_send to que %s: %s",
              security_que_name, ErrorMsg );
         memcpy( p_auth_tx->TLF01_details.response_text, Buffer, 40 );
         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
         					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
         strcat(Buffer,Log_Bufr);
         ncjcb2_log_message( 3, 2, Buffer , "hhutil_process_debit_transaction" , 0 ,p_auth_tx->TLF01_details.card_num);
         ret_val = false;
      }
      free( p_msg );
   }
   return( ret_val );
}

/*****************************************************************************

  Function:    hhutil_transfer_ctl_to_auth_app

  Description: This function will transfer the auth_tx to the authorizer.  
  Author: 
      unknown
  Inputs:
      p_auth_tx - pointer to the auth_tx structure
  Outputs:
      None
  Return values:
      true - successful
      false - unsuccessful
  Modified by:
******************************************************************************/
BOOLEAN hhutil_transfer_ctl_to_auth_app ( pAUTH_TX p_auth_tx)
{
   pPTE_MSG	p_msg_host_out = NULL_PTR;
   LONG     ret_code;
   CHAR     Buffer[512]={0};
   CHAR     ErrorMsg[512]={0};

   p_msg_host_out = ptemsg_build_msg (MT_AUTH_REQUEST,
                                      ST1_NONE, 
                                      ST2_NONE, 
                                      authorizer_que_name,
                                      application_que_name,
                                      (pBYTE)p_auth_tx,
                                      Auth_Tx_Size,
                                      0 );
   if ( NULL_PTR == p_msg_host_out )
   {
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Insufficient Memory to build PTE message Term Id:%s ,Merch Id: %s RRN: %s ",
      					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncjcb2_log_message( 3, 2, Buffer , "hhutil_transfer_ctl_to_auth_app" , 0 ,p_auth_tx->TLF01_details.card_num);
      return(false);
   }
   ret_code = pteipc_send(p_msg_host_out, authorizer_que_name);
   if( ret_code < 0 )
   {
      pteipc_get_errormsg( ret_code, ErrorMsg );
      sprintf( Buffer,
              "hhutil_transfer_ctl: pteipc error sending to que %s: %s",
               authorizer_que_name, ErrorMsg );
      memcpy( p_auth_tx->TLF01_details.response_text, Buffer, 40 );
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncjcb2_log_message( 3, 2, Buffer , "hhutil_transfer_ctl_to_auth_app" , 0 ,p_auth_tx->TLF01_details.card_num);
      free( p_msg_host_out );
      return(false);
   }
	
   free( p_msg_host_out );
   return(true);
}

/*****************************************************************************

  Function:    hhutil_process_host_request

  Description: This function will process a host request by filling the auth_tx
               structure with data.
  Author: 
      unknown
  Inputs:
      p_auth_tx - pointer to the auth_tx structure
  Outputs:
      p_auth_tx - pointer to the auth_tx structure
  Return values:
      true - successful
      false - unsuccessful
  Modified by:
******************************************************************************/
BYTE hhutil_process_host_request(pAUTH_TX p_auth_tx)
{
   char           unique_tran_str_id[21] = {0};
   CHAR           ErrorMsg[512] = {0};
   char           temp[17] = {0};
   pPTE_MSG       p_msg_ins = NULL_PTR;
   NCF30          ncf30;
   CHAR           Buff[512]={0};
   BYTE           buffer1[sizeof(AUTH_TX)+sizeof(NCF30)];
   LONG           ret_code; 

	PRINT("hhutil_process_host_request: RECEIVED DIALOG MESSAGE\n");

   ptetime_get_strtime(unique_tran_str_id);

   strcpy(p_auth_tx->TLF01_details.handler_queue, application_que_name);
   strcpy(p_auth_tx->TLF01_details.dispensation.origin,"RO");
   memset(p_auth_tx->TLF01_details.primary_key.transaction_id, 0, sizeof(p_auth_tx->TLF01_details.primary_key.transaction_id));
   memcpy(p_auth_tx->TLF01_details.primary_key.transaction_id, p_auth_tx->TLF01_details.terminal_id, sizeof(p_auth_tx->TLF01_details.terminal_id) - 1);
   strncat(p_auth_tx->TLF01_details.primary_key.transaction_id, unique_tran_str_id+2, 12);
   p_auth_tx->TLF01_details.general_status = GS_TXN_NEW;
   p_auth_tx->TLF01_details.entry_type = FINANCIAL;
	/* TF Phani Process online update 0302 message */
   if(strncmp(p_auth_tx->TLF01_details.message_type,"0302",4) == 0)
  {
	 process_0302_from_host(p_auth_tx);
	
	return true;
   }
   // Originally the currency code must be USD, it will be converted to PESOs only if it is USD
   if (0 == strncmp(p_auth_tx->TLF01_details.response_code, "91", 2))  
   {
      strcpy( p_auth_tx->TLF01_details.response_text,
             "Currency code is not US dollars" );
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Currency code is not US dollars  Term Id:%s ,Merch Id: %s RRN: %s ",
      					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      ncjcb2_log_message( 3, 1, Log_Bufr , "hhutil_process_host_request" , 0 ,p_auth_tx->TLF01_details.card_num);
      perform_error_processing( p_auth_tx );
      return(false);
   }
   if ( (AUTH_REVERSAL_ADVICE != p_auth_tx->TLF01_details.tx_key) &&
        (AUTH_REVERSAL_REPEAT != p_auth_tx->TLF01_details.tx_key)  )
   {
      // If a PIN is required, verify that one was entered
      if ('1' == p_auth_tx->TLF01_details.processing_code[1])
      {
         /* This is an ATM message. Pin is required. */
         if ( false == hhutil_verify_pin_block(p_auth_tx) )
         {
            /* Invalid ATM Message - No PIN.  Decline and return to sender. */
            strcpy( p_auth_tx->TLF01_details.response_code, INVALID_PIN );
            p_auth_tx->TLF01_details.tx_key += 100;

            /* Build response message for the host. */
            build_host_structure(p_auth_tx);
            insert_tlf01(p_auth_tx);
            if (true == Build_Host_Request(p_auth_tx))
            {
               if(false == hhutil_send_host_response(p_auth_tx))
               {
                  sprintf( Buff,
                          "Unable to send %s msg to host.",
                           p_auth_tx->TLF01_details.message_type );
                  memcpy( p_auth_tx->TLF01_details.response_text, Buff, 40 );
                  memset(Log_Bufr,0x00,sizeof(Log_Bufr));
                  sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
                  					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
                  strcat(Buff,Log_Bufr);
                  ncjcb2_log_message( 3, 2, Buff , "hhutil_process_host_request" , 0 ,p_auth_tx->TLF01_details.card_num);
                  return(false);
               }
            }
            else
            {
               memset(Log_Bufr,0x00,sizeof(Log_Bufr));
               sprintf(Log_Bufr," Error building response for the host Term Id:%s ,Merch Id: %s RRN: %s ",
               					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
               ncjcb2_log_message( 3, 2, Log_Bufr , "hhutil_process_host_request" , 0 ,p_auth_tx->TLF01_details.card_num);
               return(false);
            }
         }
      }
   }
   if ( p_auth_tx->TLF01_details.date_yyyymmdd[0] == 0x00 )
   {
      genutil_format_date(temp);
      memcpy(p_auth_tx->TLF01_details.date_yyyymmdd, temp, 8); 
      memcpy(p_auth_tx->TLF01_details.time_hhmmss, temp + 8, 6);
   }
   else if ( 0 == strncmp( "0000",p_auth_tx->TLF01_details.date_yyyymmdd+4,4) )
   {
      genutil_format_date(temp);
      memcpy(p_auth_tx->TLF01_details.date_yyyymmdd, temp, 8); 
      memcpy(p_auth_tx->TLF01_details.time_hhmmss, temp + 8, 6);
   }

   p_auth_tx->TLF01_details.verify_mag_stripe[0] = 'Y';  // tell the authorizer to check cvv

   if (AUTH_REVERSAL_ADVICE == p_auth_tx->TLF01_details.tx_key)
   {
      p_auth_tx->TLF01_details.tx_key = AUTH_REVERSAL;
      strcpy(p_auth_tx->TLF01_details.message_type, "0400");

      /* Display reversal message on Monitor.
       * This message is used to isolate problems to selected bins
       * or to certain merchants or to networks.
       */
      memset( Buff, 0x00, sizeof(Buff) );
      sprintf( Buff,
              "Reversal from JCB. Card %s, Merchant %s",
               p_auth_tx->TLF01_details.card_num,
               p_auth_tx->TLF01_details.merchant_id );

      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buff,Log_Bufr);
      ncjcb2_log_message( 2, 1, Buff , "hhutil_process_host_request" , 0 ,p_auth_tx->TLF01_details.card_num);

      // Must get the original transaction for a reversal.  If the original transaction is
      // not found, reply to the host and don't send the transaction to the authorizer. 
      memset( &ncf30, 0x00, Ncf30_Size );
      strcpy( ncf30.primary_key.network_id, AppName);
      strcpy( ncf30.primary_key.network_type, "A");
      strcpy( ncf30.card_nbr, p_auth_tx->TLF01_details.card_num );
      strcpy( ncf30.tran_amount, p_auth_tx->TLF01_details.total_amount );
      strcpy( ncf30.sys_trace_audit_nbr,p_auth_tx->TLF01_details.orig_sys_trace_num );

      // Build message to get the original transaction from data server. 
      memcpy( buffer1,p_auth_tx, Auth_Tx_Size );
      memcpy( buffer1+Auth_Tx_Size,&ncf30, Ncf30_Size );


      
      p_msg_ins = ptemsg_build_msg( MT_DB_REQUEST, 
                                    ST1_DB_SELECT, 
                                    ST2_DB_SELECT_TRAN_BY_STAN,
                                    netds_que_name, 
                                    application_que_name,
                                    (pBYTE)&buffer1,
                                    sizeof(buffer1), 
                                    NCF30_DATA );
      if ( NULL_PTR == p_msg_ins  )
      {
         ncjcb2_log_message( 3, 2, "Insufficient Memory to build PTE message: select ncf30" , "hhutil_process_host_request" , 0 ,"");
         free( p_msg_ins );
         return( false );
      }

      // Send message to get host specific data from data server. 
      ret_code = pteipc_send( p_msg_ins, netds_que_name );
      free( p_msg_ins );
      if ( 0 > ret_code )
      {
         pteipc_get_errormsg( ret_code, ErrorMsg );
         sprintf( Buff,"pteipc error sending to que: %s. %s",
                  netds_que_name, ErrorMsg );
         memcpy( p_auth_tx->TLF01_details.response_text, Buff, 40 );
         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
         					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
         strcat(Buff,Log_Bufr);
         ncjcb2_log_message( 3, 2, Buff , "hhutil_process_host_request" , 0 ,p_auth_tx->TLF01_details.card_num);
         return( false );
      }
      return( true );
   }

   /* Validate Merchant Category Code (MCC) */
   ret_code = validate_mcc( p_auth_tx );
   if ( ret_code == true )
   {
      /* Process the authorization request from the host. */
      if (false == hhutil_transfer_ctl_to_auth_app(p_auth_tx))
      {
         sprintf( Buff,
                 "Unable to forward txn to authorizer, %s.",
                  authorizer_que_name );
         memcpy( p_auth_tx->TLF01_details.response_text, Buff, 40 );
         ncjcb2_log_message( 3, 2, Buff , "hhutil_process_host_request" , 0 ,"");
         return(false);
      }
   }
   else
   {
      /* Invalid MCC.  Decline and return to sender. */
      strcpy( p_auth_tx->TLF01_details.response_code, CALL_ISSUER );
      p_auth_tx->TLF01_details.tx_key += 100;

      /* Build response message for the host. */
      build_host_structure(p_auth_tx);
      insert_tlf01(p_auth_tx);
      if (true == Build_Host_Request(p_auth_tx))
      {
         if(false == hhutil_send_host_response(p_auth_tx))
         {
            sprintf( Buff,
                  "Unable to send %s msg to host.",
                   p_auth_tx->TLF01_details.message_type );
            memcpy( p_auth_tx->TLF01_details.response_text, Buff, 40 );
            ncjcb2_log_message( 3, 2, Buff , "hhutil_process_host_request" , 0 ,"");
            return(false);
         }
      }
      else
      {
         ncjcb2_log_message( 3, 2, "Error building response for the host" , "hhutil_process_host_request" , 0 ,"");
         return(false);
      }
   }
   return(true);
}

/*****************************************************************************

  Function:    hhutil_send_host_response

  Description: This function will send the host response message out to the 
               JCB host.
  Author: 
      unknown
  Inputs:
      p_auth_tx - the auth_tx structure, used to get the tpdu_id 
  Outputs:
      None
  Return values:
      true - successful
      false - unsuccessful
  Modified by:
******************************************************************************/
BOOLEAN hhutil_send_host_response(pAUTH_TX p_auth_tx)
{
   pPTE_MSG	p_msg_out;
   LONG   ret_code;
   CHAR	 Buffer[256] = {0};
   CHAR	 ErrorMsg[100] = {0};

   p_msg_out = ptemsg_build_msg(MT_OUTGOING,
	         		            ST1_NONE,
						            ST2_NONE,
						            "dialogA",
						            application_que_name,
                              ReqRsp_Buffer,
                              buflen,
						            0); 

   if ( NULL_PTR == p_msg_out )
	{
       ncjcb2_log_message( 3, 2, "Insufficient Memory to build PTE message" , "hhutil_send_host_response" , 0 ,"");
	   return(false);
	}
   ptemsg_set_orig_comms_info(p_msg_out, p_auth_tx->TLF01_details.tpdu_id);
	ret_code = pteipc_send(p_msg_out, "dialogA");
	if( ret_code < 0 )
	{
	   pteipc_get_errormsg( ret_code, ErrorMsg );
      sprintf( Buffer, "pteipc error sending to que %s: %s", "dialogA", ErrorMsg );
      memcpy( p_auth_tx->TLF01_details.response_text, Buffer, 40 );
      ncjcb2_log_message( 3, 2, Buffer , "hhutil_send_host_response" , 0 ,"");
      free( p_msg_out );
      return(false);
	}
   else
   {
      /* Update transaction statistics. */
      update_txn_stats( &Txn_Counts, p_auth_tx, SENT );

      /* Log transaction to trace file. */
      write_to_txn_file();
   }
   free( p_msg_out );
   
   return(true);
}

/*****************************************************************************

  Function:    format_date

  Description: This function accepts a pointer to an empty character
               string.  The string will be filled by this function.  It
               will fill it with a date in the format:yyyymmddhhmmss
  Author: 
      unknown
  Inputs:
      None
  Outputs:
  temp     -  String containing system date:  YYYYMMDDhhmmss
  Return values:
      None
  Modified by:
******************************************************************************/
/*void format_date(pCHAR temp)
{

// NOTE:  This function uses the tm structure.  This should be changed to use the PTETIME function
//        Using this function will cause problems after year 2000
   
   struct   _timeb   time_structure;// Time for ftime
   time_t long_time_since_1970;     // Number of seconds since 01/01/1970 A.D. 12:00 A.M.
   struct tm *current_time;         // Current time structure
   char temp1[4] = "0";

   _ftime(&time_structure);
   long_time_since_1970 = time_structure.time;
   current_time = localtime(&long_time_since_1970);
   current_time->tm_mon++;
   if (current_time->tm_year >= 70)
      current_time->tm_year += 1900;
   else
      current_time->tm_year += 2000;
   itoa(current_time->tm_year, temp, 10);
   itoa(current_time->tm_mon, temp1 + 1, 10);
   if(2 == strlen(temp1))
      memcpy(temp + 4, temp1, 2);
   else
      memcpy(temp + 4, temp1 + 1, 2);
   memset(temp1, 0, sizeof(temp1));
   temp1[0] = '0';
   itoa(current_time->tm_mday, temp1 + 1, 10);
   if(2 == strlen(temp1))
      memcpy(temp + 6, temp1, 2);
   else
      memcpy(temp + 6, temp1 + 1, 2);
   memset(temp1, 0, sizeof(temp1));
   temp1[0] = '0';
   itoa(current_time->tm_hour, temp1 + 1, 10);
   if(2 == strlen(temp1))
      memcpy(temp + 8, temp1, 2);
   else
      memcpy(temp + 8, temp1 + 1, 2);
   memset(temp1, 0, sizeof(temp1));
   temp1[0] = '0';
   itoa(current_time->tm_min, temp1 + 1, 10);
   if(2 == strlen(temp1))
      memcpy(temp + 10, temp1, 2);
   else
      memcpy(temp + 10, temp1 + 1, 2);
   memset(temp1, 0, sizeof(temp1));
   temp1[0] = '0';
   itoa(current_time->tm_sec, temp1 + 1, 10);
   if(2 == strlen(temp1))
      memcpy(temp + 12, temp1, 2);
   else
      memcpy(temp + 12, temp1 + 1, 2);

}*/

/*****************************************************************************

  Function:    hhutil_send_retry_response_to_host

  Description: This function will 
  Author: 
      unknown
  Inputs:
      None
  Outputs:
      None
  Return values:
      None
  Modified by:
******************************************************************************/
/*void hhutil_send_retry_response_to_host(pCHAR code, pAUTH_TX p_auth_tx)
{
   char temp[17] = "";
   memcpy(p_auth_tx->TLF01_details.response_code, code, 2);
   //auth_tx.TLF01_details.tx_key = thutil_match_request_to_response(auth_tx.TLF01_details.tx_key);
   memset(temp, 0, sizeof(temp));
   format_date(temp);
   memcpy(p_auth_tx->TLF01_details.retrieval_ref_num, temp + 2, 12);
   memcpy(p_auth_tx->TLF01_details.date_yyyymmdd, temp, 8);
   memcpy(p_auth_tx->TLF01_details.time_hhmmss, temp + 8, 6);
   build_host_structure(p_auth_tx);
   Build_Host_Request(p_auth_tx);
   Build_Host_Header(p_auth_tx);
   hhutil_send_host_response(p_auth_tx);
}*/

/*****************************************************************************

  Function:    incoming_other_host_response

  Description: This function will process an incoming response from another host
               that needs to go to the JCB host.
  Author: 
      unknown
  Inputs:
      p_auth_tx - a pointer to the auth_tx of the response message
  Outputs:
      None
  Return values:
      true - successful
      false - unsuccessful
  Modified by:
******************************************************************************/
INT incoming_other_host_response(pAUTH_TX p_auth_tx)
{
   INT    ret_code;
   NCF30  ncf30;

   /* Responses to reversals and advices must be approved. */
   if ( (AUTH_REVERSAL_ADVICE_RESPONSE == p_auth_tx->TLF01_details.tx_key) ||
        (AUTH_REVERSAL_RESPONSE        == p_auth_tx->TLF01_details.tx_key) ||
        (AUTH_REVERSAL_REPEAT_RESPONSE == p_auth_tx->TLF01_details.tx_key)  )
   {
      strcpy( p_auth_tx->TLF01_details.response_code, "00" );

      memset( &ncf30, 0x00, Ncf30_Size );
      strcpy( ncf30.primary_key.network_id, AppName );
      strcpy( ncf30.primary_key.network_type, "A" );
      strcpy( ncf30.primary_key.batch_number, "ACTIVE" );
      strcpy( ncf30.primary_key.retrieval_ref_num,
              p_auth_tx->TLF01_details.orig_retrieval_ref_num );

      ret_code = delete_orig_ncf30( ncf30, p_auth_tx );
      if ( false == ret_code )
      {
         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr," Unable to delete reversal's original txn in NCF30 Term Id:%s ,Merch Id: %s RRN: %s ",
         					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
         ncjcb2_log_message( 3, 2, Log_Bufr , "incoming_other_host_response" , 0 ,p_auth_tx->TLF01_details.card_num);
      }
   }

   /* Update NCF21 and insert into NCF30.
    * But do it only for approved authorizations.
    */
   if ( 0 == strcmp( "00", p_auth_tx->TLF01_details.response_code ) )
   {
      if ( 0 != strncmp( "08", p_auth_tx->TLF01_details.message_type, 2 ) )
      {
         if ( 0 != strncmp( "04", p_auth_tx->TLF01_details.message_type, 2 ) )
          /* Send record to NCF21 and NCF30 to track totals. */
         (void)recon_update( p_auth_tx, ST2_NCF30_INSERT, "A" );
      }
   }

   /* Continue to send response to host. */
   return( incoming_other_host_response_db_reply( p_auth_tx ) );
}
/******************************************************************************
 *
 *  NAME:         incoming_other_host_response_db_reply
 *
 *  DESCRIPTION:  This function processes a response for a host.  The request
 *                from the host is stored in p_auth_tx.  This function picks
 *                up where 'incoming_other_host_response' left off.  You come
 *                here after updating NCF21 and NCF30 if they required updating.
 *
 *  INPUTS:       p_auth_tx - Data structure containing transaction record.
 *                
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false is a problem occurred
 *
 *  AUTHOR:       D. Irby
 *
 *  MODIFICATIONS:Durga 
                  Added the functionality to generate txnKey before it sends a request
                  to insert into TLF01 table
 ******************************************************************************/
INT incoming_other_host_response_db_reply( pAUTH_TX p_auth_tx)
{
   pPTE_MSG   p_msg_ins = NULL_PTR;
   CHAR       temp[3] = "";
   CHAR       tran_finish_time[17] = ""; 
   CHAR       err_mesg[200] = "";
   CHAR       temp_amount[13];
   CHAR       temp_rev_amount[13];
   CHAR       temp_cur_code[4];
   char       unique_tran_str_id[17] = ""; //12/7/99
   INT		  iRet_code = true;


   PRINT("incoming_host_response\n");
   
   if ((AUTH_REVERSAL_RESPONSE == p_auth_tx->TLF01_details.tx_key) || (AUTH_REVERSAL_REPEAT_RESPONSE == p_auth_tx->TLF01_details.tx_key))
   {
      p_auth_tx->TLF01_details.tx_key = AUTH_REVERSAL_ADVICE_RESPONSE;
      strcpy(p_auth_tx->TLF01_details.message_type,"0430");
   }

   if (AUTH_ADVICE_REPEAT_RESPONSE == p_auth_tx->TLF01_details.tx_key)
   {
      p_auth_tx->TLF01_details.tx_key = AUTH_ADVICE_RESPONSE;
      strcpy(p_auth_tx->TLF01_details.message_type,"0130");
   }

   // Save the currency code and amount because the database needs the auth curr code and amount
   strncpy(temp_cur_code, p_auth_tx->TLF01_details.currency_code, sizeof(p_auth_tx->TLF01_details.currency_code));
   strncpy(temp_amount, p_auth_tx->TLF01_details.total_amount, sizeof(p_auth_tx->TLF01_details.total_amount));
   strncpy(temp_rev_amount, p_auth_tx->TLF01_details.reversal_amount, sizeof(p_auth_tx->TLF01_details.reversal_amount));

   // Send the original currency code and amount to the host.
   strncpy(p_auth_tx->TLF01_details.currency_code,p_auth_tx->reconciliation_cur_code, sizeof(p_auth_tx->TLF01_details.currency_code));
   strncpy(p_auth_tx->TLF01_details.total_amount,p_auth_tx->reconciliation_amount, sizeof(p_auth_tx->TLF01_details.total_amount));
   //DD on 3/7/00 
   //   strncpy(p_auth_tx->TLF01_details.reversal_amount,p_auth_tx->TLF01_details.orig_amount, sizeof(p_auth_tx->TLF01_details.reversal_amount));

   //12/7/99
   ptetime_get_strtime(unique_tran_str_id);
   memset(p_auth_tx->TLF01_details.primary_key.transaction_id, 0, sizeof(p_auth_tx->TLF01_details.primary_key.transaction_id));
   memcpy(p_auth_tx->TLF01_details.primary_key.transaction_id, p_auth_tx->TLF01_details.terminal_id, sizeof(p_auth_tx->TLF01_details.terminal_id) - 1);
   strncat(p_auth_tx->TLF01_details.primary_key.transaction_id, unique_tran_str_id+2, 12);
  
   build_host_structure(p_auth_tx);
   if (true == Build_Host_Request(p_auth_tx))
   {
//         Build_Host_Header(p_auth_tx);                     No header for JCB
      if(false == hhutil_send_host_response(p_auth_tx))
	  {
		ncjcb2_log_message( 3, 2, "Error sending response for the host" , "incoming_other_host_response_db_reply" , 0 ,"");
		iRet_code = false;
	  }
   }
   else
   {
      strcpy( p_auth_tx->TLF01_details.response_text,
             "Error building response for the host" );
      ncjcb2_log_message( 3, 2, "Error building response for the host" , "incoming_other_host_response_db_reply" , 0 ,"");
	  iRet_code = false;
      
   }


   // put the auth currency code and amount back for database update
   strncpy(p_auth_tx->TLF01_details.currency_code,temp_cur_code, sizeof(p_auth_tx->TLF01_details.currency_code));
   strncpy(p_auth_tx->TLF01_details.total_amount,temp_amount, sizeof(p_auth_tx->TLF01_details.total_amount));
   strncpy(p_auth_tx->TLF01_details.reversal_amount,temp_rev_amount, sizeof(p_auth_tx->TLF01_details.reversal_amount));

   insert_tlf01(p_auth_tx);
   return iRet_code;
}


/*****************************************************************************

  Function:   hhutil_parse_db_ok

  Description: This function determines which type of database request the
               incoming message is from and routes it to the correct function.
  Author: 
      BGable   7/13/98
  Inputs:
      p_msg_in - pointer to the incoming message
  Outputs:
      None
  Return values:
      None
  Modified by:
      
******************************************************************************/
void hhutil_parse_db_ok(pPTE_MSG p_msg_in)
{
   pBYTE pCurrent;
   pPTE_MSG_DATA p_msg_data;
   BYTE  subtype1;
   BYTE  subtype2;
   BYTE  app_data_type;
   AUTH_TX auth_tx;

   p_msg_data = ptemsg_get_pte_msg_data(p_msg_in);
   pCurrent = ptemsg_get_pte_msg_data_data(p_msg_data);

   app_data_type = ptemsg_get_pte_msg_data_app_data_type(p_msg_data);
   subtype1 = ptemsg_get_msg_subtype1(p_msg_in);
   subtype2 = ptemsg_get_msg_subtype2(p_msg_in);

   if (ST1_DB_SELECT == subtype1)
   {
      if (NCF30_DATA == app_data_type)
      {
         /*  The reversal record was found,
          * send the transaction to the authorizer.
          */
         if (false == process_incoming_message_ncf30_reply(pCurrent))
         {
            ncjcb2_log_message( 3, 2, "Unable to send the request to the authorizor application" , "hhutil_parse_db_ok" , 0 ,"");
         } 
      }
      else if ( NEG01_DATA == app_data_type )
      {
         /* The NEG01 record was found, so we need to reject the transaction */
         strcpy( auth_tx.TLF01_details.response_text, "Negative Record" );

         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr," NEG01 record was found, rejecting the transaction Term Id:%s ,Merch Id: %s RRN: %s ",
         					auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
         ncjcb2_log_message( 3, 2, Log_Bufr , "hhutil_parse_db_ok" , 0 ,auth_tx.TLF01_details.card_num);
         perform_error_processing( &auth_tx );
         send_it_back_to_terminal("91", &auth_tx);
      }
      else
      {
         strcpy( auth_tx.TLF01_details.response_text,
                "Invalid incoming select message type" );
         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr," Invalid incoming select message type Msg type : %s , Term Id:%s ,Merch Id: %s RRN: %s ",
        		 auth_tx.TLF01_details.message_type,auth_tx.TLF01_details.terminal_id,
         					auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
         ncjcb2_log_message( 3, 2, Log_Bufr , "hhutil_parse_db_ok" , 0 ,auth_tx.TLF01_details.card_num);
      }
   }
   else if (ST1_DB_UPDATE == subtype1)
   {
      if (ALL_DATA == app_data_type)
      {
         memset( &auth_tx, 0,        Auth_Tx_Size );
         memcpy( &auth_tx, pCurrent, Auth_Tx_Size );

         if ( (0 == strncmp( "0130", auth_tx.TLF01_details.message_type, 4)) ||
              ( auth_tx.TLF01_details.tx_key == AUTH_ADVICE_RESPONSE )       ||
              ( auth_tx.TLF01_details.tx_key == AUTH_ADVICE_REPEAT_RESPONSE ))
         {
            /* Aready sent the respone to host.
             * Already logged into TLF01.
             */
            return; 
         }
         else
         {
            if( false == incoming_other_host_response_db_reply( &auth_tx ))
            {
                memset(Log_Bufr,0x00,sizeof(Log_Bufr));
                sprintf(Log_Bufr," Unable to send response to host Term Id:%s ,Merch Id: %s RRN: %s ",
                					auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
               ncjcb2_log_message( 3, 2, Log_Bufr , "hhutil_parse_db_ok" , 0 ,auth_tx.TLF01_details.card_num);
            }
         }         
      }
   }

} /* hhutil_parse_db_ok */

/*****************************************************************************

  Function:   hhutil_parse_db_error

  Description: This function determines which type of database request the
               incoming error message is from and routes it to the correct function.
  Author: 
      BGable   7/13/98
  Inputs:
      p_msg_in - pointer to the incoming message
  Outputs:
      None
  Return values:
      None
  Modified by:
      
******************************************************************************/
void     hhutil_parse_db_error(pPTE_MSG p_msg_in)
{
   pBYTE pCurrent,p_data;
   pPTE_MSG_DATA p_msg_data;
   pPTE_MSG p_echo_msg;
   BYTE  app_data_type;
   BYTE  subtype1;
   BYTE  subtype2;
   CHAR  error_msg[512] = {0};
   AUTH_TX       auth_tx,org_auth_tx;
   CHAR  Buffer[512]={0};
   NCF30 ncf30;
   pBYTE orig_que;
   LONG data_len;

   p_msg_data = ptemsg_get_pte_msg_data(p_msg_in);
   pCurrent = ptemsg_get_pte_msg_data_data(p_msg_data);

   memset(&auth_tx, 0, Auth_Tx_Size);
   memcpy(&auth_tx, pCurrent, Auth_Tx_Size);
   strcpy (error_msg, pCurrent+(Auth_Tx_Size));

   app_data_type = ptemsg_get_pte_msg_data_app_data_type(p_msg_data);
   subtype1 = ptemsg_get_msg_subtype1(p_msg_in);
   subtype2 = ptemsg_get_msg_subtype2(p_msg_in);
   orig_que 	 = ptemsg_get_msg_orig_queue(p_msg_in);
	 p_echo_msg = ptemsg_get_chained_pte_msg(          p_msg_in, 2 );
		p_msg_data = ptemsg_get_pte_msg_data(             p_echo_msg  );
		p_data     = ptemsg_get_pte_msg_data_data(        p_msg_data  );
		data_len   = ptemsg_get_pte_msg_data_data_length( p_msg_data  );

		if ( data_len == Auth_Tx_Size )
		{
			//Get auth_tx record that was built from host response data.
			memset( &org_auth_tx, 0x00,   Auth_Tx_Size );
			memcpy( &org_auth_tx, p_data, Auth_Tx_Size );
		}
   if (ST1_DB_SELECT == subtype1)
   {
      if (NCF30_DATA == app_data_type)
      {
         /* Unable to select an original txn for a reversal or advice.
          * For reversals, respond to host with an approval (yes, approval).
          * In both cases, insert into TLF01.  Reversal advices do not
          * require a response.
          */
         if ( AUTH_REVERSAL        == auth_tx.TLF01_details.tx_key ||
              AUTH_REVERSAL_REPEAT == auth_tx.TLF01_details.tx_key )
         {
            auth_tx.TLF01_details.tx_key = AUTH_REVERSAL_RESPONSE;
            strcpy( auth_tx.TLF01_details.response_code, "00" );

            /* This function inserts into TLF01. */
            (void)incoming_other_host_response_db_reply( &auth_tx );
         }
         else if (( AUTH_ADVICE        == auth_tx.TLF01_details.tx_key ) || 
                  ( AUTH_ADVICE_REPEAT == auth_tx.TLF01_details.tx_key ))
         {
            memset( &ncf30, 0x00, Ncf30_Size );
            (void)process_auth_advice( &auth_tx, NOT_FOUND,ncf30);
         }
         else
            insert_tlf01(&auth_tx);
      }
   }
   else if ( ST1_DB_UPDATE == subtype1 )
   {
      if ( ALL_DATA == app_data_type )
      {
         sprintf( Buffer,
                 "Unable to update DB with txn info. Error = %s",
                  error_msg );
         memcpy( auth_tx.TLF01_details.response_text, Buffer, 40 );
         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
         					auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
         strcat(Buffer,Log_Bufr);
         ncjcb2_log_message( 3, 2, Buffer , "hhutil_parse_db_error" , 0 ,auth_tx.TLF01_details.card_num);

         memset( &auth_tx, 0,        Auth_Tx_Size );
         memcpy( &auth_tx, pCurrent, Auth_Tx_Size );

         if ( (0 == strncmp( "0130", auth_tx.TLF01_details.message_type, 4)) ||
              ( auth_tx.TLF01_details.tx_key == AUTH_ADVICE_RESPONSE )       ||
              ( auth_tx.TLF01_details.tx_key == AUTH_ADVICE_REPEAT_RESPONSE ) )
         {
            /*  Already sent the respone to host.
             *  Already logged into TLF01.
             */
            return; 
         }
         else
         {
            strcpy(auth_tx.TLF01_details.response_code , "96");
            if( false == incoming_other_host_response_db_reply( &auth_tx ))
            {
               memset(Log_Bufr,0x00,sizeof(Log_Bufr));
               sprintf(Log_Bufr," Unable to send response to host Term Id:%s ,Merch Id: %s RRN: %s ",
               					auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
               ncjcb2_log_message( 3, 2, Log_Bufr , "hhutil_parse_db_error" , 0 ,auth_tx.TLF01_details.card_num);
            }
         }
      }
      else if((strncmp(orig_que,"timerdsA",5)==0 ))
            {
          	/*	TF Phani and Raghu
          	 * This is request from BDo - Marvin - MC time out - APR mandated changed 2017
          	 * In this scenario , where ATP sends TO to cadenceie, But cadencie replied with approval
          	 * for timed out trnsaction.    	 *
          	 * 	1. ATP -> cadeince : sends 100
          	 * 	2. ATP timed out
          	 * 	3. ATP sends reversal to Cadencie, however there is no approved tx for this. This will be unsloicated reversal.
          	 * 	4. ATP start processing as STIP, repsonds to orginator
          	 * 	5  Cadencie responds with 410 with 00 or 19. AS unslocited reversal.
          	 * 	5. Cadencie responds with 0110
          	 * 	6. ATP should sends 0400 to cadencie, since this tx alreday processed as STIP.
          	 * 	7. Cadencie responds with 0410. Just ingnore. No action to take.
          	 * */

          	  // Test

      		/*	this part moved above inorder to isolate the log printing.
      		 * p_echo_msg = ptemsg_get_chained_pte_msg(          p_msg_in, 2 );
      			p_msg_data = ptemsg_get_pte_msg_data(             p_echo_msg  );
      			p_data     = ptemsg_get_pte_msg_data_data(        p_msg_data  );
      			data_len   = ptemsg_get_pte_msg_data_data_length( p_msg_data  );

      			if ( data_len == Auth_Tx_Size )
      			{
      				//Get auth_tx record that was built from host response data.
      				memset( &org_auth_tx, 0x00,   Auth_Tx_Size );
      				memcpy( &org_auth_tx, p_data, Auth_Tx_Size );
      			}*/
      			  //Test
      			if( strncmp(org_auth_tx.TLF01_details.response_code,"00",2) ==0 &&
      					(strncmp(org_auth_tx.TLF01_details.message_type,"0110",4) ==0 ||
      					strncmp(org_auth_tx.TLF01_details.message_type,"0210",4) ==0))
      			{
      				sprintf(error_msg,"MCTO Sending Reversal for Late resposne subtype1: %d subtype2 %d orig_que %s STAN : %s",subtype1,subtype2,orig_que,org_auth_tx.TLF01_details.sys_trace_audit_num);
      	             memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      	             sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      	            		org_auth_tx.TLF01_details.terminal_id,org_auth_tx.TLF01_details.merchant_id,org_auth_tx.TLF01_details.retrieval_ref_num);
      	             strcat(error_msg,Log_Bufr);
      	             ncjcb2_log_message( 3, 1, error_msg , "hhutil_parse_db_error" , 0 ,org_auth_tx.TLF01_details.card_num);

      				org_auth_tx.TLF01_details.tx_key = AUTH_REVERSAL_ADVICE;
      				strncpy(org_auth_tx.TLF01_details.orig_message,org_auth_tx.TLF01_details.message_type,4);
      				strcpy(org_auth_tx.TLF01_details.message_type, "0420");
      				org_auth_tx.TLF01_details.general_status = GS_TXN_PERF_REQ;
      				strcpy( org_auth_tx.TLF01_details.orig_sys_trace_num, org_auth_tx.TLF01_details.sys_trace_audit_num);
      				strcpy( org_auth_tx.TLF01_details.orig_retrieval_ref_num, org_auth_tx.TLF01_details.retrieval_ref_num);
      				strncpy(org_auth_tx.TLF01_details.product_codes[3].quantity,TIMED_OUT,2);
      				memset( org_auth_tx.req_data_len, 0x00 , sizeof(org_auth_tx.req_data_len));
      				memset( org_auth_tx.req_buffer, 0x00 , sizeof(org_auth_tx.req_buffer));
      				memset( org_auth_tx.resp_data_len, 0x00 , sizeof(org_auth_tx.resp_data_len));
      				memset( org_auth_tx.resp_buffer, 0x00 , sizeof(org_auth_tx.resp_buffer));
      				/*TOR : Time out reversal -> it is used to drop the response 410 coming from host
      				 * for this reversal request sent for JCB timeout
      				 *
      				 * */
      				if(strlen(ncf01_i.tpdu_id)> 0)
      				{
      					strncpy( org_auth_tx.TLF01_details.tpdu_id, ncf01_i.tpdu_id,sizeof(ncf01_i.tpdu_id));
      				}
      				else
      				{
      					strncpy( org_auth_tx.TLF01_details.tpdu_id,"JCB2",4);
      				}
      				strcpy(org_auth_tx.TLF01_details.response_text, "TOR: Timed out reversal for 0110");
      				ncjcb2_send_reversal_to_host_for_late_response( &org_auth_tx );
      			}
            }
   }
   else if (ST1_DB_INSERT == subtype1 && (strncmp(orig_que,"timerdsA",5)!=0))
   {
      sprintf(Buffer,"Insert TLF01 error: %s",error_msg);
      memcpy( auth_tx.TLF01_details.response_text, Buffer, 40 );
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      					auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncjcb2_log_message( 3, 2, Buffer , "hhutil_parse_db_error" , 0 ,auth_tx.TLF01_details.card_num);
   }
   return;

} /*hhutil_parse_db_error*/


/*****************************************************************************

  Function:    hhutil_get_ncf01_cfg

  Description: This function will send a request to the dataserver to select 
               NCF01.  This is done syncronously and should be used only by
               jcbmain.c!!!
  Author: 
      BGable     6/30/98
  Inputs:
      pncf01 - pointer to the requested ncf01 structure
  Outputs:
      pncf01 - pointer to the returned ncf01 structure
      error_msg - error message returned by the data server
  Return values:
      None
  Modified by:
******************************************************************************/
BYTE hhutil_get_ncf01_cfg( pNCF01 pncf01, pCHAR error_msg )
{
   pPTE_MSG       p_msg_out;
   pPTE_MSG       p_msg_in;
   LONG           data_len;
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   BYTE           buffer[sizeof(AUTH_TX)+sizeof(NCF01)];
   AUTH_TX        auth_tx;
   BYTE           dberror;
   CHAR           temp_str[100];
   LONG           retcode;

   memset(buffer,0, sizeof(buffer));

   memcpy(buffer,&auth_tx, Auth_Tx_Size);
   memcpy(buffer+Auth_Tx_Size,pncf01, Ncf01_Size);

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_SELECT,
                                 0,
                                 NULL_PTR,
                                 interactive_que_name,
                                 (pBYTE) buffer,
                                 sizeof( buffer),
                                 NCF01_DATA );

   if( NULL_PTR == p_msg_out )
   {
      strcpy( error_msg, "Insufficient memory to build request message" );
      return PTEMSG_INSUFFICIENT_MEMORY;
   }

   p_msg_in = pteipc_send_receive( p_msg_out, netds_que_name, interactive_que_name, &retcode );
   if( NULL_PTR == p_msg_in )
   {
      pteipc_get_errormsg( retcode, temp_str );
      sprintf( error_msg, "Communication Error during request to Terminal DataServer: %s", temp_str );
      return PTEMSG_IPC_ERROR;
   }

   if( ( dberror = ptemsg_get_result_code( p_msg_in ) ) != PTEMSG_OK )
   {
      p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
      p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
      data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );

      strcpy (temp_str, p_data+Auth_Tx_Size);
      sprintf( error_msg, "Database Error: %s", temp_str );
      free(p_msg_in);
      return (dberror);
   }

   p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
   p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
   data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );

   memset (pncf01, 0, Ncf01_Size);
   memcpy(pncf01, p_data+(Auth_Tx_Size), Ncf01_Size);

   free(p_msg_in);
   return ( PTEMSG_OK );
}


/*****************************************************************************

  Function:    hhutil_get_response_text

  Description: This function will fill the response_text field in auth_tx.TLF01.
               Currently this will only be used by voice auth transactions.
  Author: 
      BGable     5/26/99
  Inputs:
     pAUTH_TX - pointer to auth_tx
  Outputs:
     pAUTH_TX - pointer to auth_tx (with the response text updated)
  Return values:
     None
  Modified by:
******************************************************************************/
VOID hhutil_get_response_text( pAUTH_TX p_auth_tx)
{
   INT i;
   static unsigned char response[][35] = {
   {"00Approved"},
   {"01Refer to Card Issuer"},
   {"03Invalid Merchant"},   
   {"04Pick-Up Card"},
   {"05Do Not Honor"},

   {"06Error"},   
   {"07Pick-Up, Counterfeit"},   
   {"09Request In Progress"},   
   {"12Refer To Issuer"},   
   {"13Invalid Amount"},   
   
   {"14Invalid Card Number"},   
   {"15No Such Issuer"},   
   {"17Customer Cancellation"},   
   {"20Invalid Response"},   
   {"30Format Error"},   
   
   {"31No Bank Support by Switch"},   
   {"33Expired Card, Pick-Up"},   
   {"34Suspected Card, Pick-Up"},   
   {"36Restricted Card, Pick-Up"},   
   {"40Function Not Supported"},   
   
   {"41Lost, Stolen, Pick-Up"},   
   {"42Universal Acct Not Found"},   
   {"43Stolen Card, Pick-Up"},   
   {"51Not Sufficient Funds"},   
   {"54Expired Card, Pick-Up"},   
   
   {"55Incorrect PIN"},   
   {"56No Card Record"},   
   {"57Trnsxn Not Permitted To Card"},   
   {"58Trnsxn Not Permintted To Term"},   
   {"59Suspected Fraud, Not Pick-Up"},   
   
   {"61Exceeds Amount Limit"},   
   {"62Restricted Card, Not Pick-Up"},   
   {"63Security Violation"},   
   {"65Exceeds WithDrawl Freq Limit"},   
   {"68Response Received Too Late"},   
   
   {"75# of PIN Tries Exceeded"},   
   {"76Incorrect Reversal"},   
   {"77Lost, Stolen, Pick-Up"},   
   {"78Shop In Black List"},   
   {"79Account Status Flag False"},   
   
   {"87Incorrect Passport Number"},   
   {"88Incorrect Date of Birth"},   
   {"89Not Approved"},   
   {"90Cutoff Is In Progress"},   
   {"91Issuer Or Switch Inoperative"},   
   
   {"92Institution Can't Be Routed To"},   
   {"94Duplicate Transmission"},   
   {"96System Malfunction"}   
   };

 
   for (i = 0; i < 47; i++)
   {
      if (0 == strncmp(p_auth_tx->TLF01_details.response_code, response[i], 2))
      {
         strcpy(p_auth_tx->TLF01_details.voice_auth_text, response[i]+2);
         break;
      }

   }
   if ('\0' == p_auth_tx->TLF01_details.voice_auth_text[0])
   {
      strcpy(p_auth_tx->TLF01_details.voice_auth_text, response[42]+2);
   }
}

/******************************************************************************
 *
 *  NAME:         TRIM_SPACES
 *
 *  DESCRIPTION:  This function converts spaces at the end of a string into
 *                nulls.
 *
 *  INPUTS:       pString - String that does not want spaces at the end
 *
 *  OUTPUTS:      pString - Same string, but without trailing spaces
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       D. Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
void trim_spaces( pBYTE pString )
{

   INT len, position;

   len = strlen(pString) - 1;
   for(position = len; position >= 0; position--)
   {
      if(pString[position] == ' ')
         pString[position] = 0;
      else
         break;
   }
}

/*****************************************************************************

  Function:   ascii_to_bin

  Description:
      Convert an ascii string to a binary string
  Author: 
      SVelaga   6/21/99
  Inputs:
      astr      - pointer to the incoming ascii string
      ascii_len - length of astr
      blen      - length of bstring
  Outputs:
      binary_string  - pointer to the outgoing binary string
  Return values:
      NONE
  Modified by:

******************************************************************************/
void ascii_to_bin( CHAR *astr, INT ascii_len, BYTE *binary_string, INT blen)
{

	BYTE temp_char;
	INT i , j = 0;

	
	memset(binary_string, 0x00, blen);
	

	for(i=0, j=0; ((i < blen ) && (j < ascii_len)); i++, j+=2)
	{
		 	
		if( astr[j] <= '9' )
		{
			if(astr[j+1] <= '9')
				temp_char = (( (unsigned char )(astr[j] & 0x0f ) << 4 ) |  (unsigned char )(astr[j+1] & 0x0f ));
			else
				temp_char = (( (unsigned char )(astr[j] & 0x0f ) << 4 ) |  (unsigned char )((astr[j+1] + '9') & 0x0f ));

		}
		else 
		{
			if(astr[j+1] <= '9')
				temp_char = (( (unsigned char )((astr[j] + '9')  & 0x0f ) << 4 ) |  (unsigned char )(astr[j+1]  & 0x0f ));
			else
				temp_char = (( (unsigned char )((astr[j] + '9')  & 0x0f ) << 4 ) |  (unsigned char )((astr[j+1] + '9') & 0x0f ));
		}
		 
			binary_string[i] = temp_char;
	}

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

/******************************************************************************
 *
 *  NAME:         isalphanum_space
 *
 *  DESCRIPTION:  This function checks for a string to be all numbers and / or
 *                alphabetical characters and / or spaces.
 *
 *  INPUTS:       in_str - String to be checked
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if string is all alphanumeric/spaces, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT isalphanum_space(pCHAR string)
{
   INT i;
   INT length;

   length = (INT)strlen(string);

   for (i = 0; i < length; i++)
   {
      if ( string[i] != ' ' )
      {
         if (!isalnum(string[i]))
           return(false);
      }
   }
   return(true);
}


INT verify_auth_tx_data(pAUTH_TX p_auth_tx)
{

   CHAR  Buffer[512]={0};
   
   /* FIELD 2: PAN */
   if( false == isnum(p_auth_tx->TLF01_details.card_num))
   {
      sprintf(Buffer, "Non-numeric PAN (Field 2): %s", p_auth_tx->TLF01_details.card_num);
      memcpy( p_auth_tx->TLF01_details.response_text, Buffer, 40 );

      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncjcb2_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);
      return(false);
   }
   /* FIELD 3: PROCESSING CODE */
   if( false == isnum(p_auth_tx->TLF01_details.processing_code))
   {
      sprintf(Buffer, "Non-numeric processing code (Field 3): %s", p_auth_tx->TLF01_details.processing_code);
      memcpy( p_auth_tx->TLF01_details.response_text, Buffer, 40 );
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncjcb2_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);
      return(false);
   }
   if (0 == strcmp(p_auth_tx->TLF01_details.message_type,"0100"))
      if (0 != strncmp(p_auth_tx->TLF01_details.processing_code,"00", 2))
	      if (0 != strncmp(p_auth_tx->TLF01_details.processing_code,"01",2))
          {
             sprintf(Buffer, "Invalid data for processing code: %s", p_auth_tx->TLF01_details.processing_code);
             memcpy( p_auth_tx->TLF01_details.response_text, Buffer, 40 );
             memset(Log_Bufr,0x00,sizeof(Log_Bufr));
             sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
             					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
             strcat(Buffer,Log_Bufr);
             ncjcb2_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);
             return(false);
          }
   /* FIELD 4: TRANSACTION AMOUNT */
   if( false == isnum(p_auth_tx->TLF01_details.total_amount))
   {
      sprintf(Buffer, "Non-numeric total amount (Field 4): %s", p_auth_tx->TLF01_details.total_amount);
      memcpy( p_auth_tx->TLF01_details.response_text, Buffer, 40 );
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncjcb2_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);
      return(false);
   }
   /* FIELD 5: SETTLEMENT AMOUNT */
   if( false == isnum(p_auth_tx->TLF01_details.settlement_total))
   {
      sprintf(Buffer, "Non-numeric settlement total (Field 5): %s", p_auth_tx->TLF01_details.settlement_total);
      memcpy( p_auth_tx->TLF01_details.response_text, Buffer, 40 );
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncjcb2_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);
      return(false);
   }
   /* FIELD 7: TRANSMISSION DATE AND TIME */
   if( false == isnum(p_auth_tx->TLF01_details.transmission_timestamp))
   {
      sprintf(Buffer, "Non-numeric transmission timestamp (Field 7): %s", p_auth_tx->TLF01_details.transmission_timestamp);
      memcpy( p_auth_tx->TLF01_details.response_text, Buffer, 40 );
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncjcb2_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);
      return(false);
   }
   /* FIELD 11: STAN */
   if( false == isnum(p_auth_tx->TLF01_details.sys_trace_audit_num))
   {
      sprintf(Buffer, "Non-numeric system trace audit number (Field 11): %s", p_auth_tx->TLF01_details.sys_trace_audit_num);
      memcpy( p_auth_tx->TLF01_details.response_text, Buffer, 40 );
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncjcb2_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);
      return(false);
   }
   /* FIELD 14: EXPIRATION DATE */
   if( false == isnum(p_auth_tx->TLF01_details.exp_date))
   {
      sprintf(Buffer, "Non-numeric expiration date (Field 14): %s", p_auth_tx->TLF01_details.exp_date);
      memcpy( p_auth_tx->TLF01_details.response_text, Buffer, 40 );
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncjcb2_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);
      return(false);
   }
   /* FIELD 19: ACQ INST COUNTRY CODE */
   if( false == isnum(p_auth_tx->country_code))
   {
      sprintf(Buffer, "Non-numeric acqiring institution country code (Field 19): %s", p_auth_tx->country_code);
      memcpy( p_auth_tx->TLF01_details.response_text, Buffer, 40 );
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncjcb2_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);
      return(false);
   }
   /* FIELD 26: CARD ACCEPTOR BUSINESS CODE */
   if( false == isnum(p_auth_tx->TLF01_details.category_code))
   {
      sprintf(Buffer, "Non-numeric category code (Field 26): %s", p_auth_tx->TLF01_details.category_code);
      memcpy( p_auth_tx->TLF01_details.response_text, Buffer, 40 );
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncjcb2_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);
      return(false);
   }
   /* FIELD 32: ACQUIRING INST ID CODE */
   if( false == isnum(p_auth_tx->acq_inst_id_cd))
   {
      sprintf(Buffer, "Non-numeric acquiring institution ID code (Field 32): %s", p_auth_tx->acq_inst_id_cd);
      memcpy( p_auth_tx->TLF01_details.response_text, Buffer, 40 );
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncjcb2_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);
      return(false);
   }
   /* FIELD 33: FORWARDING INST ID CODE */
   if( false == isnum(p_auth_tx->forwarding_inst_id_cd))
   {
      sprintf(Buffer, "Non-numeric forwarding institution ID code (Field 33): %s", p_auth_tx->forwarding_inst_id_cd);
      memcpy( p_auth_tx->TLF01_details.response_text, Buffer, 40 );
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncjcb2_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);
      return(false);
   }
   /* FIELD 37: RRN */
   if( false == isalphanum_space(p_auth_tx->TLF01_details.retrieval_ref_num))
   {
      sprintf(Buffer, "Special characters in retrieval reference number (field 37) not allowed: %s", p_auth_tx->TLF01_details.retrieval_ref_num);
      memcpy( p_auth_tx->TLF01_details.response_text, Buffer, 40 );
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncjcb2_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);
      return(false);
   }
   /* FIELD 38: AUTH CODE */
   if( false == isalphanum(p_auth_tx->TLF01_details.auth_number))
   {
      sprintf(Buffer, "Special characters in Auth Number (field 38) not allowed: %s", p_auth_tx->TLF01_details.auth_number);
      memcpy( p_auth_tx->TLF01_details.response_text, Buffer, 40 );
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncjcb2_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);
      return(false);
   }
   /* FIELD 39: RESPONSE CODE */
   if( false == isalphanum(p_auth_tx->TLF01_details.response_code))
   {
      sprintf(Buffer, "Special characters in Response Code (field 39) not allowed: %s", p_auth_tx->TLF01_details.response_code);
      memcpy( p_auth_tx->TLF01_details.response_text, Buffer, 40 );
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncjcb2_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);
      return(false);
   }
   /* FIELD 49: CURRENCY CODE, TRANSACTION */
   if( false == isnum(p_auth_tx->TLF01_details.currency_code))
   {
      sprintf(Buffer, "Non-numeric transaction currency code (Field 49): %s", p_auth_tx->TLF01_details.currency_code);
      memcpy( p_auth_tx->TLF01_details.response_text, Buffer, 40 );
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncjcb2_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);
      return(false);
   }
   return(true);
}

INT insert_tlf01(pAUTH_TX p_auth_tx)
{
   pPTE_MSG   p_msg_ins = NULL_PTR;
   BYTE       buffer[sizeof(AUTH_TX)+sizeof(TLF01)];
   LONG	     ret_code;
   CHAR       Buff[512]={0};
   CHAR       err_mesg[512] = {0};
   AUTH_TX    temp_auth_tx;
   CHAR       date_str [ 9] = {0};
   CHAR       time_str [ 7] = {0};
   CHAR       time_date[25] = {0};
   INT        ret_val;

   if ((strcmp(p_auth_tx->TLF01_details.originator_queue,"stpmgr") == 0) ) // ThoughtFocus Girija Y
   {
	   /* Update TLF01 Table in STIP manager module */
	   ret_code = true;
	   return( ret_code );
   }

   if ( p_auth_tx->host2_acquirer_cb_code[0] == '*' )
   {
      /* TLF01 has already been inserted. */
      ret_val = true;
   }
   else
   {
      p_auth_tx->host2_acquirer_cb_code[0] = '*';
      ptetime_get_strtime( p_auth_tx->TLF01_details.tran_finish_time );

      memset(&temp_auth_tx,0,Auth_Tx_Size);
      memcpy(&temp_auth_tx,p_auth_tx,Auth_Tx_Size);

      /* Get local time. */
      ptetime_get_timestamp( time_date );
      get_date( time_date, date_str );
      get_time( time_date, time_str );

      strcpy( temp_auth_tx.TLF01_details.date_yyyymmdd, date_str );
      strcpy( temp_auth_tx.TLF01_details.time_hhmmss, time_str );

      memset(buffer, 0, sizeof(buffer));
      memcpy(buffer+Auth_Tx_Size,&temp_auth_tx.TLF01_details, Tlf01_Size);

      p_msg_ins = ptemsg_build_msg( MT_DB_REQUEST, 
                                    ST1_DB_INSERT, 
                                    ST2_NONE,
                                    updatds_que_name, 
                                    application_que_name,
                                    (pBYTE)&buffer,
                                    sizeof(buffer), 
                                    TLF01_DATA ); 
      if(NULL_PTR == p_msg_ins)
      {
         ncjcb2_log_message( 3, 2, "Insufficient Memory to build insert TLF01 message" , "insert_tlf01" , 0 ,"");
         ret_val = false;
      }
      else
      {
         ret_code = pteipc_send(p_msg_ins, updatds_que_name); 
         if(ret_code < 0)
         {
            pteipc_get_errormsg(ret_code, err_mesg);
            sprintf( Buff,
                    "Failed transmit to que: %s. %s",
                     trands_que_name, err_mesg);
            memcpy( p_auth_tx->TLF01_details.response_text, Buff, 40 );
            ncjcb2_log_message( 3, 3, Buff , "insert_tlf01" , 0 ,"");
            ret_val = false;
         }
         else
            ret_val = true;

         free(p_msg_ins);
      }
   }
   return( ret_val );
}
/******************************************************************************
 *
 *  NAME:         PROCESS_REVERSAL_RESPONSES_TO_HOST
 *
 *  DESCRIPTION:  This function handles a response to a reversal. This reversal
 *                response will get sent to the host. But first we must perform
 *                an update ALL_DATA command.  This will update any database
 *                tables required by the transaction control service.  Then we
 *                will delete the reversal's original transaction from NCF30.
 *                Then we will send an 'approved' response to the host and log
 *                the transaction to TLF01.
 *
 *                This function will initiate the update ALL_DATA command.  The
 *                response will be processed in function
 *                'process_reversal_responses_db_reply'.
 *
 *  INPUTS:       p_auth_tx - This is the transaction
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   true if successful, else false
 *
 *  AUTHOR:       D. Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT process_reversal_responses_to_host( pAUTH_TX p_auth_tx )
{
   pPTE_MSG   p_msg_ins = NULL_PTR;
   BYTE       buffer[sizeof(AUTH_TX)];
   LONG       ret_code;
   CHAR       Buff[512]={0};
   CHAR       err_mesg[512] = {0};

   memset(buffer, 0x00, sizeof(buffer));
   memcpy(buffer, p_auth_tx, Auth_Tx_Size);

   /* Send message to update any Tran Control tables that need updating. */
   p_msg_ins = ptemsg_build_msg(MT_DB_REQUEST,
                                ST1_DB_UPDATE,
                                ST2_DB_UPDATE_TC_TABLES_ONLY,
                                updatds_que_name, 
                                application_que_name,
                                (pBYTE)&buffer,
                                sizeof(buffer), 
                                ALL_DATA);
   if(NULL_PTR == p_msg_ins)
   {
      ncjcb2_log_message( 3, 2, "Insufficient Memory to build UPDATE ALL message" , "insert_tlf01" , 0 ,"");
      return(false);
   }
   ret_code = pteipc_send(p_msg_ins, updatds_que_name); 
   free(p_msg_ins);
   if(ret_code < 0)
   {
      pteipc_get_errormsg(ret_code, err_mesg);
      sprintf(Buff,"Failed transmit to que: %s. %s",updatds_que_name,err_mesg);
      memcpy( p_auth_tx->TLF01_details.response_text, Buff, 40 );
      ncjcb2_log_message( 3, 2, Buff , "insert_tlf01" , 0 ,"");
      return(false);
   }
   return(true);
}

/******************************************************************************
 *
 *  NAME:         PROCESS_REVERSAL_RESPONSES_DB_REPLY
 *
 *  DESCRIPTION:  This function picks up processing of reversal responses after
 *                the call to update all_data.  Whether that call was successful
 *                or not is still processed here.  If successful, delete the
 *                reversal's original transaction, send a response to the host,
 *                and log it into TLF01.  If it failed, send to host and log
 *                to TLF01.
 *
 *  INPUTS:       pCurrent      - Data string containing auth_tx
 *                update_result - flag indicating the result of update all_data
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   true if successful, else false
 *
 *  AUTHOR:       D. Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT process_reversal_responses_db_reply( pBYTE pCurrent, INT update_result )
{
   INT     ret_code;
   AUTH_TX auth_tx;
   NCF30   ncf30;

   /* Get auth_tx structure out of the string. */
   memset( &auth_tx, 0x00,     Auth_Tx_Size);
   memcpy( &auth_tx, pCurrent, Auth_Tx_Size);

   if ( true == update_result )
   {
      /* The update ALL_DATA command was successful. */
      memset( &ncf30, 0x00, Ncf30_Size );
      strcpy( ncf30.primary_key.network_id, AppName );
      strcpy( ncf30.primary_key.network_type, "A" );
      strcpy( ncf30.primary_key.batch_number, "ACTIVE" );
      strcpy( ncf30.primary_key.retrieval_ref_num,
              auth_tx.TLF01_details.orig_retrieval_ref_num );
      ret_code = delete_orig_ncf30( ncf30, &auth_tx );
      if ( false == ret_code )
      {
         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr," Unable to delete reversal's original txn in NCF30 Term Id:%s ,Merch Id: %s RRN: %s ",
         					auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
         ncjcb2_log_message( 3, 2, Log_Bufr , "process_reversal_responses_db_reply" , 0 ,auth_tx.TLF01_details.card_num);
      }
   }
   else
   {
      /* The update ALL_DATA command failed. */
      strcpy( auth_tx.TLF01_details.response_text,
             "Failed to update DB tables for reversal" );

      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Failed to update database tables for a reversal. Term Id:%s ,Merch Id: %s RRN: %s ",
      					auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
      ncjcb2_log_message( 3, 2, Log_Bufr , "process_reversal_responses_db_reply" , 0 ,auth_tx.TLF01_details.card_num);
   }

   /* Send response to host. */
   strcpy(auth_tx.TLF01_details.response_code, "00");
   auth_tx.TLF01_details.tx_key =
                   match_request_to_response(auth_tx.TLF01_details.tx_key);

   /* This function inserts into TLF01 only if successful. */
   ret_code = incoming_other_host_response_db_reply( &auth_tx );

   memset(Log_Bufr,0x00,sizeof(Log_Bufr));
   sprintf(Log_Bufr," Unable to send reversal response to host Term Id:%s ,Merch Id: %s RRN: %s ",
   					auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
   ncjcb2_log_message( 3, 2, Log_Bufr , "process_reversal_responses_db_reply" , 0 ,auth_tx.TLF01_details.card_num);

   /* Log transaction to TLF01. */
   if ( false == ret_code )
      (void)insert_tlf01( &auth_tx );

   return( true );
}

/******************************************************************************
 *
 *  NAME:         VALIDATE_MCC
 *
 *  DESCRIPTION:  This function verifies the merchant category code resides
 *                within a specified range, if there is no track data.  If
 *                there is track data, no validation is performed, except
 *                that it is numeric.
 *
 *  INPUTS:       p_auth_tx - Structure containing transaction information
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if message is successfully sent, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
LONG validate_mcc( pAUTH_TX p_auth_tx )
{
   LONG ret_code = true;
   /* INT  mcc; */
   INT  len;
   INT  i;
   CHAR tempstr[512] = {0};

   len = strlen( p_auth_tx->TLF01_details.category_code );
   if ( len == bf18_len )
   {
      /* Make sure MCC is numeric. */
      for( i=0; i<bf18_len; i++ )
      {
         if ( !isdigit( p_auth_tx->TLF01_details.category_code[i] ) )
         {
            ret_code = false;
            sprintf(tempstr,"Invalid MCC: %s",
                    p_auth_tx->TLF01_details.category_code);
            break;
         }
      }

      if ( i >= bf18_len )
      {
         /* MCC is numeric; see if track data exists. */
         if ( (p_auth_tx->TLF01_details.track1[0] == 0x00) &&
              (p_auth_tx->TLF01_details.track2[0] == 0x00)  )
         {
            /* No track data.  Must be a MOTO or TE Merchant. */

            /*+------------------------------------------------------------+
              | Reference SCR #415: Equitable does not want to check MCC   |
              | for JCB.  Hence the commented out code below.              |
              |                                                            |
              | Reference SCR #423: Need to make sure CVV is not checked.  |
              | Hence the line to set verify_mag_stripe to 'N'.  Leave     |
              | this line in, even if SCR 415 is backed out.               |
              |                                                            |
              | However, we always need to validate CAV2.                  |
              +------------------------------------------------------------+
            */
            if ( p_auth_tx->TLF01_details.cvc_data[0] != 0x00 )
               p_auth_tx->TLF01_details.verify_mag_stripe[0] = 'Y';
            else
               p_auth_tx->TLF01_details.verify_mag_stripe[0] = 'N';

            /*-------------------------------------------------------------
            mcc = atoi( p_auth_tx->TLF01_details.category_code );
            if ( ((MCC_RANGE1_LOW <= mcc)  &&  (mcc <= MCC_RANGE1_HIGH)) ||
                 ((MCC_RANGE2_LOW <= mcc)  &&  (mcc <= MCC_RANGE2_HIGH)) ||
                 ( mcc == MCC_MOTO_MERCH_7011 ))
            ---------------------------------------------------------------*/
            ret_code = true;
         }
         else
            ret_code = true;
      }
   }

   if ( ret_code == false )
   {
      /* Display error message */
      sprintf(tempstr,"Invalid MCC: '%s'",p_auth_tx->TLF01_details.category_code);
      memcpy( p_auth_tx->TLF01_details.response_text, tempstr, 40 );
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(tempstr,Log_Bufr);
      ncjcb2_log_message( 3, 2, tempstr , "validate_mcc" , 0 ,p_auth_tx->TLF01_details.card_num);
   }
   return(ret_code);
}

/******************************************************************************
 *
 *  NAME:         BUILD_AND_SEND_REQUEST_MESSAGE
 *
 *  DESCRIPTION:  This procedure builds and sends a request message to the 
 *                desired queue.
 *
 *  INPUTS:       msgtype  - PTE message type 
 *                subtype1 - Subtype1 for the message (see PTE.h)
 *                subtype2 - Subtype2 for the message (see PTE.h and APP_INFO.h) 
 *                que_name - Name of the queue to send the message to 
 *                buffer   - Buffer that contains the data to be sent, 
 *                length   - Length of buffer
 *                app_type - App data type for the message (see APP_INFO.h)
 *                comm_info- string containing originator communications info
 *
 *  OUTPUTS:      err_msg  - Text to describe reason for failure
 *
 *  RTRN VALUE:   True if successful, False if a problem occurred
 *
 *  AUTHOR:       Beth Gable
 *
 ******************************************************************************/
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
   CHAR            Buff[512] = {0};
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

/******************************************************************************
 *
 *  NAME:         init_txn
 *
 *  DESCRIPTION:  This function initializes a new auth_tx transaction.
 *
 *  INPUTS:       p_auth_tx - Data structure containing transaction record.
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void init_txn( pAUTH_TX p_auth_tx )
{
   CHAR   unique_tran_str_id[21] = "";
   CHAR   date_str [ 9] = "";
   CHAR   time_str [ 7] = "";
   CHAR   time_date[25] = "";
   CHAR   temp_unique_tran[15]="";
   INT	 tid_len = 0;
   INT	 unique_tran_str_id_len=0;

   /* Get local time. */
  if ( p_auth_tx->TLF01_details.date_yyyymmdd[0] == 0x00 )
  {
	 ptetime_get_timestamp( time_date );
	 get_date( time_date, date_str );
	 get_time( time_date, time_str );
	 strcpy( p_auth_tx->TLF01_details.date_yyyymmdd, date_str );
	 strcpy( p_auth_tx->TLF01_details.time_hhmmss,	 time_str );
  }

   /* Create a unique transaction id for the new auth_tx record. */
   ptetime_get_strtime( unique_tran_str_id );
   memset( p_auth_tx->TLF01_details.primary_key.transaction_id, 0,
		   sizeof(p_auth_tx->TLF01_details.primary_key.transaction_id) );

   tid_len= strlen(p_auth_tx->TLF01_details.terminal_id);
   if(tid_len == 8)
   {
   		memcpy( p_auth_tx->TLF01_details.primary_key.transaction_id,
		   		p_auth_tx->TLF01_details.terminal_id, 
		   		sizeof( p_auth_tx->TLF01_details.terminal_id) - 1 );
   		strncat( p_auth_tx->TLF01_details.primary_key.transaction_id,
				unique_tran_str_id+2, 12 );
   }
   else
   {
	   /* We have to now prepare 20 digit unique Transaction ID.
		 unique_tran_str_id will be having 14 digit length
		 now we will make it as 20 by adding YYMMDD in the beginning of it.
		 20 byte will look like
		 YYMMDD"unique_tran_str_id"
		 for example if unique_tran_str_id is "1444752896.334" then it will become
		 YYMMDD1444752896.334
		 Based on ther terminal length we will copy data from new unique_tran_str_id
	   */
	   strncpy(temp_unique_tran,unique_tran_str_id,14);
	   memset(unique_tran_str_id,0x00,sizeof(unique_tran_str_id));
	  
	   strncpy(unique_tran_str_id,
				p_auth_tx->TLF01_details.date_yyyymmdd+2, 6); //YYMMDD
	   strncat (unique_tran_str_id,temp_unique_tran,14);
	   
	   unique_tran_str_id_len= strlen(unique_tran_str_id);
	   memcpy( p_auth_tx->TLF01_details.primary_key.transaction_id,
			   p_auth_tx->TLF01_details.terminal_id, tid_len );
	   strncat( p_auth_tx->TLF01_details.primary_key.transaction_id,
				unique_tran_str_id+tid_len, 
				(unique_tran_str_id_len-tid_len) );
		p_auth_tx->TLF01_details.primary_key.transaction_id[20]=0x00;
   }

   /* Initialize some of the auth_tx fields. */
   p_auth_tx->TLF01_details.general_status		=  GS_TXN_NEW;
   p_auth_tx->TLF01_details.entry_type			=  FINANCIAL;
   strcpy( p_auth_tx->TLF01_details.dispensation.origin, "RO" );
   strcpy( p_auth_tx->TLF01_details.dispensation.auth_2, "RA" );

   strcpy( p_auth_tx->TLF01_details.handler_queue,		 application_que_name);
   strcpy( p_auth_tx->TLF01_details.verify_mag_stripe,	 "Y"				 );
   strncpy( p_auth_tx->TLF01_details.tran_start_time,	 unique_tran_str_id,16  );

   /* Set issuer Id for reporting purposes. */
   strncpy( p_auth_tx->TLF01_details.issuer_id, AppName,
			sizeof(p_auth_tx->TLF01_details.issuer_id) );

   /* Create RRN */
   if ( p_auth_tx->TLF01_details.retrieval_ref_num[0] == 0x00 )
   {
	  create_rrn( p_auth_tx->TLF01_details.retrieval_ref_num );
   }

   strcpy(p_auth_tx->TLF01_details.source_key,		  ncf01_a.cwk	  );
   strcpy(p_auth_tx->TLF01_details.source_pin_format, ncf01_a.pin_type);
   strcpy(p_auth_tx->TLF01_details.tpdu_id, 		  ncf01_i.tpdu_id );

   return;
}


/******************************************************************************
 *
 *  NAME:         get_gmt_time
 *
 *  DESCRIPTION:  This function calculates GMT time and returns it in
 *                format MMDDhhmmss.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      gmt_time - Current local universal time - MMDDhhmmss
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void get_gmt_time( pBYTE gmt_time )
{
   CHAR  gmt_timedate[20]="";

   pintime_get_gmt( gmt_timedate );  /* YYYYMMDDhhmmss */

   memcpy( gmt_time, gmt_timedate+4, 10 );  /* MMDDhhmmss */
   return;
}

/******************************************************************************
 *
 *  NAME:         get_stan
 *
 *  DESCRIPTION:  This function creates a system trace audit number.  It uses
 *                the last 6 digits (not counting milliseconds) of the number
 *                of seconds since 1/1/1970.  This gives an 11 day turnover.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      stan - System Trace Audit number
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void get_stan( pBYTE stan )
{
   CHAR time_str[18]="";
   INT  len;

   ptetime_get_strtime( time_str ); /* Format = ssssssssss.jjj */
   len = strlen( time_str );

   /* Get rid of .jjj and get the last 6 's'. */
   memcpy( stan, &time_str[len-10], 6 );
   return;
}

/******************************************************************************
 *
 *  NAME:         WRITE_TO_TXN_FILE
 *
 *  DESCRIPTION:  This function calls the necessary library APIs to write a
 *                stream of data to the transaction trace file.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void write_to_txn_file()
{
   INT  rc;
   CHAR err_buf[512]={0};

   /* Log transaction to trace file. */
   if ( Tracing == TRACE_ON )
   {
      rc =log_trace_entry(ppTraceFile_fp,(pCHAR)ReqRsp_Buffer,buflen,TraceFile);
      if ( rc < 0 )
      {
         /* Error while attempting to log a trace entry. */
         Tracing = TRACE_OFF;
         memset( err_buf,   0x00, sizeof(err_buf)   );
         sprintf( err_buf,
                 "Error logging entry to Txn Trace File = %s",
                  TraceFile );
         ncjcb2_log_message( 3, 2, err_buf , "write_to_txn_file" , 0 ,"");

         if ( rc == -1 )
            strcpy( err_buf,
                   "Unable to write txn into trace file. Setting trace off.");
         else if ( rc == -2 )
            strcpy( err_buf, "Unable to close trace file. Setting trace off.");
         else if ( rc == -3 )
            strcpy( err_buf, "Unable to open trace file. Setting trace off." );
         else
            sprintf(err_buf,"Unknown return value: '%d' Setting trace off.",rc);

         ncjcb2_log_message( 3, 2, err_buf , "write_to_txn_file" , 0 ,"");
         /* Close the trace file. */
         if ( false == close_trace_file( ppTraceFile_fp ))
         {
            sprintf( err_buf, "Unable to close trace file %s", TraceFile );
            ncjcb2_log_message( 3, 2, err_buf , "write_to_txn_file" , 0 ,"");
         }
      }
   }
   return;
}


/******************************************************************************
 *
 *  NAME:         GET_CONV_RATE_FROM_DATABASE
 *
 *  DESCRIPTION:  This function makes a call to the data server to select 
 *                a record for currency conversion.
 *
 *                Use the interactive queue.  This function is being added
 *                long after JCB went to production.  The code is so messed
 *                up that it would be a ton of work to use re-entrant code.
 *                The function is not expected to be used very often, but it
 *                is needed in case of problems with shared memory.
 *                Reference SCR 1293.
 *
 *  INPUTS:       p_crf01 - Currency conversion data structure -> Currency Code
 *                p_auth_tx - Transaction Info
 *
 *  OUTPUTS:      p_crf01 - Currency conversion data structure -> Currency Rate
 *
 *  RTRN VALUE:    0 if successful
 *                -1 if a problem occurred
 *
 *  AUTHOR:       D. Irby
 *  
 ******************************************************************************/
LONG get_conv_rate_from_database( pCRF01 p_crf01, pAUTH_TX p_auth_tx )
{
   pPTE_MSG       p_msg_out;
   pPTE_MSG       p_msg_in;
   LONG           data_len;
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   BYTE           buffer[sizeof(AUTH_TX)+sizeof(NCF01)];
   BYTE           dberror;
   CHAR           temp_str[512]={0};
   CHAR           error_msg[512] = {0};
   LONG           retcode;
   LONG           ret_val;

   memset( buffer, 0x00, sizeof(buffer) );
   memcpy( buffer+Auth_Tx_Size, p_crf01, Crf01_Size );

   p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_SELECT,
                                 0,
                                 NULL_PTR,
                                 interactive_que_name,
                                 (pBYTE) buffer,
                                 sizeof( buffer),
                                 CRF01_DATA );

   if( NULL_PTR == p_msg_out )
   {
      strcpy( error_msg, "Insufficient memory to build CRF01 request message" );
      ret_val = -1;
   }
   else
   {
      p_msg_in = pteipc_send_receive( p_msg_out,
                                      trands_que_name,
                                      interactive_que_name,
                                     &retcode );
      if( NULL_PTR == p_msg_in )
      {
         pteipc_get_errormsg( retcode, temp_str );
         sprintf( error_msg,
                 "Communication Error during CRF01 request to DataServer: %s",
                  temp_str );
         ret_val = -1;
      }
      else
      {
         if( ( dberror = ptemsg_get_result_code( p_msg_in ) ) != PTEMSG_OK )
         {
            p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
            p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
            data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );

            strcpy( temp_str, p_data+Auth_Tx_Size );
            sprintf( error_msg, "DB Error: %s", temp_str );
            ret_val = -1;
         }
         else
         {
            p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
            p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
            data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );

            memset( p_crf01, 0x00, Crf01_Size );
            memcpy( p_crf01, p_data+(Auth_Tx_Size), Crf01_Size);
            ret_val = 0;
         }
         free(p_msg_in);
      }
   }

   if ( ret_val < 0 )
   {
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(error_msg,Log_Bufr);
      ncjcb2_log_message( 3, 2, error_msg , "get_conv_rate_from_database" , 0 ,p_auth_tx->TLF01_details.card_num);
   }
   return ( ret_val );
}

/******************************************************************************
 *
 *  NAME:         GET_JCB_EMV_DATA
 *
 *  DESCRIPTION:  This function steps through the EMV subfield tags.  If one
 *                that is supported by JCB is found, it is copied into the
 *                outgoing Host Message buffer.  The incoming data stream is
 *                not changed.
 *
 *  INPUTS:       emv_len_in  - Length of emv_data_in
 *                emv_data_in - EMV data that comes into ATP
 *
 *  OUTPUTS:      HOST_MESSAGE.iccdata     - EMV data that goes to JCB
 *                HOST_MESSAGE.iccdata_len - Length of EMV data to JCB
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void get_JCB_EMV_data( INT emv_len_in, pBYTE emv_data_in )
{
   INT   idx_in;
   INT   idx_out = 0;
   INT   tag_len;
   INT   data_len;
   INT   supported;
   BYTE  byteOneOfTag;
   BYTE  byteTwoOfTag;
   BYTE  emv_out_len[4] = "";

   /* Step past 2-byte length field. */
   idx_in = 2;

   /* Step through input, searching for subfields supported by MCard. */
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

      /* Check to see if this tag is supported by MCard. */
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
            case 0x03:
            case 0x09:
            case 0x10:
            case 0x1A:
            case 0x1E:
            case 0x26:
            case 0x27:
            case 0x33:
            case 0x34:
            case 0x35:
            case 0x36:
            case 0x37:
            case 0x41: break;

            default:   supported = false;
                       break;
         }
      }
      else
      {
         switch( byteOneOfTag )
         {
            case 0x4F:
            case 0x71:
            case 0x72:
            case 0x82:
            case 0x84:
            case 0x91:
            case 0x95:
            case 0x9A:
            case 0x9C: break;

            default: supported = false;
                     break;
         }
      }

      /* Copy supported MCard tags into outgoing host buffer. */
      if ( supported )
      {
         /* Subfield Tag */
         HOST_MESSAGE.iccdata[idx_out] = byteOneOfTag;
         if ( byteTwoOfTag != 0x00 )
            HOST_MESSAGE.iccdata[idx_out+1] = byteTwoOfTag;

         /* Subfield Length */
         HOST_MESSAGE.iccdata[idx_out+tag_len] = data_len;

         /* Subfield Data */
         memcpy( &HOST_MESSAGE.iccdata[idx_out+tag_len+1],
                 &emv_data_in[idx_in],
                  data_len );

         /* Increment the Outgoing Buffer index (tag+length+data) */
         idx_out += tag_len + 1 + data_len;
      }

      idx_in += data_len;
   }

   /* Get the length of the outgoing buffer. */
   HOST_MESSAGE.iccdata_len[1] = idx_out;

   return;
}

/******************************************************************************
 *
 *  NAME:         send_response_to_sbatch
 *
 *  DESCRIPTION:  This function builds and send a PTE message to the
 *                Sbatch queue using the AUTH_TX structure.  It sends
 *                a message type of MT_AUTH_RESPONSE .
 *
 *  INPUTS:       p_auth_tx  - Transaction structure
 *                msg_type   -  MT_AUTH_RESPONSE
 *                source_que - Queue name of calling application
 *
 *  OUTPUTS:      error_msg - If an error occurs, this will contain the message
 *                            Size can be up to 100 characters.
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Raghavendra
 *
 *  MODIFICATIONS:
 ******************************************************************************/

BOOLEAN send_response_to_sbatch(pAUTH_TX p_auth_tx,BYTE msg_type,
        pCHAR    source_que, pCHAR error_msg)
{
	   pPTE_MSG   p_msg   = NULL_PTR;
	   INT        ret_val = 0;

	   p_msg = ptemsg_build_msg( msg_type,
	                             ST1_NONE,
	                             ST2_NONE,
								 p_auth_tx->TLF01_details.handler_queue,
	                             source_que,
	                             (pBYTE)p_auth_tx,
	                             Auth_Tx_Size,
	                             0 );
	   if ( NULL_PTR == p_msg )
	   {
	      strcpy( error_msg, "Insufficient Memory for auth response message" );
	   }
	   else
	   {
	      ret_val = send_msg( p_msg, p_auth_tx->TLF01_details.handler_queue, error_msg );
	      free( p_msg );
	   }
	   return( ret_val );
}

BOOLEAN send_response_to_parvbh ( pAUTH_TX p_auth_tx,BYTE msg_type,
        						  pCHAR    source_que, pCHAR error_msg)
{
	pPTE_MSG   p_msg   = NULL_PTR;
	INT        ret_val = 0;

	p_msg = ptemsg_build_msg( msg_type,
							 ST1_NONE,
							 ST2_NONE,
							 parvbh_que_name,
							 source_que,
							 (pBYTE)p_auth_tx,
							 Auth_Tx_Size,
							 0 );
	if ( NULL_PTR == p_msg )
	{
	  strcpy( error_msg, "Insufficient Memory for auth response message" );
	}
	else
	{
	  ret_val = send_msg( p_msg, parvbh_que_name, error_msg );
	  free( p_msg );
	}
	return( ret_val );
}
/******************************************************************************
 *
 *  NAME:         SEND_MSG
 *
 *  DESCRIPTION:  This function sends a PTE message to a specified queue.
 *
 *  INPUTS:       p_msg     - Pointer to a PTE_MSG structure
 *                que_name  - Name of the destination queue
 *
 *  OUTPUTS:      error_msg - If an error occurs, this will contain the message
 *                            Size can be up to 100 characters.
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT send_msg( pPTE_MSG p_msg, pCHAR que_name, pCHAR error_msg )
{
   INT      ret_val = true;
   LONG     ret_code = 0;
   CHAR     Buff[256]   = {0};
   CHAR     pte_err_mesg[200] = {0 };

   ret_code = pteipc_send( p_msg, que_name );
   if ( 0 > ret_code )
   {
      pteipc_get_errormsg( ret_code, pte_err_mesg );
      sprintf(Buff,"pteipc error sending to que: %s. %s",que_name,pte_err_mesg);
      memcpy( error_msg, Buff, 99 );
      ret_val = false;
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         get_cav2_result_msg
 *
 *  DESCRIPTION:  This function returns an error message associated with
 *                a failed CAVV result value.
 *
 *  INPUTS:       cavv_result - Value from Visa concerning CAVV
 *
 *  OUTPUTS:      error_msg  - Informative message for invalid value
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Raghu
 *
 ******************************************************************************/
void get_cav2_result_msg( BYTE cavv_result, pCHAR error_msg )
{
   switch( cavv_result )
   {
      case 'M':  strcpy( error_msg, "CAV2 match" );
                 break;

      case 'N':  strcpy( error_msg, "CAV2 no match" );
                 break;

      case 'P':  strcpy( error_msg,"CAV2 not processed" );
                 break;

      case 'S':  strcpy( error_msg,"CAV2 should not be on card" );
                 break;

      case 'U':  strcpy( error_msg,"Issuer does not participate in CAV2 service" );
                 break;

      case ' ':  strcpy( error_msg,"CAV2 was not provided" );
                 break;

      default:   strcpy( error_msg, "CAV2 Result is a non-listed value." );
                 break;
   }
   return;
}

/******************************************************************************
 *
 *  NAME:         GET_CAVV_RESULT_ERROR
 *
 *  DESCRIPTION:  This function returns an error message associated with
 *                a failed CAVV result value.
 *
 *  INPUTS:       cavv_result - Value from Visa concerning CAVV
 *
 *  OUTPUTS:      error_msg  - Informative message for invalid value
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Raghu
 *
 ******************************************************************************/
void get_cavv_result_error( BYTE cavv_result, pCHAR error_msg )
{
   switch( cavv_result )
   {
      case '0':  strcpy( error_msg, "Erroneous data submitted" );
                 break;

      case '1':  strcpy( error_msg, "Failed validation" );
                 break;

      case '3':  strcpy( error_msg,
                        "Validation could not be performed or was not completed" );
                 break;

      case '4':  strcpy( error_msg,
                        "Validation could not be performed or authentication was prevented due to errors" );
                 break;

      case '5':  strcpy( error_msg,
                        "Not validated - Issuer is not a CAVV service participant" );
                 break;

      case '6':  strcpy( error_msg,
                        "Not validated - Issuer is not set to participate in a CAVV service" );
                 break;

      default:   strcpy( error_msg, "CAVV Result is a non-listed value." );
                 break;
   }
   return;
}


/*******************************************************
 * This function will get the values from tf.ini file
 * from the section DATASERVER with keyname DB_ERROR_STATICS_FLAG &
 *	DB_ERROR_STATICS_VALUE which will help to decide the db statics
 *	logging mechanism.
 * *******************************************************/

void ncjcb2_get_error_warning_file_name_path(void )
{
   DWORD rc;
   CHAR  filename   [80] = {0};
   CHAR  tmpstr     [80] = {0};
   CHAR  ncjcb2_error_warning_file_size[5] = {0};

   /* Get path to the tf.ini file. */
   memset( tmpstr, 0x00, sizeof(tmpstr) );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, "tf.ini" );

   /*Read path for creating file in order to log db statics and db oracle error messages */
   rc = GetPrivateProfileString(
								  "DATASERVERS",             /* points to section name         */
								  "DB_STATICS_PATH",       	/* points to key name             */
								   "",  					/* Default string                 */
								   ncjcb2_error_warning_file_path,              	/* points to destination buffer   */
								   sizeof(ncjcb2_error_warning_file_path)-1,   	 /* size of destination buffer     */
								   filename                  /* points to ini filename         */
						 	   );

   rc = GetPrivateProfileString(
								  "ERROR_WARNING",             /* points to section name         */
								  AppName,       	/* points to key name             */
								  "",  					/* Default string                 */
								  ncjcb2_module_error_warning_file_name,              	/* points to destination buffer   */
								  sizeof(ncjcb2_module_error_warning_file_name)-1,   	 /* size of destination buffer     */
								  filename                  /* points to ini filename         */
								);

   rc = GetPrivateProfileString(
							  "ERROR_WARNING",             /* points to section name         */
							  "ERROR_WARNING_FILE_SIZE",       	/* points to key name             */
							  "500",  					/* Default string                 */
							  ncjcb2_error_warning_file_size,              	/* points to destination buffer   */
							  sizeof(ncjcb2_error_warning_file_size)-1,   	 /* size of destination buffer     */
							  filename                  /* points to ini filename         */
							);

	/* File size size conversion */
	Max_File_Size_Defined = atoi(ncjcb2_error_warning_file_size);
	if(Max_File_Size_Defined <= 0)
	{
		Max_File_Size_Defined = 500 ;
	}
	Max_File_Size_Defined = Max_File_Size_Defined * ONE_MB_IN_BYTES ; /* 1 MB = 1048576 BYTES*/

   if((strlen(ncjcb2_error_warning_file_path) > 0) &&
	  (strlen(ncjcb2_module_error_warning_file_name)> 0))
   {
	   ncjcb2_create_Error_Filename();
   }
}

INT ncjcb2_Log_error_warning_to_File(pCHAR Error_Buf,int sev, pCHAR func,int detail)
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
    len = strlen(ncjcb2_Error_warning_Filename);
    path_len = strlen(ncjcb2_error_warning_file_path);
    if( len == 0 ||
    	path_len==0 )
    {
    	LogEvent(Error_Buf,INFO_MSG);
    	return 0;
    }
    if ( 0 != strcmp(&ncjcb2_Error_warning_Filename[len-4], current_mmdd) )
    {
		/* Now create the new filename.*/
    	ncjcb2_create_Error_Filename();
    }
	if((fp = fopen(ncjcb2_Error_warning_Filename,"a+b"))==NULL)
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

void ncjcb2_create_Error_Filename()
{
	CHAR  system_date[25] = {0};

   /* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
	ptetime_get_timestamp( system_date );

    strcpy( ncjcb2_Error_warning_Filename, ncjcb2_error_warning_file_path );
    strcat( ncjcb2_Error_warning_Filename, ncjcb2_module_error_warning_file_name );
	strcat( ncjcb2_Error_warning_Filename, ".EW." );
    strncat(ncjcb2_Error_warning_Filename, system_date,   4 );  /* YYYY */
    strncat(ncjcb2_Error_warning_Filename, system_date+5, 2 );  /* MM   */
    strncat(ncjcb2_Error_warning_Filename, system_date+8, 2 );  /* DD   */
}
/******************************************************************************
 *
 *  NAME:         ncjcb2_log_message
 *
 *  DESCRIPTION:  This function takes a string and input parameters and
 *                formats a message to be sent to either Monitor, Event Log
 *                or both.  Based on inputs, it sets the severity.
 *
 *  INPUTS:       dest - 1 = Monitor, 2 = File/Event log, 3 = Both
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
void ncjcb2_log_message( INT dest, INT sev, pCHAR msg, pCHAR func, int details ,pCHAR card_num)
{
   INT  monitor_flag = 0;
   INT  eventlog_flag = 0;
   INT  msg_type = 0;
   BYTE severity = 0;
   BYTE log_type[20] = {0};
   CHAR text_message [512] = {0};
   INT  Appname_len = 0;
   CHAR print_buffer[256] = {0} ;
   INT text_message_len = 0;
   CHAR card_num_msg[56] = {0};
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
   sprintf(text_message,"%s ,",AppName);
   Appname_len = strlen(text_message);

   memcpy( text_message + Appname_len, msg, (sizeof(text_message)-1 -Appname_len));
   if (card_num[0] == NULL)
   {
	   // No card number, hence masking not required
   }
   else
   {
	   Mask_card_number(card_num);
	   sprintf(card_num_msg, "Card num : %s",masked_card_num);
	   strcat(text_message,card_num_msg);
   }
   /* Call function to post the message. */
   ncjcb2_Log_error_warning_to_File(text_message,sev,func,details);
   if(monitor_flag == 1)
   {
	   text_message_len = strlen(text_message);
	   if(text_message_len > 255)
	   {
		   strncpy(print_buffer,text_message,254); // Maximum msg sent to moniotr is 256
		   TxUtils_Send_Msg_To_Operator( monitor_flag, print_buffer,
									 	 eventlog_flag, msg_type, func,
										 severity, log_type,
										 NULL, NULL,NULL );
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

void Mask_card_number(char * card_num)
{
	int cardlen = 0;
	int nIndex 	= 0;
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
}

BOOLEAN ncjcb_check_RPS_Transaction(pAUTH_TX p_auth_tx)
{
	if( (0 == strncmp(p_auth_tx->industry_code,"800",3 )) ||
		((0 == strncmp(p_auth_tx->TLF01_details.pos_condition_code,"08",2))&&
		 (0 == strncmp(p_auth_tx->TLF01_details.pos_entry_mode,"0012",4 ))))
	{
		return true;
	}
	else
	{
		return false;
	}

}

BOOLEAN ncjcb2_Is_AsiaPay_Transaction(pAUTH_TX p_auth_tx)
{
	if((0==strncmp(p_auth_tx->TLF01_details.message_type, "0200",4)) &&
	   (0==strncmp(p_auth_tx->TLF01_details.pos_condition_code, ECOM_POS_CC,2)) &&
	   (0 == strncmp(p_auth_tx->TLF01_details.pos_entry_mode,ECOM_POS_ENTRYMODE,3)))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void ncjcb2_read_TO_config_details_from_tf_ini()
{
	   DWORD rc;
	   CHAR  filename   [80] = {0};
	   CHAR  readstring [80] = {0};
	   CHAR  readvalue	[2]	 = {0};
	   /* Get path to the tf.ini file. */
	   memset( readstring, 0x00, sizeof(readstring) );
	   GetPinnacleConfigDirectory(readstring);
	   sprintf(filename, "%s%s", readstring, "tf.ini" );

	   rc = GetPrivateProfileString(  "REVERSL_ATTEMPTS_FOR_TIMEOUT_TRANSACTION",           /* points to section name         */
									  "REVERSL_ATTEMPTS_FOR_TIMEOUT_TRANSACTION_JCB",       	/* points to key name             */
									   "1",  												/* Default string                 */
									   readvalue,              								/* points to destination buffer   */
									   sizeof(readvalue),   	 							/* size of destination buffer     */
									   filename                  							/* points to ini filename         */
							 	   );

	   rvrsl_attmpts_value_fr_timeout_jcb=atoi(readvalue);

	   if(rc == 0)
	   {
		   ncjcb2_log_message( 2, 1, "REVERSL_ATTEMPTS_FOR_TIMEOUT_TRANSACTION_JCB Value not read from tf.ini","ncjcb2_read_tf_ini",0,"");
	   }

	   readvalue[0]='\0';
	   rc = GetPrivateProfileString(  "REVERSL_ATTEMPTS_FOR_LATERESPONSE_TRANSACTION",              /* points to section name         */
									  "REVERSL_ATTEMPTS_FOR_LATERESPONSE_TRANSACTION_JCB",       	/* points to key name             */
									   "1",  														/* Default string                 */
									   readvalue,              										/* points to destination buffer   */
									   sizeof(readvalue),   	 									/* size of destination buffer     */
									   filename                  									/* points to ini filename         */
							 	   );

	   rvrsl_attmpts_value_fr_laterspns_jcb=atoi(readvalue);

	   if(rc == 0)
	   {
		   ncjcb2_log_message( 2, 1, "REVERSL_ATTEMPTS_FOR_LATERESPONSE_TRANSACTION_JCB Value not read from tf.ini","ncjcb2_read_tf_ini",0,"");
	   }


}

INT ncjcb2_check_if_trx_is_bdo_pos_acq_trx( pAUTH_TX p_auth_tx  )
{
	 if((0 == strncmp(p_auth_tx->TLF01_details.handler_queue,
			 	 	  BDO_POS_ACQ_TXN_HANDLER_QUEUE,
					  BDO_POS_ACQ_TXN_HANDLE_QUEUE_LEN)))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}


INT ncjcb2_check_if_trx_is_sbatch( pAUTH_TX p_auth_tx  )
{
	 if((0 == strncmp(p_auth_tx->TLF01_details.handler_queue,
			 	 	  BDO_SBATCH_TXN_HANDLER_QUEUE,
					  BDO_POS_ACQ_TXN_HANDLE_QUEUE_LEN)))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

INT ncjcb2_is_valid_BDO_pos_acq_trn( pAUTH_TX p_auth_tx  )
{
	 if(0 == strncmp(p_auth_tx->TLF01_details.product_codes[4].code, "ACQUR", 5))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}



/*MobilePOS NFC Tap2Phone trx*/

INT ncjcb2_check_if_PEM_is_MobilePOS_NFC_Tap2Phone_trx_with_pin( pAUTH_TX p_auth_tx  )
{
	 if(0 == strncmp(p_auth_tx->TLF01_details.pos_entry_mode,
					 POS_MOBILE_NFC_TAP2PHONE_TXN_WITH_PIN_PEM,
					 POS_ENTRY_MODE_LEN))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

INT ncjcb2_check_if_PEM_is_MobilePOS_NFC_Tap2Phone_trx_without_pin( pAUTH_TX p_auth_tx  )
{
	if(0 == strncmp(p_auth_tx->TLF01_details.pos_entry_mode,
			 	 	POS_MOBILE_NFC_TAP2PHONE_TXN_WITHOUT_PIN_PEM,
					POS_ENTRY_MODE_LEN))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

INT ncjcb2_check_if_trx_qualify_for_MobilePOS_NFC_Tap2Phone_trx ( pAUTH_TX p_auth_tx  )
{
	if((0 == strncmp(p_auth_tx->TLF01_details.message_type,
			 	 	 POS_ACQ_SALE_MESSAGE_TYPE,
					 ISO_MESSAGE_TYPE_LEN))||
	   (0 == strncmp(p_auth_tx->TLF01_details.message_type,
			   	     POS_ACQ_AUTH_MESSAGE_TYPE,
					 ISO_MESSAGE_TYPE_LEN))||
	   (0 == strncmp(p_auth_tx->TLF01_details.message_type,
			   	   	 POS_ACQ_REVERSAL_MESSAGE_TYPE,
					 ISO_MESSAGE_TYPE_LEN))||
	   (0 == strncmp(p_auth_tx->TLF01_details.message_type,
			   	   	 POS_ACQ_REVERSAL_ADVICE_MESSAGE_TYPE,
					 ISO_MESSAGE_TYPE_LEN)))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}
INT ncjcb2_check_if_trx_is_jcb_MobilePOS_NFC_Tap2Phone_trx_with_pin( pAUTH_TX p_auth_tx  )
{
	if((true == ncjcb2_check_if_PEM_is_MobilePOS_NFC_Tap2Phone_trx_with_pin(p_auth_tx)) &&
	   (true == ncjcb2_check_if_trx_is_bdo_pos_acq_trx(p_auth_tx))&&
	   (true == ncjcb2_check_if_trx_qualify_for_MobilePOS_NFC_Tap2Phone_trx(p_auth_tx)))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

INT ncjcb2_check_if_trx_is_jcb_MobilePOS_NFC_Tap2Phone_trx_without_pin( pAUTH_TX p_auth_tx  )
{
	if((true == ncjcb2_check_if_PEM_is_MobilePOS_NFC_Tap2Phone_trx_without_pin(p_auth_tx)) &&
	   (true == ncjcb2_check_if_trx_is_bdo_pos_acq_trx(p_auth_tx))&&
	   (true == ncjcb2_check_if_trx_qualify_for_MobilePOS_NFC_Tap2Phone_trx(p_auth_tx)))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}
/********************************/


/*Swiftpass BDO Pay Credit Trx*/
INT ncjcb2_check_if_PEM_is_Swiftpass_BDO_Pay_credit_trx_with_pin( pAUTH_TX p_auth_tx  )
{
	 if(0 == strncmp(p_auth_tx->TLF01_details.pos_entry_mode,
			 	 	 POS_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITH_PIN_PEM,
					 POS_ENTRY_MODE_LEN))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

INT ncjcb2_check_if_PEM_is_Swiftpass_BDO_Pay_credit_trx_without_pin( pAUTH_TX p_auth_tx  )
{
	if(0 == strncmp(p_auth_tx->TLF01_details.pos_entry_mode,
					POS_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITHOUT_PIN_PEM,
					POS_ENTRY_MODE_LEN))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

INT ncjcb2_check_if_trx_qualify_for_Swiftpass_BDO_Pay_credit_trx ( pAUTH_TX p_auth_tx  )
{
	if((0 == strncmp(p_auth_tx->TLF01_details.message_type,
			 	 	 POS_ACQ_SALE_MESSAGE_TYPE,
					 ISO_MESSAGE_TYPE_LEN))||
	   (0 == strncmp(p_auth_tx->TLF01_details.message_type,
			   	     POS_ACQ_AUTH_MESSAGE_TYPE,
					 ISO_MESSAGE_TYPE_LEN))||
	   (0 == strncmp(p_auth_tx->TLF01_details.message_type,
			   	   	 POS_ACQ_REVERSAL_MESSAGE_TYPE,
					 ISO_MESSAGE_TYPE_LEN))||
	   (0 == strncmp(p_auth_tx->TLF01_details.message_type,
			   	   	 POS_ACQ_REVERSAL_ADVICE_MESSAGE_TYPE,
					 ISO_MESSAGE_TYPE_LEN)))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}
INT ncjcb2_check_if_trx_is_jcb_Swiftpass_BDO_Pay_credit_trx_with_pin( pAUTH_TX p_auth_tx  )
{
	if((true == ncjcb2_check_if_PEM_is_Swiftpass_BDO_Pay_credit_trx_with_pin(p_auth_tx)) &&
	   (true == ncjcb2_check_if_trx_is_bdo_pos_acq_trx(p_auth_tx))&&
	   (true == ncjcb2_check_if_trx_qualify_for_Swiftpass_BDO_Pay_credit_trx(p_auth_tx)))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

INT ncjcb2_check_if_trx_is_jcb_Swiftpass_BDO_Pay_credit_trx_without_pin( pAUTH_TX p_auth_tx  )
{
	if((true == ncjcb2_check_if_PEM_is_Swiftpass_BDO_Pay_credit_trx_without_pin(p_auth_tx)) &&
	   (true == ncjcb2_check_if_trx_is_bdo_pos_acq_trx(p_auth_tx))&&
	   (true == ncjcb2_check_if_trx_qualify_for_Swiftpass_BDO_Pay_credit_trx(p_auth_tx)))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

/*****************************/

/*Ecommerce Entry Mode From Swiftpass BDOPay Trx*/
INT ncjcb2_check_if_trx_qualify_for_ecommerce_Swiftpass_BDOPay_trx (  pAUTH_TX p_auth_tx)
{
	if((0 == strncmp(p_auth_tx->TLF01_details.message_type,
			 	 	 POS_ACQ_SALE_MESSAGE_TYPE,
					 ISO_MESSAGE_TYPE_LEN))||
	   (0 == strncmp(p_auth_tx->TLF01_details.message_type,
			   	     POS_ACQ_AUTH_MESSAGE_TYPE,
					 ISO_MESSAGE_TYPE_LEN))||
	   (0 == strncmp(p_auth_tx->TLF01_details.message_type,
			   	   	 POS_ACQ_REVERSAL_MESSAGE_TYPE,
					 ISO_MESSAGE_TYPE_LEN))||
	   (0 == strncmp(p_auth_tx->TLF01_details.message_type,
			   	   	 POS_ACQ_REVERSAL_ADVICE_MESSAGE_TYPE,
					 ISO_MESSAGE_TYPE_LEN)))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

INT ncjcb2_check_if_trx_ecommerce_POS_entry_mode_from_swiftpass_BDOPay_trx( pAUTH_TX p_auth_tx )
{
	if((0 == strncmp(p_auth_tx->TLF01_details.pos_entry_mode,
					 POS_ECOMMERCE_ENTRY_MODE_FROM_SWIFTPASS_BDOPAY_TXN_PEM,
					 POS_ENTRY_MODE_LEN))&&
	   (0 == strncmp(p_auth_tx->TLF01_details.pos_condition_code,
			   	   	 POS_ECOM_CONDITION_CODE,
					 POS_CONDITION_CODE_LEN)))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

INT ncjcb2_check_if_trx_is_jcb_ecommerce_swiftpass_BDOPay_trx( pAUTH_TX p_auth_tx )
{
	if((true == ncjcb2_check_if_trx_ecommerce_POS_entry_mode_from_swiftpass_BDOPay_trx(p_auth_tx)) &&
	   (true == ncjcb2_check_if_trx_is_bdo_pos_acq_trx(p_auth_tx))&&
	   (true == ncjcb2_check_if_trx_qualify_for_ecommerce_Swiftpass_BDOPay_trx(p_auth_tx)))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

INT ncjcb2_check_if_sbatch_qualify_for_MobilePOS_NFC_Tap2Phone_trx_with_pin( pAUTH_TX p_auth_tx  )
{
	 if((true == ncjcb2_check_if_trx_is_sbatch(p_auth_tx) ) &&
		(true == ncjcb2_is_valid_BDO_pos_acq_trn(p_auth_tx)) &&
		(true == ncjcb2_check_if_PEM_is_MobilePOS_NFC_Tap2Phone_trx_with_pin(p_auth_tx)))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

INT ncjcb2_check_if_sbatch_qualify_for_MobilePOS_NFC_Tap2Phone_trx_without_pin( pAUTH_TX p_auth_tx  )
{
	 if((true == ncjcb2_check_if_trx_is_sbatch(p_auth_tx) ) &&
		(true == ncjcb2_is_valid_BDO_pos_acq_trn(p_auth_tx)) &&
		(true == ncjcb2_check_if_PEM_is_MobilePOS_NFC_Tap2Phone_trx_without_pin(p_auth_tx)))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

INT ncjcb2_check_if_sbatch_qualify_for_Swiftpass_BDO_Pay_credit_trx_with_pin( pAUTH_TX p_auth_tx  )
{
	 if((true == ncjcb2_check_if_trx_is_sbatch(p_auth_tx) ) &&
		(true == ncjcb2_is_valid_BDO_pos_acq_trn(p_auth_tx)) &&
		(true == ncjcb2_check_if_PEM_is_Swiftpass_BDO_Pay_credit_trx_with_pin(p_auth_tx)))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

INT ncjcb2_check_if_sbatch_qualify_for_Swiftpass_BDO_Pay_credit_trx_without_pin( pAUTH_TX p_auth_tx  )
{
	 if((true == ncjcb2_check_if_trx_is_sbatch(p_auth_tx) ) &&
		(true == ncjcb2_is_valid_BDO_pos_acq_trn(p_auth_tx)) &&
		(true == ncjcb2_check_if_PEM_is_Swiftpass_BDO_Pay_credit_trx_without_pin(p_auth_tx)))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

INT ncjcb2_check_if_sbatch_qualify_for_ecommerce_swiftpass_BDOPay_trx( pAUTH_TX p_auth_tx )
{
	if((true == ncjcb2_check_if_trx_is_sbatch(p_auth_tx) ) &&
	   (true == ncjcb2_is_valid_BDO_pos_acq_trn(p_auth_tx)) &&
	   (true == ncjcb2_check_if_trx_ecommerce_POS_entry_mode_from_swiftpass_BDOPay_trx(p_auth_tx)))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}


/*********************************************/

