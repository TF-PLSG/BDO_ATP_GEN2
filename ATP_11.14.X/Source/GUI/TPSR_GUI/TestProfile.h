#if !defined(AFX_TESTPROFILE_H__32C8A40E_98FB_4BD0_9325_B256A06F6B63__INCLUDED_)
#define AFX_TESTPROFILE_H__32C8A40E_98FB_4BD0_9325_B256A06F6B63__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TestProfile.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTestProfile dialog

class CTestProfile : public CPropertyPage
{
// Construction
	DECLARE_DYNCREATE(CTestProfile)

public:
	CTestProfile(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTestProfile)
	enum { IDD = IDD_TEST_PROFILE };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestProfile)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTestProfile)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTPROFILE_H__32C8A40E_98FB_4BD0_9325_B256A06F6B63__INCLUDED_)
