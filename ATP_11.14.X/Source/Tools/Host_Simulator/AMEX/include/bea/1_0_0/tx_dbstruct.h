
/******************************************************************************

     Module:               tx_dbstruct.h
  
     Title:                Transaction Controller Database Structures

     Description: 

     Application:          Equitable

     Authours:             Last modified by Sanjoy Dasgupta
   
     Revision History:     1.0
   
     $Log:   N:\PVCS\PTE\bea\headers\tx_dbstruct.h  $
   
      Rev 1.2   Sep 24 1999 10:06:02   svelaga
   Updated MCT01 structure: 
   Added settle_file_prefix to MCT01 structure.
   
      Rev 1.1   Sep 23 1999 11:33:50   iarustam
   added currency_name to crf01
   
      Rev 1.0   08 Sep 1999 10:54:08   epriyev
   initial BEA load
   
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

#define ISO_MSG_LENGTH1                      0  //IA Equitable 02-12-99

#include "basictyp.h"

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

// This is the table that comes from the PLUS network
// Beth's application will load this data from a file
// Added by SDG on 02/04/99
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




/* user control file */
typedef struct
{
   struct
   {
      BYTE  name                                   [21];
   }primary_key;

   BYTE  password                                  [9];
   BYTE  date_added                                [9];
   BYTE  group_id                                  [16];
   struct
   {
      BYTE  subsystem_name                         [31];
      BYTE  action                                 [6];
   }control[15];


   struct
   {
      BYTE  terminals                              [21];
   }login[10];

}UCF01, *pUCF01;

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
   }control[15];

   struct
   {
      BYTE  terminals                              [21];
   }login[10];

}UCF02, *pUCF02;





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
typedef struct
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
   } range[9];

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
   BYTE  previous_data                             [51];
   BYTE  current_data                              [51];
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
   }system_components[15];
   
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
   //Added for BEA : 09/24/99
   BYTE  settle_file_prefix                        [7];


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

   BYTE  name_dba                                  [31];
   BYTE  address1                                  [31];
   BYTE  address2                                  [31];
   BYTE  city                                      [21];
   BYTE  state                                     [4];  /* AP FA, Equitable 01-14-99 size change */
   BYTE  zip_code                                  [11];
   BYTE  home_phone                                [18];
   BYTE  work_phone                                [18];
   BYTE  status                                    [3];
   BYTE  caf_profile_id                            [5];
   BYTE  type					                        [2];
   BYTE  corp_acct_nbr                             [11]; /* AP FA, Equitable 01-14-99 */
   BYTE  block_status                              [5];  /* AP FA, Equitable 01-14-99 */
   BYTE  account_name                              [31]; /* AP FA, Equitable 01-14-99 */
   BYTE  past_due_amount                           [13]; /* AP FA, Equitable 01-28-99 */
   BYTE  min_pay_code                              [2];  /* AP FA, Equitable 01-28-99 */

}PAF01, *pPAF01;

typedef struct
{
   struct
   {
      BYTE  currency_code                          [4];
   }primary_key;

   BYTE  conversion_rate                           [10];
   BYTE  currency_name                             [11]; /* IA for BEA 9/22/99 */
   
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

}CSF01, *pCSF01;





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

#endif
