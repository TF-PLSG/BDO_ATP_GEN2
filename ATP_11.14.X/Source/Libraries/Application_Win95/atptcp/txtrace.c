/******************************************************************************

   FILE:         TXTRACE.C

   Copyright (c) 2002, Hypercom, Inc.
   All Rights Reserved.

   TITLE:        Transaction Tracing = Functions

   DESCRIPTION:  This file contains API functions associated with logging
                 transaction traces to disk files.

   APPLICATION:  Advanced Transaction Processor (ATP)

   REVISION HISTORY

   $Log:   N:\PVCS\PTE\Equitable\App_lib\txtrace.c  $
   
      Rev 1.4   Jan 16 2002 13:28:52   dirby
   Made a change in log_trace_entry to fix a bug where, if a new
   filename was being created, the code was putting 2 dots in 
   the filename, before the extension.
   SCR 546
   
      Rev 1.3   Jan 16 2002 11:51:52   dirby
   Modified function log_trace_entry to output ascii data instead of
   binary data.  This is for viewability purposes.
   SCR 546
   
      Rev 1.2   Jan 16 2002 09:21:28   dirby
   Changed function log_trace_entry to return negative numbers for errors.
   SCR 546
   
      Rev 1.1   Jan 15 2002 15:25:20   dirby
   Added 'get_trace_ini_info' function.
   SCR 546
   
      Rev 1.0   Jan 15 2002 11:23:42   dirby
   Functions that support the transaction tracing feature.
   SCR 546

 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "basictyp.h"
#include "ptetime.h"
#include "app_info.h"
#include "ntutils.h"
#include "txutils.h"
#include "genutil.h"
#include "diskio.h"
#include "txtrace.h"

#ifdef WIN32
   #ifndef _WINDOWS_
      #include <windows.h>
   #endif
#else
   #include <syslog.h>
   #include <string.h>
#endif

TRACE_IDS  Trace_Id[MAX_INI_ID_LIST_SIZE];

BYTE enc_on = 1;
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
INT open_trace_file( pCHAR service_name, pCHAR path, FILE **fp, pCHAR filename )
{
   /* Create the filename. */
   create_trace_filename( service_name, path, filename );

   /* Open the file, and return the result. */
   return( open_file(fp, filename, "a+b") );
}


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
INT process_encryption_flag( pPTE_MSG p_msg_in)
{
	BYTE flag = ptemsg_get_msg_subtype1(p_msg_in);
	if(flag == ST1_SRVC_ENCON)
		enc_on = 1;
	else if(flag == ST1_SRVC_ENCOFF)
		enc_on = 0;
	else
		return 0;
	send_trace_response(p_msg_in);
	return 0;
}


INT log_trace_entry( FILE **fp, pCHAR raw_data, INT size, pCHAR filename )
{
   #ifdef WIN32
      #define DIR_SLASH '\\'
   #else
      #define DIR_SLASH '/'
   #endif

   INT   ret_val = 0;
   INT   len;
   CHAR  time_date[25]  ="";
   CHAR  timestamp[14]  ="";
   CHAR  current_mmdd[5]="";
   CHAR  service_name[MAX_APP_NAME_SIZE];
   CHAR  path[MAX_APP_NAME_SIZE];
   pCHAR tmp_ptr = NULL_PTR;
   WORD key[] = {0x13A7, 0x6135, 0x9CDF, 0xA852};

   /* Define a buffer to hold raw data in ascii format.
    * Converting from binary to ascii increases size by 2.
    * No need to initialize here; it is initialized in conversion function.
    */
   CHAR  ascii_buf[MAX_TRACE_ENTRY_SIZE * 2];

   /* This buffer holds ascii_buf plus the header. */
   CHAR  outbuf[(MAX_TRACE_ENTRY_SIZE * 2) + 12]="";


   /* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
   ptetime_get_timestamp( time_date );

   /* Pick out the month and day to compare to filename extenstion. */
   memcpy( current_mmdd,   time_date+5, 2 );
   memcpy( current_mmdd+2, time_date+8, 2 );

   /* Compare filename extension to current date. */
   len = strlen(filename);
   if ( 0 != strcmp(&filename[len-4], current_mmdd) )
   {
      /* They are not the same.  We have rolled over to a new day.
       * Close current file, and create a new one.
       *
       * First close the currently opened file.
       * --------------------------------------
       */
      if ( false == close_trace_file(fp) )
         ret_val = -2;
      else
      {
         /* Now create the new filename.
          * ----------------------------
          */
         memset( service_name, 0x00, MAX_APP_NAME_SIZE );
         memset( path,         0x00, MAX_APP_NAME_SIZE );
         tmp_ptr = strrchr( filename, DIR_SLASH );
         if ( tmp_ptr == NULL_PTR )
         {
            /* No slash found. */
            memcpy( service_name, filename, len-9 );
         }
         else
         {
            tmp_ptr++; /* Point past the slash. */
            memcpy( path, filename, tmp_ptr-filename );
            len = strlen( tmp_ptr );
            memcpy( service_name, tmp_ptr,len-9 );
         }
         create_trace_filename( service_name, path, filename );

         /* Now create/open the new file.
          * -----------------------------
          */
         if ( false == open_trace_file(service_name, path, fp, filename) )
            ret_val = -3;
      }
   }

   /*+---------------------------------+
     | Log the entry to the disk file. |
     +---------------------------------+*/

   if ( ret_val == 0 )
   {
      /* First timestamp the log entry.
       * ------------------------------
       */
      get_trace_timestamp( timestamp );
      strcpy( outbuf, timestamp );

      /* Next, convert raw data into ascii - for viewing purposes.
       * ---------------------------------------------------------
       */
      genutil_bin_to_ascii((pBYTE)raw_data, size, ascii_buf, sizeof(ascii_buf));

      /* Next concatenate timestamp and raw data.
       * ----------------------------------------
       */
       //Log Encryption
      if(enc_on == 1)
	   {
		  if(0 == des_encryption_ex1( (unsigned char *)(ascii_buf), strlen(ascii_buf), key, 1 ))
		  {
			//error in log encryption.
			printf("Cannot encrypt the txn\n");
		  }
	   }

	  len = strlen( timestamp );
      //size *= 2;	//Encrypted log size might be larger than actual ascii string.
      size = strlen(ascii_buf);
	  
		if(size > (MAX_TRACE_ENTRY_SIZE*2) )
		{
			printf("size exceeding buffer. %s > %s\n", size,(MAX_TRACE_ENTRY_SIZE*2) );
			return 0;
		}
	  
      memcpy( outbuf+len, ascii_buf, size );

      /* Append a newline character for reading purposes.
       * ------------------------------------------------
       */
        
      outbuf[size+len] = '\n';

      /* Increase size of data to be written to account for time and new line.
       * ---------------------------------------------------------------------
       */
      size += len + 1;

      /* Write the transaction to file.
       * ------------------------------
       */
      if ( false == fwrite_file(fp, outbuf, size) )
         ret_val = -1;
   }

   return( ret_val );
}


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
void create_trace_filename( pCHAR service_name, pCHAR path, pCHAR filename )
{
   CHAR  system_date[25] = "";

   /* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
   ptetime_get_timestamp( system_date );

   strcpy( filename, path );
   strcat( filename, service_name );
   strcat( filename, "." );
   strncat(filename, system_date,   4 );  /* YYYY */
   strncat(filename, system_date+5, 2 );  /* MM   */
   strncat(filename, system_date+8, 2 );  /* DD   */

   return;
}


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
void get_trace_timestamp( pCHAR timestamp )
{
   CHAR    time_date[25];

   /* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
   ptetime_get_timestamp( time_date );

   /* Format properly. */
   memcpy( timestamp,   time_date+5,  2 );   /* MM */
   memcpy( timestamp+2, time_date+8,  2 );   /* DD */
   memcpy( timestamp+4, time_date+11, 2 );   /* hh */
   memcpy( timestamp+6, time_date+14, 2 );   /* mm */
   memcpy( timestamp+8, time_date+17, 2 );   /* ss */
   strcat( timestamp, ": " );

   return;
}



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
INT close_trace_file( FILE **fp )
{
   return( close_file(fp) );
}


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
INT get_trace_ini_info( pCHAR service_name, pCHAR dir_path, pCHAR err_buf )
{
   INT   ret_code;
   INT   num_of_ids;
   INT   size;
   CHAR  sectionname  [MAX_APP_NAME_SIZE] = "";
   CHAR  filename     [MAX_APP_NAME_SIZE];
   CHAR  tmpstr       [MAX_APP_NAME_SIZE];
   CHAR  directory    [MAX_APP_NAME_SIZE];
   INT   i;


   /* Determine directory and filename of the .ini file. */
   memset( tmpstr, 0x00, MAX_APP_NAME_SIZE );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, TF_INI_FILE);

   /*----------------------------------------*/
   /* Get List of Services in the Trace list */
   /*----------------------------------------*/
   size = sizeof(TRACE_IDS);
   memset( &Trace_Id, 0x00, sizeof(Trace_Id) );
   num_of_ids = GetPrivateProfileList( filename, TRACE_SECTION,
                                       (pCHAR)&Trace_Id, size );
   if ( num_of_ids < 0 )
   {
      strcpy( sectionname, TRACE_SECTION );
      ret_code = -1;
      if ( num_of_ids == -1 )
      {
         sprintf( err_buf,
                 "Unable to open %s.  Name and path: %s",
                  TF_INI_FILE, filename );
      }
      else if ( num_of_ids == -2 )
      {
         sprintf( err_buf,
                 "Opened file %s, but cannot file section name %s",
                  TF_INI_FILE, TRACE_SECTION );
      }
      else if ( num_of_ids == -3 )
      {
         sprintf( err_buf,
                 "Unable to copy list of services into memory. Section %s",
                  TRACE_SECTION );
      }
   }
   else
   {
      /* Check to see if service is in the list. */
      if ( num_of_ids == 0 )
         ret_code = 1;
      else
      {
         ret_code = 1;
         for(i=0; i< num_of_ids;i++)
         {
            if ( 0 == strcmp(service_name, Trace_Id[i].trace_id))
            {
               /* It is in the list.  Turn tracing ON. */
               ret_code = 0;
               break;
            }
         }

         if ( ret_code == 0 )
         {
            /* Only need to get directory path if tracing is ON. */
            strcpy( sectionname, TRACE_SECTION );
            memset( directory, 0x00, sizeof(directory) );
            ret_code = GetPrivateProfileString(
                   sectionname,               /* points to section name       */
                  "Directory",                /* points to key name           */
                   "",                        /* points to default string     */
                   directory,                 /* points to destination buffer */
                   sizeof(directory) - 1,     /* size of destination buffer   */
                   filename                   /* points to ini filename       */
            );

            if ( ret_code <= 0 )
            {
               ret_code = -1;
               sprintf( err_buf,
                       "Error while getting Directory path from %s in %s file",
                        TRACE_SECTION, TF_INI_FILE );
            }
            else
            {
               if ( 0 != strcmp(directory, "") )
               {
                  strcpy( dir_path, directory );
                  ret_code = 0;
               }
               else
               {
                  ret_code = -1;
                  sprintf( err_buf,
                          "Cannot find Directory path in %s section of %s file",
                           TRACE_SECTION, TF_INI_FILE );
               }
            }
         }
      }
   }
   return( ret_code );
}

