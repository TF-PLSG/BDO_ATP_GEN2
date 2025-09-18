#if !defined(AFX_GETTLFDATA_H__4561C43B_F8AA_4191_84B6_3CD15EB4F126__INCLUDED_)
#define AFX_GETTLFDATA_H__4561C43B_F8AA_4191_84B6_3CD15EB4F126__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GetTLFData.h : header file
//

#include "CVector.h"

/////////////////////////////////////////////////////////////////////////////
// CGetTLFData thread

class CGetTLFData : public CWinThread
{
	DECLARE_DYNCREATE(CGetTLFData)
protected:
	CGetTLFData();           // protected constructor used by dynamic creation

// Attributes
public:
	ARCHIVE_TLF01_LIMITS sTLFLimRecord;
	ARCHIVE_TLF01_GUI_LIST sTLFList;
	CVector <ARCHIVE_TLF01> m_vTLFBufferList;
	BOOL bDone;
	bool bStopSearch;
	bool bError;
	unsigned long lTotalCount;
	int nSessionCount;

	ARCHIVE_BCH01 sBCH01Record;
	ARCHIVE_BCH01_GUI_LIST sBCH01List;
	ARCHIVE_BCH11 sBCH11Record;
	ARCHIVE_BCH11_GUI_LIST sBCH11List;
	ARCHIVE_BCH20 sBCH20Record;
	ARCHIVE_BCH20_GUI_LIST sBCH20List;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGetTLFData)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CGetTLFData();

	// Generated message map functions
	//{{AFX_MSG(CGetTLFData)
		// NOTE - the ClassWizard will add and remove member functions here.
		afx_msg void OnGetTLFList(WPARAM wParam, LPARAM lParam);
		afx_msg void OnGetTLFNext(WPARAM wParam, LPARAM lParam);
		afx_msg void OnGetTLFCount(WPARAM wParam, LPARAM lParam);
		afx_msg void OnPause(WPARAM wParam, LPARAM lParam);
		afx_msg void OnStop(WPARAM wParam, LPARAM lParam);
		afx_msg void OnGetBCH01List(WPARAM wParam, LPARAM lParam);
		afx_msg void OnGetBCH01Next(WPARAM wParam, LPARAM lParam);
		afx_msg void OnGetBCH11List(WPARAM wParam, LPARAM lParam);
		afx_msg void OnGetBCH11Next(WPARAM wParam, LPARAM lParam);
		afx_msg void OnGetBCH20List(WPARAM wParam, LPARAM lParam);
		afx_msg void OnGetBCH20Next(WPARAM wParam, LPARAM lParam);

	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GETTLFDATA_H__4561C43B_F8AA_4191_84B6_3CD15EB4F126__INCLUDED_)
