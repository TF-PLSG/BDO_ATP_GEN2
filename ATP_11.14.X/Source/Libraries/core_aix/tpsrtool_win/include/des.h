/******************************************************************************
  
        Module: des.c
  
         Title: DES Encryption APIs
  
   Description: Header file for the NBS DES module

   Application: general   

   $Log:   N:\PVCS\PTE\CORE\PTE_LIB\DES.H  $
   
      Rev 1.1   Sep 28 1999 14:37:48   MSALEH
   Aix port modifications
   
      Rev 1.0   Oct 23 1997 12:18:22   drodden
   This is part of the continued development of the Pinnacle 
   Transaction Environment.  Many changes, refinements,
   and additions were made to the core modules.  I'm checking
   them in now to get a snapshot of this current working version.
   
   
      Rev 1.0   Jun 09 1997 11:10:24   JGRGURIC
   Added as part of the initial Pinnacle
   security implementation for password
   encryption/decryption.
   
******************************************************************************/


extern void _DES ( unsigned short *, unsigned short *, unsigned short , unsigned short * );

extern int DESWeakKey ( unsigned short key[4] );


/* DES( unsigned short text[4], unsigned short key[4], int encrypt, unsigned short work[52] )
   work[52] is four words of key, and 48 words of subkeys 
*/

