/***************************************************************************
 *
 * Copyright (c) 2005, Hypercom, Inc.
 * All Rights Reserved.
 *
 * MODULE:      NCEQ_BITFIELDS.H
 *  
 * TITLE:       Equitable Network Control Bit Field Definitions
 *  
 * DESCRIPTION: This header file contains a definition of each ISO
 *              bitfield used by this network controller.
 *
 * APPLICATION: Advanced Transaction Processor (ATP)
 *
 * AUTHOR:      S. Yarlagadda
 *
 * REVISION HISTORY:
 *
 * $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\nceqit_ncmlnk_iso_format\nceq_bitfields.h  $
   
      Rev 1.16   Oct 05 2005 12:15:16   dirby
   Changed so that Voids (Sale and Cash) are treated like reversals
   and sent to the host as 0420's.
   SCR 17660
   
      Rev 1.15   Jul 14 2004 14:27:54   dirby
   Added definitions for the Reload transactions.
   SCR 1287
   
      Rev 1.14   Apr 01 2004 15:07:22   dirby
   Modified field 49 to be conditional instead of mandatory.  EB Host
   uses it, but Megalink does not.
   SCR 1352
   
      Rev 1.13   Mar 24 2004 17:07:38   dirby
   Added bitfields 36, 102, 103, and 128 to 0200 messages for
   Megalink; added 36 to 04nn request messages for Megalink.
   SCR 1352
   
      Rev 1.12   Sep 29 2003 13:51:42   dirby
   Added support for Fee Collection and Funds Disbursement for
   Acquirers.
   SCR 1083
   
      Rev 1.11   Aug 28 2003 14:30:22   svelaga
   Fixed bugs/issues with 0600 message.
   
      Rev 1.10   Aug 25 2003 13:47:56   svelaga
   Fixed
   1. Added support for Field 62
   2. No check for valid NMI code for Administrative text message.
   3. Backoffice adjustment and Representment are distinguised based on Field 25 ( pos condition code ) instead of processing code.
   
      Rev 1.9   Aug 05 2003 10:29:08   svelaga
   VISASMS Specification Changes: New Private field 62 and some changes to Private field 63.
   
      Rev 1.8   Jul 31 2003 16:44:06   svelaga
   Visa SMS QA bug fixes.
   
      Rev 1.7   Jul 30 2003 15:24:22   svelaga
   Removed nulls from response message.
   
      Rev 1.6   Jul 30 2003 14:45:50   svelaga
   Administrative message ( 0620 ) message added.
   
      Rev 1.5   Jul 28 2003 08:55:54   svelaga
   Build response procedurtes for Field 44, 63 are updated.
   
      Rev 1.4   Jul 25 2003 17:32:54   svelaga
   SMS projects changes -
   1. Support for BACKOFFICE_ADJUSTMENT, REPRESENTMENT transactions.
   2. Added to parse and build some extra fields like DE48, DE63,
   DE18,DE19 etc...
   
      Rev 1.3   Feb 25 2003 14:01:54   dirby
   Increased length of HOST_MESSAGE.addl_data so it is large
   enough to hold DE48 data for key change for triple length keys.
   SCR 955
   
      Rev 1.2   Aug 09 2002 16:12:38   dirby
   Modified to not send a response to a reversal immediately.
   Instead, send it after we get the original txn from NCF30.  This
   is so the Merchant ID can be populated from the NCF30 record
   into TLF01.
   SCR 871
   
      Rev 1.1   Aug 13 2001 17:36:48   SYARLAGA
   Modified   struct  trandef  TRANTABLE[] 
   
      Rev 1.0   Aug 06 2001 09:48:00   dirby
   Initial revision.
 *
 ****************************************************************************/

#ifndef NCEQ_BITFIELDS_H
#define NCEQ_BITFIELDS_H

#include "nceq_constants.h"

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
   topt2_org_proccode = 128     /* use msg's original proccode */
   };



enum bitfields 
{
   bf_done           = 0,      /* end of list                          */
   bf_1xbitfield     = 1,      /* extended bit field                   */
   bf_2pan           = 2,      /* pan                                  */
   bf_3proc          = 3,      /* processing code                      */
   bf_4amount        = 4,      /* amount                               */
   bf_5settamount    = 5,
   bf_7trntime       = 7,      /* transmission date-time               */
   bf_9settconvrate  = 9,
   bf_11stan         = 11,     /* system trace number                  */
   bf_12time         = 12,     /* time                                 */
   bf_13date         = 13,     /* date, trans                          */
   bf_14ExpDate		  =14,
   bf_15datestl      = 15,     /* settlement date                      */
   bf_16dateconv     = 16,
   bf_18mertype      = 18,     /* merchant type                        */
   bf_19acqinstcode  = 19,	    /* Acquiring istitution ID Code         */
   bf_20countrycode  = 20,
   bf_21fwdinstcntcode = 21,   /* Forwarding institution country code  */
   bf_22posentry     = 22,     /* pos entry mode                       */
   bf_23cardseqnum	 = 23,	   /* card sequence number */
   bf_25poscc        = 25,     /* pos condition code                   */
   bf_28tranfeeamt   = 28,     /* surcharge fee */
   bf_32idcode       = 32,     /* Acquiring institution's ID code      */
   bf_33fwdinstidcode= 33,     /* Forwarding institution ID code */
   bf_35track2       = 35,     /* track 2 data                         */
   bf_36track3       = 36,     /* track 3 data                         */
   bf_37rrn	         = 37,	    /* retrieval ref number */
   bf_38authcode     = 38,     /* auth code                            */
   bf_39respcode     = 39,     /* response code                        */
   bf_41termid       = 41,     /* terminal id                          */
   bf_42cardacc      = 42,     /* card acceptor id                     */
   bf_43cardacceptloc= 43,     /* card acceptor name/location */
   bf_44respdata     = 44,
   bf_45track1       = 45,     /* track 1 data   */
   bf_48addldata     = 48,     /* additional data                      */
   bf_49trncurcd     = 49,     /* transaction currency code            */
   bf_52pinblock     = 52,     /* pin block                            */
   bf_55ICCdata		 = 55,
   bf_60priv60       = 60,     /* private field 60 */
   bf_61priv61       = 61,     /* private field 61 */
   bf_62priv62       = 62,     /* private field 62 */
   bf_63priv63       = 63,     /* private field 63 */
   bf_70nmicode      = 70,     /* network management information code  */
   bf_90orgdata      = 90,     /* original data elements               */
   bf_95repl_amt     = 95,     /* replacement amount                   */
   bf_100rcvid       = 100,    /* Receiving institution identification code */
   bf_102acct1       = 102,    /* account identification 1             */
   bf_103acct2       = 103,    /* account identification 2             */
   bf_104trandesc    = 104,    /* transaction description         */
   bf_115addl_trace	 = 115,		/* Additional trace data */
   bf_126priv126     = 126,    /* DE126 Pvt 126          */
   bf_128msg_authc   = 128,    /* message authentication code          */                        
};

/***************************/
/* Transaction Definitions */
/***************************/
struct trandef
{
   BYTE   td_trantype;           /* tran type for this record */
   BYTE   td_msgid[4];           /* iso 8583 message id       */
   BYTE   td_proccode[6];        /* iso 8583 processing code  */
   enum   tranopts1  td_opts1;   /* options */
   enum   tranopts2  td_opts2;
   enum   bitfields  td_bitfields[EXTENDED_BITFIELD]; /* fields to put in msg */
};


static  struct trandef TRANTABLE[] = {

      { 
      /********************************************
         B A L A N C E   I N Q U I R Y 
      ********************************************/
      AUTH_BALANCE_INQUIRY,
      0xF0,0xF2,0xF0,0xF0,                  /* message id, EBCDIC => 0200 */
      0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,        /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,        /* MAND */
      bf_3proc,	    /* MAND */
      bf_4amount,     /* MAND */
      bf_7trntime,    /* MAND */
      bf_11stan,      /* MAND */
      bf_12time,      /* MAND */
      bf_13date,      /* MAND */
      bf_14ExpDate,
      bf_15datestl,   /* MAND */
      bf_18mertype,
      bf_22posentry,  /* MAND */
      bf_23cardseqnum,
      bf_25poscc,     /* MAND */
      bf_32idcode,    /* MAND */
      bf_35track2,    /* COND */
      bf_36track3,    /* COND */
	  bf_37rrn,	      /* MAND */	/* Girija Y - TF */
      bf_41termid,    /* COND */
      bf_42cardacc,   /* MAND */
      bf_43cardacceptloc,
      bf_44respdata,
	  bf_45track1,		    /* COND */
      bf_48addldata,  /* MAND */
      bf_49trncurcd,  /* MAND */
      bf_52pinblock,  /* MAND */
      bf_55ICCdata,
       bf_60priv60,
	  bf_62priv62,
	  bf_63priv63,	  /* COND */    /* Girija Y - TF */
      bf_102acct1,    /* COND */
      bf_103acct2,    /* COND */
	  bf_126priv126,
      bf_128msg_authc,/* COND */
      bf_done      /* end of fields to move */
      },

      {  
      /***************************************************
         B A L A N C E   I N Q U I R Y   R E S P O N S E
      ****************************************************/
      AUTH_BALANCE_INQUIRY_RESPONSE,
      0xF0,0xF2,0xF1,0xF0,                  /* message id, EBCDIC => 0210 */
      0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,             /* processing code */
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
      bf_23cardseqnum,
      bf_32idcode,   /* MAND */     
	  bf_37rrn,      /* MAND */		/* Girija Y - TF */
      bf_38authcode, /* COND */
      bf_39respcode, /* MAND */
      bf_41termid,   /* COND */
      bf_48addldata, /* MAND */
	  /*bf_49trncurcd,*/  /* COND */
      bf_55ICCdata,
	  /*bf_62priv62,*/	  /* COND */
	  bf_63priv63,	  /* COND */    /* Girija Y - TF */
      bf_102acct1,   /* MAND */  
      bf_103acct2,   /* MAND */      
      bf_done      /* end of fields to move */
      },
	  {
	  /***************************************************
		 A U T H   R E F F U N D  R E Q U E S T
	  ****************************************************/
      AUTH_REFUND,
      0xF0,0xF2,0xF0,0xF0,                  /* message id, EBCDIC => 0200 */
      0xF2,0xF0,0xF0,0xF0,0xF0,0xF0,        /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,        /* MAND */
      bf_3proc,	    /* MAND */
      bf_4amount,     /* MAND */
      bf_7trntime,    /* MAND */
      bf_11stan,      /* MAND */
      bf_12time,      /* MAND */
      bf_13date,      /* MAND */
      bf_14ExpDate,
      bf_15datestl,   /* MAND */
      bf_18mertype,
      bf_22posentry,  /* MAND */
      bf_23cardseqnum,
      bf_25poscc,     /* MAND */
      bf_32idcode,    /* MAND */
      bf_35track2,    /* COND */
      bf_36track3,    /* COND */
	  bf_37rrn,	      /* MAND */	/* Girija Y - TF */
      bf_41termid,    /* COND */
      bf_42cardacc,   /* MAND */
      bf_43cardacceptloc,
      bf_44respdata,
	  bf_45track1,		    /* COND */
      bf_48addldata,  /* MAND */
      bf_49trncurcd,  /* MAND */
      bf_52pinblock,  /* MAND */
      bf_55ICCdata,
       bf_60priv60,
	  bf_62priv62,
	  bf_63priv63,	  /* COND */    /* Girija Y - TF */
      bf_102acct1,    /* COND */
      bf_103acct2,    /* COND */
	  bf_126priv126,
      bf_128msg_authc,/* COND */
      bf_done      /* end of fields to move */
      },

      {
      /***************************************************
        A U T H   R E F F U N D  R E S P O N S E
      ****************************************************/
      AUTH_REFUND_RESPONSE,
      0xF0,0xF2,0xF1,0xF0,                  /* message id, EBCDIC => 0210 */
      0xF2,0xF0,0xF0,0xF0,0xF0,0xF0,             /* processing code */
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
      bf_23cardseqnum,
      bf_32idcode,   /* MAND */
	  bf_37rrn,      /* MAND */		/* Girija Y - TF */
      bf_38authcode, /* COND */
      bf_39respcode, /* MAND */
      bf_41termid,   /* COND */
      bf_48addldata, /* MAND */
      bf_55ICCdata,
	  bf_63priv63,	  /* COND */    /* Girija Y - TF */
      bf_102acct1,   /* MAND */
      bf_103acct2,   /* MAND */
      bf_done      /* end of fields to move */
      },
	  {
		  /***************************************************
			A U T H    P R E - A U T H   C A N C E L  R E Q U E S T
		  ****************************************************/
			AUTH_PRE_AUTHORIZATION_CANCLE,
	        0xF0,0xF2,0xF0,0xF0,                  /* message id, EBCDIC => 0200 */
	        0xF2,0xF0,0xF0,0xF0,0xF0,0xF0,        /* processing code */
	        /**** OPTIONS ****/
	        topt1_postadv,
	        topt2_referral+topt2_reversal+topt2_floor,
	        /**** BIT FIELDS ****/
	        bf_2pan,        /* MAND */
	        bf_3proc,	    /* MAND */
	        bf_4amount,     /* MAND */
	        bf_7trntime,    /* MAND */
	        bf_11stan,      /* MAND */
	        bf_12time,      /* MAND */
	        bf_13date,      /* MAND */
	        bf_14ExpDate,
	        bf_15datestl,   /* MAND */
	        bf_18mertype,
	        bf_22posentry,  /* MAND */
	        bf_23cardseqnum,
	        bf_25poscc,     /* MAND */
	        bf_32idcode,    /* MAND */
	        bf_35track2,    /* COND */
	        bf_36track3,    /* COND */
	  	  bf_37rrn,	      /* MAND */	/* Girija Y - TF */
	        bf_41termid,    /* COND */
	        bf_42cardacc,   /* MAND */
	        bf_43cardacceptloc,
	        bf_44respdata,
	  	  bf_45track1,		    /* COND */
	        bf_48addldata,  /* MAND */
	        bf_49trncurcd,  /* MAND */
	        bf_52pinblock,  /* MAND */
	        bf_55ICCdata,
	         bf_60priv60,
	  	  bf_62priv62,
	  	  bf_63priv63,	  /* COND */    /* Girija Y - TF */
	        bf_102acct1,    /* COND */
	        bf_103acct2,    /* COND */
	  	  bf_126priv126,
	        bf_128msg_authc,/* COND */
	        bf_done      /* end of fields to move */
	        },

	        {
	        /***************************************************
	          A U T H    P R E - A U T H   C A N C E L  R E S P O N S E
	        ****************************************************/
	        AUTH_PRE_AUTHORIZATION_CANCLE_RESPONSE,
	        0xF0,0xF2,0xF1,0xF0,                  /* message id, EBCDIC => 0210 */
	        0xF2,0xF0,0xF0,0xF0,0xF0,0xF0,             /* processing code */
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
	        bf_23cardseqnum,
	        bf_32idcode,   /* MAND */
	  	  bf_37rrn,      /* MAND */		/* Girija Y - TF */
	        bf_38authcode, /* COND */
	        bf_39respcode, /* MAND */
	        bf_41termid,   /* COND */
	        bf_48addldata, /* MAND */
	        bf_55ICCdata,
	  	  bf_63priv63,	  /* COND */    /* Girija Y - TF */
	        bf_102acct1,   /* MAND */
	        bf_103acct2,   /* MAND */
	        bf_done      /* end of fields to move */
	        },

      {
      /********************************************
         C A S H   A D V A N C E 
      ********************************************/
      AUTH_CASH_ADVANCE,
      0xF0,0xF2,0xF0,0xF0,                  /* message id, EBCDIC => 0200 */
      0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,        /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,        /* MAND */
      bf_3proc,	    /* MAND */
      bf_4amount,     /* MAND */
      bf_7trntime,    /* MAND */
      bf_11stan,      /* MAND */
      bf_12time,      /* MAND */ 
      bf_13date,      /* MAND */
      bf_14ExpDate,
      bf_15datestl,   /* MAND */ 
	  bf_18mertype,
      bf_22posentry,  /* MAND */
      bf_23cardseqnum,
      bf_25poscc,     /* MAND */
	  bf_32idcode,    /* MAND */
      bf_35track2,    /* COND */      
      bf_36track3,    /* COND */
	  bf_37rrn,       /* MAND */	/* Girija Y - TF */
      bf_41termid,    /* COND */
      bf_42cardacc,   /* MAND */      
      bf_43cardacceptloc,  
      bf_44respdata,
	  bf_45track1,		    /* COND */
      bf_48addldata,  /* MAND */
      bf_49trncurcd,  /* MAND */
      bf_52pinblock,  /* MAND */  
      bf_55ICCdata,
       bf_60priv60,
	  bf_62priv62,
	  bf_63priv63,	  /* COND */    /* Girija Y - TF */
      bf_102acct1,    /* COND */
      bf_103acct2,    /* COND */
	  bf_126priv126,
      bf_128msg_authc,/* COND */
      bf_done      /* end of fields to move */
      },

      {
      /***********************************************
         C A S H   A D V A N C E   R E S P O N S E
      ***********************************************/
      AUTH_CASH_ADVANCE_RESPONSE,
      0xF0,0xF2,0xF1,0xF0,                  /* message id, EBCDIC => 0210 */
      0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,        /* processing code */
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
      bf_23cardseqnum,
      bf_32idcode,   /* MAND */  
	  bf_37rrn,      /* MAND */		/* Girija Y - TF */
      bf_38authcode, /* COND */
      bf_39respcode, /* MAND */
      bf_41termid,   /* COND */
      bf_48addldata, /* MAND */
	 /* bf_49trncurcd,*/  /* COND */
      bf_55ICCdata,
	  /*bf_62priv62,*/		/* COND */
	  bf_63priv63,	  /* COND */    /* Girija Y - TF */
      bf_102acct1,   /* MAND */  
      bf_103acct2,   /* MAND */      
      bf_done      /* end of fields to move */
      },

	  {
	        /********************************************
	          MOney Send
	        ********************************************/
	        AUTH_MONEY_SEND,
	        0xF0,0xF2,0xF0,0xF0,                  /* message id, EBCDIC => 0200 */
	        0xF2,0xF8,0xF0,0xF0,0xF0,0xF0,        /* processing code */
	        /**** OPTIONS ****/
	        topt1_postadv,
	        topt2_referral+topt2_reversal+topt2_floor,
	        /**** BIT FIELDS ****/
	        bf_2pan,        /* MAND */
	        bf_3proc,	    /* MAND */
	        bf_4amount,     /* MAND */
	        bf_7trntime,    /* MAND */
	        bf_11stan,      /* MAND */
	        bf_12time,      /* MAND */
	        bf_13date,      /* MAND */
	        bf_14ExpDate,
	        bf_15datestl,   /* MAND */
	  	  bf_18mertype,
	        bf_22posentry,  /* MAND */
	        bf_23cardseqnum,
	        bf_25poscc,     /* MAND */
	  	  bf_32idcode,    /* MAND */
	        bf_35track2,    /* COND */
	        bf_36track3,    /* COND */
	  	  bf_37rrn,       /* MAND */	/* Girija Y - TF */
	        bf_41termid,    /* COND */
	        bf_42cardacc,   /* MAND */
	        bf_43cardacceptloc,
	        bf_44respdata,
	  	  bf_45track1,		    /* COND */
	        bf_48addldata,  /* MAND */
	        bf_49trncurcd,  /* MAND */
	        bf_52pinblock,  /* MAND */
	        bf_55ICCdata,
	         bf_60priv60,
	  	  bf_62priv62,
	  	  bf_63priv63,	  /* COND */    /* Girija Y - TF */
	        bf_102acct1,    /* COND */
	        bf_103acct2,    /* COND */
	  	  bf_126priv126,
	        bf_128msg_authc,/* COND */
	        bf_done      /* end of fields to move */
	        },

	        {
	        /***********************************************
	           Money send Response 
	        ***********************************************/
	        AUTH_MONEY_SEND_RESPONSE,
	        0xF0,0xF2,0xF1,0xF0,                  /* message id, EBCDIC => 0210 */
	        0xF2,0xF8,0xF0,0xF0,0xF0,0xF0,        /* processing code */
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
	        bf_23cardseqnum,
	        bf_32idcode,   /* MAND */
	  	  bf_37rrn,      /* MAND */		/* Girija Y - TF */
	        bf_38authcode, /* COND */
	        bf_39respcode, /* MAND */
	        bf_41termid,   /* COND */
	        bf_48addldata, /* MAND */
	        bf_55ICCdata,
	  	  bf_63priv63,	  /* COND */    /* Girija Y - TF */
	        bf_102acct1,   /* MAND */
	        bf_103acct2,   /* MAND */
	        bf_done      /* end of fields to move */
	        },

	  {
      /****************************************************
         C A S H   A D V A N C E  A U T H O R I Z A T I O N 
      ****************************************************/
      AUTH_CASH_ADVANCE_AUTHORIZATION,
      0xF0,0xF2,0xF0,0xF0,                  /* message id, EBCDIC => 0200 */
      0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,        /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,        /* MAND */
      bf_3proc,	    /* MAND */
      bf_4amount,     /* MAND */
      bf_7trntime,    /* MAND */
      bf_11stan,      /* MAND */
      bf_12time,      /* MAND */ 
      bf_13date,      /* MAND */ 
      bf_14ExpDate,
      bf_15datestl,   /* MAND */ 
      bf_18mertype,
      bf_22posentry,  /* MAND */
      bf_23cardseqnum,
      bf_25poscc,     /* MAND */
      bf_32idcode,    /* MAND */
      bf_35track2,    /* COND */
      bf_36track3,    /* COND */
	  bf_37rrn,       /* MAND */	/* Girija Y - TF */
      bf_41termid,    /* COND */
      bf_42cardacc,   /* MAND */      
      bf_43cardacceptloc,
	  bf_44respdata,
	  bf_48addldata,  /* MAND */
      bf_49trncurcd,  /* MAND */
      bf_52pinblock,  /* MAND */ 
      bf_55ICCdata,
       bf_60priv60,
	  bf_62priv62,
	  bf_63priv63,	  /* COND */    /* Girija Y - TF */
      bf_102acct1,    /* COND */
      bf_103acct2,    /* COND */
	  bf_126priv126,
      bf_128msg_authc,/* COND */
      bf_done      /* end of fields to move */
      },
       
	  {
      /************************************************************************
         C A S H   A D V A N C E  A U T H O R I Z A T I O N  O V E R R I D E
      **************************************************************************/
      AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE,
      0xF0,0xF2,0xF0,0xF0,                  /* message id, EBCDIC => 0200 */
      0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,        /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,        /* MAND */
      bf_3proc,	    /* MAND */
      bf_4amount,     /* MAND */
      bf_7trntime,    /* MAND */
      bf_11stan,      /* MAND */
      bf_12time,      /* MAND */ 
      bf_13date,      /* MAND */ 
      bf_15datestl,   /* MAND */ 
	  bf_18mertype,
      bf_22posentry,  /* MAND */
      bf_25poscc,     /* MAND */
      bf_32idcode,    /* MAND */
      bf_35track2,    /* COND */      
      bf_36track3,    /* COND */ 
	  bf_37rrn,       /* MAND */	/* Girija Y - TF */
      bf_41termid,    /* COND */
      bf_42cardacc,   /* MAND */      
      bf_43cardacceptloc,
      bf_48addldata,  /* MAND */
      bf_49trncurcd,  /* MAND */
      bf_52pinblock,  /* MAND */  
       bf_60priv60,
	  bf_62priv62,
	  bf_63priv63,	  /* COND */    /* Girija Y - TF */
      bf_102acct1,    /* COND */
      bf_103acct2,    /* COND */
	  bf_126priv126,
      bf_128msg_authc,/* COND */
      bf_done      /* end of fields to move */
      },

	  {
      /************************************************************************
         C A S H    A U T H O R I Z A T I O N  O V E R R I D E
      **************************************************************************/
      AUTH_AUTHORIZATION_OVERRIDE,
      0xF0,0xF2,0xF0,0xF0,                  /* message id, EBCDIC => 0200 */
      0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,        /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,        /* MAND */
      bf_3proc,	    /* MAND */
      bf_4amount,     /* MAND */
      bf_7trntime,    /* MAND */
      bf_11stan,      /* MAND */
      bf_12time,      /* MAND */ 
      bf_13date,      /* MAND */ 
      bf_15datestl,   /* MAND */ 
	  bf_18mertype,
      bf_22posentry,  /* MAND */
      bf_25poscc,     /* MAND */
      bf_32idcode,    /* MAND */
      bf_35track2,    /* COND */
      bf_36track3,    /* COND */
	  bf_37rrn,       /* MAND */	/* Girija Y - TF */
      bf_41termid,    /* COND */
      bf_42cardacc,   /* MAND */      
      bf_43cardacceptloc,
      bf_48addldata,  /* MAND */
      bf_49trncurcd,  /* MAND */
      bf_52pinblock,  /* MAND */
       bf_60priv60,
	  bf_62priv62,
	  bf_63priv63,	  /* COND */    /* Girija Y - TF */
      bf_102acct1,    /* COND */
      bf_103acct2,    /* COND */
	  bf_126priv126,
      bf_128msg_authc,/* COND */
      bf_done      /* end of fields to move */
      },

      {
      /********************************************
         S A L E 
      ********************************************/
      AUTH_SALE,
      0xF0,0xF2,0xF0,0xF0,                  /* message id, EBCDIC => 0200 */
      0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,        /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,        /* MAND */
      bf_3proc,	    /* MAND */
      bf_4amount,     /* MAND */
      bf_7trntime,    /* MAND */
      bf_11stan,      /* MAND */
      bf_12time,      /* MAND */ 
      bf_13date,      /* MAND */ 
      bf_14ExpDate,
      bf_15datestl,   /* MAND */ 
      bf_18mertype,
      bf_22posentry,  /* MAND */
      bf_23cardseqnum,
      bf_25poscc,     /* MAND */
      bf_32idcode,    /* MAND */
      bf_35track2,    /* COND */
      bf_36track3,    /* COND */
	  bf_37rrn,       /* MAND */    /* Girija Y - TF */
      bf_41termid,    /* COND */
      bf_42cardacc,   /* MAND */      
      bf_43cardacceptloc,
      bf_44respdata,
	  bf_45track1,		    /* COND */
      bf_48addldata,  /* MAND */
      bf_49trncurcd,  /* MAND */
      bf_52pinblock,  /* MAND */  
      bf_55ICCdata,
       bf_60priv60,
	  bf_62priv62,
	  bf_63priv63,	  /* COND */    /* Girija Y - TF */
      bf_102acct1,    /* COND */
      bf_103acct2,    /* COND */
	  bf_126priv126,
      bf_128msg_authc,/* COND */
      bf_done      /* end of fields to move */
      },

      {
      /***********************************************
         S A L E   R E S P O N S E
      ***********************************************/
      AUTH_SALE_RESPONSE,
      0xF0,0xF2,0xF1,0xF0,                  /* message id, EBCDIC => 0210 */
      0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,        /* processing code */
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
      bf_23cardseqnum,
      bf_32idcode,   /* MAND */    
	  bf_37rrn,      /* MAND */		/* Girija Y - TF */
      bf_38authcode, /* COND */
      bf_39respcode, /* MAND */
      bf_41termid,   /* COND */
      bf_48addldata, /* MAND */
      bf_55ICCdata,
	  bf_63priv63,	  /* COND */    /* Girija Y - TF */
      bf_102acct1,   /* MAND */  
      bf_103acct2,   /* MAND */      
      bf_done      /* end of fields to move */
      },

      {
      /********************************************
         A U T H O R I Z A T I O N
      ********************************************/
      AUTH_AUTHORIZATION,
      0xF0,0xF2,0xF0,0xF0,                  /* message id, EBCDIC => 0200 */
      0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,        /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,        /* MAND */
      bf_3proc,	    /* MAND */
      bf_4amount,     /* MAND */
      bf_7trntime,    /* MAND */
      bf_11stan,      /* MAND */
      bf_12time,      /* MAND */ 
      bf_13date,      /* MAND */ 
      bf_14ExpDate,
      bf_15datestl,   /* MAND */ 
      bf_18mertype,
      bf_22posentry,  /* MAND */
      bf_23cardseqnum,
      bf_25poscc,     /* MAND */
	  bf_28tranfeeamt, /* COND */
      bf_32idcode,    /* MAND */
      bf_35track2,    /* COND */
      bf_36track3,    /* COND */
	  bf_37rrn,      /* MAND */		/* Girija Y - TF */
      bf_41termid,    /* COND */
      bf_42cardacc,   /* MAND */      
      bf_43cardacceptloc,
      bf_44respdata,
	  bf_45track1,		    /* COND */
      bf_48addldata,  /* MAND */
      bf_49trncurcd,  /* MAND */
      bf_52pinblock,  /* MAND */
      bf_55ICCdata,
       bf_60priv60,
	  bf_62priv62,
	  bf_63priv63,	  /* COND */    /* Girija Y - TF */
      bf_102acct1,    /* COND */
      bf_103acct2,    /* COND */
	  bf_104trandesc,
	  bf_126priv126,
      bf_128msg_authc,/* COND */
      bf_done      /* end of fields to move */
      },

      {
      /***********************************************
         A U T H O R I Z A T I O N   R E S P O N S E
      ***********************************************/
      AUTH_AUTHORIZATION_RESPONSE,
      0xF0,0xF2,0xF1,0xF0,                  /* message id, EBCDIC => 0210 */
      0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,        /* processing code */
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
      bf_23cardseqnum,
      bf_32idcode,   /* MAND */   
	  bf_37rrn,      /* MAND */  /* Girija Y - TF */
      bf_38authcode, /* COND */
      bf_39respcode, /* MAND */
      bf_41termid,   /* COND */
      bf_48addldata, /* MAND */
      bf_55ICCdata,
	  bf_63priv63,	  /* COND */    /* Girija Y - TF */
      bf_102acct1,   /* MAND */  
      bf_103acct2,   /* MAND */      
      bf_done      /* end of fields to move */
      },

	  {
	        /********************************************
	           A U T H Q U A S I C A S H
	        ********************************************/
			AUTH_QUASI_CASH,
	        0xF0,0xF2,0xF0,0xF0,                  /* message id, EBCDIC => 0200 */
	        0xF1,0xF1,0xF0,0xF0,0xF0,0xF0,        /* processing code */
	        /**** OPTIONS ****/
	        topt1_postadv,
	        topt2_referral+topt2_reversal+topt2_floor,
	        /**** BIT FIELDS ****/
	        bf_2pan,        /* MAND */
	        bf_3proc,	    /* MAND */
	        bf_4amount,     /* MAND */
	        bf_7trntime,    /* MAND */
	        bf_11stan,      /* MAND */
	        bf_12time,      /* MAND */
	        bf_13date,      /* MAND */
	        bf_14ExpDate,
	        bf_15datestl,   /* MAND */
	        bf_18mertype,
	        bf_22posentry,  /* MAND */
	        bf_23cardseqnum,
	        bf_25poscc,     /* MAND */
	        bf_32idcode,    /* MAND */
	        bf_35track2,    /* COND */
	        bf_36track3,    /* COND */
			bf_37rrn,      /* MAND */		/* Girija Y - TF */
	        bf_41termid,    /* COND */
	        bf_42cardacc,   /* MAND */
	        bf_43cardacceptloc,
	        bf_44respdata,
			bf_45track1,		    /* COND */
	        bf_48addldata,  /* MAND */
	        bf_49trncurcd,  /* MAND */
	        bf_52pinblock,  /* MAND */
	        bf_55ICCdata,
	        bf_60priv60,
			bf_62priv62,
			bf_63priv63,	  /* COND */    /* Girija Y - TF */
	        bf_102acct1,    /* COND */
	        bf_103acct2,    /* COND */
			bf_126priv126,
	        bf_128msg_authc,/* COND */
	        bf_done      /* end of fields to move */
	        },


			{
			      /***********************************************
			         A U T H Q U A S I C A S H   R E S P O N S E
			      ***********************************************/
				  AUTH_QUASI_CASH_RESPONSE,
			      0xF0,0xF2,0xF1,0xF0,                  /* message id, EBCDIC => 0210 */
			      0xF1,0xF1,0xF0,0xF0,0xF0,0xF0,        /* processing code */
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
			      bf_23cardseqnum,
			      bf_32idcode,   /* MAND */
				  bf_37rrn,      /* MAND */  /* Girija Y - TF */
			      bf_38authcode, /* COND */
			      bf_39respcode, /* MAND */
			      bf_41termid,   /* COND */
			      bf_48addldata, /* MAND */
			      bf_55ICCdata,
				  bf_63priv63,	  /* COND */    /* Girija Y - TF */
			      bf_102acct1,   /* MAND */
			      bf_103acct2,   /* MAND */
			      bf_done      /* end of fields to move */
			},


	  {
	        /*****************************************************
	           A U T H   A D V I C E
	        ******************************************************/
	        AUTH_ADVICE,
	        0xF0,0xF1,0xF2,0xF0,             /* message id, EBCDIC => 0120     */
	        0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,   /* processing code  (use original */
	        /**** OPTIONS ****/
	        topt1_credit+topt1_postadv,
	        topt2_org_proccode,     /* use original processing code */
	        /**** BIT FIELDS ****/
	        bf_2pan,         /* MAND */
	        bf_3proc,        /* MAND */
	        bf_4amount,      /* MAND */
	        bf_7trntime,     /* MAND */
	        bf_11stan,       /* MAND */
	        bf_12time,       /* MAND */
	        bf_13date,       /* MAND */
	        bf_14ExpDate,
	        bf_15datestl,    /* MAND */
	        bf_22posentry,   /* MAND */
	        bf_23cardseqnum,
	        bf_25poscc,      /* MAND */
	        bf_32idcode,     /* MAND */
	        bf_35track2,     /* MAND */
	        bf_36track3,     /* MAND */
	  	    bf_37rrn,      /* MAND */  /* Girija Y - TF */
	        bf_38authcode,   /* MAND */
	        bf_39respcode,   /* MAND */
	        bf_41termid,     /* MAND */
	        bf_42cardacc,    /* MAND */
	        bf_43cardacceptloc,
			bf_44respdata,
	        bf_52pinblock,   /* MAND */
	        bf_55ICCdata,
	        bf_60priv60,
	  	    bf_62priv62,
	  	    bf_63priv63,	  /* COND */    /* Girija Y - TF */
			bf_126priv126,    /* COND */
	        bf_128msg_authc, /* MAND */
	        bf_done          /* end of fields to move */
	   },
	      {
	      /*****************************************************
	         A U T H   A D V I C E   R E S P O N S E
	      ******************************************************/
	      AUTH_ADVICE_RESPONSE,
	      0xF0,0xF1,0xF3,0xF0,             /* message id, EBCDIC => 0130     */
	      0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,   /* processing code  (use original */
	      /**** OPTIONS ****/
	      topt1_credit+topt1_postadv,
	      topt2_org_proccode,     /* use original processing code */
	      /**** BIT FIELDS ****/
	      bf_2pan,         /* MAND */
	      bf_3proc,        /* MAND */
	      bf_4amount,      /* MAND */
	      bf_7trntime,     /* MAND */
	      bf_11stan,       /* MAND */
	      bf_15datestl,    /* MAND */
	      bf_32idcode,     /* MAND */
		  bf_37rrn,      /* MAND */  /* Girija Y - TF */
	      bf_39respcode,   /* MAND */
	      bf_41termid,     /* MAND */
		  bf_44respdata,
		  bf_102acct1,   /* MAND */
		  bf_103acct2,   /* MAND */
	      bf_128msg_authc, /* MAND */
	      bf_done          /* end of fields to move */
	      },
      {
      /*****************************************************
         R E V E R S A L   A D V I C E   
      ******************************************************/
      AUTH_REVERSAL_ADVICE,
      0xF0,0xF4,0xF2,0xF0,             /* message id, EBCDIC => 0420     */
      0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,   /* processing code  (use original */
      /**** OPTIONS ****/
      topt1_credit+topt1_postadv,
      topt2_org_proccode,     /* use original processing code */
      /**** BIT FIELDS ****/
      bf_2pan,         /* MAND */
      bf_3proc,        /* MAND */
      bf_4amount,      /* MAND */      
      bf_7trntime,     /* MAND */      
      bf_11stan,       /* MAND */
      bf_12time,       /* MAND */  
      bf_13date,       /* MAND */
      bf_14ExpDate,
      bf_15datestl,    /* MAND */
      bf_22posentry,   /* MAND */  
      bf_23cardseqnum,
      bf_25poscc,      /* MAND */
      bf_32idcode,     /* MAND */
      bf_35track2,     /* MAND */
      bf_36track3,     /* MAND */
	  bf_37rrn,      /* MAND */  /* Girija Y - TF */
      bf_38authcode,   /* MAND */      
      bf_39respcode,   /* MAND */
      bf_41termid,     /* MAND */  
      bf_42cardacc,    /* MAND */
      bf_43cardacceptloc,
	  bf_49trncurcd, 	/* MAND */  /* for DCC, 126.19 = 1 */
      bf_52pinblock,   /* MAND */
      bf_55ICCdata,
       bf_60priv60,
	  bf_62priv62,
	  bf_63priv63,	  /* COND */    /* Girija Y - TF */
      bf_90orgdata,    /* MAND */
      bf_95repl_amt,   /* MAND */
	  bf_126priv126,   /* COND */
      bf_128msg_authc, /* MAND */
      bf_done          /* end of fields to move */
      },

      {
      /*****************************************************
         R E V E R S A L   A D V I C E   R E P E A T
      ******************************************************/
      AUTH_REVERSAL_REPEAT,
      0xF0,0xF4,0xF2,0xF1,             /* message id, EBCDIC => 0421     */
      0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,   /* processing code  (use original */
      /**** OPTIONS ****/
      topt1_credit+topt1_postadv,
      topt2_org_proccode,     /* use original processing code */
      /**** BIT FIELDS ****/
      bf_2pan,         /* MAND */
      bf_3proc,        /* MAND */
      bf_4amount,      /* MAND */      
      bf_7trntime,     /* MAND */      
      bf_11stan,       /* MAND */
      bf_12time,       /* MAND */  
      bf_13date,       /* MAND */
      bf_14ExpDate,
      bf_15datestl,    /* MAND */
      bf_22posentry,   /* MAND */  
      bf_25poscc,      /* MAND */
	  bf_32idcode,     /* MAND */
      bf_35track2,     /* MAND */
      bf_36track3,     /* MAND */
	  bf_37rrn,      /* MAND */  /* Girija Y - TF */
      bf_38authcode,   /* MAND */      
      bf_39respcode,   /* MAND */
      bf_41termid,     /* MAND */  
      bf_42cardacc,    /* MAND */
      bf_43cardacceptloc,
      bf_52pinblock,   /* MAND */
	  bf_60priv60,
	   bf_62priv62,
	  bf_63priv63,	  /* COND */    /* Girija Y - TF */
      bf_90orgdata,    /* MAND */
      bf_95repl_amt,   /* MAND */
      bf_128msg_authc, /* MAND */
      bf_done          /* end of fields to move */
      },

      {
      /*****************************************************
         R E V E R S A L  
      ******************************************************/
      AUTH_REVERSAL,
      0xF0,0xF4,0xF2,0xF0,             /* message id, EBCDIC => 0420     */
      0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,   /* processing code  (use original */
      /**** OPTIONS ****/
      topt1_credit+topt1_postadv,
      topt2_org_proccode,     /* use original processing code */
      /**** BIT FIELDS ****/
      bf_2pan,         /* MAND */
      bf_3proc,        /* MAND */
      bf_4amount,      /* MAND */      
      bf_7trntime,     /* MAND */      
      bf_11stan,       /* MAND */
      bf_12time,       /* MAND */  
      bf_13date,       /* MAND */
      bf_14ExpDate,
      bf_15datestl,    /* MAND */
      bf_22posentry,   /* MAND */ 
      bf_23cardseqnum,
      bf_25poscc,      /* MAND */
      bf_32idcode,     /* MAND */
      bf_35track2,     /* MAND */
      bf_36track3,     /* MAND */
	  bf_37rrn,      /* MAND */  /* Girija Y - TF */
      bf_38authcode,   /* MAND */      
      bf_39respcode,   /* MAND */
      bf_41termid,     /* MAND */  
      bf_42cardacc,    /* MAND */
      bf_43cardacceptloc,
      bf_52pinblock,   /* MAND */
      bf_55ICCdata,
       bf_60priv60,
	  bf_62priv62,
	  bf_63priv63,	  /* COND */    /* Girija Y - TF */
      bf_90orgdata,    /* MAND */
      bf_95repl_amt,   /* MAND */
      bf_128msg_authc, /* MAND */
      bf_done          /* end of fields to move */
      },

      {
      /*****************************************************
         R E V E R S A L   A D V I C E   R E S P O N S E
      ******************************************************/
      AUTH_REVERSAL_ADVICE_RESPONSE,
      0xF0,0xF4,0xF3,0xF0,             /* message id, EBCDIC => 0430     */
      0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,   /* processing code  (use original */
      /**** OPTIONS ****/
      topt1_credit+topt1_postadv,
      topt2_org_proccode,     /* use original processing code */
      /**** BIT FIELDS ****/
      bf_2pan,         /* MAND */
      bf_3proc,        /* MAND */
      bf_4amount,      /* MAND */      
      bf_7trntime,     /* MAND */      
      bf_11stan,       /* MAND */
      bf_15datestl,    /* MAND */      
      bf_32idcode,     /* MAND */   
	  bf_37rrn,      /* MAND */  /* Girija Y - TF */
      bf_39respcode,   /* MAND */
      bf_41termid,     /* MAND */
      bf_90orgdata,    /* MAND */  
      bf_95repl_amt,   /* MAND */ 
      bf_128msg_authc, /* MAND */
      bf_done          /* end of fields to move */
      },

      {
      /***********************************************
         R E V E R S A L    R E S P O N S E
      ************************************************/
      AUTH_REVERSAL_RESPONSE,
      0xF0,0xF4,0xF3,0xF0,             /* message id, EBCDIC => 0430     */
      0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,   /* processing code  (use original */
      /**** OPTIONS ****/
      topt1_credit+topt1_postadv,
      topt2_org_proccode,     /* use original processing code */
      /**** BIT FIELDS ****/
      bf_2pan,         /* MAND */
      bf_3proc,        /* MAND */
      bf_4amount,      /* MAND */      
      bf_7trntime,     /* MAND */      
      bf_11stan,       /* MAND */
      bf_15datestl,    /* MAND */      
      bf_32idcode,     /* MAND */   
	  bf_37rrn,      /* MAND */  /* Girija Y - TF */
      bf_39respcode,   /* MAND */
      bf_41termid,     /* MAND */
      bf_90orgdata,    /* MAND */  
      bf_95repl_amt,   /* MAND */ 
      bf_128msg_authc, /* MAND */
      bf_done          /* end of fields to move */
      },

      {
      /*****************************************************
         R E V E R S A L   A D V I C E   (ATM CONFIRM)
      ******************************************************/
      AUTH_ATM_CONFIRM,
      0xF0,0xF4,0xF2,0xF0,             /* message id, EBCDIC => 0420     */
      0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,   /* processing code  (use original */
      /**** OPTIONS ****/
      topt1_credit+topt1_postadv,
      topt2_org_proccode,     /* use original processing code */
      /**** BIT FIELDS ****/
      bf_2pan,         /* MAND */
      bf_3proc,        /* MAND */
      bf_4amount,      /* MAND */      
      bf_7trntime,     /* MAND */      
      bf_11stan,       /* MAND */
      bf_12time,       /* MAND */  
      bf_13date,       /* MAND */
      bf_15datestl,    /* MAND */
      bf_22posentry,   /* MAND */  
      bf_25poscc,      /* MAND */
	  bf_32idcode,     /* MAND */
      bf_35track2,     /* MAND */
      bf_36track3,     /* MAND */
	  bf_37rrn,        /* MAND */  /* Girija Y - TF */
      bf_38authcode,   /* MAND */      
      bf_39respcode,   /* MAND */
      bf_41termid,     /* MAND */  
      bf_42cardacc,    /* MAND */
	  bf_43cardacceptloc,
      bf_52pinblock,   /* MAND */
      bf_60priv60,
	  bf_62priv62,
	  bf_63priv63,	  /* COND */    /* Girija Y - TF */
      bf_90orgdata,    /* MAND */
      bf_95repl_amt,   /* MAND */
	  bf_126priv126,
      bf_128msg_authc, /* MAND */
      bf_done          /* end of fields to move */
      },

      {
      /*************************************************************************
         R E V E R S A L   A D V I C E   R E S P O N S E  (ATM CONFIRM RESPONSE)
      *************************************************************************/
      AUTH_ATM_CONFIRM_RESPONSE,
      0xF0,0xF4,0xF3,0xF0,             /* message id, EBCDIC => 0430     */
      0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,   /* processing code  (use original */
      /**** OPTIONS ****/
      topt1_credit+topt1_postadv,
      topt2_org_proccode,     /* use original processing code */
      /**** BIT FIELDS ****/
      bf_2pan,         /* MAND */
      bf_3proc,        /* MAND */
      bf_4amount,      /* MAND */      
      bf_7trntime,     /* MAND */      
      bf_11stan,       /* MAND */
      bf_15datestl,    /* MAND */      
      bf_32idcode,     /* MAND */    
	  bf_37rrn,        /* MAND */  /* Girija Y - TF */
      bf_39respcode,   /* MAND */
      bf_41termid,     /* MAND */
      bf_90orgdata,    /* MAND */  
      bf_95repl_amt,   /* MAND */ 
      bf_128msg_authc, /* MAND */
      bf_done          /* end of fields to move */
      },

      {
      /*****************************************************
         V O I D   S A L E  (Goes out as a reversal)
      ******************************************************/
      AUTH_VOID_SALE,
      0xF0,0xF4,0xF2,0xF0,             /* message id, EBCDIC => 0420     */
      0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,   /* processing code  (use original */
      /**** OPTIONS ****/
      topt1_credit+topt1_postadv,
      topt2_org_proccode,     /* use original processing code */
      /**** BIT FIELDS ****/
      bf_2pan,         /* MAND */
      bf_3proc,        /* MAND */
      bf_4amount,      /* MAND */      
      bf_7trntime,     /* MAND */      
      bf_11stan,       /* MAND */
      bf_12time,       /* MAND */  
      bf_13date,       /* MAND */
      bf_14ExpDate,
      bf_15datestl,    /* MAND */
      bf_22posentry,   /* MAND */  
      bf_25poscc,      /* MAND */
      bf_32idcode,     /* MAND */
      bf_35track2,     /* MAND */
      bf_36track3,     /* MAND */
	  bf_37rrn,						// TF PHANI ...RRN added
      bf_38authcode,   /* MAND */      
      bf_39respcode,   /* MAND */
      bf_41termid,     /* MAND */  
      bf_42cardacc,    /* MAND */
      bf_43cardacceptloc,
      bf_52pinblock,   /* MAND */
       bf_60priv60,
	  bf_62priv62,
	  bf_63priv63,					// TF PHANI ...DE63 added
      bf_90orgdata,    /* MAND */
      bf_95repl_amt,   /* MAND */
	  bf_126priv126,
      bf_128msg_authc, /* MAND */
      bf_done          /* end of fields to move */
      },

      {
      /******************************************************
         V O I D   S A L E   C A S H (Goes out as a reversal)
      *******************************************************/
      AUTH_VOID_CASH_ADVANCE,
      0xF0,0xF4,0xF2,0xF0,             /* message id, EBCDIC => 0420     */
      0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,   /* processing code  (use original */
      /**** OPTIONS ****/
      topt1_credit+topt1_postadv,
      topt2_org_proccode,     /* use original processing code */
      /**** BIT FIELDS ****/
      bf_2pan,         /* MAND */
      bf_3proc,        /* MAND */
      bf_4amount,      /* MAND */      
      bf_7trntime,     /* MAND */      
      bf_11stan,       /* MAND */
      bf_12time,       /* MAND */  
      bf_13date,       /* MAND */
      bf_15datestl,    /* MAND */
      bf_22posentry,   /* MAND */  
      bf_25poscc,      /* MAND */
      bf_32idcode,     /* MAND */
      bf_35track2,     /* MAND */
      bf_36track3,     /* MAND */
	  bf_37rrn,						// TF Girija Y ...RRN added
      bf_38authcode,   /* MAND */      
      bf_39respcode,   /* MAND */
      bf_41termid,     /* MAND */  
      bf_42cardacc,    /* MAND */
	  bf_43cardacceptloc,
      bf_52pinblock,   /* MAND */
       bf_60priv60,
	  bf_62priv62,
	  bf_63priv63,					// TF Girija Y ...DE63 added
      bf_90orgdata,    /* MAND */
      bf_95repl_amt,   /* MAND */
	  bf_126priv126,
      bf_128msg_authc, /* MAND */
      bf_done          /* end of fields to move */
      },
/* OCT and Account funding tx*/
{
      /********************************************
         OCT A U T H O R I Z A T I O N
      ********************************************/
      AUTH_OCT_TX,
      0xF0,0xF2,0xF0,0xF0,                  /* message id, EBCDIC => 0200 */
      0xF2,0xF6,0xF0,0xF0,0xF0,0xF0,        /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,        /* MAND */
      bf_3proc,	    /* MAND */
      bf_4amount,     /* MAND */
      bf_7trntime,    /* MAND */
      bf_11stan,      /* MAND */
      bf_12time,      /* MAND */ 
      bf_13date,      /* MAND */ 
	  bf_14ExpDate,	  /* COND */
      bf_15datestl,   /* MAND */ 
	  bf_18mertype,
      bf_22posentry,  /* MAND */
      bf_25poscc,     /* MAND */
      bf_32idcode,    /* MAND */
      bf_35track2,    /* COND */
      bf_36track3,    /* COND */
	  bf_37rrn,      /* MAND */		/* Girija Y - TF */
      bf_41termid,    /* COND */
      bf_42cardacc,   /* MAND */ 
	  bf_43cardacceptloc,
	  bf_44respdata,
      bf_48addldata,  /* MAND */
      bf_49trncurcd,  /* MAND */
      bf_52pinblock,  /* MAND */
	  bf_60priv60,
	  bf_62priv62,
	  bf_63priv63,	  /* COND */    /* Girija Y - TF */
      bf_102acct1,    /* COND */
      bf_103acct2,    /* COND */
	  bf_104trandesc, /* MAND */
	  bf_126priv126,
      //bf_128msg_authc,/* COND */
      bf_done      /* end of fields to move */
      },

      {
      /***********************************************
        OCT  A U T H O R I Z A T I O N   R E S P O N S E
      ***********************************************/
      AUTH_OCT_TX_RESPONSE,
      0xF0,0xF2,0xF1,0xF0,                  /* message id, EBCDIC => 0210 */
      0xF2,0xF6,0xF0,0xF0,0xF0,0xF0,        /* processing code */
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
      bf_32idcode,   /* MAND */   
	  bf_37rrn,      /* MAND */  /* Girija Y - TF */
      bf_38authcode, /* COND */
      bf_39respcode, /* MAND */
      bf_41termid,   /* COND */
      bf_48addldata, /* MAND */
	  bf_62priv62,
	  bf_63priv63,	  /* COND */    /* Girija Y - TF */
      bf_102acct1,   /* MAND */  
      bf_103acct2,   /* MAND */ 
  	  bf_104trandesc, /* MAND */

      bf_done      /* end of fields to move */
      },{
      /********************************************
         FAST FUND A U T H O R I Z A T I O N
      ********************************************/
      AUTH_FAST_FUND,
      0xF0,0xF2,0xF0,0xF0,                  /* message id, EBCDIC => 0200 */
      0xF1,0xF0,0xF0,0xF0,0xF0,0xF0,        /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,        /* MAND */
      bf_3proc,	    /* MAND */
      bf_4amount,     /* MAND */
      bf_7trntime,    /* MAND */
      bf_11stan,      /* MAND */
      bf_12time,      /* MAND */ 
      bf_13date,      /* MAND */ 
	  bf_14ExpDate,   /* COND */
      bf_15datestl,   /* MAND */ 
	  bf_18mertype,
      bf_22posentry,  /* MAND */
      bf_25poscc,     /* MAND */
      bf_32idcode,    /* MAND */
      bf_35track2,    /* COND */
      bf_36track3,    /* COND */
	  bf_37rrn,      /* MAND */		/* Girija Y - TF */
      bf_41termid,    /* COND */
      bf_42cardacc,   /* MAND */      
      bf_43cardacceptloc,
	  bf_44respdata,
      bf_48addldata,  /* MAND */
      bf_49trncurcd,  /* MAND */
      bf_52pinblock,  /* MAND */   
       bf_60priv60,
	  bf_62priv62,
	  bf_63priv63,	  /* COND */    /* Girija Y - TF */
      bf_102acct1,    /* COND */
      bf_103acct2,    /* COND */
   	  bf_104trandesc, /* MAND */
	  bf_126priv126,
      bf_done      /* end of fields to move */
      },

      {
      /***********************************************
         FAST FUND A U T H O R I Z A T I O N   R E S P O N S E
      ***********************************************/
      AUTH_FAST_FUND_RESPONSE,
      0xF0,0xF2,0xF1,0xF0,                  /* message id, EBCDIC => 0210 */
      0xF1,0xF0,0xF0,0xF0,0xF0,0xF0,        /* processing code */
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
      bf_32idcode,   /* MAND */   
	  bf_37rrn,      /* MAND */  /* Girija Y - TF */
      bf_38authcode, /* COND */
      bf_39respcode, /* MAND */
      bf_41termid,   /* COND */
      bf_48addldata, /* MAND */
	  bf_63priv63,	  /* COND */    /* Girija Y - TF */
      bf_102acct1,   /* MAND */  
      bf_103acct2,   /* MAND */
   	  bf_104trandesc, /* MAND */

      bf_done      /* end of fields to move */
      },

      {
      /********************************************
         L O G O N 
      ********************************************/
      AUTH_LOGON,
      0xF0,0xF8,0xF0,0xF0,           /* message id, EBCDIC => 0800 */
      0xF0,0xF0,0xF0,0xF0,0xF0,0xF0, /* processing code */
      /**** OPTIONS ****/
      topt1_noacct+topt1_nocapture,
      topt2_noprerev,
      /**** BIT FIELDS ****/
      bf_7trntime,   /* MAND */
      bf_11stan,     /* MAND */
      bf_15datestl,  /* MAND */
      bf_48addldata, /* MAND */
      bf_70nmicode,  /* MAND */      
      bf_done        /* end of fields to move */
      },

      {
      /********************************************
         L O G O N   R E S P O N S E
      ********************************************/
      AUTH_LOGON_RESPONSE,
      0xF0,0xF8,0xF1,0xF0,            /* message id, EBCDIC => 0810 */
      0xF9,0xF0,0xF0,0xF0,0xF0,0xF0,  /* processing code */
      /**** OPTIONS ****/
      topt1_noacct+topt1_nocapture,
      topt2_noprerev,
      /**** BIT FIELDS ****/
      bf_7trntime,   /* MAND */
      bf_11stan,     /* MAND */      
      bf_70nmicode,  /* MAND */
      bf_done        /* end of fields to move */
      },
      {
      /********************************************
        B A C K   O F F I C E  A D J U S T M E N T 
      ********************************************/
      AUTH_BACKOFFICE_ADJUSTMENT,
      0xF0,0xF2,0xF2,0xF0,                  /* message id, EBCDIC => 0220 */
      0xF0,0xF2,0xF0,0xF0,0xF0,0xF0,        /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,        /* MAND */
      bf_3proc,	    /* MAND */
      bf_4amount,     /* MAND */
      bf_7trntime,    /* MAND */
      bf_11stan,      /* MAND */
      bf_13date,      /* MAND */ 
      bf_18mertype,
      bf_19acqinstcode,
      bf_21fwdinstcntcode,
      bf_22posentry,  /* MAND */
      bf_25poscc,     /* MAND */
      bf_28tranfeeamt,
      bf_32idcode,    /* MAND */
      bf_33fwdinstidcode,
      bf_37rrn,
      bf_41termid,    /* COND */
      bf_42cardacc,   /* MAND */      
      bf_43cardacceptloc,
      bf_49trncurcd,  /* MAND */
      bf_62priv62,	
      bf_63priv63,		       
      bf_done      /* end of fields to move */
      },
      {
      /*************************************************************
       B A C K   O F F I C E  A D J U S T M E N T  R E S P O N S E
      **************************************************************/
      AUTH_BACKOFFICE_ADJUSTMENT_RESPONSE,
      0xF0,0xF2,0xF3,0xF0,                  /* message id, EBCDIC => 0220 */
      0xF0,0xF2,0xF0,0xF0,0xF0,0xF0,        /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,        /* MAND */
      bf_3proc,	    /* MAND */
      bf_5settamount,
      bf_7trntime,    /* MAND */
      bf_9settconvrate,
      bf_11stan,      /* MAND */
      bf_15datestl,
      bf_16dateconv,
      bf_19acqinstcode,
      bf_20countrycode,
      bf_25poscc,     /* MAND */
      bf_32idcode,    /* MAND */
      bf_37rrn,
      bf_39respcode,
      bf_41termid,    /* COND */
      bf_42cardacc,   /* MAND */      
      bf_44respdata,
      bf_48addldata,  /* MAND */
      bf_62priv62,
      bf_63priv63,		       
      bf_done      /* end of fields to move */
      },
      {
      /********************************************
       R E P R E S E N T M E N T 
       ********************************************/
      AUTH_REPRESENTMENT,
      0xF0,0xF2,0xF2,0xF0,                  /* message id, EBCDIC => 0200 */
      0xF0,0xF1,0xF0,0xF0,0xF0,0xF0,        /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,        /* MAND */
      bf_3proc,	    /* MAND */
      bf_4amount,     /* MAND */
      bf_7trntime,    /* MAND */
      bf_11stan,      /* MAND */
      bf_13date,      /* MAND */ 
      bf_18mertype,
      bf_19acqinstcode,
      bf_25poscc,     /* MAND */
      bf_32idcode,    /* MAND */
      bf_37rrn,
      bf_41termid,    /* COND */
      bf_42cardacc,   /* MAND */      
      bf_43cardacceptloc,
      bf_48addldata,
      bf_49trncurcd,  /* MAND */
      bf_62priv62,
      bf_63priv63,		       
      bf_done      /* end of fields to move */
      },
      {
      /********************************************
       R E P R E S E N T M E N T  R E S P O N S E
      *********************************************/
      AUTH_REPRESENTMENT_RESPONSE,
      0xF0,0xF2,0xF3,0xF0,                  /* message id, EBCDIC => 0220 */
      0xF0,0xF1,0xF0,0xF0,0xF0,0xF0,        /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,        /* MAND */
      bf_3proc,	    /* MAND */
      bf_5settamount,
      bf_7trntime,    /* MAND */
      bf_9settconvrate,
      bf_11stan,      /* MAND */
      bf_15datestl,
      bf_16dateconv,
      bf_19acqinstcode,
      bf_20countrycode,
      bf_25poscc,     /* MAND */
      bf_32idcode,    /* MAND */
      bf_37rrn,
      bf_39respcode,
      bf_41termid,    /* COND */
      bf_42cardacc,   /* MAND */      
      bf_48addldata,  /* MAND */
      bf_62priv62,
      bf_63priv63,		       
      bf_done      /* end of fields to move */
      },
      {
      /*************************************************************
        F E E   C O L L E C T I O N   R E S P O N S E
      **************************************************************/
      AUTH_FEE_COLLECTION_ACQUIRER_RESPONSE,
      0xF0,0xF2,0xF3,0xF0,                  /* message id, EBCDIC => 0230 */
      0xF1,0xF9,0xF0,0xF0,0xF0,0xF0,        /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,             /* COND */
      bf_3proc,            /* MAND */
      bf_7trntime,         /* MAND */
      bf_11stan,           /* MAND */
      bf_15datestl,        /* MAND */
      bf_19acqinstcode,    /* MAND */
      bf_32idcode,         /* MAND */
      bf_37rrn,            /* MAND */
      bf_39respcode,       /* MAND */
      bf_63priv63,         /* MAND */
      bf_done      /* end of fields to move */
      },
      {
      /*************************************************************
        F U N D S   D I S B U R S E M E N T   R E S P O N S E
      **************************************************************/
      AUTH_FUNDS_DISBURSEMENT_ACQUIRER_RESPONSE,
      0xF0,0xF2,0xF3,0xF0,                  /* message id, EBCDIC => 0230 */
      0xF2,0xF9,0xF0,0xF0,0xF0,0xF0,        /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,             /* COND */
      bf_3proc,            /* MAND */
      bf_7trntime,         /* MAND */
      bf_11stan,           /* MAND */
      bf_15datestl,        /* MAND */
      bf_19acqinstcode,    /* MAND */
      bf_32idcode,         /* MAND */
      bf_37rrn,            /* MAND */
      bf_39respcode,       /* MAND */
      bf_63priv63,         /* MAND */
      bf_done      /* end of fields to move */
      },

      { 
      /********************************************
         R E L O A D 
      ********************************************/
      AUTH_RELOAD,
      0xF0,0xF2,0xF0,0xF0,                  /* message id, EBCDIC => 0200 */
      0xF2,0xF1,0xF0,0xF0,0xF0,0xF0,        /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,        /* MAND */
      bf_3proc,	    /* MAND */
      bf_4amount,     /* MAND */
      bf_7trntime,    /* MAND */
      bf_11stan,      /* MAND */
      bf_12time,      /* MAND */
      bf_13date,      /* MAND */
      bf_15datestl,   /* MAND */
      bf_22posentry,  /* MAND */
      bf_25poscc,     /* MAND */
      bf_32idcode,    /* MAND */
      bf_35track2,    /* COND */
      bf_36track3,    /* COND */
      bf_41termid,    /* COND */
      bf_42cardacc,   /* MAND */
      bf_48addldata,  /* MAND */
      bf_49trncurcd,  /* MAND */
      bf_52pinblock,  /* MAND */
      bf_102acct1,    /* COND */
      bf_103acct2,    /* COND */
      bf_128msg_authc,/* COND */
      bf_done      /* end of fields to move */
      },

      { 
      /********************************************
         R E L O A D   R E P E A T
      ********************************************/
      AUTH_RELOAD_REPEAT,
      0xF0,0xF2,0xF0,0xF1,                  /* message id, EBCDIC => 0201 */
      0xF2,0xF1,0xF0,0xF0,0xF0,0xF0,        /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,        /* MAND */
      bf_3proc,	    /* MAND */
      bf_4amount,     /* MAND */
      bf_7trntime,    /* MAND */
      bf_11stan,      /* MAND */
      bf_12time,      /* MAND */
      bf_13date,      /* MAND */
      bf_15datestl,   /* MAND */
      bf_22posentry,  /* MAND */
      bf_25poscc,     /* MAND */
      bf_32idcode,    /* MAND */
      bf_35track2,    /* COND */
      bf_36track3,    /* COND */
      bf_41termid,    /* COND */
      bf_42cardacc,   /* MAND */
      bf_48addldata,  /* MAND */
      bf_49trncurcd,  /* MAND */
      bf_52pinblock,  /* MAND */
      bf_102acct1,    /* COND */
      bf_103acct2,    /* COND */
      bf_128msg_authc,/* COND */
      bf_done      /* end of fields to move */
      },

      { 
      /********************************************
         R E L O A D   C O M F I R M   (COMPLETION)
      ********************************************/
      AUTH_RELOAD_CONFIRM,
      0xF0,0xF2,0xF0,0xF2,                  /* message id, EBCDIC => 0202 */
      0xF2,0xF1,0xF0,0xF0,0xF0,0xF0,        /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,        /* MAND */
      bf_3proc,	    /* MAND */
      bf_4amount,     /* MAND */
      bf_7trntime,    /* MAND */
      bf_11stan,      /* MAND */
      bf_12time,      /* MAND */
      bf_13date,      /* MAND */
      bf_15datestl,   /* MAND */
      bf_22posentry,  /* MAND */
      bf_25poscc,     /* MAND */
      bf_32idcode,    /* MAND */
      bf_35track2,    /* COND */
      bf_36track3,    /* COND */
      bf_41termid,    /* COND */
      bf_42cardacc,   /* MAND */
      bf_48addldata,  /* MAND */
      bf_49trncurcd,  /* MAND */
      bf_52pinblock,  /* MAND */
      bf_102acct1,    /* COND */
      bf_103acct2,    /* COND */
      bf_128msg_authc,/* COND */
      bf_done      /* end of fields to move */
      },

      { 
      /********************************************
         R E L O A D   V O I D
      ********************************************/
      AUTH_VOID_RELOAD,
      0xF0,0xF4,0xF2,0xF0,                  /* message id, EBCDIC => 0420 */
      0xF2,0xF1,0xF0,0xF0,0xF0,0xF0,        /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,         /* MAND */
      bf_3proc,        /* MAND */
      bf_4amount,      /* MAND */      
      bf_7trntime,     /* MAND */      
      bf_11stan,       /* MAND */
      bf_12time,       /* MAND */  
      bf_13date,       /* MAND */
      bf_15datestl,    /* MAND */
      bf_22posentry,   /* MAND */  
      bf_25poscc,      /* MAND */
      bf_32idcode,     /* MAND */
      bf_35track2,     /* MAND */
      bf_36track3,     /* MAND */
      bf_38authcode,   /* MAND */      
      bf_39respcode,   /* MAND */
      bf_41termid,     /* MAND */  
      bf_42cardacc,    /* MAND */
      bf_52pinblock,   /* MAND */
      bf_90orgdata,    /* MAND */
      bf_95repl_amt,   /* MAND */
      bf_128msg_authc, /* MAND */
      bf_done      /* end of fields to move */
      },

      {
      /********************************************
       A D M I N I S T R A T I V E  T E X T 
       ********************************************/
      AUTH_ADMINISTRATIVE_TEXT,
      0xF0,0xF6,0xF0,0xF0,                  /* message id, EBCDIC => 0200 */
      0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,        /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_7trntime,    /* MAND */
      bf_11stan,      /* MAND */
      bf_21fwdinstcntcode,
      bf_37rrn,
      bf_48addldata,
      bf_63priv63,
      bf_70nmicode,
      bf_100rcvid,
      bf_done      /* end of fields to move */
      },
      {
      /****************************************************
      A D M I N I S T R A T I V E  T E X T  R E S P O N S E
      ****************************************************/
      AUTH_ADMINISTRATIVE_TEXT_RESPONSE,
      0xF0,0xF6,0xF1,0xF0,                  /* message id, EBCDIC => 0610 */
      0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,        /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_7trntime,    /* MAND */
      bf_11stan,      /* MAND */
      bf_15datestl,
      bf_21fwdinstcntcode,
      bf_37rrn,
      bf_39respcode,
      bf_48addldata,  /* MAND */
      bf_63priv63,	
      bf_70nmicode,
      bf_100rcvid,
      bf_done      /* end of fields to move */
      },
	  { 
      /***************************
         C H A R G E B A C K 
      ******************************/
      AUTH_CHARGEBACK,
	  0xF0,0xF4,0xF2,0xF2,                  /* message id, EBCDIC => 0422 */
      0xF0,0xF2,0xF0,0xF0,0xF0,0xF0,        /* processing code */
      //0x04,0x22,              /* message id */
      //0x02,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,      /* MAND */
      bf_3proc,		/* MAND */
      bf_4amount,	/* MAND */
	  bf_5settamount, 
      bf_7trntime,  /* MAND */
	  bf_9settconvrate,
      bf_11stan,	/* MAND */
      bf_13date,     /* MAND */
	  bf_15datestl,
	  bf_16dateconv,
      bf_18mertype,/* MAND */
      bf_19acqinstcode,  /* MAND */
      bf_20countrycode, /* COND */	
	  bf_21fwdinstcntcode,
      bf_25poscc,	/* MAND */
      bf_28tranfeeamt,     /* COND */
      bf_32idcode,  /* MAND */
      bf_33fwdinstidcode, /* COND */
      bf_37rrn,     /* MAND */
      bf_38authcode, /* COND */
      bf_41termid,	/* MAND */
      bf_42cardacc,	/* MAND */
      bf_43cardacceptloc,
//	  bf_48addldata,  /* COND */
      bf_62priv62,  /* COND */
      bf_63priv63,  /* MAND */
      bf_90orgdata,  /* OPT*/
      bf_done      /* end of fields to move */
      },
      { 
      /******************************************
        C H A R G E B A C K    R E S P O N S E
      ********************************************/
      AUTH_CHARGEBACK_RESPONSE,
	  0xF0,0xF4,0xF3,0xF2,                  /* message id, EBCDIC => 0422 */
      0xF0,0xF2,0xF0,0xF0,0xF0,0xF0,        /* processing code */
    //  0x04,0x32,              /* message id */
     // 0x02,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,       /* MAND */
      bf_3proc,		/* MAND */
      bf_7trntime,   /* MAND */
      bf_11stan,	   /* MAND */
      bf_15datestl,  /* MAND */
      bf_19acqinstcode,   /* MAND */
      bf_25poscc,     /* MAND */
      bf_32idcode,   /* MAND */
      bf_37rrn,      /* MAND */
      bf_39respcode, /* MAND */
      bf_41termid,	/* MAND */
      bf_42cardacc,	/* MAND */
      bf_63priv63,   /* MAND */
      bf_90orgdata,  /* OPT*/
      bf_done      /* end of fields to move */
      },
      { 
      /*****************************************
         C H A R G E B A C K    R E V E R S A L
      *******************************************/
      AUTH_CHARGEBACK_REVERSAL,
	  0xF0,0xF4,0xF2,0xF2,                  /* message id, EBCDIC => 0422 */
      0xF0,0xF2,0xF0,0xF0,0xF0,0xF0,        /* processing code */
   //   0x04,0x22,              /* message id */
   //   0x02,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,      /* COND */
      bf_3proc,		/* MAND */
      bf_4amount,	/* MAND */
	  bf_5settamount,
      bf_7trntime,  /* MAND */
	  bf_9settconvrate,
      bf_11stan,	/* MAND */
      bf_13date,     /* MAND */
	  bf_15datestl,
	  bf_16dateconv,
      bf_18mertype,/* MAND */
      bf_19acqinstcode,  /* MAND */
      bf_20countrycode, /* COND */	
	  bf_21fwdinstcntcode,
      bf_25poscc,	/* MAND */
      bf_28tranfeeamt,     /* COND */
      bf_32idcode,  /* MAND */
      bf_33fwdinstidcode, /* COND */
      bf_37rrn,     /* MAND */
      bf_38authcode, /* COND */
      bf_41termid,	/* MAND */
      bf_42cardacc,	/* MAND */
      bf_43cardacceptloc,	/* MAND */
//	  bf_48addldata,  /* MAND */
      bf_63priv63,  /* MAND */
      bf_90orgdata,  /* OPT*/
      bf_done      /* end of fields to move */
      },
      { 
      /********************************************************
        C H A R G E B A C K  R E V E R S A L  R E S P O N S E
      ********************************************************/
      AUTH_CHARGEBACK_REVERSAL_RESPONSE,
	  0xF0,0xF4,0xF3,0xF2,                  /* message id, EBCDIC => 0422 */
      0xF0,0xF2,0xF0,0xF0,0xF0,0xF0,        /* processing code */
   //   0x04,0x32,              /* message id */
     // 0x02,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,       /* MAND */
      bf_3proc,		/* MAND */
      bf_7trntime,   /* MAND */
      bf_11stan,	   /* MAND */
      bf_15datestl,  /* MAND */
      bf_19acqinstcode,   /* MAND */
      bf_25poscc,     /* MAND */
      bf_32idcode,   /* MAND */
      bf_37rrn,      /* MAND */
      bf_39respcode, /* MAND */
      bf_41termid,   /* MAND */
      bf_42cardacc,	/* MAND */
      bf_63priv63,   /* MAND */
      bf_90orgdata,  /* OPT*/
      bf_done      /* end of fields to move */
      }

 };

#define N_TRANENTRY (sizeof(TRANTABLE) / sizeof(struct trandef))

#define N_BFData ( sizeof(BFData) / sizeof (struct bitfield_data) )

struct bitfield_data *Find_Bitfield_Table ( enum bitfields     );
struct trandef       *Find_Tran_Table     ( pAUTH_TX p_auth_tx );

struct iso_msg_rec
{
	BYTE msgtype       [4];
	BYTE bitmap1_bin   [16];
	BYTE bitmap2_bin   [16];
	BYTE pan_len       [2];
	BYTE pan           [19];
	BYTE proccode      [6];
	BYTE amount        [12];
	BYTE setl_amount   [12];
	BYTE gmt_time      [10];
	BYTE setl_conv_rate[8];
	BYTE stan          [6];
	BYTE time          [6];
	BYTE date          [4];
	BYTE exp_date		[4];
	BYTE date_settle   [4];
	BYTE date_conv     [4];
	BYTE merch_type	   [4];
	BYTE coun_code     [3];
	BYTE fwd_inst_cnt_code [3];
	BYTE posentry      [3];
	BYTE acq_inst_cnt_code [3];
	BYTE cardseqnum		[3];
	BYTE pos_condcd    [2];
	BYTE tran_fee_amt  [9];
	BYTE acq_inst_len  [2];
	BYTE acq_inst_id   [11];
	BYTE fwd_inst_len  [2];
	BYTE fwd_inst_id   [11];
	BYTE track2_len    [2];
	BYTE track2        [37];
	BYTE track3_len    [3];
	BYTE track3        [104];
	BYTE rrn           [12];
	BYTE authcode      [6];
	BYTE responsecd    [2];
	BYTE termid        [8];
	BYTE cardaccid     [15];
	BYTE cardaccname   [40];
	BYTE addl_rsp_len  [2];
	BYTE addlrspdata   [26];
	BYTE track1_len    [2];
	BYTE track1        [76];
	BYTE addl_data_len [3];
	BYTE addl_data     [70];
	BYTE tran_curr_cd  [3];
	BYTE pinblock      [16];
	BYTE ICCdata_len	[1];
	BYTE ICCdata	   [256];
	BYTE priv60_len    [1];
	BYTE priv60        [12];
	BYTE priv61_len    [1];
	BYTE priv61        [18];
	BYTE priv62_len    [3];
	BYTE priv62        [99];
	BYTE priv63_len    [2];  /* Changed the length from 3 to 2, Girija Y, TF */
	BYTE priv63        [99];
	BYTE nmi_code      [3];
	BYTE orig_data     [42];
	BYTE replace_amt   [42];
	BYTE rcvid_len     [2];
	BYTE rcvid         [11];
	BYTE acct1_len     [2];
	BYTE acct1         [28];
	BYTE acct2_len     [2];
	BYTE acct2         [28];
	BYTE trandesc_len  [3];
	BYTE trandesc      [512];/* DE104 */
	BYTE addl_trace_len[2];
	BYTE addl_trace      [50];/* DE115 */
	BYTE de126_len     [3];
	BYTE de126         [128];
	BYTE msg_authc     [16];
};
struct iso_msg_rec HOST_MESSAGE;

/********************************************************/
/* BIT SETTING TABLE                                    */
/********************************************************/
struct isobit 
   {
   INT byte_no;         /* byte number for this bit field */
   INT bit_mask;        /* bit to set in this byte        */
   };




/************************************************
  Bit Field Data Record
*************************************************/
struct bitfield_data 
   {
   enum bitfields  bd_fieldid;                           /* id for this field */
   pBYTE           p_bd_pdata;                           /* ptr to data to move */
   INT             bd_fieldlen;                          /* # of bytes in iso8583 field */
   INT             (*bd_BFCond)(enum bitfields, BYTE);   /* ptr to conditional function */
   INT             (*bd_BFMove)(enum bitfields);         /* ptr to move function */
   INT             (*bd_BFRsp)(enum bitfields);          /* ptr to response handler function */
   };

INT  vEBCDICMove(          enum bitfields );
INT  vEBCDICMove_F126(          enum bitfields );
INT  EBCDICMove(           enum bitfields );
INT  RspEBCDICvMove(       enum bitfields );
INT  RspEBCDICvMove_44 (enum bitfields );
INT  RspEBCDICMove(        enum bitfields );
INT  Move_IfThere( enum bitfields fieldid, BYTE p_trantype );
INT  Move_IfThere_44( enum bitfields fieldid, BYTE p_trantype );
INT  Move_Always(  enum bitfields fieldid, BYTE p_trantype );
INT  get_variable_length(  pBYTE, INT );
INT  GenericMove(enum bitfields fieldid);
INT  RspMoveGeneric(enum bitfields fieldid);
INT RspMovevData (enum bitfields fieldid);
INT  vDataMove(          enum bitfields );
INT RspEBCDICvMove_104 (enum bitfields fieldid);

static  struct bitfield_data BFData[] = {
   {
   bf_2pan,                      /* field */
   HOST_MESSAGE.pan_len,         /* data to move */
   2,                            /* # of bytes in pan len field */
   Move_IfThere,                 /* conditional */
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
   /* new */	   
   bf_5settamount,               /* field */
   HOST_MESSAGE.setl_amount,     /* data to move */
   12,                           /* # of bytes in  iso field */
   Move_IfThere,                  /* mandatory */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_7trntime,                  /* field */
   HOST_MESSAGE.gmt_time,        /* data to move */
   10,                           /* # of bytes in  iso field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   /* new */
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
   bf_12time,                    /* field */
   HOST_MESSAGE.time,            /* data to move */
   6,                            /* # of bytes in  iso field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_13date,                    /* field */
   HOST_MESSAGE.date,            /* data to move */
   4,                            /* # of bytes in  iso field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_14ExpDate,                    /* field */
   HOST_MESSAGE.exp_date,            /* data to move */
   4,                            /* # of bytes in  iso field */
   Move_IfThere,                  /* conditional */
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
   /* new */
   bf_16dateconv,                /* field */
   HOST_MESSAGE.date_conv,       /* data to move */
   4,                            /* # of bytes in  iso field */
   Move_IfThere,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   /* new */
   bf_18mertype,                 /* field */
   HOST_MESSAGE.merch_type,            /* data to move */
   4,                            /* # of bytes in  iso field */
   Move_IfThere,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   /* new */
   bf_19acqinstcode,             /* field */
   HOST_MESSAGE.acq_inst_cnt_code,/* data to move */
   3,                            /* # of bytes in  iso field */
   Move_IfThere,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   /* new */
   bf_20countrycode,                    /* field */
   HOST_MESSAGE.coun_code,            /* data to move */
   3,                            /* # of bytes in  iso field */
   Move_IfThere,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   /* new */
   bf_21fwdinstcntcode,                    /* field */
   HOST_MESSAGE.fwd_inst_cnt_code,            /* data to move */
   3,                            /* # of bytes in  iso field */
   Move_IfThere,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_22posentry,                /* field */
   HOST_MESSAGE.posentry,        /* data to move */
   3,                            /* # of bytes in  iso field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_23cardseqnum,              /* field */
   HOST_MESSAGE.cardseqnum,      /* data to move */
   3,                            /* # of bytes in visa iso field */
   Move_IfThere,               /* conditional */
   EBCDICMove,                  /* move routine */
   RspEBCDICMove                /* response routine */
   },
   {
   bf_25poscc,                   /* field */
   HOST_MESSAGE.pos_condcd,      /* data to move */
   2,                            /* # of bytes in  iso field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   /* new */
   bf_28tranfeeamt,              /* field */
   HOST_MESSAGE.tran_fee_amt,    /* data to move */
   9,                            /* # of bytes in  iso field */
   Move_IfThere,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_32idcode,                  /* field */
   HOST_MESSAGE.acq_inst_len,    /* data to move */
   2,                            /* # of bytes in  iso field */
   Move_IfThere,                  /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_33fwdinstidcode,           /* field */
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
   Move_IfThere,                  /* conditional - Changed to Cond - TF Phani */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_36track3,                  /* field */
   HOST_MESSAGE.track3_len,      /* data to move */
   3,                            /* # of bytes in track3 len field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_37rrn,                    /* field */
   HOST_MESSAGE.rrn,            /* data to move */
   12,                          /* # of bytes in track2 len field */
   Move_Always,                 /* conditional */
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
   2,                            /* # of bytes in  iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_41termid,                  /* field */
   HOST_MESSAGE.termid,          /* data to move */
   8,                            /* # of bytes in  iso field */
   Move_Always,                  /* mandatory */
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
   bf_43cardacceptloc,           /* field */
   HOST_MESSAGE.cardaccname,     /* data to move */
   40,                           /* # of bytes in  iso field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_44respdata,                /* field */
   HOST_MESSAGE.addl_rsp_len,        /* data to move */
   2,                            /* # of bytes in  iso field */
   Move_IfThere_44,                 /* conditional */
   vEBCDICMove,                   /* move routine */
   RspEBCDICvMove_44                 /* response routine */
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
   Move_IfThere,                 /* mandatory    */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_52pinblock,                /* field */
   HOST_MESSAGE.pinblock,        /* data to move */
   16,                           /* # of bytes in  iso field */
   Move_IfThere,                 /* conditional    */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
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
   bf_60priv60,                  /* field */
   HOST_MESSAGE.priv60,      /* data to move */
   12,                            /* # of bytes in priv60 len field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                     /* move routine */
   RspEBCDICMove                   /* response routine */
   },
   {
   bf_61priv61,                  /* field */
   HOST_MESSAGE.priv61_len,      /* data to move */
   3,                            /* # of bytes in priv61 len field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                     /* move routine */
   RspEBCDICvMove                   /* response routine */
   },
   {
   bf_62priv62,                  /* field */
   HOST_MESSAGE.priv62_len,      /* data to move */
   3,                            /* # of bytes in  iso field */
   Move_IfThere,                  /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_63priv63,                  /* field */
   HOST_MESSAGE.priv63_len,      /* data to move */
   2,                             /* Changed the length from 3 to 2, Girija Y, TF */
   Move_IfThere,                  /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_70nmicode,                 /* field */
   HOST_MESSAGE.nmi_code,        /* data to move */
   3,                            /* # of bytes in  iso field */
   Move_IfThere,                  /* mandatory    */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {  
   bf_90orgdata,                 /* field */
   HOST_MESSAGE.orig_data,       /* data to move */
   42,                           /* # of bytes in  iso field */
   Move_Always,                  /* mandatory    */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_95repl_amt,                /* field */
   HOST_MESSAGE.replace_amt,     /* data to move */
   42,                           /* # of bytes in  iso field */
   Move_IfThere,                  /* conditional  */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },  
   {
   bf_100rcvid,                  /* field */
   HOST_MESSAGE.rcvid_len,       /* data to move */
   2 ,                           /* # of bytes in  iso field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_102acct1,                  /* field */
   HOST_MESSAGE.acct1_len,       /* data to move */
   2 ,                           /* # of bytes in  iso field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_103acct2,                  /* field */
   HOST_MESSAGE.acct2_len,       /* data to move */
   2,                            /* # of bytes in  iso field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_104trandesc,               /* field */
   HOST_MESSAGE.trandesc_len,    /* data to move */
   3,                            /* # of bytes in track1 len field */
   Move_IfThere,                 /* conditional    */
   vDataMove,                    /* move routine */
   RspEBCDICvMove_104                  /* response routine */
   },
   {  
   bf_115addl_trace,               /* field */
   HOST_MESSAGE.addl_trace_len,    /* data to move */
   2,                            /* # of bytes in track1 len field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_126priv126,				 /* field */
   HOST_MESSAGE.de126_len,       /* data to move */
   3,                            /* # of bytes in  iso field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove_F126,             /* move routine */
   RspEBCDICvMove                 /* response routine */
   },
   {
   bf_128msg_authc,				 /* field */
   HOST_MESSAGE.msg_authc,       /* data to move */
   16,                            /* # of bytes in  iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
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






#endif
