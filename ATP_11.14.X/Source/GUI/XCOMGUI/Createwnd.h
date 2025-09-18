#if !defined(AFX_CREATEWND_H__EBBB47B1_1E30_43AF_9F04_DCE402269DE8__INCLUDED_)
#define AFX_CREATEWND_H__EBBB47B1_1E30_43AF_9F04_DCE402269DE8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Createwnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCreatewnd dialog

class CCreatewnd : public CDialog
{
// Construction
public:
	CCreatewnd(CWnd* pParent = NULL);   // standard constructor

		//User Defined Objects
	char Createwnd_NAME[20];
	char Createwnd_NoBytes[10];
	char Createwnd_NoMsgs[10];

	CEdit Createwnd_Edit_Box1, Createwnd_Edit_Box2, Createwnd_Edit_Box3;
	CStatic create_name;
	BOOL OK_f,Cancel_f;
	BOOL Que_f, Mem_f;

// Dialog Data
	//{{AFX_DATA(CCreatewnd)
	enum { IDD = IDD_DIALOG2 };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCreatewnd)
	public:
	virtual int DoModal();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCreatewnd)
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CREATEWND_H__EBBB47B1_1E30_43AF_9F04_DCE402269DE8__INCLUDED_)
