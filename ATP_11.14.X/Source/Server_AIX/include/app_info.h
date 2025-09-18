/******************************************************************************

   Module: app_info.h
  
   Title: Application Specific Type Definitions
  
   Description: 

   Application: Equitable

   Authours: supriya.
   
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

/* Terminal Types */
#define TERMINAL_TYPE_SMART_CARD  "07"


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
#define GS_TXN_DROPPED              11

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
#define GS_TXN_PLUS_NOT_FOUND                              76
#define GS_TXN_PURCHASE_CARD_CHECK_FAILED                  77
#define GS_INVALID_AUTH_INDICATOR                          78
#define GS_TXN_INVALID_CURRENCY_CODE                       79								
#define GS_TXN_INVALID_HANDLER_QUEUE                       80
#define GS_TXN_INVALID_ORIGIN                              81
#define GS_TXN_INVALID_DEFERRED_TERM_LENGTH                82
#define GS_TXN_PAF_NOT_FOUND                               83
#define GS_TXN_DEF01_NOT_FOUND                             84
#define GS_MAGSTRIPE_FAILED                                85
#define GS_MAGSTRIPE_FAILED_REPORTED                       86
#define GS_MAGSTRIPE_FAILED_REPORTED_APPROVED              87
#define GS_ORIGINAL_TXN_NOT_FOUND_FOR_REVERSAL             88
#define GS_ORIGINAL_TXN_NOT_FOUND_FOR_VOID_SALE			   89 
#define GS_TXN_NO_MOBILE_NUM_LINK_WITH_CARD				   90
#define GS_TXN_NO_WRONG_MOBILE_NUM_IN_TRAN				   91
#define GS_TXN_3DS2_FAILED								   92
#define GS_INSUFFICIENT_PREAUTH_AMOUNT					   93
#define GS_ORIGINAL_TXN_NOT_FOUND_FOR_VOID_PREAUTH		   94
#define GS_ORIGINAL_TXN_EXPIRED_FOR_VOID_PREAUTH		   95
#define GS_ORIGINAL_TXN_PROCESSED_FOR_VOID_PREAUTH		   96
#define GS_ORIGINAL_TXN_POSTED_FOR_VOID_PREAUTH		   	   97
#define GS_ORIGINAL_TXN_VOIDED_FOR_VOID_PREAUTH		   	   98
#define GS_ORIGINAL_TXN_REVERSED_FOR_VOID_PREAUTH		   98


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

/* Card Types */
#define CARD_TYPE_LEN		  2
#define CARD_AMEX             "00"
#define CARD_VISA             "01"
#define CARD_MASTERCARD       "02"
#define CARD_JCB              "03"
#define CARD_CUP              "04"
#define CARD_DCC              "05"
#define CARD_DINERS           "06"
#define CARD_DUAL             "07"
#define CARD_FLEET            "08"


/**************Ramya Added UserID Status*****************/

#define STATUS_INACTIVE "INACTIVE"
#define STATUS_ACTIVE   "ACTIVE"
#define STATUS_EXPIRED  "EXPIRED"
#define STATUS_DELETED  "DELETED"


#define STATUS_INACTIVE_LEN 8
#define STATUS_ACTIVE_LEN	6
#define STATUS_EXPIRED_LEN  7
#define LOGIN_STATUS_LEN    1
#define STATUS_DELETED_LEN  7
/********************************************************/

/* Card Sub Type */
#define VISA_ELECTRON_PREPAID_CARD_SUBTYPE_CARD  'E'


/* Values for Mod-10 Check and Cash Bonus Eligibilty
 * -------------------------------------------------
 * These values share the Mod-10 field in Bin01.  They
 * are both flags: either on or off.
 */
#define MOD10_OFF_CASH_OFF    '0'
#define MOD10_ON_CASH_OFF     '1'
#define MOD10_OFF_CASH_ON     '2'
#define MOD10_ON_CASH_ON      '3'

/* ------------------------------------------------
 *       Mod-10, Cash Bonus, Fraudguard
 * ------------------------------------------------
 * The following extends the above 4 definitions by
 * adding Fraudguard eligibility into the mix.
 * Instead of having a constant defined for each
 * possible value, use bit masking.  Mod 10 will
 * be the least significant bit (bit 1), followed
 * by Cash Bonus Eligibility (bit 2), followed by
 * Fraudguard Eligibility (bit 3).  So it follows
 * that this field will consist of values 0-7:
 *
 *    M = Mod10, C = Cash Bonus, F = Fraudguard
 *
 *          F C M
 *        -------
 *   (0)  0 0 0 0 -----> F off, C off, M off
 *   (1)  0 0 0 1 -----> F off, C off, M on
 *   (2)  0 0 1 0 -----> F off, C on,  M off
 *   (3)  0 0 1 1 -----> F off, C on,  M on
 *   (4)  0 1 0 0 -----> F on,  C off, M off
 *   (5)  0 1 0 1 -----> F on,  C off, M on
 *   (6)  0 1 1 0 -----> F on,  C on,  M off
 *   (7)  0 1 1 1 -----> F on,  C on,  M on
 * ------------------------------------------------
 */
#define MOD10_ON  0x01
#define CASH_ON   0x02
#define FGUARD_ON 0x04


/* Keep these two values to ensure smooth transition from across versions.
 * Can remove after release 4.1.1-6 (SCR 804) is put into production.
 */
#define NONE   "0"
#define MOD10  "1"


#define IDENTIKEY             "1"
#define EIDENTIKEY            "2"
#define IBM3624               "3"
#define IBMOFFSET             "4"
#define VISA                  "5"
#define DESBILEVEL            "6"
#define DEIBOLD               "7"
#define NCR                   "8"
#define CVV                   "9"
#define PINBLOCK             "10"



/* used for message requests... tx_key */
 /* .... stay between 1 and 99 .... */
                                           /*   msg_type    procode[0] */
  #define AUTH_SALE                    1   /*    0200        00 */
  #define AUTH_REFUND                  2   /*    0200        20 */
  #define AUTH_CASH_ADVANCE            3   /*    0200        01 */
  #define AUTH_PAYMENT                 4   /*    0200        40 */
  #define AUTH_OFFLINE_SALE            5   /*    0220        00 */
  #define AUTH_OFFLINE_REFUND          6   /*    0220        20 */
  #define AUTH_SALE_ADJUSTMENT         7   /*    0220        02 */
  #define AUTH_REFUND_ADJUSTMENT       8   /*    0220        22 */
  #define AUTH_PAYMENT_ADJUSTMENT      9   /*    0220        42 */
  #define AUTH_PRE_AUTHORIZATION       10  /*    0100        30 */
  #define AUTH_AUTHORIZATION           11  /*    0100        00 */
  #define AUTH_BALANCE_INQUIRY         12  /*    0100        31 */
  #define AUTH_VOID_SALE               13  /*    0200        02 */
  #define AUTH_VOID_REFUND             14  /*    0200        22 */
  #define AUTH_SETTLE                  15  /*    0500        92 */
  #define AUTH_BATCH_UPLOAD            16  /*    0320        xx */
  #define AUTH_SETTLE_TRAILER          17  /*    0500        96 */
  #define AUTH_STATISTICS              18  /*    0800        91 */
  #define AUTH_REVERSAL                19  /*    0400        xx */
  #define AUTH_LOGON                   20  /*    0800        92 */
  #define AUTH_TEST                    21  /*    0800        99 */
  #define AUTH_PIN_CHANGE              22  /*    0800        90 */
  #define AUTH_VOID_CASH_ADVANCE       23  /*    0200        20 */
  #define AUTH_SALE_CASH               24  /*    0200        09 */
  #define AUTH_CHECK_VERIFICATION      25  /*    0100        04 */
  #define AUTH_CARD_VERIFICATION       26  /*    0100        38 */
  #define AUTH_VOID_PRE_AUTHORIZATION  27  /*    0100        02 */
  #define AUTH_SALES_COMPLETION        28  /*    0200        20 */
  #define AUTH_GUARANTEED_LATE_ARRIVAL 29  /*    0220        90 */
  #define AUTH_BATCH_DOWN_LINE_LOAD    30  /*    0500        95 */
  #define AUTH_SIGNATURE_DATA          31  /*    0320        90 */
  #define AUTH_INITIALIZATION          32  /*    0800        93 */
  #define AUTH_PLEASE_WAIT_ADVICE      33  /*    0830        90*/
  #define AUTH_NEGATIVE_DOWNLOAD       34  /*    0200        20 */
  #define AUTH_ADMIN_ADVICE            35
  #define AUTH_VOID_PAYMENT            36
  #define AUTH_REGISTRATION            37
  #define AUTH_CHECK_DL                38
  #define AUTH_BRANCH_PMT              39
  #define AUTH_ATM_CONFIRM             40
  #define AUTH_QUASI_CASH              41
  #define AUTH_ADVICE                  42
  #define AUTH_REVERSAL_ADVICE         43  /*	0420			*/
  #define AUTH_CARD_UPDATE             44
  #define AUTH_RECONCILE_ADVICE        45
  #define AUTH_NETWORK_ADVICE          46
  #define AUTH_ADVICE_REPEAT           47
  #define AUTH_REVERSAL_REPEAT         48

  #define AUTH_CASH_ADVANCE_AUTHORIZATION                49
  #define AUTH_DEFERRED_PURCHASE_AUTHORIZATION           50
  #define AUTH_DEFERRED_PURCHASE_AUTHORIZATION_OVERRIDE  51
  #define AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE       52
  #define AUTH_AUTHORIZATION_OVERRIDE                    53

  #define AUTH_OFFLINE_VOID_SALE        54
  #define AUTH_OFFLINE_VOID_REFUND      55

  #define AUTH_ISSUER_REVERSAL_ADVICE   56  /* 0422/0432 - Cirrus */
  #define AUTH_ISSUER_RECONCILE_ADVICE  57  /* 0522/0532 - Cirrus */

  /*  VISA SMS messages */
  #define AUTH_BACKOFFICE_ADJUSTMENT         58
  #define AUTH_CHARGEBACK                    59
  #define AUTH_CHARGEBACK_REVERSAL           60
  #define AUTH_REPRESENTMENT                 61
  #define AUTH_RECONCILIATION                62
  #define AUTH_ADMINISTRATIVE_TEXT           63
  #define AUTH_KEY_CHANGE                    64
  #define AUTH_CASH_DISBURSEMENT_ADJUSTMENT  65
  #define AUTH_FUND_TRANSFER_TOTAL           66
  #define AUTH_FEE_COLLECTION_ACQUIRER       67  /* 0220, 19 */
  #define AUTH_FEE_COLLECTION_ISSUER         68  /* 0422, 19 */
  #define AUTH_FUNDS_DISBURSEMENT_ACQUIRER   69  /* 0220, 29 */
  #define AUTH_FUNDS_DISBURSEMENT_ISSUER     70  /* 0422, 29 */

  /* Reload */
  #define AUTH_RELOAD                        71  /* 0200, 21 */
  #define AUTH_RELOAD_REPEAT                 72  /* 0201, 21 */
  #define AUTH_RELOAD_CONFIRM                73  /* 0202, 21 */
  #define AUTH_VOID_RELOAD                   74  /* 0200, 22 */

  #define AUTH_EMV_ADVICE                    75  /* 0340,    */
  #define AUTH_EMV_UPLOAD                    76  /* 0320, 94 */
  #define AUTH_REVERSAL_ADVICE_REPEAT         77 /* 421, 00 TF-Ajay 14 Dec,2009*/
  #define AUTH_OCT_TX						 78 /* 0200, 260000 */
  #define AUTH_FAST_FUND					 79 /* 0200, 100000 */
  #define AUTH_MONEY_SEND 					80 /* 0100 28*/

  /*  CUP messages */
  #define AUTH_PRE_AUTHORIZATION_CANCLE     81  /* 0100 20****   */
  #define CUP_KEY_DESIABLE_FLAG             82
  #define CUP_KEY_ENABLE_FLAG             	83
  #define AUTH_REFUND_REVERSAL             	84
  #define AUTH_INVALID                  99


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
#define AUTH_VOID_PRE_AUTHORIZATION_RESPONSE       AUTH_VOID_PRE_AUTHORIZATION    + 100
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
#define AUTH_ATM_CONFIRM_RESPONSE                  AUTH_ATM_CONFIRM       + 100
#define AUTH_QUASI_CASH_RESPONSE                   AUTH_QUASI_CASH        + 100
#define AUTH_ADVICE_RESPONSE                       AUTH_ADVICE            + 100
#define AUTH_REVERSAL_ADVICE_RESPONSE              AUTH_REVERSAL_ADVICE   + 100
#define AUTH_CARD_UPDATE_RESPONSE                  AUTH_CARD_UPDATE       + 100
#define AUTH_RECONCILE_ADVICE_RESPONSE             AUTH_RECONCILE_ADVICE  + 100
#define AUTH_NETWORK_ADVICE_RESPONSE               AUTH_NETWORK_ADVICE    + 100
#define AUTH_ADVICE_REPEAT_RESPONSE                AUTH_ADVICE_REPEAT     + 100
#define AUTH_REVERSAL_REPEAT_RESPONSE              AUTH_REVERSAL_REPEAT   + 100
#define AUTH_CASH_ADVANCE_AUTHORIZATION_RESPONSE               AUTH_CASH_ADVANCE_AUTHORIZATION                + 100
#define AUTH_DEFERRED_PURCHASE_AUTHORIZATION_RESPONSE          AUTH_DEFERRED_PURCHASE_AUTHORIZATION           + 100
#define AUTH_DEFERRED_PURCHASE_AUTHORIZATION_OVERRIDE_RESPONSE AUTH_DEFERRED_PURCHASE_AUTHORIZATION_OVERRIDE  + 100
#define AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE_RESPONSE      AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE       + 100
#define AUTH_AUTHORIZATION_OVERRIDE_RESPONSE                   AUTH_AUTHORIZATION_OVERRIDE                    + 100


#define AUTH_OFFLINE_VOID_SALE_RESPONSE         AUTH_OFFLINE_VOID_SALE   + 100
#define AUTH_OFFLINE_VOID_REFUND_RESPONSE       AUTH_OFFLINE_VOID_REFUND + 100

#define AUTH_ISSUER_REVERSAL_ADVICE_RESPONSE    AUTH_ISSUER_REVERSAL_ADVICE  + 100
#define AUTH_ISSUER_RECONCILE_ADVICE_RESPONSE   AUTH_ISSUER_RECONCILE_ADVICE + 100

/* visa sms messages */
#define AUTH_BACKOFFICE_ADJUSTMENT_RESPONSE  AUTH_BACKOFFICE_ADJUSTMENT + 100
#define AUTH_CHARGEBACK_RESPONSE             AUTH_CHARGEBACK            + 100
#define AUTH_CHARGEBACK_REVERSAL_RESPONSE    AUTH_CHARGEBACK_REVERSAL   + 100
#define AUTH_REPRESENTMENT_RESPONSE          AUTH_REPRESENTMENT         + 100
#define AUTH_RECONCILIATION_RESPONSE         AUTH_RECONCILIATION        + 100
#define AUTH_ADMINISTRATIVE_TEXT_RESPONSE    AUTH_ADMINISTRATIVE_TEXT   + 100
#define AUTH_KEY_CHANGE_RESPONSE             AUTH_KEY_CHANGE            + 100
#define AUTH_CASH_DISBURSEMENT_ADJUSTMENT_RESPONSE             AUTH_CASH_DISBURSEMENT_ADJUSTMENT            + 100
#define AUTH_FUND_TRANSFER_TOTAL_RESPONSE    AUTH_FUND_TRANSFER_TOTAL   + 100
#define AUTH_FEE_COLLECTION_ACQUIRER_RESPONSE     AUTH_FEE_COLLECTION_ACQUIRER    +100
#define AUTH_FEE_COLLECTION_ISSUER_RESPONSE       AUTH_FEE_COLLECTION_ISSUER      +100
#define AUTH_FUNDS_DISBURSEMENT_ACQUIRER_RESPONSE AUTH_FUNDS_DISBURSEMENT_ACQUIRER+100
#define AUTH_FUNDS_DISBURSEMENT_ISSUER_RESPONSE   AUTH_FUNDS_DISBURSEMENT_ISSUER  +100

/* Reload Responses */
#define AUTH_RELOAD_RESPONSE          AUTH_RELOAD         + 100
#define AUTH_RELOAD_REPEAT_RESPONSE   AUTH_RELOAD_REPEAT  + 100
#define AUTH_RELOAD_CONFIRM_RESPONSE  AUTH_RELOAD_CONFIRM + 100
#define AUTH_VOID_RELOAD_RESPONSE     AUTH_VOID_RELOAD    + 100

#define AUTH_EMV_ADVICE_RESPONSE      AUTH_EMV_ADVICE     + 100
#define AUTH_EMV_UPLOAD_RESPONSE      AUTH_EMV_UPLOAD     + 100
#define AUTH_REVERSAL_ADVICE_REPEAT_RESPONSE    AUTH_REVERSAL_ADVICE_REPEAT +100 /* 421, 00 TF-Ajay 14 Dec,2009*/
#define AUTH_OCT_TX_RESPONSE		  AUTH_OCT_TX + 100					  
#define AUTH_FAST_FUND_RESPONSE		  AUTH_FAST_FUND  + 100	  
#define AUTH_MONEY_SEND_RESPONSE	 AUTH_MONEY_SEND + 100

/*	CUP messages */
#define AUTH_PRE_AUTHORIZATION_CANCLE_RESPONSE   AUTH_PRE_AUTHORIZATION_CANCLE  + 100

#define  AUTH_TX_KEY_MAX			  199	


/*
 * The following definitions are for use in
 * the 'msg_type' field of the PTE_MSG.
 *
 * NOTE: Pinnacle PTE common message types are 1..99!!!  Stay above that!!!
 *
 */
#define MT_AUTH_REQUEST                100
#define MT_AUTH_RESPONSE               101
#define MT_OPERATOR                    102  /* messages meant for operations monitor */
#define MT_BUILD_FILE                  103


#define MT_SEND_LOG_ON                 104 
#define MT_SEND_LOG_OFF                105 
#define MT_SEND_ECHO_TEST              106
#define MT_SHUT_DOWN                   107
#define MT_AUTH_WORKSTATION            108
#define MT_AUTH_WORKSTATION_RESPONSE   109
#define MT_SYS_MONITOR                 110

#define MT_RESTART                     111
#define MT_NEGATIVE_FILE               112
#define MT_CLEANUP                     113
#define MT_INCOMING_GUI                114

#define MT_GET_STATS                   115
#define MT_GET_STATS_RESPONSE          116
#define MT_RESET_STATS                 117
#define MT_RESET_STATS_RESPONSE        118

#define MT_UPDATE_CONFIG               119  /* FG Mode-Refresh Property File */
#define MT_GET_CONFIG_INFO             120  /* FG Mode-Get SOAP Properties   */
#define MT_BATCH_SEND_REQUEST          121  /* FG Mode-Send Batch            */
#define MT_LOGON_RESPONSE			   122  /* Support Automatic Resend Stand-In Tx */ // ThoughtFocus Girija Y
#define MT_INCOMING_DCPIMP			   123  
#define MT_INCOMING_DCPISO			   124
#define MT_INCOMING_DCPOS              125

#define MT_AUTH_RT_REQUEST				127
#define MT_RT_RESPONSE_APPROVED			128
#define MT_RT_RESPONSE_DECLINE			129
#define MT_RT_RESPONSE_TIMEOUT			130
#define MT_AUTH_EXT10_REQUEST			131
#define MT_AUTH_RT_EMPTY_RESPONSE		132


/*
 *  Used for logon/logoff messages 
 */ 
#define  SOURCE_IS_HOST      0
#define  SOURCE_IS_FSVISA    1
#define  SOURCE_IS_STATE     2
#define  SOURCE_IS_OPERATOR  3   

/*
 * Host States
 */
#define   ONLINE    "0"  /* Online with the host              */
#define   OFFLINE   "1"  /* Offline with the host             *
#define   QUERYING  "2"   * Unknown or busy                   *
#define   DOWN      "3"   * Network Controller is not running */

#define   cONLINE   '0'  /* Online with the host              */
#define   cOFFLINE  '1'  /* Offline with the host             *
#define   cQUERYING '2'   * Unknown or busy                   *
#define   cDOWN     '3'   * Network Controller is not running */

/* Below are new host states for release 4.2.1, SCR 1065.
 * The above definition of host states may not be needed
 * but leave them in for a while to allow some backward
 * compatibility as EB transitions release 4.2.1 into prod.
 */
#define   BOTH_ONLINE   "0"  /* Iss & Acq states are online       */
#define   BOTH_OFFLINE  "1"  /* Iss & Acq states are offline      */
#define   QUERYING      "2"  /* Unknown or busy                   */
#define   DOWN          "3"  /* Network Controller is not running */
#define   ISSON_ACQOFF  "4"  /* Iss is online, Acq is offline     */
#define   ISSOFF_ACQON  "5"  /* Iss is offline, Acq is online     */

#define   cBOTH_ONLINE  '0'  /* Iss & Acq states are online       */
#define   cBOTH_OFFLINE '1'  /* Iss & Acq states are offline      */
#define   cQUERYING     '2'  /* Unknown or busy                   */
#define   cDOWN         '3'  /* Network Controller is not running */
#define   cISSON_ACQOFF '4'  /* Iss is online, Acq is offline     */
#define   cISSOFF_ACQON '5'  /* Iss is offline, Acq is online     */


/*
 * Acquiring/Issuing Network Type Definitions
 */
#define   cIssuing   'I'
#define   cAcquiring 'A'
#define   sIssuing   "I"
#define   sAcquiring "A"


#define MC_ASI_TXN_IDENTIFIER_VALUE          '8'
/*
 * The following definitions are for use in
 * the 'msg_type' field of the PTE_MSG.
 */
#define  MT_NSP_KEYCHANGE				   					200 /* CRYPTO MSG TYPES */
#define  MT_NSP_KEYCHANGE_RESPONSE							201
#define  MT_NSP_LOCAL_KEYCHANGE 			   				202
#define  MT_NSP_LOCAL_KEYCHANGE_RESPONSE					203
#define  MT_NSP_XLATETOANSI									204
#define  MT_NSP_XLATETOANSI_RESPONSE						205
#define  MT_NSP_VERIFYPIN									206
#define  MT_NSP_VERIFYPIN_RESPONSE							207
#define  MT_NSP_CVVCVC						   				208
#define  MT_NSP_CVVCVC2			    		    			209
#define  MT_NSP_CVVCVC_RESPONSE								210
#define  MT_NSP_CHANGEPIN					    			211
#define  MT_NSP_CHANGEPIN_RESPONSE							212
#define	 MT_NSP_REM_KEYCHANGE				   				213
#define  MT_NSP_REM_KEYCHANGE_RESPONSE						214
#define  MT_SCHEDULER_CONTROL            					215
#define  MT_NSP_VERIFY_MAGSTRIPE          					216
#define	 MT_NSP_VERIFY_MAGSTRIPE_RESPONSE 					217
#define  MT_NSP_TRANSLATE_PIN             					218
#define	 MT_NSP_TRANSLATE_PIN_RESPONSE  	  				219
#define  MT_NSP_VERIFY_ARQC 								220
#define  MT_NSP_VERIFY_ARQC_RESPONSE						221
#define  MT_NSP_VERIFY_3D_SECURE							222
#define  MT_NSP_VERIFY_3D_SECURE_RESPONSE					223
#define  MT_NSP_CUP_KEY_UPDATE_REQUEST         				224
#define  MT_NSP_CUP_KEY_RESPONSE                			225
#define  MT_NSP_TRANSLATE_ZMK_LMK							226
#define  MT_NSP_TRANSLATE_ZMK_LMK_RESPONSE 					227
#define  MT_NSP_CUP_KEY_TRANSLATE_RESPONSE      			228
#define  MT_NSP_DINERS_KEY_RESPONSE            			 	229
#define  MT_NSP_DINERS_KEY_TRANSLATE_RESPONSE   			230
#define  MT_NSP_DINERS_GENERATE_KEY							231
#define  MT_NSP_DINERS_GENERATE_KEY_RESPONSE				232
#define  MT_NSP_VERIFY_CUP_ECOM_CVN2            			233
#define  MT_NSP_VERIFY_CUP_ECOM_CVN2_RESPONSE   			234
#define  MT_NSP_VERIFY_3D_SECURE_2							235
#define  MT_NSP_VERIFY_3D_SECURE_RESPONSE_2					236
#define  MT_NSP_VERIFY3DS2_MAGSTRIPE						237
#define  MT_NSP_TRANSLATE_PIN_KEYBLOCK          			238
#define	 MT_NSP_TRANSLATE_PIN_RESPONSE_KEYBLOCK				239
#define  MT_NSP_DINERS_GENERATE_KEY_KEYBLOCK				240
#define  MT_NSP_DINERS_GENERATE_KEY_RESPONSE_KEYBLOCK		241
#define  MT_NSP_TRANSLATE_ZMK_LMK_KEYBLOCK					242
#define  MT_NSP_TRANSLATE_ZMK_LMK_RESPONSE_KEYBLOCK 		243
#define  MT_NSP_VERIFYPIN_KEYBLOCK							244
#define  MT_NSP_VERIFYPIN_RESPONSE_KEYBLOCK					245
#define  MT_NSP_VERIFY_MAGSTRIPE_KEYBLOCK          			246
#define	 MT_NSP_VERIFY_MAGSTRIPE_RESPONSE_KEYBLOCK 			247
#define  MT_NSP_VERIFY3DS2_MAGSTRIPE_KEYBLOCK				248
#define  MT_NSP_VERIFY_ARQC_KEYBLOCK 						249
#define  MT_NSP_VERIFY_ARQC_RESPONSE_KEYBLOCK				250
#define  MT_NSP_VERIFY_3D_SECURE_KEYBLOCK					251
#define  MT_NSP_VERIFY_3D_SECURE_RESPONSE_KEYBLOCK			252
#define  MT_NSP_VERIFY_3D_SECURE_2_KEYBLOCK					253
#define  MT_NSP_VERIFY_3D_SECURE_RESPONSE_2_KEYBLOCK		254
#define  MT_NSP_VERIFY_CUP_ECOM_CVN2_KEYBLOCK            	255
#define  MT_NSP_VERIFY_CUP_ECOM_CVN2_RESPONSE_KEYBLOCK   	160
#define  MT_NSP_KEYCHANGE_KEYBLOCK				   			161 /* CRYPTO MSG TYPES */
#define  MT_NSP_KEYCHANGE_RESPONSE_KEYBLOCK					162
#define  MT_NSP_DINERS_KEY_RESPONSE_KEYBLOCK				163
#define	 MT_NSP_DECRYPT_TRANSLATE_PIN						164
#define	 MT_NSP_DECRYPT_TRANSLATE_PIN_RESPONSE				165
#define	 MT_NSP_DECRYPT_PIN									166
#define	 MT_NSP_DECRYPT_PIN_RESPONSE						167

/*
 * The following definitions are for use in
 * the 'msg_subtype1' field of the PTE_MSG.
 *
 * NOTE: Pinnacle PTE common message subtypes are 1..99!!!  Stay above that!!!
 *
 */


#define  ST1_DB_GET_CONTEXT         100   
#define  ST1_DB_VERIFY_PAN          101   
#define  ST1_AUTH_ERROR             102   
#define  ST1_FROM_TERM_HANDLER      103   
#define  ST1_FROM_HOST_HANDLER      104   
#define  ST1_DB_CHECK_BIN_RANGE     105   
#define  ST1_DB_GET_SETTLE_TOTALS   106   
#define  ST1_DB_MOVE_TRANS          107   
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
#define  ST1_DB_SELECT_AUTH_ID		127  
#define  ST1_DB_CHECK_BIN02_RANGE   128  
#define  ST1_SAF_REQUEST            129  
#define  ST1_SIGNON_ADVICE			130
#define  ST1_SIGNOFF_ADVICE			131

#define ST1_CUP_KEY_REQUEST			201
#define ST1_CUP_KEY_SEND			202
#define ST1_CUP_KEY_TRANSLATION_SUCCESS     203
#define ST1_CUP_KEY_TRANSLATION_FAILED      204
#define ST1_DINERS_KEY_TEST_REQUEST			205
#define STI_CUP_KEY_RELOAD                  206
#define ST1_CUP_ACTIVE_ACTIVE_TRIGGER       207
#define ST1_CUP_ACTIVE_ACTIVE_TRIGGER_FAILED      208
#define ST1_CUP_ACTIVE_ACTIVE_TRIGGER_SUCCESS     209
#define ST1_DINERS_KEY_RELOD_REQUEST			  210
#define ST1_DCI_ACTIVE_ACTIVE_TRIGGER			  211
#define ST1_DCI_ACTIVE_ACTIVE_TRIGGER_FAILED      212
#define ST1_DCI_ACTIVE_ACTIVE_TRIGGER_SUCCESS     213
#define ST1_PULSE_KEY_REQUEST					  214
#define ST1_PULSE_KEY_SEND						  215
#define STI_PULSE_KEY_RELOAD					  216	
/*
 * The following definitions are for use in
 * the 'msg_subtype2' field of the PTE_MSG.
 *
 * NOTE: Pinnacle PTE common message subtypes are 1..99!!!  Stay above that!!!
 *
 */
#define  ST2_REVERSED_TRAN                        105   
#define  ST2_BY_TERM_ID_AND_BATCH                 106   
#define  ST2_GET_BATCH_NUM_DATA                   107   
#define  ST2_BY_STAN_AND_TERM_ID                  108   
#define  ST2_SETTLE_TRANS_LIST                    109   
#define  ST2_SETTLE_TRAN                          112   
#define  ST2_DEL_ORIG_REV                         113   
#define  ST2_GET_HST01_BY_FILE_NAME               115
#define  ST2_GET_HST01_BY_FILE_NAME_KEY           116
#define  ST2_GET_HST01_BY_USER_NAME               117
#define  ST2_DB_SELECT_CAF01_BIN01                118
#define  ST2_DB_SELECT_CAF01_PAF01                119
#define  ST2_DB_SELECT_CAF01_CCF02P               120
#define  ST2_DB_SELECT_CAF01_CCF02H               121
#define  ST2_DB_SELECT_BCH01_BY_STATUS            122
#define  ST2_DB_SELECT_BCH01_BY_STATUS_AND_TID    123
#define  ST2_DB_SELECT_BY_ORG_ID                  124
#define  ST2_DB_SELECT_CCF10_BY_CARD_NUM          125
#define  ST2_DB_SELECT_CCF10_BY_CARD_AND_ACCT     126
#define  ST2_DB_UPDATE_BCH01_GOOD_SETTLE          127
#define  ST2_DB_UPDATE_BCH01_DELETE_BCH10         128
#define  ST2_DB_GET_CAF01_LIST_BY_TYPE            129
#define  ST2_DB_SELECT_BCH10_BY_STAN              130
#define  ST2_DB_SELECT_TLF01_BY_RRN               131
#define  ST2_DB_GET_UCF01_LIST_BY_TERMINAL        132
#define  ST2_DB_GET_UCF01_USERLIST				  209
#define  ST2_DB_GET_UCF02_LIST_BY_TERMINAL        133
#define  ST2_DB_SELECT_BCH10_BY_BATCH_TID_STAN    134
#define  ST2_DB_UPDATE_BCH01_INSERT_BCH10         135
#define  ST2_DB_GET_NGF01_LIST_BY_BINRANGE        136
#define  ST2_DB_GET_NCF01_LIST_BY_TYPE            137
#define  ST2_DB_GET_MCT01_LIST_BY_TYPE            138
#define  ST2_DB_DELETE_ALL_NTF01_RECORDS          139
#define  ST2_DB_UPDATE_MOVE_DELETE                140
#define  ST2_DB_CHECK_BPF01_BY_JOB_ID             141
#define  ST2_DB_DELETE_BPF01_BY_JOB_ID            142
#define  ST2_DB_DELETE_JBF01_BY_APP_NAME          143
#define  ST2_DB_SELECT_BCH11_BY_BATCH_NBR         144
#define  ST2_DB_SELECT_BCH10_BY_BATCH_NBR         145
#define  ST2_DB_SELECT_BCH01_BY_DATE              146
#define  ST2_DB_SELECT_OR_INSERT_BCH01_BY_STATUS  147
#define  ST2_DB_UPDATE_FAILED_PIN_ATTEMPTS        148
#define  ST2_DB_RETURN_BCH01_DELETE_BCH10         149
#define  ST2_DB_UPDATE_BCH01_TO_CLOSE_BATCH       150
#define  ST2_DB_UPDATE_BCH10_TO_CLOSE_BATCH       151
#define  ST2_DB_SELECT_TLF01_BY_STAN              152  
#define  ST2_DB_INSERT_BCH01_UPDATE_DCF01         153   
#define  ST2_DB_SELECT_ACTIVE_BCH10               154
#define  ST2_DB_INSERT_BCH10_SETTLE               155 
#define  ST2_DB_SELECT_TLF01_BY_REVERSED_TRAN     156   
#define  ST2_DB_SELECT_TLF01_BY_ORIGINAL_TRAN     157	
#define  ST2_DB_SELECT_TLF01_FOR_SALES_COMPLETION 158
#define  ST2_DB_SELECT_TLF01_BY_TERMINAL_ID       159  
#define  ST2_DB_SELECT_LIST_BY_PARAM              160  
#define  ST2_DB_SELECT_TLF01_BY_AUTH_NBR          161   
#define  ST2_DB_SELECT_BCH20_BY_STAN              162
#define  ST2_DB_SELECT_TRAN_BY_STAN               163
#define  ST2_DB_SELECT_TRAN_BY_TERMINAL_ID        164
#define  ST2_DB_SELECT_TRAN_BY_CARD_NUM           165
#define  ST2_DB_SELECT_TRAN_BY_MERCHANT_ID        166
#define  ST2_DB_SELECT_BCH20_BY_RRN               167
#define  ST2_DB_SELECT_BCH10_BY_RRN               168
#define  ST2_DB_UPDATE_BCH01_DELETE_BCH10_FOR_GUI 169
#define  ST2_DB_DELETE_BPF01_BY_PROFILE_ID        170
#define  ST2_DB_SELECT_NEG_FILE_BY_CARD_NBR       171
#define  ST2_DB_UPDATE_TC_TABLES_ONLY             172
#define  ST2_DB_SELECT_BCH20_BY_AUTH_NUMBER       173 
#define  ST2_ISSUER                               174 
#define  ST2_ACQUIRER                             175 
#define  ST2_REPORT                               176
#define  ST2_DB_SELECT_TLF01_FOR_TERM_LENGTH      177
#define  ST2_DB_SELECT_NCF30_BY_RRN_AND_TERMINAL  178
#define  ST2_DB_SELECT_NCF30_FOR_VOICE            179
#define  ST2_DB_SELECT_TLF01_FOR_VISA_REJECTS     180
#define  ST2_DB_SELECT_BCH01_BY_BATCH_NBR         181
#define  ST2_DB_SELECT_BCH10_DUPLICATE_TXN        182
#define  ST2_DB_DELETE_ALL_CCF04_MCC_RECORDS      183
#define  ST2_GET_ALL_FG_SUCCESS                   184
#define  ST2_GET_ALL_FG_FAILED                    185
#define  ST2_GET_10_FG_FAILED                     186
#define  ST2_GET_10_MORE_FG                       187
#define  ST2_DB_SELECT_NUMERIC_AUTH_ID            188
#define  ST2_NO_VERSION                           189
#define  ST2_DB_SELECT_LAST_AUTH_ID               190 
#define  ST2_DB_SELECT_STANDIN					  191
#define  ST2_DB_SELECT_TLF01_STANDIN              192 
#define  ST2_DB_GET_UCF03_LIST_BY_TERMINAL        193/*Added UCF03 for PCI changes -TF Srikanth*/
/* Krishna-TF,2.6.11: added 2 new Message Types for Dual Encryption Select & update */
#define  ST2_DB_SELECT_FOR_DUALENCRYPTN 			194
#define  ST2_DB_UPDATE_FOR_DUALENCRYPTN 			195
#define  ST2_DB_SELECT_FOR_DUALENCRYPTN_OTHER_USER 196
#define  ST2_DB_SELECT_FOR_DUALENCRYPTN_KEY_UPDAT_COUNT 197
#define ST2_DB_SELECT_MCC_CODES 					198
#define ST2_DB_SELECT_BCH20_BY_AUTH_NUMBER_CARD_NBR 199

#define MAX_MCC_ALLOWED_LIST 200
#define ST2_DB_SELECT_BCH10_FOR_CUP      201
#define ST2_DB_SELECT_BCH20_FOR_CUP      202
#define ST2_DB_SELECT_BCH20_FOR_PREAUTH       203
#define ST2_DB_SELECT_BCH20_FOR_INCREMENTAL_PREAUTH  	204
#define ST2_DB_SELECT_UCF01_FOR_USR_STS_CNRL       			205/*user status control */
#define ST2_DB_SELECT_TLF01_FOR_REVERSAL_PUL				206  /* For ncpul reversal */
#define ST2_DB_SELECT_BCH20_FOR_PREAUTH_REVERSE  		207
#define ST2_DB_SELECT_TLF01_FOR_PURCHANGE_RETURN 			208
#define ST2_DB_SELECT_BIN_RANGE_CFB_TXN  209

#define ST2_DB_SELECT_NCF30_INCREMENTAL_AUTHORIZATION   210
#define ST2_DB_SELECT_NCF30_PARTIAL_REVERSAL_AUTHORIZATION   211
#define ST2_DB_SELECT_BCH20_FOR_ESTIMATED_AUTHORIZATION   212
#define ST2_DB_SELECT_BCH20_FOR_AMEX_PREAUTH	213
#define ST2_DB_SELECT_FRULE01_BY_CARD_BRAND 220
#define ST2_DB_GET_FRULE01_LIST_BY_CARD_BRAND 221
#define ST2_NCIFTR_ONLINE 222
#define ST2_DB_SELECT_BCH20_FOR_PREAUTH_VOID 223
#define ST2_DB_SELECT_BCH20_FOR_PREAUTH_TOTAL_AMOUNT 224

/* Size definitions for tables in shared memory */
#define  BIN01_COUNT           500
#define  CAF01_COUNT           80
#define  FAF01_COUNT           10
#define  CRF01_COUNT           10
#define  CSF01_COUNT           20

#define  MEM_ENOUGH            1
#define  MEM_ERR               2
#define  MEM_OK                0
#define  MEM_PERCENT           10

#define  LOCK                  0
#define  UNLOCK                1
#define  NOLOCK                2

#define  LOCKED               '0'
#define  UNLOCKED             '1'

#define  TABLE_ALREADY_LOCKED -1

/*
 * Size definitions for use throughout system software.
*/
#define  ACQUIRER_ID_SIZE     21
#define  TRANSACTION_ID_SIZE  21
#define  TID_SIZE             9
#define  MERCH_ID_SIZE        16
#define  PAN_SIZE             10
#define  BATCH_NUM_SIZE       7

/*
 * Definitions to set the value of 'app_data_type' in
 * a PTE_MSG_DATA structure.
 *
 * NOTE: Pinnacle PTE common app data types are 1..99!!!  Stay above that!!!
 *
 */

#define  BIN01_DATA                  100
#define  OCF01_DATA                  101
#define  OCF20_DATA                  102
#define  MCF01_DATA                  103
#define  MCF20_DATA                  104
#define  DCF01_DATA                  105
#define  BCH01_DATA                  106
#define  BCH10_DATA                  107
#define  NCF01_DATA                  108
#define  NCF20_DATA                  109
#define  UCF01_DATA                  110
#define  UCF02_DATA                  111
#define  CAF01_DATA                  112
#define  CCF01_DATA                  113
#define  ACF01_DATA                  114
#define  CCF03N_DATA                 115
#define  CCF03P_DATA                 116
#define  CCF02H_DATA                 117
#define  CCF02P_DATA                 118
#define  CCF10_DATA                  119
#define  HST01_DATA                  120
#define  ATP01_DATA                  121
#define  ATP02_DATA                  122
#define  CTF01_DATA                  123
#define  MCT01_DATA                  124
#define  PAF01_DATA                  125
#define  CRF01_DATA                  127
#define  JBF01_DATA                  128
#define  HCF01_DATA                  129
#define  FAF01_DATA                  130
#define  CLF01_DATA                  131
#define  BPF01_DATA                  132
#define  APF01_DATA                  133
#define  FCF02_DATA                  134
#define  TLF01_DATA                  135
#define  CSF01_DATA                  136
#define  ALL_DATA				     137
#define  NCF02_DATA                  138
#define  OPRMON_DATA                 139
#define  NBF01_DATA                  140
#define  NGF01_DATA                  141
#define  NFI01_DATA                  142
#define  NMF01_DATA                  143
#define  NFI02_DATA                  144
#define  BCH11_DATA                  145
#define  NTF01_DATA                  146
#define  DRF01_DATA                  147
#define  DEF01_DATA                  148
#define  CCF04_DATA                  149
#define  BON01_DATA                  150
#define  UAG01_DATA                  151 
#define  BIN02_DATA                  152 
#define  NCF21_DATA   	             153  
#define  NCF30_DATA                  154  
#define  NEG01_DATA                  155  
#define  ATM01_DATA                  156 
#define  TLF01_SAF_DATA              157
#define  AUTH_TX_DATA                158
#define  MCC102_DATA                 159
#define  MCC103_DATA                 160
#define  TLF01_GUI_LIST_DATA         161
#define  BCH01_BCH10_TLF01_DATA      162
#define  BCH20_DATA                  163
#define  SAF01_DATA                  164
#define  OFFUS_VELOCITY_DATA         165
#define  NEGATIVE_FRAUD_DATA         166
#define  MCC_LIMITS_DATA             167
#define  MCC_LIMITS_GUI_DATA         168
#define  EMV01_DATA                  169
#define  COUNTRY_AMT_LIMITS_DATA     170
#define  COUNTRY_AMT_LIMITS_GUI_DATA 171
#define  FG01_DATA                   172
#define  FGUARD_DATA                 173
#define  FGRAW_DATA                  174
#define  BON02_DATA                  175
#define  BON03_DATA                  176
#define  UCF03_DATA                  177 /*Added UCF03 DATA for PCI changes -TF Srikanth*/
#define  DB_PASSWORD_DATA            178 /*Added UCF03 DATA for PCI changes -TF Srikanth*/
#define  ENC_DEC_DATA		     179 /* Added for ENC DEC key-update*/
#define	 OMC01_DATA					180 /*Override Merchant Control*/
#define	 MCF01_ID_DATA					181 /*Used for select on MCF01 with only merchant id*/
#define  MCC01_DATA					182	/* for MCC01 table */
#define MRA_AUTH_TBL_DATA 			183 /* MRA AUTH TABLE*/
#define DB_ARCH_PASSWORD_DATA 		184
#define DB_MRA_PASSWORD_DATA 		185


#define  CAF01_EMV_DATA				186 /* CAF01 EMV table*/

#define  ARCH01_DATA				187 /*For Archive TLF table */
#define  BCH01_MP_DATA              188
#define  CUP_KEY_UPDATE             189
#define  MGF01_DATA                 190
#define  MPPARA01_DATA              191
#define  TPOS01_DATA                192
#define  RPS01_DATA		    		193
#define  DINERS_KEY_UPDATE          194
#define  GPCF01_DATA		    	195
#define  TLF01_STIP_DATA			196
#define  MIDBINBLK01_DATA           197
#define MRA01_DATA					198
#define OBS01_DATA					199
#define VISA3DS2_DATA               200
#define KEY3DS01_DATA               201
#define AMEX3DS2_DATA               202
#define CUP3DS2_DATA                203
#define DINERS3DS2_DATA             204
#define JCB3DS2_DATA                205
#define	FTLF01_DATA					206
#define FCLF01_DATA					207
#define FRULE01_DATA				208
#define CR01_DATA                   209
#define MCF03P_DATA                 210
#define MERCH_INT_PARAM_DATA		211
#define CCF02P_DATA_RT              212
#define ACF01_DATA_RT               213
#define PTSN01_DATA                 214
#define MERCH_INT_PARAM_MC_MAC_DATA 215
#define ATP_CARD_STATUS_UPDATE_BY_BDO_HOST   216
#define PAF01_DATA_RT				217
#define CCL01_DATA					218
#define BDKKEY01_DATA				219
#define CAF01_KEYBLOCK_DATA			220
#define NCF01_KEYBLOCK01_DATA		221
#define DINERS_KEY_UPDATE_KEYBLOCK  222

/* Bit maps for Update_Mask in TLF01 */
#define  CCF02P_MASK                 0x01
#define  CCF02H_MASK                 0x02
#define  OFFUS_VELOCITY_MASK         0x04
#define  CCF03P_MASK                 0x08
#define  CCF03H_MASK                 0x10
#define  CCF03N_MASK                 0x20
#define  ACF01_MASK                  0x40
#define  BON01_MASK                  0x80
#define  OMC01_MASK			         0x10/*praneeth Code*/


#define ISO_MSG_LENGTH                       0
#define SELECT_LIST_SIZE                     500
/* ThoughtFocus nirbhay : increased the list size from 500 to 1500. */
#define SELECT_SERVICE_BIN01_LIST_SIZE       9999
#define SERVICE_LIST_SIZE                    1500
#define MAX_NO_OF_RECORDS_FOR_BIN_CAF		 1000
#define MAX_OPERATOR_MESSAGE_LENGTH          101
#define MAX_MESSAGEID_SIZE                   5
#define MAX_SEVERITY_SIZE                    2
#define PREFIX_TIME_LIST_SIZE                20
#define SETTLE_TRANS_LIST_SIZE               50
/* 21 was the previous value .. TF Phani ..Increased the buffer size */
#define GUI_MAX_LIST_SIZE                    21
#define SCH_MAX_LIST_SIZE                    1000
#define PRIMARY_KEY_MAX_LEN                  50
#define BATCH_SERVICE_LIST_SIZE              30
#define MCF01_SERVICE_LIST_SIZE              30
#define CAF01_SERVICE_LIST_SIZE              500
#define FAF01_SERVICE_LIST_SIZE              70
#define DCF01_SERVICE_LIST_SIZE              50
#define GUI_MAX_LIST_SIZE1	                 10
#define ARCH_GUI_MAX_LIST_SIZE	             21
#define MAX_INI_ID_LIST_SIZE                 50
#define SELECT_MCC_LIST_SIZE                 500
#define CURR_CODE_LIST_SIZE					 300
#define BDK_KEY_LIST_SIZE					 10


/* Authorization Methods */
#define M_NEGATIVE_OFFUS_NEGATIVE_FRAUD      "F"
#define M_POSITIVE_FILE                      "P"
#define M_NEGATIVE_FILE                      "N"
#define M_POSITIVE_WITH_BALANCE_FILE         "B"
#define M_POSITIVE_WITH_PRIMARY_BALANCE_FILE "D"
#define M_NEGATIVE_WITH_OFFUS_VELOCITY       "O"
#define M_OFFUS_VELOCITY                     "V"


/* --------------------------------- */
/* Frauguard (FG) Status Definitions */
/* --------------------------------- */
#define FG_FAILED              '0' // Response came back FALSE
#define FG_SUCCESS             '1' // Response came back TRUE
#define FG_REQUEST_SENT        '2' // Request has been sent to FG, no resp yet
#define FG_SEND_REQUEST_FAILED '3' // Unable to send to SOAP Module
#define FG_COMM_FAILED         '4' // SOAP couldn't send or failed to send to FG
#define FG_RESULT_UNKNOWN      '5' // Resp came back, but not true or false
#define FG_RESP_NO_MATCH       '6' // Resp came back, but can't find tran Id
#define FG_REQUEST_RECEIVED    '7' // Request from FG - currently just log it


/* ---------------------------- */
/* Fraudguard GUI Request Modes */
/* ---------------------------- */
#define FG_MODE_REFRESH_PROPERTIES   0 /* Have FGSOAP update property file    */
#define FG_MODE_GET_PROPERTIES       1 /* Have FGSOAP send properties to GUI  */
#define FG_MODE_SEND_BATCH           2 /* Resend batch of failed transactions */



typedef struct
{
   CHAR message_id [MAX_MESSAGEID_SIZE] ; /* 4+1*/
   CHAR message[MAX_OPERATOR_MESSAGE_LENGTH] ; /*100+1*/
   CHAR application_name  [MAX_APP_NAME_SIZE] ;/*16*/
   CHAR type [MAX_APP_NAME_SIZE] ;/*16*/
   CHAR severity [MAX_SEVERITY_SIZE]; /*1+1*/
} OPERATOR_MESSAGE, *pOPERATOR_MESSAGE ;

typedef struct
{
	BYTE message[256] ; 
	BYTE module[10] ;	
	BYTE function[50] ;	
	BYTE severity ;		
	BYTE message_type[10] ; 
	
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

/*+-------------------------------------------------------------+
  | Counts displayed on Monitor in Transaction Activity section |
  +-------------------------------------------------------------+*/

/* Cannot use long numbers for this structure.  This structure
 * is passed between an NT workstation and a Unix box.  The
 * two apparently represent these differently in memory.
 */
typedef struct
{
   CHAR   cnt_from_network[10];
   CHAR   cnt_to_network[10];
   CHAR   num_approved[10];
   CHAR   num_declined[10];
   CHAR   num_reversed[10];
   CHAR   num_time_out[10];
   CHAR	  num_mgt_request[10];/*praneeth*/
   CHAR   num_stip_sent[10];	/*TF Anna*/
   CHAR   num_stip_recv[10];	/*TF Anna*/
} MONITOR_STATS, *pMONITOR_STATS;

/* This structure defines where the counts are actually accumulated. */
typedef struct
{
   DWORD  sent_count;
   DWORD  received_count;
   DWORD  approved_count;
   DWORD  declined_count;
   DWORD  reversed_count;
   DWORD  time_out_count;
   DWORD  mgtrequest_count;/*praneeth*/
   DWORD  stip_sent_count;	/*TF Anna*/
   DWORD  stip_recv_count;	/*TF Anna*/
} TXN_COUNTS, *pTXN_COUNTS;

/* Statistic Types */
#define  SENT      0
#define  RECEIVED  1
#define  NA        9
#define  MGT	   8 //praneeth
#define  STIP_SENT 10	//TF Anna
#define  STIP_RECV 11	//TF Anna

#define  MAX_STAT_COUNT   999999UL


/* --------------------------------- */
/* Oracle Database Timing Statistics */
/* --------------------------------- */

#define MAX_TIMINGS    100

typedef struct
{
   BYTE  sql_type;      /* Subtype1: select, insert, update, delete */
   BYTE  subtype2;
   BYTE  table;         /* App_Data Type */
   LONG  num_queries;   /* Number of times this query ran */
   float avg_duration;
   float max_duration;
} TIMINGS, *pTIMINGS;

typedef struct
{
	INT		num_entries;
	TIMINGS		st_timings[MAX_TIMINGS];

} TIMINGS_LIST, *pTIMINGS_LIST;
/*****************************************************
		STIP Identifiers
******************************************************/
#define BDOR_62_PREAUTH_VOID 1

#define PROCESSED			"procsd"
#define VOIDED				"voided"
#define REVERSED			"revers"
#define STANDIN				"stndin"
#define POSTED				"posted"
#define EXPIRED 			"expird"
#define USED				"prused"
#define PARREVSAL			"parvse"
#define TRAN_STATUS_LEN 	6

#ifndef WIN32
CHAR parvbh_que_name[8]  = "parvbhA";
#endif

/* ENC- DEC - definations*/
#define ENC_DEC_KEY_SIZE 25

/* Krishna-TF 10.6.11: Defined constant strings as Macros*/
#define UPDATED       "UPDATED"
#define USER_LOCKED	  "locked"
#define USER_UNLOCKED "unlocked"
#define USER_NEW	  "new"
#define DELETED      "DELETED"


/*CUP  MID and Filed 61 indicator*/
#define CUP_CARD_INDICATOR_FLAG "CC"
#define CUP_CARD_NUM   "6"
#define CUP_MID_INDICATOR_DEFAULT "7240608"
#define CUP_MID_INDICATOR_LEN      7

/*CUP EMV Capability Indicator*/
#define CUP_EMV_CAPABILITY_INDICATOR     "UC"
#define CUP_DEVICE_EMV_CAPABILITY_LEN    2
#define DEVICE_EMV_CAPABILITY_POS_FOR_CUP 2

/*Amex EMV terminal indicator*/
#define AMEX_EMV_CAPABILITY_INDICATOR     "AC"
#define AMEX_DEVICE_EMV_CAPABILITY_LEN    2
/*Diners EMV Capability Indicator*/
#define DINERS_EMV_CAPABILITY_INDICATOR     			"DC"
#define DINERS_DEVICE_EMV_CAPABILITY_LEN    			2
#define DEVICE_EMV_CAPABILITY_POS_FOR_DINERS    		4

/* Error Warning logs decleration */
#define NUM_SIZE_WRITES  1
#define ONE_MB_IN_BYTES  1048576

#define FRT_DECLINE_RC_LEN 					2
#define FRT_DECLINE_AC_LEN 					3
#define FRT_DECLINE_RC_MP_DEFAULT			"01"
#define FRT_DECLINE_RC_PLFC_DEFAULT			"01"

#define FRT_DECLINE_RC_ACQ_MC_DEFAULT			"01"
#define FRT_DECLINE_RC_ACQ_VISA_DEFAULT			"01"
#define FRT_DECLINE_RC_ACQ_AMEX_DEFAULT			"01"
#define FRT_DECLINE_RC_ACQ_JCB_DEFAULT			"01"
#define FRT_DECLINE_RC_ACQ_UPI_DEFAULT			"01"
#define FRT_DECLINE_RC_ACQ_DCI_DEFAULT			"01"

#define FRT_DECLINE_RC_ISS_MC_DEFAULT			"63"
#define FRT_DECLINE_RC_ISS_VISA_DEFAULT			"59"
#define FRT_DECLINE_RC_ISS_AMEX_DEFAULT			"107"
#define FRT_DECLINE_RC_ISS_JCB_DEFAULT			"59"
#define FRT_DECLINE_RC_ISS_UPI_DEFAULT			"59"
#define FRT_DECLINE_RC_ISS_DCI_DEFAULT			"102"


#define FRT_DECLINE_RC_VOICE_MC_DEFAULT			"01"
#define FRT_DECLINE_RC_VOICE_VISA_DEFAULT		"01"
#define FRT_DECLINE_RC_VOICE_AMEX_DEFAULT		"01"
#define FRT_DECLINE_RC_VOICE_JCB_DEFAULT		"01"
#define FRT_DECLINE_RC_VOICE_UPI_DEFAULT		"01"
#define FRT_DECLINE_RC_VOICE_DCI_DEFAULT		"01"

#define KEYBLOCK_ENABLE 1
#define KEYBLOCK_DISABLE 0
#define KEYBLOCK_FLAG_LEN 1

#endif




