/***************************************************************************
*
* Copyright (c) 2005, Hypercom, Inc.
* All Rights Reserved.
*
* MODULE:      NCUTILS.H
*  
* TITLE:       Network Control Utility Function Prototypes
*  
* DESCRIPTION: This header file contains prototypes of functions contained
*              in file ncutil.c.  These are functions commonly used by all
*              or most network control modules.  It also contains the
*              shared memory data structure.
*
* APPLICATION: Advanced Transaction Processor (ATP)
*
* AUTHOR:      D. Irby
*
* REVISION HISTORY:
*
* $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\ncmcrd\ncutils.h  $
   
      Rev 1.3   Feb 11 2005 17:32:52   dirby
   Modified to copy decline/failure messages into TLF01's response text.
   SCR 12340
   
      Rev 1.2   Aug 23 2004 12:43:52   dirby
   Added function to convert an ASCII string to binary.
   SCR 1469
   
      Rev 1.1   Jun 21 2000 14:25:48   dirby
    
   
      Rev 1.0   Nov 05 1999 12:50:08   dirby
   Initial Version
*
****************************************************************************/

#ifndef NCUTILS_H
#define NCUTILS_H   

typedef struct
{
   BYTE net_consec_tmouts_ctr    [4];
   BYTE active_txns_ctr          [4];
   BYTE host_state               [2];
}SHARED_MEM, *pSHARED_MEM;


#define HOST_STATE_CHANGED         0
#define HOST_STATE_DID_NOT_CHANGE  1


/******************************************************************************
 *
 *  NAME:         GET_NCF01
 *
 *  DESCRIPTION:  This function makes a call to the data server to select the
 *                NCF01 record for given application.
 *
 *  INPUTS:       p_auth_tx    - Transaction record to be echoed back
 *                source_que   - Queue name of calling application
 *                network_id   - Name of network controller application
 *                network_type - 'I' for Issuer, 'A' for Acquirer
 *                subtype2     - Flag to let us know where to reenter the code
 *
 *  OUTPUTS:      error_msg - If an error occurs, this will contain the message
 *                            Size can be up to 100 characters.
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT get_ncf01( pAUTH_TX p_auth_tx,  pCHAR source_que,
               pBYTE    network_id, pBYTE network_type,
               BYTE     subtype2,   pCHAR error_msg );



/******************************************************************************
 *
 *  NAME:         SEND_MSG
 *
 *  DESCRIPTION:  This function sends a PTE message to a specified queue.
 *
 *  INPUTS:       p_msg     - Pointer to a PTE_MSG structure
 *                que_name  - Name of the destination queue
 *
 *  OUTPUTS:      error_msg - If an error occurs, this will contain the message
 *                            Size can be up to 100 characters.
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT send_msg( pPTE_MSG p_msg, pCHAR que_name, pCHAR error_msg );



/******************************************************************************
 *
 *  NAME:         SET_HOST_TIMER
 *
 *  DESCRIPTION:  This procedure builds a timer message and sends it to the
 *                network data server.  This is a timer used to time messages
 *                sent to a host.
 *
 *  INPUTS:       p_auth_tx    - Transaction record
 *                source_que   - Queue name of calling application
 *                timer_length - Number of seconds to set the timer
 *
 *  OUTPUTS:      error_msg - If an error occurs, this will contain the message
 *                            Size can be up to 100 characters.
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT set_host_timer( pAUTH_TX p_auth_tx,    pCHAR source_que,
                    WORD     timer_length, pCHAR error_msg );



/******************************************************************************
 *
 *  NAME:         CLEAR_TIMER
 *
 *  DESCRIPTION:  This function clears the host state timer using a lookup
 *                key passed into it.
 *
 *  INPUTS:       lookup_key - Key to indicate which timer to clear
 *                source_que - Queue name of calling application
 *
 *  OUTPUTS:      error_msg - If an error occurs, this will contain the message
 *                            Size can be up to 100 characters.
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT clear_timer( pCHAR lookup_key, pCHAR source_que, pCHAR error_msg );



/******************************************************************************
 *
 *  NAME:         GET_DATE
 *
 *  DESCRIPTION:  This procedure takes an input string that contains a date and
 *                time.  It copies the date portion into an output string, 
 *                formatted slightly different.
 *
 *  AUTHOR:       Dennis Irby
 *
 *  INPUTS:       time_date : Format = "YYYY-0M-0D-0H.0I.0S.JJJ"
 *  OUTPUTS:      date_str  : Format = "YYYY0M0D"
 *  RTRN VALUE:   None
 *
 *  MODIFICATIONS:
 ******************************************************************************/
void get_date( pCHAR time_date, pCHAR date_str );



/******************************************************************************
 *
 *  NAME:         GET_TIME
 *
 *  DESCRIPTION:  This procedure takes an input string that contains a date and
 *                time.  It copies the time portion into an output string, 
 *                formatted slightly different.
 *
 *  AUTHOR:       Dennis Irby
 *
 *  INPUTS:       time_date : Format = "YYYY-0M-0D-0H.0I.0S.JJJ"
 *  OUTPUTS:      time_str  : Format = "0H0I0S"
 *  RTRN VALUE:   None
 *
 *  MODIFICATIONS:
 ******************************************************************************/
void get_time( pCHAR time_date, pCHAR time_str );



/*****************************************************************************
 *
 * NAME:        calc_julian_day
 *
 * DESCRIPTION: This module calculates Julian Day ( DDD ) from the 
 *              input.
 * 
 * INPUTS:      date_str   - input string format  = YYYYMMDD...
 *		
 * OUTPUTS:     julian_str - output string format = DDD
 *
 * RTRN VALUE:  None
 *
 * AUTHOR:      Dennis Irby
 *
 * MODIFICATIONS:
 ******************************************************************************/
void calc_julian_day( pCHAR date_str, pCHAR julian_str );



/******************************************************************************
 *
 *  NAME:         CONVERT_FROM_JULIAN_DAY
 *
 *  DESCRIPTION:  This procedure converts a date in format YYDDD into a date
 *                format of MMDD.
 *
 *  INPUTS:       julian_str - Date string in format YYDDD
 *
 *  OUTPUTS:      date_time  - Date string in format MMDD
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
void convert_from_julian_day( char *julian_str, char *date_time );



/******************************************************************************
 *
 *  NAME:         CREATE_RRN
 *
 *  DESCRIPTION:  This procedure creates a retrieval reference number in the
 *                format of YDDDhmmssjjj, that is:
 *                     Y   = last digit of the year
 *                     DDD = Julian day
 *                     h   = last digit of the 24 hour clock (current time)
 *                     mm  = current system minutes
 *                     ss  = current system seconds
 *                     jjj = current system milliseconds
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      rrn - Formatted as mentioned above in the description
 *                      RRN should be a 13 character string.
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
void create_rrn( pCHAR rrn );


/******************************************************************************
 *
 *  NAME:         SEND_MSG_TO_AUTHORIZER
 *
 *  DESCRIPTION:  This function builds and send a PTE message to the
 *                authorizer queue using the AUTH_TX structure.  It sends
 *                a message type of MT_AUTH_RESPONSE or MT_AUTH_REQUEST.
 *
 *  INPUTS:       p_auth_tx  - Transaction structure
 *                msg_type   - Either MT_AUTH_RESPONSE or MT_AUTH_REQUEST
 *                source_que - Queue name of calling application
 *
 *  OUTPUTS:      error_msg - If an error occurs, this will contain the message
 *                            Size can be up to 100 characters.
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT send_msg_to_authorizer( pAUTH_TX p_auth_tx,  BYTE msg_type,
                            pCHAR    source_que, pCHAR error_msg );



/******************************************************************************
 *
 *  NAME:         RJ_WITH_LEAD_ZEROS
 *
 *  DESCRIPTION:  This procedure right justifies a string, padding leading
 *                zeros.
 *
 *  INPUTS:       str      - Pointer to string to be right justified
 *                FieldLen - Length of str
 *
 *  OUTPUTS:      str - Contents of str is modified.
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
void Rj_with_lead_zeros( pCHAR str, INT FieldLen );


/******************************************************************************
 *
 *  NAME:         CREATE_SHARED_MEM_TABLE
 *
 *  DESCRIPTION:  This function creates a table in shared memory consisting
 *                of 3 values:
 *                             number of consecutive time outs
 *                             number of active transactions
 *                             host state
 *
 *                These values get initialized to: 0, 0, 2 respectively.
 *                Note that host state of '2' means not logged on yet.
 *
 *  INPUTS:       AppName - Name of application
 *
 *  OUTPUTS:      errbuf  - Contains textual message if return value is not OK
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       D. Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT create_shared_mem_table( pCHAR AppName, pCHAR errbuf );

/******************************************************************************
 *
 *  NAME:         WRITE_TO_SHARED_MEM_TABLE
 *
 *  DESCRIPTION:  This function writes to the shared memory table, defined
 *                with a global variable.  It writes 3 values to the table.
 *                These values are defined below in the INPUTS section.
 *
 *  INPUTS:       AppName - Name of application
 *                consec_time_out_count
 *                active_tran_count
 *                host_state
 *                host_state_change_flag:  HOST_STATE_CHANGED
 *                                         HOST_STATE_DID_NOT_CHANGE
 *
 *  OUTPUTS:      errbuf  - Contains textual message if return value is not OK
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       D. Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT write_to_shared_mem_table( pCHAR AppName,
                               pBYTE consec_time_out_count,
                               pBYTE active_tran_count,
                               pBYTE host_state,
                                BYTE host_state_change_flag,
                               pCHAR errbuf );

/******************************************************************************
 *
 *  NAME:         READ_SHARED_MEM_TABLE
 *
 *  DESCRIPTION:  This function reads the shared memory table, defined
 *                with a global variable.  It reads 3 values from the table.
 *                These values are defined below in the OUTPUTS section.
 *
 *  INPUTS:       AppName - Name of application
 *
 *  OUTPUTS:      consec_time_out_count
 *                active_tran_count
 *                host_state
 *                errbuf  - Contains textual message if return value is not OK
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       D. Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT read_shared_mem_table( pCHAR AppName,
                           pBYTE consec_time_out_count,
                           pBYTE active_tran_count,
                           pBYTE host_state,
                           pCHAR errbuf );

/******************************************************************************
 *
 *  NAME:         SET_HOST_STATE
 *
 *  DESCRIPTION:  This function sets the state of the host in shared memory.
 *
 *  INPUTS:       AppName - Name of application
 *                host_state - State to set the host
 *                                0 = Logged on
 *                                1 = Not lgged on
 *                                2 = Logged on, but busy
 *
 *  OUTPUTS:      errbuf  - Contains textual message if return value is not OK
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       D. Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT set_host_state( pCHAR AppName, pBYTE host_state, pCHAR errbuf );


/******************************************************************************
 *
 *  NAME:         DECREMENT_ACTIVE_COUNT
 *
 *  DESCRIPTION:  This function reads the shared memory table, decrements the
 *                active transaction counter, and stores those values back
 *                into shared memory.
 *
 *  INPUTS:       AppName - Name of application
 *
 *  OUTPUTS:      errbuf  - Contains textual message if return value is not OK
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       D. Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT decrement_active_count( pCHAR AppName, pCHAR errbuf );


/******************************************************************************
 *
 *  NAME:         INCREMENT_ACTIVE_COUNT
 *
 *  DESCRIPTION:  This function reads the shared memory table, increments the
 *                active transaction counter, and stores those values back
 *                into shared memory.  This function also compares the number
 *                of active transactions to the max allowed.  If too high,
 *                the host state is set to 1.
 *
 *  INPUTS:       AppName - Name of application
 *
 *  OUTPUTS:      errbuf  - Contains textual message if return value is not OK
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       D. Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT increment_active_count( pCHAR AppName, pCHAR errbuf );


/******************************************************************************
 *
 *  NAME:         INCREMENT_TIMEOUT_COUNT
 *
 *  DESCRIPTION:  This function reads the shared memory table, increments the
 *                transaction timeout counter, and stores those values back
 *                into shared memory.  This function also compares the number
 *                of time outs to the max allowed.  If too high, the host
 *                state is set to 2, effectively logging us off.
 *
 *  INPUTS:       AppName - Name of application
 *
 *  OUTPUTS:      errbuf  - Contains textual message if return value is not OK
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       D. Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT increment_timeout_count( pCHAR AppName, pCHAR errbuf, pAUTH_TX p_auth_tx );


/******************************************************************************
 *
 *  NAME:         RESET_TIMEOUT_COUNT
 *
 *  DESCRIPTION:  This function reads the shared memory table, resets the
 *                transaction timeout counter to zero, and stores those
 *                values back into shared memory.
 *
 *  INPUTS:       AppName - Name of application
 *
 *  OUTPUTS:      errbuf  - Contains textual message if return value is not OK
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       D. Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT reset_timeout_count( pCHAR AppName, pCHAR errbuf );


/******************************************************************************
 *
 *  NAME:         ReadMemTable
 *
 *  DESCRIPTION:  This is the function that actually connects to the shared
 *                memory table and reads the information contained in it.
 *
 *  INPUTS:       TableName - Name of application
 *
 *  OUTPUTS:      tmouts    - Number of consecutive timeouts
 *                txns      - Number of active transactions
 *                state     - Host state
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       D. Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
XINT ReadMemTable( pCHAR TableName, pBYTE tmouts, pBYTE txns, pBYTE state );


/******************************************************************************
 *
 *  NAME:         WriteMemTable
 *
 *  DESCRIPTION:  This is the function that actually connects to the shared
 *                memory table and writes information into it.
 *
 *  INPUTS:       TableName - Name of application
 *                tmouts    - Number of consecutive timeouts
 *                txns      - Number of active transactions
 *                state     - Host state
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       D. Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
LONG WriteMemTable( pCHAR TableName, pBYTE tmouts, pBYTE txns, pBYTE state );



/******************************************************************************
 *
 *  NAME:         NC_ERROR_HANDLER
 *
 *  DESCRIPTION:  This procedure is used to log errors, warnings, or
 *                informational messages to the error log and opermon.
 *                If debug is on, they will be displayed on the console.
 *
 *  INPUTS:       MonitorFlag - 0 = Do NOT write to Systems Monitor
 *                              1 = DO write to Systems Monitor
 *
 *                Message     - String of text to be displayed
 *
 *                EventLogFlag- 0 = Do NOT write to event log
 *                              1 = DO write to event log
 *
 *                LogMsgType  - INFO_MSG, WARN_MSG, ERROR_MSG, ALERT_MSG
 *
 *                FuncName    - Name of function calling this routine
 *
 *                Severity    - 0, 1, 2, 3, 4 (0 being the lowest severity)
 *
 *                MsgType     - INFO_ERROR, WARNING_ERROR, FATAL_ERROR
 *
 *                Term_Id     - Terminal ID
 *
 *                CardNum     - Card number
 *
 *                Merch_Id    - Merchant ID
 *
 *                Msg_Id      - Unique identifier to indicate where msg originated
 *
 *                RespTextFlag- 1 = Copy Message into auth_tx, else don't
 *
 *                p_auth_tx   - pointer Auth_Tx structure
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void nc_error_handler( INT MonitorFlag, pBYTE Message,  INT  EventLogFlag,
                       INT LogMsgType,  pBYTE FuncName, BYTE Severity,
                       pBYTE MsgType,   pBYTE Term_Id,  pBYTE CardNum,
                       pBYTE Merch_Id,  pBYTE Msg_Id,   INT  RespTextFlag,
                       pAUTH_TX p_auth_tx );

/******************************************************************************
 *
 *  NAME:         UPDATE_HOST_TOTALS     ( a.k.a  RECON_UPDATE )
 *
 *  DESCRIPTION:  This function sends auth_tx to the data server.
 *                Its purpose is to update totals and keep a running count.
 *                It also stores the transaction into NCF30.
 *
 *  INPUTS:       p_auth_tx    - Transaction record
 *                subtype_2    - Subtype 2 for the PTE message
 *                network_type - I or A for issuer or acquirer
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT update_host_totals( pAUTH_TX p_auth_tx, BYTE subtype_2, pCHAR network_type);


/******************************************************************************
 *
 *  NAME:         RESET_TIMER
 *
 *  DESCRIPTION:  This procedure resets a timer.  The reset is accomplished
 *                by clearing the timer, then setting it.
 *
 *  INPUTS:       AppName      - Name of application
 *                source_que   - Queue name of calling application
 *                timer_id     - ID to help create lookup key
 *                timer_length - Timeout value
 *
 *  OUTPUTS:      error_msg - If an error occurs, this will contain the message
 *                            Size can be up to 100 characters.
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT reset_timer( pCHAR AppName,  pCHAR source_que,
                 pCHAR timer_id, WORD  timer_length,
                 pCHAR error_msg );


/******************************************************************************
 *
 *  NAME:         BUILD_AND_SEND_REQUEST_MESSAGE
 *
 *  DESCRIPTION:  This procedure builds and sends a request message to the 
 *                desired queue.
 *
 *  INPUTS:       msgtype  - PTE message type 
 *                subtype1 - Subtype1 for the message (see PTE.h)
 *                subtype2 - Subtype2 for the message (see PTE.h and APP_INFO.h) 
 *                que_name - Name of the queue to send the message to 
 *                buffer   - Buffer that contains the data to be sent, 
 *                length   - Length of buffer
 *                app_type - App data type for the message (see APP_INFO.h)
 *                comm_info- string containing originator communications info
 *
 *  OUTPUTS:      err_msg  - Text to describe reason for failure
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Beth Gable
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT build_and_send_request_message( BYTE     msgtype,
								    BYTE     subtype1, 
                                    BYTE     subtype2, 
                                    pBYTE    que_name, 
                                    pBYTE    buffer, 
                                    INT      length,
                                    BYTE     app_type,
                                    pCHAR    comm_info,
                                    pCHAR    err_msg );


/*****************************************************************************

  Function:   ascii_to_bin

  Description:
      Convert an ascii string to a binary string
  Author: 
      SVelaga   6/21/99
  Inputs:
      astr      - pointer to the incoming ascii string
      ascii_len - length of astr
      blen      - length of bstring
  Outputs:
      binary_string  - pointer to the outgoing binary string
  Return values:
      NONE

****************************************************/
void ascii_to_bin( CHAR *astr, INT ascii_len, BYTE *binary_string, INT blen);


BOOLEAN hhutil_verify_pin_block(pAUTH_TX p_auth_tx);
void asc_to_bcd (pCHAR, INT, BYTE [] );
BOOLEAN ncmcrd_Is_Refund_Transaction(pAUTH_TX p_auth_tx);

char* mask_card_number(char *card_num);

#endif
