/*******************************************************************************
* Copyright (c) 2008, Hypercom, Inc.
* All Rights Reserved.
*  
* MODULE:       visautil.c
*  
* TITLE:        Visa Base I Network Interface - Utilities
*  
*	DESCRIPTION: This module contains all the utilities used my the 
*               host handler to  parse and manipulate all incoming 
*               messages from the terminal and the host.
*
* APPLICATION: 
*
*
*******************************************************************/

#include <stdlib.h>
#include <stdio.h>

#include <time.h>
#include "basictyp.h"
#include "pte.h"
#include "ptemsg.h"
#include "pteipc.h"
#include "ptetime.h"
#include "ptetimer.h"
#include "ntutils.h"
#include "visautil.h"
#include "visatran.h"

#include "app_info.h"
#include "equitdb.h"
#include "nc_dbstruct.h"
#include "tx_dbstruct.h"
#include "visamain.h"
#include "memmnger.h"
#include "txutils.h"
#include "genutil.h"
#include "txtrace.h"

/* functions in other parts of host handler */
extern void   perform_error_processing(pAUTH_TX);
extern BOOLEAN send_it_back_to_terminal(BYTE[3], pAUTH_TX);


/**********************************
     VARIABLES
**********************************/
/* local variables */
INT    reqidx;                  /* next open position in request buffer */
INT    buflen;                  /* total size of buffer */
INT    bitmapidx;               /* offset of bitmap... */
INT    invalid_msg_format;
INT    use_3_bitmaps;
INT    de23_present;
CHAR   applnk_que_name [] = "applnkA" ;
CHAR   security_que_name [] = "racalA" ;

/* global variables */
extern NCF01       ncf01_i;
extern NCF01       ncf01_a;
extern BYTE     current_state[2];
extern CHAR     AppName[];
extern INT      Third_Bitmap;


extern CHAR  ncvis2_Error_warning_Filename[256] ;
extern CHAR  ncvis2_module_error_warning_file_name[256] ;
extern CHAR  ncvis2_error_warning_file_path[256];

extern CHAR  Log_Bufr[512]; /* used to copy term id, card num and merch id to error msg
	   	   	   	   	   	   	   	   and passed it to print in File and monitor */
CHAR masked_card_num[20] = {0};
UINT Max_File_Size_Defined = 0 ;
/* Database structure sizes */
extern INT    Tlf01_Size;
extern INT    Ncf01_Size;
extern INT    Ncf30_Size;
extern INT    Auth_Tx_Size;

extern CHAR  updateds_que_name[];
extern CHAR  trands_que_name [];
extern CHAR  devds_que_name [];
extern CHAR  netds_que_name [];
extern CHAR  authorizer_que_name [];
extern CHAR  cur_app_name[];
extern CHAR  oprmon_que_name[];
extern CHAR  dialog_que_name [];
extern CHAR  vLogon_Resp_Indicator[];
extern CHAR  vLogon_NMI_Code[];
extern BYTE  orig_host_start_time[];
extern CHAR  SHARED_MEMORY_TBL[];
extern CHAR  ServiceCode[];

CHAR Dest_Stat_Id[7];

/* Statistics for Monitor */
extern TXN_COUNTS     Txn_Counts;
extern MONITOR_STATS  Monitor_Stats;

/* Txn Tracing Info */
extern CHAR   TraceFile[];
extern INT    Tracing;
extern FILE   TraceFile_fp;
extern FILE   *pTraceFile_fp;
extern FILE   **ppTraceFile_fp;

/* Miscellaneous */
extern INT    MCC_Validation_Functionality;
extern INT    Txn_Is_A_Reject;

/*******************************************************/
/* REQUEST BUILD BUFFER                                */
/*******************************************************/
BYTE ReqRsp_Buffer [COMM_BUF_SZE];

#define  VISA_HDR_SIZE   22
#define  REJECT_VISA_HDR_SIZE 26

#define VISA2 1

char  A2E[4] = "A2E";    /*  ASCII to EBCDIC conversion */
char  E2A[4] = "E2A";    /*  EBCDIC to ASCII conversion */

INT rvrsl_attmpts_value_fr_timeout_visa=0;
INT rvrsl_attmpts_value_fr_laterspns_visa=0;

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


enum bitfields {
   bf_done           = 0,      /* end of list    */
   bf_1xbitfield     = 1,      /* extended bit field */
   bf_2pan           = 2,      /* pan            */
   bf_3proc          = 3,      /* processing code */
   bf_4amount        = 4,      /* amount   */
   bf_6crdbillamt    = 6,      /* amount cardholder billing */
   bf_7trntime       = 7,      /* transmission date-time */
   bf_10convrate     = 10,     /* Conversion rate, cardholder billing */
   bf_11stan         = 11,     /* system trace number */
   bf_12time         = 12,     /* time    */
   bf_13date         = 13,     /* date, trans  */
   bf_14expiry       = 14,     /* expiry date  */
   bf_15datestl      = 15,     /* settlement date */
   bf_18sicnumber    = 18,     /* Merchant's type of business */
   bf_19ctrycd       = 19,     /* Acquiring institution's country code */
   bf_20panctrycd    = 20,     /* pan extended, country code */
   bf_22pose         = 22,     /* pos entry mode */
   bf_23cardseqnum   = 23,     /* card sequence number */
   bf_24nii          = 24,     /* nii      */
   bf_25pocc         = 25,     /* pos condition code */
   bf_26pincc        = 26,     /* pin capture code */
   bf_28feeamount    = 28,     /* Transaction Fee Amount */ /* Girija Y TF Apr 06 2009 */
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
   bf_51crdcurcd     = 51,     /* card currency code */
   bf_52pinblock     = 52,     /* pin block    */
   bf_53seccntl      = 53,     /* security related control info */
   bf_54addlamts     = 54,     /* additional amounts */
   bf_55ICCdata		 = 55,
   bf_56pad	 		 = 56,     /* PAN based Transaction*/
   bf_59posdata      = 59,     /* national point-of-service geographic data */
   bf_60priv60       = 60,     /* private field 60 */
   bf_61priv61       = 61,     /* private field 61 */
   bf_62priv62       = 62,     /* private field 62 */
   bf_63priv63       = 63,     /* private field 63 */
   bf_2xbitfield     = 65,     /* third bit field */
   bf_70nmicode      = 70,     /* network management information code */
   bf_73action       = 73,     /* Date, action */
   bf_90orgdata      = 90,     /* original data elements   */
   bf_91fileupdate   = 91,     /* file update code         */
   bf_93rspind       = 93,     /* response indicator       */
   bf_95replamts     = 95,     /* replacement amounts      */
   bf_96msgsec       = 96,     /* message security code    */
   bf_100rcvid       = 100,    /* receiving inst id code   */
   bf_101fname       = 101,    /* file name                */
   bf_102acct1       = 102,    /* acct id 1                */
   bf_103acct2       = 103,    /* acct id 2                */
   bf_104trandesc    = 104,    /* transaction description  */
   bf_116issref      = 116,    /* card issuer reference data      */
   bf_118intractry   = 118,    /* intra country data       */
   bf_120orgmsgid    = 120,    /* original message type id */
   bf_121issid       = 121,    /* issuing inst id code     */
   bf_122openuse     = 122,    /* remaining open-to-use    */
   bf_123addverf     = 123,    /* address verification data*/
   bf_124freeform    = 124,    /* free form text - Japan   */
   bf_125supinfo     = 125,    /* cris alert               */
   bf_126priv126     = 126,    /* private fields VSEC,CVV2 */
   bf_127filerecs    = 127,    /* free form text - Japan   */
   bf_130termcap     = 130,    /* terminal capability profile     */
   bf_131tvr         = 131,    /* terminal verifictation results  */
   bf_132unpredict   = 132,    /* unpredictable number            */
   bf_133termserial  = 133,    /* terminal serial number          */
   bf_134visadata    = 134,    /* Visa discretionary data         */
   bf_135issdata     = 135,    /* issuer discretionary data       */
   bf_136crytogram   = 136,    /* cryptogram                      */
   bf_137apptranctr  = 137,    /* application transaction counter */
   bf_138app_profile = 138,    /* application interchange profile */
   bf_139arpcresp    = 139,    /* ARPC Response crytpo and code   */
   bf_140issauth     = 140,    /* Issuer Authentication Data      */
   bf_142iss_script  = 142,    /* Issuer script                   */
   bf_143iss_result  = 143,    /* Issuer script result            */
   bf_144crytpotype  = 144,    /* cryptogram transaction type     */
   bf_145tcountry    = 145,    /* terminal country code           */
   bf_146termtxndate = 146,    /* terminal transaction date       */
   bf_147cryptamt    = 147,    /* cryptogram amount               */
   bf_148cryptcurcd  = 148,    /* cryptogram currency code        */
   bf_149cryptcash   = 149     /* cryptogram cash back amount     */
   }; 

/* LENGTHs FOR EACH BIT FIELD */

#define bf2_len       2
#define bf2_max_len  19
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
#define bf20_len      3
#define bf22_len      3
#define bf32_len      2
#define bf32data_len 11
#define bf33_len      2
#define bf34_len	  2
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


       
#define THIRD_BITFIELD    192
#define EXTENDED_BITFIELD 128
#define NORMAL_BITFIELD    64
INT     BitFieldSize = NORMAL_BITFIELD;


struct trandef
{
   BYTE   td_trantype;           /* tran type for this record */
   BYTE   td_msgid[2];           /* iso 8583 message id       */
   BYTE   td_proccode[3];        /* iso 8583 processing code  */
   enum	  tranopts1  td_opts1;   /* options */
   enum   tranopts2  td_opts2;
   enum   bitfields  td_bitfields[EXTENDED_BITFIELD]; /* fields to put in message */
};

static  struct trandef TRANTABLE[] = {
      { 
      /********************************************
         S A L E
      ********************************************/
      AUTH_SALE,
      0x01,0x00,              /* message id */
      0x00,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,            /* MAND */
      bf_3proc,           /* MAND */
      bf_4amount,         /* MAND */
	  bf_6crdbillamt,     // Phani
      bf_7trntime,        /* MAND */
      bf_10convrate, // ThoughtFocus nirbhay : additional requirements
      bf_11stan,          /* MAND */
      bf_14expiry,        /* COND */
      bf_18sicnumber,     /* MAND */
      bf_19ctrycd,        /* MAND */
      bf_22pose,          /* COND */
      bf_23cardseqnum,    /* COND */
      bf_25pocc,          /* MAND */
	  bf_28feeamount,     /* COND */  /* Girija Y TF, Apr 2009 Mandates */
      bf_32idcode,        /* MAND */
      bf_35track2,        /* COND */
      bf_37rrn,           /* MAND */
      bf_41termid,        /* COND */
      bf_42cardacc,       /* MAND */
      bf_43cardaccname,   /* COND */
	  bf_44respdata,      /* COND */
      bf_45track1,        /* COND */
      bf_49trncurcd,      /* MAND */
	  bf_51crdcurcd,      //Phani
      bf_52pinblock,      /* COND */
      bf_53seccntl,       /* COND */
	  bf_55ICCdata,
	  bf_56pad,
      bf_60priv60,        /* MAND */
      bf_62priv62,        /* COND */
      bf_63priv63,        /* MAND */
	  bf_104trandesc,	  /* COND */
      bf_126priv126,      /* COND */
      bf_130termcap,      /* COND */
      bf_131tvr,          /* COND */
      bf_132unpredict,    /* COND */
      bf_133termserial,   /* COND */
      bf_134visadata,     /* COND */
      bf_135issdata,      /* COND */
      bf_136crytogram,    /* COND */
      bf_137apptranctr,   /* COND */
      bf_138app_profile,  /* COND */
      bf_144crytpotype,   /* COND */
      bf_145tcountry,     /* COND */
      bf_146termtxndate,  /* COND */
      bf_147cryptamt,     /* COND */
      bf_148cryptcurcd,   /* COND */
      bf_149cryptcash,    /* COND */
      bf_done      /* end of fields to move */
      },
      { 
      /********************************************
         S A L E  R E S P O N S E
      ********************************************/
      AUTH_SALE_RESPONSE,
      0x01,0x10,              /* message id */
      0x00,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,       /* MAND */
      bf_3proc,      /* MAND */
      bf_4amount,    /* MAND */
      bf_7trntime,   /* MAND */
      bf_11stan,     /* MAND */
      bf_19ctrycd,   /* MAND */
      bf_20panctrycd,/* COND */
      bf_25pocc,     /* COND */
      bf_32idcode,   /* MAND */
      bf_34panext,   /* COND */
      bf_37rrn,      /* MAND */
      bf_38authcode, /* COND */
      bf_39respcode, /* MAND */
      bf_41termid,   /* COND */
      bf_42cardacc,  /* MAND */
      bf_44respdata, /* COND */
      bf_49trncurcd, /* MAND */
      bf_54addlamts, /* COND */
	  bf_56pad,
      bf_62priv62,   /* COND */
      bf_63priv63,   /* MAND */
      bf_102acct1,   /* COND */
      bf_103acct2,   /* COND */
      bf_121issid,   /* COND */
      bf_done      /* end of fields to move */
      },
      {
      /********************************************
         R E F U N D 
      ********************************************/
      AUTH_REFUND,
      0x01,0x00,              /* message id */
      0x20,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_credit+topt1_postadv,
      topt2_reversal,
      /**** BIT FIELDS ****/
      bf_2pan,      /* MAND */
      bf_3proc,		/* MAND */
      bf_4amount,	/* MAND */
      bf_7trntime,  /* MAND */
      bf_10convrate, // ThoughtFocus nirbhay : additional requirements
      bf_11stan,	/* MAND */
      bf_14expiry,	/* COND */
      bf_18sicnumber,/* MAND */
      bf_19ctrycd,  /* MAND */
      bf_22pose,	/* COND */
      bf_23cardseqnum, /* COND */
      bf_25pocc,	/* MAND */
      bf_32idcode,  /* MAND */
      bf_35track2,	/* COND */
      bf_37rrn,     /* MAND */
      bf_41termid,	/* COND */
      bf_42cardacc,	/* MAND */
	  bf_44respdata,      /* COND */
      bf_45track1,	/* COND */
      bf_49trncurcd, /* MAND */
	  bf_56pad,
      bf_60priv60,   /* MAND */
      bf_63priv63,        /* MAND */
	  bf_104trandesc,	  /* COND */  // TF-Ajay April2009 visa mandates
      bf_126priv126,
      bf_130termcap,      /* COND */
      bf_131tvr,          /* COND */
      bf_132unpredict,    /* COND */
      bf_133termserial,   /* COND */
      bf_134visadata,     /* COND */
      bf_135issdata,      /* COND */
      bf_136crytogram,    /* COND */
      bf_137apptranctr,   /* COND */
      bf_138app_profile,  /* COND */
      bf_144crytpotype,   /* COND */
      bf_145tcountry,     /* COND */
      bf_146termtxndate,  /* COND */
      bf_147cryptamt,     /* COND */
      bf_148cryptcurcd,   /* COND */
      bf_149cryptcash,    /* COND */
      bf_done      /* end of fields to move */
       },
	  {
	  /********************************************
		 AUTH REFUND  R E S P O N S E
	  ********************************************/
	  AUTH_REFUND_RESPONSE,
	  0x01,0x10,              /* message id */
	  0x20,0x00,0x00,         /* processing code */
	  /**** OPTIONS ****/
	  topt1_postadv,
	  topt2_referral+topt2_reversal+topt2_floor,
	  /**** BIT FIELDS ****/
	  bf_2pan,       /* MAND */
	  bf_3proc,      /* MAND */
	  bf_4amount,    /* MAND */
	  bf_7trntime,   /* MAND */
	  bf_11stan,     /* MAND */
	  bf_19ctrycd,   /* MAND */
	  bf_20panctrycd,/* COND */
	  bf_25pocc,     /* COND */
	  bf_32idcode,   /* MAND */
	  bf_34panext,   /* COND */
	  bf_37rrn,      /* MAND */
	  bf_38authcode, /* COND */
	  bf_39respcode, /* MAND */
	  bf_41termid,   /* COND */
	  bf_42cardacc,  /* MAND */
	  bf_44respdata, /* COND */
	  bf_49trncurcd, /* MAND */
	  bf_54addlamts, /* COND */
	  bf_55ICCdata,
	  bf_56pad,
	  bf_62priv62,   /* COND */
	  bf_63priv63,   /* MAND */
	  bf_102acct1,   /* COND */
	  bf_103acct2,   /* COND */
	  bf_121issid,   /* COND */
	  bf_done      /* end of fields to move */
	  },
      {
	  /********************************************
         P A Y M E N T 
      ********************************************/
      AUTH_PAYMENT,
      0x01,0x00,              /* message id */
      0x40,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,      /* MAND */
      bf_3proc,		/* MAND */
      bf_4amount,	/* MAND */
      bf_7trntime,  /* MAND */
      bf_10convrate, // ThoughtFocus nirbhay : additional requirements
      bf_11stan,	/* MAND */
      bf_14expiry,	/* COND */
      bf_18sicnumber,/* MAND */
      bf_19ctrycd,  /* MAND */
      bf_22pose,	/* COND */
      bf_25pocc,	/* MAND */
	  bf_28feeamount,     /* COND */  /* Girija Y TF, Apr 2009 Mandates */
      bf_32idcode,  /* MAND */
      bf_35track2,	/* COND */
      bf_37rrn,     /* MAND */
      bf_41termid,	/* COND */
      bf_42cardacc,	/* MAND */
	  bf_44respdata,      /* COND */
      bf_45track1,	/* COND */
      bf_49trncurcd,/* MAND */
	  bf_51crdcurcd,      //Phani
	  bf_56pad,
      bf_60priv60,	/* MAND */
      bf_63priv63,        /* MAND */
	  bf_104trandesc,	  /* COND */  // TF-Ajay April2009 visa mandates
	  bf_126priv126,
      bf_done      /* end of fields to move */
	  
	  },
	  {
	  /********************************************
	          MONEY SEND
	        ********************************************/
	        AUTH_MONEY_SEND,
	        0x01,0x00,              /* message id */
	        0x28,0x00,0x00,         /* processing code */
	        /**** OPTIONS ****/
	        topt1_postadv,
	        topt2_referral+topt2_reversal+topt2_floor,
	        /**** BIT FIELDS ****/
	        bf_2pan,       /* MAND */
	        bf_3proc,      /* MAND */
	        bf_4amount,    /* MAND */
	        bf_7trntime,  	  /* MAND */
	        bf_11stan,   	      /* MAND */
	  	    bf_12time,          /* MAND */
	  	    bf_13date,          /* MAND */
	        bf_14expiry,   /* COND */
	        bf_18sicnumber,/* MAND */
	        bf_19ctrycd,   /* MAND */
	        bf_22pose,     /* COND */
	        bf_25pocc,     /* MAND */
	        bf_32idcode,   /* MAND */
	  	    bf_35track2,   /* COND */
	        bf_37rrn,      /* MAND */
	        bf_41termid,   /* COND */
	        bf_42cardacc,  /* MAND */
	        bf_43cardaccname,/* COND */
	        bf_45track1,   /* COND */
	        bf_49trncurcd, /* MAND */
	  	    bf_51crdcurcd,
	  	    bf_55ICCdata,
	  	  	bf_56pad,
	        bf_60priv60,   /* MAND */
	        bf_62priv62,     /* COND */
	        bf_63priv63,   /* MAND */
			bf_126priv126, /* COND */
	        bf_done      /* end of fields to move */
	  },
	  {
	        AUTH_MONEY_SEND_RESPONSE,
	        0x01,0x10,              /* message id */
	        0x00,0x00,0x00,         /* processing code */
	        /**** OPTIONS ****/
	        topt1_postadv,
	        topt2_referral+topt2_reversal+topt2_floor,
	        /**** BIT FIELDS ****/
	        bf_2pan,       /* MAND */
	        bf_3proc,		/* MAND */
	        bf_4amount,	   /* MAND */
	        bf_7trntime,   /* MAND */
	        bf_11stan,	   /* MAND */
	        bf_19ctrycd,   /* MAND */
	        bf_20panctrycd,/* COND */
	        bf_25pocc,     /* COND */
	        bf_32idcode,   /* MAND */
			bf_33fwdidcode,  /* COND */
	        bf_34panext,   /* COND */
	        bf_37rrn,      /* MAND */
	        bf_38authcode, /* COND */
	        bf_39respcode, /* MAND */
	        bf_41termid,	/* COND */
	        bf_42cardacc,	/* MAND */
	        bf_44respdata, /* COND */
	        bf_49trncurcd, /* MAND */
	  	  bf_55ICCdata,
	  	  bf_56pad,
	        bf_62priv62,   /* COND */
	        bf_63priv63,   /* MAND */
	        bf_102acct1,   /* COND */
	        bf_103acct2,   /* COND */
	        bf_121issid,   /* COND */
	        bf_done      /* end of fields to move */
	  },
      {   
      /********************************************
         R E V E R S A L   R E P E A T
      ********************************************/
      AUTH_REVERSAL_REPEAT,
      0x04,0x01,              /* message id */
      0x00,0x00,0x00,         /* processing code  (use original) */
      /**** OPTIONS ****/
      topt1_credit+topt1_postadv,
      topt2_batpend,
      /**** BIT FIELDS ****/
      bf_2pan,        /* COND */
      bf_3proc,       /* MAND */
      bf_4amount,     /* MAND */
	  bf_6crdbillamt, /* COND */ // Dec 15,2009 TF-Ajay
      bf_7trntime,    /* MAND */
      bf_11stan,      /* MAND */
      bf_10convrate, // ThoughtFocus nirbhay : additional requirements
      bf_14expiry,    /* COND */
      bf_18sicnumber, /* MAND */
      bf_19ctrycd,    /* MAND */
      bf_22pose,      /* COND */
      bf_23cardseqnum,/* COND */
      bf_25pocc,      /* MAND */
	  bf_28feeamount,     /* COND */  /* Girija Y TF, Apr 2009 Mandates */
      bf_32idcode,    /* MAND */
      bf_33fwdidcode, /* COND */
      bf_37rrn,       /* MAND */
      bf_38authcode,  /* MAND */
      bf_41termid,    /* MAND */
      bf_42cardacc,   /* MAND */
      bf_43cardaccname,/*COND */
      bf_49trncurcd,  /* MAND */
	  bf_51crdcurcd,  /* MAND */// Dec 15,2009 TF-Ajay 
	  bf_56pad,
      bf_60priv60,    /* MAND */
      bf_62priv62,    /* COND */
      bf_63priv63,    /* MAND */
      bf_90orgdata,   /* MAND */
      bf_95replamts,  /* COND */
      bf_131tvr,      /* COND */
      bf_133termserial,/*COND */
      bf_134visadata,  /*COND */
      bf_137apptranctr,/*COND */
      bf_done         /* end of fields to move */
      },

      {
      /********************************************
         R E V E R S A L    R E S P O N S E
      ********************************************/
      AUTH_REVERSAL_RESPONSE,
      0x04,0x10,              /* message id */
      0x00,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_credit+topt1_postadv,
      topt2_batpend,
      /**** BIT FIELDS ****/
      bf_2pan,        /* MAND */
      bf_3proc,	    /* MAND */
      bf_4amount,	    /* MAND */
      bf_7trntime,    /* MAND */
      bf_11stan,	    /* MAND */
      bf_19ctrycd,    /* MAND */
      bf_20panctrycd, /* COND */
      bf_25pocc,	    /* MAND */
      bf_32idcode,    /* MAND */
      bf_34panext,    /* COND */
      bf_37rrn,       /* MAND */
      bf_39respcode,  /* MAND */
      bf_41termid,    /* COND */
      bf_42cardacc,   /* MAND */
      bf_48addl_data, /* OPT  */
      bf_49trncurcd,  /* MAND */
	  bf_56pad,
      bf_62priv62,    /* OPT  */
      bf_63priv63,    /* MAND */
      bf_95replamts,  /* COND */
      bf_102acct1,    /* COND */
      bf_103acct2,    /* COND */
      bf_104trandesc, /* COND */
      bf_118intractry,/* OPT  */
      bf_121issid,    /* COND */
      bf_done      /* end of fields to move */
      },

      {   
      /********************************************
         R E V E R S A L  
      ********************************************/
      AUTH_REVERSAL,
      0x04,0x00,              /* message id */
      0x00,0x00,0x00,         /* processing code  (use original) */
      /**** OPTIONS ****/
      topt1_credit+topt1_postadv,
      topt2_batpend,
      /**** BIT FIELDS ****/
      bf_2pan,        /* COND */
      bf_3proc,       /* MAND */
      bf_4amount,     /* MAND */
      bf_6crdbillamt,     // Phani
      bf_7trntime,    /* MAND */
      bf_10convrate, // ThoughtFocus nirbhay : additional requirements
      bf_11stan,      /* MAND */
      bf_14expiry,    /* COND */
      bf_18sicnumber, /* MAND */
      bf_19ctrycd,    /* MAND */
      bf_22pose,      /* COND */
      bf_23cardseqnum,/* COND */
      bf_25pocc,      /* MAND */
	  bf_28feeamount,     /* COND */  /* Girija Y TF, Apr 2009 Mandates */
      bf_32idcode,    /* MAND */
      bf_33fwdidcode, /* COND */
      bf_37rrn,       /* MAND */
      bf_38authcode,  /* MAND */
      bf_41termid,    /* MAND */
      bf_42cardacc,   /* MAND */
      bf_43cardaccname,/*COND */
      bf_49trncurcd,  /* MAND */
	  bf_51crdcurcd,      //Phani
	  bf_56pad,
      bf_60priv60,    /* MAND */
      bf_62priv62,    /* COND */
      bf_63priv63,    /* MAND */
      bf_90orgdata,   /* MAND */
      bf_95replamts,  /* COND */
	  bf_126priv126,
      bf_131tvr,      /* COND */
      bf_133termserial,/*COND */
      bf_134visadata,  /*COND */
      bf_137apptranctr,/*COND */
      bf_done         /* end of fields to move */
      },
#if BDOR_62_PREAUTH_VOID
      {
      /********************************************
        VOID PRE AUTHORIZATION
      ********************************************/
      AUTH_VOID_PRE_AUTHORIZATION,
      0x04,0x00,              /* message id */
      0x00,0x00,0x00,         /* processing code  (use original) */
      /**** OPTIONS ****/
      topt1_credit+topt1_postadv,
      topt2_batpend,
      /**** BIT FIELDS ****/
      bf_2pan,        /* COND */
      bf_3proc,       /* MAND */
      bf_4amount,     /* MAND */
      bf_6crdbillamt,     // Phani
      bf_7trntime,    /* MAND */
      bf_10convrate, // ThoughtFocus nirbhay : additional requirements
      bf_11stan,      /* MAND */
      bf_14expiry,    /* COND */
      bf_18sicnumber, /* MAND */
      bf_19ctrycd,    /* MAND */
      bf_22pose,      /* COND */
      bf_23cardseqnum,/* COND */
      bf_25pocc,      /* MAND */
	  bf_28feeamount,     /* COND */  /* Girija Y TF, Apr 2009 Mandates */
      bf_32idcode,    /* MAND */
      bf_33fwdidcode, /* COND */
      bf_37rrn,       /* MAND */
      bf_38authcode,  /* MAND */
      bf_41termid,    /* MAND */
      bf_42cardacc,   /* MAND */
      bf_43cardaccname,/*COND */
      bf_49trncurcd,  /* MAND */
	  bf_51crdcurcd,      //Phani
	  bf_56pad,
      bf_60priv60,    /* MAND */
      bf_62priv62,    /* COND */
      bf_63priv63,    /* MAND */
      bf_90orgdata,   /* MAND */
      bf_95replamts,  /* COND */
	  bf_126priv126,
      bf_131tvr,      /* COND */
      bf_133termserial,/*COND */
      bf_134visadata,  /*COND */
      bf_137apptranctr,/*COND */
      bf_done         /* end of fields to move */
      },

      {
      /********************************************
         VOID PRE AUTHORIZATION RESPONSE
      ********************************************/
      AUTH_VOID_PRE_AUTHORIZATION_RESPONSE,
      0x04,0x10,              /* message id */
      0x00,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_credit+topt1_postadv,
      topt2_batpend,
      /**** BIT FIELDS ****/
      bf_2pan,        /* MAND */
      bf_3proc,	    /* MAND */
      bf_4amount,	    /* MAND */
      bf_7trntime,    /* MAND */
      bf_11stan,	    /* MAND */
      bf_19ctrycd,    /* MAND */
      bf_20panctrycd, /* COND */
      bf_25pocc,	    /* MAND */
      bf_32idcode,    /* MAND */
      bf_34panext,    /* COND */
      bf_37rrn,       /* MAND */
      bf_39respcode,  /* MAND */
      bf_41termid,    /* COND */
      bf_42cardacc,   /* MAND */
      bf_48addl_data, /* OPT  */
      bf_49trncurcd,  /* MAND */
	  bf_56pad,
      bf_62priv62,    /* OPT  */
      bf_63priv63,    /* MAND */
      bf_95replamts,  /* COND */
      bf_102acct1,    /* COND */
      bf_103acct2,    /* COND */
      bf_104trandesc, /* COND */
      bf_118intractry,/* OPT  */
      bf_121issid,    /* COND */
      bf_done      /* end of fields to move */
      },
#endif
      {
      /********************************************
         O F F L I N E   S A L E  
      ********************************************/
      AUTH_OFFLINE_SALE,
      0x02,0x20,              /* message id */
      0x00,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_credit+topt1_postadv,
      topt2_reversal,
      /**** BIT FIELDS ****/
      bf_2pan,      /* MAND */
      bf_3proc,     /* MAND */
      bf_4amount,   /* MAND */
      bf_7trntime,  /* MAND */
      bf_10convrate, // ThoughtFocus nirbhay : additional requirements
      bf_11stan,    /* MAND */
      bf_14expiry,  /* COND */
      bf_18sicnumber,/* MAND */
      bf_19ctrycd,  /* MAND */
      bf_22pose,    /* COND */
      bf_25pocc,    /* MAND */
      bf_32idcode,  /* MAND */
      bf_35track2,  /* COND */
      bf_37rrn,     /* MAND */
      bf_41termid,  /* COND */
      bf_42cardacc, /* MAND */
      bf_45track1,  /* COND */
      bf_49trncurcd,/* MAND */
	  bf_56pad,
      bf_59posdata, /* COND */
      bf_60priv60,  /* MAND */
      bf_62priv62,  /* COND */
      bf_63priv63,  /* MAND */
      bf_done      /* end of fields to move */
      },

      {
      /********************************************
         O F F L I N E   R E F U N D 
      ********************************************/
      AUTH_OFFLINE_REFUND,
      0x02,0x20,              /* message id */
      0x20,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_credit+topt1_postadv,
      topt2_reversal,
      /**** BIT FIELDS ****/
      bf_2pan,       /* MAND */
      bf_3proc,      /* MAND */
      bf_4amount,    /* MAND */
      bf_7trntime,   /* MAND */
      bf_10convrate, // ThoughtFocus nirbhay : additional requirements
      bf_11stan,     /* MAND */
      bf_14expiry,   /* COND */
      bf_18sicnumber,/* MAND */
      bf_19ctrycd,   /* MAND */
      bf_22pose,     /* COND */
      bf_25pocc,     /* MAND */
      bf_32idcode,   /* MAND */
      bf_35track2,   /* COND */
      bf_37rrn,      /* MAND */
      bf_41termid,   /* COND */
      bf_42cardacc,  /* MAND */
      bf_45track1,   /* COND */
      bf_49trncurcd, /* MAND */
	  bf_56pad,
      bf_59posdata,  /* COND */
      bf_60priv60,   /* MAND */
      bf_62priv62,   /* COND */
      bf_63priv63,   /* MAND */
      bf_done      /* end of fields to move */
      },
      
      { 
      /********************************************
         O N L I N E   F I L E   U P D A T E
      ********************************************/
      AUTH_CARD_UPDATE,
      0x03,0x02,              /* message id */
      0x00,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,         /* MAND */
      bf_7trntime,     /* MAND */
      bf_11stan,       /* MAND */
      bf_32idcode,     /* MAND */
      bf_37rrn,        /* MAND */
      bf_73action,     /* MAND */
      bf_91fileupdate, /* MAND */
      bf_101fname,     /* MAND */
      bf_127filerecs,  /* MAND */
      bf_done      /* end of fields to move */
      },

      {
      /********************************************
         S A L E   A D J U S T M E N T   
      ********************************************/
      AUTH_SALE_ADJUSTMENT,
      0x02,0x20,              /* message id */
      0x02,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_credit+topt1_postadv,
      topt2_reversal,
      /**** BIT FIELDS ****/
      bf_2pan,      /* MAND */
      bf_3proc,		/* MAND */
      bf_4amount,	/* MAND */
      bf_7trntime,  /* MAND */
      bf_10convrate, // ThoughtFocus nirbhay : additional requirements
      bf_11stan,	/* MAND */
      bf_14expiry,	/* COND */
      bf_18sicnumber,/* MAND */
      bf_19ctrycd,  /* MAND */
      bf_22pose,	/* COND */
      bf_25pocc,	/* MAND */
      bf_32idcode,  /* MAND */
      bf_35track2,	/* COND */
      bf_37rrn,     /* MAND */
      bf_41termid,	/* COND */
      bf_42cardacc,	/* MAND */
      bf_45track1,	/* COND */
      bf_49trncurcd,/* MAND */
	  bf_56pad,
      bf_59posdata, /* COND */
      bf_60priv60,  /* MAND */
      bf_62priv62,  /* COND */
      bf_63priv63,  /* MAND */
      bf_done      /* end of fields to move */
      },
      
	   {
      /********************************************
         R E F U N D    A D J U S T M E N T   
      ********************************************/
      AUTH_REFUND_ADJUSTMENT,
      0x02,0x20,              /* message id */
      0x22,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_credit+topt1_postadv,
      topt2_reversal,
      /**** BIT FIELDS ****/
      bf_2pan,      /* MAND */
      bf_3proc,		/* MAND */
      bf_4amount,	/* MAND */
      bf_7trntime,  /* MAND */
      bf_10convrate, // ThoughtFocus nirbhay : additional requirements
      bf_11stan,	/* MAND */
      bf_14expiry,	/* COND */
      bf_18sicnumber,/* MAND */
      bf_19ctrycd,  /* MAND */
      bf_22pose,	/* COND */
      bf_25pocc,	/* MAND */
      bf_32idcode,  /* MAND */
      bf_35track2,	/* COND */
      bf_37rrn,     /* MAND */
      bf_41termid,	/* COND */
      bf_42cardacc,	/* MAND */
      bf_45track1,	/* COND */
      bf_49trncurcd,/* MAND */
	  bf_56pad,
      bf_59posdata, /* COND */
      bf_60priv60,  /* MAND */
      bf_62priv62,  /* COND */
      bf_63priv63,  /* MAND */
      bf_done      /* end of fields to move */
      },
    
      {   
      /********************************************
         P A Y M E N T    A D J U S T M E N T   
      ********************************************/
      AUTH_PAYMENT_ADJUSTMENT,
      0x02,0x20,              /* message id */
      0x42,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_credit+topt1_postadv,
      topt2_reversal,
      /**** BIT FIELDS ****/
      bf_2pan,      /* MAND */
      bf_3proc,		/* MAND */
      bf_4amount,	/* MAND */
      bf_7trntime,  /* MAND */
      bf_10convrate, // ThoughtFocus nirbhay : additional requirements
      bf_11stan,	/* MAND */
      bf_14expiry,	/* COND */
      bf_18sicnumber,/* MAND */
      bf_19ctrycd,  /* MAND */
      bf_22pose,	/* COND */
      bf_25pocc,	/* MAND */
      bf_32idcode,  /* MAND */
      bf_35track2,	/* COND */
      bf_37rrn,     /* MAND */
      bf_41termid,	/* COND */
      bf_42cardacc,	/* MAND */
      bf_45track1,	/* COND */
      bf_49trncurcd,/* MAND */
	  bf_56pad,
      bf_59posdata, /* COND */
      bf_60priv60,  /* MAND */
      bf_62priv62,  /* COND */
      bf_63priv63,  /* MAND */
      bf_done      /* end of fields to move */
      },

      {
      /********************************************
         A U T H O R I Z A T I O N 
      ********************************************/
      AUTH_AUTHORIZATION,
      0x01,0x00,              /* message id */
      0x00,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,      /* MAND */
      bf_3proc,		/* MAND */
      bf_4amount,	/* MAND*/
      bf_6crdbillamt,  // Phani
      bf_7trntime,  /* MAND */
      bf_10convrate, // ThoughtFocus nirbhay : additional requirements
      bf_11stan,	/* MAND */
      bf_14expiry,	/* COND */
      bf_18sicnumber,/* MAND */
      bf_19ctrycd,  /* MAND */
      bf_22pose,	/* COND */
      bf_23cardseqnum, /* COND */
      bf_25pocc,	/* MAND */
	  bf_28feeamount,     /* COND */  /* Girija Y TF, Apr 2009 Mandates */
      bf_32idcode,  /* MAND */
      bf_35track2,	/* COND */
      bf_37rrn,     /* MAND */
      bf_41termid,	/* COND */
      bf_42cardacc,	/* MAND */
      bf_43cardaccname,	/* COND */
	  bf_44respdata,      /* COND */
      bf_45track1,	/* COND */
      bf_49trncurcd,/* MAND */
	  bf_51crdcurcd,      //Phani
      bf_52pinblock,/* COND */
      bf_53seccntl, /* COND */
	  bf_55ICCdata,
	  bf_56pad,
 	  bf_60priv60,	/* MAND */
	  bf_62priv62,  /* COND */
      bf_63priv63,  /* MAND */
	  bf_104trandesc,	  /* COND */  // TF-Ajay April2009 visa mandates
	  bf_126priv126,
      bf_130termcap,      /* COND */
      bf_131tvr,          /* COND */
      bf_132unpredict,    /* COND */
      bf_133termserial,   /* COND */
      bf_134visadata,     /* COND */
      bf_135issdata,      /* COND */
      bf_136crytogram,    /* COND */
      bf_137apptranctr,   /* COND */
      bf_138app_profile,  /* COND */
      bf_144crytpotype,   /* COND */
      bf_145tcountry,     /* COND */
      bf_146termtxndate,  /* COND */
      bf_147cryptamt,     /* COND */
      bf_148cryptcurcd,   /* COND */
      bf_149cryptcash,    /* COND */
      bf_done      /* end of fields to move */
      },

      {
      /**********************************************
         A U T H O R I Z A T I O N   R E S P O N S E
      ***********************************************/
      AUTH_AUTHORIZATION_RESPONSE,
      0x01,0x10,              /* message id */
      0x00,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,       /* MAND */
      bf_3proc,		/* MAND */
      bf_4amount,	   /* MAND */
      bf_7trntime,   /* MAND */
      bf_11stan,	   /* MAND */
      bf_19ctrycd,   /* MAND */
      bf_20panctrycd,/* COND */
      bf_25pocc,     /* COND */
      bf_32idcode,   /* MAND */
      bf_34panext,   /* COND */
      bf_37rrn,      /* MAND */
      bf_38authcode, /* COND */
      bf_39respcode, /* MAND */
      bf_41termid,	/* COND */
      bf_42cardacc,	/* MAND */
      bf_44respdata, /* COND */
      bf_49trncurcd, /* MAND */
	  bf_56pad,
      bf_62priv62,   /* COND */
      bf_63priv63,   /* MAND */
      bf_102acct1,   /* COND */
      bf_103acct2,   /* COND */
      bf_121issid,   /* COND */
      bf_done      /* end of fields to move */
      },

	  {  
	       /********************************************
	          P R E   A U T H O R I Z A T I O N
	       ********************************************/
	       AUTH_PRE_AUTHORIZATION,
	       0x01,0x00,              /* message id */
	       0x30,0x00,0x00,         /* processing code */
	       /**** OPTIONS ****/
	       topt1_postadv,
	       topt2_referral+topt2_reversal+topt2_floor,
	       /**** BIT FIELDS ****/
	       bf_2pan,            /* MAND */
	       bf_3proc,           /* MAND */
	       bf_4amount,         /* MAND */
	       bf_7trntime,        /* MAND */
	       bf_11stan,          /* MAND */
	       bf_14expiry,        /* COND */
	       bf_18sicnumber,     /* MAND */
	       bf_19ctrycd,        /* MAND */
	       bf_22pose,          /* COND */
	       bf_23cardseqnum,    /* COND */
	       bf_25pocc,          /* MAND */
	       bf_32idcode,        /* MAND */
	       bf_35track2,        /* COND */
	       bf_37rrn,           /* MAND */
	       bf_41termid,        /* COND */
	       bf_42cardacc,       /* MAND */
		   bf_44respdata,      /* COND */
	       bf_45track1,        /* COND */
	       bf_49trncurcd,      /* MAND */
		   bf_55ICCdata,
		   bf_56pad,
		   bf_60priv60,        /* MAND */
		   bf_62priv62,   	  /* COND */
	       bf_63priv63,        /* MAND */
		   bf_126priv126,      /* COND */
	       bf_130termcap,      /* COND */
	       bf_131tvr,          /* COND */
	       bf_132unpredict,    /* COND */
	       bf_133termserial,   /* COND */
	       bf_134visadata,     /* COND */
	       bf_135issdata,      /* COND */
	       bf_136crytogram,    /* COND */
	       bf_137apptranctr,   /* COND */
	       bf_138app_profile,  /* COND */
	       bf_144crytpotype,   /* COND */
	       bf_145tcountry,     /* COND */
	       bf_146termtxndate,  /* COND */
	       bf_147cryptamt,     /* COND */
	       bf_148cryptcurcd,   /* COND */
	       bf_149cryptcash,    /* COND */
	       bf_done      /* end of fields to move */
	       },
	 	  {
	 		/********************************************
	 		  PRE_ AUTH  R E S P O N S E
	 		********************************************/
	 		AUTH_PRE_AUTHORIZATION_RESPONSE,
	 		0x01,0x10,              /* message id */
	 		0x30,0x00,0x00,         /* processing code */
	 		/**** OPTIONS ****/
	 		topt1_postadv,
	 		topt2_referral+topt2_reversal+topt2_floor,
	 		/**** BIT FIELDS ****/
	 		bf_2pan,       /* MAND */
	 		bf_3proc,		/* MAND */
	 		bf_4amount,	   /* MAND */
	 		bf_7trntime,   /* MAND */
	 		bf_11stan,	   /* MAND */
	 		bf_19ctrycd,   /* MAND */
	 		bf_20panctrycd,/* COND */
	 		bf_25pocc,     /* COND */
	 		bf_32idcode,   /* MAND */
	 		bf_34panext,   /* COND */
	 		bf_37rrn,      /* MAND */
	 		bf_38authcode, /* COND */
	 		bf_39respcode, /* MAND */
	 		bf_41termid,	/* COND */
	 		bf_42cardacc,	/* MAND */
	 		bf_44respdata, /* COND */
	 		bf_49trncurcd, /* MAND */
	 		bf_54addlamts, /* COND */
	 		bf_55ICCdata,
			bf_56pad,
	 		bf_62priv62,   /* COND */
	 		bf_63priv63,   /* MAND */
	 		bf_102acct1,   /* COND */
	 		bf_103acct2,   /* COND */
	 		bf_121issid,   /* COND */
	 		bf_done      /* end of fields to move */
	 		},

      { 
      /********************************************
         C A S H   A D V A N C E 
      ********************************************/
      AUTH_CASH_ADVANCE,
      0x01,0x00,              /* message id */
      0x01,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,      /* MAND */
      bf_3proc,		/* MAND */
      bf_4amount,	/* MAND */
	  bf_6crdbillamt,     // Phani
      bf_7trntime,  /* MAND */
      bf_10convrate, // ThoughtFocus nirbhay : additional requirements
      bf_11stan,	/* MAND */
      bf_14expiry,	/* COND */
      bf_18sicnumber,/* MAND */
      bf_19ctrycd,  /* MAND */
      bf_22pose,	/* COND */
      bf_23cardseqnum, /* COND */
      bf_25pocc,	/* MAND */
	  bf_28feeamount,     /* COND */  /* Girija Y TF, Apr 2009 Mandates */
      bf_32idcode,  /* MAND */
      bf_35track2,	/* COND */
      bf_37rrn,     /* MAND */
      bf_41termid,	/* COND */
      bf_42cardacc,	/* MAND */
      bf_43cardaccname,	/* COND */
	  bf_44respdata,      /* COND */
      bf_45track1,	/* COND */
      bf_49trncurcd,/* MAND */
	  bf_51crdcurcd,      //Phani
      bf_52pinblock,/* COND */
      bf_53seccntl, /* COND */
	  bf_55ICCdata,
	  bf_56pad,
      bf_60priv60,	/* MAND */
      bf_62priv62,  /* COND */
      bf_63priv63,        /* MAND */
	  bf_104trandesc,	  /* COND */  // TF-Ajay April2009 visa mandates
	  bf_126priv126,
      bf_130termcap,      /* COND */
      bf_131tvr,          /* COND */
      bf_132unpredict,    /* COND */
      bf_133termserial,   /* COND */
      bf_134visadata,     /* COND */
      bf_135issdata,      /* COND */
      bf_136crytogram,    /* COND */
      bf_137apptranctr,   /* COND */
      bf_138app_profile,  /* COND */
      bf_144crytpotype,   /* COND */
      bf_145tcountry,     /* COND */
      bf_146termtxndate,  /* COND */
      bf_147cryptamt,     /* COND */
      bf_148cryptcurcd,   /* COND */
      bf_149cryptcash,    /* COND */
      bf_done      /* end of fields to move */
      },

      { 
      /********************************************
         C A S H   A D V A N C E   R E S P O N S E
      ********************************************/
      AUTH_CASH_ADVANCE_RESPONSE,
      0x01,0x10,              /* message id */
      0x01,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,       /* MAND */
      bf_3proc,		/* MAND */
      bf_4amount,	   /* MAND */
      bf_7trntime,   /* MAND */
      bf_11stan,	   /* MAND */
      bf_19ctrycd,   /* MAND */
      bf_20panctrycd,/* COND */
      bf_25pocc,     /* COND */
      bf_32idcode,   /* MAND */
      bf_34panext,   /* COND */
      bf_37rrn,      /* MAND */
      bf_38authcode, /* COND */
      bf_39respcode, /* MAND */
      bf_41termid,	/* COND */
      bf_42cardacc,	/* MAND */
      bf_44respdata, /* COND */
      bf_49trncurcd, /* MAND */
      bf_54addlamts, /* COND */
	  bf_56pad,
      bf_62priv62,   /* COND */
      bf_63priv63,   /* MAND */
      bf_102acct1,   /* COND */
      bf_103acct2,   /* COND */
      bf_121issid,   /* COND */
      bf_done      /* end of fields to move */
      },
      {
      /********************************************
         REFUND
      ********************************************/
      AUTH_PRE_AUTHORIZATION_CANCLE,
      0x01,0x00,              /* message id */
      0x20,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,            /* MAND */
      bf_3proc,           /* MAND */
      bf_4amount,         /* MAND */
      bf_7trntime,  	  /* MAND */
      bf_11stan,   	      /* MAND */
	  bf_12time,          /* MAND */
	  bf_13date,          /* MAND */
      bf_14expiry,        /* COND */
      bf_18sicnumber,     /* MAND */
      bf_19ctrycd,        /* MAND */
      bf_22pose,          /* COND */
      bf_23cardseqnum,    /* COND */
      bf_25pocc,          /* MAND */
      bf_32idcode,        /* MAND */
      bf_35track2,        /* COND */
      bf_37rrn,           /* MAND */
      bf_41termid,        /* COND */
      bf_42cardacc,       /* MAND */
      bf_43cardaccname,   /* COND */
	  bf_44respdata,      /* COND */
      bf_45track1,        /* COND */
      bf_49trncurcd,      /* MAND */
      bf_52pinblock,      /* COND */
      bf_53seccntl,       /* COND */
	  bf_55ICCdata,
	  bf_56pad,
      bf_60priv60,        /* MAND */
      bf_62priv62,        /* COND */
      bf_63priv63,        /* MAND */
      bf_126priv126,      /* COND */
      bf_130termcap,      /* COND */
      bf_131tvr,          /* COND */
      bf_132unpredict,    /* COND */
      bf_133termserial,   /* COND */
      bf_134visadata,     /* COND */
      bf_135issdata,      /* COND */
      bf_136crytogram,    /* COND */
      bf_137apptranctr,   /* COND */
      bf_138app_profile,  /* COND */
      bf_144crytpotype,   /* COND */
      bf_145tcountry,     /* COND */
      bf_146termtxndate,  /* COND */
      bf_147cryptamt,     /* COND */
      bf_148cryptcurcd,   /* COND */
      bf_149cryptcash,    /* COND */
      bf_done      /* end of fields to move */
      },
      {
      /********************************************
         AUTH REFUND  R E S P O N S E
      ********************************************/
      AUTH_PRE_AUTHORIZATION_CANCLE_RESPONSE,
      0x01,0x10,              /* message id */
      0x20,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,       /* MAND */
      bf_3proc,      /* MAND */
      bf_4amount,    /* MAND */
      bf_7trntime,   /* MAND */
      bf_11stan,     /* MAND */
      bf_19ctrycd,   /* MAND */
      bf_20panctrycd,/* COND */
      bf_25pocc,     /* COND */
      bf_32idcode,   /* MAND */
      bf_34panext,   /* COND */
      bf_37rrn,      /* MAND */
      bf_38authcode, /* COND */
      bf_39respcode, /* MAND */
      bf_41termid,   /* COND */
      bf_42cardacc,  /* MAND */
      bf_44respdata, /* COND */
      bf_49trncurcd, /* MAND */
      bf_54addlamts, /* COND */
	  bf_55ICCdata,
	  bf_56pad,
      bf_62priv62,   /* COND */
      bf_63priv63,   /* MAND */
      bf_102acct1,   /* COND */
      bf_103acct2,   /* COND */
      bf_121issid,   /* COND */
      bf_done      /* end of fields to move */
      },
      { 
      /*******************************************************
         C A S H   A D V A N C E  A U T H O R I Z A T I O N
      *******************************************************/
      AUTH_CASH_ADVANCE_AUTHORIZATION,
      0x01,0x00,              /* message id */
      0x01,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,      /* MAND */
      bf_3proc,		/* MAND */
      bf_4amount,	/* MAND */
      bf_7trntime,  /* MAND */
      bf_10convrate, // ThoughtFocus nirbhay : additional requirements
      bf_11stan,	/* MAND */
      bf_14expiry,	/* COND */
      bf_18sicnumber,/* MAND */
      bf_19ctrycd,  /* MAND */
      bf_22pose,	/* COND */
      bf_25pocc,	/* MAND */
	  bf_28feeamount,     /* COND */  /* Girija Y TF, Apr 2009 Mandates */
      bf_32idcode,  /* MAND */
      bf_35track2,	/* COND */
      bf_37rrn,     /* MAND */
      bf_41termid,	/* COND */
      bf_42cardacc,	/* MAND */
      bf_43cardaccname,	/* COND */
	  bf_44respdata,      /* COND */
      bf_45track1,	/* COND */
      bf_49trncurcd,/* MAND */
      bf_51crdcurcd,      //Phani
      bf_52pinblock,/* COND */
      bf_53seccntl, /* COND */
	  bf_55ICCdata,
	  bf_56pad,
      bf_60priv60,	/* MAND */
      bf_63priv63,        /* MAND */
	  bf_104trandesc,	  /* COND */  // TF-Ajay April2009 visa mandates
	  bf_126priv126,
      bf_done      /* end of fields to move */
      },

      {  
      /********************************************
         B A L A N C E   I N Q U I R Y 
      ********************************************/
      AUTH_BALANCE_INQUIRY,
      0x01,0x00,              /* message id */
      0x30,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,      /* MAND */
      bf_3proc,	  /* MAND */
      bf_7trntime,  /* MAND */
      bf_10convrate, // ThoughtFocus nirbhay : additional requirements
      bf_11stan,	  /* MAND */
      bf_14expiry,  /* COND */
      bf_18sicnumber,/* MAND */
      bf_19ctrycd,  /* MAND */
      bf_22pose,	  /* COND */
      bf_23cardseqnum, /* COND */
      bf_25pocc,	  /* MAND */
	  bf_28feeamount,     /* COND */  /* Girija Y TF, Apr 2009 Mandates */
      bf_32idcode,  /* MAND */
      bf_33fwdidcode,/* COND */
      bf_34panext,  /* COND */
      bf_35track2,  /* COND */
      bf_37rrn,     /* MAND */
      bf_41termid,  /* COND */
      bf_42cardacc, /* MAND */
      bf_43cardaccname,	/* COND */
	  bf_44respdata,      /* COND */
      bf_45track1,  /* COND */
      bf_49trncurcd,/* MAND */
      bf_52pinblock,/* COND */
      bf_53seccntl, /* COND */
	  bf_55ICCdata,
	  bf_56pad,
      bf_60priv60,  /* MAND */
      bf_63priv63,        /* MAND */
	  bf_104trandesc,	  /* COND */  // TF-Ajay April2009 visa mandates
	  bf_126priv126,
      bf_130termcap,      /* COND */
      bf_131tvr,          /* COND */
      bf_132unpredict,    /* COND */
      bf_133termserial,   /* COND */
      bf_134visadata,     /* COND */
      bf_135issdata,      /* COND */
      bf_136crytogram,    /* COND */
      bf_137apptranctr,   /* COND */
      bf_138app_profile,  /* COND */
      bf_144crytpotype,   /* COND */
      bf_145tcountry,     /* COND */
      bf_146termtxndate,  /* COND */
      bf_147cryptamt,     /* COND */
      bf_148cryptcurcd,   /* COND */
      bf_149cryptcash,    /* COND */
      bf_done      /* end of fields to move */
      },      

      {  
      /***************************************************
         B A L A N C E   I N Q U I R Y   R E S P O N S E
      ****************************************************/
      AUTH_BALANCE_INQUIRY_RESPONSE,
      0x01,0x10,              /* message id */
      0x30,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,       /* MAND */
      bf_3proc,      /* MAND */
      bf_7trntime,   /* MAND */
      bf_11stan,     /* MAND */
      bf_19ctrycd,   /* MAND */
      bf_25pocc,     /* COND */
      bf_32idcode,   /* MAND */
      bf_34panext,   /* COND */
      bf_37rrn,      /* MAND */
      bf_38authcode, /* COND */
      bf_39respcode, /* MAND */
      bf_41termid,   /* COND */
      bf_42cardacc,  /* MAND */
      bf_44respdata, /* COND */
      bf_49trncurcd, /* MAND */
      bf_54addlamts, /* COND */
	  bf_56pad,
      bf_62priv62,   /* COND */
      bf_63priv63,   /* MAND */
      bf_done      /* end of fields to move */
      },      

      {  
      /********************************************
         V O I D   S A L E
      ********************************************/
      AUTH_VOID_SALE,
      0x04,0x00,              /* message id */
      0x00,0x00,0x00,         /* processing code  (use original) */
      /**** OPTIONS ****/
      topt1_credit+topt1_postadv,
      topt2_batpend,
      /**** BIT FIELDS ****/
      bf_2pan,        /* COND */
      bf_3proc,       /* MAND */
      bf_4amount,     /* MAND */
      bf_7trntime,    /* MAND */
      bf_10convrate, // ThoughtFocus nirbhay : additional requirements
      bf_11stan,      /* MAND */
      bf_14expiry,    /* COND */
      bf_18sicnumber, /* MAND */
      bf_19ctrycd,    /* MAND */
      bf_22pose,      /* COND */
      bf_23cardseqnum,/* COND */
      bf_25pocc,      /* MAND */
      bf_32idcode,    /* MAND */
      bf_33fwdidcode, /* COND */
      bf_37rrn,       /* MAND */
      bf_38authcode,  /* MAND */
      bf_41termid,    /* MAND */
      bf_42cardacc,   /* MAND */
      bf_43cardaccname,/*COND */
      bf_49trncurcd,  /* MAND */
	  bf_56pad,
      bf_60priv60,    /* MAND */
      bf_62priv62,    /* COND */
      bf_63priv63,    /* MAND */
      bf_90orgdata,   /* MAND */
      bf_95replamts,  /* COND */
      bf_131tvr,      /* COND */
      bf_133termserial,/*COND */
      bf_134visadata,  /*COND */
      bf_137apptranctr,/*COND */
      bf_143iss_result,/*COND */
      bf_done         /* end of fields to move */
      },

      {
      /********************************************
         V O I D   R E F U N D
      ********************************************/
      AUTH_VOID_REFUND,
      0x01,0x00,              /* message id */
      0x22,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_reversal,
      /**** BIT FIELDS ****/
      bf_2pan,      /* MAND */
      bf_3proc,		/* MAND */
      bf_4amount,	/* MAND */
      bf_7trntime,  /* MAND */
      bf_10convrate, // ThoughtFocus nirbhay : additional requirements
      bf_11stan,	/* MAND */
      bf_14expiry,	/* COND */
      bf_18sicnumber,/* MAND */
      bf_19ctrycd,  /* MAND */
      bf_22pose,	/* COND */
      bf_25pocc,	/* MAND */
      bf_32idcode,  /* MAND */
      bf_35track2,	/* COND */
      bf_37rrn,     /* MAND */
      bf_41termid,	/* COND */
      bf_42cardacc,	/* MAND */
	  bf_44respdata, /* COND */
      bf_45track1,	/* COND */
      bf_49trncurcd,/* MAND */
	  bf_56pad,
	  bf_60priv60,	/* MAND */
      bf_63priv63,    /* MAND */
	  bf_104trandesc,	  /* COND */  // TF-Ajay April2009 visa mandates
	  bf_126priv126,
      bf_done      /* end of fields to move */
      },

	  /* OCT and Account funding tx*/
	  {
      /********************************************
         OCT A U T H O R I Z A T I O N
      ********************************************/
      AUTH_OCT_TX,
      0x01,0x00,                  /* message id, EBCDIC => 0200 */
      0x26,0x00,0x00,        /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,

	  /**** BIT FIELDS ****/
      bf_2pan,        /* MAND */
      bf_3proc,	    /* MAND */
      bf_4amount,     /* MAND */
      bf_7trntime,  	  /* MAND */
      bf_11stan,   	      /* MAND */
	  bf_12time,          /* MAND */
	  bf_13date,          /* MAND */
      bf_12time,      /* MAND */
      bf_13date,      /* MAND */
      bf_15datestl,   /* MAND */
      bf_22pose,  /* MAND */
      bf_25pocc,     /* COND */
      bf_32idcode,    /* MAND */
      bf_35track2,    /* COND */
	  bf_37rrn,      /* MAND */		/* Girija Y - TF */
      bf_41termid,    /* COND */
      bf_42cardacc,   /* MAND */
      bf_48addl_data,  /* MAND */
      bf_49trncurcd,  /* MAND */
      bf_52pinblock,  /* MAND */
	  bf_56pad,
	  bf_60priv60,
	  bf_62priv62,
	  bf_63priv63,	  /* COND */    /* Girija Y - TF */
      bf_102acct1,    /* COND */
      bf_103acct2,    /* COND */
	  bf_104trandesc, /* MAND */
	  //bf_115addl_trace,/* COND */
      //bf_128msg_authc,/* COND */
      bf_done      /* end of fields to move */
      },

      {
      /***********************************************
        OCT  A U T H O R I Z A T I O N   R E S P O N S E
      ***********************************************/
      AUTH_OCT_TX_RESPONSE,
      0x01,0x10,
      0x26,0x00,0x00,  /*DE3 */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,       /* MAND */
      bf_3proc,      /* MAND */
      bf_4amount,    /* MAND */
      bf_7trntime,   /* MAND */
      bf_11stan,     /* MAND */
      bf_15datestl,  /* MAND */
      bf_19ctrycd,     /* MAND */
      bf_25pocc,     /* COND */
      bf_32idcode,   /* MAND */
	  bf_37rrn,      /* MAND */  /* Girija Y - TF */
      bf_38authcode, /* COND */
      bf_39respcode, /* MAND */
      bf_41termid,   /* COND */
      bf_42cardacc,   /* MAND */
      bf_48addl_data, /* MAND */
      bf_49trncurcd,      /* MAND */
	  bf_56pad,
	  bf_62priv62,
	  bf_63priv63,	  /* COND */    /* Girija Y - TF */
      //bf_102acct1,   /* MAND */
      //bf_103acct2,   /* MAND */
  	  //bf_104trandesc, /* MAND */
	  //bf_115addl_trace ,/* COND */

      bf_done      /* end of fields to move */
      },

	  {
	        /********************************************
	           FAST FUND A U T H O R I Z A T I O N
	        ********************************************/
	        AUTH_FAST_FUND,
	        0x01,0x00,
	        0x10,0x00,0x00,  /* DE3 */
	        /**** OPTIONS ****/
	        topt1_postadv,
	        topt2_referral+topt2_reversal+topt2_floor,
	        /**** BIT FIELDS ****/
	        bf_2pan,        /* MAND */
	        bf_3proc,	    /* MAND */
	        bf_4amount,     /* MAND */
	        bf_7trntime,  	  /* MAND */
	        bf_11stan,   	      /* MAND */
			bf_12time,          /* MAND */
			bf_13date,          /* MAND */
	        bf_12time,      /* MAND */
	        bf_13date,      /* MAND */
	        bf_15datestl,   /* MAND */
	        bf_22pose,  /* MAND */
	        bf_25pocc,     /* MAND */
	        bf_32idcode,    /* MAND */
	        bf_35track2,    /* COND */
			bf_37rrn,      /* MAND */		/* Girija Y - TF */
	        bf_41termid,    /* COND */
	        bf_42cardacc,   /* MAND */
	        bf_48addl_data,  /* MAND */
	        bf_49trncurcd,  /* MAND */
	        bf_52pinblock,  /* MAND */
			bf_56pad,
		 	bf_60priv60,	/* MAND */
			bf_62priv62,  	/* COND */
			bf_63priv63,	  /* COND */    /* Girija Y - TF */
	        bf_102acct1,    /* COND */
	        bf_103acct2,    /* COND */
	        bf_104trandesc, /* MAND */
	  	  //bf_115addl_trace ,/* COND *
			bf_126priv126,
	        bf_done      /* end of fields to move */
	        },

      {
	        /***********************************************
	         FAST FUND  A U T H O R I Z A T I O N   R E S P O N S E
	        ***********************************************/
	       AUTH_FAST_FUND_RESPONSE,
	        0x01,0x10,
	        0x10,0x00,0x00,  /* DE3 */
	        /**** OPTIONS ****/
	        topt1_postadv,
	        topt2_referral+topt2_reversal+topt2_floor,
	        /**** BIT FIELDS ****/
	        bf_2pan,       /* MAND */
	        bf_3proc,      /* MAND */
	        bf_4amount,    /* MAND */
	        bf_7trntime,   /* MAND */
	        bf_11stan,     /* MAND */
	        bf_15datestl,  /* MAND */
	        bf_19ctrycd,     /* MAND */
	        bf_25pocc,     /* COND */
	        bf_32idcode,   /* MAND */
	  	  bf_37rrn,      /* MAND */  /* Girija Y - TF */
	        bf_38authcode, /* COND */
	        bf_39respcode, /* MAND */
	        bf_41termid,   /* COND */
	        bf_42cardacc,   /* MAND */
	        bf_48addl_data, /* MAND */
	        bf_49trncurcd,      /* MAND */
	  	  bf_56pad,
	  	  bf_63priv63,	  /* COND */    /* Girija Y - TF */
	       // bf_102acct1,   /* MAND */
	       // bf_103acct2,   /* MAND */
	  	 //bf_104trandesc, /* MAND */
	  	  //bf_115addl_trace ,/* COND *
	        bf_done      /* end of fields to move */
	        },
      {
      /********************************************
         L O G O N 
      ********************************************/
      AUTH_LOGON,
      0x08,0x00,              /* message id */
      0x92,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_noacct+topt1_nocapture,
      topt2_noprerev,
      /**** BIT FIELDS ****/
      bf_7trntime,  /* MAND */
      bf_11stan,	/* MAND */
      bf_37rrn,     /* MAND */
      bf_48addl_data, /* COND */
      bf_70nmicode, /* MAND */
      bf_done       /* end of fields to move */
      },

      {
      /********************************************
         L O G O N   R E S P O N S E
      ********************************************/
      AUTH_LOGON_RESPONSE,
      0x08,0x10,              /* message id */
      0x92,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_noacct+topt1_nocapture,
      topt2_noprerev,
      /**** BIT FIELDS ****/
      bf_7trntime,  /* MAND */
	   bf_11stan,	/* MAND */
      bf_37rrn,     /* MAND */
      bf_39respcode, /* MAND */
      bf_70nmicode, /* MAND */
      bf_done       /* end of fields to move */
      },

      {
      /********************************************
         T E S T   T R A N
      ********************************************/
      AUTH_TEST,
      0x08,0x00,              /* message id */
      0x99,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_noacct+topt1_nocapture,
      topt2_noprerev,
      /**** BIT FIELDS ****/
      bf_3proc,
      bf_24nii,
      bf_41termid,
      bf_done      /* end of fields to move */
      },

      { 
      /********************************************
         C A R D   V E R I F I C A T I O N
      ********************************************/
      AUTH_CARD_VERIFICATION,
      0x01,0x00,              /* message id */
      0x00,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_new_proccode,
      /**** BIT FIELDS ****/
      bf_2pan,      /* MAND */
      bf_3proc,		/* MAND */
      bf_4amount,	/* MAND */
      bf_7trntime,  /* MAND */
      bf_10convrate, // ThoughtFocus nirbhay : additional requirements
      bf_11stan,	/* MAND */
      bf_14expiry,	/* COND */
      bf_18sicnumber,/* MAND */
      bf_19ctrycd,  /* MAND */
      bf_22pose,	/* COND */
      bf_23cardseqnum, /* COND */
      bf_25pocc,	/* MAND */
      bf_32idcode,  /* MAND */
      bf_35track2,	/* COND */
      bf_37rrn,     /* MAND */
      bf_41termid,	/* COND */
      bf_42cardacc,	/* MAND */
	  bf_44respdata,      /* COND */
      bf_45track1,	/* COND */
      bf_49trncurcd,/* MAND */
	  bf_51crdcurcd,      //Phani
	  bf_56pad,
      bf_60priv60,	/* MAND */
      bf_63priv63,    /* MAND */
	  bf_104trandesc,	  /* COND */  // TF-Ajay April2009 visa mandates
	  bf_126priv126,
      bf_130termcap,      /* COND */
      bf_131tvr,          /* COND */
      bf_132unpredict,    /* COND */
      bf_133termserial,   /* COND */
      bf_134visadata,     /* COND */
      bf_135issdata,      /* COND */
      bf_136crytogram,    /* COND */
      bf_137apptranctr,   /* COND */
      bf_138app_profile,  /* COND */
      bf_144crytpotype,   /* COND */
      bf_145tcountry,     /* COND */
      bf_146termtxndate,  /* COND */
      bf_147cryptamt,     /* COND */
      bf_148cryptcurcd,   /* COND */
      bf_149cryptcash,    /* COND */
      bf_done      /* end of fields to move */
      },
     
      {
      /********************************************
         Q U A S I   C A S H
      ********************************************/
      AUTH_QUASI_CASH,
      0x01,0x00,              /* message id */
      0x11,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,      /* MAND */
      bf_3proc,		/* MAND */
      bf_4amount,	/* MAND */
	  bf_6crdbillamt,				// TF PHANI ..Production issue, added field
      bf_7trntime,  /* MAND */
      bf_10convrate, // ThoughtFocus nirbhay : additional requirements
      bf_11stan,	/* MAND */
      bf_14expiry,	/* COND */
      bf_18sicnumber,/* MAND */
      bf_19ctrycd,  /* MAND */
      bf_22pose,	/* COND */
      bf_23cardseqnum, /* COND */
      bf_25pocc,	/* MAND */
	  bf_28feeamount,     /* COND */  /* Girija Y TF, Apr 2009 Mandates */
      bf_32idcode,  /* MAND */
      bf_35track2,	/* COND */
      bf_37rrn,     /* MAND */
      bf_41termid,	/* COND */
      bf_42cardacc,	/* MAND */
      bf_43cardaccname,	/* COND */
	  bf_44respdata,      /* COND */
      bf_45track1,	/* COND */
      bf_49trncurcd,/* MAND */
	  bf_51crdcurcd,			 // TF Phani,,, Production issue,...added field
      bf_52pinblock,/* COND */
      bf_53seccntl, /* COND */
	  bf_55ICCdata,
	  bf_56pad,
      bf_60priv60,	/* MAND */
      bf_63priv63,    /* MAND */
	  bf_104trandesc,	  /* COND */  // TF-Ajay April2009 visa mandates
	  bf_126priv126,
      bf_130termcap,      /* COND */
      bf_131tvr,          /* COND */
      bf_132unpredict,    /* COND */
      bf_133termserial,   /* COND */
      bf_134visadata,     /* COND */
      bf_135issdata,      /* COND */
      bf_136crytogram,    /* COND */
      bf_137apptranctr,   /* COND */
      bf_138app_profile,  /* COND */
      bf_144crytpotype,   /* COND */
      bf_145tcountry,     /* COND */
      bf_146termtxndate,  /* COND */
      bf_147cryptamt,     /* COND */
      bf_148cryptcurcd,   /* COND */
      bf_149cryptcash,    /* COND */
      bf_done      /* end of fields to move */
      },

      {
      /**********************************************
         Q U A S I   C A S H   R E S P O N S E
      ***********************************************/
      AUTH_QUASI_CASH_RESPONSE,
      0x01,0x10,              /* message id */
      0x00,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,       /* MAND */
      bf_3proc,      /* MAND */
      bf_4amount,	   /* MAND */
      bf_7trntime,   /* MAND */
      bf_11stan,     /* MAND */
      bf_19ctrycd,   /* MAND */
      bf_20panctrycd,/* COND */
      bf_25pocc,     /* COND */
      bf_32idcode,   /* MAND */
      bf_34panext,   /* COND */
      bf_37rrn,      /* MAND */
      bf_38authcode, /* COND */
      bf_39respcode, /* MAND */
      bf_41termid,	/* COND */
      bf_42cardacc,	/* MAND */
      bf_44respdata, /* COND */
      bf_49trncurcd, /* MAND */
	  bf_56pad,
      bf_62priv62,   /* COND */
      bf_63priv63,   /* MAND */
      bf_102acct1,   /* COND */
      bf_103acct2,   /* COND */
      bf_121issid,   /* COND */
      bf_done      /* end of fields to move */
      },

      { 
      /********************************************
          A T M   C O N F I R M
       ********************************************/
      AUTH_ATM_CONFIRM,
      0x01,0x02,
      0x00,0x00,0x00,  /* This is not used.  It is not correct. */
      //**** OPTIONS ****
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,         /* MAND */
      bf_3proc,	       /* MAND */
      bf_4amount,      /* MAND */
      bf_7trntime,   /* MAND */
      bf_10convrate, // ThoughtFocus nirbhay : additional requirements
      bf_11stan,	   /* MAND */
	  bf_14expiry,	/* COND */
	  bf_18sicnumber,/* MAND */
      bf_19ctrycd,   /* MAND */
      bf_20panctrycd,/* COND */
	  bf_22pose,	/* COND */
      bf_25pocc,	/* MAND */
      bf_32idcode,   /* MAND */
      bf_34panext,   /* COND */
      bf_37rrn,      /* MAND */
      bf_38authcode, /* COND */
      bf_39respcode, /* MAND */
      bf_41termid,	/* COND */
      bf_42cardacc,	/* MAND */
      bf_43cardaccname,/* MAND */
      bf_49trncurcd, /* MAND */
	  bf_56pad,
      bf_61priv61,	   /* MAND */
      bf_63priv63,    /* MAND */
	  bf_90orgdata,   /* MAND */  // TF Ajay 
      bf_95replamts,  /* COND */
      bf_done        /* end of fields to move */
      },
      {
	  /********************************************
        A U T H  A D V I C E  R E Q U E S T
      ********************************************/
	  AUTH_ADVICE,
      0x01,0x20,              /* message id */
      0x00,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,            /* MAND */
      bf_3proc,           /* MAND */
      bf_4amount,         /* MAND */
      bf_6crdbillamt,     // Phani
      bf_7trntime,        /* MAND */
      bf_10convrate,     // nirbhay : additional requirements
      bf_11stan,          /* MAND */
      bf_14expiry,        /* COND */
      bf_18sicnumber,     /* MAND */
      bf_19ctrycd,        /* MAND */
      bf_22pose,          /* COND */
      bf_23cardseqnum,    /* COND */
      bf_25pocc,          /* MAND */
	  bf_28feeamount,      /* MAND */ /* Girija Y TF , Apr 2009 Mandates */
      bf_32idcode,        /* MAND */
      bf_33fwdidcode,     /* COND */ // only in balance inquiry
     /* bf_34panext,*/		  /* COND */ // only in balance inquiry
      bf_35track2,        /* COND */
      bf_37rrn,           /* MAND */
      bf_38authcode,      /* MAND */  
	  bf_39respcode, 	  /* MAND */
      bf_41termid,        /* COND */
      bf_42cardacc,       /* MAND */
      bf_43cardaccname,   /* COND */
      bf_44respdata,      /* COND */
      bf_45track1,        /* COND */
      bf_49trncurcd,      /* MAND */
      bf_51crdcurcd,      //Phani
      bf_52pinblock,      /* COND */
      bf_53seccntl,       /* COND */
	  bf_54addlamts,
	  bf_55ICCdata,
	  bf_56pad,
      bf_59posdata,       /* COND */
      bf_60priv60,        /* MAND*/
      bf_62priv62,        /* COND */
      bf_63priv63,        /* COND */
	  bf_126priv126,
	  bf_done      /* end of fields to move */
      },
	  {
	  /********************************************
         A U T H  A D V I C E  R E S P O N S E
      ********************************************/
      AUTH_ADVICE_RESPONSE,
      0x01,0x30,              /* message id */
      0x00,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
	  bf_2pan,       /* MAND */
      bf_3proc,      /* MAND */
      bf_4amount,    /* MAND */
      bf_7trntime,   /* MAND */
      bf_11stan,     /* MAND */
      bf_19ctrycd,   /* MAND */
      bf_20panctrycd,/* COND */
      bf_25pocc,     /* COND */
      bf_32idcode,   /* MAND */
      bf_34panext,   /* COND */
      bf_37rrn,      /* MAND */
      bf_38authcode, /* COND */
      bf_39respcode, /* MAND */
      bf_41termid,   /* COND */
      bf_42cardacc,  /* MAND */
      bf_44respdata, /* COND */
      bf_48addl_data, /* OPT  */
      bf_49trncurcd, /* MAND */
      bf_54addlamts, /* COND */
	  bf_56pad,
      bf_done      /* end of fields to move */
      },
	  {
	  /*************************************************
        A U T H  A D V I C E  R E P E A T  R E Q U E S T
      **************************************************/
	  AUTH_ADVICE_REPEAT,
      0x01,0x21,              /* message id */
      0x00,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,            /* MAND */
      bf_3proc,           /* MAND */
      bf_4amount,         /* MAND */
      bf_6crdbillamt,     // Phani
      bf_7trntime,        /* MAND */
      bf_10convrate,     // nirbhay : additional requirements
      bf_11stan,          /* MAND */
      bf_14expiry,        /* COND */
      bf_18sicnumber,     /* MAND */
      bf_19ctrycd,        /* MAND */
      bf_22pose,          /* COND */
      bf_23cardseqnum,    /* COND */
      bf_25pocc,          /* MAND */
	  bf_28feeamount,     /* COND */  /* Girija Y TF, Apr 2009 Mandates */
      bf_32idcode,        /* MAND */
      bf_35track2,        /* COND */
      bf_37rrn,           /* MAND */
      bf_38authcode,      /* MAND */  
      bf_41termid,        /* COND */
      bf_42cardacc,       /* MAND */
      bf_43cardaccname,   /* COND */
      bf_44respdata,      /* COND */
      bf_45track1,        /* COND */
      bf_49trncurcd,      /* MAND */
      bf_51crdcurcd,      //Phani
      bf_52pinblock,      /* COND */
      bf_53seccntl,       /* COND */
	  bf_55ICCdata,
	  bf_56pad,
      bf_59posdata,       /* COND */
	  bf_126priv126,
	  bf_done      /* end of fields to move */
      },
	  {
	  /*****************************************************
         A U T H  A D V I C E   R E P E A T R E S P O N S E
      ******************************************************/
      AUTH_ADVICE_REPEAT_RESPONSE,
      0x01,0x31,              /* message id */
      0x00,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
	  bf_2pan,       /* MAND */
      bf_3proc,      /* MAND */
      bf_4amount,    /* MAND */
      bf_7trntime,   /* MAND */
      bf_11stan,     /* MAND */
      bf_19ctrycd,   /* MAND */
      bf_20panctrycd,/* COND */
      bf_25pocc,     /* COND */
      bf_32idcode,   /* MAND */
      bf_34panext,   /* COND */
      bf_37rrn,      /* MAND */
      bf_38authcode, /* COND */
      bf_39respcode, /* MAND */
      bf_41termid,   /* COND */
      bf_42cardacc,  /* MAND */
      bf_44respdata, /* COND */
      bf_48addl_data, /* OPT  */
      bf_49trncurcd, /* MAND */
      bf_54addlamts, /* COND */
	  bf_56pad,
      bf_done      /* end of fields to move */
      },
	  /* Change Request: March 31 2009, Girija Y TF */
	  {
	  /********************************************
        A U T H  A D V I C E  R E V E R S A  L  R E Q U E S T
      ********************************************/
	  AUTH_REVERSAL_ADVICE,
      0x04,0x20,              /* message id */
      0x00,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,            /* MAND */
      bf_3proc,           /* MAND */
      bf_4amount,         /* MAND */
      bf_6crdbillamt,     /* COND */
      bf_7trntime,        /* MAND */
      bf_10convrate,      /* COND */
      bf_11stan,          /* MAND */
      bf_14expiry,        /* COND */
      bf_18sicnumber,     /* MAND */
      bf_19ctrycd,        /* MAND */
      bf_22pose,          /* COND */
      bf_23cardseqnum,    /* COND */
      bf_25pocc,          /* MAND */
	  bf_28feeamount,     /* COND */  /* Girija Y TF, Apr 2009 Mandates */
      bf_32idcode,        /* MAND */
      bf_33fwdidcode,     /* COND */ 
      bf_34panext,        /* COND */  
      bf_35track2,        /* COND */
      bf_37rrn,           /* MAND */
      bf_38authcode,      /* MAND */  
      bf_41termid,        /* COND */
      bf_42cardacc,       /* MAND */
      bf_43cardaccname,   /* COND */
      bf_44respdata,      /* COND */
      bf_45track1,        /* COND */
      bf_49trncurcd,      /* MAND */
      bf_51crdcurcd,      //Phani
      bf_52pinblock,      /* COND */
      bf_53seccntl,       /* COND */
	  bf_55ICCdata,
	  bf_56pad,
      bf_59posdata,       /* COND */
      bf_60priv60,		/* MAND */ // Added below fields
      bf_62priv62,		 /* COND */ 
      bf_63priv63,		 /* MAND */
      bf_90orgdata,		/* MAND */
	  bf_126priv126,
      bf_done      /* end of fields to move */
      },
	  {
	        /********************************************

	           R E V E R S A L   ADVICE  R E S P O N S E

	        ********************************************/
	        AUTH_REVERSAL_ADVICE_RESPONSE,
	        0x04,0x30,              /* message id */
	        0x00,0x00,0x00,         /* processing code */
	        /**** OPTIONS ****/
	        topt1_credit+topt1_postadv,
	        topt2_batpend,
	        /**** BIT FIELDS ****/
	        bf_2pan,        /* MAND */
	        bf_3proc,	    /* MAND */
	        bf_4amount,	    /* MAND */
	        bf_7trntime,    /* MAND */
	        bf_11stan,	    /* MAND */
	        bf_19ctrycd,    /* MAND */
	        bf_20panctrycd, /* COND */
	        bf_25pocc,	    /* MAND */
	        bf_32idcode,    /* MAND */
	  	  bf_33fwdidcode,  /* COND */
	        bf_34panext,    /* COND */
	        bf_37rrn,       /* MAND */
	        bf_39respcode,  /* MAND */
	        bf_41termid,    /* COND */
	        bf_42cardacc,   /* MAND */
	        bf_48addl_data, /* OPT  */
	        bf_49trncurcd,  /* MAND */
	  	  bf_55ICCdata,
	  	  bf_56pad,
	        bf_62priv62,    /* OPT  */
	        bf_63priv63,    /* MAND */
	        bf_95replamts,  /* COND */
	        bf_102acct1,    /* COND */
	        bf_103acct2,    /* COND */
	        bf_104trandesc, /* COND */
	        bf_118intractry,/* OPT  */
	        bf_121issid,    /* COND */
	        bf_done      /* end of fields to move */

	        },
	  /* Change Request: Dec 14 2009,  TF-Ajay */
	  {
	  /********************************************
        A U T H  A D V I C E  R E V E R S A  L  R E P E A T  R E Q U E S T
      ********************************************/
	  AUTH_REVERSAL_ADVICE_REPEAT,
      0x04,0x21,              /* message id */
      0x00,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,            /* MAND */
      bf_3proc,           /* MAND */
      bf_4amount,         /* MAND */
      bf_6crdbillamt,     /* COND */
      bf_7trntime,        /* MAND */
      bf_10convrate,      /* COND */
      bf_11stan,          /* MAND */
      bf_14expiry,        /* COND */
      bf_18sicnumber,     /* MAND */
      bf_19ctrycd,        /* MAND */
      bf_22pose,          /* COND */
      bf_23cardseqnum,    /* COND */
      bf_25pocc,          /* MAND */
	  bf_28feeamount,     /* COND */  
      bf_32idcode,        /* MAND */
      bf_33fwdidcode,     /* COND */ 
      bf_34panext,        /* COND */  
      bf_35track2,        /* COND */
      bf_37rrn,           /* MAND */
      bf_38authcode,      /* MAND */  
      bf_41termid,        /* COND */
      bf_42cardacc,       /* MAND */
      bf_43cardaccname,   /* COND */
      bf_44respdata,      /* COND */
      bf_45track1,        /* COND */
      bf_49trncurcd,      /* MAND */
      bf_51crdcurcd,      //Phani
      bf_52pinblock,      /* COND */
      bf_53seccntl,       /* COND */
	  bf_55ICCdata,
	  bf_56pad,
      bf_59posdata,       /* COND */
	  bf_60priv60,		/* MAND */ // Added below fields
      bf_62priv62,		 /* COND */ 
      bf_63priv63,		 /* MAND */
      bf_90orgdata,		/* MAND */
	  bf_126priv126,
      bf_done      /* end of fields to move */
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

const struct isobit BITTAB[192+1] =
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
   { 7, 0x08 },   /* bit 61 */   { 7, 0x04 },   /* bit 62 */   { 7, 0x02 },   /* bit 63 */   { 7, 0x01 },   /* bit 64 */

   { 8, 0x80 },    /* bit 1  */   { 8, 0x40 },    /* bit 2  */   { 8, 0x20 },    /* bit 3  */   { 8, 0x10 },    /* bit 4  */
   { 8, 0x08 },    /* bit 5  */   { 8, 0x04 },    /* bit 6  */   { 8, 0x02 },    /* bit 7  */   { 8, 0x01 },    /* bit 8  */
   { 9, 0x80 },    /* bit 9  */   { 9, 0x40 },    /* bit 10 */   { 9, 0x20 },    /* bit 11 */   { 9, 0x10 },    /* bit 12 */
   { 9, 0x08 },    /* bit 13 */   { 9, 0x04 },    /* bit 14 */   { 9, 0x02 },    /* bit 15 */   { 9, 0x01 },    /* bit 16 */
   { 10, 0x80 },   /* bit 17 */   { 10, 0x40 },   /* bit 18 */   { 10, 0x20 },   /* bit 19 */   { 10, 0x10 },   /* bit 20 */
   { 10, 0x08 },   /* bit 21 */   { 10, 0x04 },   /* bit 22 */   { 10, 0x02 },   /* bit 23 */   { 10, 0x01 },   /* bit 24 */
   { 11, 0x80 },   /* bit 25 */   { 11, 0x40 },   /* bit 26 */   { 11, 0x20 },   /* bit 27 */   { 11, 0x10 },   /* bit 28 */
   { 11, 0x08 },   /* bit 29 */   { 11, 0x04 },   /* bit 30 */   { 11, 0x02 },   /* bit 31 */   { 11, 0x01 },   /* bit 32 */
   { 12, 0x80 },   /* bit 33 */   { 12, 0x40 },   /* bit 34 */   { 12, 0x20 },   /* bit 35 */   { 12, 0x10 },   /* bit 36 */
   { 12, 0x08 },   /* bit 37 */   { 12, 0x04 },   /* bit 38 */   { 12, 0x02 },   /* bit 39 */   { 12, 0x01 },   /* bit 40 */
   { 13, 0x80 },   /* bit 41 */   { 13, 0x40 },   /* bit 42 */   { 13, 0x20 },   /* bit 43 */   { 13, 0x10 },   /* bit 44 */
   { 13, 0x08 },   /* bit 45 */   { 13, 0x04 },   /* bit 46 */   { 13, 0x02 },   /* bit 47 */   { 13, 0x01 },   /* bit 48 */
   { 14, 0x80 },   /* bit 49 */   { 14, 0x40 },   /* bit 50 */   { 14, 0x20 },   /* bit 51 */   { 14, 0x10 },   /* bit 52 */
   { 14, 0x08 },   /* bit 53 */   { 14, 0x04 },   /* bit 54 */   { 14, 0x02 },   /* bit 55 */   { 14, 0x01 },   /* bit 56 */
   { 15, 0x80 },   /* bit 57 */   { 15, 0x40 },   /* bit 58 */   { 15, 0x20 },   /* bit 59 */   { 15, 0x10 },   /* bit 60 */
   { 15, 0x08 },   /* bit 61 */   { 15, 0x04 },   /* bit 62 */   { 15, 0x02 },   /* bit 63 */   { 15, 0x01 },   /* bit 64 */

   { 16, 0x80 },   /* bit 1  */   { 16, 0x40 },   /* bit 2  */   { 16, 0x20 },   /* bit 3  */   { 16, 0x10 },   /* bit 4  */
   { 16, 0x08 },   /* bit 5  */   { 16, 0x04 },   /* bit 6  */   { 16, 0x02 },   /* bit 7  */   { 16, 0x01 },   /* bit 8  */
   { 17, 0x80 },   /* bit 9  */   { 17, 0x40 },   /* bit 10 */   { 17, 0x20 },   /* bit 11 */   { 17, 0x10 },   /* bit 12 */
   { 17, 0x08 },   /* bit 13 */   { 17, 0x04 },   /* bit 14 */   { 17, 0x02 },   /* bit 15 */   { 17, 0x01 },   /* bit 16 */
   { 18, 0x80 },   /* bit 17 */   { 18, 0x40 },   /* bit 18 */   { 18, 0x20 },   /* bit 19 */   { 18, 0x10 },   /* bit 20 */
   { 18, 0x08 },   /* bit 21 */   { 18, 0x04 },   /* bit 22 */   { 18, 0x02 },   /* bit 23 */   { 18, 0x01 },   /* bit 24 */
   { 19, 0x80 },   /* bit 25 */   { 19, 0x40 },   /* bit 26 */   { 19, 0x20 },   /* bit 27 */   { 19, 0x10 },   /* bit 28 */
   { 19, 0x08 },   /* bit 29 */   { 19, 0x04 },   /* bit 30 */   { 19, 0x02 },   /* bit 31 */   { 19, 0x01 },   /* bit 32 */
   { 20, 0x80 },   /* bit 33 */   { 20, 0x40 },   /* bit 34 */   { 20, 0x20 },   /* bit 35 */   { 20, 0x10 },   /* bit 36 */
   { 20, 0x08 },   /* bit 37 */   { 20, 0x04 },   /* bit 38 */   { 20, 0x02 },   /* bit 39 */   { 20, 0x01 },   /* bit 40 */
   { 21, 0x80 },   /* bit 41 */   { 21, 0x40 },   /* bit 42 */   { 21, 0x20 },   /* bit 43 */   { 21, 0x10 },   /* bit 44 */
   { 21, 0x08 },   /* bit 45 */   { 21, 0x04 },   /* bit 46 */   { 21, 0x02 },   /* bit 47 */   { 21, 0x01 },   /* bit 48 */
   { 22, 0x80 },   /* bit 49 */   { 22, 0x40 },   /* bit 50 */   { 22, 0x20 },   /* bit 51 */   { 22, 0x10 },   /* bit 52 */
   { 22, 0x08 },   /* bit 53 */   { 22, 0x04 },   /* bit 54 */   { 22, 0x02 },   /* bit 55 */   { 22, 0x01 },   /* bit 56 */
   { 23, 0x80 },   /* bit 57 */   { 23, 0x40 },   /* bit 58 */   { 23, 0x20 },   /* bit 59 */   { 23, 0x10 },   /* bit 60 */
   { 23, 0x08 },   /* bit 61 */   { 23, 0x04 },   /* bit 62 */   { 23, 0x02 },   /* bit 63 */   { 23, 0x01 }    /* bit 64 */
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
INT Move_BCD_Amount_IfThere(enum bitfields, BYTE); // TF PHANI... For Filed 6..space problem
INT Move23_IfThere(enum bitfields, BYTE);
INT MoveEMV_IfThere(enum bitfields, BYTE);
INT vMove_IfThere(enum bitfields, BYTE);
INT Move_Never(enum bitfields, BYTE);
void EBCDICMove(enum bitfields);
void vEBCDICMove(enum bitfields);
void RspEBCDICvMove(enum bitfields);
void RspEBCDICMove(enum bitfields);
void RspIgnore(enum bitfields);
void vRspIgnore(enum bitfields);
void Rj_with_lead_zeros( pCHAR str, INT FieldLen );
BYTE ascii_ebcdic_conv(BYTE conv_char, pCHAR conv_type );

void Build_Host_Header (pAUTH_TX);
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
void Field62Response( pAUTH_TX p_auth_tx );
#else
	static INT CondProc(enum bitfields, BYTE);
	static INT CondExpd(enum bitfields, BYTE);
	static INT CondPan(enum bitfields, BYTE);
	static INT CondTrack1(enum bitfields, BYTE);
	static INT Move_Always(enum bitfields, BYTE);
	static INT Move_IfThere(enum bitfields, BYTE);
	static INT Move_BCD_Amount_IfThere(enum bitfields, BYTE); // TF PHANI... For Filed 6..space problem
	static INT Move23_IfThere(enum bitfields, BYTE);
	static INT MoveEMV_IfThere(enum bitfields, BYTE);
	static INT vMove_IfThere(enum bitfields, BYTE);
	static INT Move_Never(enum bitfields, BYTE);
	static void EBCDICMove(enum bitfields);
	static void vEBCDICMove(enum bitfields);
	static void RspEBCDICvMove(enum bitfields);
	static void RspEBCDICMove(enum bitfields);
	static void RspIgnore(enum bitfields);
	static void vRspIgnore(enum bitfields);
	void Rj_with_lead_zeros( pCHAR str, INT FieldLen );
	BYTE ascii_ebcdic_conv(BYTE conv_char, pCHAR conv_type );
	static  INT Move41_IfThere(enum bitfields fieldid, BYTE p_trantype);

	void Build_Host_Header (pAUTH_TX);
	static void asc_to_vbcd (pCHAR, INT, BYTE [] );
	static void asc_to_bcd (pCHAR, INT, BYTE [] );
	static void str_asc_to_bcd(pCHAR, INT, BYTE [] ); 
	static void vBCDMove(enum bitfields fieldid);
	static void RspMovevBCD (enum bitfields fieldid);
	static void RspMovevBCD_Field34 (enum bitfields fieldid);
	static void GenericMove(enum bitfields fieldid);
	static void RspMoveGeneric(enum bitfields fieldid);
	static void vDataMove(enum bitfields fieldid);
	static void RspMovevData (enum bitfields fieldid);
	void find_cvc_flag( pCHAR, pCHAR, pCHAR);
	static void Field62Response( pAUTH_TX p_auth_tx );

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
   3,                            /* # of bytes in visa iso field */
   Move_Always,                  /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_4amount,                   /* field */
   HOST_MESSAGE.amount_bin,      /* data to move */
   6,                            /* # of bytes in visa iso field */
   Move_Always,                  /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_6crdbillamt,               /* field */
   HOST_MESSAGE.card_bill_amt,   /* data to move */
   6,                            /* # of bytes in visa iso field */
   Move_BCD_Amount_IfThere,      /* conditional */ // TF PHANI ..changed from Move_IfThere
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_7trntime,                  /* field */
   HOST_MESSAGE.gmt_time,        /* data to move */
   5,                            /* # of bytes in visa iso field */
   Move_Always,                  /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_10convrate,                /* field */
   HOST_MESSAGE.conv_rate,       /* data to move */
   4,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_11stan,                    /* field */
   HOST_MESSAGE.stan_bin,        /* data to move */
   3,                            /* # of bytes in visa iso field */
   Move_Always,                  /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_12time,                    /* field */
   HOST_MESSAGE.time_bin,        /* data to move */
   3,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_13date,                    /* field */
   HOST_MESSAGE.date_bin,         /* data to move */
   2,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_14expiry,                  /* field */
   HOST_MESSAGE.date_expd,       /* data to move */
   2,                            /* # of bytes in visa iso field */
   CondExpd,                     /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_15datestl,                 /* field */
   HOST_MESSAGE.date_settle,     /* data to move */
   2,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_18sicnumber,               /* field */
   HOST_MESSAGE.sic_number,      /* data to move */
   2,                            /* # of bytes in visa iso field */
   Move_Always,                  /* mandatory    */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_19ctrycd,                  /* field */
   HOST_MESSAGE.ctry_code,       /* data to move */
   2,                            /* # of bytes in visa iso field */
   Move_Always,                  /* mandatory    */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_20panctrycd,                    /* field */
   HOST_MESSAGE.pan_ctry_cd,     /* data to move */
   2,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_22pose,                    /* field */
   HOST_MESSAGE.posentry,         /* data to move */
   2,                            /* # of bytes in visa iso field */
   Move_Always,                  /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_23cardseqnum,              /* field */
   HOST_MESSAGE.cardseqnum,      /* data to move */
   2,                            /* # of bytes in visa iso field */
   Move23_IfThere,               /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_25pocc,                    /* field */
   HOST_MESSAGE.pos_condcd,      /* data to move */
   1,                            /* # of bytes in visa iso field */
   Move_Always,                  /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_26pincc,                 /* field */
   HOST_MESSAGE.pin_captcd,      /* data to move */
   1,                            /* # of bytes in visa iso field */
   Move_Always,                  /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
/* Girija Y TF, Apr 2009 */
   {
   bf_28feeamount,				/* field */
   HOST_MESSAGE.fee_amount,      /* data to move */
   9,                            /* # of bytes in visa iso field */
   Move_IfThere,                  /* conditional */
   EBCDICMove,                  /* move routine */
   RspEBCDICMove                /* response routine */
   },         
   {
   bf_32idcode,                  /* field */
   HOST_MESSAGE.acq_inst_len,    /* data to move */
   1,                            /* # of bytes in visa iso field */
   Move_Always,                  /* conditional */
   vBCDMove,                     /* move routine */
   RspMovevBCD                   /* response routine */
   },
   {
   bf_33fwdidcode,                    /* field */
   HOST_MESSAGE.fwd_inst_len,    /* data to move */
   1,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   vBCDMove,                     /* move routine */
   RspMovevBCD                   /* response routine */
   },
   {
   bf_34panext,                    /* field */
   HOST_MESSAGE.pan_ext_len,    /* data to move */
   2,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   vBCDMove,                     /* move routine */
   RspMovevBCD_Field34           /* response routine */
   },
   {
   bf_35track2,                  /* field */
   HOST_MESSAGE.track2_len,      /* data to move */
   1,                            /* # of bytes in track2 len field */
   vMove_IfThere,                 /* conditional */
   vBCDMove,                     /* move routine */
   RspMovevBCD                   /* response routine */
   },
   {
   bf_37rrn,                     /* field */
   HOST_MESSAGE.rrn,             /* data to move */
   12,                           /* # of bytes in visa iso field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_38authcode,                /* field */
   HOST_MESSAGE.authcode,        /* data to move */
   6,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_39respcode,                /* field */
   HOST_MESSAGE.responsecd,      /* data to move */
   2,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_40src,                     /* field */
   HOST_MESSAGE.src,             /* data to move */
   3,                            /* # of bytes in visa iso field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_41termid,                  /* field */
   HOST_MESSAGE.termid,          /* data to move */
   8,                            /* # of bytes in visa iso field */
   Move41_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_42cardacc,                 /* field */
   HOST_MESSAGE.cardaccid,       /* data to move */
   15,                           /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_43cardaccname,             /* field */
   HOST_MESSAGE.cardaccname,     /* data to move */
   40,                           /* # of bytes in visa iso field */
   Move_IfThere,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_44respdata,                /* field */
   HOST_MESSAGE.addl_rsp_len,    /* data to move */
   1,                            /* # of bytes in resp data len field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_45track1,                  /* field */
   HOST_MESSAGE.track1_len,      /* data to move */
   1,                            /* # of bytes in track1 len field */
   CondTrack1,                   /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_48addl_data,               /* field */
   HOST_MESSAGE.addl_data_len,   /* data to move */
   1,                            /* # of bytes in addl data len field */
   Move_IfThere,                 /* conditional    */
   vDataMove,                    /* move routine */
   RspMovevData                  /* response routine */
   },
   {
   bf_49trncurcd,                /* field */
   HOST_MESSAGE.tran_curr_cd,    /* data to move */
   2,                            /* # of bytes in visa iso field */
   Move_Always,                  /* mandatory    */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_51crdcurcd,                /* field */
   HOST_MESSAGE.card_curr_cd,    /* data to move */
   2,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_52pinblock,                /* field */
   HOST_MESSAGE.pin_block,       /* data to move */
   8,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_53seccntl,                 /* field */
   HOST_MESSAGE.sec_cntl,        /* data to move */
   8,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_54addlamts,                /* field */
   HOST_MESSAGE.addl_len,        /* data to move */
   1,                            /* # of bytes in addl len field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_55ICCdata,                /* field */
   HOST_MESSAGE.ICCdata_len,      /* data to move */
   1,                            /* # of bytes in addl len field */
   Move_IfThere,                 /* conditional */
   vDataMove,                    /* move routine */
   RspMovevData                  /* response routine */
   },
   {
   bf_56pad,                	 /* field */
   HOST_MESSAGE.PAD_len,         /* data to move */
   2,                            /* # of bytes in addl len field */
   Move_IfThere,                 /* conditional */
   vBCDMove,                    /* move routine */
   RspMovevBCD_Field34           /* response routine */
   },
   {
   bf_59posdata,                 /* field */
   HOST_MESSAGE.posdata_len,     /* data to move */
   1,                            /* # of bytes in addl len field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_60priv60,                  /* field */
   HOST_MESSAGE.priv60_len,      /* data to move */
   1,                            /* # of bytes in priv60 len field */
   Move_IfThere,                 /* conditional */
   vBCDMove,                     /* move routine */
   RspMovevBCD                   /* response routine */
   },
   {
   bf_61priv61,                  /* field */
   HOST_MESSAGE.priv61_len,      /* data to move */
   1,                            /* # of bytes in priv61 len field */
   Move_IfThere,                 /* conditional */
   vBCDMove,                     /* move routine */
   RspMovevBCD                   /* response routine */
   },
   {
   bf_62priv62,                  /* field */
   HOST_MESSAGE.priv62_len,      /* data to move */
   1,                            /* # of bytes in priv62 len field */
   Move_IfThere,                 /* conditional */
   vDataMove,                    /* move routine */
   RspMovevData                  /* response routine */
   },
   {
   bf_63priv63,                  /* field */
   HOST_MESSAGE.priv63_len,      /* data to move */
   1,                            /* # of bytes in priv63 len field */
   Move_IfThere,                 /* conditional */
   vDataMove,                    /* move routine */
   RspMovevData                  /* response routine */
   },
   {
   bf_70nmicode,                 /* field */
   HOST_MESSAGE.nmi_code,        /* data to move */
   2,                            /* # of bytes in visa iso field */
   Move_Always,                  /* mandatory    */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_73action,                  /* field */
   HOST_MESSAGE.action,          /* data to move */
   3,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_90orgdata,                 /* field */
   HOST_MESSAGE.orig_data,       /* data to move */
   21,                           /* # of bytes in visa iso field */
   Move_Always,                  /* mandatory    */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_91fileupdate,              /* field */
   HOST_MESSAGE.file_update,     /* data to move */
   1,                            /* # of bytes in visa iso field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_93rspind,                  /* field */
   HOST_MESSAGE.resp_ind,        /* data to move */
   5,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional  */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_95replamts,                /* field */
   HOST_MESSAGE.repl_amts,       /* data to move */
   42,                           /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional  */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_96msgsec,                  /* field */
   HOST_MESSAGE.msgsec,          /* data to move */
   8,                            /* # of bytes in visa iso field */
   Move_Always,                  /* mandatory    */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_100rcvid,                  /* field */
   HOST_MESSAGE.recv_inst_len,   /* data to move */
   1,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   vBCDMove,                     /* move routine */
   RspMovevBCD                   /* response routine */
   },
   {
   bf_101fname,                  /* field */
   HOST_MESSAGE.fname_len,       /* data to move */
   1,                            /* # of bytes in track1 len field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_102acct1,                  /* field */
   HOST_MESSAGE.acct_id_1_len,   /* data to move */
   1,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_103acct2,                  /* field */
   HOST_MESSAGE.acct_id_2_len,   /* data to move */
   1,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_104trandesc,               /* field */
   HOST_MESSAGE.trandesc_len,    /* data to move */
   1,                            /* # of bytes in track1 len field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_116issref,                 /* field */
   HOST_MESSAGE.issref_len,      /* data to move */
   1,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   vDataMove,                    /* move routine */
   RspMovevData                  /* response routine */
   },
   {
   bf_118intractry,              /* field */
   HOST_MESSAGE.intra_ctry_len,  /* data to move */
   1,                            /* # of bytes in visa iso field */
   Move_Never,                   /* conditional */
   vDataMove,                    /* move routine */
   RspMovevData                  /* response routine */
   },
   {
   bf_120orgmsgid,               /* field */
   HOST_MESSAGE.org_msg_id,      /* data to move */
   3,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   vBCDMove,                     /* move routine */
   RspMovevBCD                   /* response routine */
   },
   {
   bf_121issid,                  /* field */
   HOST_MESSAGE.iss_inst_len,    /* data to move */
   1,                            /* # of bytes in visa iso field */
   Move_IfThere,                   /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_122openuse,                /* field */
   HOST_MESSAGE.open_use_len,    /* data to move */
   1,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_123addverf,                /* field */
   HOST_MESSAGE.addr_verf_len,    /* data to move */
   1,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_124freeform,               /* field */
   HOST_MESSAGE.free_text_len,   /* data to move */
   1,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
    bf_125supinfo,               /* field */
    HOST_MESSAGE.sup_info_len,   /*data to move */
    1,                           /* # of bytes in visa iso field */ 
    Move_IfThere,                /* conditional */
    vEBCDICMove,                 /* move routine */
    RspEBCDICvMove               /* response routine */
   },

   {
   bf_126priv126,                /* field */
   HOST_MESSAGE.de126_len,       /* data to move */
   1,                            /* # of bytes in priv60 len field */
   vMove_IfThere,                /* conditional */
   vDataMove,                    /* move routine */
   RspMovevData                  /* response routine */
   },
   {
   bf_127filerecs,               /* field */
   HOST_MESSAGE.file_rec_len,    /* data to move */
   1,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   vDataMove,                    /* move routine */
   RspMovevData                  /* response routine */
   },

   {
   bf_130termcap,                /* field */
   HOST_MESSAGE.term_cap_prof,   /* data to move */
   3,                            /* # of bytes in visa iso field */
   MoveEMV_IfThere,              /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },

   {
   bf_131tvr,                    /* field */
   HOST_MESSAGE.tvr,             /* data to move */
   5,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },

   {
   bf_132unpredict,              /* field */
   HOST_MESSAGE.unpredictable,   /* data to move */
   4,                            /* # of bytes in visa iso field */
   MoveEMV_IfThere,              /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },

   {
   bf_133termserial,             /* field */
   HOST_MESSAGE.tid_serial,      /* data to move */
   8,                            /* # of bytes in visa iso field */
   MoveEMV_IfThere,              /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },

   {
   bf_134visadata,               /* field */
   HOST_MESSAGE.visa_data_len,   /* data to move */
   1,                            /* # of bytes in visa iso field */
   vMove_IfThere,                /* conditional */
   vDataMove,                    /* move routine */
   RspMovevData                  /* response routine */
   },

   {
   bf_135issdata,                /* field */
   HOST_MESSAGE.iss_data_len,    /* data to move */
   1,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   vDataMove,                    /* move routine */
   RspMovevData                  /* response routine */
   },

   {
   bf_136crytogram,              /* field */
   HOST_MESSAGE.cryptogram,      /* data to move */
   8,                            /* # of bytes in visa iso field */
   MoveEMV_IfThere,              /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },

   {
   bf_137apptranctr,             /* field */
   HOST_MESSAGE.app_tran_ctr,    /* data to move */
   2,                            /* # of bytes in visa iso field */
   MoveEMV_IfThere,              /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },

   {
   bf_138app_profile,            /* field */
   HOST_MESSAGE.app_profile,     /* data to move */
   2,                            /* # of bytes in visa iso field */
   MoveEMV_IfThere,              /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },

   {
   bf_139arpcresp,               /* field */
   HOST_MESSAGE.arpc_resp,       /* data to move */
   10,                           /* # of bytes in visa iso field */
   MoveEMV_IfThere,              /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },

   {
   bf_140issauth,                /* field */
   HOST_MESSAGE.iss_auth_len,    /* data to move */
   1,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   vDataMove,                    /* move routine */
   RspMovevData                  /* response routine */
   },

   {
   bf_142iss_script,             /* field */
   HOST_MESSAGE.iss_script_len,  /* data to move */
   1,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   vDataMove,                    /* move routine */
   RspMovevData                  /* response routine */
   },

   {
   bf_143iss_result,             /* field */
   HOST_MESSAGE.iss_result_len,  /* data to move */
   1,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   vDataMove,                    /* move routine */
   RspMovevData                  /* response routine */
   },

   {
   bf_144crytpotype,             /* field */
   HOST_MESSAGE.crypto_type,     /* data to move */
   1,                            /* # of bytes in visa iso field */
   MoveEMV_IfThere,              /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },

   {
   bf_145tcountry,               /* field */
   HOST_MESSAGE.tid_country,     /* data to move */
   2,                            /* # of bytes in visa iso field */
   MoveEMV_IfThere,              /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },

   {
   bf_146termtxndate,            /* field */
   HOST_MESSAGE.tid_tran_date,   /* data to move */
   3,                            /* # of bytes in visa iso field */
   MoveEMV_IfThere,              /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },

   {
   bf_147cryptamt,               /* field */
   HOST_MESSAGE.crypto_amt,      /* data to move */
   6,                            /* # of bytes in visa iso field */
   MoveEMV_IfThere,              /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },

   {
   bf_148cryptcurcd,             /* field */
   HOST_MESSAGE.crypto_curr,     /* data to move */
   2,                            /* # of bytes in visa iso field */
   MoveEMV_IfThere,              /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },

   {
   bf_149cryptcash,              /* field */
   HOST_MESSAGE.crypto_cashbk,   /* data to move */
   6,                            /* # of bytes in visa iso field */
   MoveEMV_IfThere,              /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },

   {
   bf_done,                      /* field */
   NULL,                         /* data to move */
   99,                           /* # of bytes in visa iso field */
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


long int Hex_to_Dec(char * buf_ptr)
{

    CHAR hex[20];
    long int decimal = 0, base = 1;
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

   if ((reqidx + pBF->bd_fieldlen) <= buflen)
   {
      if (pBF != NULL)
      {
         memcpy(pBF->p_bd_pdata,         /* to data pointer      */
                &ReqRsp_Buffer[reqidx],      /* move from request buffer */
                pBF->bd_fieldlen);      /* for length of field    */

         reqidx = reqidx + pBF->bd_fieldlen;   /* increment buffer pointer */
      }
   }
   else
      invalid_msg_format = true;

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

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

	//The next 4 lines were modified by farad on 8/13/98
   if (pBF != NULL)
   {
      movelen = pBF->p_bd_pdata[0];
      ReqRsp_Buffer [reqidx] = pBF->p_bd_pdata[0];
      reqidx++;

      for( i=1; i<=movelen; i++ )
      {
         ReqRsp_Buffer[reqidx] = ascii_ebcdic_conv (pBF->p_bd_pdata[i], A2E);
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
   int                  i;
   int                  movelen;

   /*---------------------------------------------------*/
   /* Find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   /* Determine number of bytes to move. */
   if (pBF != NULL)
   {
      movelen = ReqRsp_Buffer[reqidx];
      if ((reqidx+movelen+1) <= buflen)
      {
         pBF->p_bd_pdata[0] = ReqRsp_Buffer[reqidx];
         reqidx++;

         /* Move the data, converting it from EBCDIC to ASCII. */
         for( i=1; i<=movelen; i++)
         {
            pBF->p_bd_pdata [i] = ascii_ebcdic_conv (ReqRsp_Buffer[reqidx], E2A);
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

      /* Move the data, converting it from EBCDIC to ASCII. */
      if ((reqidx + movelen) <= buflen)
      {
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
/*******************************************************************************************************/
/* Isfield Null ...... TF Neetu Bhat/Phani                                                                                           */
/*******************************************************************************************************/
BYTE IsFieldNull (char str[])
{
	if (strlen (str) == 0)
		return 1 ;
	return 0 ;
}
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
      if ((reqidx + pBF->bd_fieldlen) <= buflen)
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
	   if (34 == fieldid)
	   {
		   movelen = genutil_bcd_to_int(&pBF->p_bd_pdata[0], pBF->bd_fieldlen);
		   memcpy(&ReqRsp_Buffer[reqidx], &pBF->p_bd_pdata[0], pBF->bd_fieldlen);
		   reqidx = reqidx + 2;
	   }
	   else
	   {
      movelen = pBF->p_bd_pdata[0];
      ReqRsp_Buffer[reqidx] = pBF->p_bd_pdata[0];
      reqidx++;

	   }

      if (( 60 == fieldid ) || ( 61 == fieldid ) || ( 120 == fieldid ))
         movelen *= 2;

      if ((movelen % 2) != 0)
         movelen++;

      if (34 != fieldid)
      movelen = (movelen / 2);

      if (fieldid == 34)
    	  memcpy(&ReqRsp_Buffer[reqidx], &pBF->p_bd_pdata[2], movelen);
      else
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
      if ( (ReqRsp_Buffer[0] == 0x03) && (ReqRsp_Buffer[1] == 0x02) )
      {
         /* Convert length from BCD to binary. */
         movelen  = ((ReqRsp_Buffer[reqidx] & 0xf0) >> 4) * 10;
         movelen +=   ReqRsp_Buffer[reqidx] & 0x0f;
      }
      else
         movelen = ReqRsp_Buffer[reqidx];

      pBF->p_bd_pdata[0] = movelen;
      reqidx++;

      if (( 60 == fieldid ) || ( 61 == fieldid ) || ( 120 == fieldid ))
         movelen *= 2;

      if ((movelen % 2) != 0)
         movelen++;

      movelen/= 2;

      if (( reqidx + movelen) <= buflen)
      {
         memcpy(&pBF->p_bd_pdata[1], &ReqRsp_Buffer[reqidx], movelen);
         reqidx = reqidx + movelen;   /* increment buffer pointer */
      }
      else
         invalid_msg_format = true;
   }
} /* end RspMovevBCD */

static  void RspMovevBCD_Field34 (enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  movelen;
   INT  dec_f34 = 0;
   CHAR hex_f34[5]= {};

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   if (pBF != NULL)
   {
    	  	  	  /* DE 34 */
		/*movelen = genutil_bcd_to_int(&ReqRsp_Buffer[reqidx], pBF->bd_fieldlen);
		sprintf(hex_f34, "%ld",movelen);
		dec_f34 = Hex_to_Dec(hex_f34); */

		dec_f34 = ReqRsp_Buffer[reqidx+1];
				/* Copy length */
		memcpy(&pBF->p_bd_pdata[0], &ReqRsp_Buffer[reqidx], pBF->bd_fieldlen );
		reqidx = reqidx +2;

      if (( reqidx + dec_f34) <= buflen)
      {			/* Copy data */

			memcpy(&pBF->p_bd_pdata[2], &ReqRsp_Buffer[reqidx], dec_f34);
    	 reqidx = reqidx + dec_f34;
      }
      else
         invalid_msg_format = true;
   }
} /* end RspMovevBCD_Field34 */
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
      movelen = pBF->p_bd_pdata [0];
      ReqRsp_Buffer [reqidx] = pBF->p_bd_pdata [0];
      reqidx++;

      memcpy(&ReqRsp_Buffer[reqidx], &pBF->p_bd_pdata[1], movelen);

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

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   if (pBF != NULL)
   {
      movelen = ReqRsp_Buffer[reqidx];

      pBF->p_bd_pdata[0] = ReqRsp_Buffer[reqidx];
      reqidx++;

      memcpy(&pBF->p_bd_pdata[1], &ReqRsp_Buffer[reqidx], movelen);

      reqidx = reqidx + movelen;   /* increment buffer pointer */
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
******************************************************************************/
static  INT CondProc(enum bitfields bitfield, BYTE trantype)
{
   if (HOST_MESSAGE.msgtype_bcd [0] != 0x04)
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

  Function:    Move_IfThere

  Description: This function will determine if the data for fieldid, returns false only for NULL data and
   	   	   	   	returns true for all other cases.
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
static  INT Move41_IfThere(enum bitfields fieldid, BYTE p_trantype)
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
         if ( (pBF->p_bd_pdata [i] != 0))
            return(true);
      }
   }
   return(false);
} /* end Move41_IfThere */

// TF PHANI
/*****************************************************************************

  Function:    Move_BCD_Amount_IfThere

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
  Modified by: TF PHANI
******************************************************************************/
static  INT Move_BCD_Amount_IfThere(enum bitfields fieldid, BYTE p_trantype)
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
         if ( (pBF->p_bd_pdata [i] > 0) && (pBF->p_bd_pdata [i] < (0x99+1))  ) 
            return(true);
      }
   }
   return(false);
} /* end Move_BCD_Amount_IfThere */


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

  Function:    MoveEMV_IfThere

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
static  INT MoveEMV_IfThere(enum bitfields fieldid, BYTE p_trantype)
{
   struct bitfield_data *pBF;
   INT  i;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   /*---------------------------------------------------*/
   /* in any byte non-zero, it's there, return true     */
   /*---------------------------------------------------*/
   if (pBF != NULL)
   {
      if ( (Third_Bitmap == true) && (fieldid > 128) )
         return( true );

      for (i=0; i < pBF->bd_fieldlen; i++)
      {
         if ( (pBF->p_bd_pdata [i] != 0) && (pBF->p_bd_pdata [i] != ' ') ) 
            return(true);
      }
   }
   return(false);
} /* end Move_IfThere */

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
   char error_msg[256] = {0};

   
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
	  sprintf(error_msg,"%d this trantype not found in TRANTABLE",trantype);
      ncvis2_log_message( 3, 2, error_msg , "Find_Tran_Table" , 0 ,"");
      return(ptrandef);
   }
   return (NULL);
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
   CHAR     temp_str[100] = {0};

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
      ncvis2_log_message( 3, 2, temp_str , "Find_Bitfield_Table" , 0 ,"");
      return (pBF);
   };
 
   return (NULL);
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
	CHAR	    ErrorMsg[100] = {0};
	CHAR	    Buffer[512] = {0};

	build_host_structure(p_auth_tx);
	Build_Host_Request(p_auth_tx);
	Build_Host_Header(p_auth_tx);

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
	  ncvis2_log_message( 3, 3, Log_Bufr , "hhutil_send_net_mngmnt_response" , 0 ,p_auth_tx->TLF01_details.card_num);
	   return(false);           
	}

	ret_code = pteipc_send(p_msg_host_out, dialog_que_name );
	free(p_msg_host_out);
	if( ret_code < 0 )
	{
		pteipc_get_errormsg( ret_code, ErrorMsg );
		sprintf( Buffer, "pteipc error sending to que %s: %s", dialog_que_name, ErrorMsg );
		memset(Log_Bufr,0x00,sizeof(Log_Bufr));
		sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
				p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
		strcat(Buffer,Log_Bufr);
		ncvis2_log_message( 3, 2, Buffer , "hhutil_send_net_mngmnt_response" , 0 ,p_auth_tx->TLF01_details.card_num);
		return(false);
	}
	else
	{
	  /* Update transaction statistics */
	  update_txn_stats( &Txn_Counts, p_auth_tx, MGT );//praneeth changed SENT to MGT

	  /* Log transaction to trace file. */
	  write_to_txn_file();
	}
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
	pPTE_MSG	p_msg_host_out        = NULL_PTR;
	LONG      	ret_code;
	CHAR	    ErrorMsg[100] = {0};
	CHAR	    Buffer[512]   = {0};
	CHAR      	gmt_time[20];

	/* Current Greenwich Mean Time for the transmission-to-host time. */
	pintime_get_gmt( gmt_time );  /* YYYYMMDDhhmmss */
	strncpy( p_auth_tx->TLF01_details.transmission_timestamp,
			&gmt_time[4], 10 );  /* MMDDhhmmss */

	build_host_structure(p_auth_tx);
	Build_Host_Request(p_auth_tx);
	Build_Host_Header(p_auth_tx);

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
	  ncvis2_log_message( 3, 3, Log_Bufr , "hhutil_send_net_mngmnt_request" , 0 ,p_auth_tx->TLF01_details.card_num);
	  return(false);
	}

	ret_code = pteipc_send(p_msg_host_out, dialog_que_name );
	free( p_msg_host_out );
	if( ret_code < 0 )
	{
		pteipc_get_errormsg( ret_code, ErrorMsg );
		sprintf( Buffer, "pteipc error sending to que %s: %s", dialog_que_name, ErrorMsg );
		memset(Log_Bufr,0x00,sizeof(Log_Bufr));
		sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
	  		 p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
		strcat(Buffer,Log_Bufr);
		ncvis2_log_message( 3, 3, Buffer , "hhutil_send_net_mngmnt_request" , 0 ,p_auth_tx->TLF01_details.card_num);
		return(false);
	}
	else
	{
	  /* Update transaction statistics */
		update_txn_stats( &Txn_Counts, p_auth_tx, MGT );//praneeth changed SENT to MGT

	  /* Log transaction to trace file. */
		write_to_txn_file();
	}
	return(true);
} /* hhutil_send_net_mngmnt_request */


/*****************************************************************************

  Function:    Field62Response

  Description: This function will decode field 62 data from HOST_MESSAGE
               and stores pertinent data into the auth_tx structure.
  Author: 
      unknown
  Inputs:
      HOST_MESSAGE (global)  - structure that contains field 62 data 
  Outputs:
      p_auth_tx - Transaction record - parsed data goes into it
  Return values:
      None
  Modified by:
******************************************************************************/
void Field62Response( pAUTH_TX p_auth_tx )
{
   INT     i;
   INT     cps_bit;
   INT     cps_byte;
   INT     Len;
   INT     TotalLen;
   INT     BitMap_Len = 8;
   BYTE    BitMap[8];
   BYTE    TempStr[51];
   CHAR    temp_txn_id[17];


   /* Get length of field. */
   TotalLen = (INT)HOST_MESSAGE.priv62_len;

   /* Copy bitmap into local variable. */
   if ( TotalLen > 0 )
   {
      memset( BitMap, 0x00, sizeof(BitMap) );
      for( i=0; i<BitMap_Len; i++ )
         BitMap[i] = HOST_MESSAGE.priv62[i];

      memset( TempStr, 0x00, sizeof(TempStr) );
      memcpy( TempStr, &HOST_MESSAGE.priv62[8],
              sizeof(HOST_MESSAGE.priv62) - BitMap_Len );
   }

   /* Parse the bitmap for 1st 3 bytes, extracting the fields we want. */
   Len = 0;
   for( cps_byte=1; cps_byte<=3; cps_byte++ )
   {
      for( cps_bit=1; cps_bit<=8; cps_bit++ )
      {
         if ( BitMap[cps_byte-1] & BITTAB[cps_bit].bit_mask )
         {
            /* The bit is set. */
            switch( cps_byte )
            {
               case 1: /* Byte 1 */
                      switch( cps_bit )
                      {
                         case 1:  
                                /* CPS Authorization */
                                p_auth_tx->TLF01_details.visa_char_indicator =
                                   ascii_ebcdic_conv( TempStr[Len], E2A );

                                Len++;
                                break;
                         case 2:
                                /* CPS Transaction ID */
                                memset( temp_txn_id, 0x00, sizeof(temp_txn_id));
                                bcd_to_asc( &TempStr[Len], 8, temp_txn_id, 16 );
                                strcpy( p_auth_tx->TLF01_details.visa_tran_id,
                                       &temp_txn_id[1] );
                                Len += 8;
                                break;
                         case 3:
                                /* CPS Validation Code */
                                for( i=0; i<4 && ((Len+i)<sizeof(TempStr)); i++)
                                   p_auth_tx->TLF01_details.visa_validation_code[i]
                                   = ascii_ebcdic_conv( TempStr[Len+i], E2A );

                                /* Trim any trailing spaces. */
                                trim_spaces( p_auth_tx->TLF01_details.visa_validation_code );
                                Len += 4;
                                break;
                         case 7:
                                Len += 26;
                                break;
                         case 8:
                                Len += 3;
                                break;
                         default:
                                Len++;
                                break;
                      }
                      break;
               case 2: /* Byte 2 */
                      switch( cps_bit+8 )
                      {
                         case 10: Len += 3;
                                  break;
                         case 14: Len += 6;
                                  break;
                         case 16: Len += 2;
                                  break;
                         default: Len++;
                                  break;
                      }
                      break;
               case 3: /* Byte 3 */
                      switch( cps_bit+16 )
                      {
                         case 17:
//                                  for( i=0; (i<13) && ((Len+i)<sizeof(TempStr)); i++ )
//                                     p_auth_tx->TLF01_details.cps_master_card_info[i]
//                                     = ascii_ebcdic_conv( TempStr[Len+i], E2A );
                         default: break;
                      }
               default: break;
            }  /* End SWITCH based on BYTE */
         }  /* End IF bit is set */
      }  /* End LOOP on BIT */
   }  /* End LOOP on BYTE */
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
      ReqRsp_Buffer (global) - the buffer that contains the ISO formatted message
  Return values:
      true  - no error occurred
      false - an error occurred
  Modified by:
******************************************************************************/
BOOLEAN Build_Host_Request ( pAUTH_TX p_auth_tx)
{
   struct trandef       *pTranDef;
   struct bitfield_data *pBF;
   INT                  bfidx;    /* index to bitfields for this transaction */
   BOOLEAN              ret_val = true;

   /* Get pointer to tran table */
   pTranDef = Find_Tran_Table(p_auth_tx->TLF01_details.tx_key);
   if (pTranDef != NULL)
   {    
      /*-----------------------------------------*/
      /* Copy in Message Id for this transaction */
      /*-----------------------------------------*/
      memset(ReqRsp_Buffer, 0, COMM_BUF_SZE);   /* zero out the buffer */
      bitmapidx = 2 + VISA_HDR_SIZE;            /* point to bitmap     */

      /* point past bitmap   */
      if ( Third_Bitmap == true )
      {
         reqidx = 26 + VISA_HDR_SIZE;
         BitFieldSize = THIRD_BITFIELD;

         /* Set bits in bitmap to indicate 2nd & 3rd bitmaps. */
         ReqRsp_Buffer[bitmapidx]   |= BITTAB[bf_1xbitfield].bit_mask;
         ReqRsp_Buffer[bitmapidx+8] |= BITTAB[bf_2xbitfield].bit_mask;
      }
      else if((AUTH_AUTHORIZATION_RESPONSE == p_auth_tx->TLF01_details.tx_key)||
              (AUTH_CASH_ADVANCE_RESPONSE  == p_auth_tx->TLF01_details.tx_key) )
      {
         if ((0 != strlen(HOST_MESSAGE.acct_id_1)) ||
             (0 != strlen(HOST_MESSAGE.acct_id_2)) ||
             (0 != strlen(HOST_MESSAGE.iss_inst_id)))  
         {
            reqidx = 18 + VISA_HDR_SIZE;
            ReqRsp_Buffer[bitmapidx] |= BITTAB[bf_1xbitfield].bit_mask;   /* 2 bitmaps */
            BitFieldSize = EXTENDED_BITFIELD;
         }
         else 
         {
            reqidx = 10 + VISA_HDR_SIZE;
            BitFieldSize = NORMAL_BITFIELD;
         }
      }
      else if ( AUTH_REVERSAL_RESPONSE == p_auth_tx->TLF01_details.tx_key ||
    		    AUTH_VOID_PRE_AUTHORIZATION_RESPONSE == p_auth_tx->TLF01_details.tx_key)
      {
         if ((0 != strlen(HOST_MESSAGE.acct_id_1  )) ||
             (0 != strlen(HOST_MESSAGE.acct_id_2  )) ||
             (0 != strlen(HOST_MESSAGE.iss_inst_id)) ||
             (0 != strlen(HOST_MESSAGE.repl_amts  )) ||
             (0 != strlen(HOST_MESSAGE.intra_ctry ))   )  
         {
            reqidx = 18 + VISA_HDR_SIZE;
            ReqRsp_Buffer[bitmapidx] |= BITTAB[bf_1xbitfield].bit_mask;   /* 2 bitmaps */
            BitFieldSize = EXTENDED_BITFIELD;
         }
         else 
         {
            reqidx = 10 + VISA_HDR_SIZE;
            BitFieldSize = NORMAL_BITFIELD;
         }
      }
      else if ( (AUTH_REVERSAL        == p_auth_tx->TLF01_details.tx_key) ||
                (AUTH_REVERSAL_REPEAT == p_auth_tx->TLF01_details.tx_key) ||
                (AUTH_CARD_UPDATE     == p_auth_tx->TLF01_details.tx_key) ||
                (AUTH_VOID_SALE       == p_auth_tx->TLF01_details.tx_key) ||
				(AUTH_ATM_CONFIRM     == p_auth_tx->TLF01_details.tx_key) ||
				(AUTH_REVERSAL_ADVICE == p_auth_tx->TLF01_details.tx_key) ||
                (AUTH_REVERSAL_ADVICE_REPEAT == p_auth_tx->TLF01_details.tx_key) || // TF Ajay
				(AUTH_VOID_PRE_AUTHORIZATION == p_auth_tx->TLF01_details.tx_key)
      ) /* Added AUTH_REVERSAL_ADVICE and repeat -Tf Phani*/
      {
         reqidx = 18 + VISA_HDR_SIZE;
         ReqRsp_Buffer[bitmapidx] |= BITTAB[bf_1xbitfield].bit_mask;   /* 2 bitmaps */
         BitFieldSize = EXTENDED_BITFIELD;
      }
      else if ( (AUTH_LOGON          == p_auth_tx->TLF01_details.tx_key) ||
                (AUTH_LOGON_RESPONSE == p_auth_tx->TLF01_details.tx_key)  )
      {
         reqidx = 18 + VISA_HDR_SIZE;
         ReqRsp_Buffer[bitmapidx] |= BITTAB[bf_1xbitfield].bit_mask;   /* 2 bitmaps */
         BitFieldSize = EXTENDED_BITFIELD;
      }
      else if (HOST_MESSAGE.de126_len[0] != 0x00)/* check for VbV field 126 & create two bit maps */
      {
         reqidx = 18 + VISA_HDR_SIZE;
         ReqRsp_Buffer[bitmapidx] |= BITTAB[bf_1xbitfield].bit_mask;   /* 2 bitmaps */
         BitFieldSize = EXTENDED_BITFIELD;			
      }
      else
      {
         reqidx = 10 + VISA_HDR_SIZE;
         BitFieldSize = NORMAL_BITFIELD;
      }

      memcpy(&ReqRsp_Buffer[0+VISA_HDR_SIZE], pTranDef->td_msgid, sizeof(pTranDef->td_msgid));

      /*   
         if this msg doesn't require the original proccode (its already in the ISO
         field), overwrite it with the one from the trandef table.
       */
   
      if ((pTranDef->td_opts2 & topt2_new_proccode) != 0) 
         memcpy (&HOST_MESSAGE.proccode, pTranDef->td_proccode, sizeof(pTranDef->td_proccode));
      

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
               if ( pBF->bd_fieldid > BitFieldSize )
                  break;

               if(bfidx == 25)
               {
                  if(strlen(p_auth_tx->TLF01_details.pos_condition_code) == 0)
                     continue;
               }
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
   }
   else
      ret_val = false;

   Third_Bitmap = false;
   return( ret_val );
} /* end Build_Request_Message */



/*****************************************************************************

  Function:    hhutil_parse_host_response_msg

  Description: This function will parse a host response message
  Author: 
      unknown
  Inputs:
      p_auth_tx - a pointer to the auth_tx structure
      ReqRsp_Buffer (global) - the buffer that contains the ISO formatted msg
  Outputs:
      HOST_MESSAGE (global)  - structure that contains data to go into the 
                               ISO message
  Return values:
      true  - no error occurred
      false - an error occurred
******************************************************************************/
INT hhutil_parse_host_response_msg(pAUTH_TX p_auth_tx)
{
   struct bitfield_data *pBF;
   INT                  hdr_size;
   INT                  i;
   INT                  ret_val = true;
   INT                  bfidx;    /* index to bitfields for this transaction */
   CHAR				    ErrorMsg[512] = {0};
   CHAR                 tid[9];
   CHAR                 mid[16];
   CHAR                 card_num[20];
   CHAR                 tempstr[50];


   memset( &HOST_MESSAGE, 0x00, sizeof(HOST_MESSAGE) );
   memset( &Dest_Stat_Id, 0x00, sizeof(Dest_Stat_Id) );

   if (0x1A == ReqRsp_Buffer[0])
   {
      Txn_Is_A_Reject = true;
      hdr_size = VISA_HDR_SIZE + REJECT_VISA_HDR_SIZE;

      /* Get the Reject Code */
      memcpy( HOST_MESSAGE.msgtype_bcd,
             &ReqRsp_Buffer[REJECT_VISA_HDR_SIZE - 2],
             sizeof(HOST_MESSAGE.msgtype_bcd));

      bcd_to_asc( HOST_MESSAGE.msgtype_bcd,
                  sizeof(HOST_MESSAGE.msgtype_bcd),
                  p_auth_tx->TLF01_details.message_type,
                  sizeof(p_auth_tx->TLF01_details.message_type) );

      strcpy( ErrorMsg, "Received header reject code: " );
      strcat( ErrorMsg, p_auth_tx->TLF01_details.message_type );

      ret_val = true;
   }
   else if (0x16 == ReqRsp_Buffer[0])
   {
      Txn_Is_A_Reject = false;
      hdr_size = VISA_HDR_SIZE;

      /* Parse the Destination Station Id */
      memcpy( HOST_MESSAGE.dest_stat_id,
             &ReqRsp_Buffer[5],
              sizeof(HOST_MESSAGE.dest_stat_id) );

      /* Store the destination station id */
      bcd_to_asc( HOST_MESSAGE.dest_stat_id,
                  sizeof(HOST_MESSAGE.dest_stat_id),
                  p_auth_tx->incoming_dest_station_id,
                  sizeof(p_auth_tx->incoming_dest_station_id) );

      /* Parse the Source Station Id */
      memcpy( HOST_MESSAGE.src_stat_id,
             &ReqRsp_Buffer[8],
              sizeof(HOST_MESSAGE.src_stat_id) );

      /* Store the source station id */
      bcd_to_asc( HOST_MESSAGE.src_stat_id,
                  sizeof(HOST_MESSAGE.src_stat_id),
                  p_auth_tx->incoming_src_station_id,
                  sizeof(p_auth_tx->incoming_src_station_id) );

      /* Store the Header so we can echo any fields we need to.
       * This is for switched in txns.  Set the first byte to
       * 'I' to indicate a switched in txn.  If this txn is a
       * response to a switched out txn, this field does not
       * get used.
       */
      HOST_MESSAGE.header[0] = 0x02;
      memcpy( HOST_MESSAGE.header+1, ReqRsp_Buffer, 22 );
   }
   else if ((ReqRsp_Buffer[0] == 0x03) && (ReqRsp_Buffer[1] == 0x02) )
   {
      /* 0302 Request from a device controller. No Visa header. */
      Txn_Is_A_Reject = false;
      hdr_size = 0;
   }
   else if ((ReqRsp_Buffer[0] == 0x03) && (ReqRsp_Buffer[1] == 0x12) )	// ThoughtFocus nirbhay: Cadencie link changes
   {
	/* Response for 0302 (0302 + 10 = 0312) msg from VISA. Forward without parsing to Cadencie */
	Txn_Is_A_Reject = false;
	hdr_size = 0;
   }
   else
   {
      Txn_Is_A_Reject = false;
      ncvis2_log_message( 3, 2, "Invalid message format received. Unexpected Visa Header length" , "hhutil_parse_host_response_msg" , 0 ,"");
      ret_val = false;
   }

   if ( ret_val == true )
   {
      /* Get the Message Type */
      memcpy( HOST_MESSAGE.msgtype_bcd,
             &ReqRsp_Buffer[hdr_size],
              sizeof(HOST_MESSAGE.msgtype_bcd) );


// route 302 msg directly to ncvisa : ThoughtFocus nirbhay: cadencie changes.
	if(HOST_MESSAGE.msgtype_bcd[0] == 0x03 && HOST_MESSAGE.msgtype_bcd[1] == 0x02)
	{
		return( false );
	}
	else if(HOST_MESSAGE.msgtype_bcd[0] == 0x03 && HOST_MESSAGE.msgtype_bcd[1] == 0x12)
	{
		return( true );
	}
      /* Point past bitmap, but 1st check for extended bitmap. */
      bitmapidx = 2 + hdr_size;
      if ( ReqRsp_Buffer[bitmapidx]  &  BITTAB[bf_1xbitfield].bit_mask )
      {
         if ( ReqRsp_Buffer[bitmapidx+8]  &  BITTAB[bf_2xbitfield].bit_mask )
         {
            /* 3 bitmaps */
            reqidx = 26 + hdr_size;
            BitFieldSize = THIRD_BITFIELD;
         }
         else
         {
            /* 2 bitmaps */
            reqidx = 18 + hdr_size;
            BitFieldSize = EXTENDED_BITFIELD;
         }
      }
      else
      {
         /* 1 bitmaps */
         reqidx = 10 + hdr_size;
         BitFieldSize = NORMAL_BITFIELD;
      }

      /*------------------------------------------------------------------
         For every bitfield present in message ( except first bit ) . . .
      -------------------------------------------------------------------*/
      for (bfidx = 2; bfidx <= BitFieldSize; bfidx++)
      {
         if (ReqRsp_Buffer[bitmapidx+BITTAB[bfidx].byte_no] & BITTAB[bfidx].bit_mask)
         {
            /* Don't get bitfield for 3rd Bitmap Indicator. */
            if ( bfidx != 65 )
            {
               /* Bit is on, field is there.... */
               pBF = Find_Bitfield_Table(bfidx);       

               /*---------------------------------------*/
               /* call the Response Handling routine... */
               /*---------------------------------------*/
	
               if (pBF != NULL)
               {
                  (pBF->bd_BFRsp)(pBF->bd_fieldid);
                  if (true == invalid_msg_format)
                  {
                     ncvis2_log_message( 3, 2, "Unable to parse transaction from Visa Network." , "hhutil_parse_host_response_msg" , 0 ,"");
                     ret_val = false;
                     break;
                  }
               }
               else
               {
                  ret_val = false;
                  break;
               }
            }
         }
      }

      if ( Txn_Is_A_Reject == true )
      {
         /* Terminal ID, Merchant ID, and Card Number and log reject message. */
         memset( tid,      0x00, sizeof(tid)      );
         memset( mid,      0x00, sizeof(mid)      );
         memset( card_num, 0x00, sizeof(card_num) );

         memcpy( tid, HOST_MESSAGE.termid,    sizeof(tid)-1 );
         memcpy( mid, HOST_MESSAGE.cardaccid, sizeof(mid)-1 );

         /* Card Number is a little more tricky to get.
          * It is variable length, and might have a leading zero.
          */
         memset( tempstr, 0x00, sizeof(tempstr) );
         bcd_to_asc( HOST_MESSAGE.pan_bin,
                    (HOST_MESSAGE.pan_len[0]+1)/2,
                     tempstr,
                     sizeof(p_auth_tx->TLF01_details.card_num) );

         i = HOST_MESSAGE.pan_len[0] % 2;
         memcpy( card_num, tempstr+i, sizeof(card_num)-1 );

         /* All this work to log a useful message.  Here it is. */
         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
         		p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
         strcat(ErrorMsg,Log_Bufr);
         ncvis2_log_message( 3, 2, ErrorMsg , "hhutil_parse_host_response_msg" , 0 ,p_auth_tx->TLF01_details.card_num);
      }
   }
   return( ret_val );
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
   #define  CVV2_PRESENCE  0
   #define  CVV2_RESPONSE  1
   #define  CVV2_VALUE     2

   INT   retval = true;
   CHAR  tempstr [256];  
   pCHAR tempptr;    
   CHAR  templen [6];	 
   INT   i;
   INT   actual_int;
   INT   total_int;
   INT   reversal_int;
   CHAR  error_msg[512]={0};
   CHAR  temp[38]; 
   CHAR  Buffer[512]={0};
   BYTE  cavv_result[2];
   BYTE  carc_result[2];
   BYTE  cvv2_result[2];
   BYTE  cvv2_data[7];
   Addl_Amts  acct_bal[4];
   INT        bal_set;
   INT        len;
   INT        emv_len;
   INT        addl_amts_size;
   INT        emv_txn = false;


   memset(vLogon_NMI_Code, 0x00, 4);

   /* VISA HEADER */

   /* Store in Merchant Contact field for switched in txns.
    * If txn is switched out, this field will not get copied
    * to the auth_tx from the timer, so it has no effect.
    */
   memcpy( p_auth_tx->MCF01_details.contact, HOST_MESSAGE.header, 23 );

   /* MESSAGE TYPE */
   bcd_to_asc (HOST_MESSAGE.msgtype_bcd, sizeof(HOST_MESSAGE.msgtype_bcd), 
               p_auth_tx->TLF01_details.message_type,
               sizeof(p_auth_tx->TLF01_details.message_type));

   /* FIELD 2: PAN */
   memset(&tempstr, 0x00, sizeof(tempstr));
   memcpy(p_auth_tx->TLF01_details.card_num_len,HOST_MESSAGE.pan_len,bf2_len);
   bcd_to_asc( HOST_MESSAGE.pan_bin, (HOST_MESSAGE.pan_len[0]+1)/2, tempstr,
               sizeof(p_auth_tx->TLF01_details.card_num) );
   if ( 1 == (HOST_MESSAGE.pan_len[0] % 2) )
   {
      itoa((HOST_MESSAGE.pan_len[0]),p_auth_tx->TLF01_details.card_num_len,10);
      memcpy( p_auth_tx->TLF01_details.card_num, tempstr+1,
              sizeof(p_auth_tx->TLF01_details.card_num) );
   }
   else
   {
      itoa(HOST_MESSAGE.pan_len[0],p_auth_tx->TLF01_details.card_num_len,10);
      memcpy( p_auth_tx->TLF01_details.card_num, tempstr,
              sizeof(p_auth_tx->TLF01_details.card_num) );
   }

   /* FIELD 3: PROCESSING CODE */
   bcd_to_asc (HOST_MESSAGE.proccode,   sizeof(HOST_MESSAGE.proccode),
               p_auth_tx->TLF01_details.processing_code,
               sizeof(p_auth_tx->TLF01_details.processing_code));


   /* FIELD 4: AMOUNT */
   bcd_to_asc (HOST_MESSAGE.amount_bin, sizeof(HOST_MESSAGE.amount_bin),
               p_auth_tx->TLF01_details.total_amount,
               sizeof(p_auth_tx->TLF01_details.total_amount));

   /* FIELD 6: CARDHOLDER BILLING AMOUNT */
   bcd_to_asc (HOST_MESSAGE.card_bill_amt,   sizeof(HOST_MESSAGE.card_bill_amt),
               p_auth_tx->ch_billing_amt, sizeof(p_auth_tx->ch_billing_amt));

   /* FIELD 7: GREENWICH TIME - MMDDhhmmss */
   bcd_to_asc (HOST_MESSAGE.gmt_time,   sizeof(HOST_MESSAGE.gmt_time),
               &p_auth_tx->TLF01_details.transmission_timestamp,
               sizeof(p_auth_tx->TLF01_details.transmission_timestamp));

   /* FIELD 10: CARDHOLDER BILLING CONVERSION RATE */
   bcd_to_asc (HOST_MESSAGE.conv_rate,   sizeof(HOST_MESSAGE.conv_rate),
               p_auth_tx->ch_billing_conv_rate,
               sizeof(p_auth_tx->ch_billing_conv_rate));

   /* FIELD 11: SYSTEMS TRACE AUDIT NUMBER */
   bcd_to_asc (HOST_MESSAGE.stan_bin, sizeof(HOST_MESSAGE.stan_bin),
               p_auth_tx->TLF01_details.sys_trace_audit_num,
               sizeof(p_auth_tx->TLF01_details.sys_trace_audit_num));

   /* FIELD 14: DATE EXPIRED */
   bcd_to_asc (HOST_MESSAGE.date_expd, sizeof(HOST_MESSAGE.date_expd),
               p_auth_tx->TLF01_details.exp_date,
               sizeof(p_auth_tx->TLF01_details.exp_date));

   /* FIELD 18: SIC NUMBER (MERCHANT TYPE) */
   bcd_to_asc (HOST_MESSAGE.sic_number, sizeof(HOST_MESSAGE.sic_number),
               p_auth_tx->TLF01_details.category_code,
               sizeof(p_auth_tx->TLF01_details.category_code));

   /* FIELD 19: COUNTRY CODE */
   memset( &tempstr, 0x00, sizeof(tempstr) );
   bcd_to_asc( HOST_MESSAGE.ctry_code,
               sizeof(HOST_MESSAGE.ctry_code),
               &tempstr[0],  sizeof(tempstr));
   memcpy(p_auth_tx->country_code, tempstr+1,sizeof(p_auth_tx->country_code)-1);

   /* Also store here for logging to TLF01. */
   memcpy( p_auth_tx->TLF01_details.num_dbsales, p_auth_tx->country_code, 3 );

   /* FIELD 20: PRIMARY ACCOUNT NUMBER EXTENDED COUNTRY CODE */
   memset(&tempstr, 0x00, sizeof(tempstr));
   bcd_to_asc (HOST_MESSAGE.pan_ctry_cd, sizeof(HOST_MESSAGE.pan_ctry_cd),
               &tempstr[0],  sizeof(tempstr));
   memcpy(p_auth_tx->pan_ext_ctry_code, tempstr+1,
          sizeof(p_auth_tx->pan_ext_ctry_code)-1);

   /* FIELD 22: POS ENTRY MODE  */
   bcd_to_asc (HOST_MESSAGE.posentry, sizeof(HOST_MESSAGE.posentry),
               p_auth_tx->TLF01_details.pos_entry_mode,
               sizeof(p_auth_tx->TLF01_details.pos_entry_mode));

   if ( (HOST_MESSAGE.posentry[0] == 0x05) ||
        (HOST_MESSAGE.posentry[0] == 0x95)  )
      emv_txn = true;

   /*+-----------------------------------------------------------------+
     | Need to convert field 22 to the Hypercom Standard.              |
     |                                                                 |
     |   Visa is close to the standard, except Visa swipe and filler.  |
     |       Visa swipe = 90   convert to   Standard = 02              |
     |                                                                 |
     |   For standard, filler should be at the beginning:              |
     |       Visa = xyz0    convert to    Standard = 0xyz              |
     +-----------------------------------------------------------------+*/
   if ( 0==strncmp("90", p_auth_tx->TLF01_details.pos_entry_mode, 2) )
      memcpy( p_auth_tx->TLF01_details.pos_entry_mode, "02", 2 );

   for( i=2; i>=0; i-- )
      p_auth_tx->TLF01_details.pos_entry_mode[i+1] =
          p_auth_tx->TLF01_details.pos_entry_mode[i];
   p_auth_tx->TLF01_details.pos_entry_mode[0] = '0';
   
 /* FIELD 24: NII (Round Trip Indicator, RTI, from Visa's header */

   /* First byte is the RTI.
    * Second byte, if 'E' indicates to echo this in response.
    */
   memcpy( p_auth_tx->TLF01_details.nii, HOST_MESSAGE.nii, 2 );

   /* FIELD 25: POS CONDITION CODE */
   bcd_to_asc (HOST_MESSAGE.pos_condcd, sizeof(HOST_MESSAGE.pos_condcd),
               p_auth_tx->TLF01_details.pos_condition_code,
               sizeof(p_auth_tx->TLF01_details.pos_condition_code));

   /* FIELD 32: ACQUIRING INSTITUTION ID CODE  */
   memset(&tempstr, 0x00, sizeof(tempstr));
   bcd_to_asc (HOST_MESSAGE.acq_inst_id, (HOST_MESSAGE.acq_inst_len[0]+1)/2,
				  &tempstr[0], sizeof(tempstr));
   if (1 == HOST_MESSAGE.acq_inst_len[0] % 2)
   {
      memcpy(p_auth_tx->acq_inst_id_cd, tempstr+1,
             sizeof(p_auth_tx->acq_inst_id_cd)-1);
   }
   else
   {
      memcpy(p_auth_tx->acq_inst_id_cd, tempstr,
             sizeof(p_auth_tx->acq_inst_id_cd)-1);
   }

   /* 
    This statement is required : because to insert into TLF01, we need to copy
	acquirer institution code into TLF01 structure variable.
	*/
   strcpy(p_auth_tx->TLF01_details.acquiring_id, p_auth_tx->acq_inst_id_cd);

   p_auth_tx->acq_inst_id_len[0] =  HOST_MESSAGE.acq_inst_len[0];

   
   /* FIELD 33: FORWARDING INSTITUTION ID CODE  */
   memset(&tempstr, 0x00, sizeof(tempstr));
   bcd_to_asc (HOST_MESSAGE.fwd_inst_id, (HOST_MESSAGE.fwd_inst_len[0]+1)/2,
				  &tempstr[0], sizeof(tempstr));
   if (1 == HOST_MESSAGE.fwd_inst_len[0] % 2)
   {
      memcpy(p_auth_tx->forwarding_inst_id_cd, tempstr+1,
             sizeof(p_auth_tx->forwarding_inst_id_cd)-1);
   }
   else
   {
      memcpy(p_auth_tx->forwarding_inst_id_cd, tempstr,
             sizeof(p_auth_tx->forwarding_inst_id_cd)-1);
   }
   p_auth_tx->forwarding_inst_id_len[0] =  HOST_MESSAGE.fwd_inst_len[0];

   /* Store here for TLF01. */
   strcpy( p_auth_tx->TLF01_details.forwarding_institution_id,
           p_auth_tx->forwarding_inst_id_cd );

   /* FIELD 34: PRIMARY ACCOUNT NUMBER EXTENDED */
   /* When Cadencie responds back DE34, will echo it back to VISA */

   /* FIELD 35: TRACK 2 */
   if ( HOST_MESSAGE.track2_len[0] != 0x00 )
   {
      bcd_to_asc (HOST_MESSAGE.track2, (HOST_MESSAGE.track2_len[0]+1)/2,temp,
                  sizeof(p_auth_tx->TLF01_details.track2));

      if(temp[0]== '0')
      {
         strcpy(p_auth_tx->TLF01_details.track2,&temp[1]);
      }
      else
      {
         strcpy(p_auth_tx->TLF01_details.track2,temp);
      }

      memset(p_auth_tx->TLF01_details.exp_date, 0 ,
             sizeof(p_auth_tx->TLF01_details.exp_date));

      memset(p_auth_tx->TLF01_details.card_num, 0 ,
             sizeof(p_auth_tx->TLF01_details.card_num));

      if ( false == parse_track2( p_auth_tx, error_msg ) )
      {
         memcpy( p_auth_tx->TLF01_details.terminal_id, HOST_MESSAGE.termid,
                 sizeof(HOST_MESSAGE.termid));
         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
         		p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
         strcat(error_msg,Log_Bufr);
         ncvis2_log_message( 3, 2, error_msg , "move_iso_response_to_auth_struct" , 0 ,p_auth_tx->TLF01_details.card_num);
         return(false);
      }
   }

   /* FIELD 37: RETRIEVAL REFERENCE NUMBER */
   memcpy( p_auth_tx->TLF01_details.retrieval_ref_num, HOST_MESSAGE.rrn,
           sizeof(HOST_MESSAGE.rrn));

   /* FIELD 38: AUTH NUMBER */
   memcpy( p_auth_tx->TLF01_details.auth_number, HOST_MESSAGE.authcode,
           sizeof(HOST_MESSAGE.authcode));

   /* FIELD 39: RESPONSE CODE */
   memcpy( p_auth_tx->TLF01_details.response_code,
           HOST_MESSAGE.responsecd,
           sizeof(HOST_MESSAGE.responsecd) );

   if ( 0 == strcmp(p_auth_tx->TLF01_details.response_code,"Q1") )
      strcpy( p_auth_tx->TLF01_details.response_code, CALL_ISSUER );

   /* FIELD 41: TERMINAL ID */
   memcpy( p_auth_tx->TLF01_details.terminal_id, HOST_MESSAGE.termid,
           sizeof(HOST_MESSAGE.termid));


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

   /* FIELD 44: ADDITIONAL RESPONSE DATA */
   if ( HOST_MESSAGE.addl_rsp_len[0] > 0 )
   {
      if ( HOST_MESSAGE.addl_rsp_len[0] > 25)
         HOST_MESSAGE.addl_rsp_len[0] = 25;

     // memcpy( &p_auth_tx->TLF01_details.resp_source,
     //         &HOST_MESSAGE.addlrspdata,
     //          HOST_MESSAGE.addl_rsp_len[0] );
	  /*strncpy(&p_auth_tx->TLF01_details.resp_source[0],&HOST_MESSAGE.addl_rsp_len[0],1)	;	*/
	  memcpy( &p_auth_tx->TLF01_details.resp_source[0],
             &HOST_MESSAGE.addlrspdata,
               HOST_MESSAGE.addl_rsp_len[0] );

	 strncpy(&p_auth_tx->TLF01_details.product_codes[12].quantity[0], &HOST_MESSAGE.addl_rsp_len[0],1);

      /* Get the CAVV Result Code */
      memset( cavv_result,0x00, sizeof(cavv_result) );
      find_cavv_flag( HOST_MESSAGE.addl_rsp_len[0],
                      HOST_MESSAGE.addlrspdata,
                      cavv_result );

      if ( cavv_result[0] != 0x00 )
      {
         p_auth_tx->TLF01_details.cvc = cavv_result[0];

         if ( cavv_result[0] != '2' )
         {
            /* Log invalid result flag, then continue on. */
            get_cavv_result_error( cavv_result[0], tempstr );
            memset(Log_Bufr,0x00,sizeof(Log_Bufr));
            sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
            		p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
            strcat(Buffer,Log_Bufr);
            ncvis2_log_message( 2, 2, Buffer , "move_iso_response_to_auth_struct" , 0 ,p_auth_tx->TLF01_details.card_num);
         }
      }

      /* Get the Card Authentication Result Code */
      memset( carc_result, 0x00, sizeof(carc_result) );
      find_carc_flag( HOST_MESSAGE.addl_rsp_len[0],
                      HOST_MESSAGE.addlrspdata,
                      carc_result );

      if ( (carc_result[0] == '1') || (carc_result[0] == '2') )
      {
         emv_txn = true;
         if ( carc_result[0] == '1' )
         {
            strcpy( error_msg, "ARQC checked, but failed" );
            memset(Log_Bufr,0x00,sizeof(Log_Bufr));
            sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
            		p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
            strcat(error_msg,Log_Bufr);
            ncvis2_log_message( 3, 2, error_msg , "move_iso_response_to_auth_struct" , 0 ,p_auth_tx->TLF01_details.card_num);
         }
      }

      /* Get the CVV2 Result Code */
      memset( cvv2_result, 0x00, sizeof(cvv2_result) );
      find_cvv2_result_code( HOST_MESSAGE.addl_rsp_len[0],
                             HOST_MESSAGE.addlrspdata,
                             cvv2_result );

      p_auth_tx->TLF01_details.product_codes[17].quantity[0] = cvv2_result[0];
      if ( cvv2_result[0] == 'N' )
      {
         strcpy( error_msg, "CVV2 No Match" );
         strcpy( p_auth_tx->TLF01_details.response_text, error_msg );
         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
         		p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
         strcat(error_msg,Log_Bufr);
         ncvis2_log_message( 3, 2, error_msg , "move_iso_response_to_auth_struct" , 0 ,p_auth_tx->TLF01_details.card_num);
      }
   }

   /* FIELD 45: TRACK1 */
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

   /* FIELD 48: ADDITIONAL DATA */
   if ( HOST_MESSAGE.addl_data_len[0] != 0x00 )
   {
      len = HOST_MESSAGE.addl_data_len[0];
      if ( 0 == strcmp("0312",p_auth_tx->TLF01_details.message_type) )
      {
         /* Currently this field is only used for 0312 declines. */
         bcd_to_asc( HOST_MESSAGE.addl_data, len,
                     p_auth_tx->TLF01_details.resp_source,
                     len*2 );
      }
      else if ( p_auth_tx->TLF01_details.message_type[2] == '1' )
      {
         /* This is a response. */
         if ( 0 != strcmp(p_auth_tx->TLF01_details.response_code,APPROVED) )
         {
            /* DE48 gives reason for declines. */
            memset( tempstr, 0x00, sizeof(tempstr) );
            for( i=0; i<len; i++ )
            {
               tempstr[i] = ascii_ebcdic_conv( HOST_MESSAGE.addl_data[i], E2A );
            }
            memcpy( p_auth_tx->TLF01_details.response_text,
                    tempstr,
                    sizeof(p_auth_tx->TLF01_details.response_text)-1 );

            sprintf( Buffer, "Declined (Visa) - DE48: %s", tempstr );
            memset(Log_Bufr,0x00,sizeof(Log_Bufr));
            sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
            		p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
            strcat(Buffer,Log_Bufr);
            ncvis2_log_message( 3, 2, Buffer , "move_iso_response_to_auth_struct" , 0 ,p_auth_tx->TLF01_details.card_num);
         }
      }
   }

   /* FIELD 49 TRANSACTION CURRENCY CODE */
   memset(&tempstr, 0x00, sizeof(tempstr));
   bcd_to_asc (HOST_MESSAGE.tran_curr_cd,   sizeof(HOST_MESSAGE.tran_curr_cd),
               tempstr, sizeof(p_auth_tx->TLF01_details.currency_code));   
   memcpy( p_auth_tx->TLF01_details.currency_code, tempstr+1,
           sizeof(p_auth_tx->TLF01_details.currency_code)-1 );

   /* FIELD 51 CARDHOLDER BILLING CURRENCY CODE */
   memset(&tempstr, 0x00, sizeof(tempstr));
   bcd_to_asc (HOST_MESSAGE.card_curr_cd,   sizeof(HOST_MESSAGE.card_curr_cd),
               tempstr, sizeof(p_auth_tx->ch_billing_curr_code));
   memcpy( p_auth_tx->ch_billing_curr_code, tempstr+1,
           sizeof(p_auth_tx->ch_billing_curr_code)-1 );

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

   /* FIELD 54 ADDITIONAL AMOUNTS *
    * ---------------------------
    * This field contains current and/or available balance in one or
    * two currencies.  The two currencies are the issuer's currency
    * and the acquirer's currency.  There can be up to 4 sets of balances
    * here:  current and available for Issuer and Acquirer.  Acquirer
    * currency, which is what we want, is always the 3rd or the 3rd and
    * 4th set.
    *
    * If there is 1 set, it is in the Issuer's currency.  In this case,
    * just copy it into the auth_tx values, cause it is also our currency.
    *
    * If there are 2 sets, the Issuer has sent current and available in
    * the Issuer's currency.  Again, just copy these into auth_tx, because
    * if the Issuer sends only one currency, Issuer and Acquirer currency
    * must be the same.
    *
    * If there are 3 sets, copy the 3rd set (Acquirer) into auth_tx.
    *
    * If there are 4 sets, copy the 3rd and 4th (both Acquirer currencies)
    * into auth_tx.  Need to distinguish between current and available
    * balance.
    *
    * Current Balance:   For credit accounts, this is the current account
    *                    ledger balance and refers to the amount of credit
    *                    remaining to the customer.
    *
    * Available Balance: This is typically ledger balance less outstanding
    *                    authorizations.
    */

   /* Only get this data for responses. */
   len = HOST_MESSAGE.addl_len[0];
   if ( (len > 0) && (p_auth_tx->TLF01_details.message_type[2] == '1') )
   {
      memset( tempstr, 0x00, sizeof(tempstr) );
      memcpy( tempstr, HOST_MESSAGE.addlamts, len );

      addl_amts_size = sizeof(Addl_Amts);
      bal_set = 0;

      /* Copy balance sets into structure that temporarily holds balances. */
      i = 0;
      while( i < len )
      {
         /* Initialize the structure that will hold all four sets. */
         memset( &acct_bal[bal_set], 0x00, addl_amts_size );

         memcpy( acct_bal[bal_set].acct_type, tempstr+i,   2 );
         memcpy( acct_bal[bal_set].amt_type,  tempstr+i+2, 2 );
         memcpy( acct_bal[bal_set].cur_code,  tempstr+i+4, 3 );
         acct_bal[bal_set].sign = tempstr[i+7];
         memcpy( acct_bal[bal_set].amount,    tempstr+i+8, 12);

         bal_set++;
         i += 20;   /* Increase ptr in tempstr by length of 1 set. */
      }

      /* Find the correct balance sets and copy them into auth_tx. */
      switch( bal_set )
      {
         case 1:  (void)get_balances( &acct_bal[0], p_auth_tx );
                  break;

         case 2:  (void)get_balances( &acct_bal[0], p_auth_tx );
                  (void)get_balances( &acct_bal[1], p_auth_tx );
                  break;

         case 3:  (void)get_balances( &acct_bal[2], p_auth_tx );
                  break;

         case 4:  (void)get_balances( &acct_bal[2], p_auth_tx );
                  (void)get_balances( &acct_bal[3], p_auth_tx );
                  break;
      }
   }
   else
   {
      memset(&tempstr, 0x00, sizeof(tempstr));
      memset(&templen, 0x00, sizeof(templen));
      memcpy(templen,HOST_MESSAGE.addl_len,bf54_len);
      memcpy(tempstr,HOST_MESSAGE.addlamts,sizeof(HOST_MESSAGE.addlamts));
      i = 0;
      /* the loop is controlled by the ifs at the end of the loop */
      while (i < 21 && templen[0] > 19)
      {
         if (0 == strncmp(tempstr+i+2,"01",2))   /* indicates current balance */
         {
            memcpy(p_auth_tx->ACF01_details.cash_available, tempstr+i+11, 9);
            if ('D' == tempstr[i+7])
            {
               p_auth_tx->current_balance_sign = '-';
            }
            else if ('C' == tempstr[i+7])
            {
               p_auth_tx->current_balance_sign = '+';
            }
            strncpy(p_auth_tx->TLF01_details.currency_code, tempstr+i+4, 3);    
         }
         else if (0 == strncmp(tempstr+i+2,"02",2)) /*indicates available balance*/
         {
            memcpy(p_auth_tx->ACF01_details.cash_limit, tempstr+i+11,9);
            if ('D' == tempstr[i+7])
            {
               p_auth_tx->available_balance_sign = '-';
            }
            else if ('C' == tempstr[i+7])
            {
               p_auth_tx->available_balance_sign = '+';
            }
            strncpy(p_auth_tx->TLF01_details.currency_code, tempstr+i+4, 3);    
         }
         if (templen[0] > 20)
            i = i + 20;  /* This will allow us to get the second set of data */
         else
            i = 40;      /* This will cause the loop to end */
      }
   }

   	/*  FIELD 55 EMV/CHIP/ data */
	if ( HOST_MESSAGE.ICCdata_len[0] > 0 )
	{	CHAR  templen [6] = {0} ;INT temp_len=0;	 
	   BYTE  ICCdata [256] = {0};
	  temp_len = HOST_MESSAGE.ICCdata_len[0];
	  memcpy(p_auth_tx->EMV_details.emv_block,HOST_MESSAGE.ICCdata_len,1);
	  if ( temp_len > 0) 
	  {     memset(ICCdata, 0, sizeof(ICCdata)); 
	  		memcpy(ICCdata, HOST_MESSAGE.ICCdata, temp_len);    
			memcpy( p_auth_tx->EMV_details.emv_block+1,HOST_MESSAGE.ICCdata,temp_len);
			genutil_VISA_EMV_To_Auth_Tx( ICCdata, temp_len, p_auth_tx);   
	  }
	}

   /* FIELD 60 RESERVED */
   if ( HOST_MESSAGE.priv60_len[0] > 0 )
   {
      memset(&tempstr, 0x00, sizeof(tempstr));
      memset(&templen, 0x00, sizeof(templen));
      bcd_to_asc( HOST_MESSAGE.priv60_len, sizeof(HOST_MESSAGE.priv60_len),
                 &templen[0], sizeof(templen));

      bcd_to_asc( HOST_MESSAGE.priv60, (atoi(templen)),
                  (&tempstr[0]), sizeof(tempstr));

      memcpy( p_auth_tx->additional_pos_information, tempstr,
              sizeof(p_auth_tx->additional_pos_information)-1);

      /* Copy into here so this information is logged into TLF01 (SCR 743). */
      memcpy( p_auth_tx->TLF01_details.nfi_name, tempstr,
              sizeof(p_auth_tx->TLF01_details.nfi_name)-1 );
   }

   /* FIELD 61: OTHER AMOUNTS */
   memset(&tempstr, 0x00, sizeof(tempstr));
   memset(&templen, 0x00, sizeof(templen));
	bcd_to_asc (HOST_MESSAGE.priv61_len, sizeof(HOST_MESSAGE.priv61_len),
				  &templen[0], sizeof(templen));
   bcd_to_asc (HOST_MESSAGE.priv61, (atoi(templen)),
				  (&tempstr[0]), sizeof(tempstr));
   memcpy(p_auth_tx->TLF01_details.add_amounts, tempstr,
          sizeof(p_auth_tx->TLF01_details.add_amounts)-1);

   if ( 0 == strcmp(p_auth_tx->TLF01_details.message_type, "0102") )
   {
      /* This is an ATM Confirmation message.
       * DE61, subfield 1 (positions 1-12) is the dispensed amount.
       */
      memcpy( p_auth_tx->TLF01_details.actual_amount, tempstr, 12 );
   }

   /* FIELD 62 CUSTOM PAYMENT SERVICE FIELDS - BITMAP */
   /* Store this for echoing. */
   memcpy(p_auth_tx->int_net_facility_data, HOST_MESSAGE.priv62, 
          sizeof(HOST_MESSAGE.priv62 ));

   memcpy(p_auth_tx->int_net_facility_len, HOST_MESSAGE.priv62_len, 
          sizeof(HOST_MESSAGE.priv62_len ));
   if (HOST_MESSAGE.priv62_len[0] > 0)  
   {
      Field62Response( p_auth_tx );
      if ( false == isalpha(p_auth_tx->TLF01_details.visa_char_indicator) )
         p_auth_tx->TLF01_details.visa_char_indicator = ' ';

      if ( p_auth_tx->TLF01_details.message_type[2] == '0' )
      {
         /* This is a request, so store 62.1 and 62.2 into the following fields
          * so they get stored into NCF30 for reversals.
          * This code was added late.  Since there are no fields in NCF30 large
          * enough to hold DE62, use a combination of network data and invoice
          * number.  Network data will hold 62.1 (1 byte) and the first 11 bytes
          * of 62.2 (15 bytes). The last 4 bytes of 62.2 go into invoice number.
          */
         p_auth_tx->TLF01_details.mcard_banknet[0] =
                                   p_auth_tx->TLF01_details.visa_char_indicator;

         memcpy( p_auth_tx->TLF01_details.mcard_banknet+1,
                 p_auth_tx->TLF01_details.visa_tran_id,
                 11);

         memcpy( p_auth_tx->TLF01_details.invoice_number,
                 p_auth_tx->TLF01_details.visa_tran_id+11,
                 4 );
      }
   }
   else
      p_auth_tx->TLF01_details.visa_char_indicator = ' ';

   /* FIELD 63: V.I.P. PRIVATE USE FIELDS */
   if ( HOST_MESSAGE.priv63_len[0] > 0 )
   {
      /* Do this for switched in to echo and log the values.
       * Do for switched out to log the values.
       */
      ParseField63( HOST_MESSAGE.priv63_len, p_auth_tx );

      /* Set this bit to indicate on responses that field 63 does exist. */
      p_auth_tx->TLF01_details.product_codes[10].quantity[0] = 0x01;

      if ( p_auth_tx->TLF01_details.product_codes[12].code[0] != 0x00 )
      {
         /* STIP sent us an advice code. Log this for customer. */
         LogStipReasonCode( p_auth_tx );
      }
   }

   /* FIELD 70: NETWORK MANAGEMENT INFORMATION CODE */
   bcd_to_asc (HOST_MESSAGE.nmi_code,   sizeof(HOST_MESSAGE.nmi_code),
               nmi_code, sizeof(nmi_code));
   memcpy (p_auth_tx->function_code, nmi_code, sizeof(nmi_code));

   /* FIELD 73: DATE, ACTION */
   if ( HOST_MESSAGE.action[0] != 0x00 )
   {
      bcd_to_asc (HOST_MESSAGE.action, sizeof(HOST_MESSAGE.action),
                  p_auth_tx->TLF01_details.product_codes[0].amount,
                  sizeof(p_auth_tx->TLF01_details.product_codes[0].amount));
   }

   /* FIELD 90 ORIGINAL DATA ELEMENTS */
   if ( !blank_or_null_string(HOST_MESSAGE.orig_data,5) )
   {
      /* Field 90 is formatted in BCD as:
       *    Orig Msg:  2 bytes
       *    Orig Stan: 3 bytes
       *    Orig DE7:  5 bytes
       *    DE32/33:  11 bytes
       * for a total of 21 BCD bytes.  Above, we checked the first 5 bytes,
       * Msg and Stan, to see if they exist. If yes, then DE90 is present.
       * So now convert to 42 bytes ASCII and check to make sure Orig Msg
       * contains a non-zero value. To be valid, the 2nd char must be non-zero.
       */
      memset( tempstr, 0x00, sizeof(tempstr) );
      bcd_to_asc (HOST_MESSAGE.orig_data,   sizeof(HOST_MESSAGE.orig_data),
                  tempstr, sizeof(tempstr));
      if((tempstr[1] == '0') && (p_auth_tx->TLF01_details.message_type[1]=='4'))
      {
         sprintf(Buffer, "Invalid data for original data elements");
         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
         		p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
         strcat(Buffer,Log_Bufr);
         ncvis2_log_message( 3, 2, Buffer , "move_iso_response_to_auth_struct" , 0 ,p_auth_tx->TLF01_details.card_num);
         return false;
      }
      else
      {
         memcpy(p_auth_tx->TLF01_details.orig_message, tempstr, 4);
         memcpy(p_auth_tx->TLF01_details.orig_sys_trace_num, tempstr+4, 6);
         memcpy(p_auth_tx->orig_local_time_date, tempstr+10,10);
         memcpy(p_auth_tx->orig_acq_inst_id,tempstr+20,11);

         /* Actually forwarding institution id code */
         memcpy(p_auth_tx->amex_pos_entry_mode,tempstr+31,11);

         /* Since all values after message type in DE90 can be zero,
          * check the stan.  If zero, will need to use DE11 for reversals
          * to look up original transaction.
          */
         if ( 0 == strcmp(p_auth_tx->TLF01_details.orig_sys_trace_num,"000000"))
         {
            memcpy( p_auth_tx->TLF01_details.orig_sys_trace_num,
                    p_auth_tx->TLF01_details.sys_trace_audit_num, 6 );
         }
      }
   }

   /* FIELD 91: FILE UPDATE CODE */
   if ( HOST_MESSAGE.file_update[0] != 0x00 )
   {
      p_auth_tx->TLF01_details.processing_code[0] = HOST_MESSAGE.file_update[0];
   }

   /* FIELD 95 REPLACEMENT AMOUNTS */
   if ( p_auth_tx->TLF01_details.actual_amount[0] == 0x00 )
   {
      memcpy( p_auth_tx->TLF01_details.actual_amount, HOST_MESSAGE.repl_amts,
              sizeof(p_auth_tx->TLF01_details.actual_amount)-1);

      /* Set flag here so we know to either echo or not echo this field. */
      if ( HOST_MESSAGE.repl_amts[0] != 0x00 )
      {
         p_auth_tx->MCF01_details.totals_ind[0] = 'E';
         memcpy( p_auth_tx->MCF01_details.address01,
                 HOST_MESSAGE.repl_amts,   30 );
         memcpy( p_auth_tx->MCF01_details.address02,
                 HOST_MESSAGE.repl_amts+30,12 );
      }
   }

   /* FIELD 96: Not Used by Visa (Hdr Fld 9, Msg Status Flags, from Visa hdr. */

   /* First 3 bytes are the Message Status Flags.
    * Fourth byte, if 'E' indicates to echo this in response.
    * Use the MCF01 field because for switched in txns, MCF01 is not used.
    */
   memcpy( p_auth_tx->MCF01_details.class_code, HOST_MESSAGE.msgsec, 4 );

   /* FIELD 101: FILE NAME */
   if ( HOST_MESSAGE.fname_len[0] != 0x00 )
   {
      len = HOST_MESSAGE.fname_len[0];
      memcpy( p_auth_tx->TLF01_details.kek,
              HOST_MESSAGE.filename, len );
   }

   /* FIELD 102 ACCOUNT ID #1 */
   if ( HOST_MESSAGE.acct_id_1[0] != 0x00 )
   {
      memcpy( p_auth_tx->acct_id_1, HOST_MESSAGE.acct_id_1,
              sizeof(HOST_MESSAGE.acct_id_1));

      /* Set flag indicating this field came in a request.
       * At this point, we don't know if this is a request
       * or response, but that is ok.  Use FAF01 because
       * EB is not using the Fleet functionality at all.
       */
      p_auth_tx->FAF01_details.velocity[0].amount[0] = '1';
   }

   /* FIELD 103 ACCOUNT ID #2 */
   if ( HOST_MESSAGE.acct_id_2[0] != 0x00 )
   {
      memcpy( p_auth_tx->acct_id_2, HOST_MESSAGE.acct_id_2,
              sizeof(HOST_MESSAGE.acct_id_2));

      /* Set flag indicating this field came in a request.
       * At this point, we don't know if this is a request
       * or response, but that is ok.  Use FAF01 because
       * EB is not using the Fleet functionality at all.
       */
      p_auth_tx->FAF01_details.velocity[0].amount[1] = '1';
   }

   /* FIELD 116: CARD ISSUER REFERENCE DATA */
   if ( HOST_MESSAGE.issref_len[0] > 0 )
   {
      /* Data is present. Format it into ASCII and log into TLF01. */
      Field116_Handler( HOST_MESSAGE.issref_len, p_auth_tx );
   }

   /* FIELD 121 ISSUING INSTITUTION ID CODE */
   memcpy( p_auth_tx->iss_inst_id_cd, HOST_MESSAGE.iss_inst_id,
           sizeof(HOST_MESSAGE.iss_inst_id));

   /* FIELD 125 SUPPORT INFORMATION */
   memcpy(p_auth_tx->TLF01_details.voice_auth_text,HOST_MESSAGE.sup_info,
	      sizeof(HOST_MESSAGE.sup_info));

   /* FIELD 126 PRIVATE USE FIELDS */
   if ( HOST_MESSAGE.de126_len[0] > 0 )
   {
      /* Store this information in voice_auth_text.  It is the only field
       * large enough to hold it.  No need to add any fields to TLF01.
       * EB just wants this information logged for now - no processing on it
       * except for CVV2.
       */
      memset( cvv2_data, 0x00, sizeof(cvv2_data) );
      Field126Parser( p_auth_tx, cvv2_data );

      if ( cvv2_data[0] != 0x00 )
      {
         /* CVV2 Data is made up of 3 parts:
          * 1. Presence Indicator: 0 = CVV2 bypassed or not provided
          *                        1 = CVV2 is present
          *                        2 = CVV2 on card but illegible
          *                        9 = CH states no CVV2 on card
          *
          * 2. Response Type:      0 = Send only normal RC in field 39
          *                        1 = Send RC and CVV2 Result
          *
          * 3. CVV2 Value:         Right justified, blank filled
          */
         if ( true == blank_or_null_string(&cvv2_data[CVV2_VALUE], 4) )
         {
            /* CVV2 value is not present. */
            if ( (cvv2_data[CVV2_PRESENCE] == '2') ||
                 (cvv2_data[CVV2_PRESENCE] == '9')  )
            {
               /* Decline the transaction. */
               if ( cvv2_data[CVV2_PRESENCE] == '2' )
               {
                  strcpy( Buffer, "CVV2 Value is illegible on the card.");
                  p_auth_tx->TLF01_details.product_codes[17].quantity[0] = 'P';
               }
               else
               {
                  strcpy( Buffer, "Cardholder states no CVV2 on card." );
                  p_auth_tx->TLF01_details.product_codes[17].quantity[0] = 'S';
               }

               if ( cvv2_data[CVV2_RESPONSE] == '1' )
               {
                  /* Do these steps to make sure CVV2 Result Code is in resp. */
                  p_auth_tx->TLF01_details.product_codes[17].quantity[1] = '.';
                  p_auth_tx->TLF01_details.verify_mag_stripe[0] = 'Y';
                  p_auth_tx->TLF01_details.cvc = 'N';
                  strcpy( p_auth_tx->TLF01_details.security_response_code, "02" );
               }

               memset(Log_Bufr,0x00,sizeof(Log_Bufr));
               sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
               		p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
               strcat(Buffer,Log_Bufr);
               ncvis2_log_message( 3, 2, Buffer , "move_iso_response_to_auth_struct" , 0 ,p_auth_tx->TLF01_details.card_num);
               retval = false;
            }
            else if ( cvv2_data[CVV2_PRESENCE] == '0' )
            {
               if ( cvv2_data[CVV2_RESPONSE] == '1' )
               {
                  /* Do these steps to make sure CVV2 Result Code is in resp. */
                  p_auth_tx->TLF01_details.product_codes[17].quantity[1] = '.';
                  p_auth_tx->TLF01_details.verify_mag_stripe[0] = 'Y';
                  strcpy( p_auth_tx->TLF01_details.security_response_code, "01" );
               }
            }
         }
         else
         {
            /* Copy CVV2 into Auth Tx */
            if ( cvv2_data[CVV2_VALUE] == ' ' )
               strcpy(p_auth_tx->TLF01_details.cvc_data,&cvv2_data[CVV2_VALUE+1]);
            else
               strcpy(p_auth_tx->TLF01_details.interest_rate,&cvv2_data[CVV2_VALUE]);

            if ( cvv2_data[CVV2_RESPONSE] == '1' )
            {
               /* Set indicator for DE44.10 in response (CVV2 Result). */
               p_auth_tx->TLF01_details.product_codes[17].quantity[0] = ' ';
               p_auth_tx->TLF01_details.product_codes[17].quantity[1] = '.';
            }
         }
      }
   }

   /* FIELD 127: FILE MAINTENANCE (FORMAT 2) */
   if ( HOST_MESSAGE.file_rec_len[0] != 0x00 )
   {
      /* Subfield E.1 = Action Code */
      p_auth_tx->TLF01_details.type_of_data[0]
                      = ascii_ebcdic_conv( HOST_MESSAGE.file_rec[0], E2A );

      p_auth_tx->TLF01_details.type_of_data[1]
                      = ascii_ebcdic_conv( HOST_MESSAGE.file_rec[1], E2A );

      /* Subfield E.2 = Region Coding */
      for( i=0; i<9; i++ )
      {
         p_auth_tx->TLF01_details.product_codes[1].amount[i]
                      = ascii_ebcdic_conv( HOST_MESSAGE.file_rec[i+2], E2A );
      }
   }

   /* Store the original currency code and amount
    * before converting them (if required)
    */
   memcpy(p_auth_tx->reconciliation_cur_code,
          p_auth_tx->TLF01_details.currency_code,
          sizeof(p_auth_tx->TLF01_details.currency_code));
   memcpy( p_auth_tx->reconciliation_amount,
          p_auth_tx->TLF01_details.total_amount,
          sizeof(p_auth_tx->TLF01_details.total_amount));
   memcpy(p_auth_tx->TLF01_details.orig_amount,
          p_auth_tx->TLF01_details.actual_amount,
          sizeof(p_auth_tx->TLF01_details.actual_amount));

   /* Save the partial reversal amount into a field that does
    * not get modified.
    */
   memcpy( p_auth_tx->TLF01_details.refund_amount,
           p_auth_tx->TLF01_details.actual_amount, 12 );

   /*+------------------------------------------------------------------+
     | Here is a requirement unique to Equitable:                       |
     | ------------------------------------------                       |
     |                                                                  |
     |   If field 49 is "608" (PESO), use field 4 for the amount field, |
     |   and use the PESO Balance Account for authorization purposes.   |
     |                                                                  |
     |   If field 49 is not "608", use field 6 for the amount field, it |
     |   will be in US Dollars.                                         |
     |                                                                  |
     | This is for switched in, or Issuing, transactions only.          |
     |                                                                  |
     |                                                                  |
     | Requirements for Logging to TLF01:                               |
     | ----------------------------------                               |
     | 1.  If Currency Code (cc) is Peso                                |
     |     1.1  DE4  --> total_amount                                   |
     |     1.2  DE6  --> sales_amount  (not used, just logged)          |
     |     1.3  DE49 --> currency_code                                  |
     |     1.4  DE51 --> nfi_seq_nbr (not used, just logged)            |
     |                                                                  |
     | 2.  Else if cc is USD                                            |
     |     2.1  DE4  --> total_amount                                   |
     |     2.2  DE4  --> sales_amount (same value as DE6)               |
     |     2.3  DE49 --> currency_code                                  |
     |     2.4  DE49 --> nfi_seq_nbr (not used, just logged)            |
     |                                                                  |
     | 3.  Else (cc is other)                                           |
     |     3.1  DE4  --> sales_amount  (not used, just logged)          |
     |     3.2  DE6  --> total_amount                                   |
     |     3.3  DE49 --> nfi_seq_nbr (not used, just logged)            |
     |     3.4  DE51 --> currency_code                                  |
     |                                                                  |
     +------------------------------------------------------------------+*/
   if ((0 == strncmp(p_auth_tx->TLF01_details.message_type, "0100", 4)) ||
   (0 == strncmp(p_auth_tx->TLF01_details.message_type, "0101", 4)) ||
   (0 == strncmp(p_auth_tx->TLF01_details.message_type, "0120", 4)) ||
   (0 == strncmp(p_auth_tx->TLF01_details.message_type, "0400", 4)) ||
   (0 == strncmp(p_auth_tx->TLF01_details.message_type, "0401", 4)) ||
   (0 == strncmp(p_auth_tx->TLF01_details.message_type, "0420", 4)))
   {
      /* A currency code of Pesos */
      if ( 0 != strcmp( PESO, p_auth_tx->TLF01_details.currency_code ) )
      {
         /* or dollars is allowed. */
         if ( 0 != strcmp( USD, p_auth_tx->TLF01_details.currency_code ) )
         { 
            /* else use ch billing's currency code */
            if ( 0 == strcmp( USD, p_auth_tx->ch_billing_curr_code ) )
            {
               /* Currency Code is neither Peso nor USD.
                * Store according to comment box above.
                */

               /* Copy DE4 to sales_amount.  */
               memcpy( p_auth_tx->TLF01_details.sales_amount,
                       p_auth_tx->TLF01_details.total_amount,
                       12 );

               /* Copy DE6 to total_amount. */
               memcpy( p_auth_tx->TLF01_details.total_amount,
                       p_auth_tx->ch_billing_amt,
                       12 );

               /* Copy DE49 to nfi_seq_nbr. */
               memcpy( p_auth_tx->TLF01_details.nfi_seq_nbr,
                       p_auth_tx->TLF01_details.currency_code,
                       3 );

               /* Copy DE51 to currency_code. */
               memcpy( p_auth_tx->TLF01_details.currency_code,
                       p_auth_tx->ch_billing_curr_code,
                       3 );

               memcpy (p_auth_tx->TLF01_details.actual_amount,
                       p_auth_tx->TLF01_details.add_amounts+24,
                       sizeof(p_auth_tx->TLF01_details.actual_amount));
            }
            else
            {
               if ( 0==strncmp(p_auth_tx->TLF01_details.processing_code,"30",2))
               {
                  /* Balance Inquiry can have a 3rd currency code.  But must
                   * use USD account.  Need to save original currency code
                   * to echo back out.
                   */
                  memcpy( p_auth_tx->MCF01_details.region_nbr,
                          p_auth_tx->TLF01_details.currency_code,
                          3 );
                  memcpy( p_auth_tx->TLF01_details.currency_code, USD, 3 );
               }
               else
               {
                  /* This is used to determine if the currency code is ok. */
                  memcpy(p_auth_tx->TLF01_details.response_code, "91", 2);
               }
            }
         }
         else
         {
            /* Currency Code is USD.  Store according to comment box above. */

            /* DE4  is already in total_amount.  */
            /* DE49 is already in currency_code. */

            /* Copy DE4 to sales_amount. */
            memcpy( p_auth_tx->TLF01_details.sales_amount,
                    p_auth_tx->TLF01_details.total_amount,
                    12 );

            /* Copy DE49 to nfi_seq_nbr. */
            memcpy( p_auth_tx->TLF01_details.nfi_seq_nbr,
                    p_auth_tx->TLF01_details.currency_code,
                    3 );
         }
      }
      else
      {
         /* Currency Code is PESO.  Store according to comment box above. */

         /* DE4  is already in total_amount.  */
         /* DE49 is already in currency_code. */

         /* Copy DE6 to sales_amount. */
         memcpy( p_auth_tx->TLF01_details.sales_amount,
                 p_auth_tx->ch_billing_amt,
                 12 );

         /* Copy DE51 to nfi_seq_nbr. */
         memcpy( p_auth_tx->TLF01_details.nfi_seq_nbr,
                 p_auth_tx->ch_billing_curr_code,
                 3 );
      }
   }

   if ((0 == strncmp(p_auth_tx->TLF01_details.message_type, "0400", 4)) ||
       (0 == strncmp(p_auth_tx->TLF01_details.message_type, "0401", 4)) ||
       (0 == strncmp(p_auth_tx->TLF01_details.message_type, "0102", 4)) ||
       (0 == strncmp(p_auth_tx->TLF01_details.message_type, "0420", 4)))
   {
      total_int    = atoi(p_auth_tx->TLF01_details.total_amount);
      actual_int   = atoi(p_auth_tx->TLF01_details.actual_amount);
      reversal_int = total_int - actual_int;
      itoa(reversal_int, p_auth_tx->TLF01_details.reversal_amount,10);
      Rj_with_lead_zeros(p_auth_tx->TLF01_details.reversal_amount, 12);

      /* Do this check only for reversals, not ATM confirms.
       * The reason is, DE61 (actual amt for ATM Confirm) is in USD,
       * whereas DE4 will be in the original currency.
       */
      if ( p_auth_tx->TLF01_details.message_type[1] == '4' )
      {
         if(actual_int > total_int)
         {
            memset( tempstr, 0x00, sizeof(tempstr) );
            sprintf( tempstr,
                    "Dispensed amount (%d) is greater than total amount (%d).",
                     actual_int, total_int );
            memset(Log_Bufr,0x00,sizeof(Log_Bufr));
            sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
            		p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
            strcat(Buffer,Log_Bufr);
            ncvis2_log_message( 3, 2, Buffer , "move_iso_response_to_auth_struct" , 0 ,p_auth_tx->TLF01_details.card_num);
         }
      }
   }
#if 0
   /*+-----------------------------+
     | EMV FIELDS and THIRD BITMAP |
     +-----------------------------+*/
   if ( (emv_txn == true) ||
        (HOST_MESSAGE.arpc_resp[8]    != 0x00) ||   /* If Field 139 present */
        (HOST_MESSAGE.iss_auth_len[0] != 0x00)  )   /* If Field 140 present */
   {
      emv_len = 2;  /* Allow 2 bytes for length. */

      /* FIELD 139: ARPC RESPONSE CRYPTOGRAM AND CODE */
      if ( !blank_or_null_string( HOST_MESSAGE.arpc_resp,
           sizeof(HOST_MESSAGE.arpc_resp)-1 ))
      {
         /* Convert the last 2-bytes (Resp Code) from EBCDIC to ASCII */
         HOST_MESSAGE.arpc_resp[8] = ascii_ebcdic_conv( HOST_MESSAGE.arpc_resp[8], E2A );
         HOST_MESSAGE.arpc_resp[9] = ascii_ebcdic_conv( HOST_MESSAGE.arpc_resp[9], E2A );

         memset( tempstr, 0x00, sizeof(tempstr) );
         genutil_bin_to_ascii( HOST_MESSAGE.arpc_resp,
                               sizeof(HOST_MESSAGE.arpc_resp), 
                               &tempstr[0], sizeof(tempstr) );

         /* Supply the Fixed Length value for the Device Controller
          * Binary = 10,  ASCII = 20
          */
         p_auth_tx->EMV_details.issuer_auth_data[0] = 0x0a;
         memcpy( p_auth_tx->EMV_details.issuer_auth_data+1, tempstr, 20 );

         /* Store the raw data. */
         p_auth_tx->EMV_details.emv_block[emv_len] = 0x91;  /* Tag 91 */
         emv_len++;
         p_auth_tx->EMV_details.emv_block[emv_len] = 0x0a;
         emv_len++;

         memcpy( &p_auth_tx->EMV_details.emv_block[emv_len],
                  HOST_MESSAGE.arpc_resp,
                  10 );
         emv_len += 10;
      }

      /* FIELD 140: ISSUER AUTHENTICATION DATA */
      len = HOST_MESSAGE.iss_auth_len[0];
      if ( len > 0 )
      {
         /* This field replaces field 139 for expanded format acquirers.
          * It contains information to be used by the card to perform
          * issuer authentication.
          */
         memset( tempstr, 0x00, sizeof(tempstr) );
         genutil_bin_to_ascii( HOST_MESSAGE.iss_auth_data,
                               sizeof(HOST_MESSAGE.iss_auth_data), 
                               &tempstr[0], sizeof(tempstr) );

         /* Supply the length of the Binary string for the Device Controller */
         p_auth_tx->EMV_details.issuer_auth_data[0] = len;
         memcpy( p_auth_tx->EMV_details.issuer_auth_data+1, tempstr, len*2 );

         /* Store the raw data. */
         p_auth_tx->EMV_details.emv_block[emv_len] = 0x91;  /* Tag 91 */
         emv_len++;
         p_auth_tx->EMV_details.emv_block[emv_len] = len;
         emv_len++;

         memcpy( &p_auth_tx->EMV_details.emv_block[emv_len],
                  HOST_MESSAGE.iss_auth_data,
                  len );
         emv_len += len;
      }

      /* FIELD 142: ISSUER SCRIPT */
      memset( tempstr, 0x00, sizeof(tempstr) );
      len = HOST_MESSAGE.iss_script_len[0];
      if ( len > 0 )
      {
         genutil_bin_to_ascii( HOST_MESSAGE.iss_script_len,
                               len+1, 
                               &tempstr[0], sizeof(tempstr) );

         memcpy( p_auth_tx->EMV_details.issuer_script_1, tempstr,
                 sizeof(p_auth_tx->EMV_details.issuer_script_1)-1 );

         /* Store the raw data - tag and len are embedded in data. */
         memcpy( &p_auth_tx->EMV_details.emv_block[emv_len],
                  HOST_MESSAGE.iss_script,
                  len );
         emv_len += len;
      }

      /* FIELD 143: ISSUER SCRIPT RESULTS */
      memset( tempstr, 0x00, sizeof(tempstr) );
      len = HOST_MESSAGE.iss_result_len[0];
      if ( len > 0 )
      {
         genutil_bin_to_ascii( HOST_MESSAGE.iss_result_len,
                               len+1, 
                               &tempstr[0], sizeof(tempstr) );

         memcpy( p_auth_tx->EMV_details.issuer_script_2, tempstr,
                 sizeof(p_auth_tx->EMV_details.issuer_script_2)-1 );

         /* Store the raw data - tag and len are embedded in data. */
         memcpy( &p_auth_tx->EMV_details.emv_block[emv_len],
                  HOST_MESSAGE.iss_result,
                  len );
         emv_len += len;
      }

      memset( tempstr, 0x00, sizeof(tempstr) );
      sprintf( tempstr, "%04d", emv_len-2 );
      genutil_asc_to_bcd( tempstr, 4, p_auth_tx->EMV_details.emv_block );
   }
#endif
   if ( false == verify_auth_tx_data(p_auth_tx) )
      retval = false;

   return( retval );

} /* move_iso_response_to_auth_struct */

/*****************************************************************************

  Function:    build_host_structure

  Description: This function will use the data in the auth_tx entry to build 
               the HOST_MESSAGE structure that will be required to build the 
               VISA message for the host.

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
*********************y*********************************************************/
void build_host_structure( pAUTH_TX p_auth_tx )
{
   #define  F126_BITMAP_LEN  8
   #define  EMV_ARPC_FAILURE 0x40   /* Bit 7 */

   CHAR   temp_str[100]={0};
	CHAR   VbV_data[150]={0x00};
   CHAR   orig_data[43]={0};
   CHAR   new_string[50]={0};
   CHAR   tempstr [256]={0};
   UINT   count = 0;
   INT    i = 0;
   BYTE   F62BitMap[8]={0};
   INT    F62Len=0;
   BYTE   F63BitMap[3]={0};
   INT    F63Len = 0;
   INT    F63_3_Present = 0;
   INT    F63_ByteTwo =0;
   BYTE   temp_bcd[3]={0};
   BYTE   stan[7] ={0};
   BYTE   visa_char = 0;
   BYTE   visa_tran[17]={0};
   BYTE   visa_tran_bcd[8]={0};
   CHAR   visa_mvv_bcd[20]={0};
   BYTE   pos_entry_mode [5] ={0};
   BYTE   pos_cc[3] ={0};
   BYTE   processing_code [7]={0};
   BYTE   temp_currency_code[4]={0};
   BYTE   acct_type = 0;
   BYTE   cvc2[8] ={0};
   BYTE   visa_incremental_MC_asc[5] = {0};
   BYTE   visa_incremental_MC_bcd [3] = {0};
   INT    iIndustry = 0;
   INT    RPS_flag = 0;
   DWORD  total_amount = 0;
   DWORD  reversal_amount = 0 ;
   DWORD  actual_amount = 0;
   CHAR   gmt_time[15]={0};
   INT    F126Len = 0;
   INT    len = 0;
   INT    sublen = 0;
   INT    txn_flow = 0;
   INT    rcode = 0;
   CHAR	  tmp_len56[5] = {0};
   INT 	  f56_len = 0;

   BYTE eCommerce_Indcator[3]={0} ;
   BYTE card_auth_value[2]={0} ;
   BYTE trans_id[50]={0} ;
   BYTE cvv_data[50]={0} ;
   BYTE btran_id[41]={0};
   BYTE bcavv_data[41]={0};
   INT  num_binary_bytes = 0;
   INT  cavv_data_exists = false;
   INT temp_len = 0;
   BYTE emv_chip_code = 0;
   INT  emv_txn = 0;
   INT  emv_len = 0;
   INT  pos_entry_type = POS_ENTRY_NORMAL;
   INT  ret_val = 0;
   BYTE hex_tag[5]={0};
   BYTE arpc = 0;
   CHAR  templen [6]={0};
   BYTE   visa_inc_temp_len[4] = {0};
   BYTE Fld12613 = 0;
   BYTE F60_asci_value[13] = {0} ;


   /* Initialize the host structure before filling it. */
   memset( &HOST_MESSAGE, 0x00, sizeof(HOST_MESSAGE) );
	memset(temp_str,0x00, sizeof(temp_str));
   Third_Bitmap = false;
   iIndustry = atoi( p_auth_tx->industry_code );

   /* Determine if txn is EMV (Smart Card). */
   if ( (p_auth_tx->EMV_details.emv_block[0] == 0x00) &&
        (p_auth_tx->EMV_details.emv_block[1] == 0x00)  )
      emv_txn = false;
   else
   {
      emv_txn = true;
      if(strcmp(p_auth_tx->TLF01_details.dispensation.origin,"RO")==0)
		  emv_len = p_auth_tx->EMV_details.emv_block[0];
		  else
		  emv_len = genutil_bcd_to_int( p_auth_tx->EMV_details.emv_block, 2 );
   }

	/* Verified by visa VbV */
	/* Breakup the fields SE8, SE8, SE28, SE29*/
	if (true == ncvis2_Is_AsiaPay_Transaction(p_auth_tx))
	{
		/* all the data is in auth_tx.int_net_facility_data */
		iIndustry = ECOMMERCE;
		memset(eCommerce_Indcator,0x00, sizeof(eCommerce_Indcator));
		memset(card_auth_value,0x00, sizeof(card_auth_value));
		memset(trans_id,0x00, sizeof(trans_id));
		memset(cvv_data,0x00, sizeof(cvv_data));

		memcpy(VbV_data, p_auth_tx->int_net_facility_data, sizeof(p_auth_tx->int_net_facility_data));		
		len = 0;
		memcpy(eCommerce_Indcator, VbV_data+8, 2);
		len += 10;
		memcpy(card_auth_value, VbV_data + len, 1);
		len += 1;
		memcpy(trans_id, VbV_data + len, 20);
		len += 20;
		memcpy(cvv_data, VbV_data + len, 40);

      if ( false == blank_or_null_string(trans_id, sizeof(trans_id)) )
         cavv_data_exists = true;
      else if ( false == blank_or_null_string(cvv_data, sizeof(cvv_data)) )
         cavv_data_exists = true;
   }
	else if (iIndustry == RPS ||
			(0==strncmp(p_auth_tx->TLF01_details.pos_condition_code, "08",2) &&
			(0 == strncmp(p_auth_tx->TLF01_details.pos_entry_mode,"001",3))))
	{
		iIndustry = RPS ;
		RPS_flag = 1;
	}

   if ( (p_auth_tx->TLF01_details.tx_key >= AUTH_SALE) &&
        (p_auth_tx->TLF01_details.tx_key < AUTH_INVALID) )
      txn_flow = SWITCHED_OUT;

   else if ( p_auth_tx->TLF01_details.tx_key == AUTH_CARD_UPDATE_RESPONSE )
      txn_flow = SWITCHED_OUT;

   else
      txn_flow = SWITCHED_IN;


   /* VISA HEADER */

   /* Stored in Merchant Contact field for switched in txns.
    * If txn is switched out, this field will not get copied
    * to the auth_tx from the timer, so it has no effect.
    */
   memcpy( HOST_MESSAGE.header, p_auth_tx->MCF01_details.contact, 23 );
   
   /* If Original txn process as STIP and later send into host.
      After sent host If ATP gets reversal txn for that.
	  It should go as 420 msg - TF-Ajay 08-Jul-09 */
   if(strcmp(p_auth_tx->TLF01_details.product_codes[1].code,PROCESSED)==0) 
   {

		p_auth_tx->TLF01_details.tx_key = AUTH_REVERSAL_ADVICE; 

		strcpy(p_auth_tx->TLF01_details.message_type, "0420" ); 
		printf("sending as 420");
   }


   /* FIELD 2: PAN - Prepend leading zero if length is odd. */
	HOST_MESSAGE.pan_len[0] = atoi( p_auth_tx->TLF01_details.card_num_len );

   if ( (HOST_MESSAGE.pan_len[0] % 2) == 1 )
   {
      memset (temp_str, 0, sizeof(temp_str));
      temp_str[0] = '0';
      memcpy (&temp_str[1], p_auth_tx->TLF01_details.card_num,
              HOST_MESSAGE.pan_len[0]);
      str_asc_to_bcd(temp_str,HOST_MESSAGE.pan_len[0] + 1,HOST_MESSAGE.pan_bin);
      HOST_MESSAGE.pan_len[0] = strlen(p_auth_tx->TLF01_details.card_num);
   }
   else
   {
      str_asc_to_bcd (p_auth_tx->TLF01_details.card_num, sizeof(HOST_MESSAGE.pan_bin)*2, HOST_MESSAGE.pan_bin);
      HOST_MESSAGE.pan_len[0] = strlen(p_auth_tx->TLF01_details.card_num);
   }


   /* FIELD 3: PROCESSING CODE */
   memcpy( processing_code,
           p_auth_tx->TLF01_details.processing_code,
           sizeof(processing_code) );

   if ( txn_flow == SWITCHED_OUT )
   { /* request */
      if ( (processing_code[0] == 0x00) ||
           (AUTH_VOID_SALE == p_auth_tx->TLF01_details.tx_key) )
      {
         strcpy( processing_code, "000000" );
      }

      switch (iIndustry)
      {
         case CASH:   /* Cash */
               strncpy(processing_code, "01", 2);
            break;
         case ATM:   /* ATM */
               strncpy(processing_code, "01", 2);
            break;
         case QUASI_CASH:   /* Quasi Cash */
               strncpy(processing_code, "11", 2);
            break;
         default:
            break;
      }
      if ( ('Y' == p_auth_tx->TLF01_details.voice_auth[0]) &&
           (strncmp(processing_code,"18",2) == 0) )
      {
         memcpy( processing_code, "11", 2 );
      }

      if (AUTH_BALANCE_INQUIRY == p_auth_tx->TLF01_details.tx_key)
         strncpy(processing_code, "30", 2);

      /* These 2 lines are in here specifically for Equitable. */
      else if (AUTH_CARD_VERIFICATION == p_auth_tx->TLF01_details.tx_key)
         memcpy( processing_code, "00", 2 );
	 /* DE3 should be 00 for below case-  TF Phani*/
	  else if (AUTH_REVERSAL_ADVICE == p_auth_tx->TLF01_details.tx_key ||
		       AUTH_REVERSAL_ADVICE_REPEAT == p_auth_tx->TLF01_details.tx_key )
         memcpy( processing_code, "00", 2 );
     /* else if (0 == strcmp(MCC_QUASI_CASH,p_auth_tx->TLF01_details.category_code))
         memcpy( processing_code, "11", 2 );*/
   }
#if BDOR_62_PREAUTH_VOID
   if (AUTH_VOID_PRE_AUTHORIZATION == p_auth_tx->TLF01_details.tx_key )
   {
	   memcpy( processing_code, "00", 2 );
   }
#endif

   asc_to_bcd (processing_code, sizeof(HOST_MESSAGE.proccode)*2,
               HOST_MESSAGE.proccode);
   /* In order to show proceesing code in transaction lookup page( OCT-2017 Mandated changes) */
	memcpy(p_auth_tx->TLF01_details.product_codes[17].amount,processing_code,sizeof(HOST_MESSAGE.proccode)*2);

   /* FIELD 4: AMOUNT */
	if(0 == strncmp(p_auth_tx->TLF01_details.product_codes[3].quantity,"PR",2))
	{
		asc_to_bcd (p_auth_tx->TLF01_details.product_codes[19].amount, sizeof(HOST_MESSAGE.amount_bin)*2,
					HOST_MESSAGE.amount_bin);
	}
	else if( strlen(p_auth_tx->reconciliation_amount) !=0 )
	    asc_to_bcd (p_auth_tx->reconciliation_amount, sizeof(HOST_MESSAGE.amount_bin)*2,
	               HOST_MESSAGE.amount_bin);
   else
   asc_to_bcd (p_auth_tx->TLF01_details.total_amount, sizeof(HOST_MESSAGE.amount_bin)*2,
               HOST_MESSAGE.amount_bin);
   
#if VISA2
   if((strncmp(p_auth_tx->TLF01_details.handler_queue,"sbatchA",4) == 0) &&
		   0 != strcmp( PESO, p_auth_tx->TLF01_details.nfi_seq_nbr) &&
		   0 != strcmp( USD, p_auth_tx->TLF01_details.nfi_seq_nbr ) &&
	  	  (strcmp(p_auth_tx->TLF01_details.dispensation.origin,"RO") == 0))
   {
	   asc_to_bcd (p_auth_tx->TLF01_details.sales_amount, sizeof(HOST_MESSAGE.amount_bin)*2,
	                  HOST_MESSAGE.amount_bin);
   }
#endif

#if BDOR_62_PREAUTH_VOID
   if (AUTH_VOID_PRE_AUTHORIZATION == p_auth_tx->TLF01_details.tx_key )
   {
	   asc_to_bcd (p_auth_tx->TLF01_details.total_interest, sizeof(HOST_MESSAGE.amount_bin)*2,
	                  HOST_MESSAGE.amount_bin);
   }
#endif
   /* FIELD 6: CARDHOLDER BILLING AMOUNT */

   if (p_auth_tx->ch_billing_amt [0] != '\0')
   {
      asc_to_bcd (p_auth_tx->ch_billing_amt, sizeof(HOST_MESSAGE.card_bill_amt)*2,
                  HOST_MESSAGE.card_bill_amt);
   }
   /* If the txn sends from the sbacth copy the card holder billing amount */

   if((strncmp(p_auth_tx->TLF01_details.handler_queue,"sbatchA",4)==0)&&
	  (strcmp(p_auth_tx->TLF01_details.dispensation.origin,"RO")==0))
   {
//	 memcpy(HOST_MESSAGE.card_bill_amt,p_auth_tx->TLF01_details.product_codes[7].amount,sizeof(p_auth_tx->TLF01_details.product_codes[7].amount));
     asc_to_bcd (p_auth_tx->TLF01_details.product_codes[7].amount, sizeof(HOST_MESSAGE.card_bill_amt)*2,
                  HOST_MESSAGE.card_bill_amt);

   }


   /* FIELD 7: GREENWICH TIME - MMDDhhmmss from the YYYYMMDDhhmmss date */
   if ( (p_auth_tx->TLF01_details.tx_key == AUTH_REVERSAL) ||
        (p_auth_tx->TLF01_details.tx_key == AUTH_VOID_SALE) )
   {
      if ( p_auth_tx->orig_local_time_date[0] == 0x00 )
      {
         strcpy( p_auth_tx->orig_local_time_date,
                 p_auth_tx->TLF01_details.transmission_timestamp);

         pintime_get_gmt( gmt_time );
         strncpy( p_auth_tx->TLF01_details.transmission_timestamp,
                 &gmt_time[4],
                  10 );
      }
   }
   else if( 0  == strncmp(p_auth_tx->TLF01_details.product_codes[3].quantity,"PR",2) &&
		    '\0' != p_auth_tx->orig_local_time_date[0]	   )
  {
	   memcpy( HOST_MESSAGE.gmt_time,
			   p_auth_tx->orig_local_time_date,
			   bf7_len );
  }

   asc_to_bcd (p_auth_tx->TLF01_details.transmission_timestamp,
                sizeof(HOST_MESSAGE.gmt_time)*2,
                HOST_MESSAGE.gmt_time);

   /* FIELD 10: CARDHOLDER BILLING CONVERSION RATE */
   if (true == hhutil_verify_pin_block(p_auth_tx) ||
   	0 == strncmp(p_auth_tx->TLF01_details.handler_queue, "ncvis", 5))// nirbhay ThuoghtFocus: Cadencie link changes - pass field 10 (conversion rate) irrespective of pin block.
   {
      asc_to_bcd (p_auth_tx->ch_billing_conv_rate,
                  sizeof(HOST_MESSAGE.conv_rate)*2,
                  HOST_MESSAGE.conv_rate);
   }
   else if((strncmp(p_auth_tx->TLF01_details.handler_queue,"sbatchA",4)==0)&&
	  (strcmp(p_auth_tx->TLF01_details.dispensation.origin,"RO")==0))
   {
	   asc_to_bcd (p_auth_tx->ch_billing_conv_rate,
                  sizeof(HOST_MESSAGE.conv_rate)*2,
                  HOST_MESSAGE.conv_rate);
   }

   else
   {/* Don't do anything */ }

   /* FIELD 11: STAN */
   if ( p_auth_tx->TLF01_details.tx_key == AUTH_REVERSAL )
   {
      strcpy( stan, p_auth_tx->TLF01_details.orig_sys_trace_num );
      strcpy( p_auth_tx->TLF01_details.sys_trace_audit_num,
              p_auth_tx->TLF01_details.orig_sys_trace_num );
   }
   else if ( p_auth_tx->TLF01_details.tx_key == AUTH_VOID_SALE )
   {
      strcpy( stan, p_auth_tx->TLF01_details.orig_sys_trace_num );
   }
   else
   {
      strcpy( stan, p_auth_tx->TLF01_details.sys_trace_audit_num );
   }
#if BDOR_62_PREAUTH_VOID
   if (AUTH_VOID_PRE_AUTHORIZATION == p_auth_tx->TLF01_details.tx_key )
   {
	      strcpy( stan, p_auth_tx->TLF01_details.orig_sys_trace_num );
	      strcpy( p_auth_tx->TLF01_details.sys_trace_audit_num,
	              p_auth_tx->TLF01_details.orig_sys_trace_num );
   }
#endif
   asc_to_bcd( stan,
               sizeof(HOST_MESSAGE.stan_bin)*2,
               HOST_MESSAGE.stan_bin );

   /* FIELD 14: EXPIRATION DATE */
   asc_to_bcd (p_auth_tx->TLF01_details.exp_date, sizeof(HOST_MESSAGE.date_expd)*2,
               HOST_MESSAGE.date_expd);

   /* FIELD 18: SIC NUMBER */
   asc_to_bcd (p_auth_tx->TLF01_details.category_code , sizeof(HOST_MESSAGE.sic_number)*2,
               HOST_MESSAGE.sic_number);
    
   /* FIELD 19: COUNTRY CODE */
	if( strncmp(p_auth_tx->TLF01_details.handler_queue, "ncvis", 5) != 0)
	{
	   if ( txn_flow == SWITCHED_OUT )
	   { // request 
		   /*  copy the original txn country code TF-Ajay */ 
		   if((strncmp(p_auth_tx->TLF01_details.handler_queue,"sbatchA",4)==0)&&
			   (p_auth_tx->TLF01_details.num_dbsales[0]!=0x00))
		   {
			  strcpy (temp_str, p_auth_tx->TLF01_details.num_dbsales);
			  Rj_with_lead_zeros (temp_str, sizeof(HOST_MESSAGE.ctry_code)*2);
			  asc_to_bcd (temp_str, sizeof(HOST_MESSAGE.ctry_code)*2, HOST_MESSAGE.ctry_code);
		   }
		   else
		   {
			  strcpy (temp_str, ncf01_i.country_code);
			  Rj_with_lead_zeros (temp_str, sizeof(HOST_MESSAGE.ctry_code)*2);
			  asc_to_bcd (temp_str, sizeof(HOST_MESSAGE.ctry_code)*2, HOST_MESSAGE.ctry_code);
		   }
	   }
	   else // response 
	   {
		  strcpy (temp_str, p_auth_tx->country_code);
		  Rj_with_lead_zeros (temp_str, sizeof(HOST_MESSAGE.ctry_code)*2);
		  asc_to_bcd (temp_str, sizeof(HOST_MESSAGE.ctry_code)*2, HOST_MESSAGE.ctry_code);
	   }
  
	} // TF  Phani Old logic....
	else if(strncmp(p_auth_tx->TLF01_details.handler_queue,"sbatchA",4)==0)
	{
		if(strlen(p_auth_tx->TLF01_details.num_dbsales ) > 0)
		{
		  strcpy (temp_str, p_auth_tx->TLF01_details.num_dbsales);
		  Rj_with_lead_zeros (temp_str, sizeof(HOST_MESSAGE.ctry_code)*2);
		  asc_to_bcd (temp_str, sizeof(HOST_MESSAGE.ctry_code)*2, HOST_MESSAGE.ctry_code);
		}
		else
		{

			strcpy (temp_str, p_auth_tx->TLF01_details.currency_code);
			Rj_with_lead_zeros (temp_str, sizeof(HOST_MESSAGE.ctry_code)*2);
			asc_to_bcd (temp_str, sizeof(HOST_MESSAGE.ctry_code)*2, HOST_MESSAGE.ctry_code);
		}
	}

    else  /* New logic for Feild 19 whatever comes from VISA .. TF... Phani*/
	{

	  strcpy (temp_str, p_auth_tx->country_code);
      Rj_with_lead_zeros (temp_str, sizeof(HOST_MESSAGE.ctry_code)*2);
      asc_to_bcd (temp_str, sizeof(HOST_MESSAGE.ctry_code)*2, HOST_MESSAGE.ctry_code);

    }
   /* FIELD 20: PAN EXTENDED, COUNTRY CODE */
   strcpy (temp_str, p_auth_tx->pan_ext_ctry_code);
   Rj_with_lead_zeros (temp_str, sizeof(HOST_MESSAGE.pan_ctry_cd)*2);
   asc_to_bcd (temp_str, sizeof(HOST_MESSAGE.pan_ctry_cd)*2, HOST_MESSAGE.pan_ctry_cd);

   /* FIELD 22: POS ENTRY MODE */
   if ( txn_flow == SWITCHED_OUT )
   {
      /* Set the POS Entry Mode flag for use in other fields. */
      if ( emv_txn == true )
         pos_entry_type = POS_ENTRY_EMV_CHIP;
      else if( 0 == strncmp("0802",p_auth_tx->TLF01_details.pos_entry_mode,4) ||
    		  0 == strncmp("0801",p_auth_tx->TLF01_details.pos_entry_mode,4))
      			  pos_entry_type = POS_ENTRY_FALLBACK;
      else if ( 0 == strncmp("080",p_auth_tx->TLF01_details.pos_entry_mode,3) )
         pos_entry_type = POS_ENTRY_EMV_MAGSTRIPE;

      else if ( 0 == strncmp("008",p_auth_tx->TLF01_details.pos_entry_mode,3) )
         pos_entry_type = POS_ENTRY_EMV_FALLBACK;
   }
#if BDOR_68_REQ_1_VISA /*MobilePOS-NFC Terminal for Tap2Phone Transactions*/

   if (true == ncvis2_check_if_sbatch_qualify_for_MobilePOS_NFC_Tap2Phone_trx_with_pin(p_auth_tx))
   {
	   strncpy( pos_entry_mode,
			    VISA_MOBILE_NFC_TAP2PHONE_TXN_WITH_PIN_PEM,
			    VISA_DE22_PEM_LEN );
   }
   else if (true == ncvis2_check_if_sbatch_qualify_for_MobilePOS_NFC_Tap2Phone_trx_without_pin(p_auth_tx))
   {
	   strncpy( pos_entry_mode,
			    VISA_MOBILE_NFC_TAP2PHONE_TXN_WITHOUT_PIN_PEM,
			    VISA_DE22_PEM_LEN );
   }
   else if(true == ncvis2_check_if_trx_is_visa_MobilePOS_NFC_Tap2Phone_trx_with_pin(p_auth_tx))
   {
	   strncpy( pos_entry_mode,
			    VISA_MOBILE_NFC_TAP2PHONE_TXN_WITH_PIN_PEM,
			    VISA_DE22_PEM_LEN );
   }
   else if(true == ncvis2_check_if_trx_is_visa_MobilePOS_NFC_Tap2Phone_trx_without_pin(p_auth_tx))
   {
	   strncpy( pos_entry_mode,
			    VISA_MOBILE_NFC_TAP2PHONE_TXN_WITHOUT_PIN_PEM,
			    VISA_DE22_PEM_LEN );
   }
#endif

#if BDOR_68_REQ_3_VISA /*Swiftpass BDO Pay Credit Transactions*/
   else if (true == ncvis2_check_if_sbatch_qualify_for_Swiftpass_BDO_Pay_credit_trx_with_pin(p_auth_tx))
   {
	   strncpy( pos_entry_mode,
			    VISA_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITH_PIN_PEM,
			    VISA_DE22_PEM_LEN );
   }
   else if (true == ncvis2_check_if_sbatch_qualify_for_Swiftpass_BDO_Pay_credit_trx_without_pin(p_auth_tx))
   {
	   strncpy( pos_entry_mode,
			    VISA_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITHOUT_PIN_PEM,
			    VISA_DE22_PEM_LEN );
   }
   else if(true == ncvis2_check_if_trx_is_visa_Swiftpass_BDO_Pay_credit_trx_with_pin(p_auth_tx))
   {
	   strncpy( pos_entry_mode,
			    VISA_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITH_PIN_PEM,
			    VISA_DE22_PEM_LEN );
   }
   else if(true == ncvis2_check_if_trx_is_visa_Swiftpass_BDO_Pay_credit_trx_without_pin(p_auth_tx))
   {
	   strncpy( pos_entry_mode,
			    VISA_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITHOUT_PIN_PEM,
			    VISA_DE22_PEM_LEN );
   }
#endif
#if BDOR_68_REQ_5_VISA /*Ecommerce Entry Mode From Swiftpass BDOPay Transactions*/
   else if (true == ncvis2_check_if_sbatch_qualify_for_ecommerce_swiftpass_BDOPay_trx(p_auth_tx))
   {
	   strncpy( pos_entry_mode,
			    VISA_ECOMMERCE_SWIFTPASS_BDOPAY_TXN_PIN_PEM,
			    VISA_DE22_PEM_LEN );
   }
   else if(true == ncvis2_check_if_trx_is_visa_ecommerce_swiftpass_BDOPay_trx (p_auth_tx))
   {
	   strncpy( pos_entry_mode,
			    VISA_ECOMMERCE_SWIFTPASS_BDOPAY_TXN_PIN_PEM,
			    VISA_DE22_PEM_LEN );
   }
#endif

   else if (0 == strncmp("096",p_auth_tx->TLF01_details.pos_entry_mode,3) )
	{
		strcpy( pos_entry_mode, "9600" ); /* Request from VISA ,May have 96, send as it is*/
	}
	else if ( iIndustry == ECOMMERCE )
	{
      /* Always use this POS Entry Mode for e-commerce. */
      strncpy( pos_entry_mode, p_auth_tx->TLF01_details.pos_entry_mode, 4 );
    }
   else if ( emv_txn == true )
   {
      /* This is a chip (EMV/Smart Card) txn.  Check if
       * card was swiped or if the chip was used.
       */
     #if 0
	   if (0 == strncmp("080",p_auth_tx->TLF01_details.pos_entry_mode,3) )
      {
         /* POS Entry indicates magstripe txn at a chip card terminal.
          * Set DE22 to '90..' so as not to bear the liability of
          * a fallback transaction (DE22="95").
          */
         strcpy( pos_entry_mode, "90n0" );
         pos_entry_mode[2] = p_auth_tx->TLF01_details.pos_entry_mode[3];
      }
      else
      {
         /* Normal Chip Card Txn */
         strcpy( pos_entry_mode, "05n0" );
      }
	#endif
	  pos_entry_mode[0] = p_auth_tx->TLF01_details.pos_entry_mode[1];
	  pos_entry_mode[1] = p_auth_tx->TLF01_details.pos_entry_mode[2];	
      pos_entry_mode[2] = p_auth_tx->TLF01_details.pos_entry_mode[3];
	  pos_entry_mode[3] = 0x30;
   }
   else if ( pos_entry_type == POS_ENTRY_EMV_FALLBACK )
   {
      /* POS Entry indicates magstripe txn at a chip card terminal.
       * Set DE22 to '90..' so as not to bear the liability of
       * a fallback transaction (DE22="95").
       */
      strcpy( pos_entry_mode, "02n0" );
      pos_entry_mode[2] = p_auth_tx->TLF01_details.pos_entry_mode[3];
   }else if (0 == strncmp("080",p_auth_tx->TLF01_details.pos_entry_mode,3) )
     {
      strcpy( pos_entry_mode, "90n0" );
	  pos_entry_mode[2] = p_auth_tx->TLF01_details.pos_entry_mode[3];
   	}
   else if ((0 == strncmp("001",p_auth_tx->TLF01_details.pos_entry_mode,3)) &&
		        (0 == strncmp("00",p_auth_tx->TLF01_details.pos_condition_code,2)))
    {
	  /* Manual Keyin */
      strcpy( pos_entry_mode, "01n0" );
   	  pos_entry_mode[2] = p_auth_tx->TLF01_details.pos_entry_mode[3];
    }
   else if (0 == strncmp("001",p_auth_tx->TLF01_details.pos_entry_mode,3) &&
		    0 == strncmp("08",p_auth_tx->TLF01_details.pos_condition_code,2))
     {	/*TF Raghu - APR 2018 Visa Mandated changes: RPS Case : 001x to 10xx change */
         strcpy( pos_entry_mode, "1020" );
         pos_entry_mode[2] = p_auth_tx->TLF01_details.pos_entry_mode[3];
      }
   else if(((0 == strncmp("010",p_auth_tx->TLF01_details.pos_entry_mode,3)) &&
		   (0 == strncmp(p_auth_tx->TLF01_details.handler_queue,"sbat", 4))))
   {
	   // Credential of file should be send durring sbatch as 10X
	   strncpy( pos_entry_mode,p_auth_tx->TLF01_details.pos_entry_mode +1 , 3);
	   strncat(pos_entry_mode,"0");
	}
   else
   {
      /* Use a temporary variable to create the new pos entry mode so
       * that the original value is inserted into TLF01.
       */
      memcpy( pos_entry_mode,
              p_auth_tx->TLF01_details.pos_entry_mode,
              sizeof(pos_entry_mode));
      if (pos_entry_mode[0] != '\0')  
      {
      /*+--------------------------------------------------------------------+
        | Equitable is using some POS terminals at their provincial satellite|
        | offices acting as offsite authorization centers. Their satellite   |
        | offices use the POS terminal to key in transaction to get approval.|
        | Each satellite office has its own merchant number assignment.  The |
        | problem is that the POS Indicator being sent to Visa Int'l is      |
        | currently defaulted to '01'.  Visa is requiring Equitable to use   |
        | POS Entry Mode Code = '00' for their satellite offices.  Equitable |
        | will indicate which merchants are satellites with an Organization  |
        | ID of "0001".                                                      |
        +--------------------------------------------------------------------+*/
         if (0 == strcmp("0001",
                          p_auth_tx->MCF01_details.primary_key.organization_id))
         {
            /* '+1' is used to bypass the 0 filler in the first byte. */
            memset( pos_entry_mode+1, '0', 2 );
         }

         /*
          * input is Hypercom iso8583: 0aab   where
          *    aa = PAN entry mode
          *    b  = PIN entry capabitlity
          *
          * convert to Visa iso8583:   aab0   where
          *    aa = PAN entry mode
          *    b  = PIN entry capabitlity
          */
         pos_entry_mode [0] = 0x30;     /* set to '0' */
         pos_entry_mode [1] = pos_entry_mode [2];
         pos_entry_mode [2] = pos_entry_mode [3];
         pos_entry_mode [3] = 0x30;     /* set to '0' */

         switch (pos_entry_mode [1])
         {
            case '0': break;  /* OK.  leave as is */
            case '1': break;  /* OK.  leave as is */
            case '2':
            case '8':
               pos_entry_mode [0] = '9';
               pos_entry_mode [1] = '0';
               break;
            default:
               /* unknown PAN mode: force to '0' */
               pos_entry_mode [1] = '0';
               break;
         }

         switch (pos_entry_mode [2])
         {
            case '0': break;  /* OK.  leave as is */
            case '1': break;  /* OK.  leave as is */
           /* case '2': break;*/  /* OK.  leave as is */ // Old Logic for NcVisa
			case '2': 
				pos_entry_mode [2] = '2'; // New req for Cardeince.. TF Phani..
				break;
            default:
               /* unknown PIN mode: force to '0' */
               pos_entry_mode [2] = '0';
               break;
         }
         pos_entry_mode [4] = '\0';
      }
   }
   asc_to_bcd (pos_entry_mode, sizeof(HOST_MESSAGE.posentry)*2, 
               HOST_MESSAGE.posentry);

   memcpy(p_auth_tx->TLF01_details.product_codes[17].code,pos_entry_mode,sizeof(HOST_MESSAGE.posentry)*2);

   /* FIELD 23: CARD SEQUENCE NUMBER */
   de23_present = false;


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
/*   else if ( emv_txn == true )
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
        // Convert the value to ASCII to get the numeric value. 
         de23_present = true;
         bcd_to_asc( tempstr, len,
                     temp_str, len+len );
         i = atoi( temp_str );
      }
   } */

   

   if ( de23_present == true )
   {
      /* Value must be right justified, zero filled. */
      sprintf( temp_str, "%04d", i );
      asc_to_bcd( temp_str, sizeof(HOST_MESSAGE.cardseqnum)*2, 
                  HOST_MESSAGE.cardseqnum);
   }

   /* FIELD 25: POS CONDITION CODE */

   memcpy( pos_cc, p_auth_tx->TLF01_details.pos_condition_code, 2 );

   if(IsFieldNull(p_auth_tx->TLF01_details.pos_condition_code)) /* Neetu : Checked The Field , should copy all other values  only when 
	                  field in AUTHTX is null*/
   {					 // Neetu

	   if ( txn_flow == SWITCHED_OUT )
	   {
		  /* Request */
		  if ( ECOMMERCE == iIndustry )
			 /* E-Commerce */
			 strcpy( pos_cc, "59" );

		  else if ( (MAIL_PHONE == iIndustry) || (RPS == iIndustry) )
		  {
			 /* Mail/phone order or Recurring Payment */
			 strcpy( pos_cc, "08" );

			 /* Set RPS flag for field 60, if Recurring Payment. */
			 if ( RPS == iIndustry )
				RPS_flag = 1;
		  }
		  else
		  {
			 i = atoi(p_auth_tx->MCF01_details.voice_category_code);
			 if ((i >= 5960) && (i <= 5969))
				/* Mail/phone order */
				strcpy( pos_cc, "08" );

			 else if (NULL != strstr( p_auth_tx->TLF01_details.handler_queue, "rps" ) )
			 {
				/* RPS */
				strcpy( pos_cc, "08" );
				RPS_flag = 1;
			 }
			 else
				/* Any other purchase */
				strcpy( pos_cc, "00" );
		  }
	   }
   }  // Neetu
   else if (RPS == iIndustry)
   {
          RPS_flag = 1;
   }
   asc_to_bcd(pos_cc,sizeof(HOST_MESSAGE.pos_condcd)*2,HOST_MESSAGE.pos_condcd);

   /* FIELD 28: TRANSACTION FEE AMOUNT */
   /* Pass fee amount to Host, Girija Y TF, Apr 06 2009, Apr 2009 Mnadates */
   if( p_auth_tx->TLF01_details.tran_fee_amount[0] != 0x00 )
   {
	   /* copy the first byte of tran fee amount, TF- Ajay */
       if(0 == strncmp(p_auth_tx->TLF01_details.message_type, "0100", 4) &&
    	  p_auth_tx->TLF01_details.product_codes[2].code[0] != 0x00)
	   {
		   HOST_MESSAGE.fee_amount[0]= p_auth_tx->TLF01_details.product_codes[2].code[0];
	   }
	   else
	   {
		   if((p_auth_tx->TLF01_details.tx_key==AUTH_REVERSAL) ||
			  (p_auth_tx->TLF01_details.tx_key==AUTH_REVERSAL_REPEAT) ||
			  (p_auth_tx->TLF01_details.tx_key==AUTH_REVERSAL_ADVICE))
		   {
			   HOST_MESSAGE.fee_amount[0]='C';
		   }
		   else
		   { 
			   HOST_MESSAGE.fee_amount[0]='D';
		   }
	   }
	   
	   
	   memcpy(&HOST_MESSAGE.fee_amount[1],p_auth_tx->TLF01_details.tran_fee_amount,
		      sizeof(HOST_MESSAGE.fee_amount)-1);
   }

   /* FIELD 32: ACQUIRING INSTITUTION ID CODE */
  
   memset (new_string, 0, sizeof(new_string));
 
    if ( txn_flow == SWITCHED_OUT )
   { /* request */
    
// Thoughtfocus-- Phani-- Cadience Request. 

	if( strncmp(p_auth_tx->TLF01_details.handler_queue, "ncvis", 5) == 0)
	{
		 strcpy(new_string,p_auth_tx->acq_inst_id_cd);   
	}
	else if((strncmp(p_auth_tx->TLF01_details.handler_queue,"sbatchA",4)==0)&&
		     (p_auth_tx->TLF01_details.acquiring_id[0]!=0x00))
	{
		  strcpy (new_string, p_auth_tx->TLF01_details.acquiring_id);
	}
	else
	{
		strcpy(new_string,ncf01_i.acquirer_inst_id_code); //Neetu
	}
	  
   
      HOST_MESSAGE.acq_inst_len[0] = strlen(new_string);

      /* Store here for TLF01. */
      if ( p_auth_tx->TLF01_details.acquiring_id[0] == 0x00 )
         strcpy( p_auth_tx->TLF01_details.acquiring_id, new_string );
   }
   
   else
   { /* response */
      HOST_MESSAGE.acq_inst_len[0] = p_auth_tx->acq_inst_id_len[0];
      strcpy(new_string,p_auth_tx->acq_inst_id_cd);
   }
    
   /* If odd length, provide a leading zero. */
	if (1 == (strlen(new_string) % 2) )
   {
      memset (temp_str, 0, sizeof(temp_str));
      temp_str [0] = '0';
      strcpy( &temp_str[1], new_string );
      memset (new_string, 0, sizeof(new_string));
      strcpy( new_string,temp_str);
   }
   asc_to_vbcd (new_string, sizeof(HOST_MESSAGE.acq_inst_id)*2,
                HOST_MESSAGE.acq_inst_id);

   /* FIELD 34: PAN EXTENDED */
   if ( 1 == (strlen(p_auth_tx->pan_extended) % 2) )
   {
      memset (temp_str, 0, sizeof(temp_str));
      temp_str[0] = '0';
      memcpy (&temp_str[1], p_auth_tx->pan_extended, strlen(p_auth_tx->pan_extended));
      str_asc_to_bcd( temp_str, strlen(temp_str), HOST_MESSAGE.pan_ext );
	  //memcpy(HOST_MESSAGE.pan_ext,temp_str, strlen(temp_str));
	   HOST_MESSAGE.pan_ext_len[0] = strlen( temp_str );
   }
   else
   {
	   str_asc_to_bcd (p_auth_tx->pan_extended, sizeof(HOST_MESSAGE.pan_ext)*2, HOST_MESSAGE.pan_ext);
	   //memcpy(HOST_MESSAGE.pan_ext, p_auth_tx->pan_extended, strlen(p_auth_tx->pan_extended));
	   HOST_MESSAGE.pan_ext_len[0] = strlen( p_auth_tx->pan_extended );
   }

   //for sbatch in TLF01
  /* if(0 == strncmp(p_auth_tx->TLF01_details.handler_queue,"sbatch", 6) &&
		   0 == strncmp(p_auth_tx->TLF01_details.profile, "05", 2))
   {
	   if (strlen(p_auth_tx->TLF01_details.product_codes[13].amount) > 0)
	   {   INT f34_len = 0;
		   memset (temp_str, 0, sizeof(temp_str));
		   memcpy (temp_str, p_auth_tx->TLF01_details.product_codes[13].amount, strlen(p_auth_tx->TLF01_details.product_codes[13].amount));
		   if (strlen(p_auth_tx->TLF01_details.product_codes[18].amount) > 0)
			   memcpy (&temp_str[12], p_auth_tx->TLF01_details.product_codes[18].amount, strlen(p_auth_tx->TLF01_details.product_codes[18].amount));

		   //str_asc_to_bcd (temp_str, sizeof(HOST_MESSAGE.pan_ext)*2, HOST_MESSAGE.pan_ext);
		   genutil_asc_to_bin(temp_str, HOST_MESSAGE.pan_ext, &f34_len);
		   //memcpy(HOST_MESSAGE.pan_ext, p_auth_tx->pan_extended, strlen(p_auth_tx->pan_extended));
	 	   //HOST_MESSAGE.pan_ext_len[0] = strlen( temp_str );
		   f34_len = 0;
		   memset(tempstr, 0x00, sizeof(tempstr));
		   sprintf( tempstr, "%04X", strlen( temp_str )/2 );
		   genutil_asc_to_bin( tempstr, HOST_MESSAGE.pan_ext_len, &f34_len );
	   }
   }*/


   /* FIELD 35: TRACK 35 */
	if (p_auth_tx->TLF01_details.track2 [0] != '\0')
   {

      /* If odd length, prepend leading zero. */
      if ( 1 == (strlen(p_auth_tx->TLF01_details.track2) % 2) )
      {
         memset (temp_str, 0, sizeof(temp_str));
         temp_str [0] = '0';
         memcpy (&temp_str[1], p_auth_tx->TLF01_details.track2, strlen(p_auth_tx->TLF01_details.track2));
         asc_to_vbcd (temp_str, sizeof(HOST_MESSAGE.track2)*2, HOST_MESSAGE.track2);
         HOST_MESSAGE.track2_len[0] = strlen(p_auth_tx->TLF01_details.track2);
      }
      else
      {
         asc_to_vbcd (p_auth_tx->TLF01_details.track2, sizeof(HOST_MESSAGE.track2)*2, HOST_MESSAGE.track2);
         HOST_MESSAGE.track2_len[0] = strlen(p_auth_tx->TLF01_details.track2);
      }
   }
    
  /* New logic for Feild 37 whatever comes from VISA .. TF... Phani*/
	/* FIELD 37: RETRIEVAL REFERENCE NUMBER */
	if ( p_auth_tx->TLF01_details.orig_retrieval_ref_num [0] != 0x00 )
      memcpy( HOST_MESSAGE.rrn,
              p_auth_tx->TLF01_details.orig_retrieval_ref_num,
              sizeof(HOST_MESSAGE.rrn) );
   else
      memcpy( HOST_MESSAGE.rrn,
              p_auth_tx->TLF01_details.retrieval_ref_num,
              sizeof(HOST_MESSAGE.rrn) );

	


   /* FIELD 39: RESPONSE CODE */
   if ( p_auth_tx->TLF01_details.tx_key == AUTH_REVERSAL_RESPONSE )
   {
      if ( 0 == strcmp(p_auth_tx->TLF01_details.response_code,TIMED_OUT) )
      {
         /* This response code is not allowed in an 0410 response.
          * Instead, use 'no action taken - unable to back out prior txn'.
          * Refer to SCR 764.
          */
         strcpy( p_auth_tx->TLF01_details.response_code, NO_ACTION_TAKEN );
      }
   }

   if ( txn_flow == SWITCHED_IN )
   {
      if ( (0 == strcmp("59",pos_cc)) &&
           (0 == strcmp(CALL_ISSUER,p_auth_tx->TLF01_details.response_code)) )
      {
         /* For e-commerce transactions, switched in, do not send response
          * code "01". This causes reject code 87. This is invalid. Set it to:
          *
          *    61 if exceeds a velocity limit (default)
          *    14 if card record is not found.
          */
         if ( NULL != strstr(p_auth_tx->TLF01_details.response_code,"account not found"))
            strcpy( p_auth_tx->TLF01_details.response_code, ACCT_NOT_FOUND );
         else
            strcpy( p_auth_tx->TLF01_details.response_code, EXCEEDS_VELOCITY );
      }
   }

   if(  p_auth_tx->TLF01_details.tx_key == AUTH_ADVICE || p_auth_tx->TLF01_details.tx_key == AUTH_ADVICE_REPEAT ||
		p_auth_tx->TLF01_details.tx_key == AUTH_REVERSAL_ADVICE || p_auth_tx->TLF01_details.tx_key == AUTH_REVERSAL_ADVICE_REPEAT)
   {
	   strcpy(p_auth_tx->TLF01_details.response_code,"00");
	   memcpy( HOST_MESSAGE.responsecd,
       p_auth_tx->TLF01_details.response_code,
            bf39_len );
   }
   memcpy( HOST_MESSAGE.responsecd, p_auth_tx->TLF01_details.response_code, 2 );


   /* FIELD 38: AUTHORIZATION IDENTIFICATION RESPONSE 
    * Populate this field only when the response code is Zero
    */
   if(p_auth_tx->TLF01_details.tx_key > AUTH_INVALID)
   {
      if ( strcmp(p_auth_tx->TLF01_details.response_code,"00") == 0 )
         memcpy( HOST_MESSAGE.authcode,
                 p_auth_tx->TLF01_details.auth_number,
                 sizeof(HOST_MESSAGE.authcode) );
   }
   else
      memcpy( HOST_MESSAGE.authcode,
              p_auth_tx->TLF01_details.auth_number,
              sizeof(HOST_MESSAGE.authcode) );

	if(0 == strncmp(p_auth_tx->TLF01_details.message_type, "0120", 4) ||
	   0 == strncmp(p_auth_tx->TLF01_details.message_type, "0121", 4) ||
	   0 == strncmp(p_auth_tx->TLF01_details.message_type, "0420", 4) ||
	   0 == strncmp(p_auth_tx->TLF01_details.message_type, "0421", 4))
   {
	   if ( p_auth_tx->TLF01_details.product_codes[8].quantity[0] == '1'  &&
			0 == strncmp(p_auth_tx->TLF01_details.handler_queue, "sbatch", 6))
	   {
	            memset( HOST_MESSAGE.authcode,
	                    0x00,
	                    sizeof(HOST_MESSAGE.authcode) );
	   }
   }

   //For Advice txns, send DE38 if present in request for Sbatch - ATP119AMC-5
   if(0 == strncmp(p_auth_tx->TLF01_details.message_type, "0121", 4))
   {
	   if ( p_auth_tx->TLF01_details.product_codes[8].quantity[0] == '0'  &&
			0 == strncmp(p_auth_tx->TLF01_details.handler_queue, "sbatch", 6))
	   {
	            memcpy( HOST_MESSAGE.authcode,
	            		p_auth_tx->TLF01_details.auth_number,
	                    sizeof(HOST_MESSAGE.authcode) );
	   }
   }

   /* FIELD 41: CARD ACCEPTOR TERMINAL ID */
   if( (( 0 == strlen(p_auth_tx->TLF01_details.terminal_id)) &&
   		( 0 == strncmp (p_auth_tx->TLF01_details.handler_queue,"dcp",3)))&&
		( ( 0 == strncmp (p_auth_tx->TLF01_details.message_type,"0100",4))||
		  ( 0 == strncmp (p_auth_tx->TLF01_details.message_type,"0200",4))))
   {
	   memcpy(HOST_MESSAGE.termid,VISA_DEFAULT_TID,VISA_DEFAULT_TID_LEN);
	   memset(p_auth_tx->TLF01_details.terminal_id,0x00,VISA_DEFAULT_TID_LEN);
	   strncpy(p_auth_tx->TLF01_details.terminal_id,VISA_DEFAULT_TID,VISA_DEFAULT_TID_LEN);
   }
   else if (strlen(p_auth_tx->TLF01_details.terminal_id) < 8)
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
	memset(HOST_MESSAGE.cardaccid, 0x20, sizeof(HOST_MESSAGE.cardaccid));
    memcpy(HOST_MESSAGE.cardaccid, p_auth_tx->TLF01_details.merchant_id, strlen(p_auth_tx->TLF01_details.merchant_id));	

   /* FIELD 43: CARD ACCEPTOR NAME/LOCATION */

   /* Store DE43 here so it gets logged to TLF01:
       *   Merchant Name         --> TLF01.card_holder_name
       *   City and Country Code --> TLF01.ncf01_retired_cwk
       * SCR 775
       */
   /*Abhishek change for field 43 , fixing issue of Merchant name in sbatch*/
    if(strncmp(p_auth_tx->TLF01_details.handler_queue,"sbatchA",4)==0 ||
       strncmp(p_auth_tx->TLF01_details.handler_queue,parvbh_que_name,7)==0)
	{

	 /* Store the Merchant name */
	  memcpy(HOST_MESSAGE.cardaccname, p_auth_tx->TLF01_details.card_holder_name,25);
      memcpy( HOST_MESSAGE.cardaccname+25,p_auth_tx->TLF01_details.ncf01_retired_cwk,13 );
	  memcpy( HOST_MESSAGE.cardaccname+38, "PH", 2);
    }
    else if(!((IsFieldNull(p_auth_tx->TLF01_details.card_holder_name))||
	   (IsFieldNull(p_auth_tx->TLF01_details.ncf01_retired_cwk))))
    {
	   memcpy( HOST_MESSAGE.cardaccname, p_auth_tx->TLF01_details.card_holder_name,
                 25); //Neetu

       memcpy( HOST_MESSAGE.cardaccname+25, p_auth_tx->TLF01_details.ncf01_retired_cwk,
                 15); //Neetu
    }
  
    else

    {  //Neetu

		if ( txn_flow == SWITCHED_OUT )
		{
			 /* Only send this field for Switched out transactions. */
			memset(HOST_MESSAGE.cardaccname, 0x20, sizeof(HOST_MESSAGE.cardaccname));
			 if ( iIndustry == ATM )
			{
			/* Transaction is ATM. */
			memcpy( HOST_MESSAGE.cardaccname, p_auth_tx->MCF01_details.address01,
				 strlen(p_auth_tx->MCF01_details.address01));	
			}
		  else
		  {
			 /* Transaction is a POS transaction. */
			 if(strcmp(p_auth_tx->MCF01_details.name02,"")!=0)
			{
				memcpy( HOST_MESSAGE.cardaccname, p_auth_tx->MCF01_details.name02,
			    strlen(p_auth_tx->MCF01_details.name02));
			}
		  }
      len = strlen(p_auth_tx->MCF01_details.city);
      if ( len > 13 )
         len = 13;
      memcpy( HOST_MESSAGE.cardaccname+25,
              p_auth_tx->MCF01_details.city,
              len );
      memcpy( HOST_MESSAGE.cardaccname+38, "PH", 2);

      memcpy( p_auth_tx->TLF01_details.card_holder_name,
              HOST_MESSAGE.cardaccname,
              25 );
      memcpy( p_auth_tx->TLF01_details.ncf01_retired_cwk,
              HOST_MESSAGE.cardaccname+25,
              15 );
   }
 } //Neetu

   /* FIELD 44: ADDITIONAL RESPONSE DATA */
   if (p_auth_tx->TLF01_details.tx_key > AUTH_INVALID)  
   {  /* response */

      /* Always set field 44.1 to '5'.  This indicates the issuer
       * has set the response code.
       */
      HOST_MESSAGE.addlrspdata [0] ='5';
      HOST_MESSAGE.addl_rsp_len[0] = 1;

      memset( &HOST_MESSAGE.addlrspdata[1], ' ', 10 );
      if ( (p_auth_tx->TLF01_details.verify_mag_stripe[0]      == 'Y' ) &&
           (p_auth_tx->TLF01_details.security_response_code[0] != 0x00)  )
      {
         /* CVV and/or CVV2 was validated with success or failure. */
         rcode = atoi( p_auth_tx->TLF01_details.security_response_code );

         /*  Security Response Code 00 = CVV Successful, CVV2 not processed.
          *                         01 = CVV Failed, CVV2 not processed.
          *                         02 = CVV2 Failed, CVV success if present
          *                         03 = CVV and CVV2 Successful
          */
         if ( (p_auth_tx->TLF01_details.track2[0] != 0x00) ||
              (p_auth_tx->TLF01_details.track1[0] != 0x00)  )
         {
            /* Set CVV Result Code */
            HOST_MESSAGE.addl_rsp_len[0] = 5;
            switch( rcode )
            {
               case 0:
               case 2:
               case 3:  HOST_MESSAGE.addlrspdata[4] = '2';  break;
               case 1:  HOST_MESSAGE.addlrspdata[4] = '1';  break;
            }
         }

         if ( p_auth_tx->TLF01_details.product_codes[17].quantity[1] == '.' )
         {
            /* Set CVV2 Result Code */
            HOST_MESSAGE.addl_rsp_len[0] = 11;
            switch( rcode )
            {
               case 0:
               case 1:  HOST_MESSAGE.addlrspdata[CVV2_RESULT_LOC] = 'P';  break;
               case 3:  HOST_MESSAGE.addlrspdata[CVV2_RESULT_LOC] = 'M';  break;

               case 2:
                        if ( p_auth_tx->TLF01_details.product_codes[17].quantity[0] == ' ')
                        {
                           HOST_MESSAGE.addlrspdata[CVV2_RESULT_LOC] = 'N';
                        }
                        else
                        {
                           HOST_MESSAGE.addlrspdata[CVV2_RESULT_LOC] = 
                                 p_auth_tx->TLF01_details.product_codes[17].quantity[0];
                        }
                        break;
            }
         }
      }
      else if ( p_auth_tx->TLF01_details.product_codes[17].quantity[1] == '.' )
      {
         /* Request indicates the need for a CVV2 result.
          * Since CVV2 was not validated, populate it with a 'P'.
          */
         HOST_MESSAGE.addlrspdata[CVV2_RESULT_LOC] = 'P';
         HOST_MESSAGE.addl_rsp_len[0] = 11;
      }
   }else if( p_auth_tx->TLF01_details.tx_key == AUTH_ADVICE) // copy DE44.1 for 0120 msg 
   {
		strcpy(HOST_MESSAGE.addlrspdata,p_auth_tx->TLF01_details.resp_source);
        HOST_MESSAGE.addl_rsp_len[0] = strlen(p_auth_tx->TLF01_details.resp_source);
	
   }


   /* FIELD 45: TRACK 1 */
   if (p_auth_tx->TLF01_details.track1 [0] != '\0')
   {
      /* Track 1 is present.  Do not send out track 2. */
      HOST_MESSAGE.track2_len[0] = 0x00;

      HOST_MESSAGE.track1_len[0] = strlen(p_auth_tx->TLF01_details.track1);
      memcpy( HOST_MESSAGE.track1,
              p_auth_tx->TLF01_details.track1,
              HOST_MESSAGE.track1_len[0] );
   }

   /* FIELD 49: TRANSACTION CURRENCY CODE */
   memset(temp_str, 0, sizeof(temp_str));

  if ( p_auth_tx->TLF01_details.tx_key > 100 ) // Neetu: Currecy code field should be what ever is coming from visa dont change it
   {
		  if ( p_auth_tx->MCF01_details.region_nbr[0] != 0x00 )
			 memcpy( temp_str, p_auth_tx->MCF01_details.region_nbr, 3 );
		  else
		   strcpy( temp_str, p_auth_tx->TLF01_details.currency_code );
   }
  else if( strncmp(p_auth_tx->TLF01_details.handler_queue, "ncvis", 5)== 0)
  {
		if ( p_auth_tx->MCF01_details.region_nbr[0] != 0x00 )
			 memcpy( temp_str, p_auth_tx->MCF01_details.region_nbr, 3 );
		else	
		strcpy (temp_str, p_auth_tx->reconciliation_cur_code);
  }
  else if(0 == strncmp(p_auth_tx->TLF01_details.nfi_seq_nbr, SLE, 3))
  {
	  if (((strncmp(p_auth_tx->TLF01_details.handler_queue, "sbatchA", 4)==0) ||
		   (strncmp(p_auth_tx->TLF01_details.message_type, "0400", 4)==0) ||
		   (strncmp(p_auth_tx->TLF01_details.message_type, "0420", 4)==0)) &&
			   (strcmp(p_auth_tx->TLF01_details.dispensation.origin, "RO")==0))
		  strcpy (temp_str, p_auth_tx->TLF01_details.nfi_seq_nbr);
  }
   else if(  p_auth_tx->TLF01_details.currency_code[0] != '\0')
  {
		
		strcpy (temp_str, p_auth_tx->TLF01_details.currency_code);
  }
   else //Neetu  
   //strcpy (temp_str, p_auth_tx->TLF01_details.currency_code);  
   // TF Phani.. Txn currency code is saved..Changed to Reconcillation currency code which is original Txn currncy code.
  {
		strcpy (temp_str, p_auth_tx->reconciliation_cur_code);  
  }

  if((strncmp(p_auth_tx->TLF01_details.handler_queue,"sbatchA",4)==0)&&
		0 != strcmp( PESO, p_auth_tx->TLF01_details.nfi_seq_nbr ) &&
		0 != strcmp( USD, p_auth_tx->TLF01_details.nfi_seq_nbr ) &&
		(strcmp(p_auth_tx->TLF01_details.dispensation.origin,"RO")==0))
     {
	  	  strcpy (temp_str, p_auth_tx->TLF01_details.nfi_seq_nbr);
     }

   Rj_with_lead_zeros (temp_str, sizeof(HOST_MESSAGE.tran_curr_cd)*2);
   asc_to_bcd (temp_str, sizeof(HOST_MESSAGE.tran_curr_cd)*2, HOST_MESSAGE.tran_curr_cd);

   /* FIELD 51: CARDHOLDER BILLING CURRENCY CODE */
	if (p_auth_tx->ch_billing_curr_code [0] != '\0')
   {
      memset(temp_str, 0, sizeof(temp_str));
      strcpy (temp_str, p_auth_tx->ch_billing_curr_code);     
      Rj_with_lead_zeros (temp_str, sizeof(HOST_MESSAGE.card_curr_cd)*2);
      asc_to_bcd (temp_str, sizeof(HOST_MESSAGE.card_curr_cd)*2, HOST_MESSAGE.card_curr_cd);
   }
	/* If txn orginated form the dcpiso or Vocie no need to send this fld */
   if((strncmp(p_auth_tx->TLF01_details.handler_queue,"sbatchA",4)==0)&&
	   (strcmp(p_auth_tx->TLF01_details.dispensation.origin,"RO")==0))
	{
      //memcpy(HOST_MESSAGE.card_curr_cd,p_auth_tx->TLF01_details.product_codes[7].code,4); 
	  memset(temp_str, 0, sizeof(temp_str));
      strcpy (temp_str, p_auth_tx->TLF01_details.product_codes[7].code);     
      Rj_with_lead_zeros (temp_str, sizeof(HOST_MESSAGE.card_curr_cd)*2);
      asc_to_bcd (temp_str, sizeof(HOST_MESSAGE.card_curr_cd)*2, HOST_MESSAGE.card_curr_cd);
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
      asc_to_bcd ("2001010100000000", sizeof(HOST_MESSAGE.sec_cntl)*2,   
               HOST_MESSAGE.sec_cntl);                                   
   }

   /* FIELD 54: ADDITIONAL AMOUNT */
   

   /* This field is only for ATMs. */
   if ( 0 == strncmp(p_auth_tx->TLF01_details.handler_queue, "sbatch", 6) ||
		   p_auth_tx->additional_pos_information[0] == ATM_TERMINAL )
   {
      /* Only return balance if approved. */
      if ( 0 == strcmp(p_auth_tx->TLF01_details.response_code, APPROVED) )
      {
         /* We will return balance. Must return it in Issuer's currency.
          * That means if txn is Peso, set currency code (cc) to 608.
          * If txn is USD, set cc to 840.  For third currencies, use the
          * the account from ACF01. Default to USD, as that is what it
          * should be for a third currency.
          */
         memset( temp_currency_code, 0x00, sizeof(temp_currency_code) );
         if ( p_auth_tx->host2_src_fi_nbr[0] != 0x00 )
         {
            strcpy( temp_currency_code, p_auth_tx->host2_src_fi_nbr );
         }
         else if ( 0 == strcmp(PESO,p_auth_tx->TLF01_details.currency_code) )
            strcpy( temp_currency_code, p_auth_tx->TLF01_details.currency_code);

         else if ( 0 == strcmp(USD,p_auth_tx->TLF01_details.currency_code) )
            strcpy( temp_currency_code, p_auth_tx->TLF01_details.currency_code);

         else
         {
            acct_type = p_auth_tx->ACF01_details.primary_key.account_type[1];
            if ( acct_type == DOLLAR_ACCOUNT_TYPE )
               strcpy( temp_currency_code, USD );

            else if ( acct_type == PESO_ACCOUNT_TYPE )
               strcpy( temp_currency_code, PESO );

            else
               strcpy( temp_currency_code, USD );
         }

         count = 0;
         memset(new_string, 0, sizeof(new_string));
         if(0 != strlen(p_auth_tx->ACF01_details.cash_available))
         {
            memcpy( new_string+count,
                    p_auth_tx->TLF01_details.processing_code+2,
                    2 );
            count = count + 2;
            memcpy(new_string+count, "01", 2);
            count = count + 2;
            memcpy( new_string+count, temp_currency_code, 3 );
            count = count + 3;
            if ('-' == p_auth_tx->current_balance_sign)
            {
               memcpy(new_string+count, "D" , 1);
            }
            else
            {
               memcpy(new_string+count, "C" , 1);
            }
            count++;
            strcpy( temp_str, p_auth_tx->ACF01_details.cash_available );
            Rj_with_lead_zeros (temp_str, 12);
            memcpy(new_string+count, temp_str, 12);
            count = count + 12;
         }
         if (0 != strlen(p_auth_tx->ACF01_details.cash_limit))
         {
            memcpy( new_string+count,
                    p_auth_tx->TLF01_details.processing_code+2,
                    2 );
            count = count + 2;
            memcpy(new_string+count, "02", 2);
            count = count + 2;
            memcpy(new_string+count, temp_currency_code, 3);
            count = count + 3;
            if ('-' == p_auth_tx->available_balance_sign)
            {
               memcpy(new_string+count, "D" , 1);
            }
            else
            {
               memcpy(new_string+count, "C" , 1);
            }
            count++;
            strcpy (temp_str,p_auth_tx->ACF01_details.cash_limit); 
            Rj_with_lead_zeros (temp_str, 12);
            memcpy(new_string+count, temp_str, 12);
            count = count + 12;
         }
         memcpy( HOST_MESSAGE.addlamts,
                 new_string,
                 sizeof(HOST_MESSAGE.addlamts));
         HOST_MESSAGE.addl_len[0] = strlen(HOST_MESSAGE.addlamts);
      }
   }
	/*FIELD 55 DE55 EMV/CHIP data*/
 // if (p_auth_tx->EMV_details.emv_block[0] != 0x00)
   if ( emv_txn == true )
   {	
	   if( strncmp(p_auth_tx->TLF01_details.handler_queue, "ncvis", 5) == 0 || strcmp(p_auth_tx->TLF01_details.dispensation.origin,"RO")==0) 
	   {emv_len= p_auth_tx->EMV_details.emv_block[0];
		memcpy(HOST_MESSAGE.ICCdata_len,p_auth_tx->EMV_details.emv_block,emv_len+1);
	   }else
	   {//From BDO POS
			generate_VISA_EMV_data(emv_len,p_auth_tx->EMV_details.emv_block);
	   }

	}

   /*FIELD 56*/
   /*We need to build this only for sbatch transaction*/
   if(0 == strncmp(p_auth_tx->TLF01_details.product_codes[0].quantity,"56",2) &&
	  0 == strncmp(p_auth_tx->TLF01_details.handler_queue, "sbatch", 6))
   {
	   // We need to build 56 and send to Cadencie
	   temp_len = p_auth_tx->future_use_1[0];
	   if (temp_len > 0 && temp_len < sizeof(p_auth_tx->future_use_1))
	   {
		   memcpy( &HOST_MESSAGE.PAD_len[1],  p_auth_tx->future_use_1 , 1 );

		   memcpy( HOST_MESSAGE.PAD_data, p_auth_tx->future_use_1 +1, temp_len );
	   }

   }

   /* FIELD 60: ADDITIONAL POS INFORMATION */
  memset( tempstr, 0x00, sizeof(tempstr) ); //Neetu
  if(strlen(p_auth_tx->TLF01_details.product_codes[16].amount)> 0)
  {
	  memcpy(tempstr,p_auth_tx->TLF01_details.product_codes[16].amount,sizeof(p_auth_tx->TLF01_details.product_codes[16].amount)); //Neetu: Changed the code for Field 60
  }
  if(!(IsFieldNull(tempstr)))
  {
	   asc_to_bcd (tempstr, strlen(tempstr),
       HOST_MESSAGE.priv60); 
       HOST_MESSAGE.priv60_len[0] = (strlen(tempstr)+ 1) / 2;
    
  }  
  else
  {
   if ( p_auth_tx->TLF01_details.track2[0] != 0x00 )
   {
      /* Determine chip condition code (if relevant) for DE60.3 */
      (void)parse_track2( p_auth_tx, temp_str ); /* Gets service code */
      if ( (ServiceCode[0] == '2') || (ServiceCode[0] == '6') )
      {
         if ( pos_entry_type == POS_ENTRY_EMV_CHIP )
         {
            emv_chip_code = p_auth_tx->EMV_details.chip_condition_code[0];
            p_auth_tx->EMV_details.chip_condition_code[0] = '1';
         }
         else if ( pos_entry_type == POS_ENTRY_EMV_FALLBACK )
         {
            emv_chip_code = '2';
            p_auth_tx->EMV_details.chip_condition_code[0] = '2';
         }
         else if ( pos_entry_type == POS_ENTRY_EMV_MAGSTRIPE )
         {
            emv_chip_code = p_auth_tx->EMV_details.chip_condition_code[0];
         }
         else
         {
            emv_chip_code = '0';
         }

         /* Make sure chip code is not 0 for Service Code 2 or 6. */
         if ( (emv_chip_code != '1') && (emv_chip_code != '2') )
            emv_chip_code = '1';

         if ( p_auth_tx->EMV_details.chip_condition_code[0] == 0x00 )
            p_auth_tx->EMV_details.chip_condition_code[0] = '1';
      }
      else
      {
         emv_chip_code = '0';  /* Always for Service Code not 2 or 6 */
         if ( pos_entry_type == POS_ENTRY_EMV_CHIP )
         {
            p_auth_tx->EMV_details.chip_condition_code[0] = '1';
         }
         else if ( pos_entry_type == POS_ENTRY_EMV_FALLBACK )
         {
            p_auth_tx->EMV_details.chip_condition_code[0] = '2';
         }
         else if ( pos_entry_type != POS_ENTRY_EMV_MAGSTRIPE )
         {
            p_auth_tx->EMV_details.chip_condition_code[0] = '0';
         }
      }
   }
   if ( iIndustry == ECOMMERCE || (0==strncmp(p_auth_tx->TLF01_details.pos_condition_code, ECOM_POS_CC,2)))  /* Verified by visa VbV */
	{
	   strncpy(eCommerce_Indcator, p_auth_tx->int_net_facility_data+8, 2);
      memset( tempstr, 0x00, sizeof(tempstr) );
	   		strncpy( tempstr, "500000000040",12 );


      if ( eCommerce_Indcator[0] == 0x00 )
      {
         if ( p_auth_tx->TLF01_details.message_type[1] == '4' )
         {
            /* This is a reversal. Need to echo ECI.
             * It gets stored in settlement date (TLF01);
             * It is stored in BCH10 in product code 20.
             */
            strcpy(eCommerce_Indcator,p_auth_tx->TLF01_details.settlement_date);
         }
         else
         {
            /* Not a reversal. Use default value. */
            strcpy( eCommerce_Indcator, "07" );
         }
      }
      strncpy( tempstr+8, eCommerce_Indcator,2);

      /* Save ECI for reversals. */
      strcpy( p_auth_tx->TLF01_details.settlement_date, eCommerce_Indcator );

      if ( p_auth_tx->TLF01_details.terminal_type[1] == '7' )
         tempstr[1] = '5'; /* Terminal has Chip Read capability. */

      HOST_MESSAGE.priv60_len[0] = (strlen(tempstr)+ 1) / 2;

      str_asc_to_bcd (tempstr, sizeof(HOST_MESSAGE.priv60)*2,
                  HOST_MESSAGE.priv60);  
      strncpy(p_auth_tx->TLF01_details.product_codes[16].amount,tempstr,
       		(sizeof(p_auth_tx->TLF01_details.product_codes[16].amount)-1));

      /* Store here for Fraudguard. */
      memcpy( p_auth_tx->TLF01_details.nfi_name, tempstr,
              sizeof(p_auth_tx->TLF01_details.nfi_name)-1 );
	}
   else if ( emv_txn == true)
   {
      memset( tempstr, 0x00, sizeof(tempstr) );
      if( 0 == strncmp("0912",p_auth_tx->TLF01_details.pos_entry_mode,4))
      {
    	  strncpy( tempstr, "912000100010" ,12);
      }
      else if ( pos_entry_type == POS_ENTRY_EMV_CHIP || ( 0 == strncmp("007",p_auth_tx->TLF01_details.pos_entry_mode,3) ))
      {
    	  strncpy( tempstr, "050000100010" ,12);
      }
      if ( ncf01_i.pad_char[0] == 'E' )
      {
         /* Do this to make this Chip Txn Indicator
          * configurable while testing.
          */
         tempstr[6] = '1';
      }

      /* 60.3 = Chip Condition Code */
      if ( emv_chip_code == 0x00 )
      {
         /* EB wants this field to default to zero. */
         emv_chip_code = '0';
         p_auth_tx->EMV_details.chip_condition_code[0] = '0';
      }
      /* tempstr[2] = emv_chip_code; EB wants 60.3 set to 0. */

      HOST_MESSAGE.priv60_len[0] = (strlen(tempstr)+ 1) / 2;
      str_asc_to_bcd( tempstr, sizeof(HOST_MESSAGE.priv60)*2,
                      HOST_MESSAGE.priv60);
      strncpy(p_auth_tx->TLF01_details.product_codes[16].amount,tempstr,
       		(sizeof(p_auth_tx->TLF01_details.product_codes[16].amount)-1));

      /* Store here for Fraudguard. */
      memcpy( p_auth_tx->TLF01_details.nfi_name, tempstr,
              sizeof(p_auth_tx->TLF01_details.nfi_name)-1 );
   }
   else if ('Y' != p_auth_tx->TLF01_details.voice_auth[0])
   {
      if (0 == strcmp(p_auth_tx->TLF01_details.terminal_type,"01"))
      {
         /* Unattended gas pump */
    	  p_auth_tx->TLF01_details.product_codes[16].amount[0] = '3';
      }
      else if ( iIndustry == ATM ) 
      {
         /* ATM */
    	  p_auth_tx->TLF01_details.product_codes[16].amount[0] = ATM_TERMINAL;
      }
      else
      {
         /* Unspecified */
    	  p_auth_tx->TLF01_details.product_codes[16].amount[0] = '0';
      }

      /* Ignore the following for Ecommerce. */
      if ( iIndustry != ECOMMERCE )
      {
    	  if (p_auth_tx->TLF01_details.pos_entry_mode[2] == '2')
         {
        	 if( 0 == strncmp(p_auth_tx->TLF01_details.handler_queue,"dcp",3))
			 {
				 //magstripe
        		 memcpy( p_auth_tx->TLF01_details.product_codes[16].amount, "050000100010",12);
			 }
        	 else
			 {
				 /* Magnetic stripe read capability */
        		 p_auth_tx->TLF01_details.product_codes[16].amount[1] = '2';
			 }
         }
         else if ( iIndustry == ATM ) /* ATM */
         {
            /*  magnetic stripe read capability */
        	 p_auth_tx->TLF01_details.product_codes[16].amount[1] = '2';
         }
		else if ((p_auth_tx->TLF01_details.pos_entry_mode[2] == '1') &&  (0 == strncmp(p_auth_tx->TLF01_details.pos_condition_code,"00",2)))
         {
			   /*manual key in*/
			memcpy( p_auth_tx->TLF01_details.product_codes[16].amount, "050000100010", 12  );
		}
		else if ((p_auth_tx->TLF01_details.pos_entry_mode[2] == '1') &&
				(0 == strncmp(p_auth_tx->TLF01_details.pos_condition_code,"08",2)))
		{
		 	/*rps*/
		 	memcpy( p_auth_tx->TLF01_details.product_codes[16].amount, "010000000000", 12  );
         }
#if BDOR_68_REQ_1_VISA
		else if (true == ncvis2_check_if_sbatch_qualify_for_MobilePOS_NFC_Tap2Phone_trx_with_pin( p_auth_tx ))
		{
	    	  strncpy( p_auth_tx->TLF01_details.product_codes[16].amount,
	    			   VISA_MOBILE_NFC_TAP2PHONE_TXN_POS_DATA_WITH_PIN,
					   VISA_POS_DATA_LEN );
	    }
		else if (true == ncvis2_check_if_sbatch_qualify_for_MobilePOS_NFC_Tap2Phone_trx_without_pin( p_auth_tx ))
		{
	    	  strncpy( p_auth_tx->TLF01_details.product_codes[16].amount,
	    			   VISA_MOBILE_NFC_TAP2PHONE_TXN_POS_DATA_WITHOUT_PIN,
					   VISA_POS_DATA_LEN );
		}
		else if ((true == ncvis2_check_if_trx_is_visa_MobilePOS_NFC_Tap2Phone_trx_with_pin( p_auth_tx )))
		{
		  strncpy( p_auth_tx->TLF01_details.product_codes[16].amount,
				   VISA_MOBILE_NFC_TAP2PHONE_TXN_POS_DATA_WITH_PIN,
				   VISA_POS_DATA_LEN );
		}
		else if ((true == ncvis2_check_if_trx_is_visa_MobilePOS_NFC_Tap2Phone_trx_without_pin( p_auth_tx )))
		{
		  strncpy( p_auth_tx->TLF01_details.product_codes[16].amount,
				   VISA_MOBILE_NFC_TAP2PHONE_TXN_POS_DATA_WITHOUT_PIN,
				   VISA_POS_DATA_LEN );
		}
#endif
         else if ( pos_entry_type != POS_ENTRY_NORMAL )
         {
        	 p_auth_tx->TLF01_details.product_codes[16].amount[1] = '5';
        	 p_auth_tx->TLF01_details.product_codes[16].amount[2] = emv_chip_code;
         }
         else if((p_auth_tx->TLF01_details.pos_entry_mode[2] == '1') &&
                  		(0 == strncmp(p_auth_tx->TLF01_details.pos_condition_code,"05",2)))
          {
                  			 /* BDO PAY QR*/
                  	strncpy( p_auth_tx->TLF01_details.product_codes[16].amount, "010000000000", 12 );
         }
         else 
         {
            /* No magnetic stripe read capability */
        	 p_auth_tx->TLF01_details.product_codes[16].amount[1] = '1';
         }

         if ( pos_entry_type == POS_ENTRY_EMV_FALLBACK ||
			 pos_entry_type == POS_ENTRY_FALLBACK ||
			 pos_entry_type == POS_ENTRY_EMV_MAGSTRIPE)
         {
        	 memcpy( p_auth_tx->TLF01_details.product_codes[16].amount, "052000000010", 12 );
         	}
         }
      }			/* Oct Mandate changes 2021 */
   	  else if ('Y' == p_auth_tx->TLF01_details.voice_auth[0])
   	  {
   		  memcpy( p_auth_tx->TLF01_details.product_codes[16].amount, "010000000000", 12 );
   	  }

      if ( p_auth_tx->TLF01_details.terminal_type[1] == '7' )
      {
         /* Terminal has Chip Read capability. */
    	  p_auth_tx->TLF01_details.product_codes[16].amount[1] = '5';
      }

          asc_to_vbcd (p_auth_tx->TLF01_details.product_codes[16].amount,
                       strlen(p_auth_tx->TLF01_details.product_codes[16].amount),
                       HOST_MESSAGE.priv60);
          HOST_MESSAGE.priv60_len[0] = (strlen(p_auth_tx->TLF01_details.product_codes[16].amount)/2);

    	  /* Store here for Fraudguard. */
          memcpy( p_auth_tx->TLF01_details.nfi_name,
        		  p_auth_tx->TLF01_details.product_codes[16].amount,
                  sizeof(p_auth_tx->TLF01_details.nfi_name) -1);

   }

  if(0 == strncmp(p_auth_tx->TLF01_details.handler_queue,"dcp", 3))
  {
	if( p_auth_tx->TLF01_details.tx_key == AUTH_PRE_AUTHORIZATION ||
		p_auth_tx->TLF01_details.tx_key == AUTH_AUTHORIZATION ||
		p_auth_tx->TLF01_details.tx_key == AUTH_CARD_VERIFICATION)
	{
		/*This is a Auth transaction*/
		bcd_to_asc( HOST_MESSAGE.priv60_len, sizeof(HOST_MESSAGE.priv60_len),
				 	&visa_inc_temp_len[0], sizeof(visa_inc_temp_len));
		if( atoi(visa_inc_temp_len) >= 5)
		{
			/* Only updated data at 60 position 9 and 10 */
			HOST_MESSAGE.priv60[4] &= ~0xF0;
			if (strlen(p_auth_tx->TLF01_details.visa_tran_id) > 0)
				HOST_MESSAGE.priv60[4] |=  0x02;
			else
			HOST_MESSAGE.priv60[4] |=  0x00;
		}
		else
		{
			//  updated data and length at 60 position 9 and 10
			if (HOST_MESSAGE.priv60[4] == 0x00 &&
			    (strlen(p_auth_tx->TLF01_details.visa_tran_id) > 0)	)
			{
				HOST_MESSAGE.priv60[4] = 0x02;
			}
			else
			{
			HOST_MESSAGE.priv60[4] &= ~0xF0;
			HOST_MESSAGE.priv60[4] |=  0x00;
			}
			HOST_MESSAGE.priv60_len[0] = 0x05;
		}
	}
	else if ( RPS_flag == 1 ) //RPS
	{
		strncpy(F60_asci_value, "010000000000",12 );
		asc_to_vbcd (F60_asci_value,strlen(F60_asci_value),HOST_MESSAGE.priv60);
		HOST_MESSAGE.priv60_len[0] = (strlen(F60_asci_value)/2);
	}
 }

#if BDOR_68_REQ_3_VISA /*Swiftpass BDO Pay Credit Transactions*/

    if (true == ncvis2_check_if_sbatch_qualify_for_Swiftpass_BDO_Pay_credit_trx_with_pin(p_auth_tx))
    {
		strncpy( p_auth_tx->TLF01_details.product_codes[16].amount,
				 VISA_SWIFTPASS_BDO_PAY_CREDIT_TXN_POS_DATA,
				 VISA_POS_DATA_LEN );
        asc_to_vbcd (p_auth_tx->TLF01_details.product_codes[16].amount,
                     strlen(p_auth_tx->TLF01_details.product_codes[16].amount),
                     HOST_MESSAGE.priv60);
        HOST_MESSAGE.priv60_len[0] = (strlen(p_auth_tx->TLF01_details.product_codes[16].amount)/2);

	}
    else if (true == ncvis2_check_if_sbatch_qualify_for_Swiftpass_BDO_Pay_credit_trx_without_pin(p_auth_tx))
    {
		strncpy( p_auth_tx->TLF01_details.product_codes[16].amount,
				 VISA_SWIFTPASS_BDO_PAY_CREDIT_TXN_POS_DATA,
				 VISA_POS_DATA_LEN );
        asc_to_vbcd (p_auth_tx->TLF01_details.product_codes[16].amount,
                     strlen(p_auth_tx->TLF01_details.product_codes[16].amount),
                     HOST_MESSAGE.priv60);
        HOST_MESSAGE.priv60_len[0] = (strlen(p_auth_tx->TLF01_details.product_codes[16].amount)/2);

	}

	if(true == ncvis2_check_if_trx_is_visa_Swiftpass_BDO_Pay_credit_trx_with_pin(p_auth_tx))
	{
		strncpy( p_auth_tx->TLF01_details.product_codes[16].amount,
				 VISA_SWIFTPASS_BDO_PAY_CREDIT_TXN_POS_DATA,
				 VISA_POS_DATA_LEN );
        asc_to_vbcd (p_auth_tx->TLF01_details.product_codes[16].amount,
                     strlen(p_auth_tx->TLF01_details.product_codes[16].amount),
                     HOST_MESSAGE.priv60);
        HOST_MESSAGE.priv60_len[0] = (strlen(p_auth_tx->TLF01_details.product_codes[16].amount)/2);

	}
	else if(true == ncvis2_check_if_trx_is_visa_Swiftpass_BDO_Pay_credit_trx_without_pin(p_auth_tx))
	{
		strncpy( p_auth_tx->TLF01_details.product_codes[16].amount,
				 VISA_SWIFTPASS_BDO_PAY_CREDIT_TXN_POS_DATA,
				 VISA_POS_DATA_LEN );
        asc_to_vbcd (p_auth_tx->TLF01_details.product_codes[16].amount,
                     strlen(p_auth_tx->TLF01_details.product_codes[16].amount),
                     HOST_MESSAGE.priv60);
        HOST_MESSAGE.priv60_len[0] = (strlen(p_auth_tx->TLF01_details.product_codes[16].amount)/2);

	}
#endif

#if BDOR_68_REQ_5_VISA /*Ecommerce Entry Mode From Swiftpass BDOPay Transactions*/

	if (true == ncvis2_check_if_sbatch_qualify_for_ecommerce_swiftpass_BDOPay_trx(p_auth_tx))
	{
		   strncpy( p_auth_tx->TLF01_details.product_codes[16].amount,
					VISA_ECOMMERCE_SWIFTPASS_BDOPAY_TXN_POS_DATA,
					VISA_POS_DATA_LEN );
	       asc_to_vbcd (p_auth_tx->TLF01_details.product_codes[16].amount,
	                    strlen(p_auth_tx->TLF01_details.product_codes[16].amount),
	                    HOST_MESSAGE.priv60);
	       HOST_MESSAGE.priv60_len[0] = (strlen(p_auth_tx->TLF01_details.product_codes[16].amount)/2);

	}
	else if(true == ncvis2_check_if_trx_is_visa_ecommerce_swiftpass_BDOPay_trx (p_auth_tx))
	{
	   strncpy( p_auth_tx->TLF01_details.product_codes[16].amount,
				VISA_ECOMMERCE_SWIFTPASS_BDOPAY_TXN_POS_DATA,
				VISA_POS_DATA_LEN );
       asc_to_vbcd (p_auth_tx->TLF01_details.product_codes[16].amount,
                    strlen(p_auth_tx->TLF01_details.product_codes[16].amount),
                    HOST_MESSAGE.priv60);
       HOST_MESSAGE.priv60_len[0] = (strlen(p_auth_tx->TLF01_details.product_codes[16].amount)/2);

   }
#endif


 /* Store feild 60 data to be display on Pos data Feild in Tranlookup*/

 HOST_MESSAGE.priv60_len[0] = (strlen(p_auth_tx->TLF01_details.product_codes[16].amount)/2);
 bcd_to_asc( HOST_MESSAGE.priv60_len, sizeof(HOST_MESSAGE.priv60_len),
                 &templen[0], sizeof(templen));
 bcd_to_asc( HOST_MESSAGE.priv60, (atoi(templen)),
                  (&tempstr[0]), sizeof(tempstr));

	 memcpy( p_auth_tx->TLF01_details.product_codes[16].amount, tempstr,
			 sizeof(p_auth_tx->TLF01_details.product_codes[16].amount)-1);
	 strncpy(p_auth_tx->TLF01_details.product_codes[16].amount,tempstr,
		  		(sizeof(p_auth_tx->TLF01_details.product_codes[16].amount)-1));


   /* FIELD 61: OTHER AMOUNTS */
   if (p_auth_tx->TLF01_details.tx_key > AUTH_INVALID)  
   {  /* response */
	   asc_to_vbcd (p_auth_tx->TLF01_details.add_amounts, sizeof(HOST_MESSAGE.priv61)*2, HOST_MESSAGE.priv61);
	   HOST_MESSAGE.priv61_len[0] = strlen(p_auth_tx->TLF01_details.add_amounts);
   }
   else
   {
 	   asc_to_vbcd (p_auth_tx->TLF01_details.actual_amount, sizeof(HOST_MESSAGE.priv61)*2, HOST_MESSAGE.priv61);
   	   i = strlen(p_auth_tx->TLF01_details.actual_amount);
       if ( i < 12  ||  i > 36 )
          memset( &HOST_MESSAGE.priv61_len, 0x00, sizeof(HOST_MESSAGE.priv61_len) );
       else
       {
          /* Length is for BCD string, so divide by 2. */
          HOST_MESSAGE.priv61_len[0] = (strlen(p_auth_tx->TLF01_details.actual_amount)) / 2;
       }
   }
   

   /* FIELD 62: CUSTOM PAYMENT SERVICE (CPS) FIELDS */
   if(p_auth_tx->TLF01_details.tx_key > AUTH_INVALID || 
	  (strncmp(p_auth_tx->TLF01_details.handler_queue, "ncvis", 5) == 0))
   {  /* response */
      memcpy( &HOST_MESSAGE.priv62,
              p_auth_tx->int_net_facility_data,
              sizeof(HOST_MESSAGE.priv62 ));
      memcpy( HOST_MESSAGE.priv62_len,
              p_auth_tx->int_net_facility_len,
              sizeof(HOST_MESSAGE.priv62_len ));

   }
   else if (p_auth_tx->TLF01_details.tx_key < AUTH_INVALID)  
   {
      //if ( iIndustry == ATM )	// temporary fix for ATM transactions : NIRBHAY
      if ( p_auth_tx->TLF01_details.pin_block[0] != 0 )
      {
         /*+---------------------------------+
           | This field is in bitmap format: |
           |    Length:    1 byte            |
           |    Bit map:   8 bytes           |
           |    Field 1:   1 byte   EBCDIC   |
           |    Field 4:   1 byte   EBCDIC   |
           |    Field 5:   1 byte   BCD      |
           |                                 |
           | Field 1 is always set.          |
           | Fields 4 and 5 are set for Ind  |
           | codes 300 and 400 only.         |
           +---------------------------------+*/

         /* Initialize bitmap and field length. */
         memset( F62BitMap, 0x00, 8 );
         F62Len = 8;   /* Length of bitmap */

         if ( p_auth_tx->TLF01_details.message_type[1] != '4' )
         {
            /* Not a reversal */

            /* FIELD 62.1 - AUTH CHARACTERISTICS INDICATOR - always 'Y'. */
            HOST_MESSAGE.priv62[F62Len] = ascii_ebcdic_conv( 'Y', A2E );
            F62BitMap[0] |= 0x80;      /* Set bit 1 */
            F62Len += 1;


            /* if (iIndustry == 300)
            {
               // Hotel 
               HOST_MESSAGE.priv62[F62Len] = ascii_ebcdic_conv( 'H', A2E );
               F62Len += 1;
               asc_to_bcd( p_auth_tx->TLF01_details.duration, 2,
                           &HOST_MESSAGE.priv62[F62Len] );
               F62BitMap[0] |= 0x18;      // Set bits 4 and 5 
               F62Len += 1;
            }
            else if (iIndustry == 400)
            {
               // Auto 
               HOST_MESSAGE.priv62[F62Len] = ascii_ebcdic_conv( 'A', A2E );
               F62Len += 1;
               asc_to_bcd( p_auth_tx->TLF01_details.duration, 2,
                           &HOST_MESSAGE.priv62[F62Len] );
               F62BitMap[0] |= 0x18;      // Set bits 4 and 5 
               F62Len += 1;
            }
            */


         }
         else
         {
            /* This is a reversal. Echo 62.1 and 62.2
             * These are stored in a combination of mcard_banknet and invoice.
             * The first byte of mcard_banknet is 62.1, followed by 11 bytes
             * of 62.2.  The last 4 bytes of 62.2 are in invoice number.  This
             * was done in order to store this data into NCF30 (network_data and
             * invoice_nbr without requiring a database change.  There was no
             * available field in NCF30 large enough to hold 62.2
             *
             * Note 1:  Only send 62.1 if it is a valid, positive value.  It
             * cannot be 'N' or blank.  Valid values are: A E V C M I P.
             *
             * Note 2:  Only send 62.2 if it exists.  If reversal is due to a
             * late response from Visa (time out), it will not exist.
             */
            visa_char = p_auth_tx->TLF01_details.mcard_banknet[0];

            if ((visa_char != 'N') && (visa_char != 0x00) && (visa_char != ' '))
            {
               HOST_MESSAGE.priv62[F62Len] = ascii_ebcdic_conv( visa_char,A2E );
               F62BitMap[0] |= 0x80;      /* Set bit 1 */
               F62Len += 1;
            }

            if ( p_auth_tx->TLF01_details.mcard_banknet[1] != 0x00 )
            {
               memset(visa_tran, 0x00, sizeof(visa_tran) );
               memset(visa_tran_bcd, 0x00, sizeof(visa_tran_bcd) );

               visa_tran[0] = '0'; /* Right justify the tran id. */
               memcpy(visa_tran+1, p_auth_tx->TLF01_details.mcard_banknet+1,11);
               memcpy(visa_tran+12,p_auth_tx->TLF01_details.invoice_number,  4);

               F62BitMap[0] |= 0x40;     /* Set bit 2 */
               asc_to_bcd( visa_tran, 16, visa_tran_bcd );
               memcpy( HOST_MESSAGE.priv62+F62Len, visa_tran_bcd, 8 );
               F62Len += 8;
            }
         }
      }


      if(0 == strncmp(p_auth_tx->TLF01_details.handler_queue,"dcp", 3))
	  {
   		/* For Incremental Preauth we have to send transaction ID received for 1st Auth transaction*/
		if( p_auth_tx->TLF01_details.tx_key == AUTH_PRE_AUTHORIZATION ||
			p_auth_tx->TLF01_details.tx_key == AUTH_AUTHORIZATION ||
			p_auth_tx->TLF01_details.tx_key == AUTH_CARD_VERIFICATION ||
			(p_auth_tx->TLF01_details.tx_key == AUTH_REVERSAL &&
			 0 == strncmp(p_auth_tx->TLF01_details.product_codes[3].quantity, "PR", 2))||
			p_auth_tx->MCF01_details.rps_merchant[0]== 'Y' ||
			(true == If_Tx_is_visa_RPS_Asiapay_Transaction(p_auth_tx))||
			strncmp( p_auth_tx->TLF01_details.pos_condition_code,"08",2) == 0)
		{
			/*This is a Auth transaction,
			 * 	let check if we have transaction ID received from host*/
			if(strlen(p_auth_tx->TLF01_details.visa_tran_id) > 0)
			{
				if(F62Len == 0)
				{
					memset( F62BitMap, 0x00, 8 );
					F62Len = 8;   /* Length of bitmap */
					/* FIELD 62.1 - AUTH CHARACTERISTICS INDICATOR - always 'Y'. */
					HOST_MESSAGE.priv62[F62Len] = ascii_ebcdic_conv( 'Y', A2E );
					F62BitMap[0] |= 0x80;      /* Set bit 1 */
					F62Len += 1;
				}
				F62BitMap[0] |= 0x40;     /* Set bit 2 */
				visa_tran[0] = '0';        /* Right Justified*/
				memcpy(visa_tran +1 ,p_auth_tx->TLF01_details.visa_tran_id,15);
				asc_to_bcd( visa_tran, 16, visa_tran_bcd );
				memcpy( HOST_MESSAGE.priv62+F62Len, visa_tran_bcd, 8 );
				F62Len += 8;
			}
		}

	  }

      if(0 == strncmp(p_auth_tx->TLF01_details.handler_queue,"sbatch", 6))
      {
    	  memset( F62BitMap, 0x00, 8 );
    	  F62Len = 8;   /* Length of bitmap */

    	  if(p_auth_tx->TLF01_details.mcard_banknet[0] != '\0')
    	  {
    		  /* October Mandates 2024: VS-5 Reqt-F62.1 */
    		  F62BitMap[0] |= 0x80;
    		  HOST_MESSAGE.priv62[F62Len] = ascii_ebcdic_conv( p_auth_tx->TLF01_details.mcard_banknet[0], A2E );
    		  F62Len += 1;
    	  }

    	  if (strlen(p_auth_tx->TLF01_details.visa_tran_id) > 0)
    	  {
    		  	  /* Field 62.2 */
				F62BitMap[0] |= 0x40;     /* Set bit 2 */
				visa_tran[0] = '0';        /* Right Justified*/
				memcpy(visa_tran +1 ,p_auth_tx->TLF01_details.visa_tran_id,15);
				asc_to_bcd( visa_tran, 16, visa_tran_bcd );
				memcpy( HOST_MESSAGE.priv62+F62Len, visa_tran_bcd, 8 );
				F62Len += 8;
    	  }

    	  if (strlen(p_auth_tx->TLF01_details.product_codes[15].quantity) > 0)
    	  {

        	  //VISA4-April_Mandate_2024, Field 62.4
        	  HOST_MESSAGE.priv62[F62Len] = ascii_ebcdic_conv( p_auth_tx->TLF01_details.product_codes[15].quantity[0], A2E );
        	  F62BitMap[0] |= 0x10; /*Set bit 4*/
        	  F62Len += 1;
    	  }

    	  //HCTS4
    	  if(strlen(p_auth_tx->TLF01_details.product_codes[2].amount) > 0)
    	  {
    		  memset(visa_mvv_bcd, 0x00, sizeof(visa_mvv_bcd) );
    		  F62BitMap[2] |= 0x10;
              genutil_asc_to_bin (p_auth_tx->TLF01_details.product_codes[2].amount,visa_mvv_bcd,&len);
              memcpy( HOST_MESSAGE.priv62+F62Len, visa_mvv_bcd, len );
    		  F62Len += 5;
    	  }

      }

      if(0 == strncmp(p_auth_tx->TLF01_details.handler_queue,"dcp", 3) ||
        0 == strncmp(p_auth_tx->TLF01_details.handler_queue,"VOICE", 5))
        {

          if( p_auth_tx->MCF01_details.network[8].priority_routing[0]!= '\0')
          {
        	//HCTS4
            memset(p_auth_tx->TLF01_details.product_codes[2].amount,0x00,sizeof(p_auth_tx->TLF01_details.product_codes[2].amount));
            strncpy(p_auth_tx->TLF01_details.product_codes[2].amount,p_auth_tx->MCF01_details.network[8].priority_routing,9);
            p_auth_tx->TLF01_details.product_codes[2].amount[9]= p_auth_tx->MCF01_details.network[9].priority_routing[0];
          }

          //HCTS4
          if(strlen(p_auth_tx->TLF01_details.product_codes[2].amount) > 0)
          {
        	    memset( F62BitMap, 0x00, 8 );
        	    F62Len = 8;
                memset(visa_mvv_bcd, 0x00, sizeof(visa_mvv_bcd) );
                F62BitMap[2] |= 0x10;
                genutil_asc_to_bin (p_auth_tx->TLF01_details.product_codes[2].amount,visa_mvv_bcd,&len);
                memcpy( HOST_MESSAGE.priv62+F62Len, visa_mvv_bcd, len );
                F62Len += 5;
            }
	  }

      //VISA4-April_Mandate_2024
      if(0 == strncmp(p_auth_tx->TLF01_details.handler_queue,"ncvis3", 6) &&
         0 == strncmp(p_auth_tx->TLF01_details.message_type,"0100", 4))
      {
    	  if(F62Len < 8)
    	  {
    		  F62Len = 8;
    	  }
    	  HOST_MESSAGE.priv62[F62Len] = ascii_ebcdic_conv( p_auth_tx->TLF01_details.product_codes[15].quantity[0], A2E );
    	  F62BitMap[0] |= 0x10; /*Set bit 4*/
    	  F62Len += 1;
      }

         memcpy( &HOST_MESSAGE.priv62, F62BitMap, 8 );
         HOST_MESSAGE.priv62_len[0] = F62Len;
         if(F62Len == 0 || F62Len == 8)
         {
        	 memset(HOST_MESSAGE.priv62_len,0x00,sizeof(HOST_MESSAGE.priv62_len));
        	 memset(HOST_MESSAGE.priv62,0x00,sizeof(HOST_MESSAGE.priv62));
         }
      }
   

   /* FIELD 63: V.I.P. PRIVATE USE FIELDS */

   /*+------------------------------------------------------------------+
     | This field is in bitmap format:                                  |
     |    Length:    1 byte                                             |
     |    Bit map:   3 bytes                                            |
     |    Field 1:   2 bytes (N, BCD) Network Id                        |
     |    Field 3:   2 bytes (N, BCD) Message Reason Code for reversals |
     |    Field 4:   2 bytes (N, BCD) STIP/Switch Reason Code           |
     |                                                                  |
     | Acquirer:                                                        |
     |    Field 1 is always '0000', Visa VAP will set this.             |
     |    Field 3 is for reversals: void/rev, timeouts, ATM Confirms    |
     |                                                                  |
     | Issuers:                                                         |
     |    Field 1 will echo value from Visa (0002 or 0004).             |
     |    Field 3 will echo value from Visa (2501, 2502, or 2503)       |
     |    Field 4 will echo value from Visa (0120 and 0420 only)        |
     +------------------------------------------------------------------+*/

  if (p_auth_tx->TLF01_details.tx_key < AUTH_INVALID)  
   {
      /* ------------------------ */
      /* Acquirer - switching out */
      /* ------------------------ */

      /* Initialize bitmap and field length. */
      memset( F63BitMap, 0x00, 3 );
      F63Len = 3;   /* Length of bitmap */

      /* FIELD 63.1 - NETWORK ID */

      if((strncmp(p_auth_tx->TLF01_details.handler_queue,"ncvis",5)==0)) // TF Phani and Chandra - Field 63 Changes
	  {

		   if ( p_auth_tx->TLF01_details.product_codes[10].quantity[0] == 0x01 ) 
		  {
			 /* Initialize bitmap and field length. */
			 memset( F63BitMap, 0x00, 3 );
			 F63Len = 3;   /* Length of bitmap */

			 /* FIELD 63.1 - NETWORK ID */
			 memset( temp_bcd, 0x00, sizeof(temp_bcd) );
			 asc_to_bcd( p_auth_tx->TLF01_details.product_codes[10].code,
						 4,
						 temp_bcd );
			 memcpy( HOST_MESSAGE.priv63+F63Len, temp_bcd, 2 );
			 F63Len += 2;
			 F63BitMap[0] |= 0x80;        /* Set bit 1    */

			 memcpy( &HOST_MESSAGE.priv63, F63BitMap, 3 );
			 HOST_MESSAGE.priv63_len[0] = F63Len;
		  }
     
	  }
	  else
	  {
		  memset( HOST_MESSAGE.priv63, 0x00, 2 ); /* Always 00,00 */
		  F63BitMap[0] |= 0x80;                   /* Set bit 1    */
		  F63Len += 2;
      }    
      /* FIELD 63.3 - MESSAGE REASON CODE */

	  if ( is_txn_reversal(p_auth_tx->TLF01_details.tx_key) )
      {
         /* Yes - this is a reversal of some sort. */
         F63_3_Present = true;
         F63_ByteTwo   = 0x00;

         if ( p_auth_tx->TLF01_details.tx_key == AUTH_ATM_CONFIRM )
         {
            /* The value of field 63.3 for ATM Confirms depends on the
             * values in field 61 and 4, determining is this is a partial
             * dispense or not:
             *
             *   ATM dispenses nothing:   Field 61 has zero,
             *                            Set as a regular reversal.
             *
             *   ATM dispense is partial: Field 61 is different than field 4,
             *                            Set 2504 (Partial dispense by ATM).
             *
             *   ATM dispenses full amt:  Field 61 is same as field 4,
             *                            Do not send field 63.3
             */
            total_amount  = atol(p_auth_tx->TLF01_details.total_amount );
            actual_amount = atol(p_auth_tx->TLF01_details.actual_amount);
            if ( actual_amount == total_amount )
            {
               /* Amount dispersed is same as total amount. Do not send 63.3 */
               F63_3_Present = false;
            }
            else if ( actual_amount != 0 )
            {
               /* Amt dispersed is different than total. Set Partial Dispense */
               F63_ByteTwo = 0x04;
            }
         }

        if ( F63_3_Present == true )
         {
			if(strncmp(p_auth_tx->TLF01_details.handler_queue,"ncvis",5) == 0)	// TF Phani..If request from VISA host..send as it is..
			{
				if (p_auth_tx->TLF01_details.product_codes[11].code[0] != '\0')
				{
					memset( temp_bcd, 0x00, sizeof(temp_bcd) );
					asc_to_bcd( p_auth_tx->TLF01_details.product_codes[11].code,
							 4,
							 temp_bcd );
					memcpy( HOST_MESSAGE.priv63+F63Len, temp_bcd, 2 );
					F63Len += 2;
					F63BitMap[0] |= 0x20;           /* Set bit 3    */
				}

			}
			else
			{
				HOST_MESSAGE.priv63[F63Len++] = 0x25;  /* Byte 1 */

				if ( F63_ByteTwo == 0x00 )
				{
				   if ( 0 == strcmp(p_auth_tx->TLF01_details.response_code,TIMED_OUT) )
				   {
					  /* Reversal is due to a time out. */
					  F63_ByteTwo = 0x02;
				   }
				   else if ( p_auth_tx->TLF01_details.tx_key == AUTH_VOID_SALE )
				   {
					  /* Voided transaction. */
					  F63_ByteTwo = 0x01;
				   }
				   else
				   {
					  /* Terminal initiated reversal. */
					  F63_ByteTwo = 0x03;
				   }
				}
				HOST_MESSAGE.priv63[F63Len++] = F63_ByteTwo;
				F63BitMap[0] |= 0x20;           /* Set bit 3    */
			}
         }
      }
	  /* For Incremental Preauth we have to send transaction ID received for 1st Auth transaction*/
	   else if(p_auth_tx->TLF01_details.tx_key == AUTH_PRE_AUTHORIZATION ||
			   p_auth_tx->TLF01_details.tx_key == AUTH_AUTHORIZATION ||
			   p_auth_tx->TLF01_details.tx_key == AUTH_CARD_VERIFICATION||
			   (true == If_Tx_is_visa_RPS_Asiapay_Transaction(p_auth_tx))||
			   strncmp( p_auth_tx->TLF01_details.pos_condition_code,"08",2) == 0)
		{
			/*This is a Auth transaction,
			 * 	let check if we have transaction ID received from host and coming from BDO POS*/
		   if(0 == strncmp(p_auth_tx->TLF01_details.handler_queue,"dcp", 3))
		   {
				if(strlen(p_auth_tx->TLF01_details.visa_tran_id) > 0)
				{
					 F63BitMap[0] |= 0x20;           /* Set bit 3    */
					 strncpy(visa_incremental_MC_asc,VISA_INCREMENTAL_DE_63_3,4);
					 asc_to_bcd( visa_incremental_MC_asc, 4, visa_incremental_MC_bcd );
					 memcpy(HOST_MESSAGE.priv63 +F63Len,visa_incremental_MC_bcd ,2);
					 F63Len = F63Len + 2;
				}
		   }
		}

	  if( (0 == strncmp(p_auth_tx->TLF01_details.handler_queue,"sbatch", 6)) &&
		  (strlen(p_auth_tx->TLF01_details.product_codes[13].code) > 0))
	  {
		  /* Fee Program Indicator*/
		  memset( temp_bcd, 0x00, sizeof(temp_bcd) );
		  asc_to_bcd( p_auth_tx->TLF01_details.product_codes[13].code,
				  	  4, temp_bcd );
		  memcpy( HOST_MESSAGE.priv63+F63Len, temp_bcd, 2 );
		  F63Len += 2;
		  F63BitMap[2] |= 0x20;           /* Set bit 3    */
	  }

      memcpy( &HOST_MESSAGE.priv63, F63BitMap, 3 );
      HOST_MESSAGE.priv63_len[0] = F63Len;
   }

   else
   {

   
      /* ------------------------ */
      /*   Issuer - switched in   */
      /* ------------------------ */

      /* Check to see if field 63 was present on request.
       * The only subfield we need to echo is Network Id.
       */
      if ( p_auth_tx->TLF01_details.product_codes[10].quantity[0] == 0x01 ) 
      {
         /* Initialize bitmap and field length. */
         memset( F63BitMap, 0x00, 3 );
         F63Len = 3;   /* Length of bitmap */

         /* FIELD 63.1 - NETWORK ID */
         memset( temp_bcd, 0x00, sizeof(temp_bcd) );
         asc_to_bcd( p_auth_tx->TLF01_details.product_codes[10].code,
                     4,
                     temp_bcd );
         memcpy( HOST_MESSAGE.priv63+F63Len, temp_bcd, 2 );
         F63Len += 2;
         F63BitMap[0] |= 0x80;        /* Set bit 1    */

         memcpy( &HOST_MESSAGE.priv63, F63BitMap, 3 );
         HOST_MESSAGE.priv63_len[0] = F63Len;
      }

	}
   

   /* FIELD 70: Network Management Info Code */
   if (0 == strlen(vLogon_NMI_Code))  
   {
      asc_to_bcd (p_auth_tx->function_code, sizeof(HOST_MESSAGE.nmi_code)*2,
                  HOST_MESSAGE.nmi_code);  
   }
   else
   {
      asc_to_bcd (vLogon_NMI_Code, sizeof(HOST_MESSAGE.nmi_code)*2,
               HOST_MESSAGE.nmi_code);  
   }

   /* FIELD 73: DATE, ACTION */
   if ( (p_auth_tx->TLF01_details.tx_key == AUTH_CARD_UPDATE) ||
        (p_auth_tx->TLF01_details.tx_key == AUTH_CARD_UPDATE_RESPONSE) )
   {
	   if(p_auth_tx->TLF01_details.processing_code[0] != '3' &&
		  p_auth_tx->TLF01_details.processing_code[0] != '5')
			  asc_to_bcd( p_auth_tx->TLF01_details.product_codes[0].amount,
                  sizeof(HOST_MESSAGE.action)*2,
                  HOST_MESSAGE.action );
   }

   /* FIELD 90: Get original data elements for field 90. */
   if(p_auth_tx->TLF01_details.tx_key == AUTH_REVERSAL_RESPONSE)
   {
        //ECHO FLD 90 Data
       memset (orig_data, 0, sizeof(orig_data));
       strcat (orig_data, p_auth_tx->TLF01_details.orig_message);   /* This is from the original 400/401 msg */
       strcat (orig_data, p_auth_tx->TLF01_details.orig_sys_trace_num);
       if(0    == strncmp(p_auth_tx->TLF01_details.product_codes[3].quantity,"PR",2) &&
		  '\0' != p_auth_tx->orig_local_time_date[0])
	   {
    	   memcpy( &orig_data[10], p_auth_tx->orig_local_time_date, 10);
	   }
       strcat (orig_data, p_auth_tx->orig_local_time_date);  
	   

	    for (count = 1; count <= (11-strlen(p_auth_tx->orig_acq_inst_id)); count++)
		    strcat(orig_data, "0");
	    strcat (orig_data, p_auth_tx->orig_acq_inst_id);

        for (count = 1; count <= (11-strlen(p_auth_tx->amex_pos_entry_mode)); count++)
		    strcat(orig_data, "0");
	    strcat (orig_data, p_auth_tx->amex_pos_entry_mode);
   }
   else
   {
      memset (orig_data, 0x00, sizeof(orig_data));
	  if(p_auth_tx->TLF01_details.tx_key == AUTH_REVERSAL_ADVICE || 
		 p_auth_tx->TLF01_details.tx_key == AUTH_REVERSAL_ADVICE_REPEAT 
		)
	  strcpy (orig_data, "0120");	
	  else
      strcpy (orig_data, "0100");
      strcat (orig_data, p_auth_tx->TLF01_details.orig_sys_trace_num);

      strcat (orig_data, p_auth_tx->orig_local_time_date);

      if ( p_auth_tx->TLF01_details.tx_key == AUTH_VOID_SALE )
      {
         if ( p_auth_tx->orig_acq_inst_id[0] == 0x00 )
         {
            //strcpy(p_auth_tx->orig_acq_inst_id, ncf01_i.acquirer_inst_id_code); //Neetu : Copy from actual field 32
             strcpy(p_auth_tx->orig_acq_inst_id,p_auth_tx->acq_inst_id_cd); //Neetu
		 }
      }
	  else if (p_auth_tx->TLF01_details.tx_key == AUTH_REVERSAL) 	//ThoughtFocus nirbhay : cadencie changes
	  {
		  strcpy(p_auth_tx->orig_acq_inst_id,p_auth_tx->acq_inst_id_cd); //ThoughtFocus nirbhay : cadencie changes
	  }
	  else if ( p_auth_tx->TLF01_details.tx_key == AUTH_VOID_PRE_AUTHORIZATION)
	  {
		  strcpy(p_auth_tx->orig_acq_inst_id, p_auth_tx->TLF01_details.acquiring_id, 12);
	  }
	  
      for (count = 1; count <= (11-strlen(p_auth_tx->orig_acq_inst_id)); count++)
         strcat(orig_data, "0");
      strcat (orig_data, p_auth_tx->orig_acq_inst_id);

      /* Forward institution id code : if present
       * Fill with zeros, if Forward institution id code is null.
       * Since we are never sending out DE33, this is always zeroes.
       */
      strncat( orig_data, "00000000000", 11 );
   }
   str_asc_to_bcd (orig_data, strlen(orig_data), HOST_MESSAGE.orig_data );

   /* FIELD 91: FILE UPDATE CODE (0302 Txns) */
   if ( p_auth_tx->TLF01_details.tx_key == AUTH_CARD_UPDATE )
   {
      HOST_MESSAGE.file_update[0] = p_auth_tx->TLF01_details.processing_code[0];
   }
   else if ( p_auth_tx->TLF01_details.tx_key == AUTH_CARD_UPDATE_RESPONSE )
   {
      HOST_MESSAGE.file_update[0] = 
            ascii_ebcdic_conv(p_auth_tx->TLF01_details.processing_code[0], A2E);
   }

   /* FIELD 93 RESPONSE INDICATOR */
   memcpy( HOST_MESSAGE.resp_ind, vLogon_Resp_Indicator, sizeof(HOST_MESSAGE.resp_ind ));

   /* FIELD 95 REPLACEMENT AMOUNTS */

   /* This field is used only for partial reversals. */
   if ( p_auth_tx->TLF01_details.message_type[1] == '4' )
   {
	   /* Girija Y TF, March 24, 2009, Apr 2009 Mandates */
	  if( 0 == strncmp( p_auth_tx->TLF01_details.message_type, "0400",4) &&
		  0 != strncmp(p_auth_tx->TLF01_details.product_codes[3].quantity,"PR",2) &&
		  AUTH_VOID_PRE_AUTHORIZATION != p_auth_tx->TLF01_details.tx_key)

	  {
		  memcpy (HOST_MESSAGE.repl_amts, p_auth_tx->TLF01_details.actual_amount,12);
		  //if(p_auth_tx->TLF01_details.tx_key == AUTH_ATM_CONFIRM )
		  //  {
		  /* If totals_ind is 'E' copy the repls ammount TF-Ajay */
			 if ( p_auth_tx->MCF01_details.totals_ind[0] == 'E' )
			{
				memcpy( HOST_MESSAGE.repl_amts,
						p_auth_tx->MCF01_details.address01,
						30 );
				memcpy( HOST_MESSAGE.repl_amts+30,
						p_auth_tx->MCF01_details.address02,
						12 );
			 }
		  //}
	  }

      else if(  (atol(p_auth_tx->TLF01_details.actual_amount) == 0) &&
           ((p_auth_tx->TLF01_details.tx_key == AUTH_REVERSAL) ||
             (p_auth_tx->TLF01_details.tx_key == AUTH_REVERSAL_REPEAT)) &&
			 (0 != strncmp(p_auth_tx->TLF01_details.product_codes[3].quantity,"PR",2)))
      {
          //don't send fld95
      }
      else if(0 == strncmp(p_auth_tx->TLF01_details.product_codes[3].quantity,"PR",2))
	  {
		  memset( new_string, 0x00, sizeof(new_string) );
		  strncpy(new_string, p_auth_tx->TLF01_details.total_amount, 12);
		              for (count = 12; count < 42; count++)
		                 new_string[count] = '0';    /* zero fill the last 30+ digits */
		  memcpy (HOST_MESSAGE.repl_amts, new_string, sizeof(HOST_MESSAGE.repl_amts));
	  }
      else if ( p_auth_tx->TLF01_details.tx_key > 100 )
      {
         /* This is a response.  Echo DE95 if it was in the request. */
         if ( p_auth_tx->MCF01_details.totals_ind[0] == 'E' )
         {
            memcpy( HOST_MESSAGE.repl_amts,
                    p_auth_tx->MCF01_details.address01,
                    30 );
            memcpy( HOST_MESSAGE.repl_amts+30,
                    p_auth_tx->MCF01_details.address02,
                    12 );
         }
      }
      else
      {
         total_amount    = atol(p_auth_tx->TLF01_details.total_amount);
         reversal_amount = atol(p_auth_tx->TLF01_details.reversal_amount);
         if ( (reversal_amount < total_amount) &&
              (reversal_amount > 0           )  )
         {
            memset( new_string, 0x00, sizeof(new_string) );
            sprintf( new_string, "%012d", total_amount-reversal_amount );
            for (count = 12; count < 42; count++)
               new_string[count] = '0';    /* zero fill the last 30+ digits */

            memcpy (HOST_MESSAGE.repl_amts, new_string,
                    sizeof(HOST_MESSAGE.repl_amts));
         }
      }
   }

#if BDOR_62_PREAUTH_VOID
      else if( AUTH_VOID_PRE_AUTHORIZATION == p_auth_tx->TLF01_details.tx_key)
      {
          total_amount    = atol(p_auth_tx->TLF01_details.total_interest);
          reversal_amount = atol(p_auth_tx->TLF01_details.total_amount);
          if (p_auth_tx->TLF01_details.saf[0] == INCREMENTAL_AUTH)
          {
             memset( new_string, 0x00, sizeof(new_string) );
             sprintf( new_string, "%012d", total_amount-reversal_amount );
             for (count = 12; count < 42; count++)
                new_string[count] = '0';    /* zero fill the last 30+ digits */

          }
          else
          {
        	  	  	  /* PRIMARY_AUTH*/
              memset( new_string, 0x00, sizeof(new_string) );
              for (count = 0; count < 42; count++)
                 new_string[count] = '0';    /* zero fill the whole 42 digits */
          }
          memcpy (HOST_MESSAGE.repl_amts, new_string,
                  sizeof(HOST_MESSAGE.repl_amts));
      }
#endif

   /* FIELD 101: FILE NAME */
   if ( p_auth_tx->TLF01_details.tx_key == AUTH_CARD_UPDATE )
   {
      len = strlen( p_auth_tx->TLF01_details.kek );
      memcpy( HOST_MESSAGE.filename, p_auth_tx->TLF01_details.kek, len );
      HOST_MESSAGE.fname_len[0] = len;
   }
   else if ( p_auth_tx->TLF01_details.tx_key == AUTH_CARD_UPDATE_RESPONSE )
   {
      len = strlen( p_auth_tx->TLF01_details.kek );
      for( i=0; i<len; i++ )
      {
         HOST_MESSAGE.filename[i] = 
                      ascii_ebcdic_conv(p_auth_tx->TLF01_details.kek[i], A2E);
      }
      HOST_MESSAGE.fname_len[0] = len;
   }

   /* FIELD 102: ACCOUNT ID 1 */
   if ( p_auth_tx->acct_id_1[0] != 0x00 )
   {
      if ( p_auth_tx->TLF01_details.tx_key > AUTH_INVALID )
      {
         /* Response */
         if ( p_auth_tx->FAF01_details.velocity[0].amount[0] == '1' )
         {
            /* This flag indicates that this field came in the request. */
            HOST_MESSAGE.acct_id_1_len[0] = strlen(p_auth_tx->acct_id_1 );
            memcpy( HOST_MESSAGE.acct_id_1,
                    p_auth_tx->acct_id_1,
                    HOST_MESSAGE.acct_id_1_len[0] );
         }
      }
      else
      {
         HOST_MESSAGE.acct_id_1_len[0] = strlen(p_auth_tx->acct_id_1 );
         memcpy( HOST_MESSAGE.acct_id_1,
                 p_auth_tx->acct_id_1,
                 HOST_MESSAGE.acct_id_1_len[0] );
      }
   }

   /* FIELD 103: ACCOUNT ID 2 */
   if ( p_auth_tx->acct_id_2[0] != 0x00 )
   {
      if ( p_auth_tx->TLF01_details.tx_key > AUTH_INVALID )
      {
         /* Response */
         if ( p_auth_tx->FAF01_details.velocity[0].amount[1] == '1' )
         {
            /* This flag indicates that this field came in the request. */
            HOST_MESSAGE.acct_id_2_len[0] = strlen( p_auth_tx->acct_id_2 );
            memcpy( HOST_MESSAGE.acct_id_2,
                    p_auth_tx->acct_id_2,
                    HOST_MESSAGE.acct_id_2_len[0] );
         }
      }
      else
      {
         HOST_MESSAGE.acct_id_2_len[0] = strlen( p_auth_tx->acct_id_2 );
         memcpy( HOST_MESSAGE.acct_id_2,
                 p_auth_tx->acct_id_2,
                 HOST_MESSAGE.acct_id_2_len[0] );
      }
   }

   /* FIELD 120 ORIGINAL MESSAGE ID */
	str_asc_to_bcd ("020100", sizeof(HOST_MESSAGE.org_msg_id)*2, HOST_MESSAGE.org_msg_id);

   /* FIELD 121: ISSUING INSTITUTION ID CODE */
   HOST_MESSAGE.iss_inst_len[0] = strlen( p_auth_tx->iss_inst_id_cd);
   memcpy( HOST_MESSAGE.iss_inst_id, p_auth_tx->iss_inst_id_cd, HOST_MESSAGE.iss_inst_len[0]); 
     
	/* FIELD 126: Visa Private Use Fields */
   F126Len = F126_BITMAP_LEN;
	if ( iIndustry == ECOMMERCE )	
	{
      /*+------------------------------------------------------------------+
        | Visa VbV data = CAVV Data and Transtain (Visa Tran Id)           |
        | ------------------------------------------------------           |
        | 1.  Transaction ID                                               |
        |     1.1  Field 126.8                                             |
        |     1.2  Fixed length, binary, 20 bytes                          |
        | 2.  CAVV Data                                                    |
        |     2.1  Field 126.9                                             |
        |     2.2  Fixed length, binary, 20 bytes                          |
        |                                                                  |
        | Field 126 is a bitmapped field. VbV data is in subelements 8 & 9 |
        | So the bitmap for only VbV data will look like:                  |
        |     01 80 00 00 00 00 00 00                                      |
        |                                                                  |
        +------------------------------------------------------------------+*/

		 /* 126.10 - CVC2 */
   if(strcmp(p_auth_tx->TLF01_details.nfi_end_date,"1" == 0) &&  strncmp (p_auth_tx->TLF01_details.handler_queue, "ncvis",5) == 0)
   {
		// 3 D secure tx with CAVV data. Send data as it is from VISA
		/* BYTE temp_cavv[241]="";
		 F126Len = (INT)p_auth_tx->TLF01_details.voice_auth_text[0];
		 memcpy(HOST_MESSAGE.de126,&p_auth_tx->TLF01_details.voice_auth_text[1],F126Len);	
		 genutil_bin_to_ascii(&p_auth_tx->TLF01_details.voice_auth_text[9],(F126Len-8),temp_cavv,sizeof(temp_cavv));
		 memcpy(p_auth_tx->TLF01_details.voice_auth_text,temp_cavv,sizeof(temp_cavv)-1);
		*/
			if('\0' != p_auth_tx->TLF01_details.product_codes[4].amount[0])
			{
				 len =0;
				 HOST_MESSAGE.de126[0] ^= 0x08; /* Set bitmap - bit 5 */
				 for( i=0; i<8; i++ )
				 {
					 temp_str[i] =
					   ascii_ebcdic_conv( p_auth_tx->TLF01_details.product_codes[4].amount[i], A2E );
				 }
				 len=i;
				 memcpy( &HOST_MESSAGE.de126[F126Len], temp_str, len );
				 F126Len += len;
				 len = 0;

			}
			if('\0' != p_auth_tx->TLF01_details.operator_id[0])
			{
				len =0;
				 HOST_MESSAGE.de126[0] ^= 0x01; /* Set bitmap - bit 8 */
				 genutil_asc_to_bin(p_auth_tx->TLF01_details.operator_id,temp_str,&len);
				 memcpy( &HOST_MESSAGE.de126[F126Len], temp_str, len );
				 F126Len += len;
				 len = 0;
				 genutil_asc_to_bin(p_auth_tx->TLF01_details.workstation,temp_str,&len);
				 memcpy( &HOST_MESSAGE.de126[F126Len], temp_str, len );
				 F126Len += len;
			}

			if('\0' != p_auth_tx->TLF01_details.dcf01_retired_cwk[0] &&
				'C' != p_auth_tx->TLF01_details.dcf01_retired_cwk[0])
			{
				 len =0;
				 memset(cvv_data,0x00,sizeof(cvv_data));
				 memset(bcavv_data,0x00,sizeof(bcavv_data));
				 memcpy(cvv_data,p_auth_tx->TLF01_details.dcf01_retired_cwk,40);
				 genutil_asc_to_bin(cvv_data,bcavv_data, &len );
				 memcpy( &HOST_MESSAGE.de126[F126Len], bcavv_data, len );
				 F126Len += 20;
				 HOST_MESSAGE.de126[1] ^= 0x80;  /* Set bitmap - bit 9 */
			}

			if ('\0' != p_auth_tx->TLF01_details.product_codes[15].code[0])
			{
				 len =0;
				 memset(temp_str, 0x00, sizeof(temp_str));
				 for( i=0; i<6; i++ )
				 {
					 temp_str[i] =
					   ascii_ebcdic_conv( p_auth_tx->TLF01_details.product_codes[15].code[i], A2E );
				 }
				 len = i;
				 memcpy( &HOST_MESSAGE.de126[F126Len], temp_str, len );
				 F126Len += len;
				 HOST_MESSAGE.de126[1] ^= 0x40; /* Set bitmap - bit 10 */
				 len = 0;
			}

			if('\0' != p_auth_tx->TLF01_details.product_codes[0].code[2])
			{
				HOST_MESSAGE.de126[F126Len] = ascii_ebcdic_conv
						( p_auth_tx->TLF01_details.product_codes[0].code[2], A2E );
				 F126Len += 1;
				 HOST_MESSAGE.de126[2] ^= 0x10;  /* Set bitmap - bit 20 */
			}
   }
   else  if ( (0 != strcmp(eCommerce_Indcator, "07")) && (cavv_data_exists) )
      {
         memset( bcavv_data, 0x00, sizeof(bcavv_data) );
         memset( btran_id,   0x00, sizeof(btran_id)   );

         /* 126.8 - send out as it is received from the terminal. */
         memcpy( &HOST_MESSAGE.de126[F126Len], trans_id, 20 );
         F126Len += 20;
         HOST_MESSAGE.de126[0] ^= 0x01;  /* Set bitmap - bit 8 */

         /* 126.9 */
         trim_spaces( cvv_data );
         len = strlen( cvv_data );

         /* Convert Base64 to binary. */
         num_binary_bytes = genutil_base64_to_binary(cvv_data, len, bcavv_data);

         memcpy( &HOST_MESSAGE.de126[F126Len], bcavv_data, 20 );
         F126Len += 20;
         HOST_MESSAGE.de126[1] ^= 0x80;  /* Set bitmap - bit 9 */


         /* Store CAVV data into TLF01 so Equitable Bank can access it.
          * Use voice_auth_text since it is not used for ecommerce.
          * Store Tran ID (40), CAVV (40).
          * Use profile for E-Commerce Ind. (2).
          */

         /* First, convert CAVV to ASCII.  This will be HEX characters. */
         memset( temp_str, 0x00, sizeof(temp_str) );
         genutil_bin_to_ascii(bcavv_data, 20, temp_str, sizeof(temp_str) );

         memcpy( p_auth_tx->TLF01_details.voice_auth_text,    trans_id, 20 );
         memcpy( p_auth_tx->TLF01_details.voice_auth_text+20, temp_str, 40 );
         if(!IsFieldNull(p_auth_tx->TLF01_details.profile))
		 memcpy( p_auth_tx->TLF01_details.profile, eCommerce_Indcator,   2 );
      }
   	else if ( false == empty_string(p_auth_tx->TLF01_details.cvc_data,3))
      {
         HOST_MESSAGE.de126[1] ^= 0x40; /* Set bitmap - bit 10 */

         if (true == blank_or_null_string(p_auth_tx->TLF01_details.cvc_data,3))
            cvc2[0] = 0xf0;             /* Means cvc2 is not present. */
         else
            cvc2[0] = 0xf1;             /* Means cvc2 is present. */

         cvc2[1] = 0xf1;                /* Means send CVC2 result field 44.10 */
         cvc2[2] = 0x40;                /* Space fill on left the cvc2 value. */

         /* Convert cvc2 to EBCDIC. */
         for( i=0; i<3; i++ )
         {
            cvc2[i+3] = ascii_ebcdic_conv(p_auth_tx->TLF01_details.cvc_data[i],A2E);
            if ( cvc2[i+3] == 0x00 )
               cvc2[i+3] = 0x40; /* Set to space if Null. */
         }
         memcpy( &HOST_MESSAGE.de126[F126Len], cvc2, 6 );
         F126Len += 6;
      }
     else if ( p_auth_tx->TLF01_details.interest_rate[0] != 0x00 )
      {
         /* For Amex CSC (4-digits), field cvc_data is not large enough. */
         HOST_MESSAGE.de126[1] ^= 0x40; /* Set bitmap - bit 10 */

         if (true == blank_or_null_string(p_auth_tx->TLF01_details.interest_rate,4))
            cvc2[0] = 0xf0;             /* Means cvc2 is not present. */
         else
            cvc2[0] = 0xf1;             /* Means cvc2 is present. */

         cvc2[1] = 0xf1;                /* Means send CVC2 result field 44.10 */

         /* Convert cvc2 to EBCDIC. */
         for( i=0; i<4; i++ )
         {
            cvc2[i+2] = ascii_ebcdic_conv(p_auth_tx->TLF01_details.interest_rate[i],A2E);
            if ( cvc2[i+2] == 0x00 )
               cvc2[i+2] = 0x40; /* Set to space if Null. */
         }
         memcpy( &HOST_MESSAGE.de126[F126Len], cvc2, 6 );
         F126Len += 6;
      }
	}
  	else if ( (0 == strcmp(eCommerce_Indcator, "07")) || (!cavv_data_exists) )
   {
      if ( (pos_entry_type == POS_ENTRY_EMV_FALLBACK) &&
           (blank_or_null_string(p_auth_tx->TLF01_details.cvc_data,3)) )
      {
         /* Do not do CVV2 */
      }
      else if ( false == empty_string(p_auth_tx->TLF01_details.cvc_data,3))
      {
         HOST_MESSAGE.de126[1] ^= 0x40; /* Set bitmap - bit 10 */

         if (true == blank_or_null_string(p_auth_tx->TLF01_details.cvc_data,3))
            cvc2[0] = 0xf0;             /* Means cvc2 is not present. */
         else
            cvc2[0] = 0xf1;             /* Means cvc2 is present. */

         cvc2[1] = 0xf1;                /* Means send CVC2 result field 44.10 */
         cvc2[2] = 0x40;                /* Space fill on left the cvc2 value. */

         /* Convert cvc2 to EBCDIC. */
         for( i=0; i<3; i++ )
         {
            cvc2[i+3] = ascii_ebcdic_conv(p_auth_tx->TLF01_details.cvc_data[i],A2E);
            if ( cvc2[i+3] == 0x00 )
               cvc2[i+3] = 0x40; /* Set to space if Null. */
         }
         memcpy( &HOST_MESSAGE.de126[F126Len], cvc2, 6 );
         F126Len += 6;
      }
      else if ( p_auth_tx->TLF01_details.interest_rate[0] != 0x00 )
      {
         /* For Amex CSC (4-digits), field cvc_data is not large enough. */
         HOST_MESSAGE.de126[1] ^= 0x40; /* Set bitmap - bit 10 */

         if (true == blank_or_null_string(p_auth_tx->TLF01_details.interest_rate,4))
            cvc2[0] = 0xf0;             /* Means cvc2 is not present. */
         else
            cvc2[0] = 0xf1;             /* Means cvc2 is present. */

         cvc2[1] = 0xf1;                /* Means send CVC2 result field 44.10 */

         /* Convert cvc2 to EBCDIC. */
         for( i=0; i<4; i++ )
         {
            cvc2[i+2] = ascii_ebcdic_conv(p_auth_tx->TLF01_details.interest_rate[i],A2E);
            if ( cvc2[i+2] == 0x00 )
               cvc2[i+2] = 0x40; /* Set to space if Null. */
         }
         memcpy( &HOST_MESSAGE.de126[F126Len], cvc2, 6 );
         F126Len += 6;
      }
   }
		if (( strncmp(p_auth_tx->TLF01_details.pos_condition_code,"08",2) == 0 || iIndustry == RPS  ) &&
			0 == strncmp(p_auth_tx->TLF01_details.product_codes[4].code, "ACQUR", 5))
		{
			/* Removed old code, In case of RPS transaction 126.13 should be C */
			Fld12613= 0xD9;// EBCDIC value or R

			/* 126.13 */
			HOST_MESSAGE.de126[1] ^= 0x08;  /* Set bitmap - bit 13            */
			HOST_MESSAGE.de126[8]  = 0xD9;  /* Set RPS Indicator = R (EBCDIC) */

			F126Len += 1;
		}
   /* Apr 2009 Mandates, Girija Y TF, March 27 2009 */
   else if ( (p_auth_tx->TLF01_details.response_text[0] == 'I') || 
	         (p_auth_tx->TLF01_details.response_text[0] ==  0xC9) )
   {
	   /* 126.13 */
	  // BYTE Fld12613;
	   
	   Fld12613 = p_auth_tx->TLF01_details.response_text[0];
	   HOST_MESSAGE.de126[1] ^= 0x08;  /* Set bitmap - bit 13 */
	   HOST_MESSAGE.de126[F126Len] = Fld12613;
	   F126Len += 1;
	   /* copy the filed 104 txn description Ajay-TF. 11-Jun-2009*/
	   memcpy(HOST_MESSAGE.trandesc_len,&p_auth_tx->TLF01_details.response_text[1],1);
	   len = HOST_MESSAGE.trandesc_len[0];
       memcpy(HOST_MESSAGE.trandesc, &p_auth_tx->TLF01_details.response_text[2],len);
 
   }
		/* Check Flag to '1' to not build DE126 for VOICE in sbatch */
	if(0 == strncmp(p_auth_tx->TLF01_details.handler_queue,"sbatch",6) &&
		p_auth_tx->TLF01_details.product_codes[4].quantity[0] != '1')
	{
		if('\0' != p_auth_tx->TLF01_details.product_codes[4].amount[0])
		{
			 len =0;
			 HOST_MESSAGE.de126[0] ^= 0x08; /* Set bitmap - bit 5 */
			 for( i=0; i<8; i++ )
			 {
				 temp_str[i] =
				   ascii_ebcdic_conv( p_auth_tx->TLF01_details.product_codes[4].amount[i], A2E );
			 }
			 len=i;
			 memcpy( &HOST_MESSAGE.de126[F126Len], temp_str, len );
			 F126Len += len;
			 len = 0;

		}
		if('\0' != p_auth_tx->TLF01_details.operator_id[0])
		{
			len =0;
			 HOST_MESSAGE.de126[0] ^= 0x01; /* Set bitmap - bit 8 */
			 genutil_asc_to_bin(p_auth_tx->TLF01_details.operator_id,temp_str,&len);
			 memcpy( &HOST_MESSAGE.de126[F126Len], temp_str, len );
			 F126Len += len;
			 len = 0;
			 genutil_asc_to_bin(p_auth_tx->TLF01_details.workstation,temp_str,&len);
			 memcpy( &HOST_MESSAGE.de126[F126Len], temp_str, len );
			 F126Len += len;
		}

		if('\0' != p_auth_tx->TLF01_details.dcf01_retired_cwk[0] &&
			'C' != p_auth_tx->TLF01_details.dcf01_retired_cwk[0])
		{
			 len =0;
			 memset(cvv_data,0x00,sizeof(cvv_data));
			 memset(bcavv_data,0x00,sizeof(bcavv_data));
			 memcpy(cvv_data,p_auth_tx->TLF01_details.dcf01_retired_cwk,20);
			 genutil_asc_to_bin(cvv_data,bcavv_data, &len );
			 memcpy( &HOST_MESSAGE.de126[F126Len], bcavv_data, len );
			 F126Len += 20;
			 HOST_MESSAGE.de126[1] ^= 0x80;  /* Set bitmap - bit 9 */
		}

		if ('\0' != p_auth_tx->TLF01_details.product_codes[15].code[0])
		{
			 len =0;
			 memset(temp_str, 0x00, sizeof(temp_str));
			 for( i=0; i<6; i++ )
			 {
				 temp_str[i] =
				   ascii_ebcdic_conv( p_auth_tx->TLF01_details.product_codes[15].code[i], A2E );
			 }
			 len = i;
			 memcpy( &HOST_MESSAGE.de126[F126Len], temp_str, len );
			 F126Len += len;
			 HOST_MESSAGE.de126[1] ^= 0x40; /* Set bitmap - bit 10 */
			 len = 0;
		}

		if ('\0' != p_auth_tx->TLF01_details.product_codes[7].quantity[0])
		{
			HOST_MESSAGE.de126[F126Len] = ascii_ebcdic_conv
					(p_auth_tx->TLF01_details.product_codes[7].quantity[0], A2E);
			F126Len += 1;
			 HOST_MESSAGE.de126[1] ^= 0x08;  /* Set bitmap - bit 13 */
		}

		if('\0' != p_auth_tx->TLF01_details.product_codes[0].code[2])
		{
			HOST_MESSAGE.de126[F126Len] = ascii_ebcdic_conv
					( p_auth_tx->TLF01_details.product_codes[0].code[2], A2E );
			 F126Len += 1;
			 HOST_MESSAGE.de126[2] ^= 0x10;  /* Set bitmap - bit 20 */
		}


		/*Lets log this transaction in ewlog for VIsa Looping issue*/
		memset(Log_Bufr,0x00,sizeof(Log_Bufr));
		sprintf(Log_Bufr," Advice_Sbatch Transaction with MSG_TYPE:%s STAN:% Term Id:%s ,Merch Id: %s RRN: %s ",
				p_auth_tx->TLF01_details.message_type,p_auth_tx->TLF01_details.sys_trace_audit_num,p_auth_tx->TLF01_details.terminal_id,
				p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
		ncvis2_log_message( 2, 1, Log_Bufr , "build_host_structure" , 0 ,p_auth_tx->TLF01_details.card_num);
	}
    if ( F126Len > F126_BITMAP_LEN )
   {
      /* Set the length. */
      HOST_MESSAGE.de126_len[0] = F126Len;
   }
   /* ---------------- */
   /* End of Field 126 */
   /* ---------------- */


   /* FIELD 127: FILE MAINTENANCE (FORMAT 2) */
   if ( (p_auth_tx->TLF01_details.tx_key == AUTH_CARD_UPDATE) ||
        (p_auth_tx->TLF01_details.tx_key == AUTH_CARD_UPDATE_RESPONSE) )
   {
      /* Subfield E.1 = Action Code */
      HOST_MESSAGE.file_rec[0] =
             ascii_ebcdic_conv( p_auth_tx->TLF01_details.type_of_data[0], A2E );

      HOST_MESSAGE.file_rec[1] =
             ascii_ebcdic_conv( p_auth_tx->TLF01_details.type_of_data[1], A2E );

      /* Subfield E.2 = Region Coding */
      for( i=0; i<9; i++ )
      {
         HOST_MESSAGE.file_rec[i+2] =
            ascii_ebcdic_conv( p_auth_tx->TLF01_details.product_codes[1].amount[i], A2E );
      }
      HOST_MESSAGE.file_rec_len[0] = strlen( HOST_MESSAGE.file_rec );
   }

   if(F62Len == 0)
	{
	 memset(HOST_MESSAGE.priv62_len,0x00,sizeof(HOST_MESSAGE.priv62_len));
	 memset(HOST_MESSAGE.priv62,0x00,sizeof(HOST_MESSAGE.priv62));
	}
   /*+-----------------------------+
     | EMV FIELDS and THIRD BITMAP |
     +-----------------------------+*/
#if 0
   if ( emv_txn == true )
   {
      Third_Bitmap = true;

      /* FIELD 130: TERMINAL CAPABILITY PROFILE */
      memset( temp_str, 0x00, sizeof(temp_str) );
      hex_tag[0] = 0x9F;
      hex_tag[1] = 0x33;
      ret_val = genutil_get_emv_tag( hex_tag,
                                     p_auth_tx->EMV_details.emv_block+2,
                                     emv_len, temp_str, &len );
      if ( ret_val == true )
         memcpy( HOST_MESSAGE.term_cap_prof, temp_str, len );


      /* FIELD 131: TERMINAL VERIFICATION RESULTS */
      arpc = 0x00;
      if ( p_auth_tx->TLF01_details.message_type[1] == '4' )
      {
         /* This is a reversal.  Only send fields 131 and 134
          * if the ARPC bit (byte 5, bit 7) indicates a failure.
          * Issuer Authentication: 0 = success, 1 = failed
          */

         /* Need to convert TVR from ASCII into Binary to check this bit. */
         len = strlen( p_auth_tx->EMV_details.tvr );
         memset( temp_str, 0x00, sizeof(temp_str) );
         ascii_to_bin( p_auth_tx->EMV_details.tvr, len, temp_str, len/2 );

         arpc = temp_str[4];  /* Fifth byte */
         if ( arpc & EMV_ARPC_FAILURE )
         {
            memcpy( HOST_MESSAGE.tvr, temp_str, len/2 );
         }
      }
      else if ( p_auth_tx->TLF01_details.tx_key != AUTH_VOID_SALE )
      {
         /* Not a reversal, send this field if present. */
         memset( temp_str, 0x00, sizeof(temp_str) );
         hex_tag[0] = 0x95;
         hex_tag[1] = 0x00;
         ret_val = genutil_get_emv_tag( hex_tag,
                                        p_auth_tx->EMV_details.emv_block+2,
                                        emv_len, temp_str, &len );
         if ( ret_val == true )
         {
            memcpy( HOST_MESSAGE.tvr, temp_str, len );
         }
      }

      /* FIELD 132: UNPREDICTABLE NUMBER */
      memset( temp_str, 0x00, sizeof(temp_str) );
      hex_tag[0] = 0x9F;
      hex_tag[1] = 0x37;
      ret_val = genutil_get_emv_tag( hex_tag,
                                     p_auth_tx->EMV_details.emv_block+2,
                                     emv_len, temp_str, &len );
      if ( ret_val == true )
         memcpy( HOST_MESSAGE.unpredictable, temp_str, len );


      /* FIELD 133: INTERFACE DEVICE SERIAL NUMBER */
      memset( temp_str, 0x00, sizeof(temp_str) );
      hex_tag[0] = 0x9F;
      hex_tag[1] = 0x1E;
      ret_val = genutil_get_emv_tag( hex_tag,
                                     p_auth_tx->EMV_details.emv_block+2,
                                     emv_len, temp_str, &len );
      if ( ret_val == true )
         memcpy( HOST_MESSAGE.tid_serial, temp_str, len );


      /* FIELD 134: VISA   DISCRETIONARY DATA */
      /* FIELD 135: ISSUER DISCRETIONARY DATA */
      memset( temp_str, 0x00, sizeof(temp_str) );
      hex_tag[0] = 0x9F;
      hex_tag[1] = 0x10;
      ret_val = genutil_get_emv_tag( hex_tag,
                                     p_auth_tx->EMV_details.emv_block+2,
                                     emv_len, temp_str, &len );
      if ( ret_val == true )
      {
         if ( ncf01_i.pad_char[0] == 'E' )
         {
            /* Using the standard (old) format. */
            sublen = temp_str[0];

            if ( p_auth_tx->TLF01_details.message_type[1] == '4' )
            {
               /* This is a reversal.  Only send fields 131 and 134
                * if the ARPC bit (byte 5, bit 7) indicates a failure.
                * Issuer Authentication: 0 = success, 1 = failed
                */
               if ( arpc & EMV_ARPC_FAILURE )
               {
                  if ( p_auth_tx->EMV_details.iad[0] == 0x00 )
                  {
                     memcpy( HOST_MESSAGE.visa_data, temp_str+1, sublen );
                     HOST_MESSAGE.visa_data_len[0] = sublen;
                  }
                  else
                  {
                     /* Tag 9F10 is put into EMV_details.iad by dcpiso.
                      * For reversals, ncvisa has the EMV block of the
                      * original transaction (courtesy DEVDS). But Visa
                      * needs to send 9F10 from the reversal, not from
                      * the orig txn.  So get the value from IAD.
                      */
                     memset( temp_str, 0x00, sizeof(temp_str) );
                     genutil_asc_to_bin( p_auth_tx->EMV_details.iad,
                                         temp_str, &len );

                     strcpy( HOST_MESSAGE.visa_data, temp_str+1 );
                     HOST_MESSAGE.visa_data_len[0] = len - 1;
                  }
               }
            }
            else if ( p_auth_tx->TLF01_details.tx_key != AUTH_VOID_SALE )
            {
               /* Not a reversal. */

               /* Field 134 */
               memcpy( HOST_MESSAGE.visa_data, temp_str+1, sublen );
               HOST_MESSAGE.visa_data_len[0] = sublen;

               /* Field 135 */
               if ( (sublen + 1) < len )
               {
                  /* Field 135 data exists; send it. */
                  i = sublen + 1;
                  sublen = temp_str[i];
                  memcpy( HOST_MESSAGE.iss_data, &temp_str[i+1], sublen );
                  HOST_MESSAGE.iss_data_len[0] = sublen;
               }
            }
         }
         else
         {
            /* Using the expanded format for CCD.
             * Do not send DE135.
             * All information is stored in DE134.
             */
            if ( p_auth_tx->TLF01_details.message_type[1] == '4' )
            {
               /* This is a reversal.  Only send fields 131 and 134
                * if the ARPC bit (byte 5, bit 7) indicates a failure.
                * Issuer Authentication: 0 = success, 1 = failed
                */
               if ( arpc & EMV_ARPC_FAILURE )
               {
                  if ( p_auth_tx->EMV_details.iad[0] == 0x00 )
                  {
                     memcpy( HOST_MESSAGE.visa_data, temp_str, len );
                     HOST_MESSAGE.visa_data_len[0] = len;
                  }
                  else
                  {
                     /* Tag 9F10 is put into EMV_details.iad by dcpiso.
                      * For reversals, ncvisa has the EMV block of the
                      * original transaction (courtesy DEVDS). But Visa
                      * needs to send 9F10 from the reversal, not from
                      * the orig txn.  So get the value from IAD.
                      */
                     memset( temp_str, 0x00, sizeof(temp_str) );
                     genutil_asc_to_bin( p_auth_tx->EMV_details.iad,
                                         temp_str, &len );

                     strcpy( HOST_MESSAGE.visa_data, temp_str );
                     HOST_MESSAGE.visa_data_len[0] = len;
                  }
               }
            }
            else if ( p_auth_tx->TLF01_details.tx_key != AUTH_VOID_SALE )
            {
               /* Not a reversal, send this field. */
               memcpy( HOST_MESSAGE.visa_data, temp_str, len );
               HOST_MESSAGE.visa_data_len[0] = len;
            }
         }
      }

      /* FIELD 136: APPLICATION CRYPTOGRAM */
      memset( temp_str, 0x00, sizeof(temp_str) );
      hex_tag[0] = 0x9F;
      hex_tag[1] = 0x26;
      ret_val = genutil_get_emv_tag( hex_tag,
                                     p_auth_tx->EMV_details.emv_block+2,
                                     emv_len, temp_str, &len );
      if ( ret_val == true )
         memcpy( HOST_MESSAGE.cryptogram, temp_str, len );


      /* FIELD 137: APPLICATION TRANSACTION COUNTER */
      memset( temp_str, 0x00, sizeof(temp_str) );
      hex_tag[0] = 0x9F;
      hex_tag[1] = 0x36;
      ret_val = genutil_get_emv_tag( hex_tag,
                                     p_auth_tx->EMV_details.emv_block+2,
                                     emv_len, temp_str, &len );
      if ( ret_val == true )
         memcpy( HOST_MESSAGE.app_tran_ctr, temp_str, len );


      /* FIELD 138: APPLICATION INTERCHANGE PROFILE */
      memset( temp_str, 0x00, sizeof(temp_str) );
      hex_tag[0] = 0x82;
      hex_tag[1] = 0x00;
      ret_val = genutil_get_emv_tag( hex_tag,
                                     p_auth_tx->EMV_details.emv_block+2,
                                     emv_len, temp_str, &len );
      if ( ret_val == true )
         memcpy( HOST_MESSAGE.app_profile, temp_str, len );


      /* FIELD 143: ISSUER SCRIPT RESULTS */
      if ( p_auth_tx->TLF01_details.tx_key == AUTH_VOID_SALE )
      {
         memset( temp_str, 0x00, sizeof(temp_str) );
         hex_tag[0] = 0x9F;
         hex_tag[1] = 0x5B;
         ret_val = genutil_get_emv_tag( hex_tag,
                                        p_auth_tx->EMV_details.emv_block+2,
                                        emv_len, temp_str, &len );
         if ( ret_val == true )
         {
            memcpy( HOST_MESSAGE.iss_result, temp_str, len );
            HOST_MESSAGE.iss_result_len[0] = len;
         }
      }


      /* FIELD 144: CRYPTOGRAM TRANSACTION TYPE */
      memset( temp_str, 0x00, sizeof(temp_str) );
      hex_tag[0] = 0x9C;
      hex_tag[1] = 0x00;
      ret_val = genutil_get_emv_tag( hex_tag,
                                     p_auth_tx->EMV_details.emv_block+2,
                                     emv_len, temp_str, &len );
      if ( ret_val == true )
         memcpy( HOST_MESSAGE.crypto_type, temp_str, len );


      /* FIELD 145: TERMINAL COUNTRY CODE */
      memset( temp_str, 0x00, sizeof(temp_str) );
      hex_tag[0] = 0x9F;
      hex_tag[1] = 0x1A;
      ret_val = genutil_get_emv_tag( hex_tag,
                                     p_auth_tx->EMV_details.emv_block+2,
                                     emv_len, temp_str, &len );
      if ( ret_val == true )
         memcpy( HOST_MESSAGE.tid_country, temp_str, len );


      /* FIELD 146: TERMINAL TRANSACTION DATE */
      memset( temp_str, 0x00, sizeof(temp_str) );
      hex_tag[0] = 0x9A;
      hex_tag[1] = 0x00;
      ret_val = genutil_get_emv_tag( hex_tag,
                                     p_auth_tx->EMV_details.emv_block+2,
                                     emv_len, temp_str, &len );
      if ( ret_val == true )
         memcpy( HOST_MESSAGE.tid_tran_date, temp_str, len );


      /* FIELD 147: CRYPTOGRAM TRANSACTION AMOUNT */
      memset( temp_str, 0x00, sizeof(temp_str) );
      hex_tag[0] = 0x9F;
      hex_tag[1] = 0x02;
      ret_val = genutil_get_emv_tag( hex_tag,
                                     p_auth_tx->EMV_details.emv_block+2,
                                     emv_len, temp_str, &len );
      if ( ret_val == true )
         memcpy( HOST_MESSAGE.crypto_amt, temp_str, len );


      /* FIELD 148: CRYPTOGRAM CURRENCY CODE */
      memset( temp_str, 0x00, sizeof(temp_str) );
      hex_tag[0] = 0x5F;
      hex_tag[1] = 0x2A;
      ret_val = genutil_get_emv_tag( hex_tag,
                                     p_auth_tx->EMV_details.emv_block+2,
                                     emv_len, temp_str, &len );
      if ( ret_val == true )
         memcpy( HOST_MESSAGE.crypto_curr, temp_str, len );


      /* FIELD 149: CRYPTOGRAM CASHBACK AMOUNT */
      memset( temp_str, 0x00, sizeof(temp_str) );
      hex_tag[0] = 0x9F;
      hex_tag[1] = 0x03;
      ret_val = genutil_get_emv_tag( hex_tag,
                                     p_auth_tx->EMV_details.emv_block+2,
                                     emv_len, temp_str, &len );
      if ( ret_val == true )
         memcpy( HOST_MESSAGE.crypto_cashbk, temp_str, len );
   }
#endif
   return;
}/* end Build_Host_Structure */

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
  /* response.  These are sent from the Visa host as 0110's.          */
  if ( (0x01 == HOST_MESSAGE.msgtype_bcd [0]) && (0x10 == HOST_MESSAGE.msgtype_bcd [1]) )
  {
    /* now verify the procesing code to know what kind */
    switch(HOST_MESSAGE.proccode[0])
	 {
      case 0x00:	
               if (AUTH_SALE == tx_key)
						return(AUTH_SALE_RESPONSE);
					else
						return(AUTH_AUTHORIZATION_RESPONSE);
					  break;

      case 0x20:	return(AUTH_PRE_AUTHORIZATION_CANCLE_RESPONSE);
					  break;

      case 0x02:	return(AUTH_VOID_SALE_RESPONSE);
					  break;

      case 0x22:	return(AUTH_VOID_REFUND_RESPONSE);
					  break;

      case 0x11:	return(AUTH_QUASI_CASH_RESPONSE);
					  break;

      case 0x01:	return(AUTH_CASH_ADVANCE_RESPONSE);
					  break;

      case 0x30:	return(AUTH_PRE_AUTHORIZATION_RESPONSE);
					  break;

 	   case 0x31:	return(AUTH_BALANCE_INQUIRY_RESPONSE);
					  break;

	   case 0x40:	return(AUTH_PAYMENT_RESPONSE);
					  break;

      case 0x42:	return(AUTH_VOID_PAYMENT_RESPONSE);
					  break;

      case 0x51:	return(AUTH_CARD_VERIFICATION_RESPONSE);  // 38 from terminal, 51 from host
					  break;

      case 0x38:	return(AUTH_CARD_VERIFICATION_RESPONSE);  // Might also be 38
					  break;

	  case 0x28:	return(AUTH_MONEY_SEND_RESPONSE);  // Money Send Response
		  	  	  	  	  break;

	  case 0x26:	return(AUTH_OCT_TX_RESPONSE);  // OCT Auth Response
		  	  	  	  	  break;

      case 0x10:	return(AUTH_FAST_FUND_RESPONSE);
					  break; //AUTH_FAST_FUND
     default:	return(AUTH_INVALID);
					  break;
    }
  }  
   /* or  an advise response */
  else if ( (0x02 == HOST_MESSAGE.msgtype_bcd [0]) && (0x30 == HOST_MESSAGE.msgtype_bcd [1]) )
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
  else if ( (0x04 == HOST_MESSAGE.msgtype_bcd [0]) && (0x10 == HOST_MESSAGE.msgtype_bcd[1]) )
	   return( AUTH_REVERSAL_RESPONSE );

  else if ( (0x04 == HOST_MESSAGE.msgtype_bcd [0]) && (0x11 == HOST_MESSAGE.msgtype_bcd[1]) )
	   return( AUTH_REVERSAL_REPEAT_RESPONSE );   

  /* or a ATM confirmation  response */  
  else if ( (0x01 == HOST_MESSAGE.msgtype_bcd [0]) && (0x12 == HOST_MESSAGE.msgtype_bcd[1]) )
	   return( AUTH_ATM_CONFIRM_RESPONSE );

  /* or a ATM confirmation  response */  
  else if ( (0x01 == HOST_MESSAGE.msgtype_bcd [0]) && (0x30 == HOST_MESSAGE.msgtype_bcd[1]) )
	   return( AUTH_ADVICE_RESPONSE );

  else if ( (0x04 == HOST_MESSAGE.msgtype_bcd [0]) && (0x30 == HOST_MESSAGE.msgtype_bcd[1]) )
	   return( AUTH_REVERSAL_ADVICE_RESPONSE );
  /* STIP reversal advice repeat response Dec 15,2009 TF-Ajay */
  else if ( (0x04 == HOST_MESSAGE.msgtype_bcd [0]) && (0x31 == HOST_MESSAGE.msgtype_bcd[1]) )
	   return( AUTH_REVERSAL_ADVICE_REPEAT_RESPONSE );

  else if ( (0x03 == HOST_MESSAGE.msgtype_bcd [0]) && (0x02 == HOST_MESSAGE.msgtype_bcd [1]) )
	   return( AUTH_CARD_UPDATE );

  else if ( (0x03 == HOST_MESSAGE.msgtype_bcd [0]) && (0x12 == HOST_MESSAGE.msgtype_bcd [1]) )
	   return( AUTH_CARD_UPDATE_RESPONSE );

  /* or a statistics response */ 
  else if ( (0x08 == HOST_MESSAGE.msgtype_bcd [0]) && (0x10 == HOST_MESSAGE.msgtype_bcd [1]) )
  {
    switch(HOST_MESSAGE.proccode[0])
    {
      case 0x91:	return(AUTH_STATISTICS_RESPONSE);
					  break;

      case 0x92:	return(AUTH_LOGON_RESPONSE);
					  break;

      case 0x99:	return(AUTH_TEST_RESPONSE);
					  break;

      default:	return(AUTH_LOGON_RESPONSE);
					  break;
    }
  }	

     /* Check for an Authorization request from another host */
   else if ((0x01 == HOST_MESSAGE.msgtype_bcd [0] ) && (0x00 == HOST_MESSAGE.msgtype_bcd [1])) 
   {
      switch( HOST_MESSAGE.proccode[0] )
      {
         case 00:
            return( AUTH_AUTHORIZATION );
         break;

         case 0x20:
        	 return(AUTH_PRE_AUTHORIZATION_CANCLE);
         break;

         case 30:
            return( AUTH_BALANCE_INQUIRY );
         break;

         case 31:
            return( AUTH_BALANCE_INQUIRY );
         break;

         case 17:
            return(AUTH_QUASI_CASH);
	 	   break;

         case 01:
            return( AUTH_CASH_ADVANCE );
         break;

         case 38:
            return(AUTH_CARD_VERIFICATION);
			break;

         case 48:	                       
            return(AUTH_BALANCE_INQUIRY);
			break;

		 case 11:
            return(AUTH_QUASI_CASH);
	 	   break;

		 case 0x28:
			 return(AUTH_MONEY_SEND);  // Money Send Request
			   break;

		 case 0x10:
			 return(AUTH_FAST_FUND);
			   break;

         default:
            return( AUTH_INVALID );
         break;
      }
   }

   else if ((0x01 == HOST_MESSAGE.msgtype_bcd [0] ) && (0x01 == HOST_MESSAGE.msgtype_bcd [1])) 
      return( AUTH_AUTHORIZATION );

   /* Check for a Reversal request from another host */  
   else if ( (0x04 == HOST_MESSAGE.msgtype_bcd [0]) && (0x00 == HOST_MESSAGE.msgtype_bcd [1]))
      return( AUTH_REVERSAL );
		
   else if ( (0x04 == HOST_MESSAGE.msgtype_bcd [0]) && (0x01 == HOST_MESSAGE.msgtype_bcd [1]))
      return( AUTH_REVERSAL_REPEAT );
		
   else if ( (0x04 == HOST_MESSAGE.msgtype_bcd [0]) && (0x20 == HOST_MESSAGE.msgtype_bcd [1]))
      return( AUTH_REVERSAL_ADVICE );
		
  /* or a ATM confirmation  request */  
  else if ( (0x01 == HOST_MESSAGE.msgtype_bcd [0]) && (0x02 == HOST_MESSAGE.msgtype_bcd[1]) )
	   return( AUTH_ATM_CONFIRM );

  else if ( (0x06 == HOST_MESSAGE.msgtype_bcd [0]) && (0x20 == HOST_MESSAGE.msgtype_bcd[1]) )
	   return( AUTH_ADMIN_ADVICE );

  else if ( (0x01 == HOST_MESSAGE.msgtype_bcd [0]) && (0x20) == HOST_MESSAGE.msgtype_bcd[1] )
	   return( AUTH_ADVICE );

  /* Check for a batch upload from another host */
   else if ( (0x03 == HOST_MESSAGE.msgtype_bcd [0]) && (0x20 == HOST_MESSAGE.msgtype_bcd [1]))
      return( AUTH_BATCH_UPLOAD ); 
 
   /* Check for a settlement transaction from another host */
   else if ( (0x05 == HOST_MESSAGE.msgtype_bcd [0]) && (0x00 == HOST_MESSAGE.msgtype_bcd [1]))
   {
      switch( HOST_MESSAGE.proccode[0] )
      {
         case 92:
            return( AUTH_SETTLE );
         break;

         case 96:
            return( AUTH_SETTLE_TRAILER );
         break;

         default:
            return( AUTH_INVALID );
         break;
      }
   }

   /* Check for a statistics transaction from another host */
   else if ( (0x08 == HOST_MESSAGE.msgtype_bcd [0]) && (0x00 == HOST_MESSAGE.msgtype_bcd [1]))
   {
      switch( HOST_MESSAGE.proccode[0] )
      {
         case 91:
            return( AUTH_STATISTICS );
         break;

         case 92:
            return( AUTH_LOGON );
         break;

         case 99:
            return( AUTH_TEST );
         break;

         default:
            return( AUTH_LOGON );
         break;
      }
   }

   /*-----------------------------------------------------------------*/
   /* Check for response message types when dealing with other hosts. */
   /*-----------------------------------------------------------------*/
   else if ( (0x02 == HOST_MESSAGE.msgtype_bcd [0]) && (0x10 == HOST_MESSAGE.msgtype_bcd [1]))
   {
      /* 0210 */
      switch( HOST_MESSAGE.proccode[0] )
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
               return( AUTH_AUTHORIZATION_RESPONSE );
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

         case 17:	
            return(AUTH_QUASI_CASH_RESPONSE);
			break;

         case 01:
            return( AUTH_CASH_ADVANCE_RESPONSE );
         break;

         case 30:
            return( AUTH_BALANCE_INQUIRY_RESPONSE );
         break;

         case 31:
            return( AUTH_BALANCE_INQUIRY_RESPONSE );
         break;

         case 40:
            return( AUTH_PAYMENT_RESPONSE );
         break;
		 case 11:	 
			 return(AUTH_QUASI_CASH_RESPONSE);
			 break;

    /*     case 42:
            return( AUTH_VOID_PAYMENT_RESPONSE );
         break; DJI debug_code */
   
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

   /* To support reversal advice repeat Dec 15,2009 TF-Ajay */
  else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0431" ) )
	   return( AUTH_REVERSAL_ADVICE_REPEAT_RESPONSE );


  else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0411" ) )
	   return( AUTH_REVERSAL_REPEAT_RESPONSE );

   /* Check for a statistics response from MCard host */
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0810" ) )
   {
      switch( ProcCode )
      {
         case 91:
            return( AUTH_STATISTICS_RESPONSE );
         break;

         case 92:
            return( AUTH_LOGON_RESPONSE );
         break;

         case 99:
            return( AUTH_TEST_RESPONSE );
         break;

         default:
            return( AUTH_LOGON_RESPONSE );
         break;
      }
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
		 case 11:	
	  		return(AUTH_QUASI_CASH);
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
            return( AUTH_BALANCE_INQUIRY );
         break;

         case 11:	
            return(AUTH_QUASI_CASH);
			break;

         case 01:	
            return(AUTH_CASH_ADVANCE);
			break;

         case 31:
            return( AUTH_BALANCE_INQUIRY );
         break;

         case 38:
            return(AUTH_CARD_VERIFICATION);
			break;

         case 48:
            return(AUTH_BALANCE_INQUIRY);
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
  else if ( (0x01 == HOST_MESSAGE.msgtype_bcd [0]) && (0x02 == HOST_MESSAGE.msgtype_bcd[1]) )
	   return( AUTH_ATM_CONFIRM );

  else if ( (0x06 == HOST_MESSAGE.msgtype_bcd [0]) && (0x20 == HOST_MESSAGE.msgtype_bcd[1]) )
	   return( AUTH_ADMIN_ADVICE );

  /* or a ATM confirmation  request */  
  //The next line was modified by egable on 12/23/98
  else if ( (0x01 == HOST_MESSAGE.msgtype_bcd [0]) && (0x20 == HOST_MESSAGE.msgtype_bcd[1]) )
      return( AUTH_ADVICE );

  /* or a ATM confirmation  request */  
  //The next line was modified by egable on 12/23/98
  else if ( (0x04 == HOST_MESSAGE.msgtype_bcd [0]) && (0x20 == HOST_MESSAGE.msgtype_bcd[1]) )
	   return( AUTH_REVERSAL_ADVICE );

  else if ( (0x04 == HOST_MESSAGE.msgtype_bcd [0]) && (0x01 == HOST_MESSAGE.msgtype_bcd[1]) )
	   return( AUTH_REVERSAL_REPEAT );

   /* Check for a batch upload from another host */
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0320" ) )
      return( AUTH_BATCH_UPLOAD ); 
 
   /* Check for a settlement transaction from another host */
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0500" ) )
   {
      switch( ProcCode )
      {
         case 92:
            return( AUTH_SETTLE );
         break;

         case 96:
            return( AUTH_SETTLE_TRAILER );
         break;

         default:
            return( AUTH_INVALID );
         break;
      }
   }

   /* Check for a statistics transaction from another host */
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0800" ) )
   {
      switch( ProcCode )
      {
         case 91:
            return( AUTH_STATISTICS );
         break;

         case 92:
            return( AUTH_LOGON );
         break;

         case 99:
            return( AUTH_TEST );
         break;

         default:
            return( AUTH_LOGON );
         break;
      }
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

  /*       case 42: return( AUTH_VOID_PAYMENT_RESPONSE );
                    break;   DJI debug_code */

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

         case 30: return( AUTH_BALANCE_INQUIRY_RESPONSE );
                    break;

         case 31: return( AUTH_BALANCE_INQUIRY_RESPONSE );
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
      switch( ProcCode )
      {
         case 91: return( AUTH_STATISTICS_RESPONSE );
                    break;

         case 92: return( AUTH_LOGON_RESPONSE );
                    break;

         case 99: return( AUTH_TEST_RESPONSE );
                    break;

         default:   return( AUTH_LOGON_RESPONSE );
                    break;
      }
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

  Function:    Build_Host_Header

  Description: This function will build Visa's header and prepend it to the
               outgoing transaction.  This is required on all transactions
               going to Visa, whether they are requests or responses.

  Author: 
      unknown
  Inputs:
      p_auth_tx     - Contains Round Trip Indicator (RTI) and tx_key
      ReqRsp_Buffer - Without Visa's header
  Outputs:
      ReqRsp_Buffer - With Visa's header prepended
  Return values:
      None
******************************************************************************/
void Build_Host_Header (pAUTH_TX p_auth_tx)
{
   struct trandef  *pTranDef;
   BYTE            cMsgLen[2];
   BYTE            StationId[7];
   BYTE            net_consec_tmouts_ctr[4];
   BYTE            active_txns_ctr[4];
   BYTE            current_state[2];
   BYTE            prev_src_station[2];


   pTranDef = Find_Tran_Table(p_auth_tx->TLF01_details.tx_key);            /* Get pointer to tran table */
   //memcpy(&ReqRsp_Buffer[0], pTranDef->td_msgid, sizeof(pTranDef->td_msgid));

   memset(&ReqRsp_Buffer[0], 0x16, 1 );   /* Msg hdr len = 22,           Field 1 */
   memset(&ReqRsp_Buffer[1], 0x01, 1 );   /* Hdr format type = binary 1, Field 2 */
   memset(&ReqRsp_Buffer[2], 0x02, 1 );   /* Text format is of type 2,   Field 3 */

                                          /* Length of message,          Field 4 */
   cMsgLen[0] = ((buflen) & 0xff00)/256;
   cMsgLen[1] =  (buflen) & 0x00ff;
   memcpy(&ReqRsp_Buffer[3], cMsgLen, 2 );

   if (p_auth_tx->TLF01_details.tx_key > AUTH_INVALID)
   {
      //  Put the incoming dest into the outgoing src and incoming src into incoming dest
      str_asc_to_bcd (p_auth_tx->incoming_src_station_id, sizeof( p_auth_tx->incoming_src_station_id )-1, &ReqRsp_Buffer[5] );
      str_asc_to_bcd (p_auth_tx->incoming_dest_station_id, sizeof( p_auth_tx->incoming_dest_station_id )-1, &ReqRsp_Buffer[8] );
   }
   else
   {
	   memset(&ReqRsp_Buffer[5], 0x00, 3 );   /* Dest   station Id,          Field 5 */
      /* Source station Id - Alternate between 2 of them.                   Field 6 */
      ReadVisaTable( SHARED_MEMORY_TBL, net_consec_tmouts_ctr, 
                        active_txns_ctr, current_state, prev_src_station );

      if(prev_src_station != NULL_PTR)
      {
         memset( StationId, 0x00, sizeof( StationId ) );
         //the next line was replaced by the following line by farad on 8/19/98.
	     //if ( 0 == strncmp( ftos(host_app_src_station_flag), "1", 1 ))
         if ( '1' == prev_src_station[0] )
	     {
            strcpy( StationId, ncf01_i.station_id1);
            p_auth_tx->ncf01_prev_src_station[0]  = '2';
            prev_src_station[0]  = '2';
         }
         else
         {
            strcpy( StationId, ncf01_i.station_id2);
            p_auth_tx->ncf01_prev_src_station[0]  = '1';
            prev_src_station[0]  = '1';
         }
         str_asc_to_bcd (StationId, sizeof( StationId )-1, &ReqRsp_Buffer[8] );
      }
      else
      {
	      //memset(&ReqRsp_Buffer[8],0x00, 3);
	      memset( StationId, 0x00, sizeof( StationId ) );
	      strcpy( StationId, ncf01_i.station_id1);
         p_auth_tx->ncf01_prev_src_station[0]  = '2';
         prev_src_station[0]  = '2';
      }
      WriteVisaTable( SHARED_MEMORY_TBL, net_consec_tmouts_ctr, 
                        active_txns_ctr, current_state, prev_src_station );

   }

   /* --------------------------------------------------------------------- *
    * Fields 7 - 12 must be echoed back to Visa for switched in txns.
    * This is a switched in txn if first byte below is 0x02.
    *
    * Note 1:  The Merchant Contact field is used to store the header
    *          from Visa.  Merchant record is not used on switched in txns.
    *
    * Note 2:  Documentation indicates field 9 to be set to zeroes, but
    *          during certification, Visa informed us that it is best
    *          to echo it.
    * --------------------------------------------------------------------- */
   if ( p_auth_tx->MCF01_details.contact[0] == 0x02 )
   {
      /* Switched IN */
      ReqRsp_Buffer[11] = HOST_MESSAGE.header[12];                /* Field 7 */

      memcpy( &ReqRsp_Buffer[12], &HOST_MESSAGE.header[13], 2 );  /* Field 8 */
      memcpy( &ReqRsp_Buffer[14], &HOST_MESSAGE.header[15], 3 );  /* Field 9 */

      ReqRsp_Buffer[17] = HOST_MESSAGE.header[18];                /* Field 10*/

      memcpy( &ReqRsp_Buffer[18], &HOST_MESSAGE.header[19], 3 );  /* Field 11*/

      ReqRsp_Buffer[21] = HOST_MESSAGE.header[22];                /* Field 12*/
   }
   else
   {
      /* Switched OUT */
      memset(&ReqRsp_Buffer[11],0x00, 1 );   /* Round Trip Control,   Field 7 */
      memset(&ReqRsp_Buffer[12],0x00, 2 );   /* BASE II flags,        Field 8 */
      memset(&ReqRsp_Buffer[14],0x00, 3 );   /* Message Status Flags, Field 9 */
      memset(&ReqRsp_Buffer[17],0x00, 1 );   /* Batch Number,         Field 10*/
      memset(&ReqRsp_Buffer[18],0x00, 3 );   /* Reserved,             Field 11*/
      memset(&ReqRsp_Buffer[21],0x00, 1 );   /* User Information,     Field 12*/
   }
   return;
}

/*****************************************************************************

  Function:    empty_string

  Description: This function will determine if a string is completely nulls.
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
******************************************************************************/
BOOLEAN empty_string( pCHAR astr, INT max_len )
{
   INT i;
   for( i=0; i<max_len; i++ )
   {
      if ( astr[i] != 0x00 )
         return( false );
   }
   return( true );
}

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
   CHAR	    Buffer[512]={0};
   CHAR	    ErrorMsg[100]={0};
   BYTE      buffer1[sizeof(AUTH_TX)+sizeof(NCF01)];
   LONG      ret_code;

   /* Let racal know that this is from ncvisa */
   strcpy(p_auth_tx->TLF01_details.acquirer_id, "ncvis2");

   /* Build message for crypto. */
   memcpy( buffer1,p_auth_tx, Auth_Tx_Size );
   memcpy( buffer1+Auth_Tx_Size,&ncf01_i, Ncf01_Size );

   p_msg     = ptemsg_build_msg( MT_NSP_TRANSLATE_PIN, 
                                 ST1_NONE,
                                 ST2_NONE,
                                 security_que_name, 
                                 application_que_name,
                                 (pBYTE)&buffer1,
                                 sizeof(buffer1), 
                                 0 );
   if ( NULL_PTR == p_msg )
	{
      	ncvis2_log_message( 3, 2, "Insufficient Memory to build pin translation message" , "hhutil_process_debit_transaction" , 0 ,"");
		return(false);
	}
	ret_code = pteipc_send(p_msg, security_que_name);
	if( ret_code < 0 )
	{
	   pteipc_get_errormsg( ret_code, ErrorMsg );
      sprintf( Buffer, "error on pteipc_send to que %s: %s", security_que_name, ErrorMsg );
      ncvis2_log_message( 3, 3, Buffer , "hhutil_process_debit_transaction" , 0 ,"");
      free( p_msg );
      return(false);
	}
	
   free( p_msg );
   return(true);
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
   LONG   ret_code;
   CHAR	 Buffer[512] = {0};
   CHAR	 ErrorMsg[512] = {0};


   p_msg_host_out = ptemsg_build_msg(MT_AUTH_REQUEST,
                                     ST1_NONE, ST2_NONE,
                                     authorizer_que_name,
                                     application_que_name,
                                     (pBYTE)p_auth_tx,
                                     Auth_Tx_Size, 0);
   if ( NULL_PTR == p_msg_host_out )
   {
      ncvis2_log_message( 3, 2, "Insufficient Memory to build host message" , "hhutil_transfer_ctl_to_auth_app" , 0 ,"");
      return(false);
   }
   ret_code = pteipc_send(p_msg_host_out, authorizer_que_name);
   free( p_msg_host_out );
   if( ret_code < 0 )
   {
      pteipc_get_errormsg( ret_code, ErrorMsg );
      sprintf( Buffer,
              "hhutil_transfer_ctl: error on pteipc_send to que %s: %s",
               authorizer_que_name, ErrorMsg );
      ncvis2_log_message( 3, 3,Buffer , "hhutil_transfer_ctl_to_auth_app" , 0 ,"");
      return(false);
   }
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
   CHAR           ErrorMsg[512] = {0};
   CHAR           temp[17] = {0};
   pPTE_MSG       p_msg_ins = NULL_PTR;
   NCF30          ncf30;
   CHAR           Buff[512] = {0};
   BYTE           buffer1[sizeof(AUTH_TX)+sizeof(NCF30)] = {0};
   BYTE           subtype2 = 0;
   BYTE           ret_val = 0;
   LONG           ret_code = 0;
   LONG           ret_code1 = 0;


   /* This response_code will be set if field 4 not PESOs and field 5 not USD */
   if (0 == strncmp(p_auth_tx->TLF01_details.response_code, "91", 2))
   {
      ncvis2_log_message( 3, 1, "Currency code is not US dollars" , "hhutil_process_host_request" , 0 ,"");
      perform_error_processing( p_auth_tx );
      return(false);
   }

   if(true == ncvis2_check_for_acq_refund_transaction(p_auth_tx))
   {
	   memset(Log_Bufr,0x00,sizeof(Log_Bufr));
	   sprintf(Log_Bufr,"Received Refund transaction from cadencie, Term Id:%s ,Merch Id: %s RRN: %s ",
	         		  p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
	   strcat(ErrorMsg,Log_Bufr);
	   ncvis2_log_message( 3, 2, ErrorMsg , "hhutil_process_host_request" , 0 ,p_auth_tx->TLF01_details.card_num);

	   ret_code1 = ncvis2_transfer_refund_msg_to_VISA(p_auth_tx);
	   if(ret_code1 == false )
	   {
		   memset(Log_Bufr,0x00,sizeof(Log_Bufr));
		   sprintf(Log_Bufr," Failed to send refund transac to ncvisa, Term Id:%s ,Merch Id: %s RRN: %s ",
		         		  p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
		   strcat(ErrorMsg,Log_Bufr);
		   ncvis2_log_message( 3, 2, ErrorMsg , "hhutil_process_host_request" , 0 ,p_auth_tx->TLF01_details.card_num);
		   return false;
	   }
	   return true;
   }
   if(true == ncvis2_check_for_cadencie_acq_transaction(p_auth_tx))
   {
	   memset(Log_Bufr,0x00,sizeof(Log_Bufr));
	   sprintf(Log_Bufr,"Received  transaction req from cadencie, Term Id:%s ,Merch Id: %s RRN: %s ",
					  p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
	   strcat(ErrorMsg,Log_Bufr);
	   ncvis2_log_message( 3, 2, ErrorMsg , "hhutil_process_host_request" , 0 ,p_auth_tx->TLF01_details.card_num);

	   ret_code1 = ncvis2_transfer_refund_msg_to_VISA(p_auth_tx);
	   if(ret_code1 == false )
	   {
		   memset(Log_Bufr,0x00,sizeof(Log_Bufr));
		   sprintf(Log_Bufr," Failed to send  transaction to ncvisa, Term Id:%s ,Merch Id: %s RRN: %s ",
						  p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
		   strcat(ErrorMsg,Log_Bufr);
		   ncvis2_log_message( 3, 2, ErrorMsg , "hhutil_process_host_request" , 0 ,p_auth_tx->TLF01_details.card_num);
		   return false;
	   }
	   return true;
   }

   /* If a PIN is required, verify that one was entered */
   if ((AUTH_REVERSAL_ADVICE != p_auth_tx->TLF01_details.tx_key) && 
       (AUTH_REVERSAL        != p_auth_tx->TLF01_details.tx_key) &&
       (AUTH_ATM_CONFIRM     != p_auth_tx->TLF01_details.tx_key) &&
       (AUTH_REVERSAL_REPEAT != p_auth_tx->TLF01_details.tx_key))
   {
      /* If field 60.1 and field 18 both indicate ATM, then check PVS */
      if ( (ATM_TERMINAL == p_auth_tx->additional_pos_information[0]) &&
           (0 == strcmp(MCC_ATM, p_auth_tx->TLF01_details.category_code)) )
      {
         if (false == hhutil_verify_pin_block(p_auth_tx))
         {
            /* If Visa does Pin Verification Service (PVS) checking, then that
             * means that Visa already verified the Pin.  ATP does not need to.
             * In this case, DE39 is set to '00' and DEs 52 and 53 are not
             * sent to ATP.  So ATP will not verify the pin, but it still
             * needs to do mag stripe verification.  All we have to do here to
             * accomodate this requirement is to not return false if pin does
             * not exist.
             */
            if ( 0 != strcmp(p_auth_tx->TLF01_details.response_code,APPROVED) )
            {
               /* Invalid Response Code.  Decline and return to sender. */
               sprintf( ErrorMsg,
                       "PVS Txn: Invalid RC (%s), expecting '00'",
                        p_auth_tx->TLF01_details.response_code );

               memset(Log_Bufr,0x00,sizeof(Log_Bufr));
               sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
               		   p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
               strcat(ErrorMsg,Log_Bufr);
               ncvis2_log_message( 3, 2, ErrorMsg , "hhutil_process_host_request" , 0 ,p_auth_tx->TLF01_details.card_num);

               /* Build & send response message to the Visa host. */
               memcpy( p_auth_tx->TLF01_details.response_text, ErrorMsg, 40 );
               if ( p_auth_tx->TLF01_details.response_code[0] == 0x00 )
                  strcpy( p_auth_tx->TLF01_details.response_code, CALL_ISSUER );

               p_auth_tx->TLF01_details.general_status = GS_TXN_TRANSACTION_NOT_ALLOWED;

               if ( p_auth_tx->TLF01_details.tx_key < 100 )
                  p_auth_tx->TLF01_details.tx_key += 100;

               return( incoming_other_host_response_db_reply( p_auth_tx ) );
            }
         }
      }
   }

   /* Tell the authorizer to check CVV. if TRK2/TRK1 data is present*/
   if ( (p_auth_tx->TLF01_details.track2[0]        != 0x00) ||
        (p_auth_tx->TLF01_details.track1[0]        != 0x00) ||
        (p_auth_tx->TLF01_details.cvc_data[0]      != 0x00) ||
        (p_auth_tx->TLF01_details.interest_rate[0] != 0x00)  )
   {
      p_auth_tx->TLF01_details.verify_mag_stripe[0] = 'Y';
   }
   else 
      p_auth_tx->TLF01_details.verify_mag_stripe[0] = 'N';

   /* Set the following flag so on the response, we can tell if
    * CVV verification has been performed.  This affects DE44 in
    * the response.  Txcntl will overwrite this field if it
    * performs CVV verification.
    */
   p_auth_tx->TLF01_details.cvc = CVV_TEST_FLAG;

   if ( AUTH_REVERSAL        == p_auth_tx->TLF01_details.tx_key ||
        AUTH_REVERSAL_REPEAT == p_auth_tx->TLF01_details.tx_key ||
        AUTH_REVERSAL_ADVICE == p_auth_tx->TLF01_details.tx_key ||
        AUTH_ATM_CONFIRM     == p_auth_tx->TLF01_details.tx_key ||
        AUTH_ADVICE          == p_auth_tx->TLF01_details.tx_key   )
   {
      /* Display reversal message on Monitor.
       * This message is used to isolate problems to selected bins
       * or to certain merchants or to networks.
       */
      memset( Buff, 0x00, sizeof(Buff) );
      sprintf( Buff,
              "Reversal from VISA: %s. Card %s, Merchant %s",
               AppName,
               p_auth_tx->TLF01_details.card_num,
               p_auth_tx->TLF01_details.merchant_id );

      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      		p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buff,Log_Bufr);
      ncvis2_log_message( 2, 1, Buff , "hhutil_process_host_request" , 0 ,p_auth_tx->TLF01_details.card_num);

      /* Must get the original transaction for a reversal and advice.
       * If the original transaction is not found, reply to the
       * host for reversals and don't send the transaction to the authorizer.
       */
      memset( &ncf30, 0x00, Ncf30_Size );
      strcpy( ncf30.primary_key.network_id, AppName );
      strcpy( ncf30.primary_key.network_type, "A" );
      if ( AUTH_ADVICE == p_auth_tx->TLF01_details.tx_key )
      {
         strcpy(p_auth_tx->TLF01_details.message_type,"0120");
         subtype2 = ST2_NONE;
         strcpy( ncf30.primary_key.batch_number, "ACTIVE" );
         strcpy( ncf30.card_nbr, p_auth_tx->TLF01_details.card_num );
         strcpy( ncf30.primary_key.retrieval_ref_num,
                 p_auth_tx->TLF01_details.retrieval_ref_num );
      }
      else if ( AUTH_ATM_CONFIRM == p_auth_tx->TLF01_details.tx_key )
      {
         /* In Visa's 0102 message, only the RRN, terminal ID
          * and merchant
		  ID are guaranteed, along with the
          * processing code, to match the original transaction.
          *
          * Note:  This is a partial reversal.
          */
         subtype2 = ST2_DB_SELECT_NCF30_BY_RRN_AND_TERMINAL;
         strcpy( ncf30.primary_key.retrieval_ref_num,
                 p_auth_tx->TLF01_details.retrieval_ref_num );
         strcpy( ncf30.device_id, p_auth_tx->TLF01_details.terminal_id );
      }
      else
      {
         strcpy(p_auth_tx->TLF01_details.message_type,"0400");
         subtype2 = ST2_DB_SELECT_TRAN_BY_STAN;
         strcpy( ncf30.card_nbr, p_auth_tx->TLF01_details.card_num );
         strcpy( ncf30.tran_amount, p_auth_tx->TLF01_details.total_amount );
         strcpy( ncf30.sys_trace_audit_nbr,
                 p_auth_tx->TLF01_details.orig_sys_trace_num );

         if ( AUTH_REVERSAL        == p_auth_tx->TLF01_details.tx_key ||
              AUTH_REVERSAL_REPEAT == p_auth_tx->TLF01_details.tx_key  )
         {
            /* For reversals, set flag to not insert into TLF01 when performing
             * update all at completion of this transaction.  Reversals will get
             * inserted after sending to NETDS for the original txn, when the
             * response is sent to the device.
             */
            p_auth_tx->host2_acquirer_cb_code[0] = '*';
         }
      }

      /* Build message to get the original transaction from data server. */
      memcpy( buffer1,p_auth_tx, Auth_Tx_Size );
      memcpy( buffer1+Auth_Tx_Size,&ncf30, Ncf30_Size );
      ret_val = true;

      p_msg_ins = ptemsg_build_msg( MT_DB_REQUEST, 
                                    ST1_DB_SELECT, 
                                    subtype2,
                                    netds_que_name, 
                                    application_que_name,
                                    (pBYTE)&buffer1,
                                    sizeof(buffer1), 
                                    NCF30_DATA );
      if ( NULL_PTR == p_msg_ins  )
      {
         ncvis2_log_message( 3, 2, "Insufficient Memory for select ncf30 message" , "hhutil_process_host_request" , 0 ,"");
         ret_val = false;
      }
      else
      {
         /* Send message to get host specific data from data server. */
         ret_code = pteipc_send( p_msg_ins, netds_que_name );
         free( p_msg_ins );
         if ( 0 > ret_code )
         {
            pteipc_get_errormsg( ret_code, ErrorMsg );
            sprintf( Buff,"Failed transmit to queue: %s. %s",
                     netds_que_name,ErrorMsg );
            memset(Log_Bufr,0x00,sizeof(Log_Bufr));
            sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
            		p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
            strcat(Buff,Log_Bufr);
            ncvis2_log_message( 3, 3, Buff , "hhutil_process_host_request" , 0 ,p_auth_tx->TLF01_details.card_num);
            ret_val = false;
         }
      }

      if ( AUTH_REVERSAL        == p_auth_tx->TLF01_details.tx_key ||
           AUTH_REVERSAL_REPEAT == p_auth_tx->TLF01_details.tx_key  )
      {
         /* Send immediate response to Visa for reversals.
          * There is no response for reversal advices.
          */
         p_auth_tx->TLF01_details.tx_key = AUTH_REVERSAL_RESPONSE;
         strcpy( p_auth_tx->TLF01_details.response_code, "00" );

         /* Clear this flag so txn gets inserted into TLF01 in the next step. */
         p_auth_tx->host2_acquirer_cb_code[0] = 0x00;

         /* This function inserts into TLF01. */
         (void)incoming_other_host_response_db_reply( p_auth_tx );
      }
      return( ret_val );
   }

   if ( MCC_Validation_Functionality == MCC_VALIDATION_ON )
   {
      /* Validate Merchant Category Code (MCC) */
      ret_code = validate_mcc( p_auth_tx );
      if ( ret_code == false )
      {
         /* Invalid MCC.  Decline and return to sender. */
         strcpy( p_auth_tx->TLF01_details.response_code, CALL_ISSUER );
         p_auth_tx->TLF01_details.general_status = GS_TXN_TRANSACTION_NOT_ALLOWED;
         p_auth_tx->TLF01_details.tx_key += 100;

         /* Build & send response message to the Visa host. */
         return( incoming_other_host_response_db_reply( p_auth_tx ) );
      }
   }

   /* Process the authorization request from the host. */
   if (false == hhutil_transfer_ctl_to_auth_app(p_auth_tx))
   {
	memset(Log_Bufr,0x00,sizeof(Log_Bufr));
	sprintf(Log_Bufr," Unable to send the request to the authorizor application  Term Id:%s ,Merch Id: %s RRN: %s ",
			p_auth_tx->TLF01_details.terminal_id,
			p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      ncvis2_log_message( 3, 2, Log_Bufr , "hhutil_process_host_request" , 0 ,p_auth_tx->TLF01_details.card_num);
      return(false);
   }
   return(true);
}

/*****************************************************************************

  Function:    hhutil_send_host_response

  Description: This function will send the host response message out to the 
               VISA host.
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
   LONG   ret_code = 0;
   CHAR	 Buffer[512] = {0};
   CHAR	 ErrorMsg[512] = {0};
   INT buf_temp_len = 0;

   if(true == ncvis2_check_for_acq_refund_transaction(p_auth_tx) ||
	  true == ncvis2_check_for_cadencie_acq_transaction(p_auth_tx))
   {
		buf_temp_len = atoi(p_auth_tx->req_data_len);
		if(buf_temp_len < COMM_BUF_SZE && buf_temp_len > 0)
		{	buflen = buf_temp_len;
			memcpy(ReqRsp_Buffer,p_auth_tx->req_buffer,buflen);
		}
   }
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
	   ncvis2_log_message( 3, 3, "Insufficient Memory to build host message" , "hhutil_send_host_response" , 0 ,"");
	   return(false);
	}
   if ( p_auth_tx->TLF01_details.tpdu_id[0] != 0x00 )
      ptemsg_set_orig_comms_info(p_msg_out,p_auth_tx->TLF01_details.tpdu_id);

   else if ( ncf01_a.tpdu_id[0] != 0x00 )
      ptemsg_set_orig_comms_info(p_msg_out,ncf01_a.tpdu_id);

   else if ( p_auth_tx->TLF01_details.originator_info[0] != 0x00 )
      ptemsg_set_orig_comms_info(p_msg_out, p_auth_tx->TLF01_details.originator_info );

   else
   {
      ncvis2_log_message( 3, 2, "Do not know which host connection to send to." , "hhutil_send_host_response" , 0 ,"");
      free( p_msg_out );
      return(false);
   }

   ret_code = pteipc_send(p_msg_out, "dialogA");
   free( p_msg_out );
   if( ret_code < 0 )
   {
      pteipc_get_errormsg( ret_code, ErrorMsg );
      sprintf( Buffer, "error on pteipc_send to que %s: %s", "dialogA", ErrorMsg );
      ncvis2_log_message( 3, 3, Buffer , "hhutil_send_host_response" , 0 ,"");
      return(false);
	}
   else
   {
      /* Update transaction statistics */
      update_txn_stats( &Txn_Counts, p_auth_tx, SENT );
    
      /* Log transaction to trace file. */
      write_to_txn_file();
   }
   return(true);
}

/*****************************************************************************

  Function:    incoming_other_host_response

  Description: This function will process an incoming response from another host
               that needs to go to the VISA host.  Reversals get processed
               differently.  For reversal responses, perform an update_all
               to update any tables required by txcntl; get result, then delete
               the original transaction from NCF30 and respond with an approval
               to Visa.
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
        (AUTH_REVERSAL_REPEAT_RESPONSE == p_auth_tx->TLF01_details.tx_key) ||
		(AUTH_REVERSAL_ADVICE_REPEAT_RESPONSE == p_auth_tx->TLF01_details.tx_key) )
   {
      if ( 0 != strcmp(p_auth_tx->TLF01_details.response_code, NOT_PERMITTED) )
         strcpy( p_auth_tx->TLF01_details.response_code, "00" );

      ret_code = delete_orig_ncf30( &ncf30, p_auth_tx );
      if ( false == ret_code )
      {
         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr," Unable to delete reversal's original txn in NCF30 Term Id:%s ,Merch Id: %s RRN: %s ",
         		p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
         ncvis2_log_message( 3, 2, Log_Bufr , "incoming_other_host_response" , 0 ,p_auth_tx->TLF01_details.card_num);
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
 *  MODIFICATIONS:
 ******************************************************************************/
INT incoming_other_host_response_db_reply( pAUTH_TX p_auth_tx)
{
   pPTE_MSG   p_msg_ins = NULL_PTR;
   CHAR       temp[3] = "";
   CHAR       err_mesg[512] = "";
   CHAR       temp_amount[13];
   CHAR       temp_cur_code[4];
   INT		  iRet_code = true;

   if (AUTH_REVERSAL_REPEAT_RESPONSE == p_auth_tx->TLF01_details.tx_key)   
   {
      p_auth_tx->TLF01_details.tx_key = AUTH_REVERSAL_RESPONSE;
   }

   if (true == ncvis2_check_for_cadencie_acq_transaction(p_auth_tx)||
	  ((AUTH_ADVICE_RESPONSE != p_auth_tx->TLF01_details.tx_key) &&
       (AUTH_REVERSAL_ADVICE_RESPONSE != p_auth_tx->TLF01_details.tx_key)))
   {
      strncpy(temp_cur_code, p_auth_tx->TLF01_details.currency_code,
              sizeof(p_auth_tx->TLF01_details.currency_code));
      strncpy(temp_amount, p_auth_tx->TLF01_details.total_amount,
              sizeof(p_auth_tx->TLF01_details.total_amount));

      strncpy(p_auth_tx->TLF01_details.currency_code,
              p_auth_tx->reconciliation_cur_code,
              sizeof(p_auth_tx->TLF01_details.currency_code));
      strncpy(p_auth_tx->TLF01_details.total_amount,
              p_auth_tx->reconciliation_amount,
              sizeof(p_auth_tx->TLF01_details.total_amount));
      strncpy(p_auth_tx->TLF01_details.actual_amount,
              p_auth_tx->TLF01_details.orig_amount,
              sizeof(p_auth_tx->TLF01_details.actual_amount));

      build_host_structure(p_auth_tx);
      if (true == Build_Host_Request(p_auth_tx))
      {
         Build_Host_Header(p_auth_tx);
         if(false == hhutil_send_host_response(p_auth_tx))
         {
            memset(Log_Bufr,0x00,sizeof(Log_Bufr));
            sprintf(Log_Bufr," Error sending response to Visa host. Term Id:%s ,Merch Id: %s RRN: %s ",
            		p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
            ncvis2_log_message( 3, 2, Log_Bufr , "incoming_other_host_response_db_reply" , 0 ,p_auth_tx->TLF01_details.card_num);
            iRet_code = false;
         }
      }
      else
      {
         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr," Error building response for Visa host. Term Id:%s ,Merch Id: %s RRN: %s ",
         		p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
         ncvis2_log_message( 3, 2, Log_Bufr , "incoming_other_host_response_db_reply" , 0 ,p_auth_tx->TLF01_details.card_num);
         iRet_code = false;
      }
      strncpy(p_auth_tx->TLF01_details.currency_code,temp_cur_code,
              sizeof(p_auth_tx->TLF01_details.currency_code));
      strncpy(p_auth_tx->TLF01_details.total_amount,temp_amount,
             sizeof(p_auth_tx->TLF01_details.total_amount));
   }
   bcd_to_asc (ReqRsp_Buffer+22, 2, p_auth_tx->TLF01_details.message_type, 
               sizeof(p_auth_tx->TLF01_details.message_type));

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
   pBYTE         pCurrent;
   pPTE_MSG_DATA p_msg_data;
   BYTE          subtype1;
   BYTE          subtype2;
   BYTE          app_data_type;
   CHAR          buffer[512]={0};
   AUTH_TX       auth_tx;

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
          *  send the transaction to the authorizer.
          */
         if (false == process_incoming_message_ncf30_reply(pCurrent))
         {
            memset( &auth_tx, 0,        Auth_Tx_Size );
            memcpy( &auth_tx, pCurrent, Auth_Tx_Size );

            memset(Log_Bufr,0x00,sizeof(Log_Bufr));
            sprintf(Log_Bufr," Unable to send the request to the authorizor application Term Id:%s ,Merch Id: %s RRN: %s ",
            		auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
            ncvis2_log_message( 3, 2, Log_Bufr , "hhutil_parse_db_ok" , 0 ,auth_tx.TLF01_details.card_num);
         } 
      }
      else if ( TLF01_DATA == app_data_type )
      {
         /* A response to Visa was rejected.  We found the orig txn in TLF01.
          * Visa never got the response, so we need to reverse this txn out,
          * if it was approved.
          */
         memset( &auth_tx, 0x00,     Auth_Tx_Size );
         memcpy( &auth_tx, pCurrent, Auth_Tx_Size );
         memcpy( &auth_tx.TLF01_details, pCurrent+(Auth_Tx_Size), Tlf01_Size );

         if ( 0 == strcmp( auth_tx.TLF01_details.response_code,APPROVED) )
         {
            strcpy( auth_tx.TLF01_details.message_type, "0400" );

            auth_tx.TLF01_details.tx_key = AUTH_REVERSAL;

            strcpy( auth_tx.TLF01_details.orig_message,
                    auth_tx.TLF01_details.message_type );

            strcpy( auth_tx.orig_local_time_date,
                    auth_tx.TLF01_details.transmission_timestamp );

            strcpy( auth_tx.TLF01_details.response_text,
                   "Reversal for a Rejected Message" );

            /* Increment the last digit of the transaction id
             * for later insert into TLF01.
             */
            increment_tran_id( &auth_tx );

            if ( auth_tx.TLF01_details.orig_retrieval_ref_num[0] == 0x00 )
            {
               strcpy( auth_tx.TLF01_details.orig_retrieval_ref_num,
                       auth_tx.TLF01_details.retrieval_ref_num  );
            }

            if ( auth_tx.TLF01_details.orig_sys_trace_num[0] == 0x00 )
            {
               strcpy( auth_tx.TLF01_details.orig_sys_trace_num,
                       auth_tx.TLF01_details.sys_trace_audit_num );
            }

            /* Set up the amount to be reversed. */
            strcpy( auth_tx.TLF01_details.reversal_amount,
                    auth_tx.TLF01_details.total_amount );

            strcpy( auth_tx.TLF01_details.actual_amount, "000000000000" );

            /* Send reversal request to txcntl. */
            if ( false == hhutil_transfer_ctl_to_auth_app(&auth_tx) )
            {
               memset(Log_Bufr,0x00,sizeof(Log_Bufr));
               sprintf(Log_Bufr," Unable to send reversal for rejected txn Term Id:%s ,Merch Id: %s RRN: %s ",
               		  auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
               ncvis2_log_message( 3, 2, Log_Bufr , "hhutil_parse_db_ok" , 0 ,auth_tx.TLF01_details.card_num);
            }
         }
      }
      else
      {
         sprintf( buffer,
                 "Invalid app data type (%c) from data server.",
                  app_data_type );
         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
         		auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
         strcat(buffer,Log_Bufr);
         ncvis2_log_message( 3, 2, buffer , "hhutil_parse_db_ok" , 0 ,auth_tx.TLF01_details.card_num);
      }
   }
   else if (ST1_DB_INSERT == subtype1)
   {
      if (TLF01_DATA == app_data_type)
      {
         /* This is a reply to an insert TLF01 */
      }
   }
   else if (ST1_DB_DELETE == subtype1)
   {
   }
   else if (ST1_DB_UPDATE == subtype1)
   {
      if (ALL_DATA == app_data_type)
      {
         memset( &auth_tx, 0,        Auth_Tx_Size );
         memcpy( &auth_tx, pCurrent, Auth_Tx_Size );

         if ( false == ncvis2_check_for_cadencie_acq_transaction(&auth_tx)    &&
        	  ((auth_tx.TLF01_details.tx_key == AUTH_REVERSAL_RESPONSE      ) ||
              (auth_tx.TLF01_details.tx_key == AUTH_REVERSAL_REPEAT_RESPONSE) ||
              (auth_tx.TLF01_details.tx_key == AUTH_ADVICE_RESPONSE         ) ||
              (auth_tx.TLF01_details.tx_key == AUTH_ATM_CONFIRM_RESPONSE    ) ||
              (0 == strcmp("0130", auth_tx.TLF01_details.message_type)      )))
         {
            /* Already sent the response to host.
             * Already logged into TLF01.
             */
            return; 
         }
         else if((true == ncvis2_check_for_acq_refund_transaction(&auth_tx) 		   		&&
        		     0 == strncmp(auth_tx.TLF01_details.product_codes[1].code,"stndin",6))  ||
        		 (true == ncvis2_check_for_cadencie_acq_transaction(&auth_tx)				&&
        		     0 == strncmp(auth_tx.TLF01_details.response_code,"19",2)))
         {
        	 /* ncvis2 is not online dont send the response return from here.*/
        	   return;
         }
         else
         {
            (void)incoming_other_host_response_db_reply( &auth_tx );
         }
      }
   }
   else
   {
      memset( &auth_tx, 0,        Auth_Tx_Size );
      memcpy( &auth_tx, pCurrent, Auth_Tx_Size );

      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Invalid incoming message type  Term Id:%s ,Merch Id: %s RRN: %s ",
      		auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
      ncvis2_log_message( 3, 2, Log_Bufr , "hhutil_parse_db_ok" , 0 ,auth_tx.TLF01_details.card_num);
   }
   return;
} /* hhutil_parse_db_ok */

/*****************************************************************************

  Function:   hhutil_parse_db_error

  Description: This function determines which type of database request the
               incoming error message is from and routes it to the correct
               function.
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
void hhutil_parse_db_error(pPTE_MSG p_msg_in)
{
   pBYTE pCurrent,p_data;
   pPTE_MSG p_echo_msg;
   pPTE_MSG_DATA p_msg_data;
   BYTE  app_data_type=0;
   BYTE  subtype1=0;
   BYTE  subtype2=0;
   CHAR error_msg[512] = {0};
   AUTH_TX       auth_tx,org_auth_tx;
   CHAR Buffer[512] = {0};
   NCF30 ncf30;
   pBYTE orig_que;
   LONG data_len=0;

   p_msg_data = ptemsg_get_pte_msg_data(p_msg_in);
   pCurrent = ptemsg_get_pte_msg_data_data(p_msg_data);

   memset(&auth_tx,  0, Auth_Tx_Size);
   memcpy(&auth_tx,  pCurrent, Auth_Tx_Size);
   strcpy(error_msg, pCurrent+(Auth_Tx_Size));

   /*copy auth details into a new auth variable*/
    orig_que 	 = ptemsg_get_msg_orig_queue(p_msg_in);

   app_data_type = ptemsg_get_pte_msg_data_app_data_type(p_msg_data);
   subtype1 = ptemsg_get_msg_subtype1(p_msg_in);
   subtype2 = ptemsg_get_msg_subtype2(p_msg_in);


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
          * Reversals have already been responded to, while reversal
          * advices do not require a response.  Txn has already been
          * inserted into TLF01 for reversal, but not for reversal
          * advice.
          */
         if ( AUTH_REVERSAL        == auth_tx.TLF01_details.tx_key ||
              AUTH_REVERSAL_REPEAT == auth_tx.TLF01_details.tx_key )
         {
         }
         else if ( auth_tx.TLF01_details.tx_key == AUTH_ADVICE )
         {
             memset( &ncf30, 0x00, Ncf30_Size );
             (void)process_auth_advices( &auth_tx, NOT_FOUND, ncf30 );
         }
         else if ( auth_tx.TLF01_details.tx_key == AUTH_ATM_CONFIRM )
         {
            /* Could not find original txn.  No response necessary.
             * Log it and put out an error message.
             */
            insert_tlf01(&auth_tx);
            sprintf( Buffer, "Could not find original txn for 0102 msg. ");
            memset(Log_Bufr,0x00,sizeof(Log_Bufr));
            sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
            		auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
            strcat(Buffer,Log_Bufr);
            ncvis2_log_message( 3, 2, Buffer , "hhutil_parse_db_error" , 0 ,auth_tx.TLF01_details.card_num);
         }
         else if ( auth_tx.TLF01_details.tx_key == AUTH_AUTHORIZATION )
         {
             memset( &ncf30, 0x00, Ncf30_Size );
             (void)process_auth_repeat_part2( &auth_tx, NOT_FOUND, &ncf30 );
         }
         else
            insert_tlf01(&auth_tx);
      }
      else if (TLF01_DATA == app_data_type)
      {
         /* A response we sent to the Visa network was rejected due to
          * format error, or invalid value, or some other reason. We
          * attempted to find the orig txn in TLF01 to determine if we
          * should reverse it out. But we were unable to get the orig
          * txn.
          */
         sprintf( Buffer,
                 "Response to Visa rejected. Unable to get orig txn: %s",
                  error_msg );
         Buffer[99] = 0x00;
         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
         	    auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
         strcat(Buffer,Log_Bufr);
         ncvis2_log_message( 3, 2, Buffer , "hhutil_parse_db_error" , 0 ,auth_tx.TLF01_details.card_num);
      }
   }
   else if ( ST1_DB_UPDATE == subtype1 )
   {
      if ( ALL_DATA == app_data_type )
      {
         sprintf( Buffer,
                 "NCVISA:Problems with updating the database tables: %s",
                  error_msg);
         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
         		auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
         strcat(Buffer,Log_Bufr);
         ncvis2_log_message( 3, 2, Buffer , "hhutil_parse_db_error" , 0 ,auth_tx.TLF01_details.card_num);

         /* Response for Auth Advice and reversals have already been sent. */
         if ( (auth_tx.TLF01_details.tx_key == AUTH_REVERSAL       ) ||
              (auth_tx.TLF01_details.tx_key == AUTH_REVERSAL_REPEAT)  )
         {
            /* Do nothing */
         }
         else if ( (auth_tx.TLF01_details.tx_key == AUTH_ADVICE_RESPONSE) ||
                   (0 == strcmp("0130", auth_tx.TLF01_details.message_type)) )
         {
            /* Do nothing */
         }
         else
         {
            /* Respond with system error, log to TLF01. */
            strcpy( auth_tx.TLF01_details.response_code, "96" );
            (void)incoming_other_host_response_db_reply( &auth_tx );
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
      	             ncvis2_log_message( 3, 1, error_msg , "hutil_parse_db_error" , 0 ,org_auth_tx.TLF01_details.card_num);

      				org_auth_tx.TLF01_details.tx_key = AUTH_REVERSAL;
      				strncpy(org_auth_tx.TLF01_details.orig_message,org_auth_tx.TLF01_details.message_type,4);
      				strcpy(org_auth_tx.TLF01_details.message_type, "0400");
      				org_auth_tx.TLF01_details.general_status = GS_TXN_PERF_REQ;
      				strcpy( org_auth_tx.TLF01_details.orig_sys_trace_num, org_auth_tx.TLF01_details.sys_trace_audit_num);
      				strcpy( org_auth_tx.TLF01_details.orig_retrieval_ref_num, org_auth_tx.TLF01_details.retrieval_ref_num);
      				strncpy(org_auth_tx.TLF01_details.product_codes[3].quantity,TIMED_OUT,2);
      				memset( org_auth_tx.req_data_len, 0x00 , sizeof(org_auth_tx.req_data_len));
      				memset( org_auth_tx.req_buffer, 0x00 , sizeof(org_auth_tx.req_buffer));
      				memset( org_auth_tx.resp_data_len, 0x00 , sizeof(org_auth_tx.resp_data_len));
      				memset( org_auth_tx.resp_buffer, 0x00 , sizeof(org_auth_tx.resp_buffer));
      				/*TOR : Time out reversal -> it is used to drop the response 410 coming from host
      				 * for this reversal request sent for visa timeout
      				 *
      				 * */
      				if(strlen(ncf01_i.tpdu_id)> 0)
      				{
      					strncpy( org_auth_tx.TLF01_details.tpdu_id, ncf01_i.tpdu_id,sizeof(ncf01_i.tpdu_id));
      				}
      				else
      				{
      					strncpy( org_auth_tx.TLF01_details.tpdu_id,"VISA2",5);
      				}
      				strcpy(org_auth_tx.TLF01_details.response_text, "LR: Timed out reversal for 0110");
      				ncvis2_send_reversal_to_host_for_late_response( &org_auth_tx );
      			}
            }
   }
   else if (TLF01_DATA == app_data_type)
   {
      sprintf(Buffer,"Insert TLF01 error: %s",error_msg);
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      		auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncvis2_log_message( 3, 2, Buffer , "hhutil_parse_db_error" , 0 ,auth_tx.TLF01_details.card_num );
   }
   return;

} /*hhutil_parse_db_error*/

/*****************************************************************************

  Function:    hhutil_process_request

  Description: This function will send a request to the dataserver.  It is used
               by hhutil_get_ncf01_cfg.
  Author: 
      unknown
  Inputs:
      p_msg_out - a pointer to the outgoing message (the request to the dataserver)
      p_msg_in  - a pointer to the incoming message (from the dataserver)
      que_name  - the name of the queue to send the request to
      error_msg - the errror message from the dataserver
  Outputs:
      None
  Return values:
      dberror - the type of error
  Modified by:
******************************************************************************/
BYTE hhutil_process_request( pPTE_MSG p_msg_out, pPTE_MSG *p_msg_in, pCHAR que_name, pCHAR error_msg )
{
	LONG			data_len;
	pBYTE			p_data;
	pPTE_MSG_DATA	p_msg_data;
	CHAR			temp_str[100];
	LONG			retcode;
	BYTE			dberror;

	if( NULL_PTR == p_msg_out )
	{
		strcpy( error_msg, "Insufficient memory to build request message" );
		return PTEMSG_INSUFFICIENT_MEMORY;
	}

    *p_msg_in = pteipc_send_receive( p_msg_out, que_name, interactive_que_name, &retcode );
    if( NULL_PTR == *p_msg_in )
	{
		pteipc_get_errormsg( retcode, temp_str );
		sprintf( error_msg, "Communication Error during request to Terminal DataServer: %s", temp_str );
		return PTEMSG_IPC_ERROR;
	}

	if( ( dberror = ptemsg_get_result_code( *p_msg_in ) ) != PTEMSG_OK )
	{
		p_msg_data    = ptemsg_get_pte_msg_data( *p_msg_in );
		p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
		data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );

		strcpy (temp_str, p_data+Auth_Tx_Size);
		sprintf( error_msg, "Database Error: %s", temp_str );
		free( *p_msg_in );
	}

	return dberror;
}

/*****************************************************************************

  Function:    hhutil_get_ncf01_cfg

  Description: This function will send a request to the dataserver to select 
               NCF01.  This is done syncronously and should be used only by
               visamain.c!!!
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
	pPTE_MSG		p_msg_out;
	pPTE_MSG		p_msg_in;
	LONG			data_len;
	pBYTE			p_data;
	pPTE_MSG_DATA	p_msg_data;
	BYTE			rcode;
	BYTE     buffer[sizeof(AUTH_TX)+sizeof(NCF01)];
    AUTH_TX  auth_tx;

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


	if( ( rcode = hhutil_process_request( p_msg_out, &p_msg_in, netds_que_name, error_msg ) ) != PTEMSG_OK )	
		return ( rcode );

	p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
	p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
	data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );

   memset (pncf01, 0, Ncf01_Size);
   memcpy(pncf01, p_data+(Auth_Tx_Size), Ncf01_Size);
   
   free( p_msg_in );
	return ( PTEMSG_OK );
}

/*************************************************************************************/
/***************************************************************************************/
XINT ReadVisaTable(pCHAR TableName, pBYTE tmouts, pBYTE txns, pBYTE state, pBYTE prev_src)
{

   XINT           Rt, Ret;
   VISA_MEM       mem_crdtbl;
   LONG           CurrentPosition = 0L;

 
   Rt = MemAccess(TableName);
   if(Rt < 0)
      return(MEMACCESSERROR);
   memset(&mem_crdtbl, 0, sizeof(VISA_MEM));
   Ret = MemRead(Rt, CurrentPosition +1, sizeof(VISA_MEM), &mem_crdtbl, MEM_WAIT);
   strcpy(tmouts, mem_crdtbl.net_consec_tmouts_ctr);
   strcpy(txns, mem_crdtbl.active_txns_ctr);
   strcpy(state, mem_crdtbl.current_state);
   strcpy(prev_src, mem_crdtbl.prev_src_station);

   return(0);
      
}
/***************************************************************************************/
/***************************************************************************************/
LONG WriteVisaTable(pCHAR TableName, pBYTE tmouts, pBYTE txns, pBYTE state, pBYTE prev_src)
{
   XINT Rt, Ret,result;
   SECTION TempSec, RetSec;
   MEMINFOMEM MemInfo;
   LONG SectionSize = 0L, Current_Offset = 0L, Offset = 0L;
   VISA_MEM    mcrdtbl;
   
   
   Rt = MemAccess(TableName);
   if(Rt < 0)
      return(Rt);
   memset( &MemInfo, 0x00, sizeof(MEMINFOMEM) );
   Ret = MemInfoMem(Rt, &MemInfo);
   SectionSize = MemInfo.Size;
   Current_Offset = 0L;
   Ret = MemLock(MEM_ALL, MemList(*MemSectionBuild(&TempSec, Rt, Offset, SectionSize),
                                   MEM_EOL), &RetSec, MEM_WAIT);
   Ret = MemWrite(Rt, 0L, SectionSize, MEM_FILL(0x00), MEM_WAIT);
   strcpy( mcrdtbl.net_consec_tmouts_ctr, tmouts );
   strcpy( mcrdtbl.active_txns_ctr, txns);
   strcpy( mcrdtbl.current_state, state);
   strcpy( mcrdtbl.prev_src_station, prev_src);
   Ret = MemWrite(Rt, Current_Offset + 1, sizeof(mcrdtbl), &mcrdtbl, MEM_WAIT);
   result = MemUnlock(MemList(*MemSectionBuild(&TempSec, Rt, Offset, SectionSize),
                                  MEM_EOL), &RetSec);
   if(Ret < 0)
   {
      return(Ret);
   }
   
   return(0L);
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
   {"02Refer to Card Issuer Special Cond"},
   {"03Invalid Merchant"},   
   {"04Pick-Up Card"},

   {"05Do Not Honor"},
   {"06Error"},   
   {"07Pick-Up, Special Condition"},   
   {"09Request In Progress"},   
   {"11VIP Approval"},   

   {"12Invalid Transaction"},   
   {"13Invalid Amount"},   
   {"14Invalid Account Number"},   
   {"15No Such Issuer"},   
   {"17Customer Cancellation"},   

   {"19Re-enter transaction"},   
   {"20Invalid Response"},   
   {"21No Action Taken"},   
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
      strcpy(p_auth_tx->TLF01_details.voice_auth_text, response[1]+2);
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

/*****************************************************************************

  Function:    VERIFY_AUTH_TX_DATA

  Description: This function validates data in an auth_tx buffer.  It verifies
               numeric fields are numeric; it verifies alphanumeric fields are
               alphanumeric; and it verifies the processing code is for a
               transaction that ATP supports for Equitable.

  Author: 
      unknown
  Inputs:
      p_auth_tx - Contains data to be validated
  Outputs:
      None
  Return values:
      true  - indicates data is good
      false - indicates at least one piece of data is bad.
  Modified by:
      BGable      6/30/98
******************************************************************************/
INT verify_auth_tx_data(pAUTH_TX p_auth_tx)
{
   CHAR  Buffer[512]={0};
   
   /* FIELD 2: PAN */ 
   if( false == isnum(p_auth_tx->TLF01_details.card_num)) 
   {
      sprintf(Buffer, "Invalid data for card number: %s",
              p_auth_tx->TLF01_details.card_num);

      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      		p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncvis2_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);
                                    
      return(false);
   }

   /* FIELD 3: PROCESSING CODE */
   if( false == isnum(p_auth_tx->TLF01_details.processing_code))
   {
      sprintf(Buffer, "Invalid data for processing code: %s",
              p_auth_tx->TLF01_details.processing_code);
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      					p_auth_tx->TLF01_details.terminal_id,
      					p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncvis2_log_message( 3, 2, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);
      return(false);
   }
   if (0 == strcmp(p_auth_tx->TLF01_details.message_type,"0100"))
      if (0 != strncmp(p_auth_tx->TLF01_details.processing_code,"00", 2))
         if (0 != strncmp(p_auth_tx->TLF01_details.processing_code,"11", 2))
            if (0 != strncmp(p_auth_tx->TLF01_details.processing_code,"01",2))
               if (0 != strncmp(p_auth_tx->TLF01_details.processing_code,"30",2))
            		   if (0 != strncmp(p_auth_tx->TLF01_details.processing_code,"20",2))
            			   if (0 != strncmp(p_auth_tx->TLF01_details.processing_code,"28",2))
            				   if (0 != strncmp(p_auth_tx->TLF01_details.processing_code,"10",2))
            {
               sprintf(Buffer, "Invalid data for processing code: %s",
                       p_auth_tx->TLF01_details.processing_code);
               memset(Log_Bufr,0x00,sizeof(Log_Bufr));
               sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
               					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
               strcat(Buffer,Log_Bufr);
               ncvis2_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);
               return(false);
            }

   /* FIELD 4: TRANSACTION AMOUNT */
   if( false == isnum(p_auth_tx->TLF01_details.total_amount))
   {
      sprintf(Buffer, "Invalid data for total amount: %s",
              p_auth_tx->TLF01_details.total_amount); 
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncvis2_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);
      return(false);
   }

   /* FIELD 5: SETTLEMENT AMOUNT */
   if( false == isnum(p_auth_tx->TLF01_details.settlement_total))
   {
      sprintf(Buffer, "Invalid data for settlement total: %s",
              p_auth_tx->TLF01_details.settlement_total);
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      		  p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncvis2_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);
      return(false);
   }

   /* FIELD 7: TRANSMISSION DATE AND TIME */
   if( false == isnum(p_auth_tx->TLF01_details.transmission_timestamp))
   {
      sprintf(Buffer, "Invalid data for transmission timestamp: %s",
              p_auth_tx->TLF01_details.transmission_timestamp);
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      		p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncvis2_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);
      return(false);
   }

   /* FIELD 11: STAN */
   if( false == isnum(p_auth_tx->TLF01_details.sys_trace_audit_num))
   {
      sprintf(Buffer, "Invalid data for system trace audit number: %s",
              p_auth_tx->TLF01_details.sys_trace_audit_num);
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      		p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncvis2_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);
      return(false);
   }

   /* FIELD 14: EXPIRATION DATE */
   if( false == isnum(p_auth_tx->TLF01_details.exp_date))
   {
      sprintf(Buffer, "Invalid data for expiration date: %s",
              p_auth_tx->TLF01_details.exp_date);
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      		p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncvis2_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);
      return(false);
   }

   /* FIELD 19: ACQ INST COUNTRY CODE */
   if( false == isnum(p_auth_tx->country_code))
   {
      sprintf(Buffer,
             "Invalid data for acqiring institution country code: %s",
              p_auth_tx->country_code);
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      		p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncvis2_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);
      return(false);
   }
   /* FIELD 26: CARD ACCEPTOR BUSINESS CODE */
   if( false == isnum(p_auth_tx->TLF01_details.category_code))
   {
      sprintf(Buffer, "Invalid data for category code: %s",
              p_auth_tx->TLF01_details.category_code);
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      		  p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncvis2_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);
      return(false);
   }

   /* FIELD 32: ACQUIRING INST ID CODE */
   if( false == isnum(p_auth_tx->acq_inst_id_cd))
   {
      sprintf(Buffer,
             "Invalid data for acquiring institution ID code: %s",
              p_auth_tx->acq_inst_id_cd);
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      		p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncvis2_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);
      return(false);
   }

   /* FIELD 33: FORWARDING INST ID CODE */
   if( false == isnum(p_auth_tx->forwarding_inst_id_cd))
   {
      sprintf(Buffer, "Invalid data for forwarding institution ID code: %s", p_auth_tx->forwarding_inst_id_cd);
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      					p_auth_tx->TLF01_details.terminal_id,
      					p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncvis2_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);
      return(false);
   }
   /* FIELD 37: RRN */
   if( false == isalphanum(p_auth_tx->TLF01_details.retrieval_ref_num))
   {
      sprintf(Buffer, "Invalid data for retrieval reference number: %s", p_auth_tx->TLF01_details.retrieval_ref_num);
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      		 p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncvis2_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);
      return(false);
   }
   /* FIELD 38: AUTH CODE */
   if( false == isalphanum_space(p_auth_tx->TLF01_details.auth_number))
   {
      sprintf(Buffer, "Invalid data for approval code: %s",
              p_auth_tx->TLF01_details.auth_number);
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      		 p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncvis2_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);
      return(false);
   }

   /* FIELD 39: RESPONSE CODE */
   if( false == isalphanum(p_auth_tx->TLF01_details.response_code))
   {
      sprintf(Buffer, "Invalid data for action code: %s",
              p_auth_tx->TLF01_details.response_code);
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      					p_auth_tx->TLF01_details.terminal_id,
      					p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncvis2_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);
      return(false);
   }

   /* FIELD 49: CURRENCY CODE, TRANSACTION */
   if( false == isnum(p_auth_tx->TLF01_details.currency_code))
   {
      sprintf(Buffer, "Invalid data for transaction currency code: %s",
              p_auth_tx->TLF01_details.currency_code);
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      		  p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncvis2_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);
      return(false);
   }
   return(true);
}


/******************************************************************************
 *
 *  NAME:         INSERT_TLF01
 *
 *  DESCRIPTION:  This function inserts a transaction into TLF01.
 *
 *  INPUTS:       p_auth_tx - This is the transaction
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   true if successful, else false
 *
 *  AUTHOR:       D. Irby
 *
 ******************************************************************************/
INT insert_tlf01( pAUTH_TX p_auth_tx )
{
   pPTE_MSG   p_msg_ins = NULL_PTR;
   BYTE       buffer[sizeof(AUTH_TX)+sizeof(TLF01)];
   LONG       ret_code;
   CHAR       Buff[512]={0};
   CHAR       err_mesg[512] = {0};
   INT        ret_val;
	
   if ((strcmp(p_auth_tx->TLF01_details.originator_queue,"stpmgr") == 0)) // ThoughtFocus Girija Y
   {
	   /* Insert in STIP manager module */
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

      if ( p_auth_tx->TLF01_details.primary_key.transaction_id[0] == 0x00 )
      {
         /* Tran ID might be missing in the event of parsing errors. */
         init_txn( p_auth_tx );
      }

      memset(buffer, 0, sizeof(buffer));
      ptetime_get_strtime(p_auth_tx->TLF01_details.tran_finish_time);
      memcpy( buffer, p_auth_tx, Auth_Tx_Size );
      memcpy(buffer+Auth_Tx_Size,&p_auth_tx->TLF01_details, Tlf01_Size);

      p_msg_ins = ptemsg_build_msg(MT_DB_REQUEST, 
                                   ST1_DB_INSERT, 
                                   ST2_NONE,
                                   updateds_que_name, 
                                   application_que_name,
                                   (pBYTE)&buffer,
                                   sizeof(buffer), 
                                   TLF01_DATA); 
      if(NULL_PTR == p_msg_ins)
      {
         ncvis2_log_message( 3, 2, "Insufficient Memory to build insert TLF01 message" , "insert_tlf01" , 0 ,"");
         ret_val = false;
      }
      else
      {
         ret_code = pteipc_send(p_msg_ins, updateds_que_name); 
         free(p_msg_ins);
         if(ret_code < 0)
         {
            pteipc_get_errormsg(ret_code, err_mesg);
            sprintf( Buff,
                    "Failed transmit to que: %s. %s",
                     updateds_que_name,err_mesg );
            ncvis2_log_message( 3, 2, Buff, "insert_tlf01" , 0 ,"");
            ret_val = false;
         }
         else
            ret_val = true;
      }
   }
   return( ret_val );
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
 ******************************************************************************/
LONG validate_mcc( pAUTH_TX p_auth_tx )
{
   LONG ret_code = true;
   INT  mcc;
   INT  len;
   INT  i;
   CHAR tempstr[512]={0};

   len = strlen( p_auth_tx->TLF01_details.category_code );
   if ( len != bf18_len )
   {
      ret_code = false;
      memset( tempstr, 0x00, sizeof(tempstr) );
      sprintf(tempstr,"Invalid MCC: %s",p_auth_tx->TLF01_details.category_code);
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      		p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(tempstr,Log_Bufr);
      ncvis2_log_message( 3, 2, tempstr , "validate_mcc" , 0 ,p_auth_tx->TLF01_details.card_num);
   }
   else
   {
      /* Make sure MCC is numeric. */
      for( i=0; i<4; i++ )
      {
         if ( !isdigit( p_auth_tx->TLF01_details.category_code[i] ) )
         {
            ret_code = false;
            memset( tempstr, 0x00, sizeof(tempstr) );
            sprintf(tempstr,"Invalid MCC: %s",
                    p_auth_tx->TLF01_details.category_code);
            memset(Log_Bufr,0x00,sizeof(Log_Bufr));
            sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
            		p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
            strcat(tempstr,Log_Bufr);
            ncvis2_log_message( 3, 2, tempstr , "validate_mcc" , 0 ,p_auth_tx->TLF01_details.card_num);
         }
      }

      if ( ret_code == true )
      {
         /* See if track data exists. */
         if ( (HOST_MESSAGE.track1_len[0] == 0x00) &&
              (HOST_MESSAGE.track2_len[0] == 0x00)  )
         {
            /* No track data.  Need to verify the MCC. */
            mcc = atoi( p_auth_tx->TLF01_details.category_code );

            if ( ((MCC_RANGE1_LOW <= mcc) && (mcc <= MCC_RANGE1_HIGH))  ||
                 ((MCC_RANGE2_LOW <= mcc) && (mcc <= MCC_RANGE2_HIGH))  ||
                 (mcc == 7011) || (mcc == 6010) || (mcc == 6051)         )
            {
               ret_code = true;
            }
            else
            {
               ret_code = false;
               memset( tempstr, 0x00, sizeof(tempstr) );
               sprintf(tempstr,"Invalid MCC: %s.  No Track Data.",
                       p_auth_tx->TLF01_details.category_code);
               memset(Log_Bufr,0x00,sizeof(Log_Bufr));
               sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
               		p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
               strcat(tempstr,Log_Bufr);
               ncvis2_log_message( 3, 2, tempstr , "validate_mcc" , 0 ,p_auth_tx->TLF01_details.card_num);
            }
         }
      }
   }
   return(ret_code);
}

/******************************************************************************
 *
 *  NAME:         PARSE_TRACK2
 *
 *  DESCRIPTION:  This function parses track2 data.  It looks for the first
 *                non-numeric character to determine the field separator.
 *                It compares to anything in the card number field in auth_tx.
 *                It compares the expiration date to anything in the exp date
 *                field in auth_tx.  If empty, it copies them into those fields.
 *                It also copies the service code into a global variable in
 *                order to check for a Visa Electron transaction.
 *
 *  INPUTS:       p_auth_tx - Structure containing transaction information
 *
 *  OUTPUTS:      error_msg - Textual message in case of failure
 *
 *  RTRN VALUE:   True if track2 is successfully parsed, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT parse_track2( pAUTH_TX p_auth_tx, pCHAR error_msg )
{
   INT    ret_val = true;
   INT    card_num_len;
   INT    track2_idx;
   pBYTE  field_separator   = NULL_PTR;

   if ( p_auth_tx->TLF01_details.track2[0] != 0x00 )
   {
      /* Locate the field separator. */
      field_separator = p_auth_tx->TLF01_details.track2;
      for(track2_idx=0;track2_idx <= bf2_max_len;track2_idx++,field_separator++)
      {
         if ( !isdigit( *field_separator ) )
            break;
      }

      if ( track2_idx > bf2_max_len )
      {
         ret_val = false;
         sprintf( error_msg,
                 "Unable to parse track2. No field separator. Track2: %s",
                  p_auth_tx->TLF01_details.track2 );
      }
      else
      {
         /* Put Card Number into auth_tx. */
         if ( p_auth_tx->TLF01_details.card_num[0] == 0x00 )
         {
            card_num_len = field_separator - p_auth_tx->TLF01_details.track2;

           /* If track2 length is odd, ignore leading zero on card number. */
               memcpy( p_auth_tx->TLF01_details.card_num,
                       p_auth_tx->TLF01_details.track2, card_num_len);
            
            sprintf( p_auth_tx->TLF01_details.card_num_len,"%02d",card_num_len);
         }

         /* Get the expiration date. */
         if ( p_auth_tx->TLF01_details.exp_date[0] == 0x00 )
         {
            memcpy( p_auth_tx->TLF01_details.exp_date, field_separator+1, 4 );
         }

         /* Get the service code. */
         memset( ServiceCode, 0x00, SERVICE_CODE_SIZE_PLUS_1 );
         memcpy( ServiceCode, field_separator+5, 3 );
      }
   }
   return( ret_val );
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
   CHAR            Buff[512]= {0};
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
 *  AUTHOR:       Dirby
 *
 ******************************************************************************/
void init_txn( pAUTH_TX p_auth_tx )
{
   CHAR   unique_tran_str_id[21] = {0};
   CHAR   date_str [ 9] = {0};
   CHAR   time_str [ 7] = {0};
   CHAR   time_date[25] = {0};
   CHAR   temp_unique_tran[15]={0};
   INT	   tid_len = 0;
   INT	   unique_tran_str_id_len=0;

	/* Get local time. */
   if ( p_auth_tx->TLF01_details.date_yyyymmdd[0] == 0x00 )
   {
	  ptetime_get_timestamp( time_date );
	  get_date( time_date, date_str );
	  get_time( time_date, time_str );
	  strcpy( p_auth_tx->TLF01_details.date_yyyymmdd, date_str );
	  strcpy( p_auth_tx->TLF01_details.time_hhmmss,   time_str );
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
   }
   /* Initialize some of the auth_tx fields. */
   p_auth_tx->TLF01_details.general_status		=  GS_TXN_NEW;
   p_auth_tx->TLF01_details.entry_type			=  FINANCIAL;
   strcpy( p_auth_tx->TLF01_details.dispensation.origin, "RO" );
   strcpy( p_auth_tx->TLF01_details.dispensation.auth_2, "RA" );
   strcpy( p_auth_tx->TLF01_details.issuer_id,			  AppName);
   strcpy( p_auth_tx->TLF01_details.handler_queue,		  application_que_name);
   strcpy( p_auth_tx->TLF01_details.source_key, 		  ncf01_a.cwk);
   strcpy( p_auth_tx->TLF01_details.source_pin_format,	  ncf01_a.pin_type);

   if ( p_auth_tx->TLF01_details.tran_start_time[0] == 0x00 )
	  strncpy( p_auth_tx->TLF01_details.tran_start_time, unique_tran_str_id,16 );

   /* Create RRN */
   if ( p_auth_tx->TLF01_details.retrieval_ref_num[0] == 0x00 )
   {
	  create_rrn( p_auth_tx->TLF01_details.retrieval_ref_num, p_auth_tx );
   }
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
         ncvis2_log_message( 3, 1, err_buf , "write_to_txn_file" , 0 ,"");

         if ( rc == -1 )
            strcpy( err_buf,
                   "Unable to write txn into trace file. Setting trace off.");
         else if ( rc == -2 )
            strcpy( err_buf, "Unable to close trace file. Setting trace off.");
         else if ( rc == -3 )
            strcpy( err_buf, "Unable to open trace file. Setting trace off." );
         else
            sprintf(err_buf,"Unknown return value: '%d' Setting trace off.",rc);

         ncvis2_log_message( 3, 1, err_buf , "write_to_txn_file" , 0 ,"");

         /* Close the trace file. */
         if ( false == close_trace_file( ppTraceFile_fp ))
         {
            sprintf( err_buf, "Unable to close trace file %s", TraceFile );
            ncvis2_log_message( 3, 1, err_buf , "write_to_txn_file" , 0 ,"");
         }
      }
   }
   return;
}


/*****************************************************************************
 *
 *  Function:    FIELD126PARSER
 *
 *  Description: This function parses field 126 from Visa and puts it into
 *               'outstring' in ASCII format, including the bitmap.  The
 *               bitmap in 'outstring' will be 16 ASCII bytes.
 *
 * Author:       D. Irby
 *
 * Inputs:       HOST_MESSAGE.de126 (Global)
 *
 * Outputs:      outstring - DE126 in ASCII format.
 *               cvv2_data - Data from field 126.10
 *
 * Return values: None
 *
 ******************************************************************************/
void Field126Parser(  pAUTH_TX p_auth_tx, pBYTE cvv2_data  )
{
   #define BCD_BITMAP_LEN   8
   #define ASC_BITMAP_LEN  16

   INT     i;
   INT     cps_bit;
   INT     cps_byte;
   INT     TotalLen;
   INT     inptr  = BCD_BITMAP_LEN;   /* Point to inbuf, past bitmap. */
   INT     outptr = ASC_BITMAP_LEN;   /* Point to outstring, past bitmap */
   BYTE    bcd_BitMap[8] ={0};
   BYTE    asc_BitMap[17]={0};
   BYTE    inbuf[255]    ={0};
   BYTE    tempstr[58]	 ={0};
   BYTE	   outstring[241]={0};


   /* Get length of field. */
   TotalLen = (INT)HOST_MESSAGE.de126_len[0];
   memcpy( inbuf, HOST_MESSAGE.de126, TotalLen );

   /* Copy bitmap into local variable. */
   memcpy( bcd_BitMap, HOST_MESSAGE.de126, BCD_BITMAP_LEN );

   /* Convert bitmap to ASCII. */
   bcd_to_asc( bcd_BitMap, BCD_BITMAP_LEN, asc_BitMap, ASC_BITMAP_LEN );
   memcpy( outstring, asc_BitMap, ASC_BITMAP_LEN );

   /* Parse the bitmap for the first 2 bytes only - others are unused. */
   for( cps_byte=1; cps_byte<=2; cps_byte++ )
   {
      for( cps_bit=1; cps_bit<=8; cps_bit++ )
      {
         if ( bcd_BitMap[cps_byte-1] & BITTAB[cps_bit].bit_mask )
         {
            /* The bit is set. */
            switch( cps_byte )
            {
               case 1: /* Byte 1 */
                      switch( cps_bit )
                      {
                         case 1:  
                                /* Customer Name */
                                memset(tempstr, 0x00, sizeof(tempstr) );
                                for( i=0; i<25; i++ )
                                {
                                   tempstr[i] =
                                      ascii_ebcdic_conv( inbuf[inptr+i], E2A );
                                }
                                strcat( outstring+outptr, tempstr );
                                inptr  += 25;
                                outptr += 25;
                                break;
                         case 2:
                                /* Customer Address */
                                memset(tempstr, 0x00, sizeof(tempstr) );
                                for( i=0; i<57; i++ )
                                {
                                   tempstr[i] =
                                      ascii_ebcdic_conv( inbuf[inptr+i], E2A );
                                }
                                strcat( outstring+outptr, tempstr );
                                inptr  += 57;
                                outptr += 57;
                                break;
                         case 3:
                                /* Biller Address */
                                memset(tempstr, 0x00, sizeof(tempstr) );
                                for( i=0; i<57; i++ )
                                {
                                   tempstr[i] =
                                      ascii_ebcdic_conv( inbuf[inptr+i], E2A );
                                }
                                strcat( outstring+outptr, tempstr );
                                inptr  += 57;
                                outptr += 57;
                                break;
                         case 4:
                                /* Biller Telephone Number */
                                memset(tempstr, 0x00, sizeof(tempstr) );
                                for( i=0; i<18; i++ )
                                {
                                   tempstr[i] =
                                      ascii_ebcdic_conv( inbuf[inptr+i], E2A );
                                }
                                strcat( outstring+outptr, tempstr );
                                inptr  += 18;
                                outptr += 18;
                                break;
                         case 5:
                                /* Process by Date */
                                memset(tempstr, 0x00, sizeof(tempstr) );
                                for( i=0; i<6; i++ )
                                {
                                   tempstr[i] =
                                      ascii_ebcdic_conv( inbuf[inptr+i], E2A );
                                }
                                strcat( outstring+outptr, tempstr );
                                inptr  += 6;
                                outptr += 6;
                                break;
                         case 6:
                                /* Cardholder Certificate Serial Number */
                                memset(tempstr, 0x00, sizeof(tempstr) );
                                genutil_bin_to_ascii( &inbuf[inptr], 1,
                                                      tempstr, sizeof(tempstr));
                                strncat( outstring+outptr, tempstr, 2 );
                                inptr++;
                                outptr += 2;
                                memset(tempstr, 0x00, sizeof(tempstr) );
                                bcd_to_asc( &inbuf[inptr], 16, tempstr, 32 );
                                strcat( outstring+outptr, tempstr );
                                inptr  += 16;
                                outptr += 32;
                                break;
                         case 7:
                                /* Merchant Certificate Serial Number */
                                memset(tempstr, 0x00, sizeof(tempstr) );
                                genutil_bin_to_ascii( &inbuf[inptr], 1,
                                                      tempstr, sizeof(tempstr));
                                strncat( outstring+outptr, tempstr, 2 );
                                inptr++;
                                outptr += 2;
                                memset(tempstr, 0x00, sizeof(tempstr) );
                                bcd_to_asc( &inbuf[inptr], 16, tempstr, 32 );
                                strcat( outstring+outptr, tempstr );
                                inptr  += 16;
                                outptr += 32;
                                break;
                         case 8:
                                /* Transaction ID (VSEC) */
                                memset(tempstr, 0x00, sizeof(tempstr) );
                                genutil_bin_to_ascii( &inbuf[inptr], 20,
                                                      tempstr, sizeof(tempstr));
                                strcat( outstring+outptr, tempstr );
                                inptr  += 20;
                                outptr += 20;
                                break;
                      }
                      break;
               case 2: /* Byte 2 */
                      switch( cps_bit+8 )
                      {
                         case  9:
                                /* TranStain (VSEC) */
                                memset(tempstr, 0x00, sizeof(tempstr) );
                                genutil_bin_to_ascii( &inbuf[inptr], 20,
                                                      tempstr, sizeof(tempstr));
                                strcat( outstring+outptr, tempstr );
                                inptr  += 20;
                                outptr += 20;
                                break;
                         case 10:
                                /* CVV2 Request Data */
                                memset(tempstr, 0x00, sizeof(tempstr) );
                                for( i=0; i<6; i++ )
                                {
                                   tempstr[i] =
                                      ascii_ebcdic_conv( inbuf[inptr+i], E2A );
                                }
                                inptr  += 6;

                                strcpy( cvv2_data, tempstr );
                                break;
                         case 11:
                                /* IGOTS Transaction Description */
                                memset(tempstr, 0x00, sizeof(tempstr) );
                                for( i=0; i<2; i++ )
                                {
                                   tempstr[i] =
                                      ascii_ebcdic_conv( inbuf[inptr+i], E2A );
                                }
                                strcat( outstring+outptr, tempstr );
                                inptr  += 2;
                                outptr += 2;
                                break;
                         case 12:
                                /* Service Development Indicator */
                                strncat( outstring+outptr, &inbuf[inptr], 24 );
                                break;
                      }
                      break;
               default: break;
            }  /* End SWITCH based on BYTE */
         }  /* End IF bit is set */
      }  /* End LOOP on BIT */
   }  /* End LOOP on BYTE */
    memcpy( p_auth_tx->TLF01_details.voice_auth_text,outstring ,sizeof(outstring));

}

/*****************************************************************************
 *
 *  Function:    GET_BALANCES
 *
 *  Description: This function copies balance information from a balance
 *               structure into auth_tx for current and available balances.
 *
 * Author:       D. Irby
 *
 * Inputs:       p_acct_bal - Structure containing account balances.
 *
 * Outputs:      p_auth_tx  - Transaction structure
 *
 * Return values: None
 *
 ******************************************************************************/
void get_balances( pAddl_Amts p_acct_bal, pAUTH_TX p_auth_tx )
{
   /* Currency Code */
   memcpy( p_auth_tx->TLF01_details.currency_code, p_acct_bal->cur_code, 3 );

   if ( p_acct_bal->amt_type[1] == '1' )
   {
      /* --------------- */
      /* CURRENT BALANCE */
      /* --------------- */
      memcpy( p_auth_tx->ACF01_details.cash_available,
              p_acct_bal->amount+3, 9 );

      /* Sign */
      if ( p_acct_bal->sign == 'C' )
         p_auth_tx->current_balance_sign = '+';
      else
         p_auth_tx->current_balance_sign = '-';
   }
   else
   {
      /* ----------------- */
      /* AVAILABLE BALANCE */
      /* ----------------- */
      memcpy( p_auth_tx->ACF01_details.cash_limit,
              p_acct_bal->amount+3, 9 );

      /* Sign */
      if ( p_acct_bal->sign == 'C' )
         p_auth_tx->available_balance_sign = '+';
      else
         p_auth_tx->available_balance_sign = '-';
   }
   return;
}

/*****************************************************************************

  Function:    FIND_CAVV_FLAG

  Description: This function will dig the CAVV result value out of field
               44 response data.  This value is in byte 15.

  Author:      D. Irby

  Inputs:      len  - Length of DE44
               de44 - Field 44 data

  Outputs:     cavv_result - Value defining result of Cardholder Authentication
                             Verification Value (CAVV)

  Return values: None

******************************************************************************/
void find_cavv_flag( BYTE len, pBYTE de44, pBYTE cavv_result )
{
   if ( len > CAVV_RESULT_LOC )
   {
      *cavv_result = de44[CAVV_RESULT_LOC];
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
 *  AUTHOR:       Dennis Irby
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

/*****************************************************************************

  Function:    FIND_CARC_FLAG

  Description: This function will dig the Card Authentication Result code
               out of field 44 response data.  This value is in byte 9.

  Author:      D. Irby

  Inputs:      len  - Length of DE44
               de44 - Field 44 data

  Outputs:     carc_result - Value defining result of Cardholder Authentication
                             Result Code

  Return values: None

******************************************************************************/
void find_carc_flag( BYTE len, pBYTE de44, pBYTE carc_result )
{
   if ( len > CARC_RESULT_LOC )
   {
      *carc_result = de44[CARC_RESULT_LOC];
   }
   return;
}

/*****************************************************************************

  Function:    FIND_CVV2_RESULT_CODE

  Description: This function will dig the Card Authentication Result code
               out of field 44 response data.  This value is in byte 9.

  Author:      D. Irby

  Inputs:      len  - Length of DE44
               de44 - Field 44 data

  Outputs:     cvv2_result - Value defining result of CVV2 Verification

  Return values: None

******************************************************************************/
void find_cvv2_result_code( BYTE len, pBYTE de44, pBYTE cvv2_result )
{
   if ( len > CVV2_RESULT_LOC )
   {
      *cvv2_result = de44[CVV2_RESULT_LOC];
   }
   return;
}

/******************************************************************************
 *
 *  NAME:         ParseField63
 *
 *  DESCRIPTION:  This function parses field 63, which is a bitmapped field.
 *                The values parsed are stored in the TLF01 structure:
 *
 *                   63.1 - Network ID              => product_codes11.code
 *                   63.3 - Reversal Reason Code    => product_codes12.code
 *                   63.4 - STIP/Switch Reason Code => product_codes13.code
 *
 *  INPUTS:       f63 - Field 63 data from Visa (first byte is length)
 *
 *  OUTPUTS:      p_auth_tx - Auth Tx structure containing parsed data
 *
 *  RTRN VALUE:   True means parsing was successful, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT ParseField63( pCHAR f63, pAUTH_TX p_auth_tx )
{
   #define MAX_F63_SIZE   20
   #define F63_BITMAP_LEN  3

   INT     retval = true;
   INT     f63_bit;
   INT     Len;
   INT     TotalLen;
   BYTE    BitMap[F63_BITMAP_LEN];
   BYTE    TempStr[MAX_F63_SIZE+1];
   BYTE    buffer[6];


   /* Get length of field. */
   TotalLen = f63[0];

   /* Copy bitmap into local variable. */
   if ( (TotalLen < 0) || (TotalLen > MAX_F63_SIZE) )
   {
      retval = false;
   }
   else
   {
      memcpy( BitMap, f63+1, sizeof(BitMap) );

      memset( TempStr, 0x00, sizeof(TempStr) );
      memcpy( TempStr, f63+F63_BITMAP_LEN+1, TotalLen-F63_BITMAP_LEN );

      /* Parse the bitmap for 1st byte only, extracting the fields we want. */
      Len = 0;
      for( f63_bit=1; f63_bit<=8; f63_bit++ )
      {
         if ( BitMap[0] & BITTAB[f63_bit].bit_mask )
         {
            switch( f63_bit )
            {
               case 1:  
                      /* Network ID */
                      memset( buffer, 0x00, sizeof(buffer));
                      bcd_to_asc( &TempStr[Len], 2, buffer, 4 );
                      strcpy( p_auth_tx->TLF01_details.product_codes[10].code,
                              buffer );
                      Len+= 2;
                      break;
               case 3:
                      /* Reversal Message Reason Code */
                      memset( buffer, 0x00, sizeof(buffer));
                      bcd_to_asc( &TempStr[Len], 2, buffer, 4 );
                      strcpy(p_auth_tx->TLF01_details.product_codes[11].code,
                             buffer );
                      Len += 2;
                      break;
               case 4:
                      /* STIP/Switch Reason Code */
                      memset( buffer, 0x00, sizeof(buffer));
                      bcd_to_asc( &TempStr[Len], 2, buffer, 4 );
                      strcpy(p_auth_tx->TLF01_details.product_codes[12].code,
                             buffer );
                      Len += 2;
                      break;
               default:
                      Len++;
                      break;
            }
         }  /* End IF bit is set */
      }  /* End LOOP on BIT */
   }
   return( retval );
}

/******************************************************************************
 *
 *  NAME:         LogStipReasonCode
 *
 *  DESCRIPTION:  This function translates a STIP/Switch message code into
 *                a text message, which is logged to the system log.
 *
 *  INPUTS:       p_auth_tx - Structure with STIP/Switch message code (4 digits)
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void LogStipReasonCode( pAUTH_TX p_auth_tx )
{
   INT  icode;
   BYTE scode[5]    = {0};
   CHAR buffer[512] = {0};

   strcpy( scode, p_auth_tx->TLF01_details.product_codes[12].code );
   if ( isnum(scode) )
   {
      icode = atoi( scode );
      sprintf( buffer, "STIP Reason Code %s: ", scode );

      switch( icode )
      {
         case 9001:
                  strcat( buffer, "The issuer was signed off." );
                  break;
         case 9002:
                  strcat( buffer, "The issuer was signed off by the switch." );
                  break;
         case 9011:
                  strcat( buffer, "The line to issuer is down." );
                  break;
         case 9020:
                  strcat( buffer, "Response to issuer timed out." );
                  break;
         case 9022:
                  strcat( buffer, "PACM diverted." );
                  break;
         case 9029:
                  strcat( buffer, "IARS Authorization was approved by referral center (member-certified)" );
                  break;
         case 9030:
                  strcat( buffer, "This transaction is auto-CDB; there is a pickup response from issuer." );
                  break;
         case 9031:
                  strcat( buffer, "Original processed in stand-in." );
                  break;
         case 9041:
                  strcat( buffer, "There was a pin verification error." );
                  break;
         case 9045:
                  strcat( buffer, "Switch was unable to translate the pin." );
                  break;
         case 9047:
                  strcat( buffer, "Real-time decision (RTD) decline." );
                  break;
         case 9048:
                  strcat( buffer, "There is an invalid CVV with the All Respond Option." );
                  break;
         case 9050:
                  strcat( buffer, "Switch generated this 0620 Alert message." );
                  break;
         case 9061:
                  strcat( buffer, "Internal system error or other switch-detected error condition" );
                  break;
         case 9102:
                  strcat( buffer, "Switch generated this 0420; an approval cannot be delivered to acquirer" );
                  break;
         case 9103:
                  strcat( buffer, "An approval cannot be delivered to acquirer; issuer timed out." );
                  break;

              	/*New STIP reason codes
              		3900 Incremental authorization
              		3901 Resubmission
              		3902 Delayed charges
              		3903 Reauthorization
              		3904 No show
              		3905 Account Top-up
              		*/
                       case 3900:
                                         strcat( buffer, "-Incremental authorization..." );
                                         break;
                       case 3901:
                                         strcat( buffer, "-Resubmission..." );
                                         break;
                       case 3902:
                                         strcat( buffer, "Delayed charges." );
                                         break;
                       case 3903:
                                         strcat( buffer, "Reauthorization." );
                                         break;
                       case 3904:
                                         strcat( buffer, "NO Show." );
                                         break;
                       case 3905:
                                         strcat( buffer, "Account Top-up..." );
                                          break;
         default  :
                  strcat( buffer, "Unrecognized STIP Reason Code." );
                  break;
      }
   }
   else
   {
      sprintf( buffer,
              "Invalid STIP Advice Code in field 63.4 from host: %s, "
              "must be numeric",
               scode );
   }
   memset(Log_Bufr,0x00,sizeof(Log_Bufr));
   sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
   		   p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
   strcat(buffer,Log_Bufr);
   ncvis2_log_message( 3, 1, buffer , "LogStipReasonCode" , 0 ,p_auth_tx->TLF01_details.card_num);
   return;
}

/******************************************************************************
 *
 *  NAME:         Field116_Handler
 *
 *  DESCRIPTION:  Field 116 contains various data from other hosts, such as
 *                MasterCard and Amex.  This field comes in responses from
 *                Visa when we process MC or Amex transactions thru the VAP.
 *
 *                This function will parse the field, reformat the varying
 *                data types into ASCII, then store into TLF01.
 *
 *                The format of field 116 is thus:
 *                   There are 1 or more data sets that define the network
 *                   such as MasterCard or American Express. Within each
 *                   data set there can be 1 or more tag sets. Each tag set
 *                   will contain an identifier, length, and data. Below are
 *                   the data types of the fields, shown for MC and Amex.
 *
 *                MasterCard Format
 *                   1 byte  (binary) = Overall length of field 116
 *                   1 byte  (BCD   ) = Data Set Id (0x67 = MasterCard)
 *                   2 bytes (binary) = Length of Data Set (..0x00,0x25)
 *                   1 byte  (BCD   ) = Tag ID (0x01 = DE22 from MC)
 *                   1 byte  (binary) = Tag Len (0x03 = 3)
 *                   3 bytes (UN    ) = Example: '902' --> 0x09,0x00,0x02
 *                   1 byte  (BCD   ) = Tag ID (0x02 = DE26 from MC)
 *                   1 byte  (binary) = Tag Len (0x02 = 2)
 *                   3 bytes (UN    ) = Example: '31' --> 0x03,0x01
 *                   1 byte  (BCD   ) = Tag ID (0x03 = DE61 from MC)
 *                   1 byte  (binary) = Tag Len (var ..0x1A = ..up to 26)
 *                  26 bytes (AN    ) = Example: '1B2' --> 0x31,0x42,0x32
 *                   
 *                American Express Format
 *                   1 byte  (binary) = Overall length of field 116
 *                   1 byte  (BCD   ) = Data Set Id (0x66 = Amex)
 *                   2 bytes (binary) = Length of Data Set (..0x00,0x15)
 *                   1 byte  (BCD   ) = Tag ID (0x01 = DE22 from Amex)
 *                   1 byte  (binary) = Tag Len (0x0C = 12)
 *                  12 bytes (AN    ) = Example: '12A12B12C12D' --> 0x31,..0x44
 *
 *                Note: UN = Unpacked Numeric (lower 4 bits only)
 *
 *
 *  INPUTS:       f116 - Field 116 data from Visa (first byte is length)
 *
 *  OUTPUTS:      p_auth_tx - Auth Tx structure containing parsed data
 *
 *  RTRN VALUE:   True means parsing was successful, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT Field116_Handler( pCHAR f116, pAUTH_TX p_auth_tx )
{
   INT  retval = true;
   INT  f116_len_total;
   INT  f116_len_temp;
   INT  tlf01_buffer_len;
   INT  dataset_len;
   INT  i;
   INT  taglen;
   INT  taglen_temp;
   INT  tempNum;
   INT  moreTags       = true;
   INT  moreDataSets   = true;
   INT  errorFlag      = false;
   BYTE errbuf   [512] = {0};
   BYTE tlf01_buf[256] = {0}; /* This field contains the data for TLF01. */
   BYTE tempstr  [256] = {0};
   BYTE dataset    [5] = {0};
   BYTE dataset_Id [3] = {0};
   BYTE tagId      [2] = {0};
   BYTE tagData  [256] = {0};

   f116_len_total = f116[0];
   if ( f116_len_total > 0 )
   {
      f116_len_temp    = 1; /* Current data pointer within location within f116. */
      tlf01_buffer_len = 0; /* Current data pointer within location within buffer*/

      /* Loop through the data sets. */
      while( moreDataSets && (f116_len_temp < f116_len_total) )
      {
         /* Get the Data Set. */
         bcd_to_asc( &f116[f116_len_temp++], 1, dataset_Id, 2 );
         if ( 0 == strcmp(dataset_Id,"67") )
         {
            /* MasterCard */
            /* ---------- */
            dataset_len = f116[++f116_len_temp]; /* Pt to 2nd len byte. */
            f116_len_temp++;

            if ( dataset_len > 256 )
            {
               sprintf( errbuf,
                       "Field 116 err: Unexpected Data Set length (%d) for MC",
                        dataset_len );
               moreDataSets = false;
            }
            else
            {
               /* Copy Data Set ID and Data Set length into output buffer. */
               memset( dataset, 0x00, sizeof(dataset) );
               sprintf( dataset, "%02d", dataset_len );
               strcat( tlf01_buf, dataset_Id );
               strcat( tlf01_buf, dataset    );

               tlf01_buffer_len += strlen(dataset) + 2;

               /* Loop through the Tags for this Data Set. */
               taglen = 0;
               while( moreTags && (taglen < dataset_len) )
               {
                  /* Get Tag ID: Expecting 0x01, 0x02, 0x03 only. */
                  memset( tagData, 0x00, sizeof(tagData) );
                  if ( 0x01 == f116[f116_len_temp] )
                  {
                     /* TAG 01 - DE22 */
                     strcat( tlf01_buf, "01" );                /* Tag ID  */
                     taglen_temp = f116[++f116_len_temp];
                     sprintf( tempstr, "%02d", taglen_temp );
                     strcat( tlf01_buf, tempstr );             /* Tag Len */
                     f116_len_temp++;
                     tlf01_buffer_len += 4;

                     /* Get the tag data; convert from UN to ASCII. */
                     for( i=0; i<3; i++ )
                     {
                        memset( tagData, 0x00, sizeof(tagData) );
                        memset( tempstr, 0x00, sizeof(tempstr) );
                        tempstr[0] = f116[f116_len_temp++];
                        tempstr[0] &= 0x0f;  /* Get only the 4 LSB's (UN). */
                        tempNum = tempstr[0];
                        sprintf( tagData, "%1d", tempNum );
                        strcat( tlf01_buf, tagData );
                     }
                     taglen += 5;
                     tlf01_buffer_len += 3;
                  }
                  else if ( 0x02 == f116[f116_len_temp] )
                  {
                     /* TAG 02 - DE26 */
                     strcat( tlf01_buf, "02" );                /* Tag ID  */
                     taglen_temp = f116[++f116_len_temp];
                     sprintf( tempstr, "%02d", taglen_temp );
                     strcat( tlf01_buf, tempstr );             /* Tag Len */
                     f116_len_temp++;
                     tlf01_buffer_len += 4;

                     /* Get the tag data; convert from UN to ASCII. */
                     for( i=0; i<2; i++ )
                     {
                        memset( tagData, 0x00, sizeof(tagData) );
                        memset( tempstr, 0x00, sizeof(tempstr) );
                        tempstr[0] = f116[f116_len_temp++];
                        tempstr[0] &= 0x0f;  /* Get only the 4 LSB's (UN). */
                        tempNum = tempstr[0];
                        sprintf( tagData, "%1d", tempNum );
                        strcat( tlf01_buf, tagData );
                     }
                     taglen += 4;
                     tlf01_buffer_len += 2;
                  }
                  else if ( 0x03 == f116[f116_len_temp] )
                  {
                     /* TAG 03 - DE61 */
                     strcat( tlf01_buf, "03" );                /* Tag ID  */
                     taglen_temp = f116[++f116_len_temp];
                     sprintf( tempstr, "%02d", taglen_temp );
                     strcat( tlf01_buf, tempstr );             /* Tag Len */
                     f116_len_temp++;
                     tlf01_buffer_len += 4;

                     /* Get the tag data. It is already in ASCII. */
                     memset( tagData, 0x00, sizeof(tagData) );
                     memcpy( tagData, &f116[f116_len_temp], taglen_temp );
                     strcat( tlf01_buf, tagData );
                     f116_len_temp    += taglen_temp;
                     tlf01_buffer_len += taglen_temp;
                     taglen           += taglen_temp + 2;
                  }
                  else
                  {
                     taglen_temp    = f116[++f116_len_temp];
                     f116_len_temp += taglen_temp + 1;
                     taglen        += taglen_temp + 2;
                  }
               }
            }
         }
         else if ( 0 == strcmp(dataset_Id,"66") )
         {
            /* American Express */
            /* ---------------- */
            dataset_len = f116[++f116_len_temp]; /* Pt to 2nd len byte. */
            f116_len_temp++;

            if ( dataset_len > 256 )
            {
               sprintf( errbuf,
                       "Field 116 err: Unexpected Data Set length (%d) for Amex",
                        dataset_len );
               moreDataSets = false;
            }
            else
            {
               /* Copy Data Set ID and Data Set length into output buffer. */
               memset( dataset, 0x00, sizeof(dataset) );
               sprintf( dataset, "%02d", dataset_len );
               strcat( tlf01_buf, dataset_Id );
               strcat( tlf01_buf, dataset    );

               tlf01_buffer_len += strlen(dataset) + 2;

               /* Loop through the Tags for this Data Set. */
               taglen = 0;
               while( moreTags && (taglen < dataset_len) )
               {
                  /* Get Tag ID: Expecting 0x01 only. */
                  memset( tagData, 0x00, sizeof(tagData) );
                  if ( 0x01 == f116[f116_len_temp] )
                  {
                     /* TAG 01 - DE22 */
                     strcat( tlf01_buf, "01" );                /* Tag ID  */
                     taglen_temp = f116[++f116_len_temp];
                     sprintf( tempstr, "%02d", taglen_temp );
                     strcat( tlf01_buf, tempstr );             /* Tag Len */
                     f116_len_temp++;
                     tlf01_buffer_len += 4;

                     /* Get the tag data. It is already in ASCII. */
                     memset( tagData, 0x00, sizeof(tagData) );
                     memcpy( tagData, &f116[f116_len_temp], taglen_temp );
                     strcat( tlf01_buf, tagData );
                     f116_len_temp    += taglen_temp;
                     tlf01_buffer_len += taglen_temp;
                     taglen           += taglen_temp + 2;
                  }
                  else
                  {
                     taglen_temp    = f116[++f116_len_temp];
                     f116_len_temp += taglen_temp + 1;
                     taglen        += taglen_temp + 2;
                  }
               }
            }
         }
         else
         {
            /* This data set is not supported. Get its length and skip it. */
            dataset_len = f116[f116_len_temp];
            f116_len_temp += dataset_len + 1;

            if ( f116_len_temp < 0 )
               moreDataSets = false;
         }
      }

      if ( errbuf[0] != 0x00 )
      {
         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
         		p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
         strcat(errbuf,Log_Bufr);
         ncvis2_log_message( 3, 2, errbuf , "Field116_Handler" , 0 ,p_auth_tx->TLF01_details.card_num);
      }
      else
      {
         /* Log the parsed data into TLF01. */
         strcpy( p_auth_tx->TLF01_details.dest_key, tlf01_buf );
      }
   }
   return( retval );
}

/******************************************************************************
 *
 *  NAME:         generate_VISA_EMV_data
 *
 *  DESCRIPTION:  This function steps through the EMV subfield tags.  If one
 *                that is supported by VISA is found, it is copied into the
 *                outgoing Host Message buffer.  The incoming data stream is
 *                not changed.
 *
 *  INPUTS:       emv_len_in  - Length of emv_data_in
 *                emv_data_in - EMV data that comes into ATP
 *
 *  OUTPUTS:      HOST_MESSAGE.ICCdata - EMV data that goes to VISA
 *                HOST_MESSAGE.ICCdata_len - Length of EMV data to VISA
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Phani
 *
 ******************************************************************************/
void generate_VISA_EMV_data( INT emv_len_in, pBYTE emv_data_in )
{
    INT   tag_len;
   BYTE  emv_out_len[3] = {0},temp_len[2]={0};

 
   if(emv_len_in > 255)
   	emv_len_in = 255;
   HOST_MESSAGE.ICCdata[0]=0x01;
   HOST_MESSAGE.ICCdata[1]=0x00;
 //  HOST_MESSAGE.ICCdata[2]=0x01;
 	sprintf( emv_out_len, "%01x", emv_len_in );
   genutil_asc_to_bin((BYTE *)emv_out_len,(BYTE *)temp_len,&tag_len);
   memcpy(&HOST_MESSAGE.ICCdata[2],temp_len,1);
   // One Byte Data Set ID followed by length data set IDs
   memcpy(&HOST_MESSAGE.ICCdata[3],emv_data_in+2,emv_len_in);
   sprintf( emv_out_len, "%01x", emv_len_in+3 );
   genutil_asc_to_bin((BYTE *)emv_out_len,(BYTE *)temp_len,&tag_len);
   memcpy(HOST_MESSAGE.ICCdata_len,temp_len,1);
   return;
}
// Phani
void genutil_VISA_EMV_To_Auth_Tx( pCHAR emv_raw, INT total_len, pAUTH_TX p_auth_tx )
{
   INT    fieldLength;
   INT    dataIndex;
   INT    bTwoBytesTag;
   BYTE   byteOneOfTag;
   BYTE   byteTwoOfTag;
   BYTE   b_string[4]  = {0};
   CHAR   a_string[4]  = {0};
   CHAR   tempstr[500] = {0};


   dataIndex   = 0;
   if(emv_raw[dataIndex] == 0x01)
   {
		dataIndex=dataIndex+3;
   }
   else if(emv_raw[dataIndex] == 0x00)
   {
	   dataIndex=dataIndex+3;
   }

   while (dataIndex <= total_len)
   {
      bTwoBytesTag = 0;
      byteOneOfTag = emv_raw[dataIndex];
      dataIndex++;
      if ((byteOneOfTag & 0x1F) == 0x1F)
      {
         bTwoBytesTag = 1;
         byteTwoOfTag = emv_raw[dataIndex];
         dataIndex++;
      }
      fieldLength = emv_raw[dataIndex];
      dataIndex++;

      if (bTwoBytesTag)
      {
         if (byteOneOfTag == 0x5F)
         {
            if (byteTwoOfTag == 0x2A) 
            {
               genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
               memcpy(p_auth_tx->EMV_details.currency_code, tempstr, sizeof(p_auth_tx->EMV_details.currency_code)-1);
               dataIndex += fieldLength;
            }
            else if (byteTwoOfTag == 0x34) 
            {
               genutil_bcd_to_asc (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));	
               memcpy(p_auth_tx->EMV_details.pan_sequence_number, tempstr, sizeof(p_auth_tx->EMV_details.pan_sequence_number)-1);
               dataIndex += fieldLength;
            }
         }
         else if (byteOneOfTag == 0x9F)
         {
            switch (byteTwoOfTag)
            {
               case 0x02: 
                        genutil_bcd_to_asc (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.tran_amount, tempstr, sizeof(p_auth_tx->EMV_details.tran_amount)-1);
                        break;
               case 0x03:
                        genutil_bcd_to_asc (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.other_amount, tempstr, sizeof(p_auth_tx->EMV_details.other_amount)-1);
                        break;
               case 0x09: 
                        genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.version, tempstr, sizeof(p_auth_tx->EMV_details.version)-1);
                        break;
               case 0x10: 
                        genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.iad, tempstr, sizeof(p_auth_tx->EMV_details.iad)-1);
                        break;
               case 0x1A: 
                        genutil_bcd_to_asc (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.country_code, tempstr+1, sizeof(p_auth_tx->EMV_details.country_code)-1);
                        break;
               case 0x15:
                        genutil_bcd_to_asc (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.mcc, tempstr, sizeof(p_auth_tx->EMV_details.mcc)-1);
                        break;
               case 0x1E:
                        memcpy(p_auth_tx->EMV_details.serial_number, &emv_raw[dataIndex], fieldLength);
                        break;
               case 0x26: 
                        genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.app_crypto, tempstr, sizeof(p_auth_tx->EMV_details.app_crypto)-1);
                        break;
               case 0x27: 
                        genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.crypto_info, tempstr, sizeof(p_auth_tx->EMV_details.crypto_info)-1);
                        break;
               case 0x33:
                        genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.terminal_capabilities, tempstr, sizeof(p_auth_tx->EMV_details.terminal_capabilities)-1);
                        break;
               case 0x34:
                        genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.cvm_results, tempstr, sizeof(p_auth_tx->EMV_details.cvm_results)-1);
                        break;
               case 0x35:
                        genutil_bcd_to_asc (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.terminal_type, tempstr, sizeof(p_auth_tx->EMV_details.terminal_type)-1);
                        break;
               case 0x36:
                        genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.atc, tempstr, sizeof(p_auth_tx->EMV_details.atc)-1);
                        break;
               case 0x37:
                        genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.unpredictable_number, tempstr, sizeof(p_auth_tx->EMV_details.unpredictable_number)-1);
                        break;
               case 0x41:
                        genutil_bcd_to_asc (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.tran_sequence_ctr, tempstr, sizeof(p_auth_tx->EMV_details.tran_sequence_ctr)-1);
                        break;
               case 0x53:
                        memcpy(p_auth_tx->EMV_details.tcc, &emv_raw[dataIndex], fieldLength);
                        break;
               default:
                        break;
            }
            dataIndex += fieldLength;
         }
      }
      else
      {
         switch (byteOneOfTag)
         {
            case 0x71:
                     /* Get overall length of this field - including tag. */
                     memset( a_string, 0x00, sizeof(a_string) );
                     memset( b_string, 0x00, sizeof(b_string) );
                     b_string[0] = fieldLength+2;
                     genutil_bin_to_ascii( b_string, 1, a_string, sizeof(a_string) );
                     memcpy( p_auth_tx->EMV_details.issuer_script_1, a_string, 2 );

                     /* Get Tag ID */
                     memcpy(p_auth_tx->EMV_details.issuer_script_1+2, "71", 2 ); /* Tag ID */

                     /* Get length of data for this tag. */
                     memset( a_string, 0x00, sizeof(a_string) );
                     memset( b_string, 0x00, sizeof(b_string) );
                     b_string[0] = fieldLength;
                     genutil_bin_to_ascii( b_string, 1, a_string, sizeof(a_string) );
                     memcpy( p_auth_tx->EMV_details.issuer_script_1+4, a_string, 2 );

                     /* Finally put in the data. */
                     genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                     memcpy(p_auth_tx->EMV_details.issuer_script_1+6, tempstr, sizeof(p_auth_tx->EMV_details.issuer_script_1)-7);

                     break;
            case 0x82:
                     genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                     memcpy(p_auth_tx->EMV_details.interchange_profile, tempstr, sizeof(p_auth_tx->EMV_details.interchange_profile)-1);
                     break;
            case 0x84:
                     genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                     memcpy(p_auth_tx->EMV_details.app_id, tempstr, sizeof(p_auth_tx->EMV_details.app_id)-1);
                     break;
            case 0x91:
                     genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                     memcpy(p_auth_tx->EMV_details.issuer_auth_data+1, tempstr, sizeof(p_auth_tx->EMV_details.issuer_auth_data)-2);
                     p_auth_tx->EMV_details.issuer_auth_data[0] = fieldLength;
                     break;
            case 0x95:
                     genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                     memcpy(p_auth_tx->EMV_details.tvr, tempstr, sizeof(p_auth_tx->EMV_details.tvr)-1);
                     break;
            case 0x9A:
                     genutil_bcd_to_asc (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                     memcpy(p_auth_tx->EMV_details.tran_date, tempstr, sizeof(p_auth_tx->EMV_details.tran_date)-1);
                     break;
            case 0x9C:
                     genutil_bcd_to_asc (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                     memcpy(p_auth_tx->EMV_details.tran_type, tempstr, sizeof(p_auth_tx->EMV_details.tran_type)-1);
                     break;
            default:
                     break;
         }
         dataIndex += fieldLength;
      }
   }
   return;
}


INT If_Tx_is_visa_RPS_Asiapay_Transaction( pAUTH_TX p_auth_tx  )
{
	if( 0 == strncmp(p_auth_tx->TLF01_details.message_type,"0100",4))
	{
		if(0 == strncmp(p_auth_tx->TLF01_details.processing_code,"00",2))
		{
			if(0 == strncmp(p_auth_tx->TLF01_details.pos_condition_code,"08",2))
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else if (0 == strncmp( p_auth_tx->TLF01_details.pos_condition_code,"08",2))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}

}
/*******************************************************
 * This function will get the values from tf.ini file
 * from the section DATASERVER with keyname DB_ERROR_STATICS_FLAG &
 *	DB_ERROR_STATICS_VALUE which will help to decide the db statics
 *	logging mechanism.
 * *******************************************************/

void ncvis2_get_error_warning_file_name_path(void )
{
   DWORD rc;
   CHAR  filename   [80] = {0};
   CHAR  tmpstr     [80] = {0};
   CHAR  ncvis2_error_warning_file_size[5] = {0};

   /* Get path to the tf.ini file. */
   memset( tmpstr, 0x00, sizeof(tmpstr) );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, "tf.ini" );

   /*Read path for creating file in order to log db statics and db oracle error messages */
   rc = GetPrivateProfileString(
								  "DATASERVERS",             /* points to section name         */
								  "DB_STATICS_PATH",       	/* points to key name             */
								   "",  					/* Default string                 */
								   ncvis2_error_warning_file_path,              	/* points to destination buffer   */
								   sizeof(ncvis2_error_warning_file_path)-1,   	 /* size of destination buffer     */
								   filename                  /* points to ini filename         */
						 	   );

   rc = GetPrivateProfileString(
								  "ERROR_WARNING",             /* points to section name         */
								  AppName,       	/* points to key name             */
								  "",  					/* Default string                 */
								  ncvis2_module_error_warning_file_name,              	/* points to destination buffer   */
								  sizeof(ncvis2_module_error_warning_file_name)-1,   	 /* size of destination buffer     */
								  filename                  /* points to ini filename         */
								);

   rc = GetPrivateProfileString(
							  "ERROR_WARNING",             /* points to section name         */
							  "ERROR_WARNING_FILE_SIZE",       	/* points to key name             */
							  "500",  					/* Default string                 */
							  ncvis2_error_warning_file_size,              	/* points to destination buffer   */
							  sizeof(ncvis2_error_warning_file_size)-1,   	 /* size of destination buffer     */
							  filename                  /* points to ini filename         */
							);

	/* File size size conversion */
	Max_File_Size_Defined = atoi(ncvis2_error_warning_file_size);
	if(Max_File_Size_Defined <= 0)
	{
		Max_File_Size_Defined = 500 ;
	}
	Max_File_Size_Defined = Max_File_Size_Defined * ONE_MB_IN_BYTES ; /* 1 MB = 1048576 BYTES*/

   if((strlen(ncvis2_error_warning_file_path) > 0) &&
	  (strlen(ncvis2_module_error_warning_file_name)> 0))
   {
	   ncvis2_create_Error_Filename();
   }
}

INT ncvis2_Log_error_warning_to_File(pCHAR Error_Buf,int sev,pCHAR func ,int detail)
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
    len = strlen(ncvis2_Error_warning_Filename);
    path_len = strlen(ncvis2_error_warning_file_path);
    if( len == 0 ||
    	path_len==0 )
    {
    	LogEvent(Error_Buf,INFO_MSG);
    	return 0;
    }
    if ( 0 != strcmp(&ncvis2_Error_warning_Filename[len-4], current_mmdd) )
    {
		/* Now create the new filename.*/
    	ncvis2_create_Error_Filename();
    }
	if((fp = fopen(ncvis2_Error_warning_Filename,"a+b"))==NULL)
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

void ncvis2_create_Error_Filename()
{
	CHAR  system_date[25] = {0};

   /* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
	ptetime_get_timestamp( system_date );

    strcpy( ncvis2_Error_warning_Filename, ncvis2_error_warning_file_path );
    strcat( ncvis2_Error_warning_Filename, ncvis2_module_error_warning_file_name );
	strcat( ncvis2_Error_warning_Filename, ".EW." );
    strncat(ncvis2_Error_warning_Filename, system_date,   4 );  /* YYYY */
    strncat(ncvis2_Error_warning_Filename, system_date+5, 2 );  /* MM   */
    strncat(ncvis2_Error_warning_Filename, system_date+8, 2 );  /* DD   */
}
/******************************************************************************
 *
 *  NAME:         ncvis2_log_message
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
void ncvis2_log_message( INT dest, INT sev, pCHAR msg, pCHAR func, int details ,pCHAR card_num)
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
   /* Call function to post the message. */
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
   ncvis2_Log_error_warning_to_File(text_message,sev,func,details);
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
	   BOOLEAN    ret_val = true;

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

BOOLEAN send_response_to_parvbh	( pAUTH_TX p_auth_tx,BYTE msg_type,
        						  pCHAR    source_que, pCHAR error_msg)
{
	pPTE_MSG   p_msg   = NULL_PTR;
	BOOLEAN    ret_val = true;

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
BOOLEAN send_msg( pPTE_MSG p_msg, pCHAR que_name, pCHAR error_msg )
{
   BOOLEAN  ret_val = true;
   LONG     ret_code= 0;
   CHAR     Buff[256] = {0};
   CHAR     pte_err_mesg[200] = "";

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

BOOLEAN ncvis2_Is_AsiaPay_Transaction(pAUTH_TX p_auth_tx)
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

BOOLEAN ncvis2_check_for_acq_refund_transaction(pAUTH_TX p_auth_tx)
{
	   if(0 == strncmp(p_auth_tx->TLF01_details.message_type,"0100",4)  &&
		  0 == strncmp(p_auth_tx->TLF01_details.processing_code,"20",2) &&
		  0 == strncmp(p_auth_tx->TLF01_details.handler_queue,"ncvis2",6))
	   {
		   return true;
	   }
	   return false;
}
BOOLEAN ncvis2_check_for_cadencie_acq_transaction(pAUTH_TX p_auth_tx)
{
	if((0 == strncmp(p_auth_tx->TLF01_details.message_type,"0100",4) ||
		0 == strncmp(p_auth_tx->TLF01_details.message_type,"0400",4) ||
		0 == strncmp(p_auth_tx->TLF01_details.message_type,"0120",4) ||
		0 == strncmp(p_auth_tx->TLF01_details.message_type,"0420",4) ||
		0 == strncmp(p_auth_tx->TLF01_details.message_type,"0110",4) ||
		0 == strncmp(p_auth_tx->TLF01_details.message_type,"0410",4) ||
		0 == strncmp(p_auth_tx->TLF01_details.message_type,"0130",4) ||
		0 == strncmp(p_auth_tx->TLF01_details.message_type,"0430",4))&&
		0 == strncmp(p_auth_tx->TLF01_details.handler_queue,"ncvis2",6))
	{
	   return true;
	}
	return false;
}

BOOLEAN ncvis2_transfer_refund_msg_to_VISA( pAUTH_TX	 p_auth_tx )
{
	BOOLEAN   retval              = true;
	pPTE_MSG  p_msg_out           = NULL_PTR;
	CHAR	  visa_host_que_name  [MAX_QUE_NAME_SIZE+1] = "ncvisaA" ;
	CHAR      buffer[sizeof(AUTH_TX)+1]= {0};
	BYTE      originator_info[33] = {0};
	LONG      retCode             = 0L;
	CHAR      strError[512]       = {0};
    memcpy(buffer , p_auth_tx, sizeof(AUTH_TX) );

    p_msg_out = ptemsg_build_msg( MT_AUTH_REQUEST,
                                  ST1_NONE,
                                  ST2_NONE,
                                  visa_host_que_name,
                                  application_que_name,
                                  (pBYTE) buffer,
								  sizeof(AUTH_TX),
                                  0 );

    if ( p_msg_out == NULL_PTR )
    {
       sprintf( strError,
               "Insufficient Memory to build VISA request message. Size: %d",
                buflen );
       ncvis2_log_message( 3, 3, strError , "ncvis2_transfer_refund_msg_to_VISA" , 0 ,p_auth_tx->TLF01_details.card_num);
       retval = false;
    }
    else
    {
       ptemsg_set_orig_comms_info( p_msg_out, originator_info );
       retCode = pteipc_send( p_msg_out, visa_host_que_name );
       free (p_msg_out);

       if ( retCode < 0 )
       {
          pteipc_get_errormsg (retCode, strError);
          sprintf( strError,
                  "Error on pteipc_send to que %s: %s",
				  visa_host_que_name, strError );

          ncvis2_log_message( 3, 3, strError , "ncvis2_transfer_refund_msg_to_VISA" , 0,p_auth_tx->TLF01_details.card_num );
          retval = false;
       }
    }

	return retval;
}

void ncvis2_read_TO_config_details_tf_ini()
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
									  "REVERSL_ATTEMPTS_FOR_TIMEOUT_TRANSACTION_VISA",       	/* points to key name             */
									   "1",  												/* Default string                 */
									   readvalue,              								/* points to destination buffer   */
									   sizeof(readvalue),   	 							/* size of destination buffer     */
									   filename                  							/* points to ini filename         */
							 	   );

	   rvrsl_attmpts_value_fr_timeout_visa=atoi(readvalue);

	   if(rc == 0)
	   {
		   ncvis2_log_message( 2, 1, "REVERSL_ATTEMPTS_FOR_TIMEOUT_TRANSACTION_VISA Value not read from tf.ini","ncvis2_read_tf_ini",0,"");
	   }

	   readvalue[0]='\0';
	   rc = GetPrivateProfileString(  "REVERSL_ATTEMPTS_FOR_LATERESPONSE_TRANSACTION",              /* points to section name         */
									  "REVERSL_ATTEMPTS_FOR_LATERESPONSE_TRANSACTION_VISA",       	/* points to key name             */
									   "1",  														/* Default string                 */
									   readvalue,              										/* points to destination buffer   */
									   sizeof(readvalue),   	 									/* size of destination buffer     */
									   filename                  									/* points to ini filename         */
							 	   );

	   rvrsl_attmpts_value_fr_laterspns_visa=atoi(readvalue);

	   if(rc == 0)
	   {
		   ncvis2_log_message( 2, 1, "REVERSL_ATTEMPTS_FOR_LATERESPONSE_TRANSACTION_VISA Value not read from tf.ini","ncvis2_read_tf_ini",0,"");
	   }


}
INT ncvis2_check_if_trx_is_bdo_pos_acq_trx( pAUTH_TX p_auth_tx  )
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

INT ncvis2_is_sbatch_trx( pAUTH_TX p_auth_tx  )
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

INT ncvis2_is_valid_BDO_pos_acq_trn( pAUTH_TX p_auth_tx  )
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

/*MobilePOS NFC Tap2Phone Trx*/
INT ncvis2_check_if_PEM_is_MobilePOS_NFC_Tap2Phone_trx_with_pin( pAUTH_TX p_auth_tx  )
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

INT ncvis2_check_if_PEM_is_MobilePOS_NFC_Tap2Phone_trx_without_pin( pAUTH_TX p_auth_tx  )
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


INT ncvis2_check_if_trx_qualify_for_MobilePOS_NFC_Tap2Phone_trx ( pAUTH_TX p_auth_tx  )
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
INT ncvis2_check_if_trx_is_visa_MobilePOS_NFC_Tap2Phone_trx_with_pin( pAUTH_TX p_auth_tx  )
{
	if((true == ncvis2_check_if_PEM_is_MobilePOS_NFC_Tap2Phone_trx_with_pin(p_auth_tx)) &&
	   (true == ncvis2_check_if_trx_is_bdo_pos_acq_trx(p_auth_tx))&&
	   (true == ncvis2_check_if_trx_qualify_for_MobilePOS_NFC_Tap2Phone_trx(p_auth_tx)))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

INT ncvis2_check_if_trx_is_visa_MobilePOS_NFC_Tap2Phone_trx_without_pin( pAUTH_TX p_auth_tx  )
{
	if((true == ncvis2_check_if_PEM_is_MobilePOS_NFC_Tap2Phone_trx_without_pin(p_auth_tx)) &&
	   (true == ncvis2_check_if_trx_is_bdo_pos_acq_trx(p_auth_tx))&&
	   (true == ncvis2_check_if_trx_qualify_for_MobilePOS_NFC_Tap2Phone_trx(p_auth_tx)))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}
/***************************************************/


/*Swiftpass BDOPay Credit Trx Function*/
INT ncvis2_check_if_PEM_is_Swiftpass_BDO_Pay_credit_trx_with_pin( pAUTH_TX p_auth_tx  )
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

INT ncvis2_check_if_PEM_is_Swiftpass_BDO_Pay_credit_trx_without_pin( pAUTH_TX p_auth_tx  )
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

INT ncvis2_check_if_trx_qualify_for_Swiftpass_BDO_Pay_credit_trx ( pAUTH_TX p_auth_tx  )
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
INT ncvis2_check_if_trx_is_visa_Swiftpass_BDO_Pay_credit_trx_with_pin( pAUTH_TX p_auth_tx  )
{
	if((true == ncvis2_check_if_PEM_is_Swiftpass_BDO_Pay_credit_trx_with_pin(p_auth_tx)) &&
	   (true == ncvis2_check_if_trx_is_bdo_pos_acq_trx(p_auth_tx))&&
	   (true == ncvis2_check_if_trx_qualify_for_Swiftpass_BDO_Pay_credit_trx(p_auth_tx)))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

INT ncvis2_check_if_trx_is_visa_Swiftpass_BDO_Pay_credit_trx_without_pin( pAUTH_TX p_auth_tx  )
{
	if((true == ncvis2_check_if_PEM_is_Swiftpass_BDO_Pay_credit_trx_without_pin(p_auth_tx)) &&
	   (true == ncvis2_check_if_trx_is_bdo_pos_acq_trx(p_auth_tx))&&
	   (true == ncvis2_check_if_trx_qualify_for_Swiftpass_BDO_Pay_credit_trx(p_auth_tx)))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}
/***********************************************************/

/*E-commerce Entry Mode from Swiftpass-BDO Pay Transactions*/
INT ncvis2_check_if_trx_ecommerce_entry_mode_from_swiftpass_BDOPay_trx ( pAUTH_TX p_auth_tx  )
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

INT ncvis2_check_if_trx_ecommerce_POS_entry_mode_from_swiftpass_BDOPay_trx( pAUTH_TX p_auth_tx  )
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

INT ncvis2_check_if_trx_is_visa_ecommerce_swiftpass_BDOPay_trx( pAUTH_TX p_auth_tx  )
{
	if((true == ncvis2_check_if_trx_ecommerce_entry_mode_from_swiftpass_BDOPay_trx(p_auth_tx)) &&
	   (true == ncvis2_check_if_trx_is_bdo_pos_acq_trx(p_auth_tx))&&
	   (true == ncvis2_check_if_trx_ecommerce_POS_entry_mode_from_swiftpass_BDOPay_trx(p_auth_tx)))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

INT ncvis2_check_if_sbatch_qualify_for_MobilePOS_NFC_Tap2Phone_trx_with_pin( pAUTH_TX p_auth_tx  )
{
	 if((true == ncvis2_is_sbatch_trx(p_auth_tx) ) &&
		(true == ncvis2_is_valid_BDO_pos_acq_trn(p_auth_tx)) &&
		(true == ncvis2_check_if_PEM_is_MobilePOS_NFC_Tap2Phone_trx_with_pin(p_auth_tx)))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

INT ncvis2_check_if_sbatch_qualify_for_MobilePOS_NFC_Tap2Phone_trx_without_pin( pAUTH_TX p_auth_tx  )
{
	 if((true == ncvis2_is_sbatch_trx(p_auth_tx) ) &&
		(true == ncvis2_is_valid_BDO_pos_acq_trn(p_auth_tx)) &&
		(true == ncvis2_check_if_PEM_is_MobilePOS_NFC_Tap2Phone_trx_without_pin(p_auth_tx)))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

INT ncvis2_check_if_sbatch_qualify_for_Swiftpass_BDO_Pay_credit_trx_with_pin( pAUTH_TX p_auth_tx  )
{
	 if((true == ncvis2_is_sbatch_trx(p_auth_tx) ) &&
		(true == ncvis2_is_valid_BDO_pos_acq_trn(p_auth_tx)) &&
		(true == ncvis2_check_if_PEM_is_Swiftpass_BDO_Pay_credit_trx_with_pin(p_auth_tx)))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

INT ncvis2_check_if_sbatch_qualify_for_Swiftpass_BDO_Pay_credit_trx_without_pin( pAUTH_TX p_auth_tx  )
{
	 if((true == ncvis2_is_sbatch_trx(p_auth_tx) ) &&
		(true == ncvis2_is_valid_BDO_pos_acq_trn(p_auth_tx)) &&
		(true == ncvis2_check_if_PEM_is_Swiftpass_BDO_Pay_credit_trx_without_pin(p_auth_tx)))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

INT ncvis2_check_if_sbatch_qualify_for_ecommerce_swiftpass_BDOPay_trx( pAUTH_TX p_auth_tx  )
{
	 if((true == ncvis2_is_sbatch_trx(p_auth_tx) ) &&
		(true == ncvis2_is_valid_BDO_pos_acq_trn(p_auth_tx)) &&
		(true == ncvis2_check_if_trx_ecommerce_entry_mode_from_swiftpass_BDOPay_trx(p_auth_tx)))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

/********************************************************/
