/******************************************************************************
 *
 * Copyright (c) 2002, Hypercom, Inc.
 * All Rights Reserved.
 *
 * MODULE:      TXTRACE.H
 *  
 * TITLE:       Transaction Tracing API Function Prototypes
 *  
 * DESCRIPTION: This header file contains a list function prototypes for
 *              APIs that perform transaction tracing.
 *
 * APPLICATION: Advanced Transaction Processor (ATP)
 *
 * AUTHOR:      D. Irby
 *
 * REVISION HISTORY:
 *
 * $Log:   N:\PVCS\PTE\Equitable\App_lib\txtrace.h  $
   
      Rev 1.2   Jan 16 2002 09:22:48   dirby
   Changed function log_trace_entry to return negative numbers for errors.
   SCR 546
   
      Rev 1.1   Jan 15 2002 15:28:36   dirby
   Added function prototype for 'get_trace_ini_info'.
   SCR 546
   
      Rev 1.0   Jan 15 2002 10:43:04   dirby
   Initial revision.
 *
 *******************************************************************************/

#ifndef TXTRACE_H
#define TXTRACE_H

#define MAX_TRACE_ENTRY_SIZE  2000

/******************************************************************************
 *
 * MODULE:	    open_trace_file
 *
 * DESCRIPTION: This function opens a trace file at the locatoin specified.
 *              If the file exists, it is opened for appending.  The name
 *              of the file to be opened is based on 'service_name'.  The
 *              file's extension is based on system date.
 *
 *              Format of the filename is: service_name.MMDD
 *
 *              Note:  This file is opened as a binary file
 *
 * AUTHOR:      D. Irby
 *
 * INPUTS:      service_name - Name of application using this function
 *              path         - Directory to create the file in
 *
 * OUTPUTS:     fp       - Pointer to the file that was just opened
 *              filename - Name and path of file that was opened
 *
 * RTRN VALUE:  True  if file is successfully opened
 *              False if an error occurrred
 *
 ******************************************************************************/
INT open_trace_file( pCHAR service_name, pCHAR path, FILE **fp, pCHAR filename);


/******************************************************************************
 *
 * MODULE:	    log_trace_entry
 *
 * DESCRIPTION: This function timestamps an entry, then writes it to the
 *              output file.  It also checks to see if a new file is required,
 *              due to rolling over to a new day.
 *
 * AUTHOR:      D. Irby
 *
 * INPUTS:      fp - Pointer to the file
 *              raw_data - Data to be written to trace file
 *              size     - Number of bytes in raw_data
 *              filename - Name of trace file, used to check need of new file
 *
 * OUTPUTS:     filename - Filename is changed if a new file was needed
 *
 * RTRN VALUE:  0 = trace entry was successfully logged
 *             -1 = unable to write to file
 *             -2 = unable to close file
 *             -3 = unable to open file
 *
 ******************************************************************************/
INT log_trace_entry( FILE **fp, pCHAR raw_data, INT size, pCHAR filename );


/******************************************************************************
 *
 * MODULE:	    create_trace_filename
 *
 * DESCRIPTION: This function creates a trace filename.  It formats the
 *              filename using the application name and using year, month
 *              and day as the extension:  app_name.YYYYMMDD
 *
 *              The directory path is included in the filename.
 *
 *              Example:
 *                 /ascendent/trace/txns/ncmcrd.20020317
 *
 * AUTHOR:      D. Irby
 *
 * INPUTS:      service_name - Name of application using this function
 *              path         - Directory to create the file in
 *
 * OUTPUTS:     filename - Name of file, including directory path
 *
 * RTRN VALUE:  None
 *
 ******************************************************************************/
void create_trace_filename( pCHAR service_name, pCHAR path, pCHAR filename );


/******************************************************************************
 *
 * MODULE:	    get_trace_timestamp
 *
 * DESCRIPTION: This function gets the system time and formats it as
 *              follows:  MMDDhhmmss:
 *
 *              A colon and space is placed at the end
 *
 * AUTHOR:      D. Irby
 *
 * INPUTS:      None
 *
 * OUTPUTS:     timestamp - Current system time in format MMDDhhmmss:
 *
 * RTRN VALUE:  None
 *
 ******************************************************************************/
void get_trace_timestamp( pCHAR timestamp );


/******************************************************************************
 *
 * MODULE:	    close_trace_file
 *
 * DESCRIPTION: This function closes a trace file.  The act of closing it also
 *              flushes the file's buffer.
 *
 * AUTHOR:      D. Irby
 *
 * INPUTS:      fp - Pointer to the file
 *
 * OUTPUTS:     None
 *
 * RTRN VALUE:  True  if file is successfully closed
 *              False if an error occurrred
 *
 ******************************************************************************/
INT close_trace_file( FILE **fp );


/******************************************************************************
 *
 *  NAME:         get_trace_ini_info
 *
 *  DESCRIPTION:  This function reads information from the monitor ini file.
 *
 *  INPUTS:       service_name - Name of service that wants to do tracing
 *                               Should not include directory path
 *
 *  OUTPUTS:      dir_path - Directory path for location of trace files
 *                err_buf  - Error message
 *
 *  RTRN VALUE:  -1 - Error
 *                0 - Service name is in the .ini file list
 *                1 - Service name is not in the .ini file list
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT get_trace_ini_info( pCHAR service_name, pCHAR dir_path, pCHAR err_buf );




/* TF INI File Name */
#define  TF_INI_FILE   "tf.ini"

/* INI File Section Names */
#define  TRACE_SECTION   "TRACES"

/* IDs of Services for Tracing */
typedef struct
{
   CHAR  trace_id[MAX_APP_NAME_SIZE];
}TRACE_IDS;





#endif
