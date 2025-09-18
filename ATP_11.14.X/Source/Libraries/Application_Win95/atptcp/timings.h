/******************************************************************************
 *
 * Copyright (c) 2006, Hypercom, Inc.
 * All Rights Reserved.
 *
 * MODULE:      TXTRACE.H
 *  
 * TITLE:       Database Timing Function Prototypes
 *  
 * DESCRIPTION: This header file contains a list function prototypes for
 *              APIs that perform database timings.
 *
 * APPLICATION: Advanced Transaction Processor (ATP)
 *
 * AUTHOR:      D. Irby
 *
 * REVISION HISTORY:
 *
 * $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\App_lib\Source\timings.h  $
   
      Rev 1.1   Jul 07 2006 07:47:42   dirby
   Moved the table definition structure from the H file into the C file
   so applications that build with the library will build.
   SCR 12299
   
      Rev 1.0   Jun 30 2006 16:43:36   dirby
   Initial revision.
 *
 *******************************************************************************/

#ifndef TIMINGS_H
#define TIMINGS_H


#define   TIMING_FILE_SIZE  80
#define   TIMING_24_HOURS   86400.0  /* Number of seconds in 24 hours */


/******************************************************************************
 *
 * MODULE:	    GET_TIMING_PARAMETERS
 *
 * DESCRIPTION: This function reads tf.ini file to get the parameters from
 *              the TIMINGS section.  These are parameters to toggle the
 *              database timing functionality and to get the time of day
 *              to report the statistics.
 *
 *              If the TIMINGS section is not found, or and parameter is not
 *              found, their default values are set:
 *
 *                 functionality default is OFF
 *                 report_time   default is 2330 (11pm)
 *                 force_report  default is OFF
 *
 * AUTHOR:      D. Irby
 *
 * INPUTS:      None
 *
 * OUTPUTS:     functionality - true if switch is ON, else false
 *              report_time   - time of day (hhmm) to log the statistics
 *              force_report  - true if switch is ON, else false
 *              message       - text indicating status of parameters
 *
 * RTRN VALUE:  True  if file is successfully read (always)
 *              False if an error occurrred
 *
 ******************************************************************************/
INT get_timing_parameters( pINT  functionality,
                           pCHAR report_time,
                           pINT  force_report,
                           pCHAR message );



/******************************************************************************
 *
 * MODULE:	    UPDATE_TIMING_STATS
 *
 * DESCRIPTION: This function updates a Timings_List structure for a single
 *              database query.  The query will be inserted into the structure
 *              unless it is already there.  If it exists, the count is
 *              incremented, the average is recalculated, and the max duration
 *              is checked and maybe updated.
 *
 * AUTHOR:      D. Irby
 *
 * INPUTS:      sql_type - subtype1 which indicates select, update, etc.
 *              subtype2 - subtype2 in case subtype 1 has multiple queries
 *              table    - app_data_type to indicate database table
 *              duration - amount of time for the query
 *
 * OUTPUTS:     timing_list - structure holding the timing statistics
 *
 * RTRN VALUE:  None
 *
 ******************************************************************************/
void update_timing_stats( BYTE           sql_type,
                          BYTE           subtype2,
                          BYTE           table,
                          float          duration,
                          pTIMINGS_LIST  timing_list );



/******************************************************************************
 *
 * MODULE:	    RESET_TIMING_STATS
 *
 * DESCRIPTION: This function initializes a Timings_List structure to
 *              nulls and zeroes.
 *
 * AUTHOR:      D. Irby
 *
 * INPUTS:      timing_list - structure holding the timing statistics
 *
 * OUTPUTS:     timing_list - structure holding the timing statistics
 *
 * RTRN VALUE:  None
 *
 ******************************************************************************/
void reset_timing_stats( pTIMINGS_LIST  timing_list );



/******************************************************************************
 *
 * MODULE:	    IS_IT_REPORT_TIME
 *
 * DESCRIPTION: This function compares the 'next report time' to the
 *              current time to determine if it is time (or past time)
 *              to log the statistic report.
 *
 * AUTHOR:      D. Irby
 *
 * INPUTS:      next_report - time in which to log the statistic report
 *
 * OUTPUTS:     None
 *
 * RTRN VALUE:  True if it is time to log the statistical report, else false
 *
 ******************************************************************************/
INT is_it_report_time( double next_report );



/******************************************************************************
 *
 * MODULE:	    SET_NEXT_REPORT_TIME_AT_STARTUP
 *
 * DESCRIPTION: This function should be called at startup or when a data
 *              server is pinged to get new timing parameters.  It sets
 *              the time for the next timing report.  This value is
 *              calculated by getting current date/time and changing the
 *              time to be that of the configured report time. Then convert
 *              that time into sssssssss.jjj format and comparing it to
 *              current time in same format.
 *
 *              If configured time is greater than current time, next report
 *              time is set to itself.  Else, next report time is set to
 *              itself plus 24 hours.
 *
 * AUTHOR:      D. Irby
 *
 * INPUTS:      report_time - configurable time of day to log the report
 *
 * OUTPUTS:     None
 *
 * RTRN VALUE:  next_report - time when next timing statistics s/b reported
 *
 ******************************************************************************/
double set_next_report_time_at_startup( pCHAR report_time );


/******************************************************************************
 *
 * MODULE:	    LOG_TIMING_STATISTIC_REPORT
 *
 * DESCRIPTION: This function logs the TIMINGS_LIST structure to the
 *              system log.  Each log event contains the statistics for
 *              a single query, such as inserts into TLF01 or selects
 *              from DCF01.
 *
 * AUTHOR:      D. Irby
 *
 * INPUTS:      timing_list - structure holding the timing statistics
 *
 * OUTPUTS:     None
 *
 * RTRN VALUE:  Number of queries logged to the system log
 *
 ******************************************************************************/
INT log_timing_statistics_report( pTIMINGS_LIST  timing_list );



#endif