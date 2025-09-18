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
* 
    
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
    CString DisplayAmount ( CString );
    CString GenerateTranAmount( CString , int , pCHAR  );


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
