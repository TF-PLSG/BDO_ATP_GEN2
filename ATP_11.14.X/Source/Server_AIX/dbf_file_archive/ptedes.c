/******************************************************************************
  
        Module: ptedes.c
        
         Title: Pinnacle DES Encryption APIs
  
   Description: This module provides a set of APIs to encrypt/decrypt data
                using an algorithm based on the DES standard.

   Application: general   

   $Log:   N:\PVCS\PTE\CORE\PTE_LIB\PTEDES.C  $
   
      Rev 1.3   Oct 15 1999 11:12:22   MSALEH
   Move SSL_SUPPORT to preprocessot definition
   section of project files
   
      Rev 1.2   Sep 28 1999 14:40:48   MSALEH
   AIX Modifications
   
      Rev 1.1   Dec 01 1997 08:52:14   drodden
   Added the string header to eliminate compile warnings.
   
   
      Rev 1.0   Oct 23 1997 11:42:08   drodden
   This is part of the continued development of the Pinnacle 
   Transaction Environment.  Many changes, refinements,
   and additions were made to the core modules.  I'm checking
   them in now to get a snapshot of this current working version.
   

******************************************************************************/

#include <stdlib.h>
#include <string.h>

#ifdef WIN32
#include <io.h>
#endif

#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "basictyp.h"
#include "des.h"

#define SIZEOF_COPYBUFFER 504       /* because we know: message length can't be >500... */

#define LEN_OFFSET        1    /* offset from encryption_offset for: encrypted data length */
#define MODE_OFFSET       4    /* offset from encryption_offset for: encrypted data mode flag */
#define INIT_V_OFFSET     5    /* offset from encryption_offset for: encrypted data initial vector */
#define DATA_OFFSET       21   /* offset from encryption_offset for: encrypted data */ 

static WORD hard_key [4]   = {0x13A7, 0x6135, 0x9CDF, 0xA852};
static WORD hard_key1[4]   = {0x29DE, 0xA73E, 0xB932, 0x12F6};
static BYTE logging        = 0;      /* set to 1 for logging to des_log.txt */

extern	WORD key[];// = {0x13A7, 0x6135, 0x9CDF, 0xA852};


/*****************************************************************************
   function hexchar2word



******************************************************************************/
INT hexchar2word (pWORD dst, pBYTE src)
{
  INT    i, j;
  BYTE   n_src [16];

  for (i=0; i<16; i++)
    {
    if (src[i] <= '9')
      n_src[i] = (BYTE)(src[i] - '0');
    else
      n_src[i] = (BYTE)(src[i] - 'A' + 10);

    /* let's test key information - just for in case... */
    if (n_src[i] > 0xf)
      return 0;
    }

  for (i=0; i<4; i++)
    {
    dst[i] = 0;
    for (j=0; j<4; j++)
      dst[i] |= n_src[(4*i)+j] << (4*(3-j));
    }

  return 1;
}



/*****************************************************************************
   function 



******************************************************************************/
INT hexchar2byte (pBYTE dst, pBYTE src)
{
  INT    i;
  BYTE   n_src [16];

  for (i=0; i<2; i++)
    {
    if (src[i] <= '9')
      n_src[i] = (BYTE)(src[i] - '0');
    else
      n_src[i] = (BYTE)(src[i] - 'A' + 10);

    /* let's test key information - just for in case... */
    if (n_src[i] > 0xf)
      return 0;
    }

  dst[0] = (BYTE)((0xf0&(n_src[0]<<4)) | (0x0f&n_src[1]));
  return 1;
}



/*****************************************************************************
   function 



******************************************************************************/
void byte2hexchar ( pBYTE dst, pBYTE src, INT how_many )
{
  INT    i;
  CHAR   tmp [3];

  for (i=0; i < how_many; i++)
    {
    sprintf (tmp, "%02X", src[i]);
    dst[(2*i)]   = tmp [0];
    dst[(2*i)+1] = tmp [1];
    }
}



/*****************************************************************************
   function 

   copy data from 8 char to 4 INT (for DES) unfortunatly, we have INT DES...


******************************************************************************/
void byte2word (pWORD dst, pBYTE src)
{
  INT    i;

  for (i=0; i<4; i++)
    {
    dst[i] = src[2*i];
    dst[i] = ((dst[i]<<8) | src[(2*i)+1]);
    }
}



/*****************************************************************************
   function 

   copy data back


******************************************************************************/
void word2byte (pBYTE dst, pWORD src)
{
  INT    i;

  for (i=0; i<4; i++)
    {
    dst[2*i]   = (BYTE)(src[i]>>8);
    dst[(2*i)+1] = (BYTE)src[i];
    }
}



/*****************************************************************************
   function 



******************************************************************************/
void add_log (char* comment, pBYTE buffer, INT buffer_len)
{
  INT    i;
  CHAR   buf[10];
  INT    file;

   
  if (!logging)
    return;

#ifdef WIN32
  file = open ("des_log.txt", _O_APPEND|_O_CREAT|_O_TEXT|_O_RDWR, _S_IREAD|_S_IWRITE);
#else
  file = open ("des_log.txt", O_APPEND|O_CREAT|O_RDWR, S_IREAD|S_IWRITE);
#endif

  if (file == -1)
    return;

  if (comment != 0)
    {
    write(file, "\n", 1);
    write(file, comment, strlen(comment));
    }

  if ((buffer_len != 0) && (buffer != NULL))
    {
    for (i=0; i<buffer_len; i++)
      {
      if (!(i%40))
        write(file, "\n", 1);

      sprintf(buf, "%02X", buffer[i]);
      write(file, buf, strlen(buf));
      }
    }

  write(file, "\n", 1);

  close(file);
}



/*****************************************************************************
   function 



******************************************************************************/
void add_log_ascii (char* comment, pBYTE buffer, INT buffer_len)
{
  INT    i;
  CHAR   buf[10];
  INT    file;

  if (!logging)
    return;

#ifdef WIN32
  file = open ("des_log.txt", _O_APPEND|_O_CREAT|_O_TEXT|_O_RDWR, _S_IREAD|_S_IWRITE);
#else
  file = open ("des_log.txt", O_APPEND|O_CREAT|O_RDWR, S_IREAD|S_IWRITE);
#endif

  if (file == -1)
    return;

  if (comment != 0)
    {
    write(file, "\n", 1);
    write(file, comment, strlen(comment));
    }

  if ((buffer_len != 0) && (buffer != NULL))
    {
    for (i=0; i<buffer_len; i++)
      {
      if (!(i%80))
        write(file, "\n", 1);

      sprintf(buf, "%c", buffer[i]);
      write(file, buf, 1);
      }
    }

  write(file, "\n", 1);

  close(file);
}



/*****************************************************************************








******************************************************************************/
INT encrypt_key ( pBYTE key_in, pBYTE key_out )
{
  INT    i;
  WORD   ukey [4];
  WORD   work [52];

  if (!hexchar2word(ukey, key_in))
    return 0;

  memset(work, 0, sizeof(work));

  _DES ((unsigned short*)ukey, (unsigned short*)hard_key, (unsigned short)1, (unsigned short*)work);

  /* put key to it place... */
  for (i=0; i<4; i++)
    sprintf((char*)(key_out+(4*i)), "%04X", ukey[i]);

  return 1;
}



/*****************************************************************************








******************************************************************************/
INT decrypt_key ( pBYTE key_in, pWORD key_out )
{
  WORD   work [52];


  /* get key... */
  if (!hexchar2word(key_out, key_in))
    return 0;

  memset (work, 0, sizeof(work));

  _DES ((unsigned short*)key_out, (unsigned short*)hard_key, (unsigned short)0, (unsigned short*)work);

  return 1;
}



/*****************************************************************************








******************************************************************************/
INT des_encryption ( pBYTE buffer, INT buffer_len, pWORD des_key, INT mode )
{
  INT    offset, i;
  WORD   temp1[4];
  WORD   temp2[4];
  WORD   work [52];
  BYTE   buf_copy [SIZEOF_COPYBUFFER];


  if (buffer_len > SIZEOF_COPYBUFFER)
    return 0;

  /* make a copy of buffer */
  memset(buf_copy, 0, SIZEOF_COPYBUFFER);
  memcpy(buf_copy, buffer, buffer_len);

  if ((buffer_len % 8) != 0)
    buffer_len+= (8 - (buffer_len % 8));

  /* initial vector is 0s */ 
  memset (temp1, 0x00, sizeof(temp1));

  for (offset=0; offset < buffer_len; offset=offset+8)
    {
    /* take data from line */
    byte2word (temp2, &buf_copy [offset]);

    /* exclusive or between data and init vector */
    for (i=0; i<4; i++)
      temp1 [i] ^= temp2[i];

    memset (work, 0, sizeof(work));

    _DES ((unsigned short*)temp1, (unsigned short*)des_key, (unsigned short)1, (unsigned short*)work);

    /* put encrypted data back... */
    word2byte (&buf_copy [offset], temp1);
    }  /* for */

  if (mode == 1)
    {
    byte2hexchar (buffer, buf_copy, offset);
    offset *= 2;
    buffer[offset] = 0x00;
    }
  else
    memcpy (buffer, buf_copy, offset);
    
  return (offset);
}



/*****************************************************************************/
INT des_decryption ( pBYTE buffer, INT buffer_len, pWORD des_key, INT mode );

void DecryptPassword(pCHAR src, pCHAR dest)
{
    des_decryption ( (pBYTE)src, strlen(src), key, 1);
    strcpy(dest, src);
}








/******************************************************************************/
INT des_decryption ( pBYTE buffer, INT buffer_len, pWORD des_key, INT mode )
{
  INT    offset, i;
  WORD   temp1[4];
  WORD   temp2[4];
  WORD   work [52];
  BYTE   buf_copy [SIZEOF_COPYBUFFER];

  if (buffer_len % 8)
    return 0; /* Error! The length must be divisible by 8! */

  if (buffer_len == 0)
    return 0; /* No Error, maybe this is a command */

  if (buffer_len > SIZEOF_COPYBUFFER)
    return 0; /* Error! As I found in Pinnacle code, message length can't be bigger then 500 BYTE. */

  memset(buf_copy, 0, SIZEOF_COPYBUFFER);

  if (mode == 1)
    {
    for (i=0; i<(buffer_len/2); i++)
      if (!hexchar2byte(buf_copy+i, buffer+(2*i)))
        return 0;

    /* clear it out: will only be returning half as much */
    memset (buffer, 0x00, buffer_len);
    buffer_len /= 2;
    }
  else
    memcpy (buf_copy, buffer, buffer_len);

  /* initial vector is 0s */ 
  memset (temp1, 0x00, sizeof(temp1));

  for (offset=0; offset < buffer_len; offset=offset+8)
    {
    /* take data from line */
    byte2word (temp2, &buf_copy [offset]);

    /* unDES IT! */
    memset (work, 0, sizeof(work));

    _DES ((unsigned short*)temp2, (unsigned short*)des_key, (unsigned short)0, (unsigned short*)work);

    /* exclusive or between data and init vector */
    for (i=0; i<4; i++)
      temp2 [i] ^= temp1[i];

    /* get encrypted data for next round of decryption... */
    byte2word (temp1, &buf_copy [offset]);

    /* put encrypted data back... */
    word2byte (&buffer [offset], temp2);
    } 

  return (offset);
}



/*****************************************************************************








******************************************************************************/
INT simple_encryption ( pBYTE buffer, INT buffer_len )
{
  INT    i;


  add_log_ascii ("\n\n*****Simple Encryption*****", buffer, buffer_len);

  i = des_encryption (buffer, buffer_len, hard_key, 1);

  add_log ("after encryption ", buffer, i);

  return (i);
}



/*****************************************************************************








******************************************************************************/
INT simple_decryption (pBYTE buffer, INT buffer_len)
{
  INT    i;


  add_log_ascii ("\n\n*****Simple Decryption*****", buffer, buffer_len);

  i = des_decryption (buffer, buffer_len, hard_key, 1);

  add_log_ascii ("after decryption", buffer, i);

  return (i);
}



/*****************************************************************************

  






******************************************************************************/
INT keyed_encryption ( pBYTE buffer, INT buffer_len, pBYTE des_key, INT mode )
{
  INT    i;
  WORD ukey[4];


  add_log_ascii ("\n\n*****Keyed Encryption*****", buffer, buffer_len);
  add_log_ascii ("Terminal Key:", des_key, 16);

  /* get key... */
  decrypt_key (des_key, ukey);

  i = des_encryption (buffer, buffer_len, ukey, mode);

  add_log_ascii ("after encryption", buffer, i);

  return (i);
}



/*****************************************************************************








******************************************************************************/
INT keyed_decryption ( pBYTE buffer, INT buffer_len, pBYTE des_key, INT mode )
{
  INT    i;
  WORD   ukey[4];


  add_log_ascii ("\n\n*****Keyed Decryption*****", buffer, buffer_len);
  add_log_ascii ("Server Key:", des_key, 16);
  
  /* get key... */
  decrypt_key (des_key, ukey);

  i = des_decryption (buffer, buffer_len, ukey, mode);

  add_log_ascii ("after decryption", buffer, i);

  return (i);
}



/*****************************************************************************
*  Module:    password_cryption
*
*  Description:
*     Crypts a 16 BYTE password based on a hard coded key.
*     The password is handled as two 8 BYTE segments that are passed individually to the
*     DES routine that encrypts or decrypts them into the proper portion of the output buffer.
*     Encryption will pad the password to 16 BYTEs. Decryption will remove the 16 BYTE pad
*     to return the original password string. This way we avoid encoding the null BYTE and
*     any garbage that follows the string passed in for encryption.
*
*  Inputs:
*     data   : Pointer to the 16 BYTE password string (encrypted or decripted) to convert
*     encrypt: true (non-zero) to encrypt the password
*              false(zero) to decrypt the password
*
*  Outputs:
*     data   : 16 BYTE crypted password is returned.
*     The input password is destroyed.
*     Always returns success.
*
******************************************************************************/
INT password_cryption ( pBYTE data, INT encrypt )
{
  WORD   buffer[4];
  WORD   work [52];
  INT    i;

  if ( encrypt ) {
    /* First blank fill the password to encrypt to 16 chars */
    for (i = 0; i < 16; ++i)
      if (data[i] == 0) {
        while ( i < 16 )
          data[i++] = ' ';  /* Space fill to 16 chars */ 
      }
    data[i] = 0;  /* Null terminate the 16 char string */
  }

  /* Load WORD buffer from the character data buffer.
     Copy data from 8 char to 4 INT (for DES) unfortunatly, we have INT DES... */
  for (i=0; i<4; i++)
    buffer[i] = 0xffff&((data[2*i]<<8) | (0xff&data[2*i+1]));
  
  /* Zero work area */
  memset(work, 0, 52*sizeof(WORD));

  /* Call decryption routine for first part */
  _DES((unsigned short*)buffer, (unsigned short*)hard_key1, (unsigned short)encrypt, (unsigned short*)work);

  /* Output the decrypted data. Copy data back */
  for (i=0; i<4; i++){
    data[2*i] = (BYTE)(buffer[i]>>8);
    data[2*i+1] = (BYTE)buffer[i];
  }

  /* Load WORD buffer from the character data buffer.
     Copy data from 8 char to 4 INT (for DES) unfortunatly, we have INT DES... */
  for (i=0; i<4; i++)
    buffer[i] = 0xffff&(((data + 8)[2*i]<<8) | (0xff&(data + 8)[2*i+1]));
  
  /* Zero work area */
  memset(work, 0, 52*sizeof(WORD));

  /* Call decryption routine for second part */
  _DES((unsigned short*)buffer, (unsigned short*)hard_key1, (unsigned short)encrypt, (unsigned short*)work);

  /* Output the decrypted data. Copy data back */
  for (i=0; i<4; i++){
    (data + 8)[2*i] = (BYTE)(buffer[i]>>8);
    (data + 8)[2*i+1] = (BYTE)buffer[i];
  }

  if ( encrypt == 0 ) {
    /* Remove the trailing spaces and terminate the password after the last non_space
       character */
    for (i = 0; i < 16; ++i)
      if (data[i] == ' ')
        data[i] = 0;
  }

  return 1;
}



/*****************************************************************************




******************************************************************************/
INT vt_encrypt_data ( pBYTE buffer, INT buffer_len, pBYTE des_key, INT mode )
{
  INT    i;
  WORD   ukey[4];
  CHAR   buf[4];
  BYTE   tmp_copy [SIZEOF_COPYBUFFER];


  add_log_ascii ("\n\n*****Terminal Message Encryption*****", buffer, buffer_len);
  add_log_ascii ("Terminal Key:", des_key, 16);

  if (buffer_len > SIZEOF_COPYBUFFER)
    return 0;

  /* buffer preparation */
  /* make a copy of buffer */
  memset (tmp_copy, 0,      SIZEOF_COPYBUFFER);
  memcpy (tmp_copy, buffer, buffer_len);

  /* get key... */
  decrypt_key (des_key, ukey);

  i = des_encryption (tmp_copy, buffer_len, ukey, mode);

  buffer [0] = '>';

  if (mode == 1)
    buffer [MODE_OFFSET] = 'D';
  else
    buffer [MODE_OFFSET] = '4';

  memset (&buffer [INIT_V_OFFSET], '0',      16);
  memcpy (&buffer [DATA_OFFSET],   tmp_copy,  i);

  /* i = correct length of DATA part of message. */
  /* we must also send Initial vector and the mode (17 more) */
  i += 17;

  sprintf (buf, "%03d", buffer_len);
  memcpy  (&buffer [LEN_OFFSET], buf, 3);

  i += 4;   /* 1 for cmd BYTE, 3 for length */
  buffer [i] = 0x00;
   
  add_log_ascii ("after encryption", buffer, i);

  return (i);
}



/*****************************************************************************








******************************************************************************/
INT vt_decrypt_data ( pBYTE buffer, pBYTE des_key )
{
  INT    i, mode;
  WORD   ukey[4];
  CHAR   buf[4];


  if (buffer [0] != '>')
    return (0);

  memcpy (buf, &buffer[LEN_OFFSET], 3);
  buf [3] = 0x00;
  i       = atoi(buf);

  add_log_ascii ("\n\n*****Terminal Message Decryption*****", buffer, i+4);
  add_log_ascii ("Server Key:", des_key, 16);

  if (0x40&buffer[MODE_OFFSET])
    mode = 1;
  else
    mode = 2;

  /* grab the initial vector from the message */

  /*
     i = length of DATA part of message plus initial vector plus mode.
     Be sure to account for that 17 BYTEs when moving the data.
     Move (shift) the data to the front of the buffer and null terminate
     it.  This is what we'll decrypt.
  */
  i -= 17;
  memcpy (buffer, &buffer[DATA_OFFSET], i);
  buffer [i] = 0x00;
  
  /* get key... */
  decrypt_key (des_key, ukey);

  i = des_decryption (buffer, i, ukey, mode);

  buffer [i] = 0x00;

  add_log_ascii ("after decryption", buffer, i);

  return (i);
}



