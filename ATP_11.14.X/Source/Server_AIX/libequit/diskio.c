/******************************************************************************

   FILE:         DISKIO.C

   Copyright (c) 2002, Hypercom, Inc.
   All Rights Reserved.

   TITLE:        Disk I/O Functions

   DESCRIPTION:  This file contains general purpose functions for
                 performing disk I/O.

   APPLICATION:  Advanced Transaction Processor (ATP)

   REVISION HISTORY

   $Log:   N:\PVCS\PTE\Equitable\App_lib\diskio.c  $
   
      Rev 1.0   Jan 15 2002 11:18:04   dirby
   Functions that support disk I/O
   SCR 546
   
      Rev 1.0   Jan 15 2002 11:15:44   dirby
   Functions that support disk I/O
   SCR 546

 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "basictyp.h"
#include "diskio.h"

/******************************************************************************
 *
 * MODULE:	    open_file
 *
 * DESCRIPTION: This function opens a file in the mode specified
 *              by an input parameter.
 *
 * AUTHOR:      D. Irby
 *
 * INPUTS:      filename - Name of file to open
 *              mode     - Mode to open the file, e.g. 'r' or 'w'
 *
 * OUTPUTS:     fp - FILE * : Pointer to the file that was just opened
 *
 * RTRN VALUE:  True  if file is successfully opened
 *              False if an error occurrred
 *
 ******************************************************************************/
INT open_file( FILE **fp, pCHAR filename, pCHAR mode )
{
   INT  ret_val = true;

   if ( (*fp = fopen( filename, mode )) == NULL )
      ret_val = false;  /* Error */

   return( ret_val );
}


/******************************************************************************
 *
 * MODULE:	    open_file_for_reading
 *
 * DESCRIPTION: This function opens a text file in read mode.
 *
 * AUTHOR:      D. Irby
 *
 * INPUTS:      filename - Name of file to open
 *
 * OUTPUTS:     fp - FILE * : Pointer to the file that was just opened
 *
 * RTRN VALUE:  True  if file is successfully opened
 *              False if an error occurrred
 *
 ******************************************************************************/
INT open_file_for_reading( FILE **fp, pCHAR filename )
{
   INT  ret_val = true;

   if ( (*fp = fopen( filename, "r" )) == NULL )
      ret_val = false;  /* Error */
 
   return( ret_val );
}


/******************************************************************************
 *
 * MODULE:	    write_file
 *
 * DESCRIPTION: This function writes a data string to a text file.
 *              A newline character is written out after the data string.
 *
 * AUTHOR:      D. Irby
 *
 * INPUTS:      fp         - File pointer
 *              datastring - String of data to be written
 *
 * OUTPUTS:     None
 *
 * RTRN VALUE:  True  if file is successfully written to
 *              False  if an error occurrred
 *
 ******************************************************************************/
INT write_file( FILE **fp, pCHAR datastring )
{
   INT  ret_val;

   if ( fputs( datastring, *fp ) == EOF )
      ret_val = false;
   else if ( fputc( '\n', *fp ) != EOF )
      ret_val = true;   /* Success */
   else
      ret_val = false;

   return( ret_val );
}


/******************************************************************************
 *
 * MODULE:	    fwrite_file
 *
 * DESCRIPTION: This function writes data to a binary-mode file.  It compares
 *              the amount of data requested to be written to the amount of
 *              data actually written.  If they are not the same, it is
 *              considered an error.
 *
 *              The value of NUM_SIZE_WRITES defined within represents the
 *              number of times 'size' bytes will be written.  For example,
 *              if 'size' is 100 bytes, the amount of bytes actually
 *              written out will be 100 times NUM_SIZE_WRITES.  The value
 *              returned should be the number of times 'size' bytes was
 *              written out.
 *
 * AUTHOR:      D. Irby
 *
 * INPUTS:      fp         - File pointer
 *              datastring - Starting address of data to be written
 *              size       - Number of bytes to be written
 *
 * OUTPUTS:     None
 *
 * RTRN VALUE:  True   if file is successfully written to
 *              False  if an error occurrred
 *
 ******************************************************************************/
INT fwrite_file( FILE **fp, pCHAR datastring, INT size )
{
   #define NUM_SIZE_WRITES  1

   INT  ret_val;

   if ( NUM_SIZE_WRITES == (fwrite(datastring, size, NUM_SIZE_WRITES, *fp)) )
      ret_val = true;
   else
      ret_val = false;

   return( ret_val );
}


/******************************************************************************
 *
 * MODULE:	    flush_file
 *
 * DESCRIPTION: This function flushes a file's system buffer.  This guarantee's
 *              that the last data written to a file is actually in the file.
 *
 * AUTHOR:      D. Irby
 *
 * INPUTS:      fp - FILE * : File pointer
 *
 * OUTPUTS:     None
 *
 * RTRN VALUE:  True   if file is successfully flushed
 *              False  if an error occurrred on the flush
 *
 ******************************************************************************/
INT flush_file( FILE **fp )
{
   INT  ret_val = true;

   if ( fflush( *fp ) != 0 )
      ret_val = false;    /* Error */

   return( ret_val );
}


/******************************************************************************
 *
 * MODULE:	    close_file
 *
 * DESCRIPTION: This function closes a file.
 *
 * AUTHOR:      D. Irby
 *
 * INPUTS:      fp - FILE * : File pointer
 *
 * OUTPUTS:     None
 *
 * RTRN VALUE:  True   if file is successfully closed
 *              False  if an error occurrred on the close
 *
 ******************************************************************************/
INT close_file( FILE **fp )
{
   INT  ret_val = true;

   if ( fclose( *fp ) != 0 )
      ret_val = false;    /* Error */

   return( ret_val );
}


