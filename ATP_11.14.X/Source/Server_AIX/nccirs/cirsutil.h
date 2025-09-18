/******************************************************************************
 *
 *	CIRSUTIL.H
 *
 *  Copyright (c) 2003, Hypercom, Inc.
 *  All Rights Reserved.
 *
 *  TITLE:  Cirrus Utilities - Macros, Constants, and common structures
 *
 *  This header file is used by the utilities module of the Cirrus
 *  host handler application.
 *
 *  APPLICATION:  Advanced Transaction Processor (ATP)
 *
 *  AUTHOR:  Dennis Irby
 *
 *  REVISION HISTORY
 *
    $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\nccirrus\Cirsutil.h  $
   
      Rev 1.28   Jun 17 2005 11:41:54   chuang
   Version 4.1.1.4. Fix scr 15143, log reject reason code and 
   detail code into TLF01.vehicle_number. log issuer_id. Fix 
   parsing error of field 62.
   
      Rev 1.27   Apr 02 2003 14:16:28   dirby
   Added support for Issuer Reconciliation Advice responses.
   These are 0532 transactions.
   SCR 1072
   
      Rev 1.26   Apr 02 2003 10:46:58   dirby
   Modified bitfield 62 length field from 3 to 2.
   SCR 1073
   
      Rev 1.25   Apr 01 2003 14:51:16   dirby
   Added fields and transaction definitions necessary to support
   0422/0432 issuer reversal transactions from the host.
   SCR 1073
   
      Rev 1.24   Mar 26 2003 13:48:22   dirby
   Added bit 15 to the 0530 reconciliation response message.
   SCR 1070
   
      Rev 1.23   Mar 19 2003 20:30:30   dirby
   Modified to store settlement date in BCH10 (in the def_gross_amt
   field) to be used in reversals.
   SCR 1063
   
      Rev 1.22   Aug 21 2002 10:51:14   dirby
   Added field 48 to 0810 messages as a conditional field.  It is
   mandatory in 0810 key change transactions, but should not be
   in any other type of 0810 transaction.
   SCR 881
   
      Rev 1.21   Aug 07 2002 10:22:52   dirby
   Removed DE15 from 0200 transactions.
   SCR 864
   
      Rev 1.20   Aug 07 2000 14:53:08   dirby
   Made field 96 conditional.
   Corrected some indentation problems.
   
      Rev 1.19   Jan 27 2000 13:53:02   dirby
   Changed the condition function for DE2 from 'Move_IfThere' to 'CondPan'.
   CondPan checks against NULLs, Move_IfThere does not.
   
      Rev 1.18   Jan 23 2000 16:31:26   dirby
   Changed field 2 to be conditional.  It is required in
   0800 logons, but not in 0800 echo tests.
   
      Rev 1.17   23 Aug 1999 17:08:40   dirby
   Changed Store and Forwarding processing to use database table
   SAF01 instead of TLF01.
   Added some fields to NCF30 to support reversal processing.
   Fixed some typos.
   
      Rev 1.16   05 Aug 1999 13:26:54   dirby
   Changed bit field 54 to not be ignored.  This is where
   balance inquiry amounts come back.
   
      Rev 1.15   03 Aug 1999 13:33:28   dirby
   Added function prototype 'ascii_to_bin'.
   
      Rev 1.14   30 Jul 1999 14:37:56   dirby
   Changed name and prototype of find_cvc_flag function.
   Added a parameter to denote if field 48 contains a TCC.
   
      Rev 1.13   15 Jul 1999 14:46:26   dirby
   Added field 31.
   
      Rev 1.12   Jul 02 1999 09:11:22   svelaga
   Added some fileds and removed some fields from
   TRANSTABLE with message types 0200.
   For 0200:
   fields 12,13,33,43,52 are added. filed 14 is deleted
   
   
      Rev 1.11   28 Jun 1999 12:43:06   dirby
   Changed length of fields 28 and 29 from 8 to 9.
   
      Rev 1.10   Jun 23 1999 10:53:02   egable
   Correct the constant used for field 61.
   
      Rev 1.9   21 Jun 1999 16:36:38   dirby
   Renamed file name to start with lower case 'c'.
   
      Rev 1.8   Jun 15 1999 10:16:20   svelaga
   modified TRANTABLE array.
   Added some bit fields and deleted some bit fields
   for the elements with message type as 0220,0420.
   
      Rev 1.7   Jun 09 1999 12:08:30   svelaga
   Added some fileds and removed some fields from
   TRANSTABLE with message types 0200 and 0810.
   For 0200:
   fields 12,13,33,43,52 are added. filed 14 is deleted
   
   For 0810:
   Fields : 48 is deleted
   
   
      Rev 1.6   10 May 1999 16:22:06   dirby
   Added code to handle terminal Ids that are less than
   8 characters long.  This included modifying the way
   a timer lookup key is created.
   
      Rev 1.5   09 Apr 1999 15:43:22   dirby
   Removed function prototype for 'Rj_with_lead_zeros'.
   
      Rev 1.4   15 Mar 1999 16:28:20   dirby
   Added code to finish the requirement of pin key changes.
   
      Rev 1.3   12 Mar 1999 16:43:00   dirby
   Added functions to perform pin key exchange.
   Only the code to handle the initial request from the
   host is in place.  Still need to code the response.
   
      Rev 1.2   04 Mar 1999 16:47:14   dirby
   Updated for AIX port.
   
      Rev 1.1   03 Mar 1999 15:46:26   dirby
   Uncommented bit field 90.
   
      Rev 1.0   19 Feb 1999 16:19:56   dirby
   Initial version
 *
 ******************************************************************************/

#ifndef CIRSUTIL_H
#define CIRSUTIL_H

   

/* GLOBAL VARIABLES */


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

enum bitfields {
   bf_done           = 0,      /* end of list                          */
   bf_1xbitfield     = 1,      /* extended bit field                   */
   bf_2pan           = 2,      /* pan                                  */
   bf_3proc          = 3,      /* processing code                      */
   bf_4amount        = 4,      /* amount                               */
   bf_5settleamt     = 5,      /* settle amount                        */
   bf_6chbillgamt    = 6,      /* cardholder billing amount            */
   bf_7trntime       = 7,      /* transmission date-time               */
   bf_9convratesetl  = 9,      /* conversion rate, settlement          */
   bf_10convrate     = 10,     /* Conversion rate, cardholder billing  */
   bf_11stan         = 11,     /* system trace number                  */
   bf_12time         = 12,     /* time                                 */
   bf_13date         = 13,     /* date, trans                          */
   bf_14expiry       = 14,     /* expiry date                          */
   bf_15datestl      = 15,     /* settlement date                      */
   bf_16convdate     = 16,     /* conversion date                      */
   bf_18sicnumber    = 18,     /* Merchant's type of business          */
   bf_19ctrycd       = 19,     /* Acquiring institution's country code */
   bf_20panctrycd    = 20,     /* Identifies country of card issuer    */
   bf_22pose         = 22,     /* pos entry mode                       */
   bf_23cardseq      = 23,     /* card sequence number                 */
   bf_24nii          = 24,     /* nii                                  */
   bf_25pocc         = 25,     /* pos condition code                   */
   bf_26pincode      = 26,     /* pos pin capture code                 */
   bf_28tran_fee     = 28,     /* transaction fee amount               */
   bf_29setl_fee     = 29,     /* settlement fee amount                */
   bf_31setl_procfee = 31,     /* settlement processing fee amount     */
   bf_32idcode       = 32,     /* Acquiring institution's ID code      */
   bf_33fwdid        = 33,     /* Forwarding institution's ID code     */
   bf_35track2       = 35,     /* track 2 data                         */
   bf_37rrn          = 37,     /* retrieval reference number           */
   bf_38authcode     = 38,     /* auth code                            */
   bf_39respcode     = 39,     /* response code                        */
   bf_41termid       = 41,     /* terminal id                          */
   bf_42cardacc      = 42,     /* card acceptor id                     */
   bf_43cardaccloc   = 43,     /* card acceptor name / location        */
   bf_44respdata     = 44,     /* additional response data             */
   bf_45track1       = 45,     /* track 1 data                         */
   bf_48addldata     = 48,     /* additional data                      */
   bf_49trncurcd     = 49,     /* transaction currency code            */
   bf_50stlmtcurrcd  = 50,     /* settlement currency code             */
   bf_51crdcurcd     = 51,     /* card currency code                   */
   bf_52pinblock     = 52,     /* pin block                            */
   bf_54addlamts     = 54,     /* additional amounts                   */
   bf_60advicecode   = 60,     /* advice reason code                   */
   bf_61posdata      = 61,     /* POS data                             */
   bf_62inf          = 62,     /* Intermediate Network Facility data   */
   bf_63banknet      = 63,     /* BankNet data                         */
   bf_66setl_code    = 66,     /* Settlement Code                      */
   bf_70nmicode      = 70,     /* network management information code  */
   bf_74credit_cnt   = 74,     /* number of credits                    */
   bf_75crd_rev_cnt  = 75,     /* number of reversal credits           */
   bf_76debit_cnt    = 76,     /* number of debits                     */
   bf_77dbt_rev_cnt  = 77,     /* number of reversal debits            */
   bf_78trnfer_cnt   = 78,     /* number of transfers                  */
   bf_79trn_rev_cnt  = 79,     /* number of reversal transfers         */
   bf_80inquiry_cnt  = 80,     /* number of inquiries                  */
   bf_81auth_cnt     = 81,     /* number of authorizations             */
   bf_82crd_pr_fee   = 82,     /* processing fee amount for credits    */
   bf_83crd_tx_fee   = 83,     /* transaction fee amount for credits   */
   bf_84dbt_pr_fee   = 84,     /* processing fee amount for debits     */
   bf_85dbt_tx_fee   = 85,     /* transaction fee amount for debits    */
   bf_86credit_amt   = 86,     /* credit amount                        */
   bf_87crd_rev_amt  = 87,     /* credit reversal amount               */
   bf_88debit_amt    = 88,     /* debit amount                         */
   bf_89dbt_rev_amt  = 89,     /* debit reversal amount                */
   bf_90orgdata      = 90,     /* original data elements               */
   bf_94servind      = 94,     /* service indicator                    */
   bf_95repl_amts    = 95,     /* replacement amounts                  */
   bf_96securcode    = 96,     /* message security code                */
   bf_97net_setl     = 97,     /* net settlement amount                */
   bf_99setl_id      = 99,     /* settlement institution Id code       */
   bf_100recvid      = 100,    /* Receiving institution Id code        */
   bf_102acct1       = 102,    /* account identification 1             */
   bf_103acct2       = 103,    /* account identification 2             */
   bf_120orgmsgid    = 120,    /* original message type id             */
   bf_121mcagent     = 121,    /* authorizing agent id code            */
   bf_127privdata    = 127     /* private data                         */
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
         C A S H   A D V A N C E 
      ********************************************/
      AUTH_CASH_ADVANCE,
      0xF0,0xF2,0xF0,0xF0,             /* message id, EBCDIC => 0200 */
      0xF0,0xF1,0xF0,0xF0,0xF0,0xF0,   /* processing code - 01n000, n=0,1,2,3 */
                                       /*   n=0 -> no account specified       */
                                       /*   n=1 -> savings account            */
                                       /*   n=2 -> checking account           */
                                       /*   n=3 -> credit card / line         */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,        /* MAND */
      bf_3proc,       /* MAND */
      bf_4amount,     /* MAND */
      bf_7trntime,    /* MAND */
      bf_11stan,      /* MAND */
      bf_12time,      /* MAND */
      bf_13date,      /* MAND */
      bf_18sicnumber, /* COND */
      bf_22pose,      /* MAND */
      bf_32idcode,    /* MAND */
      bf_33fwdid,     /* MAND */
      bf_35track2,    /* MAND */
      bf_37rrn,       /* COND */
      bf_41termid,    /* MAND */
      bf_42cardacc,   /* MAND */
      bf_43cardaccloc,/* MAND */
      bf_49trncurcd,  /* MAND */
      bf_52pinblock,  /* MAND */
      bf_61posdata,   /* COND */
      bf_done      /* end of fields to move */
      },


      {  
      /********************************************
         B A L A N C E   I N Q U I R Y 
      ********************************************/
      AUTH_BALANCE_INQUIRY,
      0xF0,0xF2,0xF0,0xF0,             /* message id, EBCDIC => 0200 */
      0xF3,0xF0,0xF0,0xF0,0xF0,0xF0,   /* processing code - 30n000, n=0,1,2,3 */
                                       /*   n=0 -> no account specified       */
                                       /*   n=1 -> savings account            */
                                       /*   n=2 -> checking account           */
                                       /*   n=3 -> credit card / line         */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,        /* MAND */
      bf_3proc,       /* MAND */
      bf_4amount,     /* MAND */
      bf_7trntime,    /* MAND */
      bf_11stan,      /* MAND */
      bf_12time,      /* MAND */
      bf_13date,      /* MAND */
      bf_18sicnumber, /* COND */
      bf_22pose,      /* MAND */
      bf_32idcode,    /* MAND */
      bf_33fwdid,     /* MAND */
      bf_35track2,    /* MAND */
      bf_37rrn,       /* COND */
      bf_41termid,    /* MAND */
      bf_42cardacc,   /* MAND */
      bf_43cardaccloc,/* MAND */
      bf_49trncurcd,  /* MAND */
      bf_52pinblock,  /* MAND */
      bf_61posdata,   /* COND */
      bf_done      /* end of fields to move */
      },      

      {   
      /********************************************
         R E V E R S A L   A D V I C E
      ********************************************/
      AUTH_REVERSAL_ADVICE,
      0xF0,0xF4,0xF2,0xF0,             /* message id, EBCDIC => 0420      */
      0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,   /* processing code  (use original) */
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
      bf_23cardseq,    /* COND */
      bf_28tran_fee,   /* COND */
      bf_32idcode,     /* MAND */
      bf_33fwdid,      /* MAND */
      bf_37rrn,        /* COND */
      bf_38authcode,   /* COND */
      bf_39respcode,   /* MAND */
      bf_41termid,     /* MAND */
      bf_49trncurcd,   /* MAND */
      bf_60advicecode, /* MAND */
      bf_62inf,        /* COND */
      bf_63banknet,    /* MAND */
      bf_90orgdata,    /* MAND */
      bf_95repl_amts,  /* MAND */
      bf_done          /* end of fields to move */
      },

      {   
      /***************************************************************
         R E C O N C I L I A T I O N   A D V I C E   R E S P O N S E
      ****************************************************************/
      AUTH_RECONCILE_ADVICE_RESPONSE,
      0xF0,0xF5,0xF3,0xF0,             /* message id, EBCDIC => 0530 */
      0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,   /* processing code - none     */
      /**** OPTIONS ****/
      topt1_credit+topt1_postadv,
      topt2_org_proccode,     /* use original processing code */
      /**** BIT FIELDS ****/
      bf_7trntime,     /* MAND */
      bf_11stan,       /* MAND */
      bf_15datestl,    /* MAND */
      bf_33fwdid,      /* MAND */
      bf_39respcode,   /* MAND */
      bf_50stlmtcurrcd,/* MAND */
      bf_63banknet,    /* MAND */
      bf_66setl_code,  /* MAND */
      bf_97net_setl,   /* MAND */
      bf_99setl_id,    /* MAND */
      bf_100recvid,    /* MAND */
      bf_done          /* end of fields to move */
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
      bf_3proc,       /* MAND */
      bf_4amount,     /* MAND */
      bf_7trntime,    /* MAND */
      bf_11stan,      /* MAND */
      bf_12time,      /* MAND */
      bf_13date,      /* MAND */
      bf_18sicnumber, /* COND */
      bf_22pose,      /* MAND */
      bf_32idcode,    /* MAND */
      bf_33fwdid,     /* MAND */
      bf_35track2,    /* MAND */
      bf_37rrn,       /* COND */
      bf_41termid,    /* MAND */
      bf_42cardacc,   /* MAND */
      bf_43cardaccloc,/* MAND */
      bf_49trncurcd,  /* MAND */
      bf_52pinblock,  /* MAND */
      bf_61posdata,   /* COND */
      bf_done      /* end of fields to move */
      },

      {
      /********************************************
         R E F U N D 
      ********************************************/
      AUTH_REFUND,
      0xF0,0xF2,0xF0,0xF0,                  /* message id, EBCDIC => 0200 */
      0xF2,0xF0,0xF0,0xF0,0xF0,0xF0,        /* processing code */
      /**** OPTIONS ****/
      topt1_credit+topt1_postadv,
      topt2_reversal,
      /**** BIT FIELDS ****/
      bf_2pan,        /* MAND */
      bf_3proc,       /* MAND */
      bf_4amount,     /* MAND */
      bf_7trntime,    /* MAND */
      bf_11stan,      /* MAND */
      bf_12time,      /* MAND */
      bf_13date,      /* MAND */
      bf_18sicnumber, /* COND */
      bf_22pose,      /* MAND */
      bf_32idcode,    /* MAND */
      bf_33fwdid,     /* MAND */
      bf_35track2,    /* MAND */
      bf_37rrn,       /* COND */
      bf_41termid,    /* MAND */
      bf_42cardacc,   /* MAND */
      bf_43cardaccloc,/* MAND */
      bf_49trncurcd,  /* MAND */
      bf_52pinblock,  /* MAND */
      bf_61posdata,   /* COND */
      bf_done      /* end of fields to move */
      },

      {
	  /********************************************
         P A Y M E N T 
      ********************************************/
      AUTH_PAYMENT,
      0xF0,0xF2,0xF0,0xF0,             /* message id, EBCDIC => 0200 */
      0xF4,0xF0,0xF0,0xF0,0xF0,0xF0,   /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,        /* MAND */
      bf_3proc,       /* MAND */
      bf_4amount,     /* MAND */
      bf_7trntime,    /* MAND */
      bf_11stan,      /* MAND */
      bf_12time,      /* MAND */
      bf_13date,      /* MAND */
      bf_18sicnumber, /* COND */
      bf_22pose,      /* MAND */
      bf_32idcode,    /* MAND */
      bf_33fwdid,     /* MAND */
      bf_35track2,    /* MAND */
      bf_37rrn,       /* COND */
      bf_41termid,    /* MAND */
      bf_42cardacc,   /* MAND */
      bf_43cardaccloc,/* MAND */
      bf_49trncurcd,  /* MAND */
      bf_52pinblock,  /* MAND */
      bf_61posdata,   /* COND */
      bf_done      /* end of fields to move */
      },

      {
      /********************************************
         O F F L I N E   S A L E  
      ********************************************/
      AUTH_OFFLINE_SALE,
      0xF0,0xF2,0xF2,0xF0,             /* message id, EBCDIC => 0220 */
      0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,   /* processing code */
      /**** OPTIONS ****/
      topt1_credit+topt1_postadv,
      topt2_reversal,
      /**** BIT FIELDS ****/
      bf_2pan,        /* MAND */
      bf_3proc,       /* MAND */
      bf_4amount,     /* MAND */
      bf_7trntime,    /* MAND */
      bf_11stan,      /* MAND */
      bf_12time,      /* MAND */
      bf_13date,      /* MAND */
      bf_14expiry,    /* COND */
      bf_18sicnumber, /* MAND */
      bf_22pose,      /* MAND */
      bf_26pincode,   /* COND */
      bf_28tran_fee,  /* COND */
      bf_32idcode,    /* MAND */
      bf_33fwdid,     /* MAND */
      bf_35track2,    /* COND */
      bf_37rrn,       /* COND */
      bf_38authcode,  /* COND */
      bf_39respcode,  /* MAND */
      bf_41termid,    /* MAND */
      bf_42cardacc,   /* COND */
      bf_43cardaccloc,/* MAND */
      bf_44respdata,  /* COND */
      bf_49trncurcd,  /* MAND */
      bf_60advicecode,/* MAND */
      bf_61posdata,   /* COND */
      bf_62inf,       /* COND */
      bf_63banknet,   /* MAND */ 
      bf_90orgdata,   /* MAND */
      bf_95repl_amts, /* MAND */
      bf_102acct1,    /* COND */
      bf_103acct2,    /* COND */
      bf_done      /* end of fields to move */
      },

      {
      /********************************************
         O F F L I N E   R E F U N D 
      ********************************************/
      AUTH_OFFLINE_REFUND,
      0xF0,0xF2,0xF2,0xF0,            /* message id, EBCDIC => 0220 */
      0xF2,0xF0,0xF0,0xF0,0xF0,0xF0,  /* processing code */
      /**** OPTIONS ****/
      topt1_credit+topt1_postadv,
      topt2_reversal,
      /**** BIT FIELDS ****/
      bf_2pan,        /* MAND */
      bf_3proc,       /* MAND */
      bf_4amount,     /* MAND */
      bf_7trntime,    /* MAND */
      bf_11stan,      /* MAND */
      bf_12time,      /* MAND */
      bf_13date,      /* MAND */
      bf_14expiry,    /* COND */
      bf_18sicnumber, /* MAND */
      bf_22pose,      /* MAND */
      bf_26pincode,   /* COND */
      bf_28tran_fee,  /* COND */
      bf_32idcode,    /* MAND */
      bf_33fwdid,     /* MAND */
      bf_35track2,    /* COND */
      bf_37rrn,       /* COND */
      bf_38authcode,  /* COND */
      bf_39respcode,  /* MAND */
      bf_41termid,    /* MAND */
      bf_42cardacc,   /* COND */
      bf_43cardaccloc,/* MAND */
      bf_44respdata,  /* COND */
      bf_49trncurcd,  /* MAND */
      bf_60advicecode,/* MAND */
      bf_61posdata,   /* COND */
      bf_62inf,       /* COND */
      bf_63banknet,   /* MAND */ 
      bf_90orgdata,   /* MAND */
      bf_95repl_amts, /* MAND */
      bf_102acct1,    /* COND */
      bf_103acct2,    /* COND */
      bf_done      /* end of fields to move */
      },

      {
      /********************************************
         S A L E   A D J U S T M E N T   
      ********************************************/
      AUTH_SALE_ADJUSTMENT,
      0xF0,0xF2,0xF2,0xF0,           /* message id, EBCDIC => 0220 */
      0xF0,0xF2,0xF0,0xF0,0xF0,0xF0, /* processing code */
      /**** OPTIONS ****/
      topt1_credit+topt1_postadv,
      topt2_reversal,
      /**** BIT FIELDS ****/
      bf_2pan,        /* MAND */
      bf_3proc,       /* MAND */
      bf_4amount,     /* MAND */
      bf_7trntime,    /* MAND */
      bf_11stan,      /* MAND */
      bf_12time,      /* MAND */
      bf_13date,      /* MAND */
      bf_14expiry,    /* COND */
      bf_18sicnumber, /* MAND */
      bf_22pose,      /* MAND */
      bf_26pincode,   /* COND */
      bf_28tran_fee,  /* COND */
      bf_32idcode,    /* MAND */
      bf_33fwdid,     /* MAND */
      bf_35track2,    /* COND */
      bf_37rrn,       /* COND */
      bf_38authcode,  /* COND */
      bf_39respcode,  /* MAND */
      bf_41termid,    /* MAND */
      bf_42cardacc,   /* COND */
      bf_43cardaccloc,/* MAND */
      bf_44respdata,  /* COND */
      bf_49trncurcd,  /* MAND */
      bf_60advicecode,/* MAND */
      bf_61posdata,   /* COND */
      bf_62inf,       /* COND */
      bf_63banknet,   /* MAND */ 
      bf_90orgdata,   /* MAND */
      bf_95repl_amts, /* MAND */
      bf_102acct1,    /* COND */
      bf_103acct2,    /* COND */
      bf_done      /* end of fields to move */
      },

      {
      /********************************************
         R E F U N D    A D J U S T M E N T   
      ********************************************/
      AUTH_REFUND_ADJUSTMENT,
      0xF0,0xF2,0xF2,0xF0,             /* message id, EBCDIC => 0220 */
      0xF2,0xF2,0xF0,0xF0,0xF0,0xF0,   /* processing code */
      /**** OPTIONS ****/
      topt1_credit+topt1_postadv,
      topt2_reversal,
      /**** BIT FIELDS ****/
      bf_2pan,        /* MAND */
      bf_3proc,       /* MAND */
      bf_4amount,     /* MAND */
      bf_7trntime,    /* MAND */
      bf_11stan,      /* MAND */
      bf_12time,      /* MAND */
      bf_13date,      /* MAND */
      bf_14expiry,    /* COND */
      bf_18sicnumber, /* MAND */
      bf_22pose,      /* MAND */
      bf_26pincode,   /* COND */
      bf_28tran_fee,  /* COND */
      bf_32idcode,    /* MAND */
      bf_33fwdid,     /* MAND */
      bf_35track2,    /* COND */
      bf_37rrn,       /* COND */
      bf_38authcode,  /* COND */
      bf_39respcode,  /* MAND */
      bf_41termid,    /* MAND */
      bf_42cardacc,   /* COND */
      bf_43cardaccloc,/* MAND */
      bf_44respdata,  /* COND */
      bf_49trncurcd,  /* MAND */
      bf_60advicecode,/* MAND */
      bf_61posdata,   /* COND */
      bf_62inf,       /* COND */
      bf_63banknet,   /* MAND */ 
      bf_90orgdata,   /* MAND */
      bf_95repl_amts, /* MAND */
      bf_102acct1,    /* COND */
      bf_103acct2,    /* COND */
      bf_done      /* end of fields to move */
      },

      {   
      /********************************************
         P A Y M E N T    A D J U S T M E N T   
      ********************************************/
      AUTH_PAYMENT_ADJUSTMENT,
      0xF0,0xF2,0xF2,0xF0,             /* message id, EBCDIC => 0220 */
      0xF4,0xF2,0xF0,0xF0,0xF0,0xF0,   /* processing code */
      /**** OPTIONS ****/
      topt1_credit+topt1_postadv,
      topt2_reversal,
      /**** BIT FIELDS ****/
      bf_2pan,        /* MAND */
      bf_3proc,       /* MAND */
      bf_4amount,     /* MAND */
      bf_7trntime,    /* MAND */
      bf_11stan,      /* MAND */
      bf_12time,      /* MAND */
      bf_13date,      /* MAND */
      bf_14expiry,    /* COND */
      bf_18sicnumber, /* MAND */
      bf_22pose,      /* MAND */
      bf_26pincode,   /* COND */
      bf_28tran_fee,  /* COND */
      bf_32idcode,    /* MAND */
      bf_33fwdid,     /* MAND */
      bf_35track2,    /* COND */
      bf_37rrn,       /* COND */
      bf_38authcode,  /* COND */
      bf_39respcode,  /* MAND */
      bf_41termid,    /* MAND */
      bf_42cardacc,   /* COND */
      bf_43cardaccloc,/* MAND */
      bf_44respdata,  /* COND */
      bf_49trncurcd,  /* MAND */
      bf_60advicecode,/* MAND */
      bf_61posdata,   /* COND */
      bf_62inf,       /* COND */
      bf_63banknet,   /* MAND */ 
      bf_90orgdata,   /* MAND */
      bf_95repl_amts, /* MAND */
      bf_102acct1,    /* COND */
      bf_103acct2,    /* COND */
      bf_done      /* end of fields to move */ 
      },

      {
      /********************************************
         A U T H O R I Z A T I O N 
      ********************************************/
      AUTH_AUTHORIZATION,
      0xF0,0xF1,0xF0,0xF0,                  /* message id, EBCDIC => 0100 */
      0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,        /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,        /* MAND */
      bf_3proc,       /* MAND */
      bf_4amount,     /* MAND */
      bf_7trntime,    /* MAND */
      bf_11stan,      /* MAND */
      bf_14expiry,    /* COND */
      bf_18sicnumber, /* MAND */
      bf_22pose,      /* MAND */
      bf_32idcode,    /* MAND */
      bf_35track2,    /* COND */
      bf_37rrn,       /* MAND */
      bf_41termid,    /* COND */
      bf_42cardacc,   /* MAND */
      bf_45track1,    /* COND */
      bf_48addldata,  /* MAND */
      bf_49trncurcd,  /* MAND */
      bf_61posdata,   /* MAND */
      bf_done         /* end of fields to move */
      },

      {
      /***********************************************
         A U T H O R I Z A T I O N   R E S P O N S E
      ***********************************************/
      AUTH_AUTHORIZATION_RESPONSE,
      0xF0,0xF1,0xF1,0xF0,                  /* message id, EBCDIC => 0110 */
      0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,        /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,          /* MAND */
      bf_3proc,         /* MAND */
      bf_4amount,       /* MAND */
      bf_5settleamt,    /* COND */
      bf_6chbillgamt,   /* COND */
      bf_7trntime,      /* MAND */
      bf_9convratesetl, /* COND */
      bf_10convrate,    /* COND */
      bf_11stan,        /* MAND */
      bf_14expiry,      /* COND */
      bf_16convdate,    /* COND */
      bf_32idcode,      /* MAND */
      bf_33fwdid,       /* COND */
      bf_37rrn,         /* MAND */
      bf_38authcode,    /* COND */
      bf_39respcode,    /* MAND */
      bf_41termid,      /* COND */
      bf_48addldata,    /* COND */
      bf_49trncurcd,    /* MAND */
      bf_50stlmtcurrcd, /* COND */
      bf_51crdcurcd,    /* COND */
      bf_62inf,         /* COND */
      bf_63banknet,     /* COND */
      bf_120orgmsgid,   /* COND */
      bf_done           /* end of fields to move */
      },

      {
      /********************************************
         P R E   A U T H O R I Z A T I O N 
      ********************************************/
      AUTH_PRE_AUTHORIZATION,
      0xF0,0xF1,0xF0,0xF0,                  /* message id, EBCDIC => 0100 */
      0xF3,0xF0,0xF0,0xF0,0xF0,0xF0,        /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,        /* MAND */
      bf_3proc,       /* MAND */
      bf_4amount,     /* MAND */
      bf_7trntime,    /* MAND */
      bf_11stan,      /* MAND */
      bf_14expiry,    /* COND */
      bf_18sicnumber, /* MAND */
      bf_22pose,      /* MAND */
      bf_32idcode,    /* MAND */
      bf_35track2,    /* COND */
      bf_37rrn,       /* MAND */
      bf_41termid,    /* COND */
      bf_42cardacc,   /* MAND */
      bf_45track1,    /* COND */
      bf_48addldata,  /* MAND */
      bf_49trncurcd,  /* MAND */
      bf_61posdata,   /* MAND */
      bf_done      /* end of fields to move */
      },

      {  
      /********************************************
         V O I D   S A L E
      ********************************************/
      AUTH_VOID_SALE,
      0xF0,0xF2,0xF0,0xF0,                  /* message id, EBCDIC => 0200 */
      0xF0,0xF2,0xF0,0xF0,0xF0,0xF0,        /* processing code */
      /**** OPTIONS ****/
      topt1_credit+topt1_postadv,
      topt2_reversal,
      /**** BIT FIELDS ****/
      bf_2pan,        /* MAND */
      bf_3proc,       /* MAND */
      bf_4amount,     /* MAND */
      bf_7trntime,    /* MAND */
      bf_11stan,      /* MAND */
      bf_12time,      /* MAND */
      bf_13date,      /* MAND */
      bf_18sicnumber, /* COND */
      bf_22pose,      /* MAND */
      bf_32idcode,    /* MAND */
      bf_33fwdid,     /* MAND */
      bf_35track2,    /* MAND */
      bf_37rrn,       /* COND */
      bf_41termid,    /* MAND */
      bf_42cardacc,   /* MAND */
      bf_43cardaccloc,/* MAND */
      bf_49trncurcd,  /* MAND */
      bf_52pinblock,  /* MAND */
      bf_61posdata,   /* COND */
      bf_done      /* end of fields to move */
      },

      {
      /********************************************
         V O I D   R E F U N D
      ********************************************/
      AUTH_VOID_REFUND,
      0xF0,0xF2,0xF0,0xF0,                  /* message id, EBCDIC => 0200 */
      0xF2,0xF2,0xF0,0xF0,0xF0,0xF0,        /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_reversal,
      /**** BIT FIELDS ****/
      bf_2pan,        /* MAND */
      bf_3proc,       /* MAND */
      bf_4amount,     /* MAND */
      bf_7trntime,    /* MAND */
      bf_11stan,      /* MAND */
      bf_12time,      /* MAND */
      bf_13date,      /* MAND */
      bf_18sicnumber, /* COND */
      bf_22pose,      /* MAND */
      bf_32idcode,    /* MAND */
      bf_33fwdid,     /* MAND */
      bf_35track2,    /* MAND */
      bf_37rrn,       /* COND */
      bf_41termid,    /* MAND */
      bf_42cardacc,   /* MAND */
      bf_43cardaccloc,/* MAND */
      bf_49trncurcd,  /* MAND */
      bf_52pinblock,  /* MAND */
      bf_61posdata,   /* COND */
      bf_done      /* end of fields to move */
      },

      {
      /********************************************
         V O I D   P A Y M E N T 
      ********************************************/
      AUTH_VOID_PAYMENT,
      0xF0,0xF2,0xF0,0xF0,                  /* message id, EBCDIC => 0200 */
      0xF4,0xF2,0xF0,0xF0,0xF0,0xF0,        /* processing code */
      /**** OPTIONS ****/
      topt1_credit+topt1_postadv,
      topt2_reversal,
      /**** BIT FIELDS ****/
      bf_2pan,        /* MAND */
      bf_3proc,	      /* MAND */
      bf_4amount,     /* MAND */
      bf_7trntime,    /* MAND */
      bf_11stan,      /* MAND */
      bf_12time,      /* MAND */
      bf_13date,      /* MAND */
      bf_18sicnumber, /* COND */
      bf_22pose,      /* MAND */
      bf_32idcode,    /* MAND */
      bf_33fwdid,     /* MAND */
      bf_35track2,    /* MAND */
      bf_37rrn,       /* COND */
      bf_41termid,    /* MAND */
      bf_42cardacc,   /* MAND */
      bf_43cardaccloc,/* MAND */
      bf_49trncurcd,  /* MAND */
      bf_52pinblock,  /* MAND */
      bf_61posdata,   /* COND */
      bf_done      /* end of fields to move */
      },

      {
      /********************************************
         L O G O N 
      ********************************************/
      AUTH_LOGON,
      0xF0,0xF8,0xF0,0xF0,           /* message id, EBCDIC => 0800 */
      0xF9,0xF2,0xF0,0xF0,0xF0,0xF0, /* processing code */
      /**** OPTIONS ****/
      topt1_noacct+topt1_nocapture,
      topt2_noprerev,
      /**** BIT FIELDS ****/
      bf_2pan,       /* MAND */
      bf_7trntime,   /* MAND */
      bf_11stan,     /* MAND */
      bf_33fwdid,    /* MAND */
      bf_70nmicode,  /* MAND */
      bf_96securcode,/* COND */
      bf_done        /* end of fields to move */
      },

      {
      /********************************************
         L O G O N   R E S P O N S E
      ********************************************/
      AUTH_LOGON_RESPONSE,
      0xF0,0xF8,0xF1,0xF0,            /* message id, EBCDIC => 0810 */
      0xF9,0xF2,0xF0,0xF0,0xF0,0xF0,  /* processing code */
      /**** OPTIONS ****/
      topt1_noacct+topt1_nocapture,
      topt2_noprerev,
      /**** BIT FIELDS ****/
      bf_7trntime,   /* MAND */
      bf_11stan,     /* MAND */
      bf_33fwdid,    /* MAND */
      bf_39respcode, /* MAND */
      bf_44respdata, /* COND */
      bf_48addldata, /* COND */  /* Key Change: MAND; other 0810: not present */
      bf_63banknet,  /* COND */
      bf_70nmicode,  /* MAND */
      bf_done        /* end of fields to move */
      },

      {   
      /******************************************************************
         I S S U E R   R E V E R S A L   A D V I C E   R E S P O N S E
      *******************************************************************/
      AUTH_ISSUER_REVERSAL_ADVICE_RESPONSE,
      0xF0,0xF4,0xF3,0xF2,             /* message id, EBCDIC => 0432      */
      0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,   /* processing code  (use original) */
      /**** OPTIONS ****/
      topt1_credit+topt1_postadv,
      topt2_org_proccode,     /* use original processing code */
      /**** BIT FIELDS ****/
      bf_2pan,         /* MAND */
      bf_3proc,        /* MAND */
      bf_4amount,      /* MAND */
      bf_5settleamt,   /* COND */
      bf_7trntime,     /* MAND */
      bf_9convratesetl,/* COND */
      bf_11stan,       /* MAND */
      bf_12time,       /* MAND */
      bf_13date,       /* MAND */
      bf_15datestl,    /* MAND */
      bf_16convdate,   /* COND */
      bf_28tran_fee,   /* COND */
      bf_29setl_fee,   /* MAND */
      bf_32idcode,     /* MAND */
      bf_33fwdid,      /* MAND */
      bf_37rrn,        /* COND */
      bf_39respcode,   /* MAND */
      bf_41termid,     /* MAND */
      bf_49trncurcd,   /* MAND */
      bf_50stlmtcurrcd,/* COND */
      bf_62inf,        /* COND */
      bf_63banknet,    /* MAND */
      bf_95repl_amts,  /* MAND */
      bf_done          /* end of fields to move */
      },

      {   
      /*************************************************************************
       I S S U E R   R E C O N C I L I A T I O N   A D V I C E   R E S P O N S E
      *************************************************************************/
      AUTH_ISSUER_RECONCILE_ADVICE_RESPONSE,
      0xF0,0xF5,0xF3,0xF2,             /* message id, EBCDIC => 0530 */
      0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,   /* processing code - none     */
      /**** OPTIONS ****/
      topt1_credit+topt1_postadv,
      topt2_org_proccode,     /* use original processing code */
      /**** BIT FIELDS ****/
      bf_7trntime,     /* MAND */
      bf_11stan,       /* MAND */
      bf_15datestl,    /* MAND */
      bf_33fwdid,      /* MAND */
      bf_39respcode,   /* MAND */
      bf_50stlmtcurrcd,/* MAND */
      bf_63banknet,    /* MAND */
      bf_66setl_code,  /* MAND */
      bf_97net_setl,   /* MAND */
      bf_99setl_id,    /* MAND */
      bf_100recvid,    /* MAND */
      bf_done          /* end of fields to move */
      },

      {
      /********************************************
         A D M I N I S T R A T I V E   A D V I C E
      ********************************************/
      AUTH_ADMIN_ADVICE_RESPONSE,
      0xF0,0xF6,0xF3,0xF0,            /* message id, EBCDIC => 0630 */
      0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,  /* processing code */
      /**** OPTIONS ****/
      topt1_noacct+topt1_nocapture,
      topt2_noprerev,
      /**** BIT FIELDS ****/
      bf_7trntime,  /* MAND */
      bf_11stan,    /* MAND */
      bf_33fwdid,   /* MAND */
      bf_39respcode,/* MAND */
      bf_63banknet, /* MAND */
      bf_100recvid, /* MAND */
      bf_done       /* end of fields to move */
      },

      {
      /********************************************
         T E S T   T R A N
      ********************************************/
      AUTH_TEST,
      0xF0,0xF8,0xF0,0xF0,            /* message id, EBCDIC => 0800 */
      0xF9,0xF9,0xF0,0xF0,0xF0,0xF0,  /* processing code */
      /**** OPTIONS ****/
      topt1_noacct+topt1_nocapture,
      topt2_noprerev,
      /**** BIT FIELDS ****/
      bf_3proc,
      bf_24nii,
      bf_41termid,
      bf_done      /* end of fields to move */
      }
   };

#define N_TRANENTRY (sizeof(TRANTABLE) / sizeof(struct trandef))

#define N_BFData ( sizeof(BFData) / sizeof (struct bitfield_data) )

#ifdef USE_ORIGINAL
	struct bitfield_data *Find_Bitfield_Table ( enum bitfields     );
	struct trandef       *Find_Tran_Table     ( pAUTH_TX p_auth_tx );
#else
	static struct bitfield_data *Find_Bitfield_Table ( enum bitfields     );
	static struct trandef       *Find_Tran_Table     ( pAUTH_TX p_auth_tx );

#endif
/* Host message data structure. */
struct iso_msg_rec
{
        BYTE msgtype       [4];
        BYTE bitmap1_bin   [8];
        BYTE bitmap2_bin   [8];
        BYTE pan_len       [2];
        BYTE pan           [19];
        BYTE proccode      [6];
        BYTE amount        [12];
        BYTE settle_amt    [12];
        BYTE ch_bill_amt   [12];
        BYTE gmt_time      [10];
        BYTE conv_rate_stl [8];
        BYTE conv_rate_chb [8];
        BYTE stan          [6];
        BYTE time          [6];
        BYTE date          [4];
        BYTE date_expd     [4];
        BYTE date_settle   [4];
        BYTE conv_date     [4];
        BYTE sic_number    [4];
        BYTE ctry_code     [3];
        BYTE pan_ctry_cd   [3];
        BYTE posentry      [3];
        BYTE card_seq_num  [3];
        BYTE nii           [2];
        BYTE pos_condcd    [1];
        BYTE pos_pincode   [2];
        BYTE tran_fee      [9];
        BYTE setl_fee      [9];
        BYTE setl_procfee  [9];
        BYTE acq_inst_len  [2];
        BYTE acq_inst_id   [11];
        BYTE fwd_inst_len  [2];
        BYTE fwd_inst_id   [11];
        BYTE track2_len    [2];
        BYTE track2        [37];
        BYTE rrn           [12];
        BYTE authcode      [6];
        BYTE responsecd    [2];
        BYTE termid        [8];
        BYTE cardaccid     [15];
        BYTE cardaccloc    [40];
        BYTE addl_rsp_len  [2];
        BYTE addlrspdata   [25];
        BYTE track1_len    [2];
        BYTE track1        [76];
        BYTE addl_data_len [3];
        BYTE addl_data     [100];
        BYTE tran_curr_cd  [3];
        BYTE settle_cur_cd [3];
        BYTE card_curr_cd  [3];
        BYTE pinblock      [8];
        BYTE addl_len      [3];
        BYTE addlamts      [120];
        BYTE advice_cd_len [3];
        BYTE advice_code   [60];
        BYTE pos_data_len  [3];
        BYTE pos_data      [26];
        BYTE inf_data_len  [3];
        BYTE inf_data      [100];
        BYTE banknet_len   [3];
        BYTE banknet       [50];
        BYTE setl_code     [1];
        BYTE nmi_code      [3];
        BYTE credit_cnt    [10];
        BYTE crd_rev_cnt   [10];
        BYTE debit_cnt     [10];
        BYTE dbt_rev_cnt   [10];
        BYTE trnfer_cnt    [10];
        BYTE trn_rev_cnt   [10];
        BYTE inquiry_cnt   [10];
        BYTE auth_cnt      [10];
        BYTE crd_pr_fee    [12];
        BYTE crd_tx_fee    [12];
        BYTE dbt_pr_fee    [12];
        BYTE dbt_tx_fee    [12];
        BYTE credit_amt    [16];
        BYTE crd_rev_amt   [16];
        BYTE debit_amt     [16];
        BYTE dbt_rev_amt   [16];
        BYTE orig_data     [42];
        BYTE service_ind   [7];
        BYTE replace_amts  [42];
        BYTE security_code [8];
        BYTE net_setl_code [1];
        BYTE net_setl      [16];
        BYTE setl_id_len   [2];
        BYTE setl_id       [11];
        BYTE recv_len      [2];
        BYTE recvid        [11];
        BYTE acct1_len     [2];
        BYTE acct1         [28];
        BYTE acct2_len     [2];
        BYTE acct2         [28];
        BYTE orig_msg_id   [1];
        BYTE mc_agent_len  [3];
        BYTE mc_agent_id   [11];
        BYTE priv127_len   [3];
        BYTE priv127       [100];
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
   INT             (*bd_BFCond)(enum bitfields, BYTE);  /* ptr to conditional function */
   void            (*bd_BFMove)(enum bitfields);         /* ptr to move function */
   void            (*bd_BFRsp)(enum bitfields);          /* ptr to response handler function */
   };

/* FUNCTION PROTOTYPES */
#ifdef USE_ORIGINAL
BYTE ascii_ebcdic_conv( unsigned char, char * );
void ascii_to_bin( CHAR *, INT , BYTE *, INT );
void vEBCDICMove(    enum bitfields );
void EBCDICMove(     enum bitfields );
void RspEBCDICvMove( enum bitfields );
void RspEBCDICMove(  enum bitfields );
void RspIgnore(      enum bitfields );
void vRspIgnore(     enum bitfields );
void RspEBCDICvMove_NoLen( enum bitfields );
INT  CondProc(       enum bitfields, BYTE );
INT  CondExpd(       enum bitfields, BYTE );
INT  CondPan(        enum bitfields, BYTE );
INT  CondTrack1(     enum bitfields, BYTE );
INT  CondDE96(       enum bitfields, BYTE );
INT  Move_Always(    enum bitfields, BYTE );
INT  Move_IfThere(   enum bitfields, BYTE );
INT  Move_Never(     enum bitfields, BYTE );
INT  get_variable_length( pBYTE, INT );
void find_cvc_flag( pCHAR, pCHAR, pCHAR, INT );
#else
	BYTE ascii_ebcdic_conv( unsigned char, char * );
	void ascii_to_bin( CHAR *, INT , BYTE *, INT );
	void vEBCDICMove(    enum bitfields );
	void EBCDICMove(     enum bitfields );
	void RspEBCDICvMove( enum bitfields );
	void RspEBCDICMove(  enum bitfields );
	void RspIgnore(      enum bitfields );
	void vRspIgnore(     enum bitfields );
	void RspEBCDICvMove_NoLen( enum bitfields );
	static INT  CondProc(       enum bitfields, BYTE );
	INT  CondExpd(       enum bitfields, BYTE );
	INT  CondPan(        enum bitfields, BYTE );
	INT  CondTrack1(     enum bitfields, BYTE );
	INT  CondDE96(       enum bitfields, BYTE );
	INT  Move_Always(    enum bitfields, BYTE );
	INT  Move_IfThere(   enum bitfields, BYTE );
	static INT  Move_Never(     enum bitfields, BYTE );
	static INT  get_variable_length( pBYTE, INT );
	void find_cvc_flag( pCHAR, pCHAR, pCHAR, INT );
#endif
void GenericMove(enum bitfields fieldid);
void RspMoveGeneric(enum bitfields fieldid);
void trim_spaces( pBYTE );


static  struct bitfield_data BFData[] = {
   {
   bf_2pan,                      /* field */
   HOST_MESSAGE.pan_len,         /* data to move */
   2,                            /* # of bytes in pan len field */
   CondPan,                      /* mandatory    */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_3proc,                     /* field */
   HOST_MESSAGE.proccode,        /* data to move */
   6,                            /* # of bytes in cirrus iso field */
   Move_Always,                  /* mandatory */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_4amount,                   /* field */
   HOST_MESSAGE.amount,          /* data to move */
   12,                           /* # of bytes in cirrus iso field */
   Move_Always,                  /* mandatory */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_5settleamt,                /* field */
   HOST_MESSAGE.settle_amt,      /* data to move */
   12,                           /* # of bytes in cirrus iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_6chbillgamt,               /* field */
   HOST_MESSAGE.ch_bill_amt,     /* data to move */
   12,                           /* # of bytes in cirrus iso field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspIgnore                     /* response routine */
   },
   {
   bf_7trntime,                  /* field */
   HOST_MESSAGE.gmt_time,        /* data to move */
   10,                           /* # of bytes in cirrus iso field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_9convratesetl,             /* field */
   HOST_MESSAGE.conv_rate_stl,   /* data to move */
   8,                            /* # of bytes in cirrus iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_10convrate,                /* field */
   HOST_MESSAGE.conv_rate_chb,   /* data to move */
   8,                            /* # of bytes in cirrus iso field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspIgnore                     /* response routine */
   },
   {
   bf_11stan,                    /* field */
   HOST_MESSAGE.stan,            /* data to move */
   6,                            /* # of bytes in cirrus iso field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_12time,                    /* field */
   HOST_MESSAGE.time,            /* data to move */
   6,                            /* # of bytes in cirrus iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_13date,                    /* field */
   HOST_MESSAGE.date,            /* data to move */
   4,                            /* # of bytes in cirrus iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_14expiry,                  /* field */
   HOST_MESSAGE.date_expd,       /* data to move */
   4,                            /* # of bytes in cirrus iso field */
   CondExpd,                     /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_15datestl,                 /* field */
   HOST_MESSAGE.date_settle,      /* data to move */
   4,                            /* # of bytes in cirrus iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_16convdate,                /* field */
   HOST_MESSAGE.conv_date,       /* data to move */
   4,                            /* # of bytes in cirrus iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_18sicnumber,               /* field */
   HOST_MESSAGE.sic_number,      /* data to move */
   4,                            /* # of bytes in cirrus iso field */
   Move_Always,                  /* mandatory    */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_19ctrycd,                  /* field */
   HOST_MESSAGE.ctry_code,       /* data to move */
   3,                            /* # of bytes in cirrus iso field */
   Move_Always,                  /* mandatory    */
   EBCDICMove,                   /* move routine */
   RspIgnore                     /* response routine */
   },
   {
   bf_20panctrycd,               /* field */
   HOST_MESSAGE.conv_date,       /* data to move */
   3,                            /* # of bytes in cirrus iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspIgnore                     /* response routine */
   },
   {
   bf_22pose,                    /* field */
   HOST_MESSAGE.posentry,        /* data to move */
   3,                            /* # of bytes in cirrus iso field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_23cardseq,                 /* field */
   HOST_MESSAGE.card_seq_num,    /* data to move */
   3,                            /* # of bytes in cirrus iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspIgnore                     /* response routine */
   },
   {
   bf_24nii,                     /* field */
   HOST_MESSAGE.nii,             /* data to move */
   3,                            /* # of bytes in cirrus iso field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspIgnore                     /* response routine */
   },
   {
   bf_25pocc,                    /* field */
   HOST_MESSAGE.pos_condcd,      /* data to move */
   2,                            /* # of bytes in cirrus iso field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspIgnore                     /* response routine */
   },
   {
   bf_26pincode,                 /* field */
   HOST_MESSAGE.pos_pincode,     /* data to move */
   2,                            /* # of bytes in cirrus iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspIgnore                     /* response routine */
   },
   {
   bf_28tran_fee,                /* field */
   HOST_MESSAGE.tran_fee,        /* data to move */
   9,                            /* # of bytes in cirrus iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_29setl_fee,                /* field */
   HOST_MESSAGE.setl_fee,        /* data to move */
   9,                            /* # of bytes in cirrus iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_31setl_procfee,            /* field */
   HOST_MESSAGE.setl_procfee,    /* data to move */
   9,                            /* # of bytes in cirrus iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspIgnore                     /* response routine */
   },
   {
   bf_32idcode,                  /* field */
   HOST_MESSAGE.acq_inst_len,    /* data to move */
   2,                            /* # of bytes in cirrus iso field */
   Move_Always,                  /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_33fwdid,                   /* field */
   HOST_MESSAGE.fwd_inst_len,    /* data to move */
   2,                            /* # of bytes in cirrus iso field */
   Move_Always,                  /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_35track2,                  /* field */
   HOST_MESSAGE.track2_len,      /* data to move */
   2,                            /* # of bytes in track2 len field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_37rrn,                     /* field */
   HOST_MESSAGE.rrn,             /* data to move */
   12,                           /* # of bytes in cirrus iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_38authcode,                /* field */
   HOST_MESSAGE.authcode,        /* data to move */
   6,                            /* # of bytes in cirrus iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_39respcode,                /* field */
   HOST_MESSAGE.responsecd,      /* data to move */
   2,                            /* # of bytes in cirrus iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_41termid,                  /* field */
   HOST_MESSAGE.termid,          /* data to move */
   8,                            /* # of bytes in cirrus iso field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_42cardacc,                 /* field */
   HOST_MESSAGE.cardaccid,       /* data to move */
   15,                           /* # of bytes in cirrus iso field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_43cardaccloc,              /* field */
   HOST_MESSAGE.cardaccloc,      /* data to move */
   40,                           /* # of bytes in cirrus iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspIgnore                     /* response routine */
   },
   {
   bf_44respdata,                /* field */
   HOST_MESSAGE.addl_rsp_len,    /* data to move */
   2,                            /* # of bytes in resp data len field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_45track1,                  /* field */
   HOST_MESSAGE.track1_len,      /* data to move */
   2,                            /* # of bytes in track1 len field */
   CondTrack1,                   /* conditional */
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
   3,                            /* # of bytes in cirrus iso field */
   Move_Always,                  /* mandatory    */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_50stlmtcurrcd,             /* field */
   HOST_MESSAGE.settle_cur_cd,   /* data to move */
   3,                            /* # of bytes in cirrus iso field */
   Move_IfThere,                 /* conditional  */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_51crdcurcd,                /* field */
   HOST_MESSAGE.card_curr_cd,    /* data to move */
   3,                            /* # of bytes in cirrus iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspIgnore                     /* response routine */
   },
   {
   bf_52pinblock,                /* field */
   HOST_MESSAGE.pinblock,       /* data to move */
   8,                            /* # of bytes in cirrus iso field */
   Move_IfThere,                 /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_54addlamts,                /* field */
   HOST_MESSAGE.addl_len,        /* data to move */
   3,                            /* # of bytes in addl len field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_60advicecode,              /* field */
   HOST_MESSAGE.advice_cd_len,   /* data to move */
   3,                            /* # of bytes in addl len field */
   Move_Always,                  /* mandatory */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_61posdata,                 /* field */
   HOST_MESSAGE.pos_data_len,    /* data to move */
   3,                            /* # of bytes in addl len field */
   Move_Always,                  /* mandatory */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_62inf,                     /* field */
   HOST_MESSAGE.inf_data_len,    /* data to move */
   3,                            /* # of bytes in addl len field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_63banknet,                 /* field */
   HOST_MESSAGE.banknet_len,     /* data to move */
   3,                            /* # of bytes in addl len field */
   Move_IfThere,                 /* conditional  */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_66setl_code,               /* field */
   HOST_MESSAGE.setl_code,       /* data to move */
   1,                            /* # of bytes in cirrus iso field */
   Move_Always,                  /* mandatory    */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_70nmicode,                 /* field */
   HOST_MESSAGE.nmi_code,        /* data to move */
   3,                            /* # of bytes in cirrus iso field */
   Move_Always,                  /* mandatory    */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_74credit_cnt,              /* field */
   HOST_MESSAGE.credit_cnt,      /* data to move */
   10,                           /* # of bytes in cirrus iso field */
   Move_IfThere,                 /* conditional  */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_75crd_rev_cnt,             /* field */
   HOST_MESSAGE.crd_rev_cnt,     /* data to move */
   10,                           /* # of bytes in cirrus iso field */
   Move_IfThere,                 /* conditional  */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_76debit_cnt,               /* field */
   HOST_MESSAGE.debit_cnt,       /* data to move */
   10,                           /* # of bytes in cirrus iso field */
   Move_IfThere,                 /* conditional  */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_77dbt_rev_cnt,             /* field */
   HOST_MESSAGE.dbt_rev_cnt,     /* data to move */
   10,                           /* # of bytes in cirrus iso field */
   Move_IfThere,                 /* conditional  */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_78trnfer_cnt,              /* field */
   HOST_MESSAGE.trnfer_cnt,      /* data to move */
   10,                           /* # of bytes in cirrus iso field */
   Move_IfThere,                 /* conditional  */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_79trn_rev_cnt,             /* field */
   HOST_MESSAGE.trn_rev_cnt,      /* data to move */
   10,                           /* # of bytes in cirrus iso field */
   Move_IfThere,                 /* conditional  */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_80inquiry_cnt,             /* field */
   HOST_MESSAGE.inquiry_cnt,     /* data to move */
   10,                           /* # of bytes in cirrus iso field */
   Move_IfThere,                 /* conditional  */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_81auth_cnt,                /* field */
   HOST_MESSAGE.auth_cnt,        /* data to move */
   10,                           /* # of bytes in cirrus iso field */
   Move_IfThere,                 /* conditional  */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_82crd_pr_fee,              /* field */
   HOST_MESSAGE.crd_pr_fee,      /* data to move */
   12,                           /* # of bytes in cirrus iso field */
   Move_IfThere,                 /* conditional  */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_83crd_tx_fee,              /* field */
   HOST_MESSAGE.crd_tx_fee,      /* data to move */
   12,                           /* # of bytes in cirrus iso field */
   Move_IfThere,                 /* conditional  */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_84dbt_pr_fee,              /* field */
   HOST_MESSAGE.dbt_pr_fee,      /* data to move */
   12,                           /* # of bytes in cirrus iso field */
   Move_IfThere,                 /* conditional  */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_85dbt_tx_fee,              /* field */
   HOST_MESSAGE.dbt_tx_fee,      /* data to move */
   12,                           /* # of bytes in cirrus iso field */
   Move_IfThere,                 /* conditional  */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_86credit_amt,              /* field */
   HOST_MESSAGE.credit_amt,      /* data to move */
   16,                           /* # of bytes in cirrus iso field */
   Move_IfThere,                 /* conditional  */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_87crd_rev_amt,             /* field */
   HOST_MESSAGE.crd_rev_amt,     /* data to move */
   16,                           /* # of bytes in cirrus iso field */
   Move_IfThere,                 /* conditional  */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_88debit_amt,               /* field */
   HOST_MESSAGE.debit_amt,       /* data to move */
   16,                           /* # of bytes in cirrus iso field */
   Move_IfThere,                 /* conditional  */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_89dbt_rev_amt,             /* field */
   HOST_MESSAGE.dbt_rev_amt,     /* data to move */
   16,                           /* # of bytes in cirrus iso field */
   Move_IfThere,                 /* conditional  */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_90orgdata,                 /* field */
   HOST_MESSAGE.orig_data,       /* data to move */
   42,                           /* # of bytes in cirrus iso field */
   Move_Always,                  /* mandatory    */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_94servind,                 /* field */
   HOST_MESSAGE.service_ind,     /* data to move */
   7,                            /* # of bytes in cirrus iso field */
   Move_Always,                  /* mandatory    */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_95repl_amts,               /* field */
   HOST_MESSAGE.replace_amts,    /* data to move */
   42,                           /* # of bytes in cirrus iso field */
   Move_Always,                  /* mandatory    */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_96securcode,               /* field */
   HOST_MESSAGE.security_code,   /* data to move */
   8,                            /* # of bytes in cirrus iso field */
   CondDE96,                     /* conditional    */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_97net_setl,                /* field */
   HOST_MESSAGE.net_setl_code,   /* data to move */
   17,                           /* # of bytes in cirrus iso field */
   Move_Always,                  /* mandatory    */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_99setl_id,                 /* field */
   HOST_MESSAGE.setl_id_len,     /* data to move */
   2 ,                           /* # of bytes in cirrus iso field */
   Move_Always,                  /* mandatory    */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_100recvid,                 /* field */
   HOST_MESSAGE.recv_len,        /* data to move */
   2 ,                           /* # of bytes in cirrus iso field */
   Move_IfThere,                 /* mandatory    */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_102acct1,                  /* field */
   HOST_MESSAGE.acct1_len,       /* data to move */
   2 ,                           /* # of bytes in cirrus iso field */
   Move_Always,                  /* mandatory    */
   vEBCDICMove,                  /* move routine */
   vRspIgnore                    /* response routine */
   },
   {
   bf_103acct2,                  /* field */
   HOST_MESSAGE.acct2_len,       /* data to move */
   2,                            /* # of bytes in cirrus iso field */
   Move_Always,                  /* mandatory    */
   vEBCDICMove,                  /* move routine */
   vRspIgnore                    /* response routine */
   },
   {
   bf_120orgmsgid,               /* field */
   HOST_MESSAGE.orig_msg_id,     /* data to move */
   3,                            /* # of bytes in cirrus iso field */
   Move_IfThere,                 /* conditional  */
   vEBCDICMove,                  /* move routine */
   vRspIgnore                    /* response routine */
   },
   {
   bf_121mcagent,                /* field */
   HOST_MESSAGE.mc_agent_len,    /* data to move */
   3,                            /* # of bytes in cirrus iso field */
   Move_IfThere,                 /* conditional  */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_127privdata,               /* field */
   HOST_MESSAGE.priv127_len,     /* data to move */
   3,                            /* # of bytes in cirrus iso field */
   Move_IfThere,                 /* conditional  */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },

   {
   bf_done,                      /* field */
   NULL,                         /* data to move */
   99,                           /* # of bytes in cirrus iso field */
   NULL,                         /* conditional */
   NULL,                         /* move routine */
   NULL                          /* response routine */
   }
}; /* end BFData */


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
#define bf12_len      6
#define bf13_len      4
#define bf14_len      4
#define bf15_len      4
#define bf16_len      4
#define bf18_len      4
#define bf20_len      3
#define bf22_len      3
#define bf28_len      9
#define bf29_len      9
#define bf32_len      2
#define bf32data_len 11
#define bf33_len      2
#define bf33data_len 11
#define bf35_len      2
#define bf37_len     12
#define bf38_len      6
#define bf39_len      2
#define bf41_len      8
#define bf42_len     15
#define bf43_len     40
#define bf44_len      2
#define bf45_len      2
#define bf48_len      3
#define bf49_len      3
#define bf50_len      3
#define bf51_len      3
#define bf52_len      8
#define bf54_len      3
#define bf60_len      3
#define bf61_len      3
#define bf62_len      3
#define bf63_len      3
#define bf70_len      3
#define bf74_len     10
#define bf75_len     10
#define bf76_len     10
#define bf77_len     10
#define bf86_len     16
#define bf87_len     16
#define bf88_len     16
#define bf89_len     16
#define bf90_len     42
#define bf94_len      7
#define bf96_len      8
#define bf97_len     16
#define bf99_len      2
#define bf100_len     2
#define bf102_len     2
#define bf103_len     2
#define bf120_len     3
#define bf121_len     3
#define bf127_len     3


#define CIRRUS_POS_DATA_LEN  "016"
#define CIRRUS_POS_DATA      "1010010001201608"

#endif
