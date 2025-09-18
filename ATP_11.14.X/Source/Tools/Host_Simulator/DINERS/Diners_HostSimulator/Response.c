/*******************************************************************************

  Module:      response.c

  Title:    Host Response Definition File for MasterCard

  Description: This module defines the various Host Responses. This isolates 
                the changes required to emulate a different kind of Host
            processing system.

  Note:
  To modify the Host Response, change the set of structures and 
                functions found here in and recompile.
         
  Limitations:  By design, this module allows you to change the following:
              o The number of request transactions that are recognized.
              o The BIT MAP for each of those transactions.
              o The STATIC DATA contents of the private data fields.
                To make other changes, you will potentially have to alter
            additional modules.

  Revision History:

  $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Hostsim_Mastercard\files\RESPONSE.C  $
   
      Rev 1.24   Oct 26 2005 16:49:10   dirby
   Incorporated reversals.
   
      Rev 1.23   May 16 2005 13:08:20   dirby
   Modified fields 23 and 55 to not be mandatory for auth txns.
   
      Rev 1.22   Feb 23 2005 10:59:30   dirby
   Added support for EMV fields 23 and 55.
   
      Rev 1.21   Feb 25 2004 09:56:44   dirby
   Changed auth number in the response from '*TEST*' to '888888'.
   This is to accomodate Nera/DBS; they have some test terminals
   that cannot handle special characters.
   
      Rev 1.20   Dec 09 2003 09:59:54   dirby
   Corrected some bugs in setting field 48 response data for CAVV.
   SCR 1228
   
      Rev 1.19   Dec 09 2003 09:34:24   dirby
   Modified to initialize field 48 prior to setting it.
   SCR 1228
   
      Rev 1.18   Dec 09 2003 08:07:32   dirby
   Added support to include a CAVV result in field 48.
   SCR 1228
   
      Rev 1.17   Jun 03 2003 14:17:46   dirby
   Modifiy to set RC=00 when CVC1 fails (tag 87 = Y).
   SCR 1134
   
      Rev 1.16   Mar 03 2003 10:56:44   dirby
   Adjusted the response information for 0312 transactions to
   echo DE120, except for MCC102 Inquiries, and also to provide
   realistic data for MCC103 Inquiries in DE122.
   SCR 891
   
      Rev 1.15   Feb 26 2002 17:33:12   dirby
   Fixes a bug.  Was setting response for amount 199 using
   ASCII.  Changed it to EBCDIC.
   
      Rev 1.14   Feb 26 2002 16:50:32   dirby
   Added code to set response code to '08' when the amount is
   1.99.  This is to test 'Approve with Identification' responses
   from MasterCard.
   
      Rev 1.13   Dec 04 2001 13:38:38   dirby
   Modified to return a Merchant Advice Code in field 48, subfield
   84 if field 61 position 4 is '4' (RPS Merchant) and amount is odd.     SCR 531
   
      Rev 1.12   Feb 24 2001 02:32:16   dirby
   Corrected some bugs in the CVC2 processing portion.
   
      Rev 1.11   Feb 16 2001 14:22:16   dirby
   Added code to handle CVC2 response data.
   If CVC2 data is in the request and
       if dollar amount is 0.01, set cvc result flag to N
       if dollar amount is 0.02, set cvc result flag to P
       if dollar amount is 0.03, set cvc result flag to U
       else set cvc result flag to M.
   
      Rev 1.10   Sep 25 2000 09:46:26   dirby
   Modified so host simulator would not Dr. Watson on unknown message types.
   
      Rev 1.9   Sep 22 2000 15:15:40   dirby
   Added code to set the CVC Invalid flag to 'Y' in DE48 when the
   amount is 10.
   
      Rev 1.8   Mar 30 2000 09:10:30   dirby
   Added error path for field 2 in the parse routine.
   
      Rev 1.7   Feb 29 2000 13:06:06   dirby
   Added code to ignore administrative advices.
   
      Rev 1.6   Feb 08 2000 15:33:12   svelaga
   Added other conditions for Field 91 when 
   302 message coming in.
   
      Rev 1.5   Jan 14 2000 11:26:40   dirby
   Corrected a bug by adding data to field 122 for 03xx messages,
   and by not requiring field 41 for 03xx messages, and by not
   processing a message if there is invalid data in it.
   
      Rev 1.4   Jan 08 2000 18:02:04   svelaga
   increased the size of temporary variable to avoid 
   memory dump.
   
      Rev 1.3   Dec 01 1999 12:19:06   dirby
   Added functionality to support File Update Requests (0302/0312 messages).
   
      Rev 1.2   Oct 22 1999 09:31:18   dirby
   Changed the field width of field 12 from 3 to 6 bytes.
   
      Rev 1.1   Oct 14 1999 15:29:32   dirby
   Modified to handle 0800 messages.
   
      Rev 1.0   17 May 1999 10:53:00   dirby
   Initial Release into PVCS
   
      Rev 1.0   Oct 14 1998 14:55:40   drodden
   Initial checkin of the ISO Retail host simulator.
   
   
      Rev 1.3   Apr 09 1998 15:14:52   IGOLDFIE
   Added changes regarding REGISTRO transaction:
   structure REGISTRO_response, changes to parse_host_request,
   generate_response, make_63, case 12.
   
      Rev 1.2   09 Apr 1998 13:07:14   MSALEH
   Modified some files to use the new 
   ptecom library and header file structures
   
      Rev 1.1   18 Mar 1998 13:47:14   MSALEH
   1) Use the new ptecom library
   2) cleanup code and files
   
      Rev 1.0   Nov 11 1997 19:58:00  BROBERTS
      Initial Release


******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <sys/timeb.h>

#include "basictyp.h"
#include "pte.h"
#include "ptecom.h"
#include "response.h"
#include "equitdb.h"



extern int volatile connection_up;
extern INT init_trace();
extern void write_to_comm_trace_file( pBYTE bufptr, INT len, BYTE msg_type );
char RespCodeinTxt[4] = {0};
char AuthCodeinTxt[7] = {0};
char RRNinTxt[12] = {0};
AUTH_TX              Auth_Tx;

int    g_using_tpdu_prefix = false;      // Whether the TPDU is present or not
char   A2E[4] = "A2E";    /*  ASCII to EBCDIC conversion */
char   E2A[4] = "E2A";    /*  EBCDIC to ASCII conversion */

#define DINERS_HDR_SIZE			   0
#define  DINERS_COMM_BUF_SZE       1500
BYTE   ReqRsp_Buffer [DINERS_COMM_BUF_SZE];
/********************   VARIOUS DEFINES and MACROS  **************************/
#define MAX_BIT_FIELD_NUMBER    128

/* Processing Flags */
#define NORMAL         0x80000000
#define REVERSIBLE     0x40000000


/* LOCAL VARIABLES */
INT    reqidx;                  /* next open position in request buffer */
INT    buflen;                  /* total size of buffer */
INT    bitmapidx;               /* offset of bitmap... */
INT    invalid_msg_format;
INT    BitFieldSize;
INT extendedbit = false;

/* BASIC STRUCTURE */
/* The following structure is used to define each transaction:
{
   msg_type[0], msg_type[1],
   proc_code[0], proc_code[1], proc_code[2],
    BF01_off | BF02_off | BF03_ON  | BF04_off | BF05_off | BF06_off | BF07_off | BF08_off,
    BF09_off | BF10_off | BF11_ON  | BF12_ON  | BF13_ON  | BF14_off | BF15_off | BF16_off,
    BF17_off | BF18_off | BF19_off | BF20_off | BF21_off | BF22_off | BF23_off | BF24_off,
    BF25_off | BF26_off | BF27_off | BF28_off | BF29_off | BF30_off | BF31_off | BF32_off,
    BF33_off | BF34_off | BF35_off | BF36_off | BF37_ON  | BF38_off | BF39_ON  | BF40_off,
    BF41_ON  | BF42_off | BF43_off | BF44_off | BF45_off | BF46_off | BF47_off | BF48_off,
    BF49_off | BF50_off | BF51_off | BF52_off | BF53_off | BF54_off | BF55_off | BF56_off,
    BF57_off | BF58_off | BF59_off | BF60_off | BF61_off | BF62_off | BF63_off | BF64_off,
    BF65_off | BF66_off | BF67_off | BF68_off | BF69_off | BF70_off | BF71_off | BF72_off,
    BF73_off | BF74_off | BF75_off | BF76_off | BF77_off | BF78_off | BF79_off | BF80_off,
    BF81_off | BF82_off | BF83_off | BF84_off | BF85_off | BF86_off | BF87_off | BF88_off,
    BF89_off | BF90_off | BF91_off | BF92_off | BF93_off | BF94_off | BF95_off | BF96_off,
    BF97_off | BF98_off | BF99_off | BF100_off| BF101_off| BF102_off| BF103_off| BF104_off,
    BF105_off| BF106_off| BF107_off| BF108_off| BF109_off| BF110_off| BF111_off| BF112_off,
    BF113_off| BF114_off| BF115_off| BF116_off| BF117_off| BF118_off| BF119_off| BF120_off,
    BF121_off| BF122_off| BF123_off| BF124_off| BF125_off| BF126_off| BF127_off| BF128_off.
};
  LEGEND: Each byte in the structure is separated by a COMMA for easy reading.

  NOTE:   To turn ON a bit, just change the suffix from "off" to "ON " - note the space!
        This way you can maintain the alignment of the map, making it easy to read later.
*/


/*********************************************************************
**    BEGINNING OF MASTERCARD SPECIFIC CODE                          *
**********************************************************************
*/

/* Transaction Table Structure */
typedef struct
{
   BYTE  msg_type[4];         /* The Message Type, e.g. 0200 */
   BYTE  proc_code[6];        /* Processing Code Identifier  */
   BYTE  bit_map[16];         /* Bit map that controls which */
                              /* fields that go into the response */
   BYTE  response_code[2];    /*    Typical response code generated */
   long flag;                 /*    Flags to control manipulation */
} ISO8583_ID, *pISO8583_ID;

#define N_TRANENTRY (sizeof(TRANTABLE) / sizeof(struct trandef))

/********************************************************/
/* BIT SETTING TABLE                                    */
/********************************************************/
struct isobit 
   {
   INT byte_no;         /* byte number for this bit field */
   INT bit_mask;        /* bit to set in this byte        */
   };

const struct isobit BITTAB[128+1] =
   {
   { 0, 0 }, /*zero justify */
   { 0, 0x80 },   /* bit 1  */   { 0, 0x40 },   /* bit 2  */   { 0, 0x20 },   /* bit 3  */   { 0, 0x10 },   /* bit 4  */
   { 0, 0x08 },   /* bit 5  */   { 0, 0x04 },   /* bit 6  */   { 0, 0x02 },   /* bit 7  */   { 0, 0x01 },   /* bit 8  */
   { 1, 0x80 },   /* bit 9  */   { 1, 0x40 },   /* bit 10 */   { 1, 0x20 },   /* bit 11 */   { 1, 0x10 },   /* bit 12 */
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
   { 7, 0x08 },   /* bit 61 */   { 7, 0x04 },   /* bit 62 */   { 7, 0x02 },   /* bit 63 */   { 7, 0x01 },    /* bit 64 */

   { 8, 0x80 },    /* bit 65 */   { 8, 0x40 },    /* bit 66 */   { 8, 0x20 },    /* bit 67 */   { 8, 0x10 },    /* bit 68 */
   { 8, 0x08 },    /* bit 69 */   { 8, 0x04 },    /* bit 70 */   { 8, 0x02 },    /* bit 71 */   { 8, 0x01 },    /* bit 72 */
   { 9, 0x80 },    /* bit 73 */   { 9, 0x40 },    /* bit 74 */   { 9, 0x20 },    /* bit 75 */   { 9, 0x10 },    /* bit 76 */
   { 9, 0x08 },    /* bit 77 */   { 9, 0x04 },    /* bit 78 */   { 9, 0x02 },    /* bit 79 */   { 9, 0x01 },    /* bit 80 */
   { 10, 0x80 },   /* bit 81 */   { 10, 0x40 },   /* bit 82 */   { 10, 0x20 },   /* bit 83 */   { 10, 0x10 },   /* bit 84 */
   { 10, 0x08 },   /* bit 85 */   { 10, 0x04 },   /* bit 86 */   { 10, 0x02 },   /* bit 87 */   { 10, 0x01 },   /* bit 88 */
   { 11, 0x80 },   /* bit 89 */   { 11, 0x40 },   /* bit 90 */   { 11, 0x20 },   /* bit 91 */   { 11, 0x10 },   /* bit 92 */
   { 11, 0x08 },   /* bit 93 */   { 11, 0x04 },   /* bit 94 */   { 11, 0x02 },   /* bit 95 */   { 11, 0x01 },   /* bit 96 */
   { 12, 0x80 },   /* bit 97 */   { 12, 0x40 },   /* bit 98 */   { 12, 0x20 },   /* bit 99 */   { 12, 0x10 },   /* bit100 */
   { 12, 0x08 },   /* bit 37 */   { 12, 0x04 },   /* bit 38 */   { 12, 0x02 },   /* bit 39 */   { 12, 0x01 },   /* bit104 */
   { 13, 0x80 },   /* bit 41 */   { 13, 0x40 },   /* bit 42 */   { 13, 0x20 },   /* bit 43 */   { 13, 0x10 },   /* bit108 */
   { 13, 0x08 },   /* bit 45 */   { 13, 0x04 },   /* bit 46 */   { 13, 0x02 },   /* bit 47 */   { 13, 0x01 },   /* bit112 */
   { 14, 0x80 },   /* bit 49 */   { 14, 0x40 },   /* bit 50 */   { 14, 0x20 },   /* bit 51 */   { 14, 0x10 },   /* bit116 */
   { 14, 0x08 },   /* bit 53 */   { 14, 0x04 },   /* bit 54 */   { 14, 0x02 },   /* bit 55 */   { 14, 0x01 },   /* bit120 */
   { 15, 0x80 },   /* bit 57 */   { 15, 0x40 },   /* bit 58 */   { 15, 0x20 },   /* bit 59 */   { 15, 0x10 },   /* bit124 */
   { 15, 0x08 },   /* bit 61 */   { 15, 0x04 },   /* bit 62 */   { 15, 0x02 },   /* bit 63 */   { 15, 0x01 }   /* bit 130*/
   };


  //farad 8/13/1998 the last modifications.

/************************************************
  Bit Field Data Record
*************************************************/
struct bitfield_data 
   {
   enum bitfields  bd_fieldid;                           /* id for this field */
   pBYTE           p_bd_pdata;                           /* ptr to data to move */
   INT             bd_fieldlen;                          /* # of bytes in iso8583 field */
   INT             (*bd_BFCond)(enum bitfields, BYTE);  /* ptr to conditional function */
   INT            (*bd_BFMove)(enum bitfields);         /* ptr to move function */
   INT            (*bd_BFRsp)(enum bitfields);          /* ptr to response handler function */
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
		2,                            /* # of bytes in len field */
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
		ncdci_HOST_MESSAGE.Auth_agent_IIC,   /* data to move */
		11,                            /* # of bytes in field */
		ncdci_Move_IfThere,                  /* mandatory    */
		ncdci_GenericMove,                   /* move routine */
		ncdci_RspMovevData                 /* response routine */
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

/******************************************************************************
 *
 *  NAME:         ncdci_get_variable_length
 *
 *  DESCRIPTION:  This function converts a variable length character string
 *                into an integer.
 *
 *  INPUTS:       DataField - Pointer to ASCII character string
 *                FieldLen  - Length of 'DataField'
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   Integer value of 'DataField'
 *
 *  AUTHOR:      Abhishek Verma
 *
 *  MODIFICATIONS:
 ******************************************************************************/
int ncdci_get_variable_length( unsigned char *DataField, int FieldLen )
{
   int    i = 0;
   double len=0;

   if ( 0 < FieldLen )
   {
      for( i=0; i<FieldLen; i++ )
      {
         len += (int)( DataField[i] - 0x30 ) * (pow(10,(double)(FieldLen-i-1)));
      }
   }
   return( (int)len );
}

/***********************************************
   ncdci_GenericMove
************************************************/
INT ncdci_GenericMove(enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  ret_val = true;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = ncdci_Find_Bitfield_Table(fieldid);
   if (pBF != NULL)
   {

	  //if(fieldid == bf_39respcode)
	  //{
			// /*if((strncmp((char *)&ReqRsp_Buffer[DINERS_TOTALL_MEG_LEN],"1804",4) ==0) || 
			//	 (strncmp((char *)&ReqRsp_Buffer[DINERS_TOTALL_MEG_LEN],"1814",4) ==0))*/
			// memcpy(&ReqRsp_Buffer[reqidx],      /* move to request buffer */
   //          "800",         /* from data pointer      */
   //          pBF->bd_fieldlen);      /* for length of field    */
	  //}
	   if (fieldid == bf_6cardbillamt)
	  {
			 memcpy(&ReqRsp_Buffer[reqidx],      /* move to request buffer */
				 ncdci_HOST_MESSAGE.amount,         /* from data pointer      */
             pBF->bd_fieldlen);      /* for length of field    */
	  }
	  else 
	  {
			memcpy(&ReqRsp_Buffer[reqidx],      /* move to request buffer */
             pBF->p_bd_pdata,         /* from data pointer      */
             pBF->bd_fieldlen);      /* for length of field    */
	  }

      reqidx = reqidx + pBF->bd_fieldlen;   /* increment buffer pointer */
   }
   else
      ret_val = false;

   return( ret_val );

} /* end ncdci_GenericMove */


/*****************************************************************************

  Function:    ncdci_vDataMove

  Description: This function will move data from thencdci_HOST_MESSAGE structure to 
               the the raw ISO message.  Note: it moves the number indicated 
               by the length variable pointed to by pBF->p_db_data[0].
  Author: 
      Abhishek Verma
  Inputs:
      fieldid                - the bit field of the data to be moved
     ncdci_HOST_MESSAGE (global)  - structure that contains data to go into the 
                               outgoing ISO message
  Outputs:
      ReqRsp_Buffer (global) - the buffer that contains the ISO formatted message
  Return values:
      None
  Modified by:
******************************************************************************/
 int ncdci_vDataMove(enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  movelen = 0;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = ncdci_Find_Bitfield_Table(fieldid);

   if (pBF != NULL)
   {
  
     movelen  = ncdci_get_variable_length( pBF->p_bd_pdata, pBF->bd_fieldlen );
	 memcpy(&ReqRsp_Buffer[reqidx],&pBF->p_bd_pdata[0], pBF->bd_fieldlen);
     reqidx =reqidx + pBF->bd_fieldlen;

     memcpy(&ReqRsp_Buffer[reqidx], &pBF->p_bd_pdata[pBF->bd_fieldlen], movelen);

     reqidx = reqidx + movelen;   /* increment buffer pointer */
	 return TRUE;
   }
   return FALSE;
} /* end ncdci_vDataMove */



/*****************************************************************************

  Function:    ncdci_RspMovevData  

  Description: This function will move data from the the raw ISO message 
               to thencdci_HOST_MESSAGE structure.  Note: it moves the number 
               indicated by the length variable pointed to by pBF->p_db_data[0].
  Author: 
      Abhishek Verma
  Inputs:
      fieldid                - the bit field of the data to be moved
      ReqRsp_Buffer (global) - the buffer that contains the ISO formatted message
  Outputs:
     ncdci_HOST_MESSAGE (global)  - structure that contains data to go into the 
                               outgoing ISO message
  Return values:
      None
  Modified by:
******************************************************************************/
 int ncdci_RspMovevData (enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  movelen = 0;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = ncdci_Find_Bitfield_Table(fieldid);

   if (pBF != NULL)
   {
     
	 memcpy(&pBF->p_bd_pdata[0],&ReqRsp_Buffer[reqidx],pBF->bd_fieldlen);
	 reqidx =reqidx + pBF->bd_fieldlen;
	
  	 movelen = atoi(pBF->p_bd_pdata);
     memcpy(&pBF->p_bd_pdata[pBF->bd_fieldlen], &ReqRsp_Buffer[reqidx], movelen);
	 
      reqidx = reqidx + movelen;   /* increment buffer pointer */
	  return TRUE;
   }
   return FALSE;
} /* end ncdci_RspMovevData */


/***********************************************
   ncdci_RspMoveGeneric
************************************************/
INT ncdci_RspMoveGeneric(enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  ret_val = true;


   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = ncdci_Find_Bitfield_Table(fieldid);
   if (pBF != NULL)
   {
      if ( (reqidx + pBF->bd_fieldlen) <= buflen )
      {
         memcpy(pBF->p_bd_pdata,               /* to data pointer          */
                &ReqRsp_Buffer[reqidx],        /* move from request buffer */
                pBF->bd_fieldlen);             /* for length of field      */

         reqidx = reqidx + pBF->bd_fieldlen;   /* increment buffer pointer */
      }
      else
      {
         ret_val = false;
        
      }
   }
   else
      ret_val = false;

   return( ret_val );

} /* end ncdci_RspMoveGeneric */




/******************************************************************************
 *
 *  NAME:         ncdci_Move_IfThere
 *
 *  DESCRIPTION:  This function returns a value based on whether or not there
 *                is data in the specifiedncdci_HOST_MESSAGE field.  If there is
 *                data in it, TRUE is returned, else FALSE is returned.
 *
 *  INPUTS:       fieldid - Field Id to denote the source field of the data
 *                p_trantype - transaction type
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if data exists to be moved, else false
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT ncdci_Move_IfThere( enum bitfields fieldid, BYTE p_trantype )
{
   struct bitfield_data *pBF;
   INT i = 0;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = ncdci_Find_Bitfield_Table( fieldid );
   if (pBF != NULL)
   {
      /*---------------------------------------------------*/
      /* in any byte non-zero, it's there, return true     */
      /*---------------------------------------------------*/
      for ( i=0; i < pBF->bd_fieldlen; i++ )
      {
         if ( (pBF->p_bd_pdata [i] != 0) && (pBF->p_bd_pdata [i] != ' ') ) 
            return( true );
      }
   }
   return( false );
} /* end ncdci_Move_IfThere */


/******************************************************************************
 *
 *  NAME:         ncdci_Move_Always
 *
 *  DESCRIPTION:  This function returns true.  It is used to indicate that data
 *                SHOULD be moved fromncdci_HOST_MESSAGE into ReqRsp_Buffer.
 *
 *  INPUTS:       fieldid - Field Id to denote the source field of the data
 *                p_trantype - transaction type
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True
 *
 *  AUTHOR:       Abhishek Verma
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT ncdci_Move_Always( enum bitfields bitfield, BYTE p_trantype )
{
   return( true );
} /* end ncdci_Move_Always */


ISO8583_ID Sale_Response[] =
{
    0xF0,0xF1,0xF1,0xF0,                        /* Message Type */
    0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,              /* Processing Code */
                                 /* Bit map is 'FEE310018E91E406 0000000006000182' */
    BF01_off | BF02_ON  | BF03_ON  | BF04_ON  | BF05_ON  | BF06_ON  | BF07_ON  | BF08_off,
    BF09_ON  | BF10_ON  | BF11_ON  | BF12_off | BF13_off | BF14_off | BF15_ON  | BF16_ON ,
    BF17_off | BF18_off | BF19_off | BF20_ON  | BF21_off | BF22_off | BF23_off | BF24_off,
    BF25_off | BF26_off | BF27_off | BF28_off | BF29_off | BF30_off | BF31_off | BF32_ON ,
    BF33_off | BF34_off | BF35_off | BF36_off | BF37_ON  | BF38_ON  | BF39_ON  | BF40_off,
    BF41_ON  | BF42_off | BF43_off | BF44_off | BF45_off | BF46_off | BF47_off | BF48_ON ,
    BF49_ON  | BF50_ON  | BF51_ON  | BF52_off | BF53_off | BF54_off | BF55_off | BF56_off,
    BF57_off | BF58_off | BF59_off | BF60_off | BF61_off | BF62_off | BF63_ON  | BF64_off,
    BF65_off | BF66_off | BF67_off | BF68_off | BF69_off | BF70_off | BF71_off | BF72_off,
    BF73_off | BF74_off | BF75_off | BF76_off | BF77_off | BF78_off | BF79_off | BF80_off,
    BF81_off | BF82_off | BF83_off | BF84_off | BF85_off | BF86_off | BF87_off | BF88_off,
    BF89_off | BF90_off | BF91_off | BF92_off | BF93_off | BF94_off | BF95_off | BF96_off,
    BF97_off | BF98_off | BF99_off | BF100_off| BF101_off| BF102_off| BF103_off| BF104_off,
    BF105_off| BF106_off| BF107_off| BF108_off| BF109_off| BF110_off| BF111_off| BF112_off,
    BF113_off| BF114_off| BF115_off| BF116_off| BF117_off| BF118_off| BF119_off| BF120_off,
    BF121_off| BF122_off| BF123_off| BF124_off| BF125_off| BF126_off| BF127_off| BF128_off,
    0xF0,0xF0,                           /* Response Code */
   NORMAL
};

// TF phani .. Adivce response
ISO8583_ID Advice_Response[] =
{
    0xF0,0xF1,0xF3,0xF0,                        /* Message Type */
    0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,              /* Processing Code */
                                 /* Bit map is 'FEE310018E91E406 0000000006000182' */
    BF01_off | BF02_ON  | BF03_ON  | BF04_ON  | BF05_ON  | BF06_ON  | BF07_ON  | BF08_off,
    BF09_ON  | BF10_ON  | BF11_ON  | BF12_off | BF13_off | BF14_off | BF15_ON  | BF16_ON ,
    BF17_off | BF18_off | BF19_off | BF20_ON  | BF21_off | BF22_off | BF23_off | BF24_off,
    BF25_off | BF26_off | BF27_off | BF28_off | BF29_off | BF30_off | BF31_off | BF32_ON ,
    BF33_off | BF34_off | BF35_off | BF36_off | BF37_ON  | BF38_ON  | BF39_ON  | BF40_off,
    BF41_ON  | BF42_off | BF43_off | BF44_off | BF45_off | BF46_off | BF47_off | BF48_off ,
    BF49_ON  | BF50_ON  | BF51_ON  | BF52_off | BF53_off | BF54_off | BF55_off | BF56_off,
    BF57_off | BF58_off | BF59_off | BF60_off | BF61_off | BF62_off | BF63_off | BF64_off,
    BF65_off | BF66_off | BF67_off | BF68_off | BF69_off | BF70_off | BF71_off | BF72_off,
    BF73_off | BF74_off | BF75_off | BF76_off | BF77_off | BF78_off | BF79_off | BF80_off,
    BF81_off | BF82_off | BF83_off | BF84_off | BF85_off | BF86_off | BF87_off | BF88_off,
    BF89_off | BF90_off | BF91_off | BF92_off | BF93_off | BF94_off | BF95_off | BF96_off,
    BF97_off | BF98_off | BF99_off | BF100_off| BF101_off| BF102_off| BF103_off| BF104_off,
    BF105_off| BF106_off| BF107_off| BF108_off| BF109_off| BF110_off| BF111_off| BF112_off,
    BF113_off| BF114_off| BF115_off| BF116_off| BF117_off| BF118_off| BF119_off| BF120_off,
    BF121_off| BF122_off| BF123_off| BF124_off| BF125_off| BF126_off| BF127_off| BF128_off,
    0xF0,0xF0,                           /* Response Code */
   NORMAL
};
//TF-Ajay Reversal Advice response
ISO8583_ID Reversal_Advice_Response[] =
{
    0xF0,0xF4,0xF3,0xF0,                        /* Message Type */
    0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,              /* Processing Code */
                              /* Bit map is 2038000002000004' */
    BF01_ON  | BF02_ON  | BF03_ON  | BF04_ON  | BF05_off | BF06_off | BF07_ON  | BF08_off,
    BF09_off | BF10_off | BF11_ON  | BF12_off | BF13_off | BF14_off | BF15_ON  | BF16_ON ,
    BF17_off | BF18_off | BF19_off | BF20_ON  | BF21_off | BF22_off | BF23_off | BF24_off,
    BF25_off | BF26_off | BF27_off | BF28_off | BF29_off | BF30_off | BF31_off | BF32_ON ,
    BF33_off | BF34_off | BF35_off | BF36_off | BF37_ON  | BF38_off | BF39_ON  | BF40_off,
    BF41_ON  | BF42_off | BF43_off | BF44_off | BF45_off | BF46_off | BF47_off | BF48_ON ,
    BF49_ON  | BF50_ON  | BF51_ON  | BF52_off | BF53_off | BF54_off | BF55_off | BF56_off,
    BF57_off | BF58_off | BF59_off | BF60_off | BF61_off | BF62_off | BF63_off | BF64_off,
    BF65_off | BF66_off | BF67_off | BF68_off | BF69_off | BF70_off | BF71_off | BF72_off,
    BF73_off | BF74_off | BF75_off | BF76_off | BF77_off | BF78_off | BF79_off | BF80_off,
    BF81_off | BF82_off | BF83_off | BF84_off | BF85_off | BF86_off | BF87_off | BF88_off,
    BF89_off | BF90_ON  | BF91_off | BF92_off | BF93_off | BF94_off | BF95_off | BF96_off,
    BF97_off | BF98_off | BF99_off | BF100_off| BF101_off| BF102_off| BF103_off| BF104_off,
    BF105_off| BF106_off| BF107_off| BF108_off| BF109_off| BF110_off| BF111_off| BF112_off,
    BF113_off| BF114_off| BF115_off| BF116_off| BF117_off| BF118_off| BF119_off| BF120_off,
    BF121_off| BF122_off| BF123_off| BF124_off| BF125_off| BF126_off| BF127_off| BF128_off,
    0xF0,0xF0,                           /* Response Code */
   NORMAL
};

ISO8583_ID LOGON_Response[] =
{
    0xF0,0xF8,0xF1,0xF0,                        /* Message Type */
    0xF9,0xF2,0xF0,0xF0,0xF0,0xF0,              /* Processing Code */
                              /* Bit map is 2038000002000004' */
    BF01_ON  | BF02_ON  | BF03_off | BF04_off | BF05_off | BF06_off | BF07_ON  | BF08_off,
    BF09_off | BF10_off | BF11_ON  | BF12_off | BF13_off | BF14_off | BF15_off | BF16_off,
    BF17_off | BF18_off | BF19_off | BF20_off | BF21_off | BF22_off | BF23_off | BF24_off ,
    BF25_off | BF26_off | BF27_off | BF28_off | BF29_off | BF30_off | BF31_off | BF32_off,
    BF33_ON  | BF34_off | BF35_off | BF36_off | BF37_off | BF38_off | BF39_ON  | BF40_off,
    BF41_off | BF42_off | BF43_off | BF44_off | BF45_off | BF46_off | BF47_off | BF48_off,
    BF49_off | BF50_off | BF51_off | BF52_off | BF53_off | BF54_off | BF55_off | BF56_off,
    BF57_off | BF58_off | BF59_off | BF60_off | BF61_off | BF62_off | BF63_ON  | BF64_off,
    BF65_off | BF66_off | BF67_off | BF68_off | BF69_off | BF70_ON  | BF71_off | BF72_off,
    BF73_off | BF74_off | BF75_off | BF76_off | BF77_off | BF78_off | BF79_off | BF80_off,
    BF81_off | BF82_off | BF83_off | BF84_off | BF85_off | BF86_off | BF87_off | BF88_off,
    BF89_off | BF90_off | BF91_off | BF92_off | BF93_off | BF94_off | BF95_off | BF96_off,
    BF97_off | BF98_off | BF99_off | BF100_off| BF101_off| BF102_off| BF103_off| BF104_off,
    BF105_off| BF106_off| BF107_off| BF108_off| BF109_off| BF110_off| BF111_off| BF112_off,
    BF113_off| BF114_off| BF115_off| BF116_off| BF117_off| BF118_off| BF119_off| BF120_off,
    BF121_off| BF122_off| BF123_off| BF124_off| BF125_off| BF126_off| BF127_off| BF128_off,
    "00",                           /* Response Code */
   NORMAL
};


ISO8583_ID File_Update_Response[] =
{
    0xF0,0xF3,0xF1,0xF2,                        /* Message Type */
    0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,              /* Processing Code */
                              /* Bit map is 'C220000082000002 00008020000000A0' */
    BF01_ON  | BF02_ON  | BF03_off | BF04_off | BF05_off | BF06_off | BF07_ON  | BF08_off,
    BF09_off | BF10_off | BF11_ON  | BF12_off | BF13_off | BF14_off | BF15_off | BF16_off,
    BF17_off | BF18_off | BF19_off | BF20_off | BF21_off | BF22_off | BF23_off | BF24_off ,
    BF25_off | BF26_off | BF27_off | BF28_off | BF29_off | BF30_off | BF31_off | BF32_off,
    BF33_ON  | BF34_off | BF35_off | BF36_off | BF37_off | BF38_off | BF39_ON  | BF40_off,
    BF41_off | BF42_off | BF43_off | BF44_off | BF45_off | BF46_off | BF47_off | BF48_off,
    BF49_off | BF50_off | BF51_off | BF52_off | BF53_off | BF54_off | BF55_off | BF56_off,
    BF57_off | BF58_off | BF59_off | BF60_off | BF61_off | BF62_off | BF63_ON  | BF64_off,
    BF65_off | BF66_off | BF67_off | BF68_off | BF69_off | BF70_off | BF71_off | BF72_off,
    BF73_off | BF74_off | BF75_off | BF76_off | BF77_off | BF78_off | BF79_off | BF80_off,
    BF81_off | BF82_off | BF83_off | BF84_off | BF85_off | BF86_off | BF87_off | BF88_off,
    BF89_off | BF90_off | BF91_ON  | BF92_off | BF93_off | BF94_off | BF95_off | BF96_off,
    BF97_off | BF98_off | BF99_off | BF100_off| BF101_ON | BF102_off| BF103_off| BF104_off,
    BF105_off| BF106_off| BF107_off| BF108_off| BF109_off| BF110_off| BF111_off| BF112_off,
    BF113_off| BF114_off| BF115_off| BF116_off| BF117_off| BF118_off| BF119_off| BF120_ON,
    BF121_off| BF122_ON | BF123_off| BF124_off| BF125_off| BF126_off| BF127_off| BF128_off,
    "00",                           /* Response Code */
   NORMAL
};

ISO8583_ID Reversal_Response[] =
{
    0xF0,0xF4,0xF1,0xF0,                        /* Message Type */
    0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,              /* Processing Code */
                              /* Bit map is 2038000002000004' */
    BF01_ON  | BF02_ON  | BF03_ON  | BF04_ON  | BF05_off | BF06_off | BF07_ON  | BF08_off,
    BF09_off | BF10_off | BF11_ON  | BF12_off | BF13_off | BF14_off | BF15_ON  | BF16_ON ,
    BF17_off | BF18_off | BF19_off | BF20_ON  | BF21_off | BF22_off | BF23_off | BF24_off,
    BF25_off | BF26_off | BF27_off | BF28_off | BF29_off | BF30_off | BF31_off | BF32_ON ,
    BF33_off | BF34_off | BF35_off | BF36_off | BF37_ON  | BF38_off | BF39_ON  | BF40_off,
    BF41_ON  | BF42_off | BF43_off | BF44_off | BF45_off | BF46_off | BF47_off | BF48_ON ,
    BF49_ON  | BF50_ON  | BF51_ON  | BF52_off | BF53_off | BF54_off | BF55_off | BF56_off,
    BF57_off | BF58_off | BF59_off | BF60_off | BF61_off | BF62_off | BF63_off | BF64_off,
    BF65_off | BF66_off | BF67_off | BF68_off | BF69_off | BF70_off | BF71_off | BF72_off,
    BF73_off | BF74_off | BF75_off | BF76_off | BF77_off | BF78_off | BF79_off | BF80_off,
    BF81_off | BF82_off | BF83_off | BF84_off | BF85_off | BF86_off | BF87_off | BF88_off,
    BF89_off | BF90_ON  | BF91_off | BF92_off | BF93_off | BF94_off | BF95_off | BF96_off,
    BF97_off | BF98_off | BF99_off | BF100_off| BF101_off| BF102_off| BF103_off| BF104_off,
    BF105_off| BF106_off| BF107_off| BF108_off| BF109_off| BF110_off| BF111_off| BF112_off,
    BF113_off| BF114_off| BF115_off| BF116_off| BF117_off| BF118_off| BF119_off| BF120_off,
    BF121_off| BF122_off| BF123_off| BF124_off| BF125_off| BF126_off| BF127_off| BF128_off,
    0xF0,0xF0,                           /* Response Code */
   NORMAL
};

ISO8583_ID Balance_Response[] =
{
    0xF0,0xF1,0xF1,0xF0,                        /* Message Type */
    0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,              /* Processing Code */

    BF01_off | BF02_ON  | BF03_ON  | BF04_ON  | BF05_off | BF06_off | BF07_ON  | BF08_off,
    BF09_off | BF10_off | BF11_ON  | BF12_off | BF13_off | BF14_off | BF15_ON  | BF16_off,
    BF17_off | BF18_off | BF19_off | BF20_off | BF21_off | BF22_off | BF23_off | BF24_off,
    BF25_off | BF26_off | BF27_off | BF28_off | BF29_off | BF30_off | BF31_off | BF32_ON ,
    BF33_off | BF34_off | BF35_off | BF36_off | BF37_ON  | BF38_ON  | BF39_ON  | BF40_off,
    BF41_ON  | BF42_off | BF43_off | BF44_off | BF45_off | BF46_off | BF47_off | BF48_ON ,
    BF49_ON  | BF50_off | BF51_off | BF52_off | BF53_off | BF54_ON  | BF55_off | BF56_off,
    BF57_off | BF58_off | BF59_off | BF60_off | BF61_off | BF62_off | BF63_ON  | BF64_off,
    BF65_off | BF66_off | BF67_off | BF68_off | BF69_off | BF70_off | BF71_off | BF72_off,
    BF73_off | BF74_off | BF75_off | BF76_off | BF77_off | BF78_off | BF79_off | BF80_off,
    BF81_off | BF82_off | BF83_off | BF84_off | BF85_off | BF86_off | BF87_off | BF88_off,
    BF89_off | BF90_off | BF91_off | BF92_off | BF93_off | BF94_off | BF95_off | BF96_off,
    BF97_off | BF98_off | BF99_off | BF100_off| BF101_off| BF102_off| BF103_off| BF104_off,
    BF105_off| BF106_off| BF107_off| BF108_off| BF109_off| BF110_off| BF111_off| BF112_off,
    BF113_off| BF114_off| BF115_off| BF116_off| BF117_off| BF118_off| BF119_off| BF120_off,
    BF121_off| BF122_off| BF123_off| BF124_off| BF125_off| BF126_off| BF127_off| BF128_off,
    0xF0,0xF0,                           /* Response Code */
   NORMAL
};



          
/*******************************************************************************************************/
/* ROUTINES                                                                                            */
/*******************************************************************************************************/
struct bitfield_data *Find_Bitfield_Table (enum bitfields);
struct trandef       *Find_Tran_Table     (BYTE trantype);



/********************   FUNCTION PROTOTYPES   ********************************/
char *advance_bit_field_ptr(char *p_current, int bit_field);
long bcd_to_long( pBYTE bcd_data, int bcd_len );
int  bit_is_off(int which_bit, pBYTE p_map);
int  bit_is_ON(int which_bit, pBYTE p_map);
long determine_bit_field_length(char *p_current, int bit_field);
WORD generate_response ();
void host_notify( LPBYTE p_req, WORD len, pIPC_INFO p_ipc_info );
char *lookup_bit_map(char *p_msg, int bit, int *p_field_length);
long parse_host_request(LPBYTE p_msg, WORD length_of_msg);
void update_iso_response();
long ebcdic_to_long( char *fieldid, int length );
BYTE ascii_ebcdic_conv( unsigned char conv_char, char *conv_type );

 
static BOOLEAN pan_used = false;
static LONG    rrn = 10001;                // retrieval reference number - 12 digits ASCII
static WORD    response_buffer_length;     // Length of the response
static BYTE    host_resp_buffer [5500];    // Response Buffer
static BYTE    save_tpdu[5];               // TPDU bytes, if present
static BOOLEAN priority_set = false;

//static ISO_MESSAGE iso_request;               // Global for Request
static ISO_MESSAGE iso_response;              // Global for RESPONSE to Request

/**********************************
     VARIABLES
**********************************/
/* local variables */
INT    reqidx = 0;                  /* next open position in request buffer */
INT    bitmapidx = 4;               /* offset of bitmap... */

/*******************************************************/
/* REQUEST BUILD BUFFER                                */
/*******************************************************/

BYTE visa_header[22];
char error_msg[100];
BYTE tx_key;
BYTE          bitmap = 0;



// Array of bit field lengths
// This should match the ISO_MESSAGE structure exactly.
static pBYTE g_ISO_RESPONSE_PTR[] = 
{
   iso_response.msgtype,       /* Msg Type */
   iso_response.bitmap_bin,    /* Bitmap 1 */
   iso_response.pan_len,       /* Field 02 */
   iso_response.proccode,      /* Field 03 */
   iso_response.amount,        /* Field 04 */
   iso_response.settle_amt,    /* Field 05 */
   iso_response.ch_bill_amt,   /* Field 06 */
   iso_response.gmt_time,      /* Field 07 */
   NULL,                       /* Field 08 */
   iso_response.conv_rate_stl, /* Field 09 */
   iso_response.conv_rate_chb, /* Field 10 */
   iso_response.stan,          /* Field 11 */
   NULL,                       /* Field 12 */
   NULL,                       /* Field 13 */
   NULL,                       /* Field 14 */
   iso_response.date_settle,   /* Field 15 */
   iso_response.conv_date,     /* Field 16 */
   NULL,                       /* Field 17 */
   NULL,                       /* Field 18 */
   NULL,                       /* Field 19 */
   iso_response.ctry_code,     /* Field 20 */
   NULL,                       /* Field 21 */
   NULL,                       /* Field 22 */
   NULL,                       /* Field 23 */
   NULL,                       /* Field 24 */
   NULL,                       /* Field 25 */
   NULL,                       /* Field 26 */
   NULL,                       /* Field 27 */
   NULL,                       /* Field 28 */
   NULL,                       /* Field 29 */
   NULL,                       /* Field 30 */
   NULL,                       /* Field 31 */
   iso_response.acq_inst_len,  /* Field 32 */
   iso_response.fwd_inst_len,  /* Field 33 */
   NULL,                       /* Field 34 */
   NULL,                       /* Field 35 */
   NULL,                       /* Field 36 */
   iso_response.rrn,           /* Field 37 */
   iso_response.authcode,      /* Field 38 */
   iso_response.responsecd,    /* Field 39 */
   NULL,                       /* Field 40 */
   iso_response.termid,        /* Field 41 */
   NULL,                       /* Field 42 */
   NULL,                       /* Field 43 */
   NULL,                       /* Field 44 */
   NULL,                       /* Field 45 */
   NULL,                       /* Field 46 */
   NULL,                       /* Field 47 */
   iso_response.addl_data_len, /* Field 48 */
   iso_response.tran_curr_cd,  /* Field 49 */
   iso_response.settle_cur_cd, /* Field 50 */
   iso_response.card_curr_cd,  /* Field 51 */
   NULL,                       /* Field 52 */
   NULL,                       /* Field 53 */
   iso_response.addl_len,      /* Field 54 */
   NULL,                       /* Field 55 */
   NULL,                       /* Field 56 */
   NULL,                       /* Field 57 */
   NULL,                       /* Field 58 */
   NULL,                       /* Field 59 */
   NULL,                       /* Field 60 */
   iso_response.pos_data_len,  /* Field 61 */
   NULL,                       /* Field 62 */
   iso_response.banknet_len,   /* Field 63 */
   NULL,                       /* Field 64 */
   NULL,                       /* Field 65  */
   NULL,                       /* Field 66  */
   NULL,                       /* Field 67  */
   NULL,                       /* Field 68  */
   NULL,                       /* Field 69  */
   iso_response.nmi_code,      /* Field 70  */
   NULL,                       /* Field 71  */
   NULL,                       /* Field 72  */
   NULL,                       /* Field 73  */
   NULL,                       /* Field 74  */
   NULL,                       /* Field 75  */
   NULL,                       /* Field 76  */
   NULL,                       /* Field 77  */
   NULL,                       /* Field 78  */
   NULL,                       /* Field 79  */
   NULL,                       /* Field 80  */
   NULL,                       /* Field 81  */
   NULL,                       /* Field 82  */
   NULL,                       /* Field 83  */
   NULL,                       /* Field 84  */
   NULL,                       /* Field 85  */
   NULL,                       /* Field 86  */
   NULL,                       /* Field 87  */
   NULL,                       /* Field 88  */
   NULL,                       /* Field 89  */
   iso_response.orig_data,     /* Field 90  */
   iso_response.file_code,     /* Field 91  */
   NULL,                       /* Field 92  */
   NULL,                       /* Field 93  */
   NULL,                       /* Field 94  */
   NULL,                       /* Field 95  */
   NULL,                       /* Field 96  */
   NULL,                       /* Field 97  */
   NULL,                       /* Field 98  */
   NULL,                       /* Field 99  */
   NULL,                       /* Field 100 */
   iso_response.file_name_len, /* Field 101 */
   NULL,                       /* Field 102 */
   NULL,                       /* Field 103 */
   NULL,                       /* Field 104 */
   NULL,                       /* Field 105 */
   NULL,                       /* Field 106 */
   NULL,                       /* Field 107 */
   NULL,                       /* Field 108 */
   NULL,                       /* Field 109 */
   NULL,                       /* Field 110 */
   NULL,                       /* Field 111 */
   NULL,                       /* Field 112 */
   NULL,                       /* Field 113 */
   NULL,                       /* Field 114 */
   NULL,                       /* Field 115 */
   NULL,                       /* Field 116 */
   NULL,                       /* Field 117 */
   NULL,                       /* Field 118 */
   NULL,                       /* Field 119 */
   iso_response.rec_data_len,  /* Field 120 */
   iso_response.mc_agent_len,  /* Field 121 */
   iso_response.addl_rec_len,  /* Field 122 */
   NULL,                       /* Field 123 */
   NULL,                       /* Field 124 */
   NULL,                       /* Field 125 */
   NULL,                       /* Field 126 */
   NULL,                       /* Field 127 */
   NULL                        /* Field 128 */
};

/* Note: 101 means variable length data. */
static int g_ISO_LENGTH[] = 
{
    sizeof(iso_response.msgtype      ), /* Msg Type */
    sizeof(iso_response.bitmap_bin   ), /* Bitmap 1 */
    101,                                /* Field 02 */
    sizeof(iso_response.proccode     ), /* Field 03 */
    sizeof(iso_response.amount       ), /* Field 04 */
    sizeof(iso_response.settle_amt   ), /* Field 05 */
    sizeof(iso_response.ch_bill_amt  ), /* Field 06 */
    sizeof(iso_response.gmt_time     ), /* Field 07 */
    sizeof(NULL                      ), /* Field 08 */
    sizeof(iso_response.conv_rate_stl), /* Field 09 */
    sizeof(iso_response.conv_rate_chb), /* Field 10 */
    sizeof(iso_response.stan         ), /* Field 11 */
    sizeof(NULL                      ), /* Field 12 */
    sizeof(NULL                      ), /* Field 13 */
    sizeof(NULL                      ), /* Field 14 */
    sizeof(iso_response.date_settle  ), /* Field 15 */
    sizeof(iso_response.conv_date    ), /* Field 16 */
    sizeof(NULL                      ), /* Field 17 */
    sizeof(NULL                      ), /* Field 18 */
    sizeof(NULL                      ), /* Field 19 */
    sizeof(iso_response.ctry_code    ), /* Field 20 */
    sizeof(NULL                      ), /* Field 21 */
    sizeof(NULL                      ), /* Field 22 */
    sizeof(iso_response.panext       ), /* Field 23 */
    sizeof(NULL                      ), /* Field 24 */
    sizeof(NULL                      ), /* Field 25 */
    sizeof(NULL                      ), /* Field 26 */
    sizeof(NULL                      ), /* Field 27 */
    sizeof(NULL                      ), /* Field 28 */
    sizeof(NULL                      ), /* Field 29 */
    sizeof(NULL                      ), /* Field 30 */
    sizeof(NULL                      ), /* Field 31 */
    101,                                /* Field 32 */
    101,                                /* Field 33 */
    sizeof(NULL                      ), /* Field 34 */
    sizeof(NULL                      ), /* Field 35 */
    sizeof(NULL                      ), /* Field 36 */
    sizeof(iso_response.rrn          ), /* Field 37 */
    sizeof(iso_response.authcode     ), /* Field 38 */
    sizeof(iso_response.responsecd   ), /* Field 39 */
    sizeof(NULL                      ), /* Field 40 */
    sizeof(iso_response.termid       ), /* Field 41 */
    sizeof(NULL                      ), /* Field 42 */
    sizeof(NULL                      ), /* Field 43 */
    sizeof(NULL                      ), /* Field 44 */
    sizeof(NULL                      ), /* Field 45 */
    sizeof(NULL                      ), /* Field 46 */
    sizeof(NULL                      ), /* Field 47 */
    101,                                /* Field 48 */
    sizeof(iso_response.tran_curr_cd ), /* Field 49 */
    sizeof(iso_response.settle_cur_cd), /* Field 50 */
    sizeof(iso_response.card_curr_cd ), /* Field 51 */
    sizeof(NULL                      ), /* Field 52 */
    sizeof(NULL                      ), /* Field 53 */
    101,                                /* Field 54 */
    sizeof(iso_response.emv          ), /* Field 55 */
    sizeof(NULL                      ), /* Field 56 */
    sizeof(NULL                      ), /* Field 57 */
    sizeof(NULL                      ), /* Field 58 */
    sizeof(NULL                      ), /* Field 59 */
    sizeof(NULL                      ), /* Field 60 */
    101,                                /* Field 61 */
    sizeof(NULL                      ), /* Field 62 */
    101,                                /* Field 63 */
    sizeof(NULL                      ), /* Field 64 */
    sizeof(NULL                      ), /* Field 65 */
    sizeof(NULL                      ), /* Field 66 */
    sizeof(NULL                      ), /* Field 67 */
    sizeof(NULL                      ), /* Field 68 */
    sizeof(NULL                      ), /* Field 69 */
    sizeof(iso_response.nmi_code     ), /* Field 70 */
    sizeof(NULL                      ), /* Field 71 */
    sizeof(NULL                      ), /* Field 72 */
    sizeof(NULL                      ), /* Field 73 */
    sizeof(NULL                      ), /* Field 74 */
    sizeof(NULL                      ), /* Field 75 */
    sizeof(NULL                      ), /* Field 76 */
    sizeof(NULL                      ), /* Field 77 */
    sizeof(NULL                      ), /* Field 78 */
    sizeof(NULL                      ), /* Field 79 */
    sizeof(NULL                      ), /* Field 80 */
    sizeof(NULL                      ), /* Field 81 */
    sizeof(NULL                      ), /* Field 82 */
    sizeof(NULL                      ), /* Field 83 */
    sizeof(NULL                      ), /* Field 84 */
    sizeof(NULL                      ), /* Field 85 */
    sizeof(NULL                      ), /* Field 86 */
    sizeof(NULL                      ), /* Field 87 */
    sizeof(NULL                      ), /* Field 88 */
    sizeof(NULL                      ), /* Field 89 */
    sizeof(iso_response.orig_data    ), /* Field 90 */
    sizeof(iso_response.file_code    ), /* Field 91 */
    sizeof(NULL                      ), /* Field 92 */
    sizeof(NULL                      ), /* Field 93 */
    sizeof(NULL                      ), /* Field 94 */
    sizeof(NULL                      ), /* Field 95 */
    sizeof(NULL                      ), /* Field 96 */
    sizeof(NULL                      ), /* Field 97 */
    sizeof(NULL                      ), /* Field 98 */
    sizeof(NULL                      ), /* Field 99 */
    sizeof(NULL                      ), /* Field 100 */
    101,                                /* Field 101 */
    sizeof(NULL                      ), /* Field 102 */
    sizeof(NULL                      ), /* Field 103 */
    sizeof(NULL                      ), /* Field 104 */
    sizeof(NULL                      ), /* Field 105 */
    sizeof(NULL                      ), /* Field 106 */
    sizeof(NULL                      ), /* Field 107 */
    sizeof(NULL                      ), /* Field 108 */
    sizeof(NULL                      ), /* Field 109 */
    sizeof(NULL                      ), /* Field 110 */
    sizeof(NULL                      ), /* Field 111 */
    sizeof(NULL                      ), /* Field 112 */
    sizeof(NULL                      ), /* Field 113 */
    sizeof(NULL                      ), /* Field 114 */
    sizeof(NULL                      ), /* Field 115 */
    sizeof(NULL                      ), /* Field 116 */
    sizeof(NULL                      ), /* Field 117 */
    sizeof(NULL                      ), /* Field 118 */
    sizeof(NULL                      ), /* Field 119 */
    101,                                /* Field 120 */
    101,                                /* Field 121 */
    101,                                /* Field 122 */
    sizeof(NULL                      ), /* Field 123 */
    sizeof(NULL                      ), /* Field 124 */
    sizeof(NULL                      ), /* Field 125 */
    sizeof(NULL                      ), /* Field 126 */
    sizeof(NULL                      ), /* Field 127 */
    sizeof(NULL                      ), /* Field 128 */
};


/*****************************************************************************

  Function:    Find_Bitfield_Table

  Description: This function will determine whether the bitfield is in the
               bitfield table.
  Author: 
      unknown
  Inputs:
      bitfield - the bitfield to search for
  Outputs:
      None
  Return values:
      pointer to the bitfield data for the bitfield entered.
  Modified by:
******************************************************************************/
static  struct bitfield_data *Find_Bitfield_Table (enum bitfields bitfield)
{
   struct bitfield_data *pBF = NULL;      /* point to return record       */
   INT      i;            /* loop index         */
   CHAR     temp_str[50] = "";

   for (i=0; i <= N_BFData; i++)
      if ( BFData[i].bd_fieldid == bitfield)
      {
         pBF = &BFData[i];
         return (pBF);
         break;
      }
 
   //if (NULL == pBF)
   //{
   //   strcpy( error_msg, "Invalid message type." );
   //   LogEvent( error_msg, ERROR_MSG );
   //}
   return (pBF);
 
} /* Find_Bitfield_Table */


/******************************************************************************
 *
 *  NAME:         ncdci_Find_Tran_Table
 *
 *  DESCRIPTION:  This procedure finds a transaction type in the transaction
 *                table where the transactions are defined.  A pointer to it
 *                is returned.
 *
 *  INPUTS:       p_auth_tx - Transaction record
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   Pointer to the a specific transaction definition in the
 *                transaction table.  If one is not found, NULL is returned.
 *
 *  AUTHOR:       Abhishek Verma
 *
 *  MODIFICATIONS:
 ******************************************************************************/
struct trandef *ncdci_Find_Tran_Table( BYTE trantype )
{
   INT  i = 0;            /* loop index  */
  
   CHAR Buffer[100] = {0};
   //trantype = Auth_Tx.TLF01_details.tx_key;
  
   for ( i=0; (i <= N_TRANENTRY) && (TRANTABLE[i].td_trantype > 0); i++ )
   {
      if ( TRANTABLE[i].td_trantype == trantype )
      {
         return( &TRANTABLE[i] );
         break;
      }
   }

   /* Did not find trantype in table - log error & return NULL */
   //sprintf( Buffer, "This trantype, %d, not found in TRANTABLE", trantype );
   //ncdci_log_message( 1, 2, Buffer, "FIND_TRAN_TABLE" , 0);

   return NULL;
} /* end ncdci_Find_Tran_Table */   

//****************************************************************************
//***
//*** FUNCTION:   advance_bit_field_ptr
//*** PURPOSE:    To advance the bit field pointer by the
//***               appropriate number of bytes for the 
//***               current field. Note that some fields
//***               are variable in nature.
//*** 
//*** ARGUMENTS:  p_current = Points to the current bit
//***                    field. This may reference
//***                           a count of some kind.
//*** 
//***             bit_field = Which bit field to move for
//*** 
//*** RETURN VAL: p_next_field = Pointer to the next bit field
//***                      or NULL if the field is missing
//*** 
//*** 
//****************************************************************************
char *advance_bit_field_ptr(char *p_current, int bit_field)
{
   char *p_next_field = NULL_PTR;    // Pointer to next field
   long current_field_width = 0;   // Field width variable


   // Check arguments
   if ((p_current != NULL) && (bit_field > 1) && (bit_field <= 128))
   {
      current_field_width = determine_bit_field_length(p_current, bit_field);

      // Field was found; now compute the location of the next field
      if (current_field_width > 0)
         p_next_field = p_current + current_field_width;
   }

   return(p_next_field);
}  // end advance_bit_field_ptr



//****************************************************************************
//***
//*** FUNCTION:   bcd_to_long
//*** PURPOSE:    To convert a BINARY CODED DECIMAL string
//***               into an appropriate long integer. This
//***               will handle up to 999,999,999.
//*** 
//*** ARGUMENTS:  bcd_data  = Points to the BCD string 
//***                    that you want to convert.
//***                           This should be BCD only,
//***                         e.g. 0x12 0x34 for 1234.
//*** 
//***             bcd_len   = The length of the string;  
//***                         this controls processing
//*** 
//*** RETURN VAL: result    = The Long Integer equivalent 
//***                           of the BCD value.              
//*** 
//*** 
//****************************************************************************
long bcd_to_long( pBYTE bcd_data, int bcd_len )
{
   long result = 0;    // Long integer equivalent
   long multiplier = 1;   // How much to multiply by
   long i;              // Working variable

   // Check arguments
   if ((bcd_data) && (bcd_len > 0) && (bcd_len < 10))
   {
      // Parse the string from RIGHT to LEFT
      for (i = (bcd_len-1); i >= 0; i--)
      {
         // Add the RIGHT nibble digit
         result+= (bcd_data [i] & 0x0f) * multiplier;
         multiplier = multiplier * 10;

         // Add the LEFT nibble digit
         result+= (bcd_data [i] / 16) * multiplier;
         multiplier = multiplier * 10;
       }
   }

   return (result);
}



//****************************************************************************
//***
//*** FUNCTION:   bit_is_off
//*** PURPOSE:    To check the state of a particular BIT
//***               in the bit map. This examines the bit in
//***               memory and determines the state.
//*** 
//*** ARGUMENTS:  p_map     = Points to the current bit
//***                    map.
//*** 
//***             which_bit = Which bit to check
//*** 
//*** RETURN VAL: turned_off is a TRUTH flag:
//***             1 = BIT is in the OFF state
//***             0 = BIT is ON right now
//*** 
//****************************************************************************
int  bit_is_off(int which_bit, pBYTE p_map)
{
   int turned_off = 0;        // Status of BIT in question is OFF
   pBYTE p_where;    // Location inside the bit map
   unsigned int bit_flag;     // Flag value to check
   unsigned int one = 1;      // Constant

   // Check arguments
   if ((which_bit > 0) && (which_bit < 129) && (p_map != NULL))
   {
      p_where = p_map + (which_bit - 1) / 8;

      // Determine what the bit flag needs to be
      if ((which_bit % 8) == 0)
         bit_flag = one;
      else
         bit_flag = one << (8 - (which_bit % 8));

      // Test bit
      if (*p_where & bit_flag)
         turned_off = 0;      // Bit is turned ON
      else
         turned_off = 1;      // Bit is turned OFF
   }

   return(turned_off);
}  // end bit_is_off



//****************************************************************************
//***
//*** FUNCTION:   bit_is_ON
//*** PURPOSE:    To check the state of a particular BIT
//***               in the bit map. This examines the bit in
//***               memory and determines the state.
//*** 
//*** ARGUMENTS:  p_map     = Points to the current bit
//***                    map.
//*** 
//***             which_bit = Which bit to check
//*** 
//*** RETURN VAL: turned_on is a TRUTH flag:
//***             1 = BIT is in the ON state
//***             0 = BIT is off right now
//*** 
//****************************************************************************
int  bit_is_ON(int which_bit, pBYTE p_map)
{
   int turned_on = 0;         // Status of BIT in question is ON truth
   pBYTE p_where;    // Location inside the bit map
   unsigned int bit_flag;     // Flag value to check
   unsigned int one = 1;      // Constant

   // Check arguments
   if ((which_bit > 0) && (which_bit < 129) && (p_map != NULL))
   {
      p_where = p_map + (which_bit - 1) / 8;

      // Determine what the bit flag needs to be
      if ((which_bit % 8) == 0)
         bit_flag = one;
      else
         bit_flag = one << (8 - (which_bit % 8));


      // Test bit
      if (*p_where & bit_flag)
         turned_on = 1;    // Bit is turned ON
      else
         turned_on = 0;    // Bit is turned OFF
   }

   return(turned_on);
}  // end bit_is_ON



//****************************************************************************
//***
//*** FUNCTION:   determine_bit_field_length
//*** PURPOSE:    To advance the bit field pointer by the
//***               appropriate number of bytes for the 
//***               current field. Note that some fields
//***               are variable in nature.
//*** 
//*** ARGUMENTS:  p_current = Points to the current bit
//***                    field. This may reference
//***                           a count of some kind.
//*** 
//***             bit_field = Which bit field to move for
//*** 
//*** RETURN VAL: width is as follows:
//***             >0 = Valid field length
//***              0 = INVALID Field
//***
//*** MODIFICATIONS:
//***     11/30/98 - DIRBY - Modified to be specific to MasterCard
//*** 
//****************************************************************************
long determine_bit_field_length(char *p_current, int bit_field)
{
   long width = 0;  // Field width variable

    
   // Check arguments
   if ((p_current != NULL) && (bit_field >= 1) && (bit_field <=128))
   {
      switch(bit_field)    // Handle each field distinctly
      {
         case  2:    // PAN
            pan_used = true;
         case 32:    // Acquiring Institution ID Code
         case 33:    // Forwarding Institution ID Code
         case 44:    // Additional Response Data
         case 45:    // Track 1 Data
         case 101:   // File Name
         case 102:   // Account ID-1
         case 103:   // Account ID-2
            width    = ebcdic_to_long(p_current, 2);
            break;

         case 35:    // Track 2 Data
            pan_used = false;  
            width    = ebcdic_to_long(p_current, 2);
            break;

         case  3:    // Processing Code
         case 11:    // STAN
         case 12:    // Transaction TIME
         case 38:    // Authorization Id. Response
            width = 6;
            break;

         case  4:    // Transaction Amount
         case  5:    // Settlement amount
         case  6:    // Cardholder billing amount
         case 37:    // Retrieval Reference Number
            width = 12;
            break;

         case  7:    // Transmission date and time
            width = 10;
            break;

         case 20:    // PAN Country Code
         case 22:    // POS Entry Mode - padded
         case 23:    // PAN Extension
         case 49:    // Transaction currency code
         case 50:    // Settlement currency code
         case 51:    // Cardholder billing currency code
         case 70:    // Network Management Indicator
            width = 3;
            break;

         case 13:    // Transaction DATE
         case 14:    // Expiration DATE
         case 15:    // Settlement DATE
         case 16:    // Conversion DATE
         case 18:    // Merchant Type
         case 24:    // NII   - padded
            width = 4;
            break;

         case 25:    // POS Condition Code
         case 91:    // File Update Code
            width = 1;
            break;

         case 39:    // Response Code
            width = 2;
            break;

         case  9:    // Conversion rate, settlement
         case 10:    // Cardholder billing conversion rate
         case 41:    // Terminal Id
         case 96:    // Message Security Code
            width = 8;
            break;

         case 42:    // Card Acquirer Id
            width = 15;
            break;

         case 43:    // Card Acquirer Name
            width = 40;
            break;

         case 90:    // Original Data
            width = 42;
            break;

         case 48:    // Additional Data - Private
         case 54:    // Additional Amounts
         case 55:    // EMV
         case 61:    // Point of Service (POS) Data
         case 62:    // Intermediate Network Facility (INF) Data
         case 63:    // BankNet Data
         case 120:   // Record Data
         case 121:   // Authorizing Agent ID Code
         case 122:   // Additional Record Data
         case 127:   // Private Data
            width    = ebcdic_to_long(p_current, 3);
            break;

         case 52:    // PIN Data
            width = 8;
            break;

         case 64:    // MAC
            width = 2;
            break;

         default:
            break;
      }

      // Field was found; now compute the location of the next field
   }

   return(width);
}  // end determine_bit_field_length


/******************************************************************************
 *
 *  NAME:         ebcdic_to_long
 *
 *  DESCRIPTION:  This function converts a variable length string from EBCDIC,
 *                into a long.
 *
 *  AUTHOR:       Dennis Irby
 *
 *  INPUTS:       fieldid - Field Id to denote the destination of the data
 *                length  - Number of bytes of fieldid
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   width - Long value of contents of fieldid
 *
 *  MODIFICATIONS:
 ******************************************************************************/
long ebcdic_to_long( char *fieldid, int length )
{
   int                  i;
   long                 width = 0;
   unsigned char        cLen;

   /* Determine number of bytes to move. */
   for( i=0,width=0; i<length; i++ )
   {
      cLen = ascii_ebcdic_conv( fieldid[i], E2A );
      if ( cLen != 0x00 )
         width += (long)( (int)(cLen - 0x30) * (pow(10,(double)(length-i-1))) );
   }
   if ( width > 0 )
      width += length;
   else
      width = 0;
   return( width );

} /* end ebcdic_to_long */


/******************************************************************************
 *
 *  NAME:         ASCII_EBCDIC_CONV
 *
 *  DESCRIPTION:  This function converts an ASCII character into and EBCDIC
 *                character, or visa versa.
 *
 *                    ascii_ebcdic_conv - IBM EBCDIC
 *                    ------------------------------
 *                    Convert ASCII to EBCDIC -or- EBCDIC to ASCII.
 *                    Use input parameter 2, either "A2E" or "E2A".
 *
 *  AUTHOR:       Dennis Irby
 *
 *  INPUTS:       conv_char - Character to be converted
 *                conv_type - A2E or E2A: indicates which way to convert
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   Converted character
 *
 *  MODIFICATIONS:
 ******************************************************************************/
static BYTE ascii_ebcdic_conv( unsigned char conv_char, char *conv_type )
{
   int            i;
   int            found = 0;
   unsigned char  ReturnChar;

      /*
        +------------------------------------------------------------+
        |   0     1     2     3     4     5     6     7     8     9  |
        |   a     b     c     d     e     f     g     h     i     j  |
        |   k     l     m     n     o     p     q     r     s     t  |
        |   u     v     w     x     y     z     A     B     C     D  |
        |   E     F     G     H     I     J     K     L     M     N  |
        |   O     P     Q     R     S     T     U     V     W     X  |
        |   Y     Z           .     <     (     +     |     &     !  |
        |   $     *     )     ;     -     /     ,     %     _     >  |
        |   ?     :     #     @     '     =     "                    |
        +------------------------------------------------------------+
      */

   static unsigned char ebcdic_chart[87] = 
      {
         0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9,
         0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x91,
         0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0xA2, 0xA3,
         0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xC1, 0xC2, 0xC3, 0xC4,
         0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5,
         0xD6, 0xD7, 0xD8, 0xD9, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7,
         0xE8, 0xE9, 0x40, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x5A,
         0x5B, 0x5C, 0x5D, 0x5E, 0x60, 0x61, 0x6B, 0x6C, 0x6D, 0x6E,
         0x6F, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F
      };

   static unsigned char ascii_chart[87] = 
      {
         0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
         0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A,
         0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73, 0x74,
         0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x41, 0x42, 0x43, 0x44,
         0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E,
         0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
         0x59, 0x5A, 0x20, 0x2E, 0x3C, 0x28, 0x2B, 0x7C, 0x26, 0x21,
         0x24, 0x2A, 0x29, 0x3B, 0x2D, 0x2F, 0x2C, 0x25, 0x5F, 0x3E,
         0x3F, 0x3A, 0x23, 0x40, 0x27, 0x3D, 0x22
      };


   if ( strcmp( conv_type, A2E ) == 0 )
   {
      /* CONVERT ASCII TO EBCDIC */
      for( i=0; i<87  &&  !found; i++ )
      {
         if ( conv_char == ascii_chart[i] )
         {
            ReturnChar = ebcdic_chart[i];
            found = 1;
         }
      }
   }
   else if ( strcmp( conv_type, E2A ) == 0 )
   {
      /* CONVERT EBCDIC TO ASCII */
      for( i=0; i<87  &&  !found; i++ )
      {
         if ( conv_char == ebcdic_chart[i] )
         {
            ReturnChar = ascii_chart[i];
            found = 1;
         }
      }
   }

   if ( found == 0 )
      ReturnChar = 0x00;

   return( ReturnChar );
}



//****************************************************************************
//*** 
//***   FUNCTION: generate_response
//*** 
//*** PURPOSE:    To create a response message that is appropriate. This
//***               follows the design document definitions for each 
//***               transaction.
//*** 
//*** ARGUMENTS:  iso_response = Points to the message prototype data area
//***                      This includes all valid data for output
//*** 
//***             host_resp_buffer  = Response data buffer area. This is the 
//***                             network message buffer for output.
//*** 
//*** RETURN VAL: response_len = The number of bytes generated in the buffer
//*** 
//*** MODIFICATIONS:
//***     11/30/98 - DIRBY - Modified to be specific to MasterCard
//***
//****************************************************************************
WORD generate_response ()
{
   struct trandef       *pTranDef;
   struct bitfield_data *pBF;
   WORD        response_len;        // Length of the response generated
   WORD        data_size;           // Size of the resulting message
   int         bit_index;           // Index into bit field table
   pISO8583_ID p_identity;          // Identify of the response, i.e. which transaction

    INT                  bfidx = 0;     /* index to bitfields for this transaction */
   	INT                  bitMapsize = 0;
	BYTE                 temp_len[5] = {0};
	INT			         total_tran_len = 0;

   memset (host_resp_buffer, 0, sizeof(host_resp_buffer));


   // Insert the TPDU if appropriate;
	if (g_using_tpdu_prefix == true)
   {
      host_resp_buffer [0] = save_tpdu [0];
      host_resp_buffer [1] = save_tpdu [3];
      host_resp_buffer [2] = save_tpdu [4];
      host_resp_buffer [3] = save_tpdu [1];
      host_resp_buffer [4] = save_tpdu [2];
      response_len   = sizeof(save_tpdu);
   }
   else 
      response_len = 0;


   ///* 0100 */
   //if ( iso_response.msgtype[0] == 0xF0  && iso_response.msgtype[1] == 0xF1 &&
   //     iso_response.msgtype[2] == 0xF0  && iso_response.msgtype[3] == 0xF0 )
   //{
   //   if ( iso_response.proccode[0] == 0xF3 )
   //      p_identity = Balance_Response;
   //   else
   //      p_identity = Sale_Response;
   //}


   /* 0130 Advice response .. TF Phani Advice_Response */

	/* 1130 Advice response .. Ramya AUthorization Advice_Response */
   /*else*/ 
	//if ( iso_response.msgtype[0] == 0xF1  && iso_response.msgtype[1] == 0xF1 &&
 //       iso_response.msgtype[2] == 0xF1  && iso_response.msgtype[3] == 0xF0 )
 //  {
 //      p_identity = Advice_Response;
 //  }

 //  /* 1314 */

 //  else if ( iso_response.msgtype[0] == 0xF1  && iso_response.msgtype[1] == 0xF3 &&
 //            iso_response.msgtype[2] == 0xF1  && iso_response.msgtype[3] == 0xF4 )
 //     p_identity = File_Update_Response;

 //  /* 0800 */
 //  else if ( iso_response.msgtype[0] == 0xF0  && iso_response.msgtype[1] == 0xF8 &&
 //            iso_response.msgtype[2] == 0xF0  && iso_response.msgtype[3] == 0xF0 )
 //     p_identity = LOGON_Response;

 //  /* 0400 */
 //  else if ( iso_response.msgtype[0] == 0xF0  && iso_response.msgtype[1] == 0xF4 &&
 //            iso_response.msgtype[2] == 0xF0  && iso_response.msgtype[3] == 0xF0 )
 //     p_identity = Reversal_Response;
 //   /* 0420 */
 //  else if ( iso_response.msgtype[0] == 0xF0  && iso_response.msgtype[1] == 0xF4 &&
 //            iso_response.msgtype[2] == 0xF2  && iso_response.msgtype[3] == 0xF0 )
 //     p_identity =Reversal_Advice_Response;

 //  else
 //     response_len = 0;  /* For now, we always want to do 0100's, 0302's and 0800's. */

 //  // Set message type code and Bitmap
 //  memcpy( &host_resp_buffer[response_len], p_identity->msg_type, sizeof(p_identity->msg_type));
 //  response_len += sizeof(p_identity->msg_type);
 //  memcpy (&host_resp_buffer [response_len], p_identity->bit_map, sizeof(p_identity->bit_map));
 //  if ( (p_identity == Sale_Response) || (p_identity == Balance_Response) || (p_identity == Advice_Response) )//|| (p_identity ==Reversal_Advice_Response)
 //     response_len += 8;
 //  else
 //     response_len += sizeof(p_identity->bit_map);

	ncdci_determine_tx_key();


	

	pTranDef = ncdci_Find_Tran_Table( Auth_Tx.TLF01_details.tx_key);
   // Now process the bit map for this transaction type, loading up
   // the response buffer with appropriate data

	bitmapidx = DINERS_TOTALL_MEG_LEN + DINERS_MESSAGE_TYPE_SIZE ;
	memset(&ReqRsp_Buffer, 0, sizeof(ReqRsp_Buffer) );
if(pTranDef!=NULL)
{
	memcpy(&ReqRsp_Buffer[DINERS_TOTALL_MEG_LEN], pTranDef->td_msgid, sizeof(pTranDef->td_msgid));

	if (extendedbit == true)
	{
		bitMapsize = DINERS_EXTENDED_BITMAP_SIZE;
		ReqRsp_Buffer[bitmapidx] |= BITTAB[bf_1xbitfield].bit_mask;   /* 2 bitmaps */
		BitFieldSize = DINERS_EXTENDED_BITFIELD_SIZE;
	}
	else
	{
		bitMapsize = DINERS_NORMAL_BITMAP_SIZE;
		BitFieldSize = DINERS_NORMAL_BITFIELD_SIZE;
	}

	reqidx = bitmapidx + bitMapsize;
	/*-----------------------------------------------------*/
	/* For all requested bitfields for this transaction... */
	/*-----------------------------------------------------*/
	for (bfidx=0; bfidx < BitFieldSize; bfidx++)
	{
		/*----------------------------*/
		/* if not end of bitfields... */
		/*----------------------------*/
		if (pTranDef->td_bitfields[bfidx] != bf_done)
		{
			/*---------------------------------------------------*/
			/* find pointer to bitfield info for this bitfield...*/
			/*---------------------------------------------------*/
			pBF = ncdci_Find_Bitfield_Table(pTranDef->td_bitfields[bfidx]);

			if ( NULL == pBF )
			{
				
				return false; /* Invalid bit was set - so quit. */
			}

			/*-----------------------------------------------------------*/
			/* if conditional routine returns true, move the bitfield... */
			/*-----------------------------------------------------------*/
			if ((pBF->bd_BFCond)(pBF->bd_fieldid,Auth_Tx.TLF01_details.tx_key) == true)
			{
				/*------------------------------*/
				/* set the bit in the bitmap... */
				/*------------------------------*/
				ReqRsp_Buffer[bitmapidx +BITTAB[pTranDef->td_bitfields[bfidx]].byte_no] |=
				BITTAB[pTranDef->td_bitfields[bfidx]].bit_mask;

				/*--------------------------*/
				/* call the move routine... */
				/*--------------------------*/
				(pBF->bd_BFMove)(pBF->bd_fieldid);

			}
		}
		else
		{
			bfidx = BitFieldSize;
		}
	}


	/*Prepare the total message length in header*/
	// We are stroing response message after 1st 4 byte of ReqRsp_Buffer
	total_tran_len = reqidx - DINERS_TOTALL_MEG_LEN ;
	sprintf((char *)temp_len,"%04d",total_tran_len);
	memcpy(&ReqRsp_Buffer[0],temp_len,DINERS_TOTALL_MEG_LEN);
	//AddETX chat at the end of the transaction
	//memcpy(&ReqRsp_Buffer[reqidx],DINERS_ETX_TRN_TEXT,DINERS_TOTALL_MEG_LEN);
	ReqRsp_Buffer[reqidx]=0x03;
	reqidx= reqidx + 1;

	buflen = reqidx;

	     memcpy( (char *)host_resp_buffer, (char *)ReqRsp_Buffer, reqidx );
	     return (reqidx);
   }
   else
      return 0;

   //for (bit_index = 2; bit_index <= 128; bit_index++)
   //{
   //   // If the bit is ON, then include this parameter
   //   if (bit_is_ON(bit_index, p_identity->bit_map))
   //   {
   //      data_size = g_ISO_LENGTH[bit_index];
   //      if ( 101 == data_size )
   //      {
   //         /* Get variable length data. */
   //         if ( 2 == bit_index )
   //            data_size = (WORD)determine_bit_field_length(iso_response.pan_len, bit_index );
   //         else if ( 32 == bit_index )
   //            data_size = (WORD)determine_bit_field_length(iso_response.acq_inst_len, bit_index );
   //         else if ( 33 == bit_index )
   //            data_size = (WORD)determine_bit_field_length(iso_response.fwd_inst_len, bit_index );
   //         else if ( 48 == bit_index )
   //            data_size = (WORD)determine_bit_field_length(iso_response.addl_data_len, bit_index );
   //         else if ( 54 == bit_index )
   //            data_size = (WORD)determine_bit_field_length(iso_response.addl_len, bit_index );
   //         else if ( 55 == bit_index )
   //            data_size = (WORD)determine_bit_field_length(iso_response.emv_len, bit_index );
   //         else if ( 63 == bit_index )
   //            data_size = (WORD)determine_bit_field_length(iso_response.banknet_len, bit_index );
   //         else if ( 101== bit_index )
   //            data_size = (WORD)determine_bit_field_length(iso_response.file_name_len, bit_index );
   //         else if ( 120== bit_index )
   //            data_size = (WORD)determine_bit_field_length(iso_response.rec_data_len, bit_index );
   //         else if ( 121== bit_index )
   //            data_size = (WORD)determine_bit_field_length(iso_response.mc_agent_len, bit_index );
   //         else if ( 122== bit_index )
   //            data_size = (WORD)determine_bit_field_length(iso_response.addl_rec_len, bit_index );
   //      }
   //      memcpy (&host_resp_buffer [response_len], g_ISO_RESPONSE_PTR[bit_index], data_size);
   //      response_len += data_size;
   //   }
   //}
   
   // Finally, return the length of the generated response
   return(response_len);
}  // end generate_response



//****************************************************************************
//***
//*** FUNCTION:   lookup_bit_map
//*** PURPOSE:    To find a particular bit map field in the
//***               current message. This scans both the BIT 
//***               MAP and the message data until it locates
//***               the requested field.
//*** 
//*** ARGUMENTS:  p_msg    = A pointer to the complete 
//***                   Host Request message.
//*** 
//***             bit      = Which bit field to lookup
//*** 
//***             p_field_length = Pointer to an integer
//***                       to hold the field length
//*** 
//*** RETURN VAL: p_result = Pointer to the bit field
//***                       or NULL if the field is missing
//*** 
//*** 
//****************************************************************************
char *lookup_bit_map(char *p_msg, int bit, int *p_field_length)
{
   char *p_result = NULL_PTR;     // By default, return NULL
   char *p_map_byte;       // Points to bit map
   int bit_count = 1;         // Index into bit map for scanning
   int not_found = 1;         // Not found flag 

   // Check arguments
   if ((p_msg) && (bit > 0) && (bit < 128) && (p_field_length))
   {
      /* Get past TPDU, Msg Type, and Bit Map */
      *p_field_length = 0;    // Default to NOT FOUND condition
      if (g_using_tpdu_prefix == true)
         p_map_byte = p_msg + 9; // Provided we have a TPDU

      else
         p_map_byte = p_msg + 4; // When using TCP/IP connection

      if ( p_map_byte[0] & 0x80 )
         p_result = p_map_byte + 16; // Skip past Extended BIT MAP area
      else
         p_result = p_map_byte + 8;  // Skip past BIT MAP area


      // Scan the MESSAGE for the appropriate field
      while (not_found)
      {
         // Identify which BIT FIELD appears next in the BIT MAP
         do
            bit_count++;

         while ((bit_is_off(bit_count, p_map_byte)) && (bit_count <= 128));

         // Have we found the bit field yet?
         if (bit == bit_count)
            not_found = 0;
         else if (bit < bit_count)
         {
            p_result = NULL_PTR;  // Bit field is missing in this message!
            break;
         }
         else
         {
            // Bit field is the wrong one; Advance to the next one
            p_result = advance_bit_field_ptr(p_result, bit_count);
            if (p_result == NULL)
               break;
         }
      }

      // If FOUND, then determine the length of the field as well
      if (p_result)
      {
         // Determine field length for target field
         *p_field_length = determine_bit_field_length(p_result, bit_count);
      }
   }
   
   return(p_result);
}



//****************************************************************************
//***
//*** FUNCTION:   update_iso_response
//*** PURPOSE:    To update the ISO Response Message prototype, using
//***             known variables. For example, this sets the date and time.
//*** 
//*** ARGUMENTS:  NONE
//***
//*** GLOBALS:    The following global variables are used herein
//***             iso_request (read only)
//***             iso_response
//*** 
//*** RETURN VAL: NONE
//*** 
//***   METHOD:
//***    Update the response buffer with specific values:
//***       o Increment the Request Response Number and copy it
//***       o System trace number from the request
//***       o Date in MMDD format (i.e. Month and Day of Month)
//***       o Time when the response was generated
//***       o Length of the response buffer contents
//***
//****************************************************************************
void update_iso_response()
{
   //int  i;
   //int  amount;
   //int  len;
   //char temp_str[10];
   //char temp_amt[13];
   //char temp_addl_data[100];
   ///*
   //**   Update the response buffer with specific values:
   //**      Settlement Amount
   //**      Cardholder billing amount
   //**      Settlement conversion rate
   //**      Cardholder billing conversion rate
   //**      Settlement date
   //**      Conversion date
   //**      PAN Country Code ( USD )
   //**      Authorization ID Response ( 888888 )
   //**      Response Code ( 00 )
   //**      Settlement currency code ( USD )
   //**      Banknet Data  ( MCC )
   //**      Authorizing Agent ID Code
   //*/
   //memset( &iso_response.settle_amt,  0xF0, 12 );
   //memset( &iso_response.ch_bill_amt, 0xF0, 12 );
   //memset( &iso_response.conv_rate_stl, 0xF0, 8 );
   //memset( &iso_response.conv_rate_chb, 0xF0, 8 );
   //memset( &iso_response.date_settle,   0xF0, 4 );
   //memset( &iso_response.conv_date,     0xF0, 4 );
   //memset( &iso_response.ctry_code[0],  0xE4, 1 );
   //memset( &iso_response.ctry_code[1],  0xE2, 1 );
   //memset( &iso_response.ctry_code[2],  0xC4, 1 );
   //memset( &iso_response.authcode[0],   0xF8, 1 );
   //memset( &iso_response.authcode[1],   0xF8, 1 );
   //memset( &iso_response.authcode[2],   0xF8, 1 );
   //memset( &iso_response.authcode[3],   0xF8, 1 );
   //memset( &iso_response.authcode[4],   0xF8, 1 );
   //memset( &iso_response.authcode[5],   0xF8, 1 );
   //memset( &iso_response.responsecd[0], 0xF0, 1 );
   //memset( &iso_response.responsecd[1], 0xF0, 1 );
   //memset( &iso_response.settle_cur_cd[0], 0xE4, 1 );
   //memset( &iso_response.settle_cur_cd[1], 0xE2, 1 );
   //memset( &iso_response.settle_cur_cd[2], 0xC4, 1 );
   //memset( &iso_response.card_curr_cd[0],  0xE4, 1 );
   //memset( &iso_response.card_curr_cd[1],  0xE2, 1 );
   //memset( &iso_response.card_curr_cd[2],  0xC4, 1 );
   //memset( &iso_response.banknet_len[0],   0xF0, 2 );
   //memset( &iso_response.banknet_len[2],   0xF3, 1 );
   //memset( &iso_response.banknet[0],       0xD4, 1 );
   //memset( &iso_response.banknet[1],       0xC3, 2 );
   //memset( &iso_response.mc_agent_len[0],  0xF0, 2 );
   //memset( &iso_response.mc_agent_len[2],  0xF5, 1 );
   //memset( &iso_response.mc_agent_id,      0xF5, 5 );

   ///* Perform various events based on the amount. */
   //memset( temp_amt, 0x00, sizeof(temp_amt) );
   //memcpy( temp_amt, iso_response.amount, 12 );
   //for( i=0; i<12; i++ )
   //{
   //   temp_amt[i] = ascii_ebcdic_conv( temp_amt[i], E2A );
   //}
   //amount = atoi(temp_amt);

   //if ( amount == 199 )
   //{
   //   memset( &iso_response.responsecd[0], 0xF0, 1 );
   //   memset( &iso_response.responsecd[1], 0xF8, 1 );
   //}

   ///* For balance inquiries, set field 54 */
   //if ( iso_response.proccode[0] == 0xF3 )
   //{
   //   /* Length 40 = two sets of subfields */
   //   iso_response.addl_len[0] = 0xF0;
   //   iso_response.addl_len[1] = 0xF4;
   //   iso_response.addl_len[2] = 0xF0;

   //   /* Subfield set 1: current, 840, C, 5250.00 */
   //   memset( &iso_response.addlamts[0],  0xF0, 40 );
   //   memset( &iso_response.addlamts[3],  0xF1,  1 );
   //   memset( &iso_response.addlamts[4],  0xF8,  1 );
   //   memset( &iso_response.addlamts[5],  0xF4,  1 );
   //   memset( &iso_response.addlamts[7],  0xC3,  1 );
   //   memset( &iso_response.addlamts[14], 0xF5,  1 );
   //   memset( &iso_response.addlamts[15], 0xF2,  1 );
   //   memset( &iso_response.addlamts[16], 0xF5,  1 );

   //   /* Subfield set 2: available, 840, D, 100000.00 */
   //   memset( &iso_response.addlamts[23], 0xF2,  1 );
   //   memset( &iso_response.addlamts[24], 0xF8,  1 );
   //   memset( &iso_response.addlamts[25], 0xF4,  1 );
   //   memset( &iso_response.addlamts[27], 0xC4,  1 );
   //   memset( &iso_response.addlamts[32], 0xF1,  1 );
   //}

   ///* Do CVC testing. */
   ///* --------------- */

   ///* Get DE48 Length in ASCII. */
   //memset( temp_str, 0x00, sizeof(temp_str) );
   //for ( i=0; i<3; i++ )
   //{
   //   temp_str[i] = ascii_ebcdic_conv( iso_response.addl_data_len[i], E2A );
   //}
   //len = atoi(temp_str); 

   ///* Get DE48 Data in ASCII. */
   //memset( temp_addl_data, 0x00, sizeof(temp_addl_data) );
   //for( i=0; i<len; i++ )
   //{
   //   temp_addl_data[i] = ascii_ebcdic_conv( iso_response.addl_data[i], E2A );
   //}
   //memset( &iso_response.addl_data[1], 0x00, sizeof(iso_response.addl_data)-1 );

   ///* Does CVC Data exist?  */
   //if ( strstr( temp_addl_data, "9203" ) != NULL )
   //{
   //   /* CVC2 Data does exist. */
   //   memset( &iso_response.addl_data[1], 0xf8, 1 );
   //   memset( &iso_response.addl_data[2], 0xf7, 1 );
   //   memset( &iso_response.addl_data[3], 0xf0, 1 );
   //   memset( &iso_response.addl_data[4], 0xf1, 1 );
   //   memset( &iso_response.responsecd[1],0xF5, 1 );
   //   
   //   if ( amount == 1 )
   //      memset( &iso_response.addl_data[5], 0xd5, 1 );
   //   else if ( amount == 2 )
   //      memset( &iso_response.addl_data[5], 0xd7, 1 );
   //   else if ( amount == 3 )
   //      memset( &iso_response.addl_data[5], 0xe4, 1 );
   //   else if ( amount == 4 )
   //   {
   //      memset( &iso_response.addl_data[5], 0xe8, 1 );
   //      memset( &iso_response.responsecd[1],0xF0, 1 );
   //   }
   //   else
   //   {
   //      memset( &iso_response.addl_data[5], 0xd4, 1 );
   //      memset( &iso_response.responsecd[0],0xF0, 2 );
   //   }

   //   /* Set the length of DE48. */
   //   memset( &iso_response.addl_data_len[0], 0xf0, 1 );
   //   memset( &iso_response.addl_data_len[1], 0xf0, 1 );
   //   memset( &iso_response.addl_data_len[2], 0xf6, 1 );

   //   /* Check for RPS transaction. */
   //   if ( iso_response.pos_data[3] == 0xf4 )
   //   {
   //      if ( (amount%2) == 1 ) /* Odd */
   //      {
   //         /* Return Merchant Advice Code '01' for odd amounts. */
   //         memset( &iso_response.addl_data_len[1], 0xf1, 1 ); // Makes len 12
   //         memset( &iso_response.addl_data_len[2], 0xf2, 1 );
   //         memset( &iso_response.addl_data[6],  0xf8, 1 );
   //         memset( &iso_response.addl_data[7],  0xf4, 1 );
   //         memset( &iso_response.addl_data[8],  0xf0, 1 );
   //         memset( &iso_response.addl_data[9],  0xf2, 1 );
   //         memset( &iso_response.addl_data[10], 0xf0, 1 );
   //         memset( &iso_response.addl_data[11], 0xf1, 1 );
   //      }
   //   }
   //}

   ///* Do CAVV testing. */
   ///* ---------------- */

   //if ( strstr( temp_addl_data, "4207" ) != NULL )
   //{
   //   /* CAVV Data (SecureCode) does exist. */
   //   len = strlen( iso_response.addl_data );
   //   memset( &iso_response.addl_data[len],   0xf4, 1 );
   //   memset( &iso_response.addl_data[len+1], 0xf5, 1 );
   //   memset( &iso_response.addl_data[len+2], 0xf0, 1 );
   //   memset( &iso_response.addl_data[len+3], 0xf1, 1 );
   //   memset( &iso_response.addl_data[len+4], 0xf2, 1 ); /* Approve it,always */

   //   /* Update the length. */
   //   if ( iso_response.addl_data_len[2] == 0xf6 )
   //   {
   //      memset( &iso_response.addl_data_len[1], 0xf1, 2 ); /* Length = 11 */
   //   }
   //   else
   //   {
   //      memset( &iso_response.addl_data_len[0], 0xf0, 1 );
   //      memset( &iso_response.addl_data_len[1], 0xf0, 1 );
   //      memset( &iso_response.addl_data_len[2], 0xf6, 1 );
   //   }
   //}
   ///* Check for RPS transaction. */
   //else if ( iso_response.pos_data[3] == 0xf4 )
   //{
   //   if ( (amount%2) == 1 ) /* Odd */
   //   {
   //      /* Return Merchant Advice Code '02' for odd amounts. */
   //      memset( &iso_response.addl_data_len[0], 0xf0, 1 ); // Makes len 6
   //      memset( &iso_response.addl_data_len[1], 0xf0, 1 );
   //      memset( &iso_response.addl_data_len[2], 0xf7, 1 );
   //      memset( &iso_response.addl_data[1], 0xf8, 1 );
   //      memset( &iso_response.addl_data[2], 0xf4, 1 );
   //      memset( &iso_response.addl_data[3], 0xf0, 1 );
   //      memset( &iso_response.addl_data[4], 0xf2, 1 );
   //      memset( &iso_response.addl_data[5], 0xf0, 1 );
   //      memset( &iso_response.addl_data[6], 0xf2, 1 );
   //   }
   //}

   ///* Fill field 120 & 122 (Record Data) for 0302 Inquiries (Field 91 = 5). */
   //if ( iso_response.file_code[0] == 0xF5 )
   //{
   //   if ( iso_response.file_name[5] == 0xf2 )
   //   {
   //      /* For MCC102 Inquires, respond with activity date and time,
   //       * but do so after the card number.
   //       */

   //      /* Date:  Jan 2, 2003 */
   //      memset( &iso_response.record_data[19], 0xF0, 2 );
   //      memset( &iso_response.record_data[20], 0xF1, 2 );
   //      memset( &iso_response.record_data[21], 0xF0, 2 );
   //      memset( &iso_response.record_data[22], 0xF2, 2 );
   //      memset( &iso_response.record_data[23], 0xF0, 2 );
   //      memset( &iso_response.record_data[24], 0xF3, 2 );

   //      /* Time:  9am */
   //      memset( &iso_response.record_data[25], 0xF0, 2 );
   //      memset( &iso_response.record_data[26], 0xF9, 2 );
   //      memset( &iso_response.record_data[27], 0xF0, 2 );
   //      memset( &iso_response.record_data[28], 0xF0, 2 );

   //      /* Length = 29 */
   //      memset( &iso_response.rec_data_len[0], 0xF0, 1 );
   //      memset( &iso_response.rec_data_len[1], 0xF2, 1 );
   //      memset( &iso_response.rec_data_len[2], 0xF9, 1 );
   //   }

   //   else if ( iso_response.file_name[5] == 0xf3 )
   //   {
   //      /* Only populate DE122 for MCC103 Inquiries. */
   //      memset( &iso_response.addl_rec_len[0], 0xF0, 1 );
   //      memset( &iso_response.addl_rec_len[1], 0xF2, 1 );
   //      memset( &iso_response.addl_rec_len[2], 0xF6, 1 );

   //      /* USA - Mar 3, 2003; Jul 21, 2004 */
   //      memset( &iso_response.addl_rec_data[0],  0xf1, 1 );
   //      memset( &iso_response.addl_rec_data[1],  0xf0, 1 );
   //      memset( &iso_response.addl_rec_data[2],  0xf3, 1 );
   //      memset( &iso_response.addl_rec_data[3],  0xf0, 1 );
   //      memset( &iso_response.addl_rec_data[4],  0xf3, 1 );
   //      memset( &iso_response.addl_rec_data[5],  0xf0, 1 );
   //      memset( &iso_response.addl_rec_data[6],  0xf3, 1 );
   //      memset( &iso_response.addl_rec_data[7],  0xf0, 1 );
   //      memset( &iso_response.addl_rec_data[8],  0xf4, 1 );
   //      memset( &iso_response.addl_rec_data[9],  0xf0, 1 );
   //      memset( &iso_response.addl_rec_data[10], 0xf7, 1 );
   //      memset( &iso_response.addl_rec_data[11], 0xf2, 1 );
   //      memset( &iso_response.addl_rec_data[12], 0xf1, 1 );

   //      /* Asia - Dec 31, 2001; Oct 17, 2005 */
   //      memset( &iso_response.addl_rec_data[13], 0xc3, 1 );
   //      memset( &iso_response.addl_rec_data[14], 0xf0, 1 );
   //      memset( &iso_response.addl_rec_data[15], 0xf1, 1 );
   //      memset( &iso_response.addl_rec_data[16], 0xf1, 1 );
   //      memset( &iso_response.addl_rec_data[17], 0xf2, 1 );
   //      memset( &iso_response.addl_rec_data[18], 0xf3, 1 );
   //      memset( &iso_response.addl_rec_data[19], 0xf1, 1 );
   //      memset( &iso_response.addl_rec_data[20], 0xf0, 1 );
   //      memset( &iso_response.addl_rec_data[21], 0xf5, 1 );
   //      memset( &iso_response.addl_rec_data[22], 0xf1, 1 );
   //      memset( &iso_response.addl_rec_data[23], 0xf0, 1 );
   //      memset( &iso_response.addl_rec_data[24], 0xf1, 1 );
   //      memset( &iso_response.addl_rec_data[25], 0xf7, 1 );
   //   }
   //}
   //return;

	CHAR  timedate[25]="";
   CHAR  action_code[3]="";
   INT   len, i;
   INT   i_action_code;
   INT   found;

   //memset(&ncdci_HOST_MESSAGE,0,sizeof(ncdci_HOST_MESSAGE));
   memcpy(ncdci_HOST_MESSAGE.msgtype, &ReqRsp_Buffer[4], DINERS_MESSAGE_TYPE_SIZE);
  // memset (ncdci_HOST_MESSAGE.authcode,   '7', sizeof(ncdci_HOST_MESSAGE.authcode)  );
   //memcpy (ncdci_HOST_MESSAGE.responsecd, "800", sizeof(ncdci_HOST_MESSAGE.responsecd));
   memcpy(ncdci_HOST_MESSAGE.card_curr_cd, "333" ,sizeof(ncdci_HOST_MESSAGE.card_curr_cd));
   memcpy(ncdci_HOST_MESSAGE.networ_ref_id_len, "001" ,sizeof(ncdci_HOST_MESSAGE.networ_ref_id_len));
   memcpy(ncdci_HOST_MESSAGE.ch_bill_amt,ncdci_HOST_MESSAGE.amount,sizeof(ncdci_HOST_MESSAGE.amount));
   memcpy(ncdci_HOST_MESSAGE.responsecd,RespCodeinTxt,3 );
   memcpy(ncdci_HOST_MESSAGE.authcode,AuthCodeinTxt,6 );
    memcpy(ncdci_HOST_MESSAGE.rrn,AuthCodeinTxt,6 );


  // if ( tx_key == AUTH_CARD_UPDATE_RESPONSE )
   {
      /* In order to test errors, set response code to
       * '28' if card number ends with '00'.
       */
      if ( (ncdci_HOST_MESSAGE.data_record[15] == '0') &&
           (ncdci_HOST_MESSAGE.data_record[16] == '0') )
      {
         memcpy( ncdci_HOST_MESSAGE.responsecd, "28", 2 );
      }
      else if ( ncdci_HOST_MESSAGE.data_record[0] == '5' )
      {
         /* This is an inquiry.  Make up some results. */
         ptetime_get_strtime( timedate );

         /* Action Code: Start from the end and find the first
          * 2 digits that correspond to an action code.
          */
         len = strlen( timedate );
         for( i=len-5,found=0; i>0 && found == 0; i-- )
         {
            memcpy( action_code, &timedate[i-1], 2 );
            if ( action_code[0] == '.' )
               action_code[0] = '0';
            if ( action_code[1] == '.' )
               action_code[1] = '0';
            i_action_code = atoi( action_code );
            switch( i_action_code )
            {
               case 1:
               case 4:
               case 5:
               case 7:
               case 41:
               case 43:
                  memcpy( &ncdci_HOST_MESSAGE.data_record[17], action_code, 2 );
                  found = 1;
                  break;
            }
         }
         if ( found == 0 )
         {
            memset( action_code, 0x00, sizeof(action_code) );
            for( i=len-1,found=0; i>0 && found == 0; i-- )
            {
               memcpy( action_code, &timedate[i], 1 );
               i_action_code = atoi( action_code );
               switch( i_action_code )
               {
                  case 1:
                  case 4:
                  case 5:
                  case 7:
                     memcpy( &ncdci_HOST_MESSAGE.data_record[17], "0", 1 );
                     memcpy( &ncdci_HOST_MESSAGE.data_record[18], action_code, 1 );
                     found = 1;
                     break;
               }
            }
         }
         if ( found == 0 )
            /* Default, if cannot find an action code in time string. */
            memcpy( &ncdci_HOST_MESSAGE.data_record[17], "07", 2 );

         /* Purge Date - Current year and month */
         ptetime_get_timestamp( timedate );
         memcpy( &ncdci_HOST_MESSAGE.data_record[19], timedate,   4 );
         memcpy( &ncdci_HOST_MESSAGE.data_record[23], timedate+5, 2 );

         /* Stop Regions - Use bits of last 2 digits of timedate. */
         len = strlen( timedate );

         ncdci_HOST_MESSAGE.data_record[25] = (timedate[len-2] & 0x01) ? '1':'0';  /* Japan  */
         ncdci_HOST_MESSAGE.data_record[26] = (timedate[len-1] & 0x08) ? '1':'0';  /* Asia   */
         ncdci_HOST_MESSAGE.data_record[27] = (timedate[len-1] & 0x04) ? '1':'0';  /* USA    */
         ncdci_HOST_MESSAGE.data_record[28] = (timedate[len-1] & 0x02) ? '1':'0';  /* Europe */
         ncdci_HOST_MESSAGE.data_record[29] = (timedate[len-1] & 0x01) ? '1':'0';  /* Local  */
      }

      /* Determine length of field 120. */
      ncdci_HOST_MESSAGE.data_record_len[0] = 0x00;
      ncdci_HOST_MESSAGE.data_record_len[1] = strlen( (char *)ncdci_HOST_MESSAGE.data_record );
   }
   return;

}  // end update_iso_response



//****************************************************************************
//***
//*** FUNCTION:   parse_host_request
//*** PURPOSE:    To examine the incoming request. This
//***               determines the value of certain fields.
//***               It always returns.
//*** 
//*** ARGUMENTS:  p_msg    = A pointer to the complete 
//***                   Host Request message.
//*** 
//***             length_of_msg = The length of the message
//*** 
//*** RETURN VAL: 0  = Request was correctly processed
//***              >0  = Request is formatted INCORRECTLY
//*** 
//*** NOTES:      This function should be modified to get
//***          additional data from incoming requests.
//*** 
//*** MODIFICATIONS:
//***     11/30/98 - DIRBY - Modified to be specific to MasterCard
//*** 
//****************************************************************************
long parse_host_request(LPBYTE p_msg, WORD length_of_msg)
{
   struct trandef       *pTranDef;
   struct bitfield_data *pBF;
	
   long validity = 0;     // Validity processing flag
   char *p_section;       // Points to section of the message that contains the field data
   long width_of_field;   // Width of field
   int  logon_msg       = 0;  // 0 = Not a logon message,   1 = It is a logon message
   int  file_update_msg = 0;  // 0 = Not a file update msg, 1 = It is a file update msg
   int  auth_msg        = 0;  // 0 = Not an authorization,  1 = It is an authorization
   int  rev_msg         = 0;  // 0 = Not a reversal,        1 = It is a reversal
   int  bal_inq_msg     = 0;  // 0 = Not a balance inquiry, 1 = It is a balance Inquiry
   int  rev_advice_msg  = 0;  // 0 = Not an authorization,  1 = It is an authorization
   int bfidx = 0;


   memset (&ncdci_HOST_MESSAGE, 0, sizeof(ncdci_HOST_MESSAGE));

   // Check arguments for validity  
   if ((p_msg == NULL_PTR) || (length_of_msg == 0))
      return (255);

   // Get message type
   if (g_using_tpdu_prefix == true)
      memcpy(ncdci_HOST_MESSAGE.msgtype, p_msg + sizeof(save_tpdu), sizeof(ncdci_HOST_MESSAGE.msgtype));
   else
      memcpy(ncdci_HOST_MESSAGE.msgtype, p_msg, sizeof(ncdci_HOST_MESSAGE.msgtype));

   memset(&ReqRsp_Buffer, 0, sizeof(ReqRsp_Buffer) );
   memcpy(ReqRsp_Buffer, p_msg, sizeof(ReqRsp_Buffer));
   //ncdci_copy_to_req_resp_buffer( p_msg );
   buflen = 350;

   //   /* Determine transaction type. */
   //if( (ReqRsp_Buffer[1] == 0xf0) && (ReqRsp_Buffer[1] == 0xf1) &&
   //    (ReqRsp_Buffer[2] == 0xf0) && (ReqRsp_Buffer[3] == 0xf0) )
   //{
   //   tx_key = AUTH_AUTHORIZATION_RESPONSE;
   //}
   //else if( (ReqRsp_Buffer[1] == 0xf0) && (ReqRsp_Buffer[1] == 0xf4) &&
   //         (ReqRsp_Buffer[2] == 0xf2)  )
   //{
   //   tx_key = AUTH_REVERSAL_ADVICE_RESPONSE;
   //}
   //else if( (ReqRsp_Buffer[1] == 0xf0) && (ReqRsp_Buffer[1] == 0xf8) &&
   //         (ReqRsp_Buffer[2] == 0xf0) && (ReqRsp_Buffer[3] == 0xf0) )
   //{
   //   tx_key = AUTH_LOGON_RESPONSE;
   //}
   //else if( (ReqRsp_Buffer[1] == 0xf0) && (ReqRsp_Buffer[1] == 0xf3) &&
   //         (ReqRsp_Buffer[2] == 0xf0) && (ReqRsp_Buffer[3] == 0xf2) )
   //{
   //   tx_key = AUTH_CARD_UPDATE_RESPONSE;
   //}
   //else
   //{
   //   sprintf( error_msg, "Invalid message type." );
   //   return false;
   //}

   	/* Initialize thencdci_HOST_MESSAGE structure. */
	memset(&ncdci_HOST_MESSAGE, 0x00, sizeof (ncdci_HOST_MESSAGE));

	// Point to Message type
	reqidx =  DINERS_TOTALL_MEG_LEN ;

	/*Reterive message Type*/
	memcpy(ncdci_HOST_MESSAGE.msgtype, &ReqRsp_Buffer[reqidx], DINERS_MESSAGE_TYPE_SIZE);

    // Point to Bitmap postion
	reqidx = reqidx + DINERS_MESSAGE_TYPE_SIZE;
	bitmap = ReqRsp_Buffer[reqidx];
	bitmapidx = reqidx;

   // Determine message type.
   //if ( (iso_response.msgtype[1] == 0xf8) && (iso_response.msgtype[2] == 0xf0) )
   //{
   //   logon_msg = 1;
   //}

  // if ( (iso_response.msgtype[0] == 0xf1) && (iso_response.msgtype[1] == 0xf8) && (iso_response.msgtype[2] == 0xf0) )
  // {
  //    logon_msg = 1;
  // }
  // else if ( (iso_response.msgtype[0] == 0xf1) && (iso_response.msgtype[1] == 0xf3) )
  // {
  // }
  // else if ( (iso_response.msgtype[1] == 0xf1) && (iso_response.msgtype[1] == 0xf1) && (iso_response.msgtype[2] == 0xf0) )
  // {
  //    if ( iso_response.proccode[0] == 0xf3 )
  //       bal_inq_msg = 1;
  //    else
  //       auth_msg = 1;
  // }
  // else if (( iso_response.msgtype[0] == 0xf1 )&&( iso_response.msgtype[1] == 0xf4 ) && (iso_response.msgtype[2] == 0xf0)) /*||
		//	(( iso_response.msgtype[1] == 0xf4 ) && (iso_response.msgtype[2] == 0xf2) ) )*/
  // {
  //    rev_msg = 1;
  // }
  // // TF PHANI.. change for Advice
  // //ramya
  //// else if ((iso_response.msgtype[1] == 0xf1) && (iso_response.msgtype[2] == 0xf2) )
  //// {
		////auth_msg = 1;
  //// }
  //    else if ((iso_response.msgtype[0] == 0xf1)&&(iso_response.msgtype[1] == 0xf1) && (iso_response.msgtype[2] == 0xf2) )
  // {
		//auth_msg = 1;
  // }
  // else
  // {
  //    return( -1 );
  // }


	  /* Point past bitmap, but 1st check for extended bitmap. */

	if ( bitmap &  BITTAB[bf_1xbitfield].bit_mask )
	{
		/* Extended     */
		reqidx = reqidx + DINERS_EXTENDED_BITMAP_SIZE;
		BitFieldSize = DINERS_EXTENDED_BITFIELD_SIZE;
	}
	else
	{
		reqidx = reqidx + DINERS_NORMAL_BITMAP_SIZE;
		BitFieldSize = DINERS_NORMAL_BITFIELD_SIZE;
	}

	/*------------------------------------------------------------------
	For every bitfield present in message ( except first bit ) . . .
	-------------------------------------------------------------------*/
	for( bfidx = 2; bfidx <= BitFieldSize; bfidx++ )
	{
		if ( ReqRsp_Buffer[bitmapidx + BITTAB[bfidx].byte_no] &
			 BITTAB[bfidx].bit_mask )
		{
			/* Bit is on, field is there.... */
			pBF = ncdci_Find_Bitfield_Table(bfidx);

			if ( NULL != pBF )
			{
				if(bfidx>64)
				{
					extendedbit=1;
				}
				(pBF->bd_BFRsp)(pBF->bd_fieldid);
			}
		}
	}


   //// Get PAN
   // if ((p_section = lookup_bit_map(p_msg, bf_2pan, &width_of_field)) != NULL)
   //   memcpy(iso_response.pan_len, p_section, width_of_field);
   //else if ( auth_msg || bal_inq_msg )
   //  validity = 2;    // Error in Bit Field

   //// Get processing code
   //if ((p_section = lookup_bit_map(p_msg, bf_3proc, &width_of_field)) != NULL)
   //  memcpy(iso_response.proccode, p_section, width_of_field);
   //else if ( auth_msg || bal_inq_msg )
   //  validity = 3;    // Error in Bit Field

   //// Get amount
   //if ((p_section = lookup_bit_map(p_msg, bf_4amount, &width_of_field)) != NULL)
   //  memcpy(iso_response.amount, p_section, width_of_field);
   //else if ( auth_msg || bal_inq_msg )
   //  validity = 4;    // Error in Bit Field

   //// Get GMT time
   //if ((p_section = lookup_bit_map(p_msg, bf_7trntime, &width_of_field)) != NULL)
   //   memcpy(iso_response.gmt_time, p_section, width_of_field);
   //else
   //   validity = 7;      // Error in Bit Field

   //// Get system trace number
   //if ((p_section = lookup_bit_map(p_msg, bf_11stan, &width_of_field)) != NULL)
    // memcpy(iso_response.stan, p_section, width_of_field);
   //else
   //   validity = 11;      // Error in Bit Field

   //// Get PAN Extension
   //if ((p_section = lookup_bit_map(p_msg, bf_23panext, &width_of_field)) != NULL)
   //   memcpy(iso_response.panext, p_section, width_of_field);

   //// Get Acquiring Institution ID Code
   //if ((p_section = lookup_bit_map(p_msg, bf_32idcode, &width_of_field)) != NULL)
   //   memcpy(iso_response.acq_inst_len, p_section, width_of_field);
   //else if ( auth_msg || bal_inq_msg )
   //   validity = 32;      // Error in Bit Field

   //// Get Forwarding Institution ID Code
   //if ((p_section = lookup_bit_map(p_msg, bf_33fwdid, &width_of_field)) != NULL)
   //   memcpy(iso_response.fwd_inst_len, p_section, width_of_field);
   //else if ( logon_msg || file_update_msg )
   //   validity = 33;      // Error in Bit Field

   //// Get RRN
   //if ((p_section = lookup_bit_map(p_msg, bf_37rrn, &width_of_field)) != NULL)
   //   memcpy(iso_response.rrn, p_section, width_of_field);
   //else if ( auth_msg || bal_inq_msg )
   //   validity = 37;      // Error in Bit Field

   //// Get terminal identifier
   //if ((p_section = lookup_bit_map(p_msg, bf_41termid, &width_of_field)) != NULL)
   //   memcpy(iso_response.termid, p_section, width_of_field);
   //else if ( auth_msg || bal_inq_msg )
   //   validity = 41;      // Error in Bit Field

   //// Get Additional Data
   //if ((p_section = lookup_bit_map(p_msg, bf_48addldata, &width_of_field)) != NULL)
   //   memcpy(iso_response.addl_data_len, p_section, width_of_field);
   //else if ( auth_msg || bal_inq_msg )
   //   validity = 48;      // Error in Bit Field

   //// Get Currency Code
   //if ((p_section = lookup_bit_map(p_msg, bf_49trncurcd, &width_of_field)) != NULL)
   //   memcpy(iso_response.tran_curr_cd, p_section, width_of_field);
   //else if ( auth_msg || bal_inq_msg )
   //   validity = 49;      // Error in Bit Field

   //// Get EMV Info
   // if ((p_section = lookup_bit_map(p_msg, bf_55emv, &width_of_field)) != NULL)
   //   memcpy(iso_response.emv_len, p_section, width_of_field);

   //// Get POS Data
   //if ((p_section = lookup_bit_map(p_msg, bf_61posdata, &width_of_field)) != NULL)
   //   memcpy(iso_response.pos_data_len, p_section, width_of_field);
   //else if ( auth_msg || bal_inq_msg )
   //   validity = 61;      // Error in Bit Field


   //// Get NMI Code
   //if ((p_section = lookup_bit_map(p_msg, bf_70nmicode, &width_of_field)) != NULL)
   //   memcpy(iso_response.nmi_code, p_section, width_of_field);
   //else if ( logon_msg )
   //   validity = 70;      // Error in Bit Field

   //// Get Original Data
   //if ((p_section = lookup_bit_map(p_msg, bf_90orgdata, &width_of_field)) != NULL)
   //   memcpy(iso_response.orig_data, p_section, width_of_field);
   //else if ( rev_msg )
   //   validity = 90;      // Error in Bit Field

   //// Get File Update Code
   //if ((p_section = lookup_bit_map(p_msg, bf_91filecode, &width_of_field)) != NULL)
   //   memcpy(iso_response.file_code, p_section, width_of_field);
   //else if ( file_update_msg )
   //   validity = 91;      // Error in Bit Field

   //// Get Message Security Code
   //if ((p_section = lookup_bit_map(p_msg, bf_96security, &width_of_field)) != NULL)
   //   memcpy(iso_response.security_code, p_section, width_of_field);
   //else if ( file_update_msg )
   //   validity = 96;      // Error in Bit Field

   //// Get File Name
   //if ((p_section = lookup_bit_map(p_msg, bf_101filename, &width_of_field)) != NULL)
   //   memcpy(iso_response.file_name_len, p_section, width_of_field);
   //else if ( file_update_msg )
   //   validity = 101;      // Error in Bit Field

   //// Get Record Data
   //if ((p_section = lookup_bit_map(p_msg, bf_120orgmsgid, &width_of_field)) != NULL)
   //   memcpy(iso_response.rec_data_len, p_section, width_of_field);
   //else if ( file_update_msg )
   //   validity = 120;      // Error in Bit Field

   //return(validity);
     return true;
}  // end parse_host_request


/******************************************************************************
 *
 *  NAME:         ncdci_copy_to_req_resp_buffer
 *
 *  DESCRIPTION:  This function will copy data from a PTE message into
 *                ReqRsp_Buffer.
 *
 *  INPUTS:       p_msg - This is the incoming message from the host.
 *
 *  OUTPUTS:      ReqRsp_Buffer - (Global)Character string with txn data
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncdci_copy_to_req_resp_buffer ( LPBYTE p_msg )
{
   pPTE_MSG_DATA    p_msg_data  = NULL_PTR;
   pBYTE            p_data      = NULL_PTR;
   LONG             msglen = 0;

   memset(ReqRsp_Buffer, 0x00, DINERS_COMM_BUF_SZE);   /* zero out the buffer */

 //  p_msg_data = ptemsg_get_pte_msg_data( p_msg );
 //  msglen     = ptemsg_get_pte_msg_data_data_length( p_msg );
 //  p_data     = ptemsg_get_pte_msg_data_data( p_msg );

 //  reqidx = 0;
 //  buflen = msglen;
	//if(buflen >= DINERS_COMM_BUF_SZE)
	//{
	//	invalid_msg_format = true;
	//}else
	//{ memcpy(ReqRsp_Buffer, p_data, msglen);
	//	invalid_msg_format = false;
	//}
   /* Retrieve originator info from incoming message */
   //strcpy( Orig_Info, ptemsg_get_msg_orig_comms_info( p_msg_in ) );
   //strcpy( Orig_Queue, ptemsg_get_msg_orig_queue( p_msg_in ) );

   return;
}


/******************************************************************************
 *
 *  NAME:         ncdci_Find_Bitfield_Table
 *
 *  DESCRIPTION:  This procedure finds a bitfield definition in the bitfield
 *                table where the bitfields are defined.  A pointer to it
 *                is returned.
 *
 *  INPUTS:       bitfield - ID of bitfield definition we are looking for
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   Pointer to the a specific bitfield definition in the
 *                bitfield table.  If one is not found, NULL is returned.
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
struct bitfield_data *ncdci_Find_Bitfield_Table( enum bitfields bitfield )
{
   INT      i = 0;                    /* loop index */
   CHAR     temp_str[60] = {0};

   for ( i=0; i <= N_BFData; i++ )
      if ( BFData[i].bd_fieldid == bitfield )
         return &BFData[i];  /* Found the field - return address */


   
   return NULL;

} /* end ncdci_Find_Bitfield_Table */



//****************************************************************************
//***
//*** FUNCTION:   host_notify
//*** PURPOSE:    To initiate processing of an incoming
//***               request to the Host. This function will
//***               cause a response to be generated and sent.
//*** 
//*** ARGUMENTS:  p_req = A pointer to the complete message,
//***                 i.e. the Host Request.
//*** 
//***             len    = The length of the complete 
//***                 message in bytes.
//*** 
//*** NOTES:      This function has to initiate the processing
//***          cycle for incoming requests.
//*** 
//*** 
//****************************************************************************
void host_notify( LPBYTE p_req, WORD len, pIPC_INFO p_ipc_info )
{
   long field_error = 0;     // Error field, if any
   int  write_status; 
   unsigned char temp_msg[1000];
   /*char field_message[64];*/      // Error message, if any


   if (priority_set == false)
   {
      SetThreadPriority (GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
      priority_set = true;
   }


   init_trace();
   //MessageBox( NULL, "Incoming request received", NULL, MB_OK );
   write_to_comm_trace_file( p_req, len, '0' );

   memcpy( temp_msg, p_req, len );

   // Initialize
   if (g_using_tpdu_prefix == true)
   {
      // Save TPDU for later use
      memcpy (save_tpdu, p_req, sizeof(save_tpdu));
   }

   // Parse the incoming message
   field_error = parse_host_request (p_req, len);

   if (field_error == 0)
   {
      // Report the bad field to the user
      printf("ERROR: Bit field %d is invalid!", field_error);
      return;
   }

   // Update the message prototype to provide all information
   update_iso_response ();

   // Now generate a response message
   response_buffer_length = generate_response ();

   //MessageBox( NULL, "Request Processed", NULL, MB_OK );
   // Now write the response out to the network
   tcp_write(response_buffer_length, host_resp_buffer);

   // Temp added the delay for 2 seconds
   //Sleep(2000);

   if (response_buffer_length > 0) 
   {
      //MessageBox( NULL, "Sending Response", NULL, MB_OK );
      write_to_comm_trace_file( host_resp_buffer, response_buffer_length, '1' );
      write_status = port_write(response_buffer_length, host_resp_buffer);
      
      if (!write_status)
         connection_up = FALSE;
   }
}  // end host_notify



/******************************************************************************
 *
 *  NAME:         ncdci_determine_tx_key
 *
 *  DESCRIPTION:  This function determines the tx_key based on message type
 *                and processing code.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   False if unable to determine tx_key, else true
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT ncdci_determine_tx_key( )
{
   INT   ret_val = true;
   CHAR  err_buf[100] = {0};
   CHAR  msgtype[5] = {0};
   CHAR  procode[7] = {0};

  memcpy( msgtype, ncdci_HOST_MESSAGE.msgtype,   DINERS_MESSAGE_TYPE_SIZE );
   // memcpy( msgtype, "1100",   DINERS_MESSAGE_TYPE_SIZE );

   /* 11xx */
   if ( msgtype[1] == '1' )
   {
      if ( msgtype[2] == '0' )
      {
         /* Request */
            Auth_Tx.TLF01_details.tx_key = AUTH_AUTHORIZATION_RESPONSE;
			

      }
	  else if ( msgtype[2] == '2' )
      {
         /* Advice Request */
            Auth_Tx.TLF01_details.tx_key = AUTH_ADVICE_RESPONSE;
	
      }
	  else
			ret_val = false;
   }

   /* 12xx */
   else if ( msgtype[1] == '2' )
   {
      if ( msgtype[2]=='0')
	  {
         Auth_Tx.TLF01_details.tx_key = AUTH_SALE_CASH_RESPONSE;

	  }
      else
         ret_val = false;

   }

   /* 13xx */
   else if ( msgtype[1] == '3' )
   {
      if ( msgtype[2] == '0' )
      {
         /* Request */
         Auth_Tx.TLF01_details.tx_key = AUTH_CARD_UPDATE_RESPONSE;

      }
   }

   /* 14xx */
   else if ( msgtype[1] == '4' )
   {
      if (( msgtype[2] == '2' ) || ( msgtype[2] == '0' ))
      {
         /* Request */
         Auth_Tx.TLF01_details.tx_key = AUTH_REVERSAL_ADVICE_RESPONSE;

      }
   }

   /* 18xx */
   else if ( msgtype[1] == '8' )
   {
      if ( msgtype[2] == '0' )
      {
         /* Request */
         Auth_Tx.TLF01_details.tx_key = AUTH_LOGON_RESPONSE;

      }
      else
         ret_val = false;
   }


   return (ret_val);
  
}


