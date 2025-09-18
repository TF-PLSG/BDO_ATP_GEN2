/**************************************************************************************************
*  
* MODULE:      ISOUTIL.C
*
  Copyright (c) 2006, Hypercom, Inc.
  All Rights Reserved.
  
* TITLE:       ATP Standard - ISO Utilities.
*  
* DESCRIPTION: General purpose ISO standard utilities. 
*
* APPLICATION: multiple
*
* AUTHOR:  Unknown
**************************************************************************************************/
#include <stdio.h>
#include <string.h>

#include "basictyp.h"
#include "pte.h"
#include "pteipc.h"
#include "ptemsg.h"
#include "ntutils.h"
#include "txutils.h"

#include "global_definitions.h"
#include "genutil.h"  // Includes all function prototypes for generic utilities
#include "iso_utilities.h"  // Includes all function prototypes for ISO standard utilities
#include "general_utilities.h"

INT                reqidx;                  /* next open position in request buffer */
struct iso_msg_rec ISO_MESSAGE;
INT                bitmapidx;               /* offset of bitmap... */
BYTE               ReqRsp_Buffer [COMM_BUF_SZE];
INT                buflen;                  /* total size of ReqRsp_Buffer buffer */

INT                bHasTimeField;   
extern AUTH_TX auth_tx ;  

INT    invalid_msg_format;     /* flag used in parsing host message */                                      

/************************************************
  Bit Field Data Record
*************************************************/

static  struct bitfield_data BFData[] = {
   {
   bf_2pan,                      /* field */
   ISO_MESSAGE.pan_len,          /* data to move */
   1,                            /* # of bytes in pan len field */
   isoutil_CondPan,              /* conditional */
   isoutil_vBCDMove,             /* move routine */
   isoutil_RspMovevBCD           /* response routine */
   },
   {
   bf_3proc,                     /* field */
   ISO_MESSAGE.proccode,         /* data to move */
   3,                            /* # of bytes in iso8583 field */
   isoutil_Move_Always,          /* conditional */
   isoutil_GenericMove,          /* move routine */
   isoutil_RspMoveGeneric        /* response routine */
   },
   {
   bf_4amount,                   /* field */
   ISO_MESSAGE.amount_bin,       /* data to move */
   6,                            /* # of bytes in iso8583 field */
   isoutil_Move_Always,          /* conditional */
   isoutil_GenericMove,          /* move routine */
   isoutil_RspMoveGeneric        /* response routine */
   },
   {
   bf_11stan,                    /* field */
   ISO_MESSAGE.stan_bin,         /* data to move */
   3,                            /* # of bytes in iso8583 field */
   isoutil_Move_Always,          /* conditional */
   isoutil_GenericMove,          /* move routine */
   isoutil_RspMoveGeneric        /* response routine */
   },
   {
   bf_12time,                    /* field */
   ISO_MESSAGE.time_bin,         /* data to move */
   3,                            /* # of bytes in iso8583 field */
   isoutil_Move_IfThere,         /* conditional */
   isoutil_GenericMove,          /* move routine */
   isoutil_RspMoveGeneric        /* response routine */
   },
   {
   bf_13date,                    /* field */
   ISO_MESSAGE.date_bin,         /* data to move */
   2,                            /* # of bytes in iso8583 field */
   isoutil_Move_IfThere,         /* conditional */
   isoutil_GenericMove,          /* move routine */
   isoutil_RspMoveGeneric        /* response routine */
   },
   {
   bf_14expiry,                  /* field */
   ISO_MESSAGE.date_expd,        /* data to move */
   2,                            /* # of bytes in iso8583 field */
   isoutil_CondExpd,             /* conditional */
   isoutil_GenericMove,          /* move routine */
   isoutil_RspMoveGeneric        /* response routine */
   },
   {
   bf_15datestl,                 /* field */
   ISO_MESSAGE.date_settle,      /* data to move */
   2,                            /* # of bytes in iso8583 field */
   isoutil_Move_IfThere,         /* conditional */
   isoutil_GenericMove,          /* move routine */
   isoutil_RspMoveGeneric        /* response routine */
   },
   {
   bf_18sicnumber,               /* field */
   ISO_MESSAGE.mcc,              /* data to move */
   4,                            /* # of bytes in mcrd iso field */
   isoutil_Move_IfThere,         /* conditional */
   isoutil_GenericMove,          /* move routine */
   isoutil_RspMoveGeneric        /* response routine */
   },
   {
   bf_22pose,                    /* field */
   ISO_MESSAGE.posentry,         /* data to move */
   2,                            /* # of bytes in iso8583 field */
   isoutil_Move_Always,          /* mandatory */
   isoutil_GenericMove,          /* move routine */
   isoutil_RspMoveGeneric        /* response routine */
   },
   {
   bf_23panseqnum,               /* field */
   ISO_MESSAGE.panseqnum,        /* data to move */
   2,                            /* # of bytes in iso8583 field */
   isoutil_Move_IfThere,         /* conditional */
   isoutil_GenericMove,          /* move routine */
   isoutil_RspMoveGeneric        /* response routine */
   },
   {
   bf_24nii,                     /* field */
   ISO_MESSAGE.nii,              /* data to move */
   2,                            /* # of bytes in iso8583 field */
   isoutil_Move_Always,          /* conditional */
   isoutil_GenericMove,          /* move routine */
   isoutil_RspMoveGeneric        /* response routine */
   },
   {
   bf_25pocc,                    /* field */
   ISO_MESSAGE.pos_condcd,       /* data to move */
   1,                            /* # of bytes in iso8583 field */
   isoutil_Move_Always,          /* conditional */
   isoutil_GenericMove,          /* move routine */
   isoutil_RspMoveGeneric        /* response routine */
   },
   {
   bf_35track2,                  /* field */
   ISO_MESSAGE.track2_len,       /* data to move */
   1,                            /* # of bytes in track2 len field */
   isoutil_Move_IfThere,         /* conditional */
   isoutil_vBCDMove,             /* move routine */
   isoutil_RspMovevBCD           /* response routine */
   },
   {
   bf_37rrn,                     /* field */
   ISO_MESSAGE.rrn,              /* data to move */
   12,                           /* # of bytes in iso8583 field */
   isoutil_Move_IfThere,         /* conditional */
   isoutil_GenericMove,          /* move routine */
   isoutil_RspMoveGeneric        /* response routine */
   },
   {
   bf_38authcode,                /* field */
   ISO_MESSAGE.authcode,         /* data to move */
   6,                            /* # of bytes in iso8583 field */
   isoutil_Move_IfThere,         /* conditional */
   isoutil_GenericMove,          /* move routine */
   isoutil_RspMoveGeneric        /* response routine */
   },
   {
   bf_39respcode,                /* field */
   ISO_MESSAGE.responsecd,       /* data to move */
   2,                            /* # of bytes in iso8583 field */
   isoutil_Move_IfThere,         /* conditional */
   isoutil_GenericMove,          /* move routine */
   isoutil_RspMoveGeneric        /* response routine */
   },
   {
   bf_41termid,                  /* field */
   ISO_MESSAGE.termid,           /* data to move */
   8,                            /* # of bytes in iso8583 field */
   isoutil_Move_Always,          /* conditional */
   isoutil_GenericMove,          /* move routine */
   isoutil_RspMoveGeneric        /* response routine */
   },
   {
   bf_42cardacc,                 /* field */
   ISO_MESSAGE.cardaccid,        /* data to move */
   15,                           /* # of bytes in iso8583 field */
   isoutil_Move_Always,          /* conditional */
   isoutil_GenericMove,          /* move routine */
   isoutil_RspMoveGeneric        /* response routine */
   },
   {
   bf_43cardaccloc,              /* field */
   ISO_MESSAGE.ignore,      /* data to move */
   40,                           /* # of bytes in  iso8583 field */
   isoutil_Move_IfThere,         /* conditional */
   isoutil_GenericMove,          /* move routine */
   isoutil_RspIgnore             /* response routine */
   },
   {
   bf_45track1,                  /* field */
   ISO_MESSAGE.track1_len,       /* data to move */
   1,                            /* # of bytes in track1 len field */
   isoutil_CondTrack1,           /* conditional */
   isoutil_vDataMove,            /* move routine */
   isoutil_RspMovevData          /* response routine */
   },
   {
   bf_48adddata,                 /* field */
   ISO_MESSAGE.adddata_len,      /* data to move */
   2,                            /* # of bytes in iso8583 field */
   isoutil_Move_IfThere,         /* conditional */
   isoutil_vDataMove,            /* move routine */
   isoutil_RspMovevData          /* response routine */
   },
   {
   bf_52pinblock,                /* field */
   ISO_MESSAGE.pin_block,        /* data to move */
   8,                            /* # of bytes in iso8583 field */
   isoutil_Move_IfThere,         /* conditional */
   isoutil_GenericMove,          /* move routine */
   isoutil_RspMoveGeneric        /* response routine */
   },
   {
   bf_53secctl,                /* field */
   ISO_MESSAGE.sec_ctl,        /* data to move */
   8,                            /* # of bytes in iso8583 field */
   isoutil_Move_IfThere,         /* conditional */
   isoutil_GenericMove,          /* move routine */
   isoutil_RspMoveGeneric        /* response routine */
   },
   {
   bf_54addlamts,                /* field */
   ISO_MESSAGE.addl_len,         /* data to move */
   2,                            /* # of bytes in addl len field */
   isoutil_Move_IfThere,         /* conditional */
   isoutil_vDataMove,            /* move routine */
   isoutil_RspMovevData          /* response routine */
   },
   {
   bf_55ICCdata,                 /* field */
   ISO_MESSAGE.ICCdata_len,      /* data to move */
   2,                            /* # of bytes in addl len field */
   isoutil_Move_IfThere,         /* conditional */
   isoutil_vDataMove,            /* move routine */
   isoutil_RspMovevData          /* response routine */
   },
   {
   bf_60priv60,                  /* field */
   ISO_MESSAGE.priv60_len,       /* data to move */
   2,                            /* # of bytes in priv60 len field */
   isoutil_Move_IfThere,         /* conditional */
   isoutil_vDataMove,            /* move routine */
   isoutil_RspMovevData          /* response routine */
   },
   {
   bf_61priv61,                  /* field */
   ISO_MESSAGE.priv61_len,       /* data to move */
   2,                            /* # of bytes in priv61 len field */
   isoutil_Move_IfThere,         /* conditional */
   isoutil_vDataMove,            /* move routine */
   isoutil_RspMovevData          /* response routine */
   },
   {
   bf_62priv62,                  /* field */
   ISO_MESSAGE.priv62_len ,       /* data to move */
   2,                            /* # of bytes in priv62 len field */
   isoutil_Move_IfThere,         /* conditional */
   isoutil_vDataMove,            /* move routine */
   isoutil_RspMovevData          /* response routine */
   },
   {
   bf_63priv63,                  /* field */
   ISO_MESSAGE.priv63_len,       /* data to move */
   2,                            /* # of bytes in priv63 len field */
   isoutil_Move_IfThere,         /* conditional */
   isoutil_vDataMove,            /* move routine */
   isoutil_RspMovevData          /* response routine */
   },

   {
   bf_done,                      /* field */
   NULL,                         /* data to move */
   99,                           /* # of bytes in iso8583 field */
   NULL,                         /* conditional */
   NULL,                         /* move routine */
   NULL                          /* response routine */
   }
   }; /* end BFData */



#define N_BFData ( sizeof(BFData) / sizeof (struct bitfield_data) )


      
   static  struct trandef TRANTABLE[] = {
      
      {
      /********************************************
         A U T H O R I Z A T I O N   R E S P O N S E 
      ********************************************/
      AUTH_AUTHORIZATION_RESPONSE,
      0x01,0x10,              /* message id */
      0x00,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_3proc,		/* MAND */
      bf_11stan,	   /* MAND */
      bf_12time,	   /* MAND */
      bf_13date,	   /* MAND */
      bf_24nii,		/* MAND */
	   bf_37rrn,		/* MAND */
	   bf_38authcode, /* OPT  */
      bf_39respcode, /* MAND */
      bf_41termid,	/* MAND */
      bf_55ICCdata,  /* OPT  */
      bf_63priv63,	/* OPT  */
      bf_done        /* end of fields to move */
      },

	  {   
      /********************************************
         B A L A N C E   I N Q U I R Y   R E S P O N S E 
      ********************************************/
      AUTH_BALANCE_INQUIRY_RESPONSE,
      0x01,0x10,              /* message id */
      0x31,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_3proc,		/* MAND */
      bf_4amount,    /* MAND */
      bf_11stan,	   /* MAND */
      bf_12time,	   /* MAND */
      bf_13date,	   /* MAND */
      bf_24nii,		/* MAND */
	   bf_37rrn,		/* MAND */
	   bf_38authcode, /* OPT  */
      bf_39respcode, /* MAND */
      bf_41termid,	/* MAND */
	   bf_54addlamts, /* OPT  */
      bf_55ICCdata,  /* OPT  */
      bf_63priv63,	/* OPT  */
      bf_done        /* end of fields to move */
      },

	  {
	  /******************************************************
	  C A R D  V E R I F I C A T I O N  R E S P O N S E
	  *******************************************************/ 
	  AUTH_CARD_VERIFICATION_RESPONSE,
	  0x01,0x10,               /* message id */ 
	  0x38,0x00,0x00,          /* processing code */ 
	  /**** OPTIONS ****/
	  topt1_postadv,
	  topt2_referral+topt2_reversal+topt2_floor, /**** BIT FIELDS ****/
	  bf_3proc,       /* MAND */
	  bf_11stan,      /* MAND */
 	  bf_12time,      /* MAND */
	  bf_13date,      /* MAND */
	  bf_24nii,       /* MAND */
	  bf_37rrn,       /* MAND */
	  bf_38authcode,  /* OPT  */
	  bf_39respcode,  /* MAND */
     bf_41termid,    /* MAND */
     bf_55ICCdata,   /* OPT  */
	  bf_63priv63,    /* OPT  */
	  bf_done         /* end of fields to move */
	  },

	  {   
      /********************************************
         P R E   A U T H O R I Z A T I O N   R E S P O N S E 
      ********************************************/
      AUTH_PRE_AUTHORIZATION_RESPONSE,
      0x01,0x10,              /* message id */
      0x30,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_3proc,		/* MAND */
	   bf_4amount,	   /* OPT  */
      bf_11stan,	   /* MAND */
      bf_12time,	   /* MAND */
      bf_13date,	   /* MAND */
      bf_24nii,		/* MAND */
	   bf_37rrn,		/* MAND */
	   bf_38authcode, /* OPT  */
      bf_39respcode, /* MAND */
      bf_41termid,	/* MAND */
      bf_55ICCdata,  /* OPT  */
      bf_63priv63,	/* OPT  */
      bf_done        /* end of fields to move */
      },

      { 
      /********************************************
         R E L O A D   R E S P O N S E
      ********************************************/
      AUTH_RELOAD_RESPONSE,
      0x02,0x10,              /* message id */
      0x00,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_3proc,		/* MAND */
      bf_4amount,    /* MAND */
      bf_11stan,	   /* MAND */
      bf_12time,	   /* MAND */
      bf_13date,	   /* MAND */
      bf_24nii,		/* MAND */
      bf_37rrn,		/* MAND */
	   bf_38authcode, /* OPT  */
      bf_39respcode, /* MAND */
      bf_41termid,	/* MAND */	  
      bf_63priv63,	/* OPT  */
      bf_done        /* end of fields to move */
      },

      { 
      /********************************************
         R E L O A D   R E S P O N S E
      ********************************************/
      AUTH_RELOAD_REPEAT_RESPONSE,
      0x02,0x11,              /* message id */
      0x00,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_3proc,		/* MAND */
      bf_4amount,    /* MAND */
      bf_11stan,	   /* MAND */
      bf_12time,	   /* MAND */
      bf_13date,	   /* MAND */
      bf_24nii,		/* MAND */
      bf_37rrn,		/* MAND */
	   bf_38authcode, /* OPT  */
      bf_39respcode, /* MAND */
      bf_41termid,	/* MAND */	  
      bf_63priv63,	/* OPT  */
      bf_done        /* end of fields to move */
      },

      { 
      /******************************************************
         R E L O A D  C O M P L E T I O N   R E S P O N S E
      *******************************************************/
      AUTH_RELOAD_CONFIRM_RESPONSE,
      0x02,0x12,              /* message id */
      0x00,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_3proc,		/* MAND */
      bf_4amount,    /* MAND */
      bf_11stan,	   /* MAND */
      bf_12time,	   /* MAND */
      bf_13date,	   /* MAND */
      bf_24nii,		/* MAND */
      bf_37rrn,		/* MAND */
	   bf_38authcode, /* OPT  */
      bf_39respcode, /* MAND */
      bf_41termid,	/* MAND */	  
      bf_63priv63,	/* OPT  */
      bf_done        /* end of fields to move */
      },

      { 
      /********************************************
         S A L E   R E S P O N S E
      ********************************************/
      AUTH_SALE_RESPONSE,
      0x02,0x10,              /* message id */
      0x00,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_3proc,		/* MAND */
      bf_11stan,	   /* MAND */
      bf_12time,	   /* MAND */
      bf_13date,	   /* MAND */
      bf_24nii,		/* MAND */
      bf_37rrn,		/* MAND */
	   bf_38authcode, /* OPT  */
      bf_39respcode, /* MAND */
      bf_41termid,	/* MAND */
      bf_55ICCdata,  /* OPT  */
      bf_63priv63,	/* OPT  */
      bf_done        /* end of fields to move */
      },

      { 
      /********************************************
         D E F E R R E D   R E S P O N S E
      ********************************************/
      AUTH_DEFERRED_PURCHASE_AUTHORIZATION_RESPONSE,
      0x02,0x10,              /* message id */
      0x00,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_3proc,		/* MAND */
      bf_11stan,	   /* MAND */
      bf_12time,	   /* MAND */
      bf_13date,	   /* MAND */
      bf_24nii,		/* MAND */
      bf_37rrn,		/* MAND */
      bf_38authcode, /* OPT  */
      bf_39respcode, /* MAND */
      bf_41termid,	/* MAND */
      bf_55ICCdata,  /* OPT  */
      bf_63priv63,	/* OPT  */
      bf_done        /* end of fields to move */
      },

      { 
      /********************************************
         S A L E   C A S H   R E S P O N S E
      ********************************************/
      AUTH_SALE_CASH_RESPONSE,
      0x02,0x10,              /* message id */
      0x09,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_3proc,		/* MAND */
      bf_11stan,	   /* MAND */
      bf_12time,	   /* MAND */
      bf_13date,	   /* MAND */
      bf_24nii,		/* MAND */
      bf_37rrn,		/* MAND */
	   bf_38authcode, /* OPT  */
      bf_39respcode, /* MAND */
      bf_41termid,	/* MAND */
      bf_55ICCdata,   /* OPT  */
      bf_63priv63,	/* OPT  */
      bf_done        /* end of fields to move */
      },

      {
      /********************************************
         Q U A S I  C A S H   R E S P O N S E  
      ********************************************/
      AUTH_QUASI_CASH_RESPONSE,
      0x02,0x10,              /* message id */
      0x11,0x00,0x00,         /* processing code (EFT MASTER) */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_3proc,		/* MAND */
      bf_11stan,	   /* MAND */
      bf_12time,	   /* MAND */
      bf_13date,	   /* MAND */
      bf_24nii,		/* MAND */
      bf_37rrn,		/* MAND */
	   bf_38authcode, /* OPT  */
      bf_39respcode, /* MAND */
      bf_41termid,	/* MAND */
      bf_55ICCdata,  /* OPT  */
      bf_63priv63,	/* OPT  */
      bf_done        /* end of fields to move */
      },

      {
      /********************************************
         V O I D   S A L E   R E S P O N S E 
      ********************************************/
      AUTH_VOID_SALE_RESPONSE,
      0x02,0x10,              /* message id */
      0x02,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_credit+topt1_postadv,
      topt2_reversal,
      /**** BIT FIELDS ****/
      bf_3proc,		/* MAND */
      bf_11stan,	   /* MAND */
      bf_12time,	   /* MAND */
      bf_13date,	   /* MAND */
      bf_24nii,		/* MAND */
      bf_37rrn,		/* MAND */
	   bf_38authcode, /* OPT  */
	   bf_39respcode, /* MAND */
      bf_41termid,	/* MAND */
      bf_55ICCdata,  /* OPT  */
      bf_63priv63,   /* OPT  */
      bf_done        /* end of fields to move */
      },

      {
      /********************************************
         V O I D   R E L O A D    R E S P O N S E 
      ********************************************/
      AUTH_VOID_RELOAD_RESPONSE,
      0x02,0x10,              /* message id */
      0x02,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_credit+topt1_postadv,
      topt2_reversal,
      /**** BIT FIELDS ****/
      bf_3proc,		/* MAND */
      bf_11stan,	   /* MAND */
      bf_12time,	   /* MAND */
      bf_13date,	   /* MAND */
      bf_24nii,		/* MAND */
      bf_37rrn,		/* MAND */
	   bf_38authcode, /* OPT  */
	   bf_39respcode, /* MAND */
      bf_41termid,	/* MAND */
      bf_55ICCdata,  /* OPT  */
      bf_63priv63,   /* OPT  */
      bf_done        /* end of fields to move */
      },

      {
      /********************************************
         R E F U N D  R E S P O N S E
      ********************************************/
      AUTH_REFUND_RESPONSE,
      0x02,0x10,              /* message id */
      0x20,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_credit+topt1_postadv,
      topt2_reversal,
      /**** BIT FIELDS ****/
      bf_3proc,		/* MAND */
      bf_11stan,	   /* MAND */
      bf_12time,	   /* MAND */
      bf_13date,	   /* MAND */
      bf_24nii,		/* MAND */
      bf_37rrn,		/* MAND */
      bf_38authcode, /* OPT  */
      bf_39respcode, /* MAND */
      bf_41termid,	/* MAND */
      bf_55ICCdata,  /* OPT  */
      bf_63priv63,	/* OPT  */
      bf_done        /* end of fields to move */
      },

      {
      /********************************************
         V O I D   R E F U N D   R E S P O N S E 
      ********************************************/
      AUTH_VOID_REFUND_RESPONSE,
      0x02,0x10,              /* message id */
      0x22,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_reversal,
      /**** BIT FIELDS ****/
      bf_3proc,		/* MAND */
      bf_11stan,	   /* MAND */
      bf_12time,	   /* MAND */
      bf_13date,	   /* MAND */
      bf_24nii,		/* MAND */
      bf_37rrn,		/* MAND */
	   bf_38authcode, /* OPT  */
	   bf_39respcode, /* MAND */
      bf_41termid,	/* MAND */
      bf_55ICCdata,  /* OPT  */
      bf_63priv63,	/* OPT  */
      bf_done        /* end of fields to move */
      },

      { 
      /********************************************
         C A S H   A D V A N C E    R E S P O N S E
      ********************************************/
      AUTH_CASH_ADVANCE_RESPONSE,
      0x02,0x10,              /* message id */
      0x01,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_3proc,		/* MAND */
      bf_11stan,	   /* MAND */
      bf_12time,	   /* MAND */
      bf_13date,	   /* MAND */
      bf_24nii,		/* MAND */
      bf_37rrn,		/* MAND */
	   bf_38authcode, /* OPT  */
      bf_39respcode, /* MAND */
      bf_41termid,	/* MAND */
      bf_55ICCdata,  /* OPT  */
      bf_63priv63,	/* OPT  */
      bf_done        /* end of fields to move */
      },


	  { 
      /********************************************
         P A Y M E N T    R E S P O N S E
      ********************************************/
      AUTH_PAYMENT_RESPONSE,
      0x02,0x10,              /* message id */
      0x40,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_3proc,		/* MAND */
      bf_11stan,	   /* MAND */
      bf_12time,	   /* MAND */
      bf_13date,	   /* MAND */
      bf_24nii,		/* MAND */
      bf_37rrn,		/* MAND */
	   bf_38authcode, /* OPT  */
      bf_39respcode, /* MAND */
      bf_41termid,	/* MAND */
      bf_55ICCdata,  /* OPT  */
      bf_63priv63,	/* OPT  */
      bf_done        /* end of fields to move */
      },

	  {
      /********************************************
         O F F L I N E   VOID S A L E   R E S P O N S E
      ********************************************/
      AUTH_OFFLINE_VOID_SALE_RESPONSE,
      0x02,0x30,              /* message id */
      0x02,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_credit+topt1_postadv,
      topt2_reversal,
      /**** BIT FIELDS ****/
      bf_3proc,		/* MAND */
      bf_11stan,	   /* MAND */	
      bf_24nii,		/* MAND */
      bf_37rrn,		/* MAND */
      bf_39respcode, /* MAND */
      bf_41termid,	/* MAND */
      bf_55ICCdata,  /* OPT  */
     // bf_63priv63,	/* OPT  */
      bf_done        /* end of fields to move */
      },

	  {
      /******************************************************
         O F F L I N E   V O I D S A L E   R E S P O N S E
      *******************************************************/
      AUTH_OFFLINE_VOID_REFUND_RESPONSE,
      0x02,0x30,              /* message id */
      0x02,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_credit+topt1_postadv,
      topt2_reversal,
      /**** BIT FIELDS ****/
      bf_3proc,		/* MAND */
      bf_11stan,	   /* MAND */	
      bf_24nii,		/* MAND */
      bf_37rrn,		/* MAND */
      bf_39respcode, /* MAND */
      bf_41termid,	/* MAND */
      bf_55ICCdata,  /* OPT  */
      //bf_63priv63,	/* OPT  */
      bf_done        /* end of fields to move */
      },



      
	  {
      /********************************************
         O F F L I N E   S A L E   R E S P O N S E
      ********************************************/
      AUTH_OFFLINE_SALE_RESPONSE,
      0x02,0x30,              /* message id */
      0x00,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_credit+topt1_postadv,
      topt2_reversal,
      /**** BIT FIELDS ****/
      bf_3proc,		/* MAND */
      bf_11stan,	   /* MAND */	
      bf_24nii,		/* MAND */
      bf_37rrn,		/* MAND */
      bf_39respcode, /* MAND */
      bf_41termid,	/* MAND */
      bf_55ICCdata,  /* OPT  */
    //  bf_63priv63,	/* OPT  */
      bf_done        /* end of fields to move */
      },

	  {   
      /********************************************
         S A L E S  C O M P L E T I O N   R E S P O N S E
      ********************************************/
      AUTH_SALES_COMPLETION_RESPONSE,
      0x02,0x30,              /* message id */
      0x00,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_credit+topt1_postadv,
      topt2_reversal,
      /**** BIT FIELDS ****/
      bf_3proc,		/* MAND */
      bf_11stan,	   /* MAND */	
      bf_24nii,		/* MAND */
      bf_37rrn,		/* MAND */
      bf_39respcode, /* MAND */
      bf_41termid,	/* MAND */
      bf_55ICCdata,  /* OPT  */
  //    bf_63priv63,	/* OPT  */
      bf_done        /* end of fields to move */
      },

      {
      /********************************************
         S A L E   A D J U S T M E N T  R E S P O N S E  
      ********************************************/
      AUTH_SALE_ADJUSTMENT_RESPONSE,
      0x02,0x30,              /* message id */
      0x02,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_credit+topt1_postadv,
      topt2_reversal,
      /**** BIT FIELDS ****/
      bf_3proc,		/* MAND */
      bf_11stan,	   /* MAND */
      bf_24nii,		/* MAND */
	   bf_37rrn,		/* MAND */
	   bf_39respcode, /* MAND */
      bf_41termid,	/* MAND */
      bf_55ICCdata,  /* OPT  */
	   bf_63priv63,   /* OPT  */
      bf_done        /* end of fields to move */
      },
      
	  {
      /********************************************
         O F F L I N E   R E F U N D   R E S P O N S E
      ********************************************/
      AUTH_OFFLINE_REFUND_RESPONSE,
      0x02,0x30,              /* message id */
      0x20,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_credit+topt1_postadv,
      topt2_reversal,
      /**** BIT FIELDS ****/
      bf_3proc,		/* MAND */
	   bf_4amount,	   /* OPT  */
      bf_11stan,	   /* MAND */	
      bf_24nii,		/* MAND */
      bf_37rrn,		/* MAND */
      bf_39respcode, /* MAND */
      bf_41termid,	/* MAND */
      bf_55ICCdata,  /* OPT  */
      bf_63priv63,	/* OPT  */
      bf_done        /* end of fields to move */
      },

	  {
      /********************************************
         R E F U N D    A D J U S T M E N T  R E S P O N S E   
      ********************************************/
      AUTH_REFUND_ADJUSTMENT_RESPONSE,
      0x02,0x30,              /* message id */
      0x22,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_credit+topt1_postadv,
      topt2_reversal,
      /**** BIT FIELDS ****/
      bf_3proc,		/* MAND */
      bf_11stan,	   /* MAND */
      bf_24nii,		/* MAND */
	   bf_37rrn,		/* MAND */
	   bf_39respcode, /* MAND */
      bf_41termid,	/* MAND */
      bf_55ICCdata,  /* OPT  */
      bf_63priv63,   /* OPT  */
      bf_done        /* end of fields to move */
      },

	  {   
      /********************************************
         P A Y M E N T   A D J U S T M E N T   R E S P O N S E   
      ********************************************/
      AUTH_PAYMENT_ADJUSTMENT_RESPONSE,
      0x02,0x30,              /* message id */
      0x42,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_credit+topt1_postadv,
      topt2_reversal,
      /**** BIT FIELDS ****/
      bf_3proc,		/* MAND */
      bf_11stan,	   /* MAND */
      bf_24nii,		/* MAND */
	   bf_37rrn,		/* MAND */
	   bf_39respcode, /* MAND */
      bf_41termid,   /* MAND */
      bf_55ICCdata,  /* OPT  */
      bf_63priv63,   /* OPT  */
      bf_done        /* end of fields to move */
      },
    
	  {
      /********************************************
         B A T C H   U P L O A D  R E S P O N S E 
      ********************************************/
      AUTH_BATCH_UPLOAD_RESPONSE,
      0x03,0x30,              /* message id */
      0x00,0x00,0x00,         /* processing code (use original) */
      /**** OPTIONS ****/
      0x00,
      topt2_org_proccode,     /* use original processing code */
      /**** BIT FIELDS ****/
      bf_3proc,		/* MAND */
      bf_11stan,	   /* MAND */
      bf_12time,	   /* MAND */
      bf_13date,	   /* MAND */
	  bf_24nii,		/* MAND */
	  bf_37rrn,		/* MAND */
	  bf_39respcode, /* MAND */
      bf_41termid,	/* MAND */
	  bf_42cardacc,  /* MAND */
      //bf_55ICCdata,  /* OPT  */
      //bf_63priv63,   /* OPT  */
      bf_done        /* end of fields to move */
      },

      {
      /********************************************
         E M V   T C   U P L O A D  R E S P O N S E 
      ********************************************/
      AUTH_EMV_UPLOAD_RESPONSE,
      0x03,0x30,              /* message id */
      0x00,0x00,0x00,         /* processing code (use original) */
      /**** OPTIONS ****/
      0x00,
      topt2_org_proccode,     /* use original processing code */
      /**** BIT FIELDS ****/
      bf_3proc,      /* MAND */
      bf_11stan,     /* MAND */
      bf_12time,     /* MAND */
      bf_13date,     /* MAND */
      bf_24nii,      /* MAND */
      bf_37rrn,      /* MAND */
      bf_39respcode, /* MAND */
      bf_41termid,   /* MAND */
      bf_done        /* end of fields to move */
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
      topt2_org_proccode,      /* use original processing code */
      /**** BIT FIELDS ****/
      bf_3proc,		/* MAND */
      bf_11stan,	   /* MAND */
      bf_12time,	   /* MAND */
      bf_13date,	   /* MAND */
      bf_24nii,		/* MAND */
      bf_37rrn,      /* MAND */
      bf_38authcode, /* OPT  */
      bf_39respcode, /* MAND */
      bf_41termid,	/* MAND */
      bf_55ICCdata,  /* OPT  */
      bf_63priv63,   /* OPT  */
      bf_done        /* end of fields to move */
      },

      {  
      /********************************************
         S E T T L E   R E S P O N S E 
      ********************************************/
      AUTH_SETTLE_RESPONSE,
      0x05,0x10,              /* message id */
      0x92,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_preadv,
      topt2_referral,
      /**** BIT FIELDS ****/
      bf_3proc,		/* MAND */
      bf_11stan,	   /* MAND */
	   bf_12time,	   /* MAND */
      bf_13date,	   /* MAND */
      bf_24nii,		/* MAND */
	   bf_37rrn,		/* MAND */
	   bf_39respcode, /* MAND */
      bf_41termid,	/* MAND */
      bf_63priv63,	/* MAND  */
      bf_done        /* end of fields to move */
      },

      {   
      /********************************************
         S E T T L E   T R A I L E R   R E S P O N S E
      ********************************************/
      AUTH_SETTLE_TRAILER_RESPONSE,
      0x05,0x10,              /* message id */
      0x96,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      0x00,
      topt2_referral,
      /**** BIT FIELDS ****/
      bf_3proc,		/* MAND */
      bf_11stan,	   /* MAND */
	   bf_12time,	   /* MAND */
      bf_13date,	   /* MAND */
      bf_24nii,		/* MAND */
	   bf_37rrn,		/* MAND */
	   bf_39respcode, /* MAND */
      bf_41termid,	/* MAND */
      bf_63priv63,	/* MAND  */
      bf_done        /* end of fields to move */
      },

      {
	  /********************************************
         P I N   C H A N G E   R E S P O N S E 
      ********************************************/
      AUTH_PIN_CHANGE_RESPONSE,
      0x08,0x10,              /* message id */
      0x90,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      0x00,
      topt2_referral,
      /**** BIT FIELDS ****/
      bf_3proc,		/* MAND */
      bf_11stan,	   /* MAND */
	   bf_12time,	   /* MAND */
      bf_13date,	   /* MAND */
      bf_24nii,		/* MAND */
	   bf_37rrn,		/* MAND */
      bf_38authcode, /* OPT  */
	   bf_39respcode, /* MAND */
      bf_41termid,	/* MAND */
      bf_42cardacc,  /* MAND */
      bf_done        /* end of fields to move */
      },

      {
	  /********************************************
         S T A T I S T I C S   R E S P O N S E 
      ********************************************/
      AUTH_STATISTICS_RESPONSE,
      0x08,0x10,              /* message id */
      0x91,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      0x00,
      topt2_referral,
      /**** BIT FIELDS ****/
      bf_3proc,		/* MAND */
      bf_11stan,	   /* MAND */
	   bf_12time,	   /* MAND */
      bf_13date,	   /* MAND */
      bf_24nii,		/* MAND */
	   bf_39respcode, /* MAND */
      bf_41termid,	/* MAND */
      bf_done        /* end of fields to move */
      },

	  {  
     /********************************************
         T E S T   R E S P O N S E 
      ********************************************/
      AUTH_TEST_RESPONSE,
      0x08,0x10,              /* message id */
      0x99,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      0x00,
      0x00,
      /**** BIT FIELDS ****/
      bf_3proc,      /* MAND */
      bf_12time,   /* MAND */
      bf_13date,  /* MAND */
      bf_24nii,      /* MAND */
      bf_41termid,   /* MAND */
      bf_done      /* end of fields to move */
      },

      {
	  /********************************************
         LOGON RESPONSE
      ********************************************/
      AUTH_LOGON_RESPONSE,
      0x08,0x10,              /* message id */
      0x92,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      0x00,
      topt2_referral,
      /**** BIT FIELDS ****/
      bf_3proc,		/* MAND */
      bf_11stan,	   /* MAND */
	   bf_12time,	   /* MAND */
      bf_13date,	   /* MAND */
      bf_24nii,		/* MAND */
	   bf_39respcode, /* MAND */
      bf_41termid,	/* MAND */
	   bf_62priv62,	/* MAND */
      bf_done        /* end of fields to move */
      }

   };

#define N_TRANENTRY (sizeof(TRANTABLE) / sizeof(struct trandef))


const struct isobit BITTAB[64+1] =
   {
   { 0, 0 },      /* zero justify */
   { 0, 0x80 },   /* bit 1 */    { 0, 0x40 },   /* bit 2 */    { 0, 0x20 },   /* bit 3 */    { 0, 0x10 },   /* bit 4 */
   { 0, 0x08 },   /* bit 5 */    { 0, 0x04 },   /* bit 6 */    { 0, 0x02 },   /* bit 7 */    { 0, 0x01 },   /* bit 8 */
   { 1, 0x80 },   /* bit 9 */    { 1, 0x40 },   /* bit 10 */   { 1, 0x20 },   /* bit 11 */   { 1, 0x10 },   /* bit 12 */
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
   { 7, 0x08 },   /* bit 61 */   { 7, 0x04 },   /* bit 62 */   { 7, 0x02 },   /* bit 63 */   { 7, 0x01 }   /* bit 64 */
   };   
      

/*****************************************************************************

   Function:         isoutil_build_terminal_response

   Description:      This function builds the terminal response (ReqRsp_Buffer)
   
   Author:           Unknown

   Inputs:           trantype - transaction type

   Outputs:          None

   Return values:    None

   Modified by:      BGable      6/30/98

******************************************************************************/
extern void isoutil_build_terminal_response ( INT trantype)
{

   struct            trandef *pTranDef;
   struct            bitfield_data *pBF;
   INT               bfidx;       /* index to bitfields for this transaction */


   pTranDef = isoutil_Find_Tran_Table(trantype);   /* get pointer to tran table */
   
   if (pTranDef == NULL_PTR)						/* no tran table is found; EP */
   {
      dcpimp_log_message( 1, 2, "Response transaction message type is not found",
    		  	  	  	  	  	  " isoutil_build_terminal_response ", 0 );
	   return;
   }

   /*-----------------------------------------*/
   /* Copy in Message Id for this transaction */
   /*-----------------------------------------*/
   memset(ReqRsp_Buffer, 0, COMM_BUF_SZE);   /* zero out the buffer */
   bitmapidx = 2;                            /* point to bitmap     */
   reqidx    = 10;                           /* point past bitmap   */

  
   memcpy(&ReqRsp_Buffer[0], pTranDef->td_msgid, sizeof(pTranDef->td_msgid));

   /* Message type must be 0110 for responses to EFT Master. */
   if ( trantype > 100 )
   {
      if ( 0 == strcmp(auth_tx.TLF01_details.message_type,"0100") )
      {
         /* Set message type to '0110'. */
         memset( &ReqRsp_Buffer[0], 0x01, 1 );
         memset( &ReqRsp_Buffer[1], 0x10, 1 );
      }
   }

   /*-----------------------------------------------------*/
   /* for all requested bitfields for this transaction... */
   /*-----------------------------------------------------*/
   for (bfidx=0; bfidx < MAX_BITFIELD; bfidx++)
      {
      /*----------------------------*/
      /* if not end of bitfields... */
      /*----------------------------*/
      if (pTranDef->td_bitfields[bfidx] != bf_done)
         {
         /*---------------------------------------------------*/
         /* find pointer to bitfield info for this bitfield...*/
         /*---------------------------------------------------*/
         pBF = isoutil_Find_Bitfield_Table(pTranDef->td_bitfields[bfidx]);

         /*-----------------------------------------------------------*/
         /* if conditional routine returns true, move the bitfield... */
         /*-----------------------------------------------------------*/
         if ((pBF->bd_BFCond)(pBF->bd_fieldid,(pCHAR)&trantype) == true)
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
         }  /* end if */
      } /* end for */
   
   buflen = reqidx;

} /* end isoutil_build_terminal_response */


/*****************************************************************************

   Function:         isoutil_clear_priv63

   Description:      This function clears field 63 of the ISO_MESSAGE

   Author:           Unknown

   Inputs:           None

   Outputs:          None

   Return values:    None

   Modified by:      BGable      6/30/98

******************************************************************************/
extern void isoutil_clear_priv63()
{
	memset( ISO_MESSAGE.priv63, 0x00, sizeof(ISO_MESSAGE.priv63) );

} /* isoutil_clear_priv63 */


 /*****************************************************************************

   Function:         isoutil_CondExpd

   Description:      Determine whether there is data in the expiration date
                     ISO field

   Author:           Unknown

   Inputs:           There are two inputs, bitfield and trantype.  Neither of 
                     these inputs is used, but because this function is one of
                     many called using a table, the parameters must be in the
                     function definition.

   Outputs:        

   Return values:    true  -  expiration date field contains data 
                     false -  expiration date field doe not contain data 

   Modified by:      BGable      6/26/98

******************************************************************************/
extern INT isoutil_CondExpd(enum bitfields bitfield, pCHAR trantype)
{
   /* if track2 data here, no need for expdate field */
   if (ISO_MESSAGE.date_expd [1] != 0x00)    /* JH Apr 29  Fixed bug */
       return(true);
  
   return(false);

} /* end isoutil_CondExpd */


/*****************************************************************************

   Function:         isoutil_CondPan

   Description:      Determine whether there is data in the primary account
                     number ISO field

   Author:           Unknown

   Inputs:           bitfield    - the bit number in the bit map
                     p_trantype  - transaction type

   Outputs:

   Return values:    true  -  primary account number field contains data 
                     false -  primary account field doe not contain data 

   Modified by:      BGable      6/26/98

******************************************************************************/
extern INT isoutil_CondPan(enum bitfields bitfield, pCHAR p_trantype)
{
   /* JH Apr 29  changed procedure to only send the pan if it is there */
   if (ISO_MESSAGE.pan_len [0] != 0x00)   /* JH Apr 29  Fixed bug */
       return(isoutil_Move_IfThere(bitfield, p_trantype));
  
   return(false);

} /* end CondPan */


/*****************************************************************************

   Function:         isoutil_CondProc

   Description:      Determine whether the message type requires 
                     a processing code

   Author:           Unknown

   Inputs:           There are two inputs, bitfield and trantype.  Neither of
                     these inputs is used, but because this function is one of
                     many called using a table, the parameters must be in the
                     function definition.

   Outputs:

   Return values:    true  -   processing code require
                     false -   processing code not required

   Modified by:      BGable      6/26/98

******************************************************************************/
extern INT isoutil_CondProc(enum bitfields bitfield, pCHAR trantype)
{
   if (ISO_MESSAGE.msgtype_bcd [0] != 0x04)
      return (true);

   return(false);

} /* end isoutil_CondProc */


/*****************************************************************************

   Function:         isoutil_CondTrack1

   Description:      Determine whether there is any track 1 data 
                     in the ISO field
  
   Author:           Unknown

   Inputs:           There are two inputs, bitfield and trantype.  Neither of 
                     these inputs is used, but because this function is one of 
                     many called using a table, the parameters must be in the
                     function definition.

   Outputs:

   Return values:    true  -   track 1 data available
                     false -   track 1 data not available

   Modified by:      BGable      6/26/98

******************************************************************************/
extern INT isoutil_CondTrack1 (enum bitfields bitfield, pCHAR trantype)
{
   if (ISO_MESSAGE.track1_len [0] != 0x00)   /* JH Apr 29  Fixed bug */
       return(true);

   return(false);

} /* end isoutil_CondTrack1 */


/*****************************************************************************

   Function:         isoutil_copy_if_there

   Description:      Copy the incoming string to the outgoing string, 
                     if there is data to copy.

   Author:           Unknown

   Inputs:           source_field   -  incoming string to be copied from
                     max_len        -  maximum length of the strings

   Outputs:          dest_string    -  outgoing string to be copied to

   Return values:    None

   Modified by:      BGable      6/26/98

******************************************************************************/
extern void isoutil_copy_if_there (pCHAR source_field, BYTE dest_string[], INT max_len)
{
   pCHAR             astr;
   INT               i = 0;

   astr = source_field;

   /* if ' ', field not there... */
   if ( (strlen(astr) == 0) && (genutil_blank_string(astr, max_len) == false))
      memcpy (dest_string, astr, max_len);

} /* isoutil_copy_if_there */


/*****************************************************************************

   Function:         isoutil_Find_Bitfield_Table

   Description:      This function uses the bitfield to determine which item
                     in the BFData to point the return value

   Author:           Unknown

   Inputs:           bitfield    - the bit number in the bit map

   Outputs:

   Return values:    returns a pointer to BFData array item associated with
                     the bitfield

   Modified by:      BGable      6/26/98

******************************************************************************/
extern struct bitfield_data *isoutil_Find_Bitfield_Table (enum bitfields bitfield)
{
	struct bitfield_data *pBF = NULL;      /* point to return record       */
	INT      i;							 /* loop index         */
	pBF = NULL_PTR;

	for (i=0; i <= N_BFData; i++)
	{
		if ( BFData[i].bd_fieldid == bitfield)
		{
			pBF = &BFData[i];
			return (pBF);
			break;
		}
	}
	return(pBF);
} /* end isoutil_Find_Bitfield_Table */


/*****************************************************************************

   Function:         isoutil_Find_Tran_Table

   Description:      This function determines which element of the tran table
                     should be used with the transaction type.

   Author:           Unknown

   Inputs:           trantype

   Outputs:          None

   Return values:    pointer to the tran table element

   Modified by:      BGable      6/30/98

******************************************************************************/
extern struct trandef *isoutil_Find_Tran_Table (INT trantype)
{
	struct trandef *ptrandef = NULL;  /* point to return record    */
	INT    i;                         /* loop index                */

	for (i=0; (i <= N_TRANENTRY) && (TRANTABLE[i].td_trantype > 0); i++)
	{
		if ( TRANTABLE[i].td_trantype == trantype)
		{
			ptrandef = &TRANTABLE[i];
			return (ptrandef);
			break;
		 }
	}
	return(ptrandef);

} /* end isoutil_Find_Tran_Table */   


/*****************************************************************************

   Function:         isoutil_GenericMove

   Description:      ?????????????????

   Author:           Unknown

   Inputs:

   Outputs:

   Return values:    None

   Modified by:      BGable      6/26/98

******************************************************************************/
extern void isoutil_GenericMove(enum bitfields fieldid)
{
   struct            bitfield_data *pBF;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = isoutil_Find_Bitfield_Table(fieldid);

   memcpy(&ReqRsp_Buffer[reqidx],         /* move to request buffer */
          pBF->bd_pdata,                  /* from data pointer      */
          pBF->bd_fieldlen);              /* for length of field    */

   reqidx = reqidx + pBF->bd_fieldlen;    /* increment buffer pointer */

} /* end isoutil_GenericMove */


/*****************************************************************************

   Function:         isoutil_Move_Always

   Description:      One of several 'move' functions used in a table to 
                     determine whether an ISO item should be moved to the 
                     auth_tx.  This function will always indicate that the 
                     move should be done.

   Author:           Unknown

   Inputs:           bitfield    - the bit number in the bit map
                     trantype - transaction type

   Outputs:

   Return values:    true - indicates that a move should be performed.

   Modified by:      BGable      6/26/98

******************************************************************************/
extern INT isoutil_Move_Always(enum bitfields bitfield, pCHAR trantype)
{
   return(true);

} /* end isoutil_Move_Always */

  
/*****************************************************************************

   Function:         isoutil_Move_IfThere

   Description:      One of several 'move' functions used in a table to 
                     determine whether an ISO item should be moved to the 
                     auth_tx.TLF01_details.  This function will determine 
                     if there is data to move.  If there is data, this 
                     function will return true.

   Author:           Unknown

   Inputs:           bitfield    - the bit number in the bit map
                     trantype - transaction type

   Outputs:

   Return values:    true  - indicates that a move should be done
                     false - indicates that a move should not be done

   Modified by:      BGable      6/26/98

******************************************************************************/
extern INT isoutil_Move_IfThere(enum bitfields fieldid, pCHAR trantype)
{
   struct            bitfield_data *pBF;
   INT               i;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = isoutil_Find_Bitfield_Table(fieldid);

   /*---------------------------------------------------*/
   /* in any byte non-zero, it's there, return true     */
   /*---------------------------------------------------*/
   for (i=0; i < pBF->bd_fieldlen; i++)
      {
      if ( (pBF->bd_pdata [i] != 0) && (pBF->bd_pdata [i] != ' ') ) 
         return(true);
      }

   return(false);

} /* end isoutil_Move_IfThere */


/*****************************************************************************

   Function:         isoutil_Move_Never

   Description:      One of several 'move' functions used in a table to 
                     determine whether an ISO item should be moved to the 
                     auth_tx.TLF01_details.  This function will always 
                     indicate that the move should not be done.

   Author:           Unknown

   Inputs:           bitfield    - the bit number in the bit map
                     trantype - transaction type

   Outputs:

   Return values:    false  - indicates that a move should not be done. 

   Modified by:      BGable      6/26/98

******************************************************************************/
extern INT isoutil_Move_Never(enum bitfields bitfield, pCHAR trantype)
{
   return(false);

} /* end isoutil_Move_Never */

/**************************************************************************/
extern BOOLEAN isoutil_RspIgnore(enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   BOOLEAN  ret_val = true;


   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = isoutil_Find_Bitfield_Table(fieldid);
   if (pBF != NULL)
   {
      if ( (reqidx + pBF->bd_fieldlen) <= buflen )
         reqidx = reqidx + pBF->bd_fieldlen;   /* increment buffer pointer */
      else
      {
         invalid_msg_format = true;
         ret_val = false;
      }
   }
   else
      ret_val = false;

   return( ret_val );
} /* end RspIgnore */
/*****************************************************************************

   Function:         Copy_MC_ECI_vlaue

   Description:      This function parses the private use bit 63 field-Table id 33 and 
                     puts the ECI(2 digit) value into the auth_tx.TLF01_details.profile

   Inputs:           NOne
                     

   Outputs:          None

   Return values:    None

   Author:           Phani

   Modified by:      Phani 23rd-June-2010

******************************************************************************/

void Copy_MC_ECI_vlaue ()
{	
		CHAR    VbV_data[150]={0x00};
		BYTE    merch_serial_numbers[41];
		BYTE    security_level_ind[8];
		BYTE    ucaf_data[33];
		BYTE    cvc2_data[8] = "";
		INT     len = 0;
		INT     ecom_total_len=0;
		INT     ecom_sub_len;
		INT     ecom_idx;
		INT     serial_nbrs_len;
		INT     sec_lev_ind_len;
		BYTE    ucaf_indicator = 0x00;
		CHAR    ecom_substr[5];
		CHAR    bcd_ecom_substr[3];
		CHAR    temp_str[130];

		ecom_total_len = atoi( auth_tx.int_net_facility_len );
		memset( merch_serial_numbers,  0x00, sizeof(merch_serial_numbers) );
		memset( security_level_ind,    0x00, sizeof(security_level_ind)   );
		memset( ucaf_data,             0x00, sizeof(ucaf_data)            );

		ecom_idx = 0;
		memcpy( VbV_data, auth_tx.int_net_facility_data, ecom_total_len);

		genutil_bcd_to_asc (auth_tx.int_net_facility_data, 24,  
						   &temp_str[0], sizeof(temp_str));

		/* -------------------------------------------------------- */
		/* Merchant/Cardholder Certificate Serial Numbers (DE48.40) */
		/* -------------------------------------------------------- */

		/* Get length of Certificate subfield. */
		memset( ecom_substr,     0x00, sizeof(ecom_substr)     );
		memset( bcd_ecom_substr, 0x00, sizeof(bcd_ecom_substr) );

		memcpy( bcd_ecom_substr, VbV_data, 2 );
		genutil_bcd_to_asc( bcd_ecom_substr, 2, ecom_substr, 4 );
		ecom_sub_len = atoi( ecom_substr );
		ecom_idx += 2;

		/* Copy in the data. */
		memcpy( merch_serial_numbers, &VbV_data[ecom_idx], ecom_sub_len );
		serial_nbrs_len = ecom_sub_len;
		ecom_idx += ecom_sub_len;

		/* ---------------------------------- */
		/* Security Level Indicator (DE48.42) */
		/* ---------------------------------- */
		memset( ecom_substr,     0x00, sizeof(ecom_substr)     );
		memset( bcd_ecom_substr, 0x00, sizeof(bcd_ecom_substr) );

		memcpy( bcd_ecom_substr, &VbV_data[ecom_idx], 2 );
		genutil_bcd_to_asc( bcd_ecom_substr, 2, ecom_substr, 4 );
		ecom_sub_len = atoi( ecom_substr );
		ecom_idx += 2;

		memcpy( security_level_ind, &VbV_data[ecom_idx], ecom_sub_len );
		sec_lev_ind_len = ecom_sub_len;
		ecom_idx += ecom_sub_len;

		memcpy( auth_tx.TLF01_details.profile,
				security_level_ind+(sec_lev_ind_len-2),
				2 );

return;
}

/*****************************************************************************

   Function:         isoutil_parse_private_use_63

   Description:      This function parses the private use bit 63 field and 
                     puts the data into the auth_tx.TLF01_details.

   Inputs:           data_array - bit 63 field from the ISO request message
                     data_len   - length of data_array

   Outputs:          None

   Return values:    None

   Author:           Unknown

   Modified by:      EPriyev 10/27/98

******************************************************************************/
extern void isoutil_parse_private_use_63(pBYTE data_array, INT data_len)
{
   INT               field_len;
   INT               move_len,move_len_TID12=0;
   INT               i_table;
   pCHAR             p_char;
   CHAR              table_id[3];
   pBYTE             p_uchar;
   BYTE              tempstr[5];
   BYTE              term_data[3]="";
   INT 				 term_value=0;

   p_uchar = data_array;
   while ((field_len = genutil_bcd_to_int (p_uchar, 2)) > 0)
   {
      /*
       *    Initialize variable.
       */
      memset(table_id, 0, 3);

      /*
       *    Point to the table id of the field we're parsing.
       */
      p_uchar += 2;

      /*
       *    Get the table_id.
       */
      memcpy( table_id, p_uchar, 2 );
      i_table = atoi( table_id );

      /*
       *    Point past the table id to the beginning of the data.
       */
      p_uchar += 2;
      p_char = p_uchar;
      move_len = field_len - 2;

	  /* TF PHANI */
	  if(move_len > 1000) // Total message len never more than 1000
		  break;

      /* Switch based on Table ID. */
      switch( i_table )
      {
         case 12: /* Ticket Number */
                  /* ------------- */
                  /* TF Phani- Changes for DE 63 message for EFTPOS*/
				  move_len_TID12 = (move_len > 27) ? 27 : move_len;
                  p_uchar += move_len;
                  memcpy(auth_tx.TLF01_details.ticket_nbr, p_char, 15);// First 15 Bytes copy into Ticket Numebr
				  if(move_len > 15)
				  {
					  memcpy(auth_tx.TLF01_details.product_codes[14].amount, p_char+15, (move_len_TID12-15));// 14 Amount	
				  }

                  break;

         case 33: /* VISA       E-COMMERCE DATA */
         case 34: /* MASTERCARD E-COMMERCE DATA */

                  /* Do the breakup of field at the visa or mastercard
                   * network controller, as the fields in auth are not
                   * big enough for this.
                   */
                  p_uchar += move_len;
                  memcpy(auth_tx.int_net_facility_data, p_char, move_len);
                  memset( tempstr, 0x00, sizeof(tempstr) );
                  sprintf( tempstr, "%04d", move_len );
                  memcpy( auth_tx.int_net_facility_len, tempstr, 4 );
				  /* TF - Phani : Parse and store ECI value in TLF -> profile*/
				  if(i_table == 33)
				  {
					memcpy(auth_tx.TLF01_details.profile,&auth_tx.int_net_facility_data[8],2);
				  }
				  else if(i_table == 34)
				  {
					Copy_MC_ECI_vlaue(); 
				  }
                  break;


         case 37: /* Batch Number */
                  /* ------------ */
                  move_len = (move_len > 6) ? 6 : move_len;
                  p_uchar += move_len;
                  memcpy(auth_tx.TLF01_details.batch_number, p_char, move_len);
                  break;

         case 39: /* Tax Amount */
                  /* ---------- */
                  move_len = (move_len > 12) ? 12 : move_len;
                  p_uchar += move_len;
                  memcpy(auth_tx.TLF01_details.tax_amount, p_char, move_len);
                  break;

                  /* The following cases are for 56/57/59 and 78/79/81. There
                   * was a mistake in the spec, so I am not sure which table
                   * IDs the terminal is using.  It should be 56/57/59.
                   */
         case 56:
         case 78: /* Odometer and maybe Term Length for Online Deferreds */
                  /* --------------------------------------------------- */
                  move_len = (move_len > 7) ? 7 : move_len;
                  p_uchar += move_len;
                  memcpy(auth_tx.TLF01_details.odometer, p_char, move_len);

                  /* Does Deferred Txn Infor follow next? */
                  if ( (0 == strncmp(p_uchar, "57", 2)) ||
                       (0 == strncmp(p_uchar, "79", 2))  )
                  {
                     /* Yes */
                     p_uchar += 2;
                     memcpy( auth_tx.TLF01_details.deferred_term_length,
                             p_uchar, 3);
                     p_uchar += 3;
                  }
                  break;

         case 57:
         case 79: /* Deferred Term Length */
                  /* -------------------- */
                  p_uchar += 3;
                  memcpy( auth_tx.TLF01_details.deferred_term_length,
                          p_char, 3);
                  p_uchar += 3;
                  break;

         case 59:
         case 81: /* Online Deferred Info for Batch Uploads */
                  /* -------------------------------------- */
                  move_len = 3;
                  p_uchar += move_len;
                  memcpy( auth_tx.TLF01_details.deferred_term_length,
                          p_char, move_len );

                  /* Monthly Amount */
                  memcpy( auth_tx.TLF01_details.monthly_amort_amt, p_uchar, 12);
                  p_uchar += 12;

                  /* Gross Amount */
                  memcpy( auth_tx.TLF01_details.def_gross_amt, p_uchar, 12 );
                  p_uchar += 12;
                  break;

         default: p_uchar += move_len;
                  break;
      }

   } /* end of while loop */

/* Check if this is A MP transaction or not*/
  if(2 == strlen(data_array) )
  {
	memcpy(term_data,data_array,2);
	term_value=atoi(term_data);
	if(term_value >=2 && term_value <= 99)
	{
		/*Its MP transaction*/
		memcpy(auth_tx.TLF01_details.product_codes[2].quantity,"1",1);
		memcpy(auth_tx.TLF01_details.deferred_term_length,term_data,strlen(term_data));
	}
  }
   return;

} /* isoutil_parse_private_use_63 */


/******************************************************************************

   Function:         isoutil_verify_msg_type

   Description:      This function verifies that the message type is valid.
  
   Author:           Unknown

   Inputs:           None

   Outputs:          None

   Return values:    BYTE - contains the transaction type

   Modified by:      BGable      6/30/98

*******************************************************************************/
extern BOOLEAN isoutil_verify_msg_type()
{  
   /* Check if the transaction is a financial request */ 
   if ( (ISO_MESSAGE.msgtype_bcd [0] == 0x02) && (ISO_MESSAGE.msgtype_bcd [1] == 0x00) )
   {
      return(true);
   }
   /* reload repeat */
   else if ( (ISO_MESSAGE.msgtype_bcd [0] == 0x02) && (ISO_MESSAGE.msgtype_bcd [1] == 0x01) )
   {
      return(true);
   }
   /* reload confirm */
   else if ( (ISO_MESSAGE.msgtype_bcd [0] == 0x02) && (ISO_MESSAGE.msgtype_bcd [1] == 0x02) )
   {
      return(true);
   }

   /* or  an advise request */
   else if ( (ISO_MESSAGE.msgtype_bcd [0] == 0x02) && (ISO_MESSAGE.msgtype_bcd [1] == 0x20) )
   {
      return(true);  
   }

   /* or an Authorization request */
   else if ( (ISO_MESSAGE.msgtype_bcd [0] == 0x01) && (ISO_MESSAGE.msgtype_bcd[1] == 0x00) )
   {
      return(true);
   }

   /* or a Reversal request */  
   else if ( (ISO_MESSAGE.msgtype_bcd [0] == 0x04) && (ISO_MESSAGE.msgtype_bcd[1] == 0x00) )
   {
      return(true);
   }

   /* or a batch upload */
   else if ( (ISO_MESSAGE.msgtype_bcd [0] == 0x03) && (ISO_MESSAGE.msgtype_bcd[1] == 0x20) )
   {
      return(true);
   }

   /* or an EMV Advice */
   else if ( (ISO_MESSAGE.msgtype_bcd [0] == 0x03) && (ISO_MESSAGE.msgtype_bcd[1] == 0x40) )
   {
      return(true);
   }

   /* or a settlement transaction */
   else if ( (ISO_MESSAGE.msgtype_bcd [0] == 0x05) && (ISO_MESSAGE.msgtype_bcd [1] == 0x00) )
   {
      return(true);
   }

   /* or a statistics transaction */
   else if ( (ISO_MESSAGE.msgtype_bcd [0] == 0x08) && (ISO_MESSAGE.msgtype_bcd [1] == 0x00) )
   {
      return(true);
   }

   /* or a Please Wait Advice */  
   else if ( (ISO_MESSAGE.msgtype_bcd [0] == 0x08) && (ISO_MESSAGE.msgtype_bcd[1] == 0x30) )
   {
      return(true);
   }

   /* or a transaction that the financial server does not understand */ 
   else
   {
      return(false);
   }

} /* isoutil_verify_msg_type */


/*****************************************************************************

  Function:   isoutil_parse_terminal_request_message

  Description: 
  Author: 
      unknown
  Inputs:
      None
  Outputs:
      p_termid - terminal ID - this is commented out 
  Return values:
      None
  Modified by:
      BGable      6/30/98
******************************************************************************/
extern BOOLEAN isoutil_parse_terminal_request_message(pCHAR p_termid)
{
	struct bitfield_data *pBF;
	INT    bfidx;         /* index to bitfields for this transaction */

	bHasTimeField = false;
	memset(&ISO_MESSAGE, 0x00, sizeof(ISO_MESSAGE));
	memset(ISO_MESSAGE.sec_ctl ,0x20,sizeof(ISO_MESSAGE.sec_ctl));
	memcpy(ISO_MESSAGE.msgtype_bcd, &ReqRsp_Buffer[0], sizeof(ISO_MESSAGE.msgtype_bcd));

	if (isoutil_verify_msg_type())
	{
		bitmapidx = 2;                            /* point to bitmap     */
		reqidx    = 10;                           /* point past bitmap   */

		/*-----------------------------------------------
		  For every bitfield present in message...
		-------------------------------------------------*/
		for (bfidx = 1; bfidx <= 64; bfidx++)
		{
			if (ReqRsp_Buffer[bitmapidx+BITTAB[bfidx].byte_no] & BITTAB[bfidx].bit_mask)
			{
				/* Bit is on, field is there.... */
				pBF = isoutil_Find_Bitfield_Table(bfidx);       
				if (pBF == NULL_PTR)
					return (false);

				if (bfidx == 12) bHasTimeField = true;//JX 020899
				
				/* TF Phani: If bit is set and no data for this Field, it will become
				problem in parsing. Service may crash during the populating fileds */
				if(reqidx >= buflen)
				{
					return false;
				}
				/*---------------------------------------*/
				/* call the Response Handling routine... */
				/*---------------------------------------*/
			
				if ((pBF->bd_BFRsp)(pBF->bd_fieldid) == false)
					return (false);


			}
		}
		return(true);
	}
	else
	{
		return(false);
	}

   
} /* end isoutil_Parse_Terminal_Request_Message */  
 
 /***********************************************************************************/
extern void isoutil_parse_adddata_48()
{
   CHAR temp[70]         = "";
   CHAR templen_bcd[4]   = "";
   CHAR templen_ascii[8] = "";
   INT  cvc2_len;
   INT  num_spaces = 0;
   INT  i;
   INT  f48_len;
   INT  cvc2_exists = false;
   int serial_num_len=0;
   char Serial_num_length[3]={0};

   /* Field 48 contains the CVC2 data.
    * This is manually entered CVC information.
    * Spaces precede the CVC2 value; not sure how many.
    */
   strcpy( temp, ISO_MESSAGE.adddata );

   /* Get length of field 48. */
   memcpy( templen_bcd, ISO_MESSAGE.adddata_len, 2 );
   genutil_bcd_to_asc(templen_bcd, 2, &templen_ascii[0], sizeof(templen_ascii));
   f48_len = atoi( templen_ascii );

   if(0==strncmp(temp,"02",2))  //get the table ID
   {
	   //get 2 bytes of total length
	   memcpy(auth_tx.future_use_3,templen_ascii+2,2);
	   serial_num_len=f48_len-2;
	   sprintf(Serial_num_length,"%02d",serial_num_len);  //get the length of serial number
	   memcpy(auth_tx.future_use_3+2,Serial_num_length,2);
	   memcpy(auth_tx.future_use_3+4,temp,f48_len);
   }
   else
   {

		/* Count the number of spaces. */
		for (i = 0; i < f48_len; i++) {
			if (temp[i] == ' ')
				num_spaces++;

			else if (temp[i] != 0x00) {
				cvc2_exists = true;
				//break;
			}
		}

		if (num_spaces > 0)
			cvc2_exists = true;

		/* CVC2 can be 3 or 4 digits. */
		if (cvc2_exists == true) {
			auth_tx.TLF01_details.verify_mag_stripe[0] = 'Y';
			cvc2_len = f48_len - num_spaces;
			if (cvc2_len == 3)
				memcpy(auth_tx.TLF01_details.cvc_data, &temp[num_spaces],
						cvc2_len);
			else if (temp[2] == ' ') {
				/* First 2 bytes might be presence indicator and response type.
				 * If yes, then 3rd byte should be a space followed by cvv2 value.
				 */
				memcpy(auth_tx.TLF01_details.cvc_data, &temp[3], 3);

				if ((blank_or_null_string(auth_tx.TLF01_details.cvc_data, 3))
						|| (0 == strcmp(auth_tx.TLF01_details.cvc_data, "000"))
						|| (!isnum(auth_tx.TLF01_details.cvc_data))) {
					/* CVV2 data is not present. */
					auth_tx.TLF01_details.verify_mag_stripe[0] = 'N';
					memset(auth_tx.TLF01_details.cvc_data, ' ', 3);
				} else {
					/* Store Presence Indicator and Response Type for NC. */
					auth_tx.TLF01_details.product_codes[17].quantity[0] =
							temp[0];
					auth_tx.TLF01_details.product_codes[17].quantity[1] =
							temp[1];
				}
			} else
				memcpy(auth_tx.TLF01_details.interest_rate, &temp[num_spaces],
						cvc2_len);
		} else {
			auth_tx.TLF01_details.verify_mag_stripe[0] = 'N';
		}
	}

   return;
}

/**********************************************************************************/

extern void isoutil_priv60_cvt_ascii()
{
   CHAR   templen[6];
   CHAR   temp_batch_nbr[7]="";
   CHAR   temp_amount[13] = "";
   INT	  temp_len=0;  	

   if ( ISO_MESSAGE.msgtype_bcd[0] == 0x02 && 
       (ISO_MESSAGE.msgtype_bcd[1] == 0x20 || ISO_MESSAGE.msgtype_bcd[1] == 0x00) )
   {
      /* It is an adjustment, msg type 0220 or a void, msg type 0200 */
      genutil_bcd_to_asc (ISO_MESSAGE.priv60_len, sizeof(ISO_MESSAGE.priv60_len),
                         &templen[0], sizeof(templen));
	 /*TF Phani- To avoid Crashing in case invalid length */
	  temp_len = sizeof(temp_amount);
	  if(atoi(templen) > temp_len)
	  {
		  return;
	  }
      memcpy(temp_amount, ISO_MESSAGE.priv60, atoi(templen));
      sprintf(auth_tx.TLF01_details.orig_amount, "%012d", atol(temp_amount));
   }
   else if ( (ISO_MESSAGE.msgtype_bcd[0] == 0x05) && (ISO_MESSAGE.msgtype_bcd[1] == 0x00) )
   {
      /* It is a settlement or settlement trailer, msg types 0500 */
      genutil_bcd_to_asc (ISO_MESSAGE.priv60_len, sizeof(ISO_MESSAGE.priv60_len),
                         &templen[0], sizeof(templen));
  	 /*TF Phani- To avoid Crashing in case invalid length */
	  temp_len = sizeof(temp_batch_nbr);
	  if(atoi(templen) > temp_len)
	  {
		  return;
	  }
      memcpy(temp_batch_nbr, ISO_MESSAGE.priv60, atoi(templen));
      sprintf(auth_tx.TLF01_details.batch_number, "%06d", atoi(temp_batch_nbr));
   }
   else if ( (ISO_MESSAGE.msgtype_bcd[0] == 0x03) && (ISO_MESSAGE.msgtype_bcd[1] == 0x20) )
   {
      /* It is a batch upload, msg type 0320.
       * The first 4 bytes are the original message id,
       * next 6 are the original stan.
       */
      memcpy( auth_tx.TLF01_details.orig_message,       ISO_MESSAGE.priv60,  4);
      memcpy( auth_tx.TLF01_details.orig_sys_trace_num, ISO_MESSAGE.priv60+4,6);
	}
   return;
} /* isoutil_priv60_cvt_ascii */


extern void isoutil_priv61_cvt_ascii()
{
   /* Standard Hypercom ISO uses this field for Product Code*/
   strncpy(auth_tx.TLF01_details.product_code, ISO_MESSAGE.priv61, 8);

} /* isoutil_priv61_cvt_ascii */


extern void isoutil_priv62_cvt_ascii()
{ 
   CHAR    templen[6] = {0};
   int	   length = 0;

	/* it is message type 0100 or 0200 or 0220 or 0320 */
	genutil_bcd_to_asc (ISO_MESSAGE.priv62_len, sizeof(ISO_MESSAGE.priv62_len),  /* JH Apr 30 */
						&templen[0], sizeof(templen));

	length = sizeof(auth_tx.TLF01_details.invoice_number);
	if (atoi(templen) < length)
		memcpy(auth_tx.TLF01_details.invoice_number, ISO_MESSAGE.priv62, atoi(templen));
	else
		memcpy(auth_tx.TLF01_details.invoice_number, ISO_MESSAGE.priv62,length-1);

	if(length > 6)
	{
		// Then we have received Purchase ID type and Purchase ID
		memcpy(auth_tx.TLF01_details.product_codes[17].code,
			   ISO_MESSAGE.priv62 + INVOICE_NUMBER_LEN ,
			   PURCHASE_ID_TYPE_LEN);
		memcpy(auth_tx.TLF01_details.product_codes[17].amount,
			   ISO_MESSAGE.priv62 + PURCHASE_ID_DATA_POS ,
			   12);
		memcpy(auth_tx.TLF01_details.product_codes[18].amount,
			   ISO_MESSAGE.priv62 + PURCHASE_ID_LEFTOVER_DATA_POS_1 ,
			   12);
		memcpy(auth_tx.TLF01_details.product_codes[18].code,
			   ISO_MESSAGE.priv62 + PURCHASE_ID_LEFTOVER_DATA_POS_2 ,
			   1);
	}

} /* isoutil_priv62_cvt_ascii */


extern BOOLEAN isoutil_priv63_cvt_ascii()
{
   CHAR	templen[6];
   INT   tot_len = 0;

   if ((ISO_MESSAGE.msgtype_bcd[0] == 0x05) && (ISO_MESSAGE.msgtype_bcd[1] == 0x00) )
   {
      /* It is a settlement or settlement trailer, msg types 0500 */
      genutil_bcd_to_asc (ISO_MESSAGE.priv63_len, sizeof(ISO_MESSAGE.priv63_len), 
                          &templen[0], sizeof(templen));

      memcpy(auth_tx.TLF01_details.num_sales,       ISO_MESSAGE.priv63,       3);
      memcpy(auth_tx.TLF01_details.sales_amount,    ISO_MESSAGE.priv63 +  3, 12);
      memcpy(auth_tx.TLF01_details.num_refunds,     ISO_MESSAGE.priv63 + 15,  3);
      memcpy(auth_tx.TLF01_details.refund_amount,   ISO_MESSAGE.priv63 + 18, 12);
      memcpy(auth_tx.TLF01_details.num_dbsales,     ISO_MESSAGE.priv63 + 30,  3);
      memcpy(auth_tx.TLF01_details.dbsales_amount,  ISO_MESSAGE.priv63 + 33, 12);
      memcpy(auth_tx.TLF01_details.num_dbrefunds,   ISO_MESSAGE.priv63 + 45,  3);
      memcpy(auth_tx.TLF01_details.dbrefund_amount, ISO_MESSAGE.priv63 + 48, 12);
   }
   else if ( (ISO_MESSAGE.msgtype_bcd[0] == 0x08) && (ISO_MESSAGE.msgtype_bcd[1] == 0x00) )
   {
      /* It is a statistics, msg type 0800 */
   }
   else
   {
      /* It is Additional Data for one of the following: (0100, 0200, 0220, 0320) */
      isoutil_parse_private_use_63 (ISO_MESSAGE.priv63, tot_len); 
      tot_len = genutil_bcd_to_int (ISO_MESSAGE.priv63_len, 2);
   } 

   return (true);
} /* isoutil_priv63_cvt_ascii */


/*****************************************************************************

   Function:         isoutil_RspMoveGeneric

   Description:      This function moves a data item from incoming ISO message
                     to the internal ISO buffer

   Author:           Unknown

   Inputs:           fieldid - the bit number in the bit map

   Outputs:          None

   Return values:    None

   Modified by:      BGable      6/26/98

******************************************************************************/
extern BOOLEAN isoutil_RspMoveGeneric(enum bitfields fieldid)
{
   struct            bitfield_data *pBF;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = isoutil_Find_Bitfield_Table(fieldid);

   memcpy(pBF->bd_pdata,                     /* to data pointer             */
          &ReqRsp_Buffer[reqidx],            /* move from request buffer    */
          pBF->bd_fieldlen);                 /* for length of field         */

   reqidx = reqidx + pBF->bd_fieldlen;       /* increment buffer pointer    */

   return(true);
} /* end isoutil_RspMoveGeneric */


/*****************************************************************************

   Function:         isoutil_RspMovevBCD

   Description:      This function moves variable length BCD data from incoming 
                     ISO message to the internal ISO buffer

   Author:           Unknown

   Inputs:           fieldid - the bit number in the bit map

   Outputs:          None

   Return values:    true for success, false for failure

   Modified by:      BGable      6/26/98

******************************************************************************/
extern BOOLEAN isoutil_RspMovevBCD (enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  movelen;
   CHAR Buff[256];

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = isoutil_Find_Bitfield_Table(fieldid);

   movelen = genutil_bcd_to_int(&ReqRsp_Buffer[reqidx], pBF->bd_fieldlen);

   if ((movelen % 2) != 0)
      movelen++;

   movelen = (movelen / 2) + pBF->bd_fieldlen;     /* add for the # length byte(s) */

   if (isoutil_verify_fieldlen(fieldid, movelen) == false)
   {
      sprintf(Buff, "invalid length of ISO field %d", fieldid);
      dcpimp_log_message( 1, 2, Buff," isoutil_RspMovevBCD ", 0 );
	  return false;
   }

   memcpy(pBF->bd_pdata, &ReqRsp_Buffer[reqidx], movelen);

   reqidx = reqidx + movelen;   /* increment buffer pointer */
   return true;
} /* end isoutil_RspMovevBCD */


/*****************************************************************************

   Function:         isoutil_RspMovevData

   Description:      This function moves variable length data from incoming 
                     ISO message to the internal ISO buffer
                  
   Author:           Unknown

   Inputs:           fieldid - the bit number in the bit map

   Outputs:          None

   Return values:    true for success, false for failure

   Modified by:      BGable      6/26/98

******************************************************************************/
extern BOOLEAN isoutil_RspMovevData (enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  movelen;
   CHAR Buff[256];

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = isoutil_Find_Bitfield_Table(fieldid);

   movelen = genutil_bcd_to_int(&ReqRsp_Buffer[reqidx], pBF->bd_fieldlen) + pBF->bd_fieldlen; 

      if (isoutil_verify_fieldlen(fieldid, movelen) == false)
{
      sprintf(Buff, "invalid length of ISO field %d", fieldid);
      dcpimp_log_message( 1, 2, Buff," isoutil_RspMovevBCD ", 0 );
	  return false;
   }

   memcpy(pBF->bd_pdata, &ReqRsp_Buffer[reqidx], movelen);

   reqidx = reqidx + movelen;   /* increment buffer pointer */
   return true;
} /* end isoutil_RspMovevData */


/*****************************************************************************

   Function:         isoutil_vBCDMove

   Description:      This function moves variable length BCD data from the 
                     internal ISO buffer to the outgoing ISO message.

   Author:           Unknown

   Inputs:           fieldid - the bit number in the bit map

   Outputs:          None
  
   Return values:    None

   Modified by:      BGable      6/26/98

******************************************************************************/
extern void isoutil_vBCDMove(enum bitfields fieldid)
{
   struct            bitfield_data *pBF;
   INT               movelen;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = isoutil_Find_Bitfield_Table(fieldid);

   movelen = genutil_bcd_to_int(pBF->bd_pdata, pBF->bd_fieldlen); 

   if ((movelen % 2) != 0)
      movelen++;

   movelen = (movelen / 2) + pBF->bd_fieldlen;     /* add for the # length byte(s) */

   memcpy(&ReqRsp_Buffer[reqidx], pBF->bd_pdata, movelen);

   reqidx = reqidx + movelen;   /* increment buffer pointer */

} /* end isoutil_vBCDMove */

/*****************************************************************************

   Function:         isoutil_vDataMove

   Description:      This function moves variable length data from the internal 
                     ISO buffer to the outgoing ISO message.

   Author:           Unknown

   Inputs:           fieldid - the bit number in the bit map

   Outputs:       
   
   Return values:    None

   Modified by:      BGable      6/26/98

******************************************************************************/
extern void isoutil_vDataMove(enum bitfields fieldid)
{
   struct            bitfield_data *pBF;
   INT               movelen;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = isoutil_Find_Bitfield_Table(fieldid);

   movelen = genutil_bcd_to_int(pBF->bd_pdata, pBF->bd_fieldlen) + pBF->bd_fieldlen;

   memcpy(&ReqRsp_Buffer[reqidx], pBF->bd_pdata, movelen);

   reqidx = reqidx + movelen;   /* increment buffer pointer */

} /* end isoutil_vDataMove */


/*****************************************************************************

  Function:   isoutil_verify_fieldlen

  Description: This function checks if data to move to internal iso structure 
				is not longer then assigned size of the field
  Author: 
      Emilia P.
  Inputs:
      fieldid - the bit number in the bit map
	  movelen - the length of the data
  Outputs:
      None
  Return values:
      None
  Modified by:
******************************************************************************/
extern BOOLEAN isoutil_verify_fieldlen (enum bitfields fieldid, int movelen)
{
   INT    fieldlen = 0;

   switch (fieldid)
   {
		case bf_2pan:
		   fieldlen = sizeof(ISO_MESSAGE.pan_len) + sizeof(ISO_MESSAGE.pan_bin);
		   break;
		case bf_35track2:
		   fieldlen = sizeof(ISO_MESSAGE.track2_len) + sizeof(ISO_MESSAGE.track2);
		   break;
		case bf_45track1:
		   fieldlen = sizeof(ISO_MESSAGE.track1_len) + sizeof(ISO_MESSAGE.track1);
		   break;
		case bf_48adddata:
		   fieldlen = sizeof(ISO_MESSAGE.adddata_len) + sizeof(ISO_MESSAGE.adddata);
		   break;
		case bf_54addlamts:
		   fieldlen = sizeof(ISO_MESSAGE.addl_len) + sizeof(ISO_MESSAGE.addlamts);
		   break;
		case bf_55ICCdata:
		   fieldlen = sizeof(ISO_MESSAGE.ICCdata_len) + sizeof(ISO_MESSAGE.ICCdata);
		   break;
		case bf_60priv60:
		   fieldlen = sizeof(ISO_MESSAGE.priv60_len) + sizeof(ISO_MESSAGE.priv60);
		   break;
		case bf_61priv61:
		   fieldlen = sizeof(ISO_MESSAGE.priv61_len) + sizeof(ISO_MESSAGE.priv61);
		   break;
		case bf_62priv62:
		   fieldlen = sizeof(ISO_MESSAGE.priv62_len) + sizeof(ISO_MESSAGE.priv62);
		   break;
		case bf_63priv63:
		   fieldlen = sizeof(ISO_MESSAGE.priv63_len) + sizeof(ISO_MESSAGE.priv63);
		   break;
   }

   if (fieldlen != 0 && movelen > fieldlen)
	   return false;
   
   return true;

} /* end isoutil_verify_fieldlen */

/******************************  END OF FILE  ****************************************************/
