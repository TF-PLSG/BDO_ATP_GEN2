/******************************************************************************

   FILE:         TIMINGS.C

   Copyright (c) 2006, Hypercom, Inc.
   All Rights Reserved.

   TITLE:        Database Timing Measurements

   DESCRIPTION:  This file contains API functions associated with collecting
                 and reporting statistics about database query timings.

   APPLICATION:  Advanced Transaction Processor (ATP)

   REVISION HISTORY

   $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\App_lib\Source\timings.c  $
   
      Rev 1.5   Sep 07 2006 13:47:40   dirby
   Added tables BON02 and BON03 to the list of tables for timing analysis.
   SCR 22835
   
      Rev 1.4   Jul 07 2006 14:56:42   dirby
   Add value 26 to Table Defs.  It is missing from the list of App Data Types.
   SCR 12299
   
      Rev 1.3   Jul 07 2006 10:07:56   dirby
   Corrected a bug when reading tf.ini parameters.  On AIX, "rc" does
   not indicate number of bytes returned, but instead true or false.
   SCR 12299
   
      Rev 1.2   Jul 07 2006 07:49:32   dirby
   Moved the table definition structure into the C file from the H file.
   Corrected a pointer problem in 'reset_timing_stats'.
   Changed the name of the tf.ini configuration section.
   SCR 12299
   
      Rev 1.1   Jul 05 2006 18:33:04   dirby
   Incorporated all subtype1 values that data servers use.  Previously,
   only a few were being used to report DB timing statistics.
   SCR 12299
   
      Rev 1.0   Jun 30 2006 16:43:36   dirby
   Initial revision.

 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/timeb.h>
#include "basictyp.h"
#include "ptetime.h"
#include "app_info.h"
#include "ntutils.h"
#include "txutils.h"
#include "timings.h"

#ifdef WIN32
   #ifndef _WINDOWS_
      #include <windows.h>
   #endif
#else
   #include <syslog.h>
   #include <string.h>
#endif


#define   NUM_DB_TABLES     79

typedef struct
{
   BYTE  type;
   BYTE  name[24];

}DB_TABLE_DEFS, *pDB_TABLE_DEFS;


DB_TABLE_DEFS  Table_Defs[NUM_DB_TABLES] =
{
   BIN01_DATA,                  "BIN01",
   OCF01_DATA,                  "OCF01",
   OCF20_DATA,                  "OCF20",
   MCF01_DATA,                  "MCF01",
   MCF20_DATA,                  "MCF20",
   DCF01_DATA,                  "DCF01",
   BCH01_DATA,                  "BCH01",
   BCH10_DATA,                  "BCH10",
   NCF01_DATA,                  "NCF01",
   NCF20_DATA,                  "NCF20",
   UCF01_DATA,                  "UCF01",
   UCF02_DATA,                  "UCF02",
   CAF01_DATA,                  "CAF01",
   CCF01_DATA,                  "CCF01",
   ACF01_DATA,                  "ACF01",
   CCF03N_DATA,                 "CCF03N",
   CCF03P_DATA,                 "CCF03P",
   CCF02H_DATA,                 "CCF02H",
   CCF02P_DATA,                 "CCF02P",
   CCF10_DATA,                  "CCF01",
   HST01_DATA,                  "HST01",
   ATP01_DATA,                  "ATP01",
   ATP02_DATA,                  "ATP02",
   CTF01_DATA,                  "CTF01",
   MCT01_DATA,                  "MCT01",
   PAF01_DATA,                  "PAF01",
           26,                  "",
   CRF01_DATA,                  "CRF01",
   JBF01_DATA,                  "JBF01",
   HCF01_DATA,                  "HCF01",
   FAF01_DATA,                  "FAF01",
   CLF01_DATA,                  "CLF01",
   BPF01_DATA,                  "BPF01",
   APF01_DATA,                  "APF01",
   FCF02_DATA,                  "FCF02",
   TLF01_DATA,                  "TLF01",
   CSF01_DATA,                  "CSF01",
   ALL_DATA,                    "ALL_DATA",
   NCF02_DATA,                  "NCF02",
   OPRMON_DATA,                 "OPRMON",
   NBF01_DATA,                  "NBF01",
   NGF01_DATA,                  "NGF01",
   NFI01_DATA,                  "NFI01",
   NMF01_DATA,                  "NMF01",
   NFI02_DATA,                  "NFI02",
   BCH11_DATA,                  "BCH11",
   NTF01_DATA,                  "NTF01",
   DRF01_DATA,                  "DRF01",
   DEF01_DATA,                  "DEF01",
   CCF04_DATA,                  "CCF04",
   BON01_DATA,                  "BON01",
   UAG01_DATA,                  "UAG01",
   BIN02_DATA,                  "BIN02",
   NCF21_DATA,                  "NCF21",
   NCF30_DATA,                  "NCF30",
   NEG01_DATA,                  "NEG01",
   ATM01_DATA,                  "ATM01",
   TLF01_SAF_DATA,              "TLF01_SAF",
   AUTH_TX_DATA,                "AUTH_TX",
   MCC102_DATA,                 "MCC102",
   MCC103_DATA,                 "MCC013",
   TLF01_GUI_LIST_DATA,         "TLF01_GUI_LIST",
   BCH01_BCH10_TLF01_DATA,      "BCH01_BCH10_TLF01",
   BCH20_DATA,                  "BCH20",
   SAF01_DATA,                  "SAF01",
   OFFUS_VELOCITY_DATA,         "OFFUS_VELOCITY",
   NEGATIVE_FRAUD_DATA,         "NEGATIVE_FRAUD",
   MCC_LIMITS_DATA,             "MCC_LIMITS",
   MCC_LIMITS_GUI_DATA,         "MCC_LIMITS_GUI",
   EMV01_DATA,                  "EMV01",
   COUNTRY_AMT_LIMITS_DATA,     "COUNTRY_AMT_LIMITS",
   COUNTRY_AMT_LIMITS_GUI_DATA, "COUNTRY_AMT_LIMITS_GUI",
   FG01_DATA,                   "FG01",
   FGUARD_DATA,                 "FGUARD",
   FGRAW_DATA,                  "FGRAW",
   BON02_DATA,                  "BON02",
   BON03_DATA,                  "BON03",
   MPPARA01_DATA,				"MPPARA01"
};


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
 * RTRN VALUE:  True  if file is successfully read
 *              False if an error occurrred
 *
 ******************************************************************************/
INT get_timing_parameters( pINT  functionality,
                           pCHAR report_time,
                           pINT  force_report,
                           pCHAR message )
{
   INT    ret_val = true;
   INT    valid_time;
   INT    tmp_time;
   INT    i;
   DWORD  rc;
   CHAR   filename[TIMING_FILE_SIZE] = "";
   CHAR   tmpstr  [TIMING_FILE_SIZE] = "";
   CHAR   tmp_subtime[3];

   /* Determine directory and filename of the .ini file. */
   GetPinnacleConfigDirectory( tmpstr );
   sprintf( filename, "%stf.ini", tmpstr );

   /*------------------------------------------------------*/
   /* Get Database Timing Measurement Functionality Switch */
   /*------------------------------------------------------*/
   memset( tmpstr, 0x00, sizeof(tmpstr) );
   rc = GetPrivateProfileString(
               "DB_TIMINGS",             /* points to section name            */
               "FUNCTIONALITY",          /* points to key name                */
               "0",                      /* points to default string          */
                tmpstr,                  /* points to destination buffer      */
                TIMING_FILE_SIZE - 1,    /* size of destination buffer        */
                filename                 /* points to initialization filename */
        );

   /* Validate the value */
   if ( rc == 1 )
   {
      /* Parameter has one byte. Good! */
      if ( tmpstr[0] == '1' )
         *functionality = true;
      else
         *functionality = false;
   }
   else if ( tmpstr[0] == 0x00 )
   {
      strcpy( message,
             "Database Timing Measurements are turned OFF. Setting not found" );
   }
   else
   {
      strcpy( message,
             "Database Timing Measurements are turned OFF. Invalid setting." );
   }

   if ( *functionality == true )
   {
      /*----------------------------------------------------*/
      /* Get Database Timing Measurement Report Time (hhmm) */
      /*----------------------------------------------------*/
      memset( tmpstr, 0x00, sizeof(tmpstr) );
      rc = GetPrivateProfileString(
                  "DB_TIMINGS",          /* points to section name            */
                  "REPORT_TIME",         /* points to key name                */
                  "0",                   /* points to default string          */
                   tmpstr,               /* points to destination buffer      */
                   TIMING_FILE_SIZE - 1, /* size of destination buffer        */
                   filename              /* points to initialization filename */
        );

      /* Validate the value */
      if ( strlen(tmpstr) == 4 )
      {
         /* This is the correct length.
          * Validate numeric and hh (0-23) mm (0-59)
          */
         for( i=0,valid_time=true; i<4; i++ )
         {
            if ( !isdigit(tmpstr[i]) )
            {
               valid_time = false;
               break;
            }
         }

         if ( valid_time )
         {
            /* Validate hour: 00 - 23 */
            memset( tmp_subtime, 0x00, sizeof(tmp_subtime) );
            memcpy( tmp_subtime, tmpstr, 2 );
            tmp_time = atoi( tmp_subtime );
            if ( tmp_time > 23 )
               valid_time = false;
            else
            {
               /* Validate minute: 00 - 59 */
               memset( tmp_subtime, 0x00, sizeof(tmp_subtime) );
               memcpy( tmp_subtime, tmpstr+2, 2 );
               tmp_time = atoi( tmp_subtime );
               if ( tmp_time > 59 )
                  valid_time = false;
            }

            if ( !valid_time )
            {
               *functionality = false;
               strcpy( message,
                      "Database Timing Measurements are turned OFF. Invalid Report Time");
            }
            else
            {
               strcpy( report_time, tmpstr );

               /*----------------------------------------------------*/
               /* Get Database Timing Measurement Report Time (hhmm) */
               /*----------------------------------------------------*/
               memset( tmpstr, 0x00, sizeof(tmpstr) );
               rc = GetPrivateProfileString(
                  "DB_TIMINGS",          /* points to section name            */
                  "FORCE_REPORT",        /* points to key name                */
                  "0",                   /* points to default string          */
                   tmpstr,               /* points to destination buffer      */
                   TIMING_FILE_SIZE - 1, /* size of destination buffer        */
                   filename              /* points to initialization filename */
                  );

               /* Validate the value */
               if ( (rc == 1) && (tmpstr[0] == '1') )
                  *force_report = true;
               else
                  *force_report = false;
            }
         }
      }
   }

   if ( *functionality == true )
   {
      strcpy( message, "Database Timing Measurements are turned ON." );
   }
   return( ret_val );
}


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
                          pTIMINGS_LIST  timing_list )
{
   INT    i;
   INT    entry_found = false;
   float  total_duration;

   /* Look for this query in the structure. If found, update it. */
   for( i=0; i<timing_list->num_entries; i++ )
   {
      if ( timing_list->st_timings[i].table == table )
      {
         if ( timing_list->st_timings[i].sql_type == sql_type )
         {
            if ( timing_list->st_timings[i].subtype2 == subtype2 )
            {
               entry_found = true;

               /* Recalculate the average; first undo current average. */
               total_duration = timing_list->st_timings[i].num_queries *
                                timing_list->st_timings[i].avg_duration;

               /* Factor in this new query. */
               total_duration += duration;
               timing_list->st_timings[i].num_queries += 1;

               /* Calculate new average. */
               timing_list->st_timings[i].avg_duration = total_duration /
                                                         timing_list->st_timings[i].num_queries;

               /* Check for a new max duration. */
               if ( duration > timing_list->st_timings[i].max_duration )
                  timing_list->st_timings[i].max_duration = duration;

               break;
            }
         }
      }
   }

   if ( !entry_found )
   {
      /* This is a new query. Start a new set of statistics. */
      timing_list->st_timings[timing_list->num_entries].sql_type     = sql_type;
      timing_list->st_timings[timing_list->num_entries].subtype2     = subtype2;
      timing_list->st_timings[timing_list->num_entries].table        = table;
      timing_list->st_timings[timing_list->num_entries].num_queries  = 1;
      timing_list->st_timings[timing_list->num_entries].avg_duration = duration;
      timing_list->st_timings[timing_list->num_entries].max_duration = duration;

      timing_list->num_entries += 1;
   }

   return;
}


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
void reset_timing_stats( pTIMINGS_LIST  timing_list )
{
   memset( timing_list, 0x00, sizeof(TIMINGS_LIST) );

   return;
}


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
INT is_it_report_time( double next_report )
{
   INT     retval;
   double  current_time;

   current_time = ptetime_get_time();
   if ( next_report < current_time )
      retval = true;
   else
      retval = false;

   return( retval );
}


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
double set_next_report_time_at_startup( pCHAR report_time )
{
   double  next_report;
   double  d_current_time;
   double  d_report_time;
   CHAR    tmptime[3] = "";

   struct  timeb   time_structure;   /* Time for ftime         */
   struct  tm     *current_time;     /* Current time structure */

   /* Current time in 'seconds' format. */
   d_current_time = ptetime_get_time();

   /* Current time in tm structure format. */
   ftime (&time_structure);
   current_time = localtime (&time_structure.time);

   /* Get report time and put it into tm structure. */
   memcpy( tmptime, report_time, 2 );
   current_time->tm_hour = atoi( tmptime );

   memcpy( tmptime, report_time+2, 2 );
   current_time->tm_min = atoi( tmptime );

   /* Convert tm structure into 'seconds' format. */
   d_report_time = mktime( current_time );
   
   /* Set the time for the next report. */
   if ( d_current_time < d_report_time  )
   {
      /* Current time of day is before the configured time.
       * This means the report has not been logged yet today.
       * Set time of the next report to the configured time.
       */
      next_report = d_report_time;
   }
   else
   {
      /* Current time of day is after the configured time.
       * This means the report should have already happened today.
       * Set time of the next report to the configured time plus 24 hours
       * so it will take place tomorrow.
       */
      next_report = d_report_time + TIMING_24_HOURS;
   }
   return( next_report );
}


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
INT log_timing_statistics_report( pTIMINGS_LIST  timing_list )
{
   INT  i;
   INT  st1_flag;
   CHAR st1[20];
   CHAR tmpstr[150];


   for( i=0; i<timing_list->num_entries; i++ )
   {
      /* Make sure this is a valid entry. */
      if ( (timing_list->st_timings[i].table)-100 <= NUM_DB_TABLES )
      {
         /* Get text for subtype 1. */
         st1_flag = true;
         memset( st1, 0x00, sizeof(st1) );
         switch( timing_list->st_timings[i].sql_type )
         {
            case ST1_DB_SELECT:            strcpy( st1, "SELECT"            ); break;
            case ST1_DB_INSERT:            strcpy( st1, "INSERT"            ); break;
            case ST1_DB_UPDATE:            strcpy( st1, "UPDATE"            ); break;
            case ST1_DB_DELETE:            strcpy( st1, "DELETE"            ); break;
            case ST1_DB_GET_GUI_LIST:      strcpy( st1, "GET GUI LIST"      ); break;
            case ST1_DB_GET_SERVICE_LIST:  strcpy( st1, "GET SERVICE LIST"  ); break;
            case ST1_DB_ECHO:              strcpy( st1, "ECHO"              ); break;
            case ST1_DB_GET_COUNT:         strcpy( st1, "GET COUNT"         ); break;
            case ST1_DB_MOVE_TRANS:        strcpy( st1, "MOVE TRANS"        ); break;
            case ST1_DB_CHECK_BIN_RANGE:   strcpy( st1, "GET BIN RANGE"     ); break;
            case ST1_DB_GET_SETTLE_TOTALS: strcpy( st1, "GET SETTLE TOTALS" ); break;
            case ST1_DB_GET_ALL:           strcpy( st1, "GET ALL"           ); break;
            case ST1_DB_SEL_FOR_UPD:       strcpy( st1, "SELECT FOR UPDATE" ); break;
            case ST1_DB_SELECT_AUTH_ID:    strcpy( st1, "SELECT AUTH ID"    ); break;
            case ST1_DB_CHECK_BIN02_RANGE: strcpy( st1, "GET BIN02 RANGE"   ); break;
            default:  st1_flag = false;
         }

         if ( st1_flag == true )
         {
            /* Yes it is a valid table and subtype1. */
            memset( tmpstr, 0x00, sizeof(tmpstr) );
            sprintf( tmpstr,
                    "STATS >> Table %s, SQL %s, type %d, Hits %d, Avg %.2f, Max %.2f",
                     Table_Defs[(timing_list->st_timings[i].table)-100].name,
                     st1,
                     timing_list->st_timings[i].subtype2,
                     timing_list->st_timings[i].num_queries,
                     timing_list->st_timings[i].avg_duration,
                     timing_list->st_timings[i].max_duration );

            TxUtils_Send_Msg_To_Operator( 0, tmpstr, 1, INFO_MSG,
                                         "log_timing_statistics_report",
                                          0, INFO_ERROR,
                                          NULL_PTR, NULL_PTR, NULL_PTR );
         }
      }
   }
   return( timing_list->num_entries );
}



