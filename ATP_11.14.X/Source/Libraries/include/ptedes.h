/******************************************************************************
  
        Module: ptedes.h
  
         Title: Pinnacle DES Encryption APIs
  
   Description: This module provides a set of APIs to encrypt/decrypt data
                using an algorithm based on the DES standard.

   Application: general   

   $Log:   N:\PVCS\PTE\ptedes.h  $
   
      Rev 1.0   Oct 23 1997 11:42:12   drodden
   This is part of the continued development of the Pinnacle 
   Transaction Environment.  Many changes, refinements,
   and additions were made to the core modules.  I'm checking
   them in now to get a snapshot of this current working version.
   

******************************************************************************/

#ifndef PTEDES_H
#define PTEDES_H

#include "basictyp.h"

DWORD gui_password_encryption ( pBYTE buffer, INT buffer_len );

INT des_decryption_ex ( pBYTE buffer, INT buffer_len, pBYTE des_key, INT mode );


INT simple_encryption ( pBYTE buffer, INT buffer_len );
INT simple_decryption ( pBYTE buffer, INT buffer_len );

INT keyed_encryption  ( pBYTE buffer, INT buffer_len, pBYTE des_key, INT mode );
INT keyed_decryption  ( pBYTE buffer, INT buffer_len, pBYTE des_key, INT mode );

INT password_cryption ( pBYTE data,   INT encrypt );

INT vt_encrypt_data   ( pBYTE buffer, INT buffer_len, pBYTE des_key, INT mode );
INT vt_decrypt_data   ( pBYTE buffer, pBYTE des_key );

#endif
