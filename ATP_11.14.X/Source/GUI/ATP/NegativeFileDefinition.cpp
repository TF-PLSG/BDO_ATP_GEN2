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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\NegativeFileDefinition.cpp  $
   
      Rev 1.7   Dec 01 2003 11:29:50   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.6   Jan 05 2000 14:54:08   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:56:08   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:58   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:04   iarustam
   Initial Release
   
      Rev 1.3   May 10 1999 11:30:22   dperson
   Standardized error messages
   
      Rev 1.2   Apr 29 1999 14:56:48   iarustam
   Bug fixes
   
      Rev 1.1   Mar 16 1999 16:30:14   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:52   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:48   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:59:38   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:09:12   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:44:14   iarustam
    
*
************************************************************************************/
// NegativeFileDefinition.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "NegativeFileDefinition.h"
#include "MyUtils.h"
#include "NameList.h"
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

/////////////////////////////////////////////////////////////////////////////
// CNegativeFileDefinition property page

IMPLEMENT_DYNCREATE(CNegativeFileDefinition, CPropertyPage)

CNegativeFileDefinition::CNegativeFileDefinition() : CPropertyPage(CNegativeFileDefinition::IDD)
{
	//{{AFX_DATA_INIT(CNegativeFileDefinition)
	m_StrMessageLbl = _T("");
	m_StrNegGroupIDFld = _T("");
	//}}AFX_DATA_INIT
}

CNegativeFileDefinition::~CNegativeFileDefinition()
{
}

void CNegativeFileDefinition::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNegativeFileDefinition)
	DDX_Control(pDX, IDC_VIEW_BUTTON, m_ViewButton);
	DDX_Control(pDX, IDC_UPDATE_BUTTON, m_UpdateButton);
	DDX_Control(pDX, IDC_NEG_GROUP_ID_FLD, m_CntlNegGroupIDFld);
	DDX_Control(pDX, IDC_LIST_BUTTON, m_ListButton);
	DDX_Control(pDX, IDC_DELETE_BUTTON, m_DeleteButton);
	DDX_Control(pDX, IDC_ADD_BUTTON, m_AddButton);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	DDX_Text(pDX, IDC_NEG_GROUP_ID_FLD, m_StrNegGroupIDFld);
	DDV_MaxChars(pDX, m_StrNegGroupIDFld, 10);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNegativeFileDefinition, CPropertyPage)
	//{{AFX_MSG_MAP(CNegativeFileDefinition)
	ON_BN_CLICKED(IDC_ADD_BUTTON, OnAddButton)
	ON_BN_CLICKED(IDC_DELETE_BUTTON, OnDeleteButton)
	ON_BN_CLICKED(IDC_LIST_BUTTON, OnListButton)
	ON_BN_CLICKED(IDC_UPDATE_BUTTON, OnUpdateButton)
	ON_BN_CLICKED(IDC_VIEW_BUTTON, OnViewButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNegativeFileDefinition message handlers

BOOL CNegativeFileDefinition::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();	
	
	CFont m_font;
	LOGFONT lf;
	CString temp;
	int nIndex = -1;

	memset ( &lf, 0, sizeof (lf));
	lf.lfHeight = 12;
	lf.lfWeight = 700;
	strcpy(lf.lfFaceName, "Arial");
	m_font.CreateFontIndirect (&lf);
	GetDlgItem(IDC_MESSAGE_LBL)->SetFont (&m_font);

	m_StrMessageLbl = _T("");
	m_StrNegGroupIDFld = _T("");	

	for ( int i = 0; i <=15; i++ )
	{
		temp = ucf01.control[i].subsystem_name;
		if ( strcmp ( temp, "Merchant Control" ) == 0)
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



	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CNegativeFileDefinition::OnAddButton() 
{    
	char strErrorMessage[200];
	NGF01 sNegFileRecord;    
    CMyUtils myUtilsObject;

	m_StrMessageLbl = "";

	m_nDialogMode = MODE_ADD;

	 if (!CheckDialogForCompleteness())
        return;

	pGeneralPage->m_nDialogMode = MODE_ADD;	 
	pChangeHistoryPage->m_nDialogMode = MODE_ADD;	
	
	ZeroMemory(&sNegFileRecord,sizeof(NGF01)) ;

    memcpy ((LPSTR)(sNegFileRecord.primary_key.negative_group_id), m_StrNegGroupIDFld , sizeof (sNegFileRecord.primary_key.negative_group_id));

	int rcode = txdsapi_get_record( (pBYTE)&sNegFileRecord,sizeof (NGF01),NGF01_DATA, strErrorMessage );
	if( rcode == PTEMSG_OK )
    {
      m_StrMessageLbl = "ERROR: Negative Group ID already exists";
      m_nDialogMode = MODE_DONE;

		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return ;
    }
	if ( pGeneralPage->m_bGeneral )
	{
		pGeneralPage->CleanGeneralDialog();
		pGeneralPage->EnableGeneralDialog(FALSE);
	}
			
	if ( pChangeHistoryPage->m_bChangeHistory )
		pChangeHistoryPage->CleanChangeHistory();
	
	pGeneralPage->m_StrNegGroupIDFld = m_StrNegGroupIDFld;
    
	pChangeHistoryPage->bNext = FALSE;
	((CPropertySheet*) this->GetParent())->SetWindowText("Add Negative Group ID");
	//set version info in the title bar
	CString appProdVer;
	CString lpszTitle;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
	lpszTitle = lpszTitle + " - Version - ";
	lpszTitle += appProdVer;
	((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);
	m_CntlNegGroupIDFld.EnableWindow(FALSE);
	

	if ( pGeneralPage->m_bGeneral )
	{
	pGeneralPage->UpdateData(FALSE);
	pGeneralPage->EnableGeneralDialog( TRUE );
	strcpy((char *)pGeneralPage->sNegBinRecord.primary_key.low, " ");
	strcpy((char *)pGeneralPage->sNegBinRecord.primary_key.high, " ");		
	pGeneralPage->PopulateNegBinListDialog();

	}

	((CPropertySheet*) this->GetParent())->SetActivePage(1);
	SetModified(FALSE);
	
}

void CNegativeFileDefinition::OnDeleteButton() 
{
	CString strMessage;
    int nReturnCode;
    NGF01 sNegGroupRecord;
	char strErrorMessage[200];
    
     m_nDialogMode = MODE_EDIT;
	 m_StrMessageLbl = "";

    if (!CheckDialogForCompleteness())
        return;     

    if (!UpdateData(TRUE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}
    
	strMessage.Format ("Are you sure you want to delete Negative Group ID %s?", (LPCTSTR) m_StrNegGroupIDFld) ;

	nReturnCode = AfxMessageBox ((LPCTSTR)strMessage, MB_OKCANCEL|MB_ICONQUESTION) ;
	if (nReturnCode != IDOK) 
		return ;

	memcpy (sNegGroupRecord.primary_key.negative_group_id,m_StrNegGroupIDFld, sizeof (sNegGroupRecord.primary_key.negative_group_id)) ;

    int rcode = txdsapi_delete_record ((pBYTE)&sNegGroupRecord, sizeof ( NGF01), NGF01_DATA, strErrorMessage) ;
	if ( rcode != PTEMSG_OK )
    {
		 switch( rcode )
      {
         case PTEMSG_NOT_FOUND:
			 sprintf( strErrorMessage, "ERROR: Negative Group ID record for %s does not exist", m_StrNegGroupIDFld );
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
      PopulateHistoryTable();
      m_StrMessageLbl = "Record Deleted";

		m_StrNegGroupIDFld = "";

		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
        memcpy ((LPSTR)m_strCurrentData, "Record Deleted", sizeof (m_strCurrentData));

	
		if ( pGeneralPage->m_bGeneral )
		{
		pGeneralPage->CleanGeneralDialog();
		pGeneralPage->EnableGeneralDialog(FALSE);
		}		

		if ( pChangeHistoryPage->m_bChangeHistory )
			pChangeHistoryPage->CleanChangeHistory();

		((CPropertySheet*) this->GetParent())->SetActivePage(0);
		SetModified ( FALSE );
    }

		m_CntlNegGroupIDFld.EnableWindow(TRUE);	
	
}

void CNegativeFileDefinition::OnListButton() 
{
	CNameList listDlg;
    int nReturnCode;

	m_StrMessageLbl = "";

	UpdateData(TRUE);

    listDlg.m_nIndicator = MODE_NEGFILE;
    listDlg.strPrimaryKey1 = m_StrNegGroupIDFld;   

    nReturnCode = listDlg.DoModal();
    if ( nReturnCode == IDOK)
        m_StrNegGroupIDFld = listDlg.m_strName;
        
    if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}		
	m_CntlNegGroupIDFld.EnableWindow(TRUE);

}

void CNegativeFileDefinition::OnUpdateButton() 
{
	NGF01 sNegGroupRecord;
    HST01 sHistoryRecord;
	char strErrorMessage[200];

    m_nDialogMode = MODE_EDIT;
	pGeneralPage->m_nDialogMode = MODE_EDIT;	
	pChangeHistoryPage->m_nDialogMode = MODE_EDIT;	

	m_StrMessageLbl = "";

    if (!CheckDialogForCompleteness())
        return;
   
    memcpy ((LPSTR)(sNegGroupRecord.primary_key.negative_group_id), m_StrNegGroupIDFld, sizeof (sNegGroupRecord.primary_key.negative_group_id));   

	int rcode = txdsapi_get_record( (pBYTE)&sNegGroupRecord,sizeof (NGF01),NGF01_DATA, strErrorMessage );
	if ( rcode != PTEMSG_OK )
	{       
       	 switch( rcode )
      {
         case PTEMSG_NOT_FOUND:
			 sprintf( strErrorMessage, "ERROR: Negative Group ID record for %s does not exist", m_StrNegGroupIDFld );
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
	
	
    ZeroMemory (&sHistoryRecord, sizeof (sHistoryRecord));

    memcpy ((LPSTR)(sHistoryRecord.primary_key.unique_id), "                 ", sizeof (sHistoryRecord.primary_key.unique_id));
    memcpy ((LPSTR)(sHistoryRecord.file_name), "NGF01", sizeof (sHistoryRecord.file_name));
    memcpy ((LPSTR)(sHistoryRecord.file_primary_key), sNegGroupRecord.primary_key.negative_group_id, sizeof (sHistoryRecord.file_primary_key));

    memcpy ( &(pChangeHistoryPage->sHistoryRecord), &sHistoryRecord, sizeof(sHistoryRecord));      

	((CPropertySheet*) this->GetParent())->SetWindowText("Update Negative Group ID");
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

	PopulateNegGroupDialogFromDatabase ( sNegGroupRecord );
	((CPropertySheet*) this->GetParent())->SetActivePage(1);  

	pGeneralPage->SetModified ( FALSE );	

	m_CntlNegGroupIDFld.EnableWindow(FALSE);	
}

void CNegativeFileDefinition::OnViewButton() 
{
	NGF01 sNegGroupRecord;
    HST01 sHistoryRecord;
	char strErrorMessage[200];

    m_nDialogMode = MODE_VIEW;
	pGeneralPage->m_nDialogMode = MODE_VIEW;
	pChangeHistoryPage->m_nDialogMode = MODE_VIEW;	


	m_StrMessageLbl = "";

    if (!CheckDialogForCompleteness())
        return;
   
    memcpy ((LPSTR)(sNegGroupRecord.primary_key.negative_group_id), m_StrNegGroupIDFld, sizeof (sNegGroupRecord.primary_key.negative_group_id));   

	int rcode = txdsapi_get_record( (pBYTE)&sNegGroupRecord,sizeof (NGF01),NGF01_DATA, strErrorMessage );
	if ( rcode != PTEMSG_OK )
	{       
       	 switch( rcode )
      {
         case PTEMSG_NOT_FOUND:
			 sprintf( strErrorMessage, "ERROR: Negative Group ID record for %s does not exist", m_StrNegGroupIDFld );
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
	
	
    ZeroMemory (&sHistoryRecord, sizeof (sHistoryRecord));

    memcpy ((LPSTR)(sHistoryRecord.primary_key.unique_id), "                 ", sizeof (sHistoryRecord.primary_key.unique_id));
    memcpy ((LPSTR)(sHistoryRecord.file_name), "NGF01", sizeof (sHistoryRecord.file_name));
    memcpy ((LPSTR)(sHistoryRecord.file_primary_key), sNegGroupRecord.primary_key.negative_group_id, sizeof (sHistoryRecord.file_primary_key));

    memcpy ( &(pChangeHistoryPage->sHistoryRecord), &sHistoryRecord, sizeof(sHistoryRecord));    


	if ( pChangeHistoryPage->m_bChangeHistory )
		pChangeHistoryPage->PopulateHistoryDialog();

	PopulateNegGroupDialogFromDatabase ( sNegGroupRecord );

	pGeneralPage->SetModified ( FALSE );	
	m_CntlNegGroupIDFld.EnableWindow(TRUE);	
	((CPropertySheet*) this->GetParent())->SetActivePage(1);  
	((CPropertySheet*) this->GetParent())->SetWindowText("View Negative Group ID");
	//set version info in the title bar
	CString appProdVer;
	CString lpszTitle;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
	lpszTitle = lpszTitle + " - Version - ";
	lpszTitle += appProdVer;
	((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);

	
}

void CNegativeFileDefinition::OnCancel() 
{
	if ( pGeneralPage->m_bGeneral )
	{
		pGeneralPage->CleanGeneralDialog();
		pGeneralPage->EnableGeneralDialog(FALSE);
	}	
	
	CPropertyPage::OnCancel();
}

void CNegativeFileDefinition::PopulateNegGroupDialogFromDatabase( NGF01 sNegGroupRecord )
{	
	if ( !pGeneralPage->m_bGeneral )
		memcpy ( &pGeneralPage->sNegGroupRecordOld, &sNegGroupRecord, sizeof ( NGF01));
	else
	{
		pGeneralPage->PopulateGeneralDialogFromDatabase(sNegGroupRecord);	
		strcpy((char *)pGeneralPage->sNegBinRecord.primary_key.low, " ");
		strcpy((char *)pGeneralPage->sNegBinRecord.primary_key.high, " ");		
		pGeneralPage->PopulateNegBinListDialog();
	}

}

BOOL CNegativeFileDefinition::PopulateNegGroupDatabaseStructureFromDialog ( pNGF01 psNegGroupRecord )
{
	ZeroMemory (psNegGroupRecord, sizeof(NGF01)) ;	
    
	memcpy ((LPTSTR) psNegGroupRecord->primary_key.negative_group_id, m_StrNegGroupIDFld, sizeof (psNegGroupRecord->primary_key.negative_group_id));

	pGeneralPage->PopulateDatabaseFromGeneralDialog ( psNegGroupRecord );	

	return TRUE;
}

void CNegativeFileDefinition::PopulateHistoryTable()
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

	strcpy ( strPrimaryKey, m_StrNegGroupIDFld);
    
    myUtilsObject.GetSystemDate(strDate);  

    myUtilsObject.GetSystemTime(strTime);
    pintime_get_gmt ( dest_str );
    ZeroMemory ( &sHistoryData, sizeof (HST01));
    memcpy ((LPTSTR) sHistoryData.file_name,     (LPCTSTR)"NGF01", sizeof (sHistoryData.file_name));
    temp = strDate;
    temp = temp.Right(4) + temp.Left(2) + temp.Mid(2,2);
    
    CHAR wstation[21]; DWORD wsize;

    wsize = sizeof( wstation );
    GetComputerName( wstation, &wsize );

	memcpy ((LPTSTR)sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
    memcpy ((LPTSTR) sHistoryData.date_modified, temp, sizeof (sHistoryData.date_modified));
    memcpy ((LPTSTR) sHistoryData.file_primary_key,  (LPCTSTR)strPrimaryKey, sizeof (sHistoryData.file_primary_key));
    memcpy ((LPTSTR) sHistoryData.primary_key.unique_id,     (LPCTSTR)dest_str, sizeof (sHistoryData.primary_key.unique_id));        
    memcpy ((LPTSTR) sHistoryData.time_modified,     (LPCTSTR)strTime, sizeof (sHistoryData.time_modified));              
    memcpy ((LPTSTR)sHistoryData.current_data, (LPCTSTR)m_strCurrentData, sizeof (sHistoryData.current_data));
	memcpy ((LPTSTR)sHistoryData.changed_by,(LPTSTR)ucf01.primary_key.name, sizeof(sHistoryData.changed_by));

    int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
	if ( rcode != PTEMSG_OK )
	{
		m_StrMessageLbl = strErrorMsg;
		m_nDialogMode = MODE_DONE;
		UpdateData( FALSE );
	}
    
}

BOOL CNegativeFileDefinition::CheckDialogForCompleteness()
{
    CMyUtils myUtilsObject;

    if (!UpdateData(TRUE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}

	if (myUtilsObject.IsEmpty (m_StrNegGroupIDFld, this))
    {
		m_StrMessageLbl = "Please enter Negative Group ID";	
		m_CntlNegGroupIDFld.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		m_nDialogMode = MODE_DONE;
		return FALSE;
	}
	
    return TRUE;
}

BOOL CNegativeFileDefinition::OnApply() 
{
	NGF01 sNegGroupRecord;
	char strErrorMessage[200];
	CMyUtils myUtilsObject;

	if ( m_nDialogMode == MODE_VIEW)
	{
		((CPropertySheet*) this->GetParent())->SetActivePage(0);
		return CPropertyPage::OnApply();
	}

	if (m_nDialogMode == MODE_ADD)
	{			
		memset(&sNegGroupRecord, 0, sizeof(NGF01)) ;
		if (!PopulateNegGroupDatabaseStructureFromDialog (&sNegGroupRecord)) {		
			return FALSE;
		}

		int rcode = txdsapi_insert_record ( (pBYTE)&sNegGroupRecord, 
											 sizeof (NGF01), NGF01_DATA, strErrorMessage );
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
			
			if ( pChangeHistoryPage->m_bChangeHistory )
				pChangeHistoryPage->CleanChangeHistory();

			SetModified ( FALSE );

			m_nDialogMode = MODE_DONE;
			pGeneralPage->m_nDialogMode = MODE_DONE;
			
			pChangeHistoryPage->m_nDialogMode = MODE_DONE;
			
			m_CntlNegGroupIDFld.SetFocus();
			m_CntlNegGroupIDFld.EnableWindow(TRUE);


			return CPropertyPage::OnApply();
		} 
	}
//////////////////////////////////////////////////////////////////////////////////////////
	if (m_nDialogMode == MODE_EDIT)
	{ 
        memset(&sNegGroupRecord, 0, sizeof(NGF01)) ;
		if (!PopulateNegGroupDatabaseStructureFromDialog (&sNegGroupRecord)) {		
			return FALSE;
		}
		int rcode = txdsapi_update_record ( (pBYTE)&sNegGroupRecord,sizeof (NGF01), NGF01_DATA,  strErrorMessage );
		if ( rcode != PTEMSG_OK )
		{ 		
			m_StrMessageLbl = strErrorMessage;
			m_nDialogMode = MODE_DONE;
			((CPropertySheet*) this->GetParent())->SetActivePage(0);
			((CPropertySheet*) this->GetParent())->SetWindowText("Negative Merchant ID Definition");
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
			

			if ( pGeneralPage->m_bGeneral )
			{
			pGeneralPage->CleanGeneralDialog();
			pGeneralPage->EnableGeneralDialog(FALSE);
			}
	
			if ( pChangeHistoryPage->m_bChangeHistory )
				pChangeHistoryPage->CleanChangeHistory();

			((CPropertySheet*) this->GetParent())->SetActivePage(0);
			((CPropertySheet*) this->GetParent())->SetWindowText("Negative Merchant ID Definition");
			//set version info in the title bar
			CString appProdVer;
			CString lpszTitle;
			appProdVer.LoadString(IDS_PRODUCT_VERSION);
			((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
			lpszTitle = lpszTitle + " - Version - ";
			lpszTitle += appProdVer;
			((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);

			m_CntlNegGroupIDFld.EnableWindow(TRUE);

			SetModified ( FALSE );

			m_nDialogMode = MODE_DONE;
			pGeneralPage->m_nDialogMode = MODE_DONE;
			pChangeHistoryPage->m_nDialogMode = MODE_DONE;

			return CPropertyPage::OnApply();
		}
	
    }
	m_nDialogMode = MODE_DONE;
	
	return CPropertyPage::OnApply();
}

BOOL CNegativeFileDefinition::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}
