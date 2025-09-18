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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\DAY.CPP  $
   
      Rev 1.5   Dec 01 2003 11:29:12   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.4   Jan 05 2000 14:53:20   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:47:54   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:16   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:24   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:56:14   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:20   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:22   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:58:32   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:07:44   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:43:10   iarustam
    
*
************************************************************************************/
// this is the implementation file for class CDay

#include "stdafx.h"
#include "Day.h"



//=============================================================================================
// the constructor

CDay::CDay()
{
	m_Day = 0;
	m_Month = 0;
	m_Year = 0;
	m_WorkingDay = FALSE;
	m_JobProfile.Empty();
}

//============================================================================================
CDay::~CDay()
{
	
}

//==========================================================================================
// checks the jobprofile member variable, if its not empty returns true, else returns false
BOOL CDay::HasProfile(void)
{
	if ( m_JobProfile.IsEmpty())
		return FALSE;
	
	return TRUE;
}



