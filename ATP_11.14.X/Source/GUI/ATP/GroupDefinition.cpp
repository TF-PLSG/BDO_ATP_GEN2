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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\GroupDefinition.cpp  $
   
      Rev 1.13   Dec 01 2003 11:29:22   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.12   Jan 25 2000 14:00:20   iarustam
   bug fixes
   
      Rev 1.11   Jan 14 2000 16:25:02   mbysani
   bug fixes
   
      Rev 1.10   Jan 12 2000 14:14:34   mbysani
   Changes mades acc to bugs
   
      Rev 1.4   Oct 15 1999 17:11:02   iarustam
   bug fixes
   
      Rev 1.3   Sep 30 1999 16:17:14   npogosya
   Bug fixes
   
      Rev 1.1   Sep 22 1999 15:49:54   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jun 28 1999 13:58:34   iarustam
   Initial Release
   

*
************************************************************************************/
// GroupDefinition.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "GroupDefinition.h"


#include "MyUtils.h"
//#include "atpdb.h"
#include "NameList.h"
#include "SharedMem.h"
extern "C"
{
#include "PteTime.h"
}
#include "GroupPropertySheet.h"

//#include "htmlhelp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern CString name;
extern UCF01 ucf01;
extern CSharedMem shrmem;

/////////////////////////////////////////////////////////////////////////////
// CGroupDefinition property page

IMPLEMENT_DYNCREATE(CGroupDefinition, CPropertyPage)

CGroupDefinition::CGroupDefinition() : CPropertyPage(CGroupDefinition::IDD)
{
	//{{AFX_DATA_INIT(CGroupDefinition)
	m_StrGroupIDFld = _T("");
	m_StrMessageLbl = _T("");
	//}}AFX_DATA_INIT
}

CGroupDefinition::~CGroupDefinition()
{
}

void CGroupDefinition::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGroupDefinition)
	DDX_Control(pDX, IDC_USER_VIEW_BUTTON, m_ViewButton);
	DDX_Control(pDX, IDC_USER_UPDATE_BUTTON, m_UpdateButton);
	DDX_Control(pDX, IDC_USER_LIST_BUTTON, m_ListButton);
	DDX_Control(pDX, IDC_USER_DELETE_BUTTON, m_DeleteButton);
	DDX_Control(pDX, IDC_USER_ADD_BUTTON, m_AddButton);
	DDX_Control(pDX, IDC_GROUP_ID_FLD, m_CntlGroupIDFld);
	DDX_Text(pDX, IDC_GROUP_ID_FLD, m_StrGroupIDFld);
	DDV_MaxChars(pDX, m_StrGroupIDFld, 15);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGroupDefinition, CPropertyPage)
	//{{AFX_MSG_MAP(CGroupDefinition)
	ON_BN_CLICKED(IDC_USER_ADD_BUTTON, OnUserAddButton)
	ON_BN_CLICKED(IDC_USER_DELETE_BUTTON, OnUserDeleteButton)
	ON_BN_CLICKED(IDC_USER_LIST_BUTTON, OnUserListButton)
	ON_BN_CLICKED(IDC_USER_UPDATE_BUTTON, OnUserUpdateButton)
	ON_BN_CLICKED(IDC_USER_VIEW_BUTTON, OnUserViewButton)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGroupDefinition message handlers

BOOL CGroupDefinition::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	CFont m_font;
	LOGFONT lf;
	int nIndex = -1;
	CString temp;


	memset ( &lf, 0, sizeof (lf));
	lf.lfHeight = 12;
	lf.lfWeight = 700;
	strcpy(lf.lfFaceName, "Arial");
	m_font.CreateFontIndirect (&lf);
	GetDlgItem(IDC_MESSAGE_LBL)->SetFont (&m_font);

	m_StrGroupIDFld = "";
	m_StrMessageLbl = "";
	
	for ( int i = 0; i <=20; i++ )
	{
		temp = ucf01.control[i].subsystem_name;
		if ( strcmp ( temp, "System Administration" ) == 0)
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
   m_nDialogMode = MODE_DONE;

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CGroupDefinition::OnUserAddButton() 
{
   UCF02 sGroupRecord;
	char strErrorMessage[200];   
   CMyUtils myUtilsObject;

	m_StrMessageLbl = "";

	m_nDialogMode = MODE_ADD;  

	if (!CheckDialogForCompleteness())
        return;

	 
	pAccessPage->m_nDialogMode = MODE_ADD;  
	pWorkstationPage->m_nDialogMode = MODE_ADD;
//    pChangeHistoryPage->m_nDialogMode = MODE_ADD; 


	ZeroMemory(&sGroupRecord,sizeof(UCF02)) ;

    memcpy ((LPSTR)(sGroupRecord.primary_key.group_id), m_StrGroupIDFld , sizeof (sGroupRecord.primary_key.group_id));

	int rcode = txdsapi_get_record  ((pBYTE)&sGroupRecord,sizeof (UCF02), UCF02_DATA, strErrorMessage);
	if ( rcode == PTEMSG_OK )
	{   
		m_StrMessageLbl = "ERROR: Group ID already exists";
		m_nDialogMode = MODE_DONE; 

		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return ;
    }
	
//	pChangeHistoryPage->bNext = FALSE;
	((CPropertySheet*) this->GetParent())->SetWindowText("Add Group");
	//set version info in the title bar
	CString appProdVer;
	CString lpszTitle;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
	lpszTitle = lpszTitle + " - Version - ";
	lpszTitle += appProdVer;
	((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);

	if ( pAccessPage->m_bAccess )
	{
		pAccessPage->UpdateData(FALSE);
		pAccessPage->EnableGroupAccessDialog(TRUE);
		pAccessPage->CleanGroupAccessDialog();
	}

	if ( pWorkstationPage->m_bWorkstation )
	{
		pWorkstationPage->UpdateData(FALSE);
		pWorkstationPage->EnableGroupWorkstationDialog (TRUE);
		pWorkstationPage->CleanGroupWorkstationDialog ();
	}
    pAccessPage->m_StrGroupIDFld = m_StrGroupIDFld;
   
    pWorkstationPage->m_StrGroupIDFld = m_StrGroupIDFld;

	((CPropertySheet*) this->GetParent())->SetActivePage(2);
	((CPropertySheet*) this->GetParent())->SetActivePage(1);
	m_CntlGroupIDFld.EnableWindow(FALSE);
	
}

void CGroupDefinition::OnUserDeleteButton() 
{
	CString strMessage;
    int nReturnCode;
    UCF02 sGroupRecord;
	char strErrorMessage[200];
	int len_GroupID = 0;
    
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
    
	strMessage.Format ("Are you sure you want to delete %s ?", (LPCTSTR) m_StrGroupIDFld) ;

	nReturnCode = AfxMessageBox ((LPCTSTR)strMessage, MB_OKCANCEL|MB_ICONQUESTION) ;
	if (nReturnCode != IDOK) return ;

	len_GroupID = strlen(m_StrGroupIDFld);
	if(len_GroupID>0)
	{
		memcpy ((LPTSTR)sGroupRecord.primary_key.group_id, m_StrGroupIDFld, sizeof (sGroupRecord.primary_key.group_id)) ;
	}

	int rcode = txdsapi_delete_record  ( (pBYTE)&sGroupRecord, sizeof (UCF02), UCF02_DATA, strErrorMessage );
	if ( rcode != PTEMSG_OK )
	{     
      switch( rcode )
      {
         case PTEMSG_NOT_FOUND:
            sprintf( strErrorMessage, "ERROR: Group record for %s does not exist", m_StrGroupIDFld );
            break;
      }

        m_StrMessageLbl = strErrorMessage;
		m_nDialogMode = MODE_DONE;
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
      
      return;
	}

    else
    {
        m_StrMessageLbl = "Record Deleted";
		

		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
        memset ( m_strCurrentData, 0, sizeof (m_strCurrentData));
        memcpy ((LPSTR)m_strCurrentData, "Record Deleted", sizeof (m_strCurrentData));
        PopulateHistoryTable();
		m_StrGroupIDFld = "";
		if ( pAccessPage->m_bAccess )
		{
			pAccessPage->CleanGroupAccessDialog();
			pAccessPage->EnableGroupAccessDialog(FALSE);
		}

		if ( pWorkstationPage->m_bWorkstation )
		{
			pWorkstationPage->CleanGroupWorkstationDialog ();
			pWorkstationPage->EnableGroupWorkstationDialog(FALSE);
		}

		if ( pChangeHistoryPage->m_bChangeHistory )
			pChangeHistoryPage->CleanChangeHistory();
		((CPropertySheet*) this->GetParent())->SetActivePage(0);
		m_nDialogMode = MODE_DONE;
		SetModified ( FALSE );
    }
	
}

void CGroupDefinition::OnUserListButton() 
{
	CNameList listDlg;
    int nReturnCode;
	
	m_StrMessageLbl = "";
	UpdateData(TRUE);
    listDlg.m_nIndicator = MODE_GROUP;
	listDlg.strPrimaryKey1 = m_StrGroupIDFld;
 //   listDlg.SetWindowText ( "User List");

    nReturnCode = listDlg.DoModal();
    if ( nReturnCode == IDOK)
        m_StrGroupIDFld = listDlg.m_strName;
        
    if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}		
	
}

void CGroupDefinition::OnUserUpdateButton() 
{
    UCF02 sGroupRecord;
    HST01 sHistoryRecord;
	char strErrorMessage[200];
	int len_GroupID = 0;

    m_nDialogMode = MODE_EDIT;

	m_StrMessageLbl = "";

    if (!CheckDialogForCompleteness())
        return;

	if ( pChangeHistoryPage->m_bChangeHistory )
		pChangeHistoryPage->PopulateHistoryDialog();
	if ( pAccessPage->m_bAccess )
		pAccessPage->CleanGroupAccessDialog();
	if ( pWorkstationPage->m_bWorkstation )
		pWorkstationPage->CleanGroupWorkstationDialog();


    /*m_pUserSheet->m_AccessPage.m_psp.dwFlags &= ~PSP_HASHELP; 
    m_pUserSheet->m_WorkstationPage.m_psp.dwFlags &= ~PSP_HASHELP; 
    m_pUserSheet->m_ChangeHistoryPage.m_psp.dwFlags &= ~PSP_HASHELP;

    m_pUserSheet->m_psh.dwFlags |= PSH_NOAPPLYNOW ; 
    m_pUserSheet->m_psh.dwFlags &= ~PSH_HASHELP;*/
	
	len_GroupID = strlen(m_StrGroupIDFld);
	if(len_GroupID>0)
	{
		memcpy ((LPSTR)(sGroupRecord.primary_key.group_id), m_StrGroupIDFld, sizeof (sGroupRecord.primary_key.group_id) );
	}
   // lstrcpy ((LPSTR)(sGroupRecord.group_id), m_StrGroupIDFld );
    //lstrcpy ((LPSTR)(sGroupRecord.password), m_StrUserPasswordFld );

	int rcode = txdsapi_get_record  ((pBYTE)&sGroupRecord,sizeof (UCF02), UCF02_DATA, strErrorMessage);
	if ( rcode != PTEMSG_OK )
	{
      switch( rcode )
      {
         case PTEMSG_NOT_FOUND:
            sprintf( strErrorMessage, "ERROR: Group record for %s does not exist", m_StrGroupIDFld );
            break;
      }

      m_StrMessageLbl = strErrorMessage;
		m_nDialogMode = MODE_DONE;
		if (!UpdateData(FALSE))
		{
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
      
      return;
   }

   memcpy ( &sOldUCF02Record, &sGroupRecord, sizeof ( UCF02 ) );

	pAccessPage->m_StrGroupIDFld = m_StrGroupIDFld;
   
    pWorkstationPage->m_StrGroupIDFld = m_StrGroupIDFld;

    ZeroMemory (&sHistoryRecord, sizeof (sHistoryRecord));


    memcpy ((LPSTR)(sHistoryRecord.primary_key.unique_id), "                 ", sizeof (sHistoryRecord.primary_key.unique_id));
    memcpy ((LPSTR)(sHistoryRecord.file_name), "UCF02", sizeof (sHistoryRecord.file_name));
	len_GroupID = strlen((char *)sGroupRecord.primary_key.group_id);
	if(len_GroupID > 0)
	{
		memcpy ((LPSTR)(sHistoryRecord.file_primary_key), (LPSTR)sGroupRecord.primary_key.group_id, sizeof (sHistoryRecord.file_primary_key));
	}
    memcpy ( &(pChangeHistoryPage->sHistoryRecord), &sHistoryRecord, sizeof(sHistoryRecord));
    
//    m_StrGroupIDFld = sGroupRecord.group_id; 
	
	//((CPropertySheet*) this->GetParent())->SetActivePage(3);
	//((CPropertySheet*) this->GetParent())->SetActivePage(2);
	((CPropertySheet*) this->GetParent())->SetActivePage(1);  
    

    pAccessPage->m_nDialogMode = MODE_EDIT;
    pWorkstationPage->m_nDialogMode = MODE_EDIT;
	pChangeHistoryPage->m_nDialogMode = MODE_EDIT;
	pChangeHistoryPage->bNext = FALSE;

	((CPropertySheet*) this->GetParent())->SetWindowText("Update Group");
	//set version info in the title bar
	CString appProdVer;
	CString lpszTitle;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
	lpszTitle = lpszTitle + " - Version - ";
	lpszTitle += appProdVer;
	((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);


	PopulateGroupDialogFromDatabase ( sGroupRecord );
	pWorkstationPage->SetModified ( FALSE );	
	m_CntlGroupIDFld.EnableWindow(FALSE);

	
}

void CGroupDefinition::OnUserViewButton() 
{
    UCF02 sGroupRecord;
    HST01 sHistoryRecord;
	char strErrorMessage[200];
	int len_GroupID = 0;

    m_nDialogMode = MODE_VIEW;

	m_StrMessageLbl = "";

    if (!CheckDialogForCompleteness())
        return;

	if ( pChangeHistoryPage->m_bChangeHistory )
		pChangeHistoryPage->PopulateHistoryDialog();
	if ( pAccessPage->m_bAccess )
		pAccessPage->CleanGroupAccessDialog();
	if ( pWorkstationPage->m_bWorkstation )
		pWorkstationPage->CleanGroupWorkstationDialog();


    /*m_pUserSheet->m_AccessPage.m_psp.dwFlags &= ~PSP_HASHELP; 
    m_pUserSheet->m_WorkstationPage.m_psp.dwFlags &= ~PSP_HASHELP; 
    m_pUserSheet->m_ChangeHistoryPage.m_psp.dwFlags &= ~PSP_HASHELP;

    m_pUserSheet->m_psh.dwFlags |= PSH_NOAPPLYNOW ; 
    m_pUserSheet->m_psh.dwFlags &= ~PSH_HASHELP;*/

	len_GroupID = strlen(m_StrGroupIDFld);
	if(len_GroupID > 0)
	{
		memcpy ((LPSTR)(sGroupRecord.primary_key.group_id), m_StrGroupIDFld, sizeof (sGroupRecord.primary_key.group_id) );
	}
   // lstrcpy ((LPSTR)(sGroupRecord.group_id), m_StrGroupIDFld );
    //lstrcpy ((LPSTR)(sGroupRecord.password), m_StrUserPasswordFld );

	int rcode = txdsapi_get_record  ((pBYTE)&sGroupRecord,sizeof (UCF02), UCF02_DATA, strErrorMessage);
	if ( rcode != PTEMSG_OK )
	{
      switch( rcode )
      {
         case PTEMSG_NOT_FOUND:
            sprintf( strErrorMessage, "ERROR: Group record for %s does not exist", m_StrGroupIDFld );
            break;
      }

      m_StrMessageLbl = strErrorMessage;
	  m_nDialogMode = MODE_DONE;
	  if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
      
      return;
   }

	pAccessPage->m_StrGroupIDFld = m_StrGroupIDFld;
   
    pWorkstationPage->m_StrGroupIDFld = m_StrGroupIDFld;

    ZeroMemory (&sHistoryRecord, sizeof (sHistoryRecord));


    memcpy ((LPSTR)(sHistoryRecord.primary_key.unique_id), "                 ", sizeof (sHistoryRecord.primary_key.unique_id));
    memcpy ((LPSTR)(sHistoryRecord.file_name), "UCF02", sizeof (sHistoryRecord.file_name));
	len_GroupID = strlen((char *)sGroupRecord.primary_key.group_id);
	if(len_GroupID > 0)
	{
		memcpy ((LPSTR)(sHistoryRecord.file_primary_key), (LPSTR)sGroupRecord.primary_key.group_id, sizeof (sHistoryRecord.file_primary_key));
	}

    memcpy ( &(pChangeHistoryPage->sHistoryRecord), &sHistoryRecord, sizeof(sHistoryRecord));
    
//    m_StrGroupIDFld = sGroupRecord.group_id; 
	
	//((CPropertySheet*) this->GetParent())->SetActivePage(3);
	//((CPropertySheet*) this->GetParent())->SetActivePage(2);
	((CPropertySheet*) this->GetParent())->SetActivePage(1);  
    

    pAccessPage->m_nDialogMode = MODE_VIEW;
    pWorkstationPage->m_nDialogMode = MODE_VIEW;
	pChangeHistoryPage->m_nDialogMode = MODE_VIEW;
	pChangeHistoryPage->bNext = FALSE;

	((CPropertySheet*) this->GetParent())->SetWindowText("View Group");
	//set version info in the title bar
	CString appProdVer;
	CString lpszTitle;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
	lpszTitle = lpszTitle + " - Version - ";
	lpszTitle += appProdVer;
	((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);


	PopulateGroupDialogFromDatabase ( sGroupRecord );
	if ( m_nDialogMode != MODE_VIEW )
		pWorkstationPage->SetModified ( TRUE );	
	m_CntlGroupIDFld.EnableWindow(TRUE);

	
}

BOOL CGroupDefinition::CheckDialogForCompleteness()
{
    CMyUtils myUtilsObject;

    if (!UpdateData(TRUE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}

    if (myUtilsObject.IsEmpty (m_StrGroupIDFld, this))
    {
		m_StrMessageLbl = "Please enter Group ID";	
		m_CntlGroupIDFld.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		m_nDialogMode = MODE_DONE;
		return FALSE;
	}

    

    return TRUE;
}

BOOL CGroupDefinition::OnApply() 
{
	UCF02 sGroupDatabaseStructure;
	char strErrorMessage[200];
	
    if (m_nDialogMode == MODE_EDIT)
	{ 
      memset(&sGroupDatabaseStructure, 0, sizeof(sGroupDatabaseStructure)) ;
		if ( !pWorkstationPage->m_bWorkstation )
			((CPropertySheet*) this->GetParent())->SetActivePage(2);
		PopulateGroupDatabaseStructureFromDialog (&sGroupDatabaseStructure );

//		pAccessPage->PopulateHistoryTable();
		int rcode = txdsapi_update_record ( (pBYTE)&sGroupDatabaseStructure, sizeof (UCF02), UCF02_DATA, strErrorMessage );
		if ( rcode != PTEMSG_OK )
		{ 		
			m_StrMessageLbl = strErrorMessage;
			m_nDialogMode = MODE_DONE;
			((CPropertySheet*) this->GetParent())->SetActivePage(0);
			((CPropertySheet*) this->GetParent())->SetWindowText("User Definition");
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
			pAccessPage->CleanGroupAccessDialog();
			pAccessPage->EnableGroupAccessDialog(FALSE);
			if ( pWorkstationPage->m_bWorkstation)
			{
				pWorkstationPage->CleanGroupWorkstationDialog ();
				pWorkstationPage->EnableGroupWorkstationDialog(FALSE);
			}
			
			if ( pChangeHistoryPage->m_bChangeHistory )
				pChangeHistoryPage->CleanChangeHistory();
			((CPropertySheet*) this->GetParent())->SetActivePage(0);
			((CPropertySheet*) this->GetParent())->SetWindowText("Group Definition");
			//set version info in the title bar
			CString appProdVer;
			CString lpszTitle;
			appProdVer.LoadString(IDS_PRODUCT_VERSION);
			((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
			lpszTitle = lpszTitle + " - Version - ";
			lpszTitle += appProdVer;
			((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);

			m_CntlGroupIDFld.EnableWindow(TRUE);
			SetModified ( FALSE );
			m_nDialogMode = MODE_DONE;
			return CPropertyPage::OnApply();
		}
	
    }

	if (m_nDialogMode == MODE_ADD)
	{
	//	pWorkstationPage->PopulateLoginStructure ();
	//	pAccessPage->PopulateControlStructure ();

		memset(&sGroupDatabaseStructure, 0, sizeof(UCF02)) ;
		if (!PopulateGroupDatabaseStructureFromDialog (&sGroupDatabaseStructure)) {		
			return FALSE;
		}
		int rcode = txdsapi_insert_record ( (pBYTE)&sGroupDatabaseStructure, sizeof (UCF02), UCF02_DATA, strErrorMessage );
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

			m_StrMessageLbl = "Record Added";	
			m_CntlGroupIDFld.SetFocus();
			if (!UpdateData(FALSE)){
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}
			pAccessPage->CleanGroupAccessDialog();
			pAccessPage->EnableGroupAccessDialog(FALSE);
			if ( pWorkstationPage->m_bWorkstation)
			{
				pWorkstationPage->CleanGroupWorkstationDialog ();
				pWorkstationPage->EnableGroupWorkstationDialog(FALSE);
			}

			if ( pChangeHistoryPage->m_bChangeHistory )
				pChangeHistoryPage->CleanChangeHistory();
			m_CntlGroupIDFld.EnableWindow(TRUE);
			SetModified ( FALSE );
			m_nDialogMode = MODE_DONE;
			((CPropertySheet*) this->GetParent())->SetActivePage(0);
			((CPropertySheet*) this->GetParent())->SetWindowText("Group Definition");
			//set version info in the title bar
			CString appProdVer;
			CString lpszTitle;
			appProdVer.LoadString(IDS_PRODUCT_VERSION);
			((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
			lpszTitle = lpszTitle + " - Version - ";
			lpszTitle += appProdVer;
			((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);
			return CPropertyPage::OnApply();
		} 
	}
	m_nDialogMode = MODE_DONE;
	return CPropertyPage::OnApply();
}

BOOL CGroupDefinition::PopulateGroupDialogFromDatabase ( UCF02 pGroupRecord)
{
	
    pAccessPage->m_StrGroupIDFld = (LPSTR)pGroupRecord.primary_key.group_id ;
   
    memcpy ( &pAccessPage->m_sControl,(LPSTR)pGroupRecord.control, sizeof(pAccessPage->m_sControl));
       

    //****************** Workstation Page *********************

 //   pWorkstationPage->m_StrUserNameFld = (LPSTR)pGroupRecord.primary_key.name ;
  //  pWorkstationPage->m_StrGroupIDFld = (LPSTR)pGroupRecord.group_id ;
    memcpy ( &pWorkstationPage->m_sLogin,(LPSTR)pGroupRecord.login, sizeof(pWorkstationPage->m_sLogin));   
	
	if ( pAccessPage->m_bAccess )
		pAccessPage->PopulateDialogFromDatabase();

	if ( pWorkstationPage->m_bWorkstation )
		pWorkstationPage->PopulateWorkstationDialog();

    return TRUE;
}

BOOL CGroupDefinition::PopulateGroupDatabaseStructureFromDialog ( pUCF02 pGroupRecord ) 
{
 	CMyUtils myUtilsObject;
   char strDate[9] = ""; 
   CString date;
   int len_GroupID = 0;
 
   myUtilsObject.GetSystemDate(strDate);      

   ZeroMemory (pGroupRecord, sizeof(UCF02)) ;
	
	pAccessPage->PopulateControlStructure();
	pWorkstationPage->PopulateLoginStructure();

	if ( m_nDialogMode == MODE_EDIT )
	{
		pAccessPage->PopulateHistoryTable();
		pWorkstationPage->PopulateHistoryTable();
	}
    
    memcpy ((LPTSTR) pGroupRecord->control,				&pAccessPage->m_sControlNew, sizeof(pGroupRecord->control));   
	len_GroupID = strlen(m_StrGroupIDFld);
	if(len_GroupID>0)
	{
		memcpy ((LPTSTR) pGroupRecord->primary_key.group_id,m_StrGroupIDFld, sizeof (pGroupRecord->primary_key.group_id));
	}
    memcpy ((LPTSTR) pGroupRecord->login,				&pWorkstationPage->m_sLoginNew, sizeof(pGroupRecord->login)); 

    if ( m_nDialogMode == MODE_ADD )
    {
      date = strDate;
      date = date.Right(4) + date.Left(2) + date.Mid (2,2);
      memcpy ( (LPSTR)pGroupRecord->date_added, date, sizeof(strDate)); 
    }
    else if ( m_nDialogMode == MODE_EDIT )
       memcpy ( pGroupRecord->date_added, sOldUCF02Record.date_added, sizeof ( sOldUCF02Record.date_added ) );

	
	return TRUE;
}

void CGroupDefinition::PopulateHistoryTable()
{
    HST01 sHistoryData;
    CHAR dest_str[18];   
	 CMyUtils myUtilsObject;
	 char strErrorMsg[200] = "";
	 int len_GroupID = 0;
    CString temp;

    char strDate[9] = "";
    char strTime[7] = "";

    memset ( strDate, 0, sizeof (strDate));
    memset ( strTime, 0, sizeof (strTime));
    
    myUtilsObject.GetSystemDate(strDate);  

    myUtilsObject.GetSystemTime(strTime);
    pintime_get_gmt ( dest_str );
    ZeroMemory ( &sHistoryData, sizeof (HST01));
    memcpy ((LPTSTR) sHistoryData.file_name,     (LPCTSTR)"UCF02", sizeof (sHistoryData.file_name));
    temp = strDate;
    temp = temp.Right(4) + temp.Left(2) + temp.Mid(2,2);
    
    CHAR wstation[21]; DWORD wsize;

    wsize = sizeof( wstation );
    GetComputerName( wstation, &wsize );

	memcpy ((LPTSTR)sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
    memcpy ((LPTSTR) sHistoryData.date_modified, temp, sizeof (sHistoryData.date_modified));
	len_GroupID = strlen(m_StrGroupIDFld);
	if(len_GroupID>0)
	{
		memcpy ((LPTSTR) sHistoryData.file_primary_key,  (LPCTSTR)m_StrGroupIDFld, sizeof (sHistoryData.file_primary_key));
	}
    memcpy ((LPTSTR) sHistoryData.primary_key.unique_id,     (LPCTSTR)dest_str, sizeof (sHistoryData.primary_key.unique_id));        
    memcpy ((LPTSTR) sHistoryData.time_modified,     (LPCTSTR)strTime, sizeof (sHistoryData.time_modified)); 
	 memcpy ((LPTSTR)sHistoryData.changed_by,(LPTSTR)ucf01.primary_key.name, sizeof(sHistoryData.changed_by));
    memcpy ((LPTSTR)sHistoryData.current_data, (LPCTSTR)m_strCurrentData, sizeof (sHistoryData.current_data));
        
    int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
	 if ( rcode != PTEMSG_OK )
	 {
		 m_StrMessageLbl = strErrorMsg;
		 m_nDialogMode = MODE_DONE;
		 UpdateData( FALSE );
	 }
    
}


BOOL CGroupDefinition::OnHelpInfo(HELPINFO* pHelpInfo)  
{
/*
	HtmlHelp (AfxGetMainWnd()->GetSafeHwnd(),
	"security.chm>Main",HH_HELP_CONTEXT,IDD_GROUPS_ACCESS);
*/	
	return TRUE;
}

BOOL CGroupDefinition::OnSetActive() 
{
	((CGroupPropertySheet*) this->GetParent())->m_nActiveTab = 0;
	
	return CPropertyPage::OnSetActive();
}

BOOL CGroupDefinition::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}
