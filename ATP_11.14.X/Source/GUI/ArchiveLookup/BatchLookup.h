#if !defined(AFX_BATCHLOOKUP_H__6F660667_7413_4EB0_ACFF_8957C8743759__INCLUDED_)
#define AFX_BATCHLOOKUP_H__6F660667_7413_4EB0_ACFF_8957C8743759__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BatchLookup.h : header file
//

#include "ListViewCtrlEx.h"
#include "MYUTILS.H"
#include "GetTLFData.h"
/////////////////////////////////////////////////////////////////////////////
// CBatchLookup dialog

class CBatchLookup : public CDialog
{
// Construction
public:
	CBatchLookup(CWnd* pParent = NULL);   // standard constructor
	CVector <CString> arrColumnNames;
	ARCHIVE_BCH01 sBatchRecord;
	ARCHIVE_BCH01_GUI_LIST sBatchList;
	CGetTLFData *m_pGetTLF;
	bool bSearching;
// Dialog Data
	//{{AFX_DATA(CBatchLookup)
	enum { IDD = IDD_BCH01LOOKUP };
	CButton	m_CntlNext;
	CStatic	m_CntlStatus;
	CListViewCtrlEx	m_CntlBCH01List;
	CString	m_szBatchNbr;
	CString	m_szCloseDate;
	CString	m_szDeviceID;
	CString	m_szMerchantID;
	CString	m_szOpenDate;
	CString	m_szStatus;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBatchLookup)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBatchLookup)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnNext();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDblclkBch01List(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClose();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BATCHLOOKUP_H__6F660667_7413_4EB0_ACFF_8957C8743759__INCLUDED_)
