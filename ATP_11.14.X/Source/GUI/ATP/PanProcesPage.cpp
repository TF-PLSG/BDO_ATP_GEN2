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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\PanProcesPage.cpp  $
   
      Rev 1.8   Dec 01 2003 11:30:02   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.7   Dec 23 1999 16:03:24   mbysani
   modified based on the documents
   
      Rev 1.1   Sep 22 1999 15:57:52   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:06   iarustam
   Initial Release
   

    
*
************************************************************************************/
// PanProcesPage.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "PanProcesPage.h"
#include "BinList.h"

#include "SharedMem.h"	
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
extern CSharedMem shrmem;
/////////////////////////////////////////////////////////////////////////////
// CPanProcesPage property page

IMPLEMENT_DYNCREATE(CPanProcesPage, CPropertyPage)

CPanProcesPage::CPanProcesPage() : CPropertyPage(CPanProcesPage::IDD)
{
	//{{AFX_DATA_INIT(CPanProcesPage)
	m_panstr = _T("");
	m_strAccStatus = _T("");
	m_strVipProc = _T("");
	m_strComEdit = _T("");
	m_strType = _T("");
	m_StrAccountNameFld = _T("");
	m_StrBlockStatusFld = _T("");
	m_StrCorpAccountNumFld = _T("");
	m_StrMinPayFld = _T("");
	m_StrPastAmountFld = _T("");
	m_StrCompName = _T("");
	m_StrCustrNbr = _T("");
	m_StrAppSource = _T("");
	m_StrCreditLimit = _T("");
	//}}AFX_DATA_INIT
}

CPanProcesPage::~CPanProcesPage()
{
}

void CPanProcesPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPanProcesPage)
	DDX_Control(pDX, IDC_APP_SOURCE_FLD, m_CntlAppSource);
	DDX_Control(pDX, IDC_COMP_NAME_FLD, m_CntlCompName);
	DDX_Control(pDX, IDC_CREDIT_LMT_FLD, m_CntlCreditLimit);
	DDX_Control(pDX, IDC_CUST_NBR_FLD, m_CntlCustNbr);
	DDX_Control(pDX, IDC_PAST_AMOUNT_FLD, m_CntlPastAmountFld);
	DDX_Control(pDX, IDC_MIN_PAY_FLD, m_CntlMinPayFld);
	DDX_Control(pDX, IDC_CORP_ACCOUNT_FLD, m_CntlCorpAccountNumFld);
	DDX_Control(pDX, IDC_BLOCK_STATUS_FLD, m_CntlBlockStatusFld);
	DDX_Control(pDX, IDC_ACCOUNT_NAME_FLD, m_CntlAccountNameFld);
	DDX_Control(pDX, IDC_TYPE, m_Type);
	DDX_Control(pDX, IDC_STATUSLIST, m_StatList);
	DDX_Control(pDX, IDC_PROFLIST, m_ProfList);
	DDX_Control(pDX, IDC_ACC_NUM, m_panedit);
	DDX_Control(pDX, IDC_VIP_PROC, m_VipProc);
	DDX_Control(pDX, IDC_ACC_STATUS, m_AccStatus);
	DDX_Control(pDX, IDC_COMMENTS, m_ComEdit);
	DDX_Text(pDX, IDC_ACC_NUM, m_panstr);
	DDV_MaxChars(pDX, m_panstr, 19);
	DDX_Text(pDX, IDC_ACC_STATUS, m_strAccStatus);
	DDV_MaxChars(pDX, m_strAccStatus, 2);
	DDX_Text(pDX, IDC_VIP_PROC, m_strVipProc);
	DDV_MaxChars(pDX, m_strVipProc, 4);
	DDX_Text(pDX, IDC_COMMENTS, m_strComEdit);
	DDX_Text(pDX, IDC_TYPE, m_strType);
	DDV_MaxChars(pDX, m_strType, 1);
	DDX_Text(pDX, IDC_ACCOUNT_NAME_FLD, m_StrAccountNameFld);
	DDV_MaxChars(pDX, m_StrAccountNameFld, 30);
	DDX_Text(pDX, IDC_BLOCK_STATUS_FLD, m_StrBlockStatusFld);
	DDV_MaxChars(pDX, m_StrBlockStatusFld, 4);
	DDX_Text(pDX, IDC_CORP_ACCOUNT_FLD, m_StrCorpAccountNumFld);
	DDV_MaxChars(pDX, m_StrCorpAccountNumFld, 10);
	DDX_Text(pDX, IDC_MIN_PAY_FLD, m_StrMinPayFld);
	DDX_Text(pDX, IDC_PAST_AMOUNT_FLD, m_StrPastAmountFld);
	DDX_Text(pDX, IDC_COMP_NAME_FLD, m_StrCompName);
	DDV_MaxChars(pDX, m_StrCompName, 100);
	DDX_Text(pDX, IDC_CUST_NBR_FLD, m_StrCustrNbr);
	DDV_MaxChars(pDX, m_StrCustrNbr, 12);
	DDX_Text(pDX, IDC_APP_SOURCE_FLD, m_StrAppSource);
	DDV_MaxChars(pDX, m_StrAppSource, 10);
	DDX_Text(pDX, IDC_CREDIT_LMT_FLD, m_StrCreditLimit);
	DDV_MaxChars(pDX, m_StrCreditLimit, 10);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPanProcesPage, CPropertyPage)
	//{{AFX_MSG_MAP(CPanProcesPage)
	ON_EN_CHANGE(IDC_VIP_PROC, OnChangeVipProc)
	ON_EN_CHANGE(IDC_ACC_STATUS, OnChangeAccStatus)
	ON_BN_CLICKED(IDC_PROFLIST, OnProflist)
	ON_BN_CLICKED(IDC_STATUSLIST, OnStatuslist)
	ON_EN_CHANGE(IDC_BLOCK_STATUS_FLD, OnChangeBlockStatusFld)
	ON_EN_CHANGE(IDC_CORP_ACCOUNT_FLD, OnChangeCorpAccountFld)
	ON_EN_CHANGE(IDC_ACCOUNT_NAME_FLD, OnChangeAccountNameFld)
	ON_EN_CHANGE(IDC_RELATED_ACCT_NBR_FLD, OnChangeRelatedAcctNbrFld)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPanProcesPage message handlers


BOOL CPanProcesPage::OnSetActive() 
{
	UpdateData(FALSE);
	m_ComEdit.SetWindowText("");

	if (DialogMode == MODE_VIEW || DialogMode == MODE_DONE)
	{
		m_VipProc.EnableWindow(FALSE);
		m_Type.EnableWindow(FALSE);
		m_AccStatus.EnableWindow(FALSE);
		m_ProfList.EnableWindow(FALSE);
		m_StatList.EnableWindow(FALSE);
		m_CntlAccountNameFld.EnableWindow(FALSE);
		m_CntlBlockStatusFld.EnableWindow(FALSE);
		m_CntlCorpAccountNumFld.EnableWindow(FALSE);
		m_CntlAppSource.EnableWindow(FALSE);
		m_CntlCompName.EnableWindow(FALSE);
		m_CntlCreditLimit.EnableWindow(FALSE);
		m_CntlCustNbr.EnableWindow(FALSE);
//		m_CntlRelatedAccountNbrFld.EnableWindow(FALSE);
//		m_CntlRelatedAccountType.EnableWindow(FALSE);

	}
	else
	{
		m_VipProc.EnableWindow(TRUE);
		m_Type.EnableWindow(TRUE);
		m_AccStatus.EnableWindow(TRUE);
		m_ProfList.EnableWindow(TRUE);
		m_StatList.EnableWindow(TRUE);
		m_CntlAccountNameFld.EnableWindow(TRUE);
		m_CntlBlockStatusFld.EnableWindow(TRUE);
		m_CntlCorpAccountNumFld.EnableWindow(TRUE);
		m_CntlAppSource.EnableWindow(TRUE);
		m_CntlCompName.EnableWindow(TRUE);
		m_CntlCreditLimit.EnableWindow(TRUE);
		m_CntlCustNbr.EnableWindow(TRUE);

//		m_CntlRelatedAccountNbrFld.EnableWindow(TRUE);
//		m_CntlRelatedAccountType.EnableWindow(TRUE);
	}

	if (DialogMode == MODE_DONE )
	{
		m_panedit.SetWindowText("");
		m_strVipProc.Empty();
		m_strType.Empty();
		m_strAccStatus.Empty();
		m_StrAccountNameFld = _T("");
		m_StrBlockStatusFld = _T("");
		m_StrCorpAccountNumFld = _T("");
		m_StrMinPayFld = _T("");
		m_StrPastAmountFld = _T("");
//		m_StrRelatedAccountNbrFld= _T("");
//		m_StrRelatedAccountType= _T("");
		m_StrCreditLimit = _T("");
		m_StrCustrNbr	 = _T("");
		m_StrAppSource	 = _T("");
		m_StrCompName	 = _T("");

		UpdateData(FALSE);
		SetModified(FALSE);
	}

		if (DialogMode == MODE_ADD )
	{
		m_panedit.SetWindowText("");
		m_strVipProc.Empty();
		m_strType.Empty();
		m_strAccStatus.Empty();
		m_StrAccountNameFld = _T("");
		m_StrBlockStatusFld = _T("");
		m_StrMinPayFld = _T("");
		m_StrCorpAccountNumFld = _T("");
		m_StrPastAmountFld = _T("");
//		m_StrRelatedAccountNbrFld= _T("");
//		m_StrRelatedAccountType= _T("");
		m_StrCreditLimit = _T("");
		m_StrCustrNbr	 = _T("");
		m_StrAppSource	 = _T("");
		m_StrCompName	 = _T("");

		UpdateData(FALSE);
		SetModified(FALSE);
	}


	return CPropertyPage::OnSetActive();
}

BOOL CPanProcesPage::OnInitDialog() 
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


void CPanProcesPage::OnChangeAccStatus() 
{
	SetModified(TRUE);
	
}

void CPanProcesPage::OnChangeVipProc() 
{
	SetModified(TRUE);
	
}

void CPanProcesPage::OnProflist() 
{
	CBinList listDlg;
    int nReturnCode;

	UpdateData(TRUE);

	listDlg.m_nDialogMode = MODE_PROFID;
    nReturnCode = listDlg.DoModal();
    if ( nReturnCode == IDOK)
	{
		m_strVipProc = listDlg.m_strProfId;
		m_strType = listDlg.m_strType.Left(1);
		UpdateData(FALSE);
		SetModified(TRUE);
	}
        
}

void CPanProcesPage::OnStatuslist() 
{
	CBinList listDlg;
    int nReturnCode;

	UpdateData(TRUE);

	listDlg.m_nDialogMode = MODE_STATUS;
    nReturnCode = listDlg.DoModal();
    if ( nReturnCode == IDOK)
	{
		m_strAccStatus = listDlg.m_strStatus;
		UpdateData(FALSE);
		SetModified(TRUE);
	}	
	
}

void CPanProcesPage::OnChangeBlockStatusFld() 
{
	SetModified(TRUE);
	
}

void CPanProcesPage::OnChangeCorpAccountFld() 
{
	SetModified(TRUE);
	
}

void CPanProcesPage::OnChangeAccountNameFld() 
{
	SetModified(TRUE);
	
}

void CPanProcesPage::OnChangeRelatedAcctNbrFld() 
{
	SetModified(TRUE);
	
}



BOOL CPanProcesPage::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}
