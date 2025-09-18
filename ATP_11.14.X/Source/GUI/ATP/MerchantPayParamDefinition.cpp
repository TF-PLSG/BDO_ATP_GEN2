// MerchantPayParamDefinition.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "MerchantPayParamDefinition.h"
#include "MerchantPayParam.h"
#include "BinList.h"

#include "MyUtils.h"
#include "AlphaLookup.h"
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
extern CString strMerchantID;
extern CString strOrganizationID;
extern CSharedMem shrmem;

/////////////////////////////////////////////////////////////////////////////
// CMerchantPayParamDefinition dialog

IMPLEMENT_DYNCREATE(CMerchantPayParamDefinition, CPropertyPage)

CMerchantPayParamDefinition::CMerchantPayParamDefinition(): CPropertyPage(CMerchantPayParamDefinition::IDD)
{
	//{{AFX_DATA_INIT(CMerchantPayParamDefinition)
	m_StrProductCodeFld = _T("");
	m_StrMessageLbl = _T("");
	//}}AFX_DATA_INIT
}

CMerchantPayParamDefinition::~CMerchantPayParamDefinition()
{
}


void CMerchantPayParamDefinition::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMerchantPayParamDefinition)
	DDX_Control(pDX, IDC_STATUS, m_CntlMessageLbl);
	DDX_Control(pDX, IDC_PRODUCT_CODE_FLD, m_CntlProductCodeFld);
	DDX_Text(pDX, IDC_PRODUCT_CODE_FLD, m_StrProductCodeFld);
	DDV_MaxChars(pDX, m_StrProductCodeFld		, 4);
	DDX_Text(pDX, IDC_STATUS, m_StrMessageLbl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMerchantPayParamDefinition, CPropertyPage)
	//{{AFX_MSG_MAP(CMerchantPayParamDefinition)
	ON_BN_CLICKED(IDC_ADD_BUTTON, OnAddButton)
	ON_BN_CLICKED(IDC_DELETE_BUTTON, OnDeleteButton)
	ON_BN_CLICKED(IDC_UPDATE_BUTTON, OnUpdateButton)
	ON_BN_CLICKED(IDC_VIEW_BUTTON, OnViewButton)
	ON_BN_CLICKED(IDC_LIST_BUTTON, OnListButton)
	ON_BN_CLICKED(IDC_SEARCH, OnSearch)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMerchantPayParamDefinition message handlers

BOOL CMerchantPayParamDefinition::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CPropertyPage::OnCommand(wParam, lParam);
}

void CMerchantPayParamDefinition::OnAddButton() 
{
	// TODO: Add your control notification handler code here
	MPPARA01 MerchantPayParamDefinition;
	UpdateData(TRUE);
	char strDate[9] = {0};
   	char strErrorMessage[200] = {0};
    CMyUtils myUtilsObject;
	m_StrMessageLbl = "";
	m_nDialogMode = MODE_ADD;
	
	if (!CheckDialogForCompleteness())
        return;

	pChangeHistoryPage->m_nDialogMode = MODE_ADD;
	pMerchantPayParamGeneral->m_nDialogMode = MODE_ADD;

	ZeroMemory(&MerchantPayParamDefinition,sizeof(MPPARA01)) ;
	
	memcpy(&MerchantPayParamDefinition.primary_key.product_code, m_StrProductCodeFld , sizeof(MerchantPayParamDefinition.primary_key.product_code));

	int rcode = txdsapi_get_record( (pBYTE)&MerchantPayParamDefinition, sizeof (MPPARA01), MPPARA01_DATA, strErrorMessage );
	if( PTEMSG_OK == rcode )
    {
		m_StrMessageLbl = "ERROR: Product code already exists";
		m_nDialogMode = MODE_DONE;
		m_CntlProductCodeFld.SetFocus();
		
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return ;
    }
	
	if ( pChangeHistoryPage->m_bChangeHistory )
	{
		pChangeHistoryPage->CleanChangeHistory();
	}
	
	if ( pMerchantPayParamGeneral->m_bGeneral)
	{
		pMerchantPayParamGeneral->EnableMerchantGroupDialog(TRUE);
		pMerchantPayParamGeneral->CleanGeneralDialog();
		pMerchantPayParamGeneral->UpdateData(FALSE);
	}
	pMerchantPayParamGeneral->m_StrProductCode = m_StrProductCodeFld;

	pChangeHistoryPage->bNext = FALSE;
	((CPropertySheet*) this->GetParent())->SetWindowText("Add Product Code");
	
	CString appProdVer;
	CString lpszTitle;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
	lpszTitle = lpszTitle + " - Version - ";
	lpszTitle += appProdVer;
	((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);
	
	pMerchantPayParamGeneral->SetModified ( TRUE );	
	m_CntlProductCodeFld.EnableWindow(TRUE);
	// go to first page
	((CPropertySheet*) this->GetParent())->SetActivePage(1);
	SetModified(FALSE);
}

void CMerchantPayParamDefinition::OnDeleteButton() 
{
	MPPARA01 MerchantPayParamDefinition;
	UpdateData(TRUE);
	CString strMessage;
    int nReturnCode;
	char strErrorMessage[200];
    m_nDialogMode = MODE_EDIT;
	m_StrMessageLbl = "";

    if (!CheckDialogForCompleteness())
        return; 

    if (!UpdateData(TRUE))
	{
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}
    
	// confirm the action
	strMessage.Format ("Are you sure you want to delete Merchant Pay Parameter %s ?", (LPCTSTR) m_StrProductCodeFld) ;

	nReturnCode = AfxMessageBox ((LPCTSTR)strMessage, MB_OKCANCEL|MB_ICONQUESTION) ;
	if (nReturnCode != IDOK)
	{
		return ;
	}

	// try to delete
	memcpy ((LPTSTR)MerchantPayParamDefinition.primary_key.product_code,	m_StrProductCodeFld,sizeof (MerchantPayParamDefinition.primary_key.product_code)) ;
	
	int rcode = txdsapi_delete_record ((pBYTE)&MerchantPayParamDefinition, sizeof ( MPPARA01), MPPARA01_DATA, strErrorMessage) ;
	// failed to delete
	if ( PTEMSG_OK != rcode   )
    {
		switch( rcode )
		{
			 case PTEMSG_NOT_FOUND:
				sprintf( strErrorMessage, "ERROR: Merchant Pay Parameter record for %s does not exist", m_StrProductCodeFld );
				break;
			 default:
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
		
        memcpy ((LPSTR)m_strCurrentData, "Record Deleted", sizeof (m_strCurrentData));
		memcpy ((LPSTR)m_strPreviousData, "", sizeof (m_strCurrentData));
		memcpy ((LPSTR)m_strFieldTag, "", sizeof (m_strCurrentData));
        PopulateHistoryTable();

		m_StrProductCodeFld = _T("");	
		m_StrMessageLbl = "Record Deleted";
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}

		// clean and disable all pages
		if ( pMerchantPayParamGeneral->m_bGeneral )
		{
			pMerchantPayParamGeneral->EnableMerchantGroupDialog(FALSE);
			pMerchantPayParamGeneral->CleanGeneralDialog();
		}
		
		if ( pChangeHistoryPage->m_bChangeHistory )
			pChangeHistoryPage->CleanChangeHistory();

		m_nDialogMode = MODE_DONE;

		((CPropertySheet*) this->GetParent())->SetActivePage(0);
		SetModified ( FALSE );
    }

		m_CntlProductCodeFld.EnableWindow(TRUE);
			
}     /* function OnDeleteButton */
	


void CMerchantPayParamDefinition::OnUpdateButton() 
{
	HST01 sHistoryRecord;
	UpdateData(TRUE);
	char strErrorMessage[200];
	m_nDialogMode = MODE_EDIT;
	pMerchantPayParamGeneral->m_nDialogMode = MODE_EDIT;
	pChangeHistoryPage->m_nDialogMode = MODE_EDIT; 
	m_StrMessageLbl = "";
	
	memcpy ((LPSTR)(sMerchantPayParameterRecordOld.primary_key.product_code), m_StrProductCodeFld, sizeof (sMerchantPayParameterRecordOld.primary_key.product_code) );
	
	int rcode = txdsapi_get_record( (pBYTE)&sMerchantPayParameterRecordOld, sizeof (MPPARA01), MPPARA01_DATA, strErrorMessage );
	// failed to get the data
	if ( PTEMSG_OK != rcode )
	{       
		switch( rcode )
		{
			case PTEMSG_NOT_FOUND:
				sprintf( strErrorMessage, "ERROR: Merchant record for %s does not exist", m_StrProductCodeFld );
				break;
			default:
				break;
		}
		
		// all other cases
		m_StrMessageLbl = strErrorMessage;
		m_nDialogMode = MODE_DONE;
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		
		return;
    }

	pMerchantPayParamGeneral->CleanGeneralDialog();
	pMerchantPayParamGeneral->PopulateGeneralDialogFromDatabase(sMerchantPayParameterRecordOld);

	// get history data
    ZeroMemory (&sHistoryRecord, sizeof (sHistoryRecord));

	memset ( strFilePrimaryKey, 0, sizeof (strFilePrimaryKey));
	strcpy ( strFilePrimaryKey, (char *)sMerchantPayParameterRecordOld.primary_key.product_code);

    memcpy ((LPSTR)(sHistoryRecord.primary_key.unique_id), "                 ", sizeof (sHistoryRecord.primary_key.unique_id));
    memcpy ((LPSTR)(sHistoryRecord.file_name), "MPPARA01", sizeof (sHistoryRecord.file_name));
    memcpy ((LPSTR)(sHistoryRecord.file_primary_key), strFilePrimaryKey, sizeof (sHistoryRecord.file_primary_key));

    memcpy ( &(pChangeHistoryPage->sHistoryRecord), &sHistoryRecord, sizeof(sHistoryRecord));
    
	((CPropertySheet*) this->GetParent())->SetActivePage(1);    

	// set dialog mode for all pages

	pChangeHistoryPage->bNext = FALSE;

	((CPropertySheet*) this->GetParent())->SetWindowText("Update Merchant Group");
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

	PopulateMerchantDialogFromDatabase ( sMerchantPayParameterRecordOld );

	pMerchantPayParamGeneral->SetModified ( TRUE );	
	pMerchantPayParamGeneral->EnableWindow ( TRUE );
	pMerchantPayParamGeneral->EnableMerchantGroupDialog(TRUE);

	m_CntlProductCodeFld.EnableWindow(FALSE);

	if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}
	m_StrMessageLbl = "";
}

void CMerchantPayParamDefinition::OnViewButton() 
{
	HST01 sHistoryRecord;
	UpdateData(TRUE);
	char strErrorMessage[200];
	m_nDialogMode = MODE_VIEW;
	pMerchantPayParamGeneral->m_nDialogMode = MODE_VIEW;
	pChangeHistoryPage->m_nDialogMode = MODE_VIEW; 
	m_StrMessageLbl = "";
	
	memcpy ((LPSTR)(sMerchantPayParameterRecordOld.primary_key.product_code), m_StrProductCodeFld, sizeof (sMerchantPayParameterRecordOld.primary_key.product_code) );
	
	int rcode = txdsapi_get_record( (pBYTE)&sMerchantPayParameterRecordOld, sizeof (MPPARA01), MPPARA01_DATA, strErrorMessage );
	// failed to get the data
	if ( PTEMSG_OK != rcode )
	{       
		switch( rcode )
		{
		case PTEMSG_NOT_FOUND:
			sprintf( strErrorMessage, "ERROR: Product code record for %s does not exist", m_StrProductCodeFld );
            break;
		default:
			break;
		}
		
		// all other cases
		m_StrMessageLbl = strErrorMessage;
		m_nDialogMode = MODE_DONE;
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		
		return;
    }
	
	pMerchantPayParamGeneral->CleanGeneralDialog();
	pMerchantPayParamGeneral->PopulateGeneralDialogFromDatabase(sMerchantPayParameterRecordOld);
	
	// get history data
    ZeroMemory (&sHistoryRecord, sizeof (sHistoryRecord));
	
	memset ( strFilePrimaryKey, 0, sizeof (strFilePrimaryKey));
	strcpy ( strFilePrimaryKey, (char *)sMerchantPayParameterRecordOld.primary_key.product_code);
	
    memcpy ((LPSTR)(sHistoryRecord.primary_key.unique_id), "                 ", sizeof (sHistoryRecord.primary_key.unique_id));
    memcpy ((LPSTR)(sHistoryRecord.file_name), "MPPARA01", sizeof (sHistoryRecord.file_name));
    memcpy ((LPSTR)(sHistoryRecord.file_primary_key), strFilePrimaryKey, sizeof (sHistoryRecord.file_primary_key));
	
    memcpy ( &(pChangeHistoryPage->sHistoryRecord), &sHistoryRecord, sizeof(sHistoryRecord));
    
	((CPropertySheet*) this->GetParent())->SetActivePage(1);    
	
	// set dialog mode for all pages
	
	pChangeHistoryPage->bNext = FALSE;
	
	((CPropertySheet*) this->GetParent())->SetWindowText("View MP PARAMETER Details");
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
	
	pMerchantPayParamGeneral->EnableMerchantGroupDialog(FALSE);
	pMerchantPayParamGeneral->SetModified ( TRUE );
	pMerchantPayParamGeneral->EnableWindow ( FALSE );

	m_CntlProductCodeFld.EnableWindow(FALSE);
	m_StrMessageLbl = "";
	if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}

	
}

BOOL CMerchantPayParamDefinition::OnApply() 
{
	MPPARA01 sMerchantPayParameterDatabaseStructure;
	char strErrorMessage[200];
	CMyUtils myUtilsObject;
	int rcode =0;
	
	if(m_nDialogMode == MODE_DONE)
	{
		return FALSE;
		m_StrMessageLbl="";
	}

	if (m_nDialogMode == MODE_ADD)
	{
		memset(&sMerchantPayParameterDatabaseStructure, 0, sizeof(sMerchantPayParameterDatabaseStructure)) ;
		if (!PopulateMerchantDatabaseStructureFromDialog (&sMerchantPayParameterDatabaseStructure)) 
		{		
			
			return FALSE;
		}
	}
	/************************************************************************/
	if ( MODE_VIEW == m_nDialogMode && MODE_DONE != m_nDialogMode )
	{
	   m_StrMessageLbl="";
       return FALSE;
	}

	/*********************************************************************/

	/*********************************************************************/
	if (m_nDialogMode == MODE_EDIT)
	{ 
		MPPARA01 sMerchantDatabaseStructure;
        memset(&sMerchantDatabaseStructure, 0, sizeof(MPPARA01)) ;

		PopulateMerchantDatabaseStructureFromDialog (&sMerchantDatabaseStructure );

		if(ValidateDate(sMerchantDatabaseStructure.effective_date))
		{
			rcode = txdsapi_update_record ( (pBYTE)&sMerchantDatabaseStructure, sizeof (MPPARA01), MPPARA01_DATA, strErrorMessage );
			if ( rcode != PTEMSG_OK )
			{ 		
				m_StrMessageLbl = strErrorMessage;
				m_nDialogMode = MODE_DONE;
				((CPropertySheet*) this->GetParent())->SetActivePage(0);
				((CPropertySheet*) this->GetParent())->SetWindowText("Merchant Group Definition");
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
				pMerchantPayParamGeneral->CompareStructures();
				
				pMerchantPayParamGeneral->EnableMerchantGroupDialog(FALSE);
				if ( pMerchantPayParamGeneral->m_bGeneral)
				{
					pMerchantPayParamGeneral->CleanGeneralDialog();
				}
				
				if ( pChangeHistoryPage->m_bChangeHistory )
					pChangeHistoryPage->CleanChangeHistory();
				((CPropertySheet*) this->GetParent())->SetActivePage(0);
				((CPropertySheet*) this->GetParent())->SetWindowText("Merchant Group Definition");
				//set version info in the title bar
				CString appProdVer;
				CString lpszTitle;
				appProdVer.LoadString(IDS_PRODUCT_VERSION);
				((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
				lpszTitle = lpszTitle + " - Version - ";
				lpszTitle += appProdVer;
				((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);

				m_CntlProductCodeFld.EnableWindow(TRUE);
				
				SetModified ( FALSE );

				m_nDialogMode = MODE_DONE;
				pMerchantPayParamGeneral->m_nDialogMode = MODE_DONE;  
				pChangeHistoryPage->m_nDialogMode = MODE_DONE; 
			
				//return CPropertyPage::OnApply();
				return FALSE;
			}
		}
		else
		{
			memcpy ((LPSTR)m_strCurrentData, "Invalid Date Format", sizeof (m_strCurrentData));		
				//PopulateHistoryTable();
				((CPropertySheet*) this->GetParent())->SetActivePage(0);
				if (!UpdateData(FALSE)){
					AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
				}
				return FALSE;
		}
	}
	/*********************************************************************/
	if(ValidateDate(sMerchantPayParameterDatabaseStructure.effective_date))
	{	
		rcode = txdsapi_insert_record ( (pBYTE)&sMerchantPayParameterDatabaseStructure, sizeof (MPPARA01), MPPARA01_DATA, strErrorMessage );
		if ( rcode != PTEMSG_OK )
		{
			m_StrMessageLbl = strErrorMessage;
			m_nDialogMode = MODE_DONE;
			if (!UpdateData(FALSE)){
					AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
				}
			((CPropertySheet*) this->GetParent())->SetActivePage(0);
		}
		else
		{
				/*memcpy ((LPSTR)m_strCurrentData, "Record Added", sizeof (m_strCurrentData));		
				PopulateHistoryTable();
				((CPropertySheet*) this->GetParent())->SetActivePage(0);
				m_StrMessageLbl = "Record Added";	
				if (!UpdateData(FALSE)){
					AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
				}*/

			memcpy ((LPSTR)m_strCurrentData, "Record Added", sizeof (m_strCurrentData));
			memset ( m_strPreviousData, 0, sizeof (m_strPreviousData));
			memset ( m_strFieldTag, 0, sizeof (m_strPreviousData));
			PopulateHistoryTable();
			((CPropertySheet*) this->GetParent())->SetActivePage(0);
			m_StrMessageLbl = "Record Added";	
			if (!UpdateData(FALSE))
			{
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}
			pMerchantPayParamGeneral->EnableMerchantGroupDialog(FALSE);
				if ( pMerchantPayParamGeneral->m_bGeneral)
				{
					pMerchantPayParamGeneral->CleanGeneralDialog();
				}
				if ( pChangeHistoryPage->m_bChangeHistory )
					pChangeHistoryPage->CleanChangeHistory();

				return FALSE;



		}
	}
	else
	{
		memcpy ((LPSTR)m_strCurrentData, "Invalid Date Format", sizeof (m_strCurrentData));		
				//PopulateHistoryTable();
				((CPropertySheet*) this->GetParent())->SetActivePage(0);
				if (!UpdateData(FALSE)){
					AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
				}
				return FALSE;
	}
	
	return CPropertyPage::OnApply();
	
	
}

void CMerchantPayParamDefinition::OnListButton() 
{
	CBinList listDlg;
    int nReturnCode;
	
	UpdateData(TRUE);
	m_StrMessageLbl = "";
	UpdateData(FALSE);
	
	// set dialog Mode for Bin List
	listDlg.strItem0 = m_StrProductCodeFld;
	listDlg.m_nDialogMode = MODE_MERCHANT_PAY_PARAM;
	
    nReturnCode = listDlg.DoModal();
    if ( nReturnCode == IDOK)
	{
        m_StrProductCodeFld = listDlg.strItem0;;
		pMerchantPayParamGeneral->CleanGeneralDialog();
		m_CntlProductCodeFld.EnableWindow(TRUE);
	}
	
    if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}		
	
	
}

void CMerchantPayParamDefinition::OnSearch() 
{
	// TODO: Add your control notification handler code here
	
}

BOOL CMerchantPayParamDefinition::CheckDialogForCompleteness()
{
CMyUtils myUtilsObject;


    if (!UpdateData(TRUE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}

    if (myUtilsObject.IsEmpty (m_StrProductCodeFld, this))
    {
		m_StrMessageLbl = "Please enter Product Code";	
		m_CntlProductCodeFld.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		m_nDialogMode = MODE_DONE;
		return FALSE;
	}

	return TRUE;
}

void CMerchantPayParamDefinition::PopulateHistoryTable()
{
	HST01 sHistoryData;
    CHAR dest_str[18]="";   
	CMyUtils myUtilsObject;
	char strErrorMsg[200] = "";
    char strDate[9] = "";
    char strTime[7] = "";
	char strPrimaryKey[50];
	CHAR strDisplayErrMsg[250];
    CString temp;
	CHAR s_merch_ctr[4]   = "";
	
	memset ( strDate, 0, sizeof (strDate));
    memset ( strTime, 0, sizeof (strTime));
	memset ( strPrimaryKey, 0, sizeof (strPrimaryKey));

	strcpy ( strPrimaryKey, m_StrProductCodeFld);
	    
    myUtilsObject.GetSystemDate(strDate);
    myUtilsObject.GetSystemTime(strTime);

	//  generate a timestamp for the current Greenwich Mean Time
	// in format YYYYMMDDHHIISSJJJ .

    pintime_get_gmt (dest_str);

    ZeroMemory ( &sHistoryData, sizeof (HST01));
    CHAR wstation[21]; DWORD wsize;

    wsize = sizeof( wstation );
    GetComputerName( wstation, &wsize );

	memcpy ((LPTSTR) sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
    memcpy ((LPTSTR) sHistoryData.file_name,     (LPCTSTR)"MPPARA01", sizeof (sHistoryData.file_name));
	memcpy ((LPTSTR) sHistoryData.field_tag,     (LPCTSTR)m_strFieldTag, sizeof (sHistoryData.file_name));
    memcpy ((LPTSTR) sHistoryData.file_primary_key,  (LPCTSTR)strPrimaryKey, sizeof (sHistoryData.file_primary_key));
    memcpy ((LPTSTR) sHistoryData.primary_key.unique_id,     (LPCTSTR)dest_str, sizeof (sHistoryData.primary_key.unique_id));        
    memcpy ((LPTSTR) sHistoryData.time_modified,     (LPCTSTR)strTime, sizeof (sHistoryData.time_modified)); 
    memcpy ((LPTSTR) sHistoryData.current_data, (LPCTSTR)m_strCurrentData, sizeof (sHistoryData.current_data));
	memcpy ((LPTSTR) sHistoryData.previous_data, (LPCTSTR)m_strPreviousData, sizeof (sHistoryData.previous_data));
	memcpy ((LPTSTR) sHistoryData.changed_by,(LPTSTR)ucf01.primary_key.name, sizeof(sHistoryData.changed_by));
    temp = strDate; 
    temp = temp.Right(4) + temp.Left(2) + temp.Mid(2,2);
    
    memcpy ((LPTSTR) sHistoryData.date_modified, temp, sizeof (sHistoryData.date_modified));

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
	/* function PopulateHistoryTable */
}

BOOL CMerchantPayParamDefinition::PopulateMerchantDatabaseStructureFromDialog ( pMPPARA01 psMerchantPayParamter)
{
	memset (psMerchantPayParamter,0, sizeof(MPPARA01)) ;
	
	// save all previous data
	if ( m_nDialogMode == MODE_EDIT )
		memcpy ( psMerchantPayParamter, &sMerchantPayParameterRecordOld, sizeof (MPPARA01));
	
	memcpy ((LPTSTR) psMerchantPayParamter->primary_key.product_code, m_StrProductCodeFld, 
		sizeof (psMerchantPayParamter->primary_key.product_code));

	if(!pMerchantPayParamGeneral->PopulateDatabaseFromGeneralDialog(psMerchantPayParamter))
		return FALSE;


	return TRUE;
}

void CMerchantPayParamDefinition::PopulateMerchantDialogFromDatabase (MPPARA01 sMerchantPayParamter)
{
	memcpy(&pMerchantPayParamGeneral->sMerchantPayParameterRecordOld, &sMerchantPayParamter, sizeof(MPPARA01));

	pMerchantPayParamGeneral->PopulateGeneralDialogFromDatabase(sMerchantPayParamter);
}

BOOL CMerchantPayParamDefinition::OnInitDialog() 
{

	CPropertyPage::OnInitDialog();

	CFont m_font;
	LOGFONT lf;
	CString temp;
	int nIndex = -1;

	// set font for the Message lable field

	memset ( &lf, 0, sizeof (lf));
	lf.lfHeight = 12;
	lf.lfWeight = 700;
	strcpy(lf.lfFaceName, "Arial");
	m_font.CreateFontIndirect (&lf);
	GetDlgItem(IDC_STATUS)->SetFont (&m_font);

	m_StrProductCodeFld = _T("");
	m_StrMessageLbl = "";
	UpdateData(FALSE);
	
	// TODO: Add extra initialization here
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CMerchantPayParamDefinition::ValidateDate ( CString strDate )
{
	CString strDay;
	CString strMonth;
	CString strYear;
	CMyUtils myUtilsObject;

	int nDay, nMonth, nYear;

	UpdateData(TRUE);

	if ( myUtilsObject.IsEmpty ( strDate, this ))
		return FALSE;

	int length = strDate.GetLength();	
	int res = strspn ( strDate,"1234567890" );

	if ( length < 8 )
	{
		m_StrMessageLbl = "ERROR: Invalid date format";
		UpdateData(FALSE);
		return FALSE;
	}
		
	strYear = strDate.Left(4);
	strMonth = strDate.Mid(4,2);
	strDay = strDate.Right(2);
	

	nDay = atoi ( strDay );
	nMonth = atoi ( strMonth );
	nYear = atoi ( strYear );

	if ( nDay == 0 || nMonth == 0 || nYear ==0)
	{
		m_StrMessageLbl = "ERROR: Invalid data";
		UpdateData(FALSE);
		return FALSE;
	}


	if ( nMonth > 12 )
	{
		m_StrMessageLbl = "ERROR: Invalid month";
		UpdateData(FALSE);
		return FALSE;
	}

	if ( nMonth == 1 || nMonth == 3 || nMonth == 5 || 
		 nMonth == 7 || nMonth == 8 || nMonth == 10 || nMonth == 12 )
	{
		if ( nDay > 31 )
		{
			m_StrMessageLbl = "ERROR: Invalid day";
			UpdateData(FALSE);
			return FALSE;
		}
	}

	if ( nMonth == 4 || nMonth == 6 || nMonth == 9 || 
		 nMonth == 11 )
	{
		if ( nDay > 30 )
		{
			m_StrMessageLbl = "ERROR: Invalid day";
			UpdateData(FALSE);
			return FALSE;
		}
	}

	if ( nMonth ==2 )
	{
		int res = nYear % 4;
		if ( res != 0 && nDay >28)
		{
			m_StrMessageLbl = "ERROR: Invalid day";
			UpdateData(FALSE);
			return FALSE;
		}
		if (res == 0 && nDay >29)
		{
			m_StrMessageLbl = "ERROR: Invalid day";
			UpdateData(FALSE);
			return FALSE;
		}
	}
	
	return TRUE;
}

