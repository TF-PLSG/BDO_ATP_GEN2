#ifndef _TXUTILS
#define _TXUTILS
#include "basictyp.h"
#include "app_info.h"
#include "equitdb.h"

LONG Txutils_Calculate_Julian_Date (BYTE* yyyymmdd) ;
void Txutils_Calculate_Gregorian_Date(LONG julian, pCHAR yyyymmdd);
BOOLEAN TxUtils_Send_Msg_To_Operator (int nWriteToSystemMonitor, BYTE* strMessage, 
									  int nWriteToEventLog, int nEventLogMsgType, 
									  BYTE* strFunction, BYTE nSeverity, 
									  BYTE* strMessageType, BYTE* strTerminal, 
									  BYTE* strCardNumber, BYTE* strMerchant);

/* Used to end a list in an INI file. */
#define  END_OF_LIST     "END OF LIST"

/* For Data Server Query timer functionality */
#define DS_TIMER_OFF            0
#define DS_TIMER_ON             1
#define DEFAULT_DS_TIMER_VALUE  5



/*+----------------------------------------------------------+
  | Function Prototypes for reading a list from an INI file. |
  +----------------------------------------------------------+*/
BOOLEAN blank_or_null_str(pCHAR astr, INT max_len);


/******************************************************************************
 *
 *  NAME:         GETPRIVATEPROFILELIST
 *
 *  DESCRIPTION:  This function reads a list of identifiers from an .INI file.
 *                It opens the given filename, searches for the section title,
 *                then reads the list for that section.  It reads the list
 *                until it finds one of the following four:
 *
 *                   A blank line
 *                   End of File
 *                   The phrase "END OF LIST"
 *                   Another section title
 *
 *                The list of identifiers is stored into a character array
 *                whose elements should be defined as MAX_QUE_NAME_SIZE.  The
 *                array can hold as many as MAX_INI_ID_LIST_SIZE identifiers.
 *
 *  INPUTS:       filename        = Path and filename of INI file
 *                section_name    = Name of list to read
 *                size_of_element = Size of a single structure in 'list';
 *                                  Note that 'list' is an array of structures
 *
 *  OUTPUTS:      list = Pointer to start of memory allocated for ID's
 *
 *  RTRN VALUE:   If positive, indicates the number of ID's being returned
 *                If negative, indicates an error as follows:
 *
 *                   -1 = Unable to open the file
 *                   -2 = Cannot find the section name
 *                   -3 = Error occurred while copying an ID into the output
 *                        list.  Could indicate 'size_of_list' is too small
 *                        of a value to hold all the ID's being read.
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT GetPrivateProfileList( pCHAR filename, pCHAR section_name,
                           pCHAR list,     INT   size_of_element );


/******************************************************************************
 *
 * MODULE:	    FIND_SECTION_NAME
 *
 * DESCRIPTION: This function reads a file looking for a name defined within
 *              square brackets, i.e. [section_name].
 *
 * AUTHOR:      D. Irby
 *
 * INPUTS:      fp           = File pointer
 *              section_name = Name we are looking for within brackets
 *
 * OUTPUTS:     fp = On success, fp will point to the line after section name
 *
 * RTRN VALUE:  True  if section name is found
 *              False if section name is not found
 *
 * MODIFICATIONS:
 ******************************************************************************/
BOOLEAN find_section_name( FILE **fp, pCHAR section_name );




/******************************************************************************
 *
 * MODULE:	    READ_ID
 *
 * DESCRIPTION: This function reads application name identifiers from an ini
 *              file and stores it into an output buffer.
 *
 * AUTHOR:      D. Irby
 *
 * INPUTS:      fp = File pointer
 *
 * OUTPUTS:     id_string = String where ID is stored, must be at least
 *                          MAX_QUE_NAME_SIZE+1 in size.
 *
 * RTRN VALUE:   False = End of list - see note
 *               True  = Successful read
 *
 *       NOTE:   End of list is defined as any of the following:
 *                  The string "END OF LIST"
 *                  A blank line
 *                  A new section
 *                  End of file
 *                  File read error
 *
 * MODIFICATIONS:
 ******************************************************************************/
BOOLEAN read_id( FILE **fp, pCHAR id_string );




/******************************************************************************
 *
 * MODULE:	    COPY_ID_TO_LIST
 *
 * DESCRIPTION: This function copies an application ID into a list of IDs.
 *              It will first make sure there is enough memory allocated
 *              in the list.  It does this based on a list defined as a CHAR
 *              array[MAX_INI_ID_LIST_SIZE][MAX_QUE_NAME_SIZE+1].  If there
 *              is not enough memory, an error is returned.
 *
 * AUTHOR:      D. Irby
 *
 * INPUTS:      size_of_element = Number of bytes allocted for the output
 *              num_items       = Number of identifiers already stored in list
 *
 * OUTPUTS:     list = Array of application identifiers
 *
 * RTRN VALUE:  True  if storage of identifier into list is successful
 *              False if there is not enough storage in list
 *
 * MODIFICATIONS:
 ******************************************************************************/
BOOLEAN copy_id_to_list( pCHAR id_string,       pCHAR list, 
                         INT   size_of_element, INT   num_items );


/******************************************************************************
 *
 *  NAME:         RESET_STATS
 *
 *  DESCRIPTION:  This function will set the transaction statistics to zero.
 *                This is the number of transactions received from the host,
 *                and the number of transactions this network controller has
 *                sent to the host.  There are also stats for number of txns
 *                approved, declined, reversed, and timed out.
 *
 *  INPUTS:       p_txn_cnts - Structure containing the counts
 *
 *  OUTPUTS:      p_txn_cnts - Structure counts are updated to zero
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void reset_stats( pTXN_COUNTS p_txn_cnts );


/******************************************************************************
 *
 *  NAME:         UPDATE_TXN_STATS
 *
 *  DESCRIPTION:  This function updates the transaction statistics.  The input
 *                parameter determines which statistic gets incremented.  This
 *                function also will reset the statistics if the count gets
 *                too high.
 *
 *  INPUTS:       p_txn_cnt - Structure containing the counts
 *                p_auth_tx - Transaction data structure
 *                stat_type - SENT, RECEIVED, or NA
 *
 *  OUTPUTS:      p_txn_cnts - Structure counts are updated
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void update_txn_stats( pTXN_COUNTS p_txn_cnt, pAUTH_TX p_auth_tx,INT stat_type);


/******************************************************************************
 *
 *  NAME:         GET_TXN_STATS
 *
 *  DESCRIPTION:  This function gets the transaction statistics from the
 *                txn_cnt structure and copies them into the MONITOR_STATS
 *                structure, converting them as needed.  The MONITOR_STATS
 *                structure is used to pass the stats from a service to
 *                the monitor.
 *
 *  INPUTS:       p_txn_cnt - Structure containing the counts
 *
 *  OUTPUTS:      p_monitor_stats - Structure to be passed to Monitor
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void get_txn_stats( pTXN_COUNTS p_txn_cnt, pMONITOR_STATS p_monitor_stats );


/******************************************************************************
 *
 *  NAME:         GET_DATASERVER_INI_TIMER_VALUE
 *
 *  DESCRIPTION:  This function finds the DATASERVERS section in the tf.ini
 *                file.  It then reads the timer flag to determine if timing
 *                of queries is turned on.  If it is, then the value is read.
 *                This timer value is the criteria for logging messages to the
 *                system log (not Monitor).  This value is in seconds, e.g.
 *                a value of 60 is the same as 1 minute.  If a query takes
 *                longer than this value, then a message is logged, but
 *                processing continues.
 *
 *  INPUTS:       appname - Name of calling service, for error messages
 *
 *  OUTPUTS:      ds_timer_flag  - 0 = timing is turned off, 1 = on
 *                ds_timer_value - Number of seconds to compare a query against.
 *                errbuf         - Text message in case of problems.
 *
 *  RTRN VALUE:   True if no problems reading tf.ini file, else false.
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT get_dataserver_ini_timer_value(pINT  ds_timer_flag,
                                   pINT  ds_timer_value,
                                   pCHAR appname, pCHAR errbuf );


/******************************************************************************
 *
 *  NAME:         COMPARE_QUERY_TIME_AGAINST_CRITERIA
 *
 *  DESCRIPTION:  This function compares the time that a query spent with the
 *                database against a configured time.  If the query time is
 *                greater than the configured time, a message is returned,
 *                giving information about the query.  This is so the query
 *                can be identified, with the hopes of improving it.
 *
 *  INPUTS:       start_time    - Time before query (format = ssssssssssss.jjj)
 *                end_time      - Time after  query ( <same> )
 *                criteria_time - Number of seconds that the query should not
 *                                exceed
 *                app_name      - Name of data server service (for err msg)
 *                p_msg         - PTE Msg containing identifying info about
 *                                the query.
 *
 *  OUTPUTS:      errbuf - Text message identifying a query that exceeds
 *                         the criteria time.
 *
 *  RTRN VALUE:   True if query meets the time, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT compare_query_time_against_criteria( pCHAR start_time,    pCHAR end_time,
                                          INT  criteria_time, pPTE_MSG p_msg,
                                         pCHAR app_name,      pCHAR errbuf );



#endif
