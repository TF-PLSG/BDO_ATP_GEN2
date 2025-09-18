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
* $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\ATP_gui\CFMAIN.CPP  $
   
	 	Rev 1.13 Oct-24-2010 TF-Ajay
	Added text field to enter secondary expiry date for card holder data

      Rev 1.33   Apr 08 2005 08:47:20   lmitchel
   Add delete function to Purchase Card MCC ranges and correcting correct errors from testing
   
      Rev 1.32   Apr 01 2005 10:13:40   lmitchel
   Initial Checking prior to integration testing for updates made to support country/curreny code limits (scr13929), unulimited purchase card MCC ranges, and purchase card transaction limits(scr12785)
   
      Rev 1.31   Dec 01 2003 11:29:06   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.30   Jul 10 2002 09:55:58   lmitchel
   Ver:4.0.3.1:  Added SendMessage command in PropertySheet files (on_init function) to set the Apply button as the default.  If the enter key is pressed, action will default to  the apply button instead of the OK button.  This allows the user to remain in the current dialog instead of exiting to the main menu.
   
      Rev 1.29   Jun 15 2002 09:40:34   dirby
   Modified so that history table is updated whenever a change is
   made.  Previously, would not update history if number of failed
   pin attempts was not zero.
   SCR 785
   
      Rev 1.28   May 21 2002 13:34:02   dirby
   Modified to not exit when <enter> is pressed.  If OK is selected,
   the database gets updated still, but screen is not exited.
   SCR 587
   
      Rev 1.27   Nov 09 2001 16:46:26   lmitchel
   GUI support for RSB cards - Added 2 new fields, add_on_rate, add_on_term to card general page including support for add, update, view, change history
   
      Rev 1.26   Mar 21 2000 12:27:14   iarustam
   scr 507
   
      Rev 1.25   Mar 16 2000 10:38:18   iarustam
   Added alpha lookup screens and updated
   Merchant main, Card Record main and 
   primary account main screens
   
      Rev 1.24   Feb 01 2000 17:48:12   iarustam
   SCR # 333,340
   
      Rev 1.23   Jan 31 2000 13:08:32   iarustam
   SCR # 336
   
      Rev 1.22   Jan 25 2000 13:59:52   iarustam
   bug fixes
   
      Rev 1.21   Jan 21 2000 15:54:06   iarustam
   Fixed pvki and pin_key problems
   
      Rev 1.20   Jan 21 2000 12:22:38   iarustam
   Bug fixes # 14,24,,52,63,65,151,165,91,228,202,180,205,504,152,179,251,109,182,177,183,224,225,217,229,230,231,247,200,199,203,201,206,172
   
      Rev 1.19   Jan 14 2000 16:27:14   mbysani
   bug fixes
   
      Rev 1.18   Jan 12 2000 14:14:32   mbysani
   Changes mades acc to bugs
   
      Rev 1.17   Dec 16 1999 09:34:02   mbysani
   corrected some error based on error reports
   
      Rev 1.16   Dec 13 1999 11:49:14   mbysani
    
   
      Rev 1.4   Oct 19 1999 11:51:30   iarustam
   bug fixes
   
      Rev 1.3   Sep 30 1999 16:18:06   npogosya
   Bug fixes
   
      Rev 1.1   Sep 22 1999 15:42:52   iarustam
   Added manual batch entry GUI
   
      Rev 1.13   Jul 08 1999 16:06:16   iarustam
   Bug fixes
   
      Rev 1.0   Jul 01 1999 14:16:54   iarustam
   Initial Release
   
 

    
*
************************************************************************************/
// CFMain.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "CFMain.h"
#include "BinList.h"
#include "MyUtils.h"
#include "AlphaLookup.h"
//#include "Login.h"
#include "SharedMem.h"

extern "C"
{
#include "Txutils.h"
#include "PteTime.h"
}

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern UCF01 ucf01;
extern CString name;
extern CCF03 sCCF03Record;

extern CSharedMem shrmem;

/////////////////////////////////////////////////////////////////////////////
// CCFMain property page

IMPLEMENT_DYNCREATE(CCFMain, CPropertyPage)

CCFMain::CCFMain() : CPropertyPage(CCFMain::IDD)
{
	//{{AFX_DATA_INIT(CCFMain)
	m_strCardType = _T("");
	m_strCardNbr = _T("");
	m_strComEdit = _T("");
	//}}AFX_DATA_INIT
}

CCFMain::~CCFMain()
{
}

void CCFMain::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCFMain)
	DDX_Control(pDX, IDC_SEARCH_BUTTON, m_Search);
	DDX_Control(pDX, IDC_CARDTYPE, m_ctrlCardType);
	DDX_Control(pDX, IDC_COMMENTS, m_ComEdit);
	DDX_Control(pDX, IDC_ADD, m_Add);
	DDX_Control(pDX, IDC_VIEW, m_View);
	DDX_Control(pDX, IDC_UPDATE, m_Update);
	DDX_Control(pDX, IDC_LIST, m_List);
	DDX_Control(pDX, IDC_DELETE, m_Delete);
	DDX_Control(pDX, IDC_CARDNBR, m_ctrlCardNbr);
	DDX_Text(pDX, IDC_CARDTYPE, m_strCardType);
	DDX_Text(pDX, IDC_CARDNBR, m_strCardNbr);
	DDV_MaxChars(pDX, m_strCardNbr, 19);
	DDX_Text(pDX, IDC_COMMENTS, m_strComEdit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCFMain, CPropertyPage)
	//{{AFX_MSG_MAP(CCFMain)
	ON_BN_CLICKED(IDC_LIST, OnList)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_VIEW, OnView)
	ON_BN_CLICKED(IDC_UPDATE, OnUpdate)
	ON_EN_CHANGE(IDC_CARDNBR, OnChangeCardnbr)
	ON_EN_CHANGE(IDC_CARDTYPE, OnChangeCardtype)
	ON_BN_CLICKED(IDC_SEARCH_BUTTON, OnSearchButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCFMain message handlers

BOOL CCFMain::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	CFont m_font;
	LOGFONT lf;
	
	UpdateData(TRUE);
	memset ( &lf, 0, sizeof (lf));
	lf.lfHeight = 12;
	lf.lfWeight = 700;
	strcpy(lf.lfFaceName, "Arial");
	m_font.CreateFontIndirect (&lf);
	GetDlgItem(IDC_COMMENTS)->SetFont (&m_font);

	m_ctrlCardNbr.SetWindowText("");
	m_ctrlCardType.SetCurSel(0);	

	CString temp;
	int nIndex = -1;

	for ( int i = 0; i <=15; i++ )
	{
		temp = ucf01.control[i].subsystem_name;
		if ( strcmp ( temp, "Card Account Control" ) == 0)
			nIndex = i;
	}

	if ( nIndex != -1)
	{
		if ( ucf01.control[nIndex].action[0] == '0' )
			m_Add.EnableWindow(FALSE);
		if ( ucf01.control[nIndex].action[1] == '0' )
			m_Delete.EnableWindow(FALSE);
		if ( ucf01.control[nIndex].action[2] == '0' )
			m_Update.EnableWindow(FALSE);
		if ( ucf01.control[nIndex].action[3] == '0' )
			m_View.EnableWindow(FALSE);
	}

   m_strComEdit = "";
   UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CCFMain::CleanPages() 
{
	//Clear key variables 
	pGenPage->m_strCardNbr.Empty();
	pGenPage->m_strCardType.Empty();
	pBalPage->m_strCardNbr.Empty();
	pBalPage->m_strCardType.Empty();
	pCapPage->m_strCardNbr.Empty();
	pCapPage->m_strCardType.Empty();
	pDemogrPage->m_strCardNbr.Empty();
	pDemogrPage->m_strCardType.Empty();
	pActivPage->m_strCardNbr.Empty();
	pActivPage->m_strCardType.Empty();
	pMerchUsPage->m_strCardNbr.Empty();
	pMerchUsPage->m_strCardType.Empty();
	pTxPage->m_strCardNbr.Empty();
	pTxPage->m_strCardType.Empty();

	//Clear variables of GenPage
	pGenPage->m_strCardStatus.Empty();
	pGenPage->m_strEffDate.Empty();
	pGenPage->m_strExpDate.Empty();
    pGenPage->m_strSecExpDate.Empty(); // TF-Ajay Additional expiry date checking Oct-21-2010
	pGenPage->m_strPAN.Empty();
	pGenPage->m_strPinOffset.Empty();
	pGenPage->m_strCountryCode.Empty();
//	pGenPage->m_strCreditLine.Empty(); removed EB
	pGenPage->m_strCurCode.Empty();
	pGenPage->m_strOutstAuth.Empty();
	pGenPage->m_strRsbRate.Empty();
	pGenPage->m_strRsbTerm.Empty();
	pGenPage->m_strVipProf.Empty();
	pGenPage->m_strComEdit.Empty();
	pGenPage->m_StrPinRetryFld.Empty();
	pGenPage->m_StrCode1Fld.Empty();
	pGenPage->m_StrCode2Fld.Empty();
	pGenPage->m_StrCode3Fld.Empty();
	pGenPage->m_StrCode4Fld.Empty();
	pGenPage->m_StrCode5Fld.Empty();
	pGenPage->m_StrCode6Fld.Empty();
	pGenPage->m_StrCode7Fld.Empty();
	pGenPage->m_StrCode8Fld.Empty();
	pGenPage->m_StrCode9Fld.Empty();
	pGenPage->m_StrCode10Fld.Empty();
	pGenPage->m_StrPesoCurrentFld.Empty();
	pGenPage->m_StrPesoSavingsFld.Empty();
	pGenPage->m_StrDollarCurrentFld.Empty();
	pGenPage->m_StrDollarSavingsFld.Empty();
	pGenPage->m_strDollarTranLimit.Empty();
	pGenPage->m_strPesoTranLimit.Empty();
	pGenPage->m_StrPanSeqNbr.Empty();
    pGenPage->m_BoolCashBonus = FALSE;
	pGenPage->m_BoolVisaPurchase = FALSE;

	//Clear variables of BalPage
	pBalPage->m_strNbr1.Empty();
	pBalPage->m_strNbr2.Empty();
	pBalPage->m_strNbr3.Empty();
	pBalPage->m_strNbr4.Empty();
	pBalPage->m_strNbr5.Empty();
	pBalPage->m_strNbr6.Empty();
	pBalPage->m_strNbr7.Empty();
	pBalPage->m_strNbr8.Empty();
	pBalPage->m_strNbr9.Empty();
	pBalPage->m_strNbr10.Empty();

	pBalPage->m_strType1.Empty();
   pBalPage->m_strType1 = "";

	pBalPage->m_strType2.Empty();
	pBalPage->m_strType3.Empty();
	pBalPage->m_strType4.Empty();
	pBalPage->m_strType5.Empty();
	pBalPage->m_strType6.Empty();
	pBalPage->m_strType7.Empty();
	pBalPage->m_strType8.Empty();
	pBalPage->m_strType9.Empty();
	pBalPage->m_strType10.Empty();
	
	//Clear variables of CapPage
	pCapPage->m_CrCashAdv = FALSE;
	pCapPage->m_CrInquiry = FALSE;
	pCapPage->m_CrPayment = FALSE;
	pCapPage->m_CrPurchase = FALSE;
	pCapPage->m_CrReturn = FALSE;
	pCapPage->m_DebBalTransf = FALSE;
	pCapPage->m_FlCashAdv = FALSE;
	pCapPage->m_DebCashWithd = FALSE;
	pCapPage->m_DebDeposit = FALSE;
	pCapPage->m_DebInquiry = FALSE;
	pCapPage->m_DebPurchase = FALSE;
	pCapPage->m_DebReturn = FALSE;
	pCapPage->m_FlInquiry = FALSE;
	pCapPage->m_FlPayment = FALSE;
	pCapPage->m_FlPurchase = FALSE;
	pCapPage->m_FlReturn = FALSE;
	pCapPage->m_LoyInquiry = FALSE;
	pCapPage->m_LoyLoad = FALSE;
	pCapPage->m_LoyPurchase = FALSE;
	pCapPage->m_LoyReturn = FALSE;
	pCapPage->m_LoyUnload = FALSE;

	//Clear variables of DemogrPage
	pDemogrPage->m_bDemogrChanged = FALSE;

	pDemogrPage->m_strName.Empty();
	pDemogrPage->m_strAddr1.Empty();
	pDemogrPage->m_strAddr2.Empty();
	pDemogrPage->m_strCity.Empty();
	pDemogrPage->m_strCountry.Empty();
	pDemogrPage->m_strState.Empty();
	pDemogrPage->m_strZip.Empty();
	pDemogrPage->m_strHomePhone.Empty();
	pDemogrPage->m_strWorkPhone.Empty();
	pDemogrPage->m_strDOB.Empty();
	pDemogrPage->m_strProfession.Empty();
	pDemogrPage->m_strComEdit.Empty();
	pDemogrPage->m_StrBlockStatusFld.Empty();
	pDemogrPage->m_StrCardholderIDFld.Empty();
	pDemogrPage->m_StrMaidenNameFld.Empty();
	pDemogrPage->m_StrMembershipIDFld.Empty();
	pDemogrPage->m_StrSpecialtyCodeFld.Empty();
	pDemogrPage->m_StrVIPAccountCodeFld.Empty();

	//Clear variables of ActivPage
	pActivPage->m_strP1Amt.Empty();
	pActivPage->m_strP2Amt.Empty();
	pActivPage->m_strP3Amt.Empty();
	pActivPage->m_strP1Nbr.Empty();
	pActivPage->m_strP2Nbr.Empty();
	pActivPage->m_strP3Nbr.Empty();

	//Clear MerchUsPage
	if (pMerchUsPage->m_bMerchUsage && pMerchUsPage->m_bCCF03)
	{
		pMerchUsPage->m_ctrlMCCList.DeleteAllItems();
		pMerchUsPage->m_ctrlRepeatList.DeleteAllItems();
	}

	//Clear TxPage
	if (pTxPage->m_bTxPage)
		pTxPage->m_TxList.DeleteAllItems();

	//Clear ChangeHistoryPage
	if (pChHistPage->m_bChangeHistory)
		pChHistPage->m_CntlList.DeleteAllItems();

		
}


void CCFMain::SetModes(int mode) 
{
	DialogMode = mode;
	pGenPage->DialogMode = mode;
	pBalPage->DialogMode = mode;
	pCapPage->DialogMode = mode;
	pDemogrPage->DialogMode = mode;
	pActivPage->DialogMode = mode;
	pMerchUsPage->DialogMode = mode;
	pTxPage->DialogMode = mode;
	pChHistPage->m_nDialogMode = mode;

}


BOOL CCFMain::KeyEntered()
{
	UpdateData(TRUE);

	if (m_strCardType.IsEmpty())
	{
		m_ComEdit.SetWindowText("Please enter Card Type");
		m_ctrlCardType.SetFocus();
		return FALSE;
	}

	if (m_strCardNbr.IsEmpty())
	{
		m_ComEdit.SetWindowText("Please enter Card Number");
		m_ctrlCardNbr.SetFocus();
		return FALSE;
	}

	return TRUE;
}

void CCFMain::SetKeys()
{
		pGenPage->m_strCardNbr = m_strCardNbr;
		pGenPage->m_strCardType = m_strCardType;
		pBalPage->m_strCardNbr = m_strCardNbr;
		pBalPage->m_strCardType = m_strCardType;
		pCapPage->m_strCardNbr = m_strCardNbr;
		pCapPage->m_strCardType = m_strCardType;
		pDemogrPage->m_strCardNbr = m_strCardNbr;
		pDemogrPage->m_strCardType = m_strCardType;
		pActivPage->m_strCardNbr = m_strCardNbr;
		pActivPage->m_strCardType = m_strCardType;
		pMerchUsPage->m_strCardNbr = m_strCardNbr;
		pMerchUsPage->m_strCardType = m_strCardType;
		pTxPage->m_strCardNbr = m_strCardNbr;
		pTxPage->m_strCardType = m_strCardType;

}


BOOL CCFMain::OnSetActive() 
{
	/*if (DialogMode == MODE_DONE || DialogMode == MODE_VIEW)
		m_strComEdit.Empty();
	UpdateData(FALSE);

	if (DialogMode == MODE_ADD || DialogMode == MODE_EDIT)
	{
		m_ctrlCardNbr.EnableWindow(FALSE);
		m_ctrlCardType.EnableWindow(FALSE);
		m_View.EnableWindow(FALSE);
		m_Update.EnableWindow(FALSE);
		m_List.EnableWindow(FALSE);
		m_Delete.EnableWindow(FALSE);
		m_Add.EnableWindow(FALSE);
	}
	else
	{
		m_ctrlCardNbr.EnableWindow(TRUE);
		m_ctrlCardType.EnableWindow(TRUE);
		m_View.EnableWindow(TRUE);
		m_Update.EnableWindow(TRUE);
		m_List.EnableWindow(TRUE);
		m_Delete.EnableWindow(TRUE);
		m_Add.EnableWindow(TRUE);
	}*/

	return CPropertyPage::OnSetActive();
}

void CCFMain::OnList() 
{
	CBinList listDlg;
    int nReturnCode;

	UpdateData(TRUE);
	listDlg.m_nDialogMode = MODE_CCF;
	listDlg.strItem0 = m_strCardType.Left(1);
	listDlg.strItem1 = m_strCardNbr;

    nReturnCode = listDlg.DoModal();
    if ( nReturnCode == IDOK)
	{
		m_strCardNbr = listDlg.m_strCardNbr;
		if (listDlg.m_strCardType == "CREDIT")
			m_ctrlCardType.SetCurSel(0);
		if (listDlg.m_strCardType == "DEBIT")
			m_ctrlCardType.SetCurSel(1);
		if (listDlg.m_strCardType == "FLEET")
			m_ctrlCardType.SetCurSel(2);
		if (listDlg.m_strCardType == "LOYALTY")
			m_ctrlCardType.SetCurSel(3);

		CleanPages();    
	}

    if (!UpdateData(FALSE))
	{
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}		
	
}

BOOL CCFMain::GetCCFData(int DlgMode)
{
	char ErrorMsg[500];
	CCF02P	sCCF02Record;
	CCF01	sCCF01Record;
	CAF01	sCAF01Record;
	BOOL  retvalue = TRUE;
	CMyUtils myUtilsObject;
	char str_mob_num[31] = {0};
	int mob_num_len = 0;
	if (! KeyEntered())
		return FALSE;

	if (DlgMode == MODE_ADD)
	{
		//Check card number for mod10
		INT i, len, dble = 0, result = 0;

		char pCardNbr[20];
		len = m_strCardNbr.GetLength();
		lstrcpy ((LPTSTR) pCardNbr,m_strCardNbr);

		for (i = len - 1; i > 0; i--)
		{
			result += (pCardNbr[i] - 48);
			i--;
			dble = (pCardNbr[i] - 48) * 2;
			result = (dble >= 10) ? (result + 1 + (dble - 10)) : (result + dble);
		}

		if (len%2)
			result += (pCardNbr[i] - 48);

		if ((result%10) != 0)
		{
			m_ComEdit.SetWindowText("ERROR: Invalid Card Number");
			m_ctrlCardNbr.SetFocus();
			return FALSE;
		} 
/*		char pCardNbr[20];
		int nLastDigit;

		int len = m_strCardNbr.GetLength();
		if ( 15 != len )
		{
			m_ComEdit.SetWindowText("ERROR: Card Number Length must be 15");
			m_ctrlCardNbr.SetFocus();
			return FALSE;

		}
		lstrcpy ((LPTSTR) pCardNbr,m_strCardNbr);
		nLastDigit = myUtilsObject.GetMod10CheckDigit ( pCardNbr, len );
		itoa ( nLastDigit, pCardNbr, 10 );
		m_strCardNbr = m_strCardNbr + pCardNbr;*/
	/*	pGenPage->m_CashBonus.SetCheck(0);
		pGenPage->m_VisaPurchase.SetCheck(0);*/
		UpdateData(FALSE);
	}


	ZeroMemory(&sCCF02Record,sizeof(CCF02P)) ;

    memcpy ((LPSTR)(sCCF02Record.primary_key.card_type), m_strCardType.Left(1) , sizeof (sCCF02Record.primary_key.card_type));
    memcpy ((LPSTR)(sCCF02Record.primary_key.card_nbr), m_strCardNbr , sizeof (sCCF02Record.primary_key.card_nbr));

	int rcode2 = txdsapi_get_record((pBYTE)&sCCF02Record, sizeof(CCF02P),CCF02P_DATA, ErrorMsg);
	if( rcode2 == PTEMSG_OK )
	{
		switch (DlgMode)
		{
		case MODE_ADD:
			{
				m_ComEdit.SetWindowText("ERROR: Card Record already exists");
				retvalue = FALSE;
				break;
			}
		case MODE_EDIT:
		case MODE_VIEW:
			{
				CString strCode;
				// Save retrieved records in sOldXXXRecord for use in change history table.
				ZeroMemory(&sOldCCF02Record, sizeof(CCF02P));
				memcpy(&sOldCCF02Record, &sCCF02Record, sizeof(CCF02P));

				// Populate all the CCF fields
				pGenPage->m_strCardStatus.Format((LPCTSTR)sCCF02Record.status);
				pGenPage->m_strEffDate.Format((LPCTSTR)sCCF02Record.effect_date);
            if ( atol ( pGenPage->m_strEffDate ) != 0 )
				   pGenPage->m_strEffDate = pGenPage->m_strEffDate.Right(2) + 
										pGenPage->m_strEffDate.Mid(4,2) +
										pGenPage->m_strEffDate.Left(4);
				pGenPage->m_strExpDate.Format((LPCTSTR)sCCF02Record.expire_date);
	            pGenPage->m_strSecExpDate.Format((LPCTSTR)sCCF02Record.maint_date);
				pGenPage->m_strPinOffset.Format((LPCTSTR)sCCF02Record.pin_offset);
				pGenPage->m_strPAN.Format((LPCTSTR)sCCF02Record.primary_acct_nbr);				
//				pGenPage->m_strCreditLine.Format((LPCTSTR)sCCF02Record.credit_line); removed for EB
//				pGenPage->m_strCreditLine = myUtilsObject.GetAmount(pGenPage->m_strCreditLine); removed for EB
				pGenPage->m_strCurCode.Format((LPCTSTR)sCCF02Record.currency_code);
				pGenPage->m_strCountryCode.Format((LPCTSTR)sCCF02Record.country_code);
				pGenPage->m_strVipProf.Format((LPCTSTR)sCCF02Record.caf_profile_id);
				pGenPage->m_strOutstAuth.Format((LPCTSTR)sCCF02Record.outstanding_auths);
				strCode = sCCF02Record.product_codes;				
				pGenPage->m_StrPesoCurrentFld.Format((LPCTSTR)sCCF02Record.peso_current_acct);
				pGenPage->m_StrPesoSavingsFld.Format((LPCTSTR)sCCF02Record.peso_saving_acct);
				pGenPage->m_StrDollarCurrentFld.Format((LPCTSTR)sCCF02Record.dollar_current_acct);
				pGenPage->m_StrDollarSavingsFld.Format((LPCTSTR)sCCF02Record.dollar_saving_acct);
				//pGenPage->m_strRsbRate.Format ((LPCTSTR)sCCF02Record.add_on_rate);
				pGenPage->m_strRsbTerm.Format ((LPCTSTR)sCCF02Record.add_on_term);
				pGenPage->m_strRsbRate = myUtilsObject.DisplayAmount ( sCCF02Record.add_on_rate );
				pGenPage->m_StrPanSeqNbr.Format((LPCTSTR)sCCF02Record.percent_deposit);

				strncpy(str_mob_num,(const char *)&sCCF02Record.pin[20],30); // 30 will be length of mobile num.
				mob_num_len = strlen(str_mob_num);
				for (int i = 1 ; i < 5; i++)
				{
					str_mob_num[i] = 'X';
				}
				pGenPage->m_StrMobNbr = str_mob_num;

				if(strlen((LPCTSTR)sCCF02Record.peso_tran_limit) > 0)
				{
					pGenPage->m_strPesoTranLimit.Format((LPCTSTR)sCCF02Record.peso_tran_limit);
					pGenPage->m_strPesoTranLimit = myUtilsObject.DisplayAmount(pGenPage->m_strPesoTranLimit); 
				}

				if(strlen((LPCTSTR)sCCF02Record.dollar_tran_limit) > 0)
				{
					pGenPage->m_strDollarTranLimit.Format((LPCTSTR)sCCF02Record.dollar_tran_limit); 
					pGenPage->m_strDollarTranLimit = myUtilsObject.DisplayAmount(pGenPage->m_strDollarTranLimit);
				}



            int len = strCode.GetLength();
            if ( len != 0 )
            {
				   pGenPage->m_StrCode1Fld = strCode.Left(2);
               if ( len > 2 )
               {
				      pGenPage->m_StrCode2Fld = strCode.Mid(2,2);
                  if ( len > 4 )
                  {
				         pGenPage->m_StrCode3Fld = strCode.Mid(4,2);
                     if ( len > 6 )
                     {
				            pGenPage->m_StrCode4Fld = strCode.Mid(6,2);
                        if ( len > 8 )
                        {
				               pGenPage->m_StrCode5Fld = strCode.Mid(8,2);
                           if ( len > 10 )
                           {
				                  pGenPage->m_StrCode6Fld = strCode.Mid(10,2);
                              if ( len > 12 )
                              {
				                     pGenPage->m_StrCode7Fld = strCode.Mid(12,2);
                                 if ( len > 14 )
                                 {
				                        pGenPage->m_StrCode8Fld = strCode.Mid(14,2);
                                    if ( len > 16 )
                                    {
				                           pGenPage->m_StrCode9Fld = strCode.Mid(16,2);
                                       if ( len > 18 )
				                              pGenPage->m_StrCode10Fld = strCode.Mid(18,2);
                                    }
                                 }
                              }
                           }
                        }
                     }
                  }
               }
            }
				if ( strcmp ((char*)sCCF02Record.cash_bonus_eligible, "Y" ) == 0 )
					pGenPage->m_BoolCashBonus = TRUE;
				else
					pGenPage->m_BoolCashBonus = FALSE;

				if ( strcmp ((char*)sCCF02Record.visa_purchase_card, "Y" ) == 0 )
					pGenPage->m_BoolVisaPurchase = TRUE;
				else
					pGenPage->m_BoolVisaPurchase = FALSE;


				pBalPage->m_strType1.Format((LPCTSTR)sCCF02Record.related_acct[0].type);
				pBalPage->m_strType2.Format((LPCTSTR)sCCF02Record.related_acct[1].type);
				pBalPage->m_strType3.Format((LPCTSTR)sCCF02Record.related_acct[2].type);
				pBalPage->m_strType4.Format((LPCTSTR)sCCF02Record.related_acct[3].type);
				pBalPage->m_strType5.Format((LPCTSTR)sCCF02Record.related_acct[4].type);
				pBalPage->m_strType6.Format((LPCTSTR)sCCF02Record.related_acct[5].type);
				pBalPage->m_strType7.Format((LPCTSTR)sCCF02Record.related_acct[6].type);
				pBalPage->m_strType8.Format((LPCTSTR)sCCF02Record.related_acct[7].type);
				pBalPage->m_strType9.Format((LPCTSTR)sCCF02Record.related_acct[9].type);
				pBalPage->m_strType10.Format((LPCTSTR)sCCF02Record.related_acct[9].type);
					
				pBalPage->m_strNbr1.Format((LPCTSTR)sCCF02Record.related_acct[0].nbr);
				pBalPage->m_strNbr2.Format((LPCTSTR)sCCF02Record.related_acct[1].nbr);
				pBalPage->m_strNbr3.Format((LPCTSTR)sCCF02Record.related_acct[2].nbr);
				pBalPage->m_strNbr4.Format((LPCTSTR)sCCF02Record.related_acct[3].nbr);
				pBalPage->m_strNbr5.Format((LPCTSTR)sCCF02Record.related_acct[4].nbr);
				pBalPage->m_strNbr6.Format((LPCTSTR)sCCF02Record.related_acct[5].nbr);
				pBalPage->m_strNbr7.Format((LPCTSTR)sCCF02Record.related_acct[6].nbr);
				pBalPage->m_strNbr8.Format((LPCTSTR)sCCF02Record.related_acct[7].nbr);
				pBalPage->m_strNbr9.Format((LPCTSTR)sCCF02Record.related_acct[8].nbr);
				pBalPage->m_strNbr10.Format((LPCTSTR)sCCF02Record.related_acct[9].nbr);

				if (m_strCardType == "Credit")
				{
					pCapPage->m_CrPurchase = (BOOL)sCCF02Record.allowed[0] - 48;
					pCapPage->m_CrCashAdv = (BOOL)sCCF02Record.allowed[1] - 48;
					pCapPage->m_CrReturn = (BOOL)sCCF02Record.allowed[2] - 48;
					pCapPage->m_CrInquiry = (BOOL)sCCF02Record.allowed[3] - 48;
					pCapPage->m_CrPayment = (BOOL)sCCF02Record.allowed[4] - 48;
				}

				if (m_strCardType == "Fleet")
				{
					pCapPage->m_FlPurchase = sCCF02Record.allowed[0] - 48;
					pCapPage->m_FlCashAdv = sCCF02Record.allowed[1] - 48;
					pCapPage->m_FlReturn = sCCF02Record.allowed[2] - 48;
					pCapPage->m_FlInquiry = sCCF02Record.allowed[3] - 48;
					pCapPage->m_FlPayment = sCCF02Record.allowed[4] - 48;
				}

				if (m_strCardType == "Debit")
				{
					pCapPage->m_DebPurchase = (BOOL)sCCF02Record.allowed[0] - 48;
					pCapPage->m_DebCashWithd = (BOOL)sCCF02Record.allowed[1] - 48;
					pCapPage->m_DebReturn = (BOOL)sCCF02Record.allowed[2] - 48;
					pCapPage->m_DebInquiry = (BOOL)sCCF02Record.allowed[3] - 48;
					pCapPage->m_DebDeposit = (BOOL)sCCF02Record.allowed[4] - 48;
					pCapPage->m_DebBalTransf = (BOOL)sCCF02Record.allowed[5] - 48;
				}

				if (m_strCardType == "Loyalty")
				{
					pCapPage->m_LoyPurchase = (BOOL)sCCF02Record.allowed[0] - 48;
					pCapPage->m_LoyReturn = (BOOL)sCCF02Record.allowed[1] - 48;
					pCapPage->m_LoyInquiry = (BOOL)sCCF02Record.allowed[2] - 48;
					pCapPage->m_LoyLoad = (BOOL)sCCF02Record.allowed[3] - 48;
					pCapPage->m_LoyUnload = (BOOL)sCCF02Record.allowed[4] - 48;
				}

				ZeroMemory(&sCCF01Record,sizeof(CCF01)) ;
				memcpy ((LPSTR)(sCCF01Record.primary_key.card_type), m_strCardType.Left(1) , sizeof (sCCF01Record.primary_key.card_type));
				memcpy ((LPSTR)(sCCF01Record.primary_key.card_nbr), m_strCardNbr , sizeof (sCCF01Record.primary_key.card_nbr));

				int rcode1 = txdsapi_get_record((pBYTE)&sCCF01Record, sizeof(CCF01),CCF01_DATA, ErrorMsg);
				if ( rcode1 == PTEMSG_OK )
				{
					ZeroMemory(&sOldCCF01Record, sizeof(CCF01));
					memcpy(&sOldCCF01Record, &sCCF01Record, sizeof(CCF01));
					pDemogrPage->m_bCCF01 = TRUE;


					pDemogrPage->m_strName.Format((LPCTSTR)sCCF01Record.name_dba);
					pDemogrPage->m_strAddr1.Format((LPCTSTR)sCCF01Record.address1);
					pDemogrPage->m_strAddr2.Format((LPCTSTR)sCCF01Record.address2);
					pDemogrPage->m_strCity.Format((LPCTSTR)sCCF01Record.city);
					pDemogrPage->m_strState.Format((LPCTSTR)sCCF01Record.state);
					pDemogrPage->m_strZip.Format((LPCTSTR)sCCF01Record.zip_code);
					pDemogrPage->m_strHomePhone.Format((LPCTSTR)sCCF01Record.home_phone);
					pDemogrPage->m_strWorkPhone.Format((LPCTSTR)sCCF01Record.work_phone);
					pDemogrPage->m_strDOB.Format((LPCTSTR)sCCF01Record.dob);
					pDemogrPage->m_strProfession.Format((LPCTSTR)sCCF01Record.profession);
					pDemogrPage->m_strCountry.Format((LPCTSTR)sCCF01Record.country);
					pDemogrPage->m_StrBlockStatusFld.Format((LPCTSTR)sCCF01Record.block_status);
					pDemogrPage->m_StrCardholderIDFld.Format((LPCTSTR)sCCF01Record.cardholder_id);
					pDemogrPage->m_StrMaidenNameFld.Format((LPCTSTR)sCCF01Record.mother_maiden_name);
					pDemogrPage->m_StrMembershipIDFld.Format((LPCTSTR)sCCF01Record.membership_id);
					pDemogrPage->m_StrSpecialtyCodeFld.Format((LPCTSTR)sCCF01Record.specialty_code);
					pDemogrPage->m_StrVIPAccountCodeFld.Format((LPCTSTR)sCCF01Record.vip_acct_code);
				}
				else if(rcode1 == PTEMSG_NOT_FOUND )    
				{
					pDemogrPage->m_bCCF01 = FALSE;
				}


				ZeroMemory(&sCCF03Record,sizeof(CCF03)) ;
				memcpy ((LPSTR)(sCCF03Record.primary_key.card_type), m_strCardType.Left(1) , sizeof (sCCF03Record.primary_key.card_type));
				memcpy ((LPSTR)(sCCF03Record.primary_key.card_nbr), m_strCardNbr , sizeof (sCCF03Record.primary_key.card_nbr));

				int rcode3 = txdsapi_get_record((pBYTE)&sCCF03Record, sizeof(CCF03),CCF03P_DATA, ErrorMsg);
				if ( rcode3 == PTEMSG_OK )
				{
					memcpy ( &sOldCCF03Record, &sCCF03Record, sizeof ( CCF03 ) );
                    // enable number pin retry count
					pGenPage->bFoundCCF03 = TRUE;	


					pMerchUsPage->m_bCCF03 = TRUE;
					char yyyymmdd[9]="";
					char yyyymmdd2[9]="";
					int rcode4;
					int rcode5;
					CString mmddyyyy;
					LONG julian = atol((LPCTSTR)sCCF03Record.peso_julian_date);
					LONG julian2 = atol((LPCTSTR)sCCF03Record.dollar_julian_date);
					if (julian != 0)
						Txutils_Calculate_Gregorian_Date(julian, yyyymmdd);
					if (julian2 != 0)
						Txutils_Calculate_Gregorian_Date(julian2, yyyymmdd2);
					// Reformat date to mmddyyyy
					mmddyyyy = yyyymmdd;
               if ( mmddyyyy.GetLength() != 0 )
					   mmddyyyy = mmddyyyy.Right(2) + mmddyyyy.Mid(4,2) + mmddyyyy.Left(4);					
					pActivPage->m_strDate.Format(mmddyyyy);

					mmddyyyy = yyyymmdd2;
               if ( mmddyyyy.GetLength() != 0 )
					   mmddyyyy = mmddyyyy.Right(2) + mmddyyyy.Mid(4,2) + mmddyyyy.Left(4);					
					pActivPage->m_strDate2.Format(mmddyyyy);
										
					ZeroMemory(&sCAF01Record,sizeof(CAF01)) ;
					memcpy ((LPSTR)(sCAF01Record.primary_key.type), m_strCardType.Left(1) , sizeof (sCAF01Record.primary_key.type));
					pGenPage->m_StrPinRetryFld = sCCF03Record.failed_pin_attempts;

				// if caf_profile_id field is populated - get data
					if ( strcmp ((char*)sCCF02Record.caf_profile_id, "") != 0 )
					{
						memcpy ((LPSTR)(sCAF01Record.primary_key.caf_profile_id), (LPSTR)(sCCF02Record.caf_profile_id) , sizeof (sCAF01Record.primary_key.caf_profile_id));
						rcode4 = txdsapi_get_record((pBYTE)&sCAF01Record, sizeof(CAF01),CAF01_DATA, ErrorMsg);
						if ( rcode4 != PTEMSG_OK && rcode4 != PTEMSG_NOT_FOUND )
						{
							m_strComEdit = ErrorMsg;
							UpdateData(FALSE);
							return FALSE;
						} // if
					} // if
					// if caf_profile_id is empty for ccf02p get it from paf01
					else if ( strcmp ( (char*)sCCF02Record.primary_acct_nbr, "" ) != 0 )
					{
						PAF01 sPAFRecord;
						memcpy ((LPSTR)(sPAFRecord.primary_key.primary_acct_nbr), (LPSTR)(sCCF02Record.primary_acct_nbr) , sizeof (sPAFRecord.primary_key.primary_acct_nbr));
						rcode5 = txdsapi_get_record((pBYTE)&sPAFRecord, sizeof(PAF01),PAF01_DATA, ErrorMsg);
						if ( rcode5 == PTEMSG_OK )
						{
							if ( strcmp ((char*)sPAFRecord.caf_profile_id, "") != 0 )
							{
								memcpy ((LPSTR)(sCAF01Record.primary_key.caf_profile_id), (LPSTR)(sPAFRecord.caf_profile_id) , sizeof (sCAF01Record.primary_key.caf_profile_id));
								rcode4 = txdsapi_get_record((pBYTE)&sCAF01Record, sizeof(CAF01),CAF01_DATA, ErrorMsg);
								if ( rcode4 != PTEMSG_OK && rcode4 != PTEMSG_NOT_FOUND )
								{
									m_strComEdit = ErrorMsg;
									UpdateData(FALSE);
									return FALSE;
								} // if
							}
							else    
							{
								BIN01 sBIN01Record;
								char cardNbr[22];
								memset ( cardNbr, 0, sizeof ( cardNbr ) );
								memcpy ( cardNbr, (char*)sCCF02Record.primary_key.card_nbr, 20 );
								memcpy ( cardNbr+20, sCCF02Record.primary_key.card_type, 1 );
								rcode5 = txdsapi_find_bin(cardNbr, &sBIN01Record,BIN01_DATA, ErrorMsg);
								if ( rcode5 == PTEMSG_OK )
								{
									memcpy ((LPSTR)(sCAF01Record.primary_key.caf_profile_id), (LPSTR)(sBIN01Record.caf_profile_id) , sizeof (sCAF01Record.primary_key.caf_profile_id));
									rcode4 = txdsapi_get_record((pBYTE)&sCAF01Record, sizeof(CAF01),CAF01_DATA, ErrorMsg);
									if ( rcode4 != PTEMSG_OK  )
									{
										m_strComEdit = ErrorMsg;
										UpdateData(FALSE);
										return FALSE;
									} // if  
									
								} // if

							}// else 
   

						}// if
					} // else if

					if ( rcode4 == PTEMSG_OK )
					{
						// Populate General Page

						pGenPage->m_StrPinRetryFld = sCCF03Record.failed_pin_attempts;
						//Populate Activity Page
						int nbr[3], nbrDol[3];
						double amt[3], amtDol[3];
						CString sDays[3];

						for (int y = 0; y < 3; y++)
						{
							nbr[y] = 0;
							amt[y] = 0.0;

							nbrDol[y] = 0;
							amtDol[y] = 0.0;


							int iDays = atoi((char *)&sCAF01Record.velocity[y].day_period);
							if (iDays > 0)
							{
								if (iDays == 1)
									sDays[y] = " day Period";
								else
									sDays[y] = " days Period";

								for (int i = 0; i < iDays; i++)
								{
									nbr[y] = nbr[y] + atoi((char *)&sCCF03Record.peso_usage[i].nbr_usage);
									amt[y] = amt[y] + atof((char *)&sCCF03Record.peso_usage[i].amt_usage);

									nbrDol[y] = nbrDol[y] + atoi((char *)&sCCF03Record.dollar_usage[i].nbr_usage);
									amtDol[y] = amtDol[y] + atof((char *)&sCCF03Record.dollar_usage[i].amt_usage);
								}

							}
							else
								sDays[y] = "0 days Period";
						}

						char buffer[20];

						itoa(nbr[0],buffer,10);
						pActivPage->m_strP1Nbr.Format((LPCTSTR)buffer);
						itoa(nbr[1],buffer,10);
						pActivPage->m_strP2Nbr.Format((LPCTSTR)buffer);
						itoa(nbr[2],buffer,10);
						pActivPage->m_strP3Nbr.Format((LPCTSTR)buffer);
						
						gcvt(amt[0]/100,10,buffer);
						pActivPage->m_strP1Amt.Format((LPCTSTR)buffer);
						gcvt(amt[1]/100,10,buffer);
						pActivPage->m_strP2Amt.Format((LPCTSTR)buffer);
						gcvt(amt[2]/100,10,buffer);
						pActivPage->m_strP3Amt.Format((LPCTSTR)buffer);

						itoa(nbrDol[0],buffer,10);
						pActivPage->m_strP1NbrDol.Format((LPCTSTR)buffer);
						itoa(nbrDol[1],buffer,10);
						pActivPage->m_strP2NbrDol.Format((LPCTSTR)buffer);
						itoa(nbrDol[2],buffer,10);
						pActivPage->m_strP3NbrDol.Format((LPCTSTR)buffer);
						
						gcvt(amtDol[0]/100,10,buffer);
						pActivPage->m_strP1AmtDol.Format((LPCTSTR)buffer);
						gcvt(amtDol[1]/100,10,buffer);
						pActivPage->m_strP2AmtDol.Format((LPCTSTR)buffer);
						gcvt(amtDol[2]/100,10,buffer);
						pActivPage->m_strP3AmtDol.Format((LPCTSTR)buffer);

						pActivPage->m_strP1Lbl.Format((LPCTSTR)sCAF01Record.velocity[0].day_period);
						pActivPage->m_strP1Lbl += sDays[0]; 
						pActivPage->m_strP2Lbl.Format((LPCTSTR)sCAF01Record.velocity[1].day_period);
						pActivPage->m_strP2Lbl += sDays[1];
						pActivPage->m_strP3Lbl.Format((LPCTSTR)sCAF01Record.velocity[2].day_period);
						pActivPage->m_strP3Lbl += sDays[2];
					}

				}
				else if(rcode3 == PTEMSG_NOT_FOUND )    
				{
					pMerchUsPage->m_bCCF03 = FALSE;
					ZeroMemory ( &sOldCCF03Record, sizeof ( CCF03 ) );
					ZeroMemory ( &sCCF03Record, sizeof ( CCF03 ) );

				/*	lstrcpy ((LPTSTR) sCCF03Record.primary_key.card_type,(LPTSTR)sCCF02Record.primary_key.card_type );
					lstrcpy ((LPTSTR) sCCF03Record.primary_key.card_nbr, (LPTSTR)sCCF02Record.primary_key.card_nbr );
					lstrcpy ((LPTSTR) sCCF03Record.status,               (LPTSTR)sCCF02Record.status );           

					memcpy ( &sOldCCF03Record, &sCCF03Record, sizeof ( CCF03 ) );


					rcode3 = txdsapi_insert_record((pBYTE)&sCCF03Record,sizeof(CCF03),CCF03P_DATA, ErrorMsg);
					if(rcode3 != PTEMSG_OK )    
					 {
						m_ComEdit.SetWindowText(ErrorMsg);
						return FALSE;
					 }
				*/
					pGenPage->bFoundCCF03 = FALSE;	
				}



				retvalue = TRUE;
				break;
			}
		case MODE_DELETE:
			retvalue = TRUE;
			break;
		default:
			break;
		}
	}
	else
	{
		if (DlgMode != MODE_ADD)
		{
			m_ComEdit.SetWindowText("ERROR: Card Record does not exist");
			retvalue = FALSE;
		}
	}



	return retvalue;
}


void CCFMain::OnAdd() 
{
	pGenPage->bFoundCCF03 = FALSE;
	if (!GetCCFData(MODE_ADD))
		return;

   m_strComEdit = "";
   UpdateData(FALSE);

	SetModes(MODE_ADD);
	SetKeys();
	m_pSheet->SetActivePage(1);
	m_pSheet->SetWindowText("Add Card Record");
	//set version info in the title bar
	CString appProdVer;
	CString lpszTitle;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	m_pSheet->GetWindowText(lpszTitle);
	lpszTitle = lpszTitle + " - Version - ";
	lpszTitle += appProdVer;
	m_pSheet->SetWindowText(lpszTitle);

}	

void CCFMain::OnDelete() 
{
	CString strMessage;
	CCF01	sCCF01Record;
	CCF02P	sCCF02Record;
	CCF03	sCCF03Record;
    int nReturnCode;
	char strErrorMessage[500];

	if (!GetCCFData(MODE_DELETE))
		return;

	strMessage.Format ("Are you sure you want to delete card %s ?", (LPCTSTR) m_strCardNbr) ;
	nReturnCode = AfxMessageBox ((LPCTSTR)strMessage, MB_OKCANCEL|MB_ICONQUESTION) ;
	if (nReturnCode != IDOK) return ;
	
	memcpy ((LPTSTR)sCCF02Record.primary_key.card_type, m_strCardType.Left(1), sizeof(sCCF02Record.primary_key.card_type)) ;
	memcpy ((LPTSTR)sCCF02Record.primary_key.card_nbr, m_strCardNbr, sizeof (sCCF02Record.primary_key.card_nbr)) ;
	
	memcpy ((LPTSTR)sCCF01Record.primary_key.card_type, m_strCardType.Left(1), sizeof(sCCF01Record.primary_key.card_type)) ;
	memcpy ((LPTSTR)sCCF01Record.primary_key.card_nbr, m_strCardNbr, sizeof(sCCF01Record.primary_key.card_nbr)) ;

	memcpy ((LPTSTR)sCCF03Record.primary_key.card_type, m_strCardType.Left(1), sizeof(sCCF03Record.primary_key.card_type)) ;
	memcpy ((LPTSTR)sCCF03Record.primary_key.card_nbr, m_strCardNbr, sizeof(sCCF03Record.primary_key.card_nbr)) ;


    int rcode1 = txdsapi_delete_record ((pBYTE)&sCCF01Record, sizeof (CCF01), CCF01_DATA, strErrorMessage);
    if(rcode1 != PTEMSG_OK  )    
     {
		m_ComEdit.SetWindowText(strErrorMessage);
		return ;
	 }
    int rcode2 = txdsapi_delete_record ((pBYTE)&sCCF02Record, sizeof (CCF02P), CCF02P_DATA, strErrorMessage);
    if( rcode2 != PTEMSG_OK  )    
     {
		m_ComEdit.SetWindowText(strErrorMessage);
		return ;
	 }
    int rcode3 = txdsapi_delete_record ((pBYTE)&sCCF03Record, sizeof (CCF03), CCF03P_DATA, strErrorMessage);
    if( rcode3 != PTEMSG_OK && rcode3 != PTEMSG_NOT_FOUND )    
     {
		m_ComEdit.SetWindowText(strErrorMessage);
		return ;
	 }

	else
     {

 		// Add record to the change history table
		memcpy ((LPSTR)m_strCurrentData, "Record Deleted", sizeof (m_strCurrentData));
        PopulateHistoryTable();

		if ( pChHistPage->m_bChangeHistory )
			pChHistPage->CleanChangeHistory();

		m_ComEdit.SetWindowText("Record Deleted");
     }

	SetModes(MODE_DONE);
	CleanPages();

	
}

void CCFMain::PopulateHistoryTable()
{
    
    HST01 sHistoryData;
    CHAR dest_str[18];   
	CMyUtils myUtilsObject;
    char strErrorMsg[200] = "";
    char strDate[9] = "";
    char strTime[7] = "";
    CString temp;
    
    memset ( strDate, 0, sizeof (strDate));
    memset ( strTime, 0, sizeof (strTime));
    
    myUtilsObject.GetSystemDate(strDate);  

    myUtilsObject.GetSystemTime(strTime);
    pintime_get_gmt ( dest_str );
	CString Keys = m_strCardType.Left(1) + m_strCardNbr;
    ZeroMemory ( &sHistoryData, sizeof (HST01));
    memcpy ((LPTSTR) sHistoryData.file_name,     (LPCTSTR)"CCF02P", sizeof (sHistoryData.file_name));
    temp = strDate;
    if ( temp.GetLength() != 0 )
      temp = temp.Right(4) + temp.Left(2) + temp.Mid(2,2);
    
    CHAR wstation[21]; DWORD wsize;

    wsize = sizeof( wstation );
    GetComputerName( wstation, &wsize );

	memcpy ((LPTSTR)sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
    memcpy ((LPTSTR) sHistoryData.date_modified, temp, sizeof (sHistoryData.date_modified));
    memcpy ((LPTSTR) sHistoryData.file_primary_key,  (LPCTSTR)Keys, sizeof (sHistoryData.file_primary_key));
    memcpy ((LPTSTR) sHistoryData.primary_key.unique_id,     (LPCTSTR)dest_str, sizeof (sHistoryData.primary_key.unique_id));        
    memcpy ((LPTSTR) sHistoryData.time_modified,     (LPCTSTR)strTime, sizeof (sHistoryData.time_modified)); 
             
    memcpy ((LPTSTR)sHistoryData.current_data, (LPCTSTR)m_strCurrentData, sizeof (sHistoryData.current_data));
	memcpy ((LPTSTR)sHistoryData.changed_by, (LPCTSTR)ucf01.primary_key.name, sizeof (sHistoryData.changed_by));

	if (DialogMode == MODE_EDIT)
	{
		memcpy ((LPTSTR)sHistoryData.previous_data, (LPCTSTR)m_strPreviousData, sizeof (sHistoryData.previous_data));
		memcpy ((LPTSTR)sHistoryData.field_tag, (LPCTSTR)m_strFieldTag, sizeof (sHistoryData.field_tag));  
	}

    int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
	if ( rcode != PTEMSG_OK )
		m_ComEdit.SetWindowText(strErrorMsg);    
}


void CCFMain::OnView() 
{
	m_pSheet->SetActivePage(1);  
	m_pSheet->SetActivePage(0);  

	if (!GetCCFData(MODE_VIEW))
		return;

	SetKeys();
	if (pTxPage->m_bTxPage)
		pTxPage->PopulateTxDialog();

	if (pMerchUsPage->m_bMerchUsage && pMerchUsPage->m_bCCF03)
		pMerchUsPage->PopulateMerchUsDialog();

	PopulateHistPage();

	SetModes(MODE_VIEW);
	m_pSheet->SetActivePage(1);
	m_pSheet->SetWindowText("View Card Record");
	//set version info in the title bar
	CString appProdVer;
	CString lpszTitle;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	m_pSheet->GetWindowText(lpszTitle);
	lpszTitle = lpszTitle + " - Version - ";
	lpszTitle += appProdVer;
	m_pSheet->SetWindowText(lpszTitle);

	
}

void CCFMain::PopulateHistPage()
{
	HST01 sHistoryRecord;

	ZeroMemory(&sHistoryRecord, sizeof(sHistoryRecord));
	CString Keys = m_strCardType.Left(1) + m_strCardNbr;

    memcpy ((LPTSTR) sHistoryRecord.primary_key.unique_id, (LPCTSTR)"   ", sizeof(sHistoryRecord.primary_key.unique_id));
    memcpy ((LPTSTR) sHistoryRecord.file_name,     (LPCTSTR)"CCF02P", sizeof (sHistoryRecord.file_name));
    memcpy ((LPTSTR) sHistoryRecord.file_primary_key,  (LPCTSTR)Keys, sizeof (sHistoryRecord.file_primary_key));

	memcpy (&(pChHistPage->sHistoryRecord), &sHistoryRecord,sizeof(sHistoryRecord));

	if (pChHistPage->m_bChangeHistory)
		pChHistPage->PopulateHistoryDialog();

}

void CCFMain::OnUpdate() 
{
	m_pSheet->SetActivePage(1);  
	m_pSheet->SetActivePage(0);  
	if (!GetCCFData(MODE_EDIT))
		return;
	
	SetKeys();
	if (pTxPage->m_bTxPage)
		pTxPage->PopulateTxDialog();

	if (pMerchUsPage->m_bMerchUsage && pMerchUsPage->m_bCCF03)
		pMerchUsPage->PopulateMerchUsDialog();

	PopulateHistPage();
   m_strComEdit = "";
   UpdateData(FALSE);

	SetModes(MODE_EDIT);
	m_pSheet->SetActivePage(1);
	m_pSheet->SetWindowText("Update Card Record");
	//set version info in the title bar
	CString appProdVer;
	CString lpszTitle;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	m_pSheet->GetWindowText(lpszTitle);
	lpszTitle = lpszTitle + " - Version - ";
	lpszTitle += appProdVer;
	m_pSheet->SetWindowText(lpszTitle);
}

void CCFMain::PopulateCCF(	pCCF02P psCCF02Record, pCCF01 psCCF01Record )
{
	CMyUtils myUtilsObject;
	CString strCode;
	char strMessage[250];

	pGenPage->UpdateData(TRUE);
	pDemogrPage->UpdateData(TRUE);
	pCapPage->UpdateData(TRUE);

	memset(psCCF02Record, 0, sizeof(CCF02P)) ;
	memset(psCCF01Record, 0, sizeof(CCF01)) ;
	memset( &sCCF03Record, 0, sizeof (CCF03) );

	// Populate CCF03 table
	lstrcpy ((LPTSTR) sCCF03Record.primary_key.card_type,m_strCardType.Left(1));
	lstrcpy ((LPTSTR) sCCF03Record.primary_key.card_nbr,m_strCardNbr);
	lstrcpy ((LPTSTR) sCCF03Record.status,            pGenPage->m_strCardStatus);

	if ( DialogMode == MODE_EDIT )
		memcpy ( &sCCF03Record, &sOldCCF03Record, sizeof ( CCF03 ) );

	lstrcpy ((LPTSTR) sCCF03Record.failed_pin_attempts,pGenPage->m_StrPinRetryFld);

	lstrcpy ((LPTSTR) psCCF02Record->primary_key.card_type,m_strCardType.Left(1));
	lstrcpy ((LPTSTR) psCCF02Record->primary_key.card_nbr,m_strCardNbr);

	if ( pGenPage->m_CashBonus.GetCheck() == 1 )
		lstrcpy ((LPTSTR) psCCF02Record->cash_bonus_eligible,"Y");
	else
		lstrcpy ((LPTSTR) psCCF02Record->cash_bonus_eligible,"N");

	if ( pGenPage->m_VisaPurchase.GetCheck() == 1 )
	{
		lstrcpy ((LPTSTR) psCCF02Record->visa_purchase_card,"Y");
   	    pGenPage->m_strPesoTranLimit = myUtilsObject.GenerateTranAmount(pGenPage->m_strPesoTranLimit, 12, strMessage);
	    lstrcpy ((LPTSTR) psCCF02Record->peso_tran_limit,pGenPage->m_strPesoTranLimit);
	    pGenPage->m_strDollarTranLimit = myUtilsObject.GenerateTranAmount(pGenPage->m_strDollarTranLimit, 12, strMessage);
        lstrcpy ((LPTSTR) psCCF02Record->dollar_tran_limit,pGenPage->m_strDollarTranLimit);
	}
	else
	{
		lstrcpy ((LPTSTR) psCCF02Record->visa_purchase_card,"N");
		lstrcpy ((LPTSTR) psCCF02Record->peso_tran_limit, "");
		lstrcpy ((LPTSTR) psCCF02Record->dollar_tran_limit, "");
	}

	
   if ( pGenPage->m_StrCode1Fld.GetLength() > 0 && pGenPage->m_StrCode1Fld.GetLength() < 2 )
      pGenPage->m_StrCode1Fld = " " + pGenPage->m_StrCode1Fld;

   if ( pGenPage->m_StrCode2Fld.GetLength() > 0 && pGenPage->m_StrCode2Fld.GetLength() < 2 )
      pGenPage->m_StrCode2Fld = " " + pGenPage->m_StrCode2Fld;

   if ( pGenPage->m_StrCode3Fld.GetLength() > 0 && pGenPage->m_StrCode3Fld.GetLength() < 2 )
      pGenPage->m_StrCode3Fld = " " + pGenPage->m_StrCode3Fld;

   if ( pGenPage->m_StrCode4Fld.GetLength() > 0 && pGenPage->m_StrCode4Fld.GetLength() < 2 )
      pGenPage->m_StrCode4Fld = " " + pGenPage->m_StrCode4Fld;

   if ( pGenPage->m_StrCode5Fld.GetLength() > 0 && pGenPage->m_StrCode5Fld.GetLength() < 2 )
      pGenPage->m_StrCode5Fld = " " + pGenPage->m_StrCode5Fld;

   if ( pGenPage->m_StrCode6Fld.GetLength() > 0 && pGenPage->m_StrCode6Fld.GetLength() < 2 )
      pGenPage->m_StrCode6Fld = " " + pGenPage->m_StrCode6Fld;

   if ( pGenPage->m_StrCode7Fld.GetLength() > 0 && pGenPage->m_StrCode7Fld.GetLength() < 2 )
      pGenPage->m_StrCode7Fld = " " + pGenPage->m_StrCode7Fld;

   if ( pGenPage->m_StrCode8Fld.GetLength() > 0 && pGenPage->m_StrCode8Fld.GetLength() < 2 )
      pGenPage->m_StrCode8Fld = " " + pGenPage->m_StrCode8Fld;

   if ( pGenPage->m_StrCode9Fld.GetLength() > 0 && pGenPage->m_StrCode9Fld.GetLength() < 2 )
      pGenPage->m_StrCode9Fld = " " + pGenPage->m_StrCode9Fld;

   if ( pGenPage->m_StrCode10Fld.GetLength() > 0 && pGenPage->m_StrCode10Fld.GetLength() < 2 )
      pGenPage->m_StrCode10Fld = " " + pGenPage->m_StrCode10Fld;

	strCode = pGenPage->m_StrCode1Fld + pGenPage->m_StrCode2Fld + pGenPage->m_StrCode3Fld +
		      pGenPage->m_StrCode4Fld + pGenPage->m_StrCode5Fld + pGenPage->m_StrCode6Fld +
			  pGenPage->m_StrCode7Fld + pGenPage->m_StrCode8Fld + pGenPage->m_StrCode9Fld +
			  pGenPage->m_StrCode10Fld ;


	lstrcpy ((LPTSTR) psCCF02Record->product_codes,strCode);

	lstrcpy ((LPTSTR) psCCF02Record->primary_acct_nbr,pGenPage->m_strPAN);
	lstrcpy ((LPTSTR) psCCF02Record->caf_profile_id,pGenPage->m_strVipProf);
	lstrcpy ((LPTSTR) psCCF02Record->status,pGenPage->m_strCardStatus);
	lstrcpy ((LPTSTR) psCCF02Record->pin_offset,pGenPage->m_strPinOffset);
	lstrcpy ((LPTSTR) psCCF02Record->expire_date,pGenPage->m_strExpDate);
	lstrcpy ((LPTSTR) psCCF02Record->maint_date,pGenPage->m_strSecExpDate);
	CString temp;
   
   if ( pGenPage->m_strEffDate.GetLength() != 0 )
	temp = pGenPage->m_strEffDate.Right(4) + pGenPage->m_strEffDate.Mid(2,2)
										   + pGenPage->m_strEffDate.Left(2);
	if ( temp.GetLength() != 0 )
		lstrcpy ((LPTSTR) psCCF02Record->effect_date,temp);	
//	pGenPage->m_strCreditLine = myUtilsObject.GenerateAmount(pGenPage->m_strCreditLine, 12); removed for EB
//	lstrcpy ((LPTSTR) psCCF02Record->credit_line,pGenPage->m_strCreditLine); removed for EB
	lstrcpy ((LPTSTR) psCCF02Record->outstanding_auths,pGenPage->m_strOutstAuth);
	lstrcpy ((LPTSTR) psCCF02Record->currency_code,pGenPage->m_strCurCode);
	lstrcpy ((LPTSTR) psCCF02Record->country_code,pGenPage->m_strCountryCode);
	lstrcpy ((LPTSTR) psCCF02Record->percent_deposit,pGenPage->m_StrPanSeqNbr);
    CString strRate = myUtilsObject.GenerateTranAmount ( pGenPage->m_strRsbRate, 5, strMessage );
	lstrcpy ((LPTSTR) psCCF02Record->add_on_rate, strRate);

	lstrcpy ((LPTSTR) psCCF02Record->add_on_term,pGenPage->m_strRsbTerm);

	lstrcpy ((LPTSTR) psCCF02Record->related_acct[0].type,pBalPage->m_strType1.Left(2));
	lstrcpy ((LPTSTR) psCCF02Record->related_acct[0].nbr,pBalPage->m_strNbr1);
	lstrcpy ((LPTSTR) psCCF02Record->related_acct[1].type,pBalPage->m_strType2.Left(2));
	lstrcpy ((LPTSTR) psCCF02Record->related_acct[1].nbr,pBalPage->m_strNbr2);
	lstrcpy ((LPTSTR) psCCF02Record->related_acct[2].type,pBalPage->m_strType3.Left(2));
	lstrcpy ((LPTSTR) psCCF02Record->related_acct[2].nbr,pBalPage->m_strNbr3);
	lstrcpy ((LPTSTR) psCCF02Record->related_acct[3].type,pBalPage->m_strType4.Left(2));
	lstrcpy ((LPTSTR) psCCF02Record->related_acct[3].nbr,pBalPage->m_strNbr4);
	lstrcpy ((LPTSTR) psCCF02Record->related_acct[4].type,pBalPage->m_strType5.Left(2));
	lstrcpy ((LPTSTR) psCCF02Record->related_acct[4].nbr,pBalPage->m_strNbr5);
	lstrcpy ((LPTSTR) psCCF02Record->related_acct[5].type,pBalPage->m_strType6.Left(2));
	lstrcpy ((LPTSTR) psCCF02Record->related_acct[5].nbr,pBalPage->m_strNbr6);
	lstrcpy ((LPTSTR) psCCF02Record->related_acct[6].type,pBalPage->m_strType7.Left(2));
	lstrcpy ((LPTSTR) psCCF02Record->related_acct[6].nbr,pBalPage->m_strNbr7);
	lstrcpy ((LPTSTR) psCCF02Record->related_acct[7].type,pBalPage->m_strType8.Left(2));
	lstrcpy ((LPTSTR) psCCF02Record->related_acct[7].nbr,pBalPage->m_strNbr8);
	lstrcpy ((LPTSTR) psCCF02Record->related_acct[8].type,pBalPage->m_strType9.Left(2));
	lstrcpy ((LPTSTR) psCCF02Record->related_acct[8].nbr,pBalPage->m_strNbr9);
	lstrcpy ((LPTSTR) psCCF02Record->related_acct[9].type,pBalPage->m_strType10.Left(2));
	lstrcpy ((LPTSTR) psCCF02Record->related_acct[9].nbr,pBalPage->m_strNbr10);


	if (m_strCardType == "Credit")
	{
		psCCF02Record->allowed[0] = 48 + pCapPage->m_CrPurchase ;
		psCCF02Record->allowed[1] = 48 + pCapPage->m_CrCashAdv ;
		psCCF02Record->allowed[2] = 48 + pCapPage->m_CrReturn;
		psCCF02Record->allowed[3] = 48 + pCapPage->m_CrInquiry;
		psCCF02Record->allowed[4] = 48 + pCapPage->m_CrPayment;
	}

	if (m_strCardType == "Fleet")
	{
		psCCF02Record->allowed[0] = 48 + pCapPage->m_FlPurchase;
		psCCF02Record->allowed[1] = 48 + pCapPage->m_FlCashAdv;
		psCCF02Record->allowed[2] = 48 + pCapPage->m_FlReturn;
		psCCF02Record->allowed[3] = 48 + pCapPage->m_FlInquiry;
		psCCF02Record->allowed[4] = 48 + pCapPage->m_FlPayment;
	}

	if (m_strCardType == "Debit")
	{
		psCCF02Record->allowed[0] = 48 + pCapPage->m_DebPurchase;
		psCCF02Record->allowed[1] = 48 + pCapPage->m_DebCashWithd;
		psCCF02Record->allowed[2] = 48 + pCapPage->m_DebReturn;
		psCCF02Record->allowed[3] = 48 + pCapPage->m_DebInquiry;
		psCCF02Record->allowed[4] = 48 + pCapPage->m_DebDeposit;
		psCCF02Record->allowed[5] = 48 + pCapPage->m_DebBalTransf;
	}

	if (m_strCardType == "Loyalty")
	{
		psCCF02Record->allowed[0] = 48 + pCapPage->m_LoyPurchase ;
		psCCF02Record->allowed[1] = 48 + pCapPage->m_LoyReturn;
		psCCF02Record->allowed[2] = 48 + pCapPage->m_LoyInquiry;
		psCCF02Record->allowed[3] = 48 + pCapPage->m_LoyLoad ;
		psCCF02Record->allowed[4] = 48 + pCapPage->m_LoyUnload;
	}

	if (pDemogrPage->m_bDemogrChanged)
	{
		lstrcpy ((LPTSTR) psCCF01Record->primary_key.card_type,m_strCardType.Left(1));
		lstrcpy ((LPTSTR) psCCF01Record->primary_key.card_nbr,m_strCardNbr);

		lstrcpy ((LPTSTR) psCCF01Record->primary_acct_nbr,pGenPage->m_strPAN);
		lstrcpy ((LPTSTR) psCCF01Record->name_dba,pDemogrPage->m_strName);
		lstrcpy ((LPTSTR) psCCF01Record->address1,pDemogrPage->m_strAddr1);
		lstrcpy ((LPTSTR) psCCF01Record->address2,pDemogrPage->m_strAddr2);
		lstrcpy ((LPTSTR) psCCF01Record->city,pDemogrPage->m_strCity);
		lstrcpy ((LPTSTR) psCCF01Record->state,pDemogrPage->m_strState);
		lstrcpy ((LPTSTR) psCCF01Record->zip_code,pDemogrPage->m_strZip);
		lstrcpy ((LPTSTR) psCCF01Record->home_phone,pDemogrPage->m_strHomePhone);
		lstrcpy ((LPTSTR) psCCF01Record->work_phone,pDemogrPage->m_strWorkPhone);
		lstrcpy ((LPTSTR) psCCF01Record->dob,pDemogrPage->m_strDOB);
		lstrcpy ((LPTSTR) psCCF01Record->profession,pDemogrPage->m_strProfession);
		lstrcpy ((LPTSTR) psCCF01Record->country,pDemogrPage->m_strCountry);
		lstrcpy ((LPTSTR) psCCF01Record->status,pGenPage->m_strCardStatus);
		lstrcpy ((LPTSTR) psCCF01Record->mother_maiden_name,pDemogrPage->m_StrMaidenNameFld);
		lstrcpy ((LPTSTR) psCCF01Record->membership_id,pDemogrPage->m_StrMembershipIDFld);
		lstrcpy ((LPTSTR) psCCF01Record->cardholder_id,pDemogrPage->m_StrCardholderIDFld);
		lstrcpy ((LPTSTR) psCCF01Record->block_status,pDemogrPage->m_StrBlockStatusFld);
		lstrcpy ((LPTSTR) psCCF01Record->specialty_code,pDemogrPage->m_StrSpecialtyCodeFld);
		lstrcpy ((LPTSTR) psCCF01Record->vip_acct_code,pDemogrPage->m_StrVIPAccountCodeFld);

	}


}


BOOL CCFMain::OnApply() 
{
	char ErrorMsg[200];
	CCF02P	sCCF02Record;
	CCF01	sCCF01Record;
	CAF01	sCAFRecord;
	CSF01   sCSFRecord;
	PAF01	sPAFRecord;
	int   rcode1, rcode2, rcode3;
	CMyUtils myUtilsObject;

//   CString buttonlabel;
//   CWnd*   activebutton;

   /* This snippet of code prevents updating the database
    * and exiting the window when <enter> is pressed. The
    * database will only get updated when APPLY is selected
    * or if focus is on the OK button when <enter> is pressed
    * or OK is selected.
    */
/*   activebutton = GetFocus();
   activebutton->GetWindowText(buttonlabel);
   if ( buttonlabel != "&Apply"  && buttonlabel != "OK")
   {
      return FALSE;
   }
*/
	if (DialogMode == MODE_DONE)
			//return CPropertyPage::OnApply();
      return FALSE;

	m_pSheet->SetActivePage(4);
	m_pSheet->SetActivePage(3);
	m_pSheet->SetActivePage(2);
	m_pSheet->SetActivePage(0);

	pGenPage->UpdateData(TRUE);
	pDemogrPage->UpdateData(TRUE);

	if (!KeyEntered())
		return FALSE;
   /*CString strCode = pGenPage->m_StrCode1Fld + pGenPage->m_StrCode2Fld + pGenPage->m_StrCode3Fld +
		      pGenPage->m_StrCode4Fld + pGenPage->m_StrCode5Fld + pGenPage->m_StrCode6Fld +
			  pGenPage->m_StrCode7Fld + pGenPage->m_StrCode8Fld + pGenPage->m_StrCode9Fld +
			  pGenPage->m_StrCode10Fld ;

   if ( strCode.GetLength() < 20 && strCode.GetLength() > 0 )
	{
		pGenPage->m_strComEdit = "Invalid Product Code";
		m_pSheet->SetActivePage(1);
		pGenPage->m_CntlCode1Fld.SetFocus();
		return FALSE;
	}*/

	/*if (pGenPage->m_strPAN.IsEmpty())
	{
		pGenPage->m_strComEdit = "Please enter Primary Account Number";
		m_pSheet->SetActivePage(1);
		pGenPage->m_ctrlPan.SetFocus();
		return FALSE;
	}

	if (pGenPage->m_strVipProf.IsEmpty())
	{
		pGenPage->m_strComEdit = "Please enter VIP Profile";
		m_pSheet->SetActivePage(1);
		pGenPage->m_ctrlVipProf.SetFocus();
		return FALSE;
	}*/

	if (pGenPage->m_strCardStatus.IsEmpty())
	{
		pGenPage->m_strComEdit = "Please enter Card Status";
		m_pSheet->SetActivePage(1);
		pGenPage->m_ctrlCardStatus.SetFocus();
		return FALSE;
	}

	/*if (pGenPage->m_strExpDate.IsEmpty())
	{
		pGenPage->m_strComEdit = "Please enter Expiration Date";
		m_pSheet->SetActivePage(1);
		pGenPage->m_ctrlExpDate.SetFocus();
		return FALSE;
	}
	else 
	{*/
	int mo = atoi(pGenPage->m_strExpDate.Right(2));
	if (mo > 12)
		{
			pGenPage->m_strComEdit = "ERROR: Invalid Expiration Date";
			m_pSheet->SetActivePage(1);
			pGenPage->m_ctrlExpDate.SetFocus();
			return FALSE;
		}
	//}
	/* TF-Ajay Additional  Expiry date checking Oct-21-2010 */
    int smo = atoi(pGenPage->m_strSecExpDate.Right(2));
	if (smo > 12)
		{
			pGenPage->m_strComEdit = "ERROR: Invalid Expiration Date";
			m_pSheet->SetActivePage(1);
			pGenPage->m_cntlSecExpDate.SetFocus();
			return FALSE;
		}

	/**int product_code_len = pGenPage->m_StrCode1Fld.GetLength();
	if( product_code_len == 1)
	{
		pGenPage->m_strComEdit = "ERROR:Product Code should be of 2 digit";
		m_pSheet->SetActivePage(1);
		pGenPage->m_cntlSecExpDate.SetFocus();
		return FALSE;
	}*/

	/*if (pGenPage->m_strEffDate.IsEmpty())
	{
		pGenPage->m_strComEdit = "Please enter Effective Date";
		m_pSheet->SetActivePage(1);
		pGenPage->m_ctrlEffDate.SetFocus();

	    return TRUE;
	}*/
	else if (!ValidateDate(pGenPage->m_strEffDate, ErrorMsg) )
	{
		pGenPage->m_strComEdit = ErrorMsg;
		m_pSheet->SetActivePage(1);
		pGenPage->m_ctrlEffDate.SetFocus();
		return FALSE;
	}
		
/*	if (pGenPage->m_strCreditLine.IsEmpty())
	{
		pGenPage->m_strComEdit = "Please enter Credit Line";
		m_pSheet->SetActivePage(1);
		pGenPage->m_ctrlCreditLine.SetFocus();
		return FALSE;
	}  removed for EB */

	/*if (pGenPage->m_strCurCode.IsEmpty())
	  {
		pGenPage->m_strComEdit = "Please enter Currency Code";
		m_pSheet->SetActivePage(1);
		pGenPage->m_ctrlCurCode.SetFocus();
		return FALSE;
	  }

	  if (pGenPage->m_strCountryCode.IsEmpty())
 	  {
		pGenPage->m_strComEdit = "Please enter Country Code";
		m_pSheet->SetActivePage(1);
		pGenPage->m_ctrlCountryCode.SetFocus();
		return FALSE;
	  }*/

	//Check if PAN is valid
	ZeroMemory(&sPAFRecord,sizeof(PAF01)) ;
    memcpy ((LPSTR)(sPAFRecord.primary_key.primary_acct_nbr), (LPCTSTR)pGenPage->m_strPAN , sizeof (sPAFRecord.primary_key.primary_acct_nbr));
	int rcode = txdsapi_get_record( (pBYTE)&sPAFRecord, sizeof(PAF01), PAF01_DATA, ErrorMsg);
	if(rcode == PTEMSG_NOT_FOUND )    
	 {
		pGenPage->m_strComEdit = "ERROR: Invalid Primary Account Number";
		m_pSheet->SetActivePage(1);
		pGenPage->m_ctrlPan.SetFocus();
		return FALSE;
	 }
	else if(rcode != PTEMSG_OK )    
		 {
			m_ComEdit.SetWindowText(ErrorMsg);
			return FALSE;
		 }


	//Check if caf_profile_id is valid
	if ( !myUtilsObject.IsEmpty ( pGenPage->m_strVipProf, this ) )
	{
		ZeroMemory(&sCAFRecord,sizeof(CAF01));
		memcpy ((LPTSTR) sCAFRecord.primary_key.caf_profile_id,(LPCTSTR)pGenPage->m_strVipProf, sizeof (sCAFRecord.primary_key.caf_profile_id));
		memcpy ((LPTSTR) sCAFRecord.primary_key.type,(LPCTSTR)m_strCardType.Left(1), sizeof (sCAFRecord.primary_key.type));

		rcode1 = txdsapi_get_record((pBYTE) &sCAFRecord,sizeof(CAF01),CAF01_DATA,ErrorMsg);
		if(rcode1 == PTEMSG_NOT_FOUND )    
		 {
			pGenPage->m_strComEdit = "ERROR: Invalid VIP Profile";
			m_pSheet->SetActivePage(1);
			pGenPage->m_ctrlVipProf.SetFocus();
			return FALSE;
		 }
		else if(rcode1 != PTEMSG_OK )    
			 {
				m_ComEdit.SetWindowText(ErrorMsg);
				return FALSE;
			 }
	}
	//Check if card status code entered is valid
	ZeroMemory(&sCSFRecord,sizeof(CSF01));
    memcpy ((LPTSTR) sCSFRecord.primary_key.status,(LPCTSTR)pGenPage->m_strCardStatus, sizeof (sCSFRecord.primary_key.status));

	rcode1 = txdsapi_get_record((pBYTE) &sCSFRecord,sizeof(CSF01),CSF01_DATA,ErrorMsg);
	if(rcode1 == PTEMSG_NOT_FOUND )    
	 {
		pGenPage->m_strComEdit = "ERROR: Invalid Card Status";
		m_pSheet->SetActivePage(1);
		pGenPage->m_ctrlCardStatus.SetFocus();
		return FALSE;
	 }
	else if(rcode1 != PTEMSG_OK )    
		 {
			m_ComEdit.SetWindowText(ErrorMsg);
			return FALSE;
		 }

	PopulateCCF(&sCCF02Record,&sCCF01Record);

   if ( DialogMode == MODE_EDIT || DialogMode == MODE_VIEW )
      memcpy ( sCCF02Record.pvki, sOldCCF02Record.pvki, sizeof (sCCF02Record.pvki) - 1 );

	switch (DialogMode)
	{
	case MODE_ADD:
		{
			rcode2 = txdsapi_insert_record((pBYTE)&sCCF02Record,sizeof(CCF02P),CCF02P_DATA, ErrorMsg);
			if(rcode2 != PTEMSG_OK )    
			 {
				m_ComEdit.SetWindowText(ErrorMsg);
				return FALSE;
			 }
		/*	rcode3 = txdsapi_insert_record((pBYTE)&sCCF03Record,sizeof(CCF03),CCF03P_DATA, ErrorMsg);
			if(rcode3 != PTEMSG_OK )    
			 {
				m_ComEdit.SetWindowText(ErrorMsg);
				return FALSE;
			 }*/
			if (!pDemogrPage->m_bDemogrChanged)		
			{
				ZeroMemory( &sCCF01Record, sizeof (CCF01) );
				memcpy ((LPSTR)(sCCF01Record.primary_key.card_type), m_strCardType.Left(1) , sizeof (sCCF01Record.primary_key.card_type));
				memcpy ((LPSTR)(sCCF01Record.primary_key.card_nbr), m_strCardNbr , sizeof (sCCF01Record.primary_key.card_nbr));
			}
			rcode1 = txdsapi_insert_record((pBYTE)&sCCF01Record,sizeof(CCF01),CCF01_DATA, ErrorMsg);

			if(rcode1 != PTEMSG_OK)    
			 {
				m_ComEdit.SetWindowText(ErrorMsg);
				return FALSE;
			 }
			else
			{
				// Add record to the change history table
				memcpy ((LPSTR)m_strCurrentData, "Record Added", sizeof (m_strCurrentData));
			    PopulateHistoryTable();

				SetModes(MODE_DONE);
				//m_ComEdit.SetWindowText("Record Added");
				CleanPages();
				m_strComEdit = "Record Added";
				UpdateData(FALSE);
			}

			break;
		}

	case MODE_EDIT:
		{
			if(0 == strlen((LPSTR)sCCF02Record.pin))
			{
				memcpy (sCCF02Record.pin, sOldCCF02Record.pin , (sizeof (sCCF02Record.pin) -1));
			}
			rcode2 = txdsapi_update_record((pBYTE)&sCCF02Record,sizeof(CCF02P),CCF02P_DATA, ErrorMsg);
			if(rcode2 != PTEMSG_OK )    
			 {
				m_ComEdit.SetWindowText(ErrorMsg);
				return FALSE;
			 }
			// Update CCF03 only if failed_pin_attempts in not empty
			if (   pGenPage->bFoundCCF03 )
			{
				rcode3 = txdsapi_update_record((pBYTE)&sCCF03Record,sizeof(CCF03),CCF03P_DATA, ErrorMsg);
				if(rcode3 != PTEMSG_OK )    
				 {
					m_ComEdit.SetWindowText(ErrorMsg);
					return FALSE;
				 }
			}

         // CleanPages();
         // Add record to the change history table
         if (strcmp((char *)sOldCCF02Record.primary_acct_nbr, (char *)sCCF02Record.primary_acct_nbr) !=0)
         {
            memcpy(m_strCurrentData, sCCF02Record.primary_acct_nbr, sizeof(m_strCurrentData));
            memcpy(m_strPreviousData, sOldCCF02Record.primary_acct_nbr, sizeof(m_strPreviousData));
            memcpy(m_strFieldTag, "Primary Acc Nbr", sizeof(m_strFieldTag));

            PopulateHistoryTable();
         }

         if (strcmp((char *)sOldCCF02Record.pin_offset, (char *)sCCF02Record.pin_offset) !=0)
         {
            memcpy(m_strCurrentData, sCCF02Record.pin_offset, sizeof(m_strCurrentData));
            memcpy(m_strPreviousData, sOldCCF02Record.pin_offset, sizeof(m_strPreviousData));
            memcpy(m_strFieldTag, "Pin Offset", sizeof(m_strFieldTag));

            PopulateHistoryTable();
         }

         if (strcmp((char *)sOldCCF03Record.failed_pin_attempts, (char *)sCCF03Record.failed_pin_attempts) !=0)
         {
            memcpy(m_strCurrentData, sCCF03Record.failed_pin_attempts, sizeof(m_strCurrentData));
            memcpy(m_strPreviousData, sOldCCF03Record.failed_pin_attempts, sizeof(m_strPreviousData));
            memcpy(m_strFieldTag, "PIN Retry Count", sizeof(m_strFieldTag));

            PopulateHistoryTable();
         }
         if (strcmp((char *)sOldCCF02Record.caf_profile_id, (char *)sCCF02Record.caf_profile_id) !=0)
         {
            memcpy(m_strCurrentData, sCCF02Record.caf_profile_id, sizeof(m_strCurrentData));
            memcpy(m_strPreviousData, sOldCCF02Record.caf_profile_id, sizeof(m_strPreviousData));
            memcpy(m_strFieldTag, "VIP Profile", sizeof(m_strFieldTag));

            PopulateHistoryTable();
         }

         if (strcmp((char *)sOldCCF02Record.status, (char *)sCCF02Record.status) !=0)
         {
            memcpy(m_strCurrentData, sCCF02Record.status, sizeof(m_strCurrentData));
            memcpy(m_strPreviousData, sOldCCF02Record.status, sizeof(m_strPreviousData));
            memcpy(m_strFieldTag, "Status", sizeof(m_strFieldTag));

            PopulateHistoryTable();
         }

         if (strcmp((char *)sOldCCF02Record.cash_bonus_eligible, (char *)sCCF02Record.cash_bonus_eligible) !=0)
         {
            memcpy(m_strCurrentData, sCCF02Record.cash_bonus_eligible, sizeof(m_strCurrentData));
            memcpy(m_strPreviousData, sOldCCF02Record.cash_bonus_eligible, sizeof(m_strPreviousData));
            memcpy(m_strFieldTag, "Cash Bonus Eligible", sizeof(m_strFieldTag));

            PopulateHistoryTable();
         }
         if (strcmp((char *)sOldCCF02Record.visa_purchase_card, (char *)sCCF02Record.visa_purchase_card) !=0)
         {
            memcpy(m_strCurrentData, sCCF02Record.visa_purchase_card, sizeof(m_strCurrentData));
            memcpy(m_strPreviousData, sOldCCF02Record.visa_purchase_card, sizeof(m_strPreviousData));
            memcpy(m_strFieldTag, "Visa Purchase Card", sizeof(m_strFieldTag));

            PopulateHistoryTable();
         }
         if (strcmp((char *)sOldCCF02Record.product_codes, (char *)sCCF02Record.product_codes) !=0)
         {
            memcpy(m_strCurrentData, sCCF02Record.product_codes, sizeof(m_strCurrentData));
            memcpy(m_strPreviousData, sOldCCF02Record.product_codes, sizeof(m_strPreviousData));
            memcpy(m_strFieldTag, "MP Product Code", sizeof(m_strFieldTag));

            PopulateHistoryTable();
         }
         if (strcmp((char *)sOldCCF02Record.expire_date, (char *)sCCF02Record.expire_date) !=0)
         {
            memcpy(m_strCurrentData, sCCF02Record.expire_date, sizeof(m_strCurrentData));
            memcpy(m_strPreviousData, sOldCCF02Record.expire_date, sizeof(m_strPreviousData));
            memcpy(m_strFieldTag, "Exp.Date", sizeof(m_strFieldTag));

            PopulateHistoryTable();
         }
		 // TF-Ajay Additional expiry date checking Oct-21-2010
		if (strcmp((char *)sOldCCF02Record.maint_date, (char *)sCCF02Record.maint_date) !=0)
         {
            memcpy(m_strCurrentData, sCCF02Record.maint_date, sizeof(m_strCurrentData));
            memcpy(m_strPreviousData, sOldCCF02Record.maint_date, sizeof(m_strPreviousData));
            memcpy(m_strFieldTag, "Sec. Exp.Date", sizeof(m_strFieldTag));

            PopulateHistoryTable();
         }
         if (strcmp((char *)sOldCCF02Record.effect_date, (char *)sCCF02Record.effect_date) !=0)
         {
            memcpy(m_strCurrentData, sCCF02Record.effect_date, sizeof(m_strCurrentData));
            memcpy(m_strPreviousData, sOldCCF02Record.effect_date, sizeof(m_strPreviousData));
            memcpy(m_strFieldTag, "Effect.Date", sizeof(m_strFieldTag));

            PopulateHistoryTable();
         }

         if (strcmp((char *)sOldCCF02Record.credit_line, (char *)sCCF02Record.credit_line) !=0)
         {
            memcpy(m_strCurrentData, sCCF02Record.credit_line, sizeof(m_strCurrentData));
            memcpy(m_strPreviousData, sOldCCF02Record.credit_line, sizeof(m_strPreviousData));
            memcpy(m_strFieldTag, "Credit Line", sizeof(m_strFieldTag));

            PopulateHistoryTable();
         }

         if (strcmp((char *)sOldCCF02Record.allowed, (char *)sCCF02Record.allowed) !=0)
         {
            memcpy(m_strCurrentData, sCCF02Record.allowed, sizeof(m_strCurrentData));
            memcpy(m_strPreviousData, sOldCCF02Record.allowed, sizeof(m_strPreviousData));
            memcpy(m_strFieldTag, "Allowed", sizeof(m_strFieldTag));

            PopulateHistoryTable();
         }

         if (strcmp((char *)sOldCCF02Record.currency_code, (char *)sCCF02Record.currency_code) !=0)
         {
            memcpy(m_strCurrentData, sCCF02Record.currency_code, sizeof(m_strCurrentData));
            memcpy(m_strPreviousData, sOldCCF02Record.currency_code, sizeof(m_strPreviousData));
            memcpy(m_strFieldTag, "Currency Code", sizeof(m_strFieldTag));

            PopulateHistoryTable();
         }

         if (strcmp((char *)sOldCCF02Record.country_code, (char *)sCCF02Record.country_code) !=0)
         {
            memcpy(m_strCurrentData, sCCF02Record.country_code, sizeof(m_strCurrentData));
            memcpy(m_strPreviousData, sOldCCF02Record.country_code, sizeof(m_strPreviousData));
            memcpy(m_strFieldTag, "Country Code", sizeof(m_strFieldTag));

            PopulateHistoryTable();
         }

         if (strcmp((char *)sOldCCF02Record.add_on_rate, (char *)sCCF02Record.add_on_rate) !=0)
         {
            memcpy(m_strCurrentData, sCCF02Record.add_on_rate, sizeof(m_strCurrentData));
            memcpy(m_strPreviousData, sOldCCF02Record.add_on_rate, sizeof(m_strPreviousData));
            memcpy(m_strFieldTag, "RSB Add-On-Rate", sizeof(m_strFieldTag));

            PopulateHistoryTable();
         }

         if (strcmp((char *)sOldCCF02Record.add_on_term, (char *)sCCF02Record.add_on_term) !=0)
         {
            memcpy(m_strCurrentData, sCCF02Record.add_on_term, sizeof(m_strCurrentData));
            memcpy(m_strPreviousData, sOldCCF02Record.add_on_term, sizeof(m_strPreviousData));
            memcpy(m_strFieldTag, "RSB Add-On-Term", sizeof(m_strFieldTag));

            PopulateHistoryTable();
         }

         if (strcmp((char *)sOldCCF02Record.peso_tran_limit, (char *)sCCF02Record.peso_tran_limit) !=0)
         {
            memcpy(m_strCurrentData, sCCF02Record.peso_tran_limit, sizeof(m_strCurrentData));
            memcpy(m_strPreviousData, sOldCCF02Record.peso_tran_limit, sizeof(m_strPreviousData));
            memcpy(m_strFieldTag, "Peso Tran Limit", sizeof(m_strFieldTag));

            PopulateHistoryTable();
         }

         if (strcmp((char *)sOldCCF02Record.dollar_tran_limit, (char *)sCCF02Record.dollar_tran_limit) !=0)
         {
            memcpy(m_strCurrentData, sCCF02Record.dollar_tran_limit, sizeof(m_strCurrentData));
            memcpy(m_strPreviousData, sOldCCF02Record.dollar_tran_limit, sizeof(m_strPreviousData));
            memcpy(m_strFieldTag, "Dollar Tran Limit", sizeof(m_strFieldTag));

            PopulateHistoryTable();
         }




         for (int i = 0; i < 10; i++)
         {
            if (strcmp((char *)sOldCCF02Record.related_acct[i].type, (char *)sCCF02Record.related_acct[i].type) !=0)
            {
               memcpy(m_strCurrentData, sCCF02Record.related_acct[i].type, sizeof(m_strCurrentData));
               memcpy(m_strPreviousData, sOldCCF02Record.related_acct[i].type, sizeof(m_strPreviousData));
               memcpy(m_strFieldTag, "Bal.Acct.Type", sizeof(m_strFieldTag));

               PopulateHistoryTable();
            }

            if (strcmp((char *)sOldCCF02Record.related_acct[i].nbr, (char *)sCCF02Record.related_acct[i].nbr) !=0)
            {
               memcpy(m_strCurrentData, sCCF02Record.related_acct[i].nbr, sizeof(m_strCurrentData));
               memcpy(m_strPreviousData, sOldCCF02Record.related_acct[i].nbr, sizeof(m_strPreviousData));
               memcpy(m_strFieldTag, "Bal.Acct.Nbr", sizeof(m_strFieldTag));

               PopulateHistoryTable();
            }
         }

			if (pDemogrPage->m_bDemogrChanged)
			{
				if (pDemogrPage->m_bCCF01)
				{
					rcode1 = txdsapi_update_record((pBYTE)&sCCF01Record,sizeof(CCF01),CCF01_DATA, ErrorMsg);
					if(rcode1 != PTEMSG_OK )    
					 {
						m_ComEdit.SetWindowText(ErrorMsg);
						return FALSE;
					 }
					else
					{
						// Add record to the change history table
						if (strcmp((char *)sOldCCF01Record.mother_maiden_name, (char *)sCCF01Record.mother_maiden_name) !=0)
						{
							memcpy(m_strCurrentData, sCCF01Record.mother_maiden_name, sizeof(m_strCurrentData));
							memcpy(m_strPreviousData, sOldCCF01Record.mother_maiden_name, sizeof(m_strPreviousData));
							memcpy(m_strFieldTag, "Mother Maiden Name", sizeof(m_strFieldTag));

							PopulateHistoryTable();
						}

						if (strcmp((char *)sOldCCF01Record.membership_id, (char *)sCCF01Record.membership_id) !=0)
						{
							memcpy(m_strCurrentData, sCCF01Record.membership_id, sizeof(m_strCurrentData));
							memcpy(m_strPreviousData, sOldCCF01Record.membership_id, sizeof(m_strPreviousData));
							memcpy(m_strFieldTag, "Membership ID", sizeof(m_strFieldTag));

							PopulateHistoryTable();
						}

						if (strcmp((char *)sOldCCF01Record.cardholder_id, (char *)sCCF01Record.cardholder_id) !=0)
						{
							memcpy(m_strCurrentData, sCCF01Record.cardholder_id, sizeof(m_strCurrentData));
							memcpy(m_strPreviousData, sOldCCF01Record.cardholder_id, sizeof(m_strPreviousData));
							memcpy(m_strFieldTag, "Cardholder ID", sizeof(m_strFieldTag));

							PopulateHistoryTable();
						}

						if (strcmp((char *)sOldCCF01Record.block_status, (char *)sCCF01Record.block_status) !=0)
						{
							memcpy(m_strCurrentData, sCCF01Record.block_status, sizeof(m_strCurrentData));
							memcpy(m_strPreviousData, sOldCCF01Record.block_status, sizeof(m_strPreviousData));
							memcpy(m_strFieldTag, "Block Status", sizeof(m_strFieldTag));

							PopulateHistoryTable();
						}

						if (strcmp((char *)sOldCCF01Record.specialty_code, (char *)sCCF01Record.specialty_code) !=0)
						{
							memcpy(m_strCurrentData, sCCF01Record.specialty_code, sizeof(m_strCurrentData));
							memcpy(m_strPreviousData, sOldCCF01Record.specialty_code, sizeof(m_strPreviousData));
							memcpy(m_strFieldTag, "Cpecialty Code", sizeof(m_strFieldTag));

							PopulateHistoryTable();
						}

						if (strcmp((char *)sOldCCF01Record.vip_acct_code, (char *)sCCF01Record.vip_acct_code) !=0)
						{
							memcpy(m_strCurrentData, sCCF01Record.vip_acct_code, sizeof(m_strCurrentData));
							memcpy(m_strPreviousData, sOldCCF01Record.vip_acct_code, sizeof(m_strPreviousData));
							memcpy(m_strFieldTag, "VIP Account Code", sizeof(m_strFieldTag));

							PopulateHistoryTable();
						}

						if (strcmp((char *)sOldCCF01Record.name_dba, (char *)sCCF01Record.name_dba) !=0)
						{
							memcpy(m_strCurrentData, sCCF01Record.name_dba, sizeof(m_strCurrentData));
							memcpy(m_strPreviousData, sOldCCF01Record.name_dba, sizeof(m_strPreviousData));
							memcpy(m_strFieldTag, "Name", sizeof(m_strFieldTag));

							PopulateHistoryTable();
						}

						if (strcmp((char *)sOldCCF01Record.address1, (char *)sCCF01Record.address1) !=0)
						{
							memcpy(m_strCurrentData, sCCF01Record.address1, sizeof(m_strCurrentData));
							memcpy(m_strPreviousData, sOldCCF01Record.address1, sizeof(m_strPreviousData));
							memcpy(m_strFieldTag, "Address1", sizeof(m_strFieldTag));

							PopulateHistoryTable();
						}

						if (strcmp((char *)sOldCCF01Record.address2, (char *)sCCF01Record.address2) !=0)
						{
							memcpy(m_strCurrentData, sCCF01Record.address2, sizeof(m_strCurrentData));
							memcpy(m_strPreviousData, sOldCCF01Record.address2, sizeof(m_strPreviousData));
							memcpy(m_strFieldTag, "Address2", sizeof(m_strFieldTag));

							PopulateHistoryTable();
						}

						if (strcmp((char *)sOldCCF01Record.city, (char *)sCCF01Record.city) !=0)
						{
							memcpy(m_strCurrentData, sCCF01Record.city, sizeof(m_strCurrentData));
							memcpy(m_strPreviousData, sOldCCF01Record.city, sizeof(m_strPreviousData));
							memcpy(m_strFieldTag, "City", sizeof(m_strFieldTag));

							PopulateHistoryTable();
						}

						if (strcmp((char *)sOldCCF01Record.state, (char *)sCCF01Record.state) !=0)
						{
							memcpy(m_strCurrentData, sCCF01Record.state, sizeof(m_strCurrentData));
							memcpy(m_strPreviousData, sOldCCF01Record.state, sizeof(m_strPreviousData));
							memcpy(m_strFieldTag, "State", sizeof(m_strFieldTag));

							PopulateHistoryTable();
						}

						if (strcmp((char *)sOldCCF01Record.zip_code, (char *)sCCF01Record.zip_code) !=0)
						{
							memcpy(m_strCurrentData, sCCF01Record.zip_code, sizeof(m_strCurrentData));
							memcpy(m_strPreviousData, sOldCCF01Record.zip_code, sizeof(m_strPreviousData));
							memcpy(m_strFieldTag, "Zip_code", sizeof(m_strFieldTag));

							PopulateHistoryTable();
						}

						if (strcmp((char *)sOldCCF01Record.home_phone, (char *)sCCF01Record.home_phone) !=0)
						{
							memcpy(m_strCurrentData, sCCF01Record.home_phone, sizeof(m_strCurrentData));
							memcpy(m_strPreviousData, sOldCCF01Record.home_phone, sizeof(m_strPreviousData));
							memcpy(m_strFieldTag, "Home_phone", sizeof(m_strFieldTag));

							PopulateHistoryTable();
						}

						if (strcmp((char *)sOldCCF01Record.work_phone, (char *)sCCF01Record.work_phone) !=0)
						{
							memcpy(m_strCurrentData, sCCF01Record.work_phone, sizeof(m_strCurrentData));
							memcpy(m_strPreviousData, sOldCCF01Record.work_phone, sizeof(m_strPreviousData));
							memcpy(m_strFieldTag, "Work_phone", sizeof(m_strFieldTag));

							PopulateHistoryTable();
						}

						if (strcmp((char *)sOldCCF01Record.dob, (char *)sCCF01Record.dob) !=0)
						{
							memcpy(m_strCurrentData, sCCF01Record.dob, sizeof(m_strCurrentData));
							memcpy(m_strPreviousData, sOldCCF01Record.dob, sizeof(m_strPreviousData));
							memcpy(m_strFieldTag, "DOB", sizeof(m_strFieldTag));

							PopulateHistoryTable();
						}

						if (strcmp((char *)sOldCCF01Record.profession, (char *)sCCF01Record.profession) !=0)
						{
							memcpy(m_strCurrentData, sCCF01Record.profession, sizeof(m_strCurrentData));
							memcpy(m_strPreviousData, sOldCCF01Record.profession, sizeof(m_strPreviousData));
							memcpy(m_strFieldTag, "Profession", sizeof(m_strFieldTag));

							PopulateHistoryTable();
						}
						if (strcmp((char *)sOldCCF01Record.country, (char *)sCCF01Record.country) !=0)
						{
							memcpy(m_strCurrentData, sCCF01Record.country, sizeof(m_strCurrentData));
							memcpy(m_strPreviousData, sOldCCF01Record.country, sizeof(m_strPreviousData));
							memcpy(m_strFieldTag, "Country", sizeof(m_strFieldTag));

							PopulateHistoryTable();
						}
					}

				}
				else 
				{
					rcode1 = txdsapi_insert_record((pBYTE)&sCCF01Record,sizeof(CCF01),CCF01_DATA, ErrorMsg);
					if(rcode1 != PTEMSG_OK )    
					 {
						m_ComEdit.SetWindowText(ErrorMsg);
						return FALSE;
					 }
				}
			}
      	m_strComEdit = "Record Updated";
         UpdateData(FALSE);

			break;
		}
	default:
				return FALSE;
	}


	SetModes(MODE_DONE);
	m_pSheet->SetWindowText("Card Record Definition");
	//set version info in the title bar
	CString appProdVer;
	CString lpszTitle;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	m_pSheet->GetWindowText(lpszTitle);
	lpszTitle = lpszTitle + " - Version - ";
	lpszTitle += appProdVer;
	m_pSheet->SetWindowText(lpszTitle);

	CleanPages();

	UpdateData(FALSE);
	
	//return CPropertyPage::OnApply();
   return FALSE;
}

void CCFMain::OnChangeCardnbr() 
{
	CleanPages();
	
}

void CCFMain::OnChangeCardtype() 
{
	CleanPages();
	
}

BOOL CCFMain::ValidateDate(CString strDate, pCHAR ErrMsg) 
{
	CString strDay;
	CString strMonth;
	CString strYear;
	CMyUtils myUtilsObject;

	int nDay, nMonth, nYear;

	UpdateData(TRUE);

	if ( myUtilsObject.IsEmpty ( strDate, this ))
		return TRUE;

	int length = strDate.GetLength();	
	int res = strspn ( strDate,"1234567890" );

	if ( length < 8 )
	{
		strcpy ( ErrMsg, "ERROR: Invalid date format");
		UpdateData(FALSE);
		return FALSE;
	}
	
	strDay = strDate.Left(2);
	strMonth = strDate.Mid(2,2);
	strYear = strDate.Right(4);

	nDay = atoi ( strDay );
	nMonth = atoi ( strMonth );
	nYear = atoi ( strYear );

	if ( nDay == 0 || nMonth == 0 || nYear ==0)
	{
		strcpy ( ErrMsg, "ERROR: Invalid data");
		UpdateData(FALSE);
		return FALSE;
	}


	if ( nMonth > 12 )
	{
		strcpy ( ErrMsg, "ERROR: Invalid month");
		UpdateData(FALSE);
		return FALSE;
	}

	if ( nMonth == 1 || nMonth == 3 || nMonth == 5 || 
		 nMonth == 7 || nMonth == 8 || nMonth == 10 || nMonth == 12 )
	{
		if ( nDay > 31 )
		{
			strcpy ( ErrMsg, "ERROR: Invalid day");
			UpdateData(FALSE);
			return FALSE;
		}
	}

	if ( nMonth == 4 || nMonth == 6 || nMonth == 9 || 
		 nMonth == 11 )
	{
		if ( nDay > 30 )
		{
			strcpy ( ErrMsg,"ERROR: Invalid day");
			UpdateData(FALSE);
			return FALSE;
		}
	}

	if ( nMonth ==2 )
	{
		int res = nYear % 4;
		if ( res != 0 && nDay >28)
		{
			strcpy ( ErrMsg,"ERROR: Invalid day");
			UpdateData(FALSE);
			return FALSE;
		}
		if (res == 0 && nDay >29)
		{
			strcpy ( ErrMsg,"ERROR: Invalid day");
			UpdateData(FALSE);
			return FALSE;
		}
	}
   return TRUE;
}

void CCFMain::OnSearchButton() 
{
	CAlphaLookup alphaDlg;
    
   UpdateData(TRUE);
   alphaDlg.m_nDialogMode = MODE_CCF;
   int nReturnCode = alphaDlg.DoModal();

   if ( nReturnCode == IDOK)
	{
      m_strCardNbr = alphaDlg.strItem0;
		m_strCardType = alphaDlg.strItem1;
      m_ctrlCardType.SelectString(-1,m_strCardType);
   }

   m_ctrlCardNbr.EnableWindow(TRUE);
	m_ctrlCardType.EnableWindow(TRUE);

   m_strComEdit.Empty();

   UpdateData(FALSE);	
}


void CCFMain::OnCancel() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CPropertyPage::OnCancel();
}


BOOL CCFMain::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}
