/***************************************************************************
 *
 * Copyright (c) 2004, Hypercom, Inc.
 * All Rights Reserved.
 *
 * MODULE:      ncpul_bitfields.h
 *  
 * TITLE:       ncpul Network Control Bit Field Definitions
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

#include "pulse.h"
#include "ncpul_prototypes.h"


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
   bf_5settlmntamt		   = 5,		/*  Settlement_amount*/
   bf_6cardbillamt         = 6,      /* Card holder Billing Amount */
   bf_7trntime             = 7,      /* Transmission date-time */
   bf_9convratesettlement  = 9,      /* Conversion Rate, settlement */
   bf_10convratecardhldr   = 10,      /* Conversion Rate, settlement */
   bf_11stan               = 11,     /* System trace number */
   bf_12time               = 12,     /* Local Transaction time */
   bf_13date               = 13,     /* Local Transaction date */
   bf_14expiry		       = 14,	 /* Expiry date */
   bf_15SettleDate		   = 15,     /* settlement date */
   bf_16conversiondate	   = 16,     /* Conversion date */
   bf_17capturedate		   = 17,	 /* Capture date   */
   bf_18merchtype		   = 18,     /* Merchant type */
   bf_19acqcntrycode       = 19,	/*  Acquirer country code */
   bf_22posentry           = 22,     /* Pos entry mode  */
   bf_23csn                = 23,	 /* Card Sequence Number */
   bf_26pospincapturecode  = 26,	 /* Point of service pin capture Code (Abc) */
   bf_28txnfeeamount       = 28,	 /* Transaction fee amount */
   bf_29settlementfeeamt   = 29,     /* Settlement fee amount */
   bf_32acqidcode          = 32,     /* Acquiring institution's ID code  */
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
   bf_46additionalfees     = 46,     /* Additional Fees*/
   bf_47addldata           = 47,     /* Additional private data */
   bf_48merchname          = 48,     /* Merchant name */
   bf_49trncurcd           = 49,     /* Transaction currency code */
   bf_50settlmntcurcd	   = 50,     /* settlement currency code */
   bf_51crdcurcd           = 51,     /* Cardholder Billing Currency Code */
   bf_52pinblock           = 52,     /* Pin block */
   bf_54AddlAmount         = 54,     /* Additional amount */
   bf_55IccData		       = 55,     /* IC Card data*/
   bf_56rplcmntaddlfee     = 56,     /* Replacement Additional Fees */
   bf_57authlfecycle	   = 57,     /* Authorization life cycle */
   bf_58nationalposcndtncd = 58,	 /* National Point-Of-Service Condition Code*/
   bf_59nationalposgeodata = 59,     /* National Point-Of-Service geogrphic data  */
   bf_60advicersncode	   = 60,     /* Advice reason code */
   bf_61acqtransprtdata    = 61,     /* Acquirer transport data */
   bf_62isstransprtdata    = 62,     /* Issuer transport data */
   bf_63pulsedata		   = 63,     /* Pulse data */
   bf_70ntwrkmngmntinfocd  = 70,     /* Network managment info code*/
   bf_90originaldata	   = 90,	 /* Original data */
   bf_91fileupdatecode     = 91,     /* File update code */
   bf_95rplcmntamounts     = 95,     /* Replacement amounts */
   bf_96msgsecuritycode    = 96,     /* Message security code */
   bf_98payee			   = 98,     /* payee */
   bf_101filename          = 101,    /* File Name */
   bf_102accidentificaton1 = 102,    /* Account identification 1*/
   bf_103accidentificaton2 = 103,    /*Account identification 2 */
   bf_104txndescription    = 104,    /* Transaction description */
   bf_105largepvtdata1	   = 105,    /* Large private data 1*/
   bf_109senderdata		   = 109,    /* Senders data */
   bf_110receiverdata	   = 110,    /* Receiver data */
   bf_111additionaldataacq = 111,    /* Additional data */
   bf_113authagntinstidcd  = 113,  	 /* Authorizing agent institution id code */
   bf_114contrycode		   = 114,	 /* country code */
   bf_120accqualifiers     = 120, 	 /* account qualifiers */
   bf_121additionaldataiss = 121,    /* additional data issuer */
   bf_122sponsorbankid     = 122,    /* Sponsor bank id */
   bf_123avsauthdata       = 123,    /* Avs/check auth data*/
   bf_124info              = 124,    /* info , text */
   bf_125ntwrkmngmntinfo   = 125,     /* Network Management info */
   bf_126isstracedata      = 126,    /* issuer trace data */
   bf_127acqtracedata      = 127,    /* acquirer trace data */

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
   enum   bitfields  td_bitfields[PULSE_EXTENDED_BITFIELD_SIZE]; /* fields to put in msg */
};


static  struct trandef TRANTABLE[] = {

	/*****************************************************
	AUTHORIZATION REQUEST
	*******************************************************/
	{
		AUTH_AUTHORIZATION,
		0x30,0x32,0x30,0x30,                  /* message id, 0100 */
		0x30,0x30,0x30,0x30,0x30,0x30,        /* processing code */
		/**** OPTIONS ****/
		topt1_postadv,
		topt2_referral+topt2_reversal+topt2_floor,
		/**** BIT FIELDS ****/
		bf_2pan,									/* MAND */
		bf_3proc,									/* MAND */
		bf_4amount,				  					/* MAND */
		bf_6cardbillamt,							/* COND */
		bf_7trntime,								/* MAND */
		bf_11stan,									/* MAND */
		bf_12time,									/* MAND */
		bf_13date,									/* MAND */
		bf_14expiry,								/* COND */
		bf_15SettleDate,							/* MAND */
		bf_18merchtype, 							/* MAND */
		bf_19acqcntrycode,
		bf_22posentry,								/* MAND */
		bf_23csn,                 					/* COND */
		bf_26pospincapturecode,  	        		/* COND */
		bf_28txnfeeamount,
		bf_32acqidcode,								/* MAND */
		bf_33fwdinstidcode,	    					/* MAND */
		bf_35track2,								/* COND */
		bf_37rrn,									/* MAND */
		bf_41termid,								/* MAND */
		bf_42cardacc,								/* MAND */
		bf_43cardacceptloc,							/* MAND */
		bf_47addldata,         						/* OPTN */
		bf_48merchname,								/* MAND */
		bf_49trncurcd,								/* MAND */
		bf_51crdcurcd,								/* COND */
		bf_52pinblock,        						/* COND */
		bf_55IccData,         						/* COND */
		bf_58nationalposcndtncd, 					/* MAND */
		bf_59nationalposgeodata, 					/* MAND */
		bf_63pulsedata, 		 					/* MAND */
		bf_done			        			/* end of fields to move */
	},

	/***********************************************
	 *	AUTHORIZATION   RESPONSE
	***********************************************/
	{
		AUTH_AUTHORIZATION_RESPONSE,
		0x30,0x32,0x31,0x30,                  	/* message id, 0110 */
		0x30,0x30,0x30,0x30,0x30,0x30,        	/* processing code */
		/**** OPTIONS ****/
		topt1_postadv,
		topt2_referral+topt2_reversal+topt2_floor,
		/**** BIT FIELDS ****/
		bf_2pan,				    			/* MAND ECHO */
		bf_3proc,				    			/* MAND ECHO */
		bf_4amount,			    				/* MAND ECHO */
		bf_7trntime,			    			/* MAND ECHO */
		bf_11stan,			    				/* MAND ECHO */
		bf_12time,		        				/* MAND ECHO */
		bf_13date,                				/* MAND ECHO */
		bf_14expiry,              				/* OPT  */
		bf_15SettleDate,         				/* MAND ECHO */
		bf_22posentry,							/* MAND ECHO */
		bf_23csn,								/* MAND ECHO */
		bf_32acqidcode,              			/* MAND ECHO */
		bf_33fwdinstidcode,       				/* MAND ECHO */
		bf_37rrn,				    			/* MAND ECHO */
		bf_38authcode,		    				/* COND PREP */
		bf_39respcode,		    				/* MAND PREP */
		bf_41termid,			    			/* MAND ECHO */
		bf_42cardacc,			    			/* MAND ECHO */
		bf_44respdata,	        				/* OPTN PREP */
		bf_49trncurcd,		    				/* MAND ECHO */
		bf_54AddlAmount,          				/* OPTN PREP */
		bf_55IccData,							/* MAND ECHO */
		bf_58nationalposcndtncd,				/* MAND ECHO */
		bf_63pulsedata,      					/* MAND ECHO */
		bf_done				    			/* end of fields to move */
	},

	/********************************************
	*	BALANCE INQUIRY REQUEST
	********************************************/
	{
		AUTH_BALANCE_INQUIRY,
		0x30,0x32,0x30,0x30,				/* message id */
		0x33,0x31,0x30,0x30,0x30,0x30, 		/* processing code */
		/**** OPTIONS ****/
		topt1_postadv,
		topt2_referral+topt2_reversal+topt2_floor,
		/**** BIT FIELDS ****/
		bf_2pan,					/* MAND */
		bf_3proc,					/* MAND */
		bf_4amount,				  					/* MAND */
		bf_7trntime,				/* MAND */
		bf_11stan,					/* MAND */
		bf_12time,					/* MAND */
		bf_13date,					/* MAND */
		bf_14expiry,				/* OPTN */
		bf_15SettleDate,         	/* MAND ECHO */
		bf_18merchtype, 			/* MAND */
		bf_19acqcntrycode,
		bf_22posentry,				/* MAND */
		bf_23csn,					/* MAND ECHO */
		bf_28txnfeeamount,
		bf_32acqidcode,				/* MAND */
		bf_35track2,				/* COND */
		bf_37rrn,					/* MAND */
		bf_41termid,				/* MAND */
		bf_42cardacc,			    /* MAND */
		bf_43cardacceptloc,			/* MAND */
		bf_48merchname,     		/* MAND */
		bf_49trncurcd,     			/* MAND */
		bf_52pinblock,
		bf_54AddlAmount,      		/* COND */
		bf_55IccData,				/* MAND ECHO */
		bf_58nationalposcndtncd,	/* MAND*/
		bf_59nationalposgeodata,    /* MAND */
		bf_63pulsedata,
		bf_done			        	/* end of fields to move */
	},

	/***************************************************
	*	BALANCE INQUIRY RESPONSE
	****************************************************/
	{
		AUTH_BALANCE_INQUIRY_RESPONSE,
		0x30,0x32,0x31,0x30,				/* message id */
		0x30,0x30,0x30,0x30,0x30,0x30, 		/* processing code */
		/**** OPTIONS ****/
		topt1_postadv,
		topt2_referral+topt2_reversal+topt2_floor,
		/**** BIT FIELDS ****/
		bf_2pan,				    /* MAND ECHO */
		bf_3proc,				    /* MAND ECHO */
		bf_4amount,				  	/* MAND */
		bf_7trntime,			    /* MAND ECHO */
		bf_11stan,			    	/* MAND ECHO */
		bf_12time,		       		/* MAND ECHO */
		bf_13date,               	/* MAND ECHO */
		bf_14expiry,              	/* MAND PREP  */
		bf_15SettleDate,         	/* MAND ECHO */
		bf_22posentry,				/* MAND */
		bf_32acqidcode,             /* MAND ECHO */
		bf_33fwdinstidcode,       	/* MAND ECHO */
		bf_37rrn,				    /* MAND ECHO */
		bf_39respcode,		   		/* MAND PREP  */
		bf_41termid,			    /* MAND ECHO */
		bf_42cardacc,			    /* MAND ECHO */
		bf_49trncurcd,		    	/* MAND ECHO */
		bf_54AddlAmount,
		bf_55IccData,				/* MAND ECHO */
		bf_58nationalposcndtncd,	/* MAND*/
		bf_63pulsedata,
		bf_done				    /* end of fields to move */
	},

	/*****************************************************
		CARD VERIFICATION REQUEST
	*******************************************************/
	{
		AUTH_CARD_VERIFICATION,
		0x30,0x31,0x30,0x30,				   /* message id, 0100 */
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
		bf_26pospincapturecode,			   	/* COND */
		bf_28txnfeeamount,
		bf_32acqidcode,			  	 		/* MAND */
		bf_33fwdinstidcode,	   				/* MAND */
		bf_35track2,			   			/* COND */
		bf_37rrn,				   			/* MAND */
		bf_38authcode,			   			/* MAND */
		bf_41termid,			   			/* MAND */
		bf_42cardacc,			   			/* MAND */
		bf_43cardacceptloc,	   				/* MAND */
		bf_47addldata, 		   				/* OPTN */
		bf_49trncurcd, 		   				/* MAND */
		bf_52pinblock, 		   				/* COND */
		bf_55IccData,			   			/* COND */
		bf_done				   			/* end of fields to move */
	},

	/***********************************************
	*	CARD VERIFICATION RESPONSE
	***********************************************/
	{
		AUTH_CARD_VERIFICATION_RESPONSE,
		0x30,0x31,0x31,0x30,				   /* message id, 0110 */
		0x30,0x33,0x32,0x30,0x30,0x30, 	   /* processing code */
		/**** OPTIONS ****/
		topt1_postadv,
		topt2_referral+topt2_reversal+topt2_floor,
		/**** BIT FIELDS ****/
		bf_2pan,				   			/* MAND ECHO */
		bf_3proc,				   			/* MAND ECHO */
		bf_4amount,			  	 			/* MAND ECHO */
		bf_7trntime,			   			/* MAND ECHO */
		bf_11stan, 			   				/* MAND ECHO */
		bf_12time, 			   				/* MAND ECHO */
		bf_13date, 			   				/* MAND ECHO */
		bf_14expiry,			   			/* MAND PREP */
		bf_23csn,							/* MAND */
		bf_32acqidcode,			   			/* MAND ECHO */
		bf_33fwdinstidcode,	   				/* MAND ECHO */
		bf_37rrn,				   			/* MAND ECHO */
		bf_38authcode, 		   				/* COND PREP */
		bf_39respcode, 		   				/* MAND PREP */
		bf_41termid,			   			/* MAND ECHO */
		bf_42cardacc,			   			/* MAND ECHO */
		bf_44respdata, 		   				/* OPTN PREP */
		bf_49trncurcd, 		   				/* MAND ECHO */
		bf_54AddlAmount,		   			/* OPTN PREP   */
		bf_55IccData,
		bf_done				  		 	/* end of fields to move */
	},

	/********************************************
	*	POS SALE REQUEST
	********************************************/
	{
		AUTH_SALE,
		0x30,0x32,0x30,0x30,                  /* message id, 0200 */
		0x30,0x30,0x30,0x30,0x30,0x30,        /* processing code */
		/**** OPTIONS ****/
		topt1_postadv,
		topt2_referral+topt2_reversal+topt2_floor,
		/**** BIT FIELDS ****/
		bf_2pan,                				/* MAND */
		bf_3proc,               				/* MAND */
		bf_4amount,             				/* MAND */
		bf_7trntime,            				/* MAND */
		bf_11stan,              				/* MAND */
		bf_12time,              				/* MAND */
		bf_13date,              				/* MAND */
		bf_14expiry,		    				/* OPTN */
		bf_15SettleDate,		 				/* MAND */
		bf_18merchtype,			 				/* MAND */
		bf_19acqcntrycode,
		bf_22posentry,          				/* MAND */
		bf_23csn,								/* MAND */
		bf_26pospincapturecode,		    		/* COND */
		bf_28txnfeeamount,
		bf_32acqidcode,            				/* MAND */
		bf_33fwdinstidcode,     				/* MAND */
		bf_35track2,            				/* COND */
		bf_37rrn,	            				/* MAND */
		bf_38authcode,							/* MAND */
		bf_41termid,            				/* MAND */
		bf_42cardacc,           				/* MAND */
		bf_43cardacceptloc,    					/* MAND */
		bf_47addldata,          				/* COND */
		bf_48merchname,         				/* MAND */
		bf_49trncurcd,          				/* MAND */
		bf_52pinblock,          				/* COND */
		bf_54AddlAmount,        				/* MAND */
		bf_55IccData,							/* MAND */
		bf_58nationalposcndtncd, 				/* MAND */
		bf_59nationalposgeodata,  				/* MAND */
		bf_63pulsedata,				 			/* MAND */
		bf_111additionaldataacq,  				/* MAND */
		bf_done			        			/* end of fields to move */
	},

	/***********************************************
	*	POS SALE RESPONSE
	***********************************************/
	{
		AUTH_SALE_RESPONSE,
		0x30,0x32,0x31,0x30,                  /* message id, 0110 */
		0x30,0x30,0x34,0x30,0x30,0x30,        /* processing code */
		/**** OPTIONS ****/
		topt1_postadv,
		topt2_referral+topt2_reversal+topt2_floor,
		/**** BIT FIELDS ****/
		bf_2pan,                		/* MAND ECHO */
		bf_3proc,               		/* MAND ECHO */
		bf_4amount,             		/* MAND ECHO */
		bf_7trntime,            		/* MAND ECHO */
		bf_11stan,              		/* MAND ECHO */
		bf_12time,              		/* MAND ECHO */
		bf_13date,             	 		/* MAND ECHO */
		bf_14expiry,		    		/* MAND PREP */
		bf_15SettleDate,				/* MAND	*/
		bf_22posentry,					/* MAND	*/
		bf_23csn,						/* MAND	*/
		bf_32acqidcode,         		/* MAND ECHO */
		bf_33fwdinstidcode,     		/* MAND ECHO */
		bf_37rrn,	            		/* MAND ECHO */
		bf_38authcode,          		/* OPTN PREP */
		bf_39respcode,          		/* MAND PREP */
		bf_41termid,            		/* MAND ECHO */
		bf_42cardacc,           		/* MAND ECHO */
		bf_44respdata,	        		/* OPTN PREP */
		bf_49trncurcd,          		/* MAND ECHO */
		bf_54AddlAmount,       	 		/* OPTI PREP */
		bf_55IccData,					/* MAND	*/
		bf_58nationalposcndtncd,		/* MAND	*/
		bf_63pulsedata,					/* MAND	*/
		bf_done                 /* end of fields to move */
	},

	/*****************************************************
	*	VOID SALE REQUEST
	*******************************************************/
	{
		AUTH_VOID_SALE,
		0x30,0x34,0x32,0x30,					/* message id, 1420 */
		0x30,0x30,0x30,0x30,0x30,0x30,			/* processing code */
		/**** OPTIONS ****/
		topt1_postadv,
		topt2_referral+topt2_reversal+topt2_floor,
		/**** BIT FIELDS ****/
		bf_2pan,								/* MAND */
		bf_3proc, 								/* MAND */
		bf_4amount,								/* MAND */
		bf_7trntime,							/* MAND */
		bf_11stan,								/* MAND */
		bf_12time,								/* MAND */
		bf_13date,								/* MAND */
		bf_14expiry,							/* OPTN */
		bf_22posentry,							/* MAND */
		bf_23csn, 								/* MAND */
		bf_26pospincapturecode, 				/* COND */
		bf_28txnfeeamount,
		bf_32acqidcode,							/* MAND */
		bf_33fwdinstidcode,						/* MAND */
		bf_35track2,							/* COND */
		bf_37rrn, 								/* MAND */
		bf_38authcode,							/* MAND */
		bf_41termid,							/* MAND */
		bf_42cardacc, 							/* MAND */
		bf_43cardacceptloc,						/* MAND */
		bf_47addldata,							/* OPTN */
		bf_49trncurcd,							/* MAND */
		bf_52pinblock,							/* COND */
		bf_55IccData, 							/* COND */
		bf_done					/* end of fields to move */
	},

	{
	/***********************************************
	*	AUTH_VOID_SALE RESPONSE
	***********************************************/
		AUTH_VOID_SALE_RESPONSE,
		0x30,0x34,0x33,0x30,					/* message id, 1430 */
		0x30,0x30,0x30,0x30,0x30,0x30,			/* processing code */
		/**** OPTIONS ****/
		topt1_postadv,
		topt2_referral+topt2_reversal+topt2_floor,
		/**** BIT FIELDS ****/
		bf_2pan,								/* MAND ECHO */
		bf_3proc, 								/* MAND ECHO */
		bf_4amount,								/* MAND ECHO */
		bf_7trntime,							/* MAND ECHO */
		bf_11stan,								/* MAND ECHO */
		bf_12time,								/* MAND ECHO */
		bf_13date,								/* MAND ECHO */
		bf_14expiry,							/* MAND PREP */
		bf_23csn,								/* MAND	*/
		bf_32acqidcode,							/* MAND ECHO */
		bf_33fwdinstidcode,						/* MAND ECHO */
		bf_37rrn, 								/* MAND ECHO */
		bf_38authcode,							/* COND PREP */
		bf_39respcode,							/* MAND PREP */
		bf_41termid,							/* MAND ECHO */
		bf_42cardacc, 							/* MAND ECHO */
		bf_44respdata,							/* OPTN PREP */
		bf_49trncurcd,							/* MAND ECHO */
		bf_54AddlAmount,						/* OPTN PREP */
		bf_55IccData,							/* MAND	*/
		bf_done								/* end of fields to move */
	},

	/********************************************
	*	CASH ADVANCE REQUEST
	********************************************/
	{
		AUTH_CASH_ADVANCE,
		0x30,0x32,0x30,0x30,                  /* message id, 0200 */
		0x30,0x31,0x33,0x30,0x30,0x30,        /* processing code */
		/**** OPTIONS ****/
		topt1_postadv,
		topt2_referral+topt2_reversal+topt2_floor,
		/**** BIT FIELDS ****/
		bf_2pan,                				/* MAND */
		bf_3proc,               				/* MAND */
		bf_4amount,             				/* MAND */
		bf_7trntime,            				/* MAND */
		bf_11stan,              				/* MAND */
		bf_12time,             				 	/* MAND */
		bf_13date,              				/* MAND */
		bf_14expiry,		   				 	/* COND */
		bf_15SettleDate,						/* MAND */
		bf_18merchtype,			 				/* MAND */
		bf_19acqcntrycode,
		bf_22posentry,          				/* MAND */
		bf_23csn,								/* MAND ECHO */
		bf_26pospincapturecode, 				/* COND */
		bf_28txnfeeamount,
		bf_32acqidcode,         				/* MAND */
		bf_33fwdinstidcode,     				/* MAND */
		bf_35track2,            				/* COND */
		bf_37rrn,	            				/* MAND */
		bf_41termid,            				/* MAND */
		bf_42cardacc,           				/* MAND */
		bf_43cardacceptloc,    		 			/* MAND */
		bf_48merchname,							/* MAND */
		bf_49trncurcd,          				/* MAND */
		bf_52pinblock,          				/* COND */
		bf_55IccData, 							/* COND */
		bf_58nationalposcndtncd,				/* MAND */
		bf_59nationalposgeodata,  				/* MAND */
		bf_63pulsedata,							/* MAND	*/
		bf_done			        			/* end of fields to move */
	},

	/***********************************************
	 *  CASH ADVANCE RESPONSE
	***********************************************/
	{
		AUTH_CASH_ADVANCE_RESPONSE,
		0x30,0x32,0x31,0x30,                  /* message id, 0210 */
		0x30,0x31,0x30,0x30,0x30,0x30,        /* processing code */
		/**** OPTIONS ****/
		topt1_postadv,
		topt2_referral+topt2_reversal+topt2_floor,
		/**** BIT FIELDS ****/
		bf_2pan,                /* MAND ECHO */
		bf_3proc,               /* MAND ECHO */
		bf_4amount,             /* MAND ECHO */
		bf_7trntime,            /* MAND ECHO */
		bf_11stan,              /* MAND ECHO */
		bf_12time,              /* MAND ECHO */
		bf_13date,              /* MAND ECHO */
		bf_14expiry,		    /* MAND PREP  */
		bf_15SettleDate,		/* MAND */
		bf_22posentry,          /* MAND */
		bf_23csn,
		bf_32acqidcode,         /* MAND ECHO */
		bf_33fwdinstidcode,     /* MAND ECHO */
		bf_37rrn,	            /* MAND ECHO */
		bf_38authcode,          /* OPTN PREP   */
		bf_39respcode,          /* MAND PREP  */
		bf_41termid,            /* MAND ECHO */
		bf_42cardacc,           /* MAND ECHO */
		bf_44respdata,	        /* OPTN PREP   */
		bf_49trncurcd,          /* MAND ECHO */
		bf_54AddlAmount,        /* OPTI PREP    */
		bf_55IccData,
		bf_58nationalposcndtncd,/* MAND */
		bf_63pulsedata,			/* MAND	*/
		bf_done                 /* end of fields to move */
	},

	/********************************************
	 *  ADVICE REQUEST
	********************************************/
	{
		AUTH_ADVICE,
		0x30,0x32,0x32,0x30,                  /* message id, 0220 */
		0x30,0x30,0x30,0x30,0x30,0x30,        /* processing code */
		/**** OPTIONS ****/
		topt1_postadv,
		topt2_referral+topt2_reversal+topt2_floor,
		/**** BIT FIELDS ****/
		bf_2pan,                /* MAND ECHO */
		bf_3proc,               /* MAND 00X000 */
		bf_4amount,             /* MAND PREP  */
		bf_7trntime,            /* MAND PREP  */
		bf_11stan,              /* MAND PREP  */
		bf_12time,              /* MAND PREP  */
		bf_13date,              /* MAND PREP  */
		bf_14expiry,		    /* OPTN PREP  */
		bf_15SettleDate,		/* MAND */
		bf_18merchtype,			/* MAND */
		bf_22posentry,          /* MAND PREP */
		bf_23csn,				/* MAND */
		bf_28txnfeeamount,
		bf_32acqidcode,         /* MAND ECHO */
		bf_33fwdinstidcode,     /* MAND ECHO */
		bf_35track2,            /* COND PREP */
		bf_37rrn,	            /* MAND PREP */
		bf_38authcode,          /* MAND ECHO */
		bf_39respcode,			/* MAND */
		bf_41termid,            /* MAND PREP */
		bf_42cardacc,           /* MAND ECHO */
		bf_43cardacceptloc,     /* MAND PREP */
		bf_47addldata,          /* OPTN PREP */
		bf_48merchname,			/* MAND */
		bf_49trncurcd,          /* MAND ECHO */
		bf_52pinblock,			/* MAND */
		bf_55IccData,			/* MAND */
		bf_58nationalposcndtncd,/* MAND */
		bf_59nationalposgeodata,/* MAND */
		bf_60advicersncode,		/* MAND */
		bf_63pulsedata,			/* MAND */
		bf_111additionaldataacq,/* MAND */
		bf_done                 /* end of fields to move */
	},

	/********************************************
	 * ADVICE RESPONSE
	********************************************/
	{
		AUTH_ADVICE_RESPONSE,
		0x30,0x32,0x33,0x30,                  /* message id, 0230 */
		0x30,0x30,0x30,0x30,0x30,0x30,        /* processing code */
		/**** OPTIONS ****/
		topt1_postadv,
		topt2_referral+topt2_reversal+topt2_floor,
		/**** BIT FIELDS ****/
		bf_2pan,                /* MAND ECHO */
		bf_3proc,               /* MAND ECHO */
		bf_4amount,             /* MAND ECHO */
		bf_6cardbillamt,        /* COND PREP  */
		bf_7trntime,            /* MAND ECHO */
		bf_11stan,              /* MAND ECHO */
		bf_12time,              /* MAND ECHO */
		bf_13date,              /* MAND ECHO */
		bf_15SettleDate,		/* MAND */
		bf_23csn,				/* MAND */
		bf_32acqidcode,         /* MAND ECHO */
		bf_33fwdinstidcode,     /* MAND ECHO */
		bf_37rrn,	            /* MAND ECHO */
		bf_38authcode,          /* MAND ECHO */
		bf_39respcode,          /* MAND PREP  */
		bf_41termid,            /* MAND ECHO */
		bf_42cardacc,           /* MAND ECHO */
		bf_49trncurcd,          /* MAND ECHO */
		bf_51crdcurcd,          /* COND PREP  */
		bf_55IccData,			/* MAND */
		bf_63pulsedata,			/* MAND */
		bf_done
	},

	/****************************************
	* REVERSAL ADVICE REQUEST
	*****************************************/
	{
		AUTH_REVERSAL_ADVICE,
		0x30,0x34,0x32,0x30,                  /* message id, 0420 */
		0x30,0x30,0x30,0x30,0x30,0x30,        /* processing code */
		/**** OPTIONS ****/
		topt1_postadv,
		topt2_referral+topt2_reversal+topt2_floor,
		/**** BIT FIELDS ****/
		bf_2pan,                /* MAND ECHO */
		bf_3proc,               /* MAND ECHO */
		bf_4amount,             /* MAND ECHO */
		bf_6cardbillamt,        /* COND ECHO sent by UnionPay only */
		bf_7trntime,            /* MAND PREP  */
		bf_11stan,              /* MAND PREP  */
		bf_12time,              /* MAND ECHO */
		bf_13date,              /* MAND ECHO */
		bf_14expiry,
		bf_15SettleDate,		/* MAND */
		bf_22posentry,          /* MAND ECHO */
		bf_23csn,				/* MAND */
		bf_28txnfeeamount,
		bf_32acqidcode,         /* MAND ECHO */
		bf_33fwdinstidcode,     /* MAND ECHO */
		bf_37rrn,	            /* MAND ECHO */
		bf_38authcode,          /* COND PREP  */
		bf_39respcode,			/* MAND */
		bf_41termid,            /* MAND ECHO */
		bf_44respdata,          /* COND ECHO sent by UnionPay only */
		bf_49trncurcd,          /* MAND ECHO */
		bf_50settlmntcurcd,
		bf_51crdcurcd,          /* COND ECHO sent  only */
		bf_55IccData,			/* MAND */
		bf_58nationalposcndtncd,/* MAND */
		bf_59nationalposgeodata,
		bf_60advicersncode,		/* MAND */
		bf_63pulsedata,			/* MAND */
		bf_90originaldata,		/* MAND */
		bf_done			    /* end of fields to move */
	},

	/***********************************************
	*  REVERSAL ADVICE RESPONSE
	***********************************************/
	{
		AUTH_REVERSAL_ADVICE_RESPONSE,
		0x30,0x34,0x33,0x30,                  /* message id, 1430 */
		0x30,0x30,0x30,0x30,0x30,0x30,        /* processing code */
		/**** OPTIONS ****/
		topt1_postadv,
		topt2_referral+topt2_reversal+topt2_floor,
		/**** BIT FIELDS ****/
		bf_2pan,                /* MAND ECHO */
		bf_3proc,               /* MAND ECHO */
		bf_4amount,             /* MAND ECHO */
		bf_7trntime,            /* MAND ECHO */
		bf_11stan,              /* MAND ECHO */
		bf_12time,              /* MAND ECHO */
		bf_13date,              /* MAND ECHO */
		bf_15SettleDate,		/* MAND */
		bf_23csn,				/* MAND */
		bf_32acqidcode,         /* MAND ECHO */
		bf_33fwdinstidcode,     /* MAND ECHO */
		bf_37rrn,	            /* MAND ECHO */
		bf_39respcode,          /* MAND PREP  */
		bf_41termid,            /* MAND ECHO */
		bf_42cardacc,           /* MAND ECHO */
		bf_49trncurcd,          /* MAND ECHO */
		bf_55IccData,			/* MAND */
		bf_63pulsedata,			/* MAND */
		bf_90originaldata,		/* MAND */
		bf_done				  /* end of fields to move */
	},

	/****************************************
	 * REVERSAL REQUEST
	*****************************************/
	{
		AUTH_REVERSAL,
		0x30,0x34,0x32,0x30,                  /* message id, 0420 */
		0x30,0x30,0x30,0x30,0x30,0x30,        /* processing code */
		/**** OPTIONS ****/
		topt1_postadv,
		topt2_referral+topt2_reversal+topt2_floor,
		/**** BIT FIELDS ****/
		bf_2pan,                /* MAND ECHO */
		bf_3proc,               /* MAND ECHO */
		bf_4amount,             /* MAND ECHO */
		bf_6cardbillamt,        /* COND ECHO sent by UnionPay only */
		bf_7trntime,            /* MAND PREP  */
		bf_11stan,              /* MAND PREP  */
		bf_12time,              /* MAND ECHO */
		bf_13date,              /* MAND ECHO */
		bf_14expiry,
		bf_15SettleDate,		/* MAND */
		bf_18merchtype,
		bf_19acqcntrycode,
		bf_22posentry,          /* MAND ECHO */
		bf_23csn,				/* MAND */
		bf_28txnfeeamount,
		bf_32acqidcode,         /* MAND ECHO */
		bf_33fwdinstidcode,     /* MAND ECHO */
		bf_37rrn,	            /* MAND ECHO */
		bf_38authcode,          /* COND PREP  */
		bf_39respcode,			/* MAND */
		bf_41termid,            /* MAND ECHO */
		bf_44respdata,          /* COND ECHO sent by UnionPay only */
		bf_49trncurcd,          /* MAND ECHO */
		bf_50settlmntcurcd,
		bf_51crdcurcd,          /* COND ECHO sent  only */
		bf_55IccData,			/* MAND */
		bf_58nationalposcndtncd,/* MAND */
		bf_59nationalposgeodata,
		bf_60advicersncode,		/* MAND */
		bf_63pulsedata,			/* MAND */
		bf_90originaldata,		/* MAND */
		bf_done			    /* end of fields to move */	       /* end of fields to move */
	},

	/***********************************************
	 * REVERSAL RESPONSE
	***********************************************/
	{
		AUTH_REVERSAL_RESPONSE,
		0x30,0x34,0x33,0x30,                  /* message id, 0430 */
		0x30,0x30,0x30,0x30,0x30,0x30,        /* processing code */
		/**** OPTIONS ****/
		topt1_postadv,
		topt2_referral+topt2_reversal+topt2_floor,
		/**** BIT FIELDS ****/
		bf_2pan,                /* MAND ECHO */
		bf_3proc,               /* MAND ECHO */
		bf_4amount,             /* MAND ECHO */
		bf_7trntime,            /* MAND ECHO */
		bf_11stan,              /* MAND ECHO */
		bf_12time,              /* MAND ECHO */
		bf_13date,              /* MAND ECHO */
		bf_15SettleDate,		/* MAND */
		bf_23csn,				/* MAND */
		bf_32acqidcode,         /* MAND ECHO */
		bf_33fwdinstidcode,     /* MAND ECHO */
		bf_37rrn,	            /* MAND ECHO */
		bf_39respcode,          /* MAND PREP  */
		bf_41termid,            /* MAND ECHO */
		bf_42cardacc,           /* MAND ECHO */
		bf_49trncurcd,          /* MAND ECHO */
		bf_55IccData,			/* MAND */
		bf_63pulsedata,			/* MAND */
		bf_90originaldata,		/* MAND */
		bf_done				  /* end of fields to move */
	},

	/********************************************
	 * 	LOGON REQUEST
	********************************************/
	{
		AUTH_LOGON,
		0x30,0x38,0x30,0x30,                  /* message id, 0800 */
		0x30,0x30,0x30,0x30,0x30,0x30,        /* processing code */
		/**** OPTIONS ****/
		topt1_postadv,
		topt2_referral+topt2_reversal+topt2_floor,
		/**** BIT FIELDS ****/
		bf_7trntime,			 			/* MAND */
		bf_11stan,			 				/* MAND */
		bf_70ntwrkmngmntinfocd,	 			/* MAND */
		bf_96msgsecuritycode,				/* COND */
		bf_125ntwrkmngmntinfo,				/* COND */
		bf_done						/* end of fields to move */
	},

	/***********************************************
	 * 	LOGON  RESPONSE
	***********************************************/
	{
		AUTH_LOGON_RESPONSE,
		0x30,0x38,0x31,0x30,                  /* message id, 0810 */
		0x30,0x30,0x30,0x30,0x30,0x30,        /* processing code */
		/**** OPTIONS ****/
		topt1_postadv,
		topt2_referral+topt2_reversal+topt2_floor,
		/**** BIT FIELDS ****/
		bf_7trntime,		     			/* MAND ECHO  */
		bf_11stan,		 	 				/* MAND ECHO  */
		bf_39respcode,         				/* MAND */
		bf_70ntwrkmngmntinfocd,  			/* MAND */
		bf_done				 				/* end of fields to move */
	},

	/********************************************
	 * 	KEY RESET REQUEST
	********************************************/
	{
		AUTH_PIN_CHANGE,
		0x30,0x38,0x30,0x30,                  /* message id, 0800 */
		0x30,0x30,0x30,0x30,0x30,0x30,        /* processing code */
		/**** OPTIONS ****/
		topt1_postadv,
		topt2_referral+topt2_reversal+topt2_floor,
		/**** BIT FIELDS ****/
		bf_7trntime,			 			/* MAND */
		bf_11stan,			 				/* MAND */
		bf_47addldata,         				/* COND */
		bf_done						/* end of fields to move */
	},

	/********************************************
	 * 	 KEY RESET REPONSE
	********************************************/
	{
		AUTH_PIN_CHANGE_RESPONSE,
		0x30,0x38,0x31,0x30,                  /* message id, 0810 */
		0x30,0x30,0x30,0x30,0x30,0x30,        /* processing code */
		/**** OPTIONS ****/
		topt1_postadv,
		topt2_referral+topt2_reversal+topt2_floor,
		/**** BIT FIELDS ****/
		bf_7trntime,			 			/* MAND ECHO */
		bf_11stan,			 				/* MAND ECHO */
		bf_39respcode,         				/* MAND */
		bf_70ntwrkmngmntinfocd,
		bf_done							/* end of fields to move */
	}
};

#define N_TRANENTRY (sizeof(TRANTABLE) / sizeof(struct trandef))

#define N_BFData ( sizeof(BFData) / sizeof (struct bitfield_data) )

struct bitfield_data *ncpul_Find_Bitfield_Table (enum bitfields);
struct trandef       *ncpul_Find_Tran_Table     (pAUTH_TX p_auth_tx);

struct iso_msg_rec
{
   BYTE msgtype       				[4];
   BYTE bitmap1_bin   				[16];
   BYTE bitmap2_bin  				[16];
   BYTE pan_len       				[2];
   BYTE pan           				[19];
   BYTE proccode      				[6];
   BYTE amount        				[12];
   BYTE Settlement_amount			[12];//5
   BYTE ch_bill_amt   				[12];
   BYTE gmt_time      				[10];
   BYTE convRateSettlement			[8];//9
   BYTE convRateCardHolder			[8];//10
   BYTE stan          				[6];
   BYTE time          				[6];// hhmmss
   BYTE date		  				[4];
   BYTE date_expd	  				[4];
   BYTE SettleDate					[4];//15
   BYTE conversionDate				[4];//16
   BYTE captureDate					[4];//17
   BYTE Merch_type					[4];//18
   BYTE Acq_country_code			[3];//19
   BYTE posentry      				[3];//22
   BYTE card_seq_num  				[3];//23
   BYTE Pos_pin_capture_code   		[2];//26
   BYTE Txn_Fee_Amount				[9];//28
   BYTE Settlement_Fee_Amount		[9];//29
   BYTE acq_inst_len  				[2];//32
   BYTE acq_inst_id   				[11];
   BYTE fwd_inst_len  				[2];//33
   BYTE fwd_inst_id   				[11];
   BYTE track2_len    				[2];//35
   BYTE track2        				[37];
   BYTE rrn			  				[12];//37
   BYTE authcode      				[6];//38
   BYTE responsecd    				[2];//39
   BYTE service_code   				[3];//40
   BYTE termid        				[8];//41
   BYTE cardaccid     				[15];//42
   BYTE cardaccname 				[40];//43
   BYTE addl_rsp_len  				[2];
   BYTE addlrspdata   				[25];//44
   BYTE Additional_Fees_len			[3];//46
   BYTE Additional_Fees 			[88];
   BYTE addl_data_len				[3];//47
   BYTE addl_data     				[100];
   BYTE Merch_name_len				[3];//48
   BYTE Merch_name					[25];
   BYTE tran_curr_cd  				[3];//49
   BYTE settlement_curr_cd			[3];//50
   BYTE card_curr_cd				[3];//51
   BYTE pinblock      				[16];//52
   BYTE AddlAmts_len   				[3];//54
   BYTE AddlAmts     				[120];
   BYTE IccData_len   				[3];//55
   BYTE IccData	    				[255];
   BYTE Rplcment_additional_fee_len [3];//56
   BYTE Rplcment_additional_fee		[88];
   BYTE Auth_life_cycle_len			[3];//57
   BYTE Auth_life_cycle				[3];
   BYTE National_pos_cc_len			[3];//58
   BYTE National_pos_cc				[11];
   BYTE Nationalposgeogrphic_datalen[3];//59
   BYTE Nationalposgeographic_data	[17];
   BYTE Advice_reason_code_len		[3];//60
   BYTE	Advice_reason_code			[6];
   BYTE Acq_transport_data_len		[3];//61
   BYTE Acq_transport_data			[100];
   BYTE Iss_transport_data_len		[3];//62
   BYTE Iss_transport_data			[100];
   BYTE Pulse_data_len				[3];//63
   BYTE Pulse_data					[50];
   BYTE Network_Management_info_code[3];//70
   BYTE Original_data				[42];//90
   BYTE File_update_code			[1];//91
   BYTE Replace_amounts				[42];//95
   BYTE Msg_security_code			[8];//96
   BYTE Payee						[25];//98
   BYTE file_name_len				[2];//101
   BYTE file_name					[17];
   BYTE Account_identificaton1_len	[2];//102
   BYTE Account_identificaton1		[28];
   BYTE Account_identificaton2_len	[2];//103
   BYTE Account_identificaton2		[28];
   BYTE Txn_description_len			[3];//104
   BYTE Txn_description				[100];
   BYTE Large_pvt_data1_len			[3];//105
   BYTE Large_pvt_data1				[999];
   BYTE Sender_data_len				[3];//109
   BYTE Sender_data					[255];
   BYTE Receiver_data_len			[3];//110
   BYTE Receiver_data				[255];
   BYTE Additional_data_acq_len		[3];//111
   BYTE Additional_data_acq			[255];
   BYTE Auth_agent_inst_id_code_len [3];//113
   BYTE Auth_agent_inst_id_code	 	[11];
   BYTE Country_code_len			[3];//114
   BYTE Country_code				[3];
   BYTE Account_qualifiers_len		[3];//120
   BYTE Account_qualifiers			[6];
   BYTE Additional_data_iss_len		[3];//121
   BYTE Additional_data_iss			[255];
   BYTE Sponsor_bank_id_len			[3];//122
   BYTE Sponsor_bank_id				[11];
   BYTE Avs_check_auth_data_len		[3];//123
   BYTE Avs_check_auth_data			[255];
   BYTE Info_len					[3];//124
   BYTE Info						[255];
   BYTE Network_Mngment_info_len	[3];//125
   BYTE Network_Mngment_info		[125];
   BYTE Iss_trace_data_len			[3];//126
   BYTE	Iss_trace_data				[100];
   BYTE Acq_trace_data_len			[3];//127
   BYTE	Acq_trace_data				[100];
};
struct iso_msg_rec  ncpul_HOST_MESSAGE;

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
		bf_2pan,                      		 /* Primary Account Number  */
		ncpul_HOST_MESSAGE.pan_len,          /* data to move */
		2,                           		 /* # of bytes in pan len field */
		ncpul_Move_IfThere,                  /* mandatory */
		ncpul_vDataMove,                   	 /* move routine */
		ncpul_RspMovevData                   /* response routine */
	},
	{
		bf_3proc,                     		 /* field */
		ncpul_HOST_MESSAGE.proccode,         /* data to move */
		6,                            		 /* # of bytes in  iso field */
		ncpul_Move_Always,                 	 /* mandatory */
		ncpul_GenericMove,                   /* move routine */
		ncpul_RspMoveGeneric                 /* response routine */
	},
	{
		bf_4amount,                   		 /* field */
		ncpul_HOST_MESSAGE.amount,           /* data to move */
		12,                           		 /* # of bytes in  iso field */
		ncpul_Move_Always,                   /* mandatory */
		ncpul_GenericMove,                   /* move routine */
		ncpul_RspMoveGeneric                 /* response routine */
	},
	{
		bf_5settlmntamt,                   	  /* field */
		ncpul_HOST_MESSAGE.Settlement_amount, /* data to move */
		12,                           		  /* # of bytes in  iso field */
		ncpul_Move_IfThere,                   /* conditional */
		ncpul_GenericMove,                    /* move routine */
		ncpul_RspMoveGeneric                  /* response routine */
	},
	{
		bf_6cardbillamt,                	 /* field */
		ncpul_HOST_MESSAGE.ch_bill_amt,      /* data to move */
		12,                           		 /* # of bytes in  iso field */
		ncpul_Move_IfThere,                  /* mandatory */
		ncpul_GenericMove,                   /* move routine */
		ncpul_RspMoveGeneric                 /* response routine */
	},
	{
		bf_7trntime,                		 /* field */
		ncpul_HOST_MESSAGE.gmt_time,         /* data to move : MMDDhhmmss*/
		10,                           		 /* # of bytes in  iso field */
		ncpul_Move_Always,                   /* mandatory */
		ncpul_GenericMove,                   /* move routine */
		ncpul_RspMoveGeneric                 /* response routine */
	},
	{
    	bf_9convratesettlement,               /* field */
		ncpul_HOST_MESSAGE.convRateSettlement,/* data to move : MMDDhhmmss*/
		8,                           		  /* # of bytes in  iso field */
		ncpul_Move_IfThere,                   /* conditional */
		ncpul_GenericMove,                    /* move routine */
		ncpul_RspMoveGeneric                  /* response routine */
	},
	{
		bf_10convratecardhldr,                /* field */
		ncpul_HOST_MESSAGE.convRateCardHolder,/* data to move : MMDDhhmmss*/
		8,                           		  /* # of bytes in  iso field */
		ncpul_Move_IfThere,                   /* conditional */
		ncpul_GenericMove,                    /* move routine */
		ncpul_RspMoveGeneric                  /* response routine */
	},
	{
		bf_11stan,                    		/* field */
		ncpul_HOST_MESSAGE.stan,            /* data to move */
		6,                            		/* # of bytes in  iso field */
		ncpul_Move_Always,                  /* Mandatory */
		ncpul_GenericMove,                  /* move routine */
		ncpul_RspMoveGeneric                /* response routine */
	},
	{
		bf_12time,                      	/* field */
		ncpul_HOST_MESSAGE.time,            /* data to move */
		6,                             		/* # of bytes in  iso field */
		ncpul_Move_Always,                  /* Mandatory */
		ncpul_GenericMove,                  /* move routine */
		ncpul_RspMoveGeneric                /* response routine */
	},
	{
		bf_13date,                    		/* field */
		ncpul_HOST_MESSAGE.date,            /* effective data to move */
		4,                            		/* # of bytes in  iso field */
		ncpul_Move_Always,                  /* Mandatory */
		ncpul_GenericMove,                  /* move routine */
		ncpul_RspMoveGeneric                /* response routine */
	},
	{
		bf_14expiry,                  		/* field */
		ncpul_HOST_MESSAGE.date_expd,       /* data to move */
		4,                            		/* # of bytes in field */
		ncpul_Move_IfThere,                 /* conditional */
		ncpul_GenericMove,                  /* move routine */
		ncpul_RspMoveGeneric                /* response routine */
	},
	{
		bf_15SettleDate,                  	/* field */
		ncpul_HOST_MESSAGE.SettleDate,      /* data to move */
		4,                            		/* # of bytes in field */
		ncpul_Move_Always,                  /* Mandatory */
		ncpul_GenericMove,                  /* move routine */
		ncpul_RspMoveGeneric                /* response routine */
	},
	{
	    bf_16conversiondate,                /* field */
		ncpul_HOST_MESSAGE.conversionDate,  /* data to move */
		4,                                  /* # of bytes in field */
		ncpul_Move_IfThere,                 /* conditional */
		ncpul_GenericMove,                  /* move routine */
		ncpul_RspMoveGeneric                /* response routine */
	},
	{
		bf_17capturedate,                   /* field */
		ncpul_HOST_MESSAGE.captureDate,     /* data to move */
		4,                                  /* # of bytes in field */
		ncpul_Move_IfThere,                 /* Optional */
		ncpul_GenericMove,                  /* move routine */
		ncpul_RspMoveGeneric                /* response routine */
	},
	{
		bf_18merchtype,                     /* field */
		ncpul_HOST_MESSAGE.Merch_type,      /* data to move */
		4,                                  /* # of bytes in field */
		ncpul_Move_IfThere,                 /* conditional */
		ncpul_GenericMove,                  /* move routine */
		ncpul_RspMoveGeneric                /* response routine */
	},
	{
		bf_19acqcntrycode,                   /* field */
		ncpul_HOST_MESSAGE.Acq_country_code, /* data to move */
		3,                                   /* # of bytes in field */
		ncpul_Move_IfThere,                  /* conditional */
		ncpul_GenericMove,                   /* move routine */
		ncpul_RspMoveGeneric                 /* response routine */
	},
	{
		bf_22posentry,                		/* field */
		ncpul_HOST_MESSAGE.posentry,        /* data to move */
		3,                           		/* # of bytes in  iso field */
		ncpul_Move_IfThere,                 /* conditional */
		ncpul_GenericMove,                  /* move routine */
		ncpul_RspMoveGeneric                /* response routine */
	},
	{
		bf_23csn,                  		    /* field */
		ncpul_HOST_MESSAGE.card_seq_num,    /* data to move */
		3,                            	    /* # of bytes in field */
		ncpul_Move_IfThere,                 /* conditional */
		ncpul_GenericMove,                  /* move routine */
		ncpul_RspMoveGeneric                /* response routine */
	},
	{
	    bf_26pospincapturecode,                     /* field */
		ncpul_HOST_MESSAGE.Pos_pin_capture_code,    /* data to move */
		2,                             				/* # of bytes in field */
		ncpul_Move_IfThere,                 	    /* conditional */
		ncpul_GenericMove,                  	    /* move routine */
		ncpul_RspMoveGeneric                	    /* response routine */
	},
	{
		bf_28txnfeeamount,                  	/* field */
		ncpul_HOST_MESSAGE.Txn_Fee_Amount,    	/* data to move */
		9,                             			/* # of bytes in field */
		ncpul_Move_IfThere,                  	/* conditional */
		ncpul_GenericMove,                      /* move routine */
		ncpul_RspMoveGeneric                    /* response routine */
	},
	{
		bf_29settlementfeeamt,                    /* field */
		ncpul_HOST_MESSAGE.Settlement_Fee_Amount, /* data to move */
		9,                             			  /* # of bytes in field */
		ncpul_Move_IfThere,                       /* conditional */
		ncpul_GenericMove,                        /* move routine */
		ncpul_RspMoveGeneric                      /* response routine */
	},
	{
		bf_32acqidcode,                           /* field */
		ncpul_HOST_MESSAGE.acq_inst_len,          /* data to move */
		2,                            		      /* # of bytes in  iso field */
		ncpul_Move_Always,                        /* Mandatory */
		ncpul_vDataMove,                          /* move routine */
		ncpul_RspMovevData                        /* response routine */
	},
	{
		bf_33fwdinstidcode,                       /* field */
		ncpul_HOST_MESSAGE.fwd_inst_len,    	  /* data to move */
		2,                            		      /* # of bytes in  iso field */
		ncpul_Move_IfThere,                       /* conditional */
		ncpul_vDataMove,                          /* move routine */
		ncpul_RspMovevData                		  /* response routine */
	},
	{
		bf_35track2,                 		 	  /* field */
		ncpul_HOST_MESSAGE.track2_len,            /* data to move */
		2,                           		      /* # of bytes in track2 len field */
		ncpul_Move_IfThere,                       /* conditional */
		ncpul_vDataMove,                          /* move routine */
		ncpul_RspMovevData                        /* response routine */
	},
	{
		bf_37rrn,                  			      /* field */
		ncpul_HOST_MESSAGE.rrn,      		      /* data to move */
		12,                           	 		  /* # of bytes in track2 len field */
		ncpul_Move_Always,                  	  /* Mandatory */
		ncpul_GenericMove,                  	  /* move routine */
		ncpul_RspMoveGeneric                	  /* response routine */
	},
	{
		bf_38authcode,                			  /* field */
		ncpul_HOST_MESSAGE.authcode,        	  /* data to move */
		6,                            			  /* # of bytes in  iso field */
		ncpul_Move_IfThere,                 	  /* conditional */
		ncpul_GenericMove,                   	  /* move routine */
		ncpul_RspMoveGeneric                 	  /* response routine */
	},
	{
		bf_39respcode,                			   /* field */
		ncpul_HOST_MESSAGE.responsecd,      	   /* data to move */
		2,                            		       /* # of bytes in  iso field */
		ncpul_Move_IfThere,                 	   /* conditional */
		ncpul_GenericMove,                         /* move routine */
		ncpul_RspMoveGeneric                 	   /* response routine */
	},
	{
		bf_40service_code,                			/* field */
		ncpul_HOST_MESSAGE.service_code,      		/* data to move */
		3,                            				/* # of bytes in  iso field */
		ncpul_Move_IfThere,                 		/* conditional */
		ncpul_GenericMove,                   		/* move routine */
		ncpul_RspMoveGeneric                 		/* response routine */
	},
	{
		bf_41termid,                  				/* field */
		ncpul_HOST_MESSAGE.termid,          		/* data to move */
		8,                            				/* # of bytes in  iso field */
		ncpul_Move_Always,                 			/* Mandatory */
		ncpul_GenericMove,                  	 	/* move routine */
		ncpul_RspMoveGeneric                 		/* response routine */
	},
	{
		bf_42cardacc,                 				/* field */
		ncpul_HOST_MESSAGE.cardaccid,       		/* data to move */
		15,                          		 		/* # of bytes in  iso field */
		ncpul_Move_IfThere,                  		/* conditional */
		ncpul_GenericMove,                   		/* move routine */
		ncpul_RspMoveGeneric                 		/* response routine */
	},
	{
		bf_43cardacceptloc,                 		/* field */
		ncpul_HOST_MESSAGE.cardaccname,       		/* data to move */
		40,                          		 		/* # of bytes in  iso field */
		ncpul_Move_IfThere,                  		/* conditional */
		ncpul_GenericMove,                   		/* move routine */
		ncpul_RspMoveGeneric						/* response routine */
	},
	{
		bf_44respdata,                 				/* field */
		ncpul_HOST_MESSAGE.addl_rsp_len,       		/* data to move */
		2,                          		 		/* # of bytes in  iso field */
		ncpul_Move_IfThere,                  		/* conditional */
		ncpul_vDataMove,                   			/* move routine */
		ncpul_RspMovevData							/* response routine */
	},
	{
		bf_46additionalfees,                 		 /* field */
		ncpul_HOST_MESSAGE.Additional_Fees_len,      /* data to move */
		3,                            				 /* # of bytes in len field */
		ncpul_Move_IfThere,                   		 /* conditional */
		ncpul_vDataMove,                  			 /* move routine */
		ncpul_RspMovevData                		 	 /* response routine */
	},
	{
		bf_47addldata,                				/* field */
		ncpul_HOST_MESSAGE.addl_data_len,  	 		/* data to move */
		3,                            				/* # of bytes in track1 len field */
		ncpul_Move_IfThere,                	 		/* conditional */
		ncpul_vDataMove,                  			/* move routine */
		ncpul_RspMovevData                			/* response routine */
	},
	{
		bf_48merchname,               			 	/* field */
		ncpul_HOST_MESSAGE.Merch_name_len,   		/* data to move */
		3,                            				/* # of bytes in track1 len field */
		ncpul_Move_IfThere,                 		/* conditional */
		ncpul_vDataMove,                  			/* move routine */
		ncpul_RspMovevData                			/* response routine */
	},
	{
		bf_49trncurcd,               				/* field */
		ncpul_HOST_MESSAGE.tran_curr_cd,   			/* data to move */
		3,                           				/* # of bytes in  iso field */
		ncpul_Move_Always,                			/* mandatory */
		ncpul_GenericMove,                 			/* move routine */
		ncpul_RspMoveGeneric						/* response routine */
	},
	{
		bf_50settlmntcurcd,               			/* field */
		ncpul_HOST_MESSAGE.settlement_curr_cd,   	/* data to move */
		3,                           				/* # of bytes in  iso field */
		ncpul_Move_IfThere,               	 		/* mandatory    */
		ncpul_GenericMove,                 			/* move routine */
		ncpul_RspMoveGeneric						/* response routine */
	},
	{
		bf_51crdcurcd,              			 	/* field */
		ncpul_HOST_MESSAGE.card_curr_cd,    		/* data to move */
		3,                           				/* # of bytes in field */
		ncpul_Move_IfThere,                 		/* conditional    */
		ncpul_GenericMove,                   		/* move routine */
		ncpul_RspMoveGeneric                 		/* response routine */
	},
	{
		bf_52pinblock,                				/* field */
		ncpul_HOST_MESSAGE.pinblock,        		/* data to move */
		16,                            				/* # of bytes in  iso field */
		ncpul_Move_IfThere,                  		/* conditional */
		ncpul_GenericMove,                  		/* move routine */
		ncpul_RspMoveGeneric                		/* response routine */
	},
	{
		bf_54AddlAmount,                 			/* field */
		ncpul_HOST_MESSAGE.AddlAmts_len,   			/* data to move */
		3,                            				/* # of bytes in field */
		ncpul_Move_IfThere,                 	 	/* conditional    */
		ncpul_vDataMove,                 	 		/* move routine */
		ncpul_RspMovevData               		 	/* response routine */
	},
	{
		bf_55IccData,                 				/* field */
		ncpul_HOST_MESSAGE.IccData_len,   			/* data to move */
		3,                           		 		/* # of bytes in field */
		ncpul_Move_IfThere,                  		/* conditional    */
		ncpul_vDataMove,                  			/* move routine */
		ncpul_RspMovevData                			/* response routine */
	},
	{
		bf_56rplcmntaddlfee,                			 /* field */
		ncpul_HOST_MESSAGE.Rplcment_additional_fee_len,  /* data to move */
		3,                            					 /* # of bytes in field */
		ncpul_Move_IfThere,                 			 /* conditional    */
		ncpul_vDataMove,                  				 /* move routine */
		ncpul_RspMovevData                				 /* response routine */
	},
	{
		bf_57authlfecycle,                			 	/* field */
		ncpul_HOST_MESSAGE.Auth_life_cycle_len,   		/* data to move */
		3,                            					/* # of bytes in field */
		ncpul_Move_IfThere,                  			/* conditional    */
		ncpul_vDataMove,                  				/* move routine */
		ncpul_RspMovevData                				/* response routine */
	},
	{
		bf_58nationalposcndtncd,                 		/* field */
		ncpul_HOST_MESSAGE.National_pos_cc_len,   		/* data to move */
		3,                            					/* # of bytes in field */
		ncpul_Move_Always,                  			/* mandatory    */
		ncpul_vDataMove,                   				/* move routine */
		ncpul_RspMovevData                 				/* response routine */
	},
	{
		bf_59nationalposgeodata,                 		/* field */
		ncpul_HOST_MESSAGE.Nationalposgeogrphic_datalen,/* data to move */
		3,                            					/* # of bytes in field */
		ncpul_Move_IfThere,                 			 /* conditional    */
		ncpul_vDataMove,                  				/* move routine */
		ncpul_RspMovevData                				/* response routine */
	},
	{
		bf_60advicersncode,                 			/* field */
		ncpul_HOST_MESSAGE.Advice_reason_code_len,   	/* data to move */
		3,                           				 	/* # of bytes in field */
		ncpul_Move_IfThere,                 		 	/* conditional    */
		ncpul_vDataMove,                 			 	/* move routine */
		ncpul_RspMovevData                				/* response routine */
	},
	{
		bf_61acqtransprtdata,                 			/* field */
		ncpul_HOST_MESSAGE.Acq_transport_data_len,   	/* data to move */
		3,                            					/* # of bytes in field */
		ncpul_Move_IfThere,                 		 	/* conditional    */
		ncpul_vDataMove,                 		 		/* move routine */
		ncpul_RspMovevData               			 	/* response routine */
	},
	{
		bf_62isstransprtdata,                	 		/* field */
		ncpul_HOST_MESSAGE.Iss_transport_data_len,   	/* data to move */
		3,                            					/* # of bytes in field */
		ncpul_Move_IfThere,                  			/* conditional    */
		ncpul_vDataMove,                  				/* move routine */
		ncpul_RspMovevData                				/* response routine */
	},
	{
		bf_63pulsedata,                 				/* field */
		ncpul_HOST_MESSAGE.Pulse_data_len,   			/* data to move */
		3,                            					/* # of bytes in field */
		ncpul_Move_IfThere,                  			/* mandatory    */
		ncpul_vDataMove,                  				/* move routine */
		ncpul_RspMovevData                 				/* response routine */
	},
	{
		bf_70ntwrkmngmntinfocd,                 			/* field */
		ncpul_HOST_MESSAGE.Network_Management_info_code,    /* data to move */
		3,                            						/* # of bytes in field */
		ncpul_Move_IfThere,                  				/* mandatory    */
		ncpul_GenericMove,                  				/* move routine */
		ncpul_RspMoveGeneric                				/* response routine */
	},
	{
		bf_90originaldata,               			 		/* field */
		ncpul_HOST_MESSAGE.Original_data,    				/* data to move */
		42,                            						/* # of bytes in field */
		ncpul_Move_IfThere,                  				/* mandatory    */
		ncpul_GenericMove,                   				/* move routine */
		ncpul_RspMoveGeneric                				/* response routine */
	},
	{
		bf_91fileupdatecode,                				/* field */
		ncpul_HOST_MESSAGE.File_update_code,    			/* data to move */
		1,                            						/* # of bytes in field */
		ncpul_Move_IfThere,                  				/* mandatory    */
		ncpul_GenericMove,                  			 	/* move routine */
		ncpul_RspMoveGeneric                 				/* response routine */
	},
	{
		bf_95rplcmntamounts,                				/* field */
		ncpul_HOST_MESSAGE.Replace_amounts,    				/* data to move */
		42,                            						/* # of bytes in  iso field */
		ncpul_Move_IfThere,                 				/* conditional    */
		ncpul_GenericMove,               					/* move routine */
		ncpul_RspMoveGeneric                				/* response routine */
	},
	{
		bf_96msgsecuritycode,               	 			/* field */
		ncpul_HOST_MESSAGE.Msg_security_code,    			/* data to move */
		8,                            						/* # of bytes in  iso field */
		ncpul_Move_IfThere,                 				/* mandatory    */
		ncpul_GenericMove,               					/* move routine */
		ncpul_RspMoveGeneric                				/* response routine */
	},
	{
		bf_98payee,                							/* field */
		ncpul_HOST_MESSAGE.Payee,    						/* data to move */
		25,                            						/* # of bytes in  iso field */
		ncpul_Move_IfThere,                 				/* conditional    */
		ncpul_GenericMove,               					/* move routine */
		ncpul_RspMoveGeneric                				/* response routine */
	},
	{
		bf_101filename,                						/* field */
		ncpul_HOST_MESSAGE.file_name_len,    				/* data to move */
		2,                            						/* # of bytes in  iso field */
		ncpul_Move_IfThere,                 				/* mandatory    */
		ncpul_vDataMove,               						/* move routine */
		ncpul_RspMovevData                					/* response routine */
	},
	{
		bf_102accidentificaton1,                			/* field */
		ncpul_HOST_MESSAGE.Account_identificaton1_len,   	/* data to move */
		2,                            						/* # of bytes in  iso field */
		ncpul_Move_IfThere,                 				/* conditional    */
		ncpul_vDataMove,               						/* move routine */
		ncpul_RspMovevData                					/* response routine */
	},
	{
		bf_103accidentificaton2,                			/* field */
		ncpul_HOST_MESSAGE.Account_identificaton2_len,    	/* data to move */
		2,                            						/* # of bytes in  iso field */
		ncpul_Move_IfThere,                 				/* conditional    */
		ncpul_vDataMove,               						/* move routine */
		ncpul_RspMovevData                					/* response routine */
	},
	{
		bf_104txndescription,                				/* field */
		ncpul_HOST_MESSAGE.Txn_description_len,    			/* data to move */
		3,                            						/* # of bytes in  iso field */
		ncpul_Move_IfThere,                 				/* conditional    */
		ncpul_vDataMove,               						/* move routine */
		ncpul_RspMovevData                					/* response routine */
	},
	{
		bf_105largepvtdata1,                				/* field */
		ncpul_HOST_MESSAGE.Large_pvt_data1_len,    			/* data to move */
		3,                            						/* # of bytes in  iso field */
		ncpul_Move_IfThere,                 				/* conditional    */
		ncpul_vDataMove,               						/* move routine */
		ncpul_RspMovevData                					/* response routine */
	},
	{
		bf_109senderdata,               					/* field */
		ncpul_HOST_MESSAGE.Sender_data_len, 	   			/* data to move */
		3,                            						/* # of bytes in  iso field */
		ncpul_Move_IfThere,                 				/* conditional    */
		ncpul_vDataMove,               						/* move routine */
		ncpul_RspMovevData                					/* response routine */
	},
	{
		bf_110receiverdata,                					/* field */
		ncpul_HOST_MESSAGE.Receiver_data_len,    			/* data to move */
		3,                            						/* # of bytes in  iso field */
		ncpul_Move_IfThere,                 				/* conditional    */
		ncpul_vDataMove,               						/* move routine */
		ncpul_RspMovevData               			 		/* response routine */
	},
	{
		bf_111additionaldataacq,               			 	/* field */
		ncpul_HOST_MESSAGE.Additional_data_acq_len,    		/* data to move */
		3,                            						/* # of bytes in  iso field */
		ncpul_Move_IfThere,                 				/* conditional    */
		ncpul_vDataMove,               						/* move routine */
		ncpul_RspMovevData                					/* response routine */
	},
	{
		bf_113authagntinstidcd,               			 	/* field */
		ncpul_HOST_MESSAGE.Auth_agent_inst_id_code_len,    	/* data to move */
		3,                            						/* # of bytes in  iso field */
		ncpul_Move_IfThere,                 				/* conditional    */
		ncpul_vDataMove,               						/* move routine */
		ncpul_RspMovevData                					/* response routine */
	},
	{
		bf_114contrycode,                					/* field */
		ncpul_HOST_MESSAGE.Country_code_len,    			/* data to move */
		3,                            						/* # of bytes in  iso field */
		ncpul_Move_IfThere,                 				/* conditional    */
		ncpul_vDataMove,               						/* move routine */
		ncpul_RspMovevData                					/* response routine */
	},
	{
		bf_120accqualifiers,                				/* field */
		ncpul_HOST_MESSAGE.Account_qualifiers_len,    		/* data to move */
		3,                            						/* # of bytes in  iso field */
		ncpul_Move_IfThere,                 				/* conditional    */
		ncpul_vDataMove,               						/* move routine */
		ncpul_RspMovevData                					/* response routine */
	},
	{
		bf_121additionaldataiss,                			/* field */
		ncpul_HOST_MESSAGE.Additional_data_iss_len,    		/* data to move */
		3,                            						/* # of bytes in  iso field */
		ncpul_Move_IfThere,                 				/* optional    */
		ncpul_vDataMove,               						/* move routine */
		ncpul_RspMovevData                					/* response routine */
	},
	{
		bf_122sponsorbankid,                				/* field */
		ncpul_HOST_MESSAGE.Sponsor_bank_id_len,    			/* data to move */
		3,                            						/* # of bytes in  iso field */
		ncpul_Move_IfThere,                 				/* conditional    */
		ncpul_vDataMove,               						/* move routine */
		ncpul_RspMovevData               		 			/* response routine */
	},
	{
		bf_123avsauthdata,                					/* field */
		ncpul_HOST_MESSAGE.Avs_check_auth_data_len,    		/* data to move */
		3,                            						/* # of bytes in  iso field */
		ncpul_Move_IfThere,                 				/* conditional    */
		ncpul_vDataMove,               						/* move routine */
		ncpul_RspMovevData                					/* response routine */
	},
	{
		bf_124info,               				 			/* field */
		ncpul_HOST_MESSAGE.Info_len,    					/* data to move */
		3,                            						/* # of bytes in  iso field */
		ncpul_Move_IfThere,                 				/* conditional    */
		ncpul_vDataMove,               						/* move routine */
		ncpul_RspMovevData                					/* response routine */
	},
	{
		bf_125ntwrkmngmntinfo,                				/* field */
		ncpul_HOST_MESSAGE.Network_Mngment_info_len,    	/* data to move */
		3,                            						/* # of bytes in  iso field */
		ncpul_Move_IfThere,                 				/* conditional    */
		ncpul_vDataMove,               						/* move routine */
		ncpul_RspMovevData                					/* response routine */
	},
	{
		bf_126isstracedata,               			 		/* field */
		ncpul_HOST_MESSAGE.Iss_trace_data_len,    			/* data to move */
		3,                            						/* # of bytes in  iso field */
		ncpul_Move_IfThere,                 				/* conditional    */
		ncpul_vDataMove,               						/* move routine */
		ncpul_RspMovevData                					/* response routine */
	},
	{
		bf_127acqtracedata,                					/* field */
		ncpul_HOST_MESSAGE.Acq_trace_data_len,   	 		/* data to move */
		3,                            						/* # of bytes in  iso field */
		ncpul_Move_IfThere,                 				/* conditional    */
		ncpul_vDataMove,               						/* move routine */
		ncpul_RspMovevData                					/* response routine */
	},
	{
	bf_done,                      							/* field */
	NULL,                         							/* data to move */
	99,                          					 		/* # of bytes in  iso field */
	NULL,                         							/* conditional */
	NULL,                         							/* move routine */
	NULL                         	 						/* response routine */
	}
}; /* end BFData */






#endif
