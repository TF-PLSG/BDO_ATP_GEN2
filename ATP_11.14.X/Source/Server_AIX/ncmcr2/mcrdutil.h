/******************************************************************************

   MCRDUTIL.H

   Copyright (c) 2007, Hypercom, Inc.
   All Rights Reserved.

   TITLE:  MasterCard Utilities - Macros, Constants, and common structures

   This header file is used by the utilities module of the MasterCard
   host handler application.

   APPLICATION:  Advanced Transaction Processor (ATP)


 ******************************************************************************/


/* NCMCRD quename , all NC MC should start with ncmcrd,ncmcr2,ncmcr3...etc
*  This will used in handler queue validation */
#define NCMCRD "ncmcr"  
#define NC_CHK_LEN 5	 
#define BDOR_62_PREAUTH_VOID 1


#define PRIMARY_AUTH 		'P'
#define INCREMENTAL_AUTH 	'I'
#define ESTIMATED_AUTH 		'E'
#define PARTIAL_REVERSAL 	'R'

#define BDOR_68_REQ_1_MC 1
#define BDOR_68_REQ_3_MC 1
#define BDOR_68_REQ_5_MC 1

#define POS_ENTRY_MODE_LEN 							    			4
#define POS_MOBILE_NFC_TAP2PHONE_TXN_WITH_PIN_PEM 					"0831"
#define POS_MOBILE_NFC_TAP2PHONE_TXN_WITHOUT_PIN_PEM 				"0832"
#define POS_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITH_PIN_PEM				"0031"
#define POS_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITHOUT_PIN_PEM			"0032"
#define POS_ECOMMERCE_ENTRY_MODE_FROM_SWIFTPASS_BDOPAY_TXN_PEM		"0012"

#define MC_DE22_PEM_LEN												3
#define MC_MOBILE_NFC_TAP2PHONE_TXN_WITH_PIN 						"073"
#define MC_MOBILE_NFC_TAP2PHONE_TXN_WITHOUT_PIN						"072"
#define MC_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITH_PIN_PEM 				"031"
#define MC_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITHOUT_PIN_PEM				"032"
#define MC_ECOMMERCE_SWIFTPASS_BDOPAY_TXN_PIN_PEM					"812"

#define BDO_POS_ACQ_TXN_HANDLER_QUEUE								"dcp"
#define BDO_SBATCH_TXN_HANDLER_QUEUE								"sbatch"
#define BDO_POS_ACQ_TXN_HANDLE_QUEUE_LEN							3

#define POS_ACQ_SALE_MESSAGE_TYPE 									"0200"
#define POS_ACQ_AUTH_MESSAGE_TYPE 									"0100"
#define POS_ACQ_REVERSAL_MESSAGE_TYPE 								"0400"
#define POS_ACQ_REVERSAL_ADVICE_MESSAGE_TYPE						"0420"
#define ISO_MESSAGE_TYPE_LEN										4

#define MC_CARDHOLDER_ACTIVATED_TERMINAL_LEVEL						'9'

#define MC_POS_DATA_LEN												16
#define MC_SWIFTPASS_BDO_PAY_CREDIT_TXN_POS_DATA					"0000000000300608"
#define MC_ECOMMERCE_SWIFTPASS_BDOPAY_TXN_POS_DATA					"1025100006600608"

#define POS_CONDITION_CODE_LEN										2
#define POS_ECOM_CONDITION_CODE										"59"

#define N_TRANENTRY (sizeof(TRANTABLE) / sizeof(struct trandef))
#define N_BFData ( sizeof(BFData) / sizeof (struct bitfield_data) )

struct bitfield_data *Find_Bitfield_Table ( enum bitfields     );
struct trandef       *Find_Tran_Table     ( pAUTH_TX p_auth_tx );


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
   bf_23csn          = 23,     /* Card Sequence Number                 */
   bf_24nii          = 24,     /* nii                                  */
   bf_25pocc         = 25,     /* pos condition code                   */
   bf_26pincode      = 26,     /* pos pin capture code                 */
   bf_28feeamount	 = 28,	   /* Txx fee amount					   */ 
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
   bf_53seccntl	     = 53,	   /* Security Related control information */
   bf_54addlamts     = 54,     /* additional amounts                   */
   bf_55iccdata      = 55,     /* .Integrated Circuit Card (ICC) System-related Data*/
   bf_56ISOuse       =56,
   bf_60advicecode   = 60,     /* advice reason code                   */
   bf_61posdata      = 61,     /* POS data                             */
   bf_62inf          = 62,     /* Intermediate Network Facility data   */
   bf_63banknet      = 63,     /* BankNet data                         */
   bf_70nmicode      = 70,     /* network management information code  */
   bf_73actiondate   = 73,     /* Date, action                         */
   bf_90orgdata      = 90,     /* original data elements               */
   bf_91filecode     = 91,     /* file update code                     */
   bf_94servind      = 94,     /* service indicator                    */
   bf_95repl_amt     = 95,     /* replacement amount                   */
   bf_96securcode    = 96,     /* message security code                */
   bf_100recvid      = 100,    /* Receiving institution Id code        */
   bf_101filename    = 101,    /* file name                            */
   bf_102acct1       = 102,    /* account identification 1             */
   bf_103acct2       = 103,    /* account identification 2             */
   bf_104trandesc    = 104,    /* Transaction Description         		*/
   bf_105tranlink    = 105,	/* transaction link identifier 			*/
   bf_108moneySend   = 108,    /* Money send data 	*/ 
   bf_112addldata	 = 112,
   bf_119privateuse2 = 119,	   /* Additional data-Private use2			*/
   bf_120orgmsgid    = 120,    /* original message type id             */
   bf_121mcagent     = 121,    /* authorizing agent id code            */
   bf_122addlrec     = 122,    /* additional record data               */
   bf_124memData      = 124,    /* additional record data               */
   bf_127privdata    = 127     /* private data                         */
   }; 

/* LENGTHs FOR EACH BIT FIELD */
#define bf2_len       2
#define bf3_len       6
#define bf4_len       12
#define bf5_len       12
#define bf6_len       12
#define bf7_len       10
#define bf9_len       8
#define bf10_len      8
#define bf11_len      6
#define bf12_len      6
#define bf13_len      4
#define bf14_len      4
#define bf15_len      4
#define bf16_len      4
#define bf18_len      4
#define bf19_len      3
#define bf20_len      3
#define bf22_len      3
#define bf23_len      3
#define bf32_len      2
#define bf32data_len  11
#define bf33_len      2
#define bf33data_len  11
#define bf35_len      2
#define bf37_len      12
#define bf38_len      6
#define bf39_len      2
#define bf41_len      8
#define bf42_len      15
#define bf43_len      40
#define bf44_len      2
#define bf45_len      2
#define bf48_len      3
#define bf49_len      3
#define bf50_len      3
#define bf51_len      3
#define bf52_len      8
#define bf53_len	  16
#define bf54_len      3
#define bf55_len      3
#define bf56_len      3
#define bf60_len      3
#define bf61_len      3
#define bf62_len      3
#define bf63_len      3
#define bf70_len      3
#define bf90_len      42
#define bf94_len      7
#define bf96_len      8
#define bf100_len     2
#define bf102_len     2
#define bf103_len     2
#define bf120_len     3
#define bf121_len     3
#define bf127_len     3


/* DE feilds */
#define ID_MOBILE_POS_ACCP_DEVICES 9 /*Identification of Mobile POS Acceptance Devices*/

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
         S A L E
      ********************************************/
      AUTH_SALE,
      0xF0,0xF1,0xF0,0xF0,                  /* message id, EBCDIC => 0100 */
      0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,        /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,        /* MAND */
      bf_3proc,       /* MAND */
      bf_4amount,     /* MAND */
	  bf_5settleamt,  /* COND */
	  bf_6chbillgamt, /* COND */
      bf_7trntime,    /* MAND */
	  bf_9convratesetl,/* COND */
      bf_10convrate,   /* COND */
      bf_11stan,      /* MAND */
      bf_12time,      /* OPT  */
      bf_13date,      /* OPT  */
      bf_14expiry,    /* COND */
	  bf_15datestl,   /* COND */
      bf_16convdate,  /* COND */
      bf_18sicnumber, /* MAND */
      bf_22pose,      /* MAND */
      bf_23csn,       /* COND */
	  bf_26pincode,
      bf_32idcode,    /* MAND */
      bf_33fwdid,     /* OPT  */
      bf_35track2,    /* COND */
      bf_37rrn,       /* MAND */
      bf_41termid,    /* COND */
      bf_42cardacc,   /* MAND */
      bf_43cardaccloc,/* MAND */
      bf_45track1,    /* COND */
      bf_48addldata,  /* MAND */
      bf_49trncurcd,  /* MAND */
	  bf_50stlmtcurrcd, /* COND */
	  bf_51crdcurcd,   /* COND */
      bf_52pinblock,  /* COND */
	  bf_53seccntl,		/* COND */
      bf_55iccdata,   /* COND */
      bf_61posdata,   /* MAND */
	  bf_63banknet,  /* COND */
	  bf_104trandesc,
	  bf_105tranlink,
      bf_done      /* end of fields to move */
      },
      	{

	  /********************************************
         QUASI CASH
      ********************************************/
      AUTH_QUASI_CASH,
      0xF0,0xF1,0xF0,0xF0,                  /* message id, EBCDIC => 0100 */
      0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,        /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,        /* MAND */
      bf_3proc,       /* MAND */
      bf_4amount,     /* MAND */
	  bf_5settleamt,  /* COND */
	  bf_6chbillgamt, /* COND */
      bf_7trntime,    /* MAND */
	  bf_9convratesetl,/* COND */
      bf_10convrate,   /* COND */
      bf_11stan,      /* MAND */
      bf_12time,      /* OPT  */
      bf_13date,      /* OPT  */
      bf_14expiry,    /* COND */
	  bf_15datestl,   /* COND */
      bf_16convdate,  /* COND */
      bf_18sicnumber, /* MAND */
      bf_22pose,      /* MAND */
      bf_23csn,       /* COND */
	  bf_26pincode,
      bf_32idcode,    /* MAND */
      bf_33fwdid,     /* OPT  */
      bf_35track2,    /* COND */
      bf_37rrn,       /* MAND */
      bf_41termid,    /* COND */
      bf_42cardacc,   /* MAND */
      bf_43cardaccloc,/* MAND */
      bf_45track1,    /* COND */
      bf_48addldata,  /* MAND */
      bf_49trncurcd,  /* MAND */
	  bf_50stlmtcurrcd, /* COND */
	  bf_51crdcurcd,   /* COND */
      bf_52pinblock,  /* COND */
	  bf_53seccntl,		/* COND */
      bf_55iccdata,   /* COND */
      bf_61posdata,   /* MAND */
	  bf_63banknet,  /* COND */
	  bf_104trandesc,
      bf_done      /* end of fields to move */
      },
	  /* BDO wants to send Void and Reversal Txn to Candeince host(09-76)  TF-Ajay 02 Nov,2009 */
	   { 
      /********************************************
         R E V E R S A L
      ********************************************/
      AUTH_REVERSAL,
      0xF0,0xF4,0xF0,0xF0,                  /* message id, EBCDIC => 0400 */
      0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,        /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
	  bf_2pan,        /* MAND */
      bf_3proc,       /* MAND */
      bf_4amount,     /* MAND */
      bf_6chbillgamt,     /* COND */
      bf_7trntime,    /* MAND */
      bf_9convratesetl,/* COND */
      bf_10convrate,   /* COND */
	  bf_11stan,      /* MAND */
   	  bf_12time,      /* COND */
      bf_13date,      /* COND */
      bf_14expiry,    /* COND */
	  bf_15datestl,       /* COND */
	  bf_16convdate,	  /* MAND */
	  bf_18sicnumber, /* MAND */
	  bf_19ctrycd,    /*MAND*/
      bf_22pose,      /* MAND */
	  bf_26pincode,
	  bf_32idcode,    /* MAND */
	  bf_33fwdid,     /* COND  */
      bf_37rrn,       /* COND */
      bf_38authcode,  /* COND */
      bf_39respcode,  /* MAND */
      bf_41termid,    /* COND */
      bf_42cardacc,   /* COND */
      bf_43cardaccloc,/* COND */
      bf_48addldata,  /* MAND */
      bf_49trncurcd,  /* MAND */
	  bf_50stlmtcurrcd,   /* COND */
      bf_51crdcurcd,      /* COND */
      bf_61posdata,   /* MAND */
	  bf_62inf,           /* COND */
      bf_63banknet,		  /* MAND */
      bf_90orgdata,   /* MAND */
	  bf_95repl_amt,  /* COND */
	  bf_105tranlink,
	  bf_108moneySend,
	  bf_121mcagent,	  /* COND */
	  bf_124memData,
      bf_done      /* end of fields to move */
      },

      {
      /********************************************
	          R E V E R S A L  R E S P O N S E
	       ********************************************/
	       AUTH_REVERSAL_RESPONSE,
	       0xF0,0xF4,0xF1,0xF0,                  /* message id, EBCDIC => 0410 */
	       0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,        /* processing code */
	       /**** OPTIONS ****/
	       topt1_postadv,
	       topt2_referral+topt2_reversal+topt2_floor,
	       /**** BIT FIELDS ****/
	       bf_2pan,         /* MAND */
	       bf_3proc,        /* MAND */
	       bf_4amount,      /* MAND */
	 	  bf_5settleamt,   /* COND */
	       bf_6chbillgamt,  /* COND */
	       bf_7trntime,     /* MAND */
	 	  bf_9convratesetl,/* COND */
	       bf_10convrate,   /* COND */
	 	  bf_11stan,       /* MAND */
	 	  bf_15datestl,    /* MAND */
	       bf_16convdate,   /* COND */
	       bf_20panctrycd,  /* COND */
	 	  bf_32idcode,     /* MAND */
	 	  bf_33fwdid,      /* COND */
	       bf_37rrn,        /* COND */
	 	  bf_38authcode,  /* COND */
	       bf_39respcode,   /* MAND */
	       bf_41termid,     /* COND */
	 	  bf_42cardacc,    /* COND */
	       bf_48addldata,   /* MAND */
	       bf_49trncurcd,   /* MAND */
	       bf_50stlmtcurrcd,/* COND */
	       bf_51crdcurcd,   /* COND */
	       bf_62inf,        /* COND */
	       bf_63banknet,    /* MAND */
	       bf_90orgdata,    /* MAND */
	 	  bf_95repl_amt,   /* COND */
		  bf_105tranlink,
	 	  bf_127privdata,  /* COND */
	       bf_done      /* end of fields to move */
	       },
#if BDOR_62_PREAUTH_VOID
		   {
		         /********************************************
		           AUTH VOID PRE AUTHORIZATION
		         ********************************************/
				 AUTH_VOID_PRE_AUTHORIZATION,
		         0xF0,0xF4,0xF0,0xF0,                  /* message id, EBCDIC => 0400 */
		         0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,        /* processing code */
		         /**** OPTIONS ****/
		         topt1_postadv,
		         topt2_referral+topt2_reversal+topt2_floor,
		         /**** BIT FIELDS ****/
					bf_2pan,        /* MAND */
					bf_3proc,       /* MAND */
					bf_4amount,     /* MAND */
					bf_6chbillgamt,     /* COND */
					bf_7trntime,    /* MAND */
					bf_9convratesetl,/* COND */
					bf_10convrate,   /* COND */
					bf_11stan,      /* MAND */
					bf_12time,      /* COND */
					bf_13date,      /* COND */
					bf_14expiry,    /* COND */
					bf_15datestl,       /* COND */
					bf_16convdate,	  /* MAND */
					bf_18sicnumber, /* MAND */
					bf_22pose,      /* MAND */
					bf_26pincode,
					bf_32idcode,    /* MAND */
					bf_33fwdid,     /* COND  */
					bf_37rrn,       /* COND */
					bf_38authcode,  /* COND */
					bf_39respcode,  /* MAND */
					bf_41termid,    /* COND */
					bf_42cardacc,   /* COND */
					bf_43cardaccloc,/* COND */
					bf_48addldata,  /* MAND */
					bf_49trncurcd,  /* MAND */
					bf_50stlmtcurrcd,   /* COND */
					bf_51crdcurcd,      /* COND */
					bf_61posdata,   /* MAND */
					bf_62inf,           /* COND */
					bf_63banknet,		  /* MAND */
					bf_90orgdata,   /* MAND */
					bf_95repl_amt,  /* COND */
					bf_105tranlink,
					bf_108moneySend,
					bf_121mcagent,	  /* COND */
					bf_124memData,
					bf_done      /* end of fields to move */
		   },
		      {
		      /********************************************
			         AUTH VOID PRE AUTHORIZATION RESPONSE
			       ********************************************/
		    	   AUTH_VOID_PRE_AUTHORIZATION_RESPONSE,
			       0xF0,0xF4,0xF1,0xF0,                  /* message id, EBCDIC => 0410 */
			       0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,        /* processing code */
			       /**** OPTIONS ****/
			       topt1_postadv,
			       topt2_referral+topt2_reversal+topt2_floor,
			       /**** BIT FIELDS ****/
			       bf_2pan,         /* MAND */
			       bf_3proc,        /* MAND */
			       bf_4amount,      /* MAND */
			 	  bf_5settleamt,   /* COND */
			       bf_6chbillgamt,  /* COND */
			       bf_7trntime,     /* MAND */
			 	  bf_9convratesetl,/* COND */
			       bf_10convrate,   /* COND */
			 	  bf_11stan,       /* MAND */
			 	  bf_15datestl,    /* MAND */
			       bf_16convdate,   /* COND */
			       bf_20panctrycd,  /* COND */
			 	  bf_32idcode,     /* MAND */
			 	  bf_33fwdid,      /* COND */
			       bf_37rrn,        /* COND */
			 	  bf_38authcode,  /* COND */
			       bf_39respcode,   /* MAND */
			       bf_41termid,     /* COND */
			 	  bf_42cardacc,    /* COND */
			       bf_48addldata,   /* MAND */
			       bf_49trncurcd,   /* MAND */
			       bf_50stlmtcurrcd,/* COND */
			       bf_51crdcurcd,   /* COND */
			       bf_62inf,        /* COND */
			       bf_63banknet,    /* MAND */
			       bf_90orgdata,    /* MAND */
			 	  bf_95repl_amt,   /* COND */
				  bf_105tranlink,
			 	  bf_127privdata,  /* COND */
			       bf_done      /* end of fields to move */
			       },
#endif
      {
      /********************************************
         R E F U N D 
      ********************************************/
      AUTH_REFUND,
      0xF0,0xF1,0xF0,0xF0,                  /* message id, EBCDIC => 0100 */
      0xF2,0xF0,0xF0,0xF0,0xF0,0xF0,        /* processing code */
      /**** OPTIONS ****/
      topt1_credit+topt1_postadv,
      topt2_reversal,
      /**** BIT FIELDS ****/
      bf_2pan,        /* MAND */
      bf_3proc,       /* MAND */
	  bf_5settleamt,  /* COND */
	  bf_6chbillgamt, /* COND */
      bf_4amount,     /* MAND */
      bf_7trntime,    /* MAND */
      bf_11stan,      /* MAND */
      bf_12time,      /* OPT  */
      bf_13date,      /* OPT  */
      bf_14expiry,    /* COND */
      bf_18sicnumber, /* MAND */
      bf_22pose,      /* MAND */
      bf_23csn,       /* COND */
      bf_32idcode,    /* MAND */
      bf_33fwdid,     /* OPT  */
      bf_35track2,    /* COND */
      bf_37rrn,       /* MAND */
      bf_41termid,    /* COND */
      bf_42cardacc,   /* MAND */
      bf_43cardaccloc,/* MAND */
      bf_45track1,    /* COND */
      bf_48addldata,  /* MAND */
      bf_49trncurcd,  /* MAND */
      bf_52pinblock,  /* COND */
      bf_55iccdata,   /* COND */
      bf_61posdata,   /* MAND */
      bf_63banknet,
      bf_done      /* end of fields to move */
      },
      {
      /********************************************
         Auth Refund
      ********************************************/
      AUTH_PRE_AUTHORIZATION_CANCLE,
      0xF0,0xF1,0xF0,0xF0,                  /* message id, EBCDIC => 0100 */
      0xF2,0xF0,0xF0,0xF0,0xF0,0xF0,        /* processing code */
      /**** OPTIONS ****/
      topt1_credit+topt1_postadv,
      topt2_reversal,
      /**** BIT FIELDS ****/
      bf_2pan,        /* MAND */
      bf_3proc,       /* MAND */
	  bf_5settleamt,  /* COND */
	  bf_6chbillgamt, /* COND */
      bf_4amount,     /* MAND */
      bf_7trntime,    /* MAND */
      bf_11stan,      /* MAND */
      bf_12time,      /* OPT  */
      bf_13date,      /* OPT  */
      bf_14expiry,    /* COND */
      bf_18sicnumber, /* MAND */
      bf_22pose,      /* MAND */
      bf_23csn,       /* COND */
      bf_32idcode,    /* MAND */
      bf_33fwdid,     /* OPT  */
      bf_35track2,    /* COND */
      bf_37rrn,       /* MAND */
      bf_41termid,    /* COND */
      bf_42cardacc,   /* MAND */
      bf_43cardaccloc,/* MAND */
      bf_45track1,    /* COND */
      bf_48addldata,  /* MAND */
      bf_49trncurcd,  /* MAND */
      bf_52pinblock,  /* COND */
      bf_55iccdata,   /* COND */
      bf_61posdata,   /* MAND */
      bf_63banknet,
	  bf_104trandesc,
      bf_done      /* end of fields to move */
      },

	  {
		/********************************************
		   Authorization return
		********************************************/
		AUTH_PRE_AUTHORIZATION_CANCLE_RESPONSE,
		0xF0,0xF1,0xF1,0xF0,                  /* message id, EBCDIC => 0100 */
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
		bf_12time,      /* OPT  */
		bf_13date,      /* OPT  */
		bf_14expiry,    /* COND */
		bf_18sicnumber, /* MAND */
		bf_22pose,      /* MAND */
		bf_23csn,       /* COND */
		bf_32idcode,    /* MAND */
		bf_33fwdid,     /* OPT  */
		bf_35track2,    /* COND */
		bf_37rrn,       /* MAND */
		bf_38authcode,  /* COND */
		bf_39respcode,
		bf_41termid,    /* COND */
		bf_42cardacc,   /* MAND */
		bf_43cardaccloc,/* MAND */
		bf_45track1,    /* COND */
		bf_48addldata,  /* MAND */
		bf_49trncurcd,  /* MAND */
		bf_52pinblock,  /* COND */
		bf_55iccdata,   /* COND */
		bf_61posdata,   /* MAND */
		bf_done      /* end of fields to move */
		},
      {
	  /********************************************
         P A Y M E N T 
      ********************************************/
      AUTH_PAYMENT,
      0xF0,0xF1,0xF0,0xF0,             /* message id, EBCDIC => 0200 */
      0xF4,0xF0,0xF0,0xF0,0xF0,0xF0,   /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,        /* MAND */
      bf_3proc,	      /* MAND */
      bf_4amount,     /* MAND */
	  bf_5settleamt,  /* COND */
	  bf_6chbillgamt, /* COND */
      bf_7trntime,    /* MAND */
      bf_11stan,      /* MAND */
      bf_12time,      /* OPT  */
      bf_13date,      /* OPT  */
      bf_14expiry,    /* COND */
      bf_18sicnumber, /* MAND */
      bf_22pose,      /* MAND */
      bf_23csn,       /* COND */
      bf_32idcode,    /* MAND */
      bf_33fwdid,     /* OPT  */
      bf_35track2,    /* COND */
      bf_37rrn,       /* MAND */
      bf_41termid,    /* COND */
      bf_42cardacc,   /* MAND */
      bf_43cardaccloc,/* MAND */
      bf_45track1,    /* COND */
      bf_48addldata,  /* MAND */
      bf_49trncurcd,  /* MAND */
      bf_52pinblock,  /* COND */
	  bf_53seccntl,		/* COND */
      bf_55iccdata,   /* COND */
      bf_61posdata,   /* MAND */
      bf_63banknet,
	  bf_104trandesc,
      bf_done      /* end of fields to move */
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
      bf_5settleamt,   /* COND */
      bf_6chbillgamt,  /* COND */
      bf_7trntime,     /* MAND */
      bf_9convratesetl,/* COND */
      bf_10convrate,   /* COND */
      bf_11stan,       /* MAND */
      bf_15datestl,    /* MAND */
      bf_16convdate,   /* COND */
      bf_32idcode,     /* MAND */
      bf_33fwdid,      /* COND */
      bf_37rrn,        /* COND */
      bf_39respcode,   /* MAND */
	  bf_49trncurcd,   /* MAND */
      bf_50stlmtcurrcd,/* COND */
      bf_51crdcurcd,   /* COND */
      bf_62inf,        /* COND */
      bf_63banknet,    /* MAND */
	  bf_105tranlink,
      bf_127privdata,  /* COND */
      bf_done          /* end of fields to move */
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
      bf_55iccdata,   /* COND */
      bf_61posdata,   /* MAND */
      bf_63banknet,
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
      bf_55iccdata,   /* COND */
      bf_61posdata,   /* MAND */
	  bf_63banknet,
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
      bf_55iccdata,   /* COND */
      bf_61posdata,   /* MAND */
      bf_63banknet,
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
      bf_55iccdata,   /* COND */
      bf_61posdata,   /* MAND */
	  bf_63banknet,
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
      bf_55iccdata,   /* COND */
      bf_61posdata,   /* MAND */
      bf_63banknet,
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
	  bf_5settleamt,  /* COND */
      bf_6chbillgamt, /* COND */
      bf_7trntime,    /* MAND */
	  bf_9convratesetl,/* COND */
      bf_10convrate,   /* COND */
      bf_11stan,      /* MAND */
      bf_12time,      /* OPT  */
      bf_13date,      /* OPT  */
      bf_14expiry,    /* COND */
	  bf_15datestl,   /* COND */
      bf_16convdate,  /* COND */
      bf_18sicnumber, /* MAND */
      bf_22pose,      /* MAND */
      bf_23csn,       /* COND */
      bf_26pincode,
	  bf_32idcode,    /* MAND */
      bf_33fwdid,     /* OPT  */
      bf_35track2,    /* COND */
      bf_37rrn,       /* MAND */
      bf_41termid,    /* COND */
      bf_42cardacc,   /* MAND */
      bf_43cardaccloc,/* MAND */
      bf_45track1,    /* COND */
      bf_48addldata,  /* MAND */
      bf_49trncurcd,  /* MAND */
	  bf_50stlmtcurrcd, /* COND */
	  bf_51crdcurcd,   /* COND */
      bf_52pinblock,  /* COND */
	  bf_53seccntl,		/* COND */
      bf_55iccdata,   /* COND */
      bf_61posdata,   /* MAND */
	  bf_63banknet,  /* COND */
	  bf_104trandesc,
	  bf_105tranlink,
	  bf_119privateuse2,	/* COND */
	  bf_122addlrec,		/* COND */
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
      bf_12time,        /* OPT  */
      bf_13date,        /* OPT  */
      bf_15datestl,     /* COND */
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
      bf_55iccdata,     /* COND */
      bf_62inf,         /* COND */
      bf_63banknet,     /* COND */
	  bf_105tranlink,
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
	  bf_5settleamt,  /* COND */
	  bf_6chbillgamt, /* COND */
      bf_7trntime,    /* MAND */
      bf_11stan,      /* MAND */
      bf_12time,      /* OPT  */
      bf_13date,      /* OPT  */
      bf_14expiry,    /* COND */
      bf_18sicnumber, /* MAND */
      bf_22pose,      /* MAND */
      bf_23csn,       /* COND */
      bf_32idcode,    /* MAND */
      bf_33fwdid,     /* OPT  */
      bf_35track2,    /* COND */
      bf_37rrn,       /* MAND */
      bf_41termid,    /* COND */
      bf_42cardacc,   /* MAND */
      bf_43cardaccloc,/* MAND */
      bf_45track1,    /* COND */
      bf_48addldata,  /* MAND */
      bf_49trncurcd,  /* MAND */
      bf_52pinblock,  /* COND */
	  bf_53seccntl,		/* COND */
      bf_55iccdata,   /* COND */
      bf_61posdata,   /* MAND */
      bf_63banknet,
	  bf_104trandesc,
      bf_done      /* end of fields to move */
      },
      {  
      /********************************************
         C A R D    V E R I F I C A T I O N
      ********************************************/
      AUTH_CARD_VERIFICATION,
      0xF0,0xF1,0xF0,0xF0,                  /* message id, EBCDIC => 0100 */
      0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,        /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,        /* MAND */
      bf_3proc,       /* MAND */
      bf_4amount,     /* MAND */
	  bf_5settleamt,  /* COND */
	  bf_6chbillgamt, /* COND */
      bf_7trntime,    /* MAND */
      bf_11stan,      /* MAND */
      bf_12time,      /* OPT  */
      bf_13date,      /* OPT  */
      bf_14expiry,    /* COND */
      bf_18sicnumber, /* MAND */
      bf_22pose,      /* MAND */
      bf_23csn,       /* COND */
      bf_26pincode,
	  bf_32idcode,    /* MAND */
      bf_33fwdid,     /* OPT  */
      bf_35track2,    /* COND */
      bf_37rrn,       /* MAND */
      bf_41termid,    /* COND */
      bf_42cardacc,   /* MAND */
      bf_43cardaccloc,/* MAND */
      bf_45track1,    /* COND */
      bf_48addldata,  /* MAND */
      bf_49trncurcd,  /* MAND */
      bf_52pinblock,  /* COND */
	  bf_53seccntl,		/* COND */
      bf_55iccdata,   /* COND */
      bf_61posdata,   /* MAND */
      bf_63banknet,
	  bf_104trandesc,
      bf_done      /* end of fields to move */
      },
      {
      /*********************************************************
         C A R D    V E R I F I C A T I O N   R E S P O N S E
      *********************************************************/
      AUTH_CARD_VERIFICATION_RESPONSE,
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
      bf_12time,        /* OPT  */
      bf_13date,        /* OPT  */
      bf_15datestl,     /* COND */
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
      bf_55iccdata,     /* COND */
      bf_62inf,         /* COND */
      bf_63banknet,     /* COND */
      bf_120orgmsgid,   /* COND */
      bf_done           /* end of fields to move */
      },
	  	{ 
	  /********************************************
		 MONEY SEND
	  ********************************************/
	  AUTH_MONEY_SEND,
	  0xF0,0xF1,0xF0,0xF0,					/* message id, EBCDIC => 0100 */
	  0xF2,0xF8,0xF0,0xF0,0xF0,0xF0,		/* processing code */
	  /**** OPTIONS ****/
	  topt1_postadv,
	  topt2_referral+topt2_reversal+topt2_floor,
	  /**** BIT FIELDS ****/
	  bf_2pan,		  /* MAND */
	  bf_3proc, 	  /* MAND */
	  bf_4amount,	  /* MAND */
	  bf_7trntime,	  /* MAND */
	  bf_11stan,	  /* MAND */
	  bf_12time,	  /* OPT  */
	  bf_13date,	  /* OPT  */
	  bf_14expiry,	  /* COND */
	  bf_18sicnumber, /* MAND */
	  bf_22pose,	  /* MAND */
	  bf_23csn, 	  /* COND */
	  bf_32idcode,	  /* MAND */
	  bf_33fwdid,	  /* OPT  */
	  bf_35track2,	  /* COND */
	  bf_37rrn, 	  /* MAND */
	  bf_41termid,	  /* COND */
	  bf_42cardacc,   /* MAND */
	  bf_43cardaccloc,/* MAND */
	  bf_45track1,	  /* COND */
	  bf_48addldata,  /* MAND */
	  bf_49trncurcd,  /* MAND */
	  bf_52pinblock,  /* COND */
	  bf_53seccntl, 	/* COND */
	  bf_55iccdata,   /* COND */
	  bf_61posdata,   /* MAND */
	  bf_104trandesc,
	  bf_124memData, 
	  bf_done	   /* end of fields to move */
	},
	{
		/***********************************************
		   MONEY SEND	 R E S P O N S E
		***********************************************/
		AUTH_MONEY_SEND_RESPONSE,
		0xF0,0xF1,0xF1,0xF0,				  /* message id, EBCDIC => 0110 */
		0xF2,0xF8,0xF0,0xF0,0xF0,0xF0,		  /* processing code */
		/**** OPTIONS ****/
		topt1_postadv,
		topt2_referral+topt2_reversal+topt2_floor,
		/**** BIT FIELDS ****/
		bf_2pan,		  /* MAND */
		bf_3proc,		  /* MAND */
		bf_4amount, 	  /* MAND */
		bf_5settleamt,	  /* COND */
		bf_6chbillgamt,   /* COND */
		bf_7trntime,	  /* MAND */
		bf_9convratesetl, /* COND */
		bf_10convrate,	  /* COND */
		bf_11stan,		  /* MAND */
		bf_12time,		  /* OPT  */
		bf_13date,		  /* OPT  */
		bf_15datestl,	  /* COND */
		bf_16convdate,	  /* COND */
		bf_32idcode,	  /* MAND */
		bf_33fwdid, 	  /* COND */
		bf_37rrn,		  /* MAND */
		bf_38authcode,	  /* COND */
		bf_39respcode,	  /* MAND */
		bf_41termid,	  /* COND */
		bf_48addldata,	  /* COND */
		bf_49trncurcd,	  /* MAND */
		bf_50stlmtcurrcd, /* COND */
		bf_51crdcurcd,	  /* COND */
		bf_55iccdata,	  /* COND */
		bf_62inf,		  /* COND */
		bf_63banknet,	  /* COND */
		bf_124memData,
		bf_done 		  /* end of fields to move */
		},

      { 
      /********************************************
         C A S H   A D V A N C E 
      ********************************************/
      AUTH_CASH_ADVANCE,
      0xF0,0xF1,0xF0,0xF0,                  /* message id, EBCDIC => 0100 */
      0xF0,0xF1,0xF0,0xF0,0xF0,0xF0,        /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,        /* MAND */
      bf_3proc,       /* MAND */
      bf_4amount,     /* MAND */
	  bf_5settleamt,  /* COND */
	  bf_6chbillgamt, /* COND */
      bf_7trntime,    /* MAND */
	  bf_9convratesetl ,      /* TF Phani          */
	  bf_10convrate ,     /* TF Phani */
      bf_11stan,      /* MAND */
      bf_12time,      /* OPT  */
      bf_13date,      /* OPT  */
      bf_14expiry,    /* COND */
	  bf_15datestl,     /* COND */
      bf_16convdate,    /* COND */
      bf_18sicnumber, /* MAND */
      bf_22pose,      /* MAND */
      bf_23csn,       /* COND */
      bf_26pincode,
	  bf_32idcode,    /* MAND */
      bf_33fwdid,     /* OPT  */
      bf_35track2,    /* COND */
      bf_37rrn,       /* MAND */
      bf_41termid,    /* COND */
      bf_42cardacc,   /* MAND */
      bf_43cardaccloc,/* MAND */
      bf_45track1,    /* COND */
      bf_48addldata,  /* MAND */
      bf_49trncurcd,  /* MAND */
	  bf_50stlmtcurrcd, /* COND */
      bf_51crdcurcd,    /* COND */
      bf_52pinblock,  /* COND */
	  bf_53seccntl,		/* COND */
      bf_55iccdata,   /* COND */
      bf_61posdata,   /* MAND */
	  bf_63banknet,     /* COND */
	  bf_104trandesc,
      bf_done      /* end of fields to move */
      },


      { 
      /********************************************
         C A S H   A D V A N C E   A U T H
      ********************************************/
      AUTH_CASH_ADVANCE_AUTHORIZATION,
      0xF0,0xF1,0xF0,0xF0,                  /* message id, EBCDIC => 0100 */
      0xF0,0xF1,0xF0,0xF0,0xF0,0xF0,        /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,        /* MAND */
      bf_3proc,       /* MAND */
      bf_4amount,     /* MAND */
	  bf_5settleamt,  /* COND */
	  bf_6chbillgamt, /* COND */
      bf_7trntime,    /* MAND */
      bf_11stan,      /* MAND */
      bf_12time,      /* OPT  */
      bf_13date,      /* OPT  */
      bf_14expiry,    /* COND */
	  bf_15datestl,     /* COND */
      bf_16convdate,    /* COND */
      bf_18sicnumber, /* MAND */
      bf_22pose,      /* MAND */
      bf_23csn,       /* COND */
      bf_26pincode,
	  bf_32idcode,    /* MAND */
      bf_33fwdid,     /* OPT  */
      bf_35track2,    /* COND */
      bf_37rrn,       /* MAND */
      bf_41termid,    /* COND */
      bf_42cardacc,   /* MAND */
      bf_43cardaccloc,/* MAND */
      bf_45track1,    /* COND */
      bf_48addldata,  /* MAND */
      bf_49trncurcd,  /* MAND */
	  bf_50stlmtcurrcd, /* COND */
      bf_51crdcurcd,    /* COND */
      bf_52pinblock,  /* COND */
	  bf_53seccntl,		/* COND */
      bf_55iccdata,   /* COND */
      bf_61posdata,   /* MAND */
	  bf_63banknet,     /* COND */	
	  bf_104trandesc,
      bf_done      /* end of fields to move */
      },

      {
      /***********************************************
         C A S H   A D V A N C E   R E S P O N S E
      ***********************************************/
      AUTH_CASH_ADVANCE_RESPONSE,
      0xF0,0xF1,0xF1,0xF0,                  /* message id, EBCDIC => 0110 */
      0xF0,0xF1,0xF3,0xF0,0xF0,0xF0,        /* processing code */
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
      bf_12time,        /* OPT  */
      bf_13date,        /* OPT  */
      bf_15datestl,     /* COND */
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
      bf_55iccdata,     /* COND */
      bf_62inf,         /* COND */
      bf_63banknet,     /* COND */
      bf_120orgmsgid,   /* COND */
      bf_done           /* end of fields to move */
      },


      {  
      /********************************************
         B A L A N C E   I N Q U I R Y 
      ********************************************/
      AUTH_BALANCE_INQUIRY,
      0xF0,0xF1,0xF0,0xF0,                  /* message id, EBCDIC => 0100 */
      0xF3,0xF0,0xF0,0xF0,0xF0,0xF0,        /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,        /* MAND */
      bf_3proc,       /* MAND */
      bf_4amount,     /* MAND */
	  bf_5settleamt,  /* COND */
	  bf_6chbillgamt, /* COND */
      bf_7trntime,    /* MAND */
      bf_11stan,      /* MAND */
      bf_12time,      /* OPT  */
      bf_13date,      /* OPT  */
      bf_14expiry,    /* COND */
      bf_18sicnumber, /* MAND */
      bf_22pose,      /* MAND */
      bf_23csn,       /* COND */
      bf_32idcode,    /* MAND */
      bf_33fwdid,     /* OPT  */
      bf_35track2,    /* COND */
      bf_37rrn,       /* MAND */
      bf_41termid,    /* COND */
      bf_42cardacc,   /* MAND */
      bf_43cardaccloc,/* MAND */
      bf_45track1,    /* COND */
      bf_48addldata,  /* MAND */
      bf_49trncurcd,  /* MAND */
      bf_52pinblock,  /* COND */
	  bf_53seccntl,		/* COND */
      bf_55iccdata,   /* COND */
      bf_61posdata,   /* MAND */
	  bf_63banknet,
	  bf_104trandesc,
      bf_done      /* end of fields to move */
      },      

      {
      /***************************************************
         B A L A N C E   I N Q U I R Y   R E S P O N S E
       ***************************************************/
      AUTH_BALANCE_INQUIRY_RESPONSE,
      0xF0,0xF1,0xF1,0xF0,                  /* message id, EBCDIC => 0110 */
      0xF3,0xF0,0xF0,0xF0,0xF0,0xF0,        /* processing code */
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
      bf_12time,        /* OPT  */
      bf_13date,        /* OPT  */
      bf_15datestl,     /* COND */
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
      bf_54addlamts,    /* MAND */
      bf_55iccdata,     /* COND */
      bf_62inf,         /* COND */
      bf_63banknet,     /* COND */
      bf_120orgmsgid,   /* COND */
      bf_done           /* end of fields to move */
      },

      { /* Added some optional fields 10 Nov,2009 TF-Ajay */ 
      /********************************************
         V O I D   S A L E
      ********************************************/
	  /* Void sale txn also should go as reversal Nov 23, 2009 TF-Ajay*/
      AUTH_VOID_SALE,
      0xF0,0xF4,0xF0,0xF0,                  /* message id, EBCDIC => 0400 */
      0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,        /* processing code */
      /**** OPTIONS ****/
      topt1_credit+topt1_postadv,
      topt2_reversal,
      /**** BIT FIELDS ****/
      bf_2pan,        /* MAND */
      bf_3proc,       /* MAND */
      bf_4amount,     /* MAND */
      bf_7trntime,    /* MAND */
      bf_11stan,      /* MAND */
	  bf_14expiry,    /* COND */
      bf_18sicnumber, /* MAND */
      bf_22pose,      /* MAND */
      bf_23csn,       /* COND */
      bf_32idcode,    /* MAND */
      bf_33fwdid,     /* COND */
      /*bf_35track2,    / COND */
      bf_37rrn,       /* MAND */
	  bf_38authcode,  /* COND */
      bf_39respcode,  /* MAND */
      bf_41termid,    /* COND */
      bf_42cardacc,   /* MAND */
      bf_43cardaccloc,/* MAND */
     /* bf_45track1,    / COND */
      bf_48addldata,  /* MAND */
      bf_49trncurcd,  /* MAND */
      bf_54addlamts,  /* COND */
      /*bf_55iccdata,   / COND */
      bf_61posdata,   /* MAND */
	  bf_63banknet,	
	  bf_90orgdata,   /* MAND */
	  bf_95repl_amt,  /* COND */
      bf_done      /* end of fields to move */
      },

      {
      /********************************************
         V O I D   R E F U N D
      ********************************************/
      AUTH_VOID_REFUND,
      0xF0,0xF1,0xF0,0xF0,                  /* message id, EBCDIC => 0100 */
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
      bf_14expiry,    /* COND */
      bf_18sicnumber, /* MAND */
      bf_22pose,      /* MAND */
      bf_23csn,       /* COND */
      bf_32idcode,    /* MAND */
      bf_35track2,    /* COND */
      bf_37rrn,       /* MAND */
      bf_41termid,    /* COND */
      bf_42cardacc,   /* MAND */
      bf_43cardaccloc,/* MAND */
      bf_45track1,    /* COND */
      bf_48addldata,  /* MAND */
      bf_49trncurcd,  /* MAND */
      bf_55iccdata,   /* COND */
      bf_61posdata,   /* MAND */
	  bf_63banknet,
	  bf_104trandesc,
	  bf_done      /* end of fields to move */
      },

      
      {  
      /********************************************
         V O I D   P A Y M E N T 
      ********************************************/
      AUTH_VOID_PAYMENT,
      0xF0,0xF1,0xF0,0xF0,                  /* message id, EBCDIC => 0100 */
      0xF4,0xF2,0xF0,0xF0,0xF0,0xF0,        /* processing code */
      /**** OPTIONS ****/
      topt1_credit+topt1_postadv,
      topt2_reversal,
      /**** BIT FIELDS ****/
      bf_2pan,        /* MAND */
      bf_3proc,       /* MAND */
      bf_4amount,     /* MAND */
      bf_7trntime,    /* MAND */
      bf_11stan,      /* MAND */
      bf_14expiry,    /* COND */
      bf_18sicnumber, /* MAND */
      bf_22pose,      /* MAND */
      bf_23csn,       /* COND */
      bf_32idcode,    /* MAND */
      bf_35track2,    /* COND */
      bf_37rrn,       /* MAND */
      bf_41termid,    /* COND */
      bf_42cardacc,   /* MAND */
      bf_43cardaccloc,/* MAND */
      bf_45track1,    /* COND */
      bf_48addldata,  /* MAND */
      bf_49trncurcd,  /* MAND */
      bf_55iccdata,   /* COND */
      bf_61posdata,   /* MAND */
      bf_63banknet,
	  bf_104trandesc,
      bf_done      /* end of fields to move */
      },
/* Add 0120 for MCRD   Advices .. TF Phani */
	  { 
      /*************************************************************
         A U T H O R I Z A T I O N   A D V I C E   
       *************************************************************/
      AUTH_ADVICE,
      0xF0,0xF1,0xF2,0xF0,                  /* message id, EBCDIC => 0120 */
      0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,        /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,         /* MAND */
      bf_3proc,        /* MAND */
      bf_4amount,      /* MAND */
      bf_5settleamt,   /* COND */
      bf_6chbillgamt,  /* COND */
      bf_7trntime,     /* MAND */
      bf_9convratesetl,/* COND */
      bf_10convrate,   /* COND */
      bf_11stan,       /* MAND */
      bf_12time,       /* OPT  */
      bf_13date,       /* OPT  */
      bf_14expiry,    /* COND */
      bf_15datestl,    /* COND */
      bf_16convdate,   /* COND */
      bf_18sicnumber, /* MAND */
      bf_19ctrycd,    /*MAND*/
      bf_22pose,      /* MAND */
	  bf_28feeamount,
      bf_32idcode,     /* MAND */
      bf_33fwdid,      /* COND */
      bf_37rrn,        /* MAND */
	  bf_38authcode,    /* COND */
      bf_39respcode,   /* MAND */
      bf_41termid,     /* COND */
	  bf_42cardacc,
	  bf_43cardaccloc,
	  bf_48addldata,  /* MAND */
      bf_49trncurcd,   /* MAND */
      bf_50stlmtcurrcd,/* COND */
      bf_51crdcurcd,   /* COND */
      bf_55iccdata,    /* COND */
	  bf_56ISOuse,
	  bf_61posdata,   /* MAND */
      bf_62inf,        /* COND */
      bf_63banknet,    /* COND */
	  bf_105tranlink,
	  bf_108moneySend,
	  bf_119privateuse2, /* COND */
	  bf_121mcagent,
      bf_done      /* end of fields to move */
      },

      { 
      /*************************************************************
         A U T H O R I Z A T I O N   A D V I C E   R E S P O N S E
       *************************************************************/
      AUTH_ADVICE_RESPONSE,
      0xF0,0xF1,0xF3,0xF0,                  /* message id, EBCDIC => 0130 */
      0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,        /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,         /* MAND */
      bf_3proc,        /* MAND */
      bf_4amount,      /* MAND */
      bf_5settleamt,   /* COND */
      bf_6chbillgamt,  /* COND */
      bf_7trntime,     /* MAND */
      bf_9convratesetl,/* COND */
      bf_10convrate,   /* COND */
      bf_11stan,       /* MAND */
      bf_12time,       /* OPT  */
      bf_13date,       /* OPT  */
      bf_15datestl,    /* COND */
      bf_16convdate,   /* COND */
      bf_32idcode,     /* MAND */
      bf_33fwdid,      /* COND */
      bf_37rrn,        /* MAND */
      bf_39respcode,   /* MAND */
      bf_41termid,     /* COND */
      bf_49trncurcd,   /* MAND */
      bf_50stlmtcurrcd,/* COND */
      bf_51crdcurcd,   /* COND */
      bf_62inf,        /* COND */
      bf_63banknet,    /* COND */
	  bf_105tranlink,
      bf_done      /* end of fields to move */
      },
/* Add 0121 and 0131 for Cadience Standin Processing */
	  { 
      /*************************************************************
         A U T H O R I Z A T I O N   A D V I C E   R E P E AT   
       *************************************************************/
      AUTH_ADVICE_REPEAT,
      0xF0,0xF1,0xF2,0xF1,                  /* message id, EBCDIC => 0121*/
      0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,        /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,         /* MAND */
      bf_3proc,        /* MAND */
      bf_4amount,      /* MAND */
      bf_5settleamt,   /* COND */
      bf_6chbillgamt,  /* COND */
      bf_7trntime,     /* MAND */
      bf_9convratesetl,/* COND */
      bf_10convrate,   /* COND */
      bf_11stan,       /* MAND */
      bf_12time,       /* OPT  */
      bf_13date,       /* OPT  */
      bf_14expiry,    /* COND */
      bf_15datestl,    /* COND */
      bf_16convdate,   /* COND */
      bf_18sicnumber, /* MAND */
	  bf_19ctrycd,    /*MAND*/
      bf_22pose,      /* MAND */
	  bf_28feeamount,
      bf_32idcode,     /* MAND */
      bf_33fwdid,      /* COND */
      bf_37rrn,        /* MAND */
      bf_39respcode,   /* MAND */
      bf_41termid,     /* COND */
  	  bf_42cardacc,
	  bf_43cardaccloc,
	  bf_48addldata,  /* MAND */
      bf_49trncurcd,   /* MAND */
      bf_50stlmtcurrcd,/* COND */
      bf_51crdcurcd,   /* COND */
      bf_55iccdata,    /* COND */
	  bf_56ISOuse,
	  bf_61posdata,   /* MAND */
      bf_62inf,        /* COND */
      bf_63banknet,    /* COND */
      bf_done      /* end of fields to move */
      },
      { 
      /***********************************************************************
         A U T H O R I Z A T I O N   A D V I C E   R E P E A T  R E S P O N S E
       **********************************************************************/
      AUTH_ADVICE_REPEAT_RESPONSE,
      0xF0,0xF1,0xF3,0xF1,                  /* message id, EBCDIC => 0131 */
      0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,        /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,         /* MAND */
      bf_3proc,        /* MAND */
      bf_4amount,      /* MAND */
      bf_5settleamt,   /* COND */
      bf_6chbillgamt,  /* COND */
      bf_7trntime,     /* MAND */
      bf_9convratesetl,/* COND */
      bf_10convrate,   /* COND */
      bf_11stan,       /* MAND */
      bf_12time,       /* OPT  */
      bf_13date,       /* OPT  */
      bf_15datestl,    /* COND */
      bf_16convdate,   /* COND */
      bf_32idcode,     /* MAND */
      bf_33fwdid,      /* COND */
      bf_37rrn,        /* MAND */
      bf_39respcode,   /* MAND */
      bf_41termid,     /* COND */
      bf_49trncurcd,   /* MAND */
      bf_50stlmtcurrcd,/* COND */
      bf_51crdcurcd,   /* COND */
      bf_55iccdata,    /* COND */
      bf_62inf,        /* COND */
      bf_63banknet,    /* COND */
      bf_done      /* end of fields to move */
      },
	  {
	  /********************************************
		   Authorization return
		********************************************/
		AUTH_PRE_AUTHORIZATION_CANCLE_RESPONSE,
		0xF0,0xF1,0xF1,0xF0,                  /* message id, EBCDIC => 0100 */
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
		bf_12time,      /* OPT  */
		bf_13date,      /* OPT  */
		bf_14expiry,    /* COND */
		bf_18sicnumber, /* MAND */
		bf_22pose,      /* MAND */
		bf_23csn,       /* COND */
		bf_32idcode,    /* MAND */
		bf_33fwdid,     /* OPT  */
		bf_35track2,    /* COND */
		bf_37rrn,       /* MAND */
		bf_41termid,    /* COND */
		bf_42cardacc,   /* MAND */
		bf_43cardaccloc,/* MAND */
		bf_45track1,    /* COND */
		bf_48addldata,  /* MAND */
		bf_49trncurcd,  /* MAND */
		bf_52pinblock,  /* COND */
		bf_55iccdata,   /* COND */
		bf_61posdata,   /* MAND */
		bf_done      /* end of fields to move */
		},

	  /* Change Request: Jun 12 2009, Ajay TF */
	  {
	  /********************************************
        A U T H  A D V I C E  R E V E R S A A L  R E Q U E S T
      ********************************************/
	  AUTH_REVERSAL_ADVICE,			
	  0xF0,0xF4,0xF2,0xF0,				     /* message id */
      0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,		 /* processing code */
            
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,            /* MAND */
      bf_3proc,           /* MAND */
      bf_4amount,         /* MAND */
	  bf_5settleamt,      /* COND */
      bf_6chbillgamt,     /* COND */
      bf_7trntime,        /* MAND */
	  bf_9convratesetl,   /* COND */
      bf_10convrate,      /* COND */
      bf_11stan,          /* MAND */
      bf_12time,		  /* OPT  */
      bf_13date,          /* OPT  */
      bf_14expiry,    /* COND */
	  bf_15datestl,       /* COND */
	  bf_16convdate,	  /* MAND */
	  bf_18sicnumber, /* MAND */
	  bf_19ctrycd,    /*MAND*/
      bf_22pose,      /* MAND */
	  bf_32idcode,        /* MAND */
      bf_33fwdid,      /* COND */
      bf_37rrn,           /* MAND */
      bf_38authcode,      /* COND */
	  bf_39respcode,      /* MAND */
      bf_41termid,     /* COND */
  	  bf_42cardacc,
  	  bf_43cardaccloc,
  	  bf_48addldata,		/* MAND */
      bf_49trncurcd,      /* MAND */
	  bf_50stlmtcurrcd,   /* COND */
      bf_51crdcurcd,      /* COND */
      bf_61posdata,    /* MAND */
	  bf_62inf,           /* COND */
      bf_63banknet,		  /* MAND */
	  bf_90orgdata,       /* MAND */
      bf_95repl_amt,	  /* COND */
	  bf_105tranlink,
	  bf_121mcagent,	  /* COND */
      bf_done      /* end of fields to move */
      },
      { 
      /********************************************
         F I L E   U P D A T E
      ********************************************/
      AUTH_CARD_UPDATE,
      0xF0,0xF3,0xF0,0xF2,                  /* message id, EBCDIC => 0302 */
      0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,        /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,        /* COND */
      bf_7trntime,    /* MAND */
      bf_11stan,      /* MAND */
      bf_33fwdid,     /* COND */
      bf_91filecode,  /* MAND */
      bf_96securcode, /* COND */
      bf_101filename, /* MAND */
      bf_120orgmsgid, /* COND */
      bf_done      /* end of fields to move */
      },
/* TF PHANI -- Added 0312 response*/
	  { 
      /********************************************
         F I L E   U P D A T E RESPONSE
      ********************************************/
      AUTH_CARD_UPDATE_RESPONSE,
      0xF0,0xF3,0xF1,0xF2,                  /* message id, EBCDIC => 0312 */
      0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,        /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,        /* COND */
      bf_7trntime,    /* MAND */
      bf_11stan,      /* MAND */
      bf_33fwdid,     /* COND */
      bf_39respcode, /* MAND */
      bf_91filecode,  /* MAND */
      bf_96securcode, /* COND */
      bf_101filename, /* MAND */
      bf_120orgmsgid, /* COND */
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
      bf_94servind,  /* MAND */
      bf_96securcode,/* MAND */
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
      bf_2pan,       /* MAND */
      bf_7trntime,   /* MAND */
      bf_11stan,     /* MAND */
      bf_33fwdid,    /* MAND */
      bf_39respcode, /* MAND */
      bf_44respdata, /* IGNR */
      bf_63banknet,  /* IGNR */
      bf_70nmicode,  /* MAND */
      bf_done        /* end of fields to move */
      },


      {
      /***************************************************************
         A D M I N I S T R A T I V E   A D V I C E   R E S P O N S E
      ****************************************************************/
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
      bf_62inf,     /* COND */
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
		BYTE sec_cntl      [16];
        BYTE addl_len      [3];
        BYTE addlamts      [120];
        BYTE iccdata_len   [3];
        BYTE iccdata       [255];
        BYTE ISOdata_len   [3];
        BYTE ISOdata       [255];
        BYTE advice_cd_len [3];
        BYTE advice_code   [60];
        BYTE pos_data_len  [3];
        BYTE pos_data      [26];
        BYTE inf_data_len  [3];
        BYTE inf_data      [100];
        BYTE banknet_len   [3];
        BYTE banknet       [50];
        BYTE nmi_code      [3];
        BYTE action_date   [6];
        BYTE orig_data     [42];
        BYTE file_code     [1];
        BYTE service_ind   [7];
        BYTE replace_amt   [42];
        BYTE security_code [8];
        BYTE recv_len      [2];
        BYTE recvid        [11];
		BYTE de108_len		[3];
		BYTE de108			[999];
        BYTE filename_len  [2];
        BYTE filename      [17];
		BYTE addldata_nation_len [3];
		BYTE addldata_nation   [200];  
		BYTE private_use2_len	[3];
		BYTE private_use2	   [999];
        BYTE acct1_len     [2];
        BYTE acct1         [28];
        BYTE acct2_len     [2];
        BYTE acct2         [28];
        BYTE trandesc_len  [3];
        BYTE trandesc      [100];
        BYTE tranlink_len  [3];
        BYTE tranlink_data [999];
        BYTE orig_msg_len  [3];
        BYTE orig_msg_id   [1000];
        BYTE mc_agent_len  [3];
        BYTE mc_agent_id   [11];
        BYTE addlrec_len   [3];
        BYTE addlrec       [1000];
		BYTE memData_len   [3];
        BYTE memData       [200];
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
BYTE ascii_ebcdic_conv( unsigned char, char * );
void vEBCDICMove(    enum bitfields );
void EBCDICMove(     enum bitfields );
void RspEBCDICvMove( enum bitfields );
void RspEBCDICMove(  enum bitfields );
void RspIgnore(      enum bitfields );
void vRspIgnore(     enum bitfields );
void RspEBCDICvMove_NoLen( enum bitfields );
void vDataMove(      enum bitfields );
void RspMovevData(   enum bitfields );

INT  CondProc(       enum bitfields, BYTE );
INT  CondExpd(       enum bitfields, BYTE );
INT  CondPan(        enum bitfields, BYTE );
INT  CondTrack1(     enum bitfields, BYTE );
INT  Move_Always(    enum bitfields, BYTE );
INT  Move_IfThere(   enum bitfields, BYTE );
INT  Move_Never(     enum bitfields, BYTE );

INT  get_variable_length( pBYTE, INT );
INT  parse_field_48( pCHAR, pCHAR, pAUTH_TX);
BOOLEAN  blank_or_null_string(pCHAR astr, INT max_len);

void GenericMove(enum bitfields fieldid);
void RspMoveGeneric(enum bitfields fieldid);
INT isalphanum(pCHAR string, INT length );


static  struct bitfield_data BFData[] = {
   {
   bf_2pan,                      /* field */
   HOST_MESSAGE.pan_len,         /* data to move */
   2,                            /* # of bytes in pan len field */
   Move_IfThere,                 /* conditional  */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_3proc,                     /* field */
   HOST_MESSAGE.proccode,        /* data to move */
   6,                            /* # of bytes in mcrd iso field */
   Move_Always,                  /* mandatory */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_4amount,                   /* field */
   HOST_MESSAGE.amount,          /* data to move */
   12,                           /* # of bytes in mcrd iso field */
   Move_Always,                  /* mandatory */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_5settleamt,                /* field */
   HOST_MESSAGE.settle_amt,      /* data to move */
   12,                           /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_6chbillgamt,               /* field */
   HOST_MESSAGE.ch_bill_amt,     /* data to move */
   12,                           /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_7trntime,                  /* field */
   HOST_MESSAGE.gmt_time,        /* data to move */
   10,                           /* # of bytes in mcrd iso field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_9convratesetl,             /* field */
   HOST_MESSAGE.conv_rate_stl,   /* data to move */
   8,                            /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_10convrate,                /* field */
   HOST_MESSAGE.conv_rate_chb,   /* data to move */
   8,                            /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_11stan,                    /* field */
   HOST_MESSAGE.stan,            /* data to move */
   6,                            /* # of bytes in mcrd iso field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_12time,                    /* field */
   HOST_MESSAGE.time,            /* data to move */
   6,                            /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspIgnore                     /* response routine */
   },
   {
   bf_13date,                    /* field */
   HOST_MESSAGE.date,            /* data to move */
   4,                            /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspIgnore                     /* response routine */
   },
   {
   bf_14expiry,                  /* field */
   HOST_MESSAGE.date_expd,       /* data to move */
   4,                            /* # of bytes in mcrd iso field */
   CondExpd,                     /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_15datestl,                 /* field */
   HOST_MESSAGE.date_settle,     /* data to move */
   4,                            /* # of bytes in mcrd iso field */
   Move_IfThere,			     /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_16convdate,                /* field */
   HOST_MESSAGE.conv_date,       /* data to move */
   4,                            /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_18sicnumber,               /* field */
   HOST_MESSAGE.sic_number,      /* data to move */
   4,                            /* # of bytes in mcrd iso field */
   Move_Always,                  /* mandatory */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_19ctrycd,                  /* field */
   HOST_MESSAGE.ctry_code,       /* data to move */
   3,                            /* # of bytes in mcrd iso field */
   Move_Always,                  /* mandatory    */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                     /* response routine */
   },
   {
   bf_20panctrycd,               /* field */
   HOST_MESSAGE.pan_ctry_cd,     /* data to move */
   3,                            /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_22pose,                    /* field */
   HOST_MESSAGE.posentry,        /* data to move */
   3,                            /* # of bytes in mcrd iso field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_23csn ,                    /* field */
   HOST_MESSAGE.card_seq_num,    /* data to move */
   3,                            /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_24nii,                     /* field */
   HOST_MESSAGE.nii,             /* data to move */
   3,                            /* # of bytes in mcrd iso field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspIgnore                     /* response routine */
   },
   {
   bf_25pocc,                    /* field */
   HOST_MESSAGE.pos_condcd,      /* data to move */
   2,                            /* # of bytes in mcrd iso field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspIgnore                     /* response routine */
   },
   {
   bf_26pincode,                 /* field */
   HOST_MESSAGE.pos_pincode,      /* data to move */
   2,                            /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspIgnore                     /* response routine */
   },
    {
   bf_28feeamount,                 /* field */
   HOST_MESSAGE.tran_fee,      /* data to move */
   9,                            /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspIgnore                     /* response routine */
   },
   {
   bf_32idcode,                  /* field */
   HOST_MESSAGE.acq_inst_len,    /* data to move */
   2,                            /* # of bytes in mcrd iso field */
   Move_Always,                  /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_33fwdid,                   /* field */
   HOST_MESSAGE.fwd_inst_len,    /* data to move */
   2,                            /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional */
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
   12,                           /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_38authcode,                /* field */
   HOST_MESSAGE.authcode,        /* data to move */
   6,                            /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_39respcode,                /* field */
   HOST_MESSAGE.responsecd,      /* data to move */
   2,                            /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_41termid,                  /* field */
   HOST_MESSAGE.termid,          /* data to move */
   8,                            /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_42cardacc,                 /* field */
   HOST_MESSAGE.cardaccid,       /* data to move */
   15,                           /* # of bytes in mcrd iso field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_43cardaccloc,              /* field */
   HOST_MESSAGE.cardaccloc,      /* data to move */
   40,                           /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
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
   3,                            /* # of bytes in mcrd iso field */
   Move_Always,                  /* mandatory    */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_50stlmtcurrcd,             /* field */
   HOST_MESSAGE.settle_cur_cd,   /* data to move */
   3,                            /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional  */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_51crdcurcd,                /* field */
   HOST_MESSAGE.card_curr_cd,    /* data to move */
   3,                            /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_52pinblock,                /* field */
   HOST_MESSAGE.pinblock,        /* data to move */
   8,                            /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
/**************************** ThoughtFocus Girija Y ***************************/
   {
   bf_53seccntl,                 /* field */
   HOST_MESSAGE.sec_cntl,        /* data to move */
   16,                            /* # of bytes in MC field */
   Move_IfThere,                 /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
/******************************************************************************/
   {
   bf_54addlamts,                /* field */
   HOST_MESSAGE.addl_len,        /* data to move */
   3,                            /* # of bytes in addl len field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_55iccdata,                 /* field */
   HOST_MESSAGE.iccdata_len,     /* data to move */
   3,                            /* # of bytes in addl len field */
   Move_IfThere,                 /* conditional */
   vDataMove,                    /* move routine */
   RspMovevData                  /* response routine */
   },
   {
   	bf_56ISOuse,                 /* field */
    HOST_MESSAGE.ISOdata_len,     /* data to move */
    3,                            /* # of bytes in addl len field */
   	Move_IfThere,                 /* conditional */
   	vEBCDICMove,                    /* move routine */
   	RspEBCDICvMove                  /* response routine */
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
   bf_70nmicode,                 /* field */
   HOST_MESSAGE.nmi_code,        /* data to move */
   3,                            /* # of bytes in mcrd iso field */
   Move_Always,                  /* mandatory    */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_73actiondate,              /* field */
   HOST_MESSAGE.action_date,     /* data to move */
   6,                            /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional  */
   EBCDICMove,                   /* move routine */
   RspIgnore                     /* response routine */
   },
   { /* NOTE: Field 90 is no longer used in any message except 0400 and 0420 msg */
   bf_90orgdata,                 /* field */
   HOST_MESSAGE.orig_data,       /* data to move */
   42,                           /* # of bytes in mcrd iso field */
   Move_Always,                  /* mandatory    */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_91filecode,                /* field */
   HOST_MESSAGE.file_code,       /* data to move */
   1,                            /* # of bytes in mcrd iso field */
   Move_Always,                  /* mandatory    */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_94servind,                 /* field */
   HOST_MESSAGE.service_ind,     /* data to move */
   7,                            /* # of bytes in mcrd iso field */
   Move_Always,                  /* mandatory    */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_95repl_amt,                /* field */
   HOST_MESSAGE.replace_amt,     /* data to move */
   42,                           /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional  */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_96securcode,               /* field */
   HOST_MESSAGE.security_code,   /* data to move */
   8,                            /* # of bytes in mcrd iso field */
   Move_Always,                  /* mandatory    */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_100recvid,                 /* field */
   HOST_MESSAGE.recv_len,        /* data to move */
   2 ,                           /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional  */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_101filename,               /* field */
   HOST_MESSAGE.filename_len,    /* data to move */
   2 ,                           /* # of bytes in mcrd iso field */
   Move_Always,                  /* mandatory    */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_102acct1,                  /* field */
   HOST_MESSAGE.acct1_len,       /* data to move */
   2 ,                           /* # of bytes in mcrd iso field */
   Move_Always,                  /* mandatory    */
   vEBCDICMove,                  /* move routine */
   vRspIgnore                    /* response routine */
   },
   {
   bf_103acct2,                  /* field */
   HOST_MESSAGE.acct2_len,       /* data to move */
   2,                            /* # of bytes in mcrd iso field */
   Move_Always,                  /* mandatory    */
   vEBCDICMove,                  /* move routine */
   vRspIgnore                    /* response routine */
   },
   {
   bf_104trandesc,                 /* field */
   HOST_MESSAGE.trandesc_len,     /* data to move */
   3,                            /* # of bytes in addl len field */
   Move_IfThere,                  /* mandatory */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                 /* response routine */
   },
   {
   bf_105tranlink,                  /* field */
   HOST_MESSAGE.tranlink_len,       /* data to move */
   3,                            /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional  */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_108moneySend,                  /* field */
   HOST_MESSAGE.de108_len,       /* data to move */
   3,                            /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional  */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */   
   },
   {
   bf_112addldata,                  /* field */
   HOST_MESSAGE.addldata_nation_len,       /* data to move */
   3,                            /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional  */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */   
   },
   {
   bf_119privateuse2,                  /* field */
   HOST_MESSAGE.private_use2_len,       /* data to move */
   3,                            /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional  */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_120orgmsgid,               /* field */
   HOST_MESSAGE.orig_msg_len,    /* data to move */
   3,                            /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional  */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_121mcagent,                /* field */
   HOST_MESSAGE.mc_agent_len,    /* data to move */
   3,                            /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional  */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_122addlrec,                /* field */
   HOST_MESSAGE.addlrec_len,     /* data to move */
   3,                            /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional  */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_124memData,                /* field */
   HOST_MESSAGE.memData_len,     /* data to move */
   3,                            /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional  */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_127privdata,               /* field */
   HOST_MESSAGE.priv127_len,     /* data to move */
   3,                            /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional  */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },

   {
   bf_done,                      /* field */
   NULL,                         /* data to move */
   99,                           /* # of bytes in mcrd iso field */
   NULL,                         /* conditional */
   NULL,                         /* move routine */
   NULL                          /* response routine */
   }
   }; /* end BFData */

void 	trim_spaces( pBYTE );
INT 	trim_spaces_Only( pBYTE pString );
void	get_MC_EMV_data( INT emv_len_in, pBYTE emv_data_in );
void 	get_DEBITMC_EMV_data( INT emv_len_in, pBYTE emv_data_in );
void 	ncmcr2_log_message( INT dest, INT sev, pCHAR msg, pCHAR func, int details ,pCHAR card_num);
void 	Mask_card_number(char * card_num);
void 	ncmcr2_create_Error_Filename();
INT  	ncmcr2_Log_error_warning_to_File(pCHAR Error_Buf,int sev, pCHAR func, int detail);
void	ncmcr2_get_error_warning_file_name_path(void );
void	ncmcr2_read_tf_ini( pCHAR err_buf );
void 	ncmcr2_read_TO_config_details_from_tf_ini();
void	ncmcr2_read_queue_name_for_0302_from_tf_ini();
void	ncmcr2_read_Online_PIN_flag_from_tf_ini();
BOOLEAN ncmcr2_Is_AsiaPay_Transaction(pAUTH_TX p_auth_tx);

INT 	ncmcr2_check_if_trx_is_bdo_pos_acq_trx( pAUTH_TX p_auth_tx  );

INT 	ncmcr2_check_if_PEM_is_MobilePOS_NFC_Tap2Phone_trx_with_pin( pAUTH_TX p_auth_tx  );
INT 	ncmcr2_check_if_PEM_is_MobilePOS_NFC_Tap2Phone_trx_without_pin( pAUTH_TX p_auth_tx  );
INT 	ncmcr2_check_if_trx_qualify_for_MobilePOS_NFC_Tap2Phone_trx ( pAUTH_TX p_auth_tx  );
INT	 	ncmcr2_check_if_trx_is_MC_MobilePOS_NFC_Tap2Phone_trx_with_pin( pAUTH_TX p_auth_tx  );
INT	 	ncmcr2_check_if_trx_is_MC_MobilePOS_NFC_Tap2Phone_trx_without_pin( pAUTH_TX p_auth_tx  );

INT 	ncmcr2_check_if_PEM_is_Swiftpass_BDO_Pay_credit_trx_with_pin( pAUTH_TX p_auth_tx  );
INT 	ncmcr2_check_if_PEM_is_Swiftpass_BDO_Pay_credit_trx_without_pin( pAUTH_TX p_auth_tx  );
INT 	ncmcr2_check_if_trx_qualify_for_Swiftpass_BDO_Pay_credit_trx( pAUTH_TX p_auth_tx  );
INT 	ncmcr2_check_if_trx_is_mc_Swiftpass_BDO_Pay_credit_trx_with_pin( pAUTH_TX p_auth_tx  );
INT 	ncmcr2_check_if_trx_is_mc_Swiftpass_BDO_Pay_credit_trx_without_pin( pAUTH_TX p_auth_tx  );

INT 	ncmcr2_check_if_trx_ecommerce_entry_mode_from_swiftpass_BDOPay_trx ( pAUTH_TX p_auth_tx  );
INT 	ncmcr2_check_if_trx_ecommerce_POS_entry_mode_from_swiftpass_BDOPay_trx( pAUTH_TX p_auth_tx  );
INT 	ncmcr2_check_if_trx_is_mc_ecommerce_swiftpass_BDOPay_trx( pAUTH_TX p_auth_tx  );


