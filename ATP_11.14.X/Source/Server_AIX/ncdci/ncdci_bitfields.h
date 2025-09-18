/***************************************************************************
 *
 * Copyright (c) 2004, Hypercom, Inc.
 * All Rights Reserved.
 *
 * MODULE:      ncdci_bitfields.h
 *  
 * TITLE:       ncdci Network Control Bit Field Definitions
 *  
 * DESCRIPTION: This header file contains a definition of each ISO
 *              bitfield used by this network controller.
 *
 * APPLICATION: Advanced Transaction Processor (ATP)
 *
 * AUTHOR:  Abhishek Verma
 *
 ****************************************************************************/

#ifndef NCEQ_BITFIELDS_H
#define NCEQ_BITFIELDS_H

#include "diners.h"
#include "ncdci_prototypes.h"

#define BDOR_62_PREAUTH_VOID	1

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

enum bitfields 
{
   bf_done                 = 0,      /* end of list */
   bf_1xbitfield           = 1,      /* extended bit field */
   bf_2pan                 = 2,      /* Pan */
   bf_3proc                = 3,      /* Processing code */
   bf_4amount              = 4,      /* Transaction Amount */
   bf_6cardbillamt         = 6,      /* Cardholder Billing Amount */
   bf_7trntime             = 7,      /* Transmission date-time */
   bf_11stan               = 11,     /* System trace number */
   bf_12time               = 12,     /* Local Transaction time */
   bf_13date               = 13,     /* Local Transaction date */
   bf_14expiry		       = 14,	 /* Expiry date */
   bf_22posentry           = 22,     /* Pos entry mode  */
   bf_23csn                = 23,	 /* Card Sequence Number */
   bf_24functioncode       = 24,     /* Function Code */
   bf_25poscc              = 25,     /* Message Reason Code  */
   bf_26pincode		       = 26,	 /* Card Acceptor Business Code (Abc) */
   bf_30originalamt        = 30,     /* Original Amounts  */
   bf_32idcode             = 32,     /* Acquiring institution's ID code  */
   bf_33fwdinstidcode      = 33,     /* Forwarding institution ID code */
   bf_35track2             = 35,     /* Track 2 data */
   bf_37rrn	               = 37,	 /* Retrieval ref number */
   bf_38authcode           = 38,     /* Auth identification response/code */
   bf_39respcode           = 39,     /* Action code */
   bf_40service_code	   = 40,     /* Service Code (CVV2)*/
   bf_41termid             = 41,     /* Terminal id  */
   bf_42cardacc            = 42,     /* Card acceptor id  */
   bf_43cardacceptloc      = 43,     /* Card acceptor name/location */
   bf_44respdata	       = 44,     /* Additional response data */
   bf_45track1             = 45,     /* Track 1 data */
   bf_46surchargfee        = 46,     /* Surcharge Fee*/
   bf_48addldata           = 48,     /* Additional private data */
   bf_49trncurcd           = 49,     /* Transaction currency code */
   bf_51crdcurcd           = 51,     /* Cardholder Billing Currency Code */
   bf_52pinblock           = 52,     /* Pin block */
   bf_54AddlAmount         = 54,     /* Additional amount */
   bf_55IccData		       = 55,     /* IC Card data*/
   bf_56oridataelement     = 56,     /* Original Data Elements */
   bf_58authagentIIC       = 58,	 /* Authorizing Agent Institution Identification Code (IIC) */
   bf_59NettrandataID      = 59,     /* Network ID Transport Data */
   bf_62addauthdata 	   = 62,     /* Additional Authorization Data */
   bf_64mac				   = 64,     /* Message Authentication Code (MAC)*/
   bf_72datarecord         = 72,     /* Data Record */
   bf_92txorigIntcntrycode = 92,     /* Transaction Originator Institution Country Code*/
   bf_93txdestIIC          = 93,     /* Transaction Destination Institution Identification Code (IIC) */
   bf_94txorigIIC          = 94,     /* Transaction Originator Institution Identification Code (IIC) */
   bf_96keymangdata        = 96,     /* Key Management Data */
   bf_100revinsidcode      = 100,    /* Receiving Institution Identification Code */
   bf_101filename          = 101,    /* File Name */
   bf_104Transpecfdata	   = 104,	 /* Transactional specific data */
   bf_106Trandata		   = 106,    /* Transactional Data */
   bf_122addauthendata     = 122,    /* Additional Authentication Data */
   bf_123netrefID          = 123,    /* Network Reference Id (NRID) */
   bf_124txqualifier       = 124,    /* Transaction Qualifier */
   bf_125replacementpin    = 125     /* Replacement PIN */

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
   enum   bitfields  td_bitfields[DINERS_EXTENDED_BITFIELD_SIZE]; /* fields to put in msg */
};


static  struct trandef TRANTABLE[] = {

	/*****************************************************
	AUTHORIZATION REQUEST
	*******************************************************/
	{
		AUTH_AUTHORIZATION,
		0x31,0x31,0x30,0x30,                  /* message id, 1100 */
		0x30,0x30,0x30,0x30,0x30,0x30,        /* processing code */
		/**** OPTIONS ****/
		topt1_postadv,
		topt2_referral+topt2_reversal+topt2_floor,
		/**** BIT FIELDS ****/
		bf_2pan,							/* MAND */
		bf_3proc,							/* MAND */
		bf_4amount,							/* MAND */
		bf_6cardbillamt,
		bf_7trntime,						/* MAND */
		bf_11stan,							/* MAND */
		bf_12time,							/* MAND */
		bf_13date,							/* MAND */
		bf_14expiry,						/* OPTN  */
		bf_22posentry,						/* MAND */
		bf_23csn,                 			/* MAND */
		bf_24functioncode,					/* MAND */
		bf_25poscc, 	          			/* MAND */
		bf_26pincode,  	        			/* COND */
		bf_32idcode,						/* MAND */
		bf_33fwdinstidcode,	    			/* MAND */
		bf_35track2,						/* COND */
		bf_37rrn,							/* MAND */
		bf_40service_code,                  /* COND */
		bf_41termid,						/* MAND */
		bf_42cardacc,						/* MAND */
		bf_43cardacceptloc,					/* MAND */
		bf_45track1,		    			/* COND */
		bf_48addldata,         				/* OPTN  */
		bf_49trncurcd,						/* MAND */
		bf_51crdcurcd,
		bf_52pinblock,        				/* COND */
		bf_55IccData,         				/* COND */
		bf_62addauthdata,                   /* COND*/
		bf_92txorigIntcntrycode,			/*MAND*/
		bf_104Transpecfdata,				/*COND*/
		bf_106Trandata,						/*COND*/
		bf_122addauthendata,        		/* OPTN  */
		bf_done			        /* end of fields to move */
	},

	/***********************************************
	 *	AUTHORIZATION   RESPONSE
	***********************************************/
	{
		AUTH_AUTHORIZATION_RESPONSE,
		0x31,0x31,0x31,0x30,                  /* message id, 1110 */
		0x30,0x30,0x30,0x30,0x30,0x30,        /* processing code */
		/**** OPTIONS ****/
		topt1_postadv,
		topt2_referral+topt2_reversal+topt2_floor,
		/**** BIT FIELDS ****/
		bf_2pan,				    		/* MAND ECHO */
		bf_3proc,				    		/* MAND ECHO */
		bf_4amount,			    			/* MAND ECHO */
		bf_6cardbillamt,					/* MAND ECHO */
		bf_7trntime,			    		/* MAND ECHO */
		bf_11stan,			    			/* MAND ECHO */
		bf_12time,		        			/* MAND ECHO */
		bf_13date,                			/* MAND ECHO */
		bf_14expiry,              			/* MAND PREP */
		bf_23csn,
		bf_24functioncode,					/* MAND */
		bf_25poscc,               			/* MAND ECHO */
		bf_26pincode,						/* MAND ECHO */
		bf_32idcode,              			/* MAND ECHO */
		bf_33fwdinstidcode,       			/* MAND ECHO */
		bf_37rrn,				    		/* MAND ECHO */
		bf_38authcode,		    			/* COND PREP */
		bf_39respcode,		    			/* MAND PREP */
		bf_41termid,			    		/* MAND ECHO */
		bf_42cardacc,			    		/* MAND ECHO */
		bf_44respdata,	        			/* OPTN PREP */
		bf_48addldata,
		bf_49trncurcd,		    			/* MAND ECHO */
		bf_51crdcurcd,						/* MAND ECHO */
		bf_54AddlAmount,          			/* OPTN PREP */
		bf_55IccData,
		bf_92txorigIntcntrycode,			/*MAND*/
		bf_100revinsidcode,       			/* MAND ECHO */
		bf_123netrefID, 					/* MAND ECHO */
		bf_done				    /* end of fields to move */
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
			bf_2pan,							/* MAND */
			bf_3proc,							/* MAND */
			bf_4amount,							/* MAND */
			bf_6cardbillamt,
			bf_7trntime,						/* MAND */
			bf_11stan,							/* MAND */
			bf_12time,							/* MAND */
			bf_13date,							/* MAND */
			bf_14expiry,						/* OPTN  */
			bf_22posentry,						/* MAND */
			bf_23csn,                 			/* MAND */
			bf_24functioncode,					/* MAND */
			bf_25poscc, 	          			/* MAND */
			bf_26pincode,  	        			/* COND */
			bf_32idcode,						/* MAND */
			bf_33fwdinstidcode,	    			/* MAND */
			bf_35track2,						/* COND */
			bf_37rrn,							/* MAND */
			bf_40service_code,                  /* COND */
			bf_41termid,						/* MAND */
			bf_42cardacc,						/* MAND */
			bf_43cardacceptloc,					/* MAND */
			bf_45track1,		    			/* COND */
			bf_48addldata,         				/* OPTN  */
			bf_49trncurcd,						/* MAND */
			bf_51crdcurcd,
			bf_52pinblock,        				/* COND */
			bf_55IccData,         				/* COND */
			bf_62addauthdata,                   /* COND*/
			bf_92txorigIntcntrycode,			/*MAND*/
			bf_106Trandata,						/*COND*/
			bf_122addauthendata,        		/* OPTN  */
			bf_done			        /* end of fields to move */
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
			bf_2pan,				    		/* MAND ECHO */
			bf_3proc,				    		/* MAND ECHO */
			bf_4amount,			    			/* MAND ECHO */
			bf_6cardbillamt,					/* MAND ECHO */
			bf_7trntime,			    		/* MAND ECHO */
			bf_11stan,			    			/* MAND ECHO */
			bf_12time,		        			/* MAND ECHO */
			bf_13date,                			/* MAND ECHO */
			bf_14expiry,              			/* MAND PREP */
			bf_23csn,
			bf_24functioncode,					/* MAND */
			bf_25poscc,               			/* MAND ECHO */
			bf_26pincode,						/* MAND ECHO */
			bf_32idcode,              			/* MAND ECHO */
			bf_33fwdinstidcode,       			/* MAND ECHO */
			bf_37rrn,				    		/* MAND ECHO */
			bf_38authcode,		    			/* COND PREP */
			bf_39respcode,		    			/* MAND PREP */
			bf_41termid,			    		/* MAND ECHO */
			bf_42cardacc,			    		/* MAND ECHO */
			bf_44respdata,	        			/* OPTN PREP */
			bf_48addldata,
			bf_49trncurcd,		    			/* MAND ECHO */
			bf_51crdcurcd,						/* MAND ECHO */
			bf_54AddlAmount,          			/* OPTN PREP */
			bf_55IccData,
			bf_92txorigIntcntrycode,			/*MAND*/
			bf_100revinsidcode,       			/* MAND ECHO */
			bf_123netrefID, 					/* MAND ECHO */
			bf_done				    /* end of fields to move */
		},
	/********************************************
	*	BALANCE INQUIRY REQUEST
	********************************************/
	{
		AUTH_BALANCE_INQUIRY,
		0x31,0x31,0x30,0x30,				/* message id */
		0x30,0x30,0x30,0x30,0x30,0x30, 		/* processing code */
		/**** OPTIONS ****/
		topt1_postadv,
		topt2_referral+topt2_reversal+topt2_floor,
		/**** BIT FIELDS ****/
		bf_2pan,							/* MAND */
		bf_3proc,							/* MAND */
		bf_7trntime,						/* MAND */
		bf_11stan,							/* MAND */
		bf_12time,							/* MAND */
		bf_13date,							/* MAND */
		bf_14expiry,						/* OPTN */
		bf_22posentry,						/* MAND */
		bf_23csn,							/* OPTN */
		bf_24functioncode,					/* MAND */
		bf_25poscc, 	          			/* MAND */
		bf_26pincode,  	        			/* COND */
		bf_32idcode,						/* MAND */
		bf_33fwdinstidcode,	   				/* MAND */
		bf_35track2,						/* COND */
		bf_37rrn,							/* MAND */
		bf_40service_code,                  /* COND */
		bf_41termid,						/* MAND */
		bf_42cardacc,						/* MAND */
		bf_43cardacceptloc,					/* MAND */
		bf_45track1,		    			/* COND */
		bf_48addldata,         				/* OPTN  */
		bf_49trncurcd,						/* OPTN  */
		bf_52pinblock,        				/* COND */
		bf_55IccData,         				/* COND */
		bf_62addauthdata,                   /* COND*/
		bf_92txorigIntcntrycode,			/*MAND*/
		bf_104Transpecfdata,				/*COND*/
		bf_106Trandata,						/*COND*/
		bf_done			        	/* end of fields to move */
	},

	/***************************************************
	*	BALANCE INQUIRY RESPONSE
	****************************************************/
	{
		AUTH_BALANCE_INQUIRY_RESPONSE,
		0x31,0x31,0x31,0x30,				/* message id */
		0x30,0x30,0x30,0x30,0x30,0x30, 		/* processing code */
		/**** OPTIONS ****/
		topt1_postadv,
		topt2_referral+topt2_reversal+topt2_floor,
		/**** BIT FIELDS ****/
		bf_2pan,				    		/* MAND ECHO */
		bf_3proc,				    		/* MAND ECHO */
		bf_6cardbillamt,					/* MAND ECHO */
		bf_7trntime,			   	 		/* MAND ECHO */
		bf_11stan,			    			/* MAND ECHO */
		bf_12time,		       				/* MAND ECHO */
		bf_13date,               			/* MAND ECHO */
		bf_14expiry,              			/* MAND PREP */
		bf_23csn,							/* OPTN */
		bf_24functioncode,					/* MAND */
		bf_25poscc,               			/* MAND ECHO */
		bf_26pincode,						/* MAND ECHO */
		bf_32idcode,              			/* MAND ECHO */
		bf_33fwdinstidcode,       			/* MAND ECHO */
		bf_37rrn,				    		/* MAND ECHO */
		bf_39respcode,		   				/* MAND PREP */
		bf_41termid,			    		/* MAND ECHO */
		bf_42cardacc,			    		/* MAND ECHO */
		bf_49trncurcd,		    			/* MAND ECHO */
		bf_51crdcurcd,						/* MAND ECHO */
		bf_54AddlAmount,
		bf_55IccData,
		//bf_62addauthdata,					/* COND */
		bf_92txorigIntcntrycode,			/*MAND*/
		bf_100revinsidcode,       			/* MAND ECHO */
		bf_123netrefID, 					/* MAND ECHO */
		bf_done				    /* end of fields to move */
	},

	/*****************************************************
	*	PRE AUTHORIZATION REQUEST
	*******************************************************/
	{
		AUTH_PRE_AUTHORIZATION,
		0x31,0x31,0x30,0x30,                  /* message id, 1100 */
		0x30,0x33,0x30,0x30,0x30,0x30,        /* processing code */
		/**** OPTIONS ****/
		topt1_postadv,
		topt2_referral+topt2_reversal+topt2_floor,
		/**** BIT FIELDS ****/
		bf_2pan,							/* MAND */
		bf_3proc,							/* MAND */
		bf_4amount,							/* MAND */
		bf_7trntime,						/* MAND */
		bf_11stan,							/* MAND */
		bf_12time,							/* MAND */
		bf_13date,							/* MAND */
		bf_14expiry,						/* OPTN  */
		bf_22posentry,						/* MAND */
		bf_23csn,                 			/* MAND */
		bf_24functioncode,					/* MAND */
		bf_25poscc, 	          			/* MAND */
		bf_26pincode,  	        			/* COND */
		bf_32idcode,						/* MAND */
		bf_33fwdinstidcode,	    			/* MAND */
		bf_35track2,						/* COND */
		bf_37rrn,							/* MAND */
		bf_40service_code,                  /* COND */
		bf_41termid,						/* MAND */
		bf_42cardacc,						/* MAND */
		bf_43cardacceptloc,					/* MAND */
		bf_45track1,		    			/* COND */
		bf_48addldata,         				/* OPTN */
		bf_49trncurcd,						/* MAND */
		bf_52pinblock,        				/* COND */
		bf_55IccData,         				/* COND */
		bf_62addauthdata,                   /* COND*/
		bf_92txorigIntcntrycode,			/*MAND*/
		bf_104Transpecfdata,				/*COND*/
		bf_106Trandata,						/*COND*/
		bf_done			        /* end of fields to move */
	},

	/***********************************************
	*	PRE AUTHORIZATION  RESPONSE
	***********************************************/
	{
		AUTH_PRE_AUTHORIZATION_RESPONSE,
		0x31,0x31,0x31,0x30,                  /* message id, 0110 */
		0x30,0x33,0x30,0x30,0x30,0x30,        /* processing code */
		/**** OPTIONS ****/
		topt1_postadv,
		topt2_referral+topt2_reversal+topt2_floor,
		/**** BIT FIELDS ****/
		bf_2pan,				    		/* MAND ECHO */
		bf_3proc,				   			/* MAND ECHO */
		bf_4amount,			    			/* MAND ECHO */
		bf_6cardbillamt,					/* MAND ECHO */
		bf_7trntime,			    		/* MAND ECHO */
		bf_11stan,			    			/* MAND ECHO */
		bf_12time,		        			/* MAND ECHO */
		bf_13date,                			/* MAND ECHO */
		bf_14expiry,              			/* MAND PREP  */
		bf_23csn,
		bf_24functioncode,					/* MAND */
		bf_25poscc,               			/* MAND ECHO */
		bf_26pincode,						/* MAND ECHO */
		bf_32idcode,              			/* MAND ECHO */
		bf_33fwdinstidcode,       			/* MAND ECHO */
		bf_37rrn,				    		/* MAND ECHO */
		bf_38authcode,		    			/* COND PREP  */
		bf_39respcode,		    			/* MAND PREP  */
		bf_41termid,			    		/* MAND ECHO */
		bf_42cardacc,			    		/* MAND ECHO */
		bf_44respdata,	        			/* OPTN PREP   */
		bf_49trncurcd,		    			/* MAND ECHO */
		bf_51crdcurcd,						/* MAND ECHO */
		bf_54AddlAmount,          			/* OPTN PREP   */
		bf_55IccData,
		bf_92txorigIntcntrycode,			/*MAND*/
		bf_100revinsidcode,       			/* MAND ECHO */
		bf_123netrefID, 					/* MAND ECHO */
		bf_done				    /* end of fields to move */
	},

	/*****************************************************
		CARD VERIFICATION REQUEST
	*******************************************************/
	{
		AUTH_CARD_VERIFICATION,
		0x31,0x31,0x30,0x30,				   /* message id, 1100 */
		0x30,0x33,0x32,0x30,0x30,0x30, 	   /* processing code */
		/**** OPTIONS ****/
		topt1_postadv,
		topt2_referral+topt2_reversal+topt2_floor,
		/**** BIT FIELDS ****/
		bf_2pan,				   			/* MAND */
		bf_3proc,				   			/* MAND */
		bf_4amount,			   				/* MAND */
		bf_7trntime,			   			/* MAND */
		bf_11stan, 			   				/* MAND */
		bf_12time, 			   				/* MAND */
		bf_13date, 			   				/* MAND */
		bf_14expiry,			   			/* OPTN	*/
		bf_22posentry, 		   				/* MAND */
		bf_23csn,				   			/* MAND */
		bf_24functioncode,					/* MAND */
		bf_25poscc,			   				/* MAND */
		bf_26pincode,			   			/* COND */
		bf_32idcode,			   			/* MAND */
		bf_33fwdinstidcode,	   				/* MAND */
		bf_35track2,			   			/* COND */
		bf_37rrn,				   			/* MAND */
		bf_40service_code,                  /* COND */
		bf_41termid,			   			/* MAND */
		bf_42cardacc,			   			/* MAND */
		bf_43cardacceptloc,	   				/* MAND */
		bf_45track1,			   			/* COND */
		bf_48addldata, 		   				/* OPTN	*/
		bf_49trncurcd, 		   				/* MAND */
		bf_52pinblock, 		   				/* COND */
		bf_55IccData,			   			/* COND */
		bf_62addauthdata,                   /* COND*/
		bf_92txorigIntcntrycode,			/*MAND*/
		bf_104Transpecfdata,				/*COND*/
		bf_106Trandata,						/*COND*/
		bf_done				   /* end of fields to move */
	},

	/***********************************************
	*	CARD VERIFICATION RESPONSE
	***********************************************/
	{
		AUTH_CARD_VERIFICATION_RESPONSE,
		0x31,0x31,0x31,0x30,				   /* message id, 0110 */
		0x30,0x33,0x32,0x30,0x30,0x30, 	   /* processing code */
		/**** OPTIONS ****/
		topt1_postadv,
		topt2_referral+topt2_reversal+topt2_floor,
		/**** BIT FIELDS ****/
		bf_2pan,				   			/* MAND ECHO */
		bf_3proc,				  			/* MAND ECHO */
		bf_4amount,			   				/* MAND ECHO */
		bf_6cardbillamt,					/* MAND ECHO */
		bf_7trntime,			   			/* MAND ECHO */
		bf_11stan, 			   				/* MAND ECHO */
		bf_12time, 			   				/* MAND ECHO */
		bf_13date, 			   				/* MAND ECHO */
		bf_14expiry,			  			/* MAND PREP  */
		bf_23csn,
		bf_24functioncode,					/* MAND */
		bf_25poscc,			   				/* MAND ECHO */
		bf_26pincode,						/* MAND ECHO */
		bf_32idcode,			   			/* MAND ECHO */
		bf_33fwdinstidcode,	   				/* MAND ECHO */
		bf_37rrn,				   			/* MAND ECHO */
		bf_38authcode, 		   				/* COND PREP  */
		bf_39respcode, 		   				/* MAND PREP  */
		bf_41termid,			   			/* MAND ECHO */
		bf_42cardacc,			   			/* MAND ECHO */
		bf_44respdata, 		   				/* OPTN PREP   */
		bf_49trncurcd, 		   				/* MAND ECHO */
		bf_51crdcurcd,						/* MAND ECHO */
		bf_54AddlAmount,		   			/* OPTN PREP   */
		bf_55IccData,
		bf_92txorigIntcntrycode,			/*MAND*/
		bf_100revinsidcode,       			/* MAND ECHO */
		bf_123netrefID, 					/* MAND ECHO */
		bf_done				   /* end of fields to move */
	},

	/********************************************
	*	POS SALE REQUEST
	********************************************/
	{
		AUTH_SALE,
		0x31,0x31,0x30,0x30,                  /* message id, 1100 */
		0x30,0x30,0x30,0x30,0x30,0x30,        /* processing code */
		/**** OPTIONS ****/
		topt1_postadv,
		topt2_referral+topt2_reversal+topt2_floor,
		/**** BIT FIELDS ****/
		bf_2pan,                			/* MAND */
		bf_3proc,               			/* MAND */
		bf_4amount,             			/* MAND */
		bf_7trntime,            			/* MAND */
		bf_11stan,              			/* MAND */
		bf_12time,              			/* MAND */
		bf_13date,              			/* MAND */
		bf_14expiry,		    			/* OPTN  */
		bf_22posentry,          			/* MAND */
		bf_23csn,
		bf_24functioncode,					/* MAND */
		bf_25poscc,             			/* MAND */
		bf_26pincode,		    			/* COND */
		bf_32idcode,            			/* MAND */
		bf_33fwdinstidcode,     			/* MAND */
		bf_35track2,            			/* COND */
		bf_37rrn,	            			/* MAND */
		bf_40service_code,                  /* COND */
		bf_41termid,            			/* MAND */
		bf_42cardacc,           			/* MAND */
		bf_43cardacceptloc,     			/* MAND */
		bf_45track1,            			/* COND */
		bf_48addldata,          			/* COND */
		bf_49trncurcd,          			/* MAND */
		bf_52pinblock,          			/* COND */
		bf_55IccData,
		//bf_62addauthdata,                   /* COND*/
		bf_92txorigIntcntrycode,			/*MAND*/
		bf_106Trandata,						/*COND*/
		bf_done			        /* end of fields to move */
	},

	/***********************************************
	*	POS SALE RESPONSE
	***********************************************/
	{
		AUTH_SALE_RESPONSE,
		0x31,0x31,0x31,0x30,                  /* message id, 0110 */
		0x30,0x30,0x34,0x30,0x30,0x30,        /* processing code */
		/**** OPTIONS ****/
		topt1_postadv,
		topt2_referral+topt2_reversal+topt2_floor,
		/**** BIT FIELDS ****/
		bf_2pan,                			/* MAND ECHO */
		bf_3proc,               			/* MAND ECHO */
		bf_4amount,             			/* MAND ECHO */
		bf_6cardbillamt,					/* MAND ECHO */
		bf_7trntime,            			/* MAND ECHO */
		bf_11stan,              			/* MAND ECHO */
		bf_12time,              			/* MAND ECHO */
		bf_13date,              			/* MAND ECHO */
		bf_14expiry,		    			/* MAND PREP  */
		bf_23csn,
		bf_24functioncode,					/* MAND */
		bf_25poscc,             			/* MAND ECHO */
		bf_26pincode,						/* MAND ECHO */
		bf_32idcode,            			/* MAND ECHO */
		bf_33fwdinstidcode,     			/* MAND ECHO */
		bf_37rrn,	            			/* MAND ECHO */
		bf_38authcode,          			/* OPTN PREP */
		bf_39respcode,          			/* MAND PREP */
		bf_41termid,            			/* MAND ECHO */
		bf_42cardacc,           			/* MAND ECHO */
		bf_44respdata,	        			/* OPTN PREP */
		bf_49trncurcd,          			/* MAND ECHO */
		bf_51crdcurcd,                      /* MAND ECHO */
		bf_54AddlAmount,        			/* OPTI PREP */
		bf_55IccData,
		bf_92txorigIntcntrycode,			/*MAND*/
		bf_100revinsidcode,                 /* MAND ECHO */
		bf_123netrefID,                     /* MAND ECHO */
		bf_done                 /* end of fields to move */
	},

	/*****************************************************
	*	VOID SALE REQUEST
	*******************************************************/
	{
		AUTH_VOID_SALE,
		0x31,0x34,0x32,0x30,					/* message id, 1420 */
		0x30,0x30,0x30,0x30,0x30,0x30,		/* processing code */
		/**** OPTIONS ****/
		topt1_postadv,
		topt2_referral+topt2_reversal+topt2_floor,
		/**** BIT FIELDS ****/
		bf_2pan,							/* MAND */
		bf_3proc, 							/* MAND */
		bf_4amount,							/* MAND */
		bf_7trntime,						/* MAND */
		bf_11stan,							/* MAND */
		bf_12time,							/* MAND */
		bf_13date,							/* MAND */
		bf_14expiry,						/* OPTN  */
		bf_22posentry,						/* MAND */
		bf_23csn, 							/* MAND */
		bf_24functioncode,					/* MAND */
		bf_26pincode, 						/* COND */
		bf_32idcode,						/* MAND */
		bf_33fwdinstidcode,					/* MAND */
		bf_35track2,						/* COND */
		bf_37rrn, 							/* MAND */
		bf_38authcode,						/* MAND */
		bf_40service_code,                  /* COND */
		bf_41termid,						/* MAND */
		bf_42cardacc, 						/* MAND */
		bf_43cardacceptloc,					/* MAND */
		bf_45track1,						/* COND */
		bf_48addldata,						/* OPTN */
		bf_49trncurcd,						/* MAND */
		bf_52pinblock,						/* COND */
		bf_55IccData, 						/* COND */
		bf_56oridataelement,                /* MAND */
		//bf_62addauthdata,                   /* COND*/
		bf_106Trandata,						/*COND*/
		bf_123netrefID,                     /* MAND */

		bf_done					/* end of fields to move */
	},

	{
	/***********************************************
	*	AUTH_VOID_SALE RESPONSE
	***********************************************/
		AUTH_VOID_SALE_RESPONSE,
		0x31,0x34,0x33,0x30,					/* message id, 1430 */
		0x30,0x30,0x30,0x30,0x30,0x30,		/* processing code */
		/**** OPTIONS ****/
		topt1_postadv,
		topt2_referral+topt2_reversal+topt2_floor,
		/**** BIT FIELDS ****/
		bf_2pan,							/* MAND ECHO */
		bf_3proc, 							/* MAND ECHO */
		bf_4amount,							/* MAND ECHO */
		bf_7trntime,						/* MAND ECHO */
		bf_11stan,							/* MAND ECHO */
		bf_12time,							/* MAND ECHO */
		bf_13date,							/* MAND ECHO */
		bf_14expiry,						/* MAND PREP  */
		bf_23csn,
		bf_24functioncode,					/* MAND */
		bf_25poscc,							/* MAND ECHO */
		bf_32idcode,						/* MAND ECHO */
		bf_33fwdinstidcode,					/* MAND ECHO */
		bf_37rrn, 							/* MAND ECHO */
		bf_38authcode,						/* COND PREP  */
		bf_39respcode,						/* MAND PREP  */
		bf_41termid,						/* MAND ECHO */
		bf_42cardacc, 						/* MAND ECHO */
		bf_44respdata,						/* OPTN PREP   */
		bf_49trncurcd,						/* MAND ECHO */
		bf_54AddlAmount,					/* OPTN PREP   */
		bf_55IccData,
		bf_100revinsidcode,       			/* MAND ECHO */
		bf_123netrefID, 					/* MAND ECHO */
		bf_done					/* end of fields to move */
	},

	/********************************************
	*	CASH ADVANCE REQUEST
	********************************************/
	{
		AUTH_CASH_ADVANCE,
		0x31,0x31,0x30,0x30,                  /* message id, 1100 */
		0x30,0x31,0x34,0x30,0x30,0x30,        /* processing code */
		/**** OPTIONS ****/
		topt1_postadv,
		topt2_referral+topt2_reversal+topt2_floor,
		/**** BIT FIELDS ****/
		bf_2pan,                			/* MAND */
		bf_3proc,               			/* MAND */
		bf_4amount,             			/* MAND */
		bf_6cardbillamt,        			/* COND */
		bf_7trntime,            			/* MAND */
		bf_11stan,              			/* MAND */
		bf_12time,              			/* MAND */
		bf_13date,              			/* MAND */
		bf_14expiry,		    			/* OPTN  */
		bf_22posentry,          			/* MAND */
		bf_23csn,
		bf_24functioncode,					/* MAND */
		bf_25poscc,             			/* MAND */
		bf_26pincode,		    			/* COND */
		bf_32idcode,            			/* MAND */
		bf_33fwdinstidcode,     			/* MAND */
		bf_35track2,            			/* COND */
		bf_37rrn,	            			/* MAND */
		bf_40service_code,                  /* COND */
		bf_41termid,            			/* MAND */
		bf_42cardacc,           			/* MAND */
		bf_43cardacceptloc,    				/* MAND */
		bf_44respdata,						/* OPTN PREP   */
		bf_45track1,            			/* COND */
		bf_48addldata,          			/* COND */
		bf_49trncurcd,          			/* MAND */
		bf_51crdcurcd,          			/* COND */
		bf_52pinblock,          			/* COND */
		bf_55IccData,						/* COND */
		bf_58authagentIIC,					/* COND */
		//bf_62addauthdata,                   /* COND*/
		bf_92txorigIntcntrycode,			/*MAND*/
		bf_100revinsidcode,       			/* MAND ECHO */
		bf_106Trandata,						/*COND*/
		bf_123netrefID, 					/* MAND ECHO */
		bf_done			        /* end of fields to move */
	},

	/***********************************************
	 *  CASH ADVANCE RESPONSE
	***********************************************/
	{
		AUTH_CASH_ADVANCE_RESPONSE,
		0x31,0x31,0x31,0x30,                  /* message id, 1110 */
		0x30,0x31,0x30,0x30,0x30,0x30,        /* processing code */
		/**** OPTIONS ****/
		topt1_postadv,
		topt2_referral+topt2_reversal+topt2_floor,
		/**** BIT FIELDS ****/
		bf_2pan,                			/* MAND ECHO */
		bf_3proc,               			/* MAND ECHO */
		bf_4amount,             			/* MAND ECHO */
		bf_6cardbillamt,					/* MAND ECHO */
		bf_7trntime,            			/* MAND ECHO */
		bf_11stan,              			/* MAND ECHO */
		bf_12time,              			/* MAND ECHO */
		bf_13date,              			/* MAND ECHO */
		bf_14expiry,		    			/* MAND PREP  */
		bf_23csn,
		bf_24functioncode,					/* MAND */
		bf_25poscc,             			/* MAND ECHO */
		bf_26pincode,						/* MAND ECHO */
		bf_32idcode,            			/* MAND ECHO */
		bf_33fwdinstidcode,     			/* MAND ECHO */
		bf_37rrn,	            			/* MAND ECHO */
		bf_38authcode,          			/* OPTN PREP */
		bf_39respcode,          			/* MAND PREP */
		bf_41termid,            			/* MAND ECHO */
		bf_42cardacc,           			/* MAND ECHO */
		bf_44respdata,	        			/* OPTN PREP */
		bf_49trncurcd,          			/* MAND ECHO */
		bf_51crdcurcd,						/* MAND ECHO */
		bf_54AddlAmount,        			/* OPTI PREP */
		bf_55IccData,
		bf_58authagentIIC,					/* COND  ECHO*/
		bf_92txorigIntcntrycode,			/*MAND*/
		bf_100revinsidcode,       			/* MAND ECHO */
		bf_123netrefID, 					/* MAND ECHO */
		bf_done                 /* end of fields to move */
	},

	/********************************************
	 *  ADVICE REQUEST
	********************************************/
	{
		AUTH_ADVICE,
		0x31,0x31,0x32,0x30,                  /* message id, 1120 */
		0x30,0x30,0x30,0x30,0x30,0x30,        /* processing code */
		/**** OPTIONS ****/
		topt1_postadv,
		topt2_referral+topt2_reversal+topt2_floor,
		/**** BIT FIELDS ****/
		bf_2pan,                			/* MAND ECHO */
		bf_3proc,               			/* MAND 00X000 */
		bf_4amount,             			/* MAND PREP  */
		bf_7trntime,           				/* MAND PREP  */
		bf_11stan,              			/* MAND PREP  */
		bf_12time,              			/* MAND PREP  */
		bf_13date,              			/* MAND PREP  */
		bf_14expiry,		    			/* OPTN PREP   */
		bf_22posentry,          			/* MAND PREP  */
		bf_23csn,
		bf_24functioncode,					/* MAND */
		bf_25poscc,             			/* MAND 06      */
		bf_32idcode,            			/* MAND ECHO */
		bf_33fwdinstidcode,     			/* MAND ECHO */
		bf_35track2,            			/* COND PREP  */
		bf_37rrn,	            			/* MAND PREP  */
		bf_38authcode,          			/* MAND ECHO */
		bf_40service_code,                  /* COND */
		bf_41termid,            			/* MAND PREP  */
		bf_42cardacc,           			/* MAND ECHO */
		bf_43cardacceptloc,     			/* MAND PREP  */
		bf_45track1,            			/* COND PREP  */
		bf_48addldata,          			/* OPTN PREP  */
		bf_49trncurcd,          			/* MAND ECHO */
		bf_52pinblock,
		bf_54AddlAmount,
		bf_55IccData,
		//bf_62addauthdata,                   /* COND*/
		bf_92txorigIntcntrycode,			/*MAND*/
		bf_100revinsidcode,       			/* MAND ECHO */
		bf_104Transpecfdata,				/*COND*/
		bf_106Trandata,						/*COND*/
		bf_123netrefID, 					/* MAND ECHO */
		bf_done                 /* end of fields to move */
	},

	/********************************************
	 * ADVICE RESPONSE
	********************************************/
	{
		AUTH_ADVICE_RESPONSE,
		0x31,0x31,0x33,0x30,                  /* message id, 1130 */
		0x30,0x30,0x30,0x30,0x30,0x30,        /* processing code */
		/**** OPTIONS ****/
		topt1_postadv,
		topt2_referral+topt2_reversal+topt2_floor,
		/**** BIT FIELDS ****/
		bf_2pan,                			/* MAND ECHO */
		bf_3proc,               			/* MAND ECHO */
		bf_4amount,             			/* MAND ECHO */
		bf_6cardbillamt,        			/* COND PREP  */
		bf_7trntime,            			/* MAND ECHO */
		bf_11stan,              			/* MAND ECHO */
		bf_12time,              			/* MAND ECHO */
		bf_13date,              			/* MAND ECHO */
		bf_14expiry,              			/* MAND ECHO  */
		bf_23csn,
		bf_24functioncode,					/* MAND */
		bf_25poscc,             			/* MAND ECHO */
		bf_32idcode,            			/* MAND ECHO */
		bf_33fwdinstidcode,     			/* MAND ECHO */
		bf_37rrn,	            			/* MAND ECHO */
		//bf_38authcode,          			/* MAND ECHO */
		bf_39respcode,          			/* MAND PREP  */
		bf_41termid,            			/* MAND ECHO */
		bf_42cardacc,           			/* MAND ECHO */
		bf_49trncurcd,          			/* MAND ECHO */
		bf_51crdcurcd,          			/* COND PREP  */
		bf_54AddlAmount,          			/* OPTN PREP */
		bf_55IccData,
		bf_92txorigIntcntrycode,			/*MAND*/
		bf_100revinsidcode,       			/* MAND ECHO */
		bf_123netrefID, 					/* MAND ECHO */
		bf_done
	},

	/****************************************
	* REVERSAL ADVICE REQUEST
	*****************************************/
	{
		AUTH_REVERSAL_ADVICE,
		0x31,0x34,0x32,0x30,                  /* message id, 1420 */
		0x30,0x30,0x30,0x30,0x30,0x30,        /* processing code */
		/**** OPTIONS ****/
		topt1_postadv,
		topt2_referral+topt2_reversal+topt2_floor,
		/**** BIT FIELDS ****/
		bf_2pan,                			/* MAND ECHO */
		bf_3proc,               			/* MAND ECHO */
		bf_4amount,             			/* MAND ECHO */
		bf_6cardbillamt,        			/* COND ECHO sent by UnionPay only */
		bf_7trntime,            			/* MAND PREP  */
		bf_11stan,              			/* MAND PREP  */
		bf_12time,              			/* MAND ECHO */
		bf_13date,              			/* MAND ECHO */
		bf_22posentry,          			/* MAND ECHO */
		bf_23csn,
		bf_24functioncode,					/* MAND */
		bf_26pincode,             			/* MAND ECHO */
		bf_32idcode,            			/* MAND ECHO */
		bf_33fwdinstidcode,     			/* MAND ECHO */
		bf_37rrn,	           				/* MAND ECHO */
		bf_38authcode,          			/* COND PREP  */
		bf_41termid,            			/* MAND ECHO */
		bf_42cardacc,           			/* MAND ECHO */
		bf_43cardacceptloc,     			/* MAND ECHO */
		bf_44respdata,          			/* COND ECHO sent by UnionPay only */
		bf_49trncurcd,          			/* MAND ECHO */
		bf_51crdcurcd,          			/* COND ECHO sent by UnionPay only */
		bf_55IccData,
		bf_56oridataelement,                /* MAND */
		//bf_62addauthdata,                   /* COND*/
		bf_92txorigIntcntrycode,			/*MAND*/
		bf_106Trandata,						/*COND*/
		bf_123netrefID,                     /* MAND */
		bf_done			       /* end of fields to move */
	},

	/***********************************************
	*  REVERSAL ADVICE RESPONSE
	***********************************************/
	{
		AUTH_REVERSAL_ADVICE_RESPONSE,
		0x31,0x34,0x33,0x30,                  /* message id, 1430 */
		0x30,0x30,0x30,0x30,0x30,0x30,        /* processing code */
		/**** OPTIONS ****/
		topt1_postadv,
		topt2_referral+topt2_reversal+topt2_floor,
		/**** BIT FIELDS ****/
		bf_2pan,                			/* MAND ECHO */
		bf_3proc,               			/* MAND ECHO */
		bf_4amount,             			/* MAND ECHO */
		bf_7trntime,            			/* MAND ECHO */
		bf_11stan,              			/* MAND ECHO */
		bf_12time,              			/* MAND ECHO */
		bf_13date,              			/* MAND ECHO */
		bf_23csn,
		bf_24functioncode,					/* MAND */
		bf_25poscc,             			/* MAND ECHO */
		bf_32idcode,            			/* MAND ECHO */
		bf_33fwdinstidcode,     			/* MAND ECHO */
		bf_37rrn,	              			/* MAND ECHO */
		bf_39respcode,          			/* MAND PREP  */
		bf_41termid,            			/* MAND ECHO */
		bf_42cardacc,           			/* MAND ECHO */
		bf_49trncurcd,          			/* MAND ECHO */
		//bf_55IccData,
		bf_100revinsidcode,       			/* MAND ECHO */
		bf_123netrefID, 					/* MAND ECHO */
		bf_done				  /* end of fields to move */
	},

	/****************************************
	 * REVERSAL REQUEST
	*****************************************/
	{
		AUTH_REVERSAL,
		0x31,0x34,0x32,0x30,                  /* message id, 1420 */
		0x30,0x30,0x30,0x30,0x30,0x30,        /* processing code */
		/**** OPTIONS ****/
		topt1_postadv,
		topt2_referral+topt2_reversal+topt2_floor,
		/**** BIT FIELDS ****/
		bf_2pan,                			/* MAND ECHO */
		bf_3proc,               			/* MAND ECHO */
		bf_4amount,             			/* MAND ECHO */
		bf_6cardbillamt,        			/* COND ECHO sent by UnionPay only */
		bf_7trntime,            			/* MAND PREP  */
		bf_11stan,              			/* MAND PREP  */
		bf_12time,              			/* MAND ECHO */
		bf_13date,              			/* MAND ECHO */
		bf_22posentry,          			/* MAND ECHO */
		bf_23csn,
		bf_24functioncode,					/* MAND */
		bf_26pincode,             			/* MAND ECHO */
		bf_32idcode,            			/* MAND ECHO */
		bf_33fwdinstidcode,     			/* MAND ECHO */
		bf_37rrn,	           				/* MAND ECHO */
		bf_38authcode,          			/* COND PREP  */
		bf_41termid,            			/* MAND ECHO */
		bf_42cardacc,           			/* MAND ECHO */
		bf_43cardacceptloc,     			/* MAND ECHO */
		bf_44respdata,          			/* COND ECHO sent by UnionPay only */
		bf_49trncurcd,          			/* MAND ECHO */
		bf_51crdcurcd,          			/* COND ECHO sent by UnionPay only */
		bf_55IccData,
		bf_56oridataelement,                /* MAND */
	//	bf_62addauthdata,                   /* COND*/
		bf_92txorigIntcntrycode,			/*MAND*/
		bf_106Trandata,						/*COND*/
		bf_123netrefID,                     /* MAND */
		bf_done			       /* end of fields to move */
	},

	/***********************************************
	 * REVERSAL RESPONSE
	***********************************************/
	{
		AUTH_REVERSAL_RESPONSE,
		0x31,0x34,0x33,0x30,                  /* message id, 1430 */
		0x30,0x30,0x30,0x30,0x30,0x30,        /* processing code */
		/**** OPTIONS ****/
		topt1_postadv,
		topt2_referral+topt2_reversal+topt2_floor,
		/**** BIT FIELDS ****/
		bf_2pan,                			/* MAND ECHO */
		bf_3proc,               			/* MAND ECHO */
		bf_4amount,             			/* MAND ECHO */
		bf_7trntime,            			/* MAND ECHO */
		bf_11stan,              			/* MAND ECHO */
		bf_12time,              			/* MAND ECHO */
		bf_13date,              			/* MAND ECHO */
		bf_23csn,
		bf_24functioncode,					/* MAND */
		bf_25poscc,             			/* MAND ECHO */
		bf_32idcode,            			/* MAND ECHO */
		bf_33fwdinstidcode,     			/* MAND ECHO */
		bf_37rrn,	              			/* MAND ECHO */
		bf_39respcode,          			/* MAND PREP  */
		bf_41termid,            			/* MAND ECHO */
		bf_42cardacc,           			/* MAND ECHO */
		bf_49trncurcd,          			/* MAND ECHO */
		//bf_55IccData,
		bf_100revinsidcode,       			/* MAND ECHO */
		bf_123netrefID, 					/* MAND ECHO */
		bf_done				  /* end of fields to move */
	},

#if BDOR_62_PREAUTH_VOID

	/****************************************
	 * VOID PRE AUTH REQUEST
	*****************************************/
	{
		AUTH_VOID_PRE_AUTHORIZATION,
		0x31,0x34,0x32,0x30,                  /* message id, 1420 */
		0x30,0x30,0x30,0x30,0x30,0x30,        /* processing code */
		/**** OPTIONS ****/
		topt1_postadv,
		topt2_referral+topt2_reversal+topt2_floor,
		/**** BIT FIELDS ****/
		bf_2pan,                			/* MAND ECHO */
		bf_3proc,               			/* MAND ECHO */
		bf_4amount,             			/* MAND ECHO */
		bf_6cardbillamt,        			/* COND ECHO sent by UnionPay only */
		bf_7trntime,            			/* MAND PREP  */
		bf_11stan,              			/* MAND PREP  */
		bf_12time,              			/* MAND ECHO */
		bf_13date,              			/* MAND ECHO */
		bf_22posentry,          			/* MAND ECHO */
		bf_23csn,
		bf_24functioncode,					/* MAND */
		bf_26pincode,             			/* MAND ECHO */
		bf_30originalamt,					/* COND PREP */
		bf_32idcode,            			/* MAND ECHO */
		bf_33fwdinstidcode,     			/* MAND ECHO */
		bf_37rrn,	           				/* MAND ECHO */
		bf_38authcode,          			/* COND PREP  */
		bf_41termid,            			/* MAND ECHO */
		bf_42cardacc,           			/* MAND ECHO */
		bf_43cardacceptloc,     			/* MAND ECHO */
		bf_44respdata,          			/* COND ECHO sent by UnionPay only */
		bf_49trncurcd,          			/* MAND ECHO */
		bf_51crdcurcd,          			/* COND ECHO sent by UnionPay only */
		bf_55IccData,
		bf_56oridataelement,                /* MAND */
	//	bf_62addauthdata,                   /* COND*/
		bf_92txorigIntcntrycode,			/*MAND*/
		bf_106Trandata,						/*COND*/
		bf_123netrefID,                     /* MAND */
		bf_done			       /* end of fields to move */
	},

	/***********************************************
	 *  VOID PRE AUTH RESPONSE
	***********************************************/
	{
		AUTH_VOID_PRE_AUTHORIZATION_RESPONSE,
		0x31,0x34,0x33,0x30,                  /* message id, 1430 */
		0x30,0x30,0x30,0x30,0x30,0x30,        /* processing code */
		/**** OPTIONS ****/
		topt1_postadv,
		topt2_referral+topt2_reversal+topt2_floor,
		/**** BIT FIELDS ****/
		bf_2pan,                			/* MAND ECHO */
		bf_3proc,               			/* MAND ECHO */
		bf_4amount,             			/* MAND ECHO */
		bf_7trntime,            			/* MAND ECHO */
		bf_11stan,              			/* MAND ECHO */
		bf_12time,              			/* MAND ECHO */
		bf_13date,              			/* MAND ECHO */
		bf_23csn,
		bf_24functioncode,					/* MAND */
		bf_25poscc,             			/* MAND ECHO */
		bf_32idcode,            			/* MAND ECHO */
		bf_33fwdinstidcode,     			/* MAND ECHO */
		bf_37rrn,	              			/* MAND ECHO */
		bf_39respcode,          			/* MAND PREP  */
		bf_41termid,            			/* MAND ECHO */
		bf_42cardacc,           			/* MAND ECHO */
		bf_49trncurcd,          			/* MAND ECHO */
		//bf_55IccData,
		bf_100revinsidcode,       			/* MAND ECHO */
		bf_123netrefID, 					/* MAND ECHO */
		bf_done				  /* end of fields to move */
	},

#endif

	/****************************************
	 * REVERSAL  REPEAT REQUEST
	*****************************************/
	{
		AUTH_REVERSAL_REPEAT,
		0x31,0x34,0x32,0x30,                  /* message id, 1420 */
		0x30,0x30,0x30,0x30,0x30,0x30,        /* processing code */
		/**** OPTIONS ****/
		topt1_postadv,
		topt2_referral+topt2_reversal+topt2_floor,
		/**** BIT FIELDS ****/
		bf_2pan,                			/* MAND ECHO */
		bf_3proc,               			/* MAND ECHO */
		bf_4amount,             			/* MAND ECHO */
		bf_6cardbillamt,        			/* COND ECHO sent by UnionPay only */
		bf_7trntime,            			/* MAND PREP  */
		bf_11stan,              			/* MAND PREP  */
		bf_12time,              			/* MAND ECHO */
		bf_13date,              			/* MAND ECHO */
		bf_22posentry,          			/* MAND ECHO */
		bf_23csn,
		bf_24functioncode,					/* MAND */
		bf_26pincode,             			/* MAND ECHO */
		bf_32idcode,            			/* MAND ECHO */
		bf_33fwdinstidcode,     			/* MAND ECHO */
		bf_37rrn,	           				/* MAND ECHO */
		bf_38authcode,          			/* COND PREP  */
		bf_41termid,            			/* MAND ECHO */
		bf_42cardacc,           			/* MAND ECHO */
		bf_43cardacceptloc,     			/* MAND ECHO */
		bf_44respdata,          			/* COND ECHO sent by UnionPay only */
		bf_49trncurcd,          			/* MAND ECHO */
		bf_51crdcurcd,          			/* COND ECHO sent by UnionPay only */
		bf_55IccData,
		bf_56oridataelement,                /* MAND */
		//bf_62addauthdata,                   /* COND*/
		bf_92txorigIntcntrycode,			/*MAND*/
		bf_106Trandata,						/*COND*/
		bf_123netrefID,                     /* MAND */
		bf_done			       /* end of fields to move */
	},

	/***********************************************
	 * REVERSAL REPEAT RESPONSE
	***********************************************/
	{
		AUTH_REVERSAL_REPEAT_RESPONSE,
		0x31,0x34,0x33,0x30,                  /* message id, 1430 */
		0x30,0x30,0x30,0x30,0x30,0x30,        /* processing code */
		/**** OPTIONS ****/
		topt1_postadv,
		topt2_referral+topt2_reversal+topt2_floor,
		/**** BIT FIELDS ****/
		bf_2pan,                			/* MAND ECHO */
		bf_3proc,               			/* MAND ECHO */
		bf_4amount,             			/* MAND ECHO */
		bf_7trntime,            			/* MAND ECHO */
		bf_11stan,              			/* MAND ECHO */
		bf_12time,              			/* MAND ECHO */
		bf_13date,              			/* MAND ECHO */
		bf_23csn,
		bf_24functioncode,					/* MAND */
		bf_25poscc,             			/* MAND ECHO */
		bf_32idcode,            			/* MAND ECHO */
		bf_33fwdinstidcode,     			/* MAND ECHO */
		bf_37rrn,	              			/* MAND ECHO */
		bf_39respcode,          			/* MAND PREP  */
		bf_41termid,            			/* MAND ECHO */
		bf_42cardacc,           			/* MAND ECHO */
		bf_49trncurcd,          			/* MAND ECHO */
		//bf_55IccData,
		bf_100revinsidcode,       			/* MAND ECHO */
		bf_123netrefID, 					/* MAND ECHO */
		bf_done				  /* end of fields to move */
	},

	/********************************
	   ONLINE FILE UPDATE REQUEST
	********************************/
	{
		 AUTH_CARD_UPDATE,
		 0xF1,0xF3,0xF0,0xF4,                  /* message id, 1304 */
		 0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,        /* processing code */
		 /**** OPTIONS ****/
		 topt1_postadv,
		 topt2_referral+topt2_reversal+topt2_floor,
		 /**** BIT FIELDS ****/
		 bf_2pan,             				/* MAND */
		 bf_3proc,
		 bf_4amount,
		 bf_7trntime,
		 bf_11stan,           				/* MAND */
		 bf_12time,
		 bf_13date,           				/* MAND */
		 bf_14expiry,         				/* MAND */
		 bf_22posentry,
		 bf_24functioncode,					/* MAND */
		 bf_26pincode,
		 bf_32idcode,
		 bf_33fwdinstidcode,
		 bf_38authcode,
		 bf_39respcode,
		 bf_41termid,
		 bf_42cardacc,
		 bf_43cardacceptloc,
		 bf_44respdata,
		 bf_49trncurcd,
		 bf_56oridataelement,
		 bf_64mac,
		 bf_72datarecord,
		 bf_93txdestIIC,
		 bf_94txorigIIC,
		 bf_100revinsidcode,
		 bf_101filename,
		 bf_done			/* end of fields to move */
	  },

	  /********************************
		 ONLINE FILE UPDATE RESPONSE
	  ********************************/
	  {
		 AUTH_CARD_UPDATE_RESPONSE,
		 0xF1,0xF3,0xF1,0xF4,                  /* message id, 1314 */
		 0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,        /* processing code */
		 /**** OPTIONS ****/
		 topt1_postadv,
		 topt2_referral+topt2_reversal+topt2_floor,
		 /**** BIT FIELDS ****/
		 bf_2pan,             				/* MAND */
		 bf_3proc,
		 bf_4amount,
		 bf_7trntime,
		 bf_11stan,           				/* MAND */
		 bf_12time,
		 bf_13date,           				/* MAND */
		 bf_14expiry,         				/* MAND */
		 bf_22posentry,
		 bf_24functioncode,					/* MAND */
		 bf_26pincode,
		 bf_32idcode,
		 bf_33fwdinstidcode,
		 bf_38authcode,
		 bf_39respcode,
		 bf_41termid,
		 bf_42cardacc,
		 bf_43cardacceptloc,
		 bf_44respdata,
		 bf_49trncurcd,
		 bf_56oridataelement,
		 bf_64mac,
		 bf_72datarecord,
		 bf_93txdestIIC,
		 bf_94txorigIIC,
		 bf_100revinsidcode,
		 bf_101filename,
		 bf_done			/* end of fields to move */
	 },

	 /********************************************
	  POS AUTH QUASI CASH REQUEST
	 ********************************************/
	 {
		AUTH_QUASI_CASH,
		0xF1,0xF1,0xF0,0xF0, 				 /* message id, 1100 */
		0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,		 /* processing code */
		/**** OPTIONS ****/
		topt1_postadv,
		topt2_referral+topt2_reversal+topt2_floor,
		/**** BIT FIELDS ****/
		bf_2pan, 						 	/* MAND */
		bf_3proc,			 				/* MAND */
		bf_4amount,			 				/* MAND */
		bf_7trntime, 		 				/* OPT */
		bf_11stan,			 				/* MAND */
		bf_12time,			 				/* MAND */
		bf_13date,			 				/* OPT */
		bf_14expiry, 		 				/* OPT */
		bf_22posentry,		 				/* MAND */
		bf_23csn,
		bf_24functioncode,					/* MAND */
		bf_26pincode,		 				/* MAND */
		bf_32idcode, 		 				/* MAND */
		bf_35track2, 		 				/* COND */
		bf_37rrn,			 				/* OPT */
		bf_41termid, 		 				/* OPT */
		bf_42cardacc,		 				/* MAND */
		bf_43cardacceptloc,	 				/* OPT */
		bf_45track1, 		 				/* COND */
		bf_49trncurcd,		 				/* MAND */
		bf_55IccData,
		bf_92txorigIntcntrycode,			/*MAND*/
		bf_106Trandata,						/*COND*/
		bf_done			 /* end of fields to move */
	},

	 /***********************************************
	 	 QUASI CASH  RESPONSE
	  ***********************************************/
	{
		AUTH_QUASI_CASH_RESPONSE,
		0xF1,0xF1,0xF1,0xF0, 				 /* message id, 1110 */
		0xF0,0xF0,0xF4,0xF0,0xF0,0xF0,		 /* processing code */
		/**** OPTIONS ****/
		topt1_postadv,
		topt2_referral+topt2_reversal+topt2_floor,
		/**** BIT FIELDS ****/
		bf_2pan, 							/* MAND */
		bf_3proc,							/* MAND */
		bf_4amount,							/* MAND */
		bf_7trntime, 		 				/* OPT */
		bf_11stan,			 				/* MAND */
		bf_12time,			 				/* MAND */
		bf_13date,			 				/* OPT */
		bf_14expiry, 		 				/* OPT */
		bf_22posentry,		 				/* MAND */
		bf_23csn,
		bf_24functioncode,					/* MAND */
		bf_26pincode,		 				/* MAND */
		bf_32idcode, 		 				/* MAND */
		bf_35track2, 		 				/* COND */
		bf_37rrn,			 				/* OPT */
		bf_41termid, 		 				/* OPT */
		bf_42cardacc,		 				/* MAND */
		bf_43cardacceptloc,	 				/* OPT */
		bf_45track1, 		 				/* COND */
		bf_49trncurcd,		 				/* MAND */
		bf_55IccData,
//		bf_62addauthdata,					/* COND */
		bf_92txorigIntcntrycode,			/*MAND*/
        bf_100revinsidcode,       			/* MAND ECHO */
		bf_123netrefID, 					/* MAND ECHO */
		bf_done				 /* end of fields to move */
	},


	/********************************************
	 * 	LOGON REQUEST
	********************************************/
	{
		AUTH_LOGON,
		0x31,0x38,0x30,0x34,                  /* message id, 1804 */
		0x30,0x30,0x30,0x30,0x30,0x30,        /* processing code */
		/**** OPTIONS ****/
		topt1_postadv,
		topt2_referral+topt2_reversal+topt2_floor,
		/**** BIT FIELDS ****/
		bf_7trntime,							/* MAND */
		bf_11stan,			 					/* MAND */
		bf_12time, 								/* MAND */
		bf_24functioncode, 						/* MAND */
		bf_93txdestIIC,							/* MAND */
		bf_94txorigIIC,    						/* MAND */
		//bf_100revinsidcode,						/* COND */
		bf_done			/* end of fields to move */
	},

	/***********************************************
	 * 	LOGON  RESPONSE
	***********************************************/
	{
		AUTH_LOGON_RESPONSE,
		0x31,0x38,0x31,0x34,                  /* message id, 1814 */
		0x30,0x30,0x30,0x30,0x30,0x30,        /* processing code */
		/**** OPTIONS ****/
		topt1_postadv,
		topt2_referral+topt2_reversal+topt2_floor,
		/**** BIT FIELDS ****/
		bf_7trntime,		     				/* MAND ECHO  */
		bf_11stan,		 	 					/* MAND ECHO  */
		bf_12time, 								/* MAND ECHO  */
		bf_24functioncode, 						/* MAND ECHO*/
		bf_39respcode,			 				/* MAND   */
		bf_93txdestIIC,							/* MAND ECHO */
		bf_94txorigIIC,    						/* MAND ECHO */
		//bf_100revinsidcode,					/* COND ECHO */
		bf_done				 /* end of fields to move */
	},

	/********************************************
	 * 	KEY RESET REQUEST
	********************************************/
	{
		AUTH_PIN_CHANGE,
		0x31,0x38,0x30,0x34,                  /* message id, 1804 */
		0x30,0x30,0x30,0x30,0x30,0x30,        /* processing code */
		/**** OPTIONS ****/
		topt1_postadv,
		topt2_referral+topt2_reversal+topt2_floor,
		/**** BIT FIELDS ****/
		bf_7trntime,							/* MAND */
		bf_11stan,			 					/* MAND */
		bf_12time, 								/* MAND */
		bf_24functioncode, 						/* MAND */
		bf_93txdestIIC,							/* MAND */
		bf_94txorigIIC,    						/* MAND */
		bf_96keymangdata,						/* MAND */
		bf_done			/* end of fields to move */
	},

	/********************************************
	 * 	 KEY RESET REPONSE
	********************************************/
	{
		AUTH_PIN_CHANGE_RESPONSE,
		0x31,0x38,0x31,0x34,                  /* message id, 1814 */
		0x30,0x30,0x30,0x30,0x30,0x30,        /* processing code */
		/**** OPTIONS ****/
		topt1_postadv,
		topt2_referral+topt2_reversal+topt2_floor,
		/**** BIT FIELDS ****/
		bf_7trntime,		 	    			/* MAND ECHO  */
		bf_11stan,		 						/* MAND ECHO  */
		bf_12time, 								/* MAND ECHO */
		bf_24functioncode, 						/* MAND ECHO */
		bf_39respcode,			 				/* MAND   	 */
		bf_93txdestIIC,							/* MAND ECHO */
		bf_94txorigIIC,    						/* MAND ECHO */
		bf_done			/* end of fields to move */
	}
};

#define N_TRANENTRY (sizeof(TRANTABLE) / sizeof(struct trandef))

#define N_BFData ( sizeof(BFData) / sizeof (struct bitfield_data) )

struct bitfield_data *ncdci_Find_Bitfield_Table (enum bitfields);
struct trandef       *ncdci_Find_Tran_Table     (pAUTH_TX p_auth_tx);

struct iso_msg_rec
{
   BYTE msgtype       				[4];
   BYTE bitmap1_bin   				[16];
   BYTE bitmap2_bin  				[16];
   BYTE pan_len       				[2];
   BYTE pan           				[19];
   BYTE proccode      				[6];
   BYTE amount        				[12];
   BYTE ch_bill_amt   				[12];
   BYTE gmt_time      				[10];
   BYTE stan          				[6];
   BYTE time          				[12];
   BYTE date		  				[4];
   BYTE date_expd	  				[4];
   BYTE posentry      				[12];
   BYTE card_seq_num  				[3];
   BYTE net_manag_info_code 		[3];
   BYTE pos_condcd    				[4];
   BYTE accpt_bus_cd   				[4];
   BYTE Original_amount  			[24];
   BYTE acq_inst_len  				[2];
   BYTE acq_inst_id   				[11];
   BYTE fwd_inst_len  				[2];
   BYTE fwd_inst_id   				[11];
   BYTE track2_len    				[2];
   BYTE track2        				[37];
   BYTE rrn			  				[12];
   BYTE authcode      				[6];
   BYTE responsecd    				[3];
   BYTE service_code   				[3];
   BYTE termid        				[8];
   BYTE cardaccid     				[15];
   BYTE cardaccname_len 			[2];
   BYTE cardaccname 				[100];
   BYTE addl_rsp_len  				[2];
   BYTE addlrspdata   				[22];
   BYTE track1_len    				[2];
   BYTE track1        				[77];
   BYTE surcharge_fee_len 			[3];
   BYTE surcharge_fee 				[11];
   BYTE addl_data_len				[3];
   BYTE addl_data     				[999];
   BYTE tran_curr_cd  				[3];
   BYTE card_curr_cd  				[3];
   BYTE pinblock      				[8];
   BYTE AddlAmts_len   				[3];
   BYTE AddlAmts     				[120];
   BYTE IccData_len   				[3];
   BYTE IccData	    				[264];
   BYTE orig_data_len 				[2];
   BYTE orig_data 					[33];
   BYTE Auth_agent_IIC_len			[2];
   BYTE Auth_agent_IIC				[11];
   BYTE NetworkID_Transport_data_len[3];
   BYTE NetworkID_Transport_data	[999];
   BYTE addition_auth_data_len		[3];
   BYTE addition_auth_data			[999];
   BYTE mess_auth_code		 		[8];
   BYTE data_record_len				[3];
   BYTE data_record					[999];
   BYTE ctry_code	  				[3];
   BYTE TranDestination_ICC_len     [2];
   BYTE TranDestination_ICC     	[11];
   BYTE TranOriginator_ICC_len      [2];
   BYTE TranOriginator_ICC     		[11];
   BYTE Key_Managment_data_len 		[3];
   BYTE Key_Managment_data     		[36];
   BYTE recv_ins_id_code_len		[2];
   BYTE recv_ins_id_code			[11];
   BYTE file_name_len				[2];
   BYTE file_name					[17];
   BYTE tran_specf_data_len		    [3];
   BYTE tran_specf_data				[999];
   BYTE tran_data_len			    [3];
   BYTE tran_data					[999];
   BYTE Add_authentication_data_len	[3];
   BYTE Add_authentication_data		[999];
   BYTE networ_ref_id_len			[3];
   BYTE networ_ref_id				[15];
   BYTE transaction_qualifer_len	[3];
   BYTE transaction_qualifer		[999];
   BYTE replacement_pin_len			[3];
   BYTE replacement_pin				[8];
};
struct iso_msg_rec  ncdci_HOST_MESSAGE;

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



static  struct bitfield_data BFData[] = {
	{
		bf_2pan,                      		/* Primary Account Number  */
		ncdci_HOST_MESSAGE.pan_len,         	/* data to move */
		2,                           		/* # of bytes in pan len field */
		ncdci_Move_IfThere,                  /* mandatory */
		ncdci_vDataMove,                     /* move routine */
		ncdci_RspMovevData                   /* response routine */
	},
	{
		bf_3proc,                     		/* field */
		ncdci_HOST_MESSAGE.proccode,        	/* data to move */
		6,                            		/* # of bytes in  iso field */
		ncdci_Move_Always,                 	 /* mandatory */
		ncdci_GenericMove,                   /* move routine */
		ncdci_RspMoveGeneric                 /* response routine */
	},
	{
		bf_4amount,                   		/* field */
		ncdci_HOST_MESSAGE.amount,           /* data to move */
		12,                           		/* # of bytes in  iso field */
		ncdci_Move_Always,                   /* mandatory */
		ncdci_GenericMove,                   /* move routine */
		ncdci_RspMoveGeneric                 /* response routine */
	},
	{
		bf_6cardbillamt,                		/* field */
		ncdci_HOST_MESSAGE.ch_bill_amt,      /* data to move */
		12,                           		/* # of bytes in  iso field */
		ncdci_Move_IfThere,                  /* mandatory */
		ncdci_GenericMove,                   /* move routine */
		ncdci_RspMoveGeneric                 /* response routine */
	},
	{
		bf_7trntime,                  /* field */
		ncdci_HOST_MESSAGE.gmt_time,        /* data to move : MMDDhhmmss*/
		10,                           /* # of bytes in  iso field */
		ncdci_Move_IfThere,                  /* conditional */
		ncdci_GenericMove,                   /* move routine */
		ncdci_RspMoveGeneric                 /* response routine */
	},
	{
		bf_11stan,                    /* field */
		ncdci_HOST_MESSAGE.stan,            /* data to move */
		6,                            /* # of bytes in  iso field */
		ncdci_Move_Always,                  /* conditional */
		ncdci_GenericMove,                   /* move routine */
		ncdci_RspMoveGeneric                 /* response routine */
	},
	{
		bf_12time,                    /* field */
		ncdci_HOST_MESSAGE.time,            /* data to move */
		12,                            /* # of bytes in  iso field */
		ncdci_Move_IfThere,                  /* conditional */
		ncdci_GenericMove,                   /* move routine */
		ncdci_RspMoveGeneric                 /* response routine */
	},
	{
		bf_13date,                    /* field */
		ncdci_HOST_MESSAGE.date,            /* effective data to move */
		4,                            /* # of bytes in  iso field */
		ncdci_Move_IfThere,                  /* conditional */
		ncdci_GenericMove,                   /* move routine */
		ncdci_RspMoveGeneric                 /* response routine */
	},
	{
		bf_14expiry,                  /* field */
		ncdci_HOST_MESSAGE.date_expd,       /* data to move */
		4,                            /* # of bytes in field */
		ncdci_Move_IfThere,                     /* conditional */
		ncdci_GenericMove,                   /* move routine */
		ncdci_RspMoveGeneric                 /* response routine */
	},
	{
		bf_22posentry,                /* field */
		ncdci_HOST_MESSAGE.posentry,        /* data to move */
		12,                            /* # of bytes in  iso field */
		ncdci_Move_IfThere,                  /* conditional */
		ncdci_GenericMove,                   /* move routine */
		ncdci_RspMoveGeneric                 /* response routine */
	},
	{
		bf_23csn,                  /* field */
		ncdci_HOST_MESSAGE.card_seq_num,         /* data to move */
		3,                            /* # of bytes in field */
		ncdci_Move_IfThere,                 /* conditional */
		ncdci_GenericMove,                   /* move routine */
		ncdci_RspMoveGeneric                 /* response routine */
	},
	{
		bf_24functioncode,                  /* field */
		ncdci_HOST_MESSAGE.net_manag_info_code,         /* data to move */
		3,                            /* # of bytes in field */
		ncdci_Move_IfThere,                 /* conditional */
		ncdci_GenericMove,                   /* move routine */
		ncdci_RspMoveGeneric                 /* response routine */
	},
	{
		bf_25poscc,                   /* field */
		ncdci_HOST_MESSAGE.pos_condcd,      /* data to move */
		4,                            /* # of bytes in  iso field */
		ncdci_Move_IfThere,                  /* conditional */
		ncdci_GenericMove,                   /* move routine */
		ncdci_RspMoveGeneric                 /* response routine */
	},
	{
		bf_26pincode,                 /* field */
		ncdci_HOST_MESSAGE.accpt_bus_cd,    /* data to move */
		4,                            /* # of bytes in field */
		ncdci_Move_IfThere,                  /* conditional */
		ncdci_GenericMove,                   /* move routine */
		ncdci_RspMoveGeneric                 /* response routine */
	},
	{
		bf_30originalamt,                 /* field */
		ncdci_HOST_MESSAGE.Original_amount,    /* data to move */
		24,                            /* # of bytes in field */
		ncdci_Move_IfThere,                 /* conditional */
		ncdci_GenericMove,                   /* move routine */
		ncdci_RspMoveGeneric                 /* response routine */
	},
	{
		bf_32idcode,                  /* field */
		ncdci_HOST_MESSAGE.acq_inst_len,    /* data to move */
		2,                            /* # of bytes in  iso field */
		ncdci_Move_IfThere,                  /* conditional */
		ncdci_vDataMove,                  /* move routine */
		ncdci_RspMovevData                /* response routine */
	},
	{
		bf_33fwdinstidcode,                  /* field */
		ncdci_HOST_MESSAGE.fwd_inst_len,    /* data to move */
		2,                            /* # of bytes in  iso field */
		ncdci_Move_IfThere,                  /* conditional */
		ncdci_vDataMove,                  /* move routine */
		ncdci_RspMovevData                /* response routine */
	},
	{
		bf_35track2,                  /* field */
		ncdci_HOST_MESSAGE.track2_len,      /* data to move */
		2,                            /* # of bytes in track2 len field */
		ncdci_Move_IfThere,                  /* conditional */
		ncdci_vDataMove,                  /* move routine */
		ncdci_RspMovevData                /* response routine */
	},
	{
		bf_37rrn,                  /* field */
		ncdci_HOST_MESSAGE.rrn,      /* data to move */
		12,                            /* # of bytes in track2 len field */
		ncdci_Move_IfThere,                  /* conditional */
		ncdci_GenericMove,                  /* move routine */
		ncdci_RspMoveGeneric                /* response routine */
	},
	{
		bf_38authcode,                /* field */
		ncdci_HOST_MESSAGE.authcode,        /* data to move */
		6,                            /* # of bytes in  iso field */
		ncdci_Move_IfThere,                 /* conditional */
		ncdci_GenericMove,                   /* move routine */
		ncdci_RspMoveGeneric                 /* response routine */
	},
	{
		bf_39respcode,                /* field */
		ncdci_HOST_MESSAGE.responsecd,      /* data to move */
		3,                            /* # of bytes in  iso field */
		ncdci_Move_IfThere,                 /* conditional */
		ncdci_GenericMove,                   /* move routine */
		ncdci_RspMoveGeneric                 /* response routine */
	},
	{
		bf_40service_code,                /* field */
		ncdci_HOST_MESSAGE.service_code,      /* data to move */
		3,                            /* # of bytes in  iso field */
		ncdci_Move_IfThere,                 /* conditional */
		ncdci_GenericMove,                   /* move routine */
		ncdci_RspMoveGeneric                 /* response routine */
	},
	{
		bf_41termid,                  /* field */
		ncdci_HOST_MESSAGE.termid,          /* data to move */
		8,                            /* # of bytes in  iso field */
		ncdci_Move_IfThere,                 /* conditional */
		ncdci_GenericMove,                   /* move routine */
		ncdci_RspMoveGeneric                 /* response routine */
	},
	{
		bf_42cardacc,                 /* field */
		ncdci_HOST_MESSAGE.cardaccid,       /* data to move */
		15,                           /* # of bytes in  iso field */
		ncdci_Move_IfThere,                  /* conditional */
		ncdci_GenericMove,                   /* move routine */
		ncdci_RspMoveGeneric                 /* response routine */
	},
	{
		bf_43cardacceptloc,                 /* field */
		ncdci_HOST_MESSAGE.cardaccname_len,       /* data to move */
		2,                           /* # of bytes in  iso field */
		ncdci_Move_IfThere,                  /* conditional */
		ncdci_vDataMove,                   /* move routine */
		ncdci_RspMovevData					/* response routine */
	},
	{
		bf_44respdata,                 /* field */
		ncdci_HOST_MESSAGE.addl_rsp_len,       /* data to move */
		2,                           /* # of bytes in  iso field */
		ncdci_Move_IfThere,                  /* conditional */
		ncdci_vDataMove,                   /* move routine */
		ncdci_RspMovevData					/* response routine */
	},
	{
		bf_45track1,                  /* field */
		ncdci_HOST_MESSAGE.track1_len,      /* data to move */
		2,                            /* # of bytes in len field */
		ncdci_Move_IfThere,                   /* conditional */
		ncdci_vDataMove,                  /* move routine */
		ncdci_RspMovevData                /* response routine */
	},
	{
		bf_46surchargfee,                  /* field */
		ncdci_HOST_MESSAGE.surcharge_fee_len,      /* data to move */
		3,                            /* # of bytes in len field */
		ncdci_Move_IfThere,                   /* conditional */
		ncdci_vDataMove,                  /* move routine */
		ncdci_RspMovevData                /* response routine */
	},
	{
		bf_48addldata,                /* field */
		ncdci_HOST_MESSAGE.addl_data_len,   /* data to move */
		3,                            /* # of bytes in track1 len field */
		ncdci_Move_IfThere,                 /* conditional */
		ncdci_vDataMove,                  /* move routine */
		ncdci_RspMovevData                /* response routine */
	},
	{
		bf_49trncurcd,               /* field */
		ncdci_HOST_MESSAGE.tran_curr_cd,   /* data to move */
		3,                           /* # of bytes in  iso field */
		ncdci_Move_IfThere,                /* mandatory    */
		ncdci_GenericMove,                 /* move routine */
		ncdci_RspMoveGeneric				/* response routine */
	},
	{
		bf_51crdcurcd,               /* field */
		ncdci_HOST_MESSAGE.card_curr_cd,    /* data to move */
		3,                            /* # of bytes in field */
		ncdci_Move_IfThere,                 /* mandatory    */
		ncdci_GenericMove,                   /* move routine */
		ncdci_RspMoveGeneric                 /* response routine */
	},
	{
		bf_52pinblock,                /* field */
		ncdci_HOST_MESSAGE.pinblock,        /* data to move */
		8,                            /* # of bytes in  iso field */
		ncdci_Move_IfThere,                  /* mandatory */
		ncdci_GenericMove,                  /* move routine */
		ncdci_RspMoveGeneric                /* response routine */
	},
	{
		bf_54AddlAmount,                 /* field */
		ncdci_HOST_MESSAGE.AddlAmts_len,   /* data to move */
		3,                            /* # of bytes in field */
		ncdci_Move_IfThere,                  /* mandatory    */
		ncdci_vDataMove,                  /* move routine */
		ncdci_RspMovevData                /* response routine */
	},
	{
		bf_55IccData,                 /* field */
		ncdci_HOST_MESSAGE.IccData_len,   /* data to move */
		3,                            /* # of bytes in field */
		ncdci_Move_IfThere,                  /* mandatory    */
		ncdci_vDataMove,                  /* move routine */
		ncdci_RspMovevData                /* response routine */
	},
	{
		bf_56oridataelement,                 /* field */
		ncdci_HOST_MESSAGE.orig_data_len,   /* data to move */
		2,                            /* # of bytes in field */
		ncdci_Move_IfThere,                  /* mandatory    */
		ncdci_vDataMove,                  /* move routine */
		ncdci_RspMovevData                /* response routine */
	},
	{
		bf_58authagentIIC,                 /* field */
		ncdci_HOST_MESSAGE.Auth_agent_IIC_len,   /* data to move */
		2,                            /* # of bytes in field */
		ncdci_Move_IfThere,                 		/* mandatory    */
		ncdci_vDataMove,               		/* move routine */
		ncdci_RspMovevData                /* response routine */
	},
	{
		bf_59NettrandataID,                 /* field */
		ncdci_HOST_MESSAGE.NetworkID_Transport_data_len,   /* data to move */
		3,                            /* # of bytes in field */
		ncdci_Move_IfThere,                  /* mandatory    */
		ncdci_vDataMove,                  /* move routine */
		ncdci_RspMovevData                /* response routine */
	},
	{
		bf_62addauthdata,                 /* field */
		ncdci_HOST_MESSAGE.addition_auth_data_len,   /* data to move */
		3,                            /* # of bytes in field */
		ncdci_Move_IfThere,                  /* mandatory    */
		ncdci_vDataMove,                  /* move routine */
		ncdci_RspMovevData                /* response routine */
	},
	{
		bf_64mac,                 /* field */
		ncdci_HOST_MESSAGE.mess_auth_code,   /* data to move */
		8,                            /* # of bytes in field */
		ncdci_Move_IfThere,                  /* mandatory    */
		ncdci_GenericMove,                   /* move routine */
		ncdci_RspMovevData                 /* response routine */
	},
	{
		bf_72datarecord,                 /* field */
		ncdci_HOST_MESSAGE.data_record_len,   /* data to move */
		3,                            /* # of bytes in field */
		ncdci_Move_IfThere,                  /* mandatory    */
		ncdci_vDataMove,                  /* move routine */
		ncdci_RspMovevData                /* response routine */
	},
	{
		bf_92txorigIntcntrycode,                /* field */
		ncdci_HOST_MESSAGE.ctry_code,    /* data to move */
		3,                            /* # of bytes in field */
		ncdci_Move_IfThere,                  /* mandatory    */
		ncdci_GenericMove,                   /* move routine */
		ncdci_RspMoveGeneric                 /* response routine */              		/* response routine */
	},
	{
		bf_93txdestIIC,                /* field */
		ncdci_HOST_MESSAGE.TranDestination_ICC_len,    /* data to move */
		2,                            		/* # of bytes in  iso field */
		ncdci_Move_IfThere,                 		/* mandatory    */
		ncdci_vDataMove,               		/* move routine */
		ncdci_RspMovevData                /* response routine */
	},
	{
		bf_94txorigIIC,                /* field */
		ncdci_HOST_MESSAGE.TranOriginator_ICC_len,    /* data to move */
		2,                            		/* # of bytes in  iso field */
		ncdci_Move_IfThere,                 		/* mandatory    */
		ncdci_vDataMove,               		/* move routine */
		ncdci_RspMovevData                /* response routine */
	},
	{
		bf_96keymangdata,                /* field */
		ncdci_HOST_MESSAGE.Key_Managment_data_len,    /* data to move */
		3,                            		/* # of bytes in  iso field */
		ncdci_Move_IfThere,                 		/* mandatory    */
		ncdci_vDataMove,               		/* move routine */
		ncdci_RspMovevData                /* response routine */
	},
	{
		bf_100revinsidcode,                /* field */
		ncdci_HOST_MESSAGE.recv_ins_id_code_len,    /* data to move */
		2,                            		/* # of bytes in  iso field */
		ncdci_Move_IfThere,                 		/* mandatory    */
		ncdci_vDataMove,               		/* move routine */
		ncdci_RspMovevData                /* response routine */
	},
	{
		bf_101filename,                /* field */
		ncdci_HOST_MESSAGE.file_name_len,    /* data to move */
		2,                            		/* # of bytes in  iso field */
		ncdci_Move_IfThere,                 		/* mandatory    */
		ncdci_vDataMove,               		/* move routine */
		ncdci_RspMovevData                /* response routine */
	},
	{
		bf_104Transpecfdata,                 /* field */
		ncdci_HOST_MESSAGE.tran_specf_data_len,   /* data to move */
		3,                            /* # of bytes in field */
		ncdci_Move_IfThere,                  /* mandatory    */
		ncdci_vDataMove,                  /* move routine */
		ncdci_RspMovevData                /* response routine */
	},
	{
		bf_106Trandata,                 /* field */
		ncdci_HOST_MESSAGE.tran_data_len,   /* data to move */
		3,                            /* # of bytes in field */
		ncdci_Move_IfThere,                  /* mandatory    */
		ncdci_vDataMove,                  /* move routine */
		ncdci_RspMovevData                /* response routine */
	},
	{
		bf_122addauthendata,                /* field */
		ncdci_HOST_MESSAGE.Add_authentication_data_len,    /* data to move */
		3,                            		/* # of bytes in  iso field */
		ncdci_Move_IfThere,                 		/* mandatory    */
		ncdci_vDataMove,               		/* move routine */
		ncdci_RspMovevData                /* response routine */
	},
	{
		bf_123netrefID,                /* field */
		ncdci_HOST_MESSAGE.networ_ref_id_len,    /* data to move */
		3,                            		/* # of bytes in  iso field */
		ncdci_Move_IfThere,                 		/* mandatory    */
		ncdci_vDataMove,               		/* move routine */
		ncdci_RspMovevData                /* response routine */
	},
	{
		bf_124txqualifier,                /* field */
		ncdci_HOST_MESSAGE.transaction_qualifer_len,    /* data to move */
		3,                            		/* # of bytes in  iso field */
		ncdci_Move_IfThere,                 		/* mandatory    */
		ncdci_vDataMove,               		/* move routine */
		ncdci_RspMovevData                /* response routine */
	},
	{
		bf_125replacementpin,                /* field */
		ncdci_HOST_MESSAGE.replacement_pin_len,    /* data to move */
		3,                            		/* # of bytes in  iso field */
		ncdci_Move_IfThere,                 		/* mandatory    */
		ncdci_vDataMove,               		/* move routine */
		ncdci_RspMovevData                /* response routine */
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
