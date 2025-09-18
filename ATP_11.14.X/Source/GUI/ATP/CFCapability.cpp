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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\CFCapability.cpp  $
   
      Rev 1.5   Dec 01 2003 11:29:02   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.4   Jan 05 2000 14:52:56   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:42:24   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:16:52   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:57:56   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:55:50   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:39:58   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:00   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:57:36   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:06:28   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:42:20   iarustam
    
*
************************************************************************************/
// CFCapability.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "CFCapability.h"
#include "SharedMem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
extern CSharedMem shrmem;
/////////////////////////////////////////////////////////////////////////////
// CCFCapability property page

IMPLEMENT_DYNCREATE(CCFCapability, CPropertyPage)

CCFCapability::CCFCapability() : CPropertyPage(CCFCapability::IDD)
{
	//{{AFX_DATA_INIT(CCFCapability)
	m_strCardNbr = _T("");
	m_strCardType = _T("");
	m_CrCashAdv = FALSE;
	m_CrInquiry = FALSE;
	m_CrPayment = FALSE;
	m_CrPurchase = FALSE;
	m_CrReturn = FALSE;
	m_DebBalTransf = FALSE;
	m_DebCashWithd = FALSE;
	m_DebDeposit = FALSE;
	m_DebInquiry = FALSE;
	m_DebPurchase = FALSE;
	m_DebReturn = FALSE;
	m_FlInquiry = FALSE;
	m_FlPayment = FALSE;
	m_FlPurchase = FALSE;
	m_FlReturn = FALSE;
	m_LoyInquiry = FALSE;
	m_LoyLoad = FALSE;
	m_LoyPurchase = FALSE;
	m_LoyReturn = FALSE;
	m_LoyUnload = FALSE;
	m_FlCashAdv = FALSE;
	//}}AFX_DATA_INIT
}

CCFCapability::~CCFCapability()
{
}

void CCFCapability::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCFCapability)
	DDX_Control(pDX, IDC_LOY_INQUIRY, m_ctrlLoyInquiry);
	DDX_Control(pDX, IDC_LOY_LOAD, m_ctrlLoyLoad);
	DDX_Control(pDX, IDC_LOY_PURCHASE, m_ctrlLoyPurchase);
	DDX_Control(pDX, IDC_LOY_RETURN, m_ctrlLoyReturn);
	DDX_Control(pDX, IDC_LOY_UNLOAD, m_ctrlLoyUnload);
	DDX_Control(pDX, IDC_FL_RETURN, m_ctrlFlReturn);
	DDX_Control(pDX, IDC_FL_PURCHASE, m_ctrlFlPurchase);
	DDX_Control(pDX, IDC_FL_PAYMENT, m_ctrlFlPayment);
	DDX_Control(pDX, IDC_FL_INQUIRY, m_ctrlFlInquiry);
	DDX_Control(pDX, IDC_FL_CASHADV, m_ctrlFlCashAdv);
	DDX_Control(pDX, IDC_DEB_RETURN, m_ctrlDebReturn);
	DDX_Control(pDX, IDC_DEB_PURCHASE, m_ctrlDebPurchase);
	DDX_Control(pDX, IDC_DEB_INQUIRY, m_ctrlDebInquiry);
	DDX_Control(pDX, IDC_DEB_DEPOSIT, m_ctrlDebDeposit);
	DDX_Control(pDX, IDC_DEB_CASHWITHD, m_ctrlDebCashWithd);
	DDX_Control(pDX, IDC_DEB_BALTRANS, m_ctrlDebBalTransf);
	DDX_Control(pDX, IDC_CR_RETURN, m_ctrlCrReturn);
	DDX_Control(pDX, IDC_CR_PURCHASE, m_ctrlCrPurchase);
	DDX_Control(pDX, IDC_CR_PAYMENT, m_ctrlCrPayment);
	DDX_Control(pDX, IDC_CR_INQUIRY, m_ctrlCrInquiry);
	DDX_Control(pDX, IDC_CR_CASHADV, m_ctrlCrCashAdv);
	DDX_Control(pDX, IDC_LOYALTYBOX, m_LoyaltyBox);
	DDX_Control(pDX, IDC_CREDITBOX, m_CreditBox);
	DDX_Control(pDX, IDC_DEBITBOX, m_DebitBox);
	DDX_Control(pDX, IDC_FLEETBOX, m_FleetBox);
	DDX_Control(pDX, IDC_COMMENTS, m_ComEdit);
	DDX_Text(pDX, IDC_CARDNBR, m_strCardNbr);
	DDX_Text(pDX, IDC_CARDTYPE, m_strCardType);
	DDX_Check(pDX, IDC_CR_CASHADV, m_CrCashAdv);
	DDX_Check(pDX, IDC_CR_INQUIRY, m_CrInquiry);
	DDX_Check(pDX, IDC_CR_PAYMENT, m_CrPayment);
	DDX_Check(pDX, IDC_CR_PURCHASE, m_CrPurchase);
	DDX_Check(pDX, IDC_CR_RETURN, m_CrReturn);
	DDX_Check(pDX, IDC_DEB_BALTRANS, m_DebBalTransf);
	DDX_Check(pDX, IDC_DEB_CASHWITHD, m_DebCashWithd);
	DDX_Check(pDX, IDC_DEB_DEPOSIT, m_DebDeposit);
	DDX_Check(pDX, IDC_DEB_INQUIRY, m_DebInquiry);
	DDX_Check(pDX, IDC_DEB_PURCHASE, m_DebPurchase);
	DDX_Check(pDX, IDC_DEB_RETURN, m_DebReturn);
	DDX_Check(pDX, IDC_FL_INQUIRY, m_FlInquiry);
	DDX_Check(pDX, IDC_FL_PAYMENT, m_FlPayment);
	DDX_Check(pDX, IDC_FL_PURCHASE, m_FlPurchase);
	DDX_Check(pDX, IDC_FL_RETURN, m_FlReturn);
	DDX_Check(pDX, IDC_LOY_INQUIRY, m_LoyInquiry);
	DDX_Check(pDX, IDC_LOY_LOAD, m_LoyLoad);
	DDX_Check(pDX, IDC_LOY_PURCHASE, m_LoyPurchase);
	DDX_Check(pDX, IDC_LOY_RETURN, m_LoyReturn);
	DDX_Check(pDX, IDC_LOY_UNLOAD, m_LoyUnload);
	DDX_Check(pDX, IDC_FL_CASHADV, m_FlCashAdv);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCFCapability, CPropertyPage)
	//{{AFX_MSG_MAP(CCFCapability)
	ON_BN_CLICKED(IDC_CR_CASHADV, OnCrCashadv)
	ON_BN_CLICKED(IDC_CR_INQUIRY, OnCrInquiry)
	ON_BN_CLICKED(IDC_CR_PAYMENT, OnCrPayment)
	ON_BN_CLICKED(IDC_CR_PURCHASE, OnCrPurchase)
	ON_BN_CLICKED(IDC_CR_RETURN, OnCrReturn)
	ON_BN_CLICKED(IDC_CREDITBOX, OnCreditbox)
	ON_BN_CLICKED(IDC_DEB_BALTRANS, OnDebBaltrans)
	ON_BN_CLICKED(IDC_DEB_CASHWITHD, OnDebCashwithd)
	ON_BN_CLICKED(IDC_DEB_DEPOSIT, OnDebDeposit)
	ON_BN_CLICKED(IDC_DEB_INQUIRY, OnDebInquiry)
	ON_BN_CLICKED(IDC_DEB_PURCHASE, OnDebPurchase)
	ON_BN_CLICKED(IDC_DEB_RETURN, OnDebReturn)
	ON_BN_CLICKED(IDC_DEBITBOX, OnDebitbox)
	ON_BN_CLICKED(IDC_FL_CASHADV, OnFlCashadv)
	ON_BN_CLICKED(IDC_FL_INQUIRY, OnFlInquiry)
	ON_BN_CLICKED(IDC_FL_PAYMENT, OnFlPayment)
	ON_BN_CLICKED(IDC_FL_PURCHASE, OnFlPurchase)
	ON_BN_CLICKED(IDC_FL_RETURN, OnFlReturn)
	ON_BN_CLICKED(IDC_FLEETBOX, OnFleetbox)
	ON_BN_CLICKED(IDC_LOY_INQUIRY, OnLoyInquiry)
	ON_BN_CLICKED(IDC_LOY_LOAD, OnLoyLoad)
	ON_BN_CLICKED(IDC_LOY_PURCHASE, OnLoyPurchase)
	ON_BN_CLICKED(IDC_LOY_RETURN, OnLoyReturn)
	ON_BN_CLICKED(IDC_LOY_UNLOAD, OnLoyUnload)
	ON_BN_CLICKED(IDC_LOYALTYBOX, OnLoyaltybox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCFCapability message handlers

BOOL CCFCapability::OnInitDialog() 
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

BOOL CCFCapability::OnSetActive() 
{
	UpdateData(FALSE);

	if (DialogMode == MODE_ADD || DialogMode == MODE_EDIT)
	{
		if (m_strCardType == "Credit")
		{
			EnableCreditBox(TRUE);
			EnableDebitBox(FALSE);
			EnableFleetBox(FALSE) ;
			EnableLoyaltyBox(FALSE);
		}

		if (m_strCardType == "Debit")
		{
			EnableCreditBox(FALSE);
			EnableDebitBox(TRUE);
			EnableFleetBox(FALSE) ;
			EnableLoyaltyBox(FALSE);
		}

		if (m_strCardType == "Fleet")
		{
			EnableCreditBox(FALSE);
			EnableDebitBox(FALSE);
			EnableFleetBox(TRUE) ;
			EnableLoyaltyBox(FALSE);
		}

		if (m_strCardType == "Loyalty")
		{
			EnableCreditBox(FALSE);
			EnableDebitBox(FALSE);
			EnableFleetBox(FALSE) ;
			EnableLoyaltyBox(TRUE);
		}
	}
	else
	{
		EnableCreditBox(FALSE);
		EnableDebitBox(FALSE);
		EnableFleetBox(FALSE) ;
		EnableLoyaltyBox(FALSE);
	}

	return CPropertyPage::OnSetActive();
}

void CCFCapability::EnableCreditBox(BOOL boolval) 
{
	m_CreditBox.EnableWindow(boolval);
	m_ctrlCrCashAdv.EnableWindow(boolval);
	m_ctrlCrInquiry.EnableWindow(boolval);
	m_ctrlCrPayment.EnableWindow(boolval);
	m_ctrlCrPurchase.EnableWindow(boolval);
	m_ctrlCrReturn.EnableWindow(boolval);
	
	return ;
}

void CCFCapability::EnableDebitBox(BOOL boolval) 
{
	m_DebitBox.EnableWindow(boolval);
	m_ctrlDebBalTransf.EnableWindow(boolval);
	m_ctrlDebCashWithd.EnableWindow(boolval);
	m_ctrlDebDeposit.EnableWindow(boolval);
	m_ctrlDebInquiry.EnableWindow(boolval);
	m_ctrlDebPurchase.EnableWindow(boolval);
	m_ctrlDebReturn.EnableWindow(boolval);
	
	return ;
}

void CCFCapability::EnableFleetBox(BOOL boolval) 
{
	m_FleetBox.EnableWindow(boolval);
	m_ctrlFlCashAdv.EnableWindow(boolval);
	m_ctrlFlInquiry.EnableWindow(boolval);
	m_ctrlFlPayment.EnableWindow(boolval);
	m_ctrlFlPurchase.EnableWindow(boolval);
	m_ctrlFlReturn.EnableWindow(boolval);
	
	return ;
}

void CCFCapability::EnableLoyaltyBox(BOOL boolval) 
{
	m_LoyaltyBox.EnableWindow(boolval);
	m_ctrlLoyLoad.EnableWindow(boolval);
	m_ctrlLoyInquiry.EnableWindow(boolval);
	m_ctrlLoyUnload.EnableWindow(boolval);
	m_ctrlLoyPurchase.EnableWindow(boolval);
	m_ctrlLoyReturn.EnableWindow(boolval);
	
	return ;
}


void CCFCapability::OnCrCashadv() 
{
	SetModified(TRUE);
	
}

void CCFCapability::OnCrInquiry() 
{
	SetModified(TRUE);
	
}

void CCFCapability::OnCrPayment() 
{
	SetModified(TRUE);
	
}

void CCFCapability::OnCrPurchase() 
{
	SetModified(TRUE);
	
}

void CCFCapability::OnCrReturn() 
{
	SetModified(TRUE);
	
}

void CCFCapability::OnCreditbox() 
{
	SetModified(TRUE);
	
}

void CCFCapability::OnDebBaltrans() 
{
	SetModified(TRUE);
	
}

void CCFCapability::OnDebCashwithd() 
{
	SetModified(TRUE);
	
}

void CCFCapability::OnDebDeposit() 
{
	SetModified(TRUE);
	
}

void CCFCapability::OnDebInquiry() 
{
	SetModified(TRUE);
	
}

void CCFCapability::OnDebPurchase() 
{
	SetModified(TRUE);
	
}

void CCFCapability::OnDebReturn() 
{
	SetModified(TRUE);
	
}

void CCFCapability::OnDebitbox() 
{
	SetModified(TRUE);
	
}

void CCFCapability::OnFlCashadv() 
{
	SetModified(TRUE);
	
}

void CCFCapability::OnFlInquiry() 
{
	SetModified(TRUE);
	
}

void CCFCapability::OnFlPayment() 
{
	SetModified(TRUE);
	
}

void CCFCapability::OnFlPurchase() 
{
	SetModified(TRUE);
	
}

void CCFCapability::OnFlReturn() 
{
	SetModified(TRUE);
	
}

void CCFCapability::OnFleetbox() 
{
	SetModified(TRUE);
	
}

void CCFCapability::OnLoyInquiry() 
{
	SetModified(TRUE);
	
}

void CCFCapability::OnLoyLoad() 
{
	SetModified(TRUE);
	
}

void CCFCapability::OnLoyPurchase() 
{
	SetModified(TRUE);
	
}

void CCFCapability::OnLoyReturn() 
{
	SetModified(TRUE);
	
}

void CCFCapability::OnLoyUnload() 
{
	SetModified(TRUE);
	
}

void CCFCapability::OnLoyaltybox() 
{
	SetModified(TRUE);
	
}
