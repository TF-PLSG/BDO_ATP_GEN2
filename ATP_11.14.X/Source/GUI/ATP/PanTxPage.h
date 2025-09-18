/*********************************************************************************
* Module:
*
* Title:
*
* Description:
*
* Application:
*
* Author:
*
* Revision History:
*  
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\PanTxPage.h  $
   
      Rev 1.5   Dec 01 2003 11:30:02   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.4   Jan 05 2000 14:54:16   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:58:14   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:08   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:14   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:56:56   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:41:04   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:56   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:59:58   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:09:34   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:44:36   iarustam
    
*
************************************************************************************/
// PanTxPage.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CPanTxPage dialog
#ifndef TXPAGE
#define TXPAGE

class CPanTxPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CPanTxPage)

// Construction
public:
	CPanTxPage();
	~CPanTxPage();

	int DialogMode;

	CCF10 sTxRecord;
	BOOL  bNext;
    BOOL bDone;
	BOOL m_bTxPage;

	void PopulateTxDialog();
	CTime c_time;
// Dialog Data
	//{{AFX_DATA(CPanTxPage)
	enum { IDD = IDD_PANTX };
	CButton	m_Next;
	CEdit	m_panedit;
	CListCtrl	m_TxList;
	CButton	m_View;
	CStatic	m_ComEdit;
	CString	m_panstr;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPanTxPage)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPanTxPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnNext();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

#endif
