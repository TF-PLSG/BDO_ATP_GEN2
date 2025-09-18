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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\PanProcesPage.h  $
   
      Rev 1.5   Dec 01 2003 11:30:02   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.4   Dec 23 1999 16:03:24   mbysani
   modified based on the documents
   
      Rev 1.1   Sep 22 1999 15:57:56   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:06   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:14   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:56:54   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:41:02   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:54   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:59:54   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:09:32   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:44:32   iarustam
    
*
************************************************************************************/
// PanProcesPage.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CPanProcesPage dialog
#ifndef PROCPAGE
#define PROCPAGE

class CPanProcesPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CPanProcesPage)

// Construction
public:
	CPanProcesPage();
	~CPanProcesPage();

	int DialogMode;
	
	CTime c_time;
// Dialog Data
	//{{AFX_DATA(CPanProcesPage)
	enum { IDD = IDD_PANPROCES };
	CEdit	m_CntlAppSource;
	CEdit	m_CntlCompName;
	CEdit	m_CntlCreditLimit;
	CEdit	m_CntlCustNbr;
	CEdit	m_CntlRelatedAccountNbrFld;
	CComboBox	m_CntlRelatedAccountType;
	CEdit	m_CntlPastAmountFld;
	CEdit	m_CntlMinPayFld;
	CEdit	m_CntlCorpAccountNumFld;
	CEdit	m_CntlBlockStatusFld;
	CEdit	m_CntlAccountNameFld;
	CEdit	m_Type;
	CButton	m_StatList;
	CButton	m_ProfList;
	CEdit	m_panedit;
	CEdit	m_VipProc;
	CEdit	m_AccStatus;
	CStatic	m_ComEdit;
	CString	m_panstr;
	CString	m_strAccStatus;
	CString	m_strVipProc;
	CString	m_strComEdit;
	CString	m_strType;
	CString	m_StrAccountNameFld;
	CString	m_StrBlockStatusFld;
	CString	m_StrCorpAccountNumFld;
	CString	m_StrMinPayFld;
	CString	m_StrPastAmountFld;
	CString	m_StrCompName;
	CString	m_StrCustrNbr;
	CString	m_StrAppSource;
	CString	m_StrCreditLimit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPanProcesPage)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPanProcesPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeVipProc();
	afx_msg void OnChangeAccStatus();
	afx_msg void OnProflist();
	afx_msg void OnStatuslist();
	afx_msg void OnChangeBlockStatusFld();
	afx_msg void OnChangeCorpAccountFld();
	afx_msg void OnChangeAccountNameFld();
	afx_msg void OnChangeRelatedAcctNbrFld();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

#endif
