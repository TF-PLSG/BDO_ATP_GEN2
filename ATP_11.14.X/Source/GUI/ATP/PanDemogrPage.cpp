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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\PanDemogrPage.cpp  $
   
	 Rev        April 28.4.11          Krishna Kiran S@TF
   Added Idle Time Calculation & calling Login screen on Idle Timeout

      Rev 1.6   Dec 01 2003 11:30:00   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.5   Jan 05 2000 14:54:10   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:57:24   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:04   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:12   iarustam
   Initial Release
   
      Rev 1.2   Apr 29 1999 14:56:52   iarustam
   Bug fixes
   
      Rev 1.1   Apr 01 1999 14:09:18   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:41:00   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:52   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:59:50   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:09:28   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:44:28   iarustam
    
*
************************************************************************************/
// PanDemogrPage.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "PanDemogrPage.h"
#include "Login.h"
#include "SharedMem.h"	
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CSharedMem shrmem;

/////////////////////////////////////////////////////////////////////////////
// CPanDemogrPage property page

IMPLEMENT_DYNCREATE(CPanDemogrPage, CPropertyPage)

CPanDemogrPage::CPanDemogrPage() : CPropertyPage(CPanDemogrPage::IDD)
{
	//{{AFX_DATA_INIT(CPanDemogrPage)
	m_panstr = _T("");
	m_strAddr1 = _T("");
	m_strAddr2 = _T("");
	m_strCity = _T("");
	m_strHomePhone = _T("");
	m_strName = _T("");
	m_strState = _T("");
	m_strWorkPhone = _T("");
	m_strZip = _T("");
	//}}AFX_DATA_INIT
}

CPanDemogrPage::~CPanDemogrPage()
{
}

void CPanDemogrPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPanDemogrPage)
	DDX_Control(pDX, IDC_PAN, m_panedit);
	DDX_Control(pDX, IDC_ZIP, m_Zip);
	DDX_Control(pDX, IDC_WORKPHONE, m_WorkPhone);
	DDX_Control(pDX, IDC_STATE, m_State);
	DDX_Control(pDX, IDC_NAME, m_Name);
	DDX_Control(pDX, IDC_HOMEPHONE, m_HomePnone);
	DDX_Control(pDX, IDC_COMMENTS, m_ComEdit);
	DDX_Control(pDX, IDC_CITY, m_City);
	DDX_Control(pDX, IDC_ADDR2, m_Addr2);
	DDX_Control(pDX, IDC_ADDR1, m_Addr1);
	DDX_Text(pDX, IDC_PAN, m_panstr);
	DDV_MaxChars(pDX, m_panstr, 19);
	DDX_Text(pDX, IDC_ADDR1, m_strAddr1);
	DDV_MaxChars(pDX, m_strAddr1, 30);
	DDX_Text(pDX, IDC_ADDR2, m_strAddr2);
	DDV_MaxChars(pDX, m_strAddr2, 30);
	DDX_Text(pDX, IDC_CITY, m_strCity);
	DDV_MaxChars(pDX, m_strCity, 20);
	DDX_Text(pDX, IDC_HOMEPHONE, m_strHomePhone);
	DDV_MaxChars(pDX, m_strHomePhone, 30);
	DDX_Text(pDX, IDC_NAME, m_strName);
	DDV_MaxChars(pDX, m_strName, 150);
	DDX_Text(pDX, IDC_STATE, m_strState);
	DDV_MaxChars(pDX, m_strState, 3);
	DDX_Text(pDX, IDC_WORKPHONE, m_strWorkPhone);
	DDV_MaxChars(pDX, m_strWorkPhone, 30);
	DDX_Text(pDX, IDC_ZIP, m_strZip);
	DDV_MaxChars(pDX, m_strZip, 10);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPanDemogrPage, CPropertyPage)
	//{{AFX_MSG_MAP(CPanDemogrPage)
	ON_EN_CHANGE(IDC_ZIP, OnChangeZip)
	ON_EN_CHANGE(IDC_ADDR1, OnChangeAddr1)
	ON_EN_CHANGE(IDC_ADDR2, OnChangeAddr2)
	ON_EN_CHANGE(IDC_CITY, OnChangeCity)
	ON_EN_CHANGE(IDC_HOMEPHONE, OnChangeHomephone)
	ON_EN_CHANGE(IDC_NAME, OnChangeName)
	ON_EN_CHANGE(IDC_STATE, OnChangeState)
	ON_EN_CHANGE(IDC_WORKPHONE, OnChangeWorkphone)
	//}}AFX_MSG_MAP


END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPanDemogrPage message handlers

BOOL CPanDemogrPage::OnSetActive() 
{
	UpdateData(FALSE);
	m_ComEdit.SetWindowText("");

	if (DialogMode == MODE_VIEW || DialogMode == MODE_DONE)
	{
		m_Zip.EnableWindow(FALSE);
		m_WorkPhone.EnableWindow(FALSE);
		m_State.EnableWindow(FALSE);
		m_Name.EnableWindow(FALSE);
		m_HomePnone.EnableWindow(FALSE);
		m_City.EnableWindow(FALSE);
		m_Addr2.EnableWindow(FALSE);
		m_Addr1.EnableWindow(FALSE);
	}
	else
	{
		m_Zip.EnableWindow(TRUE);
		m_WorkPhone.EnableWindow(TRUE);
		m_State.EnableWindow(TRUE);
		m_Name.EnableWindow(TRUE);
		m_HomePnone.EnableWindow(TRUE);
		m_City.EnableWindow(TRUE);
		m_Addr2.EnableWindow(TRUE);
		m_Addr1.EnableWindow(TRUE);
	}

	if (DialogMode == MODE_DONE)
	{
		m_panedit.SetWindowText("");
		m_Zip.SetWindowText("");
		m_WorkPhone.SetWindowText("");
		m_HomePnone.SetWindowText("");
		m_State.SetWindowText("");
		m_Name.SetWindowText("");
		m_City.SetWindowText("");
		m_Addr2.SetWindowText("");
		m_Addr1.SetWindowText("");
	}

	return CPropertyPage::OnSetActive();
}

BOOL CPanDemogrPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	CFont m_font;
	LOGFONT lf;
	
	memset ( &lf, 0, sizeof (lf));
	lf.lfHeight = 12;
	lf.lfWeight = 700;
	strcpy(lf.lfFaceName, "Arial");
	m_font.CreateFontIndirect (&lf);
	GetDlgItem(IDC_COMMENTS)->SetFont (&m_font);

	//UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CPanDemogrPage::OnChangeZip() 
{
	if ( DialogMode != MODE_DONE && DialogMode != MODE_VIEW )
		SetModified(TRUE);
	
}


void CPanDemogrPage::OnChangeAddr1() 
{
	if ( DialogMode != MODE_DONE && DialogMode != MODE_VIEW )
	SetModified(TRUE);
	
}

void CPanDemogrPage::OnChangeAddr2() 
{
	if ( DialogMode != MODE_DONE && DialogMode != MODE_VIEW )
	SetModified(TRUE);
	
}

void CPanDemogrPage::OnChangeCity() 
{
	if ( DialogMode != MODE_DONE && DialogMode != MODE_VIEW )
	SetModified(TRUE);
	
}

void CPanDemogrPage::OnChangeHomephone() 
{
	if ( DialogMode != MODE_DONE && DialogMode != MODE_VIEW )
	SetModified(TRUE);
	
}

void CPanDemogrPage::OnChangeName() 
{
	if ( DialogMode != MODE_DONE && DialogMode != MODE_VIEW )
	SetModified(TRUE);
	
}

void CPanDemogrPage::OnChangeState() 
{
	if ( DialogMode != MODE_DONE && DialogMode != MODE_VIEW )
	SetModified(TRUE);
	
}

void CPanDemogrPage::OnChangeWorkphone() 
{
	if ( DialogMode != MODE_DONE && DialogMode != MODE_VIEW )
	SetModified(TRUE);
	
}



BOOL CPanDemogrPage::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}
