/******************************************************************************
  
        Module: ptetime.h
  
         Title: Pinnacle Time APIs
  
   Description: This module provides a set of APIs to access the C time
                routines.

   $Log:   N:\PVCS\PTE\CORE\PTE_LIB\PTETIME.H  $
   
      Rev 1.2   May 27 1998 13:59:40   drodden
   Added a function to return a timestamp of the current Greenwich Mean (UTC) time.
   
   
      Rev 1.1   Apr 17 1998 10:06:38   drodden
   Fixed the month returned in the timestamp.  It was zero relative.
   Removed the microseconds from the timestamp.  It really wasn't
   microseconds but an internal counter that gave a ficticious value.
   
   
      Rev 1.0   Oct 23 1997 11:42:38   drodden
   This is part of the continued development of the Pinnacle 
   Transaction Environment.  Many changes, refinements,
   and additions were made to the core modules.  I'm checking
   them in now to get a snapshot of this current working version.
   
   
      (came from Rev 1.9 of pintime.h)
******************************************************************************/

#ifndef PINTIME_H
#define PINTIME_H

#include "basictyp.h"


/******************************************************************************
*  function ptetime_get_time
*
*  This function returns the current time as a double precision real number
*  in the form  ssssssssss.jjj  where:
*     ssssssssss  = seconds since 1-1-1970.
*     jjj         = milliseconds after the second
*
*  Parameters: none.
*
******************************************************************************/
double ptetime_get_time ();



/******************************************************************************
*  function ptetime_get_strtime
*
*  Same as above except the time is returned in the string specified.
*
*  Parameters:
*     deststr.....string to place the time into (should be at least 14 chars).
*
******************************************************************************/
void ptetime_get_strtime ( pCHAR dest_str );



/******************************************************************************
*  function ptetime_get_timestamp
*
*  This function generates a timestamp for representing the clock time at
*  execution.  The format of the timestamp is ISO standard (???) and has this
*  format:
*     YYYY-MM-DD-HH.II.SS.JJJ
*     where:
*        YYYY   = The year in 4 digits
*        MM     = The month with zero padding, e.g. Feb is 02
*        DD     = The day of the month with zero padding as well
*        HH     = The hour of the day on a 24 hour clock
*        II     = The minute of the hour with zero padding
*        SS     = The seconds after the minute
*        JJJ    = The milliseconds after the second
*
*  Parameters:
*     dest_str.....string to place the timestamp into (should be at least 24 chars).
*
******************************************************************************/
void ptetime_get_timestamp ( pCHAR dest_str );



/******************************************************************************
*  function ptetime_get_gmt
*
*  This function generates a timestamp for the current Greenwich Mean Time.
*
*  The format of the timestamp is:
*     YYYYMMDDHHIISSJJJ
*     where:
*        YYYY   = The year in 4 digits
*        MM     = The month with zero padding, e.g. Feb is 02
*        DD     = The day of the month with zero padding as well
*        HH     = The hour of the day on a 24 hour clock
*        II     = The minute of the hour with zero padding
*        SS     = The seconds after the minute
*        JJJ    = The milliseconds after the second
*
*  Parameters:
*     dest_str.....string to place the timestamp in (should be at least 18 chars).
*
******************************************************************************/
void pintime_get_gmt ( pCHAR dest_str );


#endif


