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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\DBPasswordsPropertySheet.h  $
   
      Rev 1.0   Jan 01 2011 11:30:22   TF_Srikanth
   Added DBPasswordsPropertySheet for Storing Dualencryption key control Class    
    
*
************************************************************************************/
/////////////////////////////////////////////////////////////////////////////
// CDBPasswordsPropertySheet header file
/////////////////////////////////////////////////////////////////////////////


#include "DBPasswords.h"



class CDBPasswordsPropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CDBPasswordsPropertySheet)

// Construction
public:
	CDBPasswordsPropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CDBPasswordsPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

// Operations
public:
	CDBPasswords m_DBPasswordsPage;

    int m_nActiveTab;
	CString userName;
//	void SetData();
//	CString m_StrPassword;
int m_dialogmode;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDBPasswordsPropertySheet)
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDBPasswordsPropertySheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDBPasswordsPropertySheet)
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);


	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

