#if !defined(AFX_BCH11LOOKUP_H__EEB9ACAD_A553_42ED_B6FF_BB7340B9EB3A__INCLUDED_)
#define AFX_BCH11LOOKUP_H__EEB9ACAD_A553_42ED_B6FF_BB7340B9EB3A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BCH11Lookup.h : header file
//

#include "ListViewCtrlEx.h"
#include "MYUTILS.H"
#include "GetTLFData.h"
/////////////////////////////////////////////////////////////////////////////
// CBCH11Lookup dialog

class CBCH11Lookup : public CDialog
{
// Construction
public:
	CBCH11Lookup(CWnd* pParent = NULL);   // standard constructor
	CVector <CString> arrColumnNames;
	ARCHIVE_BCH11 sBCH11Record;
	ARCHIVE_BCH11_GUI_LIST sBCH11List;
	CGetTLFData *m_pGetTLF;
	CRect m_ListRect;
	bool bSearching;
// Dialog Data
	//{{AFX_DATA(CBCH11Lookup)
	enum { IDD = IDD_BCH11LOOKUP };
	CButton	m_CntlNext;
	CStatic	m_CntlStatus;
	CListViewCtrlEx	m_CntlBCHList;
	CString	m_szBatchNbr;
	CString	m_szDeviceID;
	CString	m_szMerchantID;
	CString	m_szOpenDate;
	CString	m_szStatus;
	CString	m_szRetrievalRefNum;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBCH11Lookup)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBCH11Lookup)
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

#endif // !defined(AFX_BCH11LOOKUP_H__EEB9ACAD_A553_42ED_B6FF_BB7340B9EB3A__INCLUDED_)
