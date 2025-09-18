/**************************************************************************************************
*  
* MODULE:      TxDataServer.h
*  
* TITLE:       ATP Standard 
*  
* DESCRIPTION: This includes all function prototypes for txdataserver.c 
*
* APPLICATION: TXCNTL
*
* AUTHOR:  Sanjoy Dasgupta
*
* REVISION HISTORY:
*
*
* $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\txcntl\txdataserver.h  $

      Rev 1.18   Jun 01 2007  DIrby
   Added function prototypes to support storing auth numbers in shared memory.
   SCR 12299

      Rev 1.17   Feb 09 2007 13:52:02   DIrby
   Added function prototype of a function to get a UAG01 record on
   the interactive queue (with wait).
   SCR 22836
   
      Rev 1.16   Oct 27 2005 16:25:40   dirby
   Fixed FraudGuard related issues:
   1. If txn is rejected by ATP, still need to send txn to FG.
   2. Do not send to FG if Host is unavailable (offline).
   3. Always send Acquirer Id (DE32), even on declines by ATP.
   4. Added capability to turn FG ON/OFF by card family.
   SCR 17661
   
      Rev 1.15   Apr 13 2005 13:55:08   lmitchel
   Correct arguments to get_acf01_details
   
      Rev 1.14   Apr 01 2005 10:10:28   lmitchel
   Initial Checking prior to integration testing for updates made to support country/curreny code limits (scr13929), unlimited purchase card MCC ranges, and purchase card transaction limits(scr12785).
   
      Rev 1.13   Sep 23 2004 11:17:36   lmitchel
   SCR12497 : Enhancement to route ATM transactions with JCB bin ranges to Cirrus
   
      Rev 1.12   Jun 04 2004 17:26:08   dirby
   Added function prototypes for Negative Fraud and MCC Amount Limits.
   SCRs 1373 and 1380
   
      Rev 1.11   Sep 13 2002 17:35:10   dirby
   Removed function prototype for get_BON01_details.
   SCR 804
   
      Rev 1.10   Apr 17 2000 09:40:20   iarustam
   added bin_blocking and offus velocity functionality
   
      Rev 1.9   Dec 30 1999 13:32:56   ddabberu
   aix_build
   
      Rev 1.8   Dec 17 1999 19:18:38   iarustam
   fixed offline voids
   
      Rev 1.6   Jun 01 1999 17:14:16   sdasgupt
   1. Changed logic such that messages are written to auth_Tx.voice_auth_text only for Voice Auth transactions
   
   
      Rev 1.5   May 13 1999 14:53:16   sdasgupt
   Fixed bug where the shared memory did not contain the updated value of CAF01 (cvv_processing)
   Checking is done for mechant and bin-type for both on-us and off-us transaction
   
      Rev 1.4   Apr 01 1999 11:20:42   sdasgupt
   changes to allow reversals for VOICE
   transactions
   
   GB
   
      Rev 1.3   Feb 23 1999 11:11:36   sdasgupt
    
   
      Rev 1.2   Feb 17 1999 18:13:24   sdasgupt
   Program Changes
   
      Rev 1.1   Feb 16 1999 17:50:34   sdasgupt
   TxCntl Changes
   
      Rev 1.0   Feb 12 1999 18:54:34   sdasgupt
   Initial version
*******************************************************************************/
#ifndef _TXDATASERVER
#define _TXDATASERVER

#include "txcntl.h"

#define PADDED_ZEROES "000000000"
#define JCB_PADDED_ZEROES "000000000"

BOOLEAN TxDataServer_Get_DCF01_Details (void) ;
void TxDataServer_Verify_Self_Validation(char * Supplemental_output);
int calculate_julian_date(int mm,int dd, int year);
BOOLEAN TxDataServer_Get_MCF01_Details (BYTE* organization_id, BYTE* merchant_id) ;
BOOLEAN TxDataServer_Get_MCT01_Details () ;
BYTE    TxDataServer_Get_CAF01_Details (BYTE read_mode, BYTE* caf_profile_id, BYTE* type, BYTE MessageSubType2) ;
BOOLEAN TxDataServer_Get_FAF01_Details (BYTE* caf_profile_id) ;
BYTE    TxDataServer_Get_CCF02_Details (void) ;
BYTE TxDataServer_Get_CCF02_Details_For_RT (void);
BOOLEAN TxDataServer_Get_CCF03_Details (void) ;
BOOLEAN TxDataServer_Get_Offus_Velocity_Details (void) ;
BOOLEAN TxDataServer_Get_ACF01_Details (BYTE* type, BYTE* nbr, BYTE MessageSubType2) ;
BOOLEAN TxDataServer_Get_ACF01_Details_For_RT (void);
BOOLEAN TxDataServer_Get_PAF01_Details (void) ;
BYTE    TxDataServer_Get_BIN01_Details (BYTE read_mode) ;
BOOLEAN TxDataServer_Get_CRF01_Details (BYTE* merchant_currency_code) ;
BOOLEAN TxDataServer_Get_CSF01_Details (BYTE* status) ;
BOOLEAN TxDataServer_Get_DEF01_Details (void) ;
BOOLEAN TxDataServer_Get_Cntry_Limits_Details(BYTE MessageSubType2);
BYTE TxDataServer_STIP_Currency_Conversion_When_Acct_Not_Found(BYTE MessageSubType2);
INT TxDataServer_perform_currency_conversion( DWORD  amt_in,  pBYTE cc_in, pBYTE cc_out,
                                 pDWORD amt_out, pCHAR err_buf );
BYTE TxDataServer_Continue_With_CCF02P_Processing (BYTE MessageSubType2) ;

BYTE    TxDataServer_Process_Db_Reply (pPTE_MSG) ;

BYTE TxDataServer_Process_CAF_Found (BYTE caf_context) ;
BYTE TxDataServer_Process_CAF_Not_Found (BYTE caf_context) ;
BYTE TxDataServer_Retrieve_CAF_Profile (BYTE* caf_profile_id,
													 BYTE* type,
													 BYTE caf_context) ;
BYTE TxDataServer_Continue_With_Authorizing_Transaction (void) ;
BYTE TxDataServer_Continue_With_ACF01_Processing (BYTE MessageSubType2) ;
BYTE TxDataServer_Get_Bin_Level_Profile (void) ;
BYTE  TxDataServer_Continue_With_Transaction_Processing (void) ;
BYTE TxDataServer_Continue_With_Pin_Processing (void) ;
BOOLEAN TxDataServer_Get_UAG01_Details (void) ;
BOOLEAN TxDataServer_Get_CCF04_Details (void) ;
BYTE TxDataServer_Validate_Category_Code_Range (CCF04 sCCF04) ;
BYTE TxDataServer_Get_BIN02_Details (void) ;
BOOLEAN TxDataServer_Get_NEG01_Details (void) ;
BYTE TxDataServer_Validate_Profile_For_Floor_Limit (void) ;
BYTE TxDataServer_Validate_Tran_Limit () ;
BYTE TxDataServer_Continue_With_Local_Processing (void) ;
BOOLEAN TxDataServer_Get_Neg_Fraud_Details( void );
BOOLEAN TxDataServer_Get_MCC_Limit_Details( BYTE subtype2 );
INT     TxDataServer_Get_Ncf01_With_Wait( pNCF01 pncf01, pCHAR error_msg );
INT     TxDataServer_initAuthNumber( void );
BOOLEAN TxDataServer_Update_UAG01( pCHAR );
INT     TxDataServer_Get_Uag01_With_Wait( pUAG01 puag01, BYTE subtype2, pCHAR error_msg );
BYTE    TxDataServer_Send_Interactive_DB_Request( pPTE_MSG  p_msg_out,
                                                  pPTE_MSG *p_msg_in,
                                                  pCHAR     que_name,
                                                  pCHAR     error_msg );
void TxDataServer_DBReply_Continue( void );
BOOLEAN Txdataserver_Verify_AVN_Value();

#ifdef MC07_OCT2023
void TxDataServer_separateNames(const char *full_name, char *first_name, char *last_name);
#endif

INT TxDataServer_Check_Exception_Card_Num(CR01  sCR01);
BYTE TxDataServer_Perform_Single_amount_limit (CR01 sCR01);
void Txcntl_Get_Falcon_RT_ConfigureLink_From_DB_For_All_Brands();
void Txcntl_Get_Falcon_RT_ConfigureLink_From_DB_For_AMEX();
void Txcntl_Get_Falcon_RT_ConfigureLink_From_DB_For_CUP();
void Txcntl_Get_Falcon_RT_ConfigureLink_From_DB_For_DCI();
void Txcntl_Get_Falcon_RT_ConfigureLink_From_DB_For_JCB();
void Txcntl_Get_Falcon_RT_ConfigureLink_From_DB_For_MC();
void Txcntl_Get_Falcon_RT_ConfigureLink_From_DB_For_VISA();
void Txcntl_Get_Falcon_RT_ConfigureLink_From_DB_For_IFTR();
void Txcntl_Get_Falcon_RT_ConfigureRule_From_DB_For_All_Brands();
void Txcntl_Get_Falcon_RT_ConfigureRule_From_DB_For_AMEX();
void Txcntl_Get_Falcon_RT_ConfigureRule_From_DB_For_CUP();
void Txcntl_Get_Falcon_RT_ConfigureRule_From_DB_For_DCI();
void Txcntl_Get_Falcon_RT_ConfigureRule_From_DB_For_JCB();
void Txcntl_Get_Falcon_RT_ConfigureRule_From_DB_For_MC();
void Txcntl_Get_Falcon_RT_ConfigureRule_From_DB_For_VISA();
void Txcntl_Get_Falcon_RT_ConfigureRule_From_DB_For_PL();

#endif
