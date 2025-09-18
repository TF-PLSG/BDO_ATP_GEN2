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

/******************************************************/
/* DATA STRUCTURE FOR STORING DATA IN FINAL FORMAT... */
/******************************************************/
struct iso_msg_rec
   {
        BYTE msgtype_bcd   [4];			//DI - 04/14/99 was size [2]
        BYTE bitmap1_bin   [8];
        BYTE bitmap2_bin   [8];
        BYTE pan_len       [1];			//farad 8/9/98	BYTE pan_len       [2];
        BYTE pan_bin       [10];		//farad 8/9/98	BYTE pan           [19];
        BYTE proccode      [3];			//farad 8/9/98	BYTE proccode      [6];
        BYTE amount_bin    [6];			//farad 8/9/98	BYTE amount        [12]; 
										//farad 8/9/98  BYTE settle_amt    [12];
        BYTE card_bill_amt [6];		//farad 8/9/98	BYTE ch_bill_amt   [12];
        BYTE gmt_time      [5];			//farad 8/9/98	BYTE gmt_time      [10];
        BYTE conv_rate	   [4];			//farad 8/9/98  BYTE conv_rate_stl [8];
        								//farad 8/9/98	BYTE conv_rate_chb [8];
        BYTE stan_bin      [3];			//farad 8/9/98	BYTE stan          [6];
        BYTE time_bin      [3];			//farad 8/9/98  BYTE time          [6];
        BYTE date_bin      [2];			//farad 8/9/98  BYTE date          [4];
        BYTE date_expd     [2];			//farad 8/9/98  BYTE date_expd     [4];
        BYTE date_settle   [2];			//farad 8/9/98  BYTE date_settle   [4]; 
								        //farad 8/9/98	BYTE conv_date     [4];
        BYTE sic_number    [2];			//farad 8/9/98  BYTE sic_number    [4];
        BYTE ctry_code     [2];			//farad 8/9/98  BYTE ctry_code     [3];
		  BYTE pan_ctry_cd   [2];
        BYTE posentry      [2];	
		BYTE card_seq_num  [2];			//farad 8/9/98  BYTE posentry      [3]; 
        BYTE nii           [2];			  
        BYTE pos_condcd    [1];			   
        BYTE pin_captcd    [1];			   
        BYTE acq_inst_len  [1];			//farad 8/9/98  BYTE acq_inst_len  [2];
		  BYTE acq_inst_id   [6]; 		//farad 8/10/98 BYTE acq_inst_id   [11];
        BYTE fwd_inst_len  [1];			
        BYTE fwd_inst_id   [7];
        BYTE pan_ext_len   [1];
        BYTE pan_ext       [15];
        BYTE track2_len    [1];
		  BYTE track2        [37];
        BYTE rrn           [12];
        BYTE authcode      [6];			 
        BYTE responsecd    [2];
		  BYTE src 		      [3];			//farad 8/10/98 Did not exist in the mcrd.
        BYTE termid        [8];
        BYTE cardaccid     [15];		 
		  BYTE cardaccname   [40];		//farad 8/10/98 Did not exist in the mcrd.
        BYTE addl_rsp_len  [1];			//farad 8/10/98 BYTE addl_rsp_len  [2];
        BYTE addlrspdata   [25];
        BYTE track1_len    [1];			//farad 8/10/98  BYTE track1_len    [2];
        BYTE track1        [76];
        BYTE addl_data_len [1];			//farad 8/10/98	 BYTE addl_data_len [3];
        BYTE addl_data     [225];		//farad 8/10/98  BYTE addl_data     [100];
        BYTE tran_curr_cd  [3];			//farad 8/10/98  BYTE tran_curr_cd  [3];
        							  	         //farad 8/10/98  BYTE settle_cur_cd [3];
        BYTE card_curr_cd  [3];			//farad 8/10/98	 BYTE card_curr_cd  [3];
		  BYTE pin_block   [8];			//farad 8/10/98  did not exist in the mcrd.
        BYTE sec_cntl      [8];
        BYTE addl_len      [1];			//farad 8/10/98  BYTE addl_len      [3];
        BYTE addlamts      [120];		//farad 8/10/98	 BYTE addlamts      [120];
     	BYTE iccdata_len   [3];								//farad 8/10/98  BYTE advice_cd_len [3];
		BYTE iccdata       [255];								//farad 8/10/98  BYTE advice_code   [60];
        BYTE posdata_len   [1];			//farad 8/10/98  BYTE pos_data_len  [3];
        BYTE posdata       [14];		//farad 8/10/98  BYTE pos_data      [26];
     	
        BYTE priv60_len    [1];
        BYTE priv60        [3];
        BYTE priv61_len    [1];
        BYTE priv61        [18];
        BYTE priv62_len    [1];
        BYTE priv62        [58];
        BYTE priv63_len    [1];
        BYTE priv63        [255];
        BYTE nmi_code      [2];
        BYTE action        [3];
        BYTE orig_data     [21];
        BYTE file_update   [1];
        BYTE resp_ind      [5];
        BYTE repl_amts     [42];
        BYTE msgsec        [8];
        BYTE setl_inst_len [1];
        BYTE setl_inst_id  [7];
        BYTE recv_inst_len [1];
        BYTE recv_inst_id  [7];
        BYTE fname_len     [1];
        BYTE filename      [18];
        BYTE acct_id_1_len [1];
        BYTE acct_id_1     [29];
        BYTE acct_id_2_len [1];
        BYTE acct_id_2     [29];
        BYTE trandesc_len  [1];
        BYTE trandesc      [100];
        BYTE intra_ctry_len [1];
        BYTE intra_ctry    [256];
        BYTE record_len    [2];
        BYTE record        [30];
        BYTE iss_inst_len  [1];
        BYTE iss_inst_id   [12];
        BYTE open_use_len  [1];
        BYTE open_use      [14];
        BYTE addr_verf_len [1];
        BYTE addr_verf     [29];
        BYTE free_text_len [1];
        BYTE free_text     [136];
        BYTE access_cd_len [2];
        BYTE access_cd     [255];
		  BYTE dest_stat_id  [3];
    	
} ISO_MESSAGE, *pISO_MESSAGE;


typedef struct
{
    unsigned char data_len[2];
    unsigned char data[1500];
} BIT_63_STRUCT;

/* End of module RESPONSE.H */

