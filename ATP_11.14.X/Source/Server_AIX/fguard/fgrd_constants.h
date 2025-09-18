/***************************************************************************
 *
 * Copyright (c) 2006, Hypercom, Inc.
 * All Rights Reserved.
 *
 * MODULE:      FGRD_CONSTANTS.H
 *  
 * TITLE:       Equitable Fraudguard Macros and Constants
 *  
 * DESCRIPTION: This header file contains a list of constants that are
 *              commonly used by the Fraudguard interface.
 *
 * APPLICATION: Advanced Transaction Processor (ATP)
 *
 * AUTHOR:      D. Irby
 *
 * REVISION HISTORY:
 *
 * $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\fguard\fgrd_constants.h  $
   
      Rev 1.2   Oct 27 2006 13:42:40   DIrby
   Made the following bug fixes for Issuer txns:
   1.  Issuer labels to be lower case
   2.  Card Currency field needs to be always present
   3.  For local txns, never convert f4, f6, f49, and f51.
   4.  F43 should always be present; f43c defaults to PHL for local txns.
   5.  If f19 not present, populate it from field 49.
   6.  Field 25 should always be present. Use default values is missing.
   7.  If fields 5/6 and 50/51 are missing, populate using field 4 and 49.
   SCR 23522
   
      Rev 1.1   Oct 10 2006 13:48:44   DIrby
   Added capability to send Issuer (ATP approves the txns) to Fraudguard.
   SCR 21851
   
      Rev 1.0   Jul 08 2005 09:15:46   dirby
   Initial revision.
 *
 ****************************************************************************/

#ifndef FGRD_CONSTANTS_H
#define FGRD_CONSTANTS_H

/* Transaction Flow */
#define   SWITCHED_IN    0
#define   SWITCHED_OUT   1

#define   FG_REQUEST   0
#define   FG_RESPONSE  1

/* TRANSACTION SOURCE */
#define   HOST  1
#define   ATP   2

/* Structure Sizes */
#define   AUTH_TX_SIZE    sizeof(AUTH_TX)
#define   CRF01_SIZE      sizeof(CRF01)
#define   FGUARD_SIZE     sizeof(FGUARD)
#define   FGRAW_SIZE      sizeof(FG_RAW)
#define   FG01_SIZE       sizeof(FG01)

/* Transaction Tracing Flags */
#define  TRACE_OFF      0
#define  TRACE_ON       1

/* Shared Memory Tables */
#define  CRF01_TABLE    "Crf01Table"

/* Country Codes */
#define   USD    "840"
#define   PESO   "608"

/* Currency Conversion Direction */
#define   TO_USD    0
#define   FROM_USD  1


/* Miscellaneous */
#define  COMM_BUF_SZE       10000
#define  MAX_APPL_MSG_SIZE  20000

/* Field Conditions */
#define  TAG_OPT  0
#define  TAG_MAN  1


/* Fraudguard Tags */
#define  FG_RESULT_GROUP_TAG     "<CTL_RBS_SEND_RESULT_GROUP>"
#define  FG_RESULT_TAG           "<CTL_RBS_SEND_RESULT>"
#define  FG_RESULT_END_TAG       "</CTL_RBS_SEND_RESULT>"

#define  FG_REQUEST_MSG_TAG      "<CTL_RBS_MESSAGE>"
#define  FG_REQUEST_MSG_END_TAG  "</CTL_RBS_MESSAGE>"

#define  FG_MESSAGE_GROUP        "<CTL_RBS_MESSAGE_GROUP>"
#define  FG_MESSAGE_GROUP_END    "</CTL_RBS_MESSAGE_GROUP>"

#define  FG_TAG_TIMESTAMP  "authtimestamp>"
#define  FG_TAG_I000       "I000>"
#define  FG_TAG_I002       "I002>"
#define  FG_TAG_I003       "I003>"
#define  FG_TAG_I004       "I004>"
#define  FG_TAG_I006       "I006>"
#define  FG_TAG_I007       "I007>"
#define  FG_TAG_I011       "I011>"
#define  FG_TAG_I018       "I018>"
#define  FG_TAG_I019       "I019>"
#define  FG_TAG_I022       "I022>"
#define  FG_TAG_I025       "I025>"
#define  FG_TAG_I032       "I032>"
#define  FG_TAG_I038       "I038>"
#define  FG_TAG_I039       "I039>"
#define  FG_TAG_I041       "I041>"
#define  FG_TAG_I042       "I042>"
#define  FG_TAG_I043a      "I043a>"
#define  FG_TAG_I043b      "I043b>"
#define  FG_TAG_I043c      "I043c>"
#define  FG_TAG_I049       "I049>"
#define  FG_TAG_I051       "I051>"
#define  FG_TAG_I060       "I060>"

#define  FGi_TAG_I000      "i000>"
#define  FGi_TAG_I002      "i002>"
#define  FGi_TAG_I003      "i003>"
#define  FGi_TAG_I004      "i004>"
#define  FGi_TAG_I006      "i006>"
#define  FGi_TAG_I007      "i007>"
#define  FGi_TAG_I011      "i011>"
#define  FGi_TAG_I018      "i018>"
#define  FGi_TAG_I019      "i019>"
#define  FGi_TAG_I022      "i022>"
#define  FGi_TAG_I025      "i025>"
#define  FGi_TAG_I032      "i032>"
#define  FGi_TAG_I038      "i038>"
#define  FGi_TAG_I039      "i039>"
#define  FGi_TAG_I041      "i041>"
#define  FGi_TAG_I042      "i042>"
#define  FGi_TAG_I043a     "i043a>"
#define  FGi_TAG_I043b     "i043b>"
#define  FGi_TAG_I043c     "i043c>"
#define  FGi_TAG_I049      "i049>"
#define  FGi_TAG_I051      "i051>"
#define  FGi_TAG_I060      "i060>"


#define  FG_TAG_RESULT_CODE    "CTL_RBS_RESULT>"
#define  FG_TAG_APPLICATION    "CTL_RBS_APPLICATION>"
#define  FG_TAG_RULE_DOMAIN    "CTL_RBS_RULEDOMAIN>"
#define  FG_TAG_CALLER_ID      "CTL_RBS_CALLERID>"
#define  FG_TAG_CALLER_MSG_ID  "CTL_RBS_CALLERMSGID>"
#define  FG_TAG_RESULT_TEXT    "CTL_RBS_RESULT_TEXT>"
#define  FG_TAG_PRODUCT        "CTL_RBS_PRODUCT>"
#define  FG_TAG_SOURCE         "CTL_RBS_SOURCE>"
#define  FG_TAG_USERNAME       "CTL_RBS_USERNAME>"
#define  FG_TAG_PRIORITY       "CTL_RBS_PRIORITY>"

#define  FG_TAG_AMT_CENTER     "amt_center>"
#define  FG_TAG_AMT_CARD       "amt_card>"
#define  FG_TAG_OTB_AMT_CARD   "otb_amt_card>"

typedef struct
{
   BYTE  f4          [13];
   BYTE  f6          [13];
   BYTE  amt_center  [13];
   BYTE  amt_card    [13];
   BYTE  otb_amt_card[13];
   BYTE  f49         [4];
   BYTE  f51         [4];
   BYTE  country_code[4];
   BYTE  f25         [3];

} ISS_AMTS, *pISS_AMTS;


/* Card Families */
#define  CARD_FAMILY_AMEX  1
#define  CARD_FAMILY_JCB   2
#define  CARD_FAMILY_MC    3
#define  CARD_FAMILY_VISA  4
#define  CARD_FAMILY_UNDEFINED  -1


#ifdef _DEBUG
   #define PRINT( x ) printf( x )
#else
   #define PRINT( x )
#endif

#ifdef _DEBUG
   #define PRINT2( x, y ) printf( x, y )
#else
   #define PRINT2( x, y )
#endif

#endif
