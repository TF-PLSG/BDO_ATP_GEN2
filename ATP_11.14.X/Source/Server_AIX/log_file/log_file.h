/*******************************************************************************
*  
* MODULE:           log_file.h
*
* Copyright (c) 2006, Hypercom, Inc. All Rights Reserved.   
*  
* TITLE:            Visa Outgoing Center Transaction files creator . 
*  
* DESCRIPTION:      This module allows to create transaction log files
*                   for particular day and time.  
*
* APPLICATION:      log_file.exe
*
* AUTHOR:           Irene Goldfild
*
* REVISION HISTORY:
*
* $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\tran_log_file\log_file.h  $
   
      Rev 1.15   Feb 17 2006 13:44:56   dirby
   Added terminal_type to the TLF01 structure.
   SCR 19844
   
      Rev 1.14   Dec 20 2005 15:40:14   dirby
   Added service_code to the TLF01 list structure.
   SCR 19010
   
      Rev 1.13   Jun 09 2005 10:54:26   dirby
   Modified to log reload transactions only if they have been confirmed.
   SCR 15748
   
      Rev 1.12   May 18 2005 15:34:24   dirby
   Added message_type to TLF01_AUTH_LIST_STRUCT.
   SCR 15661
   
      Rev 1.11   Jul 02 2002 13:17:12   dirby
   1.  Updated version to 4.0.2.1
   2.  Removed database hit to MCF01 by getting TCC from TLF01.
   3.  Renamed tlf01_list_structure to tlf01_auth_list_structure to
   avoid confusion with tlf01_list_structure in equitdb.h.
   SCR 800
   
      Rev 1.10   Nov 08 2001 14:12:58   dirby
   Added definition for RSB cards and added def_gross_amt and
   type_of_data to the TLF01 list structure.
   
      Rev 1.9   Jul 13 2001 11:50:00   lmitchel
   modifications to add tlf01.cvc to output files
   
      Rev 1.8   Aug 09 2000 18:29:38   lmitchel
   Correct available amount field(tlf01.outstanding) =  available amount prior to transaction
   
      Rev 1.7   Jan 24 2000 11:52:12   farad
   Fixed bug #191 (farad 1-24-2000).  Modified the code such 
   that it only would take 2 digits from the 3 digit field and write
    it to the file.
   
      Rev 1.6   Jul 30 1999 14:33:26   apinto
   Transaction Category Code is tlf01.vs_tcc_code
   or tlf01.mc_fcc_code : Logic Verified-OK!
   
      Rev 1.5   Jun 15 1999 10:20:00   dperson
    
   
      Rev 1.3   May 24 1999 13:21:26   dperson
    
   
      Rev 1.2   May 24 1999 06:58:28   dperson
    
   
      Rev 1.1   May 18 1999 13:34:26   dperson
    
   
      Rev 1.0   Feb 15 1999 13:34:44   IGOLDFIE
   initial release
*
*******************************************************************************/

#ifdef _DEBUG
	#define PRINT(x) printf(x) 
#else	
	#define PRINT
#endif 


/* Function Prototypes */
void MainProcessor(void);
BOOLEAN create_detail_structure(void);
BOOLEAN date_format_ok(pBYTE date_requested);
void display_error_message(pBYTE buffer, pBYTE function);
void display_info_message(pBYTE buffer, pBYTE function);
void display_log_info(void);
void EndProcess(void);
BOOLEAN open_log_files(void);
void ProcessTransactions(pCHAR tran_date);
BOOLEAN set_start_date(void);
void shut_down(void);
pBYTE trim_string(pBYTE instr);
BOOLEAN write_to_log_files(void);

#define RSB_CARD  "RS"

   /*08/08/00LM*/
   typedef struct
   {
      BYTE transaction_id         [21];
      BYTE terminal_id            [9];
      BYTE merchant_id            [16];
      BYTE card_num               [20];
      BYTE exp_date               [5];
      BYTE total_amount           [13];
      BYTE terminal_type          [3];
      BYTE pos_entry_mode         [5];
      BYTE response_code          [3];
      BYTE date_yyyymmdd          [9];
      BYTE time_hhmmss            [7];
      BYTE tx_key                    ; 
      BYTE auth_number            [7];
      BYTE processing_code        [7];
      BYTE message_type           [5];
      BYTE sys_trace_audit_num    [7];
      BYTE outstanding_balance    [13];
      BYTE ticket_nbr             [16];
      BYTE product_code           [9];
      BYTE deferred_factor        [8];  
      BYTE deferred_term_length   [4];  
      BYTE category_code          [5];
      BYTE tran_start_time        [17];
      BYTE tran_finish_time       [17];
      BYTE reversal_amount        [13];
      BYTE retrieval_ref_num      [13];
      BYTE pos_condition_code     [3];
      BYTE odometer               [8];
      BYTE currency_code          [4];
      BYTE cvc                       ;
      BYTE def_gross_amt          [13];
      BYTE type_of_data           [3];
      BYTE tcc                    [7];
      BYTE reload_confirmed       [13];
      BYTE service_code           [4];

   }TLF01_AUTH_LIST_STRUCT, *pTLF01_AUTH_LIST_STRUCT;

   typedef struct
   {
      BYTE  num_returned             [5];
      TLF01_AUTH_LIST_STRUCT  tlf01_auth_list_struct[SCH_MAX_LIST_SIZE];

   }TLF01_LOG_LIST, *pTLF01_LOG_LIST;


