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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\TerminalCapabilities.h  $
   
      Rev 1.5   Dec 01 2003 11:30:18   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.4   Jan 05 2000 14:54:30   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 16:01:10   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:24   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:32   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:57:08   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:41:22   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:10:10   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 11:00:20   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:10:04   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:44:56   iarustam
    
*
************************************************************************************/
// TerminalCapabilities.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTerminalCapabilities dialog
#ifndef TERMINALCAP
#define TERMINALCAP


class CTerminalCapabilities : public CPropertyPage
{
	DECLARE_DYNCREATE(CTerminalCapabilities)

// Construction
public:
	CTerminalCapabilities();
	~CTerminalCapabilities();
	CTime c_time;
// Dialog Data
	//{{AFX_DATA(CTerminalCapabilities)
	enum { IDD = IDD_TERMINAL_CAPABILITIES };
	CEdit	m_CntlDebitReturnFld;
	CEdit	m_CntlDebitPurchaseFld;
	CEdit	m_CntlDebitPreauthFld;
	CEdit	m_CntlDebitManualFld;
	CEdit	m_CntlDebitCashbackFld;
	CEdit	m_CntlDebitCashFld;
	CEdit	m_CntlDebitAuthFld;
	CEdit	m_CntlCreditReturnFld;
	CEdit	m_CntlCreditPurchaseFld;
	CEdit	m_CntlCreditPreauthFld;
	CEdit	m_CntlCreditManualFld;
	CEdit	m_CntlCreditCashbackFld;
	CEdit	m_CntlCreditCashFld;
	CEdit	m_CntlCreditAuthFld;
	CString	m_StrCreditAuthFld;
	CString	m_StrCreditCashFld;
	CString	m_StrCreditCashbackFld;
	CString	m_StrCreditManualFld;
	CString	m_StrCreditPreauthFld;
	CString	m_StrCreditPurchaseFld;
	CString	m_StrCreditReturnFld;
	CString	m_StrDebitAuthFld;
	CString	m_StrDebitCashFld;
	CString	m_StrDebitManualFld;
	CString	m_StrDebitPreauthFld;
	CString	m_StrDebitPurchaseFld;
	CString	m_StrDebitReturnFld;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTerminalCapabilities)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CTerminalCapabilities)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
#endif