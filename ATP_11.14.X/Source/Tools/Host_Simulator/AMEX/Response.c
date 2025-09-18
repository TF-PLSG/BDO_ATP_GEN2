/*******************************************************************************

  Module:      response.c

  Title:    Host Response Definition File for MasterCard

  Description: This module defines the various Host Responses. This isolates 
                the changes required to emulate a different kind of Host
            processing system.

  Note:
  To modify the Host Response, change the set of structures and 
                functions found here in and recompile.
         
  Limitations:  By design, this module allows you to change the following:
              o The number of request transactions that are recognized.
              o The BIT MAP for each of those transactions.
              o The STATIC DATA contents of the private data fields.
                To make other changes, you will potentially have to alter
            additional modules.

  Revision History:

  $Log:   N:\PVCS\PTE\Hostsim_Amex\files\RESPONSE.C  $
   
      Rev 1.4   Dec 02 2003 12:37:58   dirby
   Modified to set response code to approved only if it is not already
   set prior to sending the response.
   SCR 1200
   
      Rev 1.3   Dec 02 2003 11:36:52   dirby
   Corrected bug in message type of Online Update responses.
   SCR 1200
   
      Rev 1.2   Dec 01 2003 10:54:10   dirby
   Updated to handle 1304 messages (Online updates).
   
      Rev 1.1   Oct 27 2003 13:04:16   dirby
   Updated for Amex Acquiring
   
      Rev 1.0   Aug 15 2000 15:30:46   dirby
   Initial version for AMEX
   

******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <sys/timeb.h>

#include "basictyp.h"
#include "pte.h"
#include "ptecom.h"
#include "response.h"

extern int volatile connection_up;
extern INT init_trace();
extern void write_to_comm_trace_file( pBYTE bufptr, INT len, BYTE msg_type );
char RespCodeinTxt[4] = {0};
char AuthCodeinTxt[7] = {0};
char RRNinTxt[12] = {0};


int    g_using_tpdu_prefix = false;      // Whether the TPDU is present or not

/********************   FUNCTION PROTOTYPES   ********************************/
char *advance_bit_field_ptr(char *p_current, int bit_field);
long bcd_to_long( pBYTE bcd_data, int bcd_len );
WORD generate_response ();
void host_notify( LPBYTE p_req, WORD len, pIPC_INFO p_ipc_info );
long ebcdic_to_long( char *fieldid, int length );
BYTE ascii_ebcdic_conv( unsigned char conv_char, char *conv_type );

 
static BOOLEAN pan_used = false;
static LONG    rrn = 10001;                // retrieval reference number - 12 digits ASCII
static WORD    response_buffer_length;     // Length of the response
static BYTE    host_resp_buffer [5500];    // Response Buffer
static BYTE    save_tpdu[5];               // TPDU bytes, if present
static BOOLEAN priority_set = false;

/**********************************
      FUNCTION PROTOTYPES
**********************************/
/* functions local */
BYTE ascii_ebcdic_conv_trk1( unsigned char conv_char, char *conv_type,unsigned char fldSep);
BYTE getFldSep( unsigned char* resp_buff, int index );
INT CondProc(enum bitfields, BYTE);
INT CondExpd(enum bitfields, BYTE);
INT CondPan(enum bitfields, BYTE);
INT CondTrack1(enum bitfields, BYTE);
INT Move_Always(enum bitfields, BYTE);
INT Move_IfThere(enum bitfields, BYTE);
INT vMove_IfThere(enum bitfields, BYTE);
INT Move_Never(enum bitfields, BYTE);
void EBCDICMove(enum bitfields);
void vEBCDICMove(enum bitfields);
void RspEBCDICvMove(enum bitfields);
void RspEBCDICMove(enum bitfields);
void RspIgnore(enum bitfields);
void vRspIgnore(enum bitfields);
void Rj_with_lead_zeros( pCHAR str, INT FieldLen );


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

INT parse_host_msg(LPBYTE p_msg);



/**********************************
     VARIABLES
**********************************/
/* local variables */
INT    reqidx;                  /* next open position in request buffer */
INT    buflen;                  /* total size of buffer */
INT    bitmapidx;               /* offset of bitmap... */
CHAR   applnk_que_name [] = "applnkA" ;
CHAR   security_que_name [] = "securityA" ;
static int temp_55_counter;


/*******************************************************/
/* REQUEST BUILD BUFFER                                */
/*******************************************************/
#define  COMM_BUF_SZE   1000
BYTE ReqRsp_Buffer [COMM_BUF_SZE];
BYTE msgtype[5];
BYTE tx_key;

char  A2E[4] = "A2E";    /*  ASCII to EBCDIC conversion */
char  E2A[4] = "E2A";    /*  EBCDIC to ASCII conversion */




/******************************************************/
/* DATA STRUCTURE FOR STORING DATA IN FINAL FORMAT... */
/******************************************************/
struct iso_msg_rec HOST_MESSAGE;



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
   bf_done           = 0,      /* end of list                          */
   bf_1xbitfield     = 1,      /* extended bit field                   */
   bf_2pan           = 2,      /* pan                                  */
   bf_3proc          = 3,      /* processing code                      */
   bf_4amount        = 4,      /* amount                               */
   bf_5recon_amt	 = 5,
   bf_7trntime       = 7,      /* transmission date-time               */
   bf_9settconvrate  = 9,
   bf_11stan         = 11,     /* system trace number                  */
   bf_12datetime         = 12,     /* time                                 */
   bf_13date         = 13,     /* date, trans                          */
   bf_14expiry		 = 14,	   /* date, expiry						   */
   bf_15datestl      = 15,     /* settlement date                      */
   bf_16dateconv     = 16,
   bf_18mertype		 = 18,	   /* merchant type						   */
   bf_19ctrycd       = 19,     /* Acquiring institution's country code */
   bf_20countrycode  = 20,
   bf_21fwdinstcntcode = 21,   /* Forwarding institution country code  */
   bf_22posentry     = 22,     /* pos entry mode                       */
   bf_24funccd		 = 24,
   bf_25poscc        = 25,     /* pos condition code                   */
   bf_26accptcd		 = 26,		
   bf_28tranfeeamt   = 28,     /* surcharge fee */
   bf_30origamt      = 30,     /* original amount   */
   bf_31tranid = 31,	   /* Acquirer reference data, Transaction Identifier */
   bf_32idcode       = 32,     /* Acquiring institution's ID code      */
   bf_33fwdinstidcode= 33,     /* Forwarding institution ID code */
   bf_35track2       = 35,     /* track 2 data                         */
   bf_37rrn	         = 37,	    /* retrieval ref number */
   bf_38authcode     = 38,     /* auth code                            */
   bf_39respcode     = 39,     /* response code                        */
   bf_41termid       = 41,     /* terminal id                          */
   bf_42cardacc      = 42,     /* card acceptor id                     */
   bf_43cardacceptloc= 43,     /* card acceptor name/location */
   bf_44respdata	 = 44,
   bf_45track1       = 45,     /* track 1 data   */
   bf_48addldata     = 48,     /* additional data                      */
   bf_49trncurcd     = 49,     /* transaction currency code            */
   bf_50recncurcd    = 50,     /* reconcilliation currency code */
   bf_52pinblock     = 52,     /* pin block                            */
   bf_53seccntl      = 53,     /* security related control info */
   bf_55IccData		 = 55,		/*emv data*/
   bf_56orgdata      = 56,     /* original data elements */	   
   bf_63priv63		 = 63     /* private field 63 */
   }; 

/* LENGTHs FOR EACH BIT FIELD */
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
#define bf20_len      3
#define bf22_len      3
#define bf32_len      2
#define bf32data_len 11
#define bf33_len      2
#define bf35_len      2
#define bf37_len     12
#define bf38_len      6
#define bf39_len      2
#define bf41_len      8
#define bf42_len     15
#define bf44_address_verif_len      2
#define bf45_len      2
#define bf48_len      3
#define bf49_len      3
#define bf50_len      3
#define bf51_len      3
#define bf54_len      3
#define bf55_len	  3
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
   BYTE   td_proccode[6]; /* iso 8583 processing code  */
   enum   tranopts1  td_opts1;   /* options */
   enum   tranopts2  td_opts2;
   enum   bitfields  td_bitfields[EXTENDED_BITFIELD]; /* fields to put in message */
};

static  struct trandef TRANTABLE[] = {
   /* 1100 MESSAGES */
   {
       /********************************************
          A U T H O R I Z A T I O N
        ********************************************/
        AUTH_AUTHORIZATION,
        0xF1,0xF1,0xF0,0xF0,                  /* message id, 1100 */
        0xF0,0xF0,0xF4,0xF0,0xF0,0xF0,        /* processing code */
		  /**** OPTIONS ****/
		  topt1_postadv,
		  topt2_referral+topt2_reversal+topt2_floor,
		  /**** BIT FIELDS ****/
		  bf_2pan,				/* MAND */
		  bf_3proc,				/* MAND */
		  bf_4amount,			/* MAND */
		  bf_7trntime,			/* OPT */
		  bf_11stan,			/* MAND */
		  bf_12datetime,		/* MAND */ 
		  bf_13date,			/* OPT */ 
		  bf_14expiry,			/* OPT */
		  bf_19ctrycd,			/* MAND */
		  bf_22posentry,		/* MAND */
		  bf_26accptcd,		/* MAND */
		  bf_32idcode,			/* MAND */
		  bf_35track2,			/* COND */
		  bf_37rrn,				/* OPT */
		  bf_41termid,			/* OPT */
		  bf_42cardacc,		/* MAND */      
		  bf_43cardacceptloc,/* OPT */
		  bf_45track1,	      /* COND */
		  bf_49trncurcd,     /* MAND */
		  bf_53seccntl,      /* COND */
		  bf_55IccData,		/*COND*/
		  bf_done			/* end of fields to move */
      },

      {
		  /***********************************************
			 A U T H O R I Z A T I O N   R E S P O N S E
		  ***********************************************/
		  AUTH_AUTHORIZATION_RESPONSE,
		  0xF1,0xF1,0xF1,0xF0,                  /* message id, 1110 */
		  0xF0,0xF0,0xF4,0xF0,0xF0,0xF0,        /* processing code */
		  /**** OPTIONS ****/
		  topt1_postadv,
		  topt2_referral+topt2_reversal+topt2_floor,
		  /**** BIT FIELDS ****/
		  bf_2pan,				/* MAND ECHO */
		  bf_3proc,				/* MAND ECHO */
		  bf_4amount,			/* MAND ECHO */
		  bf_7trntime,			/* OPT  ECHO */
		  bf_11stan,			/* MAND ECHO*/
		  bf_12datetime,		/* MAND ECHO*/ 
		  bf_24funccd,			/* COND ECHO*/
		  bf_31tranid,			/* COND */		
		  bf_32idcode,			/* MAND ECHO*/
		  bf_37rrn,				/* OPT  ECHO*/
	      bf_38authcode,		/* COND */
		  bf_39respcode,		/* MAND */
		  bf_41termid,			/* OPT  ECHO*/
		  bf_42cardacc,			/* MAND ECHO*/      
		  bf_49trncurcd,		/* MAND ECHO*/
		  bf_55IccData,			/*COND*/
		  bf_done				/* end of fields to move */
      },

	  {
	     /********************************************
			 P O S   S A L E
		  ********************************************/
		  AUTH_SALE,
		  0xF1,0xF1,0xF0,0xF0,                  /* message id, 1100 */
		  0xF0,0xF0,0xF4,0xF0,0xF0,0xF0,        /* processing code */
		  /**** OPTIONS ****/
		  topt1_postadv,
		  topt2_referral+topt2_reversal+topt2_floor,
		  /**** BIT FIELDS ****/
		  bf_2pan,				/* MAND */
		  bf_3proc,				/* MAND */
		  bf_4amount,			/* MAND */
		  bf_7trntime,			/* OPT */
		  bf_11stan,			/* MAND */
		  bf_12datetime,			/* MAND */ 
		  bf_13date,			/* OPT */ 
		  bf_14expiry,			/* OPT */
		  bf_19ctrycd,			/* MAND */
		  bf_22posentry,		/* MAND */
		  bf_26accptcd,			/* MAND */
		  bf_32idcode,			/* MAND */
		  bf_35track2,			/* COND */
		  bf_37rrn,				/* OPT */
		  bf_41termid,			/* OPT */
		  bf_42cardacc,			/* MAND */      
		  bf_43cardacceptloc,	/* OPT */
		  bf_45track1,		    /* COND */
		  bf_49trncurcd,		/* MAND */
		  bf_53seccntl,			/* COND */
		  bf_55IccData,
		  bf_done			/* end of fields to move */
      },

      {
		  /***********************************************
			 P O S   S A L E  R E S P O N S E
		  ***********************************************/
		  AUTH_SALE_RESPONSE,
		  0xF1,0xF1,0xF1,0xF0,                  /* message id, 1110 */
		  0xF0,0xF0,0xF4,0xF0,0xF0,0xF0,        /* processing code */
		  /**** OPTIONS ****/
		  topt1_postadv,
		  topt2_referral+topt2_reversal+topt2_floor,
		  /**** BIT FIELDS ****/
		  bf_2pan,				/* MAND ECHO */
		  bf_3proc,				/* MAND ECHO */
		  bf_4amount,			/* MAND ECHO */
		  bf_7trntime,			/* OPT  ECHO */
		  bf_11stan,			/* MAND ECHO*/
		  bf_12datetime,		/* MAND ECHO*/ 
		  bf_24funccd,			/* COND ECHO*/
		  bf_31tranid,			/* COND */		
		  bf_32idcode,			/* MAND ECHO*/
		  bf_37rrn,				/* OPT  ECHO*/
	      bf_38authcode,		/* COND */
		  bf_39respcode,		/* MAND */
		  bf_41termid,			/* OPT  ECHO*/
		  bf_42cardacc,			/* MAND ECHO*/      
		  bf_49trncurcd,		/* MAND ECHO*/
		  bf_55IccData,			/*COND*/
		  bf_done				/* end of fields to move */
      },

	  {
		  /********************************************
			 A U T H   C A S H   A D V A N C E
		  ********************************************/
		  AUTH_CASH_ADVANCE,
		  0xF1,0xF1,0xF0,0xF0,                  /* message id, 1100 */
		  0xF0,0xF1,0xF4,0xF0,0xF0,0xF0,        /* processing code */
		  /**** OPTIONS ****/
		  topt1_postadv,
		  topt2_referral+topt2_reversal+topt2_floor,
		  /**** BIT FIELDS ****/
		  bf_2pan,				/* MAND */
		  bf_3proc,				/* MAND */
		  bf_4amount,			/* MAND */
		  bf_7trntime,			/* OPT */
		  bf_11stan,			/* MAND */
		  bf_12datetime,			/* MAND */ 
		  bf_13date,			/* OPT */ 
		  bf_14expiry,			/* OPT */
		  bf_19ctrycd,			/* MAND */
		  bf_22posentry,		/* MAND */
		  bf_26accptcd,			/* MAND */
		  bf_32idcode,			/* MAND */
		  bf_35track2,			/* COND */
		  bf_37rrn,				/* OPT */
		  bf_41termid,			/* OPT */
		  bf_42cardacc,			/* MAND */      
		  bf_43cardacceptloc,	/* OPT */
		  bf_45track1,		    /* COND */
		  bf_49trncurcd,		/* MAND */
		  bf_53seccntl,			/* COND */
		  bf_done			/* end of fields to move */
      },

      {
		  /*************************************************
		   A U T H   C A S H   A D V A N C E  R E S P O N S E
		  *************************************************/
		  AUTH_CASH_ADVANCE_RESPONSE,
		  0xF1,0xF1,0xF1,0xF0,                  /* message id, 1110 */
		  0xF0,0xF1,0xF4,0xF0,0xF0,0xF0,        /* processing code */
		  /**** OPTIONS ****/
		  topt1_postadv,
		  topt2_referral+topt2_reversal+topt2_floor,
		  /**** BIT FIELDS ****/
		  bf_2pan,				/* MAND ECHO */
		  bf_3proc,				/* MAND ECHO */
		  bf_4amount,			/* MAND ECHO */
		  bf_7trntime,			/* OPT  ECHO */
		  bf_11stan,			/* MAND ECHO*/
		  bf_12datetime,			/* MAND ECHO*/ 
		  bf_24funccd,			/* COND ECHO*/
		  bf_31tranid,			/* COND */		
		  bf_32idcode,			/* MAND ECHO*/
		  bf_37rrn,				/* OPT  ECHO*/
	      bf_38authcode,		/* COND */
		  bf_39respcode,		/* MAND */
		  bf_41termid,			/* OPT  ECHO*/
		  bf_42cardacc,			/* MAND ECHO*/      
		  bf_49trncurcd,		/* MAND ECHO*/
		  bf_done      /* end of fields to move */
      },
	  /* 1120 MESSAGES */
	{
		  /********************************************
			 A U T H O R I Z A T I O N   A D V I C E
		  ********************************************/
		  AUTH_ADVICE,
		  0xF1,0xF1,0xF2,0xF0,                  /* message id, 1120 */
		  0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,        /* processing code */
		  /**** OPTIONS ****/
		  topt1_postadv,
		  topt2_referral+topt2_reversal+topt2_floor,
		  /**** BIT FIELDS ****/
		  bf_2pan,				/* MAND */
		  bf_3proc,				/* MAND */
		  bf_4amount,			/* MAND */
		  bf_11stan,			/* MAND */
		  bf_12datetime,			/* MAND */ 
		  bf_14expiry,			/* OPT */
		  bf_22posentry,		/* MAND */
		  bf_24funccd,			/* COND */
		  bf_26accptcd,			/* MAND */
		  bf_32idcode,			/* MAND */
		  bf_35track2,			/* COND */
		  bf_38authcode,		/* COND */
		  bf_39respcode,		/* MAND */
		  bf_42cardacc,			/* MAND */      
		  bf_45track1,		    /* COND */
		  bf_49trncurcd,		/* MAND */
		  bf_done			/* end of fields to move */
      },

      {
		  /***********************************************************
			 A U T H O R I Z A T I O N   A D V I C E   R E S P O N S E
		  ************************************************************/
		  AUTH_ADVICE_RESPONSE,
		  0xF1,0xF1,0xF3,0xF0,                  /* message id, 1130 */
		  0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,        /* processing code */
		  /**** OPTIONS ****/
		  topt1_postadv,
		  topt2_referral+topt2_reversal+topt2_floor,
		  /**** BIT FIELDS ****/
		  bf_2pan,				/* MAND ECHO */
		  bf_3proc,				/* MAND ECHO */
		  bf_4amount,			/* MAND ECHO */
		  bf_11stan,			/* MAND ECHO*/
		  bf_12datetime,			/* MAND ECHO*/ 
		  bf_31tranid,			/* COND */		
		  bf_32idcode,			/* MAND ECHO*/
		  bf_37rrn,				/* OPT  ECHO*/
		  bf_39respcode,		/* MAND */
		  bf_42cardacc,			/* MAND ECHO*/      
		  bf_49trncurcd,		/* MAND ECHO*/
		  bf_done				/* end of fields to move */
      },

	 {
		  /********************************************
			 A U T H O R I Z A T I O N   R E F U N D 
		  ********************************************/
		  AUTH_REFUND,
		  0xF1,0xF1,0xF2,0xF0,                  /* message id , 1120 */
		  0xF2,0xF0,0xF4,0xF0,0xF0,0xF0,        /* processing code */
		  /**** OPTIONS ****/
		  topt1_postadv,
		  topt2_referral+topt2_reversal+topt2_floor,
		  /**** BIT FIELDS ****/
		  bf_2pan,				/* MAND */
		  bf_3proc,				/* MAND */
		  bf_4amount,			/* MAND */
		  bf_11stan,			/* MAND */
		  bf_12datetime,			/* MAND */ 
		  bf_14expiry,			/* OPT */
		  bf_22posentry,		/* MAND */
		  bf_24funccd,			/* COND */
		  bf_26accptcd,			/* MAND */
		  bf_32idcode,			/* MAND */
		  bf_35track2,			/* COND */
		  bf_38authcode,		/* COND */
		  bf_39respcode,		/* MAND */
		  bf_42cardacc,			/* MAND */      
		  bf_45track1,		    /* COND */
		  bf_49trncurcd,			/* MAND */
		  bf_done			/* end of fields to move */
      },

      {
		  /***********************************************************
		  A U T H O R I Z A T I O N  R E F U N D  R E S P O N S E
		  ************************************************************/
		  AUTH_REFUND_RESPONSE,
		  0xF1,0xF1,0xF3,0xF0,                  /* message id, EBCDIC => 1130 */
		  0xF2,0xF0,0xF4,0xF0,0xF0,0xF0,        /* processing code */
		  /**** OPTIONS ****/
		  topt1_postadv,
		  topt2_referral+topt2_reversal+topt2_floor,
		  /**** BIT FIELDS ****/
		  bf_2pan,				/* MAND ECHO */
		  bf_3proc,				/* MAND ECHO */
		  bf_4amount,			/* MAND ECHO */
		  bf_11stan,			/* MAND ECHO*/
		  bf_12datetime,			/* MAND ECHO*/ 
		  bf_31tranid,			/* COND */		
		  bf_32idcode,			/* MAND ECHO*/
		  bf_37rrn,				/* OPT  ECHO*/
		  bf_39respcode,		/* MAND */
		  bf_42cardacc,			/* MAND ECHO*/      
		  bf_49trncurcd,		/* MAND ECHO*/
		  bf_done				/* end of fields to move */
      },

	    /* 1200 MESSAGES */
      {
		  /********************************************
			ATM C A S H
		  ********************************************/
		  AUTH_SALE_CASH,
		  0xF1,0xF2,0xF0,0xF0,                  /* message id, 1200 */
		  0xF0,0xF1,0xF4,0xF0,0xF0,0xF0,        /* processing code */
		  /**** OPTIONS ****/
		  topt1_postadv,
		  topt2_referral+topt2_reversal+topt2_floor,
		  /**** BIT FIELDS ****/
		  bf_3proc,				/* MAND */
		  bf_4amount,			/* MAND */
		  bf_5recon_amt,        /* OPT */
		  bf_11stan,			/* MAND */
		  bf_12datetime,			/* MAND */ 
		  bf_19ctrycd,			/* MAND */
		  bf_22posentry,		/* MAND */
		  bf_24funccd,			/* MAND */
		  bf_26accptcd,			/* OPT */
		  bf_32idcode,			/* MAND */
		  bf_33fwdinstidcode,   /* OPT */
		  bf_35track2,			/* MAND */
		  bf_37rrn,				/* OPT */
		  bf_41termid,			/* MAND */
		  bf_43cardacceptloc,	/* MAND */
		  bf_49trncurcd,		/* MAND */
		  bf_50recncurcd,	    /* OPT */
		  bf_52pinblock,	    /* MAND */
		  bf_done			/* end of fields to move */
      },

      {
		  /***********************************************
		   ATM	 C A S H  R E S P O N S E
		  ***********************************************/
		  AUTH_SALE_CASH_RESPONSE,
		  0xF1,0xF2,0xF1,0xF0,                  /* message id, 1110 */
		  0xF0,0xF1,0xF4,0xF0,0xF0,0xF0,        /* processing code */
		  /**** OPTIONS ****/
		  topt1_postadv,
		  topt2_referral+topt2_reversal+topt2_floor,
		  /**** BIT FIELDS ****/
		  bf_2pan,				/* MAND */
		  bf_3proc,				/* MAND ECHO */
		  bf_4amount,			/* MAND */
		  bf_5recon_amt,		/* MAND */
		  bf_11stan,			/* MAND ECHO*/
		  bf_12datetime,			/* MAND ECHO*/
  		  bf_19ctrycd,			/* MAND ECHO*/
		  bf_22posentry,		/* MAND ECHO*/
		  bf_24funccd,			/* MAND ECHO*/
		  bf_26accptcd,			/* COND ECHO*/
		  bf_31tranid,			/* COND */		
		  bf_32idcode,			/* MAND ECHO*/
		  bf_33fwdinstidcode,   /* COND ECHO*/
		  bf_37rrn,				/* COND ECHO*/
	      bf_38authcode,		/* MAND */
		  bf_39respcode,		/* MAND */
		  bf_41termid,			/* MAND ECHO*/
		  bf_49trncurcd,		/* MAND ECHO*/
		  bf_50recncurcd,	    /* MAND */
		  bf_done				/* end of fields to move */
      },
	  /* 1420 MESSAGES */
	   {
		  /********************************
			 R E V E R S A L 
		  **********************************/
		  AUTH_REVERSAL,
		  0xF1,0xF4,0xF2,0xF0,                  /* message id, 1420 */
		  0xF0,0xF0,0xF4,0xF0,0xF0,0xF0,        /* processing code */
		  /**** OPTIONS ****/
		  topt1_postadv,
		  topt2_referral+topt2_reversal+topt2_floor,
		  /**** BIT FIELDS ****/
		  bf_2pan,				/* MAND */
		  bf_3proc,				/* MAND */
		  bf_4amount,			/* MAND */
		  bf_11stan,			/* MAND */
		  bf_12datetime,     /* MAND */ 
		  bf_24funccd,			/* MAND */
		  bf_25poscc,			/* MAND */
		  bf_31tranid,			/* COND */		
		  bf_32idcode,			/* OPT */
		  bf_37rrn,				/* OPT */
		  bf_38authcode,		/* OPT */
        bf_41termid,       /* COND */
        bf_42cardacc,      /* COND */      
		  bf_49trncurcd,		/* MAND */
		  bf_56orgdata,			/* OPT */
		  bf_done			/* end of fields to move */
      },

      {
		  /***********************************************
			 R E V E R S A L   R E S P O N S E
		  ***********************************************/
		  AUTH_REVERSAL_RESPONSE,
		  0xF1,0xF4,0xF3,0xF0,                  /* message id, 1430 */
		  0xF0,0xF0,0xF4,0xF0,0xF0,0xF0,        /* processing code */
		  /**** OPTIONS ****/
		  topt1_postadv,
		  topt2_referral+topt2_reversal+topt2_floor,
		  /**** BIT FIELDS ****/
		  bf_2pan,				/* MAND ECHO */
		  bf_3proc,				/* MAND ECHO */
		  bf_4amount,			/* MAND ECHO */
		  bf_11stan,			/* MAND ECHO*/
		  bf_12datetime,			/* MAND ECHO*/ 
		  bf_31tranid,			/* COND */		
		  bf_32idcode,			/* OPT  ECHO*/
		  bf_37rrn,				/* OPT  ECHO*/
		  bf_39respcode,		/* MAND */
        bf_41termid,       /* COND */
        bf_42cardacc,      /* COND */      
		  bf_49trncurcd,		/* MAND ECHO*/
		  bf_done				/* end of fields to move */
      },

      {
		  /********************************
			 R E V E R S A L  A D V I C E
		  **********************************/
		  AUTH_REVERSAL_ADVICE,
		  0xF1,0xF4,0xF2,0xF0,                  /* message id, 1420 */
		  0xF0,0xF0,0xF4,0xF0,0xF0,0xF0,        /* processing code */
		  /**** OPTIONS ****/
		  topt1_postadv,
		  topt2_referral+topt2_reversal+topt2_floor,
		  /**** BIT FIELDS ****/
		  bf_2pan,				/* MAND */
		  bf_3proc,				/* MAND */
		  bf_4amount,			/* MAND */
		  bf_11stan,			/* MAND */
		  bf_12datetime,			/* MAND */ 
		  bf_24funccd,			/* MAND */
		  bf_25poscc,			/* MAND */
		  bf_31tranid,			/* COND */		
		  bf_32idcode,			/* OPT */
		  bf_37rrn,				/* OPT */
		  bf_38authcode,		/* OPT */
        bf_41termid,       /* COND */
        bf_42cardacc,      /* COND */      
		  bf_49trncurcd,		/* MAND */
		  bf_56orgdata,			/* OPT */
		  bf_done			/* end of fields to move */
      },

      {
		  /***********************************************
			 R E V E R S A L  A D V I C E   R E S P O N S E
		  ***********************************************/
		  AUTH_REVERSAL_ADVICE_RESPONSE,
		  0xF1,0xF4,0xF3,0xF0,                  /* message id, 1430 */
		  0xF0,0xF0,0xF4,0xF0,0xF0,0xF0,        /* processing code */
		  /**** OPTIONS ****/
		  topt1_postadv,
		  topt2_referral+topt2_reversal+topt2_floor,
		  /**** BIT FIELDS ****/
		  bf_2pan,				/* MAND ECHO */
		  bf_3proc,				/* MAND ECHO */
		  bf_4amount,			/* MAND ECHO */
		  bf_11stan,			/* MAND ECHO*/
		  bf_12datetime,			/* MAND ECHO*/ 
		  bf_31tranid,			/* COND */		
		  bf_32idcode,			/* OPT  ECHO*/
		  bf_37rrn,				/* OPT  ECHO*/
		  bf_39respcode,		/* MAND */
        bf_41termid,       /* COND */
        bf_42cardacc,      /* COND */      
		  bf_49trncurcd,		/* MAND ECHO*/
		  bf_done				/* end of fields to move */
      },
      {
		  /********************************
			ATM R E V E R S A L  A D V I C E
		  **********************************/
		  AUTH_ATM_CONFIRM,
		  0xF1,0xF4,0xF2,0xF0,                  /* message id, 1420 */
		  0xF0,0xF1,0xF4,0xF0,0xF0,0xF0,        /* processing code */
		  /**** OPTIONS ****/
		  topt1_postadv,
		  topt2_referral+topt2_reversal+topt2_floor,
		  /**** BIT FIELDS ****/
		  bf_2pan,				/* MAND */
		  bf_4amount,			/* MAND */
		  bf_5recon_amt,		/* OPT */
		  bf_11stan,			/* MAND */
		  bf_12datetime,		/* MAND */ 
		  bf_19ctrycd,		    /* MAND */
		  bf_24funccd,			/* MAND */
		  bf_25poscc,			/* MAND */
        bf_30origamt,      /* MAND */
		  bf_31tranid,			/* COND */		
		  bf_32idcode,			/* MAND ECHO */
		  bf_33fwdinstidcode,   /* OPT */
		  bf_37rrn,				/* OPT */
		  bf_41termid,			/* MAND ECHO*/
		  bf_49trncurcd,		/* MAND */
		  bf_50recncurcd,	    /* MAND */
		  bf_56orgdata,			/* OPT */
		  bf_done			/* end of fields to move */
      },

      {
		  /***********************************************
			 A T M  R E V E R S A L  A D V I C E   R E S P O N S E
		  ***********************************************/
		  AUTH_ATM_CONFIRM_RESPONSE,
		  0xF1,0xF4,0xF3,0xF0,                  /* message id, 1430 */
		  0xF0,0xF1,0xF4,0xF0,0xF0,0xF0,        /* processing code */
		  /**** OPTIONS ****/
		  topt1_postadv,
		  topt2_referral+topt2_reversal+topt2_floor,
		  /**** BIT FIELDS ****/
		  bf_2pan,				/* MAND ECHO */
		  bf_4amount,			/* MAND ECHO */
		  bf_5recon_amt,		/* MAND */
		  bf_11stan,			/* MAND ECHO*/
		  bf_12datetime,		/* MAND ECHO*/ 
		  bf_19ctrycd,		    /* MAND ECHO*/
		  bf_31tranid,			/* COND ECHO*/		
		  bf_32idcode,			/* MAND ECHO*/
		  bf_37rrn,				/* COND ECHO*/
		  bf_39respcode,		/* MAND */
		  bf_41termid,			/* MAND ECHO*/
		  bf_49trncurcd,		/* MAND ECHO*/
		  bf_50recncurcd,	    /* MAND */
		  bf_56orgdata,			/* MAND ECHO */
		  bf_done				/* end of fields to move */
      },

	   {
		  /********************************
			 VOID CASH ADVANCE 
		  **********************************/
		  AUTH_VOID_CASH_ADVANCE,
		  0xF1,0xF4,0xF2,0xF0,                  /* message id, 1420 */
		  0xF0,0xF1,0xF4,0xF0,0xF0,0xF0,        /* processing code */
		  /**** OPTIONS ****/
		  topt1_postadv,
		  topt2_referral+topt2_reversal+topt2_floor,
		  /**** BIT FIELDS ****/
		  bf_2pan,				/* MAND */
		  bf_3proc,				/* MAND */
		  bf_4amount,			/* MAND */
		  bf_11stan,			/* MAND */
		  bf_12datetime,			/* MAND */ 
		  bf_24funccd,			/* MAND */
		  bf_25poscc,			/* MAND */
		  bf_31tranid,			/* COND */		
		  bf_32idcode,			/* OPT */
		  bf_37rrn,				/* OPT */
		  bf_38authcode,		/* OPT */
		  bf_42cardacc,			/* MAND */      
		  bf_49trncurcd,		/* MAND */
		  bf_56orgdata,			/* OPT */
		  bf_done			/* end of fields to move */
      },

      {
		  /***********************************************
			 VOID CASH ADVANCE R E S P O N S E
		  ***********************************************/
		  AUTH_VOID_CASH_ADVANCE_RESPONSE,
		  0xF1,0xF4,0xF3,0xF0,                  /* message id, 1430 */
		  0xF0,0xF1,0xF4,0xF0,0xF0,0xF0,        /* processing code */
		  /**** OPTIONS ****/
		  topt1_postadv,
		  topt2_referral+topt2_reversal+topt2_floor,
		  /**** BIT FIELDS ****/
		  bf_2pan,				/* MAND ECHO */
		  bf_3proc,				/* MAND ECHO */
		  bf_4amount,			/* MAND ECHO */
		  bf_11stan,			/* MAND ECHO*/
		  bf_12datetime,			/* MAND ECHO*/ 
		  bf_31tranid,			/* COND */		
		  bf_32idcode,			/* OPT  ECHO*/
		  bf_37rrn,				/* OPT  ECHO*/
		  bf_39respcode,		/* MAND */
		  bf_42cardacc,			/* MAND ECHO*/      
		  bf_49trncurcd,		/* MAND ECHO*/
		  bf_done				/* end of fields to move */
      },
	   {
		  /********************************
			 VOID SALE
		  **********************************/
		  AUTH_VOID_SALE,
		  0xF1,0xF4,0xF2,0xF0,                  /* message id, 1420 */
		  0xF0,0xF0,0xF4,0xF0,0xF0,0xF0,        /* processing code */
		  /**** OPTIONS ****/
		  topt1_postadv,
		  topt2_referral+topt2_reversal+topt2_floor,
		  /**** BIT FIELDS ****/
		  bf_2pan,				/* MAND */
		  bf_3proc,				/* MAND */
		  bf_4amount,			/* MAND */
		  bf_11stan,			/* MAND */
		  bf_12datetime,			/* MAND */ 
		  bf_24funccd,			/* MAND */
		  bf_25poscc,			/* MAND */
		  bf_31tranid,			/* COND */		
		  bf_32idcode,			/* OPT */
		  bf_37rrn,				/* OPT */
		  bf_38authcode,		/* OPT */
		  bf_42cardacc,			/* MAND */      
		  bf_49trncurcd,		/* MAND */
		  bf_56orgdata,			/* OPT */
		  bf_done			/* end of fields to move */
      },

      {
		  /***********************************************
			 VOID SALE R E S P O N S E
		  ***********************************************/
		  AUTH_VOID_SALE_RESPONSE,
		  0xF1,0xF4,0xF3,0xF0,                  /* message id, 1430 */
		  0xF0,0xF0,0xF4,0xF0,0xF0,0xF0,        /* processing code */
		  /**** OPTIONS ****/
		  topt1_postadv,
		  topt2_referral+topt2_reversal+topt2_floor,
		  /**** BIT FIELDS ****/
		  bf_2pan,				/* MAND ECHO */
		  bf_3proc,				/* MAND ECHO */
		  bf_4amount,			/* MAND ECHO */
		  bf_11stan,			/* MAND ECHO*/
		  bf_12datetime,			/* MAND ECHO*/ 
		  bf_31tranid,			/* COND */		
		  bf_32idcode,			/* OPT  ECHO*/
		  bf_37rrn,				/* OPT  ECHO*/
		  bf_39respcode,		/* MAND */
		  bf_42cardacc,			/* MAND ECHO*/      
		  bf_49trncurcd,		/* MAND ECHO*/
		  bf_done				/* end of fields to move */
      },
	   {
		  /********************************
			VOID REFUND
		  **********************************/
		  AUTH_VOID_REFUND,
		  0xF1,0xF4,0xF2,0xF0,                  /* message id, 1420 */
		  0xF2,0xF0,0xF4,0xF0,0xF0,0xF0,        /* processing code */
		  /**** OPTIONS ****/
		  topt1_postadv,
		  topt2_referral+topt2_reversal+topt2_floor,
		  /**** BIT FIELDS ****/
		  bf_2pan,				/* MAND */
		  bf_3proc,				/* MAND */
		  bf_4amount,			/* MAND */
		  bf_11stan,			/* MAND */
		  bf_12datetime,			/* MAND */ 
		  bf_24funccd,			/* MAND */
		  bf_25poscc,			/* MAND */
		  bf_31tranid,			/* COND */		
		  bf_32idcode,			/* OPT */
		  bf_37rrn,				/* OPT */
		  bf_38authcode,		/* OPT */
		  bf_42cardacc,			/* MAND */      
		  bf_49trncurcd,		/* MAND */
		  bf_56orgdata,			/* OPT */
		  bf_done			/* end of fields to move */
      },

      {
		  /***********************************************
			 V O I D   R E F U N D  R E S P O N S E
		  ***********************************************/
		  AUTH_VOID_REFUND_RESPONSE,
		  0xF1,0xF4,0xF3,0xF0,                  /* message id, 1430 */
		  0xF2,0xF0,0xF4,0xF0,0xF0,0xF0,        /* processing code */
		  /**** OPTIONS ****/
		  topt1_postadv,
		  topt2_referral+topt2_reversal+topt2_floor,
		  /**** BIT FIELDS ****/
		  bf_2pan,				/* MAND ECHO */
		  bf_3proc,				/* MAND ECHO */
		  bf_4amount,			/* MAND ECHO */
		  bf_11stan,			/* MAND ECHO*/
		  bf_12datetime,			/* MAND ECHO*/ 
		  bf_31tranid,			/* COND */		
		  bf_32idcode,			/* OPT  ECHO*/
		  bf_37rrn,				/* OPT  ECHO*/
		  bf_39respcode,		/* MAND */
		  bf_42cardacc,			/* MAND ECHO*/      
		  bf_49trncurcd,		/* MAND ECHO*/
		  bf_done				/* end of fields to move */
      },
      {
         /********************************
             O N L I N E   U P D A T E
          ********************************/
         AUTH_CARD_UPDATE_RESPONSE,
         0xF1,0xF3,0xF1,0xF4,                  /* message id, 1314 */
         0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,        /* processing code */
         /**** OPTIONS ****/
         topt1_postadv,
         topt2_referral+topt2_reversal+topt2_floor,
         /**** BIT FIELDS ****/
         bf_2pan,             /* MAND */
         bf_11stan,           /* MAND */
         bf_12datetime,       /* MAND */ 
         bf_14expiry,         /* MAND */
         bf_24funccd,         /* MAND */
         bf_25poscc,          /* MAND */
         bf_33fwdinstidcode,  /* MAND */
         bf_39respcode,       /* MAND */
         bf_done			/* end of fields to move */
      },

	  	  /* 1800 MESSAGES */
      {
		  /********************************************
			 L O G O N
		  ********************************************/
		  AUTH_LOGON,
		  0xF1,0xF8,0xF0,0xF4,                  /* message id, 1804 */
		  0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,        /* processing code */
		  /**** OPTIONS ****/
		  topt1_postadv,
		  topt2_referral+topt2_reversal+topt2_floor,
		  /**** BIT FIELDS ****/
		  bf_3proc,				/* MAND */
		  bf_11stan,			/* MAND */
		  bf_12datetime,			/* MAND */ 
		  bf_24funccd,			/* MAND */
		  bf_33fwdinstidcode,   /* MAND */
		  bf_done			/* end of fields to move */
      },

      {
		  /***********************************************
			 L O G O N  R E S P O N S E
		  ***********************************************/
		  AUTH_LOGON_RESPONSE,
		  0xF1,0xF8,0xF1,0xF4,                  /* message id, 1814 */
		  0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,        /* processing code */
		  /**** OPTIONS ****/
		  topt1_postadv,
		  topt2_referral+topt2_reversal+topt2_floor,
		  /**** BIT FIELDS ****/
		  bf_3proc,				/* MAND ECHO */
		  bf_11stan,		/* MAND ECHO*/
		  bf_12datetime,		/* MAND ECHO*/ 
		  bf_24funccd,			/* MAND ECHO*/
		  bf_33fwdinstidcode,   /* MAND ECHO*/
		  bf_39respcode,		/* MAND */
		  bf_done				/* end of fields to move */
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
   { 15, 0x08 },   /* bit 61 */   { 15, 0x04 },   /* bit 62 */   { 15, 0x02 },   /* bit 63 */   { 15, 0x01 }   /* bit 64 */
   };


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


static  struct bitfield_data BFData[] = {
   {
   bf_2pan,                      /* field */
   HOST_MESSAGE.pan_len,         /* data to move */
   2,                            /* # of bytes in pan len field */
   Move_Always,                  /* mandatory */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_3proc,                     /* field */
   HOST_MESSAGE.proccode,        /* data to move */
   6,                            /* # of bytes in  iso field */
   Move_Always,                  /* mandatory */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_4amount,                   /* field */
   HOST_MESSAGE.amount,          /* data to move */
   12,                           /* # of bytes in  iso field */
   Move_Always,                  /* mandatory */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_5recon_amt,                /* field */
   HOST_MESSAGE.setl_amount,     /* data to move */
   12,                           /* # of bytes in  iso field */
   Move_IfThere,                  /* mandatory */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_7trntime,                  /* field */
   HOST_MESSAGE.gmt_time,        /* data to move : MMDDhhmmss*/
   10,                           /* # of bytes in  iso field */
   Move_IfThere,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_9settconvrate,             /* field */
   HOST_MESSAGE.setl_conv_rate,  /* data to move */
   8,                            /* # of bytes in  iso field */
   Move_IfThere,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_11stan,                    /* field */
   HOST_MESSAGE.stan,            /* data to move */
   6,                            /* # of bytes in  iso field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_12datetime,                    /* field */
   HOST_MESSAGE.datetime,            /* data to move */
   12,                            /* # of bytes in  iso field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_13date,                    /* field */
   HOST_MESSAGE.eff_date,            /* effective data to move */
   4,                            /* # of bytes in  iso field */
   Move_IfThere,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_14expiry,                  /* field */
   HOST_MESSAGE.date_expd,       /* data to move */
   4,                            /* # of bytes in field */
   Move_IfThere,                     /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_15datestl,                 /* field */
   HOST_MESSAGE.date_settle,     /* data to move */
   4,                            /* # of bytes in  iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_16dateconv,                /* field */
   HOST_MESSAGE.date_conv,       /* data to move */
   4,                            /* # of bytes in  iso field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_18mertype,                 /* field */
   HOST_MESSAGE.merch_type,            /* data to move */
   4,                            /* # of bytes in  iso field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_19ctrycd,                  /* field */
   HOST_MESSAGE.ctry_code,       /* data to move */
   3,                            /* # of bytes in field */
   Move_Always,                  /* mandatory    */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_20countrycode,                    /* field */
   HOST_MESSAGE.coun_code,            /* data to move */
   3,                            /* # of bytes in  iso field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
    {
   bf_21fwdinstcntcode,                    /* field */
   HOST_MESSAGE.fwd_inst_cnt_code,            /* data to move */
   3,                            /* # of bytes in  iso field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_22posentry,                /* field */
   HOST_MESSAGE.posentry,        /* data to move */
   12,                            /* # of bytes in  iso field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_24funccd,                  /* field */
   HOST_MESSAGE.func_cd,         /* data to move */
   3,                            /* # of bytes in field */
   Move_Always,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_25poscc,                   /* field */
   HOST_MESSAGE.pos_condcd,      /* data to move */
   4,                            /* # of bytes in  iso field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
    {
   bf_26accptcd,                 /* field */
   HOST_MESSAGE.accpt_bus_cd,    /* data to move */
   4,                            /* # of bytes in field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_28tranfeeamt,              /* field */
   HOST_MESSAGE.tran_fee_amt,      /* data to move */
   8,                            /* # of bytes in  iso field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_30origamt,                 /* field */
   HOST_MESSAGE.orig_amt,        /* data to move */
   24,                           /* # of bytes in field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_31tranid,                  /* field */
   HOST_MESSAGE.tranid_len,    /* data to move */
   2,                            /* # of bytes in  iso field */
   Move_IfThere,                  /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_32idcode,                  /* field */
   HOST_MESSAGE.acq_inst_len,    /* data to move */
   2,                            /* # of bytes in  iso field */
   Move_Always,                  /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_33fwdinstidcode,                  /* field */
   HOST_MESSAGE.fwd_inst_len,    /* data to move */
   2,                            /* # of bytes in  iso field */
   Move_IfThere,                  /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_35track2,                  /* field */
   HOST_MESSAGE.track2_len,      /* data to move */
   2,                            /* # of bytes in track2 len field */
   Move_IfThere,                  /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_37rrn,                  /* field */
   HOST_MESSAGE.rrn,      /* data to move */
   12,                            /* # of bytes in track2 len field */
   Move_IfThere,                  /* conditional */
   EBCDICMove,                  /* move routine */
   RspEBCDICMove                /* response routine */
   },
   {
   bf_38authcode,                /* field */
   HOST_MESSAGE.authcode,        /* data to move */
   6,                            /* # of bytes in  iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_39respcode,                /* field */
   HOST_MESSAGE.responsecd,      /* data to move */
   3,                            /* # of bytes in  iso field */
   Move_Always,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_41termid,                  /* field */
   HOST_MESSAGE.termid,          /* data to move */
   8,                            /* # of bytes in  iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_42cardacc,                 /* field */
   HOST_MESSAGE.cardaccid,       /* data to move */
   15,                           /* # of bytes in  iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_43cardacceptloc,                 /* field */
   HOST_MESSAGE.cardaccnamelen,       /* data to move */
   2,                           /* # of bytes in  iso field */
   Move_IfThere,                  /* conditional */
   vEBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_44respdata,                 /* field */
   HOST_MESSAGE.addl_rsp_len,       /* data to move */
   2,                           /* # of bytes in  iso field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_45track1,                  /* field */
   HOST_MESSAGE.track1_len,      /* data to move */
   2,                            /* # of bytes in len field */
   Move_IfThere,                   /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_48addldata,                /* field */
   HOST_MESSAGE.addl_data_len,   /* data to move */
   3,                            /* # of bytes in track1 len field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_49trncurcd,                /* field */
   HOST_MESSAGE.tran_curr_cd,    /* data to move */
   3,                            /* # of bytes in  iso field */
   Move_Always,                  /* mandatory    */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_50recncurcd,               /* field */
   HOST_MESSAGE.recn_curr_cd,    /* data to move */
   3,                            /* # of bytes in field */
   Move_IfThere,                 /* mandatory    */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_52pinblock,                /* field */
   HOST_MESSAGE.pinblock,        /* data to move */
   16,                            /* # of bytes in  iso field */
   Move_Always,                 /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_53seccntl,                 /* field */
   HOST_MESSAGE.sec_cntl_len,    /* data to move */
   2,                            /* # of bytes in field */
   Move_IfThere,                   /* mandatory    */
   vEBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
	bf_55IccData,                 /* field */
	HOST_MESSAGE.IccData_len,   /* data to move */
	3,                            /* # of bytes in field */
	Move_IfThere,                  /* mandatory    */
	vEBCDICMove,                  /* move routine */
	RspEBCDICvMove                /* response routine */
   },
   {
   bf_56orgdata,                 /* field */
   HOST_MESSAGE.orig_data_len,   /* data to move */
   2,                            /* # of bytes in field */
   Move_Always,                  /* mandatory    */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_63priv63,                 /* field */
   HOST_MESSAGE.priv63_len,     /* data to move */
   3,                           /* # of bytes in  iso field */
   Move_IfThere,                 /* mandatory    */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_done,                      /* field */
   NULL,                         /* data to move */
   99,                           /* # of bytes in  iso field */
   NULL,                         /* conditional */
   NULL,                         /* move routine */
   NULL                          /* response routine */
   }
   }; /* end BFData */


#define N_BFData ( sizeof(BFData) / sizeof (struct bitfield_data) )
          

/*******************************************************************************************************/
/* ROUTINES                                                                                            */
/*******************************************************************************************************/
struct bitfield_data *Find_Bitfield_Table (enum bitfields);
struct trandef       *Find_Tran_Table     (BYTE trantype);


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
      memcpy(pBF->p_bd_pdata,         /* to data pointer      */
             &ReqRsp_Buffer[reqidx],      /* move from request buffer */
             pBF->bd_fieldlen);      /* for length of field    */
   
      reqidx = reqidx + pBF->bd_fieldlen;   /* increment buffer pointer */
   }
} /* end RspMoveGeneric */


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
void ascii_to_bin_field55( CHAR *astr, INT ascii_len, BYTE *binary_string, INT blen)
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
   CHAR                 tempstr[4];

   /***************************/
		  char val_1[4] = "AGNS";
		  int j;
			BYTE header_version[5]="0001";
			BYTE val_2[3]={0};
		  char val_3[2] = "91";
		  char val_4[7] = "ABCDEF";
		  char val_6[3]		=	"00";
		  char rsp_55_msg[18] ={0}; //"AGNS000191ABCDEFXX00";
		  int temp_val = 0;
		 
		  


   /***************************/

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   /* Determine the value of the variable length field.  Note: for
      AMEX, even the length field is in EBCDIC.  For Visa, the
      length comes in as binary. */

   if (pBF != NULL)
   {
      memset (tempstr, 0, sizeof(tempstr));
      strncpy(tempstr, pBF->p_bd_pdata,pBF->bd_fieldlen);  // needed because no nulls in HOST_MESSAGE
      movelen = atoi(tempstr);
      for( i=0; i<pBF->bd_fieldlen; i++ )
      {
         ReqRsp_Buffer [reqidx] = ascii_ebcdic_conv (pBF->p_bd_pdata[i], A2E);
         reqidx++;
      }
	  if(fieldid == 55)
	  {
		  // Copy the data based on above len. Do the memcpy
			/*sprintf(rsp_55_msg,"%s",val_1,sizeof(val_1));
			ascii_to_bin_field55(header_version,strlen(header_version),&val_2[0],4);
			sprintf(rsp_55_msg,"%s%s",rsp_55_msg,val_2,sizeof(val_2));
			sprintf(rsp_55_msg,"%s%d",rsp_55_msg,val_3,sizeof(val_3));
			sprintf(rsp_55_msg,"%s%s",rsp_55_msg,val_4,sizeof(val_4));
		  /*for(i = 0;i<21;i++)
		  {
			sprintf(&rsp_55_msg[16],"%d",temp_55_counter,2);
			temp_55_counter++;
		  }----
			//sprintf(&rsp_55_msg[16],"%d",temp_55_counter,2);
		  sprintf(rsp_55_msg,"%s%02d",rsp_55_msg,temp_55_counter,2);
			temp_55_counter++;
		  sprintf(rsp_55_msg,"%s%s",rsp_55_msg,val_6,sizeof(val_6));
		  
		  sprintf(&pBF->p_bd_pdata[3],"%s",rsp_55_msg,sizeof(rsp_55_msg));*/

		    memcpy(rsp_55_msg,val_1,sizeof(val_1));
			ascii_to_bin_field55(header_version,strlen(header_version),&val_2[0],4);
			memcpy(&rsp_55_msg[4],val_2,sizeof(val_2));
			memcpy(&rsp_55_msg[6],val_3,sizeof(val_3));
			memcpy(&rsp_55_msg[8],val_4,sizeof(val_4));
			sprintf(&rsp_55_msg[13],"02%d",temp_55_counter);
			temp_55_counter++;
			memcpy(&rsp_55_msg[15],val_6,sizeof(val_6));
			memcpy(&pBF->p_bd_pdata[3],rsp_55_msg,sizeof(rsp_55_msg));
		 /* temp_val = (movelen+pBF->bd_fieldlen)-18;
		  
		  for(j = 18;j<temp_val;j++)
		  {
			 reqidx++;
		  }*/
		  for( i=pBF->bd_fieldlen; i<movelen+pBF->bd_fieldlen; i++ )
		{
			 ReqRsp_Buffer[reqidx] = ascii_ebcdic_conv (pBF->p_bd_pdata[i], A2E);
			reqidx++;
		}
	  }
	  else
	  {
		 for( i=pBF->bd_fieldlen; i<movelen+pBF->bd_fieldlen; i++ )
		{
			 ReqRsp_Buffer[reqidx] = ascii_ebcdic_conv (pBF->p_bd_pdata[i], A2E);
			reqidx++;
		}
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
   char                 templen[5];
   BYTE    fld_sep;


   /*---------------------------------------------------*/
   /* Find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   /* Determine the value of the variable length field.  Note: for
      AMEX, even the length field is in EBCDIC.  For Visa, the
      length comes in as binary. */

   if (pBF != NULL)
   {
      for( i=0; i<pBF->bd_fieldlen; i++ )
      {
         templen[i] = ascii_ebcdic_conv (ReqRsp_Buffer[reqidx], E2A);
         reqidx++;
      }
      movelen = atoi(templen);
      strcpy(pBF->p_bd_pdata, templen);
     
      if(fieldid == 45) //TRACK1
      {
         //call the function that returns the first non-numeric character
         fld_sep= getFldSep( ReqRsp_Buffer, reqidx); 
          /* Move the data, converting it from EBCDIC to ASCII. */
          for( i=pBF->bd_fieldlen; i<movelen+pBF->bd_fieldlen; i++)
          {
             pBF->p_bd_pdata [i] = ascii_ebcdic_conv_trk1 (ReqRsp_Buffer[reqidx], E2A,fld_sep);
             reqidx++;
          }

      }
	  else if(fieldid == 55) //EMV data
      {
		  for( i=pBF->bd_fieldlen; i<movelen+pBF->bd_fieldlen; i++)
		  {
			  reqidx++;
		  }
      }
      else
      {
          /* Move the data, converting it from EBCDIC to ASCII. */
          for( i=pBF->bd_fieldlen; i<movelen+pBF->bd_fieldlen; i++)
          {
             pBF->p_bd_pdata [i] = ascii_ebcdic_conv (ReqRsp_Buffer[reqidx], E2A);
             reqidx++;
          }
      }
   }
} /* end RspEBCDICvMove */


  
  
BYTE getFldSep( unsigned char* resp_buff, int index )
{
   INT   flg=1,cnt=0;
   BYTE  returnChar=0x00;

      cnt=index;
      while( flg == 1)
      {
        switch( resp_buff[cnt])
        {
        case 0xF0:
        case 0xF1:
        case 0xF2:
        case 0xF3:
        case 0xF4:
        case 0xF5:
        case 0xF6:
        case 0xF7:
        case 0xF8:
        case 0xF9:
        case 0xC2: cnt++;break; //"B" Format Specifier
        default:flg=0; returnChar=resp_buff[cnt];break;
        }
      }
      return( returnChar );
}   //end of getFldSep

BYTE ascii_ebcdic_conv_trk1( unsigned char conv_char, char *conv_type,unsigned char fldSep)
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
        |   ?     :     #     @     '     =     "     ^              |
        +------------------------------------------------------------+
      */

   static unsigned char ebcdic_chart[89] = 
      {
         0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9,
         0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x91,
         0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0xA2, 0xA3,
         0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xC1, 0xC2, 0xC3, 0xC4,
         0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5,
         0xD6, 0xD7, 0xD8, 0xD9, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7,
         0xE8, 0xE9, 0x40, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x5A,
         0x5B, 0x5C, 0x5D, 0x5E, 0x60, 0x61, 0x6B, 0x6C, 0x6D, 0x6E,
         0x6F, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F, 0xB0, 0x8A
      };

   static unsigned char ascii_chart[89] = 
      {
         0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
         0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A,
         0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73, 0x74,
         0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x41, 0x42, 0x43, 0x44,
         0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E,
         0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
         0x59, 0x5A, 0x20, 0x2E, 0x3C, 0x28, 0x2B, 0x7C, 0x26, 0x21,
         0x24, 0x2A, 0x29, 0x3B, 0x2D, 0x2F, 0x2C, 0x25, 0x5F, 0x3E,
         0x3F, 0x3A, 0x23, 0x40, 0x27, 0x3D, 0x22, 0x5E, 0x5E
      };


   if ( 0 == strcmp( conv_type, A2E ) )
   {
      /* CONVERT ASCII TO EBCDIC */
	  for( i=0; i<88 && !found; i++ )
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
      for( i=0; i<88 && !found; i++ )
      {
         if ( conv_char == ebcdic_chart[i] )
         {
            ReturnChar = ascii_chart[i];
            found = 1;
         }

      }
      if(fldSep==conv_char)
      {
        ReturnChar = 0x5E;
        found = 1;
      }

   }

   if ( 0 == found )
      ReturnChar = 0x00;

   return( ReturnChar );
}   //end of the ascii_ebcdic_conv_trk1
  
  
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
   char                 templen[5];

   /*---------------------------------------------------*/
   /* Find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   

	if(fieldid == 43) // name and location
	{
	   if (pBF != NULL)
	   {
		  for( i=0; i<pBF->bd_fieldlen; i++ )
		  {
			 templen[i] = ascii_ebcdic_conv (ReqRsp_Buffer[reqidx], E2A);
			 reqidx++;
		  }
		  movelen = atoi(templen);
		  strcpy(pBF->p_bd_pdata, templen);
     
  

			  /* Move the data, converting it from EBCDIC to ASCII. */
			  for( i=pBF->bd_fieldlen; i<movelen+pBF->bd_fieldlen; i++)
			  {
         		pBF->p_bd_pdata [i] = ascii_ebcdic_conv (ReqRsp_Buffer[reqidx], E2A);
         		reqidx++;
			  }

	   }
	}
	else
	{

		   if (pBF != NULL)
		   {
			  movelen = pBF->bd_fieldlen;

			  /* Move the data, converting it from EBCDIC to ASCII. */
			  for( i=0; i<movelen; i++)
			  {
				 pBF->p_bd_pdata [i] = ascii_ebcdic_conv (ReqRsp_Buffer[reqidx], E2A);
				 reqidx++;
			  }
		   }
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
      reqidx = reqidx + pBF->bd_fieldlen;   /* increment buffer pointer */
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

      reqidx = reqidx + movelen;   /* increment buffer pointer */
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

      memcpy(&pBF->p_bd_pdata[1], &ReqRsp_Buffer[reqidx], movelen);

      reqidx = reqidx + movelen;   /* increment buffer pointer */
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
      movelen = ReqRsp_Buffer [reqidx];
      pBF->p_bd_pdata[0] = ReqRsp_Buffer [reqidx];
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
   if (HOST_MESSAGE.msgtype [0] != 0x04)
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
      return(NULL);
   }
   return(NULL);

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
        |   ?     :     #     @     '     =     "     ^              |
        +------------------------------------------------------------+
      */

   static unsigned char ebcdic_chart[89] = 
      {
         0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9,
         0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x91,
         0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0xA2, 0xA3,
         0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xC1, 0xC2, 0xC3, 0xC4,
         0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5,
         0xD6, 0xD7, 0xD8, 0xD9, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7,
         0xE8, 0xE9, 0x40, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x5A,
         0x5B, 0x5C, 0x5D, 0x5E, 0x60, 0x61, 0x6B, 0x6C, 0x6D, 0x6E,
         0x6F, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F, 0xB0, 0x8A
      };

   static unsigned char ascii_chart[89] = 
      {
         0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
         0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A,
         0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73, 0x74,
         0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x41, 0x42, 0x43, 0x44,
         0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E,
         0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
         0x59, 0x5A, 0x20, 0x2E, 0x3C, 0x28, 0x2B, 0x7C, 0x26, 0x21,
         0x24, 0x2A, 0x29, 0x3B, 0x2D, 0x2F, 0x2C, 0x25, 0x5F, 0x3E,
         0x3F, 0x3A, 0x23, 0x40, 0x27, 0x3D, 0x22, 0x5E, 0x5E
      };


   if ( 0 == strcmp( conv_type, A2E ) )
   {
      /* CONVERT ASCII TO EBCDIC */
	  for( i=0; i<88 && !found; i++ )
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
      for( i=0; i<88 && !found; i++ )
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

  Function:    Generate_Response

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
WORD generate_response ()
{
   struct trandef       *pTranDef;
   struct bitfield_data *pBF;
   int                  bfidx;         /* index to bitfields for this transaction */

   pTranDef = Find_Tran_Table(tx_key);   /* get pointer to tran table */
   if (pTranDef != NULL)
   {    
      /*-----------------------------------------*/
      /* Copy in Message Id for this transaction */
      /*-----------------------------------------*/
      memset(ReqRsp_Buffer, 0, COMM_BUF_SZE);   /* zero out the buffer */
      bitmapidx = 4;                            /* point to bitmap     */

      
      /* Point past bitmap, but 1st check for extended bitmap. */
      if ( ReqRsp_Buffer[bitmapidx]  &  BITTAB[bf_1xbitfield].bit_mask )
      {
         reqidx = 20;                         /* Extended     */
         ReqRsp_Buffer[bitmapidx] |= BITTAB[bf_1xbitfield].bit_mask;   /* 2 bitmaps */
         BitFieldSize = EXTENDED_BITFIELD;
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
         memcpy (&HOST_MESSAGE.proccode, pTranDef->td_proccode, sizeof(pTranDef->td_proccode));

      if ( HOST_MESSAGE.responsecd[0] == 0x00 )
         memcpy( HOST_MESSAGE.responsecd, "800", 3 );  /* Approve */

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
               if ((pBF->bd_BFCond)(pBF->bd_fieldid,tx_key) == true)
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


}

/*****************************************************************************

  Function:    update_response

  Description: This function will set some response values.

******************************************************************************/
void update_response()
{
   memcpy( HOST_MESSAGE.authcode, "A42315", 6 );
   memcpy( HOST_MESSAGE.responsecd, "000",  3 );
  memcpy(HOST_MESSAGE.responsecd,RespCodeinTxt,3 );

   /* Determine transaction type */
   memset( msgtype, 0x00, sizeof(msgtype) );
   memcpy( msgtype, HOST_MESSAGE.msgtype, 4 );

   /* 1100 */
   if ( HOST_MESSAGE.msgtype[0] == 0x31  && HOST_MESSAGE.msgtype[1] == 0x31 &&
        HOST_MESSAGE.msgtype[2] == 0x30  && HOST_MESSAGE.msgtype[3] == 0x30 )
      tx_key = AUTH_AUTHORIZATION_RESPONSE;

   /* 1120 */
   else if ( HOST_MESSAGE.msgtype[0] == 0x31  && HOST_MESSAGE.msgtype[1] == 0x31 &&
             HOST_MESSAGE.msgtype[2] == 0x32  && HOST_MESSAGE.msgtype[3] == 0x30 )
      tx_key = AUTH_ADVICE_RESPONSE;

   /* 1200 */
   else if ( HOST_MESSAGE.msgtype[0] == 0x31  && HOST_MESSAGE.msgtype[1] == 0x32 &&
             HOST_MESSAGE.msgtype[2] == 0x30  && HOST_MESSAGE.msgtype[3] == 0x30 )
      tx_key = AUTH_SALE_CASH_RESPONSE;

   /* 1304 */
   else if ( HOST_MESSAGE.msgtype[0] == 0x31  && HOST_MESSAGE.msgtype[1] == 0x33 &&
             HOST_MESSAGE.msgtype[2] == 0x30  && HOST_MESSAGE.msgtype[3] == 0x34 )
   {
      tx_key = AUTH_CARD_UPDATE_RESPONSE;
      //if ( HOST_MESSAGE.pan[0] == '3' )
      //   /* Approved */
      //   memcpy( HOST_MESSAGE.responsecd, "300", 3 );
      //else if ( HOST_MESSAGE.pan[0] == '4' )
      //   /* Duplicate record, old record replaced */
      //   memcpy( HOST_MESSAGE.responsecd, "303", 3 );
      //else if ( HOST_MESSAGE.pan[0] == '5' )
      //   /* File update not accepted; higher priority item */
      //   memcpy( HOST_MESSAGE.responsecd, "380", 3 );
      //else
      //   /* Unable to locate record on file */
      //   memcpy( HOST_MESSAGE.responsecd, "302", 3 );
   }

   /* 1420 */
   else if ( HOST_MESSAGE.msgtype[0] == 0x31  && HOST_MESSAGE.msgtype[1] == 0x34 &&
             HOST_MESSAGE.msgtype[2] == 0x32  && HOST_MESSAGE.msgtype[3] == 0x30 )
      tx_key = AUTH_REVERSAL_RESPONSE;

   /* 1804 */
   else if ( HOST_MESSAGE.msgtype[0] == 0x31  && HOST_MESSAGE.msgtype[1] == 0x38 &&
             HOST_MESSAGE.msgtype[2] == 0x30  && HOST_MESSAGE.msgtype[3] == 0x34 )
      tx_key = AUTH_LOGON_RESPONSE;

   else
      tx_key = 0;
}


/*****************************************************************************

  Function:    parse_host_msg

  Description: This function will parse a host response message
  Author: 
      unknown
  Inputs:
      p_auth_tx - a pointer to the auth_tx structure
      ReqRsp_Buffer (global) - the buffer that contains the ISO formatted message
  Outputs:
      HOST_MESSAGE (global)  - structure that contains data to go into the 
                               ISO message
  Return values:
      true  - no error occurred
      false - an error occurred
  Modified by:
      FArad      8/13/98
******************************************************************************/

int parse_host_msg(LPBYTE p_msg)
{
   //struct trandef       *pTranDef;
   struct bitfield_data *pBF;
   int                  bfidx;         /* index to bitfields for this transaction */
   int                  i;

   memset(&HOST_MESSAGE, 0x00, sizeof(HOST_MESSAGE));
   

   for( i=0; i<4; i++ )
   {
      HOST_MESSAGE.msgtype[i] = ascii_ebcdic_conv (p_msg[i], E2A);
   }

   bitmapidx = 4;       /* point to bitmap */

   /* Point past bitmap, but 1st check for extended bitmap. */
   if ( p_msg[bitmapidx]  &  BITTAB[bf_1xbitfield].bit_mask )
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
      if (p_msg[bitmapidx+BITTAB[bfidx].byte_no] & BITTAB[bfidx].bit_mask)
      {
         /* Bit is on, field is there.... */
         pBF = Find_Bitfield_Table(bfidx); 
         if (pBF != NULL)       
         {
            /*---------------------------------------*/
            /* call the Response Handling routine... */
            /*---------------------------------------*/
	   
            (pBF->bd_BFRsp)(pBF->bd_fieldid);
         }
         else
         {
            return(false);
         }
      }
   }
   return(true);
} /* parse_host_msg */




//****************************************************************************
//***
//*** FUNCTION:   host_notify
//*** PURPOSE:    To initiate processing of an incoming
//***               request to the Host. This function will
//***               cause a response to be generated and sent.
//*** 
//*** ARGUMENTS:  p_req = A pointer to the complete message,
//***                 i.e. the Host Request.
//*** 
//***             len    = The length of the complete 
//***                 message in bytes.
//*** 
//*** NOTES:      This function has to initiate the processing
//***          cycle for incoming requests.
//*** 
//*** 
//****************************************************************************
void host_notify( LPBYTE p_req, WORD len, pIPC_INFO p_ipc_info )
{
   long field_error = 0;     // Error field, if any
   int  write_status; 
   /*char field_message[64];*/      // Error message, if any

 //  		GetPrivateProfileString1(
	//	"RESPONSE_CODE",	            /*  points to section name  */
	//	"AMEXRC",                   /*  points to key name  */
	//	"0",                     /*  points to default string  */
	//	valuefromRCfile,                         /*  points to destination buffer  */
	//	sizeof(valuefromRCfile) - 1,             /*  size of destination buffer  */
	//	filename                        /*  points to initialization filename  */
	//   );

 //  	strncpy(RespCodeinTxt,valuefromRCfile,3);
	//sprintf( global_err_str, "Value of Amex Response Code is %s", RespCodeinTxt);
	//AfxMessageBox(global_err_str,MB_ICONINFORMATION | MB_OK);

   if (priority_set == false)
   {
      SetThreadPriority (GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
      priority_set = true;
   }

   //init_trace();

   //MessageBox( NULL, "Incoming request received", NULL, MB_OK );
   //write_to_comm_trace_file( p_req, len, '0' );

   memcpy( ReqRsp_Buffer, p_req, len );

   // Initialize
   if (g_using_tpdu_prefix == true)
   {
      // Save TPDU for later use
      memcpy (save_tpdu, p_req, sizeof(save_tpdu));
   }

   // Parse the incoming message
   if (false == parse_host_msg(p_req))
   {
      /* Prevents sending response to host (can't send a response to
       * a request we don't understand).
       */
      printf("ERROR: Unable to parse message\n");
      return;
   }

   update_response();

   // Now generate a response message
   response_buffer_length = generate_response ();

   //MessageBox( NULL, "Request Processed", NULL, MB_OK );
   // Now write the response out to the network
   tcp_write(response_buffer_length, host_resp_buffer);

   if (response_buffer_length > 0) 
   {
      //MessageBox( NULL, "Sending Response", NULL, MB_OK );
      write_to_comm_trace_file( host_resp_buffer, response_buffer_length, '1' );
      write_status = port_write((WORD)buflen, ReqRsp_Buffer);
      
      if (!write_status)
         connection_up = FALSE;
   }
}  // end host_notify



