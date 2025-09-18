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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\SettlePrefixTime.cpp  $
   
      Rev 1.5   Dec 01 2003 11:30:14   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.4   Jan 05 2000 14:54:24   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 16:00:22   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:20   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:28   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:57:06   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:41:20   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:10:08   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 11:00:14   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:09:56   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:44:50   iarustam
    
*
************************************************************************************/
// SettlePrefixTime.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "SettlePrefixTime.h"
#include "SharedMem.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
extern CSharedMem shrmem;
/////////////////////////////////////////////////////////////////////////////
// CSettlePrefixTime property page

IMPLEMENT_DYNCREATE(CSettlePrefixTime, CPropertyPage)

CSettlePrefixTime::CSettlePrefixTime() : CPropertyPage(CSettlePrefixTime::IDD)
{
	//{{AFX_DATA_INIT(CSettlePrefixTime)
	m_StrSettleFileFld = _T("");
	m_StrSettleTimeFld = _T("");
	m_StrMessageLbl = _T("");
	//}}AFX_DATA_INIT
}

CSettlePrefixTime::~CSettlePrefixTime()
{
}

void CSettlePrefixTime::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSettlePrefixTime)
	DDX_Control(pDX, IDC_SETTLE_TIME_FLD, m_CntlSettleTimeFld);
	DDX_Control(pDX, IDC_SETTLE_FILE_FLD, m_CntlSettleFileFld);
	DDX_Text(pDX, IDC_SETTLE_FILE_FLD, m_StrSettleFileFld);
	DDX_Text(pDX, IDC_SETTLE_TIME_FLD, m_StrSettleTimeFld);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSettlePrefixTime, CPropertyPage)
	//{{AFX_MSG_MAP(CSettlePrefixTime)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSettlePrefixTime message handlers



BOOL CSettlePrefixTime::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}
