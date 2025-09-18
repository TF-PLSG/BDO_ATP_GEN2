/******************************************************************************
 *
 * Copyright (c) 2002, Hypercom, Inc.
 * All Rights Reserved.
 *
 * MODULE:      DISKIO.H
 *  
 * TITLE:       Disk Input and Output Function Prototypes
 *  
 * DESCRIPTION: This header file contains a list function prototypes for
 *              functions that perform disk I/O.
 *
 * APPLICATION: Advanced Transaction Processor (ATP)
 *
 * AUTHOR:      D. Irby
 *
 * REVISION HISTORY:
 *
 * $Log:   N:\PVCS\PTE\Equitable\App_lib\diskio.h  $
   
      Rev 1.0   Jan 15 2002 10:42:06   dirby
   Initial revision.
 *
 *******************************************************************************/

#ifndef DISKIO_H
#define DISKIO_H


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
INT open_file( FILE **fp, pCHAR filename, pCHAR mode );


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
INT open_file_for_reading( FILE **fp, pCHAR filename );


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
INT write_file( FILE **fp, pCHAR datastring );


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
INT fwrite_file( FILE **fp, pCHAR datastring, INT size );


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
INT flush_file( FILE **fp );



/******************************************************************************
 *
 * MODULE:	    close_file
 *
 * DESCRIPTION: This function flushes a buffer, then closes the file.
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
INT close_file( FILE **fp );



#endif
