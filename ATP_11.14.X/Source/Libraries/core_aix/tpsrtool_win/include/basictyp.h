/******************************************************************************

   basictyp.h

   Copyright (c) 1995, Hypercom, Inc.
   All Rights Reserved.

   Purpose:   Recommended set of standard typedefs and defines.

   This file should be included in all 'C' source code files.

   ----------------------------------------------------------------------------

   Author:     Mark Vanden Boom

   Modification History:

   $Log:   N:\PVCS\PTE\CORE\PTE_LIB\BASICTYP.H  $
   
      Rev 1.11   Feb 01 2000 10:11:16   MSALEH
   increased MAX_APP_NAME_SIZE to 256
   
      Rev 1.10   Nov 10 1999 17:51:04   MSALEH
   added MAX_INSTANCE_NAME_SIZE 51
   to hold the complete xipc login string
   
      Rev 1.9   Sep 28 1999 14:37:48   MSALEH
   Aix port modifications
   
      Rev 1.8   May 01 1998 07:48:02   drodden
   Added the system name max length definition.
   
   
      Rev 1.7   Jan 21 1998 10:07:40   drodden
   Added the pBOOLEAN type.
   
   
      Rev 1.6   Jan 07 1998 10:29:46   drodden
   Added support for the current/last tran IDs in the PTE msgs.
   Increased the tran ID size to 24.
   
   
      Rev 1.5   Dec 17 1997 15:41:36   drodden
   Removed a (void*) typecast from the null_ptr definition.  Had not intended
   to check this in (it was just a test).
   
   
      Rev 1.4   Dec 17 1997 09:36:20   drodden
   Moved some of the common size definitions to BASICTYP
   from PTEMSG and APP_INFO.
   
   
      Rev 1.3   Sep 30 1997 11:10:06   drodden
   Changed NULL_STR to be "".
   
   
      Rev 1.2   19 Sep 1997 14:37:02   mvandenb
   Added 'NULL_STR' for string checking and comparisons.
   
      Rev 1.1   17 Sep 1997 09:00:36   mvandenb
   Changed BOOL to BOOLEAN to avoid future conflicts with
   definitions in windef.h and other tool/library conflicts.
   
      Rev 1.0   15 Sep 1997 16:06:04   mvandenb
   Initial version of PTE 'C' types for use in all 'C' source files.
   
      Rev 1.0   15 Sep 1997 15:48:06   mvandenb
   Initial version of 'C' types for use in all 'C' source files.

******************************************************************************/


#ifndef BASICTYP
#define BASICTYP


/*
 * Definitions for maximum array sizes.
 */
#define MAX_APP_NAME_SIZE        256
#define MAX_QUE_NAME_SIZE        16
#define MAX_SYSTEM_NAME_SIZE     16
#define MAX_ITOA_SIZE            12
#define MAX_TRAN_ID_SIZE         24
#define MAX_INSTANCE_NAME_SIZE   51 


//Krishna@TF, 14.6.11: Added for preventing old exe's from conecting to Applnk
#define	MAX_APPVERSION_SIZE   	 20
#define MAX_LEN_PCISTR			 3  
#define PCISTR_FOR_VERSION_VALIDATION	"PCI"
#define PCI_MIN_VERSION_INDEX    '5'



/*
 * For pointer initialization and comparisons.
 */
#define  NULL_PTR    0


/*
 * For use in string manipulation and comparisons.
 */
#define  NULL_STR    ""


/*
 * PRIVATE is defined to show the intent to hide data, rather than
 * make it persistent beyond a function's scope.
 */
#define  PRIVATE  static


/*
 * Set up the fundamental character type and various pointers to it.
 */
#ifndef MMCOS_H
typedef char         CHAR;
#endif
typedef char*        pCHAR;
typedef char**       ppCHAR;
typedef char*  const pcCHAR;
typedef char** const ppcCHAR;


/*
 * Set up the same for the unsigned character type and pointers to it.
 */
typedef unsigned char         BYTE;
typedef unsigned char*        pBYTE;
typedef unsigned char**       ppBYTE;
typedef unsigned char*  const pcBYTE;
typedef unsigned char** const ppcBYTE;


/*
 * Short integer types - signed.
 */
typedef signed short         SHORT;
typedef signed short*        pSHORT;
typedef signed short**       ppSHORT;
typedef signed short*  const pcSHORT;
typedef signed short** const ppcSHORT;


/*
 * Short integer types - unsigned.
 */
typedef unsigned short         WORD;
typedef unsigned short*        pWORD;
typedef unsigned short**       ppWORD;
typedef unsigned short*  const pcWORD;
typedef unsigned short** const ppcWORD;


/*
 * Default integer types - signed.
 */
#ifndef MMCOS_H
typedef signed int         INT;
#endif
typedef signed int*        pINT;
typedef signed int**       ppINT;
typedef signed int*  const pcINT;
typedef signed int** const ppcINT;


/*
 * Default integer types - unsigned.
 */
typedef unsigned int         UINT;
typedef unsigned int*        pUINT;
typedef unsigned int**       ppUINT;
typedef unsigned int*  const pcUINT;
typedef unsigned int** const ppcUINT;


/*
 * Long integer types - signed.
 */
typedef signed long         LONG;
typedef signed long*        pLONG;
typedef signed long**       ppLONG;
typedef signed long*  const pcLONG;
typedef signed long** const ppcLONG;


/*
 * Long integer types - unsigned.
 */
typedef unsigned long         DWORD;
typedef unsigned long*        pDWORD;
typedef unsigned long**       ppDWORD;
typedef unsigned long*  const pcDWORD;
typedef unsigned long** const ppcDWORD;


/*
 * Define pointers to functions of various types.
 */
typedef unsigned int       ( *pFUNCint )();

#ifdef WIN32
typedef unsigned int const ( *pcFUNCint )();
#endif


typedef void               ( *pFUNCvoid )();

#ifdef WIN32
typedef void const         ( *pcFUNCvoid )();
#endif


/*
 * Define a pointer to data.
 *
 */
typedef unsigned char* pDATA;


/*
 * Define a type for addresses.
 */
typedef void*  ADDR;


/*
 * Define a type for offsets
 */
typedef DWORD  OFFSET;


/*
 * Define a basic Boolean type and values to use for setting.
 */
typedef unsigned char  BOOLEAN;
typedef unsigned char* pBOOLEAN;

#ifndef  false
#define  false    0x00
#endif

#ifndef  true
#define  true     0x01
#endif


#endif
