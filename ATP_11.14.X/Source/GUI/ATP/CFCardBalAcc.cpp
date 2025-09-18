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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\CFCardBalAcc.cpp  $
   
      Rev 1.8   Dec 01 2003 11:29:04   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.7   Jan 25 2000 14:01:14   iarustam
   bug fixes
   
      Rev 1.6   Jan 05 2000 14:52:58   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:42:38   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:16:52   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:57:56   iarustam
   Initial Release
   
      Rev 1.3   Apr 29 1999 14:55:52   iarustam
   Bug fixes
   
      Rev 1.2   Mar 04 1999 13:31:42   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:00   iarustam
   Initial Release
   

    
*
************************************************************************************/
// CFCardBalAcc.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "CFCardBalAcc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#include "SharedMem.h"

extern CSharedMem shrmem;
/////////////////////////////////////////////////////////////////////////////
// CCFCardBalAcc property page

IMPLEMENT_DYNCREATE(CCFCardBalAcc, CPropertyPage)

CCFCardBalAcc::CCFCardBalAcc() : CPropertyPage(CCFCardBalAcc::IDD)
{
	//{{AFX_DATA_INIT(CCFCardBalAcc)
	m_strNbr1 = _T("");
	m_strNbr10 = _T("");
	m_strNbr2 = _T("");
	m_strNbr3 = _T("");
	m_strNbr4 = _T("");
	m_strNbr5 = _T("");
	m_strNbr6 = _T("");
	m_strNbr7 = _T("");
	m_strNbr8 = _T("");
	m_strNbr9 = _T("");
	m_strType10 = _T("");
	m_strType2 = _T("");
	m_strType3 = _T("");
	m_strType4 = _T("");
	m_strType5 = _T("");
	m_strType6 = _T("");
	m_strType7 = _T("");
	m_strType8 = _T("");
	m_strType9 = _T("");
	m_strCardType = _T("");
	m_strCardNbr = _T("");
	m_strComEdit = _T("");
	m_strType1 = _T("");
	//}}AFX_DATA_INIT
}

CCFCardBalAcc::~CCFCardBalAcc()
{
}

void CCFCardBalAcc::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCFCardBalAcc)
	DDX_Control(pDX, IDC_TYPE1, m_ctrlType1);
	DDX_Control(pDX, IDC_TYPE9, m_ctrlType9);
	DDX_Control(pDX, IDC_TYPE8, m_ctrlType8);
	DDX_Control(pDX, IDC_TYPE7, m_ctrlType7);
	DDX_Control(pDX, IDC_TYPE6, m_ctrlType6);
	DDX_Control(pDX, IDC_TYPE5, m_ctrlType5);
	DDX_Control(pDX, IDC_TYPE4, m_ctrlType4);
	DDX_Control(pDX, IDC_TYPE3, m_ctrlType3);
	DDX_Control(pDX, IDC_TYPE2, m_ctrlType2);
	DDX_Control(pDX, IDC_TYPE10, m_ctrlType10);
	DDX_Control(pDX, IDC_NBR9, m_ctrlNbr9);
	DDX_Control(pDX, IDC_NBR8, m_ctrlNbr8);
	DDX_Control(pDX, IDC_NBR7, m_ctrlNbr7);
	DDX_Control(pDX, IDC_NBR6, m_ctrlNbr6);
	DDX_Control(pDX, IDC_NBR5, m_ctrlNbr5);
	DDX_Control(pDX, IDC_NBR4, m_ctrlNbr4);
	DDX_Control(pDX, IDC_NBR3, m_ctrlNbr3);
	DDX_Control(pDX, IDC_NBR2, m_ctrlNbr2);
	DDX_Control(pDX, IDC_NBR10, m_ctrlNbr10);
	DDX_Control(pDX, IDC_NBR1, m_ctrlNbr1);
	DDX_Text(pDX, IDC_NBR1, m_strNbr1);
	DDV_MaxChars(pDX, m_strNbr1, 19);
	DDX_Text(pDX, IDC_NBR10, m_strNbr10);
	DDV_MaxChars(pDX, m_strNbr10, 19);
	DDX_Text(pDX, IDC_NBR2, m_strNbr2);
	DDV_MaxChars(pDX, m_strNbr2, 19);
	DDX_Text(pDX, IDC_NBR3, m_strNbr3);
	DDV_MaxChars(pDX, m_strNbr3, 19);
	DDX_Text(pDX, IDC_NBR4, m_strNbr4);
	DDV_MaxChars(pDX, m_strNbr4, 19);
	DDX_Text(pDX, IDC_NBR5, m_strNbr5);
	DDV_MaxChars(pDX, m_strNbr5, 19);
	DDX_Text(pDX, IDC_NBR6, m_strNbr6);
	DDV_MaxChars(pDX, m_strNbr6, 19);
	DDX_Text(pDX, IDC_NBR7, m_strNbr7);
	DDV_MaxChars(pDX, m_strNbr7, 19);
	DDX_Text(pDX, IDC_NBR8, m_strNbr8);
	DDV_MaxChars(pDX, m_strNbr8, 19);
	DDX_Text(pDX, IDC_NBR9, m_strNbr9);
	DDV_MaxChars(pDX, m_strNbr9, 19);
	DDX_CBString(pDX, IDC_TYPE10, m_strType10);
	DDX_CBString(pDX, IDC_TYPE2, m_strType2);
	DDX_CBString(pDX, IDC_TYPE3, m_strType3);
	DDX_CBString(pDX, IDC_TYPE4, m_strType4);
	DDX_CBString(pDX, IDC_TYPE5, m_strType5);
	DDX_CBString(pDX, IDC_TYPE6, m_strType6);
	DDX_CBString(pDX, IDC_TYPE7, m_strType7);
	DDX_CBString(pDX, IDC_TYPE8, m_strType8);
	DDX_CBString(pDX, IDC_TYPE9, m_strType9);
	DDX_Text(pDX, IDC_CARDTYPE, m_strCardType);
	DDX_Text(pDX, IDC_CARDNBR, m_strCardNbr);
	DDX_Text(pDX, IDC_COMMENTS, m_strComEdit);
	DDX_CBString(pDX, IDC_TYPE1, m_strType1);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCFCardBalAcc, CPropertyPage)
	//{{AFX_MSG_MAP(CCFCardBalAcc)
	ON_EN_CHANGE(IDC_NBR1, OnChangeNbr1)
	ON_EN_CHANGE(IDC_NBR10, OnChangeNbr10)
	ON_EN_CHANGE(IDC_NBR2, OnChangeNbr2)
	ON_EN_CHANGE(IDC_NBR3, OnChangeNbr3)
	ON_EN_CHANGE(IDC_NBR4, OnChangeNbr4)
	ON_EN_CHANGE(IDC_NBR5, OnChangeNbr5)
	ON_EN_CHANGE(IDC_NBR6, OnChangeNbr6)
	ON_EN_CHANGE(IDC_NBR7, OnChangeNbr7)
	ON_EN_CHANGE(IDC_NBR8, OnChangeNbr8)
	ON_EN_CHANGE(IDC_NBR9, OnChangeNbr9)
	ON_CBN_SELCHANGE(IDC_TYPE1, OnSelchangeType1)
	ON_CBN_SELCHANGE(IDC_TYPE10, OnSelchangeType10)
	ON_CBN_SELCHANGE(IDC_TYPE2, OnSelchangeType2)
	ON_CBN_SELCHANGE(IDC_TYPE3, OnSelchangeType3)
	ON_CBN_SELCHANGE(IDC_TYPE4, OnSelchangeType4)
	ON_CBN_SELCHANGE(IDC_TYPE5, OnSelchangeType5)
	ON_CBN_SELCHANGE(IDC_TYPE6, OnSelchangeType6)
	ON_CBN_SELCHANGE(IDC_TYPE7, OnSelchangeType7)
	ON_CBN_SELCHANGE(IDC_TYPE8, OnSelchangeType8)
	ON_CBN_SELCHANGE(IDC_TYPE9, OnSelchangeType9)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCFCardBalAcc message handlers

BOOL CCFCardBalAcc::OnInitDialog() 
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

BOOL CCFCardBalAcc::OnSetActive() 
{
	if (DialogMode == MODE_DONE || DialogMode == MODE_VIEW)
		m_strComEdit.Empty();
	UpdateData(FALSE);

	if (DialogMode == MODE_ADD || DialogMode == MODE_EDIT)
	{
		m_ctrlNbr1.EnableWindow(TRUE);
		m_ctrlNbr2.EnableWindow(TRUE);
		m_ctrlNbr3.EnableWindow(TRUE);
		m_ctrlNbr4.EnableWindow(TRUE);
		m_ctrlNbr5.EnableWindow(TRUE);
		m_ctrlNbr6.EnableWindow(TRUE);
		m_ctrlNbr7.EnableWindow(TRUE);
		m_ctrlNbr8.EnableWindow(TRUE);
		m_ctrlNbr9.EnableWindow(TRUE);
		m_ctrlNbr10.EnableWindow(TRUE);

		m_ctrlType1.EnableWindow(TRUE);
		m_ctrlType2.EnableWindow(TRUE);
		m_ctrlType3.EnableWindow(TRUE);
		m_ctrlType4.EnableWindow(TRUE);
		m_ctrlType5.EnableWindow(TRUE);
		m_ctrlType6.EnableWindow(TRUE);
		m_ctrlType7.EnableWindow(TRUE);
		m_ctrlType8.EnableWindow(TRUE);
		m_ctrlType9.EnableWindow(TRUE);
		m_ctrlType10.EnableWindow(TRUE);
	}
	else
	{
		m_ctrlNbr1.EnableWindow(FALSE);
		m_ctrlNbr2.EnableWindow(FALSE);
		m_ctrlNbr3.EnableWindow(FALSE);
		m_ctrlNbr4.EnableWindow(FALSE);
		m_ctrlNbr5.EnableWindow(FALSE);
		m_ctrlNbr6.EnableWindow(FALSE);
		m_ctrlNbr7.EnableWindow(FALSE);
		m_ctrlNbr8.EnableWindow(FALSE);
		m_ctrlNbr9.EnableWindow(FALSE);
		m_ctrlNbr10.EnableWindow(FALSE);

		m_ctrlType1.EnableWindow(FALSE);
		m_ctrlType2.EnableWindow(FALSE);
		m_ctrlType3.EnableWindow(FALSE);
		m_ctrlType4.EnableWindow(FALSE);
		m_ctrlType5.EnableWindow(FALSE);
		m_ctrlType6.EnableWindow(FALSE);
		m_ctrlType7.EnableWindow(FALSE);
		m_ctrlType8.EnableWindow(FALSE);
		m_ctrlType9.EnableWindow(FALSE);
		m_ctrlType10.EnableWindow(FALSE);
	}


	
	return CPropertyPage::OnSetActive();
}


void CCFCardBalAcc::OnChangeNbr1() 
{
	SetModified(TRUE);
	
}

void CCFCardBalAcc::OnChangeNbr10() 
{
	SetModified(TRUE);
	
}

void CCFCardBalAcc::OnChangeNbr2() 
{
	SetModified(TRUE);
	
}

void CCFCardBalAcc::OnChangeNbr3() 
{
	SetModified(TRUE);
	
}

void CCFCardBalAcc::OnChangeNbr4() 
{
	SetModified(TRUE);
	
}

void CCFCardBalAcc::OnChangeNbr5() 
{
	SetModified(TRUE);
	
}

void CCFCardBalAcc::OnChangeNbr6() 
{
	SetModified(TRUE);
	
}

void CCFCardBalAcc::OnChangeNbr7() 
{
	SetModified(TRUE);
	
}

void CCFCardBalAcc::OnChangeNbr8() 
{
	SetModified(TRUE);
	
}

void CCFCardBalAcc::OnChangeNbr9() 
{
	SetModified(TRUE);
	
}

void CCFCardBalAcc::OnSelchangeType1() 
{
	SetModified(TRUE);
	
}

void CCFCardBalAcc::OnSelchangeType10() 
{
	SetModified(TRUE);
	
}

void CCFCardBalAcc::OnSelchangeType2() 
{
	SetModified(TRUE);
	
}

void CCFCardBalAcc::OnSelchangeType3() 
{
	SetModified(TRUE);
	
}

void CCFCardBalAcc::OnSelchangeType4() 
{
	SetModified(TRUE);
	
}

void CCFCardBalAcc::OnSelchangeType5() 
{
	SetModified(TRUE);
	
}

void CCFCardBalAcc::OnSelchangeType6() 
{
	SetModified(TRUE);
	
}

void CCFCardBalAcc::OnSelchangeType7() 
{
	SetModified(TRUE);
	
}

void CCFCardBalAcc::OnSelchangeType8() 
{
	SetModified(TRUE);
	
}

void CCFCardBalAcc::OnSelchangeType9() 
{
	SetModified(TRUE);
	
}

BOOL CCFCardBalAcc::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}
