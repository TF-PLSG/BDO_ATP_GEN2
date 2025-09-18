/////////////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif

#include "Resource.h"

// CLoginfrm dialog
#include "Sharedmem.h"
#include "basictyp.h"
#define CANCEL 2

class CLoginfrm : public CDialog
{
// Construction
public:
	CLoginfrm(CWnd* pParent = NULL);   // standard constructor
	short CheckLoginDetails();
	BYTE ValidateLoginParams(char m_userid[], char m_userpwd[], char *error);
	BYTE SendCmdforRequstUcf01Params(pBYTE psStructurePtr, int nStructureSize, 
                         BYTE  app_data_type,
                         pCHAR error_msg );
// Dialog Data
	//{{AFX_DATA(CLoginfrm)
	enum { IDD = IDD_IDLE_LOGIN };
	CString	m_userid;
	CString	m_userpwd;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLoginfrm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support	
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLoginfrm)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
