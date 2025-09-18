/*******************************************************************************

  Module:      response.c

  Title:    Host Response Definition File for Visa

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

   REVISION HISTORY
   ----------------

   Rev 1.24  Feb 08 2008  dirby
      Updated version to 1.2.0.7
      Added response code to 0810 responses; removed field 93 from same.
      Added DE62.3 for MasterCard transactions.
      Added DE116 for MasterCard and American Express transactions.
   SCR 25045

      Rev 1.23   Apr 03 2006 09:47:02   dirby
   Added 5-digit auth codes for amounts 105 and 106.
   
      Rev 1.22   Dec 20 2005 13:14:40   dirby
   Modified to handle 0302/0312 Visa Online File Maintenance transactions.
   SCR 18962
   
      Rev 1.21   Apr 13 2005 16:37:32   dirby
   Added 2nd bitmap to responses for reversals.
   
      Rev 1.20   Feb 09 2005 15:50:50   dirby
   Added card sequence number (field 23) to the host structure.
   This is for EMV.
   
      Rev 1.19   Jan 10 2005 11:51:42   dirby
   Modified to accept 3rd bitmap; it will be ignored.
   
      Rev 1.18   Jul 19 2004 11:03:18   dirby
   Removed code to echo msg back if amount is too high.  Modified
   code that deals with amount to use 'double' instead of 'long'.  This
   is so that very large amounts can be handled properly.
   SCR 1386
   
      Rev 1.17   Dec 09 2003 13:43:06   dirby
   Fixed bugs in updating response information for private field 126 CAVV data.
   SCR 1228
   
      Rev 1.16   Dec 05 2003 14:43:16   dirby
   Added support in fields 126 and 44 for VbV.
   SCR 1228
   
      Rev 1.15   Feb 27 2001 11:32:06   dirby
   Had several items in EBCDIC.  Changed them to ASCII.
   
      Rev 1.14   Feb 16 2001 16:22:38   dirby
   Added code to create various responses to CVV2 data in requests.
   
      Rev 1.13   Jan 03 2001 17:17:54   dirby
   Added support for reversal repeats.
   Added code to ensure logons get an extended bitmap.
   
      Rev 1.12   Jun 08 2000 14:19:32   dirby
   Made fields 62, 102, 103, and 121 so they are in the response
   only if they are in the request.  Also made field 62 so the first value
   in the bitmap is 'C0'.
   
      Rev 1.11   May 01 2000 10:14:50   dirby
   Modified the visa transaction id so it is right justified.
   
      Rev 1.10   May 01 2000 09:30:06   dirby
   Added visa_transaction_id as part of field 62's response.
   
      Rev 1.9   Apr 21 2000 11:27:18   dirby
   Corrected the check on the dollar amount to bo 100,000.00 to
   echo a message back.  Also, removed the code that changed
   the message type.  Now if a transaction comes in over $100000
   the message gets echoed back just when it came in.
   
      Rev 1.8   Apr 20 2000 16:44:42   dirby
   Added code to echo a request back to the originator.
   Only the message type is modified.  This is a special
   case to test an issue with DBS Card Centre.
   
      Rev 1.7   Apr 19 2000 11:02:34   dirby
   Removed some fields in the response that DBS cannot handle.
   
      Rev 1.6   Apr 19 2000 10:50:04   dirby
   Removed field 20 from any transaction type that had it.
   Did this because DBS Visa does not handle this field.
   
      Rev 1.5   Apr 18 2000 15:54:58   dirby
   Put code for field 62 back in.  It will always respond with
   an 'A', which means transaction qualified for a discount.
   
      Rev 1.4   Apr 17 2000 11:43:20   dirby
   Remove code that populates fields for responses.
   Some of them are not used.  Removed field 62 so
   it will get echoed back to the originator.
   
      Rev 1.3   Mar 27 2000 14:53:36   dirby
   Modified code to calculate the total message length for the
   Visa header.  Before, it was just echoing it.
   
      Rev 1.2   Jan 10 2000 13:29:30   dirby
   Increased message buffer size from 200 to 1000 bytes.
   
      Rev 1.1   Dec 09 1999 15:09:34   dirby
   Added functionality for reversals and logons.
   
      Rev 1.0   May 11 1999 11:55:56   rmalathk
   Initial release
   
      Rev 1.0   Oct 14 1998 14:55:40   drodden
   Initial checkin of the ISO Retail host simulator.
   
   
      Rev 1.3   Apr 09 1998 15:14:52   IGOLDFIE
   Added changes regarding REGISTRO transaction:
   structure REGISTRO_response, changes to parse_host_request,
   generate_response, make_63, case 12.
   
      Rev 1.2   09 Apr 1998 13:07:14   MSALEH
   Modified some files to use the new 
   ptecom library and header file structures
   
      Rev 1.1   18 Mar 1998 13:47:14   MSALEH
   1) Use the new ptecom library
   2) cleanup code and files
   
      Rev 1.0   Nov 11 1997 19:58:00  BROBERTS
      Initial Release


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

#include "ntutils.h"

extern int volatile connection_up;
extern INT init_trace();
extern void write_to_comm_trace_file( pBYTE bufptr, INT len, BYTE msg_type );
char RespCodeinTxt[4] = {0};
char AuthCodeinTxt[7] = {0};
char RRNinTxt[12] = {0};

int    g_using_tpdu_prefix = false;      // Whether the TPDU is present or not


/**********************************
      FUNCTION PROTOTYPES
**********************************/
/* functions local */
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
BYTE ascii_ebcdic_conv(BYTE conv_char, pCHAR conv_type );

//farad 8/13/98 
long bcd_to_long( pBYTE bcd_data, int bcd_len );
void asc_to_vbcd (pCHAR, INT, BYTE [] );
void asc_to_bcd (pCHAR, INT, BYTE [] );
void str_asc_to_bcd(pCHAR, INT, BYTE [] ); 
void vBCDMove(enum bitfields fieldid);
void RspMovevBCD (enum bitfields fieldid);
void GenericMove(enum bitfields fieldid);
void RspMoveGeneric(enum bitfields fieldid);
void vDataMove(enum bitfields fieldid);
void RspMovevData (enum bitfields fieldid);



/**********************************
     VARIABLES
**********************************/
/* local variables */
INT    reqidx = 0;                  /* next open position in request buffer */
INT    bitmapidx = 0;               /* offset of bitmap... */
INT    RespCodeFlag = 1;


/*******************************************************/
/* REQUEST BUILD BUFFER                                */
/*******************************************************/
BYTE ReqRsp_Buffer [1000];
BYTE visa_header[22];
char error_msg[100];
static WORD    response_buffer_length;     // Length of the response
static BYTE    host_resp_buffer [5500];    // Response Buffer
static BYTE    save_tpdu[5];               // TPDU bytes, if present
static BOOLEAN priority_set = false;
BYTE tx_key;
INT  CAVV_Data_Exists;

//static   BYTE ReqRsp_Buffer  [COMM_BUF_SZE+50];

//farad 8/10/1998 added becuase it did not exist before.
#define  VISA_HDR_SIZE   22
#define  REJECT_VISA_HDR_SIZE 26

char  A2E[4] = "A2E";    /*  ASCII to EBCDIC conversion */
char  E2A[4] = "E2A";    /*  EBCDIC to ASCII conversion */

/******************************************************/
/* DATA STRUCTURE FOR STORING DATA IN FINAL FORMAT... */
/******************************************************/
struct iso_msg_rec HOST_MESSAGE;



//farad 8/10/1998 Added the totals_record sturcture, becuase it did not exist in mcrc.

static  char proccode_str [7];


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
//   topt2_org_proccode = 128     /* use msg's original proccode */
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
   bf_28tran_fee     = 28,     /* Transaction fee amount */    /* TF- Ajay Added */
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
   bf_100rcvid       = 100,    /* receiving inst id code   */
   bf_101fname       = 101,    /* file name                */
   bf_102acct1       = 102,    /* acct id 1                */
   bf_103acct2       = 103,    /* acct id 2                */
   bf_104trandesc    = 104,    /* transaction description  */
   bf_116issref      = 116,    /* issuer reference data    */
   bf_118intractry   = 118,    /* intra country data       */
   bf_120orgmsgid    = 120,    /* original message type id */
   bf_121issid       = 121,    /* issuing inst id code     */
   bf_122openuse     = 122,    /* remaining open-to-use    */
   bf_123addverf     = 123,    /* address verification data*/
   bf_124freeform    = 124,    /* free form text - Japan   */
   bf_126privuse     = 126,    /* Private Use - CVV2       */
   bf_127filerecs    = 127     /* free form text - Japan   */
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
#define bf126_len     1
#define bf127_len     3



#define EXTENDED_BITFIELD 128
#define NORMAL_BITFIELD    64
int     BitFieldSize = NORMAL_BITFIELD;


struct trandef
{
   BYTE   td_trantype;  /* tran type for this record */
//farad 8/10/1998.
//   BYTE   td_msgid[4];   /* iso 8583 message id       */
//   BYTE   td_proccode[6]; /* iso 8583 processing code  */
   BYTE   td_msgid[2];   /* iso 8583 message id       */
   BYTE   td_proccode[3]; /* iso 8583 processing code  */
   enum	  tranopts1  td_opts1;   /* options */
   enum   tranopts2  td_opts2;
   enum   bitfields  td_bitfields[EXTENDED_BITFIELD]; /* fields to put in message */
};

static  struct trandef TRANTABLE[] = {

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
      bf_2pan,       /* MAND */
      bf_3proc,      /* MAND */
      bf_4amount,    /* MAND */
      bf_7trntime,   /* MAND */
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
      bf_60priv60,   /* MAND */
      bf_126privuse, /* COND */
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
      bf_6crdbillamt,/* COND */
      bf_7trntime,   /* MAND */
      bf_10convrate, /* COND */
      bf_11stan,	   /* MAND */
      bf_19ctrycd,   /* MAND */
      bf_25pocc,     /* MAND */
      bf_32idcode,   /* MAND */
      bf_34panext,   /* COND */
      bf_37rrn,      /* MAND */
      bf_38authcode, /* COND */
      bf_39respcode, /* MAND */
      bf_41termid,	/* COND */
      bf_42cardacc,	/* MAND */
      bf_44respdata, /* COND */
      bf_49trncurcd, /* MAND */
      bf_51crdcurcd, /* COND */
	  bf_55ICCdata,
      bf_62priv62,   /* COND */
      bf_63priv63,   /* COND */
      bf_116issref,  /* COND */
      bf_done      /* end of fields to move */
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
      bf_25pocc,	    /* MAND */
      bf_32idcode,    /* MAND */
      bf_34panext,    /* COND */
      bf_37rrn,       /* MAND */
      bf_39respcode,  /* MAND */
      bf_41termid,    /* COND */
      bf_42cardacc,   /* MAND */
      bf_48addl_data, /* OPT  */
      bf_49trncurcd,  /* MAND */
	  bf_55ICCdata,
      bf_62priv62,    /* OPT  */
      bf_63priv63,   /* COND */
      bf_90orgdata,   /* OPT  */
      bf_95replamts,  /* COND */
      bf_done      /* end of fields to move */
      },
      {
      /****************************************************
         R E V E R S A L    R E P E A T    R E S P O N S E
      *****************************************************/
      AUTH_REVERSAL_REPEAT_RESPONSE,
      0x04,0x11,              /* message id */
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
      bf_25pocc,	    /* MAND */
      bf_32idcode,    /* MAND */
      bf_34panext,    /* COND */
      bf_37rrn,       /* MAND */
      bf_39respcode,  /* MAND */
      bf_41termid,    /* COND */
      bf_42cardacc,   /* MAND */
      bf_48addl_data, /* OPT  */
      bf_49trncurcd,  /* MAND */
      bf_62priv62,    /* OPT  */
      bf_63priv63,   /* COND */
      bf_90orgdata,   /* OPT  */
      bf_95replamts,  /* COND */
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
      bf_done      /* end of fields to move */
      },
	  {
      /********************************************
         R E V E R S A L  A D V I C E   R E S P O N S E
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
      bf_25pocc,	    /* MAND */
      bf_32idcode,    /* MAND */
      bf_34panext,    /* COND */
      bf_37rrn,       /* MAND */
	  bf_38authcode, /* COND */
      bf_39respcode,  /* MAND */
      bf_41termid,    /* COND */
      bf_42cardacc,   /* MAND */
      bf_48addl_data, /* OPT  */
      bf_49trncurcd,  /* MAND */
      bf_54addlamts,    /* OPT  */
      //bf_63priv63,   /* COND */
      //bf_90orgdata,   /* OPT  */
      //bf_95replamts,  /* COND */
      bf_done      /* end of fields to move */
      },
	  {
	  /********************************************
         R E V E R S A L  A D V I C E   R E P E A T  R E S P O N S E
      ********************************************/
      AUTH_REVERSAL_ADVICE_REPEAT_RESPONSE,
      0x04,0x31,              /* message id */
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
      bf_25pocc,	    /* MAND */
      bf_32idcode,    /* MAND */
      bf_34panext,    /* COND */
      bf_37rrn,       /* MAND */
	  bf_38authcode, /* COND */
      bf_39respcode,  /* MAND */
      bf_41termid,    /* COND */
      bf_42cardacc,   /* MAND */
      bf_48addl_data, /* OPT  */
      bf_49trncurcd,  /* MAND */
      bf_54addlamts,    /* OPT  */
      //bf_63priv63,   /* COND */
      //bf_90orgdata,   /* OPT  */
      //bf_95replamts,  /* COND */
      bf_done      /* end of fields to move */
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
      bf_11stan,    /* MAND */
      bf_37rrn,     /* MAND */
      bf_39respcode,/* MAND */
      bf_70nmicode, /* MAND */
      bf_done       /* end of fields to move */
      },
      { 
      /***********************************************************
         O N L I N E   F I L E   U P D A T E   R E S P O N S E
      ************************************************************/
      AUTH_CARD_UPDATE_RESPONSE,
      0x03,0x12,              /* message id */
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
      bf_39respcode,   /* MAND */
      bf_48addl_data,  /* COND */
      bf_73action,     /* MAND */
      bf_91fileupdate, /* MAND */
      bf_101fname,     /* MAND */
      bf_127filerecs,  /* MAND */
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
   Move_IfThere,                 /* conditional */
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
   Move_IfThere,                 /* conditional */
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
   /* Apr 2009 Mandates, Ajay TF, March 25, 2009 */
   {
   bf_28tran_fee,				 /* field */
   HOST_MESSAGE.tran_fee,	     /* data to move */
   9,							 /* # of bytes in visa iso field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
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
   bf_33fwdidcode,               /* field */
   HOST_MESSAGE.fwd_inst_len,    /* data to move */
   1,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   vBCDMove,                     /* move routine */
   RspMovevBCD                   /* response routine */
   },
   {
   bf_34panext,                    /* field */
   HOST_MESSAGE.pan_ext_len,    /* data to move */
   1,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   vBCDMove,                     /* move routine */
   RspMovevBCD                   /* response routine */
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
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_42cardacc,                 /* field */
   HOST_MESSAGE.cardaccid,       /* data to move */
   15,                           /* # of bytes in visa iso field */
   Move_Always,                  /* conditional */
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
   vBCDMove,                     /* move routine */
   RspMovevBCD                   /* response routine */
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
   Move_Always,                  /* mandatory    */
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
   Move_Always,                  /* conditional */
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
   Move_IfThere,                 /* conditional */
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
   HOST_MESSAGE.addr_verf_len,   /* data to move */
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
   bf_126privuse,                /* field */
   HOST_MESSAGE.priv126_len,     /* data to move */
   1,                            /* # of bytes in DE126 len field */
   Move_IfThere,                 /* conditional */
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
struct bitfield_data *Find_Bitfield_Table (enum bitfields);
struct trandef       *Find_Tran_Table     (BYTE trantype);



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

//****************************************************************************
//***
//*** FUNCTION:   bcd_to_long
//*** PURPOSE:    To convert a BINARY CODED DECIMAL string
//***               into an appropriate long integer. This
//***               will handle up to 999,999,999.
//*** 
//*** ARGUMENTS:  bcd_data  = Points to the BCD string 
//***                    that you want to convert.
//***                           This should be BCD only,
//***                         e.g. 0x12 0x34 for 1234.
//*** 
//***             bcd_len   = The length of the string;  
//***                         this controls processing
//*** 
//*** RETURN VAL: result    = The Long Integer equivalent 
//***                           of the BCD value.              
//*** 
//*** 
//****************************************************************************
long bcd_to_long( pBYTE bcd_data, int bcd_len )
{
   long result = 0;    // Long integer equivalent
   long multiplier = 1;   // How much to multiply by
   long i;              // Working variable

   // Check arguments
   if ((bcd_data) && (bcd_len > 0) && (bcd_len < 10))
   {
      // Parse the string from RIGHT to LEFT
      for (i = (bcd_len-1); i >= 0; i--)
      {
         // Add the RIGHT nibble digit
         result+= (bcd_data [i] & 0x0f) * multiplier;
         multiplier = multiplier * 10;

         // Add the LEFT nibble digit
         result+= (bcd_data [i] / 16) * multiplier;
         multiplier = multiplier * 10;
       }
   }

   return (result);
}

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
      pBF->p_bd_pdata[0] = ReqRsp_Buffer[reqidx];
      reqidx++;

      /* Move the data, converting it from EBCDIC to ASCII. */
      for( i=1; i<=movelen; i++)
      {
         pBF->p_bd_pdata [i] = ascii_ebcdic_conv (ReqRsp_Buffer[reqidx], E2A);
         reqidx++;
      }
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
      for( i=0; i<movelen; i++)
      {
         pBF->p_bd_pdata [i] = ascii_ebcdic_conv (ReqRsp_Buffer[reqidx], E2A);
         reqidx++;
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

      if (( 60 == fieldid ) || ( 61  == fieldid ) ||
          ( 48 == fieldid ) || ( 120 == fieldid )  )
         movelen *= 2;

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

      if (( 60 == fieldid ) || ( 61 == fieldid ) || ( 120 == fieldid ))
         movelen *= 2;

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
      LogEvent( temp_str, ERROR_MSG );
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
  Modified by:
      FArad      8/13/98
******************************************************************************/
static  BYTE ascii_ebcdic_conv( unsigned char conv_char, char *conv_type )
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





//****************************************************************************
//*** 
//***   FUNCTION: generate_response
//*** 
//*** PURPOSE:    To create a response message that is appropriate. This
//***               follows the design document definitions for each 
//***               transaction.
//*** 
//*** ARGUMENTS:  iso_response = Points to the message prototype data area
//***                      This includes all valid data for output
//*** 
//***             host_resp_buffer  = Response data buffer area. This is the 
//***                             network message buffer for output.
//*** 
//*** RETURN VAL: response_len = The number of bytes generated in the buffer
//*** 
//*** MODIFICATIONS:
//***     11/30/98 - DIRBY - Modified to be specific to MasterCard
//***
//****************************************************************************
WORD generate_response ()
{

   struct trandef       *pTranDef;
   struct bitfield_data *pBF;
   INT                  bfidx;         /* index to bitfields for this transaction */
   INT                  response_len = 0;
   INT                  total_msg_len;
   BYTE                 cMsgLen[2];

   pTranDef = Find_Tran_Table(tx_key);   /* get pointer to tran table */
   if (pTranDef != NULL)
   {    
      /*-----------------------------------------*/
      /* Copy in Message Id for this transaction */
      /*-----------------------------------------*/
      memset( ReqRsp_Buffer, 0, sizeof(ReqRsp_Buffer) );   /* zero out the buffer */
      bitmapidx = 2;            /* point to bitmap     */

      /* point past bitmap   */
      if ( (tx_key == AUTH_LOGON) || (tx_key == AUTH_LOGON_RESPONSE) )
      {
         reqidx = 18;
         ReqRsp_Buffer[bitmapidx] |= BITTAB[bf_1xbitfield].bit_mask;   /* 2 bitmaps */
         BitFieldSize = EXTENDED_BITFIELD;
      }
      else if ((0 != strlen(HOST_MESSAGE.acct_id_1)) || (0 != strlen(HOST_MESSAGE.acct_id_2)) ||
         (0 != strlen(HOST_MESSAGE.iss_inst_id)))  
      {
         reqidx = 18;
         ReqRsp_Buffer[bitmapidx] |= BITTAB[bf_1xbitfield].bit_mask;   /* 2 bitmaps */
         BitFieldSize = EXTENDED_BITFIELD;
      }
      else if ( (tx_key == AUTH_REVERSAL) || (tx_key == AUTH_REVERSAL_RESPONSE) )
      {
         reqidx = 18;
         ReqRsp_Buffer[bitmapidx] |= BITTAB[bf_1xbitfield].bit_mask;   /* 2 bitmaps */
         BitFieldSize = EXTENDED_BITFIELD;
      }
      else if ( (tx_key == AUTH_CARD_UPDATE) || (tx_key == AUTH_CARD_UPDATE_RESPONSE) )
      {
         reqidx = 18;
         ReqRsp_Buffer[bitmapidx] |= BITTAB[bf_1xbitfield].bit_mask;   /* 2 bitmaps */
         BitFieldSize = EXTENDED_BITFIELD;
      }
      else if ( HOST_MESSAGE.issref_len[0] != 0x00 )
      {
         reqidx = 18;
         ReqRsp_Buffer[bitmapidx] |= BITTAB[bf_1xbitfield].bit_mask;   /* 2 bitmaps */
         BitFieldSize = EXTENDED_BITFIELD;
      }
      else 
      {
         reqidx = 10;
         BitFieldSize = NORMAL_BITFIELD;
      }
   
      memcpy(ReqRsp_Buffer, pTranDef->td_msgid, sizeof(pTranDef->td_msgid));

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
   
      // Insert the TPDU if appropriate;
      if (g_using_tpdu_prefix == true)
      {
         host_resp_buffer [0] = save_tpdu [0];
         host_resp_buffer [1] = save_tpdu [3];
         host_resp_buffer [2] = save_tpdu [4];
         host_resp_buffer [3] = save_tpdu [1];
         host_resp_buffer [4] = save_tpdu [2];
         response_len   = sizeof(save_tpdu);
      }
      else 
         response_len = 0;

      // visa header   
      total_msg_len = response_len+VISA_HDR_SIZE+reqidx;
      cMsgLen[0] = ((total_msg_len) & 0xff00)/256;
      cMsgLen[1] =  (total_msg_len) & 0x00ff;
      memcpy( &visa_header[3], cMsgLen, 2 );
      memcpy( &host_resp_buffer[response_len], visa_header, VISA_HDR_SIZE );

      // copy the rest of the message
      memcpy( &host_resp_buffer[response_len+VISA_HDR_SIZE], ReqRsp_Buffer, reqidx );

      return (response_len+VISA_HDR_SIZE+reqidx);
   }
   else
      return 0;

}  // end generate_response





//****************************************************************************
//***
//*** FUNCTION:   parse_host_request
//*** PURPOSE:    To examine the incoming request. This
//***               determines the value of certain fields.
//***               It always returns.
//*** 
//*** ARGUMENTS:  p_msg    = A pointer to the complete 
//***                   Host Request message.
//*** 
//***             length_of_msg = The length of the message
//*** 
//*** RETURN VAL: 0  = Request was correctly processed
//***              >0  = Request is formatted INCORRECTLY
//*** 
//*** NOTES:      This function should be modified to get
//***          additional data from incoming requests.
//*** 
//*** MODIFICATIONS:
//*** 
//****************************************************************************
int parse_host_request()
{
   //struct trandef       *pTranDef;
   struct bitfield_data *pBF;
   INT                  bfidx;         /* index to bitfields for this transaction */

   memset(&HOST_MESSAGE, 0x00, sizeof(HOST_MESSAGE));

   if (0x16 == ReqRsp_Buffer[0])
   {
	   memcpy(HOST_MESSAGE.msgtype_bcd, &ReqRsp_Buffer[VISA_HDR_SIZE],
					sizeof(HOST_MESSAGE.msgtype_bcd));

      memcpy( visa_header, ReqRsp_Buffer, VISA_HDR_SIZE );
      bitmapidx = 2 + VISA_HDR_SIZE;            /* point to bitmap     */
   }
   else
   {
      sprintf( error_msg, "Invalid visa header" );
	   return(false);
   }

   // check if it is a '0100', if not reject it
   if( (ReqRsp_Buffer[22] == 0x01) && (ReqRsp_Buffer[23] == 0x00) )
   {
      tx_key = AUTH_AUTHORIZATION_RESPONSE;
   }
   else if( (ReqRsp_Buffer[22] == 0x04) && (ReqRsp_Buffer[23] == 0x00) )
   {
      tx_key = AUTH_REVERSAL_RESPONSE;
   }
   else if( (ReqRsp_Buffer[22] == 0x04) && (ReqRsp_Buffer[23] == 0x01) )
   {
      tx_key = AUTH_REVERSAL_REPEAT_RESPONSE;
   }
   else if( (ReqRsp_Buffer[22] == 0x04) && (ReqRsp_Buffer[23] == 0x20) )
   {
      tx_key = AUTH_REVERSAL_ADVICE_RESPONSE;
   }
   else if( (ReqRsp_Buffer[22] == 0x04) && (ReqRsp_Buffer[23] == 0x21) )
   {
      tx_key = AUTH_REVERSAL_ADVICE_REPEAT_RESPONSE;
   }
   else if( (ReqRsp_Buffer[22] == 0x01) && (ReqRsp_Buffer[23] == 0x20) )
   {
      tx_key = AUTH_ADVICE_RESPONSE;
   }
   else if( (ReqRsp_Buffer[22] == 0x08) && (ReqRsp_Buffer[23] == 0x00) )
   {
      tx_key = AUTH_LOGON_RESPONSE;
   }
   else if( (ReqRsp_Buffer[22] == 0x03) && (ReqRsp_Buffer[23] == 0x02) )
   {
      tx_key = AUTH_CARD_UPDATE_RESPONSE;
   }
   else
   {
      sprintf( error_msg, "Unsupported Msg ID: %x%x",
               ReqRsp_Buffer[22], ReqRsp_Buffer[23] );
      return false;
   }

   /* Point past bitmap, but 1st check for extended bitmap. */
   if ( ReqRsp_Buffer[bitmapidx]  &  BITTAB[bf_1xbitfield].bit_mask )
   {
      if ( ReqRsp_Buffer[bitmapidx+8]  &  0x80 ) /* Check bit 65 */
      {
         /* 3 bitmaps */
         reqidx = 26 + VISA_HDR_SIZE;      /* Data starts after 3rd bitmap  */
         BitFieldSize = EXTENDED_BITFIELD; /* Ignore 3rd bitmap data fields */
      }
      else
      {
         /* 2 bitmaps */
         reqidx = 18 + VISA_HDR_SIZE;         /* Extended     */
         BitFieldSize = EXTENDED_BITFIELD;
      }
   }
   else
   {
      reqidx = 10 + VISA_HDR_SIZE;         /* Not Extended */
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

         /*---------------------------------------*/
         /* call the Response Handling routine... */
         /*---------------------------------------*/
	
         if (pBF != NULL)
         {
            (pBF->bd_BFRsp)(pBF->bd_fieldid);
         }
      }
   }

   return(true);
}  // end parse_host_request





//****************************************************************************
//***
//*** FUNCTION:   update_iso_response
//*** PURPOSE:    To update the ISO Response Message prototype, using
//***               known variables. For example, this sets the date and time.
//*** 
//*** ARGUMENTS:  NONE
//***
//*** GLOBALS:    The following global variables are used herein
//***             iso_request (read only)
//***             iso_response
//*** 
//*** RETURN VAL: NONE
//*** 
//***   METHOD:
//***    Update the response buffer with specific values:
//***       o Increment the Request Response Number and copy it
//***       o System trace number from the request
//***       o Date in MMDD format (i.e. Month and Day of Month)
//***       o Time when the response was generated
//***       o Length of the response buffer contents
//***       o 5-digit auth code for $1.05 or $1.06
//***
//*** MODIFICATIONS:
//***    2/22/07 DJI - Decline all txns when amount is $25 or less
//****************************************************************************
void update_iso_response()
{
   char  temp_str[40];
   char  tran_id[16];
   BYTE  presence_indicator;
   BYTE  response_type;
   BYTE  cvv2[5];
   BYTE  bitmap126[8];
   BYTE  f116[256];
   double amount;
   INT   i,j;
   INT   F126_8  = false;
   INT   F126_9  = false;
   INT   F126_10 = false;
   INT   F126_index = 0;
   INT   F44_len    = 0;
   INT	 emv_len=0;
   if ( tx_key == AUTH_LOGON_RESPONSE )
   {
      /* Alternate logon response code between approval and decline. */
       if ( RespCodeFlag == 1 )
      {
         memset( HOST_MESSAGE.responsecd, '0', sizeof(HOST_MESSAGE.responsecd));
         RespCodeFlag = 0;
      }
      else
      { /* Changed to approved all the time */
         HOST_MESSAGE.responsecd[0] = '0';
         HOST_MESSAGE.responsecd[1] = '0';
         RespCodeFlag = 1;
      } 
	  
   }
   else
   {
      memcpy( HOST_MESSAGE.pan_ctry_cd,
              HOST_MESSAGE.ctry_code,
              sizeof(HOST_MESSAGE.pan_ctry_cd) );

      memset( HOST_MESSAGE.authcode,   '7', sizeof(HOST_MESSAGE.authcode)  );
      memset( HOST_MESSAGE.responsecd, '0', sizeof(HOST_MESSAGE.responsecd));	

      /* 5-digit Auth Code for 1.05 or 1.06 amounts */
      memset( temp_str, 0x00, sizeof(temp_str) );
      bcd_to_asc (HOST_MESSAGE.amount_bin, sizeof(HOST_MESSAGE.amount_bin),
                  temp_str, 13 );
      amount = atof( temp_str );

      if ( amount == 105.0 )
         HOST_MESSAGE.authcode[0] = ' ';
      else if ( amount == 106.0 )
         HOST_MESSAGE.authcode[5] = ' ';

      /* Decline all amounts less than or equal to $25.00 */
      if ( amount <= 2500.0 )
      {
         /* Decline - erase authcode; set Response Code to '05'. */
         memset( HOST_MESSAGE.authcode, 0x00, sizeof(HOST_MESSAGE.authcode) );
         memcpy( HOST_MESSAGE.responsecd, "00", 2 );
      }

      /* Currency Code */
      asc_to_bcd ( "0786", sizeof(HOST_MESSAGE.card_curr_cd)*2,
                  HOST_MESSAGE.card_curr_cd);

      /* FIELD 48: Decline Reasons for 0302 Requests. Set for MC only. */
      if ( (tx_key == AUTH_CARD_UPDATE) || (tx_key == AUTH_CARD_UPDATE_RESPONSE) )
      {
         if ( (HOST_MESSAGE.pan_bin[0] & 0xf0) == 0x50 )
         {
            /* Card number starts with '5' = MasterCard. Decline it. */
            HOST_MESSAGE.addl_data_len[0] = 2;
            HOST_MESSAGE.addl_data[0] = 0x05;
            HOST_MESSAGE.addl_data[1] = 0x67;
            memcpy( HOST_MESSAGE.responsecd, "06", 2 );
         }
      }
	/* DE 55*/
		emv_len= HOST_MESSAGE.ICCdata_len[0];
	  if(emv_len > 0 )
	  {
			// add issuer data
	  }
      /* FIELD 62:  VISA CHAR INDICATOR and Transaction Identifier */
      if ( (HOST_MESSAGE.pan_bin[0] & 0xf0) == 0x50 )
      {
         /* This is a MasterCard txn being processed by Visa.
          * Set the card validation code response: 62.3
          */
         memset( temp_str, 0x00, sizeof(temp_str) );
         memset( temp_str, 0x20, 1 );     /* Set field 3 in bitmap. */
         memset( &temp_str[8], 0xD3, 1 ); /* 'L' in EBCDIC          */
         memset( &temp_str[9], 0x40, 3 ); /*  3 trailing spaces     */
         memcpy( HOST_MESSAGE.priv62, temp_str, 12 );

         HOST_MESSAGE.priv62_len[0] = 12;
      }
      else
      {
         /* Create response data only if request data exists. */
         if ( HOST_MESSAGE.priv62_len[0] != 0x00 )
         {
            memset( temp_str, 0x00, sizeof(temp_str) );
            memset( temp_str, 0xC0, 1 );   /* Set fields 1 and 2 in bitmap. */
            memset( &temp_str[8], 0xc1, 1 );
            memcpy( HOST_MESSAGE.priv62, temp_str, 9 );

            memset( tran_id, 0x00, sizeof(tran_id) );
            asc_to_bcd( "0123456789012345", sizeof(tran_id), tran_id );

            memcpy( &HOST_MESSAGE.priv62[9], tran_id, 8 );
            HOST_MESSAGE.priv62_len[0] = 17;
         }
      }

      /* FIELD 63: VIP Private Field - Set Network ID */
      if ( HOST_MESSAGE.priv63_len[0] != 0x00 )
      {
         HOST_MESSAGE.priv63[0] |= 0x80;  /* Make sure bit one is set. */
         HOST_MESSAGE.priv63[3]  = 0x00;
         HOST_MESSAGE.priv63[4]  = 0x02;  /* Network ID = 0002 */
      }

      /* FIELD 102: ACCOUNT ID 1 */
      if ( HOST_MESSAGE.acct_id_1_len[0] != 0 )
      {
         HOST_MESSAGE.acct_id_1_len[0] = 6;
         memset( HOST_MESSAGE.acct_id_1, '9', HOST_MESSAGE.acct_id_1_len[0]);   
      }

      /* FIELD 103: ACCOUNT ID 1 */
      if ( HOST_MESSAGE.acct_id_2_len[0] != 0 )
      {
         HOST_MESSAGE.acct_id_2_len[0] = 6;
         memset( HOST_MESSAGE.acct_id_2, '8', HOST_MESSAGE.acct_id_2_len[0]);   
      }

      /* FIELD 116: Card Issuer Reference Data (MC and Amex) */
      if ( (HOST_MESSAGE.pan_bin[0] & 0xf0) == 0x50 )
      {
         /* This is a MasterCard txn being processed by Visa. */
         memset( f116, 0x00, sizeof(f116) );
         memset( f116,    0x28, 1 );  /* Field 116 Total Length */
         memset( f116+1,  0x67, 1 );  /* Data Set ID            */
         memset( f116+3,  0x25, 1 );  /* Data Set Length        */
         memset( f116+4,  0x01, 1 );  /* Tag ID = 01            */
         memset( f116+5,  0x03, 1 );  /* Tag Len                */
         memset( f116+6,  0x09, 1 );
         memset( f116+7,  0x00, 1 );
         memset( f116+8,  0x02, 1 );  /* Tag Data (DE22)        */
         memset( f116+9,  0x02, 1 );  /* Tag ID = 02            */
         memset( f116+10, 0x02, 1 );  /* Tag Len                */
         memset( f116+11, 0x07, 1 );
         memset( f116+12, 0x05, 1 );  /* Tag Data (DE26)        */
         memset( f116+13, 0x03, 1 );  /* Tag ID = 03            */
         memset( f116+14, 0x1A, 1 );  /* Tag Len                */
         memset( f116+15, 'A' , 1 );
         memset( f116+16, 'B' , 1 );
         memset( f116+17, 'C' , 1 );
         memset( f116+18, 'D' , 1 );
         memset( f116+19, 'E' , 2 );
         memset( f116+21, 'F' , 1 );
         memset( f116+22, 'G' , 3 );
         memset( f116+25, '0' , 1 );
         memset( f116+26, '1' , 1 );
         memset( f116+27, '4' , 12);
         memset( f116+39, '2' , 2 );

         memcpy( HOST_MESSAGE.issref_len, f116, 41 );
      }
      else if ( (HOST_MESSAGE.pan_bin[0] & 0xf0) == 0x50 )
      {
         /* This is an American Express txn being processed by Visa. */
         memset( f116, 0x00, sizeof(f116) );
         memset( f116,    0x12, 1 );  /* Field 116 Total Length */
         memset( f116+1,  0x66, 1 );
         memset( f116+3,  0x0e, 1 );
         memset( f116+4,  0x01, 1 );
         memset( f116+5,  0x0c, 1 );
         memset( f116+6,  '1' , 1 );
         memset( f116+7,  '2' , 1 );
         memset( f116+8,  '3' , 1 );
         memset( f116+9,  '4' , 1 );
         memset( f116+10, '5' , 1 );
         memset( f116+11, '6' , 1 );
         memset( f116+12, '7' , 1 );
         memset( f116+13, '8' , 1 );
         memset( f116+14, '9' , 1 );
         memset( f116+15, '0' , 1 );
         memset( f116+16, '1' , 1 );
         memset( f116+17, '2' , 1 );

         memcpy( HOST_MESSAGE.issref_len, f116, 18 );
      }

      /* FIELD 121: ISSUING INSTITUTION ID CODE */
      if ( HOST_MESSAGE.iss_inst_len[0] != 0 )
      {
         HOST_MESSAGE.iss_inst_len[0] = 4;
         memset( HOST_MESSAGE.iss_inst_id, '1', HOST_MESSAGE.iss_inst_len[0]);   
      }

      /* FIELD 126: Private Use - CVV2 and/or CAVV */
      CAVV_Data_Exists = false;
      memset( HOST_MESSAGE.addlrspdata, '0', 14 );
      if ( HOST_MESSAGE.priv126_len[0] > 0 )
      {
         /* CVV2 and/or CAVV Data exists.  Get Bitmap. */
         memcpy( bitmap126, HOST_MESSAGE.priv126, 8 );
         if ( 0x01 & bitmap126[0] )
         {
            F126_8 = true;
            F126_index += 20;
            CAVV_Data_Exists = true;
         }

         if ( 0x80 & bitmap126[1] )
         {
            F126_9 = true;
            F126_index += 20;
            CAVV_Data_Exists = true;
         }

         if ( F126_8 && F126_9 )
            HOST_MESSAGE.addlrspdata[14] = '2';
         else if ( F126_8 )
            HOST_MESSAGE.addlrspdata[14] = '3';
         else if ( F126_9 )
            HOST_MESSAGE.addlrspdata[14] = '4';

         if ( F126_8 || F126_9 )
            F44_len = 15;

         if ( 0x40 & bitmap126[1] )
            F126_10 = true;

         if ( F126_10 )
         {
            /* CVV2 Data exists */
            j = 8 + F126_index;
            presence_indicator = ascii_ebcdic_conv( HOST_MESSAGE.priv126[j++],  E2A);
            response_type      = ascii_ebcdic_conv( HOST_MESSAGE.priv126[j++],E2A);

            memset( cvv2, 0x00, sizeof(cvv2) );
            for( i=0; i<4; i++ )
            {
               cvv2[i] = ascii_ebcdic_conv( HOST_MESSAGE.priv126[j+i], E2A );
            }

            memset( temp_str, 0x00, sizeof(temp_str) );
            bcd_to_asc (HOST_MESSAGE.amount_bin, sizeof(HOST_MESSAGE.amount_bin),
                        temp_str, 13 );
            amount = atof( temp_str );

            /* Set results based upon input. */
            if ( response_type == '1' )
            {
               if ( F44_len == 0 )
                  F44_len = 11;
            }
            if ( presence_indicator == '0' )
            {
               HOST_MESSAGE.responsecd[0] = '0';
               HOST_MESSAGE.responsecd[1] = '5';

               if ( response_type == '1' )
                  HOST_MESSAGE.addlrspdata[10] = 'S';
            }
            else if ( presence_indicator == '1' )
            {
               memset( HOST_MESSAGE.responsecd, '0', 2 );
               if ( amount == 1.0 )
               {
                  HOST_MESSAGE.responsecd[0] = 'N';
                  HOST_MESSAGE.responsecd[1] = '7';

                  if ( response_type == '1' )
                     HOST_MESSAGE.addlrspdata[10] = 'N';
               }
               else if ( amount == 2.0 )
               {
                  if ( response_type == '1' )
                     HOST_MESSAGE.addlrspdata[10] = 'P';
               }
               else if ( amount == 3.0 )
               {
                  if ( response_type == '1' )
                     HOST_MESSAGE.addlrspdata[10] = 'U';
               }
               else
               {
                  if ( response_type == '1' )
                     HOST_MESSAGE.addlrspdata[10] = 'M';
               }
            }
            else if ( presence_indicator == '2' )
            {
               memset( HOST_MESSAGE.responsecd, '0', 2 );
               if ( response_type == '1' )
                  HOST_MESSAGE.addlrspdata[10] = 'M';
            }
            else if ( presence_indicator == '9' )
            {
               HOST_MESSAGE.responsecd[0] = '0';
               HOST_MESSAGE.responsecd[1] = '5';
               if ( response_type == '1' )
                  HOST_MESSAGE.addlrspdata[10] = 'S';
            }
            else if ( amount < 10.0 )
            {
               HOST_MESSAGE.responsecd[0] = 'N';
               HOST_MESSAGE.responsecd[1] = '7';

               if ( response_type == '1' )
                  HOST_MESSAGE.addlrspdata[10] = 'N';
            }
            else
            {
               memset( HOST_MESSAGE.responsecd, '0', 2 );
               if ( response_type == '1' )
                  HOST_MESSAGE.addlrspdata[10] = 'M';
            }
         }
      }
      HOST_MESSAGE.addl_rsp_len[0] = F44_len;
   }
   return;
}


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
   long          field_error = 0;     // Error field, if any
   int           write_status; 
   unsigned char temp_msg[1000];
   INT           success = false;

   memset( host_resp_buffer, 0x00, sizeof(host_resp_buffer) );
   if (priority_set == false)
   {
      SetThreadPriority (GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
      priority_set = true;
   }

   init_trace();

   //MessageBox( NULL, "Incoming request received", NULL, MB_OK );
   //write_to_comm_trace_file( p_req, len, '0' );

   memcpy( temp_msg, p_req+20, len );
   memcpy( ReqRsp_Buffer, p_req+20, len );

   /* There will be no response for ATM Confirmation messages. */
   if( (ReqRsp_Buffer[22] == 0x01) && (ReqRsp_Buffer[23] == 0x02) )
      return;

   // Initialize
   if (g_using_tpdu_prefix == true)
   {
      // Save TPDU for later use
      memcpy (save_tpdu, p_req, sizeof(save_tpdu));
   }

   // Parse the incoming message
   success = parse_host_request ();

   if (!success)
   {
      // Report the bad field to the user
      LogEvent( error_msg, ERROR_MSG );
      return;
   }

   // Update the message prototype to provide all information
   update_iso_response ();

   // Now generate a response message
   response_buffer_length = generate_response ();

   
   // Now write the response out to the network
   tcp_write(response_buffer_length, host_resp_buffer);
   if (response_buffer_length > 0) 
   {
      //MessageBox( NULL, host_resp_buffer, NULL, MB_OK );
      write_to_comm_trace_file( host_resp_buffer, response_buffer_length, '1' );
      write_status = port_write(response_buffer_length, host_resp_buffer);
      
      if (!write_status)
         connection_up = FALSE;
   }
}  // end host_notify



