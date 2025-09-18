/* FILE: RESPONSE.H */
/* GOAL: To document various symbols and structures for RESPONSE.C */

/*****************************************************************************/
/**                          BIT MAP SYMBOLS                                **/
/*****************************************************************************/

#include "app_info.h"
#include "txutils.h"
#include "ptetime.h"

struct bitfield_data *ncdci_Find_Bitfield_Table( enum bitfields bitfield );
void ncdci_RspMovevBCD (enum bitfields fieldid);
INT ncdci_GenericMove(enum bitfields fieldid);
INT ncdci_vDataMove(enum bitfields fieldid);
INT ncdci_RspMovevData (enum bitfields fieldid);
INT ncdci_RspMoveGeneric(enum bitfields fieldid);
INT ncdci_Move_IfThere( enum bitfields fieldid, BYTE p_trantype );
INT ncdci_Move_Always( enum bitfields bitfield, BYTE p_trantype );
int ncdci_get_variable_length( unsigned char *DataField, int FieldLen );
struct trandef *ncdci_Find_Tran_Table( BYTE trantype );
struct bitfield_data *ncdci_Find_Bitfield_Table( enum bitfields bitfield );
void ncdci_copy_to_req_resp_buffer ( LPBYTE p_msg );

#define DINERS_TOTALL_MEG_LEN			4
#define DINERS_MESSAGE_TYPE_SIZE		4
#define DINERS_DE3_SIZE       			6
#define DINERS_EXTENDED_BITMAP_SIZE		16
#define DINERS_NORMAL_BITMAP_SIZE		8
#define DINERS_EXTENDED_BITFIELD_SIZE	128
#define DINERS_NORMAL_BITFIELD_SIZE		64

int BitFieldSize = DINERS_NORMAL_BITFIELD_SIZE;

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
		bf_92txorigIntcntrycode,			/*MAND*/
		bf_100revinsidcode,
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
		//ramya
		bf_6cardbillamt,
		bf_7trntime,			    		/* MAND ECHO */
		bf_11stan,			    			/* MAND ECHO */
		bf_12time,		        			/* MAND ECHO */
		bf_13date,                			/* MAND ECHO */
		bf_14expiry,              			/* MAND PREP  */
		bf_23csn,
		bf_24functioncode,					/* MAND */
		bf_25poscc,               			/* MAND ECHO */
		bf_32idcode,              			/* MAND ECHO */
		bf_33fwdinstidcode,       			/* MAND ECHO */
		bf_37rrn,				    		/* MAND ECHO */
		bf_38authcode,		    			/* COND PREP  */
		bf_39respcode,		    			/* MAND PREP  */
		bf_41termid,			    		/* MAND ECHO */
		bf_42cardacc,			    		/* MAND ECHO */

		bf_44respdata,	        			/* OPTN PREP   */
		//ramya
		bf_48addldata,
		bf_49trncurcd,		    			/* MAND ECHO */
		//ramya
		bf_51crdcurcd,
		bf_54AddlAmount,          			/* OPTN PREP   */
		bf_55IccData,
		bf_92txorigIntcntrycode,			/*MAND*/
		bf_100revinsidcode,       /* MAND ECHO */
		//ramya
		bf_123netrefID,
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
		bf_14expiry,						/* OPTN  */
		bf_22posentry,						/* MAND */
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
		bf_92txorigIntcntrycode,			/*MAND*/
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
		bf_7trntime,			   	 		/* MAND ECHO */
		bf_11stan,			    			/* MAND ECHO */
		bf_12time,		       				/* MAND ECHO */
		bf_13date,               			/* MAND ECHO */
		bf_14expiry,              			/* MAND PREP  */
		bf_24functioncode,					/* MAND */
		bf_25poscc,               			/* MAND ECHO */
		bf_32idcode,              			/* MAND ECHO */
		bf_33fwdinstidcode,       			/* MAND ECHO */
		bf_37rrn,				    		/* MAND ECHO */
		bf_39respcode,		   				/* MAND PREP  */
		bf_41termid,			    		/* MAND ECHO */
		bf_42cardacc,			    		/* MAND ECHO */
		bf_49trncurcd,		    			/* MAND ECHO */
		bf_54AddlAmount,
		bf_92txorigIntcntrycode,			/*MAND*/
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
		bf_48addldata,         				/* OPTN  */
		bf_49trncurcd,						/* MAND */
		bf_52pinblock,        				/* COND */
		bf_55IccData,         				/* COND */
		bf_92txorigIntcntrycode,			/*MAND*/
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
		bf_7trntime,			    		/* MAND ECHO */
		bf_11stan,			    			/* MAND ECHO */
		bf_12time,		        			/* MAND ECHO */
		bf_13date,                			/* MAND ECHO */
		bf_14expiry,              			/* MAND PREP  */
		bf_23csn,
		bf_24functioncode,					/* MAND */
		bf_25poscc,               			/* MAND ECHO */
		bf_32idcode,              			/* MAND ECHO */
		bf_33fwdinstidcode,       			/* MAND ECHO */
		bf_37rrn,				    		/* MAND ECHO */
		bf_38authcode,		    			/* COND PREP  */
		bf_39respcode,		    			/* MAND PREP  */
		bf_41termid,			    		/* MAND ECHO */
		bf_42cardacc,			    		/* MAND ECHO */
		bf_44respdata,	        			/* OPTN PREP   */
		bf_49trncurcd,		    			/* MAND ECHO */
		bf_54AddlAmount,          			/* OPTN PREP   */
		bf_55IccData,
		bf_92txorigIntcntrycode,			/*MAND*/
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
		bf_92txorigIntcntrycode,			/*MAND*/
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
		bf_7trntime,			   			/* MAND ECHO */
		bf_11stan, 			   				/* MAND ECHO */
		bf_12time, 			   				/* MAND ECHO */
		bf_13date, 			   				/* MAND ECHO */
		bf_14expiry,			  			/* MAND PREP  */
		bf_23csn,
		bf_24functioncode,					/* MAND */
		bf_25poscc,			   				/* MAND ECHO */
		bf_32idcode,			   			/* MAND ECHO */
		bf_33fwdinstidcode,	   				/* MAND ECHO */
		bf_37rrn,				   			/* MAND ECHO */
		bf_38authcode, 		   				/* COND PREP  */
		bf_39respcode, 		   				/* MAND PREP  */
		bf_41termid,			   			/* MAND ECHO */
		bf_42cardacc,			   			/* MAND ECHO */
		bf_44respdata, 		   				/* OPTN PREP   */
		bf_49trncurcd, 		   				/* MAND ECHO */
		bf_54AddlAmount,		   			/* OPTN PREP   */
		bf_55IccData,
		bf_92txorigIntcntrycode,			/*MAND*/
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
		bf_92txorigIntcntrycode,			/*MAND*/
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
		bf_7trntime,            			/* MAND ECHO */
		bf_11stan,              			/* MAND ECHO */
		bf_12time,              			/* MAND ECHO */
		bf_13date,              			/* MAND ECHO */
		bf_14expiry,		    			/* MAND PREP  */
		bf_23csn,
		bf_24functioncode,					/* MAND */
		bf_25poscc,             			/* MAND ECHO */
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
		bf_45track1,            			/* COND */
		bf_48addldata,          			/* COND */
		bf_49trncurcd,          			/* MAND */
		bf_51crdcurcd,          			/* COND */
		bf_52pinblock,          			/* COND */
		bf_55IccData,
		bf_92txorigIntcntrycode,			/*MAND*/
		bf_done			        /* end of fields to move */
	},

	/***********************************************
	 *  CASH ADVANCE RESPONSE
	***********************************************/
	{
		AUTH_CASH_ADVANCE_RESPONSE,
		0x31,0x32,0x31,0x30,                  /* message id, 1210 */
		0x30,0x31,0x30,0x30,0x30,0x30,        /* processing code */
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
		bf_14expiry,		    			/* MAND PREP  */
		bf_23csn,
		bf_24functioncode,					/* MAND */
		bf_25poscc,             			/* MAND ECHO */
		bf_32idcode,            			/* MAND ECHO */
		bf_33fwdinstidcode,     			/* MAND ECHO */
		bf_37rrn,	            			/* MAND ECHO */
		bf_38authcode,          			/* OPTN PREP   */
		bf_39respcode,          			/* MAND PREP  */
		bf_41termid,            			/* MAND ECHO */
		bf_42cardacc,           			/* MAND ECHO */
		bf_44respdata,	        			/* OPTN PREP   */
		bf_49trncurcd,          			/* MAND ECHO */
		bf_54AddlAmount,        			/* OPTI PREP    */
		bf_55IccData,
		bf_92txorigIntcntrycode,			/*MAND*/
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
		bf_55IccData,
		bf_92txorigIntcntrycode,			/*MAND*/
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
		bf_23csn,
		bf_24functioncode,					/* MAND */
		bf_25poscc,             			/* MAND ECHO */
		bf_32idcode,            			/* MAND ECHO */
		bf_33fwdinstidcode,     			/* MAND ECHO */
		bf_37rrn,	            			/* MAND ECHO */
		bf_38authcode,          			/* MAND ECHO */
		bf_39respcode,          			/* MAND PREP  */
		bf_41termid,            			/* MAND ECHO */
		bf_42cardacc,           			/* MAND ECHO */
		bf_49trncurcd,          			/* MAND ECHO */
		bf_51crdcurcd,          			/* COND PREP  */
		bf_55IccData,
		bf_92txorigIntcntrycode,			/*MAND*/
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
		bf_92txorigIntcntrycode,			/*MAND*/
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
		bf_55IccData,
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
		bf_92txorigIntcntrycode,			/*MAND*/
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
		bf_55IccData,
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
		bf_92txorigIntcntrycode,			/*MAND*/
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



#define BF01_ON   0x80
#define BF01_off     0
#define BF02_ON   0x40
#define BF02_off     0
#define BF03_ON   0x20
#define BF03_off     0
#define BF04_ON   0x10
#define BF04_off     0
#define BF05_ON   0x08
#define BF05_off     0
#define BF06_ON   0x04
#define BF06_off     0
#define BF07_ON   0x02
#define BF07_off     0
#define BF08_ON   0x01
#define BF08_off     0
#define BF09_ON   0x80
#define BF09_off     0
#define BF10_ON   0x40
#define BF10_off     0
#define BF11_ON   0x20
#define BF11_off     0
#define BF12_ON   0x10
#define BF12_off     0
#define BF13_ON   0x08
#define BF13_off     0
#define BF14_ON   0x04
#define BF14_off     0
#define BF15_ON   0x02
#define BF15_off     0
#define BF16_ON   0x01
#define BF16_off     0
#define BF17_ON   0x80
#define BF17_off     0
#define BF18_ON   0x40
#define BF18_off     0
#define BF19_ON   0x20
#define BF19_off     0
#define BF20_ON   0x10
#define BF20_off     0
#define BF21_ON   0x08
#define BF21_off     0
#define BF22_ON   0x04
#define BF22_off     0
#define BF23_ON   0x02
#define BF23_off     0
#define BF24_ON   0x01
#define BF24_off     0
#define BF25_ON   0x80
#define BF25_off     0
#define BF26_ON   0x40
#define BF26_off     0
#define BF27_ON   0x20
#define BF27_off     0
#define BF28_ON   0x10
#define BF28_off     0
#define BF29_ON   0x08
#define BF29_off     0
#define BF30_ON   0x04
#define BF30_off     0
#define BF31_ON   0x02
#define BF31_off     0
#define BF32_ON   0x01
#define BF32_off     0
#define BF33_ON   0x80
#define BF33_off     0
#define BF34_ON   0x40
#define BF34_off     0
#define BF35_ON   0x20
#define BF35_off     0
#define BF36_ON   0x10
#define BF36_off     0
#define BF37_ON   0x08
#define BF37_off     0
#define BF38_ON   0x04
#define BF38_off     0
#define BF39_ON   0x02
#define BF39_off     0
#define BF40_ON   0x01
#define BF40_off     0
#define BF41_ON   0x80
#define BF41_off     0
#define BF42_ON   0x40
#define BF42_off     0
#define BF43_ON   0x20
#define BF43_off     0
#define BF44_ON   0x10
#define BF44_off     0
#define BF45_ON   0x08
#define BF45_off     0
#define BF46_ON   0x04
#define BF46_off     0
#define BF47_ON   0x02
#define BF47_off     0
#define BF48_ON   0x01
#define BF48_off     0
#define BF49_ON   0x80
#define BF49_off     0
#define BF50_ON   0x40
#define BF50_off     0
#define BF51_ON   0x20
#define BF51_off     0
#define BF52_ON   0x10
#define BF52_off     0
#define BF53_ON   0x08
#define BF53_off     0
#define BF54_ON   0x04
#define BF54_off     0
#define BF55_ON   0x02
#define BF55_off     0
#define BF56_ON   0x01
#define BF56_off     0
#define BF57_ON   0x80
#define BF57_off     0
#define BF58_ON   0x40
#define BF58_off     0
#define BF59_ON   0x20
#define BF59_off     0
#define BF60_ON   0x10
#define BF60_off     0
#define BF61_ON   0x08
#define BF61_off     0
#define BF62_ON   0x04
#define BF62_off     0
#define BF63_ON   0x02
#define BF63_off     0
#define BF64_ON   0x01
#define BF64_off     0

#define BF65_ON   0x80
#define BF65_off     0
#define BF66_ON   0x40
#define BF66_off     0
#define BF67_ON   0x20
#define BF67_off     0
#define BF68_ON   0x10
#define BF68_off     0
#define BF69_ON   0x08
#define BF69_off     0
#define BF70_ON   0x04
#define BF70_off     0
#define BF71_ON   0x02
#define BF71_off     0
#define BF72_ON   0x01
#define BF72_off     0
#define BF73_ON   0x80
#define BF73_off     0
#define BF74_ON   0x40
#define BF74_off     0
#define BF75_ON   0x20
#define BF75_off     0
#define BF76_ON   0x10
#define BF76_off     0
#define BF77_ON   0x08
#define BF77_off     0
#define BF78_ON   0x04
#define BF78_off     0
#define BF79_ON   0x02
#define BF79_off     0
#define BF80_ON   0x01
#define BF80_off     0
#define BF81_ON   0x80
#define BF81_off     0
#define BF82_ON   0x40
#define BF82_off     0
#define BF83_ON   0x20
#define BF83_off     0
#define BF84_ON   0x10
#define BF84_off     0
#define BF85_ON   0x08
#define BF85_off     0
#define BF86_ON   0x04
#define BF86_off     0
#define BF87_ON   0x02
#define BF87_off     0
#define BF88_ON   0x01
#define BF88_off     0
#define BF89_ON   0x80
#define BF89_off     0
#define BF90_ON   0x40
#define BF90_off     0
#define BF91_ON   0x20
#define BF91_off     0
#define BF92_ON   0x10
#define BF92_off     0
#define BF93_ON   0x08
#define BF93_off     0
#define BF94_ON   0x04
#define BF94_off     0
#define BF95_ON   0x02
#define BF95_off     0
#define BF96_ON   0x01
#define BF96_off     0
#define BF97_ON   0x80
#define BF97_off     0
#define BF98_ON   0x40
#define BF98_off     0
#define BF99_ON   0x20
#define BF99_off     0
#define BF100_ON   0x10
#define BF100_off     0
#define BF101_ON   0x08
#define BF101_off     0
#define BF102_ON   0x04
#define BF102_off     0
#define BF103_ON   0x02
#define BF103_off     0
#define BF104_ON   0x01
#define BF104_off     0
#define BF105_ON   0x80
#define BF105_off     0
#define BF106_ON   0x40
#define BF106_off     0
#define BF107_ON   0x20
#define BF107_off     0
#define BF108_ON   0x10
#define BF108_off     0
#define BF109_ON   0x08
#define BF109_off     0
#define BF110_ON   0x04
#define BF110_off     0
#define BF111_ON   0x02
#define BF111_off     0
#define BF112_ON   0x01
#define BF112_off     0
#define BF113_ON   0x80
#define BF113_off     0
#define BF114_ON   0x40
#define BF114_off     0
#define BF115_ON   0x20
#define BF115_off     0
#define BF116_ON   0x10
#define BF116_off     0
#define BF117_ON   0x08
#define BF117_off     0
#define BF118_ON   0x04
#define BF118_off     0
#define BF119_ON   0x02
#define BF119_off     0
#define BF120_ON   0x01
#define BF120_off     0
#define BF121_ON   0x80
#define BF121_off     0
#define BF122_ON   0x40
#define BF122_off     0
#define BF123_ON   0x20
#define BF123_off     0
#define BF124_ON   0x10
#define BF124_off     0
#define BF125_ON   0x08
#define BF125_off     0
#define BF126_ON   0x04
#define BF126_off     0
#define BF127_ON   0x02
#define BF127_off     0
#define BF128_ON   0x01
#define BF128_off     0


/*****************************************************************************/
/**                          BIT FIELD SYMBOLS                              **/
/*****************************************************************************/



/******************************************************/
/* DATA STRUCTURE FOR STORING DATA IN FINAL FORMAT... */
/******************************************************/
typedef struct
{
	// Current Complete Structure for an ISO8583 MasterCard type message
        unsigned char msgtype       [4];
        unsigned char bitmap_bin    [16];
        unsigned char pan_len       [2];
        unsigned char pan           [19];
        unsigned char proccode      [6];
        unsigned char amount        [12];
        unsigned char settle_amt    [12];
        unsigned char ch_bill_amt   [12];
        unsigned char gmt_time      [10];
        unsigned char conv_rate_stl [8];
        unsigned char conv_rate_chb [8];
        unsigned char stan          [6];
        unsigned char time          [6];
        unsigned char date          [4];
        unsigned char date_expd     [4];
        unsigned char date_settle   [4];
        unsigned char conv_date     [4];
        unsigned char sic_number    [4];
        unsigned char ctry_code     [3];
        unsigned char pan_ctry_cd   [3];
        unsigned char posentry      [3];
        unsigned char panext        [3];
        unsigned char nii           [2];
        unsigned char pos_condcd    [1];
        unsigned char acq_inst_len  [2];
        unsigned char acq_inst_id   [11];
        unsigned char fwd_inst_len  [2];
        unsigned char fwd_inst_id   [11];
        unsigned char track2_len    [2];
        unsigned char track2        [37];
        unsigned char rrn           [12];
        unsigned char authcode      [6];
        unsigned char responsecd    [2];
        unsigned char termid        [8];
        unsigned char cardaccid     [15];
        unsigned char addl_rsp_len  [2];
        unsigned char addlrspdata   [25];
        unsigned char track1_len    [2];
        unsigned char track1        [76];
        unsigned char addl_data_len [3];
        unsigned char addl_data     [100];
        unsigned char tran_curr_cd  [3];
        unsigned char settle_cur_cd [3];
        unsigned char card_curr_cd  [3];
        unsigned char emv_len       [3];
        unsigned char emv           [256];
        unsigned char addl_len      [3];
        unsigned char addlamts      [120];
        unsigned char advice_cd_len [3];
        unsigned char advice_code   [60];
        unsigned char pos_data_len  [3];
        unsigned char pos_data      [26];
        unsigned char inf_data_len  [3];
        unsigned char inf_data      [100];
        unsigned char banknet_len   [3];
        unsigned char banknet       [50];
        unsigned char nmi_code      [3];
        unsigned char orig_data     [42];
        unsigned char file_code     [1];
        unsigned char service_ind   [7];
        unsigned char security_code [8];
        unsigned char recv_len      [2];
        unsigned char recvid        [11];
        unsigned char file_name_len [2];
        unsigned char file_name     [17];
        unsigned char acct1_len     [2];
        unsigned char acct1         [28];
        unsigned char acct2_len     [2];
        unsigned char acct2         [28];
        unsigned char orig_msg_id   [1];
        unsigned char rec_data_len  [3];
        unsigned char record_data   [100];
        unsigned char mc_agent_len  [3];
        unsigned char mc_agent_id   [11];
        unsigned char addl_rec_len  [3];
        unsigned char addl_rec_data [50];
        unsigned char priv127_len   [3];
        unsigned char priv127       [100];

} ISO_MESSAGE, *pISO_MESSAGE;


typedef struct
{
    unsigned char data_len[2];
    unsigned char data[1500];
} BIT_63_STRUCT;

/* End of module RESPONSE.H */

