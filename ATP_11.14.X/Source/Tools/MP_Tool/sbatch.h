/*******************************************************************************
*  
* MODULE:           log_file.h
*
* Copyright (c) 2008, ThoughtFocus Technologies Pvt. Limited.  All Rights Reserved.   
*  
* TITLE:            Standin Batch to Host 
*  
* DESCRIPTION:      This process sends all the standin approved transactions to host. 
*
* APPLICATION:      standin_batch_to_host.exe
*
* AUTHOR:           Ravikumar Nageshaiah
*
* REVISION HISTORY:
*

   initial release
*
*******************************************************************************/

#ifdef _DEBUG
	#define PRINT(x) printf(x) 
#else	
	#define PRINT
#endif 

#define  strEqual(x,y)  (!strcmp(x,y))
/* Function Prototypes */
/*BYTE db_select_ncf01 (char*, NCF01*, char*);*/
int MainProcessor();
BOOLEAN create_detail_structure(void);
BOOLEAN date_format_ok(pBYTE date_requested);
void display_error_message(pBYTE buffer, pBYTE function);
void display_info_message(pBYTE buffer, pBYTE function);
void display_log_info(void);
void EndProcess(void);
BOOLEAN open_log_files(void);
void ProcessTransactions();
BOOLEAN set_start_date(void);
void shut_down(void);
pBYTE trim_string(pBYTE instr);
void PinnacleMsgHandler(pPTE_MSG);
BYTE Process_Auth_Advice_Response (pPTE_MSG);
BYTE db_update_bch10_stip( pTLF01 tlf01_ptr, pCHAR ErrorMsg );
BYTE db_update_bch20_stip( pTLF01 tlf01_ptr, pCHAR ErrorMsg );
BYTE db_update_ncf30_stip( pTLF01 tlf01_ptr, pCHAR ErrorMsg );
BYTE db_update_tlf01_stip_count( pTLF01 tlf01_ptr, pCHAR ErrorMsg );
BYTE db_update_tlf01_stip(pTLF01 tlf01_ptr, pCHAR ErrorMsg );
int tlf01_stip_handler(void);
void get_date (pCHAR time_date, pCHAR date_str) ;
BYTE Check_Is_Host_Up (BYTE* destination_name);
BYTE db_select_emvraw( pCHAR     Primary_key, 
                       pEMV_RAW  emvraw_ptr, 
                       pCHAR     ErrorMsg );
					   


BOOLEAN write_to_log_files(void);
BYTE Send_Request_To_Service (BYTE* , BYTE , BYTE , 
							  BYTE , BYTE , 
							  pBYTE ,   INT );


#define RSB_CARD  "RS"




/* network control file */
typedef struct
{
   struct
   {
      BYTE network_id                              [11];
      BYTE network_type                            [2];
   } primary_key;

   BYTE  name                                      [30];
   BYTE  status                                    [2];
   BYTE  totals_ind                                [2];
   BYTE  statistics_ind                            [2];
   BYTE  acquirer_inst_id_code                     [12];
   BYTE  forwarding_inst_id_code                   [12];
   BYTE  receiving_inst_id_code                    [12];
   BYTE  remote_nsp_type                           [2];
   BYTE  pin_type                                  [2];
   BYTE  encr_type                                 [2];
   BYTE  pad_char                                  [2];
   BYTE  cwk                                       [49];
   BYTE  kek                                       [17];
   BYTE  retired_cwk                               [49];
   BYTE  auto_logon                                [2];
   BYTE  echo_interval                             [4];
   BYTE  max_consecutive_timeouts                  [4];
   BYTE  max_active_txns                           [4];
   BYTE  que_name                                  [12];
   BYTE  local_nsp_type                            [8];
   BYTE  request_timer                             [4];
   BYTE  idle_line_timer                           [4];
   BYTE  advice_repeat_counter                     [3];
   BYTE  admin_message_timer                       [4];
   BYTE  store_forward_processing                  [2];
   BYTE  currency_code_purchase                    [4];
   BYTE  currency_code_cash                        [4];
   BYTE  country_code                              [4];
   BYTE  station_id1                               [7];
   BYTE  station_id2                               [7];
   BYTE  prev_src_station                          [7];
   BYTE  tpdu_id                                   [16];
   struct
   {
      BYTE  identifier                             [7];
   }logon_bin[10];
   struct
   {
      BYTE  processing_code                        [7];
   }allowed[20];
   BYTE zmk                                        [49];
   BYTE  neg_access_code                           [7];

}NCF01, *pNCF01;

