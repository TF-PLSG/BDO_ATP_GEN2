/***************************************************************************
 *
 * Copyright (c) 2004, Hypercom, Inc.
 * All Rights Reserved.
 *
 * MODULE:      nccup_bitfields.h
 *  
 * TITLE:       CUP Network Control Bit Field Definitions
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

#include "nccup_prototypes.h"
#include "cup.h"


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
   bf_5settleamt	       = 5,	     /* Settlement Amount */
   bf_6cardbillamt         = 6,      /* Cardholder Billing Amount */
   bf_7trntime             = 7,      /* Transmission date-time */
   bf_9settconvrate        = 9,      /* Settlement Conversion Rate */
   bf_10cardbillconvrate   = 10,     /* Cardholder Billing Conversion Rate */
   bf_11stan               = 11,     /* System trace number */
   bf_12time               = 12,     /* Local Transaction time */
   bf_13date               = 13,     /* Local Transaction date */
   bf_14expiry		       = 14,	 /* Expiry date */
   bf_15datestl            = 15,     /* Settlement date */
   bf_16dateconv           = 16,     /* Conversion date */
   bf_18mertype		       = 18,	 /* Merchant type	*/
   bf_19ctrycd             = 19,     /* Acquiring institution's country code(MCC) */
   bf_22posentry           = 22,     /* Pos entry mode  */
   bf_23csn                = 23,	 /* Card Sequence Number */
   bf_25poscc              = 25,     /* Pos condition code  */
   bf_26pincode		       = 26,	 /* Point of Service PIN Capture Code */
   bf_28tranfeeamt         = 28,     /* Transaction fee amount */
   bf_32idcode             = 32,     /* Acquiring institution's ID code  */
   bf_33fwdinstidcode      = 33,     /* Forwarding institution ID code */
   bf_35track2             = 35,     /* Track 2 data */
   bf_36track3             = 36,     /* Track 3 data */
   bf_37rrn	               = 37,	 /* Retrieval ref number */
   bf_38authcode           = 38,     /* Auth identification response/code */
   bf_39respcode           = 39,     /* Response code */
   bf_41termid             = 41,     /* Terminal id  */
   bf_42cardacc            = 42,     /* Card acceptor id  */
   bf_43cardacceptloc      = 43,     /* Card acceptor name/location */
   bf_44respdata	       = 44,     /* Additional response data */
   bf_45track1             = 45,     /* Track 1 data */
   bf_48addldata           = 48,     /* Additional private data */
   bf_49trncurcd           = 49,     /* Transaction currency code */
   bf_50stlmtcurrcd        = 50,     /* Settlement currency code */
   bf_51crdcurcd           = 51,     /* Cardholder Billing Currency Code */
   bf_52pinblock           = 52,     /* Pin block */
   bf_53seccntl            = 53,     /* Recurity related control info  */
   bf_54AddlAmount         = 54,     /* Additional amount */
   bf_55IccData		       = 55,     /* IC Card data*/
   bf_57adddatapvt         = 57,	 /* Additional Data Private */
   bf_59detinqdata1        = 59,     /* Detailed Inquiry Data1 */
   bf_60selfdeffeild 	   = 60,     /* Self-Defined Field */
   bf_61cardholderauthinfo = 61,     /* Cardholder Authentication Information */
   bf_62SwitchingData      = 62,     /* Switching Data*/
   bf_70netmanginfocode    = 70,     /* Network Management Information Code */
   bf_90origdata           = 90,     /* Original Data */
   bf_96messeccode         = 96,     /* Message Security Code */
   bf_100revinsidcode      = 100,    /* Receiving Institution Identification Code */
   bf_102actid1            = 102,    /* Account Identification 1 */
   bf_104tranindappinfo    = 104,    /* Field 104 Transaction and Industry Application Information */
   bf_117PaymtFacilitator  = 117,	 /* Payment Facilitator */
   bf_121CUPSres           = 121,    /* CUPS Reserved */
   bf_122accqinstres       = 122,    /* Acquirer Institution Reserved */
   bf_123issrinstres       = 123,    /* Issuer Institution Reserved */
   bf_125dstransactionid   = 125,
   bf_128messuthcode       = 128     /* Message Authentication Code */
   
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

	  /* 0100 MESSAGES */
      {
		  /*****************************************************
		  	Abhishek
			 AUTHORIZATION REQUEST
				Pre-authorization Message request 
					:Value of field 25 is 6
				MO/TO Pre-authorization request 
					:Value of field 25 is 18
					
				Note:Only the value of field 25 is used to distinguish from
					traditional pre-authorization transaction and 
					MO/TO Pre-authorization request
		  *******************************************************/
		  AUTH_AUTHORIZATION,
		  0x30,0x31,0x30,0x30,                  /* message id, 0100 */
		  0x30,0x30,0x30,0x30,0x30,0x30,        /* processing code */
		  /**** OPTIONS ****/
		  topt1_postadv,
		  topt2_referral+topt2_reversal+topt2_floor,
		  /**** BIT FIELDS ****/
		  bf_2pan,					/* MAND */
		  bf_3proc,					/* MAND */
		  bf_4amount,				/* MAND */
		  bf_6cardbillamt,
		  bf_7trntime,				/* MAND */
		  bf_10cardbillconvrate,
		  bf_11stan,				/* MAND */
		  bf_12time,				/* MAND */ 
		  bf_13date,				/* MAND */ 
		  bf_14expiry,				/* OPTN  */
		  bf_15datestl,
		  bf_18mertype,  	        /* MAND */
		  bf_19ctrycd,				/* MAND */
		  bf_22posentry,			/* MAND */
		  bf_23csn,                 /* MAND */
		  bf_25poscc, 	          	/* MAND */
		  bf_26pincode,  	        /* COND */
		  bf_32idcode,				/* MAND */
		  bf_33fwdinstidcode,	    /* MAND */
		  bf_35track2,				/* COND */
		  bf_37rrn,					/* MAND */
		  bf_38authcode,            /*COND*/ // used for cup pre auth incremental authrorization.
		  bf_41termid,				/* MAND */
		  bf_42cardacc,				/* MAND */      
		  bf_43cardacceptloc,		/* MAND */
		  bf_45track1,		    	/* COND */
		  bf_48addldata,         	/* OPTN  */
		  bf_49trncurcd,			/* MAND */
		  bf_51crdcurcd,
		  bf_52pinblock,        	/* COND */
		  bf_53seccntl,				/* COND */
		  bf_55IccData,         	/* COND */
		  bf_60selfdeffeild,    	/* MAND */
		  bf_61cardholderauthinfo,  /* COND */
		  bf_90origdata,             /*COND*/ // used for cup pre auth incremental authrorization.
		  bf_100revinsidcode,
		  bf_104tranindappinfo,
		  bf_117PaymtFacilitator,   /*COND  */
		  bf_122accqinstres,        /* OPTN  */
		  bf_125dstransactionid,
		  bf_128messuthcode,        /* COND  */
		  bf_done			        /* end of fields to move */
      },
      
		/* 0110 MESSAGES */
      {
		  /***********************************************
		    Abhishek
			 AUTHORIZATION   RESPONSE
			
				Pre-authorization Message request 
					:Value of field 25 is 6
				MO/TO Pre-authorization request 
					:Value of field 25 is 18
					
				Note:Only the value of field 25 is used to distinguish from
					traditional pre-authorization transaction and 
					MO/TO Pre-authorization request
		  ***********************************************/
		  AUTH_AUTHORIZATION_RESPONSE,
		  0x30,0x31,0x31,0x30,                  /* message id, 0110 */
		  0x30,0x30,0x30,0x30,0x30,0x30,        /* processing code */
		  /**** OPTIONS ****/
		  topt1_postadv,
		  topt2_referral+topt2_reversal+topt2_floor,
		  /**** BIT FIELDS ****/
		  bf_2pan,				    /* MAND ECHO */
		  bf_3proc,				    /* MAND ECHO */
		  bf_4amount,			    /* MAND ECHO */
		  bf_7trntime,			    /* MAND ECHO */
		  bf_11stan,			    /* MAND ECHO */
		  bf_12time,		        /* MAND ECHO */
		  bf_13date,                /* MAND ECHO */
		  bf_14expiry,              /* MAND PREP  */
		  bf_15datestl,             /* MAND ECHO */
		  bf_18mertype,             /* MAND ECHO */
		  bf_19ctrycd,              /* MAND ECHO */
		  bf_23csn,
		  bf_25poscc,               /* MAND ECHO */
		  bf_32idcode,              /* MAND ECHO */
		  bf_33fwdinstidcode,       /* MAND ECHO */
		  bf_37rrn,				    /* MAND ECHO */
	      bf_38authcode,		    /* COND PREP  */
		  bf_39respcode,		    /* MAND PREP  */
		  bf_41termid,			    /* MAND ECHO */
		  bf_42cardacc,			    /* MAND ECHO */
		  bf_44respdata,	        /* OPTN PREP   */
		  bf_49trncurcd,		    /* MAND ECHO */
		  bf_54AddlAmount,          /* OPTN PREP   */
		  bf_55IccData,
		  bf_57adddatapvt,          /* OPTN PREP   */
		  bf_60selfdeffeild,        /* MAND ECHO */
		  bf_61cardholderauthinfo,  /* COND PREP  */
		  bf_100revinsidcode,       /* MAND ECHO */
		  bf_121CUPSres,            /* COND ECHO */
		  bf_123issrinstres,        /* OPTN PREP   */
		  bf_125dstransactionid,
		  bf_128messuthcode,        /* COND PREP  */
		  bf_done				    /* end of fields to move */
      },
      
	  {  
		/********************************************
		   B A L A N C E   I N Q U I R Y 
		********************************************/
		AUTH_BALANCE_INQUIRY,
		0x30,0x31,0x30,0x30,				/* message id */
		0x30,0x30,0x30,0x30,0x30,0x30, 		/* processing code */
		/**** OPTIONS ****/
		topt1_postadv,
		topt2_referral+topt2_reversal+topt2_floor,
		/**** BIT FIELDS ****/
		bf_2pan,					/* MAND */
	 	bf_3proc,					/* MAND */
	 	bf_7trntime,				/* MAND */
	  	bf_11stan,					/* MAND */
	 	bf_12time,					/* MAND */ 
	 	bf_13date,					/* MAND */ 
	 	bf_14expiry,				/* OPTN  */
	 	bf_15datestl,            	/* COND */
	 	bf_18mertype,  	        	/* MAND */
	 	bf_19ctrycd,				/* MAND */
	 	bf_22posentry,				/* MAND */
	 	bf_25poscc, 	          	/* MAND */
		bf_26pincode,  	        	/* COND */
		bf_32idcode,				/* MAND */
		bf_33fwdinstidcode,	   		/* MAND */
		bf_35track2,				/* COND */
		bf_37rrn,					/* MAND */
		bf_41termid,				/* MAND */
		bf_42cardacc,				/* MAND */      
		bf_43cardacceptloc,			/* MAND */
		bf_45track1,		    	/* COND */
		bf_48addldata,         		/* OPTN  */
		bf_49trncurcd,				/* OPTN  */
		bf_52pinblock,        		/* COND */
		bf_53seccntl,				/* COND */
		bf_55IccData,         		/* COND */
		bf_60selfdeffeild,    		/* MAND */
		bf_61cardholderauthinfo,	/* COND */
	    bf_100revinsidcode,       /* MAND ECHO */
		bf_104tranindappinfo,
		bf_117PaymtFacilitator,   	/*COND  */
	 	bf_122accqinstres,       	/* OPTN  */
	 	bf_128messuthcode,      	/* COND  */
	 	bf_done			        	/* end of fields to move */
		},
		
		{  
		/***************************************************
		   B A L A N C E   I N Q U I R Y   R E S P O N S E
		****************************************************/
		AUTH_BALANCE_INQUIRY_RESPONSE,
		0x30,0x31,0x31,0x30,				/* message id */
		0x30,0x30,0x30,0x30,0x30,0x30, 		/* processing code */
		/**** OPTIONS ****/
		topt1_postadv,
		topt2_referral+topt2_reversal+topt2_floor,
		/**** BIT FIELDS ****/
		bf_2pan,				    /* MAND ECHO */
		bf_3proc,				    /* MAND ECHO */
		bf_7trntime,			    /* MAND ECHO */
		bf_11stan,			    	/* MAND ECHO */
		bf_12time,		       		/* MAND ECHO */
		bf_13date,               	/* MAND ECHO */
		bf_14expiry,              	/* MAND PREP  */
		bf_15datestl,            	/* MAND ECHO */
		bf_18mertype,            	/* MAND ECHO */
		bf_19ctrycd,             	/* MAND ECHO */
		bf_25poscc,               	/* MAND ECHO */
		bf_32idcode,              	/* MAND ECHO */
		bf_33fwdinstidcode,       	/* MAND ECHO */
		bf_37rrn,				    /* MAND ECHO */
		bf_38authcode,		    	/* COND PREP  */
	    bf_39respcode,		   		/* MAND PREP  */
		bf_41termid,			    /* MAND ECHO */
		bf_42cardacc,			    /* MAND ECHO */
		bf_49trncurcd,		    	/* MAND ECHO */
		bf_54AddlAmount,
		bf_57adddatapvt,         	/* OPTN PREP   */
		bf_60selfdeffeild,        	/* MAND ECHO */
		bf_61cardholderauthinfo,  	/* COND PREP  */
		bf_100revinsidcode,       	/* MAND ECHO */
		bf_121CUPSres,            	/* COND ECHO */
		bf_122accqinstres,          /* COND ECHO */
		bf_123issrinstres,        	/* OPTN PREP   */
		bf_128messuthcode,        	/* COND PREP  */
		bf_done				    /* end of fields to move */
	},		

	   /* 0100 MESSAGES */
      {
		  /*****************************************************
		  	Abhishek
			 PRE AUTHORIZATION REQUEST
				Pre-authorization Message request 
					:Value of field 25 is 6
				MO/TO Pre-authorization request 
					:Value of field 25 is 18
					
				Note:Only the value of field 25 is used to distinguish from
					traditional pre-authorization transaction and 
					MO/TO Pre-authorization request
		  *******************************************************/
		  AUTH_PRE_AUTHORIZATION,
		  0x30,0x31,0x30,0x30,                  /* message id, 0100 */
		  0x30,0x33,0x30,0x30,0x30,0x30,        /* processing code */
		  /**** OPTIONS ****/
		  topt1_postadv,
		  topt2_referral+topt2_reversal+topt2_floor,
		  /**** BIT FIELDS ****/
		  bf_2pan,					/* MAND */
		  bf_3proc,					/* MAND */
		  bf_4amount,				/* MAND */
		  bf_7trntime,				/* MAND */
		  bf_11stan,				/* MAND */
		  bf_12time,				/* MAND */ 
		  bf_13date,				/* MAND */ 
		  bf_14expiry,				/* OPTN  */
		  bf_18mertype,  	        /* MAND */
		  bf_19ctrycd,				/* MAND */
		  bf_22posentry,			/* MAND */
		  bf_23csn,                 /* MAND */
		  bf_25poscc, 	          	/* MAND */
		  bf_26pincode,  	        /* COND */
		  bf_32idcode,				/* MAND */
		  bf_33fwdinstidcode,	    /* MAND */
		  bf_35track2,				/* COND */
		  bf_37rrn,					/* MAND */
		  bf_38authcode,            /*COND*/ // used for cup pre auth incremental authrorization.
		  bf_41termid,				/* MAND */
		  bf_42cardacc,				/* MAND */      
		  bf_43cardacceptloc,		/* MAND */
		  bf_45track1,		    	/* COND */
		  bf_48addldata,         	/* OPTN  */
		  bf_49trncurcd,			/* MAND */
		  bf_52pinblock,        	/* COND */
		  bf_53seccntl,				/* COND */
		  bf_55IccData,         	/* COND */
		  bf_60selfdeffeild,    	/* MAND */
		  bf_61cardholderauthinfo,  /* COND */
		  bf_90origdata,             /*COND*/ // used for cup pre auth incremental authrorization.
		  bf_100revinsidcode,       /* MAND */
		  bf_104tranindappinfo,
		  bf_117PaymtFacilitator,   /*COND  */
		  bf_122accqinstres,        /* OPTN  */
		  bf_128messuthcode,        /* COND  */
		  bf_done			        /* end of fields to move */
      },
      
		/* 0110 MESSAGES */
      {
		  /***********************************************
		    Abhishek
			 PRE AUTHORIZATION   RESPONSE
			
				Pre-authorization Message request 
					:Value of field 25 is 6
				MO/TO Pre-authorization request 
					:Value of field 25 is 18
					
				Note:Only the value of field 25 is used to distinguish from
					traditional pre-authorization transaction and 
					MO/TO Pre-authorization request
		  ***********************************************/
		  AUTH_PRE_AUTHORIZATION_RESPONSE,
		  0x30,0x31,0x31,0x30,                  /* message id, 0110 */
		  0x30,0x33,0x30,0x30,0x30,0x30,        /* processing code */
		  /**** OPTIONS ****/
		  topt1_postadv,
		  topt2_referral+topt2_reversal+topt2_floor,
		  /**** BIT FIELDS ****/
		  bf_2pan,				    /* MAND ECHO */
		  bf_3proc,				    /* MAND ECHO */
		  bf_4amount,			    /* MAND ECHO */
		  bf_7trntime,			    /* MAND ECHO */
		  bf_11stan,			    /* MAND ECHO */
		  bf_12time,		        /* MAND ECHO */
		  bf_13date,                /* MAND ECHO */
		  bf_14expiry,              /* MAND PREP  */
		  bf_15datestl,             /* MAND ECHO */
		  bf_18mertype,             /* MAND ECHO */
		  bf_19ctrycd,              /* MAND ECHO */
		  bf_23csn,
		  bf_25poscc,               /* MAND ECHO */
		  bf_32idcode,              /* MAND ECHO */
		  bf_33fwdinstidcode,       /* MAND ECHO */
		  bf_37rrn,				    /* MAND ECHO */
	      bf_38authcode,		    /* COND PREP  */
		  bf_39respcode,		    /* MAND PREP  */
		  bf_41termid,			    /* MAND ECHO */
		  bf_42cardacc,			    /* MAND ECHO */
		  bf_44respdata,	        /* OPTN PREP   */
		  bf_49trncurcd,		    /* MAND ECHO */
		  bf_54AddlAmount,          /* OPTN PREP   */
		  bf_55IccData,
		  bf_57adddatapvt,          /* OPTN PREP   */
		  bf_60selfdeffeild,        /* MAND ECHO */
		  bf_61cardholderauthinfo,  /* COND PREP  */
		  bf_100revinsidcode,       /* MAND ECHO */
		  bf_121CUPSres,            /* COND ECHO */
		  bf_123issrinstres,        /* OPTN PREP   */
		  bf_128messuthcode,        /* COND PREP  */
		  bf_done				    /* end of fields to move */
      },

	  /* 0110 MESSAGES */
		{
		  /***********************************************
			Abhishek
			 AUTH_CARD_VERIFICATION_RESPONSE
		  ***********************************************/
		  AUTH_CARD_VERIFICATION_RESPONSE,
		  0x30,0x31,0x31,0x30,                  /* message id, 0110 */
		  0x30,0x33,0x30,0x30,0x30,0x30,        /* processing code */
		  /**** OPTIONS ****/
		  topt1_postadv,
		  topt2_referral+topt2_reversal+topt2_floor,
		  /**** BIT FIELDS ****/
		  bf_2pan,				    /* MAND ECHO */
		  bf_3proc,				    /* MAND ECHO */
		  bf_4amount,			    /* MAND ECHO */
		  bf_7trntime,			    /* MAND ECHO */
		  bf_11stan,			    /* MAND ECHO */
		  bf_12time,		        /* MAND ECHO */
		  bf_13date,                /* MAND ECHO */
		  bf_14expiry,              /* MAND PREP  */
		  bf_15datestl,             /* MAND ECHO */
		  bf_18mertype,             /* MAND ECHO */
		  bf_19ctrycd,              /* MAND ECHO */
		  bf_23csn,
		  bf_25poscc,               /* MAND ECHO */
		  bf_32idcode,              /* MAND ECHO */
		  bf_33fwdinstidcode,       /* MAND ECHO */
		  bf_37rrn,				    /* MAND ECHO */
		  bf_38authcode,		    /* COND PREP  */
		  bf_39respcode,		    /* MAND PREP  */
		  bf_41termid,			    /* MAND ECHO */
		  bf_42cardacc,			    /* MAND ECHO */
		  bf_44respdata,	        /* OPTN PREP   */
		  bf_49trncurcd,		    /* MAND ECHO */
		  bf_54AddlAmount,          /* OPTN PREP   */
		  bf_55IccData,
		  bf_57adddatapvt,          /* OPTN PREP   */
		  bf_60selfdeffeild,        /* MAND ECHO */
		  bf_61cardholderauthinfo,  /* COND PREP  */
		  bf_100revinsidcode,       /* MAND ECHO */
		  bf_121CUPSres,            /* COND ECHO */
		  bf_123issrinstres,        /* OPTN PREP   */
		  bf_128messuthcode,        /* COND PREP  */
		  bf_done				    /* end of fields to move */
		},
  /* 0100 MESSAGES */
	 {
		 /*****************************************************
		   Abhishek
			PRE AUTHORIZATION CANCLE
			   Pre-authorization Message request 
				   :Value of field 25 is 6
			   MO/TO Pre-authorization request 
				   :Value of field 25 is 18
				   
			   Note:Only the value of field 25 is used to distinguish from
				   traditional pre-authorization transaction and 
				   MO/TO Pre-authorization request
		 *******************************************************/
		 AUTH_PRE_AUTHORIZATION_CANCLE,
		 0x30,0x31,0x30,0x30,				   /* message id, 0100 */
		 0x30,0x33,0x32,0x30,0x30,0x30, 	   /* processing code */
		 /**** OPTIONS ****/
		 topt1_postadv,
		 topt2_referral+topt2_reversal+topt2_floor,
		 /**** BIT FIELDS ****/
		 bf_2pan,				   /* MAND */
		 bf_3proc,				   /* MAND */
		 bf_4amount,			   /* MAND */
		 bf_7trntime,			   /* MAND */
		 bf_11stan, 			   /* MAND */
		 bf_12time, 			   /* MAND */ 
		 bf_13date, 			   /* MAND */ 
		 bf_14expiry,			   /* OPTN	*/
		 bf_15datestl,
		 bf_18mertype,			   /* MAND */
		 bf_19ctrycd,			   /* MAND */
		 bf_22posentry, 		   /* MAND */
		 bf_23csn,				   /* MAND */
		 bf_25poscc,			   /* MAND */
		 bf_26pincode,			   /* COND */
		 bf_32idcode,			   /* MAND */
		 bf_33fwdinstidcode,	   /* MAND */
		 bf_35track2,			   /* COND */
		 bf_37rrn,				   /* MAND */
		 bf_38authcode,			   /* MAND */
		 bf_41termid,			   /* MAND */
		 bf_42cardacc,			   /* MAND */	   
		 bf_43cardacceptloc,	   /* MAND */
		 bf_45track1,			   /* COND */
		 bf_48addldata, 		   /* OPTN	*/
		 bf_49trncurcd, 		   /* MAND */
		 bf_52pinblock, 		   /* COND */
		 bf_53seccntl,			   /* COND */
		 bf_55IccData,			   /* COND */
		 bf_60selfdeffeild, 	   /* MAND */
		 bf_61cardholderauthinfo,  /* COND */
		 bf_90origdata,
		 bf_100revinsidcode,
		 bf_104tranindappinfo,
		 bf_117PaymtFacilitator,   /*COND  */
		 bf_122accqinstres, 	   /* OPTN	*/
		 bf_128messuthcode, 	   /* COND	*/
		 bf_done				   /* end of fields to move */
	 },
	 
	   /* 0110 MESSAGES */
	 {
		 /***********************************************
		   Abhishek
			PRE AUTHORIZATION CANCLE RESPONSE
		   
			   Pre-authorization Message request 
				   :Value of field 25 is 6
			   MO/TO Pre-authorization request 
				   :Value of field 25 is 18
				   
			   Note:Only the value of field 25 is used to distinguish from
				   traditional pre-authorization transaction and 
				   MO/TO Pre-authorization request
		 ***********************************************/
		 AUTH_PRE_AUTHORIZATION_CANCLE_RESPONSE,
		 0x30,0x31,0x31,0x30,				   /* message id, 0110 */
		 0x30,0x33,0x32,0x30,0x30,0x30, 	   /* processing code */
		 /**** OPTIONS ****/
		 topt1_postadv,
		 topt2_referral+topt2_reversal+topt2_floor,
		 /**** BIT FIELDS ****/
		 bf_2pan,				   /* MAND ECHO */
		 bf_3proc,				   /* MAND ECHO */
		 bf_4amount,			   /* MAND ECHO */
		 bf_7trntime,			   /* MAND ECHO */
		 bf_11stan, 			   /* MAND ECHO */
		 bf_12time, 			   /* MAND ECHO */
		 bf_13date, 			   /* MAND ECHO */
		 bf_14expiry,			   /* MAND PREP  */
		 bf_15datestl,			   /* MAND ECHO */
		 bf_18mertype,			   /* MAND ECHO */
		 bf_19ctrycd,			   /* MAND ECHO */
		 bf_23csn,
		 bf_25poscc,			   /* MAND ECHO */
		 bf_32idcode,			   /* MAND ECHO */
		 bf_33fwdinstidcode,	   /* MAND ECHO */
		 bf_37rrn,				   /* MAND ECHO */
		 bf_38authcode, 		   /* COND PREP  */
		 bf_39respcode, 		   /* MAND PREP  */
		 bf_41termid,			   /* MAND ECHO */
		 bf_42cardacc,			   /* MAND ECHO */
		 bf_44respdata, 		   /* OPTN PREP   */
		 bf_49trncurcd, 		   /* MAND ECHO */
		 bf_54AddlAmount,		   /* OPTN PREP   */
		 bf_55IccData,
		 bf_57adddatapvt,		   /* OPTN PREP   */
		 bf_60selfdeffeild, 	   /* MAND ECHO */
		 bf_61cardholderauthinfo,  /* COND PREP  */
		 bf_100revinsidcode,	   /* MAND ECHO */
		 bf_121CUPSres, 		   /* COND ECHO */
		 bf_123issrinstres, 	   /* OPTN PREP   */
		 bf_128messuthcode, 	   /* COND PREP  */
		 bf_done				   /* end of fields to move */
	 },
			 

		  
		/* 0200 MESSAGES */
	  {
	     /********************************************
		     Abhishek
			 POS SALE REQUEST
				 Purchase
					:Value of field 25 is 00
				 MO/TO Purchase
					:Value of field 25 is 08
				Recurring
					:Value of field 25 is 28
					
			 	Note:Only the value of field 25 is used to distinguish from
					traditional pre-authorization transaction and 
					MO/TO POS   SALE request
		  ********************************************/
		  AUTH_SALE,
		  0x30,0x32,0x30,0x30,                  /* message id, 0200 */
		  0x30,0x30,0x30,0x30,0x30,0x30,        /* processing code */
		  /**** OPTIONS ****/
		  topt1_postadv,
		  topt2_referral+topt2_reversal+topt2_floor,
		  /**** BIT FIELDS ****/
			bf_2pan,                /* MAND */
			bf_3proc,               /* MAND */               
			bf_4amount,             /* MAND */             
			bf_7trntime,            /* MAND */
			bf_11stan,              /* MAND */
			bf_12time,              /* MAND */
			bf_13date,              /* MAND */
			bf_14expiry,		    /* OPTN  */
			bf_18mertype,		    /* MAND */
			bf_19ctrycd,            /* MAND */
			bf_22posentry,          /* MAND */
			bf_23csn,
			bf_25poscc,             /* MAND */
			bf_26pincode,		    /* COND */
			bf_32idcode,            /* MAND */
			bf_33fwdinstidcode,     /* MAND */
			bf_35track2,            /* COND */
			bf_37rrn,	            /* MAND */
			bf_38authcode,
			bf_41termid,            /* MAND */
			bf_42cardacc,           /* MAND */
			bf_43cardacceptloc,     /* MAND */
			bf_45track1,            /* COND */
			bf_48addldata,          /* COND */
			bf_49trncurcd,          /* MAND */
			bf_52pinblock,          /* COND */
			bf_53seccntl,           /* COND */
			bf_55IccData,
			bf_60selfdeffeild, 	    /* MAND */
			bf_61cardholderauthinfo,/* COND */
			bf_90origdata,
			bf_104tranindappinfo,
			bf_117PaymtFacilitator,   /*COND  */
			bf_122accqinstres,      /* OPTN  */
			bf_128messuthcode,      /* COND */
		    bf_done			        /* end of fields to move */
      },
      
		/* 0210 MESSAGES */
      {
		  /***********************************************
			 POS   SALE  RESPONSE
			 Abhishek
			  	Purchase response
					:Value of field 25 is 00
				 MO/TO Purchase response
					:Value of field 25 is 08
				Recurring
					:Value of field 25 is 28
					
			 	Note:Only the value of field 25 is used to distinguish from
					traditional pre-authorization transaction and 
					MO/TO POS   SALE request
		  ***********************************************/
		  AUTH_SALE_RESPONSE,
		  0x30,0x32,0x31,0x30,                  /* message id, 0210 */
		  0x30,0x30,0x34,0x30,0x30,0x30,        /* processing code */
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
		    bf_15datestl,           /* MAND ECHO */
		    bf_18mertype,		    /* MAND ECHO */
		    bf_19ctrycd,            /* MAND ECHO */
		    bf_23csn,
		    bf_25poscc,             /* MAND ECHO */
		    bf_32idcode,            /* MAND ECHO */
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
		    bf_57adddatapvt,        /* COND PREP   */
		    bf_60selfdeffeild, 	    /* MAND ECHO */
		    bf_61cardholderauthinfo,/* COND PREP  */
		    bf_100revinsidcode,     /* MAND ECHO */
		    bf_121CUPSres,          /* COND ECHO */
		    bf_123issrinstres,      /* OPTN PREP   */
		    bf_128messuthcode,      /* COND PREP  */
		    bf_done                 /* end of fields to move */
      },

	  /* 0200 MESSAGES */
		  {
			  /*****************************************************
				Abhishek
				 AUTH_VOID_SALE
					Pre-authorization Message request 
						:Value of field 25 is 6
					MO/TO Pre-authorization request 
						:Value of field 25 is 18
						
					Note:Only the value of field 25 is used to distinguish from
						traditional pre-authorization transaction and 
						MO/TO Pre-authorization request
			  *******************************************************/
			  AUTH_VOID_SALE,
			  0x30,0x32,0x30,0x30,					/* message id, 0200 */
			  0x30,0x32,0x30,0x30,0x30,0x30,		/* processing code */
			  /**** OPTIONS ****/
			  topt1_postadv,
			  topt2_referral+topt2_reversal+topt2_floor,
			  /**** BIT FIELDS ****/
			  bf_2pan,					/* MAND */
			  bf_3proc, 				/* MAND */
			  bf_4amount,				/* MAND */
			  bf_7trntime,				/* MAND */
			  bf_11stan,				/* MAND */
			  bf_12time,				/* MAND */ 
			  bf_13date,				/* MAND */ 
			  bf_14expiry,				/* OPTN  */
			  bf_15datestl,
			  bf_18mertype, 			/* MAND */
			  bf_19ctrycd,				/* MAND */
			  bf_22posentry,			/* MAND */
			  bf_23csn, 				/* MAND */
			  bf_25poscc,				/* MAND */
			  bf_26pincode, 			/* COND */
			  bf_32idcode,				/* MAND */
			  bf_33fwdinstidcode,		/* MAND */
			  bf_35track2,				/* COND */
			  bf_37rrn, 				/* MAND */
			  bf_38authcode,			/* MAND */
			  bf_41termid,				/* MAND */
			  bf_42cardacc, 			/* MAND */		
			  bf_43cardacceptloc,		/* MAND */
			  bf_45track1,				/* COND */
			  bf_48addldata,			/* OPTN  */
			  bf_49trncurcd,			/* MAND */
			  bf_52pinblock,			/* COND */
			  bf_53seccntl, 			/* COND */
			  bf_55IccData, 			/* COND */
			  bf_60selfdeffeild,		/* MAND */
			  bf_61cardholderauthinfo,	/* COND */
			  bf_62SwitchingData,
			  bf_90origdata,
			  bf_100revinsidcode,
			  bf_104tranindappinfo,
			  bf_117PaymtFacilitator,   /*COND  */
			  bf_122accqinstres,		/* OPTN  */
			  bf_128messuthcode,		/* COND  */
			  bf_done					/* end of fields to move */
		  },
		  
			/* 0210 MESSAGES */
		  {
			  /***********************************************
				Abhishek
				 AUTH_VOID_SALE RESPONSE
				
					Pre-authorization Message request 
						:Value of field 25 is 6
					MO/TO Pre-authorization request 
						:Value of field 25 is 18
						
					Note:Only the value of field 25 is used to distinguish from
						traditional pre-authorization transaction and 
						MO/TO Pre-authorization request
			  ***********************************************/
			  AUTH_VOID_SALE_RESPONSE,
			  0x30,0x32,0x31,0x30,					/* message id, 0210 */
			  0x30,0x32,0x30,0x30,0x30,0x30,		/* processing code */
			  /**** OPTIONS ****/
			  topt1_postadv,
			  topt2_referral+topt2_reversal+topt2_floor,
			  /**** BIT FIELDS ****/
			  bf_2pan,					/* MAND ECHO */
			  bf_3proc, 				/* MAND ECHO */
			  bf_4amount,				/* MAND ECHO */
			  bf_7trntime,				/* MAND ECHO */
			  bf_11stan,				/* MAND ECHO */
			  bf_12time,				/* MAND ECHO */
			  bf_13date,				/* MAND ECHO */
			  bf_14expiry,				/* MAND PREP  */
			  bf_15datestl, 			/* MAND ECHO */
			  bf_18mertype, 			/* MAND ECHO */
			  bf_19ctrycd,				/* MAND ECHO */
			  bf_23csn,
			  bf_25poscc,				/* MAND ECHO */
			  bf_32idcode,				/* MAND ECHO */
			  bf_33fwdinstidcode,		/* MAND ECHO */
			  bf_37rrn, 				/* MAND ECHO */
			  bf_38authcode,			/* COND PREP  */
			  bf_39respcode,			/* MAND PREP  */
			  bf_41termid,				/* MAND ECHO */
			  bf_42cardacc, 			/* MAND ECHO */
			  bf_44respdata,			/* OPTN PREP   */
			  bf_49trncurcd,			/* MAND ECHO */
			  bf_54AddlAmount,			/* OPTN PREP   */
			  bf_55IccData,
			  bf_57adddatapvt,			/* OPTN PREP   */
			  bf_60selfdeffeild,		/* MAND ECHO */
			  bf_61cardholderauthinfo,	/* COND PREP  */
			  bf_100revinsidcode,		/* MAND ECHO */
			  bf_121CUPSres,			/* COND ECHO */
			  bf_123issrinstres,		/* OPTN PREP   */
			  bf_128messuthcode,		/* COND PREP  */
			  bf_done					/* end of fields to move */
		  },
					  

	  {
       /********************************************
		     Abhishek
			 ATM CASH WITHDRAWAL
				 Purchase
					:Value of field 25 is 00
				 MO/TO Purchase
					:Value of field 25 is 08
				Recurring
					:Value of field 25 is 28
					
			 	Note:Only the value of field 25 is used to distinguish from
					traditional pre-authorization transaction and 
					MO/TO POS   SALE request
		  ********************************************/
		  AUTH_CASH_ADVANCE,
		  0x30,0x32,0x30,0x30,                  /* message id, 0200 */
		  0x30,0x31,0x34,0x30,0x30,0x30,        /* processing code */
		  /**** OPTIONS ****/
		  topt1_postadv,
		  topt2_referral+topt2_reversal+topt2_floor,
		  /**** BIT FIELDS ****/
			bf_2pan,                /* MAND */
			bf_3proc,               /* MAND */               
			bf_4amount,             /* MAND */
			bf_5settleamt,	        /* COND */
  			bf_6cardbillamt,        /* COND */
			bf_7trntime,            /* MAND */
			bf_9settconvrate,       /* COND */
			bf_10cardbillconvrate,  /* COND */
			bf_11stan,              /* MAND */
			bf_12time,              /* MAND */
			bf_13date,              /* MAND */
			bf_14expiry,		    /* OPTN  */
			bf_15datestl,           /* COND */
			bf_16dateconv,          /*COND*/
			bf_18mertype,		    /* MAND */
			bf_19ctrycd,            /* MAND */
			bf_22posentry,          /* MAND */
			bf_23csn,
			bf_25poscc,             /* MAND */
			bf_26pincode,		    /* COND */
			bf_28tranfeeamt, 		/* COND */
			bf_32idcode,            /* MAND */
			bf_33fwdinstidcode,     /* MAND */
			bf_35track2,            /* COND */
			bf_37rrn,	            /* MAND */
			bf_38authcode,
			bf_41termid,            /* MAND */
			bf_42cardacc,           /* MAND */
			bf_43cardacceptloc,     /* MAND */
			bf_45track1,            /* COND */
			bf_48addldata,          /* COND */
			bf_49trncurcd,          /* MAND */
			bf_50stlmtcurrcd,       /* COND */
			bf_51crdcurcd,          /* COND */
			bf_52pinblock,          /* COND */
			bf_53seccntl,           /* COND */
			bf_55IccData,
			bf_60selfdeffeild, 	    /* MAND */
			bf_61cardholderauthinfo,/* COND */
			bf_90origdata,          /*COND*/
			bf_100revinsidcode,     /*MAND */
			bf_104tranindappinfo,
			bf_117PaymtFacilitator, /*COND  */
			bf_122accqinstres,      /* OPTN  */
			bf_128messuthcode,      /* COND */
		    bf_done			        /* end of fields to move */
      },
      
		/* 0210 MESSAGES */
      {
		  /***********************************************
			 ATM CASH WITHDRAWA RESPONSE
			 Abhishek
			  	Purchase response
					:Value of field 25 is 00
				 MO/TO Purchase response
					:Value of field 25 is 08
				Recurring
					:Value of field 25 is 28
					
			 	Note:Only the value of field 25 is used to distinguish from
					traditional pre-authorization transaction and 
					MO/TO POS   SALE request
		  ***********************************************/
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
		    bf_15datestl,           /* MAND ECHO */
		    bf_18mertype,		    /* MAND ECHO */
		    bf_19ctrycd,            /* MAND ECHO */
		    bf_23csn,
		    bf_25poscc,             /* MAND ECHO */
		    bf_32idcode,            /* MAND ECHO */
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
		    bf_57adddatapvt,        /* COND PREP   */
		    bf_60selfdeffeild, 	    /* MAND ECHO */
		    bf_61cardholderauthinfo,/* COND PREP  */
		    bf_100revinsidcode,     /* MAND ECHO */
		    bf_121CUPSres,          /* COND ECHO */
		    bf_123issrinstres,      /* OPTN PREP   */
		    bf_128messuthcode,      /* COND PREP  */
		    bf_done                 /* end of fields to move */
      },

		/*0220 message type */
	  {

	  	/********************************************
			   SALE COMPLETION REQUEST
			   Abhishek

			   	Pre-authorization Message request 
					:Value of field 25 is 6
				MO/TO Pre-authorization request 
					:Value of field 25 is 18
				
				Note:Only the value of field 25 is used to distinguish from
					traditional pre-authorization transaction and 
					MO/TO Pre-authorization request
		   ********************************************/
		  AUTH_SALES_COMPLETION,
		  0x30,0x32,0x32,0x30,                  /* message id, 0220 */
		  0x30,0x30,0x34,0x30,0x30,0x30,        /* processing code */
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
		    bf_14expiry,		    /* OPTN PREP   */
		    bf_18mertype,		    /* MAND ECHO */
		    bf_19ctrycd,            /* MAND ECHO */
		    bf_22posentry,          /* MAND PREP  */
		    bf_23csn,
		    bf_25poscc,             /* MAND 06      */
		    bf_32idcode,            /* MAND ECHO */            
		    bf_33fwdinstidcode,     /* MAND ECHO */
		    bf_35track2,            /* COND PREP  */
		    bf_37rrn,	            /* MAND PREP  */
		    bf_38authcode,          /* MAND ECHO */
		    bf_41termid,            /* MAND PREP  */
		    bf_42cardacc,           /* MAND ECHO */
		    bf_43cardacceptloc,     /* MAND PREP  */
		    bf_45track1,            /* COND PREP  */
		    bf_48addldata,          /* OPTN PREP  */
		    bf_49trncurcd,          /* MAND ECHO */
		    bf_52pinblock,
		    bf_55IccData,
		    bf_60selfdeffeild, 	    /* MAND PREP  */
			bf_104tranindappinfo,
			bf_117PaymtFacilitator, /*COND  */
		    bf_122accqinstres,      /* OPTN PREP  */
		    bf_128messuthcode,      /* COND PREP  */
			bf_done                 /* end of fields to move */

	  },
	  
	  /*0230 message type */

	 {
	  	 /********************************************
			   SALE COMPLETION RESPONSE 
			   Abhishek

			   	Pre-authorization Message request 
					:Value of field 25 is 6
				MO/TO Pre-authorization request 
					:Value of field 25 is 18
				
				Note:Only the value of field 25 is used to distinguish from
					traditional pre-authorization transaction and 
					MO/TO Pre-authorization request
						
		   ********************************************/
		  AUTH_SALES_COMPLETION_RESPONSE,
		  0x30,0x32,0x33,0x30,                  /* message id, 0230 */
		  0x30,0x30,0x34,0x30,0x30,0x30,        /* processing code */
		  /**** OPTIONS ****/
		  topt1_postadv,
		  topt2_referral+topt2_reversal+topt2_floor,
		  /**** BIT FIELDS ****/
		 	bf_2pan,                /* MAND ECHO */
		    bf_3proc,               /* MAND ECHO */
		    bf_4amount,             /* MAND ECHO */
		    bf_5settleamt,	        /* COND PREP  */
			bf_6cardbillamt,        /* COND PREP  */
		    bf_7trntime,            /* MAND ECHO */
		    bf_9settconvrate,       /* COND PREP  */
			bf_10cardbillconvrate,  /* COND PREP  */
		    bf_11stan,              /* MAND ECHO */             
		    bf_12time,              /* MAND ECHO */
		    bf_13date,              /* MAND ECHO */
		    bf_15datestl,           /* MAND PREP  */
		    bf_16dateconv,          /* COND PREP  */
		    bf_18mertype,		    /* MAND ECHO */
		    bf_19ctrycd,            /* MAND ECHO */
		    bf_23csn,
		    bf_25poscc,             /* MAND ECHO */
		    bf_32idcode,            /* MAND ECHO */
		    bf_33fwdinstidcode,     /* MAND ECHO */
		    bf_37rrn,	            /* MAND ECHO */
		    bf_38authcode,          /* MAND ECHO */
		    bf_39respcode,          /* MAND PREP  */
		    bf_41termid,            /* MAND ECHO */
		    bf_42cardacc,           /* MAND ECHO */
		    bf_49trncurcd,          /* MAND ECHO */
		    bf_50stlmtcurrcd,       /* COND PREP  */ 
		    bf_51crdcurcd,          /* COND PREP  */
		    bf_55IccData,
		    bf_57adddatapvt,        /* OPTN PREP   */
		    bf_60selfdeffeild, 	    /* MAND ECHO */
		    bf_100revinsidcode,     /* MAND PREP  */
		    bf_121CUPSres,          /* OPTN PREP   */
		    bf_122accqinstres,      /* COND ECHO */
		    bf_128messuthcode,      /* COND PREP  */
		    bf_done
	  },

  
     /* 0420 MESSAGES */
	   {
		  /****************************************	
		  	Abhishek 
			 REVERSAL REQUEST

			 	 Reversal Message request (sent by the acquirer)

			 	 Reveral Message request (sent by UnionPay)

			 	 
		  *****************************************/
		  AUTH_REVERSAL_ADVICE,
		  0x30,0x34,0x32,0x30,                  /* message id, 0420 */
		  0x30,0x30,0x34,0x30,0x30,0x30,        /* processing code */
		  /**** OPTIONS ****/
		  topt1_postadv,
		  topt2_referral+topt2_reversal+topt2_floor,
		  /**** BIT FIELDS ****/
		   bf_2pan,                /* MAND ECHO */   
   		   bf_3proc,               /* MAND ECHO */   
		   bf_4amount,             /* MAND ECHO */
		   bf_5settleamt,          /* COND ECHO sent by UnionPay only */
		   bf_6cardbillamt,        /* COND ECHO sent by UnionPay only */
		   bf_7trntime,            /* MAND PREP  */
		   bf_9settconvrate,       /* COND ECHO sent by UnionPay only */   
		   bf_10cardbillconvrate,  /* COND ECHO sent by UnionPay only */
		   bf_11stan,              /* MAND PREP  */   
		   bf_12time,              /* MAND ECHO */   
		   bf_13date,              /* MAND ECHO */
		   bf_15datestl,           /* MAND ECHO sent by UnionPay only */
		   bf_16dateconv,          /* COND ECHO sent by UnionPay only */
		   bf_18mertype,		   /* MAND ECHO */  
		   bf_19ctrycd,            /* OPTN  PREP  */   
		   bf_22posentry,          /* MAND ECHO */
		   bf_23csn,
		   bf_25poscc,             /* MAND ECHO */   
		   bf_32idcode,            /* MAND ECHO */   
		   bf_33fwdinstidcode,     /* MAND ECHO */   
		   bf_37rrn,	           /* MAND ECHO */  
		   bf_38authcode,          /* COND PREP  */   
		   bf_41termid,            /* MAND ECHO */   
		   bf_42cardacc,           /* MAND ECHO */   
		   bf_43cardacceptloc,     /* MAND ECHO */ 
		   bf_44respdata,          /* COND ECHO sent by UnionPay only */
		   bf_49trncurcd,          /* MAND ECHO */
		   bf_50stlmtcurrcd,       /* COND ECHO sent by UnionPay only */
		   bf_51crdcurcd,          /* COND ECHO sent by UnionPay only */
		   bf_55IccData,
		   bf_60selfdeffeild, 	   /* MAND PREP */  
		   bf_90origdata,          /* MAND ECHO */  
		   bf_100revinsidcode,     /* MAND ECHO sent by UnionPay only */
		   bf_104tranindappinfo,
		   bf_117PaymtFacilitator, /*COND  */
		   bf_121CUPSres,          /* OPTN PREP sent by UnionPay only */
		   bf_122accqinstres,      /* OPTN PREP sent by the acquirer  */   
		   bf_128messuthcode,  	   /* COND PREP  */	
		   bf_done			       /* end of fields to move */
      },

		/* 0430 MESSAGES */
      {
		  /***********************************************
			 REVERSAL RESPONSE

			 	 Reversal Message request (sent by the acquirer)
		  
				 Reveral Message request (sent by UnionPay)
				 
		  ***********************************************/
		  AUTH_REVERSAL_ADVICE_RESPONSE,
		  0x30,0x34,0x33,0x30,                  /* message id, 1430 */
		  0x30,0x30,0x34,0x30,0x30,0x30,        /* processing code */
		  /**** OPTIONS ****/
		  topt1_postadv,
		  topt2_referral+topt2_reversal+topt2_floor,
		  /**** BIT FIELDS ****/
		  bf_2pan,                /* MAND ECHO */ 
		  bf_3proc,               /* MAND ECHO */ 
		  bf_4amount,             /* MAND ECHO */ 
		  bf_5settleamt,          /* COND ECHO sent by the acquirer only */ 
		  bf_7trntime,            /* MAND ECHO */ 
	      bf_11stan,              /* MAND ECHO */ 
		  bf_12time,              /* MAND ECHO */ 
		  bf_13date,              /* MAND ECHO */ 
		  bf_15datestl,           /* MAND ECHO */ 
		  bf_16dateconv,          /* COND ECHO sent by the acquirer only */ 
		  bf_18mertype,		      /* MAND ECHO */ 
		  bf_19ctrycd,            /* COND ECHO */
		  bf_23csn,
		  bf_25poscc,             /* MAND ECHO */ 
		  bf_32idcode,            /* MAND ECHO */ 
		  bf_33fwdinstidcode,     /* MAND ECHO */ 
		  bf_37rrn,	              /* MAND ECHO */
		  bf_39respcode,          /* MAND PREP  */ 
		  bf_41termid,            /* MAND ECHO */ 
		  bf_42cardacc,           /* MAND ECHO */ 
		  bf_49trncurcd,          /* MAND ECHO */ 
		  bf_50stlmtcurrcd,       /* COND ECHO sent by the acquirer only */ 
		  bf_55IccData,
		  bf_60selfdeffeild, 	  /* MAND ECHO */ 
		  /*
		    bf_100revinsidcode  MAND PREP sent by the acquirer 
		    bf_100revinsidcode  MAND ECHO sent by UnionPay
		  */
		  bf_100revinsidcode,     
		  bf_121CUPSres,          /* OPTN PREP  */ 
		  bf_122accqinstres,      /* COND ECHO sent by the acquirer only */ 
		  bf_123issrinstres,      /* OPTN PREP  sent by UnionPay */
		  bf_128messuthcode,      /* COND PREP  */ 
		  bf_done				  /* end of fields to move */
      },

		  /* 0420 MESSAGES */
	   {
		  /****************************************	
		  	Abhishek 
			 REVERSAL REQUEST

			 	 Reversal Message request (sent by the acquirer)

			 	 Reveral Message request (sent by UnionPay)

			 	 
		  *****************************************/
		  AUTH_REVERSAL,
		  0x30,0x34,0x32,0x30,                  /* message id, 0420 */
		  0x30,0x30,0x34,0x30,0x30,0x30,        /* processing code */
		  /**** OPTIONS ****/
		  topt1_postadv,
		  topt2_referral+topt2_reversal+topt2_floor,
		  /**** BIT FIELDS ****/
		   bf_2pan,                /* MAND ECHO */   
   		   bf_3proc,               /* MAND ECHO */   
		   bf_4amount,             /* MAND ECHO */
		   bf_5settleamt,          /* COND ECHO sent by UnionPay only */
		   bf_6cardbillamt,        /* COND ECHO sent by UnionPay only */
		   bf_7trntime,            /* MAND PREP  */
		   bf_9settconvrate,       /* COND ECHO sent by UnionPay only */   
		   bf_10cardbillconvrate,  /* COND ECHO sent by UnionPay only */
		   bf_11stan,              /* MAND PREP  */   
		   bf_12time,              /* MAND ECHO */   
		   bf_13date,              /* MAND ECHO */
		   bf_15datestl,           /* MAND ECHO sent by UnionPay only */
		   bf_16dateconv,          /* COND ECHO sent by UnionPay only */
		   bf_18mertype,		   /* MAND ECHO */  
		   bf_19ctrycd,            /* OPTN  PREP  */   
		   bf_22posentry,          /* MAND ECHO */
		   bf_23csn,
		   bf_25poscc,             /* MAND ECHO */   
		   bf_32idcode,            /* MAND ECHO */   
		   bf_33fwdinstidcode,     /* MAND ECHO */   
		   bf_37rrn,	           /* MAND ECHO */  
		   bf_38authcode,          /* COND PREP  */   
		   bf_41termid,            /* MAND ECHO */   
		   bf_42cardacc,           /* MAND ECHO */   
		   bf_43cardacceptloc,     /* MAND ECHO */ 
		   bf_44respdata,          /* COND ECHO sent by UnionPay only */
		   bf_49trncurcd,          /* MAND ECHO */
		   bf_50stlmtcurrcd,       /* COND ECHO sent by UnionPay only */
		   bf_51crdcurcd,          /* COND ECHO sent by UnionPay only */
		   bf_55IccData,
		   bf_60selfdeffeild, 	   /* MAND PREP */  
		   bf_90origdata,          /* MAND ECHO */  
		   bf_100revinsidcode,     /* MAND ECHO sent by UnionPay only */
		   bf_104tranindappinfo,
		   bf_117PaymtFacilitator, /*COND  */
		   bf_121CUPSres,          /* OPTN PREP sent by UnionPay only */
		   bf_122accqinstres,      /* OPTN PREP sent by the acquirer  */   
		   bf_128messuthcode,  	   /* COND PREP  */	
		   bf_done			       /* end of fields to move */
      },

		/* 0430 MESSAGES */
      {
		  /***********************************************
			 REVERSAL RESPONSE

			 	 Reversal Message request (sent by the acquirer)
		  
				 Reveral Message request (sent by UnionPay)
				 
		  ***********************************************/
		  AUTH_REVERSAL_RESPONSE,
		  0x30,0x34,0x33,0x30,                  /* message id, 0430 */
		  0x30,0x30,0x34,0x30,0x30,0x30,        /* processing code */
		  /**** OPTIONS ****/
		  topt1_postadv,
		  topt2_referral+topt2_reversal+topt2_floor,
		  /**** BIT FIELDS ****/
		  bf_2pan,                /* MAND ECHO */ 
		  bf_3proc,               /* MAND ECHO */ 
		  bf_4amount,             /* MAND ECHO */ 
		  bf_5settleamt,          /* COND ECHO sent by the acquirer only */ 
		  bf_7trntime,            /* MAND ECHO */ 
	      bf_11stan,              /* MAND ECHO */ 
		  bf_12time,              /* MAND ECHO */ 
		  bf_13date,              /* MAND ECHO */ 
		  bf_15datestl,           /* MAND ECHO */ 
		  bf_16dateconv,          /* COND ECHO sent by the acquirer only */ 
		  bf_18mertype,		      /* MAND ECHO */ 
		  bf_19ctrycd,            /* COND ECHO */
		  bf_23csn,
		  bf_25poscc,             /* MAND ECHO */ 
		  bf_32idcode,            /* MAND ECHO */ 
		  bf_33fwdinstidcode,     /* MAND ECHO */ 
		  bf_37rrn,	              /* MAND ECHO */ 
		  bf_39respcode,          /* MAND PREP  */ 
		  bf_41termid,            /* MAND ECHO */ 
		  bf_42cardacc,           /* MAND ECHO */ 
		  bf_49trncurcd,          /* MAND ECHO */ 
		  bf_50stlmtcurrcd,       /* COND ECHO sent by the acquirer only */ 
		  bf_55IccData,
		  bf_60selfdeffeild, 	  /* MAND ECHO */ 
		  /*
		    bf_100revinsidcode  MAND PREP sent by the acquirer 
		    bf_100revinsidcode  MAND ECHO sent by UnionPay
		  */
		  bf_100revinsidcode,     
		  bf_121CUPSres,          /* OPTN PREP  */ 
		  bf_122accqinstres,      /* COND ECHO sent by the acquirer only */ 
		  bf_123issrinstres,      /* OPTN PREP  sent by UnionPay */
		  bf_128messuthcode,      /* COND PREP  */ 
		  bf_done				  /* end of fields to move */
      },
     	  
	  /* 0820 MESSAGES */
      {
		  /********************************************
			 L O G O N
		  ********************************************/
		  AUTH_LOGON,
		  0x30,0x38,0x32,0x30,                  /* message id, 0820 */
		  0x30,0x30,0x30,0x30,0x30,0x30,        /* processing code */
		  /**** OPTIONS ****/
		  topt1_postadv,
		  topt2_referral+topt2_reversal+topt2_floor,
		  /**** BIT FIELDS ****/
		  bf_7trntime,			 /* MAND */
		  bf_11stan,			 /* MAND */
		  bf_15datestl,          /* MAND */
		  bf_32idcode,			 /* MAND */ 
		  bf_33fwdinstidcode,    /* MAND */
		  bf_53seccntl,          /* MAND */
		  bf_70netmanginfocode,  /* MAND */
		  bf_100revinsidcode,    /* MAND */
		  bf_done			/* end of fields to move */
      },

	  /* 0830 MESSAGES */
      {
		  /***********************************************
			 L O G O N  R E S P O N S E
		  ***********************************************/
		  AUTH_LOGON_RESPONSE,
		  0x30,0x38,0x33,0x30,                  /* message id, 0830 */
		  0x30,0x30,0x30,0x30,0x30,0x30,        /* processing code */
		  /**** OPTIONS ****/
		  topt1_postadv,
		  topt2_referral+topt2_reversal+topt2_floor,
		  /**** BIT FIELDS ****/
		  bf_7trntime,		     /* MAND ECHO  */
		  bf_11stan,		 	 /* MAND ECHO  */
		  bf_15datestl,          /* MAND */
		  bf_32idcode,			 /* MAND ECHO  */ 
		  bf_33fwdinstidcode,    /* MAND ECHO  */
		  bf_39respcode,         /* MAND */
  		  bf_53seccntl,          /* MAND */
		  bf_70netmanginfocode,  /* MAND ECHO*/
		  bf_100revinsidcode,    /* MAND ECHO */
		  bf_done				 /* end of fields to move */
      },

	  /* 0800 Key Reset Request Message*/
	  {
		  /********************************************
			 KEY RESET
		  ********************************************/
		  AUTH_PIN_CHANGE,
		  0x30,0x38,0x30,0x30,                  /* message id, 0800 */
		  0x30,0x30,0x30,0x30,0x30,0x30,        /* processing code */
		  /**** OPTIONS ****/
		  topt1_postadv,
		  topt2_referral+topt2_reversal+topt2_floor,
		  /**** BIT FIELDS ****/
		  bf_7trntime,			 /* MAND */
		  bf_11stan,			 /* MAND */
		  bf_48addldata,         /* COND */
		  bf_53seccntl,          /* MAND */
		  bf_70netmanginfocode,  /* MAND */
		  bf_96messeccode,       /* MAND */
		  bf_100revinsidcode,    /* MAND */
		  bf_128messuthcode,     /* COND */
		  bf_done			/* end of fields to move */
      },


	  /* 0810 Key Reset Response Message*/
	  {
		  /********************************************
			KEY RESET REPONSE
		  ********************************************/
		  AUTH_PIN_CHANGE_RESPONSE,
		  0x30,0x38,0x31,0x30,                  /* message id, 0810 */
		  0x30,0x30,0x30,0x30,0x30,0x30,        /* processing code */
		  /**** OPTIONS ****/
		  topt1_postadv,
		  topt2_referral+topt2_reversal+topt2_floor,
		  /**** BIT FIELDS ****/
		  bf_7trntime,			 /* MAND ECHO */
		  bf_11stan,			 /* MAND ECHO */
		  bf_39respcode,         /* MAND */
		  bf_53seccntl,          /* MAND ECHO */
		  bf_70netmanginfocode,  /* MAND ECHO */
		  bf_100revinsidcode,    /* MAND ECHO */
		  //bf_128messuthcode,     /* COND */
		  bf_done			/* end of fields to move */
      }

 };

#define N_TRANENTRY (sizeof(TRANTABLE) / sizeof(struct trandef))

#define N_BFData ( sizeof(BFData) / sizeof (struct bitfield_data) )

struct bitfield_data *Find_Bitfield_Table (enum bitfields);
struct trandef       *Find_Tran_Table     (pAUTH_TX p_auth_tx);

struct iso_msg_rec
{
   BYTE hdr_len       [1];
   BYTE hdr_flag_ver  [1];
   BYTE tot_msg_len   [4];
   BYTE dest_stat_id  [11];
   BYTE source_id     [11];
   BYTE res_for_use   [3];
   BYTE batch_no      [1];
   BYTE tran_info     [8];
   BYTE user_info     [1];
   BYTE rej_code      [5];
   BYTE msgtype       [4];
   BYTE bitmap1_bin   [16];
   BYTE bitmap2_bin   [16];
   BYTE pan_len       [2];
   BYTE pan           [19];
   BYTE proccode      [6];
   BYTE amount        [12];
   BYTE setl_amount   [12];
   BYTE ch_bill_amt   [12];
   BYTE gmt_time      [10];
   BYTE conv_rate_stl [8];
   BYTE setl_conv_rate[8];
   BYTE conv_rate_chb [8];
   BYTE stan          [6];
   BYTE time          [6];
   BYTE date		  [4];
   BYTE date_expd	  [4];
   BYTE date_settle   [4];
   BYTE conv_date     [4];
   BYTE merch_type	  [4];
   BYTE ctry_code	  [3];
   BYTE posentry      [3];
   BYTE card_seq_num  [3];
   BYTE pos_condcd    [4];
   BYTE pos_pincode   [2];
   BYTE tran_fee_amt  [9];
   BYTE acq_inst_len  [2];
   BYTE acq_inst_id   [11];
   BYTE fwd_inst_len  [2];
   BYTE fwd_inst_id   [11];
   BYTE track2_len    [2];
   BYTE track2        [37];
   BYTE track3_len    [3];
   BYTE track3        [104];
   BYTE rrn			  [12];
   BYTE authcode      [6];
   BYTE responsecd    [2];
   BYTE termid        [8];
   BYTE cardaccid     [15];
   BYTE cardaccname [40];
   BYTE addl_rsp_len  [2];
   BYTE addlrspdata   [25];
   BYTE track1_len    [2];
   BYTE track1        [77];
   BYTE addl_data_len [3];
   BYTE addl_data     [70];
   BYTE tran_curr_cd  [3];
   BYTE settle_cur_cd  [3];
   BYTE card_curr_cd  [3];
   BYTE pinblock      [8];
   BYTE sec_cntl      [16];
   BYTE AddlAmts_len   [3];
   BYTE AddlAmts     [40];
   BYTE IccData_len   [3];
   BYTE IccData	    [264];
   BYTE add_data_pvt_len [3];
   BYTE add_data_pvt [100];	
   BYTE det_inq_data1_len [3];
   BYTE det_inq_data1 [600];
   BYTE self_def_feild_len [3];
   BYTE self_def_feild [100];
   BYTE card_holder_auth_info_len [3];
   BYTE card_holder_auth_info [200];
   BYTE switching_data_len[3];
   BYTE switching_data[200];
   BYTE net_manag_info_code [3];
   BYTE orig_data [42];
   BYTE mess_sec_code[8];
   BYTE recv_ins_id_code_len[2];
   BYTE recv_ins_id_code[11];
   BYTE acc_id_len[2];
   BYTE acc_id[28];
   BYTE tranindappinfo_data_len[3];
   BYTE tranindappinfo_data[999];
   BYTE cup_payFacl_len[2];
   BYTE cup_payFacl[15];
   BYTE cup_resv_len[3];
   BYTE cup_resv[100];
   BYTE accq_inst_resv_len[3];
   BYTE accq_inst_resv[100];
   BYTE issu_inst_resv_len [3];
   BYTE issu_inst_resv [100];
   BYTE ds_transactionID_len[3];
   BYTE ds_transactionID[257];
   BYTE mess_auth_code [8];
};
struct iso_msg_rec HOST_MESSAGE;


/*typedef struct
{
	BYTE av_verification_rc			        [4];
	BYTE authentication_attempt_rc          [4];
	BYTE cvn2_validation_flag				[2];
	BYTE av_validation_flag				    [2];
}CUP3DS2, *pCUP3DS2;*/

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
void vBCDMove(enum bitfields fieldid);
void RspMovevBCD (enum bitfields fieldid);
void vDataMove(enum bitfields fieldid);
void RspMovevData (enum bitfields fieldid);




static  struct bitfield_data BFData[] = {
   {
   bf_2pan,                      /* Primary Account Number  */
   HOST_MESSAGE.pan_len,         /* data to move */
   2,                            /* # of bytes in pan len field */
   Move_IfThere,                  /* mandatory */
   vDataMove,                     /* move routine */
   RspMovevData                   /* response routine */
   },
   {
   bf_3proc,                     /* field */
   HOST_MESSAGE.proccode,        /* data to move */
   6,                            /* # of bytes in  iso field */
   Move_Always,                  /* mandatory */
   GenericMove,                   /* move routine */
   RspMoveGeneric                 /* response routine */
   },
   {
   bf_4amount,                   /* field */
   HOST_MESSAGE.amount,          /* data to move */
   12,                           /* # of bytes in  iso field */
   Move_Always,                  /* mandatory */
   GenericMove,                   /* move routine */
   RspMoveGeneric                 /* response routine */
   },
   {
   bf_5settleamt,                /* field */
   HOST_MESSAGE.setl_amount,     /* data to move */
   12,                           /* # of bytes in  iso field */
   Move_IfThere,                  /* mandatory */
   GenericMove,                   /* move routine */
   RspMoveGeneric                 /* response routine */
   },
   {
   bf_6cardbillamt,                /* field */
   HOST_MESSAGE.ch_bill_amt,     /* data to move */
   12,                           /* # of bytes in  iso field */
   Move_IfThere,                  /* mandatory */
   GenericMove,                   /* move routine */
   RspMoveGeneric                 /* response routine */
   },
   {
   bf_7trntime,                  /* field */
   HOST_MESSAGE.gmt_time,        /* data to move : MMDDhhmmss*/
   10,                           /* # of bytes in  iso field */
   Move_IfThere,                  /* conditional */
   GenericMove,                   /* move routine */
   RspMoveGeneric                 /* response routine */
   },
   {
   bf_9settconvrate,             /* field */
   HOST_MESSAGE.setl_conv_rate,  /* data to move */
   8,                            /* # of bytes in  iso field */
   Move_IfThere,                  /* conditional */
   GenericMove,                   /* move routine */
   RspMoveGeneric                 /* response routine */
   },
   {
   bf_10cardbillconvrate,             /* field */
   HOST_MESSAGE.conv_rate_chb,  /* data to move */
   8,                            /* # of bytes in  iso field */
   Move_IfThere,                  /* conditional */
   GenericMove,                   /* move routine */
   RspMoveGeneric                 /* response routine */
   },
   {
   bf_11stan,                    /* field */
   HOST_MESSAGE.stan,            /* data to move */
   6,                            /* # of bytes in  iso field */
   Move_Always,                  /* conditional */
   GenericMove,                   /* move routine */
   RspMoveGeneric                 /* response routine */
   },
   {
   bf_12time,                    /* field */
   HOST_MESSAGE.time,            /* data to move */
   6,                            /* # of bytes in  iso field */
   Move_IfThere,                  /* conditional */
   GenericMove,                   /* move routine */
   RspMoveGeneric                 /* response routine */
   },
   {
   bf_13date,                    /* field */
   HOST_MESSAGE.date,            /* effective data to move */
   4,                            /* # of bytes in  iso field */
   Move_IfThere,                  /* conditional */
   GenericMove,                   /* move routine */
   RspMoveGeneric                 /* response routine */
   },
   {
   bf_14expiry,                  /* field */
   HOST_MESSAGE.date_expd,       /* data to move */
   4,                            /* # of bytes in field */
   Move_IfThere,                     /* conditional */
   GenericMove,                   /* move routine */
   RspMoveGeneric                 /* response routine */
   },
   {
   bf_15datestl,                 /* field */
   HOST_MESSAGE.date_settle,     /* data to move */
   4,                            /* # of bytes in  iso field */
   Move_IfThere,                 /* conditional */
   GenericMove,                   /* move routine */
   RspMoveGeneric                 /* response routine */
   },
   {
   bf_16dateconv,                /* field */
   HOST_MESSAGE.conv_date,       /* data to move */
   4,                            /* # of bytes in  iso field */
   Move_IfThere,                  /* conditional */
   GenericMove,                   /* move routine */
   RspMoveGeneric                 /* response routine */
   },
   {
   bf_18mertype,                 /* field */
   HOST_MESSAGE.merch_type,            /* data to move */
   4,                            /* # of bytes in  iso field */
   Move_IfThere,                  /* conditional */
   GenericMove,                   /* move routine */
   RspMoveGeneric                 /* response routine */
   },
   {
   bf_19ctrycd,                  /* field */
   HOST_MESSAGE.ctry_code,       /* data to move */
   3,                            /* # of bytes in field */
   Move_IfThere,                  /* mandatory    */
   GenericMove,                   /* move routine */
   RspMoveGeneric                 /* response routine */
   },
   {
   bf_22posentry,                /* field */
   HOST_MESSAGE.posentry,        /* data to move */
   3,                            /* # of bytes in  iso field */
   Move_IfThere,                  /* conditional */
   GenericMove,                   /* move routine */
   RspMoveGeneric                 /* response routine */
   },
   {
   bf_23csn,                  /* field */
   HOST_MESSAGE.card_seq_num,         /* data to move */
   3,                            /* # of bytes in field */
   Move_IfThere,                 /* conditional */
   GenericMove,                   /* move routine */
   RspMoveGeneric                 /* response routine */
   },
   {
   bf_25poscc,                   /* field */
   HOST_MESSAGE.pos_condcd,      /* data to move */
   2,                            /* # of bytes in  iso field */
   Move_IfThere,                  /* conditional */
   GenericMove,                   /* move routine */
   RspMoveGeneric                 /* response routine */
   },
    {
   bf_26pincode,                 /* field */
   HOST_MESSAGE.pos_pincode,    /* data to move */
   2,                            /* # of bytes in field */
   Move_IfThere,                  /* conditional */
   GenericMove,                   /* move routine */
   RspMoveGeneric                 /* response routine */
   },
   {
   bf_28tranfeeamt,              /* field */
   HOST_MESSAGE.tran_fee_amt,      /* data to move */
   9,                            /* # of bytes in  iso field */
   Move_IfThere,                  /* conditional */
   GenericMove,                   /* move routine */
   RspMoveGeneric                 /* response routine */
   },
   {
   bf_32idcode,                  /* field */
   HOST_MESSAGE.acq_inst_len,    /* data to move */
   2,                            /* # of bytes in  iso field */
   Move_IfThere,                  /* conditional */
   vDataMove,                  /* move routine */
   RspMovevData                /* response routine */
   },
   {
   bf_33fwdinstidcode,                  /* field */
   HOST_MESSAGE.fwd_inst_len,    /* data to move */
   2,                            /* # of bytes in  iso field */
   Move_IfThere,                  /* conditional */
   vDataMove,                  /* move routine */
   RspMovevData                /* response routine */
   },
   {
   bf_35track2,                  /* field */
   HOST_MESSAGE.track2_len,      /* data to move */
   2,                            /* # of bytes in track2 len field */
   Move_IfThere,                  /* conditional */
   vDataMove,                  /* move routine */
   RspMovevData                /* response routine */
   },
   {
   bf_36track3,                  /* field */
   HOST_MESSAGE.track3_len,      /* data to move */
   3,                            /* # of bytes in track2 len field */
   Move_IfThere,                  /* conditional */
   vDataMove,                  /* move routine */
   RspMovevData                /* response routine */
   },
   {
   bf_37rrn,                  /* field */
   HOST_MESSAGE.rrn,      /* data to move */
   12,                            /* # of bytes in track2 len field */
   Move_IfThere,                  /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_38authcode,                /* field */
   HOST_MESSAGE.authcode,        /* data to move */
   6,                            /* # of bytes in  iso field */
   Move_IfThere,                 /* conditional */
   GenericMove,                   /* move routine */
   RspMoveGeneric                 /* response routine */
   },
   {
   bf_39respcode,                /* field */
   HOST_MESSAGE.responsecd,      /* data to move */
   2,                            /* # of bytes in  iso field */
   Move_IfThere,                 /* conditional */
   GenericMove,                   /* move routine */
   RspMoveGeneric                 /* response routine */
   },
   {
   bf_41termid,                  /* field */
   HOST_MESSAGE.termid,          /* data to move */
   8,                            /* # of bytes in  iso field */
   Move_IfThere,                 /* conditional */
   GenericMove,                   /* move routine */
   RspMoveGeneric                 /* response routine */
   },
   {
   bf_42cardacc,                 /* field */
   HOST_MESSAGE.cardaccid,       /* data to move */
   15,                           /* # of bytes in  iso field */
   Move_IfThere,                  /* conditional */
   GenericMove,                   /* move routine */
   RspMoveGeneric                 /* response routine */
   },
   {
   bf_43cardacceptloc,                 /* field */
   HOST_MESSAGE.cardaccname,       /* data to move */
   40,                           /* # of bytes in  iso field */
   Move_IfThere,                  /* conditional */
   GenericMove,                   /* move routine */
   RspMoveGeneric                 /* response routine */
   },
   {
   bf_44respdata,                 /* field */
   HOST_MESSAGE.addl_rsp_len,       /* data to move */
   2,                           /* # of bytes in  iso field */
   Move_IfThere,                  /* conditional */
   vDataMove,                   /* move routine */
   RspMovevData					/* response routine */
   },
   {
   bf_45track1,                  /* field */
   HOST_MESSAGE.track1_len,      /* data to move */
   2,                            /* # of bytes in len field */
   Move_IfThere,                   /* conditional */
   vDataMove,                  /* move routine */
   RspMovevData                /* response routine */
   },
   {
   /*Need to write function to read binary data*/
   bf_48addldata,                /* field */
   HOST_MESSAGE.addl_data_len,   /* data to move */
   3,                            /* # of bytes in track1 len field */
   Move_IfThere,                 /* conditional */
   vDataMove,                  /* move routine */
   RspMovevData                /* response routine */
   },
   {
   bf_49trncurcd,               /* field */
   HOST_MESSAGE.tran_curr_cd,   /* data to move */
   3,                           /* # of bytes in  iso field */
   Move_IfThere,                /* mandatory    */
   GenericMove,                 /* move routine */
   RspMoveGeneric				/* response routine */
   },
   {
   bf_50stlmtcurrcd,               /* field */
   HOST_MESSAGE.settle_cur_cd,    /* data to move */
   3,                            /* # of bytes in field */
   Move_IfThere,                 /* mandatory    */
   GenericMove,                   /* move routine */
   RspMoveGeneric                 /* response routine */
   },
   {
   bf_51crdcurcd,               /* field */
   HOST_MESSAGE.card_curr_cd,    /* data to move */
   3,                            /* # of bytes in field */
   Move_IfThere,                 /* mandatory    */
   GenericMove,                   /* move routine */
   RspMoveGeneric                 /* response routine */
   },
   {
   bf_52pinblock,                /* field */
   HOST_MESSAGE.pinblock,        /* data to move */
   8,                            /* # of bytes in  iso field */
   Move_IfThere,                  /* mandatory */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_53seccntl,                 /* field */
   HOST_MESSAGE.sec_cntl,    /* data to move */
   16,                            /* # of bytes in field */
   Move_IfThere,                   /* mandatory    */
   GenericMove,                   /* move routine */
   RspMoveGeneric                 /* response routine */
   },
   {
   bf_54AddlAmount,                 /* field */
   HOST_MESSAGE.AddlAmts_len,   /* data to move */
   3,                            /* # of bytes in field */
   Move_IfThere,                  /* mandatory    */
   vDataMove,                  /* move routine */
   RspMovevData                /* response routine */
   },
   {
   bf_55IccData,                 /* field */
   HOST_MESSAGE.IccData_len,   /* data to move */
   3,                            /* # of bytes in field */
   Move_IfThere,                  /* mandatory    */
   vDataMove,                  /* move routine */
   RspMovevData                /* response routine */
   },
   {
   bf_57adddatapvt,                 /* field */
   HOST_MESSAGE.add_data_pvt_len,   /* data to move */
   3,                            /* # of bytes in field */
   Move_IfThere,                  /* mandatory    */
   vDataMove,                  /* move routine */
   RspMovevData                /* response routine */
   },
   {
   bf_59detinqdata1,                 /* field */
   HOST_MESSAGE.det_inq_data1_len,   /* data to move */
   3,                            /* # of bytes in field */
   Move_IfThere,                  /* mandatory    */
   vDataMove,                  /* move routine */
   RspMovevData                /* response routine */
   },
   {   
   bf_60selfdeffeild,                 /* field */   
   HOST_MESSAGE.self_def_feild_len,     /* data to move */  
   3,                           /* # of bytes in field */   
   Move_IfThere,                /* mandatory    */   
   vDataMove,                  /* move routine */   
   RspMovevData                /* response routine */   
   },
   {   
   bf_61cardholderauthinfo,                 /* field */   
   HOST_MESSAGE.card_holder_auth_info_len,     /* data to move */  
   3,                           /* # of bytes in field */   
   Move_IfThere,                /* mandatory    */   
   vDataMove,                  /* move routine */   
   RspMovevData                /* response routine */   
   },
   {
   bf_62SwitchingData,                 /* field */   
   HOST_MESSAGE.switching_data_len,     /* data to move */  
   3,                           /* # of bytes in field */   
   Move_IfThere,                /* mandatory    */   
   vDataMove,                  /* move routine */   
   RspMovevData                /* response routine */   
   },
   {
   bf_70netmanginfocode,                /* field */
   HOST_MESSAGE.net_manag_info_code,    /* data to move */
   3,                            		/* # of bytes in  iso field */
   Move_IfThere,                 		/* mandatory    */
   GenericMove,               		/* move routine */
   RspMoveGeneric               		/* response routine */
   },
   {
   bf_90origdata,                /* field */
   HOST_MESSAGE.orig_data,    /* data to move */
   42,                            		/* # of bytes in  iso field */
   Move_IfThere,                 		/* mandatory    */
   GenericMove,               		/* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_96messeccode,                /* field */
   HOST_MESSAGE.mess_sec_code,    /* data to move */
   8,                            		/* # of bytes in  iso field */
   Move_Always,                 		/* mandatory    */
   GenericMove,               		/* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_100revinsidcode,                /* field */
   HOST_MESSAGE.recv_ins_id_code_len,    /* data to move */
   2,                            		/* # of bytes in  iso field */
   Move_IfThere,                 		/* mandatory    */
   vDataMove,               		/* move routine */
   RspMovevData                /* response routine */
   },
   {
   bf_102actid1,                /* field */
   HOST_MESSAGE.acc_id_len,    /* data to move */
   2,                            		/* # of bytes in  iso field */
   Move_IfThere,                 		/* mandatory    */
   vDataMove,               		/* move routine */
   RspMovevData                /* response routine */
   },
   {
   bf_104tranindappinfo,                /* field */
   HOST_MESSAGE.tranindappinfo_data_len,   /* data to move */
   3,                            /* # of bytes in track1 len field */
   Move_IfThere,                 /* conditional */
   vDataMove,                  /* move routine */
   RspMovevData                /* response routine */
   },
   {
   bf_117PaymtFacilitator,              		 	/* field */
   HOST_MESSAGE.cup_payFacl_len,       	 			/* data to move */
   2,                           		 			/* # of bytes in  iso field */
   Move_IfThere,                 		 			/* conditional */
   vDataMove,                   		 			/* move routine */
   RspMovevData							 			/* response routine */
   },
   {
   bf_121CUPSres,                /* field */
   HOST_MESSAGE.cup_resv_len,    /* data to move */
   3,                            		/* # of bytes in  iso field */
   Move_IfThere,                 		/* mandatory    */
   vDataMove,               		/* move routine */
   RspMovevData                /* response routine */
   },
   {
   bf_122accqinstres,                /* field */
   HOST_MESSAGE.accq_inst_resv_len,    /* data to move */
   3,                            		/* # of bytes in  iso field */
   Move_IfThere,                 		/* mandatory    */
   vDataMove,               		/* move routine */
   RspMovevData                /* response routine */
   },
   {
   bf_123issrinstres,                /* field */
   HOST_MESSAGE.issu_inst_resv_len,    /* data to move */
   3,                            		/* # of bytes in  iso field */
   Move_IfThere,                 		/* mandatory    */
   vDataMove,               		/* move routine */
   RspMovevData                /* response routine */
   },
   {
    bf_125dstransactionid,                 /* field */
	HOST_MESSAGE.ds_transactionID_len,     /* data to move */
    3,                           /* # of bytes in field */
    Move_IfThere,                /* mandatory    */
	vDataMove,                  /* move routine */
	RspMovevData                /* response routine */
   },
   {
   bf_128messuthcode,                /* field */
   HOST_MESSAGE.mess_auth_code ,    /* data to move */
   8,                            		/* # of bytes in  iso field */
   Move_IfThere,                 		/* mandatory    */
   GenericMove,               		/* move routine */
   RspMoveGeneric                /* response routine */
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
