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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\CFActivity.cpp  $
   
      Rev 1.8   Dec 01 2003 11:29:02   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.7   Jan 05 2000 14:52:56   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:42:10   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:16:50   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:57:56   iarustam
   Initial Release
   
      Rev 1.4   Jun 04 1999 17:41:26   iarustam
   Bug fixes
   
      Rev 1.3   Apr 29 1999 14:55:50   iarustam
   Bug fixes
   
      Rev 1.2   Mar 04 1999 13:31:40   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:39:58   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:00   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:57:36   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:06:26   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:42:18   iarustam
    
*
************************************************************************************/
// CFActivity.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "CFActivity.h"
#include "SharedMem.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
extern CSharedMem shrmem;
/////////////////////////////////////////////////////////////////////////////
// CCFActivity property page

IMPLEMENT_DYNCREATE(CCFActivity, CPropertyPage)

CCFActivity::CCFActivity() : CPropertyPage(CCFActivity::IDD)
{
	//{{AFX_DATA_INIT(CCFActivity)
	m_strCardType = _T("");
	m_strDate = _T("");
	m_strP1Amt = _T("");
	m_strP1Nbr = _T("");
	m_strP1Lbl = _T("");
	m_strP2Amt = _T("");
	m_strP2Nbr = _T("");
	m_strP2Lbl = _T("");
	m_strP3Amt = _T("");
	m_strP3Nbr = _T("");
	m_strP3Lbl = _T("");
	m_strP1AmtDol = _T("");
	m_strP1NbrDol = _T("");
	m_strP2AmtDol = _T("");
	m_strP2NbrDol = _T("");
	m_strP3AmtDol = _T("");
	m_strP3NbrDol = _T("");
	m_strCardNbr = _T("");
	m_strDate2 = _T("");
	//}}AFX_DATA_INIT
}

CCFActivity::~CCFActivity()
{
}

void CCFActivity::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCFActivity)
	DDX_Control(pDX, IDC_COMMENTS, m_ComEdit);
	DDX_Text(pDX, IDC_CARDTYPE, m_strCardType);
	DDX_Text(pDX, IDC_DATE, m_strDate);
	DDX_Text(pDX, IDC_P1_AMT, m_strP1Amt);
	DDX_Text(pDX, IDC_P1_NBR, m_strP1Nbr);
	DDX_Text(pDX, IDC_P1LABEL, m_strP1Lbl);
	DDX_Text(pDX, IDC_P2_AMT, m_strP2Amt);
	DDX_Text(pDX, IDC_P2_NBR, m_strP2Nbr);
	DDX_Text(pDX, IDC_P2LABEL, m_strP2Lbl);
	DDX_Text(pDX, IDC_P3_AMT, m_strP3Amt);
	DDX_Text(pDX, IDC_P3_NBR, m_strP3Nbr);
	DDX_Text(pDX, IDC_P3LABEL, m_strP3Lbl);
	DDX_Text(pDX, IDC_P1_AMT2, m_strP1AmtDol);
	DDX_Text(pDX, IDC_P1_NBR2, m_strP1NbrDol);
	DDX_Text(pDX, IDC_P2_AMT2, m_strP2AmtDol);
	DDX_Text(pDX, IDC_P2_NBR2, m_strP2NbrDol);
	DDX_Text(pDX, IDC_P3_AMT2, m_strP3AmtDol);
	DDX_Text(pDX, IDC_P3_NBR2, m_strP3NbrDol);
	DDX_Text(pDX, IDC_CARDNBR, m_strCardNbr);
	DDX_Text(pDX, IDC_DATE2, m_strDate2);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCFActivity, CPropertyPage)
	//{{AFX_MSG_MAP(CCFActivity)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCFActivity message handlers

BOOL CCFActivity::OnSetActive() 
{
	m_ComEdit.SetWindowText("");
	UpdateData(FALSE);

	return CPropertyPage::OnSetActive();
}

BOOL CCFActivity::OnInitDialog() 
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

	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CCFActivity::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}
