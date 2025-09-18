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
* $Log:   N:\PVCS\PTE\Equitable\tran_lookup\MYUTILS.H  $
   
      Rev 1.1   Jun 24 1999 16:14:06   iarustam
   Bug fixes
   
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
	CString GetAmount_not_inDecimal( CString );
    CString DisplayAmount ( CString );
    CString GenerateTranAmount( CString , int , pCHAR  );
	CString DisplayAmount_with_decplcs ( CString, int );


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
