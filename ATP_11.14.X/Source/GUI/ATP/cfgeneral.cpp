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
* $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\ATP_gui\cfgeneral.cpp  $
   
      
		Rev 1.13 Oct-24-2010 TF-Ajay
	Added text field to enter secondary expiry date for card holder data
	Rev 1.12   Apr 08 2005 08:47:20   lmitchel
   Add delete function to Purchase Card MCC ranges and correcting correct errors from testing
   
      Rev 1.11   Apr 01 2005 10:13:40   lmitchel
   Initial Checking prior to integration testing for updates made to support country/curreny code limits (scr13929), unulimited purchase card MCC ranges, and purchase card transaction limits(scr12785)
   
      Rev 1.10   Dec 01 2003 11:29:04   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.9   Nov 09 2001 16:46:26   lmitchel
   GUI support for RSB cards - Added 2 new fields, add_on_rate, add_on_term to card general page including support for add, update, view, change history
   
      Rev 1.8   Jan 25 2000 14:01:08   iarustam
   bug fixes
   
      Rev 1.7   Jan 05 2000 14:55:02   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.2   Oct 19 1999 11:52:42   iarustam
   bug fixes
   
      Rev 1.1   Sep 22 1999 16:04:08   iarustam
   Added manual batch entry GUI
   
      Rev 1.6   Jul 08 1999 16:19:48   iarustam
   Bug fixes
   
   
      Rev 1.5   May 21 1999 10:18:02   iarustam
   Bug fixes
   
   
      Rev 1.4   Apr 29 1999 14:57:20   iarustam
   Bug fixes
   
      Rev 1.3   Mar 22 1999 11:13:10   iarustam
   Bug fixes
   
   
      Rev 1.2   Mar 16 1999 16:30:42   iarustam
   Bug fixes
   
      Rev 1.1   Mar 04 1999 13:32:40   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:41:34   iarustam
   Initial Release
   

    
*
************************************************************************************/
// CFGeneral.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "CFGeneral.h"
#include "BinList.h"
#include "MyUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#include "SharedMem.h"

extern CSharedMem shrmem;

/////////////////////////////////////////////////////////////////////////////
// CCFGeneral property page

IMPLEMENT_DYNCREATE(CCFGeneral, CPropertyPage)

CCFGeneral::CCFGeneral() : CPropertyPage(CCFGeneral::IDD)
{
	//{{AFX_DATA_INIT(CCFGeneral)
	m_strCardNbr = _T("");
	m_StrMobNbr = _T("");
	m_strCardType = _T("");
	m_strCardStatus = _T("");
	m_strEffDate = _T("");
	m_strExpDate = _T("");
	m_strPAN = _T("");
	m_strPinOffset = _T("");
	m_strCreditLine = _T("");
	m_strCurCode = _T("");
	m_strVipProf = _T("");
	m_strOutstAuth = _T("");
	m_strCountryCode = _T("");
	m_strComEdit = _T("");
	m_StrCode1Fld = _T("");
	m_StrCode10Fld = _T("");
	m_StrCode2Fld = _T("");
	m_StrCode3Fld = _T("");
	m_StrCode4Fld = _T("");
	m_StrCode5Fld = _T("");
	m_StrCode6Fld = _T("");
	m_StrCode7Fld = _T("");
	m_StrCode8Fld = _T("");
	m_StrCode9Fld = _T("");
	m_StrPesoCurrentFld = _T("");
	m_StrPesoSavingsFld = _T("");
	m_StrDollarCurrentFld = _T("");
	m_StrDollarSavingsFld = _T("");
	m_StrPinRetryFld = _T("");
	m_BoolCashBonus = FALSE;
	m_BoolVisaPurchase = FALSE;
	m_strRsbRate = _T("");
	m_strRsbTerm = _T("");
	m_strDollarTranLimit = _T("");
	m_strPesoTranLimit = _T("");
	m_strSecExpDate = _T("");
	m_StrPanSeqNbr = _T("");
	//}}AFX_DATA_INIT
}

CCFGeneral::~CCFGeneral()
{
}

void CCFGeneral::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCFGeneral)
	DDX_Control(pDX, IDC_PAN_SEQ_NBR, m_CntlPanSeqNbr);
	DDX_Control(pDX, IDC_SECEXPDATE, m_cntlSecExpDate);
	DDX_Control(pDX, IDC_PESO_TRAN_LIMIT, m_ctrlPesoTranLimit);
	DDX_Control(pDX, IDC_DOLLAR_TRAN_LIMIT, m_ctrlDollarTranLimit);
	DDX_Control(pDX, IDC_RSBTERM, m_cntlRsbTerm);
	DDX_Control(pDX, IDC_RSBRATE, m_cntlRsbRate);
	DDX_Control(pDX, IDC_PINOFFSET, m_CntlPinOffset);
	DDX_Control(pDX, IDC_VISA_PURCHASE, m_VisaPurchase);
	DDX_Control(pDX, IDC_PIN_RETRY_FLD, m_CntlPinRetryFld);
	DDX_Control(pDX, IDC_CODE9_FLD, m_CntlCode9Fld);
	DDX_Control(pDX, IDC_CODE8_FLD, m_CntlCode8Fld);
	DDX_Control(pDX, IDC_CODE7_FLD, m_CntlCode7Fld);
	DDX_Control(pDX, IDC_CODE6_FLD, m_CntlCode6Fld);
	DDX_Control(pDX, IDC_CODE5_FLD, m_CntlCode5Fld);
	DDX_Control(pDX, IDC_CODE4_FLD, m_CntlCode4Fld);
	DDX_Control(pDX, IDC_CODE3_FLD, m_CntlCode3Fld);
	DDX_Control(pDX, IDC_CODE2_FLD, m_CntlCode2Fld);
	DDX_Control(pDX, IDC_CODE10_FLD, m_CntlCode10Fld);
	DDX_Control(pDX, IDC_CODE1_FLD, m_CntlCode1Fld);
	DDX_Control(pDX, IDC_CASH_BONUS, m_CashBonus);
	DDX_Control(pDX, IDC_VIPPROF, m_ctrlVipProf);
	DDX_Control(pDX, IDC_STATLIST, m_ctrlStatList);
	DDX_Control(pDX, IDC_PROFLIST, m_ctrlProfList);
	DDX_Control(pDX, IDC_PAN, m_ctrlPan);
	DDX_Control(pDX, IDC_EXPDATE, m_ctrlExpDate);
	DDX_Control(pDX, IDC_EFFECTIVEDATE, m_ctrlEffDate);
	DDX_Control(pDX, IDC_CURCODE, m_ctrlCurCode);
	DDX_Control(pDX, IDC_CREDITLINE, m_ctrlCreditLine);
	DDX_Control(pDX, IDC_COUNTRYCODE, m_ctrlCountryCode);
	DDX_Control(pDX, IDC_CARDSTATUS, m_ctrlCardStatus);
	DDX_Control(pDX, IDC_COMMENTS, m_ComEdit);
	DDX_Text(pDX, IDC_CARDNBR, m_strCardNbr);
	DDX_Text(pDX,IDC_CARDNBR2, m_StrMobNbr);
	DDV_MaxChars(pDX, m_StrMobNbr, 30);
	DDX_Text(pDX, IDC_CARDTYPE, m_strCardType);
	DDX_Text(pDX, IDC_CARDSTATUS, m_strCardStatus);
	DDV_MaxChars(pDX, m_strCardStatus, 2);
	DDX_Text(pDX, IDC_EFFECTIVEDATE, m_strEffDate);
	DDV_MaxChars(pDX, m_strEffDate, 8);
	DDX_Text(pDX, IDC_EXPDATE, m_strExpDate);
	DDV_MaxChars(pDX, m_strExpDate, 4);
	DDX_Text(pDX, IDC_PAN, m_strPAN);
	DDV_MaxChars(pDX, m_strPAN, 19);
	DDX_Text(pDX, IDC_PINOFFSET, m_strPinOffset);
	DDV_MaxChars(pDX, m_strPinOffset, 8);
	DDX_Text(pDX, IDC_CREDITLINE, m_strCreditLine);
	DDV_MaxChars(pDX, m_strCreditLine, 10);
	DDX_Text(pDX, IDC_CURCODE, m_strCurCode);
	DDV_MaxChars(pDX, m_strCurCode, 3);
	DDX_Text(pDX, IDC_VIPPROF, m_strVipProf);
	DDV_MaxChars(pDX, m_strVipProf, 4);
	DDX_Text(pDX, IDC_OUTSTAUTH, m_strOutstAuth);
	DDX_Text(pDX, IDC_COUNTRYCODE, m_strCountryCode);
	DDV_MaxChars(pDX, m_strCountryCode, 3);
	DDX_Text(pDX, IDC_COMMENTS, m_strComEdit);
	DDX_Text(pDX, IDC_CODE1_FLD, m_StrCode1Fld);
	DDV_MaxChars(pDX, m_StrCode1Fld, 4);
	DDX_Text(pDX, IDC_CODE10_FLD, m_StrCode10Fld);
	DDV_MaxChars(pDX, m_StrCode10Fld, 2);
	DDX_Text(pDX, IDC_CODE2_FLD, m_StrCode2Fld);
	DDV_MaxChars(pDX, m_StrCode2Fld, 2);
	DDX_Text(pDX, IDC_CODE3_FLD, m_StrCode3Fld);
	DDV_MaxChars(pDX, m_StrCode3Fld, 2);
	DDX_Text(pDX, IDC_CODE4_FLD, m_StrCode4Fld);
	DDV_MaxChars(pDX, m_StrCode4Fld, 2);
	DDX_Text(pDX, IDC_CODE5_FLD, m_StrCode5Fld);
	DDV_MaxChars(pDX, m_StrCode5Fld, 2);
	DDX_Text(pDX, IDC_CODE6_FLD, m_StrCode6Fld);
	DDV_MaxChars(pDX, m_StrCode6Fld, 2);
	DDX_Text(pDX, IDC_CODE7_FLD, m_StrCode7Fld);
	DDV_MaxChars(pDX, m_StrCode7Fld, 2);
	DDX_Text(pDX, IDC_CODE8_FLD, m_StrCode8Fld);
	DDV_MaxChars(pDX, m_StrCode8Fld, 2);
	DDX_Text(pDX, IDC_CODE9_FLD, m_StrCode9Fld);
	DDV_MaxChars(pDX, m_StrCode9Fld, 2);
	DDX_Text(pDX, IDC_PESO_CURRENT_FLD, m_StrPesoCurrentFld);
	DDX_Text(pDX, IDC_PESO_SAVINGS_FLD, m_StrPesoSavingsFld);
	DDX_Text(pDX, IDC_DOLLAR_CURRENT_FLD, m_StrDollarCurrentFld);
	DDX_Text(pDX, IDC_DOLLAR_SAVINGS_FLD, m_StrDollarSavingsFld);
	DDX_Text(pDX, IDC_PIN_RETRY_FLD, m_StrPinRetryFld);
	DDV_MaxChars(pDX, m_StrPinRetryFld, 1);
	DDX_Check(pDX, IDC_CASH_BONUS, m_BoolCashBonus);
	DDX_Check(pDX, IDC_VISA_PURCHASE, m_BoolVisaPurchase);
	DDX_Text(pDX, IDC_RSBRATE, m_strRsbRate);
	DDV_MaxChars(pDX, m_strRsbRate, 6);
	DDX_Text(pDX, IDC_RSBTERM, m_strRsbTerm);
	DDV_MaxChars(pDX, m_strRsbTerm, 3);
	DDX_Text(pDX, IDC_DOLLAR_TRAN_LIMIT, m_strDollarTranLimit);
	DDX_Text(pDX, IDC_PESO_TRAN_LIMIT, m_strPesoTranLimit);
	DDX_Text(pDX, IDC_SECEXPDATE, m_strSecExpDate);
	DDV_MaxChars(pDX, m_strSecExpDate, 4);
	DDX_Text(pDX, IDC_PAN_SEQ_NBR, m_StrPanSeqNbr);
	DDV_MaxChars(pDX, m_StrPanSeqNbr, 2);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCFGeneral, CPropertyPage)
	//{{AFX_MSG_MAP(CCFGeneral)
	ON_BN_CLICKED(IDC_PROFLIST, OnProflist)
	ON_BN_CLICKED(IDC_STATLIST, OnStatlist)
	ON_EN_CHANGE(IDC_CARDSTATUS, OnChangeCardstatus)
	ON_EN_CHANGE(IDC_COUNTRYCODE, OnChangeCountrycode)
	ON_EN_CHANGE(IDC_CREDITLINE, OnChangeCreditline)
	ON_EN_CHANGE(IDC_CURCODE, OnChangeCurcode)
	ON_EN_CHANGE(IDC_EFFECTIVEDATE, OnChangeEffectivedate)
	ON_EN_CHANGE(IDC_EXPDATE, OnChangeExpdate)
	ON_EN_CHANGE(IDC_PAN, OnChangePan)
	ON_EN_CHANGE(IDC_VIPPROF, OnChangeVipprof)
	ON_BN_CLICKED(IDC_CASH_BONUS, OnCashBonus)
	ON_EN_CHANGE(IDC_CODE1_FLD, OnChangeCode1Fld)
	ON_EN_CHANGE(IDC_CODE10_FLD, OnChangeCode10Fld)
	ON_EN_CHANGE(IDC_CODE2_FLD, OnChangeCode2Fld)
	ON_EN_CHANGE(IDC_CODE3_FLD, OnChangeCode3Fld)
	ON_EN_CHANGE(IDC_CODE4_FLD, OnChangeCode4Fld)
	ON_EN_CHANGE(IDC_CODE5_FLD, OnChangeCode5Fld)
	ON_EN_CHANGE(IDC_CODE6_FLD, OnChangeCode6Fld)
	ON_EN_CHANGE(IDC_CODE7_FLD, OnChangeCode7Fld)
	ON_EN_CHANGE(IDC_CODE8_FLD, OnChangeCode8Fld)
	ON_EN_CHANGE(IDC_CODE9_FLD, OnChangeCode9Fld)
	ON_EN_CHANGE(IDC_PIN_RETRY_FLD, OnChangePinRetryFld)
	ON_BN_CLICKED(IDC_VISA_PURCHASE, OnVisaPurchase)
	ON_EN_CHANGE(IDC_PINOFFSET, OnChangePinoffset)
	ON_EN_CHANGE(IDC_RSBRATE, OnChangeRsbrate)
	ON_EN_CHANGE(IDC_RSBTERM, OnChangeRsbterm)
	ON_EN_KILLFOCUS(IDC_RSBRATE, OnKillfocusRsbrate)
	ON_EN_CHANGE(IDC_PESO_TRAN_LIMIT, OnChangePesoTranLimit)
	ON_EN_CHANGE(IDC_DOLLAR_TRAN_LIMIT, OnChangeDollarTranLimit)
	//}}AFX_MSG_MAP

//	ON_CONTROL_RANGE(EN_CHANGE,IDC_VIPPROF,IDC_CARDSTATUS,SetModified(TRUE))

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCFGeneral message handlers

BOOL CCFGeneral::OnInitDialog() 
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

BOOL CCFGeneral::OnSetActive() 
{
	UpdateData(FALSE);

	if (DialogMode == MODE_VIEW || DialogMode == MODE_DONE)
	{
		m_ctrlCardStatus.EnableWindow(FALSE);
		m_ctrlCountryCode.EnableWindow(FALSE);
		m_ctrlCreditLine.EnableWindow(FALSE);
		m_ctrlCurCode.EnableWindow(FALSE);
		m_ctrlEffDate.EnableWindow(FALSE);
		m_ctrlExpDate.EnableWindow(FALSE);
		m_cntlSecExpDate.EnableWindow(FALSE); // TF-Ajay Additional expiry date checking 
		m_ctrlPan.EnableWindow(FALSE);
		m_ctrlProfList.EnableWindow(FALSE);
		m_ctrlStatList.EnableWindow(FALSE);
		m_ctrlVipProf.EnableWindow(FALSE);
		m_CashBonus.EnableWindow(FALSE);
		m_VisaPurchase.EnableWindow(FALSE);
		m_CntlCode1Fld.EnableWindow(FALSE);
		m_CntlCode10Fld.EnableWindow(FALSE);
		m_CntlCode2Fld.EnableWindow(FALSE);
		m_CntlCode3Fld.EnableWindow(FALSE);
		m_CntlCode4Fld.EnableWindow(FALSE);
		m_CntlCode5Fld.EnableWindow(FALSE);
		m_CntlCode6Fld.EnableWindow(FALSE);
		m_CntlCode7Fld.EnableWindow(FALSE);
		m_CntlCode8Fld.EnableWindow(FALSE);
		m_CntlCode9Fld.EnableWindow(FALSE);
		m_CntlPinRetryFld.EnableWindow(FALSE);
        m_CntlPinOffset.EnableWindow(FALSE);
		m_cntlRsbRate.EnableWindow(FALSE);
		m_cntlRsbTerm.EnableWindow(FALSE);
		m_ctrlDollarTranLimit.EnableWindow(FALSE);
		m_ctrlPesoTranLimit.EnableWindow(FALSE);
		m_CntlPanSeqNbr.EnableWindow(FALSE);

	}
	else
	{
		m_ctrlCardStatus.EnableWindow(TRUE);
		m_ctrlCountryCode.EnableWindow(TRUE);
		m_ctrlCreditLine.EnableWindow(TRUE);
		m_ctrlCurCode.EnableWindow(TRUE);
		m_ctrlEffDate.EnableWindow(TRUE);
		m_ctrlExpDate.EnableWindow(TRUE);
		m_cntlSecExpDate.EnableWindow(TRUE); // TF-Ajay Additional expiry date checking 
		m_ctrlPan.EnableWindow(TRUE);
		m_ctrlProfList.EnableWindow(TRUE);
		m_ctrlStatList.EnableWindow(TRUE);
		m_ctrlVipProf.EnableWindow(TRUE);
		m_CashBonus.EnableWindow(TRUE);
		m_VisaPurchase.EnableWindow(TRUE);
		m_CntlCode1Fld.EnableWindow(TRUE);
		m_CntlCode10Fld.EnableWindow(TRUE);
		m_CntlCode2Fld.EnableWindow(TRUE);
		m_CntlCode3Fld.EnableWindow(TRUE);
		m_CntlCode4Fld.EnableWindow(TRUE);
		m_CntlCode5Fld.EnableWindow(TRUE);
		m_CntlCode6Fld.EnableWindow(TRUE);
		m_CntlCode7Fld.EnableWindow(TRUE);
		m_CntlCode8Fld.EnableWindow(TRUE);
		m_CntlCode9Fld.EnableWindow(TRUE);
        m_CntlPinOffset.EnableWindow(TRUE);
		m_cntlRsbRate.EnableWindow(TRUE);
		m_cntlRsbTerm.EnableWindow(TRUE);
	    m_CntlPanSeqNbr.EnableWindow(TRUE);
		if(m_BoolVisaPurchase)
		{
			m_ctrlDollarTranLimit.EnableWindow(TRUE);
			m_ctrlPesoTranLimit.EnableWindow(TRUE);
		}
		else
		{
			m_ctrlDollarTranLimit.EnableWindow(FALSE);
			m_ctrlPesoTranLimit.EnableWindow(FALSE);
		}

		if ( bFoundCCF03 )
			m_CntlPinRetryFld.EnableWindow(TRUE); 
		else 
			m_CntlPinRetryFld.EnableWindow(FALSE); 

	}
	
	return CPropertyPage::OnSetActive();
}

void CCFGeneral::OnProflist() 
{
	CBinList listDlg;
    int nReturnCode;

	UpdateData(TRUE);
	listDlg.m_nDialogMode = MODE_PROFIDBYTYPE;
 	listDlg.m_strType = m_strCardType.Left(1);
    nReturnCode = listDlg.DoModal();
    if ( nReturnCode == IDOK)
	{
		m_strVipProf = listDlg.m_strProfId;
		UpdateData(FALSE);
		SetModified(TRUE);
	}
        
	
}

void CCFGeneral::OnStatlist() 
{
	CBinList listDlg;
    int nReturnCode;

	UpdateData(TRUE);
	listDlg.m_nDialogMode = MODE_STATUS;
    nReturnCode = listDlg.DoModal();
    if ( nReturnCode == IDOK)
	{
		m_strCardStatus = listDlg.m_strStatus;
		UpdateData(FALSE);
		SetModified(TRUE);
	}	
	
}



void CCFGeneral::OnChangeCardstatus() 
{
	SetModified(TRUE);
}

void CCFGeneral::OnChangeCountrycode() 
{
	SetModified(TRUE);
}

void CCFGeneral::OnChangeCreditline() 
{
	SetModified(TRUE);
}

void CCFGeneral::OnChangeCurcode() 
{
	SetModified(TRUE);
}

void CCFGeneral::OnChangeEffectivedate() 
{
	SetModified(TRUE);
}

void CCFGeneral::OnChangeExpdate() 
{
	SetModified(TRUE);
}

void CCFGeneral::OnChangePan() 
{
	SetModified(TRUE);
}

void CCFGeneral::OnChangeVipprof() 
{
	SetModified(TRUE);
}

void CCFGeneral::OnCashBonus() 
{
	SetModified(TRUE);
}

void CCFGeneral::OnChangeCode1Fld() 
{
	SetModified(TRUE);
}

void CCFGeneral::OnChangeCode10Fld() 
{
	SetModified(TRUE);
}

void CCFGeneral::OnChangeCode2Fld() 
{
	SetModified(TRUE);
}

void CCFGeneral::OnChangeCode3Fld() 
{
	SetModified(TRUE);
}

void CCFGeneral::OnChangeCode4Fld() 
{
	SetModified(TRUE);
}

void CCFGeneral::OnChangeCode5Fld() 
{
	SetModified(TRUE);
}

void CCFGeneral::OnChangeCode6Fld() 
{
	SetModified(TRUE);
}

void CCFGeneral::OnChangeCode7Fld() 
{
	SetModified(TRUE);
}

void CCFGeneral::OnChangeCode8Fld() 
{
	SetModified(TRUE);
}

void CCFGeneral::OnChangeCode9Fld() 
{
	SetModified(TRUE);
}

void CCFGeneral::OnChangePinRetryFld() 
{
	SetModified(TRUE);
}

void CCFGeneral::OnVisaPurchase() 
{
	UpdateData(TRUE);
	if(m_BoolVisaPurchase)
	{
		m_ctrlDollarTranLimit.EnableWindow(TRUE);
		m_ctrlPesoTranLimit.EnableWindow(TRUE);
	    m_ctrlPesoTranLimit.SetFocus();
	}
	else
	{
		m_ctrlDollarTranLimit.EnableWindow(FALSE);
		m_ctrlPesoTranLimit.EnableWindow(FALSE);
	}
	UpdateData(FALSE);

	SetModified(TRUE);
}

void CCFGeneral::OnChangePinoffset() 
{
	SetModified(TRUE);
}

void CCFGeneral::OnChangeRsbrate() 
{
	SetModified(TRUE);
}

void CCFGeneral::OnChangeRsbterm() 
{
	SetModified(TRUE);
}
void CCFGeneral::OnChangePesoTranLimit() 
{
	SetModified(TRUE);	
}

void CCFGeneral::OnChangeDollarTranLimit() 
{
	SetModified(TRUE);	
}

void CCFGeneral::OnKillfocusRsbrate() 
{
    CMyUtils myUtilsObject;
	char strMessage[250] = "";
	int res = 0;
	int length = 0;

	UpdateData(TRUE);

    CString strRate = myUtilsObject.GenerateTranAmount ( m_strRsbRate, 5, strMessage );
	if ( strRate == "Error" )
	{
		m_strComEdit = strMessage;
		UpdateData(FALSE);
        m_cntlRsbRate.SetFocus();
		return ;
	}
	else
	{
		m_strComEdit.Empty();
    	UpdateData(FALSE);
	}

}


BOOL CCFGeneral::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	

	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}
