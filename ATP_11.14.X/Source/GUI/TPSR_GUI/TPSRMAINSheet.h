#if !defined(AFX_TPSRMAIN_H__8D8E0979_F098_4E27_8A0D_6624D46036F4__INCLUDED_)
#define AFX_TPSRMAIN_H__8D8E0979_F098_4E27_8A0D_6624D46036F4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TPSRMAINSheet.h : header file
//
#include "TPSR_GUIDlg.h"
#include "CardMerchantDevDlg.h"
#include "TestProfile.h"
#include "TpsGraph.h"
#include "PosConfig.h"
#include "StatusSplashWindow.h"
#include "TPSRMainDlg.h"
#include "CardAndTrackDataDlg.h"
/////////////////////////////////////////////////////////////////////////////
// CTPSRMAINSHEET

class CTPSRMAINSHEET : public CPropertySheet
{
	DECLARE_DYNAMIC(CTPSRMAINSHEET)

// Construction
public:
	CTPSRMAINSHEET(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CTPSRMAINSHEET(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

	CMenu *m_pMainMenu;
	CTPSRMainDlg		m_TpsrMainDlg;
//	CTPSR_GUIDlg		m_TpsrConfigDlg;
	CCardMerchantDevDlg m_CardMerDevDlg;
	CPosConfig			m_PosConfigDlg;
	CTestProfile		m_TestProfileDlg;
	CCardAndTrackDataDlg m_CardAndTrackDataDlg;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTPSRMAINSHEET)
	public:
		virtual BOOL OnInitDialog();
	protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTPSRMAINSHEET();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTPSRMAINSHEET)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TPSRMAIN_H__8D8E0979_F098_4E27_8A0D_6624D46036F4__INCLUDED_)
