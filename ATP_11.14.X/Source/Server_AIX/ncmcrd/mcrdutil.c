/******************************************************************************

   MCRDUTIL.C

   Copyright (c) 2007, Hypercom, Inc.
   All Rights Reserved.

   TITLE:  MasterCard Host Handler Utilities

   This module is a part of the MasterCard Host Handler.  It contains all
   the utilities used by the host handler to parse and manipulate all incoming
   messages from the terminal and from the host.

   APPLICATION:  Advanced Transaction Processor (ATP)

   AUTHOR:  Dennis Irby

 
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#ifdef WIN32
#include <conio.h>
#endif
#include <string.h>
#include <time.h>
#include <math.h>
#include "ptemsg.h"
#include "basictyp.h"
#include "app_info.h"
#include "equitdb.h"
#include "nc_database.h"
#include "nc_dbstruct.h"
#include "pteipc.h"
#include "ptellist.h"
#include "ntutils.h"
#include "ptetimer.h"
#include "ptetime.h"
#include "memmnger.h"
#include "txutils.h"
#include "mcrdmain.h"
#include "mcrdutil.h"
#include "genutil.h"
#include "ncutils.h"
#include "txtrace.h"
#include "tx_dbstruct.h"


/* GLOBAL VARIABLES */
extern CHAR  Stan[7];
extern CHAR  Logon_NMI_Code[4];
extern INT   Tran_Flow;


extern CHAR  ncmcrd_Error_warning_Filename[256];
extern CHAR  ncmcrd_module_error_warning_file_name[256];
extern CHAR  ncmcrd_error_warning_file_path[256];
extern CHAR  Log_Bufr[512]; /* used to copy term id, card num and merch id to error msg
	   	   	   	   	   	   	   	   and passed it to print in File and monitor */
CHAR masked_card_num[20] = {0};

UINT Max_File_Size_Defined = 0 ;
/* Queue definitions */
extern CHAR  updateds_que_name[];
extern CHAR  cur_app_name [];
extern CHAR  authorizer_que_name[];
extern CHAR  dialog_que_name [];
extern CHAR  applnk_que_name [];
extern CHAR  trands_que_name[];
extern CHAR  devds_que_name[];
extern CHAR  netds_que_name[];
extern CHAR  security_que_name[];
extern CHAR  AppName[8];

/* Database structures */
extern NCF01               ncf01_i;
extern NCF01               ncf01_a;
extern NCF30               ncf30;
extern TLF01               tlf01;
extern MCC_ONLINE_UPDATES  Mcc_Gui;
extern BDKKEY01  Bdkkey01;

/* Database structure sizes */
extern INT    Tlf01_Size;
extern INT    Ncf01_Size;
extern INT    Ncf30_Size;
extern INT    Auth_Tx_Size;
extern INT    Mcc_Gui_Size;
extern INT	   Bdkkey01_Size;

/* Shared memory variables */
extern BYTE   net_consec_tmouts_ctr[4];
extern BYTE   active_txns_ctr[4];
extern BYTE   current_state[2];
extern CHAR   SHARED_MEMORY_TBL[15];

/* Statistics for Monitor */
extern TXN_COUNTS     Txn_Counts;
extern MONITOR_STATS  Monitor_Stats;

/* Txn Tracing Info */
extern CHAR   TraceFile[];
extern INT    Tracing;
extern FILE   TraceFile_fp;
extern FILE   *pTraceFile_fp;
extern FILE   **ppTraceFile_fp;

/* These are used for reject 0620/0630 messages. */
extern CHAR   fwd_inst_id_len[3];
extern CHAR   fwd_inst_id[12];
extern CHAR   addl_data_len[4];
extern CHAR   addl_data[999];
extern CHAR   advice_len[4];
extern CHAR   advice[61];
extern CHAR   recv_inst_len[3];
extern CHAR   recv_inst[12];
extern CHAR   F60_len[4];
extern CHAR   reject_advice_code[4];
extern CHAR   reject_detail_code[5];
extern CHAR   reject_text[54];

extern INT    reqidx;                 /* next open position in request buffer */
extern INT    buflen;                 /* total size of buffer */
extern INT    bitmapidx;              /* offset of bitmap... */
extern INT    invalid_msg_format;     /* flag used in parsing host message */

extern BYTE   ReqRsp_Buffer[COMM_BUF_SZE];
extern int    BitFieldSize;

extern char   A2E[4];    /*  ASCII to EBCDIC conversion */
extern char   E2A[4];    /*  EBCDIC to ASCII conversion */

/* Acq 3D secure version storage variable */
CHAR Mc3Dsecureversion = 0;
CHAR mc_asi_txn_rc[3]={0};
CHAR mc_scheme_timeout_rc[3]={0};
CHAR mc_merchnt_pgway_id [12] = {0};
extern CHAR response_code_for_Incoming_timeout_Transaction[3];

INT Build_DE48_tag87_from_Tag92=0;

extern char name_validation_flag;
extern char mc_acceptor_country_code[4];

/* LOGON PAN GROUP ID*/
#define LOG_ON_GROUP_ID_PAN_LEN 5

const struct isobit BITTAB[128+1] =
{
   { 0, 0 }, /*zero justify */
   { 0, 0x80 },    /* bit 1  */   { 0, 0x40 },    /* bit 2  */   { 0, 0x20 },    /* bit 3  */   { 0, 0x10 },    /* bit 4  */
   { 0, 0x08 },    /* bit 5  */   { 0, 0x04 },    /* bit 6  */   { 0, 0x02 },    /* bit 7  */   { 0, 0x01 },    /* bit 8  */
   { 1, 0x80 },    /* bit 9  */   { 1, 0x40 },    /* bit 10 */   { 1, 0x20 },    /* bit 11 */   { 1, 0x10 },    /* bit 12 */
   { 1, 0x08 },    /* bit 13 */   { 1, 0x04 },    /* bit 14 */   { 1, 0x02 },    /* bit 15 */   { 1, 0x01 },    /* bit 16 */
   { 2, 0x80 },    /* bit 17 */   { 2, 0x40 },    /* bit 18 */   { 2, 0x20 },    /* bit 19 */   { 2, 0x10 },    /* bit 20 */
   { 2, 0x08 },    /* bit 21 */   { 2, 0x04 },    /* bit 22 */   { 2, 0x02 },    /* bit 23 */   { 2, 0x01 },    /* bit 24 */
   { 3, 0x80 },    /* bit 25 */   { 3, 0x40 },    /* bit 26 */   { 3, 0x20 },    /* bit 27 */   { 3, 0x10 },    /* bit 28 */
   { 3, 0x08 },    /* bit 29 */   { 3, 0x04 },    /* bit 30 */   { 3, 0x02 },    /* bit 31 */   { 3, 0x01 },    /* bit 32 */
   { 4, 0x80 },    /* bit 33 */   { 4, 0x40 },    /* bit 34 */   { 4, 0x20 },    /* bit 35 */   { 4, 0x10 },    /* bit 36 */
   { 4, 0x08 },    /* bit 37 */   { 4, 0x04 },    /* bit 38 */   { 4, 0x02 },    /* bit 39 */   { 4, 0x01 },    /* bit 40 */
   { 5, 0x80 },    /* bit 41 */   { 5, 0x40 },    /* bit 42 */   { 5, 0x20 },    /* bit 43 */   { 5, 0x10 },    /* bit 44 */
   { 5, 0x08 },    /* bit 45 */   { 5, 0x04 },    /* bit 46 */   { 5, 0x02 },    /* bit 47 */   { 5, 0x01 },    /* bit 48 */
   { 6, 0x80 },    /* bit 49 */   { 6, 0x40 },    /* bit 50 */   { 6, 0x20 },    /* bit 51 */   { 6, 0x10 },    /* bit 52 */
   { 6, 0x08 },    /* bit 53 */   { 6, 0x04 },    /* bit 54 */   { 6, 0x02 },    /* bit 55 */   { 6, 0x01 },    /* bit 56 */
   { 7, 0x80 },    /* bit 57 */   { 7, 0x40 },    /* bit 58 */   { 7, 0x20 },    /* bit 59 */   { 7, 0x10 },    /* bit 60 */
   { 7, 0x08 },    /* bit 61 */   { 7, 0x04 },    /* bit 62 */   { 7, 0x02 },    /* bit 63 */   { 7, 0x01 },    /* bit 64 */

   { 8, 0x80 },    /* bit 1  */   { 8, 0x40 },    /* bit 2  */   { 8, 0x20 },    /* bit 3  */   { 8, 0x10 },    /* bit 4  */
   { 8, 0x08 },    /* bit 5  */   { 8, 0x04 },    /* bit 6  */   { 8, 0x02 },    /* bit 7  */   { 8, 0x01 },    /* bit 8  */
   { 9, 0x80 },    /* bit 9  */   { 9, 0x40 },    /* bit 10 */   { 9, 0x20 },    /* bit 11 */   { 9, 0x10 },    /* bit 12 */
   { 9, 0x08 },    /* bit 13 */   { 9, 0x04 },    /* bit 14 */   { 9, 0x02 },    /* bit 15 */   { 9, 0x01 },    /* bit 16 */
   { 10, 0x80 },   /* bit 17 */   { 10, 0x40 },   /* bit 18 */   { 10, 0x20 },   /* bit 19 */   { 10, 0x10 },   /* bit 20 */
   { 10, 0x08 },   /* bit 21 */   { 10, 0x04 },   /* bit 22 */   { 10, 0x02 },   /* bit 23 */   { 10, 0x01 },   /* bit 24 */
   { 11, 0x80 },   /* bit 25 */   { 11, 0x40 },   /* bit 26 */   { 11, 0x20 },   /* bit 27 */   { 11, 0x10 },   /* bit 28 */
   { 11, 0x08 },   /* bit 29 */   { 11, 0x04 },   /* bit 30 */   { 11, 0x02 },   /* bit 31 */   { 11, 0x01 },   /* bit 32 */
   { 12, 0x80 },   /* bit 33 */   { 12, 0x40 },   /* bit 34 */   { 12, 0x20 },   /* bit 35 */   { 12, 0x10 },   /* bit 36 */
   { 12, 0x08 },   /* bit 37 */   { 12, 0x04 },   /* bit 38 */   { 12, 0x02 },   /* bit 39 */   { 12, 0x01 },   /* bit 40 */
   { 13, 0x80 },   /* bit 41 */   { 13, 0x40 },   /* bit 42 */   { 13, 0x20 },   /* bit 43 */   { 13, 0x10 },   /* bit 44 */
   { 13, 0x08 },   /* bit 45 */   { 13, 0x04 },   /* bit 46 */   { 13, 0x02 },   /* bit 47 */   { 13, 0x01 },   /* bit 48 */
   { 14, 0x80 },   /* bit 49 */   { 14, 0x40 },   /* bit 50 */   { 14, 0x20 },   /* bit 51 */   { 14, 0x10 },   /* bit 52 */
   { 14, 0x08 },   /* bit 53 */   { 14, 0x04 },   /* bit 54 */   { 14, 0x02 },   /* bit 55 */   { 14, 0x01 },   /* bit 56 */
   { 15, 0x80 },   /* bit 57 */   { 15, 0x40 },   /* bit 58 */   { 15, 0x20 },   /* bit 59 */   { 15, 0x10 },   /* bit 60 */
   { 15, 0x08 },   /* bit 61 */   { 15, 0x04 },   /* bit 62 */   { 15, 0x02 },   /* bit 63 */   { 15, 0x01 }   /* bit 64 */
};


/******************************************************************************
 *
 *  NAME:         ASCII_EBCDIC_CONV
 *
 *  DESCRIPTION:  This function converts an ASCII character into and an EBCDIC
 *                character, or visa versa.
 *
 *                    ascii_ebcdic_conv - IBM EBCDIC
 *                    ------------------------------
 *                    Convert ASCII to EBCDIC -or- EBCDIC to ASCII.
 *                    Use input parameter 2, either "A2E" or "E2A".
 *
 *  INPUTS:       conv_char - Character to be converted
 *                conv_type - A2E or E2A: indicates which way to convert
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   Converted character
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 *     DI - 03/15/99 - Added '^' to the conversion chart.
 *     DI - 05/19/99 - Changed '^' from 0xB0 to 0x9A as the EBCDIC value.
 ******************************************************************************/
BYTE ascii_ebcdic_conv( unsigned char conv_char, char *conv_type )
{

#define  CHART_SIZE  90

   INT   i;
   INT   found = 0;
   BYTE  ReturnChar;

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
        |   ?     :     #     @     '     =     "     ^              |
        +------------------------------------------------------------+
      */

   static unsigned char ebcdic_chart[CHART_SIZE] = 
      {
         0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9,
         0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x91,
         0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0xA2, 0xA3,
         0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xC1, 0xC2, 0xC3, 0xC4,
         0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5,
         0xD6, 0xD7, 0xD8, 0xD9, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7,
         0xE8, 0xE9, 0x40, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x5A,
         0x5B, 0x5C, 0x5D, 0x5E, 0x60, 0x61, 0x6B, 0x6C, 0x6D, 0x6E,
         0x6F, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F, 0x5F, 0x9A, 0xA1
      };

   static unsigned char ascii_chart[CHART_SIZE] = 
      {
         0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
         0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A,
         0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73, 0x74,
         0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x41, 0x42, 0x43, 0x44,
         0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E,
         0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
         0x59, 0x5A, 0x20, 0x2E, 0x3C, 0x28, 0x2B, 0x7C, 0x26, 0x21,
         0x24, 0x2A, 0x29, 0x3B, 0x2D, 0x2F, 0x2C, 0x25, 0x5F, 0x3E,
         0x3F, 0x3A, 0x23, 0x40, 0x27, 0x3D, 0x22, 0x5E, 0x5E, 0x7E
      };


   if ( 0 == strcmp( conv_type, A2E ) )
   {
      /* CONVERT ASCII TO EBCDIC */
      for( i=0; i<CHART_SIZE  &&  !found; i++ )
      {
         if ( conv_char == ascii_chart[i] )
         {
            ReturnChar = ebcdic_chart[i];
            found = 1;
         }
      }
   }
   else if ( 0 == strcmp( conv_type, E2A ) )
   {
      /* CONVERT EBCDIC TO ASCII */
      for( i=0; i<CHART_SIZE  &&  !found; i++ )
      {
         if ( conv_char == ebcdic_chart[i] )
         {
            ReturnChar = ascii_chart[i];
            found = 1;
         }
      }
   }

   if ( 0 == found )
      ReturnChar = 0x00;

   return( ReturnChar );
}


/******************************************************************************
 *
 *  NAME:         vEBCDICMove
 *
 *  DESCRIPTION:  This function converts a variable length string into EBCDIC,
 *                then copies it into ReqRsp_Buffer.  The length is also copied
 *                into the buffer in EBCDIC format.
 *
 *  INPUTS:       fieldid - Field Id to denote where the source data exists
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void vEBCDICMove( enum bitfields fieldid )
{
   struct bitfield_data *pBF;
   int                  movelen;
   int                  i;
   unsigned char        cFieldData;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);
   if (pBF != NULL)
   {
      movelen  = get_variable_length( pBF->p_bd_pdata, pBF->bd_fieldlen );
      movelen += pBF->bd_fieldlen;

      for( i=0; i<movelen; i++ )
      {
         cFieldData = ascii_ebcdic_conv( (unsigned char)(pBF->p_bd_pdata[i]), A2E );
         memset(&ReqRsp_Buffer[reqidx], cFieldData, 1);
         reqidx++;
      }
   }

} /* end vEBCDICMove */



/******************************************************************************
 *
 *  NAME:         EBCDICMove
 *
 *  DESCRIPTION:  This function converts a fixed length string into EBCDIC,
 *                then copies it into ReqRsp_Buffer.
 *
 *  INPUTS:       fieldid - Field Id to denote where the source data exists
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void EBCDICMove( enum bitfields fieldid )
{
   struct bitfield_data *pBF;
   int                  movelen;
   int                  i;
   unsigned char        cFieldData;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);
   if (pBF != NULL)
   {
      movelen = pBF->bd_fieldlen;

      for( i=0; i<movelen; i++ )
      {
         cFieldData = ascii_ebcdic_conv( (unsigned char)(pBF->p_bd_pdata[i]), A2E );
         memset(&ReqRsp_Buffer[reqidx], cFieldData, 1);
         reqidx++;
      }
   }

} /* end EBCDICMove */



/******************************************************************************
 *
 *  NAME:         RspEBCDICvMove
 *
 *  DESCRIPTION:  This function converts a variable length string from EBCDIC,
 *                into ASCII.  It copies it from ReqRsp_Buffer into where ever
 *                the parameter 'fieldid' is pointing, usually into specific
 *                fields in the HOST_MESSAGE data structure.  The length is
 *                also copied.
 *
 *  INPUTS:       fieldid - Field Id to denote the destination of the data
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void RspEBCDICvMove (enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   int                  i;
   int                  FieldLen;
   double               movelen;
   unsigned char        cLen;
   unsigned char        cFieldData;

   /*---------------------------------------------------*/
   /* Find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);
   if (pBF != NULL)
   {
      FieldLen = pBF->bd_fieldlen;

      /* Determine number of bytes to move. */
      for( i=0,movelen=0; i<FieldLen; i++ )
      {
         cLen = ascii_ebcdic_conv( ReqRsp_Buffer[reqidx+i], E2A );
         movelen += (int)(cLen - 0x30) * (pow(10,(double)(FieldLen-i-1)));
      }
      movelen += pBF->bd_fieldlen;

      if ( (reqidx + movelen) <= buflen )
      {
         /* Move the data, converting it from EBCDIC to ASCII. */
         for( i=0; i<movelen; i++,reqidx++ )
         {
            cFieldData = ascii_ebcdic_conv( (unsigned char)(ReqRsp_Buffer[reqidx]), E2A );
            memset(&pBF->p_bd_pdata[i], cFieldData, 1);
         }
      }
      else
         invalid_msg_format = true;
   }

} /* end RspEBCDICvMove */

/******************************************************************************
 *
 *  NAME:         RspEBCDICvMove_NoLen
 *
 *  DESCRIPTION:  This function does the same thing as function RspEBCDICvMove.
 *                However, it does it without moving the length field.
 *
 *  INPUTS:       fieldid - Field Id to denote the destination of the data
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 *****************************************************************************/
void RspEBCDICvMove_NoLen (enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   int                  i;
   int                  FieldLen;
   double               movelen;
   unsigned char        cLen;
   unsigned char        cFieldData;

   /*---------------------------------------------------*/
   /* Find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);
   if (pBF != NULL)
   {
      FieldLen = pBF->bd_fieldlen;

      /* Determine number of bytes to move. */
      for( i=0,movelen=0; i<FieldLen; i++ )
      {
         cLen = ascii_ebcdic_conv( ReqRsp_Buffer[reqidx+i], E2A );
         movelen += (int)(cLen - 0x30) * (pow(10,(double)(FieldLen-i-1)));
      }
      reqidx += FieldLen;  /* Do not move the field length data. */

      if ( (reqidx + movelen) <= buflen )
      {
         /* Move the data, converting it from EBCDIC to ASCII. */
         for( i=0; i<movelen; i++,reqidx++ )
         {
            cFieldData = ascii_ebcdic_conv( (unsigned char)(ReqRsp_Buffer[reqidx]), E2A );
            memset(&pBF->p_bd_pdata[i], cFieldData, 1);
         }
      }
      else
         invalid_msg_format = true;
   }
} /* end RspEBCDICvMove_NoLen */


/******************************************************************************
 *
 *  NAME:         RspEBCDICMove
 *
 *  DESCRIPTION:  This function converts a fixed length string from EBCDIC,
 *                into ASCII.  It copies it from ReqRsp_Buffer into where ever
 *                the parameter 'fieldid' is pointing, usually into specific
 *                fields in the HOST_MESSAGE data structure.
 *
 *  INPUTS:       fieldid - Field Id to denote the destination of the data
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
void RspEBCDICMove (enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   int                  i;
   int                  movelen;
   unsigned char        cFieldData;

   /*---------------------------------------------------*/
   /* Find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);
   if (pBF != NULL)
   {
      movelen = pBF->bd_fieldlen;

      if ( (reqidx + movelen) <= buflen )
      {
         /* Move the data, converting it from EBCDIC to ASCII. */
         for( i=0; i<movelen; i++,reqidx++ )
         {
            cFieldData = ascii_ebcdic_conv( (unsigned char)(ReqRsp_Buffer[reqidx]), E2A );
            memset(&pBF->p_bd_pdata[i], cFieldData, 1);
         }
      }
      else
         invalid_msg_format = true;
   }
} /* end RspEBCDICMove */


/***********************************************
   GenericMove
************************************************/
void GenericMove(enum bitfields fieldid)
{
   struct bitfield_data *pBF;


   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
        /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);
   if (pBF != NULL)
   {
      memcpy(&ReqRsp_Buffer[reqidx],      /* move to request buffer */
             pBF->p_bd_pdata,         /* from data pointer      */
             pBF->bd_fieldlen);      /* for length of field    */

      reqidx = reqidx + pBF->bd_fieldlen;   /* increment buffer pointer */
   }
} /* end GenericMove */



/***********************************************
   RspMoveGeneric
************************************************/
void RspMoveGeneric(enum bitfields fieldid)
{
   struct bitfield_data *pBF;


   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
        /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);
   if (pBF != NULL)
   {
      if ( (reqidx + pBF->bd_fieldlen) <= buflen )
      {
         memcpy(pBF->p_bd_pdata,         /* to data pointer      */
                &ReqRsp_Buffer[reqidx],      /* move from request buffer */
                pBF->bd_fieldlen);      /* for length of field    */

         reqidx = reqidx + pBF->bd_fieldlen;   /* increment buffer pointer */
      }
      else
         invalid_msg_format = true;
   }
} /* end RspMoveGeneric */


/******************************************************************************
 *
 *  NAME:         vDataMove
 *
 *  DESCRIPTION:  This function moves a variable length string into
 *                ReqRsp_Buffer.  The length is also copied, but converted
 *                into EBCDIC format.  The data is not converted.
 *
 *  INPUTS:       fieldid - Field Id to denote where the source data exists
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void vDataMove( enum bitfields fieldid )
{
   struct bitfield_data *pBF;
   int                  movelen;
   int                  i;
   unsigned char        cFieldData;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);
   if (pBF != NULL)
   {
      movelen  = get_variable_length( pBF->p_bd_pdata, pBF->bd_fieldlen );

      /* Convert length to EBCDIC and move to ReqRsp_Buffer */
      for( i=0; i<pBF->bd_fieldlen; i++ )
      {
         cFieldData =ascii_ebcdic_conv((unsigned char)(pBF->p_bd_pdata[i]),A2E);
         memset(&ReqRsp_Buffer[reqidx], cFieldData, 1);
         reqidx++;
      }

      /* Move the rest of the data; do not convert it. */
      memcpy(&ReqRsp_Buffer[reqidx], pBF->p_bd_pdata+pBF->bd_fieldlen, movelen);
      reqidx += movelen;
   }

} /* end vDataMove */


/******************************************************************************
 *
 *  NAME:         RspMovevData
 *
 *  DESCRIPTION:  This function moves a variable length string from
 *                ReqRsp_Buffer into HOST_MESSAGE.  The length field
 *                is in EBCDIC and must be converted. But the data is
 *                in various formats and is moved without any conversion.
 *
 *  INPUTS:       fieldid - Field Id to denote the destination of the data
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void RspMovevData (enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  i;
   INT                  FieldLen;
   double               movelen;
   unsigned char        cLen;
   unsigned char        cFieldData;

   /*---------------------------------------------------*/
   /* Find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);
   if (pBF != NULL)
   {
      FieldLen = pBF->bd_fieldlen;

      /* Determine number of bytes to move. */
      for( i=0,movelen=0; i<FieldLen; i++ )
      {
         cLen = ascii_ebcdic_conv( ReqRsp_Buffer[reqidx+i], E2A );
         movelen += (int)(cLen - 0x30) * (pow(10,(double)(FieldLen-i-1)));
      }

      /* Convert length from EBCDIC and move to HOST_MESSAGE. */
      for( i=0; i<FieldLen; i++,reqidx++ )
      {
         cFieldData = ascii_ebcdic_conv( (unsigned char)(ReqRsp_Buffer[reqidx]), E2A );
         memset(&pBF->p_bd_pdata[i], cFieldData, 1);
      }

      if ( (reqidx + movelen) <= buflen )
      {
         /* Move the rest of the data; do not convert it. */
         memcpy( pBF->p_bd_pdata+FieldLen, ReqRsp_Buffer+reqidx, (INT)movelen );
         reqidx += (INT)movelen;
      }
      else
         invalid_msg_format = true;
   }

} /* end RspEBCDICvMove */



/******************************************************************************
 *
 *  NAME:         get_variable_length
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
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
int get_variable_length( unsigned char *DataField, int FieldLen )
{
   int    i;
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


/******************************************************************************
 *
 *  NAME:         RspIgnore - Do not copy response field
 *
 *  DESCRIPTION:  This function is called when a host response is being parsed
 *                and we do not care to copy the contents of the field into
 *                the HOST_MESSAGE structure from the ReqRsp_Buffer.  The only
 *                thing that happens here is the ReqRsp_Buffer is incremented
 *                to point past this data.
 *
 *  INPUTS:       fieldid - Field Id to denote the destination of the data
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void RspIgnore(enum bitfields fieldid)
{
   struct bitfield_data *pBF;


   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);
   if (pBF != NULL)
   {
      if ( (reqidx + pBF->bd_fieldlen) <= buflen )
         reqidx = reqidx + pBF->bd_fieldlen;   /* increment buffer pointer */
      else
         invalid_msg_format = true;
   }
} /* end RspIgnore */


/******************************************************************************
 *
 *  NAME:         vRspIgnore - Do not copy variable length response field
 *
 *  DESCRIPTION:  This function is called when a host response is being parsed
 *                and we do not care to copy the contents of the field into
 *                the HOST_MESSAGE structure from the ReqRsp_Buffer.  The only
 *                thing that happens here is the ReqRsp_Buffer is incremented
 *                to point past this data.
 *
 *  INPUTS:       fieldid - Field Id to denote the destination of the data
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void vRspIgnore(enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   int                  i;
   int                  FieldLen;
   double               movelen;
   unsigned char        cLen;

   /*---------------------------------------------------*/
   /* Find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);
   if (pBF != NULL)
   {
      FieldLen = pBF->bd_fieldlen;

      if ( (reqidx + FieldLen) <= buflen )
      {
         /* Determine number of bytes to move. */
         for( i=0,movelen=0; i<FieldLen; i++ )
         {
            cLen = ascii_ebcdic_conv( ReqRsp_Buffer[reqidx+i], E2A );
            movelen += (int)(cLen - 0x30) * (pow(10,(double)(FieldLen-i-1)));
         }
         movelen += pBF->bd_fieldlen;

         if ( (reqidx + movelen) <= buflen )
            reqidx = reqidx + (int)movelen;   /* Increment buffer pointer */
         else
            invalid_msg_format = true;
      }
      else
      {
         invalid_msg_format = true;
      }
   }
} /* end vRspIgnore */


/******************************************************************************
 *
 *  NAME:         CondPan
 *
 *  DESCRIPTION:  This function copies the PAN field from HOST_MESSAGE into
 *                ReqRsp_Buffer if the PAN field has data.
 *
 *  INPUTS:       fieldid - Field Id to denote the source field of the data
 *                p_trantype - transaction type
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if data was moved, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT CondPan( enum bitfields bitfield, BYTE p_trantype )
{
   if ( 0x00 != HOST_MESSAGE.pan_len [0] )
       return( Move_IfThere(bitfield, p_trantype) );

   return( false );
} /* end CondPan */


/******************************************************************************
 *
 *  NAME:         CondExpd
 *
 *  DESCRIPTION:  This function copies the date expiration field from
 *                HOST_MESSAGE into ReqRsp_Buffer if the date expiration field
 *                has data.
 *
 *  INPUTS:       fieldid - Field Id to denote the source field of the data
 *                p_trantype - transaction type
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if data was moved, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT CondExpd( enum bitfields bitfield, BYTE p_trantype )
{
   /* if track2 data here, no need for expdate field */
   if ( 0x00 != HOST_MESSAGE.date_expd [1] )
      return( true );

   return( false );
} /* end CondExpd */


/******************************************************************************
 *
 *  NAME:         CondProc
 *
 *  DESCRIPTION:  This function copies the processing code field from
 *                HOST_MESSAGE into ReqRsp_Buffer if the processing code field
 *                has data.
 *
 *  INPUTS:       fieldid - Field Id to denote the source field of the data
 *                p_trantype - transaction type
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if data was moved, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT CondProc(enum bitfields bitfield, BYTE p_trantype)
{
   if ( 0x04 != HOST_MESSAGE.msgtype [0] )
      return ( true );

   return( false);
} /* end CondProc */



/******************************************************************************
 *
 *  NAME:         CondTrack1
 *
 *  DESCRIPTION:  This function copies the track 1 field from
 *                HOST_MESSAGE into ReqRsp_Buffer if the track 1 field
 *                has data.
 *
 *  INPUTS:       fieldid - Field Id to denote the source field of the data
 *                p_trantype - transaction type
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if data was moved, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT CondTrack1( enum bitfields bitfield, BYTE p_trantype )
{
   if ( 0x00 != HOST_MESSAGE.track1_len [0] )
       return( true );

   return( false );
} /* end CondTrack1 */


/******************************************************************************
 *
 *  NAME:         Move_IfThere
 *
 *  DESCRIPTION:  This function returns a value based on whether or not there
 *                is data in the specified HOST_MESSAGE field.  If there is
 *                data in it, TRUE is returned, else FALSE is returned.
 *
 *  INPUTS:       fieldid - Field Id to denote the source field of the data
 *                p_trantype - transaction type
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if data exists to be moved, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT Move_IfThere( enum bitfields fieldid, BYTE p_trantype )
{
   struct bitfield_data *pBF;
   INT i;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table( fieldid );
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
} /* end Move_IfThere */


/******************************************************************************
 *
 *  NAME:         Move_Always
 *
 *  DESCRIPTION:  This function returns true.  It is used to indicate that data
 *                SHOULD be moved from HOST_MESSAGE into ReqRsp_Buffer.
 *
 *  INPUTS:       fieldid - Field Id to denote the source field of the data
 *                p_trantype - transaction type
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT Move_Always( enum bitfields bitfield, BYTE p_trantype )
{
   return( true );
} /* end Move_Always */


/******************************************************************************
 *
 *  NAME:         Move_Never
 *
 *  DESCRIPTION:  This function returns false.  It is used to indicate that data
 *                SHOULD NOT be moved from HOST_MESSAGE into ReqRsp_Buffer.
 *
 *  INPUTS:       fieldid - Field Id to denote the source field of the data
 *                p_trantype - transaction type
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT Move_Never( enum bitfields bitfield, BYTE p_trantype )
{
   return( false );
} /* end Move_Never */

BOOLEAN build_DE48_from_req_for_STIP_txn_3DS2(INT *src_len, char *src, pAUTH_TX p_auth_tx)
{
	#define  FIELD48_MAX_LEN  256
	char aav_data[256]={0};
	INT  sub_field_len = 0;
	INT  MainIndex = 0;
	INT  TotalLen = 0;
	CHAR tag[3] = {0};
	CHAR len[3] = {0};
	char De48SE22SFlen[3]={0};
	char DE48SE22data[50]={0};
	char DE48SE22_builtdata[50]={0};
	int Se22index=0;
	char SE22_tag[3]={0};
	int actual_lenSe22=0;
	char stratctual_len[3]={0};


	*src_len = 0;
	TotalLen = strlen(p_auth_tx->EMV_details.issuer_script_2);
	if(TotalLen != 0)
	{
		memcpy(aav_data,p_auth_tx->EMV_details.issuer_script_2,TotalLen);
	}
	else if(p_auth_tx->future_use_3[0] != '\0')
	{/*DE48 length expected to be more than 128 bytes so future use 3,4 and 5 will be combinely used to store the DE48*/
		TotalLen = p_auth_tx->future_use_3[0];
		memcpy(aav_data,p_auth_tx->future_use_3+1,TotalLen);
	}


	/* Copy the TCC value recived in the rquest*/
	src[0] = aav_data[0];
	*src_len += 1;
	for( MainIndex=1; MainIndex<TotalLen && MainIndex<FIELD48_MAX_LEN; )
	{
	      memset( tag, 0x00, sizeof( tag ) );
	      strncpy( tag, &aav_data[MainIndex], 2 );
	      MainIndex += 2;

		 memset( len, 0x00, sizeof(len) );
		 memcpy( len, &aav_data[MainIndex], 2 );
		 sub_field_len = atoi( len );
		 MainIndex += 2;
		 if( 0 == strncmp(tag, DE48_TAG56, 2) ||
			 0 == strncmp(tag, DE48_TAG66, 2) ||
			 0 == strncmp(tag, DE48_TAG50, 2) ||
			 0 == strncmp(tag, DE48_TAG37, 2) )
		 {
			 /*dont copy SE56 and SE66 from the req for response sent for STIP txn
			  * */

		 }
		 else if(0 == strncmp(tag, DE48_TAG87, 2) &&
				 p_auth_tx->TLF01_details.cvc != '\0')
		 {
			 /* Dont copy request DE48 tag 87, ncmcrd/ncmcr3 already filled it.
			  * */
		 }
		 else if(0 == strncmp(tag, DE48_TAG22, 2))
		 {
			 /*check for SF06 in tag 22, ATP should not echo back DE48 SE22 SF06 JIRA:BDOPS2024-14*/
			 strncat(src,tag,2);
			 strncpy(DE48SE22data,&aav_data[MainIndex],sub_field_len);
			 Se22index += 2;

			 memset( De48SE22SFlen, 0x00, sizeof(De48SE22SFlen) );
			 memcpy( De48SE22SFlen, &DE48SE22data[Se22index], 2 );
			 Se22index += 2;

			 for(Se22index=0;Se22index<sub_field_len;)
			 {
				 memset( SE22_tag, 0x00, sizeof( SE22_tag ) );
				 strncpy( SE22_tag, &DE48SE22data[Se22index], 2 );
				 Se22index += 2;

			     if( 0 == strncmp(SE22_tag,DE48_TAG22SF01,2) || 0 == strncmp(SE22_tag,DE48_TAG22SF02,2) ||
			         0 == strncmp(SE22_tag,DE48_TAG22SF03,2) || 0 == strncmp(SE22_tag,DE48_TAG22SF04,2) ||
					 0 == strncmp(SE22_tag,DE48_TAG22SF05,2))
			     {
					 memset( De48SE22SFlen, 0x00, sizeof(De48SE22SFlen) );
				     memcpy( De48SE22SFlen, &DE48SE22data[Se22index], 2 );
				     Se22index += 2;

			    	 strncat(DE48SE22_builtdata,SE22_tag,2);
			    	 strncat(DE48SE22_builtdata,De48SE22SFlen,2);
			    	 actual_lenSe22 += 4;

			    	 strncat( DE48SE22_builtdata, &DE48SE22data[Se22index], atoi(De48SE22SFlen) );

			    	 actual_lenSe22 +=  atoi(De48SE22SFlen);
			    	 Se22index += atoi(De48SE22SFlen);
			     }
			     else if(0 == strncmp(SE22_tag,DE48_TAG22SF06,2))
				 {
			    	 /* dont echo back*/
			    	 memset( De48SE22SFlen, 0x00, sizeof(De48SE22SFlen) );
			    	 memcpy( De48SE22SFlen, &DE48SE22data[Se22index], 2 );
			    	 Se22index += 2;

			    	 Se22index += atoi(De48SE22SFlen);

				 }
			 }
			 sprintf(stratctual_len,"%02d",actual_lenSe22);
			 strncat(src,stratctual_len,2);
			 strncat(src,DE48SE22_builtdata,actual_lenSe22);

			 *src_len += 4;
			 *src_len += actual_lenSe22;
		 }
		 else
		 {
			 strncat(src,tag,2);
			 strncat(src,len,2);
			 strncat(src,&aav_data[MainIndex],sub_field_len);
			 *src_len += 4;
			 *src_len += sub_field_len;
		 }
		 MainIndex += sub_field_len;
	}

	return true;

}

BOOLEAN build_DE48_from_req_for_STIP_txn(INT *src_len, char *src, pAUTH_TX p_auth_tx)
{
	#define  FIELD48_MAX_LEN  256
	char aav_data[256]={0};
	INT  sub_field_len = 0;
	INT  MainIndex = 0;
	INT  TotalLen = 0;
	CHAR tag[3] = {0};
	CHAR len[3] = {0};

	*src_len = 0;
	TotalLen = strlen(p_auth_tx->EMV_details.issuer_script_2);
	if(TotalLen != 0)
	{
		memcpy(aav_data,p_auth_tx->EMV_details.issuer_script_2,TotalLen);
	}
	else if(p_auth_tx->future_use_3[0] != '\0')
	{
		TotalLen = p_auth_tx->future_use_3[0];
		memcpy(aav_data,p_auth_tx->future_use_3+1,TotalLen);
	}


	/* Copy the TCC value recived in the rquest*/
	src[0] = aav_data[0];
	*src_len += 1;
	for( MainIndex=1; MainIndex<TotalLen && MainIndex<FIELD48_MAX_LEN; )
	{
	      memset( tag, 0x00, sizeof( tag ) );
	      strncpy( tag, &aav_data[MainIndex], 2 );
	      MainIndex += 2;

		 memset( len, 0x00, sizeof(len) );
		 memcpy( len, &aav_data[MainIndex], 2 );
		 sub_field_len = atoi( len );
		 MainIndex += 2;
		 if( 0 == strncmp(tag, DE48_TAG87, 2) ||
			0 == strncmp(tag, DE48_TAG50, 2) ||
			0 == strncmp(tag, DE48_TAG37, 2) ||
			0 == strncmp(tag, DE48_TAG22, 2) )
		 {
			 /*dont copy SE56 and SE66 from the req for response sent for STIP txn
			  * */

		 }
		 else
		 {
			 strncat(src,tag,2);
			 strncat(src,len,2);
			 strncat(src,&aav_data[MainIndex],sub_field_len);
			 *src_len += 4;
			 *src_len += sub_field_len;
		 }
		 MainIndex += sub_field_len;
	}

	return true;

}



/******************************************************************************
 *
 *  NAME:         PARSE_FIELD_48
 *
 *  DESCRIPTION:  This procedure parses field 48 of a 0110 response message
 *                looking for various subfields known as tags.
 *
 *                Tag 42 = Used for e-commerce security level settings
 *                Tag 43 = Security data used in conjunction with tag 42
 *                Tag 84 = 2 byte merchant advice code for RPS merchants
 *                Tag 87 = 1 byte flag: Magnetic Stripe / CVC Invalid
 *                Tag 88 = Magnetic stripe compliance status indicator
 *                         1 byte flag: 'Y' = DE22 is 02, not 90,
 *                         If present, must be echoed in response.
 *                Tag 89 = Magnetic stripe compliance error indicator
 *                         1 byte flag indicating reason for error,
 *                         If present, must be echoed in response.
 *                Tag 92 = 3 byte CVC2 number
 *
 *  FIELD 48 FORMAT:
 *
 *     -------+------+---------+---------+---------+---------+-----
 *      Total |Trans | First   | First   | First   | Second  |
 *       Len  | Cat. |SubField |SubField |SubField |SubField | . . .
 *            | Code |  Tag    | Length  |  Data   |  Tag    |
 *     -------+------+---------+---------+---------+---------+-----
 * BYTES: 3       1       2         2         x         2
 *
 *
 *  INPUTS:       src_len - Length of 'src'.
 *                src     - String containing field 48 data
 *
 *  OUTPUTS:      auth_tx - Transaction data structure
 *
 *  RTRN VALUE:   0 = No problems
 *                n = Received a subfield that requires us to decline
 *                    the transaction.  n = subfield number.
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT parse_field_48( char *src_len, char *src, pAUTH_TX p_auth_tx )
{
   #define  FIELD48_MAX_LEN  256

   INT  ret_val = 0;
   INT  sub_field_len = 0;
   INT  MainIndex = 0;
   INT  TotalLen = 0;
   INT SubFieldIDLen=0;
   CHAR tag[3] = {0};
   CHAR len[3] = {0};
   CHAR DE42_SubFld1[4]={0};
   CHAR DE42_SubFld2[4]={0};
   CHAR DE42_SubFld3[4]={0};
   CHAR DE66_len[3]={0};
   CHAR SE22_SubFld1[3]={0};
   CHAR SE22_SubFld2[3]={0};
   CHAR SE22_SubFld5[3]={0};
   CHAR SE22_SubFldLen[3]={0};
   char SE37_SubFld4[3]={0};
   char SE37_SubFld5[13]={0};
   char SE37_SubFld[3]={0};
   char Tag57_SubFld1[3]={0};
   char SE67_SubFld2[3]={0};
   CHAR SE75_SubFld1[3]={0};
   CHAR SE75_SubFld2[3]={0};
   CHAR SE75_SubFld3[3]={0};
   CHAR SE75_SubFld4[3]={0};

   INT	tag_len = 0;
   INT total_index = 0;
   INT dummy_mainindex=0;
   char buffer[514]={0};

   TotalLen = atoi( src_len );

   /* MainIndex starts at 1 to skip over the Transaction Category Code. */
   for( MainIndex=1; MainIndex<TotalLen && MainIndex<FIELD48_MAX_LEN; )
   {
      /* Get subfield tag (name of subfield). */
      memset( tag, 0x00, sizeof( tag ) );
      strncpy( tag, &src[MainIndex], 2 );
      MainIndex += 2;

 //     /* Get length of subfield. */
 //     if ( 0 == strcmp(tag, DE48_TAG43) )
 //     {
 //        /* Subfield 43 is fixed length. */
 //        sub_field_len = DE48_TAG43_LEN;

 //        /* Currently, do not need the data from this subfield. */
 //     }
//      else
 //     {
         /* Other subfields are variable length. */
         memset( len, 0x00, sizeof(len) );
         memcpy( len, &src[MainIndex], 2 );
         sub_field_len = atoi( len );
         MainIndex += 2;

         /* Get the data from these subfields. */
		if ( 0 == strncmp(tag, DE48_TAG43, 2) )
		 {
			 strcpy(p_auth_tx->TLF01_details.nfi_end_date,"1"); // 3D secure flag for MC
			 strcpy( p_auth_tx->industry_code, "700" );
			 if(src[MainIndex] == 'j' || src[MainIndex] == 'h' || src[MainIndex] == 'l' ||
				src[MainIndex] == 'm' || src[MainIndex] == 'n' || src[MainIndex] == 'i')
			 {
				p_auth_tx->TLF01_details.product_codes[5].quantity[0]= src[MainIndex];
				p_auth_tx->TLF01_details.product_codes[5].quantity[1] = '\0';
			 }
			 else
			 {
			 strncpy( p_auth_tx->TLF01_details.product_codes[5].quantity,&src[MainIndex],2);
				 p_auth_tx->TLF01_details.product_codes[5].quantity[2] = '\0';
			 }
			 if(sub_field_len == 28)
			 {/* if tag 43 length is 28, MC has verified the UCAF, so no
			 need to verify in ATP STIP, enable a flag to use it in txcntl  */
				 p_auth_tx->TLF01_details.product_codes[4].quantity[1] = '1' ;
			 }
		 }

        else if (0 == strncmp(tag, DE48_TAG41, 2) &&
        		(0==strncmp(p_auth_tx->TLF01_details.pos_entry_mode,"010",3) ||
        		HOST_MESSAGE.pos_data[3] == '4'))
        {
       	 memcpy(p_auth_tx->TLF01_details.dbsales_amount,&src[MainIndex+4],12);
       	 memcpy(p_auth_tx->TLF01_details.refund_amount,&src[MainIndex+16],12);
       	 memcpy(p_auth_tx->TLF01_details.period,&src[MainIndex+28],2);
        }
         else if ( 0 == strncmp(tag, DE48_TAG87, 2) )
         {
            p_auth_tx->TLF01_details.cvc = src[MainIndex];
         }

         else if ( 0 == strncmp(tag, DE48_TAG92, 2) )
         {
            memcpy( p_auth_tx->TLF01_details.cvc_data,&src[MainIndex],3) ;
         }
         else if( 0 == strncmp(tag, DE48_TAG84, 2) )
         {
        	strcpy(p_auth_tx->TLF01_details.response_text,"DE48 SE84:");
            memcpy( p_auth_tx->TLF01_details.response_text+10,&src[MainIndex],2 );
         }
         else if ( 0 == strncmp(tag, DE48_TAG57, 2) )
         {
        	memcpy(Tag57_SubFld1,&src[MainIndex],2 );
        	if(0 == strncmp(Tag57_SubFld1,"01",2))
        	{
        		memcpy(p_auth_tx->TLF01_details.visa_validation_code,&src[MainIndex+4],3);
        	}
         }
         else if( 0 == strncmp(tag, DE48_TAG42, 2) )

         {
            /* Set the industry code for e-commerce. */
            strcpy( p_auth_tx->industry_code, "700" );
			
			// Store 4 an 2 in code[0] and code[1] so that it can be used in ncmcrd2 for verification
			//strncpy(p_auth_tx->TLF01_details.product_codes[14].code,DE48_TAG42,2);

			// Point toSubfeild Pos3 and  store Subelement 42 Subfeild 1 Pos3 
			//strncpy(p_auth_tx->TLF01_details.product_codes[14].code+2,&src[MainIndex +6],1);

            /*Store DE48 Sub field 42
             *
             * Here is the format for DE42
             * 42 - TAG 42
				07  - L+SUB filed len
				010321x - SUb field 1 - <Sub filed 01 ><Length 03 ><value 21x>
				0203212  - SUb field 2 - <Sub filed 02><Length 03><value 212>
				03011	- SUb field 3 - <Sub filed 03 ><Length 01><value 1>
             *
             * */
            memcpy(DE42_SubFld1,&src[MainIndex],2 );

            if(sub_field_len <= 7)
            {
            	// We have only DE42 Sub field 1
            	memcpy( p_auth_tx->TLF01_details.profile,&src[MainIndex+5],2 );
            }
            else  if(sub_field_len > 7)
            {
            	 //DE42 Sub field 2
            	memcpy( p_auth_tx->TLF01_details.product_codes[14].code,&src[MainIndex+4],3 );
            	 //DE42 Sub field 3
            	memcpy( p_auth_tx->TLF01_details.product_codes[14].quantity,&src[MainIndex+18],1 );
            }
         }
         else if(0 == strncmp(tag, DE48_TAG71_MSBS, 2))
         {
        	 // In ncmcr2 module, we will prepare DE48 SF71 and send data defined in DE48_TAG71_MSBS
        	 if( 4 == sub_field_len )
        	 { /* both SF1 & SF2 present */
        		 memcpy(p_auth_tx->TLF01_details.product_codes[19].quantity,&src[MainIndex],2);
        		 memcpy(p_auth_tx->TLF01_details.product_codes[16].quantity,&src[MainIndex+2],1);
        	 }
         }
         else if (0 == strncmp(tag, DE48_TAG21_SF1, 2))
         {
        	 memcpy(p_auth_tx->TLF01_details.product_codes[15].quantity,&src[MainIndex+4],1);
         }

         else if(0 == strncmp(tag, DE48_TAG63, 4))
         {
        	 strncpy( p_auth_tx->TLF01_details.mcard_banknet,&src[MainIndex],9);
			 strncat( p_auth_tx->TLF01_details.settlement_date,&src[MainIndex],4);
         }
         else if (0 == strncmp(tag, DE48_TAG66, DE48_TAG66_LEN))
         {
        	 //Need to store SF1 and SF2.
     		/* TAG 66 has 2 SF, SF1 has 1 byte length.
     		 * SF2 has 36 byte length.
     		 * SF2 stored in two fields TLF01_details.workstation and TLF01_details.operator_id */
        	 if(sub_field_len == 45)
        	 {/* BOTH SF1 and SF2 present */
    			 memset(p_auth_tx->TLF01_details.workstation,0x00,sizeof(p_auth_tx->TLF01_details.workstation));
    			 memset(p_auth_tx->TLF01_details.operator_id,0x00,sizeof(p_auth_tx->TLF01_details.operator_id));

        		 memcpy(p_auth_tx->TLF01_details.product_codes[11].quantity,&src[MainIndex+4],1);
				 memcpy(p_auth_tx->TLF01_details.workstation,&src[MainIndex+9],20);
				 memcpy(p_auth_tx->TLF01_details.operator_id,&src[MainIndex+29],16);
        	 }
         }
         else if ( 0 == strncmp(tag, DE48_TAG65, 2) )
		 {
			// Storing TLE Compliant Certified value in quantity[0] and  UKPT/DUKPT Compliant Certified values in quantity[1]
			strncpy(p_auth_tx->TLF01_details.product_codes[0].quantity,&src[MainIndex],2);
		 }
		 else if ( 0 == strncmp(tag, DE48_TAG67, 2) )
		 {
			 if( 7 == sub_field_len)
			 {
				 /* only SF01 is present */
			strncpy(p_auth_tx->TLF01_details.product_codes[15].code,DE48_TAG67,2);
			// Point to data of Subfeild 1 store MoneySend Information data.
			strncpy(p_auth_tx->TLF01_details.product_codes[15].code+2,&src[MainIndex+4],3);
		   	}
			 else
			 {
				 /* Both SF01 and SF02 is present */
				 /* Copy SF01 */
				strncpy(p_auth_tx->TLF01_details.product_codes[15].code,DE48_TAG67,2);
				strncpy(p_auth_tx->TLF01_details.product_codes[15].code+2,&src[MainIndex+4],3);

				/* Copy SF02 .. ATP can support only 24 bytes out of 58 byes */
				strncpy(SE67_SubFld2, &src[MainIndex+9], 2);
				tag_len = atoi(SE67_SubFld2);
				if (tag_len > 0 && tag_len <= 12)
				{
					strncpy(p_auth_tx->TLF01_details.product_codes[8].amount, &src[MainIndex+11], tag_len);
				}
				else if (tag_len > 12 && tag_len <= 24)
				{
					strncpy(p_auth_tx->TLF01_details.product_codes[8].amount, &src[MainIndex+11], 12);
					strncpy(p_auth_tx->TLF01_details.product_codes[14].amount, &src[MainIndex+23], tag_len -12);
				}
			 }

		}
         else if ( 0 == strncmp(tag, DE48_TAG88, 2) )
         {
            memcpy( p_auth_tx->MCF01_details.network[8].priority_routing,
                    &src[MainIndex], 1 );
         }
         else if ( 0 == strncmp(tag, DE48_TAG89, 2) )
         {
            ret_val = 89;
            memcpy( p_auth_tx->MCF01_details.network[9].priority_routing,
                    &src[MainIndex], 1 );
         }
         else if(0 == strncmp(tag, DE48_TAG22, 2))
         {
        	 memcpy( SE22_SubFld1,&src[MainIndex],2 );
        	 if(0 == strncmp(SE22_SubFld1,DE48_TAG22SF01,2))
        	 {
        		 memcpy( SE22_SubFld2,&src[MainIndex+2],2 );
        		 if(0 == strncmp(SE22_SubFld2,"06",2))
        		 {// its a value copy it into TLF01
        			 memcpy(p_auth_tx->TLF01_details.product_codes[4].quantity,SE22_SubFld2,2);
        		 }
        		 else
        		 {// SF01 length is available read the length and cpoy the data
        			 memcpy(p_auth_tx->TLF01_details.num_dbsales,SE22_SubFld2,2);
        			 memcpy(p_auth_tx->TLF01_details.product_codes[4].quantity,&src[MainIndex+4],2);
        		 }
        	 }
        	 else if (0 == strncmp(SE22_SubFld1, DE48_TAG22SF05, 2 ))
        	 {
        		 //copy the value of SF05 into TLF01
        		 memcpy(p_auth_tx->TLF01_details.product_codes[5].code,&src[MainIndex+4],4);

        	 }
     		else if(0 == strncmp(SE22_SubFld1, DE48_TAG22_SF06+4, 2) )
     		{
     			memcpy(p_auth_tx->TLF01_details.product_codes[8].quantity,&src[MainIndex+4],1);
     		}
        	 else
        	 {
        		 //ignore
        	 }

         }
         else if ( 0 == strncmp(tag, DE48_TAG75, 2) )
         {
        	 if (13 == sub_field_len)
        	 {
        		 /* Store only DE48 SE75 SF01,02 */
             	memcpy(SE75_SubFld1,&src[MainIndex],2 );
             	if(0 == strncmp(SE75_SubFld1,"01",2))
             	{
             		/* Store SF01 data and SF02 tag, data */
             		memcpy(p_auth_tx->TLF01_details.product_codes[2].amount,&src[MainIndex+2],11);
             	}
        	 }
        	 else if (sub_field_len > 13)
        	 {
        		 /* Store all the DE48 SE75 SF01,02,03,04 */
              	memcpy(SE75_SubFld1,&src[MainIndex],2 );
              	if(0 == strncmp(SE75_SubFld1,"01",2))
              	{
              		/* Store SF01 data and SF02 tag, data */
              		memcpy(p_auth_tx->TLF01_details.product_codes[2].amount,&src[MainIndex+2],11);
              	}
              	memcpy(SE75_SubFld3,&src[MainIndex + 13],2 );
              	if(0 == strncmp(SE75_SubFld3,"03",2))
              	{
              		/* Store SF03 data and SF04 tag, data */
              		memcpy(p_auth_tx->TLF01_details.product_codes[3].amount,&src[MainIndex+15],11);
              	}
        	 }

         }
         else if(0 == strncmp(tag, DE48_TAG77, 2))
         {
        	 //strncpy(p_auth_tx->TLF01_details.product_codes[13].code, DE48_TAG77, 2);
        	 strncpy(p_auth_tx->TLF01_details.product_codes[13].code, &src[MainIndex], 3 );
         }
         else if (0 == strncmp(tag, DE48_TAG80, 2))
         {
        	 //strncpy(p_auth_tx->TLF01_details.product_codes[13].code, DE48_TAG77, 2);
        	 strncpy(p_auth_tx->TLF01_details.agent_id_len, &src[MainIndex], 2 );
         }
         else if(0 == strncmp(tag, DE48_TAG37, 2))
         {
        	 if(sub_field_len == 7)
        	 {
        		 memcpy( SE37_SubFld4,&src[MainIndex],2 );
        		 if(0 == strncmp(SE37_SubFld4,DE48_TAG37+4,2))
        		 {
        			 memcpy(p_auth_tx->TLF01_details.orig_auth_number,&src[MainIndex+4],3 );
        		 }
        		 else
        		 {
        			 //ignore
        		 }
        	 }
        	 else if(sub_field_len == 15)
        	 {
        		 memcpy( SE37_SubFld5,&src[MainIndex],2 );
        		 if(0 == strncmp(SE37_SubFld5,DE48_TAG37_SF05+4,2))
        		 {
        			 memcpy(p_auth_tx->TLF01_details.dbrefund_amount,&src[MainIndex+4],11 );
        		 }
        		 else
        		 {
        			 //ignore
        		 }
        	 }
        	 else if(sub_field_len >=22) // Use this when both SF4 and SF5 are present.
        	 {
			total_index = MainIndex+sub_field_len;
                   dummy_mainindex = MainIndex;
        	     while(dummy_mainindex < total_index)
        	     {
        	       memcpy( SE37_SubFld,&src[dummy_mainindex],2 );
        	     dummy_mainindex += 2;
        	       memcpy( len, &src[dummy_mainindex], 2 );
        	       SubFieldIDLen = atoi( len );
        	       dummy_mainindex += 2;
        	       if(0 == strncmp(SE37_SubFld,DE48_TAG37+4,2))
        	       {
        	          memcpy(p_auth_tx->TLF01_details.orig_auth_number,&src[dummy_mainindex],SubFieldIDLen );
        	          dummy_mainindex=dummy_mainindex+SubFieldIDLen;
        	          continue;

        	       }
        	       else if(0 == strncmp(SE37_SubFld,DE48_TAG37_SF05+4,2))
        	       {
         	         memcpy(p_auth_tx->TLF01_details.dbrefund_amount,&src[dummy_mainindex],SubFieldIDLen );
         	         dummy_mainindex=dummy_mainindex+SubFieldIDLen;
        	          break;
			}
        	       else
        	        {
        	            //ignore
        	        }
        	        dummy_mainindex=dummy_mainindex+SubFieldIDLen;
        	      }
        	  }
        	 else
        	 {
        		 ncmcrd_log_message( 1,2, "Invalid tag length", "parse_field_48" , 0 ,"");

        	 }



         }



      MainIndex += sub_field_len;
   }
   return( ret_val );
}  /* End parse_field_48 */


/******************************************************************************
 *
 *  NAME:         Find_Tran_Table
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
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
struct trandef *Find_Tran_Table( pAUTH_TX p_auth_tx )
{
   INT  i;            /* loop index  */
   BYTE trantype;
   CHAR Buffer[512]={0};

   trantype = p_auth_tx->TLF01_details.tx_key;
   for ( i=0; (i <= N_TRANENTRY) && (TRANTABLE[i].td_trantype > 0); i++ )
   {
      if ( TRANTABLE[i].td_trantype == trantype )
      {
         return( &TRANTABLE[i] );
         break;
      }
   }

   /* Did not find trantype in table - log error & return NULL */
   sprintf( Buffer, "This trantype, %d, not found in TRANTABLE", trantype );
   memset(Log_Bufr,0x00,sizeof(Log_Bufr));
   sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",p_auth_tx->TLF01_details.terminal_id,
		   p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
   memcpy( p_auth_tx->TLF01_details.response_text, Buffer,
                 sizeof(p_auth_tx->TLF01_details.response_text)-1 );
   Mask_card_number(p_auth_tx->TLF01_details.card_num);
   if (masked_card_num[0] != '\0' )
   {
	   strcat (Log_Bufr , " Card Num : ");
	   strcat (Log_Bufr ,masked_card_num );
   }
   strcat(Buffer,Log_Bufr);
   ncmcrd_log_message( 1, 3, Buffer , "FIND_TRAN_TABLE" , 0 ,"");

   return NULL;
} /* end Find_Tran_Table */   


/******************************************************************************
 *
 *  NAME:         Find_Bitfield_Table
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
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
struct bitfield_data *Find_Bitfield_Table( enum bitfields bitfield )
{
   int      i;                    /* loop index */
   char     temp_str[60];

   for ( i=0; i <= N_BFData; i++ )
      if ( BFData[i].bd_fieldid == bitfield )
         return &BFData[i];  /* Found the field - return address */

   /* Field not found in table - log error and return NULL */
   sprintf(temp_str, "This bitfield not found in BFData, bitfield: %d",
                      bitfield);
   ncmcrd_log_message( 1, 3, temp_str , "Find_Bitfield_Table" , 0 ,"");
   return NULL;

} /* end Find_Bitfield_Table */


/******************************************************************************
*
* FUNCTION:     mcrdutil_send_host_response
*
* DESCRIPTION:  Sends response back to terminal
*               
*
* INPUTS:       p_auth_tx - Transaction record
*
* OUTPUTS:      None
*
*  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
*
* AUTHOR:       Dennis Irby
*
* MODIFICATIONS
*    DI - 03/23/99 - Added transaction finish time
*******************************************************************************/
INT mcrdutil_send_host_response( pAUTH_TX p_auth_tx )
{
   pPTE_MSG   p_msg_out;
   INT        ret_code = OK;
   CHAR       err_msg[512]={0};

   p_msg_out = ptemsg_build_msg( MT_OUTGOING,
                                 ST1_NONE,
                                 ST2_NONE,
                                 dialog_que_name,
                                 application_que_name,
                                 ReqRsp_Buffer,
                                 buflen,
                                 0); 

   if ( NULL_PTR == p_msg_out )
   {
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Insufficient Memory to build host message Term Id:%s ,Merch Id: %s RRN: %s ",
      		p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      memcpy( p_auth_tx->TLF01_details.response_text, "Insufficient Memory to build host message",
                    sizeof(p_auth_tx->TLF01_details.response_text)-1 );
      ncmcrd_log_message( 3, 3, Log_Bufr , "mcrdutil_send_host_response" , 0 ,p_auth_tx->TLF01_details.card_num);
   }
   else
   {
      memset( err_msg, 0x00, sizeof(err_msg) );
       if ( ncf01_i.tpdu_id[0] != 0x00 )
         ptemsg_set_orig_comms_info(p_msg_out,ncf01_i.tpdu_id);
	   else if ( p_auth_tx->TLF01_details.tpdu_id[0] != 0x00 )
         ptemsg_set_orig_comms_info(p_msg_out,p_auth_tx->TLF01_details.tpdu_id);

      else
      {
         strcpy( err_msg, "Do not know how to send msg to host. No Comm info.");
         ret_code = NOT_OK;
	  }

      if ( OK == ret_code )
         ret_code = send_msg( p_msg_out, dialog_que_name, err_msg );

      if ( OK != ret_code )
      {
         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
         					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
         memcpy( p_auth_tx->TLF01_details.response_text, err_msg,
                       sizeof(p_auth_tx->TLF01_details.response_text)-1 );
         strcat(err_msg,Log_Bufr);
         ncmcrd_log_message( 3, 2, err_msg , "mcrdutil_send_host_response" , 0 ,p_auth_tx->TLF01_details.card_num);
      }
      else
      {
         /* Update transaction statistics */
         update_txn_stats( &Txn_Counts, p_auth_tx, SENT );

         /* Log transaction to trace file. */
         write_to_txn_file();
      }
      free( p_msg_out );
   }
   return( ret_code );
}
/******************************************************************************
 *
 *  NAME:         BUILD_HOST_REQUEST
 *
 *  DESCRIPTION:  This procedure copies the information from the HOST_MESSAGE
 *                structure into a character string.  The character string is
 *                then shipped to the host.
 *
 *  INPUTS:       p_auth_tx - Transaction record
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 *     DI - 03/16/99 - Added extended bitmap for File Updates
 ******************************************************************************/
INT build_host_request( pAUTH_TX p_auth_tx )
{
   struct trandef       *pTranDef;
   struct bitfield_data *pBF;
   CHAR                 Buff[512]= {0};
   BYTE                 trantype;
   int                  bfidx;         /* index to bitfields for this transaction */

   /* Get pointer to transaction table */
   trantype = p_auth_tx->TLF01_details.tx_key;
   pTranDef = Find_Tran_Table( p_auth_tx );

   if ( NULL == pTranDef )
      return NOT_OK; /* Did not find TX type - so quit */
       
   /*-----------------------------------------*/
   /* Copy in Message Id for this transaction */
   /*-----------------------------------------*/
   memset(ReqRsp_Buffer, 0, COMM_BUF_SZE);   /* zero out the buffer */
   bitmapidx = 4;                            /* point to bitmap     */

   /* Point past bitmap   */
   if ( AUTH_LOGON == trantype  ||  AUTH_LOGON_RESPONSE == trantype )
   {
      reqidx = 20;
      ReqRsp_Buffer[bitmapidx] |= BITTAB[bf_1xbitfield].bit_mask;   /* 2 bitmaps */
      BitFieldSize = EXTENDED_BITFIELD;
   }
   else if ( AUTH_ADMIN_ADVICE_RESPONSE == trantype )
   {
      reqidx = 20;
      ReqRsp_Buffer[bitmapidx] |= BITTAB[bf_1xbitfield].bit_mask;   /* 2 bitmaps */
      BitFieldSize = EXTENDED_BITFIELD;
   }
   else if ( AUTH_CARD_UPDATE == trantype )
   {
      reqidx = 20;
      ReqRsp_Buffer[bitmapidx] |= BITTAB[bf_1xbitfield].bit_mask;   /* 2 bitmaps */
      BitFieldSize = EXTENDED_BITFIELD;
   }
   /* BDO wants to send Reversal txn MC host TF-Ajay  30-Oct-2009 */
   else if ( AUTH_REVERSAL == trantype || AUTH_VOID_PRE_AUTHORIZATION == trantype)
   {
      reqidx = 20;
      ReqRsp_Buffer[bitmapidx] |= BITTAB[bf_1xbitfield].bit_mask;   /* 2 bitmaps */
      BitFieldSize = EXTENDED_BITFIELD;
   }
   else if ( AUTH_VOID_SALE == trantype )
   {
      reqidx = 20;
      ReqRsp_Buffer[bitmapidx] |= BITTAB[bf_1xbitfield].bit_mask;   /* 2 bitmaps */
      BitFieldSize = EXTENDED_BITFIELD;
   }
   else if ( AUTH_REVERSAL_RESPONSE == trantype || AUTH_REVERSAL_ADVICE_RESPONSE == trantype || AUTH_VOID_PRE_AUTHORIZATION_RESPONSE == trantype)
   {
      reqidx = 20;
      ReqRsp_Buffer[bitmapidx] |= BITTAB[bf_1xbitfield].bit_mask;   /* 2 bitmaps */
      BitFieldSize = EXTENDED_BITFIELD;
   }
   else if ( AUTH_VOID_SALE_RESPONSE == trantype )
   {
      reqidx = 20;
      ReqRsp_Buffer[bitmapidx] |= BITTAB[bf_1xbitfield].bit_mask;   /* 2 bitmaps */
      BitFieldSize = EXTENDED_BITFIELD;
   }
   else if( p_auth_tx->TLF01_details.deferred_term_length[0]!= 0x00)
   {
	   reqidx = 20;
	   ReqRsp_Buffer[bitmapidx] |= BITTAB[bf_1xbitfield].bit_mask;	 /* 2 bitmaps */
	   BitFieldSize = EXTENDED_BITFIELD;
   }
   else if( strlen(p_auth_tx->future_use_1) > 0)
   {
	   if( (0 == strncmp( p_auth_tx->TLF01_details.message_type,"0100",4))||
		   (0 == strncmp( p_auth_tx->TLF01_details.message_type,"0200",4)))
	   {
		   reqidx = 20;
		   ReqRsp_Buffer[bitmapidx] |= BITTAB[bf_1xbitfield].bit_mask;	 /* 2 bitmaps */
		   BitFieldSize = EXTENDED_BITFIELD;
	   }
   }
   else if(AUTH_MONEY_SEND == trantype || AUTH_MONEY_SEND_RESPONSE == trantype)
   {
	   reqidx = 20;
	   ReqRsp_Buffer[bitmapidx] |= BITTAB[bf_1xbitfield].bit_mask;	 /* 2 bitmaps */
	   BitFieldSize = EXTENDED_BITFIELD;

   }
   else if((MC_ASI_TXN_IDENTIFIER_VALUE == p_auth_tx->TLF01_details.product_codes[16].amount[6])&&
			  strncmp(p_auth_tx->TLF01_details.processing_code ,"000000",6)==0 &&
			  strncmp(p_auth_tx->TLF01_details.total_amount ,"000000000000",12)==0 &&
			  p_auth_tx->MCF01_details.credit_purch_max[0]!='\0')
   {
	   reqidx = 20;
	   ReqRsp_Buffer[bitmapidx] |= BITTAB[bf_1xbitfield].bit_mask;	 /* 2 bitmaps */
	   BitFieldSize = EXTENDED_BITFIELD;
   }
#ifdef MC01_APR2024
   else if( p_auth_tx->TLF01_details.product_codes[19].amount[0] != 0x00 ||
		    p_auth_tx->TLF01_details.product_codes[10].amount[0] != 0x00) //DE105 - TLID is present
   {
	   reqidx = 20;
	   ReqRsp_Buffer[bitmapidx] |= BITTAB[bf_1xbitfield].bit_mask;	 /* 2 bitmaps */
	   BitFieldSize = EXTENDED_BITFIELD;
   }
#endif
   else
   {
      reqidx = 12;
      BitFieldSize = NORMAL_BITFIELD;
   }

   memcpy(&ReqRsp_Buffer[0], pTranDef->td_msgid, sizeof(pTranDef->td_msgid));

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
         pBF = Find_Bitfield_Table(pTranDef->td_bitfields[bfidx]);

		 if ( NULL == pBF )
         {
            sprintf( Buff,"Unable to find bitfield (%d) to build msg for host.",
                     bfidx );
            memset(Log_Bufr,0x00,sizeof(Log_Bufr));
            sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
            		p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
            memcpy( p_auth_tx->TLF01_details.response_text, Buff,
                          sizeof(p_auth_tx->TLF01_details.response_text)-1 );
            strcat(Buff,Log_Bufr);
            ncmcrd_log_message( 3, 2, Buff , "build_host_request" , 0 ,p_auth_tx->TLF01_details.card_num);
            return NOT_OK; /* Invalid bit was set - so quit. */
         }

         /* Field 38 */
         if(bfidx == 38)
         {
            if(strncmp(p_auth_tx->TLF01_details.response_code,APPROVED,2) != 0)
               continue;
         }

         /*-----------------------------------------------------------*/
         /* if conditional routine returns true, move the bitfield... */
         /*-----------------------------------------------------------*/
         if ((pBF->bd_BFCond)(pBF->bd_fieldid,trantype) == true)
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
      }
      else
      {
         bfidx = BitFieldSize;
      }  /* end if */
   } /* end for */
   
   buflen = reqidx;
   return( OK );
} /* end Build_Request_Message */



/******************************************************************************
 *
 *  NAME:         MCRDUTIL_PARSE_HOST_RESPONSE_MSG
 *
 *  DESCRIPTION:  This procedure copies information from a string sent by the
 *                host.  It copies it into the HOST_MESSAGE structure.  The
 *                data is converted from EBCDIC to ASCII as they are moved.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT mcrdutil_parse_host_response_msg()
{
   struct bitfield_data *pBF;
   INT                   bfidx;    /* Index to bitfields for this transaction */
   INT                   i;
   CHAR                  Buff[70];


   /* Initialize the HOST_MESSAGE structure. */
   memset(&HOST_MESSAGE, 0x00, sizeof(HOST_MESSAGE));

   /* Extract the message type from the host message. */
   memcpy( HOST_MESSAGE.msgtype, &ReqRsp_Buffer[0],
           sizeof(HOST_MESSAGE.msgtype) );

   /* Convert Message Type from EBCDIC to ASCII. */
   for( i=0; i<4; i++ )
      HOST_MESSAGE.msgtype[i] = ascii_ebcdic_conv(HOST_MESSAGE.msgtype[i], E2A);

   bitmapidx = 4;       /* point to bitmap */

   /* Point past bitmap, but 1st check for extended bitmap. */
   if ( ReqRsp_Buffer[bitmapidx]  &  BITTAB[bf_1xbitfield].bit_mask )
   {
      reqidx = 20;                         /* Extended     */
      BitFieldSize = EXTENDED_BITFIELD;
   }
   else
   {
      reqidx = 12;                         /* Not Extended */
      BitFieldSize = NORMAL_BITFIELD;
   }

   /*------------------------------------------------------------------
      For every bitfield present in message ( except first bit ) . . .
   -------------------------------------------------------------------*/
   for( bfidx = 2; bfidx <= BitFieldSize; bfidx++ )
   {
      if ( ReqRsp_Buffer[bitmapidx+BITTAB[bfidx].byte_no] &
           BITTAB[bfidx].bit_mask )
      {
         /* Bit is on, field is there.... */
         pBF = Find_Bitfield_Table(bfidx);

         if ( NULL == pBF )
         {
            sprintf( Buff, "Unexpected bitfield (%d) in message from host.",
                     bfidx );
            strcat( Buff, " MsgType = " );
            strncat( Buff, HOST_MESSAGE.msgtype, 4 );
            ncmcrd_log_message( 3, 3, Buff , "mcrdutil_parse_host_response_msg" , 0 ,"");
            return NOT_OK; /* Invalid field in message - so quit. */
		 }

         /*---------------------------------------*/
         /* call the Response Handling routine... */
         /*---------------------------------------*/
	
         (pBF->bd_BFRsp)(pBF->bd_fieldid);
         if ( true == invalid_msg_format )
         {
            strcpy( Buff, "Invalid MasterCard message format." );
            strcat( Buff, " MsgType = " );
            strncat( Buff, HOST_MESSAGE.msgtype, 4 );
            ncmcrd_log_message( 3, 3, Buff , "mcrdutil_parse_host_response_msg" , 0,"");
            return( NOT_OK );
         }
      }
   }
   return( OK );
} /* mcrdutil_parse_host_response_msg */   


/******************************************************************************
 *
 *  NAME:         MOVE_ISO_RESPONSE_TO_AUTH_STRUCT
 *
 *  DESCRIPTION:  This procedure copies information from the HOST_MESSAGE
 *                structure into the auth_tx structure.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      p_auth_tx - Transaction record containing data from host
 *                resp_code - Decline response code if validation fails
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT move_iso_response_to_auth_struct( pAUTH_TX p_auth_tx, pBYTE resp_code )
{
   INT   ret_val = OK;
   INT   de48_retcode = 0;
   pCHAR tempptr;    
   CHAR  tempstr[1024] = {0};
   CHAR  errbuf[512]={0};
   CHAR  itemp[5] = {0};
   CHAR  temp_card_num[20] = {0};
   INT   iF60_len = 0;
   INT   F54_len = 0;
   INT  F56_len=0;
   BYTE  ams_choice = 0;
   CHAR asci_de124_len[4] = {0};
   BYTE  tem_len[3] = {0};
   INT   len = 0;
   INT bitfiled_error = 0;

   INT      i, j,de124_len=0;
   BOOLEAN  atm = false;
   INT De48_len = 0;

   BYTE strfield_120_len[4]={0};
   int field_120_len =0;
   CHAR time_date[25];

   /* MESSAGE TYPE */
   memcpy( p_auth_tx->TLF01_details.message_type,
           HOST_MESSAGE.msgtype, sizeof(HOST_MESSAGE.msgtype ) );

   /* FIELD 2: PAN */
   memcpy( p_auth_tx->TLF01_details.card_num_len,HOST_MESSAGE.pan_len,bf2_len);
   memcpy( p_auth_tx->TLF01_details.card_num, HOST_MESSAGE.pan,
           atoi( p_auth_tx->TLF01_details.card_num_len ) );

   if (( strlen(p_auth_tx->TLF01_details.card_num) > 19) ||
	   (false == MC_isnum(p_auth_tx->TLF01_details.card_num)))
   {
	   bitfiled_error = 2;
   }

   /* FIELD 3: PROCESSING CODE */
   memcpy( p_auth_tx->TLF01_details.processing_code,
           HOST_MESSAGE.proccode, sizeof(HOST_MESSAGE.proccode) );

	if( strncmp(p_auth_tx->TLF01_details.handler_queue,"dcp",3) == 0 ||
		strncmp(p_auth_tx->TLF01_details.handler_queue,"VOICE",3) == 0	)
	{
		// do nothing as you copied it in while sendig the request
	}
	else
	{
		   /* In order to show proceesing code in transaction lookup page( OCT-2017 Mandated changes) */
		memcpy( p_auth_tx->TLF01_details.product_codes[17].amount,
		           HOST_MESSAGE.proccode, sizeof(HOST_MESSAGE.proccode) );
	}

	if (((strlen(p_auth_tx->TLF01_details.processing_code) > 6) ||
		 (false == MC_isnum(p_auth_tx->TLF01_details.processing_code))) &&
		(0 == bitfiled_error))
	{
		bitfiled_error = 3;
	}

   /* FIELD 4: AMOUNT  in case this is a Balance Inquiry transaction. */
   memcpy( p_auth_tx->TLF01_details.total_amount,
           HOST_MESSAGE.amount, bf4_len );

   /* FIELD 5: SETTLEMENT AMOUNT */
   memcpy( p_auth_tx->TLF01_details.settlement_total,
           HOST_MESSAGE.settle_amt, bf5_len ); 

   /* FIELD 6: CARDHOLDER BILLING AMOUNT */
   memcpy( p_auth_tx->ch_billing_amt, HOST_MESSAGE.ch_bill_amt, bf6_len ); 

   /* FIELD 7: TRANSMISSION TIME */
   memcpy( p_auth_tx->TLF01_details.transmission_timestamp,
           HOST_MESSAGE.gmt_time, bf7_len );

   /* FIELD 9: SETTLEMENT CONVERSION RATE */
   memcpy( p_auth_tx->TLF01_details.conversion_rate,
           HOST_MESSAGE.conv_rate_stl, bf9_len );
   /* Store DE-9 in product13-amount, required populate in SBATCH for advices - TF phani*/		
   memcpy( p_auth_tx->TLF01_details.product_codes[12].amount,
           HOST_MESSAGE.conv_rate_stl, bf9_len );	
   /* Added for equitable for echo of conversion rate */
   memcpy(p_auth_tx->settlement_conv_rate,HOST_MESSAGE.conv_rate_stl,bf9_len);

   /* FIELD 10: CARDHOLDER BILLING CONVERSION RATE */
   memcpy(p_auth_tx->ch_billing_conv_rate,HOST_MESSAGE.conv_rate_chb,bf10_len);

   /* FIELD 11: SYSTEMS TRACE AUDIT NUMBER */
   memcpy( p_auth_tx->TLF01_details.sys_trace_audit_num,
           HOST_MESSAGE.stan, sizeof(HOST_MESSAGE.stan) );

   /* FIELD 12: TRAN TIME hhmmss */
   memcpy( p_auth_tx->TLF01_details.time_hhmmss, HOST_MESSAGE.time, bf12_len);

   /* FIELD 13: TRAN DATE mmdd */
   if ( 0x00 != HOST_MESSAGE.date[0] )
   {
	   genutil_format_date(time_date);
	   memcpy( p_auth_tx->TLF01_details.date_yyyymmdd,   time_date, 4);
	   memcpy( p_auth_tx->TLF01_details.date_yyyymmdd+4, HOST_MESSAGE.date, bf13_len);
   }

   /* FIELD 14: EXPIRATION DATE */
   memcpy( p_auth_tx->TLF01_details.exp_date, HOST_MESSAGE.date_expd, bf14_len);

   /* FIELD 15: SETTLEMENT DATE (MMDD) */
   memcpy( p_auth_tx->TLF01_details.settlement_date,
           HOST_MESSAGE.date_settle, bf15_len );

   /* FIELD 16: CONVERSION DATE */
   memcpy( p_auth_tx->conv_date, HOST_MESSAGE.conv_date, bf16_len );

   /* FIELD 18: MERCHANT TYPE / ACQUIRER SIC NUMBER */
   memcpy( p_auth_tx->TLF01_details.category_code,
           HOST_MESSAGE.sic_number, bf18_len );

#ifdef MC10_OCT2023
   /* FIELD 19: Acquiring Country code */
   memcpy( p_auth_tx->TLF01_details.num_dbrefunds,
           HOST_MESSAGE.ctry_code, bf19_len );
#endif
   /* FIELD 20: PAN COUNTRY CODE */
   memcpy( p_auth_tx->country_code, HOST_MESSAGE.pan_ctry_cd, bf20_len );

   /* FIELD 22: POS ENTRY MODE */
   if ( 0x00 != HOST_MESSAGE.posentry[0] )
   {
      /* Convert POS Entry Mode to Hypercom Standard.
       *
       * Hypercom Standard needs a preceding zero.
       *
       * If ecommerce, set to all zeroes,
       * else if card swipe (90n), convert '90' to '02',
       * else use the same values.
       */
	  strncpy(p_auth_tx->TLF01_details.product_codes[16].code,HOST_MESSAGE.posentry,3);// Store orginal POS mode for Cadience
      if(0 == strncmp(p_auth_tx->TLF01_details.category_code,"5542",4))
      {
    	  // Don't change the pos entry mode send as it is-OCT 2019.
    	  strncpy(p_auth_tx->TLF01_details.pos_entry_mode,HOST_MESSAGE.posentry,3);
      }
      else if ( HOST_MESSAGE.posentry[0] == '8' )
      {
         /* E-commerce */
         strcpy( p_auth_tx->TLF01_details.pos_entry_mode, "080" );
		 p_auth_tx->TLF01_details.pos_entry_mode[2] = HOST_MESSAGE.posentry[1];
      }
      else if ( HOST_MESSAGE.posentry[0] == '9' )
      {
         /* Card Swipe */
         strcpy( p_auth_tx->TLF01_details.pos_entry_mode, "002" );
      }
      else if(0 == strncmp(HOST_MESSAGE.posentry,"10",2))
      {
    	  strcpy( p_auth_tx->TLF01_details.pos_entry_mode, "010" );
      }
      else
      {
         /* Manual or unspecified */
         strcpy( p_auth_tx->TLF01_details.pos_entry_mode, "00" );
         p_auth_tx->TLF01_details.pos_entry_mode[2] = HOST_MESSAGE.posentry[1];
      }
      p_auth_tx->TLF01_details.pos_entry_mode[3] = HOST_MESSAGE.posentry[2];
   }
	if( strncmp(p_auth_tx->TLF01_details.handler_queue,"dcp",3) == 0 ||
		strncmp(p_auth_tx->TLF01_details.handler_queue,"VOICE",3) == 0	)
	{
		// do nothing as you copied it in while sendig the request
	}
	else
	{
		memcpy(p_auth_tx->TLF01_details.product_codes[9].code,p_auth_tx->TLF01_details.pos_entry_mode,sizeof(p_auth_tx->TLF01_details.pos_entry_mode)-1);
	}

/* FIELD 23: CARD SEQ NBR */
    if ( 0x00 != HOST_MESSAGE.card_seq_num[0] )
   {
		memcpy(p_auth_tx->EMV_details.pan_sequence_number,HOST_MESSAGE.card_seq_num,3);
		memcpy(p_auth_tx->host2_message_seq_nbr,HOST_MESSAGE.card_seq_num,3);

   }

	/* FIELD 26 : pos_pincode */
	if ( 0x00 != HOST_MESSAGE.pos_pincode[0] )
   {
		memcpy(p_auth_tx->TLF01_details.product_codes[17].code,HOST_MESSAGE.pos_pincode,2);
	
   }
	// Field 28
	if( 0x00 != HOST_MESSAGE.tran_fee[0])
	{
		memcpy(p_auth_tx->TLF01_details.tran_fee_amount,HOST_MESSAGE.tran_fee,sizeof(HOST_MESSAGE.tran_fee));
	}

   /* FIELD 32: ACQUIRING INSTITUTION IDENTIFICATION CODE */
   memset( itemp, 0x00, sizeof(itemp) );
   memcpy( itemp, HOST_MESSAGE.acq_inst_len, bf32_len );
   i = atoi(itemp);
   if ( i > sizeof(HOST_MESSAGE.acq_inst_id) )
      i = sizeof(HOST_MESSAGE.acq_inst_id);
   memcpy( p_auth_tx->TLF01_details.acquiring_id, HOST_MESSAGE.acq_inst_id, i );

   /* FIELD 33: FORWARDING INSTITUTION IDENTIFICATION CODE */
   memset( itemp, 0x00, sizeof(itemp) );
   memcpy( itemp, HOST_MESSAGE.fwd_inst_len, bf33_len );
   i = atoi(itemp);
   if ( i > sizeof(HOST_MESSAGE.fwd_inst_id) )
      i = sizeof(HOST_MESSAGE.fwd_inst_id);
   memcpy( p_auth_tx->TLF01_details.forwarding_institution_id,
           HOST_MESSAGE.fwd_inst_id, i );
   
   /* FIELD 35: TRACK 2 */
   memset( tempstr, 0x00, sizeof(tempstr) );

   /* Parse the track2 for Field separator other than '='.
    * If the field separator is not equal to '=',
    * we will change the field separator to '='.
    */
   for(i=0;i<(INT)strlen(HOST_MESSAGE.track2);i++)
   {
     if(HOST_MESSAGE.track2[i] == 'D')
     {
        HOST_MESSAGE.track2[i] = '=';
        break;
     }
   }

   memcpy( p_auth_tx->TLF01_details.track2, HOST_MESSAGE.track2,
           sizeof(p_auth_tx->TLF01_details.track2)-1 );

   memcpy( tempstr, HOST_MESSAGE.track2,
           sizeof(p_auth_tx->TLF01_details.track2)-1 );

   if (0x00 != (tempptr = strchr(tempstr, '=')))
   {
      memcpy(p_auth_tx->TLF01_details.exp_date, tempptr+1,
             sizeof(p_auth_tx->TLF01_details.exp_date)-1);
	  memcpy(p_auth_tx->TLF01_details.service_code, tempptr+1+4,
             sizeof(p_auth_tx->TLF01_details.service_code)-1);
      *tempptr = 0x00;

      /* tempstr now contains only the card number.
       * If there is a card number in DE2, we need to compare
       * these two values.  They must be the same, else reject.
       * Reference SCR 1000.
       */
      if ( p_auth_tx->TLF01_details.card_num[0] != 0x00 )
      {
         if ( 0 != strcmp(tempstr,p_auth_tx->TLF01_details.card_num) )
         {
            /* They do not match. Log error and reject.
             * Continue processing to gather the rest of the txn info
             * to be echoed in the response to host.
             */
            strcpy( resp_code, DO_NOT_HONOR );
            ret_val = NOT_OK;
            bitfiled_error = 35;
            memset( errbuf, 0x00, sizeof(errbuf) );
            sprintf( errbuf,
                    "Rejecting %s from MC. DE2 (%s) does not match DE35 (%s).",
                     p_auth_tx->TLF01_details.message_type,
                     p_auth_tx->TLF01_details.card_num,
                     p_auth_tx->TLF01_details.track2 );
         }
      }

      if ( ret_val == OK )
      {
         memcpy(p_auth_tx->TLF01_details.card_num, tempstr,
                sizeof(p_auth_tx->TLF01_details.card_num)-1);
         sprintf( p_auth_tx->TLF01_details.card_num_len,"%02d",strlen(tempstr));
      }
   }

   /* FIELD 37: RETRIEVAL REFERENCE NUMBER */
   memcpy( p_auth_tx->TLF01_details.retrieval_ref_num,
           HOST_MESSAGE.rrn, sizeof(HOST_MESSAGE.rrn) );
   if ( p_auth_tx->TLF01_details.retrieval_ref_num[0] == 0x00 )
   {
	   // FLag it if RRN NULL
	   p_auth_tx->amex_pos_entry_mode[0]= '1';

   }
   /* FIELD 38: AUTH NUMBER */
   memcpy( p_auth_tx->TLF01_details.auth_number,
           HOST_MESSAGE.authcode, sizeof(HOST_MESSAGE.authcode ) );

   /* FIELD 39: RESPONSE CODE */
   memcpy( p_auth_tx->TLF01_details.response_code,
           HOST_MESSAGE.responsecd, sizeof(HOST_MESSAGE.responsecd) );
   /* Store the response code sent by MC to display in tranlookup*/
	if(strncmp(p_auth_tx->TLF01_details.message_type,"0120",4) ||
	   strncmp(p_auth_tx->TLF01_details.message_type,"0420",4) ||
	   strncmp(p_auth_tx->TLF01_details.response_code,"87",2))
	{
		strncpy(p_auth_tx->TLF01_details.product_codes[13].quantity, p_auth_tx->TLF01_details.response_code, 2);
	}



   if(strncmp (p_auth_tx->TLF01_details.response_code,"30",2) == 0)
   {
	  memset( errbuf, 0x00, sizeof(errbuf) );
	  memset(Log_Bufr,0x00,sizeof(Log_Bufr));
	  if( strncmp(p_auth_tx->TLF01_details.message_type,"0410",4) == 0 ||
		  strncmp(p_auth_tx->TLF01_details.message_type,"0430",4) == 0  )
	  {

		 strcpy( errbuf, " Reversal Transaction Rejected with RC : 30 ");
	  }
	  else
	  {
		  strcpy( errbuf, " Transaction Rejected with RC : 30 ");
	  }
	  sprintf( Log_Bufr," Stan:%s ,RRN: %s Txn date and Time : %s",
	      	   p_auth_tx->TLF01_details.sys_trace_audit_num,p_auth_tx->TLF01_details.retrieval_ref_num,p_auth_tx->TLF01_details.transmission_timestamp);
	  strcat (errbuf,Log_Bufr);
	  ncmcrd_log_message( 3, 3, errbuf, "move_iso_response_to_auth_struct" , 0 ,p_auth_tx->TLF01_details.card_num);
   }

   /* FIELD 41: TERMINAL ID */
   memcpy( p_auth_tx->TLF01_details.terminal_id,
           HOST_MESSAGE.termid, sizeof(HOST_MESSAGE.termid) );

   /* Do this in case terminal Id is not 8 characters.
    * For example, DBS has 7 character terminal Ids.
    */
   //trim_spaces( p_auth_tx->TLF01_details.terminal_id );

   /* FIELD 42: CARD ACCEPTOR IDENTIFICATION CODE */
   memcpy( p_auth_tx->TLF01_details.merchant_id,
           HOST_MESSAGE.cardaccid, bf42_len );

   /* FIELD 43:	CARD ACCEPTOR NAME / LOCATION
    *
    * Store DE43 here so it gets logged to TLF01:
    *   Merchant Name         --> TLF01.card_holder_name
    *   City and Country Code --> TLF01.ncf01_retired_cwk
    * SCR 775
    */
   memcpy( p_auth_tx->TLF01_details.card_holder_name,
           HOST_MESSAGE.cardaccloc,
           22 );
   memcpy( p_auth_tx->TLF01_details.ncf01_retired_cwk,
           HOST_MESSAGE.cardaccloc+23,
           13 );
   memcpy( p_auth_tx->TLF01_details.ncf01_retired_cwk+13, 
           &HOST_MESSAGE.cardaccloc[37],
           3 );

   /* FIELD 44: ADDITIONAL RESPONSE DATA */
   memcpy( p_auth_tx->TLF01_details.resp_source_len,
           HOST_MESSAGE.addl_rsp_len, sizeof(HOST_MESSAGE.addl_rsp_len) );
   memcpy( p_auth_tx->TLF01_details.resp_source,
           HOST_MESSAGE.addlrspdata, sizeof(HOST_MESSAGE.addlrspdata) );

   if ( 0 == strcmp(FORMAT_ERROR, p_auth_tx->TLF01_details.response_code) )
   {
      /* Request message had a format error.  Display contents of
       * DE44.  It usually indicates the field that had the error. */

      memset( tempstr, 0x00, sizeof( tempstr ) );
      sprintf( tempstr,
              "Format Err: Field 44 = \"%s\"",
               p_auth_tx->TLF01_details.resp_source );
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      		  p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      memcpy( p_auth_tx->TLF01_details.response_text, tempstr,
                    sizeof(p_auth_tx->TLF01_details.response_text)-1 );
      strcat(tempstr,Log_Bufr);
      ncmcrd_log_message( 2, 2, tempstr , "move_iso_response_to_auth_struct" , 0 ,p_auth_tx->TLF01_details.card_num);
   }

   /* FIELD 45: TRACK1 */
   if ( HOST_MESSAGE.track1[0] != 0x00 )
   {
      /* We have Track 1 data. */
      memset( tempstr, 0x00, sizeof(tempstr) );
      memcpy( p_auth_tx->TLF01_details.track1, &HOST_MESSAGE.track1[1],
              sizeof(p_auth_tx->TLF01_details.track1)-2 );
      strcpy( tempstr, p_auth_tx->TLF01_details.track1 );

      /* Get card number */
      if (0x00 != (tempptr = strchr(tempstr, '^')))
      {
         memset( temp_card_num, 0x00, sizeof(temp_card_num) );
         memcpy( temp_card_num, tempstr, tempptr-tempstr );

         /* temp_card_num now contains the card number from track 1.
          * If there is a card number in DE2, we need to compare
          * these two values.  They must be the same, else reject.
          * Reference SCR 1000.
          */
         if ( p_auth_tx->TLF01_details.card_num[0] != 0x00 )
         {
            if ( 0 != strcmp(temp_card_num,p_auth_tx->TLF01_details.card_num) )
            {
               /* They do not match. Log error and reject.
                * Continue processing to gather the rest of the txn info
                * to be echoed in the response to host.
                */
               strcpy( resp_code, DO_NOT_HONOR );
               ret_val = NOT_OK;
               bitfiled_error = 45;
               memset( errbuf, 0x00, sizeof(errbuf) );
               sprintf( errbuf,
                       "Rejecting %s from MC. DE2 (%s) does not match DE45 (%s).",
                        p_auth_tx->TLF01_details.message_type,
                        p_auth_tx->TLF01_details.card_num, temp_card_num );
            }
         }

         if ( (ret_val == OK) && (p_auth_tx->TLF01_details.card_num_len[0] == 0x00))
            sprintf( p_auth_tx->TLF01_details.card_num_len,"%02d",tempptr-tempstr);
      }

      /* Get exp date */
      /* Search for ^ from the END of track1 because track1 contains two ^s. */
      if ((tempptr = strrchr(tempstr, '^')) != 0x00)
      {
         memcpy( p_auth_tx->TLF01_details.exp_date, tempptr+1,
                 sizeof(p_auth_tx->TLF01_details.exp_date)-1);
      }
   }

   /* FIELD 48: ADDITIONAL DATA - ONLY NEED THE CVC STATUS FLAG. */
   memset( tempstr, 0x00, sizeof( tempstr ) );
   de48_retcode = parse_field_48( HOST_MESSAGE.addl_data_len,
                                  HOST_MESSAGE.addl_data,
                                  p_auth_tx );

   /* Do we need to decline this transaction based on a value in DE48? */
   if ( de48_retcode > 0 )
   {
      /* Yes, if this is an auth request. */
      if ( 0 == strcmp("0100",p_auth_tx->TLF01_details.message_type) )
      {
         /* It is a request; decline it if we are not already declining it.*/
         if ( ret_val == OK )
         {
            strcpy( resp_code, CALL_ISSUER );
            ret_val = NOT_OK;
            bitfiled_error = 48;
            memset( errbuf, 0x00, sizeof(errbuf) );
            sprintf( errbuf,
                "ATP rejects request from MC. DE48 subfield 89 = %s, STAN = %s",
                 p_auth_tx->MCF01_details.network[9].priority_routing,
                 p_auth_tx->TLF01_details.sys_trace_audit_num );
         }
      }
   }

   /* FIELD 48: ADDITIONAL DATA FOR REJECT MESSAGES */
   memset( addl_data_len, 0x00, sizeof( addl_data_len ) );
   memset( addl_data,     0x00, sizeof( addl_data     ) );

   memcpy( addl_data_len, HOST_MESSAGE.addl_data_len, 3 );
   memcpy( addl_data, HOST_MESSAGE.addl_data, atoi(addl_data_len) );

   /* FIELD 48: ADDITIONAL DATA - ATM CASH DISBURSEMENT */
   if ( addl_data[0] == 'Z' )
      atm = true;   /* There is supposed to be a pin block with this. */

   /* Allow us to echo the incoming data. Use this field due to its size */
   De48_len = atoi(addl_data_len);
   if(De48_len<= 128)
   {
	   memcpy( p_auth_tx->EMV_details.issuer_script_2,HOST_MESSAGE.addl_data,De48_len);
   }
   else if(De48_len > 128)
   {
	   /*DE48 length expected to be more than 128 bytes so future use 3,4 and 5 will be combinely used to store the DE48*/
	   p_auth_tx->future_use_3[0] = De48_len;
	   memcpy( p_auth_tx->future_use_3+1,HOST_MESSAGE.addl_data,De48_len);
   }


   /* The industry code will be set already if it is e-commerce.  This gets
    * set in function parse_field_48, called earlier.  So if it is already
    * set, skip this switch statement.
    */
   if ( p_auth_tx->industry_code[0] == 0x00 )
   {
      switch( addl_data[0] )
      {
         case 'R':  strcpy( p_auth_tx->industry_code, "600" ); break;
         case 'F':  strcpy( p_auth_tx->industry_code, "101" ); break;
         case 'O':  strcpy( p_auth_tx->industry_code, "110" ); break;
         case 'X':  strcpy( p_auth_tx->industry_code, "200" ); break;
         case 'H':  strcpy( p_auth_tx->industry_code, "300" ); break;
         case 'A':  strcpy( p_auth_tx->industry_code, "400" ); break;
         case 'T':  strcpy( p_auth_tx->industry_code, "500" ); break;
         case 'C':  strcpy( p_auth_tx->industry_code, "900" ); break;
         default:   strcpy( p_auth_tx->industry_code, "000" ); break;
      }
   }
   
   /* Save Transaction category code in TLF01.
    * Use remote_nsp_type field, which is not being used, to
    * avoid adding another field to TLF01.
    */
   p_auth_tx->TLF01_details.remote_nsp_type[0] = addl_data[0];

   /* FIELD 49: TRANSACTION CURRENCY CODE */
   memcpy( p_auth_tx->TLF01_details.currency_code,
           HOST_MESSAGE.tran_curr_cd, bf49_len );

   /* Save here where it is saved as original in TLF01: SCR 565, Equitable. */
   strcpy( p_auth_tx->TLF01_details.nfi_seq_nbr,
           p_auth_tx->TLF01_details.currency_code );

   /* Log amount here to keep a record of amt in orig currency. SCR 565/745 */
   memcpy( p_auth_tx->TLF01_details.sales_amount,
           HOST_MESSAGE.amount, 12 );
   memcpy(p_auth_tx->reconciliation_amount,
          p_auth_tx->TLF01_details.total_amount,
          sizeof(p_auth_tx->TLF01_details.total_amount));
   strcpy( p_auth_tx->reconciliation_cur_code,
              p_auth_tx->TLF01_details.currency_code );
  // strcpy( p_auth_tx->TLF01_details.orig_amount,
  //            p_auth_tx->TLF01_details.total_amount );

   /*+------------------------------------------------------------------+
     | Here is a requirement unique to Equitable:                       |
     | ------------------------------------------                       |
     |   Equitable must authorize in PESO or USD.  So if the currency   |
     |   in field 49 is "608" or "840", use field 4 for the amount field|
     |   and use the PESO Balance Account for authorization purposes.   |
     |                                                                  |
     |   If field 49 is neither, use field 5 for the amount field, it   |
     |   will be in US Dollars.  Use the Dollar Balance account for     |
     |   authorization purposes.  However, the original amount must be  |
     |   saved in order to echo it back to MasterCard.                  |
     |                                                                  |
     | This is for switched in, or Issuing, transactions only.          |
     +------------------------------------------------------------------+*/
   if ( (0 != strcmp( PESO, p_auth_tx->TLF01_details.currency_code )) &&
        (0 != strcmp( USD,  p_auth_tx->TLF01_details.currency_code )) )
   {
      /* Save original currency code and amount. */
      strcpy( p_auth_tx->reconciliation_cur_code,
              p_auth_tx->TLF01_details.currency_code );

      strcpy( p_auth_tx->TLF01_details.orig_amount,
              p_auth_tx->TLF01_details.total_amount );

/*      if (0 == strncmp(HOST_MESSAGE.card_curr_cd, USD, bf51_len ))
      {// DE51 will be parsed and stored below, We are using the DE51 prior just check the
          condition
          strcpy( p_auth_tx->TLF01_details.total_amount,
                  p_auth_tx->ch_billing_amt );

          strcpy( p_auth_tx->TLF01_details.currency_code, USD );
      }*/
      /* This is not PESO or USD, field 50 must be USD. */
      if ( 0 == strncmp( HOST_MESSAGE.settle_cur_cd, USD, bf50_len ) )
      {
         strcpy( p_auth_tx->TLF01_details.total_amount,
                 p_auth_tx->TLF01_details.settlement_total );

         strcpy( p_auth_tx->TLF01_details.currency_code, USD );
      }
      else
      {
         /* Field 50 is not USD. Blank out currency code to force an error.*/
         memset( p_auth_tx->TLF01_details.currency_code, 0x00,
                 sizeof(p_auth_tx->TLF01_details.currency_code) );
      }
   }

   /* FIELD 50: SETTLEMENT CURRENCY CODE */
   memcpy( p_auth_tx->settlement_curr_code,
           HOST_MESSAGE.settle_cur_cd, bf50_len );
   /* Store DE-50 in product13-code, required populate in SBATCH for advices - TF phani*/		
	memcpy( p_auth_tx->TLF01_details.product_codes[12].code,
           HOST_MESSAGE.settle_cur_cd, bf50_len );

   /* FIELD 51: CARDHOLDER BILLING CURRENCY CODE */
   memcpy( p_auth_tx->ch_billing_curr_code,HOST_MESSAGE.card_curr_cd,bf51_len);

   /* FIELD 52: PIN BLOCK */
   if (false == blank_or_null_string(HOST_MESSAGE.pinblock,
                                     sizeof(HOST_MESSAGE.pinblock)))
   {
      memset(tempstr, 0, sizeof(tempstr));
      genutil_bin_to_ascii(HOST_MESSAGE.pinblock, sizeof(HOST_MESSAGE.pinblock), 
                           &tempstr[0], sizeof(tempstr) );   
      memcpy( p_auth_tx->TLF01_details.pin_block, tempstr,
              sizeof(p_auth_tx->TLF01_details.pin_block)-1 );
   }
	

   /* FIELD 53: SECURITY RELATED CONTROL INFORMATION */
	if(HOST_MESSAGE.sec_cntl[0] != 0x00)
	{ memcpy( p_auth_tx->mac_key,HOST_MESSAGE.sec_cntl,
		      sizeof(HOST_MESSAGE.sec_cntl));                                  
	}

/*****************************************************************************/


   /* FIELD 54: ADDITIONAL AMOUNTS - CARDHOLDER BALANCE */
   if ( HOST_MESSAGE.addlamts[0] != 0x00 )
   {
      /*
       +----------------------------------------------------------------+
       | Just get current and available amounts:                        |
       |    Current amount   -> Amount Type = '01'  (Ledger Amount)     |
       |    Available amount -> Amount Type = '02'                      |
       |                                                                |
       | Field 54 is made up of subfields as follows:                   |
       |      +-----------+----------+-----------+--------+--------+    |
       |      | Acct Type | Amt Type | Curr Code | D or C | Amount |    |
       |      +-----------+----------+-----------+--------+--------+    |
       |   Position: 1-2     3-4        5-7         8        9-20       |
       |   where                                                        |
       |      Acct Type = Account Type                                  |
       |                     10 = Savings                               |
       |                     20 = Checking                              |
       |                     30 = Credit card                           |
       |      Amt Type  = Amount Type                                   |
       |                     01 = Ledger Balance (Current balance)      |
       |                     02 = Available Balance                     |
       |                     40 = Cash-back                             |
       |                     90 = Available Credit                      |
       |                     91 = Credit Limit                          |
       |      Currency Code                                             |
       |      D or C    = D is Debit (negative), C is Credit (positive) |
       |      Amount    = Account balance                               |
       +----------------------------------------------------------------+
      */
      memset( tempstr, 0x00, sizeof(tempstr) );
      memcpy( tempstr, HOST_MESSAGE.addl_len, bf54_len );

      F54_len = atoi(tempstr);
      if ( 120 < F54_len )
         F54_len = 120;

      memcpy( tempstr, HOST_MESSAGE.addlamts, F54_len );
      for( i=0; i<F54_len; i+=20 )
      {
         if ( 0 == strncmp(tempstr+2,"01",2) )
         {
            /* Current Balance */
            if ( 'D' == tempstr[7] )
               p_auth_tx->current_balance_sign = '-';
            else
               p_auth_tx->current_balance_sign = '+';

            memcpy( p_auth_tx->current_balance, tempstr+11, 9 );

            /* This is for the terminal. */
            memcpy( p_auth_tx->TLF01_details.total_amount, tempstr+8, 12 );

            /* Copy into this field for nceqit. */
            memcpy( p_auth_tx->ACF01_details.cash_available,
                    p_auth_tx->current_balance, 9 );
         }
         else if ( 0 == strncmp(tempstr+2,"02",2) )
         {
            /* Available Balance */
            if ( 'D' == tempstr[7] )
               p_auth_tx->available_balance_sign = '-';
            else
               p_auth_tx->available_balance_sign, '+';

            memcpy( p_auth_tx->available_balance, tempstr+11, 9 );

            /* Copy into this field for nceqit. */
            memcpy( p_auth_tx->ACF01_details.cash_limit,
                    p_auth_tx->available_balance, 9 );
         }
      }
   }

   /* FIELD 55: Integrated Circuit Card (ICC) System-related Data */
   if (false == blank_or_null_string(HOST_MESSAGE.iccdata,
                                     sizeof(HOST_MESSAGE.iccdata)))
   {

	 /* Convert length to BCD. */
      memset( itemp,   0x00, sizeof(itemp)   );
      memset( tempstr, 0x00, sizeof(tempstr) );

      memcpy( itemp+1, HOST_MESSAGE.iccdata_len, 3 );
      itemp[0] = '0';
      i = atoi( itemp );
      genutil_asc_to_bcd( itemp, 4, tempstr );
	  if(0 == strcmp("0110",p_auth_tx->TLF01_details.message_type) )// RESPONSE
	   {
			  memcpy( p_auth_tx->EMV_details.emv_block,tempstr,2);
			   /* Copy the rest of the data in, untouched. */
			  memcpy( p_auth_tx->EMV_details.emv_block+2,
					HOST_MESSAGE.iccdata, i );
	   }
	  else // REQUEST
      {
		 CHAR  ICCdata [256] = {0};	
		 memset(ICCdata, 0, sizeof(ICCdata));    
		 memcpy(ICCdata,  HOST_MESSAGE.iccdata, i);
		  /* Copy the BCD length into AUTH_TX's EMV Block */
	     // memcpy( p_auth_tx->EMV_details.emv_block, HOST_MESSAGE.iccdata_len, i+sizeof(HOST_MESSAGE.iccdata_len) );
			 memcpy( p_auth_tx->EMV_details.emv_block,tempstr,2);
	      /* Copy the rest of the data in, untouched. */
	      memcpy( p_auth_tx->EMV_details.emv_block+2,
	              HOST_MESSAGE.iccdata, i );
		  genutil_EMV_To_Auth_Tx( ICCdata, i, p_auth_tx );

	  }

      /* Copy the response values into Auth Tx (ASCII) for logging. */

      /* Comment this out so dcpiso doesn't use it to send to device.
       * We want dcpiso to send the raw response, no conversions.
       *
       * genutil_EMV_To_Auth_Tx( HOST_MESSAGE.iccdata, i, p_auth_tx );
       */
	}
   /* For ATMs, there must be a pin block
    *
    * Commented this out, because we found out during certification
    * that apparently, there can be an cash txn without a pin.
    *
   if ( atm && (0 == strlen( p_auth_tx->TLF01_details.pin_block ) ))
      strcpy( p_auth_tx->TLF01_details.pin_block, "DECLINE" );
    *
    */
   /* FIELD 56: ISO Future use */

   memset( tempstr, 0x00, sizeof(tempstr) );
   memcpy( tempstr, HOST_MESSAGE.ISOdata_len, bf56_len );

   F56_len = atoi(tempstr);

   memcpy( tempstr, HOST_MESSAGE.ISOdata, F56_len );

   //Store 29 bytes of data in 3 fields
   memcpy(p_auth_tx->TLF01_details.down_payment,tempstr+8,12);
   memcpy(p_auth_tx->TLF01_details.total_interest,tempstr+12,12);
   memcpy(p_auth_tx->TLF01_details.actual_amount,tempstr+24,5);

   /* FIELD 60: ADVICE REASON CODE */
   memset( F60_len,            0x00, sizeof( F60_len )            );
   memset( reject_advice_code, 0x00, sizeof( reject_advice_code ) );
   memset( reject_detail_code, 0x00, sizeof( reject_detail_code ) );
   memset( reject_text,        0x00, sizeof( reject_text )        );

   memcpy( F60_len, HOST_MESSAGE.advice_cd_len, 3 ); 
   iF60_len = atoi( F60_len );
   memcpy( reject_advice_code, HOST_MESSAGE.advice_code, 3 );
   strncpy( p_auth_tx->TLF01_details.product_codes[18].amount,
		   reject_advice_code,3);
   if ( 3 < iF60_len )
   {
      memcpy( reject_detail_code, &HOST_MESSAGE.advice_code[3], 4 );
      strncat( p_auth_tx->TLF01_details.product_codes[18].amount,
    		  reject_detail_code,3);
   }
   if ( 7 < iF60_len )
   {
      if ( iF60_len > sizeof(reject_text)-1 )
         iF60_len = sizeof(reject_text);
      memcpy( reject_text, &HOST_MESSAGE.advice_code[7], iF60_len-7 );
   }

   /* FIELD 61: POINT OF SERVICE DATA */
   if ( HOST_MESSAGE.pos_data[3] == '4' )
    p_auth_tx->MCF01_details.rps_merchant[0]='Y';
   memcpy(p_auth_tx->acceptor_term_name,HOST_MESSAGE.pos_data,sizeof(HOST_MESSAGE.pos_data));
   //strncpy(p_auth_tx->country_code,&HOST_MESSAGE.pos_data[13],3);

   //Identification of Mobile POS Acceptance Devices 
   if(HOST_MESSAGE.pos_data[9] == 'ID_MOBILE_POS_ACCP_DEVICES' )
   {
		strncpy(p_auth_tx->TLF01_details.product_codes[1].quantity,&HOST_MESSAGE.pos_data[9],1);
   }
	// Store feild 61 data to be display on Pos data Feild in Tranlookup ( 1st 11 bytes is important )
   	strncpy(p_auth_tx->TLF01_details.product_codes[16].amount,HOST_MESSAGE.pos_data,
   			(sizeof(p_auth_tx->TLF01_details.product_codes[16].amount)-1));
   	//Store Acq cntry code to display in tranlookup
   	strncpy(p_auth_tx->TLF01_details.product_codes[18].code, &HOST_MESSAGE.pos_data[13], 3 );

   /* FIELD 62: INTERMEDIATE NETWORK FACILITY (INF) DATA */
   if ( HOST_MESSAGE.inf_data_len[0] != 0x00 )
   {
      memset( itemp, 0x00, sizeof(itemp) );
      memcpy( itemp, HOST_MESSAGE.inf_data_len, 3 );
      i = atoi( itemp );
      memcpy( p_auth_tx->int_net_facility_data, HOST_MESSAGE.inf_data, i );
   }

   /* FIELD 63: BANKNET DATA */
	memcpy( p_auth_tx->TLF01_details.mcard_banknet_len,
			HOST_MESSAGE.banknet_len, sizeof( HOST_MESSAGE.banknet_len ) );
	memcpy( p_auth_tx->TLF01_details.mcard_banknet,
			&HOST_MESSAGE.banknet[0],
			atoi( p_auth_tx->TLF01_details.mcard_banknet_len ) );

   /* FIELD 70: NETWORK MANAGEMENT INFORMATION CODE */
   memcpy(Logon_NMI_Code,HOST_MESSAGE.nmi_code,sizeof(HOST_MESSAGE.nmi_code));

   /*FIELD 90 : ORIGINAL DATA elements*/
   if(strlen( HOST_MESSAGE.orig_data) > 0 && strlen( HOST_MESSAGE.orig_data)< 43 &&
		( strncmp( p_auth_tx->TLF01_details.message_type,"0400",3) == 0  ||
		  strncmp( p_auth_tx->TLF01_details.message_type,"0420",3) == 0
		 )
	 )
   {	/*Copy the orgianl data elements of DE90 and echo back*/
	   memcpy(p_auth_tx->TLF01_details.dcf01_retired_cwk,HOST_MESSAGE.orig_data,42);

   }
   /* FIELD 91: FILE UPDATE CODE */
   if ( HOST_MESSAGE.file_code[0] != 0x00 )
   {
      ams_choice = 0x00;
      switch( HOST_MESSAGE.file_code[0] )
      {
         case '1':  ams_choice = CHOICE_ADD;      break;
         case '2':  ams_choice = CHOICE_UPDATE;   break;
         case '3':  ams_choice = CHOICE_DELETE;   break;
         case '5':  ams_choice = CHOICE_INQUIRE;  break;
         default:   ams_choice = HOST_MESSAGE.file_code[0];  break;
      }
   }

   /* FIELD 100: RECEIVING INSTITUTION IDENTIFICATION CODE */
   memset( &recv_inst_len, 0x00, sizeof(recv_inst_len) );
   memset( &recv_inst,     0x00, sizeof(recv_inst    ) );

   memcpy( recv_inst_len, HOST_MESSAGE.recv_len, bf100_len );
   memcpy( recv_inst, HOST_MESSAGE.recvid, atoi(recv_inst_len) );

/* FIELD 101:*/

   /*FIELD 105 - Transaction Link Identifier*/
#if(defined MC01_APR2024 || defined MC02_APR2024)/*AN 7102 Introducing Mastercard Transaction Link Identifier (Effective Date: June 4, 2024) - Issuing*/
   if(HOST_MESSAGE.tranlink_len[0] != '\0')
   {
	   de48_retcode = parse_field_105( HOST_MESSAGE.tranlink_len,
	                                     HOST_MESSAGE.tranlink_data,
	                                     p_auth_tx );
   }

#endif

   /*FIELD 108:(Additional Transaction Reference Data) */

#ifdef MC07_OCT2023
   memset( tempstr, 0x00, sizeof( tempstr ) );
   if(HOST_MESSAGE.de108_len[0] != '\0')
   {
	   de48_retcode = parse_field_108( HOST_MESSAGE.de108_len,
	                                     HOST_MESSAGE.de108,
	                                     p_auth_tx );
   }

#endif

   /* FIELD 112- TF Phani- Installment data*/
   if(HOST_MESSAGE.addldata_nation_len[0] != 0x00 )
   {
   //sotre installement data in SOURCE_KEY

    memcpy(p_auth_tx->TLF01_details.source_key,HOST_MESSAGE.addldata_nation,DE112_TAG009_LEN);
   	
   }

   /* FIELD 119 - Addl Private2 Data */
   if (HOST_MESSAGE.private_use2_len[0] != '\0')
   {
	   /* store for Sbatch */
   }

   /* FIELD 120: RECORD DATA */
   if ( HOST_MESSAGE.orig_msg_id[0] != 0x00 )
   {
      /* This field is echoed for Adds/Updates/Deletes.
       * For Inquires of MCC102 messages, it also contains
       * Entry reason, date and time of last activity.
       *
       * For Inquires of MCC103 messages, it is an echo of
       * just the card number.  The additional information
       * will be in DE122.
       */
      memset( &Mcc_Gui, 0x00, Mcc_Gui_Size );

      /* Filename: MCC102 or MCC103 */
      memcpy( Mcc_Gui.msg_type, HOST_MESSAGE.filename, 6 );
	
      /* File Update Code: Add, Update, Delete, Inquire from DE91. */
      Mcc_Gui.choice = ams_choice;

	  	 /* File Update Code: Add, Update, Delete, Inquire from DE96- Security code. */
      memcpy( Mcc_Gui.err_msg3, HOST_MESSAGE.security_code, 8 );


      memcpy( Mcc_Gui.card_nbr, HOST_MESSAGE.orig_msg_id, 19 );
      i = 19;

      if ( 0 == strcmp(Mcc_Gui.msg_type, "MCC102") )
      {
         if( Mcc_Gui.choice != CHOICE_DELETE )
         {
            Mcc_Gui.reason = HOST_MESSAGE.orig_msg_id[i++];

            /* Entry Reason might not be here for inquiry. In this
             * case, it would be the first digit from the date. So
             * we should back off the index pointer, i.
             */
            if ( isdigit(Mcc_Gui.reason) )
               i--;

            /* Convert Activity Date from MMDDYY to DDMMYY. */
            memcpy( &Mcc_Gui.activity_date[2], &HOST_MESSAGE.orig_msg_id[i], 2);
            i += 2;
            memcpy( &Mcc_Gui.activity_date[0], &HOST_MESSAGE.orig_msg_id[i], 2);
            i += 2;
            memcpy( &Mcc_Gui.activity_date[4], &HOST_MESSAGE.orig_msg_id[i], 2);
            i += 2;

            memcpy( Mcc_Gui.activity_time, &HOST_MESSAGE.orig_msg_id[i], 4 );
            i += 4 + 2; /* Get past time and next 2 'unused' bytes. */

            if(Mcc_Gui.reason == 'V')
            {
               memcpy( Mcc_Gui.vip_limit_amt, &HOST_MESSAGE.orig_msg_id[i], 12);
               i += 12;
               memcpy(Mcc_Gui.vip_currency_code,&HOST_MESSAGE.orig_msg_id[i],3);
            }
         }
      }
      else
      {
         if ( Mcc_Gui.choice != CHOICE_INQUIRE )
         {
            /* Get past Issuing ICA. It's already in NCF01. */
            i += 6;

            if ( Mcc_Gui.choice == CHOICE_ADD )
            {
               /* Card Program */
               memcpy( Mcc_Gui.product_code, &HOST_MESSAGE.orig_msg_id[i], 3 );
               i += 5;  /* (Product Code) 3 + (Response Code) 2 = 5 */

               /* Entry Reason */
               Mcc_Gui.reason = HOST_MESSAGE.orig_msg_id[i];

               /* Get past filler. */
               i = 55;

               /* Check the 6 regions */
               for ( j=0; j<6; j++ )
               {
                  if ( HOST_MESSAGE.orig_msg_id[i] != 0x00 )
                  {
                     switch( HOST_MESSAGE.orig_msg_id[i] )
                     {
                        case '1': Mcc_Gui.region_us.indicator = '1';
                                  memcpy( Mcc_Gui.region_us.purge_date,
                                          &HOST_MESSAGE.orig_msg_id[i+1], 6 );
                                  break;

                        case 'A': Mcc_Gui.region_canada.indicator = 'A';
                                  memcpy( Mcc_Gui.region_canada.purge_date,
                                          &HOST_MESSAGE.orig_msg_id[i+1], 6 );
                                  break;

                        case 'B': Mcc_Gui.region_sa.indicator = 'B';
                                  memcpy( Mcc_Gui.region_sa.purge_date,
                                          &HOST_MESSAGE.orig_msg_id[i+1], 6 );
                                  break;

                        case 'C': Mcc_Gui.region_asia.indicator = 'C';
                                  memcpy( Mcc_Gui.region_asia.purge_date,
                                          &HOST_MESSAGE.orig_msg_id[i+1], 6 );
                                  break;

                        case 'D': Mcc_Gui.region_europe.indicator = 'D';
                                  memcpy( Mcc_Gui.region_europe.purge_date,
                                          &HOST_MESSAGE.orig_msg_id[i+1], 6 );
                                  break;

                        case 'E': Mcc_Gui.region_africa.indicator = 'E';
                                  memcpy( Mcc_Gui.region_africa.purge_date,
                                          &HOST_MESSAGE.orig_msg_id[i+1], 6 );
                                  break;

                        default:  j = 6;  break;
                     }
                     i += 7;
                  }
                  else
                  {
                     /* Break the FOR loop; no more regions. */
                     break;
                  }
               } /* end FOR loop */
            }
         }

         // Lets copy this filed for echo back to host in response in 0110 message type
         if( (0 == strncmp( p_auth_tx->TLF01_details.message_type,"0100",4))||
        		  (0 == strncmp( p_auth_tx->TLF01_details.message_type,"0200",4)))
         {
        	 memcpy(strfield_120_len, HOST_MESSAGE.orig_msg_len, sizeof(HOST_MESSAGE.orig_msg_len));
        	 field_120_len= atoi(strfield_120_len);
        	 if(field_120_len > 0)
        	 {

        		 memcpy( p_auth_tx->future_use_1,strfield_120_len, (sizeof( strfield_120_len)-1) );

        		 if(field_120_len<97)
        		 {
        			 memcpy( p_auth_tx->future_use_1 + 3 ,HOST_MESSAGE.orig_msg_id,field_120_len);
        		 }
        		 else if(field_120_len<196)
        		 {
        			 memcpy( p_auth_tx->future_use_1 + 3 , HOST_MESSAGE.orig_msg_id,
        					 (sizeof( p_auth_tx->future_use_1 ) - 4) );
        			 memcpy( p_auth_tx->future_use_2, HOST_MESSAGE.orig_msg_id + (sizeof( p_auth_tx->future_use_1 ) - 4) ,
        					 (field_120_len - (sizeof( p_auth_tx->future_use_1 ) - 4)) );
        		 }
        	 }
         }
      }
   }

   /* FIELD 121: AUTHORIZING AGENT IDENTIFICATION CODE */
   if (HOST_MESSAGE.mc_agent_len[0] != 0x00)
   {
   memcpy( p_auth_tx->TLF01_details.agent_id_len,
           HOST_MESSAGE.mc_agent_len, sizeof( HOST_MESSAGE.mc_agent_len ) );
   memcpy( p_auth_tx->TLF01_details.agent_id,
           HOST_MESSAGE.mc_agent_id, sizeof( HOST_MESSAGE.mc_agent_id ) );
   }

   /* FIELD 122: ADDITIONAL RECORD DATA - FOR FILE INQUIRIES */
   if ( AUTH_CARD_UPDATE_RESPONSE == p_auth_tx->TLF01_details.tx_key )
   {
   if ( HOST_MESSAGE.addlrec[0] != 0x00 )
   {
      /* This field contains results of an Inquire for MCC103 0302/0312 txns.
       * It contains up to 6 records in format RYYMMDDyymmdd where
       *    R  = region indicator
       *    YY = Chargeback Effective Year
       *    MM = Chargeback Effective Month
       *    DD = Chargeback Effective Day
       *    yy = Purge date year
       *    mm = Purge date month
       *    dd = Purge date day
       */
      for ( j=0,i=0; j<6; j++ )
      {
         if ( HOST_MESSAGE.addlrec[i] != 0x00 )
         {
            switch( HOST_MESSAGE.addlrec[i] )
            {
               case '1': Mcc_Gui.region_us.indicator = '1';
                         memcpy( Mcc_Gui.region_us.chargeback_date,
                                &HOST_MESSAGE.addlrec[i+1], 6 );
                         memcpy( Mcc_Gui.region_us.purge_date,
                                &HOST_MESSAGE.addlrec[i+7], 6 );
                         break;

               case 'A': Mcc_Gui.region_canada.indicator = 'A';
                         memcpy( Mcc_Gui.region_canada.chargeback_date,
                                &HOST_MESSAGE.addlrec[i+1], 6 );
                         memcpy( Mcc_Gui.region_canada.purge_date,
                                &HOST_MESSAGE.addlrec[i+7], 6 );
                         break;

               case 'B': Mcc_Gui.region_sa.indicator = 'B';
                         memcpy( Mcc_Gui.region_sa.chargeback_date,
                                &HOST_MESSAGE.addlrec[i+1], 6 );
                         memcpy( Mcc_Gui.region_sa.purge_date,
                                &HOST_MESSAGE.addlrec[i+7], 6 );
                         break;

               case 'C': Mcc_Gui.region_asia.indicator = 'C';
                         memcpy( Mcc_Gui.region_asia.chargeback_date,
                                &HOST_MESSAGE.addlrec[i+1], 6 );
                         memcpy( Mcc_Gui.region_asia.purge_date,
                                &HOST_MESSAGE.addlrec[i+7], 6 );
                         break;

               case 'D': Mcc_Gui.region_europe.indicator = 'D';
                         memcpy( Mcc_Gui.region_europe.chargeback_date,
                                &HOST_MESSAGE.addlrec[i+1], 6 );
                         memcpy( Mcc_Gui.region_europe.purge_date,
                                &HOST_MESSAGE.addlrec[i+7], 6 );
                         break;

               case 'E': Mcc_Gui.region_africa.indicator = 'E';
                         memcpy( Mcc_Gui.region_africa.chargeback_date,
                                &HOST_MESSAGE.addlrec[i+1], 6 );
                         memcpy( Mcc_Gui.region_africa.purge_date,
                                &HOST_MESSAGE.addlrec[i+7], 6 );
                         break;

               default:  j = 6;  break;
            }
            i += 13;
         }
         else
         {
            /* Break the FOR loop; no more regions. */
            break;
         }
      }
   }
   }

   /* FIELD 124: ADDITIONAL RECORD DATA - FOR FILE INQUIRIES */
	if(HOST_MESSAGE.memData[0] != 0x00)
	{
		strncpy(asci_de124_len,HOST_MESSAGE.memData_len,3);
		de124_len = atoi(asci_de124_len);
		if(de124_len > 0 && de124_len < sizeof(p_auth_tx->TLF01_details.source_key) - 1)
		{
			memcpy(p_auth_tx->TLF01_details.source_key,HOST_MESSAGE.memData,de124_len);
		}
		else
		{
			memset(Log_Bufr,0x00,sizeof(Log_Bufr));
			sprintf(Log_Bufr,"DE124 from MC is more than 48, hence not stored in TLF01, Term Id:%s ,Merch Id: %s RRN: %s ",
			    		  p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
			ncmcrd_log_message( 3, 2,Log_Bufr , "move_iso_response_to_auth_struct" , 0 ,p_auth_tx->TLF01_details.card_num);
		}
	}
	
   if ( ret_val != OK )
   {
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
    		  p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      memcpy( p_auth_tx->TLF01_details.response_text, errbuf,
                    sizeof(p_auth_tx->TLF01_details.response_text)-1 );
      strcat(errbuf,Log_Bufr);
      ncmcrd_log_message( 3, 2, errbuf , "move_iso_response_to_auth_struct" , 0 ,p_auth_tx->TLF01_details.card_num);
   }

   if(bitfiled_error > 0)
   {
 	  // Its format error. Lets store the error bitfield
 	  ret_val = bitfiled_error;
   }

   return( ret_val );
}  /* End move_iso_response_to_auth_struct */


/******************************************************************************
 *
 *  NAME:         BUILD_HOST_STRUCTURE
 *
 *  DESCRIPTION:  This procedure uses the data in the auth_tx entry to build
 *                the HOST MESSAGE structure that will be required to build
 *                the MasterCard message for the host.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void build_host_structure( pAUTH_TX p_auth_tx )
{
   #define RFID_DEVICE '8'

   INT     itemp  = 0;
   INT    src_len = 0;
   INT     va_len = 0;
   INT     echo_tags = 0;
   INT     iIndustry = 0;
   INT     time_date_exists = false;
   CHAR    temp_str[130]={0};
   CHAR    temp_str3[4] = {0};
   CHAR    gmt_time[20] = {0};
   CHAR    POS_string[5] = {0};
   CHAR    time_date[25] = {0};
   BOOLEAN e_commerce = 0;
   BOOLEAN emv_txn = 0;
   BOOLEAN rfid =0 ;
   CHAR    VbV_data[150]={0x00};
   BYTE    merch_serial_numbers[41] = {0};
   BYTE    security_level_ind[8] = {0};
   BYTE    ucaf_data[33] = {0};
   BYTE    cvc2_data[8] = {0};
   INT     len = 0;
   INT     ecom_total_len = 0 ;
   INT     ecom_sub_len = 0;
   INT     ecom_idx = 0;
   INT     serial_nbrs_len = 0;
   INT     sec_lev_ind_len = 0;
   INT     ucaf_data_len = 0;
   BYTE    ucaf_indicator = 0x00;
   CHAR    ecom_substr[5] = {0};
   CHAR    bcd_ecom_substr[3] = {0};
   INT     emv_len = 0;
   CHAR    orig_data[43] = {0};
   CHAR    def_length[2] = {0};
	BYTE 		temp_error_bit_nun[4]= {0};
	BYTE 		temp_error_bit_info [7] = {0};
	int 		error_bit = 0;
	CHAR  		tempstr[512] = {0};
   BYTE strfield_120_len[4]={0};
   int field_120_len =0;
   char *mac_value=NULL;
	  char TAG48_len[3]={0};
	  int len_tag48=0;
	  int DE108_len=0;
#if BDOR_62_PREAUTH_VOID
   BYTE	   replace_amt[43] = {0};
   long	   remain_amt	   = 0;
#endif

   /* Initialize the host structure before filling it. */
   memset( &HOST_MESSAGE, 0x00, sizeof(HOST_MESSAGE) );

   if ( AUTH_INVALID < p_auth_tx->TLF01_details.tx_key )
   {
      /* This is a response to a request from the host. */
      Tran_Flow = SWITCHED_IN;
   }
   else
   {
      /* This is a request going to the host. */
      Tran_Flow = SWITCHED_OUT;
   }

   iIndustry = atoi( p_auth_tx->industry_code );

   /* Some fields need to know if this txn is an e-commerce txn. */
   if( true == ncmcrd_Is_AsiaPay_Transaction(p_auth_tx))
   {
      e_commerce = true;
   }
   else
      e_commerce = false;

   /* Determine if this is an EMV (chip) transaction. */
   emv_len = genutil_bcd_to_int( p_auth_tx->EMV_details.emv_block, 2 );
   if ( emv_len > 0 )
      emv_txn = true;
   else
      emv_txn = false;


   /*+--------------------------------------------------------------+
     | FIELD 2: PAN  -  Right justify the length portion of the PAN |
     | ------------------------------------------------------------ |
     | Use ICA for log on/off messages if no PAN exists.            |
     +--------------------------------------------------------------+*/
   if ( 0 == strcmp( NULL_STR, p_auth_tx->TLF01_details.card_num ) )
   {
      if ( 0 == strcmp( Logon_NMI_Code, LOGON_NMI_GROUP_ID     )  ||
           0 == strcmp( Logon_NMI_Code, LOGON_NMI_GROUP_LOGOFF )  ||
           0 == strcmp( Logon_NMI_Code, LOGON_SAF_REQUEST      )  ||
           0 == strcmp( Logon_NMI_Code, ECHO_NMI               )  )
      {
         /* It is a Log ON/OFF by Group Id */
         if ( 0 != strcmp( NULL_STR, ncf01_i.forwarding_inst_id_code ) )
         {
            /* Only use the 5 right-most digits.  MC is using what they
             * call extended ICA.  This means the forward and acquire
             * institution IDs can be 6 digits.  But for logons by group
             * this field must only be 5 characters.
             */
			 /* TF Phani..copy 6 digits from forward and acquire
             * institution IDs */
            itemp = strlen( ncf01_i.forwarding_inst_id_code );
            if ( itemp < LOG_ON_GROUP_ID_PAN_LEN )
               itemp = LOG_ON_GROUP_ID_PAN_LEN;

            strcpy( temp_str, &ncf01_i.forwarding_inst_id_code[itemp-LOG_ON_GROUP_ID_PAN_LEN] );
         }
         else if(0 != strcmp( NULL_STR, ncf01_i.acquirer_inst_id_code ))
         {
            /* No Forwarding Inst. Id code.  Use the Acquiring Inst. Id code. */
            itemp = strlen( ncf01_i.acquirer_inst_id_code );
            if ( itemp < LOG_ON_GROUP_ID_PAN_LEN )
               itemp = LOG_ON_GROUP_ID_PAN_LEN;

            strcpy( temp_str, &ncf01_i.acquirer_inst_id_code[itemp-LOG_ON_GROUP_ID_PAN_LEN] );
         }
		 else // TF Phani ..For sign on DE 2should be "001788"..req by Kenneth
		 {
			 itemp = LOG_ON_GROUP_ID_PAN_LEN;
			 strcpy( temp_str, "01788" );

		 }
         itemp = strlen( temp_str );
         sprintf( HOST_MESSAGE.pan_len, "%02d", itemp );
         memcpy( HOST_MESSAGE.pan, temp_str, LOG_ON_GROUP_ID_PAN_LEN );
      }
   }
   else
   {
      itemp = strlen( p_auth_tx->TLF01_details.card_num );
      if ( itemp > 0 )
      {
         sprintf( HOST_MESSAGE.pan_len, "%02d", itemp );
         strcpy( HOST_MESSAGE.pan, p_auth_tx->TLF01_details.card_num );
      }
   }

   /* FIELD 3: PROCESSING CODE */
   if ( SWITCHED_IN == Tran_Flow )
   {
      /* This is a response to a request from the host.  Echo the orig value. */
      memcpy( HOST_MESSAGE.proccode, p_auth_tx->TLF01_details.processing_code,
              bf3_len );
   }

   /* This is a request for the host.  Create the processing code. */
   else if ( p_auth_tx->TLF01_details.processing_code[0] == 0x00 )
   {
      strcpy( p_auth_tx->TLF01_details.processing_code, "000000" );
      memcpy( HOST_MESSAGE.proccode, "000000", 6 );
   }
   else if ( p_auth_tx->TLF01_details.tx_key == AUTH_CARD_VERIFICATION )
   {
      /* send a special processing code for CARD_VERIFICATION message */
	   memcpy(HOST_MESSAGE.proccode, "000000",6);
   }
   else
   {
      memcpy( HOST_MESSAGE.proccode,p_auth_tx->TLF01_details.processing_code,6);

      if ( p_auth_tx->TLF01_details.tx_key == AUTH_CASH_ADVANCE_AUTHORIZATION )
      {
         /* Cash Advance or Quasi-cash from Voice */
         memcpy( HOST_MESSAGE.proccode, "17", 2 );
      }
      else if ( p_auth_tx->TLF01_details.tx_key == AUTH_VOID_SALE ||
    		  strncmp(p_auth_tx->TLF01_details.processing_code , "02" ,2) == 0)
      {
         /* Void - send the 00 */
         memcpy( HOST_MESSAGE.proccode, "00", 2 );
      }
      else if ( p_auth_tx->TLF01_details.tx_key == AUTH_BALANCE_INQUIRY )
      {
         memcpy( HOST_MESSAGE.proccode, BALANCE_INQUIRY, 2 );
      }
	  else if (AUTH_QUASI_CASH == p_auth_tx->TLF01_details.tx_key) 
      {
         memcpy(HOST_MESSAGE.proccode, "00", 2);
	  }
	  else if ( strncmp(p_auth_tx->TLF01_details.processing_code , "11" ,2) == 0)
	  {
		  memcpy(HOST_MESSAGE.proccode, "00", 2);
	  }else if ( strncmp(p_auth_tx->TLF01_details.processing_code , "30" ,2) == 0 ||
			  strncmp(p_auth_tx->TLF01_details.processing_code , "38" ,2) == 0 ||
			  strncmp(p_auth_tx->TLF01_details.processing_code , "03" ,2) == 0
#if BDOR_62_PREAUTH_VOID
			  || strncmp(p_auth_tx->TLF01_details.processing_code , "02" ,2) == 0
#endif
			  )
	  {
		  memcpy(HOST_MESSAGE.proccode, "00", 2);
	  }

   }
   /* In order to show proceesing code in transaction lookup page( OCT-2017 Mandated changes) */
   memcpy(p_auth_tx->TLF01_details.product_codes[17].amount,HOST_MESSAGE.proccode,sizeof(HOST_MESSAGE.proccode));

   /* FIELD 4: TRANSACTION AMOUNT */
   if ( (p_auth_tx->TLF01_details.tx_key == AUTH_BALANCE_INQUIRY_RESPONSE) ||
        (p_auth_tx->TLF01_details.tx_key == AUTH_BALANCE_INQUIRY) )
   {
      memset( p_auth_tx->TLF01_details.total_amount, '0', bf4_len );
   }

   if(0 == strncmp(p_auth_tx->TLF01_details.product_codes[3].quantity,"PR",2))
   {
	   memcpy( HOST_MESSAGE.amount,            /* Its Partial reversal */
			   p_auth_tx->TLF01_details.product_codes[19].amount,
			   bf4_len );
   }
   else
   {
	   memcpy( HOST_MESSAGE.amount,            /* See also field 49 */
			   p_auth_tx->TLF01_details.total_amount,
			   bf4_len );
   }
#if BDOR_62_PREAUTH_VOID
   if (p_auth_tx->TLF01_details.tx_key == AUTH_VOID_PRE_AUTHORIZATION)
   {
	  if (p_auth_tx->TLF01_details.total_interest[0] != '\0')
	  {
	 	  memcpy( HOST_MESSAGE.amount,
	 	             p_auth_tx->TLF01_details.total_interest,
	 	             bf4_len );
	  }
   }
#endif
   /* FIELD 5: SETTLEMENT AMOUNT */
   memcpy( HOST_MESSAGE.settle_amt,
           p_auth_tx->TLF01_details.settlement_total,
           bf5_len );

   /* FIELD 6: CARDHOLDER BILLING AMOUNT */
   memcpy( HOST_MESSAGE.ch_bill_amt, p_auth_tx->ch_billing_amt, bf6_len );

   /* FIELD 7: TRANSMISSION DATE & TIME */
   if( 0  == strncmp(p_auth_tx->TLF01_details.product_codes[3].quantity,"PR",2) &&
	 '\0' != p_auth_tx->orig_local_time_date[0]	   )
   {
	   memcpy( HOST_MESSAGE.gmt_time,
			   p_auth_tx->orig_local_time_date,
			   bf7_len );
   }
   else if ( 0 == strcmp( NULL_STR, p_auth_tx->TLF01_details.transmission_timestamp ))
   {
	  pintime_get_gmt( gmt_time );  /* YYYYMMDDhhmmss */
	  strncpy( p_auth_tx->TLF01_details.transmission_timestamp,
			   &gmt_time[4], 10 ); /* MMDDhhmmss */

	  memcpy( HOST_MESSAGE.gmt_time,
			  p_auth_tx->TLF01_details.transmission_timestamp,
			  bf7_len );
   }
   else
   {
   memcpy( HOST_MESSAGE.gmt_time,
		   p_auth_tx->TLF01_details.transmission_timestamp,
		   bf7_len );
   }

   /* FIELD 9: SETTLEMENT CONVERSION RATE */
   memcpy( HOST_MESSAGE.conv_rate_stl,
           p_auth_tx->TLF01_details.conversion_rate,
           bf9_len );

   memcpy(HOST_MESSAGE.conv_rate_stl,p_auth_tx->settlement_conv_rate,bf9_len);

   /* If there was originally no conversion rate, the authorizer
    * fills in this field with '1' followed by NULLs.  I need to
    * convert this '1' to MasterCard's format of '10000010'.  The
    * left most '1' indicates the number of positions the decimal 
    * separator should be moved from the right.
    */
   if ( 0 == strncmp( "1", HOST_MESSAGE.conv_rate_stl, 1 ) )
      memcpy( HOST_MESSAGE.conv_rate_stl, "10000010", bf9_len );

   /* FIELD 10: CARDHOLDER BILLING CONVERSION RATE */
   memcpy( HOST_MESSAGE.conv_rate_chb,
           p_auth_tx->ch_billing_conv_rate,
           bf10_len );

   /* FIELD 11: SYSTEMS TRACE AUDIT NUMBER */
   if ( AUTH_LOGON          == p_auth_tx->TLF01_details.tx_key ||
        AUTH_LOGON_RESPONSE == p_auth_tx->TLF01_details.tx_key )
      memcpy( HOST_MESSAGE.stan, Stan, bf11_len );
   else
   {
      memcpy( HOST_MESSAGE.stan, p_auth_tx->TLF01_details.sys_trace_audit_num,
              bf11_len );
      if((HOST_MESSAGE.stan[0]== '\0') || (HOST_MESSAGE.stan[0] == ' '))
      {
         /* Create a STAN unique for this day; use the local time. */
         ptetime_get_timestamp( time_date );
         time_date_exists = true;
         get_time( time_date, p_auth_tx->TLF01_details.sys_trace_audit_num );
         memcpy( HOST_MESSAGE.stan,
                 p_auth_tx->TLF01_details.sys_trace_audit_num, bf11_len );
      }
   }

   /* FIELD 12: TRANSACTION TIME */
   if ( SWITCHED_OUT == Tran_Flow )
   {
	   if( p_auth_tx->TLF01_details.tx_key == AUTH_REVERSAL_ADVICE 			||
		   p_auth_tx->TLF01_details.tx_key == AUTH_REVERSAL		   			||
		   p_auth_tx->TLF01_details.tx_key == AUTH_REVERSAL_ADVICE_REPEAT	||
		   p_auth_tx->TLF01_details.tx_key == AUTH_REVERSAL_REPEAT 			||
		   p_auth_tx->TLF01_details.tx_key == AUTH_VOID_SALE)
	   {
		   if(true==ncmcrd_check_for_acq_refund_transaction(p_auth_tx))
		   {
			   //The time_hhmmss field becomes empty for acquiring reversal transaction. hence dont copy anything.
		   }
		   else
		   {
		   memcpy(p_auth_tx->TLF01_details.time_hhmmss, p_auth_tx->local_date_time+6, bf12_len);
	   }
	   }
	   else if ( p_auth_tx->TLF01_details.time_hhmmss[0] == 0x00 )
       {
         /* No time yet.  Need to create. */
         if ( time_date_exists == false )
         {
            ptetime_get_timestamp( time_date );
            time_date_exists = true;
         }
         get_time( time_date, p_auth_tx->TLF01_details.time_hhmmss );
      }
	  memcpy(HOST_MESSAGE.time, p_auth_tx->TLF01_details.time_hhmmss, bf12_len);

      /* FIELD 13: TRANSACTION DATE */
      if ( p_auth_tx->TLF01_details.date_yyyymmdd[0] == 0x00 )
      {
         /* No date yet.  Need to create. */
         if ( time_date_exists == false )
         {
            ptetime_get_timestamp( time_date );
            time_date_exists = true;
         }
         get_time( time_date, p_auth_tx->TLF01_details.date_yyyymmdd );
      }
      memcpy(HOST_MESSAGE.date, p_auth_tx->TLF01_details.date_yyyymmdd+4, bf13_len);
   }

   /* FIELD 14: EXPIRATION DATE */
   if ( 0 != strcmp( NULL_STR, p_auth_tx->TLF01_details.exp_date ) )
      memcpy( HOST_MESSAGE.date_expd,
              p_auth_tx->TLF01_details.exp_date,
              bf14_len );

   /* FIELD 15: SETTLEMENT DATE */
   if ( 0 != strcmp( NULL_STR, p_auth_tx->TLF01_details.settlement_date ) )
      memcpy( HOST_MESSAGE.date_settle,
              p_auth_tx->TLF01_details.settlement_date,
              bf15_len );

   /* FIELD 16: CONVERSION DATE */
   if ( 0 != strcmp( NULL_STR, p_auth_tx->conv_date ) )
      memcpy( HOST_MESSAGE.conv_date, p_auth_tx->conv_date, bf16_len );

   /* FIELD 18: MERCHANT TYPE / ACQUIRER SIC NUMBER */
   if ( p_auth_tx->MCF01_details.voice_category_code[0] != 0x00 )
   {
      memcpy( HOST_MESSAGE.sic_number,
              p_auth_tx->MCF01_details.voice_category_code,
              bf18_len );
   }
   else
   {
      memcpy( HOST_MESSAGE.sic_number,
              p_auth_tx->TLF01_details.category_code,
              bf18_len );
   }

   /* FIELD 20: PAN COUNTRY CODE */
   memcpy( HOST_MESSAGE.pan_ctry_cd, p_auth_tx->country_code, bf20_len );


   /*+--------------------------------------------------------------+
     | FIELD 22: POINT OF SERVICE ENTRY MODE                        |
     +--------------------------------------------------------------+
     | HYPERCOM (0nnn)   MCARD (nnn)   Desc.                        |
     | ---------------   -----------   --------------------         |
     |    000n             00n         Unknown or unspecified       |
     |    001n             01n         Manual                       |
     |    002n             90n         Magnetic Stripe              |
     |    005n             05n         EMV                          |
     |    007n             07n         Contactless Chip             |
     |    080n             80n         Mag Stripe, but chip capable |
     |                     81n         E-commerce, including EMV    |
     |    091n             91n         Contactless Mag Stripe       |
     | Pin Entry Mode                                               |
     |    0nn1             nn1         Pin entry capability         |
     |    0nn2             nn2         No Pin entry capability      |
     +--------------------------------------------------------------+
   */

   /* Check for POS Entry Mode = RFID (Contactless) */
   if ( 0 == strncmp("091", p_auth_tx->TLF01_details.pos_entry_mode, 3) )
   {
      rfid = true;
   }
   else if ((0 == strncmp("007", p_auth_tx->TLF01_details.pos_entry_mode, 3)) &&
            (p_auth_tx->TLF01_details.interest_rate[0] == 0x00) )
   {
      /* Value 07x indicates contactless chip card, but Equitable also uses it
       * for Amex manual entry with 4DBC.  The 4DBC value is stored in the
       * interest rate field.  So if this field is populated, then the entry
       * was  manual. But if empty, and DE22 is 07x, then contactless.  Need
       * to make this check in case an Amex card gets routed through ncmcrd.
       */
      rfid = true;
   }
   else
   {
      rfid = false;
   }

   strcpy( POS_string, p_auth_tx->TLF01_details.pos_entry_mode );
   if ( e_commerce )
   {
      /* Use this even if e-commerce txn contains chip data. */
      memcpy( HOST_MESSAGE.posentry, "81", 2 );

      if ( POS_string[3] == '0' )
         HOST_MESSAGE.posentry[2] = '2'; /* Default to no pin capability */
      else
         HOST_MESSAGE.posentry[2] = POS_string[3];
   }
   else if ( emv_txn )
   {
      /* Pass the POS Entry Mode unaltered to MC. */
      memcpy( HOST_MESSAGE.posentry, POS_string+1, 3 );
   }
   else if ( p_auth_tx->TLF01_details.pos_entry_mode[0] != 0x00 )
   {
      /* Checking for a fallback txn.  Not sure which position
       * of Field 22 to check for the 8.  Spec says [1], but
       * Equitable is seeing [2].
       */
      if ( (POS_string[2] == '8') || (POS_string[1] == '8') )
      {
         /* This is an EMV Fallback -> Magstripe, but chip capable. */
         memcpy( HOST_MESSAGE.posentry, "80", 2 );
      }
      else if (0 == strncmp("001",p_auth_tx->TLF01_details.pos_entry_mode,3) &&
      		    0 == strncmp("08",p_auth_tx->TLF01_details.pos_condition_code,2))
       {	/*TF Raghu - APR 2018 MC Mandated changes: RPS Case : 001x to 10xx change */
               strncpy( HOST_MESSAGE.posentry, "10",2);
        }
      else if ( POS_string[2] == '2' )
      {
         /* For Mag Stripe Swipe, MC needs to see 90 in 1st two positions. */
         memcpy( HOST_MESSAGE.posentry, "90", 2 );
      }
      else
      {
         /* Default to what the terminal sent. */
         memcpy( HOST_MESSAGE.posentry, POS_string+1, 2 );
      }

      /* DE22.2 - Pin Entry Mode */
      HOST_MESSAGE.posentry[2] = POS_string[3];
   }
   else
   {
      /* Terminal did not send POS Entry Mode */
      if ( (p_auth_tx->TLF01_details.track2[0] != 0x00) ||
           (p_auth_tx->TLF01_details.track1[0] != 0x00)  )
      {
         /* Track exists, must be card swipe. */
         memcpy( HOST_MESSAGE.posentry, "900", 3 );
      }
      else
      {
         /* Manual entry, pin capability is unknown. */
         memcpy( HOST_MESSAGE.posentry, "010", 3 );
      }
   }

#if BDOR_68_REQ_1_MC   /* MobilePOS-NFC Terminal for Tap2Phone Transactions */
   if(true == ncmcrd_check_if_trx_is_mc_MobilePOS_NFC_Tap2Phone_trx_with_pin(p_auth_tx))
   {
	   strncpy( HOST_MESSAGE.posentry,
			    MC_MOBILE_NFC_TAP2PHONE_TXN_WITH_PIN,
				MC_DE22_PEM_LEN );
   }
   else if(true == ncmcrd_check_if_trx_is_mc_MobilePOS_NFC_Tap2Phone_trx_without_pin(p_auth_tx))
   {
	   strncpy( HOST_MESSAGE.posentry,
			    MC_MOBILE_NFC_TAP2PHONE_TXN_WITHOUT_PIN,
				MC_DE22_PEM_LEN );
   }
#endif

#if BDOR_68_REQ_3_MC   /* Swiftpass BDO Pay Credit */
   else if(true == ncmcrd_check_if_trx_is_mc_Swiftpass_BDO_Pay_credit_trx_with_pin(p_auth_tx))
   {
	   strncpy( HOST_MESSAGE.posentry,
			    MC_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITH_PIN_PEM,
				MC_DE22_PEM_LEN );
   }
   else if(true == ncmcrd_check_if_trx_is_mc_Swiftpass_BDO_Pay_credit_trx_without_pin(p_auth_tx))
   {
	   strncpy( HOST_MESSAGE.posentry,
			    MC_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITHOUT_PIN_PEM,
				MC_DE22_PEM_LEN );
   }
#endif

#if BDOR_68_REQ_5_MC /*Ecommerce Entry Mode From Swiftpass BDOPay Transactions*/

    else if(true == ncmcrd_check_if_trx_is_mc_ecommerce_swiftpass_BDOPay_trx (p_auth_tx))
    {
	   strncpy( HOST_MESSAGE.posentry,
			    MC_ECOMMERCE_SWIFTPASS_BDOPAY_TXN_PIN_PEM,
				MC_DE22_PEM_LEN );
   }
#endif

   memcpy(p_auth_tx->TLF01_details.product_codes[9].code,HOST_MESSAGE.posentry,sizeof(HOST_MESSAGE.posentry));

   /* FIELD 23 -- Card Sequence Number for EMV TAG 5F34 */
   if (p_auth_tx->EMV_details.pan_sequence_number[0] != 0x00)
   {
      itemp = atoi( p_auth_tx->EMV_details.pan_sequence_number );
      sprintf( temp_str3, "%03d", itemp );
      memcpy( HOST_MESSAGE.card_seq_num, temp_str3, 3 );
   }
   else if ( p_auth_tx->host2_message_seq_nbr[0] != 0x00 )
   {
      /* Card Seq. num came in field 23, but not in DE55. */
      itemp = atoi( p_auth_tx->host2_message_seq_nbr );
      sprintf( temp_str3, "%03d", itemp );
      memcpy( HOST_MESSAGE.card_seq_num, temp_str3, 3 );
   }


   /* FIELD 32: ACQUIRING INSTITUTION IDENTIFICATION CODE */
   if ( SWITCHED_IN == Tran_Flow )
   {
      /* This is a response to a request from the host.  Echo the orig value. */
      itemp = strlen( p_auth_tx->TLF01_details.acquiring_id );
      if ( itemp > 0 )
      {
         sprintf( HOST_MESSAGE.acq_inst_len, "%02d", itemp );
         memcpy( HOST_MESSAGE.acq_inst_id,p_auth_tx->TLF01_details.acquiring_id,
                 sizeof(HOST_MESSAGE.acq_inst_id) );
	  }
   }
   else
   {
      /* This is a request for the host.  Use ID in NCF01. */
      itemp = strlen( ncf01_i.acquirer_inst_id_code );
      if ( itemp > 0 )
      {
         sprintf( HOST_MESSAGE.acq_inst_len, "%02d", itemp );
         memcpy( HOST_MESSAGE.acq_inst_id, ncf01_i.acquirer_inst_id_code,
                 sizeof(HOST_MESSAGE.acq_inst_id) );

         /* Store acquiring id for TLF01. */
         if ( p_auth_tx->TLF01_details.acquiring_id[0] == 0x00 )
            strcpy( p_auth_tx->TLF01_details.acquiring_id,
                    HOST_MESSAGE.acq_inst_id );
      }
   }

   /* FIELD 33: FORWARDING INSTITUTION IDENTIFICATION CODE */
   if ( SWITCHED_IN == Tran_Flow )
   {
      /* This is a response to a request from the host.  Echo the orig value. */
      itemp = strlen( p_auth_tx->TLF01_details.forwarding_institution_id );
      if ( itemp > 0 )
      {
         sprintf( HOST_MESSAGE.fwd_inst_len, "%02d", itemp );
         strncpy( HOST_MESSAGE.fwd_inst_id,
                  p_auth_tx->TLF01_details.forwarding_institution_id,
                  itemp );
      }
   }
   else if ( 0 != strcmp( NULL_STR, ncf01_i.forwarding_inst_id_code ) )
   {
      itemp = strlen( ncf01_i.forwarding_inst_id_code );
      sprintf( HOST_MESSAGE.fwd_inst_len, "%02d", itemp );
      memcpy( HOST_MESSAGE.fwd_inst_id, ncf01_i.forwarding_inst_id_code,
              sizeof(HOST_MESSAGE.fwd_inst_id));

      /* Store here for TLF01. */
      if ( p_auth_tx->TLF01_details.forwarding_institution_id[0] == 0x00 )
         strcpy( p_auth_tx->TLF01_details.forwarding_institution_id,
                 HOST_MESSAGE.fwd_inst_id );
	  else if( AUTH_CARD_UPDATE  == p_auth_tx->TLF01_details.tx_key)
	  {
		  itemp = strlen( p_auth_tx->TLF01_details.forwarding_institution_id );
		  if ( itemp > 0 )
		  {
			 sprintf( HOST_MESSAGE.fwd_inst_len, "%02d", itemp );
			 strncpy( HOST_MESSAGE.fwd_inst_id,
					  p_auth_tx->TLF01_details.forwarding_institution_id,
					  itemp );
		  }

	  }
   }
   else
   {
      /* No Forwarding Inst. Id code.  Use the Acquiring Inst. Id code. */
      strncpy( HOST_MESSAGE.fwd_inst_len, HOST_MESSAGE.acq_inst_len, bf33_len );
      strcpy(  HOST_MESSAGE.fwd_inst_id,  HOST_MESSAGE.acq_inst_id            );
   }
   if ( 0 != strncmp( NULL_STR, HOST_MESSAGE.fwd_inst_len, bf33_len ) )
      Rj_with_lead_zeros( HOST_MESSAGE.fwd_inst_len, bf33_len );

   /* FIELD 35: TRACK 2 */
   if ( 0 != strcmp( NULL_STR, p_auth_tx->TLF01_details.track2 ) )
   {
      itemp = strlen( p_auth_tx->TLF01_details.track2 );
      sprintf( HOST_MESSAGE.track2_len, "%02d", itemp );
      strcpy( HOST_MESSAGE.track2, p_auth_tx->TLF01_details.track2 );
   }

   /* FIELD 37: RETRIEVAL REFERENCE NUMBER */
   
   if ( SWITCHED_IN == Tran_Flow )
   {
      /* This is a response to a request from the host.  Echo the orig value. */
  	   if((0 == strncmp(p_auth_tx->TLF01_details.message_type,"0100",4)) ||
  		  (0 == strncmp(p_auth_tx->TLF01_details.message_type,"0110",4)))
  	   {
  		   if ( p_auth_tx->amex_pos_entry_mode[0] == '1' )
  		   {	/* IF flag is 1 means, No RRN received in Request*/
  			   memset(HOST_MESSAGE.rrn, 0x00, sizeof(HOST_MESSAGE.rrn));
  		   }
  		   else
  		   { 	memcpy( HOST_MESSAGE.rrn,
                p_auth_tx->TLF01_details.retrieval_ref_num,
                bf37_len );
  		   }
  	   }
  	   else
  	   {
      memcpy( HOST_MESSAGE.rrn,
              p_auth_tx->TLF01_details.retrieval_ref_num,
              bf37_len );

  	   }
   }
   /* Copy original RRN for reversal and void sale txn  (09-76) Nov 11,2009 TF-Ajay*/
   else if ( (AUTH_REVERSAL == p_auth_tx->TLF01_details.tx_key)||
	         (AUTH_VOID_SALE== p_auth_tx->TLF01_details.tx_key) )
      memcpy( HOST_MESSAGE.rrn,
              p_auth_tx->TLF01_details.orig_retrieval_ref_num,
              bf37_len );
   else if ( 0 != strcmp( NULL_STR, p_auth_tx->TLF01_details.retrieval_ref_num))
      memcpy( HOST_MESSAGE.rrn,
              p_auth_tx->TLF01_details.retrieval_ref_num,
              bf37_len );
   else
      memcpy( HOST_MESSAGE.rrn,
              p_auth_tx->TLF01_details.orig_retrieval_ref_num,
              bf37_len );

   /* FIELD 38: AUTHORIZATION IDENTIFICATION RESPONSE */

   /* Send DE38 only if switched in txn is approved. */
   if ( SWITCHED_IN == Tran_Flow )
   {
      if ( 0   == strcmp( APPROVED, p_auth_tx->TLF01_details.response_code ) ||
    	  true == ncmcrd_Is_Refund_Transaction (p_auth_tx))
      {
         memcpy( HOST_MESSAGE.authcode, p_auth_tx->TLF01_details.auth_number,
                 sizeof(HOST_MESSAGE.authcode) );
   }
   }
   /* 09-76 Void and reversal txn for MC 31 Oct 2009 TF-Ajay */
   else if ( (AUTH_REVERSAL == p_auth_tx->TLF01_details.tx_key)||
	         (AUTH_VOID_SALE== p_auth_tx->TLF01_details.tx_key)||
			 (AUTH_VOID_PRE_AUTHORIZATION == p_auth_tx->TLF01_details.tx_key))
   {
      memcpy( HOST_MESSAGE.authcode,
              p_auth_tx->TLF01_details.auth_number,
              sizeof(HOST_MESSAGE.authcode) );
   }

   /* FIELD 39: RESPONSE CODE */
   if ( SWITCHED_IN == Tran_Flow )
   {
/* In all cases 0130 should have 00 RC*/

		if( AUTH_ADVICE_RESPONSE 		  == p_auth_tx->TLF01_details.tx_key ||
			AUTH_REVERSAL_ADVICE_RESPONSE == p_auth_tx->TLF01_details.tx_key ||
			AUTH_REVERSAL_RESPONSE 		  == p_auth_tx->TLF01_details.tx_key)
		{
			memcpy( HOST_MESSAGE.responsecd, "00",bf39_len );
		}
		else if ( 0 == strcmp( "82", p_auth_tx->TLF01_details.response_code ) )
		{
			memcpy( HOST_MESSAGE.responsecd, "05", 2 );
		}
		else if ( 0 == strncmp( NULL_STR, p_auth_tx->TLF01_details.response_code,2))
		{
			  strcpy( p_auth_tx->TLF01_details.response_code, "12" );
		}
		else if(p_auth_tx->TLF01_details.remote_nsp_type[0]=='P')
		{
			memcpy( HOST_MESSAGE.responsecd, "05", 2 );
		}
		else
		{
			  memcpy( HOST_MESSAGE.responsecd,
							  p_auth_tx->TLF01_details.response_code,
							  bf39_len );
		}
	}
   else if ( (AUTH_REVERSAL == p_auth_tx->TLF01_details.tx_key)||
	         (AUTH_VOID_SALE== p_auth_tx->TLF01_details.tx_key)||
			 (AUTH_VOID_PRE_AUTHORIZATION == p_auth_tx->TLF01_details.tx_key))
   {
	  /* Reversal and void sale won't exist unless orig txn is approved. 31 Oct,09 TF-Ajay */
	 /* Reversal response code should be as below - TF Phani
	  For Case 1, MC is expecting RC 06 to be sent in the 0400. full reversal
	  For Case 2, MC is expecting RC 17 to be sent in the 0400. full void 
	  For Case 3, MC is expecting RC 68 to be sent in the 0400. Reversal by late response 
	  For Case 4, MC is expecting RC 32 to be sent in the 0400 partial void */
	 if(AUTH_REVERSAL == p_auth_tx->TLF01_details.tx_key )
	 {
		 //memcpy( HOST_MESSAGE.responsecd, "68", 2 );
		 // As per Marvin comments sending 00 RC instead of 68 10/23/2017.
		 memcpy( HOST_MESSAGE.responsecd, "00", 2 );
	 }
	 else if(AUTH_VOID_SALE== p_auth_tx->TLF01_details.tx_key ||
			 AUTH_VOID_PRE_AUTHORIZATION == p_auth_tx->TLF01_details.tx_key)
	 {
	  memcpy( HOST_MESSAGE.responsecd, "17", 2 );
	 }
    
   }

   /*
    * October Mandated 2017
    * Sending of 0110 to MasterCard in case of timeouts 0110 with Cadencie,
    * ATP should not send value of "19" to MasterCard. Make it "01".
    *
    * ATP should not send DE39=19 or De39=91. Instead, ATP should send the following in case of timeout to Cadencie for applicable cases.
	  0410: De39=01
	  0430: De39=00
	  0130: De39=00
    *
    */
   if((0 == strncmp(p_auth_tx->TLF01_details.message_type,"0100",4))||
	  (0 == strncmp(p_auth_tx->TLF01_details.message_type,"0110",4)))
   {
	   if(0 == strncmp(p_auth_tx->TLF01_details.response_code,"19",2))
	   {
		   memset(HOST_MESSAGE.responsecd, 0x00, 2);
		   strncpy(p_auth_tx->TLF01_details.response_code,
				response_code_for_Incoming_timeout_Transaction,
				RESPONSE_CODE_LEN);
		   strncpy(HOST_MESSAGE.responsecd,
			   p_auth_tx->TLF01_details.response_code,
	   			RESPONSE_CODE_LEN);
	   }

	 /*  if(0 == strncmp(p_auth_tx->TLF01_details.response_code,"19",2))
	   {
		   memset(HOST_MESSAGE.responsecd, 0x00, 2);
		   strncpy(p_auth_tx->TLF01_details.response_code,"01",2);
		   memcpy(HOST_MESSAGE.responsecd,
				   p_auth_tx->TLF01_details.response_code,2);
	   }*/
   } /*This may not required, Since we respond 04xx immediately */
   else if((0 == strncmp(p_auth_tx->TLF01_details.message_type,"0400",4))||
    	   (0 == strncmp(p_auth_tx->TLF01_details.message_type,"0410",4))||
		   (0 == strncmp(p_auth_tx->TLF01_details.message_type,"0420",4))||
		   (0 == strncmp(p_auth_tx->TLF01_details.message_type,"0430",4))||
		   (0 == strncmp(p_auth_tx->TLF01_details.message_type,"0120",4))||
		   (0 == strncmp(p_auth_tx->TLF01_details.message_type,"0130",4))
   )
   {
	   if((0 == strncmp(p_auth_tx->TLF01_details.response_code,"19",2))||
	   	  (0 == strncmp(p_auth_tx->TLF01_details.response_code,"91",2)))
	   {
		   memset(HOST_MESSAGE.responsecd, 0x00, 2);
		   strncpy(p_auth_tx->TLF01_details.response_code,"00",2);
		   memcpy(HOST_MESSAGE.responsecd,
		   		  p_auth_tx->TLF01_details.response_code,2);
	   }
   }

   if(  0 != strncmp(p_auth_tx->TLF01_details.product_codes[1].code,"stndin",6)	&&
		0 == strncmp(p_auth_tx->TLF01_details.response_code,"96",2) 			&&
	   (0 == strncmp(p_auth_tx->TLF01_details.handler_queue,"ncmcrd",6)||
		0 == strncmp(p_auth_tx->TLF01_details.handler_queue,"ncmcr3",6)))
   {/* will include all ncmcrd and ncmcr3*/
	   if( p_auth_tx->TLF01_details.product_codes[7].code[0]!='\0' )
	   {
		   strncpy(p_auth_tx->TLF01_details.response_code,
			   p_auth_tx->TLF01_details.product_codes[7].code,2);
	   }
   }

   if(true == ncmcrd_check_for_ASI_txn(p_auth_tx) &&
	  true == ncmcrd_check_for_ASI_not_Supported_RC(p_auth_tx))
   {
	   strncpy(p_auth_tx->TLF01_details.response_code,
			   mc_asi_txn_rc,2);
	   memcpy(HOST_MESSAGE.responsecd,mc_asi_txn_rc,2);
   }

   /* FIELD 41: CARD ACCEPTOR TERMINAL IDENTIFICATION */
   memset( HOST_MESSAGE.termid, 0x20, bf41_len );
   memcpy( HOST_MESSAGE.termid,
           p_auth_tx->TLF01_details.terminal_id,
           strlen(p_auth_tx->TLF01_details.terminal_id) );

   /* FIELD 42: CARD ACCEPTOR IDENTIFICATION CODE */
   memset( HOST_MESSAGE.cardaccid, 0x20, bf42_len );
   memcpy( HOST_MESSAGE.cardaccid,
           p_auth_tx->TLF01_details.merchant_id,
           strlen(p_auth_tx->TLF01_details.merchant_id) );

   /* FIELD 43:	CARD ACCEPTOR NAME / LOCATION */
   memset( HOST_MESSAGE.cardaccloc, 0x20, bf43_len );
   itemp = strlen( p_auth_tx->MCF01_details.name02 );
   memcpy( HOST_MESSAGE.cardaccloc, p_auth_tx->MCF01_details.name02, 22 );
   if ( 22 > itemp )
   {
      /* Make sure spaces follow the merchant name. */
      memset( &HOST_MESSAGE.cardaccloc[itemp], 0x20, 22-itemp );
   }
   itemp = strlen( p_auth_tx->MCF01_details.city );
   memcpy( &HOST_MESSAGE.cardaccloc[23], p_auth_tx->MCF01_details.city, 13 );
   if ( 13 > itemp )
   {
      /* Make sure spaces follow the merchant's city. */
      memset( &HOST_MESSAGE.cardaccloc[23+itemp], 0x20, 13-itemp );
   }
   /* Country code - 3 alpha characters */
#ifdef MC7_APR2024
   if(strncmp(p_auth_tx->MCF01_details.intl_code, "PH", 2) == 0 ||
	   strlen(p_auth_tx->MCF01_details.intl_code) == 0)
   {
	   if(strlen(mc_acceptor_country_code) == 3)
	   {
		   memcpy( &HOST_MESSAGE.cardaccloc[37], mc_acceptor_country_code, 3 );
	   }
	   else
	   {
		   memcpy( &HOST_MESSAGE.cardaccloc[37], "PHL", 3 );
	   }
   }
   else
#endif
   memcpy( &HOST_MESSAGE.cardaccloc[37], "PHL", 3 );

	if( strncmp(p_auth_tx->TLF01_details.handler_queue, parvbh_que_name, 7) == 0 )
	{
		memset( HOST_MESSAGE.cardaccloc, 0x20, bf43_len );
	    itemp = strlen( p_auth_tx->TLF01_details.card_holder_name);

		memcpy(   HOST_MESSAGE.cardaccloc,
				  p_auth_tx->TLF01_details.card_holder_name,
				  itemp );
		if ( 22 > itemp )
		{
			/* Make sure spaces follow the merchant name. */
			memset( &HOST_MESSAGE.cardaccloc[itemp], 0x20, 22-itemp );
		}

		if(p_auth_tx->TLF01_details.ncf01_retired_cwk[0] != '\0')
		{
			memcpy( HOST_MESSAGE.cardaccloc+23,
					p_auth_tx->TLF01_details.ncf01_retired_cwk,
					13 );

			memcpy( HOST_MESSAGE.cardaccloc+37,
					p_auth_tx->TLF01_details.ncf01_retired_cwk+13,
					3 );
		}
		else
		{
			memset( &HOST_MESSAGE.cardaccloc[23], 0x20, 13 );
		    /* Country code - 3 alpha characters */
		    memcpy( &HOST_MESSAGE.cardaccloc[37], "PHL", 3 );
		}
	}

   /* Store DE43 here so it gets logged to TLF01:
    *   Merchant Name         --> TLF01.card_holder_name
    *   City and Country Code --> TLF01.ncf01_retired_cwk
    * SCR 775
    */
   if ( p_auth_tx->TLF01_details.card_holder_name[0] == 0x00 )
   {
      memcpy( p_auth_tx->TLF01_details.card_holder_name,
              HOST_MESSAGE.cardaccloc,
              22 );
   }

   if ( p_auth_tx->TLF01_details.ncf01_retired_cwk[0] == 0x00 )
   {
      memcpy( p_auth_tx->TLF01_details.ncf01_retired_cwk,
              HOST_MESSAGE.cardaccloc+23,
              13 );
      memcpy( p_auth_tx->TLF01_details.ncf01_retired_cwk+13, "PHL", 3 );
   }

    /* FIELD 44: Additional Response Data */
	//Account Status Inquiry Service Transactions
	if( (p_auth_tx->acceptor_term_name[6]== '8') &&
		(0 == strncmp(p_auth_tx->TLF01_details.visa_merchant_id,"01",2)))
	 {
		memcpy( HOST_MESSAGE.addl_rsp_len,"03", 2 );
		memcpy( HOST_MESSAGE.addlrspdata, "004", 3);
	 }

	if(strlen(p_auth_tx->TLF01_details.num_sales)> 0)
	{
		strncpy(temp_error_bit_nun,p_auth_tx->TLF01_details.num_sales,3);
		error_bit = atoi(temp_error_bit_nun);

		switch(error_bit)
		{
			case 48:
				// As of now only 89 tg atp is validatin in
				memcpy( HOST_MESSAGE.addlrspdata,"048089",6);
				memcpy( HOST_MESSAGE.addl_rsp_len,"06", 2 );
				break;
			case 108:
				memcpy( HOST_MESSAGE.addlrspdata,"108",3);
				memcpy( HOST_MESSAGE.addl_rsp_len,"03", 2 );
				break;
			case 112:
				memcpy( HOST_MESSAGE.addlrspdata,"112",3);
				memcpy( HOST_MESSAGE.addl_rsp_len,"03", 2 );
				break;
			default:
				memcpy( HOST_MESSAGE.addlrspdata,temp_error_bit_nun,3);
				memcpy( HOST_MESSAGE.addl_rsp_len,"03", 2 );
				break;
		}
		memset( tempstr, 0x00, sizeof( tempstr ) );
		sprintf( tempstr,"Format Err: Field 44 = \"%s\"",
				   p_auth_tx->TLF01_details.num_sales );

		memset(Log_Bufr,0x00,sizeof(Log_Bufr));
		sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
				p_auth_tx->TLF01_details.terminal_id,
				p_auth_tx->TLF01_details.merchant_id,
				p_auth_tx->TLF01_details.retrieval_ref_num);
		memcpy( p_auth_tx->TLF01_details.response_text, tempstr,
				sizeof(p_auth_tx->TLF01_details.response_text)-1 );
		strcat(tempstr,Log_Bufr);
		ncmcrd_log_message( 2, 2, tempstr , "build_host_structure" , 0 ,p_auth_tx->TLF01_details.card_num);
	}

   /* FIELD 45: TRACK 1 */
   if ( 0 != strcmp( NULL_STR, p_auth_tx->TLF01_details.track1 ) )
   {
      /* Track 1 is present.  Do not send out track 2. */
      memset( HOST_MESSAGE.track2_len, 0x00, sizeof(HOST_MESSAGE.track2_len) );

      itemp = strlen( p_auth_tx->TLF01_details.track1 );
      sprintf( HOST_MESSAGE.track1_len, "%02d", itemp );
      strcpy( HOST_MESSAGE.track1, p_auth_tx->TLF01_details.track1 );
   }

   /* FIELD 48: ADDITIONAL DATA */
   if ( SWITCHED_IN == Tran_Flow )
   {
      /* For switched in transactions, according to MasterCard, we should
       * only echo what came in with the request if there is some sort of
       * issuer response involved.  The only response information we are
       * currently sending back is tags 84 and 87.  So if we are sending
       * either, or both, of these tags, then we should also send back
       * whatever came in the request (stored in issuer_script_2). But if
       * we are not sending back one of these tags, do not send anything
       * back in this field.
       *
       * However, there is 1 subfield that is an exception. If subfield
       * 89 is in the request, it MUST be echoed back in the response.
       *
       * When sending info in this field, be sure to echo the request info
       * first because it has the TCC (the first byte), then the response
       * info can follow.
       */
	    itemp = 0;
		echo_tags = 0;
		va_len = strlen( p_auth_tx->EMV_details.issuer_script_2 );
	  
		if(va_len == 0)
		{
			va_len = p_auth_tx->future_use_3[0];
		}
	  
   		  if ( va_len > 0 )
		  {
			 /* Check for response information. */
			 memset( temp_str, 0x00, sizeof(temp_str) );
			 if (p_auth_tx->MCF01_details.rps_merchant[0]=='Y')
			 {
				 // During request parsing, if transaction is RPS, rps flag will be filled in MCF01
				/* Send Merchant Advice Code in subfield 84. */
				if ( (strncmp(p_auth_tx->TLF01_details.response_text,"01",2)==0) ||
					 (strncmp(p_auth_tx->TLF01_details.response_text,"02",2)==0) ||
					 (strncmp(p_auth_tx->TLF01_details.response_text,"03",2)==0)  )
				{
				   itemp += 6;
				   strcat(  temp_str, DE48_TAG84 );
				   strncat( temp_str, p_auth_tx->TLF01_details.response_text, 2 );
				}

			 }
#if MC03_OCT2023
			 else
			 {
				 if(p_auth_tx->TLF01_details.product_codes[16].amount[4] == CARD_NOT_PRESENT_MC_DE61_SE5)
				 {

					 /*DE48 SE84:*/
					if(NULL != (mac_value = strstr(p_auth_tx->TLF01_details.response_text,"DE48 SE84:")))
					{
						itemp += 6;
					   strcat(  temp_str, DE48_TAG84 );
					   strncat( temp_str, mac_value+10, 2 );
					   mac_value= NULL;
					}

				 }

			 }
#endif




			 /* Check the CVC2 result.
			 if ( p_auth_tx->TLF01_details.cvc != 0x00 )
			 {
				itemp += 5;  //Increase length of field 48 by 5

				/*2 byte tag + 2 byte len
				strncat( temp_str, DE48_TAG87, 4 );
				temp_str[itemp-1] = p_auth_tx->TLF01_details.cvc;
			 }
			/* Check for CVC2 data - TF Phani , If CVC2 data is not verified, Populate below code for ISSUERS
			 else if ( p_auth_tx->TLF01_details.cvc_data != 0x00 )
			 {
				itemp += 5;  //Increase length of field 48 by 5

				/*2 byte tag + 2 byte len
				strncat( temp_str, DE48_TAG87_NP, 5 ); // Issuer not processed CVC2
				//temp_str[itemp-1] = p_auth_tx->TLF01_details.cvc;
			 }*/
			 if(0 == strncmp(p_auth_tx->TLF01_details.message_type,"0100",4) 	&&
				p_auth_tx->TLF01_details.cvc_data[0] != '\0')
			 {
				Build_DE48_tag87_from_Tag92=1;
				itemp += 5;
				strncat( temp_str, DE48_TAG87, 4 );

				/*For ATP STIP*/
				if(strcmp(p_auth_tx->TLF01_details.product_codes[1].code,STANDIN)==0 )
				{
				/* Tag 87 data should be copied from cvc field*/
				if('\0' != p_auth_tx->TLF01_details.cvc)
				{
				temp_str[itemp-1] = p_auth_tx->TLF01_details.cvc;
					memset(Log_Bufr,0x00,sizeof(Log_Bufr));
				   sprintf(Log_Bufr,"Filled value from Magstripe txn Term Id:%s ,Merch Id: %s RRN: %s ",
						   p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
						ncmcrd_log_message( 2, 1, Log_Bufr , "mcrdutil_transfer_ctl_to_auth_app" , 0 ,p_auth_tx->TLF01_details.card_num);
				}
				else if(0 == strncmp(p_auth_tx->TLF01_details.security_response_code,"00",2))
				{
					temp_str[itemp-1] = DE48_TAG87_CVC2_MATCH;
					memset(Log_Bufr,0x00,sizeof(Log_Bufr));
				   sprintf(Log_Bufr,"Filled value for non magstripe txn, cvc2 match  Term Id:%s ,Merch Id: %s RRN: %s ",
						   p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
						ncmcrd_log_message( 2, 1, Log_Bufr , "mcrdutil_transfer_ctl_to_auth_app" , 0 ,p_auth_tx->TLF01_details.card_num);
				}
				else
				{
					temp_str[itemp-1] = DE48_TAG87_CVC2_NO_MATCH;
					sprintf(Log_Bufr,"Filled value for non magstripe txn, no cvc2 match  Term Id:%s ,Merch Id: %s RRN: %s ",
											   p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
						ncmcrd_log_message( 2, 1, Log_Bufr , "mcrdutil_transfer_ctl_to_auth_app" , 0 ,p_auth_tx->TLF01_details.card_num);
					}
				}
				/*Timeout Senario*/
				else if(0 == strncmp(p_auth_tx->TLF01_details.response_code,
							   	     response_code_for_Incoming_timeout_Transaction,
									 RESPONSE_CODE_LEN))
				   {
						temp_str[itemp-1] = DE48_TAG87_CVC2_NOT_PROCESSED;
						sprintf(Log_Bufr,"Timeout from Cadencie, no cvc2 Processed  Term Id:%s ,Merch Id: %s RRN: %s ",
								p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
						ncmcrd_log_message( 2, 1, Log_Bufr , "mcrdutil_transfer_ctl_to_auth_app" , 0 ,p_auth_tx->TLF01_details.card_num);
				}
			 }


			 if((0 == strncmp(p_auth_tx->TLF01_details.handler_queue,"ncmcrd",6)||
				0 == strncmp(p_auth_tx->TLF01_details.handler_queue,"ncmcr3",6)) &&
			   (0 == strncmp(p_auth_tx->TLF01_details.message_type,"0100",4) ||
				0 == strncmp(p_auth_tx->TLF01_details.message_type,"0120",4) ||
				0 == strncmp(p_auth_tx->TLF01_details.message_type,"0400",4) ||
				0 == strncmp(p_auth_tx->TLF01_details.message_type,"0420",4)) &&
			   (0 == strncmp(p_auth_tx->TLF01_details.processing_code,"00",2) ||
				0 == strncmp(p_auth_tx->TLF01_details.processing_code,"20",2)))
			 {
				 build_DE48_from_req_for_STIP_txn_3DS2(&src_len, HOST_MESSAGE.addl_data,p_auth_tx );
				 va_len = src_len;
			 }
			 else
			 {
				 if(va_len < 128)
				 {
						memcpy( HOST_MESSAGE.addl_data,p_auth_tx->EMV_details.issuer_script_2,va_len );
				 }
				 else
				 {
					 memcpy( HOST_MESSAGE.addl_data,p_auth_tx->future_use_3+1,va_len);
				 }
			 }

			/*  if and else both have the same checks
			 * else
			 {
				 if(Build_DE48_tag87_from_Tag92=1)
				 {
					 build_DE48_from_req_for_STIP_txn(&src_len, HOST_MESSAGE.addl_data,p_auth_tx );
					 va_len = src_len;
				 }
				 else if((0 == strncmp(p_auth_tx->TLF01_details.handler_queue,"ncmcrd",6)||
						 	0 == strncmp(p_auth_tx->TLF01_details.handler_queue,"ncmcr3",6))&&
						 	(0 == strncmp(p_auth_tx->TLF01_details.message_type,"0100",4) ||
					 	 	0 == strncmp(p_auth_tx->TLF01_details.message_type,"0120",4) ||
							0 == strncmp(p_auth_tx->TLF01_details.message_type,"0400",4) ||
							0 == strncmp(p_auth_tx->TLF01_details.message_type,"0420",4)) &&
							(0 == strncmp(p_auth_tx->TLF01_details.processing_code,"00",2) ||
							0 == strncmp(p_auth_tx->TLF01_details.processing_code,"20",2)))
				 {
					 build_DE48_from_req_for_STIP_txn(&src_len, HOST_MESSAGE.addl_data,p_auth_tx );
					 va_len = src_len;
				 }
				 else
				 {
					 if(va_len <= 128)
					 {
						 memcpy( HOST_MESSAGE.addl_data,
								p_auth_tx->EMV_details.issuer_script_2,
								va_len );
					 }
					 else
					 {
						 memcpy( HOST_MESSAGE.addl_data,p_auth_tx->future_use_3+1,va_len);
					 }
				 }
			 }*/
			/* Always echo back De48 received from MC during STIP processing,
			 * requirement provided by Marvin, April, 2020 */
			 strcat( HOST_MESSAGE.addl_data, temp_str );
			 itemp += va_len;

	         /* Do this for cleanup. */
	         memset( p_auth_tx->EMV_details.issuer_script_2, 0x00,
	                 sizeof(p_auth_tx->EMV_details.issuer_script_2) );

			 /*if ( itemp > 0 )
			 {
				/* We do have response info, so echo back the request info.
				memcpy( HOST_MESSAGE.addl_data,
						p_auth_tx->EMV_details.issuer_script_2,
						va_len );

				// Now append the response info to the end of the request info.
				strcat( HOST_MESSAGE.addl_data, temp_str );
				itemp += va_len;
			 }
			 else
			 {
				// There is no response info for the host. Check
				 // to see if any subfields need to be echoed.

				 // Subfield 89 is stored in network[9]. Must be echoed.
// During request parsing, based in the DE48 tag 89 present MCF01 will be filled.
				if (p_auth_tx->MCF01_details.network[9].priority_routing[0] != 0x00)
				{
				   //Subfield 89 is present.  Echo all of DE48.
				   itemp = va_len;
				   memcpy( HOST_MESSAGE.addl_data,
						   p_auth_tx->EMV_details.issuer_script_2,
						   va_len );
				}
			 }*/
      }

   }
   else
   {
      /* SWITCHED OUT */
		memset(HOST_MESSAGE.addl_data,0x00,sizeof(HOST_MESSAGE.addl_data));
       /* Verified by visa VbV */
      itemp = 1;
      ucaf_data_len = 0;
      ecom_total_len = 0;
      if ( e_commerce )
      {
         ecom_total_len = atoi( p_auth_tx->int_net_facility_len );
         if ( ecom_total_len > 0 )
         {
            /* UCAF data exists.  Format of this string of data is
             * multiple fields of 2 BCD bytes giving length of ASCII data.
             */

            /* Put UCAF data into proper subfields. */
            memset( merch_serial_numbers,  0x00, sizeof(merch_serial_numbers) );
            memset( security_level_ind,    0x00, sizeof(security_level_ind)   );
            memset( ucaf_data,             0x00, sizeof(ucaf_data)            );

            ecom_idx = 0;
            memcpy( VbV_data, p_auth_tx->int_net_facility_data, ecom_total_len);

            genutil_bcd_to_asc (p_auth_tx->int_net_facility_data, 24,  
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

            /* ------------------- */
            /* UCAF Data (DE48.43) */
            /* ------------------- */
            memset( ecom_substr,     0x00, sizeof(ecom_substr)     );
            memset( bcd_ecom_substr, 0x00, sizeof(bcd_ecom_substr) );

            memcpy( bcd_ecom_substr, &VbV_data[ecom_idx], 2 );
            genutil_bcd_to_asc( bcd_ecom_substr, 2, ecom_substr, 4 );
            ecom_sub_len = atoi( ecom_substr );
            ecom_idx += 2;

            memcpy( ucaf_data, &VbV_data[ecom_idx], ecom_sub_len );
            ucaf_data_len = ecom_sub_len;

            /* -------------------------------------- */
            /* Populate Field 48 E-Commerce Subfields */
            /* -------------------------------------- */

            /* The E-Commerce data is now parsed into its separate
             * components.  Now put each of them into field 48 subfields
             * using proper tags, lengths, and values.
             */
          		strcpy( HOST_MESSAGE.addl_data, "T" ); /* TCC = E-Commerce */
				ecom_total_len = 1;

				/****************************************
				 * Currently Not Supporting Subfield 40 *
				 ****************************************/

				/* TAG 40 - Serial Numbers *
				strcat( HOST_MESSAGE.addl_data, "40" );

				/* Length of Tag 40 *
				memset( temp_str3, 0x00, sizeof(temp_str3) );
				sprintf( temp_str3, "%02d", serial_nbrs_len );
				strcat( HOST_MESSAGE.addl_data, temp_str3 );
				ecom_total_len = 5;   /* TCC + TAG + Len = 5 *

				/* Tag 40 Data *
				memcpy( HOST_MESSAGE.addl_data + ecom_total_len,
						merch_serial_numbers,
						sizeof(merch_serial_numbers) );
				ecom_total_len += serial_nbrs_len;

				/********************************************************
				 * End of Not Supporting Subfield 40 (except see below) *
				 ********************************************************/

				/*TAG 37*/
				if( 0x00 != p_auth_tx->TLF01_details.orig_auth_number[0] &&
						(0    == strncmp(p_auth_tx->TLF01_details.message_type,"0120",4)||
						 0    == strncmp(p_auth_tx->TLF01_details.message_type,"0100",4)||
						 0    == strncmp(p_auth_tx->TLF01_details.message_type,"0200",4)) &&
						(0    == strncmp(p_auth_tx->TLF01_details.handler_queue,"VOICE",5)||
						 0    == strncmp(p_auth_tx->TLF01_details.handler_queue,"dcp",3)))
				{
						build_DE48_TAG37_foracq_txn(p_auth_tx,&itemp);
						ecom_total_len +=11;
				}

				if( 0x00 != p_auth_tx->TLF01_details.dbsales_amount[0] &&
						0x00 != p_auth_tx->TLF01_details.refund_amount[0] &&
						0x00 != p_auth_tx->TLF01_details.period[0] &&
						(0 == strncmp("010", p_auth_tx->TLF01_details.pos_entry_mode, 3)) ||
							p_auth_tx->MCF01_details.rps_merchant[0]=='Y'						&&
						(0    == strncmp(p_auth_tx->TLF01_details.message_type,"0120",4)||
						 0    == strncmp(p_auth_tx->TLF01_details.message_type,"0100",4)||
						 0    == strncmp(p_auth_tx->TLF01_details.message_type,"0400",4)) &&
						(0    == strncmp(p_auth_tx->TLF01_details.handler_queue,"ncmcrd",6)||
						 0    == strncmp(p_auth_tx->TLF01_details.handler_queue,"ncmcrd3",7)) &&
						( 0==strcmp(p_auth_tx->TLF01_details.product_codes[1].code,STANDIN)==0))
				{

						strncat( HOST_MESSAGE.addl_data, DE48_TAG41 ,7);
						strncat( HOST_MESSAGE.addl_data+7, p_auth_tx->TLF01_details.dbsales_amount ,12);
						strncat( HOST_MESSAGE.addl_data+19, p_auth_tx->TLF01_details.refund_amount ,12);
						strncat( HOST_MESSAGE.addl_data+31, p_auth_tx->TLF01_details.period ,2);
						ecom_total_len += 31;

				}


				/* TAG 42 - E-Commerce Indicators */
				ucaf_indicator = security_level_ind[sec_lev_ind_len-1];/* Last char*/
				if(ucaf_data[0] == 'h' || ucaf_data[0] == 'H' )
				{
					memcpy( HOST_MESSAGE.addl_data + ecom_total_len,
										"42070103211",
										11 );

					ecom_total_len += 11;
					/* store the ECI value sent to cadencie*/
					strncpy(p_auth_tx->TLF01_details.product_codes[12].quantity,"11",2);
				}
				else if(ucaf_data[0] == 'j' || ucaf_data[0] == 'J' )
				{
					memcpy( HOST_MESSAGE.addl_data + ecom_total_len,
							"42070103212",
							11 );

					ecom_total_len += 11;
					/*memcpy( HOST_MESSAGE.addl_data + ecom_total_len,
							DE48_TAG71,
							DE48_TAG71_LEN );
					ecom_total_len = ecom_total_len + DE48_TAG71_LEN; */
					/* store the ECI value sent to cadencie*/
					strncpy(p_auth_tx->TLF01_details.product_codes[12].quantity,"12",2);

				}
				else
				{
					len = strlen(DE48_TAG42);
					memcpy( HOST_MESSAGE.addl_data + ecom_total_len,
					DE48_TAG42,
					len );
					ecom_total_len += len;

					//if ( p_auth_tx->MCF01_details.totals_ind[0] == 'N' )
					/* if ucaf_data doesn't contain j or h send downgrade ECI value 210 to MC */
					ucaf_indicator = UCAF_NOT_SUPPORTED[0];

					memset( HOST_MESSAGE.addl_data + ecom_total_len, ucaf_indicator, 1);
					ecom_total_len++;
					/* store the ECI value sent to cadencie*/
					strncpy(p_auth_tx->TLF01_details.product_codes[12].quantity,"10",2);
				}

            /* TAG 43 - Universal Cardholder Authentication Field (UCAF) */
            if ( ucaf_indicator == '2' || ucaf_data_len > 0 )
            {
               memcpy( HOST_MESSAGE.addl_data + ecom_total_len, "43", 2 );
               ecom_total_len += 2;

               /* Length of Tag 43 */
               memset( temp_str3, 0x00, sizeof(temp_str3) );
               sprintf( temp_str3, "%02d", ucaf_data_len );
               memcpy( HOST_MESSAGE.addl_data + ecom_total_len, temp_str3, 2 );
               ecom_total_len += 2;

               /* Tag 43 Data */
               memcpy( HOST_MESSAGE.addl_data + ecom_total_len,
                       ucaf_data,
                       ucaf_data_len );
               ecom_total_len += ucaf_data_len;
            }
            else
               ucaf_data_len = 0;

            /* Store UCAF data into TLF01 so Equitable Bank can access it.
             * Use voice_auth_text since it is not used for ecommerce.
             * Store Serial #'s (..40), UCAF (..32).
             * Use profile for Security Level Inicator (7) - store last 2.
             */

            /**********************************
             * Comment out support for tag 40 *
            memcpy( p_auth_tx->TLF01_details.voice_auth_text,
                    merch_serial_numbers,
                    serial_nbrs_len );

            memcpy( p_auth_tx->TLF01_details.voice_auth_text+serial_nbrs_len,
             *
             **********************************/
            memcpy( p_auth_tx->TLF01_details.profile,
                    security_level_ind+(sec_lev_ind_len-2),
                    2 );

            memcpy( p_auth_tx->TLF01_details.voice_auth_text,
                    ucaf_data,
                    ucaf_data_len );

			 /* For AsiaPay/Ecom Txn send DE48 SE66-SF1, the velaue will be read from tf.ini file*/
			memcpy( HOST_MESSAGE.addl_data + ecom_total_len, DE48_TAG66, 2 );
			ecom_total_len = ecom_total_len + 2 ;
			memcpy( HOST_MESSAGE.addl_data + ecom_total_len, "050101", 6 );
			ecom_total_len = ecom_total_len + 6;
			HOST_MESSAGE.addl_data[ecom_total_len] = Mc3Dsecureversion;
			ecom_total_len++;

            itemp = ecom_total_len;
         }
         else
         {
            /* There is no UCAF data. Still need subfield 42. */
            strcpy( HOST_MESSAGE.addl_data, "T" );
            strcat( HOST_MESSAGE.addl_data, DE48_TAG42 );
            strcat(HOST_MESSAGE.addl_data, UCAF_NOT_SUPPORTED );

           /* if ( p_auth_tx->MCF01_details.totals_ind[0] == 'N' )
               strcat(HOST_MESSAGE.addl_data, UCAF_NOT_SUPPORTED );
            else
               strcat(HOST_MESSAGE.addl_data, UCAF_SUPPORTED_BUT_NOT_POPULATED);
			*/
            itemp = strlen(HOST_MESSAGE.addl_data);
         }
      }
      else
      {
         /* Determine Transaction Category Code based on Industry code */
         switch( iIndustry )
         {
            case 100:
            case 600:  strcpy( HOST_MESSAGE.addl_data, "R" ); break;
            case 101:  strcpy( HOST_MESSAGE.addl_data, "F" ); break;
            case 110:  strcpy( HOST_MESSAGE.addl_data, "O" ); break;
            case 200:  strcpy( HOST_MESSAGE.addl_data, "X" ); break;
            case 300:  strcpy( HOST_MESSAGE.addl_data, "H" ); break;
            case 400:  strcpy( HOST_MESSAGE.addl_data, "A" ); break;
            case 500:
            case 509:
            case 501:
            case 700:
            case 701:
            case 702:
            case 800:
            		   strcpy( HOST_MESSAGE.addl_data, "T" ); break;
            case 900:  strcpy( HOST_MESSAGE.addl_data, "C" ); break;
            case 902:  strcpy( HOST_MESSAGE.addl_data, "C" ); break;
			case 903:  strcpy( HOST_MESSAGE.addl_data, "U" ); break; /*Added new case for U*/
            default:   strcpy( HOST_MESSAGE.addl_data, "U" ); break;
         }

      }

      if(e_commerce == false)
      {
    		  /*TAG 37*/
    	  if(  0x00 != p_auth_tx->TLF01_details.orig_auth_number[0] &&
    		   (0    == strncmp(p_auth_tx->TLF01_details.message_type,"0100",4)||
    			0    == strncmp(p_auth_tx->TLF01_details.message_type,"0200",4)||
    			0    == strncmp(p_auth_tx->TLF01_details.message_type,"0120",4)) &&
   			   (0    == strncmp(p_auth_tx->TLF01_details.handler_queue,"VOICE",5)||
   				0    == strncmp(p_auth_tx->TLF01_details.handler_queue,"dcp",3)))
       	  {
   				  build_DE48_TAG37_foracq_txn(p_auth_tx,&itemp);
   				  ecom_total_len +=11;
          }

			if( 0x00 != p_auth_tx->TLF01_details.dbsales_amount[0] &&
					0x00 != p_auth_tx->TLF01_details.refund_amount[0] &&
					0x00 != p_auth_tx->TLF01_details.period[0] &&
					(0 == strncmp("010", p_auth_tx->TLF01_details.pos_entry_mode, 3)) ||
						p_auth_tx->MCF01_details.rps_merchant[0]=='Y'						&&
					(0    == strncmp(p_auth_tx->TLF01_details.message_type,"0120",4)||
					 0    == strncmp(p_auth_tx->TLF01_details.message_type,"0100",4)||
					 0    == strncmp(p_auth_tx->TLF01_details.message_type,"0400",4)) &&
					(0    == strncmp(p_auth_tx->TLF01_details.handler_queue,"ncmcrd",6)||
					 0    == strncmp(p_auth_tx->TLF01_details.handler_queue,"ncmcrd3",7)) &&
					( 0==strcmp(p_auth_tx->TLF01_details.product_codes[1].code,STANDIN)==0))
			{

					strncat( HOST_MESSAGE.addl_data, DE48_TAG41 ,7);
					strncat( HOST_MESSAGE.addl_data+7, p_auth_tx->TLF01_details.dbsales_amount ,12);
					strncat( HOST_MESSAGE.addl_data+19, p_auth_tx->TLF01_details.refund_amount ,12);
					strncat( HOST_MESSAGE.addl_data+31, p_auth_tx->TLF01_details.period ,2);
					ecom_total_len += 31;

			}
      }


      /* Get CVC2 data if it exists. */
      if ( p_auth_tx->TLF01_details.cvc_data[0] != 0x00 )
      {
         /* CVC2 Data is stored in 'cvc_data' for all card types except
          * Amex. Amex has a 4-digit CSC and is stored in interest_rate.
          * Visa requires a presence indicator and a response type.
          * So the length is variable - dependent upon card type.
          */
         strcat( HOST_MESSAGE.addl_data, DE48_TAG92 );
         if ( 0 == strcmp(p_auth_tx->BIN01_details.card_family,CARD_MASTERCARD))
         {
            itemp += 7;
            strcpy( cvc2_data, "03" ); /* Length */
            strcat( cvc2_data, p_auth_tx->TLF01_details.cvc_data );
         }
         else if ( 0 == strcmp(p_auth_tx->BIN01_details.card_family,CARD_VISA))
         {
            itemp += 10;
            strcpy( cvc2_data, "0611 " ); /* Length=6, Presence=1, response=1 */
            strcat( cvc2_data, p_auth_tx->TLF01_details.cvc_data );
         }
         else
         {
            itemp += 7;
            strcpy( cvc2_data, "03" ); /* Length */
            strcat( cvc2_data, p_auth_tx->TLF01_details.cvc_data );
         }

         strcat( HOST_MESSAGE.addl_data, cvc2_data );
      }
      else if ( p_auth_tx->TLF01_details.interest_rate[0] != 0x00 )
      {
         /* For Amex CSC2, the value will be in 'interest_rate' because
          * it is too long for 'cvc_data' (4-digits).  Is this an Amex card?
          * Note this could also be a Visa card with an Amex CSC2.
          */
         if ( (0 == strcmp(p_auth_tx->BIN01_details.card_family,CARD_AMEX)) ||
              (0 == strcmp(p_auth_tx->BIN01_details.card_family,CARD_VISA))  )
         {
            /* Yes, this is an Amex (or Visa) card. Get CSC2. */
            itemp += 8;
            strcat( HOST_MESSAGE.addl_data, DE48_TAG92 );
            strcat( HOST_MESSAGE.addl_data, "04" ); /* Length */
            strcat( HOST_MESSAGE.addl_data,
                    p_auth_tx->TLF01_details.interest_rate );
         }
      }

#if BDOR_68_REQ_1_MC
      if ((true == ncmcrd_check_if_trx_is_mc_MobilePOS_NFC_Tap2Phone_trx_with_pin( p_auth_tx ))||
          (true == ncmcrd_check_if_trx_is_mc_MobilePOS_NFC_Tap2Phone_trx_without_pin( p_auth_tx )))
      {
    	  itemp += DE48_TAG21_SF1_LEN;
    	  strncat( HOST_MESSAGE.addl_data, DE48_TAG21_SF1_FOR_MPOS ,DE48_TAG21_SF1_LEN);
      }
#endif

 // Installement data
	  if( p_auth_tx->TLF01_details.deferred_term_length[0]!= 0x00)
	  	{	  	
		itemp += 8;
		strcat( HOST_MESSAGE.addl_data, DE48_TAG95);
		strcat( HOST_MESSAGE.addl_data, DE48_TAG95_INST_CODE);
	  	}
	//Populate DE 48 Subelement 20 is mandatory for both 0120 and 0400.
	if(AUTH_REVERSAL == p_auth_tx->TLF01_details.tx_key || 
	   AUTH_REVERSAL_ADVICE== p_auth_tx->TLF01_details.tx_key ||
	   AUTH_REVERSAL_REPEAT== p_auth_tx->TLF01_details.tx_key ||
	   AUTH_VOID_SALE== p_auth_tx->TLF01_details.tx_key ||
	   AUTH_VOID_PRE_AUTHORIZATION== p_auth_tx->TLF01_details.tx_key ||
	   AUTH_ADVICE== p_auth_tx->TLF01_details.tx_key ||
	   AUTH_ADVICE_REPEAT== p_auth_tx->TLF01_details.tx_key)
		{//DE48_TAG20
		 itemp += 5;
		 strcat( HOST_MESSAGE.addl_data, DE48_TAG20 );
		}
	  //Populate DE48.Sub element 63 for reversal
	  

	  
	  if(AUTH_REVERSAL == p_auth_tx->TLF01_details.tx_key || 
		 AUTH_REVERSAL_ADVICE== p_auth_tx->TLF01_details.tx_key ||
		 AUTH_REVERSAL_REPEAT== p_auth_tx->TLF01_details.tx_key ||
		 AUTH_VOID_SALE== p_auth_tx->TLF01_details.tx_key		||
		 AUTH_VOID_PRE_AUTHORIZATION == p_auth_tx->TLF01_details.tx_key
		 )
	  	{
	  	 itemp += 19;
		 strcat( HOST_MESSAGE.addl_data, DE48_TAG63 );
		if('Y' == p_auth_tx->TLF01_details.voice_auth[0] )
		{
			strncat( HOST_MESSAGE.addl_data, p_auth_tx->TLF01_details.visa_tran_id,9);
			strncpy(p_auth_tx->TLF01_details.mcard_banknet_len,'009',3);
			strncpy(p_auth_tx->TLF01_details.mcard_banknet,p_auth_tx->TLF01_details.visa_tran_id,9);
		}
		else if ( p_auth_tx->TLF01_details.mcard_banknet[0] != 0x00 )
		 strncat( HOST_MESSAGE.addl_data,  p_auth_tx->TLF01_details.mcard_banknet,9);
		else
		 strncat( HOST_MESSAGE.addl_data, "000000000",9);
		 
		// Settlement date followed by 2 spaces
		if(p_auth_tx->TLF01_details.settlement_date[0] != 0x00)
		{
			//strncat( HOST_MESSAGE.addl_data, "1212" ,4);
			//mc reversal from POS (OCT mandated 2017)
			strncat( HOST_MESSAGE.addl_data,  p_auth_tx->TLF01_details.settlement_date,4);
		}
		else
		{
			strncat( HOST_MESSAGE.addl_data,  "0000",4);
		}
			
		 strncat( HOST_MESSAGE.addl_data, "  ",2);
		#ifdef _DEBUG
			printf("%s\n",(char *)HOST_MESSAGE.addl_data);
		printf("%d",itemp);
		#endif 
   	    }

	  else if(((0 == strncmp(p_auth_tx->TLF01_details.processing_code,"30",2))||
			   (0 == strncmp(p_auth_tx->TLF01_details.processing_code,"38",2)))&&
			  (p_auth_tx->TLF01_details.mcard_banknet[0] != 0x00))
			{
		  	  	 itemp += 19;
		  		 strcat( HOST_MESSAGE.addl_data, DE48_TAG63 );
		  		 strncat( HOST_MESSAGE.addl_data,  p_auth_tx->TLF01_details.mcard_banknet,9);
		  		 strncpy(p_auth_tx->TLF01_details.mcard_banknet_len,'009',3);
		  		 strncat( HOST_MESSAGE.addl_data,  p_auth_tx->TLF01_details.settlement_date,4);
		  		 strncat( HOST_MESSAGE.addl_data, "  ",2);
			}
	// 	contactless transactions should have a device type=card, DE 48, SE 23=00
	  /*if(strncmp(p_auth_tx->TLF01_details.pos_entry_mode,"007",3) == 0)
	  	{	// This is a CHIP contactless card
			
			itemp += 6;
		 strcat( HOST_MESSAGE.addl_data, DE48_TAG23);
		 strcat (HOST_MESSAGE.addl_data,"00");
		 
	  	}  Removing the code to send De23 in 0100 and 0400 for MC */

	  /*
	   * For RPS acquiring transaction build SE22SF05 only for transactions coming from
	   * DCPISO. For other handlers queue build only SE22 SF06
	   */
		if((0    == strncmp(p_auth_tx->TLF01_details.message_type,"0100",4) ||
		    0    == strncmp(p_auth_tx->TLF01_details.message_type,"0200",4) ||
		    0    == strncmp(p_auth_tx->TLF01_details.message_type,"0400",4)) &&
		    0 == strncmp(p_auth_tx->TLF01_details.pos_condition_code,"08",2))
		{	//DE48T22SF05
			if(0 == strncmp( p_auth_tx->TLF01_details.handler_queue,"dcp",3))
			{

				itemp += 17;
				strncat( HOST_MESSAGE.addl_data, "22130504M1010601A" ,17);

				/* APR MNDT 2024 MC-8: DE48TG37SF05 */
				if (0 != strncmp(p_auth_tx->industry_code, "500", 3))
				{
					itemp += 19;
					strncat( HOST_MESSAGE.addl_data, "37150511" ,8);
					Rj_with_lead_zeros(mc_merchnt_pgway_id, 11);
					strncat( HOST_MESSAGE.addl_data, mc_merchnt_pgway_id ,11);
					/* store here to display in Tranlookup */
					strncpy(p_auth_tx->TLF01_details.dbrefund_amount, mc_merchnt_pgway_id, 11);
				}

			}
			else if(0 == strncmp( p_auth_tx->TLF01_details.handler_queue,"VOICE",5))
			{
				strncat( HOST_MESSAGE.addl_data, DE48_TAG22_SF06 ,9);
				itemp += 9;
			}
			else if(0 == strncmp( p_auth_tx->TLF01_details.handler_queue,"dcpos",5))
			{
				strncat( HOST_MESSAGE.addl_data, DE48_TAG22_SF06 ,9);
				itemp += 9;
			}

		}

		if ((0 == strncmp(p_auth_tx->TLF01_details.message_type,"0100",4))&&
			(0 == strncmp(p_auth_tx->TLF01_details.handler_queue,"ncmcrd",6)||
			 0 == strncmp(p_auth_tx->TLF01_details.handler_queue,"ncmcrd3",7)))
		{
			strncat( HOST_MESSAGE.addl_data, DE48_TAG80 ,4);
			strncat( HOST_MESSAGE.addl_data, p_auth_tx->TLF01_details.agent_id_len ,2);
			itemp += 6;
		}
       /*
        * For all types of transactions other than RPS transactions coming from dcpiso, dcpos and VOICE build only SE22 SF06.
        * RPS transactions are handled in the above case.
        */
		if((0 == strncmp( p_auth_tx->TLF01_details.handler_queue,"dcpiso",6) ||
				0 == strncmp( p_auth_tx->TLF01_details.handler_queue,"dcpos",5) ||
				0 == strncmp( p_auth_tx->TLF01_details.handler_queue,"VOICE",5))	&&
		   (0    == strncmp(p_auth_tx->TLF01_details.message_type,"0100",4) ||
		    0    == strncmp(p_auth_tx->TLF01_details.message_type,"0200",4) ||
		    0    == strncmp(p_auth_tx->TLF01_details.message_type,"0400",4))&&
			0 != strncmp(p_auth_tx->TLF01_details.pos_condition_code,"08",2))
		{
			strncat( HOST_MESSAGE.addl_data, DE48_TAG22_SF06 ,9);
			itemp += 9;

		}

	 /* Place MasterCard Assigend IDs in DE48 SE 32 feild and send it to Cadencie*/
	 if((strlen(p_auth_tx->MCF01_details.network[2].priority_routing) > 0 ) &&(strlen(p_auth_tx->MCF01_details.network[2].priority_routing) < 7 ) )
	  {	
	  	 /* MasterCard Assigned ID should be Alpha numeric only*/
	  	if(true==isalphanum(p_auth_tx->MCF01_details.network[2].priority_routing, 6))//N -> 8
		{
			  itemp += 10;
			 strcat( HOST_MESSAGE.addl_data, DE48_TAG32);
			 strcat (HOST_MESSAGE.addl_data,p_auth_tx->MCF01_details.network[2].priority_routing);
		}
		 
	  }
	  /*
	    Acquring Transaction
	    TF - Abhishek For Retai/Sale transaction and Processing code <> 38, 30, DE48 Subelement 61
		subfield 5 = 1 (Final Authorization). This is done for April 2016 mandated changes.
		In dcpiso we are storing original key for card verification in auth_tx.mac_key[0] filed.
	  */
	 if(	AUTH_SALE  == p_auth_tx->TLF01_details.tx_key ||
			 AUTH_ADVICE == p_auth_tx->TLF01_details.tx_key ||
			 AUTH_AUTHORIZATION == p_auth_tx->TLF01_details.tx_key ||
			 AUTH_CASH_ADVANCE == p_auth_tx->TLF01_details.tx_key
	 		 )
	{

		 /* identifier for final authorization on DE48 SE61 which should have value "00001"
		  * For cash advance transaction, requested by marvin, 9.7 1st release
		  * */
		if((0 == strncmp( p_auth_tx->TLF01_details.handler_queue,"dcp",3)) ||
				((0 == strncmp( p_auth_tx->TLF01_details.handler_queue,"VOICE",5))))
		 {
			if( (0 != strncmp(p_auth_tx->TLF01_details.processing_code,"30",2))&&
				(0 != strncmp(p_auth_tx->TLF01_details.processing_code,"38",2))&&
				(0 != strncmp(p_auth_tx->TLF01_details.message_type,"04",2))&&
				( p_auth_tx->mac_key[0] != AUTH_CARD_VERIFICATION))
				{
					itemp += 9;
					if(true==isalphanum(p_auth_tx->MCF01_details.network[2].priority_routing, 6))//N -> 8
					{
						strcat( HOST_MESSAGE.addl_data, "610500101");//
						memcpy( p_auth_tx->TLF01_details.product_codes[13].amount,
								"610500101", 9 );
					}
					else
					{
						strcat( HOST_MESSAGE.addl_data, DE48_TAG61);//
						memcpy( p_auth_tx->TLF01_details.product_codes[13].amount,
								DE48_TAG61, 9 );
					}
				}
		 }
	}
   }

   /* Calculate total length of field 48. */
   if ( itemp > 0 )
   {
      /* Do not 'sprintf' directly into addl_data_len.
       * Null would overwrite a piece of data.
       */
      sprintf( temp_str3, "%03d", itemp );
      memcpy( HOST_MESSAGE.addl_data_len, temp_str3, 3 );
   }

   /* Save Transaction category code in TLF01.
    * Use remote_nsp_type field, which is not being used, to
    * avoid adding another field to TLF01.
    */
   p_auth_tx->TLF01_details.remote_nsp_type[0] = HOST_MESSAGE.addl_data[0];


   /* FIELD 49: TRANSACTION CURRENCY CODE */
   if ( SWITCHED_IN == Tran_Flow )
   {
      /*+-------------------------------------------------------------------+
        | This is a response to a request from the host.                    |
        | Echo the orig value.  However, the original value can be stored   |
        | in reconciliation_cur_code if it is not 608 or 840.               |
        +-------------------------------------------------------------------+*/
      if ( 0 != strcmp( NULL_STR, p_auth_tx->reconciliation_cur_code ) )
      {
         memcpy( HOST_MESSAGE.tran_curr_cd,
                 p_auth_tx->reconciliation_cur_code,
                 bf49_len );
		if ( 0 != strcmp( NULL_STR, p_auth_tx->reconciliation_amount ) )
         memcpy( HOST_MESSAGE.amount,
                 p_auth_tx->reconciliation_amount,
                 bf4_len );
      }
      else
      {
        if ( 0 != strncmp( p_auth_tx->TLF01_details.nfi_seq_nbr, p_auth_tx->TLF01_details.currency_code,3 ) )
		{memcpy( HOST_MESSAGE.tran_curr_cd,
                 p_auth_tx->TLF01_details.nfi_seq_nbr,
                 bf49_len );
		memcpy(  p_auth_tx->TLF01_details.currency_code,
                 p_auth_tx->TLF01_details.nfi_seq_nbr,
                 bf49_len );
		}
		else 
		{	memcpy( HOST_MESSAGE.tran_curr_cd,
                 p_auth_tx->TLF01_details.currency_code,
                 bf49_len );
		}
      }
   }
   else
   {
      /* This is a request to the host. */
      strncpy( HOST_MESSAGE.tran_curr_cd, 
               p_auth_tx->MCF01_details.currency_code,
               bf49_len );
   }
   Rj_with_lead_zeros( HOST_MESSAGE.tran_curr_cd, bf49_len );

   /* Save in these fields so switched in and switched out db values match. */
   if ( p_auth_tx->TLF01_details.currency_code[0] == 0x00 )
      memcpy( p_auth_tx->TLF01_details.currency_code,
              HOST_MESSAGE.tran_curr_cd, bf49_len );

   if ( p_auth_tx->TLF01_details.nfi_seq_nbr[0] == 0x00 )
      memcpy( p_auth_tx->TLF01_details.nfi_seq_nbr,
              HOST_MESSAGE.tran_curr_cd, bf49_len );


   /* FIELD 50: SETTLEMENT CURRENCY CODE */
   if ( 0 != strcmp( NULL_STR, p_auth_tx->settlement_curr_code ) )
   {
      memcpy( HOST_MESSAGE.settle_cur_cd,
              p_auth_tx->settlement_curr_code, bf50_len );
      Rj_with_lead_zeros( HOST_MESSAGE.settle_cur_cd, bf50_len );
   }

   /* FIELD 51: CARDHOLDER BILLING CURRENCY CODE */
   if ( 0 != strcmp( NULL_STR, p_auth_tx->ch_billing_curr_code ) )
   {
      memcpy( HOST_MESSAGE.card_curr_cd,
              p_auth_tx->ch_billing_curr_code, bf51_len );
      Rj_with_lead_zeros( HOST_MESSAGE.card_curr_cd, bf51_len );
   }

   /* FIELD 52: PIN BLOCK */
   if ( p_auth_tx->TLF01_details.pin_block[0] != 0x00 )
   {
      memset( temp_str, 0, sizeof(temp_str) );
      ascii_to_bin( p_auth_tx->TLF01_details.pin_block,
                    strlen(p_auth_tx->TLF01_details.pin_block),
                    &temp_str[0], sizeof(temp_str));

      memcpy( HOST_MESSAGE.pinblock, temp_str, sizeof(HOST_MESSAGE.pinblock) );
   }
 /* FIELD 53: Addl Security Information .. TF PHANI*/ 
    if (true == hhutil_verify_pin_block(p_auth_tx))
   {
	  if(0 != strcmp( NULL_STR, p_auth_tx->mac_key ))
	  {memcpy( HOST_MESSAGE.sec_cntl,p_auth_tx->mac_key,
		        sizeof(HOST_MESSAGE.sec_cntl));
	  }
   }

   /* FIELD 54: ADDITIONAL AMOUNTS - CARDHOLDER BALANCE */
   if ( p_auth_tx->TLF01_details.tx_key == AUTH_BALANCE_INQUIRY_RESPONSE )
   {
      /* This field is only pertinent for Balance Inquiry. */
      if ( 0 == strcmp(p_auth_tx->TLF01_details.response_code, APPROVED) )
      {
         /* It is approved. Supply balance information if it exists. */
         if ( p_auth_tx->ACF01_details.available[0] != 0x00 )
         {
            memset( temp_str, 0x00, sizeof(temp_str) );

            /* Account Type - from processing code bytes 3 & 4 */
            memcpy( temp_str, p_auth_tx->TLF01_details.processing_code+2, 2 );

            /* Amount Type - always using Ledger Balance (01) */
            memcpy( temp_str+2, "01", 2 );

            /* Currency Code - should be 608 or 840; if neither, use 840. */
            if ( 0 == strncmp(HOST_MESSAGE.tran_curr_cd, PESO, 3) )
               memcpy( temp_str+4, PESO, 3 );
            else
               memcpy( temp_str+4, USD, 3 );

            /* Amount: (C)redit or (D)ebit */
            if ( p_auth_tx->available_balance_sign == '+' )
               memset( temp_str+7, 'C', 1 );
            else
               memset( temp_str+7, 'D', 1 );

            /* Amount */
            memcpy( temp_str+8, p_auth_tx->ACF01_details.available, 12 );

            itemp = strlen( temp_str );
            sprintf( HOST_MESSAGE.addl_len, "%03d", itemp );

            memcpy( HOST_MESSAGE.addlamts, temp_str, itemp );
         }
      }
   }

   /* FIELD 55 .Integrated Circuit Card (ICC) System-related Data */
	if(strcmp(p_auth_tx->TLF01_details.product_codes[1].code,STANDIN)==0 )
	{
		BYTE str_itemp[5]={0};
			  /* Convert length to BCD. */
			  memset( str_itemp,   0x00, sizeof(itemp)   );
			 
			  memcpy( str_itemp+1, p_auth_tx->EMV_details.future_field1, 3 );
			  str_itemp[0] = '0';
			  INT i = atoi( str_itemp );
			 memcpy( HOST_MESSAGE.iccdata_len, p_auth_tx->EMV_details.future_field1, 3 );
			 memcpy( HOST_MESSAGE.iccdata, p_auth_tx->EMV_details.future_field1+3, i );
	}
	else if(strncmp(p_auth_tx->TLF01_details.response_code,"00",2) != 0 && SWITCHED_IN == Tran_Flow )
	{
		memset( HOST_MESSAGE.iccdata_len, 0x00, sizeof(HOST_MESSAGE.iccdata_len) );
		memset( HOST_MESSAGE.iccdata, 0x00 , sizeof(HOST_MESSAGE.iccdata) );
	}
    else if (p_auth_tx->EMV_details.emv_block[0] != 0x00 || p_auth_tx->EMV_details.emv_block[1] != 0x00 ) // TF Phani Check if second byte has something..first 2 bytes has length
    {
      /* Determine the block size */
      emv_len = genutil_bcd_to_int( p_auth_tx->EMV_details.emv_block, 2 );

      /* Copy only the EMV Tags that MC accepts. */
      get_MC_EMV_data( emv_len, p_auth_tx->EMV_details.emv_block );
	}

	  /* FIELD 56 :PAYMENT ACCOUNT DATA  */


   /* FIELD 60: ADVICE REASON CODE */
   strncpy( HOST_MESSAGE.advice_cd_len, "003", bf60_len );
   strcpy(  HOST_MESSAGE.advice_code,   "401"           );

   /* FIELD 61: POINT OF SERVICE DATA */
   strncpy( HOST_MESSAGE.pos_data_len, "026", bf61_len );
   memset(  HOST_MESSAGE.pos_data, 0x30, 26 );
	memset( temp_str3, 0x00, sizeof(temp_str3) );
   memcpy( temp_str3, HOST_MESSAGE.posentry, 3 );
   if ( e_commerce )
   {
      strcpy( HOST_MESSAGE.pos_data, "1025100006600608" );

      /* If merchant supports UCAF (see DE48, tag 42), then
       * set position four to '5' and position ten to '6'.
       * It doesn't matter if DE48.43 is populated.
       * Right now, no Equitable merchants support UCAF, so
       * leave these values as they are set above.
       */
   }

   else if ( ('Y' == p_auth_tx->TLF01_details.voice_auth[0]) ||
             ('y' == p_auth_tx->TLF01_details.voice_auth[0])   )
      strcpy( HOST_MESSAGE.pos_data, "2031100000600608" );

   else
   {
   	  /*RPS*/
	   if (p_auth_tx->MCF01_details.rps_merchant[0]=='Y' || iIndustry == 800 ||
		   ((0 == strncmp(temp_str3,"01",2) || 0 == strncmp(temp_str3,"10",2) )&&
			(0 == strncmp(p_auth_tx->TLF01_details.pos_condition_code,"08",2))))
	   {
			 strcpy( HOST_MESSAGE.pos_data, "2034100000100608" );
	   }
	   else if ((0 == strncmp(temp_str3, "01", 2) ) &&
			   (0 == strncmp(p_auth_tx->TLF01_details.pos_condition_code,"00",2)))
	   {/* For manula entry transactions - 61.5 should be 1 - TF Phani*/
	   		 
			 strcpy( HOST_MESSAGE.pos_data, "0001110000300608" );
	   }
	   else if( 0 == strncmp(temp_str3, "91", 2) ||
	   			0 == strncmp(temp_str3, "07", 2))
	   {
		   /* Contact less*/
		   strcpy( HOST_MESSAGE.pos_data, "0000010000300608" );
	   }

	   else if(0 == strncmp(temp_str3, "05", 2) ||
	   			0 == strncmp(temp_str3, "90", 2) ||
				0 == strncmp(temp_str3, "80", 2))

	   {/* chip, contact, mag stripe */
			strcpy( HOST_MESSAGE.pos_data, "0000010000300608" );
	   }
	   else if((0 == strncmp(temp_str3, "01", 2) || (0 == strncmp(temp_str3, "10", 2)))&&
			   (0 == strncmp(p_auth_tx->TLF01_details.pos_condition_code,"05",2)))
	   {
		   /* BDO PAY QR */
		   strcpy( HOST_MESSAGE.pos_data, "2031100001000608" );
	   }
	   else if(iIndustry == 500 )
	   {	/* MOTO */
	   		strcpy( HOST_MESSAGE.pos_data, "2032100000000608" );
	   }
	   else
	   { /* chip, contact, mag stripe */
			strcpy( HOST_MESSAGE.pos_data, "0000010000300608" );
	   }
   }

	/*
		TF - Abhishek For Card Ver/ Pre-Auth and Processing code 38, 30, DE61 Subfield 7 
		shoud be 4. This is done for April 2016 mandated changes.
		In dcpiso we are storing original key for card verification in auth_tx.mac_key[0] filed.
	*/
	    if( (0 == strncmp(p_auth_tx->TLF01_details.processing_code,"30",2))||
		    (0 == strncmp(p_auth_tx->TLF01_details.processing_code,"38",2))||
			( p_auth_tx->mac_key[0] == AUTH_CARD_VERIFICATION) ||
			(p_auth_tx->TLF01_details.tx_key == AUTH_VOID_PRE_AUTHORIZATION) ||
			(0 == strncmp(p_auth_tx->TLF01_details.product_codes[3].quantity,"PR",2)))
			{
				HOST_MESSAGE.pos_data[6]  = '4';
			}
	
#if BDOR_68_REQ_1_MC
	    if ((true == ncmcrd_check_if_trx_is_mc_MobilePOS_NFC_Tap2Phone_trx_with_pin( p_auth_tx ))||
			(true == ncmcrd_check_if_trx_is_mc_MobilePOS_NFC_Tap2Phone_trx_without_pin( p_auth_tx )))
		 {
	    	HOST_MESSAGE.pos_data[9] = MC_CARDHOLDER_ACTIVATED_TERMINAL_LEVEL;
		 }
#endif

#if BDOR_68_REQ_3_MC   /* Swiftpass BDO Pay Credit */
	    if(true == ncmcrd_check_if_trx_is_mc_Swiftpass_BDO_Pay_credit_trx_with_pin(p_auth_tx))
	    {
	    	strncpy( HOST_MESSAGE.pos_data,
	    			 MC_SWIFTPASS_BDO_PAY_CREDIT_TXN_POS_DATA,
					 MC_POS_DATA_LEN );
	    }
	    else if(true == ncmcrd_check_if_trx_is_mc_Swiftpass_BDO_Pay_credit_trx_without_pin(p_auth_tx))
	    {
	    	strncpy( HOST_MESSAGE.pos_data,
	    			 MC_SWIFTPASS_BDO_PAY_CREDIT_TXN_POS_DATA,
					 MC_POS_DATA_LEN );
	    }
#endif
#if BDOR_68_REQ_5_MC /*Ecommerce Entry Mode From Swiftpass BDOPay Transactions*/

    else if(true == ncmcrd_check_if_trx_is_mc_ecommerce_swiftpass_BDOPay_trx (p_auth_tx))
    {
	   strncpy( HOST_MESSAGE.pos_data,
			    MC_ECOMMERCE_SWIFTPASS_BDOPAY_TXN_POS_DATA,
				MC_POS_DATA_LEN );
   }
#endif
   /* ----------------------------------------------------- */
   /* POS Card Data Terminal Input Capability = Field 61.11 */
   /* ----------------------------------------------------- */

   /********************************************************************
    *
    *  Set field 61.11 based on field 22 as follows. The
    *  value of this field is NOT dependent on terminal type.
    *
    *  +----------------+-------------+-----------------------------+
    *  | POS Entry Mode | Field 61.11 | Description (61.11)         |
    *  +----------------+-------------+-----------------------------+
    *  |    05X         |     3       | Mag stripe, key entry, chip |
    *  |    01X         |     3       |                             |
    *  |    80X         |     3       |                             |
    *  |    90X         |     3       |                             |
    *  |    072         |     3       | Contactless M/Chip          |
    *  |    912         |     3       |                             |
    *  +----------------+-------------+-----------------------------+
    *
    ********************************************************************
    */

   
 /* can remove from here to
   if ( (0 == strncmp(temp_str3, "90", 2)) || 
        (0 == strncmp(temp_str3, "05", 2)) ||
        (0 == strncmp(temp_str3, "01", 2)) ||
        (0 == strncmp(temp_str3, "80", 2))
      )
   {
      // Mag stripe, key entry, and EMV-Compatible chip reader
	  if( ((HOST_MESSAGE.pos_data[3] == '0') && (HOST_MESSAGE.pos_data[4] == '1'))) 
	  {
	    // To avoid 8.h edit error
      	//HOST_MESSAGE.pos_data[10]  = '7';
	  }
	  else if( (0 == strncmp(p_auth_tx->TLF01_details.pos_condition_code, "59",2))||
			   ('Y' == p_auth_tx->TLF01_details.voice_auth[0])||
			   ('y' == p_auth_tx->TLF01_details.voice_auth[0])||
			   (iIndustry == 800) ||
			   (p_auth_tx->MCF01_details.rps_merchant[0]=='Y'))
	  {
		  // Do nothing
	  }
	  else
	  {
		  //HOST_MESSAGE.pos_data[10]  = '8';
	  }
   }
   else if ( (0 == strcmp (temp_str3, "072"  )) ||
             (0 == strcmp (temp_str3, "912"  )) ||
			 (0 == strncmp(temp_str3, "07",2 )) ||
			 (0 == strncmp(temp_str3, "911",3 ))
           )
   {
      // Contactless M/Chip
      //HOST_MESSAGE.pos_data[10]  = '3';
   } 
    else if (  (0 == strncmp (temp_str3, "91",2  ))    )
   {
      // Contactless M/Chip
     // HOST_MESSAGE.pos_data[10]  = '4';
   } 
    // Check for Ecomerce and RPS , 61.11 should be "0" .. TF Phani
	if(0 == strncmp(p_auth_tx->TLF01_details.pos_condition_code,"08",2))
	{
		// HOST_MESSAGE.pos_data[10]  = '0';
	}


   // Edit error 8.f check
   if ( (0 == strncmp(temp_str3, "90", 2)) || 
        (0 == strncmp(temp_str3, "02", 2)))
   {
		if(HOST_MESSAGE.pos_data[10]=='6')
		{
			//HOST_MESSAGE.pos_data[10]  = '7';
		}
   }

   // Edit error 8.i check
   if ( (0 == strncmp(temp_str3, "90", 2)) || 
        (0 == strncmp(temp_str3, "05", 2)) ||
        (0 == strncmp(temp_str3, "02", 2)) ||
        (0 == strncmp(temp_str3, "80", 2))
      )
   {

		//HOST_MESSAGE.pos_data[4]  = '0';
   }

   // Edit error 8.e check
   if ( (0 == strncmp(temp_str3, "00", 2)))
   {
		if((HOST_MESSAGE.pos_data[10]=='2')||
		   (HOST_MESSAGE.pos_data[10]=='5')||
		   (HOST_MESSAGE.pos_data[10]=='7')||
		   (HOST_MESSAGE.pos_data[10]=='8'))
		{
			//HOST_MESSAGE.pos_data[10]  = '0';
		}
   }
 
   // Edit error 8.d check
   if ( (0 == strncmp(temp_str3, "00", 2))||
   		(0 == strncmp(temp_str3, "01", 2)))
   {
		if(HOST_MESSAGE.pos_data[10]=='1')
		{

			//HOST_MESSAGE.pos_data[0]='2';
			//HOST_MESSAGE.pos_data[2]='3';
		}
   }
   // Edit error 3.d and 3.e
   if((0 == strncmp(temp_str3, "80", 2))||
      (0 == strncmp(temp_str3, "90", 2))||
   	  (0 == strncmp(temp_str3, "91", 2)))
   {
		// Check if DE45 and DE35 is present or not
		if((strlen(HOST_MESSAGE.track2) > 0)||
		   (strlen(HOST_MESSAGE.track1) > 0))
		{
			//HOST_MESSAGE.pos_data[3]  = '0';
			if(HOST_MESSAGE.pos_data[4]  == '1')
			{
				//HOST_MESSAGE.pos_data[4]  = '0';
			}
		}
   }
    can remove from here to */

   /* Blank fill whatever zip code does not fill. */
   strncpy( &HOST_MESSAGE.pos_data[16], p_auth_tx->MCF01_details.zip_code,10);
   itemp = strlen( p_auth_tx->MCF01_details.zip_code );
   memset( &HOST_MESSAGE.pos_data[16+itemp], 0x20, 10 - itemp );

   /* For automated fuel dispenser transactions, subfield 10 must be '2' */
   if ( 0 == strncmp( AUTO_FUEL_MCC, p_auth_tx->TLF01_details.category_code,4 ))
   {
      /* Industry code 600 should take care of this,
       * but just in case, do it here, else MasterCard charges a fee.
       */
      //memset( &HOST_MESSAGE.pos_data[9], 0x32, 1 );
   }
   // copy POS data in AUTH->acceptor_term_name
	if(ncmcrd_Check_Reversal_Advice_Iss_txns ( p_auth_tx ) == true)
	{
		// Don't change the POS data for advice and reversal incoming txn.
	}
	else
	{
		strncpy(p_auth_tx->acceptor_term_name, HOST_MESSAGE.pos_data,25);
	}

	if((0 == strncmp( p_auth_tx->TLF01_details.handler_queue,"dcp",3)) ||
	   (0 == strncmp( p_auth_tx->TLF01_details.handler_queue,"VOICE",5)))
	{
		strncpy(p_auth_tx->TLF01_details.product_codes[16].amount, HOST_MESSAGE.pos_data,12);
	}

   /* FIELD 62:  INTERMEDIATE NETWORK FACILITY (INF) DATA */
	if ( SWITCHED_IN == Tran_Flow )  /* only for switch in transactions */
   {
		itemp = strlen( p_auth_tx->int_net_facility_data );
		if ( itemp > 0 )
		{
			sprintf( HOST_MESSAGE.inf_data_len, "%03d", itemp );
			memcpy( HOST_MESSAGE.inf_data, p_auth_tx->int_net_facility_data, itemp );
		}
	}
   /* FIELD 63: BANKNET */
	if((0== strncmp(p_auth_tx->TLF01_details.handler_queue,"dcpiso",6)) ||
	   (0== strncmp(p_auth_tx->TLF01_details.handler_queue,"VOICE",5)) )
	{
		/*For Acquring transaction De63 should not be posted to MC. Its a Issuer Field*/
	}
	else if ( p_auth_tx->TLF01_details.mcard_banknet[0] != 0x00 )
	{
		itemp = strlen( p_auth_tx->TLF01_details.mcard_banknet );
		sprintf( HOST_MESSAGE.banknet_len, "%03d", itemp );
		strncpy( HOST_MESSAGE.banknet,
				p_auth_tx->TLF01_details.mcard_banknet,
				itemp );
	}

   /* FIELD 70: NETWORK MANAGEMENT INFORMATION CODE */
   strncpy( HOST_MESSAGE.nmi_code, Logon_NMI_Code, bf70_len );
   strcpy( Logon_NMI_Code, NULL_STR );  // Reset for next transaction

   /* FIELD 90: ORIGINAL DATA ELEMENTS - Used for Reversals. */

   /* BDO wants send void and reversal txn to MC Host 30 Oct,2009 TF-Ajay 
     +-------------------------------------------------+
     | BYTES  1- 4:  Original Message Type => '0100'   |
     | BYTES  5-10:  STAN of Original message          |
     | BYTES 11-20:  Trans date & time of original msg |
     | BYTES 21-31:  Acquiring Institution Code        |
     | BYTES 32-42:  Forwarding Institution Code = '0' |
     +-------------------------------------------------+
   */
   memset(orig_data, '0', bf90_len );

   if( (p_auth_tx->TLF01_details.tx_key == AUTH_REVERSAL_RESPONSE ||
		p_auth_tx->TLF01_details.tx_key == AUTH_REVERSAL_ADVICE_RESPONSE) &&
		(strlen(p_auth_tx->TLF01_details.dcf01_retired_cwk)> 0 && strlen(p_auth_tx->TLF01_details.dcf01_retired_cwk) < 43 )
	 )
   {
	  memcpy(orig_data, p_auth_tx->TLF01_details.dcf01_retired_cwk,42);
   }
   else
   {
	   strncpy( orig_data, "0100", 4 );
	   if( p_auth_tx->TLF01_details.orig_sys_trace_num[0]!= '\0' )
	   {
		   memcpy(&orig_data[4], p_auth_tx->TLF01_details.orig_sys_trace_num, 6 );
	   }
	   else
	   {
	   memcpy(&orig_data[4], p_auth_tx->TLF01_details.sys_trace_audit_num, 6 );
	   }


	   /* Time of original tx (MMDDhhmmss). Use the RRN (YYMMDDhhmmss).
	   memset( temp_str,  0x00, sizeof( temp_str  ) );
	   memset( temp_str2, 0x00, sizeof( temp_str2 ) );
	   strcpy( temp_str,  p_auth_tx->TLF01_details.retrieval_ref_num );
	   strcpy( temp_str2, &temp_str[2] );
	   memcpy( &orig_data[10], temp_str2, 10 );*/

	  /* Time of original tx (MMDDhhmmss). */
	   if(0    == strncmp(p_auth_tx->TLF01_details.product_codes[3].quantity,"PR",2) &&
		  '\0' != p_auth_tx->orig_local_time_date[0])
	   {
		   memcpy( &orig_data[10], p_auth_tx->orig_local_time_date, 10);
	   }
	   else
	   {
	   memcpy( &orig_data[10], p_auth_tx->TLF01_details.transmission_timestamp, 10);
	   }


	  /* Forward ist iD or Acquiring Institution ID Code */

	   memset( temp_str, 0x00, sizeof( temp_str ) );
	   strncpy( temp_str, ncf01_i.acquirer_inst_id_code, bf32data_len );

	   Rj_with_lead_zeros( temp_str, bf32data_len );
	   strncpy( &orig_data[20], temp_str, bf32data_len );

	   // F ID
	   memset( temp_str, 0x00, sizeof( temp_str ) );
	   strncpy( temp_str, ncf01_i.forwarding_inst_id_code, bf33data_len );

	   Rj_with_lead_zeros( temp_str, bf33data_len );
	   strncpy( &orig_data[31], temp_str, bf33data_len );
   }

   strncpy( HOST_MESSAGE.orig_data, orig_data, bf90_len );

   /* FIELD 91: FILE UPDATE CODE */
   if ( AUTH_CARD_UPDATE == p_auth_tx->TLF01_details.tx_key )
   {
      switch( Mcc_Gui.choice )
      {
         case CHOICE_ADD:     HOST_MESSAGE.file_code[0] = '1';  break;
         case CHOICE_UPDATE:  HOST_MESSAGE.file_code[0] = '2';  break;
         case CHOICE_DELETE:  HOST_MESSAGE.file_code[0] = '3';  break;
         case CHOICE_INQUIRE: HOST_MESSAGE.file_code[0] = '5';  break;
         default:             HOST_MESSAGE.file_code[0] = Mcc_Gui.choice; break;
      }
      p_auth_tx->TLF01_details.nii[0] = Mcc_Gui.choice; // For logging to TLF01
   }

   /* FIELD 94: SERVICE INDICATOR */
   strncpy( HOST_MESSAGE.service_ind, "0B0    ", bf94_len ); /* TF PHANI.. Value changed from 'CB0' to '0B0' */

   /* FIELD 95: SERVICE INDICATOR */
   if(0 == strncmp(p_auth_tx->TLF01_details.product_codes[3].quantity,"PR",2))
   {
	   // We need to store  the OFFLINE sale amount
	   memset( HOST_MESSAGE.replace_amt,'0', 42 );
	   strncpy( HOST_MESSAGE.replace_amt,p_auth_tx->TLF01_details.total_amount,12);
   }

#if BDOR_62_PREAUTH_VOID
   /* FIELD 95: REPLACEMENT AMOUNT */
   if (AUTH_VOID_PRE_AUTHORIZATION == p_auth_tx->TLF01_details.tx_key)
   {
	   memset(replace_amt, 0x00, sizeof(replace_amt)- 1);

	   /* SF1 Actual Amount, transaction */
	   if (strlen(p_auth_tx->TLF01_details.total_interest) == 0)
	   {
		   strncpy(p_auth_tx->TLF01_details.total_interest,
				   	   p_auth_tx->TLF01_details.orig_amount, 12);
	   }
	   remain_amt = atol(p_auth_tx->TLF01_details.total_interest) - atol(p_auth_tx->TLF01_details.total_amount);

	   if (p_auth_tx->TLF01_details.saf[0] == INCREMENTAL_AUTH)
	   {
		   sprintf(replace_amt, "%012d", remain_amt);
	   }
	   else
	   {			/* PRIMARY_AUTH	*/
		   strncpy(replace_amt, "000000000000", 12);
	   }


	   /* SF2 Settlement Amount */
	   strncat(replace_amt, "000000000000", 12);

	   /* SF3 Billing Amount */
	   strncat(replace_amt, "000000000000", 12);

	   /* SF4 Fill with zeros */
	   strncat (replace_amt, "000000", 6);

	   memcpy(HOST_MESSAGE.replace_amt, replace_amt, 42);
   }
#endif

   /* FIELD 96: */
   if ( AUTH_CARD_UPDATE == p_auth_tx->TLF01_details.tx_key		&&
	    strncmp(p_auth_tx->TLF01_details.originator_queue,"ncmcr2A",4)==0
     )
   {
	strncpy( HOST_MESSAGE.security_code, Mcc_Gui.err_msg3,8 );
   }
   else
   strncpy( HOST_MESSAGE.security_code, "00000000", bf96_len );


   /* FIELD 100: RECEIVING INSTITUTION ID CODE */
   strncpy( HOST_MESSAGE.recv_len, recv_inst_len, bf100_len );
   Rj_with_lead_zeros( HOST_MESSAGE.recv_len, bf100_len );
   strncpy( HOST_MESSAGE.recvid,   recv_inst, atoi(recv_inst_len) );

   /* FIELD 101: FILE NAME */
   memcpy( HOST_MESSAGE.filename_len, "06", 2 );
   memcpy( HOST_MESSAGE.filename, Mcc_Gui.msg_type, 6 );
/* Installment data */
/* TF Phani 
	MC expects below data in DE112, 
	DE112 sub element 009(Contents of subfields 1-5):
	1.  Type of Installment 1-2             - value 21 (For PHILIPPINES merchant finance)
	2.  Number of Installments 3-4     - value (Incoming message)
	3.  Interest Rate 5-9                       - value Interest Rate contains the (two-decimal) interest rate that the issuer charges the cardholder for the installment payments. Default rate is zero percent (00000).
    	4.  First Installment Amount 10-21 First Installment Amount contains the amount of the first installment
       5.  Subsequent Installment Amount 22-33 Subsequent Installment Amount contains the amount of the subsequent installment.
*/

   /* FIELD 105: TRANSACTION LINK IDENTIFIER */
#ifdef MC01_APR2024
   if(p_auth_tx->TLF01_details.tx_key > AUTH_INVALID &&
	  p_auth_tx->TLF01_details.product_codes[19].amount[0] != 0x00)
   {
	   memcpy( HOST_MESSAGE.tranlink_len, "028", 3 );
	   strncpy( HOST_MESSAGE.tranlink_data, "001022", 6 );
	   strncat( HOST_MESSAGE.tranlink_data, p_auth_tx->TLF01_details.product_codes[19].amount, 10 );
	   strncat( HOST_MESSAGE.tranlink_data, p_auth_tx->TLF01_details.product_codes[1].amount, 12 );
   }
#endif

#ifdef MC02_APR2024
   if(((0== strncmp(p_auth_tx->TLF01_details.handler_queue, "dcpiso", 6)) ||
	   (0== strncmp(p_auth_tx->TLF01_details.handler_queue, "VOICE",  5)) ||
	   (0== strncmp(p_auth_tx->TLF01_details.handler_queue, "ncmcr2", 6))) 	&&
	  (p_auth_tx->TLF01_details.tx_key == AUTH_PRE_AUTHORIZATION   	||
	   p_auth_tx->TLF01_details.tx_key == AUTH_REVERSAL  		  	||
	   p_auth_tx->TLF01_details.tx_key == AUTH_VOID_SALE 			||
	   p_auth_tx->TLF01_details.tx_key == AUTH_REFUND 				||
	   p_auth_tx->TLF01_details.tx_key == AUTH_VOID_PRE_AUTHORIZATION	||
	   (strncmp(p_auth_tx->TLF01_details.product_codes[3].quantity, "PR", 2) == 0)) &&
	  (p_auth_tx->TLF01_details.product_codes[10].amount[0] != '\0')
	 )
   {
	   memcpy( HOST_MESSAGE.tranlink_len, "028", 3 );
	   strncpy( HOST_MESSAGE.tranlink_data, "001022", 6 );
	   strncat( HOST_MESSAGE.tranlink_data, p_auth_tx->TLF01_details.product_codes[10].amount, 10 );
	   strncat( HOST_MESSAGE.tranlink_data, p_auth_tx->TLF01_details.product_codes[1].amount, 12 );
   }
#endif

   /*FIELD 108:(Additional Transaction Reference Data) */
#ifdef MC07_OCT2023


   if(p_auth_tx->MCF01_details.credit_purch_max[0]!='\0')
	{
		strncpy( HOST_MESSAGE.de108,DE108_SE07,2);
		/*
		* 0060102(006:total sub element length,01:Subfield ID,02:Subfield Length)
		*/
		strncat(HOST_MESSAGE.de108,"0060102",7);
		strncat( HOST_MESSAGE.de108,p_auth_tx->MCF01_details.credit_purch_max,2);
	}
    if(MC_ASI_TXN_IDENTIFIER_VALUE == p_auth_tx->TLF01_details.product_codes[16].amount[6] &&
		   p_auth_tx->MCF01_details.credit_purch_max[0]!='\0' &&
	  (0==strncmp(p_auth_tx->MCF01_details.credit_purch_max,"10",2) ||
	   0==strncmp(p_auth_tx->MCF01_details.credit_purch_max,"11",2)))

   {
	   /*
	    * If De108 SE07((Data Verification Request) value is 10, then build DE108 SE08 SF1.
	    * else if value is 11,then build DE108 SE08 SF2.
	    */
    	if( 0 == strncmp(p_auth_tx->TLF01_details.product_codes[1].code,"stndin",6))
    	{
    		 if (strncmp(p_auth_tx->TLF01_details.processing_code , "28" ,2) == 0 || name_validation_flag== 'N' )
			 {
				strncat( HOST_MESSAGE.de108,DE108_SE08,2);
				if(0==strncmp(p_auth_tx->MCF01_details.credit_purch_max,"10",2))
				{
				   strncat(HOST_MESSAGE.de108,"0050101",7);
				   strncat(HOST_MESSAGE.de108,"U",1);
				}
				else if(0==strncmp(p_auth_tx->MCF01_details.credit_purch_max,"11",2))
				{
				   strncat(HOST_MESSAGE.de108,"0050201",7);
				   strncat(HOST_MESSAGE.de108,"U",1);
				}
			  }
			  else if(strncmp(p_auth_tx->TLF01_details.processing_code ,"000000",6)==0 &&
					  strncmp(p_auth_tx->TLF01_details.total_amount ,"000000000000",12)==0)
			   {
				   /*
					* Perform name validation for REcurring ASI transactions. Based on the Name validation.
					* The Valid values of DE108 SE 08 are:
					* B: Name validation successful( First and Last name only)
					* C:Name Validation failed
					*/
				   strncat( HOST_MESSAGE.de108,DE108_SE08,2);
				   if(0==strncmp(p_auth_tx->MCF01_details.credit_purch_max,"10",2))
				   {
					   strncat(HOST_MESSAGE.de108,"0050101",7);
					   strncat(HOST_MESSAGE.de108,p_auth_tx->MCF01_details.status,1);
				   }
				   else if(0==strncmp(p_auth_tx->MCF01_details.credit_purch_max,"11",2))
				   {
					   strncat(HOST_MESSAGE.de108,"0050201",7);
					   strncat(HOST_MESSAGE.de108,p_auth_tx->MCF01_details.status,1);
				   }
			   }
    	}
   }

   DE108_len=strlen(HOST_MESSAGE.de108);
   if ( DE108_len > 0 )
   {
      /* Do not 'sprintf' directly into addl_data_len.
       * Null would overwrite a piece of data.
       */
      sprintf( temp_str3, "%03d", DE108_len );
      memcpy( HOST_MESSAGE.de108_len, temp_str3, 3 );
   }

#endif
	/* FIELD 112: (Additional Data, National Use), Subelement
            009�Installment Payment Data */
    if ( SWITCHED_IN == Tran_Flow )  /* only for switch in transactions */
   {
   	/*	if(p_auth_tx->TLF01_details.source_key[0]!= 0x00)
   		{
		strcpy(HOST_MESSAGE.addldata_nation_len,"36");
		memcpy(HOST_MESSAGE.addldata_nation,p_auth_tx->TLF01_details.source_key,DE112_TAG009_LEN);
   		}*/
   }
	else
	{
	   if( p_auth_tx->TLF01_details.deferred_term_length[0]!= 0x00)
	   	{
		//length
		strcpy(HOST_MESSAGE.addldata_nation_len,"36");
		sprintf(def_length,"%d",atoi(p_auth_tx->TLF01_details.deferred_term_length));
	   	// Subelement ID 009
		strcpy(HOST_MESSAGE.addldata_nation,DE112_TAG009);
		// type of installement
		strcat(HOST_MESSAGE.addldata_nation,DE112_TYPE_OF_INST);
		strcat(HOST_MESSAGE.addldata_nation, def_length);
	   //Interest Rate 5�9
		if(p_auth_tx->TLF01_details.deferred_factor[0]!=0x00)
		strcat(HOST_MESSAGE.addldata_nation,p_auth_tx->TLF01_details.deferred_factor);
		else
		strcat(HOST_MESSAGE.addldata_nation,"00000");
		// 1st installement
		//monthly_amort_amt
		strcat(HOST_MESSAGE.addldata_nation,p_auth_tx->TLF01_details.monthly_amort_amt);
		// 2nd installement
		//monthly_amort_amt
		strcat(HOST_MESSAGE.addldata_nation,p_auth_tx->TLF01_details.monthly_amort_amt);
	    }
	}
   /* FIELD 120: RECORD DATA */
   if ( AUTH_CARD_UPDATE == p_auth_tx->TLF01_details.tx_key )
   {
      if ( 0 == strcmp( "MCC102", Mcc_Gui.msg_type ) )
      {
         itemp = 0;

         memset(HOST_MESSAGE.orig_msg_id,0x20,sizeof(HOST_MESSAGE.orig_msg_id));
         itemp = strlen( Mcc_Gui.card_nbr );
         memcpy( &HOST_MESSAGE.orig_msg_id[0],  Mcc_Gui.card_nbr, itemp );
         memset(&HOST_MESSAGE.orig_msg_id[itemp],' ',(19 - itemp));
         itemp += ( 19 - itemp);

         if((Mcc_Gui.choice == CHOICE_ADD) || (Mcc_Gui.choice == CHOICE_UPDATE))
         {
            HOST_MESSAGE.orig_msg_id[itemp] = Mcc_Gui.reason;
            itemp++;

            /* Currently the format of Mcc_Gui.activity_date is DDMMYY
             * Need to send this as MMDDYY
             */
            memcpy(&HOST_MESSAGE.orig_msg_id[itemp],&Mcc_Gui.activity_date[2],2);
            itemp +=2;
            memcpy(&HOST_MESSAGE.orig_msg_id[itemp],&Mcc_Gui.activity_date[0],2);
            itemp += 2;
            memcpy(&HOST_MESSAGE.orig_msg_id[itemp],&Mcc_Gui.activity_date[4],2);
            itemp += 2;
            memcpy(&HOST_MESSAGE.orig_msg_id[itemp],Mcc_Gui.activity_time,4);
            itemp += 4;
            memcpy(&HOST_MESSAGE.orig_msg_id[itemp],"00", 2);
            itemp += 2;

            if(Mcc_Gui.reason == 'V')
            {
               memcpy( &HOST_MESSAGE.orig_msg_id[itemp], Mcc_Gui.vip_limit_amt,12);
               itemp += 12;
               memcpy( &HOST_MESSAGE.orig_msg_id[itemp], Mcc_Gui.vip_currency_code,3);
               itemp += 3;
            }
            else
            {
               memcpy( &HOST_MESSAGE.orig_msg_id[itemp],"000000000000",12);
               itemp += 12;
               memcpy( &HOST_MESSAGE.orig_msg_id[itemp],"000",3);
               itemp += 3;
            }
         }
         sprintf( temp_str3, "%03d", itemp );
         memcpy( HOST_MESSAGE.orig_msg_len, temp_str3, 3 );
      }
      else   /* MCC103 */
      {
         itemp = 0;
         memset(HOST_MESSAGE.orig_msg_id,0x20,sizeof(HOST_MESSAGE.orig_msg_id));
         itemp = strlen( Mcc_Gui.card_nbr );
         memcpy( &HOST_MESSAGE.orig_msg_id[0],  Mcc_Gui.card_nbr, itemp );
         memset(&HOST_MESSAGE.orig_msg_id[itemp],' ',(19 - itemp));
         itemp += ( 19 - itemp);

         if ( Mcc_Gui.choice != CHOICE_INQUIRE )
         {
            len = strlen( ncf01_i.acquirer_inst_id_code );
            memcpy( &HOST_MESSAGE.orig_msg_id[19],
                    ncf01_i.acquirer_inst_id_code,
                    len );
            Rj_with_lead_zeros( &HOST_MESSAGE.orig_msg_id[19], len );
            itemp += len;

            if( Mcc_Gui.choice == CHOICE_ADD )
            {
               memcpy( &HOST_MESSAGE.orig_msg_id[itemp], Mcc_Gui.product_code, 3 );
               itemp += 3;
               memcpy( &HOST_MESSAGE.orig_msg_id[itemp], "04", 2 );
               itemp += 2;

               HOST_MESSAGE.orig_msg_id[itemp++] = Mcc_Gui.reason;
               itemp += 25;  /* Filler */

               if ( Mcc_Gui.region_us.indicator != 0x00 )
               {
                  HOST_MESSAGE.orig_msg_id[itemp++]= '1';
                  memcpy( &HOST_MESSAGE.orig_msg_id[itemp],
                          Mcc_Gui.region_us.purge_date, 6 );
                  itemp += 6;
               }

               if ( Mcc_Gui.region_canada.indicator != 0x00 )
               {
                  HOST_MESSAGE.orig_msg_id[itemp++] = 'A';
                  memcpy( &HOST_MESSAGE.orig_msg_id[itemp],
                          Mcc_Gui.region_canada.purge_date, 6 );
                  itemp += 6;
               }

               if ( Mcc_Gui.region_sa.indicator != 0x00 )
               {
                  HOST_MESSAGE.orig_msg_id[itemp++] = 'B';
                  memcpy( &HOST_MESSAGE.orig_msg_id[itemp],
                          Mcc_Gui.region_sa.purge_date, 6 );
                  itemp += 6;
               }

               if ( Mcc_Gui.region_asia.indicator != 0x00 )
               {
                  HOST_MESSAGE.orig_msg_id[itemp++] = 'C';
                  memcpy( &HOST_MESSAGE.orig_msg_id[itemp],
                          Mcc_Gui.region_asia.purge_date, 6 );
                  itemp += 6;
               }

               if ( Mcc_Gui.region_europe.indicator != 0x00 )
               {
                  HOST_MESSAGE.orig_msg_id[itemp++] = 'D';
                  memcpy( &HOST_MESSAGE.orig_msg_id[itemp],
                          Mcc_Gui.region_europe.purge_date, 6 );
                  itemp += 6;
               }

               if ( Mcc_Gui.region_africa.indicator != 0x00 )
               {
                  HOST_MESSAGE.orig_msg_id[itemp++] = 'E';
                  memcpy( &HOST_MESSAGE.orig_msg_id[itemp],
                          Mcc_Gui.region_africa.purge_date, 6 );
                  itemp += 6;
               }
            }
         }
         sprintf( temp_str3, "%03d", itemp );
         memcpy( HOST_MESSAGE.orig_msg_len, temp_str3, 3 );
      }
   }

   else if( (0 == strncmp( p_auth_tx->TLF01_details.message_type,"0100",4))||
		    (0 == strncmp( p_auth_tx->TLF01_details.message_type,"0200",4)))
	{
		memcpy(strfield_120_len, p_auth_tx->future_use_1, (sizeof(strfield_120_len)-1));
		memcpy( HOST_MESSAGE.orig_msg_len, strfield_120_len, 3 );

		field_120_len= atoi(strfield_120_len);

		if(field_120_len > 0)
		{

			if(field_120_len<97)
			{
				memcpy( HOST_MESSAGE.orig_msg_id, p_auth_tx->future_use_1 + 3, field_120_len);
			}
			else if(field_120_len<196)
			{
				memcpy( HOST_MESSAGE.orig_msg_id, p_auth_tx->future_use_1 + 3,
						(sizeof( p_auth_tx->future_use_1 )-4) );

				memcpy( HOST_MESSAGE.orig_msg_id + (sizeof( p_auth_tx->future_use_1 )-4),
						p_auth_tx->future_use_2 ,
						(field_120_len -(sizeof( p_auth_tx->future_use_1 )-4)));
			}
		}
	}


   /* FIELD 121: AUTHORIZING AGENT IDENTIFICATION CODE */
   if ( 0 != strcmp( NULL_STR, p_auth_tx->TLF01_details.agent_id ) )
   {
      itemp = strlen( p_auth_tx->TLF01_details.agent_id );
      sprintf( HOST_MESSAGE.mc_agent_len, "%03d", itemp );
      strcpy( HOST_MESSAGE.mc_agent_id, p_auth_tx->TLF01_details.agent_id );
   }
   return;
}


/******************************************************************************
 *
 *  NAME:         COPY_TO_REQ_RESP_BUFFER
 *
 *  DESCRIPTION:  This function clears ReqRsp_Buffer, then copies an input
 *                string into it, for a given length.
 *
 *  INPUTS:       indata - A string to be copied into ReqRsp_Buffer
 *                msglen - Length of data in 'indata' to be copied
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 *     DI - 06/07/99 - Added 'invalid_msg_format' initialization.
 ******************************************************************************/
void copy_to_req_resp_buffer( pCHAR indata, INT msglen )
{
   memset( ReqRsp_Buffer, 0, COMM_BUF_SZE );   /* zero out the buffer */
   buflen = msglen;
   reqidx = 0; 
   memcpy( ReqRsp_Buffer, indata, msglen );
   invalid_msg_format = false;
} /* end copy_to_req_resp_buffer */


/******************************************************************************
 *
 *  NAME:         GET_TRAN_TYPE
 *
 *  DESCRIPTION:  This procedure gets the transaction type from a
 *                message based on message type and processing code.
 *
 *  INPUTS:       p_auth_tx - Transaction record
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   Transaction type
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
BYTE  get_tran_type( pAUTH_TX p_auth_tx ) 
{
   INT  ProcCode;
   BYTE TranType;

   ProcCode = atoi( p_auth_tx->TLF01_details.processing_code );
   ProcCode = ProcCode / 10000;   /* Get just the 1st 2 digits. */
   TranType = p_auth_tx->TLF01_details.tx_key;

   /* Check if transaction is a financial response or an authorization */
   /* response.  These are sent from the MCard host as 0110's.         */
   if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0110" ) )
   {
      /* It is, now verify the procesing code to know what kind */
      switch( ProcCode )
      {
         case 0:
            if ( AUTH_SALE == TranType )
               return( AUTH_SALE_RESPONSE );
            else
               return( AUTH_AUTHORIZATION_RESPONSE );
         break;

         case 20:
            return( AUTH_REFUND_RESPONSE );
         break;

         case 2:
            return( AUTH_VOID_SALE_RESPONSE );
         break;

         case 22:
            return( AUTH_VOID_REFUND_RESPONSE );
         break;

         case 1:
            return( AUTH_CASH_ADVANCE_RESPONSE );
         break;

         case 30:
            return( AUTH_PRE_AUTHORIZATION_RESPONSE );
         break;

         case 31:
            return( AUTH_BALANCE_INQUIRY_RESPONSE );
         break;

         case 40:
            return( AUTH_PAYMENT_RESPONSE );
         break;

         case 42:
            return( AUTH_VOID_PAYMENT_RESPONSE );
         break;

         default:
            return( AUTH_INVALID );
         break;
      } /* End MCard's version of 0110's */
   }  
  
   /* Check for an advise response from MCard host */
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0230" ) )
   {
      switch( ProcCode )
      {

         case 00:
            return( AUTH_OFFLINE_SALE_RESPONSE );
         break;

         case 20:
            return( AUTH_OFFLINE_REFUND_RESPONSE );
         break;

         case 2:
            return( AUTH_SALE_ADJUSTMENT_RESPONSE );
         break;

         case 22:
            return( AUTH_REFUND_ADJUSTMENT_RESPONSE );
         break;

         case 42:
            return( AUTH_PAYMENT_ADJUSTMENT_RESPONSE );
         break;

         default:
            return( AUTH_INVALID );
         break;
      }
   } 
 
   /* Check for a Reversal response from MCard host */
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0410" ) )
      return( AUTH_REVERSAL_RESPONSE );

   /* Check for a statistics response from MCard host */
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0810" ) )
   {
      switch( ProcCode )
      {
         case 91:
            return( AUTH_STATISTICS_RESPONSE );
         break;

         case 92:
            return( AUTH_LOGON_RESPONSE );
         break;

         case 99:
            return( AUTH_TEST_RESPONSE );
         break;

         default:
            return( AUTH_INVALID );
         break;
      }
   }	

   /* 0200 from another host */
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0200" ) )
   {
      switch( ProcCode )
      {
	     case 0x00:
	        return( AUTH_SALE );
         break;

         case 0x20:
            return( AUTH_REFUND );
         break;

         case 0x02:
            return( AUTH_VOID_SALE );
         break;

         case 0x22:
            return( AUTH_VOID_REFUND );
         break;

         case 0x01:
            return( AUTH_CASH_ADVANCE );
         break;

         case 0x40:
            return( AUTH_PAYMENT );
         break;

         default:
            return( p_auth_tx->TLF01_details.tx_key+100 );
         break;
      }
   }

   /* Check for an advise request from another host */
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0220" ) )
   {
      switch( ProcCode )
      {
         case 0x00:
            return( AUTH_OFFLINE_SALE );
         break;

         case 0x20:
            return( AUTH_OFFLINE_REFUND );
         break;

         case 0x02:
            return( AUTH_SALE_ADJUSTMENT );
         break;

         case 0x22:
            return( AUTH_REFUND_ADJUSTMENT );
         break;

         case 0x42:
            return( AUTH_PAYMENT_ADJUSTMENT );
         break;

         default:
        	 return( p_auth_tx->TLF01_details.tx_key+100 );
         break;
      }
   }
  
   /* Check for an Authorization request from another host */
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0100" ) )
   {
      switch( ProcCode )
      {
         case 0x00:
            return( AUTH_AUTHORIZATION );
         break;

         case 0x30:
            return( AUTH_PRE_AUTHORIZATION );
         break;

         case 0x31:
            return( AUTH_BALANCE_INQUIRY );
         break;

		 case 0x28:
            return( AUTH_MONEY_SEND );
         break;

         default:
        	 return( p_auth_tx->TLF01_details.tx_key+100 );
         break;
      }
   }

   /* Check for a Reversal request from another host */  
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0400" ) )
      return( AUTH_REVERSAL );
	else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0420" ) )
      return( AUTH_REVERSAL_ADVICE );	
   /* Check for a batch upload from another host */
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0320" ) )
      return( AUTH_BATCH_UPLOAD ); 
 
   /* Check for a settlement transaction from another host */
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0500" ) )
   {
      switch( ProcCode )
      {
         case 0x92:
            return( AUTH_SETTLE );
         break;

         case 0x96:
            return( AUTH_SETTLE_TRAILER );
         break;

         default:
        	 return( p_auth_tx->TLF01_details.tx_key+100 );
         break;
      }
   }

   /* Check for a statistics transaction from another host */
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0800" ) )
   {
      switch( ProcCode )
      {
         case 0x91:
            return( AUTH_STATISTICS );
         break;

         case 0x92:
            return( AUTH_LOGON );
         break;

         case 0x99:
            return( AUTH_TEST );
         break;

         default:
        	 return( p_auth_tx->TLF01_details.tx_key+100 );
         break;
      }
   }

   /*-----------------------------------------------------------------*/
   /* Check for response message types when dealing with other hosts. */
   /*-----------------------------------------------------------------*/
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0210" ) )
   {
      /* 0210 */
      switch( ProcCode )
      {
         case 0x00: return( AUTH_SALE_RESPONSE );
                    break;

         case 0x20: return( AUTH_REFUND_RESPONSE );
                    break;

         case 0x02: return( AUTH_VOID_SALE_RESPONSE );
                    break;

         case 0x22: return( AUTH_VOID_REFUND_RESPONSE );
                    break;

         case 0x01: return( AUTH_CASH_ADVANCE_RESPONSE );
                    break;

         case 0x40: return( AUTH_PAYMENT_RESPONSE );
                    break;

         case 0x42: return( AUTH_VOID_PAYMENT_RESPONSE );
                    break;

         default:   return( p_auth_tx->TLF01_details.tx_key+100 );
                    break;
      }
   }  
   /* 0230 */
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0230" ) )
   {
      switch( ProcCode )
      {
         case 0x00: return( AUTH_OFFLINE_SALE_RESPONSE );
                    break;

         case 0x20: return( AUTH_OFFLINE_REFUND_RESPONSE );
                    break;

         case 0x02: return( AUTH_SALE_ADJUSTMENT_RESPONSE );
                    break;

         case 0x22: return( AUTH_REFUND_ADJUSTMENT_RESPONSE );
                    break;

         case 0x42: return( AUTH_PAYMENT_ADJUSTMENT_RESPONSE );
                    break;

         default:   return( p_auth_tx->TLF01_details.tx_key+100 );
                    break;
      }
   } 

   /* 0110 */
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0110" ) )
   {
      switch( ProcCode )
      {
         case 0x00: return( AUTH_AUTHORIZATION_RESPONSE );
                    break;

         case 0x30: return( AUTH_PRE_AUTHORIZATION_RESPONSE );
                    break;

         case 0x31: return( AUTH_BALANCE_INQUIRY_RESPONSE );
                    break;

         default:   return( p_auth_tx->TLF01_details.tx_key+100 );
                    break;
      }
   }

   /* 0410 */
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0410" ) )
      return( AUTH_REVERSAL_RESPONSE );

   /* 0810 */ 
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0810" ) )
   {
      switch( ProcCode )
      {
         case 0x91: return( AUTH_STATISTICS_RESPONSE );
                    break;

         case 0x92: return( AUTH_LOGON_RESPONSE );
                    break;

         case 0x99: return( AUTH_TEST_RESPONSE );
                    break;

         default:   return( p_auth_tx->TLF01_details.tx_key+100 );
                    break;
      }
   }

   /* Type Unknown */
   else
	   return( p_auth_tx->TLF01_details.tx_key+100 );
}

/******************************************************************************
 *
 *  NAME:         GET_FILE_UPDATE_ERR_MSG
 *
 *  DESCRIPTION:  This function grabs an error message from the file update
 *                error message table.  It puts it into Mcc_Gui.err_msg.
 *
 *  INPUTS:       p_auth_tx - Structure containing most of the transacation data
 *                Mcc_Gui.msg_type - (Global) MCC102 or MCC103
 *
 *  OUTPUTS:      Mcc_Gui.err_msg - (Global) Textual error message
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void get_file_update_err_msg( pAUTH_TX p_auth_tx )
{
   LONG   err_code;

   CHAR   list_of_valid_regions[] = { "1ABCDE" };

   memset( Mcc_Gui.err_msg1, 0x00, MCC_ERR_LINE_LEN );
   memset( Mcc_Gui.err_msg2, 0x00, MCC_ERR_LINE_LEN );
   memset( Mcc_Gui.err_msg3, 0x00, MCC_ERR_LINE_LEN );

   err_code = atol( p_auth_tx->TLF01_details.resp_source );

   /* Break out error messages according to message type: MCC102 or MCC103 */
   if ( 0 == strcmp("MCC102", Mcc_Gui.msg_type) )
   {
      switch( err_code )
      {
         case 120001:

            strcpy( Mcc_Gui.err_msg1,
                   "PAN not numeric, or BIN not on FIM, or check digit not correct,");

            strcpy( Mcc_Gui.err_msg2,
                   "or PAN not on Account File (for delete or inquiry).");
            break;

         case 120002:

            sprintf(Mcc_Gui.err_msg1,"Invalid ENTRY REASON: %c", Mcc_Gui.reason);
            break;

         case 120005:

            strcpy( Mcc_Gui.err_msg1, "PIN length is not numeric." );
            break;

         case 120006:

            sprintf( Mcc_Gui.err_msg1,
                    "Entry reason = V, and VIP Accumulative Amount is not numeric: %s",
                     Mcc_Gui.vip_limit_amt );
            break;

         default:

            strcpy( Mcc_Gui.err_msg1, "File update format error." );
            sprintf( Mcc_Gui.err_msg2,
                    "Response code = %s. Error code in field 44 = %d",
                     p_auth_tx->TLF01_details.response_code, err_code );
            break;
      }
   }
   else
   {
      switch( err_code )
      {
         case 101001:

            strcpy( Mcc_Gui.err_msg1, "Primary account number not on file." );
            break;

         case 120001:

            strcpy( Mcc_Gui.err_msg1, "PAN not numeric, or BIN is invalid," );
            strcpy( Mcc_Gui.err_msg2, 
                   "or check digit not correct and card program is not MNS = Non-standard,");
            strcpy( Mcc_Gui.err_msg3,
                   "or origin of message invalid for BIN in PAN.");
            break;

         case 120002:

            sprintf( Mcc_Gui.err_msg1,
                    "Issuing ICA (customer ID = %s) is not associated with PAN's BIN.",
                     ncf01_i.acquirer_inst_id_code );
            break;

         case 120003:

            sprintf( Mcc_Gui.err_msg1,
                    "Card program (%s) is not equal to a valid card program value.",
                     Mcc_Gui.product_code );
            break;

         case 120004:

            strcpy( Mcc_Gui.err_msg1, "Response code is not equal to 04 = capture card." );
            break;

         case 120005:

            sprintf( Mcc_Gui.err_msg1,
                    "Invalid ENTRY REASON: %c. Expected C, F, O, or X",
                     Mcc_Gui.reason );
            break;

         case 120006:

            if ( Mcc_Gui.region_us.indicator != 0x00 )
            {
               if ( NULL == strchr(list_of_valid_regions,Mcc_Gui.region_us.indicator))
               {
                  sprintf( Mcc_Gui.err_msg1,
                          "Invalid Regional Indicator: %c. Expected: 1, A, B, C, D, OR E",
                           Mcc_Gui.region_us.indicator );
               }
            }
            else if ( Mcc_Gui.region_sa.indicator != 0x00 )
            {
               if ( NULL == strchr(list_of_valid_regions,Mcc_Gui.region_sa.indicator))
               {
                  sprintf( Mcc_Gui.err_msg1,
                          "Invalid Regional Indicator: %c. Expected: 1, A, B, C, D, OR E",
                           Mcc_Gui.region_sa.indicator );
               }
            }
            else if ( Mcc_Gui.region_canada.indicator != 0x00 )
            {
               if ( NULL == strchr(list_of_valid_regions,Mcc_Gui.region_canada.indicator))
               {
                  sprintf( Mcc_Gui.err_msg1,
                          "Invalid Regional Indicator: %c. Expected: 1, A, B, C, D, OR E",
                           Mcc_Gui.region_canada.indicator );
               }
            }
            else if ( Mcc_Gui.region_asia.indicator != 0x00 )
            {
               if ( NULL == strchr(list_of_valid_regions,Mcc_Gui.region_asia.indicator))
               {
                  sprintf( Mcc_Gui.err_msg1,
                          "Invalid Regional Indicator: %c. Expected: 1, A, B, C, D, OR E",
                           Mcc_Gui.region_asia.indicator );
               }
            }
            else if ( Mcc_Gui.region_europe.indicator != 0x00 )
            {
               if ( NULL == strchr(list_of_valid_regions,Mcc_Gui.region_europe.indicator))
               {
                  sprintf( Mcc_Gui.err_msg1,
                          "Invalid Regional Indicator: %c. Expected: 1, A, B, C, D, OR E",
                           Mcc_Gui.region_europe.indicator );
               }
            }
            else if ( Mcc_Gui.region_africa.indicator != 0x00 )
            {
               if ( NULL == strchr(list_of_valid_regions,Mcc_Gui.region_africa.indicator))
               {
                  sprintf( Mcc_Gui.err_msg1,
                          "Invalid Regional Indicator: %c. Expected: 1, A, B, C, D, OR E",
                           Mcc_Gui.region_africa.indicator );
               }
            }
            else
            {
               strcpy( Mcc_Gui.err_msg1,
                      "Regional indicators are valid, but not in ascending order");
            }
            break;

         case 120007:

            strcpy( Mcc_Gui.err_msg1,
                   "Region-requested purge date is invalid or");
            strcpy( Mcc_Gui.err_msg2,
                   "not equal to current or future date.");
            break;

         default:

            strcpy( Mcc_Gui.err_msg1, "File update format error." );
            sprintf( Mcc_Gui.err_msg2,
                    "Response code = %s. Error code in field 44 = %d",
                     p_auth_tx->TLF01_details.response_code, err_code );
            break;
      }
   }
   return;
}


/*****************************************************************************
*
* FUNCTION:     mcrdutil_get_strtime
*
* DESCRIPTION:  Function sets transaction start time
*               
*
* PARAMETERS:   CHAR*
*
* RETURN:       NONE
*
******************************************************************************/
void mcrdutil_get_strtime ( pCHAR deststr )
{
   time_t seconds;

   seconds = time(NULL_PTR);      /* get the current date and time in seconds */
   sprintf( deststr, "%ld", seconds );
}

/******************************************************************************
 *
 *  NAME:         BLANK_STRING
 *
 *  DESCRIPTION:  This function determines if a string consists of all blanks.
 *
 *  INPUTS:       max_len - Number of bytes to check as blank or not
 *                astr    - String to be checked
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if 'astr' is all blank, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
BOOLEAN blank_string(pCHAR astr, INT max_len)
{
   int i;

   for(i = 0;i < max_len; i++)
   {
      if (astr[i] != ' ')
         return( false );
   }
   return( true );
}

/*****************************************************************************

  Function:    blank_or_null_string

  Description: This function will determine if a string is blank or contains
               only nulls
  Author: 
      E. Gable  3/7/99
  Inputs:
      astr - the string to test
      max_len - the length of the string to test.
  Outputs:
      None
  Return values:
      true - the string is blank
      false - the string is not blank
  Modified by:
******************************************************************************/
BOOLEAN blank_or_null_string(pCHAR astr, INT max_len)
{
   int i;
   for(i = 0;i < max_len; i++)
   {
      if ((astr[i] != ' ') && (astr[i] != '\0'))
         return(false);
   }
   return(true);    
}
/******************************************************************************
 *
 *  NAME:         MCRDUTIL_VERIFY_PIN_BLOCK
 *
 *  DESCRIPTION:  This function verifies a PIN block by checking to if there
 *                is data in the field.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   OK if PIN block exists,
 *                else NOT_OK if field is null or all blanks
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT mcrdutil_verify_pin_block( pAUTH_TX p_auth_tx )
{
   INT   ret_val = OK;

   if ( 0 == strlen(p_auth_tx->TLF01_details.pin_block) )
   {
      ret_val = NOT_OK;
   }
   else if ( true == blank_string( p_auth_tx->TLF01_details.pin_block,
                     strlen(p_auth_tx->TLF01_details.pin_block)) )
   {
      ret_val = NOT_OK;
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         mcrdutil_transfer_ctl_to_auth_app
 *
 *  DESCRIPTION:  This function builds the transaction Id into the XIPC buffer
 *                and sets the terminal status timer, and transfers control to
 *                the authorizer module.  This function gets called when
 *                processing a request from a host.
 *
 *  INPUTS:       p_auth_tx - Data structure containing transaction record
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Unknown
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT mcrdutil_transfer_ctl_to_auth_app( pAUTH_TX p_auth_tx )
{
   INT   ret_code;
   CHAR  err_msg[512]={0};

   memset( err_msg, 0x00, sizeof(err_msg) );
   ret_code = send_msg_to_authorizer( p_auth_tx, MT_AUTH_REQUEST,
                                      application_que_name, err_msg );
   if ( OK != ret_code )
   {
	   memset(Log_Bufr,0x00,sizeof(Log_Bufr));
	   sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
	   		   p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
	   memcpy( p_auth_tx->TLF01_details.response_text, err_msg,
	                 sizeof(p_auth_tx->TLF01_details.response_text)-1 );
	   strcat(err_msg,Log_Bufr);
	   ncmcrd_log_message( 3, 2, err_msg , "mcrdutil_transfer_ctl_to_auth_app" , 0 ,p_auth_tx->TLF01_details.card_num);
   }

   return( ret_code );
}

/******************************************************************************
 *
 *  NAME:         mcrdutil_process_host_request
 *
 *  DESCRIPTION:  This function processes a request from a host.  It initiates
 *                a transaction record - auth_tx.
 *
 *  INPUTS:       p_auth_tx - Data structure containing transaction record.
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Unknown
 *
 ******************************************************************************/
INT mcrdutil_process_host_request( pAUTH_TX p_auth_tx )
{
   INT    retval;
   CHAR   unique_tran_str_id[11] = {0};
   CHAR   date_str [ 9] = {0};
   CHAR   time_str [ 7] = {0};
   CHAR   time_date[25] = {0};
   CHAR   ErrorMsg[512] = {0};

   if ( 0 == strcmp( p_auth_tx->TLF01_details.pin_block, "DECLINE" ))
   {
      /* Decline due to no pin block. */
      strcpy( p_auth_tx->TLF01_details.response_code, INVALID_PIN );
      p_auth_tx->TLF01_details.tx_key += 100;

      /* Build response message for the host. */
      build_host_structure( p_auth_tx );
      build_host_request( p_auth_tx );

      /* Send response to the host. */
      retval = mcrdutil_send_host_response( p_auth_tx );
      (void)insert_into_TLF01( p_auth_tx );
      return( retval );
   }

   /* Check to see if the PIN block has data in it. */
   if ( OK == mcrdutil_verify_pin_block(p_auth_tx) )
   {
      /* Store current working key and pin type in transaction. */
      strcpy( p_auth_tx->TLF01_details.source_key,        ncf01_a.cwk      );
      strcpy( p_auth_tx->TLF01_details.source_pin_format, ncf01_a.pin_type );
   }

   if ( OK != mcrdutil_transfer_ctl_to_auth_app(p_auth_tx) )
   {
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Failed to process a request from MasterCard. Term Id:%s ,Merch Id: %s RRN: %s ",
      		  p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      memcpy( p_auth_tx->TLF01_details.response_text, "Failed to process a request from MasterCard.",
                    sizeof(p_auth_tx->TLF01_details.response_text)-1 );
      ncmcrd_log_message( 1, 3, Log_Bufr , "mcrdutil_process_host_request" , 0 ,p_auth_tx->TLF01_details.card_num);
      return( NOT_OK );
   }
   return( OK );
}

/******************************************************************************
 *
 *  NAME:         mcrdutil_init_txn
 *
 *  DESCRIPTION:  This function initializes a new auth_tx transaction.
 *
 *  INPUTS:       p_auth_tx - Data structure containing transaction record.
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Unknown
 *
 *  MODIFICATIONS:
 *     DI - 04/09/99 - Added creation of RRN.
 *     DI - 04/29/99 - Modified the way tran_id is created for more granularity.
 ******************************************************************************/
void mcrdutil_init_txn( pAUTH_TX p_auth_tx )
{
   CHAR   unique_tran_str_id[21] = "";
   CHAR   date_str [ 9] = "";
   CHAR   time_str [ 7] = "";
   CHAR   time_date[25] = "";
   CHAR   temp_unique_tran[15]="";
   INT     tid_len = 0;
   INT     unique_tran_str_id_len=0;
   

	/* Get local time. */
   if ( p_auth_tx->TLF01_details.date_yyyymmdd[0] == 0x00 )
   {
      ptetime_get_timestamp( time_date );
      get_date( time_date, date_str );
      get_time( time_date, time_str );
      strcpy( p_auth_tx->TLF01_details.date_yyyymmdd, date_str );
      strcpy( p_auth_tx->TLF01_details.time_hhmmss,   time_str );
   }
   
   /* Create a unique transaction id for the new auth_tx record. */
   ptetime_get_strtime( unique_tran_str_id );
   memset( p_auth_tx->TLF01_details.primary_key.transaction_id, 0,
           sizeof(p_auth_tx->TLF01_details.primary_key.transaction_id) );
   tid_len= strlen(p_auth_tx->TLF01_details.terminal_id);
   if(tid_len == 8)
   {
   		memcpy( p_auth_tx->TLF01_details.primary_key.transaction_id,
        	   p_auth_tx->TLF01_details.terminal_id, 
           		sizeof( p_auth_tx->TLF01_details.terminal_id) - 1 );
   		strncat( p_auth_tx->TLF01_details.primary_key.transaction_id,
        	    unique_tran_str_id+2, 12 );
   }
   else
   {
   	   /* We have to now prepare 20 digit unique Transaction ID.
   	   	 unique_tran_str_id will be having 14 digit length
   	   	 now we will make it as 20 by adding YYMMDD in the beginning of it.
   	   	 20 byte will look like
   	   	 YYMMDD"unique_tran_str_id"
   	   	 for example if unique_tran_str_id is "1444752896.334" then it will become
   	   	 YYMMDD1444752896.334
		 Based on ther terminal length we will copy data from new unique_tran_str_id
	   */
	   strncpy(temp_unique_tran,unique_tran_str_id,14);
	   memset(unique_tran_str_id,0x00,sizeof(unique_tran_str_id));
	  
	   strncpy(unique_tran_str_id,
	   			p_auth_tx->TLF01_details.date_yyyymmdd+2, 6); //YYMMDD
	   strncat (unique_tran_str_id,temp_unique_tran,14);
	   
   	   unique_tran_str_id_len= strlen(unique_tran_str_id);
	   memcpy( p_auth_tx->TLF01_details.primary_key.transaction_id,
			   p_auth_tx->TLF01_details.terminal_id, tid_len );
	   strncat( p_auth_tx->TLF01_details.primary_key.transaction_id,
			    unique_tran_str_id+tid_len, 
			    (unique_tran_str_id_len-tid_len) );
				
		p_auth_tx->TLF01_details.primary_key.transaction_id[20]=0x00;
   }

   /* Initialize some of the auth_tx fields. */
   p_auth_tx->TLF01_details.general_status      =  GS_TXN_NEW;
   p_auth_tx->TLF01_details.entry_type          =  FINANCIAL;
   strcpy( p_auth_tx->TLF01_details.dispensation.origin, "RO" );
   strcpy( p_auth_tx->TLF01_details.dispensation.auth_2, "RA" );

   strcpy( p_auth_tx->TLF01_details.handler_queue,       application_que_name);
   strncpy( p_auth_tx->TLF01_details.tran_start_time,     unique_tran_str_id, 16 );

   /* Tell the authorizer to check CVV if TRK2/TRK1 data is present
    * or if CVC2 data is present.
    */
   if ( (p_auth_tx->TLF01_details.track2[0]   != 0x00) ||
        (p_auth_tx->TLF01_details.track1[0]   != 0x00) ||
        (p_auth_tx->TLF01_details.cvc_data[0] != 0x00)  )
   {
      p_auth_tx->TLF01_details.verify_mag_stripe[0] = 'Y';
   }
   else
   {
      p_auth_tx->TLF01_details.verify_mag_stripe[0] = 'N';
   }

   /* Set issuer Id for reporting purposes. */
   strncpy( p_auth_tx->TLF01_details.issuer_id, AppName,
            sizeof(p_auth_tx->TLF01_details.issuer_id) );

   /* Set indicator to use Alpha Country Code for Country Code
    * Transaction Limits.  Alpha code is obtained from DE43.
    * M = Transaction is from MasterCard network.
    */
   p_auth_tx->TLF01_details.product_codes[18].quantity[0] = 'M';

   /* Create RRN */
   if ( p_auth_tx->TLF01_details.retrieval_ref_num[0] == 0x00 )
   {
      create_rrn( p_auth_tx->TLF01_details.retrieval_ref_num );
   }
}

/******************************************************************************
 *
 *  NAME:         incoming_other_host_response
 *
 *  DESCRIPTION:  This function processes a response for a host.  The request
 *                from the host is stored in p_auth_tx.
 *
 *  INPUTS:       p_auth_tx - Data structure containing transaction record.
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Unknown
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT incoming_other_host_response( pAUTH_TX p_auth_tx)
{
   INT   ret_code;

   /* Responses to reversals must be approved. */
   if ( AUTH_REVERSAL_ADVICE_RESPONSE == p_auth_tx->TLF01_details.tx_key )
   {
      strcpy( p_auth_tx->TLF01_details.response_code, APPROVED );
      ret_code = delete_orig_ncf30( ncf30, p_auth_tx );
      if ( false == ret_code )
      {
         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr," Unable to delete reversal's original txn in NCF30 Term Id:%s ,Merch Id: %s RRN: %s ",
         		 p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
         ncmcrd_log_message( 3, 2, Log_Bufr , "incoming_other_host_response" , 0 ,p_auth_tx->TLF01_details.card_num);
      }
   }

   /* Update NCF21 and insert into NCF30.
    * But do it only for approved authorizations.
    */
   if ( 0 == strcmp( APPROVED, p_auth_tx->TLF01_details.response_code ) )
   {
      if ( 0 == strncmp( "01", p_auth_tx->TLF01_details.message_type, 2 ) )
      {
         /* Send record to NCF21 and NCF30 to track totals. */
         (void)update_host_totals( p_auth_tx, ST2_NCF30_INSERT, "A" );
      }
   }

   /* Continue to send response to host. */
   return( incoming_other_host_response_db_reply( p_auth_tx ) );
}


/******************************************************************************
 *
 *  NAME:         incoming_other_host_response_db_reply
 *
 *  DESCRIPTION:  This function processes a response for a host.  The request
 *                from the host is stored in p_auth_tx.  This function picks
 *                up where 'incoming_other_host_response' left off.  You come
 *                here after updating NCF21 and NCF30 if they required updating.
 *
 *  INPUTS:       p_auth_tx - Data structure containing transaction record.
 *                
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       D. Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT incoming_other_host_response_db_reply( pAUTH_TX p_auth_tx)
{
   INT        ret_code = NOT_OK;
   INT        i,buf_temp_len = 0;
   CHAR       temp_amount[13] = {0};
   CHAR       temp_cur_code[4] = {0};

   if ((AUTH_ADVICE_RESPONSE != p_auth_tx->TLF01_details.tx_key) &&
       (AUTH_REVERSAL_ADVICE_RESPONSE != p_auth_tx->TLF01_details.tx_key))
   {
      strncpy(temp_cur_code, p_auth_tx->TLF01_details.currency_code,
              sizeof(p_auth_tx->TLF01_details.currency_code));
      strncpy(temp_amount, p_auth_tx->TLF01_details.total_amount,
              sizeof(p_auth_tx->TLF01_details.total_amount));

      strncpy(p_auth_tx->TLF01_details.currency_code,
              p_auth_tx->reconciliation_cur_code,
              sizeof(p_auth_tx->TLF01_details.currency_code));
      strncpy(p_auth_tx->TLF01_details.total_amount,
              p_auth_tx->reconciliation_amount,
              sizeof(p_auth_tx->TLF01_details.total_amount));
     
   }
   /* Build response message for the host. */
   build_host_structure( p_auth_tx );
   if ( OK  != build_host_request( p_auth_tx ) )
   {
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Failed to build host request message Term Id:%s ,Merch Id: %s RRN: %s ",
      		  p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      memcpy( p_auth_tx->TLF01_details.response_text, "Failed to build host request message",
                    sizeof(p_auth_tx->TLF01_details.response_text)-1 );
      ncmcrd_log_message( 3, 2, Log_Bufr , "incoming_other_host_response_db_reply" , 0 ,p_auth_tx->TLF01_details.card_num);
   }
   else
   {
        /* Forward Msg coming from Cadience HOST */
		 if(strcmp(p_auth_tx->TLF01_details.dispensation.origin,"RO")==0 &&
		   (0 == strncmp(p_auth_tx->TLF01_details.handler_queue, "ncmcrd", 6)||
		 	0 == strncmp(p_auth_tx->TLF01_details.handler_queue, "ncmcr3", 6)))
		{/* will include all ncmcrd and ncmcr3*/
			buf_temp_len = atoi(p_auth_tx->resp_data_len);
			if(buf_temp_len < COMM_BUF_SZE && buf_temp_len > 0)
			{	buflen = buf_temp_len;
				memcpy(ReqRsp_Buffer,p_auth_tx->resp_buffer,buflen);
			}
		 }
	   /* Send response to the host. */
      ret_code = mcrdutil_send_host_response(p_auth_tx);
      if ( OK == ret_code )
      {
         /* Transaction is completed.  Insert it into the database. */
         for( i=0; i<4; i++ )
            p_auth_tx->TLF01_details.message_type[i] =
                           ascii_ebcdic_conv(ReqRsp_Buffer[i], E2A);
      }
   }
	if ((AUTH_ADVICE_RESPONSE != p_auth_tx->TLF01_details.tx_key) &&
       (AUTH_REVERSAL_ADVICE_RESPONSE != p_auth_tx->TLF01_details.tx_key))
   {
	strncpy(p_auth_tx->TLF01_details.currency_code,temp_cur_code,
              sizeof(p_auth_tx->TLF01_details.currency_code));
      strncpy(p_auth_tx->TLF01_details.total_amount,temp_amount,
             sizeof(p_auth_tx->TLF01_details.total_amount));
	}
	if(ncmcrd_Check_Reversal_Advice_Iss_txns(p_auth_tx) == true)
	{
		/*No need to insert into TLF01 table right now, lets wait for response from txcntl*/
	}
	else
	{
		insert_into_TLF01( p_auth_tx );
	}

   return( ret_code );
}

/******************************************************************************
 *
 *  NAME:         insert_into_TLF01
 *
 *  DESCRIPTION:  This function builds a PTE message containing auth_tx and
 *                sends it to the device data server to insert it into TLF01.
 *
 *  INPUTS:       p_auth_tx - Data structure containing transaction record.
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       D. Irby
 *
 ******************************************************************************/
INT insert_into_TLF01( pAUTH_TX p_auth_tx)
{
   INT        ret_code  = NOT_OK;
   CHAR       err_msg[512]={0};
   BYTE       buffer[sizeof(AUTH_TX)+sizeof(TLF01)];
   CHAR       unique_tran_str_id[20] = "";
   CHAR       time_date[25] = "";

   if ( p_auth_tx->host2_acquirer_cb_code[0] == '*' )
   {
      /* TLF01 has already been inserted. */
      ret_code = OK;
   }
   else
   {
      p_auth_tx->host2_acquirer_cb_code[0] = '*';

      /* Create a unique transaction id for the new auth_tx record. */
      if(p_auth_tx->TLF01_details.primary_key.transaction_id[0] == '\0')
      {
	      ptetime_get_strtime( unique_tran_str_id );
         memset( p_auth_tx->TLF01_details.primary_key.transaction_id, 0,
                 sizeof(p_auth_tx->TLF01_details.primary_key.transaction_id) );
         memcpy( p_auth_tx->TLF01_details.primary_key.transaction_id,
                 p_auth_tx->TLF01_details.terminal_id, 
                 sizeof( p_auth_tx->TLF01_details.terminal_id) - 1 );
         strncat( p_auth_tx->TLF01_details.primary_key.transaction_id,
                  unique_tran_str_id+2, 12 );
      }

      /* Create date and time. */
      if ( p_auth_tx->TLF01_details.date_yyyymmdd[0] == 0x00 )
      {
         ptetime_get_timestamp( time_date );
         get_date( time_date, p_auth_tx->TLF01_details.date_yyyymmdd );
         get_time( time_date, p_auth_tx->TLF01_details.time_hhmmss   );
      }
      ptetime_get_strtime( p_auth_tx->TLF01_details.tran_finish_time );

      memset(buffer, 0, sizeof(buffer) );
      memcpy(buffer+Auth_Tx_Size,&p_auth_tx->TLF01_details,Tlf01_Size);

      ret_code = build_and_send_request_message( MT_DB_REQUEST, 
                                                 ST1_DB_INSERT, 
                                                 ST2_NONE,
                                                 updateds_que_name, 
                                                 (pBYTE)&buffer,
                                                 sizeof(buffer), 
                                                 TLF01_DATA,
                                                 NULL_STR,
                                                 err_msg );
      if ( OK != ret_code )
      {
         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
         		 p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
         memcpy( p_auth_tx->TLF01_details.response_text, err_msg,
                       sizeof(p_auth_tx->TLF01_details.response_text)-1 );
         strcat(err_msg,Log_Bufr);
         ncmcrd_log_message( 3, 3, err_msg , "insert_into_TLF01" , 0 ,p_auth_tx->TLF01_details.card_num);
      }
   }
   return( ret_code );
}


/*****************************************************************************

  Function:   mcrdutil_parse_db_ok

  Description: This function determines which type of database request the
               incoming message is from and routes it to the correct function.
  Inputs:
      p_msg_in - pointer to the incoming message
  Outputs:
      None
  Return values:
      None
  Author: 
      BGable   7/13/98
  Modified by:
      
******************************************************************************/
void  mcrdutil_parse_db_ok( pPTE_MSG p_msg_in )
{
   CHAR	         Buffer[512]={0};
   pBYTE         pCurrent;
   pPTE_MSG_DATA p_msg_data;
   BYTE          subtype1;
   BYTE          subtype2;
   BYTE          app_data_type;
   AUTH_TX       auth_tx;

   p_msg_data = ptemsg_get_pte_msg_data( p_msg_in );
   pCurrent   = ptemsg_get_pte_msg_data_data( p_msg_data );

   app_data_type = ptemsg_get_pte_msg_data_app_data_type( p_msg_data );
   subtype1 = ptemsg_get_msg_subtype1( p_msg_in );
   subtype2 = ptemsg_get_msg_subtype2( p_msg_in );

   if ( ST1_DB_SELECT == subtype1 )
   {
      if (NCF30_DATA == app_data_type)
      {
         /*  The reversal record was found,
          *  send the transaction to the authorizer.
          */
         if(ST2_DB_SELECT_TRAN_BY_STAN == subtype2)
         {
            if (false == process_incoming_message_ncf30_reply(pCurrent))
            {
                ncmcrd_log_message( 3, 3,  "Unable to send the request to the authorizor application" , "mcrdutil_parse_db_ok" , 0 ,"");
            }
         }
         else if ( ST2_DB_SELECT_TRAN_BY_CARD_NUM == subtype2 )
         {
            if (false == process_incoming_message_ncf30_reply(pCurrent))
            {
                ncmcrd_log_message( 3, 3,  "Unable to send the request to the authorizor application" , "mcrdutil_parse_db_ok" , 0 ,"");
            }
         }
         else if(ST2_NONE == subtype2)
         {
            if (false == process_incoming_message_ncf30_reply(pCurrent))
            {
               ncmcrd_log_message( 3, 3,  "Unable to send the request to the authorizor application" , "mcrdutil_parse_db_ok" , 0 ,"");
            }
         }
         else 
         {
            ncmcrd_log_message( 3, 3,  "Unknown Subtype2: Subtype1 is SELECT, app_data_type = NCF30_DATA" , "mcrdutil_parse_db_ok" , 0 ,"");
         }
      }

      /* Negative file processing */
      else if ( NEG01_DATA == app_data_type )
      {
         /* Card number is in the negative file.  This means it
          * should not be processed ( lost or stolen, etc. ).
          */
         if ( OK != process_neg_file_reply(pCurrent, false) )
         {
            ncmcrd_log_message( 1, 3,  "Error processing reply from neg file" , "mcrdutil_parse_db_ok" , 0 ,"");
         }
      }
      else if ( TLF01_DATA == app_data_type )
      {
		   if ( OK != process_incoming_message_ncf30_reply(pCurrent) )
		   {
			  ncmcrd_log_message( 1, 3,  "Error processing reply from neg file purchase return" ,
					  	  	  	  "mcrdutil_parse_db_ok" , 0 ,"");
		   }
      }
      else
      {
         sprintf( Buffer, "invalid incoming message type");
         ncmcrd_log_message( 1, 3,  "Error processing incoming terminal message" , "mcrdutil_parse_db_ok" , 0 ,"");
      }
   }
   else if (ST1_DB_UPDATE == subtype1)
   {
      if (ALL_DATA == app_data_type)
      {
         memset( &auth_tx, 0,        Auth_Tx_Size );
         memcpy( &auth_tx, pCurrent, Auth_Tx_Size );
         if( ncmcrd_Check_Reversal_Advice_Iss_txns(&auth_tx) == true )
         {
            // we already sent the response back and updated all tables 
            return; 
         }
		 else if(false == incoming_other_host_response_db_reply( &auth_tx ))
         {
            memset(Log_Bufr,0x00,sizeof(Log_Bufr));
            sprintf(Log_Bufr," Unable to send response to host. Term Id:%s ,Merch Id: %s RRN: %s ",
            		auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
            memcpy( auth_tx.TLF01_details.response_text, "Unable to send response to host.",
                          sizeof(auth_tx.TLF01_details.response_text)-1 );
            strcat(Log_Bufr,Log_Bufr);
            ncmcrd_log_message( 3, 2, Log_Bufr , "mcrdutil_parse_db_ok" , 0 ,auth_tx.TLF01_details.card_num);
         }
      }
   }
   else if ( ST1_DB_DELETE == subtype1 )
   {
      if ( NCF30_DATA == app_data_type )
      {
         ncmcrd_log_message( 3, 2, "Deleted from NCF30" , "mcrdutil_parse_db_ok" , 0 ,"");
      }
   }
   else if ( ST1_DB_INSERT != subtype1 )
   {
      ncmcrd_log_message( 2, 2, "Invalid incoming message type " , "mcrdutil_parse_db_ok" , 0 ,"");
   }

} /* mcrdutil_parse_db_ok */


/*****************************************************************************

  Function:   mcrdutil_parse_db_error

  Description: This function determines which type of database request the
               incoming error message is from and routes it to the correct
               function.
  Inputs:
      p_msg_in - pointer to the incoming message
  Outputs:
      None
  Return values:
      None
  Author: 
      BGable   7/13/98
  Modified by:
      DI - 03/16/99 - Removed call to 'perform_error_processing' for NCF01
                      errors on messages that came from the host.
******************************************************************************/
void  mcrdutil_parse_db_error( pPTE_MSG p_msg_in )
{
   pBYTE pCurrent;
   pPTE_MSG_DATA p_msg_data;
   BYTE  app_data_type;
   BYTE  subtype1;
   BYTE  subtype2;
   CHAR error_msg[512] = {0};
   AUTH_TX       auth_tx;
   CHAR Buffer[512] = {0};
   NCF30 ncf30;
   CHAR Buff[512] = {0};

   p_msg_data = ptemsg_get_pte_msg_data(p_msg_in);
   pCurrent   = ptemsg_get_pte_msg_data_data(p_msg_data);

   memset( &auth_tx, 0, Auth_Tx_Size );
   memcpy( &auth_tx, pCurrent, Auth_Tx_Size );
   strcpy (error_msg, pCurrent+(Auth_Tx_Size));

   app_data_type = ptemsg_get_pte_msg_data_app_data_type(p_msg_data);
   subtype1      = ptemsg_get_msg_subtype1(p_msg_in);
   subtype2      = ptemsg_get_msg_subtype2(p_msg_in);

   if ( ST1_DB_SELECT == subtype1 )
   {
      if ( NCF30_DATA == app_data_type )
      {
    	  if( ncmcrd_Check_Reversal_Advice_Iss_txns(&auth_tx) == true )
    	  {
    		  // Do nothing
    	  }
    	  else if ( (AUTH_REVERSAL        == auth_tx.TLF01_details.tx_key) ||
              (AUTH_REVERSAL_REPEAT == auth_tx.TLF01_details.tx_key )) 
         {
            /* If original txn not found for reversal txn
				a. Display error msg on monitor
				b. set general status 
				c. send response back (09-76) 11 Nov, 2009 TF-Ajay */
			if(AUTH_REVERSAL        == auth_tx.TLF01_details.tx_key)
			{	
				auth_tx.TLF01_details.tx_key = AUTH_REVERSAL_RESPONSE;
				strcpy( auth_tx.TLF01_details.response_code, APPROVED );
				auth_tx.TLF01_details.general_status= GS_ORIGINAL_TXN_NOT_FOUND_FOR_REVERSAL ;
				memset( Buff, 0x00, sizeof(Buff) );
				sprintf( Buff,
				 "Original txn for a reversal not found in database. Stan: %s",
				  auth_tx.TLF01_details.sys_trace_audit_num );
				/* Display error message on monitor  */
				memset(Log_Bufr,0x00,sizeof(Log_Bufr));
				sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
						auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
				strcat(Buff,Log_Bufr);
				ncmcrd_log_message( 1, 2, Buff , "mcrdutil_parse_db_error" , 0 ,auth_tx.TLF01_details.card_num);
				 /* This function inserts into TLF01. */
				(void)incoming_other_host_response_db_reply( &auth_tx );

			}
			else
			{
				auth_tx.TLF01_details.tx_key = AUTH_REVERSAL_RESPONSE;
				strcpy( auth_tx.TLF01_details.response_code, APPROVED );

                 /* This function inserts into TLF01. */
				/* The below flow commented because, we have already replied back to MC
				* before sending request to Netds for ncf30 .*/
				//(void)incoming_other_host_response_db_reply( &auth_tx );
			}
         }
         else if(AUTH_REVERSAL_ADVICE == auth_tx.TLF01_details.tx_key )
         {
            auth_tx.TLF01_details.tx_key = AUTH_REVERSAL_ADVICE_RESPONSE;
            strcpy( auth_tx.TLF01_details.response_code, APPROVED );

            /* This function inserts into TLF01. */
            (void)incoming_other_host_response_db_reply( &auth_tx );
         }
		 /* For void sale txn, if original txn not found in NCF30.
		 Just approve txn , dispaly error msg on monitor and send response back (09-76) 11 Nov, 2009 TF-Ajay */
		 else if(AUTH_VOID_SALE == auth_tx.TLF01_details.tx_key )
         {
            auth_tx.TLF01_details.tx_key = AUTH_VOID_SALE_RESPONSE;
            strcpy( auth_tx.TLF01_details.response_code, APPROVED );
		    auth_tx.TLF01_details.general_status= GS_ORIGINAL_TXN_NOT_FOUND_FOR_VOID_SALE ;
			memset( Buff, 0x00, sizeof(Buff) );
            sprintf( Buff,
                 "Original txn for a void not found in database. Stan: %s",
                  auth_tx.TLF01_details.sys_trace_audit_num );
			/* Display error message on monitor  */
             memset(Log_Bufr,0x00,sizeof(Log_Bufr));
             sprintf(Log_Bufr," Original txn not found in NCF30 for Void sale txn Term Id:%s ,Merch Id: %s RRN: %s ",
             		 auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
             ncmcrd_log_message( 3, 2, Log_Bufr , "mcrdutil_parse_db_error" , 0 ,auth_tx.TLF01_details.card_num);
            /* This function inserts into TLF01. */
            (void)incoming_other_host_response_db_reply( &auth_tx );
         }
         else if(AUTH_ADVICE == auth_tx.TLF01_details.tx_key )
         {
            memset( &ncf30, 0x00, Ncf30_Size );
           (void)process_auth_advices (&auth_tx, NOT_FOUND , ncf30 );
         }
         else
            insert_into_TLF01(&auth_tx);
      }
      else if ( NEG01_DATA == app_data_type )
      {
         /* Card number was NOT found in negative file.
          * This is good!  Continue processing to the host.
          */
         if ( OK != process_neg_file_reply(pCurrent, true) )
         {
            ncmcrd_log_message( 1, 3, "Error processing reply from neg file" , "mcrdutil_parse_db_error" , 0 ,"");
         }
      }
      else if ( TLF01_DATA == app_data_type )
	  {
    	  if (false == mcrdutil_transfer_ctl_to_auth_app(&auth_tx))
		  {
    		  ncmcrd_log_message( 1, 3, "Unable to send Purchase return to application","mcrdutil_parse_db_error",1 , "");
		  }
	  }
   }
   else if ( ST1_DB_UPDATE == subtype1 )
   {
      if ( ALL_DATA == app_data_type )
      {


         memset( &auth_tx, 0,        Auth_Tx_Size );
         memcpy( &auth_tx, pCurrent, Auth_Tx_Size );
         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr,
                "NCMCRD:Problems with updating the database tables: %s",
                 error_msg);
         sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
         		 auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
         strcat(Buffer,Log_Bufr);
         ncmcrd_log_message( 3, 2, Buffer , "mcrdutil_parse_db_error" , 0 ,auth_tx.TLF01_details.card_num);
         if(ncmcrd_Check_Reversal_Advice_Iss_txns(&auth_tx) == true )
         {
            /* we already sent the response back and updated all tables */
            return; 
         }
         else if(AUTH_REVERSAL_ADVICE_RESPONSE == auth_tx.TLF01_details.tx_key ||
			 	AUTH_REVERSAL_ADVICE_REPEAT_RESPONSE == auth_tx.TLF01_details.tx_key ||
		 		AUTH_REVERSAL_RESPONSE == auth_tx.TLF01_details.tx_key ||
		 		AUTH_REVERSAL_REPEAT_RESPONSE == auth_tx.TLF01_details.tx_key)
         {
            // Dont change RC code. Let go as it is from cadience
         }
         else
            strcpy( auth_tx.TLF01_details.response_code, "96" );

         if(false == incoming_other_host_response_db_reply( &auth_tx ))
		 {
            memset(Log_Bufr,0x00,sizeof(Log_Bufr));
            sprintf(Log_Bufr," Unable to send response to host. Term Id:%s ,Merch Id: %s RRN: %s ",
            		auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
            memcpy( auth_tx.TLF01_details.response_text, "Unable to send response to host.",
                          sizeof(auth_tx.TLF01_details.response_text)-1 );
            ncmcrd_log_message( 3, 3, Log_Bufr , "mcrdutil_parse_db_error" , 0 ,auth_tx.TLF01_details.card_num);
		 }
      }
   }
   else if ( ST1_DB_DELETE == subtype1 )
   {
      if ( NCF30_DATA == app_data_type )
      {
         sprintf(Buffer,"Unable to delete  NCF30, error: %s",
                 error_msg );
         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
        		 auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
         strcat(Buffer,Log_Bufr);
         ncmcrd_log_message( 3, 2, Buffer , "mcrdutil_parse_db_error" , 0 ,auth_tx.TLF01_details.card_num);
      }
   }
   else if ( ST1_DB_INSERT == subtype1 )
   {
      if ( TLF01_DATA == app_data_type )
      {
         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr,"Unable to Insert into TLF01 , error: %s",
                 error_msg );
         sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
        		auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
         strcat(Buffer,Log_Bufr);
         ncmcrd_log_message( 3, 2, Buffer , "mcrdutil_parse_db_error" , 0 ,auth_tx.TLF01_details.card_num);
      }
   }
   else
   {
      ncmcrd_log_message( 3, 2,  "Invalid incoming message type " , "mcrdutil_parse_db_error" , 0 ,"");
   }
} /*mcrdutil_parse_db_error*/


/******************************************************************************
 *
 *  NAME:         Determine_Auth_Msg_Type
 *
 *  DESCRIPTION:  This function determines the type of message from the
 *                MasterCard Host where the response message type field
 *                contains "0110".  This function determines if it is really
 *                a "0210" message.  A 0110 can be either a real 0110 or a 0210.
 *
 *  INPUTS:       p_host_auth_tx - Transaction record response from the host
 *
 *  OUTPUTS:      p_orig_auth_tx - Transaction record prior to going to the host
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
void Determine_Auth_Msg_Type( pAUTH_TX p_host_auth_tx, pAUTH_TX p_orig_auth_tx )
{
   INT  ProcCode;

   /* Only do this for '0110's.  Other message types are unique. */
   if ( 0 == strncmp( p_host_auth_tx->TLF01_details.message_type, "0110", 4 ) )
   {
      /* Determine message type based on processing code.  However, if it
       * is a processing code of 0, it could be a Sale or an Authorization.
       * In this case, use the message type of the original transaction.
       */
      ProcCode = atoi( p_host_auth_tx->TLF01_details.processing_code );
      ProcCode = ProcCode / 10000;   /* Get just the 1st 2 digits. */

      switch( ProcCode )
      {
         case 30:   /* Pre-Authorization  */
         case 31:   /* Balance Inquiry    */
         case  4:   /* Check Verification */
         case 38:   /* Card Verification  */
                  strncpy(p_orig_auth_tx->TLF01_details.message_type,"0110",4);
                  break;

         case 20:   /* Refund       */
         case  1:   /* Cash Advance */
         case 40:   /* Payment      */
         case  9:   /* Sale & Cash  */
         case  2:   /* Void Sale    */
         case 42:   /* Void Payment */
         case 22:   /* Void Refund  */
                  strncpy(p_orig_auth_tx->TLF01_details.message_type,"0210",4);
                  break;

         default:
                  /* Check to see if it is an Authorization. */
                  if ( 0 == strncmp( p_orig_auth_tx->TLF01_details.message_type,
                                     "0100", 4 ) )
                  {
                     /* It is an Authorization. */
                     strncpy( p_orig_auth_tx->TLF01_details.message_type,
                              "0110", 4 );
                  }
                  else
                  {
                     /* It is not an Authorization; it must be a Sale. */
                     strncpy( p_orig_auth_tx->TLF01_details.message_type,
                              "0210", 4 );
                  }
                  break;
      } /* End Switch */
   }
   else
   {
      /* This is not a message type of "0110".
       * So just copy it straight over.
       */
      strncpy( p_orig_auth_tx->TLF01_details.message_type,
               p_host_auth_tx->TLF01_details.message_type,
               4 );
   }
}


/******************************************************************************
 *
 *  NAME:         get_tran_type_2
 *
 *  DESCRIPTION:  This function determines what the transaction key of a
 *                transaction should be, based on message type.
 *
 *                This function is based upon the currently supported list
 *                of transactions from MasterCard:
 *                   AUTHORIZATION, AUTHORIZATION_ADVICE, FILE_UPDATE,
 *                   REVERSALS, REVERSAL_ADVICES, ADMINISTRATIVE_ADVICE,
 *                   NETWORK_MANAGEMENT, and BALANCE INQUIRY
 *
 *  INPUTS:       p_auth_tx - Transaction containing message type
 *
 *  OUTPUTS:      p_auth_tx - The input transaction with the 'tx_key' field
 *                            filled in.
 *
 *  RTRN VALUE:   OK or NOT_OK - NOT_OK if transaction type cannot be
 *                                determined.
 *
 *  AUTHOR:       D. Irby
 *
 ******************************************************************************/
INT get_tran_type_2( pAUTH_TX p_auth_tx )
{
   CHAR  msg_type[5];
   CHAR  Buff[512] = {0};
   INT   ret_val = OK;

   /* Transaction can be a request from a host or a response from a host.
    *
    * First check for a request from a host.
    * --------------------------------------
    */
   strcpy( msg_type,  p_auth_tx->TLF01_details.message_type    );

   if ( 0 == strcmp( msg_type, "0100" ) )
   {
      if(0==strncmp(CASH_DISBURSEMENT,p_auth_tx->TLF01_details.processing_code,2))
      {
         p_auth_tx->TLF01_details.tx_key = AUTH_CASH_ADVANCE;
      }
      else if(0==strncmp(WITHDRAWAL,p_auth_tx->TLF01_details.processing_code,2))
      {
         p_auth_tx->TLF01_details.tx_key = AUTH_CASH_ADVANCE;
      }
      else if(0==strncmp(BALANCE_INQUIRY,p_auth_tx->TLF01_details.processing_code,2))
      {
         p_auth_tx->TLF01_details.tx_key = AUTH_BALANCE_INQUIRY;
      }
	  /* (09-76)  added to identify void sale txn  10 Nov, 2009 Tf-Ajay */
	   else if(0==strncmp(VOID_SALE,p_auth_tx->TLF01_details.processing_code,2))
	   {
         p_auth_tx->TLF01_details.tx_key = AUTH_VOID_SALE;
	   }
	else if(0==strncmp(MONEY_SEND,p_auth_tx->TLF01_details.processing_code,2))
	   {
         p_auth_tx->TLF01_details.tx_key = AUTH_MONEY_SEND ;
	   }
	   else if(0==strncmp(AUTHORIZATION_REFUND,p_auth_tx->TLF01_details.processing_code,2))
	   {
	        p_auth_tx->TLF01_details.tx_key = AUTH_PRE_AUTHORIZATION_CANCLE ;
	   }
      else
      {
         p_auth_tx->TLF01_details.tx_key = AUTH_AUTHORIZATION;
   }
   }

   else if ( 0 == strcmp( msg_type, "0120" ) )
      p_auth_tx->TLF01_details.tx_key = AUTH_ADVICE;

   else if ( 0 == strcmp( msg_type, "0302" ) )
      p_auth_tx->TLF01_details.tx_key = AUTH_CARD_UPDATE;
   /* Forward reversal txn to Cadencie 06 Nov, 2009 TF-Ajay */
   else if ( 0 == strcmp( msg_type, "0400" ) )
   {
      p_auth_tx->TLF01_details.tx_key = AUTH_REVERSAL;

   }
   else if ( 0 == strcmp( msg_type, "0420" ) )
      p_auth_tx->TLF01_details.tx_key = AUTH_REVERSAL_ADVICE;

   else if ( 0 == strcmp( msg_type, "0620" ) )
      p_auth_tx->TLF01_details.tx_key = AUTH_ADMIN_ADVICE;

   else if ( 0 == strcmp( msg_type, "0800" ) )
      p_auth_tx->TLF01_details.tx_key = AUTH_LOGON;

   else if ( 0 == strcmp( msg_type, "0820" ) )
      p_auth_tx->TLF01_details.tx_key = AUTH_NETWORK_ADVICE;


   /* If not found yet, check for a response from a host. */
   else if ( 0 == strcmp( msg_type, "0110" ) )
   {
      if ( 0 == strncmp(p_auth_tx->TLF01_details.processing_code,BALANCE_INQUIRY,2))
         p_auth_tx->TLF01_details.tx_key = AUTH_BALANCE_INQUIRY_RESPONSE;
   /* (09-76)  added to identify void sale response txn  10 Nov, 2009 Tf-Ajay */
	  else if(0==strncmp(VOID_SALE,p_auth_tx->TLF01_details.processing_code,2))
         p_auth_tx->TLF01_details.tx_key = AUTH_VOID_SALE_RESPONSE;
      else
         p_auth_tx->TLF01_details.tx_key = AUTH_AUTHORIZATION_RESPONSE;
   }
   else if ( 0 == strcmp( msg_type, "0130" ) )
      p_auth_tx->TLF01_details.tx_key = AUTH_ADVICE_RESPONSE;

   else if ( 0 == strcmp( msg_type, "0312" ) )
      p_auth_tx->TLF01_details.tx_key = AUTH_CARD_UPDATE_RESPONSE;
   /* TF-Ajay 30 Oct,2009 Reversal txn 09-76*/
   else if ( 0 == strcmp( msg_type, "0410" ) )
      p_auth_tx->TLF01_details.tx_key = AUTH_REVERSAL_RESPONSE;

   else if ( 0 == strcmp( msg_type, "0430" ) )
      p_auth_tx->TLF01_details.tx_key = AUTH_REVERSAL_ADVICE_RESPONSE;

   else if ( 0 == strcmp( msg_type, "0630" ) )
      p_auth_tx->TLF01_details.tx_key = AUTH_ADMIN_ADVICE_RESPONSE;

   else if ( 0 == strcmp( msg_type, "0810" ) )
      p_auth_tx->TLF01_details.tx_key = AUTH_LOGON_RESPONSE;

   else
   {
      /* Unable to determine transaction type.  Error out! */
      p_auth_tx->TLF01_details.tx_key = AUTH_INVALID;
      strcpy( Buff, "Unable to determine transaction type for message type (" );
      strcat( Buff, msg_type );
      strcat( Buff, ") and processing code (" );
      strcat( Buff, p_auth_tx->TLF01_details.processing_code );
      strcat( Buff, ")" );
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      		p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      memcpy( p_auth_tx->TLF01_details.response_text, Buff,
                    sizeof(p_auth_tx->TLF01_details.response_text)-1 );
      strcat(Buff,Log_Bufr);
      ncmcrd_log_message( 1, 2, Buff , "get_tran_type_2" , 0, p_auth_tx->TLF01_details.card_num);
      ret_val = NOT_OK;
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         verify_processing_code
 *
 *  DESCRIPTION:  This function validates a processing code received from
 *                the network.  It checks to see if it is a supported txn.
 *
 *  INPUTS:       p_auth_tx - Transaction containing processing code
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   OK or NOT_OK - NOT_OK if processing code is not supported.
 *
 *  AUTHOR:       D. Irby
 *
 ******************************************************************************/
INT verify_processing_code( pAUTH_TX p_auth_tx )
{
   INT      ret_val = NOT_OK;
   CHAR     proc_code[7];
   INT      i;

   /* Here is a list of supported processing codes for Equitable. */
   /* Addedd more processing code  03 Nov, 2009 TF-Ajay */
   CHAR     pc_table[MAX_PROC_CODE][7] = { "000000", "003000",
                               "010000", "013000",
                               "170000", "173000",
                                 "300000","303000",
								 "020000","280000",
   	   	   	   	   	   	   	   	 "200000"};

   if(0 ==strncmp(p_auth_tx->TLF01_details.message_type, "0100",4) &&
     0 == strncmp(p_auth_tx->TLF01_details.processing_code, "20",2))
   {
  	// Don't validate the processing code for Refund transaction.
	   return OK;
   }
   /* Only perform this for authorizations. */
   if ( (0 == strcmp( "0100", p_auth_tx->TLF01_details.message_type ))  ||
        (0 == strcmp( "0110", p_auth_tx->TLF01_details.message_type ))    )
   {
      memset( proc_code, 0x00, sizeof(proc_code) );
      strncpy( proc_code, p_auth_tx->TLF01_details.processing_code, 6 );

      for( i=0; i<MAX_PROC_CODE; i++ )
      {
         if ( 0 == strcmp( proc_code, pc_table[i] ) )
         {
            ret_val = OK;
            break;
         }
      }
   }
   else
   {
      /* Do not care about processing code for non-authorizations. */
      ret_val = OK;
   }

   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         TRIM_SPACES
 *
 *  DESCRIPTION:  This function converts spaces at the end of a string into
 *                nulls.
 *
 *  INPUTS:       pString - String that does not want spaces at the end
 *
 *  OUTPUTS:      pString - Same string, but without trailing spaces
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       D. Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
void trim_spaces( pBYTE pString )
{

   INT len, position;

   len = strlen(pString) - 1;
   for(position = len; position >= 0; position--)
   {
      if(pString[position] == ' ')
         pString[position] = 0;
      else
         break;
   }
}

/******************************************************************************
 *
 *  NAME:         trim_spaces_Only
 *
 *  DESCRIPTION:  This function trim space for a given data
 *
 *  INPUTS:       pString - String that does not want spaces only data
 *
 *  OUTPUTS:      pString - No space data
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Abhishek Verma
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT trim_spaces_Only( pBYTE pString )
{

   INT len = 0, position;

   len = strlen(pString) - 1;

   for(position = len; position >= 0; position--)
   {
	   if (pString[position] != ' ')
	   {
		   return true;
	   }
   }
   for(position = len; position >= 0; position--)
   {
	   if(pString[position] == ' ')
	   {
		   pString[position] = 0;
	   }
	   else if (pString[position] != ' ')
	   {
		   break;
	   }
   }
}

/*****************************************************************************

  Function:    mcrdutil_get_response_text

  Description: This function will fill the response_text field in auth_tx.TLF01.

  Author: 
      BGable     5/26/99
  Inputs:
     pAUTH_TX - pointer to auth_tx
  Outputs:
     pAUTH_TX - pointer to auth_tx (with the response text updated)
  Return values:
     None
  Modified by:
******************************************************************************/
VOID mcrdutil_get_response_text( pAUTH_TX p_auth_tx)
{
   INT i;
   static unsigned char response[][45] = {
   {"00Approved"},
   {"01Refer to Card Issuer"},
   {"03Invalid Merchant"},   
   {"04Capture Card"},
   {"05Do Not Honor"},

   {"06Error"},   
   {"08Honor with identification"},   
   {"09Request In Progress"},   
   {"12Invalid transaction"},   
   {"13Invalid Amount"},   
   
   {"14Invalid Card Number"},   
   {"15No Such Issuer"},   
   {"25Unable to locate record on file"},   
   {"27File update field edit error"},   
   {"30Format Error"},   
   
   {"31No Bank Support by Switch"},   
   {"33Expired Card, Pick-Up"},   
   {"34Suspected Card, Pick-Up"},   
   {"36Restricted Card, Pick-Up"},   
   {"40Function Not Supported"},   
   
   {"41Lost, Stolen, Pick-Up"},   
   {"42Universal Acct Not Found"},   
   {"43Stolen Card, Pick-Up"},   
   {"51Not Sufficient Funds"},   
   {"54Expired Card, Pick-Up"},   
   
   {"55Incorrect PIN"},   
   {"56No Card Record"},   
   {"57Trnsxn Not Permitted To Card"},   
   {"58Trnsxn Not Permintted To Term"},   
   {"59Suspected Fraud, Not Pick-Up"},   
   
   {"61Exceeds Amount Limit"},   
   {"62Restricted Card, Not Pick-Up"},   
   {"63Security Violation"},   
   {"65Exceeds WithDrawl Freq Limit"},   
   {"68Response Received Too Late"},   
   
   {"75# of PIN Tries Exceeded"},   
   {"76Invalid/nonexistent TO account specified"},   
   {"77Invalid/nonexistent FROM account specified"},   
   {"78Invalid/nonexistent account specified"},   
   {"79Key exchange validation failed"},   
   
   {"80Duplicate add, action not performed"},   
   {"84Invalid authorization Life Cycle"},   
   {"85Not declined"},   
   {"90Cutoff Is In Progress"},   
   {"91Issuer Or Switch Inoperative"},   
   
   {"92Institution Can't Be Routed To"},   
   {"94Duplicate Transmission"},   
   {"96System Malfunction"}   
   };

 
   for (i = 0; i < 47; i++)
   {
      if (0 == strncmp(p_auth_tx->TLF01_details.response_code, response[i], 2))
      {
         strcpy(p_auth_tx->TLF01_details.voice_auth_text, response[i]+2);
         break;
      }

   }
   if ('\0' == p_auth_tx->TLF01_details.voice_auth_text[0])
   {
      strcpy(p_auth_tx->TLF01_details.voice_auth_text, "Not Approved" );
   }
}

/*****************************************************************************

  Function:    process_request

  Description: This function will send a request to the dataserver.  It is used
               by get_ncf01_cfg.
  Author: 
  Inputs:
      p_msg_out - a pointer to the outgoing message (the request to dataserver)
      p_msg_in  - a pointer to the incoming message (from the dataserver)
      que_name  - the name of the queue to send the request to
      error_msg - the errror message from the dataserver
  Outputs:
      None
  Return values:
      dberror - the type of error
  Modified by:
******************************************************************************/
BYTE process_request( pPTE_MSG p_msg_out, pPTE_MSG *p_msg_in,
                      pCHAR que_name,     pCHAR error_msg )
{
   LONG            data_len;
   pBYTE           p_data;
   pPTE_MSG_DATA   p_msg_data;
   CHAR            temp_str[512] = {0};
   LONG            retcode;
   BYTE            dberror;

   *p_msg_in = pteipc_send_receive( p_msg_out, que_name,
                                    interactive_que_name, &retcode );
   if( NULL_PTR == *p_msg_in )
   {
      pteipc_get_errormsg( retcode, temp_str );
      sprintf( error_msg,
              "Communication Error during request to Network DataServer: %s",
               temp_str );
      return PTEMSG_IPC_ERROR;
   }

   if( ( dberror = ptemsg_get_result_code( *p_msg_in ) ) != PTEMSG_OK )
   {
      p_msg_data    = ptemsg_get_pte_msg_data( *p_msg_in );
      p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
      data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );

      strncpy (temp_str, p_data+Auth_Tx_Size, sizeof(temp_str)-1);
      sprintf( error_msg, "Database Error: %s", temp_str );
   }
   return dberror;
}

/*****************************************************************************

  Function:    get_ncf01_cfg

  Description: This function will send a request to the dataserver to select 
               NCF01.  This is done syncronously and should be used only by
               mcrdmain.c!!!
  Author: 
      BGable     6/30/98
  Inputs:
      pncf01 - pointer to the requested ncf01 structure
  Outputs:
      pncf01 - pointer to the returned ncf01 structure
      error_msg - error message returned by the data server
  Return values:
      None
  Modified by:
******************************************************************************/
BYTE get_ncf01_cfg( pNCF01 pncf01, pCHAR error_msg )
{
   pPTE_MSG	     p_msg_out;
   pPTE_MSG	     p_msg_in;
   LONG          data_len;
   pBYTE         p_data;
   pPTE_MSG_DATA p_msg_data;
   BYTE          rcode;
   BYTE          buffer[sizeof(AUTH_TX)+sizeof(NCF01)];
   AUTH_TX       auth_tx;

   memset(buffer,0, sizeof(buffer));
   memset(&auth_tx, 0x00, Auth_Tx_Size);

   memcpy(buffer,&auth_tx, Auth_Tx_Size);
   memcpy(buffer+Auth_Tx_Size,pncf01, Ncf01_Size);

   p_msg_in  = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_SELECT,
                                 0,
                                 NULL_PTR,
                                 interactive_que_name,
                                 (pBYTE) buffer,
                                 sizeof( buffer),
                                 NCF01_DATA );
   if( NULL_PTR == p_msg_out )
   {
      strcpy( error_msg, "Insufficient memory to build request message" );
      rcode = PTEMSG_INSUFFICIENT_MEMORY;
   }
   else
   {
      if((rcode = process_request( p_msg_out, &p_msg_in, netds_que_name,
                                   error_msg ) ) == PTEMSG_OK )
      {
         p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
         p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
         data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );

         memset(pncf01, 0, Ncf01_Size);
         memcpy(pncf01, p_data+(Auth_Tx_Size), Ncf01_Size);
      }

      if ( PTEMSG_IPC_ERROR != rcode )
      {
         free( p_msg_in );
      }
   }
   return ( rcode );
}


BYTE get_bdkkey01_cfg( pBDKKEY01 pbdkkey01, pCHAR error_msg )
{
   pPTE_MSG	     p_msg_out;
   pPTE_MSG	     p_msg_in;
   LONG          data_len;
   pBYTE         p_data;
   pPTE_MSG_DATA p_msg_data;
   BYTE          rcode;
   BYTE          buffer[sizeof(AUTH_TX)+sizeof(BDKKEY01)];
   AUTH_TX       auth_tx;

   memset(buffer,0, sizeof(buffer));
   memset(&auth_tx, 0x00, Auth_Tx_Size);

   memcpy(buffer,&auth_tx, Auth_Tx_Size);
   memcpy(buffer+Auth_Tx_Size,pbdkkey01, Bdkkey01_Size);

   p_msg_in  = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_SELECT,
                                 0,
                                 NULL_PTR,
                                 interactive_que_name,
                                 (pBYTE) buffer,
                                 sizeof( buffer),
								 BDKKEY01_DATA );
   if( NULL_PTR == p_msg_out )
   {
      strcpy( error_msg, "Insufficient memory to build request message" );
      rcode = PTEMSG_INSUFFICIENT_MEMORY;
   }
   else
   {
      if((rcode = process_request( p_msg_out, &p_msg_in, trands_que_name,
                                   error_msg ) ) == PTEMSG_OK )
      {
         p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
         p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
         data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );

         memset(pbdkkey01, 0, Bdkkey01_Size);
         memcpy(pbdkkey01, p_data+(Auth_Tx_Size), Bdkkey01_Size);
      }

      if ( PTEMSG_IPC_ERROR != rcode )
      {
         free( p_msg_in );
      }
   }
   return ( rcode );
}
/******************************************************************************
 *
 *  NAME:         WRITE_TO_TXN_FILE
 *
 *  DESCRIPTION:  This function calls the necessary library APIs to write a
 *                stream of data to the transaction trace file.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void write_to_txn_file()
{
   INT  rc;
   CHAR err_buf[256];

   /* Log transaction to trace file. */
   if ( Tracing == TRACE_ON )
   {
      rc =log_trace_entry(ppTraceFile_fp,(pCHAR)ReqRsp_Buffer,buflen,TraceFile);
      if ( rc < 0 )
      {
         /* Error while attempting to log a trace entry. */
         Tracing = TRACE_OFF;
         memset( err_buf,   0x00, sizeof(err_buf)   );
         sprintf( err_buf,
                 "Error logging entry to Txn Trace File = %s",
                  TraceFile );
         ncmcrd_log_message( 3, 1, err_buf , "write_to_txn_file" , 0 ,"");

         if ( rc == -1 )
            strcpy( err_buf,
                   "Unable to write txn into trace file. Setting trace off.");
         else if ( rc == -2 )
            strcpy( err_buf, "Unable to close trace file. Setting trace off.");
         else if ( rc == -3 )
            strcpy( err_buf, "Unable to open trace file. Setting trace off." );
         else
            sprintf(err_buf,"Unknown return value: '%d' Setting trace off.",rc);

         ncmcrd_log_message( 3, 1, err_buf , "write_to_txn_file" , 0 ,"");

         /* Close the trace file. */
         if ( false == close_trace_file( ppTraceFile_fp ))
         {
            sprintf( err_buf, "Unable to close trace file %s", TraceFile );
            ncmcrd_log_message( 3, 1, err_buf , "write_to_txn_file" , 0 ,"");
         }
      }
      else
      {
    	  flush_file(ppTraceFile_fp);
      }
   }
   return;
}

/******************************************************************************
 *
 *  NAME:         REJECT_SWITCHED_IN_TXN
 *
 *  DESCRIPTION:  This procedure assumes a switched in transaction failed
 *                some validation/verification test(s).  It will create a
 *                response to send back to the host; populating auth_tx
 *                fields for insertion into TLF01.
 *
 *  INPUTS:       p_auth_tx - Information from the switched in transaction
 *                resp_code - Response code to be sent back to the host
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void reject_switched_in_txn( pAUTH_TX p_auth_tx, pBYTE resp_code )
{
   CHAR  tempstr[512] = {0};

   Tran_Flow = SWITCHED_IN;
   mcrdutil_init_txn( p_auth_tx );

   /* Get the transaction type. */
   if ( OK == get_tran_type_2( p_auth_tx ) )
   {
      /* Make sure it is a response type. */
      p_auth_tx->TLF01_details.tx_key = match_request_to_response( p_auth_tx->TLF01_details.tx_key );

      /* Set message type to a response type. */
      if ( p_auth_tx->TLF01_details.message_type[2] == '0' )
         p_auth_tx->TLF01_details.message_type[2] = '1';

      else if ( p_auth_tx->TLF01_details.message_type[2] == '2' )
         p_auth_tx->TLF01_details.message_type[2] = '3';

      /* Send the desired response code to the host. */
      if(0 == strlen(resp_code) )
      {
    	  strcpy(resp_code,FORMAT_ERROR);
      }
      strcpy( p_auth_tx->TLF01_details.response_code, resp_code );

      /* Set routing information. */
      strcpy( p_auth_tx->TLF01_details.tpdu_id, ncf01_i.tpdu_id );
      strcpy( p_auth_tx->TLF01_details.originator_queue, dialog_que_name );

      /* Send response to the host.  This also inserts into TLF01. */
      if ( OK != incoming_other_host_response( p_auth_tx ) )
      {
         memset( tempstr, 0x00, sizeof(tempstr) );
         sprintf( tempstr,
                 "Unable to send response to MCard host. Stan: %s, RC: %s",
                  p_auth_tx->TLF01_details.sys_trace_audit_num, resp_code );
         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
         		p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
         memcpy( p_auth_tx->TLF01_details.response_text, tempstr,
                       sizeof(p_auth_tx->TLF01_details.response_text)-1 );
         strcat(tempstr,Log_Bufr);
         ncmcrd_log_message( 3, 1, tempstr , "reject_switched_in_txn" , 0 ,p_auth_tx->TLF01_details.card_num);
      }
   }
   return;
}


/******************************************************************************
 *
 *  NAME:         GET_MC_EMV_DATA
 *
 *  DESCRIPTION:  This function steps through the EMV subfield tags.  If one
 *                that is supported by MC is found, it is copied into the
 *                outgoing Host Message buffer.  The incoming data stream is
 *                not changed.
 *
 *  INPUTS:       emv_len_in  - Length of emv_data_in
 *                emv_data_in - EMV data that comes into ATP
 *
 *  OUTPUTS:      HOST_MESSAGE.iccdata - EMV data that goes to MC
 *                HOST_MESSAGE.iccdata_len - Length of EMV data to MC
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void get_MC_EMV_data( INT emv_len_in, pBYTE emv_data_in )
{
   INT   idx_in;
   INT   idx_out = 0;
   INT   tag_len;
   INT   data_len;
   INT   supported;
   BYTE  byteOneOfTag;
   BYTE  byteTwoOfTag;
   BYTE  emv_out_len[4] = "";

   /*MC06_OCT2023 added 9F60 and 96 tag*/
   /* Step past 2-byte length field. */
   idx_in = 2;

   /* Step through input, searching for subfields supported by MCard. */
   while( idx_in < emv_len_in )
   {
      /* Get the 1 or 2 byte subfield (tag) identifier. */
      byteOneOfTag = emv_data_in[idx_in++];
      if ( (byteOneOfTag & 0x1F) == 0x1F )
      {
         byteTwoOfTag = emv_data_in[idx_in++];
         tag_len = 2;
      }
      else
      {
         byteTwoOfTag = 0x00;
         tag_len = 1;
      }

      /* Get the length of this tag. */
      data_len = emv_data_in[idx_in++];

      /* Check to see if this tag is supported by MCard. */
      supported = true;
      if ( byteOneOfTag == 0x5F )
      {
         if ( byteTwoOfTag != 0x2A )
            supported = false;
      }
      else if (byteOneOfTag == 0x9F )
      {
         switch( byteTwoOfTag )
         {
            case 0x02:
            case 0x03:
            case 0x09:
            case 0x10:
            case 0x1A:
            case 0x1E:
            case 0x26:
            case 0x27:
            case 0x33:
            case 0x34:
            case 0x35:
            case 0x36:
            case 0x37:
            case 0x41:
            case 0x53:
            case 0x60:
            	break;

            default:   supported = false;
                       break;
         }
      }
      else
      {
         switch( byteOneOfTag )
         {
            case 0x71:
            case 0x72:
            case 0x82:
            case 0x84:
            case 0x91:
            case 0x95:
            case 0x9A:
            case 0x9C:
            case 0x96:
            	break;

            default: supported = false;
                     break;
         }
      }

      /* Copy supported MCard tags into outgoing host buffer. */
      if ( supported )
      {
         /* Subfield Tag */
         HOST_MESSAGE.iccdata[idx_out] = byteOneOfTag;
         if ( byteTwoOfTag != 0x00 )
            HOST_MESSAGE.iccdata[idx_out+1] = byteTwoOfTag;

         /* Subfield Length */
         HOST_MESSAGE.iccdata[idx_out+tag_len] = data_len;

         /* Subfield Data */
         memcpy( &HOST_MESSAGE.iccdata[idx_out+tag_len+1],
                 &emv_data_in[idx_in],
                  data_len );

         /* Increment the Outgoing Buffer index (tag+length+data) */
         idx_out += tag_len + 1 + data_len;
      }

      idx_in += data_len;
   }

   /* Get the length of the outgoing buffer. */
   sprintf( emv_out_len, "%03d", idx_out );
   memcpy( HOST_MESSAGE.iccdata_len, emv_out_len, 3 );

   return;
}

/******************************************************************************
 *
 *  NAME:         isalphanum
 *
 *  DESCRIPTION:  This Function will check wheather a given string is having only alpha 
 *                          numaric data or not
 *
 *  INPUTS:       string Addres of the inout string
 *                	   length - Length of th string
 *
 *  OUTPUTS:      None
 *                
 *  RTRN VALUE:   True when string contains only Alpha numeric
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/

INT isalphanum(pCHAR string, INT length )
{
   INT i;

   //length = (INT)strlen(string);

   for (i = 0; i < length; i++)
   {
      if (!isalnum(string[i]))
        return(false);
   }
   return(true);
}

INT pin_block_exists( pAUTH_TX p_auth_tx )
{
   INT  ret_val = true;

   if ( 0 == strlen(p_auth_tx->TLF01_details.pin_block) )
      ret_val = false;

   else if (blank_string(p_auth_tx->TLF01_details.pin_block,
                         sizeof(p_auth_tx->TLF01_details.pin_block)-1) )
      ret_val = false;

   return( ret_val );
}

INT ncmcrd_is_POS_PIN_transaction( pAUTH_TX p_auth_tx )
{
	INT ret_val = true;

	/* Check if txn has F52, F64 and F22 pos 3 is '1' */
	if (0 == strncmp(p_auth_tx->TLF01_details.handler_queue, "dcpiso", 6) &&
		true == pin_block_exists( p_auth_tx ) &&
		'\0' != p_auth_tx->TLF01_details.product_codes[4].amount[0] &&
		 '1' == p_auth_tx->TLF01_details.pos_entry_mode[3] )
	{
		ret_val = true;
	}
	else
	{
		ret_val = false;
	}
	return (ret_val );
}
/******************************************************************************
 *
 *  NAME:         Check_Reversal_Advice_Iss_txns
 *
 *  DESCRIPTION:  This Function will check wheather txn is Issuing and reversal ,
 *                          reversal advice or advice
 *
 *  INPUTS:       Auth_tx
 *
 *
 *  OUTPUTS:      TRUE OR FALSE
 *
 *  RTRN VALUE:   True it is a Reversal_Advice_Iss_txns
 *
 *  AUTHOR:       Raghavendra
 *
 ******************************************************************************/

INT ncmcrd_Check_Reversal_Advice_Iss_txns (pAUTH_TX p_auth_tx)
{
	if(strncmp(p_auth_tx->TLF01_details.handler_queue,"ncmcrd",6) == 0||
	   strncmp(p_auth_tx->TLF01_details.handler_queue,"ncmcr3",6) ==0 )
	{/* will include all ncmcrd and ncmcr3*/
		if(ncmcrd_Check_for_Reversal_txn(p_auth_tx) == true ||
		   ncmcrd_Check_for_Advice_txn(p_auth_tx)   == true )
		{
			return true ;
		}
		else
		{
			return false ;
		}
	}
	else
	{
		return false ;
	}
}

/******************************************************************************
 *
 *  NAME:         Check_Reversal_Advice_Iss_txns
 *
 *  DESCRIPTION:  This Function will check wheather txn is Issuing and reversal ,
 *                          reversal advice or advice
 *
 *  INPUTS:       Auth_tx
 *
 *
 *  OUTPUTS:      TRUE OR FALSE
 *
 *  RTRN VALUE:   True it is a Reversal_Advice_Iss_txns
 *
 *  AUTHOR:       Raghavendra
 *
 ******************************************************************************/

INT ncmcrd_Check_for_Reversal_txn(pAUTH_TX p_auth_tx)
{
	switch (p_auth_tx->TLF01_details.tx_key)
	{
		case AUTH_REVERSAL:
		case AUTH_REVERSAL_ADVICE:
		case AUTH_REVERSAL_REPEAT:
		case AUTH_REVERSAL_ADVICE_REPEAT:
		case AUTH_REVERSAL_RESPONSE:
		case AUTH_REVERSAL_ADVICE_RESPONSE:
		case AUTH_REVERSAL_REPEAT_RESPONSE:
		case AUTH_REVERSAL_ADVICE_REPEAT_RESPONSE:
			return true ;
		default:
			return false ;
	}
}

/******************************************************************************
 *
 *  NAME:         Check_Reversal_Advice_Iss_txns
 *
 *  DESCRIPTION:  This Function will check wheather txn is Issuing and reversal ,
 *                          reversal advice or advice
 *
 *  INPUTS:       Auth_tx
 *
 *
 *  OUTPUTS:      TRUE OR FALSE
 *
 *  RTRN VALUE:   True it is a Reversal_Advice_Iss_txns
 *
 *  AUTHOR:       Raghavendra
 *
 ******************************************************************************/
INT ncmcrd_Check_for_Advice_txn(pAUTH_TX p_auth_tx)
{
	char strMessageType [20] = {0} ;
	char strFirstTwoBytesOfMessageType [20] = {0} ;
	char strLastTwoBytesOfMessageType [20] = {0} ;

	strcpy (strMessageType, p_auth_tx->TLF01_details.message_type) ;
	strncpy (strFirstTwoBytesOfMessageType, strMessageType, 2) ;
	strncpy (strLastTwoBytesOfMessageType, strMessageType+2, 2) ;

	if (strncmp (strLastTwoBytesOfMessageType, "20" , 2) == 0 ||
		strncmp (strLastTwoBytesOfMessageType, "21" , 2) == 0 ||
		strncmp (strLastTwoBytesOfMessageType, "01" , 2) == 0 ||
		strncmp (strLastTwoBytesOfMessageType, "30" , 2) == 0 ||
		strncmp (strLastTwoBytesOfMessageType, "31" , 2) == 0 ||
		strncmp (strLastTwoBytesOfMessageType, "11" , 2) == 0 )
	{
		return true ;
	}
	else
	{
		return false ;
	}
}
/*******************************************************
 * This function will get the values from tf.ini file
 * from the section DATASERVER with keyname DB_ERROR_STATICS_FLAG &
 *	DB_ERROR_STATICS_VALUE which will help to decide the db statics
 *	logging mechanism.
 * *******************************************************/

void ncmcrd_get_error_warning_file_name_path(void )
{
   DWORD rc;
   CHAR  filename   [80] = {0};
   CHAR  tmpstr     [80] = {0};
   CHAR  ncmcrd_error_warning_file_size[5] = {0};

   /* Get path to the tf.ini file. */
   memset( tmpstr, 0x00, sizeof(tmpstr) );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, "tf.ini" );

   /*Read path for creating file in order to log db statics and db oracle error messages */
   rc = GetPrivateProfileString(
								  "DATASERVERS",             /* points to section name         */
								  "DB_STATICS_PATH",       	/* points to key name             */
								   "",  					/* Default string                 */
								   ncmcrd_error_warning_file_path,              	/* points to destination buffer   */
								   sizeof(ncmcrd_error_warning_file_path)-1,   	 /* size of destination buffer     */
								   filename                  /* points to ini filename         */
						 	   );

   rc = GetPrivateProfileString(
								  "ERROR_WARNING",             /* points to section name         */
								  AppName,       	/* points to key name             */
								  "",  					/* Default string                 */
								  ncmcrd_module_error_warning_file_name,              	/* points to destination buffer   */
								  sizeof(ncmcrd_module_error_warning_file_name)-1,   	 /* size of destination buffer     */
								  filename                  /* points to ini filename         */
								);

   rc = GetPrivateProfileString(
							  "ERROR_WARNING",             /* points to section name         */
							  "ERROR_WARNING_FILE_SIZE",       	/* points to key name             */
							  "500",  					/* Default string                 */
							  ncmcrd_error_warning_file_size,              	/* points to destination buffer   */
							  sizeof(ncmcrd_error_warning_file_size)-1,   	 /* size of destination buffer     */
							  filename                  /* points to ini filename         */
							);

	/* File size size conversion */
	Max_File_Size_Defined = atoi(ncmcrd_error_warning_file_size);
	if(Max_File_Size_Defined <= 0)
	{
		Max_File_Size_Defined = 500 ;
	}
	Max_File_Size_Defined = Max_File_Size_Defined * ONE_MB_IN_BYTES ; /* 1 MB = 1048576 BYTES*/

   if((strlen(ncmcrd_error_warning_file_path) > 0) &&
	  (strlen(ncmcrd_module_error_warning_file_name)> 0))
   {
	   ncmcrd_create_Error_Filename();
   }
}

INT ncmcrd_Log_error_warning_to_File(pCHAR Error_Buf,int sev,pCHAR func, int detail)
{
	INT   ret_val = 0;
	INT   len=0;
	INT   cardlen = 0;
	INT   path_len = 0;
	INT   nIndex = 0;
	CHAR  time_date[25]  ={0};
	CHAR  timestamp[14]  ={0};
	CHAR  current_mmdd[5]={0};
	CHAR Buffer[1024]={0};
	CHAR card_tid_mid_details [256] = {0};
	CHAR tempcard[21] = {0};
	CHAR masked_cardnum[21] = {0};
	char *cardIndex = NULL ;
	FILE *fp;
	UINT file_size = 0 ;
	char buf[256] = {0} ;


	/* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
	ptetime_get_timestamp( time_date );

	/* Pick out the month and day to compare to filename extenstion. */
    memcpy( current_mmdd,   time_date+5, 2 );
    memcpy( current_mmdd+2, time_date+8, 2 );

	/* Compare filename extension to current date. */
    len = strlen(ncmcrd_Error_warning_Filename);
    path_len = strlen(ncmcrd_error_warning_file_path);
    if( len == 0 ||
    	path_len==0 )
    {
    	LogEvent(Error_Buf,INFO_MSG);
    	return 0;
    }
    if ( 0 != strcmp(&ncmcrd_Error_warning_Filename[len-4], current_mmdd) )
    {
		/* Now create the new filename.*/
    	ncmcrd_create_Error_Filename();
    }
	if((fp = fopen(ncmcrd_Error_warning_Filename,"a+b"))==NULL)
	{
		LogEvent(Error_Buf,INFO_MSG);
		return 0;
	}
	strcpy(Buffer,time_date);
	strcat(Buffer,":");
	if(sev == 1)
	{
		strcat(Buffer," INFO");
	}
	else if (sev == 2)
	{
		strcat(Buffer," WARN");
	}
	else
	{
		strcat(Buffer," ERROR");
	}
	strcat(Buffer,": ");
	strcat(Buffer,Error_Buf);
	strcat(Buffer," ");
	strcat(Buffer, func);
	strcat(Buffer,"\n");
	len=strlen(Buffer);

	if(fwrite(Buffer, len, NUM_SIZE_WRITES, fp)==NUM_SIZE_WRITES)
	{
		// Do nothing fall below
	}
	else
	{
		LogEvent(Buffer,INFO_MSG);
		fclose(fp);
		return  0;
	}
	fseek (fp, 0, SEEK_END);
	file_size=ftell (fp);

	if( file_size >= Max_File_Size_Defined )
	{
		sprintf(buf,"ERROR_WARNING_FILE  size is exceeding the permissible size, need attention" );
		TxUtils_Send_Msg_To_Operator( 1, buf,1, ALERT_MSG, "", 4, INFO_ERROR,NULL,NULL,NULL );
	}
	fclose(fp);
	return  0;
}

void ncmcrd_create_Error_Filename()
{
	CHAR  system_date[25] = {0};

   /* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
	ptetime_get_timestamp( system_date );

    strcpy( ncmcrd_Error_warning_Filename, ncmcrd_error_warning_file_path );
    strcat( ncmcrd_Error_warning_Filename, ncmcrd_module_error_warning_file_name );
	strcat( ncmcrd_Error_warning_Filename, ".EW." );
    strncat(ncmcrd_Error_warning_Filename, system_date,   4 );  /* YYYY */
    strncat(ncmcrd_Error_warning_Filename, system_date+5, 2 );  /* MM   */
    strncat(ncmcrd_Error_warning_Filename, system_date+8, 2 );  /* DD   */
}
/******************************************************************************
 *
 *  NAME:         ncmcrd_log_message
 *
 *  DESCRIPTION:  This function takes a string and input parameters and
 *                formats a message to be sent to either Monitor, Event Log
 *                or both.  Based on inputs, it sets the severity.
 *
 *  INPUTS:       dest - 1 = Monitor, 2 = File/Event log, 3 = Both
 *                sev  - 1 = Info,    2 = Warning,   3 = Error
 *                msg  - Text message to be logged
 *                func - Name of function logging the error
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncmcrd_log_message( INT dest, INT sev, pCHAR msg, pCHAR func, int details ,pCHAR card_num )
{
   INT  monitor_flag = 0;
   INT  eventlog_flag = 0;
   INT  msg_type = 0;
   BYTE severity = 0;
   BYTE log_type[20] = {0};
   CHAR text_message [512] = {0};
   INT  Appname_len = 0;
   CHAR print_buffer[256] = {0} ;
   INT text_message_len = 0;
   CHAR card_num_msg[56] = {0};

   /* Set Monitor flag */
   if ( dest == 1  ||  dest == 3 )
      monitor_flag = 1;
   else
      monitor_flag = 0;

   /* Set Event Log flag */
   /*if ( dest > 1 )
      eventlog_flag = 1;
   else
      eventlog_flag = 0;*/

   /* Set message type and severity */
   if ( sev == 2 )
   {
      msg_type = WARN_MSG;
      severity = '3';
      strcpy( log_type, WARNING_ERROR );
   }
   else if ( sev == 1 )
   {
      msg_type = INFO_MSG;
      severity = '0';
      strcpy( log_type, INFO_ERROR );
   }
   else if ( sev == 3 )
   {
      msg_type = ALERT_MSG;
      severity = '4';
      strcpy( log_type, FATAL_ERROR );
   }

   /* Make sure text message is not too long. */
   sprintf(text_message,"%s ,",AppName);
   Appname_len = strlen(text_message);

   memcpy( text_message + Appname_len, msg, (sizeof(text_message)-1 -Appname_len));
   if (card_num[0] == NULL)
   {
	   // No card number, hence masking not required
   }
   else
   {
	   Mask_card_number(card_num);
	   sprintf(card_num_msg, "Card num : %s",masked_card_num);
	   strcat(text_message,card_num_msg);
   }
   /* Call function to post the message. */
   ncmcrd_Log_error_warning_to_File(text_message,sev,func,details);
   if(monitor_flag == 1)
   {
	   text_message_len = strlen(text_message);
	   if(text_message_len > 255)
	   {
		   strncpy(print_buffer,text_message,254); // Maximum msg sent to moniotr is 256
		   TxUtils_Send_Msg_To_Operator( monitor_flag, print_buffer,
									 	 eventlog_flag, msg_type, func,
										 severity, log_type,
										 NULL, NULL,NULL );
	   }
	   else
	   {
		   TxUtils_Send_Msg_To_Operator( monitor_flag, text_message,
									 	 eventlog_flag, msg_type, func,
										 severity, log_type,
										 NULL, NULL,NULL );
	   }
   }

   return;
}

void Mask_card_number(char * card_num)
{
	int cardlen = 0;
	int nIndex 	= 0;
	memset(masked_card_num ,0x00, sizeof(masked_card_num));
	cardlen = strlen( card_num );
	if (cardlen > 0)
	{
		/* Masking card number first 5 and last 4 in clear form remaining all masked. */
		strncpy( masked_card_num, card_num, cardlen);
		for ( nIndex = cardlen-5; nIndex >= cardlen-11; nIndex-- )
		{
			masked_card_num[nIndex] = '*';
		}
	}
}

/******************************************************************************
 *
 *  NAME:         MC_isnum
 *
 *  DESCRIPTION:  This function checks for a string to be all numbers.
 *
 *  INPUTS:       in_str - String to be checked
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if string is all numbers, else false
 *
 *  AUTHOR:       Abhishek Verma
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT MC_isnum(pCHAR in_str)
{
   INT i = 0;
   INT length = 0;

   length = (INT)strlen(in_str);

   if( 0 == length)
   {
	   return(true);
   }
   else
   {
	   for (i = 0; i < length; i++)
	   {
		   if (!isdigit(in_str[i]))
			   return(false);
	   }
   }
   return(true);
}

/******************************************************************************
 *
 *  NAME:         MC_isalphanum
 *
 *  DESCRIPTION:  This function checks for a string to be all numbers and / or
 *                alphabetical characters.
 *
 *  INPUTS:       in_str - String to be checked
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if string is all alphanumeric, else false
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT MC_isalphanum(pCHAR string)
{
   INT i = 0;
   INT length = 0;

   length = (INT)strlen(string);
   if(length == 0)
   {
   		return (true);
   }
   for (i = 0; i < length; i++)
   {
      if (!isalnum(string[i]))
        return(false);
   }
   return(true);
}

void ncmcrd_read_tf_ini( pCHAR err_buf )
{
	   DWORD rc;
	   CHAR  filename   [80] = {0};
	   CHAR  readstring [80] = {0};
	   CHAR  readvalue	[2]	 = {0};
	   CHAR  responsecode[3]	 = {0};
	   CHAR	 pgy_id[12]			 = {0};
	   /* Get path to the tf.ini file. */
	   memset( readstring, 0x00, sizeof(readstring) );
	   GetPinnacleConfigDirectory(readstring);
	   sprintf(filename, "%s%s", readstring, "tf.ini" );

	   /*Read path for creating file in order to log db statics and db oracle error messages */
	   rc = GetPrivateProfileString(  "3DSECURE_VERSION",             /* points to section name         */
									  "MC3DVERSION",       	/* points to key name             */
									   "",  					/* Default string                 */
									   readvalue,              /* points to destination buffer   */
									   sizeof(readvalue)-1,   	 /* size of destination buffer     */
									   filename                  /* points to ini filename         */
							 	   );

	   Mc3Dsecureversion = readvalue[0];

	   rc = GetPrivateProfileString(  "MC_ASI_RESPONSE_CODE",             /* points to section name         */
									  "MC_ASI_RC",       	/* points to key name             */
									   "63",  					/* Default string                 */
									   responsecode,              /* points to destination buffer   */
									   sizeof(responsecode)-1,   	 /* size of destination buffer     */
									   filename                  /* points to ini filename         */
							 	   );

	   strncpy(mc_asi_txn_rc,responsecode,2);

		/* APR MNDT 2024 MC-8: DE48TG37SF05 */
	   rc = GetPrivateProfileString(  "MERCHANT_PAYMENT_GATEWAY",             /* points to section name         */
									  "MC_PGY_ID",       	/* points to key name             */
									   "9999997",  					/* Default string                 */
									   pgy_id,              /* points to destination buffer   */
									   sizeof(pgy_id)-1,   	 /* size of destination buffer     */
									   filename                  /* points to ini filename         */
							 	   );

	   if(strlen(pgy_id) == 0)
		   strncpy(mc_merchnt_pgway_id, "9999997", 7);
	   else
		   strncpy(mc_merchnt_pgway_id, pgy_id, strlen(pgy_id));

	/*   rc = GetPrivateProfileString(  "SCHEME_TIMEOUT_RC",           // points to section name
									  "SCHEME_TIMEOUT_RC_FOR_MC",       	// points to key name
									   "19",  												// Default string
									   readvalue,              								//points to destination buffer
									   sizeof(readvalue)-1,   	 							// size of destination buffer
									   filename                  							// points to ini filename
							 	   );

	   strncpy(mc_scheme_timeout_rc,responsecode,2);
	   if(rc == 0)
	   {
		   ncmcrd_log_message( 2, 1, "SCHEME_TIMEOUT_RC_FOR_MC Value not read from tf.ini","ncmcrd_read_tf_ini",0);
	   }*/


}

BOOLEAN ncmcrd_Is_AsiaPay_Transaction(pAUTH_TX p_auth_tx)
{
	if((0==strncmp(p_auth_tx->TLF01_details.message_type, "0200",4)) &&
	   (0==strncmp(p_auth_tx->TLF01_details.pos_condition_code, ECOM_POS_CC,2)) &&
	   (0 == strncmp(p_auth_tx->TLF01_details.pos_entry_mode,ECOM_POS_ENTRYMODE,3)))
	{
		return true;
	}
	else if((0==strncmp(p_auth_tx->TLF01_details.message_type, "0100",4)) &&
			(0==strncmp(p_auth_tx->TLF01_details.handler_queue, "dcpiso",6)) &&
			   (0==strncmp(p_auth_tx->TLF01_details.pos_condition_code, ECOM_POS_CC,2)) &&
			   (0 == strncmp(p_auth_tx->TLF01_details.pos_entry_mode,ECOM_POS_ENTRYMODE,3)))
	{
		return true;
	}
	else
	{
		return false;
	}
}

INT ncmcrd_send_reversal_to_host(pAUTH_TX p_auth_tx)
{
   CHAR   unique_tran_str_id[21] = "";
   CHAR   date_str [ 9] = "";
   CHAR   time_str [ 7] = "";
   CHAR   time_date[25] = "";
   CHAR   temp_unique_tran[15]="";
   INT     tid_len = 0;
   INT     unique_tran_str_id_len=0;

	p_auth_tx->TLF01_details.tx_key = AUTH_REVERSAL;

	/* Create a unique transaction id for the new auth_tx record. */
	ptetime_get_strtime( unique_tran_str_id );
	memset( p_auth_tx->TLF01_details.primary_key.transaction_id, 0,
		   sizeof(p_auth_tx->TLF01_details.primary_key.transaction_id) );
	tid_len= strlen(p_auth_tx->TLF01_details.terminal_id);
	if(tid_len == 8)
	{
		memcpy( p_auth_tx->TLF01_details.primary_key.transaction_id,
			   p_auth_tx->TLF01_details.terminal_id,
				sizeof( p_auth_tx->TLF01_details.terminal_id) - 1 );
		strncat( p_auth_tx->TLF01_details.primary_key.transaction_id,
				unique_tran_str_id+2, 12 );
	}
	else
	{
	   /* We have to now prepare 20 digit unique Transaction ID.
		 unique_tran_str_id will be having 14 digit length
		 now we will make it as 20 by adding YYMMDD in the beginning of it.
		 20 byte will look like
		 YYMMDD"unique_tran_str_id"
		 for example if unique_tran_str_id is "1444752896.334" then it will become
		 YYMMDD1444752896.334
		 Based on ther terminal length we will copy data from new unique_tran_str_id
	   */
	   strncpy(temp_unique_tran,unique_tran_str_id,14);
	   memset(unique_tran_str_id,0x00,sizeof(unique_tran_str_id));

	   strncpy(unique_tran_str_id,
				p_auth_tx->TLF01_details.date_yyyymmdd+2, 6); //YYMMDD
	   strncat (unique_tran_str_id,temp_unique_tran,14);

	   unique_tran_str_id_len= strlen(unique_tran_str_id);
	   memcpy( p_auth_tx->TLF01_details.primary_key.transaction_id,
			   p_auth_tx->TLF01_details.terminal_id, tid_len );
	   strncat( p_auth_tx->TLF01_details.primary_key.transaction_id,
				unique_tran_str_id+tid_len,
				(unique_tran_str_id_len-tid_len) );

		p_auth_tx->TLF01_details.primary_key.transaction_id[20]=0x00;
	}
	/* Set up the 'original' information for field 90. */
	strncpy( p_auth_tx->TLF01_details.orig_sys_trace_num,
			p_auth_tx->TLF01_details.sys_trace_audit_num,
			sizeof(p_auth_tx->TLF01_details.orig_sys_trace_num));

	strncpy( p_auth_tx->orig_acq_inst_id,
	ncf01_i.acquirer_inst_id_code,
	sizeof(p_auth_tx->orig_acq_inst_id));

	strncpy( p_auth_tx->TLF01_details.orig_message, "0100" , 4 );
	strncpy( p_auth_tx->TLF01_details.message_type, "0400" ,4);
	strncpy( p_auth_tx->TLF01_details.orig_retrieval_ref_num,
			 p_auth_tx->TLF01_details.retrieval_ref_num,
			 sizeof(p_auth_tx->TLF01_details.orig_retrieval_ref_num));

	/* Send out the reversal request to the host. */
	p_auth_tx->TLF01_details.general_status = GS_TXN_PERF_REQ;
	return(send_request_to_host( p_auth_tx ));


}

BOOLEAN ncmcrd_check_for_partial_approved_txn(pAUTH_TX p_auth_tx)
{
	if((0 == strncmp(p_auth_tx->TLF01_details.handler_queue,"dcpiso",6) ||
	    0 == strncmp(p_auth_tx->TLF01_details.handler_queue,"VOICE",5)) &&
		0 == strncmp(p_auth_tx->TLF01_details.response_code,"10",2))
	{
		return true;
	}
	else if((0 == strncmp(p_auth_tx->TLF01_details.handler_queue,"dcpiso",6) ||
		    0 == strncmp(p_auth_tx->TLF01_details.handler_queue,"VOICE",5)) &&
			(0 == strncmp(p_auth_tx->TLF01_details.response_code,"05",2)&&
			 0 == strncmp(p_auth_tx->TLF01_details.product_codes[13].quantity,"10",2)))
	{
		return true;
	}

	return false;
}

BOOLEAN ncmcrd_check_for_ASI_not_Supported_RC(pAUTH_TX p_auth_tx)
{
	if(0 == strncmp(p_auth_tx->TLF01_details.response_code,"12",2) ||
	   0 == strncmp(p_auth_tx->TLF01_details.response_code,"13",2) ||
	   0 == strncmp(p_auth_tx->TLF01_details.response_code,"30",2) ||
	   0 == strncmp(p_auth_tx->TLF01_details.response_code,"51",2) ||
	   0 == strncmp(p_auth_tx->TLF01_details.response_code,"57",2) ||
	   0 == strncmp(p_auth_tx->TLF01_details.response_code,"58",2))
	{
		return true;
	}
	return false;
}

BOOLEAN ncmcrd_check_for_ASI_txn(pAUTH_TX p_auth_tx)
{
	if((0 == strncmp(p_auth_tx->TLF01_details.product_codes[1].code,"stndin",6)) &&
	   (0 == strncmp(p_auth_tx->TLF01_details.handler_queue,"ncmcrd",6) ||
	    0 == strncmp(p_auth_tx->TLF01_details.handler_queue,"ncmcr3",6))&&
	   (0 == strncmp(p_auth_tx->TLF01_details.message_type,"0100",4) 	||
		0 == strncmp(p_auth_tx->TLF01_details.message_type,"0110",4))	&&
	   (MC_ASI_TXN_IDENTIFIER_VALUE == p_auth_tx->TLF01_details.product_codes[16].amount[6]))
	{
		return true;
	}
	return false;
}
void build_DE48_TAG37_foracq_txn(pAUTH_TX p_auth_tx, int *itemp)
{

	if( p_auth_tx->TLF01_details.orig_auth_number[0]!= '\0' &&
	    strcmp(p_auth_tx->TLF01_details.orig_auth_number,"000")!=0)
	{
		*itemp += 11;
		strncat( HOST_MESSAGE.addl_data, DE48_TAG37 ,6);
		strncat( HOST_MESSAGE.addl_data, "03" ,2); /* SF04 id length we can send only 2 byte*/
		strncat( HOST_MESSAGE.addl_data, p_auth_tx->TLF01_details.orig_auth_number ,3);

	}
}

#ifdef MC07_OCT2023
INT parse_field_108( char *src_len, char *src, pAUTH_TX p_auth_tx )
{

#define  FIELD108_MAX_LEN  999

   INT  ret_val = 0;
   INT  sub_element_len = 0;
   INT  tag_element_len = 0;
   INT  MainIndex = 0;
   INT  TotalLen = 0;
   INT SubFieldIDLen=0;
   CHAR tag[3] = {0};
   CHAR len[3] = {0};
   CHAR Element_tag[3] = {0};
   CHAR Element_len[3] = {0};
   char First_name[36]={0};
   char Middle_name[2]={0};
   char Last_name[36]={0};
   INT  tagIndex = 0;
    INT  Tag_TotalLen = 0;
    char Total_len[3]={0};

    strncpy(Total_len,src_len,3);

    TotalLen = atoi( Total_len );

      /* MainIndex starts at 1 to skip over the Transaction Category Code. */
   for( MainIndex=0; MainIndex<TotalLen && MainIndex<FIELD108_MAX_LEN; )
   {
      /* Get subfield tag (name of subfield). */
	  tagIndex=0;
      memset( Element_tag, 0x00, sizeof( Element_tag ) );
      strncpy( Element_tag, &src[MainIndex], 2 );
      MainIndex += 2;

	     memset( Element_len, 0x00, sizeof(Element_len) );
         memcpy( Element_len, &src[MainIndex], 3 );
         sub_element_len = atoi( Element_len );
         MainIndex += 3;

		 if ( 0 == strncmp(Element_tag, DE108_SE01, 2) ||0 == strncmp(Element_tag, DE108_SE02, 2))
		 {
			 for(tagIndex=MainIndex; tagIndex<sub_element_len;)
			 {
				 memset( tag, 0x00, sizeof( tag ) );
				 strncpy( tag, &src[tagIndex], 2 );

				 tagIndex += 2;

			     memset( len, 0x00, sizeof(len) );
			     tag_element_len=0;
		         memcpy( len, &src[tagIndex], 2 );
		         tag_element_len = atoi( len );

		         tagIndex += 2;

				if( 0 == strncmp(tag, DE108_SE01, 2))
				{
					/* max 35 byte, name01 is used here*/
					 strncpy(First_name,&src[tagIndex],tag_element_len);
					 if(tag_element_len > 30)
					 {
						 memcpy(p_auth_tx->MCF01_details.name01,First_name,30);
						 memcpy(p_auth_tx->MCF01_details.name02,First_name+30,tag_element_len-30);
					 }
					 else
					 {
						 memcpy(p_auth_tx->MCF01_details.name01,First_name,tag_element_len);
					 }
				}
				else if( 0 == strncmp(tag, DE108_SE02, 2))
				{
					strncpy(Middle_name,&src[tagIndex],tag_element_len);
					strncat(p_auth_tx->MCF01_details.state,Middle_name,tag_element_len);
				}
				else if( 0 == strncmp(tag, DE108_SE03, 2))
				{
					/*max 35 byte, address01 is used here*/
						strncpy(Last_name,&src[tagIndex],tag_element_len);
						if(tag_element_len > 30)
						 {
							 memcpy(p_auth_tx->MCF01_details.address01,Last_name,30);
							 memcpy(p_auth_tx->MCF01_details.address02,Last_name+30,tag_element_len-30);
						 }
						 else
						 {
							 memcpy(p_auth_tx->MCF01_details.address01,Last_name,tag_element_len);
						 }
				}
		        tagIndex += tag_element_len;
			 }
			 /*print the log
			  * */
			   memset(Log_Bufr,0x00,sizeof(Log_Bufr));
			   sprintf(Log_Bufr,"DE108 first name(%s), middle name(%s) and last name(%s) Term Id:%s ,Merch Id: %s RRN: %s ",
					   First_name,Middle_name,Last_name,p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
			   ncmcrd_log_message( 2, 1, Log_Bufr , "parse_field_108" , 0 ,p_auth_tx->TLF01_details.card_num);

		 }
		 else if (0 == strncmp(Element_tag, DE108_SE03, 2))
		 {
				/* APR MNDT 2024 MC-6: DE108SE03SF05 */
			 tagIndex = MainIndex;
			 tag_element_len = 0;
			 for (tagIndex = MainIndex; tagIndex < (sub_element_len + MainIndex);)
			 {
				 memset(tag, 0x00, sizeof(tag));
				 strncpy( tag, &src[tagIndex], 2 );

				 tagIndex += 2;

			     memset( len, 0x00, sizeof(len) );
		         memcpy( len, &src[tagIndex], 2 );
		         tag_element_len = atoi( len );
				 tagIndex += 2;

				if (0 == strncmp(tag, DE108_SE03_SF01, 2))
				{
					/* do not copy, just iterate */
				}
				else if (0 == strncmp(tag, DE108_SE03_SF03, 2))
				{
					/* do not copy, just iterate */
				}
				else if (0 == strncmp(tag, DE108_SE03_SF05, 2))
				{
					strncpy(p_auth_tx->TLF01_details.product_codes[10].quantity, &src[tagIndex], tag_element_len);
				}
				 tagIndex += tag_element_len;
			 }

			sprintf(Log_Bufr," DE108 SE03 SF05 Money Send Txn purpose received, value is(%s)", p_auth_tx->TLF01_details.product_codes[10].quantity);
			ncmcrd_log_message( 2, 1, Log_Bufr , "parse_field_108" , 0 ,p_auth_tx->TLF01_details.card_num);

		 }
		 else if ( 0 == strncmp(Element_tag, DE108_SE07, 2))
		 {
			 tagIndex=MainIndex;
			 strncpy( tag, &src[tagIndex], 2 );

			 tagIndex += 2;

		     memset( len, 0x00, sizeof(len) );
	         memcpy( len, &src[tagIndex], 2 );
	         tag_element_len = atoi( len );

	         tagIndex += 2;
	         if(0 == strncmp(tag,"01",2))
	         {
	        	 strncpy(p_auth_tx->MCF01_details.credit_purch_max,&src[tagIndex],2);
	         }

	         if(0 == strncmp(p_auth_tx->MCF01_details.credit_purch_max,"10",2))
	         {
	        	 ncmcrd_log_message( 2, 1, "Receiver/Recipient Data Verification Request received" , "parse_field_108" , 0 ,p_auth_tx->TLF01_details.card_num);
	         }
	         else if(0 == strncmp(p_auth_tx->MCF01_details.credit_purch_max,"11",2))
	         {
	        	 ncmcrd_log_message( 2, 1, "Sender Data Verification Request received" , "parse_field_108" , 0 ,p_auth_tx->TLF01_details.card_num);
	         }
	         else
	         {
	        	 sprintf(Log_Bufr,"unknown DE108 SE07 SF01 received, value is(%s)",p_auth_tx->MCF01_details.credit_purch_max);
	        	 ncmcrd_log_message( 2, 1, Log_Bufr , "parse_field_108" , 0 ,p_auth_tx->TLF01_details.card_num);
	         }
		 }

		 MainIndex += sub_element_len;
   }
   return( ret_val );
}

#endif


#ifdef MC01_APR2024
INT parse_field_105( char *src_len, char *src, pAUTH_TX p_auth_tx )
{
    #define  FIELD105_MAX_LEN  999

    INT  ret_val 		 = 0;
    INT  sub_element_len = 0;
    INT  tag_element_len = 0;
    INT  MainIndex 		 = 0;
    INT  TotalLen 		 = 0;
    CHAR Element_tag[4]  = {0};
    CHAR Element_len[4]  = {0};
    CHAR Total_len[4]    = {0};

    strncpy(Total_len, src_len, 3);

    TotalLen = atoi( Total_len );

    /* MainIndex starts at 1 to skip over the Transaction Category Code. */
    for( MainIndex=0; MainIndex<TotalLen && MainIndex<FIELD105_MAX_LEN; )
    {
    	/* Get subfield tag (name of subfield). */
    	memset( Element_tag, 0x00, sizeof( Element_tag ) );
    	strncpy( Element_tag, &src[MainIndex], 3 );
    	MainIndex += 3;

    	memset( Element_len, 0x00, sizeof( Element_len ) );
    	strncpy( Element_len, &src[MainIndex], 3 );
    	sub_element_len = atoi(Element_len);
    	MainIndex += 3;

		if (0 == strncmp(Element_tag, DE105_SE01, 3))
		{
			/* if(sub_element_len > 10)
				 ncmcrd_log_message( 2, 1, "Only storing 10 bytes TLID data", "parse_field_105", 0, p_auth_tx->TLF01_details.card_num); */

			/*Store 10 bytes Transaction Link Identifier in product_codes[19].amount*/
			memcpy(p_auth_tx->TLF01_details.product_codes[19].amount, &src[MainIndex], 10);
			memcpy(p_auth_tx->TLF01_details.product_codes[1].amount, &src[MainIndex + 10], 12);

		 }
		 MainIndex += sub_element_len;
    }
    return( ret_val );
}

#endif

INT ncmcrd_check_if_trx_is_bdo_pos_acq_trx( pAUTH_TX p_auth_tx  )
{
	 if((0 == strncmp(p_auth_tx->TLF01_details.handler_queue,
			 	 	  BDO_POS_ACQ_TXN_HANDLER_QUEUE,
					  BDO_POS_ACQ_TXN_HANDLE_QUEUE_LEN)))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}


/*MobilePOS NFC Tap2Phone trx*/
INT ncmcrd_check_if_PEM_is_MobilePOS_NFC_Tap2Phone_trx_with_pin( pAUTH_TX p_auth_tx  )
{
	 if(0 == strncmp(p_auth_tx->TLF01_details.pos_entry_mode,
					 POS_MOBILE_NFC_TAP2PHONE_TXN_WITH_PIN_PEM,
					 POS_ENTRY_MODE_LEN))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

INT ncmcrd_check_if_PEM_is_MobilePOS_NFC_Tap2Phone_trx_without_pin( pAUTH_TX p_auth_tx  )
{
	if(0 == strncmp(p_auth_tx->TLF01_details.pos_entry_mode,
			 	 	POS_MOBILE_NFC_TAP2PHONE_TXN_WITHOUT_PIN_PEM,
					POS_ENTRY_MODE_LEN))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

INT ncmcrd_check_if_trx_qualify_for_MobilePOS_NFC_Tap2Phone_trx ( pAUTH_TX p_auth_tx  )
{
	if((0 == strncmp(p_auth_tx->TLF01_details.message_type,
			 	 	 POS_ACQ_SALE_MESSAGE_TYPE,
					 ISO_MESSAGE_TYPE_LEN))||
	   (0 == strncmp(p_auth_tx->TLF01_details.message_type,
			   	     POS_ACQ_AUTH_MESSAGE_TYPE,
					 ISO_MESSAGE_TYPE_LEN))||
	   (0 == strncmp(p_auth_tx->TLF01_details.message_type,
			   	   	 POS_ACQ_REVERSAL_MESSAGE_TYPE,
					 ISO_MESSAGE_TYPE_LEN))||
	   (0 == strncmp(p_auth_tx->TLF01_details.message_type,
			   	   	 POS_ACQ_REVERSAL_ADVICE_MESSAGE_TYPE,
					 ISO_MESSAGE_TYPE_LEN)))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}
INT ncmcrd_check_if_trx_is_mc_MobilePOS_NFC_Tap2Phone_trx_with_pin( pAUTH_TX p_auth_tx  )
{
	if((true == ncmcrd_check_if_PEM_is_MobilePOS_NFC_Tap2Phone_trx_with_pin(p_auth_tx)) &&
	   (true == ncmcrd_check_if_trx_is_bdo_pos_acq_trx(p_auth_tx))&&
	   (true == ncmcrd_check_if_trx_qualify_for_MobilePOS_NFC_Tap2Phone_trx(p_auth_tx)))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

INT ncmcrd_check_if_trx_is_mc_MobilePOS_NFC_Tap2Phone_trx_without_pin( pAUTH_TX p_auth_tx  )
{
	if((true == ncmcrd_check_if_PEM_is_MobilePOS_NFC_Tap2Phone_trx_without_pin(p_auth_tx)) &&
	   (true == ncmcrd_check_if_trx_is_bdo_pos_acq_trx(p_auth_tx))&&
	   (true == ncmcrd_check_if_trx_qualify_for_MobilePOS_NFC_Tap2Phone_trx(p_auth_tx)))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

/*****************************/

/*Swiftpass BDO Pay Credit Trx*/
INT ncmcrd_check_if_PEM_is_Swiftpass_BDO_Pay_credit_trx_with_pin( pAUTH_TX p_auth_tx  )
{
	 if(0 == strncmp(p_auth_tx->TLF01_details.pos_entry_mode,
			 	 	 POS_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITH_PIN_PEM,
					 POS_ENTRY_MODE_LEN))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

INT ncmcrd_check_if_PEM_is_Swiftpass_BDO_Pay_credit_trx_without_pin( pAUTH_TX p_auth_tx  )
{
	if(0 == strncmp(p_auth_tx->TLF01_details.pos_entry_mode,
					POS_SWIFTPASS_BDO_PAY_CREDIT_TXN_WITHOUT_PIN_PEM,
					POS_ENTRY_MODE_LEN))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

INT ncmcrd_check_if_trx_qualify_for_Swiftpass_BDO_Pay_credit_trx ( pAUTH_TX p_auth_tx  )
{
	if((0 == strncmp(p_auth_tx->TLF01_details.message_type,
			 	 	 POS_ACQ_SALE_MESSAGE_TYPE,
					 ISO_MESSAGE_TYPE_LEN))||
	   (0 == strncmp(p_auth_tx->TLF01_details.message_type,
			   	     POS_ACQ_AUTH_MESSAGE_TYPE,
					 ISO_MESSAGE_TYPE_LEN))||
	   (0 == strncmp(p_auth_tx->TLF01_details.message_type,
			   	   	 POS_ACQ_REVERSAL_MESSAGE_TYPE,
					 ISO_MESSAGE_TYPE_LEN))||
	   (0 == strncmp(p_auth_tx->TLF01_details.message_type,
			   	   	 POS_ACQ_REVERSAL_ADVICE_MESSAGE_TYPE,
					 ISO_MESSAGE_TYPE_LEN)))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}
INT ncmcrd_check_if_trx_is_mc_Swiftpass_BDO_Pay_credit_trx_with_pin( pAUTH_TX p_auth_tx  )
{
	if((true == ncmcrd_check_if_PEM_is_Swiftpass_BDO_Pay_credit_trx_with_pin(p_auth_tx)) &&
	   (true == ncmcrd_check_if_trx_is_bdo_pos_acq_trx(p_auth_tx))&&
	   (true == ncmcrd_check_if_trx_qualify_for_Swiftpass_BDO_Pay_credit_trx(p_auth_tx)))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

INT ncmcrd_check_if_trx_is_mc_Swiftpass_BDO_Pay_credit_trx_without_pin( pAUTH_TX p_auth_tx  )
{
	if((true == ncmcrd_check_if_PEM_is_Swiftpass_BDO_Pay_credit_trx_without_pin(p_auth_tx)) &&
	   (true == ncmcrd_check_if_trx_is_bdo_pos_acq_trx(p_auth_tx))&&
	   (true == ncmcrd_check_if_trx_qualify_for_Swiftpass_BDO_Pay_credit_trx(p_auth_tx)))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

/*****************************/

/*E-commerce Entry Mode from Swiftpass-BDO Pay Transactions*/
INT ncmcrd_check_if_trx_ecommerce_entry_mode_from_swiftpass_BDOPay_trx ( pAUTH_TX p_auth_tx  )
{
	if((0 == strncmp(p_auth_tx->TLF01_details.message_type,
			 	 	 POS_ACQ_SALE_MESSAGE_TYPE,
					 ISO_MESSAGE_TYPE_LEN))||
	   (0 == strncmp(p_auth_tx->TLF01_details.message_type,
			   	     POS_ACQ_AUTH_MESSAGE_TYPE,
					 ISO_MESSAGE_TYPE_LEN))||
	   (0 == strncmp(p_auth_tx->TLF01_details.message_type,
			   	   	 POS_ACQ_REVERSAL_MESSAGE_TYPE,
					 ISO_MESSAGE_TYPE_LEN))||
	   (0 == strncmp(p_auth_tx->TLF01_details.message_type,
			   	   	 POS_ACQ_REVERSAL_ADVICE_MESSAGE_TYPE,
					 ISO_MESSAGE_TYPE_LEN)))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

INT ncmcrd_check_if_trx_ecommerce_POS_entry_mode_from_swiftpass_BDOPay_trx( pAUTH_TX p_auth_tx  )
{
	if((0 == strncmp(p_auth_tx->TLF01_details.pos_entry_mode,
					 POS_ECOMMERCE_ENTRY_MODE_FROM_SWIFTPASS_BDOPAY_TXN_PEM,
					 POS_ENTRY_MODE_LEN))&&
	   (0 == strncmp(p_auth_tx->TLF01_details.pos_condition_code,
			   	   	 POS_ECOM_CONDITION_CODE,
					 POS_CONDITION_CODE_LEN)))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

INT ncmcrd_check_if_trx_is_mc_ecommerce_swiftpass_BDOPay_trx( pAUTH_TX p_auth_tx  )
{
	if((true == ncmcrd_check_if_trx_ecommerce_entry_mode_from_swiftpass_BDOPay_trx(p_auth_tx)) &&
	   (true == ncmcrd_check_if_trx_is_bdo_pos_acq_trx(p_auth_tx))&&
	   (true == ncmcrd_check_if_trx_ecommerce_POS_entry_mode_from_swiftpass_BDOPay_trx(p_auth_tx)))
	{
		return (true);
	}
	else
	{
		return (false);
	}
}
/********************************************************/

