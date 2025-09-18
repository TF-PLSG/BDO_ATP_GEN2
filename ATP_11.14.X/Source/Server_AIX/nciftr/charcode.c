/******************************************************************************
 
 	 charcode.c 
 
    Copyright (c) 2005, Hypercom, Inc.
    All Rights Reserved.
 
    TITLE:  CUP Host Handler Driver
 
 	 This module contains the conversion routines for the
    ASCII and EBCDIC character codes.

    APPLICATION:  Advanced Transaction Processor (ATP)

   
   
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "basictyp.h"

BYTE      a2e[257];
BYTE      e2a[257];
BOOLEAN   maps_initialised = false;


/*****************************************************************************

Function:      init_character_code_maps

Description:   This function will initialise the EBCDIC and ASCII code 
               maps needed for the conversion routines.
               
Author: 
               Ram Malathkar
Inputs:
               None
Outputs:
               None
Return values:
               None
******************************************************************************/
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
   a2e[0x3D]=0x7E; a2e[0x22]=0x7F; a2e[0x5E]=0x5F;
   
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
   e2a[0x7E]=0x3D; e2a[0x7F]=0x22; e2a[0x5F]=0x5E;
   
   maps_initialised = true;

} /* init_character_code_maps */





/*************************************************************************************
       NAME:               convert_int_to_ascii

       DESCRIPTION:        converts a stream of bytes from integer to ASCII

       INPUTS:
           source          pointer to a stream of bytes to be converted
           len             length of the byte stream
       OUTPUTS:
           dest            pointer to stream of converted bytes

       RETURNS:            
           None.
           
       AUTHOR:             Abhishek Verma
*************************************************************************************/
void convert_int_to_ascii( pBYTE source, pBYTE dest, INT len )
{
   INT  temp;
   BYTE temp_byte;

   for( temp = 0; temp < len; temp++ )
   {
      temp_byte = *(source + temp);
      *(dest + temp) = itoa();
   }

   return;
} /* convert_int_to_ascii */





/*************************************************************************************
       NAME:               convert_to_ascii

       DESCRIPTION:        converts a stream of bytes from EBCDIC to ASCII

       INPUTS:
           source          pointer to a stream of bytes to be converted
           len             length of the byte stream
       OUTPUTS:
           dest            pointer to stream of converted bytes

       RETURNS:            
           None.
           
       AUTHOR:             Ram Malathkar
*************************************************************************************/
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



/*************************************************************************************
       NAME:               convert_to_ebcdic

       DESCRIPTION:        converts a stream of bytes from ASCII to EBCDIC

       INPUTS:
           source          pointer to a stream of bytes to be converted
           len             length of the byte stream
       OUTPUTS:
           dest            pointer to stream of converted bytes

       RETURNS:            
           None.
           
       AUTHOR:             Ram Malathkar
*************************************************************************************/
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

/*****************************************************************************/
/******************************************************************************
 *
 *  NAME:         HEX_TO_EBCDIC
 *
 *  DESCRIPTION:  This function converts a byte string from HEX format
 *                into EBCDIC format.  It does this by first converting
 *                from hex to ascii.  Then it calls an existing function
 *                to convert from ascii to ebcdic.
 *
 *  INPUTS:       instr - Hex string to be converted to EBCDIC
 *                len   - Number of bytes in 'instr'
 *
 *  OUTPUTS:      outstr - The converted string in EBCDIC format
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
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

/******************************************************************************
 *
 *  NAME:         EBCDIC_TO_HEX
 *
 *  DESCRIPTION:  This function converts a byte string from EBCDIC format
 *                into HEX format.  It does this by first converting
 *                from ebcdic to ascii by calling an existing function.
 *                Then it converts from ascii to hex.
 *
 *  INPUTS:       instr - EBCDIC string to be converted to HEX
 *                len   - Number of bytes in 'instr'. Maximum len is 100.
 *
 *  OUTPUTS:      outstr - The converted string in HEX format
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
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

  Function:    asc_to_bcd
  Description: This function will convert an ascii string to bcd.
  Author: 
      unknown
  Inputs:
      afield - a pointer to the ascii string to convert
      how_many - number of characters in p_astr to convert
  Outputs:
      bstring - the bcd string 
  Return values:
      None
  Modified by:
      FArad      8/14/98
******************************************************************************/
 void asc_to_bcd (afield, how_many, bstring)
   pCHAR         afield;
   INT           how_many;
   BYTE          bstring[];
{  
   pCHAR  astr;
   UINT   i = 0;

   astr = afield;
   
   for (i = (strlen(afield) - how_many); i < strlen(afield); i=i+2)
   {
      if ('\0' == astr [i])
         break;

      if (astr[i] != ' ')
         bstring[i/2] = ((astr[i]-'0')*16);

      if (((i+1) < strlen(afield)) && (astr[i+1] != ' '))
         bstring[i/2] += (astr[i+1]-'0');

   }
   return;
   
} /* asc_to_bcd */

/*****************************************************************************/
/*****************************************************************************

  Function:    bcd_to_asc

  Description: This function will convert bcd string to an ascii string 
  Author: 
      Farad       8/13/98
  Inputs:
      bstring  - the bcd string to conver to ascii
      blen     - the length of bstring
  Outputs:
      astr     - the new ascii string
      astr_len - the length of astr
  Return values:
      None
  Modified by:
******************************************************************************/
void bcd_to_asc (bstring, blen, astr, astr_len)
   BYTE bstring[];
   INT          blen;
   pCHAR        astr;
   INT          astr_len;
{
   int  i = 0;

   for (i = 0; i < (blen * 2); i=i+2)
      {
      astr[i] = (bstring[i/2] / 16) + '0';
      astr[i+1] = (bstring[i/2] & 0x0F) + '0';
      }

   astr [(blen * 2)] = '\0';
   return;
} /* bcd_to_asc */
