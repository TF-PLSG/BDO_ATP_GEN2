// MerchantGroupDefinition.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "MerchantGroupDefinition.h"
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
// CMerchantGroupDefinition property page

IMPLEMENT_DYNCREATE(CMerchantGroupDefinition, CPropertyPage)

CMerchantGroupDefinition::CMerchantGroupDefinition() : CPropertyPage(CMerchantGroupDefinition::IDD)
{
	//{{AFX_DATA_INIT(CMerchantGroupDefinition)
	m_StrMerchantGroupId = _T("");
	m_StrMessageLbl = _T("");
	//}}AFX_DATA_INIT
}

CMerchantGroupDefinition::~CMerchantGroupDefinition()
{
}

void CMerchantGroupDefinition::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMerchantGroupDefinition)
	DDX_Control(pDX, IDC_STATUS, m_CntlMessageLbl);
	DDX_Control(pDX, IDC_MERCHANT_GROUP_ID_FLD, m_CntlMerchantGroupId);
	DDX_Text(pDX, IDC_MERCHANT_GROUP_ID_FLD, m_StrMerchantGroupId);
	DDV_MaxChars(pDX, m_StrMerchantGroupId		, 15);
	DDX_Text(pDX, IDC_STATUS, m_StrMessageLbl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMerchantGroupDefinition, CPropertyPage)
	//{{AFX_MSG_MAP(CMerchantGroupDefinition)
	ON_BN_CLICKED(IDC_ADD_BUTTON, OnAddButton)
	ON_BN_CLICKED(IDC_DELETE_BUTTON, OnDeleteButton)
	ON_BN_CLICKED(IDC_UPDATE_BUTTON, OnUpdateButton)
	ON_BN_CLICKED(IDC_VIEW_BUTTON, OnViewButton)
	ON_BN_CLICKED(IDC_LIST_BUTTON, OnListButton)
	ON_BN_CLICKED(IDC_SEARCH, OnSearch)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMerchantGroupDefinition message handlers


BOOL CMerchantGroupDefinition::OnInitDialog() 
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

	m_StrMerchantGroupId = _T("");
	

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CMerchantGroupDefinition::OnAddButton() 
{
	MGF01 sMerchantGroupRecord;
	UpdateData(TRUE);
	char strDate[9] = {0};
    
	char strErrorMessage[200] = {0};
    
    CMyUtils myUtilsObject;

	m_StrMessageLbl = "";

	m_nDialogMode = MODE_ADD;
	
	if (!CheckDialogForCompleteness())
        return;

	pChangeHistoryPage->m_nDialogMode = MODE_ADD;
	pMerchantGroupGeneral->m_nDialogMode = MODE_ADD;

	ZeroMemory(&sMerchantGroupRecord,sizeof(MGF01)) ;

	
	memcpy(&sMerchantGroupRecord.primary_key.merchant_group_id, m_StrMerchantGroupId , sizeof(sMerchantGroupRecord.primary_key.merchant_group_id));

	int rcode = txdsapi_get_record( (pBYTE)&sMerchantGroupRecord, sizeof (MGF01), MGF01_DATA, strErrorMessage );
	if( PTEMSG_OK == rcode )
    {
		m_StrMessageLbl = "ERROR: Merchant ID already exists";
		m_nDialogMode = MODE_DONE;
		m_CntlMerchantGroupId.SetFocus();
		
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return ;
    }
	
	if ( pChangeHistoryPage->m_bChangeHistory )
	{
		pChangeHistoryPage->CleanChangeHistory();
	}
	if ( pMerchantGroupGeneral->m_bGeneral)
	{
		pMerchantGroupGeneral->EnableMerchantGroupDialog(TRUE);
		pMerchantGroupGeneral->CleanGeneralDialog();
		pMerchantGroupGeneral->UpdateData(FALSE);
	}
	
	pMerchantGroupGeneral->m_StrMechantGroupIdFld = m_StrMerchantGroupId;

	pChangeHistoryPage->bNext = FALSE;
	((CPropertySheet*) this->GetParent())->SetWindowText("Add Merchant Group");
	
	CString appProdVer;
	CString lpszTitle;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
	lpszTitle = lpszTitle + " - Version - ";
	lpszTitle += appProdVer;
	((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);
	
	pMerchantGroupGeneral->SetModified ( TRUE );	
	m_CntlMerchantGroupId.EnableWindow(FALSE);

	// go to first page
	((CPropertySheet*) this->GetParent())->SetActivePage(1);
	SetModified(FALSE);
	
}

void CMerchantGroupDefinition::OnDeleteButton() 
{
	MGF01 sMerchantGroupRecord;
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
	strMessage.Format ("Are you sure you want to delete Merchant Group ID %s ?", (LPCTSTR) m_StrMerchantGroupId) ;

	nReturnCode = AfxMessageBox ((LPCTSTR)strMessage, MB_OKCANCEL|MB_ICONQUESTION) ;
	if (nReturnCode != IDOK)
	{
		return ;
	}

	// try to delete
	memcpy ((LPTSTR)sMerchantGroupRecord.primary_key.merchant_group_id,	m_StrMerchantGroupId,sizeof (sMerchantGroupRecord.primary_key.merchant_group_id)) ;
	
	int rcode = txdsapi_delete_record ((pBYTE)&sMerchantGroupRecord, sizeof ( MGF01), MGF01_DATA, strErrorMessage) ;
	// failed to delete
	if ( PTEMSG_OK != rcode   )
    {
		switch( rcode )
		{
			 case PTEMSG_NOT_FOUND:
				sprintf( strErrorMessage, "ERROR: Merchant record for %s does not exist", m_StrMerchantGroupId );
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

		m_StrMerchantGroupId = _T("");	
		m_StrMessageLbl = "Record Deleted";

		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}

		// clean and disable all pages
		if ( pMerchantGroupGeneral->m_bGeneral )
		{
			pMerchantGroupGeneral->EnableMerchantGroupDialog(FALSE);
			pMerchantGroupGeneral->CleanGeneralDialog();
		}
		
		if ( pChangeHistoryPage->m_bChangeHistory )
			pChangeHistoryPage->CleanChangeHistory();

		m_nDialogMode = MODE_DONE;

		((CPropertySheet*) this->GetParent())->SetActivePage(0);
		SetModified ( FALSE );
    }
		m_CntlMerchantGroupId.EnableWindow(TRUE);
			
}     /* function OnDeleteButton */
	
void CMerchantGroupDefinition::OnUpdateButton() 
{
	HST01 sHistoryRecord;
	UpdateData(TRUE);
	char strErrorMessage[200];
	m_nDialogMode = MODE_EDIT;
	pMerchantGroupGeneral->m_nDialogMode = MODE_EDIT;
	pChangeHistoryPage->m_nDialogMode = MODE_EDIT; 
	m_StrMessageLbl = "";
	
	memcpy ((LPSTR)(sMerchantGroupRecordOld.primary_key.merchant_group_id), m_StrMerchantGroupId, sizeof (sMerchantGroupRecordOld.primary_key.merchant_group_id) );
	
	int rcode = txdsapi_get_record( (pBYTE)&sMerchantGroupRecordOld, sizeof (MGF01), MGF01_DATA, strErrorMessage );
	// failed to get the data
	if ( PTEMSG_OK != rcode )
	{       
		switch( rcode )
		{
			case PTEMSG_NOT_FOUND:
				sprintf( strErrorMessage, "ERROR: Merchant record for %s does not exist", m_StrMerchantGroupId );
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

	pMerchantGroupGeneral->CleanGeneralDialog();
	pMerchantGroupGeneral->PopulateGeneralDialogFromDatabase(sMerchantGroupRecordOld);

	// get history data
    ZeroMemory (&sHistoryRecord, sizeof (sHistoryRecord));

	memset ( strFilePrimaryKey, 0, sizeof (strFilePrimaryKey));
	strcpy ( strFilePrimaryKey, (char *)sMerchantGroupRecordOld.primary_key.merchant_group_id);

    memcpy ((LPSTR)(sHistoryRecord.primary_key.unique_id), "                 ", sizeof (sHistoryRecord.primary_key.unique_id));
    memcpy ((LPSTR)(sHistoryRecord.file_name), "MGF01", sizeof (sHistoryRecord.file_name));
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

	PopulateMerchantDialogFromDatabase ( sMerchantGroupRecordOld );

	pMerchantGroupGeneral->SetModified ( TRUE );	
	pMerchantGroupGeneral->EnableWindow ( TRUE );
	pMerchantGroupGeneral->EnableMerchantGroupDialog(TRUE);

	m_CntlMerchantGroupId.EnableWindow(FALSE);

	if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}
	m_StrMessageLbl = "";
}

void CMerchantGroupDefinition::OnViewButton() 
{
	// TODO: Add your control notification handler code here
	HST01 sHistoryRecord;
	UpdateData(TRUE);
	char strErrorMessage[200];
	m_nDialogMode = MODE_VIEW;
	pMerchantGroupGeneral->m_nDialogMode = MODE_VIEW;
	pChangeHistoryPage->m_nDialogMode = MODE_VIEW; 
	m_StrMessageLbl = "";
	
	memcpy ((LPSTR)(sMerchantGroupRecordOld.primary_key.merchant_group_id), m_StrMerchantGroupId, sizeof (sMerchantGroupRecordOld.primary_key.merchant_group_id) );
	
	int rcode = txdsapi_get_record( (pBYTE)&sMerchantGroupRecordOld, sizeof (MGF01), MGF01_DATA, strErrorMessage );
	// failed to get the data
	if ( PTEMSG_OK != rcode )
	{       
		switch( rcode )
		{
		case PTEMSG_NOT_FOUND:
			sprintf( strErrorMessage, "ERROR: Merchant record for %s does not exist", m_StrMerchantGroupId );
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
	
	pMerchantGroupGeneral->CleanGeneralDialog();
	pMerchantGroupGeneral->PopulateGeneralDialogFromDatabase(sMerchantGroupRecordOld);
	
	// get history data
    ZeroMemory (&sHistoryRecord, sizeof (sHistoryRecord));
	
	memset ( strFilePrimaryKey, 0, sizeof (strFilePrimaryKey));
	strcpy ( strFilePrimaryKey, (char *)sMerchantGroupRecordOld.primary_key.merchant_group_id);
	
    memcpy ((LPSTR)(sHistoryRecord.primary_key.unique_id), "                 ", sizeof (sHistoryRecord.primary_key.unique_id));
    memcpy ((LPSTR)(sHistoryRecord.file_name), "MGF01", sizeof (sHistoryRecord.file_name));
    memcpy ((LPSTR)(sHistoryRecord.file_primary_key), strFilePrimaryKey, sizeof (sHistoryRecord.file_primary_key));
	
    memcpy ( &(pChangeHistoryPage->sHistoryRecord), &sHistoryRecord, sizeof(sHistoryRecord));
    
	((CPropertySheet*) this->GetParent())->SetActivePage(1);    
	
	// set dialog mode for all pages
	
	pChangeHistoryPage->bNext = FALSE;
	
	((CPropertySheet*) this->GetParent())->SetWindowText("View Merchant Group Details");
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
	
	pMerchantGroupGeneral->EnableMerchantGroupDialog(FALSE);
	pMerchantGroupGeneral->SetModified ( TRUE );
	pMerchantGroupGeneral->EnableWindow ( FALSE );

	m_CntlMerchantGroupId.EnableWindow(FALSE);
	m_StrMessageLbl = "";
	if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
}

void CMerchantGroupDefinition::OnListButton() 
{
	// TODO: Add your control notification handler code here
	CBinList listDlg;
    int nReturnCode;
	
	UpdateData(TRUE);
	m_StrMessageLbl = "";
	UpdateData(FALSE);
	
	// set dialog Mode for Bin List
	listDlg.strItem0 = m_StrMerchantGroupId;
	listDlg.m_nDialogMode = MODE_MGF01;
	
    nReturnCode = listDlg.DoModal();
    if ( nReturnCode == IDOK)
	{
        m_StrMerchantGroupId = listDlg.strItem0;
		pMerchantGroupGeneral->CleanGeneralDialog();
		m_CntlMerchantGroupId.EnableWindow(TRUE);
	}
	
    if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}		
	
	
}

void CMerchantGroupDefinition::OnSearch() 
{
	// TODO: Add your control notification handler code here
	CAlphaLookup alphaDlg;

	UpdateData(TRUE);

	alphaDlg.m_nDialogMode = MODE_MGF01;
	int nReturnCode = alphaDlg.DoModal();

	if ( nReturnCode == IDOK)
	{
		m_StrMerchantGroupId = alphaDlg.strItem0;
	}

	m_CntlMerchantGroupId.EnableWindow(TRUE);
	UpdateData(FALSE);

	}

/******************************************************************************
 *
 * MODULE:	    PopulateMerchantDatabaseStructureFromDialog
 *
 * DESCRIPTION: This function populates MGF01 structure.
 *
 * AUTHOR:      Abhishek
 *
 * INPUTS:      pointer to MGF01 function             
 *
 * OUTPUTS:     None
 *
 * RTRN VALUE:  None
 *
 * MODIFICATIONS:
 *
 ******************************************************************************/
/*BOOL CMerchantGroupDefinition::PopulateMerchantDatabaseStructureFromDialog ( pMGF01 psMerchantGroupRecord)
{
	ZeroMemory (psMerchantGroupRecord, sizeof(MGF01)) ;
	
	// save all previous data
	if ( m_nDialogMode == MODE_EDIT )
		memcpy ( psMerchantGroupRecord, &sMerchantGroupRecordOld, sizeof (MGF01));
    
	memcpy ((LPTSTR) psMerchantGroupRecord->primary_key.merchant_group_id, m_StrMerchantGroupId, sizeof (psMerchantGroupRecord->primary_key.merchant_group_id));
	
	if (!pMerchantGroupGeneral->PopulateDatabaseFromGeneralDialog ( psMerchantGroupRecord ))
		return FALSE;
	
	return TRUE;
}  /* function PopulateMerchantDatabaseStructureFromDialog */

BOOL CMerchantGroupDefinition::OnApply() 
{	
	MGF01 sMerchantGroupDatabaseStructure;
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
		memset(&sMerchantGroupDatabaseStructure, 0, sizeof(sMerchantGroupDatabaseStructure)) ;
		if (!PopulateMerchantDatabaseStructureFromDialog (&sMerchantGroupDatabaseStructure)) 
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
		MGF01 sMerchantDatabaseStructure;
        memset(&sMerchantDatabaseStructure, 0, sizeof(MGF01)) ;

		PopulateMerchantDatabaseStructureFromDialog (&sMerchantDatabaseStructure );
		if(ValidateDate(sMerchantDatabaseStructure.effective_date))
		{

			rcode = txdsapi_update_record ( (pBYTE)&sMerchantDatabaseStructure, sizeof (MGF01), MGF01_DATA, strErrorMessage );
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
				pMerchantGroupGeneral->CompareStructures();
				
				pMerchantGroupGeneral->EnableMerchantGroupDialog(FALSE);
				if ( pMerchantGroupGeneral->m_bGeneral)
				{
					pMerchantGroupGeneral->CleanGeneralDialog();
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

				m_CntlMerchantGroupId.EnableWindow(TRUE);
				
				SetModified ( FALSE );

				m_nDialogMode = MODE_DONE;
				pMerchantGroupGeneral->m_nDialogMode = MODE_DONE;  
				pChangeHistoryPage->m_nDialogMode = MODE_DONE; 
			
				//return CPropertyPage::OnApply();
				return FALSE;
			}
		}
		else
		{
			memcpy ((LPSTR)m_strCurrentData, "Invlid date Format, Currect format is (CCYYMMDD)", sizeof (m_strCurrentData));		
				//PopulateHistoryTable();
				((CPropertySheet*) this->GetParent())->SetActivePage(0);
				if (!UpdateData(FALSE)){
					AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);	
					}
				return FALSE;
		}
	}
	/*********************************************************************/
	if(ValidateDate(sMerchantGroupDatabaseStructure.effective_date))
	{
		rcode = txdsapi_insert_record ( (pBYTE)&sMerchantGroupDatabaseStructure, sizeof (MGF01), MGF01_DATA, strErrorMessage );
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
				memcpy ((LPSTR)m_strCurrentData, "Record Added", sizeof (m_strCurrentData));		
				//PopulateHistoryTable();
				((CPropertySheet*) this->GetParent())->SetActivePage(0);
				m_StrMessageLbl = "Record Added";	
				if (!UpdateData(FALSE)){
					AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
				}
		}
	}
	else
	{
		memcpy ((LPSTR)m_strCurrentData, "Invlid date Format, Currect format is (CCYYMMDD)", sizeof (m_strCurrentData));		
				//PopulateHistoryTable();
				((CPropertySheet*) this->GetParent())->SetActivePage(0);
				if (!UpdateData(FALSE)){
					AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);	
					}
				return FALSE;
	}
	return CPropertyPage::OnApply();
}

void CMerchantGroupDefinition::OnCancel() 
{
	// TODO: Add your specialized code here and/or call the base class
	m_StrMessageLbl = _T("");
	CPropertyPage::OnCancel();
	pMerchantGroupGeneral->CleanGeneralDialog();
}

BOOL CMerchantGroupDefinition::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CPropertyPage::OnCommand(wParam, lParam);
}

BOOL CMerchantGroupDefinition::CheckDialogForCompleteness()
{
 CMyUtils myUtilsObject;


    if (!UpdateData(TRUE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}

    if (myUtilsObject.IsEmpty (m_StrMerchantGroupId, this))
    {
		m_StrMessageLbl = "Please enter Merchant Group ID";	
		m_CntlMerchantGroupId.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		m_nDialogMode = MODE_DONE;
		return FALSE;
	}

	return TRUE;
}

BOOL CMerchantGroupDefinition::ValidateDate ( CString strDate )
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


void CMerchantGroupDefinition::PopulateHistoryTable()
{
	HST01 sHistoryData;
    CHAR dest_str[18];   
	CMyUtils myUtilsObject;
	char strErrorMsg[200] = "";
    char strDate[9] = "";
    char strTime[7] = "";
	char strPrimaryKey[50];
	CHAR strDisplayErrMsg[250];
    CString temp;

    memset ( strDate, 0, sizeof (strDate));
    memset ( strTime, 0, sizeof (strTime));
	memset ( strPrimaryKey, 0, sizeof (strPrimaryKey));

	strcpy ( strPrimaryKey, m_StrMerchantGroupId);
	    
    myUtilsObject.GetSystemDate(strDate);
    myUtilsObject.GetSystemTime(strTime);

	//  generate a timestamp for the current Greenwich Mean Time
	// in format YYYYMMDDHHIISSJJJ .

    pintime_get_gmt ( dest_str );

    ZeroMemory ( &sHistoryData, sizeof (HST01));
    CHAR wstation[21]; DWORD wsize;

    wsize = sizeof( wstation );
    GetComputerName( wstation, &wsize );

	memcpy ((LPTSTR) sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
    memcpy ((LPTSTR) sHistoryData.file_name,     (LPCTSTR)"MGF01", sizeof (sHistoryData.file_name));
	memcpy ((LPTSTR) sHistoryData.field_tag,     (LPCTSTR)m_strFieldTag, sizeof (sHistoryData.file_name));
    memcpy ((LPTSTR) sHistoryData.file_primary_key,  (LPCTSTR)strPrimaryKey, sizeof (sHistoryData.file_primary_key));
    memcpy ((LPTSTR) sHistoryData.primary_key.unique_id,     (LPCTSTR)dest_str, sizeof (sHistoryData.primary_key.unique_id));        
    memcpy ((LPTSTR) sHistoryData.time_modified,     (LPCTSTR)strTime, sizeof (sHistoryData.time_modified)); 
    memcpy ((LPTSTR) sHistoryData.current_data, (LPCTSTR)m_strCurrentData, sizeof (sHistoryData.current_data));
	memcpy ((LPTSTR) sHistoryData.previous_data, (LPCTSTR)m_strPreviousData, sizeof (sHistoryData.previous_data));
	memcpy ((LPTSTR)sHistoryData.changed_by,(LPTSTR)ucf01.primary_key.name, sizeof(sHistoryData.changed_by));
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


BOOL CMerchantGroupDefinition::PopulateMerchantDatabaseStructureFromDialog (pMGF01 psMerchantGroupRecord)
{
	memset (psMerchantGroupRecord,0, sizeof(MGF01)) ;
	
	// save all previous data
	if ( m_nDialogMode == MODE_EDIT )
		memcpy ( psMerchantGroupRecord, &sMerchantGroupRecordOld, sizeof (MGF01));
	
	memcpy ((LPTSTR) psMerchantGroupRecord->primary_key.merchant_group_id, m_StrMerchantGroupId, sizeof (psMerchantGroupRecord->primary_key.merchant_group_id));

	if(!pMerchantGroupGeneral->PopulateDatabaseFromGeneralDialog(psMerchantGroupRecord))
		return FALSE;


	return TRUE;
}

void CMerchantGroupDefinition::PopulateMerchantDialogFromDatabase (MGF01 sMerchantGroupRecord)
{
	memcpy(&pMerchantGroupGeneral->sMerchantGroupRecordOld, &sMerchantGroupRecord, sizeof(MGF01));

	pMerchantGroupGeneral->PopulateGeneralDialogFromDatabase(sMerchantGroupRecord);
}

