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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\WorkstationDefinition.cpp  $
   
      Rev 1.9   Dec 01 2003 11:30:26   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.8   Jan 14 2000 16:25:02   mbysani
   bug fixes
   
      Rev 1.1   Sep 22 1999 16:02:56   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:32   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:42   iarustam
   Initial Release
   
      Rev 1.4   May 10 1999 11:42:08   dperson
   Standardized error messages
   
      Rev 1.3   Apr 29 1999 14:57:16   iarustam
   Bug fixes
   
      Rev 1.2   Mar 16 1999 16:30:40   iarustam
   Bug fixes
   
      Rev 1.1   Mar 04 1999 13:32:38   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:41:30   iarustam
   Initial Release
  
    
*
************************************************************************************/
// WorkstationDefinition.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "WorkstationDefinition.h"
#include "MyUtils.h"
#include "NameList.h"
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

extern CString name;
extern UCF01 ucf01;
extern CSharedMem shrmem;

/////////////////////////////////////////////////////////////////////////////
// CWorkstationDefinition property page

IMPLEMENT_DYNCREATE(CWorkstationDefinition, CPropertyPage)

CWorkstationDefinition::CWorkstationDefinition() : CPropertyPage(CWorkstationDefinition::IDD)
{
	//{{AFX_DATA_INIT(CWorkstationDefinition)
	m_Message = _T("");
	m_WorkstationIdStr = _T("");
	//}}AFX_DATA_INIT
}

CWorkstationDefinition::~CWorkstationDefinition()
{
}

void CWorkstationDefinition::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWorkstationDefinition)
	DDX_Control(pDX, IDC_WORKSTATION_VIEW_BUTTON, m_ViewButton);
	DDX_Control(pDX, IDC_WORKSTATION_UPDATE_BUTTON, m_UpdateButton);
	DDX_Control(pDX, IDC_WORKSTATION_LIST_BUTTON, m_ListButton);
	DDX_Control(pDX, IDC_WORKSTATION_DELETE_BUTTON, m_DeleteButton);
	DDX_Control(pDX, IDC_WORKSTATION_ADD_BUTTON, m_AddButton);
	DDX_Control(pDX, IDC_WORKSTATION_ID, m_WorkstationIdCtrl);
	DDX_Control(pDX, IDC_MESSAGE, m_MessageCtrl);
	DDX_Text(pDX, IDC_MESSAGE, m_Message);
	DDX_Text(pDX, IDC_WORKSTATION_ID, m_WorkstationIdStr);
	DDV_MaxChars(pDX, m_WorkstationIdStr, 20);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWorkstationDefinition, CPropertyPage)
	//{{AFX_MSG_MAP(CWorkstationDefinition)
	ON_BN_CLICKED(IDC_WORKSTATION_ADD_BUTTON, OnWorkstationAddButton)
	ON_BN_CLICKED(IDC_WORKSTATION_DELETE_BUTTON, OnWorkstationDeleteButton)
	ON_BN_CLICKED(IDC_WORKSTATION_LIST_BUTTON, OnWorkstationListButton)
	ON_BN_CLICKED(IDC_WORKSTATION_UPDATE_BUTTON, OnWorkstationUpdateButton)
	ON_BN_CLICKED(IDC_WORKSTATION_VIEW_BUTTON, OnWorkstationViewButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorkstationDefinition message handlers

//*************************************************************************************
//*************************************************************************************
void CWorkstationDefinition::OnWorkstationAddButton() 
{
	// TODO: Add your control notification handler code here
	char strErrorMessage[200];
    
   CMyUtils myUtilsObject;
	m_Message = "";

	if (!CheckDialogForCompleteness())
   {
	   m_nDialogMode = MODE_DONE; 
      return;
   }
	 
	ZeroMemory(&pDescriptionPage->atp02,sizeof(ATP02)) ;

   strcpy ((char *)(&pDescriptionPage->atp02.primary_key.workstation_id), m_WorkstationIdStr );
	int rcode = txdsapi_get_record ((pBYTE)&pDescriptionPage->atp02, sizeof (ATP02),ATP02_DATA, strErrorMessage);
	if (rcode == PTEMSG_OK)
	{    
	    m_nDialogMode = MODE_DONE; 
	    m_Message = "ERROR: Workstation already exists";
		 if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	    }
		 return;
   }
	   
	m_nDialogMode = MODE_ADD; 
	pDescriptionPage->m_nDialogMode = MODE_ADD; 
	((CPropertySheet*) this->GetParent())->SetWindowText("Add Workstation");
	//set version info in the title bar
	CString appProdVer;
	CString lpszTitle;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
	lpszTitle = lpszTitle + " - Version - ";
	lpszTitle += appProdVer;
	((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);
	if ( pDescriptionPage->m_bDescription )
	{
		pDescriptionPage->UpdateData(FALSE);
		pDescriptionPage->EnableWorkstationDescriptionDialog(TRUE);
	}
		if ( pChangeHistoryPage->m_bChangeHistory )
			pChangeHistoryPage->CleanChangeHistory();
	
	((CPropertySheet*) this->GetParent())->SetActivePage(1);
	pDescriptionPage->CleanWorkstationDescriptionDialog();
	pDescriptionPage->m_WorkstationIdStr = m_WorkstationIdStr; 
	pDescriptionPage->m_WorkstationStatus.SetCurSel( 0 ); 
   strcpy ((char *)(&pDescriptionPage->atp02.workstation_status), "A" );
	pDescriptionPage->UpdateData(FALSE);
	m_WorkstationIdCtrl.EnableWindow(FALSE);	
   SetModified( TRUE );
}


//*************************************************************************************
//*************************************************************************************
void CWorkstationDefinition::OnWorkstationDeleteButton() 
{
	// TODO: Add your control notification handler code here
	CString strMessage;
   int nReturnCode;
   ATP02 sWorkstation;
	char strErrorMessage[200];
	CNameList listDlg;

    m_nDialogMode = MODE_EDIT;
	m_Message = "";

    if (!CheckDialogForCompleteness())
        return;   
   
    if (!UpdateData(TRUE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}
    
	strMessage.Format ("Are you sure you want to delete %s ?", (LPCTSTR) m_WorkstationIdStr) ;

	nReturnCode = AfxMessageBox ((LPCTSTR)strMessage, MB_OKCANCEL|MB_ICONQUESTION) ;
	if (nReturnCode != IDOK) return ;

	memcpy ((LPTSTR)sWorkstation.primary_key.workstation_id, m_WorkstationIdStr, sizeof (sWorkstation.primary_key.workstation_id)) ;

	ZeroMemory (&listDlg.sUserRecord, sizeof (UCF01));
	memcpy (listDlg.sUserRecord.login[0].terminals, m_WorkstationIdStr, sizeof (listDlg.sUserRecord.login[0].terminals));
	memcpy (listDlg.sUserRecord.primary_key.name," ", sizeof (listDlg.sUserRecord.primary_key.name));

	int rcode = txdsapi_get_list ((pBYTE)&listDlg.sUserRecord, sizeof (UCF01), 
									(pBYTE)&listDlg.sUserList, sizeof (UCF01_LIST),
									ST2_DB_GET_UCF01_LIST_BY_TERMINAL, UCF01_DATA, strErrorMessage);

	if ( strcmp ((char*)&listDlg.sUserList.num_returned, "0") != 0)
	{
		strMessage = "You cannot delete this workstation because it currently has users assigned to it. \nDo you want to see the list of users assigned to this workstation?";

		nReturnCode = AfxMessageBox ((LPCTSTR)strMessage, MB_OKCANCEL|MB_ICONQUESTION) ;
		if (nReturnCode != IDOK) return ;

	   listDlg.m_nIndicator = MODE_DISPLAY_USER;
   
	   nReturnCode = listDlg.DoModal();
		
	   return;

	}

	ZeroMemory (&listDlg.sGroupRecord, sizeof (UCF02));
	memcpy (listDlg.sGroupRecord.login[0].terminals, m_WorkstationIdStr, sizeof (listDlg.sGroupRecord.login[0].terminals));
	memcpy (listDlg.sGroupRecord.primary_key.group_id," ", sizeof (listDlg.sGroupRecord.primary_key.group_id));

	rcode = txdsapi_get_list ((pBYTE)&listDlg.sGroupRecord, sizeof (UCF02), 
									(pBYTE)&listDlg.sGroupList, sizeof (UCF02_LIST),
									ST2_DB_GET_UCF02_LIST_BY_TERMINAL, UCF02_DATA, strErrorMessage);

	if ( strcmp ((char*)&listDlg.sGroupList.num_returned, "0") != 0)
	{
		strMessage = "You cannot delete this workstation because it currently has groups assigned to it. \nDo you want to see the list of groups assigned to this workstation?";

		nReturnCode = AfxMessageBox ((LPCTSTR)strMessage, MB_OKCANCEL|MB_ICONQUESTION) ;
		if (nReturnCode != IDOK) return ;

	   listDlg.m_nIndicator = MODE_DISPLAY_GROUP;
   
	   nReturnCode = listDlg.DoModal();
		
	   return;

	}


	rcode = txdsapi_delete_record ((pBYTE)&sWorkstation, sizeof ( ATP02 ), ATP02_DATA, strErrorMessage);
	if (rcode != PTEMSG_OK)
	{        
		m_Message = strErrorMessage;

		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return ;
	}

    else
    {
      memset ( m_strPreviousData, 0, sizeof (m_strPreviousData));
      memset ( m_strCurrentData, 0, sizeof (m_strCurrentData));
      memset ( m_strFieldTag, 0, sizeof (m_strFieldTag));
      memcpy ((LPSTR)m_strCurrentData, "Record Deleted", sizeof (m_strCurrentData));
      PopulateHistoryTable();

      m_Message = "Record Deleted";
		m_WorkstationIdStr = _T("");

		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}

		if ( pDescriptionPage->m_bDescription )
		{
			pDescriptionPage->CleanWorkstationDescriptionDialog();
			pDescriptionPage->EnableWorkstationDescriptionDialog(FALSE);
		}

		if ( pChangeHistoryPage->m_bChangeHistory )
			pChangeHistoryPage->CleanChangeHistory();
			m_WorkstationIdCtrl.EnableWindow(TRUE);
		((CPropertySheet*) this->GetParent())->SetActivePage(0);
		SetModified ( FALSE );
    }	
}



//*************************************************************************************
//*************************************************************************************
void CWorkstationDefinition::OnWorkstationListButton() 
{
	CNameList listDlg;
   int nReturnCode;

   UpdateData(TRUE);

	m_Message = "";
   listDlg.m_nIndicator = MODE_WORKSTATION;
    listDlg.strPrimaryKey1 = m_WorkstationIdStr;   
   
   nReturnCode = listDlg.DoModal();
   if ( nReturnCode == IDOK)
        m_WorkstationIdStr = name;
        
    if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}		
	
	
}


//*************************************************************************************
//*************************************************************************************
void CWorkstationDefinition::OnWorkstationUpdateButton() 
{
	// TODO: Add your control notification handler code here
	char strErrorMessage[200];
    
   CMyUtils myUtilsObject;
	m_Message = "";

	if (!CheckDialogForCompleteness())
   {
		m_nDialogMode = MODE_DONE;
      return;
   }
	 
	ZeroMemory(&pDescriptionPage->atp02,sizeof(ATP02)) ;
   strcpy ((char *)(&pDescriptionPage->atp02.primary_key.workstation_id), m_WorkstationIdStr );
	int rcode = txdsapi_get_record ((pBYTE)&pDescriptionPage->atp02,sizeof(ATP02), ATP02_DATA, strErrorMessage);
	if ( rcode != PTEMSG_OK )
	{	    
      switch( rcode )
      {
         case PTEMSG_NOT_FOUND:
            sprintf( strErrorMessage, "ERROR: Workstation record for %s does not exist", m_WorkstationIdStr );
            break;
      }

      m_Message = strErrorMessage;
		m_nDialogMode = MODE_DONE;
		if (!UpdateData(FALSE))
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		      
      return;
	}

   strStatus =  pDescriptionPage->atp02.workstation_status;
   
   m_nDialogMode = MODE_EDIT;
   pDescriptionPage->m_nDialogMode = MODE_EDIT; 
   pChangeHistoryPage->m_nDialogMode = MODE_EDIT;
	pChangeHistoryPage->bNext = FALSE;
   
   ((CPropertySheet*) this->GetParent())->SetActivePage(1);
	((CPropertySheet*) this->GetParent())->SetWindowText("Update Workstation");
	//set version info in the title bar
	CString appProdVer;
	CString lpszTitle;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
	lpszTitle = lpszTitle + " - Version - ";
	lpszTitle += appProdVer;
	((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);
	if ( pDescriptionPage->m_bDescription )
	{
		pDescriptionPage->UpdateData(FALSE);
		pDescriptionPage->EnableWorkstationDescriptionDialog(TRUE);
	}
    ZeroMemory (&pChangeHistoryPage->sHistoryRecord, sizeof (HST01));


    memcpy ((LPSTR)(pChangeHistoryPage->sHistoryRecord.primary_key.unique_id), " ", sizeof (pChangeHistoryPage->sHistoryRecord.primary_key.unique_id));
    memcpy ((LPSTR)(pChangeHistoryPage->sHistoryRecord.file_name), "ATP02", sizeof (pChangeHistoryPage->sHistoryRecord.file_name));
    memcpy ((LPSTR)(pChangeHistoryPage->sHistoryRecord.file_primary_key), m_WorkstationIdStr, sizeof (pChangeHistoryPage->sHistoryRecord.file_primary_key));

	if ( pChangeHistoryPage->m_bChangeHistory )
		pChangeHistoryPage->PopulateHistoryDialog();
	
	pDescriptionPage->CleanWorkstationDescriptionDialog();
	pDescriptionPage->m_WorkstationIdStr = m_WorkstationIdStr; 
   if( strcmp( (char *)&pDescriptionPage->atp02.workstation_status, "A" ) == 0 )
	   pDescriptionPage->m_WorkstationStatus.SetCurSel( 0 ); 
   else
	   pDescriptionPage->m_WorkstationStatus.SetCurSel( 1 ); 

   pDescriptionPage->UpdateData(FALSE);
	m_WorkstationIdCtrl.EnableWindow(FALSE);	
}


//*************************************************************************************
//*************************************************************************************
void CWorkstationDefinition::OnWorkstationViewButton() 
{
	// TODO: Add your control notification handler code here
   ATP02 sWorkstation;
	char strErrorMessage[200];

    m_nDialogMode = MODE_VIEW;
    pDescriptionPage->m_nDialogMode = MODE_VIEW;    
    pChangeHistoryPage->m_nDialogMode = MODE_VIEW;
	pChangeHistoryPage->bNext = FALSE;
    SetModified( FALSE );
    if (!CheckDialogForCompleteness())
        return;	   

   // m_pUserSheet->m_psh.dwFlags |= PSH_NOAPPLYNOW ; 
   // m_pUserSheet->m_psh.dwFlags &= ~PSH_HASHELP;*/

   memcpy ((LPSTR)(sWorkstation.primary_key.workstation_id), m_WorkstationIdStr, sizeof (sWorkstation.primary_key.workstation_id) );   
	int rcode = txdsapi_get_record ((pBYTE)&sWorkstation,sizeof(ATP02), ATP02_DATA, strErrorMessage);
	if (rcode != PTEMSG_OK)
	{     
      m_Message = strErrorMessage;
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
        return;
   }
   
   ((CPropertySheet*) this->GetParent())->SetActivePage(1);
    ZeroMemory (&pChangeHistoryPage->sHistoryRecord, sizeof (HST01));


    memcpy ((LPSTR)(pChangeHistoryPage->sHistoryRecord.primary_key.unique_id), " ", sizeof (pChangeHistoryPage->sHistoryRecord.primary_key.unique_id));
    memcpy ((LPSTR)(pChangeHistoryPage->sHistoryRecord.file_name), "ATP02", sizeof (pChangeHistoryPage->sHistoryRecord.file_name));
    memcpy ((LPSTR)(pChangeHistoryPage->sHistoryRecord.file_primary_key), m_WorkstationIdStr, sizeof (pChangeHistoryPage->sHistoryRecord.file_primary_key));
    
	if ( pChangeHistoryPage->m_bChangeHistory )
		pChangeHistoryPage->PopulateHistoryDialog();

	((CPropertySheet*) this->GetParent())->SetWindowText("View Workstation");
	//set version info in the title bar
	CString appProdVer;
	CString lpszTitle;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
	lpszTitle = lpszTitle + " - Version - ";
	lpszTitle += appProdVer;
	((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);

	PopulateWorkstationDialogFromDatabase ( sWorkstation );	
}



//*************************************************************************************
//*************************************************************************************
BOOL CWorkstationDefinition::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	CFont m_font;
	LOGFONT lf;

	m_WorkstationIdCtrl.Clear();
	m_WorkstationIdStr.Empty();
	memset ( &lf, 0, sizeof (lf));
	lf.lfHeight = 12;
	lf.lfWeight = 700;
	strcpy(lf.lfFaceName, "Arial");
	m_font.CreateFontIndirect (&lf);
	GetDlgItem(IDC_MESSAGE)->SetFont (&m_font);
	m_Message = "";

   m_nDialogMode = MODE_DONE;
   pDescriptionPage->m_nDialogMode = MODE_DONE;    
   ZeroMemory( &pDescriptionPage->atp02, sizeof( ATP02 ) );


   CString temp;
	int nIndex = -1;

	for ( int i = 0; i <=15; i++ )
	{
		temp = ucf01.control[i].subsystem_name;
		if ( strcmp ( temp, "System Configuration" ) == 0)
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
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


//*************************************************************************************
//*************************************************************************************
BOOL CWorkstationDefinition::CheckDialogForCompleteness()
{
    CMyUtils myUtilsObject;

    if (!UpdateData(TRUE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}

    if (myUtilsObject.IsEmpty (m_WorkstationIdStr, this))
    {
		m_Message = "Please enter Workstation ID";	
		m_WorkstationIdCtrl.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		m_nDialogMode = MODE_DONE;
		return FALSE;
	}


    return TRUE;
}


//*************************************************************************************
//*************************************************************************************
BOOL CWorkstationDefinition::PopulateWorkstationDialogFromDatabase ( ATP02 Workstation)
{
	
    pDescriptionPage->m_WorkstationIdStr = (LPSTR)Workstation.primary_key.workstation_id;
    if( strcmp( (char *)Workstation.workstation_status, "A" ) == 0 )
	   pDescriptionPage->m_StrWorkstationStatus = "Active";
    else
	   pDescriptionPage->m_StrWorkstationStatus = "Inactive";

	pDescriptionPage->UpdateData(FALSE);

   if( m_nDialogMode == MODE_VIEW )
	   pDescriptionPage->EnableWorkstationDescriptionDialog(FALSE );
   else
	   pDescriptionPage->EnableWorkstationDescriptionDialog(TRUE );
    
    return TRUE;
}


//*************************************************************************************
//*************************************************************************************
void CWorkstationDefinition::OnCancel() 
{
	// TODO: Add your specialized code here and/or call the base class
   m_WorkstationIdStr = "";
	if ( pDescriptionPage->m_bDescription)
		pDescriptionPage->CleanWorkstationDescriptionDialog();

   m_nDialogMode = MODE_DONE;
   UpdateData( FALSE );
	
	CPropertyPage::OnCancel();
}




//*************************************************************************************
//*************************************************************************************
BOOL CWorkstationDefinition::OnApply() 
{
	char strErrorMessage[200];
	
	if( m_nDialogMode == MODE_ADD )
   {
      int rcode = txdsapi_insert_record ((pBYTE)&pDescriptionPage->atp02, sizeof (ATP02), ATP02_DATA,strErrorMessage);
      if (rcode != PTEMSG_OK)
      {   	
	      m_Message = strErrorMessage;
	      if (!UpdateData(FALSE))
		      AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		

      }
      else 
      {            
         memset ( m_strPreviousData, 0, sizeof (m_strPreviousData));
         memset ( m_strCurrentData, 0, sizeof (m_strCurrentData));
         memset ( m_strFieldTag, 0, sizeof (m_strFieldTag));
			memcpy ((LPSTR)m_strCurrentData, "Record Added", sizeof (m_strCurrentData));
			PopulateHistoryTable();
	      m_Message = "Record Added";	
	      m_WorkstationIdCtrl.SetFocus();
	      m_WorkstationIdCtrl.EnableWindow(TRUE);
	      
         if (!UpdateData(FALSE))
		      AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		

         pDescriptionPage->CleanWorkstationDescriptionDialog();
	      pDescriptionPage->EnableWorkstationDescriptionDialog(FALSE);
	   
	      SetModified ( FALSE );
	      m_nDialogMode = MODE_DONE;
      }
   }
	else if( m_nDialogMode == MODE_EDIT )
   {
      int rcode = txdsapi_update_record ((pBYTE)&pDescriptionPage->atp02,sizeof(ATP02), ATP02_DATA, strErrorMessage);
      if (rcode != PTEMSG_OK)
      {   	
	      m_Message = strErrorMessage;
	      if (!UpdateData(FALSE))
		      AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		

      }
      else 
      {            
         if ( strcmp ( strStatus, (char*)&pDescriptionPage->atp02.workstation_status ) != 0 )
         {
            memset ( m_strPreviousData, 0, sizeof (m_strPreviousData));
            memset ( m_strCurrentData, 0, sizeof (m_strCurrentData));
            memset ( m_strFieldTag, 0, sizeof (m_strFieldTag));
            if ( strStatus == "A" )
               memcpy ( m_strPreviousData,  "Active", 6);
            else if ( strStatus == "I" )
               memcpy ( m_strPreviousData,  "Inactive", 8);

            memcpy ( m_strFieldTag,  "Status", sizeof (m_strFieldTag));

            if ( 0 == strcmp( (char *)pDescriptionPage->atp02.workstation_status, "A" ))
               memcpy ( (char *)&m_strCurrentData,  "Active", 6);
            else if ( 0 == strcmp( (char *)pDescriptionPage->atp02.workstation_status, "I" ))
               memcpy ( (char *)&m_strCurrentData,  "Inactive", 8);
            PopulateHistoryTable();
         }
	      m_Message = "Record Updated";	
	      m_WorkstationIdCtrl.SetFocus();
	      m_WorkstationIdCtrl.EnableWindow(TRUE);


	      if (!UpdateData(FALSE))
		      AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		

         pDescriptionPage->CleanWorkstationDescriptionDialog();
	      pDescriptionPage->EnableWorkstationDescriptionDialog(FALSE);
	   
		   if ( pChangeHistoryPage->m_bChangeHistory )
			   pChangeHistoryPage->CleanChangeHistory();
	      SetModified ( FALSE );
	      m_nDialogMode = MODE_DONE;
      }
   }

	m_WorkstationIdStr.Empty();

   ZeroMemory( &pDescriptionPage->atp02, sizeof( ATP02 ) );
	((CPropertySheet*) this->GetParent())->SetActivePage(0);
	return CPropertyPage::OnApply();
}

void CWorkstationDefinition::PopulateHistoryTable()
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
    memcpy ((LPTSTR) sHistoryData.file_name,     (LPCTSTR)"ATP02", sizeof (sHistoryData.file_name));
    temp = strDate;
    temp = temp.Right(4) + temp.Left(2) + temp.Mid(2,2);
    

    wsize = sizeof( wstation );
    GetComputerName( wstation, &wsize );

	memcpy ((LPTSTR)sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
    memcpy ((LPTSTR) sHistoryData.date_modified, temp, sizeof (sHistoryData.date_modified));
    memcpy ((LPTSTR) sHistoryData.file_primary_key,  (LPCTSTR)m_WorkstationIdStr, sizeof (sHistoryData.file_primary_key));
    memcpy ((LPTSTR) sHistoryData.primary_key.unique_id,     (LPCTSTR)dest_str, sizeof (sHistoryData.primary_key.unique_id));        
    memcpy ((LPTSTR) sHistoryData.time_modified,     (LPCTSTR)strTime, sizeof (sHistoryData.time_modified));              
    memcpy ((LPTSTR)sHistoryData.current_data, (LPCTSTR)m_strCurrentData, sizeof (sHistoryData.current_data));
	 memcpy ((LPTSTR) sHistoryData.field_tag,     (LPCTSTR)m_strFieldTag, sizeof (sHistoryData.field_tag));
    memcpy ((LPTSTR)sHistoryData.changed_by,(LPTSTR)ucf01.primary_key.name, sizeof(sHistoryData.changed_by));
	 memcpy ((LPTSTR) sHistoryData.previous_data,  m_strPreviousData, sizeof (sHistoryData.previous_data));	
 
	
    int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
		if ( rcode != PTEMSG_OK )
		{
			m_Message = strErrorMsg;
			UpdateData( FALSE );
		}
    
}

BOOL CWorkstationDefinition::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}
