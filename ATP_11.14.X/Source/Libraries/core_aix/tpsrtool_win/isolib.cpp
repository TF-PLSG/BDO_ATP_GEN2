/*

//
//							DEVELOPED BY 
//		THOUGHTFOCUS (C) INFORMATION TECHNOLOGIES PVT. LTD.
//							BANGALORE, INDIA
//#*********************************************************************************
//#*********************************************************************************
*/
/**************************************************************************************************
*  
*		Utility functions for ISO 8583 messages
*
**************************************************************************************************/
#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "isolib.h"

#include "pte.h"
#include "pteipc.h"
#include "ptemsg.h"
#include "NTUtils.h"
#include "TxUtils.h"

//#include "thglobal.h"
//#include "therror.h"
#include "genutil.h"  // Includes all function prototypes for generic utilities
#include "isoutil.h"  // Includes all function prototypes for ISO standard utilities
//#include "card.h"


int                reqidx;                  /* next open position in request buffer */
struct iso_msg_rec ISO_MESSAGE;
int                bitmapidx;               /* offset of bitmap... */
BYTE               ReqRsp_Buffer [COMM_BUF_SZE];
int                buflen;                  /* total size of ReqRsp_Bufferbuffer */

int                bHasTimeField;
int                bHasPinField;
                                            
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
   bf_22pose,                    /* field */
   ISO_MESSAGE.posentry,         /* data to move */
   2,                            /* # of bytes in iso8583 field */
   isoutil_Move_Always,          /* conditional */
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
   bf_54addlamts,                /* field */
   ISO_MESSAGE.addl_len,         /* data to move */
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
   ISO_MESSAGE.priv62_len,       /* data to move */
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


      

const struct isobit BITTAB[64+1] =
   {
   { 0, 0 }, /*zero justify */
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

  Function:   genutil_bcd_to_asc

  Description:  
      convert bcd/binary string to ascii string
  Author: 
      unknown
  Inputs:
      bstring  - incoming bcd string
      blen     - length of bstring
      astr_len - length of astr
  Outputs:
      astr     - outgoing ascii character string
  Return values:
      None
  Modified by:
 ******************************************************************************/
void genutil_bcd_to_asc (BYTE bstring[], INT blen, pCHAR astr, INT astr_len)
{
   INT  i = 0;

   memset (astr, 0x00, astr_len);

   for (i = 0; i < (blen * 2); i=i+2)
      {
      astr[i] = (bstring[i/2] / 16) + '0';
      astr[i+1] = (bstring[i/2] & 0x0F) + '0';
      }
} /* genutil_bcd_to_asc */

/*****************************************************************************

  Function:   genutil_bcd_to_int

  Description:
      Convert a bcd string into an integer value.
  Author: 
      unknown
  Inputs:
      p_bcd_data  -  pointer to the incoming bcd string
      bcd_len     -  length of the bcd string
  Outputs:
  Return values:
      INT         -  integer value of the bcd string
  Modified by:
 ******************************************************************************/
INT genutil_bcd_to_int (pBYTE  p_bcd_data, INT bcd_len)
{
   INT   temp_int;
   INT   multiplier;
   INT   i;


   multiplier = 1;
   temp_int   = 0;
   
   for (i = (bcd_len-1); i >= 0; i--)
      {
      temp_int+= (p_bcd_data [i] & 0x0f) * multiplier;
      multiplier = multiplier * 10;
      temp_int+= (p_bcd_data [i] / 16) * multiplier;
      multiplier = multiplier * 10;
      }

   return (temp_int);
} /*genutil_bcd_to_int */

/*****************************************************************************

  Function:   genutil_blank_string

  Description:
      This function determines if an input string it blank.
  Author: 
      unknown
  Inputs:
      astr     - incoming string
      max_len  - maximum length of the incoming string 
  Outputs:
      None
  Return values:
      true     - the incoming string is blank
      false    - the incoming string is not blank
  Modified by:
 ******************************************************************************/
BOOLEAN genutil_blank_string(pCHAR astr, INT max_len)
{
   INT i;
   for(i = 0;i < max_len; i++)
   {
      if (astr[i] != ' ')
         return(false);
   }
   return(true);    
} /* genutil_blank_string */

 /*****************************************************************************

  Function:   isoutil_CondExpd

  Description:
      Determine whether there is data in the expiration date ISO field
  Author: 
      unknown
  Inputs:  There are two inputs, bitfield and trantype.  Neither of these 
           inputs is used, but because this function is one of many called
           using a table, the parameters must be in the function definition.
  Outputs:
  Return values:
      true  -  expiration date field contains data 
      false -  expiration date field doe not contain data 
  Modified by:
 ******************************************************************************/
int isoutil_CondExpd(enum bitfields bitfield, pCHAR trantype)
{
        /* if track2 data here, no need for expdate field */
   if (ISO_MESSAGE.date_expd [1] != 0x00)    /* JH Apr 29  Fixed bug */
       return(true);
  
   return(false);
} /* end isoutil_CondExpd */

/*****************************************************************************

  Function:   isoutil_CondPan

  Description:
      Determine whether there is data in the primary account number ISO field
  Author: 
      unknown
  Inputs:  
      bitfield    - the bit number in the bit map
      p_trantype  - transaction type
  Outputs:
  Return values:
      true  -  primary account number field contains data 
      false -  primary account field doe not contain data 
  Modified by:
 ******************************************************************************/
int isoutil_CondPan(enum bitfields bitfield, pCHAR p_trantype)
{
/* JH Apr 29  changed procedure to only send the pan if it is there */
   if (ISO_MESSAGE.pan_len [0] != 0x00)   /* JH Apr 29  Fixed bug */
       return(isoutil_Move_IfThere(bitfield, p_trantype));
  
   return(false);

} /* end CondPan */
/*****************************************************************************

  Function:   isoutil_CondProc

  Description:
      Determine whether the message type requires a processing code
  Author: 
      unknown
  Inputs:  
      There are two inputs, bitfield and trantype.  Neither of these 
      inputs is used, but because this function is one of many called
      using a table, the parameters must be in the function definition.
  Outputs:
  Return values:
      true  -   processing code require
      false -   processing code not required
  Modified by:
 ******************************************************************************/
int isoutil_CondProc(enum bitfields bitfield, pCHAR trantype)
{
   if (ISO_MESSAGE.msgtype_bcd [0] != 0x04)
      return (true);

   return(false);
} /* end isoutil_CondProc */

/*****************************************************************************

  Function:   isoutil_CondTrack1

  Description:
      Determine whether there is any track 1 data in the ISO field
  Author: 
      unknown
  Inputs:  
      There are two inputs, bitfield and trantype.  Neither of these 
      inputs is used, but because this function is one of many called
      using a table, the parameters must be in the function definition.
  Outputs:
  Return values:
      true  -   track 1 data available
      false -   track 1 data not available
  Modified by:
 ******************************************************************************/
int isoutil_CondTrack1 (enum bitfields bitfield, pCHAR trantype)
{
   if (ISO_MESSAGE.track1_len [0] != 0x00)   /* JH Apr 29  Fixed bug */
       return(true);

   return(false);
} /* end isoutil_CondTrack1 */

/*****************************************************************************

  Function:   isoutil_Find_Bitfield_Table

  Description: This function uses the bitfield to determine which item in
               the BFData to point the return value
  Author: 
      unknown
  Inputs:
      bitfield    - the bit number in the bit map
  Outputs:
  Return values:
      returns a pointer to BFData array item associated with the bitfield
  Modified by:
 ******************************************************************************/
struct bitfield_data *isoutil_Find_Bitfield_Table (enum bitfields bitfield)
{
	struct bitfield_data *pBF = NULL;      /* point to return record       */
	int      i;							 /* loop index         */
	pBF = NULL_PTR;
	for (i=0; i <= N_BFData; i++)
	if ( BFData[i].bd_fieldid == bitfield)
	{
		pBF = &BFData[i];
		return (pBF);
	}
	return(pBF);
} /* end isoutil_Find_Bitfield_Table */


/*****************************************************************************

  Function:   isoutil_GenericMove

  Description:  ?????????????????
  Author: 
      unknown
  Inputs:
  Outputs:
  Return values:
      None
  Modified by:
 ******************************************************************************/
void isoutil_GenericMove(enum bitfields fieldid)
{
   struct bitfield_data *pBF;


   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = isoutil_Find_Bitfield_Table(fieldid);

   memcpy(&ReqRsp_Buffer[reqidx],      /* move to request buffer */
          pBF->bd_pdata,         /* from data pointer      */
          pBF->bd_fieldlen);      /* for length of field    */

   reqidx = reqidx + pBF->bd_fieldlen;   /* increment buffer pointer */
} /* end isoutil_GenericMove */
/*****************************************************************************

  Function:   isoutil_Move_Always

  Description: One of several 'move' functions used in a table to determine 
               whether an ISO item should be moved to the auth_tx.  This function
               will always indicate that the move should be done.
  Author: 
      unknown
  Inputs:
      bitfield    - the bit number in the bit map
      trantype - transaction type
  Outputs:
  Return values:
      true - indicates that a move should be performed.
  Modified by:
 ******************************************************************************/
int isoutil_Move_Always(enum bitfields bitfield, pCHAR trantype)
{
   return(true);
} /* end isoutil_Move_Always */

  
/*****************************************************************************

  Function:   isoutil_Move_IfThere

  Description: One of several 'move' functions used in a table to determine 
               whether an ISO item should be moved to the auth_tx.TLF01_details.  This function
               will determine if there is data to move.  If there is data, this 
               function will return true.
  Author: 
      unknown
  Inputs:
      bitfield    - the bit number in the bit map
      trantype - transaction type
  Outputs:
  Return values:
      true  - indicates that a move should be done
      false - indicates that a move should not be done
  Modified by:
 ******************************************************************************/
int isoutil_Move_IfThere(enum bitfields fieldid, pCHAR trantype)
{
   struct bitfield_data *pBF;
        int i;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = isoutil_Find_Bitfield_Table(fieldid);

   /*---------------------------------------------------*/
   /* in any byte non-zero, it's there, return true     */
   /*---------------------------------------------------*/
   for (i=0; i < pBF->bd_fieldlen; i++)
      {
      if (pBF->bd_pdata [i] != 0)
/* 01/19/01 removed check for spaces. For a valid length value of 20, was not
   not moving data*/
/*	  if ( (pBF->bd_pdata [i] != 0) && (pBF->bd_pdata [i] != ' ') ) */

         return(true);
      }

   return(false);
} /* end isoutil_Move_IfThere */


/********************************************************************* ********

Function:   isoutil_verify_msg_type

Description: This function verifies that the message type is valid. Author: 
unknown
Inputs:
None
Outputs:
None
Return values:
BYTE - contains the transaction type
Modified by:
BGable      6/30/98
********************************************************************** ********/
BOOLEAN isoutil_verify_msg_type()
{  

	//PRINT("isoutil_verify_msg_type");

	/* Check if the transaction is a financial request */ 
	if ( (ISO_MESSAGE.msgtype_bcd [0] == 0x02) && (ISO_MESSAGE.msgtype_bcd [1] == 0x00) )
	{
		return(true);
	}

	/* or  a financial response */   /* NJ Nov 23  New else block */ 
	else if ( (ISO_MESSAGE.msgtype_bcd [0] == 0x02) && (ISO_MESSAGE.msgtype_bcd [1] == 0x10) )
	{
		return(true); 
	}

	/* or  an advise request */   /* JH Apr 25  New else block */ 
	else if ( (ISO_MESSAGE.msgtype_bcd [0] == 0x02) && (ISO_MESSAGE.msgtype_bcd [1] == 0x20) )
	{
		return(true);  // 
	}

	/* or an Authorization request */  /* JH Apr 25  New else block */ 
	else if ( (ISO_MESSAGE.msgtype_bcd [0] == 0x01) && (ISO_MESSAGE.msgtype_bcd[1] == 0x00) )
	{
		return(true);
	}

	/* or an Authorization response */  /* JH Apr 25  New else block */ 
	else if ( (ISO_MESSAGE.msgtype_bcd [0] == 0x01) && (ISO_MESSAGE.msgtype_bcd[1] == 0x10) )
	{
		return(true);
	}

	/* or  a offline response */ 
	else if ( (ISO_MESSAGE.msgtype_bcd [0] == 0x02) && (ISO_MESSAGE.msgtype_bcd [1] == 0x30) )
	{
		return(true); 
	}

	/* or a Reversal request */  
	else if ( (ISO_MESSAGE.msgtype_bcd [0] == 0x04) && (ISO_MESSAGE.msgtype_bcd[1] == 0x00) )
		return(true);

	/* or a batch upload */  /* JH May 13  New else block */
	else if ( (ISO_MESSAGE.msgtype_bcd [0] == 0x03) && (ISO_MESSAGE.msgtype_bcd[1] == 0x20) )
	{
		return(true);
	}
	/* or a settlement transaction */
	else if ( (ISO_MESSAGE.msgtype_bcd [0] == 0x05) && (ISO_MESSAGE.msgtype_bcd [1] == 0x00) )
	{
		return(true);
	}
	/* or a settlement response */
	else if ( (ISO_MESSAGE.msgtype_bcd [0] == 0x05) && (ISO_MESSAGE.msgtype_bcd [1] == 0x10) )
	{
		return(true);
	}
	/* or a negative file download */
	else if ( (ISO_MESSAGE.msgtype_bcd [0] == 0x03) && (ISO_MESSAGE.msgtype_bcd[1] == 0x02) )
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
		return(true);

	/* or a transaction that the financial server does not understand */ 
	else
		return(false);
} /* isoutil_verify_msg_type */

/*****************************************************************************

  Function:   isoutil_parse_terminal_request_or_response_message

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
BOOLEAN isoutil_parse_terminal_request_or_response_message(pBYTE in_msg, int len, int field_no, pBYTE out_ptr)
{

   struct bitfield_data *pBF;
   int                  bfidx;         /* index to bitfields for this transaction */
   char Buff[256];

   memcpy(ReqRsp_Buffer, in_msg, len);
   bHasTimeField = false;
   bHasPinField = false;
   memset(&ISO_MESSAGE, 0x00, sizeof(ISO_MESSAGE));
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
			 pBF = isoutil_Find_Bitfield_Table((enum bitfields)bfidx);       
 			 if(pBF == NULL_PTR)
 			 {
			 	 return (false);
 			 }

			 if (bfidx == 12) bHasTimeField = true;//JX 020899
			 if (bfidx == 52) bHasPinField = true;
			 /*---------------------------------------*/
			 /* call the Response Handling routine... */
			 /*---------------------------------------*/
			 if ((pBF->bd_BFRsp)(pBF->bd_fieldid) == false)
			 {
				 if (pBF->bd_fieldid > 41)
				 {
					sprintf(Buff, "error parsing ISO field %d", pBF->bd_fieldid);
					printf("isoutil_Parse_Terminal_Request_Message: ");
					printf(Buff);
					printf("\n");
				 }
				 return(false);
			 }
			 if(pBF->bd_fieldid == field_no)
			 {
				 memcpy(out_ptr, pBF->bd_pdata, pBF->bd_fieldlen);
			 }
		 }
	  }
	   return(true);
   }
   else
   {
	   return(false);
   }
} /* end isoutil_Parse_Terminal_Request_Message */   


/*****************************************************************************

  Function:   isoutil_RspMoveGeneric

  Description: This function moves a data item from incoming ISO message to the 
               internal ISO buffer
  Author: 
      unknown
  Inputs:
      fieldid - the bit number in the bit map
  Outputs:
      None
  Return values:
      None
  Modified by:
 ******************************************************************************/
BOOLEAN isoutil_RspMoveGeneric(enum bitfields fieldid)
{
   struct bitfield_data *pBF;


   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
        /*---------------------------------------------------*/
   pBF = isoutil_Find_Bitfield_Table(fieldid);

   memcpy(pBF->bd_pdata,         /* to data pointer      */
          &ReqRsp_Buffer[reqidx],      /* move from request buffer */
          pBF->bd_fieldlen);      /* for length of field    */

   reqidx = reqidx + pBF->bd_fieldlen;   /* increment buffer pointer */

   return true;
} /* end isoutil_RspMoveGeneric */

/*****************************************************************************

  Function:   isoutil_RspMovevBCD

  Description: This function moves variable length BCD data from incoming ISO
               message to the internal ISO buffer
  Author: 
      unknown
  Inputs:
      fieldid - the bit number in the bit map
  Outputs:
      None
  Return values:
      None
  Modified by:
 ******************************************************************************/
BOOLEAN isoutil_RspMovevBCD (enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   int                  movelen;
   char Buff[256];

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
	  printf("isoutil_RspMovevBCD: ");
	  printf(Buff);
	  printf("\n");
      //TxUtils_Send_Msg_To_Operator(1,Buff,1,WARN_MSG,"isoutil_RspMovevBCD",
//								2,WARNING_ERROR,NULL,NULL,NULL);
	  return false;
   }

   memcpy(pBF->bd_pdata, &ReqRsp_Buffer[reqidx], movelen);

   reqidx = reqidx + movelen;   /* increment buffer pointer */
   return true;
} /* end isoutil_RspMovevBCD */

/*****************************************************************************

  Function:   isoutil_RspMovevData

  Description: This function moves variable length data from incoming ISO message
               to the internal ISO buffer
  Author: 
      unknown
  Inputs:
      fieldid - the bit number in the bit map
  Outputs:
      None
  Return values:
      None
  Modified by:
 ******************************************************************************/
BOOLEAN isoutil_RspMovevData (enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   int                  movelen;
   char Buff[256];

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = isoutil_Find_Bitfield_Table(fieldid);

   movelen = genutil_bcd_to_int(&ReqRsp_Buffer[reqidx], pBF->bd_fieldlen) + pBF->bd_fieldlen; 

   if (isoutil_verify_fieldlen(fieldid, movelen) == false)
   {
      sprintf(Buff, "invalid length of ISO field %d", fieldid);
	  printf("isoutil_RspMovevData: ");
	  printf(Buff);
	  printf("\n");
      //TxUtils_Send_Msg_To_Operator(1,Buff,1,WARN_MSG,"isoutil_RspMovevData",
	//							2,WARNING_ERROR,NULL,NULL,NULL);
	  return false;
   }

   memcpy(pBF->bd_pdata, &ReqRsp_Buffer[reqidx], movelen);

   reqidx = reqidx + movelen;   /* increment buffer pointer */
   return true;
} /* end isoutil_RspMovevData */

/*****************************************************************************

  Function:   isoutil_vBCDMove

  Description: This function moves variable length BCD data from the internal 
               ISO buffer to the outgoing ISO message.
  Author: 
      unknown
  Inputs:
      fieldid - the bit number in the bit map
  Outputs:
      None
  Return values:
      None
  Modified by:
 ******************************************************************************/
void isoutil_vBCDMove(enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   int                  movelen;

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

  Function:   isoutil_vDataMove

  Description: This function moves variable length data from the internal 
               ISO buffer to the outgoing ISO message.
  Author: 
      unknown
  Inputs:
      fieldid - the bit number in the bit map
  Outputs:
  Return values:
      None
  Modified by:
 ******************************************************************************/
void isoutil_vDataMove(enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   int                  movelen;

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
BOOLEAN isoutil_verify_fieldlen (enum bitfields fieldid, int movelen)
{
   int    fieldlen = 0;

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
