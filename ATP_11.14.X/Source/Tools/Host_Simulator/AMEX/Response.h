/* FILE: RESPONSE.H */
/* GOAL: To document various symbols and structures for RESPONSE.C */



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
        unsigned char nii           [2];
        unsigned char pos_condcd    [1];
        unsigned char tran_fee      [9];
        unsigned char setl_fee      [9];
        unsigned char setl_procfee  [9];
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
        unsigned char cardaccloc    [40];
        unsigned char addl_rsp_len  [2];
        unsigned char addlrspdata   [25];
        unsigned char track1_len    [2];
        unsigned char track1        [76];
        unsigned char addl_data_len [3];
        unsigned char addl_data     [100];
        unsigned char tran_curr_cd  [3];
        unsigned char settle_cur_cd [3];
        unsigned char card_curr_cd  [3];
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
        unsigned char replace_amts  [42];
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
		unsigned char IccData_len   [2];
		unsigned char IccData	    [255];
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

  #define AUTH_SALE                   1   /*    0200        00*/
  #define AUTH_REFUND                 2   /*    0200        20*/
  #define AUTH_CASH_ADVANCE           3   /*    0200        01*/
  #define AUTH_PAYMENT                4   /*    0200        40 */
  #define AUTH_OFFLINE_SALE           5   /*    0220        00 */
  #define AUTH_OFFLINE_REFUND         6   /*    0220        20 */
  #define AUTH_SALE_ADJUSTMENT        7   /*    0220        02 */
  #define AUTH_REFUND_ADJUSTMENT      8   /*    0220        22 */
  #define AUTH_PAYMENT_ADJUSTMENT     9   /*    0220        42 */
  #define AUTH_PRE_AUTHORIZATION      10  /*    0100        30 */
  #define AUTH_AUTHORIZATION          11  /*    0100        00 */
  #define AUTH_BALANCE_INQUIRY        12  /*    0100        31 */
  #define AUTH_VOID_SALE              13  /*/    0200        02*/
  #define AUTH_VOID_REFUND            14  /*/    0200        22 */
  #define AUTH_SETTLE                 15  /*/    0500        92 */
  #define AUTH_BATCH_UPLOAD           16  /*/    0320        xx */
  #define AUTH_SETTLE_TRAILER         17  /*/    0500        96 */
  #define AUTH_STATISTICS             18  /*/    0800        91 */
  #define AUTH_REVERSAL               19  /*/    0400        xx */
  #define AUTH_LOGON                  20  /*/    0800        92 */
  #define AUTH_TEST                   21  /*/    0800        99 */
  #define AUTH_PIN_CHANGE             22  /*/    0800        90*/
  #define AUTH_VOID_CASH_ADVANCE      23  /*/    0200        20*/
  #define AUTH_SALE_CASH              24  /*    0200        09 */
  #define AUTH_CHECK_VERIFICATION     25  /*    0100        04 */
  #define AUTH_CARD_VERIFICATION      26  /*    0100        38 */
  #define AUTH_VOID_PRE_AUTHORIZATION 27  /*    0100        02 */
  #define AUTH_SALES_COMPLETION       28  /*    0200        20 */
  #define AUTH_GUARANTEED_LATE_ARRIVAL 29  /*   0220        90 */
  #define AUTH_BATCH_DOWN_LINE_LOAD   30  /*    0500        95 */
  #define AUTH_SIGNATURE_DATA         31  /*    0320        90 */
  #define AUTH_INITIALIZATION         32  /*    0800        93 */
  #define AUTH_PLEASE_WAIT_ADVICE     33  /*    0830        90*/
  #define AUTH_NEGATIVE_DOWNLOAD      34  /*    0200        20 */
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

  #define AUTH_CASH_ADVANCE_AUTHORIZATION				49 /* Added by SDG for Voice Authorizations*/
  #define AUTH_DEFERRED_PURCHASE_AUTHORIZATION		    50 /* Added by SDG for Voice Authorizations	*/
  #define AUTH_DEFERRED_PURCHASE_AUTHORIZATION_OVERRIDE	51 /* Added by IA for Voice Authorizations	04-08-99*/
  #define AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE		52 /* Added by IA for Voice Authorizations	04-08-99*/
  #define AUTH_AUTHORIZATION_OVERRIDE		            53 /* Added by IA for Voice Authorizations	04-08-99*/
 
  #define AUTH_OFFLINE_VOID_SALE		            54 /* Added by EP 07-20-99*/
  #define AUTH_OFFLINE_VOID_REFUND		            55 /* Added by EP 07-20-99*/

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

#define AUTH_OFFLINE_VOID_SALE_RESPONSE		            AUTH_OFFLINE_VOID_SALE + 100
#define AUTH_OFFLINE_VOID_REFUND_RESPONSE		        AUTH_OFFLINE_VOID_REFUND + 100



struct iso_msg_rec
{
   BYTE msgtype       [4];
   BYTE bitmap1_bin   [16];
   BYTE bitmap2_bin   [16];
   BYTE pan_len       [2];
   BYTE pan           [19];
   BYTE proccode      [6];
   BYTE amount        [12];
   BYTE setl_amount   [12];
   BYTE gmt_time      [10];
   BYTE setl_conv_rate[8];
   BYTE stan          [6];
   BYTE datetime      [12];
   BYTE eff_date      [4];
   BYTE date_expd     [4];
   BYTE date_settle   [4];
   BYTE date_conv     [4];
   BYTE merch_type    [4];
   BYTE ctry_code     [3];
   BYTE coun_code     [3];
   BYTE fwd_inst_cnt_code [3];
   BYTE posentry      [12];
   BYTE func_cd       [3];
   BYTE pos_condcd    [4];
   BYTE accpt_bus_cd  [4];
   BYTE orig_amt      [24];	
   BYTE tran_fee_amt  [8];
   BYTE tranid_len    [2];
   BYTE tranid        [40];
   BYTE acq_inst_len  [2];
   BYTE acq_inst_id   [11];
   BYTE fwd_inst_len  [2];
   BYTE fwd_inst_id   [11];
   BYTE track2_len    [2];
   BYTE track2        [37];
   BYTE rrn           [12];
   BYTE authcode      [6];
   BYTE responsecd    [3];
   BYTE termid        [8];
   BYTE cardaccid     [15];
   BYTE cardaccnamelen[2];
   BYTE cardaccname   [101];
   BYTE addl_rsp_len  [2];
   BYTE addlrspdata   [25];
   BYTE track1_len    [2];
   BYTE track1        [76];
   BYTE addl_data_len [3];
   BYTE addl_data     [70];
   BYTE tran_curr_cd  [3];
   BYTE recn_curr_cd  [3];
   BYTE pinblock      [16];
   BYTE sec_cntl_len  [2];
   BYTE sec_cntl      [8];
   BYTE orig_data_len [2];
   BYTE orig_data     [35];
   BYTE IccData_len   [3];
   BYTE IccData	      [264];
   BYTE priv63_len    [3];
   BYTE priv63	       [99];
};

/* End of module RESPONSE.H */

