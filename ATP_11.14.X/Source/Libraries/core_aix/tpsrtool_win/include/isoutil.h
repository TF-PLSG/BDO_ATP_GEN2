/*

//							DEVELOPED BY 
//		THOUGHTFOCUS (C) INFORMATION TECHNOLOGIES PVT. LTD.
//							BANGALORE, INDIA
//#*********************************************************************************
//#*********************************************************************************
*/
/**************************************************************************************************
*  
* MODULE:      ISOUTIL.H
*  
* DESCRIPTION: Contains function prototypes and definitions for the ISO standard utilities.  
*
*
**************************************************************************************************/
#ifndef __ISOUTIL_H_
#define __ISOUTIL_H_

enum bitfields {
   bf_done           = 0,      /* end of list    */
   bf_2pan           = 2,      /* pan            */
   bf_3proc          = 3,      /* processing code */
   bf_4amount        = 4,      /* amount   */
   bf_11stan         = 11,     /* system trace number */
   bf_12time         = 12,     /* time    */
   bf_13date         = 13,     /* date, trans  */
   bf_14expiry       = 14,     /* expiry date  */
   bf_15datestl      = 15,     /* settlement date */
   bf_22pose         = 22,     /* pos entry mode */
   bf_24nii          = 24,     /* nii      */
   bf_25pocc         = 25,     /* pos condition code */
   bf_35track2       = 35,     /* track 2 data */
   bf_37rrn          = 37,     /* retrieval reference number */
   bf_38authcode     = 38,     /* auth code   */
   bf_39respcode     = 39,     /* response code*/
   bf_41termid       = 41,     /* terminal id  */
   bf_42cardacc      = 42,     /* card acceptor id */
   bf_45track1       = 45,     /* track 1 data   */
   bf_48adddata      = 48,     /* add data private */ 
   bf_52pinblock     = 52,     /* pin block    */
   bf_54addlamts     = 54,     /* additional amounts */
   bf_60priv60       = 60,     /* private field 60 */
   bf_61priv61       = 61,     /* private field 61 */
   bf_62priv62       = 62,     /* private field 62 */
   bf_63priv63       = 63      /* private field 63 */
   }; 

       
#define MAX_BITFIELD 64



/********************************************************/
/*  STRUCTURE FOR PRIVATE FIELD 63, TABLE 73			*/
/********************************************************/
typedef struct 
{
	char data_length       [3];
	char table_id          [3];
	char registration_flag [2];
	char account_selection [2];
	char card_data         [17];
	char challenge1        [5];
	char challenge2        [5];
	char cresponse         [3];
}Table73_Type;
  
/********************************************************/
/*  STRUCTURE FOR PRIVATE FIELD 63, TABLE 74			*/
/********************************************************/
typedef struct 
{
	char data_length       [3];
	char table_id          [3];
	char challenge1        [5];
	char challenge2        [5];
}Table74_Type;
  

/******************************************************/
/* DATA STRUCTURE FOR STORING DATA IN FINAL FORMAT... */
/******************************************************/

struct iso_msg_rec
   {
        unsigned char msgtype_bcd   [2];
        unsigned char bitmap_bin    [8];
        unsigned char pan_len       [1];
        unsigned char pan_bin       [10];
        unsigned char proccode      [3];
        unsigned char amount_bin    [6];
        unsigned char stan_bin      [3];
        unsigned char time_bin      [3];
        unsigned char date_bin      [2];
        unsigned char date_expd     [2];
        unsigned char date_settle   [2];
        unsigned char posentry      [2];
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
        unsigned char adddata_len   [2]; 
        unsigned char adddata       [16];
        unsigned char pin_block     [8];
        unsigned char addl_len      [2];
        unsigned char addlamts      [50];
        unsigned char priv60_len    [2];
        unsigned char priv60        [100];
        unsigned char priv61_len    [2];
        unsigned char priv61        [150];
        unsigned char priv62_len    [2];
        unsigned char priv62        [100];
        unsigned char priv63_len    [2];
        unsigned char priv63        [350];
   };

struct bitfield_data
{
   enum bitfields  bd_fieldid;                           /* id for this field */
   unsigned char   *bd_pdata;                            /* ptr to data to move */
   int             bd_fieldlen;                          /* # of bytes in iso8583 field */
   int             (*bd_BFCond)(enum bitfields, pCHAR); /* ptr to conditional function */
   void            (*bd_BFMove)(enum bitfields);         /* ptr to move function */
   BOOLEAN         (*bd_BFRsp)(enum bitfields);          /* ptr to response handler function */
};

struct totals_record
   {
   unsigned char captured_sales_count   [3];
   unsigned char captured_sales_amount  [12];
   unsigned char captured_refund_count  [3];
   unsigned char captured_refund_amount [12];
   unsigned char debit_sales_count      [3];
   unsigned char debit_sales_amount     [12];
   unsigned char debit_refund_count     [3];
   unsigned char debit_refund_amount    [12];
   };

/*******************************************************/
/* REQUEST BUILD BUFFER                                */
/*******************************************************/
#define               COMM_BUF_SZE  1500




/*******************************************************/
/*  TRANSACTION ENUMERATION                            */
/*  THIS WILL BE GROWING AND CHANGING!                 */
/*******************************************************/
enum trantypes {
   tt_sale,             /* sale */
   tt_check_dl,         /* check - driv lice verify */
   tt_debit,            /* debit */
   tt_offline,          /* offline txn */
   tt_refund,           /* refund */
   tt_settle,           /* settlement */
   tt_void,             /* void */
   tt_test,             /* test */
   tt_sale_response     /* Sale Response , Chipstipe Demo */
   };

enum tranopts1 {
   topt1_notip       = 1,      /* no tip amount */
   topt1_noacct      = 2,      /* no account required in proc code */
   topt1_nocapture   = 4,      /* no capture required */
   topt1_nosystime   = 8,      /* no system time required */
   topt1_credit      = 16,     /* credit type transaction */
   topt1_preadv      = 32,     /* send all advises prior */
   topt1_settle      = 64,     /* settle type transaction */
   topt1_postadv     = 128     /* send single advise after */
   };

enum tranopts2 {
   topt2_noprerev     = 1,      /* no reversal prior */
   topt2_crfl         = 2,      /* CRFL processing allowed */
   topt2_floor        = 4,      /* floor limit processing allowed */
   topt2_referral     = 8,      /* referral processing allowed */
   topt2_batpend      = 16,     /* set batch pending flag for this transaction */
   topt2_debit        = 32,     /* debit transaction */
   topt2_reversal     = 64,     /* send reversal if failure */
   topt2_org_proccode = 128     /* use msg's original proccode */
   };

struct trandef
   {
   BYTE            td_trantype;  /* tran type for this record */
   unsigned char   td_msgid[2];   /* iso 8583 message id       */
   unsigned char   td_proccode[3]; /* iso 8583 processing code  */
   enum tranopts1  td_opts1;   /* options */
   enum tranopts2  td_opts2;
   enum bitfields  td_bitfields[MAX_BITFIELD]; /* fields to put in message */
   };


/******************************  END OF FILE  ****************************************************/
/********************************************************/
/* BIT SETTING TABLE                                    */
/********************************************************/
struct isobit 
   {
   int byte_no;         /* byte number for this bit field */
   int bit_mask;        /* bit to set in this byte        */
   };

#endif

