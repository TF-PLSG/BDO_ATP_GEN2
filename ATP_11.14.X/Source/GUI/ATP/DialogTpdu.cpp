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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\DialogTpdu.cpp  $
   
      Rev 1.5   Dec 01 2003 11:29:20   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.4   Jan 05 2000 14:53:32   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:49:32   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:28   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:32   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:56:20   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:24   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:26   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:58:42   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:07:56   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:43:20   iarustam
    
*
************************************************************************************/
// DialogTpdu.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "DialogTpdu.h"
#include "SharedMem.h"	
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern CSharedMem shrmem;
/////////////////////////////////////////////////////////////////////////////
// CDialogTpdu dialog


CDialogTpdu::CDialogTpdu(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogTpdu::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogTpdu)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDialogTpdu::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogTpdu)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogTpdu, CDialog)
	//{{AFX_MSG_MAP(CDialogTpdu)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogTpdu message handlers

BOOL CDialogTpdu::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CDialog::OnCommand(wParam, lParam);
}
