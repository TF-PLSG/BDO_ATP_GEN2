// ArchiveLookupDlg.h : header file
//

#if !defined(AFX_ARCHIVELOOKUPDLG_H__0963E5F4_8724_4517_9B93_5CA938840DCC__INCLUDED_)
#define AFX_ARCHIVELOOKUPDLG_H__0963E5F4_8724_4517_9B93_5CA938840DCC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ListViewCtrlEx.h"
#include "MYUTILS.H"
#include "GetTLFData.h"
/////////////////////////////////////////////////////////////////////////////
// CArchiveLookupDlg dialog

class CArchiveLookupDlg : public CDialog
{
// Construction
public:
	BOOL ValidateDate ( CString strDate );
	CArchiveLookupDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CArchiveLookupDlg();
	CGetTLFData *m_pGetTLF;

	CVector <CString> arrColumnNames;
	ARCHIVE_TLF01_LIMITS sTLFLimRecord;
	ARCHIVE_TLF01_GUI_LIST sTLFList;
	bool bSearching;
	CMapStringToString tlfCardMap;

// Dialog Data
	//{{AFX_DATA(CArchiveLookupDlg)
	enum { IDD = IDD_ARCHIVELOOKUP_DIALOG };
	CButton	m_ChkMaskCard;
	CButton	m_CntlNext;
	CButton	m_CntlSearch;
	CButton	m_CntlPause;
	CButton	m_CntlStopSearch;
	CStatic	m_CntlStatus;
	CEdit	m_CntlTerminalID;
	CEdit	m_CntlStartDate;
	CEdit	m_CntlOperatorID;
	CEdit	m_CntlMerchantID;
	CEdit	m_CntlEndDate;
	CEdit	m_CntlCardNbr;
	CEdit	m_CntlAuthResponseID;
	CListViewCtrlEx	m_ArchiveList;
	CString	m_szAuthResponseID;
	CString	m_szCardNbr;
	CString	m_szEndDate;
	CString	m_szMerchantID;
	CString	m_szOperatorID;
	CString	m_szStartDate;
	CString	m_szTerminalID;
	CString	m_szStatus;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CArchiveLookupDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CArchiveLookupDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	afx_msg void OnPause();
	afx_msg void OnStop();
	afx_msg void OnDblclkArchiveList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNext100();
	afx_msg void OnClose();
	virtual void OnCancel();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMask();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ARCHIVELOOKUPDLG_H__0963E5F4_8724_4517_9B93_5CA938840DCC__INCLUDED_)
