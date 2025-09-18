// stipmgrDlg.h : header file
//
#include "ListCtrlEx.h"

#if !defined(AFX_STIPMGRDLG_H__3699F49B_4AFB_46C0_83B1_18EFF77B7378__INCLUDED_)
#define AFX_STIPMGRDLG_H__3699F49B_4AFB_46C0_83B1_18EFF77B7378__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Lock.h"
#include "SharedMem.h"

#define  IDT_TIMER  WM_USER + 200
/////////////////////////////////////////////////////////////////////////////
// CStipmgrDlg dialog

class CStipmgrDlg : public CDialog
{
// Construction
public:
	CStipmgrDlg(CWnd* pParent = NULL);	// standard constructor
	~CStipmgrDlg();
	TLF01_LIMITS sTLFLimRecord;
	TLF01 sTLFRecord;
	TLF01 sTlf01[GUI_MAX_LIST_SIZE];
	int nNumberReturned;
	NCF01 sNcf01;
	TLF01_GUI_LIST sTLFList[GUI_MAX_LIST_SIZE];
	BYTE strErrorMessage[100];
	BOOL ValidateDate ( CString strDate );
	BOOL ValidateTime ( CString strTime );
	void PopulateTranListDialog();
	int GetSelectedItem();
	BOOL bDone;
	CListCtrl *pcntlList;
	virtual BOOL OnInitDialog();
// Dialog Data
	//{{AFX_DATA(CStipmgrDlg)
	enum { IDD = IDD_STIPMGR_DIALOG };
	CButton	m_cntlCancle;
	CButton	m_Help_Button;
	CButton	m_cntlNext;
	CButton	m_Search_Button;
	CEdit	m_cntlCounter;
	CButton	m_cntlResendAll;
	CListCtrlEx	m_cntlList;
	CComboBox	m_CNBHostType;
	CButton	m_Resend_All_Button;
	CButton	m_Resend_Button;
	CEdit	m_cntlTranDate;
	CEdit	m_cntlMerchantId;
	CEdit	m_cntlCardNum;
	CString	m_strCardNum;
	CString	m_strMerchantId;
	CString	m_strTranDate;
	CString	m_strMessage_Lbl;
	BOOL	m_chkResendAll;
	CString	m_strCounter;
	//}}AFX_DATA
	CString m_strHostTypeSelected;
	
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStipmgrDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

	UINT timer_handle;
	int cntr;
	int idle_time;
	char idle_time_val[7];
	bool StopTimer();
	int StartTimer(int time);
	void DisableEditControls();
	void EnableEditControls();
	void ResetCntr();
	short LockScreen();
	CTime c_time, shm_time;
	int ret, status;
// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CStipmgrDlg)
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSelchangeComboNetworkController();
	afx_msg void OnGetCardNumber();
	afx_msg void OnGetMerchantID();
	afx_msg void OnTranDate();
	//afx_msg int OnResendButton();
	afx_msg void OnResendButton();
	//afx_msg int OnResendAllButton();
	afx_msg void OnResendAllButton();
	afx_msg void OnSearchButton();
	afx_msg void OnChangeRepeatCounter();
	afx_msg void OnSTIPHelp();
	afx_msg void OnNext10Tx();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	CDMLock * pLock;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STIPMGRDLG_H__3699F49B_4AFB_46C0_83B1_18EFF77B7378__INCLUDED_)
