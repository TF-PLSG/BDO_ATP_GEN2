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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\DAY.H  $
   
      Rev 1.5   Dec 01 2003 11:29:14   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.4   Jan 05 2000 14:53:20   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:48:00   iarustam
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
   
      Rev 1.1   Jan 21 1999 13:07:46   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:43:10   iarustam
    
*
************************************************************************************/
// this file defines the class CDay, CDay is used with the ATP 
// calendar control to set working days and job profiles. 

#include "stdafx.h"

#ifndef _DAY_
#define _DAY_

class CDay:public CObject
{

private:
	
	unsigned int m_Day;
	unsigned int m_Month;
	unsigned int m_Year;
	BOOL m_WorkingDay;
	CString m_JobProfile;

public:

	CDay();
	~CDay();
	int GetDay(void) { return m_Day;};
	void SetDay( int day) { m_Day = day;};
	int GetMonth(void) { return m_Month; };
	void SetMonth( int month) { m_Month = month; };
	int GetYear(void) { return m_Year; };
	void SetYear( int year ) { m_Year = year; };
	BOOL IsWorkingDay (void) { return m_WorkingDay; };
	void MakeWorkingDay(void) { m_WorkingDay = TRUE; };
	void MakeHoliday(void) { m_WorkingDay = FALSE; };
	CString GetJobProfile(void) { return m_JobProfile; };
	void SetJobProfile(CString job) { m_JobProfile = job; };
	void ClearJobProfile(void) { m_JobProfile.Empty(); };
	BOOL HasProfile(void);

 
};

#endif