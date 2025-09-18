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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\MerchantTerminalConfig.cpp  $
   
      Rev 1.15   Jul 16 2004 16:07:28   lmitchel
   SCR1287 - Added new fields to support Auto Init and Reload Mechants;   A new Auto Init tab and support fields were added to the Device Definition GUI.  A new reload flag was added to the Merchant Definition General GUI.  A new credti subtype = PREPAID was added to the BIN Definition GUI.
   
      Rev 1.14   Jan 09 2004 17:12:30   dirby
   Added code to support new button to set batch number to null.
   SCR 1288
   
      Rev 1.13   Dec 01 2003 11:29:42   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.12   Jul 10 2002 09:56:24   lmitchel
   Ver:4.0.3.1:  Added SendMessage command in PropertySheet files (on_init function) to set the Apply button as the default.  If the enter key is pressed, action will default to  the apply button instead of the OK button.  This allows the user to remain in the current dialog instead of exiting to the main menu.
   
      Rev 1.11   May 21 2002 13:38:56   dirby
   Modified to not exit when <enter> is pressed.  If OK is selected,
   the database gets updated still, but screen is not exited.
   SCR 587
   
      Rev 1.10   Jan 21 2000 12:23:26   iarustam
   Bug fixes # 14,24,,52,63,65,151,165,91,228,202,180,205,504,152,179,251,109,182,177,183,224,225,217,229,230,231,247,200,199,203,201,206,172
   
      Rev 1.9   Jan 14 2000 16:25:02   mbysani
   bug fixes
   
      Rev 1.8   Dec 16 1999 09:34:04   mbysani
   corrected some error based on error reports
   
      Rev 1.4   Oct 15 1999 17:14:50   iarustam
   bug fixes
   
      Rev 1.3   Sep 30 1999 16:17:28   npogosya
   Bug fixes
   
      Rev 1.1   Sep 22 1999 15:54:04   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:48   iarustam
   Initial Release
   

*
************************************************************************************/
// MerchantTerminalConfig.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "MerchantTerminalConfig.h"
#include "MyUtils.h"
#include "BinList.h"

#include "SharedMem.h"	
extern "C"{
#include "PteTime.h"
}

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern UCF01 ucf01;
extern CSharedMem shrmem;
/* Flag used to set batch number to null.
 *    0 = Leave as is
 *    1 = Set to Null
 */
INT  Reset_Batch_Nbr = 0;

/////////////////////////////////////////////////////////////////////////////
// CMerchantTerminalConfig property page

IMPLEMENT_DYNCREATE(CMerchantTerminalConfig, CPropertyPage)

CMerchantTerminalConfig::CMerchantTerminalConfig() : CPropertyPage(CMerchantTerminalConfig::IDD)
{
//    m_TerminalSheet = new CMerchantTerminalPropertySheet ("  ");
	//{{AFX_DATA_INIT(CMerchantTerminalConfig)
	m_StrMessageLbl = _T("");
	m_StrOrganizationIDFld = _T("");
	m_StrMerchantIDFld = _T("");
	m_StrTerminalIDFld = _T("");
	//}}AFX_DATA_INIT
}

CMerchantTerminalConfig::~CMerchantTerminalConfig()
{
}

void CMerchantTerminalConfig::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMerchantTerminalConfig)
	DDX_Control(pDX, IDC_TERMINAL_ID_FLD, m_CntlTerminalIDFld);
	DDX_Control(pDX, IDC_MERCHANT_ID_FLD, m_CntlMerchantIDFld);
	DDX_Control(pDX, IDC_CHAIN_ID_FLD, m_CntlOrganizationIDFld);
	DDX_Control(pDX, IDC_TERMINAL_VIEW_BUTTON, m_ViewButton);
	DDX_Control(pDX, IDC_TERMINAL_UPDATE_BUTTON, m_UpdateButton);
	DDX_Control(pDX, IDC_TERMINAL_LIST_BUTTON, m_ListButton);
	DDX_Control(pDX, IDC_TERMINAL_DELETE_BUTTON, m_DeleteButton);
	DDX_Control(pDX, IDC_TERMINAL_ADD_BUTTON, m_AddButton);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	DDX_Text(pDX, IDC_CHAIN_ID_FLD, m_StrOrganizationIDFld);
	DDV_MaxChars(pDX, m_StrOrganizationIDFld, 4);
	DDX_Text(pDX, IDC_MERCHANT_ID_FLD, m_StrMerchantIDFld);
	DDV_MaxChars(pDX, m_StrMerchantIDFld, 15);
	DDX_Text(pDX, IDC_TERMINAL_ID_FLD, m_StrTerminalIDFld);
	DDV_MaxChars(pDX, m_StrTerminalIDFld, 8);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMerchantTerminalConfig, CPropertyPage)
	//{{AFX_MSG_MAP(CMerchantTerminalConfig)
	ON_BN_CLICKED(IDC_TERMINAL_ADD_BUTTON, OnTerminalAddButton)
	ON_BN_CLICKED(IDC_TERMINAL_DELETE_BUTTON, OnTerminalDeleteButton)
	ON_BN_CLICKED(IDC_TERMINAL_LIST_BUTTON, OnTerminalListButton)
	ON_BN_CLICKED(IDC_TERMINAL_UPDATE_BUTTON, OnTerminalUpdateButton)
	ON_BN_CLICKED(IDC_TERMINAL_VIEW_BUTTON, OnTerminalViewButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMerchantTerminalConfig message handlers
BOOL CMerchantTerminalConfig::OnInitDialog() 
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

	m_StrTerminalIDFld = _T("");
	m_StrMessageLbl = _T("");
	m_StrOrganizationIDFld = _T("");
	m_StrMerchantIDFld = _T("");

	CString temp;
	int nIndex = -1;

	for ( int i = 0; i <=15; i++ )
	{
		temp = ucf01.control[i].subsystem_name;
		if ( strcmp ( temp, "Device Definition" ) == 0)
			nIndex = i;
	}

	if ( nIndex != -1)
	{
		if ( ucf01.control[nIndex].action[0] == '0' )
			m_AddButton.EnableWindow(FALSE);
		if ( ucf01.control[nIndex].action[1] == '0' )
			m_DeleteButton.EnableWindow(FALSE);
		if ( ucf01.control[nIndex].action[2] == '0' )
			m_UpdateButton.EnableWindow(FALSE);
		if ( ucf01.control[nIndex].action[3] == '0' )
			m_ViewButton.EnableWindow(FALSE);
	}

   Reset_Batch_Nbr = 0;  /* Initialize to not set batch number to null */

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
void CMerchantTerminalConfig::OnTerminalAddButton() 
{   
	char strDate[9] = "";
    
	char strErrorMessage[200];
	MCF01 sMerchantRecord;
	DCF01 sTerminalRecord;

    
    CMyUtils myUtilsObject;

	m_StrMessageLbl = "";

	m_nDialogMode = MODE_ADD;

	if (!CheckDialogForCompleteness())
        return;

	pGeneralPage->m_nDialogMode = MODE_ADD;
	pTranPage->m_nDialogMode = MODE_ADD;
	pTranPage->m_bOpen = TRUE;
	pCurrentPage->m_nDialogMode = MODE_ADD;
	pBatchHistoryPage->m_nDialogMode = MODE_ADD;
	pKeysPage->m_nDialogMode = MODE_ADD;
	pChangeHistoryPage->m_nDialogMode = MODE_ADD;
	pAutoInitPage->m_nDialogMode = MODE_ADD;
	pAutoInitPage->m_bOpen = TRUE;

	 
	
	ZeroMemory(&sTerminalRecord,sizeof(DCF01)) ;

    memcpy ((LPSTR)(sTerminalRecord.primary_key.device_id), m_StrTerminalIDFld , sizeof (sTerminalRecord.primary_key.device_id));
	memcpy ((LPSTR)(sTerminalRecord.merchant_id), m_StrMerchantIDFld, sizeof (sTerminalRecord.merchant_id) ); 
	
	int rcode = txdsapi_get_record( (pBYTE)&sTerminalRecord,sizeof (DCF01),DCF01_DATA, strErrorMessage );
	if( rcode == PTEMSG_OK )
    {
      m_StrMessageLbl = "ERROR: Device ID already exists";
      m_nDialogMode = MODE_DONE;

		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return ;
    }

		ZeroMemory(&sMerchantRecord,sizeof(MCF01)) ;

    memcpy ((LPSTR)(sMerchantRecord.primary_key.merchant_id), m_StrMerchantIDFld , sizeof (sMerchantRecord.primary_key.merchant_id));
    memcpy ((LPSTR)(sMerchantRecord.primary_key.organization_id), m_StrOrganizationIDFld , sizeof (sMerchantRecord.primary_key.organization_id));  

	rcode = txdsapi_get_record( (pBYTE)&sMerchantRecord,sizeof (MCF01), MCF01_DATA, strErrorMessage );
	if( rcode == PTEMSG_NOT_FOUND )
    {
      m_StrMessageLbl = "ERROR: Invalid Merchant ID";
      m_nDialogMode = MODE_DONE;

		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return ;
    }
	if(sMerchantRecord.status[0] == 'R')
	{
		m_StrMessageLbl = "Cannot Add TID: MERCHANT ID is RESTRICTED";
		m_nDialogMode = MODE_DONE;
		if (!UpdateData(FALSE))
		{
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return ;
	}
	if ( pGeneralPage->m_bGeneral )
	{
		pGeneralPage->CleanGeneralDialog();	
		pGeneralPage->EnableGeneralDialog( TRUE );
	}

	if ( pCurrentPage->m_bCurrentBatch )
	{
		pCurrentPage->CleanCurrentBatchDialog();
		pCurrentPage->UpdateData(FALSE);
	}
	if ( pCUPCurrentPage->m_bCurrentBatch )
	{
		pCUPCurrentPage->CleanCurrentBatchDialog();
		pCUPCurrentPage->UpdateData(FALSE);
	}

	if ( pBatchHistoryPage->m_bBatchHistory )
	{
		pBatchHistoryPage->CleanBatchHistoryDialog();
		pBatchHistoryPage->UpdateData(FALSE);
	}

	if ( pKeysPage->m_bKeys )
	{
		pKeysPage->CleanKeysDialog();	
		pKeysPage->EnableKeysDialog( TRUE );
	}

	if ( pTranPage->m_bTran )
	{
		pTranPage->CleanTranDialog(1);	
		pTranPage->EnableTranDialog( TRUE );
	}

	if ( pAutoInitPage->m_bAutoInit )
	{
		pAutoInitPage->CleanAutoInitDialog();	
		pAutoInitPage->EnableAutoInitDialog( TRUE );
	}


   if ( pChangeHistoryPage->m_bChangeHistory )
   {
		pChangeHistoryPage->CleanChangeHistory();
   }

	pGeneralPage->m_StrMerchantIDFld = m_StrMerchantIDFld;
    pGeneralPage->m_StrOrganizationIDFld = m_StrOrganizationIDFld;
    pGeneralPage->m_StrTerminalIDFld = m_StrTerminalIDFld;

	myUtilsObject.GetSystemDate(strDate);	
	pGeneralPage->m_StrDateAddedFld = strDate;
	pGeneralPage->m_StrDateAddedFld =	  pGeneralPage->m_StrDateAddedFld.Mid(2,2) + "/" 
										+ pGeneralPage->m_StrDateAddedFld.Left(2) + "/" 
										+ pGeneralPage->m_StrDateAddedFld.Right(4);

	pCurrentPage->m_StrMerchantIDFld = m_StrMerchantIDFld;
    pCurrentPage->m_StrOrganizationIDFld = m_StrOrganizationIDFld;
    pCurrentPage->m_StrTerminalIDFld = m_StrTerminalIDFld;

	pBatchHistoryPage->m_StrMerchantIDFld = m_StrMerchantIDFld;
    pBatchHistoryPage->m_StrOrganizationIDFld = m_StrOrganizationIDFld;
    pBatchHistoryPage->m_StrTerminalIDFld = m_StrTerminalIDFld;

	pTranPage->m_StrMerchantIDFld = m_StrMerchantIDFld;
    pTranPage->m_StrOrganizationIDFld = m_StrOrganizationIDFld;
    pTranPage->m_StrTerminalIDFld = m_StrTerminalIDFld;


	pKeysPage->m_StrMerchantIDFld = m_StrMerchantIDFld;
    pKeysPage->m_StrOrganizationIDFld = m_StrOrganizationIDFld;
    pKeysPage->m_StrTerminalIDFld = m_StrTerminalIDFld;

	

	pChangeHistoryPage->bNext = FALSE;
	((CPropertySheet*) this->GetParent())->SetWindowText("Add Device");
	//set version info in the title bar
	CString appProdVer;
	CString lpszTitle;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
	lpszTitle = lpszTitle + " - Version - ";
	lpszTitle += appProdVer;
	((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);
	m_CntlMerchantIDFld.EnableWindow(FALSE);
	m_CntlOrganizationIDFld.EnableWindow(FALSE);
	m_CntlTerminalIDFld.EnableWindow(FALSE);	

	
/*	((CPropertySheet*) this->GetParent())->SetActivePage(4);
	pTranPage->m_VoidSaleOnLine.SetCheck(1);
	pTranPage->m_VoidRefundOnLine.SetCheck(1);
	pTranPage->m_TestTransaction.SetCheck(1);
	pTranPage->m_Statistics.SetCheck(1);
	pTranPage->m_SignatureData.SetCheck(1);
	pTranPage->m_SettlementRequest.SetCheck(1);
	pTranPage->m_SettlementAfterUpload.SetCheck(1);
	pTranPage->m_SalesCompletion.SetCheck(1);
	pTranPage->m_SaleAndCash.SetCheck(1);
	pTranPage->m_Sale.SetCheck(1);
	pTranPage->m_Reversal.SetCheck(1);
	pTranPage->m_Refund.SetCheck(1);
	pTranPage->m_ReAuth.SetCheck(1);
	pTranPage->m_PreAuthorization.SetCheck(1);
	pTranPage->m_PleaseWaitAdvice.SetCheck(1);
	pTranPage->m_OffLineSale.SetCheck(1);
	pTranPage->m_OffLineRefund.SetCheck(1);
	pTranPage->m_Logon.SetCheck(1);
	pTranPage->m_Initialization.SetCheck(1);
	pTranPage->m_GuaranteedLateArrival.SetCheck(1);
	pTranPage->m_CheckVerification.SetCheck(1);
	pTranPage->m_Cash.SetCheck(1);
	pTranPage->m_CardVerification.SetCheck(1);
	pTranPage->m_BatchUpload.SetCheck(1);
	pTranPage->m_BatchDownLineLoad.SetCheck(1);
	pTranPage->m_BalanceInquiry.SetCheck(1);
	pTranPage->m_Authorization.SetCheck(1);
	pTranPage->m_AdjustSale.SetCheck(1);
	pTranPage->m_AdjustRefund.SetCheck(1);*/



	((CPropertySheet*) this->GetParent())->SetActivePage(1);
	SetModified(FALSE);
	
}

void CMerchantTerminalConfig::OnTerminalDeleteButton() 
{
	CString strMessage;
    int nReturnCode;
    DCF01 sTerminalRecord;
	char strErrorMessage[200];
    
     m_nDialogMode = MODE_EDIT;
	 m_StrMessageLbl = "";

    if (!CheckDialogForCompleteness())
        return;   

   /* m_pUserSheet->m_AccessPage.m_psp.dwFlags &= ~PSP_HASHELP; 
    m_pUserSheet->m_WorkstationPage.m_psp.dwFlags &= ~PSP_HASHELP; 
    m_pUserSheet->m_ChangeHistoryPage.m_psp.dwFlags &= ~PSP_HASHELP;
   
    m_pUserSheet->m_psh.dwFlags &= ~PSH_HASHELP;*/

    if (!UpdateData(TRUE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}
    
	strMessage.Format ("Are you sure you want to delete Device ID %s,\n Merchant ID %s,Organization ID %s?", (LPCTSTR) m_StrTerminalIDFld,(LPCTSTR) m_StrMerchantIDFld,(LPCTSTR) m_StrOrganizationIDFld) ;

	nReturnCode = AfxMessageBox ((LPCTSTR)strMessage, MB_OKCANCEL|MB_ICONQUESTION) ;
	if (nReturnCode != IDOK) 
		return ;

	memcpy ((LPTSTR)sTerminalRecord.primary_key.device_id,m_StrTerminalIDFld, sizeof (sTerminalRecord.primary_key.device_id)) ;
	memcpy ((LPSTR)(sTerminalRecord.merchant_id), m_StrMerchantIDFld, sizeof (sTerminalRecord.merchant_id) );   

    int rcode = txdsapi_delete_record ((pBYTE)&sTerminalRecord, sizeof ( DCF01), DCF01_DATA, strErrorMessage) ;
	if ( rcode != PTEMSG_OK )
    {
		 switch( rcode )
      {
         case PTEMSG_NOT_FOUND:
			 sprintf( strErrorMessage, "ERROR: Device record for %s does not exist", m_StrMerchantIDFld );
            break;
      }

      m_StrMessageLbl = strErrorMessage;
		m_nDialogMode = MODE_DONE;
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return ;
	}

    else
    {
        m_StrMessageLbl = "Record Deleted";

        memcpy ((LPSTR)m_strCurrentData, "Record Deleted", sizeof (m_strCurrentData));

        PopulateHistoryTable();
		m_StrMerchantIDFld = _T("");	
		m_StrOrganizationIDFld = _T("");
		m_StrTerminalIDFld = "";

		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
	
		if ( pGeneralPage->m_bGeneral )
		{
		pGeneralPage->CleanGeneralDialog();
		pGeneralPage->EnableGeneralDialog(FALSE);
		}
		if ( pCurrentPage->m_bCurrentBatch )
		{
		pCurrentPage->CleanCurrentBatchDialog();
		}
		if ( pCUPCurrentPage->m_bCurrentBatch )
		{
		pCUPCurrentPage->CleanCurrentBatchDialog();
		}
		if ( pBatchHistoryPage->m_bBatchHistory )
		{
		pBatchHistoryPage->CleanBatchHistoryDialog();
		}
		if ( pKeysPage->m_bKeys)
		{
		pKeysPage->CleanKeysDialog();
		pKeysPage->EnableKeysDialog( FALSE );
		}
		if ( pTranPage->m_bTran)
		{
		pTranPage->CleanTranDialog(0);
		pTranPage->EnableTranDialog( FALSE );
		}

	if ( pAutoInitPage->m_bAutoInit )
	{
		pAutoInitPage->CleanAutoInitDialog();	
		pAutoInitPage->EnableAutoInitDialog( FALSE );
	}

		m_nDialogMode = MODE_DONE;

		if ( pChangeHistoryPage->m_bChangeHistory )
			pChangeHistoryPage->CleanChangeHistory();

		((CPropertySheet*) this->GetParent())->SetActivePage(0);
		SetModified ( FALSE );
    }

		m_CntlMerchantIDFld.EnableWindow(TRUE);
		m_CntlOrganizationIDFld.EnableWindow(TRUE);
		m_CntlTerminalIDFld.EnableWindow(TRUE);
	
}

void CMerchantTerminalConfig::OnTerminalListButton() 
{
	CBinList listDlg;
    int nReturnCode;

	UpdateData(TRUE);
	m_StrMessageLbl = "";
	UpdateData(FALSE);

   // listDlg.m_nIndicator = MODE_GROUP;
 //   listDlg.SetWindowText ( "User List");
	listDlg.m_nDialogMode = MODE_TERMINAL;
	listDlg.strItem0 = m_StrTerminalIDFld;

    nReturnCode = listDlg.DoModal();
    if ( nReturnCode == IDOK)
	{
        m_StrMerchantIDFld = listDlg.strItem1;
		m_StrOrganizationIDFld = listDlg.strItem2;
		m_StrTerminalIDFld = listDlg.strItem0;

				if ( pGeneralPage->m_bGeneral )
		{
		pGeneralPage->CleanGeneralDialog();
		pGeneralPage->EnableGeneralDialog(FALSE);
		}
		
		pCurrentPage->CleanCurrentBatchDialog();
		pCUPCurrentPage->CleanCurrentBatchDialog();
		pBatchHistoryPage->CleanBatchHistoryDialog();

		if ( pKeysPage->m_bKeys)
		{
		pKeysPage->CleanKeysDialog();
		pKeysPage->EnableKeysDialog( FALSE );
		}
		if ( pTranPage->m_bTran)
		{
		pTranPage->CleanTranDialog(0);
		pTranPage->EnableTranDialog( FALSE );
		}

		if ( pAutoInitPage->m_bAutoInit )
		{
			pAutoInitPage->CleanAutoInitDialog();	
			pAutoInitPage->EnableAutoInitDialog( FALSE );
		}


		m_CntlMerchantIDFld.EnableWindow(TRUE);
		m_CntlOrganizationIDFld.EnableWindow(TRUE);
		m_CntlTerminalIDFld.EnableWindow(TRUE);
	

	}
        
    if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}		
	
}

void CMerchantTerminalConfig::OnTerminalUpdateButton() 
{
    HST01 sHistoryRecord;
	char strErrorMessage[200];

    m_nDialogMode = MODE_EDIT;

	m_StrMessageLbl = "";

    if (!CheckDialogForCompleteness())
        return;

    /*m_pUserSheet->m_AccessPage.m_psp.dwFlags &= ~PSP_HASHELP; 
    m_pUserSheet->m_WorkstationPage.m_psp.dwFlags &= ~PSP_HASHELP; 
    m_pUserSheet->m_ChangeHistoryPage.m_psp.dwFlags &= ~PSP_HASHELP;

    m_pUserSheet->m_psh.dwFlags |= PSH_NOAPPLYNOW ; 
    m_pUserSheet->m_psh.dwFlags &= ~PSH_HASHELP;*/

    memcpy ((LPSTR)(sTerminalRecordOld.primary_key.device_id), m_StrTerminalIDFld, sizeof (sTerminalRecordOld.primary_key.device_id) );   
	memcpy ((LPSTR)(sTerminalRecordOld.merchant_id), m_StrMerchantIDFld, sizeof (sTerminalRecordOld.merchant_id) );   

	int rcode = txdsapi_get_record( (pBYTE)&sTerminalRecordOld,sizeof (DCF01),DCF01_DATA, strErrorMessage );
	if ( rcode != PTEMSG_OK )
	{       
       	 switch( rcode )
      {
         case PTEMSG_NOT_FOUND:
			 sprintf( strErrorMessage, "ERROR: Device record for %s does not exist", m_StrTerminalIDFld );
            break;
      }

      m_StrMessageLbl = strErrorMessage;
		m_nDialogMode = MODE_DONE;
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}

		return;
    }

	if ( pGeneralPage->m_bGeneral )
	{
		pGeneralPage->CleanGeneralDialog();
		pGeneralPage->EnableGeneralDialog(FALSE);
	}
	if ( pCurrentPage->m_bCurrentBatch )
	{
		pCurrentPage->CleanCurrentBatchDialog();
	}
	if ( pCUPCurrentPage->m_bCurrentBatch )
	{
		pCUPCurrentPage->CleanCurrentBatchDialog();
	}
	if ( pBatchHistoryPage->m_bBatchHistory )
	{
		pBatchHistoryPage->CleanBatchHistoryDialog();

	}
	if ( pTranPage->m_bTran)
	{
		pTranPage->CleanTranDialog(0);
		pTranPage->EnableTranDialog( FALSE );
	}
	if ( pKeysPage->m_bKeys)
	{
		pKeysPage->CleanKeysDialog();
		pKeysPage->EnableKeysDialog( FALSE );
	}

	if ( pAutoInitPage->m_bAutoInit )
	{
		pAutoInitPage->CleanAutoInitDialog();	
		pAutoInitPage->EnableAutoInitDialog( FALSE );
	}

	
    ZeroMemory (&sHistoryRecord, sizeof (sHistoryRecord));

    memcpy ((LPSTR)(sHistoryRecord.primary_key.unique_id), "                 ", sizeof (sHistoryRecord.primary_key.unique_id));
    memcpy ((LPSTR)(sHistoryRecord.file_name), "DCF01", sizeof (sHistoryRecord.file_name));
    memcpy ((LPSTR)(sHistoryRecord.file_primary_key), sTerminalRecordOld.primary_key.device_id, sizeof (sHistoryRecord.file_primary_key));
	//strcat ((LPSTR)(sHistoryRecord.file_primary_key),m_StrMerchantIDFld );
    memcpy ( &(pChangeHistoryPage->sHistoryRecord), &sHistoryRecord, sizeof(sHistoryRecord));

	((CPropertySheet*) this->GetParent())->SetActivePage(1);  
    
	pGeneralPage->m_nDialogMode = MODE_EDIT;
	pCurrentPage->m_nDialogMode = MODE_EDIT;
	pCUPCurrentPage->m_nDialogMode = MODE_EDIT;
	pBatchHistoryPage->m_nDialogMode = MODE_EDIT;
	pKeysPage->m_nDialogMode = MODE_EDIT;
	pTranPage->m_nDialogMode = MODE_EDIT;
	pAutoInitPage->m_nDialogMode = MODE_EDIT;


    pChangeHistoryPage->m_nDialogMode = MODE_EDIT;
	pChangeHistoryPage->bNext = FALSE;

	((CPropertySheet*) this->GetParent())->SetWindowText("Update Device");
	//set version info in the title bar
	CString appProdVer;
	CString lpszTitle;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
	lpszTitle = lpszTitle + " - Version - ";
	lpszTitle += appProdVer;
	((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);

	if ( pChangeHistoryPage->m_bChangeHistory )
		pChangeHistoryPage->PopulateHistoryDialog();

	PopulateTerminalDialogFromDatabase ( sTerminalRecordOld );

	pGeneralPage->SetModified ( FALSE );	

	m_CntlMerchantIDFld.EnableWindow(FALSE);
	m_CntlOrganizationIDFld.EnableWindow(FALSE);
	m_CntlTerminalIDFld.EnableWindow(FALSE);

	
}

void CMerchantTerminalConfig::OnTerminalViewButton() 
{
	DCF01 sTerminalRecord;
    HST01 sHistoryRecord;
	char strErrorMessage[200];

    m_nDialogMode = MODE_VIEW;

	m_StrMessageLbl = "";

		SetModified (FALSE);
	pGeneralPage->SetModified ( FALSE );	

    if (!CheckDialogForCompleteness())
        return;

    memcpy ((LPSTR)(sTerminalRecord.primary_key.device_id), m_StrTerminalIDFld, sizeof (sTerminalRecord.primary_key.device_id) );   
    memcpy ((LPSTR)(sTerminalRecord.merchant_id), m_StrMerchantIDFld, sizeof (sTerminalRecord.merchant_id) );   

	int rcode = txdsapi_get_record( (pBYTE)&sTerminalRecord,sizeof (DCF01),DCF01_DATA, strErrorMessage );
	if ( rcode != PTEMSG_OK )
	{       
       	 switch( rcode )
      {
         case PTEMSG_NOT_FOUND:
			 sprintf( strErrorMessage, "ERROR: Device record for %s does not exist", m_StrTerminalIDFld );
            break;
      }

      m_StrMessageLbl = strErrorMessage;
		m_nDialogMode = MODE_DONE;
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}

		return;
    }

	if ( pGeneralPage->m_bGeneral )
	{
	pGeneralPage->CleanGeneralDialog();
	pGeneralPage->EnableGeneralDialog(FALSE);
	}
	if ( pCurrentPage->m_bCurrentBatch )
	{
	pCurrentPage->CleanCurrentBatchDialog();
//		pCurrentPage->EnableCurrentDialog( FALSE );
	}
	if ( pCUPCurrentPage->m_bCurrentBatch )
	{
	pCUPCurrentPage->CleanCurrentBatchDialog();
//		pCurrentPage->EnableCurrentDialog( FALSE );
	}
	if ( pBatchHistoryPage->m_bBatchHistory )
	{
	pBatchHistoryPage->CleanBatchHistoryDialog();
//		pBatchHistoryPage->EnableBatchHistoryDialog( FALSE );
	}
	if ( pKeysPage->m_bKeys)
	{
	pKeysPage->CleanKeysDialog();
	pKeysPage->EnableKeysDialog( FALSE );
	}
	if ( pTranPage->m_bTran)
	{
	pTranPage->CleanTranDialog(0);
	pTranPage->EnableTranDialog( FALSE );
	}

	if ( pAutoInitPage->m_bAutoInit )
	{
		pAutoInitPage->CleanAutoInitDialog();	
		pAutoInitPage->EnableAutoInitDialog( FALSE );
	}


	
    ZeroMemory (&sHistoryRecord, sizeof (sHistoryRecord));

//	memset ( strFilePrimaryKey, 0, sizeof (strFilePrimaryKey));
//	strcpy ( strFilePrimaryKey, (char *)sTerminalRecord.primary_key.device_id);

    memcpy ((LPSTR)(sHistoryRecord.primary_key.unique_id), "                 ", sizeof (sHistoryRecord.primary_key.unique_id));
    memcpy ((LPSTR)(sHistoryRecord.file_name), "DCF01", sizeof (sHistoryRecord.file_name));
    memcpy ((LPSTR)(sHistoryRecord.file_primary_key), sTerminalRecord.primary_key.device_id, sizeof (sHistoryRecord.file_primary_key));
	//strcat ((LPSTR)(sHistoryRecord.file_primary_key),m_StrMerchantIDFld );

    memcpy ( &(pChangeHistoryPage->sHistoryRecord), &sHistoryRecord, sizeof(sHistoryRecord));
    
//    m_StrGroupIDFld = sMerchantRecord.group_id; 
	
	//((CPropertySheet*) this->GetParent())->SetActivePage(3);
	//((CPropertySheet*) this->GetParent())->SetActivePage(2);
	((CPropertySheet*) this->GetParent())->SetActivePage(1);  
    
	pGeneralPage->m_nDialogMode = MODE_VIEW;
	pCurrentPage->m_nDialogMode = MODE_VIEW;
	pCUPCurrentPage->m_nDialogMode = MODE_VIEW;
	pBatchHistoryPage->m_nDialogMode = MODE_VIEW;
	pKeysPage->m_nDialogMode = MODE_VIEW;
	pTranPage->m_nDialogMode = MODE_VIEW;
	pAutoInitPage->m_nDialogMode = MODE_VIEW;
    pChangeHistoryPage->m_nDialogMode = MODE_VIEW;
	pChangeHistoryPage->bNext = FALSE;

	((CPropertySheet*) this->GetParent())->SetWindowText("View Device");
	//set version info in the title bar
	CString appProdVer;
	CString lpszTitle;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
	lpszTitle = lpszTitle + " - Version - ";
	lpszTitle += appProdVer;
	((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);

	if ( pChangeHistoryPage->m_bChangeHistory )
		pChangeHistoryPage->PopulateHistoryDialog();

	PopulateTerminalDialogFromDatabase ( sTerminalRecord );	
	m_CntlMerchantIDFld.EnableWindow(TRUE);
	m_CntlOrganizationIDFld.EnableWindow(TRUE);
	m_CntlTerminalIDFld.EnableWindow(TRUE);

}

BOOL CMerchantTerminalConfig::CheckDialogForCompleteness()
{
    CMyUtils myUtilsObject;

    if (!UpdateData(TRUE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}

	if (myUtilsObject.IsEmpty (m_StrTerminalIDFld, this))
    {
		m_StrMessageLbl = "Please enter Device ID";	
		m_CntlTerminalIDFld.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		m_nDialogMode = MODE_DONE;
		return FALSE;
	}

	if ( m_nDialogMode == MODE_ADD )
	{
		if (myUtilsObject.IsEmpty (m_StrMerchantIDFld, this))
		{
			m_StrMessageLbl = "Please enter Merchant ID";	
			m_CntlMerchantIDFld.SetFocus();
			if (!UpdateData(FALSE)){
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}
			m_nDialogMode = MODE_DONE;
			return FALSE;
		}

		if (myUtilsObject.IsEmpty (m_StrOrganizationIDFld, this))
		{
			m_StrMessageLbl = "Please enter Organization ID";	
			m_CntlOrganizationIDFld.SetFocus();
			if (!UpdateData(FALSE)){
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}
			m_nDialogMode = MODE_DONE;
			return FALSE;
		}
	}

	
	
    return TRUE;
}

void CMerchantTerminalConfig::PopulateHistoryTable()
{
    HST01 sHistoryData;
    CHAR dest_str[18];   
	CMyUtils myUtilsObject;
	char strErrorMsg[200] = "";
   CString temp;

    char strDate[9] = "";
    char strTime[7] = "";

	char strPrimaryKey[50];

    memset ( strDate, 0, sizeof (strDate));
    memset ( strTime, 0, sizeof (strTime));
	memset ( strPrimaryKey, 0, sizeof (strPrimaryKey));

	strcpy ( strPrimaryKey, m_StrTerminalIDFld);
    
    myUtilsObject.GetSystemDate(strDate);  

    myUtilsObject.GetSystemTime(strTime);
    pintime_get_gmt ( dest_str );
    ZeroMemory ( &sHistoryData, sizeof (HST01));
    memcpy ((LPTSTR) sHistoryData.file_name,     (LPCTSTR)"DCF01", sizeof (sHistoryData.file_name));
    temp = strDate;
    temp = temp.Right(4) + temp.Left(2) + temp.Mid(2,2);
    
    CHAR wstation[21]; DWORD wsize;

    wsize = sizeof( wstation );
    GetComputerName( wstation, &wsize );

	memcpy ((LPTSTR)sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
	 memcpy ((LPTSTR) sHistoryData.file_name,     (LPCTSTR)"DCF01", sizeof (sHistoryData.file_name));
    memcpy ((LPTSTR) sHistoryData.date_modified, temp, sizeof (sHistoryData.date_modified));
    memcpy ((LPTSTR) sHistoryData.file_primary_key,  (LPCTSTR)strPrimaryKey, sizeof (sHistoryData.file_primary_key));
    memcpy ((LPTSTR) sHistoryData.primary_key.unique_id,     (LPCTSTR)dest_str, sizeof (sHistoryData.primary_key.unique_id));        
    memcpy ((LPTSTR) sHistoryData.time_modified,     (LPCTSTR)strTime, sizeof (sHistoryData.time_modified)); 
             
    memcpy ((LPTSTR)sHistoryData.current_data, (LPCTSTR)m_strCurrentData, sizeof (sHistoryData.current_data));
//	memcpy ((LPTSTR)sHistoryData.previous_data, (LPCTSTR)m_strPreviousData, sizeof (sHistoryData.previous_data));
//	memcpy ((LPTSTR)sHistoryData.field_tag, (LPCTSTR)m_strFieldTag, sizeof (sHistoryData.field_tag));
	memcpy ((LPTSTR)sHistoryData.changed_by,(LPTSTR)ucf01.primary_key.name, sizeof(sHistoryData.changed_by));

    int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
	if ( rcode != PTEMSG_OK )
	{
		m_StrMessageLbl = strErrorMsg;
		m_nDialogMode = MODE_DONE;
		UpdateData( FALSE );
	}
    
}

void CMerchantTerminalConfig::PopulateTerminalDialogFromDatabase( DCF01 sTerminalRecord )
{	

	if ( !pGeneralPage->m_bGeneral )
		memcpy ( &pGeneralPage->sTerminalRecordOld, &sTerminalRecord, sizeof ( DCF01));
	else
		pGeneralPage->PopulateGeneralDialogFromDatabase(sTerminalRecord);

	if ( pTranPage->m_bTran )	
		pTranPage->PopulateTranDialogFromDatabase(sTerminalRecord);

	memcpy ( &pTranPage->sTerminalRecordOld, &sTerminalRecord, sizeof ( DCF01));

	memset ( &(pCurrentPage->sBatchRecord), 0, sizeof (BCH01));
	memset ( &(pCUPCurrentPage->sBatchRecord), 0, sizeof (BCH01));

	memcpy ( pCurrentPage->sBatchRecord.primary_key.device_id, m_StrTerminalIDFld, sizeof (pCurrentPage->sBatchRecord.primary_key.device_id));
	memcpy (pCurrentPage->sBatchRecord.primary_key.batch_nbr , "ACTIVE",sizeof (pCurrentPage->sBatchRecord.primary_key.batch_nbr));
	memcpy ( pCurrentPage->sBatchRecord.merchant_id, m_StrMerchantIDFld, sizeof (pCurrentPage->sBatchRecord.merchant_id));

	memcpy ( pCUPCurrentPage->sBatchRecord.primary_key.device_id, m_StrTerminalIDFld, sizeof (pCurrentPage->sBatchRecord.primary_key.device_id));
	memcpy (pCUPCurrentPage->sBatchRecord.primary_key.batch_nbr , "ACTIVE",sizeof (pCurrentPage->sBatchRecord.primary_key.batch_nbr));
	memcpy ( pCUPCurrentPage->sBatchRecord.merchant_id, m_StrMerchantIDFld, sizeof (pCurrentPage->sBatchRecord.merchant_id));
	if ( !pCurrentPage->m_bCurrentBatch )
		memcpy ( &pCurrentPage->sTerminalRecord, &sTerminalRecord, sizeof ( DCF01));
	else		
   {
      pCurrentPage->PopulateCurrentBatchDialogFromDatabase(sTerminalRecord);
		memcpy ( &pCurrentPage->sTerminalRecord, &sTerminalRecord, sizeof ( DCF01));

   }
	if ( !pCUPCurrentPage->m_bCurrentBatch )
		memcpy ( &pCUPCurrentPage->sTerminalRecord, &sTerminalRecord, sizeof ( DCF01));
		else		
   {
      pCUPCurrentPage->PopulateCurrentBatchDialogFromDatabase(sTerminalRecord);
		memcpy ( &pCUPCurrentPage->sTerminalRecord, &sTerminalRecord, sizeof ( DCF01));

   }

   ZeroMemory ( &pBatchHistoryPage->sBatchRecord, sizeof ( BCH01) );
	memcpy ( pBatchHistoryPage->sBatchRecord.primary_key.device_id, m_StrTerminalIDFld, sizeof (pBatchHistoryPage->sBatchRecord.primary_key.device_id));
	memcpy (pBatchHistoryPage->sBatchRecord.primary_key.batch_nbr , " ",sizeof (pBatchHistoryPage->sBatchRecord.primary_key.batch_nbr));
	memcpy (pBatchHistoryPage->sBatchRecord.merchant_id , m_StrMerchantIDFld,sizeof (pBatchHistoryPage->sBatchRecord.merchant_id));
	memcpy (pBatchHistoryPage->sBatchRecord.open_date , " ",sizeof (pBatchHistoryPage->sBatchRecord.open_date));
	memcpy (pBatchHistoryPage->sBatchRecord.open_time , " ",sizeof (pBatchHistoryPage->sBatchRecord.open_time));
	if ( !pBatchHistoryPage->m_bBatchHistory )	
		memcpy ( &pBatchHistoryPage->sTerminalRecord, &sTerminalRecord, sizeof ( DCF01));
	else
		pBatchHistoryPage->PopulateBatchHistoryDialog(sTerminalRecord);

/*	pKeysPage->m_StrMerchantIDFld = m_StrMerchantIDFld;
    pKeysPage->m_StrOrganizationIDFld = m_StrOrganizationIDFld;
	pKeysPage->m_StrTerminalIDFld = m_StrTerminalIDFld;*/

	pKeysPage->strPinFormat = sTerminalRecord.pin_block_format;
	pKeysPage->strEncryptionMethod = sTerminalRecord.encryption_method;
	if ( !pKeysPage->m_bKeys )
		memcpy ( &pKeysPage->sTerminalRecordOld, &sTerminalRecord, sizeof ( DCF01));
	else
		pKeysPage->PopulateKeysDialogFromDatabase(sTerminalRecord);	

	if ( pAutoInitPage->m_bAutoInit )
		pAutoInitPage->PopulateAutoInitDialogFromDatabase(sTerminalRecord);

	memcpy ( &pAutoInitPage->sTerminalRecordOld, &sTerminalRecord, sizeof(DCF01));
	
}

void CMerchantTerminalConfig::OnCancel() 
{
		if ( pGeneralPage->m_bGeneral )
	{
	pGeneralPage->CleanGeneralDialog();
	pGeneralPage->EnableGeneralDialog(FALSE);
	}
	if ( pCurrentPage->m_bCurrentBatch )
	{
	pCurrentPage->CleanCurrentBatchDialog();
//		pCurrentPage->EnableCurrentDialog( FALSE );
	}
	if ( pCUPCurrentPage->m_bCurrentBatch )
	{
	pCUPCurrentPage->CleanCurrentBatchDialog();
//		pCurrentPage->EnableCurrentDialog( FALSE );
	}
	if ( pBatchHistoryPage->m_bBatchHistory )
	{
	pBatchHistoryPage->CleanBatchHistoryDialog();
//		pBatchHistoryPage->EnableBatchHistoryDialog( FALSE );
	}
	if ( pKeysPage->m_bKeys)
	{
	pKeysPage->CleanKeysDialog();
	pKeysPage->EnableKeysDialog( FALSE );
	}
	if ( pTranPage->m_bTran)
	{
	pTranPage->CleanTranDialog(0);
	pTranPage->EnableTranDialog( FALSE );
	}

	if ( pAutoInitPage->m_bAutoInit )
	{
		pAutoInitPage->CleanAutoInitDialog();	
		pAutoInitPage->EnableAutoInitDialog( FALSE );
	}

	
	CPropertyPage::OnCancel();
}

BOOL CMerchantTerminalConfig::OnApply() 
{
	DCF01 sTerminalDatabaseStructure;
	char strErrorMessage[200];
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
	if ( m_nDialogMode == MODE_VIEW)
	{
		((CPropertySheet*) this->GetParent())->SetActivePage(0);
		//return CPropertyPage::OnApply();
      return FALSE;
	}

	if ( m_nDialogMode != MODE_VIEW && m_nDialogMode != MODE_DONE)
	{
		if ( myUtilsObject.IsEmpty ( pGeneralPage->m_StrCategoryCodeFld, this))
		{
			((CPropertySheet*) this->GetParent())->SetActivePage(1);
			pGeneralPage->m_CntlCategoryCodeFld.SetFocus();
			pGeneralPage->m_StrMessageLbl = "Please enter Category Code";
			pGeneralPage->UpdateData(FALSE);
			return FALSE;
		}	

		if ( myUtilsObject.IsEmpty ( pGeneralPage->m_StrShortNameFld, this))
		{
			((CPropertySheet*) this->GetParent())->SetActivePage(1);
			pGeneralPage->m_CntlShortNameFld.SetFocus();
			pGeneralPage->m_StrMessageLbl = "Please enter Short Name";
			pGeneralPage->UpdateData(FALSE);
			return FALSE;
		}	
		if ( myUtilsObject.IsEmpty ( pGeneralPage->m_StrIndustryCodeFld, this))
		{
			((CPropertySheet*) this->GetParent())->SetActivePage(1);
			pGeneralPage->m_CntlIndustryCodeFld.SetFocus();
			pGeneralPage->m_StrMessageLbl = "Please enter Industry Code";
			pGeneralPage->UpdateData(FALSE);
			return FALSE;
		}		
		
		
	}
	

	if (m_nDialogMode == MODE_ADD)
	{			
		memset(&sTerminalDatabaseStructure, 0, sizeof(DCF01)) ;
		if (!PopulateTerminalDatabaseStructureFromDialog (&sTerminalDatabaseStructure)) {		
			return FALSE;
		}

		int rcode = txdsapi_insert_record ( (pBYTE)&sTerminalDatabaseStructure, 
											 sizeof (DCF01), DCF01_DATA, strErrorMessage );
		if ( rcode != PTEMSG_OK )
		{  	
			m_StrMessageLbl = strErrorMessage;
			m_nDialogMode = MODE_DONE;
			if (!UpdateData(FALSE)){
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}
			((CPropertySheet*) this->GetParent())->SetActivePage(0);
			return FALSE;
		}
		else 
		{            
			memcpy ((LPSTR)m_strCurrentData, "Record Added", sizeof (m_strCurrentData));		
			PopulateHistoryTable();
			((CPropertySheet*) this->GetParent())->SetActivePage(0);
			m_StrMessageLbl = "Record Added";	
			if (!UpdateData(FALSE)){
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}
			if ( pGeneralPage->m_bGeneral )
			{
			pGeneralPage->CleanGeneralDialog();
			pGeneralPage->EnableGeneralDialog(FALSE);
			}
			if ( pCurrentPage->m_bCurrentBatch )
			{
			pCurrentPage->CleanCurrentBatchDialog();
		//		pCurrentPage->EnableCurrentDialog( FALSE );
			}
			if ( pCUPCurrentPage->m_bCurrentBatch )
			{
			pCUPCurrentPage->CleanCurrentBatchDialog();
		//		pCurrentPage->EnableCurrentDialog( FALSE );
			}
			if ( pBatchHistoryPage->m_bBatchHistory )
			{
			pBatchHistoryPage->CleanBatchHistoryDialog();
		//		pBatchHistoryPage->EnableBatchHistoryDialog( FALSE );
			}
			if ( pKeysPage->m_bKeys)
			{
			pKeysPage->CleanKeysDialog();
			pKeysPage->EnableKeysDialog( FALSE );
			}

			if ( pTranPage->m_bTran)
			{
			pTranPage->CleanTranDialog(0);
			pTranPage->EnableTranDialog( FALSE );
			}
			
			if (pAutoInitPage->m_bAutoInit)
			{
				pAutoInitPage->CleanAutoInitDialog();
				pAutoInitPage->EnableAutoInitDialog(FALSE);
			}
			
			if ( pChangeHistoryPage->m_bChangeHistory )
				pChangeHistoryPage->CleanChangeHistory();

			SetModified ( FALSE );

			m_nDialogMode = MODE_DONE;
			pGeneralPage->m_nDialogMode = MODE_DONE;
			pCurrentPage->m_nDialogMode = MODE_DONE;
			pBatchHistoryPage->m_nDialogMode = MODE_DONE;
			pKeysPage->m_nDialogMode = MODE_DONE;
			pTranPage->m_nDialogMode = MODE_DONE;
			pAutoInitPage->m_nDialogMode = MODE_DONE;
			pChangeHistoryPage->m_nDialogMode = MODE_DONE;
			
			m_CntlMerchantIDFld.SetFocus();
			m_CntlMerchantIDFld.EnableWindow(TRUE);
			m_CntlOrganizationIDFld.EnableWindow(TRUE);
			m_CntlTerminalIDFld.EnableWindow(TRUE);

			//return CPropertyPage::OnApply();
         return FALSE;
		} 
	}
//////////////////////////////////////////////////////////////////////////////////////////
	if (m_nDialogMode == MODE_EDIT)
	{ 
        memset(&sTerminalDatabaseStructure, 0, sizeof(DCF01)) ;

		PopulateTerminalDatabaseStructureFromDialog (&sTerminalDatabaseStructure );

		int rcode = txdsapi_update_record ( (pBYTE)&sTerminalDatabaseStructure,sizeof (DCF01), DCF01_DATA,  strErrorMessage );
		if ( rcode != PTEMSG_OK )
		{ 		
			m_StrMessageLbl = strErrorMessage;
			m_nDialogMode = MODE_DONE;
			((CPropertySheet*) this->GetParent())->SetActivePage(0);
			((CPropertySheet*) this->GetParent())->SetWindowText("Device Definition");
			//set version info in the title bar
			CString appProdVer;
			CString lpszTitle;
			appProdVer.LoadString(IDS_PRODUCT_VERSION);
			((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
			lpszTitle = lpszTitle + " - Version - ";
			lpszTitle += appProdVer;
			((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);

			if (!UpdateData(FALSE)){
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}
			return FALSE;
		}
		else
		{
			m_StrMessageLbl = "Record Updated";
			if (!UpdateData(FALSE)){
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}
			pGeneralPage->CompareStructures();
			if ( pKeysPage->m_bKeys )
				pKeysPage->CompareStructures();

			if ( pTranPage->m_bTran )
				pTranPage->CompareStructures(sTerminalDatabaseStructure);

			if( pAutoInitPage->m_bAutoInit )
				pAutoInitPage->CompareStructures(sTerminalDatabaseStructure);


			if ( pGeneralPage->m_bGeneral )
			{
				pGeneralPage->CleanGeneralDialog();
				pGeneralPage->EnableGeneralDialog(FALSE);
			}
			pCurrentPage->CleanCurrentBatchDialog();
			pCUPCurrentPage->CleanCurrentBatchDialog();
			pBatchHistoryPage->CleanBatchHistoryDialog();

			if ( pKeysPage->m_bKeys)
			{
				pKeysPage->CleanKeysDialog();
				pKeysPage->EnableKeysDialog( FALSE );
			}

			if ( pTranPage->m_bTran)
			{
				pTranPage->CleanTranDialog(0);
				pTranPage->EnableTranDialog( FALSE );
			}

			if ( pAutoInitPage->m_bAutoInit)
			{
				pAutoInitPage->CleanAutoInitDialog();
				pAutoInitPage->EnableAutoInitDialog( FALSE );
			}
			if ( pChangeHistoryPage->m_bChangeHistory )
				pChangeHistoryPage->CleanChangeHistory();

			((CPropertySheet*) this->GetParent())->SetActivePage(0);
			((CPropertySheet*) this->GetParent())->SetWindowText("Device Definition");
			//set version info in the title bar
			CString appProdVer;
			CString lpszTitle;
			appProdVer.LoadString(IDS_PRODUCT_VERSION);
			((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
			lpszTitle = lpszTitle + " - Version - ";
			lpszTitle += appProdVer;
			((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);

			m_CntlMerchantIDFld.EnableWindow(TRUE);
			m_CntlOrganizationIDFld.EnableWindow(TRUE);
			m_CntlTerminalIDFld.EnableWindow(TRUE);

			SetModified ( FALSE );

			m_nDialogMode = MODE_DONE;
			pGeneralPage->m_nDialogMode = MODE_DONE;
			pCurrentPage->m_nDialogMode = MODE_DONE;
			pBatchHistoryPage->m_nDialogMode = MODE_DONE;
			pKeysPage->m_nDialogMode = MODE_DONE;
			pTranPage->m_nDialogMode = MODE_DONE;
			pAutoInitPage->m_nDialogMode = MODE_DONE;
			pChangeHistoryPage->m_nDialogMode = MODE_DONE;

			//return CPropertyPage::OnApply();
         return FALSE;
		}
	
    }
	m_nDialogMode = MODE_DONE;
	
	//return CPropertyPage::OnApply();
   return FALSE;
}

BOOL CMerchantTerminalConfig::PopulateTerminalDatabaseStructureFromDialog ( pDCF01 psTerminalRecord )
{
	ZeroMemory (psTerminalRecord, sizeof(DCF01)) ;
	
	if ( m_nDialogMode == MODE_EDIT )
		memcpy ( psTerminalRecord, &sTerminalRecordOld, sizeof (DCF01));
    
	memcpy ((LPTSTR) psTerminalRecord->merchant_id, m_StrMerchantIDFld, sizeof (psTerminalRecord->merchant_id));
	memcpy ((LPTSTR) psTerminalRecord->organization_id,m_StrOrganizationIDFld, sizeof (psTerminalRecord->organization_id));
	memcpy ((LPTSTR) psTerminalRecord->primary_key.device_id,m_StrTerminalIDFld, sizeof (psTerminalRecord->primary_key.device_id));

	pGeneralPage->PopulateDatabaseFromGeneralDialog ( psTerminalRecord );

   if ( Reset_Batch_Nbr == 1 )
   {
      /* Set batch number to Null. */
      memset( psTerminalRecord->batch_nbr, 0x00, sizeof(psTerminalRecord->batch_nbr) );
      Reset_Batch_Nbr = 0;
   }

   if ( pTranPage->m_bTran)
		pTranPage->PopulateDatabaseFromTranDialog ( psTerminalRecord );
	else
	{
		// if they did not open this page - by default set this field to 111...
		if ( m_nDialogMode == MODE_ADD )
			memset (psTerminalRecord->valid_transactions, 49, sizeof (psTerminalRecord->valid_transactions)-1);
	}

	if ( pKeysPage->m_bKeys )
		pKeysPage->PopulateDatabaseFromKeysDialog ( psTerminalRecord );

	if (pAutoInitPage->m_bAutoInit )
		pAutoInitPage->PopulateDatabaseFromAutoInitDialog (psTerminalRecord );

	

	return TRUE;
}

BOOL CMerchantTerminalConfig::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}
