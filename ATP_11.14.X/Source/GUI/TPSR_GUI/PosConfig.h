#if !defined(AFX_POSCONFIG_H__F78E77B7_84FB_4B13_A828_E89F413287E0__INCLUDED_)
#define AFX_POSCONFIG_H__F78E77B7_84FB_4B13_A828_E89F413287E0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PosConfig.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPosConfig dialog

class CPosConfig : public CPropertyPage
{
	DECLARE_DYNCREATE(CPosConfig)

// Construction
public:
	CPosConfig();
	~CPosConfig();

// Dialog Data
	//{{AFX_DATA(CPosConfig)
	enum { IDD = IDD_POS_CONFIG };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPosConfig)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPosConfig)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POSCONFIG_H__F78E77B7_84FB_4B13_A828_E89F413287E0__INCLUDED_)
