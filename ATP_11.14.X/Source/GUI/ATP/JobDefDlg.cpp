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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\JobDefDlg.cpp  $
   
	 Rev        April 19.5.11          Krishna Kiran S@TF
	 Added Reseting Idle time, whenever any event is fired in GUI

      Rev 1.5   Dec 01 2003 11:29:26   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.4   Jan 05 2000 14:53:38   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:50:36   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:32   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:36   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:56:24   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:30   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:30   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:58:54   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:08:10   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:43:26   iarustam
    
*
************************************************************************************/
// JobDefDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "JobDefDlg.h"
#include "SharedMem.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
extern CSharedMem shrmem;
/////////////////////////////////////////////////////////////////////////////
// CJobDefDlg dialog


CJobDefDlg::CJobDefDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CJobDefDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CJobDefDlg)
	m_ProfileName = _T("");
	//}}AFX_DATA_INIT
}


void CJobDefDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CJobDefDlg)
	DDX_Text(pDX, IDC_PROFILENAME, m_ProfileName);
	DDV_MaxChars(pDX, m_ProfileName, 15);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CJobDefDlg, CDialog)
	//{{AFX_MSG_MAP(CJobDefDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CJobDefDlg message handlers

BOOL CJobDefDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CDialog::OnCommand(wParam, lParam);
}
