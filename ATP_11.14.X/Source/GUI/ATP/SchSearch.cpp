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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\SchSearch.cpp  $
   
      Rev 1.6   Dec 01 2003 11:30:12   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.5   Jan 05 2000 14:54:20   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:59:44   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:16   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:24   iarustam
   Initial Release
   
      Rev 1.2   May 10 1999 11:38:22   dperson
   Standardized error messages
   
      Rev 1.1   Apr 29 1999 14:57:02   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:41:16   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:10:04   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 11:00:08   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:09:44   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:44:42   iarustam
    
*
************************************************************************************/
// SchSearch.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "SchSearch.h"

#include "SharedMem.h"	
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSchSearch dialog
extern CSharedMem shrmem;

CSchSearch::CSchSearch(CWnd* pParent /*=NULL*/)
	: CDialog(CSchSearch::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSchSearch)
	m_SearchItem = _T("");
	m_SearchLbl = _T("");
	//}}AFX_DATA_INIT
}


void CSchSearch::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSchSearch)
	DDX_Control(pDX, IDC_SEARCH_ITEM, m_SearchCtrl);
	DDX_Text(pDX, IDC_SEARCH_ITEM, m_SearchItem);
	DDV_MaxChars(pDX, m_SearchItem, 15);
	DDX_Text(pDX, IDC_SEARCH_LBL, m_SearchLbl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSchSearch, CDialog)
	//{{AFX_MSG_MAP(CSchSearch)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSchSearch message handlers

BOOL CSchSearch::OnInitDialog() 
{
	CDialog::OnInitDialog();
   m_SearchCtrl.SetFocus();
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSchSearch::OnOK() 
{
	// TODO: Add extra validation here
   UpdateData( TRUE );
   if( m_SearchItem.IsEmpty() )
   {
      AfxMessageBox( "Please enter Search Key", MB_ICONSTOP | MB_OK );
      return;
   }
	
	CDialog::OnOK();
}

BOOL CSchSearch::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CDialog::OnCommand(wParam, lParam);
}
