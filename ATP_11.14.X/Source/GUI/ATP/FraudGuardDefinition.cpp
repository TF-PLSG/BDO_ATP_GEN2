/*********************************************************************************
* Module:				FruadGuardDefinition.cpp
*	
* Title:				Fruad Guard Configuration Definition
*
* Description:			GUI Interface to the Fraud Guard Configuration data: FG01 Table
*
* Application:			ATP
*
* Author:				LMitchell July 2005
*
* Revision History:
*  
* $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\ATP_gui\FraudGuardDefinition.cpp  $
   
      Rev 1.2   Sep 12 2005 11:00:54   dirby
   Changed to make Product field optional.  Was mandatory.
   SCR 16789
   
      Rev 1.1   Jul 15 2005 15:54:26   dirby
   Changed default TYPE from 'A' to "ACQUIRER".
   Correct some of the messages that are displayed if required fields
   are empty.
   SCR 14421
   
      Rev 1.0   Jul 07 2005 15:54:22   lmitchel
   Initial revision.
   
   
*
************************************************************************************/
// FraudGuardDefinition.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "FraudGuardDefinition.h"
#include "ChangeHistory.h"
#include "FraudGuardGeneral.h"
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

extern UCF01 ucf01;
extern CString name;
extern CSharedMem shrmem;
/////////////////////////////////////////////////////////////////////////////
// CFraudGuardDefinition property page

IMPLEMENT_DYNCREATE(CFraudGuardDefinition, CPropertyPage)

CFraudGuardDefinition::CFraudGuardDefinition() : CPropertyPage(CFraudGuardDefinition::IDD)
{
	//{{AFX_DATA_INIT(CFraudGuardDefinition)
	m_StrNameId = _T("");
	m_StrType = _T("");
	m_StrMessageLbl = _T("");
	//}}AFX_DATA_INIT
}

CFraudGuardDefinition::~CFraudGuardDefinition()
{
}

void CFraudGuardDefinition::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFraudGuardDefinition)
	DDX_Control(pDX, IDC_MESSAGE_LBL, m_CntlMessageLbl);
	DDX_Control(pDX, IDC_FGUARD_TYPE_FLD, m_CntlType);
	DDX_Control(pDX, IDC_FGUARD_ID_FLD, m_CntlNameId);
	DDX_Control(pDX, IDC_FGUARD_VIEW_BUTTON, m_ViewButton);
	DDX_Control(pDX, IDC_FGUARD_UPDATE_BUTTON, m_UpdateButton);
	DDX_Control(pDX, IDC_FGUARD_LIST_BUTTON, m_ListButton);
	DDX_Control(pDX, IDC_FGUARD_DELETE_BUTTON, m_DeleteButton);
	DDX_Control(pDX, IDC_FGUARD_ADD_BUTTON, m_AddButton);
	DDX_Text(pDX, IDC_FGUARD_ID_FLD, m_StrNameId);
	DDX_Text(pDX, IDC_FGUARD_TYPE_FLD, m_StrType);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFraudGuardDefinition, CPropertyPage)
	//{{AFX_MSG_MAP(CFraudGuardDefinition)
	ON_BN_CLICKED(IDC_FGUARD_ADD_BUTTON, OnFguardAddButton)
	ON_BN_CLICKED(IDC_FGUARD_DELETE_BUTTON, OnFguardDeleteButton)
	ON_BN_CLICKED(IDC_FGUARD_LIST_BUTTON, OnFguardListButton)
	ON_BN_CLICKED(IDC_FGUARD_UPDATE_BUTTON, OnFguardUpdateButton)
	ON_BN_CLICKED(IDC_FGUARD_VIEW_BUTTON, OnFguardViewButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFraudGuardDefinition message handlers

void CFraudGuardDefinition::OnFguardAddButton() 
{
   FG01	 sFGuard;
   CMyUtils myUtilsObject;
   char	 strErrorMessage [200]="";

   m_StrMessageLbl = "";

   m_nDialogMode = MODE_ADD;

   // verify name field is not blank
   if (!CheckDialogForCompleteness())
      return ;

   // set the mode on each page to mode ADD
   pGeneralPage->m_nDialogMode = MODE_ADD;
   pChangeHistoryPage->m_nDialogMode = MODE_ADD;

   //copy the Name ID data from control to FG01 table structure
   ZeroMemory (&sFGuard, sizeof (FG01));
   memcpy ((LPSTR)(sFGuard.name), m_StrNameId ,sizeof (sFGuard.name));
   memcpy ((LPSTR)(sFGuard.type), m_StrType, 1 );

   //verify fg01 name does not already exists in FG01 database 
   int rcode = txdsapi_get_record((pBYTE)&sFGuard, sizeof (FG01), FG01_DATA, strErrorMessage);

   if(rcode == PTEMSG_OK )
   {
      m_StrMessageLbl = "ERROR: Fraud Guard name already exists";
      m_nDialogMode = MODE_DONE;

      if (!UpdateData(FALSE))
      {
         AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);
      }
      return ;
   }

   pChangeHistoryPage->bNext = FALSE;      

   ((CPropertySheet*) this->GetParent())->SetWindowText("Add Fraud Guard Definition");
   //set version info in the title bar
	CString appProdVer;
	CString lpszTitle;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
	lpszTitle = lpszTitle + " - Version - ";
	lpszTitle += appProdVer;
	((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);

   m_CntlNameId.EnableWindow(FALSE);
   m_CntlType.EnableWindow(FALSE);   

   if ( pChangeHistoryPage->m_bChangeHistory)
   {  
      pChangeHistoryPage->UpdateData(FALSE); 
      pChangeHistoryPage->EnableWindow(TRUE);
      pChangeHistoryPage->CleanChangeHistory();
   }   

   //enable controls on the General page
   if ( pGeneralPage->m_bGeneral )
   {
      pGeneralPage->CleanGeneralDialog();
      pGeneralPage->EnableGeneralDialog(TRUE);
   }

   pGeneralPage->m_StrNameId  = m_StrNameId;
   pGeneralPage->m_StrTypeFld = m_StrType;

   ((CPropertySheet*) this->GetParent())->SetActivePage(1);

   pGeneralPage->UpdateData(FALSE);
}

void CFraudGuardDefinition::OnFguardDeleteButton() 
{
   FG01     sFGuard;
   CMyUtils myUtilsObject;
   char     strErrorMessage [200]="";
   int      nReturnCode = 0;
   CString  strMessage; 

    m_nDialogMode = MODE_DONE;

	m_StrMessageLbl = "";

	if (!CheckDialogForCompleteness())
        return;   
   
    if (!UpdateData(TRUE))
    {
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);	
	}
    
	strMessage.Format ("Are you sure you want to delete Fraud Guard record %s?", (LPCTSTR) m_StrNameId) ;	

	nReturnCode = AfxMessageBox ((LPCTSTR)strMessage,MB_OKCANCEL|MB_ICONQUESTION) ;

	if (nReturnCode != IDOK) 
        return ;

    //copy the Name ID data from control to FG01 table structure
    memcpy ((LPSTR)(sFGuard.name), m_StrNameId ,sizeof (sFGuard.name));
	memcpy ((LPSTR)(sFGuard.type), m_StrType, sizeof(sFGuard.type));
	   
    int rcode = txdsapi_delete_record ((pBYTE)&sFGuard, sizeof (FG01), FG01_DATA, strErrorMessage);

    if(rcode != PTEMSG_OK )    
    {
		m_StrMessageLbl = strErrorMessage;
        m_nDialogMode = MODE_DONE;
     
		if (!UpdateData(FALSE))
        {
		    AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		    return ;
	}
    else
    {
         memcpy ((LPSTR)m_strCurrentData, "Record Deleted", sizeof (m_strCurrentData));

         PopulateHistoryTable();
         m_StrMessageLbl = "Record Deleted";
         m_StrNameId = _T("");        

		 if (!UpdateData(FALSE))
         {
			 AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		 }
		
   
		 if ( pGeneralPage->m_bGeneral )
		 {
			 pGeneralPage->CleanGeneralDialog();
			 pGeneralPage->EnableGeneralDialog(FALSE);
		 } 
       
		 if ( pChangeHistoryPage->m_bChangeHistory )
			  pChangeHistoryPage->CleanChangeHistory();

		 ((CPropertySheet*) this->GetParent())->SetActivePage(0);
		 SetModified ( FALSE );    

		m_CntlNameId.EnableWindow(TRUE);
		m_CntlType.EnableWindow(TRUE);   

    } 
	
}

void CFraudGuardDefinition::OnFguardListButton() 
{
    CNameList listDlg;   
    int nReturnCode;

	m_StrMessageLbl = "";
	UpdateData(TRUE);

    listDlg.m_nIndicator = MODE_FGUARD;
    listDlg.strPrimaryKey1 = m_StrNameId;
	listDlg.strNetworkType = m_StrType;

   
    nReturnCode = listDlg.DoModal();

    if ( nReturnCode == IDOK)
        m_StrNameId = name;
   
	if ( pGeneralPage->m_bGeneral )	
	{
			pGeneralPage->CleanGeneralDialog();
			pGeneralPage->EnableGeneralDialog(FALSE);
	}
        
	if ( pChangeHistoryPage->m_bChangeHistory )
		pChangeHistoryPage->CleanChangeHistory();

	m_CntlNameId.EnableWindow(TRUE);
	m_CntlType.EnableWindow(TRUE);
     
    if (!UpdateData(FALSE))
    {
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}	
}
	

void CFraudGuardDefinition::OnFguardUpdateButton() 
{
    HST01	sHistoryRecord;
	FG01	sFGuard;
	char	strErrorMessage[200];
    
    m_nDialogMode = MODE_EDIT;
   // set the mode on each page to mode ADD
	pGeneralPage->m_nDialogMode = MODE_EDIT;
    pChangeHistoryPage->m_nDialogMode = MODE_EDIT;
	m_StrMessageLbl = "";

    if (!CheckDialogForCompleteness())
        return;
    
     //copy the Name ID data from control to FG01 table structure
    memcpy ((LPSTR)(sFGuard.name), m_StrNameId ,sizeof (sFGuard.name));
	memcpy ((LPSTR)(sFGuard.type), m_StrType, sizeof(sFGuard.type));

    int rcode = txdsapi_get_record((pBYTE)&sFGuard, sizeof (FG01), FG01_DATA, strErrorMessage);

	if (rcode != PTEMSG_OK)
	{        
       m_StrMessageLbl = strErrorMessage;
       m_nDialogMode = MODE_DONE;

	   if (!UpdateData(FALSE))
       {
		   AfxMessageBox (IDS_UPDATEDATAFAILED, MB_ICONSTOP);		
	   }
           return;
    }


    ZeroMemory (&sHistoryRecord, sizeof (sHistoryRecord));

    memset ( strFilePrimaryKey,0, sizeof (strFilePrimaryKey));

    strcpy ( strFilePrimaryKey, m_StrNameId);
    strcat ( strFilePrimaryKey, m_StrType);  

    memcpy ((LPSTR)(sHistoryRecord.primary_key.unique_id),"                 ",sizeof (sHistoryRecord.primary_key.unique_id));
    memcpy ((LPSTR)(sHistoryRecord.file_name),"FG01",	sizeof (sHistoryRecord.file_name));
    memcpy ((LPSTR)(sHistoryRecord.file_primary_key), strFilePrimaryKey,	sizeof (sHistoryRecord.file_primary_key));
    memcpy ( &(pChangeHistoryPage->sHistoryRecord),&sHistoryRecord,sizeof(sHistoryRecord));
    
	((CPropertySheet*) this->GetParent())->SetActivePage(1);       
    
	pChangeHistoryPage->bNext = FALSE;

	((CPropertySheet*) this->GetParent())->SetWindowText("Update Fraud Guard Record");
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

	pGeneralPage->PopulateFraudGuardDialogFromDatabase ( sFGuard );
    
	m_CntlNameId.EnableWindow(FALSE);
    m_CntlType.EnableWindow(FALSE);



    pGeneralPage->UpdateData(FALSE); 
    pGeneralPage->EnableGeneralDialog(TRUE); 
	pGeneralPage->SetModified(TRUE);
  


	((CPropertySheet*) this->GetParent())->SetActivePage(1); 
}
	

void CFraudGuardDefinition::OnFguardViewButton() 
{
	FG01	sFGuard;
    HST01   sHistoryRecord;
	char	strErrorMessage[200];
    
    m_nDialogMode = MODE_VIEW;
	pGeneralPage->m_nDialogMode = MODE_VIEW;
    pChangeHistoryPage->m_nDialogMode = MODE_VIEW;
//	pChangeHistoryPage->bNext = FALSE;

	m_StrMessageLbl = "";

    if (!CheckDialogForCompleteness())
        return;
    
     //copy the Name ID data from control to FG01 table structure
    memcpy ((LPSTR)(sFGuard.name), m_StrNameId ,sizeof (sFGuard.name));
	memcpy ((LPSTR)(sFGuard.type), m_StrType, sizeof(sFGuard.type));

    int rcode = txdsapi_get_record((pBYTE)&sFGuard, sizeof (FG01), FG01_DATA, strErrorMessage);

	if (rcode != PTEMSG_OK)
	{        
       m_StrMessageLbl = strErrorMessage;
       m_nDialogMode = MODE_DONE;

	   if (!UpdateData(FALSE))
       {
		   AfxMessageBox (IDS_UPDATEDATAFAILED, MB_ICONSTOP);		
	   }
           return;
    }

	//pChangeHistoryPage->bNext = FALSE;

	if ( pChangeHistoryPage->m_bChangeHistory )
		pChangeHistoryPage->PopulateHistoryDialog();

	pGeneralPage->PopulateFraudGuardDialogFromDatabase ( sFGuard );
    
	m_CntlNameId.EnableWindow(TRUE);
    m_CntlType.EnableWindow(TRUE);

	pGeneralPage->SetModified(FALSE);

	if ( pGeneralPage->m_bGeneral)
    {   
 	    pGeneralPage->UpdateData(FALSE); 
        pGeneralPage->EnableGeneralDialog(FALSE);       
    }	  


	// get history data
    ZeroMemory (&sHistoryRecord, sizeof (sHistoryRecord));

	memset ( strFilePrimaryKey, 0, sizeof (strFilePrimaryKey));
	strcpy ( strFilePrimaryKey, (char *)sFGuard.name);
	strcat ( strFilePrimaryKey, (char *)sFGuard.type);

    memcpy ((LPSTR)(sHistoryRecord.primary_key.unique_id), "                 ", sizeof (sHistoryRecord.primary_key.unique_id));
    memcpy ((LPSTR)(sHistoryRecord.file_name), "FG01", sizeof (sHistoryRecord.file_name));
    memcpy ((LPSTR)(sHistoryRecord.file_primary_key), strFilePrimaryKey, sizeof (sHistoryRecord.file_primary_key));

    memcpy ( &(pChangeHistoryPage->sHistoryRecord), &sHistoryRecord, sizeof(sHistoryRecord));
    
	((CPropertySheet*) this->GetParent())->SetActivePage(1);    

	// set dialog mode for all pages

	pChangeHistoryPage->bNext = FALSE;

	((CPropertySheet*) this->GetParent())->SetWindowText("View Fraud Guard Definition");
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

	pGeneralPage->PopulateFraudGuardDialogFromDatabase ( sFGuard );

	pGeneralPage->SetModified ( FALSE );	

	m_CntlNameId.EnableWindow(TRUE);
	m_CntlType.EnableWindow(TRUE);
	m_StrMessageLbl.Empty();

	UpdateData(FALSE);


	((CPropertySheet*) this->GetParent())->SetActivePage(1); 
	
}

BOOL CFraudGuardDefinition::OnInitDialog() 
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

   m_StrNameId = _T("");
   m_StrMessageLbl = _T("");
   m_StrType = _T("ACQUIRER"); /*default value*/

   CString temp;
   int nIndex = -1;

   for ( int i = 0; i <=15; i++ )
   {
      temp = ucf01.control[i].subsystem_name;
      if ( strcmp ( temp, "Network Control" ) == 0)
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

   m_nDialogMode = MODE_DONE;	

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

/******************************************************************************************/

BOOL CFraudGuardDefinition::CheckDialogForCompleteness()
{
    CMyUtils myUtilsObject;

    if (!UpdateData(TRUE))
	{
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}

    if (myUtilsObject.IsEmpty (m_StrNameId, this))
    {
		m_StrMessageLbl = "Please enter Name ID";	
		m_CntlNameId.SetFocus();
		if (!UpdateData(FALSE))
		{
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		m_nDialogMode = MODE_DONE;
		return FALSE;
	}	
   
	if (myUtilsObject.IsEmpty (m_StrType, this))
    {
		m_StrMessageLbl = "Please enter Type - ACQUIRER or ISSUER";	
		m_CntlType.SetFocus();
		if (!UpdateData(FALSE))
		{
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		m_nDialogMode = MODE_DONE;
		return FALSE;
	}	
    return TRUE;
}

/******************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////


BOOL CFraudGuardDefinition::OnApply() 
{
	FG01 sFGuardStruture;
 
	char strErrorMessage[200]="";
	CMyUtils myUtilsObject;
    CString buttonlabel;
    CWnd*   activebutton;

   /* This snippet of code prevents updating the database
    * and exiting the window when <enter> is pressed. The
    * database will only get updated when APPLY is selected
    * or if focus is on the OK button when <enter> is pressed
    * or OK is selected.
    */
   activebutton = GetFocus();
   activebutton->GetWindowText(buttonlabel);
   if ( buttonlabel != "&Apply"  && buttonlabel != "OK")
   {
      return FALSE;
   }


	if (m_nDialogMode != MODE_DONE)
	{
		if ( myUtilsObject.IsEmpty (pGeneralPage->m_StrNameId, this)) 
			{
				((CPropertySheet*) this->GetParent())->SetActivePage(1);
				pGeneralPage->m_CntlNameId.SetFocus();
				pGeneralPage->m_StrMessageLbl = "Please enter Name";
				pGeneralPage->UpdateData(FALSE);
				return FALSE;
			}

		if ( myUtilsObject.IsEmpty (pGeneralPage->m_StrApplicationCombo, this)) 
			{
				((CPropertySheet*) this->GetParent())->SetActivePage(1);
				pGeneralPage->m_CntlApplicationCombo.SetFocus();
				pGeneralPage->m_StrMessageLbl = "Please select an Application";
				pGeneralPage->UpdateData(FALSE);
				return FALSE;
			}

		if ( myUtilsObject.IsEmpty (pGeneralPage->m_StrRuleDomainFld, this)) 
			{
				((CPropertySheet*) this->GetParent())->SetActivePage(1);
				pGeneralPage->m_CntlRuleDomainFld.SetFocus();
				pGeneralPage->m_StrMessageLbl = "Please enter Rule Domain";
				pGeneralPage->UpdateData(FALSE);
				return FALSE;
			}

		if ( myUtilsObject.IsEmpty (pGeneralPage->m_StrTpduFld, this)) 
			{
				((CPropertySheet*) this->GetParent())->SetActivePage(1);
				pGeneralPage->m_CntlTpduFld.SetFocus();
				pGeneralPage->m_StrMessageLbl = "Please enter TPDU";
				pGeneralPage->UpdateData(FALSE);
				return FALSE;
			}

		if ( myUtilsObject.IsEmpty (pGeneralPage->m_StrQueNameFld, this)) 
			{
				((CPropertySheet*) this->GetParent())->SetActivePage(1);
				pGeneralPage->m_CntlQueNameFld.SetFocus();
				pGeneralPage->m_StrMessageLbl = "Please enter Queue Name";
				pGeneralPage->UpdateData(FALSE);
				return FALSE;
			}
	}

    if (m_nDialogMode == MODE_EDIT)
	{ 
        memset(&sFGuardStruture,0,sizeof(sFGuardStruture)) ;
       
        pGeneralPage->PopulateDatabaseFromFGuardDialog(&sFGuardStruture);        
	     		
	    int rcode = txdsapi_update_record((pBYTE)&sFGuardStruture, sizeof (FG01), FG01_DATA, strErrorMessage); 
		if (rcode != PTEMSG_OK)
		{    	
			m_StrMessageLbl = strErrorMessage;
            m_nDialogMode = MODE_DONE;

            ((CPropertySheet*) this->GetParent())->SetActivePage(0);
            ((CPropertySheet*) this->GetParent())->SetWindowText(" Fraud Guard Definition");
			//set version info in the title bar
			CString appProdVer;
			CString lpszTitle;
			appProdVer.LoadString(IDS_PRODUCT_VERSION);
			((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
			lpszTitle = lpszTitle + " - Version - ";
			lpszTitle += appProdVer;
			((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);

		    m_CntlNameId.EnableWindow(TRUE);

			if (!UpdateData(FALSE))
            {
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}
			    return FALSE;
		}
        else
		{
			m_StrMessageLbl = "Record Updated";
			if (!UpdateData(FALSE))
            {
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

            pChangeHistoryPage->m_nDialogMode = MODE_DONE;
			pGeneralPage->m_nDialogMode   = MODE_DONE;
 
			((CPropertySheet*) this->GetParent())->SetActivePage(0);
            ((CPropertySheet*) this->GetParent())->SetWindowText("Fraud Guard Definition");		
			//set version info in the title bar
			CString appProdVer;
			CString lpszTitle;
			appProdVer.LoadString(IDS_PRODUCT_VERSION);
			((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
			lpszTitle = lpszTitle + " - Version - ";
			lpszTitle += appProdVer;
			((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);
			
			m_CntlNameId.EnableWindow(TRUE);
			m_CntlType.EnableWindow(TRUE);
           	SetModified ( FALSE );
            m_nDialogMode = MODE_DONE;			
			//return CPropertyPage::OnApply();
            return FALSE;
        }    
            
    }
	if (m_nDialogMode == MODE_ADD)
    {
        memset(&sFGuardStruture,0,sizeof(sFGuardStruture)) ;
        
        if (!(pGeneralPage->PopulateDatabaseFromFGuardDialog(&sFGuardStruture)))
        {		
			return FALSE;
		}             

		int rcode = txdsapi_insert_record((pBYTE)&sFGuardStruture, sizeof (FG01), FG01_DATA, strErrorMessage);  
             
        if (rcode != PTEMSG_OK)
		{   	
			m_StrMessageLbl = strErrorMessage;
            m_nDialogMode = MODE_DONE;

			if (!UpdateData(FALSE))
            {
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);                
			}
            ((CPropertySheet*) this->GetParent())->SetActivePage(0);
			return FALSE;                       
        }
        else 
		{            
			memcpy ((LPSTR)m_strCurrentData,"Record Added",sizeof (m_strCurrentData));
		
			PopulateHistoryTable();

		    ((CPropertySheet*) this->GetParent())->SetActivePage(0);                
		
			m_StrMessageLbl = "Record Added";	
			
			if (!UpdateData(FALSE))
            {
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}

           if ( pGeneralPage->m_bGeneral )
			{
				pGeneralPage->CleanGeneralDialog();
				pGeneralPage->EnableGeneralDialog(FALSE);				
			}			
     
     	
            if ( pChangeHistoryPage->m_bChangeHistory )
				pChangeHistoryPage->CleanChangeHistory();
			
            pChangeHistoryPage->m_nDialogMode = MODE_DONE;
			pGeneralPage->m_nDialogMode       = MODE_DONE;
            m_CntlNameId.SetFocus();
			m_CntlNameId.EnableWindow(TRUE);
			m_CntlType.EnableWindow(TRUE);
            SetModified ( FALSE );
            m_nDialogMode = MODE_DONE;
            //return CPropertyPage::OnApply();
            return FALSE;
        }          	         
     }
     m_nDialogMode = MODE_DONE;		
     //return CPropertyPage::OnApply();
     return FALSE;	
	return CPropertyPage::OnApply();
}

void CFraudGuardDefinition::PopulateHistoryTable()
{
    HST01 sHistoryData;
    CHAR dest_str[18];   
	CMyUtils myUtilsObject;
	char strErrorMsg [200] = "";
    CString temp;

    char strDate[9] = "";
    char strTime[7] = "";
    char strPrimaryKey[50] = "";
    CHAR wstation[21]; DWORD wsize;

    memset ( strDate, 0, sizeof (strDate));
    memset ( strTime, 0, sizeof (strTime));
    memset ( strPrimaryKey, 0, sizeof (strPrimaryKey));

    strcpy ( strPrimaryKey, m_StrNameId);
    strcat ( strPrimaryKey, m_StrType);
    
    myUtilsObject.GetSystemDate(strDate); 
    myUtilsObject.GetSystemTime(strTime);

    pintime_get_gmt ( dest_str );

    ZeroMemory ( &sHistoryData, sizeof (HST01));
    memcpy ((LPTSTR) sHistoryData.file_name,         (LPCTSTR)"FG01", sizeof (sHistoryData.file_name));
    temp = strDate;
    temp = temp.Right(4) + temp.Left(2) + temp.Mid(2,2);
    

    wsize = sizeof( wstation );
    GetComputerName( wstation, &wsize );

	memcpy ((LPTSTR) sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
    memcpy ((LPTSTR) sHistoryData.date_modified, temp, sizeof (sHistoryData.date_modified));
    memcpy ((LPTSTR) sHistoryData.file_primary_key,  (LPCTSTR)strPrimaryKey, sizeof (sHistoryData.file_primary_key));
    memcpy ((LPTSTR) sHistoryData.primary_key.unique_id,     (LPCTSTR)dest_str, sizeof (sHistoryData.primary_key.unique_id));        
    memcpy ((LPTSTR) sHistoryData.time_modified,     (LPCTSTR)strTime, sizeof (sHistoryData.time_modified)); 
             
    memcpy ((LPTSTR)sHistoryData.current_data, (LPCTSTR)m_strCurrentData, sizeof (sHistoryData.current_data));
	memcpy ((LPTSTR)sHistoryData.changed_by, (LPCTSTR)ucf01.primary_key.name, sizeof (sHistoryData.changed_by));
        
    int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
	if ( rcode != PTEMSG_OK )
	{
		m_StrMessageLbl = strErrorMsg;
		m_nDialogMode = MODE_DONE;
		UpdateData( FALSE );
	}
    
}

BOOL CFraudGuardDefinition::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}
