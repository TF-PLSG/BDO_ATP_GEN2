#if !defined(AFX_VOICEAUTHORIZATIONIO_H__20C1140B_714A_11D3_9764_0008C789BA2A__INCLUDED_)
#define AFX_VOICEAUTHORIZATIONIO_H__20C1140B_714A_11D3_9764_0008C789BA2A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VoiceAuthorizationIO.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CVoiceAuthorizationIO dialog

class CVoiceAuthorizationIO : public CDialog
{
// Construction
public:
	CVoiceAuthorizationIO(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CVoiceAuthorizationIO)
	enum { IDD = IDD_VOICE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVoiceAuthorizationIO)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CVoiceAuthorizationIO)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VOICEAUTHORIZATIONIO_H__20C1140B_714A_11D3_9764_0008C789BA2A__INCLUDED_)
