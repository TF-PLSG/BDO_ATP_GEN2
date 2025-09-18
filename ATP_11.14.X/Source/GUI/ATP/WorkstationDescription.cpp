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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\WorkstationDescription.cpp  $
   
      Rev 1.5   Dec 01 2003 11:30:26   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.4   Jan 05 2000 14:54:34   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 16:03:08   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:32   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:42   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:57:16   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:41:30   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:10:18   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 11:00:34   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:10:18   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:45:12   iarustam
    
*
************************************************************************************/
// WorkstationDescription.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "WorkstationDescription.h"
#include "SharedMem.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
extern CSharedMem shrmem;
/////////////////////////////////////////////////////////////////////////////
// CWorkstationDescription property page

IMPLEMENT_DYNCREATE(CWorkstationDescription, CPropertyPage)

CWorkstationDescription::CWorkstationDescription() : CPropertyPage(CWorkstationDescription::IDD)
{
	//{{AFX_DATA_INIT(CWorkstationDescription)
	m_StrMessageLbl = _T("");
	m_WorkstationIdStr = _T("");
	m_StrWorkstationStatus = _T("");
	//}}AFX_DATA_INIT
}

CWorkstationDescription::~CWorkstationDescription()
{
}

void CWorkstationDescription::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWorkstationDescription)
	DDX_Control(pDX, IDC_WORKSTATION_ID, m_WorkstationIdCtrl);
	DDX_Control(pDX, IDC_WORKSTATION_STATUS, m_WorkstationStatus);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	DDX_Text(pDX, IDC_WORKSTATION_ID, m_WorkstationIdStr);
	DDV_MaxChars(pDX, m_WorkstationIdStr, 20);
	DDX_CBString(pDX, IDC_WORKSTATION_STATUS, m_StrWorkstationStatus);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWorkstationDescription, CPropertyPage)
	//{{AFX_MSG_MAP(CWorkstationDescription)
	ON_CBN_SELCHANGE(IDC_WORKSTATION_STATUS, OnSelchangeWorkstationStatus)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorkstationDescription message handlers
BOOL CWorkstationDescription::PopulateDialogFromDatabase()
{
	if ( m_nDialogMode == MODE_EDIT || m_nDialogMode == MODE_VIEW )
	{
		EnableWorkstationDescriptionDialog(TRUE );
    }

        	
	if (!UpdateData(FALSE))
	{
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}

	return TRUE;
}


void CWorkstationDescription::EnableWorkstationDescriptionDialog(int nStatus)
{
   m_WorkstationStatus.EnableWindow(nStatus);

}


void CWorkstationDescription::CleanWorkstationDescriptionDialog()
{
    m_WorkstationIdStr = "";
	 m_StrMessageLbl = "";
    UpdateData(FALSE);
}



BOOL CWorkstationDescription::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
		CFont m_font;
	LOGFONT lf;

	memset ( &lf, 0, sizeof (lf));
	lf.lfHeight = 12;
	lf.lfWeight = 700;
	strcpy(lf.lfFaceName, "Arial");
	m_font.CreateFontIndirect (&lf);
	GetDlgItem(IDC_MESSAGE_LBL)->SetFont (&m_font);
	
	SetModified ( FALSE );

   m_WorkstationStatus.AddString( "Active" );
   m_WorkstationStatus.AddString( "Inactive" );

   if( m_nDialogMode == MODE_DONE )
      m_WorkstationStatus.EnableWindow( FALSE );

	m_bDescription = TRUE;
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


BOOL CWorkstationDescription::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}

void CWorkstationDescription::OnSelchangeWorkstationStatus() 
{
	// TODO: Add your control notification handler code here
   if( m_WorkstationStatus.GetCurSel() == 0 )
      strcpy( (char *)atp02.workstation_status, "A" );
   else
      strcpy( (char *)atp02.workstation_status, "I" );

   SetModified( TRUE );
	
}
