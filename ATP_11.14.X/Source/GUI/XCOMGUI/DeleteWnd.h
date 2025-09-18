#if !defined(AFX_DELETEWND_H__B8DE1274_3DA7_4258_B814_1BB7F2046646__INCLUDED_)
#define AFX_DELETEWND_H__B8DE1274_3DA7_4258_B814_1BB7F2046646__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DeleteWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDeleteWnd dialog

class CDeleteWnd : public CDialog
{
// Construction
public:
	CDeleteWnd(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDeleteWnd)
	enum { IDD = IDD_DIALOG3 };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
	//USER DEFINED OBJECTS
	CStatic del_name;
	BOOL OK_f,Cancel_f;
	BOOL Que_f, Mem_f, User_f;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDeleteWnd)
	public:
	virtual int DoModal();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDeleteWnd)
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DELETEWND_H__B8DE1274_3DA7_4258_B814_1BB7F2046646__INCLUDED_)
