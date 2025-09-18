/***************************************************************************
 *
 * Copyright (c) 2004, Hypercom, Inc.
 * All Rights Reserved.
 *
 * MODULE:      NCAMX_BITFIELDS.H
 *  
 * TITLE:       Amex Network Control Bit Field Definitions
 *  
 * DESCRIPTION: This header file contains a definition of each ISO
 *              bitfield used by this network controller.
 *
 * APPLICATION: Advanced Transaction Processor (ATP)
 *
 ****************************************************************************/

#ifndef NCEQ_BITFIELDS_H
#define NCEQ_BITFIELDS_H

#include "ncamx_prototypes.h"
#include "ncamx_constants.h"

#define BDOR_62_PREAUTH_VOID	 1

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
   bf_54AddlAmount   = 54,		/*Add amount*/
   bf_55IccData		 = 55,
   bf_56orgdata      = 56,     /* original data elements */
   bf_60priv60       = 60,     /* private field 60 */
   bf_61priv61       = 61,     /* private field 61 */
   bf_62priv62       = 62,     /* private field 62 */
   bf_63priv63		 = 63     /* private field 63 */
   
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
		  bf_5recon_amt,		/* COND */			// ThoughtFocus Girija Y: Added reconciliation amount
		  bf_7trntime,			/* OPT */
		  bf_11stan,			/* MAND */
		  bf_12datetime,			/* MAND */ 
		  bf_13date,			/* OPT */ 
		  bf_14expiry,			/* OPT */
		  bf_19ctrycd,			/* MAND */
		  bf_22posentry,		/* MAND */
		  bf_24funccd, 			/*COND*/
		  bf_26accptcd,			/* MAND */
		  bf_31tranid,			/* COND */		
		  bf_32idcode,			/* MAND */
		  bf_35track2,			/* COND */
		  bf_37rrn,				/* OPT */
		  bf_41termid,			/* OPT */
		  bf_42cardacc,			/* MAND */      
		  bf_43cardacceptloc,	/* OPT */
		  bf_45track1,		    /* COND */
		  bf_49trncurcd,		/* MAND */
		  bf_50recncurcd,		/* COND */			// ThoughtFocus Girija Y: Added reconciliation currency code
		  bf_53seccntl,			/* COND */
		  bf_55IccData,
		  bf_60priv60, 			/*COND*/
		  bf_61priv61,
		  bf_done			/* end of fields to move */
      },
	  /*****************************************************
	 	  		AUTH REFUND REQUEST
	 	  		*******************************************************/
	 	  		{
	 	  			AUTH_REFUND,
	 	  			0x31,0x31,0x30,0x30,                  /* message id, 1100 */
	 	  			0x32,0x30,0x30,0x30,0x30,0x30,        /* processing code */
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
					bf_61priv61,
				   bf_done			/* end of fields to move */
	 	  		},

	 	  		/***********************************************
	 	  		 *	AUTH REFUND  RESPONSE
	 	  		***********************************************/
	 	  		{
	 	  			AUTH_REFUND_RESPONSE,
	 	  			0x31,0x31,0x31,0x30,                  /* message id, 1110 */
	 	  			0x32,0x30,0x30,0x30,0x30,0x30,        /* processing code */
	 	  			/**** OPTIONS ****/
	 	  			topt1_postadv,
	 	  			topt2_referral+topt2_reversal+topt2_floor,
	 	  			/**** BIT FIELDS ****/
					 bf_2pan,        /* MAND */
					bf_3proc,		 /* MAND */
					bf_4amount, 	 /* MAND */
					bf_5recon_amt,  /* OPT  */
					bf_11stan,	    /* MAND */
					bf_12datetime,	    /* MAND */
					bf_24funccd,    /* COND */
					bf_32idcode,    /* MAND */
				    bf_37rrn,       /* OPT  */
					bf_38authcode,  /* COND */
					bf_39respcode,  /* MAND */
					bf_41termid,	 /* OPT  */
					bf_42cardacc,	 /* MAND */
					bf_44respdata, /*COND*/
					bf_48addldata, /* COND */
				    bf_49trncurcd,  /* MAND */
					bf_50recncurcd, /* OPT  */
					bf_55IccData,
					bf_61priv61,
				    bf_62priv62,
					bf_done      /* end of fields to move */
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
		  bf_44respdata,	    /* OPT */  /* ThoughtFocus Girija Y: Added address verification */
		  bf_49trncurcd,		/* MAND ECHO*/
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
  		  bf_31tranid,			/* COND */		
		  bf_32idcode,			/* MAND */
		  bf_33fwdinstidcode,
		  bf_35track2,			/* COND */
		  bf_37rrn,				/* OPT */
		  bf_41termid,			/* OPT */
		  bf_42cardacc,			/* MAND */      
		  bf_43cardacceptloc,	/* OPT */
		  bf_45track1,		    /* COND */
		  bf_49trncurcd,		/* MAND */
		  bf_50recncurcd,		/* COND */		/* ThoughtFocus - Girija Y */
		  bf_53seccntl,			/* COND */
		  bf_55IccData,
		  bf_60priv60, 			/*COND*/
		  bf_61priv61,
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
		  bf_44respdata,	    /* OPT */  /* ThoughtFocus Girija Y: Added address verification */
		  bf_49trncurcd,		/* MAND ECHO*/
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
		  bf_31tranid,			/* COND */		
		  bf_32idcode,			/* MAND */
		  bf_33fwdinstidcode,
		  bf_35track2,			/* COND */
		  bf_37rrn,				/* OPT */
		  bf_41termid,			/* OPT */
		  bf_42cardacc,			/* MAND */      
		  bf_43cardacceptloc,	/* OPT */
		  bf_45track1,		    /* COND */
		  bf_49trncurcd,		/* MAND */
		  bf_50recncurcd,		/* COND */		/* ThoughtFocus - Girija Y */
		  bf_53seccntl,			/* COND */
		  bf_55IccData,
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
		  bf_44respdata,	    /* OPT */  /* ThoughtFocus Girija Y: Added address verification */
		  bf_49trncurcd,		/* MAND ECHO*/
		  bf_done      /* end of fields to move */
      },
	  
	  /*QUASI CASH*/
	  {
		  /********************************************
			  P O S   AUTH QUASI CASH
		   ********************************************/
		   AUTH_QUASI_CASH,
		   0xF1,0xF1,0xF0,0xF0, 				 /* message id, 1100 */
		   0xF0,0xF0,0xF4,0xF0,0xF0,0xF0,		 /* processing code */
		   /**** OPTIONS ****/
		   topt1_postadv,
		   topt2_referral+topt2_reversal+topt2_floor,
		   /**** BIT FIELDS ****/
		   bf_2pan, 			 /* MAND */
		   bf_3proc,			 /* MAND */
		   bf_4amount,			 /* MAND */
		   bf_7trntime, 		 /* OPT */
		   bf_11stan,			 /* MAND */
		   bf_12datetime,			 /* MAND */ 
		   bf_13date,			 /* OPT */ 
		   bf_14expiry, 		 /* OPT */
		   bf_19ctrycd, 		 /* MAND */
		   bf_22posentry,		 /* MAND */
		   bf_26accptcd,		 /* MAND */
		   bf_32idcode, 		 /* MAND */
		   bf_35track2, 		 /* COND */
		   bf_37rrn,			 /* OPT */
		   bf_41termid, 		 /* OPT */
		   bf_42cardacc,		 /* MAND */ 	 
		   bf_43cardacceptloc,	 /* OPT */
		   bf_45track1, 		 /* COND */
		   bf_49trncurcd,		 /* MAND */
		   bf_53seccntl,		 /* COND */
		   bf_55IccData,
		   bf_61priv61,
		   bf_done			 /* end of fields to move */
	   },
  
	   {
	   /***********************************************
		 QUASI CASJ  R E S P O N S E
	   ***********************************************/
	   AUTH_QUASI_CASH_RESPONSE,
	   0xF1,0xF1,0xF1,0xF0, 				 /* message id, 1110 */
	   0xF0,0xF0,0xF4,0xF0,0xF0,0xF0,		 /* processing code */
	   /**** OPTIONS ****/
	   topt1_postadv,
	   topt2_referral+topt2_reversal+topt2_floor,
	   /**** BIT FIELDS ****/
	   bf_2pan, 			 /* MAND ECHO */
	   bf_3proc,			 /* MAND ECHO */
	   bf_4amount,			 /* MAND ECHO */
	   bf_7trntime, 		 /* OPT  ECHO */
	   bf_11stan,			 /* MAND ECHO*/
	   bf_12datetime,		 /* MAND ECHO*/ 
	   bf_24funccd, 		 /* COND ECHO*/
	   bf_31tranid, 		 /* COND */ 	 
	   bf_32idcode, 		 /* MAND ECHO*/
	   bf_37rrn,			 /* OPT  ECHO*/
	   bf_38authcode,		 /* COND */
	   bf_39respcode,		 /* MAND */
	   bf_41termid, 		 /* OPT  ECHO*/
	   bf_42cardacc,		 /* MAND ECHO*/ 	 
	   bf_49trncurcd,		 /* MAND ECHO*/
	   bf_done				 /* end of fields to move */
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
		  bf_5recon_amt,		/* MAND */	/* Girija Y TF, Mar 04, 2009 */
		  bf_11stan,			/* MAND */
		  bf_12datetime,			/* MAND */ 
		  bf_14expiry,			/* OPT */
		  bf_19ctrycd,          /*MAND */
		  bf_22posentry,		/* MAND */
		  bf_24funccd,			/* COND */
		  bf_26accptcd,			/* MAND */
		  bf_31tranid,			/* COND */		
		  bf_32idcode,			/* MAND */
		  bf_33fwdinstidcode,
		  bf_35track2,			/* COND */
		  bf_37rrn,             /*MAND*/
		  bf_38authcode,		/* COND */
		  bf_39respcode,		/* MAND */
  		  bf_41termid,			/* OPT  ECHO*/
		  bf_42cardacc,			/* MAND */
		  bf_43cardacceptloc,   /*MAND */
		  bf_45track1,		    /* COND */
		  bf_48addldata,		/* COND */ /* Girija Y TF, Mar 04, 2009 */
		  bf_49trncurcd,		/* MAND */
		  bf_50recncurcd,       /* MAND */ /* Girija Y TF, Mar 04, 2009 */
		  bf_55IccData,
		  bf_60priv60,         /* only for incremental auth*/
		  bf_done			/* end of fields to move */
      },



      {
		  /***********************************************************
			 A U T H O R I Z A T I O N   A D V I C E   R E S P O N S E
		  ************************************************************/
		  AUTH_AUTHORIZATION_RESPONSE,
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
   	  /* 1121 MESSAGES ADDED FOR cadience standin support rel 08-16@TF, Ravi*/
	{
		  /*********************************************************
			 A U T H O R I Z A T I O N   A D V I C E    R E P E A T
		  **********************************************************/
		  AUTH_ADVICE,
		  0xF1,0xF1,0xF2,0xF1,                  /* message id, 1121 */
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
		  bf_31tranid,			/* COND */		
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
          /*******************************************************************
		 A U T H O R I Z A T I O N   A D V I C E  R E P E A T  R E S P O N S E
		 *********************************************************************/
		  AUTH_AUTHORIZATION_RESPONSE,
		  0xF1,0xF1,0xF3,0xF1,                  /* message id, 1131 */
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
		  bf_31tranid,			/* COND */		
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
		  bf_31tranid,			/* COND */		
		  bf_32idcode,			/* MAND */
		  bf_33fwdinstidcode,
		  bf_35track2,			/* MAND */
		  bf_37rrn,				/* OPT */
		  bf_41termid,			/* MAND */
		  bf_43cardacceptloc,	/* MAND */
		  bf_49trncurcd,		/* MAND */
		  bf_50recncurcd,	    /* OPT */
		  bf_52pinblock,	    /* MAND */
		  bf_55IccData,
		  bf_done			/* end of fields to move */
      },

      {
		  /***********************************************
		   ATM	 C A S H  R E S P O N S E
		  ***********************************************/
		  AUTH_SALE_CASH_RESPONSE,
		  0xF1,0xF2,0xF1,0xF0,                  /* message id, 1210 */
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
		  bf_5recon_amt,		/* COND */	
		  bf_11stan,			/* MAND */
		  bf_12datetime,			/* MAND */ 
		  bf_24funccd,			/* MAND */
		  bf_25poscc,			/* MAND */
		  bf_31tranid,			/* COND */		
		  bf_32idcode,			/* OPT */
		  bf_33fwdinstidcode,   /* MAND */ // TF PHani
		  bf_37rrn,				/* OPT */
		  bf_38authcode,		/* OPT */
		  bf_42cardacc,			/* MAND */      
		  bf_49trncurcd,		/* MAND */
		  bf_50recncurcd,		/* COND */		/* ThoughtFocus Girija Y */
		  bf_56orgdata,			/* OPT */
		  bf_60priv60,
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
		  bf_42cardacc,			/* MAND ECHO*/      
		  bf_49trncurcd,		/* MAND ECHO*/
		  bf_50recncurcd,        /* COND ECHO */  /* ThoughtFocus Girija Y */
		  bf_60priv60,
		  bf_done				/* end of fields to move */
      },
#if BDOR_62_PREAUTH_VOID
	  /* 1420 MESSAGES FOR VOID PRE AUTH */
	   {
		  /********************************
			 VOID PRE AUTHORIZATION
		  **********************************/
		  AUTH_VOID_PRE_AUTHORIZATION,
		  0xF1,0xF4,0xF2,0xF0,                  /* message id, 1420 */
		  0xF0,0xF0,0xF4,0xF0,0xF0,0xF0,        /* processing code */
		  /**** OPTIONS ****/
		  topt1_postadv,
		  topt2_referral+topt2_reversal+topt2_floor,
		  /**** BIT FIELDS ****/
		  bf_2pan,				/* MAND */
		  bf_3proc,				/* MAND */
		  bf_4amount,			/* MAND */
		  bf_5recon_amt,		/* COND */
		  bf_11stan,			/* MAND */
		  bf_12datetime,			/* MAND */
		  bf_24funccd,			/* MAND */
		  bf_25poscc,			/* MAND */
		  bf_26accptcd,			/* COND */
		  bf_30origamt,			/* COND */
		  bf_32idcode,			/* OPT */
		  bf_33fwdinstidcode,   /* MAND */ // TF PHani
		  bf_37rrn,				/* OPT */
		  bf_38authcode,		/* OPT */
		  bf_42cardacc,			/* MAND */
		  bf_49trncurcd,		/* MAND */
		  bf_50recncurcd,		/* COND */		/* ThoughtFocus Girija Y */
		  bf_56orgdata,			/* OPT */
		  bf_60priv60,
		  bf_done			/* end of fields to move */
      },

      {
		  /***********************************************
			VOID PRE AUTH RESPONSE
		  ***********************************************/
    	  AUTH_VOID_PRE_AUTHORIZATION_RESPONSE,
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
		  bf_50recncurcd,        /* COND ECHO */  /* ThoughtFocus Girija Y */
		  bf_60priv60,
		  bf_done				/* end of fields to move */
      },
#endif
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
		  bf_5recon_amt,		/* COND */	// Tf Phani
		  bf_11stan,			/* MAND */
		  bf_12datetime,			/* MAND */ 
		  bf_24funccd,			/* MAND */
		  bf_25poscc,			/* MAND */
		  bf_26accptcd,         /* ECHO if present*/
		  bf_30origamt,			/* ECHO if present*/
		  bf_31tranid,			/* COND */		
		  bf_32idcode,			/* OPT */
		  bf_33fwdinstidcode,   /* MAND */  // TF PHANI
		  bf_37rrn,				/* OPT */
		  bf_38authcode,		/* OPT */
		  bf_41termid,			/* OPT */ // TF PHani
		  bf_42cardacc,			/* MAND */      
		  bf_49trncurcd,		/* MAND */
		  bf_50recncurcd,		/* COND */  /* ThoughtFocus Girija Y */
		  bf_56orgdata,			/* OPT */
		  bf_60priv60,
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
		  bf_42cardacc,			/* MAND ECHO*/      
		  bf_49trncurcd,		/* MAND ECHO*/
		  bf_50recncurcd,        /* COND ECHO */  /* ThoughtFocus Girija Y */
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
		  bf_3proc,				/* MAND */
		  bf_4amount,			/* MAND */
		  bf_5recon_amt,		/* OPT */
		  bf_11stan,			/* MAND */
		  bf_12datetime,		/* MAND */ 
		  bf_19ctrycd,		    /* MAND */
		  bf_24funccd,			/* MAND */
		  bf_25poscc,			/* MAND */
  	      bf_30origamt,			/* MAND */
		  bf_31tranid,			/* COND */		
		  bf_32idcode,			/* MAND ECHO */
		  bf_33fwdinstidcode,   /* MAND */  // TF PHANI
		  bf_37rrn,				/* OPT */
		  bf_41termid,			/* MAND ECHO*/
		  bf_42cardacc,			/* MAND */      // TF Phani
		  bf_49trncurcd,		/* MAND */
		  bf_50recncurcd,	    /* MAND */
		  bf_55IccData,
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
         AUTH_CARD_UPDATE,
         0xF1,0xF3,0xF0,0xF4,                  /* message id, 1304 */
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
         bf_done			/* end of fields to move */
      },
	  {
         /********************************
             O N L I N E   U P D A T E
          ********************************/
         AUTH_CARD_UPDATE_RESPONSE,			   /* Girija Y , ThoughtFocus */
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
		 bf_39respcode,		  /* MAND */
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
		  bf_11stan,			/* MAND ECHO*/
		  bf_12datetime,			/* MAND ECHO*/ 
		  bf_24funccd,			/* MAND ECHO*/
		  bf_33fwdinstidcode,   /* MAND ECHO*/
		  bf_39respcode,		/* MAND */
		  bf_done				/* end of fields to move */
      }

 };

#define N_TRANENTRY (sizeof(TRANTABLE) / sizeof(struct trandef))

#define N_BFData ( sizeof(BFData) / sizeof (struct bitfield_data) )

struct bitfield_data *Find_Bitfield_Table (enum bitfields);
struct trandef       *Find_Tran_Table     (pAUTH_TX p_auth_tx);

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
   BYTE datetime      [12];
   BYTE eff_date	  [4];
   BYTE date_expd	  [4];
   BYTE date_settle   [4];
   BYTE date_conv     [4];
   BYTE merch_type	  [4];
   BYTE ctry_code	  [3];
   BYTE coun_code     [3];
   BYTE fwd_inst_cnt_code [3];
   BYTE posentry      [12];
   BYTE func_cd       [3];
   BYTE pos_condcd    [4];
   BYTE accpt_bus_cd  [4];
   BYTE orig_amt      [24];	
   BYTE tran_fee_amt  [8];
   BYTE tranid_len	  [2];
   BYTE tranid		  [40];
   BYTE acq_inst_len  [2];
   BYTE acq_inst_id   [11];
   BYTE fwd_inst_len  [2];
   BYTE fwd_inst_id   [11];
   BYTE track2_len    [2];
   BYTE track2        [37];
   BYTE rrn			  [12];
   BYTE authcode      [6];
   BYTE responsecd    [3];
   BYTE termid        [8];
   BYTE cardaccid     [15];
   BYTE cardaccnamelen[2];
   BYTE cardaccname	  [101];
   BYTE addl_rsp_len  [2];
   BYTE addlrspdata   [25];
   BYTE track1_len    [2];
   BYTE track1        [76];
   BYTE addl_data_len [3];
   BYTE addl_data     [70];
   BYTE tran_curr_cd  [3];
   BYTE recn_curr_cd  [3];
   BYTE pinblock      [8];
   BYTE sec_cntl_len  [2];
   BYTE sec_cntl      [8];
   BYTE AddlAmts_len   [3];
   BYTE AddlAmts     [246];
   BYTE IccData_len   [3];
   BYTE IccData	    [264];
   BYTE orig_data_len [2];
   BYTE orig_data     [35];
   BYTE priv60_len	  [3];
   BYTE priv60 	      [212];
   BYTE priv61_len	  [3];
   BYTE priv61		  [99];
   BYTE priv62_len	  [3];
   BYTE priv62		  [99];
   BYTE priv63_len	  [3];
   BYTE priv63		  [99];
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
INT  EBCDICMove(           enum bitfields );
INT  RspEBCDICvMove(       enum bitfields );
INT  RspEBCDICMove(        enum bitfields );
INT  Move_IfThere( enum bitfields fieldid, BYTE p_trantype );
INT  Move_Always(  enum bitfields fieldid, BYTE p_trantype );
INT  get_variable_length(  pBYTE, INT );
INT  GenericMove(enum bitfields fieldid);
INT  RspMoveGeneric(enum bitfields fieldid);

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
   Move_IfThere,                  /* mandatory */
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
   Move_IfThere,                  /* conditional */
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
   Move_IfThere,                  /* conditional */
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
   Move_IfThere,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_43cardacceptloc,                 /* field */
   HOST_MESSAGE.cardaccnamelen,       /* data to move */
   2,                           /* # of bytes in  iso field */
   Move_IfThere,                  /* conditional */
   vEBCDICMove,                   /* move routine */
   RspEBCDICvMove                 /* response routine */
   },
   {
   bf_44respdata,                 /* field */
   HOST_MESSAGE.addl_rsp_len,       /* data to move */
   2,                           /* # of bytes in  iso field */
   Move_Always,                  /* conditional */
   vEBCDICMove,                   /* move routine */
   RspEBCDICvMove                 /* response routine */
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
   8,                            /* # of bytes in  iso field */
   Move_Always,                  /* mandatory */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_53seccntl,                 /* field */
   HOST_MESSAGE.sec_cntl_len,    /* data to move */
   2,                            /* # of bytes in field */
   Move_IfThere,                   /* mandatory    */
   vEBCDICMove,                   /* move routine */
   RspEBCDICvMove                 /* response routine */
   },
   {
   bf_54AddlAmount,                 /* field */
   HOST_MESSAGE.AddlAmts_len,   /* data to move */
   3,                            /* # of bytes in field */
   Move_IfThere,                  /* mandatory    */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
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
   bf_60priv60,                 /* field */   
   HOST_MESSAGE.priv60_len,     /* data to move */  
   3,                           /* # of bytes in field */   
   Move_IfThere,                /* mandatory    */   
   vEBCDICMove,                  /* move routine */   
   RspEBCDICvMove                /* response routine */   
   },
   {   
   bf_61priv61,                 /* field */   
   HOST_MESSAGE.priv61_len,     /* data to move */  
   3,                           /* # of bytes in field */   
   Move_IfThere,                /* mandatory    */   
   vEBCDICMove,                  /* move routine */   
   RspEBCDICvMove                /* response routine */   
   },
   {   
   bf_62priv62,                 /* field */   
   HOST_MESSAGE.priv62_len,     /* data to move */  
   3,                           /* # of bytes in field */   
   Move_IfThere,                /* mandatory    */   
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






#endif
