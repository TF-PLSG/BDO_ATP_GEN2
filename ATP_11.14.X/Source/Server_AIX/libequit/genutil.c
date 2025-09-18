/*****************************************************************************
*  
* MODULE:      GENUTIL.C
*  
* TITLE:       ATP Standard - Utilities.
*  
* DESCRIPTION: General purpose utilities for use by any application.
*
* APPLICATION: multiple
*
* AUTHOR:  BGABLE
*
* REVISION HISTORY:
*
* $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\App_lib\Source\GENUTIL.C  $
   
      Rev 1.13   Jun 10 2005 16:12:48   dirby
   Added a function to copy raw EMV data into the Auth Tx structure.
   SCR 15725
   
      Rev 1.12   Jun 01 2005 17:00:30   dirby
   1.  Added function to check for a valid 2-char US State Abbreviation.
   2.  Changed the Country Code function to initialize it first time through
       so that on subsequent passes, that big structure does not need
       to be repopulated for each transaction.
   SCR 15607
   
      Rev 1.11   May 20 2005 08:21:38   dirby
   Added country code conversion for 3-char country codes to 3-digit
   country codes.
   SCR 15607
   
      Rev 1.10   Mar 10 2005 17:11:38   dirby
   Added function genutil_asc_to_bin for EMV applications.
   SCR 14238
   
      Rev 1.9   Nov 19 2004 14:41:20   dirby
   Fixed a bug in the function that converts Base64 to Binary.  The
   bug was not converting the last byte unless it was 'A' or '=', in which
   case it didn't matter.
   SCR 13183
   
      Rev 1.8   Oct 08 2004 11:57:46   dirby
   Fixed a bug in converting country code - the 'while' loop would not
   exit if the country code was found.
   SCR 12462
   
      Rev 1.7   Oct 07 2004 14:50:08   dirby
   Added function genutil_get_country_code, a function to convert
   country codes between alpha and numeric.
   SCR 12462
   
      Rev 1.6   Jul 21 2004 16:55:42   dirby
   Added function to find a specific subfield within the raw data of
   an EMV block - finds an EMV tag and returns its value and length.
   SCR 1388
   
      Rev 1.5   Jan 13 2004 13:58:48   dirby
   Added functions to convert Base64 to binary and to convert
   binary into BCD.
   SCR 1117
   
      Rev 1.4   Dec 29 1999 15:33:58   gbarnes
   changed case on include files and changed
   to c style comments.
   
      Rev 1.3   Nov 16 1999 14:13:16   mbysani
    
   
      Rev 1.2   Nov 16 1999 11:19:10   mbysani
   Added 3 functions
   
      Rev 1.2   Nov 16 1999 09:50:38   mbysani
    
   
      Rev 1.1   Feb 02 1999 15:36:02   IGOLDFIE
   Changed include files
   
      Rev 1.0   01 Feb 1999 16:24:30   jhapitas
   Initial Revision
   
      Rev 1.0   Dec 04 1998 15:58:16   MSALEH
    
*
*******************************************************************************/

#include <stdio.h>
#include <time.h>
#include <sys/timeb.h>
#include <string.h>
#include <math.h>

#include "basictyp.h"
#include "pteipc.h"
#include "ntutils.h"
#include "app_info.h"
#include "equitdb.h"
#include "dc_dbstruct.h"
#include "tx_dbstruct.h"
#include "nc_dbstruct.h"
#include "genutil.h"  /* Includes all function prototypes for generic utilities */
#include "memmnger.h"


extern  CHAR    oprmon_que_name[];
PRIVATE CHAR    applnk_que_name[] = "applnkA";


/* Country Codes */
CHAR cc_table[240][11];
INT  cc_table_initialized = false;
void init_country_codes();

/* State Abbreviations */
CHAR st_table[62][3];
INT  st_table_initialized = false;
void init_state_codes();

BYTE      a2e[257];
BYTE      e2a[257];
BOOLEAN   maps_initialised = false;

#ifdef	_DEBUG
	#define PRINT(x) printf(x) 
#else	
	#define PRINT
#endif  

/*******************************************************************************************************/
/* ROUTINES                                                                                            */
/*******************************************************************************************************/

/*****************************************************************************

  Function:   genutil_asc_to_bcd

  Description: 
      convert ascii string to bcd string
  Author: 
      unknown
  Inputs:   
      afield   - pointer to the incoming ascii string to be converted
      how_many - number to convert
  Outputs: 
      bstring  - outgoing bcd string
  Return values:
      None
  Modified by:
      BGable      6/26/98
******************************************************************************/
extern void genutil_asc_to_bcd (pCHAR afield, INT how_many, BYTE bstring[])
{
   pCHAR astr;
   UINT  i = 0;


   astr = afield;

   for (i = (strlen(afield) - how_many); i < strlen(afield); i=i+2)
      {
      /* if I hit a null terminator, quit.  */
      if (astr[i] == '\0')
         break;

      /* if these are leading blanks, leave the bcd zero.  */
      if (astr[i] != ' ')
         bstring[i/2] = ((astr[i]-'0')*16);

      if ( ((i+1) < strlen(afield)) &&
           (astr[i+1] != ' ')          )
         bstring[i/2] += (astr[i+1]-'0');
      }
} /* genutil_asc_to_bcd */

/*****************************************************************************

  Function:   genutil_asc_to_vbcd

  Description: 
      convert ascii string to bcd string
  Author: 
      unknown
  Inputs:
      afield   - pointer to the incoming ascii string to be converted
      fld_len  - number to convert
  Outputs:
      bcd_buf  - outgoing bcd string
  Return values:
      None
  Modified by:
      BGable      6/26/98
******************************************************************************/
extern void genutil_asc_to_vbcd (pCHAR afield, INT fld_len, BYTE bcd_buf[])
{
   pCHAR astr;
   INT   i = 0;
   

   astr = afield;

   for (i = 0; i < fld_len; i=i+2)
      {
      /* if I hit a null terminator, quit.  */
      if (astr[i] == '\0')
         break;

      bcd_buf [i/2] = ((astr[i]-'0')*16);

      if ((i+1) < fld_len)
         bcd_buf [i/2] += (astr[i+1]-'0');
      else
         bcd_buf [i/2] += 0x0f;
      }
} /* genutil_asc_to_vbcd */

/*****************************************************************************

  Function:   genutil_bcd_to_asc

  Description:  
      convert bcd/binary string to ascii string
  Author: 
      unknown
  Inputs:
      bstring  - incoming bcd string
      blen     - length of bstring
      astr_len - length of astr
  Outputs:
      astr     - outgoing ascii character string
  Return values:
      None
  Modified by:
      BGable      6/26/98
******************************************************************************/
extern void genutil_bcd_to_asc (BYTE bstring[], INT blen, pCHAR astr, INT astr_len)
{
   INT  i = 0;

   memset (astr, 0x00, astr_len);

   for (i = 0; i < (blen * 2); i=i+2)
      {
      astr[i] = (bstring[i/2] / 16) + '0';
      astr[i+1] = (bstring[i/2] & 0x0F) + '0';
      }
} /* genutil_bcd_to_asc */

/*****************************************************************************

  Function:   genutil_bcd_to_int

  Description:
      Convert a bcd string into an integer value.
  Author: 
      unknown
  Inputs:
      p_bcd_data  -  pointer to the incoming bcd string
      bcd_len     -  length of the bcd string
  Outputs:
  Return values:
      INT         -  integer value of the bcd string
  Modified by:
      BGable      6/26/98
******************************************************************************/
extern INT genutil_bcd_to_int (pBYTE  p_bcd_data, INT bcd_len)
{
   INT   temp_int;
   INT   multiplier;
   INT   i;


   multiplier = 1;
   temp_int   = 0;
   
   for (i = (bcd_len-1); i >= 0; i--)
      {
      temp_int+= (p_bcd_data [i] & 0x0f) * multiplier;
      multiplier = multiplier * 10;
      temp_int+= (p_bcd_data [i] / 16) * multiplier;
      multiplier = multiplier * 10;
      }

   return (temp_int);
} /*genutil_bcd_to_int */

/*****************************************************************************

  Function:   genutil_bin_to_ascii

  Description:
      Convert binary string to ascii string
  Author: 
      JHapitas   5/6/98
  Inputs:
      bstring  - incoming binary string
      blen     - length of bstring
      astr_len - length of astr
  Outputs:
      astr     - pointer to the outgoing ascii string
  Return values:
  Modified by:
      BGable      6/26/98
******************************************************************************/
/*****************************************
   convert binary string to ascii string     JH May 06  New Procedure
*****************************************/
extern void genutil_bin_to_ascii (BYTE bstring[], INT blen, pCHAR astr, INT astr_len)
{
   INT  i = 0;

   memset (astr, 0x00, astr_len);

   for (i = 0; i < (blen * 2); i+=2)
   {
      if ((bstring[i/2] / 16) <= 9) 
        astr[i] = (bstring[i/2] / 16) + '0';
      else
        astr[i] = (bstring[i/2] / 16 - 0x0a)+'A';

      if ((bstring[i/2] & 0x0f) <= 9)
        astr[i+1] = (bstring[i/2] & 0x0F) + '0'; 
      else
        astr[i+1] = (bstring[i/2] & 0x0F) - 0x0a + 'A';    
   }
} /* genutil_bin_to_ascii */

/*****************************************************************************

  Function:   genutil_blank_string

  Description:
      This function determines if an input string it blank.
  Author: 
      unknown
  Inputs:
      astr     - incoming string
      max_len  - maximum length of the incoming string 
  Outputs:
      None
  Return values:
      true     - the incoming string is blank
      false    - the incoming string is not blank
  Modified by:
      BGable      6/26/98
******************************************************************************/
extern BOOLEAN genutil_blank_string(pCHAR astr, INT max_len)
{
   INT i;
   for(i = 0;i < max_len; i++)
   {
      if (astr[i] != ' ')
         return(false);
   }
   return(true);    
} /* genutil_blank_string */

void init_character_code_maps()
{
   // ascii to ebcdic map
   memset( a2e, 0, sizeof( a2e ) );

   a2e[0x30]=0xF0; a2e[0x31]=0xF1; a2e[0x32]=0xF2; a2e[0x33]=0xF3; a2e[0x34]=0xF4;
   a2e[0x35]=0xF5; a2e[0x36]=0xF6; a2e[0x37]=0xF7; a2e[0x38]=0xF8; a2e[0x39]=0xF9;
   a2e[0x61]=0x81; a2e[0x62]=0x82; a2e[0x63]=0x83; a2e[0x64]=0x84; a2e[0x65]=0x85;
   a2e[0x66]=0x86; a2e[0x67]=0x87; a2e[0x68]=0x88; a2e[0x69]=0x89; a2e[0x6A]=0x91;
   a2e[0x6B]=0x92; a2e[0x6C]=0x93; a2e[0x6D]=0x94; a2e[0x6E]=0x95; a2e[0x6F]=0x96;
   a2e[0x70]=0x97; a2e[0x71]=0x98; a2e[0x72]=0x99; a2e[0x73]=0xA2; a2e[0x74]=0xA3;
   a2e[0x75]=0xA4; a2e[0x76]=0xA5; a2e[0x77]=0xA6; a2e[0x78]=0xA7; a2e[0x79]=0xA8;
   a2e[0x7A]=0xA9; a2e[0x41]=0xC1; a2e[0x42]=0xC2; a2e[0x43]=0xC3; a2e[0x44]=0xC4;
   a2e[0x45]=0xC5; a2e[0x46]=0xC6; a2e[0x47]=0xC7; a2e[0x48]=0xC8; a2e[0x49]=0xC9;
   a2e[0x4A]=0xD1; a2e[0x4B]=0xD2; a2e[0x4C]=0xD3; a2e[0x4D]=0xD4; a2e[0x4E]=0xD5;
   a2e[0x4F]=0xD6; a2e[0x50]=0xD7; a2e[0x51]=0xD8; a2e[0x52]=0xD9; a2e[0x53]=0xE2;
   a2e[0x54]=0xE3; a2e[0x55]=0xE4; a2e[0x56]=0xE5; a2e[0x57]=0xE6; a2e[0x58]=0xE7;
   a2e[0x59]=0xE8; a2e[0x5A]=0xE9; a2e[0x20]=0x40; a2e[0x2E]=0x4B; a2e[0x3C]=0x4C;
   a2e[0x28]=0x4D; a2e[0x2B]=0x4E; a2e[0x7C]=0x4F; a2e[0x26]=0x50; a2e[0x21]=0x5A;
   a2e[0x24]=0x5B; a2e[0x2A]=0x5C; a2e[0x29]=0x5D; a2e[0x3B]=0x5E; a2e[0x2D]=0x60;
   a2e[0x2F]=0x61; a2e[0x2C]=0x6B; a2e[0x25]=0x6C; a2e[0x5F]=0x6D; a2e[0x3E]=0x6E;
   a2e[0x3F]=0x6F; a2e[0x3A]=0x7A; a2e[0x23]=0x7B; a2e[0x40]=0x7C; a2e[0x27]=0x7D;
   a2e[0x3D]=0x7E; a2e[0x22]=0x7F;

   // ebcdic to ascii map
   memset( e2a, 0, sizeof( e2a ) );

   e2a[0xF0]=0x30; e2a[0xF1]=0x31; e2a[0xF2]=0x32; e2a[0xF3]=0x33; e2a[0xF4]=0x34;
   e2a[0xF5]=0x35; e2a[0xF6]=0x36; e2a[0xF7]=0x37; e2a[0xF8]=0x38; e2a[0xF9]=0x39;
   e2a[0x81]=0x61; e2a[0x82]=0x62; e2a[0x83]=0x63; e2a[0x84]=0x64; e2a[0x85]=0x65;
   e2a[0x86]=0x66; e2a[0x87]=0x67; e2a[0x88]=0x68; e2a[0x89]=0x69; e2a[0x91]=0x6A;
   e2a[0x92]=0x6B; e2a[0x93]=0x6C; e2a[0x94]=0x6D; e2a[0x95]=0x6E; e2a[0x96]=0x6F;
   e2a[0x97]=0x70; e2a[0x98]=0x71; e2a[0x99]=0x72; e2a[0xA2]=0x73; e2a[0xA3]=0x74;
   e2a[0xA4]=0x75; e2a[0xA5]=0x76; e2a[0xA6]=0x77; e2a[0xA7]=0x78; e2a[0xA8]=0x79;
   e2a[0xA9]=0x7A; e2a[0xC1]=0x41; e2a[0xC2]=0x42; e2a[0xC3]=0x43; e2a[0xC4]=0x44;
   e2a[0xC5]=0x45; e2a[0xC6]=0x46; e2a[0xC7]=0x47; e2a[0xC8]=0x48; e2a[0xC9]=0x49;
   e2a[0xD1]=0x4A; e2a[0xD2]=0x4B; e2a[0xD3]=0x4C; e2a[0xD4]=0x4D; e2a[0xD5]=0x4E;
   e2a[0xD6]=0x4F; e2a[0xD7]=0x50; e2a[0xD8]=0x51; e2a[0xD9]=0x52; e2a[0xE2]=0x53;
   e2a[0xE3]=0x54; e2a[0xE4]=0x55; e2a[0xE5]=0x56; e2a[0xE6]=0x57; e2a[0xE7]=0x58;
   e2a[0xE8]=0x59; e2a[0xE9]=0x5A; e2a[0x40]=0x20; e2a[0x4B]=0x2E; e2a[0x4C]=0x3C;
   e2a[0x4D]=0x28; e2a[0x4E]=0x2B; e2a[0x4F]=0x7C; e2a[0x50]=0x26; e2a[0x5A]=0x21;
   e2a[0x5B]=0x24; e2a[0x5C]=0x2A; e2a[0x5D]=0x29; e2a[0x5E]=0x3B; e2a[0x60]=0x2D;
   e2a[0x61]=0x2F; e2a[0x6B]=0x2C; e2a[0x6C]=0x25; e2a[0x6D]=0x5F; e2a[0x6E]=0x3E;
   e2a[0x6F]=0x3F; e2a[0x7A]=0x3A; e2a[0x7B]=0x23; e2a[0x7C]=0x40; e2a[0x7D]=0x27;
   e2a[0x7E]=0x3D; e2a[0x7F]=0x22;

   maps_initialised = true;

} /* init_character_code_maps */

void convert_to_ascii( pBYTE source, pBYTE dest, INT len )
{
   INT  temp;
   BYTE temp_byte;

   if( !maps_initialised )
      init_character_code_maps();

   for( temp = 0; temp < len; temp++ )
   {
      temp_byte = *(source + temp);
      *(dest + temp) = e2a[temp_byte];
   }

   return;
} /* convert_to_ascii */

void convert_to_ebcdic( pBYTE source, pBYTE dest, INT len )
{
   INT temp;
   BYTE temp_byte;

   if( !maps_initialised )
      init_character_code_maps();

   for( temp = 0; temp < len; temp++ )
   {
      temp_byte = *(source + temp);
      *(dest + temp) = a2e[temp_byte];
   }

   return;
} /* convert_to_ebcdic */
void hex_to_ebcdic( pBYTE instr, pBYTE outstr, INT len )
{
   INT   i, j, outstr_idx;
   BYTE  temp_byte;

   BYTE  hex_chars[16] =   { 0x00, 0x01, 0x02, 0x03,
                             0x04, 0x05, 0x06, 0x07,
                             0x08, 0x09, 0x0a, 0x0b,
                             0x0c, 0x0d, 0x0e, 0x0f };

   BYTE  ascii_chars[16] = {  '0',  '1',  '2',  '3',
                              '4',  '5',  '6',  '7',
                              '8',  '9',  'A',  'B',
                              'C',  'D',  'E',  'F' };

   /* Loop through the input string for each byte. */
   for ( i=0,outstr_idx=0; i<len; i++,outstr_idx += 2 )
   {
      /* Convert upper half of byte to ASCII. */
      temp_byte = instr[i] & 0xf0;

      /* Shift the bits 4 positions to the right. */
      temp_byte >>= 4;

      /* Find the ASCII equivalent. */
      for ( j=0; j<16; j++ )
      {
         if ( hex_chars[j] == temp_byte )
         {
            outstr[outstr_idx] = ascii_chars[j];
            break;
         }
      }

      /* Convert the lower half of byte to ASCII. */
      temp_byte = instr[i] & 0x0f;

      /* Find the ASCII equivalent. */
      for ( j=0; j<16; j++ )
      {
         if ( hex_chars[j] == temp_byte )
         {
            outstr[outstr_idx+1] = ascii_chars[j];
            break;
         }
      }
   } /* End outer loop */

   /* Outstr is now in ASCII.  Now convert it to EBCDIC.
    * Len has doubled in size because it went from hex to ASCII.
    */
   convert_to_ebcdic( outstr, outstr, len*2 );

   return;
}

void ebcdic_to_hex( pBYTE instr, pBYTE outstr, INT len )
{
   #define  MAX_LEN 100

   INT   i, j, outstr_idx;
   BYTE  temp_byte;
   BYTE  temp_str[MAX_LEN] = "";

   BYTE  hex_chars[22] =   { 0x00, 0x01, 0x02, 0x03,
                             0x04, 0x05, 0x06, 0x07,
                             0x08, 0x09, 0x0a, 0x0b,
                             0x0c, 0x0d, 0x0e, 0x0f,
                             0x0A, 0x0B, 0x0C, 0x0D,
                             0x0E, 0x0F };

   BYTE  ascii_chars[22] = {  '0',  '1',  '2',  '3',
                              '4',  '5',  '6',  '7',
                              '8',  '9',  'a',  'b',
                              'c',  'd',  'e',  'f',
                              'A',  'B',  'C',  'D',
                              'E',  'F' };

   /* Convert from EBCDIC into ASCII. */
   if ( len > MAX_LEN )
      len = MAX_LEN;

   convert_to_ascii( instr, temp_str, len );

   /* Loop through the input string for each byte. */
   for ( i=0,outstr_idx=0; i<len; i+=2,outstr_idx++ )
   {
      /* Get upper half of hex byte. */
      temp_byte = temp_str[i];
      for ( j=0; j<22; j++ )
      {
         if ( ascii_chars[j] == temp_byte )
         {
            outstr[outstr_idx] = hex_chars[j];

            /* Move to upper nibble. */
            outstr[outstr_idx] <<= 4;
            break;
         }
      }

      /* Get lower half of hex byte. */
      temp_byte = temp_str[i+1];
      for ( j=0; j<22; j++ )
      {
         if ( ascii_chars[j] == temp_byte )
         {
            outstr[outstr_idx] |= hex_chars[j];
            break;
         }
      }
   } /* End outer loop */

   return;
}

/*****************************************************************************

  Function:   genutil_format_date

  Description:  ?????????????????
  Author: 
      unknown
  Inputs:
      temp 
  Outputs:
  Return values:
      None
  Modified by:
      BGable      6/26/98
******************************************************************************/
extern void genutil_format_date(pCHAR temp)
{
   struct timeb   time_structure;   /* Time for ftime */
   time_t long_time_since_1970;        /* Number of seconds since 01/01/1970 A.D. 12:00 A.M.*/
   struct tm *current_time;            /* Current time structure*/
   CHAR   temp1[4] = "0";

   ftime(&time_structure);
   long_time_since_1970 = time_structure.time;
   current_time = localtime(&long_time_since_1970);
   current_time->tm_mon++;
   if (current_time->tm_year >= 70)
      current_time->tm_year += 1900;
   else
      current_time->tm_year += 2000;
   itoa(current_time->tm_year, temp, 10);
   itoa(current_time->tm_mon, temp1 + 1, 10);
   if(strlen(temp1) == 2)
      memcpy(temp + 4, temp1, 2);
   else
      memcpy(temp + 4, temp1 + 1, 2);
   memset(temp1, 0, sizeof(temp1));
   temp1[0] = '0';
   itoa(current_time->tm_mday, temp1 + 1, 10);
   if(strlen(temp1) == 2)
      memcpy(temp + 6, temp1, 2);
   else
      memcpy(temp + 6, temp1 + 1, 2);
   memset(temp1, 0, sizeof(temp1));
   temp1[0] = '0';
   itoa(current_time->tm_hour, temp1 + 1, 10);
   if(strlen(temp1) == 2)
      memcpy(temp + 8, temp1, 2);
   else
      memcpy(temp + 8, temp1 + 1, 2);
   memset(temp1, 0, sizeof(temp1));
   temp1[0] = '0';
   itoa(current_time->tm_min, temp1 + 1, 10);
   if(strlen(temp1) == 2)
      memcpy(temp + 10, temp1, 2);
   else
      memcpy(temp + 10, temp1 + 1, 2);
   memset(temp1, 0, sizeof(temp1));
   temp1[0] = '0';
   itoa(current_time->tm_sec, temp1 + 1, 10);
   if(strlen(temp1) == 2)
      memcpy(temp + 12, temp1, 2);
   else
      memcpy(temp + 12, temp1 + 1, 2);

} /* genutil_format_date */

/**************************************************************************************************
*
* FUNCTION:     genutil_get_strtime
*
* DESCRIPTION:  Function sets transaction start time
*               
*
* PARAMETERS:   pCHAR
*
* RETURN:       NONE
*
**************************************************************************************************/
extern void genutil_get_strtime ( pCHAR deststr )
{
   time_t seconds;

   seconds = time(NULL_PTR);        /* get the current date and time in seconds */
   itoa(seconds, deststr, 10);    /* convert the long integer to a string */
} /* genutil_get_strtime */

/*****************************************************************************

  Function:   genutil_hex_to_bin

  Description: 
      This function will deocde the hex characters into an integer.
  Author: 
      unknown
  Inputs:
      afield   - pointer to the ascii hex digits ('0' to '9', 'A' to 'F').
                 this buffer is not assumed, nor expected, to be a null
                 terminated string.
      how_many - number of digits in the buffer.
  Outputs:
      bstring  -     returns the integer decode result.
  Return values:
      None
  Modified by:
      BGable      6/26/98
******************************************************************************/
extern void genutil_hex_to_bin (pCHAR afield, INT how_many, BYTE bstring[])
{
   pCHAR   astr;
   INT     index;


   astr = afield;

   for (index = 0; index < how_many; index=index+2) 
      {
      if ( (astr [index] >= '0')  &&  (astr [index] <= '9') )
         bstring [index/2] = ((INT) (astr [index] - '0')) * 16;
      else
         if ( (astr [index] >= 'A')  &&  (astr [index] <= 'F') )
            bstring [index/2] = ((INT) (astr [index] - 'A') + 10) * 16;

      if ( (astr [index+1] >= '0')  &&  (astr [index+1] <= '9') )
         bstring [index/2]+= ((INT) (astr [index+1] - '0'));
      else
         if ( (astr [index+1] >= 'A')  &&  (astr [index+1] <= 'F') )
            bstring [index/2]+= ((INT) (astr [index+1] - 'A') + 10);
      } /* end of for*/
} /* genutil_hex_to_bin */

/*****************************************************************************

  Function:   genutil_hex_to_int

  Description:
      This function will deocde the hex characters into an integer.
  Author: 
      unknown
  Inputs:
      buf_ptr  - pointer to the ascii hex digits ('0' to '9', 'A' to 'F').
                 this buffer is not assumed, nor expected, to be a null
                 terminated string.
      buf_len  - number of digits in the buffer.
  Outputs:
      None
  Return values:
      INT   -  the integer decode result.
  Modified by:
      BGable      6/26/98
******************************************************************************/
extern INT genutil_hex_to_int (pCHAR buf_ptr, INT buf_len  )
{
   INT     index;
   INT     n;
  
   n=0;

   for (index = 0; index < buf_len; index++, buf_ptr++) 
      {
      if (*buf_ptr >= '0'  &&  *buf_ptr <= '9')
         n = (n * 16) + ((INT) (*buf_ptr - '0'));
      else
         if (*buf_ptr >= 'A'  &&  *buf_ptr <= 'F')
            n = (n * 16) + ((INT) (*buf_ptr - 'A') + 10);
         else
            return (-1);  /* error*/
      } /* end of for*/

  return (n);
} /* genutil_hex_to_int */
/*****************************************************************************

  Function:   genutil_minim

  Description:  Determine the minimum value of two integer inputs
  Author: 
      unknown
  Inputs:
      a  -  an integer to be compared to 'b'
      b  -  an integer to be compared to 'a'
  Outputs:
      None
  Return values:
      INT - the integer that has the least value
  Modified by:
      BGable      6/26/98
******************************************************************************/
extern INT genutil_minim (INT a, INT b)
{
   if (a > b)
      return(b);
   else
      return(a);
}  /* end genutil_min */ 


/*****************************************************************************

  Function:   genutil_opermon_update

  Description: This function will output a message to the operator monitor.
  Author: 
      unknown
  Inputs:
      msg_id   - ID number
      msg      - The error message string
      app_name - The name of the application sending out the message
      etype    - "WARNING", "Informational", "Error"
      severity -
  Outputs:
      None
  Return values:
      None
  Modified by:
      BGable      6/30/98
******************************************************************************/
extern void genutil_opermon_update(pCHAR msg_id, pCHAR msg, pCHAR app_name, pCHAR etype, pCHAR severity)
{
   pPTE_MSG         p_msg_out = NULL_PTR;
   LONG             ret_code = 0L;
   INT              nIndex = 0;
   OPERATOR_MESSAGE oper_msg;
   CHAR             error_msg[200] = "";
   CHAR             temp[256] = "";

   memset(&oper_msg, 0, sizeof(OPERATOR_MESSAGE));
   strcpy(oper_msg.message_id, msg_id);
   strcpy(oper_msg.message, msg);
   strcpy(oper_msg.application_name, app_name);
   strcpy(oper_msg.type, etype);
   strcpy(oper_msg.severity, severity);

   p_msg_out = ptemsg_build_msg(MT_OPERATOR, 0, 0, oprmon_que_name,
                                application_que_name, (pBYTE)&oper_msg, sizeof(OPERATOR_MESSAGE), 0);
   ret_code = pteipc_send(p_msg_out, oprmon_que_name);
   if(ret_code < 0)
   {
      pteipc_get_errormsg( ret_code, error_msg);
      sprintf(temp, "Error on pteipc_send: %s", error_msg);
      if(p_msg_out != NULL_PTR)
         free(p_msg_out);
      return;
   }
   else
   {
      if(p_msg_out != NULL_PTR)
         free(p_msg_out);
      return;
   }
} /* genutil_opermon_update */

/*****************************************************************************

  Function:   genutil_process_error

  Description: Process Terminal Handler errors. This function will process all Terminal
               Handler error types: error reporting to the console window under debugging,
               report errors to the NT event log, and displaying errors to the terminal
               display via the thutil_abort_tran function.
  Author: 
      unknown
  Inputs:
      func_name      - The name of the function calling this function
      err_code_str   - The error message string
      err_msg        - ??
      tid            - Terminal ID
      disp_abort1    - ??
      disp_abort2    - ??
  Outputs:
      None
  Return values:
      None
  Modified by:
      BGable      6/26/98
******************************************************************************/
extern void genutil_process_error(pCHAR func_name, pCHAR err_code_str, pCHAR err_msg, pCHAR tid,
                          pCHAR disp_abort1, pCHAR disp_abort2, int log_event)
{
   CHAR    buff_1[256];
   CHAR    buff_2[256];
   CHAR    AppName[8];
   pPTE_MSG p_msg_out  = NULL;
   LONG     ret_code   = 0L;
   CHAR     connection_id[MAX_ORIG_INFO_SIZE +1] ={0};
   
   memset(buff_1, 0, sizeof(buff_1));
   memset(buff_2, 0, sizeof(buff_2));
   /* Build function name and error code string */ 
   strncpy(buff_1, func_name, sizeof(buff_1) - 1);
   strcat(buff_1, ": ");
   strncat(buff_1, err_code_str, (sizeof(buff_1) - strlen(buff_1)) - 1);

   /* If error message is present */
   if(strcmp(err_msg, NULL_STR) != 0)
   {
       strcat(buff_1, " ::: ");
       strcat(buff_1, err_msg);
   }
   /* Display to console if in debug mode */
   PRINT(buff_1);
   PRINT("\n");
   /* If terminal id is present */
   if(strcmp(tid, NULL_STR) != 0)
   {
       sprintf(buff_2, "TID: %s", tid);
       /* Display to console if in debug mode */
       PRINT(buff_2); 
       PRINT("\n");
   }

   if(log_event == 1)
   {
      LogEvent(buff_2, WARN_MSG);
   }

   /* Add terminal id to error string for NT event logging */
   GetAppName(AppName);
   strncat(buff_2, AppName, sizeof(AppName));
   strcat(buff_2, " - ");
	strncat(buff_2, buff_1, (sizeof(buff_2) - strlen(buff_2)) - 1);
	
   ret_code = ReadMemMonitorTable(connection_id);
   if(ret_code == MEMACCESSERROR)
      return;
   if(connection_id[0] != 0)
   {
      p_msg_out = ptemsg_build_msg(MT_SYS_MONITOR, 
                                    ST1_OPRMSG,
                                    0,
                                    applnk_que_name,
                                    application_que_name,
                                    (pBYTE)buff_2,
                                    strlen(buff_2),
                                    0);

      ptemsg_set_orig_comms_info(p_msg_out, connection_id);
      ret_code = pteipc_send(p_msg_out, applnk_que_name);
      free(p_msg_out);
   }
   /*genutil_opermon_update("200", err_code_str, AppName, "WARNING", NULL_STR);*/

} /* genutil_process_error */

/*****************************************************************************

  Function:   genutil_str_asc_to_bcd

  Description: Converts and ascii string to a bcd string
  Author: 
      unknown
  Inputs:
      astr     - incoming ascii string
      how_many - number to move
  Outputs:
      bstring  - outgoing bcd strin
  Return values:
      None
  Modified by:
      BGable      6/26/98
******************************************************************************/
extern void genutil_str_asc_to_bcd (pCHAR astr, INT how_many, BYTE bstring[])
{
   UINT  i = 0;

   for (i = (strlen(astr) - how_many); i < strlen(astr); i=i+2)
      {
      /* if I hit a null terminator, quit.  */
      if (astr[i] == '\0')
         break;

      /* if these are leading blanks, leave the bcd zero.  */
      if (astr[i] != ' ')
         bstring[i/2] = ((astr[i]-'0')*16);

      if ( ((i+1) < strlen(astr)) &&
           (astr[i+1] != ' ')          )
         bstring[i/2] += (astr[i+1]-'0');
      }
} /* genutil_str_asc_to_bcd */


/*****************************************************************************
 *
 * Function:    genutil_asc_to_bin
 *
 * Description: Convert an ASCII string of numbers and/or letters A-F
 *              into a hexidecimal string.
 *
 *              Example:  CHAR = "1A2C56BD" (length is 8 bytes)
 *                        BYTE = 1A 2C 56 BD (length is 4 bytes)
 *
 * Inputs:      astr - ASCII string that will be converted
 *
 * Outputs:     bstr     - Binary string
 *              bstr_len - Number of bytes in the binary string
 *
 * RTRN VALUE:  None
 *
 * AUTHOR:      Dennis Irby
 *
 *****************************************************************************/
extern void genutil_asc_to_bin( pCHAR astr, pBYTE bstr, pINT bstr_len )
{
   INT  i,j;
   INT  len;

   len = strlen( astr );
   *bstr_len = 0;
   for( i=0,j=0; i<len; i+=2,j++ )
   {
      /* Upper nibble */
      if ( astr[i] <= '9' )
         bstr[j] = (astr[i] & 0x0f) * 16;
      else
         bstr[j] = ((astr[i] & 0x0f) + 9) * 16;

      /* Lower nibble */
      if ( astr[i+1] <= '9' )
         bstr[j] |= (astr[i+1] & 0x0f);
      else
         bstr[j] |= ((astr[i+1] & 0x0f) + 9);

      (*bstr_len)++;
   }
   return;
}


/*****************************************************************************

  Function:    genutil_base64_to_binary

  Description: Decode a base64 encoded stream discarding padding, line
               breaks and noise.

               * * * See BASE64 Alphabet following this function. * * *

  Author:      Dennis Irby, modification of original source by Bob Trower
               of Trantor Standard Systems Inc.  See License info below

  Inputs:      in_rec - Stream of Base64 characters
               in_len - Number of bytes of in_rec

  Outputs:     out_rec - Stream of binary characters converted from in_rec

  RTRN Values: Number of bytes in out_rec.


  LICENSE Info of Original Source Code:
                Copyright (c) 2001 Bob Trower, Trantor Standard Systems Inc.

                Permission is hereby granted, free of charge, to any person
                obtaining a copy of this software and associated
                documentation files (the "Software"), to deal in the
                Software without restriction, including without limitation
                the rights to use, copy, modify, merge, publish, distribute,
                sublicense, and/or sell copies of the Software, and to
                permit persons to whom the Software is furnished to do so,
                subject to the following conditions:

                The above copyright notice and this permission notice shall
                be included in all copies or substantial portions of the
                Software.

                THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
                KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
                WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
                PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
                OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
                OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
                OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
                SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

******************************************************************************/
INT genutil_base64_to_binary( pBYTE in_rec, INT in_len, pBYTE out_rec )
{
   /*
    * Translation Table as described in RFC1113 (not used, here for reference.
    *
    * static const char cb64[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    */

   /*
    * Translation Table to decode
    */
   static const char cd64[]="|$$$}rstuvwxyz{$$$$$$$>?@ABCDEFGHIJKLMNOPQRSTUVW$$$$$$XYZ[\\]^_`abcdefghijklmnopq";

   BYTE  in[4], out[3], v;
   INT   i, len;
   INT   in_ctr  = 0;
   INT   out_ctr = 0;

   while ( in_ctr < in_len )
   {
      memset( in,  0x00, sizeof(in)  );
      memset( out, 0x00, sizeof(out) );

      for( len = 0, i = 0; i < 4 && (in_ctr < in_len); i++ )
      {
         v = 0;
         while( (in_ctr < in_len) && v == 0 )
         {
            v = in_rec[in_ctr++];
            v = (unsigned char) ((v < 43 || v > 122) ? 0 : cd64[ v - 43 ]);
            if( v )
            {
               v = (unsigned char) ((v == '$') ? 0 : v - 61);
            }
         }
         if (in_ctr <= in_len)
         {
            len++;
            if( v )
            {
               in[ i ] = (unsigned char) (v - 1);
            }
         }
         else
         {
            in[i] = 0;
         }
      }

      if ( len )
      {
         /* Decode 4 '6-bit' characters into 3 '8-bit' binary bytes */
         out[ 0 ] = (unsigned char ) (in[0] << 2 | in[1] >> 4);
         out[ 1 ] = (unsigned char ) (in[1] << 4 | in[2] >> 2);
         out[ 2 ] = (unsigned char ) (((in[2] << 6) & 0xc0) | in[3]);

         /* Put the 3 binary bytes into the output buffer. */
         for( i = 0; i < len - 1; i++ )
         {
            out_rec[out_ctr+i] = out[i];
         }
         out_ctr += len-1;
      }
   }
   return( out_ctr );
}

/*************************************************************************

               T H E   B A S E 6 4   A L P H A B E T
               -------------------------------------

   CHAR  DEC  HEX         CHAR  DEC  HEX         CHAR  DEC  HEX         
   ----  ---  ---         ----  ---  ---         ----  ---  ---
     A     0   00           W    22   16           s    44   2C
     B     1   01           X    23   17           t    45   2D
     C     2   02           Y    24   18           u    46   2E
     D     3   03           Z    25   19           v    47   2F
     E     4   04           a    26   1A           w    48   30
     F     5   05           b    27   1B           x    49   31
     G     6   06           c    28   1C           y    50   32
     H     7   07           d    29   1D           z    51   33
     I     8   08           e    30   1E           0    52   34
     J     9   09           f    31   1F           1    53   35
     K    10   0A           g    32   20           2    54   36
     L    11   0B           h    33   21           3    55   37
     M    12   0C           i    34   22           4    56   38
     N    13   0D           j    35   23           5    57   39
     O    14   0E           k    36   24           6    58   3A
     P    15   0F           l    37   25           7    59   3B
     Q    16   10           m    38   26           8    60   3C
     R    17   11           n    39   27           9    61   3D
     S    18   12           o    40   28           +    62   3E
     T    19   13           p    41   29           /    63   3F
     U    20   14           q    42   2A                       
     V    21   15           r    43   2B           =  (pad) (pad)

**************************************************************************/


/*****************************************************************************

  Function:    genutil_binary_to_bcd

  Description: This function converts a single binary byte into a BCD value.
               The return value indicates the number of BCD bytes returned.

               Example 1:  If input is 6 or 16, bcd[0] will be 0x06 or 0x16,
                           So the return value is 1.

               Example 2:  If input is 116, bcd[0] will be 0x01 and
                           bcd[1] is 0x16.  So the return value is 2.

  Author:      Sanjoy Dasgupta

  Inputs:      long_val - Binary value to be converted,
                          treated internally as long.

  Outputs:     bcd_str  - Stream of 1 or more bytes containing BCD
                          representation of the input 'long_val'.

  RTRN Values: Number of bytes returned. See description and examples above.

******************************************************************************/
INT genutil_binary_to_bcd(long long_val, unsigned char bcd[])
{
   int i=0;
   unsigned int length;

   length = (int)ceil(getLengthOfLong(long_val)/2.0);

   /* Length will be the number of bytes to store the number.
    *
    * Example 1: If long_val is 6 or 16, bcd[0] will be 0x06 or 0x16,
    *            So, length is 1.
    * 
    * Example 2: If long_val is 116, bcd[0] will be 0x01 and bcd[1] is 0x16,
    *            So the length is 2.
    */
   for (i = length-1; i >= 0; i--)
   {
      bcd[i] = long_val % 10;
      long_val /= 10;
      bcd[i] |= ((long_val%10) << 4);
      long_val /= 10;
   }

   return length;
}


/* This function is used internally by 'genutil_binary_to_bcd'.
 * It gets the number of bytes required to represent a binary
 * value in BCD format.
 */
INT getLengthOfLong( long n )
{
   unsigned int length = 0;

   if (n == 0)
   {
      length = 1;
   }
   else
   {
      while (n != 0)
      {
         length++;
         n /= 10;
      }
   }
   return length;
}


/******************************************************************************
 *
 *  NAME:         GENUTIL_GET_EMV_TAG
 *
 *  DESCRIPTION:  This function finds an EMV tag in the raw EMV data from
 *                a terminal.  The tag can be any length.  The length of the
 *                tag's data field, and the tag's data are returned.
 *
 *                This function uses EMV 98 specifications for determining
 *                the length of a tag as it searches through the data field.
 *                If the last 5 bits of the first byte are set, then another
 *                byte follows.  For example, xxx11111 indicates another byte
 *                follows for the tag. Typical is 0x5F and 0x9F.  For subsequent
 *                bytes, if the most significant bit is set (1xxxxxxx), then
 *                another byte follows.
 *
 *                Here is the format of the EMV field:
 *
 *                   TAG1 LEN1 VALUE1 TAG2 LEN2 VALUE2 . . . TAGn LENn VALUEn
 *
 *                   Each TAG can be multiple bytes in length.
 *                   Each LEN can be 1, 2, or 3 bytes
 *                   Each VALUE can be up to 255 bytes.
 *
 *                The coding of the LEN field is as follows:
 *                When the most significant bit of the length field is set to
 *                0, the length field consists of only one byte.  The other
 *                bits code the number of bytes of the value field.  The
 *                length field is within the range 1 to 127.
 *
 *                When the most significant bit of first byte of the length
 *                field is set to 1, the subsequent bits indicate the number
 *                of subsequent bytes in the length field.  The subsequent
 *                bytes code an integer representing the number of bytes in
 *                the value field.  Two bytes are necessary to express up to
 *                255 bytes in the value field.
 *
 *  INPUTS:       tag_in    - Tag to search for
 *                emv_field - Raw EMV data - all tags
 *                emv_len   - Length of emv_field
 *
 *  OUTPUTS:      tag_value     - String containing value of 'tag_in' if found
 *                tag_value_len - Length of tag_value 
 *
 *  RTRN VALUE:   True if tag is found; false if tag not found.
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT genutil_get_emv_tag( pBYTE tag_in, pBYTE emv_field,  INT emv_len,
                                       pBYTE tag_value, pINT tag_value_len )
{
   INT   tag_found = false;
   INT   done      = false;
   INT   emv_idx = 0;
   INT   len;
   BYTE  tag[5];
   BYTE  temp_len[4];

   /* Loop through all the tags in the EMV field. */
   while( !tag_found && !done )
   {
      if ( emv_field[emv_idx] != 0x00 )
      {
         /*+------------------+
           | Get TAG subfield |
           +------------------+*/
         len = 1;
         if ( (emv_field[emv_idx] & 0x1f) == 0x1f )
         {
            /* The tag is more than 1 byte long.
             * Check the continuation bit to see
             * how many bytes there are to this tag.
             * As long as a byte has its continuation
             * bit set, keep adding to the length.
             * But for standard EMV, the tag is either
             * 1 or 2 bytes long.
             */
            len ++;
            while( emv_field[emv_idx+len-1] & 0x80 )
            {
               len++;
               if ( len > emv_len )
               {
                  /* We went past the end of the EMV field. Exit. */
                  done = true;
                  break;
               }
            }
         }

         /* We now know how long the tag is. Get the tag to compare. */
         memset( tag, 0x00, sizeof(tag) );
         memcpy( tag, &emv_field[emv_idx], len );

         /*+---------------------+
           | Get LENGTH subfield |
           +---------------------+*/

         if ( !done )
         {
            /* Now get the length of the EMV value.  Need to
             * get this whether this is the right tag or not.
             */
            emv_idx += len;
            memset( temp_len, 0x00, sizeof(temp_len) );
            if ( emv_field[emv_idx] & 0x80 )
            {
               /* The length subfield is more than 1 byte.
                * The rest of this byte indicates how many bytes follow.
                */
               memcpy( temp_len, &emv_field[emv_idx], 1 );
               temp_len[0] &= 0x7f; /* Strip off continuation bit */
               len = temp_len[0];

               /* 'len' now indicates number of length bytes, 1 or 2 that
                * follow the first length byte.  The first byte can be
                * skipped.  Its job is done. Now use the next 1 or 2 bytes
                * to get the data length.
                */
               emv_idx++; /* Step past initial length byte. */
               if ( len == 1 )
               {
                  /* Number of bytes in length subfield is 2 (1+first). */
                  len = emv_field[emv_idx++];
               }
               else
               {
                  /* Number of bytes in length subfield is 3 (2+first). */
                  len  = emv_field[emv_idx++];
                  len += emv_field[emv_idx++];
               }
            }
            else
            {
               /* Number of bytes in length subfield is 1. */
               len = emv_field[emv_idx++];
            }

            if ( (emv_idx + len) > emv_len )
            {
               /* We have an invalid length. We went
                * past the end of the EMV field.  Exit.
                */
               done = true;
            }
            else
            {
               /*+--------------------+
                 | Get VALUE subfield |
                 +--------------------+*/

               /* Is this the tag we are looking for? */
               if ( 0 == strcmp(tag_in, tag) )
               {
                  /* Yes - we found it!  Copy it to the output field. */
                  tag_found = true;
                  memcpy( tag_value, &emv_field[emv_idx], len );
                  *tag_value_len = len;
               }
               else
               {
                  /* No, skip past this tag. */
                  emv_idx += len;
               }
            }
         }
      }
      else
      {
         /* No more tags; we didn't find it. */
         done = true;
      }
   }
   return( tag_found );
}


/******************************************************************************
 *
 *  NAME:         GENUTIL_GET_COUNTRY_CODE
 *
 *  DESCRIPTION:  This function will return convert an alpha country code
 *                into a numeric country code.  Conversely, it can convert a
 *                numeric country code an alpha country code.
 *
 *                This function works with 2-char country codes, 3-char
 *                country codes, and 3-digit country codes.  For example, for
 *                the United States, the values:  "US", "USA", and "840".
 *
 *                When converting from numeric to alpha, the 2-char country
 *                code is always returned.
 *
 *                If a conversion cannot be done due to value not found,
 *                the result will be null for alpha, or zero for numeric.
 *
 *  INPUTS:       method  - Flag to indicate which way to convert:
 *                             A2N = alpha->numeric  or  
 *                             N2A = numeric->alpha
 *                char_cc - 2-char country code or null
 *                num_cc  - 3-digit country code or zero
 *
 *  OUTPUTS:      char_cc - 2-char alpha country code or null
 *                num_cc  - 3-digit numeric country code or null
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void genutil_get_country_code( INT method, pCHAR char_cc, pCHAR num_cc )
{
   INT  i;
   INT  retval = false;

   if ( cc_table_initialized == false )
      init_country_codes();

   if ( method == A2N )
   {
      memset( num_cc, 0x00, 4 );
      i = 0;
      if ( 2 == strlen(char_cc) )
      {
         while( cc_table[i][0] != 0x00 )
         {
            if ( 0 == strncmp(char_cc, cc_table[i], 2) )
            {
               memcpy( num_cc, cc_table[i]+7, 3 );
               retval = true;
               break;
            }
            else
               i++;
         }
      }
      else if ( 3 == strlen(char_cc) )
      {
         while( cc_table[i][0] != 0x00 )
         {
            if ( 0 == strncmp(char_cc, cc_table[i]+3, 3) )
            {
               memcpy( num_cc, cc_table[i]+7, 3 );
               retval = true;
               break;
            }
            else
               i++;
         }
      }

      if ( retval == false )
         memset( num_cc, 0x00, 1 );
   }
   else
   {
      memset( char_cc, 0x00, 3 );
      i = 1;
      while( cc_table[i][0] != 0x00 )
      {
         if ( 0 == strncmp(num_cc, cc_table[i]+7, 3) )
         {
            memcpy( char_cc, cc_table[i], 2 );
            retval = true;
            break;
         }
         else
            i++;
      }

      if ( retval == false )
         memset( char_cc, 0x00, 1 );
   }
   return;
}


/******************************************************************************
 *
 *  NAME:         INIT_COUNTRY_CODES
 *
 *  DESCRIPTION:  This function initializes the country code structures that
 *                contain alpha and numeric country codes.  This is done here
 *                so it does not need to be done for every transaction, that
 *                is, everytime "genutil_get_country_code" is called.
 *
 *  INPUTS:       cc_table - (Global) Empty Structure of country codes
 *
 *  OUTPUTS:      cc_table - (Global) Full Structure of country codes
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void init_country_codes()
{
   memset( cc_table, 0x00, sizeof(cc_table) );

   /* Two and Three Char Country Codes with Numeric Country Code */
   strcpy( cc_table[0],    "AF AFG 004" ); /* Afghanistan              */
   strcpy( cc_table[1],    "AL ALB 008" ); /* Albania                  */
   strcpy( cc_table[2],    "DZ DZA 012" ); /* Algeria                  */
   strcpy( cc_table[3],    "AS ASM 016" ); /* American Samoa           */
   strcpy( cc_table[4],    "AD AND 020" ); /* Andorra                  */
   strcpy( cc_table[5],    "AO AGO 024" ); /* Angola                   */
   strcpy( cc_table[6],    "AI AIA 660" ); /* Anguilla                 */
   strcpy( cc_table[7],    "AQ ATA 010" ); /* Antarctica               */
   strcpy( cc_table[8],    "AG ATG 028" ); /* Antigua                  */
   strcpy( cc_table[9],    "AR ARG 032" ); /* Argentina                */
   strcpy( cc_table[10],   "AM ARM 051" ); /* Armenia                  */
   strcpy( cc_table[11],   "AW ABW 533" ); /* Aruba                    */
   strcpy( cc_table[12],   "AU AUS 036" ); /* Australia                */
   strcpy( cc_table[13],   "AT AUT 040" ); /* Austria                  */
   strcpy( cc_table[14],   "AZ AZE 031" ); /* Azerbaijan               */
   strcpy( cc_table[15],   "BS BHS 044" ); /* Bahamas                  */
   strcpy( cc_table[16],   "BH BHR 048" ); /* Bahrain                  */
   strcpy( cc_table[17],   "BD BGD 050" ); /* Bangladesh               */
   strcpy( cc_table[18],   "BB BRB 052" ); /* Barbados                 */
   strcpy( cc_table[19],   "BY BLR 112" ); /* Belarus                  */
   strcpy( cc_table[20],   "BE BEL 056" ); /* Belgium                  */
   strcpy( cc_table[21],   "BZ BLZ 084" ); /* Belize                   */
   strcpy( cc_table[22],   "BJ BEN 204" ); /* Benin                    */
   strcpy( cc_table[23],   "BM BMU 060" ); /* Bermuda                  */
   strcpy( cc_table[24],   "BT BTN 064" ); /* Bhutan                   */
   strcpy( cc_table[25],   "BO BOL 068" ); /* Bolivia                  */
   strcpy( cc_table[26],   "BA BIH 070" ); /* Bosnia                   */
   strcpy( cc_table[27],   "BW BWA 072" ); /* Botswana                 */
   strcpy( cc_table[28],   "BV BVT 074" ); /* Bouvet Isle              */
   strcpy( cc_table[29],   "BR BRA 076" ); /* Brazil                   */
   strcpy( cc_table[30],   "IO IOT 086" ); /* Br. Ind Oc.              */
   strcpy( cc_table[31],   "BN BRN 096" ); /* Brunei Darussalam        */
   strcpy( cc_table[32],   "BG BGR 100" ); /* Bulgaria                 */
   strcpy( cc_table[33],   "BF BFA 854" ); /* Burkina Faso             */
   strcpy( cc_table[34],   "BI BDI 108" ); /* Burundi                  */
   strcpy( cc_table[35],   "KH KHM 116" ); /* Cambodia                 */
   strcpy( cc_table[36],   "CM CMR 120" ); /* Cameroon                 */
   strcpy( cc_table[37],   "CA CAN 124" ); /* Canada                   */
   strcpy( cc_table[38],   "CV CPV 132" ); /* CapeVerde                */
   strcpy( cc_table[39],   "KY CYM 136" ); /* Cayman Islands           */
   strcpy( cc_table[40],   "CF CAF 140" ); /* C Africa Rep             */
   strcpy( cc_table[41],   "TD TCD 148" ); /* Chad                     */
   strcpy( cc_table[42],   "CL CHL 152" ); /* Chile                    */
   strcpy( cc_table[43],   "CN CHN 156" ); /* China                    */
   strcpy( cc_table[44],   "CX CXR 162" ); /* Christmas Is             */
   strcpy( cc_table[45],   "CC CCK 166" ); /* Cocos Islands            */
   strcpy( cc_table[46],   "CO COL 170" ); /* Colombia                 */
   strcpy( cc_table[47],   "CG COG 178" ); /* Congo                    */
   strcpy( cc_table[48],   "CK COK 184" ); /* Cook Islands             */
   strcpy( cc_table[49],   "CR CRI 188" ); /* Costa Rica               */
   strcpy( cc_table[50],   "CI CIV 384" ); /* Cote D'Ivoir             */
   strcpy( cc_table[51],   "HR HRV 191" ); /* Croatia Hrvatska         */
   strcpy( cc_table[52],   "CU CUB 192" ); /* Cuba                     */
   strcpy( cc_table[53],   "CY CYP 196" ); /* Cyprus                   */
   strcpy( cc_table[54],   "CS CZE 203" ); /* Czechoslovak             */
   strcpy( cc_table[55],   "DK DNK 208" ); /* Denmark                  */
   strcpy( cc_table[56],   "DJ DJI 262" ); /* Djibouti                 */
   strcpy( cc_table[57],   "DM DMA 212" ); /* Dominica                 */
   strcpy( cc_table[58],   "DO DOM 214" ); /* Dominican Rp             */
   strcpy( cc_table[59],   "TP TMP 626" ); /* East Timor               */
   strcpy( cc_table[60],   "EC ECU 218" ); /* Ecuador                  */
   strcpy( cc_table[61],   "EG EGY 818" ); /* Egypt                    */
   strcpy( cc_table[62],   "SV SLV 222" ); /* El Salvador              */
   strcpy( cc_table[63],   "GQ GNQ 226" ); /* Equatorial Guinea        */
   strcpy( cc_table[64],   "ER ERI 232" ); /* Eritrea                  */
   strcpy( cc_table[65],   "EE EST 233" ); /* Estonia                  */
   strcpy( cc_table[66],   "ET ETH 231" ); /* Ethiopia                 */
   strcpy( cc_table[67],   "FK FLK 238" ); /* Falkland Islands         */
   strcpy( cc_table[68],   "FO FRO 234" ); /* Faroe Island             */
   strcpy( cc_table[69],   "FJ FJI 242" ); /* Fiji                     */
   strcpy( cc_table[70],   "FI FIN 246" ); /* Finland                  */
   strcpy( cc_table[71],   "FR FRA 250" ); /* France                   */
   strcpy( cc_table[72],   "FX FXX 249" ); /* France Metro             */
   strcpy( cc_table[73],   "GF GUF 254" ); /* French Guiana            */
   strcpy( cc_table[74],   "PF PYF 258" ); /* French Polyn             */
   strcpy( cc_table[75],   "TF ATF 260" ); /* French S Territor        */
   strcpy( cc_table[76],   "GA GAB 266" ); /* Gabon                    */
   strcpy( cc_table[77],   "GM GMB 270" ); /* Gambia                   */
   strcpy( cc_table[78],   "GE GEO 268" ); /* Georgia                  */
   strcpy( cc_table[79],   "DE DEU 280" ); /* Germany                  */
   strcpy( cc_table[80],   "GH GHA 288" ); /* Ghana                    */
   strcpy( cc_table[81],   "GI GIB 292" ); /* Gibraltar                */
   strcpy( cc_table[82],   "GR GRC 300" ); /* Greece                   */
   strcpy( cc_table[83],   "GL GRL 304" ); /* Greenland                */
   strcpy( cc_table[84],   "GD GRD 308" ); /* Grenada                  */
   strcpy( cc_table[85],   "GP GLP 312" ); /* Guadeloupe               */
   strcpy( cc_table[86],   "GU GUM 316" ); /* Guam                     */
   strcpy( cc_table[87],   "GT GTM 320" ); /* Guatemala                */
   strcpy( cc_table[88],   "GN GIN 324" ); /* Guinea                   */
   strcpy( cc_table[89],   "GW GNB 624" ); /* Guinea-Bissau            */
   strcpy( cc_table[90],   "GY GUY 328" ); /* Guyana                   */
   strcpy( cc_table[91],   "HT HTI 332" ); /* Haiti                    */
   strcpy( cc_table[92],   "HM HMD 334" ); /* Heard/McD Is             */
   strcpy( cc_table[93],   "HN HND 340" ); /* Honduras                 */
   strcpy( cc_table[94],   "HK HKG 344" ); /* Hong Kong                */
   strcpy( cc_table[95],   "HU HUN 348" ); /* Hungary                  */
   strcpy( cc_table[96],   "IS ISL 352" ); /* Iceland                  */
   strcpy( cc_table[97],   "IN IND 356" ); /* India                    */
   strcpy( cc_table[98],   "ID IDN 360" ); /* Indonesia                */
   strcpy( cc_table[99],   "IR IRN 364" ); /* Iran                     */
   strcpy( cc_table[100],  "IQ IRQ 368" ); /* Iraq                     */
   strcpy( cc_table[101],  "IE IRL 372" ); /* Ireland                  */
   strcpy( cc_table[102],  "IL ISR 376" ); /* Israel                   */
   strcpy( cc_table[103],  "IT ITA 380" ); /* Italy                    */
   strcpy( cc_table[104],  "JM JAM 388" ); /* Jamaica                  */
   strcpy( cc_table[105],  "JP JPN 392" ); /* Japan                    */
   strcpy( cc_table[106],  "JO JOR 400" ); /* Jordan                   */
   strcpy( cc_table[107],  "KZ KAZ 398" ); /* Kazakhstan               */
   strcpy( cc_table[108],  "KE KEN 404" ); /* Kenya                    */
   strcpy( cc_table[109],  "KI KIR 296" ); /* Kiribati                 */
   strcpy( cc_table[110],  "KP PRK 408" ); /* N. Korea                 */
   strcpy( cc_table[111],  "KR KOR 410" ); /* S. Korea                 */
   strcpy( cc_table[112],  "KW KWT 414" ); /* Kuwait                   */
   strcpy( cc_table[113],  "KG KGZ 417" ); /* Kyrgyzstan               */
   strcpy( cc_table[114],  "LA LAO 418" ); /* Lao Dem Rep              */
   strcpy( cc_table[115],  "LV LVA 428" ); /* Latvia                   */
   strcpy( cc_table[116],  "LB LBN 422" ); /* Lebanon                  */
   strcpy( cc_table[117],  "LS LSO 426" ); /* Lesotho                  */
   strcpy( cc_table[118],  "LR LBR 430" ); /* Liberia                  */
   strcpy( cc_table[119],  "LY LBY 434" ); /* Libya                    */
   strcpy( cc_table[120],  "LI LIE 438" ); /* Liechtenstein            */
   strcpy( cc_table[121],  "LT LTU 440" ); /* Lithuania                */
   strcpy( cc_table[122],  "LU LUX 442" ); /* Luxembourg               */
   strcpy( cc_table[123],  "MO MAC 446" ); /* Macau                    */
   strcpy( cc_table[124],  "MK MKD 807" ); /* Macedonia                */
   strcpy( cc_table[125],  "MG MDG 450" ); /* Madagascar               */
   strcpy( cc_table[126],  "MW MWI 454" ); /* Malawi                   */
   strcpy( cc_table[127],  "MY MYS 458" ); /* Malaysia                 */
   strcpy( cc_table[128],  "MV MDV 462" ); /* Maldives                 */
   strcpy( cc_table[129],  "ML MLI 466" ); /* Mali                     */
   strcpy( cc_table[130],  "MT MLT 470" ); /* Malta                    */
   strcpy( cc_table[131],  "MH MHL 584" ); /* Marshall Islands         */
   strcpy( cc_table[132],  "MQ MTQ 474" ); /* Martinique               */
   strcpy( cc_table[133],  "MR MRT 478" ); /* Mauritania               */
   strcpy( cc_table[134],  "MU MUS 480" ); /* Mauritius                */
   strcpy( cc_table[135],  "YT MYT 175" ); /* Mayotte                  */
   strcpy( cc_table[136],  "MX MEX 484" ); /* Mexico                   */
   strcpy( cc_table[137],  "FM FSM 583" ); /* Micronesia               */
   strcpy( cc_table[138],  "MD MDA 498" ); /* Moldova                  */
   strcpy( cc_table[139],  "MC MCO 492" ); /* Monaco                   */
   strcpy( cc_table[140],  "MN MNG 496" ); /* Mongolia                 */
   strcpy( cc_table[141],  "MS MSR 500" ); /* Montserrat               */
   strcpy( cc_table[142],  "MA MAR 504" ); /* Morocco                  */
   strcpy( cc_table[143],  "MZ MOZ 508" ); /* Mozambique               */
   strcpy( cc_table[144],  "MM MMR 104" ); /* Myanmar                  */
   strcpy( cc_table[145],  "NA NAM 516" ); /* Namibia                  */
   strcpy( cc_table[146],  "NR NRU 520" ); /* Nauru                    */
   strcpy( cc_table[147],  "NP NPL 524" ); /* Nepal                    */
   strcpy( cc_table[148],  "NL NLD 528" ); /* Netherlands              */
   strcpy( cc_table[149],  "AN ANT 530" ); /* Netherlands Antil        */
   strcpy( cc_table[150],  "NC NCL 540" ); /* New Caledoni             */
   strcpy( cc_table[151],  "NZ NZL 554" ); /* New Zealand              */
   strcpy( cc_table[152],  "NI NIC 558" ); /* Nicaragua                */
   strcpy( cc_table[153],  "NE NER 562" ); /* Niger                    */
   strcpy( cc_table[154],  "NG NGA 566" ); /* Nigeria                  */
   strcpy( cc_table[155],  "NU NIU 570" ); /* Niue                     */
   strcpy( cc_table[156],  "NF NFK 574" ); /* Norfolk Isle             */
   strcpy( cc_table[157],  "MP MNP 580" ); /* N. Mariana Isles         */
   strcpy( cc_table[158],  "NO NOR 578" ); /* Norway                   */
   strcpy( cc_table[159],  "OM OMN 512" ); /* Oman                     */
   strcpy( cc_table[160],  "PK PAK 586" ); /* Pakistan                 */
   strcpy( cc_table[161],  "PW PLW 585" ); /* Palau                    */
   strcpy( cc_table[162],  "PA PAN 591" ); /* Panama                   */
   strcpy( cc_table[163],  "PG PNG 598" ); /* Papua New Guinea         */
   strcpy( cc_table[164],  "PY PRY 600" ); /* Paraguay                 */
   strcpy( cc_table[165],  "PE PER 504" ); /* Peru                     */
   strcpy( cc_table[166],  "PH PHL 608" ); /* Philippines              */
   strcpy( cc_table[167],  "PN PCN 612" ); /* Pitcairn                 */
   strcpy( cc_table[168],  "PL POL 616" ); /* Poland                   */
   strcpy( cc_table[169],  "PT PRT 620" ); /* Portugal                 */
   strcpy( cc_table[170],  "PR PRI 630" ); /* Puerto Rico              */
   strcpy( cc_table[171],  "QA QAT 634" ); /* Qatar                    */
   strcpy( cc_table[172],  "RE REU 638" ); /* Reunion                  */
   strcpy( cc_table[173],  "RO ROM 642" ); /* Romania                  */
   strcpy( cc_table[174],  "RU RUS 643" ); /* Russian Fed.             */
   strcpy( cc_table[175],  "RW RWA 646" ); /* Rwanda                   */
   strcpy( cc_table[176],  "GS SGS 239" ); /* S Georgia Is             */
   strcpy( cc_table[177],  "KN KNA 659" ); /* St Kitts & Nevis         */
   strcpy( cc_table[178],  "LC LCA 662" ); /* Saint Lucia              */
   strcpy( cc_table[179],  "WD WSM 882" ); /* Samoa                    */
   strcpy( cc_table[180],  "SM SMR 674" ); /* San Marino               */
   strcpy( cc_table[181],  "ST STP 678" ); /* SaoTome & Princip        */
   strcpy( cc_table[182],  "SQ SAU 682" ); /* Saudi Arabia             */
   strcpy( cc_table[183],  "SN SEN 686" ); /* Senegal                  */
   strcpy( cc_table[184],  "SC SYC 690" ); /* Seychelles               */
   strcpy( cc_table[185],  "SL SLE 694" ); /* Sierra Leone             */
   strcpy( cc_table[186],  "SG SGP 702" ); /* Singapore                */
   strcpy( cc_table[187],  "SK SVK 703" ); /* Slovakia                 */
   strcpy( cc_table[188],  "SI SVN 705" ); /* Slovenia                 */
   strcpy( cc_table[189],  "SB SLB 090" ); /* Solomon Islands          */
   strcpy( cc_table[190],  "SO SOM 706" ); /* Somalia                  */
   strcpy( cc_table[191],  "ZA ZAF 710" ); /* South Africa             */
   strcpy( cc_table[192],  "ES ESP 724" ); /* Spain                    */
   strcpy( cc_table[193],  "LK LKA 144" ); /* Sri Lanka                */
   strcpy( cc_table[194],  "SH SHN 654" ); /* St. Helena               */
   strcpy( cc_table[195],  "PM SPM 666" ); /* St. Pierre/Miquel        */
   strcpy( cc_table[196],  "VC VCT 670" ); /* St. Vincent              */
   strcpy( cc_table[197],  "SD SDN 736" ); /* Sudan                    */
   strcpy( cc_table[198],  "SR SUR 740" ); /* Surinam                  */
   strcpy( cc_table[199],  "SJ SJM 744" ); /* Svalbard/Jan Maye        */
   strcpy( cc_table[200],  "SZ SWZ 748" ); /* Swaziland                */
   strcpy( cc_table[201],  "DE SWE 752" ); /* Sweden                   */
   strcpy( cc_table[202],  "CH CHE 756" ); /* Switzerland              */
   strcpy( cc_table[203],  "SY SYR 760" ); /* Syrian Arab Rep          */
   strcpy( cc_table[204],  "TW TWN 158" ); /* Taiwan                   */
   strcpy( cc_table[205],  "TJ TZA 834" ); /* Tanzania                 */
   strcpy( cc_table[206],  "TH THA 764" ); /* Thailand                 */
   strcpy( cc_table[207],  "TG TGO 768" ); /* Togo                     */
   strcpy( cc_table[208],  "TK TKL 772" ); /* Tokelau                  */
   strcpy( cc_table[209],  "TO TON 776" ); /* Tonga                    */
   strcpy( cc_table[210],  "TT TTO 780" ); /* Trinidad/Tob             */
   strcpy( cc_table[211],  "TN TUN 788" ); /* Tunisia                  */
   strcpy( cc_table[212],  "TR TUR 792" ); /* Turkey                   */
   strcpy( cc_table[213],  "TM TKM 795" ); /* Turkmenistan             */
   strcpy( cc_table[214],  "TC TCA 796" ); /* Turks/Caicos             */
   strcpy( cc_table[215],  "TV TUV 798" ); /* Tuvalu                   */
   strcpy( cc_table[216],  "UG UGA 800" ); /* Uganda                   */
   strcpy( cc_table[217],  "UA UKR 804" ); /* Ukraine                  */
   strcpy( cc_table[218],  "AE ARE 784" ); /* U. Arab Emir             */
   strcpy( cc_table[219],  "GB GBR 826" ); /* United Kingdom           */
   strcpy( cc_table[220],  "US USA 840" ); /* United States            */
   strcpy( cc_table[221],  "UM UMI 581" ); /* United States Isl        */
   strcpy( cc_table[222],  "UY URY 858" ); /* Uruguay                  */
   strcpy( cc_table[223],  "UZ UZB 860" ); /* Uzbekistan               */
   strcpy( cc_table[224],  "VU VUT 548" ); /* Vanuatu                  */
   strcpy( cc_table[225],  "VA VAT 336" ); /* Vatican City             */
   strcpy( cc_table[226],  "VE VEN 862" ); /* Venezuela                */
   strcpy( cc_table[227],  "VN VNM 704" ); /* Viet Nam                 */
   strcpy( cc_table[228],  "VG VG  092" ); /* Virgin (Br.)             */
   strcpy( cc_table[229],  "VI VIR 850" ); /* Virgin Isles (US)        */
   strcpy( cc_table[230],  "WF WLF 876" ); /* Wallis/Futun             */
   strcpy( cc_table[231],  "EH ESH 732" ); /* Western Sahara           */
   strcpy( cc_table[232],  "YE YEM 887" ); /* Yemen                    */
   strcpy( cc_table[233],  "YU YUG 891" ); /* Yugoslavia               */
   strcpy( cc_table[234],  "ZR ZR  180" ); /* Zaire                    */
   strcpy( cc_table[235],  "ZM ZMB 894" ); /* Zambia                   */
   strcpy( cc_table[236],  "ZW ZWE 716" ); /* Zimbabwe                 */

   cc_table_initialized = true;
   return;
}


/******************************************************************************
 *
 *  NAME:         GENUTIL_IS_US_STATE_CODE_VALID
 *
 *  DESCRIPTION:  This function will determines if the input string is a
 *                valid State code of the United States.
 *
 *  INPUTS:       state_code - Two-character US State Code
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True is state_code is valid, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT genutil_is_US_state_code_valid( pCHAR state_code )
{
   INT  retval = false;
   INT  i;


   if ( st_table_initialized == false )
      init_state_codes();

   i = 1;
   while( st_table[i][0] != 0x00 )
   {
      if ( 0 == strcmp(state_code, st_table[i]) )
      {
         retval = true;
         break;
      }
      else
         i++;
   }

   return( retval );
}

/******************************************************************************
 *
 *  NAME:         INIT_STATE_CODES
 *
 *  DESCRIPTION:  This function initializes the st_table structure that
 *                contains the US State Abbreviations.  This is done here
 *                so it does not need to be done for every transaction, that
 *                is, everytime "genutil_is_US_state_code_valid" is called.
 *
 *  INPUTS:       st_table - (Global) Empty Structure of US State Abbreviations
 *
 *  OUTPUTS:      st_table - (Global) Full Structure of US State Abbreviations
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void init_state_codes()
{
   /* Two-Character US State Codes */
   memset( st_table, 0x00, sizeof(st_table) );
   strcpy( st_table[0],  "AL" );  /* Alabama                  */
   strcpy( st_table[1],  "AK" );  /* Alaska                   */
   strcpy( st_table[2],  "AS" );  /* American Samoa           */
   strcpy( st_table[3],  "AZ" );  /* Arizona                  */
   strcpy( st_table[4],  "AR" );  /* Arkansas                 */
   strcpy( st_table[5],  "CA" );  /* California               */
   strcpy( st_table[6],  "CO" );  /* Colorado                 */
   strcpy( st_table[7],  "CT" );  /* Conneticut               */
   strcpy( st_table[8],  "DE" );  /* Delaware                 */
   strcpy( st_table[9],  "DC" );  /* District of Columbia     */
   strcpy( st_table[10], "FL" );  /* Florida                  */
   strcpy( st_table[11], "FM" );  /* Fed States of Micronesia */
   strcpy( st_table[12], "GA" );  /* Georgia                  */
   strcpy( st_table[13], "GU" );  /* Guam                     */
   strcpy( st_table[14], "HI" );  /* Hawaii                   */
   strcpy( st_table[15], "ID" );  /* Idaho                    */
   strcpy( st_table[16], "IL" );  /* Illinois                 */
   strcpy( st_table[17], "IN" );  /* Indiana                  */
   strcpy( st_table[18], "IA" );  /* IOWA                     */
   strcpy( st_table[19], "KS" );  /* Kansas                   */
   strcpy( st_table[20], "KY" );  /* Kentucky                 */
   strcpy( st_table[21], "LA" );  /* Louisiana                */
   strcpy( st_table[22], "ME" );  /* Maine                    */
   strcpy( st_table[23], "MH" );  /* Marshall Islands         */
   strcpy( st_table[24], "MD" );  /* Maryland                 */
   strcpy( st_table[25], "MA" );  /* Massachusetts            */
   strcpy( st_table[26], "MI" );  /* Michigan                 */
   strcpy( st_table[27], "MN" );  /* Minnesota                */
   strcpy( st_table[28], "MS" );  /* Mississippi              */
   strcpy( st_table[29], "MO" );  /* Missouri                 */
   strcpy( st_table[30], "MT" );  /* Montana                  */
   strcpy( st_table[31], "NE" );  /* Nebraska                 */
   strcpy( st_table[32], "NV" );  /* Nevada                   */
   strcpy( st_table[33], "NH" );  /* New Hampshire            */
   strcpy( st_table[34], "NJ" );  /* New Jersey               */
   strcpy( st_table[35], "NM" );  /* New Mexico               */
   strcpy( st_table[36], "NY" );  /* New York                 */
   strcpy( st_table[37], "NC" );  /* North Carolina           */
   strcpy( st_table[38], "ND" );  /* North Dakota             */
   strcpy( st_table[39], "MP" );  /* N. Marianna Islands      */
   strcpy( st_table[40], "OH" );  /* Ohio                     */
   strcpy( st_table[41], "OK" );  /* Oklahoma                 */
   strcpy( st_table[42], "OR" );  /* Oregon                   */
   strcpy( st_table[43], "PW" );  /* Palau                    */
   strcpy( st_table[44], "PA" );  /* Pennsylvania             */
   strcpy( st_table[45], "PR" );  /* Puerto Rico              */
   strcpy( st_table[46], "RI" );  /* Rhode Island             */
   strcpy( st_table[47], "SC" );  /* South Carolina           */
   strcpy( st_table[48], "SD" );  /* South Dakota             */
   strcpy( st_table[49], "TN" );  /* Tennessee                */
   strcpy( st_table[50], "TX" );  /* Texas                    */
   strcpy( st_table[51], "UT" );  /* Utah                     */
   strcpy( st_table[52], "VT" );  /* Vermont                  */
   strcpy( st_table[53], "VI" );  /* Virgin Islands           */
   strcpy( st_table[54], "VA" );  /* Virginia                 */
   strcpy( st_table[55], "WA" );  /* Washington               */
   strcpy( st_table[56], "WV" );  /* West Virginia            */
   strcpy( st_table[57], "WI" );  /* Wisconsin                */
   strcpy( st_table[58], "WY" );  /* Wyoming                  */

   st_table_initialized = true;
   return;
}


/******************************************************************************
 *
 *  NAME:         GENUTIL_EMV_TO_AUTH_TX
 *
 *  DESCRIPTION:  Convert ISO8583 Field 55 (EMV Data) from their binary/bcd
 *                fields to their ASCII fields in Auth_Tx.
 *
 *  INPUTS:       emv_raw   - The EMV data in its raw format
 *                total_len - Length of the EMV Raw data
 *
 *  OUTPUTS:      p_auth_tx - Auth Tx structure with EMV in ASCII format
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void genutil_EMV_To_Auth_Tx( pCHAR emv_raw, INT total_len, pAUTH_TX p_auth_tx )
{
   INT    fieldLength;
   INT    dataIndex;
   INT    bTwoBytesTag;
   BYTE   byteOneOfTag;
   BYTE   byteTwoOfTag;
   BYTE   b_string[4]  = {0};
   CHAR   a_string[4]  = {0};
   CHAR   tempstr[500] = {0};
#if MC06_OCT2023
   CHAR str_96[19]={0};
#endif

   dataIndex   = 0;

   while (dataIndex <= total_len)
   {
      bTwoBytesTag = 0;
      byteOneOfTag = emv_raw[dataIndex];
      dataIndex++;
      if ((byteOneOfTag & 0x1F) == 0x1F)
      {
         bTwoBytesTag = 1;
         byteTwoOfTag = emv_raw[dataIndex];
         dataIndex++;
      }
      fieldLength = emv_raw[dataIndex];
      dataIndex++;

      if (bTwoBytesTag)
      {
         if (byteOneOfTag == 0x5F)
         {
          	if (byteTwoOfTag == 0x20) 
            {
               genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
               memcpy(p_auth_tx->EMV_details.cardholder_name, tempstr, sizeof(p_auth_tx->EMV_details.cardholder_name)-1);
               dataIndex += fieldLength;
            }
            else if (byteTwoOfTag == 0x2A) 
            {
               genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
               memcpy(p_auth_tx->EMV_details.currency_code, tempstr, sizeof(p_auth_tx->EMV_details.currency_code)-1);
               dataIndex += fieldLength;
            }
            else if (byteTwoOfTag == 0x34) 
            {
               memset(p_auth_tx->EMV_details.pan_sequence_number,0x00,sizeof(p_auth_tx->EMV_details.pan_sequence_number));
               genutil_bcd_to_asc (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));	
               memcpy(p_auth_tx->EMV_details.pan_sequence_number, tempstr, sizeof(p_auth_tx->EMV_details.pan_sequence_number)-1);
               dataIndex += fieldLength;
            }
         }
         else if (byteOneOfTag == 0x9F)
         {
            switch (byteTwoOfTag)
            {
               case 0x02: 
                        genutil_bcd_to_asc (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.tran_amount, tempstr, sizeof(p_auth_tx->EMV_details.tran_amount)-1);
                        break;
               case 0x03:
                        genutil_bcd_to_asc (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.other_amount, tempstr, sizeof(p_auth_tx->EMV_details.other_amount)-1);
                        break;
               case 0x09: 
                        genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.version, tempstr, sizeof(p_auth_tx->EMV_details.version)-1);
                        break;
               case 0x10: 
                        genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.iad, tempstr, sizeof(p_auth_tx->EMV_details.iad)-1);
                        break;
			   case 0x0B: 
                        genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.UPI_card_std_len, tempstr, sizeof(p_auth_tx->EMV_details.UPI_card_std_len)-1);
                        break;
               case 0x1A: 
                        genutil_bcd_to_asc (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.country_code, tempstr+1, sizeof(p_auth_tx->EMV_details.country_code)-1);
                        break;
               case 0x15:
                        genutil_bcd_to_asc (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.mcc, tempstr, sizeof(p_auth_tx->EMV_details.mcc)-1);
                        break;
               case 0x1E:
            	   	    genutil_bcd_to_asc (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
            	   	    memcpy(p_auth_tx->EMV_details.serial_number, tempstr, sizeof(p_auth_tx->EMV_details.serial_number)-1);
                        //memcpy(p_auth_tx->EMV_details.serial_number, &emv_raw[dataIndex], fieldLength);
                        break;
               case 0x26: 
                        genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.app_crypto, tempstr, sizeof(p_auth_tx->EMV_details.app_crypto)-1);
                        break;
               case 0x27: 
                        genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.crypto_info, tempstr, sizeof(p_auth_tx->EMV_details.crypto_info)-1);
                        break;
               case 0x33:
                        genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.terminal_capabilities, tempstr, sizeof(p_auth_tx->EMV_details.terminal_capabilities)-1);
                        break;
               case 0x34:
                        genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.cvm_results, tempstr, sizeof(p_auth_tx->EMV_details.cvm_results)-1);
                        break;
               case 0x35:
                        genutil_bcd_to_asc (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.terminal_type, tempstr, sizeof(p_auth_tx->EMV_details.terminal_type)-1);
                        break;
               case 0x36:
                        genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.atc, tempstr, sizeof(p_auth_tx->EMV_details.atc)-1);
                        break;
               case 0x37:
                        genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.unpredictable_number, tempstr, sizeof(p_auth_tx->EMV_details.unpredictable_number)-1);
                        break;
               case 0x41:
                        genutil_bcd_to_asc (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.tran_sequence_ctr, tempstr, sizeof(p_auth_tx->EMV_details.tran_sequence_ctr)-1);
                        break;
               case 0x53:
                        memcpy(p_auth_tx->EMV_details.tcc, &emv_raw[dataIndex], fieldLength);
                        break;
#if MC06_OCT2023
               case 0x60:
            	   	    if(p_auth_tx->future_use_4[0] == '\0')
            	   	    {
            	   	    	memcpy(p_auth_tx->future_use_4,"9F60",4);
            	   	    	p_auth_tx->future_use_4[4]= fieldLength;
            	   	    	// fieldLength will be calculated above, keep the raw data for 9F60, due to non availability of the fileds
            	   	    	memcpy(p_auth_tx->future_use_4+5, &emv_raw[dataIndex], fieldLength);
            	   	    }
            	   	    else
            	   	    {
            	   	    	memcpy(str_96,p_auth_tx->future_use_4,18);
            	   	    	memset(p_auth_tx->future_use_4,0x00,sizeof(p_auth_tx->future_use_4));
            	   	    	memcpy(p_auth_tx->future_use_4,"9F60",4);
            	   	    	p_auth_tx->future_use_4[4]= fieldLength;
            	   	    	memcpy(p_auth_tx->future_use_4+5, &emv_raw[dataIndex], fieldLength);
            	   	        memcpy(p_auth_tx->future_use_4+(fieldLength+5),str_96,18);
            	   	    }

                        break;
#endif
               default:
                        break;
            }
            dataIndex += fieldLength;
         }
      }
      else
      {
         switch (byteOneOfTag)
         {
            case 0x71:
                     /* Get overall length of this field - including tag. */
                     memset( a_string, 0x00, sizeof(a_string) );
                     memset( b_string, 0x00, sizeof(b_string) );
                     b_string[0] = fieldLength+2;
                     genutil_bin_to_ascii( b_string, 1, a_string, sizeof(a_string) );
                     memcpy( p_auth_tx->EMV_details.issuer_script_1, a_string, 2 );

                     /* Get Tag ID */
                     memcpy(p_auth_tx->EMV_details.issuer_script_1+2, "71", 2 ); /* Tag ID */

                     /* Get length of data for this tag. */
                     memset( a_string, 0x00, sizeof(a_string) );
                     memset( b_string, 0x00, sizeof(b_string) );
                     b_string[0] = fieldLength;
                     genutil_bin_to_ascii( b_string, 1, a_string, sizeof(a_string) );
                     memcpy( p_auth_tx->EMV_details.issuer_script_1+4, a_string, 2 );

                     /* Finally put in the data. */
                     genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                     memcpy(p_auth_tx->EMV_details.issuer_script_1+6, tempstr, sizeof(p_auth_tx->EMV_details.issuer_script_1)-7);

                     break;
            case 0x82:
                     genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                     memcpy(p_auth_tx->EMV_details.interchange_profile, tempstr, sizeof(p_auth_tx->EMV_details.interchange_profile)-1);
                     break;
            case 0x84:
                     genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                     memcpy(p_auth_tx->EMV_details.app_id, tempstr, sizeof(p_auth_tx->EMV_details.app_id)-1);
                     break;
			case 0x8A:
                     genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                     memcpy(p_auth_tx->EMV_details.auth_response_code, tempstr, sizeof(p_auth_tx->EMV_details.auth_response_code)-1);
                     break;
            case 0x91:
                     genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                     memcpy(p_auth_tx->EMV_details.issuer_auth_data+1, tempstr, sizeof(p_auth_tx->EMV_details.issuer_auth_data)-2);
                     p_auth_tx->EMV_details.issuer_auth_data[0] = fieldLength;
                     break;
            case 0x95:
                     genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                     memcpy(p_auth_tx->EMV_details.tvr, tempstr, sizeof(p_auth_tx->EMV_details.tvr)-1);
                     break;
            case 0x9A:
                     genutil_bcd_to_asc (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                     memcpy(p_auth_tx->EMV_details.tran_date, tempstr, sizeof(p_auth_tx->EMV_details.tran_date)-1);
                     break;
            case 0x9C:
                     genutil_bcd_to_asc (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                     memcpy(p_auth_tx->EMV_details.tran_type, tempstr, sizeof(p_auth_tx->EMV_details.tran_type)-1);
                     break;
#if MC06_OCT2023
            case 0x96:
                     genutil_bcd_to_asc (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                     if(p_auth_tx->future_use_4[0] == '\0')
                     {
                    	 memcpy(p_auth_tx->future_use_4,"96",2);
                    	 memcpy(p_auth_tx->future_use_4+2, tempstr, 16);
                     }
                     else
                     {
                    	 memcpy(p_auth_tx->future_use_4+(p_auth_tx->future_use_4[4]+5),"96",2);
                    	 memcpy(p_auth_tx->future_use_4+(p_auth_tx->future_use_4[4]+7), tempstr, 16);
                     }

                     break;
#endif
            default:
                     break;
         }
         dataIndex += fieldLength;
      }
   }
   return;
}





/******************************  END OF FILE  *********************************/

