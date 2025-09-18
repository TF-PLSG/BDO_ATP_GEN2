/***************************************************************************
 *
 * Copyright (c) 2004, Hypercom, Inc.
 * All Rights Reserved.
 *
 * MODULE:      nciftr_bitfields.h
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

#include "nciftr_prototypes.h"
#include "nciftr_constants.h"


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
   bf_7trntime             = 7,      /* Transmission date-time */
   bf_11stan               = 11,     /* System trace number */
   bf_12time               = 12,     /* Local Transaction time */
   bf_13date               = 13,     /* Local Transaction date */
   bf_14expiry		       = 14,	 /* Expiry date */
   bf_15datestl            = 15,
   bf_18sicnumber          = 18,
   bf_22posentry           = 22,     /* Pos entry mode  */
   bf_23panseqnum          = 23,
   bf_24nii                = 24,	 /* Card Sequence Number */
   bf_25poscc              = 25,     /* Pos condition code  */
   bf_35track2             = 35,     /* Track 2 data */
   bf_37rrn	               = 37,	 /* Retrieval ref number */
   bf_38authcode           = 38,     /* Auth identification response/code */
   bf_39respcode           = 39,     /* Response code */
   bf_41termid             = 41,     /* Terminal id  */
   bf_42cardacc            = 42,     /* Card acceptor id  */
   bf_43cardacceptloc      = 43,     /* Card acceptor name/location */
   bf_45track1             = 45,     /* Track 1 data */
   bf_48addldata           = 48,     /* Additional private data */
   bf_52pinblock           = 52,     /* Pin block */
   bf_53seccntl            = 53,     /* Recurity related control info  */
   bf_54AddlAmount         = 54,     /* Additional amount */
   bf_55IccData		       = 55,     /* IC Card data*/
   bf_60private_use 	   = 60,     /* Self-Defined Field */
   bf_61private_use        = 61,     /* Cardholder Authentication Information */
   bf_62private_use        = 62,     /* Network Management Information Code */
   bf_63private_use        = 63,     /* Original Data */
   bf_64Mess_auth_code     = 64,     /* Message Security Code */   
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

		  *******************************************************/
		  AUTH_AUTHORIZATION,
		  0x30,0x31,0x30,0x30,                  /* message id, 0100 */
		  0x30,0x30,0x34,0x30,0x30,0x30,        /* processing code */
		  /**** OPTIONS ****/
		  topt1_postadv,
		  topt2_referral+topt2_reversal+topt2_floor,
		  /**** BIT FIELDS ****/
		  bf_2pan,					/* MAND */
		  bf_3proc,					/* MAND */
		  bf_4amount,				/* MAND */
		  bf_11stan,				/* MAND */
		  bf_12time,				/* MAND */ 
		  bf_13date,				/* MAND */ 
		  bf_14expiry,				/* OPTN  */
		  bf_22posentry,			/* MAND */
		  bf_23panseqnum,           /* OPTN */
		  bf_24nii, 	          	/* MAND */
		  bf_25poscc  ,  	        /* COND */
		  bf_35track2,				/* COND */
		  bf_37rrn,					/* MAND */
		  bf_38authcode,            /* COND */
		  bf_39respcode,            /* COND */
		  bf_41termid,				/* MAND */
		  bf_42cardacc,				/* MAND */      
		  bf_45track1,		    	/* COND */
		  bf_48addldata,         	/* OPTN  */
		  bf_52pinblock,        	/* COND */
		  bf_53seccntl,				/* COND */
		  bf_54AddlAmount,          /* COND */
		  bf_55IccData,         	/* COND */
		  bf_60private_use,    	/* MAND */
		  bf_61private_use,  /* COND */
		  bf_62private_use,        /* OPTN  */
		  bf_63private_use,        /* COND  */
		  bf_64Mess_auth_code,
		  bf_done			        /* end of fields to move */
      },
      
		/* 0110 MESSAGES */
      {
		  /***********************************************
		    Abhishek
			 AUTHORIZATION   RESPONSE
			
		  ***********************************************/
		  AUTH_AUTHORIZATION_RESPONSE,
		  0x30,0x31,0x31,0x30,                  /* message id, 0110 */
		  0x30,0x30,0x34,0x30,0x30,0x30,        /* processing code */
		  /**** OPTIONS ****/
		  topt1_postadv,
		  topt2_referral+topt2_reversal+topt2_floor,
		  /**** BIT FIELDS ****/
		  bf_2pan,					/* MAND */
		  bf_3proc,					/* MAND */
		  bf_4amount,				/* MAND */
		  bf_11stan,				/* MAND */
		  bf_12time,				/* MAND */ 
		  bf_13date,				/* MAND */ 
		  bf_24nii, 	          	/* MAND */
		  bf_37rrn,					/* MAND */
		  bf_38authcode,            /* OPTN  */
		  bf_39respcode,            /* OPTN  */
		  bf_41termid,				/* MAND */
		  bf_48addldata,         	/* OPTN  */
		  bf_53seccntl,				/* COND */
		  bf_55IccData,         	/* COND */
		  bf_63private_use,         /* COND */
		  bf_64Mess_auth_code,      /* OPTN  */
		  bf_done				    /* end of fields to move */
      },

	  { 
		  /*************************************************************
			A U T H O R I Z A T I O N	A D V I C E   
		  *************************************************************/
		  AUTH_ADVICE,
		  0x30,0x32,0x32,0x30,				   /* message id, 0220 */
		  0x30,0x30,0x30,0x30,0x30,0x30,	   /* processing code */
		  /**** OPTIONS ****/
		  topt1_postadv,
		  topt2_referral+topt2_reversal+topt2_floor,
		  /**** BIT FIELDS ****/
		  bf_2pan,		  /* MAND */
		  bf_3proc, 	  /* MAND */
		  bf_4amount,	  /* MAND */
		  bf_7trntime,
		  bf_11stan,	  /* MAND */
		  bf_12time,	  /* OPT  */
		  bf_13date,	  /* OPT  */
		  bf_14expiry,	  /* COND */
		  bf_22posentry,	 /* MAND */
		  bf_23panseqnum,
		  bf_24nii,
		  bf_25poscc,
		  //bf_35track2,
		  bf_37rrn, 	  /* MAND */
		  bf_38authcode,   /* COND */ 
		  bf_39respcode,  /* MAND */
		  bf_41termid,	  /* COND */
		  bf_42cardacc, 		   
		  //bf_43cardacceptloc,
		  //bf_45track1,
		  //bf_48addldata,  /* MAND */ 
		  //bf_52pinblock,
		  //bf_53seccntl,
		  //bf_54AddlAmount,
		  bf_55IccData,   /* COND */
		  //bf_60private_use,   /* MAND */
		  bf_61private_use, 	  /* COND */
		  bf_62private_use,   /* COND */
		  bf_63private_use,
		  //bf_64Mess_auth_code,
		  bf_done	  /* end of fields to move */
	  },
	  
	  { 
		  /*************************************************************
			A U T H O R I Z A T I O N	A D V I C E   R E S P O N S E
		  *************************************************************/
		  AUTH_ADVICE_RESPONSE,
		  0x30,0x32,0x33,0x30,				   /* message id, 0230 */
		  0x30,0x30,0x30,0x30,0x30,0x30,	   /* processing code */
		  /**** OPTIONS ****/
		  topt1_postadv,
		  topt2_referral+topt2_reversal+topt2_floor,
		  /**** BIT FIELDS ****/
		  bf_2pan,		  /* MAND */
		  bf_3proc, 	  /* MAND */
		  bf_4amount,	  /* MAND */
		  bf_11stan,	  /* MAND */
		  bf_12time,	  /* OPT  */
		  bf_13date,	  /* OPT  */
		  bf_22posentry,
		  bf_24nii,
		  bf_37rrn, 	  /* MAND */
		  bf_38authcode,
		  bf_39respcode,  /* MAND */
		  bf_41termid,	  /* COND */
		  bf_53seccntl,
		  bf_63private_use,
		  bf_64Mess_auth_code,
		  bf_done	  /* end of fields to move */
	  },
		  /* Add 0121 and 0131 for Cadience Standin Processing */
	  { 
		  /*************************************************************
			A U T H O R I Z A T I O N	A D V I C E   R E P E AT   
		  *************************************************************/
		  AUTH_ADVICE_REPEAT,
		  0x30,0x32,0x32,0x30,				   /* message id, 0220*/
		  0x30,0x30,0x30,0x30,0x30,0x30,	   /* processing code */
		  /**** OPTIONS ****/
		  topt1_postadv,
		  topt2_referral+topt2_reversal+topt2_floor,
		  /**** BIT FIELDS ****/
		  bf_2pan,		  /* MAND */
		  bf_3proc, 	  /* MAND */
		  bf_4amount,	  /* MAND */
		  bf_7trntime,
		  bf_11stan,	  /* MAND */
		  bf_12time,	  /* OPT  */
		  bf_13date,	  /* OPT  */
		  bf_14expiry,	  /* COND */
		  bf_22posentry,	 /* MAND */
		  bf_23panseqnum,
		  bf_24nii,
		  bf_25poscc,
		  //bf_35track2,
		  bf_37rrn, 	  /* MAND */
		  bf_38authcode,   /* COND */ 
		  bf_39respcode,  /* MAND */
		  bf_41termid,	  /* COND */
		  bf_42cardacc, 		   
		  //bf_43cardacceptloc,
		  //bf_45track1,
		  //bf_48addldata,  /* MAND */ 
		  //bf_52pinblock,
		  //bf_53seccntl,
		  //bf_54AddlAmount,
		  bf_55IccData,   /* COND */
		  //bf_60private_use,   /* MAND */
		  bf_61private_use, 	  /* COND */
		  bf_62private_use,   /* COND */
		  bf_63private_use,
		  //bf_64Mess_auth_code,
		  bf_done	  /* end of fields to move */
	  },
	  
	  { 
		  /***********************************************************************
			A U T H O R I Z A T I O N	A D V I C E   R E P E A T  R E S P O N S E
		  **********************************************************************/
		  AUTH_ADVICE_REPEAT_RESPONSE,
		  0x30,0x32,0x33,0x30,				   /* message id,0230 */
		  0x30,0x30,0x30,0x30,0x30,0x30,	   /* processing code */
		  /**** OPTIONS ****/
		  topt1_postadv,
		  topt2_referral+topt2_reversal+topt2_floor,
		  /**** BIT FIELDS ****/
		   bf_2pan,		  /* MAND */
		   bf_3proc, 	  /* MAND */
		   bf_4amount,	  /* MAND */
		   bf_11stan,	  /* MAND */
		   bf_12time,	  /* OPT  */
		   bf_13date,	  /* OPT  */
		   bf_15datestl,   /* COND */
		   bf_22posentry,
		   bf_24nii,
		   bf_37rrn, 	  /* MAND */
		   bf_38authcode,
		   bf_39respcode,  /* MAND */
		   bf_41termid,	  /* COND */
		   bf_53seccntl,
		   bf_63private_use,
		   bf_64Mess_auth_code,
		   bf_done	  /* end of fields to move */
	  }, 

		  
		/* 0200 MESSAGES */
	  {
	     /********************************************
		     Abhishek
			 POS SALE REQUEST

		  ********************************************/
		  AUTH_SALE,
		  0x30,0x32,0x30,0x30,                  /* message id, 0200 */
		  0x30,0x30,0x30,0x30,0x30,0x30,        /* processing code */
		  /**** OPTIONS ****/
		  topt1_postadv,
		  topt2_referral+topt2_reversal+topt2_floor,
		  /**** BIT FIELDS ****/
		  bf_2pan,					/* MAND */
		  bf_3proc,					/* MAND */
		  bf_4amount,				/* MAND */
		  bf_11stan,				/* MAND */
		  bf_12time,				/* MAND */
		  bf_13date,				/* MAND */
		  bf_14expiry,				/* OPTN  */
		  bf_22posentry,			/* MAND */
		  bf_23panseqnum,           /* OPTN */
		  bf_24nii, 	          	/* MAND */
		  bf_25poscc,   	        /* COND */
		  bf_35track2,				/* COND */
		  bf_37rrn,				/* MAND */
		  bf_41termid,				/* MAND */
		  bf_42cardacc,				/* MAND */      
		  bf_43cardacceptloc,		/* MAND */
		  bf_45track1,		    	/* COND */
		  bf_52pinblock,        	/* COND */
		  bf_53seccntl,				/* COND */
		  bf_54AddlAmount,          /* OPTN */
		  bf_55IccData,         	/* COND */
		  bf_60private_use,    	    /* MAND */
		  bf_61private_use,         /* COND */
		  bf_62private_use,         /* OPTN  */
		  bf_63private_use,         /* COND  */
		  bf_64Mess_auth_code,
		  bf_done			        /* end of fields to move */
      },
      
		/* 0210 MESSAGES */
      {
		  /***********************************************
			 POS   SALE  RESPONSE
			 Abhishek

		  ***********************************************/
		  AUTH_SALE_RESPONSE,
		  0x31,0x32,0x31,0x30,                  /* message id, 0210 */
		  0x30,0x30,0x30,0x30,0x30,0x30,        /* processing code */
		  /**** OPTIONS ****/
		  topt1_postadv,
		  topt2_referral+topt2_reversal+topt2_floor,
		  /**** BIT FIELDS ****/
		  bf_3proc,					/* MAND */
		  bf_4amount,				/* MAND */
		  bf_11stan,				/* MAND */
		  bf_12time,				/* MAND */ 
		  bf_13date,				/* MAND */ 
		  bf_23panseqnum,
		  bf_24nii, 	          	/* MAND */
		  bf_37rrn,					/* MAND */
		  bf_38authcode,            /* MAND */ 
	      bf_39respcode,            /* MAND */
		  bf_41termid,				/* MAND */
		  bf_48addldata,         	/* OPTN  */
		  bf_53seccntl,				/* COND */
		  bf_54AddlAmount,          /* OPTN  */
		  bf_55IccData,         	/* COND */
		  bf_63private_use,         /* COND */
		  bf_64Mess_auth_code,      /* COND */
		    bf_done                 /* end of fields to move */
      },

		/* 0200 MESSAGES */
	  {
	     /********************************************
		     Abhishek
			 POS SALE VOID REQUEST

		  ********************************************/
		  AUTH_VOID_SALE,
		  0x30,0x32,0x30,0x30,                  /* message id, 0200 */
		  0x30,0x32,0x30,0x30,0x30,0x30,        /* processing code */
		  /**** OPTIONS ****/
		  topt1_postadv,
		  topt2_referral+topt2_reversal+topt2_floor,
		  /**** BIT FIELDS ****/
		  bf_2pan,					/* MAND */
		  bf_3proc,					/* MAND */
		  bf_4amount,				/* MAND */
		  bf_11stan,				/* MAND */
		  bf_12time,				/* MAND */ 
		  bf_13date,				/* MAND */ 
		  bf_14expiry,				/* OPTN  */
		  bf_22posentry,			/* MAND */
		  bf_23panseqnum,           /* OPTN */
		  bf_24nii, 	          	/* MAND */
		  bf_25poscc,   	        /* COND */
		  bf_35track2,				/* COND */
		  bf_37rrn,					/* MAND */
		  bf_41termid,				/* MAND */
		  bf_42cardacc,				/* MAND */      
		  bf_43cardacceptloc,		/* MAND */
		  bf_45track1,		    	/* COND */
		  bf_52pinblock,        	/* COND */
		  bf_53seccntl,				/* COND */
		  bf_54AddlAmount,          /* OPTN */
		  bf_55IccData,         	/* COND */
		  bf_60private_use,    	    /* MAND */
		  bf_61private_use,         /* COND */
		  bf_62private_use,         /* OPTN  */
		  bf_63private_use,         /* COND  */
		  bf_64Mess_auth_code,
		  bf_done			        /* end of fields to move */
      },

		 /* 0210 MESSAGES */
	   {
		   /***********************************************
			  POS SALE VOID RESPONSE
			  Abhishek
 
		   ***********************************************/
		   AUTH_VOID_SALE_RESPONSE,
		   0x31,0x32,0x31,0x30, 				 /* message id, 0210 */
		   0x30,0x32,0x30,0x30,0x30,0x30,		 /* processing code */
		   /**** OPTIONS ****/
		   topt1_postadv,
		   topt2_referral+topt2_reversal+topt2_floor,
		   /**** BIT FIELDS ****/
		   bf_3proc,				 /* MAND */
		   bf_4amount,				 /* MAND */
		   bf_11stan,				 /* MAND */
		   bf_12time,				 /* MAND */ 
		   bf_13date,				 /* MAND */ 
		   bf_23panseqnum,
		   bf_24nii,				 /* MAND */
		   bf_37rrn,				 /* MAND */
		   bf_38authcode,			 /* MAND */ 
		   bf_39respcode,			 /* MAND */
		   bf_41termid, 			 /* MAND */
		   bf_48addldata,			 /* OPTN  */
		   bf_53seccntl,			 /* COND */
		   bf_54AddlAmount, 		 /* OPTN  */
		   bf_55IccData,			 /* COND */
		   bf_63private_use,		 /* COND */
		   bf_64Mess_auth_code, 	 /* COND */
			 bf_done				 /* end of fields to move */
	   },

		/*0220 message type */
	  {

	  	/********************************************
			   SALE COMPLETION REQUEST
			   Abhishek

		   ********************************************/
		  AUTH_SALES_COMPLETION,
		  0x30,0x32,0x32,0x30,                  /* message id, 0220 */
		  0x30,0x30,0x34,0x30,0x30,0x30,        /* processing code */
		  /**** OPTIONS ****/
		  topt1_postadv,
		  topt2_referral+topt2_reversal+topt2_floor,
		  /**** BIT FIELDS ****/
		  bf_2pan,                
		  bf_3proc,               
		  bf_4amount,             
		  bf_11stan,             
		  bf_12time,              
		  bf_13date,              
		  bf_14expiry,		  
		  bf_22posentry,          
		  bf_23panseqnum,         
		  bf_24nii,               
		  bf_25poscc,             
		  bf_37rrn,	          
		  bf_38authcode,          
		  bf_39respcode,          
		  bf_41termid,
		  bf_42cardacc,
		  bf_53seccntl,           
		  bf_54AddlAmount,        
		  bf_55IccData,		  
		  bf_60private_use, 	  
		  bf_61private_use,       
		  bf_62private_use,       
		  bf_63private_use,       
		  bf_64Mess_auth_code, 
		  bf_done                 /* end of fields to move */

	  },
	  
	  /*0230 message type */

	 {
	  	 /********************************************
			   SALE COMPLETION RESPONSE 
			   Abhishek

		   ********************************************/
		  AUTH_SALES_COMPLETION_RESPONSE,
		  0x30,0x32,0x33,0x30,                  /* message id, 0230 */
		  0x30,0x30,0x34,0x30,0x30,0x30,        /* processing code */
		  /**** OPTIONS ****/
		  topt1_postadv,
		  topt2_referral+topt2_reversal+topt2_floor,
		  /**** BIT FIELDS ****/
		  bf_2pan,
		  bf_3proc,               
		  bf_4amount,             
		  bf_11stan,             
		  bf_12time,              
		  bf_13date,
		  bf_15datestl,
		  bf_22posentry,
		  bf_23panseqnum,
		  bf_24nii,               
		  bf_37rrn,	          
		  bf_38authcode,          
		  bf_39respcode,          
		  bf_41termid,            
		  bf_53seccntl,           
		  bf_54AddlAmount,        
		  bf_55IccData,		  
		  bf_63private_use,       
		  bf_64Mess_auth_code, 
		  bf_done                 /* end of fields to move */
	  },

 		/*0320 message type */
	  {
 
		 /********************************************
				BATCH UPLOAD REQUEST
				Abhishek
 
		  ********************************************/
		   AUTH_BATCH_UPLOAD,
		   0x30,0x33,0x32,0x30, 				 /* message id, 0320 */
		   0x30,0x30,0x34,0x30,0x30,0x30,		 /* processing code */
		   /**** OPTIONS ****/
		   topt1_postadv,
		   topt2_referral+topt2_reversal+topt2_floor,
		   /**** BIT FIELDS ****/
		   bf_2pan, 			   
		   bf_3proc,			   
		   bf_4amount,			   
		   bf_11stan,			  
		   bf_12time,			   
		   bf_13date,			   
		   bf_14expiry, 	   
		   bf_22posentry,	
		   bf_23panseqnum,
		   bf_24nii,			   
		   bf_25poscc,			   
		   bf_37rrn,		   
		   bf_38authcode,		   
		   bf_39respcode,		   
		   bf_41termid, 		   
		   bf_42cardacc,		   
		   bf_53seccntl,		   
		   bf_54AddlAmount, 	   
		   bf_60private_use,	   
		   bf_61private_use,	   
		   bf_62private_use,	   
		   bf_63private_use,	   
		   bf_64Mess_auth_code, 
		   bf_done				   /* end of fields to move */
 
	  },
	   
	   /*0330 message type */
 
	  {
		  /********************************************
				BATCH UPLOAD RESPONSE 
				Abhishek
 
		  ********************************************/
		   AUTH_BATCH_UPLOAD_RESPONSE,
		   0x30,0x33,0x33,0x30, 				 /* message id, 0330 */
		   0x30,0x30,0x34,0x30,0x30,0x30,		 /* processing code */
		   /**** OPTIONS ****/
		   topt1_postadv,
		   topt2_referral+topt2_reversal+topt2_floor,
		   /**** BIT FIELDS ****/
		   bf_3proc,			   
		   bf_4amount,			   
		   bf_11stan,			  
		   bf_12time,			   
		   bf_13date,			   
		   bf_37rrn,		   
		   bf_39respcode,		   
		   bf_41termid, 		   
		   bf_53seccntl,		   
		   bf_63private_use,	   
		   bf_64Mess_auth_code, 
		   bf_done				   /* end of fields to move */
	   },

		 
     /* 0400 MESSAGES */
	   {
		  /****************************************	
		  	Abhishek 
			REVERSAL REQUEST

			Reversal Message request (sent by the BDO HOST SYSTEM)
			 	 
		  *****************************************/
		  AUTH_REVERSAL,
		  0x30,0x34,0x30,0x30,                  /* message id, 0400 */
		  0x30,0x30,0x34,0x30,0x30,0x30,        /* processing code */
		  /**** OPTIONS ****/
		  topt1_postadv,
		  topt2_referral+topt2_reversal+topt2_floor,
		  /**** BIT FIELDS ****/
		  bf_2pan,					/* MAND */
		  bf_3proc,					/* MAND */
		  bf_4amount,				/* MAND */
		  bf_11stan,				/* MAND */
		  bf_12time,				/* MAND */ 
		  bf_13date,				/* MAND */ 
		  bf_14expiry,				/* OPTN  */
		  bf_22posentry,			/* MAND */
		  bf_23panseqnum,           /* COND */
		  bf_24nii, 	          	/* MAND */
		  bf_25poscc,  	            /* COND */
		  bf_35track2,				/* COND */
		  bf_37rrn,					/* MAND */
		  bf_41termid,				/* MAND */
		  bf_42cardacc,				/* MAND */      
		  bf_45track1,		    	/* COND */
		  bf_52pinblock,        	/* COND */
		  bf_53seccntl,				/* COND */
		  bf_55IccData,             /* COND */
		  bf_61private_use,         /* COND */
		  bf_62private_use,         /* OPTN  */
		  bf_63private_use,         /* COND */
		  bf_64Mess_auth_code,	    /* COND */
		  bf_done			       /* end of fields to move */
      },

		/* 0410 MESSAGES */
      {
		  /***********************************************
			REVERSAL RESPONSE

			Reversal Message response (sent by the BDO HOST SYSTEM)
		  				 
		  ***********************************************/
		  AUTH_REVERSAL_RESPONSE,
		  0x30,0x34,0x31,0x30,                  /* message id, 0410 */
		  0x30,0x30,0x34,0x30,0x30,0x30,        /* processing code */
		  /**** OPTIONS ****/
		  topt1_postadv,
		  topt2_referral+topt2_reversal+topt2_floor,
		  /**** BIT FIELDS ****/
		  bf_2pan,
		  bf_3proc,					/* MAND */
		  bf_4amount,				/* MAND */
		  bf_11stan,				/* MAND */
		  bf_12time,				/* MAND */ 
		  bf_13date,				/* MAND */ 
		  bf_22posentry,
		  bf_23panseqnum,
		  bf_24nii, 	          	/* MAND */
		  bf_37rrn,					/* MAND */
		  bf_38authcode,            /* MAND */
		  bf_39respcode,            /* MAND */
		  bf_41termid,				/* MAND */
		  bf_53seccntl,				/* COND */
		  bf_63private_use,        /* COND  */
		  bf_64Mess_auth_code, 
		  bf_done				  /* end of fields to move */
      },

	  /* 0420 MESSAGES */
	   {
		  /****************************************
			Abhishek
			REVERSAL ADVICE REQUEST
		  *****************************************/
		  AUTH_REVERSAL_ADVICE,
		  0x30,0x34,0x32,0x30,                  /* message id, 0420 */
		  0x30,0x30,0x30,0x30,0x30,0x30,        /* processing code */
		  /**** OPTIONS ****/
		  topt1_postadv,
		  topt2_referral+topt2_reversal+topt2_floor,
		  /**** BIT FIELDS ****/
		  bf_2pan,					/* MAND */
		  bf_3proc,					/* MAND */
		  bf_4amount,				/* MAND */
		  bf_11stan,				/* MAND */
		  //bf_12time,				/* MAND */
		  //bf_13date,				/* MAND */
		  bf_14expiry,				/* OPTN  */
		  bf_22posentry,			/* MAND */
		  //bf_23panseqnum,           /* COND */
		  bf_24nii, 	          	/* MAND */
		  bf_25poscc,  	            /* COND */
		  bf_35track2,				/* COND */
		  bf_37rrn,					/* MAND */
		  //bf_38authcode,            /* MAND */
		  bf_41termid,				/* MAND */
		  bf_42cardacc,				/* MAND */
		  //bf_45track1,		    	/* COND */
		  //bf_55IccData,             /* COND */
		  bf_61private_use,         /* MAND */
		  bf_62private_use,         /* OPTN  */
		  bf_63private_use,        /* COND  */
		  bf_done			       /* end of fields to move */
		},

	/* 0430 MESSAGES */
	{
		  /***********************************************
			REVERSAL ADVICE RESPONSE
		  ***********************************************/
		  AUTH_REVERSAL_ADVICE_RESPONSE,
		  0x30,0x34,0x33,0x30,                  /* message id, 0430 */
		  0x30,0x30,0x34,0x30,0x30,0x30,        /* processing code */
		  /**** OPTIONS ****/
		  topt1_postadv,
		  topt2_referral+topt2_reversal+topt2_floor,
		  /**** BIT FIELDS ****/
		  bf_3proc,					/* MAND */
		  bf_4amount,				/* MAND */
		  bf_11stan,				/* MAND */
		  bf_12time,				/* MAND */
		  bf_13date,				/* MAND */
		  bf_24nii, 	          	/* MAND */
		  bf_37rrn,					/* MAND */
		  bf_38authcode,            /* MAND */
		  bf_39respcode,            /* MAND */
		  bf_41termid,				/* MAND */
		  bf_54AddlAmount,			/* COND */
		  bf_done				  /* end of fields to move */
	},

	 /*0500 message type */
     {
  
	 /********************************************
			SETTLEMENT REQUEST
			Abhishek
  
	   ********************************************/
	   AUTH_SETTLE,
	   0x30,0x35,0x30,0x30, 				 /* message id, 0500 */
	   0x30,0x30,0x34,0x30,0x30,0x30,		 /* processing code */
	   /**** OPTIONS ****/
	   topt1_postadv,
	   topt2_referral+topt2_reversal+topt2_floor,
	   /**** BIT FIELDS ****/
	   bf_3proc,			   
	   bf_11stan,			  
	   bf_24nii,			   
	   bf_41termid, 		   
	   bf_42cardacc,		   
	   bf_53seccntl,		   
	   bf_60private_use,	   
	   bf_63private_use,	   
	   bf_64Mess_auth_code, 
	   bf_done				   /* end of fields to move */
  
   },
   
   /*0510 message type */
  
  {
	  /********************************************
			SETTLEMENT RESPONSE 
			Abhishek
  
   	   ********************************************/
	   AUTH_SETTLE_RESPONSE,
	   0x30,0x35,0x31,0x30, 				 /* message id, 0510 */
	   0x30,0x30,0x34,0x30,0x30,0x30,		 /* processing code */
	   /**** OPTIONS ****/
	   topt1_postadv,
	   topt2_referral+topt2_reversal+topt2_floor,
	   /**** BIT FIELDS ****/
	   
	   bf_3proc,			   
	   bf_11stan,			  
	   bf_12time,			   
	   bf_13date,			   
	   bf_24nii,			   
	   bf_37rrn,		   
	   bf_39respcode,		   
	   bf_41termid, 		   
	   bf_48addldata,		   
	   bf_53seccntl,		   
	   bf_63private_use,	   
	   bf_64Mess_auth_code, 
	   bf_done				   /* end of fields to move */
   },
	 /*0500 message type */
		 {
	  
		 /********************************************
				SETTLEMENT TRAILER REQUEST
				Abhishek
	  
		   ********************************************/
		   AUTH_SETTLE_TRAILER,
		   0x30,0x35,0x30,0x30, 				 /* message id, 0500 */
		   0x39,0x36,0x30,0x30,0x30,0x30,		 /* processing code */
		   /**** OPTIONS ****/
		   topt1_postadv,
		   topt2_referral+topt2_reversal+topt2_floor,
		   /**** BIT FIELDS ****/
		   bf_3proc,			   
		   bf_11stan,			  
		   bf_24nii,			   
		   bf_41termid, 		   
		   bf_42cardacc,		   
		   bf_53seccntl,		   
		   bf_60private_use,	   
		   bf_63private_use,	   
		   bf_64Mess_auth_code, 
		   bf_done				   /* end of fields to move */
	  
	   },
	   
	   /*0510 message type */
	  
	  {
		  /********************************************
				SETTLEMENT TRAILER RESPONSE 
				Abhishek
	  
		   ********************************************/
		   AUTH_SETTLE_TRAILER_RESPONSE,
		   0x30,0x35,0x31,0x30, 				 /* message id, 0510 */
		   0x39,0x36,0x30,0x30,0x30,0x30,		 /* processing code */
		   /**** OPTIONS ****/
		   topt1_postadv,
		   topt2_referral+topt2_reversal+topt2_floor,
		   /**** BIT FIELDS ****/
		   
		   bf_3proc,			   
		   bf_11stan,			  
		   bf_12time,			   
		   bf_13date,			   
		   bf_24nii,			   
		   bf_37rrn,		   
		   bf_39respcode,		   
		   bf_41termid, 		   
		   bf_48addldata,		   
		   bf_53seccntl,		   
		   bf_63private_use,	   
		   bf_64Mess_auth_code, 
		   bf_done				   /* end of fields to move */
	   },

	  /*Dummy 200 message for logon*/
   {
	  /********************************************
		 L O G O N
	  ********************************************/
	  AUTH_LOGON,
	  0x30,0x31,0x30,0x30,                  /* message id, 0200 */
	  0x30,0x30,0x30,0x30,0x30,0x30,        /* processing code */
	  /**** OPTIONS ****/
	  topt1_postadv,
	  topt2_referral+topt2_reversal+topt2_floor,
	  /**** BIT FIELDS ****/
	  bf_3proc,
	  bf_11stan,			 /* MAND */
	  bf_22posentry,          /* MAND */
	  bf_24nii,			 /* MAND */ 
	  bf_25poscc,    /* MAND */
	  bf_35track2,          /* MAND */
	  bf_41termid,  /* MAND */
	  bf_42cardacc,  /* MAND */
	  bf_62private_use,  /* MAND */
  	  bf_63private_use,  /* MAND */
	  bf_done			/* end of fields to move */
   },
 };

#define N_TRANENTRY (sizeof(TRANTABLE) / sizeof(struct trandef))

#define N_BFData ( sizeof(BFData) / sizeof (struct bitfield_data) )

struct bitfield_data *Find_Bitfield_Table (enum bitfields);
struct trandef       *Find_Tran_Table     (pAUTH_TX p_auth_tx);

struct iso_msg_rec
{
	unsigned char msgtype       [4];
 	unsigned char msgtype_bcd   [2];
	unsigned char bitmap_bin    [8];
	unsigned char pan_len       [1];
	unsigned char pan       [10];
	unsigned char proccode      [3];
	unsigned char amount    [6];
	unsigned char gmt_time    [5];
	unsigned char stan          [3];
	unsigned char time          [3];
	unsigned char date          [2];
	unsigned char date_expd     [2];
	unsigned char date_settle   [2];
	unsigned char mcc           [4];
	unsigned char posentry      [2];
	unsigned char panseqnum     [2];
	unsigned char card_seq_num  [2];
	unsigned char nii           [2];
	unsigned char pos_condcd    [1];
	unsigned char track2_len    [1];
	unsigned char track2        [19];
	unsigned char rrn           [12];
	unsigned char authcode      [6];
	unsigned char responsecd    [2];
	unsigned char termid        [8];
	unsigned char cardaccid     [15];		
	unsigned char track1_len    [1];
	unsigned char track1        [76];
	unsigned char addl_data_len   [2]; 
	unsigned char addl_data     [16];
	unsigned char pinblock      [8];
	unsigned char sec_cntl       [8];
	unsigned char addl_len      [2];
	unsigned char addlamts      [50];
	unsigned char IccData_len   [2];
	unsigned char IccData       [255];
	unsigned char priv60_len    [2];
	unsigned char priv60        [100];
	unsigned char priv61_len    [2];
	unsigned char priv61        [150];
	unsigned char priv62_len    [2];
	unsigned char priv62        [100];
	unsigned char priv63_len    [2];
	unsigned char priv63        [350];
	unsigned char cardaccname   [41];
	unsigned char mess_auth_code [8];
	unsigned char addl_rsp_len   [2];
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
void vBCDMove(enum bitfields fieldid);
void RspMovevBCD (enum bitfields fieldid);
void vDataMove(enum bitfields fieldid);
void RspMovevData (enum bitfields fieldid);
INT isoutil_Move_IfThere(enum bitfields fieldid, pCHAR trantype);
INT isoutil_CondPan(enum bitfields bitfield, pCHAR p_trantype);
INT isoutil_CondTrack2(enum bitfields bitfield, pCHAR p_trantype);
void isoutil_vBCDMove(enum bitfields fieldid);
BOOLEAN isoutil_RspMovevBCD (enum bitfields fieldid);
INT isoutil_Move_Always(enum bitfields bitfield, pCHAR trantype);
void isoutil_GenericMove(enum bitfields fieldid);
BOOLEAN isoutil_RspMoveGeneric(enum bitfields fieldid);
INT isoutil_CondExpd(enum bitfields bitfield, pCHAR trantype);
INT isoutil_CondTrack1 (enum bitfields bitfield, pCHAR trantype);
BOOLEAN isoutil_RspIgnore(enum bitfields fieldid);
void isoutil_vDataMove(enum bitfields fieldid);
BOOLEAN isoutil_RspMovevData (enum bitfields fieldid);


static  struct bitfield_data BFData[] = {
  	  {
	  bf_2pan,						/* field */
	  HOST_MESSAGE.pan_len,			/* data to move */
	  1,							/* # of bytes in pan len field */
	  isoutil_CondPan,				/* conditional */
	  isoutil_vBCDMove, 			/* move routine */
	  isoutil_RspMovevBCD			/* response routine */
	  },
	  {
	  bf_3proc, 					/* field */
	  HOST_MESSAGE.proccode, 		/* data to move */
	  3,							/* # of bytes in iso8583 field */
	  isoutil_Move_Always,			/* conditional */
	  isoutil_GenericMove,			/* move routine */
	  isoutil_RspMoveGeneric		/* response routine */
	  },
	  {
	  bf_4amount,					/* field */
	  HOST_MESSAGE.amount,		/* data to move */
	  6,							/* # of bytes in iso8583 field */
	  isoutil_Move_Always,			/* conditional */
	  isoutil_GenericMove,			/* move routine */
	  isoutil_RspMoveGeneric		/* response routine */
	  },
	  {
	  bf_7trntime,					/* field */
	  HOST_MESSAGE.gmt_time, 		/* data to move : MMDDhhmmss*/
	  5,							/* # of bytes in iso8583 field */
	  isoutil_Move_Always,			/* conditional */
	  isoutil_GenericMove,			/* move routine */
	  isoutil_RspMoveGeneric		/* response routine */
	  },
	  {
	  bf_11stan,					/* field */
	  HOST_MESSAGE.stan, 		/* data to move */
	  3,							/* # of bytes in iso8583 field */
	  isoutil_Move_Always,			/* conditional */
	  isoutil_GenericMove,			/* move routine */
	  isoutil_RspMoveGeneric		/* response routine */
	  },
	  {
	  bf_12time,					/* field */
	  HOST_MESSAGE.time, 		/* data to move */
	  3,							/* # of bytes in iso8583 field */
	  isoutil_Move_IfThere, 		/* conditional */
	  isoutil_GenericMove,			/* move routine */
	  isoutil_RspMoveGeneric		/* response routine */
	  },
	  {
	  bf_13date,					/* field */
	  HOST_MESSAGE.date, 		/* data to move */
	  2,							/* # of bytes in iso8583 field */
	  isoutil_Move_IfThere, 		/* conditional */
	  isoutil_GenericMove,			/* move routine */
	  isoutil_RspMoveGeneric		/* response routine */
	  },
	  {
	  bf_14expiry,					/* field */
	  HOST_MESSAGE.date_expd,		/* data to move */
	  2,							/* # of bytes in iso8583 field */
	  isoutil_CondExpd, 			/* conditional */
	  isoutil_GenericMove,			/* move routine */
	  isoutil_RspMoveGeneric		/* response routine */
	  },
	  {
	  bf_15datestl, 				/* field */
	  HOST_MESSAGE.date_settle,		/* data to move */
	  2,							/* # of bytes in iso8583 field */
	  isoutil_Move_IfThere, 		/* conditional */
	  isoutil_GenericMove,			/* move routine */
	  isoutil_RspMoveGeneric		/* response routine */
	  },
	  {
	  bf_18sicnumber,				/* field */
	  HOST_MESSAGE.mcc,				/* data to move */
	  4,							/* # of bytes in mcrd iso field */
	  isoutil_Move_IfThere, 		/* conditional */
	  isoutil_GenericMove,			/* move routine */
	  isoutil_RspMoveGeneric		/* response routine */
	  },
	  {
	  bf_22posentry,					/* field */
	  HOST_MESSAGE.posentry, 		/* data to move */
	  2,							/* # of bytes in iso8583 field */
	  isoutil_Move_Always,			/* mandatory */
	  isoutil_GenericMove,			/* move routine */
	  isoutil_RspMoveGeneric		/* response routine */
	  },
	  {
	  bf_23panseqnum,				/* field */
	  HOST_MESSAGE.panseqnum,		/* data to move */
	  2,							/* # of bytes in iso8583 field */
	  isoutil_Move_IfThere, 		/* conditional */
	  isoutil_GenericMove,			/* move routine */
	  isoutil_RspMoveGeneric		/* response routine */
	  },
	  {
	  bf_24nii, 					/* field */
	  HOST_MESSAGE.nii,				/* data to move */
	  2,							/* # of bytes in iso8583 field */
	  isoutil_Move_Always,			/* conditional */
	  isoutil_GenericMove,			/* move routine */
	  isoutil_RspMoveGeneric		/* response routine */
	  },
	  {
	  bf_25poscc,					/* field */
	  HOST_MESSAGE.pos_condcd,		/* data to move */
	  1,							/* # of bytes in iso8583 field */
	  isoutil_Move_Always,			/* conditional */
	  isoutil_GenericMove,			/* move routine */
	  isoutil_RspMoveGeneric		/* response routine */
	  },
	  {
	  bf_35track2,					/* field */
	  HOST_MESSAGE.track2_len,		/* data to move */
	  1,							/* # of bytes in track2 len field */
	  isoutil_CondTrack2, 			/* conditional */
	  isoutil_vBCDMove, 			/* move routine */
	  isoutil_RspMovevBCD			/* response routine */
	  },
	  {
	  bf_37rrn, 					/* field */
	  HOST_MESSAGE.rrn,				/* data to move */
	  12,							/* # of bytes in iso8583 field */
	  isoutil_Move_IfThere, 		/* conditional */
	  isoutil_GenericMove,			/* move routine */
	  isoutil_RspMoveGeneric		/* response routine */
	  },
	  {
	  bf_38authcode,				/* field */
	  HOST_MESSAGE.authcode, 		/* data to move */
	  6,							/* # of bytes in iso8583 field */
	  isoutil_Move_IfThere, 		/* conditional */
	  isoutil_GenericMove,			/* move routine */
	  isoutil_RspMoveGeneric		/* response routine */
	  },
	  {
	  bf_39respcode,				/* field */
	  HOST_MESSAGE.responsecd,		/* data to move */
	  2,							/* # of bytes in iso8583 field */
	  isoutil_Move_IfThere, 		/* conditional */
	  isoutil_GenericMove,			/* move routine */
	  isoutil_RspMoveGeneric		/* response routine */
	  },
	  {
	  bf_41termid,					/* field */
	  HOST_MESSAGE.termid,			/* data to move */
	  8,							/* # of bytes in iso8583 field */
	  isoutil_Move_Always,			/* conditional */
	  isoutil_GenericMove,			/* move routine */
	  isoutil_RspMoveGeneric		/* response routine */
	  },
	  {
	  bf_42cardacc, 				/* field */
	  HOST_MESSAGE.cardaccid,		/* data to move */
	  15,							/* # of bytes in iso8583 field */
	  isoutil_Move_Always,			/* conditional */
	  isoutil_GenericMove,			/* move routine */
	  isoutil_RspMoveGeneric		/* response routine */
	  },
	  {
	  bf_43cardacceptloc,				/* field */
	  HOST_MESSAGE.cardaccname,	   /* data to move */
	  40,							/* # of bytes in  iso8583 field */
	  isoutil_Move_IfThere, 		/* conditional */
	  isoutil_GenericMove,			/* move routine */
	  isoutil_RspIgnore 			/* response routine */
	  },
	  {
	  bf_45track1,					/* field */
	  HOST_MESSAGE.track1_len,		/* data to move */
	  1,							/* # of bytes in track1 len field */
	  isoutil_CondTrack1,			/* conditional */
	  isoutil_vDataMove,			/* move routine */
	  isoutil_RspMovevData			/* response routine */
	  },
	  {
	  bf_48addldata, 				/* field */
	  HOST_MESSAGE.addl_data_len,		/* data to move */
	  2,							/* # of bytes in iso8583 field */
	  isoutil_Move_IfThere, 		/* conditional */
	  isoutil_vDataMove,			/* move routine */
	  isoutil_RspMovevData			/* response routine */
	  },
	  {
	  bf_52pinblock,				/* field */
	  HOST_MESSAGE.pinblock,		/* data to move */
	  8,							/* # of bytes in iso8583 field */
	  isoutil_Move_IfThere, 		/* conditional */
	  isoutil_GenericMove,			/* move routine */
	  isoutil_RspMoveGeneric		/* response routine */
	  },
	  {
	  bf_53seccntl,				  /* field */
	  HOST_MESSAGE.sec_cntl,		  /* data to move */
	  8,							/* # of bytes in iso8583 field */
	  isoutil_Move_IfThere, 		/* conditional */
	  isoutil_GenericMove,			/* move routine */
	  isoutil_RspMoveGeneric		/* response routine */
	  },
	  {
	  bf_54AddlAmount,				/* field */
	  HOST_MESSAGE.addl_len, 		/* data to move */
	  2,							/* # of bytes in addl len field */
	  isoutil_Move_IfThere, 		/* conditional */
	  isoutil_vDataMove,			/* move routine */
	  isoutil_RspMovevData			/* response routine */
	  },
	  {
	  bf_55IccData, 				/* field */
	  HOST_MESSAGE.IccData_len,		/* data to move */
	  2,							/* # of bytes in addl len field */
	  isoutil_Move_IfThere, 		/* conditional */
	  isoutil_vDataMove,			/* move routine */
	  isoutil_RspMovevData			/* response routine */
	  },
	  {
	  bf_60private_use,					/* field */
	  HOST_MESSAGE.priv60_len,		/* data to move */
	  2,							/* # of bytes in priv60 len field */
	  isoutil_Move_IfThere, 		/* conditional */
	  isoutil_vDataMove,			/* move routine */
	  isoutil_RspMovevData			/* response routine */
	  },
	  {
	  bf_61private_use,					/* field */
	  HOST_MESSAGE.priv61_len,		/* data to move */
	  2,							/* # of bytes in priv61 len field */
	  isoutil_Move_IfThere, 		/* conditional */
	  isoutil_vDataMove,			/* move routine */
	  isoutil_RspMovevData			/* response routine */
	  },
	  {
	  bf_62private_use,					/* field */
	  HOST_MESSAGE.priv62_len ,		 /* data to move */
	  2,							/* # of bytes in priv62 len field */
	  isoutil_Move_IfThere, 		/* conditional */
	  isoutil_vDataMove,			/* move routine */
	  isoutil_RspMovevData			/* response routine */
	  },
	  {
	  bf_63private_use,					/* field */
	  HOST_MESSAGE.priv63_len,		/* data to move */
	  2,							/* # of bytes in priv63 len field */
	  isoutil_Move_IfThere, 		/* conditional */
	  isoutil_vDataMove,			/* move routine */
	  isoutil_RspMovevData			/* response routine */
	  },
      {
	  bf_64Mess_auth_code,                /* field */
	  HOST_MESSAGE.mess_auth_code,    /* data to move */
	  3,                            		/* # of bytes in  iso field */
	  Move_IfThere,                 		/* mandatory    */
	  GenericMove,               		/* move routine */
	  RspMoveGeneric               		/* response routine */
	  },
	  {
	  bf_done,						/* field */
	  NULL, 						/* data to move */
	  99,							/* # of bytes in iso8583 field */
	  NULL, 						/* conditional */
	  NULL, 						/* move routine */
	  NULL							/* response routine */
	  }
 }; /* end BFData */






#endif
