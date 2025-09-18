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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\MerchantTerminalPoints.cpp  $
   
      Rev 1.6   Dec 01 2003 11:29:44   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.5   Mar 04 2003 16:28:50   lmitchel
   Validate length of TPK key and for hex characters; allow entry of blank key
   
      Rev 1.4   Jan 05 2000 14:54:04   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:54:46   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:52   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:58   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:56:42   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:48   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:42   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:59:26   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:08:54   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:44:04   iarustam
    
*
************************************************************************************/
// MerchantTerminalPoints.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "MerchantTerminalPoints.h"
#include "SharedMem.h"	
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
extern CSharedMem shrmem;
/////////////////////////////////////////////////////////////////////////////
// CMerchantTerminalPoints property page

IMPLEMENT_DYNCREATE(CMerchantTerminalPoints, CPropertyPage)

CMerchantTerminalPoints::CMerchantTerminalPoints() : CPropertyPage(CMerchantTerminalPoints::IDD)
{
	//{{AFX_DATA_INIT(CMerchantTerminalPoints)
	m_StrChainIDFld = _T("");
	m_StrMerchantIDFld = _T("");
	m_StrPointsPurseFld = _T("");
	m_StrSpecialPointsEndFld = _T("");
	m_StrSpecialPointsPurseFld = _T("");
	m_StrSpecialPointsStartFld = _T("");
	m_StrTerminalIDFld = _T("");
	//}}AFX_DATA_INIT
}

CMerchantTerminalPoints::~CMerchantTerminalPoints()
{
}

void CMerchantTerminalPoints::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMerchantTerminalPoints)
	DDX_Control(pDX, IDC_SPECIAL_POINTS_START_FLD, m_CntlSpecialPointsStartFld);
	DDX_Control(pDX, IDC_SPECIAL_POINTS_PURSE_FLD, m_CntlSpecialPointsPurseFld);
	DDX_Control(pDX, IDC_SPECIAL_POINTS_END_FLD, m_CntlSpecialPointsEndFld);
	DDX_Control(pDX, IDC_POINTS_PURSE_FLD, m_CntlPointsPurseFld);
	DDX_Text(pDX, IDC_CHAIN_ID_FLD, m_StrChainIDFld);
	DDX_Text(pDX, IDC_MERCHANT_ID_FLD, m_StrMerchantIDFld);
	DDX_Text(pDX, IDC_POINTS_PURSE_FLD, m_StrPointsPurseFld);
	DDX_Text(pDX, IDC_SPECIAL_POINTS_END_FLD, m_StrSpecialPointsEndFld);
	DDX_Text(pDX, IDC_SPECIAL_POINTS_PURSE_FLD, m_StrSpecialPointsPurseFld);
	DDX_Text(pDX, IDC_SPECIAL_POINTS_START_FLD, m_StrSpecialPointsStartFld);
	DDX_Text(pDX, IDC_TERMINAL_ID_FLD, m_StrTerminalIDFld);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMerchantTerminalPoints, CPropertyPage)
	//{{AFX_MSG_MAP(CMerchantTerminalPoints)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMerchantTerminalPoints message handlers

BOOL CMerchantTerminalPoints::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}
