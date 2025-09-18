#if !defined(AFX_BCH20LOOKUP_H__26D8CEE1_105A_4FFC_8C40_BEAF8F6791B5__INCLUDED_)
#define AFX_BCH20LOOKUP_H__26D8CEE1_105A_4FFC_8C40_BEAF8F6791B5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BCH20Lookup.h : header file
//
#include "ListViewCtrlEx.h"
#include "MYUTILS.H"
#include "GetTLFData.h"
/////////////////////////////////////////////////////////////////////////////
// CBCH20Lookup dialog

class CBCH20Lookup : public CDialog
{
// Construction
public:
	CBCH20Lookup(CWnd* pParent = NULL);   // standard constructor
	CVector <CString> arrColumnNames;
	ARCHIVE_BCH20 sBCH20Record;
	ARCHIVE_BCH20_GUI_LIST sBCH20List;
	CGetTLFData *m_pGetTLF;
	bool bSearching;
// Dialog Data
	//{{AFX_DATA(CBCH20Lookup)
	enum { IDD = IDD_BCH20LOOKUP };
	CButton	m_CntlNext;
	CListViewCtrlEx	m_CntlBCHList;
	CStatic	m_CntlStatus;
	CString	m_szBatchNbr;
	CString	m_szDeviceID;
	CString	m_szMerchantID;
	CString	m_szOpenDate;
	CString	m_szStatus;
	CString	m_szRetrievalRefNum;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBCH20Lookup)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBCH20Lookup)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNext();
	afx_msg void OnDblclkBch01List(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClose();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BCH20LOOKUP_H__26D8CEE1_105A_4FFC_8C40_BEAF8F6791B5__INCLUDED_)
