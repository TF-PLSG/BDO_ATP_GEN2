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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\PanMainPage.cpp  $
   
      Rev 1.14   Dec 01 2003 11:30:00   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.13   Jul 10 2002 09:56:38   lmitchel
   Ver:4.0.3.1:  Added SendMessage command in PropertySheet files (on_init function) to set the Apply button as the default.  If the enter key is pressed, action will default to  the apply button instead of the OK button.  This allows the user to remain in the current dialog instead of exiting to the main menu.
   
      Rev 1.12   May 21 2002 13:42:16   dirby
   Modified to not exit when <enter> is pressed.  If OK is selected,
   the database gets updated still, but screen is not exited.
   SCR 587
   
      Rev 1.11   Mar 21 2000 12:28:08   iarustam
   scr 507
   
      Rev 1.10   Mar 16 2000 10:39:14   iarustam
   Added alpha lookup screens and updated
   Merchant main, Card Record main and 
   primary account main screens
   
      Rev 1.9   Jan 25 2000 14:00:46   iarustam
   bug fixes
   
      Rev 1.8   Dec 23 1999 16:03:24   mbysani
   modified based on the documents
   
      Rev 1.3   Sep 30 1999 16:17:38   npogosya
   Bug fixes
   
      Rev 1.1   Sep 22 1999 15:57:38   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:06   iarustam
   Initial Release
   

    
*
************************************************************************************/
// PanMainPage.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "PanMainPage.h"
#include "BinList.h"
#include "MyUtils.h"
#include "AlphaLookup.h"
//#include "Login.h"
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
extern CString name;
extern CSharedMem shrmem;
//extern CLogin login;
/////////////////////////////////////////////////////////////////////////////
// CPanMainPage property page

IMPLEMENT_DYNCREATE(CPanMainPage, CPropertyPage)

CPanMainPage::CPanMainPage() : CPropertyPage(CPanMainPage::IDD)
{
	//{{AFX_DATA_INIT(CPanMainPage)
	m_panselected = _T("");
	m_StrMessageLbl = _T("");
	//}}AFX_DATA_INIT
}

CPanMainPage::~CPanMainPage()
{
}

void CPanMainPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPanMainPage)
	DDX_Control(pDX, IDC_SEARCH, m_Search);
	DDX_Control(pDX, IDC_VIEW, m_View);
	DDX_Control(pDX, IDC_UPDATE, m_Update);
	DDX_Control(pDX, IDC_LIST, m_List);
	DDX_Control(pDX, IDC_DELETE, m_Delete);
	DDX_Control(pDX, IDC_ADD, m_Add);
	DDX_Control(pDX, IDC_PAN, m_pan);
	DDX_Control(pDX, IDC_COMMENTS, m_ComEdit);
	DDX_Text(pDX, IDC_PAN, m_panselected);
	DDV_MaxChars(pDX, m_panselected, 19);
	DDX_Text(pDX, IDC_COMMENTS, m_StrMessageLbl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPanMainPage, CPropertyPage)
	//{{AFX_MSG_MAP(CPanMainPage)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_VIEW, OnView)
	ON_BN_CLICKED(IDC_UPDATE, OnUpdate)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_LIST, OnList)
	ON_EN_CHANGE(IDC_PAN, OnChangePan)
	ON_BN_CLICKED(IDC_SEARCH, OnSearch)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPanMainPage message handlers

void CPanMainPage::OnAdd() 
{
	if (!GetPAFData(MODE_ADD))
		return;

	SetModes(MODE_ADD);
	SetPanEdits();
	m_pSheet->SetActivePage(1);
	m_pSheet->SetWindowText("Add Primary Account");
	//set version info in the title bar
	CString appProdVer;
	CString lpszTitle;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	m_pSheet->GetWindowText(lpszTitle);
	lpszTitle = lpszTitle + " - Version - ";
	lpszTitle += appProdVer;
	m_pSheet->SetWindowText(lpszTitle);

}

BOOL CPanMainPage::PanEntered()
{
	UpdateData(TRUE);
	if (m_panselected.IsEmpty())
	{
		m_ComEdit.SetWindowText("Please enter Primary Account Number");
		m_pan.SetFocus();
		return FALSE;
	}

	return TRUE;
}

void CPanMainPage::SetPanEdits()
{
	pProcPage->m_panstr = m_panselected;
	pDemoPage->m_panstr = m_panselected;
	pTxPage->m_panstr = m_panselected;

}

BOOL CPanMainPage::OnSetActive() 
{
	UpdateData(TRUE);

/*	if (DialogMode == MODE_ADD || DialogMode == MODE_EDIT)
	{
		m_pan.EnableWindow(FALSE);
		m_View.EnableWindow(FALSE);
		m_Update.EnableWindow(FALSE);
		m_List.EnableWindow(FALSE);
		m_Delete.EnableWindow(FALSE);
		m_Add.EnableWindow(FALSE);
	}
	else
	{
		m_pan.EnableWindow(TRUE);
		m_View.EnableWindow(TRUE);
		m_Update.EnableWindow(TRUE);
		m_List.EnableWindow(TRUE);
		m_Delete.EnableWindow(TRUE);
		m_Add.EnableWindow(TRUE);
	}*/

	return CPropertyPage::OnSetActive();
}

BOOL CPanMainPage::OnInitDialog() 
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


	m_pan.SetWindowText("");
	
/*	cntr = 0;
   
   timer_handle = StartTimer(1000);	   

    if (timer_handle == 0)
    {
          AfxMessageBox("Unable to obtain timer");
    }
*/

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CPanMainPage::OnView() 
{

	if (!GetPAFData(MODE_VIEW))
		return;

	SetPanEdits();
	if (pTxPage->m_bTxPage)
		pTxPage->PopulateTxDialog();
	PopulateHistPage();

	SetModes(MODE_VIEW);
	m_pSheet->SetActivePage(1);
	m_pSheet->SetWindowText("View Primary Account");
	//set version info in the title bar
	CString appProdVer;
	CString lpszTitle;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	m_pSheet->GetWindowText(lpszTitle);
	lpszTitle = lpszTitle + " - Version - ";
	lpszTitle += appProdVer;
	m_pSheet->SetWindowText(lpszTitle);

	
}

BOOL CPanMainPage::GetPAFData(int DlgMode)
{
	char ErrorMsg[500];
	PAF01	sPAFRecord;
	BOOL  retvalue = FALSE;

	if (! PanEntered())
		return FALSE;

	ZeroMemory(&sPAFRecord,sizeof(PAF01)) ;

    memcpy ((LPSTR)(sPAFRecord.primary_key.primary_acct_nbr), m_panselected , sizeof (sPAFRecord.primary_key.primary_acct_nbr));

	int rcode = txdsapi_get_record( (pBYTE)&sPAFRecord, sizeof(PAF01), PAF01_DATA, ErrorMsg);
	if( rcode == PTEMSG_OK )
	{
		switch (DlgMode)
		{
		case MODE_ADD:
			{
				m_ComEdit.SetWindowText("ERROR: Primary Account already exists");
				m_pan.SetFocus();
				retvalue = FALSE;
				break;
			}
		case MODE_EDIT:
		case MODE_VIEW:
			{
				// Save retrieved record in sOldPAFRecord for use in change history table.
				ZeroMemory(&sOldPAFRecord, sizeof(PAF01));
				memcpy(&sOldPAFRecord, &sPAFRecord, sizeof(PAF01));

				// Populate all the PAF fields
				pProcPage->m_strVipProc.Format((LPCTSTR)sPAFRecord.caf_profile_id);
				pProcPage->m_strAccStatus.Format((LPCTSTR)sPAFRecord.status);
				pProcPage->m_strType.Format((LPCTSTR)sPAFRecord.type);
				pProcPage->m_StrAccountNameFld.Format((LPCTSTR)sPAFRecord.account_name);
				pProcPage->m_StrBlockStatusFld.Format((LPCTSTR)sPAFRecord.block_status);
				pProcPage->m_StrCorpAccountNumFld.Format((LPCTSTR)sPAFRecord.corp_acct_nbr);
				pProcPage->m_StrMinPayFld.Format((LPCTSTR)sPAFRecord.min_pay_code);
				pProcPage->m_StrPastAmountFld.Format((LPCTSTR)sPAFRecord.past_due_amount);
//				pProcPage->m_StrRelatedAccountNbrFld.Format((LPCTSTR)sPAFRecord.related_acct_nbr);
				pProcPage->m_StrCustrNbr.Format("%s", sPAFRecord.Custr_nbr);
				pProcPage->m_StrCreditLimit.Format("%s", sPAFRecord.Credit_limit);
				pProcPage->m_StrAppSource.Format("%s", sPAFRecord.APP_Source);
				pProcPage->m_StrCompName.Format("%s", sPAFRecord.COMP_Name);

/*				if ( strcmp ( (CHAR*)sPAFRecord.related_acct_type, "03" ) == 0 )
					pProcPage->m_StrRelatedAccountType = "Credit";*/

				pDemoPage->m_strName.Format((LPCTSTR)sPAFRecord.name_dba);
				pDemoPage->m_strAddr1.Format((LPCTSTR)sPAFRecord.address1);
				pDemoPage->m_strAddr2.Format((LPCTSTR)sPAFRecord.address2);
				pDemoPage->m_strCity.Format((LPCTSTR)sPAFRecord.city);
				pDemoPage->m_strState.Format((LPCTSTR)sPAFRecord.state);
				pDemoPage->m_strZip.Format((LPCTSTR)sPAFRecord.zip_code);
				pDemoPage->m_strHomePhone.Format((LPCTSTR)sPAFRecord.home_phone);
				pDemoPage->m_strWorkPhone.Format((LPCTSTR)sPAFRecord.work_phone);

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
			m_ComEdit.SetWindowText("ERROR: Primary Account does not exist");
			m_pan.SetFocus();
			retvalue = FALSE;
		}
		else
		{
			retvalue = TRUE;
		}

	}



	return retvalue;
}

void CPanMainPage::OnUpdate() 
{
	if (!GetPAFData(MODE_EDIT))
		return;
	
	SetPanEdits();
	if (pTxPage->m_bTxPage)
		pTxPage->PopulateTxDialog();
	PopulateHistPage();
	SetModes(MODE_EDIT);
	m_pSheet->SetActivePage(1);
	m_pSheet->SetWindowText("Update Primary Account");
	//set version info in the title bar
	CString appProdVer;
	CString lpszTitle;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	m_pSheet->GetWindowText(lpszTitle);
	lpszTitle = lpszTitle + " - Version - ";
	lpszTitle += appProdVer;
	m_pSheet->SetWindowText(lpszTitle);

	
}

void CPanMainPage::OnDelete() 
{
	CString strMessage;
	PAF01	sPAFRecord;
    int nReturnCode;
	char strErrorMessage[200];

	if (!GetPAFData(MODE_DELETE))
		return;

	strMessage.Format ("Are you sure you want to delete %s ?", (LPCTSTR) m_panselected) ;
	nReturnCode = AfxMessageBox ((LPCTSTR)strMessage, MB_OKCANCEL|MB_ICONQUESTION) ;
	if (nReturnCode != IDOK) return ;
	
	memcpy ((LPTSTR)sPAFRecord.primary_key.primary_acct_nbr, m_panselected, sizeof (sPAFRecord.primary_key.primary_acct_nbr)) ;

    int rcode = txdsapi_delete_record ((pBYTE)&sPAFRecord, sizeof (PAF01), PAF01_DATA, strErrorMessage);
    if(rcode != PTEMSG_OK )    
     {
		m_ComEdit.SetWindowText(strErrorMessage);
		m_pan.SetFocus();
		return ;
	 }

	else
     {

 		// Add record to the change history table
		memcpy ((LPSTR)m_strCurrentData, "Record Deleted", sizeof (m_strCurrentData));
      m_pan.SetWindowText("");
        PopulateHistoryTable();

		if ( pChHistPage->m_bChangeHistory )
			pChHistPage->CleanChangeHistory();

		m_ComEdit.SetWindowText("Record Deleted");
     }

	SetModes(MODE_DONE);
}

void CPanMainPage::PopulateHistoryTable()
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
    ZeroMemory ( &sHistoryData, sizeof (HST01));
    memcpy ((LPTSTR) sHistoryData.file_name,     (LPCTSTR)"PAF01", sizeof (sHistoryData.file_name));
    temp = strDate;
    temp = temp.Right(4) + temp.Left(2) + temp.Mid(2,2);
    
    CHAR wstation[21]; DWORD wsize;

    wsize = sizeof( wstation );
    GetComputerName( wstation, &wsize );

	memcpy ((LPTSTR)sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
    memcpy ((LPTSTR) sHistoryData.date_modified, temp, sizeof (sHistoryData.date_modified));
    memcpy ((LPTSTR) sHistoryData.file_primary_key,  (LPCTSTR)m_panselected, sizeof (sHistoryData.file_primary_key));
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

 
/**************************************************************/

BOOL CPanMainPage::OnApply() 
{
	char ErrorMsg[200];
	PAF01	sPAFRecord;
	CAF01	sCAFRecord;
	CSF01   sCSFRecord;
	int   rcode;
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
  if ( DialogMode != MODE_DONE )
  {
	m_pSheet->SetActivePage(2);
//	m_pSheet->SetActivePage(1);
	m_pSheet->SetActivePage(0);

	pProcPage->UpdateData(TRUE);
	pDemoPage->UpdateData(TRUE);

	if (!PanEntered())
		return FALSE;
	
    if ( !myUtilsObject.IsEmpty( pProcPage->m_strVipProc, this ) &&
		 !myUtilsObject.IsEmpty( pProcPage->m_strType, this ) )
	{
		ZeroMemory(&sCAFRecord,sizeof(CAF01));
		memcpy ((LPTSTR) sCAFRecord.primary_key.caf_profile_id,(LPCTSTR)pProcPage->m_strVipProc, sizeof (sCAFRecord.primary_key.caf_profile_id));
		memcpy ((LPTSTR) sCAFRecord.primary_key.type,(LPCTSTR)pProcPage->m_strType, sizeof (sCAFRecord.primary_key.type));

		rcode = txdsapi_get_record((pBYTE) &sCAFRecord,sizeof(CAF01),CAF01_DATA,ErrorMsg);
		if(rcode == PTEMSG_NOT_FOUND )    
		 {
			pProcPage->m_strComEdit = "ERROR: Invalid VIP Profile";
			m_pSheet->SetActivePage(1);		
			return FALSE;
		 }
		else if(rcode != PTEMSG_OK )    
			 {
				m_ComEdit.SetWindowText(ErrorMsg);
				return FALSE;
			 }
	}
	ZeroMemory(&sCSFRecord,sizeof(CSF01));
    memcpy ((LPTSTR) sCSFRecord.primary_key.status,(LPCTSTR)pProcPage->m_strAccStatus, sizeof (sCSFRecord.primary_key.status));

	rcode = txdsapi_get_record((pBYTE) &sCSFRecord,sizeof(CSF01),CSF01_DATA,ErrorMsg);
	if(rcode == PTEMSG_NOT_FOUND )    
	 {
		pProcPage->m_strComEdit = "ERROR: Invalid Card Status";
		m_pSheet->SetActivePage(1);
		pProcPage->m_AccStatus.SetFocus();
		return FALSE;
	 }
	else if(rcode != PTEMSG_OK )    
		 {
			m_ComEdit.SetWindowText(ErrorMsg);
			return FALSE;
		 }





	switch (DialogMode)
	{
	case MODE_ADD:
		{
		PopulatePAF01(&sPAFRecord);
			rcode = txdsapi_insert_record((pBYTE)&sPAFRecord,sizeof(PAF01),PAF01_DATA, ErrorMsg);
			if(rcode != PTEMSG_OK )    
			 {
				m_ComEdit.SetWindowText(ErrorMsg);
				m_pan.SetFocus();
				return FALSE;
			 }
			else
			{
				// Add record to the change history table
				memcpy ((LPSTR)m_strCurrentData, "Record Added", sizeof (m_strCurrentData));
			    PopulateHistoryTable();
				m_pan.SetFocus();

				SetModes(MODE_DONE);
				m_ComEdit.SetWindowText("Record Added");
				((CPropertySheet*) this->GetParent())->SetActivePage(0);
				CleanPages();
			}

			

			break;
		}

	case MODE_EDIT:
		{
			PopulatePAF01(&sPAFRecord);
			rcode = txdsapi_update_record((pBYTE)&sPAFRecord,sizeof(PAF01),PAF01_DATA, ErrorMsg);
			if(rcode != PTEMSG_OK )    
			 {
				m_ComEdit.SetWindowText(ErrorMsg);
				return FALSE;
			 }
			else
			{
				// Add record to the change history table
/*				if (strcmp((char *)sOldPAFRecord.related_acct_nbr, (char *)sPAFRecord.related_acct_nbr) !=0)
				{
					memcpy(m_strCurrentData, sPAFRecord.related_acct_nbr, sizeof(m_strCurrentData));
					memcpy(m_strPreviousData, sOldPAFRecord.related_acct_nbr, sizeof(m_strPreviousData));
					memcpy(m_strFieldTag, "Related Account Number", sizeof(m_strFieldTag));

					PopulateHistoryTable();
				}
				if (strcmp((char *)sOldPAFRecord.related_acct_type, (char *)sPAFRecord.related_acct_type) !=0)
				{
					memcpy(m_strCurrentData, sPAFRecord.related_acct_type, sizeof(m_strCurrentData));
					memcpy(m_strPreviousData, sOldPAFRecord.related_acct_type, sizeof(m_strPreviousData));
					memcpy(m_strFieldTag, "Related Account Type", sizeof(m_strFieldTag));

					PopulateHistoryTable();
				}*/
				if (strcmp((char *)sOldPAFRecord.name_dba, (char *)sPAFRecord.name_dba) !=0)
				{
					memcpy(m_strCurrentData, sPAFRecord.name_dba, sizeof(m_strCurrentData));
					memcpy(m_strPreviousData, sOldPAFRecord.name_dba, sizeof(m_strPreviousData));
					memcpy(m_strFieldTag, "Name", sizeof(m_strFieldTag));

					PopulateHistoryTable();
				}

				if (strcmp((char *)sOldPAFRecord.block_status, (char *)sPAFRecord.block_status) !=0)
				{
					memcpy(m_strCurrentData, sPAFRecord.block_status, sizeof(m_strCurrentData));
					memcpy(m_strPreviousData, sOldPAFRecord.block_status, sizeof(m_strPreviousData));
					memcpy(m_strFieldTag, "Block Status", sizeof(m_strFieldTag));

					PopulateHistoryTable();
				}
				if (strcmp((char *)sOldPAFRecord.corp_acct_nbr, (char *)sPAFRecord.corp_acct_nbr) !=0)
				{
					memcpy(m_strCurrentData, sPAFRecord.corp_acct_nbr, sizeof(m_strCurrentData));
					memcpy(m_strPreviousData, sOldPAFRecord.corp_acct_nbr, sizeof(m_strPreviousData));
					memcpy(m_strFieldTag, "Corporate Account No.", sizeof(m_strFieldTag));

					PopulateHistoryTable();
				}
				if (strcmp((char *)sOldPAFRecord.account_name, (char *)sPAFRecord.account_name) !=0)
				{
					memcpy(m_strCurrentData, sPAFRecord.account_name, sizeof(m_strCurrentData));
					memcpy(m_strPreviousData, sOldPAFRecord.account_name, sizeof(m_strPreviousData));
					memcpy(m_strFieldTag, "Account Name", sizeof(m_strFieldTag));

					PopulateHistoryTable();
				}
				if (strcmp((char *)sOldPAFRecord.address1, (char *)sPAFRecord.address1) !=0)
				{
					memcpy(m_strCurrentData, sPAFRecord.address1, sizeof(m_strCurrentData));
					memcpy(m_strPreviousData, sOldPAFRecord.address1, sizeof(m_strPreviousData));
					memcpy(m_strFieldTag, "Address1", sizeof(m_strFieldTag));

					PopulateHistoryTable();
				}

				if (strcmp((char *)sOldPAFRecord.address2, (char *)sPAFRecord.address2) !=0)
				{
					memcpy(m_strCurrentData, sPAFRecord.address2, sizeof(m_strCurrentData));
					memcpy(m_strPreviousData, sOldPAFRecord.address2, sizeof(m_strPreviousData));
					memcpy(m_strFieldTag, "Address2", sizeof(m_strFieldTag));

					PopulateHistoryTable();
				}

				if (strcmp((char *)sOldPAFRecord.city, (char *)sPAFRecord.city) !=0)
				{
					memcpy(m_strCurrentData, sPAFRecord.city, sizeof(m_strCurrentData));
					memcpy(m_strPreviousData, sOldPAFRecord.city, sizeof(m_strPreviousData));
					memcpy(m_strFieldTag, "City", sizeof(m_strFieldTag));

					PopulateHistoryTable();
				}

				if (strcmp((char *)sOldPAFRecord.state, (char *)sPAFRecord.state) !=0)
				{
					memcpy(m_strCurrentData, sPAFRecord.state, sizeof(m_strCurrentData));
					memcpy(m_strPreviousData, sOldPAFRecord.state, sizeof(m_strPreviousData));
					memcpy(m_strFieldTag, "State", sizeof(m_strFieldTag));

					PopulateHistoryTable();
				}

				if (strcmp((char *)sOldPAFRecord.zip_code, (char *)sPAFRecord.zip_code) !=0)
				{
					memcpy(m_strCurrentData, sPAFRecord.zip_code, sizeof(m_strCurrentData));
					memcpy(m_strPreviousData, sOldPAFRecord.zip_code, sizeof(m_strPreviousData));
					memcpy(m_strFieldTag, "Zip_code", sizeof(m_strFieldTag));

					PopulateHistoryTable();
				}

				if (strcmp((char *)sOldPAFRecord.home_phone, (char *)sPAFRecord.home_phone) !=0)
				{
					memcpy(m_strCurrentData, sPAFRecord.home_phone, sizeof(m_strCurrentData));
					memcpy(m_strPreviousData, sOldPAFRecord.home_phone, sizeof(m_strPreviousData));
					memcpy(m_strFieldTag, "Home_phone", sizeof(m_strFieldTag));

					PopulateHistoryTable();
				}

				if (strcmp((char *)sOldPAFRecord.work_phone, (char *)sPAFRecord.work_phone) !=0)
				{
					memcpy(m_strCurrentData, sPAFRecord.work_phone, sizeof(m_strCurrentData));
					memcpy(m_strPreviousData, sOldPAFRecord.work_phone, sizeof(m_strPreviousData));
					memcpy(m_strFieldTag, "Work_phone", sizeof(m_strFieldTag));

					PopulateHistoryTable();
				}

				if (strcmp((char *)sOldPAFRecord.status, (char *)sPAFRecord.status) !=0)
				{
					memcpy(m_strCurrentData, sPAFRecord.status, sizeof(m_strCurrentData));
					memcpy(m_strPreviousData, sOldPAFRecord.status, sizeof(m_strPreviousData));
					memcpy(m_strFieldTag, "Status", sizeof(m_strFieldTag));

					PopulateHistoryTable();
				}

				if (strcmp((char *)sOldPAFRecord.caf_profile_id, (char *)sPAFRecord.caf_profile_id) !=0)
				{
					memcpy(m_strCurrentData, sPAFRecord.caf_profile_id, sizeof(m_strCurrentData));
					memcpy(m_strPreviousData, sOldPAFRecord.caf_profile_id, sizeof(m_strPreviousData));
					memcpy(m_strFieldTag, "VIP Profile", sizeof(m_strFieldTag));

					PopulateHistoryTable();
				}

				if (strcmp((char *)sOldPAFRecord.Credit_limit, (char *)sPAFRecord.Credit_limit) !=0)
				{
					memcpy(m_strCurrentData, sPAFRecord.Credit_limit, sizeof(m_strCurrentData));
					memcpy(m_strPreviousData, sOldPAFRecord.Credit_limit, sizeof(m_strPreviousData));
					memcpy(m_strFieldTag, "Credit_limit", sizeof(m_strFieldTag));

					PopulateHistoryTable();
				}

				if (strcmp((char *)sOldPAFRecord.Custr_nbr, (char *)sPAFRecord.Custr_nbr) !=0)
				{
					memcpy(m_strCurrentData, sPAFRecord.Custr_nbr, sizeof(m_strCurrentData));
					memcpy(m_strPreviousData, sOldPAFRecord.Custr_nbr, sizeof(m_strPreviousData));
					memcpy(m_strFieldTag, "Customer number", sizeof(m_strFieldTag));

					PopulateHistoryTable();
				}

				if (strcmp((char *)sOldPAFRecord.APP_Source, (char *)sPAFRecord.APP_Source) !=0)
				{
					memcpy(m_strCurrentData, sPAFRecord.APP_Source, sizeof(m_strCurrentData));
					memcpy(m_strPreviousData, sOldPAFRecord.APP_Source, sizeof(m_strPreviousData));
					memcpy(m_strFieldTag, "App Source", sizeof(m_strFieldTag));

					PopulateHistoryTable();
				}

				if (strcmp((char *)sOldPAFRecord.COMP_Name, (char *)sPAFRecord.COMP_Name) !=0)
				{
					memcpy(m_strCurrentData, sPAFRecord.COMP_Name, sizeof(m_strCurrentData));
					memcpy(m_strPreviousData, sOldPAFRecord.COMP_Name, sizeof(m_strPreviousData));
					memcpy(m_strFieldTag, "Comp Name", sizeof(m_strFieldTag));

					PopulateHistoryTable();
				}

				SetModes(MODE_DONE);
				CleanPages();
				m_ComEdit.SetWindowText("Record Updated");
			}

			break;
		}
	case MODE_VIEW:
		{
			SetModes(MODE_DONE);
			CleanPages();
			break;
		}
				return FALSE;
	}
}
	m_pSheet->SetWindowText("Primary Account Definition");
	//set version info in the title bar
	CString appProdVer;
	CString lpszTitle;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	m_pSheet->GetWindowText(lpszTitle);
	lpszTitle = lpszTitle + " - Version - ";
	lpszTitle += appProdVer;
	m_pSheet->SetWindowText(lpszTitle);
	

	//return CPropertyPage::OnApply();
   return FALSE;
}


void CPanMainPage::PopulatePAF01(pPAF01	psPAFRecord) 
{
	pProcPage->UpdateData(TRUE);
	pDemoPage->UpdateData(TRUE);

	memset(psPAFRecord, 0, sizeof(PAF01)) ;
	lstrcpy ((LPTSTR) psPAFRecord->primary_key.primary_acct_nbr,m_panselected);

	lstrcpy ((LPTSTR) psPAFRecord->caf_profile_id,	pProcPage->m_strVipProc);
	lstrcpy ((LPTSTR) psPAFRecord->type,			pProcPage->m_strType);
	lstrcpy ((LPTSTR) psPAFRecord->status,			pProcPage->m_strAccStatus);
	lstrcpy ((LPTSTR) psPAFRecord->account_name,	pProcPage->m_StrAccountNameFld);
	lstrcpy ((LPTSTR) psPAFRecord->block_status,	pProcPage->m_StrBlockStatusFld);
	lstrcpy ((LPTSTR) psPAFRecord->corp_acct_nbr,	pProcPage->m_StrCorpAccountNumFld);
	lstrcpy ((LPTSTR) psPAFRecord->min_pay_code,	pProcPage->m_StrMinPayFld);
	lstrcpy ((LPTSTR) psPAFRecord->past_due_amount,	pProcPage->m_StrPastAmountFld);
	lstrcpy ((LPTSTR) psPAFRecord->Credit_limit,	pProcPage->m_StrCreditLimit);
	lstrcpy ((LPTSTR) psPAFRecord->Custr_nbr,	pProcPage->m_StrCustrNbr);
	lstrcpy ((LPTSTR) psPAFRecord->APP_Source,	pProcPage->m_StrAppSource);
	lstrcpy ((LPTSTR) psPAFRecord->COMP_Name,	pProcPage->m_StrCompName);
/*	lstrcpy ((LPTSTR) psPAFRecord->related_acct_nbr,	pProcPage->m_StrRelatedAccountNbrFld);

	if ( pProcPage->m_StrRelatedAccountType == "Credit" )
		lstrcpy ((LPTSTR) psPAFRecord->related_acct_type, "03");*/

	lstrcpy ((LPTSTR) psPAFRecord->name_dba,pDemoPage->m_strName);
	lstrcpy ((LPTSTR) psPAFRecord->address1,pDemoPage->m_strAddr1);
	lstrcpy ((LPTSTR) psPAFRecord->address2,pDemoPage->m_strAddr2);
	lstrcpy ((LPTSTR) psPAFRecord->city,pDemoPage->m_strCity);
	lstrcpy ((LPTSTR) psPAFRecord->state,pDemoPage->m_strState);
	lstrcpy ((LPTSTR) psPAFRecord->zip_code,pDemoPage->m_strZip);
	lstrcpy ((LPTSTR) psPAFRecord->home_phone,pDemoPage->m_strHomePhone);
	lstrcpy ((LPTSTR) psPAFRecord->work_phone,pDemoPage->m_strWorkPhone);

}

void CPanMainPage::SetModes(int mode) 
{
	DialogMode = mode;
	pProcPage->DialogMode = mode;
	pDemoPage->DialogMode = mode;
	pTxPage->DialogMode = mode;
	pChHistPage->m_nDialogMode = mode;


}

void CPanMainPage::OnList() 
{
	CBinList listDlg;
    int nReturnCode;

	UpdateData(TRUE);
	listDlg.m_nDialogMode = MODE_PRIMACC;
	listDlg.strItem0 = m_panselected;
    nReturnCode = listDlg.DoModal();
    if ( nReturnCode == IDOK)
	{
		m_panselected = listDlg.m_strPan;
		CleanPages();
	}
        
    if (!UpdateData(FALSE))
	{
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}
	m_ComEdit.SetWindowText("");
	
}


void CPanMainPage::PopulateHistPage()
{
	HST01 sHistoryRecord;

	ZeroMemory(&sHistoryRecord, sizeof(sHistoryRecord));

    memcpy ((LPTSTR) sHistoryRecord.primary_key.unique_id, (LPCTSTR)"   ", sizeof(sHistoryRecord.primary_key.unique_id));
    memcpy ((LPTSTR) sHistoryRecord.file_name,     (LPCTSTR)"PAF01", sizeof (sHistoryRecord.file_name));
    memcpy ((LPTSTR) sHistoryRecord.file_primary_key,  (LPCTSTR)m_panselected, sizeof (sHistoryRecord.file_primary_key));

	memcpy (&(pChHistPage->sHistoryRecord), &sHistoryRecord,sizeof(sHistoryRecord));

	if (pChHistPage->m_bChangeHistory)
		pChHistPage->PopulateHistoryDialog();

}

void CPanMainPage::CleanPages()
{
	pProcPage->m_panstr.Empty();
	pProcPage->m_strVipProc.Empty();
	pProcPage->m_strAccStatus.Empty();
	pProcPage->m_strType.Empty();
	pProcPage->m_strComEdit.Empty();
	pProcPage->m_StrCustrNbr.Empty();
	pProcPage->m_StrCreditLimit.Empty();
	pProcPage->m_StrAppSource.Empty();
	pProcPage->m_StrCompName.Empty();
//	pProcPage->m_StrRelatedAccountNbrFld.Empty();
//	pProcPage->m_StrRelatedAccountType.Empty();

	pDemoPage->m_panstr.Empty();
	pDemoPage->m_strName.Empty();
	pDemoPage->m_strAddr1.Empty();
	pDemoPage->m_strAddr2.Empty();
	pDemoPage->m_strCity.Empty();
	pDemoPage->m_strState.Empty();
	pDemoPage->m_strZip.Empty();
	pDemoPage->m_strHomePhone.Empty();
	pDemoPage->m_strWorkPhone.Empty();
	
	pTxPage->m_panstr.Empty();
	if (pTxPage->m_bTxPage)
		pTxPage->m_TxList.DeleteAllItems();

	if (pChHistPage->m_bChangeHistory)
		pChHistPage->m_CntlList.DeleteAllItems();
}



void CPanMainPage::OnChangePan() 
{
	CleanPages();
	
}

void CPanMainPage::OnSearch() 
{
	CAlphaLookup alphaDlg;
    
   UpdateData(TRUE);
   alphaDlg.m_nDialogMode = MODE_PRIMACC;
   int nReturnCode = alphaDlg.DoModal();

   if ( nReturnCode == IDOK)
	{
      m_panselected = alphaDlg.strItem0;
   }

	m_pan.EnableWindow(TRUE);

   m_StrMessageLbl.Empty();

   UpdateData(FALSE);		
}

void CPanMainPage::OnCancel() 
{
	// TODO: Add your specialized code here and/or call the base class
	//StopTimer();
	CPropertyPage::OnCancel();
}

BOOL CPanMainPage::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}
