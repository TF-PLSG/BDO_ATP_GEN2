/*******************************************************************************
*  
* MODULE:      PrimaryAccountList.cpp
*  
* TITLE:       
*  
* DESCRIPTION:
*
* APPLICATION: Ascendent System Monitor
*
* AUTHOR:  Ram Malathkar
*
* REVISION HISTORY:
*
* $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\monitor\files\PrimaryAccountList.cpp  $
   
      Rev 1.3   Apr 04 2005 15:58:50   lmitchel
   Rebuild 4_4_1
   
      Rev 1.2   Aug 29 2000 15:24:36   lmitchel
   New implementation of monitor including new functions for displaying device and network controller statistics and dialog status messages.  Two windows added to the existing monitor to display device and network transaction counts (sent, received, tps rate) and dialog status (connected, disconnected).  The new windows are updated with current data every nn seconds defined by the interval in the monitor.ini file.  Functions added to send and recieved stats and status updates form the controllers.
   
      Rev 1.1   Nov 24 1999 14:32:02   lmitchel
   Recompile w/new libraries ver 3_2_0/core1_3_1_3
   
      Rev 1.0   24 Feb 1999 15:59:08   rmalathk
   Initial Revision
   
      Rev 1.0   Dec 04 1998 15:33:34   MSALEH
   initial release
   
      Rev 1.0   Dec 04 1998 15:00:04   MSALEH
   initial release
   
      Rev 1.0   28 Oct 1998 16:31:00   jhapitas
   Initial checkin
*
*******************************************************************************/
// PrimaryAccountList.cpp : implementation file
//

#include "stdafx.h"
#include "monitor.h"
#include "PrimaryAccountList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPrimaryAccountList dialog


CPrimaryAccountList::CPrimaryAccountList(CWnd* pParent /*=NULL*/)
	: CDialog(CPrimaryAccountList::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPrimaryAccountList)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CPrimaryAccountList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPrimaryAccountList)
	DDX_Control(pDX, IDC_PRIMARYACCOUNT_LIST, m_PrimaryAccountList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPrimaryAccountList, CDialog)
	//{{AFX_MSG_MAP(CPrimaryAccountList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrimaryAccountList message handlers

void CPrimaryAccountList::OnOK() 
{
	// TODO: Add extra validation here
   INT ret = m_PrimaryAccountList.GetCurSel();
   if( ret == LB_ERR )
   {
      AfxMessageBox( "Please Select a Primary Account Number", MB_ICONSTOP | MB_OK );
      return;
   }


   m_PrimaryAccountList.GetText( ret, m_pan );	
	CDialog::OnOK();
}

BOOL CPrimaryAccountList::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

   m_PrimaryAccountList.AddString( "(No Bin)" );
   for( INT temp = 0; temp < 10; temp++ )
   {
      if( strlen( (char *)m_logon_bin [temp].identifier ) != 0 )
         m_PrimaryAccountList.AddString( (char *)m_logon_bin [temp].identifier );

   }
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
