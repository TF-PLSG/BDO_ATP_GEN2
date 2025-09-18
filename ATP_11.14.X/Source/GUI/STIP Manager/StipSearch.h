#if !defined(AFX_STIPSEARCH_H__CD301903_D1D0_4F2D_AB6A_3882B57CD3A8__INCLUDED_)
#define AFX_STIPSEARCH_H__CD301903_D1D0_4F2D_AB6A_3882B57CD3A8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StipSearch.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CStipSearch dialog

class CStipSearch : public CDialog
{
// Construction
public:
	CStipSearch(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CStipSearch)
	enum { IDD = _UNKNOWN_RESOURCE_ID_ };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStipSearch)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CStipSearch)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STIPSEARCH_H__CD301903_D1D0_4F2D_AB6A_3882B57CD3A8__INCLUDED_)
