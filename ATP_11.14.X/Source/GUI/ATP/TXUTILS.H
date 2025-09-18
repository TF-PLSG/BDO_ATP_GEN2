/*********************************************************************************
* Module:
*
* Title:
*
* Description:
*
* Application:
*
* Author:
*
* Revision History:
*  
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\TXUTILS.H  $
   
      Rev 1.5   Dec 01 2003 11:30:20   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.4   Jan 05 2000 14:54:28   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 16:00:56   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:24   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:32   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:57:08   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:41:22   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:10:10   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 11:00:18   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:10:04   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:44:54   iarustam
    
*
************************************************************************************/

#ifndef _TXUTILS
#define _TXUTILS
#include "basictyp.h"

LONG Txutils_Calculate_Julian_Date (BYTE* yyyymmdd) ;
void Txutils_Calculate_Gregorian_Date(LONG julian, pCHAR yyyymmdd);

#endif