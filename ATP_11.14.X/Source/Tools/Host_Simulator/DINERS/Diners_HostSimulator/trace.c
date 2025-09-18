#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "ptemsg.h"
#include "pteipc.h"
#include "ntutils.h"
#include "coreinfo.h"

#define MAX_CHARS_TO_DISPLAY  50

FILE    *fptr;
CHAR    log_dir[256];

CHAR    newline[] = "\n\0";


/*************************************************************/
/*************************************************************/
INT init_trace()
{

   sprintf( log_dir, "hostsim.log" );
    if ((fptr = fopen(log_dir,"w")) != NULL)
    {
        fputs("  \n", fptr);
        fclose(fptr);
        return 1;
    }
    else
        return 0;
}




/*************************************************************/
/*************************************************************/
pCHAR get_time()
{
    struct tm *newtime;

    time_t aclock;
    time( &aclock );                
    newtime = localtime( &aclock );
    return (asctime( newtime ));
}


/***********************************************************/
//    function write_to_comm_trace_file ()
//
//    This function will write the data buffer to the trace
//    file as a hex ascii dump.
//
/***********************************************************/
void write_to_comm_trace_file( pBYTE bufptr, INT len, BYTE msg_type )
{
   INT  numlines; 
   INT   line_count;
   INT   char_count;
   INT   col_index;
   INT   buf_index;
   CHAR tempstr [80];

   /*
      log the timestamp and message length.
   */
  if ((fptr = fopen(log_dir,"a+")) == NULL)
  {
    return;
  }
   sprintf (tempstr, "%s\n", get_time());

   fputs(tempstr, fptr);

   if( msg_type == '1' )
      sprintf (tempstr, "outgoing message length = %d\n", len);
   else
      sprintf (tempstr, "incoming message length = %d\n", len);

   fputs(tempstr, fptr);

   /*
      format every sixteen bytes of the data into a trace line.
      "xx xx xx xx xx xx xx xx - xx xx xx xx xx xx xx xx       [................]"
      this outer for-loop controls the line count.
   */
   buf_index = 0;
   numlines  = (len / 16);

   if ((len % 16) != 0)
      numlines++;

   for (line_count=0; line_count < numlines; line_count++)
      {
      /*
         this while loop will format the next sixteen bytes into
         the hex ascii digits on the left side of the trace line.
      */
      col_index  = 0;
      char_count = 0;

      while ( (char_count             < 16 ) &&
              ((buf_index+char_count) < len)    )
         {
         sprintf (&tempstr [col_index], "%02x ", bufptr[(buf_index+char_count)]);

         char_count++;
         col_index = col_index + 3;

         if (char_count == 8)
            {
            tempstr [col_index]     = '-';
            tempstr [col_index + 1] = ' ';
            col_index = col_index + 2;
            }
         }  /* while */


      /*
         this while loop will format the same sixteen bytes into the ascii
         text display on the right end of the trace line.  Non-printable
         characters are represented by a ".".
      */
      memset (&tempstr [col_index], ' ', sizeof(tempstr) - col_index - 1);
      tempstr [57] = '[';
      col_index  = 58;
      char_count = 0;
      
      while (char_count < 16)
         {
         if ((buf_index+char_count) < len) 
            {
            if ( (bufptr[(buf_index+char_count)] > 31 ) &&
                 (bufptr[(buf_index+char_count)] < 127)    )
               tempstr [col_index] = bufptr[(buf_index+char_count)];
            else
               tempstr [col_index] = '.';
            }

         col_index++;
         char_count++;
         }  /* while */

      /*
         finish the trace line by adding the closing bracket and a newline,
         then write it to the log.
      */
      tempstr [col_index  ] = ']';
      tempstr [col_index+1] = '\0';
      strcat (tempstr, newline);
      fputs(tempstr, fptr);
      buf_index = buf_index + 16;
      }  /* for line_count */

   fputs(newline, fptr);
   fclose(fptr);
}


