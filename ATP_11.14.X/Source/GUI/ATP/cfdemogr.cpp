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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\cfdemogr.cpp  $
   
      Rev 1.6   Dec 01 2003 11:29:04   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.5   Jan 25 2000 14:01:06   iarustam
   bug fixes
   
      Rev 1.4   Jan 14 2000 16:25:04   mbysani
   bug fixes
   
      Rev 1.1   Sep 22 1999 16:03:58   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:38   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:50   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:57:18   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:41:34   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:02   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:57:40   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:06:34   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:48:16   iarustam
    
*
************************************************************************************/
// CFDemogr.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "CFDemogr.h"
#include "SharedMem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CSharedMem shrmem;

/////////////////////////////////////////////////////////////////////////////
// CCFDemogr property page

IMPLEMENT_DYNCREATE(CCFDemogr, CPropertyPage)

CCFDemogr::CCFDemogr() : CPropertyPage(CCFDemogr::IDD)
{
	//{{AFX_DATA_INIT(CCFDemogr)
	m_strAddr1 = _T("");
	m_strAddr2 = _T("");
	m_strCardNbr = _T("");
	m_strCardType = _T("");
	m_strCity = _T("");
	m_strCountry = _T("");
	m_strHomePhone = _T("");
	m_strName = _T("");
	m_strState = _T("");
	m_strWorkPhone = _T("");
	m_strZip = _T("");
	m_strProfession = _T("");
	m_strDOB = _T("");
	m_strComEdit = _T("");
	m_StrBlockStatusFld = _T("");
	m_StrCardholderIDFld = _T("");
	m_StrMaidenNameFld = _T("");
	m_StrMembershipIDFld = _T("");
	m_StrSpecialtyCodeFld = _T("");
	m_StrVIPAccountCodeFld = _T("");
	//}}AFX_DATA_INIT
}

CCFDemogr::~CCFDemogr()
{
}

void CCFDemogr::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCFDemogr)
	DDX_Control(pDX, IDC_SPECIALTY_CODE_FLD, m_CntlSpecialtyCodeFld);
	DDX_Control(pDX, IDC_VIP_ACCOUNT_CODE_FLD, m_CntlVIPAccountCodeFld);
	DDX_Control(pDX, IDC_MEMBERSHIP_ID_FLD, m_CntlMembershipIDFld);
	DDX_Control(pDX, IDC_MAIDEN_NAME_FLD, m_CntlMaidenNameFld);
	DDX_Control(pDX, IDC_CARDHOLDER_ID_FLD, m_CntlCardholderIDFld);
	DDX_Control(pDX, IDC_BL_STATUS_FLD, m_CntlBlockStatusFld);
	DDX_Control(pDX, IDC_CARDNBR, m_CardNbr);
	DDX_Control(pDX, IDC_CARDTYPE, m_CardType);
	DDX_Control(pDX, IDC_ZIP, m_Zip);
	DDX_Control(pDX, IDC_WORKPHONE, m_WorkPhone);
	DDX_Control(pDX, IDC_STATE, m_State);
	DDX_Control(pDX, IDC_PROFESSION, m_Profession);
	DDX_Control(pDX, IDC_NAME, m_Name);
	DDX_Control(pDX, IDC_HOMEPHONE, m_HomePhone);
	DDX_Control(pDX, IDC_DOB, m_DOB);
	DDX_Control(pDX, IDC_COUNTRY, m_Country);
	DDX_Control(pDX, IDC_CITY, m_City);
	DDX_Control(pDX, IDC_ADDR2, m_Addr2);
	DDX_Control(pDX, IDC_ADDR1, m_Addr1);
	DDX_Text(pDX, IDC_ADDR1, m_strAddr1);
	DDV_MaxChars(pDX, m_strAddr1, 30);
	DDX_Text(pDX, IDC_ADDR2, m_strAddr2);
	DDV_MaxChars(pDX, m_strAddr2, 30);
	DDX_Text(pDX, IDC_CARDNBR, m_strCardNbr);
	DDX_Text(pDX, IDC_CARDTYPE, m_strCardType);
	DDX_Text(pDX, IDC_CITY, m_strCity);
	DDV_MaxChars(pDX, m_strCity, 20);
	DDX_Text(pDX, IDC_COUNTRY, m_strCountry);
	DDV_MaxChars(pDX, m_strCountry, 20);
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
	DDX_Text(pDX, IDC_PROFESSION, m_strProfession);
	DDV_MaxChars(pDX, m_strProfession, 20);
	DDX_Text(pDX, IDC_DOB, m_strDOB);
	DDV_MaxChars(pDX, m_strDOB, 8);
	DDX_Text(pDX, IDC_COMMENTS, m_strComEdit);
	DDX_Text(pDX, IDC_BL_STATUS_FLD, m_StrBlockStatusFld);
	DDV_MaxChars(pDX, m_StrBlockStatusFld, 4);
	DDX_Text(pDX, IDC_CARDHOLDER_ID_FLD, m_StrCardholderIDFld);
	DDV_MaxChars(pDX, m_StrCardholderIDFld, 19);
	DDX_Text(pDX, IDC_MAIDEN_NAME_FLD, m_StrMaidenNameFld);
	DDV_MaxChars(pDX, m_StrMaidenNameFld, 30);
	DDX_Text(pDX, IDC_MEMBERSHIP_ID_FLD, m_StrMembershipIDFld);
	DDV_MaxChars(pDX, m_StrMembershipIDFld, 9);
	DDX_Text(pDX, IDC_SPECIALTY_CODE_FLD, m_StrSpecialtyCodeFld);
	DDV_MaxChars(pDX, m_StrSpecialtyCodeFld, 2);
	DDX_Text(pDX, IDC_VIP_ACCOUNT_CODE_FLD, m_StrVIPAccountCodeFld);
	DDV_MaxChars(pDX, m_StrVIPAccountCodeFld, 1);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCFDemogr, CPropertyPage)
	//{{AFX_MSG_MAP(CCFDemogr)
	ON_EN_CHANGE(IDC_ADDR1, OnChangeAddr1)
	ON_EN_CHANGE(IDC_ADDR2, OnChangeAddr2)
	ON_EN_CHANGE(IDC_CITY, OnChangeCity)
	ON_EN_CHANGE(IDC_COUNTRY, OnChangeCountry)
	ON_EN_CHANGE(IDC_DOB, OnChangeDob)
	ON_EN_CHANGE(IDC_HOMEPHONE, OnChangeHomephone)
	ON_EN_CHANGE(IDC_NAME, OnChangeName)
	ON_EN_CHANGE(IDC_PROFESSION, OnChangeProfession)
	ON_EN_CHANGE(IDC_STATE, OnChangeState)
	ON_EN_CHANGE(IDC_WORKPHONE, OnChangeWorkphone)
	ON_EN_CHANGE(IDC_ZIP, OnChangeZip)
	ON_EN_CHANGE(IDC_BL_STATUS_FLD, OnChangeBlStatusFld)
	ON_EN_CHANGE(IDC_CARDHOLDER_ID_FLD, OnChangeCardholderIdFld)
	ON_EN_CHANGE(IDC_MAIDEN_NAME_FLD, OnChangeMaidenNameFld)
	ON_EN_CHANGE(IDC_MEMBERSHIP_ID_FLD, OnChangeMembershipIdFld)
	ON_EN_CHANGE(IDC_SPECIALTY_CODE_FLD, OnChangeSpecialtyCodeFld)
	ON_EN_CHANGE(IDC_VIP_ACCOUNT_CODE_FLD, OnChangeVipAccountCodeFld)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCFDemogr message handlers

BOOL CCFDemogr::OnSetActive() 
{
	//if (DialogMode == MODE_DONE || DialogMode == MODE_VIEW)
	//	m_strComEdit.Empty();
	UpdateData(FALSE);

	if (DialogMode == MODE_VIEW || DialogMode == MODE_DONE)
	{
		m_Zip.EnableWindow(FALSE);
		m_WorkPhone.EnableWindow(FALSE);
		m_State.EnableWindow(FALSE);
		m_Name.EnableWindow(FALSE);
		m_HomePhone.EnableWindow(FALSE);
		m_City.EnableWindow(FALSE);
		m_Addr2.EnableWindow(FALSE);
		m_Addr1.EnableWindow(FALSE);
		m_Country.EnableWindow(FALSE);
		m_DOB.EnableWindow(FALSE);
		m_Profession.EnableWindow(FALSE);
		m_CntlBlockStatusFld.EnableWindow(FALSE);
		m_CntlCardholderIDFld.EnableWindow(FALSE);
		m_CntlMaidenNameFld.EnableWindow(FALSE);
		m_CntlMembershipIDFld.EnableWindow(FALSE);
		m_CntlSpecialtyCodeFld.EnableWindow(FALSE);
		m_CntlVIPAccountCodeFld.EnableWindow(FALSE);
	}
	else
	{
		m_Zip.EnableWindow(TRUE);
		m_WorkPhone.EnableWindow(TRUE);
		m_State.EnableWindow(TRUE);
		m_Name.EnableWindow(TRUE);
		m_HomePhone.EnableWindow(TRUE);
		m_City.EnableWindow(TRUE);
		m_Addr2.EnableWindow(TRUE);
		m_Addr1.EnableWindow(TRUE);
		m_Country.EnableWindow(TRUE);
		m_DOB.EnableWindow(TRUE);
		m_Profession.EnableWindow(TRUE);
		m_CntlBlockStatusFld.EnableWindow(TRUE);
		m_CntlCardholderIDFld.EnableWindow(TRUE);
		m_CntlMaidenNameFld.EnableWindow(TRUE);
		m_CntlMembershipIDFld.EnableWindow(TRUE);
		m_CntlSpecialtyCodeFld.EnableWindow(TRUE);
		m_CntlVIPAccountCodeFld.EnableWindow(TRUE);
	}

	return CPropertyPage::OnSetActive();
}

BOOL CCFDemogr::OnInitDialog() 
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

void CCFDemogr::OnChangeAddr1() 
{
	m_bDemogrChanged = TRUE;
	SetModified(TRUE);
	
}

void CCFDemogr::OnChangeAddr2() 
{
	m_bDemogrChanged = TRUE;
	SetModified(TRUE);
	
}

void CCFDemogr::OnChangeCity() 
{
	m_bDemogrChanged = TRUE;
	SetModified(TRUE);
	
}

void CCFDemogr::OnChangeCountry() 
{
	m_bDemogrChanged = TRUE;
	SetModified(TRUE);
	
}

void CCFDemogr::OnChangeDob() 
{
	m_bDemogrChanged = TRUE;
	SetModified(TRUE);
	
}

void CCFDemogr::OnChangeHomephone() 
{
	m_bDemogrChanged = TRUE;
	SetModified(TRUE);
	
}

void CCFDemogr::OnChangeName() 
{
	m_bDemogrChanged = TRUE;
	SetModified(TRUE);
	
}

void CCFDemogr::OnChangeProfession() 
{
	m_bDemogrChanged = TRUE;
	SetModified(TRUE);
	
}

void CCFDemogr::OnChangeState() 
{
	m_bDemogrChanged = TRUE;
	SetModified(TRUE);
	
}

void CCFDemogr::OnChangeWorkphone() 
{
	m_bDemogrChanged = TRUE;
	SetModified(TRUE);
	
}

void CCFDemogr::OnChangeZip() 
{
	m_bDemogrChanged = TRUE;
	SetModified(TRUE);
	
}

void CCFDemogr::OnChangeBlStatusFld() 
{
	m_bDemogrChanged = TRUE;
	SetModified(TRUE);
	
}

void CCFDemogr::OnChangeCardholderIdFld() 
{
	m_bDemogrChanged = TRUE;
	SetModified(TRUE);
	
}

void CCFDemogr::OnChangeMaidenNameFld() 
{
	m_bDemogrChanged = TRUE;
	SetModified(TRUE);
	
}

void CCFDemogr::OnChangeMembershipIdFld() 
{
	m_bDemogrChanged = TRUE;
	SetModified(TRUE);
	
}

void CCFDemogr::OnChangeSpecialtyCodeFld() 
{
	m_bDemogrChanged = TRUE;
	SetModified(TRUE);
	
}

void CCFDemogr::OnChangeVipAccountCodeFld() 
{
	m_bDemogrChanged = TRUE;
	SetModified(TRUE);
	
}

BOOL CCFDemogr::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class

	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);

	return CPropertyPage::OnCommand(wParam, lParam);
}
