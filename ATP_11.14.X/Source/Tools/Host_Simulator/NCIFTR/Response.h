/* FILE: RESPONSE.H */
/* GOAL: To document various symbols and structures for RESPONSE.C */

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
  #define AUTH_REVERSAL_ADVICE        43
  #define AUTH_CARD_UPDATE            44
  #define AUTH_ADVICE                 42
  #define AUTH_ADVICE_REPEAT          47
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
#define AUTH_VOID_PRE_AUTHORIZATION_RESPONSE       AUTH_VOID_PRE_AUTHORIZATION  + 100
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
#define AUTH_BRANCH_PMT_RESPONSE                   AUTH_BRANCH_PMT           + 100
#define AUTH_REVERSAL_ADVICE_RESPONSE      AUTH_REVERSAL_ADVICE   + 100
#define AUTH_CARD_UPDATE_RESPONSE          AUTH_CARD_UPDATE       + 100
  #define AUTH_ADVICE_RESPONSE                AUTH_ADVICE + 100
#define AUTH_ADVICE_REPEAT_RESPONSE                AUTH_ADVICE_REPEAT     + 100

/******************************************************/
/* DATA STRUCTURE FOR STORING DATA IN FINAL FORMAT... */
/******************************************************/
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
} ISO_MESSAGE, *pISO_MESSAGE;


typedef struct
{
    unsigned char data_len[2];
    unsigned char data[1500];
} BIT_63_STRUCT;

/* End of module RESPONSE.H */
