/**************************************************************************************************
*  
* MODULE:      TXCNTL.H
*  
* TITLE:       ATP Standard - Terminal Handler Main.
*  
* DESCRIPTION: Contains function prototypes for functions in TXCNTL.C
*
   Initial version
*******************************************************************************/
#ifndef _TXCNTL
#define _TXCNTL
#include "app_info.h"
#include "txservice.h"
#include "equitdb.h"
#include "tx_dbstruct.h"
#include "dc_dbstruct.h"
#include "nc_dbstruct.h"
#include "ptetime.h"
#include "txauthorize.h"

#define FALCON_CONTROLLER 1
#define VISA7 	     1
#define MC12_OCT2023 1
#define MC13_OCT2023 1
#define MC05_OCT2023 1
#define MC03_OCT2023 1
#define MC07_OCT2023 1
#define AMX1_APR2024 1
#define BNDL1_APR2024 1
#define MC03_APRIL2024 1

#ifdef	_DEBUG
	#define PRINT(x) printf(x) 
#else	
	#define PRINT
#endif 

#define  strEqual(x,y)  (!strcmp(x,y))




void EndProcess(void) ;
void MainProcessor(void) ;
void PinnacleMsgHandler(pPTE_MSG) ;
INT  get_visa_debit_routing( void );
void Get_MCC_Allowable_values_From_Ini(void);
void Get_MCC_Allowable_values_From_DB(void);
void get_keyblock_flag_details( void );
void set_keyblock_flag_details( pCHAR );

#ifdef MC07_OCT2023
void Get_Name_Validation_Parameters_FromTF_INI();
#endif

#ifdef MC12_OCT2023
#define MC_CATEGORY_CODE "1234"
BYTE cat1_excessive_declines_validation_flag=0;
BYTE cat2_excessive_declines_validation_flag=0;
BYTE excessive_declines_flag=0;
#endif

#ifdef MC13_OCT2023
#define MAC_CATEGORY_CODE "1111"
BYTE mac_cat1_validation_flag = 0;
BYTE mac_cat2_validation_flag = 0;
BYTE mac_pos_entry_mode_cat1[5] = {0};
BYTE mac_pos_cond_code_cat1[3]  = {0};
BYTE mac_pos_entry_mode_cat2[5] = {0};
BYTE mac_pos_cond_code_cat2[3] 	= {0};
#endif

#ifdef VISA7
#define VISA_CATEGORY_CODE "1234"
#define  MERCHANT_INTEGRITY_INI_FILE   "merchant_integrity.ini"
BYTE cat1_MID_integrity_validation_flag=0;
BYTE cat2_MID_integrity_validation_flag=0;
BYTE cat3_MID_integrity_validation_flag=0;
BYTE cat4_MID_integrity_validation_flag=0;
#endif


#endif



