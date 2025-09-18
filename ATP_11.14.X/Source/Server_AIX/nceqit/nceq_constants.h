/***************************************************************************
 *
 * Copyright (c) 2004, Hypercom, Inc.
 * All Rights Reserved.
 *
 * MODULE:      NCEQ_CONSTANTS.H
 *  
 * TITLE:       Equitable Network Control Macros and Constants
 *  
 * DESCRIPTION: This header file contains a list of constants that are
 *              commonly used by the network controller.
 *
 * APPLICATION: Advanced Transaction Processor (ATP)
 *
 * AUTHOR:      D. Irby
 *
 * REVISION HISTORY:
 *
 * $Log:   N:\PVCS\PTE\Equitable\nceqit_ncmlnk_iso_format\nceq_constants.h  $
   
    
		Rev 1.17   Jul 14 2004 10:15:06   dirby
   Added a second Central Bank code for Equitable field 43.
   SCR 1425
   
      Rev 1.16   Jul 12 2004 13:31:48   dirby
   Added definition for EB Central Bank Code.  This is the merchant
   ID for Equitable ATM's.
   SCR 1425
   
      Rev 1.15   Mar 24 2004 15:06:12   dirby
   Added field length definitions for field 36, 102, and 103 for Megalink.
   SCR 1352
   
      Rev 1.14   Mar 01 2004 08:26:24   dirby
   Added definitions to differentiate between nceqit and ncmlnk.
   SCR 1352
   
      Rev 1.13   Sep 29 2003 13:51:40   dirby
   Added support for Fee Collection and Funds Disbursement for
   Acquirers.
   SCR 1083
   
      Rev 1.12   Aug 06 2003 10:38:16   svelaga
   Version 4.2.0.9:  Support for more POSCC added.
   
      Rev 1.11   Jul 25 2003 17:32:54   svelaga
   SMS projects changes -
   1. Support for BACKOFFICE_ADJUSTMENT, REPRESENTMENT transactions.
   2. Added to parse and build some extra fields like DE48, DE63,
   DE18,DE19 etc...
   
      Rev 1.10   Apr 23 2003 15:36:24   dirby
   Added definitions for processing code account types.  These
   indicate if a transaction is from savings, checking, or credit.
   SCR 1101
   
      Rev 1.9   Jun 17 2002 16:10:24   dirby
   Increased size of message types.
   SCR 789
   
      Rev 1.8   Jun 17 2002 13:33:10   dirby
   Added definitions for message sizes so they can be validated
   prior to building a PTE message.
   SCR 789
   
      Rev 1.7   Apr 19 2002 11:05:26   dirby
   Changed response code from "86" to "55" for a security failure.
   This is because Visa rejects "86", and also because "55" is
   more standard.
   SCR 707
   
      Rev 1.6   Jan 16 2002 12:31:02   dirby
   Added transaction tracing capability.     SCR 546
   
      Rev 1.5   Jan 14 2002 18:14:10   dirby
   Modified the transaction statistic functions to keep statistics
   on number of approved, declined, timed out, and reversed
   txns.  These stats are displayed on Monitor.
   SCR 484
   
      Rev 1.4   Dec 10 2001 14:02:32   dirby
   Removed definition of host states.  These are now in app_info.h.     SCR 488
   
      Rev 1.3   Oct 10 2001 17:21:06   dirby
   Added response code EQIT_TIMEOUT.  This is the value EB
   wants for reversals of timed out txns.  SCR 510
   
      Rev 1.2   Sep 27 2001 14:08:42   dirby
   Added definitions for track1 and track 2 field separators.  SCR 446
   
      Rev 1.1   Aug 17 2001 12:51:40   dirby
   Added KEY_CHANGE_ENABLED flag, used to only issue key
   change if pad character in ncf01_i is set to this flag.
   Reference SCR 425.
   
      Rev 1.0   Aug 06 2001 09:48:04   dirby
   Initial revision.
 *
 ****************************************************************************/

#ifndef NCEQ_CONSTANTS_H
#define NCEQ_CONSTANTS_H

/* Network Types */
#define   ISSUER   1
#define   ACQUIRER 2
#define   BOTH     3

/* Networks */
#define   NCEQIT   0
#define   NCMLNK   1

/* TRANSACTION SOURCE */
#define   HOST  1
#define   ATP   2

/* ASCII and EBCDIC conversion */
#define   A2E    "A2E"    /*  ASCII to EBCDIC conversion */
#define   E2A    "E2A"    /*  EBCDIC to ASCII conversion */

/* Error Codes */
#define   TOO_MANY_ACTIVE_TRANSACTIONS   "96"
#define   TOO_MANY_TIMEOUTS              "96"
#define   HOST_UNAVAILABLE               "91"
#define   ISSUER_INOPERATIVE             "96"
#define   SYSTEM_ERROR                   "96"
#define   TO                             "19"
#define   VISA_TO                        "91"
#define   EQIT_TIMEOUT                   "68"
#define   INVALID_TXN                    "58"
#define   SECURITY_FAILURE               "55"
#define   APPROVED                       "00"
#define   INVALID_FIELD					 "30"

/* Result codes of the security module */
#define   SECURITY_FAILED    0
#define   SECURITY_SUCCESS   1

/* Result codes of the currency conversion module */
#define   CONVERSION_FAILED    0
#define   CONVERSION_SUCCESS   1

/* Country Codes */
#define   USA_CURRENCY_CODE    "840"
#define   PESO_CURRENCY_CODE   "608"

/* Industry Codes */
#define   ATM_INDUSTRY_CODE    "901"

/* Condition Codes */
#define   sATM_POSCC    "02"
#define   ATM_POSCC       2
#define   POS_POSCC      51
#define   sPOS_POSCC    "51"
#define   NORMAL_POSCC    0
#define   sNORMAL_POSCC "00"
#define   ADJ_POSCC1     13
#define   ADJ_POSCC2     17
#define   ADJ_POSCC3     54
#define   sADJ_POSCC1   "13"
#define   sADJ_POSCC2   "17"
#define   sADJ_POSCC3   "54"
#define   sRPS_POSCC	"09"


/* Acquiring Institution ID Codes */
#define   LOCAL_POS      "00"
#define   INTL_ATM_POS   "95"

/* Transaction Origins */
#define   LOCAL_ORIGIN   "LO"
#define   REMOTE_ORIGIN  "RO"

/* Transaction Flow */
#define   SWITCHED_IN    0
#define   SWITCHED_OUT   1

/* 0800 Request Types */
#define   LOGON         0
#define   LOGOFF        1
#define   ECHO          2
#define   KEY_EXCHANGE  3

/* 0800 NMI Codes for DE70 */
#define   NMI_LOGON     "001"             
#define   NMI_LOGOFF    "002"
#define   NMI_ECHO      "301"
#define   NMI_KEYCHANGE "101"
#define   NMI_CUTOFF    "201"
#define   NMI_SAF       "360"

/* Echo Timer Set/Reset Values */
#define   ECHO_TIMER_IS_SET      1
#define   ECHO_TIMER_IS_CLEARED  0

/* Key Change Flags */
#define   KEY_CHANGE_ENABLED    'E'

/* Structure Sizes */
#define   AUTH_TX_SIZE    sizeof(AUTH_TX)
#define   NCF01_SIZE      sizeof(NCF01)
#define   NCF30_SIZE      sizeof(NCF30)
#define   ATM01_SIZE      sizeof(ATM01)
#define   TLF01_SIZE      sizeof(TLF01)
#define   CRF01_SIZE      sizeof(CRF01)

/* Message Sizes */
#define   MAX_HOST_MSG_SIZE    750
#define   MAX_APPL_MSG_SIZE  20000

/* Usages for field 48 */
#define  USAGE_NONE   0
#define  USAGE_5      5

/* Cash Advance Types for field 102. */
#define  ECN_VISA        'A'
#define  OFF_US_VISA     'B'  /* Credit */
#define  OFF_US_MC       'C'
#define  ECN_MC          'D'
#define  CIRRUS          'E'
#define  OFF_US_PLUS_CA  'F'  /* Current/Check */
#define  OFF_US_PLUS_SA  'G'  /* Savings       */
#define  ECN_JCB         'H'
#define  OFF_US_JCB      'I'
#define  ECN_AMEX        'J'
#define  OFF_US_AMEX     'K'
#define  OFF_US_DCI      'L'
#define  ON_US_DCI       'M'
#define  OFF_US_PUL      'N'
#define  ECN_CUP		 'O'
#define  OFF_US_CUP      'P'

/* Processing Code Account Types (From) */
#define  PROC_CODE_SAVINGS  '1'
#define  PROC_CODE_CURRENT  '2'
#define  PROC_CODE_CREDIT   '3'
#define  PROC_CODE_UNKNOWN  '0'

/* Transaction Tracing Flags */
#define  TRACE_OFF      0
#define  TRACE_ON       1

/* TIMER TYPES */
#define  REQUEST_TIMER  0
#define  ECHO_TIMER     1

/* TIMER APPS */
#define  TIMER_REVERSAL   "Reversal"
#define  TIMER_FINANCIAL  "Financial"
#define  TIMER_0800       "0800"
#define  TIMER_ECHO       "Echo"

/* Shared Memory Tables */
#define  CRF01_TABLE    "Crf01Table"

/* Flags used when sending responses to host */
#define  SEND_UNCONDITIONALLY  0
#define  SEND_IF_NOT_REVERSAL  1

/* Track Data Field Separators */
#define  TRK1_FS     '^'
#define  TRK2_FS     '='

/* For Field 43 Acceptor Terminal Name */
#define  EB_CENTRAL_BANK_CODE1  "13"
#define  EB_CENTRAL_BANK_CODE2  "71"

/* Miscellaneous */
#define  COMM_BUF_SZE          1500
#define  EXTENDED_BITFIELD      128
#define  NORMAL_BITFIELD         64
#define  ATM_POS_ENTRY_MODE    "021"
#define  LOOKUP_KEY_SIZE         22

/* Debug Macros */
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

/* Bit Field Sizes */
#define DE2_LEN_SIZE   2
#define MAX_DE2_SIZE  19
#define DE3_SIZE       6
#define DE4_SIZE      12
#define DE5_SIZE      12
#define DE7_SIZE      10
#define DE9_SIZE       8
#define DE11_SIZE      6
#define DE12_SIZE      6
#define DE13_SIZE      4
#define DE14_SIZE      4
#define DE15_SIZE      4
#define DE16_SIZE	     4
#define DE18_SIZE	     4
#define DE19_SIZE	     3
#define DE20_SIZE      3
#define DE21_SIZE	     3
#define DE22_SIZE      3
#define DE25_SIZE      2
#define DE28_SIZE	     8
#define DE32_SIZE      2
#define DE33_SIZE      2
#define DE35_LEN_SIZE  2
#define DE36_SIZE	   104
#define DE37_SIZE	    12
#define MAX_DE35_SIZE 37
#define DE38_SIZE      6
#define DE39_SIZE      2
#define DE41_SIZE      8
#define DE42_SIZE     15
#define DE43_SIZE     40
#define DE48_SIZE     38
#define DE49_SIZE      3
#define DE52_SIZE     16
#define DE70_SIZE      3
#define DE95_SIZE     42
#define DE102_SIZE    28
#define DE102_MEGALINK_SIZE    13
#define DE103_MEGALINK_SIZE    13
#define DE103_SIZE    28
#define DE126_SIZE	 128
#define DE128_SIZE    16
#define MAX_DE44_RESPONSE_LEN 25

#endif
