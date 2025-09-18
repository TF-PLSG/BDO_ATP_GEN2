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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\MYUTILS.H  $
   
      Rev 1.7   Dec 01 2003 11:29:48   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.6   Feb 27 2003 10:05:16   lmitchel
   SCR955 - Modifications to expand the following key fields from 16 to 48 bytes:  IDC_CWK_FLD, IDC_ZMK_FLD, IDC_TPK_FLD.  Added validation routines to verify field entry is in Hex digits only and to check length of key entered.
   
      Rev 1.5   Jan 05 2000 14:53:44   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.2   Sep 30 1999 16:17:20   npogosya
   Bug fixes
   
      Rev 1.1   Sep 22 1999 15:51:28   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:38   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:42   iarustam
   Initial Release
   
      Rev 1.2   Apr 29 1999 14:56:28   iarustam
   Bug fixes
   
      Rev 1.1   Mar 08 1999 17:20:32   iarustam
   Bug fixes
   
   
      Rev 1.0   Feb 23 1999 11:40:36   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:32   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:59:00   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:08:18   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:42:08   iarustam
    
*
************************************************************************************/
#ifndef MYUTILS
#define MYUTILS
#include "basictyp.h"
#include <winver.h>

class CMyUtils : public CWnd
{
// Construction
public:
	HANDLE m_hFileMappingObject ;
	int IsEmpty (CString& strStringToCheck, CDialog* _this) ;
	BOOLEAN IfExistsPreviousInstance() ;
	BOOLEAN ActivateCurrentInstance() ;
	CMyUtils() ;

	void GetSystemDate( pCHAR strDate);
    void GetSystemTime( pCHAR strDate);
	CString GenerateAmount ( CString, int );
	CString GetAmount( CString );
    CString DisplayAmount ( CString );
    CString GenerateTranAmount( CString , int , pCHAR  );
	INT GetMod10CheckDigit( pCHAR, INT );
	CString CMyUtils::GenerateDate( CString strMyDate );
	BOOLEAN CMyUtils::IsHexEntry(CString strKeyString);  
	CString GetProductVersionInfo(HMODULE hLib, CString csEntry);
	
	CString FormatDate( CString strMyDate );


    SYSTEMTIME SystemTime;
// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyUtils)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMyUtils();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMyUtils)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif
