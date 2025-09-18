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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\TerminalCapabilities.cpp  $
   
      Rev 1.5   Dec 01 2003 11:30:18   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.4   Jan 05 2000 14:54:30   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 16:01:02   iarustam
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
   
      Rev 1.1   Jan 22 1999 11:00:18   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:10:04   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:44:54   iarustam
    
*
************************************************************************************/
// TerminalCapabilities.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "TerminalCapabilities.h"
#include "SharedMem.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CSharedMem shrmem;

/////////////////////////////////////////////////////////////////////////////
// CTerminalCapabilities property page

IMPLEMENT_DYNCREATE(CTerminalCapabilities, CPropertyPage)

CTerminalCapabilities::CTerminalCapabilities() : CPropertyPage(CTerminalCapabilities::IDD)
{
	//{{AFX_DATA_INIT(CTerminalCapabilities)
	m_StrCreditAuthFld = _T("");
	m_StrCreditCashFld = _T("");
	m_StrCreditCashbackFld = _T("");
	m_StrCreditManualFld = _T("");
	m_StrCreditPreauthFld = _T("");
	m_StrCreditPurchaseFld = _T("");
	m_StrCreditReturnFld = _T("");
	m_StrDebitAuthFld = _T("");
	m_StrDebitCashFld = _T("");
	m_StrDebitManualFld = _T("");
	m_StrDebitPreauthFld = _T("");
	m_StrDebitPurchaseFld = _T("");
	m_StrDebitReturnFld = _T("");
	//}}AFX_DATA_INIT
}

CTerminalCapabilities::~CTerminalCapabilities()
{
}

void CTerminalCapabilities::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTerminalCapabilities)
	DDX_Control(pDX, IDC_DEBIT_RETURN_FLD, m_CntlDebitReturnFld);
	DDX_Control(pDX, IDC_DEBIT_PURCHASE_FLD, m_CntlDebitPurchaseFld);
	DDX_Control(pDX, IDC_DEBIT_PREAUTH_FLD, m_CntlDebitPreauthFld);
	DDX_Control(pDX, IDC_DEBIT_MANUAL_FLD, m_CntlDebitManualFld);
	DDX_Control(pDX, IDC_DEBIT_CASHBACK_FLD, m_CntlDebitCashbackFld);
	DDX_Control(pDX, IDC_DEBIT_CASH_FLD, m_CntlDebitCashFld);
	DDX_Control(pDX, IDC_DEBIT_AUTH_FLD, m_CntlDebitAuthFld);
	DDX_Control(pDX, IDC_CREDIT_RETURN_FLD, m_CntlCreditReturnFld);
	DDX_Control(pDX, IDC_CREDIT_PURCHASE_FLD, m_CntlCreditPurchaseFld);
	DDX_Control(pDX, IDC_CREDIT_PREAUTH_FLD, m_CntlCreditPreauthFld);
	DDX_Control(pDX, IDC_CREDIT_MANUAL_FLD, m_CntlCreditManualFld);
	DDX_Control(pDX, IDC_CREDIT_CASHBACK_FLD, m_CntlCreditCashbackFld);
	DDX_Control(pDX, IDC_CREDIT_CASH_FLD, m_CntlCreditCashFld);
	DDX_Control(pDX, IDC_CREDIT_AUTH_FLD, m_CntlCreditAuthFld);
	DDX_Text(pDX, IDC_CREDIT_AUTH_FLD, m_StrCreditAuthFld);
	DDX_Text(pDX, IDC_CREDIT_CASH_FLD, m_StrCreditCashFld);
	DDX_Text(pDX, IDC_CREDIT_CASHBACK_FLD, m_StrCreditCashbackFld);
	DDX_Text(pDX, IDC_CREDIT_MANUAL_FLD, m_StrCreditManualFld);
	DDX_Text(pDX, IDC_CREDIT_PREAUTH_FLD, m_StrCreditPreauthFld);
	DDX_Text(pDX, IDC_CREDIT_PURCHASE_FLD, m_StrCreditPurchaseFld);
	DDX_Text(pDX, IDC_CREDIT_RETURN_FLD, m_StrCreditReturnFld);
	DDX_Text(pDX, IDC_DEBIT_AUTH_FLD, m_StrDebitAuthFld);
	DDX_Text(pDX, IDC_DEBIT_CASH_FLD, m_StrDebitCashFld);
	DDX_Text(pDX, IDC_DEBIT_MANUAL_FLD, m_StrDebitManualFld);
	DDX_Text(pDX, IDC_DEBIT_PREAUTH_FLD, m_StrDebitPreauthFld);
	DDX_Text(pDX, IDC_DEBIT_PURCHASE_FLD, m_StrDebitPurchaseFld);
	DDX_Text(pDX, IDC_DEBIT_RETURN_FLD, m_StrDebitReturnFld);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTerminalCapabilities, CPropertyPage)
	//{{AFX_MSG_MAP(CTerminalCapabilities)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTerminalCapabilities message handlers

BOOL CTerminalCapabilities::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}
