/******************************************************************************

   Module: app_info.h
  
   Title: Application Specific Type Definitions
  
   Description: 

   Application: BP

   Authours: Last modified by supriya.
   
   Revision History:

   $Log:   N:\PVCS\PTE\bea\headers\APP_INFO.H  $
   
      Rev 1.5   Oct 29 1999 10:12:56   iarustam
   added card_atm
   
      Rev 1.4   Oct 22 1999 17:18:12   iarustam
    
   
      Rev 1.3   Oct 18 1999 15:08:38   iarustam
   added new ST2_DB_DELETE_BPF01_BY_PROFILE_ID
   for GUI
   
      Rev 1.2   Oct 01 1999 14:35:52   iarustam
   changes for repds
   
      Rev 1.1   Sep 23 1999 12:36:06   iarustam
   added st2 for repds
   
      Rev 1.0   08 Sep 1999 10:53:56   epriyev
   initial BEA load
   
      Rev 1.30   18 Aug 1999 09:49:58   dirby
   Added some subtype2's for selecting transaction records from
   the database.
   
      Rev 1.29   17 Aug 1999 10:18:42   dirby
   Added SAF01_DATA.
   
      Rev 1.28   10 Aug 1999 11:29:36   epriyev
   added definition of  ST2_DB_SELECT_BCH10_BY_STAN
   
      Rev 1.27   10 Aug 1999 10:47:34   epriyev
   added definition of BCH20_DATA 
   
      Rev 1.26   20 Jul 1999 18:02:36   epriyev
   defined AUTH_OFFLINE_VOID_SALE
   and AUTH_OFFLINE_VOID_REFUND
   
      Rev 1.25   May 27 1999 11:49:04   npogosya
   Added subtype1 ST1_SAF_REQUEST
   
      Rev 1.24   Apr 08 1999 12:47:46   sdasgupt
   Changed #define
   
      Rev 1.22   Apr 08 1999 10:55:06   iarustam
   Added new tx_keys for override transaction
   
      Rev 1.21   Apr 01 1999 11:04:36   iarustam
   Added select_tlf01_by_auth_number for
   Voice Auth Reversal transaction
   
      Rev 1.20   25 Mar 1999 17:57:50   epriyev
   defined VISA1_SUPPORT 
   for routing visa1 messages to
   visa1 terminal handler's que.
   
      Rev 1.19   24 Mar 1999 16:21:52   epriyev
   set ISO_MSG_LENGTH to zero.
   
      Rev 1.18   Mar 24 1999 16:19:20   iarustam
   added new BCH01_BCH10_TLF01_DATA
   
      Rev 1.17   Mar 22 1999 14:35:50   iarustam
   Added tlf01_gui_list
   
      Rev 1.16   Mar 20 1999 08:55:58   iarustam
   Changes for GUI lists
   
      Rev 1.15   Mar 15 1999 16:29:54   sdasgupt
   added 3 new types for the general status field
   in the TLF01 table.
   
   
   
      Rev 1.14   26 Feb 1999 15:13:58   dirby
   Added subtype2 -> ST2_DB_SELECT_TLF01_BY_TERMINAL_ID.
   
      Rev 1.13   Feb 24 1999 17:15:56   iarustam
   Added MCC102_DATA,
             MCC103_DATA, and 
             AUTH_TX_DATA
   
   
      Rev 1.12   Feb 19 1999 16:42:00   IGOLDFIE
   Added ST2_DB_SELECT_TLF01_FOR_SALES_COMPLETION
   
      Rev 1.11   Feb 16 1999 16:58:38   sdasgupt
   Added message sub types
   
      Rev 1.10   11 Feb 1999 09:37:32   dirby
   Added message type MT_INCOMING_GUI.  It gets
   used when the GUI sends a message to a network
   controller via applink.
   
      Rev 1.9   Feb 09 1999 17:16:32   IGOLDFIE
   Added type TLF01_SAF_DATA
   
      Rev 1.8   Feb 08 1999 17:55:54   sdasgupt
   Added Voice Auth Transaction types
   
      Rev 1.7   Feb 08 1999 15:56:26   sdasgupt
   Added general status Constants
   
      Rev 1.6   08 Feb 1999 15:02:08   dirby
   Added tx_keys 'AUTH_ATM_CONFIRM' through 'AUTH_REVERSAL_REPEAT'.
   
      Rev 1.5   Feb 04 1999 17:57:08   sdasgupt
   TxCntl changes
   
      Rev 1.4   04 Feb 1999 16:49:44   jhapitas
   Added 4 new data defintions for the
   Network group.
   
      Rev 1.3   Feb 03 1999 14:43:14   sdasgupt
   Added #defines for pin verification methods
   
   
      Rev 1.2   Feb 03 1999 11:03:48   sdasgupt
   Added txcntl related stuff
   
      Rev 1.1   Feb 02 1999 18:34:50   sdasgupt
   TxCntl changes
   
      Rev 1.0   02 Feb 1999 16:59:56   jhapitas
   Initial Revision
 *****************************************************************************/

#ifndef APP_INFO_H
#define APP_INFO_H

#include "basictyp.h"
#include "pte.h"
#include "ptemsg.h"

/* Used for routing visa1 messages to visa1 terminal handler*/
#define  VISA1_SUPPORT

  
/* Used for PIN Block Format  */
#define  ANSI_X98             '1'
#define  IBM_3624             '2'
#define  PIN_PAD_CHAR         '3' /* not an error */
#define  DOCUTEL              '3' /* not an error */
#define  IBM_PIN_PAD          '4'
#define  BURROUGHS            '5'
#define  IBM_4731             '9'


/* Used for encryption method */
#define  DES            1
#define  DUKPT          2


/* Used for pad character */
#define  UNSPECIFIED             'X'
#define  DISABLE_SANITY_CHECK    'W'


/* Used for remote host encryption hardware */
#define  ATALLA            1
#define  OTHER             2


/* Used for auth_tx entry_type field */
#define FINANCIAL             1
#define ADMINISTRATIVE        2
#define TERMINAL_STATUS       3


/* Used for terminal status field */
#define GS_STATUS_IDLE              1
#define GS_STATUS_WAIT_RSP          2
#define GS_STATUS_WAIT_PREREV       3
#define GS_STATUS_WAIT_POSTADV      4
#define GS_STATUS_WAIT_PREADV       5
#define GS_STATUS_WAIT_UPLOAD       6


/* Used for general status field */ 

#define GS_TXN_NEW                  1
#define GS_TXN_PERF_REQ             2
#define GS_TXN_PERF_PREREV          3
#define GS_TXN_PERF_PREADV          4
#define GS_TXN_PERF_POSTADV         5
#define GS_TXN_PERF_UPLOAD          6
#define GS_TXN_COMPLETED            7
#define GS_TXN_TIMEDOUT             8
#define GS_TXN_COMPLETED_OFF        9
#define GS_TXN_DROPPED              10

#define GS_TXN_BIN_NOT_FOUND                               18 
#define GS_TXN_MERCHANT_NOT_FOUND                          19 
#define GS_TXN_MERCHANT_BIN_NOT_FOUND                      20
#define GS_TXN_SYSTEM_ERROR                                21
#define GS_TXN_CAF_NOT_FOUND                               22
#define GS_TXN_INVALID_CAF_STATUS                          23
#define GS_TXN_INVALID_MERCHANT_STATUS                     24
#define GS_TXN_INVALID_MERCHANT_CURRENCY_CODE              25
#define GS_TXN_INVALID_CAF                                 26
#define GS_TXN_CCF02P_NOT_FOUND                            27
#define GS_TXN_INVALID_CCF02P_STATUS                       28
#define GS_TXN_INVALID_CCF02P_CURRENCY_CODE                29
#define GS_TXN_INVALID_PAF01_STATUS                        30
#define GS_TXN_ACF01_NOT_FOUND                             31
#define GS_TXN_INVALID_ACF01_STATUS                        32
#define GS_TXN_CRF_NOT_FOUND                               33
#define GS_TXN_INVALID_CCF03_STATUS                        34
#define GS_TXN_INVALID_CCF02_RELATED_ACCT                  35
#define GS_TXN_INVALID_TRANSACTION_AMOUNT                  36
#define GS_TXN_INVALID_CCF02P_EXPIRY_DATE                  37
#define GS_TXN_INVALID_TRACK_EXPIRY_DATE                   38
#define GS_TXN_INVALID_EXPIRY_DATE                         39
#define GS_TXN_INVALID_EXPIRY_DATE_LOCATION                40
#define GS_TXN_TRANSACTION_NOT_ALLOWED                     41
#define GS_TXN_EXCEEDED_MAX_TRANSACTION_LIMIT_FOR_MERCHANT 42
#define GS_TXN_VELOCITY_NBR_USAGE_CHECK_FAILED             43
#define GS_TXN_VELOCITY_AMT_USAGE_CHECK_FAILED             44
#define GS_TXN_TYPE_OF_VEHICLE_CHECK_FAILED                45
#define GS_TXN_MERCHANT_CARD_USAGE_CHECK_FAILED            46
#define GS_TXN_CATEGORY_USAGE_CHECK_FAILED                 47
#define GS_TXN_INVALID_CCF02P_CREDIT_AUTH                  48
#define GS_TXN_CARD_LEVEL_CREDIT_LINE_LIMIT_FAILED         49
#define GS_TXN_INVALID_ACF01_AVAILABLE_FAILED              50
#define GS_TXN_ACF01_BALANCE_ACCOUNT_LIMIT_FAILED          51
#define GS_TXN_INVALID_PIN_INDICATOR_ON_TRACK              52
#define GS_TXN_INVALID_FAILED_PIN_LIMIT_EXCEEDED           53
#define GS_TXN_INVALID_PIN                                 54

#define GS_TXN_INVALID_TRACK2_SERVICE_CODE                 55
#define GS_TXN_INVALID_TRACK2_TERMINAL_TYPE                56
#define GS_TXN_INVALID_TRACK2_DEVICE_TYPE                  57
#define GS_TXN_INVALID_TRACK2_INTERCHANGE_DESIGNATOR       58
#define GS_TXN_TIME_RESTRICTION_CHECK_FAILED               59
#define GS_TXN_SWITCHED_TO_NETWORK                         60
#define GS_TXN_INVALID_AUTHORIZATION_METHOD                61
#define GS_TXN_INVALID_ACF01_ACCOUNT                       62
#define GS_TXN_OFFLINE_SALE_COMPLETED                      63
#define GS_TXN_EXCEEDED_MAX_REFUND_LIMIT_FOR_MERCHANT      64
#define GS_TXN_BCH10_NOT_FOUND                             65
#define GS_TXN_NOT_VERIFY                                  66
#define GS_TXN_VERIFY                                      67
#define GS_TXN_GOOD_BATCH                                  68
#define GS_TXN_DUPLICATE_BATCH                             69
#define GS_TXN_REJECTED_BATCH                              70
#define GS_TXN_INVALID_DESTINATION                         71
#define GS_TXN_HOST_NOT_AVAILABLE                          72
#define GS_TXN_INVALID_PRODUCT_CODE                        73
#define GS_TXN_BONUS_NOT_FOUND                             74
#define GS_TXN_UAG_NOT_FOUND                               75
#define GS_TXN_PLUS_NOT_FOUND		                   76
#define GS_TXN_PURCHASE_CARD_CHECK_FAILED		   77
#define GS_INVALID_AUTH_INDICATOR	                   78
#define GS_TXN_INVALID_CURRENCY_CODE		           79								
#define GS_TXN_INVALID_HANDLER_QUEUE	                   80
#define GS_TXN_INVALID_ORIGIN		                   81
#define GS_TXN_INVALID_DEFERRED_TERM_LENGTH		   82
#define GS_TXN_PAF_NOT_FOUND			           83 
#define GS_TXN_DEF01_NOT_FOUND				   84

#define GS_MAGSTRIPE_FAILED				   85
#define GS_MAGSTRIPE_FAILED_REPORTED			   86
#define GS_MAGSTRIPE_FAILED_REPORTED_APPROVED              87


/* Used for network security processor */
#define GS_NSP_ALLOK                0
#define GS_NSP_KEYCHANGE_FAILED     11 
#define GS_NSP_VERIFYPIN_FAILED     12     
#define GS_NSP_CVVCVC_FAILED        13        
#define GS_NSP_SANITYCHECK_FAILED   14  
#define GS_NSP_PINCHANGE_FAILED		15 
#define GS_NSP_TIMEOUT				   16
#define GS_NSP_OUTOFSERVICE			17

/* Used for reversal status field */
#define RV_TXN_REV_NEEDED           1
#define RV_TXN_REV_IF_FAILS         2
#define RV_TXN_WAIT_PREREV          3
#define RV_TXN_WAIT_POSTADV         4
#define RV_TXN_WAIT_PREADV          5
#define RV_TXN_ADV_NEEDED           6

/* Error codes for message_type field in opr_message structure */
#define INFO_ERROR                  "INFO"
#define WARNING_ERROR               "WARNING"
#define FATAL_ERROR                 "FATAL"

/* Used for reversal status field */
#define RESP_APPROVED               1
#define RESP_TIMEOUT                2
#define RESP_NOT_AVAILABLE          3
#define RESP_CALL_HELP              4
#define RESP_DECLINE                5

#define CARD_AMEX			"00"  // Added by SDG on Feb 3 '99 for Equitable
#define CARD_VISA			"01"
#define CARD_MASTERCARD	"02"
#define CARD_JCB			"03"
#define CARD_ATM			"04"

#define IDENTIKEY			"1"
#define EIDENTIKEY		"2"
#define IBM3624			"3"
#define IBMOFFSET			"4"
#define VISA				"5"
#define DESBILEVEL		"6"
#define DEIBOLD			"7"
#define NCR					"8"
#define CVV					"9"
#define PINBLOCK			"10"






/* used for message requests... tx_key */
 /* .... stay between 1 and 39 .... */
                                          //    msg_type    procode[0]
  #define AUTH_SALE                   1   //    0200        00
  #define AUTH_REFUND                 2   //    0200        20
  #define AUTH_CASH_ADVANCE           3   //    0200        01
  #define AUTH_PAYMENT                4   //    0200        40 
  #define AUTH_OFFLINE_SALE           5   //    0220        00 
  #define AUTH_OFFLINE_REFUND         6   //    0220        20 
  #define AUTH_SALE_ADJUSTMENT        7   //    0220        02 
  #define AUTH_REFUND_ADJUSTMENT      8   //    0220        22 
  #define AUTH_PAYMENT_ADJUSTMENT     9   //    0220        42 
  #define AUTH_PRE_AUTHORIZATION      10  //    0100        30 
  #define AUTH_AUTHORIZATION          11  //    0100        00 
  #define AUTH_BALANCE_INQUIRY        12  //    0100        31 
  #define AUTH_VOID_SALE              13  //    0200        02
  #define AUTH_VOID_REFUND            14  //    0200        22 
  #define AUTH_SETTLE                 15  //    0500        92 
  #define AUTH_BATCH_UPLOAD           16  //    0320        xx 
  #define AUTH_SETTLE_TRAILER         17  //    0500        96 
  #define AUTH_STATISTICS             18  //    0800        91 
  #define AUTH_REVERSAL               19  //    0400        xx 
  #define AUTH_LOGON                  20  //    0800        92 
  #define AUTH_TEST                   21  //    0800        99 
  #define AUTH_PIN_CHANGE             22  //    0800        90
  #define AUTH_VOID_CASH_ADVANCE      23  //    0200        20
  #define AUTH_SALE_CASH              24  //    0200        09 
  #define AUTH_CHECK_VERIFICATION     25  //    0100        04 
  #define AUTH_CARD_VERIFICATION      26  //    0100        38 
  #define AUTH_VOID_PRE_AUTHORIZATION 27  //    0100        02
  #define AUTH_SALES_COMPLETION       28  //    0200        20 
  #define AUTH_GUARANTEED_LATE_ARRIVAL 29  //   0220        90 
  #define AUTH_BATCH_DOWN_LINE_LOAD   30  //    0500        95 
  #define AUTH_SIGNATURE_DATA         31  //    0320        90 
  #define AUTH_INITIALIZATION         32  //    0800        93 
  #define AUTH_PLEASE_WAIT_ADVICE     33  //    0830        90
  #define AUTH_NEGATIVE_DOWNLOAD      34  //    0200        20 
  #define AUTH_ADMIN_ADVICE           35
  #define AUTH_VOID_PAYMENT           36
  #define AUTH_REGISTRATION           37
  #define AUTH_CHECK_DL               38
  #define AUTH_BRANCH_PMT             39
  #define AUTH_ATM_CONFIRM            40
  #define AUTH_QUASI_CASH             41
  #define AUTH_ADVICE                 42
  #define AUTH_REVERSAL_ADVICE        43
  #define AUTH_CARD_UPDATE            44
  #define AUTH_RECONCILE_ADVICE       45
  #define AUTH_NETWORK_ADVICE         46
  #define AUTH_ADVICE_REPEAT          47
  #define AUTH_REVERSAL_REPEAT        48

  #define AUTH_CASH_ADVANCE_AUTHORIZATION				49 // Added by SDG for Voice Authorizations
  #define AUTH_DEFERRED_PURCHASE_AUTHORIZATION		    50 // Added by SDG for Voice Authorizations	
  #define AUTH_DEFERRED_PURCHASE_AUTHORIZATION_OVERRIDE	51 // Added by IA for Voice Authorizations	04-08-99
  #define AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE		52 // Added by IA for Voice Authorizations	04-08-99
  #define AUTH_AUTHORIZATION_OVERRIDE		            53 // Added by IA for Voice Authorizations	04-08-99
 
  #define AUTH_OFFLINE_VOID_SALE		            54 // Added by EP 07-20-99
  #define AUTH_OFFLINE_VOID_REFUND		            55 // Added by EP 07-20-99

  #define AUTH_INVALID                99

/* used for message responses... tx_key */
#define AUTH_SALE_RESPONSE                100 + AUTH_SALE               
#define AUTH_REFUND_RESPONSE              AUTH_REFUND             + 100
#define AUTH_CASH_ADVANCE_RESPONSE        AUTH_CASH_ADVANCE       + 100
#define AUTH_PAYMENT_RESPONSE             AUTH_PAYMENT            + 100
#define AUTH_OFFLINE_SALE_RESPONSE        AUTH_OFFLINE_SALE       + 100
#define AUTH_OFFLINE_REFUND_RESPONSE      AUTH_OFFLINE_REFUND     + 100      
#define AUTH_SALE_ADJUSTMENT_RESPONSE     AUTH_SALE_ADJUSTMENT    + 100   
#define AUTH_REFUND_ADJUSTMENT_RESPONSE   AUTH_REFUND_ADJUSTMENT  + 100
#define AUTH_PAYMENT_ADJUSTMENT_RESPONSE  AUTH_PAYMENT_ADJUSTMENT + 100
#define AUTH_PRE_AUTHORIZATION_RESPONSE   AUTH_PRE_AUTHORIZATION  + 100
#define AUTH_AUTHORIZATION_RESPONSE       AUTH_AUTHORIZATION      + 100
#define AUTH_BALANCE_INQUIRY_RESPONSE     AUTH_BALANCE_INQUIRY    + 100
#define AUTH_VOID_SALE_RESPONSE           AUTH_VOID_SALE          + 100
#define AUTH_VOID_REFUND_RESPONSE         AUTH_VOID_REFUND        + 100
#define AUTH_SETTLE_RESPONSE              AUTH_SETTLE             + 100
#define AUTH_BATCH_UPLOAD_RESPONSE        AUTH_BATCH_UPLOAD       + 100
#define AUTH_SETTLE_TRAILER_RESPONSE      AUTH_SETTLE_TRAILER     + 100
#define AUTH_STATISTICS_RESPONSE          AUTH_STATISTICS         + 100
#define AUTH_REVERSAL_RESPONSE            AUTH_REVERSAL           + 100
#define AUTH_LOGON_RESPONSE               AUTH_LOGON              + 100
#define AUTH_TEST_RESPONSE                AUTH_TEST               + 100
#define AUTH_PIN_CHANGE_RESPONSE          AUTH_PIN_CHANGE         + 100
#define AUTH_NEGATIVE_DOWNLOAD_RESPONSE   AUTH_NEGATIVE_DOWNLOAD  + 100
#define AUTH_SALE_CASH_RESPONSE           AUTH_SALE_CASH          + 100
#define AUTH_CHECK_VERIFICATION_RESPONSE           AUTH_CHECK_VERIFICATION + 100
#define AUTH_CARD_VERIFICATION_RESPONSE            AUTH_CARD_VERIFICATION  + 100
#define AUTH_VOID_PRE_AUTHORIZATION_RESPONSE       AUTH_VOID_PRE_AUTHORIZATION   + 100
#define AUTH_SALES_COMPLETION_RESPONSE             AUTH_SALES_COMPLETION   + 100
#define AUTH_GUAREANTEED_LATE_ARRIVAL_RESPONSE     AUTH_GUAREANTEED_LATE_ARRIVAL + 100
#define AUTH_BATCH_DOWN_LINE_LOAD_RESPONSE         AUTH_BATCH_DOWN_LINE_LOAD + 100
#define AUTH_SIGNATURE_DATA_RESPONSE               AUTH_SIGNATURE_DATA       + 100
#define AUTH_INITIALIZATION_RESPONSE               AUTH_INITIALIZATION       + 100
#define AUTH_PLEASE_WAIT_ADVICE_RESPONSE           AUTH_PLEASE_WAIT_ADVICE   + 100
#define AUTH_VOID_CASH_ADVANCE_RESPONSE            AUTH_VOID_CASH_ADVANCE    + 100
#define AUTH_ADMIN_ADVICE_RESPONSE                 AUTH_ADMIN_ADVICE         + 100
#define AUTH_VOID_PAYMENT_RESPONSE                 AUTH_VOID_PAYMENT         + 100
#define AUTH_REGISTRATION_RESPONSE                 AUTH_REGISTRATION         + 100
#define AUTH_CHECK_DL_RESPONSE                     AUTH_CHECK_DL             + 100
#define AUTH_ATM_CONFIRM_RESPONSE          AUTH_ATM_CONFIRM       + 100
#define AUTH_QUASI_CASH_RESPONSE           AUTH_QUASI_CASH        + 100
#define AUTH_ADVICE_RESPONSE               AUTH_ADVICE            + 100
#define AUTH_REVERSAL_ADVICE_RESPONSE      AUTH_REVERSAL_ADVICE   + 100
#define AUTH_CARD_UPDATE_RESPONSE          AUTH_CARD_UPDATE       + 100
#define AUTH_RECONCILE_ADVICE_RESPONSE     AUTH_RECONCILE_ADVICE  + 100
#define AUTH_NETWORK_ADVICE_RESPONSE       AUTH_NETWORK_ADVICE    + 100
#define AUTH_ADVICE_REPEAT_RESPONSE        AUTH_ADVICE_REPEAT     + 100
#define AUTH_REVERSAL_REPEAT_RESPONSE      AUTH_REVERSAL_REPEAT   + 100
#define AUTH_CASH_ADVANCE_AUTHORIZATION_RESPONSE			AUTH_CASH_ADVANCE_AUTHORIZATION + 100
#define AUTH_DEFERRED_PURCHASE_AUTHORIZATION_RESPONSE		AUTH_DEFERRED_PURCHASE_AUTHORIZATION + 100
#define AUTH_DEFERRED_PURCHASE_AUTHORIZATION_OVERRIDE_RESPONSE	AUTH_DEFERRED_PURCHASE_AUTHORIZATION_OVERRIDE  + 100
#define AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE_RESPONSE		AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE  + 100
#define AUTH_AUTHORIZATION_OVERRIDE_RESPONSE		            AUTH_AUTHORIZATION_OVERRIDE  + 100

/*
 * The following definitions are for use in
 * the 'msg_type' field of the PTE_MSG.
 *
 * NOTE: Pinnalce PTE common message types are 1..99!!!  Stay above that!!!
 *
 */
#define MT_AUTH_REQUEST                100
#define MT_AUTH_RESPONSE               101
#define MT_OPERATOR                    102  /* messages meant for operations monitor */
#define MT_BUILD_FILE                  103
//#define MT_DB_RSP_OK                   104  // GD commented out 7-31
//#define MT_DB_RSP_ERR                  105  //GD commneted out 7-31
#define MT_SEND_LOG_ON                 104 
#define MT_SEND_LOG_OFF                105 
#define MT_SEND_ECHO_TEST              106
#define MT_SHUT_DOWN                   107
#define MT_AUTH_WORKSTATION            108
#define MT_AUTH_WORKSTATION_RESPONSE   109
#define MT_SYS_MONITOR                 110
//#define MT_DB_REPLY                    111
#define MT_RESTART                     111
#define MT_NEGATIVE_FILE               112
#define MT_CLEANUP                     113
#define MT_INCOMING_GUI                114


/* DR moved common system wide defines to PTE.H
#define  MT_DB_REQUEST           100
#define  MT_DB_REPLY             101
#define  MT_STATE_TIMEOUT        102
#define  MT_INCOMING             103
#define  MT_OUTGOING             106
*/

/*
 *  Used for logon/logoff messages 
 */ 
#define  SOURCE_IS_HOST      0
#define  SOURCE_IS_FSVISA    1
#define  SOURCE_IS_STATE     2
#define  SOURCE_IS_OPERATOR  3   

/*
 * The following definitions are for use in
 * the 'msg_type' field of the PTE_MSG.
 */
#define  MT_NSP_KEYCHANGE				   	200 /* CRYPTO MSG TYPES */
#define  MT_NSP_KEYCHANGE_RESPONSE			201
#define  MT_NSP_LOCAL_KEYCHANGE 			   202
#define  MT_NSP_LOCAL_KEYCHANGE_RESPONSE	203
#define  MT_NSP_XLATETOANSI					204
#define  MT_NSP_XLATETOANSI_RESPONSE		205
#define  MT_NSP_VERIFYPIN					   206
#define  MT_NSP_VERIFYPIN_RESPONSE			207
#define  MT_NSP_CVVCVC						   208
#define  MT_NSP_CVVCVC2			    		   209
#define  MT_NSP_CVVCVC_RESPONSE				210
#define  MT_NSP_CHANGEPIN					   211
#define  MT_NSP_CHANGEPIN_RESPONSE			212
#define	MT_NSP_REM_KEYCHANGE				   213
#define  MT_NSP_REM_KEYCHANGE_RESPONSE		214
#define  MT_SCHEDULER_CONTROL             215
#define  MT_NSP_VERIFY_MAGSTRIPE          216
#define	MT_NSP_VERIFY_MAGSTRIPE_RESPONSE 217
#define  MT_NSP_TRANSLATE_PIN             218
#define	MT_NSP_TRANSLATE_PIN_RESPONSE    219

/*
 * The following definitions are for use in
 * the 'msg_subtype1' field of the PTE_MSG.
 *
 * NOTE: Pinnalce PTE common message subtypes are 1..99!!!  Stay above that!!!
 *
 */


#define  ST1_DB_GET_CONTEXT         100   //5
#define  ST1_DB_VERIFY_PAN          101   //6
#define  ST1_AUTH_ERROR             102   //8
#define  ST1_FROM_TERM_HANDLER      103   //10 /* used in conjuntion with msg_type = MT_OPERATOR */
#define  ST1_FROM_HOST_HANDLER      104   //11 /* used in conjuntion with msg_type = MT_OPERATOR */
#define  ST1_DB_CHECK_BIN_RANGE     105   //12 
#define  ST1_DB_GET_SETTLE_TOTALS   106   //13
#define  ST1_DB_MOVE_TRANS          107   //14
#define  ST1_DB_GET_GUI_LIST        108
#define  ST1_DB_GET_COUNT           109
#define  ST1_DB_GET_SERVICE_LIST    110
#define  ST1_DB_GET_SCH_LIST        111
#define  ST1_SCH_STATUS             112
#define  ST1_SCH_CANCELJOB          113
#define  ST1_SCH_ADDJOB             114
#define  ST1_MONITOR_UP             115
#define  ST1_MONITOR_DOWN           116
#define  ST1_ECHOTEST               117
#define  ST1_LOGON                  118
#define  ST1_LOGOFF                 119
#define  ST1_OPRMSG                 120
#define  ST1_PROCESS_COMPLETE       121
#define  ST1_DB_CHECK_DATA          122
#define  ST1_DELETE_BY_DATE         123
#define  ST1_PROCESS_START          124
#define  ST1_NETWORK_QUERY          125
#define  ST1_NETWORK_UPDATE         126
#define  ST1_DB_SELECT_AUTH_ID		127  // Added by SDG
#define  ST1_DB_CHECK_BIN02_RANGE   128  // Added by SDG
#define  ST1_SAF_REQUEST            129  // Added by NarinaP





/* DR moved common system wide defines to PTE.H
#define  ST1_DB_INSERT           1
#define  ST1_DB_DELETE           2
#define  ST1_DB_UPDATE           3
#define  ST1_DB_SELECT           4
#define  ST1_DB_GET_ALL          7
#define  ST1_DB_ECHO_TEST        9
*/


/*
 * The following definitions are for use in
 * the 'msg_subtype2' field of the PTE_MSG.
 *
 * NOTE: Pinnalce PTE common message subtypes are 1..99!!!  Stay above that!!!
 *
 */
#define  ST2_REVERSED_TRAN                      105   //6
#define  ST2_BY_TERM_ID_AND_BATCH               106   //7
#define  ST2_GET_BATCH_NUM_DATA                 107   //8
#define  ST2_BY_STAN_AND_TERM_ID                108   //9
#define  ST2_SETTLE_TRANS_LIST                  109   //10
#define  ST2_SETTLE_TRAN                        112   //13
#define  ST2_DEL_ORIG_REV                       113   //14
#define  ST2_GET_HST01_BY_FILE_NAME             115
#define  ST2_GET_HST01_BY_FILE_NAME_KEY		   116
#define  ST2_GET_HST01_BY_USER_NAME				   117
#define  ST2_DB_SELECT_CAF01_BIN01  	         118
#define  ST2_DB_SELECT_CAF01_PAF01  	         119
#define  ST2_DB_SELECT_CAF01_CCF02P 	         120
#define  ST2_DB_SELECT_CAF01_CCF02H 	         121
#define  ST2_DB_SELECT_BCH01_BY_STATUS          122
#define  ST2_DB_SELECT_BCH01_BY_STATUS_AND_TID  123
#define  ST2_DB_SELECT_BY_ORG_ID                124
#define  ST2_DB_SELECT_CCF10_BY_CARD_NUM        125
#define  ST2_DB_SELECT_CCF10_BY_CARD_AND_ACCT   126
#define  ST2_DB_UPDATE_BCH01_GOOD_SETTLE        127
#define  ST2_DB_UPDATE_BCH01_DELETE_BCH10       128
#define  ST2_DB_GET_CAF01_LIST_BY_TYPE          129
#define  ST2_DB_SELECT_BCH10_BY_STAN            130
#define  ST2_DB_SELECT_TLF01_BY_RRN             131
#define  ST2_DB_GET_UCF01_LIST_BY_TERMINAL      132
#define  ST2_DB_GET_UCF02_LIST_BY_TERMINAL      133
#define  ST2_DB_SELECT_BCH10_BY_BATCH_TID_STAN  134
#define  ST2_DB_UPDATE_BCH01_INSERT_BCH10       135
#define  ST2_DB_GET_NGF01_LIST_BY_BINRANGE      136
#define  ST2_DB_GET_NCF01_LIST_BY_TYPE          137
#define  ST2_DB_GET_MCT01_LIST_BY_TYPE          138
#define  ST2_DB_DELETE_ALL_NTF01_RECORDS        139
#define  ST2_DB_UPDATE_MOVE_DELETE              140
#define  ST2_DB_CHECK_BPF01_BY_JOB_ID           141
#define  ST2_DB_DELETE_BPF01_BY_JOB_ID          142
#define  ST2_DB_DELETE_JBF01_BY_APP_NAME        143
#define  ST2_DB_SELECT_BCH11_BY_BATCH_NBR       144
#define  ST2_DB_SELECT_BCH10_BY_BATCH_NBR       145
#define  ST2_DB_SELECT_BCH01_BY_DATE            146
#define  ST2_DB_SELECT_OR_INSERT_BCH01_BY_STATUS 147
#define  ST2_DB_UPDATE_FAILED_PIN_ATTEMPTS      148
#define  ST2_DB_RETURN_BCH01_DELETE_BCH10       149
#define  ST2_DB_UPDATE_BCH01_TO_CLOSE_BATCH     150
#define  ST2_DB_UPDATE_BCH10_TO_CLOSE_BATCH     151
#define  ST2_DB_SELECT_TLF01_BY_STAN            152  
#define  ST2_DB_INSERT_BCH01_UPDATE_DCF01       153   
#define  ST2_DB_SELECT_ACTIVE_BCH10             154
#define  ST2_DB_INSERT_BCH10_SETTLE             155 
#define  ST2_DB_SELECT_TLF01_BY_REVERSED_TRAN	156   // by SDG on Feb 16
#define  ST2_DB_SELECT_TLF01_BY_ORIGINAL_TRAN	157	// by SDG on Feb 16
#define  ST2_DB_SELECT_TLF01_FOR_SALES_COMPLETION	158
#define  ST2_DB_SELECT_TLF01_BY_TERMINAL_ID     159  
#define  ST2_DB_SELECT_LIST_BY_PARAM			   160  
#define  ST2_DB_SELECT_TLF01_BY_AUTH_NBR    	   161   // by IA on Apr 1 for Voice Reversal
#define  ST2_DB_SELECT_BCH20_BY_STAN            162
#define  ST2_DB_SELECT_TRAN_BY_STAN             163
#define  ST2_DB_SELECT_TRAN_BY_TERMINAL_ID      164
#define  ST2_DB_SELECT_TRAN_BY_CARD_NUM         165
#define  ST2_DB_SELECT_TRAN_BY_MERCHANT_ID      166
#define  ST2_DB_UPDATE_TC_TABLES_ONLY			   167
#define  ST2_DB_GET_TLF01_LIST_FOR_LOOKUP       168    // IA GUI Repds 9/22/99
#define  ST2_DB_DELETE_BPF01_BY_PROFILE_ID      169    // IA GUI 10/18/99
#define  ST2_DB_SELECT_BCH20_BY_RRN             170

/* Size definitions for tables in shared memory */
#define  BIN01_COUNT           150
#define  CAF01_COUNT           20
#define  FAF01_COUNT           10
#define  CRF01_COUNT           10
#define  CSF01_COUNT           20

#define  MEM_ENOUGH            1
#define  MEM_ERR               2
#define  MEM_OK                0
#define  MEM_PERCENT           10             
/*
 * Size definitions for use throughout system software.
*/
#define  ACQUIRER_ID_SIZE     21
#define  TRANSACTION_ID_SIZE  21
#define  TID_SIZE             9
#define  MERCH_ID_SIZE        16
#define  PAN_SIZE             10
#define  BATCH_NUM_SIZE       7

/* DR moved common system wide defines to PTE.H
#define  ROUTER_ID_SIZE       17
#define  TPDU_ID_SIZE         16
#define  MAX_KEY_SIZE         40
*/

/*
 * Definitions to set the value of 'app_data_type' in
 * a PTE_MSG_DATA structure.
 *
 * NOTE: Pinnalce PTE common app data types are 1..99!!!  Stay above that!!!
 *
 */

#define  BIN01_DATA              100
#define  OCF01_DATA              101
#define  OCF20_DATA              102
#define  MCF01_DATA              103
#define  MCF20_DATA              104
#define  DCF01_DATA              105
#define  BCH01_DATA              106
#define  BCH10_DATA              107
#define  NCF01_DATA              108
#define  NCF20_DATA              109
#define  UCF01_DATA              110
#define  UCF02_DATA              111
#define  CAF01_DATA              112
#define  CCF01_DATA              113
#define  ACF01_DATA              114
#define  CCF03N_DATA             115
#define  CCF03P_DATA             116
#define  CCF02H_DATA             117
#define  CCF02P_DATA             118
#define  CCF10_DATA              119
#define  HST01_DATA              120
#define  ATP01_DATA              121
#define  ATP02_DATA              122
#define  CTF01_DATA              123
#define  MCT01_DATA              124
#define  PAF01_DATA              125
#define  CRF01_DATA              127
#define  JBF01_DATA              128
#define  HCF01_DATA              129
#define  FAF01_DATA              130
#define  CLF01_DATA              131
#define  BPF01_DATA              132
#define  APF01_DATA              133
#define  FCF02_DATA              134
#define  TLF01_DATA              135
#define  CSF01_DATA              136
#define  ALL_DATA				      137
#define  NCF02_DATA              138
#define  OPRMON_DATA             139
#define  NBF01_DATA              140
#define  NGF01_DATA              141
#define  NFI01_DATA              142
#define  NMF01_DATA              143
#define  NFI02_DATA              144
#define  BCH11_DATA              145
#define  NTF01_DATA              146
#define  DRF01_DATA              147
#define  DEF01_DATA              148
#define  CCF04_DATA              149
#define  BON01_DATA              150
#define  UAG01_DATA				 151 // Added by SDG on Feb 2'99
#define  BIN02_DATA              152  // Added by SDG on Feb 2'99
#define  NCF21_DATA   	         153   /* Reconciliation totals       */ 
#define  NCF30_DATA              154   /* Reconciliation transactions */ 
#define  NEG01_DATA              155   /* Negative File records       */ 
#define  ATM01_DATA              156   /* Megalink ATM Location File  */
#define  TLF01_SAF_DATA          157
#define  AUTH_TX_DATA            158
#define  MCC102_DATA             159
#define  MCC103_DATA             160
#define  TLF01_GUI_LIST_DATA     161
#define  BCH01_BCH10_TLF01_DATA  162
#define  BCH20_DATA              163
#define  SAF01_DATA              164
#define  CRF02_DATA              165



/* Bit maps for Update_Mask in TLF01 */


#define  CCF02P_MASK                 0x01
#define  CCF02H_MASK                 0x02
#define  CCF02N_MASK                 0x04
#define  CCF03P_MASK                 0x08
#define  CCF03H_MASK                 0x10
#define  CCF03N_MASK                 0x20
#define  ACF01_MASK                  0x40
#define  BON01_MASK                  0x80


/* DR moved common system wide defines to PTE.H
#define  NO_DATA_INCLUDED     0
#define  APPL_INFO_DATA       1
#define  SELECT_DATA          9
#define  DM_CONFIG_DATA       11
#define  ROUTER_DATA          12
#define  TPDU_DATA            13
*/

#define ISO_MSG_LENGTH                       0
#define SELECT_LIST_SIZE                     500
#define SERVICE_LIST_SIZE                    150
#define MAX_OPERATOR_MESSAGE_LENGTH          101
#define MAX_MESSAGEID_SIZE                   5
#define MAX_SEVERITY_SIZE                    2
#define PREFIX_TIME_LIST_SIZE                20
#define SETTLE_TRANS_LIST_SIZE               50
#define GUI_MAX_LIST_SIZE                    21
#define GUI_MAX_LIST_SIZE1                   16
#define SCH_MAX_LIST_SIZE                    100
#define PRIMARY_KEY_MAX_LEN                  50
#define BATCH_SERVICE_LIST_SIZE              30
#define MCF01_SERVICE_LIST_SIZE              30
#define CAF01_SERVICE_LIST_SIZE              30
#define FAF01_SERVICE_LIST_SIZE              70
#define DCF01_SERVICE_LIST_SIZE              50

/* DR moved common system wide defines to PTE.H
#define DIALOG_LIST_SIZE     100
*/


/* DR moved common system wide structures to PTE.H, COREINFO.H
typedef struct
{
   CHAR  executable_name   [MAX_APP_NAME_SIZE];
   CHAR  application_name  [MAX_APP_NAME_SIZE];
   CHAR  shared_queue      [MAX_QUE_NAME_SIZE];
   CHAR  control_queue     [MAX_QUE_NAME_SIZE];
   CHAR  interactive_queue [MAX_QUE_NAME_SIZE];
} APPL_INFO, *pAPPL_INFO;

*/




typedef struct
{
   CHAR message_id [MAX_MESSAGEID_SIZE] ; // 4+1
   CHAR message[MAX_OPERATOR_MESSAGE_LENGTH] ; //100+1
   CHAR application_name  [MAX_APP_NAME_SIZE] ;//16
   CHAR type [MAX_APP_NAME_SIZE] ;//16
   CHAR severity [MAX_SEVERITY_SIZE]; //1+1
} OPERATOR_MESSAGE, *pOPERATOR_MESSAGE ;

typedef struct
{
	BYTE message[256] ; // message text
	BYTE module[10] ;	// application
	BYTE function[50] ;	// function where error occurs
	BYTE severity ;		// severity of error (0,1,2,3,4) 0 is the lowest severity
	BYTE message_type[10] ; // valid values are INFO_MESSAGE, WARNING_MESSAGE, FATAL_MESSAGE
	
} OPR_MESSAGE ;

typedef struct
{
   CHAR  tid [TID_SIZE + 1];
} TERMINAL_ID, *pTERMINAL_ID;


typedef struct
{
   UINT  sale_total;
   UINT  sale_count;
   UINT  refund_total;
   UINT  refund_count;
   UINT  debit_sale_total;
   UINT  debit_sale_count;
   UINT  debit_refund_total;
   UINT  debit_refund_count;
} SETTLE_TOTALS, *pSETTLE_TOTALS;


typedef struct
{
   BYTE total_amount                       [13];
   BYTE invoice_number                     [9];
   BYTE date_yyyymmdd                      [9];
   BYTE card_num                           [21];
   BYTE exp_date                           [5];
   BYTE terminal_id                        [9];
   BYTE acquirer_id                        [21];
   BYTE time_hhmmss                        [7];
   BYTE retrieval_ref_num                  [13];
   BYTE transaction_id                     [21];
   BYTE entry_type; 
   BYTE card_acceptor_id                   [16];
   BYTE auth_number                        [7];
   BYTE tax_amount                         [13];
   BYTE tip_amount                         [13];
   BYTE settle_file_prefix                 [7];
   BYTE batch_number                       [7];
   BYTE message_type                       [5];
   BYTE processing_code                    [7];
   BYTE tx_key;
   BYTE binn_range_type                    [31];
} SETTLE_TRANS, *pSETTLE_TRANS;


typedef struct
{
   BYTE transaction_id       [21];
   BYTE terminal_id          [9];
   BYTE batch_number         [7];
} SETTLE_INFO, *pSETTLE_INFO;


typedef struct 
{
   INT num_returned;
   SETTLE_TRANS settle_trans[SETTLE_TRANS_LIST_SIZE];
} SETTLE_TRANS_LIST, *pSETTLE_TRANS_LIST;


typedef struct
{
   INT   num_returned;
   CHAR  merchant_id[SELECT_LIST_SIZE][MERCH_ID_SIZE];
} MERCHANT_LIST, *pMERCHANT_LIST;


typedef struct
{
   INT   num_returned;
   CHAR  tid[SELECT_LIST_SIZE][TID_SIZE];
} TERMINAL_LIST, *pTERMINAL_LIST;


typedef struct
{
   INT   num_returned;
   CHAR  acquirer_id[SELECT_LIST_SIZE][ACQUIRER_ID_SIZE];
} ACQUIRER_LIST, *pACQUIRER_LIST;


typedef struct
{
   INT   num_returned;
   CHAR  batch_number[SELECT_LIST_SIZE][BATCH_NUM_SIZE];
} BATCH_NUM_LIST, *pBATCH_NUM_LIST;


typedef struct
{
   BYTE    tid[TID_SIZE];
   BYTE    card_type; 
   BYTE    pan[20];  
   BYTE    acquirer_id[ACQUIRER_ID_SIZE];
} VERIFY_PAN, *pVERIFY_PAN;





#endif
