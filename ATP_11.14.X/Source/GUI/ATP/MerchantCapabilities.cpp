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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\MerchantCapabilities.cpp  $

  Rev 1.8   Jul 08 2004 13:00:38   dirby
  Updated History File logging to make sure Merchant ID is in the
  File_Primary_Key and to make sure fields are not accidently logged
  when no changes have been made.
  SCR 1421
  
	Rev 1.7   Dec 01 2003 11:29:32   lmitchel
	Added AMEX Online Updates - scr1200
	
      Rev 1.6   Feb 10 2002 00:34:18   dirby
	  Modified PopulateHistoryTable function so the unique_id field
	  does not give a constraint violation.  It was frequently giving a
	  unique constraint violation when updating the general and
	  blocking tabs because it could do more than 1 within the same
	  millisecond.  Also, if the data server returned with an error,
	  there was an UpdateData(FALSE) command that would crash
	  the program because it was trying to update a window that did
	  not exist.  I replaced the command with an informational box.
	  
		Rev 1.5   Jan 05 2000 14:53:48   gbarnes
		Major update. Not updated to PVCS recently.
		PVCS was locked.
		
		  Rev 1.1   Sep 22 1999 15:52:02   iarustam
		  Added manual batch entry GUI
		  
			Rev 1.0   Jul 01 1999 14:17:40   iarustam
			Initial Release
			
			  Rev 1.0   Jun 28 1999 13:58:44   iarustam
			  Initial Release
			  
				Rev 1.2   Apr 29 1999 14:56:30   iarustam
				Bug fixes
				
				  Rev 1.1   Mar 16 1999 16:30:02   iarustam
				  Bug fixes
				  
					Rev 1.0   Feb 23 1999 11:40:38   iarustam
					Initial Release
					
					  Rev 1.0   Feb 17 1999 10:09:34   iarustam
					  Initial Release
					  
						Rev 1.1   Jan 22 1999 10:59:04   iarustam
						Bug fixes and core 1_3_0 changes
						
						  Rev 1.1   Jan 21 1999 13:08:24   iarustam
						  Bug fixes
						  
							
							  Rev 1.1   Dec 18 1998 13:43:32   iarustam
							  
								*
************************************************************************************/
// MerchantCapabilities.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "MerchantCapabilities.h"
#include "MyUtils.h"
#include "SharedMem.h"

extern "C"
{
#include "PteTime.h"
}

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern UCF01 ucf01;
//extern merch_ctr;
extern int merch_ctr;
extern CSharedMem shrmem;

extern CString  MerchantID;
extern CString  OrganizationID;


/////////////////////////////////////////////////////////////////////////////
// CMerchantCapabilities property page

IMPLEMENT_DYNCREATE(CMerchantCapabilities, CPropertyPage)

CMerchantCapabilities::CMerchantCapabilities() : CPropertyPage(CMerchantCapabilities::IDD)
{
	//{{AFX_DATA_INIT(CMerchantCapabilities)
	m_StrCreditAuthFld = _T("");
	m_StrCreditCashAdvancedFld = _T("");
	m_StrCreditCashbackFld = _T("");
	m_StrCreditManualEntryFld = _T("");
	m_StrCreditPurchaseFld = _T("");
	m_StrCreditReturnFld = _T("");
	m_StrDebitAuthFld = _T("");
	m_StrDebitCashAdvanceFld = _T("");
	m_StrDebitCashbackFld = _T("");
	m_StrDebitManualEntryFld = _T("");
	m_StrDebitPurchaseFld = _T("");
	m_StrDebitReturnFld = _T("");
	m_StrMerchantIDFld = _T("");
	m_StrMessageLbl = _T("");
	m_StrOrganizationIDFld = _T("");
	m_StrCreditFloorLimitFld = _T("");
	m_StrDebitFloorLimitFld = _T("");
	//}}AFX_DATA_INIT
}

CMerchantCapabilities::~CMerchantCapabilities()
{
}

void CMerchantCapabilities::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMerchantCapabilities)
	DDX_Control(pDX, IDC_DEBIT_FLOOR_LIMIT_FLD, m_CntlDebitFloorLimitFld);
	DDX_Control(pDX, IDC_CREDIT_FLOOR_LIMIT_FLD, m_CntlCreditFloorLimitFld);
	DDX_Control(pDX, IDC_MERCHANT_ID_FLD, m_CntlMerchantIDFld);
	DDX_Control(pDX, IDC_DEBIT_RETURN_FLD, m_CntlDebitReturnFld);
	DDX_Control(pDX, IDC_DEBIT_PURCHASE_FLD, m_CntlDebitPurchaseFld);
	DDX_Control(pDX, IDC_DEBIT_MANUAL_ENTRY_FLD, m_CntlDebitManualEntryFld);
	DDX_Control(pDX, IDC_DEBIT_CASHBACK_FLD, m_CntlDebitCashbackFld);
	DDX_Control(pDX, IDC_DEBIT_CASH_ADVANCE_FLD, m_CntlDebitCashAdvanceFld);
	DDX_Control(pDX, IDC_DEBIT_AUTH_FLD, m_CntlDebitAuthFld);
	DDX_Control(pDX, IDC_CREDIT_RETURN_FLD, m_CntlCreditReturnFld);
	DDX_Control(pDX, IDC_CREDIT_PURCHASE_FLD, m_CntlCreditPurchaseFld);
	DDX_Control(pDX, IDC_CREDIT_MANUAL_ENTRY_FLD, m_CntlCreditManualEntryFld);
	DDX_Control(pDX, IDC_CREDIT_CASHBACK_FLD, m_CntlCreditCashbackFld);
	DDX_Control(pDX, IDC_CREDIT_CASH_ADVANCE_FLD, m_CntlCreditCashAdvancedFld);
	DDX_Control(pDX, IDC_CREDIT_AUTH_FLD, m_CntlCreditAuthFld);
	DDX_Text(pDX, IDC_CREDIT_AUTH_FLD, m_StrCreditAuthFld);
	DDV_MaxChars(pDX, m_StrCreditAuthFld, 10);
	DDX_Text(pDX, IDC_CREDIT_CASH_ADVANCE_FLD, m_StrCreditCashAdvancedFld);
	DDV_MaxChars(pDX, m_StrCreditCashAdvancedFld, 10);
	DDX_Text(pDX, IDC_CREDIT_CASHBACK_FLD, m_StrCreditCashbackFld);
	DDV_MaxChars(pDX, m_StrCreditCashbackFld, 10);
	DDX_Text(pDX, IDC_CREDIT_MANUAL_ENTRY_FLD, m_StrCreditManualEntryFld);
	DDV_MaxChars(pDX, m_StrCreditManualEntryFld, 10);
	DDX_Text(pDX, IDC_CREDIT_PURCHASE_FLD, m_StrCreditPurchaseFld);
	DDV_MaxChars(pDX, m_StrCreditPurchaseFld, 10);
	DDX_Text(pDX, IDC_CREDIT_RETURN_FLD, m_StrCreditReturnFld);
	DDV_MaxChars(pDX, m_StrCreditReturnFld, 10);
	DDX_Text(pDX, IDC_DEBIT_AUTH_FLD, m_StrDebitAuthFld);
	DDV_MaxChars(pDX, m_StrDebitAuthFld, 10);
	DDX_Text(pDX, IDC_DEBIT_CASH_ADVANCE_FLD, m_StrDebitCashAdvanceFld);
	DDV_MaxChars(pDX, m_StrDebitCashAdvanceFld, 10);
	DDX_Text(pDX, IDC_DEBIT_CASHBACK_FLD, m_StrDebitCashbackFld);
	DDV_MaxChars(pDX, m_StrDebitCashbackFld, 10);
	DDX_Text(pDX, IDC_DEBIT_MANUAL_ENTRY_FLD, m_StrDebitManualEntryFld);
	DDV_MaxChars(pDX, m_StrDebitManualEntryFld, 10);
	DDX_Text(pDX, IDC_DEBIT_PURCHASE_FLD, m_StrDebitPurchaseFld);
	DDV_MaxChars(pDX, m_StrDebitPurchaseFld, 10);
	DDX_Text(pDX, IDC_DEBIT_RETURN_FLD, m_StrDebitReturnFld);
	DDV_MaxChars(pDX, m_StrDebitReturnFld, 10);
	DDX_Text(pDX, IDC_MERCHANT_ID_FLD, m_StrMerchantIDFld);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	DDX_Text(pDX, IDC_CHAIN_ID, m_StrOrganizationIDFld);
	DDX_Text(pDX, IDC_CREDIT_FLOOR_LIMIT_FLD, m_StrCreditFloorLimitFld);
	DDV_MaxChars(pDX, m_StrCreditFloorLimitFld, 9);
	DDX_Text(pDX, IDC_DEBIT_FLOOR_LIMIT_FLD, m_StrDebitFloorLimitFld);
	DDV_MaxChars(pDX, m_StrDebitFloorLimitFld, 9);
	
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMerchantCapabilities, CPropertyPage)
//{{AFX_MSG_MAP(CMerchantCapabilities)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMerchantCapabilities message handlers

BOOL CMerchantCapabilities::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	CFont m_font;
	LOGFONT lf;
	
	memset ( &lf, 0, sizeof (lf));
	lf.lfHeight = 12;
	lf.lfWeight = 700;
	strcpy(lf.lfFaceName, "Arial");
	m_font.CreateFontIndirect (&lf);
	GetDlgItem(IDC_MESSAGE_LBL)->SetFont (&m_font);
	
    m_bCapabilities = TRUE;
	
	if (m_nDialogMode != MODE_DONE)
	{
		if ( m_nDialogMode != MODE_VIEW )
			EnableCapabilitiesDialog(TRUE);	
		if ( m_nDialogMode != MODE_ADD )
			PopulateCapabilitiesDialogFromDatabase ( sMerchantRecordOld );
		
	}  
	
	
	if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CMerchantCapabilities::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if ( m_nDialogMode != MODE_VIEW &&  m_nDialogMode != MODE_DONE)
		SetModified (TRUE);
	
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}

void CMerchantCapabilities::EnableCapabilitiesDialog ( int nStatus )
{
	m_CntlCreditAuthFld.EnableWindow(nStatus);
	m_CntlCreditCashAdvancedFld.EnableWindow(nStatus);
	m_CntlCreditCashbackFld.EnableWindow(nStatus);
	m_CntlCreditManualEntryFld.EnableWindow(nStatus);
	m_CntlCreditPurchaseFld.EnableWindow(nStatus);
	m_CntlCreditReturnFld.EnableWindow(nStatus);
	m_CntlCreditFloorLimitFld.EnableWindow(nStatus);
	m_CntlDebitAuthFld.EnableWindow(nStatus);
	m_CntlDebitCashAdvanceFld.EnableWindow(nStatus);
	m_CntlDebitCashbackFld.EnableWindow(nStatus);
	m_CntlDebitManualEntryFld.EnableWindow(nStatus);
	m_CntlDebitPurchaseFld.EnableWindow(nStatus);
	m_CntlDebitReturnFld.EnableWindow(nStatus);
	m_CntlDebitFloorLimitFld.EnableWindow(nStatus);
	
}

BOOL CMerchantCapabilities::PopulateCapabilitiesDialogFromDatabase ( MCF01 sMerchantRecord )
{
	CString temp;
	CMyUtils myUtilsObject;
	
	if ( m_nDialogMode != MODE_VIEW )	
		EnableCapabilitiesDialog(TRUE);
	
	ZeroMemory ( &sMerchantRecordOld, sizeof ( MCF01) );
	
	m_StrMerchantIDFld = sMerchantRecord.primary_key.merchant_id;
	m_StrOrganizationIDFld = sMerchantRecord.primary_key.organization_id;
	
	
	m_StrCreditAuthFld = sMerchantRecord.credit_auth_max;
	m_StrCreditCashAdvancedFld = sMerchantRecord.credit_cash_adv_max;
	m_StrCreditCashbackFld = sMerchantRecord.credit_cash_back_max;
	m_StrCreditManualEntryFld = sMerchantRecord.credit_man_entry_max;
	m_StrCreditPurchaseFld = sMerchantRecord.credit_purch_max;
	m_StrCreditReturnFld = sMerchantRecord.credit_return_max;
	m_StrDebitAuthFld = sMerchantRecord.debit_auth_max;
	m_StrDebitCashAdvanceFld = sMerchantRecord.debit_cash_adv_max;
	m_StrDebitCashbackFld = sMerchantRecord.debit_cash_back_max;
	m_StrDebitManualEntryFld = sMerchantRecord.debit_man_entry_max;
	m_StrDebitPurchaseFld = sMerchantRecord.debit_purch_max;
	m_StrDebitReturnFld = sMerchantRecord.debit_return_max;
	m_StrCreditFloorLimitFld = sMerchantRecord.network[0].priority_routing;
	m_StrDebitFloorLimitFld = sMerchantRecord.network[1].priority_routing;
	
	m_StrCreditAuthFld = myUtilsObject.GetAmount ( m_StrCreditAuthFld );
	m_StrCreditCashAdvancedFld = myUtilsObject.GetAmount ( m_StrCreditCashAdvancedFld );
	m_StrCreditCashbackFld = myUtilsObject.GetAmount ( m_StrCreditCashbackFld );
	m_StrCreditManualEntryFld = myUtilsObject.GetAmount ( m_StrCreditManualEntryFld );
	m_StrCreditPurchaseFld = myUtilsObject.GetAmount ( m_StrCreditPurchaseFld );
	m_StrCreditReturnFld = myUtilsObject.GetAmount ( m_StrCreditReturnFld );
	m_StrDebitAuthFld = myUtilsObject.GetAmount ( m_StrDebitAuthFld );
	m_StrDebitCashAdvanceFld = myUtilsObject.GetAmount ( m_StrDebitCashAdvanceFld );
	m_StrDebitCashbackFld = myUtilsObject.GetAmount ( m_StrDebitCashbackFld );
	m_StrDebitManualEntryFld = myUtilsObject.GetAmount ( m_StrDebitManualEntryFld );
	m_StrDebitPurchaseFld = myUtilsObject.GetAmount ( m_StrDebitPurchaseFld );
	m_StrDebitReturnFld = myUtilsObject.GetAmount ( m_StrDebitReturnFld );
	m_StrCreditFloorLimitFld = myUtilsObject.GetAmount ( m_StrCreditFloorLimitFld );
	m_StrDebitFloorLimitFld = myUtilsObject.GetAmount ( m_StrDebitFloorLimitFld );
	
	UpdateData(FALSE);
	
	memcpy ( &sMerchantRecordOld, &sMerchantRecord, sizeof ( MCF01));
	
	
	return TRUE;
	
}

void CMerchantCapabilities::CleanCapabilitiesDialog()
{
	m_StrCreditAuthFld = _T("");
	m_StrCreditCashAdvancedFld = _T("");
	m_StrCreditCashbackFld = _T("");
	m_StrCreditManualEntryFld = _T("");
	m_StrCreditPurchaseFld = _T("");
	m_StrCreditReturnFld = _T("");
	m_StrDebitAuthFld = _T("");
	m_StrDebitCashAdvanceFld = _T("");
	m_StrDebitCashbackFld = _T("");
	m_StrDebitManualEntryFld = _T("");
	m_StrDebitPurchaseFld = _T("");
	m_StrDebitReturnFld = _T("");
	m_StrMerchantIDFld = _T("");
	m_StrMessageLbl = _T("");
	m_StrOrganizationIDFld = _T("");
	m_StrCreditFloorLimitFld = _T("");
	m_StrDebitFloorLimitFld = _T("");
	
	UpdateData (FALSE);
}

BOOL  CMerchantCapabilities::PopulateDatabaseFromCapabilitiesDialog ( pMCF01 psMerchantRecord )
{
	CMyUtils myUtilsObject;
	
	if ( !m_bCapabilities )
		return TRUE;
	
	m_StrCreditAuthFld = myUtilsObject.GenerateAmount ( m_StrCreditAuthFld, 12 );
	m_StrCreditCashAdvancedFld = myUtilsObject.GenerateAmount ( m_StrCreditCashAdvancedFld, 12 );	
	m_StrCreditCashbackFld = myUtilsObject.GenerateAmount ( m_StrCreditCashbackFld, 12 );	
	m_StrCreditManualEntryFld = myUtilsObject.GenerateAmount ( m_StrCreditManualEntryFld, 12 );	
	m_StrCreditPurchaseFld = myUtilsObject.GenerateAmount ( m_StrCreditPurchaseFld, 12 );	
	m_StrCreditReturnFld = myUtilsObject.GenerateAmount ( m_StrCreditReturnFld, 12 );	
	m_StrDebitAuthFld = myUtilsObject.GenerateAmount ( m_StrDebitAuthFld, 12 );	
	m_StrDebitCashAdvanceFld = myUtilsObject.GenerateAmount ( m_StrDebitCashAdvanceFld, 12 );	
	m_StrDebitCashbackFld = myUtilsObject.GenerateAmount ( m_StrDebitCashbackFld, 12 );	
	m_StrDebitManualEntryFld = myUtilsObject.GenerateAmount ( m_StrDebitManualEntryFld, 12 );	
	m_StrDebitPurchaseFld = myUtilsObject.GenerateAmount ( m_StrDebitPurchaseFld, 12 );
	m_StrDebitReturnFld = myUtilsObject.GenerateAmount ( m_StrDebitReturnFld, 12 );
	m_StrCreditFloorLimitFld = myUtilsObject.GenerateAmount ( m_StrCreditFloorLimitFld, 9 );
	m_StrDebitFloorLimitFld = myUtilsObject.GenerateAmount ( m_StrDebitFloorLimitFld, 9 );
	
	
	memcpy ( psMerchantRecord->credit_auth_max,		 m_StrCreditAuthFld,		sizeof ( psMerchantRecord->credit_auth_max));
	memcpy ( psMerchantRecord->credit_cash_adv_max,  m_StrCreditCashAdvancedFld,sizeof ( psMerchantRecord->credit_cash_adv_max));
	memcpy ( psMerchantRecord->credit_cash_back_max, m_StrCreditCashbackFld,	sizeof ( psMerchantRecord->credit_cash_back_max));
	memcpy ( psMerchantRecord->credit_man_entry_max, m_StrCreditManualEntryFld,	sizeof ( psMerchantRecord->credit_man_entry_max));
	memcpy ( psMerchantRecord->credit_purch_max,	 m_StrCreditPurchaseFld,	sizeof ( psMerchantRecord->credit_purch_max));
	memcpy ( psMerchantRecord->credit_return_max,	 m_StrCreditReturnFld,		sizeof ( psMerchantRecord->credit_return_max));
	memcpy ( psMerchantRecord->debit_auth_max,		 m_StrDebitAuthFld,			sizeof ( psMerchantRecord->debit_auth_max));
	memcpy ( psMerchantRecord->debit_cash_adv_max,	 m_StrDebitCashAdvanceFld,	sizeof ( psMerchantRecord->debit_cash_adv_max));
	memcpy ( psMerchantRecord->debit_cash_back_max,	 m_StrDebitCashbackFld,		sizeof ( psMerchantRecord->debit_cash_back_max));
	memcpy ( psMerchantRecord->debit_man_entry_max,	 m_StrDebitManualEntryFld,	sizeof ( psMerchantRecord->debit_man_entry_max));
	memcpy ( psMerchantRecord->debit_purch_max,		 m_StrDebitPurchaseFld,		sizeof ( psMerchantRecord->debit_purch_max));
	memcpy ( psMerchantRecord->debit_return_max,	 m_StrDebitReturnFld,		sizeof ( psMerchantRecord->debit_return_max));
	memcpy ( psMerchantRecord->network[0].priority_routing,	 m_StrCreditFloorLimitFld,		sizeof ( psMerchantRecord->network[0].priority_routing));
	memcpy ( psMerchantRecord->network[1].priority_routing,	 m_StrDebitFloorLimitFld,		sizeof ( psMerchantRecord->network[1].priority_routing));
	
	
	return TRUE;
}

BOOL CMerchantCapabilities::CompareStructures()
{
	CString strTemp;
	CMyUtils myUtilsObject;
	
	if ( m_bCapabilities == TRUE )
	{
		if (strcmp ( m_StrCreditAuthFld, (char *)sMerchantRecordOld.credit_auth_max) != 0)
		{
			m_StrCreditAuthFld = myUtilsObject.GetAmount(m_StrCreditAuthFld);
			strTemp = sMerchantRecordOld.credit_auth_max;
			strTemp = myUtilsObject.GetAmount(strTemp);
			
			memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );
			
			memcpy(m_strCurrentData, m_StrCreditAuthFld, sizeof (m_strCurrentData));
			memcpy(m_strPreviousData, strTemp, sizeof (m_strPreviousData));
			memcpy(m_strFieldTag, "Credit Auth", sizeof (m_strFieldTag));
			
			PopulateHistoryTable();
		}
		
		if (strcmp ( m_StrCreditCashAdvancedFld, (char *)sMerchantRecordOld.credit_cash_adv_max) != 0)
		{
			m_StrCreditCashAdvancedFld = myUtilsObject.GetAmount(m_StrCreditCashAdvancedFld);
			strTemp = sMerchantRecordOld.credit_cash_adv_max;
			strTemp = myUtilsObject.GetAmount(strTemp);
			
			memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );
			
			memcpy(m_strCurrentData, m_StrCreditCashAdvancedFld, sizeof (m_strCurrentData));
			memcpy(m_strPreviousData, strTemp, sizeof (m_strPreviousData));
			memcpy(m_strFieldTag, "Credit Cash Adv", sizeof (m_strFieldTag));
			
			PopulateHistoryTable();
		}
		
		if (strcmp ( m_StrCreditCashbackFld, (char *)sMerchantRecordOld.credit_cash_back_max) != 0)
		{
			m_StrCreditCashbackFld = myUtilsObject.GetAmount(m_StrCreditCashbackFld);
			strTemp = sMerchantRecordOld.credit_cash_back_max;
			strTemp = myUtilsObject.GetAmount(strTemp);
			
			memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );
			
			memcpy(m_strCurrentData, m_StrCreditCashbackFld, sizeof (m_strCurrentData));
			memcpy(m_strPreviousData, strTemp, sizeof (m_strPreviousData));
			memcpy(m_strFieldTag, "Credit Cashback", sizeof (m_strFieldTag));
			
			PopulateHistoryTable();
		}
		
		if (strcmp ( m_StrCreditManualEntryFld, (char *)sMerchantRecordOld.credit_man_entry_max) != 0)
		{
			m_StrCreditManualEntryFld = myUtilsObject.GetAmount(m_StrCreditManualEntryFld);
			strTemp = sMerchantRecordOld.credit_man_entry_max;
			strTemp = myUtilsObject.GetAmount(strTemp);
			
			memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );
			
			memcpy(m_strCurrentData, m_StrCreditManualEntryFld, sizeof (m_strCurrentData));
			memcpy(m_strPreviousData, strTemp, sizeof (m_strPreviousData));
			memcpy(m_strFieldTag, "Credit Man Entry", sizeof (m_strFieldTag));
			
			PopulateHistoryTable();
		}
		
		if (strcmp ( m_StrCreditPurchaseFld, (char *)sMerchantRecordOld.credit_purch_max) != 0)
		{
			m_StrCreditPurchaseFld = myUtilsObject.GetAmount(m_StrCreditPurchaseFld);
			strTemp = sMerchantRecordOld.credit_purch_max;
			strTemp = myUtilsObject.GetAmount(strTemp);
			
			memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );
			
			memcpy(m_strCurrentData, m_StrCreditPurchaseFld, sizeof (m_strCurrentData));
			memcpy(m_strPreviousData, strTemp, sizeof (m_strPreviousData));
			memcpy(m_strFieldTag, "Credit Purchase", sizeof (m_strFieldTag));
			
			PopulateHistoryTable();
		}
		
		if (strcmp ( m_StrCreditReturnFld, (char *)sMerchantRecordOld.credit_return_max) != 0)
		{
			m_StrCreditReturnFld = myUtilsObject.GetAmount(m_StrCreditReturnFld);
			strTemp = sMerchantRecordOld.credit_return_max;
			strTemp = myUtilsObject.GetAmount(strTemp);
			
			memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );
			
			memcpy(m_strCurrentData, m_StrCreditReturnFld, sizeof (m_strCurrentData));
			memcpy(m_strPreviousData, strTemp, sizeof (m_strPreviousData));
			memcpy(m_strFieldTag, "Credit Return", sizeof (m_strFieldTag));
			
			PopulateHistoryTable();
		}
		
		if (strcmp ( m_StrDebitAuthFld, (char *)sMerchantRecordOld.debit_auth_max) != 0)
		{
			m_StrDebitAuthFld = myUtilsObject.GetAmount(m_StrDebitAuthFld);
			strTemp = sMerchantRecordOld.debit_auth_max;
			strTemp = myUtilsObject.GetAmount(strTemp);
			
			memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );
			
			memcpy(m_strCurrentData, m_StrDebitAuthFld, sizeof (m_strCurrentData));
			memcpy(m_strPreviousData, strTemp, sizeof (m_strPreviousData));
			memcpy(m_strFieldTag, "Debit Auth", sizeof (m_strFieldTag));
			
			PopulateHistoryTable();
		}
		
		if (strcmp ( m_StrDebitCashAdvanceFld, (char *)sMerchantRecordOld.debit_cash_adv_max) != 0)
		{
			m_StrDebitCashAdvanceFld = myUtilsObject.GetAmount(m_StrDebitCashAdvanceFld);
			strTemp = sMerchantRecordOld.debit_cash_adv_max;
			strTemp = myUtilsObject.GetAmount(strTemp);
			
			memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );
			
			memcpy(m_strCurrentData, m_StrDebitCashAdvanceFld, sizeof (m_strCurrentData));
			memcpy(m_strPreviousData, strTemp, sizeof (m_strPreviousData));
			memcpy(m_strFieldTag, "Debit Cash Advance", sizeof (m_strFieldTag));
			
			PopulateHistoryTable();
		}
		
		if (strcmp ( m_StrDebitCashbackFld, (char *)sMerchantRecordOld.debit_cash_back_max) != 0)
		{
			m_StrDebitCashbackFld = myUtilsObject.GetAmount(m_StrDebitCashbackFld);
			strTemp = sMerchantRecordOld.debit_cash_back_max;
			strTemp = myUtilsObject.GetAmount(strTemp);
			
			memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );
			
			memcpy(m_strCurrentData, m_StrDebitCashbackFld, sizeof (m_strCurrentData));
			memcpy(m_strPreviousData, strTemp, sizeof (m_strPreviousData));
			memcpy(m_strFieldTag, "Debit Cashback", sizeof (m_strFieldTag));
			
			PopulateHistoryTable();
		}
		
		if (strcmp ( m_StrDebitManualEntryFld, (char *)sMerchantRecordOld.debit_man_entry_max) != 0)
		{
			m_StrDebitManualEntryFld = myUtilsObject.GetAmount(m_StrDebitManualEntryFld);
			strTemp = sMerchantRecordOld.debit_man_entry_max;
			strTemp = myUtilsObject.GetAmount(strTemp);
			
			memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );
			
			memcpy(m_strCurrentData, m_StrDebitManualEntryFld, sizeof (m_strCurrentData));
			memcpy(m_strPreviousData, strTemp, sizeof (m_strPreviousData));
			memcpy(m_strFieldTag, "Debit Man Entry", sizeof (m_strFieldTag));
			
			PopulateHistoryTable();
		}
		
		if (strcmp ( m_StrDebitPurchaseFld, (char *)sMerchantRecordOld.debit_purch_max) != 0)
		{
			m_StrDebitPurchaseFld = myUtilsObject.GetAmount(m_StrDebitPurchaseFld);
			strTemp = sMerchantRecordOld.debit_purch_max;
			strTemp = myUtilsObject.GetAmount(strTemp);
			
			memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );
			
			memcpy(m_strCurrentData, m_StrDebitPurchaseFld, sizeof (m_strCurrentData));
			memcpy(m_strPreviousData, strTemp, sizeof (m_strPreviousData));
			memcpy(m_strFieldTag, "Debit Purchase", sizeof (m_strFieldTag));
			
			PopulateHistoryTable();
		}
		
		if (strcmp ( m_StrDebitReturnFld, (char *)sMerchantRecordOld.debit_return_max) != 0)
		{
			m_StrDebitReturnFld = myUtilsObject.GetAmount(m_StrDebitReturnFld);
			strTemp = sMerchantRecordOld.debit_return_max;
			strTemp = myUtilsObject.GetAmount(strTemp);
			
			memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );
			
			memcpy(m_strCurrentData, m_StrDebitReturnFld, sizeof (m_strCurrentData));
			memcpy(m_strPreviousData, strTemp, sizeof (m_strPreviousData));
			memcpy(m_strFieldTag, "Debit Return", sizeof (m_strFieldTag));
			
			PopulateHistoryTable();
		}

		if (strcmp ( m_StrCreditFloorLimitFld, (char *)sMerchantRecordOld.network[0].priority_routing) != 0)
		{
			m_StrCreditFloorLimitFld = myUtilsObject.GetAmount(m_StrCreditFloorLimitFld);
			strTemp = sMerchantRecordOld.network[0].priority_routing;
			strTemp = myUtilsObject.GetAmount(strTemp);
			
			memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );
			
			memcpy(m_strCurrentData, m_StrCreditFloorLimitFld, sizeof (m_strCurrentData));
			memcpy(m_strPreviousData, strTemp, sizeof (m_strPreviousData));
			memcpy(m_strFieldTag, "Credit Floor Limit", sizeof (m_strFieldTag));
			
			PopulateHistoryTable();
		}

		if (strcmp ( m_StrDebitFloorLimitFld, (char *)sMerchantRecordOld.network[1].priority_routing) != 0)
		{
			m_StrDebitFloorLimitFld = myUtilsObject.GetAmount(m_StrDebitFloorLimitFld);
			strTemp = sMerchantRecordOld.network[1].priority_routing;
			strTemp = myUtilsObject.GetAmount(strTemp);
			
			memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );
			
			memcpy(m_strCurrentData, m_StrDebitFloorLimitFld, sizeof (m_strCurrentData));
			memcpy(m_strPreviousData, strTemp, sizeof (m_strPreviousData));
			memcpy(m_strFieldTag, "Debit Floor Limit", sizeof (m_strFieldTag));
			
			PopulateHistoryTable();
		}
   }
   return TRUE;
}


void CMerchantCapabilities::PopulateHistoryTable()
{
	HST01 sHistoryData;
	CHAR dest_str[18];   
	CMyUtils myUtilsObject;
	char strErrorMsg[200] = "";
	char strDate[9]       = "";
	char strTime[7]       = "";
	char strPrimaryKey[50]= "";
	CHAR s_merch_ctr[4]   = "";
	CHAR strDisplayErrMsg[250];
	CString temp;
	
	
	strcpy( strPrimaryKey, MerchantID     );
	strcat( strPrimaryKey, OrganizationID );
	
	myUtilsObject.GetSystemDate(strDate);  
	myUtilsObject.GetSystemTime(strTime);
	
	/* Get unique primary key for history table.
    * Use a static counter, instead of milliseconds,
    * to prevent unique constraint violations.
    */
	pintime_get_gmt ( dest_str );
	merch_ctr++;
	if ( merch_ctr > 999 )
		merch_ctr = 0;
	sprintf( s_merch_ctr, "%03d", merch_ctr );
	memcpy( &dest_str[14], s_merch_ctr, 3 );
	
	ZeroMemory ( &sHistoryData, sizeof (HST01));
	memcpy ((LPTSTR)sHistoryData.file_name,
		(LPCTSTR)"MCF01",
		sizeof (sHistoryData.file_name));
	
	temp = strDate;
	temp = temp.Right(4) + temp.Left(2) + temp.Mid(2,2);
	
	CHAR wstation[21]; DWORD wsize;
	
	wsize = sizeof( wstation );
	GetComputerName( wstation, &wsize );
	
	memcpy ((LPTSTR)sHistoryData.reserved,             (LPTSTR)wstation,               sizeof(sHistoryData.reserved));
	memcpy ((LPTSTR) sHistoryData.date_modified,       temp,                           sizeof (sHistoryData.date_modified));
	memcpy ((LPTSTR)sHistoryData.file_primary_key,     (LPCTSTR)strPrimaryKey,         sizeof (sHistoryData.file_primary_key));
	memcpy ((LPTSTR)sHistoryData.primary_key.unique_id,(LPCTSTR)dest_str,              sizeof (sHistoryData.primary_key.unique_id));        
	memcpy ((LPTSTR)sHistoryData.time_modified,        (LPCTSTR)strTime,               sizeof (sHistoryData.time_modified));              
	memcpy ((LPTSTR)sHistoryData.current_data,         (LPCTSTR)m_strCurrentData,      sizeof (sHistoryData.current_data));
	memcpy ((LPTSTR)sHistoryData.previous_data,        (LPCTSTR)m_strPreviousData,     sizeof (sHistoryData.previous_data));
	memcpy ((LPTSTR)sHistoryData.field_tag,            (LPCTSTR)m_strFieldTag,         sizeof (sHistoryData.field_tag));
	memcpy ((LPTSTR)sHistoryData.changed_by,           (LPTSTR)ucf01.primary_key.name, sizeof(sHistoryData.changed_by));
	
	int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
	if ( rcode != PTEMSG_OK )
	{
		m_nDialogMode = MODE_DONE;
		memset( strDisplayErrMsg, 0x00, sizeof(strDisplayErrMsg) );
		sprintf( strDisplayErrMsg,
			"Unable to update History Table HST01 - %s",
			strErrorMsg );
		AfxMessageBox( strDisplayErrMsg, MB_OK );
	}
	return;
}
