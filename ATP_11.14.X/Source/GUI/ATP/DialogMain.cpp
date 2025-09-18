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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\DialogMain.cpp  $
   
      Rev 1.5   Dec 01 2003 11:29:18   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.4   Jan 05 2000 14:53:30   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:48:58   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:22   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:28   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:56:18   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:22   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:24   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:58:36   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:07:50   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:43:16   iarustam
    
*
************************************************************************************/
// DialogMain.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "DialogMain.h"
#include "SharedMem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CSharedMem shrmem;

/////////////////////////////////////////////////////////////////////////////
// CDialogMain property page

IMPLEMENT_DYNCREATE(CDialogMain, CPropertyPage)

CDialogMain::CDialogMain() : CPropertyPage(CDialogMain::IDD)
{
	//{{AFX_DATA_INIT(CDialogMain)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CDialogMain::~CDialogMain()
{
}

void CDialogMain::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogMain)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogMain, CPropertyPage)
	//{{AFX_MSG_MAP(CDialogMain)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogMain message handlers

BOOL CDialogMain::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}
