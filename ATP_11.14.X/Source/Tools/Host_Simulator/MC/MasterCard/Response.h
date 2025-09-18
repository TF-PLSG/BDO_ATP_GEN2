/* FILE: RESPONSE.H */
/* GOAL: To document various symbols and structures for RESPONSE.C */

/*****************************************************************************/
/**                          BIT MAP SYMBOLS                                **/
/*****************************************************************************/
#define BF01_ON   0x80
#define BF01_off     0
#define BF02_ON   0x40
#define BF02_off     0
#define BF03_ON   0x20
#define BF03_off     0
#define BF04_ON   0x10
#define BF04_off     0
#define BF05_ON   0x08
#define BF05_off     0
#define BF06_ON   0x04
#define BF06_off     0
#define BF07_ON   0x02
#define BF07_off     0
#define BF08_ON   0x01
#define BF08_off     0
#define BF09_ON   0x80
#define BF09_off     0
#define BF10_ON   0x40
#define BF10_off     0
#define BF11_ON   0x20
#define BF11_off     0
#define BF12_ON   0x10
#define BF12_off     0
#define BF13_ON   0x08
#define BF13_off     0
#define BF14_ON   0x04
#define BF14_off     0
#define BF15_ON   0x02
#define BF15_off     0
#define BF16_ON   0x01
#define BF16_off     0
#define BF17_ON   0x80
#define BF17_off     0
#define BF18_ON   0x40
#define BF18_off     0
#define BF19_ON   0x20
#define BF19_off     0
#define BF20_ON   0x10
#define BF20_off     0
#define BF21_ON   0x08
#define BF21_off     0
#define BF22_ON   0x04
#define BF22_off     0
#define BF23_ON   0x02
#define BF23_off     0
#define BF24_ON   0x01
#define BF24_off     0
#define BF25_ON   0x80
#define BF25_off     0
#define BF26_ON   0x40
#define BF26_off     0
#define BF27_ON   0x20
#define BF27_off     0
#define BF28_ON   0x10
#define BF28_off     0
#define BF29_ON   0x08
#define BF29_off     0
#define BF30_ON   0x04
#define BF30_off     0
#define BF31_ON   0x02
#define BF31_off     0
#define BF32_ON   0x01
#define BF32_off     0
#define BF33_ON   0x80
#define BF33_off     0
#define BF34_ON   0x40
#define BF34_off     0
#define BF35_ON   0x20
#define BF35_off     0
#define BF36_ON   0x10
#define BF36_off     0
#define BF37_ON   0x08
#define BF37_off     0
#define BF38_ON   0x04
#define BF38_off     0
#define BF39_ON   0x02
#define BF39_off     0
#define BF40_ON   0x01
#define BF40_off     0
#define BF41_ON   0x80
#define BF41_off     0
#define BF42_ON   0x40
#define BF42_off     0
#define BF43_ON   0x20
#define BF43_off     0
#define BF44_ON   0x10
#define BF44_off     0
#define BF45_ON   0x08
#define BF45_off     0
#define BF46_ON   0x04
#define BF46_off     0
#define BF47_ON   0x02
#define BF47_off     0
#define BF48_ON   0x01
#define BF48_off     0
#define BF49_ON   0x80
#define BF49_off     0
#define BF50_ON   0x40
#define BF50_off     0
#define BF51_ON   0x20
#define BF51_off     0
#define BF52_ON   0x10
#define BF52_off     0
#define BF53_ON   0x08
#define BF53_off     0
#define BF54_ON   0x04
#define BF54_off     0
#define BF55_ON   0x02
#define BF55_off     0
#define BF56_ON   0x01
#define BF56_off     0
#define BF57_ON   0x80
#define BF57_off     0
#define BF58_ON   0x40
#define BF58_off     0
#define BF59_ON   0x20
#define BF59_off     0
#define BF60_ON   0x10
#define BF60_off     0
#define BF61_ON   0x08
#define BF61_off     0
#define BF62_ON   0x04
#define BF62_off     0
#define BF63_ON   0x02
#define BF63_off     0
#define BF64_ON   0x01
#define BF64_off     0

#define BF65_ON   0x80
#define BF65_off     0
#define BF66_ON   0x40
#define BF66_off     0
#define BF67_ON   0x20
#define BF67_off     0
#define BF68_ON   0x10
#define BF68_off     0
#define BF69_ON   0x08
#define BF69_off     0
#define BF70_ON   0x04
#define BF70_off     0
#define BF71_ON   0x02
#define BF71_off     0
#define BF72_ON   0x01
#define BF72_off     0
#define BF73_ON   0x80
#define BF73_off     0
#define BF74_ON   0x40
#define BF74_off     0
#define BF75_ON   0x20
#define BF75_off     0
#define BF76_ON   0x10
#define BF76_off     0
#define BF77_ON   0x08
#define BF77_off     0
#define BF78_ON   0x04
#define BF78_off     0
#define BF79_ON   0x02
#define BF79_off     0
#define BF80_ON   0x01
#define BF80_off     0
#define BF81_ON   0x80
#define BF81_off     0
#define BF82_ON   0x40
#define BF82_off     0
#define BF83_ON   0x20
#define BF83_off     0
#define BF84_ON   0x10
#define BF84_off     0
#define BF85_ON   0x08
#define BF85_off     0
#define BF86_ON   0x04
#define BF86_off     0
#define BF87_ON   0x02
#define BF87_off     0
#define BF88_ON   0x01
#define BF88_off     0
#define BF89_ON   0x80
#define BF89_off     0
#define BF90_ON   0x40
#define BF90_off     0
#define BF91_ON   0x20
#define BF91_off     0
#define BF92_ON   0x10
#define BF92_off     0
#define BF93_ON   0x08
#define BF93_off     0
#define BF94_ON   0x04
#define BF94_off     0
#define BF95_ON   0x02
#define BF95_off     0
#define BF96_ON   0x01
#define BF96_off     0
#define BF97_ON   0x80
#define BF97_off     0
#define BF98_ON   0x40
#define BF98_off     0
#define BF99_ON   0x20
#define BF99_off     0
#define BF100_ON   0x10
#define BF100_off     0
#define BF101_ON   0x08
#define BF101_off     0
#define BF102_ON   0x04
#define BF102_off     0
#define BF103_ON   0x02
#define BF103_off     0
#define BF104_ON   0x01
#define BF104_off     0
#define BF105_ON   0x80
#define BF105_off     0
#define BF106_ON   0x40
#define BF106_off     0
#define BF107_ON   0x20
#define BF107_off     0
#define BF108_ON   0x10
#define BF108_off     0
#define BF109_ON   0x08
#define BF109_off     0
#define BF110_ON   0x04
#define BF110_off     0
#define BF111_ON   0x02
#define BF111_off     0
#define BF112_ON   0x01
#define BF112_off     0
#define BF113_ON   0x80
#define BF113_off     0
#define BF114_ON   0x40
#define BF114_off     0
#define BF115_ON   0x20
#define BF115_off     0
#define BF116_ON   0x10
#define BF116_off     0
#define BF117_ON   0x08
#define BF117_off     0
#define BF118_ON   0x04
#define BF118_off     0
#define BF119_ON   0x02
#define BF119_off     0
#define BF120_ON   0x01
#define BF120_off     0
#define BF121_ON   0x80
#define BF121_off     0
#define BF122_ON   0x40
#define BF122_off     0
#define BF123_ON   0x20
#define BF123_off     0
#define BF124_ON   0x10
#define BF124_off     0
#define BF125_ON   0x08
#define BF125_off     0
#define BF126_ON   0x04
#define BF126_off     0
#define BF127_ON   0x02
#define BF127_off     0
#define BF128_ON   0x01
#define BF128_off     0


/*****************************************************************************/
/**                          BIT FIELD SYMBOLS                              **/
/*****************************************************************************/
enum bitfields
{   bf_done           = 0,      /* end of list                          */
   bf_1xbitfield     = 1,      /* extended bit field                   */
   bf_2pan           = 2,      /* pan                                  */
   bf_3proc          = 3,      /* processing code                      */
   bf_4amount        = 4,      /* amount                               */
   bf_5settleamt     = 5,      /* settle amount                        */
   bf_6chbillgamt    = 6,      /* cardholder billing amount            */
   bf_7trntime       = 7,      /* transmission date-time               */
   bf_9convratesetl  = 9,      /* conversion rate, settlement          */
   bf_10convrate     = 10,     /* Conversion rate, cardholder billing  */
   bf_11stan         = 11,     /* system trace number                  */
   bf_12time         = 12,     /* time                                 */
   bf_13date         = 13,     /* date, trans                          */
   bf_14expiry       = 14,     /* expiry date                          */
   bf_15datestl      = 15,     /* settlement date                      */
   bf_16convdate     = 16,     /* conversion date                      */
   bf_18sicnumber    = 18,     /* Merchant's type of business          */
   bf_19ctrycd       = 19,     /* Acquiring institution's country code */
   bf_20panctrycd    = 20,     /* Identifies country of card issuer    */
   bf_22pose         = 22,     /* pos entry mode                       */
   bf_23panext       = 23,     /* PAN Extention                        */
   bf_24nii          = 24,     /* nii                                  */
   bf_25pocc         = 25,     /* pos condition code                   */
   bf_32idcode       = 32,     /* Acquiring institution's ID code      */
   bf_33fwdid        = 33,     /* Forwarding institution's ID code     */
   bf_35track2       = 35,     /* track 2 data                         */
   bf_37rrn          = 37,     /* retrieval reference number           */
   bf_38authcode     = 38,     /* auth code                            */
   bf_39respcode     = 39,     /* response code                        */
   bf_41termid       = 41,     /* terminal id                          */
   bf_42cardacc      = 42,     /* card acceptor id                     */
   bf_44respdata     = 44,     /* additional response data             */
   bf_45track1       = 45,     /* track 1 data                         */
   bf_48addldata     = 48,     /* additional data                      */
   bf_49trncurcd     = 49,     /* transaction currency code            */
   bf_50stlmtcurrcd  = 50,     /* settlement currency code             */
   bf_51crdcurcd     = 51,     /* card currency code                   */
   bf_52pinblock     = 52,     /* pin block                            */
   bf_54addlamts     = 54,     /* additional amounts                   */
   bf_55emv          = 55,     /* Chip Card information                */
   bf_56ISOuse		 = 56,		/* ISO use */
   bf_60advicecode   = 60,     /* advice reason code                   */
   bf_61posdata      = 61,     /* POS data                             */
   bf_62inf          = 62,     /* Intermediate Network Facility data   */
   bf_63banknet      = 63,     /* BankNet data                         */
   bf_70nmicode      = 70,     /* network management information code  */
   bf_90orgdata      = 90,     /* original data elements               */
   bf_91filecode     = 91,     /* file update code                     */
   bf_94servind      = 94,     /* service indicator                    */
   bf_96security     = 96,     /* Message security code                */
   bf_100recvid      = 100,    /* Receiving institution Id code        */
   bf_101filename    = 101,    /* File Name                            */
   bf_102acct1       = 102,    /* account identification 1             */
   bf_103acct2       = 103,    /* account identification 2             */
   bf_120orgmsgid    = 120,    /* original message type id             */
   bf_121mcagent     = 121,    /* authorizing agent id code            */
   bf_127privdata    = 127     /* private data                         */
}; 


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
        unsigned char panext        [3];
        unsigned char nii           [2];
        unsigned char pos_condcd    [1];
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
        unsigned char addl_rsp_len  [2];
        unsigned char addlrspdata   [25];
        unsigned char track1_len    [2];
        unsigned char track1        [76];
        unsigned char addl_data_len [3];
        unsigned char addl_data     [100];
        unsigned char tran_curr_cd  [3];
        unsigned char settle_cur_cd [3];
        unsigned char card_curr_cd  [3];
        unsigned char emv_len       [3];
        unsigned char emv           [256];
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

/* End of module RESPONSE.H */

