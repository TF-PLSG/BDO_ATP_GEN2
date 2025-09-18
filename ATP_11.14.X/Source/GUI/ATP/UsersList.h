#if !defined(AFX_USERSLIST_H__5BAD8A29_D6D4_4D30_A23C_FB86CEFF9A8B__INCLUDED_)
#define AFX_USERSLIST_H__5BAD8A29_D6D4_4D30_A23C_FB86CEFF9A8B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UsersList.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CUsersList dialog

class CUsersList : public CDialog
{
// Construction
public:
	CUsersList(CWnd* pParent = NULL);   // standard constructor
	int CalculateTotalUsers(CStringArray &strArr);
	int GetUserRecord(CString username);
	FILE * m_pFile;
	int nNumberReturned;
	CString strBinType;
	CString m_strName;
	CString strPrimaryKey1;
	CString strNetworkType;

    int m_nIndicator;
	int m_top_index;
    
	UCF01_LIST sUserList;
	UCF01 sUserRecord;
// Dialog Data
	//{{AFX_DATA(CUsersList)
	enum { IDD = IDD_USERSLIST };
	CButton	m_CntlStartBtn;
	CButton	m_CntlCancelBtn;
	CStatic	m_CntlStatus;
	CString	m_StrStatus;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUsersList)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CUsersList)
	afx_msg void OnStart();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_USERSLIST_H__5BAD8A29_D6D4_4D30_A23C_FB86CEFF9A8B__INCLUDED_)
