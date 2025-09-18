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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\UserDefinition.cpp  $

      Rev 1.14   Jan 01 2011 11:30:22   TF-Srikanth
   Added PCI changes Unlocking User account through GUI
   
      Rev 1.13   Dec 01 2003 11:30:22   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.12   Jul 10 2002 09:56:56   lmitchel
   Ver:4.0.3.1:  Added SendMessage command in PropertySheet files (on_init function) to set the Apply button as the default.  If the enter key is pressed, action will default to  the apply button instead of the OK button.  This allows the user to remain in the current dialog instead of exiting to the main menu.
   
      Rev 1.11   Jan 27 2000 13:06:50   iarustam
   SCR # 314,328,330,300,159
   
      Rev 1.10   Jan 25 2000 14:01:02   iarustam
   bug fixes
   
      Rev 1.9   Jan 05 2000 14:54:32   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.2   Oct 15 1999 17:22:46   iarustam
   bug fixes
   
      Rev 1.1   Oct 06 1999 16:46:52   iarustam
   Bug fixes # 65,118,58,145,117,116,96,189,97,
   120,77,50,177,95,94
   
      Rev 1.0   Jun 28 1999 13:59:36   iarustam
   Initial Release
 
    
*
************************************************************************************/
// UserDefinition.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "UserAccountStatus.h"
#include "UserDefinition.h"
#include "UserAccess.h"
#include "UserWorkstation.h"
#include "ChangeHistory.h"
#include "MyUtils.h"
#include "UserPropertySheet.h"
#include "NameList.h"
#include "common.h"
#include "ptepci.h"
#include "SharedMem.h"

extern "C"
{
#include "PteTime.h"
#include "Txutils.h"
}

//#include "htmlhelp.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CUserAccess* pAccessPage;
CUserWorkstation* pWorkstationPage;
CUserAccountStatus* pAccountStatusPage; 

extern CString name;
extern CString UCF01_CUserStatus;
extern UCF01 ucf01;
extern CSharedMem shrmem;
extern GPCF01 gpcf01;
//extern UCF03 ucf03;

/////////////////////////////////////////////////////////////////////////////
// CUserDefinition property page

IMPLEMENT_DYNCREATE(CUserDefinition, CPropertyPage)

CUserDefinition::CUserDefinition() : CPropertyPage(CUserDefinition::IDD)
{
	//{{AFX_DATA_INIT(CUserDefinition)
	m_StrGroupIDFld = _T("");
	m_StrUserNameFld = _T("");
	m_StrUserPasswordFld = _T("");
	m_StrMessageLbl = _T("");
	m_StrUserPassword2Fld = _T("");
	m_StrUserNameFld2 = _T("");
	//}}AFX_DATA_INIT
}

CUserDefinition::~CUserDefinition()
{
}

void CUserDefinition::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUserDefinition)
	DDX_Control(pDX, IDC_GROUP_LIST, m_CntlListGroupsBtn);
	DDX_Control(pDX, IDC_USER_NAME_FLD2, m_CntlUserNameFld2);
	DDX_Control(pDX, IDC_USER_ADD_BUTTON, m_AddButton);
	DDX_Control(pDX, IDC_USER_VIEW_BUTTON, m_ViewButton);
	DDX_Control(pDX, IDC_USER_UPDATE_BUTTON, m_UpdateButton);
	DDX_Control(pDX, IDC_USER_LIST_BUTTON, m_ListButton);
	DDX_Control(pDX, IDC_USER_DELETE_BUTTON, m_DeleteButton);
	DDX_Control(pDX, IDC_PASSWORD2_FLD, m_CntlUserPassword2Fld);
	DDX_Control(pDX, IDC_USER_PASSWORD_FLD, m_CntlUserPasswordFld);
	DDX_Control(pDX, IDC_USER_NAME_FLD, m_CntlUserNameFld);
	DDX_Control(pDX, IDC_GROUP_ID_FLD, m_CntlGroupIDFld);
	DDX_Text(pDX, IDC_GROUP_ID_FLD, m_StrGroupIDFld);
	DDV_MaxChars(pDX, m_StrGroupIDFld, 15);
	DDX_Text(pDX, IDC_USER_NAME_FLD, m_StrUserNameFld);
	DDV_MaxChars(pDX, m_StrUserNameFld, MAX_USERNAME_LEN);
	DDX_Text(pDX, IDC_USER_PASSWORD_FLD, m_StrUserPasswordFld);
	DDV_MaxChars(pDX, m_StrUserPasswordFld, MAX_USERPWD_LEN);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	DDX_Text(pDX, IDC_PASSWORD2_FLD, m_StrUserPassword2Fld);
	DDV_MaxChars(pDX, m_StrUserPassword2Fld, MAX_USERPWD_LEN);
	DDX_Text(pDX, IDC_USER_NAME_FLD2, m_StrUserNameFld2);
	DDV_MaxChars(pDX, m_StrUserNameFld2, 20);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUserDefinition, CPropertyPage)
	//{{AFX_MSG_MAP(CUserDefinition)
	ON_BN_CLICKED(IDC_USER_ADD_BUTTON, OnUserAddButton)
	ON_BN_CLICKED(IDC_USER_DELETE_BUTTON, OnUserDeleteButton)
	ON_BN_CLICKED(IDC_USER_LIST_BUTTON, OnUserListButton)
	ON_BN_CLICKED(IDC_USER_UPDATE_BUTTON, OnUserUpdateButton)
	ON_BN_CLICKED(IDC_USER_VIEW_BUTTON, OnUserViewButton)
	ON_WM_HELPINFO()
	ON_BN_CLICKED(IDC_USER_ACESS_BUTTON, OnUserAcessButton)
	ON_BN_CLICKED(IDC_GROUP_LIST, OnGroupList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUserDefinition message handlers

BOOL CUserDefinition::OnInitDialog() 
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

	m_StrUserNameFld = "";
	m_StrUserNameFld2 = "";
	m_StrUserPasswordFld = "";
	m_StrUserPassword2Fld = "";
	m_StrGroupIDFld = "";
	m_StrMessageLbl = "";
	CString temp;
	int nIndex = -1;

	for ( int i = 0; i <=15; i++ )
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


//	pWorkstationPage->EnableWindow(FALSE);
//	pAccessPage->EnableWindow(TRUE);
	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CUserDefinition::OnUserAddButton() 
{
	UCF01 sUserRecord;
    UCF02 sGroupRecord;
	//UCF01 snewUserRecord;
	char strErrorMessage[200];
	char toDayDate[10];

    CMyUtils myUtilsObject;

	m_StrMessageLbl = "";

	m_nDialogMode = MODE_ADD;
	m_nPrevMode = MODE_ADD;	


	SetExpiryDate(toDayDate);
	CString tep_exp_date =toDayDate;
	int year1,mon1,day1;
	
	if(!tep_exp_date.IsEmpty())
	{
		year1 = atoi(tep_exp_date.Mid(0,4));
		mon1 = atoi(tep_exp_date.Mid(4,2));
		day1 = atoi(tep_exp_date.Mid(6,2));
		CTime m_StrUserExpiryDate_temp(year1,mon1,day1,0,0,0);
		pAccountStatusPage->m_StrUserExpiryDate = m_StrUserExpiryDate_temp;
	 }
	//pAccountStatusPage->m_StrUserExpiryDate = 0;
	 if (!CheckDialogForCompleteness())
        return;

	 
	pAccessPage->m_nDialogMode = MODE_ADD;  
	pWorkstationPage->m_nDialogMode = MODE_ADD;
	pAccountStatusPage->m_nDialogMode = MODE_ADD;
    pChangeHistoryPage->m_nDialogMode = MODE_ADD; 
	pWorkstationPage->m_bGroup = FALSE;
	pWorkstationPage->m_bGroup = FALSE;
	pAccountStatusPage->m_nDialogMode=MODE_ADD;
	pAccountStatusPage->m_viewPopulate=FALSE;
	pAccountStatusPage->m_bPopulate=TRUE;

	UpdateData(FALSE);

	//pAccessPage->CheckCountofUsersassignedforDualEncryption(m_StrUserNameFld, 0);

	ZeroMemory(&sUserRecord,sizeof(UCF01)) ;

    memcpy ((LPSTR)(sUserRecord.primary_key.name), m_StrUserNameFld , sizeof (sUserRecord.primary_key.name));
    //memcpy ((LPSTR)(snewUserRecord.primary_key.name), m_StrUserNameFld , sizeof (snewUserRecord.primary_key.name));
  

	int rcode = txdsapi_get_record  ((pBYTE)&sUserRecord, sizeof (UCF01), UCF01_DATA, strErrorMessage);
	if( rcode == PTEMSG_OK )
    {
	   m_nDialogMode = MODE_DONE;
      m_StrMessageLbl = "ERROR: User Name already exists";

		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return ;
    }

	


    if ( !myUtilsObject.IsEmpty (m_StrGroupIDFld, this ))
    {
        ZeroMemory ( &sGroupRecord, sizeof (UCF02));
        memcpy ((LPSTR)(sGroupRecord.primary_key.group_id), m_StrGroupIDFld , sizeof (sGroupRecord.primary_key.group_id));
		int rcode = txdsapi_get_record  ((pBYTE)&sGroupRecord,sizeof (UCF02), UCF02_DATA, strErrorMessage);
		if ( rcode == PTEMSG_NOT_FOUND )
		{	       
			m_StrMessageLbl = "ERROR: Invalid Group ID";
         m_nDialogMode = MODE_DONE;

		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
            m_CntlGroupIDFld.SetFocus();
            pAccessPage->m_bGroup = FALSE;
            pWorkstationPage->m_bGroup = FALSE;
		    return ;
        }
      else if ( rcode != PTEMSG_OK )
		{	       
			m_StrMessageLbl = strErrorMessage;
         m_nDialogMode = MODE_DONE;

		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
            m_CntlGroupIDFld.SetFocus();
            pAccessPage->m_bGroup = FALSE;
            pWorkstationPage->m_bGroup = FALSE;
		    return ;
        }
        else 
        {
			((CPropertySheet*) this->GetParent())->SetActivePage(2);
			((CPropertySheet*) this->GetParent())->SetActivePage(1);			
            pAccessPage->m_bGroup = TRUE;
            pWorkstationPage->m_bGroup = TRUE;
            PopulateUserDialogFromGroupDatabase ( sGroupRecord );
			pAccessPage->PopulateDialogFromDatabase ();
			pWorkstationPage->PopulateWorkstationDialog();
			// useraccstutus->PopulateAccountstatus();
			
        }
            
    }
    else
    {
		((CPropertySheet*) this->GetParent())->SetActivePage(2);
		((CPropertySheet*) this->GetParent())->SetActivePage(1);
		((CPropertySheet*) this->GetParent())->SetActivePage(0);
		((CPropertySheet*) this->GetParent())->SetActivePage(1);
        pAccessPage->m_bGroup = FALSE;
		pAccessPage->EnableUserAccessDialog(TRUE);
		pAccessPage->CleanUserAccessDialog();

        pWorkstationPage->m_bGroup = FALSE;
		pWorkstationPage->EnableUserWorkstationDialog(TRUE);
		pWorkstationPage->CleanUserWorkstationDialog ();
		if ( pChangeHistoryPage->m_bChangeHistory )
			pChangeHistoryPage->CleanChangeHistory();

    }
	
		
	pAccessPage->m_StrUserNameFld = m_StrUserNameFld;
    pAccessPage->m_StrGroupIDFld = m_StrGroupIDFld;
    pAccessPage->m_StrUserNameFld2 = m_StrUserNameFld2;
    pWorkstationPage->m_StrUserNameFld = m_StrUserNameFld;
    pWorkstationPage->m_StrGroupIDFld = m_StrGroupIDFld;
	pAccountStatusPage->m_StrUserNameFld = m_StrUserNameFld;
	pChangeHistoryPage->bNext = FALSE;
	((CPropertySheet*) this->GetParent())->SetWindowText("Add User");
	//set version info in the title bar
	CString appProdVer;
	CString lpszTitle;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
	lpszTitle = lpszTitle + " - Version - ";
	lpszTitle += appProdVer;
	((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);

	
 
	m_CntlUserNameFld.EnableWindow(FALSE);
	pAccountStatusPage->m_bAccStatus ="New User Account";
	pAccountStatusPage->m_addPopulate=0;
	pAccessPage->UpdateData(FALSE);
	pWorkstationPage->UpdateData(FALSE);



//	pAccessPage->SetModified ( FALSE );
//	pWorkstationPage->SetModified ( FALSE );  

}

BOOL CUserDefinition::PopulateUserDialogFromGroupDatabase ( UCF02 sGroupRecord )
{

    pAccessPage->m_StrGroupIDFld = (LPSTR)sGroupRecord.primary_key.group_id ;
    memcpy ( &pAccessPage->m_sControl,(LPSTR)sGroupRecord.control, sizeof(m_sControl1));

    pWorkstationPage->m_StrGroupIDFld = (LPSTR)sGroupRecord.primary_key.group_id ;
    memcpy ( &pWorkstationPage->m_sLogin,(LPSTR)sGroupRecord.login, sizeof(m_sLogin1));
	
	pAccessPage->m_bPopulate = TRUE;

    return TRUE;
}

void CUserDefinition::OnUserDeleteButton() 
{
	CString strMessage;
    int nReturnCode;
    UCF01 sUserRecord;
	//UCF01 snewUserRecord;
	char strErrorMessage[200]={0};
     m_nDialogMode = MODE_EDIT;
	 m_nPrevMode = MODE_EDIT;
	 int rcode=0;

	 m_StrMessageLbl = "";

    if (!CheckDialogForCompleteness())
        return;   

   /* m_pUserSheet->m_AccessPage.m_psp.dwFlags &= ~PSP_HASHELP; 
    m_pUserSheet->m_WorkstationPage.m_psp.dwFlags &= ~PSP_HASHELP; 
    m_pUserSheet->m_ChangeHistoryPage.m_psp.dwFlags &= ~PSP_HASHELP;
   
    m_pUserSheet->m_psh.dwFlags &= ~PSH_HASHELP;*/

    if (!UpdateData(TRUE))
	{
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}
    
	strMessage.Format ("Are you sure you want to delete %s ?", (LPCTSTR) m_StrUserNameFld) ;

	nReturnCode = AfxMessageBox ((LPCTSTR)strMessage, MB_OKCANCEL|MB_ICONQUESTION) ;
	if (nReturnCode != IDOK) return ;

	ZeroMemory(&sUserRecord,sizeof(UCF01)) ;

	memcpy ((LPSTR)(sUserRecord.primary_key.name), m_StrUserNameFld , sizeof (sUserRecord.primary_key.name));
	 rcode = txdsapi_get_record  ((pBYTE)&sUserRecord, sizeof (UCF01), UCF01_DATA, strErrorMessage);
	 if( rcode == PTEMSG_OK )
	 {
		if(strcmp((LPTSTR)(sUserRecord.User_Status),"DELETED")!=0)
		{
			memcpy((LPTSTR)(sUserRecord.User_Status),STATUS_DELETED,sizeof (sUserRecord.User_Status));
			rcode = txdsapi_update_record  ((pBYTE)&sUserRecord, sizeof (UCF01), UCF01_DATA,strErrorMessage);
		}
	 }

	//memcpy ((LPTSTR)snewUserRecord.primary_key.name, m_StrUserNameFld, sizeof (snewUserRecord.primary_key.name)) ;
	//int nrcode = txdsapi_delete_record((pBYTE)&snewUserRecord, sizeof (UCF01), UCF01_DATA,strErrorMessage);
	
	//if ( rcode != PTEMSG_OK &&  nrcode != PTEMSG_OK )
	if (rcode != PTEMSG_OK)
	{	    
      switch( rcode )
      {
         case PTEMSG_NOT_FOUND:
            sprintf( strErrorMessage, "ERROR: User record for %s does not exist", m_StrUserNameFld );
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
		/**adding proper data into the HST01 table, so it reflects proper output in the audit log file, -TF_Veena**/
			
				memset ( m_strCurrentData, 0, sizeof (m_strCurrentData));
				memcpy ((LPSTR)m_strCurrentData, "Record Deleted", sizeof (m_strCurrentData));
				//memcpy((LPSTR)m_strCurrentData,m_StrUserNameFld,sizeof (m_strCurrentData));
				
		/************************************************************************************************************/	
	    PopulateHistoryTable();
        m_StrGroupIDFld = _T("");
		  m_StrUserNameFld = _T("");
		  m_StrUserPasswordFld = _T("");
		  m_StrUserPassword2Fld = _T("");
        m_StrMessageLbl = "Record Deleted";
		
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		if ( pAccessPage->m_bAccess )
		{
			pAccessPage->CleanUserAccessDialog();
			pAccessPage->EnableUserAccessDialog(FALSE);
		}

		if ( pWorkstationPage->m_bWorkstation )
		{
			pWorkstationPage->CleanUserWorkstationDialog ();
			pWorkstationPage->EnableUserWorkstationDialog(FALSE);
		}

		if ( pChangeHistoryPage->m_bChangeHistory )
			pChangeHistoryPage->CleanChangeHistory();
		((CPropertySheet*) this->GetParent())->SetActivePage(1);
		((CPropertySheet*) this->GetParent())->SetActivePage(3);
		((CPropertySheet*) this->GetParent())->SetActivePage(0);
		m_nDialogMode = MODE_DONE;
		m_CntlUserNameFld.SetFocus();
		m_CntlUserNameFld.EnableWindow(TRUE);
		m_CntlUserPasswordFld.EnableWindow(TRUE);
		m_CntlUserPassword2Fld.EnableWindow(TRUE);
		m_CntlGroupIDFld.EnableWindow(TRUE);
		m_CntlListGroupsBtn.EnableWindow(TRUE);
		pAccountStatusPage->m_StrUserNameFld="";
		pAccountStatusPage->m_StrLabel1="";
		pAccountStatusPage->EnableUserAccountStatusDialog(FALSE);

	//	((CPropertySheet*) this->GetParent())->SetActivePage(0);
		pAccessPage->SetModified ( FALSE );
		pWorkstationPage->SetModified ( FALSE );
    }
	
}

void CUserDefinition::OnUserListButton() 
{
	CNameList listDlg;
    int nReturnCode;

	m_StrMessageLbl = "";

	UpdateData(TRUE);

    listDlg.m_nIndicator = MODE_USER;
    listDlg.strPrimaryKey1 = m_StrUserNameFld;   

    nReturnCode = listDlg.DoModal();
    if ( nReturnCode == IDOK)
	{
        m_StrUserNameFld = name;
		m_CntlUserNameFld.EnableWindow(TRUE);
		m_StrGroupIDFld = _T("");
		m_StrUserPasswordFld = _T("");
		m_StrMessageLbl = _T("");
		m_StrUserPassword2Fld = _T("");
	}
        
    if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}		
	
}

/*Krishna@TF, 26.5.11 : All referencesof Ucf03 are removed as contents of Ucf03 table
are updated in Ucf01 itself. Any changes to Ucf03 contents are updated in Ucf01. */
void CUserDefinition::OnUserUpdateButton() 
{	
    UCF01 sUserRecord;
	//UCF01 snewUserRecord;

    HST01 sHistoryRecord;
	char strErrorMessage[200];

    m_nDialogMode = MODE_EDIT;
	m_nPrevMode = MODE_EDIT;


	m_StrMessageLbl = "";

	 m_StrMessageLbl = "";
    char toDayDate[10];
	SetExpiryDate(toDayDate);
	CString tep_exp_date =toDayDate;
	int year1,mon1,day1;
	
	if(!tep_exp_date.IsEmpty())
	{
		year1 = atoi(tep_exp_date.Mid(0,4));
		mon1 = atoi(tep_exp_date.Mid(4,2));
		day1 = atoi(tep_exp_date.Mid(6,2));
		CTime m_StrUserExpiryDate_temp(year1,mon1,day1,0,0,0);
		pAccountStatusPage->m_StrUserExpiryDate = 0;
		//pAccountStatusPage->m_StrUserExpiryDate = m_StrUserExpiryDate_temp;
	 }
    if (!CheckDialogForCompleteness())
        return;

    /*m_pUserSheet->m_AccessPage.m_psp.dwFlags &= ~PSP_HASHELP; 
    m_pUserSheet->m_WorkstationPage.m_psp.dwFlags &= ~PSP_HASHELP; 
    m_pUserSheet->m_ChangeHistoryPage.m_psp.dwFlags &= ~PSP_HASHELP;

    m_pUserSheet->m_psh.dwFlags |= PSH_NOAPPLYNOW ; 
    m_pUserSheet->m_psh.dwFlags &= ~PSH_HASHELP;*/

	//memcpy ((LPSTR)(snewUserRecord.primary_key.name), m_StrUserNameFld, sizeof (snewUserRecord.primary_key.name) );
	//int rcode1 = txdsapi_get_record  ((pBYTE)&snewUserRecord, sizeof (UCF01), UCF01_DATA, strErrorMessage);
	
	/*Added ucf01 table for unlocking user account from GUI-TF-Srikanth*/
    memcpy ((LPSTR)(sUserRecord.primary_key.name), m_StrUserNameFld, sizeof (sUserRecord.primary_key.name) );	
	
	

	int rcode = txdsapi_get_record  ((pBYTE)&sUserRecord, sizeof (UCF01), UCF01_DATA, strErrorMessage);
	if( rcode == PTEMSG_OK )
	 {
		if(strcmp((LPTSTR)(sUserRecord.User_Status),"DELETED")==0)
		{				
			AfxMessageBox( "Deleted User records can not be updated.", MB_ICONSTOP | MB_OK );
			return;		
		}
	 }
	
	//ramya
	pAccountStatusPage->prev_UserStatus = (LPSTR)sUserRecord.User_Status;
	pAccountStatusPage->prev_ExpDate    = (LPSTR)sUserRecord.User_Expiry_Date;

	//if ( pAccountStatusPage->prev_UserStatus.CompareNoCase( "ACTIVE")==0)
	//	pAccountStatusPage->prev_UserStatus = STATUS_ACTIVE; 

	//else if (pAccountStatusPage->m_StrUserStatus.CompareNoCase("INACTIVE")==0)
 //      pAccountStatusPage->prev_UserStatus = STATUS_INACTIVE;

	//else  if (pAccountStatusPage->prev_UserStatus.CompareNoCase("EXPIRED")==0)
	//	pAccountStatusPage->prev_UserStatus = STATUS_EXPIRED;

		
	if ( rcode != PTEMSG_OK )
	{	    
      switch( rcode )
      {
         case PTEMSG_NOT_FOUND:
            sprintf( strErrorMessage, "ERROR: User record for %s does not exist", m_StrUserNameFld );
            break;
      }

      m_StrMessageLbl = strErrorMessage;
		m_nDialogMode = MODE_DONE;
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
      
      return;
	}

    memcpy ( &sOldUCF01Record, &sUserRecord, sizeof ( UCF01 ) );

	memcpy (m_strGroup, (LPSTR)(sUserRecord.group_id),  sizeof (m_strGroup) );
    memcpy (m_strPassword, (LPSTR)(sUserRecord.password),  sizeof (m_strPassword) );
    	

    ZeroMemory (&sHistoryRecord, sizeof (sHistoryRecord));

    memcpy ((LPSTR)(sHistoryRecord.primary_key.unique_id), "                 ", sizeof (sHistoryRecord.primary_key.unique_id));
    memcpy ((LPSTR)(sHistoryRecord.file_name), "UCF01", sizeof (sHistoryRecord.file_name));
    memcpy ((LPSTR)(sHistoryRecord.file_primary_key), (LPSTR)sUserRecord.primary_key.name, sizeof (sHistoryRecord.file_primary_key));
	memcpy ((LPSTR)(sHistoryRecord.changed_by), "USERDEFN", 8);

    memcpy ( &(pChangeHistoryPage->sHistoryRecord), &sHistoryRecord, sizeof(sHistoryRecord));
    
    m_StrGroupIDFld = sUserRecord.group_id; 
    UpdateData( FALSE );
	
	//((CPropertySheet*) this->GetParent())->SetActivePage(3);
	//((CPropertySheet*) this->GetParent())->SetActivePage(2);
	((CPropertySheet*) this->GetParent())->SetActivePage(1);  
    

    pAccessPage->m_nDialogMode = MODE_EDIT;
    pWorkstationPage->m_nDialogMode = MODE_EDIT;
	pAccountStatusPage->m_nDialogMode = MODE_EDIT;
    pChangeHistoryPage->m_nDialogMode = MODE_EDIT;
	pChangeHistoryPage->bNext = FALSE;	
	pAccountStatusPage->m_viewPopulate=FALSE;
	pAccountStatusPage->m_bPopulate= TRUE;
	UpdateData( FALSE );

	((CPropertySheet*) this->GetParent())->SetWindowText("Update User");
	//set version info in the title bar
	CString appProdVer;
	CString lpszTitle;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
	lpszTitle = lpszTitle + " - Version - ";
	lpszTitle += appProdVer;
	((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);

	if ( pAccessPage->m_bAccess )
		pAccessPage->CleanUserAccessDialog();
	
	if ( pWorkstationPage->m_bWorkstation )
		pWorkstationPage->CleanUserWorkstationDialog();

	if ( pAccountStatusPage->m_bStatus )
		pAccountStatusPage->CleanUserAccountStatusDialog();
	
	if ( pChangeHistoryPage->m_bChangeHistory )
		pChangeHistoryPage->PopulateHistoryDialog();
	
	//sending ucf01,ucf01 structure to enable user dialog
	PopulateUserDialogFromDatabase( sUserRecord,sUserRecord );
	m_CntlUserNameFld.EnableWindow(FALSE);
    m_CntlGroupIDFld.EnableWindow( FALSE );
	m_CntlListGroupsBtn.EnableWindow( FALSE );
}

void CUserDefinition::OnUserViewButton() 
{
    UCF01 sUserRecord;
	//UCF01 snewUserRecord;
    HST01 sHistoryRecord;
	char strErrorMessage[200];

    m_nDialogMode = MODE_VIEW;
	m_nPrevMode = MODE_VIEW;

	pAccessPage->m_nDialogMode = MODE_VIEW;
    pWorkstationPage->m_nDialogMode = MODE_VIEW;
    pChangeHistoryPage->m_nDialogMode = MODE_VIEW;
	pChangeHistoryPage->bNext = FALSE;
	pAccountStatusPage->m_nDialogMode = MODE_VIEW;
	pAccountStatusPage->m_viewPopulate=TRUE;
	UpdateData(TRUE);
	char toDayDate[10];
	SetExpiryDate(toDayDate);
	CString tep_exp_date =toDayDate;
	int year1,mon1,day1;
	
	if(!tep_exp_date.IsEmpty())
	{
		year1 = atoi(tep_exp_date.Mid(0,4));
		mon1 = atoi(tep_exp_date.Mid(4,2));
		day1 = atoi(tep_exp_date.Mid(6,2));
		CTime m_StrUserExpiryDate_temp(year1,mon1,day1,0,0,0);
		pAccountStatusPage->m_StrUserExpiryDate = m_StrUserExpiryDate_temp;
		UpdateData(FALSE);
	}

    if (!CheckDialogForCompleteness())
        return;	  

    /*m_pUserSheet->m_AccessPage.m_psp.dwFlags &= ~PSP_HASHELP; 
    m_pUserSheet->m_WorkstationPage.m_psp.dwFlags &= ~PSP_HASHELP; 
    m_pUserSheet->m_ChangeHistoryPage.m_psp.dwFlags &= ~PSP_HASHELP;

    m_pUserSheet->m_psh.dwFlags |= PSH_NOAPPLYNOW ; 
    m_pUserSheet->m_psh.dwFlags &= ~PSH_HASHELP;*/


    memcpy ((LPSTR)(sUserRecord.primary_key.name), m_StrUserNameFld, sizeof (sUserRecord.primary_key.name) );
//	memcpy ((LPSTR)(snewUserRecord.primary_key.name), m_StrUserNameFld, sizeof (snewUserRecord.primary_key.name) );

   // lstrcpy ((LPSTR)(sUserRecord.group_id), m_StrGroupIDFld );
    //lstrcpy ((LPSTR)(sUserRecord.password), m_StrUserPasswordFld );
	//int rcode1 = txdsapi_get_record  ((pBYTE)&snewUserRecord, sizeof (UCF01), UCF01_DATA, strErrorMessage);
	
	int rcode = txdsapi_get_record  ((pBYTE)&sUserRecord, sizeof (UCF01), UCF01_DATA, strErrorMessage);
	if ( rcode != PTEMSG_OK )
	{	    
      switch( rcode )
      {
         case PTEMSG_NOT_FOUND:
            sprintf( strErrorMessage, "ERROR: User record for %s does not exist", m_StrUserNameFld );
            break;
      }

      m_StrMessageLbl = strErrorMessage;
		m_nDialogMode = MODE_DONE;
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
      
      return;
	}

    ZeroMemory (&sHistoryRecord, sizeof (sHistoryRecord));


    memcpy ((LPSTR)(sHistoryRecord.primary_key.unique_id), "                 ", sizeof (sHistoryRecord.primary_key.unique_id));
    memcpy ((LPSTR)(sHistoryRecord.file_name), "UCF01", sizeof (sHistoryRecord.file_name));
    memcpy ((LPSTR)(sHistoryRecord.file_primary_key), (LPSTR)sUserRecord.primary_key.name, sizeof (sHistoryRecord.file_primary_key));
	memcpy ((LPSTR)(sHistoryRecord.changed_by), "USERDEFN", 8);

    memcpy ( &(pChangeHistoryPage->sHistoryRecord), &sHistoryRecord, sizeof(sHistoryRecord));
    
    m_StrGroupIDFld = sUserRecord.group_id; 
	m_StrUserNameFld2 = sUserRecord.user_info_1;
	
	//m_StrUserNameFld2 += sUserRecord.user_info_2;
	m_StrUserNameFld2 += (CString)((char *)sUserRecord.user_info_2);
   UpdateData( FALSE );
	
	((CPropertySheet*) this->GetParent())->SetActivePage(1);
//	((CPropertySheet*) this->GetParent())->SetActivePage(2);
	//((CPropertySheet*) this->GetParent())->SetActivePage(1);  

	//pAccountStatusPage->m_viewPopulate=TRUE;
		pChangeHistoryPage->bNext = FALSE;
	((CPropertySheet*) this->GetParent())->SetWindowText("View User");
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

	if ( pAccessPage->m_bAccess )
		pAccessPage->CleanUserAccessDialog();
	if ( pWorkstationPage->m_bWorkstation )
		pWorkstationPage->CleanUserWorkstationDialog();
	//if(pAccountStatusPage->m_bUAccStatus)
	//	pAccountStatusPage->PopulateDialogFromDatabase(sUserRecord);
	
	PopulateUserDialogFromDatabase( sUserRecord,sUserRecord );//sending ucf01,ucf01 structure to enable user dialog
	

	m_CntlUserNameFld.EnableWindow(TRUE);
	m_CntlUserPasswordFld.EnableWindow(TRUE);
	m_CntlUserPassword2Fld.EnableWindow(TRUE);
	m_CntlGroupIDFld.EnableWindow(TRUE);
	m_CntlListGroupsBtn.EnableWindow(TRUE);
	

	
}

BOOL CUserDefinition::CheckDialogForCompleteness()
{
    CMyUtils myUtilsObject;
	char temp_passwd[50]={'\0'};
	char tmpbuff[256]={'\0'};
	CPtepci ptepci;

	/*****TF_Veena*********/
	CHAR temp_connect_str[50] = { 0 };
	CHAR temp_name_str[50] = { 0 };
    int temp1=0;
	/**********************/
	
    if (!UpdateData(TRUE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}

	
    if (myUtilsObject.IsEmpty (m_StrUserNameFld, this))
    {
		m_StrMessageLbl = "Please enter User Name";	
		m_CntlUserNameFld.SetFocus();
		m_nDialogMode = MODE_DONE;
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return FALSE;
	}

    if (m_nDialogMode == MODE_ADD) {

		int len = m_StrUserNameFld.GetLength();
		char *str = m_StrUserNameFld.GetBuffer(len);
		for(int i=0;i<len;i++)
		{
			if((str[i] > 64 && str[i] < 91) || (str[i] > 47 && str[i] < 58) || str[i] == '_')
			{
				//The character is a ALPHABHET or NUMBER
			}
			else
			{
				m_StrMessageLbl = "No Special characters allowed in User Name";	
				m_CntlUserNameFld.SetFocus();
				m_nDialogMode = MODE_DONE;
				if (!UpdateData(FALSE)){
					AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
				}
				m_StrUserNameFld.ReleaseBuffer(len);

				return FALSE;
			}
		}
		m_StrUserNameFld.ReleaseBuffer(len);

		if (myUtilsObject.IsEmpty (m_StrUserPasswordFld, this))
        {
            m_StrMessageLbl = "Please enter User Password";	
			m_CntlUserPasswordFld.SetFocus();
			m_nDialogMode = MODE_DONE;
			if (!UpdateData(FALSE)){
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);
			}
		return FALSE;
	    }

		  if(m_StrUserNameFld.GetLength() < MIN_USERNAME_LEN)
		  {
			  sprintf(tmpbuff, "User Name length is less than %d chars", MIN_USERNAME_LEN);
			  AfxMessageBox(tmpbuff);
			  return FALSE;
		  }
		//  if(m_StrUserPasswordFld.GetLength() < MIN_USERPWD_LEN || m_StrUserPassword2Fld.GetLength()< MIN_USERPWD_LEN)
		  
		  if(m_StrUserPasswordFld.GetLength() < atoi((char *)gpcf01.min_password_length) || m_StrUserPassword2Fld.GetLength()< atoi((char *)gpcf01.min_password_length))
		  {
			 // sprintf(tmpbuff, "User Password length is less than %d chars", MIN_USERPWD_LEN);
			  sprintf(tmpbuff, "User Password length is less than %s chars", gpcf01.min_password_length);
			  AfxMessageBox(tmpbuff);
			  return FALSE;
		  }

		  strcpy(temp_passwd, m_StrUserPasswordFld);
		  if(!ptepci.isalphabet(temp_passwd))
		  {
			  AfxMessageBox("User Password does not contain any Alphabets");
			  return FALSE;
		  }
		  if(!ptepci.isnumeric(temp_passwd))
		  {
			  AfxMessageBox("User Password does not contain any Numbers");
			  return FALSE;
		  }
		  
		  strcpy(temp_passwd, m_StrUserPassword2Fld);
		  if(!ptepci.isalphabet(temp_passwd))
		  {
			  AfxMessageBox("User Password does not contain any Alphabets");
			  return FALSE;
		  }
		  if(!ptepci.isnumeric(temp_passwd))
		  {
			  AfxMessageBox("User Password does not contain any Numbers");
			  return FALSE;
		  }

        

		if ( strcmp ( m_StrUserPasswordFld, m_StrUserPassword2Fld) != 0 )
		{
            m_StrMessageLbl = "ERROR: User Password not confirmed";	
			m_CntlUserPasswordFld.SetFocus();
			m_nDialogMode = MODE_DONE;
			if (!UpdateData(FALSE)){
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}
		return FALSE;
	    }
	


	/***********************************User ID cannot be nominated for password. *************/
		 temp_connect_str[strlen(temp_connect_str)] = 0; //Make sure string is terminated
		 temp1 = 0;

		 strcpy(temp_connect_str,m_StrUserPasswordFld); //copying temp password into the temp var which will later convert to upper case.
		 strcpy(temp_name_str,m_StrUserNameFld);
		 while ( 0 != temp_connect_str[temp1] )
			temp_connect_str[temp1++] = toupper(temp_connect_str[temp1]); // Convert string to upper case

		//if( strstr( (char *)ucf01.primary_key.name,temp_connect_str ) != NULL )
		 if( strstr(temp_name_str , temp_connect_str ) != NULL )
		{
			AfxMessageBox( "Password should not contain any similarity to Username", MB_ICONSTOP | MB_OK );
			return FALSE;
		}

   /***************************************************************************************/
	
	
		
	
    }

    return TRUE;
}

/*Added ucf01 table for unlocking user account from GUI-TF-Srikanth*/
/*Krishna@TF, 26.5.11 : All referencesof Ucf03 are removed as contents of Ucf03 table
are updated in Ucf01 itself. Any changes to Ucf03 contents are updated in Ucf01. */
BOOL CUserDefinition::OnApply() 
{
	
	//populate the database structures
	UCF01 sUserDatabaseStructure;
	//UCF01 snewUserDatabaseStructure;
	char tmpbuff[100] = { 0 };
	int jday1=0,jday2=0,year=0,month=0,day=0;
	char ToDayDate[10] = {0}; 
	char ExpiryDate[11]= {0};
	char strErrorMessage[200] = { 0 }, newDate[10] = { 0 };
	char error_msg[256] = {0};
	CTime theTime;		
	int toDayDate=0;		
	theTime = CTime::GetCurrentTime();
	year= 	theTime.GetYear ();
	month = theTime.GetMonth();
	day = theTime.GetDay ();
	toDayDate = theTime.GetDay ();
	sprintf(newDate,"%d%02d%02d",year,month,toDayDate);//converting newdate to yyyymmdd format			
	sprintf(ToDayDate,"%d%02d%02d",year,month,day);//converting toDayDate to yyyymmdd format
	int rcode;
	BYTE rcode1;

	rcode1 = txdsapi_get_record( (pBYTE)&gpcf01,sizeof (GPCF01), GPCF01_DATA, error_msg );


	if (m_nDialogMode == MODE_EDIT)
	{ 
		memset(&sUserDatabaseStructure, 0, sizeof(sUserDatabaseStructure)) ;
		
		if ( !pWorkstationPage->m_bWorkstation )
		((CPropertySheet*) this->GetParent())->SetActivePage(2);
		((CPropertySheet*) this->GetParent())->SetActivePage(3);
		PopulateUserDatabaseStructureFromDialog (&sUserDatabaseStructure );

		
		strncpy(ExpiryDate,(char*)sUserDatabaseStructure.User_Expiry_Date,sizeof(sUserDatabaseStructure.User_Expiry_Date));
		
		jday1=Txutils_Calculate_Julian_Date((unsigned char*)ToDayDate);
		jday2=Txutils_Calculate_Julian_Date((unsigned char*)ExpiryDate);

		if(jday2==0)
		{
			sprintf(tmpbuff, "Please define an expiry date for the user");
			AfxMessageBox(tmpbuff);
			return FALSE;
		}

		/**** Encrypt the password  - Start ****/
		{
			int len = strlen((char *)sUserDatabaseStructure.password);
			if(len <= MAX_USERPWD_LEN) 
			
			{
				int encrypt_len = gui_password_encryption((BYTE *)sUserDatabaseStructure.password,len);
			}
		}

			if(jday2-jday1 <= 0 && jday2 !=0)
		{
			sprintf(tmpbuff, "Expiry Date must be greater than Today's Date  %02d/%02d/%d",day,month,year);
			AfxMessageBox(tmpbuff);
			return FALSE;
		}

		/**** Encrypt the password  - End ****/
		
		
		//Krishna@TF, 7.6.11: This below function is comenteed to maintain uniformity
		//PopulatenewUserDatabaseStructureFromDialog (&snewUserDatabaseStructure );	
		
		//pAccessPage->PopulateHistoryTable();
			/* the below line commented because when user is updated ,his created date should not be modified*/
		//memcpy(sUserDatabaseStructure.date_added,newDate,sizeof(sUserDatabaseStructure.date_added));
		memcpy(sUserDatabaseStructure.password_modified_date,newDate,sizeof(sUserDatabaseStructure.password_modified_date));
		//ramya
		memcpy(sUserDatabaseStructure.User_Status,pAccountStatusPage->m_StrUserStatus,sizeof(pAccountStatusPage->m_StrUserStatus));

		//memcpy(&(pAccountStatusPage->m_StrUserExpiryDate),&(sUserDatabaseStructure.User_Expiry_Date),sizeof(sUserDatabaseStructure.User_Expiry_Date));
		//pAccountStatusPage->m_StrUserExpiryDate = atoi((char *)sUserDatabaseStructure.User_Expiry_Date);
		
		//strncpy((LPSTR)sUserDatabaseStructure.User_Expiry_Date,pAccountStatusPage->m_StrUserExpiryDate,sizeof(pAccountStatusPage->m_StrUserExpiryDate));

		if ( pAccountStatusPage->m_StrUserStatus.CompareNoCase( "ACTIVE")==0)
      memcpy ((LPTSTR)sUserDatabaseStructure.User_Status , STATUS_ACTIVE, STATUS_ACTIVE_LEN); 

	 else if (pAccountStatusPage->m_StrUserStatus.CompareNoCase("INACTIVE")==0)
	 {
      memcpy ((LPTSTR)sUserDatabaseStructure.User_Status , STATUS_INACTIVE, STATUS_INACTIVE_LEN); 
	  char Today[10];
	  SetExpiryDate(Today);
	  strncpy((LPTSTR)sUserDatabaseStructure.Date_Of_Deactivation,Today,sizeof(Today));
	 }

	 else  if (pAccountStatusPage->m_StrUserStatus.CompareNoCase("EXPIRED")==0)
      memcpy ((LPTSTR)sUserDatabaseStructure.User_Status , STATUS_EXPIRED, STATUS_EXPIRED_LEN); 

		rcode = txdsapi_update_record  ((pBYTE)&sUserDatabaseStructure, sizeof (UCF01), UCF01_DATA,strErrorMessage);
		char UserStatus[10] = { 0 }, ExpiryDate[11] = { 0 };

		//pAccountStatusPage->PopulateControlStructure(UserStatus,ExpiryDate);


		pAccountStatusPage-> new_UserStatus = sUserDatabaseStructure.User_Status;
		pAccountStatusPage->new_ExpDate =    sUserDatabaseStructure.User_Expiry_Date;
		pAccountStatusPage->PopulateControlStructure(UserStatus,ExpiryDate);
		pAccountStatusPage->PopulateHistoryTable();
		pAccountStatusPage->PopulateHistoryTable();
		


		//if(pAccountStatusPage->prev_ExpDate!=pAccountStatusPage->new_ExpDate && pAccountStatusPage->new_ExpDate != "")
		//{
		//	/*ZeroMemory ( &sHistoryData, sizeof (HST01));
		//	memcpy ((LPTSTR) sHistoryData.file_name, (LPCTSTR)"UCF01", sizeof (sHistoryData.file_name));*/
		//	memcpy ((LPTSTR)sHistoryData.changed_by,(LPTSTR)ucf01.primary_key.name, sizeof(ucf01.primary_key.name)); 	
		//	memcpy ( sHistoryData.reserved,	wstation, sizeof(sHistoryData.reserved));
		//	memcpy ( sHistoryData.date_modified, strDate, sizeof(sHistoryData.date_modified));
		//	memcpy ( sHistoryData.time_modified, strTime, sizeof(sHistoryData.time_modified));
		//	memcpy ( sHistoryData.file_primary_key,  m_StrUserNameFld, sizeof (sHistoryData.file_primary_key));
		//	memcpy ( sHistoryData.primary_key.unique_id, dest_str, sizeof (sHistoryData.primary_key.unique_id));
		//	memcpy ( sHistoryData.field_tag, "Expiry Date", sizeof (sHistoryData.field_tag));
		//	memcpy ( sHistoryData.current_data, pAccountStatusPage->new_ExpDate, sizeof (sHistoryData.current_data));
		//	memcpy ( sHistoryData.previous_data, pAccountStatusPage->prev_ExpDate, sizeof (sHistoryData.previous_data));
		//
		//	int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
		//	if ( rcode != PTEMSG_OK )
		//	{
		//		m_StrMessageLbl = strErrorMsg;
		//		UpdateData( FALSE );
		//	}
		//
		//}
		

		//PopulateUserDatabaseStructureFromDialog (&sUserDatabaseStructure );
		
		//memset(&snewUserDatabaseStructure, 0, sizeof(snewUserDatabaseStructure)) ;
		//strcpy( (char *)snewUserDatabaseStructure.password_modified_date, newDate);				
		//memcpy(snewUserDatabaseStructure.password_modified_date,newDate,sizeof(snewUserDatabaseStructure.password_modified_date));
		//nrcode = txdsapi_update_record  ((pBYTE)&snewUserDatabaseStructure, sizeof (UCF01), UCF01_DATA,strErrorMessage);		
		//if ( rcode != PTEMSG_OK && nrcode != PTEMSG_OK)

		if ( rcode != PTEMSG_OK)
		{		
			m_StrMessageLbl = strErrorMessage;
			((CPropertySheet*) this->GetParent())->SetActivePage(0);
			if (!UpdateData(FALSE)){
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}
			return FALSE;
		}
		else
		{
				  m_StrMessageLbl = "Record Updated";
				  m_StrGroupIDFld = _T("");
				  m_StrUserPasswordFld = _T("");
				  m_StrUserPassword2Fld = _T("");

					if (!UpdateData(FALSE)){
						AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
					}
					pAccessPage->CleanUserAccessDialog();
					pAccessPage->EnableUserAccessDialog(FALSE);
					pAccountStatusPage->CleanUserAccountStatusDialog();
					pAccountStatusPage->EnableUserAccountStatusDialog(FALSE);


					if ( pWorkstationPage->m_bWorkstation )
					{
						pWorkstationPage->CleanUserWorkstationDialog ();
						pWorkstationPage->EnableUserWorkstationDialog(FALSE);
					}
					if ( pChangeHistoryPage->m_bChangeHistory )
						pChangeHistoryPage->CleanChangeHistory();
					((CPropertySheet*) this->GetParent())->SetActivePage(0);
					((CPropertySheet*) this->GetParent())->SetActivePage(1);
					((CPropertySheet*) this->GetParent())->SetActivePage(2);
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
					//			SetDefID(IDC_USER_UPDATE_BUTTON);
					m_CntlUserNameFld.SetFocus();
					m_CntlUserNameFld.EnableWindow(TRUE);
					m_CntlGroupIDFld.EnableWindow( TRUE );
					m_CntlListGroupsBtn.EnableWindow( TRUE );
					SetModified ( FALSE );
					pAccountStatusPage->m_StrUserNameFld="";
					//		m_nDialogMode = MODE_DONE;
					//CPropertyPage::SetModified( FALSE );
					return CPropertyPage::OnApply();
			
		}
    
	}

	if (m_nDialogMode == MODE_VIEW)
	{ 	
		
			pAccessPage->CleanUserAccessDialog();
			pAccessPage->EnableUserAccessDialog(FALSE);
			if ( pWorkstationPage->m_bWorkstation )
			{
				pWorkstationPage->CleanUserWorkstationDialog ();
				pWorkstationPage->EnableUserWorkstationDialog(FALSE);
			}
			if ( pChangeHistoryPage->m_bChangeHistory )
				pChangeHistoryPage->CleanChangeHistory();
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

			SetModified ( FALSE );
			m_StrMessageLbl = " ";
			if (!UpdateData(FALSE)){
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}
			m_nDialogMode = MODE_DONE;
			//CPropertyPage::SetModified( FALSE );
			return CPropertyPage::OnApply();		
    }




	if (m_nDialogMode == MODE_ADD)
	{	
//		pWorkstationPage->PopulateLoginStructure ();
	//	pAccessPage->PopulateControlStructure ();

		memset(&sUserDatabaseStructure, 0, sizeof(sUserDatabaseStructure)) ;
		//memset(&snewUserDatabaseStructure, 0, sizeof(snewUserDatabaseStructure)) ;

		if (!PopulateUserDatabaseStructureFromDialog (&sUserDatabaseStructure)) {		
			return FALSE;
		}

		strncpy(ExpiryDate,(char*)sUserDatabaseStructure.User_Expiry_Date,sizeof(sUserDatabaseStructure.User_Expiry_Date));
		jday1=Txutils_Calculate_Julian_Date((unsigned char*)ToDayDate);
		jday2=Txutils_Calculate_Julian_Date((unsigned char*)ExpiryDate);
	
		if(jday2-jday1 <= 0)
		{
			sprintf(tmpbuff, "Expiry Date must be greater than Today's Date  %02d/%02d/%d",day,month,year);
			AfxMessageBox(tmpbuff);
			return FALSE;
		}
		
		/**** Encrypt the password  - Start ****/
		{
			int len = strlen((char *)sUserDatabaseStructure.password);
			if(len <= MAX_USERPWD_LEN) 
			
				int encrypt_len = gui_password_encryption((BYTE *)sUserDatabaseStructure.password,len);
		}
		/**** Encrypt the password  - End ****/
		//copy todaysdate to subsystem15
		memcpy(sUserDatabaseStructure.date_added, newDate, sizeof(sUserDatabaseStructure.date_added));
		memcpy(sUserDatabaseStructure.password_modified_date, newDate, sizeof(sUserDatabaseStructure.password_modified_date));
		memcpy(sUserDatabaseStructure.password_1,sUserDatabaseStructure.password,sizeof(sUserDatabaseStructure.password_1));
		/*strcpy((char *)sUserDatabaseStructure.accountstatus,USER_UNLOCKED);*/
		/***************************first time when an user is added,Accountstatus(ucf01 table) is set to "new" which will***
		***enforce the first time logging user to change the password - TF_Veena*********************************************/
			strcpy((char *)sUserDatabaseStructure.accountstatus,USER_NEW);
			//strcpy((char *)sUserDatabaseStructure.accountstatus,)

		/***************************************************************************************************************/
		
		
		//memcpy(snewUserDatabaseStructure.primary_key.name,sUserDatabaseStructure.primary_key.name,sizeof(sUserDatabaseStructure.primary_key.name));
		//memcpy(snewUserDatabaseStructure.password_1,sUserDatabaseStructure.password,sizeof(sUserDatabaseStructure.password));
		//memcpy(snewUserDatabaseStructure.password_modified_date,sUserDatabaseStructure.date_added,sizeof(sUserDatabaseStructure.date_added));
		//memcpy(sUserDatabaseStructure.password_modified_date,sUserDatabaseStructure.date_added,sizeof(sUserDatabaseStructure.date_added));		
		//strcpy((char *)snewUserDatabaseStructure.accountstatus,"unlocked");
		//rcode = txdsapi_insert_record  ((pBYTE)&snewUserDatabaseStructure, sizeof (UCF01), UCF01_DATA, strErrorMessage);
		
		
		rcode = txdsapi_insert_record  ((pBYTE)&sUserDatabaseStructure, sizeof (UCF01), UCF01_DATA, strErrorMessage);
		
		//if ( rcode != PTEMSG_OK && nrcode != PTEMSG_OK)
		if (rcode != PTEMSG_OK)
		{		
			m_StrMessageLbl = strErrorMessage;
			if (!UpdateData(FALSE)){
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);
			}
			((CPropertySheet*) this->GetParent())->SetActivePage(0);
			return FALSE;
		}
		else 
		{   /**adding proper data into the HST01 table, so it reflects proper output in the audit log file, -TF_Veena**/
			
				memcpy ((LPSTR)m_strCurrentData, "Record Added", sizeof (m_strCurrentData));
			      //memcpy((LPSTR)m_strCurrentData,sUserDatabaseStructure.primary_key.name,sizeof(m_strCurrentData));
			/************************************************************************************************************/
			PopulateHistoryTable();
			m_StrMessageLbl = "Record Added";	

            m_CntlUserNameFld.SetFocus();
			if (!UpdateData(FALSE)){
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}
			pAccessPage->CleanUserAccessDialog();
			pAccessPage->EnableUserAccessDialog(FALSE);
			if ( pWorkstationPage->m_bWorkstation )
			{
				pWorkstationPage->CleanUserWorkstationDialog ();
				pWorkstationPage->EnableUserWorkstationDialog(FALSE);
			}
			if ( pChangeHistoryPage->m_bChangeHistory )
				pChangeHistoryPage->CleanChangeHistory();
			m_CntlUserNameFld.EnableWindow(TRUE);
			SetModified ( FALSE );
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
			m_CntlUserNameFld.SetFocus();
		 //CPropertyPage::SetModified( FALSE );
         return CPropertyPage::OnApply();
		} 
	}
	
	m_nDialogMode = MODE_DONE;
	//CPropertyPage::SetModified( FALSE );
	return CPropertyPage::OnApply();
}

void CUserDefinition::PopulateHistoryTable()
{
    HST01 sHistoryData;
    CHAR dest_str[18];   
	CMyUtils myUtilsObject;
	char strErrorMsg[200] = "";
    CString temp;
    CHAR wstation[21]; DWORD wsize;

    char strDate[9] = "";
    char strTime[7] = "";

    memset ( strDate, 0, sizeof (strDate));
    memset ( strTime, 0, sizeof (strTime));
    
    myUtilsObject.GetSystemDate(strDate);
    myUtilsObject.GetSystemTime(strTime);

    pintime_get_gmt ( dest_str );
    ZeroMemory ( &sHistoryData, sizeof (HST01));
    memcpy ((LPTSTR) sHistoryData.file_name,     (LPCTSTR)"UCF01", sizeof (sHistoryData.file_name));
    temp = strDate;
    temp = temp.Right(4) + temp.Left(2) + temp.Mid(2,2);
    

    wsize = sizeof( wstation );
    GetComputerName( wstation, &wsize );

	memcpy ((LPTSTR)sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
    memcpy ((LPTSTR)sHistoryData.date_modified, temp, sizeof (sHistoryData.date_modified));
    memcpy ((LPTSTR)sHistoryData.file_primary_key,  (LPCTSTR)m_StrUserNameFld, sizeof (sHistoryData.file_primary_key));
    memcpy ((LPTSTR)sHistoryData.primary_key.unique_id,     (LPCTSTR)dest_str, sizeof (sHistoryData.primary_key.unique_id));        
    memcpy ((LPTSTR)sHistoryData.time_modified,     (LPCTSTR)strTime, sizeof (sHistoryData.time_modified));              
    memcpy ((LPTSTR)sHistoryData.current_data, (LPCTSTR)m_strCurrentData, sizeof (sHistoryData.current_data));
   	memcpy ((LPTSTR)sHistoryData.changed_by,(LPTSTR)ucf01.primary_key.name, sizeof(sHistoryData.changed_by));
 
	
    int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
		if ( rcode != PTEMSG_OK )
		{
			m_StrMessageLbl = strErrorMsg;
			UpdateData( FALSE );
		}    
}

BOOL CUserDefinition::PopulatenewUserDatabaseStructureFromDialog ( pUCF01 pnewUser_data ) 
{
	//pAccountStatusPage->PopulateControlStructure();//check in declaration, function added with arguments
	//pUserAccountStatusPage->PopulateUserStatus();
	memcpy ((LPTSTR) pnewUser_data->primary_key.name,m_StrUserNameFld, sizeof (pnewUser_data->primary_key.name));


	strcpy((LPSTR)pnewUser_data->accountstatus,pAccountStatusPage->m_bAccStatus);
	return TRUE;
}

BOOL CUserDefinition::PopulateUserDatabaseStructureFromDialog ( pUCF01 pUser_data ) 
{
	UCF01 sUserDatabaseStructure;
	char strErrorMessage[200];
	CMyUtils myUtilsObject;
   char strDate[9] = "";    
   char UserStatus[15]={0};
   char ExpiryDate[11]={0};
   CString date;
  int year,month,day; char ExpDate[10];

   memset ( strDate, 0, sizeof (strDate));   
	//ZeroMemory (pUser_data, sizeof(UCF01)) ;
   memset(pUser_data, 0 ,sizeof(UCF01));
    
   myUtilsObject.GetSystemDate(strDate);
	date = strDate;
        date = date.Right(4) + date.Left(2) + date.Mid (2,2);

	pAccessPage->UpdateData(TRUE);
	pAccessPage->PopulateControlStructure();
	pWorkstationPage->PopulateLoginStructure();
	//pAccountStatusPage->PopulateUserStatus();
	memcpy ((LPSTR)pUser_data->group_id, pAccessPage->m_StrGroupIDFld, sizeof (pUser_data->group_id) );
	if ( m_nDialogMode == MODE_EDIT )
	{
		//Krishna@TF, 7.6.2011: Added PopulateControlStructure() of Account Status Class to maintain uniformity
		//pAccountStatusPage->PopulateControlStructure(UserStatus,ExpiryDate);
		pAccountStatusPage->PopulateControlStructure();
		pAccessPage->PopulateHistoryTable();	
		pWorkstationPage->PopulateHistoryTable();
		pAccountStatusPage->PopulateHistoryTable();
		pAccessPage->UpdateData(TRUE);
		//pUserAccountStatusPage->PopulateControlStructure();

		
		memcpy ((LPSTR)pUser_data->password, m_strPassword,   sizeof (pUser_data->password) );
		if(pAccessPage->m_strNewPassword.IsEmpty())
		{
			memcpy ((LPSTR)pUser_data->password, m_strPassword,   sizeof (pUser_data->password) );
		}
		else
		{
			strcpy((LPSTR)pUser_data->password_4,(LPSTR)pUser_data->password_3);
			strcpy((LPSTR)pUser_data->password_3,(LPSTR)pUser_data->password_2);
			strcpy((LPSTR)pUser_data->password_2,(LPSTR)pUser_data->password_1);		
			strcpy((LPSTR)pUser_data->password_1, (LPSTR)pUser_data->password);
			memcpy((LPSTR)pUser_data->password, pAccessPage->m_strNewPassword, sizeof (pUser_data->password) );
			strcpy((LPSTR)pUser_data->password_modified_date, date);
				
		}
		
	}
	//else if (m_nDialogMode == MODE_ADD)
	//{
	//	//fun call
	//	pUserAccountStatusPage->PopulateUserStatus();
	//}
	
	CString str1;
	CString str2;
	if( pAccessPage->m_StrUserNameFld2.GetLength() > 10)
	{
		str1 =  pAccessPage->m_StrUserNameFld2.Left(10);
		str2 =  pAccessPage->m_StrUserNameFld2.Right( pAccessPage->m_StrUserNameFld2.GetLength() - 10);
	}
	else
	{
		str1 =  pAccessPage->m_StrUserNameFld2;
		str2 = "";
	}
	memcpy((LPTSTR) pUser_data->user_info_1,str1, sizeof(pUser_data->user_info_1));
	memcpy((LPTSTR) pUser_data->user_info_2,str2, sizeof(pUser_data->user_info_2));
	
	memset(pUser_data->user_info_5,0,sizeof(pUser_data->user_info_5));
	sprintf((LPSTR)pUser_data->user_info_5,"%d",pAccessPage->m_CntlOverrideCheck.GetCheck());

    memcpy ( (LPSTR)pUser_data->control, &pAccessPage->m_sControlNew, sizeof(m_sControl1));   
	memcpy ((LPTSTR) pUser_data->primary_key.name,     m_StrUserNameFld, sizeof (pUser_data->primary_key.name));
	
	//Krishna@TF, 7.6.11, Copy the Accoun updated status to DB
	//	memcpy( (LPTSTR)pUser_data->accountstatus, pAccountStatusPage->m_bAccStatus, pAccountStatusPage->m_bAccStatus.GetLength());
	/************when an user password is updated,Accountstatus(ucf01 table) is set to "new" which will***
	***enforce the the user to change the password - TF_Veena*********************************************/
	//if(strcmp((char*) ucf01.accountstatus,USER_NEW) == 0)
	if(strcmp((char *)temp_accountstatus,USER_NEW) == 0)
	{
		//pAccountStatusPage->m_bAccStatus = USER_NEW;
		//memcpy( (LPTSTR)pUser_data->accountstatus, pAccountStatusPage->m_bAccStatus, pAccountStatusPage->m_bAccStatus.GetLength());
		memcpy( (LPTSTR)pUser_data->accountstatus,USER_NEW, sizeof(pUser_data->accountstatus));
		memset((char *)temp_accountstatus, 0 ,sizeof((char *)temp_accountstatus));
	}
	else
	{
		memcpy( (LPTSTR)pUser_data->accountstatus, pAccountStatusPage->m_bAccStatus, pAccountStatusPage->m_bAccStatus.GetLength());
	}
	

	
	if ( m_nDialogMode == MODE_ADD )
	{
		memcpy ((LPTSTR) pUser_data->password,    m_StrUserPasswordFld, sizeof (pUser_data->password));  
        
	    memcpy ( (LPSTR)pUser_data->date_added, date, sizeof(strDate)); 
	}
   else  if ( m_nDialogMode == MODE_EDIT ) 
      memcpy ( pUser_data->date_added, sOldUCF01Record.date_added, sizeof ( sOldUCF01Record.date_added ) );
      memcpy ( (LPSTR)pUser_data->login, &pWorkstationPage->m_sLoginNew, sizeof(m_sLogin1)); 





	  //pAccountStatusPage->m_StrUserStatus=(UCF01_CUserStatus);
	  /***********ram commented *********/
	if ( pAccountStatusPage->m_StrUserStatus.CompareNoCase( "ACTIVE")==0)
      memcpy ((LPTSTR)pUser_data->User_Status , STATUS_ACTIVE, STATUS_ACTIVE_LEN); 

	 else if (pAccountStatusPage->m_StrUserStatus.CompareNoCase("INACTIVE")==0)
	 memcpy ((LPTSTR)pUser_data->User_Status , STATUS_INACTIVE, STATUS_INACTIVE_LEN); 
	 
	 else  if (pAccountStatusPage->m_StrUserStatus.CompareNoCase("EXPIRED")==0)
      memcpy ((LPTSTR)pUser_data->User_Status , STATUS_EXPIRED, STATUS_EXPIRED_LEN); 

	if ((pAccountStatusPage->m_StrUserExpiryDate != 0))	
	 {
		year = pAccountStatusPage->m_StrUserExpiryDate.GetYear();
		month = pAccountStatusPage->m_StrUserExpiryDate.GetMonth();
		day = pAccountStatusPage->m_StrUserExpiryDate.GetDay();

		sprintf(ExpDate,"%d%02d%02d",year,month,day);

		memcpy((LPTSTR)pUser_data->User_Expiry_Date,ExpDate,sizeof(ExpDate));
	 }
	else
	{
		strncpy((LPTSTR)pUser_data->User_Expiry_Date,"",sizeof(ExpDate));
	}
	return TRUE;

}


BOOL CUserDefinition::PopulateUserDialogFromDatabase ( UCF01 pUserRecord,UCF01 pnewUserRecord)
{
	//m_StrUserStatus = pnewUserRecord.User_Status;
	pAccessPage->m_StrUserNameFld = (LPSTR)pUserRecord.primary_key.name ;
    pAccessPage->m_StrGroupIDFld  = (LPSTR)pUserRecord.group_id ;
	pAccessPage->Old_Gid          = (LPSTR) pUserRecord.group_id;
	pAccessPage->Old_Uname		  = (LPSTR)pUserRecord.user_info_1 ;
	pAccessPage->Old_Uname		  += (LPSTR)pUserRecord.user_info_2 ;
	pAccessPage->m_StrUserNameFld2 = (LPSTR) pUserRecord.user_info_1;
	pAccessPage->m_StrUserNameFld2 += (LPSTR) pUserRecord.user_info_2;
	pAccessPage->Old_pswrd = pUserRecord.password;
	pAccessPage->m_CntlOverrideCheck.SetCheck(atoi((LPSTR)pUserRecord.user_info_5));
   
    memcpy ( &pAccessPage->m_sControl,(LPSTR)pUserRecord.control, sizeof(m_sControl1));
	memcpy( &pAccessPage->m_sUserRecord, &pUserRecord, sizeof(pUserRecord));

    //****************** Workstation Page *********************

    pWorkstationPage->m_StrUserNameFld = (LPSTR)pUserRecord.primary_key.name ;
    pWorkstationPage->m_StrGroupIDFld = (LPSTR)pUserRecord.group_id ;
    memcpy ( &pWorkstationPage->m_sLogin,(LPSTR)pUserRecord.login, sizeof(m_sLogin1)); 
	
	 //****************** AccountStatus Page *********************
	pAccountStatusPage->m_StrUserNameFld = (LPSTR)pnewUserRecord.primary_key.name;
	if(strcmp((LPTSTR)(pnewUserRecord.User_Status),"DELETED")==0)
	{
		pAccountStatusPage->m_bAccStatus = (LPSTR)pnewUserRecord.User_Status;
	}
	else
	{
	pAccountStatusPage->m_bAccStatus = (LPSTR)pnewUserRecord.accountstatus;
	}
  
	
	pAccountStatusPage->m_bPopulate = TRUE;

	UpdateData(FALSE);

	if ( pAccessPage->m_bAccess )
		pAccessPage->PopulateDialogFromDatabase();

	if ( pWorkstationPage->m_bWorkstation )
		pWorkstationPage->PopulateWorkstationDialog();

	//pAccountStatusPage->new_UserStatus = pAccountStatusPage->m_StrUserStatus ;


    return TRUE;
}

BOOL CUserDefinition::OnSetActive() 
{
	((CUserPropertySheet*) this->GetParent())->m_nActiveTab = 0;

	return TRUE;
}

void CUserDefinition::OnOK() 
{
	if ( m_nDialogMode == MODE_DONE)	
		CPropertyPage::OnOK();
	else 
	{
		m_nDialogMode = MODE_DONE;
		return;
	}
}

void CUserDefinition::OnCancel() 
{
	// TODO: Add your specialized code here and/or call the base class

   //clear out all the user fields on the main tab
	m_StrGroupIDFld = _T("");
	m_StrUserNameFld = _T("");
	m_StrUserPasswordFld = _T("");
	m_StrMessageLbl = _T("");
	m_StrUserPassword2Fld = _T("");

   m_nDialogMode   = MODE_DONE;

	CPropertyPage::OnCancel();
}



BOOL CUserDefinition::OnHelpInfo(HELPINFO* pHelpInfo) 
{
/*	HtmlHelp (AfxGetMainWnd()->GetSafeHwnd(),
	"security.chm>Main",HH_HELP_CONTEXT,IDD_USER_DEFINITON);
	
*/	
	return TRUE;
}

void CUserDefinition::OnUserAcessButton() 
{

/*	UCF01 sUserRecord; 
	UCF01 snewUserRecord; 
	char strErrorMessage[200];
    m_nDialogMode = MODE_EDIT;
	m_nPrevMode = MODE_EDIT;

	m_StrMessageLbl = "";

    if (!CheckDialogForCompleteness())
        return;


	  memcpy ((LPSTR)(sUserRecord.primary_key.name), m_StrUserNameFld, sizeof (sUserRecord.primary_key.name) );
	memcpy ((LPSTR)(snewUserRecord.primary_key.name), m_StrUserNameFld, sizeof (snewUserRecord.primary_key.name) );

	int rcode1 = txdsapi_get_record  ((pBYTE)&snewUserRecord, sizeof (UCF01), UCF01_DATA, strErrorMessage);
	int rcode = txdsapi_get_record  ((pBYTE)&sUserRecord, sizeof (UCF01), UCF01_DATA, strErrorMessage);

	if ( rcode != PTEMSG_OK && rcode1 != PTEMSG_OK)
	{	    
		switch( rcode )
		{
			 case PTEMSG_NOT_FOUND:
				sprintf( strErrorMessage, "ERROR: User record for %s does not exist", m_StrUserNameFld );
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

	UpdateData( FALSE );	
	pAccountStatusPage->m_nDialogMode = MODE_EDIT;
	((CPropertySheet*) this->GetParent())->SetActivePage(3); 
	((CPropertySheet*) this->GetParent())->SetWindowText("Update User");
	pAccountStatusPage->m_bStatus=TRUE;
	PopulateUserDialogFromDatabase( sUserRecord,snewUserRecord );
	pWorkstationPage->m_bWorkstation=TRUE;
	m_CntlUserNameFld.EnableWindow(FALSE);*/
}


BOOL CUserDefinition::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}

void CUserDefinition::OnGroupList() 
{
	CNameList listDlg;
    int nReturnCode;

	m_StrMessageLbl = "";
	UpdateData(TRUE);
    listDlg.m_nIndicator = MODE_GROUP;
	listDlg.strPrimaryKey1 = m_StrGroupIDFld;

    nReturnCode = listDlg.DoModal();
    if ( nReturnCode == IDOK)
        m_StrGroupIDFld = listDlg.m_strName;
        
    if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}	
}

void CUserDefinition::SetExpiryDate(char *ToDayDate)
{
	CTime theTime;	
	int year,month,day;
	theTime = CTime::GetCurrentTime();
	year= 	theTime.GetYear ();
	month = theTime.GetMonth();
	day = theTime.GetDay ();
	
	sprintf(ToDayDate,"%d%02d%02d",year,month,day);//converting toDayDate to yyyymmdd format
}