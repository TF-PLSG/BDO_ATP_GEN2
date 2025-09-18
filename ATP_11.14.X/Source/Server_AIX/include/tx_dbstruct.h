
/******************************************************************************

     Module:               tx_dbstruct.h
  
     Title:                Transaction Controller Database Structures

     Description: 

     Application:          Equitable

     Authours:             Last modified by Sanjoy Dasgupta
   
     Revision History:     1.0
   
     $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\App_lib\tx_dbstruct.h  $
   

      Rev 1.17   Jan 02 2011 10:52:34   TF-Srikanth
   Added UCF03 table for PCI Changes
  
        Rev 1.16   Apr 01 2005 10:52:34   lmitchel
   4_4_1: updates to support countr/currency code limits, unlimited purchase card MCC ranges
   
      Rev 1.15   Mar 10 2005 10:22:22   lmitchel
   Added country_amt_limits structures
   
      Rev 1.14   Jun 18 2004 13:48:20   dirby
   Broke out the amount field in MCC_Limits into peso_amt_limit
   and usd_amt_limit.
   SCR 1380
   
      Rev 1.13   May 24 2004 16:33:38   dirby
   Added data structures for Negative Fraud and MCC Limits.
   SCRs 1373 and 1380
   
      Rev 1.12   Jul 11 2001 11:36:06   dirby
   Removed settle_file_prefix from the MCT01 structure.  This
   field is used in the new base product.  It is not a field in the
   Equitable database.
   
      Rev 1.11   Nov 29 2000 10:47:48   apinto
   Added/Removed fields for the new product...A.Pinto 11.29.00
   
      Rev 1.10   Apr 17 2000 11:12:40   iarustam
    
   
      Rev 1.8   Jan 18 2000 16:35:44   ddabberu
   removed c++ style comments
   
      Rev 1.7   Nov 17 1999 10:25:32   blee1
   Added CRF01.currency_name
   
      Rev 1.6   Mar 17 1999 14:40:22   iarustam
   Bug fixes
   
      Rev 1.5   Mar 08 1999 08:59:12   iarustam
   Added def01_gui_list
   
      Rev 1.4   Feb 19 1999 12:20:58   MSALEH
   increased reserved field in HST01 to 21
   
      Rev 1.3   Feb 17 1999 08:38:18   iarustam
   GUI changes
   
      Rev 1.2   Feb 12 1999 17:58:02   iarustam
   Added  ISO_MSG_LENGTH1                
   
      Rev 1.1   Feb 04 1999 17:57:22   sdasgupt
   Added structures
   
      Rev 1.0   01 Feb 1999 14:43:08   jhapitas
   Initial Revision
   
      Rev 1.0   Dec 04 1998 14:44:02   MSALEH
   initial release
******************************************************************************/

#ifndef TX_DBSTRUCT_H
#define TX_DBSTRUCT_H

#define ISO_MSG_LENGTH1                      0  

#include "basictyp.h"

#define VISA7 1

typedef struct
{
	BYTE auth_id [7] ;
} UAG01, *pUAG01 ;


/* AP FA, Equitable 01-14-99 */
/* New table "DEF01" for deferred processing */
typedef struct
{
   struct
   {
      BYTE  organization_id                        [5];
      BYTE  merchant_id                            [16];
      BYTE  term_length                            [4];
   }primary_key;

   BYTE  factor                                    [8];  /* AP SD 01-28-99 size change */
   BYTE  deferred_min_amount                       [11];
   BYTE  deferred_max_amount                       [11];
}DEF01, *pDEF01;
/*
// This is the table that comes from the PLUS network
// Beth's application will load this data from a file
// Added by SDG on 02/04/99
*/
  typedef struct
{
	struct 
	{
		BYTE length [3] ;
		BYTE lo [17] ;
	} primary_key ;

	BYTE high [17] ;
	BYTE pan_length[3] ;
	BYTE destination_name[11] ; 
	BYTE description[31] ;
} BIN02, *pBIN02 ; 

#ifdef VISA7
typedef struct
{
   struct
   {
      BYTE  merchant_id   [16];
      BYTE  card_nbr      [20];
      BYTE  category_cd   [5];
   }primary_key;

   BYTE  c1_block            [2];
   BYTE  c2_block            [2];
   BYTE  c3_block            [2];
   BYTE  c4_block            [2];
   BYTE	 c1_count			 [3];
   BYTE  c1_block_start_date [9];
   BYTE  c1_block_end_date   [9];
   BYTE	 c2_count			 [3];
   BYTE  c2_block_start_date [9];
   BYTE  c2_block_end_date   [9];
   BYTE	 c3_count			 [3];
   BYTE  c3_block_start_date [9];
   BYTE  c3_block_end_date   [9];
   BYTE	 c4_count			 [3];
   BYTE  c4_block_start_date [9];
   BYTE  c4_block_end_date   [9];
   BYTE  block_reset_date	 [9];
   BYTE  last_declined_rc    [3];
   BYTE  first_reattempt_date[9];
   BYTE  reattempts_date	 [200];

}MERCH_INT_PARAMS, *pMERCH_INT_PARAMS;
#endif

/* user control file */
typedef struct
{
   struct
   {
      BYTE  name                                   [51];
   }primary_key;

   BYTE  password                                  [51];
   BYTE  date_added                                [9];
   BYTE  group_id                                  [16];
   struct
   {
      BYTE  subsystem_name                         [31];
      BYTE  action                                 [6];
   }control[20];


   struct
   {
      BYTE  terminals                              [21];
   }login[10];

   /* Fields for Login Validation of BDO ATPGen2 */
	BYTE password_1									[51];
	BYTE password_2									[51];    
	BYTE password_3									[51];
	BYTE password_4									[51];

	BYTE accountstatus								[11];  
	BYTE password_modified_date						[9];
	BYTE user_info_1								[11];         
	BYTE user_info_2								[11];         
	BYTE user_info_3								[11];        
	BYTE user_info_4								[11];    
	BYTE user_info_5								[11];    
	BYTE user_info_6								[11];    
	BYTE user_info_7								[11];    
	BYTE ekey										[51];		 
	BYTE dekey										[51];
	BYTE buffer										[501];

	/* ramya */
	BYTE Login_WorkStationId						[31];
	BYTE Login_Status								[31];
	BYTE User_Status								[10];
	BYTE Days_Inactive                              [5];
	BYTE Login_Date                                 [20];
	BYTE User_Expiry_Date                           [11];
	BYTE Date_Of_Deactivation						[11];

	BYTE password_5									[51];
	BYTE password_6									[51];
	BYTE password_7									[51];
	BYTE password_8									[51];
	BYTE password_9									[51];
	BYTE password_10								[51];
	BYTE Logout_Date                                [20];

}UCF01, *pUCF01;


typedef struct
{
	BYTE self_validation_ac			        [4];
	BYTE authentication_result_code_ac      [4];
	BYTE authentication_attempt_ac          [4];
    BYTE authentication_attempt_verify_flag [2];
	BYTE csc_validation_flag				[2];
}AMEX3DS2, *pAMEX3DS2;

typedef struct
{
	BYTE av_verification_rc			        [3];
	BYTE authentication_attempt_rc          [3];
	BYTE cvn2_validation_flag				[2];
	BYTE av_validation_flag				    [2];
	BYTE cup_authentication_attempt_verify_flag	[2];
}CUP3DS2, *pCUP3DS2;

typedef struct
{
	BYTE self_validation_pass_cavv_rc [3];
	BYTE self_validation_cavv_rc [3];
	BYTE authentication_result_code_ac [4];
	BYTE authentication_result_code_cavv_rc [3];
	BYTE authentication_attmpt_ac [4];
	BYTE cvv2_validation_flag [2];
	BYTE authentication_attempt_verify_flag [2];
}DCI3DS2, *pDCI3DS2;


typedef struct
{
   struct
   {
      BYTE  group_id                               [16];
   }primary_key;

   BYTE date_added                                 [9];
   struct
   {
      BYTE  subsystem_name                         [31];
      BYTE  action                                 [6];
   }control[20];

   struct
   {
      BYTE  terminals                              [21];
   }login[10];

   BYTE buffer										[501];

}UCF02, *pUCF02;


/*Added UCF03 Table for PCI changes -TF Srikanth*/
typedef struct
{
	struct
	{
		  BYTE  name                               [21];
	}foreign_key;


	BYTE  password_1							   [16];
	BYTE  password_2							   [16];
	BYTE  password_3							   [16];
	BYTE  password_4							   [16];
	BYTE  accountstatus                            [9];
	BYTE  password_modified_date                   [9];
	BYTE  user_info_1 		          [11]; 
	BYTE  user_info_2 		          [11]; 
	BYTE  user_info_3 		          [11]; 
	BYTE  user_info_4 		          [11];
	BYTE  ekey 						  [13]; 
	BYTE  dekey 					  [25];	


}UCF03, *pUCF03;

typedef struct
{

  BYTE caf_profile_id                 [5];
  BYTE key3ds                         [121];
}KEY3DS01,*pKEY3DS01;

/*Added Db password structure for DB password changes in tf.ini file -TF Srikanth*/
typedef struct
{
	BYTE  core_password							   [50];
	BYTE  app_password							   [50];
	BYTE  core_user_id							   [13];
	BYTE  app_user_id							   [13];
	BYTE  core_old_password						   [50];
	BYTE  app_old_password						   [50];

}DbPasswords, *pDbPasswords;

/* positive card file */
typedef struct
{
   struct
   {
      BYTE  card_type                              [2];
      BYTE  card_nbr                               [20];
   }primary_key;

   BYTE  primary_acct_nbr                          [20];
   BYTE  name_dba                                  [151];
   BYTE  address1                                  [31];
   BYTE  address2                                  [31];
   BYTE  city                                      [21]; /* AP FA, Equitable 01-14-99 size change */
   BYTE  state                                     [4];  /* AP FA, Equitable 01-14-99 size change */
   BYTE  zip_code                                  [11];
   BYTE  home_phone                                [31];
   BYTE  work_phone                                [31];
   BYTE  dob                                       [9];
   BYTE  profession                                [21];
   BYTE  country                                   [21];
   BYTE  nbr_cards                                 [3];
   BYTE  status                                    [3];
   BYTE  sex                                       [2];  /* AP FA, Equitable 01-14-99 */
   BYTE  mother_maiden_name                        [31]; /* AP FA, Equitable 01-14-99 */
   BYTE  membership_id                             [10]; /* AP FA, Equitable 01-14-99 */
   BYTE  cardholder_id                             [20]; /* AP FA, Equitable 01-14-99 */
   BYTE  block_status                              [5];  /* AP FA, Equitable 01-14-99 */
   BYTE  specialty_code                            [3];  /* AP FA, Equitable 01-14-99 */
   BYTE  vip_acct_code                             [2];  /* AP FA, Equitable 01-14-99 */
}CCF01, *pCCF01;

typedef struct
{
   struct
   {
      BYTE  card_type                              [2];
      BYTE  card_nbr                               [20];
   }primary_key;

   BYTE  action_code                               [2];
   BYTE  crind_autocard                            [2];
   BYTE  odometer_required                         [2];
   BYTE  order_number_required                     [2];
   BYTE  signature_required                        [2];
   BYTE  driver_flag                               [2];
   BYTE  vehicle_id_flag                           [2];
   BYTE  fleet_id_flag                             [2];
   BYTE  driver_id_flag                            [2];
   BYTE  registration_number                       [11];
   BYTE  super                                     [2];
   BYTE  super_low_lead                            [2];
   BYTE  gold                                      [2];
   BYTE  autogas                                   [2];
   BYTE  unleaded                                  [2];
   BYTE  diesel                                    [2];
   BYTE  oil                                       [2];
   BYTE  service                                   [2];
   BYTE  accessories                               [2];
   BYTE  tires                                     [2];
   BYTE  batteries                                 [2];
   BYTE  repairs_maint                             [2];
   BYTE  shop                                      [2];
   BYTE  clean_and_go                              [2];
   BYTE  accident_and_damage                       [2];
   BYTE  meals                                     [2];
   BYTE  parking                                   [2];
   BYTE  last_odometer_reading                     [7];
   BYTE  value_limits                              [3];
   BYTE  litre_limits                              [3];
   BYTE  time_limits                               [3];

}FCF02, *pFCF02;



typedef struct
{
   struct
   {
      BYTE card_type                                 [2];
      BYTE card_nbr                                  [20];
   }primary_key;
  
BYTE caf_profile_id                                  [5];
BYTE status                                          [3];
BYTE date_added                                      [9];
BYTE maint_date                                      [9];
BYTE changed_by                                      [8];

}CCF02PH, *pCCF02H;



/* AP SD 01-28-1999 New table for merchant category code range checks */
/*typedef struct
{
   struct
   {
      BYTE  card_type                              [2];
      BYTE  card_nbr                               [20];
   }primary_key;

   struct
   {
      BYTE  mcc_lo_range                              [5];
      BYTE  mcc_hi_range                              [5];
   } range[20];

}CCF04, *pCCF04;
*/

/*new ccf04 table  - 20050321*/
typedef struct
{
   struct
   {
      BYTE  card_type                                  [2];
      BYTE  card_nbr                                   [20];
   }primary_key;

   BYTE  mcc_lo_range                                   [5];
   BYTE  mcc_hi_range                                   [5];

}CCF04, *pCCF04;



/* hot card file */
typedef struct
{
   struct
   {
      BYTE  card_type                              [3];
      BYTE  card_nbr                               [20];
   }primary_key;

   BYTE  date_added                                [9];
   BYTE  last_maint                                [9];
   BYTE  changed_by                                [21];
   BYTE  status                                    [3];

}HCF02, *pHCF02;

/* card activity file */
typedef struct
{
   struct
   {
      BYTE  retrieval_ref_num                      [13];
      BYTE  card_nbr                               [20];
   }primary_key;

   BYTE  processing_code                           [7];
   BYTE  primary_acct_nbr                          [20];
   BYTE  tran_amount                               [13];
   BYTE  response_code                             [3];
   BYTE  auth_number                               [7];
   BYTE  tran_date                                 [9];
   BYTE  tran_time                                 [7];

}CCF10, *pCCF10;

/* history change file */
typedef struct
{
   struct
   {
      BYTE  unique_id                              [18];
   }primary_key;

   BYTE  file_name                                 [10];
   BYTE  file_primary_key                          [51];
   BYTE  changed_by                                [21];
   BYTE  date_modified                             [9];
   BYTE  time_modified                             [7];
   BYTE  field_tag                                 [40];
   BYTE  previous_data                             [151];
   BYTE  current_data                              [151];
   BYTE  reserved                                  [21];

}HST01, *pHST01;

/* batch profile file */
typedef struct
{
   struct
   {
      BYTE  profile_id                             [16];
      BYTE  job_id                                 [16];
   }primary_key;

}BPF01, *pBPF01;

/*calendar table*/
typedef struct
{
   struct
   {
      BYTE  year                                    [5];
      BYTE month                                    [3];
   }primary_key;

   struct
   {
      BYTE  working_day                             [2];
      BYTE  profile_id                              [16];
   }day[31];

}CLF01, *pCLF01;

/*job table*/
typedef struct
{
   struct
   {
      BYTE  job_id                                  [16];
   }primary_key;

   BYTE  time_to_run                                [21];
   BYTE  status                                     [2];
   BYTE  last_run                                   [31];
   BYTE  app_name                                   [16];

}JBF01, *pJBF01;

/*application table*/
typedef struct
{
   struct
   {
      BYTE  app_name                                 [16];
   }primary_key;

   BYTE  exe_name                                    [16];
   BYTE  path                                        [100];
   BYTE  parameters                                  [100];

}APF01, *pAPF01;

/* atp configuration file */
typedef struct
{
   BYTE  site_name                                 [31];
   BYTE  tlf_log_date                              [9];
   struct
   {
      BYTE  subsystem_name                         [31];
   }system_components[20];
   
}ATP01, *pATP01;

typedef struct
{
   struct
   {
      BYTE  workstation_id                         [21];
   }primary_key;

   BYTE  workstation_status                        [3];
   
}ATP02, *pATP02;

typedef struct
{
   struct
   {
      BYTE  card_type                              [5];
      BYTE  merchant_id                            [16]; 
      BYTE  organization_id                        [5];
   }primary_key;

   BYTE  acquirer_id                               [7];
   BYTE  corporate_id                              [7];
   BYTE  settlement_id                             [17]; /* AP FA, Equitable 01-14-99 size change */
   BYTE  floor_limit                               [13];

}MCT01, *pMCT01;

typedef struct
{
   struct
   {
      BYTE  card_type                              [5];
   }primary_key;

   BYTE  description                               [31];

}CTF01, *pCTF01;

typedef struct
{
   struct
   {
      BYTE  primary_acct_nbr                       [20];
   }primary_key;

   BYTE  name_dba                                  [151];
   BYTE  address1                                  [31];
   BYTE  address2                                  [31];
   BYTE  city                                      [21];
   BYTE  state                                     [4];  /* AP FA, Equitable 01-14-99 size change */
   BYTE  zip_code                                  [11];
   BYTE  home_phone                                [31];
   BYTE  work_phone                                [31];
   BYTE  status                                    [3];
   BYTE  caf_profile_id                            [5];
   BYTE  type					                        [2];
   BYTE  corp_acct_nbr                             [11]; /* AP FA, Equitable 01-14-99 */
   BYTE  block_status                              [5];  /* AP FA, Equitable 01-14-99 */
   BYTE  account_name                              [31]; /* AP FA, Equitable 01-14-99 */
   BYTE  past_due_amount                           [13]; /* AP FA, Equitable 01-28-99 */
   BYTE  min_pay_code                              [2];  /* AP FA, Equitable 01-28-99 */
   CHAR  Credit_limit  					   		   [11];  /* Added for FALCON*/
   CHAR  Custr_nbr     					   		   [13];
   CHAR  APP_Source     					  	   [11];  
   CHAR  COMP_Name      						   [101];

}PAF01, *pPAF01;

typedef struct
{
   struct
   {
      BYTE  currency_code                          [4];
   }primary_key;

   BYTE  conversion_rate                           [10];
   BYTE  currency_name                             [11];
   
}CRF01, *pCRF01;

typedef struct
{
   struct
   {
      BYTE  status                                 [3];
   }primary_key;

   BYTE  description                               [15];
   BYTE  iso_resp_code                             [3];
   BYTE  resp_text_msg                             [21];
   BYTE  mac_value								   [3];

}CSF01, *pCSF01;


typedef struct
{
   struct
   {
      BYTE  mcc               [6];
      BYTE  bin_low           [17];
      BYTE  bin_high          [17];
      INT   pan_length        ;
   }primary_key;

   INT   bin_length        ;
   BYTE  country_code      [4];

}NEGATIVE_FRAUD, *pNEGATIVE_FRAUD;


typedef struct
{
   struct
   {
      BYTE  caf_profile_id   [5];
      BYTE  type             [2];
      BYTE  mcc              [6];
   }primary_key;

   BYTE  usage               [5];
   BYTE  peso_amt_limit      [13];
   BYTE  usd_amt_limit       [13];
}MCC_LIMITS, *pMCC_LIMITS;


typedef struct
{
   BYTE  num_returned     [5];
   MCC_LIMITS mcc_limits  [SELECT_MCC_LIST_SIZE];
}MCC_LIMITS_GUI, *pMCC_LIMITS_GUI;


typedef struct
{
   struct
   {
      BYTE  caf_profile_id   [5];
      BYTE  type             [2];
   }primary_key;

   BYTE  country_code        [4];
   BYTE  amount_limit        [13];
}COUNTRY_AMT_LIMITS, *pCOUNTRY_AMT_LIMITS;


typedef struct
{
   BYTE				  num_returned     [5];
   COUNTRY_AMT_LIMITS   country_amt_limits [SELECT_LIST_SIZE];
}COUNTRY_AMT_LIMITS_GUI, *pCOUNTRY_AMT_LIMITS_GUI;


typedef struct
{
   BYTE  num_returned     [5];
   CCF10 ccf10_record     [GUI_MAX_LIST_SIZE];
}CCF10_GUI_LIST, *pCCF10_GUI_LIST;


typedef struct
{
   BYTE  num_returned     [5];
   MCT01 merch_card       [GUI_MAX_LIST_SIZE];
}MCT01_GUI_LIST, *pMCT01_GUI_LIST;

typedef struct
{
   BYTE  num_returned     [5];
   BYTE  card_type        [SERVICE_LIST_SIZE][5];
}CTF01_LIST, *pCTF01_LIST;

typedef struct
{
   BYTE  num_returned     [5];
   BYTE  card_type        [GUI_MAX_LIST_SIZE][5];
}CTF01_GUI_LIST, *pCTF01_GUI_LIST;

typedef struct
{
   BYTE  num_returned     [5];
   CCF04 ccf04_list       [GUI_MAX_LIST_SIZE];
}CCF04_GUI_LIST, *pCCF04_GUI_LIST;

typedef struct
{
   BYTE  num_returned     [5];
   DEF01 def_list       [GUI_MAX_LIST_SIZE];
}DEF01_GUI_LIST, *pDEF01_GUI_LIST;


typedef struct
{
   BYTE  num_returned             [5];
   struct
   {
      BYTE  primary_acct_nbr      [20];
      BYTE  name_dba              [31];
   }paf_info                        [GUI_MAX_LIST_SIZE];

}PAF01_GUI_LIST, *pPAF01_GUI_LIST;

typedef struct
{
   BYTE  num_returned     [5];
   BYTE  currency_code    [GUI_MAX_LIST_SIZE][4];
}CRF01_GUI_LIST, *pCRF01_GUI_LIST;

typedef struct
{
	BYTE auth_result_code_value[3];
	BYTE response_code [3];
	BYTE validation_flag      [2];

}JCB3DS2,*pJCB3DS2;

typedef struct
{
   BYTE  num_returned             [5];
   JCB3DS2 jcb3ds2_record             [GUI_MAX_LIST_SIZE];

} JCB3DS2_GUI_LIST, *pJCB3DS2_GUI_LIST;
typedef struct
{
   BYTE  num_returned           [5];
   CSF01 csf01_list             [SERVICE_LIST_SIZE];
}CSF01_LIST, *pCSF01_LIST;

typedef struct
{
   BYTE  num_returned     [5];
   CRF01 crf01_list       [SERVICE_LIST_SIZE];
}CRF01_LIST, *pCRF01_LIST;

typedef struct
{
	BYTE currency_value					[4];
	BYTE currency_code					[4];
	BYTE currency_decimal_places		[2];
	BYTE currency_name					[15];
	BYTE future_field1					[20];
	BYTE future_field2					[30];
	BYTE future_field3					[50];

}CCL01, *pCCL01;

typedef struct
{
   BYTE  num_returned             [5];
   CCL01 ccl01_record             [CURR_CODE_LIST_SIZE];

} CCL01_GUI_LIST, *pCCL01_GUI_LIST;

typedef struct
{
	char card_brand					[11];
	char bdk_key					[65];
	char future_field1				[33];
	char future_field2				[33];
	char future_field3				[37];

}BDKKEY01, *pBDKKEY01;

typedef struct
{
   BYTE  num_returned             [5];
   BDKKEY01 bdkkey01_record       [BDK_KEY_LIST_SIZE];

} BDKKEY01_GUI_LIST, *pBDKKEY01_GUI_LIST;

typedef struct
{
	struct
	{
		char Caf_Profile_id			[5];
		char Type					[2];
	}primary_key;

	char Cvk_Mag_Keyblock		[129];
	char Cvk_Acq_3DS_Keyblock	[129];
	char Cvk_Pin_Verification	[129];
	char Cvk_Mkac_Keyblock		[129];
	char Csck_Iss_3ds_Keyblock	[129];
	char Csck_Mag_Keyblock		[129];
	char Dci_3ds_Keyblock		[129];
	char Cvk_3ds_Cak_Keyblock	[129];
	char Future_Field1			[129];
	char Future_Field2			[129];
	char Future_Field3			[129];
	char Future_Field4			[129];
	char Future_Field5			[129];

}CAF01_KEYBLOCK, *pCAF01_KEYBLOCK;


typedef struct
{
	BYTE  				num_returned			[5];
	   struct
	   {
		  BYTE  caf_profile_id   [5];
		  BYTE  type			 [2];
	   }caf01_keyblock_record	[GUI_MAX_LIST_SIZE];

}CAF01_KEYBLOCK_GUI_LIST, *pCAF01_KEYBLOCK_GUI_LIST;

typedef struct
{
   BYTE  delete_flag;
   CSF01 csf01_record;
}CSF01_MEM, *pCSF01_MEM;

typedef struct
{
   BYTE  num_returned             [5];
   CSF01 csf01_record             [GUI_MAX_LIST_SIZE];
}CSF01_GUI_LIST, *pCSF01_GUI_LIST;

typedef struct
{
   BYTE  delete_flag;
   CRF01 crf01_record;
}CRF01_MEM, *pCRF01_MEM;




typedef struct
{
   BYTE net_consec_tmouts_ctr    [4];
   BYTE active_txns_ctr          [4];
   BYTE current_state            [2];
}CARD_MEM, *pCARD_MEM;


typedef struct
{
   BYTE  num_returned     [5];
   BYTE  profile_id       [GUI_MAX_LIST_SIZE][16];
}BPF01_GUI_LIST, *pBPF01_GUI_LIST;

typedef struct
{
   BYTE  num_returned     [5];
   BYTE  app_name         [GUI_MAX_LIST_SIZE][16];
}APF01_GUI_LIST, *pAPF01_GUI_LIST;

typedef struct
{
   BYTE  num_returned     [5];
   BYTE  job_id           [GUI_MAX_LIST_SIZE][16];
}JBF01_GUI_LIST, *pJBF01_GUI_LIST;


typedef struct
{
   BYTE  num_returned     [5];
   BPF01 batch_profile    [SCH_MAX_LIST_SIZE];
}BPF01_SCH_LIST, *pBPF01_SCH_LIST;


typedef struct
{
   BYTE  num_returned     [5];
   BYTE  name             [GUI_MAX_LIST_SIZE][21];
}UCF01_LIST, *pUCF01_LIST;


typedef struct
{
   BYTE  num_returned     [5];
   BYTE  group_id         [GUI_MAX_LIST_SIZE][16];
}UCF02_LIST, *pUCF02_LIST;

typedef struct				/*Added UCF03 table for PCI changes -TF Srikanth*/										
{
   BYTE  num_returned     [5];
   BYTE  name             [GUI_MAX_LIST_SIZE][21];
}UCF03_LIST, *pUCF03_LIST;

typedef struct
{
   BYTE  num_returned     [5];
   HST01 history          [GUI_MAX_LIST_SIZE];
}HST01_LIST, *pHST01_LIST;


typedef struct
{
   BYTE  num_returned     [5];
   ATP02 atp_list         [GUI_MAX_LIST_SIZE];
}ATP02_GUI_LIST, *pATP02_GUI_LIST;

typedef struct
{
   BYTE  num_returned     [5];
   ATP02 atp_list         [SERVICE_LIST_SIZE];
}ATP02_LIST, *pATP02_LIST;

/*Added Db password structure for DB password changes in tf.ini file -TF Srikanth*/
typedef struct
{
	BYTE  arch_password							   [50];
	BYTE  arch_user_id							   [13];
	BYTE  arch_old_password						   [50];

}ArchDbPasswords, *pArchDbPasswords;

typedef struct
{
	BYTE  mra_password							   [50];
	BYTE  mra_user_id							   [13];
	BYTE  mra_old_password						   [50];

}MraDbPasswords, *pMraDbPasswords;

typedef struct
{
	BYTE  days_inactive							  		    [5];
	BYTE max_prev_passwords_not_allowed                     [3];
	BYTE max_password_retries                      			[2];
	BYTE min_password_length                       			[3];
	BYTE days_before_expiration					   			[5];
	BYTE user_status_cntrl_tmeout_val						[5];
}GPCF01, *pGPCF01;

typedef struct
{
	BYTE pan_length								[4];
	BYTE bin_low								[18];
	BYTE card_type								[3];
	BYTE bin_high								[18];
	BYTE bin_length								[4];
	struct
	{
	    BYTE merchant_id     					[16];
	}merchant_blocking[50];
}MIDBINBLK01, *pMIDBINBLK01;
#endif
