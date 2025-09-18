/*******************************************************************************
* Copyright (c) 2002, Hypercom, Inc.
* All Rights Reserved.
*  
* MODULE:       visautil.h
*  
* TITLE:       
*  
*	DESCRIPTION: This module contains all the function prototypes for visautil.c
*
* APPLICATION: 
*
* AUTHOR:       B. Gable
*
* REVISION HISTORY:
*
* $Log:   N:\PVCS\PTE\Equitable\App_lib\vsmsutil.h  $
   
      Rev 1.2   Jul 23 2003 08:47:56   mbysani
   update as per visa sms spec
   
      Rev 1.1   Jul 17 2003 17:22:10   mbysani
   Updated for visa sms
   
      Rev 1.16   Jun 18 2002 00:05:58   dirby
   Change validation check of DE38, approval number, to allow spaces.
   
      Rev 1.15   May 30 2002 14:47:08   dirby
   Added function prototype 'get_balances'.
   SCR 750
   
      Rev 1.14   May 23 2002 16:30:56   dirby
   Added function prototype Field126Parser.
   SCR 743
   
      Rev 1.13   Jan 24 2000 10:36:20   ddabberu
   new echo_logic 
   
      Rev 1.12   Aug 25 1999 17:07:10   egable
   Change function definitions.
   
      Rev 1.11   Aug 12 1999 16:53:08   egable
   Add function definition.
   
      Rev 1.10   Jul 13 1999 16:02:20   egable
   Add function prototype.
   
      Rev 1.9   Jun 21 1999 16:03:40   egable
   Add prototype for ascii_to_bin
   
      Rev 1.8   Jun 16 1999 16:21:20   egable
   Add prototype for trim_spaces.
   
      Rev 1.7   May 27 1999 13:32:02   egable
   Add function definitions to read and write
   shared memory, and to fill the response text.
   
      Rev 1.6   May 25 1999 16:11:56   egable
   Put the previous source station flag into
   shared memory to allow alternating source
   station ids.
   
   
      Rev 1.5   Mar 26 1999 15:26:28   egable
   Add new function definition.
   
      Rev 1.4   Mar 17 1999 17:27:08   egable
   Add currency codes
   
      Rev 1.3   Mar 09 1999 14:53:00   egable
   Add new function definition.
   
      Rev 1.2   Feb 02 1999 15:59:40   egable
   Changes to message flow, headers and
   'if' statements.  This is the basis for JCB.
   
   
      Rev 1.1   Jan 11 1999 09:56:54   egable
   Initial changes for equitable, added  issuing
   paths and several message types.
   
      Rev 1.0   17 Dec 1998 16:25:20   jhapitas
   Initial Equitable Revison
   
      Rev 1.0   Dec 04 1998 14:56:36   MSALEH
   initial release
*
*******************************************************************/

#ifndef VISAUTIL_H
#define VISAUTIL_H

#include "app_info.h"
#include "equitdb.h"
#include "nc_dbstruct.h"
#include "vsmsmain.h"


void    Rj_with_lead_zeros( pCHAR str, INT FieldWidth );
INT     hhutil_send_net_mngmnt_response(pAUTH_TX p_auth_tx);
INT     hhutil_send_net_mngmnt_request(pAUTH_TX p_auth_tx);
void    convert_from_julian_day( pCHAR julian_str, pCHAR date_time );
BOOLEAN Build_Host_Request ( pAUTH_TX p_auth_tx);
INT     hhutil_parse_host_response_msg(pAUTH_TX p_auth_tx);
INT     move_iso_response_to_auth_struct(CHAR nmi_code[], pAUTH_TX p_auth_tx);
void    build_host_structure( pAUTH_TX p_auth_tx );
void    copy_to_req_resp_buffer (pCHAR indata, INT  msglen   );
BYTE    get_tran_type_2(BYTE);
BYTE    get_tran_type( pAUTH_TX p_auth_tx ) ;
void    TestResponses(BYTE ReqRsp_Buffer[COMM_BUF_SZE]);
void    Build_Host_Header (pAUTH_TX p_auth_tx);
BOOLEAN blank_string(pCHAR astr, INT max_len);
void    hhutil_get_strtime ( pCHAR deststr );
BYTE    hhutil_process_host_request(pAUTH_TX p_auth_tx);
INT     incoming_other_host_response(pAUTH_TX p_auth_tx);
BOOLEAN hhutil_verify_pin_block(pAUTH_TX p_auth_tx);
BOOLEAN hhutil_process_debit_transaction(pAUTH_TX p_auth_tx);
BOOLEAN hhutil_send_host_response(pAUTH_TX p_auth_tx);
void    hhutil_send_retry_response_to_host(pCHAR code, pAUTH_TX p_auth_tx);
void    hhutil_parse_db_ok(pPTE_MSG p_msg_in);
void    hhutil_parse_db_error(pPTE_MSG p_msg_in);
BYTE    hhutil_get_ncf01_cfg( pNCF01 pncf01, pCHAR error_msg );
BOOLEAN blank_or_null_string(pCHAR astr, INT max_len);
XINT    ReadVisaTable(pCHAR TableName, pBYTE tmouts, pBYTE txns, pBYTE state, pBYTE prev_src);
LONG    WriteVisaTable(pCHAR TableName, pBYTE tmouts, pBYTE txns, pBYTE state, pBYTE prev_src);
VOID    hhutil_get_response_text( pAUTH_TX p_auth_tx);
void    trim_spaces( pBYTE pString );
void    ascii_to_bin( CHAR *astr, INT ascii_len, BYTE *binary_string, INT blen);
INT     verify_auth_tx_data(pAUTH_TX p_auth_tx);
INT     incoming_other_host_response_db_reply( pAUTH_TX p_auth_tx);
BOOLEAN hhutil_transfer_ctl_to_auth_app ( pAUTH_TX p_auth_tx);
INT     insert_tlf01(pAUTH_TX p_auth_tx);
void    update_ncf01( pAUTH_TX p_auth_tx );
void    Field126Parser( pBYTE outstring );
void    get_balances( pAddl_Amts p_acct_bal, pAUTH_TX p_auth_tx );
INT     isalphanum_space(pCHAR string);
BOOLEAN send_msg( pPTE_MSG p_msg, pCHAR que_name, pAUTH_TX p_auth_tx );

void ncvsms_log_message( INT dest, INT sev, pCHAR msg, pCHAR func, int details ,pCHAR card_num);
void ncvsms_create_Error_Filename();
INT  ncvsms_Log_error_warning_to_File(pCHAR Error_Buf,int sev, pCHAR func,int detail);
void ncvsms_get_error_warning_file_name_path(void );
void Mask_card_number(char * card_num);
void convert_to_ebcdic( pBYTE source, pBYTE dest, INT len );
void init_character_code_maps();

/* Currency Codes */
#define  PESO     "608"
#define  USD      "840"

/* Currency Code len*/
#define CURRENCY_CODE_LEN 3

#define VISA_DEFAULT_TID 		"99999999"
#define VISA_DEFAULT_TID_LEN 	8

/* Network Controller INI File name */
#define NC_INI_FILENAME  "network.ini"

typedef struct
{
   BYTE net_consec_tmouts_ctr    [4];
   BYTE active_txns_ctr          [4];
   BYTE current_state            [2];
   BYTE prev_src_station         [4];
}VISA_MEM, *pVISA_MEM;



#endif
