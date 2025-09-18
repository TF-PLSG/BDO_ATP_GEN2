#if !defined(AFX_CARDMERCHANTDEVDLG_H__3D86C03A_64DB_4377_9E91_6631B47D3DDB__INCLUDED_)
#define AFX_CARDMERCHANTDEVDLG_H__3D86C03A_64DB_4377_9E91_6631B47D3DDB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CardMerchantDevDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCardMerchantDevDlg dialog

class CCardMerchantDevDlg : public CPropertyPage
{
// Construction
	DECLARE_DYNCREATE(CCardMerchantDevDlg)

public:
	CCardMerchantDevDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCardMerchantDevDlg)
	enum { IDD = IDD_CARD_DEV_MERCHANT };
	CEdit	m_CntlDeviceNbrListFile;
	CEdit	m_CntlCardNbrListFile;
	CString	m_StrCardNbrListFile;
	CString	m_StrDeviceNbrListFile;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCardMerchantDevDlg)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCardMerchantDevDlg)
	afx_msg void OnBrowseCardFile();
	afx_msg void OnBrowseDeviceFile();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CARDMERCHANTDEVDLG_H__3D86C03A_64DB_4377_9E91_6631B47D3DDB__INCLUDED_)
