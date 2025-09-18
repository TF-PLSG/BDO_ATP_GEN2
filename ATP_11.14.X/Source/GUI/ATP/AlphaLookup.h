#if !defined(AFX_ALPHALOOKUP_H__14450481_F902_11D3_976E_0008C789BA2A__INCLUDED_)
#define AFX_ALPHALOOKUP_H__14450481_F902_11D3_976E_0008C789BA2A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AlphaLookup.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAlphaLookup dialog

#include "ListCtrlEx.h"

class CAlphaLookup : public CDialog
{
// Construction
public:
	CAlphaLookup(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAlphaLookup)
	enum { IDD = IDD_ALPHA_LOOKUP };
	CButton	m_Next;
	CListCtrlEx	m_CntlList;
	CString	m_StrNameFld;
	CString	m_StrMessageLbl;
	//}}AFX_DATA

   int m_nDialogMode;

   CCF01 sCCFRecord;
   MCF01 sMerchantRecord;
   PAF01 sPAFRecord;

   CString strItem0;
	CString strItem1;
	CTime c_time, shm_time;

   bool bDone;


   void  PopulateCCFListDialog();
   void  PopulatePAFListDialog();
   void  PopulateMCFListDialog();





// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAlphaLookup)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAlphaLookup)
	virtual BOOL OnInitDialog();
	afx_msg void OnInquiry();
	afx_msg void OnNext();
	virtual void OnOK();
	afx_msg void OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ALPHALOOKUP_H__14450481_F902_11D3_976E_0008C789BA2A__INCLUDED_)
